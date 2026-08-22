/***************************************************************************
 * Zero Point - minimal persistent dialogue and memory layer
 *
 * Dialogue owns conversation choices and long-term player memories. Quest
 * state remains authoritative in quest.c; MUDProgs remain responsible for
 * physical world reactions. Dialogue choices can deliberately fire an
 * existing speech trigger when a world-scripted response is required.
 ***************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mud.h"

#define DIALOGUE_FILE "../system/dialogues.dat"

typedef enum
{
   DIALOGUE_QUEST_ANY = 0,
   DIALOGUE_QUEST_AVAILABLE,
   DIALOGUE_QUEST_ACTIVE,
   DIALOGUE_QUEST_COMPLETE,
   DIALOGUE_QUEST_FAILED
} dialogue_quest_requirement;

typedef struct dialogue_choice_definition DIALOGUE_CHOICE_DEFINITION;
typedef struct dialogue_node_definition DIALOGUE_NODE_DEFINITION;

struct dialogue_choice_definition
{
   const char *text;
   int next_node_id;
   int quest_start_id;
   const char *memory_key;
   const char *memory_value;
   const char *trigger_speech;
   bool close_dialogue;
};

struct dialogue_node_definition
{
   DIALOGUE_NODE_DEFINITION *next;
   DIALOGUE_NODE_DEFINITION *prev;
   int id;
   int mob_vnum;
   bool root;
   const char *text;
   int required_quest_id;
   short required_quest_state;
   int required_quest_stage;
   const char *required_memory_key;
   const char *required_memory_value;
   short choice_count;
   DIALOGUE_CHOICE_DEFINITION choices[MAX_DIALOGUE_CHOICES];
};

static DIALOGUE_NODE_DEFINITION *first_dialogue = NULL;
static DIALOGUE_NODE_DEFINITION *last_dialogue = NULL;

/* Existing layered entry points retained behind Makefile symbols. */
void quest_engine_load_quests( void );
void quest_engine_save_player( FILE *fp, CHAR_DATA *ch );
bool quest_engine_load_player_field( CHAR_DATA *ch, const char *word, FILE *fp );
void geography_ui_load_commands( void );

DECLARE_DO_FUN( do_talk );
DECLARE_DO_FUN( do_choose );

static bool dialogue_memory_token_valid( const char *token, size_t maximum )
{
   const unsigned char *scan;

   if( !token || !token[0] || strlen( token ) >= maximum )
      return false;

   for( scan = ( const unsigned char * )token; *scan; ++scan )
      if( !isalnum( *scan ) && *scan != '.' && *scan != '_' && *scan != '-' && *scan != ':' )
         return false;

   return true;
}

const char *dialogue_memory_get( CHAR_DATA *ch, const char *key )
{
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata || !key || !key[0] )
      return "";

   for( i = 0; i < MAX_PLAYER_MEMORIES; ++i )
      if( ch->pcdata->memories[i].key[0]
          && !str_cmp( ch->pcdata->memories[i].key, key ) )
         return ch->pcdata->memories[i].value;

   return "";
}

bool dialogue_memory_matches( CHAR_DATA *ch, const char *key, const char *value )
{
   const char *remembered;

   if( !key || !key[0] )
      return true;
   if( !value )
      value = "";

   remembered = dialogue_memory_get( ch, key );
   return remembered[0] && !str_cmp( remembered, value );
}

bool dialogue_memory_set( CHAR_DATA *ch, const char *key, const char *value )
{
   PLAYER_MEMORY *free_slot = NULL;
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata
       || !dialogue_memory_token_valid( key, MAX_MEMORY_KEY_LENGTH )
       || !dialogue_memory_token_valid( value, MAX_MEMORY_VALUE_LENGTH ) )
      return false;

   for( i = 0; i < MAX_PLAYER_MEMORIES; ++i )
   {
      PLAYER_MEMORY *memory = &ch->pcdata->memories[i];

      if( memory->key[0] && !str_cmp( memory->key, key ) )
      {
         strlcpy( memory->value, value, sizeof( memory->value ) );
         return true;
      }
      if( !free_slot && !memory->key[0] )
         free_slot = memory;
   }

   if( !free_slot )
      return false;

   strlcpy( free_slot->key, key, sizeof( free_slot->key ) );
   strlcpy( free_slot->value, value, sizeof( free_slot->value ) );
   return true;
}

bool dialogue_memory_clear( CHAR_DATA *ch, const char *key )
{
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata || !key || !key[0] )
      return false;

   for( i = 0; i < MAX_PLAYER_MEMORIES; ++i )
      if( ch->pcdata->memories[i].key[0]
          && !str_cmp( ch->pcdata->memories[i].key, key ) )
      {
         memset( &ch->pcdata->memories[i], 0, sizeof( ch->pcdata->memories[i] ) );
         return true;
      }

   return false;
}

