#pragma once

#include "../lib.h"

namespace VanillaCube {
namespace Scheduling {

struct TaskInfo
{
    using counter_t = uint32_t;

    const char* name;
    counter_t   runCntr;
    counter_t   runTime;
    float       load;
    counter_t   lastRunTime;

    float getAverageRuntime() const;
    void  updateLoad(counter_t passed_since_last_update);
};

// --------------------------------------------------------------------------------------------------------------------

using loop_function_t = void (*)();

class Task
{
    loop_function_t fLoop;
    uint8_t         priority;
    TaskInfo        info;

  public:
    Task(const char* name, unsigned priority, loop_function_t loop_function);

    unsigned        getPriority() const;
    const TaskInfo& getInfo() const;
    bool            isHigherPriority(const Task& other) const;

    void updateLoad(TaskInfo::counter_t passed_since_last_update);

    virtual void execute();
    virtual bool isReady() = 0;
};

} // namespace Scheduling
} // namespace VanillaCube
