#pragma once

#include "bitmap.hpp"
#include "canvas.hpp"

namespace VanillaCube {
namespace Graphics {

struct Display : public Canvas
{
    Display(Bitmap& draw_area);

    virtual void render() = 0;
};

template<uint_t WIDTH, uint_t HEIGHT>
struct BufferedDisplay : public BitMapAllocator<WIDTH, HEIGHT>, public Display
{
    BufferedDisplay()
        : BitMapAllocator<WIDTH, HEIGHT>()
        , Display(static_cast<Bitmap&>(*this))
    {}
};

} // namespace Graphics
} // namespace VanillaCube
