# ZeroPoint Elevator System

`src/elevator.c` implements persistent, collective-control elevators.

## Player interaction

At a configured floor:

- `look panel` / `look button` shows the call panel and current car indicator.
- `push call` (also `push button`, `push elevator`, `push lift`, `push turbolift`) queues that floor.
- When the car arrives, a real two-way room exit is created between the floor and the car.

Inside the car:

- `look panel` lists floor choices, current floor, direction, state and queued requests.
- `push <floor>` or `push floor <floor>` queues a stop.
- `push open` opens the doors only while stationary.
- `push close` closes them immediately and resumes queued travel.

Non-elevator uses of the inherited `push` and `look` commands are preserved.

## Scheduling invariant

The car keeps a persistent `isRising` direction. While moving upward it services requested floors encountered above it in ascending order and never reverses because a lower floor was requested. When no request remains above, it reverses and services lower requests. Downward travel follows the inverse rule.

When idle, directional inertia is preserved when possible; otherwise the nearest outstanding request establishes the next direction.

Travel is simulated one configured floor at a time. The current implementation uses a two-second travel interval and leaves doors open for four seconds.

## World topology

Floor VNUMs and door directions are explicit configuration. The elevator never infers world topology from coordinates or player movement.

The car is one persistent room. Door exits exist only while the doors are open at the current floor. Opening or closing doors republishes authoritative MSDP `ROOM` data to players in the affected rooms so Mudlet receives the changed exit table immediately.

## First installation: Directorate City, The Hague

The initial installation replaces one legacy three-level static turbolift chain:

| Floor | Floor room | Replaced legacy lift room | Floor door | Label |
|---|---:|---:|---|---|
| 1 | 398 | 411 | northwest | Lower Directorate Street |
| 2 | 322 | 410 | northwest | Central Directorate Street |
| 3 | 358 | 409 | northwest | Upper Directorate Street |

Persistent car room: `427`.

The car-side doorway is `southeast`.

At initialization the subsystem removes only the legacy floor-to-shaft edges above and isolates room 427 from the old shaft before using it as the moving car.

## Adding another elevator

Add an `elevator_stop[]`, an `elevator_data`, and register the elevator in `all_elevators[]` in `src/elevator.c`. Each stop needs:

- logical floor number;
- actual floor room VNUM;
- old/static destination VNUM to remove, where applicable;
- direction from the floor into the car;
- player-facing floor label.

The car needs its own persistent room VNUM and the direction from the car out to a floor.

Keep the number of stops at or below the current request-array capacity of 16 unless the subsystem storage is expanded.

## Validation checklist

1. Clean build: `make clean && make`.
2. Stand at one configured floor and `look panel`.
3. `push call`; verify no car exit exists until arrival.
4. Enter the car once doors open and `look panel`.
5. Queue multiple floors both above and below the current floor.
6. Verify the car completes the current directional sweep before reversing.
7. Verify duplicate requests do not produce duplicate stops.
8. Verify `push open` is rejected while moving.
9. Verify door exits disappear when doors close and reappear only at the actual current floor.
10. With MSDP enabled, verify `ROOM.EXITS` changes immediately as doors open/close.
11. Outside the elevator, verify legacy `push <object>` and ordinary `look` behavior are unchanged.
