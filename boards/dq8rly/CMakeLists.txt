# dq8rly firmware target (STM32F042, shared core/).
add_i2c_hat_board(
    NAME          dq8rly
    MCU           STM32F042x6
    CPU           cortex-m0
    LINKER        STM32F042K6Tx_FLASH.ld
    STARTUP       startup/startup_stm32f042x6.s
    USES_DIGITAL_OUTPUTS
)
