# ZeroPoint Mudlet Mapper + Room Inspector

## Mapper version 0.3.1 / Room UI version 0.4.0

ZeroPoint remains strict MSDP-only.

- `ROOM.VNUM` is the unique room identity.
- `ROOM.EXITS` is the only source of map links.
- Movement commands and room text have no mapping side effects.
- Coordinates are visual presentation only.
- If two distinct VNUMs would occupy the same visual coordinate, the later room
  is moved to the nearest free visual coordinate. The rooms remain separate and
  only their authoritative MSDP exits are linked.

## Room inspector

`ZeroPointRoomUI.lua` adds a dockable ZeroPoint window with three vertical zones:

1. the live Mudlet map;
2. room name/VNUM, area, terrain, description and visible windows;
3. a scrollable room-contents view grouped into people, ships, furniture and
   objects.

The room inspector does not scrape terminal `look` text. The server publishes
these fields inside the authoritative MSDP `ROOM` table:

- `DESCRIPTION`
- `WINDOWS`
- `PEOPLE`
- `SHIPS`
- `FURNITURE`
- `OBJECTS`

The contents tables are snapshots of the server-owned room state at ROOM
publication time and are intended as the foundation for later item/shop
identity and inspection support.

## Commands

Mapper:

- `zpmap start`
- `zpmap stop`
- `zpmap status`
- `zpmap here`
- `zpmap debug`
- `zpmap clear`
- `zpmap clear confirm`
- `zpmap help`

Room inspector:

- `zproom show`
- `zproom hide`
- `zproom toggle`
- `zproom status`

After upgrading from mapper 0.3.0 or earlier, use `zpmap clear confirm` once and
rebuild the map so old overlapping placements do not remain in the Mudlet map
database.
