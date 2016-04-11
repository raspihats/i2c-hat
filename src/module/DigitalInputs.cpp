/*
 * DigitalInputs.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "DigitalInputs.h"
#include "Utils.h"

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (1)

#define DEBOUNCE_TIME_MS                (4)

namespace i2c_hat {

/**
  * @brief  Builds a DigitalInputs Module object
  * @param  dinPort: pointer to a DigitalInputPort
  * @retval None
  */
DigitalInputs::DigitalInputs(DigitalInputPort *port) :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS) {
    uint32_t channelCount;
    uint32_t channel;

    _port = port;
    channelCount = _port->getSize();
    _risingEdgeCounters = new uint32_t[channelCount];
    _fallingEdgeCounters = new uint32_t[channelCount];
    _integrators = new uint32_t[channelCount];
    for(channel = 0; channel < channelCount; channel++) {
        _risingEdgeCounters[channel] = 0;
        _integrators[channel] = 0;
        _fallingEdgeCounters[channel] = 0;
    }
    _filteredInputs = port->read();
    _integratorUpperLimit = DEBOUNCE_TIME_MS * TASK_PERIOD_MS;
}

/**
  * @brief  Gets specific digital input channel state
  * @param  channel: channel index
  * @param  state: pointer where to store digital input channel state
  * @retval error code
  */
di_errcode_t DigitalInputs::getSingleChannel(const uint32_t channel, uint8_t* const state) {
    di_errcode_t errCode;

    if(channel < _port->getSize()) {
        *state = (uint8_t)((_filteredInputs >> channel) & 0x01);
        errCode = DI_ERRCODE_SUCCESS;
    }
    else {
        errCode = DI_ERRCODE_CHANNEL_OUT_OF_RANGE;
    }
    return errCode;
}

/**
  * @brief  Gets all digital input channels states
  * @param  None
  * @retval digital input channel states
  */
uint32_t DigitalInputs::getAllChannels() {
    return _filteredInputs;
}

/**
  * @brief  Gets specific channel edge counter
  * @param  channel: channel index
  * @param  type: counter type
  * @param  pointer where to store counter value
  * @retval error code
  */
di_errcode_t DigitalInputs::getSingleCounter(const uint32_t channel, const di_counter_type_t type, uint32_t* const value) {
    di_errcode_t errCode;

    if(channel < _port->getSize()) {
        switch(type) {
        case DIGITAL_INPUT_COUNTER_TYPE_FALLING_EDGE:
            *value = _fallingEdgeCounters[channel];
            errCode = DI_ERRCODE_SUCCESS;
            break;
        case DIGITAL_INPUT_COUNTER_TYPE_RISING_EDGE:
            *value = _risingEdgeCounters[channel];
            errCode = DI_ERRCODE_SUCCESS;
            break;
        default:
            errCode = DI_ERRCODE_COUNTER_TYPE_OUT_OF_RANGE;
        }
    }
    else {
        errCode = DI_ERRCODE_CHANNEL_OUT_OF_RANGE;
    }
    return errCode;
}

/**
  * @brief  Resets specific channel edge counter
  * @param  channel: channel index
  * @param  type: counter type
  * @retval error code
  */
di_errcode_t DigitalInputs::resetSingleCounter(const uint32_t channel, const di_counter_type_t type) {
    di_errcode_t errCode;

    if(channel < _port->getSize()) {
        if(type == DIGITAL_INPUT_COUNTER_TYPE_FALLING_EDGE) {
            _fallingEdgeCounters[channel] = 0;
        }
        else {
            _risingEdgeCounters[channel] = 0;
        }
        errCode = DI_ERRCODE_SUCCESS;
    }
    else {
        errCode = DI_ERRCODE_CHANNEL_OUT_OF_RANGE;
    }
    return errCode;
}

/**
  * @brief  Resets all channels edge counters
  * @param  None
  * @retval None
  */
void DigitalInputs::resetAllCounters() {
    uint32_t channel;
    uint32_t channelCount;

    channelCount = _port->getSize();
    for(channel = 0; channel < channelCount; channel++) {
        _fallingEdgeCounters[channel] = 0;
        _risingEdgeCounters[channel] = 0;
    }
}

