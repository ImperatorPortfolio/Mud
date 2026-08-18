/***************************************************************************
 * Zero Point - Unified Attribute Training
 *
 * Player-facing syntax:
 *   train <attribute>
 *   train <attribute> <equipment>
 *
 * Self-directed training is always available. Matching equipment and an
 * ACT_TRAIN trainer in the room improve the same shared progress calculation.
 ***************************************************************************/

#include <ctype.h>
#include <string.h>
#include "mud.h"

#define TRAINING_EQUIPMENT_MARKER 1414680135
#define TRAINING_DEFAULT_QUALITY  100
#define TRAINING_DEFAULT_PROGRESS 8
#define TRAINING_SELF_QUALITY     85
#define TRAINING_SELF_PROGRESS    5

struct training_session_data
{
   char attribute[16];
   char equipment[128];
   short ability;
   short trainer_modifier;
   short equipment_modifier;
   short base_progress;
   short exertion;
   bool used_equipment;
};

void apply_exertion_nutrition(
   CHAR_DATA *ch,
   int exertion )
{
   if( !ch
       || IS_NPC( ch )
       || !ch->pcdata )
      return;

   switch( exertion )
   {
      case EXERTION_LIGHT:
         if( number_range( 1, 4 ) == 1 )
            consume_nutrition_values(
               ch,
               0, 1, 0, 0, 0, 1 );
         break;

      case EXERTION_MODERATE:
         consume_nutrition_values(
            ch,
            0, 1, 0, 0, 0, 1 );
         break;

      case EXERTION_HEAVY:
         consume_nutrition_values(
            ch,
            1, 2, 0, 0, 1, 2 );
         break;

      case EXERTION_EXTREME:
         consume_nutrition_values(
            ch,
            1, 3, 1, 0, 1, 3 );
         break;

      default:
         break;
   }
}

static void split_training_arguments(
   const char *argument,
   char *attribute,
   size_t attribute_size,
   char *equipment,
   size_t equipment_size )
{
   const char *p;
   size_t length;

   if( attribute_size > 0 )
      attribute[0] = '\0';
   if( equipment_size > 0 )
      equipment[0] = '\0';

   if( !argument )
      return;

   p = argument;
   while( *p && isspace( ( unsigned char )*p ) )
      ++p;

   length = 0;
   while( p[length]
          && !isspace( ( unsigned char )p[length] ) )
      ++length;

   if( length > 0 && attribute_size > 0 )
   {
      size_t copy_length;

      copy_length = UMIN( length, attribute_size - 1 );
      memcpy( attribute, p, copy_length );
      attribute[copy_length] = '\0';
   }

   p += length;
   while( *p && isspace( ( unsigned char )*p ) )
      ++p;

   if( equipment_size > 0 )
      strlcpy( equipment, p, equipment_size );
}

static int training_ability_from_name(
   const char *name,
   int *effect,
   const char **canonical_name )
{
   if( !name || !effect || !canonical_name )
      return -1;

   if( !str_cmp( name, "str" )
       || !str_cmp( name, "strength" ) )
   {
      *effect = TRAIT_EFFECT_STR_POTENTIAL;
      *canonical_name = "strength";
      return ABILITY_SCORE_STR;
   }

   if( !str_cmp( name, "dex" )
       || !str_cmp( name, "dexterity" ) )
   {
      *effect = TRAIT_EFFECT_DEX_POTENTIAL;
      *canonical_name = "dexterity";
      return ABILITY_SCORE_DEX;
   }

   if( !str_cmp( name, "con" )
       || !str_cmp( name, "constitution" ) )
   {
      *effect = TRAIT_EFFECT_CON_POTENTIAL;
      *canonical_name = "constitution";
      return ABILITY_SCORE_CON;
   }

   if( !str_cmp( name, "int" )
       || !str_cmp( name, "intelligence" ) )
   {
      *effect = TRAIT_EFFECT_INT_POTENTIAL;
      *canonical_name = "intelligence";
      return ABILITY_SCORE_INT;
   }

   if( !str_cmp( name, "wis" )
       || !str_cmp( name, "wisdom" ) )
   {
      *effect = TRAIT_EFFECT_WIS_POTENTIAL;
      *canonical_name = "wisdom";
      return ABILITY_SCORE_WIS;
   }

   if( !str_cmp( name, "cha" )
       || !str_cmp( name, "charisma" ) )
   {
      *effect = TRAIT_EFFECT_CHA_POTENTIAL;
      *canonical_name = "charisma";
      return ABILITY_SCORE_CHA;
   }

   return -1;
}

