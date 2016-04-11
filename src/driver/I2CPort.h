/*
 * I2CPort.h
 *
 *  Created on: Apr 3, 2016
 *      Author: fcos
 */

#ifndef DRIVER_I2CPORT_H_
#define DRIVER_I2CPORT_H_

#include "BoardDef.h"

typedef enum {
    I2C_ST_INIT,
    I2C_ST_WAIT_ADDR_FLAG,
    I2C_ST_WAIT_STOP_FLAG,
} i2c_state_t;

namespace i2c_hat {

class I2CPort {
private:
    i2c_port_t *_port;
    uint32_t _bufferSize;
    uint8_t *_rxBuffer;
    uint8_t *_txBuffer;
    uint8_t _address;
    i2c_state_t _state;
    void initTransmitDma();
    void initReceiveDma();
public:
    I2CPort(i2c_port_t *port, const uint32_t bufferSize, const uint8_t address);
    uint8_t getAddress();
    uint8_t *getReceiveBuffer();
    uint8_t *getTransmitBuffer();
    uint32_t getBufferSize();
    void transfer(uint32_t * const rxLength, uint32_t * const txLength);
    ~I2CPort();
};

} /* namespace i2c_hat */

#endif /* DRIVER_I2CPORT_H_ */
