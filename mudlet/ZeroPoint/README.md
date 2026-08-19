# ZeroPoint Mudlet Mapper

## Version 0.3.1

ZeroPoint Mapper is strict MSDP-only.

- `ROOM.VNUM` is the unique room identity.
- `ROOM.EXITS` is the only source of map links.
- Movement commands and room text have no mapping side effects.
- Coordinates are visual presentation only.
- If two distinct VNUMs would occupy the same visual coordinate (for example a
  direct northeast exit versus east-then-north), the later room is moved to the
  nearest free visual coordinate. The rooms remain separate and only their
  authoritative MSDP exits are linked.
- Exit creation runs in deterministic direction order.
- `zpmap debug` displays the current authoritative exit table and reports any
  remaining coordinate overlap.

## Commands

- `zpmap start`
- `zpmap stop`
- `zpmap status`
- `zpmap here`
- `zpmap debug`
- `zpmap clear`
- `zpmap clear confirm`
- `zpmap help`

After upgrading from 0.3.0 or earlier, use `zpmap clear confirm` once and rebuild
the map so old overlapping placements do not remain in the Mudlet map database.
