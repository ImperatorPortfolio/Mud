/***************************************************************************
 * Zero Point - D20-P5 Proper Armor & Defense
 *
 * This file becomes the authoritative ascending Defense calculation.
 *
 * The inherited engine stores armor as a descending "lower is better" value
 * in CHAR_DATA::armor.  Equipment continues to update that field for legacy
 * compatibility, but EQUIPPED ARMOR NO LONGER DERIVES DEFENSE FROM:
 *
 *     (100 - ch->armor) / 10
 *
 * Instead, worn ITEM_ARMOR objects are inspected directly and translated
 * into bounded d20-scale armor and shield bonuses.
 *
 * The only remaining use of ch->armor here is to preserve non-equipment
 * APPLY_AC effects while those effects still use the inherited storage field.
 ***************************************************************************/

#include "mud.h"

/*
 * Convert the engine's 0-100 progression into a heroic level of 1-20.
 *
 *  1-5   -> 1
 *  6-10  -> 2
 *  ...
 * 96-100 -> 20
 */
static int get_heroic_level_from_progression( int progression )
{
   if( progression <= 0 )
      return 0;

   return URANGE(
      1,
      ( progression + 4 ) / 5,
      20 );
}

/*
 * Current transitional heroic Defense progression.
 *
 * Level  1 -> +2
 * Level  5 -> +4
 * Level 10 -> +6
 * Level 15 -> +9
 * Level 20 -> +11
 *
 * This remains separate from Base Attack Bonus.  A later class/profession
 * stage can replace this progression without touching armor math.
 */
static int get_heroic_defense_bonus( CHAR_DATA *ch )
{
   int heroic_level;

   if( !ch )
      return 0;

   heroic_level =
      get_heroic_level_from_progression(
         ch->top_level );

   if( heroic_level <= 0 )
      return 0;

   return
      1 + ( ( heroic_level + 1 ) / 2 );
}

/*
 * Existing area/object data expresses armor quality in obj->value[0].
 *
 * We keep that content usable, but translate worn protection into a bounded
 * d20-scale value rather than allowing every body slot to add a full bonus.
 *
 * Body, legs, head and about-body pieces contribute more coverage.  Small
 * pieces contribute one protection unit per value point.
 */
static int get_armor_piece_protection(
   OBJ_DATA *obj )
{
   int multiplier;

   if( !obj
       || obj->item_type != ITEM_ARMOR
       || obj->wear_loc == WEAR_NONE
       || obj->wear_loc == WEAR_SHIELD )
      return 0;

   switch( obj->wear_loc )
   {
      case WEAR_BODY:
         multiplier = 3;
         break;

      case WEAR_HEAD:
      case WEAR_LEGS:
      case WEAR_ABOUT:
         multiplier = 2;
         break;

      case WEAR_FEET:
      case WEAR_HANDS:
      case WEAR_ARMS:
      case WEAR_FINGER_L:
      case WEAR_FINGER_R:
      case WEAR_NECK_1:
      case WEAR_NECK_2:
      case WEAR_WAIST:
      case WEAR_WRIST_L:
      case WEAR_WRIST_R:
      case WEAR_HOLD:
      case WEAR_EYES:
         multiplier = 1;
         break;

      default:
         return 0;
   }

   return
      UMAX( 0, obj->value[0] )
      * multiplier;
}

/*
 * This duplicates the old coverage arithmetic only for isolating legacy
 * APPLY_AC effects from the CHAR_DATA::armor accumulator.
 *
 * It does NOT determine the new armor Defense bonus.
 */
static int get_legacy_equipment_protection(
   CHAR_DATA *ch )
{
   OBJ_DATA *obj;
   int protection;
   int multiplier;

   if( !ch )
      return 0;

   protection = 0;

   for( obj = ch->first_carrying;
        obj;
        obj = obj->next_content )
   {
      if( obj->item_type != ITEM_ARMOR
          || obj->wear_loc == WEAR_NONE )
         continue;

      switch( obj->wear_loc )
      {
         case WEAR_BODY:
            multiplier = 3;
            break;

         case WEAR_HEAD:
         case WEAR_LEGS:
         case WEAR_ABOUT:
            multiplier = 2;
            break;

         case WEAR_FEET:
         case WEAR_HANDS:
         case WEAR_ARMS:
         case WEAR_SHIELD:
         case WEAR_FINGER_L:
         case WEAR_FINGER_R:
         case WEAR_NECK_1:
         case WEAR_NECK_2:
         case WEAR_WAIST:
         case WEAR_WRIST_L:
         case WEAR_WRIST_R:
         case WEAR_HOLD:
         case WEAR_EYES:
            multiplier = 1;
            break;

         default:
            multiplier = 0;
            break;
      }

      if( multiplier > 0 )
      {
         protection +=
            UMAX( 0, obj->value[0] )
            * multiplier;
      }
   }

   return protection;
}

