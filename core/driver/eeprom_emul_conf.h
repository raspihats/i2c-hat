/*
 * eeprom_emul_conf.h
 *
 *  Application configuration for ST's X-CUBE-EEPROM emulation middleware
 *  (middleware/eeprom_emul/, G0-family boards).
 *
 *  eeprom_emul.h includes this by bare name, the same vendor-config
 *  arrangement as eeprom_emulation_conf.h for the F0 middleware. The virtual
 *  address layout is NOT duplicated here: it comes from
 *  eeprom_emulation_conf.h, which stays the single owner of the EEP_VIRT_ADR_*
 *  enum for both families.
 *
 *  Address mapping on this middleware: virtual address 0 is reserved (erased
 *  element), valid addresses are 1..NB_OF_VARIABLES, and one address holds a
 *  full 32-bit value. The Eeprom driver therefore stores each variable at
 *  <EEP_VIRT_ADR_*_LOW> + 1 and the _HIGH slots of the legacy pairs are simply
 *  never written.
 */

#ifndef EEPROM_EMUL_CONF_H_
#define EEPROM_EMUL_CONF_H_

#include "eeprom_emulation_conf.h"

/* Flash area used for emulation. The board's linker script must exclude the
 * same pages (see the board's *_FLASH.ld). PAGES_NUMBER in eeprom_emul.h
 * derives the page count: with fewer than ~250 variables and no guard pages
 * that is 2 pages, i.e. 4 KB on the G0's 2 KB pages. */
#ifndef EEPROM_EMUL_START_PAGE_ADDRESS
#error "board.h must define EEPROM_EMUL_START_PAGE_ADDRESS (first flash page reserved for EEPROM emulation)"
#endif
#define START_PAGE_ADDRESS      EEPROM_EMUL_START_PAGE_ADDRESS
#define CYCLES_NUMBER           1U  /* 1 x 10K erase cycles - config writes are rare
                                       (and the driver skips unchanged values) */
#define GUARD_PAGES_NUMBER      0U  /* no guard pages on 32K-flash parts */

/* CRC16 over each stored element, computed by the hardware CRC unit. */
#define CRC_POLYNOMIAL_LENGTH   LL_CRC_POLYLENGTH_16B
#define CRC_POLYNOMIAL_VALUE    0x8005U

/* Valid virtual addresses are 1..NB_OF_VARIABLES; the highest address the
 * driver uses is <last _LOW slot> + 1 <= EEP_VIRT_ADR_COUNT. */
#define NB_OF_VARIABLES         ((uint16_t)EEP_VIRT_ADR_COUNT)

#endif /* EEPROM_EMUL_CONF_H_ */
