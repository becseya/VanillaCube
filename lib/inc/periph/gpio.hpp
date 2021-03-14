#pragma once

#include "lib.h"

#include <stm32f1xx_ll_gpio.h>

#ifdef GPIOA_BASE
    #define PortA (GPIOA_BASE)
#endif
#ifdef GPIOB_BASE
    #define PortB (GPIOB_BASE)
#endif
#ifdef GPIOC_BASE
    #define PortC (GPIOC_BASE)
#endif
#ifdef GPIOD_BASE
    #define PortD (GPIOD_BASE)
#endif
#ifdef GPIOE_BASE
    #define PortE (GPIOE_BASE)
#endif
#ifdef GPIOF_BASE
    #define PortF (GPIOF_BASE)
#endif
#ifdef GPIOG_BASE
    #define PortG (GPIOG_BASE)
#endif

// --------------------------------------------------------------------------------------------------------------------

namespace VanillaCube {
namespace Periph {

enum class GpioSpeed
{
    Slow,
    Medium,
    Fast
};

enum class OutputMode
{
    PushPull,
    OpenDrain,
    AltFuncPP,
    AltFuncOD,
};

enum class InputMode
{
    Analog,
    Floating,
    PullDown,
    PullUp,
};

typedef uint32_t gpio_port_t;

namespace { // helper functions inside anonymous namespace

struct port_masks_t
{
    uint32_t h_set_mask          = 0;
    uint32_t h_cli_mask_inverted = 0xFFFF;
    uint32_t l_set_mask          = 0;
    uint32_t l_cli_mask_inverted = 0xFFFF;
};

uint32_t GET_MASK(const OutputMode mode, const GpioSpeed speed = GpioSpeed::Slow);
uint32_t GET_MASK(const InputMode mode);
void     SET_MASK(gpio_port_t port, const uint32_t pin, const uint32_t mask);
void     CALCULATE_MASK(port_masks_t& masks, const uint32_t mode, const uint32_t size, const uint32_t offset);

ALWAYS_INLINE GPIO_TypeDef& DEREF(gpio_port_t port)
{
    return *(GPIO_TypeDef*)port;
}

} // namespace

// --------------------------------------------------------------------------------------------------------------------

namespace Gpio {

static void configure(gpio_port_t port, const uint32_t pin, const OutputMode mode, const GpioSpeed speed)
{
    SET_MASK(port, pin, GET_MASK(mode, speed));
}

static void configure(gpio_port_t port, const uint32_t pin, const InputMode mode)
{
    SET_MASK(port, pin, GET_MASK(mode));

    if (mode == InputMode::PullDown)
        DEREF(port).BRR = (1 << pin);
    else if (mode == InputMode::PullUp)
        DEREF(port).BSRR = (1 << pin);
}

ALWAYS_INLINE void set(gpio_port_t port, const uint32_t pin, bool val)
{
    if (val)
        DEREF(port).BSRR = (1 << pin);
    else
        DEREF(port).BRR = (1 << pin);
}

ALWAYS_INLINE static bool get(gpio_port_t port, const uint32_t pin)
{
    return (DEREF(port).IDR & (1 << pin));
}

ALWAYS_INLINE static void toggle(gpio_port_t port, const uint32_t pin)
{
    if (DEREF(port).ODR & (1 << pin))
        DEREF(port).BRR = (1 << pin);
    else
        DEREF(port).BSRR = (1 << pin);
}
} // namespace Gpio

template<gpio_port_t PORT, uint32_t PIN>
struct GpioPin
{
    ALWAYS_INLINE static void configure(const OutputMode mode, const GpioSpeed speed = GpioSpeed::Slow)
    {
        Gpio::configure(PORT, PIN, mode, speed);
    }

    ALWAYS_INLINE static void configure(const InputMode mode)
    {
        Gpio::configure(PORT, PIN, mode);
    }

    ALWAYS_INLINE static void set(bool val)
    {
        Gpio::set(PORT, PIN, val);
    }

    ALWAYS_INLINE static bool get()
    {
        return Gpio::get(PORT, PIN);
    }

    ALWAYS_INLINE static void toggle()
    {
        Gpio::toggle(PORT, PIN);
    }
};

template<gpio_port_t PORT, uint32_t SIZE, uint32_t OFFSET = 0>
class GpioPort
{
    // switching between I/O modes is highly time critical, therefore we are calculating the masks in advance
    static inline port_masks_t mInput;
    static inline port_masks_t mOutput;

