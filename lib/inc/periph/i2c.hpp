#pragma once

#include "gpio.hpp"
#include "peripheral.hpp"

#include "../timing/systick.hpp"

#include <stm32f1xx_ll_i2c.h>

namespace VanillaCube {
namespace Periph {
namespace I2c {

#ifdef I2C1_BASE
using Bus1 = periph_wrapper_t<I2C1_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_I2C1>;
#endif
#ifdef I2C2_BASE
using Bus2 = periph_wrapper_t<I2C2_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_I2C2>;
#endif

enum class AddrMode : uint32_t
{
    WR = 0, // Write
    RD = 1, // Read
};

enum class Result : int32_t
{
    Success            = 0,
    Fail_BusIsBusy     = -1,
    Fail_Start         = -2,
    Fail_RepeatedStart = -3,
    Fail_SendAddrRD    = -4,
    Fail_SendAddrWr    = -5,
    Fail_TxData        = -6,
    Fail_RxData        = -7,
};

bool operator!(const Result& result)
{
    return (result != Result::Success);
}

template<typename PERIPH>
struct Bus : public Peripheral<PERIPH, I2C_TypeDef>
{
    using Peripheral<PERIPH, I2C_TypeDef>::CTRL_STRUCT;

    template<typename GPIO_PIN>
    static void connectPin()
    {
        GPIO_PIN::configure(OutputMode::AltFuncOD, GpioSpeed::Fast);
        GPIO_PIN::set(true); // pull-up
    }

    static void configure(uint32_t clock_speed = 400000)
    {
        ClockControl<PERIPH>::MakeSureClockIsEnabled();

        LL_I2C_InitTypeDef init_params = { 0 };

        LL_I2C_DeInit(&CTRL_STRUCT());
        LL_I2C_DisableOwnAddress2(&CTRL_STRUCT());
        LL_I2C_DisableGeneralCall(&CTRL_STRUCT());
        LL_I2C_EnableClockStretching(&CTRL_STRUCT());

        init_params.PeripheralMode  = LL_I2C_MODE_I2C;
        init_params.ClockSpeed      = clock_speed;
        init_params.DutyCycle       = LL_I2C_DUTYCYCLE_2;
        init_params.OwnAddress1     = 0;
        init_params.TypeAcknowledge = LL_I2C_ACK;
        init_params.OwnAddrSize     = LL_I2C_OWNADDRESS1_7BIT;

        LL_I2C_Init(&CTRL_STRUCT(), &init_params);
        LL_I2C_SetOwnAddress2(&CTRL_STRUCT(), 0);
        LL_I2C_Enable(&CTRL_STRUCT());
    }

    // ---

    static ALWAYS_INLINE Result generateStart(bool repeated = false)
    {
        if (!repeated) {
            // wait until busy flag is cleared
            if (WAIT_FOR_FLAG([&]() -> bool { return !LL_I2C_IsActiveFlag_BUSY(&CTRL_STRUCT()); }))
                return Result::Fail_BusIsBusy;

            // disable POS (master mode)
            LL_I2C_DisableBitPOS(&CTRL_STRUCT());
        }

        // generate START
        LL_I2C_GenerateStartCondition(&CTRL_STRUCT());
        if (WAIT_FOR_FLAG([&]() -> bool { return LL_I2C_IsActiveFlag_SB(&CTRL_STRUCT()); }))
            return (repeated ? Result::Fail_RepeatedStart : Result::Fail_Start);

        return Result::Success;
    }

    static ALWAYS_INLINE void generateStop()
    {
        LL_I2C_GenerateStopCondition(&CTRL_STRUCT());
    }

    static ALWAYS_INLINE void nextDataACK()
    {
        LL_I2C_AcknowledgeNextData(&CTRL_STRUCT(), LL_I2C_ACK);
    }

    static ALWAYS_INLINE void nextDataNACK()
    {
        LL_I2C_AcknowledgeNextData(&CTRL_STRUCT(), LL_I2C_NACK);
    }

    // ---

    static ALWAYS_INLINE Result sendAddress(AddrMode mode, uint32_t address_7bit)
    {
        bool ack_fail = false;

        // send device address
        CTRL_STRUCT().DR = ((address_7bit << 1) | Util::ENUM_TO_UINT(mode));

        if (WAIT_FOR_FLAG([&]() -> bool {
                if (LL_I2C_IsActiveFlag_AF(&CTRL_STRUCT())) { // ACK Failure
                    LL_I2C_ClearFlag_AF(&CTRL_STRUCT());
                    ack_fail = true;
                }
                return (ack_fail || LL_I2C_IsActiveFlag_ADDR(&CTRL_STRUCT()));
            }) ||
            ack_fail)
            return ((mode == AddrMode::RD) ? Result::Fail_SendAddrRD : Result::Fail_SendAddrWr);

        LL_I2C_ClearFlag_ADDR(&CTRL_STRUCT());
        return Result::Success;
    }

    static ALWAYS_INLINE Result sendData(uint32_t data)
    {
        bool ack_fail = false;

        // send data
        CTRL_STRUCT().DR = data;

        if (WAIT_FOR_FLAG([&]() -> bool {
                if (LL_I2C_IsActiveFlag_AF(&CTRL_STRUCT())) { // ACK Failure
                    LL_I2C_ClearFlag_AF(&CTRL_STRUCT());
                    ack_fail = true;
                }
                return (ack_fail || LL_I2C_IsActiveFlag_TXE(&CTRL_STRUCT()));
            }) ||
            ack_fail)
            return Result::Fail_TxData;

        return Result::Success;
    }

    template<typename T>
    static ALWAYS_INLINE Result receiveData(T& data)
    {
        if (WAIT_FOR_FLAG([&]() -> bool { return LL_I2C_IsActiveFlag_RXNE(&CTRL_STRUCT()); }))
            return Result::Fail_RxData;

        data = CTRL_STRUCT().DR;
        return Result::Success;
    }

  private:
    template<typename F>
    static ALWAYS_INLINE bool WAIT_FOR_FLAG(const F& flag_is_true)
    {
        uint32_t TIMEOUT_TICKS = 30;
        return Timing::WAIT_FOR_FLAG(flag_is_true, TIMEOUT_TICKS);
    }
};

} // namespace I2c
} // namespace Periph
} // namespace VanillaCube
