/*
 * StatusLed.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "StatusLed.h"

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (5)

#define STATUS_LED_ON_TIME_MS           (75)
#define STATUS_LED_OFF_SHORT_TIME_MS    (75)
#define STATUS_LED_OFF_LONG_TIME_MS     (3000)

typedef enum {
    ST_LED_INIT,
    ST_LED_ON,
    ST_LED_OFF_SHORT,
    ST_LED_OFF_LONG,
} state_t;

namespace i2c_hat {

/**
  * @brief  Builds a SattusLed Module object
  * @param  pin: pointer to a DigitalOutputPin
  * @retval None
  */
StatusLed::StatusLed(DigitalOutputPin* const pin) :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS) {
    _pin = pin;
    _communication = false;
}

/**
  * @brief  StatusLed Module receiveEvent implementation
  * @param  event: event code
  * @retval None
  */
void StatusLed::receiveEvent(const uint32_t event) {
    if(event == MODULE_EVENT_CWDT_MONITORING) {
        _communication = true;
    }
    else if(event == MODULE_EVENT_CWDT_TIMEOUT) {
        _communication = false;
    }
}

/**
  * @brief  StatusLed Module cooperative task implementation
  * @param  None
  * @retval None
  */
void StatusLed::task() {
    static uint32_t ledOnPeriodCnt = 0;
    static uint32_t ledOffPeriodCnt = 0;
    static uint32_t pulseCnt = 0;
    static bool communication = false;
    static state_t state = ST_LED_INIT;

    switch(state) {
    case ST_LED_INIT:
        _pin->write(true);
        ledOnPeriodCnt = 0;
        state = ST_LED_ON;
        break;
    case ST_LED_ON:
        if(ledOnPeriodCnt < STATUS_LED_ON_TIME_MS) {
            ledOnPeriodCnt += TASK_PERIOD_MS;
        }
        else {
            _pin->write(false);
            ledOffPeriodCnt = 0;
            if(communication && (pulseCnt == 0)) {
                state = ST_LED_OFF_SHORT;
            }
            else {
                state = ST_LED_OFF_LONG;
            }
        }
        break;
    case ST_LED_OFF_SHORT:
        if(ledOffPeriodCnt < STATUS_LED_OFF_SHORT_TIME_MS) {
            ledOffPeriodCnt += TASK_PERIOD_MS;
        }
        else {
            _pin->write(true);
            ledOnPeriodCnt = 0;
            state = ST_LED_ON;
            pulseCnt++;
        }
        break;
    case ST_LED_OFF_LONG:
        if(ledOffPeriodCnt < STATUS_LED_OFF_LONG_TIME_MS) {
            ledOffPeriodCnt += TASK_PERIOD_MS;
        }
        else {
            _pin->write(true);
            ledOnPeriodCnt = 0;
            state = ST_LED_ON;
            pulseCnt = 0;
            communication = _communication;
        }
        break;
    default:
        ledOnPeriodCnt = 0;
        state = ST_LED_ON;
        break;
    }
}

/**
  * @brief  Processes I2C request frame
  * @param  request: pointer to request frame
  * @param  response: pointer to response frame
  * @retval 1 if response
  *         0 if no response
  */
uint32_t StatusLed::processRequest(I2CFrame *request, I2CFrame *response) {
    (void)request;
    (void)response;
//    _communication = true;
    return 0;
}

} /* namespace i2c_hat */
