# Releasing & versioning

## What the version means

Each board reports a 3-byte firmware version over I2C (`GET_FIRMWARE_VERSION`,
`0x11`), read by the host `raspihats` library. The whole command table in
`core/commands.h` is the **contract with the host**, so versions follow SemVer
against that contract:

| Bump  | Meaning to the host driver                         | Trigger |
|-------|----------------------------------------------------|---------|
| MAJOR | old driver may break                               | change/remove a command, change a response layout or the address scheme |
| MINOR | new optional capability; old driver still works    | *add* a command / register / channel type |
| PATCH | same interface, same intended behaviour, now correct | bug fix |

The version lives in `boards/<board>/board.h`:

```c
#define FW_VERSION_MAJOR (2)
#define FW_VERSION_MINOR (1)
#define FW_VERSION_PATCH (3)
```

Versions are **per board and independent** — `dq8rly 2.4.1` alongside
`di16ac 2.1.3` is normal. A shared-core change uses the *same kind* of bump on
every affected board, but each board increments its own counter.

## The daily workflow

**Core change (fans out to all boards):**

```sh
# 1. make the fix in core/, then:
make release-core KIND=patch          # bump ALL boards + build them all
make changelog BOARD=dq8rly MSG="I2C clock-stretch fix (core)"   # or edit CHANGELOG.md
git commit -am "core: I2C clock-stretch fix; bump all boards (patch)"
# 2. flash each board
```

`make release-core` bumps every board and runs `build-all`; if any board fails
to compile you find out before shipping. Pick `KIND` from the table above.

**One-board change:**

```sh
make bump BOARD=di16ac KIND=minor
make build BOARD=di16ac
make changelog BOARD=di16ac MSG="add DI encoder command"
git commit -am "di16ac: add DI encoder command (minor)"
```

Everything `make` does is a one-liner you could also run by hand — see the
`Makefile` and `tools/`. `tools/bump.sh` just rewrites the three `FW_VERSION_*`
lines; `tools/changelog.sh` reads the current versions from each `board.h` and
prepends a dated entry to `CHANGELOG.md`, so the changelog can't drift.

**Case matrix**

| Change | Boards bumped | Kind |
|--------|---------------|------|
| core bug fix | all | PATCH |
| core fix that changes host-visible behaviour | all | MAJOR |
| core new command | all | MINOR |
| board-specific bug fix | that board | PATCH |
| board-specific new command | that board | MINOR |
| new board | just it (start 1.0.0) | — |
| refactor, no behaviour change | none | no bump |

## Optional: tags & provenance

- Tag a shipped board when you want a marker: `git tag di16ac-v2.1.4`.
- To answer "which physical board runs which build", bake `git describe` into
  the binary and expose it via a future `GET_BUILD_ID` command (a MINOR change).

Neither is required for the daily loop; add them the day you miss them.

---

# Migration notes (monorepo, first pass)

This branch was assembled from the old per-board branches. The reconcile is
recorded here so the choices are auditable.

## What is shared vs per-board

- `core/` is the **STM32F0** shared C++ library used by all the F0 boards.
- `hal/` is the ST vendor HAL/LL + CMSIS, **one pinned copy shared by all boards**
  (the full LL set; `--gc-sections` drops what a board doesn't use). CubeMX still
  regenerates a per-board `boards/<name>/Drivers/`, but the build ignores it and
  it's git-ignored — see the "Regenerating" section in README.
- `middleware/eeprom/` is shared third-party C (ST EEPROM emulation, AN4061) that
  sits above HAL. Deduped because it's identical everywhere and *not* CubeMX-managed.
- `boards/<name>/` is now just the CubeMX **app**: `.ioc`, `Inc/`, `Src/`, startup,
  linker, `board.h`, `CMakeLists.txt`.

## `core/` file provenance

`core/` was assembled by taking the newest/canonical version of each file:

- **base + digital-input module + common drivers** — from `di16ac`
- **output stack** (`digital_outputs`, `relay_output_channel`, `digital_output_pwm`)
  — taken *together* from `dq10rly` so the APIs stay consistent. (An earlier
  attempt mixing `dq8rly`'s 2019 `relay_output_channel` with `dq10rly`'s 2026
  `digital_outputs.cpp` failed to compile — `RelayOutputChannel::Init/Tick`
  signatures had diverged. Taking the whole stack from one branch avoids that.)
- **`module/module.h`** — from `dq10rly` (has the `BYTES_TO_UINT32` paren fix).
- **`commands.h`** — from `ai3tcdq4rly` (richest superset of opcodes)
- **`i2c_hat.{h,cpp}`** — rewritten to be board-agnostic (module set via `board.h` macros)

Dropped / changed during the merge:

- Legacy `digital_output_channel` (superseded by `digital_output_pwm`; its only
  user, the `dq6od` board, was later dropped from the monorepo).
- `ll_utils` (only `dq8rly` had it, needed solely by its superseded PWM driver).
- `module.h` no longer includes `board.h` or `queue.h` (it used neither); the files
  that actually use board macros / the queue now include them directly. This broke
  a circular include that the old layout hid.
- `digital_inputs`/`digital_outputs` are wrapped in
  `#ifdef DIGITAL_(INPUT|OUTPUT)_CHANNEL_COUNT` so a module is compiled only when
  the board declares its channels.

## The F3 analog board (ai3tcdq4rly) is out of scope

`ai3tcdq4rly` (STM32F378 / Cortex-M4) is **not** included in this monorepo. It
remains on its `origin/ai3tcdq4rly` branch. The shared `core/` targets the F0
family (references `stm32f0xx_*` LL headers), so bringing the F3 board in needs an
MCU-family abstraction for those headers first. `commands.h` here is still the
superset that includes its `AI_*` opcodes, so the host contract stays stable for
when it rejoins.

## Build status — compile-verified

All 5 F0 boards were **compiled clean** with `arm-none-eabi-gcc 10.3` via
`make build-all` (flash 10–14 KB each, well within the 32 KB parts). What still
needs *you*:

1. **Hardware smoke-test each board.** Compiling is not running — flash one of
   each and confirm I2C, the status LED, and the IO behave.
2. **Vendor pack was standardized.** All boards now share `hal/`, taken from
   `dq5rly`'s newer CMSIS 5 / LL pack. The other four boards were previously on an
   older pack and compiled clean against the newer one — but a clock/peripheral
   smoke-test on real hardware is the real confirmation, since `system_stm32f0xx.c`
   (per board) now runs against newer CMSIS headers.
3. **`board.h` module wiring** for each board was derived from that board's old
   `i2c_hat` constructor; confirm the module set and the IRQ status bit
   (`di16ac`, `di6acdq6rly`) match the hardware.
3. **`dq5rly`'s linker script** had GCC11-only `(READONLY)` markers stripped so it
   links with GCC10 (the file's own comment recommends this); harmless on GCC11+.
4. **`dq10rly` vs `di6acdq6rly` `digital_outputs.cpp`** were edited the same day
   (2026-06); the `dq10rly` version was taken. Diff if they differed functionally.

## What was intentionally left untouched

- The old `master` branch and every per-board branch are preserved as-is; nothing
  was deleted upstream. This restructure is delivered as a PR onto `master`.
