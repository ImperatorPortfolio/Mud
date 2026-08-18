#ifndef ZEROPOINT_SPACE_PLANET_H
#define ZEROPOINT_SPACE_PLANET_H


struct bounty_data
{
   BOUNTY_DATA *next;
   BOUNTY_DATA *prev;
   const char *target;
   long int amount;
};

struct guard_data
{
   GUARD_DATA *next;
   GUARD_DATA *prev;
   GUARD_DATA *next_on_planet;
   GUARD_DATA *prev_on_planet;
   CHAR_DATA *mob;
   ROOM_INDEX_DATA *reset_loc;
   PLANET_DATA *planet;
};

struct senate_data
{
   SENATE_DATA *next;
   SENATE_DATA *prev;
   const char *name;
};

struct planet_data
{
   PLANET_DATA *next;
   PLANET_DATA *prev;
   PLANET_DATA *next_in_system;
   PLANET_DATA *prev_in_system;
   GUARD_DATA *first_guard;
   GUARD_DATA *last_guard;
   SPACE_DATA *starsystem;
   AREA_DATA *first_area;
   AREA_DATA *last_area;
   const char *name;
   const char *filename;
   long base_value;
   CLAN_DATA *governed_by;
   int population;
   bool flags;
   float pop_support;
};

#define PLANET_NOCAPTURE  BV00

struct clan_data
{
   CLAN_DATA *next;  /* next clan in list       */
   CLAN_DATA *prev;  /* previous clan in list      */
   CLAN_DATA *next_subclan;
   CLAN_DATA *prev_subclan;
   CLAN_DATA *first_subclan;
   CLAN_DATA *last_subclan;
   CLAN_DATA *mainclan;
   const char *filename;   /* Clan filename        */
   const char *name; /* Clan name            */
   const char *description;   /* A brief description of the clan  */
   const char *leader;  /* Head clan leader        */
   const char *number1; /* First officer        */
   const char *number2; /* Second officer       */
   int pkills; /* Number of pkills on behalf of clan  */
   int pdeaths;   /* Number of pkills against clan */
   int mkills; /* Number of mkills on behalf of clan  */
   int mdeaths;   /* Number of clan deaths due to mobs   */
   short clan_type;  /* See clan type defines      */
   short members; /* Number of clan members     */
   int board;  /* Vnum of clan board         */
   int storeroom; /* Vnum of clan's store room     */
   int guard1; /* Vnum of clan guard type 1     */
   int guard2; /* Vnum of clan guard type 2     */
   int patrol1;   /* vnum of patrol */
   int patrol2;   /* vnum of patrol */
   int trooper1;  /* vnum of reinforcements */
   int trooper2;  /* vnum of elite troopers */
   long int funds;
   int spacecraft;
   int vehicles;
   int jail;
   const char *tmpstr;
};


#endif /* ZEROPOINT_SPACE_PLANET_H */
