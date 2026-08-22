from pathlib import Path
import re

ROOT = Path('.')


def read(path):
    return (ROOT / path).read_text(encoding='utf-8')


def write(path, text):
    (ROOT / path).write_text(text, encoding='utf-8')


def replace_once(text, old, new, label):
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f'{label}: expected 1 match, found {count}')
    return text.replace(old, new, 1)


# mud.h publishes quest contracts before PC_DATA uses them.
p = 'src/mud.h'
s = read(p)
s = replace_once(
    s,
    '#include "game/social.h"\n#include "world/affect.h"',
    '#include "game/social.h"\n#include "game/quest.h"\n#include "world/affect.h"',
    'mud.h quest include')
write(p, s)

# Fixed-size player storage: safe with the codebase's calloc PC_DATA allocation.
p = 'src/character/player.h'
s = read(p)
old = '''   short quest_number;  /* current *QUEST BEING DONE* DON'T REMOVE! */
   short quest_curr; /* current number of quest points */
   int quest_accum;  /* quest points accumulated in players life */
'''
new = old + '''   PLAYER_QUEST_STATE quests[MAX_ACTIVE_QUESTS];
   PLAYER_QUEST_HISTORY quest_history[MAX_QUEST_HISTORY];
   PLAYER_REPUTATION quest_reputation[MAX_QUEST_REPUTATIONS];
'''
s = replace_once(s, old, new, 'player quest storage')
write(p, s)

# Export player and MUDProg quest commands.
p = 'src/game/api.h'
s = read(p)
s = replace_once(
    s,
    'DECLARE_DO_FUN( do_quest );\n',
    'DECLARE_DO_FUN( do_quest );\nDECLARE_DO_FUN( do_quests );\nDECLARE_DO_FUN( do_abandon );\n',
    'quest command declarations')
s = replace_once(
    s,
    'DECLARE_DO_FUN( do_mpgain );\n',
    'DECLARE_DO_FUN( do_mpgain );\nDECLARE_DO_FUN( do_mpqueststart );\nDECLARE_DO_FUN( do_mpquestadvance );\nDECLARE_DO_FUN( do_mpquestcomplete );\nDECLARE_DO_FUN( do_mpquestfail );\n',
    'mp quest declarations')
write(p, s)

# Boot quest definitions after skills are ready (reward skill indices can then be used).
p = 'src/db.c'
s = read(p)
s = replace_once(
    s,
    '   load_skill_table(  );\n   sort_skill_table(  );\n',
    '   load_skill_table(  );\n   sort_skill_table(  );\n\n   log_string( "Loading quest definitions..." );\n   load_quests(  );\n',
    'quest boot')
write(p, s)

# Persist active state, objective progress, completion history/cooldowns, reputation.
p = 'src/save.c'
s = read(p)
s = replace_once(
    s,
    '   fprintf( fp, "MGlory       %d\\n", ch->pcdata->quest_accum );\n',
    '   fprintf( fp, "MGlory       %d\\n", ch->pcdata->quest_accum );\n   quest_save_player( fp, ch );\n',
    'quest save writer')
qcase = '''         case 'Q':
            if( quest_load_player_field( ch, word, fp ) )
            {
               fMatch = TRUE;
               break;
            }
            break;

'''
s = replace_once(
    s,
    "         case 'R':\n            KEY( \"Race\", ch->race, fread_number( fp ) );",
    qcase + "         case 'R':\n            KEY( \"Race\", ch->race, fread_number( fp ) );",
    'quest save reader')
write(p, s)

# Compile quest.c and retain the inherited quest command behind a compatibility symbol.
p = 'src/Makefile'
s = read(p)
s = replace_once(
    s,
    'misc.c mssp.c mud_comm.c mud_prog.c natural_progress.c player.c reset.c room_metadata.c save.c sha256.c shops.c \\\n',
    'misc.c mssp.c mud_comm.c mud_prog.c natural_progress.c player.c quest.c reset.c room_metadata.c save.c sha256.c shops.c \\\n',
    'Makefile quest.c')
legacy_candidates = []
for source in Path('src').glob('*.c'):
    if source.name == 'quest.c':
        continue
    body = source.read_text(encoding='utf-8', errors='ignore')
    if re.search(r'\bvoid\s+do_quest\s*\(\s*CHAR_DATA', body):
        legacy_candidates.append(source.stem)
