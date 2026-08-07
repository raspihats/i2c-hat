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
#define FW_VERSION_MINOR                    (0)
#define FW_VERSION_PATCH                    (1)

#define BOARD_NAME_SIZE                     (25)
#define BOARD_NAME                          "DQ5rly I2C-HAT"

#define I2C_BASE_ADDRESS                    (0x50)

#define STATUS_LED_PIN                      driver::DigitalOutputPin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, true)

#define DIGITAL_OUTPUT_CHANNEL_COUNT        (5)

#define DIGITAL_OUTPUT_CHANNELS \
        driver::DigitalOutputPwm(TIM3, LL_TIM_CHANNEL_CH1), \
        driver::DigitalOutputPwm(TIM3, LL_TIM_CHANNEL_CH2), \
        driver::DigitalOutputPwm(TIM3, LL_TIM_CHANNEL_CH3), \
        driver::DigitalOutputPwm(TIM3, LL_TIM_CHANNEL_CH4), \
        driver::DigitalOutputPwm(TIM1, LL_TIM_CHANNEL_CH1)


/* ---- shared-core module wiring (consumed by core/i2c_hat.{h,cpp}) ---- */
#ifdef __cplusplus
#include "stm32f0xx_ll_rcc.h"
#include "module/digital_outputs.h"

#define BOARD_MODULE_MEMBERS \
        module::DigitalOutputs digital_outputs_;

#define BOARD_REGISTER_MODULES \
        Register(digital_outputs_);
#endif /* __cplusplus */

#endif /* BOARD_H_ */
