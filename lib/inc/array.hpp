#pragma once

#include <tuple>

namespace VanillaCube {

template<typename T>
struct Array
{
    const size_t size;
    T            items[];

    T& operator[](size_t n)
    {
        return items[n];
    }

    const T& operator[](size_t n) const
    {
        return items[n];
    }

    Array(const Array&) = delete;
    Array& operator=(const Array&) = delete;
};

} // namespace VanillaCube

// --------------------------------------------------------------------------------------------------------------------

#define NUMBER_OF_VA_ARGS(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

#define ARRAY_INITALIZER(...)                                                                                          \
    {                                                                                                                  \
        NUMBER_OF_VA_ARGS(__VA_ARGS__),                                                                                \
        {                                                                                                              \
            __VA_ARGS__                                                                                                \
        }                                                                                                              \
    }
