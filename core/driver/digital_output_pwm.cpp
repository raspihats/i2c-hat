/*
 * digital_output_pwm.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: fcos
 */

#include "digital_output_pwm.h"
#include "math.h"

namespace i2c_hat {
namespace driver {

#define CLK_FREQ                    (48000000)
#define DEFAULT_PWM_FREQUENCY       (10000)
#define DEFAULT_PWM_DUTY_CYCLE      (50)


DigitalOutputPwm::DigitalOutputPwm(TIM_TypeDef* tim, uint16_t channel, const bool inverted) :
        tim_(tim),
        channel_(channel),
        inverted_(inverted),
        duty_cycle_(0) {
//    SetCompare(0);
}

void DigitalOutputPwm::Init() {

    LL_TIM_CC_EnableChannel(tim_, channel_);

    LL_TIM_OC_SetMode(tim_, channel_, LL_TIM_OCMODE_PWM1);

    if(inverted_) {
        LL_TIM_OC_ConfigOutput(tim_, channel_, LL_TIM_OCPOLARITY_LOW);
    }
    else {
        LL_TIM_OC_ConfigOutput(tim_, channel_, LL_TIM_OCPOLARITY_HIGH);
    }

    SetFrequency(DEFAULT_PWM_FREQUENCY);
    SetDutyCycle(DEFAULT_PWM_DUTY_CYCLE);

    LL_TIM_EnableCounter(tim_);
}

void DigitalOutputPwm::SetPeriod(const uint16_t value) {
    LL_TIM_SetAutoReload(tim_, value);
}

void DigitalOutputPwm::SetCompare(const uint16_t value) {
    switch(channel_) {
    case LL_TIM_CHANNEL_CH1:
        LL_TIM_OC_SetCompareCH1(tim_, value);
        break;
    case LL_TIM_CHANNEL_CH2:
        LL_TIM_OC_SetCompareCH2(tim_, value);
        break;
    case LL_TIM_CHANNEL_CH3:
        LL_TIM_OC_SetCompareCH3(tim_, value);
        break;
    case LL_TIM_CHANNEL_CH4:
        LL_TIM_OC_SetCompareCH4(tim_, value);
        break;
    default:
        break;
    }
}

uint16_t DigitalOutputPwm::GetCompare() {
    uint32_t compare;

    switch(channel_) {
    case LL_TIM_CHANNEL_CH1:
        compare = LL_TIM_OC_GetCompareCH1(tim_);
        break;
    case LL_TIM_CHANNEL_CH2:
        compare = LL_TIM_OC_GetCompareCH2(tim_);
        break;
    case LL_TIM_CHANNEL_CH3:
        compare = LL_TIM_OC_GetCompareCH3(tim_);
        break;
    case LL_TIM_CHANNEL_CH4:
        compare = LL_TIM_OC_GetCompareCH4(tim_);
        break;
    default:
        compare = 0;
    }

    return compare;
}

bool DigitalOutputPwm::SetFrequency(const float value) {
    float cycles;
    float period_threshold;
    static volatile uint16_t prescaler;
    static volatile uint16_t period;
    if( (0.1 <= value) and (value <= 480000) ) {
        cycles = (float)CLK_FREQ / value;
        if(cycles >= 10000) {
            period_threshold = 10000;
        }
        else if(cycles >= 1000) {
            period_threshold = 1000;
        }
        else {
            period_threshold = 100;
        }

        prescaler = (uint16_t)truncf(cycles / period_threshold);
        period = (uint16_t)roundf(cycles / prescaler);

        LL_TIM_DisableCounter(tim_);
        LL_TIM_DisableAllOutputs(tim_);

        LL_TIM_SetCounter(tim_, 0);
        LL_TIM_SetPrescaler(tim_, prescaler - 1);
        LL_TIM_SetAutoReload(tim_, period);
        SetDutyCycle(duty_cycle_);

        LL_TIM_EnableAllOutputs(tim_);
        LL_TIM_EnableCounter(tim_);

        return true;
    }
    return false;
}

float DigitalOutputPwm::GetFrequency() {
    return (float)CLK_FREQ / (float)((LL_TIM_GetPrescaler(tim_) + 1) * LL_TIM_GetAutoReload(tim_));
}

bool DigitalOutputPwm::SetDutyCycle(const float value) {
    uint32_t period;
    if( (0 <= value) and (value <= 100) ) {
        duty_cycle_ = value;
        period = LL_TIM_GetAutoReload(tim_);
        SetCompare(roundf((value * period) / 100));
        return true;
    }
    return false;
}


float DigitalOutputPwm::GetDutyCycle() {
    return (float)(GetCompare() * 100) / (float)LL_TIM_GetAutoReload(tim_);
}

} /* namespace driver */
} /* namespace i2c_hat */
