from pathlib import Path
import re

AREA = Path('area/Earth/DenHaagArrivals.are')
text = AREA.read_text(encoding='utf-8')

BASE_ROOM_VNUMS = list(range(200, 258))
BASE_MOB_COUNT = 16
BASE_OBJECT_VNUMS = [202, *range(204, 210), *range(211, 225), *range(226, 246)]
NEW_OBJECT_VNUMS = [246, 247, 248, 249, 250]


def block_pattern(kind, vnum):
    return re.compile(rf'(?ms)^#{kind}\nVnum\s+{vnum}\n.*?^#END{kind}\s*$')


def get_block(kind, vnum):
    m = block_pattern(kind, vnum).search(text)
    if not m:
        raise SystemExit(f'Missing {kind} {vnum}')
    return m.group(0)


def replace_block(kind, vnum, new_block):
    global text
    pat = block_pattern(kind, vnum)
    text2, n = pat.subn(new_block.rstrip(), text, count=1)
    if n != 1:
        raise SystemExit(f'Expected one {kind} {vnum}, replaced {n}')
    text = text2


def append_before_end(kind, vnum, addition):
    block = get_block(kind, vnum)
    marker = f'#END{kind}'
    if addition.strip() in block:
        return
    pos = block.rfind(marker)
    new_block = block[:pos].rstrip() + '\n' + addition.strip() + '\n' + block[pos:]
    replace_block(kind, vnum, new_block)


def add_reset(room_vnum, reset_line):
    block = get_block('ROOM', room_vnum)
    if reset_line in block:
        return
    insert_at = block.find('#MUDPROG')
    if insert_at < 0:
        insert_at = block.rfind('#ENDROOM')
    new_block = block[:insert_at].rstrip() + '\n' + reset_line + '\n\n' + block[insert_at:].lstrip()
    replace_block('ROOM', room_vnum, new_block)


def prog(progtype, arglist, commands):
    return f'''#MUDPROG
Progtype  {progtype}~
Arglist   {arglist}~
Comlist   {commands.rstrip()}
~
#ENDPROG'''


