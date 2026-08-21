/***************************************************************************
 * Zero Point hierarchical geography builder commands.
 *
 * Provides first-class create/list/show/edit tooling for the persisted
 * Starsystem -> Sector -> Planet -> Zone -> Area hierarchy.
 ***************************************************************************/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "mud.h"

/* tables.c retains the inherited loader behind this compatibility symbol. */
void legacy_load_commands( void );

/*
 * Builder selectors should be pleasant to use at the prompt. Normalize case,
 * spaces, separators and a leading "The", then permit one-character typos.
 * Exact matches always outrank normalized/fuzzy matches, and tied fuzzy
 * matches are rejected so a shorthand can never select an arbitrary object.
 */
static void geography_normalize_selector( const char *source, char *target, size_t size )
{
   const unsigned char *p;
   size_t out = 0;

   if( !target || size == 0 )
      return;

   target[0] = '\0';
   if( !source )
      return;

   for( p = ( const unsigned char * )source; *p && out + 1 < size; ++p )
      if( isalnum( *p ) )
         target[out++] = ( char )tolower( *p );
   target[out] = '\0';

   if( out > 3 && target[0] == 't' && target[1] == 'h' && target[2] == 'e' )
      memmove( target, target + 3, out - 2 );
}

static bool geography_one_edit_match( const char *left, const char *right )
{
   size_t llen;
   size_t rlen;
   size_t i = 0;
   size_t j = 0;
   int edits = 0;

   if( !left || !right )
      return FALSE;

   llen = strlen( left );
   rlen = strlen( right );
   if( llen < 4 || rlen < 4 )
      return FALSE;
   if( llen > rlen + 1 || rlen > llen + 1 )
      return FALSE;

   while( i < llen && j < rlen )
   {
      if( left[i] == right[j] )
      {
         ++i;
         ++j;
         continue;
      }

      if( ++edits > 1 )
         return FALSE;

      if( llen > rlen )
         ++i;
      else if( rlen > llen )
         ++j;
      else
      {
         ++i;
         ++j;
      }
   }

   if( i < llen || j < rlen )
      ++edits;

   return edits <= 1;
}

static int geography_selector_rank( const char *selector, const char *candidate )
{
   char normalized_selector[MAX_INPUT_LENGTH];
   char normalized_candidate[MAX_INPUT_LENGTH];

   if( !selector || !candidate || selector[0] == '\0' || candidate[0] == '\0' )
      return 0;

   if( !str_cmp( selector, candidate ) )
      return 3;

   geography_normalize_selector( selector, normalized_selector, sizeof( normalized_selector ) );
   geography_normalize_selector( candidate, normalized_candidate, sizeof( normalized_candidate ) );

   if( normalized_selector[0] == '\0' || normalized_candidate[0] == '\0' )
      return 0;
   if( !strcmp( normalized_selector, normalized_candidate ) )
      return 2;
   if( geography_one_edit_match( normalized_selector, normalized_candidate ) )
      return 1;

   return 0;
}

static SECTOR_DATA *find_sector_selector( const char *selector )
{
   SECTOR_DATA *sector;
   SECTOR_DATA *best = NULL;
   int best_rank = 0;
   bool ambiguous = FALSE;

   if( !selector || selector[0] == '\0' )
      return NULL;

   for( sector = first_sector; sector; sector = sector->next )
   {
      int rank = geography_selector_rank( selector, sector->name );
      int filename_rank = geography_selector_rank( selector, sector->filename );

      if( filename_rank > rank )
         rank = filename_rank;

      if( rank > best_rank )
      {
         best = sector;
         best_rank = rank;
         ambiguous = FALSE;
      }
      else if( rank > 0 && rank == best_rank && sector != best )
         ambiguous = TRUE;
   }

   return ambiguous ? NULL : best;
}

