#include <graphics/font.hpp>

#include <math.hpp>

using namespace VanillaCube::Graphics;

// --------------------------------------------------------------------------------------------------------------------

Font::Font(const Params& params, const width_t* widths, const uint8_t* p_data)
    : params(params)
    , widths(widths)
    , pData(p_data)
{}

const Font::Params& Font::getParams() const
{
    return params;
}

const Bitmap Font::getChar(char c) const
{
    int idx = getIndex(c);

    if (idx < 0)
        return Bitmap(0, 0, static_cast<const uint8_t*>(nullptr));
    else
        return Bitmap(widths[idx], params.height, pData + getOffset(idx));
}

size_t Font::getStringWidth(const char* str) const
{
    size_t width = 0;
    int    idx;

    while (*str) {
        if (*str == ' ')
            width += params.spaceSize;
        else {
            idx = getIndex(*str);
            if (idx >= 0) {
                if (width > 0)
                    width += params.hSpace;

                width += widths[idx];
            }
        }
        str++;
    }

    return width;
}

// --------------------------------------------------------------------------------------------------------------------

int Font::getIndex(char c) const
{

    if (params.flags & F_REDUCED_SET) {
        switch (c) {
        case '%': c = ';'; break;
        case C_DEG: c = '<'; break;
        case '\'': c = '='; break;
        case '"': c = '>'; break;
        case '!': c = '?'; break;
        case '+': c = '@'; break;
        }
    }

    if (params.flags & F_CAPITAL_ONLY)
        if (('a' <= c) && (c <= 'z'))
            c -= 'a' - 'A';

    if ((c < params.firstChar) || (c > (params.firstChar + params.size - 1)))
        return -1;

    return (c - params.firstChar);
}

size_t Font::getOffset(int index) const
{
    return index * params.height * Math::ceilDiv(params.maxWidth, 8);
}
