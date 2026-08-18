/***************************************************************************
 * Zero Point - Natural Attribute Development
 *
 * Converts meaningful ordinary gameplay into small, rate-limited hidden
 * attribute progress. This is intentionally a thin source layer over the
 * shared gain_ability_training_progress() backend so nutrition and natural
 * trait potential remain authoritative regardless of where progress came
 * from.
 *
 * Sources in this first passive slice:
 *   - active combat develops Constitution as physical endurance;
 *   - recently executed D20 skill commands develop their governing ability.
 *
 * Maintenance is deliberately forgiving. Hidden, unbanked development
 * progress receives a fourteen-day grace period and then loses only one point
 * per additional seven days of continued disuse. Permanent ability scores are
 * not reduced by this pass.
 ***************************************************************************/

#include "mud.h"

#define NATURAL_SCAN_SECONDS          5
#define NATURAL_SKILL_COOLDOWN       90
#define NATURAL_COMBAT_COOLDOWN     180
#define NATURAL_SKILL_PROGRESS        1
#define NATURAL_COMBAT_PROGRESS       1

#define MAINTENANCE_SCAN_SECONDS   3600
#define MAINTENANCE_GRACE_SECONDS  ( 14 * 24 * 60 * 60 )
#define MAINTENANCE_DECAY_SECONDS  (  7 * 24 * 60 * 60 )
#define MAINTENANCE_DECAY_PROGRESS 1

void legacy_update_handler( void );

static const char *natural_ability_name( int ability )
{
   switch( ability )
   {
      case ABILITY_SCORE_STR: return "strength";
      case ABILITY_SCORE_DEX: return "dexterity";
      case ABILITY_SCORE_CON: return "constitution";
      case ABILITY_SCORE_INT: return "intelligence";
      case ABILITY_SCORE_WIS: return "wisdom";
      case ABILITY_SCORE_CHA: return "charisma";
      default: return "ability";
   }
}

static bool natural_gain_ready(
   CHAR_DATA *ch,
   int ability,
   int cooldown )
{
   time_t last_used;

   if( !ch
       || IS_NPC( ch )
       || !ch->pcdata )
      return FALSE;

   if( ability < ABILITY_SCORE_STR
       || ability > ABILITY_SCORE_CHA )
      return FALSE;

   last_used =
      ch->pcdata->attribute_last_used[ability];

   if( last_used <= 0 )
      return TRUE;

   return
      current_time - last_used >= cooldown;
}

static void award_natural_progress(
   CHAR_DATA *ch,
   int ability,
   int base_progress,
   int cooldown )
{
   bool improved;

   if( !natural_gain_ready(
          ch,
          ability,
          cooldown ) )
      return;

   improved =
      gain_ability_training_progress(
         ch,
         ability,
         base_progress,
         100,
         100 );

   if( improved )
   {
      ch_printf(
         ch,
         "&GThrough continued use, your %s has improved.&D\r\n",
         natural_ability_name( ability ) );
   }
}

static int get_recent_skill_ability(
   CHAR_DATA *ch )
{
   int sn;

   if( !ch
       || !ch->last_cmd
       || ch->wait <= 0 )
      return -1;

   for( sn = 0; sn < top_sn; ++sn )
   {
      SKILLTYPE *skill;

      skill = skill_table[sn];

      if( !skill
          || !skill->skill_fun )
         continue;

      if( skill->skill_fun == ch->last_cmd )
      {
         int ability;

         ability = get_skill_ability( sn );

         if( ability >= ABILITY_SCORE_STR
             && ability <= ABILITY_SCORE_CHA )
            return ability;

         return -1;
      }
   }

   return -1;
}

static void natural_attribute_update( void )
{
   CHAR_DATA *ch;

   for( ch = first_char; ch; ch = ch->next )
   {
      int ability;

      if( IS_NPC( ch )
          || !ch->pcdata
          || !ch->desc )
         continue;

      /*
       * Staying engaged in a real fight develops physical endurance.
       * Offensive technique is covered independently by governing abilities
       * on actual combat skills, avoiding blanket STR/DEX inflation.
       */
      if( ch->fighting )
      {
         award_natural_progress(
            ch,
            ABILITY_SCORE_CON,
            NATURAL_COMBAT_PROGRESS,
            NATURAL_COMBAT_COOLDOWN );
      }

      /*
       * check_skill() records real do-fun skills in last_cmd and applies a
       * wait state. Sampling only while that wait remains means an old command
       * cannot keep generating progress while the player stands idle.
       */
      ability = get_recent_skill_ability( ch );

      if( ability >= ABILITY_SCORE_STR
          && ability <= ABILITY_SCORE_CHA )
      {
         award_natural_progress(
            ch,
            ability,
            NATURAL_SKILL_PROGRESS,
            NATURAL_SKILL_COOLDOWN );
      }
   }
}

static void maintenance_attribute_update( void )
{
   CHAR_DATA *ch;

   for( ch = first_char; ch; ch = ch->next )
   {
      int ability;

      if( IS_NPC( ch )
          || !ch->pcdata )
         continue;

      for( ability = ABILITY_SCORE_STR;
           ability <= ABILITY_SCORE_CHA;
           ++ability )
      {
         time_t last_used;
         time_t inactive;
         time_t overdue;
         int decay_steps;
         int progress;

         progress =
            ch->pcdata->attribute_progress[ability];

         if( progress <= 0 )
            continue;

         last_used =
            ch->pcdata->attribute_last_used[ability];

         /*
          * Old characters may have progress from before timestamps existed.
          * Give them a fresh grace period instead of treating that progress as
          * ancient and deleting it on first login/update.
          */
         if( last_used <= 0 )
         {
            ch->pcdata->attribute_last_used[ability] =
               current_time;
            continue;
         }

         inactive =
            current_time - last_used;

         if( inactive <= MAINTENANCE_GRACE_SECONDS )
            continue;

         overdue =
            inactive - MAINTENANCE_GRACE_SECONDS;

         decay_steps =
            1 + (int)( overdue / MAINTENANCE_DECAY_SECONDS );

         decay_steps =
            UMIN(
               decay_steps,
               progress / MAINTENANCE_DECAY_PROGRESS );

         if( decay_steps <= 0 )
            continue;

         ch->pcdata->attribute_progress[ability] =
            UMAX(
               0,
               progress
               - ( decay_steps * MAINTENANCE_DECAY_PROGRESS ) );

         /*
          * Advance only the decay anchor, not all the way to current_time.
          * Continued disuse therefore loses one additional point per seven
          * days while preserving the original fourteen-day grace semantics.
          */
         ch->pcdata->attribute_last_used[ability] +=
            decay_steps * MAINTENANCE_DECAY_SECONDS;
      }
   }
}

/*
 * Keep the inherited update scheduler authoritative, then perform bounded
 * passive-development and maintenance samples. update.c is compiled with its
 * original update_handler renamed to legacy_update_handler.
 */
void update_handler( void )
{
   static time_t next_natural_scan = 0;
   static time_t next_maintenance_scan = 0;

   legacy_update_handler();

   if( current_time >= next_natural_scan )
   {
      next_natural_scan =
         current_time + NATURAL_SCAN_SECONDS;

      natural_attribute_update();
   }

   if( current_time >= next_maintenance_scan )
   {
      next_maintenance_scan =
         current_time + MAINTENANCE_SCAN_SECONDS;

      maintenance_attribute_update();
   }
}
