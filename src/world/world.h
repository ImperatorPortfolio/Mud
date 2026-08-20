#ifndef ZEROPOINT_WORLD_WORLD_H
#define ZEROPOINT_WORLD_WORLD_H

struct liq_type
{
   const char *liq_name;
   const char *liq_color;
   short liq_affect[3];
};

/*
* Extra description data for a room or object.
*/
struct extra_descr_data
{
   EXTRA_DESCR_DATA *next; /* Next in list                     */
   EXTRA_DESCR_DATA *prev; /* Previous in list                 */
   const char *keyword; /* Keyword in look/examine          */
   const char *description;   /* What to see                      */
};

/*
* Prototype for an object.
*/
struct obj_index_data
{
   OBJ_INDEX_DATA *next;
   OBJ_INDEX_DATA *next_sort;
   EXTRA_DESCR_DATA *first_extradesc;
   EXTRA_DESCR_DATA *last_extradesc;
   AFFECT_DATA *first_affect;
   AFFECT_DATA *last_affect;
   MPROG_DATA *mudprogs;   /* objprogs */
   AREA_DATA *area;
   int progtypes; /* objprogs */
   const char *name;
   const char *short_descr;
   const char *description;
   const char *action_desc;
   int vnum;
   short level;
   short item_type;
   int extra_flags;
   int magic_flags;  /*Need more bitvectors for spells - Scryn */
   int wear_flags;
   short count;
   short weight;
   int cost;
   int value[6];
   /*
 * Per-object nutritional values:
 * protein, carbs, fats, vitamins, minerals, hydration.
 */
short nutrition[MAX_NUTRITION];
   int serial;
   short layers;
   int rent;   /* Unused */
};

/*
* One object.
*/
struct obj_data
{
   OBJ_DATA *next;
   OBJ_DATA *prev;
   OBJ_DATA *next_content;
   OBJ_DATA *prev_content;
   OBJ_DATA *first_content;
   OBJ_DATA *last_content;
   OBJ_DATA *in_obj;
   CHAR_DATA *carried_by;
   EXTRA_DESCR_DATA *first_extradesc;
   EXTRA_DESCR_DATA *last_extradesc;
   AFFECT_DATA *first_affect;
   AFFECT_DATA *last_affect;
   OBJ_INDEX_DATA *pIndexData;
   ROOM_INDEX_DATA *in_room;
   const char *armed_by;
   const char *name;
   const char *short_descr;
   const char *description;
   const char *action_desc;
   short item_type;
   short mpscriptpos;
   int extra_flags;
   int magic_flags;  /*Need more bitvectors for spells - Scryn */
   int wear_flags;
   int blaster_setting;
   MPROG_ACT_LIST *mpact;  /* mudprogs */
   int mpactnum;  /* mudprogs */
   short wear_loc;
   short weight;
   int cost;
   short level;
   short timer;
   int value[6];
   short nutrition[MAX_NUTRITION];

   short count;   /* support for object grouping */
   int serial; /* serial number         */
   int room_vnum; /* hotboot tracker */
};

/*
* Exit data.
*/
struct exit_data
{
   EXIT_DATA *prev;  /* previous exit in linked list  */
   EXIT_DATA *next;  /* next exit in linked list   */
   EXIT_DATA *rexit; /* Reverse exit pointer    */
   ROOM_INDEX_DATA *to_room;  /* Pointer to destination room   */
   const char *keyword; /* Keywords for exit or door  */
   const char *description;   /* Description of exit     */
   int vnum;   /* Vnum of room exit leads to */
   int rvnum;  /* Vnum of room in opposite dir  */
   int exit_info; /* door states & other flags  */
   int key; /* Key vnum       */
   short vdir; /* Physical "direction"    */
   short distance;   /* how far to the next room   */
};

/*
* Reset commands:
*   '*': comment
*   'M': read a mobile
*   'O': read an object
*   'P': put object in object
*   'G': give object to mobile
*   'E': equip object to mobile
*   'H': hide an object
*   'B': set a bitvector
*   'T': trap an object
*   'D': set state of door
*   'R': randomize room exits
*   'S': stop (end of list)
*/

/*
* Area-reset definition.
*/
struct reset_data
{
   RESET_DATA *next;
   RESET_DATA *prev;
   RESET_DATA *first_reset;
   RESET_DATA *last_reset;
   RESET_DATA *next_reset;
   RESET_DATA *prev_reset;
   char command;
   int extra;
   int arg1;
   int arg2;
   int arg3;
   bool sreset;
};

/* Constants for arg2 of 'B' resets. */
#define BIT_RESET_DOOR			0
#define BIT_RESET_OBJECT		1
#define BIT_RESET_MOBILE		2
#define BIT_RESET_ROOM			3
#define BIT_RESET_TYPE_MASK		0xFF  /* 256 should be enough */
#define BIT_RESET_DOOR_THRESHOLD	8
#define BIT_RESET_DOOR_MASK		0xFF00   /* 256 should be enough */
#define BIT_RESET_SET			BV30
#define BIT_RESET_TOGGLE		BV31
#define BIT_RESET_FREEBITS	  0x3FFF0000   /* For reference */

