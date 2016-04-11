/*
 * DigitalInputs.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_DIGITALINPUTS_H_
#define MODULE_DIGITALINPUTS_H_

#include "Module.h"
#include "driver/DigitalInputPort.h"

namespace i2c_hat {

typedef enum {
    DI_ERRCODE_SUCCESS,
    DI_ERRCODE_CHANNEL_OUT_OF_RANGE,
    DI_ERRCODE_COUNTER_TYPE_OUT_OF_RANGE,
} di_errcode_t;

typedef enum {
    DIGITAL_INPUT_COUNTER_TYPE_FALLING_EDGE,
    DIGITAL_INPUT_COUNTER_TYPE_RISING_EDGE,
    DIGITAL_INPUT_COUNTER_TYPES_COUNT
} di_counter_type_t;

class DigitalInputs: public Module {
private:
    DigitalInputPort *_port;
    uint32_t _integratorUpperLimit;
    uint32_t _filteredInputs;
    uint32_t *_risingEdgeCounters;
    uint32_t *_fallingEdgeCounters;
    uint32_t *_integrators;

    di_errcode_t getSingleChannel(const uint32_t channel, uint8_t* const state);
    uint32_t getAllChannels();
    di_errcode_t getSingleCounter(const uint32_t channel, const di_counter_type_t type, uint32_t* const value);
    di_errcode_t resetSingleCounter(const uint32_t channel, const di_counter_type_t type);
    void resetAllCounters();
public:
    DigitalInputs(DigitalInputPort *dinPort);
    void onInit(const uint32_t tickFrequency);
    void task();
    uint32_t processRequest(I2CFrame *request, I2CFrame *response);
    ~DigitalInputs();
};

} /* namespace i2c_hat */

#endif /* MODULE_DIGITALINPUTS_H_ */
