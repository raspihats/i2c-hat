/*
 * EepromData.h
 *
 *  Created on: Jan 10, 2016
 *      Author: fcos
 */

#ifndef EEPROMDATA_H_
#define EEPROMDATA_H_

#include "stm32f0xx.h"
#include "eeprom.h"

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

void EepromData_init(void);
uint32_t EepromData_readUInt32(const uint16_t virtAddress, const uint32_t defaultValue);
void EepromData_writeUInt32(const uint16_t virtAddress, const uint32_t value);

#endif /* EEPROMDATA_H_ */
