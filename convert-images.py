#!/usr/bin/env python3

import os
import sys
import math
from glob import glob
from PIL import Image

if len(sys.argv) < 2:
    print("Missing argument")
    exit(-1)

sourceFolder = sys.argv[1]

if not os.path.isdir(sourceFolder):
    print("Path must be a dir")
    exit(-1)

# ---------------------------------------------------------------------------------------------------------------------

def processBitmap(path):
    image = Image.open(path).convert('RGB')

    width, height = image.size
    columns = int((width + 7) / 8)

    data = []
    for x in range(columns):
        data.append([0 for y in range(height)])

    for y in range(height):
        for x in range(width):
            if image.getpixel((x, y)) == (0, 0, 0):
                data[int(x / 8)][y] |= (1 << (7-(x % 8)))

    output = []
    for y in range(height):
        line = []
        for x in range(columns):
            line.append("0x%02x," % data[x][y])
        output.append(" ".join(line))

    return (width, height, " ".join(output))

# ---------------------------------------------------------------------------------------------------------------------

hpp_header = (
    "#pragma once\n"
    "\n"
    "#include <graphics/{}.hpp>\n"
    "\n"
    "// clang-format off\n"
    "\n"
    "namespace {} {{\n"
    "\n"
)

hpp_body = (
    "extern const VanillaCube::Graphics::{} {};\n"
)

cpp_header = (
    "#include \"{}.hpp\"\n"
    "\n"
    "using namespace VanillaCube::Graphics;\n"
    "\n"
    "// clang-format off\n"
    "\n"
    "namespace {} {{\n"
)

cpp_i_body = (
    "\n"
    "const {} {} ({}, {}, (const uint8_t[]) {{\n"
    "{}\n"
    "}});\n"
)

footer =  (
    "\n"
    "}\n"
)

# ---------------------------------------------------------------------------------------------------------------------

bitmapFiles = glob(sourceFolder + "/*.bmp")
bitmapFiles.sort()

outputFolder = sourceFolder + "/generated"
if not os.path.exists(outputFolder):
    os.makedirs(outputFolder)

iHpp = open(outputFolder + "/images.hpp", "w")
iCpp = open(outputFolder + "/images.cpp", "w")

iHpp.write(hpp_header.format("bitmap", "img"))
iCpp.write(cpp_header.format("images", "img"))

for bmp in bitmapFiles:
    print("Converting {}...".format(bmp))

    width, height, data_txt = processBitmap(bmp)
    name = os.path.splitext(os.path.basename(bmp))[0]

    iHpp.write(hpp_body.format("Bitmap", name))
    iCpp.write(cpp_i_body.format("Bitmap", name, width, height, "    " + data_txt))

iHpp.write(footer)
iCpp.write(footer)

iHpp.close()
iCpp.close()
