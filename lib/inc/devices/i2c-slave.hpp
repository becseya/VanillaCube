#pragma once

#include "../math.h"
#include "../periph/i2c.hpp"

namespace VanillaCube {
namespace Devices {

using Periph::I2c::Result;

template<typename I2C_BUS>
class I2cSlave
{
    using AddrMode = Periph::I2c::AddrMode;

    const uint32_t deviceAddress;
    const bool     internalAddressIs16Bit;

  protected:
    Result st;

  public:
    I2cSlave(uint32_t device_address, bool internal_addr_16b)
        : deviceAddress(device_address)
        , internalAddressIs16Bit(internal_addr_16b)
        , st(Result::Success)
    {}

    const Result& getLastResult()
    {
        return st;
    }

    bool ping(AddrMode mode = AddrMode::RD)
    {
        startTransaction(mode);
        endTransaction();

        return (st == Result::Success);
    }

    const Result& writeSingleRegister(uint16_t internal_addr, uint8_t value)
    {
        return write(internal_addr, &value, 1);
    }

    const Result& write(uint16_t internal_addr, const uint8_t* p_data, size_t data_size)
    {
        while (true) {
            // START and set internal memory address
            if (!startInternalTransaction(internal_addr))
                break;

            sendData(p_data, data_size);
            break;
        }

        endTransaction();
        return st;
    }

    const Result& read(uint16_t internal_addr, uint8_t* p_buffer, size_t read_size)
    {
        while (true) {
            // START and set internal memory address
            if (!startInternalTransaction(internal_addr))
                break;

            // generate Repeated START
            if (!startTransaction(AddrMode::RD, true))
                break;

            recvData(p_buffer, read_size);
            break;
        }

        endTransaction();
        return st;
    }

  protected:
    const Result& sendData(const uint8_t* p_data, uint8_t data_size)
    {
        // write data
        while (data_size > 0) {
            if (!(st = I2C_BUS::sendData(*p_data)))
                break;

            p_data++;
            data_size--;
        }

        return st;
    }

    const Result& recvData(uint8_t* p_buffer, uint8_t read_size)
    {
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

        return st;
    }

    // ---

    void endTransaction()
    {
        I2C_BUS::generateStop();
    }

    const Result& startTransaction(AddrMode mode, bool repeated_start = false)
    {
        // generate START
        if (!(st = I2C_BUS::generateStart(repeated_start)))
            return st;

        // send device address
        if (!(st = I2C_BUS::sendAddress(mode, deviceAddress)))
            return st;

        return st;
    }

    const Result& startInternalTransaction(uint16_t internal_addr)
    {
        if (!startTransaction(AddrMode::WR))
            return st;

        if (internalAddressIs16Bit) {
            // send external address MSB
            if (!(st = I2C_BUS::sendData((internal_addr >> 8) & 0xFF)))
                return st;
        }

        // send external address LSB
        if (!(st = I2C_BUS::sendData(internal_addr & 0xFF)))
            return st;

        return st;
    }

    const Result& waitUntilReadyForTransaction(uint32_t timeout_ticks, AddrMode mode)
    {
        st = Result::Success;

        if (!Timing::WAIT_FOR_FLAG([&]() -> bool { return ping(mode); }, timeout_ticks))
            st = Result::Fail_DeviceIsBusy;

        return st;
    }
};

// --------------------------------------------------------------------------------------------------------------------

template<typename I2C_BUS, size_t PAGE_SIZE = 64>
struct I2cEEPROM : public I2cSlave<I2C_BUS>
{
    using I2cSlave<I2C_BUS>::st;
    using I2cSlave<I2C_BUS>::read;
    using I2cSlave<I2C_BUS>::write;
    using I2cSlave<I2C_BUS>::waitUntilReadyForTransaction;
    using AddrMode = Periph::I2c::AddrMode;

    static constexpr uint32_t BURST_WRITE_TIMEOUT = 10; // systicks

  public:
    I2cEEPROM(uint32_t device_address)
        : I2cSlave<I2C_BUS>(device_address, true)
    {}

    Result readBurst(size_t page_num, uint8_t* p_buffer, size_t read_size)
    {
        do {
            const size_t n = Math::min(PAGE_SIZE, read_size);

            if (!read(page_num * PAGE_SIZE, p_buffer, n))
                break;

            read_size -= n;
            p_buffer += n;
            page_num++;
        } while (read_size > 0);

        return st;
    }

    Result writeBurst(uint16_t page_num, const uint8_t* p_data, size_t data_size)
    {
        do {
            const size_t n = Math::min(PAGE_SIZE, data_size);

            if (!waitUntilReadyForTransaction(BURST_WRITE_TIMEOUT, AddrMode::WR))
                break;

            if (!write(page_num * PAGE_SIZE, p_data, n))
                break;

            data_size -= n;
            p_data += n;
            page_num++;
        } while (data_size > 0);

        return st;
    }
};

} // namespace Devices
} // namespace VanillaCube
