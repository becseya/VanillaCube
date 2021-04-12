#include <timing/periodic-timer.hpp>

using namespace VanillaCube::Timing;

// --------------------------------------------------------------------------------------------------------------------

PeriodicTimer::PeriodicTimer(uint32_t period, bool single_shot)
    : lastUpdate(getSysick())
    , period(period)
    , singleShot(single_shot)
{}

void PeriodicTimer::init(uint32_t period, bool single_shot)
{
    PeriodicTimer::period     = period;
    PeriodicTimer::singleShot = single_shot;
    reset();
}

bool PeriodicTimer::execute()
{
    bool passed = (getSystickSince(lastUpdate) >= period);

    if (passed && !singleShot)
        reset();

    return passed;
}

void PeriodicTimer::reset()
{
    lastUpdate = getSysick();
}
