/*
 * I2CFrame.h
 *
 *  Created on: Dec 20, 2015
 *      Author: fcos
 */

#ifndef I2C_FRAME_H_
#define I2C_FRAME_H_

#include <stdint.h>
#include <stdio.h>

typedef enum {
    FD_RCODE_SUCCESS,
    FD_RCODE_BAD_LENGTH,
    FD_RCODE_BAD_CRC,
} fd_rcode_t;

class I2CFrame {
private:
    uint8_t _id;
    uint8_t _cmd;
    uint8_t* _data;
    uint32_t _length;
public:
    I2CFrame();
    I2CFrame(const uint8_t id, const uint8_t cmd);
    void setId(const uint8_t id);
    uint8_t getId();
    void setCmd(const uint8_t cmd);
    uint8_t getCmd();
    bool setData(uint8_t* data, const uint32_t length);
    uint8_t* getData();
    void setLength(const uint32_t length);
    uint32_t getLength();
    uint32_t encode(uint8_t* const data, const uint32_t length);
    fd_rcode_t decode(uint8_t* const data, const uint32_t length);
    ~I2CFrame();
};

#endif /* I2C_FRAME_H_ */
