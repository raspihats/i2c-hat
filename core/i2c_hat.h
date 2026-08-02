/*
 * i2c_hat.h
 *
 *  Created on: Oct 12, 2017
 *      Author: fcos
 *
 *  Shared, board-agnostic I2C-HAT framework.
 *
 *  The set of modules a board exposes is NOT hard-coded here anymore. Each
 *  board declares it in its own board.h via three macros, keeping this file a
 *  single source of truth shared by every board:
 *
 *      BOARD_MODULE_MEMBERS    - member declarations for the board's modules
 *      BOARD_REGISTER_MODULES  - Register(...) calls for those modules
 *      BOARD_STATUS_BITS        - (optional) extra status-word bits
 *
 *  board.h is also responsible for pulling in the module headers it references
 *  and the MCU-family LL headers (stm32f0xx_ll_rcc.h / stm32f3xx_ll_rcc.h).
 */

#ifndef I2C_HAT_H_
#define I2C_HAT_H_

#include "cooperative_os/scheduler.h"
#include "driver/eeprom.h"
#include "driver/i2c_port.h"
#include "frame/frame.h"
#include "module/communication_watchdog.h"
#include "module/status_led.h"
#include "board.h"

#ifndef BOARD_MODULE_MEMBERS
#define BOARD_MODULE_MEMBERS
#endif

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
    uint32_t status_;

    /******** MODULES *******/
    module::CommunicationWatchdog communication_watchdog_;
    module::StatusLed status_led_;
    BOARD_MODULE_MEMBERS

    void UpdateStatusWord();
    uint32_t GetStatusWord();
    bool Register(module::Module& module);
    module::Module** GetModuleList();
    uint32_t GetModuleCount();
    bool ProcessRequest(Frame& request, Frame& response);
};

} /* namespace i2c_hat */

#endif /* I2C_HAT_H_ */
