/*
 * EepromData.c
 *
 *  Created on: Jan 10, 2016
 *      Author: fcos
 */
#include "eeprom.h"

namespace i2c_hat {
namespace driver {

extern "C" {
uint16_t VirtAddVarTab[EEP_VIRT_ADR_COUNT];
}

void Eeprom::Init(void) {
    uint32_t i;
    for(i = 0; i < EEP_VIRT_ADR_COUNT; i++) {
        VirtAddVarTab[i] = i;
    }
    FLASH_Unlock();
    EE_Init();
}

bool Eeprom::Read(const uint16_t virtAddress, uint32_t& value) {
    uint16_t data_low, data_high;
    uint16_t status1, status2;

    status1 = EE_ReadVariable(virtAddress, &data_low);
    status2 = EE_ReadVariable(virtAddress + 1, &data_high);
    if( (status1 == 0) and (status2 == 0) ) {
        value = ((uint32_t)data_high << 16) + (uint32_t)data_low;
        return true;
    }
    return false;
}

bool Eeprom::Write(const uint16_t virtAddress, const uint32_t value) {
    uint16_t status1, status2;

    status1 = EE_WriteVariable(virtAddress, (uint16_t)value);
    status2 = EE_WriteVariable(virtAddress + 1, (uint16_t)(value >> 16));
    if( (status1 == FLASH_COMPLETE) and (status2 == FLASH_COMPLETE) ) {
        return true;
    }
    return false;
}

} /* namespace driver */
} /* namespace i2c_hat */
