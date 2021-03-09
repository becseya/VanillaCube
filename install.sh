#!/bin/bash

function fatal_error()
{
    echo "$1"
    exit 1
}

test -z "$PATH_CUBE_MX" &&
{
    PATH_CUBE_MX="$HOME/.bin/STM32CubeMX"
}

test -z "$PATH_CUBE_PROG" &&
{
    PATH_CUBE_PROG="$HOME/.bin/STM32_Programmer_CLI"
}

R=`pwd`
VCUBE_DIR=$(dirname `realpath $0`)

# TODO: test env paths

# install folder
VC_INSTALL="$R/build/vcube-install"
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

cp -RT "$VCUBE_DIR/skeleton" "$R"
