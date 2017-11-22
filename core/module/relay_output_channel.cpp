/*
 * relay_output_channel.cpp
 *
 *  Created on: Nov 20, 2017
 *      Author: fcos
 */

#include "relay_output_channel.h"

namespace i2c_hat {
namespace module {

RelayOutputChannel::RelayOutputChannel(driver::DigitalOutputPwm pin, const uint32_t pull, const uint32_t hold) :
        pin_(pin),
        pull_(pull),
        hold_(hold),
        counter_(0),
        state_(ST_OFF) {
    pin_.SetDuty(0);
}

uint32_t RelayOutputChannel::pull() {
    return pull_;
}

void RelayOutputChannel::set_pull(const uint32_t pull) {
    pull_ = pull;
}

uint32_t RelayOutputChannel::hold() {
    return hold_;
}

void RelayOutputChannel::set_hold(const uint32_t hold) {
    hold_ = hold;
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

void RelayOutputChannel::Tick() {
    switch(state_) {
    case ST_OFF:
        pin_.SetDuty(0);
        break;
    case ST_PULLING:
        if(counter_ < pull_) {
            pin_.SetDuty(100);
        }
        else {
            state_ = ST_HOLDING;
        }
        counter_++;
        break;
    case ST_HOLDING:
        pin_.SetDuty(hold_);
        break;
    default:
        state_ = ST_OFF;
    }
}

} /* namespace module */
} /* namespace i2c_hat */
