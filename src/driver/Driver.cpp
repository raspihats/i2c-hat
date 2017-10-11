/*
 * Utils.cpp
 *
 *  Created on: Oct 11, 2017
 *      Author: fcos
 */

#include "Driver.h"

namespace i2c_hat {

uint16_t Driver::getPinSource(uint16_t pin) {
    uint16_t pinSource;

    pinSource = 0;
    while(pin > 0x01) {
        pinSource++;
        pin >>= 1;
    }
    return pinSource;
}

} /* namespace i2c_hat */
