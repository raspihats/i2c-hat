# ai4dcv10 firmware target (STM32G031K6T, shared core/).
# First G0-family board: FAMILY G0 selects hal/STM32G0xx_HAL_Driver and the
# X-CUBE-EEPROM middleware (middleware/eeprom_emul/).
#
# Everything CubeMX generates is consumed under its own name and location
# (ai4dcv10.ioc -> Core/Src, Core/Inc, startup_stm32g031xx.s,
# STM32G031xx_FLASH.ld), so regeneration round-trips directly - the app
# wiring lives in USER CODE blocks. Hand-owned files: board.h, this file,
# and Inc/stm32g0xx_hal_conf.h (CubeMX doesn't generate one in LL mode;
# the EEPROM emulation's HAL flash core needs it).
#
# The linker script carries two local deltas CubeMX doesn't know about:
# FLASH LENGTH 28K (EEPROM page reservation) and the .noinit section
# (bootloader-entry magic). If CubeMX regenerates the script they are lost,
# so fail the configure until they are re-applied:
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/STM32G031xx_FLASH.ld _ai4dcv10_ld)
if(NOT _ai4dcv10_ld MATCHES "noinit" OR NOT _ai4dcv10_ld MATCHES "LENGTH = 28K")
    message(FATAL_ERROR
        "STM32G031xx_FLASH.ld looks freshly CubeMX-generated: re-apply the two "
        "local deltas (FLASH LENGTH 32K -> 28K for the EEPROM pages, and the "
        ".noinit (NOLOAD) section before ._user_heap_stack for the bootloader "
        "magic). See boards/ai4dcv10/README.md, 'CubeMX round-trip'.")
endif()

# The vendor drivers come from the shared, pinned hal/ tree; CubeMX cannot
# be pointed at it (it only understands full STM32Cube firmware packages),
# so drift between the CubeMX package and the pinned hal/ must be detected
# instead. Primary tripwire: the .ioc records which package generated the
# code - warn when it is no longer the one hal/ was vendored from.
set(_pinned_fw_package "STM32Cube FW_G0 V1.6.3")   # ships HAL v1.4.7 / CMSIS-device v1.4.5, == hal/
file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/ai4dcv10.ioc _ioc_fw_line
     REGEX "^ProjectManager\\.FirmwarePackage=")
string(REPLACE "ProjectManager.FirmwarePackage=" "" _ioc_fw_package "${_ioc_fw_line}")
if(NOT _ioc_fw_package STREQUAL _pinned_fw_package)
    message(WARNING
        "ai4dcv10.ioc was generated with '${_ioc_fw_package}' but hal/ is "
        "vendored from '${_pinned_fw_package}'. If the package bump is "
        "intentional, follow the bump procedure in hal/README.md; otherwise "
        "re-select '${_pinned_fw_package}' in CubeMX (Project Manager -> "
        "Firmware Package).")
endif()

# Secondary tripwire (only fires in CubeMX 'copy libraries' mode, where a
# board-local Drivers/ tree exists - gitignored, unused by this build):
# byte-compare a version-bearing file against the pinned hal/.
set(_mx_hal ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal.c)
set(_pinned_hal ${CMAKE_SOURCE_DIR}/hal/STM32G0xx_HAL_Driver/Src/stm32g0xx_hal.c)
if(EXISTS ${_mx_hal})
    file(SHA1 ${_mx_hal} _mx_sha)
    file(SHA1 ${_pinned_hal} _pinned_sha)
    if(NOT _mx_sha STREQUAL _pinned_sha)
        message(WARNING
            "CubeMX generated a DIFFERENT HAL than the pinned hal/ tree "
            "(firmware package updated?). The build still compiles the pinned "
            "copy. If the bump is intentional, follow the procedure in "
            "hal/README.md; otherwise re-select the pinned package version in "
            "CubeMX (.ioc: ProjectManager.FirmwarePackage).")
    endif()
endif()

add_i2c_hat_board(
    NAME          ai4dcv10
    MCU           STM32G031xx
    FAMILY        G0
    CPU           cortex-m0plus
    LINKER        STM32G031xx_FLASH.ld
    STARTUP       startup_stm32g031xx.s
)
