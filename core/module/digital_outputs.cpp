/*
 * digital_outputs.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */
#include "digital_outputs.h"

#ifdef DIGITAL_OUTPUT_CHANNEL_COUNT

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (10)

#define DEFAULT_POWER_ON_VALUE          (0)
#define DEFAULT_SAFETY_VALUE            (0)
// CiA 401 defaults: no inversion; every channel applies the safety value on a
// CWDT trip (mode FFh) - which is also the pre-mask firmware behavior, so
// boards upgraded in the field behave identically until commissioned otherwise
#define DEFAULT_POLARITY                (0)
#define DEFAULT_SAFETY_MASK             (((uint32_t)0x01 << DIGITAL_OUTPUT_CHANNEL_COUNT) - 1)

#define RELAY_PULL_MS                   (20)
#define RELAY_HOLD_DUTY_CYCLE           (40)

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
        safety_value_(DEFAULT_SAFETY_VALUE),
        polarity_(DEFAULT_POLARITY),
        safety_mask_(DEFAULT_SAFETY_MASK) {

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
        if(driver::Eeprom::Write(EEP_VIRT_ADR_DO_POWER_ON_VALUE, value)) {
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
  * @brief  Loads Safety Value on a CWDT trip
  * @param  None
  * @retval None
  *
  * CiA 401 0x6206 semantics: channels whose safety-mask bit is set take the
  * safety value; the rest hold their last state.
  */
void DigitalOutputs::LoadSafetyValue() {
    SetValue((GetValue() & ~safety_mask_) | (safety_value_ & safety_mask_));
}

/**
  * @brief  Sets the Safety Value
  * @param  safetyValue: the Safety Value
  * @retval None
  */
bool DigitalOutputs::SetSafetyValue(const uint32_t value) {
    if(IsValid(value)) {
        if(driver::Eeprom::Write(EEP_VIRT_ADR_DO_SAFETY_VALUE, value)) {
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
  * @param  value: logical channel states
  * @retval None
  *
  * CiA 401 0x6202: the wire carries logical state everywhere; polarity
  * inverts between logical value and pin. Applying it here (and in the
  * single-channel accessors) covers every source - process writes, the
  * safety value on a trip, the power-on value at boot.
  */
bool DigitalOutputs::SetValue(const uint32_t value) {
    uint32_t i, mask, pin_value;

    if(IsValid(value)) {
        pin_value = value ^ polarity_;
        for(i = 0; i < kChannelCount; i++) {
            mask = 0x01 << i;
            channels_[i].SetState((pin_value & mask) > 0);
        }
        return true;
    }
    return false;
}

/**
  * @brief  Sets state of a single channel
  * @param  index: channel index
  * @param  state: channel state
  * @retval error code
  */
bool DigitalOutputs::SetChannelState(const uint8_t index, const bool state) {

    if(index < kChannelCount) {
        channels_[index].SetState(state != (bool)((polarity_ >> index) & 0x01));
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
        state = channels_[index].GetState() != (bool)((polarity_ >> index) & 0x01);
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
        value |= channels_[i].GetState() ? 0x01 << i : 0x00 ;
    }
    return value ^ polarity_;   // pins back to logical (CiA 401 0x6202)
}

/**
  * @brief  Sets the output polarity (CiA 401 0x6202), persistent
  * @param  value: per-bit invert mask, applied between logical value and pin
  * @retval true on success
  *
  * Commissioning hazard, by design: the physical pins whose polarity bits
  * change flip immediately, while the LOGICAL value is preserved - the pins
  * are re-driven so wire-visible state stays the same. An engineering act;
  * controllers reconcile it at activation, not during RUN.
  */
bool DigitalOutputs::SetPolarity(const uint32_t value) {
    uint32_t logical;

    if(IsValid(value)) {
        if(driver::Eeprom::Write(EEP_VIRT_ADR_DO_POLARITY, value)) {
            logical = GetValue();
            polarity_ = value;
            SetValue(logical);
            return true;
        }
        else {
            // TODO Error handler
        }
    }
    return false;
}

/**
  * @brief  Sets the safety mask (CiA 401 0x6206), persistent
  * @param  value: per-bit: 1 = load the safety value on a CWDT trip,
  *                0 = hold last state
  * @retval true on success
  */
bool DigitalOutputs::SetSafetyMask(const uint32_t value) {
    if(IsValid(value)) {
        if(driver::Eeprom::Write(EEP_VIRT_ADR_DO_SAFETY_MASK, value)) {
            safety_mask_ = value;
            return true;
        }
        else {
            // TODO Error handler
        }
    }
    return false;
}

/**
  * @brief  Module Init implementation
  * @param  None
  * @retval None
  */
void DigitalOutputs::Init() {
    bool success;
    uint32_t i;

    success = driver::Eeprom::Read(EEP_VIRT_ADR_DO_POWER_ON_VALUE, power_on_value_);
    if(not success) {
        // TODO Error Handling
    }

    success = driver::Eeprom::Read(EEP_VIRT_ADR_DO_SAFETY_VALUE, safety_value_);
    if(not success) {
        // TODO Error Handling
    }

    // polarity must be known before LoadPowerOnValue() below so the power-on
    // value reaches the pins through it; on boards upgraded from firmware
    // without these registers the reads fail and the defaults stand
    // (polarity 0, mask all channels - the pre-mask behavior)
    if(not driver::Eeprom::Read(EEP_VIRT_ADR_DO_POLARITY, polarity_)) {
        polarity_ = DEFAULT_POLARITY;
    }
    if(not driver::Eeprom::Read(EEP_VIRT_ADR_DO_SAFETY_MASK, safety_mask_)) {
        safety_mask_ = DEFAULT_SAFETY_MASK;
    }

    for(i = 0; i < kChannelCount; i++) {
        channels_[i].Init(RELAY_PULL_MS / TASK_PERIOD_MS, RELAY_HOLD_DUTY_CYCLE);
    }

    LoadPowerOnValue();
}

/**
  * @brief  Module cooperative Task implementation
  * @param  None
  * @retval None
  */
void DigitalOutputs::Run() {
    uint32_t i;

    for(i = 0; i < kChannelCount; i++) {
        channels_[i].Tick();
    }
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
    static uint32_t temp;
    uint8_t index;
    bool state;
    bool response_flag;

    response_flag = false;
    switch((Command)request.command()) {
    case Command::DO_SET_POWER_ON_VALUE:
        if(request.payload_size() == 4) {
            BYTES_TO_UINT32(request.payload(), temp);
            if(SetPowerOnValue(temp)) {
                response.set_payload((uint8_t*)&power_on_value_, 4);
                response_flag = true;
            }
        }
        break;
    case Command::DO_GET_POWER_ON_VALUE:
        if(request.payload_size() == 0) {
            response.set_payload((uint8_t*)&power_on_value_, 4);
            response_flag = true;
        }
        break;
    case Command::DO_SET_SAFETY_VALUE:
        if(request.payload_size() == 4) {
            BYTES_TO_UINT32(request.payload(), temp);
            if(SetSafetyValue(temp)) {
                response.set_payload((uint8_t*)&safety_value_, 4);
                response_flag = true;
            }
        }
        break;
    case Command::DO_GET_SAFETY_VALUE:
        if(request.payload_size() == 0) {
            response.set_payload((uint8_t*)&safety_value_, 4);
            response_flag = true;
        }
        break;
    case Command::DO_SET_POLARITY:
        if(request.payload_size() == 4) {
            BYTES_TO_UINT32(request.payload(), temp);
            if(SetPolarity(temp)) {
                response.set_payload((uint8_t*)&polarity_, 4);
                response_flag = true;
            }
        }
        break;
    case Command::DO_GET_POLARITY:
        if(request.payload_size() == 0) {
            response.set_payload((uint8_t*)&polarity_, 4);
            response_flag = true;
        }
        break;
    case Command::DO_SET_SAFETY_MASK:
        if(request.payload_size() == 4) {
            BYTES_TO_UINT32(request.payload(), temp);
            if(SetSafetyMask(temp)) {
                response.set_payload((uint8_t*)&safety_mask_, 4);
                response_flag = true;
            }
        }
        break;
    case Command::DO_GET_SAFETY_MASK:
        if(request.payload_size() == 0) {
            response.set_payload((uint8_t*)&safety_mask_, 4);
            response_flag = true;
        }
        break;
    case Command::DO_SET_VALUE:
        if(request.payload_size() == 4) {
            BYTES_TO_UINT32(request.payload(), temp);
            if(SetValue(temp)) {
                temp = GetValue();
                response.set_payload((uint8_t*)&temp, 4);
                response_flag = true;
            }
        }
        break;
    case Command::DO_GET_VALUE:
        if(request.payload_size() == 0) {
            temp = GetValue();
            response.set_payload((uint8_t*)&temp, 4);
            response_flag = true;
        }
        break;
    case Command::DO_SET_CHANNEL_STATE:
        if(request.payload_size() == 2) {
            index = request.payload()[0];
            state = request.payload()[1];
            if(SetChannelState(index, state)) {
                uint8_t *data = (uint8_t*)&temp;
                data[0] = index;
                data[1] = state;
                response.set_payload(request.payload(), 2);
                response_flag = true;
            }
        }
        break;
    case Command::DO_GET_CHANNEL_STATE:
        if(request.payload_size() == 1) {
            index = request.payload()[0];
            if(GetChannelState(index, state)) {
                uint8_t *data = (uint8_t*)&temp;
                data[0] = index;
                data[1] = state;
                response.set_payload(data, 2);
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

#endif /* DIGITAL_OUTPUT_CHANNEL_COUNT */
