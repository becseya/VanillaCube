#include <graphics/bitmap.hpp>

#include <math.hpp>

using namespace VanillaCube::Graphics;

// --------------------------------------------------------------------------------------------------------------------

#define BITSHIFT_X(x) (1 << (7 - (x & 0b0111)))

Bitmap::Bitmap(uint_t width, uint_t height, const uint8_t* const_buffer)
    : pWrite(nullptr)
    , pRead(const_buffer)
    , columns(Math::ceilDiv(width, 8))
    , bufferSize(columns * height)
    , width(width)
    , height(height)
{}

Bitmap::Bitmap(uint_t width, uint_t height, uint8_t* buffer)
    : pWrite(buffer)
    , pRead(buffer)
    , columns(Math::ceilDiv(width, 8))
    , bufferSize(columns * height)
    , width(width)
    , height(height)
{}

uint_t Bitmap::getWidth() const
{
    return width;
}

uint_t Bitmap::getHeight() const
{
    return height;
}

bool Bitmap::get(uint_t x, uint_t y) const
{
    const int idx = (columns * y) + (x / 8);

    if ((idx < 0) || (idx >= bufferSize))
        return false;

    return (pRead[idx] & BITSHIFT_X(x));
}

void Bitmap::set(bool val, uint_t x, uint_t y)
{
    const int idx = (columns * y) + (x / 8);

    if ((idx < 0) || (idx >= bufferSize))
        return;

    if (val)
        pWrite[idx] |= BITSHIFT_X(x);
    else
        pWrite[idx] &= ~BITSHIFT_X(x);
}

void Bitmap::invert(uint_t x, uint_t y)
{
    const int idx = (columns * y) + (x / 8);

    if ((idx < 0) || (idx >= bufferSize))
        return;

    pWrite[idx] ^= BITSHIFT_X(x);
}

void Bitmap::clearAll()
{
    for (int i = 0; i < bufferSize; i++)
        pWrite[i] = 0;
}
