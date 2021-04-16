#include <scheduling/scheduler.hpp>

using namespace VanillaCube::Scheduling;

// --------------------------------------------------------------------------------------------------------------------

#define LOAD_CHECK_TICKS 1500

Scheduler::Scheduler(const tasks_t& tasks)
    : tasks(tasks)
{}

float Scheduler::getCpuLoad() const
{
    return cpuLoad;
}

size_t Scheduler::getNumberOfTasks() const
{
    return tasks.size();
}

const TaskInfo& Scheduler::getTaskInfo(size_t idx) const
{
    return tasks[idx]->getInfo();
}

void Scheduler::iterate()
{
    Task* candinde = getTaskToExecute();

    if (candinde)
        candinde->execute();

    updateCpuLoad();
}

void Scheduler::updateCpuLoad()
{
    static Timing::systick_t last_load_check = 0;
    static uint64_t          last_sum        = 0;

    auto d_tick = Timing::getSystickSince(last_load_check);

    if (d_tick > LOAD_CHECK_TICKS) {
        uint64_t runtime_sum = 0;

        for (size_t i = 0; i < tasks.size(); i++)
            runtime_sum += tasks[i]->getInfo().runTime;

        cpuLoad         = (float)(runtime_sum - last_sum) / d_tick;
        last_load_check = Timing::getSysick();
        last_sum        = runtime_sum;
    }
}
