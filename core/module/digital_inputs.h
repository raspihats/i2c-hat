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

namespace i2c_hat {
namespace module {

class DigitalInputs: public Module {
public:
    DigitalInputs();
    void Init();
    void Run();
    void ReceiveEvent(const uint32_t event);
    bool ProcessRequest(Frame& request, Frame& response);
private:
    enum class CounterTypes {
        FALLING_EDGE,
        RISING_EDGE,
    };

    const uint32_t kChannelCount;
    DigitalInputChannel channels_[DIGITAL_INPUT_CHANNEL_COUNT];
    driver::DigitalOutputPin irq_;
    uint32_t irq_status_;
    uint32_t irq_capture_;

    bool IsValid(const uint32_t value);
    void ClearIRQ();
    bool GetChannelState(const uint32_t index, bool& state);
    uint32_t GetValue();
    bool GetCounter(const uint32_t index, const CounterTypes type, uint32_t& value);
    bool ResetCounter(const uint32_t channel, const CounterTypes type);
    void ResetCounters();
    uint32_t GetIRQReg(const IRQReg reg);
    bool SetIRQReg(const IRQReg reg, const uint32_t value);
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_DIGITAL_INPUTS_H_ */
