#pragma once

#include "bitmap.hpp"
#include "types.hpp"

#define C_DEG   '`'
#define STR_DEG "`"

namespace VanillaCube {
namespace Graphics {

struct Font
{
    using width_t = uint8_t;

    enum Flags
    {
        F_REDUCED_SET  = (1 << 0),
        F_CAPITAL_ONLY = (1 << 1)
    };

    struct Params
    {
        uint_t size;
        uint_t height;
        uint_t maxWidth;
        char   firstChar;
        uint_t spaceSize;
        uint_t hSpace;
        uint_t vSpace;
        uint_t flags;
    };

  private:
    const Params         params;
    const width_t* const widths;
    const uint8_t* const pData;

  public:
    Font(const Params& params, const width_t* widths, const uint8_t* p_data);

    const Bitmap  getChar(char c) const;
    const Params& getParams() const;
    size_t        getStringWidth(const char* str) const;

  private:
    int    getIndex(char c) const;
    size_t getOffset(int index) const;
};

} // namespace Graphics
} // namespace VanillaCube
