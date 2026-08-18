/***************************************************************************
 * Zero Point - Attribute Training Guards
 *
 * Authoritative natural-potential enforcement for trainer chance and shared
 * attribute progress. Future exercise/passive gain sources must flow through
 * gain_ability_training_progress(), so the ceiling cannot be bypassed.
 ***************************************************************************/

#include "mud.h"

static int get_training_ability_score(
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

static int get_training_trait_effect(
   int ability )
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

int get_trait_training_chance(
   CHAR_DATA *ch,
   CHAR_DATA *trainer,
   int effect )
{
   int current;
   int trainer_stat;
   int potential;
   int trainer_adjust;
   int chance_value;

   if( !ch || !trainer )
      return 0;

   current = 0;
   trainer_stat = 0;

   switch( effect )
   {
      case TRAIT_EFFECT_STR_POTENTIAL:
         current = ch->perm_str;
         trainer_stat = trainer->perm_str;
         break;

      case TRAIT_EFFECT_CON_POTENTIAL:
         current = ch->perm_con;
         trainer_stat = trainer->perm_con;
         break;

      case TRAIT_EFFECT_DEX_POTENTIAL:
         current = ch->perm_dex;
         trainer_stat = trainer->perm_dex;
         break;

      case TRAIT_EFFECT_INT_POTENTIAL:
         current = ch->perm_int;
         trainer_stat = trainer->perm_int;
         break;

      case TRAIT_EFFECT_WIS_POTENTIAL:
         current = ch->perm_wis;
         trainer_stat = trainer->perm_wis;
         break;

      case TRAIT_EFFECT_CHA_POTENTIAL:
         current = ch->perm_cha;
         trainer_stat = trainer->perm_cha;
         break;

      default:
         return 0;
   }

   potential = get_trait_training_potential( ch, effect );

   if( current >= potential )
      return 0;

   trainer_adjust =
      URANGE( -20, ( trainer_stat - current ) * 3, 20 );

   chance_value =
      70
      - ( 12 * ( current - potential ) )
      + trainer_adjust;

   return URANGE( 1, chance_value, 95 );
}

bool gain_ability_training_progress(
   CHAR_DATA *ch,
   int ability,
   int base_progress,
   int equipment_modifier,
   int trainer_modifier )
{
   int effect;
   int current;
   int potential;
   int nutrition_modifier;
   int progress;
   int required;

   if( !ch
       || IS_NPC( ch )
       || !ch->pcdata )
      return FALSE;

   if( ability < ABILITY_SCORE_STR
       || ability > ABILITY_SCORE_CHA )
      return FALSE;

   if( base_progress <= 0 )
      return FALSE;

   effect = get_training_trait_effect( ability );
   current = get_training_ability_score( ch, ability );
   potential = get_trait_training_potential( ch, effect );

   /*
    * Natural potential is an authoritative backend boundary. Do not award
    * additional progress while the character is at or above the ceiling.
    */
   if( effect == TRAIT_EFFECT_NONE
       || current >= potential )
      return FALSE;

   equipment_modifier =
      URANGE( 50, equipment_modifier, 150 );

   trainer_modifier =
      URANGE( 50, trainer_modifier, 150 );

   nutrition_modifier =
      get_nutrition_training_modifier(
         ch,
         ability );

   progress =
      base_progress
      * nutrition_modifier
      * equipment_modifier
      * trainer_modifier
      / 1000000;

   progress = UMAX( 1, progress );

   ch->pcdata->attribute_progress[ability] +=
      progress;

   ch->pcdata->attribute_last_used[ability] =
      current_time;

   required =
      get_ability_training_progress_required(
         ch,
         ability );

   if( ch->pcdata->attribute_progress[ability]
       < required )
      return FALSE;

   ch->pcdata->attribute_progress[ability] -=
      required;

   switch( ability )
   {
      case ABILITY_SCORE_STR:
         ++ch->perm_str;
         break;

      case ABILITY_SCORE_DEX:
         ++ch->perm_dex;
         break;

      case ABILITY_SCORE_CON:
         ++ch->perm_con;
         break;

      case ABILITY_SCORE_INT:
         ++ch->perm_int;
         break;

      case ABILITY_SCORE_WIS:
         ++ch->perm_wis;
         break;

      case ABILITY_SCORE_CHA:
         ++ch->perm_cha;
         break;

      default:
         return FALSE;
   }

   return TRUE;
}
