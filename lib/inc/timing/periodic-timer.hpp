#pragma once

#include "systick.hpp"

namespace VanillaCube {
namespace Timing {

class PeriodicTimer
{
    systick_t lastUpdate;
    uint32_t  period;
    bool      singleShot;

  public:
    PeriodicTimer(uint32_t period, bool single_shot = false);

    void init(uint32_t period, bool single_shot = false);
    bool execute();
    void reset();
};

} // namespace Timing
} // namespace VanillaCube
