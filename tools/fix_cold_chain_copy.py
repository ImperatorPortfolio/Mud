from pathlib import Path
p = Path('area/Earth/DenHaagArrivals.are')
s = p.read_text(encoding='utf-8')
old = 'ExDesc The reusable crate carries fresh-produce labels from a regional hydroponic cooperative. Temperature seals along the lid remain green and intact.~'
new = 'ExDesc The reusable crate carries fresh-produce labels from a regional hydroponic cooperative. Most temperature seals remain green, but one numbered seal has split away from the lid.~'
if old not in s:
    raise SystemExit('supplier-crate description marker not found')
s = s.replace(old, new, 1)
if 'GHOST IN THE SCANNER' in s:
    raise SystemExit('obsolete scanner quest remains')
if s.count('Flags     window~') < 4:
    raise SystemExit('security windows not restored')
for target in (236, 239, 250, 253):
    marker = f'ToRoom    {target}\nFlags     isdoor closed locked secret~'
    if marker not in s:
        raise SystemExit(f'backroom {target} is not hidden and locked')
p.write_text(s, encoding='utf-8')
print('Cold Chain copy and hidden-room validation passed')
