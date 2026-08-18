#ifndef ZEROPOINT_WORLD_SERVICES_H
#define ZEROPOINT_WORLD_SERVICES_H

/*
* TO types for act.
*/
#define TO_ROOM		    0
#define TO_NOTVICT	    1
#define TO_VICT		    2
#define TO_CHAR		    3

#define INIT_WEAPON_CONDITION    12
#define MAX_ITEM_IMPACT		 30

/*
* Help table types.
*/
struct help_data
{
   HELP_DATA *next;
   HELP_DATA *prev;
   short level;
   const char *keyword;
   const char *text;
};

/*
* Shop types.
*/
#define MAX_TRADE	 5

struct shop_data
{
   SHOP_DATA *next;  /* Next shop in list    */
   SHOP_DATA *prev;  /* Previous shop in list   */
   int keeper; /* Vnum of shop keeper mob */
   short buy_type[MAX_TRADE]; /* Item types shop will buy   */
   short profit_buy; /* Cost multiplier for buying */
   short profit_sell;   /* Cost multiplier for selling   */
   short open_hour;  /* First opening hour      */
   short close_hour; /* First closing hour      */
};

#define MAX_FIX		3
#define SHOP_FIX	1
#define SHOP_RECHARGE	2

struct repairshop_data
{
   REPAIR_DATA *next;   /* Next shop in list    */
   REPAIR_DATA *prev;   /* Previous shop in list   */
   int keeper; /* Vnum of shop keeper mob */
   short fix_type[MAX_FIX];   /* Item types shop will fix   */
   short profit_fix; /* Cost multiplier for fixing */
   short shop_type;  /* Repair shop type     */
   short open_hour;  /* First opening hour      */
   short close_hour; /* First closing hour      */
};

/* Mob program structures */
struct act_prog_data
{
   struct act_prog_data *next;
   void *vo;
};

struct mob_prog_act_list
{
   MPROG_ACT_LIST *next;
   const char *buf;
   CHAR_DATA *ch;
   OBJ_DATA *obj;
   void *vo;
};

struct mob_prog_data
{
   MPROG_DATA *next;
   int type;
   bool triggered;
   int resetdelay;
   const char *arglist;
   const char *comlist;
   bool fileprog;
};

extern bool MOBtrigger;

/* race dedicated stuff */
struct race_type
{
   const char *race_name;  /* Race name         */
   int affected;  /* Default affect bitvectors  */
   short str_plus;   /* Str bonus/penalty    */
   short dex_plus;   /* Dex      "        */
   short wis_plus;   /* Wis      "        */
   short int_plus;   /* Int      "        */
   short con_plus;   /* Con      "        */
   short cha_plus;   /* Cha      "        */
   short lck_plus;   /* Lck       "       */
   short frc_plus;   /* Frc       "       */
   short hit;
   short mana;
   short resist;
   short suscept;
   int class_restriction;  /* Flags for illegal classes  */
   int language;  /* Default racial language      */
};


#endif /* ZEROPOINT_WORLD_SERVICES_H */
