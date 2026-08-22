from pathlib import Path
import re

PATH = Path('area/Earth/DenHaagArrivals.are')
text = PATH.read_text(encoding='utf-8')

END = {'MOBILE': '#ENDMOBILE', 'OBJECT': '#ENDOBJECT', 'ROOM': '#ENDROOM'}

def get_block(src, kind, vnum):
    pat = re.compile(rf'#{kind}\nVnum\s+{vnum}\n.*?{re.escape(END[kind])}\n', re.S)
    m = pat.search(src)
    if not m:
        raise RuntimeError(f'missing {kind} {vnum}')
    return m, m.group(0)

def replace_block(src, kind, vnum, new_block):
    m, _ = get_block(src, kind, vnum)
    return src[:m.start()] + new_block + src[m.end():]

def strip_progs(block):
    return re.sub(r'#MUDPROG\n.*?#ENDPROG\n', '', block, flags=re.S)

def add_progs(block, progs):
    return block.replace(END['MOBILE'] + '\n', progs + END['MOBILE'] + '\n')

def set_name_desc(block, name, lines):
    if len(lines) != 5:
        raise RuntimeError('room descriptions must be five lines')
    block = re.sub(r'Name\s+.*?~\n', f'Name     {name}~\n', block, count=1)
    desc = 'Desc     ' + lines[0] + '\n' + '\n'.join(lines[1:]) + '\n~\n'
    block = re.sub(r'Desc\s+.*?\n~\n', desc, block, count=1, flags=re.S)
    return block

def set_exit(block, direction, toroom, flags=None, key=None):
    pat = re.compile(rf'#EXIT\nDirection {re.escape(direction)}~\nToRoom\s+{toroom}\n.*?#ENDEXIT\n', re.S)
    m = pat.search(block)
    if not m:
        raise RuntimeError(f'missing exit {direction}->{toroom}')
    out = f'#EXIT\nDirection {direction}~\nToRoom    {toroom}\n'
    if flags:
        out += f'Flags     {flags}~\n'
    if key is not None:
        out += f'Key       {key}\n'
    out += '#ENDEXIT\n'
    return block[:m.start()] + out + block[m.end():]

def add_reset(block, reset_line):
    if reset_line in block:
        return block
    return block.replace('#ENDROOM\n', reset_line + '\n#ENDROOM\n')

# ---------------------------------------------------------------------------
# Replace Ghost in the Scanner with Cold Chain.
# ---------------------------------------------------------------------------
_, b = get_block(text, 'MOBILE', 204)
b = strip_progs(b)
b = add_progs(b, '''#MUDPROG
Progtype  rand_prog~
Arglist   8~
Comlist   mpecho A scanner gives a soft chime as the officer waves another passenger through.
~
#ENDPROG
''')
text = replace_block(text, 'MOBILE', 204, b)

_, b = get_block(text, 'MOBILE', 219)
b = strip_progs(b)
b = add_progs(b, '''#MUDPROG
Progtype  rand_prog~
Arglist   8~
Comlist   mpecho The maintenance technician checks a diagnostic pad and reseats a tool in the service belt.
~
#ENDPROG
''')
text = replace_block(text, 'MOBILE', 219, b)

_, b = get_block(text, 'MOBILE', 202)
b = strip_progs(b)
b = add_progs(b, '''#MUDPROG
Progtype  rand_prog~
Arglist   11~
Comlist   mpecho The food-service droid studies a flashing delivery alert and mutters, "Cold-chain problem in the stockroom. SAY DELIVERY if you can help."
mpecho [Quest hint: SAY DELIVERY]
~
#ENDPROG
#MUDPROG
Progtype  speech_prog~
Arglist   delivery shipment cold-chain cold chain stockroom~
Comlist   say A chilled supplier crate arrived with a damaged temperature seal. I need the broken seal brought back for the delivery report.
say The stockroom is staff-only, but I can open it while you check this shipment.
mpoload 255
unlock north
open north
mpjunk stockroom
mpechoat $n [Quest: COLD CHAIN]
mpechoat $n [Next: NORTH; EXAMINE CRATE; GET SEAL; SOUTH; GIVE SEAL DROID.]
~
#ENDPROG
#MUDPROG
Progtype  give_prog~
Arglist   damaged cold-chain cold chain temperature seal~
Comlist   mpechoat $n The food-service droid scans the broken seal and records its batch number.
mpechoaround $n The food-service droid checks a damaged temperature seal against the delivery manifest.
mpjunk damaged
mpoload 255
close north
lock north
mpjunk stockroom
say Logged and isolated. That shipment stays off sale until it is replaced. Take this electrolyte water for the help.
mpoload 208
give electrolyte $n
mpechoat $n [Quest complete: COLD CHAIN]
~
#ENDPROG
''')
text = replace_block(text, 'MOBILE', 202, b)

