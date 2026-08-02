/*
 * event_bus.h
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#ifndef COOPERATIVE_OS_EVENT_BUS_H_
#define COOPERATIVE_OS_EVENT_BUS_H_

#include <stdint.h>

#define COOP_OS_EVBUS_QUEUE_SIZE            (10)

namespace cooperative_os {

class EventBus {
public:
    EventBus();
    void Send(const uint32_t event);
    bool Receive(uint32_t * const event);
private:
    uint32_t queue_[COOP_OS_EVBUS_QUEUE_SIZE];
    uint32_t count_;
};

} /* namespace cooperative_os */

#endif /* COOPERATIVE_OS_EVENT_BUS_H_ */
