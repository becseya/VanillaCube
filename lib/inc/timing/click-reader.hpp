#pragma once

#include "systick.hpp"

namespace VanillaCube {
namespace Timing {

enum class ClickType
{
    None,
    Short,
    Long,
};

static constexpr systick_t SHORTPRESS_TIME_MS = 30;
static constexpr systick_t LONGPRESS_TIME_MS  = 500;

class ClickReader
{
    systick_t timeOfRise;
    bool      signalPrev;
    bool      longClicked;
    ClickType click;

  public:
    ClickReader();

    bool      getSignal() const;
    bool      isLongCLicked() const;
    ClickType getClick() const;

    bool update(bool signal);
};

} // namespace Timing
} // namespace VanillaCube