# ---------------------------------------------------------------------------
# Every object in the area gets a proper look/examine description.
# ---------------------------------------------------------------------------
EXDESC = {
    202: 'Flexible composite soles, reinforced heels and breathable uppers make these shoes suitable for long terminal walks. A small size label is printed inside each tongue.',
    204: 'Sliced chicken, leaves and a light dressing are packed between two pieces of grain bread. The wrapper carries a preparation time and a short chilled-storage notice.',
    205: 'Pieces of melon, berries, citrus and apple fill a clear chilled cup. A snap lid and disposable fork keep the serving secure during travel.',
    206: 'Whole grains, roasted vegetables and pulses fill a sealed heat-safe bowl. A printed label lists the ingredients and reheating instructions.',
    207: 'Layers of yoghurt, rolled oats and berries fill a small lidded pot. The ingredients and preparation time are printed around the rim.',
    208: 'A clear bottle contains lightly mineralised water with added electrolytes. The seal is intact and a nutritional panel runs down one side.',
    209: 'The folded map covers central Den Haag, the terminal, surface transit and the major passenger districts. Colour-coded lines continue onto a compact route index.',
    211: 'The guide summarises Den Haag districts, public services, landmarks and visitor facilities. Several pages are devoted to terminal connections and local transport.',
    212: 'The timetable lists terminal shuttle links, surface routes and scheduled connection windows. A small diagram shows the four local shuttle gates.',
    213: 'The soft-sided bag has reinforced seams, a shoulder strap and a compact main compartment. A blank identity sleeve is stitched beside the handle.',
    214: 'A folding driver, cutters, probe, small spanner and fastening bits are secured inside a padded tool roll. The kit is intended for routine personal-equipment repairs.',
    215: 'The sealed power cell is a common low-voltage replacement unit for portable electronics. Charge, polarity and disposal markings cover one end of the casing.',
    216: 'The hand-held communicator has a simple keypad, local-band selector and small status display. Its packaging includes a short terminal-frequency reference card.',
    217: 'The compact medical pouch contains dressings, antiseptic, seal strips and basic emergency supplies. A tamper strip runs across the main closure.',
    218: 'Soap sheets, toothbrush supplies, wipes and a small towel are sealed inside the travel kit. Each component is packed separately for short journeys.',
    219: 'Several folded sanitary wipes are sealed inside a flat moisture-proof packet. The printed label lists surface and personal-cleaning uses.',
    220: 'The benches use moulded composite seats fixed to a heavy floor rail. Small gaps beneath the seats provide space for carry-on luggage.',
    221: 'The display combines large gate lettering with smaller arrival, departure and connection fields. A narrow status strip along the bottom carries service notices.',
    222: 'The customs unit combines an identity reader, declaration pad and compact baggage sensor. Indicator lights show the sequence for routine passenger processing.',
    223: 'The arch contains paired sensor arrays behind smooth composite panels. A small maintenance display is recessed into the inner right-hand upright.',
    224: 'The trolley has a low luggage platform, telescoping handle and four locking wheels. A terminal asset number is stamped into the frame.',
    226: 'Separate openings accept paper, packaging and general waste. Internal status lamps show that the three collection bins are below capacity.',
    227: 'The suitcase has a dark hard shell, reinforced corners and a recessed telescoping handle. Several old routing stickers have been removed from the case.',
    228: 'The slim wallet has sleeves for identification, travel permits and booking slips. One internal pocket still contains an expired orbital-hotel receipt with no identifying name.',
    229: 'The rechargeable card carries the Den Haag transit mark and a contactless payment strip. A serial number and basic fare instructions are printed on the reverse.',
    230: 'Large arrows divide the terminal into customs, passenger services and shuttle routes. Smaller symbols mark food, health, facilities and Lost Property.',
    231: 'A glass-fronted vending unit holds water, fruit snacks and sealed travel food. Product numbers and payment instructions surround a compact touch panel.',
    232: 'Six protected charging sockets sit beneath a narrow status display. Each bay has a cable recess, charge indicator and automatic current limiter.',
    233: 'Cleaning liquids, cloths, liners and a compact floor tool are secured to the service cart. The upper tray carries a terminal maintenance inventory tag.',
    234: 'The terminal presents a touch map of arrivals, customs, passenger services and all four shuttle routes. A second page lists current surface-transport connections.',
    235: 'A reinforced weighing plate sits flush with the floor beside a digital readout. Markings around the edge show standard cabin and checked-baggage limits.',
    236: 'A flexible belt retracts into a weighted post fitted with a magnetic floor lock. Matching posts can be linked to form temporary boarding lanes.',
    237: 'The recessed fountain has a sensor valve, bottle-filling outlet and drain grille. A small indicator confirms that the local filtration unit is operating normally.',
    238: 'The shallow tray is moulded from impact-resistant composite and sized for small bags and loose personal items. A number is printed on each end.',
    239: 'Rows of numbered secure compartments fill the locker bank. Each door has a tag slot, status light and staff-controlled electronic latch.',
    240: 'The wall-mounted dryer uses a recessed air outlet and proximity sensor. A service light beneath the housing shows normal operation.',
    241: 'The basin is formed from a single easy-clean composite surface. A sensor tap and soap dispenser are built directly into the rear edge.',
    242: 'The placard uses a large berth identifier above departure time, destination and boarding status. A lower strip displays accessibility and baggage information.',
    243: 'Modular shelves use adjustable rails and clear product labels. The units are shallow enough to preserve the terminal circulation aisle.',
    244: 'Chilled glass-fronted compartments hold prepared meals, fruit and bottled drinks. Digital temperature strips run along the edge of each shelf.',
    245: 'The compact counter contains a payment terminal, staff display and recessed storage. A low passenger-facing shelf keeps the circulation side clear.',
}

