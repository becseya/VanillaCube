#pragma once

#include <initializer_list>
#include <tuple>

namespace VanillaCube {

template<typename T>
class Array
{
    const size_t _size;
    T* const     _items;

  public:
    Array(size_t size, T* items)
        : _size(size)
        , _items(items)
    {}

    size_t size() const
    {
        return _size;
    }

    T& operator[](size_t n)
    {
        return _items[n];
    }

    const T& operator[](size_t n) const
    {
        return _items[n];
    }

    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;
};

template<typename T, size_t N>
struct ArrayFactory : public Array<T>
{
    T items[N];

    template<typename... E>
    ArrayFactory(E&&... e)
        : Array<T>(N, items)
        , items{ std::forward<E>(e)... }
    {}
};

} // namespace VanillaCube
