/*
 * WatchDog.h
 *
 *  Created on: Apr 6, 2016
 *      Author: fcos
 */

#ifndef DRIVER_WATCHDOG_H_
#define DRIVER_WATCHDOG_H_

namespace i2c_hat {

class WatchDog {
public:
    WatchDog();
    void feed();
};

} /* namespace i2c_hat */

#endif /* DRIVER_WATCHDOG_H_ */