if len(legacy_candidates) != 1:
    raise RuntimeError(f'expected one inherited do_quest definition, found {legacy_candidates}')
legacy_rule = f'o/{legacy_candidates[0]}.o: C_FLAGS += -Ddo_quest=legacy_do_quest\n'
marker = '#\n# Modern output flushing\n#\n'
if legacy_rule not in s:
    s = replace_once(s, marker, '#\n# Modern quest journal owns do_quest; keep the inherited implementation link-safe.\n#\n' + legacy_rule + '\n' + marker, 'legacy do_quest rule')
write(p, s)

# Data-driven commands. Existing `quest` now resolves to quest.c's authoritative symbol.
p = 'system/commands.dat'
s = read(p)
for name in ('quests', 'abandon', 'mpqueststart', 'mpquestadvance', 'mpquestcomplete', 'mpquestfail'):
    if re.search(rf'^Name\s+{re.escape(name)}~$', s, re.M):
        raise RuntimeError(f'commands.dat already has {name}')
extra_commands = '''
#COMMAND
Name        quests~
Code        do_quests
Position    0
Level       0
Log         0
End

#COMMAND
Name        abandon~
Code        do_abandon
Position    0
Level       0
Log         0
End

#COMMAND
Name        mpqueststart~
Code        do_mpqueststart
Position    0
Level       0
Log         0
End

#COMMAND
Name        mpquestadvance~
Code        do_mpquestadvance
Position    0
Level       0
Log         0
End

#COMMAND
Name        mpquestcomplete~
Code        do_mpquestcomplete
Position    0
Level       0
Log         0
End

#COMMAND
Name        mpquestfail~
Code        do_mpquestfail
Position    0
Level       0
Log         0
End
'''
s = s.rstrip() + '\n' + extra_commands
write(p, s)

# MUDProg ifchecks and automatic objective event bridges.
p = 'src/mud_prog.c'
s = read(p)
legacy_if = '''      if( !str_cmp( chck, "doingquest" ) )
      {
         return IS_NPC( actor ) ? FALSE : mprog_veval( chkchar->pcdata->quest_number, opr, atoi( rval ), mob );
      }
'''
modern_if = '''      if( !str_cmp( chck, "questactive" ) )
      {
         return actor && !IS_NPC( actor ) && quest_is_active( actor, atoi( cvar ) );
      }
      if( !str_cmp( chck, "questcomplete" ) )
      {
         return actor && !IS_NPC( actor ) && quest_is_complete( actor, atoi( cvar ) );
      }
      if( !str_cmp( chck, "questfailed" ) )
      {
         return actor && !IS_NPC( actor ) && quest_is_failed( actor, atoi( cvar ) );
      }
      if( !str_cmp( chck, "queststage" ) )
      {
         const int stage = actor && !IS_NPC( actor ) ? quest_stage( actor, atoi( cvar ) ) : 0;
         return !*opr ? stage > 0 : mprog_veval( stage, opr, atoi( rval ), mob );
      }
      if( !str_cmp( chck, "questoutcome" ) )
      {
         const int outcome = actor && !IS_NPC( actor ) ? quest_outcome( actor, atoi( cvar ) ) : 0;
         return !*opr ? outcome != 0 : mprog_veval( outcome, opr, atoi( rval ), mob );
      }
'''
s = replace_once(s, legacy_if, modern_if + legacy_if, 'MUDProg quest ifchecks')

old = '''void mprog_death_trigger( CHAR_DATA * killer, CHAR_DATA * mob )
{
   if( IS_NPC( mob ) && killer != mob && ( mob->pIndexData->progtypes & DEATH_PROG ) )
   {
      mprog_percent_check( mob, killer, NULL, NULL, DEATH_PROG );
   }
   death_cry( mob );
}
'''
new = '''void mprog_death_trigger( CHAR_DATA * killer, CHAR_DATA * mob )
{
   const int quest_mob_vnum = mob && mob->pIndexData ? mob->pIndexData->vnum : 0;

   if( IS_NPC( mob ) && killer != mob && ( mob->pIndexData->progtypes & DEATH_PROG ) )
   {
      mprog_percent_check( mob, killer, NULL, NULL, DEATH_PROG );
   }
   if( killer && !IS_NPC( killer ) && quest_mob_vnum > 0 )
      quest_event( killer, QUEST_OBJECTIVE_KILL, quest_mob_vnum, 0 );
   death_cry( mob );
}
'''
s = replace_once(s, old, new, 'kill quest event')

