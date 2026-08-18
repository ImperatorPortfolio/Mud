#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "mud.h"

struct trait_definition
{
   short id;
   const char *name;
   bool positive;
   short effect;
   short opposite;
};

struct trait_package
{
   short positive[4];
   short negative[2];
};

static const trait_package trait_packages[] =
{
   { { 1, 1, 1, 1 }, { 2, 2 } },
   { { 2, 1, 1, 1 }, { 3, 1 } },
   { { 2, 2, 1, 1 }, { 3, 2 } },
   { { 3, 2, 1, 1 }, { 3, 3 } }
};

static const trait_definition trait_table[] =
{
   { TRAIT_POWERFUL_BUILD, "Powerful Build", TRUE, TRAIT_EFFECT_STR_POTENTIAL, TRAIT_WEAK_BUILD },
   { TRAIT_WEAK_BUILD, "Weak Build", FALSE, TRAIT_EFFECT_STR_POTENTIAL, TRAIT_POWERFUL_BUILD },

   { TRAIT_ROBUST_PHYSIOLOGY, "Robust Physiology", TRUE, TRAIT_EFFECT_CON_POTENTIAL, TRAIT_FRAIL_PHYSIOLOGY },
   { TRAIT_FRAIL_PHYSIOLOGY, "Frail Physiology", FALSE, TRAIT_EFFECT_CON_POTENTIAL, TRAIT_ROBUST_PHYSIOLOGY },

   { TRAIT_QUICK_REFLEXES, "Quick Reflexes", TRUE, TRAIT_EFFECT_DEX_POTENTIAL, TRAIT_SLUGGISH_REFLEXES },
   { TRAIT_SLUGGISH_REFLEXES, "Sluggish Reflexes", FALSE, TRAIT_EFFECT_DEX_POTENTIAL, TRAIT_QUICK_REFLEXES },

   { TRAIT_ANALYTICAL_MIND, "Analytical Mind", TRUE, TRAIT_EFFECT_INT_POTENTIAL, TRAIT_SLOW_MIND },
   { TRAIT_SLOW_MIND, "Slow Mind", FALSE, TRAIT_EFFECT_INT_POTENTIAL, TRAIT_ANALYTICAL_MIND },

   { TRAIT_DISCIPLINED_MIND, "Disciplined Mind", TRUE, TRAIT_EFFECT_WIS_POTENTIAL, TRAIT_WEAK_WILLED },
   { TRAIT_WEAK_WILLED, "Weak Willed", FALSE, TRAIT_EFFECT_WIS_POTENTIAL, TRAIT_DISCIPLINED_MIND },

   { TRAIT_COMMANDING_PRESENCE, "Commanding Presence", TRUE, TRAIT_EFFECT_CHA_POTENTIAL, TRAIT_SOCIALLY_AWKWARD },
   { TRAIT_SOCIALLY_AWKWARD, "Socially Awkward", FALSE, TRAIT_EFFECT_CHA_POTENTIAL, TRAIT_COMMANDING_PRESENCE },

   { TRAIT_FORTUNATE, "Fortunate", TRUE, TRAIT_EFFECT_LCK_POTENTIAL, TRAIT_ILL_FATED },
   { TRAIT_ILL_FATED, "Ill-Fated", FALSE, TRAIT_EFFECT_LCK_POTENTIAL, TRAIT_FORTUNATE },

   { TRAIT_FORCE_ATTUNED, "Force Attuned", TRUE, TRAIT_EFFECT_FRC_POTENTIAL, TRAIT_FORCE_DULL },
   { TRAIT_FORCE_DULL, "Force Dull", FALSE, TRAIT_EFFECT_FRC_POTENTIAL, TRAIT_FORCE_ATTUNED },

   { TRAIT_TOXIN_RESISTANT, "Toxin Resistant", TRUE, TRAIT_EFFECT_SAVE_POISON, TRAIT_TOXIN_SENSITIVE },
   { TRAIT_TOXIN_SENSITIVE, "Toxin Sensitive", FALSE, TRAIT_EFFECT_SAVE_POISON, TRAIT_TOXIN_RESISTANT },

   { TRAIT_STEADY_NERVOUS_SYSTEM, "Steady Nervous System", TRUE, TRAIT_EFFECT_SAVE_PARA, TRAIT_PARALYSIS_PRONE },
   { TRAIT_PARALYSIS_PRONE, "Paralysis Prone", FALSE, TRAIT_EFFECT_SAVE_PARA, TRAIT_STEADY_NERVOUS_SYSTEM },

   { TRAIT_STRONG_LUNGS, "Strong Lungs", TRUE, TRAIT_EFFECT_SAVE_BREATH, TRAIT_WEAK_LUNGS },
   { TRAIT_WEAK_LUNGS, "Weak Lungs", FALSE, TRAIT_EFFECT_SAVE_BREATH, TRAIT_STRONG_LUNGS },

   { TRAIT_DEVICE_HARDENED, "Device Hardened", TRUE, TRAIT_EFFECT_SAVE_WAND, TRAIT_DEVICE_SUSCEPTIBLE },
   { TRAIT_DEVICE_SUSCEPTIBLE, "Device Susceptible", FALSE, TRAIT_EFFECT_SAVE_WAND, TRAIT_DEVICE_HARDENED },

   { TRAIT_FORCE_RESISTANT, "Force Resistant", TRUE, TRAIT_EFFECT_SAVE_FORCE, TRAIT_FORCE_SUSCEPTIBLE },
   { TRAIT_FORCE_SUSCEPTIBLE, "Force Susceptible", FALSE, TRAIT_EFFECT_SAVE_FORCE, TRAIT_FORCE_RESISTANT },

   { TRAIT_NATURAL_MARKSMAN, "Natural Marksman", TRUE, TRAIT_EFFECT_SKILL_BLASTERS, TRAIT_POOR_SHOT },
   { TRAIT_POOR_SHOT, "Poor Shot", FALSE, TRAIT_EFFECT_SKILL_BLASTERS, TRAIT_NATURAL_MARKSMAN },

   { TRAIT_BOWCASTER_SAVANT, "Bowcaster Savant", TRUE, TRAIT_EFFECT_SKILL_BOWCASTERS, TRAIT_BOWCASTER_INEPT },
   { TRAIT_BOWCASTER_INEPT, "Bowcaster Inept", FALSE, TRAIT_EFFECT_SKILL_BOWCASTERS, TRAIT_BOWCASTER_SAVANT },

   { TRAIT_PIKE_FIGHTER, "Pike Fighter", TRUE, TRAIT_EFFECT_SKILL_FORCE_PIKES, TRAIT_PIKE_CLUMSY },
   { TRAIT_PIKE_CLUMSY, "Pike Clumsy", FALSE, TRAIT_EFFECT_SKILL_FORCE_PIKES, TRAIT_PIKE_FIGHTER },

   { TRAIT_SABER_INSTINCT, "Saber Instinct", TRUE, TRAIT_EFFECT_SKILL_LIGHTSABERS, TRAIT_SABER_HESITATION },
   { TRAIT_SABER_HESITATION, "Saber Hesitation", FALSE, TRAIT_EFFECT_SKILL_LIGHTSABERS, TRAIT_SABER_INSTINCT },

   { TRAIT_VIBROBLADE_HAND, "Vibroblade Hand", TRUE, TRAIT_EFFECT_SKILL_VIBRO_BLADES, TRAIT_POOR_BLADE_CONTROL },
   { TRAIT_POOR_BLADE_CONTROL, "Poor Blade Control", FALSE, TRAIT_EFFECT_SKILL_VIBRO_BLADES, TRAIT_VIBROBLADE_HAND },

   { TRAIT_EVASIVE_INSTINCT, "Evasive Instinct", TRUE, TRAIT_EFFECT_SKILL_DODGE, TRAIT_FLAT_FOOTED },
   { TRAIT_FLAT_FOOTED, "Flat-Footed", FALSE, TRAIT_EFFECT_SKILL_DODGE, TRAIT_EVASIVE_INSTINCT },

   { TRAIT_DEFENSIVE_GUARD, "Defensive Guard", TRUE, TRAIT_EFFECT_SKILL_PARRY, TRAIT_OPEN_GUARD },
   { TRAIT_OPEN_GUARD, "Open Guard", FALSE, TRAIT_EFFECT_SKILL_PARRY, TRAIT_DEFENSIVE_GUARD },

   { TRAIT_DISARMING_HAND, "Disarming Hand", TRUE, TRAIT_EFFECT_SKILL_DISARM, TRAIT_WEAK_WEAPON_CONTROL },
   { TRAIT_WEAK_WEAPON_CONTROL, "Weak Weapon Control", FALSE, TRAIT_EFFECT_SKILL_DISARM, TRAIT_DISARMING_HAND },

   { TRAIT_AMBIDEXTROUS, "Ambidextrous", TRUE, TRAIT_EFFECT_SKILL_DUAL_WIELD, TRAIT_OFFHAND_INEPT },
   { TRAIT_OFFHAND_INEPT, "Off-Hand Inept", FALSE, TRAIT_EFFECT_SKILL_DUAL_WIELD, TRAIT_AMBIDEXTROUS },

   { TRAIT_HEAVY_HITTER, "Heavy Hitter", TRUE, TRAIT_EFFECT_SKILL_ENHANCED_DAMAGE, TRAIT_WEAK_STRIKER },
   { TRAIT_WEAK_STRIKER, "Weak Striker", FALSE, TRAIT_EFFECT_SKILL_ENHANCED_DAMAGE, TRAIT_HEAVY_HITTER },

   { TRAIT_SNIPERS_PATIENCE, "Sniper's Patience", TRUE, TRAIT_EFFECT_SKILL_SNIPE, TRAIT_IMPATIENT_SHOOTER },
   { TRAIT_IMPATIENT_SHOOTER, "Impatient Shooter", FALSE, TRAIT_EFFECT_SKILL_SNIPE, TRAIT_SNIPERS_PATIENCE },

   { TRAIT_THROWING_ARM, "Throwing Arm", TRUE, TRAIT_EFFECT_SKILL_THROW, TRAIT_POOR_THROWER },
   { TRAIT_POOR_THROWER, "Poor Thrower", FALSE, TRAIT_EFFECT_SKILL_THROW, TRAIT_THROWING_ARM },

   { TRAIT_GRENADIER, "Grenadier", TRUE, TRAIT_EFFECT_SKILL_GRENADES, TRAIT_EXPLOSIVES_NERVOUSNESS },
   { TRAIT_EXPLOSIVES_NERVOUSNESS, "Explosives Nervousness", FALSE, TRAIT_EFFECT_SKILL_GRENADES, TRAIT_GRENADIER },

   { TRAIT_FIELD_MEDIC, "Field Medic", TRUE, TRAIT_EFFECT_SKILL_FIRST_AID, TRAIT_SQUEAMISH },
   { TRAIT_SQUEAMISH, "Squeamish", FALSE, TRAIT_EFFECT_SKILL_FIRST_AID, TRAIT_FIELD_MEDIC },

   { TRAIT_NATURAL_TRACKER, "Natural Tracker", TRUE, TRAIT_EFFECT_SKILL_TRACK, TRAIT_POOR_TRACKER },
   { TRAIT_POOR_TRACKER, "Poor Tracker", FALSE, TRAIT_EFFECT_SKILL_TRACK, TRAIT_NATURAL_TRACKER },

   { TRAIT_KEEN_SEARCHER, "Keen Searcher", TRUE, TRAIT_EFFECT_SKILL_SEARCH, TRAIT_INATTENTIVE },
   { TRAIT_INATTENTIVE, "Inattentive", FALSE, TRAIT_EFFECT_SKILL_SEARCH, TRAIT_KEEN_SEARCHER },

   { TRAIT_QUIET_STEP, "Quiet Step", TRUE, TRAIT_EFFECT_SKILL_SNEAK, TRAIT_HEAVY_FOOTED },
   { TRAIT_HEAVY_FOOTED, "Heavy Footed", FALSE, TRAIT_EFFECT_SKILL_SNEAK, TRAIT_QUIET_STEP },

   { TRAIT_CONCEALMENT_INSTINCT, "Concealment Instinct", TRUE, TRAIT_EFFECT_SKILL_HIDE, TRAIT_CONSPICUOUS },
   { TRAIT_CONSPICUOUS, "Conspicuous", FALSE, TRAIT_EFFECT_SKILL_HIDE, TRAIT_CONCEALMENT_INSTINCT },

   { TRAIT_LOCK_SENSE, "Lock Sense", TRUE, TRAIT_EFFECT_SKILL_PICK_LOCK, TRAIT_CLUMSY_LOCKSMITH },
   { TRAIT_CLUMSY_LOCKSMITH, "Clumsy Locksmith", FALSE, TRAIT_EFFECT_SKILL_PICK_LOCK, TRAIT_LOCK_SENSE },

   { TRAIT_SHIPLOCK_SPECIALIST, "Shiplock Specialist", TRUE, TRAIT_EFFECT_SKILL_PICKSHIPLOCK, TRAIT_SHIPLOCK_INEPT },
   { TRAIT_SHIPLOCK_INEPT, "Shiplock Inept", FALSE, TRAIT_EFFECT_SKILL_PICKSHIPLOCK, TRAIT_SHIPLOCK_SPECIALIST },

   { TRAIT_HIJACKERS_NERVE, "Hijacker's Nerve", TRUE, TRAIT_EFFECT_SKILL_HIJACK, TRAIT_BOARDING_HESITATION },
   { TRAIT_BOARDING_HESITATION, "Boarding Hesitation", FALSE, TRAIT_EFFECT_SKILL_HIJACK, TRAIT_HIJACKERS_NERVE },

   { TRAIT_FIGHTER_PILOT, "Fighter Pilot", TRUE, TRAIT_EFFECT_SKILL_STARFIGHTERS, TRAIT_FIGHTER_ANXIETY },
   { TRAIT_FIGHTER_ANXIETY, "Fighter Anxiety", FALSE, TRAIT_EFFECT_SKILL_STARFIGHTERS, TRAIT_FIGHTER_PILOT },

   { TRAIT_FREIGHTER_HAND, "Freighter Hand", TRUE, TRAIT_EFFECT_SKILL_MIDSHIPS, TRAIT_POOR_MIDSHIP_HANDLING },
   { TRAIT_POOR_MIDSHIP_HANDLING, "Poor Midship Handling", FALSE, TRAIT_EFFECT_SKILL_MIDSHIPS, TRAIT_FREIGHTER_HAND },

   { TRAIT_CAPITAL_COMMAND, "Capital Command", TRUE, TRAIT_EFFECT_SKILL_CAPITALSHIPS, TRAIT_CAPITALSHIP_OVERLOAD },
   { TRAIT_CAPITALSHIP_OVERLOAD, "Capitalship Overload", FALSE, TRAIT_EFFECT_SKILL_CAPITALSHIPS, TRAIT_CAPITAL_COMMAND },

   { TRAIT_ASTROGATION_INSTINCT, "Astrogation Instinct", TRUE, TRAIT_EFFECT_SKILL_NAVIGATION, TRAIT_DIRECTIONALLY_CHALLENGED },
   { TRAIT_DIRECTIONALLY_CHALLENGED, "Directionally Challenged", FALSE, TRAIT_EFFECT_SKILL_NAVIGATION, TRAIT_ASTROGATION_INSTINCT },

   { TRAIT_GUNNERY_INSTINCT, "Gunnery Instinct", TRUE, TRAIT_EFFECT_SKILL_WEAPONSYSTEMS, TRAIT_POOR_GUNNER },
   { TRAIT_POOR_GUNNER, "Poor Gunner", FALSE, TRAIT_EFFECT_SKILL_WEAPONSYSTEMS, TRAIT_GUNNERY_INSTINCT },

   { TRAIT_SYSTEMS_APTITUDE, "Systems Aptitude", TRUE, TRAIT_EFFECT_SKILL_SHIPSYSTEMS, TRAIT_SYSTEMS_CONFUSION },
   { TRAIT_SYSTEMS_CONFUSION, "Systems Confusion", FALSE, TRAIT_EFFECT_SKILL_SHIPSYSTEMS, TRAIT_SYSTEMS_APTITUDE },

   { TRAIT_TRACTOR_PRECISION, "Tractor Precision", TRUE, TRAIT_EFFECT_SKILL_TRACTORBEAMS, TRAIT_POOR_TRACTOR_CONTROL },
   { TRAIT_POOR_TRACTOR_CONTROL, "Poor Tractor Control", FALSE, TRAIT_EFFECT_SKILL_TRACTORBEAMS, TRAIT_TRACTOR_PRECISION },

   { TRAIT_SHIP_MECHANIC, "Ship Mechanic", TRUE, TRAIT_EFFECT_SKILL_SHIPMAINTENANCE, TRAIT_MECHANICAL_INEPTITUDE },
   { TRAIT_MECHANICAL_INEPTITUDE, "Mechanical Ineptitude", FALSE, TRAIT_EFFECT_SKILL_SHIPMAINTENANCE, TRAIT_SHIP_MECHANIC },

   { TRAIT_ARMOUR_SMITH, "Armour Smith", TRUE, TRAIT_EFFECT_SKILL_MAKEARMOR, TRAIT_POOR_ARMOURER },
   { TRAIT_POOR_ARMOURER, "Poor Armourer", FALSE, TRAIT_EFFECT_SKILL_MAKEARMOR, TRAIT_ARMOUR_SMITH },

   { TRAIT_BLASTER_SMITH, "Blaster Smith", TRUE, TRAIT_EFFECT_SKILL_MAKEBLASTER, TRAIT_POOR_GUNSMITH },
   { TRAIT_POOR_GUNSMITH, "Poor Gunsmith", FALSE, TRAIT_EFFECT_SKILL_MAKEBLASTER, TRAIT_BLASTER_SMITH },

   { TRAIT_SHIELD_ENGINEER, "Shield Engineer", TRUE, TRAIT_EFFECT_SKILL_MAKESHIELD, TRAIT_POOR_SHIELD_ENGINEER },
   { TRAIT_POOR_SHIELD_ENGINEER, "Poor Shield Engineer", FALSE, TRAIT_EFFECT_SKILL_MAKESHIELD, TRAIT_SHIELD_ENGINEER },

   { TRAIT_SABER_CRAFTSMAN, "Saber Craftsman", TRUE, TRAIT_EFFECT_SKILL_LIGHTSABER_CRAFTING, TRAIT_POOR_SABER_CRAFTSMAN },
   { TRAIT_POOR_SABER_CRAFTSMAN, "Poor Saber Craftsman", FALSE, TRAIT_EFFECT_SKILL_LIGHTSABER_CRAFTING, TRAIT_SABER_CRAFTSMAN },

   { TRAIT_INTELLIGENCE_OPERATIVE, "Intelligence Operative", TRUE, TRAIT_EFFECT_SKILL_GATHER_INTELLIGENCE, TRAIT_POOR_ANALYST },
   { TRAIT_POOR_ANALYST, "Poor Analyst", FALSE, TRAIT_EFFECT_SKILL_GATHER_INTELLIGENCE, TRAIT_INTELLIGENCE_OPERATIVE },

   { TRAIT_SMOOTH_TALKER, "Smooth Talker", TRUE, TRAIT_EFFECT_SKILL_SMALLTALK, TRAIT_POOR_CONVERSATIONALIST },
   { TRAIT_POOR_CONVERSATIONALIST, "Poor Conversationalist", FALSE, TRAIT_EFFECT_SKILL_SMALLTALK, TRAIT_SMOOTH_TALKER }
};

