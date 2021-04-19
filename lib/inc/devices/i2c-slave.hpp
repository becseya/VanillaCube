#pragma once

#include "../periph/i2c.hpp"

namespace VanillaCube {
namespace Devices {

using Periph::I2c::Result;

template<typename I2C_BUS>
struct I2cSlave
{
    using AddrMode = Periph::I2c::AddrMode;

    uint32_t deviceAddress;

  public:
    I2cSlave(uint32_t device_address)
        : deviceAddress(device_address)
    {}

    Result writeToMemAddress(uint16_t mem_addr, const uint8_t* p_data, uint8_t data_size)
    {
        Result st;

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

    Result readFromMemAddress(uint16_t mem_addr, uint8_t* p_buffer, uint8_t read_size)
    {
        Result st;

        while (true) {
            // START and set internal memory address
            if (!(st = startTransaction(deviceAddress, mem_addr)))
                break;

            // generate Repeated START
            if (!(st = I2C_BUS::generateStart(true)))
                break;

            // send device address
            if (!(st = I2C_BUS::sendAddress(AddrMode::RD, deviceAddress)))
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
    static Result startTransaction(uint32_t device_address, uint16_t mem_addr)
    {
        Result st;

        // generate START
        if (!(st = I2C_BUS::generateStart()))
            return st;

        // send device address
        if (!(st = I2C_BUS::sendAddress(AddrMode::WR, device_address)))
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

} // namespace Devices
} // namespace VanillaCube
