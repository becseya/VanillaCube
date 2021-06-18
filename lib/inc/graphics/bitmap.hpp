#pragma once

#include "types.hpp"

namespace VanillaCube {
namespace Graphics {

class Bitmap
{
    uint8_t*       pWrite;
    const uint8_t* pRead;
    uint_t         columns;
    uint_t         bufferSize;

    uint_t width;
    uint_t height;

  public:
    Bitmap(uint_t width, uint_t height, const uint8_t* const_buffer);
    Bitmap(uint_t width, uint_t height, uint8_t* buffer);

    uint_t getWidth() const;
    uint_t getHeight() const;

    bool get(uint_t x, uint_t y) const;

    void set(bool val, uint_t x, uint_t y);
    void invert(uint_t x, uint_t y);
    void clearAll();
};

template<uint_t WIDTH, uint_t HEIGHT>
struct BitMapAllocator : public Bitmap
{
    static constexpr size_t BUFFER_SIZE = HEIGHT * Math::ceilDiv(WIDTH, 8);

  protected:
    uint8_t buffer[BUFFER_SIZE];

  public:
    BitMapAllocator()
        : Bitmap(WIDTH, HEIGHT, buffer)
    {}
};

} // namespace Graphics
} // namespace VanillaCube
