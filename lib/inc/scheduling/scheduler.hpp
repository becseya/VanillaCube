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

    void  iterate();
    float getCpuLoad();

    virtual Task* getTaskToExecute() = 0;

  private:
    void updateCpuLoad();
};

} // namespace Scheduling
} // namespace VanillaCube
