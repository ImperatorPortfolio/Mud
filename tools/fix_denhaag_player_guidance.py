from pathlib import Path
import re

AREA = Path('area/Earth/DenHaagArrivals.are')
text = AREA.read_text(encoding='utf-8')


def section(kind, vnum):
    pat = re.compile(rf"(#{kind}\nVnum\s+{vnum}\n.*?#END{kind})", re.S)
    m = pat.search(text)
    if not m:
        raise SystemExit(f'missing {kind} {vnum}')
    return m


def replace_section(kind, vnum, fn):
    global text
    m = section(kind, vnum)
    new = fn(m.group(1))
    if new == m.group(1):
        raise SystemExit(f'no change for {kind} {vnum}')
    text = text[:m.start()] + new + text[m.end():]


def add_prog(kind, vnum, prog):
    def fn(s):
        marker = f'#END{kind}'
        if prog.strip() in s:
            return s
        return s.replace(marker, prog.rstrip() + '\n' + marker, 1)
    replace_section(kind, vnum, fn)


def replace_prog(vnum, progtype, arglist, comlist):
    def fn(s):
        pat = re.compile(
            rf"(#MUDPROG\nProgtype\s+{re.escape(progtype)}~\nArglist\s+{re.escape(arglist)}~\nComlist\s+)(.*?)(\n~\n#ENDPROG)",
            re.S,
        )
        m = pat.search(s)
        if not m:
            raise SystemExit(f'missing prog {vnum} {progtype} {arglist}')
        return s[:m.start()] + m.group(1) + comlist.rstrip() + m.group(3) + s[m.end():]
    replace_section('MOBILE', vnum, fn)


def replace_obj_prog(vnum, progtype, arglist, comlist):
    def fn(s):
        pat = re.compile(
            rf"(#MUDPROG\nProgtype\s+{re.escape(progtype)}~\nArglist\s+{re.escape(arglist)}~\nComlist\s+)(.*?)(\n~\n#ENDPROG)",
            re.S,
        )
        m = pat.search(s)
        if not m:
            raise SystemExit(f'missing obj prog {vnum} {progtype} {arglist}')
        return s[:m.start()] + m.group(1) + comlist.rstrip() + m.group(3) + s[m.end():]
    replace_section('OBJECT', vnum, fn)


# Quest 1: Wrong Bag. Every stage explicitly advertises the command the player needs.
add_prog('MOBILE', 218, '''#MUDPROG
Progtype  rand_prog~
Arglist   12~
Comlist   mpecho A tired business traveller checks an empty claim stub and mutters, "Lost luggage again... if anyone can help, ask me about my LUGGAGE."
mpecho [Quest hint: SAY LUGGAGE]
~
#ENDPROG
''')
replace_prog(218, 'speech_prog', 'wrong bag luggage suitcase', '''say My black case was exchanged for another after the interstellar arrival.
say Lost Property is west of Alpha Gate Junction. Ask the clerk about LUGGAGE and check their archive for the routing tag.
mpechoat $n [Next: Find Lost Property Reception and SAY LUGGAGE.]''')
replace_prog(218, 'give_prog', 'recovered business suitcase black luggage', '''mpechoat $n The traveller compares the case number with a battered claim stub and unlocks the suitcase.
mpechoaround $n The traveller checks the recovered suitcase against a battered claim stub.
say That's it. Thank you. Take my spare local transit card; I will not need it now.
mpjunk recovered
mpoload 229
give transit $n
mpechoat $n [Quest complete: WRONG BAG]''')

