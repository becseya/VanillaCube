#include <timing/click-reader.hpp>

using namespace VanillaCube::Timing;

// --------------------------------------------------------------------------------------------------------------------

ClickReader::ClickReader()
    : timeOfRise(0)
    , signalPrev(false)
    , longClicked(false)
    , click(ClickType::None)
{}

bool ClickReader::getSignal() const
{
    return signalPrev;
}

ClickType ClickReader::getClick() const
{
    return click;
}

bool ClickReader::isLongCLicked() const
{
    return longClicked;
}

ClickType ClickReader::update(bool signal)
{
    click = ClickType::None; // return different only on state change

    // rise
    if (signal && !signalPrev) {
        timeOfRise  = getSysick();
        longClicked = false;
    }
    // long timeout
    else if (signal && !longClicked) {
        if (getSystickSince(timeOfRise) >= LONGPRESS_TIME_MS) {
            click       = ClickType::Long;
            longClicked = true;
        }
    }
    // fall
    else if (!signal && signalPrev) {
        if ((getSystickSince(timeOfRise) >= SHORTPRESS_TIME_MS) && !longClicked)
            click = ClickType::Short;
    }

    signalPrev = signal;
    return click;
}
