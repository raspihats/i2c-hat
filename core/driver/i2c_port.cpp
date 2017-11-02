/*
 * i2c_port.cpp
 *
 *  Created on: Apr 3, 2016
 *      Author: fcos
 */

#include "i2c_port.h"

namespace i2c_hat {
namespace driver {

I2CPort::I2CPort(I2C_TypeDef* port) :
        rx_buffer_{0},
        tx_buffer_{0},
        port_(port),
        state_(ST_INIT) {
}

/**
  * @brief  Reads the I2C address.
  * @param  None
  * @retval I2C address
  */
uint8_t I2CPort::address() {
    return port_->OAR1 >> 1;
}

/**
  * @brief  Get pointer to receive buffer.
  * @param  None
  * @retval I2C receive buffer pointer
  */
uint8_t *I2CPort::receive_buffer() {
    return rx_buffer_;
}

/**
  * @brief  Get receive buffer size.
  * @param  None
  * @retval Receive buffer size
  */
uint32_t I2CPort::reveive_buffer_size() {
    return BUFFER_SIZE;
}

/**
  * @brief  Get pointer to transmit buffer.
  * @param  None
  * @retval I2C transmit buffer pointer
  */
uint8_t *I2CPort::transmit_buffer() {
    return tx_buffer_;
}

/**
  * @brief  Get transmit buffer size.
  * @param  None
  * @retval Transmit buffer size
  */
uint32_t I2CPort::transmit_buffer_size() {
    return BUFFER_SIZE;
}

/**
  * @brief  Transfer I2C data, used for both read and write.
  * @param  rxSize
  * @param  txSize
  * @retval None
  */
void I2CPort::transfer(uint32_t * const receive_size, uint32_t * const transmit_size) {
    static uint32_t rx_count;
    static uint32_t tx_count;
    static uint32_t dummy;
    static uint32_t dir_count = 0;

    if( LL_I2C_IsActiveFlag_OVR(port_)
            or LL_I2C_IsActiveFlag_BERR(port_)
            or LL_I2C_IsActiveFlag_ARLO(port_) ) {
        state_ = ST_INIT;
    }

    if(LL_I2C_IsActiveFlag_ARLO(port_)) {
        LL_I2C_ClearFlag_ARLO(port_);
    }

    switch(state_) {

    case ST_INIT:
        rx_count = 0;
        *receive_size = 0;
        tx_count = 0;
        // clear all flags by disabling I2C port
        LL_I2C_Disable(port_);
        LL_I2C_Enable(port_);
        state_ = ST_WAIT_ADR;
        break;

    case ST_WAIT_ADR:
        if(LL_I2C_IsActiveFlag_ADDR(port_)) {
            LL_I2C_ClearFlag_ADDR(port_);
            LL_I2C_ClearFlag_TXE(port_); // purge tx reg
            if(LL_I2C_GetTransferDirection(port_) == LL_I2C_DIRECTION_WRITE) {
                dir_count++;
                rx_count = 0;
                *receive_size = 0;
                state_ = ST_WAIT_RX_STOP;
            }
            else {
                tx_count = 0;
                state_ = ST_WAIT_TX_STOP;
            }
        }
        break;

    case ST_WAIT_RX_STOP:
        if(LL_I2C_IsActiveFlag_RXNE(port_)) {
            if(rx_count < BUFFER_SIZE) {
                rx_buffer_[rx_count++] = port_->RXDR;
            }
            else {
                dummy = port_->RXDR;
            }
        }
        else if(LL_I2C_IsActiveFlag_ADDR(port_)) {
            state_ = ST_WAIT_ADR;
        }
        else if(LL_I2C_IsActiveFlag_STOP(port_)) {
            LL_I2C_ClearFlag_STOP(port_);
            *receive_size = rx_count;
            state_ = ST_WAIT_RX_PROCESSING;
        }
        break;

    case ST_WAIT_RX_PROCESSING:
        // Clock will be stretched until received data is processed
        if(*receive_size == 0) {
            state_ = ST_WAIT_ADR;
        }
        break;

    case ST_WAIT_TX_STOP:
        if(LL_I2C_IsActiveFlag_TXE(port_)) {
            if(tx_count < *transmit_size) {
                port_->TXDR = tx_buffer_[tx_count++];
            }
            else {
                *transmit_size = 0;
                port_->TXDR = dummy;
            }
        }
        else if(LL_I2C_IsActiveFlag_ADDR(port_)) {
            state_ = ST_WAIT_ADR;
        }
        else if(LL_I2C_IsActiveFlag_STOP(port_)) {
            LL_I2C_ClearFlag_STOP(port_);
            *transmit_size = 0;
            state_ = ST_WAIT_ADR;
        }
        break;

    default:
        state_ = ST_INIT;
    }
}

} /* namespace driver */
} /* namespace i2c_hat */
