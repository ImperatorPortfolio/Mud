#!/usr/bin/env python3
# Local Sphere basic conversion installer.
#
# Expected base:
#   ImperatorPortfolio/Mud
#   a9d8072ed7430f1a152c783cd64edfbb758ccfc4
#
# This script intentionally preserves filenames and VNUMs.

from __future__ import annotations

from pathlib import Path
import argparse
import re
import shutil
import sys

BASE_COMMIT = "a9d8072ed7430f1a152c783cd64edfbb758ccfc4"
BACKUP_DIR = ".local_sphere_backup"

RACE_TABLE = r'''const struct race_type race_table[MAX_RACE] = {
   /*
    * Internal race indexes are retained for save-file compatibility.
    * Character creation exposes only the eight Local Sphere species.
    *
    * race name     DEF_AFF      st dx ws in cn ch lk fc hp mn re su   RESTRICTION  LANGUAGE
    */
   {
    "Human", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LANG_COMMON},

   /* RACE_WOOKIEE -> Vordan */
   {
    "Vordan", 0, +4, -2, -2, 0, +2, -2, 0, 0, 0, 0, 0, 0, 0, LANG_WOOKIEE},

   /* RACE_TWI_LEK -> Lethari */
   {
    "Lethari", 0, 0, +2, -2, 0, -2, +2, 0, 0, 0, 0, 0, 0, 0, LANG_TWI_LEK},

   /* Legacy Rodian index -> Keshari */
   {
    "Keshari", AFF_SNEAK, 0, +2, +2, -2, 0, -2, 0, 0, 0, 0, 0, 0, 0, LANG_NOGHRI},

   /* Legacy Hutt index -> Lethari */
   {
    "Lethari", 0, 0, +2, -2, 0, -2, +2, 0, 0, 0, 0, 0, 0, 0, LANG_TWI_LEK},

   /* RACE_MON_CALAMARI -> Pelagian */
   {
    "Pelagian", AFF_AQUA_BREATH, -2, -2, +2, +2, 0, 0, 0, 0, 0, 0, 0, 0, 0, LANG_MON_CALAMARI},

   /* RACE_NOGHRI -> Keshari */
   {
    "Keshari", AFF_SNEAK, 0, +2, +2, -2, 0, -2, 0, 0, 0, 0, 0, 0, 0, LANG_NOGHRI},

   /* Legacy Gamorrean index -> Drakken */
   {
    "Drakken", AFF_INFRARED, +2, -2, 0, -2, +4, -2, 0, 0, 0, 0, 0, 0, 0, LANG_TRANDOSHAN},

   /* Legacy Jawa index -> Veyran */
   {
    "Veyran", 0, -2, +2, 0, +4, -2, -2, 0, 0, 0, 0, 0, 0, 0, LANG_VERPINE},

   /* Legacy Adarian index -> Lethari */
   {
    "Lethari", 0, 0, +2, -2, 0, -2, +2, 0, 0, 0, 0, 0, 0, 0, LANG_TWI_LEK},

   /* Legacy Ewok index -> Noxian */
   {
    "Noxian", AFF_INFRARED | AFF_SNEAK, -2, +4, +2, 0, -2, -2, 0, 0, 0, 0, 0, 0, 0, LANG_DEFEL},

   /* RACE_VERPINE -> Veyran */
   {
    "Veyran", 0, -2, +2, 0, +4, -2, -2, 0, 0, 0, 0, 0, 0, 0, LANG_VERPINE},

   /* RACE_DEFEL -> Noxian */
   {
    "Noxian", AFF_INFRARED | AFF_SNEAK, -2, +4, +2, 0, -2, -2, 0, 0, 0, 0, 0, 0, 0, LANG_DEFEL},

   /* RACE_TRANDOSHAN -> Drakken */
   {
    "Drakken", AFF_INFRARED, +2, -2, 0, -2, +4, -2, 0, 0, 0, 0, 0, 0, 0, LANG_TRANDOSHAN},

   /* Legacy Chadra-Fan index -> Noxian */
   {
    "Noxian", AFF_INFRARED | AFF_SNEAK, -2, +4, +2, 0, -2, -2, 0, 0, 0, 0, 0, 0, 0, LANG_DEFEL},

   /* Legacy Quarren index -> Pelagian */
   {
    "Pelagian", AFF_AQUA_BREATH, -2, -2, +2, +2, 0, 0, 0, 0, 0, 0, 0, 0, 0, LANG_MON_CALAMARI},

   /* Legacy Duinuogwuin index -> Vordan */
   {
    "Vordan", 0, +4, -2, -2, 0, +2, -2, 0, 0, 0, 0, 0, 0, 0, LANG_WOOKIEE}
};'''

