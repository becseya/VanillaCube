#pragma once

#include "../math.hpp"
#include "bitmap.hpp"
#include "font.hpp"
#include "point.hpp"

namespace VanillaCube {
namespace Graphics {

enum class Mixing
{
    Add,
    Sub,
    Invert,
    Copy
};

class Canvas
{
    Bitmap&     bitmap;
    Mixing      mixing;
    const Font* font;

    uint_t width;
    uint_t height;
    Point  o; // origin

  public:
    Canvas(Bitmap& bitmap);

    uint_t getWidth();
    uint_t getHeight();

    void clear();
    void setMixing(Mixing mixing);
    void setFont(const Font& font);

    int16_t write(const char* str, Point p0, Alignment alignment = Alignment::TopLeft);

    void drawLine(const Point& p0, const Point& p1);
    void drawCircle(Point p0, float r);
    void drawImage(Point p0, const Bitmap& img, Alignment alignment = Alignment::TopLeft);

  private:
    void drawLineByX(Point p, int_t dx, float m);
    void drawLineByY(Point p, int_t dy, float m);
    void drawPoint(int_t x, int_t y);
};

} // namespace Graphics
} // namespace VanillaCube
