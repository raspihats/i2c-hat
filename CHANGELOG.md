# Changelog

## 2026-08-08  core: atomic combined-transfer commands (write+Sr+read in one transaction); the SMBus dummy-byte path is unchanged, so smbus2 hosts are unaffected (see PROTOCOL.md)
Applied to: di16ac 2.2.0, di6acdq6rly 2.2.0, dq10rly 2.2.0, dq5rly 1.1.0, dq8rly 2.1.0


## 2026-08-07  core: EEPROM page-transfer data-loss fix; skip flash writes when value unchanged; fix command-drop race so hosts need no write-to-read delay; 50 ms hardware SCL-low timeout backstop; TIMINGR unified on higher data-setup margin; state-machine cleanup (see PROTOCOL.md)
Applied to: di16ac 2.1.3, di6acdq6rly 2.1.4, dq10rly 2.1.2, dq5rly 1.0.1, dq8rly 2.0.2


