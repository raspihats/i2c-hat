/*
 * DigitalOutputChannel.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: fcos
 */

#include "digital_output_channel.h"

namespace i2c_hat {
namespace module {

DigitalOutputChannel::DigitalOutputChannel(driver::DigitalOutputPwm pin) :
        pin_(pin) {
}

bool DigitalOutputChannel::GetState() {
//    return pin_.GetState();
    return false;
}

void DigitalOutputChannel::SetState(const bool state) {
//    pin_.SetState(state);
}

} /* namespace module */
} /* namespace i2c_hat */
