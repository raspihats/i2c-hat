/*
 * digital_output_pwm.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: fcos
 */

#include "digital_output_pwm.h"

namespace i2c_hat {
namespace driver {

DigitalOutputPwm::DigitalOutputPwm(TIM_TypeDef* tim, uint16_t channel, const bool inverted) :
        tim_(tim),
        channel_(channel),
        inverted_(inverted) {
    SetDuty(0);
}

void DigitalOutputPwm::SetPeriod(const uint32_t period) {
    LL_TIM_SetAutoReload(tim_, period);
}

void DigitalOutputPwm::SetDuty(uint32_t duty) {
    uint32_t temp;

    if(inverted_) {
      duty = 100 - duty;
    }

    if(duty == 0) {
        temp = 0;
    }
    else {
        temp = (LL_TIM_GetAutoReload(tim_) * duty) / 100 + 1;
    }

    switch(channel_) {
    case LL_TIM_CHANNEL_CH1:
        LL_TIM_OC_SetCompareCH1(tim_, temp);
        break;
    case LL_TIM_CHANNEL_CH2:
        LL_TIM_OC_SetCompareCH2(tim_, temp);
        break;
    case LL_TIM_CHANNEL_CH3:
        LL_TIM_OC_SetCompareCH3(tim_, temp);
        break;
    case LL_TIM_CHANNEL_CH4:
        LL_TIM_OC_SetCompareCH4(tim_, temp);
        break;
    default:
        break;
    }
}

//bool DigitalOutputPwm::GetState() {
//    return LL_GPIO_IsOutputPinSet(port_, pin_) ^ inverted_;
//}
//
//void DigitalOutputPwm::SetState(bool state) {
//    if(state ^ inverted_) {
//        LL_GPIO_SetOutputPin(port_, pin_);
//    }
//    else {
//        LL_GPIO_ResetOutputPin(port_, pin_);
//    }
//}

} /* namespace driver */
} /* namespace i2c_hat */
