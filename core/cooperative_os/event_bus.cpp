/*
 * event_bus.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#include "event_bus.h"

namespace cooperative_os {

EventBus::EventBus() :
    count_(0) {
}

void EventBus::Send(const uint32_t event) {
    if(count_ < COOP_OS_EVBUS_QUEUE_SIZE) {
        queue_[count_++] = event;
    }
}

bool EventBus::Receive(uint32_t * const event) {
    bool success = false;

    if(count_ > 0) {
        *event = queue_[0];
        count_--;
        for(uint32_t i = 0; i < count_; i++) {
            queue_[i] = queue_[i + 1];
        }
        success = true;
    }
    return success;
}

} /* namespace cooperative_os */