old = '''void mprog_give_trigger( CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj )
{
   char buf[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;

   if( IS_NPC( mob ) && ( mob->pIndexData->progtypes & GIVE_PROG ) )
'''
new = '''void mprog_give_trigger( CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj )
{
   char buf[MAX_INPUT_LENGTH];
   MPROG_DATA *mprg;
   const int quest_obj_vnum = obj && obj->pIndexData ? obj->pIndexData->vnum : 0;
   const int quest_mob_vnum = mob && mob->pIndexData ? mob->pIndexData->vnum : 0;

   if( IS_NPC( mob ) && ( mob->pIndexData->progtypes & GIVE_PROG ) )
'''
s = replace_once(s, old, new, 'give quest capture')
# Place GIVE progression after the script has consumed/returned the item, using captured vnums.
anchor = '''      }
   }
}

void mprog_greet_trigger( CHAR_DATA * ch )
'''
replacement = '''      }
   }

   if( ch && !IS_NPC( ch ) && quest_obj_vnum > 0 && quest_mob_vnum > 0 )
      quest_event( ch, QUEST_OBJECTIVE_GIVE, quest_obj_vnum, quest_mob_vnum );
}

void mprog_greet_trigger( CHAR_DATA * ch )
'''
s = replace_once(s, anchor, replacement, 'give quest event')

old = '''void mprog_greet_trigger( CHAR_DATA * ch )
{
   CHAR_DATA *vmob, *vmob_next;
'''
new = '''void mprog_greet_trigger( CHAR_DATA * ch )
{
   CHAR_DATA *vmob, *vmob_next;
'''
s = replace_once(s, old, new, 'greet signature guard')
# Emit ENTER after greet scripts so a greet prog may start a quest first.
anchor = '''      else if( vmob->pIndexData->progtypes & ALL_GREET_PROG )
         mprog_percent_check( vmob, ch, NULL, NULL, ALL_GREET_PROG );
   }
}

void mprog_hitprcnt_trigger'''
replacement = '''      else if( vmob->pIndexData->progtypes & ALL_GREET_PROG )
         mprog_percent_check( vmob, ch, NULL, NULL, ALL_GREET_PROG );
   }

   if( ch && !IS_NPC( ch ) && ch->in_room )
      quest_event( ch, QUEST_OBJECTIVE_ENTER, ch->in_room->vnum, 0 );
}

void mprog_hitprcnt_trigger'''
s = replace_once(s, anchor, replacement, 'enter quest event')

# Credit NPC INTERACT only when a speech_prog actually matched, after its script.
start = s.index('void mprog_wordlist_check(')
end = s.index('\nvoid mprog_percent_check(', start)
chunk = s[start:end]
needle = '                  mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );\n                  break;'
if chunk.count(needle) != 2:
    raise RuntimeError(f'interact event: expected 2 speech driver sites, found {chunk.count(needle)}')
chunk = chunk.replace(
    needle,
    '                  mprog_driver( mprg->comlist, mob, actor, obj, vo, FALSE );\n'
    '                  if( actor && !IS_NPC( actor ) && mob && mob->pIndexData && ( type & SPEECH_PROG ) )\n'
    '                     quest_event( actor, QUEST_OBJECTIVE_INTERACT, mob->pIndexData->vnum, 0 );\n'
    '                  break;')
s = s[:start] + chunk + s[end:]

# GET: run object script first, then automatic tracker with a captured stable vnum.
pattern = re.compile(r'''void oprog_get_trigger\( CHAR_DATA \* ch, OBJ_DATA \* obj \)\n\{\n(.*?)\n\}\n''', re.S)
m = pattern.search(s)
if not m:
    raise RuntimeError('oprog_get_trigger not found')
body = m.group(1)
if 'QUEST_OBJECTIVE_GET' in body:
    raise RuntimeError('get quest event already present')