# Add extra descriptions before object programs/end markers.
for vnum in BASE_OBJECT_VNUMS:
    block = get_block('OBJECT', vnum)
    if '#EXDESC' in block:
        continue
    km = re.search(r'^Keywords\s+(.*?)~\s*$', block, re.M)
    if not km:
        raise SystemExit(f'Object {vnum} has no Keywords')
    key = km.group(1).strip()
    desc = EXDESC[vnum]
    ex = f'''#EXDESC
ExDescKey {key}~
ExDesc {desc}~
#ENDEXDESC'''
    insert_at = block.find('#MUDPROG')
    if insert_at < 0:
        insert_at = block.rfind('#ENDOBJECT')
    new_block = block[:insert_at].rstrip() + '\n' + ex + '\n' + block[insert_at:].lstrip()
    replace_block('OBJECT', vnum, new_block)


# ---------------------------------------------------------------------------
# Quest-state objects. These keep both quests inside ordinary MUDProg state.
# ---------------------------------------------------------------------------
NEW_OBJECTS = r'''#OBJECT
Vnum     246
Keywords misrouted baggage routing tag alpha transfer~
Type     trash~
Short    a misrouted baggage tag~
Long     A narrow baggage-routing tag lies among the archive sorting trays.~
WFlags   take hold~
Values   0 0 0 0 0 0
Stats    1 0 0 0 0
#EXDESC
ExDescKey misrouted baggage routing tag alpha transfer~
ExDesc The printed route begins at the interstellar berth but ends with an Alpha transfer code that does not match the case number. A handwritten Lost Property notation marks it for verification.~
#ENDEXDESC
#MUDPROG
Progtype  get_prog~
Arglist   100~
Comlist   mpechoat $n The routing tag carries a mismatched Alpha transfer code and a Lost Property verification mark.
mpechoaround $n $n lifts a narrow routing tag from the archive sorting trays.
~
#ENDPROG
#ENDOBJECT

#OBJECT
Vnum     247
Keywords verified baggage release slip alpha~
Type     trash~
Short    a verified baggage release slip~
Long     A freshly validated baggage-release slip has been printed here.~
WFlags   take hold~
Values   0 0 0 0 0 0
Stats    1 0 0 0 0
#EXDESC
ExDescKey verified baggage release slip alpha~
ExDesc The slip matches the misrouted tag to a black business case held by Lost Property. A validation stamp authorises the counter clerk to release it.~
#ENDEXDESC
#ENDOBJECT

#OBJECT
Vnum     248
Keywords recovered business suitcase black luggage~
Type     container~
Short    a recovered black business suitcase~
Long     A recovered black business suitcase rests ready for collection.~
WFlags   take~
Values   20 0 0 0 0 0
Stats    4 0 0 0 0
#EXDESC
ExDescKey recovered business suitcase black luggage~
ExDesc The hard black shell carries the same case number recorded on the baggage-release slip. A worn interstellar claim stub remains fixed beneath the handle.~
#ENDEXDESC
#ENDOBJECT

#OBJECT
Vnum     249
Keywords faulted security sensor coupler scanner~
Type     trash~
Short    a faulted security sensor coupler~
Long     A heat-marked sensor coupler lies beside an open service panel.~
WFlags   take hold~
Values   0 0 0 0 0 0
Stats    1 0 0 0 0
#EXDESC
ExDescKey faulted security sensor coupler scanner~
ExDesc One contact is darkened by repeated arcing and the ceramic collar has a hairline crack. The part number matches the passenger-screening sensor family.~
#ENDEXDESC
#MUDPROG
Progtype  get_prog~
Arglist   100~
Comlist   mpechoat $n A tiny diagnostic diode flashes amber when the damaged coupler is moved.
mpechoaround $n $n retrieves a heat-marked component from beside the service panel.
~
#ENDPROG
#ENDOBJECT

#OBJECT
Vnum     250
Keywords calibrated security sensor coupler scanner~
Type     trash~
Short    a calibrated security sensor coupler~
Long     A calibrated replacement sensor coupler has been set here.~
WFlags   take hold~
Values   0 0 0 0 0 0
Stats    1 0 0 0 0
#EXDESC
ExDescKey calibrated security sensor coupler scanner~
ExDesc The replacement component has clean contacts, an intact ceramic collar and a fresh calibration mark. Its part number matches the security scanner's east sensor loop.~
#ENDEXDESC
#ENDOBJECT

'''

