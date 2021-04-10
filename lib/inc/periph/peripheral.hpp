#pragma once

#include "../lib.h"
#include "../util.hpp"

#include <stm32f1xx_ll_bus.h>

// --------------------------------------------------------------------------------------------------------------------

namespace VanillaCube {
namespace Periph {

using Util::ENUM_TO_UINT;

enum class Bus
{
    APB1,
    AHB1,
    APB2,
};

template<uint32_t CONTROL_ADDR, Bus BUS, uint32_t CLOCK_FLAG>
struct periph_wrapper_t
{
    static constexpr uint32_t controlAddr = CONTROL_ADDR;
    static constexpr Bus      bus         = BUS;
    static constexpr uint32_t clockFlag   = CLOCK_FLAG;
};

template<typename PERIPH>
struct ClockControl
{
    static void enableClock()
    {
        switch (PERIPH::bus) {
        case Bus::APB1: LL_APB1_GRP1_EnableClock(PERIPH::clockFlag); break;
        case Bus::AHB1: LL_AHB1_GRP1_EnableClock(PERIPH::clockFlag); break;
        case Bus::APB2: LL_APB2_GRP1_EnableClock(PERIPH::clockFlag); break;
        }
    }

    static void disableClock()
    {
        switch (PERIPH::bus) {
        case Bus::APB1: LL_APB1_GRP1_DisableClock(PERIPH::clockFlag); break;
        case Bus::AHB1: LL_AHB1_GRP1_DisableClock(PERIPH::clockFlag); break;
        case Bus::APB2: LL_APB2_GRP1_DisableClock(PERIPH::clockFlag); break;
        }
    }

    static bool isClockEnabled()
    {
        switch (PERIPH::bus) {
        case Bus::APB1: return LL_APB1_GRP1_IsEnabledClock(PERIPH::clockFlag);
        case Bus::AHB1: return LL_AHB1_GRP1_IsEnabledClock(PERIPH::clockFlag);
        case Bus::APB2: return LL_APB2_GRP1_IsEnabledClock(PERIPH::clockFlag);
        }

        return false;
    }

    static void MakeSureClockIsEnabled()
    {
        if (!isClockEnabled())
            enableClock();
    }
};

template<typename PERIPH, typename T>
struct Peripheral : public ClockControl<PERIPH>
{
    static ALWAYS_INLINE T& CTRL_STRUCT()
    {
        return *static_cast<T*>(reinterpret_cast<void*>(PERIPH::controlAddr));
    }
};

} // namespace Periph
} // namespace VanillaCube