static const int trait_table_count =
   sizeof( trait_table ) / sizeof( trait_table[0] );

static const trait_definition *get_trait_definition( int id )
{
   int i;

   for( i = 0; i < trait_table_count; ++i )
   {
      if( trait_table[i].id == id )
         return &trait_table[i];
   }

   return NULL;
}

static const char *get_trait_rank_name( int rank )
{
   switch( rank )
   {
      case 1:
         return "I";

      case 2:
         return "II";

      case 3:
         return "III";
   }

   return "?";
}

static const char *get_trait_effect_name( int effect )
{
   switch( effect )
   {
      case TRAIT_EFFECT_STR_POTENTIAL:
         return "STR potential";

      case TRAIT_EFFECT_CON_POTENTIAL:
         return "CON potential";

      case TRAIT_EFFECT_DEX_POTENTIAL:
         return "DEX potential";

      case TRAIT_EFFECT_INT_POTENTIAL:
         return "INT potential";

      case TRAIT_EFFECT_WIS_POTENTIAL:
         return "WIS potential";

      case TRAIT_EFFECT_CHA_POTENTIAL:
         return "CHA potential";

      case TRAIT_EFFECT_LCK_POTENTIAL:
         return "Luck";

      case TRAIT_EFFECT_FRC_POTENTIAL:
         return "Force potential";

      case TRAIT_EFFECT_SAVE_POISON:
         return "Poison/death save";

      case TRAIT_EFFECT_SAVE_PARA:
         return "Paralysis save";

      case TRAIT_EFFECT_SAVE_BREATH:
         return "Breath save";

      case TRAIT_EFFECT_SAVE_WAND:
         return "Device/wand save";

      case TRAIT_EFFECT_SAVE_FORCE:
         return "Force/spell save";

      case TRAIT_EFFECT_SKILL_BLASTERS:
         return "Blasters aptitude";

      case TRAIT_EFFECT_SKILL_BOWCASTERS:
         return "Bowcasters aptitude";

      case TRAIT_EFFECT_SKILL_FORCE_PIKES:
         return "Force pikes aptitude";

      case TRAIT_EFFECT_SKILL_LIGHTSABERS:
         return "Lightsabers aptitude";

      case TRAIT_EFFECT_SKILL_VIBRO_BLADES:
         return "Vibro blades aptitude";

      case TRAIT_EFFECT_SKILL_SHIELDWORK:
         return "Shieldwork aptitude";

      case TRAIT_EFFECT_SKILL_DODGE:
         return "Dodge aptitude";

      case TRAIT_EFFECT_SKILL_PARRY:
         return "Parry aptitude";

      case TRAIT_EFFECT_SKILL_DISARM:
         return "Disarm aptitude";

      case TRAIT_EFFECT_SKILL_DUAL_WIELD:
         return "Dual wield aptitude";

      case TRAIT_EFFECT_SKILL_ENHANCED_DAMAGE:
         return "Enhanced damage aptitude";

      case TRAIT_EFFECT_SKILL_SNIPE:
         return "Snipe aptitude";

      case TRAIT_EFFECT_SKILL_THROW:
         return "Throw aptitude";

      case TRAIT_EFFECT_SKILL_GRENADES:
         return "Grenades aptitude";

      case TRAIT_EFFECT_SKILL_FIRST_AID:
         return "First aid aptitude";

      case TRAIT_EFFECT_SKILL_TRACK:
         return "Track aptitude";

      case TRAIT_EFFECT_SKILL_SEARCH:
         return "Search aptitude";

      case TRAIT_EFFECT_SKILL_SNEAK:
         return "Sneak aptitude";

      case TRAIT_EFFECT_SKILL_HIDE:
         return "Hide aptitude";

      case TRAIT_EFFECT_SKILL_PICK_LOCK:
         return "Pick lock aptitude";

      case TRAIT_EFFECT_SKILL_PICKSHIPLOCK:
         return "Pick ship lock aptitude";

      case TRAIT_EFFECT_SKILL_HIJACK:
         return "Hijack aptitude";

      case TRAIT_EFFECT_SKILL_SLICE:
         return "Slice aptitude";

      case TRAIT_EFFECT_SKILL_STARFIGHTERS:
         return "Starfighters aptitude";

      case TRAIT_EFFECT_SKILL_MIDSHIPS:
         return "Midships aptitude";

      case TRAIT_EFFECT_SKILL_CAPITALSHIPS:
         return "Capitalships aptitude";

      case TRAIT_EFFECT_SKILL_NAVIGATION:
         return "Navigation aptitude";

      case TRAIT_EFFECT_SKILL_WEAPONSYSTEMS:
         return "Weapon systems aptitude";

      case TRAIT_EFFECT_SKILL_SHIPSYSTEMS:
         return "Ship systems aptitude";

      case TRAIT_EFFECT_SKILL_TRACTORBEAMS:
         return "Tractor beams aptitude";

      case TRAIT_EFFECT_SKILL_SHIPMAINTENANCE:
         return "Ship maintenance aptitude";

      case TRAIT_EFFECT_SKILL_MAKEARMOR:
         return "Make armor aptitude";

      case TRAIT_EFFECT_SKILL_MAKEBLASTER:
         return "Make blaster aptitude";

      case TRAIT_EFFECT_SKILL_MAKESHIELD:
         return "Make shield aptitude";

      case TRAIT_EFFECT_SKILL_LIGHTSABER_CRAFTING:
         return "Lightsaber crafting aptitude";

      case TRAIT_EFFECT_SKILL_GATHER_INTELLIGENCE:
         return "Gather intelligence aptitude";

      case TRAIT_EFFECT_SKILL_SMALLTALK:
         return "Smalltalk aptitude";

      default:
         return "Unknown";
   }
}