PLAYABLE_HELPER = r'''
/*
 * Local Sphere playable species.
 *
 * Old numeric race identifiers remain in place for save compatibility,
 * but new character creation exposes only the eight converted species.
 */
static bool is_local_sphere_playable_race( int race )
{
   switch( race )
   {
      case RACE_HUMAN:        /* Human */
      case RACE_WOOKIEE:      /* Vordan */
      case RACE_TWI_LEK:      /* Lethari */
      case RACE_NOGHRI:       /* Keshari */
      case RACE_MON_CALAMARI: /* Pelagian */
      case RACE_TRANDOSHAN:   /* Drakken */
      case RACE_VERPINE:      /* Veyran */
      case RACE_DEFEL:        /* Noxian */
         return TRUE;

      default:
         return FALSE;
   }
}
'''.strip()

WHEREHOME = r'''int wherehome( CHAR_DATA * ch )
{
   if( ch->plr_home )
      return ch->plr_home->vnum;

   if( get_trust( ch ) >= LEVEL_IMMORTAL )
      return ROOM_START_IMMORTAL;

   /*
    * Local Sphere homeworld mapping.
    *
    * Internal race constants and room VNUMs are deliberately retained.
    */
   switch( ch->race )
   {
      case RACE_HUMAN:        /* Human -> Terran Orbital Academy */
         return 50000;

      case RACE_WOOKIEE:      /* Vordan -> Vordan High-Orbit Academy */
         return 50200;

      case RACE_TWI_LEK:      /* Lethari -> Lethari Orbital Academy */
         return 50400;

      case RACE_NOGHRI:       /* Keshari -> Keshari Academy */
         return 50600;

      case RACE_MON_CALAMARI: /* Pelagian -> Pelagian Academy */
         return 50800;

      case RACE_TRANDOSHAN:   /* Drakken -> Drakken Proxima Academy */
         return 51000;

      case RACE_VERPINE:      /* Veyran -> Veyran Technical Academy */
         return 51200;

      case RACE_DEFEL:        /* Noxian -> Noxian Academy */
         return 51400;

      default:
         return ROOM_VNUM_TEMPLE;
   }
}'''

AREA_DISPLAY_NAMES = {
    "newacad.are": "Earth Academy",
    "coruscant_streets": "Earth - The Hague Metropolitan District",
    "space.are": "Deep Space",
    "menari_spaceport": "Earth - The Hague Interstellar Terminal",
    "omni_complex": "Administrative Oversight Complex",
    "adari": "Teegarden's Star b - Lethari Capital",
    "gamorr": "GJ 273 b - Vordan Territory",
    "adari01": "Teegarden's Star b - Outer Districts",
    "monument.plaza": "Earth - The Hague Civic Plaza",
    "tatooine": "Barnard b - Frontier Settlements",
    "darkhand": "Ross 128 b - Hunter Enclave",
    "honoghr": "Ross 128 b - Keshari Territory",
    "cloning": "Earth Regenerative Medicine Centre",
    "senate": "Earth Assembly",
    "kashyyyk": "TRAPPIST-1 e - Veyran Territory",
    "a.byss.city": "Proxima Centauri b - Primary Settlement",
    "grand_towers": "Earth Administrative Towers",
    "land.vehicles": "Earth Surface Transit Network",
    "endor": "TRAPPIST-1 f - Noxian Wilderness",
    "Sorosuub": "Veyran Industrial Complex",
    "jedi_acad": "Aptitude Institute",
    "hunter.ships": "Contractors' Fleet",
    "corus.pt2": "Earth Metropolitan District II",
    "mc90": "Pelagian Fleet Cruiser",
    "hunters.guild": "Contractors' Guild",
    "mall": "Local Sphere Exchange",
    "mon_cal.are": "LHS 1140 b - Pelagian Capital",
    "quarren.are": "LHS 1140 b - Abyssal District",
    "dagobah": "TRAPPIST-1 f - Deep Wilderness",
    "throne.city": "Proxima Centauri b - Drakken Capital",
    "nal.hut": "Barnard b - Free Trade Enclave",
    "monolith": "TRAPPIST Archaeological Site",
    "corus.jail": "The Hague Detention Complex",
    "byss.jail": "Proxima Centauri b - Penal Complex",
    "nar.shadda": "Barnard Orbital Freeport",
}

