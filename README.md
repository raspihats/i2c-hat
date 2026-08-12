# I2C-HAT firmware

Firmware for the [raspihats.com](https://raspihats.com) I2C-HAT boards.

This is a **monorepo**: one shared firmware library (`core/`) plus one folder per
board (`boards/<name>/`). A fix in `core/` reaches every board at once — there is
a single source of truth, no per-board branches to keep in sync.

## Layout

```
core/                  shared, board-agnostic firmware (STM32F0 + STM32G0)
  cooperative_os/      cooperative scheduler / tasks / event bus
  driver/              LL-based peripheral drivers (gpio, i2c, pwm, eeprom, bootloader)
    stm32xx_ll.h       family umbrella: includes the right LL headers per FAMILY
  frame/               I2C request/response framing + CRC
  module/              command modules (digital in/out, watchdog, status led)
  i2c_hat.{h,cpp}      the framework; module set comes from each board.h
  commands.h           the I2C command opcodes (host contract)

boards/<name>/         one product; the CubeMX app only
  board.h              board identity, pins, channels, and MODULE WIRING
  <Name>.ioc           STM32CubeMX project
  Inc/ Src/            CubeMX-generated app (main.c, it.c, system, ...)
                       modern CubeMX writes Core/Inc + Core/Src instead — both work
  startup/  *.ld       startup code + linker script
  CMakeLists.txt       calls add_i2c_hat_board() with this board's specifics

hal/                   ST vendor drivers, ONE pinned copy PER FAMILY (hal/README.md)
  STM32F0xx_HAL_Driver/  LL drivers (Inc + Src)
  STM32G0xx_HAL_Driver/  LL + the HAL modules the G0 EEPROM emulation needs
  CMSIS/                 CMSIS core + STM32F0 / STM32G0 device headers
middleware/            ST EEPROM emulation, C, above HAL — one per flash generation
  eeprom/              F0: AN4061 (half-word programming, ships its own flash driver)
  eeprom_emul/         G0: X-CUBE-EEPROM (64-bit-only programming, ECC-aware)
cmake/                 toolchain file + the add_i2c_hat_board() helper
tools/                 bump.sh, changelog.sh, tag.sh
Makefile               thin, hand-readable wrapper over CMake
PROTOCOL.md            the I2C transport contract (framing, transactions, stretching)
REGISTER-MAP.md        the whole command surface with since-versions (host-facing)
ROADMAP.md             CiA 401 alignment — adopted, decided, proposed
RELEASING.md           versioning, the bump/changelog/tag workflow
```

Four layers, kept separate on purpose:
**`core/`** — our C++ application / framework;
**`hal/`** — ST vendor drivers (HAL/LL + CMSIS), one pinned copy per MCU family;
**`middleware/`** — shared third-party C above HAL (the ST EEPROM emulation);
**`boards/<name>/`** — the per-board CubeMX app (`.ioc`, `Inc/`, `Src/`, startup, linker, `board.h`).

`core/` was F0-only until the G0 arrived; it is now family-agnostic. A board
picks its family with `FAMILY F0|G0` in `add_i2c_hat_board()` (default `F0`),
and that one argument selects the `hal/` tree, the EEPROM middleware and the
include paths. Inside `core/`, the common LL headers come from
`driver/stm32xx_ll.h`, which keys off the CMSIS device define the build
passes (`-DSTM32F042x6`, `-DSTM32G031xx`) and defines
`I2C_HAT_MCU_FAMILY_F0` / `_G0` for the few places the code genuinely
differs (RCC reset flags, EEPROM middleware, the bootloader's memory remap).
Adding a family means adding its arm to that header.

## The core ↔ board seam

`core/i2c_hat.{h,cpp}` is fully shared. Which modules a board exposes is declared
by that board — in the same `board.h` that already declares its pins and channels
— via three macros:

```c
#define BOARD_MODULE_MEMBERS    module::DigitalOutputs digital_outputs_;
#define BOARD_REGISTER_MODULES  Register(digital_outputs_);
#define BOARD_STATUS_BITS       /* optional extra status-word bits */
```

`board.h` also `#include`s the module headers it needs and its own family's
`..._ll_rcc.h`. This block is wrapped in `#ifdef __cplusplus` so the C
CubeMX sources (`main.c`, `stm32f0xx_it.c`) can still include `board.h` for the
plain `#define`s.

Two more things `board.h` owns, both with compile-time consequences:

- `FW_VERSION_MAJOR/MINOR/PATCH` — the single source of truth for the version
  the board reports over I2C, read by `tools/bump.sh` and `tools/tag.sh`
  (RELEASING.md).
- `BOOT0_GPIO_PORT/PIN/PERIPH` (F0 boards only) — the pad the ROM's boot
  selector shares with a GPIO, which `ENTER_BOOTLOADER` drives high. It is
  **package-specific** (LQFP32 → PB8, LQFP48 → PF11), and an F0 board that
  omits it fails the build with an `#error`. The G0 path is deliberately
  pad-free — see the header comment in `core/driver/bootloader.cpp`, which
  carries the bench evidence for both families.

## Build

Requires `arm-none-eabi-gcc` and `cmake`.

```sh
make build BOARD=dq10rly      # configure + build one board
make build-all               # build every board (the CI gate)
make list                    # list boards
make clean
```

Or drive CMake directly:

```sh
cmake -B build/dq10rly -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-toolchain.cmake -DBOARD=dq10rly
cmake --build build/dq10rly
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

Recent CubeMX versions generate into `Core/Inc` + `Core/Src` instead of
`Inc` + `Src` (that is what `ai4dcv10` uses). The build globs both, so either
layout compiles with no sync step — don't "fix" a board into the other one.

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
   compiles `hal/`) and it's git-ignored, so just leave it or delete it. Changing
   the pinned driver version moves every board of that family at once — that's
   the point — so it is a deliberate release event with its own procedure in
   [hal/README.md](hal/README.md), not something to do as a side effect of a
   CubeMX update prompt. `ai4dcv10`'s `CMakeLists.txt` pins the G0 firmware
   package and warns at configure time when its `.ioc` drifts off it — the
   tripwire that catches an accidental bump; the frozen F0 tree has no
   equivalent.
2. **If you enable a peripheral whose LL driver isn't in `hal/` yet** (e.g. SPI),
   copy that family's `stm32?0xx_ll_*.c/.h` into `hal/` once — every board of
   that family then has it.
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
6. **If a board's linker script carries local deltas, regen silently drops
   them.** `ai4dcv10` reserves flash for the EEPROM pages and adds a `.noinit`
   section for the bootloader magic, so its `CMakeLists.txt` greps for both at
   configure time and **fails the build** with re-apply instructions. Any board
   that grows such a delta should get the same guard rather than a comment.

Workflow:

```sh
# open boards/dq10rly/DQ10rly.ioc in CubeMX -> Generate Code
git diff boards/dq10rly        # review exactly what regen changed
make build BOARD=dq10rly        # confirm it still compiles
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

[REGISTER-MAP.md](REGISTER-MAP.md) is the self-contained version for host
implementers: every command and IRQ sub-register with the firmware version it
appeared in, per board. Read it before assuming an opcode exists — unknown and
reserved-but-unimplemented commands answer `0xEE` filler that fails CRC, by
design ("absent means no claim").

Where the protocol is heading: [ROADMAP.md](ROADMAP.md) maps CiA 401 (the
CANopen device profile for generic I/O modules) onto the I2C-HATs — adopted,
decided, and proposed features in one prioritized list.

## Releasing / versioning

See [RELEASING.md](RELEASING.md). Short version:

```sh
make release-core KIND=patch          # core fix -> bump+build every board
make bump BOARD=di16ac KIND=minor     # one-board change
```

## Boards

Each board folder has a README with its specifics; the host-facing command
surface for all of them is [REGISTER-MAP.md](REGISTER-MAP.md).

| Board | MCU | Base address | Function |
|-------|-----|--------------|----------|
| di16ac       | STM32F042C6 (LQFP48) | 0x40 | 16 digital inputs (+IRQ) |
| di6acdq6rly  | STM32F042K6 (LQFP32) | 0x60 | 6 digital inputs (+IRQ) + 6 relay outputs |
| dq5rly       | STM32F042K6 (LQFP32) | 0x50 | 5 relay outputs |
| dq8rly       | STM32F042K6 (LQFP32) | 0x50 | 8 relay outputs — **prototype**, not a shipping board |
| dq10rly      | STM32F042K6 (LQFP32) | 0x50 | 10 relay outputs |
| ai4dcv10     | STM32G031K6 (LQFP32) | 0x70 (planned) | 4 analog channels (V/I/P via INA228) — **in development** |

The low nibble of the address comes from the board's address jumpers, read
once at startup.

> The F3 analog board (the `ai3tcdq4rly` branch) is **not** part of this
> monorepo. `core/` has since grown the family abstraction that was the
> blocker — `driver/stm32xx_ll.h`, proven by the G0 port — so bringing it in
> is now a matter of adding an F3 arm there plus a vendored `hal/` tree, not
> a redesign. `commands.h` already carries its `AI_*` opcodes.
