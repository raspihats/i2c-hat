/*
 * relay_output_channel.h
 *
 *  Created on: Nov 20, 2017
 *      Author: fcos
 */

#ifndef MODULE_RELAY_OUTPUT_CHANNEL_H_
#define MODULE_RELAY_OUTPUT_CHANNEL_H_

#include "../driver/digital_output_pwm.h"

namespace i2c_hat {
namespace module {

class RelayOutputChannel {
public:
    RelayOutputChannel(driver::DigitalOutputPwm pin);
    uint32_t pull();
    void set_pull(const uint32_t pull);
    uint32_t hold();
    void set_hold(const uint32_t hold);
    bool GetState();
    void SetState(const bool state);
    void Init(const uint32_t pull, const uint32_t hold);
    void Tick();
private:
    enum State {
        ST_OFF,
        ST_PULLING,
        ST_HOLDING,
    };
    driver::DigitalOutputPwm pin_;
    uint32_t pull_;
    uint32_t hold_;
    volatile uint32_t counter_;
    volatile State state_;
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_RELAY_OUTPUT_CHANNEL_H_ */
