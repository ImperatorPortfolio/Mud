#ifndef ZEROPOINT_SPACE_SHIP_H
#define ZEROPOINT_SPACE_SHIP_H


struct ship_data
{
   SHIP_DATA *next;
   SHIP_DATA *prev;
   SHIP_DATA *next_in_starsystem;
   SHIP_DATA *prev_in_starsystem;
   SHIP_DATA *next_in_room;
   SHIP_DATA *prev_in_room;
   ROOM_INDEX_DATA *in_room;
   SPACE_DATA *starsystem;
   const char *filename;
   const char *name;
   const char *home;
   const char *description;
   const char *owner;
   const char *pilot;
   const char *copilot;
   const char *dest;
   short type;
   short ship_class;
   short comm;
   short sensor;
   short astro_array;
   short hyperspeed;
   int hyperdistance;
   short realspeed;
   short currspeed;
   short shipstate;
   short statet0;
   short statet1;
   short statet2;
   short missiletype;
   short missilestate;
   short missiles;
   short maxmissiles;
   short torpedos;
   short maxtorpedos;
   short rockets;
   short maxrockets;
   short lasers;
   short tractorbeam;
   short manuever;
   bool bayopen;
   bool hatchopen;
   bool autorecharge;
   bool autotrack;
   bool autospeed;
   float vx, vy, vz;
   float hx, hy, hz;
   float jx, jy, jz;
   int maxenergy;
   int energy;
   int shield;
   int maxshield;
   int hull;
   int maxhull;
   int cockpit;
   int turret1;
   int turret2;
   int location;
   int lastdoc;
   int shipyard;
   int entrance;
   int hanger;
   int engineroom;
   int firstroom;
   int lastroom;
   int navseat;
   int pilotseat;
   int coseat;
   int gunseat;
   long collision;
   SHIP_DATA *target0;
   SHIP_DATA *target1;
   SHIP_DATA *target2;
   SPACE_DATA *currjump;
   short chaff;
   short maxchaff;
   short chaff_released;
   bool autopilot;
};

struct missile_data
{
   MISSILE_DATA *next;
   MISSILE_DATA *prev;
   MISSILE_DATA *next_in_starsystem;
   MISSILE_DATA *prev_in_starsystem;
   SPACE_DATA *starsystem;
   SHIP_DATA *target;
   SHIP_DATA *fired_from;
   const char *fired_by;
   short missiletype;
   short age;
   int speed;
   int mx, my, mz;
};

#endif /* ZEROPOINT_SPACE_SHIP_H */
