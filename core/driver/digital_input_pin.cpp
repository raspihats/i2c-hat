/*
 * digital_input_pin.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#include "digital_input_pin.h"

namespace i2c_hat {
namespace driver {

DigitalInputPin::DigitalInputPin(GPIO_TypeDef* port, uint16_t pin, const bool inverted) :
        port_(port),
        pin_(pin),
        inverted_(inverted) {

}

bool DigitalInputPin::GetState() {
    return LL_GPIO_IsInputPinSet(port_, pin_) ^ inverted_;
}

} /* namespace driver */
} /* namespace i2c_hat */
