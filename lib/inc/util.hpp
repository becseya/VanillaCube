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

} // namespace Util
} // namespace VanillaCube
