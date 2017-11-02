/*
 * Frame.h
 *
 *  Created on: Oct 12, 2017
 *      Author: fcos
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <stdint.h>
#include <stdio.h>

namespace i2c_hat {

typedef enum {
    FD_RCODE_SUCCESS,
    FD_RCODE_BAD_LENGTH,
    FD_RCODE_BAD_CRC,
} fd_rcode_t;

class Frame {
public:
    Frame();
    uint8_t id();
    void set_id(const uint8_t id);
    uint8_t command();
    void set_command(const uint8_t cmd);
    const uint8_t* payload();
    uint32_t payload_size();
    void set_payload(const uint8_t* data, const uint32_t size);
    uint32_t Encode(uint8_t* const buffer, const uint32_t size);
    fd_rcode_t Decode(uint8_t* const buffer, const uint32_t size);
private:
    uint8_t id_;
    uint8_t cmd_;
    const uint8_t* payload_;
    uint32_t payload_size_;
};

} /* namespace i2c_hat */

#endif /* FRAME_H_ */
