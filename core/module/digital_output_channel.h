/*
 * digital_output_channel.h
 *
 *  Created on: Nov 1, 2017
 *      Author: fcos
 */

#ifndef MODULE_DIGITAL_OUTPUT_CHANNEL_H_
#define MODULE_DIGITAL_OUTPUT_CHANNEL_H_

#include "../driver/digital_output_pwm.h"

namespace i2c_hat {
namespace module {

class DigitalOutputChannel {
public:
    DigitalOutputChannel(driver::DigitalOutputPwm pin);
    bool GetState();
    void SetState(const bool state);
private:
    driver::DigitalOutputPwm pin_;
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_DIGITAL_OUTPUT_CHANNEL_H_ */
