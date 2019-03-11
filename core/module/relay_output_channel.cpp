/*
 * relay_output_channel.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: fcos
 */

#include "relay_output_channel.h"

#define DEFAULT_PULL_TIME_MS      (500)
#define DEFAULT_FREQUENCY         (25000)
#define DEFAULT_HOLD_PERCENT      (70)

namespace i2c_hat {
namespace module {

RelayOutputChannel::RelayOutputChannel(driver::DigitalOutputPwm pin) :
        pin_(pin),
        pull_time_ms_(DEFAULT_PULL_TIME_MS),
        hold_duty_(DEFAULT_HOLD_PERCENT),
        counter_(0),
        state_(ST_OFF) {

}

uint32_t RelayOutputChannel::pull() {
    return pull_time_ms_;
}

void RelayOutputChannel::set_pull(const uint32_t pull) {
    pull_time_ms_ = pull;
}

uint32_t RelayOutputChannel::hold() {
    return hold_duty_;
}

void RelayOutputChannel::set_hold(const uint32_t hold) {
    hold_duty_ = hold;
}

bool RelayOutputChannel::GetState() {
    return !(state_ == ST_OFF);
}

void RelayOutputChannel::SetState(const bool state) {
    if(state) {
        if(state_ == ST_OFF) {
            counter_ = 0;
            state_ = ST_PULLING;
        }
    }
    else {
        state_ = ST_OFF;
    }
}

void RelayOutputChannel::Init(const uint32_t pull_time_ms, const float frequency, const float hold_duty) {
  pin_.Init(frequency, 0);
  pull_time_ms_ = pull_time_ms;
  hold_duty_ = hold_duty;
}

void RelayOutputChannel::Tick(const uint32_t elapsed_ms) {
    switch(state_) {
    case ST_OFF:
        pin_.SetDuty(0);
        break;
    case ST_PULLING:
        if(counter_ < pull_time_ms_) {
            pin_.SetDuty(100);
        }
        else {
            state_ = ST_HOLDING;
        }
        counter_ += elapsed_ms;
        break;
    case ST_HOLDING:
        pin_.SetDuty(hold_duty_);
        break;
    default:
        state_ = ST_OFF;
    }
}

} /* namespace module */
} /* namespace i2c_hat */
