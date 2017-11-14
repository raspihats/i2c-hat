/*
 * board.h
 *
 *  Created on: Nov 1, 2017
 *      Author: fcos
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "main.h"

#define FW_VERSION_SIZE                     (3)
#define FW_VERSION_MAJOR                    (1)
#define FW_VERSION_MINOR                    (2)
#define FW_VERSION_PATCH                    (0)

#define BOARD_NAME_SIZE                     (25)
#define BOARD_NAME                          "DQ6od I2C-HAT"

#define I2C_BASE_ADDRESS                    (0x50)

#define STATUS_LED_PIN                      driver::DigitalOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, true)

//#define DIGITAL_OUTPUT_CHANNEL_COUNT        (10)
//
//#define DIGITAL_OUTPUT_CHANNELS \
//        driver::DigitalOutputPin(RLY0_GPIO_Port, RLY0_Pin), \
//        driver::DigitalOutputPin(RLY1_GPIO_Port, RLY1_Pin), \
//        driver::DigitalOutputPin(RLY2_GPIO_Port, RLY2_Pin), \
//        driver::DigitalOutputPin(RLY3_GPIO_Port, RLY3_Pin), \
//        driver::DigitalOutputPin(RLY4_GPIO_Port, RLY4_Pin), \
//        driver::DigitalOutputPin(RLY5_GPIO_Port, RLY5_Pin), \
//        driver::DigitalOutputPin(RLY6_GPIO_Port, RLY6_Pin), \
//        driver::DigitalOutputPin(RLY7_GPIO_Port, RLY7_Pin), \
//        driver::DigitalOutputPin(RLY8_GPIO_Port, RLY8_Pin), \
//        driver::DigitalOutputPin(RLY9_GPIO_Port, RLY9_Pin)

#endif /* BOARD_H_ */
