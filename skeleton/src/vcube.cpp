#include <main.h>

void vcube_init()
{

}

void vcube_loop()
{
    LL_mDelay(1000);
    LL_GPIO_TogglePin(PIN_BACKLIGHT_GPIO_Port, PIN_BACKLIGHT_Pin);
}
