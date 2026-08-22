#ifndef ZEROPOINT_CHARACTER_PLAYER_H
#define ZEROPOINT_CHARACTER_PLAYER_H


struct killed_data
{
   int vnum;
   char count;
};

/*
* Data which only PC's have.
*/
struct pc_data
{
   CLAN_DATA *clan;
   AREA_DATA *area;
   const char *homepage;
   const char *clan_name;
   const char *pwd;
   const char *email;
   const char *bamfin;
   const char *bamfout;
   const char *rank;
   const char *title;
   const char *bestowments;   /* Special bestowed commands     */
   int flags;  /* Whether the player is deadly and whatever else we add.      */
   int pkills; /* Number of pkills on behalf of clan */
   int pdeaths;   /* Number of times pkilled (legally)  */
   int mkills; /* Number of mobs killed         */
   int mdeaths;   /* Number of deaths due to mobs       */
   int illegal_pk;   /* Number of illegal pk's committed   */
   long int outcast_time;  /* The time at which the char was outcast */
   long int restore_time;  /* The last time the char did a restore all */
   int r_range_lo; /* room range */
   int r_range_hi;
   int m_range_lo; /* mob range  */
   int m_range_hi;
   int o_range_lo; /* obj range  */
   int o_range_hi;
   short wizinvis;   /* wizinvis level */
   short min_snoop;  /* minimum snoop level */
short condition[MAX_CONDS];

   /*
    * Protein, carbohydrates, fats,
    * vitamins, minerals and hydration.
    *
    * Range: 0-100.
    */
short nutrition[MAX_NUTRITION];

/*
 * Progress toward the next permanent D&D ability point.
 * Indexed by ABILITY_SCORE_STR through ABILITY_SCORE_CHA.
 */
int attribute_progress[6];

/*
 * Last time the ability received meaningful use or training.
 */
time_t attribute_last_used[6];

short learned[MAX_SKILL];
   KILLED_DATA killed[MAX_KILLTRACK];
   short quest_number;  /* current *QUEST BEING DONE* DON'T REMOVE! */
   short quest_curr; /* current number of quest points */
   int quest_accum;  /* quest points accumulated in players life */
   PLAYER_QUEST_STATE quests[MAX_ACTIVE_QUESTS];
   PLAYER_QUEST_HISTORY quest_history[MAX_QUEST_HISTORY];
   PLAYER_REPUTATION quest_reputation[MAX_QUEST_REPUTATIONS];
   int auth_state;
   time_t release_date; /* Auto-helling.. Altrag */
   const char *helled_by;
   const char *bio;  /* Personal Bio */
   const char *authed_by;  /* what crazy imm authed this name ;) */
   SKILLTYPE *special_skills[5]; /* personalized skills/spells */
   const char *prompt;  /* User config prompts */
   const char *subprompt;  /* Substate prompt */
   short pagerlen;   /* For pager (NOT menus) */
   bool openedtourney;
   short addiction[10];
   short drug_level[10];
   int wanted_flags;
   long bank;
bool hotboot;  /* hotboot tracker */

   struct character_trait traits[MAX_CHARACTER_TRAITS];
   bool traits_initialized;
};

#endif /* ZEROPOINT_CHARACTER_PLAYER_H */
