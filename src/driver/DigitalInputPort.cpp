/*
 * DigitalInputPort.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#include "DigitalInputPort.h"

namespace i2c_hat {

DigitalInputPort::DigitalInputPort(gpio_t* gpios, const uint32_t size, const GPIOPuPd_TypeDef pupd, const bool inverted) {
    GPIO_InitTypeDef GPIO_InitStructure;

    _gpios = gpios;
    _size = size;
    _inverted = inverted;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = pupd;

    for(uint32_t i = 0; i < _size; i++) {
        GPIO_InitStructure.GPIO_Pin = _gpios[i].pin;
        GPIO_Init(_gpios[i].port, &GPIO_InitStructure);
    }
}

uint32_t DigitalInputPort::getSize() {
    return _size;
}

uint32_t DigitalInputPort::read() {
    uint32_t bitState;
    uint32_t value;

    value = 0;
    for(uint32_t bitPosition = 0; bitPosition < _size; bitPosition++) {
        bitState = GPIO_ReadInputDataBit(_gpios[bitPosition].port, _gpios[bitPosition].pin);
        if(_inverted) {
            bitState ^= 0x01;
        }
        if(bitState == 0x01) {
            value |= (0x01 << bitPosition);
        }
    }
    return value;
}

bool DigitalInputPort::readBit(const uint32_t bitPosition) {
    bool bitState = false;
    if(bitPosition < _size) {
        bitState = (GPIO_ReadInputDataBit(_gpios[bitPosition].port, _gpios[bitPosition].pin) == Bit_SET) ? true : false;
        bitState = _inverted ? bitState ^ true : bitState;
    }
    return bitState;
}

} /* namespace i2c_hat */
