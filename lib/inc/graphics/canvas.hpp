#pragma once

#include "../math.hpp"
#include "bitmap.hpp"
#include "font.hpp"
#include "point.hpp"
#include "types.hpp"

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
    Canvas(const Canvas& c, Point p0, uint_t width, uint_t height);

    uint_t      getWidth() const;
    uint_t      getHeight() const;
    const Font& getFont() const;

    void clear();
    void setMixing(Mixing mixing);
    void setFont(const Font& font);

    int_t write(const char* str, Point p0, Alignment alignment = Alignment::TopLeft);

    void drawLine(const Point& p0, const Point& p1);
    void drawCircle(Point p0, float r);
    void drawImage(Point p0, const Bitmap& img, Alignment alignment = Alignment::TopLeft);
    void fillRectange(Point p0, uint_t width, uint_t height, bool clear = false);

  private:
    void drawLineByX(Point p, int_t dx, float m);
    void drawLineByY(Point p, int_t dy, float m);
    void drawPoint(int_t x, int_t y);
};

} // namespace Graphics
} // namespace VanillaCube
