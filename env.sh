#!/bin/bash

function fatal_error()
{
    echo "$1"
    exit 1
}

# ---------------------------------------------------------------------------------------------------------------------

test -z "$PATH_CUBE_MX" &&
{
    PATH_CUBE_MX="$HOME/.bin/STM32CubeMX"
}

test -z "$PATH_CUBE_PROG" &&
{
    PATH_CUBE_PROG="$HOME/.bin/STM32_Programmer_CLI"
}

# TODO: test env paths

test -z "$PATH_CUBE_MX_FOLDER" &&
{
    if [ -L "$PATH_CUBE_MX" ]; then
        PATH_CUBE_MX_REAL=`readlink $PATH_CUBE_MX`
    else
        PATH_CUBE_MX_REAL="$PATH_CUBE_MX"
    fi

    PATH_CUBE_MX_FOLDER=`dirname $PATH_CUBE_MX_REAL`
}
