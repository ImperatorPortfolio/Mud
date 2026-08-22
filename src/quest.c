/***************************************************************************
 * Zero Point - persistent multi-quest engine
 *
 * Quest definitions and player progression live here. MUDProgs remain the
 * presentation/world-interaction layer and call the small mpquest* API.
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mud.h"

#define QUEST_FILE "../system/quests.dat"

QUEST_DEFINITION *first_quest = NULL;
QUEST_DEFINITION *last_quest = NULL;

void gain_exp( CHAR_DATA *ch, long gain, int ability );
CHAR_DATA *get_char_room_mp( CHAR_DATA *ch, const char *argument );

namespace
{
int quest_objective_type_from_name( const char *name )
{
   if( !str_cmp( name, "kill" ) )      return QUEST_OBJECTIVE_KILL;
   if( !str_cmp( name, "get" ) )       return QUEST_OBJECTIVE_GET;
   if( !str_cmp( name, "give" ) )      return QUEST_OBJECTIVE_GIVE;
   if( !str_cmp( name, "enter" ) )     return QUEST_OBJECTIVE_ENTER;
   if( !str_cmp( name, "examine" ) )   return QUEST_OBJECTIVE_EXAMINE;
   if( !str_cmp( name, "interact" ) )  return QUEST_OBJECTIVE_INTERACT;
   if( !str_cmp( name, "custom" ) )    return QUEST_OBJECTIVE_CUSTOM;
   return QUEST_OBJECTIVE_NONE;
}

const char *quest_objective_type_name( int type )
{
   switch( type )
   {
      case QUEST_OBJECTIVE_KILL:     return "kill";
      case QUEST_OBJECTIVE_GET:      return "get";
      case QUEST_OBJECTIVE_GIVE:     return "give";
      case QUEST_OBJECTIVE_ENTER:    return "enter";
      case QUEST_OBJECTIVE_EXAMINE:  return "examine";
      case QUEST_OBJECTIVE_INTERACT: return "interact";
      case QUEST_OBJECTIVE_CUSTOM:   return "custom";
      default:                       return "unknown";
   }
}

void free_quest_definition( QUEST_DEFINITION *quest )
{
   int i;

   if( !quest )
      return;

   STRFREE( quest->name );
   STRFREE( quest->description );
   for( i = 0; i < quest->objective_count && i < MAX_QUEST_OBJECTIVES; ++i )
      STRFREE( quest->objectives[i].description );
   DISPOSE( quest );
}

PLAYER_QUEST_STATE *find_player_quest_state( CHAR_DATA *ch, int quest_id )
{
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata || quest_id <= 0 )
      return NULL;

   for( i = 0; i < MAX_ACTIVE_QUESTS; ++i )
      if( ch->pcdata->quests[i].quest_id == quest_id )
         return &ch->pcdata->quests[i];

   return NULL;
}

PLAYER_QUEST_STATE *free_player_quest_state( CHAR_DATA *ch )
{
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return NULL;

   for( i = 0; i < MAX_ACTIVE_QUESTS; ++i )
      if( ch->pcdata->quests[i].quest_id == 0 )
         return &ch->pcdata->quests[i];

   return NULL;
}

PLAYER_QUEST_HISTORY *find_player_quest_history( CHAR_DATA *ch, int quest_id )
{
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata || quest_id <= 0 )
      return NULL;

   for( i = 0; i < MAX_QUEST_HISTORY; ++i )
      if( ch->pcdata->quest_history[i].quest_id == quest_id )
         return &ch->pcdata->quest_history[i];

   return NULL;
}

PLAYER_QUEST_HISTORY *free_player_quest_history( CHAR_DATA *ch )
{
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return NULL;

   for( i = 0; i < MAX_QUEST_HISTORY; ++i )
      if( ch->pcdata->quest_history[i].quest_id == 0 )
         return &ch->pcdata->quest_history[i];

   return NULL;
}

PLAYER_REPUTATION *find_player_reputation( CHAR_DATA *ch, int reputation_id, bool create )
{
   PLAYER_REPUTATION *free_slot = NULL;
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata || reputation_id <= 0 )
      return NULL;

   for( i = 0; i < MAX_QUEST_REPUTATIONS; ++i )
   {
      if( ch->pcdata->quest_reputation[i].id == reputation_id )
         return &ch->pcdata->quest_reputation[i];
      if( !free_slot && ch->pcdata->quest_reputation[i].id == 0 )
         free_slot = &ch->pcdata->quest_reputation[i];
   }

   if( create && free_slot )
   {
      free_slot->id = reputation_id;
      free_slot->value = 0;
      return free_slot;
   }

   return NULL;
}

int quest_d20_level( const CHAR_DATA *ch )
{
   if( !ch || ch->top_level <= 0 )
      return 0;

   return URANGE( 1, ( ch->top_level + 4 ) / 5, 20 );
}

int current_quest_stage( const PLAYER_QUEST_STATE *state, const QUEST_DEFINITION *quest )
{
   int stage = 0;
   int i;

   if( !state || !quest )
      return 0;

   for( i = 0; i < quest->objective_count; ++i )
   {
      const QUEST_OBJECTIVE_DEFINITION *objective = &quest->objectives[i];
      if( state->progress[i] >= objective->required )
         continue;
      if( stage == 0 || objective->stage < stage )
         stage = objective->stage;
   }

   return stage;
}

bool quest_all_objectives_complete( const PLAYER_QUEST_STATE *state, const QUEST_DEFINITION *quest )
{
   int i;

   if( !state || !quest )
      return false;

   for( i = 0; i < quest->objective_count; ++i )
      if( state->progress[i] < quest->objectives[i].required )
         return false;

   return true;
}

int count_inventory_vnum( OBJ_DATA *obj, int vnum )
{
   int count = 0;

   for( ; obj; obj = obj->next_content )
   {
      if( obj->pIndexData && obj->pIndexData->vnum == vnum )
         count += UMAX( 1, obj->count );
      if( obj->first_content )
         count += count_inventory_vnum( obj->first_content, vnum );
   }

   return count;
}

void show_current_quest_stage( CHAR_DATA *ch, PLAYER_QUEST_STATE *state, QUEST_DEFINITION *quest )
{
   const int stage = current_quest_stage( state, quest );
   int i;

   if( !ch || !state || !quest || stage <= 0 )
      return;

   ch_printf( ch, "&Y[Quest]&w %s - Stage %d\r\n", quest->name, stage );
   for( i = 0; i < quest->objective_count; ++i )
   {
      QUEST_OBJECTIVE_DEFINITION *objective = &quest->objectives[i];
      if( objective->stage != stage )
         continue;
      ch_printf( ch, "  %s (%d/%d)\r\n", objective->description,
                 UMIN( state->progress[i], objective->required ), objective->required );
   }
}

void apply_quest_rewards( CHAR_DATA *ch, QUEST_DEFINITION *quest )
{
   if( !ch || !quest || IS_NPC( ch ) || !ch->pcdata )
      return;

   if( quest->reward_xp_amount > 0 )
   {
      int ability = quest->reward_xp_ability;
      if( ability < 0 )
         ability = ch->main_ability;
      if( ability >= 0 && ability < MAX_ABILITY )
      {
         gain_exp( ch, quest->reward_xp_amount, ability );
         ch_printf( ch, "  +%d %s experience\r\n", quest->reward_xp_amount, ability_name[ability] );
      }
   }

   if( quest->reward_credits > 0 )
   {
      ch->gold += quest->reward_credits;
      ch_printf( ch, "  +%d credits\r\n", quest->reward_credits );
   }

   if( quest->reward_item_vnum > 0 )
   {
      OBJ_INDEX_DATA *index = get_obj_index( quest->reward_item_vnum );
      if( index )
      {
         OBJ_DATA *reward = create_object( index, UMAX( 1, ch->top_level ) );
         obj_to_char( reward, ch );
         ch_printf( ch, "  +%s\r\n", reward->short_descr );
      }
      else
         bug( "Quest %d reward object %d does not exist", quest->id, quest->reward_item_vnum );
   }

   if( quest->reward_reputation_id > 0 && quest->reward_reputation_amount != 0 )
   {
      PLAYER_REPUTATION *rep = find_player_reputation( ch, quest->reward_reputation_id, true );
      if( rep )
      {
         rep->value += quest->reward_reputation_amount;
         ch_printf( ch, "  Reputation %+d (track %d)\r\n",
                    quest->reward_reputation_amount, quest->reward_reputation_id );
      }
   }

   if( quest->reward_skill_sn > 0 && quest->reward_skill_sn < top_sn && quest->reward_skill_value > 0 )
   {
      const int value = URANGE( 1, quest->reward_skill_value, 100 );
      if( ch->pcdata->learned[quest->reward_skill_sn] < value )
      {
         ch->pcdata->learned[quest->reward_skill_sn] = value;
         if( skill_table[quest->reward_skill_sn] && skill_table[quest->reward_skill_sn]->name )
            ch_printf( ch, "  Skill learned: %s (%d)\r\n",
                       skill_table[quest->reward_skill_sn]->name, value );
      }
   }

   if( quest->reward_quest_points > 0 )
   {
      ch->pcdata->quest_curr = URANGE( 0,
         ch->pcdata->quest_curr + quest->reward_quest_points, 32000 );
      ch->pcdata->quest_accum += quest->reward_quest_points;
      ch_printf( ch, "  +%d quest points\r\n", quest->reward_quest_points );
   }
}

bool objective_matches_event( const QUEST_OBJECTIVE_DEFINITION *objective,
                              int objective_type, int target_vnum, int aux_vnum )
{
   if( !objective || objective->type != objective_type )
      return false;
   if( objective->target_vnum > 0 && objective->target_vnum != target_vnum )
      return false;
   if( objective->aux_vnum > 0 && objective->aux_vnum != aux_vnum )
      return false;
   return true;
}

bool validate_quest_definition( QUEST_DEFINITION *quest )
{
   int i;

   if( !quest || quest->id <= 0 || !quest->name || !quest->name[0] )
      return false;
   if( get_quest_definition( quest->id ) )
      return false;
   if( quest->min_level < 0 )
      quest->min_level = 0;
   if( quest->max_level <= 0 )
      quest->max_level = 20;
   if( quest->max_level > 20 || quest->max_level < quest->min_level )
      return false;
   if( quest->objective_count <= 0 || quest->objective_count > MAX_QUEST_OBJECTIVES )
      return false;

   for( i = 0; i < quest->objective_count; ++i )
   {
      QUEST_OBJECTIVE_DEFINITION *objective = &quest->objectives[i];
      if( objective->type <= QUEST_OBJECTIVE_NONE || objective->type > QUEST_OBJECTIVE_CUSTOM )
         return false;
      if( objective->stage <= 0 || objective->required <= 0 )
         return false;
      if( !objective->description || !objective->description[0] )
         return false;
   }

   return true;
}

void show_quest_rewards( CHAR_DATA *ch, QUEST_DEFINITION *quest )
{
   bool any = false;

   if( quest->reward_xp_amount > 0 )
   {
      ch_printf( ch, "  XP: %d\r\n", quest->reward_xp_amount );
      any = true;
   }
   if( quest->reward_credits > 0 )
   {
      ch_printf( ch, "  Credits: %d\r\n", quest->reward_credits );
      any = true;
   }
   if( quest->reward_item_vnum > 0 )
   {
      OBJ_INDEX_DATA *index = get_obj_index( quest->reward_item_vnum );
      ch_printf( ch, "  Item: %s\r\n", index ? index->short_descr : "configured item" );
      any = true;
   }
   if( quest->reward_reputation_id > 0 && quest->reward_reputation_amount != 0 )
   {
      ch_printf( ch, "  Reputation: %+d (track %d)\r\n",
                 quest->reward_reputation_amount, quest->reward_reputation_id );
      any = true;
   }
   if( quest->reward_quest_points > 0 )
   {
      ch_printf( ch, "  Quest points: %d\r\n", quest->reward_quest_points );
      any = true;
   }
   if( !any )
      send_to_char( "  None.\r\n", ch );
}
}

QUEST_DEFINITION *get_quest_definition( int id )
{
   QUEST_DEFINITION *quest;

   for( quest = first_quest; quest; quest = quest->next )
      if( quest->id == id )
         return quest;
   return NULL;
}

QUEST_DEFINITION *find_quest_definition( const char *name_or_id )
{
   QUEST_DEFINITION *quest;

   if( !name_or_id || !name_or_id[0] )
      return NULL;

   if( is_number( name_or_id ) )
      return get_quest_definition( atoi( name_or_id ) );

   for( quest = first_quest; quest; quest = quest->next )
      if( quest->name && !str_cmp( name_or_id, quest->name ) )
         return quest;

   for( quest = first_quest; quest; quest = quest->next )
      if( quest->name && !str_prefix( name_or_id, quest->name ) )
         return quest;

   return NULL;
}

void load_quests( void )
{
   FILE *fp;
   const char *word;
   int count = 0;
   char logline[MAX_STRING_LENGTH];

   first_quest = NULL;
   last_quest = NULL;

   if( !( fp = fopen( QUEST_FILE, "r" ) ) )
   {
      bug( "load_quests: unable to open %s", QUEST_FILE );
      return;
   }

   for( ;; )
   {
      word = fread_word( fp );
      if( !word || !word[0] )
         break;
      if( !str_cmp( word, "#END" ) )
         break;
      if( str_cmp( word, "#QUEST" ) )
      {
         bug( "load_quests: expected #QUEST, found %s", word );
         fread_to_eol( fp );
         continue;
      }

      QUEST_DEFINITION *quest;
      CREATE( quest, QUEST_DEFINITION, 1 );
      quest->max_level = 20;
      quest->reward_xp_ability = -1;

      for( ;; )
      {
         word = fread_word( fp );
         if( !str_cmp( word, "End" ) )
            break;

         if( !str_cmp( word, "Id" ) )
            quest->id = fread_number( fp );
         else if( !str_cmp( word, "Name" ) )
            quest->name = fread_string( fp );
         else if( !str_cmp( word, "Description" ) )
            quest->description = fread_string( fp );
         else if( !str_cmp( word, "MinLevel" ) )
            quest->min_level = fread_number( fp );
         else if( !str_cmp( word, "MaxLevel" ) )
            quest->max_level = fread_number( fp );
         else if( !str_cmp( word, "Prerequisite" ) )
         {
            quest->prerequisite_id = fread_number( fp );
            quest->prerequisite_outcome = fread_number( fp );
         }
         else if( !str_cmp( word, "Repeatable" ) )
            quest->repeatable = fread_number( fp ) != 0;
         else if( !str_cmp( word, "Cooldown" ) )
            quest->cooldown_seconds = UMAX( 0, fread_number( fp ) );
         else if( !str_cmp( word, "RewardXP" ) )
         {
            quest->reward_xp_ability = fread_number( fp );
            quest->reward_xp_amount = UMAX( 0, fread_number( fp ) );
         }
         else if( !str_cmp( word, "RewardCredits" ) )
            quest->reward_credits = UMAX( 0, fread_number( fp ) );
         else if( !str_cmp( word, "RewardItem" ) )
            quest->reward_item_vnum = UMAX( 0, fread_number( fp ) );
         else if( !str_cmp( word, "RewardReputation" ) )
         {
            quest->reward_reputation_id = fread_number( fp );
            quest->reward_reputation_amount = fread_number( fp );
         }
         else if( !str_cmp( word, "RewardSkill" ) )
         {
            quest->reward_skill_sn = fread_number( fp );
            quest->reward_skill_value = fread_number( fp );
         }
         else if( !str_cmp( word, "RewardQuestPoints" ) )
            quest->reward_quest_points = UMAX( 0, fread_number( fp ) );
         else if( !str_cmp( word, "Objective" ) )
         {
            char type_name[MAX_INPUT_LENGTH];
            QUEST_OBJECTIVE_DEFINITION *objective;

            if( quest->objective_count >= MAX_QUEST_OBJECTIVES )
            {
               bug( "Quest %d exceeds MAX_QUEST_OBJECTIVES", quest->id );
               fread_to_eol( fp );
               continue;
            }

            strlcpy( type_name, fread_word( fp ), sizeof( type_name ) );
            objective = &quest->objectives[quest->objective_count++];
            objective->type = quest_objective_type_from_name( type_name );
            objective->stage = fread_number( fp );
            objective->target_vnum = fread_number( fp );
            objective->aux_vnum = fread_number( fp );
            objective->required = fread_number( fp );
            objective->description = fread_string( fp );
         }
         else
         {
            bug( "Quest %d: unknown field %s", quest->id, word );
            fread_to_eol( fp );
         }
      }

      if( !quest->description )
         quest->description = STRALLOC( "" );

      if( !validate_quest_definition( quest ) )
      {
         bug( "load_quests: invalid quest definition %d", quest->id );
         free_quest_definition( quest );
         continue;
      }

      LINK( quest, first_quest, last_quest, next, prev );
      ++count;
   }

   FCLOSE( fp );
   snprintf( logline, sizeof( logline ), "Loaded %d quest definitions.", count );
   log_string( logline );
}

bool quest_is_active( CHAR_DATA *ch, int quest_id )
{
   PLAYER_QUEST_STATE *state = find_player_quest_state( ch, quest_id );
   return state && state->status == QUEST_STATE_ACTIVE;
}

bool quest_is_failed( CHAR_DATA *ch, int quest_id )
{
   PLAYER_QUEST_STATE *state = find_player_quest_state( ch, quest_id );
   return state && state->status == QUEST_STATE_FAILED;
}

bool quest_is_complete( CHAR_DATA *ch, int quest_id )
{
   PLAYER_QUEST_HISTORY *history = find_player_quest_history( ch, quest_id );
   return history && history->completion_count > 0;
}

int quest_outcome( CHAR_DATA *ch, int quest_id )
{
   PLAYER_QUEST_HISTORY *history = find_player_quest_history( ch, quest_id );
   return history ? history->outcome : 0;
}

int quest_stage( CHAR_DATA *ch, int quest_id )
{
   PLAYER_QUEST_STATE *state = find_player_quest_state( ch, quest_id );
   QUEST_DEFINITION *quest = get_quest_definition( quest_id );
   return current_quest_stage( state, quest );
}

int quest_get_reputation( CHAR_DATA *ch, int reputation_id )
{
   PLAYER_REPUTATION *rep = find_player_reputation( ch, reputation_id, false );
   return rep ? rep->value : 0;
}

bool quest_is_available( CHAR_DATA *ch, int quest_id )
{
   QUEST_DEFINITION *quest = get_quest_definition( quest_id );
   PLAYER_QUEST_HISTORY *history;
   int heroic_level;

   if( !ch || IS_NPC( ch ) || !ch->pcdata || !quest )
      return false;
   if( find_player_quest_state( ch, quest_id ) )
      return false;

   heroic_level = quest_d20_level( ch );
   if( heroic_level < quest->min_level || heroic_level > quest->max_level )
      return false;

   if( quest->prerequisite_id > 0 )
   {
      PLAYER_QUEST_HISTORY *required = find_player_quest_history( ch, quest->prerequisite_id );
      if( !required || required->completion_count <= 0 )
         return false;
      if( quest->prerequisite_outcome != 0 && required->outcome != quest->prerequisite_outcome )
         return false;
   }

   history = find_player_quest_history( ch, quest_id );
   if( history && history->completion_count > 0 )
   {
      if( !quest->repeatable )
         return false;
      if( history->cooldown_until > current_time )
         return false;
   }

   return true;
}

bool quest_start( CHAR_DATA *ch, int quest_id )
{
   QUEST_DEFINITION *quest = get_quest_definition( quest_id );
   PLAYER_QUEST_STATE *state;
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata || !quest )
      return false;

   state = find_player_quest_state( ch, quest_id );
   if( state )
   {
      ch_printf( ch, "&Y[Quest]&w %s is already %s.\r\n", quest->name,
                 state->status == QUEST_STATE_FAILED ? "failed" : "active" );
      return false;
   }

   if( !quest_is_available( ch, quest_id ) )
   {
      PLAYER_QUEST_HISTORY *history = find_player_quest_history( ch, quest_id );
      if( history && history->completion_count > 0 && !quest->repeatable )
         ch_printf( ch, "&Y[Quest]&w %s has already been completed.\r\n", quest->name );
      else if( history && history->cooldown_until > current_time )
         ch_printf( ch, "&Y[Quest]&w %s is on cooldown for %ld more seconds.\r\n",
                    quest->name, (long)( history->cooldown_until - current_time ) );
      else
         ch_printf( ch, "&Y[Quest]&w %s is not currently available.\r\n", quest->name );
      return false;
   }

   state = free_player_quest_state( ch );
   if( !state )
   {
      send_to_char( "&RYour quest journal is full. Abandon a quest first.\r\n&w", ch );
      return false;
   }

   memset( state, 0, sizeof( *state ) );
   state->quest_id = quest_id;
   state->status = QUEST_STATE_ACTIVE;
   state->objective_count = quest->objective_count;
   state->started_at = current_time;
   state->updated_at = current_time;

   ch_printf( ch, "&G[Quest Started]&w %s\r\n", quest->name );
   if( quest->description && quest->description[0] )
      ch_printf( ch, "%s\r\n", quest->description );

   for( i = 0; i < quest->objective_count; ++i )
   {
      QUEST_OBJECTIVE_DEFINITION *objective = &quest->objectives[i];
      if( objective->type == QUEST_OBJECTIVE_GET && objective->stage == current_quest_stage( state, quest ) )
      {
         const int carried = count_inventory_vnum( ch->first_carrying, objective->target_vnum );
         state->progress[i] = UMIN( objective->required, carried );
      }
   }

   if( quest_all_objectives_complete( state, quest ) )
      return quest_complete( ch, quest_id, 0 );

   show_current_quest_stage( ch, state, quest );
   return true;
}

bool quest_advance_objective( CHAR_DATA *ch, int quest_id, int objective_index, int amount )
{
   PLAYER_QUEST_STATE *state = find_player_quest_state( ch, quest_id );
   QUEST_DEFINITION *quest = get_quest_definition( quest_id );
   QUEST_OBJECTIVE_DEFINITION *objective;
   const int index = objective_index - 1;
   int old_stage;

   if( !state || state->status != QUEST_STATE_ACTIVE || !quest )
      return false;
   if( index < 0 || index >= quest->objective_count || amount <= 0 )
      return false;

   objective = &quest->objectives[index];
   old_stage = current_quest_stage( state, quest );
   if( objective->stage != old_stage || state->progress[index] >= objective->required )
      return false;

   state->progress[index] = UMIN( objective->required, state->progress[index] + amount );
   state->updated_at = current_time;
   ch_printf( ch, "&Y[Quest]&w %s: %s (%d/%d)\r\n", quest->name,
              objective->description, state->progress[index], objective->required );

   if( quest_all_objectives_complete( state, quest ) )
      return quest_complete( ch, quest_id, 0 );

   if( current_quest_stage( state, quest ) != old_stage )
      show_current_quest_stage( ch, state, quest );

   return true;
}

void quest_event( CHAR_DATA *ch, int objective_type, int target_vnum, int aux_vnum )
{
   int slot;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   for( slot = 0; slot < MAX_ACTIVE_QUESTS; ++slot )
   {
      PLAYER_QUEST_STATE *state = &ch->pcdata->quests[slot];
      QUEST_DEFINITION *quest;
      int stage;
      int i;

      if( state->quest_id <= 0 || state->status != QUEST_STATE_ACTIVE )
         continue;
      quest = get_quest_definition( state->quest_id );
      if( !quest )
         continue;
      stage = current_quest_stage( state, quest );
      if( stage <= 0 )
         continue;

      for( i = 0; i < quest->objective_count; ++i )
      {
         QUEST_OBJECTIVE_DEFINITION *objective = &quest->objectives[i];
         if( objective->stage != stage || state->progress[i] >= objective->required )
            continue;
         if( objective_matches_event( objective, objective_type, target_vnum, aux_vnum ) )
            quest_advance_objective( ch, quest->id, i + 1, 1 );
      }
   }
}

bool quest_complete( CHAR_DATA *ch, int quest_id, int outcome )
{
   PLAYER_QUEST_STATE *state = find_player_quest_state( ch, quest_id );
   PLAYER_QUEST_HISTORY *history;
   QUEST_DEFINITION *quest = get_quest_definition( quest_id );

   if( !state || state->status != QUEST_STATE_ACTIVE || !quest )
      return false;

   history = find_player_quest_history( ch, quest_id );
   if( !history )
      history = free_player_quest_history( ch );
   if( !history )
   {
      send_to_char( "&RYour completed quest history is full. Please report this to staff.\r\n&w", ch );
      return false;
   }

   if( history->quest_id == 0 )
      memset( history, 0, sizeof( *history ) );
   history->quest_id = quest_id;
   history->completion_count++;
   history->outcome = outcome;
   history->completed_at = current_time;
   history->cooldown_until = quest->repeatable && quest->cooldown_seconds > 0
      ? current_time + quest->cooldown_seconds : 0;

   memset( state, 0, sizeof( *state ) );

   ch_printf( ch, "&G[Quest Complete]&w %s\r\n", quest->name );
   apply_quest_rewards( ch, quest );
   return true;
}

bool quest_fail( CHAR_DATA *ch, int quest_id )
{
   PLAYER_QUEST_STATE *state = find_player_quest_state( ch, quest_id );
   QUEST_DEFINITION *quest = get_quest_definition( quest_id );

   if( !state || state->status != QUEST_STATE_ACTIVE || !quest )
      return false;

   state->status = QUEST_STATE_FAILED;
   state->updated_at = current_time;
   ch_printf( ch, "&R[Quest Failed]&w %s\r\n", quest->name );
   send_to_char( "Use ABANDON <quest> to clear it from your journal.\r\n", ch );
   return true;
}

bool quest_abandon( CHAR_DATA *ch, int quest_id )
{
   PLAYER_QUEST_STATE *state = find_player_quest_state( ch, quest_id );
   QUEST_DEFINITION *quest = get_quest_definition( quest_id );

   if( !state || !quest )
      return false;

   memset( state, 0, sizeof( *state ) );
   ch_printf( ch, "&Y[Quest Abandoned]&w %s\r\n", quest->name );
   return true;
}

void quest_save_player( FILE *fp, CHAR_DATA *ch )
{
   int i, j;

   if( !fp || !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   for( i = 0; i < MAX_ACTIVE_QUESTS; ++i )
   {
      PLAYER_QUEST_STATE *state = &ch->pcdata->quests[i];
      if( state->quest_id <= 0 )
         continue;
      fprintf( fp, "QuestActive  %d %d %d %ld %ld",
               state->quest_id, state->status, state->outcome,
               (long)state->started_at, (long)state->updated_at );
      for( j = 0; j < MAX_QUEST_OBJECTIVES; ++j )
         fprintf( fp, " %d", state->progress[j] );
      fprintf( fp, "\n" );
   }

   for( i = 0; i < MAX_QUEST_HISTORY; ++i )
   {
      PLAYER_QUEST_HISTORY *history = &ch->pcdata->quest_history[i];
      if( history->quest_id <= 0 )
         continue;
      fprintf( fp, "QuestDone    %d %d %d %ld %ld\n",
               history->quest_id, history->completion_count, history->outcome,
               (long)history->completed_at, (long)history->cooldown_until );
   }

   for( i = 0; i < MAX_QUEST_REPUTATIONS; ++i )
   {
      PLAYER_REPUTATION *rep = &ch->pcdata->quest_reputation[i];
      if( rep->id > 0 )
         fprintf( fp, "QuestRep     %d %d\n", rep->id, rep->value );
   }
}

bool quest_load_player_field( CHAR_DATA *ch, const char *word, FILE *fp )
{
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata || !word || !fp )
      return false;

   if( !str_cmp( word, "QuestActive" ) )
   {
      int id = fread_number( fp );
      int status = fread_number( fp );
      int outcome = fread_number( fp );
      time_t started = (time_t)fread_number( fp );
      time_t updated = (time_t)fread_number( fp );
      int progress[MAX_QUEST_OBJECTIVES];
      PLAYER_QUEST_STATE *state = find_player_quest_state( ch, id );
      QUEST_DEFINITION *quest = get_quest_definition( id );

      for( i = 0; i < MAX_QUEST_OBJECTIVES; ++i )
         progress[i] = fread_number( fp );

      if( !quest )
         return true;
      if( !state )
         state = free_player_quest_state( ch );
      if( !state )
         return true;

      memset( state, 0, sizeof( *state ) );
      state->quest_id = id;
      state->status = status == QUEST_STATE_FAILED ? QUEST_STATE_FAILED : QUEST_STATE_ACTIVE;
      state->outcome = outcome;
      state->objective_count = quest->objective_count;
      state->started_at = started;
      state->updated_at = updated;
      for( i = 0; i < MAX_QUEST_OBJECTIVES; ++i )
         state->progress[i] = UMAX( 0, progress[i] );
      return true;
   }

   if( !str_cmp( word, "QuestDone" ) )
   {
      int id = fread_number( fp );
      int completions = fread_number( fp );
      int outcome = fread_number( fp );
      time_t completed = (time_t)fread_number( fp );
      time_t cooldown = (time_t)fread_number( fp );
      PLAYER_QUEST_HISTORY *history = find_player_quest_history( ch, id );

      if( !history )
         history = free_player_quest_history( ch );
      if( history )
      {
         memset( history, 0, sizeof( *history ) );
         history->quest_id = id;
         history->completion_count = UMAX( 0, completions );
         history->outcome = outcome;
         history->completed_at = completed;
         history->cooldown_until = cooldown;
      }
      return true;
   }

   if( !str_cmp( word, "QuestRep" ) )
   {
      int id = fread_number( fp );
      int value = fread_number( fp );
      PLAYER_REPUTATION *rep = find_player_reputation( ch, id, true );
      if( rep )
         rep->value = value;
      return true;
   }

   return false;
}

extern "C" void do_quests( CHAR_DATA *ch, const char *argument )
{
   QUEST_DEFINITION *quest;
   int active = 0, failed = 0, available = 0, completed = 0;
   int i;

   (void)argument;
   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   send_to_char( "&WQuest Journal&w\r\n", ch );
   for( i = 0; i < MAX_ACTIVE_QUESTS; ++i )
   {
      PLAYER_QUEST_STATE *state = &ch->pcdata->quests[i];
      if( state->quest_id <= 0 )
         continue;
      quest = get_quest_definition( state->quest_id );
      if( !quest )
         continue;
      if( state->status == QUEST_STATE_FAILED )
      {
         if( failed++ == 0 ) send_to_char( "&RFailed:&w\r\n", ch );
         ch_printf( ch, "  [%d] %s\r\n", quest->id, quest->name );
      }
      else
      {
         if( active++ == 0 ) send_to_char( "&GActive:&w\r\n", ch );
         ch_printf( ch, "  [%d] %s - stage %d\r\n", quest->id, quest->name,
                    current_quest_stage( state, quest ) );
      }
   }

   for( quest = first_quest; quest; quest = quest->next )
      if( quest_is_available( ch, quest->id ) )
      {
         if( available++ == 0 ) send_to_char( "&YAvailable:&w\r\n", ch );
         ch_printf( ch, "  [%d] %s\r\n", quest->id, quest->name );
      }

   for( i = 0; i < MAX_QUEST_HISTORY; ++i )
      if( ch->pcdata->quest_history[i].quest_id > 0
          && ch->pcdata->quest_history[i].completion_count > 0 )
         ++completed;

   if( active == 0 && failed == 0 && available == 0 )
      send_to_char( "  No active or currently available quests.\r\n", ch );
   ch_printf( ch, "Completed quest records: %d\r\n", completed );
   send_to_char( "Use QUEST <name or id> for details.\r\n", ch );
}

extern "C" void do_quest( CHAR_DATA *ch, const char *argument )
{
   QUEST_DEFINITION *quest;
   PLAYER_QUEST_STATE *state;
   PLAYER_QUEST_HISTORY *history;
   int stage = 0;
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   if( !argument || !argument[0] )
   {
      do_quests( ch, "" );
      return;
   }

   if( !str_cmp( argument, "points" ) )
   {
      ch_printf( ch, "Quest points: %d current / %d lifetime.\r\n",
                 ch->pcdata->quest_curr, ch->pcdata->quest_accum );
      return;
   }

   quest = find_quest_definition( argument );
   if( !quest )
   {
      send_to_char( "No quest by that name or id exists.\r\n", ch );
      return;
   }

   state = find_player_quest_state( ch, quest->id );
   history = find_player_quest_history( ch, quest->id );
   if( state )
      stage = current_quest_stage( state, quest );

   ch_printf( ch, "&W[%d] %s&w\r\n", quest->id, quest->name );
   ch_printf( ch, "%s\r\n", quest->description ? quest->description : "" );
   if( state && state->status == QUEST_STATE_FAILED )
      send_to_char( "Status: FAILED\r\n", ch );
   else if( state )
      ch_printf( ch, "Status: ACTIVE - Stage %d\r\n", stage );
   else if( history && history->completion_count > 0 )
   {
      ch_printf( ch, "Status: COMPLETED (%d time%s)",
                 history->completion_count, history->completion_count == 1 ? "" : "s" );
      if( history->outcome != 0 )
         ch_printf( ch, ", outcome %d", history->outcome );
      send_to_char( "\r\n", ch );
   }
   else if( quest_is_available( ch, quest->id ) )
      send_to_char( "Status: AVAILABLE\r\n", ch );
   else
      send_to_char( "Status: LOCKED\r\n", ch );

   send_to_char( "Objectives:\r\n", ch );
   for( i = 0; i < quest->objective_count; ++i )
   {
      QUEST_OBJECTIVE_DEFINITION *objective = &quest->objectives[i];
      const int progress = state ? state->progress[i] : 0;
      const char mark = state && progress >= objective->required ? 'X'
                        : state && objective->stage == stage ? '>' : ' ';
      ch_printf( ch, " [%c] %s (%d/%d)\r\n", mark, objective->description,
                 UMIN( progress, objective->required ), objective->required );
   }

   send_to_char( "Rewards:\r\n", ch );
   show_quest_rewards( ch, quest );
   ch_printf( ch, "Repeatable: %s", quest->repeatable ? "yes" : "no" );
   if( quest->repeatable && quest->cooldown_seconds > 0 )
      ch_printf( ch, " (%d second cooldown)", quest->cooldown_seconds );
   send_to_char( "\r\n", ch );
}

extern "C" void do_abandon( CHAR_DATA *ch, const char *argument )
{
   QUEST_DEFINITION *quest;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;
   if( !argument || !argument[0] )
   {
      send_to_char( "Usage: ABANDON <quest name or id>\r\n", ch );
      return;
   }

   quest = find_quest_definition( argument );
   if( !quest || !find_player_quest_state( ch, quest->id ) )
   {
      send_to_char( "That quest is not in your active journal.\r\n", ch );
      return;
   }
   quest_abandon( ch, quest->id );
}

extern "C" void do_mpqueststart( CHAR_DATA *ch, const char *argument )
{
   char who[MAX_INPUT_LENGTH], qid[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   if( !ch || !IS_NPC( ch ) )
   {
      if( ch ) send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, who );
   one_argument( argument, qid );
   if( !who[0] || !is_number( qid ) )
   {
      progbug( "Mpqueststart: syntax mpqueststart <player> <quest-id>", ch );
      return;
   }
   victim = get_char_room_mp( ch, who );
   if( !victim || IS_NPC( victim ) )
   {
      progbug( "Mpqueststart: player not found", ch );
      return;
   }
   quest_start( victim, atoi( qid ) );
}

extern "C" void do_mpquestadvance( CHAR_DATA *ch, const char *argument )
{
   char who[MAX_INPUT_LENGTH], qid[MAX_INPUT_LENGTH], objective[MAX_INPUT_LENGTH], amount[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int value = 1;

   if( !ch || !IS_NPC( ch ) )
   {
      if( ch ) send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, who );
   argument = one_argument( argument, qid );
   argument = one_argument( argument, objective );
   one_argument( argument, amount );
   if( !who[0] || !is_number( qid ) || !is_number( objective ) )
   {
      progbug( "Mpquestadvance: syntax mpquestadvance <player> <quest-id> <objective> [amount]", ch );
      return;
   }
   if( amount[0] && is_number( amount ) )
      value = UMAX( 1, atoi( amount ) );
   victim = get_char_room_mp( ch, who );
   if( !victim || IS_NPC( victim ) )
   {
      progbug( "Mpquestadvance: player not found", ch );
      return;
   }
   quest_advance_objective( victim, atoi( qid ), atoi( objective ), value );
}

extern "C" void do_mpquestcomplete( CHAR_DATA *ch, const char *argument )
{
   char who[MAX_INPUT_LENGTH], qid[MAX_INPUT_LENGTH], outcome[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;
   int result = 0;

   if( !ch || !IS_NPC( ch ) )
   {
      if( ch ) send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, who );
   argument = one_argument( argument, qid );
   one_argument( argument, outcome );
   if( !who[0] || !is_number( qid ) )
   {
      progbug( "Mpquestcomplete: syntax mpquestcomplete <player> <quest-id> [outcome]", ch );
      return;
   }
   if( outcome[0] && is_number( outcome ) )
      result = atoi( outcome );
   victim = get_char_room_mp( ch, who );
   if( !victim || IS_NPC( victim ) )
   {
      progbug( "Mpquestcomplete: player not found", ch );
      return;
   }
   quest_complete( victim, atoi( qid ), result );
}

extern "C" void do_mpquestfail( CHAR_DATA *ch, const char *argument )
{
   char who[MAX_INPUT_LENGTH], qid[MAX_INPUT_LENGTH];
   CHAR_DATA *victim;

   if( !ch || !IS_NPC( ch ) )
   {
      if( ch ) send_to_char( "Huh?\r\n", ch );
      return;
   }

   argument = one_argument( argument, who );
   one_argument( argument, qid );
   if( !who[0] || !is_number( qid ) )
   {
      progbug( "Mpquestfail: syntax mpquestfail <player> <quest-id>", ch );
      return;
   }
   victim = get_char_room_mp( ch, who );
   if( !victim || IS_NPC( victim ) )
   {
      progbug( "Mpquestfail: player not found", ch );
      return;
   }
   quest_fail( victim, atoi( qid ) );
}
