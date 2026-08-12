# Roadmap - aligning the I2C-HATs with CiA 401

CiA 401 is the CANopen device profile for generic I/O modules: decades of
settled semantics for exactly the features a digital I/O board grows over
its life. The raspihats I2C-HAT firmware already follows it in several
places; this document maps the profile's digital object set onto the
framed I2C protocol - what is adopted, what is decided, what is proposed -
so the firmware roadmap is one list to pick from. Prokop-side notes
describe what the controller does (or would do) with each piece.

Conventions assumed throughout, from the existing protocol:

- Commands are framed `[id, cmd, ...data, crc_lo, crc_hi]`, CRC16-Modbus
  over `[id, cmd, ...data]`. `id` is the sequence field tying a command to
  its response; SET commands echo the value back, and a mismatched echo
  means "did not take".
- Channel state travels as u32-LE bitmasks, bit = channel.
- Persistent registers get a GET/SET opcode pair and live in flash. The
  controller reconciles them read-compare-write at activation, so an
  unchanged project performs zero flash writes.
- A capability exists on a board if (and only if) its catalog descriptor
  declares the field - "absent means no claim". The controller never sends
  a command the descriptor did not promise the firmware answers.

CiA 401 defines each object in 8-bit granularity with 1-bit and
16/32-bit access variants at neighbouring indices; the mapping here is to
the canonical 8-bit objects, since the HATs move 32-bit masks regardless.

## Digital outputs

| CiA 401 | name | I2C-HAT | status |
| --- | --- | --- | --- |
| `0x6200` | Write output | `DQ_SET_ALL_CHANNEL_STATES` 0x34 (bulk), single-channel 0x36/0x37 (= the `0x6220` bit-access form), read-back 0x35 | **adopted** |
| `0x6207` | Error value output | `DQ_SET/GET_SAFETY_VALUE` 0x32/0x33, loaded by the board on a CWDT trip | **adopted** |
| `0x6206` | Error mode output | `DO_SET/GET_SAFETY_MASK` 0x3A/0x3B - per bit: 1 = load the error value on a trip, 0 = hold last state | **adopted** (1.2.0/2.2.0/2.3.0 series) |
| `0x6202` | Change polarity output | `DO_SET/GET_POLARITY` 0x38/0x39 - persistent per-bit invert, applied by firmware between process value and pin | **adopted** (1.2.0/2.2.0/2.3.0 series) |
| `0x6208` | Filter mask output | `DO_SET/GET_WRITE_MASK` 0x3C/0x3D - gates bulk writes only, volatile (all-ones after reset) | **adopted** (3.1.0 series) |
| - | (no CiA analogue) | `DQ_SET/GET_POWER_ON_VALUE` 0x30/0x31, applied at power-up | **adopted** |

Notes per object:

- **`0x6206` error mode.** CiA defaults: mode `FFh` (every bit applies the
  error value), value `0h` - de-energize everywhere, which is also
  Rockwell's and Beckhoff's default posture. The HAT keeps those defaults.
  Until this register exists, hold-last-state is all-or-nothing per board
  (CWDT off entirely); the register is what makes it per-channel. With it,
  every channel gets the three-way choice one UI control can render:
  de-energize (mode 1, value 0), hold (mode 0), energize (mode 1, value 1).
- **`0x6202` polarity.** Firmware applies it at the pin for EVERY source of
  output state - process writes, the error value on a trip, the power-on
  value at boot - so everything on the wire stays logical, CiA 401's own
  model. Default 0 (no inversion). One commissioning hazard to respect:
  writing this register flips live pins instantly, so it is an engineering
  act - the controller reconciles it at activation, never during RUN.
