#pragma once

#include "../task.hpp"

#include "../../timing/periodic-timer.hpp"

namespace VanillaCube {
namespace Scheduling {
namespace NPE {

struct PeriodicTask : public Task, public Timing::PeriodicTimer
{
    bool readyLatch;

  public:
    PeriodicTask(const char* name, unsigned priority, uint32_t period, loop_function_t loop_function);

    void execute() override;
    bool isReady() override;
};

// --------------------------------------------------------------------------------------------------------------------

class DependentTask : public Task
{
    Task&             taskToDependOn;
    info_t::counter_t lastCounter;

  public:
    DependentTask(const char* name, unsigned priority, Task& parent_task, loop_function_t loop_function);

    void execute() override;
    bool isReady() override;
};

} // namespace NPE
} // namespace Scheduling
} // namespace VanillaCube
