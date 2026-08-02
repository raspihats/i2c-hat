# add_i2c_hat_board(): defines one board firmware target.
#
# Each board's CMakeLists.txt calls this with its hardware specifics and declares
# which optional core modules it uses. Only the core sources a board actually
# uses are compiled, so an input-only board never pulls in the PWM driver (and
# the vendor TIM headers it doesn't ship), and vice-versa.
#
# Usage:
#   add_i2c_hat_board(
#       NAME          dq8rly
#       MCU           STM32F042x6            # -D<MCU> selects the CMSIS device header
#       CPU           cortex-m0
#       LINKER        STM32F042K6Tx_FLASH.ld
#       STARTUP       startup/startup_stm32f042x6.s
#       HAL_INC       Drivers/STM32F0xx_HAL_Driver/Inc
#       CMSIS_DEVICE  STM32F0xx
#       USES_DIGITAL_OUTPUTS                 # + USES_DIGITAL_INPUTS / USES_ANALOG_INPUTS
#       [CORE_DIR     <path>]                # default: <repo>/core
#       [FPU fpv4-sp-d16 FLOAT_ABI hard]
#       [EXTRA_INCLUDE_DIRS ...] [EXTRA_DEFINES ...]
#   )

function(add_i2c_hat_board)
    cmake_parse_arguments(B
        "USES_DIGITAL_INPUTS;USES_DIGITAL_OUTPUTS;USES_ANALOG_INPUTS"   # options
        "NAME;MCU;CPU;FPU;FLOAT_ABI;LINKER;STARTUP;HAL_INC;CMSIS_DEVICE;CORE_DIR"
        "EXTRA_INCLUDE_DIRS;EXTRA_DEFINES"
        ${ARGN})

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
        ${B_CORE_DIR}/driver/eeprom.cpp
        ${B_CORE_DIR}/driver/i2c_port.cpp
        ${B_CORE_DIR}/driver/digital_output_pin.cpp)   # status LED

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

    # ---- board app + vendor drivers (all present files) ----
    file(GLOB_RECURSE DRIVER_SRC CONFIGURE_DEPENDS ${BOARD_DIR}/Drivers/*.c)
    file(GLOB          APP_SRC   CONFIGURE_DEPENDS ${BOARD_DIR}/Src/*.c ${BOARD_DIR}/Src/*.cpp)

    add_executable(${B_NAME}
        ${CORE_SRC} ${DRIVER_SRC} ${APP_SRC} ${BOARD_DIR}/${B_STARTUP})

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
        ${BOARD_DIR}/Inc                # main.h, *_it.h, hal_conf
        ${B_CORE_DIR}                   # shared firmware headers
        ${BOARD_DIR}/${B_HAL_INC}
        ${BOARD_DIR}/Drivers/CMSIS/Device/ST/${B_CMSIS_DEVICE}/Include
        ${BOARD_DIR}/Drivers/CMSIS/Include
        ${BOARD_DIR}/Drivers/EEPROM
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
