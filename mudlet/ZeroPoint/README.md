# ZeroPoint Mudlet Mapper

## Version 0.3.0 — strict MSDP-only mapping

ZeroPoint Mapper no longer parses room text and no longer intercepts movement commands for mapping. The server's MSDP `ROOM` table is the only mapping authority.

### Authoritative contract
- `ROOM.VNUM` = unique Mudlet room ID
- `ROOM.NAME` = room title
- `ROOM.AREA` = mapper area
- `ROOM.TERRAIN` = terrain metadata
- `ROOM.EXITS` = exact one-way direction -> destination VNUM topology

The client never synthesizes reverse exits. Existing local standard exits that disagree with the current server packet are removed.

The server publisher now excludes invalid/unloaded exits, hidden exits, windows, and `xAUTO` keyword exits from normal cardinal topology.

### Commands
- `zpmap start` — start MSDP-only mapping
- `zpmap stop` — stop mapping
- `zpmap status` — show whether authoritative MSDP ROOM data is active
- `zpmap here` — center on the authoritative current room
- `zpmap debug` — show the exact last authoritative room packet and exits
- `zpmap clear` — show destructive map-clear warning
- `zpmap clear confirm` — delete the entire Mudlet map database
- `zpmap help`

### Clean rebuild after older mapper versions
1. Install the v0.3.0 package.
2. Enable MSDP in the Mudlet profile and reconnect.
3. Run `zpmap clear` and then `zpmap clear confirm`.
4. Run `zpmap start`.
5. Walk normally. Movement commands have no mapper side effects.
6. If a room looks wrong, run `zpmap debug`; the displayed exits are exactly what the plugin received from the server.

If MSDP ROOM data is unavailable, the mapper maps nothing rather than guessing.
