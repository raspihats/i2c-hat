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
| `0x6206` | Error mode output | `safety_mask` - per bit: 1 = load the error value on a trip, 0 = hold last state | **planned** (next firmware increment) |
| `0x6202` | Change polarity output | persistent per-bit invert register, applied by firmware between process value and pin | **decided 2026-08-08** |
| `0x6208` | Filter mask output | per-bit "which bits does a bulk write affect" | proposed (drawer) |
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
- **`0x6208` filter mask.** Today the controller drives unmapped channels
  to 0 on every bulk write; with this register an unmapped channel is
  genuinely untouched (a test rig or manual tool can own it). Decision to
  make when it is picked up: whether the mask gates only write commands
  (CiA's reading) or also the error/power-on value application.
- **The CWDT itself** is the CiA 301 piece the error block hangs off:
  node-guarding/heartbeat by another name (`CWDT_SET/GET_PERIOD`
  0x14/0x15, fed by any successful transaction, trips into `0x6207`'s
  value per `0x6206`'s mask).

## Digital inputs

| CiA 401 | name | I2C-HAT | status |
| --- | --- | --- | --- |
| `0x6000` | Read input | `DI_GET_ALL_CHANNEL_STATES` 0x20 (bulk) | **adopted** |
| `0x6003` | Filter constant input | persistent input filter/debounce time | **proposed - ranked first** |
| `0x6002` | Change polarity input | persistent per-bit invert, process value = pin XOR polarity | proposed (pairs with `0x6202`) |
| `0x6005` | Global interrupt enable | with the three masks below: event-driven inputs over the HAT's IRQ line | proposed |
| `0x6006` | Interrupt mask, any change | " | proposed |
| `0x6007` | Interrupt mask, low-to-high | " | proposed |
| `0x6008` | Interrupt mask, high-to-low | " | proposed |

Notes per object:

- **`0x6003` filter constant** is ranked first because of the AC input
  boards: an opto input on 50/60 Hz mains ripples at line frequency, and
  whether that reads as a clean TRUE or as chatter is a filtering
  decision. The alignment makes the filter time a configurable persistent
  register (unit: ms; per channel or per 8-channel group - firmware's
  choice) instead of a build-time constant, so a fast DC proximity switch
  and a slow AC contactor feedback coexist on one board. Controller side
  it is one more field in the reconcile.
- **The interrupt block** maps onto registers the firmware already half
  has (`irq_get_reg` 0x16 / `irq_set_reg` 0x17, currently unused by the
  controller) plus the HAT's IRQ line to a GPIO. With per-edge masks, the
  controller sleeps on the GPIO edge and reads inputs when it fires -
  input latency becomes one bus transaction instead of up to one poll
  period, without raising the poll rate for idle boards. Decisions to
  make: IRQ line semantics (asserted until served; cleared by the DI read
  itself, or by a dedicated ack), and behaviour when the controller is
  gone (the CWDT trip should probably clear the assertion). This is the
  largest joint firmware + controller effort on the list.

## Device level (CiA 301 support objects)

| CiA | name | I2C-HAT | status |
| --- | --- | --- | --- |
| `0x1001` + EMCY | Error register | `GET_STATUS_WORD` 0x12: PORRST 0x01, SFTRST 0x02, IWDGRST 0x04, CWDT tripped 0x08; cleared when read | **adopted** |
| `0x1011` | Restore default parameters | factory-reset opcode for the whole persistent block | **proposed - ranked second** |
| `0x1020` | Verify configuration | a stored config signature; steady-state reconcile becomes one read | proposed |
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

## Priority

| rank | piece | why |
| --- | --- | --- |
| in flight | `0x6206` safety mask + `0x6202` polarity | decided; sensibly ship in one firmware release |
| 1 | `0x6003` input filter constant | correctness on AC inputs, not a tuning nicety |
| 2 | `0x1011` restore defaults | the pre-armed-HAT scenario has already happened |
| 3 | `0x6005`-`0x6008` + IRQ line | biggest latency win, biggest joint effort |
| 4 | `0x1020` config signature | pays off more with every register added |
| 5 | `0x6208` output write mask | drawer, until shared-board outputs are real |

The analog half of CiA 401 (`0x6401`/`0x6411` values, scaling, limit
interrupts) is the same exercise for the day an AI/AQ board joins the
family; nothing here depends on it.
