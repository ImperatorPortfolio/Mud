#ifndef ZEROPOINT_CHARACTER_CHARACTER_H
#define ZEROPOINT_CHARACTER_CHARACTER_H

struct mob_index_data
{
   MOB_INDEX_DATA *next;
   MOB_INDEX_DATA *next_sort;
   SPEC_FUN *spec_fun;
   SPEC_FUN *spec_2;
   SHOP_DATA *pShop;
   REPAIR_DATA *rShop;
   MPROG_DATA *mudprogs;
   int progtypes;
   const char *player_name;
   const char *short_descr;
   const char *long_descr;
   const char *description;
   const char *spec_funname;
   const char *spec_funname2;
   int vnum;
   short count;
   short killed;
   short sex;
   short level;
   int act;
   int affected_by;
   short alignment;
   short mobthac0;   /* Unused */
   short ac;
   short hitnodice;
   short hitsizedice;
   short hitplus;
   short damnodice;
   short damsizedice;
   short damplus;
   short numattacks;
   int gold;
   int exp;
   int xflags;
   int resistant;
   int immune;
   int susceptible;
   int attacks;
   int defenses;
   int speaks;
   int speaking;
   short position;
   short defposition;
   short height;
   short weight;
   short race;
   short hitroll;
   short damroll;
   short perm_str;
   short perm_int;
   short perm_wis;
   short perm_dex;
   short perm_con;
   short perm_cha;
   short perm_lck;
   short perm_frc;
   short saving_poison_death;
   short saving_wand;
   short saving_para_petri;
   short saving_breath;
   short saving_spell_staff;
   int vip_flags;
};

struct hunt_hate_fear
{
   const char *name;
   CHAR_DATA *who;
};

struct fighting_data
{
   CHAR_DATA *who;
   int xp;
   short align;
   short duration;
   short timeskilled;
};

struct editor_data
{
   short numlines;
   short on_line;
   short size;
   char line[49][81];
};

struct extracted_char_data
{
   EXTRACT_CHAR_DATA *next;
   CHAR_DATA *ch;
   ROOM_INDEX_DATA *room;
   ch_ret retcode;
   bool extract;
};

/*
* One character (PC or NPC).
* (Shouldn't most of that build interface stuff use substate, dest_buf,
* spare_ptr and tempnum?  Seems a little redundant)
*/
struct char_data
{
   CHAR_DATA *next;
   CHAR_DATA *prev;
   CHAR_DATA *next_in_room;
   CHAR_DATA *prev_in_room;
   CHAR_DATA *master;
   CHAR_DATA *leader;
   FIGHT_DATA *fighting;
   CHAR_DATA *reply;
   CHAR_DATA *switched;
   CHAR_DATA *mount;
   HHF_DATA *hunting;
   HHF_DATA *fearing;
   HHF_DATA *hating;
   SPEC_FUN *spec_fun;
   SPEC_FUN *spec_2;
   const char *spec_funname;
   const char *spec_funname2;
   MPROG_ACT_LIST *mpact;
   int mpactnum;
   unsigned short mpscriptpos;
   MOB_INDEX_DATA *pIndexData;
   DESCRIPTOR_DATA *desc;
   AFFECT_DATA *first_affect;
   AFFECT_DATA *last_affect;
   NOTE_DATA *pnote;
   NOTE_DATA *comments;
   OBJ_DATA *first_carrying;
   OBJ_DATA *last_carrying;
   ROOM_INDEX_DATA *in_room;
   ROOM_INDEX_DATA *was_in_room;
   ROOM_INDEX_DATA *was_sentinel;
   ROOM_INDEX_DATA *plr_home;
   PC_DATA *pcdata;
   DO_FUN *last_cmd;
   DO_FUN *prev_cmd; /* mapping */
   void *dest_buf;
   void *dest_buf_2;
   void *spare_ptr;
   int tempnum;
   EDITOR_DATA *editor;
   TIMER *first_timer;
   TIMER *last_timer;
   const char *name;
   const char *short_descr;
   const char *long_descr;
   const char *description;
   short num_fighting;
   short substate;
   short sex;
   short race;
   short top_level;
   short skill_level[MAX_ABILITY];
   short trust;
   int played;
   time_t logon;
   time_t save_time;
   short timer;
   short wait;
   short hit;
   short max_hit;
   short mana;
   short max_mana;
   short move;
   short max_move;
   short numattacks;
   int gold;
   long experience[MAX_ABILITY];
   int act;
   int affected_by;
   int carry_weight;
   int carry_number;
   int xflags;
   int resistant;
   int immune;
   int susceptible;
   int attacks;
   int defenses;
   int speaks;
   int speaking;
   short saving_poison_death;
   short saving_wand;
   short saving_para_petri;
   short saving_breath;
   short saving_spell_staff;
   short alignment;
   short barenumdie;
   short baresizedie;
   short mobthac0;
   short hitroll;
   short damroll;
   short hitplus;
   short damplus;
   short position;
   short defposition;
   short height;
   short weight;
   short armor;
   short wimpy;
   int deaf;
   short perm_str;
   short perm_int;
   short perm_wis;
   short perm_dex;
   short perm_con;
   short perm_cha;
   short perm_lck;
   short perm_frc;
   short mod_str;
   short mod_int;
   short mod_wis;
   short mod_dex;
   short mod_con;
   short mod_cha;
   short mod_lck;
   short mod_frc;
   short mental_state;  /* simplified */
   short emotional_state;  /* simplified */
   int retran;
   int regoto;
   short mobinvis;   /* Mobinvis level SB */
   int vip_flags;
   short backup_wait;   /* reinforcements */
   int backup_mob;   /* reinforcements */
   short was_stunned;
   const char *mob_clan;   /* for spec_clan_guard.. set by postguard */
   GUARD_DATA *guard_data;
   short main_ability;
   short colors[MAX_COLORS];
   int home_vnum; /* hotboot tracker */
   int resetvnum;
   int resetnum;
};


#endif /* ZEROPOINT_CHARACTER_CHARACTER_H */
