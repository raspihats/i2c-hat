/*
 * i2c_hat.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: fcos
 */

#include "i2c_hat.h"
#include "stm32f0xx_ll_rcc.h"

namespace i2c_hat {

I2CHat::I2CHat() :
        cooperative_os::Scheduler(),
        kFirmwareVersion{FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH},
        kBoardName(BOARD_NAME),
        i2c_port_(I2C1) {

    // Register modules
    Register(status_led_);
    Register(communication_watchdog_);

#ifdef DIGITAL_INPUT_CHANNEL_COUNT
    Register(digital_inputs_);
#endif

#ifdef DIGITAL_OUTPUT_CHANNEL_COUNT
    Register(digital_outputs_);
#endif
}

uint8_t* I2CHat::GetStatusWord() {
    static uint32_t status;

    status = 0;
    status |= LL_RCC_IsActiveFlag_PORRST() ? 0x01 : 0;
    status |= LL_RCC_IsActiveFlag_SFTRST() ? 0x02 : 0;
    status |= LL_RCC_IsActiveFlag_IWDGRST() ? 0x04 : 0;
    LL_RCC_ClearResetFlags();
    return (uint8_t*)&status;
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
    i2c_port_.transfer(&receive_size, &transmit_size);
    if(request.Decode(i2c_port_.receive_buffer(), receive_size) == FD_RCODE_SUCCESS) {
        if(ProcessRequest(request, response)) {
            transmit_size = response.Encode(i2c_port_.transmit_buffer(), i2c_port_.transmit_buffer_size());
        }
    }
    // feed watchdog
    LL_IWDG_ReloadCounter(IWDG);
}

bool I2CHat::ProcessRequest(Frame& request, Frame& response) {
    bool response_flag;
    uint32_t i;
    module::Module **moduleList;

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
        switch(request.command()) {
        case CMD_GET_BOARD_NAME:
            if(request.payload_size() == 0) {
                response.set_payload(kBoardName, sizeof(kBoardName));
                response_flag = true;
            }
            break;
        case CMD_GET_FIRMWARE_VERSION:
            if(request.payload_size() == 0) {
                response.set_payload(kFirmwareVersion, sizeof(kFirmwareVersion));
                response_flag = true;
            }
            break;
        case CMD_GET_STATUS_WORD:
            if(request.payload_size() == 0) {
                response.set_payload(GetStatusWord(), 4);
                response_flag = true;
            }
            break;
        case CMD_RESET:
            if(request.payload_size() == 0) {
                NVIC_SystemReset();
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
