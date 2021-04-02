#pragma once

#include "lib.h"

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

} // namespace Util
} // namespace VanillaCube
