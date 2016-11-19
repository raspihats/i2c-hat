/*
 * Module.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_H_
#define MODULE_H_

#include <stdint.h>
#include "stm32f0xx.h"
#include "coop_sch/CooperativeTask.h"
#include "i2c_frame/I2CFrame.h"
#include "Commands.h"

class Module : public CooperativeTask {
public:
    enum Event {
        EVENT_CWDT_DISABLED,
        EVENT_CWDT_MONITORING,
        EVENT_CWDT_TIMEOUT,
    };
    Module(const uint32_t delay, const uint32_t period);
    virtual uint32_t processRequest(I2CFrame *request, I2CFrame *response);
};

#endif /* MODULE_H_ */
