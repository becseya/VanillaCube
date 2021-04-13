#include <timing/pulser.hpp>

using namespace VanillaCube::Timing;

// --------------------------------------------------------------------------------------------------------------------

Pulser::Pulser()
    : lastUpdate()
    , signal()
    , tHigh()
    , tLow()
    , numberOfPulses()
{}

bool Pulser::isRunning() const
{
    return (numberOfPulses != 0);
}

void Pulser::stop()
{
    if (signal)
        handleSignalChange(false);

    signal         = false;
    numberOfPulses = 0;
}

bool Pulser::update()
{
    if (isRunning()) {
        if (signal && (getSystickSince(lastUpdate) >= tHigh)) {
            signal = false;
            handleSignalChange(signal);
            lastUpdate += tHigh;

            if (numberOfPulses > 0)
                numberOfPulses--;

        } else if (!signal && (getSystickSince(lastUpdate) >= tLow)) {
            signal = true;
            handleSignalChange(signal);
            lastUpdate += tLow;
        }
    }

    return signal;
}

// --------------------------------------------------------------------------------------------------------------------

void Pulser::pulse(uint32_t pulse_width)
{
    pulse(pulse_width, 0, 1);
}

void Pulser::pulse(uint32_t high, uint32_t low, int number_of_pulses)
{
    if (!signal)
        handleSignalChange(true);

    signal         = true;
    numberOfPulses = number_of_pulses;
    tHigh          = high;
    tLow           = low;
    lastUpdate     = getSysick();
}

// --------------------------------------------------------------------------------------------------------------------

void Pulser::handleSignalChange(bool /*new_signal*/) {}
