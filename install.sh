#!/bin/bash

R=`pwd`
VCUBE_DIR=$(dirname `realpath $0`)
source "$VCUBE_DIR/env.sh"

# ---------------------------------------------------------------------------------------------------------------------

function process_xml
{
    local XML_FILE="$1"

    local MX_TAG=`basename "$XML_FILE" .xml`
    local MX_PACK=`cat "$XML_FILE" | grep -Po '(?<=Package=")[A-Z]+' | tr '[:upper:]' '[:lower:]'`

    local MX_TAG_SHORT=`echo "${MX_TAG:5}" | tr '-' '.' | tr -d '),' | tr '(' '-' | tr '[:upper:]' '[:lower:]'`

    local TARGET="${MX_TAG_SHORT::-2}_$MX_PACK"
    test -f "$DIR_VC_TARGETS/$TARGET" &&
        TARGET="${MX_TAG_SHORT}_$MX_PACK"

    echo "$MX_TAG" > "$DIR_VC_TARGETS/$TARGET"
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

cp -RT "$VCUBE_DIR/skeleton" "$R"