add_prog('MOBILE', 217, '''#MUDPROG
Progtype  rand_prog~
Arglist   10~
Comlist   mpecho The Lost Property clerk checks the claim register and calls, "Missing a bag? SAY LUGGAGE and I can check the archive."
mpecho [Quest hint: SAY LUGGAGE]
~
#ENDPROG
''')
replace_prog(217, 'speech_prog', 'bag luggage suitcase claim', '''say Misrouted baggage is held in the archive until its routing tag can be verified at an information centre.
say The archive is directly north. Check the sorting trays for a tag.
mpechoat $n [Next: NORTH; EXAMINE TAG; GET TAG.]''')
replace_prog(217, 'give_prog', 'verified baggage release slip alpha', '''mpechoat $n The clerk scans the release slip and opens the matching property record.
mpechoaround $n The Lost Property clerk checks a release slip against the secure archive.
say Found it. Black case, interstellar claim. Return it to its owner in the western gate lounges.
mpjunk verified
mpoload 248
give recovered $n
mpechoat $n [Next: Find the tired business traveller and GIVE SUITCASE TRAVELLER.]''')

add_prog('MOBILE', 214, '''#MUDPROG
Progtype  rand_prog~
Arglist   8~
Comlist   mpecho A route panel beside the information clerk reads: SAY ALPHA, SAY BRAVO, SAY CHARLIE or SAY DELTA for directions.
~
#ENDPROG
''')
replace_prog(214, 'give_prog', 'misrouted baggage routing tag alpha transfer', '''mpechoat $n The information clerk runs the routing code through the terminal register.
mpechoaround $n The information clerk checks a baggage-routing tag against the terminal register.
say This tag was diverted through Alpha by mistake. Lost Property is holding the matching black case.
mpjunk misrouted
mpoload 247
give verified $n
mpechoat $n [Next: Return to Lost Property Reception and GIVE SLIP CLERK.]''')

replace_obj_prog(246, 'get_prog', '100', '''mpechoat $n The routing tag carries a mismatched Alpha transfer code and a Lost Property verification mark.
mpechoaround $n $n lifts a narrow routing tag from the archive sorting trays.
mpechoat $n [Next: Take this to either Information Centre and GIVE TAG CLERK.]''')
add_prog('OBJECT', 246, '''#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n The handwritten note says: VERIFY ROUTING AT INFORMATION BEFORE RELEASE.
mpechoat $n [Next: GET TAG]
~
#ENDPROG
''')

# Quest 2: Ghost in the Scanner. Make the route physically traversable and signpost every command.
add_prog('MOBILE', 204, '''#MUDPROG
Progtype  rand_prog~
Arglist   12~
Comlist   mpecho The screening officer frowns at an amber scanner warning and mutters, "False alarm again. If someone can help, ask me about the SCANNER."
mpecho [Quest hint: SAY SCANNER]
~
#ENDPROG
''')
replace_prog(204, 'speech_prog', 'scanner fault false alarm maintenance', '''say The east sensor loop keeps throwing false positives. Start by examining the security arch.
say The east control door is usable from this checkpoint; beyond it, go south into the service passage and retrieve the damaged coupler.
mpechoat $n [Next: EXAMINE ARCH; OPEN EAST; EAST; SOUTH; EXAMINE COUPLER; GET COUPLER.]''')
replace_prog(204, 'give_prog', 'calibrated security sensor coupler scanner', '''mpechoat $n The screening officer seats the calibrated coupler and runs a short diagnostic cycle.
mpechoaround $n The screening officer fits a replacement component into the scanner console.
mpecho The security arch pulses amber once, then settles to a steady green status light.
say Clean signal. That fixed it. Take this spare power cell for the trouble.
mpjunk calibrated
mpoload 215
give replacement $n
mpechoat $n [Quest complete: GHOST IN THE SCANNER]''')