static ZONE_DATA *find_zone_selector( const char *selector )
{
   ZONE_DATA *zone;
   ZONE_DATA *best = NULL;
   int best_rank = 0;
   bool ambiguous = FALSE;

   if( !selector || selector[0] == '\0' )
      return NULL;

   for( zone = first_zone; zone; zone = zone->next )
   {
      int rank = geography_selector_rank( selector, zone->name );
      int filename_rank = geography_selector_rank( selector, zone->filename );

      if( filename_rank > rank )
         rank = filename_rank;

      if( rank > best_rank )
      {
         best = zone;
         best_rank = rank;
         ambiguous = FALSE;
      }
      else if( rank > 0 && rank == best_rank && zone != best )
         ambiguous = TRUE;
   }

   return ambiguous ? NULL : best;
}

static PLANET_DATA *find_planet_selector( const char *selector )
{
   PLANET_DATA *planet;
   PLANET_DATA *best = NULL;
   int best_rank = 0;
   bool ambiguous = FALSE;

   if( !selector || selector[0] == '\0' )
      return NULL;

   for( planet = first_planet; planet; planet = planet->next )
   {
      int rank = geography_selector_rank( selector, planet->name );
      int filename_rank = geography_selector_rank( selector, planet->filename );

      if( filename_rank > rank )
         rank = filename_rank;

      if( rank > best_rank )
      {
         best = planet;
         best_rank = rank;
         ambiguous = FALSE;
      }
      else if( rank > 0 && rank == best_rank && planet != best )
         ambiguous = TRUE;
   }

   return ambiguous ? NULL : best;
}

static AREA_DATA *find_area_selector( const char *selector )
{
   AREA_DATA *area;

   if( !selector || selector[0] == '\0' )
      return NULL;

   for( area = first_area; area; area = area->next )
      if( ( area->name && !str_cmp( selector, area->name ) )
          || ( area->filename && !str_cmp( selector, area->filename ) ) )
         return area;

   return NULL;
}

static bool geography_filename_valid( const char *filename )
{
   const unsigned char *p;

   if( !filename || filename[0] == '\0' )
      return FALSE;

   for( p = ( const unsigned char * )filename; *p; ++p )
      if( !isalnum( *p ) && *p != '_' && *p != '-' )
         return FALSE;

   return TRUE;
}

static void geography_make_filename( const char *name, char *filename, size_t size )
{
   size_t out = 0;
   bool underscore = FALSE;
   const unsigned char *p;

   if( !filename || size == 0 )
      return;

   filename[0] = '\0';
   if( !name )
      return;

   for( p = ( const unsigned char * )name; *p && out + 1 < size; ++p )
   {
      if( isalnum( *p ) || *p == '-' )
      {
         filename[out++] = ( char )*p;
         underscore = FALSE;
      }
      else if( !underscore && out > 0 )
      {
         filename[out++] = '_';
         underscore = TRUE;
      }
   }

   while( out > 0 && filename[out - 1] == '_' )
      --out;
   filename[out] = '\0';
}

static int sector_planet_count( const SECTOR_DATA *sector )
{
   PLANET_DATA *planet;
   int count = 0;

   if( !sector )
      return 0;
   for( planet = sector->first_planet; planet; planet = planet->next_in_sector )
      ++count;
   return count;
}

static int zone_area_count( const ZONE_DATA *zone )
{
   AREA_DATA *area;
   int count = 0;

   if( !zone )
      return 0;
   for( area = zone->first_area; area; area = area->next_on_zone )
      ++count;
   return count;
}

static void set_sector_system_link( SECTOR_DATA *sector, SPACE_DATA *starsystem )
{
   if( !sector || sector->starsystem == starsystem )
      return;

   if( sector->starsystem )
      UNLINK( sector, sector->starsystem->first_sector, sector->starsystem->last_sector,
              next_in_system, prev_in_system );

   sector->next_in_system = NULL;
   sector->prev_in_system = NULL;
   sector->starsystem = starsystem;

   if( starsystem )
      LINK( sector, starsystem->first_sector, starsystem->last_sector, next_in_system, prev_in_system );
}

static void set_zone_planet_link( ZONE_DATA *zone, PLANET_DATA *planet )
{
   if( !zone || zone->planet == planet )
      return;

   if( zone->planet )
      UNLINK( zone, zone->planet->first_zone, zone->planet->last_zone,
              next_in_planet, prev_in_planet );

   zone->next_in_planet = NULL;
   zone->prev_in_planet = NULL;
   zone->planet = planet;

   if( planet )
      LINK( zone, planet->first_zone, planet->last_zone, next_in_planet, prev_in_planet );
}