PLANETS = {
    "coruscant": ("Earth", "Sol System", "Terran Union"),
    "byss": ("Proxima Centauri b", "Proxima Centauri System", "Proxima Directorate"),
    "honoghr": ("Ross 128 b", "Ross 128 System", None),
    "gamorr": ("GJ 273 b", "GJ 273 System", None),
    "endor": ("TRAPPIST-1 f", "TRAPPIST-1 f Orbital Zone", None),
    "Kashyyyk": ("TRAPPIST-1 e", "TRAPPIST-1 e Orbital Zone", None),
    "moncal": ("LHS 1140 b", "LHS 1140 System", None),
    "adari": ("Teegarden's Star b", "Teegarden's Star System", None),
    "tatooine": ("Barnard b", "Barnard's Star System", None),
}

# Optional physical file names used by the renamed Local Sphere data set.
# The engine reads planet.lst entries as filenames, not display names.
RENAMED_PLANET_FILES = {
    "coruscant": "Earth",
    "byss": "ProximaCentauriB",
    "honoghr": "Ross128B",
    "gamorr": "GJ273B",
    "endor": "TRAPPIST-1F",
    "Kashyyyk": "TRAPPIST-1E",
    "moncal": "LHS1140B",
    "adari": "Teegarden",
    "tatooine": "BarnardB",
}

# All 15 active entries currently listed in space/space.lst are retained.
SYSTEMS = {
    "coruscant.system": {
        "Name": "Sol System",
        "Planet1": "Earth",
        "Star1": "Sol",
        "Location1a": "The Hague Interstellar Terminal",
    },
    "byss.system": {
        "Name": "Proxima Centauri System",
        "Planet1": "Proxima Centauri b",
        "Star1": "Proxima Centauri",
    },
    "kashyyyk.system": {
        "Name": "TRAPPIST-1 e Orbital Zone",
        "Planet1": "TRAPPIST-1 e",
        "Star1": "TRAPPIST-1",
    },
    "ryloth.system": {
        "Name": "GJ 667 C System",
        "Star1": "GJ 667 C",
    },
    "rodia.system": {
        "Name": "Epsilon Eridani System",
        "Star1": "Epsilon Eridani",
    },
    "nalhutta.system": {
        "Name": "Wolf 1061 System",
        "Star1": "Wolf 1061",
    },
    "moncalamari.system": {
        "Name": "LHS 1140 System",
        "Planet1": "LHS 1140 b",
        "Star1": "LHS 1140",
    },
    "honoghr.system": {
        "Name": "Ross 128 System",
        "Planet1": "Ross 128 b",
        "Star1": "Ross 128",
    },
    "gamorr.system": {
        "Name": "GJ 273 System",
        "Planet1": "GJ 273 b",
        "Star1": "GJ 273",
    },
    "tatooine.system": {
        "Name": "Barnard's Star System",
        "Planet1": "Barnard b",
        "Star1": "Barnard's Star",
    },
    "endor.system": {
        "Name": "TRAPPIST-1 f Orbital Zone",
        "Planet1": "TRAPPIST-1 f",
        "Star1": "TRAPPIST-1",
    },
    "adaria.system": {
        "Name": "Teegarden's Star System",
        "Planet1": "Teegarden's Star b",
        "Star1": "Teegarden's Star",
    },
    "core.system": {
        "Name": "GJ 1061 System",
        "Star1": "GJ 1061",
    },
    "yavin.system": {
        "Name": "HD 40307 System",
        "Star1": "HD 40307",
    },
    "corperate.system": {
        "Name": "HD 20794 System",
        "Star1": "HD 20794",
    },
}

