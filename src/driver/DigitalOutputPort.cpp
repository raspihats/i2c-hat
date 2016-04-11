/*
 * DigitalOutputPort.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#include "DigitalOutputPort.h"

namespace i2c_hat {

DigitalOutputPort::DigitalOutputPort(gpio_t* gpios, const uint32_t size, const bool inverted, const GPIOOType_TypeDef oType, const GPIOPuPd_TypeDef pupd) {
    GPIO_InitTypeDef GPIO_InitStructure;
    BitAction bitState;
    uint32_t channel;

    _gpios = gpios;
    _size = size;
    _inverted = inverted;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = oType;
    GPIO_InitStructure.GPIO_PuPd = pupd;

    for(channel = 0; channel < _size; channel++) {
        // set output state before changing pin mode form input to output
        bitState = _inverted ? Bit_SET : Bit_RESET;
        GPIO_WriteBit(_gpios[channel].port, _gpios[channel].pin, bitState);
        GPIO_InitStructure.GPIO_Pin = _gpios[channel].pin;
        GPIO_Init(_gpios[channel].port, &GPIO_InitStructure);
    }
}

uint32_t DigitalOutputPort::getSize() {
    return _size;
}

void DigitalOutputPort::write(const uint32_t value) {
    uint32_t channel;
    uint32_t bitState;

    for(channel = 0; channel < _size; channel++) {
        bitState = (value >> channel) & 0x01;
        if(_inverted) {
            bitState ^= 0x01;
        }
        GPIO_WriteBit(_gpios[channel].port, _gpios[channel].pin, (BitAction)bitState);
    }
}

uint32_t DigitalOutputPort::read() {
    uint32_t channel;
    uint32_t bitState;
    uint32_t value;

    value = 0;
    for(channel = 0; channel < _size; channel++) {
        bitState = GPIO_ReadOutputDataBit(_gpios[channel].port, _gpios[channel].pin);
        if(_inverted) {
            bitState ^= 0x01;
        }
        if(bitState == 0x01) {
            value |= (0x01 << channel);
        }
    }
    return value;
}

void DigitalOutputPort::writeBit(const uint32_t bitPosition, const bool bitState) {
    bool temp;
    BitAction bitAction;
    if(bitPosition < _size) {
        temp = _inverted ? bitState ^ true : bitState;
        bitAction = temp ? Bit_SET : Bit_RESET;
        GPIO_WriteBit(_gpios[bitPosition].port, _gpios[bitPosition].pin, bitAction);
    }
}

bool DigitalOutputPort::readBit(const uint32_t bitPosition) {
    bool bitState = false;
    if(bitPosition < _size) {
        bitState = (GPIO_ReadOutputDataBit(_gpios[bitPosition].port, _gpios[bitPosition].pin) == Bit_SET) ? true : false;
        bitState = _inverted ? bitState ^ true : bitState;
    }
    return bitState;
}

} /* namespace i2c_hat */