static bool sector_accepts_system( const SECTOR_DATA *sector, const SPACE_DATA *starsystem )
{
   PLANET_DATA *planet;

   if( !sector )
      return FALSE;

   for( planet = sector->first_planet; planet; planet = planet->next_in_sector )
      if( planet->starsystem != starsystem )
         return FALSE;

   return TRUE;
}

static bool zone_accepts_planet( const ZONE_DATA *zone, const PLANET_DATA *planet )
{
   AREA_DATA *area;

   if( !zone )
      return FALSE;

   for( area = zone->first_area; area; area = area->next_on_zone )
      if( area->planet != planet )
         return FALSE;

   return TRUE;
}

static bool sector_filename_exists( const char *filename, const SECTOR_DATA *ignore )
{
   SECTOR_DATA *sector;

   for( sector = first_sector; sector; sector = sector->next )
      if( sector != ignore && sector->filename && !str_cmp( sector->filename, filename ) )
         return TRUE;
   return FALSE;
}

static bool zone_filename_exists( const char *filename, const ZONE_DATA *ignore )
{
   ZONE_DATA *zone;

   for( zone = first_zone; zone; zone = zone->next )
      if( zone != ignore && zone->filename && !str_cmp( zone->filename, filename ) )
         return TRUE;
   return FALSE;
}

void do_sectors( CHAR_DATA *ch, const char *argument )
{
   SECTOR_DATA *sector;

   ( void )argument;
   send_to_char( "&WSectors\r\n", ch );
   send_to_char( "&zName                          Filename                 Starsystem              Planets\r\n", ch );

   for( sector = first_sector; sector; sector = sector->next )
      ch_printf( ch, "&w%-29.29s %-24.24s %-23.23s %d\r\n",
                 sector->name ? sector->name : "",
                 sector->filename ? sector->filename : "",
                 sector->starsystem && sector->starsystem->name ? sector->starsystem->name : "(none)",
                 sector_planet_count( sector ) );
}

void do_showsector( CHAR_DATA *ch, const char *argument )
{
   SECTOR_DATA *sector;
   PLANET_DATA *planet;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: showsector <name|filename>\r\n", ch );
      return;
   }

   sector = find_sector_selector( argument );
   if( !sector )
   {
      send_to_char( "No unique sector matched that name.\r\n", ch );
      return;
   }

   ch_printf( ch, "&WSector: &w%s\r\n", sector->name ? sector->name : "" );
   ch_printf( ch, "&WFilename: &w%s\r\n", sector->filename ? sector->filename : "" );
   ch_printf( ch, "&WStarsystem: &w%s\r\n",
              sector->starsystem && sector->starsystem->name ? sector->starsystem->name : "(none)" );
   ch_printf( ch, "&WFlags: &w%d\r\n", sector->flags );
   send_to_char( "&WPlanets:\r\n", ch );
   for( planet = sector->first_planet; planet; planet = planet->next_in_sector )
      ch_printf( ch, "  &w%s\r\n", planet->name ? planet->name : "" );
}

void do_makesector( CHAR_DATA *ch, const char *argument )
{
   SECTOR_DATA *sector;
   char filename[MAX_INPUT_LENGTH];

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: makesector <name>\r\n", ch );
      return;
   }

   if( get_sector( argument ) )
   {
      send_to_char( "A sector with that name already exists.\r\n", ch );
      return;
   }

   geography_make_filename( argument, filename, sizeof( filename ) );
   if( !geography_filename_valid( filename ) )
   {
      send_to_char( "That name cannot produce a valid sector filename.\r\n", ch );
      return;
   }
   if( sector_filename_exists( filename, NULL ) )
   {
      send_to_char( "A sector with that generated filename already exists.\r\n", ch );
      return;
   }

   CREATE( sector, SECTOR_DATA, 1 );
   sector->name = STRALLOC( argument );
   sector->filename = strdup( filename );
   LINK( sector, first_sector, last_sector, next, prev );

   save_sector( sector );
   write_sector_list();
   ch_printf( ch, "Created sector '%s' (%s).\r\n", sector->name, sector->filename );
}

