/*
 * error_led.h
 *
 *  The CiA 303-3 ERR indicator (red). Compiled only for boards whose board.h
 *  defines ERROR_LED_PIN; the LED is otherwise absent and so is this module.
 *
 *  It exists because the status word is not a diagnostic channel when the
 *  host is the thing that failed: reading it needs a working master, and the
 *  most common field question ("why did all my outputs drop?") is asked
 *  precisely when there is none. The red LED answers it on the board.
 *
 *  Conditions are a bitmask so several can be true at once; the most severe
 *  one owns the LED. Report them with Set()/Clear() from anywhere - they are
 *  static, so a driver deep in the stack (EEPROM, a sensor bus) can raise a
 *  fault without needing a reference to this module.
 *
 *  Deliberately NOT derived from the status word: GetStatusWord() clears
 *  status_ on every read, so a polling host would silently wipe the
 *  indication. The conditions below track live state instead.
 */

#ifndef MODULE_ERROR_LED_H_
#define MODULE_ERROR_LED_H_

#include "module.h"
#include "board.h"

#ifdef ERROR_LED_PIN

#include "led_pattern.h"
#include "../driver/digital_output_pin.h"

namespace i2c_hat {
namespace module {

class ErrorLed: public Module {
public:
    /* CiA 303-3 ERR patterns, listed least to most severe. */
    enum Condition {
        /* single flash - a limit was reached, the device still works
         * (analog boards: a measurement alert; digital boards: unused) */
        CONDITION_WARNING           = 0x01,
        /* double flash - error control event. The CWDT timeout is exactly
         * CANopen's node-guarding/heartbeat failure by another name. */
        CONDITION_ERROR_CONTROL     = 0x02,
        /* blinking - invalid configuration: a config-signature mismatch
         * (CiA 301 0x1020) or an EEPROM that would not read or write */
        CONDITION_INVALID_CONFIG    = 0x04,
        /* on - fatal: the board cannot do its job at all */
        CONDITION_FATAL             = 0x08,
    };

    ErrorLed();
    void Init();
    void Run();
    bool ProcessRequest(Frame& request, Frame& response);

    static void Set(const uint32_t condition);
    static void Clear(const uint32_t condition);

private:
    static uint32_t conditions_;

    driver::DigitalOutputPin pin_;
    LedPattern pattern_;

    void ReceiveEvent(const uint32_t event);
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* ERROR_LED_PIN */

#endif /* MODULE_ERROR_LED_H_ */