replace_obj_prog(223, 'exa_prog', '100', '''mpechoat $n The maintenance display reports EAST SENSOR LOOP 2: COUPLER IMPEDANCE HIGH.
mpechoat $n A service route diagram shows the east control door, then the passage immediately south.
mpechoaround $n The security arch opens a small amber diagnostic panel.
mpechoat $n [Next: OPEN EAST; EAST; SOUTH; EXAMINE COUPLER; GET COUPLER.]''')
replace_obj_prog(249, 'get_prog', '100', '''mpechoat $n A tiny diagnostic diode flashes amber when the damaged coupler is moved.
mpechoaround $n $n retrieves a heat-marked component from beside the service panel.
mpechoat $n [Next: Take this to the maintenance technician at Alpha Shuttle Berth. If lost, ask an information clerk with SAY ALPHA. Then GIVE COUPLER TECHNICIAN.]''')
add_prog('OBJECT', 249, '''#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n The cracked ceramic collar and scorched contacts match the scanner diagnostic exactly.
mpechoat $n [Next: GET COUPLER]
~
#ENDPROG
''')
add_prog('MOBILE', 219, '''#MUDPROG
Progtype  rand_prog~
Arglist   9~
Comlist   mpecho The maintenance technician checks a calibration pad and says, "Scanner fault parts go through me. Bring me the COUPLER."
mpecho [Quest hint: GIVE COUPLER TECHNICIAN]
~
#ENDPROG
''')
replace_prog(219, 'give_prog', 'faulted security sensor coupler scanner', '''mpechoat $n The technician checks the scorched contacts, then matches the serial against a replacement part.
mpechoaround $n The maintenance technician compares a damaged coupler with a diagnostic pad.
say The ceramic collar is cracked. Here is a calibrated replacement for the screening officer.
mpjunk faulted
mpoload 250
give calibrated $n
mpechoat $n [Next: Return to Primary Security Screening and GIVE COUPLER OFFICER.]''')

# Turn the four bogus window links between Screening and the control offices into real glazed security doors.
for vnum in (208, 210, 211):
    def make_doors(s):
        if 'Flags     window~' not in s:
            raise SystemExit(f'expected window exit in room {vnum}')
        return s.replace('Flags     window~', 'Flags     isdoor closed~')
    replace_section('ROOM', vnum, make_doors)

# Public-facing shop back areas: customer archive/collection rooms are accessible; private offices stay locked.
public_pairs = [(235,236,'north'), (236,235,'south'), (238,239,'north'), (239,238,'south'), (249,250,'north'), (250,249,'south'), (252,253,'north'), (253,252,'south')]
for vnum, dest, direction in public_pairs:
    def unlock_public(s, dest=dest, direction=direction):
        pat = re.compile(rf"(#EXIT\nDirection {direction}~\nToRoom\s+{dest}\nFlags\s+isdoor closed) locked~")
        new, n = pat.subn(r"\1~", s, count=1)
        if n != 1:
            raise SystemExit(f'expected locked public shop door {vnum}->{dest}')
        return new
    replace_section('ROOM', vnum, unlock_public)

