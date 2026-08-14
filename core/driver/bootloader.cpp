/*
 * bootloader.cpp - ST's SystemMemory bootloader jump, see bootloader.h.
 */
#include "bootloader.h"
#include "stm32xx_ll.h"
#if defined(I2C_HAT_MCU_FAMILY_G0)
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_cortex.h"
#include "stm32g0xx_ll_rcc.h"
#else
#include "board.h"      /* BOOT0_GPIO_* - the pad differs per package */
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_rcc.h"
#if !defined(BOOT0_GPIO_PORT) || !defined(BOOT0_GPIO_PIN) || !defined(BOOT0_GPIO_PERIPH)
#error "board.h must define BOOT0_GPIO_PORT/PIN/PERIPH - see an existing F0 board"
#endif
#endif

/* ROM system-memory base (AN2606). The reset vector pair at its start holds
 * the ROM's own initial MSP and its reset handler. */
#if defined(I2C_HAT_MCU_FAMILY_G0)
#define SYSTEM_MEMORY_BASE      (0x1FFF0000UL)
#else
#define SYSTEM_MEMORY_BASE      (0x1FFFC400UL)
/* The ROM's initial stack pointer, hardcoded as ST's example does. Read off
 * this chip with stm32flash: the word at 0x1FFFC400 is 0x200014C0, inside
 * the F042K6's 6 KB SRAM (0x20000000..0x20001800). */
#define SYSTEM_MEMORY_MSP       (0x200014C0UL)
#endif

extern "C" void Bootloader_Request(void) {
    /* #5 target, resolved first as ST's example does */
    void (*SysMemBootJump)(void) =
        reinterpret_cast<void (*)(void)>(
            *reinterpret_cast<uint32_t const*>(SYSTEM_MEMORY_BASE + 4U));

    /* #1 + #2 RCC_DeInit(): back to HSI, PLL off, clocks at reset defaults */
    LL_RCC_HSI_Enable();
    while(LL_RCC_HSI_IsReady() != 1U) {}

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI) {}

    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

    LL_RCC_PLL_Disable();
    while(LL_RCC_PLL_IsReady() != 0U) {}
    LL_RCC_HSE_Disable();
#if !defined(I2C_HAT_MCU_FAMILY_G0)
    LL_RCC_HSE_DisableCSS();    /* no LL equivalent on G0 */
#endif

    /* Clock-interrupt enables and flags. LL has no per-family helper that
     * clears them wholesale, so use its register accessors: the F0 keeps
     * both in CIR, the G0 splits them into CIER (enables) and CICR (flags). */
#if defined(I2C_HAT_MCU_FAMILY_G0)
    LL_RCC_WriteReg(CIER, 0U);
    LL_RCC_WriteReg(CICR, 0xFFFFFFFFU);
#else
    LL_RCC_WriteReg(CIR, 0U);
#endif

    /* #1 stop the SysTick timer. Core registers, not a peripheral: LL only
     * exposes the interrupt bit (LL_SYSTICK_DisableIT), so the counter and
     * reload still go through CMSIS - as do __set_PRIMASK/__set_MSP below. */
    LL_SYSTICK_DisableIT();
    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL  = 0U;

    /* #3 disable interrupts */
    __set_PRIMASK(1);

    /* NOT in ST's sequence - the piece under test here. On F04x the ROM runs
     * an empty check and hands a non-empty flash straight back to the
     * application; driving the BOOT0 pad high marks the entry as deliberate
     * so it skips that fallback. Reset-then-jump on master needs exactly this
     * too, so the question this build answers is narrow: is the pad drive
     * sufficient WITHOUT the reset, i.e. does ST's jump-from-the-running-app
     * recipe reach the ROM once the pad says "jumper fitted"?
     * ODR before MODER so the pin never drives a low first. Package-specific
     * pad (LQFP32 = PB8, LQFP48 = PF11) via the board.h macros. The G0 does
     * not sample the pin at all - no drive there, by decision. */
#if !defined(I2C_HAT_MCU_FAMILY_G0)
    LL_AHB1_GRP1_EnableClock(BOOT0_GPIO_PERIPH);
    LL_GPIO_SetOutputPin(BOOT0_GPIO_PORT, BOOT0_GPIO_PIN);
    LL_GPIO_SetPinMode(BOOT0_GPIO_PORT, BOOT0_GPIO_PIN, LL_GPIO_MODE_OUTPUT);
#endif

    /* Cortex-M0 has no VTOR: map the ROM at 0x00000000 so its own vector
     * table serves exceptions. Not in ST's list - their example targets a
     * part with a VTOR - but required on this core. */
#if defined(I2C_HAT_MCU_FAMILY_G0)
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
#else
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
#endif
    LL_SYSCFG_SetRemapMemory(LL_SYSCFG_REMAP_SYSTEMFLASH);

    /* #4 main stack pointer to its default value */
#if defined(SYSTEM_MEMORY_MSP)
    __set_MSP(SYSTEM_MEMORY_MSP);
#else
    __set_MSP(*reinterpret_cast<uint32_t const*>(SYSTEM_MEMORY_BASE));
#endif

    /* #5 load the PC with the SystemMemory reset vector */
    SysMemBootJump();

    while(1) {}     /* not reached */
}
