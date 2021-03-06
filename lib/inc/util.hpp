#pragma once

#include "lib.h"

#include <typeinfo>

extern "C" void LL_mDelay(uint32_t Delay);

// --------------------------------------------------------------------------------------------------------------------

namespace VanillaCube {
namespace Util {

ALWAYS_INLINE void DelayMs(uint32_t miliseconds)
{
    LL_mDelay(miliseconds);
}

template<unsigned N>
ALWAYS_INLINE static void NOP()
{
    asm("nop");
    NOP<N - 1>();
}

template<>
ALWAYS_INLINE void NOP<0>(){};

template<typename T>
ALWAYS_INLINE uint32_t& ENUM_TO_UINT(T& val)
{
    static_assert(std::is_enum<T>::value, "T must be an enum");

    return (uint32_t&)(val);
}

ALWAYS_INLINE uint32_t getSerial()
{
    uint32_t tmp = LL_GetUID_Word0();
    tmp ^= LL_GetUID_Word1();
    tmp ^= LL_GetUID_Word2();
    return tmp;
}

} // namespace Util
} // namespace VanillaCube
