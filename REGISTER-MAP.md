# I2C-HAT command/register map — with introduction versions

Self-contained reference for host implementers — the whole command surface
with the firmware version each piece appeared in. Derived from
`core/commands.h`, `PROTOCOL.md`, `CHANGELOG.md` and `ROADMAP.md`; when a
release changes the command set, update this file in the same commit
(RELEASING.md daily workflow). Last regenerated 2026-08-11.

Versions are **per board and independent** (SemVer against the I2C command
contract). "always" below means: present in every version of that board's
released firmware line.

## Boards

| Board | Address base + jumper offset | Modules | Current released FW | Notes |
|---|---|---|---|---|
| di16ac | 0x40 | DI×16, IRQ line | **3.0.0** | |
| dq5rly | 0x50 | DO×5 (relays) | **1.2.0** | own 1.x line |
| dq8rly | 0x50 | DO×8 | 2.2.0 | **prototype — do not target** |
| dq10rly | 0x50 | DO×10 | 2.3.0 | built/released, never hardware-validated |
| di6acdq6rly | 0x60 | DI×6, DO×6 (relays), IRQ line | **3.0.0** | |
| ai4dcv10 | 0x70 (planned) | AI×4 (INA228), STM32G0 | unreleased, in development | first G0 board |

## Transport (identical on all boards)

- I2C slave ~100 kHz, clock stretching REQUIRED (hard ceiling 50 ms, Pi kernel
  aborts >35 ms — retry is always safe).
- Frame (both directions): `[ID, CMD, payload..., CRC16lo, CRC16hi]`.
  CRC-16/MODBUS over ID+CMD+payload. Response echoes ID+CMD (host must check).
  Minimum frame 4 bytes. Host keeps ID in 0x01..0x7F.
- Two steps: STOP-terminated command write, then response read. Over-reading
  returns 0xEE filler (fails CRC by design). **Unknown/unimplemented commands
  also answer 0xEE filler** — "absent means no claim".
- Channel data travels as u32-LE bitmasks (bit N = channel N). SET commands
  echo the written value; mismatched echo = did not take.
- No write→read delay needed since: di16ac 2.1.3, di6acdq6rly 2.1.4,
  dq10rly 2.1.2, dq5rly 1.0.1, dq8rly 2.0.2. (Older: keep ~1 ms gap.)
- Atomic combined transfers (write+Sr+read, one transaction) since:
  di16ac 2.2.0, di6acdq6rly 2.2.0, dq10rly 2.2.0, dq5rly 1.1.0, dq8rly 2.1.0.
- Communication watchdog: ANY valid frame feeds it. On timeout: DO channels
  load safety values per mask, status bit 0x08 sets, and (fw ≥ 3.0.0) the
  IRQ block disarms.

## System commands (all boards)

| Opcode | Command | Since | Notes |
|---|---|---|---|
| 0x10 | GET_BOARD_NAME | always | e.g. `"DI6acDQ6rly I2C-HAT"` |
| 0x11 | GET_FIRMWARE_VERSION | always | 3 bytes major, minor, patch |
| 0x12 | GET_STATUS_WORD | always | bits: 0x01 POR, 0x02 soft reset, 0x04 IWDG reset, 0x08 CWDT tripped (cleared when read), 0x10 IRQ capture queue full (DI boards) |
| 0x13 | RESET | always | software reset |
| 0x14 / 0x15 | CWDT_SET_PERIOD / CWDT_GET_PERIOD | always | u32 ms, 0 disables, persistent |
| 0x16 / 0x17 | IRQ_GET_REG / IRQ_SET_REG | DI boards, always | sub-register access, payload `[reg, u32-LE]` — see IRQ block |
| 0x18 | RESTORE_FACTORY_DEFAULTS | di16ac 2.3.0, di6acdq6rly 2.3.0, dq10rly 2.3.0, dq5rly 1.2.0, dq8rly 2.2.0 | CiA 301 0x1011; only with ASCII payload `"load"`; formats EEPROM + resets |
| 0x19 | ENTER_BOOTLOADER | G0 boards only (ai4dcv10, unreleased) | `"boot"`-guarded; re-enumerates at ROM addr 0x56. **Not implemented on any F0 board** |

## Digital inputs (di16ac: 16 ch, di6acdq6rly: 6 ch)

| Opcode | Command | Since | Notes |
|---|---|---|---|
| 0x20 | DI_GET_VALUE | always | bitmask. **Behavior change: < 3.0.0 this read released the IRQ line (could strand queued captures); ≥ 3.0.0 it never touches the line** |
| 0x21 | DI_GET_CHANNEL_STATE | always | `[index]` → `[index, state]` |
| 0x22 | DI_GET_COUNTER | always | `[index, type]`, type 0=falling 1=rising → u32 |
| 0x23 | DI_RESET_COUNTER | always | |
| 0x24 | DI_RESET_ALL_COUNTERS | always | |
| 0x25 | DI_GET_COUNTERS_STATUS | **reserved, NOT implemented** | responds as unknown (0xEE) |
| 0x26–0x29 | DI encoder commands | **reserved, NOT implemented** | opcodes exist in the enum only |
| 0x2A / 0x2B | DI_SET/GET_CHANNEL_FILTER | di16ac 2.3.0, di6acdq6rly 2.3.0 | CiA 401 0x6003. Per channel, ms 1..65535, persistent, default 2 ms. Payload `[index, u32 ms]` |
| 0x2C / 0x2D | DI_SET/GET_POLARITY | di16ac 2.3.0, di6acdq6rly 2.3.0 | CiA 401 0x6002. Per-bit invert BEFORE the filter, persistent, default 0. Writing re-seats debouncers without counting an edge |

