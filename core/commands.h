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
    // CiA 301 0x1011: acts only with the "load" signature (payload
    // 'l','o','a','d'); formats the EEPROM pages and resets - every
    // persistent register falls back to its factory default
    RESTORE_FACTORY_DEFAULTS            = 0x18,

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
    // CiA 401 alignment (see ROADMAP.md): 0x6003 filter constant input
    // (per-channel, ms), 0x6002 change polarity input
    DI_SET_CHANNEL_FILTER               = 0x2A,
    DI_GET_CHANNEL_FILTER               = 0x2B,
    DI_SET_POLARITY                     = 0x2C,
    DI_GET_POLARITY                     = 0x2D,

    DO_SET_POWER_ON_VALUE               = 0x30,
    DO_GET_POWER_ON_VALUE               = 0x31,
    DO_SET_SAFETY_VALUE                 = 0x32,
    DO_GET_SAFETY_VALUE                 = 0x33,
    DO_SET_VALUE                        = 0x34,
    DO_GET_VALUE                        = 0x35,
    DO_SET_CHANNEL_STATE                = 0x36,
    DO_GET_CHANNEL_STATE                = 0x37,
    // CiA 401 alignment (see ROADMAP.md): 0x6202 change polarity output,
    // 0x6206 error mode output (per-channel safety mask)
    DO_SET_POLARITY                     = 0x38,
    DO_GET_POLARITY                     = 0x39,
    DO_SET_SAFETY_MASK                  = 0x3A,
    DO_GET_SAFETY_MASK                  = 0x3B,

    AI_GET_CHANNEL_VOLTAGE              = 0x40,
    AI_GET_CHANNEL_CURRENT              = 0x41,
    AI_GET_CHANNEL_TEMPERATURE          = 0x42,
    AI_SET_CHANNEL_SENSOR_TYPE          = 0x43,
    AI_GET_CHANNEL_SENSOR_TYPE          = 0x44,
    AI_SET_CHANNEL_GAIN                 = 0x45,
    AI_GET_CHANNEL_GAIN                 = 0x46,
    AI_SET_CHANNEL_CALIB                = 0x47,
    AI_GET_CHANNEL_CALIB                = 0x48,

};


enum class IRQReg {
    DI_FALLING_EDGE_CONTROL             = 0x20,
    DI_RISING_EDGE_CONTROL              = 0x21,
    DI_CAPTURE                          = 0x22, //interface to capture queue
};

#endif /* COMMANDS_H_ */
