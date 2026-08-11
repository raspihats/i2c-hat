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
 *
 * The deinit recipe was measured against this one on the bench (DUT,
 * 2026-08-11). Three runs, and the reset-flag readings are what make them
 * conclusive - they rule out a crash or a watchdog as the explanation:
 *   - full deinit + direct jump, BOOT0 pad untouched     -> ROM returned to
 *     the application, status word 0x00 (no reset at all);
 *   - ST's published recipe verbatim (community KB: disable IRQs, stop
 *     SysTick, RCC deinit, clear NVIC, enable IRQs, MSP, jump - no memory
 *     remap, no peripheral resets), pad untouched        -> same, 0x00;
 *   - same deinit + BOOT0 pad driven high                -> bootloader at
 *     0x3E in under a second.
 * So the deinit path buys nothing on F04x: what ends the session is the
 * ROM's own empty-check (below), which runs after any recipe has finished.
 * It also drags a live IWDG into the ROM, since the watchdog cannot be
 * stopped once the application started it - a jump without a reset must
 * stretch it to its maximum, 26 s at the nominal 40 kHz LSI but only
 * ~17.5 s at the 60 kHz end of the spec. (A measured write+verify of this
 * 17 KB image takes 5 s, so that window would have been survivable; the
 * reason to prefer reset-then-jump is the empty-check result plus the free
 * IWDG clear, not the timing.)
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
 * so the BOOT0 PIN decides the boot area. On top of that these parts carry
 * the EMPTY CHECK, which is what actually ends a software entry - ST states
 * it plainly (community article "Empty check mechanism on STM32", which
 * names STM32F04x and STM32F070x6): "The system bootloader ... can detect
 * that flash is no longer empty. It then changes the boot memory mapping to
 * Main Flash and performs a jump to user code programmed there."
 * That check runs INSIDE the ROM, after any jump recipe has done its work,
 * which is why no published sequence avoids it. Driving BOOT0 high is what
 * tells the ROM the entry was deliberate, so it skips the fallback - that is
 * the only stateless escape, and it is what CheckAndEnter() does below. The
 * alternatives are the nBOOT0/BOOT_SEL option bits (persistent, and it
 * disables jumper recovery) or genuinely empty flash (destructive).
 *
 * G0 (ai4dcv10): ships with nBOOT_SEL=1, which means the BOOT0 pin is NOT
 * SAMPLED AT ALL - the nBOOT0/nBOOT1 option bits decide, and their defaults
 * say "user flash unless it is empty". BOOT0 also shares PA14 with SWCLK
 * there, which the board keeps as SWCLK, and it carries no boot jumper.
 *   DECIDED: the G0 will NOT use a pad drive - the pin is not sampled, so
 *   there is nothing to fake, and PA14 must stay SWCLK. The G0 path below is
 *   deliberately pad-free.
 *   TO VALIDATE on ai4dcv10 hardware: whether the G0 ROM applies its own
 *   empty-check style fallback to a software entry. If it turns out to, the
 *   lever is the option byte, not a GPIO: clear nBOOT0 before the reset and
 *   have the application restore it via OBL_LAUNCH afterwards - persistent
 *   state, so a power cut mid-sequence leaves the board in the ROM until
 *   something jumps it back out. Only reach for that if the pad-free path
 *   proves insufficient.
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

    // F04x only: defeat the ROM's empty check (see the header comment). It
    // reads BOOT0 live, and with the pad low it remaps to main flash and
    // relaunches the non-empty application. Driving the pad push-pull high
    // marks the entry as deliberate, exactly as a fitted jumper would; the
    // pin returns to its reset state at the next chip reset. WHICH pad is
    // package-specific (LQFP32 = PB8, LQFP48 = PF11), hence the board.h
    // macros - driving the wrong one silently relaunches the application,
    // which is precisely how di16ac 3.1.0 shipped broken.
    // The G0 needs none of this: its ROM does not sample the pin.
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