void do_setsector( CHAR_DATA *ch, const char *argument )
{
   char selector[MAX_INPUT_LENGTH];
   char field[MAX_INPUT_LENGTH];
   SECTOR_DATA *sector;

   argument = one_argument( argument, selector );
   argument = one_argument( argument, field );

   if( selector[0] == '\0' || field[0] == '\0' )
   {
      send_to_char( "Usage: setsector <name|filename> <name|filename|starsystem|flags|planet|removeplanet> <value>\r\n", ch );
      return;
   }

   sector = find_sector_selector( selector );
   if( !sector )
   {
      send_to_char( "No unique sector matched that name.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "name" ) )
   {
      SECTOR_DATA *other;

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "You must choose a name.\r\n", ch );
         return;
      }
      other = get_sector( argument );
      if( other && other != sector )
      {
         send_to_char( "A sector with that name already exists.\r\n", ch );
         return;
      }
      STRFREE( sector->name );
      sector->name = STRALLOC( argument );
      save_sector( sector );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "filename" ) )
   {
      char oldpath[512];
      char newpath[512];

      if( !geography_filename_valid( argument ) )
      {
         send_to_char( "Filenames may contain only letters, numbers, '_' and '-'.\r\n", ch );
         return;
      }
      if( sector_filename_exists( argument, sector ) )
      {
         send_to_char( "A sector with that filename already exists.\r\n", ch );
         return;
      }
      if( sector->filename && !str_cmp( sector->filename, argument ) )
      {
         send_to_char( "That is already the sector filename.\r\n", ch );
         return;
      }

      snprintf( oldpath, sizeof( oldpath ), "%s%s", SECTOR_DIR, sector->filename ? sector->filename : "" );
      snprintf( newpath, sizeof( newpath ), "%s%s", SECTOR_DIR, argument );
      if( sector->filename && sector->filename[0] != '\0' && rename( oldpath, newpath ) != 0 )
      {
         ch_printf( ch, "Unable to rename sector file: %s\r\n", strerror( errno ) );
         return;
      }

      DISPOSE( sector->filename );
      sector->filename = strdup( argument );
      save_sector( sector );
      write_sector_list();
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "flags" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "Flags must be numeric.\r\n", ch );
         return;
      }
      sector->flags = atoi( argument );
      save_sector( sector );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "starsystem" ) )
   {
      SPACE_DATA *starsystem = NULL;

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Choose a starsystem name, or 'none'.\r\n", ch );
         return;
      }
      if( str_cmp( argument, "none" ) )
      {
         starsystem = starsystem_from_name( argument );
         if( !starsystem )
         {
            send_to_char( "No such starsystem.\r\n", ch );
            return;
         }
      }
      if( !sector_accepts_system( sector, starsystem ) )
      {
         send_to_char( "Every planet in this sector must belong to that starsystem first.\r\n", ch );
         return;
      }

      set_sector_system_link( sector, starsystem );
      save_sector( sector );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "planet" ) )
   {
      PLANET_DATA *planet;
      SECTOR_DATA *oldsector;

      planet = get_planet( argument );
      if( !planet )
      {
         send_to_char( "No such planet.\r\n", ch );
         return;
      }
      if( sector->starsystem && planet->starsystem != sector->starsystem )
      {
         send_to_char( "That planet belongs to a different starsystem.\r\n", ch );
         return;
      }
      if( !sector->starsystem && !planet->starsystem )
      {
         send_to_char( "Assign the planet to a starsystem before placing it in a sector.\r\n", ch );
         return;
      }
      if( !sector->starsystem )
         set_sector_system_link( sector, planet->starsystem );

      if( planet->sector == sector )
      {
         send_to_char( "That planet is already in this sector.\r\n", ch );
         return;
      }

      oldsector = planet->sector;
      if( oldsector )
      {
         UNLINK( planet, oldsector->first_planet, oldsector->last_planet, next_in_sector, prev_in_sector );
         planet->next_in_sector = NULL;
         planet->prev_in_sector = NULL;
         planet->sector = NULL;
         save_sector( oldsector );
      }

      planet->sector = sector;
      LINK( planet, sector->first_planet, sector->last_planet, next_in_sector, prev_in_sector );
      save_sector( sector );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "removeplanet" ) )
   {
      PLANET_DATA *planet = get_planet( argument );

      if( !planet || planet->sector != sector )
      {
         send_to_char( "That planet is not in this sector.\r\n", ch );
         return;
      }

      UNLINK( planet, sector->first_planet, sector->last_planet, next_in_sector, prev_in_sector );
      planet->next_in_sector = NULL;
      planet->prev_in_sector = NULL;
      planet->sector = NULL;
      save_sector( sector );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   send_to_char( "Unknown sector field.\r\n", ch );
}

