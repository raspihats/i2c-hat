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
    void Init();
    bool SetFrequency(const float value);
    float GetFrequency();
    bool SetDutyCycle(const float value);
    float GetDutyCycle();
private:
    TIM_TypeDef*    tim_;
    uint16_t        channel_;
    bool            inverted_;
    float           duty_cycle_;
    void SetPeriod(const uint16_t value);
    void SetCompare(const uint16_t value);
    uint16_t GetCompare();
};

} /* namespace driver */
} /* namespace i2c_hat */

#endif /* DRIVER_DIGITAL_OUTPUT_PWM_H_ */