new_body = '   const int quest_obj_vnum = obj && obj->pIndexData ? obj->pIndexData->vnum : 0;\n' + body + '\n\n   if( ch && !IS_NPC( ch ) && quest_obj_vnum > 0 )\n      quest_event( ch, QUEST_OBJECTIVE_GET, quest_obj_vnum, 0 );'
s = s[:m.start()] + 'void oprog_get_trigger( CHAR_DATA * ch, OBJ_DATA * obj )\n{\n' + new_body + '\n}\n' + s[m.end():]

# EXAMINE: same ordering so an exa_prog can start a quest before the event is credited.
pattern = re.compile(r'''void oprog_examine_trigger\( CHAR_DATA \* ch, OBJ_DATA \* obj \)\n\{\n(.*?)\n\}\n''', re.S)
m = pattern.search(s)
if not m:
    raise RuntimeError('oprog_examine_trigger not found')
body = m.group(1)
if 'QUEST_OBJECTIVE_EXAMINE' in body:
    raise RuntimeError('examine quest event already present')
new_body = '   const int quest_obj_vnum = obj && obj->pIndexData ? obj->pIndexData->vnum : 0;\n' + body + '\n\n   if( ch && !IS_NPC( ch ) && quest_obj_vnum > 0 )\n      quest_event( ch, QUEST_OBJECTIVE_EXAMINE, quest_obj_vnum, 0 );'
s = s[:m.start()] + 'void oprog_examine_trigger( CHAR_DATA * ch, OBJ_DATA * obj )\n{\n' + new_body + '\n}\n' + s[m.end():]
write(p, s)

# Migrate the two Den Haag quests from item-as-state scripting to authoritative quest state.
p = 'area/Earth/DenHaagArrivals.are'
s = read(p)

# Cold Chain start/opening.
old = '''Comlist   say A chilled supplier crate arrived with a damaged temperature seal. I need the broken seal brought back for the delivery report.
say The stockroom is staff-only, but I can open it while you check this shipment.
mpoload 255
unlock north
open north
mpjunk stockroom
mpechoat $n [Quest: COLD CHAIN]
mpechoat $n [Next: NORTH; EXAMINE CRATE; GET SEAL; SOUTH; GIVE SEAL DROID.]
~
'''
new = '''Comlist   mpqueststart $n 2002
if questactive(2002)
say A chilled supplier crate arrived with a damaged temperature seal. I need the broken seal brought back for the delivery report.
say The stockroom is staff-only, but I can open it while you check this shipment.
mpoload 255
unlock north
open north
mpjunk stockroom
mpechoat $n [Next: NORTH; EXAMINE CRATE; GET SEAL; SOUTH; GIVE SEAL DROID.]
endif
~
'''
s = replace_once(s, old, new, 'Cold Chain start')

old = '''Comlist   mpechoat $n The food-service droid scans the broken seal and records its batch number.
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
'''
new = '''Comlist   if queststage(2002) == 3
mpechoat $n The food-service droid scans the broken seal and records its batch number.
mpechoaround $n The food-service droid checks a damaged temperature seal against the delivery manifest.
mpjunk damaged
mpoload 255
close north
lock north
mpjunk stockroom
say Logged and isolated. That shipment stays off sale until it is replaced. Thank you for the help.
else
say I do not have an active delivery report for that seal.
give damaged $n
endif
~
'''
s = replace_once(s, old, new, 'Cold Chain hand-in')

# Wrong Bag quest start.
old = '''Comlist   say My black case was exchanged for another after the interstellar arrival.
say Lost Property is west of Alpha Gate Junction. Ask the clerk about LUGGAGE and check their archive for the routing tag.
mpechoat $n [Next: Find Lost Property Reception and SAY LUGGAGE.]
~
'''
new = '''Comlist   mpqueststart $n 2001
if questactive(2001)
say My black case was exchanged for another after the interstellar arrival.
say Lost Property is west of Alpha Gate Junction. Ask the clerk about LUGGAGE and check their archive for the routing tag.
mpechoat $n [Next: Find Lost Property Reception and SAY LUGGAGE.]
endif
~
'''
s = replace_once(s, old, new, 'Wrong Bag start')