int get_trait_rank_value( int rank )
{
   switch( rank )
   {
      case 1:
         return 1;

      case 2:
         return 2;

      case 3:
         return 4;
   }

   return 0;
}

void clear_character_traits( CHAR_DATA *ch )
{
   int i;

   if( !ch || !ch->pcdata )
      return;

   for( i = 0; i < MAX_CHARACTER_TRAITS; ++i )
   {
      ch->pcdata->traits[i].id = TRAIT_NONE;
      ch->pcdata->traits[i].rank = 0;
   }

   ch->pcdata->traits_initialized = FALSE;
}

static bool trait_is_selected( CHAR_DATA *ch, int id )
{
   int i;

   for( i = 0; i < MAX_CHARACTER_TRAITS; ++i )
   {
      if( ch->pcdata->traits[i].id == id )
         return TRUE;
   }

   return FALSE;
}

static bool trait_conflicts_with_selected( CHAR_DATA *ch, int id )
{
   int i;
   const trait_definition *candidate;

   candidate = get_trait_definition( id );

   if( !candidate )
      return TRUE;

   for( i = 0; i < MAX_CHARACTER_TRAITS; ++i )
   {
      const trait_definition *selected;

      if( ch->pcdata->traits[i].id == TRAIT_NONE )
         continue;

      selected = get_trait_definition( ch->pcdata->traits[i].id );

      if( !selected )
         continue;

      if( candidate->opposite == selected->id
          || selected->opposite == candidate->id )
         return TRUE;
   }

   return FALSE;
}

