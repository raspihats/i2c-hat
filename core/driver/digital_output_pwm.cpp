/*
 * digital_output_pwm.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: fcos
 */

#include "digital_output_pwm.h"
#include "ll_utils.h"
#include "stm32f0xx_ll_rcc.h"

namespace i2c_hat {
namespace driver {


DigitalOutputPwm::DigitalOutputPwm(TIM_TypeDef* tim, uint16_t channel, const bool inverted) :
        tim_(tim),
        channel_(channel),
        inverted_(inverted) {
    SetDuty(0);
}

void DigitalOutputPwm::Init(const float frequency, const float duty) {
    SetFrequency(frequency);
    SetDuty(duty);
}

void DigitalOutputPwm::SetFrequency(const float value) {
    uint32_t clk_frequency = 48000000;
    uint32_t prescaler = 0;
    uint32_t autoreload;

    do {
      prescaler++;
      autoreload = (uint32_t)((float)clk_frequency / (float)prescaler / value) - 1;
    } while((autoreload > 0xFFFF) && ((prescaler - 1) <= 0xFFFF));
    prescaler--;

    if(prescaler > 0xFFFF) {
      return;
    }

    LL_TIM_SetPrescaler(tim_, prescaler);
    LL_TIM_SetAutoReload(tim_, autoreload);
}

void DigitalOutputPwm::SetDuty(const float value) {
    uint32_t compare_value;

    if(value > 0) {
        compare_value = ((LL_TIM_GetAutoReload(tim_) * value) / 100) + 1;
    }
    else {
        compare_value = 0;
    }

    LL_TIM_OC_SetCompare(tim_, channel_, compare_value);
}



} /* namespace driver */
} /* namespace i2c_hat */
