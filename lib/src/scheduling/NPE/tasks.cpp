#include <scheduling/NPE/tasks.hpp>

using namespace VanillaCube::Scheduling::NPE;

// --------------------------------------------------------------------------------------------------------------------

PeriodicTask::PeriodicTask(const char* name, unsigned priority, uint32_t period, loop_function_t loop_function)
    : Task(name, priority, loop_function)
    , PeriodicTimer(period)
    , readyLatch(false)
{}

void PeriodicTask::execute()
{
    Task::execute();
    readyLatch = false;
}

bool PeriodicTask::isReady()
{
    if (readyLatch)
        return true;

    readyLatch = PeriodicTimer::execute();
    return readyLatch;
}

// --------------------------------------------------------------------------------------------------------------------

DependentTask::DependentTask(const char* name, unsigned priority, Task& parent, loop_function_t loop_func)
    : Task(name, priority, loop_func)
    , taskToDependOn(parent)
    , lastCounter(0)
{}

void DependentTask::execute()
{
    Task::execute();
    lastCounter = taskToDependOn.getInfo().runCntr;
}

bool DependentTask::isReady()
{
    return (lastCounter != taskToDependOn.getInfo().runCntr);
}
