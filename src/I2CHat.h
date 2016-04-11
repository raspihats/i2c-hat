/*
 * I2CHat.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef I2CHAT_H_
#define I2CHAT_H_

#include <stdint.h>
#include <stdio.h>
#include "driver/Board.h"
#include "coop_sch/CooperativeScheduler.h"
#include "i2c_frame/I2CFrame.h"
#include "module/Module.h"

namespace i2c_hat {

class I2CHat : public CooperativeScheduler, Board {
private:
    uint32_t _rxLength;
    uint32_t _txLength;
    I2CFrame *_requestFrame;
    I2CFrame *_responseFrame;
    bool registerModule(Module* module);
    Module** getModuleList();
    uint32_t processRequest(I2CFrame *request, I2CFrame *response);
public:
    I2CHat();
    void run();
};

} /* namespace i2c_hat */

#endif /* I2CHAT_H_ */
