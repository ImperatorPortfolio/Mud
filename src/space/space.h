#ifndef ZEROPOINT_SPACE_SPACE_H
#define ZEROPOINT_SPACE_SPACE_H

typedef enum
{
   CLAN_PLAIN, CLAN_CRIME, CLAN_GUILD, CLAN_SUBCLAN
} clan_types;

typedef enum
{ SHIP_CIVILIAN, SHIP_REPUBLIC, SHIP_IMPERIAL, MOB_SHIP } ship_types;
typedef enum
{ SHIP_DOCKED, SHIP_READY, SHIP_BUSY, SHIP_BUSY_2, SHIP_BUSY_3, SHIP_REFUEL,
SHIP_LAUNCH, SHIP_LAUNCH_2, SHIP_LAND, SHIP_LAND_2, SHIP_HYPERSPACE, SHIP_DISABLED, SHIP_FLYING
} ship_states;
typedef enum
{ MISSILE_READY, MISSILE_FIRED, MISSILE_RELOAD, MISSILE_RELOAD_2, MISSILE_DAMAGED } missile_states;
typedef enum
{ FIGHTER_SHIP, MIDSIZE_SHIP, CAPITAL_SHIP, SHIP_PLATFORM, CLOUD_CAR, OCEAN_SHIP, LAND_SPEEDER, WHEELED, LAND_CRAWLER,
WALKER
} ship_classes;
typedef enum
{ CONCUSSION_MISSILE, PROTON_TORPEDO, HEAVY_ROCKET, HEAVY_BOMB } missile_types;

typedef enum
{ GROUP_CLAN, GROUP_COUNCIL, GROUP_GUILD } group_types;

#define LASER_DAMAGED    -1
#define LASER_READY       0

struct space_data
{
   SPACE_DATA *next;
   SPACE_DATA *prev;
   SHIP_DATA *first_ship;
   SHIP_DATA *last_ship;
   MISSILE_DATA *first_missile;
   MISSILE_DATA *last_missile;
   PLANET_DATA *first_planet;
   PLANET_DATA *last_planet;
   const char *filename;
   const char *name;
   const char *star1;
   const char *star2;
   const char *planet1;
   const char *planet2;
   const char *planet3;
   const char *location1a;
   const char *location2a;
   const char *location3a;
   const char *location1b;
   const char *location2b;
   const char *location3b;
   const char *location1c;
   const char *location2c;
   const char *location3c;
   int xpos;
   int ypos;
   int s1x;
   int s1y;
   int s1z;
   int s2x;
   int s2y;
   int s2z;
   int doc1a;
   int doc2a;
   int doc3a;
   int doc1b;
   int doc2b;
   int doc3b;
   int doc1c;
   int doc2c;
   int doc3c;
   int p1x;
   int p1y;
   int p1z;
   int p2x;
   int p2y;
   int p2z;
   int p3x;
   int p3y;
   int p3z;
   int gravitys1;
   int gravitys2;
   int gravityp1;
   int gravityp2;
   int gravityp3;
   int p1_low;
   int p1_high;
   int p2_low;
   int p2_high;
   int p3_low;
   int p3_high;
   int crash;
};

#include "planet.h"
#include "ship.h"

#endif /* ZEROPOINT_SPACE_SPACE_H */
