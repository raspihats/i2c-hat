/*
 * DigitalOutputs.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "DigitalOutputs.h"
#include "Utils.h"
extern "C" {
#include "EepromData.h"
}

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (10)

#define DEFAULT_POWER_ON_VALUE          (0)
#define DEFAULT_SAFETY_VALUE            (0)

namespace i2c_hat {

/**
  * @brief  Builds a DigitalOutputs Module object
  * @param  port: pointer to a DigitalOutputPort
  * @retval None
  */
DigitalOutputs::DigitalOutputs(DigitalOutputPort* port) :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS) {
    _port = port;
    _powerOnValue = EepromData_readUInt32(EEP_VIRT_ADR_DO_POWER_ON_VALUE, DEFAULT_POWER_ON_VALUE);
    _safetyValue = EepromData_readUInt32(EEP_VIRT_ADR_DO_SAFETY_VALUE, DEFAULT_SAFETY_VALUE);
    loadPowerOnValue();
}

/**
  * @brief  Checks if value is valid to be written to port
  * @param  value: value to be checked
  * @retval None
  */
bool DigitalOutputs::validValue(const uint32_t value) {
    return value < ((uint32_t)0x01 << _port->getSize());
}

/**
  * @brief  Loads PowerOn Value
  * @param  None
  * @retval None
  */
void DigitalOutputs::loadPowerOnValue() {
    setAllChannels(_powerOnValue);
}

/**
  * @brief  Sets the PowerOn Value
  * @param  powerOnValue: the PowerOn Value
  * @retval None
  */
void DigitalOutputs::setPowerOnValue(const uint32_t powerOnValue) {
    _powerOnValue = powerOnValue;
    EepromData_writeUInt32(EEP_VIRT_ADR_DO_POWER_ON_VALUE, _powerOnValue);
}

/**
  * @brief  Loads Safety Value
  * @param  None
  * @retval None
  */
void DigitalOutputs::loadSafetyValue() {
    setAllChannels(_safetyValue);
}

/**
  * @brief  Sets the Safety Value
  * @param  safetyValue: the Safety Value
  * @retval None
  */
void DigitalOutputs::setSafetyValue(const uint32_t safetyValue) {
    _safetyValue = safetyValue;
    EepromData_writeUInt32(EEP_VIRT_ADR_DO_SAFETY_VALUE, _safetyValue);
}

/**
  * @brief  Sets all channel states
  * @param  states:
  * @retval None
  */
void DigitalOutputs::setAllChannels(const uint32_t states) {
    _port->write(states);
}

/**
  * @brief  Sets state of a single channel
  * @param  channel: channel index
  * @param  state: state
  * @retval error code
  */
do_errcode_t DigitalOutputs::setSingleChannel(const uint8_t channel, const uint8_t state) {
    do_errcode_t errCode;

    if(channel < _port->getSize()) {
        _port->writeBit(channel, state > 0);
        errCode = DO_ERRCODE_SUCCESS;
    }
    else {
        errCode = DO_ERRCODE_CHANNEL_OUT_OF_RANGE;
    }
    return errCode;
}

/**
  * @brief  Gets state of a single channel
  * @param  channel: channel index
  * @param  state: pointer where to store state
  * @retval error code
  */
do_errcode_t DigitalOutputs::getSingleChannel(const uint8_t channel, uint8_t* const state) {
    do_errcode_t errCode;

    if(channel < _port->getSize()) {
        *state = _port->readBit(channel) ?  0x01 : 0x00;
        errCode = DO_ERRCODE_SUCCESS;
    }
    else {
        errCode = DO_ERRCODE_CHANNEL_OUT_OF_RANGE;
    }
    return errCode;
}

/**
  * @brief  Gets all channel states
  * @param  None
  * @retval all channel states
  */
uint32_t DigitalOutputs::getAllChannels() {
    return _port->read();
}

/**
  * @brief  DigitalOutputs Module receiveEvent implementation
  * @param  event: event code
  * @retval None
  */
void DigitalOutputs::receiveEvent(const uint32_t event) {
    if(event == EVENT_CWDT_TIMEOUT) {
        loadSafetyValue();
    }
}

/**
  * @brief  Processes I2C request frame
  * @param  request: pointer to request frame
  * @param  response: pointer to response frame
  * @retval 1 if response
  *         0 if no response
  */
uint32_t DigitalOutputs::processRequest(I2CFrame *request, I2CFrame *response) {
    uint32_t tempU32;
    uint8_t channel;
    uint8_t value;
    uint32_t responseFlag = 0;

    switch(request->getCmd()) {
    case CMD_DO_SET_POWER_ON_VALUE:
        if(request->getLength() == 4) {
            Utils::convertBytesToUint32(request->getData(), &tempU32);
            if(validValue(tempU32)) {
                _powerOnValue = tempU32;
                EepromData_writeUInt32(EEP_VIRT_ADR_DO_POWER_ON_VALUE, _powerOnValue);
                Utils::convertUint32ToBytes(_powerOnValue, response->getData());
                response->setLength(4);
                responseFlag = 1;
            }
        }
        break;
    case CMD_DO_GET_POWER_ON_VALUE:
        if(request->getLength() == 0) {
            Utils::convertUint32ToBytes(_powerOnValue, response->getData());
            response->setLength(4);
            responseFlag = 1;
        }
        break;
    case CMD_DO_SET_SAFETY_VALUE:
        if(request->getLength() == 4) {
            Utils::convertBytesToUint32(request->getData(), &tempU32);
            if(validValue(tempU32)) {
                _safetyValue = tempU32;
                EepromData_writeUInt32(EEP_VIRT_ADR_DO_SAFETY_VALUE, _safetyValue);
                Utils::convertUint32ToBytes(_safetyValue, response->getData());
                response->setLength(4);
                responseFlag = 1;
            }
        }
        break;
    case CMD_DO_GET_SAFETY_VALUE:
        if(request->getLength() == 0) {
            Utils::convertUint32ToBytes(_safetyValue, response->getData());
            response->setLength(4);
            responseFlag = 1;
        }
        break;
    case CMD_DO_SET_ALL_CHANNEL_STATES:
        if(request->getLength() == 4) {
            Utils::convertBytesToUint32(request->getData(), &tempU32);
            if(validValue(tempU32)) {
                setAllChannels(tempU32);
                tempU32 = getAllChannels();
                Utils::convertUint32ToBytes(tempU32, response->getData());
                response->setLength(4);
                responseFlag = 1;
            }
        }
        break;
    case CMD_DO_GET_ALL_CHANNEL_STATES:
        if(request->getLength() == 0) {
            tempU32 = getAllChannels();
            Utils::convertUint32ToBytes(tempU32, response->getData());
            response->setLength(4);
            responseFlag = 1;
        }
        break;
    case CMD_DO_SET_CHANNEL_STATE:
        if(request->getLength() == 2) {
            channel = request->getData()[0];
            value = request->getData()[1];
            if(setSingleChannel(channel, value) == DO_ERRCODE_SUCCESS) {
                response->getData()[0] = channel;
                response->getData()[1] = value;
                response->setLength(2);
                responseFlag = 1;
            }
        }
        break;
    case CMD_DO_GET_CHANNEL_STATE:
        if(request->getLength() == 1) {
            channel = request->getData()[0];
            if(getSingleChannel(channel, &value) == DO_ERRCODE_SUCCESS) {
                response->getData()[0] = channel;
                response->getData()[1] = value;
                response->setLength(2);
                responseFlag = 1;
            }
        }
        break;
    default:
        responseFlag = 0;
    }
    return responseFlag;
}

} /* namespace i2c_hat */
