/*
 * Commands.h
 *
 *  Created on: Apr 6, 2016
 *      Author: fcos
 */

#ifndef MODULE_COMMANDS_H_
#define MODULE_COMMANDS_H_

#define CMD_GET_BOARD_NAME                      0x10
#define CMD_GET_FIRMWARE_VERSION                0x11
#define CMD_GET_STATUS_WORD                     0x12
#define CMD_RESET                               0x13

#define CMD_CWDT_SET_PERIOD                     0x14
#define CMD_CWDT_GET_PERIOD                     0x15

#define CMD_DI_GET_ALL_CHANNEL_STATES           0x20
#define CMD_DI_GET_CHANNEL_STATE                0x21
#define CMD_DI_GET_COUNTER                      0x22
#define CMD_DI_RESET_COUNTER                    0x23
#define CMD_DI_RESET_ALL_COUNTERS               0x24

#define CMD_DO_SET_POWER_ON_VALUE               0x30
#define CMD_DO_GET_POWER_ON_VALUE               0x31
#define CMD_DO_SET_SAFETY_VALUE                 0x32
#define CMD_DO_GET_SAFETY_VALUE                 0x33
#define CMD_DO_SET_ALL_CHANNEL_STATES           0x34
#define CMD_DO_GET_ALL_CHANNEL_STATES           0x35
#define CMD_DO_SET_CHANNEL_STATE                0x36
#define CMD_DO_GET_CHANNEL_STATE                0x37

#endif /* MODULE_COMMANDS_H_ */
