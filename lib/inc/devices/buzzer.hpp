#pragma once

#include "../periph/gpio.hpp"
#include "../periph/timer.hpp"
#include "../timing/pulser.hpp"
#include "../util.hpp"

namespace VanillaCube {
namespace Devices {

template<typename GPIO_PIN, typename TIMER_CH, bool INVERT_POLARITY = false>
struct Buzzer : public Timing::Pulser
{
    static void configure()
    {
        TIMER_CH::startOc(getInactiveMode());
        GPIO_PIN::configure(Periph::OutputMode::AltFuncPP);
    }

    static void beepSingleBlocking(uint32_t pulse_width)
    {
        TIMER_CH::setOcMode(Periph::TimerOcMode::Toggle);
        Util::DelayMs(pulse_width);
        TIMER_CH::setOcMode(getInactiveMode());
    }

  private:
    void handleSignalChange(bool new_signal) override
    {
        if (new_signal) {
            TIMER_CH::setOcMode(Periph::TimerOcMode::Toggle);
        } else
            TIMER_CH::setOcMode(getInactiveMode());
    }

    ALWAYS_INLINE static Periph::TimerOcMode getInactiveMode()
    {
        if (INVERT_POLARITY)
            return Periph::TimerOcMode::ForcedInactive;
        else
            return Periph::TimerOcMode::ForcedActive;
    }
};

} // namespace Devices
} // namespace VanillaCube
