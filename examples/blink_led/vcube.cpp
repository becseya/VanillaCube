#include <main.h>

#include <periph/gpio.hpp>
#include <util.hpp>

using namespace VanillaCube;
using namespace Periph;

using PinLED = GpioPin<GpioA, 0>;

void vcube_init()
{
    PinLED::enableClock();
    PinLED::configure(OutputMode::PushPull);
}

void vcube_loop()
{
    Util::DelayMs(1000);
    PinLED::toggle();
}
