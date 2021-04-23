#pragma once

#include "lib.h"

#include <math.h>
#include <type_traits>

namespace VanillaCube {
namespace Math {

template<typename T>
ALWAYS_INLINE constexpr T min(const T& a, const T& b)
{
    static_assert(std::is_arithmetic<T>::value, "Type must be numberic");

    return ((a < b) ? a : b);
}

template<typename T>
ALWAYS_INLINE constexpr T max(const T& a, const T& b)
{
    static_assert(std::is_arithmetic<T>::value, "Type must be numberic");

    return ((a > b) ? a : b);
}

template<typename T>
ALWAYS_INLINE constexpr T abs(const T& a)
{
    static_assert(std::is_arithmetic<T>::value, "Type must be numberic");

    if (a >= 0)
        return a;
    else
        return -a;
}

template<typename T, typename T2>
ALWAYS_INLINE constexpr T ceilDiv(const T& divident, const T2& n)
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
        *this = operator+(other);
        return *this;
    }
};

template<typename T>
struct vector_3d_t
{
    T x;
    T y;
    T z;
};

template<typename T>
struct Vector3D : public vector_3d_t<T>
{
    using vector_3d_t<T>::x;
    using vector_3d_t<T>::y;
    using vector_3d_t<T>::z;

    Vector3D(const vector_3d_t<T>& other)
        : vector_3d_t<T>(other)
    {}

    Vector3D(T x, T y, T z)
    {
        vector_3d_t<T>::x = x;
        vector_3d_t<T>::y = y;
        vector_3d_t<T>::z = z;
    }

    bool operator==(const Vector3D& other) const
    {
        return ((x == other.x) && (y == other.y) && (z == other.z));
    }

    Vector3D operator+(const Vector3D& other) const
    {
        return { static_cast<T>(x + other.x), static_cast<T>(y + other.y), static_cast<T>(z + other.z) };
    }

    T abs()
    {
        return sqrt(x * x + y * y + z * z);
    }

    Vector3D normalize()
    {
        T r = abs();
        return { static_cast<T>(x / r), static_cast<T>(y / r), static_cast<T>(z / r) };
    }

    Vector3D& operator+=(const Vector3D& other)
    {
        *this = operator+(other);
        return *this;
    }
};

} // namespace Math
} // namespace VanillaCube
