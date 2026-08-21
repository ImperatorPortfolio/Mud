/***************************************************************************
 * Zero Point hierarchical geography persistence.
 *
 * Adds the two missing world hierarchy layers without removing the legacy
 * starsystem->planet and planet->area links used by existing game code:
 *
 *   Starsystem -> Sector -> Planet -> Zone -> Area
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mud.h"

SECTOR_DATA *first_sector;
SECTOR_DATA *last_sector;
ZONE_DATA *first_zone;
ZONE_DATA *last_zone;

/* clans.c retains the inherited loader behind this compatibility symbol. */
void legacy_load_planets( void );

static SPACE_DATA *geography_starsystem_from_name( const char *name )
{
   SPACE_DATA *starsystem;

   if( !name )
      return NULL;

   for( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
      if( starsystem->name && !str_cmp( name, starsystem->name ) )
         return starsystem;

   return NULL;
}

static PLANET_DATA *geography_planet_from_name( const char *name )
{
   PLANET_DATA *planet;

   if( !name )
      return NULL;

   for( planet = first_planet; planet; planet = planet->next )
      if( planet->name && !str_cmp( name, planet->name ) )
         return planet;

   return NULL;
}

static AREA_DATA *geography_area_from_filename( const char *filename )
{
   AREA_DATA *area;

   if( !filename )
      return NULL;

   for( area = first_area; area; area = area->next )
      if( area->filename && !str_cmp( filename, area->filename ) )
         return area;

   return NULL;
}

SECTOR_DATA *get_sector( const char *name )
{
   SECTOR_DATA *sector;

   if( !name )
      return NULL;

   for( sector = first_sector; sector; sector = sector->next )
      if( sector->name && !str_cmp( name, sector->name ) )
         return sector;

   return NULL;
}

ZONE_DATA *get_zone( const char *name )
{
   ZONE_DATA *zone;

   if( !name )
      return NULL;

   for( zone = first_zone; zone; zone = zone->next )
      if( zone->name && !str_cmp( name, zone->name ) )
         return zone;

   return NULL;
}

SECTOR_DATA *sector_from_planet( const PLANET_DATA *planet )
{
   return planet ? planet->sector : NULL;
}

ZONE_DATA *zone_from_area( const AREA_DATA *area )
{
   return area ? area->zone : NULL;
}

void write_sector_list( void )
{
   SECTOR_DATA *sector;
   FILE *fpout;
   char filename[256];

   snprintf( filename, sizeof( filename ), "%s%s", SECTOR_DIR, SECTOR_LIST );
   fpout = fopen( filename, "w" );
   if( !fpout )
   {
      bug( "%s: FATAL: cannot open sector.lst for writing!\r\n", __func__ );
      return;
   }

   for( sector = first_sector; sector; sector = sector->next )
      fprintf( fpout, "%s\n", sector->filename );
   fprintf( fpout, "$\n" );
   FCLOSE( fpout );
}

void write_zone_list( void )
{
   ZONE_DATA *zone;
   FILE *fpout;
   char filename[256];

   snprintf( filename, sizeof( filename ), "%s%s", ZONE_DIR, ZONE_LIST );
   fpout = fopen( filename, "w" );
   if( !fpout )
   {
      bug( "%s: FATAL: cannot open zone.lst for writing!\r\n", __func__ );
      return;
   }

   for( zone = first_zone; zone; zone = zone->next )
      fprintf( fpout, "%s\n", zone->filename );
   fprintf( fpout, "$\n" );
   FCLOSE( fpout );
}

void save_sector( SECTOR_DATA *sector )
{
   PLANET_DATA *planet;
   FILE *fp;
   char filename[256];

   if( !sector )
   {
      bug( "%s: null sector pointer!", __func__ );
      return;
   }

   if( !sector->filename || sector->filename[0] == '\0' )
   {
      bug( "%s: %s has no filename", __func__, sector->name ? sector->name : "(unnamed sector)" );
      return;
   }

   snprintf( filename, sizeof( filename ), "%s%s", SECTOR_DIR, sector->filename );
   if( ( fp = fopen( filename, "w" ) ) == NULL )
   {
      bug( "%s: fopen", __func__ );
      perror( filename );
      return;
   }

   fprintf( fp, "#SECTOR\n" );
   fprintf( fp, "Name         %s~\n", sector->name ? sector->name : "" );
   fprintf( fp, "Filename     %s~\n", sector->filename );
   fprintf( fp, "Flags        %d\n", sector->flags );
   if( sector->starsystem && sector->starsystem->name )
      fprintf( fp, "Starsystem   %s~\n", sector->starsystem->name );
   for( planet = sector->first_planet; planet; planet = planet->next_in_sector )
      if( planet->name )
         fprintf( fp, "Planet       %s~\n", planet->name );
   fprintf( fp, "End\n\n" );
   fprintf( fp, "#END\n" );
   FCLOSE( fp );
}

void save_zone( ZONE_DATA *zone )
{
   AREA_DATA *area;
   FILE *fp;
   char filename[256];

   if( !zone )
   {
      bug( "%s: null zone pointer!", __func__ );
      return;
   }

   if( !zone->filename || zone->filename[0] == '\0' )
   {
      bug( "%s: %s has no filename", __func__, zone->name ? zone->name : "(unnamed zone)" );
      return;
   }

   snprintf( filename, sizeof( filename ), "%s%s", ZONE_DIR, zone->filename );
   if( ( fp = fopen( filename, "w" ) ) == NULL )
   {
      bug( "%s: fopen", __func__ );
      perror( filename );
      return;
   }

   fprintf( fp, "#ZONE\n" );
   fprintf( fp, "Name         %s~\n", zone->name ? zone->name : "" );
   fprintf( fp, "Filename     %s~\n", zone->filename );
   fprintf( fp, "Flags        %d\n", zone->flags );
   if( zone->planet && zone->planet->name )
      fprintf( fp, "Planet       %s~\n", zone->planet->name );
   for( area = zone->first_area; area; area = area->next_on_zone )
      if( area->filename )
         fprintf( fp, "Area         %s~\n", area->filename );
   fprintf( fp, "End\n\n" );
   fprintf( fp, "#END\n" );
   FCLOSE( fp );
}

static void fread_sector( SECTOR_DATA *sector, FILE *fp )
{
   const char *word;
   bool fMatch;

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               if( !sector->name )
                  sector->name = STRALLOC( "" );
               return;
            }
            break;

         case 'F':
            KEY( "Filename", sector->filename, fread_string_nohash( fp ) );
            KEY( "Flags", sector->flags, fread_number( fp ) );
            break;

         case 'N':
            KEY( "Name", sector->name, fread_string( fp ) );
            break;

         case 'P':
            if( !str_cmp( word, "Planet" ) )
            {
               const char *planet_name = fread_string( fp );
               PLANET_DATA *planet = geography_planet_from_name( planet_name );

               if( planet )
               {
                  if( planet->sector && planet->sector != sector )
                     bug( "%s: planet %s already belongs to sector %s", __func__, planet->name,
                          planet->sector->name ? planet->sector->name : "(unnamed)" );
                  else if( !planet->sector )
                  {
                     planet->sector = sector;
                     LINK( planet, sector->first_planet, sector->last_planet, next_in_sector, prev_in_sector );
                  }
               }
               fMatch = TRUE;
               STRFREE( planet_name );
            }
            break;

         case 'S':
            if( !str_cmp( word, "Starsystem" ) )
            {
               const char *system_name = fread_string( fp );
               SPACE_DATA *starsystem = geography_starsystem_from_name( system_name );

               if( starsystem )
               {
                  if( sector->starsystem && sector->starsystem != starsystem )
                     bug( "%s: sector %s has more than one starsystem", __func__,
                          sector->name ? sector->name : "(unnamed)" );
                  else if( !sector->starsystem )
                  {
                     sector->starsystem = starsystem;
                     LINK( sector, starsystem->first_sector, starsystem->last_sector, next_in_system, prev_in_system );
                  }
               }
               fMatch = TRUE;
               STRFREE( system_name );
            }
            break;
      }

      if( !fMatch )
         bug( "%s: no match: %s", __func__, word );
   }
}