VISIBLE_REPLACEMENTS = [
    (r"\bMenari Spaceport\b", "The Hague Interstellar Terminal"),
    (r"\bMonument Plaza\b", "The Hague Civic Plaza"),
    (r"\bCoruscant System\b", "Sol System"),
    (r"\bByss System\b", "Proxima Centauri System"),
    (r"\bHonoghr System\b", "Ross 128 System"),
    (r"\bGamorr System\b", "GJ 273 System"),
    (r"\bKashyyyk System\b", "TRAPPIST-1 e Orbital Zone"),
    (r"\bEndor Starsystem\b", "TRAPPIST-1 f Orbital Zone"),
    (r"\bEndor System\b", "TRAPPIST-1 f Orbital Zone"),
    (r"\bAdari System\b", "Teegarden's Star System"),
    (r"\bTatooine System\b", "Barnard's Star System"),
    (r"\bMon Calamari System\b", "LHS 1140 System"),
    (r"\bplanet Mon Calamari\b", "planet LHS 1140 b"),
    (r"\bworld of Mon Calamari\b", "world of LHS 1140 b"),
    (r"\bNal Hutta\b", "Barnard Freehold"),
    (r"\bNar Shaddaa\b", "Barnard Orbital Freeport"),
    (r"\bNar Shadda\b", "Barnard Orbital Freeport"),

    (r"\bCoruscant\b", "Earth"),
    (r"\bByss\b", "Proxima Centauri b"),
    (r"\bHonoghr\b", "Ross 128 b"),
    (r"\bGamorr\b", "GJ 273 b"),
    (r"\bEndor\b", "TRAPPIST-1 f"),
    (r"\bKashyyyk\b", "TRAPPIST-1 e"),
    (r"\bAdari\b", "Teegarden's Star b"),
    (r"\bTatooine\b", "Barnard b"),

    (r"\bWookiees\b", "Vordans"),
    (r"\bWookiee\b", "Vordan"),
    (r"\bTwi['’]leks\b", "Lethari"),
    (r"\bTwi['’]lek\b", "Lethari"),
    (r"\bRodians\b", "Keshari"),
    (r"\bRodian\b", "Keshari"),
    (r"\bNoghris\b", "Keshari"),
    (r"\bNoghri\b", "Keshari"),
    (r"\bGamorreans\b", "Drakken"),
    (r"\bGamorrean\b", "Drakken"),
    (r"\bTrandoshans\b", "Drakken"),
    (r"\bTrandoshan\b", "Drakken"),
    (r"\bJawas\b", "Veyran"),
    (r"\bJawa\b", "Veyran"),
    (r"\bVerpines\b", "Veyran"),
    (r"\bVerpine\b", "Veyran"),
    (r"\bAdarians\b", "Lethari"),
    (r"\bAdarian\b", "Lethari"),
    (r"\bEwoks\b", "Noxians"),
    (r"\bEwok\b", "Noxian"),
    (r"\bDefels\b", "Noxians"),
    (r"\bDefel\b", "Noxian"),
    (r"\bChadra-Fans\b", "Noxians"),
    (r"\bChadra-Fan\b", "Noxian"),
    (r"\bQuarrens\b", "Pelagians"),
    (r"\bQuarren\b", "Pelagian"),
    (r"\bDuinuogwuins\b", "Vordans"),
    (r"\bDuinuogwuin\b", "Vordan"),
    (r"\bHutts\b", "Lethari"),
    (r"\bHutt\b", "Lethari"),
    (r"\bMon Calamari\b", "Pelagian"),

    (r"\bThe New Republic\b", "Terran Union"),
    (r"\bNew Republic\b", "Terran Union"),
    (r"\bThe Empire\b", "Proxima Directorate"),
]

