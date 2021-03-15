#include <main.h>

#include <periph/gpio.hpp>
#include <util.hpp>

using namespace VanillaCube;
using namespace Periph;

using PinLED = GpioPin<GpioA, 0>;

void main_init()
{
    PinLED::enableClock();
    PinLED::configure(OutputMode::PushPull);
}

void main_loop()
{
    Util::DelayMs(1000);
    PinLED::toggle();
}
