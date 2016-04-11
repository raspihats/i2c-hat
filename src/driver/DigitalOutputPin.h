/*
 * DigitalOutputPin.h
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#ifndef DRIVER_DIGITALOUTPUTPIN_H_
#define DRIVER_DIGITALOUTPUTPIN_H_

#include "BoardDef.h"

namespace i2c_hat {

class DigitalOutputPin {
private:
    gpio_t* _gpio;
    bool _inverted;
public:
    DigitalOutputPin(gpio_t* pin, const bool inverted = false);
    bool read();
    void write(bool state);
};

} /* namespace i2c_hat */

#endif /* DRIVER_DIGITALOUTPUTPIN_H_ */