# Update shop/public back-room names and five-line descriptions so OPEN NORTH is visible without psychic knowledge.
room_text = {
235: ('&CNews & Transit Kiosk &w:: &zDen Haag Arrivals~', [
'A compact kiosk displays city maps, news feeds and transit details.',
'Rotating racks hold printed guides beside an electronic ticket terminal.',
'A service counter faces the concourse beneath Den Haag transit symbols.',
'A sign beside the north door reads CUSTOMER ARCHIVE - OPEN NORTH.',
'The kiosk opens south to services and north into its public archive.'
]),
236: ('&CNews & Transit Archive &w:: &zDen Haag Arrivals~', [
'Narrow shelving fills a customer archive behind the news-and-transit kiosk.',
'Older maps, route notices and retired schedules are filed by district.',
'A route-history board records recent changes to surface connections.',
'Current sales stock remains separated behind the kiosk counter.',
'The only public exit leads south through the archive door.'
]),
238: ('&CTravel Supplies Checkout &w:: &zDen Haag Arrivals~', [
'A narrow checkout holds a payment terminal and baggage packing counter.',
'Travel goods pass beneath pricing and warranty information displays.',
'The staffed station separates the sales floor from reserve stock.',
'A sign beside the north door reads COLLECTION ROOM - OPEN NORTH.',
'Routes lead east into the shop and north into customer collections.'
]),
239: ('&CTravel Supplies Collection Room &w:: &zDen Haag Arrivals~', [
'Metal shelving holds boxed luggage, returns and reserve travel supplies.',
'Collection labels divide customer orders by equipment type and size.',
'A packing bench occupies the centre beneath bright task lighting.',
'A locked staff office remains east beyond the customer collection area.',
'The public route returns south to the travel-supplies checkout.'
]),
249: ('&CHealth & Convenience &w:: &zDen Haag Arrivals~', [
'A bright kiosk combines basic medical and travel necessities.',
'Shelves carry hygiene goods beside first-aid and hydration supplies.',
'A service counter contains payment and product-information terminals.',
'A sign beside the north door reads COLLECTION ROOM - OPEN NORTH.',
'The kiosk opens south to concourse and north into customer collections.'
]),
250: ('&CHealth Collection Room &w:: &zDen Haag Arrivals~', [
'Compact shelving stores sealed medical and convenience reserve stock.',
'Labelled bins separate first-aid supplies from toiletries and consumables.',
'A recall board lists current batch checks and product safety notices.',
'Customer collections occupy the open shelving nearest the doorway.',
'The only public exit leads south through the collection-room door.'
]),
252: ('&CFood Market Checkout &w:: &zDen Haag Arrivals~', [
'A service counter handles purchases from the arrivals food market.',
'Payment terminals and collection space occupy the public side.',
'Menu screens list prices, ingredients and current service times.',
'A sign beside the north door reads COLLECTION ROOM - OPEN NORTH.',
'Routes lead west into the market and north into customer collections.'
]),
253: ('&CFood Market Collection Room &w:: &zDen Haag Arrivals~', [
'Shelves and chilled cabinets hold reserve food and pre-ordered collections.',
'Dry goods fill labelled racks while prepared food stays in cold storage.',
'A central bench supports stock rotation and passenger order collection.',
'A locked staff office remains west beyond the customer collection area.',
'The public route returns south to the food-market checkout.'
]),
}
for vnum, (name, lines) in room_text.items():
    def rewrite_room(s, name=name, lines=lines):
        s = re.sub(r'Name\s+.*?~\n', 'Name     ' + name + '\n', s, count=1)
        s = re.sub(r'Desc\s+.*?\n~\n', 'Desc     ' + '\n'.join(lines) + '\n~\n', s, count=1, flags=re.S)
        return s
    replace_section('ROOM', vnum, rewrite_room)