static short dialogue_quest_requirement_from_name( const char *name )
{
   if( !name || !name[0] || !str_cmp( name, "any" ) )
      return DIALOGUE_QUEST_ANY;
   if( !str_cmp( name, "available" ) )
      return DIALOGUE_QUEST_AVAILABLE;
   if( !str_cmp( name, "active" ) )
      return DIALOGUE_QUEST_ACTIVE;
   if( !str_cmp( name, "complete" ) || !str_cmp( name, "completed" ) )
      return DIALOGUE_QUEST_COMPLETE;
   if( !str_cmp( name, "failed" ) )
      return DIALOGUE_QUEST_FAILED;
   return -1;
}

static DIALOGUE_NODE_DEFINITION *get_dialogue_node( int id )
{
   DIALOGUE_NODE_DEFINITION *node;

   for( node = first_dialogue; node; node = node->next )
      if( node->id == id )
         return node;
   return NULL;
}

static void free_dialogue_node( DIALOGUE_NODE_DEFINITION *node )
{
   int i;

   if( !node )
      return;

   STRFREE( node->text );
   STRFREE( node->required_memory_key );
   STRFREE( node->required_memory_value );

   for( i = 0; i < node->choice_count && i < MAX_DIALOGUE_CHOICES; ++i )
   {
      STRFREE( node->choices[i].text );
      STRFREE( node->choices[i].memory_key );
      STRFREE( node->choices[i].memory_value );
      STRFREE( node->choices[i].trigger_speech );
   }

   DISPOSE( node );
}

static void free_dialogues( void )
{
   DIALOGUE_NODE_DEFINITION *node = first_dialogue;

   while( node )
   {
      DIALOGUE_NODE_DEFINITION *next = node->next;
      free_dialogue_node( node );
      node = next;
   }

   first_dialogue = NULL;
   last_dialogue = NULL;
}

static bool validate_dialogue_choice( const DIALOGUE_CHOICE_DEFINITION *choice )
{
   if( !choice || !choice->text || !choice->text[0] )
      return false;
   if( choice->memory_key && choice->memory_key[0] )
   {
      if( !choice->memory_value || !choice->memory_value[0] )
         return false;
      if( !dialogue_memory_token_valid( choice->memory_key, MAX_MEMORY_KEY_LENGTH )
          || !dialogue_memory_token_valid( choice->memory_value, MAX_MEMORY_VALUE_LENGTH ) )
         return false;
   }
   return true;
}

static bool validate_dialogue_node( DIALOGUE_NODE_DEFINITION *node )
{
   int i;

   if( !node || node->id <= 0 || node->mob_vnum <= 0 || !node->text || !node->text[0] )
      return false;
   if( get_dialogue_node( node->id ) )
      return false;
   if( node->required_quest_state < DIALOGUE_QUEST_ANY
       || node->required_quest_state > DIALOGUE_QUEST_FAILED )
      return false;
   if( node->required_memory_key && node->required_memory_key[0] )
   {
      if( !node->required_memory_value || !node->required_memory_value[0] )
         return false;
      if( !dialogue_memory_token_valid( node->required_memory_key, MAX_MEMORY_KEY_LENGTH )
          || !dialogue_memory_token_valid( node->required_memory_value, MAX_MEMORY_VALUE_LENGTH ) )
         return false;
   }
   if( node->choice_count <= 0 || node->choice_count > MAX_DIALOGUE_CHOICES )
      return false;

   for( i = 0; i < node->choice_count; ++i )
      if( !validate_dialogue_choice( &node->choices[i] ) )
         return false;

   return true;
}

