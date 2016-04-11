/*
 * Utils.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "Utils.h"

/**
  * @brief  Converts 4 bytes to a u32 value
  * @param  data: pointer to a 4 bytes stream
  * @param  value: pointer to u32 value
  * @retval None
  */
void Utils::convertBytesToUint32(const uint8_t* const data, uint32_t* const value) {
    *value = (uint32_t)data[0];
    *value |= (uint32_t)data[1] << 8;
    *value |= (uint32_t)data[2] << 16;
    *value |= (uint32_t)data[3] << 24;
}

/**
  * @brief  Converts u32 value to a 4 bytes stream
  * @param  value: u32 value to be converted
  * @param  data: pointer to a 4 bytes stream
  * @retval None
  */
void Utils::convertUint32ToBytes(const uint32_t value, uint8_t* const data) {
    data[0] = (uint8_t)(value);
    data[1] = (uint8_t)(value >> 8);
    data[2] = (uint8_t)(value >> 16);
    data[3] = (uint8_t)(value >> 24);
}
