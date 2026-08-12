# DQ10rly I2C-HAT

10 relay outputs — the widest output board in the fleet. **STM32F042K6,
LQFP32.**

| | |
|---|---|
| Base I2C address | `0x50` + address-jumper offset (low nibble) |
| Modules | `DigitalOutputs` (`USES_DIGITAL_OUTPUTS`) |
| Channels | 10 relays on TIM3 CH1-4 + TIM1 CH1-4 + TIM2 CH1-2 |
| BOOT0 pad | PB8 (LQFP32) |
| CubeMX project | `DQ10rly.ioc` (classic `Inc/` + `Src/` layout) |

Version lives in `board.h` (`FW_VERSION_*`) — the single source of truth the
firmware reports over I2C and the one `tools/bump.sh` / `tools/tag.sh` read.
Release history: `git tag -l 'dq10rly-*'`.

```sh
make build BOARD=dq10rly
```

## Board-specific notes

- **The output stack in `core/` came from this board** during the monorepo
  merge — `digital_outputs`, `relay_output_channel` and `digital_output_pwm`
  were taken together from here so their APIs stayed consistent (RELEASING.md,
  migration notes). It is therefore the closest thing to a reference
  implementation for output behaviour.
- **Ten channels across three timers** is the widest spread in the fleet, so
  it is the board where a channel-index-to-timer mapping error surfaces. All
  three timers are started in `main.c`, and TIM1 additionally needs
  `LL_TIM_EnableAllOutputs()` (the MOE bit) or channels 4-7 stay dead.
- Relay channels are PWM outputs, not plain GPIOs: the core pulls a relay at
  full drive then holds it at a reduced duty cycle
  (`core/module/digital_outputs.cpp`).
- Bit 9 is the highest valid channel bit; masks default to
  `(1 << channel_count) - 1`, so the safety mask reads `0x3FF` on a factory
  board — a useful sanity check that you are talking to a DQ10rly and not one
  of the other 0x50-family boards.
