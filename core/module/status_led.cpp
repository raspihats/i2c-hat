/*
 * status_led.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "status_led.h"

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (5)

#define STATUS_LED_ON_TIME_MS           (75)
#define STATUS_LED_OFF_SHORT_TIME_MS    (75)
#define STATUS_LED_OFF_LONG_TIME_MS     (3000)

namespace i2c_hat {
namespace module {

/**
  * @brief  Builds a SattusLed Module object
  * @param  pin: pointer to a DigitalOutputPin
  * @retval None
  */
StatusLed::StatusLed() :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS),
        pin_(STATUS_LED_PIN),
        cwdt_monitoring_(false) {
}

/**
  * @brief  StatusLed Module receiveEvent implementation
  * @param  event: event code
  * @retval None
  */
void StatusLed::ReceiveEvent(const uint32_t event) {
    if(event == EVENT_CWDT_MONITORING) {
        cwdt_monitoring_ = true;
    }
    else if((event == EVENT_CWDT_DISABLED) or (event == EVENT_CWDT_TIMEOUT)) {
        cwdt_monitoring_ = false;
    }
}

/**
  * @brief  StatusLed Module cooperative Init implementation
  * @param  None
  * @retval None
  */
void StatusLed::Init() {
    pin_.SetState(false);
}

/**
  * @brief  StatusLed Module cooperative Task implementation
  * @param  None
  * @retval None
  */
void StatusLed::Run() {
    static uint32_t ledOnPeriodCnt = 0;
    static uint32_t ledOffPeriodCnt = 0;
    static uint32_t pulseCnt = 0;
    static bool communication = false;
    static State state = STATE_LED_INIT;

    switch(state) {
    case STATE_LED_INIT:
        pin_.SetState(true);
        ledOnPeriodCnt = 0;
        state = STATE_LED_ON;
        break;
    case STATE_LED_ON:
        if(ledOnPeriodCnt < STATUS_LED_ON_TIME_MS) {
            ledOnPeriodCnt += TASK_PERIOD_MS;
        }
        else {
            pin_.SetState(false);
            ledOffPeriodCnt = 0;
            if(communication && (pulseCnt == 0)) {
                state = STATE_LED_OFF_SHORT;
            }
            else {
                state = STATE_LED_OFF_LONG;
            }
        }
        break;
    case STATE_LED_OFF_SHORT:
        if(ledOffPeriodCnt < STATUS_LED_OFF_SHORT_TIME_MS) {
            ledOffPeriodCnt += TASK_PERIOD_MS;
        }
        else {
            pin_.SetState(true);
            ledOnPeriodCnt = 0;
            state = STATE_LED_ON;
            pulseCnt++;
        }
        break;
    case STATE_LED_OFF_LONG:
        if(ledOffPeriodCnt < STATUS_LED_OFF_LONG_TIME_MS) {
            ledOffPeriodCnt += TASK_PERIOD_MS;
        }
        else {
            pin_.SetState(true);
            ledOnPeriodCnt = 0;
            state = STATE_LED_ON;
            pulseCnt = 0;
            communication = cwdt_monitoring_;
        }
        break;
    default:
        ledOnPeriodCnt = 0;
        state = STATE_LED_ON;
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
bool StatusLed::ProcessRequest(Frame& request, Frame& response) {
    (void)request;
    (void)response;
//    _communication = true;
    return false;
}

} /* namespace module */
} /* namespace i2c_hat */