static void fread_zone( ZONE_DATA *zone, FILE *fp )
{
   const char *word;
   bool fMatch;

   for( ;; )
   {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch( UPPER( word[0] ) )
      {
         case '*':
            fMatch = TRUE;
            fread_to_eol( fp );
            break;

         case 'A':
            if( !str_cmp( word, "Area" ) )
            {
               const char *area_name = fread_string( fp );
               AREA_DATA *area = geography_area_from_filename( area_name );

               if( area )
               {
                  if( area->zone && area->zone != zone )
                     bug( "%s: area %s already belongs to zone %s", __func__, area->filename,
                          area->zone->name ? area->zone->name : "(unnamed)" );
                  else if( !area->zone )
                  {
                     area->zone = zone;
                     LINK( area, zone->first_area, zone->last_area, next_on_zone, prev_on_zone );
                  }
               }
               fMatch = TRUE;
               STRFREE( area_name );
            }
            break;

         case 'E':
            if( !str_cmp( word, "End" ) )
            {
               if( !zone->name )
                  zone->name = STRALLOC( "" );
               return;
            }
            break;

         case 'F':
            KEY( "Filename", zone->filename, fread_string_nohash( fp ) );
            KEY( "Flags", zone->flags, fread_number( fp ) );
            break;

         case 'N':
            KEY( "Name", zone->name, fread_string( fp ) );
            break;

         case 'P':
            if( !str_cmp( word, "Planet" ) )
            {
               const char *planet_name = fread_string( fp );
               PLANET_DATA *planet = geography_planet_from_name( planet_name );

               if( planet )
               {
                  if( zone->planet && zone->planet != planet )
                     bug( "%s: zone %s has more than one planet", __func__, zone->name ? zone->name : "(unnamed)" );
                  else if( !zone->planet )
                  {
                     zone->planet = planet;
                     LINK( zone, planet->first_zone, planet->last_zone, next_in_planet, prev_in_planet );
                  }
               }
               fMatch = TRUE;
               STRFREE( planet_name );
            }
            break;
      }

      if( !fMatch )
         bug( "%s: no match: %s", __func__, word );
   }
}

