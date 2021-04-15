#pragma once

#include "../scheduler.hpp"

namespace VanillaCube {
namespace Scheduling {
namespace NPE {

// Non-preemptive Scheduling Algorithms

struct FixedPriorityScheduler : public Scheduler
{
    FixedPriorityScheduler(const tasks_t& tasks);

    Task* getTaskToExecute() override;
};

} // namespace NPE
} // namespace Scheduling
} // namespace VanillaCube