# Add one interactable discovery to each newly accessible shop back area.
new_objects = r'''#OBJECT
Vnum     251
Keywords route history board archive notice transit~
Type     furniture~
Short    a route-history board~
Long     A route-history board displays retired transit notices and old route changes.~
Values   0 0 0 0 0 0
Stats    5 0 0 0 0
#EXDESC
ExDescKey route history board archive notice transit~
ExDesc Layers of archived notices show how surface routes around Den Haag have shifted as orbital traffic increased. Several obsolete shuttle symbols have been crossed out by hand.~
#ENDEXDESC
#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n One archived notice marks the terminal expansion that created the present Alpha-Bravo and Charlie-Delta gate split.
mpechoaround $n The route-history board scrolls back through several years of retired transit notices.
~
#ENDPROG
#ENDOBJECT
#OBJECT
Vnum     252
Keywords returns crate travel supplies collection damaged labels~
Type     furniture~
Short    a marked returns crate~
Long     A marked returns crate sits beside the travel-supplies packing bench.~
Values   0 0 0 0 0 0
Stats    10 0 0 0 0
#EXDESC
ExDescKey returns crate travel supplies collection damaged labels~
ExDesc Returned chargers, luggage fittings and packaging are sorted into labelled compartments. Most are mundane warranty returns awaiting collection by a supplier.~
#ENDEXDESC
#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n A returns label shows that terminal shops consolidate damaged electronics here before nightly supplier pickup.
mpechoaround $n Packaging rustles as the marked returns crate is inspected.
~
#ENDPROG
#ENDOBJECT
#OBJECT
Vnum     253
Keywords medical recall board batch notice health collection~
Type     furniture~
Short    a medical recall board~
Long     A medical recall board lists batch checks for the kiosk's reserve stock.~
Values   0 0 0 0 0 0
Stats    5 0 0 0 0
#EXDESC
ExDescKey medical recall board batch notice health collection~
ExDesc The board lists medpac seal checks, hydration-stock dates and hygiene-product batch numbers. All current rows are marked cleared for sale.~
#ENDEXDESC
#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n The newest entry records a routine seal inspection on the kiosk's compact travel medpacs; every tested batch passed.
mpechoaround $n The medical recall board refreshes its batch-status column.
~
#ENDPROG
#ENDOBJECT
#OBJECT
Vnum     254
Keywords supplier delivery crate produce food market collection~
Type     furniture~
Short    a chilled supplier delivery crate~
Long     A chilled supplier crate waits beside the food-market collection shelves.~
Values   0 0 0 0 0 0
Stats    15 0 0 0 0
#EXDESC
ExDescKey supplier delivery crate produce food market collection~
ExDesc The reusable crate carries fresh-produce labels from a regional hydroponic cooperative. Temperature seals along the lid remain green and intact.~
#ENDEXDESC
#MUDPROG
Progtype  exa_prog~
Arglist   100~
Comlist   mpechoat $n The delivery label traces the produce to a hydroponic cooperative outside central Den Haag, packed only a few hours ago.
mpechoaround $n A green temperature seal glows briefly on the chilled delivery crate.
~
#ENDPROG
#ENDOBJECT
'''
if re.search(r'#OBJECT\nVnum\s+251\n', text):
    raise SystemExit('new object vnums already present')
text = text.replace('#ROOM\nVnum     200\n', new_objects + '#ROOM\nVnum     200\n', 1)

# Add resets for the four shop discoveries while staying within the 0/1/2 density rule.
reset_additions = {
236: 'Reset O 0 251 1 236',
239: 'Reset O 0 252 1 239',
250: 'Reset O 0 253 1 250',
253: 'Reset O 0 254 1 253',
}
for vnum, reset in reset_additions.items():
    def add_reset(s, reset=reset):
        if reset in s:
            return s
        return s.replace('#ENDROOM', reset + '\n#ENDROOM', 1)
    replace_section('ROOM', vnum, add_reset)

# Validation: room descriptions remain exactly five lines and player-facing routes are valid.
rooms = re.findall(r'#ROOM\nVnum\s+(\d+)\n(.*?)#ENDROOM', text, re.S)
if len(rooms) != 58:
    raise SystemExit(f'expected 58 rooms, got {len(rooms)}')
for vnum, body in rooms:
    m = re.search(r'Desc\s+(.*?)\n~\n', body, re.S)
    if not m:
        raise SystemExit(f'room {vnum} missing desc')
    lines = m.group(1).splitlines()
    if len(lines) != 5:
        raise SystemExit(f'room {vnum} has {len(lines)} desc lines')

for vnum in (208, 210, 211):
    m = re.search(rf'#ROOM\nVnum\s+{vnum}\n(.*?)#ENDROOM', text, re.S)
    if 'Flags     window~' in m.group(1):
        raise SystemExit(f'room {vnum} still has window-only player exit')

for vnum, reset in reset_additions.items():
    if reset not in text:
        raise SystemExit(f'missing reset {reset}')

for cmd in ('SAY LUGGAGE', 'EXAMINE TAG', 'GIVE TAG CLERK', 'SAY SCANNER', 'EXAMINE ARCH', 'GET COUPLER', 'GIVE COUPLER TECHNICIAN', 'GIVE COUPLER OFFICER'):
    if cmd not in text:
        raise SystemExit(f'missing player guidance {cmd}')

AREA.write_text(text, encoding='utf-8')
print('Den Haag player-guidance pass complete')
print('58 rooms; quest commands signposted; security doors traversable; four shop back areas active')
