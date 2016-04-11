/*
 * CooperativeScheduler.h
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#ifndef COOPERATIVE_SCHEDULER_H_
#define COOPERATIVE_SCHEDULER_H_

#include "CooperativeEventBus.h"
#include "CooperativeTask.h"

class CooperativeScheduler {
private:
    uint32_t _maxTaskCount;
    CooperativeTask **_taskList;
    CooperativeEventBus *_eventBus;
protected:
    CooperativeTask **getTaskList();
    uint32_t getTaskCount();
public:
    CooperativeScheduler(const uint32_t maxTaskCount);
    bool registerTask(CooperativeTask* task);
    bool deleteTask(CooperativeTask* task);
    void tick();
    void dispatch();
    virtual ~CooperativeScheduler();
};

#endif /* COOPERATIVE_SCHEDULER_H_ */
