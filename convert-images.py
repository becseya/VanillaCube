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

fHpp = open(outputFolder + "/images.hpp", "w")
fCpp = open(outputFolder + "/images.cpp", "w")

fHpp.write(hpp_header)
fCpp.write(cpp_header)

for bmp in bitmapFiles:
    print("Converting {}...".format(bmp))

    image = Image.open(bmp)
    rawData = list(image.getdata())

    width, height = image.size

    columns = int((width + 7) / 8)

    data = []
    for x in range(columns):
        data.append([0 for y in range(height)])

    for y in range(height):
        for x in range(width):
            index = x + y * width
            if rawData[index] == 1:
                data[int(x / 8)][y] |= (1 << (7-(x % 8)))

    output = []
    for y in range(height):
        line = []
        for x in range(columns):
            line.append("0x%02x," % data[x][y])
        output.append(" ".join(line))

    name = os.path.splitext(os.path.basename(bmp))[0]

    fHpp.write(hpp_body.format(name))
    fCpp.write(cpp_body.format(name, width, height, "    "+" ".join(output)))

fHpp.write(hpp_footer)
fCpp.write(cpp_footer)

fHpp.close()
fCpp.close()
