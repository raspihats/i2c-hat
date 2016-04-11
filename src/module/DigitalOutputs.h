/*
 * DigitalOutputs.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_DIGITALOUTPUTS_H_
#define MODULE_DIGITALOUTPUTS_H_

#include "Module.h"
#include "driver/DigitalOutputPort.h"

namespace i2c_hat {

typedef enum {
    DO_ERRCODE_SUCCESS,
    DO_ERRCODE_CHANNEL_OUT_OF_RANGE
} do_errcode_t;

class DigitalOutputs: public Module {
private:
    DigitalOutputPort *_port ;
    uint32_t _powerOnValue;
    uint32_t _safetyValue;

    bool validValue(const uint32_t state);
    void loadPowerOnValue();
    void setPowerOnValue(const uint32_t powerOnstate);
    void loadSafetyValue();
    void setSafetyValue(const uint32_t safetystate);
    do_errcode_t setSingleChannel(const uint8_t channel, const uint8_t state);
    do_errcode_t getSingleChannel(const uint8_t channel, uint8_t* const state);
    void setAllChannels(const uint32_t states);
    uint32_t getAllChannels();
    void receiveEvent(const uint32_t event);
public:
    DigitalOutputs(DigitalOutputPort* doPort);
    uint32_t processRequest(I2CFrame *request, I2CFrame *response);
};

} /* namespace i2c_hat */

#endif /* MODULE_DIGITALOUTPUTS_H_ */
