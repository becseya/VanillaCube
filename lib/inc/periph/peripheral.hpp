#pragma once

#include "../lib.h"

extern "C" void LL_APB1_GRP1_EnableClock(uint32_t);
extern "C" void LL_APB1_GRP1_DisableClock(uint32_t);
extern "C" void LL_AHB1_GRP1_EnableClock(uint32_t);
extern "C" void LL_AHB1_GRP1_DisableClock(uint32_t);
extern "C" void LL_APB2_GRP1_EnableClock(uint32_t);
extern "C" void LL_APB2_GRP1_DisableClock(uint32_t);

// --------------------------------------------------------------------------------------------------------------------

namespace VanillaCube {
namespace Periph {

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
struct Peripheral
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
};

} // namespace Periph
} // namespace VanillaCube
