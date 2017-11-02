/*
 * digital_inputs.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "digital_inputs.h"

#ifdef DIGITAL_INPUT_CHANNEL_COUNT

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (1)

#define DEBOUNCE_TIME_MS                (2)

namespace i2c_hat {
namespace module {

/**
  * @brief  Builds a DigitalInputs Module object
  * @param  dinPort: pointer to a DigitalInputPort
  * @retval None
  */
DigitalInputs::DigitalInputs() :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS),
        kChannelCount(DIGITAL_INPUT_CHANNEL_COUNT),
        channels_{DIGITAL_INPUT_CHANNELS},
        irq_(IRQ_PIN) {
    uint32_t i;

    for(i = 0; i < kChannelCount; i++) {
        channels_[i].set_debounce(DEBOUNCE_TIME_MS / TASK_PERIOD_MS);
    }
}

/**
  * @brief  Gets specific digital input channel state
  * @param  channel: channel index
  * @param  state: reference where to store digital input channel state
  * @retval error code
  */
bool DigitalInputs::GetChannelState(const uint32_t index, bool& state) {

    if(index < kChannelCount) {
        state = channels_[index].state();
        return true;
    }
    return false;
}

/**
  * @brief  Gets all digital input channels states
  * @param  None
  * @retval digital input channel states
  */
uint32_t DigitalInputs::GetValue() {
    uint32_t value;
    uint32_t i;

    value = 0;
    for(i = 0; i < kChannelCount; i++) {
        value |= (uint32_t)channels_[i].state() << i;
    }

	irq_.SetState(false);

    return value;
}

/**
  * @brief  Gets specific channel edge counter
  * @param  channel: channel index
  * @param  type: counter type
  * @param  pointer where to store counter value
  * @retval
  */
bool DigitalInputs::GetCounter(const uint32_t channel, const di_counter_type_t type, uint32_t& value) {
    if(channel < kChannelCount) {
        if(type == DIGITAL_INPUT_COUNTER_TYPE_FALLING_EDGE) {
            value = channels_[channel].falling_edge_counter();
            return true;
        }
        if(type == DIGITAL_INPUT_COUNTER_TYPE_RISING_EDGE) {
            value = channels_[channel].rising_edge_counter();
            return true;
        }
    }
    return false;
}

/**
  * @brief  Resets specific channel edge counter
  * @param  channel: channel index
  * @param  type: counter type
  * @retval
  */
bool DigitalInputs::ResetCounter(const uint32_t index, const di_counter_type_t type) {
    if(index < kChannelCount) {
        if(type == DIGITAL_INPUT_COUNTER_TYPE_RISING_EDGE) {
            channels_[index].ResetRisingEdgeCounter();
            return true;
        }
        if(type == DIGITAL_INPUT_COUNTER_TYPE_FALLING_EDGE) {
            channels_[index].ResetFallingEdgeCounter();
            return true;
        }
    }
    return false;
}

/**
  * @brief  Resets all channels edge counters
  * @param  None
  * @retval None
  */
void DigitalInputs::ResetCounters() {
    uint32_t i;

    for(i = 0; i < kChannelCount; i++) {
        channels_[i].ResetCounters();
    }
}

/**
  * @brief  Module Init implementation
  * @param  None
  * @retval None
  */

void DigitalInputs::Init() {

}

/**
  * @brief  Module cooperative Task implementation
  * @param  None
  * @retval None
  */
void DigitalInputs::Run() {
    static uint32_t value_old = GetValue();
    uint32_t value;
    uint32_t i;

    for(i = 0; i < kChannelCount; i++) {
        channels_[i].Tick();
    }

    value = GetValue();

    // Generate IRQ
    if(value != value_old) {
        irq_.SetState(true);
    }
    value_old = value;

//    // Encoders CW and CCW counters
//    if(channel & 0x01) { // enter here every second encoder input channel, even input channel numbers
//        chAOld = (filteredInputsOld >> (channel - 1)) & 0x01;
//        chBOld = (filteredInputsOld >> channel) & 0x01;
//        chA = (_filteredInputs >> (channel - 1)) & 0x01;
//        chB = (_filteredInputs >> channel) & 0x01;
//
//        if((chA == true) and (chA != chAOld)) {
//            if(chB == false) {
//                // increment CW counter
//                _encoders[channel - 1] = (_encoders[channel - 1] + 1) & 0xFFFF;
//            }
//            else {
//                // increment CCW counter
//                _encoders[channel] = (_encoders[channel] + 1) & 0xFFFF;
//            }
//        }
//    }

}

