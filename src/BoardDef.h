/*
 * BoardDef.h
 *
 *  Created on: Dec 26, 2015
 *      Author: fcos
 */

#ifndef BOARDDEF_H_
#define BOARDDEF_H_

#include "stdint.h"
#include "stm32f0xx.h"

typedef struct {
    GPIO_TypeDef*   port;
    uint16_t        pin;
} gpio_t;

typedef struct {
    gpio_t gpio;
    uint16_t pinSource;
    uint8_t alternateFunction;
} gpioaf_t;

typedef struct {
    I2C_TypeDef* port;
    gpioaf_t pins[2];
    uint8_t irqChannel;
    DMA_Channel_TypeDef* dmaTxChannel;
    DMA_Channel_TypeDef* dmaRxChannel;
} i2c_port_t;

#define FW_VERSION_MAJOR                        (1)
#define FW_VERSION_MINOR                        (0)
#define FW_VERSION_PATCH                        (0)
#define FW_VERSION_SIZE                         (3)

#define BOARD_NAME_SIZE                         (25)

#define DI16

#ifdef DI16
    #define BOARD_NAME                          "Di16 I2C-HAT"

    #define I2C_ADDRESS_HIGH_NIBBLE             (0x04)

    #define AHB_PERIPHERALS \
            RCC_AHBPeriph_GPIOA, \
            RCC_AHBPeriph_GPIOB, \
            RCC_AHBPeriph_DMA1

    #define APB1_PERIPHERALS \
            RCC_APB1Periph_I2C1

    #define I2C_PORT \
            I2C1, \
            { { { GPIOB, GPIO_Pin_6 }, GPIO_PinSource6, GPIO_AF_1 }, \
            { { GPIOB, GPIO_Pin_7 }, GPIO_PinSource7, GPIO_AF_1 } }, \
            I2C1_IRQn, \
            DMA1_Channel2, \
            DMA1_Channel3

    #define I2C_ADDRESS_LOW_NIBBLE_GPIOS \
            { GPIOA, GPIO_Pin_0 }, \
            { GPIOA, GPIO_Pin_1 }, \
            { GPIOA, GPIO_Pin_2 }, \
            { GPIOA, GPIO_Pin_3 }

    #define STATUS_LED_GPIO                     { GPIOB, GPIO_Pin_5 }

    #define DIGITAL_INPUT_CHANNEL_COUNT         (16)

    #define DIGITAL_INPUT_CHANNELS_GPIOS \
            { GPIOA, GPIO_Pin_4 }, \
            { GPIOA, GPIO_Pin_5 }, \
            { GPIOA, GPIO_Pin_6 }, \
            { GPIOA, GPIO_Pin_7 }, \
            { GPIOB, GPIO_Pin_0 }, \
            { GPIOB, GPIO_Pin_1 }, \
            { GPIOA, GPIO_Pin_8 }, \
            { GPIOA, GPIO_Pin_9 }, \
            { GPIOA, GPIO_Pin_10 }, \
            { GPIOA, GPIO_Pin_11 }, \
            { GPIOA, GPIO_Pin_12 }, \
            { GPIOA, GPIO_Pin_13 }, \
            { GPIOA, GPIO_Pin_14 }, \
            { GPIOA, GPIO_Pin_15 }, \
            { GPIOB, GPIO_Pin_3 }, \
            { GPIOB, GPIO_Pin_4 }

#endif

#ifdef RLY10
    #define BOARD_NAME                          "Rly10 I2C-HAT"

    #define I2C_ADDRESS_HIGH_NIBBLE             (0x05)

    #define AHB_PERIPHERALS \
            RCC_AHBPeriph_GPIOA, \
            RCC_AHBPeriph_GPIOB, \
            RCC_AHBPeriph_DMA1

    #define APB1_PERIPHERALS \
            RCC_APB1Periph_I2C1

    #define I2C_PORT \
            I2C1, \
            { { { GPIOB, GPIO_Pin_6 }, GPIO_PinSource6, GPIO_AF_1 }, \
            { { GPIOB, GPIO_Pin_7 }, GPIO_PinSource7, GPIO_AF_1 } }, \
            I2C1_IRQn, \
            DMA1_Channel2, \
            DMA1_Channel3

    #define I2C_ADDRESS_LOW_NIBBLE_GPIOS \
            { GPIOA, GPIO_Pin_0 }, \
            { GPIOA, GPIO_Pin_1 }, \
            { GPIOA, GPIO_Pin_2 }, \
            { GPIOA, GPIO_Pin_3 }

    #define STATUS_LED_GPIO                     { GPIOB, GPIO_Pin_5 }

    #define DIGITAL_OUTPUT_CHANNEL_COUNT        (10)

    #define DIGITAL_OUTPUT_CHANNELS_GPIOS \
            { GPIOA, GPIO_Pin_5 }, \
            { GPIOA, GPIO_Pin_6 }, \
            { GPIOA, GPIO_Pin_7 }, \
            { GPIOB, GPIO_Pin_0 }, \
            { GPIOB, GPIO_Pin_1 }, \
            { GPIOA, GPIO_Pin_8 }, \
            { GPIOA, GPIO_Pin_9 }, \
            { GPIOA, GPIO_Pin_10 }, \
            { GPIOA, GPIO_Pin_11 }, \
            { GPIOA, GPIO_Pin_12 }

#endif

#ifdef DI6RLY6
    #define BOARD_NAME                          "Di6Rly6 I2C-HAT"

    #define I2C_ADDRESS_HIGH_NIBBLE             (0x06)

    #define AHB_PERIPHERALS \
            RCC_AHBPeriph_GPIOA, \
            RCC_AHBPeriph_GPIOB, \
            RCC_AHBPeriph_DMA1

    #define APB1_PERIPHERALS \
            RCC_APB1Periph_I2C1

    #define I2C_PORT \
            I2C1, \
            { { { GPIOB, GPIO_Pin_6 }, GPIO_PinSource6, GPIO_AF_1 }, \
            { { GPIOB, GPIO_Pin_7 }, GPIO_PinSource7, GPIO_AF_1 } }, \
            I2C1_IRQn, \
            DMA1_Channel2, \
            DMA1_Channel3

    #define I2C_ADDRESS_LOW_NIBBLE_GPIOS \
            { GPIOA, GPIO_Pin_0 }, \
            { GPIOA, GPIO_Pin_1 }, \
            { GPIOA, GPIO_Pin_2 }, \
            { GPIOA, GPIO_Pin_3 }

    #define STATUS_LED_GPIO                     { GPIOB, GPIO_Pin_5 }

    #define DIGITAL_INPUT_CHANNEL_COUNT         (6)

    #define DIGITAL_INPUT_CHANNELS_GPIOS \
            { GPIOA, GPIO_Pin_4 }, \
            { GPIOA, GPIO_Pin_5 }, \
            { GPIOA, GPIO_Pin_6 }, \
            { GPIOA, GPIO_Pin_7 }, \
            { GPIOB, GPIO_Pin_0 }, \
            { GPIOB, GPIO_Pin_1 }

    #define DIGITAL_OUTPUT_CHANNEL_COUNT        (6)

    #define DIGITAL_OUTPUT_CHANNELS_GPIOS \
            { GPIOA, GPIO_Pin_8 }, \
            { GPIOA, GPIO_Pin_9 }, \
            { GPIOA, GPIO_Pin_10 }, \
            { GPIOA, GPIO_Pin_11 }, \
            { GPIOA, GPIO_Pin_12 }, \
            { GPIOA, GPIO_Pin_15 }

#endif

#endif /* BOARDDEF_H_ */
