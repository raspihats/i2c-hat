/*
 * digital_inputs.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "digital_inputs.h"

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

}

/**
  * @brief  Checks if value is valid to be written to register
  * @param  value: value to be checked
  * @retval value validation status
  */
bool DigitalInputs::IsValid(const uint32_t value) {
    return value < ((uint32_t)0x01 << kChannelCount);
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

    return value;
}

/**
  * @brief  Gets specific channel edge counter
  * @param  channel: channel index
  * @param  type: counter type
  * @param  pointer where to store counter value
  * @retval
  */
bool DigitalInputs::GetCounter(const uint32_t channel, const CounterTypes type, uint32_t& value) {
    if(channel < kChannelCount) {
        if(type == CounterTypes::FALLING_EDGE) {
            value = channels_[channel].falling_edge_counter();
            return true;
        }
        if(type == CounterTypes::RISING_EDGE) {
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
bool DigitalInputs::ResetCounter(const uint32_t index, const CounterTypes type) {
    if(index < kChannelCount) {
        if(type == CounterTypes::RISING_EDGE) {
            channels_[index].ResetRisingEdgeCounter();
            return true;
        }
        if(type == CounterTypes::FALLING_EDGE) {
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
  * @brief  Triggers IRQ pin, pulls down IRQ pin
  * @param  None
  * @retval None
  */
inline void DigitalInputs::TriggerIRQ() {
    irq_.SetState(false);
}

/**
  * @brief  Releases IRQ pin
  * @param  None
  * @retval None
  */
inline void DigitalInputs::ReleaseIRQ() {
    irq_.SetState(true);
}

/**
  * @brief  Gets IRQ register
  * @param  reg - Register
  * @retval Register value
  */
uint32_t DigitalInputs::GetIRQReg(const IRQReg reg) {
    uint32_t value;
    uint32_t i;
    bool flag;

    switch(reg) {
    case IRQReg::DI_RISING_EDGE_CONTROL:
        value = 0;
        for(i = 0; i < kChannelCount; i++) {
            flag = channels_[i].rising_edge_irq_enable_flag();
            if(flag) {
                value |= 0x01 << i;
            }
        }
        break;
    case IRQReg::DI_FALLING_EDGE_CONTROL:
        value = 0;
        for(i = 0; i < kChannelCount; i++) {
            flag = channels_[i].falling_edge_irq_enable_flag();
            if(flag) {
                value |= 0x01 << i;
            }
        }
        break;
    case IRQReg::DI_CAPTURE:
        flag = irq_capture_queue_.Get(value);
        if(not flag) {
            value = 0;  // This means an empty irq_queue_
        }
        break;
    default:
        value = 0;
        break;
    }

    return value;
}

/**
  * @brief  Sets IRQ register
  * @param  reg - Register
  * @retval None
  */
bool DigitalInputs::SetIRQReg(const IRQReg reg, const uint32_t value) {
    uint32_t i;
    bool result;

    result = false;
    if(IsValid(value)) {
        switch(reg) {
        case IRQReg::DI_RISING_EDGE_CONTROL:
            for(i = 0; i < kChannelCount; i++) {
                channels_[i].set_rising_edge_irq_enable_flag((value >> i) & 0x01);
            }
            result = true;
            break;
        case IRQReg::DI_FALLING_EDGE_CONTROL:
            for(i = 0; i < kChannelCount; i++) {
                channels_[i].set_falling_edge_irq_enable_flag((value >> i) & 0x01);
            }
            result = true;
            break;
        case IRQReg::DI_CAPTURE:
            if(value == 0) {
                irq_capture_queue_.Clear();
                result = true;
            }
            break;
        default:
            result = false;
        }
    }
    return result;
}

bool DigitalInputs::IsIRQCaptureQueueFull() {
    return irq_capture_queue_.IsFull();
}

/**
  * @brief  Module Init implementation
  * @param  None
  * @retval None
  */
void DigitalInputs::Init() {
    uint32_t i;

    for(i = 0; i < kChannelCount; i++) {
        channels_[i].Init(DEBOUNCE_TIME_MS / TASK_PERIOD_MS);
    }
}

/**
  * @brief  Module cooperative Task implementation
  * @param  None
  * @retval None
  */
void DigitalInputs::Run() {
    uint32_t i;
    uint32_t irq_status;
    uint32_t value;
    uint32_t dump;

    // IRQ queue
    irq_status = 0;
    value = 0;
    for(i = 0; i < kChannelCount; i++) {
        channels_[i].Tick();
        irq_status |= channels_[i].irq_flag() << i;         // read and clear irq state
        value |= channels_[i].state() << i;
    }

    if(irq_status > 0) {
        if(irq_capture_queue_.IsFull()) {
            irq_capture_queue_.Get(dump);   // dump one value because queue was not read in time and must store new value
        }
        irq_capture_queue_.Put((value << 16) + irq_status);
        TriggerIRQ();
    }
    else {
        if(irq_capture_queue_.IsEmpty()) {
            ReleaseIRQ();
        }
    }

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
    uint8_t *data;
    uint8_t index;
    uint8_t u8_temp;
    uint32_t u32_temp;
    IRQReg irq_reg;

    switch((Command)request.command()) {
    case Command::DI_GET_VALUE:
        if(request.payload_size() == 0) {
            u32_temp = GetValue();
            buffer[0] = (uint8_t)u32_temp;
            buffer[1] = (uint8_t)(u32_temp >> 8);
            buffer[2] = (uint8_t)(u32_temp >> 16);
            buffer[3] = (uint8_t)(u32_temp >> 24);
            response.set_payload(buffer, 4);
            response_flag = true;
            ReleaseIRQ();
        }
        break;
    case Command::DI_GET_CHANNEL_STATE:
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
    case Command::DI_GET_COUNTER:
        if(request.payload_size() == 2) {
            index = request.payload()[0];
            u8_temp = request.payload()[1];
            if(GetCounter(index, (CounterTypes)u8_temp, u32_temp)) {
                buffer[0] = index;
                buffer[1] = u8_temp;
                buffer[2] = (uint8_t)u32_temp;
                buffer[3] = (uint8_t)(u32_temp >> 8);
                buffer[4] = (uint8_t)(u32_temp >> 16);
                buffer[5] = (uint8_t)(u32_temp >> 24);
                response.set_payload(buffer, 6);
                response_flag = true;
            }
        }
        break;
    case Command::DI_RESET_COUNTER:
        if(request.payload_size() == 2) {
            index = request.payload()[0];
            u8_temp = request.payload()[1];
            if(ResetCounter(index, (CounterTypes)u8_temp)) {
                buffer[0] = index;
                buffer[1] = u8_temp;
                response.set_payload(buffer, 2);
                response_flag = true;
            }
        }
        break;
    case Command::DI_RESET_ALL_COUNTERS:
        if(request.payload_size() == 0) {
            ResetCounters();
            response.set_payload(NULL, 0);
            response_flag = true;
        }
        break;
    case Command::DI_GET_COUNTERS_STATUS:
        // TODO implement
        response_flag = false;
        break;
    case Command::DI_GET_ENCODER:
        if(request.payload_size() == 1) {
            index = request.payload()[0];
//            if(GetEncoder(channel, &tempU32)) {
//                buffer[0] = index;
//                response->setLength(5);
//                response_flag = true;
//            }
        }
        break;
    case Command::DI_RESET_ENCODER:
        // TODO implement
        response_flag = false;
        break;
    case Command::DI_RESET_ALL_ENCODERS:
        // TODO implement
        response_flag = false;
        break;
    case Command::DI_GET_ENCODERS_STATUS:
        // TODO implement
        response_flag = false;
        break;
    // TODO move IRQ GET/SET to separate module
    case Command::IRQ_GET_REG:
        if(request.payload_size() == 1) {
            irq_reg = (IRQReg)request.payload()[0];
            if( (irq_reg == IRQReg::DI_FALLING_EDGE_CONTROL) or
                    (irq_reg == IRQReg::DI_RISING_EDGE_CONTROL) or
                    (irq_reg == IRQReg::DI_CAPTURE) ) {
                u32_temp = GetIRQReg(irq_reg);
                buffer[0] = static_cast<int>(irq_reg);
                buffer[1] = (uint8_t)u32_temp;
                buffer[2] = (uint8_t)(u32_temp >> 8);
                buffer[3] = (uint8_t)(u32_temp >> 16);
                buffer[4] = (uint8_t)(u32_temp >> 24);
                response.set_payload(buffer, 5);
                response_flag = true;
            }
        }
        break;
    case Command::IRQ_SET_REG:
        if(request.payload_size() == 5) {
            data = (uint8_t*)request.payload();
            irq_reg = (IRQReg)data[0];
            BYTES_TO_UINT32(data + 1, u32_temp);
            if( (irq_reg == IRQReg::DI_FALLING_EDGE_CONTROL) or
                    (irq_reg == IRQReg::DI_RISING_EDGE_CONTROL) or
                    (irq_reg == IRQReg::DI_CAPTURE) ) {
                if(SetIRQReg(irq_reg, u32_temp)) {
                    u32_temp = GetIRQReg(irq_reg);
                    buffer[0] = static_cast<int>(irq_reg);
                    buffer[1] = (uint8_t)u32_temp;
                    buffer[2] = (uint8_t)(u32_temp >> 8);
                    buffer[3] = (uint8_t)(u32_temp >> 16);
                    buffer[4] = (uint8_t)(u32_temp >> 24);
                    response.set_payload(buffer, 5);
                    response_flag = true;
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
