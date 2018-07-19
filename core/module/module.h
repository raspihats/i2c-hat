/*
 * module.h
 *
 *  Created on: Feb 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_MODULE_H_
#define MODULE_MODULE_H_

#include "../cooperative_os/task.h"
#include "../frame/frame.h"
#include "../commands.h"
#include "../board.h"

#define BYTES_TO_UINT32(__BYTES__, __UINT32__)  (__UINT32__ = __BYTES__[0] + (__BYTES__[1] << 8) + (__BYTES__[2] << 16) + (__BYTES__[3] << 24))
#define UINT32_TO_BYTES(__UINT32__, __BYTES__)  { \
    __BYTES__[0] = (uint8_t)__UINT32__; \
    __BYTES__[1] = (uint8_t)(__UINT32__>> 8); \
    __BYTES__[2] = (uint8_t)(__UINT32__>> 16); \
    __BYTES__[3] = (uint8_t)(__UINT32__>> 24); \
}

#define BYTES_TO_FLOAT(__BYTES__, __FLOAT__)  { \
    ((uint8_t*)(&(__FLOAT__)))[0] = (__BYTES__)[0]; \
    ((uint8_t*)(&(__FLOAT__)))[1] = (__BYTES__)[1]; \
    ((uint8_t*)(&(__FLOAT__)))[2] = (__BYTES__)[2]; \
    ((uint8_t*)(&(__FLOAT__)))[3] = (__BYTES__)[3]; \
}

#define FLOAT_TO_BYTES(__FLOAT__, __BYTES__)  { \
    (__BYTES__)[0] = ((uint8_t*)(&(__FLOAT__)))[0]; \
    (__BYTES__)[1] = ((uint8_t*)(&(__FLOAT__)))[1]; \
    (__BYTES__)[2] = ((uint8_t*)(&(__FLOAT__)))[2]; \
    (__BYTES__)[3] = ((uint8_t*)(&(__FLOAT__)))[3]; \
}

namespace i2c_hat {
namespace module {

class Module : public cooperative_os::Task {
public:
    enum Event {
        EVENT_CWDT_DISABLED,
        EVENT_CWDT_MONITORING,
        EVENT_CWDT_TIMEOUT,
    };
    Module(const uint32_t delay, const uint32_t period);
    virtual void Init()=0;
    virtual bool ProcessRequest(Frame& request, Frame& response)=0;
};

} /* namespace module */
} /* namespace i2c_hat */

#endif /* MODULE_MODULE_H_ */
