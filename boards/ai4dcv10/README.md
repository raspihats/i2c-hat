# AI4dcv10 I2C-HAT (STM32G031K6T)

4 analog channels measuring voltage / current / power via TI INA228 monitors
on the I2C2 master bus. First G0-family board; first consumer of the
X-CUBE-EEPROM emulation (`middleware/eeprom_emul/`) selected by
`FAMILY G0` in `CMakeLists.txt`.

## Status

Builds and runs the shared core only: HAT protocol on I2C1
(name / version / status word / CWDT / factory restore), EEPROM emulation,
IWDG, status LED, address jumpers. **No analog functionality yet.**

## CubeMX round-trip

`ai4dcv10.ioc` is the hardware source of truth, and the build consumes
CubeMX's output **under its own names and locations** - `Core/Src`,
`Core/Inc`, `startup_stm32g031xx.s`, `STM32G031xx_FLASH.ld` - so
regeneration round-trips with **no sync step**. The app wiring lives only
in USER CODE blocks, which CubeMX preserves: `interface.h` include,
`Bootloader_CheckAndEnter()` first in `main()`, address jumpers ->
own-address, `I2CHat_init/run` in `main.c`; flash-ECC NMI repair and
`HAL_IncTick()` in `stm32g0xx_it.c`. (CubeMX's own `cmake/`,
`CMakePresets.json`, `Drivers/` are not used by this repo's build - the
vendor drivers come from the shared `hal/` tree, verified byte-identical
to the CubeMX-fetched HAL v1.4.7.)

Hand-owned files CubeMX never touches: `board.h`, `CMakeLists.txt`,
`Inc/stm32g0xx_hal_conf.h`, `README.md`.

CubeMX cannot be pointed at the shared `hal/` tree (it only understands
full STM32Cube firmware packages), so the relationship is inverted: the
firmware package is **pinned by version** (`STM32Cube FW_G0 V1.6.3`, which
ships exactly the vendored HAL v1.4.7 / CMSIS-device v1.4.5), and
`CMakeLists.txt` warns at configure time if the `.ioc` was regenerated
with any other package. The board-local `Drivers/` copy is gitignored and
unused; switching CubeMX to "Add necessary library files as reference"
(Project Manager → Code Generator) stops it being written at all - both
modes work.

**One exception needs care: the linker script.** `STM32G031xx_FLASH.ld`
carries two local deltas (FLASH LENGTH 32K -> 28K for the EEPROM pages,
and the `.noinit` section for the bootloader magic). If CubeMX regenerates
it they are silently lost - so `CMakeLists.txt` greps for them at configure
time and **fails the build with re-apply instructions** until they are
restored.

Generation style is **LL** (Project Manager → Advanced Settings), matching
the F0 fleet. HAL survives in exactly one role: the flash driver + tick
under the EEPROM emulation (`Inc/stm32g0xx_hal_conf.h` is kept by hand for
it — CubeMX no longer generates it in LL mode; `SysTick_Handler` calls
`HAL_IncTick()` for the flash timeouts). The INA228 driver will be an LL
master state machine in `core/driver/` — non-blocking, which fits the
cooperative scheduler better than blocking HAL master calls anyway.

## Config facts (from the .ioc)

- **64 MHz SYSCLK** (HSI16 → PLL N=8/R=2), I2C kernel clock = PCLK = 64 MHz.
  CubeMX Timing `0x10B17DB5` keeps the unified **375 ns SCLDEL** contract
  (PROTOCOL.md) at this clock; the core's 50 ms SCL-low TIMEOUTA backstop
  scales via `I2C_KERNEL_CLOCK_HZ` in `board.h`.
- I2C1 slave PB6/PB7 (host), I2C2 master PA11/PA12 (4× INA228, addresses
  0x40-0x43), IWDG, status LED **PC15**, address jumpers PA0-PA3.
  PF2 stays NRST and PA14 stays SWCLK (no BOOT0 jumper): **zero option-byte
  provisioning**, full SWD debug — keep SWD accessible on the PCB, it is
  the only recovery path if a flashed app boots but goes deaf on I2C.
- **EEPROM emulation area = last 4 KB** (2 × 2 KB pages at `0x0800_7000`).
  The build links with CubeMX's own `STM32G031xx_FLASH.ld` carrying the two
  guarded deltas described above; the startup file is CubeMX's
  `startup_stm32g031xx.s`, used as-is. Virtual-address layout
  comes from `core/driver/eeprom_emulation_conf.h` (shared with F0); each
  32-bit variable sits at `*_LOW`+1, `*_HIGH` slots unused. Note: `EE_Init()`
  intentionally burns one 8-byte element per boot as a torn-write guard
  (ST design, AN4894) - negligible for an always-on device.
  ST's successor package ([stm32-util-eeprom-emulation](https://github.com/STMicroelectronics/stm32-util-eeprom-emulation),
  v2.x 2026) was evaluated and deferred: its ready-made flash interfaces
  are HAL2/EDATA-only, which the G0 doesn't have - revisit if ST adds
  classic-HAL interfaces or a board moves to a HAL2-era MCU.

## Flashing without a jumper: ENTER_BOOTLOADER (0x19)

Command `0x19` with the `"boot"` signature (guarded like 0x18's `"load"`)
resets into the ROM system bootloader: the handler plants a magic word in
`.noinit` RAM and system-resets; `Bootloader_CheckAndEnter()` — the first
statement of `main()`, before the IWDG can ever start — sees it and jumps
to system memory (`core/driver/bootloader.{h,cpp}`). The board then
re-enumerates on the **same host bus** at the ROM's I2C address **0x56**
(AN2606, I2C1 PB6/PB7):

    <0x19 "boot">  →  device leaves app address  →
    stm32flash -w fw.bin -v -a 0x56 /dev/i2c-1  →  stm32flash -g 0 -a 0x56

No boot-jumper relay on the bench for this board. The IWDG never runs on
the bootloader path (it cannot be stopped once enabled - that is why the
entry is reset-then-jump, never a direct jump from the running app).

## Open hardware decisions (before schematic freeze)

- [ ] **Base address 0x70 with 4 jumpers** overruns into the I2C reserved
      block (0x78-0x7F) for offsets 8-15: move the base or drop ADR_3.
- [ ] **ADR_0-3 are NOPULL** in the .ioc: needs external pull-ups on the
      schematic, or switch them to internal pull-up in CubeMX.
- [ ] SWD header/pogo pads on the PCB (recovery path, see above).

## TODO (firmware)

- [ ] `core/driver/ina228.*` (I2C2 master driver) and
      `core/module/analog_inputs.*`; wire `BOARD_MODULE_MEMBERS`
- [ ] AI opcodes beyond the existing `0x40`-`0x48` block: power, energy,
      charge, alert thresholds (CiA 401 `0x6401` alignment for the reads)
- [ ] `SHUNT_CAL` / shunt-value calibration via `0x47`/`0x48`
- [ ] Hardware-validate the 0x19 → 0x56 bootloader round-trip on the bench
      (incl. whether the F0's stm32flash first-probe resync quirk exists
      on the G0 ROM)
- [ ] raspihats lib + raspihats-utils: `enter_bootloader()` and the
      jumperless flash procedure
