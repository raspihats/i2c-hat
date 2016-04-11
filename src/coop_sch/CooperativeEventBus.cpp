/*
 * CooperativeEventBus.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#include "CooperativeEventBus.h"

CooperativeEventBus::CooperativeEventBus(const uint32_t size) {
    _queue = new uint32_t[size];
    _size = size;
    _count = 0;
}

void CooperativeEventBus::send(const uint32_t event) {
    if(_count < _size) {
        _queue[_count++] = event;
    }
}

bool CooperativeEventBus::receive(uint32_t * const event) {
    bool ret = false;
    if(_count > 0) {
        *event = _queue[0];
        _count--;
        for(uint32_t i = 0; i < _count; i++) {
            _queue[i] = _queue[i + 1];
        }
        ret = true;
    }
    return ret;
}

CooperativeEventBus::~CooperativeEventBus() {
    delete[](_queue);
}

