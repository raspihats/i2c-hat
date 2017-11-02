/*
 * digital_input_pin.h
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#ifndef DRIVER_DIGITAL_INPUT_PIN_H_
#define DRIVER_DIGITAL_INPUT_PIN_H_

#include "stm32f0xx.h"
#include "stm32f0xx_ll_gpio.h"

namespace i2c_hat {
namespace driver {

class DigitalInputPin {
public:
    DigitalInputPin(GPIO_TypeDef* port, uint16_t pin, const bool inverted=false);
    bool GetState();
private:
    GPIO_TypeDef*   port_;
    uint16_t        pin_;
    bool            inverted_;
};

} /* namespace driver */
} /* namespace i2c_hat */

#endif /* DRIVER_DIGITAL_INPUT_PIN_H_ */
