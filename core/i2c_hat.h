/*
 * I2CHat.h
 *
 *  Created on: Oct 12, 2017
 *      Author: fcos
 */

#ifndef I2C_HAT_H_
#define I2C_HAT_H_

#include "cooperative_os/scheduler.h"
#include "driver/eeprom.h"
#include "driver/i2c_port.h"
#include "frame/frame.h"
#include "module/communication_watchdog.h"
#include "module/digital_inputs.h"
#include "module/status_led.h"
#include "board.h"

namespace i2c_hat {

class I2CHat : public cooperative_os::Scheduler {
public:
    I2CHat();
    void Init();
    void Run();
private:
    const uint8_t kFirmwareVersion[FW_VERSION_SIZE];
    const uint8_t kBoardName[BOARD_NAME_SIZE];
    driver::I2CPort i2c_port_;

    /******** MODULES *******/
    module::CommunicationWatchdog communication_watchdog_;
    module::StatusLed status_led_;
    module::DigitalInputs digital_inputs_;

    uint8_t* GetStatusWord();
    bool Register(module::Module& module);
    module::Module** GetModuleList();
    uint32_t GetModuleCount();
    bool ProcessRequest(Frame& request, Frame& response);
};

} /* namespace i2c_hat */

#endif /* I2C_HAT_H_ */
