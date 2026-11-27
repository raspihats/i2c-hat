# i2c-hat

- **Command design follows CANopen CiA 401** (CiA 301 for system commands,
  CiA 406 for counters). Before adding or changing any command, read and
  apply README.md "Rule: commands follow CANopen (CiA 401)", and update
  REGISTER-MAP.md (the command's CiA object + the cross-reference table +
  deviations) in the same commit. Verify object indices and data types
  against the spec or a vendor DS401 EDS/manual, never from memory.
- `core/` is shared with the i2c-io repo (forked at 03d4517): keep changes
  carryable in both directions.