if not block_pattern('OBJECT', 246).search(text):
    idx = text.find('#ROOM\n')
    if idx < 0:
        raise SystemExit('Could not locate first room')
    text = text[:idx] + NEW_OBJECTS + text[idx:]


# ---------------------------------------------------------------------------
# Quest 1: Wrong Bag -- discovery -> verification -> recovery -> return.
# ---------------------------------------------------------------------------
append_before_end('MOBILE', 218, prog('speech_prog', 'wrong bag luggage suitcase', '''say My black case was exchanged for another after the interstellar arrival.
say Lost Property said the routing tag should still be in their archive if it has not been cleared yet.'''))
append_before_end('MOBILE', 218, prog('give_prog', 'recovered business suitcase black luggage', '''mpechoat $n The traveller compares the case number with a battered claim stub and unlocks the suitcase.
mpechoaround $n The traveller checks the recovered suitcase against a battered claim stub.
say That's it. Thank you. Take my spare local transit card; I will not need it now.
mpjunk recovered
mpoload 229
give transit $n'''))

append_before_end('MOBILE', 214, prog('give_prog', 'misrouted baggage routing tag alpha transfer', '''mpechoat $n The information clerk runs the routing code through the terminal register.
mpechoaround $n The information clerk checks a baggage-routing tag against the terminal register.
say This tag was diverted through Alpha by mistake. Lost Property is holding the matching black case.
mpjunk misrouted
mpoload 247
give verified $n'''))

append_before_end('MOBILE', 217, prog('speech_prog', 'bag luggage suitcase claim', '''say Misrouted baggage is held in the archive until its routing tag can be verified at an information centre.
say Bring me a validated release slip and I can release the matching case.'''))
append_before_end('MOBILE', 217, prog('give_prog', 'verified baggage release slip alpha', '''mpechoat $n The clerk scans the release slip and opens the matching property record.
mpechoaround $n The Lost Property clerk checks a release slip against the secure archive.
say Found it. Black case, interstellar claim. Return it to its owner if you can.
mpjunk verified
mpoload 248
give recovered $n'''))

# Three physical objects is intentionally reserved for the Lost Property archive.
add_reset(231, 'Reset O 0 246 1 231')


# ---------------------------------------------------------------------------
# Quest 2: Ghost in the Scanner -- diagnose -> replace -> restore.
# ---------------------------------------------------------------------------
append_before_end('MOBILE', 204, prog('speech_prog', 'scanner fault false alarm maintenance', '''say The east sensor loop keeps throwing false positives. The arch diagnostic points into the east service passage.
say If a damaged coupler has come loose, the terminal technician at Alpha berth can calibrate a replacement.'''))
append_before_end('MOBILE', 204, prog('give_prog', 'calibrated security sensor coupler scanner', '''mpechoat $n The screening officer seats the calibrated coupler and runs a short diagnostic cycle.
mpechoaround $n The screening officer fits a replacement component into the scanner console.
mpecho The security arch pulses amber once, then settles to a steady green status light.
say Clean signal. That fixed it. Take this spare power cell for the trouble.
mpjunk calibrated
mpoload 215
give replacement $n'''))

append_before_end('MOBILE', 219, prog('speech_prog', 'scanner coupler maintenance', '''say Scanner couplers are matched components. Bring me the damaged one and I can calibrate a replacement against its serial.'''))
append_before_end('MOBILE', 219, prog('give_prog', 'faulted security sensor coupler scanner', '''mpechoat $n The technician checks the scorched contacts, then matches the serial against a replacement part.
mpechoaround $n The maintenance technician compares a damaged coupler with a diagnostic pad.
say The ceramic collar is cracked. Here is a calibrated replacement for the screening officer.
mpjunk faulted
mpoload 250
give calibrated $n'''))

add_reset(212, 'Reset O 0 249 1 212')


# ---------------------------------------------------------------------------
# Useful interactables and small discoveries, kept sparse.
# ---------------------------------------------------------------------------
# A forgotten document wallet is a harmless environmental discovery.
add_reset(203, 'Reset O 0 228 1 203')
# A real fountain gives the central crossway a functional utility object.
add_reset(205, 'Reset O 0 237 1 205')
# One charging station and vending unit make the two transfer lounges useful.
add_reset(214, 'Reset O 0 232 1 214')
add_reset(218, 'Reset O 0 231 1 218')
# Both information centres receive a terminal; max count is normalised below.
add_reset(222, 'Reset O 0 234 2 222')
add_reset(226, 'Reset O 0 234 2 226')

