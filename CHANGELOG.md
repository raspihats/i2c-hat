# Changelog

## 2026-08-11  di16ac 3.1.1: ENTER_BOOTLOADER drives the right BOOT0 pad for its package. The 0x19 software entry drives the BOOT0 pad high so the F04x ROM's live-reading boot selector sees "jumper fitted", but the pad is package-specific: LQFP32 parts (dq5rly/dq8rly/dq10rly/di6acdq6rly) share it with PB8, the LQFP48 di16ac with PF11. 3.1.0 drove PB8 unconditionally, so on di16ac 0x19 was a no-op that merely reset the board (caught by the new Enter Bootloader Roundtrip test). The pad is now a board.h macro (BOOT0_GPIO_PORT/PIN/PERIPH) with a compile-time #error if an F0 board omits it; the four LQFP32 binaries are byte-identical to their 3.1.0 builds, so only di16ac bumps
Applied to: di16ac 3.1.1

## 2026-08-11  Host-convenience series: ENTER_BOOTLOADER fleet-wide + CiA 301 0x1020 + CiA 401 0x6208. (1) 0x19 'boot'-guarded software entry into the ROM bootloader now works on the F0 boards - jumperless flashing at 0x3E. Three fixes made it real: SYSCFG system-memory remap before the jump (M0 has no VTOR), the magic check moved to the top of Reset_Handler (true reset-state chip - before SystemInit, data/bss init and C++ static ctors), and driving the PB8/BOOT0 pad high pre-jump because the F04x ROM's boot selector LIVE-reads BOOT0 and otherwise relaunches a non-empty flash app (bench-proven). (2) 0x1020 configuration signature (0x1A/0x1B): controller-owned persistent u32, voided to 0 by any REAL change to another persistent value (choke point in Eeprom::Write, after the skip-if-unchanged guard - identical re-writes keep it); factory restore wipes it. Steady-state reconcile becomes one read. (3) 0x6208 output write mask (0x3C/0x3D): gates BULK writes only, single-channel writes bypass it, trips/power-on governed by 0x6206/power-on value alone; VOLATILE - all-ones after every reset (DS401 practice); DO_SET_VALUE validates and echoes the RECEIVED value so oblivious bulk writers keep their echo check
Applied to: di16ac 3.1.0, di6acdq6rly 3.1.0, dq10rly 2.4.0, dq5rly 1.3.0, dq8rly 2.3.0


## 2026-08-11  CiA 401 IRQ block (ROADMAP.md): global interrupt enable 0x6005 (IRQ reg 0x23 via 0x16/0x17 - volatile arming bit, 0 after every reset; disarming, by command or CWDT timeout, dumps the capture queue and releases the IRQ line); edge-control masks 0x6007/0x6008 (IRQ regs 0x21/0x20) now EEPROM-persistent; the IRQ line is derived from the capture queue alone - a DI value read no longer releases it. BREAKING for IRQ users: capture requires the armed global enable. 0x6006 any-change is intentionally not implemented (it is rising OR falling per bit)
Applied to: di16ac 3.0.0, di6acdq6rly 3.0.0


## 2026-08-08  CiA 401/301 alignment (ROADMAP.md): output polarity 0x6202 (DO 0x38/0x39) + per-channel safety mask 0x6206 (DO 0x3A/0x3B); input polarity 0x6002 (DI 0x2C/0x2D) + per-channel filter constant 0x6003 (DI 0x2A/0x2B, ms, replaces the fixed 2 ms debounce); restore factory defaults 0x1011 (0x18, 'load'-guarded, formats EEPROM + reset)
Applied to: di16ac 2.3.0, di6acdq6rly 2.3.0, dq10rly 2.3.0, dq5rly 1.2.0, dq8rly 2.2.0


## 2026-08-08  core: atomic combined-transfer commands (write+Sr+read in one transaction); the SMBus dummy-byte path is unchanged, so smbus2 hosts are unaffected (see PROTOCOL.md)
Applied to: di16ac 2.2.0, di6acdq6rly 2.2.0, dq10rly 2.2.0, dq5rly 1.1.0, dq8rly 2.1.0


## 2026-08-07  core: EEPROM page-transfer data-loss fix; skip flash writes when value unchanged; fix command-drop race so hosts need no write-to-read delay; 50 ms hardware SCL-low timeout backstop; TIMINGR unified on higher data-setup margin; state-machine cleanup (see PROTOCOL.md)
Applied to: di16ac 2.1.3, di6acdq6rly 2.1.4, dq10rly 2.1.2, dq5rly 1.0.1, dq8rly 2.0.2


