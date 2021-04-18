#include <scheduling/task.hpp>

#include <timing/systick.hpp>

using namespace VanillaCube::Scheduling;

// --------------------------------------------------------------------------------------------------------------------

float TaskInfo::getAverageRuntime() const
{
    return (float)runTime / runCntr;
}

void TaskInfo::updateLoad(counter_t passed_since_last_update)
{
    load = (float)(runTime - lastRunTime) /passed_since_last_update,
    lastRunTime = runTime;
}

// --------------------------------------------------------------------------------------------------------------------

Task::Task(const char* name, unsigned priority, loop_function_t loop_function)
    : fLoop(loop_function)
    , priority(priority)
    , info({ name, 0, 0 })
{}

unsigned Task::getPriority() const
{
    return priority;
}

const TaskInfo& Task::getInfo() const
{
    return info;
}

void Task::execute()
{
    Timing::systick_t start_time = Timing::getSysick();

    fLoop();
    info.runCntr++;
    info.runTime += Timing::getSystickSince(start_time);
}

bool Task::isHigherPriority(const Task& other) const
{
    return (priority < other.priority);
}

void Task::updateLoad(TaskInfo::counter_t passed_since_last_update)
{
    info.updateLoad(passed_since_last_update);
}
