/*
 * i2c_port.h
 *
 *  Created on: Apr 3, 2016
 *      Author: fcos
 */

#ifndef DRIVER_I2CPORT_H_
#define DRIVER_I2CPORT_H_

#include "stm32f0xx.h"
#include "stm32f0xx_ll_i2c.h"

#define BUFFER_SIZE         (256)

namespace i2c_hat {
namespace driver {

class I2CPort {
public:
    I2CPort(I2C_TypeDef* port);
    uint8_t address();
    uint8_t* receive_buffer();
    uint32_t reveive_buffer_size();
    uint8_t* transmit_buffer();
    uint32_t transmit_buffer_size();
    void transfer(uint32_t * const receive_size, uint32_t * const transmit_size);
private:
    enum State {
        ST_INIT,
        ST_WAIT_ADR,
        ST_WAIT_RX_STOP,
        ST_WAIT_RX_PROCESSING,
        ST_WAIT_TX_STOP,
        ST_WAIT_TRANSFER,
    };
    uint8_t             rx_buffer_[BUFFER_SIZE];
    uint8_t             tx_buffer_[BUFFER_SIZE];
    I2C_TypeDef*        port_;
    State               state_;
};

} /* namespace driver */
} /* namespace i2c_hat */

#endif /* DRIVER_I2CPORT_H_ */
