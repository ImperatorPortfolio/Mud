/***************************************************************************
 * Zero Point - Equipment-based Attribute Training
 *
 * Training equipment uses the existing object Values persistence:
 *   value[0] = ability (ABILITY_SCORE_*)
 *   value[1] = equipment quality modifier (50-150, 0 => 100)
 *   value[2] = base progress per completed session (1-25, 0 => 8)
 *   value[3] = exertion override (0 auto, 1 light, 2 moderate, 3 heavy, 4 extreme)
 *   value[4] = reserved
 *   value[5] = TRAINING_EQUIPMENT_MARKER
 *
 * This keeps area data fully data-driven without adding a second persistence
 * path. Dedicated training objects should use an otherwise passive object type
 * such as trash or furniture so these values are not consumed by another
 * object behaviour.
 ***************************************************************************/

#include "mud.h"

#define TRAINING_EQUIPMENT_MARKER 1414680135
#define TRAINING_DEFAULT_QUALITY  100
#define TRAINING_DEFAULT_PROGRESS 8

extern "C" void do_exercise( CHAR_DATA *ch, const char *argument );

struct exercise_session_data
{
   char equipment[128];
   short ability;
   short equipment_modifier;
   short base_progress;
   short exertion;
};

static int exercise_trait_effect( int ability )
{
   switch( ability )
   {
      case ABILITY_SCORE_STR: return TRAIT_EFFECT_STR_POTENTIAL;
      case ABILITY_SCORE_DEX: return TRAIT_EFFECT_DEX_POTENTIAL;
      case ABILITY_SCORE_CON: return TRAIT_EFFECT_CON_POTENTIAL;
      case ABILITY_SCORE_INT: return TRAIT_EFFECT_INT_POTENTIAL;
      case ABILITY_SCORE_WIS: return TRAIT_EFFECT_WIS_POTENTIAL;
      case ABILITY_SCORE_CHA: return TRAIT_EFFECT_CHA_POTENTIAL;
      default: return TRAIT_EFFECT_NONE;
   }
}

static int exercise_ability_score( CHAR_DATA *ch, int ability )
{
   if( !ch )
      return 0;

   switch( ability )
   {
      case ABILITY_SCORE_STR: return ch->perm_str;
      case ABILITY_SCORE_DEX: return ch->perm_dex;
      case ABILITY_SCORE_CON: return ch->perm_con;
      case ABILITY_SCORE_INT: return ch->perm_int;
      case ABILITY_SCORE_WIS: return ch->perm_wis;
      case ABILITY_SCORE_CHA: return ch->perm_cha;
      default: return 0;
   }
}

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
      default: return "unknown";
   }
}

static bool is_training_equipment( OBJ_DATA *obj )
{
   if( !obj )
      return FALSE;

   return obj->value[5] == TRAINING_EQUIPMENT_MARKER;
}

static int exercise_exertion( OBJ_DATA *obj, int ability )
{
   int configured;

   configured = obj ? obj->value[3] : 0;

   switch( configured )
   {
      case 1: return EXERTION_LIGHT;
      case 2: return EXERTION_MODERATE;
      case 3: return EXERTION_HEAVY;
      case 4: return EXERTION_EXTREME;
      default:
         break;
   }

   if( ability == ABILITY_SCORE_STR
       || ability == ABILITY_SCORE_DEX
       || ability == ABILITY_SCORE_CON )
      return EXERTION_HEAVY;

   return EXERTION_LIGHT;
}

static void list_training_equipment( CHAR_DATA *ch )
{
   OBJ_DATA *obj;
   bool found;

   if( !ch || !ch->in_room )
      return;

   found = FALSE;

   send_to_char( "Available training equipment:\r\n", ch );

   for( obj = ch->in_room->first_content; obj; obj = obj->next_content )
   {
      int ability;
      int quality;

      if( !is_training_equipment( obj ) )
         continue;

      ability = obj->value[0];
      if( ability < ABILITY_SCORE_STR || ability > ABILITY_SCORE_CHA )
         continue;

      quality = obj->value[1];
      if( quality <= 0 )
         quality = TRAINING_DEFAULT_QUALITY;
      quality = URANGE( 50, quality, 150 );

      ch_printf(
         ch,
         "  %-32s  %-12s %d%%\r\n",
         obj->short_descr ? obj->short_descr : "training equipment",
         exercise_ability_name( ability ),
         quality );

      found = TRUE;
   }

   if( !found )
      send_to_char( "  None.\r\n", ch );
}