void do_showplanet( CHAR_DATA *ch, const char *argument )
{
   PLANET_DATA *planet;
   ZONE_DATA *zone;
   AREA_DATA *area;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: showplanet <name|filename>\r\n", ch );
      return;
   }

   planet = find_planet_selector( argument );
   if( !planet )
   {
      send_to_char( "No unique planet matched that name.\r\n", ch );
      return;
   }

   ch_printf( ch, "&WPlanet: &w%s\r\n", planet->name ? planet->name : "" );
   ch_printf( ch, "&WFilename: &w%s\r\n", planet->filename ? planet->filename : "" );
   ch_printf( ch, "&WStarsystem: &w%s\r\n",
              planet->starsystem && planet->starsystem->name ? planet->starsystem->name : "(none)" );
   ch_printf( ch, "&WSector: &w%s\r\n",
              planet->sector && planet->sector->name ? planet->sector->name : "(none)" );
   ch_printf( ch, "&WGoverned by: &w%s\r\n",
              planet->governed_by && planet->governed_by->name ? planet->governed_by->name : "(none)" );
   ch_printf( ch, "&WBase value: &w%ld\r\n", planet->base_value );
   ch_printf( ch, "&WPopulation: &w%d\r\n", planet->population );
   ch_printf( ch, "&WPopular support: &w%.2f\r\n", planet->pop_support );
   ch_printf( ch, "&WFlags: &w%d\r\n", ( int )planet->flags );

   send_to_char( "&WZones:\r\n", ch );
   for( zone = planet->first_zone; zone; zone = zone->next_in_planet )
      ch_printf( ch, "  &w%s (%s)\r\n",
                 zone->name ? zone->name : "",
                 zone->filename ? zone->filename : "" );

   send_to_char( "&WAreas:\r\n", ch );
   for( area = planet->first_area; area; area = area->next_on_planet )
      ch_printf( ch, "  &w%s (%s)%s%s\r\n",
                 area->name ? area->name : "",
                 area->filename ? area->filename : "",
                 area->zone && area->zone->name ? " - " : "",
                 area->zone && area->zone->name ? area->zone->name : "" );
}

void do_geozones( CHAR_DATA *ch, const char *argument )
{
   ZONE_DATA *zone;

   ( void )argument;
   send_to_char( "&WZones\r\n", ch );
   send_to_char( "&zName                          Filename                 Planet                  Areas\r\n", ch );

   for( zone = first_zone; zone; zone = zone->next )
      ch_printf( ch, "&w%-29.29s %-24.24s %-23.23s %d\r\n",
                 zone->name ? zone->name : "",
                 zone->filename ? zone->filename : "",
                 zone->planet && zone->planet->name ? zone->planet->name : "(none)",
                 zone_area_count( zone ) );
}

void do_showzone( CHAR_DATA *ch, const char *argument )
{
   ZONE_DATA *zone;
   AREA_DATA *area;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: showzone <name|filename>\r\n", ch );
      return;
   }

   zone = find_zone_selector( argument );
   if( !zone )
   {
      send_to_char( "No unique zone matched that name.\r\n", ch );
      return;
   }

   ch_printf( ch, "&WZone: &w%s\r\n", zone->name ? zone->name : "" );
   ch_printf( ch, "&WFilename: &w%s\r\n", zone->filename ? zone->filename : "" );
   ch_printf( ch, "&WPlanet: &w%s\r\n", zone->planet && zone->planet->name ? zone->planet->name : "(none)" );
   ch_printf( ch, "&WFlags: &w%d\r\n", zone->flags );
   send_to_char( "&WAreas:\r\n", ch );
   for( area = zone->first_area; area; area = area->next_on_zone )
      ch_printf( ch, "  &w%s (%s)\r\n",
                 area->name ? area->name : "",
                 area->filename ? area->filename : "" );
}

