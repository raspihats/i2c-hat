/*
 * i2c_hat.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: fcos
 *
 *  Shared, board-agnostic I2C-HAT framework implementation.
 *  Board-specific wiring comes from board.h (see i2c_hat.h).
 */

#include "i2c_hat.h"
#include "driver/bootloader.h"

#ifndef BOARD_REGISTER_MODULES
#define BOARD_REGISTER_MODULES
#endif

#ifndef BOARD_STATUS_BITS
#define BOARD_STATUS_BITS
#endif

namespace i2c_hat {

I2CHat::I2CHat() :
        cooperative_os::Scheduler(),
        kFirmwareVersion{FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH},
        kBoardName(BOARD_NAME),
        i2c_port_(I2C1),
        status_(0) {

    // Register modules. status_led_ and communication_watchdog_ exist on every
    // board; the board-specific modules are registered via the board.h macro.
    Register(status_led_);
    Register(communication_watchdog_);
    BOARD_REGISTER_MODULES
}

void I2CHat::UpdateStatusWord() {
    // Bit 0 is "power-on reset": the F0 RCC reports it as PORRST, the G0
    // folds POR/PDR/BOR into PWRRST (there is no PORRST flag).
#if defined(I2C_HAT_MCU_FAMILY_G0)
    status_ |= LL_RCC_IsActiveFlag_PWRRST()             ? 0x01 : 0x00;
#else
    status_ |= LL_RCC_IsActiveFlag_PORRST()             ? 0x01 : 0x00;
#endif
    status_ |= LL_RCC_IsActiveFlag_SFTRST()             ? 0x02 : 0x00;
    status_ |= LL_RCC_IsActiveFlag_IWDGRST()            ? 0x04 : 0x00;
    status_ |= communication_watchdog_.IsExpired()      ? 0x08 : 0x00;
    BOARD_STATUS_BITS
}

uint32_t I2CHat::GetStatusWord() {
    uint32_t status;

    status = status_;

    // clear status
    status_ = 0;
    LL_RCC_ClearResetFlags();

    return status;
}

bool I2CHat::Register(module::Module& module) {
    return RegisterTask(&module);
}

module::Module** I2CHat::GetModuleList() {
    return (module::Module**)GetTaskList();
}

uint32_t I2CHat::GetModuleCount() {
    return GetTaskCount();
}

void I2CHat::Init() {
    uint32_t i;

    driver::Eeprom::Init();

    for(i = 0; i < GetModuleCount(); i++) {
        GetModuleList()[i]->Init();
    }
}

void I2CHat::Run() {
    static Frame request;
    static Frame response;
    static uint32_t receive_size;
    static uint32_t transmit_size;

    Dispatch();

    receive_size = 0;
    i2c_port_.transfer(receive_size, transmit_size);
    if(request.Decode(i2c_port_.receive_buffer(), receive_size) == FD_RCODE_SUCCESS) {
        if(ProcessRequest(request, response)) {
            transmit_size = response.Encode(i2c_port_.transmit_buffer(), i2c_port_.transmit_buffer_size());
        }
    }

    UpdateStatusWord();
    // feed watchdog
    LL_IWDG_ReloadCounter(IWDG);
}

bool I2CHat::ProcessRequest(Frame& request, Frame& response) {
    module::Module **moduleList;
    uint32_t i;
    static uint32_t u32;
    bool response_flag;

    response.set_id(request.id());
    response.set_command(request.command());

    response_flag = false;
    moduleList = GetModuleList();
    for(i = 0; i < GetModuleCount(); i++) {
        response_flag = moduleList[i]->ProcessRequest(request, response);
        if(response_flag) {
            break;
        }
    }

    if(not response_flag) {
        switch((Command)request.command()) {
        case Command::GET_BOARD_NAME:
            if(request.payload_size() == 0) {
                response.set_payload(kBoardName, sizeof(kBoardName));
                response_flag = true;
            }
            break;
        case Command::GET_FIRMWARE_VERSION:
            if(request.payload_size() == 0) {
                response.set_payload(kFirmwareVersion, sizeof(kFirmwareVersion));
                response_flag = true;
            }
            break;
        case Command::GET_STATUS_WORD:
            if(request.payload_size() == 0) {
                u32 = GetStatusWord();
                response.set_payload((uint8_t*)&u32, 4);
                response_flag = true;
            }
            break;
        case Command::RESET:
            if(request.payload_size() == 0) {
                NVIC_SystemReset();
            }
            break;
        case Command::RESTORE_FACTORY_DEFAULTS:
            // CiA 301 0x1011: acts only on the "load" signature, so a stray
            // write can never wipe a commissioning. Formats the EEPROM and
            // resets; every persistent register falls back to its default.
            // Like RESET, no response is sent.
            if(request.payload_size() == 4
                    and request.payload()[0] == 'l'
                    and request.payload()[1] == 'o'
                    and request.payload()[2] == 'a'
                    and request.payload()[3] == 'd') {
                if(driver::Eeprom::Format()) {
                    NVIC_SystemReset();
                }
            }
            break;
        case Command::ENTER_BOOTLOADER:
            // Guarded like 0x1011: only the "boot" signature acts. Resets
            // with a .noinit magic planted; the pre-init check in the
            // board's main() then jumps to the ROM bootloader (see
            // driver/bootloader.h for why it is not a direct jump - IWDG).
            // Like RESET, no response is sent. The board re-enumerates at
            // the ROM's I2C address (0x3E on F04x, 0x56 on G0).
            if(request.payload_size() == 4
                    and request.payload()[0] == 'b'
                    and request.payload()[1] == 'o'
                    and request.payload()[2] == 'o'
                    and request.payload()[3] == 't') {
                Bootloader_Request();
            }
            break;
        case Command::CONFIG_SET_SIGNATURE:
            // CiA 301 0x1020: stored verbatim; any real change to another
            // persistent value voids it back to 0 (choke point in
            // driver/eeprom.cpp). Echoes the value like every SET.
            if(request.payload_size() == 4) {
                BYTES_TO_UINT32(request.payload(), u32);
                if(driver::Eeprom::Write(EEP_VIRT_ADR_CONFIG_SIGNATURE, u32)) {
                    response.set_payload((uint8_t*)&u32, 4);
                    response_flag = true;
                }
            }
            break;
        case Command::CONFIG_GET_SIGNATURE:
            if(request.payload_size() == 0) {
                if(not driver::Eeprom::Read(EEP_VIRT_ADR_CONFIG_SIGNATURE, u32)) {
                    u32 = 0;    // never stored (or wiped): no claim
                }
                response.set_payload((uint8_t*)&u32, 4);
                response_flag = true;
            }
            break;
        default:
            response_flag = false;
            break;
        }
    }

    return response_flag;
}

} /* namespace i2c_hat */
