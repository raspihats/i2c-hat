# di16ac firmware target (STM32F042, shared core/).
add_i2c_hat_board(
    NAME          di16ac
    MCU           STM32F042x6
    CPU           cortex-m0
    LINKER        STM32F042C6_FLASH.ld
    STARTUP       startup/startup_stm32f042x6.s
    USES_DIGITAL_INPUTS
)
