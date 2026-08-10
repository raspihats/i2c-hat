/*
 * EepromData.c
 *
 *  Created on: Jan 10, 2016
 *      Author: fcos
 */
#include "eeprom.h"

namespace i2c_hat {
namespace driver {

#if defined(I2C_HAT_MCU_FAMILY_G0)

/* X-CUBE-EEPROM (middleware/eeprom_emul/): 1-based virtual addresses, one
 * write-once 64-bit flash element per stored 32-bit value. Each variable
 * lives at <EEP_VIRT_ADR_*_LOW> + 1; the _HIGH slot of the legacy F0 pair
 * layout is never written on this family. */

void Eeprom::Init(void) {
    HAL_FLASH_Unlock();
    EE_Init(EE_CONDITIONAL_ERASE);
}

bool Eeprom::Read(const uint16_t virtAddress, uint32_t& value) {
    return EE_ReadVariable32bits(virtAddress + 1U, &value) == EE_OK;
}

/**
  * @brief  Erases the whole emulated EEPROM area, CiA 301 0x1011.
  *         Every stored variable is gone; on the next boot each module's
  *         Read() fails and its factory default stands. Blocks for the page
  *         erases - the caller is expected to reset right after.
  * @retval true on success
  */
bool Eeprom::Format() {
    return EE_Format(EE_FORCED_ERASE) == EE_OK;
}

bool Eeprom::Write(const uint16_t virtAddress, const uint32_t value) {
    uint32_t current;
    EE_Status status;

    // Same skip-if-unchanged guard as the F0 path below: controllers re-send
    // the same configuration on every start, and each redundant write burns
    // a flash element.
    if(Read(virtAddress, current) and (current == value)) {
        return true;
    }

    status = EE_WriteVariable32bits(virtAddress + 1U, value);
    if((status & EE_STATUSMASK_CLEANUP) != 0U) {
        // The write landed but a page ran full: reclaim the old page now.
        // One blocking page erase, the same order of stall as the old F0
        // page transfer. Candidate for the idle task once the cooperative
        // scheduler grows a low-priority slot for it.
        return EE_CleanUp() == EE_OK;
    }
    return status == EE_OK;
}

#else /* I2C_HAT_MCU_FAMILY_F0: ST AN4061 halfword emulation */

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

/**
  * @brief  Erases the whole emulated EEPROM (both pages), CiA 301 0x1011.
  *         Every stored variable is gone; on the next boot each module's
  *         Read() fails and its factory default stands. Blocks for two page
  *         erases (~40-80 ms) - the caller is expected to reset right after.
  * @retval true on success
  */
bool Eeprom::Format() {
    return EE_Format() == FLASH_COMPLETE;
}

bool Eeprom::Write(const uint16_t virtAddress, const uint32_t value) {
    uint16_t status1, status2;
    uint32_t current;

    // Skip the flash program entirely when the stored value already matches --
    // controllers re-send the same configuration on every start, and each
    // redundant write burns a page slot, eventually forcing a ~20-40 ms page
    // transfer that stalls the core (and stretches SCL) mid-transaction.
    // A failed read (first ever write, torn value) falls through to the write.
    if(Read(virtAddress, current) and (current == value)) {
        return true;
    }

    status1 = EE_WriteVariable(virtAddress, (uint16_t)value);
    status2 = EE_WriteVariable(virtAddress + 1, (uint16_t)(value >> 16));
    if( (status1 == FLASH_COMPLETE) and (status2 == FLASH_COMPLETE) ) {
        return true;
    }
    return false;
}

#endif /* I2C_HAT_MCU_FAMILY_* */

} /* namespace driver */
} /* namespace i2c_hat */
