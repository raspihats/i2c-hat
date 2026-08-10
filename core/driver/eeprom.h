/*
 * eeprom.h
 *
 *  Created on: Jan 10, 2016
 *      Author: fcos
 */

#ifndef DRIVER_EEPROM_H_
#define DRIVER_EEPROM_H_

// Included before the extern "C" block below: the emulation headers pull in
// board.h too, and board.h carries C++ module headers that must not get C
// linkage.
#include "board.h"
#include "stm32xx_ll.h"

// Two vendor middlewares, one per flash generation. The F0 keeps ST's AN4061
// halfword emulation; the G0 flash (64-bit-only programming, ECC) requires
// ST's X-CUBE-EEPROM EEPROM_Emul (middleware/eeprom_emul/).
#if defined(I2C_HAT_MCU_FAMILY_G0)
extern "C" {
#include "eeprom_emul.h"
}
#include "eeprom_emulation_conf.h"      // EEP_VIRT_ADR_* layout only
#else
extern "C" {
#include "eeprom_emulation.h"
}
#endif

// The EEP_VIRT_ADR_* layout lives in eeprom_emulation_conf.h, pulled in above via
// eeprom_emulation.h. It sits at file scope so the middleware's C sources can read
// the same definitions.

namespace i2c_hat {
namespace driver {

class Eeprom {
public:
    static void Init();
    static bool Read(const uint16_t address, uint32_t& value);
    static bool Write(const uint16_t address, const uint32_t value);
    static bool Format();
};

} /* namespace driver */
} /* namespace i2c_hat */

#endif /* DRIVER_EEPROM_H_ */
