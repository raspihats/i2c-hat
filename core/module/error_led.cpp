/*
 * error_led.cpp - see error_led.h.
 */

#include "error_led.h"

#ifdef ERROR_LED_PIN

#define TASK_DELAY_MS                   (0)
#define TASK_PERIOD_MS                  (5)

namespace i2c_hat {
namespace module {

uint32_t ErrorLed::conditions_ = 0;

ErrorLed::ErrorLed() :
        Module(TASK_DELAY_MS, TASK_PERIOD_MS),
        pin_(ERROR_LED_PIN),
        pattern_() {
}

void ErrorLed::Set(const uint32_t condition) {
    conditions_ |= condition;
}

void ErrorLed::Clear(const uint32_t condition) {
    conditions_ &= ~condition;
}

/**
  * @brief  The communication watchdog drives the error-control condition.
  *         Both ways out of TIMEOUT clear it: the master coming back
  *         (MONITORING) and supervision being switched off (DISABLED).
  * @param  event: event code
  * @retval None
  */
void ErrorLed::ReceiveEvent(const uint32_t event) {
    if(event == EVENT_CWDT_TIMEOUT) {
        Set(CONDITION_ERROR_CONTROL);
    }
    else if((event == EVENT_CWDT_MONITORING) or (event == EVENT_CWDT_DISABLED)) {
        Clear(CONDITION_ERROR_CONTROL);
    }
}

void ErrorLed::Init() {
    pin_.SetState(false);
    pattern_.set(LedPattern::Kind::OFF);
}

/**
  * @brief  Module cooperative Task implementation: the most severe active
  *         condition owns the LED.
  * @param  None
  * @retval None
  */
void ErrorLed::Run() {
    LedPattern::Kind kind;

    if((conditions_ & CONDITION_FATAL) != 0) {
        kind = LedPattern::Kind::ON;
    }
    else if((conditions_ & CONDITION_INVALID_CONFIG) != 0) {
        kind = LedPattern::Kind::BLINKING;
    }
    else if((conditions_ & CONDITION_ERROR_CONTROL) != 0) {
        kind = LedPattern::Kind::DOUBLE_FLASH;
    }
    else if((conditions_ & CONDITION_WARNING) != 0) {
        kind = LedPattern::Kind::SINGLE_FLASH;
    }
    else {
        kind = LedPattern::Kind::OFF;
    }

    pattern_.set(kind);
    pin_.SetState(pattern_.Tick(TASK_PERIOD_MS));
}

bool ErrorLed::ProcessRequest(Frame& request, Frame& response) {
    (void)request;
    (void)response;
    return false;
}

} /* namespace module */
} /* namespace i2c_hat */

#endif /* ERROR_LED_PIN */