void do_makezone( CHAR_DATA *ch, const char *argument )
{
   ZONE_DATA *zone;
   char filename[MAX_INPUT_LENGTH];

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: makezone <name>\r\n", ch );
      return;
   }

   if( get_zone( argument ) )
   {
      send_to_char( "A zone with that name already exists.\r\n", ch );
      return;
   }

   geography_make_filename( argument, filename, sizeof( filename ) );
   if( !geography_filename_valid( filename ) )
   {
      send_to_char( "That name cannot produce a valid zone filename.\r\n", ch );
      return;
   }
   if( zone_filename_exists( filename, NULL ) )
   {
      send_to_char( "A zone with that generated filename already exists.\r\n", ch );
      return;
   }

   CREATE( zone, ZONE_DATA, 1 );
   zone->name = STRALLOC( argument );
   zone->filename = strdup( filename );
   LINK( zone, first_zone, last_zone, next, prev );

   save_zone( zone );
   write_zone_list();
   ch_printf( ch, "Created zone '%s' (%s).\r\n", zone->name, zone->filename );
}

void do_setzone( CHAR_DATA *ch, const char *argument )
{
   char selector[MAX_INPUT_LENGTH];
   char field[MAX_INPUT_LENGTH];
   ZONE_DATA *zone;

   argument = one_argument( argument, selector );
   argument = one_argument( argument, field );

   if( selector[0] == '\0' || field[0] == '\0' )
   {
      send_to_char( "Usage: setzone <name|filename> <name|filename|planet|flags|area|removearea> <value>\r\n", ch );
      return;
   }

   zone = find_zone_selector( selector );
   if( !zone )
   {
      send_to_char( "No unique zone matched that name.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "name" ) )
   {
      ZONE_DATA *other;

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "You must choose a name.\r\n", ch );
         return;
      }
      other = get_zone( argument );
      if( other && other != zone )
      {
         send_to_char( "A zone with that name already exists.\r\n", ch );
         return;
      }
      STRFREE( zone->name );
      zone->name = STRALLOC( argument );
      save_zone( zone );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "filename" ) )
   {
      char oldpath[512];
      char newpath[512];

      if( !geography_filename_valid( argument ) )
      {
         send_to_char( "Filenames may contain only letters, numbers, '_' and '-'.\r\n", ch );
         return;
      }
      if( zone_filename_exists( argument, zone ) )
      {
         send_to_char( "A zone with that filename already exists.\r\n", ch );
         return;
      }
      if( zone->filename && !str_cmp( zone->filename, argument ) )
      {
         send_to_char( "That is already the zone filename.\r\n", ch );
         return;
      }

      snprintf( oldpath, sizeof( oldpath ), "%s%s", ZONE_DIR, zone->filename ? zone->filename : "" );
      snprintf( newpath, sizeof( newpath ), "%s%s", ZONE_DIR, argument );
      if( zone->filename && zone->filename[0] != '\0' && rename( oldpath, newpath ) != 0 )
      {
         ch_printf( ch, "Unable to rename zone file: %s\r\n", strerror( errno ) );
         return;
      }

      DISPOSE( zone->filename );
      zone->filename = strdup( argument );
      save_zone( zone );
      write_zone_list();
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "flags" ) )
   {
      if( !is_number( argument ) )
      {
         send_to_char( "Flags must be numeric.\r\n", ch );
         return;
      }
      zone->flags = atoi( argument );
      save_zone( zone );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "planet" ) )
   {
      PLANET_DATA *planet = NULL;

      if( !argument || argument[0] == '\0' )
      {
         send_to_char( "Choose a planet name, or 'none'.\r\n", ch );
         return;
      }
      if( str_cmp( argument, "none" ) )
      {
         planet = get_planet( argument );
         if( !planet )
         {
            send_to_char( "No such planet.\r\n", ch );
            return;
         }
      }
      if( !zone_accepts_planet( zone, planet ) )
      {
         send_to_char( "Every area in this zone must belong to that planet first.\r\n", ch );
         return;
      }

      set_zone_planet_link( zone, planet );
      save_zone( zone );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "area" ) )
   {
      AREA_DATA *area;
      ZONE_DATA *oldzone;

      area = find_area_selector( argument );
      if( !area )
      {
         send_to_char( "No such area. Use its exact name or filename.\r\n", ch );
         return;
      }
      if( !area->planet )
      {
         send_to_char( "Assign that area to a planet before placing it in a zone.\r\n", ch );
         return;
      }
      if( zone->planet && zone->planet != area->planet )
      {
         send_to_char( "That area belongs to a different planet.\r\n", ch );
         return;
      }
      if( !zone->planet )
         set_zone_planet_link( zone, area->planet );

      if( area->zone == zone )
      {
         send_to_char( "That area is already in this zone.\r\n", ch );
         return;
      }

      oldzone = area->zone;
      if( oldzone )
      {
         UNLINK( area, oldzone->first_area, oldzone->last_area, next_on_zone, prev_on_zone );
         area->next_on_zone = NULL;
         area->prev_on_zone = NULL;
         area->zone = NULL;
         save_zone( oldzone );
      }

      area->zone = zone;
      LINK( area, zone->first_area, zone->last_area, next_on_zone, prev_on_zone );
      save_zone( zone );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   if( !str_cmp( field, "removearea" ) )
   {
      AREA_DATA *area = find_area_selector( argument );

      if( !area || area->zone != zone )
      {
         send_to_char( "That area is not in this zone.\r\n", ch );
         return;
      }

      UNLINK( area, zone->first_area, zone->last_area, next_on_zone, prev_on_zone );
      area->next_on_zone = NULL;
      area->prev_on_zone = NULL;
      area->zone = NULL;
      save_zone( zone );
      send_to_char( "Done.\r\n", ch );
      return;
   }

   send_to_char( "Unknown zone field.\r\n", ch );
}

