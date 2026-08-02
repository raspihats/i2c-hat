/*
 * digital_output_pin.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#include "digital_output_pin.h"

namespace i2c_hat {
namespace driver {

DigitalOutputPin::DigitalOutputPin(GPIO_TypeDef* port, uint16_t pin, const bool inverted, const bool initial) :
        port_(port),
        pin_(pin),
        inverted_(inverted) {
    SetState(initial);
}

bool DigitalOutputPin::GetState() {
    return LL_GPIO_IsOutputPinSet(port_, pin_) ^ inverted_;
}

void DigitalOutputPin::SetState(bool state) {
    if(state ^ inverted_) {
        LL_GPIO_SetOutputPin(port_, pin_);
    }
    else {
        LL_GPIO_ResetOutputPin(port_, pin_);
    }
}

} /* namespace driver */
} /* namespace i2c_hat */
