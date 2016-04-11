/*
 * EepromData.c
 *
 *  Created on: Jan 10, 2016
 *      Author: fcos
 */
#include "EepromData.h"

uint16_t VirtAddVarTab[EEP_VIRT_ADR_COUNT];

void EepromData_init(void) {
    uint32_t i;
    for(i = 0; i < EEP_VIRT_ADR_COUNT; i++) {
        VirtAddVarTab[i] = i;
    }
    FLASH_Unlock();
    EE_Init();
}

uint32_t EepromData_readUInt32(const uint16_t virtAddress, const uint32_t defaultValue) {
    uint16_t dataLow;
    uint16_t dataHigh;
    uint16_t status1, status2;
    uint32_t data;

    status1 = EE_ReadVariable(virtAddress, &dataLow);
    status2 = EE_ReadVariable(virtAddress + 1, &dataHigh);
    if( (status1 == 0) && (status2 == 0) ) {
        data = ((uint32_t)dataHigh << 16) + (uint32_t)dataLow;
    }
    else {
        if( (status1 == NO_VALID_PAGE) || (status2 == NO_VALID_PAGE) ) {
            //TODO handle EEPROM NO_VALID_PAGE
        }
        data = defaultValue;
    }
    return data;
}

void EepromData_writeUInt32(const uint16_t virtAddress, const uint32_t value) {
    uint16_t status1, status2;

    status1 = EE_WriteVariable(virtAddress, (uint16_t)value);
    status2 = EE_WriteVariable(virtAddress + 1, (uint16_t)(value >> 16));
    if( (status1 == 0) && (status2 == 0) ) {

    }
    else {
        //TODO handle EEPROM write FAILED
    }
}
