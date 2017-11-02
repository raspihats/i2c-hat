/*
 * digital_input_channel.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: fcos
 */

#include "digital_input_channel.h"

namespace i2c_hat {
namespace module {

DigitalInputChannel::DigitalInputChannel(driver::DigitalInputPin pin, const uint32_t debounce) :
        pin_(pin),
        debounce_(debounce),
        integrator_(0),
        state_(false),
        rising_edge_counter_(0),
        falling_edge_counter_(0) {
    state_ = pin_.GetState();
}

bool DigitalInputChannel::state() {
    return state_;
}

uint32_t DigitalInputChannel::debounce() {
    return debounce_;
}

void DigitalInputChannel::set_debounce(const uint32_t debounce) {
    debounce_ = debounce;
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

void DigitalInputChannel::Tick() {
    /* Step 1:
     * Update the integrator based on the input signal. Note that the integrator
     * follows the input, decreasing or increasing towards the limits as
     * determined by the input state (0 or 1). */
    if(pin_.GetState()) {
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
        }
    }
    else if (integrator_ == 0) {
        if(state_ == true) {
            state_ = false;  //state of debounced input is 0
            falling_edge_counter_++;
        }
    }
}

} /* namespace module */
} /* namespace i2c_hat */
