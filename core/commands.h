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
    // acts only with the "boot" signature (payload 'b','o','o','t');
    // resets into the ROM system bootloader - the board re-enumerates at
    // the ROM's I2C address on the same bus (AN2606: 0x3E on the F04x
    // boards, 0x56 on G0). Jumperless flashing; a full bus scan (or
    // stm32flash -g 0) returns the bootloader to the application.
    ENTER_BOOTLOADER                    = 0x19,
    // CiA 301 0x1020: configuration signature. The controller stores any
    // non-zero token after a successful reconcile; the firmware voids it
    // (back to 0) whenever any OTHER persistent value actually changes, so
    // a surviving signature proves the whole configuration is untouched.
    // Unchanged re-writes keep it (the EEPROM skip-if-unchanged guard runs
    // first). 0 = no claim; factory restore wipes it with everything else.
    CONFIG_SET_SIGNATURE                = 0x1A,
    CONFIG_GET_SIGNATURE                = 0x1B,

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
    // CiA 401 0x6208 filter mask output: gates BULK writes (0x34) only -
    // a masked-out channel keeps its state whatever the bulk frame says.
    // Single-channel writes (0x36) bypass it (the other owner's door),
    // and the CWDT trip / power-on paths are governed solely by 0x6206 /
    // the power-on value. VOLATILE by design (DS401 practice): all-ones
    // after every reset, so the controller reclaims the whole port at a
    // known baseline.
    DO_SET_WRITE_MASK                   = 0x3C,
    DO_GET_WRITE_MASK                   = 0x3D,

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
    DI_FALLING_EDGE_CONTROL             = 0x20, // CiA 401 0x6008, persistent
    DI_RISING_EDGE_CONTROL              = 0x21, // CiA 401 0x6007, persistent
    DI_CAPTURE                          = 0x22, //interface to capture queue
    // CiA 401 0x6005 global interrupt enable: volatile arming bit, 0 after
    // every reset. Writing 0 (or a CWDT timeout) disarms the block - the
    // capture queue is dumped and the IRQ line released - while the
    // persistent edge masks above keep the commissioning. 0x6006 (any
    // change) is not implemented: it is rising|falling by definition.
    DI_GLOBAL_ENABLE                    = 0x23,
};

#endif /* COMMANDS_H_ */
