/*
 * bootloader.cpp — see bootloader.h for the pattern and its rationale.
 *
 * Why reset-then-jump rather than the widespread "jump from the running
 * application" recipe: that one has to undo the application first - every
 * peripheral in reverse init order, each IRQ disabled individually, SysTick
 * reset - and anything forgotten leaves the ROM running on a dirty chip.
 * Planting a magic word and resetting hands the ROM a chip that is already
 * in its reset state, so there is nothing to undo; the only setup left is
 * the memory remap and the stack pointer.
 */
#include "bootloader.h"
#include "stm32xx_ll.h"
#if defined(I2C_HAT_MCU_FAMILY_G0)
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#else
#include "board.h"      /* BOOT0_GPIO_* - the pad differs per package */
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#if !defined(BOOT0_GPIO_PORT) || !defined(BOOT0_GPIO_PIN) || !defined(BOOT0_GPIO_PERIPH)
#error "board.h must define BOOT0_GPIO_PORT/PIN/PERIPH - see an existing F0 board"
#endif
#endif

/* How the two families select their boot area - they are opposites, and the
 * difference decides how much work a software entry has to do.
 *
 * F04x (all the F0 boards here): the User option byte ships with BOOT_SEL=1,
 * so the BOOT0 PIN decides. The ROM re-reads that pin on entry and hands a
 * non-empty flash straight back to the application - bench-proven 2026-08-11,
 * which is why CheckAndEnter() below drives the pad high before jumping.
 *
 * G0 (ai4dcv10): ships with nBOOT_SEL=1, which means the BOOT0 pin is NOT
 * SAMPLED AT ALL - the nBOOT0/nBOOT1 option bits decide, and their defaults
 * say "user flash unless it is empty". So the pad-driving hack is neither
 * possible nor expected to be needed here: BOOT0 shares PA14 with SWCLK, the
 * board keeps that as SWCLK and carries no boot jumper.
 *   UNVALIDATED until ai4dcv10 hardware exists: whether the G0 ROM
 *   re-evaluates nBOOT0 on a software entry the way F04x re-reads its pin.
 *   If it does, the lever is the option byte, not a GPIO: clear nBOOT0
 *   (or nBOOT_SEL) before the reset and have the application restore it via
 *   OBL_LAUNCH afterwards - persistent state, so a power cut mid-sequence
 *   leaves the board sitting in the ROM bootloader until something jumps it
 *   back out. Prefer the current pin-free path if it simply works.
 * Two more G0 notes for that bring-up:
 *   - Cortex-M0+ HAS VTOR, so SCB->VTOR = SYSTEM_MEMORY_BASE is available
 *     instead of the SYSCFG memory remap used below (M0 on F0 has no VTOR).
 *   - CheckAndEnter() is called from main() on G0 but from the top of
 *     Reset_Handler on F0, which is what finally made F0 work (ahead of
 *     SystemInit, data/bss init and the C++ static ctors). Moving the G0 call
 *     there means patching a CubeMX-generated startup file, so it needs the
 *     same configure-time guard the board's linker-script deltas already have.
 *   - Deaf-app recovery on a jumperless G0 board is the SWD pads, plus the
 *     ROM's empty check: a fully erased chip enters the bootloader by itself.
 *
 * ROM system-memory base (AN2606). The reset vector pair at its start is
 * the jump target. */
#if defined(I2C_HAT_MCU_FAMILY_G0)
#define SYSTEM_MEMORY_BASE      (0x1FFF0000UL)
#else /* F0 (F04x): same ROM bootloader the BOOT0 jumper reaches; software
       * entry lands on I2C at the familiar 0x3E. */
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

    // Cortex-M0/M0+ has no VTOR: remap system memory to 0x00000000 so the
    // ROM's own vector table serves exceptions - exactly what the hardware
    // BOOT0 path does at reset. Without the remap, any exception inside the
    // ROM vectors through the application's table and relaunches the app
    // (observed on the bench: 0x19 soft-reset but the app came back).
#if defined(I2C_HAT_MCU_FAMILY_G0)
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
#else
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

    // F04x: the ROM's boot selector does a LIVE read of the BOOT0 pad and
    // jumps straight back to a non-empty flash app when it reads low -
    // bench-proven: the same software entry stays in the bootloader when
    // BOOT0 is held high externally. Drive the pad push-pull high before
    // the jump so the selector sees "jumper fitted"; it falls back to its
    // reset state on the next chip reset. WHICH pad is package-specific
    // (LQFP32 = PB8, LQFP48 = PF11), hence the board.h macros - driving
    // the wrong one silently relaunches the application.
    LL_AHB1_GRP1_EnableClock(BOOT0_GPIO_PERIPH);
    LL_GPIO_SetOutputPin(BOOT0_GPIO_PORT, BOOT0_GPIO_PIN);
    LL_GPIO_SetPinMode(BOOT0_GPIO_PORT, BOOT0_GPIO_PIN, LL_GPIO_MODE_OUTPUT);
#endif
    LL_SYSCFG_SetRemapMemory(LL_SYSCFG_REMAP_SYSTEMFLASH);

    // Called before any clock/peripheral init otherwise, so the chip is in
    // its reset state - what the ROM bootloader expects. It then listens on
    // I2C among its other interfaces (AN2606: 0x3E on F04x, 0x56 on G0).
    uint32_t stack = *reinterpret_cast<uint32_t const*>(SYSTEM_MEMORY_BASE);
    uint32_t entry = *reinterpret_cast<uint32_t const*>(SYSTEM_MEMORY_BASE + 4U);
    __set_MSP(stack);
    reinterpret_cast<void (*)(void)>(entry)();
    // not reached
}
