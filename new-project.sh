#!/bin/bash

R=`pwd`
VCUBE_DIR=$(dirname `realpath $0`)
source "$VCUBE_DIR/install.sh"

# ---------------------------------------------------------------------------------------------------------------------

function input_project_name()
{
    while : ; do
        read -p 'Enter name of new project: ' I_PROJECT_NAME

        if echo "$I_PROJECT_NAME" | grep -P '[^0-9A-Za-z_.\-]' >/dev/null; then
            echo 'Only alphanumeric characters and _.- are allowed!'
        elif test -n "$I_PROJECT_NAME"; then
            break
        fi
    done
}

function input_target_file()
{
    cd "$1"
    while : ; do
        read -p 'Select target (Use TAB to list available options): ' -e I_TARGET_FILE

        if ! test -f "$I_TARGET_FILE"; then
            echo "Not found!"
        else
            break
        fi
    done
    cd - >/dev/null
}

# ---------------------------------------------------------------------------------------------------------------------

DIR_GENERATED="$R/generated"
NEW_PROJ_SCRIPT="$DIR_OUTPUT/new-project.script"

input_project_name
input_target_file "$DIR_VC_TARGETS"

TARGET=`cat "$DIR_VC_TARGETS/$I_TARGET_FILE"`
echo "Project name: $I_PROJECT_NAME"
echo "Target: $TARGET"

cat <<EOT > "$NEW_PROJ_SCRIPT"
load $TARGET
project name $I_PROJECT_NAME
project path $DIR_GENERATED
project toolchain "Makefile"
project generate
exit
EOT

rm -Rf "$DIR_GENERATED" "$DIR_OUTPUT/.generated" "$DIR_OUTPUT/.injected" "$DIR_OUTPUT/generate.script"
mkdir "$DIR_GENERATED"
$PATH_CUBE_MX -q "$NEW_PROJ_SCRIPT"
mv $DIR_GENERATED/$I_PROJECT_NAME/* "$DIR_GENERATED" && rm -Rf "$DIR_GENERATED/$I_PROJECT_NAME"
touch "$DIR_OUTPUT/.generated"
