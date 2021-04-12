#pragma once

#include "../lib.h"

// --------------------------------------------------------------------------------------------------------------------

namespace VanillaCube {
namespace Timing {

using systick_t = uint64_t;

const systick_t& getSysick();
systick_t        getSystickSince(systick_t past);

} // namespace Timing
} // namespace VanillaCube
