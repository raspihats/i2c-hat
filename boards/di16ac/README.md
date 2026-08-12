# DI16ac I2C-HAT

16 opto-isolated AC digital inputs, with an IRQ line. **STM32F042C6, LQFP48** —
the only LQFP48 part in the fleet, which matters more than it sounds (below).

| | |
|---|---|
| Base I2C address | `0x40` + address-jumper offset (low nibble) |
| Modules | `DigitalInputs` (`USES_DIGITAL_INPUTS`) |
| Channels | 16 inputs, `DI0_Pin`..`DI15_Pin` via `main.h` user-labels |
| Extra status bit | `0x10` = IRQ capture queue full |
| CubeMX project | `DI16ac.ioc` (classic `Inc/` + `Src/` layout) |

Version lives in `board.h` (`FW_VERSION_*`) — the single source of truth the
firmware reports over I2C and the one `tools/bump.sh` / `tools/tag.sh` read.
Release history: `git tag -l 'di16ac-*'`.

```sh
make build BOARD=di16ac
```

## Board-specific notes

- **BOOT0 is PF11 here, not PB8.** The `ENTER_BOOTLOADER` (0x19) software entry
  drives the pad the ROM's boot selector shares with a GPIO, and the LQFP48
  package routes it differently from the LQFP32 parts. `board.h` declares it
  via `BOOT0_GPIO_PORT/PIN/PERIPH`; getting this wrong makes 0x19 a plain reset
  that silently lands back in the application — which is exactly what 3.1.0 did
  before 3.1.1 fixed it. See `core/driver/bootloader.cpp` for why the pad drive
  is needed at all (the ROM's empty check, with the bench evidence).
- **No output module**, so a CWDT trip has no safety values to load — it sets
  the status word bit and (firmware ≥ 3.0.0) disarms the IRQ block.
- **This is the board to watch as the persistent register set grows.** The
  EEPROM layout allocates two virtual addresses per stored 32-bit variable
  (`core/driver/eeprom_emulation_conf.h`), and 16 channels of per-channel
  filter constant take 32 of them on their own — more than every other
  persistent register on the board put together. Still comfortable against a
  page that holds ~125 *changed* values, but this is where a future register
  block will bite first.
- The IRQ line is open-drain and wired-OR with any other DI board in the stack.
  Semantics (level, not edge) are in [REGISTER-MAP.md](../../REGISTER-MAP.md).
