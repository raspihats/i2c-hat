/*
 * Board.cpp
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#include <stddef.h>
#include "Board.h"
extern "C" {
#include "EepromData.h"
}

#define I2C_BUFFER_SIZE                         (256)

namespace i2c_hat {

static const uint8_t firmwareVersion[FW_VERSION_SIZE] = {FW_VERSION_MAJOR, FW_VERSION_MINOR, FW_VERSION_PATCH};
static const char boardName[BOARD_NAME_SIZE] = BOARD_NAME;

static gpio_t statusLedGpio = STATUS_LED_GPIO;
static gpio_t i2cLowNibbleAddressGpios[] = {I2C_ADDRESS_LOW_NIBBLE_GPIOS};

static i2c_port_t i2cPort = {I2C_PORT};

#ifdef DIGITAL_INPUT_CHANNEL_COUNT
static gpio_t digitalInputsGpios[DIGITAL_INPUT_CHANNEL_COUNT] = {DIGITAL_INPUT_CHANNELS_GPIOS};
#endif

#ifdef DIGITAL_OUTPUT_CHANNEL_COUNT
static gpio_t digitalOutputsGpios[DIGITAL_OUTPUT_CHANNEL_COUNT] = {DIGITAL_OUTPUT_CHANNELS_GPIOS};
#endif

Board::Board() {
    uint8_t address;

    initPeripheralsClocks();
    EepromData_init();

    _firmwareVersion = (uint8_t *)firmwareVersion;
    _firmwareVersionSize = sizeof(firmwareVersion);

    _name = (uint8_t*)boardName;
    _nameSize = sizeof(boardName);

    _watchdog = new WatchDog();

    _statusLedPin = new DigitalOutputPin(&statusLedGpio, true);
    _i2cLowNibbleAddressPort = new DigitalInputPort(i2cLowNibbleAddressGpios, 4, GPIO_PuPd_UP, true);

    address = (I2C_ADDRESS_HIGH_NIBBLE << 4) | _i2cLowNibbleAddressPort->read();
    _i2cPort = new I2CPort(&i2cPort, I2C_BUFFER_SIZE, address);

#ifdef DIGITAL_INPUT_CHANNEL_COUNT
    _digitalInputPort = new DigitalInputPort(digitalInputsGpios, DIGITAL_INPUT_CHANNEL_COUNT, GPIO_PuPd_NOPULL, true);
#endif

#ifdef DIGITAL_OUTPUT_CHANNEL_COUNT
    _digitalOutputPort = new DigitalOutputPort(digitalOutputsGpios, DIGITAL_OUTPUT_CHANNEL_COUNT);
#endif
}

void Board::initPeripheralsClocks() {
    uint32_t i;

#ifdef AHB_PERIPHERALS
    uint32_t ahbPeripherals[] = { AHB_PERIPHERALS };
    for(i = 0; i < sizeof(ahbPeripherals) >> 2; i++) {
        RCC_AHBPeriphClockCmd(ahbPeripherals[i], ENABLE);
    }
#endif
#ifdef APB1_PERIPHERALS
    uint32_t apb1Peripherals[] = { APB1_PERIPHERALS };
    for(i = 0; i < sizeof(apb1Peripherals) >> 2; i++) {
        RCC_APB1PeriphClockCmd(apb1Peripherals[i], ENABLE);
    }
#endif
#ifdef APB2_PERIPHERALS
    uint32_t apb2Peripherals[] = { APB2_PERIPHERALS };
    for(uint32_t i = 0; i < sizeof(apb2Peripherals) >> 2; i++) {
        RCC_APB2PeriphClockCmd(apb2Peripherals[i], ENABLE);
    }
#endif
}

uint8_t* Board::getFirmwareVersion() {
    return _firmwareVersion;
}

uint32_t Board::getFirmwareVersionSize() {
    return _firmwareVersionSize;
}

uint8_t* Board::getName() {
    return _name;
}

uint32_t Board::getNameSize() {
    return _nameSize;
}

uint32_t Board::getStatusWord() {
    uint32_t statusWord;

    statusWord = 0;
    if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET) statusWord |= 0x01;
    if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET) statusWord |= 0x02;
    if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) statusWord |= 0x04;
    RCC_ClearFlag();
    return statusWord;
}

DigitalOutputPin* Board::getStatusLedPin() {
    return _statusLedPin;
}

DigitalInputPort* Board::getI2CLowNibbleAddressPort() {
    return _i2cLowNibbleAddressPort;
}

I2CPort* Board::getI2CPort() {
    return _i2cPort;
}

DigitalInputPort* Board::getDigitalInputPort() {
    return _digitalInputPort;
}

DigitalOutputPort* Board::getDigitalOutputPort() {
    return _digitalOutputPort;
}

WatchDog* Board::getWatchDog() {
    return _watchdog;
}

} /* namespace i2c_hat */
