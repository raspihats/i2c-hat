/*
 * I2CPort.cpp
 *
 *  Created on: Apr 3, 2016
 *      Author: fcos
 */

#include "I2CPort.h"

namespace i2c_hat {

I2CPort::I2CPort(i2c_port_t *port, const uint32_t bufferSize, const uint8_t address) {
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;
    uint32_t i;
    uint16_t pinSource;

    _port = port;
    _bufferSize = bufferSize;
    _rxBuffer = new uint8_t[bufferSize];
    _txBuffer = new uint8_t[bufferSize];
    _address = address;
    _state = I2C_ST_INIT;

    // GPIOs
    GPIO_StructInit(&GPIO_InitStructure);

    /* I2C SDA and SCL GPIOs configuration */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    for(i = 0; i < 2; i++) {
        GPIO_InitStructure.GPIO_Pin = _port->pins[i].pin;
        GPIO_Init(_port->pins[i].port, &GPIO_InitStructure);
        pinSource = Driver::getPinSource(_port->pins[i].pin);
        GPIO_PinAFConfig(_port->pins[i].port, pinSource, _port->pins[i].altFunction);
    }

    //I2C peripheral
    I2C_StretchClockCmd(_port->port, ENABLE);

    /* I2C configuration */
    I2C_StructInit(&I2C_InitStructure);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_OwnAddress1 = _address << 1; //shift because of R/W bit
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(_port->port, &I2C_InitStructure);

    I2C_DMACmd(_port->port, I2C_DMAReq_Tx, ENABLE);
    I2C_DMACmd(_port->port, I2C_DMAReq_Rx, ENABLE);
    I2C_Cmd(_port->port, ENABLE);
}

/**
  * @brief  Initializes the I2C transmit DMA channel.
  * @param  None
  * @retval None
  */
void I2CPort::initTransmitDma() {
    DMA_InitTypeDef DMA_InitStructure;

    _port->port->ISR |= I2C_FLAG_TXE;      // Flush data transmit register

    DMA_Cmd(_port->dmaTxChannel, DISABLE);
    DMA_DeInit(_port->dmaTxChannel);

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(_port->port->TXDR));
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)_txBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = _bufferSize;

//    DMA_ITConfig(_port->dmaTxChannel, DMA_IT_TC, ENABLE);
    DMA_Init(_port->dmaTxChannel, &DMA_InitStructure);
    DMA_Cmd(_port->dmaTxChannel, ENABLE);
//    I2C_DMACmd(_port->port, I2C_DMAReq_Tx, ENABLE);
}

/**
  * @brief  Initializes the I2C receive DMA channel.
  * @param  None
  * @retval None
  */
void I2CPort::initReceiveDma() {
    DMA_InitTypeDef DMA_InitStructure;

    DMA_Cmd(_port->dmaRxChannel, DISABLE);
    DMA_DeInit(_port->dmaRxChannel);

    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(_port->port->RXDR));
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)_rxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = _bufferSize;

//    DMA_ITConfig(_port->dmaRxChannel, DMA_IT_TC, ENABLE);
    DMA_Init(_port->dmaRxChannel, &DMA_InitStructure);
    DMA_Cmd(_port->dmaRxChannel, ENABLE);
//    I2C_DMACmd(_port->port, I2C_DMAReq_Rx, ENABLE);
}

/**
  * @brief  Reads the I2C address.
  * @param  None
  * @retval I2C address
  */
uint8_t I2CPort::getAddress() {
    return _address;
}

/**
  * @brief  Get pointer to receive buffer.
  * @param  None
  * @retval I2C receive buffer pointer
  */
uint8_t *I2CPort::getReceiveBuffer() {
    return _rxBuffer;
}

/**
  * @brief  Get pointer to transmit buffer.
  * @param  None
  * @retval I2C transmit buffer pointer
  */
uint8_t *I2CPort::getTransmitBuffer() {
    return _txBuffer;
}

/**
  * @brief  Get receive and transmit buffers size.
  * @param  None
  * @retval buffer size
  */
uint32_t I2CPort::getBufferSize() {
    return _bufferSize;
}

void I2CPort::transfer(uint32_t * const rxLength, uint32_t * const txLength) {
    *rxLength = 0;

    if( (I2C_GetFlagStatus(_port->port, I2C_FLAG_BERR) == SET) or
            (I2C_GetFlagStatus(_port->port, I2C_FLAG_ARLO) == SET) or
            (I2C_GetFlagStatus(_port->port, I2C_FLAG_OVR) == SET) or
            (I2C_GetFlagStatus(_port->port, I2C_FLAG_PECERR) == SET) ) {
        I2C_Cmd(_port->port, DISABLE);
        I2C_Cmd(_port->port, ENABLE);
        _state = I2C_ST_INIT;
    }

    switch(_state) {

    case I2C_ST_INIT:
        initReceiveDma();
        _state = I2C_ST_WAIT_TRANSFER;
        break;

    case I2C_ST_WAIT_TRANSFER:
        if( (I2C_GetFlagStatus(_port->port, I2C_FLAG_ADDR) == SET)
                or (I2C_GetFlagStatus(_port->port, I2C_FLAG_STOPF) == SET) ) {

            if(I2C_GetFlagStatus(_port->port, I2C_FLAG_STOPF) == SET) {
                I2C_ClearFlag(_port->port, I2C_FLAG_STOPF);
            }

            if(I2C_GetTransferDirection(_port->port) == I2C_Direction_Transmitter) {
                // Master wants to transmit data to Slave, clock is stretched
                *rxLength = _bufferSize - _port->dmaRxChannel->CNDTR;
                initReceiveDma();
            }
            else {
                // Master wants to receive data from Slave, clock is stretched
                if(*txLength > 0) {
                    initTransmitDma();
                    *txLength = 0;
                }
                else {

                }
            }
            if(I2C_GetFlagStatus(_port->port, I2C_FLAG_ADDR) == SET) {
                I2C_ClearFlag(_port->port, I2C_FLAG_ADDR);
            }
        }
        else {
            if( (I2C_GetTransferDirection(_port->port) == I2C_Direction_Receiver) and
                    (_port->dmaTxChannel->CNDTR == 0) and
                    (I2C_GetFlagStatus(_port->port, I2C_FLAG_TXE) == SET) ) {
                /*
                 * ############## Master reads more bytes than expected ##################
                 * Load dummy byte(0xFF) in case Master want's to read more bytes than the
                 * DMA has to transmit, to avoid blocking the I2C bus by clock-stretching.
                 */
                I2C_SendData(_port->port, 0xFF);
            }
        }
        break;

    default:
        _state = I2C_ST_INIT;
    }
}

I2CPort::~I2CPort() {
    delete[](_rxBuffer);
    delete[](_txBuffer);
}


} /* namespace i2c_hat */
