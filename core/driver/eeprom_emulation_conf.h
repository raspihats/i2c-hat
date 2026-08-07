/*
 * eeprom_emulation_conf.h
 *
 *  Application configuration for the ST EEPROM emulation middleware.
 *
 *  middleware/eeprom/eeprom_emulation.h includes this by name; the application
 *  supplies it on the include path (the usual vendor-config arrangement, as with
 *  stm32f0xx_hal_conf.h). That keeps the middleware free of any knowledge about
 *  which IO a board has, while still letting the layout below drive it.
 *
 *  Kept C-compatible: the middleware's C sources include it too.
 */

#ifndef EEPROM_EMULATION_CONF_H_
#define EEPROM_EMULATION_CONF_H_

#include "board.h"

/* Virtual addresses, one per 16-bit half of each stored 32-bit variable.
 *
 * Allocated per feature: a board only reserves slots for the IO it actually has.
 * The power-on and safety values belong to the digital outputs, so they exist
 * only on boards whose board.h declares them.
 *
 * Append new entries at the end. The values are baked into flash on deployed
 * boards, so reordering or inserting in the middle would misread stored data. */
enum {
    EEP_VIRT_ADR_CWDT_PERIOD_LOW,
    EEP_VIRT_ADR_CWDT_PERIOD_HIGH,
#ifdef DIGITAL_OUTPUT_CHANNEL_COUNT
    EEP_VIRT_ADR_DO_POWER_ON_VALUE_LOW,
    EEP_VIRT_ADR_DO_POWER_ON_VALUE_HIGH,
    EEP_VIRT_ADR_DO_SAFETY_VALUE_LOW,
    EEP_VIRT_ADR_DO_SAFETY_VALUE_HIGH,
#endif
    EEP_VIRT_ADR_COUNT,
};

#define EEP_VIRT_ADR_CWDT_PERIOD            EEP_VIRT_ADR_CWDT_PERIOD_LOW
#ifdef DIGITAL_OUTPUT_CHANNEL_COUNT
#define EEP_VIRT_ADR_DO_POWER_ON_VALUE      EEP_VIRT_ADR_DO_POWER_ON_VALUE_LOW
#define EEP_VIRT_ADR_DO_SAFETY_VALUE        EEP_VIRT_ADR_DO_SAFETY_VALUE_LOW
#endif

/* How many entries of VirtAddVarTab (core/driver/eeprom.cpp) the middleware walks
 * on a page transfer. It is the table's length by definition, so the two cannot
 * drift apart. */
#define EEP_NB_OF_VAR                       EEP_VIRT_ADR_COUNT

#endif /* EEPROM_EMULATION_CONF_H_ */