static void read_dialogue_choice( FILE *fp, DIALOGUE_NODE_DEFINITION *node )
{
   DIALOGUE_CHOICE_DEFINITION *choice;
   const char *word;

   if( node->choice_count >= MAX_DIALOGUE_CHOICES )
   {
      bug( "Dialogue node %d exceeds MAX_DIALOGUE_CHOICES", node->id );
      for( ;; )
      {
         word = fread_word( fp );
         if( !word || !word[0] || !str_cmp( word, "EndChoice" ) )
            return;
         fread_to_eol( fp );
      }
   }

   choice = &node->choices[node->choice_count++];

   for( ;; )
   {
      char key[MAX_MEMORY_KEY_LENGTH];

      word = fread_word( fp );
      if( !word || !word[0] || !str_cmp( word, "EndChoice" ) )
         break;

      if( !str_cmp( word, "Text" ) )
         choice->text = fread_string( fp );
      else if( !str_cmp( word, "Next" ) )
         choice->next_node_id = fread_number( fp );
      else if( !str_cmp( word, "QuestStart" ) )
         choice->quest_start_id = fread_number( fp );
      else if( !str_cmp( word, "Memory" ) )
      {
         strlcpy( key, fread_word( fp ), sizeof( key ) );
         choice->memory_key = STRALLOC( key );
         choice->memory_value = STRALLOC( fread_word( fp ) );
      }
      else if( !str_cmp( word, "TriggerSpeech" ) )
         choice->trigger_speech = fread_string( fp );
      else if( !str_cmp( word, "Close" ) )
         choice->close_dialogue = fread_number( fp ) != 0;
      else
      {
         bug( "Dialogue node %d choice: unknown field %s", node->id, word );
         fread_to_eol( fp );
      }
   }
}

void load_dialogues( void )
{
   FILE *fp;
   const char *word;
   int count = 0;
   char logline[MAX_STRING_LENGTH];

   free_dialogues();

   if( !( fp = fopen( DIALOGUE_FILE, "r" ) ) )
   {
      bug( "load_dialogues: unable to open %s", DIALOGUE_FILE );
      return;
   }

   for( ;; )
   {
      DIALOGUE_NODE_DEFINITION *node;

      word = fread_word( fp );
      if( !word || !word[0] || !str_cmp( word, "#END" ) )
         break;
      if( str_cmp( word, "#NODE" ) )
      {
         bug( "load_dialogues: expected #NODE, found %s", word );
         fread_to_eol( fp );
         continue;
      }

      CREATE( node, DIALOGUE_NODE_DEFINITION, 1 );
      node->required_quest_state = DIALOGUE_QUEST_ANY;

      for( ;; )
      {
         char key[MAX_MEMORY_KEY_LENGTH];

         word = fread_word( fp );
         if( !word || !word[0] || !str_cmp( word, "End" ) )
            break;

         if( !str_cmp( word, "Id" ) )
            node->id = fread_number( fp );
         else if( !str_cmp( word, "Mob" ) )
            node->mob_vnum = fread_number( fp );
         else if( !str_cmp( word, "Root" ) )
            node->root = fread_number( fp ) != 0;
         else if( !str_cmp( word, "Text" ) )
            node->text = fread_string( fp );
         else if( !str_cmp( word, "RequireQuest" ) )
         {
            short requirement;
            node->required_quest_id = fread_number( fp );
            requirement = dialogue_quest_requirement_from_name( fread_word( fp ) );
            node->required_quest_state = requirement;
            node->required_quest_stage = fread_number( fp );
         }
         else if( !str_cmp( word, "RequireMemory" ) )
         {
            strlcpy( key, fread_word( fp ), sizeof( key ) );
            node->required_memory_key = STRALLOC( key );
            node->required_memory_value = STRALLOC( fread_word( fp ) );
         }
         else if( !str_cmp( word, "Choice" ) )
            read_dialogue_choice( fp, node );
         else
         {
            bug( "Dialogue node %d: unknown field %s", node->id, word );
            fread_to_eol( fp );
         }
      }

      if( !validate_dialogue_node( node ) )
      {
         bug( "load_dialogues: invalid dialogue node %d", node->id );
         free_dialogue_node( node );
         continue;
      }

      LINK( node, first_dialogue, last_dialogue, next, prev );
      ++count;
   }

   FCLOSE( fp );

   for( DIALOGUE_NODE_DEFINITION *node = first_dialogue; node; node = node->next )
   {
      int i;

      if( node->required_quest_id > 0 && !get_quest_definition( node->required_quest_id ) )
         bug( "Dialogue node %d requires missing quest %d", node->id, node->required_quest_id );

      for( i = 0; i < node->choice_count; ++i )
      {
         DIALOGUE_CHOICE_DEFINITION *choice = &node->choices[i];
         if( choice->next_node_id > 0 && !get_dialogue_node( choice->next_node_id ) )
            bug( "Dialogue node %d points to missing node %d", node->id, choice->next_node_id );
         if( choice->quest_start_id > 0 && !get_quest_definition( choice->quest_start_id ) )
            bug( "Dialogue node %d starts missing quest %d", node->id, choice->quest_start_id );
      }
   }

   snprintf( logline, sizeof( logline ), "Loaded %d dialogue nodes.", count );
   log_string( logline );
}

