# I2C-HAT firmware

Firmware for the [raspihats.com](https://raspihats.com) I2C-HAT boards.

This is a **monorepo**: one shared firmware library (`core/`) plus one folder per
board (`boards/<name>/`). A fix in `core/` reaches every board at once — there is
a single source of truth, no per-board branches to keep in sync.

## Layout

```
core/                  shared, board-agnostic firmware (STM32F0 family)
  cooperative_os/      cooperative scheduler / tasks / event bus
  driver/              LL-based peripheral drivers (gpio, i2c, pwm, ...)
  frame/               I2C request/response framing + CRC
  module/              command modules (digital in/out, watchdog, status led)
  i2c_hat.{h,cpp}      the framework; module set comes from each board.h
  commands.h           the I2C command opcodes (host contract)

boards/<name>/         one product; self-contained CubeMX project
  board.h              board identity, pins, channels, and MODULE WIRING
  <Name>.ioc           STM32CubeMX project
  Inc/ Src/            CubeMX-generated app (main.c, it.c, system, ...)
  Drivers/             vendor HAL/LL/CMSIS for that board
  startup/  *.ld       startup code + linker script
  CMakeLists.txt       calls add_i2c_hat_board() with this board's specifics

cmake/                 toolchain file + the add_i2c_hat_board() helper
tools/                 bump.sh, changelog.sh
Makefile               thin, hand-readable wrapper over CMake
```

## The core ↔ board seam

`core/i2c_hat.{h,cpp}` is fully shared. Which modules a board exposes is declared
by that board — in the same `board.h` that already declares its pins and channels
— via three macros:

```c
#define BOARD_MODULE_MEMBERS    module::DigitalOutputs digital_outputs_;
#define BOARD_REGISTER_MODULES  Register(digital_outputs_);
#define BOARD_STATUS_BITS       /* optional extra status-word bits */
```

`board.h` also `#include`s the module headers it needs and the MCU-family
`..._ll_rcc.h`. This block is wrapped in `#ifdef __cplusplus` so the C
CubeMX sources (`main.c`, `stm32f0xx_it.c`) can still include `board.h` for the
plain `#define`s.

## Build

Requires `arm-none-eabi-gcc` and `cmake`.

```sh
make build BOARD=dq8rly      # configure + build one board
make build-all               # build every board (the CI gate)
make list                    # list boards
make clean
```

Or drive CMake directly:

```sh
cmake -B build/dq8rly -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-toolchain.cmake -DBOARD=dq8rly
cmake --build build/dq8rly
```

Artifacts land in `build/<board>/` as `<board>.elf`, `.bin`, `.hex`.

VS Code: point the CMake Tools "configure args" at the toolchain file and set
`-DBOARD=<name>`. STM32CubeIDE can import the folder as a CMake project.

## Releasing / versioning

See [RELEASING.md](RELEASING.md). Short version:

```sh
make release-core KIND=patch          # core fix -> bump+build every board
make bump BOARD=di16ac KIND=minor     # one-board change
```

## Boards

| Board | MCU | Function |
|-------|-----|----------|
| di16ac       | STM32F042C6 | 16 digital inputs (+IRQ) |
| di6acdq6rly  | STM32F042K6 | 6 digital inputs (+IRQ) + 6 relay outputs |
| dq5rly       | STM32F042K6 | 5 relay outputs |
| dq6od        | STM32F042K6 | 6 open-drain outputs |
| dq8rly       | STM32F042K6 | 8 relay outputs |
| dq10rly      | STM32F042K6 | 10 relay outputs |

> The F3 analog board `ai3tcdq4rly` is **not** part of this monorepo yet — it stays
> on its `origin/ai3tcdq4rly` branch until the shared `core/` gets an MCU-family
> abstraction for its LL headers (see RELEASING.md).
