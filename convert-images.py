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

if len(sys.argv) >= 3:
    outputFolder = sys.argv[2]
    if not os.path.isdir(outputFolder):
        print("Path must be a dir")
        exit(-1)
else:
    outputFolder = sourceFolder + "/generated"
    if not os.path.exists(outputFolder):
        os.makedirs(outputFolder)

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

def getWidths(path, fontHeight, imgWidth, imgHeight):
    image = Image.open(path).convert('RGB')
    width = 0
    widths = []

    for y in range(imgHeight):
        for x in range(imgWidth):
            if image.getpixel((x, y)) == (0, 0, 0):
                if (x + 1) > width:
                    width = x + 1
        if y % fontHeight == fontHeight-1:
            widths.append(str(width))
            width = 0

    return ", ".join(widths)

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

cpp_f_body = (
    "\n"
    "const {} {}(\n"
    "{{\n"
    "{}\n"
    "}},\n"
    "(const Font::width_t[]){{\n"
    "    {}\n"
    "}},\n"
    "(const uint8_t[]){{\n"
    "    {}\n"
    "}});\n"
)

footer =  (
    "\n"
    "}\n"
)

# ---------------------------------------------------------------------------------------------------------------------

bitmapFiles = glob(sourceFolder + "/*.bmp")
bitmapFiles.sort()

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

## fonts

bitmapFiles = glob(sourceFolder + "/fonts/*.bmp")
bitmapFiles.sort()

fHpp = open(outputFolder + "/fonts.hpp", "w")
fCpp = open(outputFolder + "/fonts.cpp", "w")

fHpp.write(hpp_header.format("font", "font"))
fCpp.write(cpp_header.format("fonts", "font"))

for bmp in bitmapFiles:
    params = {}
    with open(bmp + ".info", "r") as infoFile:
        for line in infoFile:
            name, var = line.partition("=")[::2]
            params[name.strip()] = var.strip()

    fontHeight = int(params['height'])
    print("Converting font {} with height {}...".format(bmp, fontHeight))

    paramsArr = []
    for name, var in params.items():
        paramsArr.append("    ." + name + " = " + var)

    width, totalHeight, data_txt = processBitmap(bmp)
    widths = getWidths(bmp, fontHeight, width, totalHeight)
    paramsStr = ",\n".join(paramsArr)
    name = os.path.splitext(os.path.basename(bmp))[0]

    fHpp.write(hpp_body.format("Font", name))
    fCpp.write(cpp_f_body.format("Font", name, paramsStr, widths, data_txt))

fHpp.write(footer)
fCpp.write(footer)

fHpp.close()
fCpp.close()
