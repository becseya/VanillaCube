#!/bin/bash

R=`pwd`
VCUBE_DIR=$(dirname `realpath $0`)
source "$VCUBE_DIR/env.sh"

# ---------------------------------------------------------------------------------------------------------------------

function add_target
{
    local TARGET="$1"
    local PACKAGE="$2"

    # remove 'STR32' prefix and make lowercase
    local TARGET_NAME=`echo "${TARGET:5}" | tr '[:upper:]' '[:lower:]'`

    # try to shorten name
    local FILE_NAME="${TARGET_NAME::-2}-$PACKAGE"
    test -f "$DIR_VC_TARGETS/$FILE_NAME" &&
        FILE_NAME="${TARGET_NAME}-$PACKAGE"

    echo "$TARGET" > "$DIR_VC_TARGETS/$FILE_NAME"
}

function explode_variants
{
    local TARGET="$1"
    local PACKAGE="$2"

    # prepare template and variant list
    local NAME_TEMPLATE=`echo "$TARGET" | sed -e 's+[(][^)]*[)]+@+'`
    local VARIANTS=`echo "$TARGET" | grep -Po '\([^\)]+\)' | tr -d '(),'`
    VARIANTS="${VARIANTS//-}"

    # make variants
    for (( i=0; i<${#VARIANTS}; i++ )); do
        TARGET=`echo "$NAME_TEMPLATE" | sed -e "s+@+${VARIANTS:$i:1}+"`
        add_target "$TARGET" "$PACKAGE"
    done
}

function process_xml
{
    local XML_FILE="$1"

    local TARGET=`basename "$XML_FILE" .xml`
    local PACKAGE=`cat "$XML_FILE" | grep -Po '(?<=Package=")[A-Z]+' | tr '[:upper:]' '[:lower:]'`

    if echo "$TARGET" | grep '(' > /dev/null; then
        explode_variants "$TARGET" "$PACKAGE"
    else
        add_target "$TARGET" "$PACKAGE"
    fi
}

function copy_or_link
{
    local CMD="$1"
    local FILE_NAME="$2"

    ! test -f "$R/$FILE_NAME" && ! test -L "$R/$FILE_NAME" &&
        $CMD "$VCUBE_DIR/$FILE_NAME" "$R/$FILE_NAME"
}

# ---------------------------------------------------------------------------------------------------------------------

# install folder
DIR_OUTPUT="$R/build"
VC_INSTALL="$DIR_OUTPUT/vcube-install"
! test -d "$VC_INSTALL" &&
    mkdir -p "$VC_INSTALL"

# env for makefile
MK_ENV="$VC_INSTALL/env.mk"
! test -f "$MK_ENV"
{
    echo "PATH_VCUBE = $VCUBE_DIR" > "$MK_ENV"
    echo "PATH_CUBE_MX = $PATH_CUBE_MX" >> "$MK_ENV"
    echo "PATH_CUBE_PROG = $PATH_CUBE_PROG" >> "$MK_ENV"
}

# mcu targets
DIR_VC_TARGETS="$VC_INSTALL/targets"
! test -d "$DIR_VC_TARGETS" &&
{
    echo "Installing targets..."
    mkdir -p "$DIR_VC_TARGETS"

    for f in $PATH_CUBE_MX_FOLDER/db/mcu/STM32*.xml; do
        process_xml "$f"
    done
}

copy_or_link "cp" ".gitignore"
copy_or_link "cp" ".clang-format"
copy_or_link "ln -s" "Makefile"

# hello world
DIR_SRC="$R/src"
! test -d "$DIR_SRC" &&
{
    mkdir -p "$DIR_SRC"
    cp "$VCUBE_DIR/examples/blink_led/main.cpp" "$DIR_SRC"
}

exit 0
