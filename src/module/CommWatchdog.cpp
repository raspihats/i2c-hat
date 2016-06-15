/*
 * CommWatchdog.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */
#include "Utils.h"
#include "CommWatchdog.h"
extern "C" {
#include "EepromData.h"
}

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (5)

#define CWDT_DISABLED_PERIOD_MS         (0)
#define CWDT_DEFAULT_PERIOD_MS          (CWDT_DISABLED_PERIOD_MS)

namespace i2c_hat {

/**
  * @brief  Builds CommWatchdog Module object
  * @retval None
  */
CommWatchdog::CommWatchdog() :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS) {
    _period = EepromData_readUInt32(EEP_VIRT_ADR_CWDT_PERIOD, CWDT_DEFAULT_PERIOD_MS);
    _periodCnt = 0;
    _monitoring = false;
}

/**
  * @brief  Sets communication watchdog period
  * @param  period - communication watchdog period in milliseconds
  * @retval None
  */
void CommWatchdog::setCwdtPeriod(const uint32_t period) {
    EepromData_writeUInt32(EEP_VIRT_ADR_CWDT_PERIOD, period);
    _period = period;
}

/**
  * @brief  Gets communication watchdog period
  * @param  None
  * @retval communication watchdog period in milliseconds
  */
uint32_t CommWatchdog::getCwdtPeriod() {
    return _period;
}

/**
  * @brief  Feeds, clears, kicks the communication watchdog
  * @param  None
  * @retval None
  */
void CommWatchdog::feed() {
    _periodCnt = 0;
}

/**
  * @brief  CommWatchdog Module cooperative task implementation
  * @param  None
  * @retval None
  */
void CommWatchdog::task() {
    if(_monitoring) {
        _periodCnt += TASK_PERIOD_MS;
        if(_periodCnt >= _period) {
            sendEvent(MODULE_EVENT_CWDT_TIMEOUT);
            _periodCnt = 0;
            _monitoring = false;
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
uint32_t CommWatchdog::processRequest(I2CFrame *request, I2CFrame *response) {
    uint32_t tempU32;
    uint32_t responseFlag = 0;

    feed();

    switch(request->getCmd()) {
    case CMD_CWDT_SET_PERIOD:
        if(request->getLength() == 4) {
            Utils::convertBytesToUint32(request->getData(), &tempU32);
            setCwdtPeriod(tempU32);
            tempU32 = getCwdtPeriod();
            Utils::convertUint32ToBytes(tempU32, response->getData());
            response->setLength(4);
            responseFlag = 1;
          }
        break;
    case CMD_CWDT_GET_PERIOD:
        if(request->getLength() == 0) {
            tempU32 = getCwdtPeriod();
            Utils::convertUint32ToBytes(tempU32, response->getData());
            response->setLength(4);
            responseFlag = 1;
        }
        break;
    default:
        responseFlag = 0;
    }

    if((_period != CWDT_DISABLED_PERIOD_MS) && !_monitoring) {
        _monitoring = true;
        sendEvent(MODULE_EVENT_CWDT_MONITORING);
    }

    return responseFlag;
}

} /* namespace i2c_hat */

