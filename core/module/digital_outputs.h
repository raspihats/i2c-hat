/*
 * digital_outputs.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_DIGITAL_OUTPUTS_H_
#define MODULE_DIGITAL_OUTPUTS_H_

#include "module.h"
#include "digital_output_channel.h"
#include "../driver/eeprom.h"

namespace i2c_hat {
namespace module {

class DigitalOutputs: public Module {
public:
    DigitalOutputs();
    bool ProcessRequest(Frame& request, Frame& response);
private:
    const uint32_t kChannelCount;
    DigitalOutputChannel channels_[DIGITAL_OUTPUT_CHANNEL_COUNT];
    uint32_t power_on_value_;
    uint32_t safety_value_;

    bool IsValid(const uint32_t state);
    void LoadPowerOnValue();
    bool SetPowerOnValue(const uint32_t value);
    void LoadSafetyValue();
    bool SetValue(const uint32_t value);
    uint32_t GetValue();
    bool SetSafetyValue(const uint32_t value);
    bool SetChannelState(const uint8_t index, const bool state);
    bool GetChannelState(const uint8_t index, bool& state);
    bool SetChannelFrequency(const uint8_t index, const float frequency);
    bool GetChannelFrequency(const uint8_t index, float& frequency);
    bool SetChannelDutyCycle(const uint8_t index, const float duty_cycle);
    bool GetChannelDutyCycle(const uint8_t index, float& duty_cycle);
    void Init();
    void Run();
    void ReceiveEvent(const uint32_t event);
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_DIGITAL_OUTPUTS_H_ */