AREA_STRUCTURAL_PREFIXES = (
    "Race ",
    "Speaks ",
    "Speaking ",
    "VIPFlags ",
    "Filename ",
    "Area ",
)


def repo_root() -> Path:
    root = Path.cwd()
    if (root / "src" / "const.c").is_file():
        return root

    for parent in root.parents:
        if (parent / "src" / "const.c").is_file():
            return parent

    raise SystemExit(
        "Could not find the Mud repository root.\n"
        "Run this script from the directory containing src/, area/, planets/ and space/."
    )


def backup(root: Path, path: Path) -> None:
    relative = path.relative_to(root)
    destination = root / BACKUP_DIR / relative
    if destination.exists():
        return
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(path, destination)


def write_if_changed(root: Path, path: Path, new_text: str, dry_run: bool, changed: list[str]) -> None:
    old_text = path.read_text(encoding="utf-8", errors="surrogateescape") if path.is_file() else ""
    if old_text == new_text:
        return

    changed.append(str(path.relative_to(root)))
    if dry_run:
        return

    if path.is_file():
        backup(root, path)
    else:
        path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(new_text, encoding="utf-8", errors="surrogateescape")


def replace_race_table(root: Path, dry_run: bool, changed: list[str]) -> None:
    path = root / "src" / "const.c"
    text = path.read_text(encoding="utf-8", errors="surrogateescape")

    pattern = re.compile(
        r"const struct race_type race_table\[MAX_RACE\] = \{.*?\n\};"
        r"(?=\n\n\nconst char \*const npc_race)",
        re.DOTALL,
    )
    new_text, count = pattern.subn(RACE_TABLE, text, count=1)
    if count != 1:
        raise RuntimeError("Could not locate the current race_table block in src/const.c.")

    write_if_changed(root, path, new_text, dry_run, changed)


def patch_character_creation(root: Path, dry_run: bool, changed: list[str]) -> None:
    path = root / "src" / "comm.c"
    text = path.read_text(encoding="utf-8", errors="surrogateescape")

    helper_marker = "static bool is_local_sphere_playable_race( int race )"
    helper_declaration = "static bool is_local_sphere_playable_race( int race );"
    sex_function_marker = "void nanny_get_new_sex( DESCRIPTOR_DATA *d, const char *argument );"
    function_marker = "void nanny_get_new_race( DESCRIPTOR_DATA *d, const char *argument )\n{"

    if function_marker not in text:
        raise RuntimeError("Could not locate nanny_get_new_race() in src/comm.c.")

    if helper_declaration not in text:
        if sex_function_marker not in text:
            raise RuntimeError("Could not locate nanny_get_new_sex() declaration in src/comm.c.")
        text = text.replace(
            sex_function_marker,
            sex_function_marker + "\n" + helper_declaration,
            1,
        )

    if helper_marker not in text:
        text = text.replace(
            function_marker,
            PLAYABLE_HELPER + "\n\n" + function_marker,
            1,
        )

    race_prompt_marker = "You may choose from the following races"
    section_start = text.find(race_prompt_marker)
    if section_start == -1:
        raise RuntimeError("Could not locate the character-creation race prompt.")

    next_function = "void nanny_get_new_class( DESCRIPTOR_DATA *d, const char *argument )\n{"
    section_end = text.find(next_function, section_start)
    if section_end == -1:
        raise RuntimeError("Could not locate the end of nanny_get_new_race() in src/comm.c.")

    before = text[:section_start]
    block = text[section_start:section_end]
    after = text[section_end:]

    loop_pattern = re.compile(
        r"(for\( iRace = 0; iRace < MAX_RACE; iRace\+\+ \)\s*\n\s*\{)"
        r"(?!\s*\n\s*if\( !is_local_sphere_playable_race\( iRace \) \))"
    )
    insert = (
        r"\1"
        "\n      if( !is_local_sphere_playable_race( iRace ) )"
        "\n         continue;"
    )
    block, _ = loop_pattern.subn(insert, block)

    filter_count = block.count("if( !is_local_sphere_playable_race( iRace ) )")
    if filter_count != 2:
        raise RuntimeError(
            "Expected playable-race filters in the display and selection loops; "
            f"found {filter_count}."
        )

    text = before + block + after
    write_if_changed(root, path, text, dry_run, changed)


