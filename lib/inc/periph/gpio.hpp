#pragma once

#include "peripheral.hpp"

#include <stm32f1xx_ll_gpio.h>

namespace VanillaCube {
namespace Periph {

#ifdef GPIOA_BASE
using GpioA = periph_wrapper_t<GPIOA_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_GPIOA>;
#endif
#ifdef GPIOB_BASE
using GpioB = periph_wrapper_t<GPIOB_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_GPIOB>;
#endif
#ifdef GPIOC_BASE
using GpioC = periph_wrapper_t<GPIOC_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_GPIOC>;
#endif
#ifdef GPIOD_BASE
using GpioD = periph_wrapper_t<GPIOD_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_GPIOD>;
#endif
#ifdef GPIOE_BASE
using GpioE = periph_wrapper_t<GPIOE_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_GPIOE>;
#endif
#ifdef GPIOF_BASE
using GpioF = periph_wrapper_t<GPIOF_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_GPIOF>;
#endif
#ifdef GPIOG_BASE
using GpioG = periph_wrapper_t<GPIOG_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_GPIOG>;
#endif

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

// --------------------------------------------------------------------------------------------------------------------

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
void     SET_MASK(GPIO_TypeDef& gpio, const uint32_t pin, const uint32_t mask);
void     CALCULATE_MASK(port_masks_t& masks, const uint32_t mode, const uint32_t size, const uint32_t offset);

} // namespace

// --------------------------------------------------------------------------------------------------------------------

template<typename PERIPH, uint32_t PIN>
struct GpioPin : public Peripheral<PERIPH, GPIO_TypeDef>
{
    using Peripheral<PERIPH, GPIO_TypeDef>::CTRL_STRUCT;

    ALWAYS_INLINE static void configure(const OutputMode mode, const GpioSpeed speed = GpioSpeed::Slow)
    {
        ClockControl<PERIPH>::MakeSureClockIsEnabled();

        SET_MASK(CTRL_STRUCT(), PIN, GET_MASK(mode, speed));
    }

    ALWAYS_INLINE static void configure(const InputMode mode)
    {
        ClockControl<PERIPH>::MakeSureClockIsEnabled();

        SET_MASK(CTRL_STRUCT(), PIN, GET_MASK(mode));

        if (mode == InputMode::PullDown)
            CTRL_STRUCT().BRR = (1 << PIN);
        else if (mode == InputMode::PullUp)
            CTRL_STRUCT().BSRR = (1 << PIN);
    }

    ALWAYS_INLINE static void set(bool val)
    {
        if (val)
            CTRL_STRUCT().BSRR = (1 << PIN);
        else
            CTRL_STRUCT().BRR = (1 << PIN);
    }

    ALWAYS_INLINE static bool get()
    {
        return (CTRL_STRUCT().IDR & (1 << PIN));
    }

    ALWAYS_INLINE static void toggle()
    {
        if (CTRL_STRUCT().ODR & (1 << PIN))
            CTRL_STRUCT().BRR = (1 << PIN);
        else
            CTRL_STRUCT().BSRR = (1 << PIN);
    }
};

template<typename PERIPH, uint32_t SIZE, uint32_t OFFSET = 0>
class GpioPort : private Peripheral<PERIPH, GPIO_TypeDef>
{
    using Peripheral<PERIPH, GPIO_TypeDef>::CTRL_STRUCT;

    static_assert(SIZE <= 16, "Max GpioPort<SIZE> is 16");
    static_assert((SIZE + OFFSET) <= 16, "GpioPort<OFFSET> is too high");

    // switching between I/O modes is highly time critical, therefore we are calculating the masks in advance
    static inline port_masks_t mInput;
    static inline port_masks_t mOutput;

  public:
    ALWAYS_INLINE static void write(uint32_t data)
    {
        uint32_t data_cli = (~data & MASK_FOR_N_BITS(SIZE));
        data &= MASK_FOR_N_BITS(SIZE);

        CTRL_STRUCT().BSRR = (data_cli << (OFFSET + 16)) | (data << OFFSET);
    }

    ALWAYS_INLINE static uint32_t read()
    {
        return (CTRL_STRUCT().IDR >> OFFSET) & MASK_FOR_N_BITS(SIZE);
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
        ClockControl<PERIPH>::MakeSureClockIsEnabled();

        CALCULATE_MASK(mOutput, GET_MASK(mode, speed), SIZE, OFFSET);
        setAsOutput();
    }

    static void setInputMode(const InputMode mode)
    {
        ClockControl<PERIPH>::MakeSureClockIsEnabled();

        CALCULATE_MASK(mInput, GET_MASK(mode), SIZE, OFFSET);
        setAsInput();
    }

    static void configure(const InputMode i_mode, const OutputMode o_mode, const GpioSpeed speed = GpioSpeed::Fast)
    {
        ClockControl<PERIPH>::MakeSureClockIsEnabled();

        CALCULATE_MASK(mInput, GET_MASK(i_mode), SIZE, OFFSET);
        CALCULATE_MASK(mOutput, GET_MASK(o_mode, speed), SIZE, OFFSET);
    }

  private:
    ALWAYS_INLINE static void applyMasks(const port_masks_t& m)
    {
        if (m.l_cli_mask_inverted != 0xFFFF)
            CTRL_STRUCT().CRL = (CTRL_STRUCT().CRL & m.l_cli_mask_inverted) | m.l_set_mask;

        if (m.h_cli_mask_inverted != 0xFFFF)
            CTRL_STRUCT().CRH = (CTRL_STRUCT().CRH & m.h_cli_mask_inverted) | m.h_set_mask;
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

void SET_MASK(GPIO_TypeDef& gpio, const uint32_t pin, const uint32_t mask)
{
    if (pin < 8)
        gpio.CRL = (gpio.CRL & ~(0b1111 << (4 * pin))) | (mask << (4 * pin));
    else
        gpio.CRH = (gpio.CRH & ~(0b1111 << (4 * (pin - 8)))) | (mask << (4 * (pin - 8)));
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
