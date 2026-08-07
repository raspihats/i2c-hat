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
void I2CPort::transfer(uint32_t& receive_size, uint32_t& transmit_size) {
    static uint32_t rx_count;
    static uint32_t tx_count;
    static uint32_t dir_count = 0;
    static uint32_t arlo_cnt = 0;
    static bool release_clock_stretch_flag;

    // No OVR check: with clock stretching enabled (NOSTRETCH=0) the peripheral
    // stretches SCL instead of overrunning, so OVR can never fire (RM0091).
    if( LL_I2C_IsActiveFlag_BERR(port_)
            or LL_I2C_IsActiveFlag_ARLO(port_) ) {
        state_ = ST_INIT;
    }

    if(LL_I2C_IsActiveFlag_ARLO(port_)) {
        LL_I2C_ClearFlag_ARLO(port_);
        arlo_cnt++;
        state_ = ST_INIT;
    }

    switch(state_) {

    case ST_INIT:
        rx_count = 0;
        receive_size = 0;
        tx_count = 0;
        // clear all flags by disabling I2C port
        LL_I2C_Disable(port_);
        // PE must stay low >= 3 APB cycles for the internal reset to take
        // effect (RM0091); the readback enforces the hold time
        while(LL_I2C_IsEnabled(port_)) {
        }
        LL_I2C_Enable(port_);
        release_clock_stretch_flag = false;
        state_ = ST_WAIT_ADR;
        break;

    case ST_WAIT_ADR:
        if(LL_I2C_IsActiveFlag_ADDR(port_)) {
            if(LL_I2C_GetTransferDirection(port_) == LL_I2C_DIRECTION_WRITE) {
                LL_I2C_ClearFlag_ADDR(port_);
                dir_count++;
                rx_count = 0;
                receive_size = 0;
                state_ = ST_WAIT_MASTER_WRITE_STOP;
            }
            else {
                // Clock is stretched until ADDR flag is cleared
                tx_count = 0;
                state_ = ST_WAIT_PROCESSING;
            }
        }
        break;

    case ST_WAIT_MASTER_WRITE_STOP:
        if(LL_I2C_IsActiveFlag_RXNE(port_)) {
            if(rx_count < BUFFER_SIZE) {
                rx_buffer_[rx_count++] = port_->RXDR;
            }
            else {
                // buffer full: drain RXDR so the byte doesn't stretch the bus;
                // volatile read, not elided despite the discarded value
                (void)port_->RXDR;
            }
        }
        else if(LL_I2C_IsActiveFlag_STOP(port_)) {
            // Checked before ADDR: a fast master starts the response read
            // within microseconds of the write's STOP, so both flags can be
            // pending by the time this loop looks. STOP must win, or the
            // completed command is dropped and the read is served a stale
            // response. The pending read keeps SCL stretched in hardware
            // (ADDR stays uncleared) while the command is processed - the
            // master needs no delay between write and read.
            LL_I2C_ClearFlag_STOP(port_);
            receive_size = rx_count;
            state_ = ST_WAIT_ADR;
        }
        else if(LL_I2C_IsActiveFlag_ADDR(port_)) {
            // ADDR without STOP: a repeated-START combined transfer. Its
            // write part is discarded by design - it is the SMBus command/
            // dummy byte of read_i2c_block_data (see raspihats lib), not a
            // framed command. Commands must arrive in STOP-terminated writes.
            state_ = ST_WAIT_ADR;
        }
        break;

    case ST_WAIT_PROCESSING:
        // Clock is stretched until ADDR flag is cleared
        // Clock will be stretched until received data is processed
        if(receive_size == 0) {     // Wait until received data is processed
            state_ = ST_WAIT_MASTER_READ_STOP;
            release_clock_stretch_flag = true;
        }
        break;

    case ST_WAIT_MASTER_READ_STOP:
        if(LL_I2C_IsActiveFlag_TXE(port_)) {
            if(tx_count < transmit_size) {
                port_->TXDR = tx_buffer_[tx_count++];
                if(release_clock_stretch_flag) {
                    release_clock_stretch_flag = false;
                    LL_I2C_ClearFlag_ADDR(port_);
                }
                if(tx_count == transmit_size) {
                    transmit_size = 0;
                    tx_count = 0;
                }
            }
            else {
                // load dummy value, it's OK to enter once after full master read
                port_->TXDR = 0xEE; // master over-read error code
                if(release_clock_stretch_flag) {
                    release_clock_stretch_flag = false;
                    LL_I2C_ClearFlag_ADDR(port_);
                }
            }
        }
        else if(LL_I2C_IsActiveFlag_ADDR(port_)) {
            LL_I2C_ClearFlag_TXE(port_);  // purge tx reg
            LL_I2C_ClearFlag_NACK(port_); // master NACKs the last read byte by design
            state_ = ST_WAIT_ADR;
        }
        else if(LL_I2C_IsActiveFlag_STOP(port_)) {
            LL_I2C_ClearFlag_TXE(port_);  // purge tx reg
            LL_I2C_ClearFlag_NACK(port_); // master NACKs the last read byte by design
            LL_I2C_ClearFlag_STOP(port_);
            state_ = ST_WAIT_ADR;
        }
        break;

    default:
        state_ = ST_INIT;
    }
}

} /* namespace driver */
} /* namespace i2c_hat */