static int pick_random_trait( CHAR_DATA *ch, bool positive )
{
   short candidates[50];
   int count;
   int i;

   count = 0;

   for( i = 0; i < trait_table_count; ++i )
   {
      const trait_definition *trait;

      trait = &trait_table[i];

      if( trait->positive != positive )
         continue;

      if( trait_is_selected( ch, trait->id ) )
         continue;

      if( trait_conflicts_with_selected( ch, trait->id ) )
         continue;

      candidates[count++] = trait->id;
   }

   if( count <= 0 )
      return TRAIT_NONE;

   return candidates[number_range( 0, count - 1 )];
}

bool validate_character_traits( CHAR_DATA *ch )
{
   int positive_count;
   int negative_count;
   int positive_points;
   int negative_points;
   int i;
   int j;

   if( !ch || !ch->pcdata )
      return FALSE;

   positive_count = 0;
   negative_count = 0;
   positive_points = 0;
   negative_points = 0;

   for( i = 0; i < MAX_CHARACTER_TRAITS; ++i )
   {
      const trait_definition *trait;
      int rank_value;

      trait = get_trait_definition( ch->pcdata->traits[i].id );

      if( !trait )
         return FALSE;

      rank_value = get_trait_rank_value( ch->pcdata->traits[i].rank );

      if( rank_value <= 0 )
         return FALSE;

      for( j = i + 1; j < MAX_CHARACTER_TRAITS; ++j )
      {
         const trait_definition *other;

         if( ch->pcdata->traits[i].id == ch->pcdata->traits[j].id )
            return FALSE;

         other = get_trait_definition( ch->pcdata->traits[j].id );

         if( other
             && ( trait->opposite == other->id
                  || other->opposite == trait->id ) )
            return FALSE;
      }

      if( trait->positive )
      {
         ++positive_count;
         positive_points += rank_value;
      }
      else
      {
         ++negative_count;
         negative_points += rank_value;
      }
   }

   if( positive_count != 4 )
      return FALSE;

   if( negative_count != 2 )
      return FALSE;

   if( positive_points != negative_points )
      return FALSE;

   return TRUE;
}