/*
 * Translate total worn armor protection into one Armor bonus.
 *
 * This prevents the MUD's many equipment slots from stacking linearly into
 * impossible tabletop-style Defense values.
 */
static int get_equipped_armor_bonus(
   CHAR_DATA *ch )
{
   OBJ_DATA *obj;
   int protection;

   if( !ch )
      return 0;

   protection = 0;

   for( obj = ch->first_carrying;
        obj;
        obj = obj->next_content )
   {
      protection +=
         get_armor_piece_protection(
            obj );
   }

   if( protection <= 0 )
      return 0;

   if( protection < 10 )
      return 1;

   if( protection < 20 )
      return 2;

   if( protection < 35 )
      return 3;

   if( protection < 50 )
      return 4;

   if( protection < 70 )
      return 5;

   if( protection < 95 )
      return 6;

   if( protection < 125 )
      return 7;

   if( protection < 160 )
      return 8;

   return 9;
}

/*
 * Energy/physical shields are handled as a distinct Defense source.
 * Only an ITEM_ARMOR object actually worn in WEAR_SHIELD contributes.
 */
static int get_equipped_shield_bonus(
   CHAR_DATA *ch )
{
   OBJ_DATA *obj;
   int best_bonus;

   if( !ch )
      return 0;

   best_bonus = 0;

   for( obj = ch->first_carrying;
        obj;
        obj = obj->next_content )
   {
      int value;
      int bonus;

      if( obj->item_type != ITEM_ARMOR
          || obj->wear_loc != WEAR_SHIELD )
         continue;

      value = UMAX( 0, obj->value[0] );

      if( value <= 5 )
         bonus = 1;
      else if( value <= 12 )
         bonus = 2;
      else
         bonus = 3;

      best_bonus =
         UMAX(
            best_bonus,
            bonus );
   }

   return best_bonus;
}

/*
 * Armor limits only positive Dexterity bonuses.
 * Negative DEX modifiers always apply in full.
 */
static int get_armor_max_dex_bonus(
   int armor_bonus )
{
   if( armor_bonus <= 2 )
      return 100;

   if( armor_bonus <= 4 )
      return 5;

   if( armor_bonus <= 6 )
      return 3;

   if( armor_bonus <= 8 )
      return 2;

   return 1;
}

/*
 * Transitional support for old APPLY_AC effects.
 *
 * Equipment protection is removed from the old armor accumulator first.
 * What remains represents spell/effect/builder modifiers that have not yet
 * been migrated to a native Defense field.
 *
 * Positive legacy protection gives a positive misc Defense bonus.
 * Negative legacy protection gives a penalty.
 */
static int get_legacy_misc_defense_bonus(
   CHAR_DATA *ch )
{
   int legacy_total_protection;
   int equipment_protection;
   int effect_protection;

   if( !ch )
      return 0;

   legacy_total_protection =
      100 - ch->armor;

   equipment_protection =
      get_legacy_equipment_protection(
         ch );

   effect_protection =
      legacy_total_protection
      - equipment_protection;

   return
      URANGE(
         -10,
         effect_protection / 10,
         10 );
}

/*
 * Authoritative ascending Defense.
 *
 * Defense =
 *   10
 *   + heroic Defense progression
 *   + DEX modifier (subject to armor maximum)
 *   + armor bonus
 *   + shield bonus
 *   + transitional non-equipment Defense effects
 */
int get_defense( CHAR_DATA *ch )
{
   int heroic_bonus;
   int dex_bonus;
   int max_dex_bonus;
   int armor_bonus;
   int shield_bonus;
   int misc_bonus;
   int defense;

   if( !ch )
      return 10;

   heroic_bonus =
      get_heroic_defense_bonus(
         ch );

   armor_bonus =
      get_equipped_armor_bonus(
         ch );

   shield_bonus =
      get_equipped_shield_bonus(
         ch );

   if( IS_AWAKE( ch ) )
   {
      dex_bonus =
         get_ability_modifier(
            get_curr_dex( ch ) );
   }
   else
   {
      dex_bonus = 0;
   }

   max_dex_bonus =
      get_armor_max_dex_bonus(
         armor_bonus );

   if( dex_bonus > max_dex_bonus )
      dex_bonus = max_dex_bonus;

   misc_bonus =
      get_legacy_misc_defense_bonus(
         ch );

   defense =
      10
      + heroic_bonus
      + dex_bonus
      + armor_bonus
      + shield_bonus
      + misc_bonus;

   return
      UMAX(
         1,
         defense );
}
