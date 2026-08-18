/***************************************************************************
 * Zero Point - Exercise compatibility command
 *
 * Attribute development is now presented through one player-facing command:
 *   train <attribute> [equipment]
 *
 * Keep EXERCISE as a compatibility alias for existing player habits/scripts.
 * It infers the attribute from a configured training object and forwards the
 * request into the authoritative TRAIN implementation.
 ***************************************************************************/

#include <string.h>
#include "mud.h"

#define TRAINING_EQUIPMENT_MARKER 1414680135

extern "C" void do_exercise( CHAR_DATA *ch, const char *argument );

static const char *exercise_ability_name( int ability )
{
   switch( ability )
   {
      case ABILITY_SCORE_STR: return "strength";
      case ABILITY_SCORE_DEX: return "dexterity";
      case ABILITY_SCORE_CON: return "constitution";
      case ABILITY_SCORE_INT: return "intelligence";
      case ABILITY_SCORE_WIS: return "wisdom";
      case ABILITY_SCORE_CHA: return "charisma";
      default: return NULL;
   }
}

void do_exercise( CHAR_DATA *ch, const char *argument )
{
   char equipment[MAX_INPUT_LENGTH];
   char train_argument[MAX_INPUT_LENGTH * 2];
   OBJ_DATA *obj;
   const char *ability_name;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   strlcpy(
      equipment,
      argument ? argument : "",
      sizeof( equipment ) );

   if( equipment[0] == '\0' )
   {
      send_to_char(
         "Exercise is now part of attribute training.\r\n"
         "Use: train <attribute> [equipment]\r\n"
         "Example: train strength weights\r\n",
         ch );
      return;
   }

   obj = get_obj_here( ch, equipment );
   if( !obj )
   {
      send_to_char(
         "You don't see that training equipment here.\r\n",
         ch );
      return;
   }

   if( obj->value[5] != TRAINING_EQUIPMENT_MARKER )
   {
      ch_printf(
         ch,
         "You don't think %s would make useful training equipment.\r\n",
         obj->short_descr ? obj->short_descr : "that" );
      return;
   }

   ability_name = exercise_ability_name( obj->value[0] );
   if( !ability_name )
   {
      send_to_char(
         "That equipment is not configured for a valid attribute.\r\n",
         ch );
      return;
   }

   snprintf(
      train_argument,
      sizeof( train_argument ),
      "%s %s",
      ability_name,
      equipment );

   do_train( ch, train_argument );
}
