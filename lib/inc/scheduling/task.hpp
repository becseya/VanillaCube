#pragma once

#include "../lib.h"

namespace VanillaCube {
namespace Scheduling {

using loop_function_t = void (*)();

struct Task
{
    struct info_t
    {
        using counter_t = uint32_t;

        const char* name;
        counter_t   runCntr;
        counter_t   runTime;
    };

  private:
    loop_function_t fLoop;
    uint8_t         priority;
    info_t          info;

  public:
    Task(const char* name, unsigned priority, loop_function_t loop_function);

    unsigned      getPriority() const;
    const info_t& getInfo() const;
    bool          isHigherPriority(const Task& other) const;

    virtual void execute();
    virtual bool isReady() = 0;
};

} // namespace Scheduling
} // namespace VanillaCube
