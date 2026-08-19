# ZeroPoint Mudlet Mapper

## Version 0.2.1

The mapper prefers authoritative MSDP `ROOM` data from the Zero Point server.

### Fix in 0.2.1
- Server `ROOM.EXITS` is treated as the only authority for normal outgoing exits.
- The mapper no longer invents reverse exits.
- When a room is revisited, stale local cardinal exits that are absent from MSDP are deleted.
- Existing destination VNUMs remain the Mudlet room IDs.
- Text parsing remains fallback-only when MSDP has not been received.

### Use
- `zpmap start`
- `zpmap stop`
- `zpmap status`
- `zpmap here`
- `zpmap help`

Enable MSDP in the Mudlet profile and reconnect before starting the mapper.

Existing incorrect links from v0.2.0 are repaired as their source rooms are revisited and receive authoritative MSDP data.