static bool dialogue_quest_matches( CHAR_DATA *ch, const DIALOGUE_NODE_DEFINITION *node )
{
   bool matches = true;

   if( !node || node->required_quest_id <= 0 )
      return true;

   switch( node->required_quest_state )
   {
      case DIALOGUE_QUEST_AVAILABLE:
         matches = quest_is_available( ch, node->required_quest_id );
         break;
      case DIALOGUE_QUEST_ACTIVE:
         matches = quest_is_active( ch, node->required_quest_id );
         break;
      case DIALOGUE_QUEST_COMPLETE:
         matches = quest_is_complete( ch, node->required_quest_id );
         break;
      case DIALOGUE_QUEST_FAILED:
         matches = quest_is_failed( ch, node->required_quest_id );
         break;
      case DIALOGUE_QUEST_ANY:
      default:
         matches = true;
         break;
   }

   if( matches && node->required_quest_stage > 0 )
      matches = quest_stage( ch, node->required_quest_id ) == node->required_quest_stage;

   return matches;
}

static bool dialogue_node_matches( CHAR_DATA *ch, const DIALOGUE_NODE_DEFINITION *node )
{
   if( !dialogue_quest_matches( ch, node ) )
      return false;
   if( node->required_memory_key && node->required_memory_key[0]
       && !dialogue_memory_matches( ch, node->required_memory_key, node->required_memory_value ) )
      return false;
   return true;
}

static DIALOGUE_NODE_DEFINITION *find_dialogue_root( CHAR_DATA *ch, int mob_vnum )
{
   DIALOGUE_NODE_DEFINITION *node;

   for( node = first_dialogue; node; node = node->next )
      if( node->root && node->mob_vnum == mob_vnum && dialogue_node_matches( ch, node ) )
         return node;

   return NULL;
}

static CHAR_DATA *find_dialogue_speaker( CHAR_DATA *ch, int mob_vnum )
{
   CHAR_DATA *speaker;

   if( !ch || !ch->in_room || mob_vnum <= 0 )
      return NULL;

   for( speaker = ch->in_room->first_person; speaker; speaker = speaker->next_in_room )
      if( IS_NPC( speaker ) && speaker->pIndexData && speaker->pIndexData->vnum == mob_vnum )
         return speaker;

   return NULL;
}

static void clear_dialogue_session( CHAR_DATA *ch )
{
   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;
   memset( &ch->pcdata->dialogue, 0, sizeof( ch->pcdata->dialogue ) );
}

static void show_dialogue_node( CHAR_DATA *ch, CHAR_DATA *speaker, DIALOGUE_NODE_DEFINITION *node )
{
   const char *speaker_name;
   int i;

   if( !ch || !speaker || !node || IS_NPC( ch ) || !ch->pcdata )
      return;

   speaker_name = speaker->short_descr && speaker->short_descr[0]
      ? speaker->short_descr : speaker->name;

   ch_printf( ch, "\r\n&C%s&w: \"%s\"\r\n", speaker_name ? speaker_name : "Someone", node->text );
   for( i = 0; i < node->choice_count; ++i )
      ch_printf( ch, "  &Y%d)&w %s\r\n", i + 1, node->choices[i].text );

   ch->pcdata->dialogue.node_id = node->id;
   ch->pcdata->dialogue.mob_vnum = node->mob_vnum;
   send_to_char( "&zChoose with &WCHOOSE <number>&z.\r\n&w", ch );
}

extern "C" void do_talk( CHAR_DATA *ch, const char *argument )
{
   CHAR_DATA *speaker;
   DIALOGUE_NODE_DEFINITION *node;
   int mob_vnum;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   if( !argument || !argument[0] )
   {
      send_to_char( "Talk to whom? Usage: TALK <person>\r\n", ch );
      return;
   }

   speaker = get_char_room( ch, argument );
   if( !speaker || !IS_NPC( speaker ) || !speaker->pIndexData )
   {
      send_to_char( "You do not see anyone by that name here to talk to.\r\n", ch );
      clear_dialogue_session( ch );
      return;
   }

   mob_vnum = speaker->pIndexData->vnum;
   quest_event( ch, QUEST_OBJECTIVE_INTERACT, mob_vnum, 0 );
   node = find_dialogue_root( ch, mob_vnum );
   if( !node )
   {
      send_to_char( "They have nothing particular to discuss with you right now.\r\n", ch );
      clear_dialogue_session( ch );
      return;
   }

   show_dialogue_node( ch, speaker, node );
}