def patch_homeworlds(root: Path, dry_run: bool, changed: list[str]) -> None:
    path = root / "src" / "act_move.c"
    text = path.read_text(encoding="utf-8", errors="surrogateescape")

    pattern = re.compile(
        r"int wherehome\( CHAR_DATA \* ch \)\s*\{.*?\n\}"
        r"(?=\n\nchar \*grab_word)",
        re.DOTALL,
    )
    new_text, count = pattern.subn(WHEREHOME, text, count=1)
    if count != 1:
        raise RuntimeError("Could not locate wherehome() in src/act_move.c.")

    write_if_changed(root, path, new_text, dry_run, changed)


def replace_areadata_name(text: str, new_name: str) -> str:
    start = text.find("#AREADATA")
    end = text.find("#ENDAREADATA", start)
    if start == -1 or end == -1:
        return text

    block = text[start:end]
    block, count = re.subn(
        r"(?m)^Name\s+.*~$",
        f"Name         {new_name}~",
        block,
        count=1,
    )
    if count == 0:
        return text

    return text[:start] + block + text[end:]


def visible_replace_line(line: str) -> str:
    stripped = line.lstrip()

    if any(stripped.startswith(prefix) for prefix in AREA_STRUCTURAL_PREFIXES):
        return line

    result = line
    for pattern, replacement in VISIBLE_REPLACEMENTS:
        result = re.sub(pattern, replacement, result, flags=re.IGNORECASE)
    return result


def patch_areas(root: Path, dry_run: bool, changed: list[str]) -> None:
    area_dir = root / "area"
    for path in area_dir.iterdir():
        if not path.is_file():
            continue

        if path.name in {"area.lst", "boot.txt"}:
            continue

        text = path.read_text(encoding="utf-8", errors="surrogateescape")

        if path.name in AREA_DISPLAY_NAMES:
            text = replace_areadata_name(text, AREA_DISPLAY_NAMES[path.name])

        text = "".join(visible_replace_line(line) for line in text.splitlines(keepends=True))
        write_if_changed(root, path, text, dry_run, changed)


def set_field(text: str, field: str, value: str) -> str:
    pattern = re.compile(rf"(?m)^{re.escape(field)}\s+.*~$")
    replacement = f"{field:<13}{value}~"
    new_text, count = pattern.subn(replacement, text, count=1)
    return new_text if count else text


def active_planet_path(root: Path, legacy_name: str) -> Path:
    legacy_path = root / "planets" / legacy_name
    if legacy_path.is_file():
        return legacy_path

    renamed_name = RENAMED_PLANET_FILES.get(legacy_name)
    if renamed_name:
        renamed_path = root / "planets" / renamed_name
        if renamed_path.is_file():
            return renamed_path

    raise RuntimeError(f"Missing expected planet file for {legacy_name}.")


def patch_renamed_planet_index(root: Path, dry_run: bool, changed: list[str]) -> None:
    """Point planet.lst at user-renamed planet files without renaming them back."""
    path = root / "planets" / "planet.lst"
    text = path.read_text(encoding="utf-8", errors="surrogateescape")

    for legacy_name, renamed_name in RENAMED_PLANET_FILES.items():
        legacy_path = root / "planets" / legacy_name
        renamed_path = root / "planets" / renamed_name
        if not legacy_path.is_file() and renamed_path.is_file():
            text = re.sub(
                rf"(?m)^{re.escape(legacy_name)}$",
                renamed_name,
                text,
            )

    write_if_changed(root, path, text, dry_run, changed)


