#pragma once

#include "systick.hpp"

namespace VanillaCube {
namespace Timing {

class Pulser
{
    systick_t lastUpdate;
    bool      signal;
    uint32_t  tHigh;
    uint32_t  tLow;
    int       numberOfPulses; // -1 means repeating infinitely

  public:
    Pulser();

    bool isRunning() const;

    bool update();
    void stop();

    void pulse(uint32_t pulse_width);
    void pulse(uint32_t high, uint32_t low, int number_of_pulses);

  protected:
    virtual void handleSignalChange(bool new_signal);
};

} // namespace Timing
} // namespace VanillaCube
