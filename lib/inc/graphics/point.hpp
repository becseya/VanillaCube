#pragma once

#include "types.hpp"

namespace VanillaCube {
namespace Graphics {

enum class Alignment
{
    TopLeft, // default
    TopCenter,
    TopRight,
    MidLeft,
    MidCenter,
    MidRight,
    BotLeft,
    BotCenter,
    BotRight
};

struct Point : public vector_t
{
    Point(int_t x, int_t y);
    Point(const vector_t& other);

    Point& align(Alignment alignment, uint_t w, uint_t h);
};

} // namespace Graphics
} // namespace VanillaCube
