/*
 * CooperativeScheduler.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#include <stddef.h>
#include "CooperativeScheduler.h"

CooperativeScheduler::CooperativeScheduler(const uint32_t maxTaskCount) {
    _maxTaskCount = maxTaskCount;
    _taskList = new CooperativeTask*[_maxTaskCount];
    for(uint32_t i = 0; i < _maxTaskCount; i++) {
        _taskList[i] = NULL;
    }
    _eventBus = new CooperativeEventBus(_maxTaskCount);
}

CooperativeTask **CooperativeScheduler::getTaskList() {
    return _taskList;
}

uint32_t CooperativeScheduler::getTaskCount() {
    return _maxTaskCount;
}

bool CooperativeScheduler::registerTask(CooperativeTask* task) {
    uint32_t i;
    bool taskRegistered = false;

    for(i = 0; i < _maxTaskCount; i++) {
        if(_taskList[i] == NULL) {
            _taskList[i] = task;
            task->setMessageBus(_eventBus);
            taskRegistered = true;
            break;
        }
    }
    return taskRegistered;
}

bool CooperativeScheduler::deleteTask(CooperativeTask* task) {
    uint32_t i;
    bool taskDeleted = false;

    for(i = 0; i < _maxTaskCount; i++) {
        if(_taskList[i] == task) {
            _taskList[i] = NULL;
            taskDeleted = true;
            break;
        }
    }
    return taskDeleted;
}

void CooperativeScheduler::tick() {
    uint32_t i;
    uint32_t delay;
    uint32_t period;
    uint32_t runCnt;
    CooperativeTask *task;

    for(i = 0; i < _maxTaskCount; i++) {
        task = _taskList[i];
        if(task != NULL) {
            delay = task->getTaskDelay();
            period = task->getTaskPeriod();
            runCnt = task->getTaskRunCounter();
            if (delay != 0) {
                delay -= 1;
                task->setTaskDelay(delay);
            }
            if (delay == 0) {
                runCnt += 1;
                task->setTaskRunCounter(runCnt);
                if (period > 0) {
                    task->setTaskDelay(period);
                }
            }
        }
    }
}

void CooperativeScheduler::dispatch() {
    uint32_t i;
    uint32_t runCnt;
    CooperativeTask *task;
    uint32_t event;

    // dispatch tasks
    for(i = 0; i < _maxTaskCount; i++) {
        task = _taskList[i];
        if(task != NULL) {
            runCnt = task->getTaskRunCounter();
            if (runCnt > 0) {
                task->task();
                runCnt -= 1;
                task->setTaskRunCounter(runCnt);
                if(task->getTaskPeriod() == 0) {
                    deleteTask(task);
                }
            }
        }
    }

    // dispatch messages
    if(_eventBus->receive(&event)) {
        for(i = 0; i < _maxTaskCount; i++) {
            task = _taskList[i];
            if(task != NULL) {
                task->receiveEvent(event);
            }
        }
    }
}

CooperativeScheduler::~CooperativeScheduler() {
    // TODO Auto-generated destructor stub
}