void generate_character_traits( CHAR_DATA *ch )
{
   int attempt;

   if( !ch || !ch->pcdata || IS_NPC( ch ) )
      return;

   for( attempt = 0; attempt < 100; ++attempt )
   {
      const trait_package *package;
      int package_index;
      int i;
      bool failed;

      clear_character_traits( ch );

      package_index =
         number_range( 0,
            ( sizeof( trait_packages ) / sizeof( trait_packages[0] ) ) - 1 );

      package = &trait_packages[package_index];
      failed = FALSE;

      for( i = 0; i < 4; ++i )
      {
         int id;

         id = pick_random_trait( ch, TRUE );

         if( id == TRAIT_NONE )
         {
            failed = TRUE;
            break;
         }

         ch->pcdata->traits[i].id = id;
         ch->pcdata->traits[i].rank = package->positive[i];
      }

      if( failed )
         continue;

      for( i = 0; i < 2; ++i )
      {
         int id;

         id = pick_random_trait( ch, FALSE );

         if( id == TRAIT_NONE )
         {
            failed = TRUE;
            break;
         }

         ch->pcdata->traits[4 + i].id = id;
         ch->pcdata->traits[4 + i].rank = package->negative[i];
      }

      if( failed )
         continue;

      if( validate_character_traits( ch ) )
      {
         ch->pcdata->traits_initialized = TRUE;
         return;
      }
   }

   clear_character_traits( ch );
   bug( "%s: unable to generate a valid balanced trait set for %s.",
        __func__,
        ch->name ? ch->name : "(unnamed)" );
}

