/*
 * eeprom.h
 *
 *  Created on: Jan 10, 2016
 *      Author: fcos
 */

#ifndef DRIVER_EEPROM_H_
#define DRIVER_EEPROM_H_

extern "C" {
#include "eeprom_emulation.h"
}

namespace i2c_hat {
namespace driver {

enum {
    EEP_VIRT_ADR_CWDT_PERIOD_LOW,
    EEP_VIRT_ADR_CWDT_PERIOD_HIGH,
    EEP_VIRT_ADR_DO_POWER_ON_VALUE_LOW,
    EEP_VIRT_ADR_DO_POWER_ON_VALUE_HIGH,
    EEP_VIRT_ADR_DO_SAFETY_VALUE_LOW,
    EEP_VIRT_ADR_DO_SAFETY_VALUE_HIGH,
    EEP_VIRT_ADR_COUNT,
};

#define EEP_VIRT_ADR_CWDT_PERIOD            EEP_VIRT_ADR_CWDT_PERIOD_LOW
#define EEP_VIRT_ADR_DO_POWER_ON_VALUE      EEP_VIRT_ADR_DO_POWER_ON_VALUE_LOW
#define EEP_VIRT_ADR_DO_SAFETY_VALUE        EEP_VIRT_ADR_DO_SAFETY_VALUE_LOW

class Eeprom {
public:
    static void Init();
    static bool Read(const uint16_t address, uint32_t& value);
    static bool Write(const uint16_t address, const uint32_t value);
};

} /* namespace driver */
} /* namespace i2c_hat */

#endif /* DRIVER_EEPROM_H_ */
