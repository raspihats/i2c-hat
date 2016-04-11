/*
 * StatusLed.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_STATUSLED_H_
#define MODULE_STATUSLED_H_

#include "Module.h"
#include "driver/DigitalOutputPin.h"

namespace i2c_hat {

class StatusLed: public Module {
private:
    DigitalOutputPin *_pin;
    bool _communication;
    void receiveEvent(const uint32_t event);
public:
    StatusLed(DigitalOutputPin* const statusLedPin);
    void task();
    uint32_t processRequest(I2CFrame *request, I2CFrame *response);
};

} /* namespace i2c_hat */

#endif /* MODULE_STATUSLED_H_ */
