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

void DigitalOutputChannel::Init() {
    pin_.Init();
}

bool DigitalOutputChannel::SetFrequency(const float value) {
    return pin_.SetFrequency(value);
}

float DigitalOutputChannel::GetFrequency() {
    return pin_.GetFrequency();
}

bool DigitalOutputChannel::SetDutyCycle(const float value) {
    return pin_.SetDutyCycle(value);
}

float DigitalOutputChannel::GetDutyCycle() {
    return pin_.GetDutyCycle();
}

} /* namespace module */
} /* namespace i2c_hat */
