/*
 * board.h
 *
 *  AI4dcv10 I2C-HAT: 4 analog channels measuring voltage/current/power via
 *  TI INA228 monitors on the I2C2 master bus. First G0-family board
 *  (STM32G031K6T), first consumer of the X-CUBE-EEPROM emulation middleware.
 *
 *  Hardware config comes from CubeMX (ai4dcv10.ioc): 64 MHz SYSCLK
 *  (HSI16 -> PLL), I2C1 slave PB6/PB7 to the Pi, I2C2 master PA11/PA12 to
 *  the INA228s, IWDG, status LED on PF2 (NRST repurposed as GPIO - needs the
 *  NRST_MODE option byte set accordingly), address jumpers on PA0-PA3.
 *
 *  The analog-inputs module (INA228 driver + module::AnalogInputs) is not
 *  written yet - this board currently builds as a core-only firmware
 *  (name/version/status/CWDT over the HAT protocol) to prove out the G0 port.
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "main.h"

#define FW_VERSION_SIZE                     (3)
#define FW_VERSION_MAJOR                    (0)
#define FW_VERSION_MINOR                    (1)
#define FW_VERSION_PATCH                    (0)

#define BOARD_NAME_SIZE                     (25)
#define BOARD_NAME                          "AI4dcv10 I2C-HAT"

/* PROVISIONAL: 0x40=di16ac, 0x50=dq*, 0x60=di6acdq6rly ranges are taken.
 * The .ioc carries 4 address jumpers (PA0-PA3), but 0x70 + offsets 8..15
 * lands in the I2C reserved block at 0x78-0x7F - either the base moves or
 * jumper ADR_3 goes unused. Decide before hardware. */
#define I2C_BASE_ADDRESS                    (0x70)

/* I2C1 kernel clock (PCLK = SYSCLK = 64 MHz): scales the core's 50 ms
 * SCL-low TIMEOUTA backstop (core/driver/i2c_port.cpp). The CubeMX TIMINGR
 * 0x10B17DB5 keeps the unified 375 ns SCLDEL at this clock. */
#define I2C_KERNEL_CLOCK_HZ                 (64000000)

#define STATUS_LED_PIN                      driver::DigitalOutputPin(ST_LED_GPIO_Port, ST_LED_Pin, true)

/* 4 x INA228 on the I2C2 bus; A1 strapped to GND, A0 spread over
 * GND/VS/SDA/SCL -> 7-bit addresses 0x40..0x43 (INA228 datasheet table). */
#define ANALOG_INPUT_CHANNEL_COUNT          (4)
#define ANALOG_INPUT_INA228_ADDRESSES       { 0x40, 0x41, 0x42, 0x43 }

/* First flash page handed to the X-CUBE-EEPROM middleware: last 4K of the
 * 32K flash (2 pages of 2K), matching STM32G031K6_FLASH.ld which stops the
 * image at 28K. (CubeMX's STM32G031xx_FLASH.ld covers the full 32K and is
 * NOT the build's linker script.) */
#define EEPROM_EMUL_START_PAGE_ADDRESS      (0x08007000U)

/* ---- shared-core module wiring (consumed by core/i2c_hat.{h,cpp}) ---- */
#ifdef __cplusplus
#include "stm32g0xx_ll_rcc.h"

/* TODO: module::AnalogInputs (INA228) once the module exists in core/. */
#define BOARD_MODULE_MEMBERS
#define BOARD_REGISTER_MODULES
#define BOARD_STATUS_BITS
#endif /* __cplusplus */

#endif /* BOARD_H_ */
