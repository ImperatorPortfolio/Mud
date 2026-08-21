#ifndef ZEROPOINT_SPACE_GEOGRAPHY_H
#define ZEROPOINT_SPACE_GEOGRAPHY_H

#define SECTOR_DIR  "../sectors/"
#define SECTOR_LIST "sector.lst"
#define ZONE_DIR    "../zones/"
#define ZONE_LIST   "zone.lst"

typedef struct sector_data SECTOR_DATA;
typedef struct zone_data ZONE_DATA;
typedef struct zone_area_data ZONE_AREA_DATA;

struct sector_data
{
   SECTOR_DATA *next;
   SECTOR_DATA *prev;
   SECTOR_DATA *next_in_system;
   SECTOR_DATA *prev_in_system;
   PLANET_DATA *first_planet;
   PLANET_DATA *last_planet;
   SPACE_DATA *starsystem;
   const char *name;
   const char *filename;
   int flags;
};

struct zone_area_data
{
   ZONE_AREA_DATA *next;
   ZONE_AREA_DATA *prev;
   AREA_DATA *area;
};

struct zone_data
{
   ZONE_DATA *next;
   ZONE_DATA *prev;
   ZONE_DATA *next_in_planet;
   ZONE_DATA *prev_in_planet;
   ZONE_AREA_DATA *first_area;
   ZONE_AREA_DATA *last_area;
   PLANET_DATA *planet;
   const char *name;
   const char *filename;
   int flags;
};

extern SECTOR_DATA *first_sector;
extern SECTOR_DATA *last_sector;
extern ZONE_DATA *first_zone;
extern ZONE_DATA *last_zone;

SECTOR_DATA *get_sector( const char *name );
ZONE_DATA *get_zone( const char *name );
SECTOR_DATA *sector_from_planet( const PLANET_DATA *planet );
ZONE_DATA *zone_from_area( const AREA_DATA *area );

void save_sector( SECTOR_DATA *sector );
void save_zone( ZONE_DATA *zone );
void write_sector_list( void );
void write_zone_list( void );
void load_sectors( void );
void load_zones( void );

#endif /* ZEROPOINT_SPACE_GEOGRAPHY_H */
