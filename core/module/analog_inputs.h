/*
 * analog_inputs.h
 *
 *  Created on: May 6, 2018
 *      Author: fcos
 */

#ifndef MOULE_ANALOG_INPUTS_H_
#define MOULE_ANALOG_INPUTS_H_

#include "module.h"
//#include "relay_output_channel.h"
#include "../driver/eeprom.h"

namespace i2c_hat {
namespace module {

class AnalogInputs: public Module {
public:
    AnalogInputs();
    bool ProcessRequest(Frame& request, Frame& response);
private:
    float voltages_[ANALOG_INPUT_CHANNEL_COUNT];
    float temperature_[ANALOG_INPUT_CHANNEL_COUNT];

    bool GetChannelTemperature(const uint8_t channel, float& temperature);
    uint32_t GetValue();
    void Init();
    void Run();
    void ReceiveEvent(const uint32_t event);
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MOULE_ANALOG_INPUTS_H_ */
