LOCAL SPHERE — BASIC ZeroPoint CONVERSION PACK
============================================

Target repository:
    ImperatorPortfolio/Mud

Target base commit:
    a9d8072ed7430f1a152c783cd64edfbb758ccfc4
    "Add trait catalog and complete d20 combat helpers"

PURPOSE
-------
This is a SAFE, BASIC conversion pack for turning the current Zero Point
setting into the generic "Local Sphere" setting without renumbering rooms,
changing area filenames, changing planet filenames, or deleting existing game
content.

The pack keeps:
  - every currently listed area;
  - every currently listed planet;
  - every currently listed active space system;
  - all room/object/mobile VNUMs;
  - existing save-file race indexes;
  - existing internal area/planet/system filenames.

The pack changes:
  - character creation to 8 playable species;
  - playable species names and racial stat modifiers;
  - species home/start locations;
  - visible planet names to real astronomical locations;
  - visible active star-system names to real astronomical systems;
  - area display names;
  - a first-pass set of visible race/world references in area/help prose.

IMPORTANT
---------
This is intentionally NOT a full IP scrub.

It does not yet rename or redesign every occurrence of:
  - Jedi / Sith;
  - Force;
  - lightsabers;
  - specific ship classes;
  - every faction, NPC, object, quest, or historical reference.

Those should be a second lore/terminology pass after this basic geography and
race conversion is installed and tested.

INSTALL — WINDOWS
-----------------
1. Back up or commit your repository.
2. Extract this ZIP anywhere.
3. Copy:
       apply_conversion.py
       apply_conversion.bat
   into the ROOT of your Mud repository, beside src/, area/, planets/, space/.
4. Double-click apply_conversion.bat
   OR run:
       py apply_conversion.py

INSTALL — LINUX
---------------
1. Back up or commit your repository.
2. Copy apply_conversion.py and apply_conversion.sh into the repository root.
3. Run:
       chmod +x apply_conversion.sh
       ./apply_conversion.sh

DRY RUN
-------
    py apply_conversion.py --dry-run

RESTORE
-------
The first install creates:
    .local_sphere_backup/

To restore the files backed up by the conversion:
    py apply_conversion.py --restore

DESIGN RULE
-----------
The astronomical locations are real. The inhabited settlements, alien species,
politics, terraforming, cities, stations, ecosystems, and future history are
fictional.

EIGHT PLAYABLE SPECIES
----------------------
Human
    Home: Earth / Sol
    Role: adaptable generalist
    STR +0 DEX +0 WIS +0 INT +0 CON +0 CHA +0

Vordan
    Home: GJ 273 b
    Role: large, strong, resilient combat species
    STR +4 DEX -2 WIS -2 INT +0 CON +2 CHA -2

Lethari
    Home: Teegarden's Star b
    Role: agile diplomats, traders and social specialists
    STR +0 DEX +2 WIS -2 INT +0 CON -2 CHA +2

Keshari
    Home: Ross 128 b
    Role: fast hunter/scout species
    STR +0 DEX +2 WIS +2 INT -2 CON +0 CHA -2

Pelagian
    Home: LHS 1140 b
    Role: amphibious engineers and navigators
    STR -2 DEX -2 WIS +2 INT +2 CON +0 CHA +0
    Innate: aquatic breathing

Drakken
    Home: Proxima Centauri b
    Role: exceptionally hardy reptilian combat/survival species
    STR +2 DEX -2 WIS +0 INT -2 CON +4 CHA -2
    Innate: infrared vision

Veyran
    Home: TRAPPIST-1 e
    Role: small technical/engineering species
    STR -2 DEX +2 WIS +0 INT +4 CON -2 CHA -2

Noxian
    Home: TRAPPIST-1 f
    Role: nocturnal stealth/scouting species
    STR -2 DEX +4 WIS +2 INT +0 CON -2 CHA -2
    Innate: infrared vision + sneak

PRIMARY PLANET SLOT CONVERSION
------------------------------
coruscant  -> Earth
byss       -> Proxima Centauri b
honoghr    -> Ross 128 b
gamorr     -> GJ 273 b
endor      -> TRAPPIST-1 f
Kashyyyk   -> TRAPPIST-1 e
moncal     -> LHS 1140 b
adari      -> Teegarden's Star b
tatooine   -> Barnard b

EARTH
-----
Coruscant becomes Earth.
Menari Spaceport becomes The Hague Interstellar Terminal.
Monument Plaza becomes The Hague Civic Plaza.
The Coruscant city areas become the Earth / Hague metropolitan region.

COMPATIBILITY
-------------
The converter deliberately keeps old filenames such as:
    planets/coruscant
    area/coruscant_streets
    space/coruscant.system

That is intentional. Renaming those files would force changes through many
cross-references and offers no gameplay benefit. Their in-game display names
are converted instead.

The old race constants (for example RACE_WOOKIEE) are also left as internal
numeric identifiers for this first pack. Character creation exposes only the
new eight species. This preserves old save indexes and reduces migration risk.

After testing this pack, a later cleanup can rename internal source constants
without changing gameplay.
