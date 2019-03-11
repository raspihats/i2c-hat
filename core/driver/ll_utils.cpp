/*
 * ll_utils.cpp
 *
 *  Created on: 17 Sep 2018
 *      Author: fcos
 */

#include "ll_utils.h"


void LL_GPIO_WriteOutputPin(GPIO_TypeDef* gpio, const uint32_t pin, const uint32_t value) {
  if(value > 0) {
    LL_GPIO_SetOutputPin(gpio, pin);
  }
  else {
    LL_GPIO_ResetOutputPin(gpio, pin);
  }
}


void LL_TIM_OC_SetCompare(TIM_TypeDef * const tim, const uint32_t channel, const uint32_t value) {
  switch(channel) {
  case LL_TIM_CHANNEL_CH1:
      LL_TIM_OC_SetCompareCH1(tim, value);
      break;
  case LL_TIM_CHANNEL_CH2:
      LL_TIM_OC_SetCompareCH2(tim, value);
      break;
  case LL_TIM_CHANNEL_CH3:
      LL_TIM_OC_SetCompareCH3(tim, value);
      break;
  case LL_TIM_CHANNEL_CH4:
      LL_TIM_OC_SetCompareCH4(tim, value);
      break;
  default:
      break;
  }
}


uint32_t LL_TIM_OC_GetCompare(TIM_TypeDef * const tim, const uint32_t channel) {
  uint32_t value;

  switch(channel) {
  case LL_TIM_CHANNEL_CH1:
      value = LL_TIM_OC_GetCompareCH1(tim);
      break;
  case LL_TIM_CHANNEL_CH2:
      value = LL_TIM_OC_GetCompareCH2(tim);
      break;
  case LL_TIM_CHANNEL_CH3:
      value = LL_TIM_OC_GetCompareCH3(tim);
      break;
  case LL_TIM_CHANNEL_CH4:
      value = LL_TIM_OC_GetCompareCH4(tim);
      break;
  default:
      value = 0;
      break;
  }
  return value;
}
