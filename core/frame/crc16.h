/*
 * Crc16.h
 *
 *  Created on: Apr 8, 2016
 *      Author: fcos
 */

#ifndef BOARD_CRC16_H_
#define BOARD_CRC16_H_

#include <stdint.h>

namespace i2c_hat {

class Crc16 {
public:
    static uint16_t Calc(const uint8_t* const data, const uint32_t size);
};

} /* namespace i2c_hat */

#endif /* BOARD_CRC16_H_ */
