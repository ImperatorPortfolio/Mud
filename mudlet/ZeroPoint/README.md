# ZeroPoint Mudlet Mapper

Starter mapper package for ImperatorPortfolio/Mud.

## Install
1. Open your Zero Point profile in Mudlet.
2. Open Package Manager (`Alt+O`).
3. Choose **Install New Package**.
4. Select `ZeroPoint.mpackage`.

## Use
- `zpmap start The Hague` — start mapping and issue `look`
- `zpmap stop` — stop automatic mapping
- `zpmap status` — show current mapper state
- `zpmap here` — center Mudlet's mapper
- `zpmap help` — command help

While mapping, normal movement commands (`n`, `ne`, `e`, etc.) are observed and sent to the MUD.
The mapper parses Zero Point's `Obvious exits:` block and the `Health:x/x Movement:x/x >` prompt,
creates rooms in Mudlet's native mapper database, links exits, and adds exit stubs for unexplored exits.

## Version
0.1.0

## Current limitations
- Failed movement is not yet explicitly detected.
- Loops reached for the first time through a previously-unmapped exit can create a duplicate room.
- Special/non-cardinal exits are not yet mapped automatically.
- This first version targets the Zero Point room/prompt format currently in use.
