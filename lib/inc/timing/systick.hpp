#pragma once

#include "../lib.h"

// --------------------------------------------------------------------------------------------------------------------

namespace VanillaCube {
namespace Timing {

using systick_t = uint64_t;

const systick_t& getSysick();
systick_t        getSystickSince(systick_t past);

/**
 * @brief  Waits up to a maximum number of system tick for a given flag to be set true
 * @param  flag_is_true lambda expression: takes no paramaterers, returns true, when flag is set
 * @param  max_ticks number of system thick before timeout.
 * @param  past timestamp to which time passed will be measured to. (defult is current systick)
 * @retval A bool, which is
 *          - true, flag was set true in time
 *          - false, if timed out
 */
template<typename F>
ALWAYS_INLINE bool WAIT_FOR_FLAG(const F& flag_is_true, uint32_t max_ticks, systick_t past = getSysick())
{
    while (!flag_is_true()) {
        if (getSystickSince(past) >= max_ticks)
            return false;
    }

    return true;
}

} // namespace Timing
} // namespace VanillaCube
