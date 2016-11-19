/*
 * CommWatchdog.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_COMMWATCHDOG_H_
#define MODULE_COMMWATCHDOG_H_

#include "Module.h"

namespace i2c_hat {

class CommWatchdog: public Module {
private:
    enum State {
        STATE_DISABLED,
        STATE_MONITORING,
        STATE_TIMEOUT,
    };

    State _state;
    uint32_t _periodCnt;
    uint32_t _period;
    bool _communication;


    void setCwdtPeriod(const uint32_t period);
    uint32_t getCwdtPeriod();
    void feed();
    void task();
public:
    CommWatchdog();
    uint32_t processRequest(I2CFrame *request, I2CFrame *response);
};

} /* namespace i2c_hat */

#endif /* MODULE_COMMWATCHDOG_H_ */