static int training_ability_score(
   CHAR_DATA *ch,
   int ability )
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

static bool is_training_equipment(
   OBJ_DATA *obj )
{
   return obj
      && obj->value[5] == TRAINING_EQUIPMENT_MARKER;
}

static int training_exertion(
   OBJ_DATA *obj,
   int ability )
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

static CHAR_DATA *find_attribute_trainer(
   CHAR_DATA *ch )
{
   CHAR_DATA *mob;

   if( !ch || !ch->in_room )
      return NULL;

   for( mob = ch->in_room->first_person;
        mob;
        mob = mob->next_in_room )
   {
      if( IS_NPC( mob )
          && IS_SET( mob->act, ACT_TRAIN ) )
         return mob;
   }

   return NULL;
}

static void show_training_usage(
   CHAR_DATA *ch )
{
   send_to_char(
      "Train which attribute?\r\n"
      "Usage: train <attribute> [equipment]\r\n"
      "Attributes: strength, dexterity, constitution, intelligence, wisdom, charisma\r\n"
      "Examples: train strength   |   train strength weights\r\n",
      ch );
}

void do_train( CHAR_DATA *ch, const char *argument )
{
   char attribute_arg[MAX_INPUT_LENGTH];
   char equipment_arg[MAX_INPUT_LENGTH];
   int effect;
   int ability;
   int current;
   int potential;
   int trainer_modifier;
   int equipment_modifier;
   int base_progress;
   int session_exertion;
   CHAR_DATA *trainer;
   OBJ_DATA *equipment;
   const char *attribute_name;
   bool improved;

   if( IS_NPC( ch ) || !ch->pcdata )
      return;

   switch( ch->substate )
   {
      default:
      {
         struct training_session_data *session;

         split_training_arguments(
            argument,
            attribute_arg,
            sizeof( attribute_arg ),
            equipment_arg,
            sizeof( equipment_arg ) );

         if( attribute_arg[0] == '\0' )
         {
            show_training_usage( ch );
            return;
         }

         if( !IS_AWAKE( ch ) )
         {
            send_to_char( "In your dreams, or what?\r\n", ch );
            return;
         }

         if( !ch->in_room )
            return;

         effect = TRAIT_EFFECT_NONE;
         attribute_name = NULL;
         ability = training_ability_from_name(
            attribute_arg,
            &effect,
            &attribute_name );

         if( ability < ABILITY_SCORE_STR
             || ability > ABILITY_SCORE_CHA )
         {
            show_training_usage( ch );
            return;
         }

         current = training_ability_score( ch, ability );
         potential = get_trait_training_potential( ch, effect );

         if( effect == TRAIT_EFFECT_NONE
             || current >= potential )
         {
            ch_printf(
               ch,
               "&YYou have reached your natural potential for %s: %d.&w\r\n",
               attribute_name,
               potential );
            return;
         }

         equipment = NULL;
         equipment_modifier = TRAINING_SELF_QUALITY;
         base_progress = TRAINING_SELF_PROGRESS;
         session_exertion = training_exertion( NULL, ability );

         if( equipment_arg[0] != '\0' )
         {
            equipment = get_obj_here( ch, equipment_arg );

            if( !equipment )
            {
               send_to_char(
                  "You don't see that training equipment here.\r\n",
                  ch );
               return;
            }

            if( !is_training_equipment( equipment )
                || equipment->value[0] != ability )
            {
               ch_printf(
                  ch,
                  "You don't think %s would help you train your %s.\r\n",
                  equipment->short_descr
                     ? equipment->short_descr
                     : "that equipment",
                  attribute_name );
               return;
            }

            equipment_modifier = equipment->value[1];
            if( equipment_modifier <= 0 )
               equipment_modifier = TRAINING_DEFAULT_QUALITY;
            equipment_modifier =
               URANGE( 50, equipment_modifier, 150 );

            base_progress = equipment->value[2];
            if( base_progress <= 0 )
               base_progress = TRAINING_DEFAULT_PROGRESS;
            base_progress = URANGE( 1, base_progress, 25 );

            session_exertion =
               training_exertion( equipment, ability );
         }

         trainer = find_attribute_trainer( ch );
         trainer_modifier = 100;

         if( trainer )
         {
            int trainer_chance;

            trainer_chance =
               get_trait_training_chance(
                  ch,
                  trainer,
                  effect );

            if( trainer_chance > 0 )
               trainer_modifier =
                  URANGE(
                     75,
                     50 + trainer_chance,
                     150 );
         }

         CREATE(
            session,
            struct training_session_data,
            1 );

         strlcpy(
            session->attribute,
            attribute_name,
            sizeof( session->attribute ) );
         session->equipment[0] = '\0';

         if( equipment )
            strlcpy(
               session->equipment,
               equipment->short_descr
                  ? equipment->short_descr
                  : "training equipment",
               sizeof( session->equipment ) );

         session->ability = ability;
         session->trainer_modifier = trainer_modifier;
         session->equipment_modifier = equipment_modifier;
         session->base_progress = base_progress;
         session->exertion = session_exertion;
         session->used_equipment = equipment != NULL;

         ch->dest_buf = session;

         if( session->used_equipment )
         {
            ch_printf(
               ch,
               "&GYou begin training your %s using %s.&w\r\n",
               session->attribute,
               session->equipment );
         }
         else
         {
            ch_printf(
               ch,
               "&GYou begin a self-directed %s training session.&w\r\n",
               session->attribute );
         }

         if( trainer )
            ch_printf(
               ch,
               "&G%s guides your training.&w\r\n",
               capitalize( trainer->short_descr ) );

         add_timer(
            ch,
            TIMER_DO_FUN,
            10,
            do_train,
            1 );
         return;
      }

      case 1:
      {
         struct training_session_data *session;

         if( !ch->dest_buf )
            return;

         session =
            ( struct training_session_data * )ch->dest_buf;

         strlcpy(
            attribute_arg,
            session->attribute,
            sizeof( attribute_arg ) );
         strlcpy(
            equipment_arg,
            session->equipment,
            sizeof( equipment_arg ) );

         ability = session->ability;
         trainer_modifier = session->trainer_modifier;
         equipment_modifier = session->equipment_modifier;
         base_progress = session->base_progress;
         session_exertion = session->exertion;
         equipment = session->used_equipment ? ( OBJ_DATA * )1 : NULL;

         DISPOSE( ch->dest_buf );
         break;
      }

      case SUB_TIMER_DO_ABORT:
         DISPOSE( ch->dest_buf );
         ch->substate = SUB_NONE;

         send_to_char(
            "&RYou stop before completing your training session.&w\r\n",
            ch );
         return;
   }

   ch->substate = SUB_NONE;

   apply_exertion_nutrition(
      ch,
      session_exertion );

   improved =
      gain_ability_training_progress(
         ch,
         ability,
         base_progress,
         equipment_modifier,
         trainer_modifier );

   if( improved )
   {
      ch_printf(
         ch,
         "&GYour %s training pays off. Your ability has improved.&w\r\n",
         attribute_arg );
   }
   else if( equipment )
   {
      ch_printf(
         ch,
         "&GYou complete a productive %s session using %s.&w\r\n",
         attribute_arg,
         equipment_arg );
   }
   else
   {
      ch_printf(
         ch,
         "&GYou complete a productive self-directed %s training session.&w\r\n",
         attribute_arg );
   }
}
