#pragma once

#include "../periph/i2c.hpp"

namespace VanillaCube {
namespace Devices {

using Periph::I2c::Result;

template<typename I2C_BUS>
class I2cSlave
{
  protected:
    using AddrMode = Periph::I2c::AddrMode;

    const uint32_t deviceAddress;
    const bool     externalAddressIs16Bit;
    Result         st;

  public:
    I2cSlave(uint32_t device_address, bool ext_addr_16)
        : deviceAddress(device_address)
        , externalAddressIs16Bit(ext_addr_16)
    {}

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

    const Result& startExternalTransaction(uint32_t device_addr, uint16_t external_addr)
    {
        // generate START
        if (!(st = I2C_BUS::generateStart()))
            return st;

        // send device address
        if (!(st = I2C_BUS::sendAddress(AddrMode::WR, device_addr)))
            return st;

        if (externalAddressIs16Bit) {
            // send external address MSB
            if (!(st = I2C_BUS::sendData((external_addr >> 8) & 0xFF)))
                return st;
        }

        // send external address LSB
        if (!(st = I2C_BUS::sendData(external_addr & 0xFF)))
            return st;

        return st;
    }
};

// --------------------------------------------------------------------------------------------------------------------

template<typename I2C_BUS>
struct I2cEEPROM : public I2cSlave<I2C_BUS>
{
    using I2cSlave<I2C_BUS>::st;
    using I2cSlave<I2C_BUS>::deviceAddress;
    using AddrMode = Periph::I2c::AddrMode;

  public:
    I2cEEPROM(uint32_t device_address)
        : I2cSlave<I2C_BUS>(device_address, true)
    {}

    Result write(uint16_t mem_addr, const uint8_t* p_data, uint8_t data_size)
    {
        while (true) {
            // START and set internal memory address
            if (!I2cSlave<I2C_BUS>::startExternalTransaction(deviceAddress, mem_addr))
                break;

            I2cSlave<I2C_BUS>::sendData(p_data, data_size);
            break;
        }

        // generate STOP
        I2C_BUS::generateStop();

        return st;
    }

    Result read(uint16_t mem_addr, uint8_t* p_buffer, uint8_t read_size)
    {
        while (true) {
            // START and set internal memory address
            if (!(st = I2cSlave<I2C_BUS>::startExternalTransaction(deviceAddress, mem_addr)))
                break;

            // generate Repeated START
            if (!(st = I2C_BUS::generateStart(true)))
                break;

            // send device address
            if (!(st = I2C_BUS::sendAddress(AddrMode::RD, deviceAddress)))
                break;

            I2cSlave<I2C_BUS>::recvData(p_buffer, read_size);
            break;
        }

        // generate STOP
        I2C_BUS::generateStop();

        return st;
    }
};

} // namespace Devices
} // namespace VanillaCube
