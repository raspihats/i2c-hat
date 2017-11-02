/*
 * status_led.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_STATUS_LED_H_
#define MODULE_STATUS_LED_H_

#include "module.h"
#include "../driver/digital_output_pin.h"

namespace i2c_hat {
namespace module {

class StatusLed: public Module {
public:
    StatusLed();
    void Init();
    void Run();
    bool ProcessRequest(Frame& request, Frame& response);
private:
    typedef enum {
        STATE_LED_INIT,
        STATE_LED_ON,
        STATE_LED_OFF_SHORT,
        STATE_LED_OFF_LONG,
    } State;
    driver::DigitalOutputPin pin_;
    bool cwdt_monitoring_;
    void ReceiveEvent(const uint32_t event);
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_STATUS_LED_H_ */
