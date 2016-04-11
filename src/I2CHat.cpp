/*
 * I2CHat.cpp
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#include "I2CHat.h"
#include "Utils.h"
#include "module/StatusLed.h"
#include "module/CommWatchdog.h"
#include "module/DigitalOutputs.h"
#include "module/DigitalInputs.h"

#define MAX_TASK_COUNT      (10)

namespace i2c_hat {

I2CHat::I2CHat() : CooperativeScheduler(MAX_TASK_COUNT), Board() {
    _rxLength = 0;
    _txLength = 0;
    _requestFrame = new I2CFrame();
    _responseFrame = new I2CFrame();

    // register modules
    registerModule(new CommWatchdog());
    registerModule(new StatusLed(getStatusLedPin()));

#ifdef DIGITAL_INPUT_CHANNEL_COUNT
    registerModule(new DigitalInputs(getDigitalInputPort()));
#endif

#ifdef DIGITAL_OUTPUT_CHANNEL_COUNT
    registerModule(new DigitalOutputs(getDigitalOutputPort()));
#endif
}

bool I2CHat::registerModule(Module* module) {
    return registerTask(module);
}

Module** I2CHat::getModuleList() {
    return (Module**)getTaskList();
}

void I2CHat::run() {
    dispatch();
    getI2CPort()->transfer(&_rxLength, &_txLength);
    if(_requestFrame->decode(getI2CPort()->getReceiveBuffer(), _rxLength) == FD_RCODE_SUCCESS) {
        if(processRequest(_requestFrame, _responseFrame)) {
            _txLength = _responseFrame->encode(getI2CPort()->getTransmitBuffer(), getI2CPort()->getBufferSize());
        }
    }
    getWatchDog()->feed();
}

uint32_t I2CHat::processRequest(I2CFrame *request, I2CFrame *response) {
    uint32_t i;
    uint32_t responseFlag;
    Module **moduleList;
    Module *module;

    response->setId(request->getId());
    response->setCmd(request->getCmd());

    moduleList = getModuleList();

    responseFlag = 0;
    i = 0;
    do {
        module = moduleList[i];
        if(module != NULL) {
            responseFlag = module->processRequest(request, response);
        }
    } while((responseFlag == 0) && (++i < getTaskCount()));

    if(responseFlag == 0) {
        switch(request->getCmd()) {
        case CMD_GET_BOARD_NAME:
            if(request->getLength() == 0) {
                response->setData(getName(), getNameSize());
                responseFlag = 1;
            }
            break;
        case CMD_GET_FIRMWARE_VERSION:
            if(request->getLength() == 0) {
                response->setData(getFirmwareVersion(), getFirmwareVersionSize());
                responseFlag = 1;
            }
            break;
        case CMD_GET_STATUS_WORD:
            if(request->getLength() == 0) {
                Utils::convertUint32ToBytes(getStatusWord(), response->getData());
                response->setLength(4);
                responseFlag = 1;
            }
            break;
        case CMD_RESET:
            if(request->getLength() == 0) {
                NVIC_SystemReset();
            }
            break;
        default:
            responseFlag = 0;
            break;
        }
    }

    return responseFlag;
}

} /* namespace i2c_hat */
