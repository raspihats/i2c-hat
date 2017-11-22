/*
 * digital_output_pwm.h
 *
 *  Created on: Nov 20, 2017
 *      Author: fcos
 */

#ifndef DRIVER_DIGITAL_OUTPUT_PWM_H_
#define DRIVER_DIGITAL_OUTPUT_PWM_H_

#include "stm32f0xx_ll_tim.h"

namespace i2c_hat {
namespace driver {

class DigitalOutputPwm {
public:
    DigitalOutputPwm(TIM_TypeDef* tim, uint16_t channel, const bool inverted=false);
    void SetPeriod(const uint32_t period);
    void SetDuty(const uint32_t duty);
//    bool GetState();
//    void SetState(bool state);
private:
    TIM_TypeDef*    tim_;
    uint16_t        channel_;
    bool            inverted_;
};

} /* namespace driver */
} /* namespace i2c_hat */

#endif /* DRIVER_DIGITAL_OUTPUT_PWM_H_ */
