/*
 * CooperativeTask.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#include <stddef.h>
#include "CooperativeTask.h"

CooperativeTask::CooperativeTask(const uint32_t delay, const uint32_t period) {
    _delay = delay;
    _period = period;
    _runCnt = 0;
    _messageBus = NULL;
}

void CooperativeTask::setMessageBus(CooperativeEventBus* messageBus) {
    _messageBus = messageBus;
}

uint32_t CooperativeTask::getTaskDelay() {
    return _delay;
}

void CooperativeTask::setTaskDelay(const uint32_t delay) {
    _delay = delay;
}

uint32_t CooperativeTask::getTaskPeriod() {
    return _period;
}

void CooperativeTask::setTaskPeriod(const uint32_t period) {
    _period = period;
}

uint32_t CooperativeTask::getTaskRunCounter() {
    return _runCnt;
}

void CooperativeTask::setTaskRunCounter(const uint32_t runCounter) {
    _runCnt = runCounter;
}

void CooperativeTask::sendEvent(const uint32_t event) {
    if(_messageBus != NULL) {
        _messageBus->send(event);
    }
}

void CooperativeTask::receiveEvent(const uint32_t event) {
    (void)event;
}

void CooperativeTask::task() {
}

CooperativeTask::~CooperativeTask() {
    // TODO Auto-generated destructor stub
}
