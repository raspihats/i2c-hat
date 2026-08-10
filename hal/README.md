# Vendored ST drivers — one pinned copy per MCU family

Every board builds against these trees (selected by `FAMILY` in
`add_i2c_hat_board()`); the per-board `Drivers/` copies CubeMX regenerates
are gitignored scratch. Curated subsets, not full packages: LL sources the
core uses, plus (G0 only) the HAL modules the EEPROM emulation requires.

| Tree | Pinned at | Source |
|---|---|---|
| `CMSIS/Device/ST/STM32F0xx`, `STM32F0xx_HAL_Driver` | F0Cube-era LL set (pre-monorepo, unchanged since) | ST |
| `CMSIS/Device/ST/STM32G0xx` | cmsis_device_g0 **v1.4.5** | github.com/STMicroelectronics/cmsis_device_g0 |
| `STM32G0xx_HAL_Driver` | **v1.4.7** | github.com/STMicroelectronics/stm32g0xx_hal_driver |
| (both G0 trees) | == STM32Cube **FW_G0 V1.6.3**, byte-verified | pinned in `boards/ai4dcv10/CMakeLists.txt` |

## Bumping the HAL/CMSIS — the deliberate procedure

A bump changes every binary of that family: treat it as a release event
(RELEASING.md), done for a reason (errata fix, new part, needed feature),
never as a side effect of a CubeMX update prompt.

1. **Read the vendor release notes** for the delta, specifically I2C, FLASH,
   RCC and CMSIS device changes — those touch the core's contracts
   (TIMINGR/TIMEOUTA, EEPROM emulation, reset flags).
2. **In CubeMX**: install + select the new firmware package, regenerate.
   The configure-time package-pin warning fires — expected, it stays until
   step 4.
3. **Re-vendor**: clone the matching release tags from ST's GitHub (the
   package release notes name the component versions) and copy the same
   curated file lists over `hal/<family tree>` — check whether generated
   code now needs additional LL/HAL modules and add them to the list.
   Byte-verify the copy against the CubeMX package's `Drivers/` (or the
   central repository) like it was done for FW_G0 V1.6.3.
4. **Update the pin**: `_pinned_fw_package` in the family's board
   CMakeLists (currently `boards/ai4dcv10/CMakeLists.txt`).
5. **Build the whole fleet.** Binaries of the bumped family WILL differ —
   review the size/map delta; the other family must remain byte-identical
   to its released binaries.
6. **Hardware-validate before release**: the affected boards' regression
   suites on the bench (F0: the robot suites; G0: at minimum EEPROM
   persistence, 0x19 bootloader round-trip, host-bus soak). Then version
   bumps per RELEASING.md.
7. **Commit the vendor change on its own** (`vendor: bump G0 HAL v1.4.7 ->
   v1.5.x (FW_G0 V1.7.x)`), separate from any feature work riding on it.

The F0 tree predates this policy and is effectively frozen: its boards are
shipped and hardware-proven, and nothing forces a bump. If one is ever
needed, the same procedure applies, with the byte-identical check replaced
by the full robot regression on the rig.