# Security scanner is interactive but no longer a quest lead.
_, b = get_block(text, 'OBJECT', 223)
b = strip_progs(b)
b = b.replace('#ENDOBJECT\n', '''#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n The recessed maintenance display reports both sensor loops nominal and shows the most recent calibration timestamp.
mpechoaround $n The security arch opens a small green diagnostic panel.
~
#ENDPROG
#ENDOBJECT
''')
text = replace_block(text, 'OBJECT', 223, b)

# Old scanner quest salvage remains hidden in the staff service passage, but is not a quest item.
_, b = get_block(text, 'OBJECT', 249)
b = strip_progs(b)
b = b.replace('#ENDOBJECT\n', '''#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n The cracked ceramic collar and scorched contacts mark this as discarded maintenance scrap. No active work order is attached to it.
mpechoaround $n A tiny amber diode flickers once on the discarded coupler.
~
#ENDPROG
#ENDOBJECT
''')
text = replace_block(text, 'OBJECT', 249, b)

# Remove the obsolete calibrated-coupler quest object.
m, _ = get_block(text, 'OBJECT', 250)
text = text[:m.start()] + text[m.end():]

# Cold Chain key is created only long enough for the shop droid to operate the door.
new_objects = '''#OBJECT
Vnum     255
Keywords food market stockroom staff access key~
Type     key~
Short    a food-market stockroom access key~
Long     A small staff access key is tagged for the food-market stockroom.~
WFlags   take hold~
Values   0 0 0 0 0 0
Stats    1 0 0 0 0
#EXDESC
ExDescKey food market stockroom staff access key~
ExDesc A plain coded key carries a FOOD MARKET - STOCKROOM tag. It is an internal staff key rather than a passenger access token.~
#ENDEXDESC
#ENDOBJECT
#OBJECT
Vnum     256
Keywords damaged cold-chain cold chain temperature seal broken~
Type     trash~
Short    a damaged cold-chain seal~
Long     A split green temperature seal is caught beside the chilled supplier crate.~
WFlags   take hold~
Values   0 0 0 0 0 0
Stats    1 0 0 0 0
#EXDESC
ExDescKey damaged cold-chain cold chain temperature seal broken~
ExDesc The numbered green seal has split through its temperature strip. Its printed batch code matches the chilled supplier crate beside it.~
#ENDEXDESC
#MUDPROG
Progtype  get_prog~
Arglist   100~
Comlist   mpechoat $n The broken temperature strip flashes once as the damaged seal is lifted from the crate.
mpechoaround $n $n retrieves a split green seal from beside the chilled supplier crate.
mpechoat $n [Next: SOUTH; GIVE SEAL DROID.]
~
#ENDPROG
#ENDOBJECT
'''
if '\n#OBJECT\nVnum     255\n' not in text:
    text = text.replace('#ROOM\nVnum     200\n', new_objects + '#ROOM\nVnum     200\n', 1)

# Supplier crate explicitly points to the quest item.
_, b = get_block(text, 'OBJECT', 254)
b = strip_progs(b)
b = b.replace('#ENDOBJECT\n', '''#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n The delivery crate is still cold, but one numbered green temperature seal has split away from the lid. The batch code matches the delivery alert.
mpechoat $n [Next: GET SEAL]
mpechoaround $n The chilled supplier crate gives a quiet refrigeration hum.
~
#ENDPROG
#ENDOBJECT
''')
text = replace_block(text, 'OBJECT', 254, b)

# ---------------------------------------------------------------------------
# Restore the security observation windows.
# ---------------------------------------------------------------------------
_, b = get_block(text, 'ROOM', 208)
b = set_name_desc(b, '&CPrimary Security Screening &w:: &zDen Haag Arrivals', [
    'Security arches span the route beneath cameras and identity readers.',
    'Screening tables and inspection trays stand beside officer stations.',
    'Reinforced observation windows overlook the checkpoint from both sides.',
    'Status panels above each arch show clearance and inspection instructions.',
    'The checkpoint joins customs south with the landside concourse north.'
])
b = set_exit(b, 'east', 210, 'window')
b = set_exit(b, 'west', 211, 'window')
text = replace_block(text, 'ROOM', 208, b)

