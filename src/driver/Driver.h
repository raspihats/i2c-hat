/*
 * Utils.h
 *
 *  Created on: Oct 11, 2017
 *      Author: fcos
 */

#ifndef DRIVER_DRIVER_H_
#define DRIVER_DRIVER_H_

#include "BoardDef.h"

namespace i2c_hat {

class Driver {
public:
    static uint16_t getPinSource(uint16_t pin);
};

} /* namespace i2c_hat */

#endif /* DRIVER_DRIVER_H_ */