## IRQ block (DI boards; sub-registers via 0x16/0x17)

| Sub-reg | Name | Since | Notes |
|---|---|---|---|
| 0x20 | DI_FALLING_EDGE_CONTROL | always (queue semantics since x.1.0 of each 2.x line) | CiA 401 0x6008. Per-bit mask. **Volatile < 3.0.0 (0 after reset, re-arm each boot); EEPROM-persistent ≥ 3.0.0** |
| 0x21 | DI_RISING_EDGE_CONTROL | same | CiA 401 0x6007. Same persistence rule |
| 0x22 | DI_CAPTURE | since the 2.1.0 capture queue | read → one queue entry `(states << 16) \| edge_status`, 0 = empty (a real entry is never 0); write 0 clears the queue. Queue depth 128, oldest dumped on overflow |
| 0x23 | DI_GLOBAL_ENABLE | **3.0.0 only** | CiA 401 0x6005. Volatile arming bit, reads 0 after every reset. Write 1 to arm, 0 to disarm (queue dumped + line released in one transaction, masks untouched). **On < 3.0.0 firmware this sub-reg answers 0xEE (unknown)** |

IRQ line semantics (active-low, open-drain wired-OR across stacked boards,
Pi GPIO21 with pull-up; treat as a LEVEL, check it before sleeping):

- **< 3.0.0**: edges captured whenever a mask bit is set. Line asserted on
  capture, released when the queue drains — but also released (wrongly) by any
  DI_GET_VALUE read.
- **≥ 3.0.0**: edges captured ONLY while DI_GLOBAL_ENABLE=1. Line asserted iff
  armed AND queue non-empty; nothing else touches it. CWDT trip disarms the
  block. **BREAKING: hosts that only set masks see no captures until they arm
  0x23.** Edges are taken post-filter/post-polarity.

## Digital outputs (dq5rly: 5, dq8rly: 8, dq10rly: 10, di6acdq6rly: 6)

| Opcode | Command | Since | Notes |
|---|---|---|---|
| 0x30 / 0x31 | DO_SET/GET_POWER_ON_VALUE | always | persistent, loaded at boot |
| 0x32 / 0x33 | DO_SET/GET_SAFETY_VALUE | always | persistent; loaded on CWDT trip per the mask (CiA 0x6207) |
| 0x34 / 0x35 | DO_SET/GET_VALUE | always | bitmask (CiA 0x6200) |
| 0x36 / 0x37 | DO_SET/GET_CHANNEL_STATE | always | single channel |
| 0x38 / 0x39 | DO_SET/GET_POLARITY | di6acdq6rly 2.3.0, dq10rly 2.3.0, dq5rly 1.2.0, dq8rly 2.2.0 | CiA 401 0x6202. Per-bit invert at the pin for EVERY state source (writes, safety, power-on); bus value stays logical. Persistent, default 0. Writing flips live pins instantly |
| 0x3A / 0x3B | DO_SET/GET_SAFETY_MASK | same versions | CiA 401 0x6206. Trip applies `(value & ~mask) \| (safety & mask)`. Persistent, default all-ones (every channel loads safety value) |

## Analog inputs (0x40–0x48)

Reserved for the AI family (ai4dcv10, unreleased). Not implemented on any
released board — all answer 0xEE.

## CiA 401/301 cross-reference

| CiA object | Command(s) | Status |
|---|---|---|
| 0x6000 read input | 0x20 | adopted |
| 0x6002 input polarity | 0x2C/0x2D | adopted (2.3.0 series) |
| 0x6003 input filter | 0x2A/0x2B | adopted (2.3.0 series) |
| 0x6005 global IRQ enable | IRQ reg 0x23 | adopted (3.0.0 series) |
| 0x6006 IRQ mask any-change | — | n/a by design (= rising OR falling) |
| 0x6007 / 0x6008 IRQ masks | IRQ regs 0x21 / 0x20 | adopted, persistent (3.0.0 series) |
| 0x6200 write output | 0x34–0x37 | adopted |
| 0x6202 output polarity | 0x38/0x39 | adopted (CiA series) |
| 0x6206 error mode output | 0x3A/0x3B | adopted (CiA series) |
| 0x6207 error value output | 0x32/0x33 | adopted |
| 0x6208 output write mask | — | proposed (roadmap rank 2) |
| 0x1011 restore defaults | 0x18 `"load"` | adopted (CiA series) |
| 0x1020 verify configuration | — | proposed (roadmap rank 1) |
| 0x1001-ish error register | 0x12 status word | adopted |

## Host library

Python `raspihats` **3.0.0** on PyPI covers everything above
(`di.polarity`, `di.filters[i]`, `di.irq_reg.*` incl. `global_enable`,
`dq.polarity`, `dq.safety_mask`, `restore_factory_defaults()`).
Retry policy: 5× with 10 ms backoff on I/O error / bad CRC / ID-CMD mismatch.
