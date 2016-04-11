/*
 * DigitalInputPin.h
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#ifndef DRIVER_DIGITALINPUTPIN_H_
#define DRIVER_DIGITALINPUTPIN_H_

#include "BoardDef.h"

namespace i2c_hat {

class DigitalInputPin {
private:
    gpio_t* _gpio;
    bool _inverted;
public:
    DigitalInputPin(gpio_t* gpio, const bool inverted);
    bool read();
};

} /* namespace i2c_hat */

#endif /* DRIVER_DIGITALINPUTPIN_H_ */
