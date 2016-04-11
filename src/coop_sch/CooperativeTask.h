/*
 * CooperativeTask.h
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#ifndef COOPERATIVE_TASK_H_
#define COOPERATIVE_TASK_H_

#include <stdint.h>
#include "CooperativeEventBus.h"

class CooperativeTask {
private:
    volatile uint32_t _delay;
    volatile uint32_t _period;
    volatile uint32_t _runCnt;
    CooperativeEventBus* _messageBus;
protected:
    void sendEvent(const uint32_t event);
public:
    CooperativeTask(const uint32_t delay, const uint32_t period);
    void setMessageBus(CooperativeEventBus* messageBus);
    uint32_t getTaskDelay();
    void setTaskDelay(const uint32_t delay);
    uint32_t getTaskPeriod();
    void setTaskPeriod(const uint32_t period);
    uint32_t getTaskRunCounter();
    void setTaskRunCounter(const uint32_t runCounter);
    virtual void receiveEvent(const uint32_t event);
    virtual void task();
    virtual ~CooperativeTask();
};

#endif /* COOPERATIVE_TASK_H_ */
