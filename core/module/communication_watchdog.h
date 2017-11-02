/*
 * communication_watchdog.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_COMMUNICATION_WATCHDOG_H_
#define MODULE_COMMUNICATION_WATCHDOG_H_

#include "module.h"

namespace i2c_hat {
namespace module {

class CommunicationWatchdog: public Module {
public:
    CommunicationWatchdog();
    bool ProcessRequest(Frame& request, Frame& response);
private:
    enum State {
        STATE_DISABLED,
        STATE_MONITORING,
        STATE_TIMEOUT,
    };

    uint32_t period_;
    bool communication_;
    State state_;

    uint32_t period();
    bool SetPeriod(const uint32_t period);
    void feed();
    void Init();
    void Run();
    void ReceiveEvent(const uint32_t event);
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_COMMUNICATION_WATCHDOG_H_ */
