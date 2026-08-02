/*
 * scheduler.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: fcos
 */

#include "scheduler.h"

namespace cooperative_os {

Scheduler::Scheduler() :
    task_list_{NULL} {
}

Task **Scheduler::GetTaskList() {
    return task_list_;
}

uint32_t Scheduler::GetTaskCount() {
    uint32_t task_count, i;

    task_count = 0;
    for(i = 0; i < COOPSCH_MAX_TASK_COUNT; i++) {
        if(task_list_[i] != NULL) {
            task_count++;
        }
    }
    return task_count;
}

bool Scheduler::RegisterTask(Task* task) {
    bool success = false;

    for(uint32_t i = 0; i < COOPSCH_MAX_TASK_COUNT; i++) {
        if(task_list_[i] == NULL) {
            task_list_[i] = task;
            task->set_event_bus(&event_bus_);
            success = true;
            break;
        }
    }
    return success;
}

bool Scheduler::DeleteTask(Task* task) {
    bool success = false;
    uint32_t i, j;

    for(i = 0; i < COOPSCH_MAX_TASK_COUNT; i++) {
        if(task_list_[i] == task) {
            task_list_[i] = NULL;
            success = true;
            break;
        }
    }

    for(j = i; j < COOPSCH_MAX_TASK_COUNT - 1; j++) {
        task_list_[j] = task_list_[j + 1];
    }
    task_list_[j] = NULL;

    return success;
}

void Scheduler::Tick() {
    uint32_t i;
    uint32_t delay;
    uint32_t period;
    uint32_t runCnt;
    Task *task;

    for(i = 0; i < COOPSCH_MAX_TASK_COUNT; i++) {
        task = task_list_[i];
        if(task != NULL) {
            delay = task->delay();
            period = task->period();
            runCnt = task->run_counter();
            if (delay != 0) {
                delay -= 1;
                task->set_delay(delay);
            }
            if (delay == 0) {
                runCnt += 1;
                task->set_run_counter(runCnt);
                if (period > 0) {
                    task->set_delay(period);
                }
            }
        }
    }
}

void Scheduler::Dispatch() {
    uint32_t i;
    uint32_t runCnt;
    Task *task;
    uint32_t event;

    // dispatch tasks
    for(i = 0; i < COOPSCH_MAX_TASK_COUNT; i++) {
        task = task_list_[i];
        if(task != NULL) {
            runCnt = task->run_counter();
            if (runCnt > 0) {
                task->Run();
                runCnt -= 1;
                task->set_run_counter(runCnt);
                if(task->period() == 0) {
                    DeleteTask(task);
                }
            }
        }
    }

    // dispatch messages
    if(event_bus_.Receive(&event)) {
        for(i = 0; i < COOPSCH_MAX_TASK_COUNT; i++) {
            task = task_list_[i];
            if(task != NULL) {
                task->ReceiveEvent(event);
            }
        }
    }
}

} /* namespace cooperative_os */