INTERACTIONS = {
    221: '''mpechoat $n The display expands its connection panel: Alpha and Bravo west, Charlie and Delta east, with customs and surface transit shown above them.
mpechoaround $n The departure display refreshes as $n checks the connection panel.''',
    222: '''mpechoat $n The customs scanner wakes its standby display: IDENTIFICATION, DECLARATION, BAGGAGE, CLEARANCE. All four stages show ready.
mpechoaround $n A customs scanner runs a brief green self-test.''',
    223: '''mpechoat $n The maintenance display reports repeated checksum drift on EAST SENSOR LOOP 2 and flags COUPLER IMPEDANCE HIGH. The service reference points east from Customs Hall.
mpechoaround $n The security arch opens a small amber diagnostic panel.''',
    230: '''mpechoat $n The sign lists Customs and Security north; Lost Property and Alpha-Bravo services west; Food, Health and Charlie-Delta services east.
mpechoaround $n The wayfinding sign brightens its route arrows.''',
    231: '''mpechoat $n The vending panel lists water, fruit snacks and sealed travel meals. Each slot shows stock, price and allergen markings.
mpechoaround $n The vending unit illuminates its product panel.''',
    232: '''mpechoat $n The charging station wakes and runs a socket test. Six bays report READY beneath individual current-limit indicators.
mpechoaround $n A charging station cycles through a short green self-test.''',
    234: '''mpechoat $n The terminal map highlights Customs and Security north, Alpha-Bravo west, Charlie-Delta east, and Lost Property beyond Alpha services. A maintenance overlay marks the east security service passage.
mpechoaround $n The information terminal redraws its route map.''',
    235: '''mpechoat $n The baggage scale settles at zero. A side panel lists standard cabin limits and the dimensions accepted by the interstellar transfer berth.
mpechoaround $n The baggage scale display wakes briefly.''',
    239: '''mpechoat $n Most lockers show sealed or empty status. One archive row is marked MISROUTED - VERIFY TAG BEFORE RELEASE.
mpechoaround $n Status lights move down the lost-property locker bank.''',
    242: '''mpechoat $n The placard shows berth, destination, boarding status and baggage limits. Its active header is tied directly to the local shuttle gate controller.
mpechoaround $n The shuttle placard refreshes its boarding-status strip.''',
}
for vnum, commands in INTERACTIONS.items():
    append_before_end('OBJECT', vnum, prog('exa_prog', '100', commands))

# The forgotten wallet has a tiny non-quest discovery when collected.
append_before_end('OBJECT', 228, prog('get_prog', '100', '''mpechoat $n An expired orbital-hotel receipt slips from the wallet; the guest-name field has been cleanly torn away.
mpechoaround $n A small paper receipt slips from the travel document wallet.'''))

# Quiet environmental clues, only in the two investigation-heavy rooms.
append_before_end('ROOM', 212, prog('rand_prog', '4', 'mpecho An amber service light flickers once behind an open east-loop access panel.'))
append_before_end('ROOM', 231, prog('rand_prog', '3', 'mpecho A locker status light changes from SORTING to VERIFY TAG and gives a quiet chime.'))


# ---------------------------------------------------------------------------
# Reset caps: in SMAUG/FUSS O resets, the max field must allow all intended
# placements of the same prototype. Normalise local object placements.
# ---------------------------------------------------------------------------
reset_re = re.compile(r'(?m)^(\s*Reset O\s+\d+\s+)(\d+)(\s+)(\d+)(\s+\d+\s*)$')
placements = {}
for m in reset_re.finditer(text):
    vnum = int(m.group(2))
    if vnum in set(BASE_OBJECT_VNUMS + NEW_OBJECT_VNUMS):
        placements[vnum] = placements.get(vnum, 0) + 1

