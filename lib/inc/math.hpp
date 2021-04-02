#pragma once

#include "lib.h"

#include <math.h>
#include <type_traits>

namespace VanillaCube {
namespace Math {

template<typename T>
ALWAYS_INLINE T min(const T& a, const T& b)
{
    static_assert(std::is_arithmetic<T>::value, "Type must be numberic");

    return ((a < b) ? a : b);
}

template<typename T>
ALWAYS_INLINE T max(const T& a, const T& b)
{
    static_assert(std::is_arithmetic<T>::value, "Type must be numberic");

    return ((a > b) ? a : b);
}

template<typename T>
ALWAYS_INLINE T abs(const T& a)
{
    static_assert(std::is_arithmetic<T>::value, "Type must be numberic");

    if (a >= 0)
        return a;
    else
        return -a;
}

template<typename T, typename T2>
ALWAYS_INLINE T ceilDiv(const T& divident, const T2& n)
{
    static_assert(std::is_integral<T>::value && std::is_integral<T2>::value, "Type must be integer");

    return ((divident + (n - 1)) / n);
}

// --------------------------------------------------------------------------------------------------------------------

template<typename T>
struct Vector2D
{
    T x;
    T y;

    Vector2D(T x, T y)
        : x(x)
        , y(y)
    {}

    bool operator==(const Vector2D& other) const
    {
        return ((x == other.x) && (y == other.y));
    }

    Vector2D operator+(const Vector2D& other) const
    {
        return { static_cast<T>(x + other.x), static_cast<T>(y + other.y) };
    }

    Vector2D& operator+=(const Vector2D& other)
    {
        operator+(other);
        return *this;
    }
};

} // namespace Math
} // namespace VanillaCube
