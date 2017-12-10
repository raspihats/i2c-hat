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
#define BOARD_NAME                          "DQ8rly I2C-HAT"

#define I2C_BASE_ADDRESS                    (0x50)

#define STATUS_LED_PIN                      driver::DigitalOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, true)

#define DIGITAL_OUTPUT_CHANNEL_COUNT        (8)

#define DIGITAL_OUTPUT_CHANNELS \
        driver::DigitalOutputPwm(TIM3, LL_TIM_CHANNEL_CH1), \
        driver::DigitalOutputPwm(TIM3, LL_TIM_CHANNEL_CH2), \
        driver::DigitalOutputPwm(TIM3, LL_TIM_CHANNEL_CH3), \
        driver::DigitalOutputPwm(TIM3, LL_TIM_CHANNEL_CH4), \
        driver::DigitalOutputPwm(TIM1, LL_TIM_CHANNEL_CH1), \
        driver::DigitalOutputPwm(TIM1, LL_TIM_CHANNEL_CH2), \
        driver::DigitalOutputPwm(TIM1, LL_TIM_CHANNEL_CH3), \
        driver::DigitalOutputPwm(TIM1, LL_TIM_CHANNEL_CH4)

#endif /* BOARD_H_ */
