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
    enum OutputType {
    	OT_PUSH_PULL,
		OT_OPEN_DRAIN,
    };
    DigitalOutputPin(gpio_t* pin, const bool inverted = false, const OutputType output_type = OT_PUSH_PULL);
    bool read();
    void write(bool state);
};

} /* namespace i2c_hat */

#endif /* DRIVER_DIGITALOUTPUTPIN_H_ */
