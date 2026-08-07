/*
 * digital_inputs.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_DIGITAL_INPUTS_H_
#define MODULE_DIGITAL_INPUTS_H_

#include "module.h"
#include "board.h"
#include "digital_input_channel.h"
#include "../driver/digital_output_pin.h"
#include "../utils/queue.h"

#ifdef DIGITAL_INPUT_CHANNEL_COUNT

namespace i2c_hat {
namespace module {

class DigitalInputs: public Module {
public:
    DigitalInputs();
    bool IsIRQCaptureQueueFull();
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
    utils::Queue<128> irq_capture_queue_; // use only powers of 2 for queue size
    // CiA 401 alignment (ROADMAP.md): 0x6002; per-channel filter times
    // (0x6003) live in the channels themselves
    uint32_t polarity_;

    bool IsValid(const uint32_t value);
    bool SetPolarity(const uint32_t value);
    bool SetChannelFilter(const uint8_t index, const uint32_t ms);
    bool GetChannelFilter(const uint8_t index, uint32_t& ms);
    bool GetChannelState(const uint32_t index, bool& state);
    uint32_t GetValue();
    bool GetCounter(const uint32_t index, const CounterTypes type, uint32_t& value);
    bool ResetCounter(const uint32_t channel, const CounterTypes type);
    void ResetCounters();
    void TriggerIRQ();
    void ReleaseIRQ();
    uint32_t GetIRQReg(const IRQReg reg);
    bool SetIRQReg(const IRQReg reg, const uint32_t value);
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* DIGITAL_INPUT_CHANNEL_COUNT */

#endif /* MODULE_DIGITAL_INPUTS_H_ */