static bool geography_command_exists( const char *name )
{
   CMDTYPE *command;
   int bucket;

   if( !name || name[0] == '\0' )
      return FALSE;

   bucket = LOWER( name[0] ) % 126;
   for( command = command_hash[bucket]; command; command = command->next )
      if( command->name && !str_cmp( command->name, name ) )
         return TRUE;

   return FALSE;
}

static void geography_add_command( const char *name, DO_FUN *function, const char *function_name, int level )
{
   CMDTYPE *command;

   if( geography_command_exists( name ) )
      return;

   CREATE( command, CMDTYPE, 1 );
   command->name = strdup( name );
   command->do_fun = function;
   command->fun_name = strdup( function_name );
   command->position = 0;
   command->level = level;
   command->log = 0;
   add_command( command );
}

/*
 * Preserve the existing command file as authority, then ensure the geography
 * commands exist. If save_commands() later persists them into commands.dat,
 * the duplicate guard keeps subsequent boots idempotent.
 */
void load_commands( void )
{
   legacy_load_commands();

   geography_add_command( "sectors", do_sectors, "do_sectors", 1 );
   geography_add_command( "showsector", do_showsector, "do_showsector", 1 );
   geography_add_command( "makesector", do_makesector, "do_makesector", LEVEL_SUPREME );
   geography_add_command( "setsector", do_setsector, "do_setsector", LEVEL_SUPREME );
   geography_add_command( "showplanet", do_showplanet, "do_showplanet", 1 );
   geography_add_command( "geozones", do_geozones, "do_geozones", 1 );
   geography_add_command( "showzone", do_showzone, "do_showzone", 1 );
   geography_add_command( "makezone", do_makezone, "do_makezone", LEVEL_SUPREME );
   geography_add_command( "setzone", do_setzone, "do_setzone", LEVEL_SUPREME );
}
