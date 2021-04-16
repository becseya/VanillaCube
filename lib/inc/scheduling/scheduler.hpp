#pragma once

#include "task.hpp"

#include "../array.hpp"
#include "../timing/systick.hpp"

namespace VanillaCube {
namespace Scheduling {

using tasks_t = Array<Task*>;

class Scheduler
{
    float cpuLoad;

  protected:
    const tasks_t& tasks;

  public:
    Scheduler(const tasks_t& tasks);

    void iterate();

    float               getCpuLoad() const;
    size_t              getNumberOfTasks() const;
    const Task::info_t& getTaskInfo(size_t idx) const;

  protected:
    virtual Task* getTaskToExecute() = 0;

  private:
    void updateCpuLoad();
};

} // namespace Scheduling
} // namespace VanillaCube
