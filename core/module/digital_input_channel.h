/*
 * digital_input_channel.h
 *
 *  Created on: Nov 1, 2017
 *      Author: fcos
 */

#ifndef MODULE_DIGITAL_INPUT_CHANNEL_H_
#define MODULE_DIGITAL_INPUT_CHANNEL_H_

#include "../driver/digital_input_pin.h"

namespace i2c_hat {
namespace module {

class DigitalInputChannel {
public:
    DigitalInputChannel(driver::DigitalInputPin pin);
    bool state();
    uint32_t debounce();
    void set_debounce(const uint32_t value);
    bool polarity();
    void set_polarity(const bool value);
    bool rising_edge_irq_enable_flag();
    void set_rising_edge_irq_enable_flag(const bool value);
    bool falling_edge_irq_enable_flag();
    void set_falling_edge_irq_enable_flag(const bool value);
    bool irq_flag();
    uint32_t rising_edge_counter();
    uint32_t falling_edge_counter();
    void ResetRisingEdgeCounter();
    void ResetFallingEdgeCounter();
    void ResetCounters();
    void Init(const uint32_t debounce);
    void Tick();
private:
    driver::DigitalInputPin pin_;
    uint32_t debounce_;
    uint32_t integrator_;
    bool polarity_;     // CiA 401 0x6002: logical = pin XOR polarity, applied
                        // BEFORE the debounce integrator so state, counters
                        // and IRQ edges all follow the logical signal
    bool state_;
    bool rising_edge_irq_enable_flag_;
    bool falling_edge_irq_enable_flag_;
    bool irq_flag_;
    uint32_t rising_edge_counter_;
    uint32_t falling_edge_counter_;
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_DIGITAL_INPUT_CHANNEL_H_ */
