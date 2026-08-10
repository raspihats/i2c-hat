# add_i2c_hat_board(): defines one board firmware target.
#
# Each board's CMakeLists.txt calls this with its hardware specifics and declares
# which optional core modules it uses. Only the core sources a board actually uses
# are compiled. The ST vendor drivers (HAL/LL + CMSIS) are shared once from hal/,
# and the ST EEPROM emulation from middleware/eeprom/.
#
# Usage:
#   add_i2c_hat_board(
#       NAME          dq8rly
#       MCU           STM32F042x6            # -D<MCU> selects the CMSIS device header
#       FAMILY        F0                     # MCU family: F0 (default) or G0 -
#                                            # selects hal/ tree + EEPROM middleware
#       CPU           cortex-m0
#       LINKER        STM32F042K6Tx_FLASH.ld
#       STARTUP       startup/startup_stm32f042x6.s
#       USES_DIGITAL_OUTPUTS                 # + USES_DIGITAL_INPUTS / USES_ANALOG_INPUTS
#       [CORE_DIR     <path>]                # default: <repo>/core
#       [FPU fpv4-sp-d16 FLOAT_ABI hard]
#       [EXTRA_INCLUDE_DIRS ...] [EXTRA_DEFINES ...]
#   )

function(add_i2c_hat_board)
    cmake_parse_arguments(B
        "USES_DIGITAL_INPUTS;USES_DIGITAL_OUTPUTS;USES_ANALOG_INPUTS"   # options
        "NAME;MCU;FAMILY;CPU;FPU;FLOAT_ABI;LINKER;STARTUP;CORE_DIR"
        "EXTRA_INCLUDE_DIRS;EXTRA_DEFINES"
        ${ARGN})

    if(NOT B_FAMILY)
        set(B_FAMILY F0)
    endif()

    set(BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    if(NOT B_CORE_DIR)
        set(B_CORE_DIR ${CMAKE_SOURCE_DIR}/core)
    endif()

    # ---- core sources: the always-on framework ----
    set(CORE_SRC
        ${B_CORE_DIR}/cooperative_os/event_bus.cpp
        ${B_CORE_DIR}/cooperative_os/scheduler.cpp
        ${B_CORE_DIR}/cooperative_os/task.cpp
        ${B_CORE_DIR}/frame/crc16.cpp
        ${B_CORE_DIR}/frame/frame.cpp
        ${B_CORE_DIR}/interface.cpp
        ${B_CORE_DIR}/i2c_hat.cpp
        ${B_CORE_DIR}/module/module.cpp
        ${B_CORE_DIR}/module/status_led.cpp
        ${B_CORE_DIR}/module/communication_watchdog.cpp
        ${B_CORE_DIR}/driver/bootloader.cpp
        ${B_CORE_DIR}/driver/eeprom.cpp
        ${B_CORE_DIR}/driver/i2c_port.cpp
        ${B_CORE_DIR}/driver/digital_output_pin.cpp)   # status LED

    # ---- shared middleware: ST EEPROM emulation, C, sits above HAL.
    #      One per flash generation: F0 keeps AN4061 (ships the SPL flash
    #      driver), G0 uses X-CUBE-EEPROM EEPROM_Emul (64-bit elements, ECC-
    #      aware, needs the HAL flash driver + hardware CRC). ----
    if(B_FAMILY STREQUAL "G0")
        set(MIDDLEWARE_SRC
            ${CMAKE_SOURCE_DIR}/middleware/eeprom_emul/eeprom_emul.c
            ${CMAKE_SOURCE_DIR}/middleware/eeprom_emul/flash_interface.c)
        set(MIDDLEWARE_INC ${CMAKE_SOURCE_DIR}/middleware/eeprom_emul)
    else()
        set(MIDDLEWARE_SRC
            ${CMAKE_SOURCE_DIR}/middleware/eeprom/eeprom_emulation.c
            ${CMAKE_SOURCE_DIR}/middleware/eeprom/stm32f0xx_flash.c)
        set(MIDDLEWARE_INC ${CMAKE_SOURCE_DIR}/middleware/eeprom)
    endif()

    # ---- optional modules, selected per board ----
    if(B_USES_DIGITAL_OUTPUTS)
        list(APPEND CORE_SRC
            ${B_CORE_DIR}/module/digital_outputs.cpp
            ${B_CORE_DIR}/module/relay_output_channel.cpp
            ${B_CORE_DIR}/driver/digital_output_pwm.cpp)
    endif()
    if(B_USES_DIGITAL_INPUTS)
        list(APPEND CORE_SRC
            ${B_CORE_DIR}/module/digital_inputs.cpp
            ${B_CORE_DIR}/module/digital_input_channel.cpp
            ${B_CORE_DIR}/driver/digital_input_pin.cpp)
    endif()
    if(B_USES_ANALOG_INPUTS)
        list(APPEND CORE_SRC
            ${B_CORE_DIR}/module/analog_inputs.cpp
            ${B_CORE_DIR}/driver/thermocouple/k_type.cpp
            ${B_CORE_DIR}/driver/thermocouple/thermocouple.cpp)
    endif()

    # ---- board app sources (CubeMX-generated: main.c, it.c, system, ...).
    #      Old CubeMX layout is Src/Inc (F0 boards), modern CubeMX generates
    #      into Core/Src + Core/Inc (ai4dcv10) - compiled straight from where
    #      CubeMX writes them, so regeneration round-trips with no sync step
    #      (app wiring lives in USER CODE blocks). ----
    file(GLOB APP_SRC CONFIGURE_DEPENDS
        ${BOARD_DIR}/Src/*.c ${BOARD_DIR}/Src/*.cpp
        ${BOARD_DIR}/Core/Src/*.c ${BOARD_DIR}/Core/Src/*.cpp)

    # ---- shared vendor HAL/LL: one pinned copy per family in hal/.
    #      The full vendored set is compiled; --gc-sections drops what a board
    #      doesn't use. ----
    file(GLOB HAL_SRC CONFIGURE_DEPENDS
        ${CMAKE_SOURCE_DIR}/hal/STM32${B_FAMILY}xx_HAL_Driver/Src/*.c)

    add_executable(${B_NAME}
        ${CORE_SRC} ${MIDDLEWARE_SRC} ${HAL_SRC} ${APP_SRC} ${BOARD_DIR}/${B_STARTUP})

    # ---- cpu / fpu flags (shared by compile + link) ----
    set(CPU_FLAGS -mcpu=${B_CPU} -mthumb)
    if(B_FPU)
        list(APPEND CPU_FLAGS -mfpu=${B_FPU} -mfloat-abi=${B_FLOAT_ABI})
    else()
        list(APPEND CPU_FLAGS -mfloat-abi=soft)
    endif()

    target_compile_definitions(${B_NAME} PRIVATE
        ${B_MCU} USE_FULL_LL_DRIVER ${B_EXTRA_DEFINES})

    target_include_directories(${B_NAME} PRIVATE
        ${BOARD_DIR}                    # board.h
        ${BOARD_DIR}/Inc                # old layout: main.h, *_it.h; new layout: hand-kept conf only
        ${BOARD_DIR}/Core/Inc           # modern CubeMX layout: main.h, *_it.h
        ${B_CORE_DIR}                   # shared firmware headers
        ${B_CORE_DIR}/driver            # eeprom_emulation_conf.h, by bare name
        ${CMAKE_SOURCE_DIR}/hal/STM32${B_FAMILY}xx_HAL_Driver/Inc
        ${CMAKE_SOURCE_DIR}/hal/STM32${B_FAMILY}xx_HAL_Driver/Inc/Legacy
        ${CMAKE_SOURCE_DIR}/hal/CMSIS/Device/ST/STM32${B_FAMILY}xx/Include
        ${CMAKE_SOURCE_DIR}/hal/CMSIS/Include
        ${MIDDLEWARE_INC}
        ${B_EXTRA_INCLUDE_DIRS})

    target_compile_options(${B_NAME} PRIVATE
        ${CPU_FLAGS}
        -ffunction-sections -fdata-sections
        -Wall
        $<$<CONFIG:Debug>:-Og -g3>
        $<$<CONFIG:Release>:-Os -g>
        $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti -fno-threadsafe-statics>)

    target_link_options(${B_NAME} PRIVATE
        ${CPU_FLAGS}
        -T${BOARD_DIR}/${B_LINKER}
        -Wl,--gc-sections
        -specs=nano.specs -specs=nosys.specs
        -Wl,-Map=${B_NAME}.map,--cref)

    set_target_properties(${B_NAME} PROPERTIES OUTPUT_NAME ${B_NAME} SUFFIX ".elf")

    add_custom_command(TARGET ${B_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${B_NAME}> ${B_NAME}.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${B_NAME}> ${B_NAME}.hex
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${B_NAME}>
        VERBATIM)
endfunction()