_, b = get_block(text, 'ROOM', 210)
b = set_name_desc(b, '&CEast Security Control &w:: &zDen Haag Arrivals', [
    'Reinforced glazing overlooks the eastern half of security screening.',
    'Workstations display camera feeds, access logs and lane information.',
    'Evidence cabinets and equipment lockers occupy the rear office wall.',
    'A compact interview desk sits beneath the recording and call system.',
    'The west window overlooks screening while staff access continues south.'
])
b = set_exit(b, 'west', 208, 'window')
text = replace_block(text, 'ROOM', 210, b)

_, b = get_block(text, 'ROOM', 211)
b = set_name_desc(b, '&CWest Security Control &w:: &zDen Haag Arrivals', [
    'Reinforced glazing overlooks the western half of security screening.',
    'Duty consoles display alarms, camera feeds and customs coordination.',
    'Secure lockers and incident equipment fill the rear office recesses.',
    'A small interview station keeps reports away from the public lanes.',
    'The east window overlooks screening while staff access continues south.'
])
b = set_exit(b, 'east', 208, 'window')
text = replace_block(text, 'ROOM', 211, b)

# ---------------------------------------------------------------------------
# Restore hidden locked shop back rooms. They are not public circulation.
# ---------------------------------------------------------------------------
_, b = get_block(text, 'ROOM', 235)
b = set_name_desc(b, '&CNews & Transit Kiosk &w:: &zDen Haag Arrivals', [
    'A compact kiosk displays city maps, news feeds and transit details.',
    'Rotating racks hold printed guides beside an electronic ticket terminal.',
    'A service counter faces the concourse beneath Den Haag transit symbols.',
    'Locked storage access sits behind the passenger-facing kiosk counter.',
    'The kiosk opens south to passenger services beneath bright route signs.'
])
b = set_exit(b, 'north', 236, 'isdoor closed locked secret')
text = replace_block(text, 'ROOM', 235, b)

_, b = get_block(text, 'ROOM', 236)
b = set_name_desc(b, '&CNews Kiosk Stockroom &w:: &zDen Haag Arrivals', [
    'Narrow shelving fills the secure news-and-transit stockroom.',
    'Boxes of maps, guides and schedules are arranged by route and district.',
    'Ticketing supplies and display stock occupy labelled wall cabinets.',
    'An inventory terminal sits beside the locked internal service door.',
    'The concealed staff exit leads south behind the public kiosk counter.'
])
b = set_exit(b, 'south', 235, 'isdoor closed locked secret')
b = add_reset(b, 'Reset O 0 212 1 236')
text = replace_block(text, 'ROOM', 236, b)

_, b = get_block(text, 'ROOM', 238)
b = set_name_desc(b, '&CTravel Supplies Checkout &w:: &zDen Haag Arrivals', [
    'A narrow checkout holds a payment terminal and baggage packing counter.',
    'Travel goods pass beneath pricing and warranty information displays.',
    'The staffed station separates the sales floor from locked reserve stock.',
    'Small shelves hold quick-sale items beside the point-of-sale terminal.',
    'The public route leads east into the travel-supplies sales floor.'
])
b = set_exit(b, 'north', 239, 'isdoor closed locked secret')
text = replace_block(text, 'ROOM', 238, b)

_, b = get_block(text, 'ROOM', 239)
b = set_name_desc(b, '&CTravel Supplies Stockroom &w:: &zDen Haag Arrivals', [
    'Metal shelving holds boxed luggage and reserve travel supplies.',
    'Inventory labels divide stock by equipment type, size and sales area.',
    'A packing bench occupies the centre beneath bright task lighting.',
    'Two locked staff doors separate stock from public and office areas.',
    'The concealed public-side door lies south while the office remains east.'
])
b = set_exit(b, 'south', 238, 'isdoor closed locked secret')
b = add_reset(b, 'Reset O 0 215 1 239')
text = replace_block(text, 'ROOM', 239, b)

