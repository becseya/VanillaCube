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
    "#include <graphics/bitmap.hpp>\n"
    "\n"
    "namespace img {\n"
    "\n"
)

hpp_body = (
    "extern const VanillaCube::Graphics::Bitmap {};\n"
)

hpp_footer =  (
    "\n"
    "} // namespace img\n"
)

cpp_header = (
    "#include \"images.hpp\"\n"
    "\n"
    "using namespace VanillaCube::Graphics;\n"
    "\n"
    "namespace img {\n"
    "\n"
    "// clang-format off\n"
)

cpp_body = (
    "\n"
    "const Bitmap {} ({}, {}, (const uint8_t[]) {{\n"
    "{}\n"
    "}});\n"
)

cpp_footer =  (
    "\n"
    "// clang-format on\n"
    "\n"
    "} // namespace img\n"
)

# ---------------------------------------------------------------------------------------------------------------------

bitmapFiles = glob(sourceFolder + "/*.bmp")
bitmapFiles.sort()

outputFolder = sourceFolder + "/generated"
if not os.path.exists(outputFolder):
    os.makedirs(outputFolder)

iHpp = open(outputFolder + "/images.hpp", "w")
iCpp = open(outputFolder + "/images.cpp", "w")

iHpp.write(hpp_header)
iCpp.write(cpp_header)

for bmp in bitmapFiles:
    print("Converting {}...".format(bmp))

    width, height, data_txt = processBitmap(bmp)
    name = os.path.splitext(os.path.basename(bmp))[0]

    iHpp.write(hpp_body.format(name))
    iCpp.write(cpp_body.format(name, width, height, "    " + data_txt))

iHpp.write(hpp_footer)
iCpp.write(cpp_footer)

iHpp.close()
iCpp.close()