# Information handoff must be at objective stage 2; otherwise return the tag.
old = '''Comlist   mpechoat $n The information clerk runs the routing code through the terminal register.
mpechoaround $n The information clerk checks a baggage-routing tag against the terminal register.
say This tag was diverted through Alpha by mistake. Lost Property is holding the matching black case.
mpjunk misrouted
mpoload 247
give verified $n
mpechoat $n [Next: Return to Lost Property Reception and GIVE SLIP CLERK.]
~
'''
new = '''Comlist   if queststage(2001) == 2
mpechoat $n The information clerk runs the routing code through the terminal register.
mpechoaround $n The information clerk checks a baggage-routing tag against the terminal register.
say This tag was diverted through Alpha by mistake. Lost Property is holding the matching black case.
mpjunk misrouted
mpoload 247
give verified $n
mpechoat $n [Next: Return to Lost Property Reception and GIVE SLIP CLERK.]
else
say I cannot verify this without an active baggage claim.
give misrouted $n
endif
~
'''
s = replace_once(s, old, new, 'Wrong Bag information handoff')

# Lost Property handoff.
old = '''Comlist   mpechoat $n The clerk scans the release slip and opens the matching property record.
mpechoaround $n The Lost Property clerk checks a release slip against the secure archive.
say Found it. Black case, interstellar claim. Return it to its owner in the western gate lounges.
mpjunk verified
mpoload 248
give recovered $n
mpechoat $n [Next: Find the tired business traveller and GIVE SUITCASE TRAVELLER.]
~
'''
new = '''Comlist   if queststage(2001) == 3
mpechoat $n The clerk scans the release slip and opens the matching property record.
mpechoaround $n The Lost Property clerk checks a release slip against the secure archive.
say Found it. Black case, interstellar claim. Return it to its owner in the western gate lounges.
mpjunk verified
mpoload 248
give recovered $n
mpechoat $n [Next: Find the tired business traveller and GIVE SUITCASE TRAVELLER.]
else
say This release slip is not attached to an active claim.
give verified $n
endif
~
'''
s = replace_once(s, old, new, 'Wrong Bag Lost Property handoff')

# Final handoff: engine awards item/credits/XP after this give_prog returns.
old = '''Comlist   mpechoat $n The traveller compares the case number with a battered claim stub and unlocks the suitcase.
mpechoaround $n The traveller checks the recovered suitcase against a battered claim stub.
say That's it. Thank you. Take my spare local transit card; I will not need it now.
mpjunk recovered
mpoload 229
give transit $n
mpechoat $n [Quest complete: WRONG BAG]
~
'''
new = '''Comlist   if queststage(2001) == 4
mpechoat $n The traveller compares the case number with a battered claim stub and unlocks the suitcase.
mpechoaround $n The traveller checks the recovered suitcase against a battered claim stub.
say That's it. Thank you. Keep my spare local transit card; I will not need it now.
mpjunk recovered
else
say That is not the case attached to an active claim of mine.
give recovered $n
endif
~
'''
s = replace_once(s, old, new, 'Wrong Bag completion handoff')
write(p, s)

# Static invariants.
area = read('area/Earth/DenHaagArrivals.are')
if len(re.findall(r'^#ROOM$', area, re.M)) != 58:
    raise RuntimeError('Den Haag room count changed')
room208 = re.search(r'#ROOM\nVnum\s+208\n.*?#ENDROOM', area, re.S).group(0)
for direction, target in [('east', '210'), ('west', '211')]:
    if not re.search(rf'Direction {direction}~\nToRoom\s+{target}\nFlags\s+window~', room208):
        raise RuntimeError(f'Security {direction} observation window was not preserved')
for public, hidden in [(235, 236), (238, 239), (249, 250), (252, 253)]:
    block = re.search(rf'#ROOM\nVnum\s+{public}\n.*?#ENDROOM', area, re.S).group(0)
    if not re.search(rf'ToRoom\s+{hidden}\nFlags\s+isdoor closed locked secret~', block):
        raise RuntimeError(f'shop backroom {public}->{hidden} is not hidden+locked')

commands = read('system/commands.dat')
for name in ('quest', 'quests', 'abandon', 'mpqueststart', 'mpquestadvance', 'mpquestcomplete', 'mpquestfail'):
    if len(re.findall(rf'^Name\s+{name}~$', commands, re.M)) != 1:
        raise RuntimeError(f'command {name} missing or duplicated')

print(f'Quest integration complete; inherited do_quest is in {legacy_candidates[0]}.c')
