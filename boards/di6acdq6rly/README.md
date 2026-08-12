# DI6acDQ6rly I2C-HAT

6 opto-isolated AC digital inputs with an IRQ line, plus 6 relay outputs.
**STM32F042K6, LQFP32.** The only board carrying both module types, which makes
it the reference target for anything touching the core.

| | |
|---|---|
| Base I2C address | `0x60` + address-jumper offset (low nibble) |
| Modules | `DigitalInputs` + `DigitalOutputs` (`USES_DIGITAL_INPUTS USES_DIGITAL_OUTPUTS`) |
| Channels | 6 inputs (`DI0_Pin`..`DI5_Pin`); 6 relays on TIM1 CH1-4 + TIM2 CH1-2 |
| Extra status bit | `0x10` = IRQ capture queue full |
| BOOT0 pad | PB8 (LQFP32) |
| CubeMX project | `DI6acDQ6rly.ioc` (classic `Inc/` + `Src/` layout) |

Version lives in `board.h` (`FW_VERSION_*`) — the single source of truth the
firmware reports over I2C and the one `tools/bump.sh` / `tools/tag.sh` read.
Release history: `git tag -l 'di6acdq6rly-*'`.

```sh
make build BOARD=di6acdq6rly
```

## Board-specific notes

- **Having both modules is the point.** Every interaction that only exists when
  inputs and outputs share a board shows up here first: a CWDT trip that both
  loads the output safety values *and* disarms the IRQ block, and the full
  persistent set (output polarity/safety mask/write mask alongside input
  polarity/filters/IRQ masks) in one EEPROM layout. It is the board the release
  gate runs against.
- Relay channels are PWM outputs, not plain GPIOs: the core pulls a relay at
  full drive then holds it at a reduced duty cycle
  (`core/module/digital_outputs.cpp`). Two timers are involved, so both are
  started in `main.c` — TIM1 additionally needs `LL_TIM_EnableAllOutputs()`
  (the MOE bit) or nothing reaches the pins.
- The IRQ line is open-drain and wired-OR with any other DI board in the stack.
  Semantics (level, not edge) are in [REGISTER-MAP.md](../../REGISTER-MAP.md).
