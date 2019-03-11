/*
 * ll_utils.h
 *
 *  Created on: 17 Sep 2018
 *      Author: fcos
 */

#ifndef LL_UTILS_H_
#define LL_UTILS_H_

#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_tim.h"

void LL_GPIO_WriteOutputPin(GPIO_TypeDef* gpio, const uint32_t pin, const uint32_t value);

void LL_TIM_OC_SetCompare(TIM_TypeDef * const tim, const uint32_t channel, const uint32_t value);
uint32_t LL_TIM_OC_GetCompare(TIM_TypeDef * const tim, const uint32_t channel);

#endif /* LL_UTILS_H_ */
