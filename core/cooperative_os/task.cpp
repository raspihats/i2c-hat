/*
 * task.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#include "task.h"

namespace cooperative_os {

Task::Task(const uint32_t delay, const uint32_t period) :
        delay_(delay),
        period_(period),
        run_count_(0),
        event_bus_(NULL) {
}

void Task::set_event_bus(EventBus* event_bus) {
    event_bus_ = event_bus;
}

uint32_t Task::delay() {
    return delay_;
}

void Task::set_delay(const uint32_t delay) {
    delay_ = delay;
}

uint32_t Task::period() {
    return period_;
}

void Task::set_period(const uint32_t period) {
    period_ = period;
}

uint32_t Task::run_counter() {
    return run_count_;
}

void Task::set_run_counter(const uint32_t run_counter) {
    run_count_ = run_counter;
}

void Task::SendEvent(const uint32_t event) {
    if(event_bus_ != NULL) {
        event_bus_->Send(event);
    }
}

Task::~Task() {
}

} /* namespace cooperative_os */
