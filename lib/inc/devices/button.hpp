#pragma once

#include "../periph/gpio.hpp"
#include "../timing/click-reader.hpp"

namespace VanillaCube {
namespace Devices {

template<typename GPIO_PIN, Periph::InputMode MODE>
struct Button : public Timing::ClickReader
{
    static void configure()
    {
        GPIO_PIN::configure(MODE);
    }

    bool update()
    {
        if (MODE == Periph::InputMode::PullUp)
            return ClickReader::update(!GPIO_PIN::get());
        else
            return ClickReader::update(GPIO_PIN::get());
    }
};

} // namespace Devices
} // namespace VanillaCube