extern "C" void do_choose( CHAR_DATA *ch, const char *argument )
{
   DIALOGUE_NODE_DEFINITION *node;
   DIALOGUE_CHOICE_DEFINITION *choice;
   CHAR_DATA *speaker;
   int selection;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   if( ch->pcdata->dialogue.node_id <= 0 || ch->pcdata->dialogue.mob_vnum <= 0 )
   {
      send_to_char( "You are not in a conversation. Use TALK <person> first.\r\n", ch );
      return;
   }

   if( !argument || !argument[0] || !is_number( argument ) )
   {
      send_to_char( "Choose which response? Usage: CHOOSE <number>\r\n", ch );
      return;
   }

   selection = atoi( argument );
   node = get_dialogue_node( ch->pcdata->dialogue.node_id );
   speaker = find_dialogue_speaker( ch, ch->pcdata->dialogue.mob_vnum );

   if( !node || !speaker || node->mob_vnum != ch->pcdata->dialogue.mob_vnum )
   {
      send_to_char( "That conversation is no longer available.\r\n", ch );
      clear_dialogue_session( ch );
      return;
   }

   if( selection < 1 || selection > node->choice_count )
   {
      ch_printf( ch, "Choose a response from 1 to %d.\r\n", node->choice_count );
      return;
   }

   choice = &node->choices[selection - 1];
   ch_printf( ch, "&WYou&w: \"%s\"\r\n", choice->text );

   if( choice->memory_key && choice->memory_key[0]
       && !dialogue_memory_set( ch, choice->memory_key, choice->memory_value ) )
      send_to_char( "&RYour character memory is full; please report this to staff.\r\n&w", ch );

   if( choice->quest_start_id > 0 )
      quest_start( ch, choice->quest_start_id );

   if( choice->trigger_speech && choice->trigger_speech[0] )
      mprog_speech_trigger( choice->trigger_speech, ch );

   if( choice->close_dialogue || choice->next_node_id <= 0 )
   {
      clear_dialogue_session( ch );
      return;
   }

   node = get_dialogue_node( choice->next_node_id );
   if( !node || node->mob_vnum != speaker->pIndexData->vnum || !dialogue_node_matches( ch, node ) )
   {
      clear_dialogue_session( ch );
      return;
   }

   show_dialogue_node( ch, speaker, node );
}

static CMDTYPE *dialogue_find_command( const char *name )
{
   CMDTYPE *command;
   int bucket;

   if( !name || !name[0] )
      return NULL;

   bucket = LOWER( name[0] ) % 126;
   for( command = command_hash[bucket]; command; command = command->next )
      if( command->name && !str_cmp( command->name, name ) )
         return command;

   return NULL;
}

static void dialogue_bind_command( const char *name, DO_FUN *function, const char *function_name )
{
   CMDTYPE *command = dialogue_find_command( name );

   if( command )
   {
      command->do_fun = function;
      command->position = 5;
      command->level = 0;
      return;
   }

   CREATE( command, CMDTYPE, 1 );
   command->name = strdup( name );
   command->do_fun = function;
   command->fun_name = strdup( function_name );
   command->position = 5;
   command->level = 0;
   command->log = 0;
   add_command( command );
}

/* Final command-loader layer: preserve geography UI bindings, then add TALK/CHOOSE. */
void load_commands( void )
{
   geography_ui_load_commands();
   dialogue_bind_command( "talk", do_talk, "do_talk" );
   dialogue_bind_command( "choose", do_choose, "do_choose" );
}

/* Final quest bootstrap layer: quest definitions first, then dependent dialogue data. */
void load_quests( void )
{
   quest_engine_load_quests();
   load_dialogues();
}

/* Keep long-term memories in the same player save stream as modern quest state. */
void quest_save_player( FILE *fp, CHAR_DATA *ch )
{
   int i;

   quest_engine_save_player( fp, ch );

   if( !fp || !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   for( i = 0; i < MAX_PLAYER_MEMORIES; ++i )
      if( ch->pcdata->memories[i].key[0] && ch->pcdata->memories[i].value[0] )
         fprintf( fp, "Memory       %s %s\n",
                  ch->pcdata->memories[i].key,
                  ch->pcdata->memories[i].value );
}

bool quest_load_player_field( CHAR_DATA *ch, const char *word, FILE *fp )
{
   if( ch && !IS_NPC( ch ) && ch->pcdata && word && fp && !str_cmp( word, "Memory" ) )
   {
      char key[MAX_MEMORY_KEY_LENGTH];
      char value[MAX_MEMORY_VALUE_LENGTH];

      strlcpy( key, fread_word( fp ), sizeof( key ) );
      strlcpy( value, fread_word( fp ), sizeof( value ) );
      if( !dialogue_memory_set( ch, key, value ) )
         bug( "Unable to restore player memory %s=%s", key, value );
      return true;
   }

   return quest_engine_load_player_field( ch, word, fp );
}
