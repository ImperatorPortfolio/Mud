#ifndef ZEROPOINT_CHARACTER_PROGRESSION_H
#define ZEROPOINT_CHARACTER_PROGRESSION_H


int get_trait_rank_value( int rank );
int get_trait_modifier( CHAR_DATA *ch, int effect );
int get_trait_skill_modifier( CHAR_DATA *ch, int sn );
int get_trait_training_potential( CHAR_DATA *ch, int effect );
int get_trait_training_chance( CHAR_DATA *ch, CHAR_DATA *trainer, int effect );

bool validate_character_traits( CHAR_DATA *ch );

void clear_character_traits( CHAR_DATA *ch );
void generate_character_traits( CHAR_DATA *ch );
void show_character_traits( CHAR_DATA *ch );

int get_ability_modifier( int score );

int get_base_attack_bonus( CHAR_DATA *ch );
int get_defense( CHAR_DATA *ch );

int get_str_tohit_bonus( int strength );
int get_str_todam_bonus( int strength );
int get_str_carry_bonus( int strength );
int get_str_wield_bonus( int strength );
int get_dex_defensive_bonus( int dexterity );
int get_int_learn_bonus( int intelligence );

typedef enum
{
   ABILITY_SCORE_STR,
   ABILITY_SCORE_DEX,
   ABILITY_SCORE_CON,
   ABILITY_SCORE_INT,
   ABILITY_SCORE_WIS,
   ABILITY_SCORE_CHA
} ability_score_type;

typedef enum
{
   SAVE_FORTITUDE,
   SAVE_REFLEX,
   SAVE_WILL
} saving_throw_type;

int get_character_ability_modifier(
   CHAR_DATA *ch,
   int ability );

int get_skill_ability( int sn );
int get_skill_difficulty_class( int sn );

int get_skill_bonus(
   CHAR_DATA *ch,
   int sn,
   int ability );

bool skill_check(
   CHAR_DATA *ch,
   int sn,
   int ability,
   int difficulty );

int get_save_bonus(
   CHAR_DATA *ch,
   int save_type );

bool saving_throw(
   CHAR_DATA *ch,
   int save_type,
   int difficulty,
   int trait_effect_id,
   int misc_bonus );

/*
* Liquids.
*/
#define LIQ_WATER        0
#define LIQ_MAX		19

#endif /* ZEROPOINT_CHARACTER_PROGRESSION_H */