def patch_planets(root: Path, dry_run: bool, changed: list[str]) -> None:
    for filename, (display, system, government) in PLANETS.items():
        path = active_planet_path(root, filename)

        text = path.read_text(encoding="utf-8", errors="surrogateescape")
        text = set_field(text, "Name", display)
        text = set_field(text, "Starsystem", system)

        # Keep the internal filename aligned with the physical renamed file.
        if path.name != filename:
            text = set_field(text, "Filename", path.name)

        if government is not None:
            gov_pattern = re.compile(r"(?m)^GovernedBy\s+.*~$")
            if gov_pattern.search(text):
                text = gov_pattern.sub(f"{'GovernedBy':<13}{government}~", text, count=1)

        write_if_changed(root, path, text, dry_run, changed)


def patch_systems(root: Path, dry_run: bool, changed: list[str]) -> None:
    for filename, fields in SYSTEMS.items():
        path = root / "space" / filename
        if not path.is_file():
            raise RuntimeError(f"Missing expected active system file: {path}")

        text = path.read_text(encoding="utf-8", errors="surrogateescape")

        for field, value in fields.items():
            text = set_field(text, field, value)

        converted_lines = []
        for line in text.splitlines(keepends=True):
            # Filename is an internal cross-reference and must not be renamed.
            if line.lstrip().startswith("Filename "):
                converted_lines.append(line)
                continue

            for pattern, replacement in VISIBLE_REPLACEMENTS:
                line = re.sub(pattern, replacement, line, flags=re.IGNORECASE)
            converted_lines.append(line)

        text = "".join(converted_lines)

        # Reassert authoritative fields after the general visible-name pass.
        for field, value in fields.items():
            text = set_field(text, field, value)

        write_if_changed(root, path, text, dry_run, changed)


def add_shared_academy_objects(root: Path, dry_run: bool, changed: list[str]) -> None:
    """Provide legacy starter objects for global resets after academy VNUMs move."""
    academy_names = {
        "DrakkenAcademy.are", "HumanAcademy.are", "KeshariAcademy.are",
        "LethariAcademy.are", "NoxianAcademy.are", "PelagianAcademy.are",
        "VeyranAcademy.are", "VordanAcademy.are",
    }
    list_path = root / "area" / "area.lst"
    list_text = list_path.read_text(encoding="utf-8", errors="surrogateescape")
    loaded_areas = set(list_text.splitlines())
    if not academy_names.intersection(loaded_areas):
        return

    source_path = root / "area" / "newacad.are"
    if not source_path.is_file():
        source_path = root / BACKUP_DIR / "area" / "newacad.are"
    if not source_path.is_file():
        raise RuntimeError("Missing original academy data needed for shared starter objects.")

    source = source_path.read_text(encoding="utf-8", errors="surrogateescape")
    header_end = source.find("#MOBILE\n")
    if header_end == -1:
        raise RuntimeError("Could not locate the academy area header.")

    required_vnums = {10311, 10312, 10313, 10314, 10315, 10317, 10424, 10487, 10488}
    object_pattern = re.compile(
        r"(?ms)^#OBJECT\nVnum\s+(\d+).*?(?=^#OBJECT\n|^#ROOM\n|^#ENDAREA\n)"
    )
    blocks = {
        int(match.group(1)): match.group(0)
        for match in object_pattern.finditer(source)
        if int(match.group(1)) in required_vnums
    }
    missing = sorted(required_vnums - blocks.keys())
    if missing:
        raise RuntimeError(f"Original academy lacks required object VNUMs: {missing}")

    header = replace_areadata_name(source[:header_end], "Shared Training Equipment")
    header = re.sub(
        r"(?m)^ResetMsg\s+.*$",
        "ResetMsg     Shared starter equipment is being restocked~",
        header,
        count=1,
    )
    content = header + "\n".join(blocks[vnum] for vnum in sorted(blocks)) + "#ENDAREA\n"
    compatibility_path = root / "area" / "academy_common_objects.are"
    write_if_changed(root, compatibility_path, content, dry_run, changed)

    if "academy_common_objects.are" not in loaded_areas:
        insertion = "academy_common_objects.are\n"
        last_academy = max(
            list_text.find(name) + len(name)
            for name in academy_names if name in loaded_areas
        )
        line_end = list_text.find("\n", last_academy)
        list_text = list_text[:line_end + 1] + insertion + list_text[line_end + 1:]
        write_if_changed(root, list_path, list_text, dry_run, changed)


