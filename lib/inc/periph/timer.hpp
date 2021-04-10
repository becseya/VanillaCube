#pragma once

#include "peripheral.hpp"

#include <stm32f1xx_ll_tim.h>

namespace VanillaCube {
namespace Periph {

#ifdef TIM1_BASE
using Timer1 = periph_wrapper_t<TIM1_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_TIM1>;
#endif
#ifdef TIM2_BASE
using Timer2 = periph_wrapper_t<TIM2_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_TIM2>;
#endif
#ifdef TIM3_BASE
using Timer3 = periph_wrapper_t<TIM3_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_TIM3>;
#endif
#ifdef TIM4_BASE
using Timer4 = periph_wrapper_t<TIM4_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_TIM4>;
#endif
#ifdef TIM5_BASE
using Timer5 = periph_wrapper_t<TIM5_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_TIM5>;
#endif
#ifdef TIM6_BASE
using Timer6 = periph_wrapper_t<TIM6_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_TIM6>;
#endif
#ifdef TIM7_BASE
using Timer7 = periph_wrapper_t<TIM7_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_TIM7>;
#endif

enum class TimerChannel_t : uint32_t
{
    Ch1 = LL_TIM_CHANNEL_CH1,
    Ch2 = LL_TIM_CHANNEL_CH2,
    Ch3 = LL_TIM_CHANNEL_CH3,
    Ch4 = LL_TIM_CHANNEL_CH4,
};

enum class TimerOcMode : uint32_t
{
    PWM            = LL_TIM_OCMODE_PWM1,
    Toggle         = LL_TIM_OCMODE_TOGGLE,
    ForcedActive   = LL_TIM_OCMODE_FORCED_ACTIVE,
    ForcedInactive = LL_TIM_OCMODE_FORCED_INACTIVE,
};

// --------------------------------------------------------------------------------------------------------------------

namespace { // helper functions inside anonymous namespace

static constexpr uint32_t CH_ALL = (LL_TIM_CHANNEL_CH1 | LL_TIM_CHANNEL_CH2 | LL_TIM_CHANNEL_CH3 | LL_TIM_CHANNEL_CH4);

} // namespace

// --------------------------------------------------------------------------------------------------------------------

template<typename PERIPH>
struct Timer : public Peripheral<PERIPH, TIM_TypeDef>
{
    using Peripheral<PERIPH, TIM_TypeDef>::CTRL_STRUCT;

    static void configure(uint32_t period, uint32_t prescaler = 0)
    {
        LL_TIM_InitTypeDef init_params = { 0 };

        ClockControl<PERIPH>::MakeSureClockIsEnabled();

        init_params.Prescaler     = prescaler;
        init_params.Autoreload    = period;
        init_params.CounterMode   = LL_TIM_COUNTERMODE_UP;
        init_params.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;

        LL_TIM_Init(&CTRL_STRUCT(), &init_params);
    }

    static void startOc(TimerChannel_t ch, TimerOcMode mode, uint32_t compare_value = 0)
    {
        LL_TIM_OC_InitTypeDef init_params = { 0 };

        init_params.OCMode       = ENUM_TO_UINT(mode);
        init_params.OCState      = LL_TIM_OCSTATE_DISABLE;
        init_params.OCNState     = LL_TIM_OCSTATE_DISABLE;
        init_params.CompareValue = compare_value;
        init_params.OCPolarity   = LL_TIM_OCPOLARITY_HIGH;

        LL_TIM_OC_Init(&CTRL_STRUCT(), ENUM_TO_UINT(ch), &init_params);
        setEnable(true, ch);
    }

    static void stop(TimerChannel_t ch)
    {
        setEnable(false, ch);
    }

    static void setOcMode(TimerChannel_t ch, TimerOcMode mode)
    {
        LL_TIM_OC_SetMode(&CTRL_STRUCT(), ENUM_TO_UINT(ch), ENUM_TO_UINT(mode));
    }

    static ALWAYS_INLINE void setComparator(TimerChannel_t ch, uint32_t val)
    {
        switch (ch) {
        case TimerChannel_t::Ch1: LL_TIM_OC_SetCompareCH1(&CTRL_STRUCT(), val); break;
        case TimerChannel_t::Ch2: LL_TIM_OC_SetCompareCH2(&CTRL_STRUCT(), val); break;
        case TimerChannel_t::Ch3: LL_TIM_OC_SetCompareCH3(&CTRL_STRUCT(), val); break;
        case TimerChannel_t::Ch4: LL_TIM_OC_SetCompareCH4(&CTRL_STRUCT(), val); break;
        default: break;
        }
    }

    static ALWAYS_INLINE void setDutyCycle(TimerChannel_t ch, float val)
    {
        setComparator(ch, CTRL_STRUCT().ARR * val);
    }

  private:
    static void setEnable(bool val, TimerChannel_t ch)
    {
        if (val) {
            LL_TIM_CC_EnableChannel(&CTRL_STRUCT(), ENUM_TO_UINT(ch));
            LL_TIM_EnableCounter(&CTRL_STRUCT());
        } else {
            LL_TIM_CC_DisableChannel(&CTRL_STRUCT(), ENUM_TO_UINT(ch));

            if ((CTRL_STRUCT().CCER & CH_ALL) == 0UL)
                LL_TIM_DisableCounter(&CTRL_STRUCT());
        }
    }
};

// --------------------------------------------------------------------------------------------------------------------

template<typename PERIPH, TimerChannel_t CH>
struct TimerChannel : protected Timer<PERIPH>
{
    static void startOc(TimerOcMode mode, uint32_t compare_value = 0)
    {
        Timer<PERIPH>::startOc(CH, mode, compare_value);
    }

    static void stop()
    {
        Timer<PERIPH>::stop(CH);
    }

    static void setOcMode(TimerOcMode mode)
    {
        Timer<PERIPH>::setOcMode(CH, mode);
    }

    static ALWAYS_INLINE void setComparator(uint32_t val)
    {
        Timer<PERIPH>::setComparator(CH, val);
    }

    static ALWAYS_INLINE void setDutyCycle(float val)
    {
        Timer<PERIPH>::setDutyCycle(CH, val);
    }
};

} // namespace Periph
} // namespace VanillaCube
