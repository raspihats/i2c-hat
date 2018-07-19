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
  * @brief  Loads PowerOn Value
  * @param  None
  * @retval None
  */
void DigitalOutputs::LoadPowerOnValue() {
    SetValue(power_on_value_);
}

/**
  * @brief  Sets the PowerOn Value
  * @param  powerOnValue: the PowerOn Value
  * @retval None
  */
bool DigitalOutputs::SetPowerOnValue(const uint32_t value) {
    if(IsValid(value)) {
        if(driver::Eeprom::Write(driver::EEP_VIRT_ADR_DO_POWER_ON_VALUE, value)) {
            power_on_value_ = value;
            return true;
        }
        else {
            // TODO Error handler
        }
    }
    return false;
}

/**
  * @brief  Loads Safety Value
  * @param  None
  * @retval None
  */
void DigitalOutputs::LoadSafetyValue() {
    SetValue(safety_value_);
}

/**
  * @brief  Sets the Safety Value
  * @param  safetyValue: the Safety Value
  * @retval None
  */
bool DigitalOutputs::SetSafetyValue(const uint32_t value) {
    if(IsValid(value)) {
        if(driver::Eeprom::Write(driver::EEP_VIRT_ADR_DO_SAFETY_VALUE, value)) {
            safety_value_ = value;
            return true;
        }
        else {
            // TODO Error handler
        }
    }
    return false;
}

/**
  * @brief  Sets all channel states
  * @param  states:
  * @retval None
  */
bool DigitalOutputs::SetValue(const uint32_t value) {
    uint32_t i, mask;

    if(IsValid(value)) {
        for(i = 0; i < kChannelCount; i++) {
            mask = 0x01 << i;
//            channels_[i].SetState((value & mask) > 0);
        }
        return true;
    }
    return false;
}

/**
  * @brief  Gets all channel states
  * @param  None
  * @retval all channel states
  */
uint32_t DigitalOutputs::GetValue() {
    uint32_t i, value;

    value = 0;
    for(i = 0; i < kChannelCount; i++) {
//        value |= channels_[i].GetState() ? 0x01 << i : 0x00 ;
    }
    return value;
}

/**
  * @brief  Sets state of a single channel
  * @param  index: channel index
  * @param  state: channel state
  * @retval error code
  */
bool DigitalOutputs::SetChannelState(const uint8_t index, const bool state) {

    if(index < kChannelCount) {
//        channels_[index].SetState(state);
        return true;
    }
    return false;
}

/**
  * @brief  Gets state of a single channel
  * @param  index: channel index
  * @param  state: reference to store state
  * @retval error code
  */
bool DigitalOutputs::GetChannelState(const uint8_t index, bool& state) {
    if(index < kChannelCount) {
//        state = channels_[index].GetState();
        return true;
    }
    return false;
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
        LoadSafetyValue();
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
    static uint32_t u32_temp;
    static float f_temp;
    static uint8_t data[5];
    uint8_t index;
    bool state;
    bool response_flag;

    response_flag = false;
    switch((Command)request.command()) {
//    case Command::DO_SET_POWER_ON_VALUE:
//        if(request.payload_size() == 4) {
//            BYTES_TO_UINT32(request.payload(), u32_temp);
//            if(SetPowerOnValue(u32_temp)) {
//                response.set_payload((uint8_t*)&power_on_value_, 4);
//                response_flag = true;
//            }
//        }
//        break;
//    case Command::DO_GET_POWER_ON_VALUE:
//        if(request.payload_size() == 0) {
//            response.set_payload((uint8_t*)&power_on_value_, 4);
//            response_flag = true;
//        }
//        break;
//    case Command::DO_SET_SAFETY_VALUE:
//        if(request.payload_size() == 4) {
//            BYTES_TO_UINT32(request.payload(), u32_temp);
//            if(SetSafetyValue(u32_temp)) {
//                response.set_payload((uint8_t*)&safety_value_, 4);
//                response_flag = true;
//            }
//        }
//        break;
//    case Command::DO_GET_SAFETY_VALUE:
//        if(request.payload_size() == 0) {
//            response.set_payload((uint8_t*)&safety_value_, 4);
//            response_flag = true;
//        }
//        break;
//    case Command::DO_SET_VALUE:
//        if(request.payload_size() == 4) {
//            BYTES_TO_UINT32(request.payload(), u32_temp);
//            if(SetValue(u32_temp)) {
//                u32_temp = GetValue();
//                response.set_payload((uint8_t*)&u32_temp, 4);
//                response_flag = true;
//            }
//        }
//        break;
//    case Command::DO_GET_VALUE:
//        if(request.payload_size() == 0) {
//            u32_temp = GetValue();
//            response.set_payload((uint8_t*)&u32_temp, 4);
//            response_flag = true;
//        }
//        break;
//    case Command::DO_SET_CHANNEL_STATE:
//        if(request.payload_size() == 2) {
//            index = request.payload()[0];
//            state = request.payload()[1];
//            if(SetChannelState(index, state)) {
//                uint8_t *data = (uint8_t*)&u32_temp;
//                data[0] = index;
//                data[1] = state;
//                response.set_payload(request.payload(), 2);
//                response_flag = true;
//            }
//        }
//        break;
//    case Command::DO_GET_CHANNEL_STATE:
//        if(request.payload_size() == 1) {
//            index = request.payload()[0];
//            if(GetChannelState(index, state)) {
//                uint8_t *data = (uint8_t*)&u32_temp;
//                data[0] = index;
//                data[1] = state;
//                response.set_payload(data, 2);
//                response_flag = true;
//            }
//        }
//        break;
    case Command::DO_SET_PWM_CHANNEL_FREQUENCY:
        if(request.payload_size() == 5) {
            index = request.payload()[0];
            BYTES_TO_FLOAT(request.payload() + 1, f_temp);
            if(SetChannelFrequency(index, f_temp)) {
                if(GetChannelFrequency(index, f_temp)) {
                    data[0] = index;
                    FLOAT_TO_BYTES(f_temp, data + 1);
                    response.set_payload(request.payload(), 5);
                    response_flag = true;
                }
            }
        }
        break;
    case Command::DO_GET_PWM_CHANNEL_FREQUENCY:
        if(request.payload_size() == 1) {
            index = request.payload()[0];
            if(GetChannelFrequency(index, f_temp)) {
                data[0] = index;
                FLOAT_TO_BYTES(u32_temp, data + 1);
                response.set_payload(request.payload(), 5);
                response_flag = true;
            }
        }
        break;
    case Command::DO_SET_PWM_CHANNEL_DUTY_CYCLE:
        if(request.payload_size() == 5) {
            index = request.payload()[0];
            BYTES_TO_FLOAT(request.payload() + 1, f_temp);
            if(SetChannelDutyCycle(index, f_temp)) {
                if(GetChannelDutyCycle(index, f_temp)) {
                    data[0] = index;
                    FLOAT_TO_BYTES(f_temp, data + 1);
                    response.set_payload(request.payload(), 5);
                    response_flag = true;
                }
            }
        }
        break;
    case Command::DO_GET_PWM_CHANNEL_DUTY_CYCLE:
        if(request.payload_size() == 1) {
            index = request.payload()[0];
            if(GetChannelDutyCycle(index, f_temp)) {
                data[0] = index;
                FLOAT_TO_BYTES(u32_temp, data + 1);
                response.set_payload(request.payload(), 5);
                response_flag = true;
            }
        }
        break;
    default:
        response_flag = 0;
    }
    return response_flag;
}

} /* namespace module */
} /* namespace i2c_hat */
