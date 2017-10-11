/*
 * DigitalOutputPort.h
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#ifndef DRIVER_DIGITALOUTPUTPORT_H_
#define DRIVER_DIGITALOUTPUTPORT_H_

#include "Driver.h"

namespace i2c_hat {

class DigitalOutputPort {
private:
    gpio_t* _gpios;
    uint32_t _size;
    bool _inverted;
public:
    DigitalOutputPort(gpio_t* gpios, const uint32_t size, const bool inverted = false, const GPIOOType_TypeDef oType = GPIO_OType_PP, const GPIOPuPd_TypeDef pupd = GPIO_PuPd_NOPULL);
    uint32_t getSize();
    void write(const uint32_t value);
    uint32_t read();
    void writeBit(const uint32_t bitPosition, const bool bitState);
    bool readBit(const uint32_t bitPosition);
};

} /* namespace i2c_hat */

#endif /* DRIVER_DIGITALOUTPUTPORT_H_ */
