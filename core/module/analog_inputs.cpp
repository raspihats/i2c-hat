/*
 * analog_inputs.cpp
 *
 *  Created on: May 6, 2018
 *      Author: fcos
 */
#include "analog_inputs.h"
#include "driver/thermocouple/k_type.h"

#define SDADC_OVERSAMPLING      64

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (1)

namespace i2c_hat {
namespace module {

/**
  * @brief  Builds a AnalogInputs Module object
  * @param  port: pointer to a DigitalOutputPort
  * @retval None
  */
AnalogInputs::AnalogInputs() :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS)
        {
}

/**
  * @brief  Gets temperature of a single channel
  * @param  index: channel index
  * @param  teperature: reference to store temperature
  * @retval error code
  */
bool AnalogInputs::GetChannelTemperature(const uint8_t channel, float& temperature) {
    if(channel < ANALOG_INPUT_CHANNEL_COUNT) {
        temperature = temperature_[channel];
        return true;
    }
    return false;
}

/**
  * @brief  Gets all channel states
  * @param  None
  * @retval all channel states
  */
uint32_t AnalogInputs::GetValue() {
    uint32_t i, value;

    value = 0;
    for(i = 0; i < ANALOG_INPUT_CHANNEL_COUNT; i++) {
//        value |= channels_[i].GetState() ? 0x01 << i : 0x00 ;
    }
    return value;
}

/**
  * @brief  Module Init implementation
  * @param  None
  * @retval None
  */
void AnalogInputs::Init() {
    bool success;
    uint32_t i;

//    success = driver::Eeprom::Read(driver::EEP_VIRT_ADR_DO_POWER_ON_VALUE, power_on_value_);
//    if(not success) {
//        // TODO Error Handling
//    }
//
//    success = driver::Eeprom::Read(driver::EEP_VIRT_ADR_DO_SAFETY_VALUE, safety_value_);
//    if(not success) {
//        // TODO Error Handling
//    }
//
//    for(i = 0; i < kChannelCount; i++) {
//        channels_[i].Init(RELAY_PULL_MS / TASK_PERIOD_MS, 70);
//    }

    SDADC_ChannelSelect(SDADC1, SDADC_Channel_6);
    SDADC_SoftwareStartConv(SDADC1);
}

/**
  * @brief  Module cooperative Task implementation
  * @param  None
  * @retval None
  */
void AnalogInputs::Run() {
    static uint32_t oversampling_cnt = 0;
    static int32_t accumulator = 0;
    int16_t sdadc_value;
    static float vdda = 3.3;
    static float cjc_voltage;
    static float cjc_temperature;
    static float tc_voltage;
    static float tc_temperature;
    static K_Type thcp;

    if(SDADC_GetFlagStatus(SDADC1, SDADC_FLAG_REOC) == SET) {
        sdadc_value = SDADC_GetConversionValue(SDADC1);

        accumulator += sdadc_value;
        oversampling_cnt++;
        if(oversampling_cnt == SDADC_OVERSAMPLING) {
            accumulator = accumulator / SDADC_OVERSAMPLING;
            tc_voltage = accumulator * (vdda / 65535);
            tc_voltage = tc_voltage / 32.83;
            voltages_[0] = tc_voltage;

            cjc_temperature = 24;
            thcp.ConvertTempToVoltage(cjc_temperature, cjc_voltage);
            thcp.ConvertVoltageToTemp(tc_voltage + cjc_voltage, tc_temperature);
            temperature_[0] = tc_temperature;

            oversampling_cnt = 0;
            accumulator = 0;
        }

        SDADC_ChannelSelect(SDADC1, SDADC_Channel_6);
        SDADC_SoftwareStartConv(SDADC1);
    }

//    for(i = 0; i < kChannelCount; i++) {
//        channels_[i].Tick();
//    }
}

/**
  * @brief  Module receiveEvent implementation
  * @param  event: event code
  * @retval None
  */
void AnalogInputs::ReceiveEvent(const uint32_t event) {
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
bool AnalogInputs::ProcessRequest(Frame& request, Frame& response) {
    static uint8_t data[8];
    uint8_t* u8p;
    uint8_t channel;
    float temperature;
    bool response_flag;

    response_flag = false;
    switch((Command)request.command()) {
//    case Command::DO_SET_POWER_ON_VALUE:
//        if(request.payload_size() == 4) {
//            BYTES_TO_UINT32(request.payload(), temp);
//            if(SetPowerOnValue(temp)) {
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
//            BYTES_TO_UINT32(request.payload(), temp);
//            if(SetSafetyValue(temp)) {
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
//            BYTES_TO_UINT32(request.payload(), temp);
//            if(SetValue(temp)) {
//                temp = GetValue();
//                response.set_payload((uint8_t*)&temp, 4);
//                response_flag = true;
//            }
//        }
//        break;
//    case Command::DO_GET_VALUE:
//        if(request.payload_size() == 0) {
//            temp = GetValue();
//            response.set_payload((uint8_t*)&temp, 4);
//            response_flag = true;
//        }
//        break;
//    case Command::DO_SET_CHANNEL_STATE:
//        if(request.payload_size() == 2) {
//            index = request.payload()[0];
//            state = request.payload()[1];
//            if(SetChannelState(index, state)) {
//                uint8_t *data = (uint8_t*)&temp;
//                data[0] = index;
//                data[1] = state;
//                response.set_payload(request.payload(), 2);
//                response_flag = true;
//            }
//        }
//        break;
    case Command::AI_GET_CHANNEL_TEMPERATURE:
        if(request.payload_size() == 1) {
            channel = request.payload()[0];
            if(GetChannelTemperature(channel, temperature)) {
                data[0] = channel;
                u8p = (uint8_t *)&temperature;
                data[1] = u8p[0];
                data[2] = u8p[1];
                data[3] = u8p[2];
                data[4] = u8p[3];
                response.set_payload(data, 5);
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
