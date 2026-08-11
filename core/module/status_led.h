/*
 * status_led.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_STATUS_LED_H_
#define MODULE_STATUS_LED_H_

#include "module.h"
#include "board.h"
#include "../driver/digital_output_pin.h"

#ifdef STATUS_LED_CIA_303_3
#include "led_pattern.h"
#endif

namespace i2c_hat {
namespace module {

/* Two behaviours, chosen by the board:
 *
 * Legacy (default, every F0 board): one 75 ms pulse every 3 s, doubled while
 * the communication watchdog is monitoring. It carries "alive" and "a master
 * is talking" on the single LED those boards have, so it must NOT change -
 * they are in the field and their blink is what users know.
 *
 * STATUS_LED_CIA_303_3 (boards that also carry an ERR LED): the green LED
 * becomes the CiA 303-3 RUN indicator and nothing else, because the red one
 * now owns faults. It maps 1:1 onto the watchdog's own three states:
 *      MONITORING -> on            OPERATIONAL     (supervised)
 *      DISABLED   -> blinking      PRE-OPERATIONAL (alive, unsupervised)
 *      TIMEOUT    -> single flash  STOPPED         (+ red double flash)
 * Single flash for TIMEOUT rather than falling back to blinking keeps the
 * mapping 1:1 and keeps the trip diagnosable on a variant that does not
 * populate the red LED. */
class StatusLed: public Module {
public:
    StatusLed();
    void Init();
    void Run();
    bool ProcessRequest(Frame& request, Frame& response);
private:
#ifdef STATUS_LED_CIA_303_3
    enum class RunState {
        PRE_OPERATIONAL,
        OPERATIONAL,
        STOPPED,
    };
    driver::DigitalOutputPin pin_;
    RunState run_state_;
    LedPattern pattern_;
#else
    typedef enum {
        STATE_LED_INIT,
        STATE_LED_ON,
        STATE_LED_OFF_SHORT,
        STATE_LED_OFF_LONG,
    } State;
    driver::DigitalOutputPin pin_;
    bool cwdt_monitoring_;
#endif
    void ReceiveEvent(const uint32_t event);
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_STATUS_LED_H_ */
