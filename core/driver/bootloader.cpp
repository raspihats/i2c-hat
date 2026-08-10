/*
 * bootloader.cpp — see bootloader.h for the pattern and its rationale.
 */
#include "bootloader.h"
#include "stm32xx_ll.h"

/* ROM system-memory base (AN2606). The reset vector pair at its start is
 * the jump target. */
#if defined(I2C_HAT_MCU_FAMILY_G0)
#define SYSTEM_MEMORY_BASE      (0x1FFF0000UL)
#else /* F0 (F04x). Unused today: the F0 boards enter via the BOOT0 jumper,
       * and adopting the command there is a future coherent release. */
#define SYSTEM_MEMORY_BASE      (0x1FFFC400UL)
#endif

#define BOOTLOADER_MAGIC        (0xB0071ABDUL)

/* Survives NVIC_SystemReset (RAM keeps its content); the startup code must
 * not zero it, hence .noinit. On a cold power-up it holds junk, which is
 * exactly as safe: junk != magic. */
__attribute__((section(".noinit")))
static uint32_t bootloader_magic;

extern "C" void Bootloader_Request(void) {
    bootloader_magic = BOOTLOADER_MAGIC;
    NVIC_SystemReset();
}

extern "C" void Bootloader_CheckAndEnter(void) {
    if(bootloader_magic != BOOTLOADER_MAGIC) {
        return;
    }
    bootloader_magic = 0;

    // Called before any clock/peripheral init, so the chip is still in its
    // reset state - what the ROM bootloader expects. On the G0 it then
    // listens on I2C1 (PB6/PB7) at 7-bit address 0x56, among its other
    // interfaces.
    uint32_t stack = *reinterpret_cast<uint32_t const*>(SYSTEM_MEMORY_BASE);
    uint32_t entry = *reinterpret_cast<uint32_t const*>(SYSTEM_MEMORY_BASE + 4U);
    __set_MSP(stack);
    reinterpret_cast<void (*)(void)>(entry)();
    // not reached
}
