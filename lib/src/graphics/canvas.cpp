#include <graphics/canvas.hpp>

#include <math.hpp>

using namespace VanillaCube::Graphics;

// --------------------------------------------------------------------------------------------------------------------

Canvas::Canvas(Bitmap& bitmap)
    : bitmap(bitmap)
    , mixing(Mixing::Add)
    , font(nullptr)
    , width(bitmap.getWidth())
    , height(bitmap.getHeight())
    , o(0, 0)
{}

Canvas::Canvas(const Canvas& c, Point p0, uint_t width, uint_t height)
    : bitmap(c.bitmap)
    , mixing(c.mixing)
    , font(c.font)
    , width(width)
    , height(height)
    , o(p0)
{}

uint_t Canvas::getWidth() const
{
    return width;
}

uint_t Canvas::getHeight() const
{
    return height;
}

const Font& Canvas::getFont() const
{
    return *font;
}

void Canvas::setMixing(Mixing mixing)
{
    Canvas::mixing = mixing;
}

void Canvas::setFont(const Font& font)
{
    Canvas::font = &font;
}

void Canvas::clear()
{
    if ((o == Point(0, 0)) && (width == bitmap.getWidth()) && (height == bitmap.getHeight()))
        bitmap.clearAll();
    else
        for (uint_t x = 0; x < width; x++)
            for (uint_t y = 0; y < height; y++)
                bitmap.set(false, o.x + x, o.y + y);
}

// --------------------------------------------------------------------------------------------------------------------

int_t Canvas::write(const char* str, Point p, Alignment alignment)
{
    if (!font)
        return 0;

    p.align(alignment, font->getStringWidth(str), font->getParams().height);
    auto x_start = p.x;

    while (*str && (p.x < width)) {
        if (*str == ' ')
            p.x += font->getParams().spaceSize;
        else {
            const Bitmap img = font->getChar(*str);

            if (img.getWidth() > 0) {
                drawImage(p, img);
                p.x += (img.getWidth() + font->getParams().hSpace);
            }
        }
        str++;
    }

    return (p.x - x_start);
}

// --------------------------------------------------------------------------------------------------------------------

void Canvas::drawLine(const Point& p0, const Point& p1)
{
    if (Math::abs(p0.x - p1.x) >= Math::abs(p0.y - p1.y)) {
        if (p1.x > p0.x)
            drawLineByX(p0, (p1.x - p0.x), ((float)(p1.y - p0.y)) / (p1.x - p0.x));
        else
            drawLineByX(p1, (p0.x - p1.x), ((float)(p0.y - p1.y)) / (p0.x - p1.x));
    } else {
        if (p1.y > p0.y)
            drawLineByY(p0, (p1.y - p0.y), ((float)(p1.x - p0.x)) / (p1.y - p0.y));
        else
            drawLineByY(p1, (p0.y - p1.y), ((float)(p0.x - p1.x)) / (p0.y - p1.y));
    }
}

void Canvas::drawCircle(Point p0, float r)
{
    p0 += o;

    const uint_t i_max = r / sqrt(2.f) + 1.5f;
    const float  r2    = r * r;

    for (uint_t i = 0; i < i_max; i++) {
        uint_t fx = sqrt(1 - (float)(i * i) / r2) * r + 0.5f;

        bitmap.set(true, p0.x + i, p0.y + fx);
        bitmap.set(true, p0.x - i, p0.y + fx);
        bitmap.set(true, p0.x + i, p0.y - fx);
        bitmap.set(true, p0.x - i, p0.y - fx);
        bitmap.set(true, p0.x + fx, p0.y + i);
        bitmap.set(true, p0.x + fx, p0.y - i);
        bitmap.set(true, p0.x - fx, p0.y + i);
        bitmap.set(true, p0.x - fx, p0.y - i);
    }
}

void Canvas::drawImage(Point p0, const Bitmap& img, Alignment alignment)
{
    p0.align(alignment, img.getWidth(), img.getHeight());

    const int_t x_max = Math::min<int_t>(img.getWidth() - 1, width - p0.x);
    const int_t y_max = Math::min<int_t>(img.getHeight() - 1, height - p0.y);

    p0 += o;

    for (int16_t x = 0; x <= x_max; x++) {
        for (int16_t y = 0; y <= y_max; y++) {
            switch (mixing) {
            case Mixing::Copy: {
                bitmap.set(true, p0.x + x, p0.y + y);
                break;
            }
            case Mixing::Add:
                if (img.get(x, y))
                    bitmap.set(true, p0.x + x, p0.y + y);
                break;
            case Mixing::Sub:
                if (img.get(x, y))
                    bitmap.set(false, p0.x + x, p0.y + y);
                break;
            case Mixing::Invert:
                if (img.get(x, y))
                    bitmap.invert(p0.x + x, p0.y + y);
                break;
            }
        }
    }
}

void Canvas::fillRectange(Point p0, uint_t w, uint_t h, bool clear)
{
    const int_t x_max = Math::min<int_t>(w, width - p0.x);
    const int_t y_max = Math::min<int_t>(h, height - p0.y);
    p0 += o;

    if (clear)
        for (int16_t x = 0; x < x_max; x++)
            for (int16_t y = 0; y < y_max; y++)
                bitmap.set(false, p0.x + x, p0.y + y);
    else
        for (int16_t x = 0; x < x_max; x++)
            for (int16_t y = 0; y < y_max; y++)
                bitmap.set(true, p0.x + x, p0.y + y);
}

// --------------------------------------------------------------------------------------------------------------------

void Canvas::drawLineByX(Point p, int_t dx, float m)
{
    const int_t x_max = Math::min<int_t>(dx, width - p.x - 1);
    p += o;

    for (int_t x = 0; x <= x_max; x++)
        drawPoint(p.x + x, (float)p.y + x * m + 0.5f);
}

void Canvas::drawLineByY(Point p, int_t dy, float m)
{
    const int_t y_max = Math::min<int_t>(dy, height - p.y - 1);
    p += o;

    for (int_t y = 0; y <= y_max; y++)
        drawPoint((float)p.x + y * m + 0.5f, p.y + y);
}

void Canvas::drawPoint(int_t x, int_t y)
{
    if ((x < 0) || (y < 0) || ((x - o.x) >= width) || ((y - o.y) >= height))
        return;

    switch (mixing) {
    case Mixing::Copy:
    case Mixing::Add: bitmap.set(true, x, y); break;
    case Mixing::Sub: bitmap.set(false, x, y); break;
    case Mixing::Invert: bitmap.invert(x, y); break;
    }
}
