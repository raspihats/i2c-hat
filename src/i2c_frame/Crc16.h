/*
 * Crc16.h
 *
 *  Created on: Apr 8, 2016
 *      Author: fcos
 */

#ifndef I2C_FRAME_CRC16_H_
#define I2C_FRAME_CRC16_H_

#include <stdint.h>

class Crc16 {
public:
    static uint16_t calc(const uint8_t* const data, const uint32_t length);
};

#endif /* I2C_FRAME_CRC16_H_ */
