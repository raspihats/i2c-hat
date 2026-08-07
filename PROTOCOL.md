# I2C-HAT communication protocol

How a host (typically a Raspberry Pi) talks to an I2C-HAT board. This is the
transport contract implemented by `core/driver/i2c_port.cpp` +
`core/frame/`; the command opcodes themselves live in `core/commands.h`.

## Physical layer

- I2C slave, 7-bit addressing, ~100 kHz, **clock stretching required** (see
  [Clock stretching](#clock-stretching) — a master that cannot stretch cannot
  talk to these boards).
- Address = board-family base + jumper-selected offset (low nibble):

| Family | Base address |
|---|---|
| di16ac | `0x40` |
| dq5rly / dq8rly / dq10rly | `0x50` |
| di6acdq6rly | `0x60` |

## Frame format

Requests and responses share one framing:

```
byte 0      ID       frame id, echoed back in the response
byte 1      CMD      command opcode (core/commands.h)
byte 2..N   payload  command-specific, may be empty
last 2      CRC16    over ID+CMD+payload, little-endian (low byte first)
```

- CRC is **CRC-16/MODBUS**: poly `0x8005` reflected (`0xA001` table), init
  `0xFFFF`, no final XOR.
- Minimum frame is 4 bytes (ID + CMD + CRC). Firmware buffers are 256 bytes.
- The response **echoes the request's ID and CMD** — this is the host's proof
  that the response belongs to *this* request and not a previous one. Always
  check it (see [Errors and retries](#errors-and-retries)).
- Convention: hosts keep ID in `0x01..0x7F` (the raspihats library masks to
  7 bits to sidestep an old Raspberry Pi MSb corruption bug).

## Transaction model

Every exchange is **two steps**:

```
1. command write:    S addr+W  [ID CMD payload CRC]  P        (STOP-terminated)
2. response read:    S addr+R  [ID CMD payload CRC]  NA P
```

The rules, exactly as the firmware implements them:

1. **Commands must arrive in STOP-terminated writes.** The STOP is what tells
   the firmware "the frame is complete — decode it". Response preparation
   happens immediately after.
2. **The write part of a combined (repeated-START) transfer is discarded.**
   `read_i2c_block_data`-style reads (`S addr+W cmd Sr addr+R ...`) carry a
   mandatory SMBus command byte; the firmware throws it away and serves the
   staged response. Send `0xFF` as that byte. This is why the smbus2 path
   works unchanged.
3. Consequently, **a real command sent inside a combined transfer is silently
   dropped**. What the read part then returns depends on whether a staged
   response is still unconsumed: normally there is none (responses are consumed
   by reading them, rule 5), so the read yields `0xEE` filler — a standalone
   `write-cmd + Sr + read` transaction always comes back as `0xEE`. Only if the
   previous response was never read does it come back instead. Either way the
   frame check catches it (bad CRC / wrong ID): don't send commands combined.
4. **No delay is needed between the write and the read.** The firmware
   finalizes the command even when the read arrives back-to-back with the
   write's STOP; the pending read is paced by hardware clock stretching until
   the fresh response is staged. This holds from the following firmware
   versions (query with `GET_FIRMWARE_VERSION`, `0x11`):

   | Board | No-delay since |
   |---|---|
   | di16ac | 2.1.3 |
   | di6acdq6rly | 2.1.4 |
   | dq5rly | 1.0.1 |
   | dq8rly | 2.0.2 |
   | dq10rly | 2.1.2 |

   Older firmware has a race that drops the command when the read follows
   within ~10 µs — hosts talking to older versions should keep a ~1 ms gap
   between write and read.
5. **Over-reading yields `0xEE` filler bytes.** The response is consumed by
   reading it: a second read without a new command returns `0xEE` padding
   (which fails CRC — by design).

Both plain I2C reads (`ioctl(I2C_RDWR)`, raw `read()`) and SMBus combined
reads work for step 2; the raspihats Python library uses
`write_i2c_block_data` + `read_i2c_block_data` (rule 2), C drivers typically
use two plain transactions.

## Clock stretching

The firmware is a polled slave: it holds SCL low (hardware ADDR stretch) from
the moment the response read arrives until the response is staged. Expected
stretch durations:

| Situation | Stretch |
|---|---|
| Any GET, or a SET that doesn't change the stored value | microseconds |
| SET that changes an EEPROM-backed value (CWDT period, DO power-on/safety) | 250–500 µs (flash program) |
| EEPROM page transfer (fills up after ~125 *changed* values — effectively never) | 20–45 ms |

A master must tolerate stretching. The Raspberry Pi kernel driver aborts any
transfer stretched beyond **35 ms** (`i2c-bcm2835` CLKT) — only the
page-transfer case can hit that, and a single retry then always succeeds
(the flash operation has finished by the time it's issued).

**Hard ceiling: 50 ms.** The firmware arms the I2C peripheral's SCL-low
timeout: if any stretch exceeds ~50 ms — which no legitimate operation does —
the hardware releases the bus on its own and the slave resets its transfer
state. This bounds the bus hold even if the firmware crashes mid-transaction
(or is halted by a debugger); the interrupted transfer fails and should simply
be retried.

## Errors and retries

A robust host treats each of these as "retry the whole write+read exchange":

- read returns an I/O error (stretch timeout, NAK);
- response CRC check fails;
- response ID or CMD doesn't echo the request.

The raspihats Python library retries 5× with 10 ms backoff; that policy is a
good default. A NACK on the last read byte is *normal* I2C (it's how the
master signals "done") — not an error.

## Communication watchdog

Every valid decoded frame — any command — feeds the board's communication
watchdog (`CWDT_SET_PERIOD` configures it; 0 disables). If the configured
period elapses with no valid frame, digital outputs drop to their safety
values and the status word flags the timeout. Polling anything, e.g.
`GET_STATUS_WORD`, is sufficient keep-alive.

## Raspberry Pi notes

- **Old Pi models / kernels (BCM2835 BSC, Pi 1/2/3 era):** the hardware master
  mis-samples when a slave stretches immediately after the read address —
  exactly this firmware's pattern. If you see intermittent CRC failures,
  lower the bus speed: `dtparam=i2c_arm_baudrate=50000` (or 10000) in
  `config.txt`.
- **All Pis, mainline kernels (~4.13+):** 35 ms hard stretch limit per
  transfer (see above). Expect a rare, self-healing retry on EEPROM-writing
  commands.
- The 32-byte SMBus block limit applies only to the `read_i2c_block_data`
  path, not to plain I2C reads.
