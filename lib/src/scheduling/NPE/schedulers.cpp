#include <scheduling/NPE/schedulers.hpp>

using namespace VanillaCube::Scheduling;
using namespace NPE;

// --------------------------------------------------------------------------------------------------------------------

FixedPriorityScheduler::FixedPriorityScheduler(const tasks_t& tasks)
    : Scheduler(tasks)
{}

Task* FixedPriorityScheduler::getTaskToExecute()
{
    Task* candinde = nullptr;

    for (size_t i = 0; i < tasks.size(); i++) {
        if (tasks[i]->isReady() && (!candinde || tasks[i]->isHigherPriority(*candinde)))
            candinde = tasks[i];
    }

    return candinde;
}
