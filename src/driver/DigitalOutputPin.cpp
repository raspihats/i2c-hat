/*
 * DigitalOutputPin.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#include "DigitalOutputPin.h"

namespace i2c_hat {

DigitalOutputPin::DigitalOutputPin(gpio_t* gpio, const bool inverted, const OutputType output_type) {
    GPIO_InitTypeDef GPIO_InitStructure;
    BitAction bitState;

    _gpio = gpio;
    _inverted = inverted;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    switch(output_type) {
    case OT_OPEN_DRAIN:
    	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    	break;
    default:
    	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    }
    GPIO_InitStructure.GPIO_Pin = _gpio->pin;
    // set output state before changing pin mode form input to output
    bitState = _inverted ? Bit_SET : Bit_RESET;
    GPIO_WriteBit(_gpio->port, _gpio->pin, bitState);
    GPIO_Init(_gpio->port, &GPIO_InitStructure);
}

bool DigitalOutputPin::read() {
    return (GPIO_ReadOutputDataBit(_gpio->port, _gpio->pin) == Bit_SET) ^ _inverted;
}

void DigitalOutputPin::write(bool state) {
    BitAction bitAction;
    bitAction = (state ^ _inverted) ? Bit_SET : Bit_RESET;
    GPIO_WriteBit(_gpio->port, _gpio->pin, bitAction);
}

} /* namespace i2c_hat */
