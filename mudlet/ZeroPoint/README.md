# ZeroPoint Mudlet Mapper

Version 0.2.0 adds server-authoritative MSDP mapping.

## Recommended setup
1. Install `ZeroPoint.mpackage` in Mudlet (`Alt+O`).
2. In Mudlet Preferences, enable **MSDP** for the profile.
3. Reconnect so Mudlet can negotiate MSDP with Zero Point.
4. Type `zpmap start`.

## Commands
- `zpmap start [area]`
- `zpmap stop`
- `zpmap status`
- `zpmap here`
- `zpmap help`

## Mapping model
When MSDP is available the server publishes the standard `ROOM` table with:
- `VNUM` — authoritative unique room identity
- `NAME`
- `AREA`
- `TERRAIN`
- `EXITS` — abbreviated direction -> destination VNUM

The package uses server VNUMs as Mudlet room IDs. This prevents drift from failed movement,
teleports, scripted transfers, loops and repeated room names. Coordinates remain client-side
and are laid out as rooms are discovered.

If MSDP is unavailable, the v0.1 text parser remains as a fallback.