void do_exercise( CHAR_DATA *ch, const char *argument )
{
   char arg[MAX_INPUT_LENGTH];
   OBJ_DATA *obj;
   int ability;
   int effect;
   int current;
   int potential;
   int quality;
   int base_progress;
   int exertion;
   bool improved;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   strlcpy( arg, argument ? argument : "", sizeof( arg ) );

   switch( ch->substate )
   {
      default:
      {
         exercise_session_data *session;

         if( arg[0] == '\0' )
         {
            send_to_char( "Exercise with what?\r\n", ch );
            list_training_equipment( ch );
            return;
         }

         if( !IS_AWAKE( ch ) )
         {
            send_to_char( "In your dreams, or what?\r\n", ch );
            return;
         }

         obj = get_obj_here( ch, arg );
         if( !obj )
         {
            send_to_char( "You don't see that training equipment here.\r\n", ch );
            return;
         }

         if( !is_training_equipment( obj ) )
         {
            ch_printf(
               ch,
               "%s is not training equipment.\r\n",
               capitalize( obj->short_descr ) );
            return;
         }

         ability = obj->value[0];
         if( ability < ABILITY_SCORE_STR || ability > ABILITY_SCORE_CHA )
         {
            send_to_char( "That equipment is not configured for a valid ability.\r\n", ch );
            return;
         }

         effect = exercise_trait_effect( ability );
         current = exercise_ability_score( ch, ability );
         potential = get_trait_training_potential( ch, effect );

         if( effect == TRAIT_EFFECT_NONE || current >= potential )
         {
            ch_printf(
               ch,
               "&YYou have reached your natural potential for %s: %d.&w\r\n",
               exercise_ability_name( ability ),
               potential );
            return;
         }

         quality = obj->value[1];
         if( quality <= 0 )
            quality = TRAINING_DEFAULT_QUALITY;
         quality = URANGE( 50, quality, 150 );

         base_progress = obj->value[2];
         if( base_progress <= 0 )
            base_progress = TRAINING_DEFAULT_PROGRESS;
         base_progress = URANGE( 1, base_progress, 25 );

         exertion = exercise_exertion( obj, ability );

         CREATE( session, exercise_session_data, 1 );
         strlcpy(
            session->equipment,
            obj->short_descr ? obj->short_descr : "training equipment",
            sizeof( session->equipment ) );
         session->ability = ability;
         session->equipment_modifier = quality;
         session->base_progress = base_progress;
         session->exertion = exertion;

         ch->dest_buf = session;

         ch_printf(
            ch,
            "&GYou begin a %s training session using %s.&w\r\n",
            exercise_ability_name( ability ),
            session->equipment );

         add_timer( ch, TIMER_DO_FUN, 10, do_exercise, 1 );
         return;
      }

      case 1:
      {
         exercise_session_data *session;

         if( !ch->dest_buf )
            return;

         session = ( exercise_session_data * )ch->dest_buf;
         ability = session->ability;
         quality = session->equipment_modifier;
         base_progress = session->base_progress;
         exertion = session->exertion;
         strlcpy( arg, session->equipment, sizeof( arg ) );

         DISPOSE( ch->dest_buf );
         break;
      }

      case SUB_TIMER_DO_ABORT:
         DISPOSE( ch->dest_buf );
         ch->substate = SUB_NONE;
         send_to_char( "&RYou stop before completing your exercise session.&w\r\n", ch );
         return;
   }

   ch->substate = SUB_NONE;

   apply_exertion_nutrition( ch, exertion );

   improved = gain_ability_training_progress(
      ch,
      ability,
      base_progress,
      quality,
      100 );

   if( improved )
   {
      ch_printf(
         ch,
         "&GYour work with %s pays off. Your %s improves.&w\r\n",
         arg,
         exercise_ability_name( ability ) );
   }
   else
   {
      ch_printf(
         ch,
         "&GYou complete a productive %s session using %s.&w\r\n",
         exercise_ability_name( ability ),
         arg );
   }
}