/**
  * @brief  DigitalInputs cooperative task implementation
  * @param  None
  * @retval None
  */
void DigitalInputs::task() {
    uint32_t channel;
    uint32_t channelCount;

    channelCount = _port->getSize();
    for(channel = 0; channel < channelCount; channel++) {
        /* Step 1:
         * Update the integrator based on the input signal. Note that the integrator
         * follows the input, decreasing or increasing towards the limits as
         * determined by the input state (0 or 1). */
        if(_port->readBit(channel)) {
            if (_integrators[channel] < _integratorUpperLimit) {
                _integrators[channel]++;
            }
        }
        else {
            if (_integrators[channel] > 0) {
                _integrators[channel]--;
            }
        }

        /* Step 2:
         * Update the output state based on the integrator. Note that the output will
         * only change states if the integrator has reached a limit, either 0 or MAXIMUM. */
        if (_integrators[channel] >= _integratorUpperLimit) {
            if(((_filteredInputs >> channel) & 0x01) == 0x00) {
                _filteredInputs |= (0x01 << channel);   //state of debounced input is 1
                _risingEdgeCounters[channel]++;
            }
        }
        else if (_integrators[channel] == 0) {
            if(((_filteredInputs >> channel) & 0x01) == 0x01) {
                _filteredInputs &= ~(0x01 << channel);  //state of debounced input is 0
                _fallingEdgeCounters[channel]++;
            }
        }
    }
}

/**
  * @brief  Processes I2C request frame
  * @param  request: pointer to request frame
  * @param  response: pointer to response frame
  * @retval 1 if response
  *         0 if no response
  */
uint32_t DigitalInputs::processRequest(I2CFrame *request, I2CFrame *response) {
    uint32_t responseFlag = 0;
    uint32_t tempU32;
    uint8_t tempU8;
    uint8_t channel;
    uint8_t counterType;
//    uint8_t *data;

    switch(request->getCmd()) {
    case CMD_DI_GET_ALL_CHANNEL_STATES:
        if(request->getLength() == 0) {
            tempU32 = getAllChannels();
            Utils::convertUint32ToBytes(tempU32, response->getData());
            response->setLength(4);
            responseFlag = 1;
        }
        break;
    case CMD_DI_GET_CHANNEL_STATE:
        if(request->getLength() == 1) {
            channel = request->getData()[0];
            if(getSingleChannel(channel, &tempU8) == DI_ERRCODE_SUCCESS) {
                response->getData()[0] = channel;
                response->getData()[1] = tempU8;
                response->setLength(2);
                responseFlag = 1;
            }
        }
        break;
    case CMD_DI_GET_COUNTER:
        if(request->getLength() == 2) {
            channel = request->getData()[0];
            counterType = request->getData()[1];
            if(getSingleCounter(channel, (di_counter_type_t)counterType, &tempU32) == DI_ERRCODE_SUCCESS) {
                response->getData()[0] = channel;
                response->getData()[1] = counterType;
                Utils::convertUint32ToBytes(tempU32, &response->getData()[2]);
                response->setLength(6);
                responseFlag = 1;
            }
        }
        break;
    case CMD_DI_RESET_COUNTER:
        if(request->getLength() == 2) {
            channel = request->getData()[0];
            counterType = request->getData()[1];
            if(resetSingleCounter(channel, (di_counter_type_t)counterType) == DI_ERRCODE_SUCCESS) {
                response->getData()[0] = channel;
                response->getData()[1] = counterType;
                response->setLength(2);
                responseFlag = 1;
            }
        }
        break;
    case CMD_DI_RESET_ALL_COUNTERS:
        if(request->getLength() == 0) {
            resetAllCounters();
            response->setLength(0);
            responseFlag = 1;
        }
        break;
    default:
        responseFlag = 0;
    }
    return responseFlag;
}

DigitalInputs::~DigitalInputs() {
    delete(_risingEdgeCounters);
    delete(_fallingEdgeCounters);
    delete(_integrators);
}

} /* namespace i2c_hat */