int get_trait_modifier( CHAR_DATA *ch, int effect )
{
   int total;
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return 0;

   total = 0;

   for( i = 0; i < MAX_CHARACTER_TRAITS; ++i )
   {
      const trait_definition *trait;
      int value;

      trait = get_trait_definition( ch->pcdata->traits[i].id );

      if( !trait || trait->effect != effect )
         continue;

      value = get_trait_rank_value( ch->pcdata->traits[i].rank );

      if( trait->positive )
         total += value;
      else
         total -= value;
   }

   return total;
}

int get_trait_skill_modifier( CHAR_DATA *ch, int sn )
{
   if( !ch || IS_NPC( ch ) || !ch->pcdata || sn < 0 )
      return 0;

   if( sn == gsn_blasters )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_BLASTERS );

   if( sn == gsn_bowcasters )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_BOWCASTERS );

   if( sn == gsn_force_pikes )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_FORCE_PIKES );

   if( sn == gsn_lightsabers )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_LIGHTSABERS );

   if( sn == gsn_vibro_blades )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_VIBRO_BLADES );

   if( sn == gsn_dodge )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_DODGE );

   if( sn == gsn_parry )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_PARRY );

   if( sn == gsn_disarm )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_DISARM );

   if( sn == gsn_dual_wield )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_DUAL_WIELD );

   if( sn == gsn_enhanced_damage )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_ENHANCED_DAMAGE );

   if( sn == gsn_snipe )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_SNIPE );

   if( sn == gsn_throw )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_THROW );

   if( sn == gsn_grenades )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_GRENADES );

   if( sn == gsn_first_aid )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_FIRST_AID );

   if( sn == gsn_track )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_TRACK );

   if( sn == gsn_search )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_SEARCH );

   if( sn == gsn_sneak )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_SNEAK );

   if( sn == gsn_hide )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_HIDE );

   if( sn == gsn_pick_lock )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_PICK_LOCK );

   if( sn == gsn_pickshiplock )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_PICKSHIPLOCK );

   if( sn == gsn_hijack )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_HIJACK );

   if( sn == gsn_starfighters )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_STARFIGHTERS );

   if( sn == gsn_midships )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_MIDSHIPS );

   if( sn == gsn_capitalships )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_CAPITALSHIPS );

   if( sn == gsn_navigation )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_NAVIGATION );

   if( sn == gsn_weaponsystems )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_WEAPONSYSTEMS );

   if( sn == gsn_shipsystems )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_SHIPSYSTEMS );

   if( sn == gsn_tractorbeams )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_TRACTORBEAMS );

   if( sn == gsn_shipmaintenance )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_SHIPMAINTENANCE );

   if( sn == gsn_makearmor )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_MAKEARMOR );

   if( sn == gsn_makeblaster )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_MAKEBLASTER );

   if( sn == gsn_makeshield )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_MAKESHIELD );

   if( sn == gsn_lightsaber_crafting )
      return get_trait_modifier(
         ch,
         TRAIT_EFFECT_SKILL_LIGHTSABER_CRAFTING );

   if( sn == gsn_gather_intelligence )
      return get_trait_modifier(
         ch,
         TRAIT_EFFECT_SKILL_GATHER_INTELLIGENCE );

   if( sn == gsn_smalltalk )
      return get_trait_modifier( ch, TRAIT_EFFECT_SKILL_SMALLTALK );

   return 0;
}

