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
    /* CiA 401 alignment (ROADMAP.md): 0x6202 polarity, 0x6206 safety mask.
       Appended per the rule above; on boards upgraded from older firmware
       these read back as not-found and the module falls back to defaults
       (polarity 0, mask all channels) - the pre-mask behavior. */
    EEP_VIRT_ADR_DO_POLARITY_LOW,
    EEP_VIRT_ADR_DO_POLARITY_HIGH,
    EEP_VIRT_ADR_DO_SAFETY_MASK_LOW,
    EEP_VIRT_ADR_DO_SAFETY_MASK_HIGH,
#endif
#ifdef DIGITAL_INPUT_CHANNEL_COUNT
    /* CiA 401 alignment (ROADMAP.md): 0x6002 input polarity, 0x6003 filter
       constant - one u32 (ms) per input channel, allocated as a block. */
    EEP_VIRT_ADR_DI_POLARITY_LOW,
    EEP_VIRT_ADR_DI_POLARITY_HIGH,
    EEP_VIRT_ADR_DI_FILTER_BASE,
    EEP_VIRT_ADR_DI_FILTER_LAST = EEP_VIRT_ADR_DI_FILTER_BASE
                                  + 2 * DIGITAL_INPUT_CHANNEL_COUNT - 1,
#endif
    EEP_VIRT_ADR_COUNT,
};

#define EEP_VIRT_ADR_CWDT_PERIOD            EEP_VIRT_ADR_CWDT_PERIOD_LOW
#ifdef DIGITAL_OUTPUT_CHANNEL_COUNT
#define EEP_VIRT_ADR_DO_POWER_ON_VALUE      EEP_VIRT_ADR_DO_POWER_ON_VALUE_LOW
#define EEP_VIRT_ADR_DO_SAFETY_VALUE        EEP_VIRT_ADR_DO_SAFETY_VALUE_LOW
#define EEP_VIRT_ADR_DO_POLARITY            EEP_VIRT_ADR_DO_POLARITY_LOW
#define EEP_VIRT_ADR_DO_SAFETY_MASK         EEP_VIRT_ADR_DO_SAFETY_MASK_LOW
#endif
#ifdef DIGITAL_INPUT_CHANNEL_COUNT
#define EEP_VIRT_ADR_DI_POLARITY            EEP_VIRT_ADR_DI_POLARITY_LOW
#define EEP_VIRT_ADR_DI_FILTER(channel)     (EEP_VIRT_ADR_DI_FILTER_BASE + 2 * (channel))
#endif

/* How many entries of VirtAddVarTab (core/driver/eeprom.cpp) the middleware walks
 * on a page transfer. It is the table's length by definition, so the two cannot
 * drift apart. */
#define EEP_NB_OF_VAR                       EEP_VIRT_ADR_COUNT

#endif /* EEPROM_EMULATION_CONF_H_ */
