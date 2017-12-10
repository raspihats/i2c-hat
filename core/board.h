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
#define FW_VERSION_MAJOR                    (2)
#define FW_VERSION_MINOR                    (0)
#define FW_VERSION_PATCH                    (0)

#define BOARD_NAME_SIZE                     (25)
#define BOARD_NAME                          "DI16ac I2C-HAT"

#define I2C_BASE_ADDRESS                    (0x40)

#define STATUS_LED_PIN                      driver::DigitalOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, true)

#define DIGITAL_INPUT_CHANNEL_COUNT         (16)

#define DIGITAL_INPUT_CHANNELS \
        driver::DigitalInputPin(DI0_GPIO_Port, DI0_Pin, true), \
        driver::DigitalInputPin(DI1_GPIO_Port, DI1_Pin, true), \
        driver::DigitalInputPin(DI2_GPIO_Port, DI2_Pin, true), \
        driver::DigitalInputPin(DI3_GPIO_Port, DI3_Pin, true), \
        driver::DigitalInputPin(DI4_GPIO_Port, DI4_Pin, true), \
        driver::DigitalInputPin(DI5_GPIO_Port, DI5_Pin, true), \
        driver::DigitalInputPin(DI6_GPIO_Port, DI6_Pin, true), \
        driver::DigitalInputPin(DI7_GPIO_Port, DI7_Pin, true), \
        driver::DigitalInputPin(DI8_GPIO_Port, DI8_Pin, true), \
        driver::DigitalInputPin(DI9_GPIO_Port, DI9_Pin, true), \
        driver::DigitalInputPin(DI10_GPIO_Port, DI10_Pin, true), \
        driver::DigitalInputPin(DI11_GPIO_Port, DI11_Pin, true), \
        driver::DigitalInputPin(DI12_GPIO_Port, DI12_Pin, true), \
        driver::DigitalInputPin(DI13_GPIO_Port, DI13_Pin, true), \
        driver::DigitalInputPin(DI14_GPIO_Port, DI14_Pin, true), \
        driver::DigitalInputPin(DI15_GPIO_Port, DI15_Pin, true)

#define IRQ_PIN                             driver::DigitalOutputPin(IRQ_GPIO_Port, IRQ_Pin, true)

#endif /* BOARD_H_ */