int get_trait_training_potential( CHAR_DATA *ch, int effect )
{
   int racial_bonus;

   if( !ch )
      return 20;

   racial_bonus = 0;

   switch( effect )
   {
      case TRAIT_EFFECT_STR_POTENTIAL:
         racial_bonus = race_table[ch->race].str_plus;
         break;

      case TRAIT_EFFECT_CON_POTENTIAL:
         racial_bonus = race_table[ch->race].con_plus;
         break;

      case TRAIT_EFFECT_DEX_POTENTIAL:
         racial_bonus = race_table[ch->race].dex_plus;
         break;

      case TRAIT_EFFECT_INT_POTENTIAL:
         racial_bonus = race_table[ch->race].int_plus;
         break;

      case TRAIT_EFFECT_WIS_POTENTIAL:
         racial_bonus = race_table[ch->race].wis_plus;
         break;

      case TRAIT_EFFECT_CHA_POTENTIAL:
         racial_bonus = race_table[ch->race].cha_plus;
         break;

      case TRAIT_EFFECT_LCK_POTENTIAL:
         racial_bonus = race_table[ch->race].lck_plus;
         break;

      case TRAIT_EFFECT_FRC_POTENTIAL:
         racial_bonus = race_table[ch->race].frc_plus;
         break;

      default:
         return 20;
   }

   return 20
      + racial_bonus
      + get_trait_modifier( ch, effect );
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

   trainer_adjust =
      URANGE( -20, ( trainer_stat - current ) * 3, 20 );

   chance_value =
      70
      - ( 12 * ( current - potential ) )
      + trainer_adjust;

   return URANGE( 1, chance_value, 95 );
}