_, b = get_block(text, 'ROOM', 249)
b = set_name_desc(b, '&CHealth & Convenience &w:: &zDen Haag Arrivals', [
    'A bright kiosk combines basic medical and travel necessities.',
    'Shelves carry hygiene goods beside first-aid and hydration supplies.',
    'A service counter contains payment and product-information terminals.',
    'Green medical symbols distinguish the kiosk from passenger services.',
    'The kiosk opens south while secured reserve stock remains out of sight.'
])
b = set_exit(b, 'north', 250, 'isdoor closed locked secret')
text = replace_block(text, 'ROOM', 249, b)

_, b = get_block(text, 'ROOM', 250)
b = set_name_desc(b, '&CHealth Kiosk Stockroom &w:: &zDen Haag Arrivals', [
    'Compact shelving stores sealed medical and convenience stock.',
    'Labelled bins separate first-aid supplies from toiletries and consumables.',
    'A small inventory screen tracks stock and scheduled deliveries.',
    'Spare shelving and sealed cartons occupy the remaining wall recesses.',
    'A concealed locked door leads south behind the health-kiosk counter.'
])
b = set_exit(b, 'south', 249, 'isdoor closed locked secret')
b = add_reset(b, 'Reset O 0 219 1 250')
text = replace_block(text, 'ROOM', 250, b)

_, b = get_block(text, 'ROOM', 252)
b = set_name_desc(b, '&CFood Market Checkout &w:: &zDen Haag Arrivals', [
    'A service counter handles purchases from the arrivals food market.',
    'Payment terminals and collection space occupy the public side.',
    'Menu screens list prices, ingredients and current service times.',
    'A locked staff door behind the till controls stockroom access.',
    'The public route leads west into the arrivals food market.'
])
b = set_exit(b, 'north', 253, 'isdoor closed locked secret', 255)
text = replace_block(text, 'ROOM', 252, b)

_, b = get_block(text, 'ROOM', 253)
b = set_name_desc(b, '&CFood Market Stockroom &w:: &zDen Haag Arrivals', [
    'Shelves and chilled cabinets hold reserve food-market stock.',
    'Dry goods fill labelled racks while prepared food stays in cold storage.',
    'A central bench supports stock rotation and delivery checks.',
    'Two locked doors divide the room from checkout and administration.',
    'The concealed checkout door lies south while the office remains west.'
])
b = set_exit(b, 'south', 252, 'isdoor closed locked secret', 255)
b = add_reset(b, 'Reset O 0 256 1 253')
text = replace_block(text, 'ROOM', 253, b)

# ---------------------------------------------------------------------------
# Static validation.
# ---------------------------------------------------------------------------
rooms = re.findall(r'#ROOM\nVnum\s+(\d+)\n(.*?)#ENDROOM\n', text, re.S)
if len(rooms) != 58:
    raise RuntimeError(f'expected 58 rooms, got {len(rooms)}')
for vnum, body in rooms:
    m = re.search(r'Desc\s+(.*?)\n~\n', body, re.S)
    if not m:
        raise RuntimeError(f'room {vnum} missing desc')
    lines = m.group(1).splitlines()
    if len(lines) != 5:
        raise RuntimeError(f'room {vnum} has {len(lines)} description lines')

for needle in [
    'Direction east~\nToRoom    210\nFlags     window~',
    'Direction west~\nToRoom    211\nFlags     window~',
    'Direction west~\nToRoom    208\nFlags     window~',
    'Direction east~\nToRoom    208\nFlags     window~',
    '[Quest hint: SAY DELIVERY]',
    '[Quest complete: COLD CHAIN]',
    'Key       255',
    'Reset O 0 256 1 253',
]:
    if needle not in text:
        raise RuntimeError(f'missing validation marker: {needle}')

for forbidden in ['GHOST IN THE SCANNER', '[Quest hint: SAY SCANNER]', 'OPEN EAST; EAST; SOUTH']:
    if forbidden in text:
        raise RuntimeError(f'obsolete scanner quest text remains: {forbidden}')

for rv, target in [(235,236),(238,239),(249,250),(252,253)]:
    _, rb = get_block(text, 'ROOM', rv)
    if f'ToRoom    {target}\nFlags     isdoor closed locked secret~' not in rb:
        raise RuntimeError(f'room {rv} backroom exit is not hidden+locked')

PATH.write_text(text, encoding='utf-8')
print('Den Haag hidden-backroom pass complete')
print('58 rooms, security windows restored, Cold Chain installed, shop backrooms hidden+locked')
