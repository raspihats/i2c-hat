/*
 * Board.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: fcos
 */

#include "i2c_hat.h"

static i2c_hat::I2CHat hat;

extern "C" {


void I2CHat_init() {
    hat.Init();
}

void I2CHat_tick() {
    hat.Tick();
}

void I2CHat_run() {
    hat.Run();
}

}



