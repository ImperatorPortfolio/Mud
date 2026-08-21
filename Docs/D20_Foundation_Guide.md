# Local Sphere D20 Foundation Guide

This document defines the original Local Sphere baseline used to author and validate ordinary D20 content. It documents the mechanics implemented by this codebase; it does not reproduce text from any external tabletop rulebook.

## 1. Heroic scale

The inherited 0-100 progression maps to a 1-20 heroic scale in five-point bands:

- 1-5 = D20 level 1
- 21-25 = D20 level 5
- 46-50 = D20 level 10
- 71-75 = D20 level 15
- 96-100 = D20 level 20

NPC Base Attack Bonus uses the NPC `top_level` through this mapping. Player BAB uses Combat Ability through the same mapping.

## 2. Ability scores

Ability modifiers use the standard D20 progression around score 10:

`modifier = floor((score - 10) / 2)`

Foundation NPCs use ordinary ability scores rather than legacy percentage-scale attributes.

## 3. Attack resolution

Normal combat uses ascending D20 attack resolution:

- Melee: `d20 + BAB + STR modifier + proficiency + trait + hitroll`
- Ranged blaster/bowcaster: `d20 + BAB + DEX modifier + proficiency + trait + hitroll`
- Natural 1 misses; natural 20 hits.
- Existing equipment `hitroll` is a direct D20 modifier, so ordinary baseline equipment should normally stay in the -1 to +1 range.

Weapon proficiency remains transitional percentage data; every 20 percentage points currently contribute +1 attack.

## 4. Damage baseline

- Weapons use their authored minimum/maximum damage range.
- Melee and unarmed attacks add STR modifier.
- Blasters and bowcasters do not add DEX to damage.
- Equipment `damroll` remains a direct additive damage modifier.

The Foundation avoids percentage-era damage multipliers and large hitroll/damroll bonuses.

## 5. Ascending Defense

Defense is authored around the current engine calculation:

`10 + heroic defense + DEX + armor + shield + misc`

Armor and shields are real `ITEM_ARMOR` equipment. Unarmored Foundation calibration actors use legacy AC 100 so the compatibility accumulator adds no free Defense.

### Canonical armor tiers

The Foundation supplies three body-armor reference tiers:

| Tier | Prototype | Protection value | Approx. armor bonus |
|---|---|---:|---:|
| Light | Light field armor vest | 6 | +2 |
| Medium | Medium tactical armor suit | 14 | +4 |
| Heavy | Heavy assault armor suit | 24 | +6 |

Positive DEX is capped as armor becomes heavier by the engine's authoritative Defense rules.

### Canonical shields

| Tier | Prototype | Value | Shield bonus |
|---|---|---:|---:|
| Compact | Compact personal shield | 5 | +1 |
| Tactical | Tactical personal shield | 10 | +2 |
| Heavy | Heavy personal shield | 15 | +3 |

Only a shield actually worn in the shield slot contributes the shield bonus.

## 6. Baseline equipment package

`system_nexus.are` retains every pre-existing prototype and adds a self-contained reference package covering:

- light, medium and heavy body armor;
- +1/+2/+3 shield tiers;
- supplemental helmet, leg and hand protection;
- utility knife, baton, vibroknife, vibroaxe, force pike, bowcaster and staff;
- field pack and utility belt;
- toolkit, lockpick set, sensor lens and survival shovel;
- standard and trauma medpacs;
- ammunition cell and comlink;
- battery, duraplast, durasteel, circuit, superconductor, oven, fabric, rare metal, magnet, thread and chemical supplies;
- hydration and nutrition-aware consumables;
- a portable field scanner and Foundation reference manual.

Existing pulse weapons remain available, but their legacy +/-2 and +/-3 hit modifiers are normalized to D20-scale values.

## 7. Nutrition baseline

Foundation food prototypes use the six implemented nutritional axes:

- protein
- carbohydrates
- fats
- vitamins
- minerals
- hydration

The System Nexus nutrient ration is a balanced meal. Shared academy rations and hydration sources now publish nutrition data rather than remaining nutrition-neutral.

## 8. Foundation rooms and services

The System Nexus now contains a self-contained baseline hub with:

- general outfitter;
- weapons counter;
- armor fitting bay;
- medical clinic;
- factory-enabled fabrication workshop;
- bank/account services;
- hotel/rest quarters;
- physical attribute training;
- cognitive/social attribute training and master instructor;
- rules/archive room;
- supply storage;
- neutral equipment test range;
- five D20 calibration cells.

These establish the room/service primitives expected of a fully supported gameplay hub without making ordinary world areas mandatory or gated.

## 9. Encounter calibration actors

The five calibration drones deliberately use zero equipment hitroll/damroll and AC 100. Their progression values are:

| Drone | SWR level | D20 band |
|---|---:|---:|
| Mark I | 5 | 1 |
| Mark V | 25 | 5 |
| Mark X | 50 | 10 |
| Mark XV | 75 | 15 |
| Mark XX | 100 | 20 |

Builders should compare ordinary encounters against these clean reference actors before adding elite traits, unusual resistances, special attacks or unique equipment.

## 10. Authoring rules

For ordinary content:

1. Use real ability scores and the implemented D20 modifier rules.
2. Keep ordinary equipment attack modifiers within -1 to +1 unless the item is explicitly exceptional.
3. Use real armor/shield objects instead of manufacturing large legacy AC values.
4. Use AC 100 for genuinely unarmored NPCs so the compatibility accumulator contributes zero misc Defense.
5. Use the 1/5/10/15/20 calibration cells to validate encounter bands.
6. Use `Nutrition` on food/drink prototypes intended to support attribute development.
7. Use native room flags for factory, bank, hotel, safe and other services.
8. Keep unique and story-specific rewards outside this baseline package.

## 11. Known engine-level compatibility debt

This content package is D20-native, but two inherited code paths remain broader engine work rather than area-data work:

- reinforcement spawning in `swskills.c` still overwrites spawned NPC armor/hitroll/damroll using inherited progression math after creation;
- first-aid medpac healing still uses a legacy fixed random healing range rather than a D20-scaled healing contract.

Those paths should be converted in a dedicated gameplay-code slice because changing them affects every area, not only the Foundation package.
