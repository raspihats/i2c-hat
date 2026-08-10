/*
 * stm32xx_ll.h
 *
 *  MCU-family selector for the shared core drivers.
 *
 *  The core is written against the ST LL API, which is identical across the
 *  families we target (same GPIO/TIM and I2C peripheral IP). The only thing
 *  that differs is which vendor headers to pull in, keyed off the CMSIS
 *  device define the build passes (-DSTM32F042x6, -DSTM32G031xx, ...).
 *
 *  Sets I2C_HAT_MCU_FAMILY_F0 / _G0 for the few places core code genuinely
 *  differs per family (RCC reset flags, EEPROM emulation middleware).
 */

#ifndef DRIVER_STM32XX_LL_H_
#define DRIVER_STM32XX_LL_H_

#if defined(STM32G031xx) || defined(STM32G030xx) || defined(STM32G041xx)

#define I2C_HAT_MCU_FAMILY_G0   1
#include "stm32g0xx.h"
#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_i2c.h"
#include "stm32g0xx_ll_iwdg.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_rcc.h"

#elif defined(STM32F042x6) || defined(STM32F030x6) || defined(STM32F030x8) \
   || defined(STM32F051x8) || defined(STM32F091xC)

#define I2C_HAT_MCU_FAMILY_F0   1
#include "stm32f0xx.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_i2c.h"
#include "stm32f0xx_ll_iwdg.h"
#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_rcc.h"

#else
#error "Unknown MCU device define: add its family to core/driver/stm32xx_ll.h"
#endif

#endif /* DRIVER_STM32XX_LL_H_ */
