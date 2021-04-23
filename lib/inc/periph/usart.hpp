#pragma once

#include "gpio.hpp"
#include "peripheral.hpp"

#include <stm32f1xx_ll_usart.h>

namespace VanillaCube {
namespace Periph {

#ifdef USART1_BASE
using Usart1 = periph_wrapper_t<USART1_BASE, Bus::APB2, LL_APB2_GRP1_PERIPH_USART1>;
#endif
#ifdef USART2_BASE
using Usart2 = periph_wrapper_t<USART2_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_USART2>;
#endif
#ifdef USART3_BASE
using Usart3 = periph_wrapper_t<USART3_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_USART3>;
#endif

enum class UsartMode : uint32_t
{
    Rx   = LL_USART_DIRECTION_RX,
    Tx   = LL_USART_DIRECTION_TX,
    TxRx = LL_USART_DIRECTION_TX_RX,
};

enum class UsartParity : uint32_t
{
    None = LL_USART_PARITY_NONE,
    Even = LL_USART_PARITY_EVEN,
    Odd  = LL_USART_PARITY_ODD,
};

enum class UsartInterrupt
{
    ParityError,
    TxEmpty,
    TxComplete,
    RxNotEmpty,
};

template<typename PERIPH>
struct Usart : public Peripheral<PERIPH, USART_TypeDef>
{
    using Peripheral<PERIPH, USART_TypeDef>::CTRL_STRUCT;

    template<typename GPIO_PIN>
    static void connectTxPin()
    {
        GPIO_PIN::configure(OutputMode::AltFuncPP, GpioSpeed::Fast);
    }

    template<typename GPIO_PIN>
    static void connectRxPin()
    {
        GPIO_PIN::configure(InputMode::Floating);
    }

    ALWAYS_INLINE static void configureAsync(uint32_t baud, UsartMode mode, UsartParity parity = UsartParity::None)
    {
        LL_USART_InitTypeDef init_params = { 0 };

        ClockControl<PERIPH>::MakeSureClockIsEnabled();

        LL_USART_DeInit(&CTRL_STRUCT());

        init_params.BaudRate            = baud;
        init_params.DataWidth           = ((parity == UsartParity::None) ? LL_USART_DATAWIDTH_8B : LL_USART_DATAWIDTH_9B);
        init_params.StopBits            = LL_USART_STOPBITS_1;
        init_params.Parity              = Util::ENUM_TO_UINT(parity);
        init_params.TransferDirection   = Util::ENUM_TO_UINT(mode);
        init_params.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
        init_params.OverSampling        = LL_USART_OVERSAMPLING_16;

        LL_USART_Init(&CTRL_STRUCT(), &init_params);
        LL_USART_ConfigAsyncMode(&CTRL_STRUCT());
        LL_USART_Enable(&CTRL_STRUCT());
    }

    // ---

    ALWAYS_INLINE static void setInterrupt(UsartInterrupt interrupt, bool enabled)
    {
        if (enabled) {
            switch (interrupt) {
            case UsartInterrupt::ParityError: LL_USART_EnableIT_PE(&CTRL_STRUCT()); break;
            case UsartInterrupt::RxNotEmpty: LL_USART_EnableIT_RXNE(&CTRL_STRUCT()); break;
            case UsartInterrupt::TxComplete: LL_USART_EnableIT_TC(&CTRL_STRUCT()); break;
            case UsartInterrupt::TxEmpty: LL_USART_EnableIT_TXE(&CTRL_STRUCT()); break;
            }
        } else {
            switch (interrupt) {
            case UsartInterrupt::ParityError: LL_USART_DisableIT_PE(&CTRL_STRUCT()); break;
            case UsartInterrupt::RxNotEmpty: LL_USART_DisableIT_RXNE(&CTRL_STRUCT()); break;
            case UsartInterrupt::TxComplete: LL_USART_DisableIT_TC(&CTRL_STRUCT()); break;
            case UsartInterrupt::TxEmpty: LL_USART_DisableIT_TXE(&CTRL_STRUCT()); break;
            }
        }
    }

    ALWAYS_INLINE static bool isReceiveFull()
    {
        return (CTRL_STRUCT().SR & USART_SR_RXNE);
    }

    ALWAYS_INLINE static bool isTxEmptry()
    {
        return (CTRL_STRUCT().SR & USART_SR_TXE);
    }

    ALWAYS_INLINE static bool isTxComplete()
    {
        return (CTRL_STRUCT().SR & USART_SR_TC);
    }

    // ---

    ALWAYS_INLINE static uint8_t read()
    {
        return CTRL_STRUCT().DR;
    }

    ALWAYS_INLINE static void sendChar(const char c)
    {
        while (!isTxEmptry())
            ;

        CTRL_STRUCT().DR = c;
    }

    static void sendString(const char* str)
    {
        while (*str) {
            sendChar(*str);
            str++;
        }
    }
};

} // namespace Periph
} // namespace VanillaCube
