#include <timing/systick.hpp>

using namespace VanillaCube::Timing;

// --------------------------------------------------------------------------------------------------------------------

extern "C" volatile systick_t __vanillacube_systick_counter;

const systick_t& VanillaCube::Timing::getSysick()
{
    return (systick_t&)__vanillacube_systick_counter;
}

systick_t VanillaCube::Timing::getSystickSince(systick_t past)
{
    return __vanillacube_systick_counter - past;
}
