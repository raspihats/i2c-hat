/*
 * I2CFrame.cpp
 *
 *  Created on: Dec 20, 2015
 *      Author: fcos
 */
#include "Crc16.h"
#include "I2CFrame.h"

#define FID_LENGTH              (1)
#define CMD_LENGTH              (1)
#define CRC_LENGTH              (2)
#define MINIMUM_LENGTH          (FID_LENGTH + CMD_LENGTH + CRC_LENGTH)
#define DATA_BUFFER_SIZE        (512)


/**
  * @brief  Builds a I2CFrame object.
  * @param  None
  * @retval None
  */
I2CFrame::I2CFrame() {
    _id = 0;
    _cmd = 0;
    _data = new uint8_t[DATA_BUFFER_SIZE];
    _length = 0;
}

/**
  * @brief  Builds a I2CFrame object.
  * @param  None
  * @retval None
  */
I2CFrame::I2CFrame(const uint8_t id, const uint8_t cmd) {
    _id = id;
    _cmd = cmd;
    _data = new uint8_t[DATA_BUFFER_SIZE];;
    _length = 0;
}

/**
  * @brief  Sets the frame id byte.
  * @param  id: frame id
  * @retval None
  */
void I2CFrame::setId(const uint8_t id) {
    _id = id;
}

/**
  * @brief  Gets the frame id byte.
  * @param  None
  * @retval frame id
  */
uint8_t I2CFrame::getId() {
    return _id;
}

/**
  * @brief  Sets the frame command byte.
  * @param  cmd: frame command
  * @retval None
  */
void I2CFrame::setCmd(const uint8_t cmd) {
    _cmd = cmd;
}

/**
  * @brief  Gets the frame command byte.
  * @param  None
  * @retval frame command
  */
uint8_t I2CFrame::getCmd() {
    return _cmd;
}

/**
  * @brief  Sets the frame data pointer.
  * @param  data: frame data pointer
  * @param  length: frame data pointer
  * @retval None
  */
bool I2CFrame::setData(uint8_t* data, const uint32_t length) {
    bool copied = false;
    if(length < DATA_BUFFER_SIZE) {
        for(uint32_t i = 0; i < length; i++) {
            _data[i] = data[i];
        }
        _length = length;
        copied = true;
    }
    return copied;
}

/**
  * @brief  Gets the frame data pointer.
  * @param  None
  * @retval frame data pointer
  */
uint8_t* I2CFrame::getData() {
    return _data;
}

/**
  * @brief  Sets the frame data length.
  * @param  length: frame data length
  * @retval None
  */
void I2CFrame::setLength(const uint32_t length) {
    _length = length;
}

/**
  * @brief  Gets the frame data length.
  * @param  None
  * @retval frame data length
  */
uint32_t I2CFrame::getLength() {
    return _length;
}

/**
  * @brief  Encodes frame to a byte stream.
  * @param  data: pointer to byte stream
  * @param  length: length of byte stream
  * @retval The encode length value
  */
uint32_t I2CFrame::encode(uint8_t* const data, const uint32_t length) {
    uint32_t i, encodeLength;
    uint16_t crc;

    if(length >= (_length + MINIMUM_LENGTH)) {
        encodeLength = 0;
        data[encodeLength++] = _id;
        data[encodeLength++] = _cmd;
        for(i = 0; i < _length; i++) {
            data[encodeLength++] = _data[i];
        }
        crc = Crc16::calc(data, FID_LENGTH + CMD_LENGTH + _length);
        data[encodeLength++] = (uint8_t)crc;
        data[encodeLength++] = (uint8_t)(crc >> 8);
    }
    else {
        encodeLength = 0;
    }
    return encodeLength;
}

/**
  * @brief  Decodes a byte stream to frame.
  * @param  data: pointer to byte stream
  * @param  length: length of byte stream
  * @retval error code
  */
fd_rcode_t I2CFrame::decode(uint8_t* const data, const uint32_t length) {
    uint32_t crc;
    fd_rcode_t retCode;

    if ( (MINIMUM_LENGTH <= length) && (length < DATA_BUFFER_SIZE) ) {
        crc = (data[length-1] << 8) + data[length-2];
        if(Crc16::calc(data, length - CRC_LENGTH) == crc) {
            _id = data[0];
            _cmd = data[1];
            _length = length - MINIMUM_LENGTH;
            for(uint32_t i = 0; i < _length; i++) {
                _data[i] = data[2 + i];
            }
            retCode = FD_RCODE_SUCCESS;
        }
        else {
            retCode = FD_RCODE_BAD_CRC;
        }
    }
    else {
        retCode = FD_RCODE_BAD_LENGTH;
    }
    return retCode;
}

I2CFrame::~I2CFrame() {
    delete[](_data);
}
