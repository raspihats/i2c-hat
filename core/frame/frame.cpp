/*
 * Frame.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: fcos
 */
#include "crc16.h"
#include "frame.h"

namespace i2c_hat {

#define FID_SIZE                (1)
#define CMD_SIZE                (1)
#define CRC_SIZE                (2)
#define MIN_SIZE                (FID_SIZE + CMD_SIZE + CRC_SIZE)

#define FID_INDEX               (0)
#define CMD_INDEX               (1)
#define PAYLOAD_INDEX           (2)


/**
  * @brief  Builds a Frame object.
  * @param  None
  * @retval None
  */
Frame::Frame() :
        id_(0),
        cmd_(0),
        payload_(NULL),
        payload_size_(0) {
}

/**
  * @brief  Gets the frame ID byte.
  * @param  None
  * @retval frame ID
  */
uint8_t Frame::id() {
    return id_;
}

/**
  * @brief  Sets the frame ID byte.
  * @param  id: frame ID
  * @retval None
  */
void Frame::set_id(const uint8_t id) {
    id_ = id;
}

/**
  * @brief  Gets the frame COMMAND byte.
  * @param  None
  * @retval frame COMMAND
  */
uint8_t Frame::command() {
    return cmd_;
}

/**
  * @brief  Sets the frame COMMAND byte.
  * @param  cmd: frame COMMAND
  * @retval None
  */
void Frame::set_command(const uint8_t cmd) {
    cmd_ = cmd;
}

/**
  * @brief  Copies data to frame PAYLOAD.
  * @param  data: data pointer
  * @param  size: frame data pointer
  * @retval None
  */
void Frame::set_payload(const uint8_t* data, const uint32_t size) {
    payload_ = data;
    payload_size_ = size;
}

/**
  * @brief  Gets the frame PAYLOAD pointer.
  * @param  None
  * @retval frame data pointer
  */
const uint8_t* Frame::payload() {
    return payload_;
}

/**
  * @brief  Gets the frame data length.
  * @param  None
  * @retval frame data length
  */
uint32_t Frame::payload_size() {
    return payload_size_;
}

/**
  * @brief  Encodes frame to a byte buffer.
  * @param  buffer: pointer to buffer
  * @param  size: size of buffer
  * @retval The encode size value
  */
uint32_t Frame::Encode(uint8_t* const buffer, const uint32_t size) {
    uint32_t encodeSize, i;
    uint16_t crc;

    encodeSize = 0;
    if(size >= MIN_SIZE + payload_size_) {
        buffer[encodeSize++] = id_;
        buffer[encodeSize++] = cmd_;
        for(i = 0; i < payload_size_; i++) {
            buffer[encodeSize++] = payload_[i];
        }
        crc = Crc16::Calc(buffer, FID_SIZE + CMD_SIZE + payload_size_);
        buffer[encodeSize++] = (uint8_t)crc;
        buffer[encodeSize++] = (uint8_t)(crc >> 8);
    }
    return encodeSize;
}

/**
  * @brief  Decodes a byte buffer to frame.
  * @param  buffer: pointer to buffer
  * @param  size: size of buffer
  * @retval error code
  */
fd_rcode_t Frame::Decode(uint8_t* const buffer, const uint32_t size) {
    uint32_t crc;
    fd_rcode_t retCode;

    if (size >= MIN_SIZE) {
        crc = (buffer[size-1] << 8) + buffer[size-2];
        if(Crc16::Calc(buffer, size - CRC_SIZE) == crc) {
            id_ = buffer[FID_INDEX];
            cmd_ = buffer[CMD_INDEX];
            payload_size_ = size - MIN_SIZE;
            payload_ = buffer + PAYLOAD_INDEX;
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

} /* namespace i2c_hat */
