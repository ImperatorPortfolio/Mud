from pathlib import Path
import re

path = Path("area/Earth/DenHaagArrivals.are")
text = path.read_text()

# Remove obsolete legacy object prototypes no longer used by the terminal.
remove_objs = {200, 201, 203, 210, 225}

def obj_repl(match):
    block = match.group(0)
    vm = re.search(r"^Vnum\s+(\d+)", block, re.M)
    return "" if vm and int(vm.group(1)) in remove_objs else block

text = re.sub(r"#OBJECT\n.*?#ENDOBJECT\n\n?", obj_repl, text, flags=re.S)

# Keep physical room contents sparse: most rooms empty, ordinary rooms 0-1,
# busy/service rooms up to 2. Shopkeeper inventory is carried, not room clutter.
room_resets = {
    201:["Reset M 0 209 1 201","Reset O 0 221 1 201"],
    202:["Reset M 0 208 1 202"], 203:["Reset M 0 218 2 203"],
    204:[], 205:[],
    206:["Reset M 0 207 2 206","Reset M 0 216 1 206","Reset O 0 221 1 206"],
    207:["Reset M 0 213 2 207","Reset O 0 222 1 207"],
    208:["Reset M 0 204 2 208","Reset O 0 223 1 208","Reset O 0 238 1 208"],
    209:[], 210:["Reset M 0 203 1 210"], 211:["Reset M 0 203 2 211"],
    212:[], 213:[], 214:[], 215:[], 216:[], 217:[], 218:[], 219:[],
    220:["Reset M 0 207 3 220"], 221:[], 222:["Reset M 0 214 2 222"],
    223:["Reset M 0 215 4 223"], 224:["Reset M 0 218 2 224"],
    225:["Reset M 0 215 4 225"], 226:["Reset M 0 214 2 226"],
    227:["Reset M 0 215 4 227"], 228:[], 229:["Reset M 0 215 4 229"],
    230:["Reset M 0 217 1 230","Reset O 0 245 1 230"],
    231:["Reset O 0 239 1 231","Reset O 0 227 1 231"], 232:[], 233:[],
    234:["Reset M 0 219 1 234","Reset O 0 242 1 234"],
    235:["Reset M 0 211 1 235","  Reset G 1 209 1","  Reset G 1 211 1","  Reset G 1 212 1","Reset O 0 245 1 235"],
    236:[], 237:["Reset O 0 243 1 237","Reset O 0 235 1 237"],
    238:["Reset M 0 201 1 238","  Reset G 1 202 1","  Reset G 1 213 1","  Reset G 1 214 1","  Reset G 1 215 1","  Reset G 1 216 1","Reset O 0 245 1 238"],
    239:["Reset O 0 243 1 239"], 240:[], 241:[],
    242:["Reset M 0 207 3 242","Reset O 0 220 1 242"],
    243:["Reset O 0 242 1 243"],
    244:["Reset O 0 241 1 244","Reset O 0 240 1 244"],
    245:["Reset O 0 230 1 245"],
    246:["Reset O 0 241 1 246","Reset O 0 240 1 246"],
    247:[], 248:["Reset O 0 242 1 248"],
    249:["Reset M 0 212 1 249","  Reset G 1 217 1","  Reset G 1 218 1","  Reset G 1 219 1","  Reset G 1 208 1","Reset O 0 245 1 249"],
    250:[], 251:["Reset O 0 243 1 251","Reset O 0 244 1 251"],
    252:["Reset M 0 202 1 252","  Reset G 1 204 1","  Reset G 1 205 1","  Reset G 1 206 1","  Reset G 1 207 1","  Reset G 1 208 1","Reset O 0 245 1 252"],
    253:["Reset O 0 244 1 253"], 254:[], 255:[],
    256:["Reset O 0 220 1 256"], 257:["Reset O 0 242 1 257"],
}

def room_repl(match):
    block = match.group(0)
    vm = re.search(r"^Vnum\s+(\d+)", block, re.M)
    if not vm:
        return block
    vnum = int(vm.group(1))
    if vnum not in room_resets:
        return block

    lines = block.splitlines()
    kept = [line for line in lines if not re.match(r"^\s*Reset\s+[MOGEPDRHT]\b", line)]
    insert_at = next((i for i, line in enumerate(kept) if line == "#MUDPROG"), len(kept) - 1)
    resets = room_resets[vnum]
    if resets:
        while insert_at > 0 and kept[insert_at - 1] == "":
            kept.pop(insert_at - 1)
            insert_at -= 1
        chunk = [""] + resets
        if insert_at < len(kept) and kept[insert_at] != "#ENDROOM":
            chunk.append("")
        kept[insert_at:insert_at] = chunk
    return "\n".join(kept) + "\n"

text = re.sub(r"#ROOM\n.*?#ENDROOM\n", room_repl, text, flags=re.S)

# Make controlled doors reciprocal.
def patch_exit(room_vnum, direction):
    global text
    room_match = re.search(rf"(#ROOM\nVnum\s+{room_vnum}\n.*?#ENDROOM\n)", text, re.S)
    if not room_match:
        raise RuntimeError(f"room {room_vnum} not found")
    block = room_match.group(1)
    exit_match = re.search(rf"(#EXIT\nDirection {direction}~\n.*?#ENDEXIT)", block, re.S)
    if not exit_match:
        raise RuntimeError(f"exit {room_vnum} {direction} not found")
    exit_block = exit_match.group(1)
    flags = "isdoor closed locked"
    if re.search(r"^Flags\s+", exit_block, re.M):
        exit_block = re.sub(r"^Flags\s+.*$", f"Flags     {flags}~", exit_block, flags=re.M)
    else:
        exit_block = re.sub(r"(ToRoom\s+\d+\n)", r"\1Flags     " + flags + "~\n", exit_block, 1)
    block = block[:exit_match.start(1)] + exit_block + block[exit_match.end(1):]
    text = text[:room_match.start(1)] + block + text[room_match.end(1):]

for room_vnum, direction in [(207,"east"),(212,"west"),(207,"west"),(213,"east"),(230,"south"),(232,"north")]:
    patch_exit(room_vnum, direction)

path.write_text(text)
