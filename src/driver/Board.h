/*
 * Board.h
 *
 *  Created on: Mar 25, 2016
 *      Author: fcos
 */

#ifndef DRIVER_BOARD_H_
#define DRIVER_BOARD_H_

#include "BoardDef.h"
#include "I2CPort.h"
#include "DigitalOutputPin.h"
#include "DigitalInputPort.h"
#include "DigitalOutputPort.h"
#include "WatchDog.h"

namespace i2c_hat {

class Board {
private:
    uint8_t* _firmwareVersion;
    uint32_t _firmwareVersionSize;
    uint8_t* _name;
    uint32_t _nameSize;
    DigitalOutputPin *_statusLedPin;
    DigitalInputPort *_i2cLowNibbleAddressPort;
    I2CPort *_i2cPort;

    DigitalInputPort *_digitalInputPort;
    DigitalOutputPort *_digitalOutputPort;

    WatchDog *_watchdog;

    uint32_t _statusWord;

    void initPeripheralsClocks();
public:
    Board();
    uint8_t* getFirmwareVersion();
    uint32_t getFirmwareVersionSize();
    uint8_t* getName();
    uint32_t getNameSize();
    uint32_t getStatusWord();
    DigitalOutputPin* getStatusLedPin();
    DigitalInputPort* getI2CLowNibbleAddressPort();
    I2CPort* getI2CPort();
    DigitalInputPort* getDigitalInputPort();
    DigitalOutputPort* getDigitalOutputPort();
    WatchDog* getWatchDog();
};

} /* namespace i2c_hat */

#endif /* DRIVER_BOARD_H_ */
