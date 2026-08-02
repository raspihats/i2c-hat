/*
 * scheduler.h
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#ifndef COOPERATIVE_OS_SCHEDULER_H_
#define COOPERATIVE_OS_SCHEDULER_H_

#include "task.h"

#define COOPSCH_MAX_TASK_COUNT          (10)

namespace cooperative_os {

class Scheduler {
public:
    Scheduler();
    bool RegisterTask(Task* task);
    bool DeleteTask(Task* task);
    void Tick();
    void Dispatch();
protected:
    Task **GetTaskList();
    uint32_t GetTaskCount();
private:
    Task* task_list_[COOPSCH_MAX_TASK_COUNT];
    EventBus event_bus_;
};

} /* namespace cooperative_os */

#endif /* COOPERATIVE_OS_SCHEDULER_H_ */
