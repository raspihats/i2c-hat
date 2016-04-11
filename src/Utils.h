/*
 * Utils.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "stdint.h"

class Utils {
public:
    static void convertBytesToUint32(const uint8_t* const data, uint32_t* const value);
    static void convertUint32ToBytes(const uint32_t value, uint8_t* const data);
};

#endif /* UTILS_H_ */
