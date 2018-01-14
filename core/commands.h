/*
 * Commands.h
 *
 *  Created on: Apr 6, 2016
 *      Author: fcos
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

enum class Command {
    GET_BOARD_NAME                      = 0x10,
    GET_FIRMWARE_VERSION                = 0x11,
    GET_STATUS_WORD                     = 0x12,
    RESET                               = 0x13,

    CWDT_SET_PERIOD                     = 0x14,
    CWDT_GET_PERIOD                     = 0x15,
    IRQ_GET_REG                         = 0x16,
    IRQ_SET_REG                         = 0x17,

    DI_GET_VALUE                        = 0x20,
    DI_GET_CHANNEL_STATE                = 0x21,
    DI_GET_COUNTER                      = 0x22,
    DI_RESET_COUNTER                    = 0x23,
    DI_RESET_ALL_COUNTERS               = 0x24,
    DI_GET_COUNTERS_STATUS              = 0x25,
    DI_GET_ENCODER                      = 0x26,
    DI_RESET_ENCODER                    = 0x27,
    DI_RESET_ALL_ENCODERS               = 0x28,
    DI_GET_ENCODERS_STATUS              = 0x29,

    DO_SET_POWER_ON_VALUE               = 0x30,
    DO_GET_POWER_ON_VALUE               = 0x31,
    DO_SET_SAFETY_VALUE                 = 0x32,
    DO_GET_SAFETY_VALUE                 = 0x33,
    DO_SET_VALUE                        = 0x34,
    DO_GET_VALUE                        = 0x35,
    DO_SET_CHANNEL_STATE                = 0x36,
    DO_GET_CHANNEL_STATE                = 0x37,
};


#endif /* COMMANDS_H_ */
