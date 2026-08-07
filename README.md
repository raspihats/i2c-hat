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

boards/<name>/         one product; the CubeMX app only
  board.h              board identity, pins, channels, and MODULE WIRING
  <Name>.ioc           STM32CubeMX project
  Inc/ Src/            CubeMX-generated app (main.c, it.c, system, ...)
  startup/  *.ld       startup code + linker script
  CMakeLists.txt       calls add_i2c_hat_board() with this board's specifics

hal/                   ST vendor drivers, ONE pinned copy shared by all boards
  STM32F0xx_HAL_Driver/  LL drivers (Inc + Src)
  CMSIS/                 CMSIS core + STM32F0 device headers
middleware/
  eeprom/              ST EEPROM emulation (AN4061, C) — above HAL, shared by all boards
cmake/                 toolchain file + the add_i2c_hat_board() helper
tools/                 bump.sh, changelog.sh
Makefile               thin, hand-readable wrapper over CMake
PROTOCOL.md            the I2C transport contract (framing, transactions, stretching)
```

Four layers, kept separate on purpose:
**`core/`** — our C++ application / framework;
**`hal/`** — ST vendor drivers (HAL/LL + CMSIS), pinned to one version, shared by all boards;
**`middleware/`** — shared third-party C above HAL (the ST EEPROM emulation);
**`boards/<name>/`** — the per-board CubeMX app (`.ioc`, `Inc/`, `Src/`, startup, linker, `board.h`).

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

## Regenerating with STM32CubeMX / STM32CubeIDE

Each board is still a normal CubeMX project. Open its `.ioc` **from inside its own
folder** (`boards/<name>/<Name>.ioc`) and Generate Code — CubeMX regenerates in
place (`Inc/ Src/ Drivers/ startup/` + linker), exactly the existing layout. This
is identical whether you use standalone CubeMX or the CubeMX embedded in
STM32CubeIDE (same generator).

What survives regeneration:

- **Your firmware glue.** The `#include "interface.h"` and the `I2CHat_init() /
  I2CHat_run() / I2CHat_tick()` calls live inside CubeMX `USER CODE` blocks, and
  `KeepUserCode=true` in every `.ioc`, so regen preserves them.
- **`board.h` is yours** — CubeMX never touches it. It regenerates `main.h` (the
  `STATUS_LED_Pin`, `TIM3`, … pin macros `board.h` references). Keep your pin
  *user-labels* stable and `board.h` keeps matching.
- **`board.h` and your linker/startup** stay put (CubeMX rewrites `main.h`, the
  app in `Src/`, etc.).

Rules to avoid surprises:

1. **The vendor drivers are shared in `hal/`, not per-board.** CubeMX will still
   regenerate a `boards/<name>/Drivers/` folder — but the build ignores it (it
   compiles `hal/`) and it's git-ignored, so just leave it or delete it. To change
   the pinned driver version, update `hal/` **deliberately** (regenerate one board,
   copy its refreshed `Drivers/STM32F0xx_HAL_Driver` + `CMSIS` into `hal/`, rebuild
   all). All boards move together — that's the point.
2. **If you enable a peripheral whose LL driver isn't in `hal/` yet** (e.g. SPI),
   copy that `stm32f0xx_ll_*.c/.h` into `hal/` once — every board then has it.
3. **Don't set the `.ioc` Toolchain/IDE to "CMake".** That makes CubeMX emit its
   own `CMakeLists.txt` into the board folder and clobber ours. Keep it on
   STM32CubeIDE (the old `SW4STM32`/`TrueSTUDIO` ones will offer to migrate —
   accept; it only changes IDE files, which are git-ignored). We build with our
   own CMake regardless of that setting.
4. **If regen renames the linker/startup file**, update that board's one-line
   `LINKER` / `STARTUP` in `boards/<name>/CMakeLists.txt`. On GCC 10, also re-strip
   any GCC11-only `(READONLY)` markers CubeMX may re-add to the linker script.
5. **If you enable a peripheral that adds a whole module** (e.g. give an input
   board outputs), also flip the `USES_DIGITAL_OUTPUTS` flag in that board's
   `CMakeLists.txt` and add the `BOARD_MODULE_MEMBERS` / `BOARD_REGISTER_MODULES`
   macros in its `board.h`.

Workflow:

```sh
# open boards/dq8rly/DQ8rly.ioc in CubeMX -> Generate Code
git diff boards/dq8rly        # review exactly what regen changed
make build BOARD=dq8rly        # confirm it still compiles
```

`git diff` is the safety net — you'll see immediately if regen touched anything
outside the `USER CODE` regions.

### Building in STM32CubeIDE

CubeIDE regenerates code exactly as above, but its *native* Eclipse managed build
only sees files inside the board's own folder — it won't find the shared `core/`
at the repo root. So either:

- **(recommended)** use CubeIDE just to edit / generate the `.ioc`, and build with
  our CMake (terminal or VS Code) — one build source of truth; or
- **import the repo as a CMake project** in CubeIDE (1.15+) and point it at the
  top-level `CMakeLists.txt` with `-DBOARD=<name>`, so CubeIDE builds through our
  CMake and picks up `core/`.

Don't add `core/` as a linked folder in a managed Eclipse build — that duplicates
include paths and flags and drifts from the CMake setup. CubeIDE's generated
`.project` / `.cproject` / `.mxproject` files are already git-ignored per board.

## Talking to the boards

The host-facing I2C contract — frame format, the write-then-read transaction
model, clock-stretching expectations, and Raspberry Pi caveats — is documented
in [PROTOCOL.md](PROTOCOL.md). The command opcodes are in `core/commands.h`.

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
| dq8rly       | STM32F042K6 | 8 relay outputs |
| dq10rly      | STM32F042K6 | 10 relay outputs |

> The F3 analog board `ai3tcdq4rly` is **not** part of this monorepo yet — it stays
> on its `origin/ai3tcdq4rly` branch until the shared `core/` gets an MCU-family
> abstraction for its LL headers (see RELEASING.md).
