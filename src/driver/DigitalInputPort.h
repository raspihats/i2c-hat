/*
 * DigitalInputPort.h
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#ifndef DRIVER_DIGITALINPUTPORT_H_
#define DRIVER_DIGITALINPUTPORT_H_

#include "BoardDef.h"

namespace i2c_hat {

class DigitalInputPort {
private:
    gpio_t* _gpios;
    uint32_t _size;
    bool _inverted;
public:
    DigitalInputPort(gpio_t* pins, const uint32_t size, const GPIOPuPd_TypeDef pupd = GPIO_PuPd_NOPULL, const bool inverted = false);
    uint32_t getSize();
    uint32_t read();
    bool readBit(const uint32_t index);
};

} /* namespace i2c_hat */

#endif /* DRIVER_DIGITALINPUTPORT_H_ */
