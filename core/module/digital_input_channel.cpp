/*
 * digital_input_channel.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: fcos
 */

#include "digital_input_channel.h"

namespace i2c_hat {
namespace module {

DigitalInputChannel::DigitalInputChannel(driver::DigitalInputPin pin) :
        pin_(pin),
        debounce_(100),
        integrator_(0),
        polarity_(false),
        state_(false),
        rising_edge_irq_enable_flag_(false),
        falling_edge_irq_enable_flag_(false),
        irq_flag_(false),
        rising_edge_counter_(0),
        falling_edge_counter_(0) {
}

bool DigitalInputChannel::state() {
    return state_;
}

uint32_t DigitalInputChannel::debounce() {
    return debounce_;
}

void DigitalInputChannel::set_debounce(const uint32_t value) {
    debounce_ = (value > 0) ? value : 1;    // 0 would jam the integrator
    if(integrator_ > debounce_) {
        integrator_ = debounce_;
    }
}

bool DigitalInputChannel::polarity() {
    return polarity_;
}

void DigitalInputChannel::set_polarity(const bool value) {
    polarity_ = value;
    // re-seat the debouncer on the new logical level WITHOUT counting an
    // edge - changing polarity is a commissioning act, not a signal change
    state_ = (pin_.GetState() != polarity_);
    integrator_ = state_ ? debounce_ : 0;
}

bool DigitalInputChannel::rising_edge_irq_enable_flag() {
    return rising_edge_irq_enable_flag_;
}

void DigitalInputChannel::set_rising_edge_irq_enable_flag(const bool value) {
    rising_edge_irq_enable_flag_ = value;
}

bool DigitalInputChannel::falling_edge_irq_enable_flag() {
    return falling_edge_irq_enable_flag_;
}

void DigitalInputChannel::set_falling_edge_irq_enable_flag(const bool value) {
    falling_edge_irq_enable_flag_ = value;
}

bool DigitalInputChannel::irq_flag() {
    bool temp = irq_flag_;
    irq_flag_ = false;
    return temp;
}

uint32_t DigitalInputChannel::rising_edge_counter() {
    return rising_edge_counter_;
}

uint32_t DigitalInputChannel::falling_edge_counter() {
    return falling_edge_counter_;
}

void DigitalInputChannel::ResetRisingEdgeCounter() {
    rising_edge_counter_ = 0;
}

void DigitalInputChannel::ResetFallingEdgeCounter() {
    falling_edge_counter_ = 0;
}

void DigitalInputChannel::ResetCounters() {
    rising_edge_counter_ = 0;
    falling_edge_counter_ = 0;
}

void DigitalInputChannel::Init(const uint32_t debounce) {
    debounce_ = (debounce > 0) ? debounce : 1;
    state_ = (pin_.GetState() != polarity_);
    if(state_) {
        integrator_ = debounce;
    }
    else {
        integrator_ = 0;
    }
}

void DigitalInputChannel::Tick() {
    /* Step 1:
     * Update the integrator based on the input signal. Note that the integrator
     * follows the input, decreasing or increasing towards the limits as
     * determined by the input state (0 or 1). Polarity (CiA 401 0x6002) is
     * applied here, so everything downstream is logical. */
    if(pin_.GetState() != polarity_) {
        if (integrator_ < debounce_) {
            integrator_++;
        }
    }
    else {
        if (integrator_ > 0) {
            integrator_--;
        }
    }

    /* Step 2:
     * Update the output state based on the integrator. Note that the output will
     * only change states if the integrator has reached a limit, either 0 or MAXIMUM. */
    if (integrator_ >= debounce_) {
        if(state_ == false) {
            state_ = true;  // state of debounced input is 1
            rising_edge_counter_++;
            if(rising_edge_irq_enable_flag_) {
                irq_flag_ = true;
            }
        }
    }
    else if (integrator_ == 0) {
        if(state_ == true) {
            state_ = false;  //state of debounced input is 0
            falling_edge_counter_++;
            if(falling_edge_irq_enable_flag_) {
                irq_flag_ = true;
            }
        }
    }
}

} /* namespace module */
} /* namespace i2c_hat */
