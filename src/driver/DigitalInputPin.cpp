/*
 * DigitalInputPin.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#include "DigitalInputPin.h"

namespace i2c_hat {

DigitalInputPin::DigitalInputPin(gpio_t* gpio, const bool inverted) {
    GPIO_InitTypeDef GPIO_InitStructure;
    _gpio = gpio;
    _inverted = inverted;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = _gpio->pin;
    GPIO_Init(_gpio->port, &GPIO_InitStructure);
}

bool DigitalInputPin::read() {
    return (GPIO_ReadOutputDataBit(_gpio->port, _gpio->pin) == Bit_SET) ^ _inverted;
}

} /* namespace i2c_hat */
