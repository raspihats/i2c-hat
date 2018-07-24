/*
 * digital_outputs.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */
#include "digital_outputs.h"

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (10)

#define DEFAULT_POWER_ON_VALUE          (0)
#define DEFAULT_SAFETY_VALUE            (0)

namespace i2c_hat {
namespace module {

/**
  * @brief  Builds a DigitalOutputs Module object
  * @param  port: pointer to a DigitalOutputPort
  * @retval None
  */
DigitalOutputs::DigitalOutputs() :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS),
        kChannelCount(DIGITAL_OUTPUT_CHANNEL_COUNT),
        channels_{DIGITAL_OUTPUT_CHANNELS},
        power_on_value_(DEFAULT_POWER_ON_VALUE),
        safety_value_(DEFAULT_SAFETY_VALUE) {
}

/**
  * @brief  Checks if value is valid to be written to port
  * @param  value: value to be checked
  * @retval None
  */
bool DigitalOutputs::IsValid(const uint32_t value) {
    return value < ((uint32_t)0x01 << kChannelCount);
}


/**
  * @brief  Sets frequency of a all channels
  * @param  frequency desired channels frequency
  * @retval True on success
  * @retval False on fail
  */
bool DigitalOutputs::SetFrequency(const float frequency) {
    uint32_t i;
    for (i = 0; i < kChannelCount; i++) {
        if (!channels_[i].SetFrequency(frequency)) {
            return false;
        }
    }
    return true;
}


/**
  * @brief  Sets duty cycle of a all channels
  * @param  duty_cycle desired channels duty cycle
  * @retval True on success
  * @retval False on fail
  */
bool DigitalOutputs::SetDutyCycle (const float duty_cycle) {
    uint32_t i;
    for (i = 0; i < kChannelCount; i++) {
        if (!channels_[i].SetDutyCycle(duty_cycle)) {
            return false;
        }
    }
    return true;
}


/**
  * @brief  Sets frequency of a single channel
  * @param  index channel index
  * @param  frequency channel frequency
  * @retval True on success
  * @retval False on fail
  */
bool DigitalOutputs::SetChannelFrequency(const uint8_t index, const float frequency) {
    if(index < kChannelCount) {
        return channels_[index].SetFrequency(frequency);
    }
    return false;
}

/**
  * @brief  Gets frequency of a single channel
  * @param  index channel index
  * @param  frequency reference to store frequency
  * @retval True on success
  * @retval False on fail
  */
bool DigitalOutputs::GetChannelFrequency(const uint8_t index, float& frequency) {
    if(index < kChannelCount) {
        frequency = channels_[index].GetFrequency();
        return true;
    }
    return false;
}

/**
  * @brief  Sets duty cycle of a single channel
  * @param  index channel index
  * @param  duty cycle channel frequency
  * @retval True on success
  * @retval False on fail
  */
bool DigitalOutputs::SetChannelDutyCycle(const uint8_t index, const float duty_cycle) {
    if(index < kChannelCount) {
        return channels_[index].SetDutyCycle(duty_cycle);
    }
    return false;
}


/**
  * @brief  Gets duty cycle of a single channel
  * @param  index channel index
  * @param  duty cycle reference to store duty cycle
  * @retval True on success
  * @retval False on fail
  */
bool DigitalOutputs::GetChannelDutyCycle(const uint8_t index, float& duty_cycle) {
    if(index < kChannelCount) {
        duty_cycle = channels_[index].GetDutyCycle();
        return true;
    }
    return false;
}


/**
  * @brief  Module Init implementation
  * @param  None
  * @retval None
  */
void DigitalOutputs::Init() {
//    bool success;

//    success = driver::Eeprom::Read(driver::EEP_VIRT_ADR_DO_POWER_ON_VALUE, power_on_value_);
//    if(not success) {
//        // TODO Error Handling
//    }
//
//    success = driver::Eeprom::Read(driver::EEP_VIRT_ADR_DO_SAFETY_VALUE, safety_value_);
//    if(not success) {
//        // TODO Error Handling
//    }

//    LoadPowerOnValue();

    uint32_t i;
    for(i = 0; i < kChannelCount; i++) {
        channels_[i].Init();
    }
}

/**
  * @brief  Module cooperative Task implementation
  * @param  None
  * @retval None
  */
void DigitalOutputs::Run() {

}

/**
  * @brief  Module receiveEvent implementation
  * @param  event: event code
  * @retval None
  */
void DigitalOutputs::ReceiveEvent(const uint32_t event) {
    if(event == EVENT_CWDT_TIMEOUT) {

    }
}

/**
  * @brief  Processes I2C request frame
  * @param  request: pointer to request frame
  * @param  response: pointer to response frame
  * @retval 1 if response
  *         0 if no response
  */
bool DigitalOutputs::ProcessRequest(Frame& request, Frame& response) {
    uint32_t i, channel_count;
    static float f_temp;
    static uint8_t data[64];
    uint8_t channel_index;
    bool response_flag;

    response_flag = false;
    switch((Command)request.command()) {

    case Command::DO_SET_FREQUENCY:
        if(request.payload_size() == 4) {   // Set all channels at same frequency
            BYTES_TO_FLOAT(request.payload(), f_temp);
            if(SetFrequency(f_temp)) {
                response.set_payload(NULL, 0);
                response_flag = true;
            }
        }
        else if(request.payload_size() == 5) {   // Set single channel frequency
            channel_index = request.payload()[0];
            BYTES_TO_FLOAT(request.payload() + 1, f_temp);
            if(SetChannelFrequency(channel_index, f_temp)) {
                if(GetChannelFrequency(channel_index, f_temp)) {
                    response.set_payload(NULL, 0);
                    response_flag = true;
                }
            }
        }
        else if(request.payload_size() == 6) {   // Set multiple channels at the same frequency
            channel_index = request.payload()[0];
            channel_count = request.payload()[1];
            if((channel_index + channel_count) <= kChannelCount) {
                BYTES_TO_FLOAT(request.payload() + 2, f_temp);
                response_flag = true;
                for(i = 0; i < channel_count; i++) {
                    if(!SetChannelFrequency(channel_index + i, f_temp)) {
                        response_flag = false;
                        break;
                    }
                }
                if (response_flag) {
                    response.set_payload(NULL, 0);
                }
            }
        }
        else if(request.payload_size() > 9) {   // Set multiple channels at different frequencies
            if(((request.payload_size() - 2) % 4) == 0) {   // check for proper size, first byte is channel_index followed by sets of 4 bytes
                channel_count = (request.payload_size() - 2) >> 2;
                channel_index = request.payload()[0];
                if((channel_index + channel_count) <= kChannelCount) {
                    response_flag = true;
                    for(i = 0; i < channel_count; i++) {
                        BYTES_TO_FLOAT(request.payload() + 2 + (i * 4), f_temp);
                        if(!SetChannelFrequency(channel_index + i, f_temp)) {
                            response_flag = false;
                            break;
                        }
                    }
                    if (response_flag) {
                        response.set_payload(NULL, 0);
                    }
                }
            }
        }
        break;

    case Command::DO_GET_FREQUENCY:
        if(request.payload_size() == 0) {   // Get all channels frequency
            response_flag = true;
            for(i = 0; i < kChannelCount; i++) {
                if(GetChannelFrequency(i, f_temp)) {
                    FLOAT_TO_BYTES(f_temp, data + (i * 4));
                }
                else {
                    response_flag = false;
                    break;
                }
            }
            if(response_flag) {
                response.set_payload((const uint8_t*)data, kChannelCount * 4);
            }
        }
        else if(request.payload_size() == 1) {  // Get single channel frequency
            channel_index = request.payload()[0];
            if(GetChannelFrequency(channel_index, f_temp)) {
                FLOAT_TO_BYTES(f_temp, data);
                response.set_payload((const uint8_t*)data, 4);
                response_flag = true;
            }
        }
        else if(request.payload_size() == 2) {  // Get multiple channels frequency
            channel_index = request.payload()[0];
            channel_count = request.payload()[1];
            if((channel_index + channel_count) <= kChannelCount) {
                response_flag = true;
                for(i = 0; i < kChannelCount; i++) {
                    if(GetChannelFrequency(i, f_temp)) {
                        FLOAT_TO_BYTES(f_temp, data + (i * 4));
                    }
                    else {
                        response_flag = false;
                        break;
                    }
                }
                if(response_flag) {
                    response.set_payload((const uint8_t*)data, channel_count * 4);
                }
            }
        }
        break;


    case Command::DO_SET_DUTY_CYCLE:
        if(request.payload_size() == 4) {   // Set all channels at same duty cycle
            BYTES_TO_FLOAT(request.payload(), f_temp);
            if(SetDutyCycle(f_temp)) {
                response.set_payload(NULL, 0);
                response_flag = true;
            }
        }
        else if(request.payload_size() == 5) {   // Set single channel duty cycle
            channel_index = request.payload()[0];
            BYTES_TO_FLOAT(request.payload() + 1, f_temp);
            if(SetChannelDutyCycle(channel_index, f_temp)) {
                response.set_payload(NULL, 0);
                response_flag = true;
            }
        }
        else if(request.payload_size() == 6) {   // Set multiple channels at the same duty cycle
            channel_index = request.payload()[0];
            channel_count = request.payload()[1];
            if((channel_index + channel_count) <= kChannelCount) {
                BYTES_TO_FLOAT(request.payload() + 2, f_temp);
                response_flag = true;
                for(i = 0; i < channel_count; i++) {
                    if(!SetChannelDutyCycle(channel_index + i, f_temp)) {
                        response_flag = false;
                        break;
                    }
                }
                if (response_flag) {
                    response.set_payload(NULL, 0);
                }
            }
        }
        else if(request.payload_size() > 9) {   // Set multiple channels at different duty cycles
            if(((request.payload_size() - 2) % 4) == 0) {   // check for proper size, first byte is channel_index followed by sets of 4 bytes
                channel_count = (request.payload_size() - 2) >> 2;
                channel_index = request.payload()[0];
                if((channel_index + channel_count) <= kChannelCount) {
                    response_flag = true;
                    for(i = 0; i < channel_count; i++) {
                        BYTES_TO_FLOAT(request.payload() + 2 + (i * 4), f_temp);
                        if(!SetChannelDutyCycle(channel_index + i, f_temp)) {
                            response_flag = false;
                            break;
                        }
                    }
                    if (response_flag) {
                        response.set_payload(NULL, 0);
                    }
                }
            }
        }
        break;

    case Command::DO_GET_DUTY_CYCLE:
        if(request.payload_size() == 0) {   // Get all channels duty cycle
            response_flag = true;
            for(i = 0; i < kChannelCount; i++) {
                if(GetChannelDutyCycle(i, f_temp)) {
                    FLOAT_TO_BYTES(f_temp, data + (i * 4));
                }
                else {
                    response_flag = false;
                    break;
                }
            }
            if(response_flag) {
                response.set_payload((const uint8_t*)data, kChannelCount * 4);
            }
        }
        else if(request.payload_size() == 1) {  // Get single channel duty cycle
            channel_index = request.payload()[0];
            if(GetChannelDutyCycle(channel_index, f_temp)) {
                FLOAT_TO_BYTES(f_temp, data);
                response.set_payload((const uint8_t*)data, 4);
                response_flag = true;
            }
        }
        else if(request.payload_size() == 2) {  // Get multiple channels duty cycle
            channel_index = request.payload()[0];
            channel_count = request.payload()[1];
            if((channel_index + channel_count) <= kChannelCount) {
                response_flag = true;
                for(i = 0; i < kChannelCount; i++) {
                    if(GetChannelDutyCycle(i, f_temp)) {
                        FLOAT_TO_BYTES(f_temp, data + (i * 4));
                    }
                    else {
                        response_flag = false;
                        break;
                    }
                }
                if(response_flag) {
                    response.set_payload((const uint8_t*)data, channel_count * 4);
                }
            }
        }
        break;

    default:
        response_flag = false;
    }
    return response_flag;
}

} /* namespace module */
} /* namespace i2c_hat */
