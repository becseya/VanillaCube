#include <main.h>
#include <periph/gpio.hpp>

using namespace VanillaCube;
using namespace Periph;

using PinLED = GpioPin<PortA, 0>;

void vcube_init()
{
    PinLED::configure(OutputMode::PushPull);
}

void vcube_loop()
{
    LL_mDelay(1000);
    PinLED::toggle();
}