def restore(root: Path) -> int:
    backup_root = root / BACKUP_DIR
    if not backup_root.is_dir():
        print("No .local_sphere_backup directory exists. Nothing to restore.")
        return 0

    restored = 0
    for source in backup_root.rglob("*"):
        if not source.is_file():
            continue
        relative = source.relative_to(backup_root)
        destination = root / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)
        restored += 1

    print(f"Restored {restored} files from {BACKUP_DIR}/")
    return 0


def self_test() -> int:
    sample_const = (
        "const struct race_type race_table[MAX_RACE] = {\n"
        '   {"Human", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, LANG_COMMON}\n'
        "};\n\n\n"
        "const char *const npc_race[MAX_NPC_RACE] = {\n"
    )
    pattern = re.compile(
        r"const struct race_type race_table\[MAX_RACE\] = \{.*?\n\};"
        r"(?=\n\n\nconst char \*const npc_race)",
        re.DOTALL,
    )
    assert pattern.search(sample_const)

    sample_area = "#FUSSAREA\n#AREADATA\nName         Endor~\n#ENDAREADATA\n"
    assert "TRAPPIST-1 f" in replace_areadata_name(
        sample_area, "TRAPPIST-1 f - Noxian Wilderness"
    )

    assert "Vordan" in visible_replace_line("Long       A Wookiee walks here.\n")
    assert visible_replace_line("Race       Wookiee~\n") == "Race       Wookiee~\n"

    print("Self-test passed.")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Apply the Local Sphere basic conversion.")
    parser.add_argument("--dry-run", action="store_true", help="Report changes without writing files.")
    parser.add_argument("--restore", action="store_true", help="Restore files from the first-run backup.")
    parser.add_argument("--self-test", action="store_true", help="Run installer self-tests only.")
    args = parser.parse_args()

    if args.self_test:
        return self_test()

    root = repo_root()

    if args.restore:
        return restore(root)

    required = [
        root / "src" / "const.c",
        root / "src" / "comm.c",
        root / "src" / "act_move.c",
        root / "area" / "area.lst",
        root / "planets" / "planet.lst",
        root / "space" / "space.lst",
    ]
    missing = [str(p) for p in required if not p.is_file()]
    if missing:
        raise SystemExit("Missing required repository files:\n  " + "\n  ".join(missing))

    changed: list[str] = []

    replace_race_table(root, args.dry_run, changed)
    patch_character_creation(root, args.dry_run, changed)
    patch_homeworlds(root, args.dry_run, changed)
    patch_renamed_planet_index(root, args.dry_run, changed)
    patch_planets(root, args.dry_run, changed)
    patch_systems(root, args.dry_run, changed)
    patch_areas(root, args.dry_run, changed)
    add_shared_academy_objects(root, args.dry_run, changed)

    if args.dry_run:
        print(f"Dry run complete: {len(changed)} files would change.")
    else:
        print(f"Conversion complete: {len(changed)} files changed.")
        print(f"Original versions were backed up under {BACKUP_DIR}/")

    if changed:
        print("\nChanged files:")
        for name in sorted(changed):
            print(f"  {name}")

    print("\nNext:")
    print("  1. Compile the MUD.")
    print("  2. Create a NEW character and confirm only 8 species are offered.")
    print("  3. Check Earth/The Hague, one alien homeworld, and SPACE.")
    print("  4. Do not delete the backup until the conversion has been tested.")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except Exception as exc:
        print(f"\nERROR: {exc}", file=sys.stderr)
        raise