def cap_repl(m):
    vnum = int(m.group(2))
    if vnum not in placements:
        return m.group(0)
    return f'{m.group(1)}{vnum}{m.group(3)}{placements[vnum]}{m.group(5)}'
text = reset_re.sub(cap_repl, text)


# ---------------------------------------------------------------------------
# Validation: preserve the base area while proving the new content is sane.
# ---------------------------------------------------------------------------
room_blocks = re.findall(r'(?ms)^#ROOM\n.*?^#ENDROOM\s*$', text)
mob_blocks = re.findall(r'(?ms)^#MOBILE\n.*?^#ENDMOBILE\s*$', text)
obj_blocks = re.findall(r'(?ms)^#OBJECT\n.*?^#ENDOBJECT\s*$', text)

room_vnums = [int(re.search(r'^Vnum\s+(\d+)', b, re.M).group(1)) for b in room_blocks]
mob_vnums = [int(re.search(r'^Vnum\s+(\d+)', b, re.M).group(1)) for b in mob_blocks]
obj_vnums = [int(re.search(r'^Vnum\s+(\d+)', b, re.M).group(1)) for b in obj_blocks]

assert room_vnums == BASE_ROOM_VNUMS, room_vnums
assert len(mob_blocks) == BASE_MOB_COUNT, len(mob_blocks)
assert len(obj_blocks) == 46, len(obj_blocks)
assert len(set(obj_vnums)) == len(obj_vnums)
for v in NEW_OBJECT_VNUMS:
    assert v in obj_vnums

# Every object must be directly examinable through an extra description.
for b in obj_blocks:
    v = int(re.search(r'^Vnum\s+(\d+)', b, re.M).group(1))
    assert '#EXDESC' in b, f'Object {v} missing EXDESC'
    assert 'ExDescKey ' in b and 'ExDesc ' in b, f'Object {v} incomplete EXDESC'

# Room prose and topology remain structurally intact: exactly five Desc lines.
for b in room_blocks:
    v = int(re.search(r'^Vnum\s+(\d+)', b, re.M).group(1))
    m = re.search(r'(?ms)^Desc\s+(.*?)\n~\s*$', b)
    assert m, f'Room {v} missing description'
    lines = m.group(1).splitlines()
    assert len(lines) == 5, f'Room {v} has {len(lines)} desc lines'

# Every local M/O reset must resolve; G resets must resolve as local objects too.
local_mobs = set(mob_vnums)
local_objs = set(obj_vnums)
for line in text.splitlines():
    s = line.strip()
    if s.startswith('Reset M '):
        parts = s.split()
        assert int(parts[3]) in local_mobs, line
    elif s.startswith('Reset O '):
        parts = s.split()
        ov = int(parts[3])
        if 200 <= ov <= 999:
            assert ov in local_objs, line
    elif s.startswith('Reset G ') or s.startswith('Reset E '):
        parts = s.split()
        ov = int(parts[3])
        if 200 <= ov <= 999:
            assert ov in local_objs, line

# Sparse room-object rule: <=2 normally, Lost Property Archive alone may use 3.
for b in room_blocks:
    v = int(re.search(r'^Vnum\s+(\d+)', b, re.M).group(1))
    count = len(re.findall(r'(?m)^\s*Reset O\s+', b))
    limit = 3 if v == 231 else 2
    assert count <= limit, f'Room {v} has {count} object resets'

# Quest-chain and interaction sanity markers.
for needle in [
    'Progtype  give_prog~',
    'Progtype  exa_prog~',
    'Progtype  get_prog~',
    'misrouted baggage routing tag alpha transfer~',
    'verified baggage release slip alpha~',
    'recovered business suitcase black luggage~',
    'faulted security sensor coupler scanner~',
    'calibrated security sensor coupler scanner~',
    'Reset O 0 246 1 231',
    'Reset O 0 249 1 212',
]:
    assert needle in text, needle

AREA.write_text(text.rstrip() + '\n', encoding='utf-8')
print(f'Validated Den Haag: {len(room_blocks)} rooms, {len(mob_blocks)} mobs, {len(obj_blocks)} objects, {text.count("#MUDPROG")} MUDProgs')
