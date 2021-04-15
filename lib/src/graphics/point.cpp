#include <graphics/point.hpp>

using namespace VanillaCube::Graphics;

// --------------------------------------------------------------------------------------------------------------------

Point::Point(int_t x, int_t y)
    : vector_t(x, y)
{}

Point::Point(const vector_t& other)
    : vector_t(other)
{}

Point& Point::align(Alignment alignment, uint_t width, uint_t height)
{
    // clang-format off
        switch (alignment) {
            case Alignment::TopLeft:                                break;
            case Alignment::TopCenter:                x -= width/2; break;
            case Alignment::TopRight:                 x -= width;   break;
            case Alignment::MidLeft:   y -= height/2;               break;
            case Alignment::MidCenter: y -= height/2; x -= width/2; break;
            case Alignment::MidRight:  y -= height/2; x -= width;   break;
            case Alignment::BotLeft:   y -= height;                 break;
            case Alignment::BotCenter: y -= height;   x -= width/2; break;
            case Alignment::BotRight:  y -= height;   x -= width;   break;
        }
    // clang-format on

    return *this;
}
