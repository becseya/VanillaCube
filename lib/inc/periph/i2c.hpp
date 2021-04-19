#pragma once

#include "gpio.hpp"
#include "peripheral.hpp"

#include "../timing/systick.hpp"

#include <stm32f1xx_ll_i2c.h>

namespace VanillaCube {
namespace Periph {

#ifdef I2C1_BASE
using I2CBus1 = periph_wrapper_t<I2C1_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_I2C1>;
#endif
#ifdef I2C2_BASE
using I2CBus2 = periph_wrapper_t<I2C2_BASE, Bus::APB1, LL_APB1_GRP1_PERIPH_I2C2>;
#endif

enum class I2CAddrMode : uint32_t
{
    WR = 0, // Write
    RD = 1, // Read
};

enum class I2CResult : int32_t
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

bool operator!(const I2CResult& result)
{
    return (result != I2CResult::Success);
}

template<typename PERIPH>
struct I2C : public Peripheral<PERIPH, I2C_TypeDef>
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

    static ALWAYS_INLINE I2CResult generateStart(bool repeated = false)
    {
        if (!repeated) {
            // wait until busy flag is cleared
            if (WAIT_FOR_FLAG([&]() -> bool { return !LL_I2C_IsActiveFlag_BUSY(&CTRL_STRUCT()); }))
                return I2CResult::Fail_BusIsBusy;

            // disable POS (master mode)
            LL_I2C_DisableBitPOS(&CTRL_STRUCT());
        }

        // generate START
        LL_I2C_GenerateStartCondition(&CTRL_STRUCT());
        if (WAIT_FOR_FLAG([&]() -> bool { return LL_I2C_IsActiveFlag_SB(&CTRL_STRUCT()); }))
            return (repeated ? I2CResult::Fail_RepeatedStart : I2CResult::Fail_Start);

        return I2CResult::Success;
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

    static ALWAYS_INLINE I2CResult sendAddress(I2CAddrMode mode, uint32_t address_7bit)
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
            return ((mode == I2CAddrMode::RD) ? I2CResult::Fail_SendAddrRD : I2CResult::Fail_SendAddrWr);

        LL_I2C_ClearFlag_ADDR(&CTRL_STRUCT());
        return I2CResult::Success;
    }

    static ALWAYS_INLINE I2CResult sendData(uint32_t data)
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
            return I2CResult::Fail_TxData;

        return I2CResult::Success;
    }

    template<typename T>
    static ALWAYS_INLINE I2CResult receiveData(T& data)
    {
        if (WAIT_FOR_FLAG([&]() -> bool { return LL_I2C_IsActiveFlag_RXNE(&CTRL_STRUCT()); }))
            return I2CResult::Fail_RxData;

        data = CTRL_STRUCT().DR;
        return I2CResult::Success;
    }

  private:
    template<typename F>
    static ALWAYS_INLINE bool WAIT_FOR_FLAG(const F& flag_is_true)
    {
        uint32_t TIMEOUT_TICKS = 30;
        return Timing::WAIT_FOR_FLAG(flag_is_true, TIMEOUT_TICKS);
    }
};

template<typename I2C_BUS>
struct I2CDevice
{
    uint32_t deviceAddress;

  public:
    I2CDevice(uint32_t device_address)
        : deviceAddress(device_address)
    {}

    I2CResult writeToMemAddress(uint16_t mem_addr, const uint8_t* p_data, uint8_t data_size)
    {
        I2CResult st;

        while (true) {
            // START and set internal memory address
            if (!(st = startTransaction(deviceAddress, mem_addr)))
                break;

            // write data
            while (data_size > 0) {
                if (!(st = I2C_BUS::sendData(*p_data)))
                    break;

                p_data++;
                data_size--;
            }

            break;
        }

        // generate STOP
        I2C_BUS::generateStop();

        return st;
    }

    I2CResult readFromMemAddress(uint16_t mem_addr, uint8_t* p_buffer, uint8_t read_size)
    {
        I2CResult st;

        while (true) {
            // START and set internal memory address
            if (!(st = startTransaction(deviceAddress, mem_addr)))
                break;

            // generate Repeated START
            if (!(st = I2C_BUS::generateStart(true)))
                break;

            // send device address
            if (!(st = I2C_BUS::sendAddress(I2CAddrMode::RD, deviceAddress)))
                break;

            // set ACK to be send after each receive
            if (read_size > 1)
                I2C_BUS::nextDataACK();

            // read data
            while (read_size > 0) {
                if (read_size == 1)
                    I2C_BUS::nextDataNACK(); // end of reception

                if (!(st = I2C_BUS::receiveData(*p_buffer)))
                    break;

                p_buffer++;
                read_size--;
            }

            break;
        }

        // generate STOP
        I2C_BUS::generateStop();

        return st;
    }

  private:
    static I2CResult startTransaction(uint32_t device_address, uint16_t mem_addr)
    {
        I2CResult st;

        // generate START
        if (!(st = I2C_BUS::generateStart()))
            return st;

        // send device address
        if (!(st = I2C_BUS::sendAddress(I2CAddrMode::WR, device_address)))
            return st;

        // send memory address MSB
        if (!(st = I2C_BUS::sendData((mem_addr >> 8) & 0xFF)))
            return st;

        // send memory address LSB
        if (!(st = I2C_BUS::sendData(mem_addr & 0xFF)))
            return st;

        return st;
    }
};

} // namespace Periph
} // namespace VanillaCube
