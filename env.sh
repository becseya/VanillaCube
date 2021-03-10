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
