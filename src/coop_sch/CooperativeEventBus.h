/*
 * CooperativeEventBus.h
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#ifndef COOPERATIVE_EVENT_BUS_H_
#define COOPERATIVE_EVENT_BUS_H_

#include <stdint.h>

class CooperativeEventBus {
private:
    uint32_t *_queue;
    uint32_t _size;
    uint32_t _count;
public:
    CooperativeEventBus(const uint32_t size);
    void send(const uint32_t event);
    bool receive(uint32_t * const event);
    virtual ~CooperativeEventBus();
};

#endif /* COOPERATIVE_EVENT_BUS_H_ */