/**
  * @brief  Module ReceiveEvent implementation
  * @param  event: event code
  * @retval None
  */
void DigitalInputs::ReceiveEvent(const uint32_t event) {
    (void)event;
}

/**
  * @brief  Processes I2C request frame
  * @param  request: reference to request frame
  * @param  response: reference to response frame
  * @retval true if response
  *         false if no response
  */
bool DigitalInputs::ProcessRequest(Frame& request, Frame& response) {
    uint32_t response_flag = false;
    static uint8_t buffer[10];
    bool state;
    uint8_t index;
    uint8_t counter_type;
    uint32_t u32_temp;

    switch(request.command()) {
    case CMD_DI_GET_VALUE:
        if(request.payload_size() == 0) {
            u32_temp = GetValue();
            buffer[0] = (uint8_t)u32_temp;
            buffer[1] = (uint8_t)(u32_temp >> 8);
            buffer[2] = (uint8_t)(u32_temp >> 16);
            buffer[3] = (uint8_t)(u32_temp >> 24);
            response.set_payload(buffer, 4);
            response_flag = true;
        }
        break;
    case CMD_DI_GET_CHANNEL_STATE:
        if(request.payload_size() == 1) {
            index = request.payload()[0];
            if(GetChannelState(index, state)) {
                buffer[0] = index;
                buffer[1] = (uint8_t)state;
                response.set_payload(buffer, 2);
                response_flag = true;
            }
        }
        break;
    case CMD_DI_GET_COUNTER:
        if(request.payload_size() == 2) {
            index = request.payload()[0];
            counter_type = request.payload()[1];
            if(GetCounter(index, (di_counter_type_t)counter_type, u32_temp)) {
                buffer[0] = index;
                buffer[1] = counter_type;
                buffer[2] = (uint8_t)u32_temp;
                buffer[3] = (uint8_t)(u32_temp >> 8);
                buffer[4] = (uint8_t)(u32_temp >> 16);
                buffer[5] = (uint8_t)(u32_temp >> 24);
                response.set_payload(buffer, 6);
                response_flag = true;
            }
        }
        break;
    case CMD_DI_RESET_COUNTER:
        if(request.payload_size() == 2) {
            index = request.payload()[0];
            counter_type = request.payload()[1];
            if(ResetCounter(index, (di_counter_type_t)counter_type)) {
                buffer[0] = index;
                buffer[1] = counter_type;
                response.set_payload(buffer, 2);
                response_flag = true;
            }
        }
        break;
    case CMD_DI_RESET_ALL_COUNTERS:
        if(request.payload_size() == 0) {
            ResetCounters();
            response.set_payload(NULL, 0);
            response_flag = true;
        }
        break;
    case CMD_DI_GET_COUNTERS_STATUS:
        // TODO implement
        response_flag = false;
        break;
    case CMD_DI_GET_ENCODER:
        if(request.payload_size() == 1) {
            index = request.payload()[0];
//            if(GetEncoder(channel, &tempU32)) {
//                buffer[0] = index;
//                response->setLength(5);
//                response_flag = true;
//            }
        }
        break;
    case CMD_DI_RESET_ENCODER:
        // TODO implement
        response_flag = false;
        break;
    case CMD_DI_RESET_ALL_ENCODERS:
        // TODO implement
        response_flag = false;
        break;
    case CMD_DI_GET_ENCODERS_STATUS:
        // TODO implement
        response_flag = false;
        break;
    default:
        response_flag = false;
    }
    return response_flag;
}

} /* namespace module */
} /* namespace i2c_hat */

#endif
