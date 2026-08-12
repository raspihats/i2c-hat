# DQ8rly I2C-HAT

8 relay outputs. **STM32F042K6, LQFP32.**

> **Prototype — not a shipping board.** It is built by `make build-all` and
> receives every core release like the others, but it is deliberately left
> **untagged** (`git tag -l 'dq8rly-*'` is empty) and no physical unit is
> hardware-validated. Don't treat its version numbers as released firmware, and
> don't point a host at it as a reference target — use dq10rly.

| | |
|---|---|
| Base I2C address | `0x50` + address-jumper offset (low nibble) |
| Modules | `DigitalOutputs` (`USES_DIGITAL_OUTPUTS`) |
| Channels | 8 relays on TIM3 CH1-4 + TIM1 CH1-4 |
| BOOT0 pad | PB8 (LQFP32) |
| CubeMX project | `DQ8rly.ioc` (classic `Inc/` + `Src/` layout) |

```sh
make build BOARD=dq8rly
```

Its value is as a **build-time canary**: it shares the 0x50 address family with
dq5rly and dq10rly but a different channel count and timer split, so a core
change that quietly depends on a channel count or a timer layout tends to fail
here first.

## Board-specific notes

- **`board.h` declares `RELAY_DRIVE_PULL_TIME_MS`, `RELAY_DRIVE_FREQUENCY` and
  `RELAY_DRIVE_HOLD_DUTY_CYCLE` — nothing reads them.** They are leftovers from
  this board's pre-monorepo branch; the shared core takes its pull time and hold
  duty from constants in `core/module/digital_outputs.cpp`, so editing them
  here changes nothing. Delete them, or make the core honour per-board
  overrides, but don't trust them as documentation of what the hardware does.
- Its `DIGITAL_OUTPUT_CHANNELS` entries are brace-wrapped, unlike the other
  output boards' — cosmetic, same initialisation.
- Relay channels are PWM outputs, not plain GPIOs. TIM1 needs
  `LL_TIM_EnableAllOutputs()` (the MOE bit) in `main.c` or channels 4-7 stay
  dead.
- A new DQ8rly is the first candidate for the planned G0 migration, and it
  would be a fresh design rather than a port of this one.
