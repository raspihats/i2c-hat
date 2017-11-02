/*
 * task.h
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#ifndef COOPERATIVE_OS_TASK_H_
#define COOPERATIVE_OS_TASK_H_

#include <stdint.h>
#include <stddef.h>
#include "event_bus.h"

namespace cooperative_os {

class Task {
public:
    Task(const uint32_t delay, const uint32_t period);
    uint32_t delay();
    void set_delay(const uint32_t delay);
    uint32_t period();
    void set_period(const uint32_t period);
    uint32_t run_counter();
    void set_run_counter(const uint32_t run_counter);
    void set_event_bus(EventBus* event_bus);
    virtual void ReceiveEvent(const uint32_t event)=0;
    virtual void Run()=0;
    virtual ~Task();
protected:
    void SendEvent(const uint32_t event);
private:
    volatile uint32_t delay_;
    volatile uint32_t period_;
    volatile uint32_t run_count_;
    EventBus* event_bus_;
};

} /* namespace cooperative_os */

#endif /* COOPERATIVE_OS_TASK_H_ */