  public:
    ALWAYS_INLINE static void write(uint32_t data)
    {
        data &= N_ONES(SIZE);
        DEREF(PORT).BSRR = ((~data << OFFSET) << 16) | ((data << OFFSET) & 0xFF);
    }

    ALWAYS_INLINE static uint32_t read()
    {
        return (DEREF(PORT).IDR >> OFFSET) & N_ONES(SIZE);
    }

    ALWAYS_INLINE static void setAsOutput()
    {
        applyMasks(mOutput);
    }

    ALWAYS_INLINE static void setAsInput()
    {
        applyMasks(mInput);
    }

    static void setOutputMode(const OutputMode mode, const GpioSpeed speed = GpioSpeed::Fast)
    {
        CALCULATE_MASK(mOutput, GET_MASK(mode, speed), SIZE, OFFSET);
        setAsOutput();
    }

    static void setInputMode(const InputMode mode)
    {
        CALCULATE_MASK(mInput, GET_MASK(mode), SIZE, OFFSET);
        setAsInput();
    }

  private:
    ALWAYS_INLINE static void applyMasks(const port_masks_t& m)
    {
        if (m.l_cli_mask_inverted != 0xFFFF)
            DEREF(PORT).CRL = (DEREF(PORT).CRL & m.l_cli_mask_inverted) | m.l_set_mask;

        if (m.h_cli_mask_inverted != 0xFFFF)
            DEREF(PORT).CRH = (DEREF(PORT).CRH & m.h_cli_mask_inverted) | m.h_set_mask;
    }
};

// --------------------------------------------------------------------------------------------------------------------

namespace {

uint32_t GET_MASK(const OutputMode mode, const GpioSpeed speed)
{
    switch (mode) {
    case OutputMode::PushPull:
        switch (speed) {
        case GpioSpeed::Slow: return 0b0001;
        case GpioSpeed::Medium: return 0b0010;
        case GpioSpeed::Fast: return 0b0011;
        }
    case OutputMode::OpenDrain:
        switch (speed) {
        case GpioSpeed::Slow: return 0b0101;
        case GpioSpeed::Medium: return 0b0110;
        case GpioSpeed::Fast: return 0b0111;
        }
    case OutputMode::AltFuncPP:
        switch (speed) {
        case GpioSpeed::Slow: return 0b1001;
        case GpioSpeed::Medium: return 0b1010;
        case GpioSpeed::Fast: return 0b1011;
        }
    case OutputMode::AltFuncOD:
        switch (speed) {
        case GpioSpeed::Slow: return 0b1101;
        case GpioSpeed::Medium: return 0b1110;
        case GpioSpeed::Fast: return 0b1111;
        }
    }

    return 0; // silence compiler
}

uint32_t GET_MASK(const InputMode mode)
{
    switch (mode) {
    case InputMode::Analog: return 0b0000;
    case InputMode::Floating: return 0b0100;
    case InputMode::PullDown: // falltrough
    case InputMode::PullUp: return 0b1000;
    }

    return 0; // silence compiler
}

void SET_MASK(gpio_port_t port, const uint32_t pin, const uint32_t mask)
{
    if (pin < 8)
        DEREF(port).CRL = (DEREF(port).CRL & ~(0b1111 << (4 * pin))) | (mask << (4 * pin));
    else
        DEREF(port).CRH = (DEREF(port).CRH & ~(0b1111 << (4 * (pin - 8)))) | (mask << (4 * (pin - 8)));
}

void CALCULATE_MASK(port_masks_t& m, const uint32_t mode, const uint32_t size, const uint32_t offset)
{
    m.h_set_mask          = 0;
    m.h_cli_mask_inverted = 0xFFFF;
    m.l_set_mask          = 0;
    m.l_cli_mask_inverted = 0xFFFF;

    if (offset < 8) {
        for (uint32_t i = offset; (i < offset + size) && (i < 8); i++) {
            m.l_cli_mask_inverted &= ~(0b1111 << (4 * i));
            m.l_set_mask |= (mode << (4 * i));
        }
    }

    const int32_t last_index = (offset + size - 1);
    if (last_index >= 8) {
        for (int32_t i = (last_index - 8); (i >= (int32_t)offset - 8) && (i >= 0); i--) {
            m.h_cli_mask_inverted &= ~(0b1111 << (4 * i));
            m.h_set_mask |= (mode << (4 * i));
        }
    }
}

} // namespace

} // namespace Periph
} // namespace VanillaCube
