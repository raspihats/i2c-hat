/*
 * led_pattern.h
 *
 *  The CiA 303-3 indicator flash grammar, shared by the RUN (status) and
 *  ERR LEDs so both boards and both indicators speak the same dialect.
 *
 *  Header-only on purpose: it is a few counters, and keeping it out of the
 *  build file means a board that uses neither indicator pays nothing.
 *
 *  Timings are the ones CiA 303-3 specifies:
 *      flash on        200 ms
 *      gap in a burst  200 ms
 *      pause after     1000 ms
 *      blinking        200 ms on / 200 ms off
 *  A "single flash" is therefore 200 ms on, 1000 ms off; a double flash is
 *  on-gap-on-pause, and so on. Off and On are steady states.
 */

#ifndef MODULE_LED_PATTERN_H_
#define MODULE_LED_PATTERN_H_

#include <stdint.h>

namespace i2c_hat {
namespace module {

class LedPattern {
public:
    enum class Kind {
        OFF,
        SINGLE_FLASH,
        DOUBLE_FLASH,
        TRIPLE_FLASH,
        BLINKING,
        ON,
    };

    LedPattern() : kind_(Kind::OFF), on_(false), elapsed_(0), pulse_(0) {}

    /* Selecting a different pattern restarts it, so a state change is always
     * visible from the first flash instead of joining mid-burst. */
    void set(const Kind kind) {
        if(kind != kind_) {
            kind_ = kind;
            on_ = (kind == Kind::ON) or (kind == Kind::BLINKING)
                    or IsFlash(kind);
            elapsed_ = 0;
            pulse_ = 0;
        }
    }

    Kind kind() const { return kind_; }

    /* Advances by one task period and returns the state the pin should have. */
    bool Tick(const uint32_t period_ms) {
        switch(kind_) {
        case Kind::OFF:
            return false;
        case Kind::ON:
            return true;
        case Kind::BLINKING:
            elapsed_ += period_ms;
            if(elapsed_ >= kBlinkMs) {
                elapsed_ = 0;
                on_ = not on_;
            }
            return on_;
        default:
            break;
        }

        /* flash bursts */
        elapsed_ += period_ms;
        if(on_) {
            if(elapsed_ >= kFlashOnMs) {
                elapsed_ = 0;
                on_ = false;
                pulse_++;
            }
        }
        else {
            const uint32_t off_ms = (pulse_ >= Pulses(kind_)) ? kPauseMs : kGapMs;
            if(elapsed_ >= off_ms) {
                elapsed_ = 0;
                on_ = true;
                if(pulse_ >= Pulses(kind_)) {
                    pulse_ = 0;
                }
            }
        }
        return on_;
    }

private:
    static const uint32_t kFlashOnMs = 200;
    static const uint32_t kGapMs     = 200;
    static const uint32_t kPauseMs   = 1000;
    static const uint32_t kBlinkMs   = 200;

    static bool IsFlash(const Kind kind) {
        return (kind == Kind::SINGLE_FLASH) or (kind == Kind::DOUBLE_FLASH)
                or (kind == Kind::TRIPLE_FLASH);
    }

    static uint32_t Pulses(const Kind kind) {
        switch(kind) {
        case Kind::SINGLE_FLASH: return 1;
        case Kind::DOUBLE_FLASH: return 2;
        case Kind::TRIPLE_FLASH: return 3;
        default:                 return 0;
        }
    }

    Kind kind_;
    bool on_;
    uint32_t elapsed_;
    uint32_t pulse_;
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_LED_PATTERN_H_ */