void show_character_traits( CHAR_DATA *ch )
{
   int i;

   if( !ch || IS_NPC( ch ) || !ch->pcdata )
      return;

   send_to_char( "\r\n&CPersonal Traits&w\r\n", ch );
   send_to_char( "------------------------------------------------------------\r\n", ch );

   if( !ch->pcdata->traits_initialized )
   {
      send_to_char( "No personal traits recorded.\r\n", ch );
      return;
   }

   for( i = 0; i < MAX_CHARACTER_TRAITS; ++i )
   {
      const trait_definition *trait;
      int value;

      trait = get_trait_definition( ch->pcdata->traits[i].id );

      if( !trait )
         continue;

      value = get_trait_rank_value( ch->pcdata->traits[i].rank );

      if( !trait->positive )
         value = -value;

      ch_printf(
         ch,
         "%c %-28s %-3s  %+d %s\r\n",
         trait->positive ? '+' : '-',
         trait->name,
         get_trait_rank_name( ch->pcdata->traits[i].rank ),
         value,
         get_trait_effect_name( trait->effect ) );
   }

   send_to_char( "------------------------------------------------------------\r\n", ch );
}

void do_traits( CHAR_DATA *ch, const char *argument )
{
   if( IS_NPC( ch ) || !ch->pcdata )
      return;

   show_character_traits( ch );
}