- **`0x6208` filter mask** - shipped in the 3.1.0 series. The design
  record is kept in full below, since the reasoning (what the mask gates
  and what it deliberately does not) is the part worth re-reading.

  *Concept.* A per-bit mask answering one question: which channels does a
  BULK output write affect? Bit set = the channel obeys `DO_SET_VALUE`;
  bit clear = bulk writes flow around it, whatever value they carry in
  that bit position. Without it a controller drives unmapped channels to 0
  on every bulk write - the port is all-or-nothing. With
  the mask, an unmapped channel is genuinely nobody's: the controller
  owns its channels, a test rig / manual tool / second process owns the
  rest via single-channel writes, and neither stomps the other. It sat in
  the drawer until one board's outputs really had two masters, then shipped
  with the rest of the host-convenience bundle.

  *Checked against a fielded DS401 implementation (Ascon Tecnologic
  IO-CB/DO-04RL manual, 2026-08-11):* it documents 6208h as a PREPROCESS
  of the 6200h write path ("two preprocess items are performed:
  polarisation 6202h, masking 6208h"; 0 = "the received output value is
  neglected ... and the old output value is kept", default FFh), routes
  error mode/value through a separate path the mask does not touch, and
  even exempts its proprietary pulse-write function from polarity and
  filter mask - real-world precedent for exempting non-bulk write paths.
  Whether the spec text itself gates the 1-bit objects (6220h) remains
  unverified from public sources; consult the CiA 401 document for the
  letter of it.

  *Settled design (recorded 2026-08-11).* Orthogonal masks, one job
  each - no double meanings:
  - `0x6208` gates BULK process writes (0x34) only - CiA's strict
    reading of the filter-mask object.
  - `0x6206` (already shipped) alone decides per-channel CWDT-trip
    behavior; a masked-out-of-writes channel can still be forced safe
    by a trip if its 0x6206 bit says so. Safety stays safety.
  - The power-on value stays global (whole-board boot posture).
  - Single-channel writes (0x36) BYPASS the mask - they are exactly the
    mechanism the other owner uses.

  *Implemented (3.1.0 series, 2026-08-11).* Opcode pair
  `DO_SET/GET_WRITE_MASK` 0x3C/0x3D; the bulk path applies
  `effective = (current & ~mask) | (written & mask)`. **VOLATILE, not
  persistent** - all-ones after every reset (DS401 practice: fielded
  devices exclude 6208h from non-volatile storage). Rationale: the mask
  is a runtime ownership arrangement, not commissioning - at reset the
  controller reclaims the whole port at a known baseline, a stale mask
  cannot leave phantom-dead channels across power cycles, and it stays
  out of the 0x1020 signature by construction. `DO_SET_VALUE` validates
  and echoes the RECEIVED value (the command took; masked bits were
  processed per the mask), so an oblivious bulk-writing host keeps its
  echo verification; `DO_GET_VALUE` reads the truth.
- **The CWDT itself** is the CiA 301 piece the error block hangs off:
  node-guarding/heartbeat by another name (`CWDT_SET/GET_PERIOD`
  0x14/0x15, fed by any successful transaction, trips into `0x6207`'s
  value per `0x6206`'s mask).

## Digital inputs

| CiA 401 | name | I2C-HAT | status |
| --- | --- | --- | --- |
| `0x6000` | Read input | `DI_GET_ALL_CHANNEL_STATES` 0x20 (bulk) | **adopted** |
| `0x6003` | Filter constant input | `DI_SET/GET_CHANNEL_FILTER` 0x2A/0x2B - persistent per-channel filter time in ms | **adopted** (2.3.0 series) |
| `0x6002` | Change polarity input | `DI_SET/GET_POLARITY` 0x2C/0x2D - persistent per-bit invert, applied before the debouncer | **adopted** (2.3.0 series) |
| `0x6005` | Global interrupt enable | IRQ reg 0x23 via `IRQ_SET/GET_REG` 0x17/0x16 - volatile arming bit, 0 after every reset; disarm dumps the queue and releases the line | **adopted** (3.0.0 series) |
| `0x6006` | Interrupt mask, any change | any-change is rising OR falling per bit - a second spelling of `0x6007`+`0x6008` | n/a (derived, settled) |
| `0x6007` | Interrupt mask, low-to-high | IRQ reg 0x21 rising edge control - persistent | **adopted** (3.0.0 series) |
| `0x6008` | Interrupt mask, high-to-low | IRQ reg 0x20 falling edge control - persistent | **adopted** (3.0.0 series) |

Notes per object:

- **`0x6003` filter constant** is ranked first because of the AC input
  boards: an opto input on 50/60 Hz mains ripples at line frequency, and
  whether that reads as a clean TRUE or as chatter is a filtering
  decision. The alignment makes the filter time a configurable persistent
  register (unit: ms; per channel or per 8-channel group - firmware's
  choice) instead of a build-time constant, so a fast DC proximity switch
  and a slow AC contactor feedback coexist on one board. Controller side
  it is one more field in the reconcile.
- **The interrupt block** shipped in the 3.0.0 series with these decided
  semantics: the line is derived from the capture queue alone - asserted
  if and only if the block is armed and captures pend, so draining (or
  disarming) releases it and a plain DI read never does. The edge masks
  are persistent commissioning; the global enable is a volatile arming
  bit the controller sets after every reconcile, and a CWDT trip disarms
  the block (masks untouched) so a dead controller is never held on the
  line. Controller side: sleep on the GPIO but treat it as a LEVEL -
  captures stored while the line is already low make no new edge, so
  check the line before sleeping and drain every armed board on wake.
  Bench-validated 2026-08-11 (`DI6acDQ6rly.irq_block.robot`, 6/6).

## Device level (CiA 301 support objects)

| CiA | name | I2C-HAT | status |
| --- | --- | --- | --- |
| `0x1001` + EMCY | Error register | `GET_STATUS_WORD` 0x12: PORRST 0x01, SFTRST 0x02, IWDGRST 0x04, CWDT tripped 0x08; cleared when read | **adopted** |
| `0x1011` | Restore default parameters | `RESTORE_FACTORY_DEFAULTS` 0x18, acts only on the "load" signature; formats the EEPROM and resets | **adopted** (1.2.0/2.2.0/2.3.0 series) |
| `0x1020` | Verify configuration | `CONFIG_SET/GET_SIGNATURE` 0x1A/0x1B - controller-owned persistent u32, voided by the firmware when any other persistent value really changes; steady-state reconcile becomes one read | **adopted** (3.1.0 series) |
| `0x1010` | Store parameters | not needed - each SET persists immediately (different model, settled) | n/a |

Notes per object:

- **`0x1011` restore defaults.** The persistent block keeps growing (CWDT
  period, safety value, power-on value, then mask, polarity, filter
  constants), and the motivating scenario is already real: a HAT arrives
  from another machine with someone else's commissioning in flash. The
  existing `reset` 0x13 is a software reset, not a parameter restore.
  Worth adopting CiA's guard as well: `0x1011` only acts when the
  signature `"load"` (0x64616F6C) is written - a magic payload in the
  framed command prevents an accidental factory reset from a stray write.
  Pairs naturally with the planned "prove the safe state" commissioning
  action.
- **`0x1020` verify configuration.** CiA stores the date+time of the last
  configuration; the adoption is any stored signature the controller writes
  after a successful reconcile. Steady state then checks ONE register
  instead of reading every persistent value back, falling back to the
  full per-register reconcile on mismatch. Grows in value with every
  register the block gains.

  *Implemented (3.1.0 series, 2026-08-11).* The firmware's half of the
  contract is invalidation: `Eeprom::Write` voids the signature to 0 from
  a single choke point, placed AFTER the skip-if-unchanged guard so an
  identical re-write of some other register does not cost the controller
  its claim. `RESTORE_FACTORY_DEFAULTS` wipes it with everything else, and
  0 means "no claim" - a board that has never been commissioned, or one
  whose configuration moved under the controller's feet, reads the same.
  The value itself is opaque to the firmware: the controller picks what it
  means (project hash, timestamp, revision counter).

## Priority

| rank | piece | why |
| --- | --- | --- |
| shipped | `0x6206` + `0x6202` + `0x6003` + `0x6002` + `0x1011` | the 1.2.0 / 2.2.0 / 2.3.0 firmware series |
| shipped | `0x6005` + `0x6007`/`0x6008` + IRQ line semantics | the 3.0.0 series (DI boards; `0x6006` settled as n/a) |
| shipped | `0x1020` config signature + `0x6208` output write mask | the 3.1.0 series (with `ENTER_BOOTLOADER` 0x19, which is not a CiA object) |

**The digital half of CiA 401 is fully adopted - this list is empty.**
Every object in the digital tables above is either implemented or settled
as n/a with the reason recorded; nothing is parked in the drawer.

The analog half (`0x6401`/`0x6411` values, scaling, limit interrupts) is
the next campaign, and the same exercise: it starts when the AI board's
hardware and its INA228 driver exist (see `boards/ai4dcv10/README.md`).
Nothing above depends on it.
