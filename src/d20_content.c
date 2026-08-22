/***************************************************************************
 * Zero Point - D20 area-content normalization
 *
 * The world still uses SWR's native 1-100 progression.  This module does
 * not alter that progression.  It converts legacy area prototype fields
 * that now feed directly into the D20 resolution layer so old world data
 * cannot bypass the modern attack, Defense and equipment contracts.
 *
 * This is deliberately a post-load compatibility boundary: every area in
 * area.lst remains readable in its inherited format, while all prototypes
 * entering live gameplay obey the same D20 rules as the System Nexus
 * Foundation package.
 ***************************************************************************/

#include <limits.h>
#include "mud.h"

void legacy_boot_db( bool fCopyOver );

extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
extern OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];

namespace
{
struct d20_mobile_profile
{
   short ac;
   short hitnodice;
   short hitsizedice;
   short hitplus;
   short damnodice;
   short damsizedice;
   short damplus;
   short numattacks;
   short hitroll;
   short damroll;
   short strength;
   short intelligence;
   short wisdom;
   short dexterity;
   short constitution;
   short charisma;
   short luck;
   short force;
   bool replace_hit_dice;
   bool replace_damage_dice;
};

struct d20_content_counts
{
   int mobiles_checked;
   int mobiles_normalized;
   int objects_checked;
   int armor_normalized;
   int affects_normalized;
   int live_mobiles_normalized;
   int live_objects_normalized;
};

int d20_heroic_band( int progression )
{
   if( progression <= 0 )
      return 1;

   return URANGE( 1, ( progression + 4 ) / 5, 20 );
}

short d20_ability_score( int value, bool force_score )
{
   if( force_score )
      return ( short )URANGE( 0, value, 30 );

   return ( short )URANGE( 3, value, 30 );
}

short d20_direct_modifier( int value )
{
   int magnitude;
   int sign;

   if( value >= -1 && value <= 1 )
      return ( short )value;

   sign = value < 0 ? -1 : 1;
   magnitude = value < 0 ? -value : value;
   magnitude = URANGE( 1, ( magnitude + 2 ) / 5, 5 );
   return ( short )( sign * magnitude );
}

void d20_reference_damage( int heroic, short *number, short *size, short *plus )
{
   if( heroic <= 2 )
   {
      *number = 1;
      *size = 4;
      *plus = 0;
   }
   else if( heroic <= 4 )
   {
      *number = 1;
      *size = 6;
      *plus = 1;
   }
   else if( heroic <= 7 )
   {
      *number = 1;
      *size = 8;
      *plus = 2;
   }
   else if( heroic <= 10 )
   {
      *number = 2;
      *size = 6;
      *plus = 2;
   }
   else if( heroic <= 15 )
   {
      *number = 2;
      *size = 8;
      *plus = 3;
   }
   else
   {
      *number = 3;
      *size = 8;
      *plus = 4;
   }
}

int dice_maximum( int number, int size, int plus )
{
   if( number <= 0 || size <= 0 )
      return 0;

   return UMAX( 1, number * size + plus );
}

int dice_average( int number, int size, int plus )
{
   if( number <= 0 || size <= 0 )
      return 1;

   return UMAX( 1, number * ( size + 1 ) / 2 + plus );
}

d20_mobile_profile make_mobile_profile( const MOB_INDEX_DATA *mob )
{
   d20_mobile_profile profile = {};
   int heroic;
   int con_modifier;
   int old_hit_max;
   int reference_hit_max;
   int old_damage_max;
   int reference_damage_max;
   short reference_damage_number;
   short reference_damage_size;
   short reference_damage_plus;

   heroic = d20_heroic_band( mob ? mob->level : 1 );

   profile.ac = 100;
   profile.hitroll = d20_direct_modifier( mob ? mob->hitroll : 0 );
   profile.damroll = d20_direct_modifier( mob ? mob->damroll : 0 );
   profile.numattacks = ( short )URANGE( 1, mob && mob->numattacks > 0 ? mob->numattacks : 1, 2 );

   profile.strength = d20_ability_score( mob ? mob->perm_str : 10, false );
   profile.intelligence = d20_ability_score( mob ? mob->perm_int : 10, false );
   profile.wisdom = d20_ability_score( mob ? mob->perm_wis : 10, false );
   profile.dexterity = d20_ability_score( mob ? mob->perm_dex : 10, false );
   profile.constitution = d20_ability_score( mob ? mob->perm_con : 10, false );
   profile.charisma = d20_ability_score( mob ? mob->perm_cha : 10, false );
   profile.luck = d20_ability_score( mob ? mob->perm_lck : 10, false );
   profile.force = d20_ability_score( mob ? mob->perm_frc : 0, true );

   con_modifier = get_ability_modifier( profile.constitution );
   profile.hitnodice = ( short )heroic;
   profile.hitsizedice = 10;
   profile.hitplus = ( short )( heroic * UMAX( 2, con_modifier ) );

   old_hit_max = mob ? dice_maximum( mob->hitnodice, mob->hitsizedice, mob->hitplus ) : 0;
   reference_hit_max = dice_maximum( profile.hitnodice, profile.hitsizedice, profile.hitplus );
   profile.replace_hit_dice = !mob || mob->hitnodice <= 0 || mob->hitsizedice <= 0
      || old_hit_max > reference_hit_max * 3 / 2;

   d20_reference_damage( heroic,
                         &reference_damage_number,
                         &reference_damage_size,
                         &reference_damage_plus );
   profile.damnodice = reference_damage_number;
   profile.damsizedice = reference_damage_size;
   profile.damplus = reference_damage_plus;

   old_damage_max = mob ? dice_maximum( mob->damnodice, mob->damsizedice, mob->damplus ) : 0;
   reference_damage_max = dice_maximum( profile.damnodice, profile.damsizedice, profile.damplus );
   profile.replace_damage_dice = !mob || mob->damnodice <= 0 || mob->damsizedice <= 0
      || old_damage_max > reference_damage_max * 3 / 2;

   if( mob && !profile.replace_hit_dice )
   {
      profile.hitnodice = mob->hitnodice;
      profile.hitsizedice = mob->hitsizedice;
      profile.hitplus = mob->hitplus;
   }

   if( mob && !profile.replace_damage_dice )
   {
      profile.damnodice = mob->damnodice;
      profile.damsizedice = mob->damsizedice;
      profile.damplus = mob->damplus;
   }

   return profile;
}

bool mobile_profile_changes( const MOB_INDEX_DATA *mob, const d20_mobile_profile &profile )
{
   if( !mob )
      return false;

   return mob->mobthac0 != 0
      || mob->ac != profile.ac
      || mob->hitnodice != profile.hitnodice
      || mob->hitsizedice != profile.hitsizedice
      || mob->hitplus != profile.hitplus
      || mob->damnodice != profile.damnodice
      || mob->damsizedice != profile.damsizedice
      || mob->damplus != profile.damplus
      || mob->numattacks != profile.numattacks
      || mob->hitroll != profile.hitroll
      || mob->damroll != profile.damroll
      || mob->perm_str != profile.strength
      || mob->perm_int != profile.intelligence
      || mob->perm_wis != profile.wisdom
      || mob->perm_dex != profile.dexterity
      || mob->perm_con != profile.constitution
      || mob->perm_cha != profile.charisma
      || mob->perm_lck != profile.luck
      || mob->perm_frc != profile.force;
}

void apply_mobile_profile( MOB_INDEX_DATA *mob, const d20_mobile_profile &profile )
{
   if( !mob )
      return;

   mob->mobthac0 = 0;
   mob->ac = profile.ac;
   mob->hitnodice = profile.hitnodice;
   mob->hitsizedice = profile.hitsizedice;
   mob->hitplus = profile.hitplus;
   mob->damnodice = profile.damnodice;
   mob->damsizedice = profile.damsizedice;
   mob->damplus = profile.damplus;
   mob->numattacks = profile.numattacks;
   mob->hitroll = profile.hitroll;
   mob->damroll = profile.damroll;
   mob->perm_str = profile.strength;
   mob->perm_int = profile.intelligence;
   mob->perm_wis = profile.wisdom;
   mob->perm_dex = profile.dexterity;
   mob->perm_con = profile.constitution;
   mob->perm_cha = profile.charisma;
   mob->perm_lck = profile.luck;
   mob->perm_frc = profile.force;
}

void normalize_live_mobile( CHAR_DATA *ch, d20_content_counts *counts )
{
   MOB_INDEX_DATA *prototype;
   d20_mobile_profile profile;
   int old_base_ac;
   int old_max_hit;
   int new_max_hit;
   int new_hit;
   bool changed;

   if( !ch || !IS_NPC( ch ) || !( prototype = ch->pIndexData ) )
      return;

   profile = make_mobile_profile( prototype );
   changed = mobile_profile_changes( prototype, profile );
   if( !changed )
      return;

   old_base_ac = prototype->ac
      ? prototype->ac
      : ( int )( LEVEL_HERO - prototype->level * 2.5 );
   ch->armor += profile.ac - old_base_ac;

   ch->mobthac0 = 0;
   ch->hitroll += profile.hitroll - prototype->hitroll;
   ch->damroll += profile.damroll - prototype->damroll;
   ch->numattacks = profile.numattacks;

   ch->perm_str = profile.strength;
   ch->perm_int = profile.intelligence;
   ch->perm_wis = profile.wisdom;
   ch->perm_dex = profile.dexterity;
   ch->perm_con = profile.constitution;
   ch->perm_cha = profile.charisma;
   ch->perm_lck = profile.luck;
   ch->perm_frc = profile.force;

   ch->barenumdie = profile.damnodice;
   ch->baresizedie = profile.damsizedice;
   ch->damplus = profile.damplus;

   if( profile.replace_hit_dice )
   {
      old_max_hit = UMAX( 1, ch->max_hit );
      new_max_hit = dice_average( profile.hitnodice, profile.hitsizedice, profile.hitplus );
      new_hit = ch->hit * new_max_hit / old_max_hit;
      ch->max_hit = ( short )URANGE( 1, new_max_hit, SHRT_MAX );
      ch->hit = ( short )URANGE( 1, new_hit, ch->max_hit );
   }

   if( counts )
      ++counts->live_mobiles_normalized;
}

bool normalize_object_affects( AFFECT_DATA *first, CHAR_DATA *carrier, bool equipped )
{
   AFFECT_DATA *affect;
   const int hitroll_location = get_atype( "hitroll" );
   const int damroll_location = get_atype( "damroll" );
   bool changed = false;

   for( affect = first; affect; affect = affect->next )
   {
      int replacement;

      if( affect->location != hitroll_location && affect->location != damroll_location )
         continue;
      if( affect->modifier >= -5 && affect->modifier <= 5 )
         continue;

      replacement = d20_direct_modifier( affect->modifier );
      if( replacement == affect->modifier )
         continue;

      if( carrier && equipped )
         affect_modify( carrier, affect, false );
      affect->modifier = replacement;
      if( carrier && equipped )
         affect_modify( carrier, affect, true );
      changed = true;
   }

   return changed;
}

bool normalize_live_object( OBJ_DATA *obj )
{
   bool changed = false;

   if( !obj )
      return false;

   if( obj->item_type == ITEM_ARMOR )
   {
      if( obj->value[0] <= 0 && obj->value[1] > 0 )
      {
         obj->value[0] = obj->value[1];
         if( obj->carried_by && obj->wear_loc != WEAR_NONE )
            obj->carried_by->armor -= apply_ac( obj, obj->wear_loc );
         changed = true;
      }
      else if( obj->value[1] <= 0 && obj->value[0] > 0 )
      {
         obj->value[1] = obj->value[0];
         changed = true;
      }
   }

   if( normalize_object_affects( obj->first_affect,
                                 obj->carried_by,
                                 obj->wear_loc != WEAR_NONE ) )
      changed = true;

   return changed;
}

bool normalize_object_prototype( OBJ_INDEX_DATA *obj, d20_content_counts *counts )
{
   bool changed = false;

   if( !obj )
      return false;

   if( obj->item_type == ITEM_ARMOR )
   {
      if( obj->value[0] <= 0 && obj->value[1] > 0 )
      {
         obj->value[0] = obj->value[1];
         changed = true;
      }
      else if( obj->value[1] <= 0 && obj->value[0] > 0 )
      {
         obj->value[1] = obj->value[0];
         changed = true;
      }

      if( changed && counts )
         ++counts->armor_normalized;
   }

   if( normalize_object_affects( obj->first_affect, NULL, false ) )
   {
      changed = true;
      if( counts )
         ++counts->affects_normalized;
   }

   return changed;
}

void normalize_loaded_d20_content( void )
{
   d20_content_counts counts = {};
   CHAR_DATA *ch;
   OBJ_DATA *obj;
   int hash;

   /* Live reset instances still contain the pre-normalization prototype
    * values, so repair them before changing the prototypes themselves. */
   for( ch = first_char; ch; ch = ch->next )
      normalize_live_mobile( ch, &counts );

   for( obj = first_object; obj; obj = obj->next )
      if( normalize_live_object( obj ) )
         ++counts.live_objects_normalized;

   for( hash = 0; hash < MAX_KEY_HASH; ++hash )
   {
      MOB_INDEX_DATA *mob;
      OBJ_INDEX_DATA *object;

      for( mob = mob_index_hash[hash]; mob; mob = mob->next )
      {
         d20_mobile_profile profile = make_mobile_profile( mob );
         ++counts.mobiles_checked;
         if( mobile_profile_changes( mob, profile ) )
         {
            apply_mobile_profile( mob, profile );
            ++counts.mobiles_normalized;
         }
      }

      for( object = obj_index_hash[hash]; object; object = object->next )
      {
         ++counts.objects_checked;
         normalize_object_prototype( object, &counts );
      }
   }

   {
      char logline[MAX_STRING_LENGTH];
      snprintf( logline, sizeof( logline ),
                "D20 content normalization: %d/%d mobiles, %d/%d live mobiles, "
                "%d armor prototypes, %d object affects, %d live objects normalized.",
                counts.mobiles_normalized, counts.mobiles_checked,
                counts.live_mobiles_normalized, counts.mobiles_checked,
                counts.armor_normalized, counts.affects_normalized,
                counts.live_objects_normalized );
      log_string( logline );
   }
}
}

/* Final database boot boundary: load the inherited world first, then make
 * every live area prototype conform to the D20 Foundation contract. */
void boot_db( bool fCopyOver )
{
   legacy_boot_db( fCopyOver );
   normalize_loaded_d20_content();
}
