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
#define FW_VERSION_MINOR                    (1)
#define FW_VERSION_PATCH                    (2)

#define BOARD_NAME_SIZE                     (25)
#define BOARD_NAME                          "AI3tcDQ4rly I2C-HAT"

#define I2C_BASE_ADDRESS                    (0x70)

#define STATUS_LED_PIN                      driver::DigitalOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, true)

#define ANALOG_INPUT_CHANNEL_COUNT          (3)

/*
#define DIGITAL_INPUT_CHANNELS \
        driver::DigitalInputPin(DI0_GPIO_Port, DI0_Pin, true), \
        driver::DigitalInputPin(DI1_GPIO_Port, DI1_Pin, true), \
        driver::DigitalInputPin(DI2_GPIO_Port, DI2_Pin, true), \
        driver::DigitalInputPin(DI3_GPIO_Port, DI3_Pin, true), \
        driver::DigitalInputPin(DI4_GPIO_Port, DI4_Pin, true), \
        driver::DigitalInputPin(DI5_GPIO_Port, DI5_Pin, true)
*/

#define DIGITAL_OUTPUT_CHANNEL_COUNT        (4)

#define DIGITAL_OUTPUT_CHANNELS \
        driver::DigitalOutputPwm(TIM2, LL_TIM_CHANNEL_CH3), \
        driver::DigitalOutputPwm(TIM2, LL_TIM_CHANNEL_CH4), \
        driver::DigitalOutputPwm(TIM2, LL_TIM_CHANNEL_CH1), \
        driver::DigitalOutputPwm(TIM2, LL_TIM_CHANNEL_CH2)

#endif /* BOARD_H_ */
