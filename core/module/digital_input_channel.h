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
    DigitalInputChannel(driver::DigitalInputPin pin, const uint32_t debounce=5);
    bool state();
    uint32_t debounce();
    void set_debounce(const uint32_t debounce);
    bool rising_edge_irq_enable_flag();
    void set_rising_edge_irq_enable_flag(const bool value);
    bool falling_edge_irq_enable_flag();
    void set_falling_edge_irq_enable_flag(const bool value);
    bool irq_flag();
    bool reset_irq_flag();
    uint32_t rising_edge_counter();
    uint32_t falling_edge_counter();
    void ResetRisingEdgeCounter();
    void ResetFallingEdgeCounter();
    void ResetCounters();
    void Tick();
private:
    driver::DigitalInputPin pin_;
    uint32_t debounce_;
    uint32_t integrator_;
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
