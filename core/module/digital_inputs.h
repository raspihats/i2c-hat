/*
 * digital_inputs.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_DIGITAL_INPUTS_H_
#define MODULE_DIGITAL_INPUTS_H_

#include "module.h"
#include "digital_input_channel.h"
#include "../driver/digital_output_pin.h"

#ifdef DIGITAL_INPUT_CHANNEL_COUNT

namespace i2c_hat {
namespace module {

typedef enum {
    DIGITAL_INPUT_COUNTER_TYPE_FALLING_EDGE,
    DIGITAL_INPUT_COUNTER_TYPE_RISING_EDGE,
    DIGITAL_INPUT_COUNTER_TYPES_COUNT
} di_counter_type_t;

class DigitalInputs: public Module {
public:
    DigitalInputs();
    void Init();
    void Run();
    void ReceiveEvent(const uint32_t event);
    bool ProcessRequest(Frame& request, Frame& response);
private:
    const uint32_t kChannelCount;
    DigitalInputChannel channels_[DIGITAL_INPUT_CHANNEL_COUNT];
    driver::DigitalOutputPin irq_;

    bool GetChannelState(const uint32_t index, bool& state);
    uint32_t GetValue();
    bool GetCounter(const uint32_t index, const di_counter_type_t type, uint32_t& value);
    bool ResetCounter(const uint32_t channel, const di_counter_type_t type);
    void ResetCounters();
};

} /* namespace module */
} /* namespace i2c_hat */

#endif

#endif /* MODULE_DIGITAL_INPUTS_H_ */