static bool load_sector_file( const char *sectorfile )
{
   char filename[256];
   SECTOR_DATA *sector;
   FILE *fp;
   bool found = FALSE;

   CREATE( sector, SECTOR_DATA, 1 );
   sector->starsystem = NULL;
   sector->first_planet = NULL;
   sector->last_planet = NULL;

   snprintf( filename, sizeof( filename ), "%s%s", SECTOR_DIR, sectorfile );
   if( ( fp = fopen( filename, "r" ) ) != NULL )
   {
      found = TRUE;
      for( ;; )
      {
         char letter = fread_letter( fp );
         const char *word;

         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }
         if( letter != '#' )
         {
            bug( "%s: # not found.", __func__ );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "SECTOR" ) )
         {
            fread_sector( sector, fp );
            break;
         }
         if( !str_cmp( word, "END" ) )
            break;

         bug( "%s: bad section: %s.", __func__, word );
         break;
      }
      FCLOSE( fp );
   }

   if( !found )
      DISPOSE( sector );
   else
      LINK( sector, first_sector, last_sector, next, prev );

   return found;
}

static bool load_zone_file( const char *zonefile )
{
   char filename[256];
   ZONE_DATA *zone;
   FILE *fp;
   bool found = FALSE;

   CREATE( zone, ZONE_DATA, 1 );
   zone->planet = NULL;
   zone->first_area = NULL;
   zone->last_area = NULL;

   snprintf( filename, sizeof( filename ), "%s%s", ZONE_DIR, zonefile );
   if( ( fp = fopen( filename, "r" ) ) != NULL )
   {
      found = TRUE;
      for( ;; )
      {
         char letter = fread_letter( fp );
         const char *word;

         if( letter == '*' )
         {
            fread_to_eol( fp );
            continue;
         }
         if( letter != '#' )
         {
            bug( "%s: # not found.", __func__ );
            break;
         }

         word = fread_word( fp );
         if( !str_cmp( word, "ZONE" ) )
         {
            fread_zone( zone, fp );
            break;
         }
         if( !str_cmp( word, "END" ) )
            break;

         bug( "%s: bad section: %s.", __func__, word );
         break;
      }
      FCLOSE( fp );
   }

   if( !found )
      DISPOSE( zone );
   else
      LINK( zone, first_zone, last_zone, next, prev );

   return found;
}

void load_sectors( void )
{
   FILE *fpList;
   const char *filename;
   char sectorlist[256];
   SPACE_DATA *starsystem;
   PLANET_DATA *planet;

   first_sector = NULL;
   last_sector = NULL;

   for( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
   {
      starsystem->first_sector = NULL;
      starsystem->last_sector = NULL;
   }

   for( planet = first_planet; planet; planet = planet->next )
   {
      planet->next_in_sector = NULL;
      planet->prev_in_sector = NULL;
      planet->sector = NULL;
   }

   log_string( "Loading sectors..." );
   snprintf( sectorlist, sizeof( sectorlist ), "%s%s", SECTOR_DIR, SECTOR_LIST );
   if( ( fpList = fopen( sectorlist, "r" ) ) == NULL )
   {
      perror( sectorlist );
      exit( 1 );
   }

   for( ;; )
   {
      filename = feof( fpList ) ? "$" : fread_word( fpList );
      log_string( filename );
      if( filename[0] == '$' )
         break;

      if( !load_sector_file( filename ) )
         bug( "%s: Cannot load sector file: %s", __func__, filename );
   }
   FCLOSE( fpList );
   log_string( " Done sectors " );
}

void load_zones( void )
{
   FILE *fpList;
   const char *filename;
   char zonelist[256];
   PLANET_DATA *planet;
   AREA_DATA *area;

   first_zone = NULL;
   last_zone = NULL;

   for( planet = first_planet; planet; planet = planet->next )
   {
      planet->first_zone = NULL;
      planet->last_zone = NULL;
   }

   for( area = first_area; area; area = area->next )
   {
      area->next_on_zone = NULL;
      area->prev_on_zone = NULL;
      area->zone = NULL;
   }

   log_string( "Loading zones..." );
   snprintf( zonelist, sizeof( zonelist ), "%s%s", ZONE_DIR, ZONE_LIST );
   if( ( fpList = fopen( zonelist, "r" ) ) == NULL )
   {
      perror( zonelist );
      exit( 1 );
   }

   for( ;; )
   {
      filename = feof( fpList ) ? "$" : fread_word( fpList );
      log_string( filename );
      if( filename[0] == '$' )
         break;

      if( !load_zone_file( filename ) )
         bug( "%s: Cannot load zone file: %s", __func__, filename );
   }
   FCLOSE( fpList );
   log_string( " Done zones " );
}

/*
 * Keep db.c's existing load_planets() boot call as the single geography
 * bootstrap point. Existing planet loading runs first, then the two new
 * hierarchy layers resolve against the already-loaded systems/planets/areas.
 */
void load_planets( void )
{
   legacy_load_planets();
   load_sectors();
   load_zones();
}
