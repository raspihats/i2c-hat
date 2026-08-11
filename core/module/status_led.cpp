/*
 * status_led.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "status_led.h"
#include "board.h"

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
#ifdef STATUS_LED_CIA_303_3
StatusLed::StatusLed() :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS),
        pin_(STATUS_LED_PIN),
        run_state_(RunState::PRE_OPERATIONAL),
        pattern_() {
}
#else
StatusLed::StatusLed() :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS),
        pin_(STATUS_LED_PIN),
        cwdt_monitoring_(false) {
}
#endif

/**
  * @brief  StatusLed Module receiveEvent implementation
  * @param  event: event code
  * @retval None
  */
void StatusLed::ReceiveEvent(const uint32_t event) {
#ifdef STATUS_LED_CIA_303_3
    if(event == EVENT_CWDT_MONITORING) {
        run_state_ = RunState::OPERATIONAL;
    }
    else if(event == EVENT_CWDT_DISABLED) {
        run_state_ = RunState::PRE_OPERATIONAL;
    }
    else if(event == EVENT_CWDT_TIMEOUT) {
        run_state_ = RunState::STOPPED;
    }
#else
    // Legacy: a trip and "never supervised" deliberately look the same here,
    // because this LED has no way to say more. Boards with an ERR LED use
    // STATUS_LED_CIA_303_3 instead, where the two are distinct states.
    if(event == EVENT_CWDT_MONITORING) {
        cwdt_monitoring_ = true;
    }
    else if((event == EVENT_CWDT_DISABLED) or (event == EVENT_CWDT_TIMEOUT)) {
        cwdt_monitoring_ = false;
    }
#endif
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
#ifdef STATUS_LED_CIA_303_3
void StatusLed::Run() {
    switch(run_state_) {
    case RunState::OPERATIONAL:
        pattern_.set(LedPattern::Kind::ON);
        break;
    case RunState::STOPPED:
        pattern_.set(LedPattern::Kind::SINGLE_FLASH);
        break;
    default:
        pattern_.set(LedPattern::Kind::BLINKING);
        break;
    }
    pin_.SetState(pattern_.Tick(TASK_PERIOD_MS));
}
#else
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

#endif /* STATUS_LED_CIA_303_3 */

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