/*
* Area definition.
*/
struct area_data
{
   AREA_DATA *next;
   AREA_DATA *prev;
   AREA_DATA *next_sort;
   AREA_DATA *prev_sort;
   AREA_DATA *next_sort_name; /* Used for alphanum. sort */
   AREA_DATA *prev_sort_name; /* Ditto, Fireblade */
   PLANET_DATA *planet;
   AREA_DATA *next_on_planet;
   AREA_DATA *prev_on_planet;
   ROOM_INDEX_DATA *first_room;
   ROOM_INDEX_DATA *last_room;
   const char *name;
   const char *filename;
   int flags;
   short version;
   short status;  /* h, 8/11 */
   short age;
   short nplayer;
   short reset_frequency;
   int low_r_vnum;
   int hi_r_vnum;
   int low_o_vnum;
   int hi_o_vnum;
   int low_m_vnum;
   int hi_m_vnum;
   int low_soft_range;
   int hi_soft_range;
   int low_hard_range;
   int hi_hard_range;
   const char *author;  /* Scryn */
   const char *resetmsg;   /* Rennard */
   short max_players;
   int mkills;
   int mdeaths;
   int pkills;
   int pdeaths;
   int gold_looted;
   int illegal_pk;
   int high_economy;
   int low_economy;
};

/*
* Used to keep track of system settings and statistics		-Thoric
*/
struct system_data
{
   int maxplayers;   /* Maximum players this boot   */
   int alltimemax;   /* Maximum players ever   */
   const char *time_of_max;   /* Time of max ever */
   bool NO_NAME_RESOLVING; /* Hostnames are not resolved  */
   bool DENY_NEW_PLAYERS;  /* New players cannot connect  */
   bool WAIT_FOR_AUTH;  /* New players must be auth'ed */
   short read_all_mail; /* Read all player mail(was 54) */
   short read_mail_free;   /* Read mail for free (was 51) */
   short write_mail_free;  /* Write mail for free(was 51) */
   short take_others_mail; /* Take others mail (was 54)   */
   short muse_level; /* Level of muse channel */
   short think_level;   /* Level of think channel LEVEL_HIGOD */
   short build_level;   /* Level of build channel LEVEL_BUILD */
   short log_level;  /* Level of log channel LEVEL LOG */
   short level_modify_proto;  /* Level to modify prototype stuff LEVEL_LESSER */
   short level_override_private; /* override private flag */
   short level_mset_player;   /* Level to mset a player */
   short stun_plr_vs_plr;  /* Stun mod player vs. player */
   short stun_regular;  /* Stun difficult */
   short dam_plr_vs_plr;   /* Damage mod player vs. player */
   short dam_plr_vs_mob;   /* Damage mod player vs. mobile */
   short dam_mob_vs_plr;   /* Damage mod mobile vs. player */
   short dam_mob_vs_mob;   /* Damage mod mobile vs. mobile */
   short level_getobjnotake;  /* Get objects without take flag */
   short level_forcepc; /* The level at which you can use force on players. */
   short max_sn;  /* Max skills */
   const char *guild_overseer;   /* Pointer to char containing the name of the */
   const char *guild_advisor; /* guild overseer and advisor. */
   int save_flags;   /* Toggles for saving conditions */
   short save_frequency;   /* How old to autosave someone */
   void *dlHandle;
};

/*
* Room type.
*/
struct room_index_data
{
   ROOM_INDEX_DATA *next;
   ROOM_INDEX_DATA *next_sort;
   CHAR_DATA *first_person;
   CHAR_DATA *last_person;
   OBJ_DATA *first_content;
   OBJ_DATA *last_content;
   EXTRA_DESCR_DATA *first_extradesc;
   EXTRA_DESCR_DATA *last_extradesc;
   AREA_DATA *area;
   EXIT_DATA *first_exit;
   EXIT_DATA *last_exit;
   SHIP_DATA *first_ship;
   SHIP_DATA *last_ship;
   RESET_DATA *first_reset;
   RESET_DATA *last_reset;
   RESET_DATA *last_mob_reset;
   RESET_DATA *last_obj_reset;
   ROOM_INDEX_DATA *next_aroom; /* Rooms within an area */
   ROOM_INDEX_DATA *prev_aroom;
   const char *name;
   const char *description;
   int vnum;
   int room_flags;
   MPROG_ACT_LIST *mpact;  /* mudprogs */
   int mpactnum;  /* mudprogs */
   MPROG_DATA *mudprogs;   /* mudprogs */
   short mpscriptpos;
   int progtypes; /* mudprogs */
   short light;
   short sector_type;
   int tele_vnum;
   short tele_delay;
   short tunnel;  /* max people that will fit */
   int map_x;
   int map_y;
   int map_z;
   bool map_coords_set;
};

/*
* Delayed teleport type.
*/
struct teleport_data
{
   TELEPORT_DATA *next;
   TELEPORT_DATA *prev;
   ROOM_INDEX_DATA *room;
   short timer;
};

/*
* Types of skill numbers.  Used to keep separate lists of sn's
* Must be non-overlapping with spell/skill types,
* but may be arbitrary beyond that.
*/
#define TYPE_UNDEFINED               -1
#define TYPE_HIT                     1000 /* allows for 1000 skills/spells */
#define TYPE_HERB		     2000   /* allows for 1000 attack types  */
#define TYPE_PERSONAL		     3000   /* allows for 1000 herb types    */

/*
*  Target types.
*/

#endif /* ZEROPOINT_WORLD_WORLD_H */
