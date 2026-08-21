/***************************************************************************
 * Zero Point geography command UX layer.
 *
 * Keeps the persisted geography/builders in geography_commands.c while
 * providing forgiving human-facing show selectors and Planet show parity.
 ***************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "mud.h"

/* geography_commands.c retains its command-loader layer behind this symbol. */
void geography_load_commands( void );

DECLARE_DO_FUN( do_geo_showsector );
DECLARE_DO_FUN( do_geo_showplanet );
DECLARE_DO_FUN( do_geo_showzone );

static void geography_normalize_selector( const char *input, char *output, size_t size )
{
   size_t out = 0;
   const unsigned char *p;

   if( !output || size == 0 )
      return;

   output[0] = '\0';
   if( !input )
      return;

   for( p = ( const unsigned char * )input; *p && out + 1 < size; ++p )
      if( isalnum( *p ) )
         output[out++] = ( char )tolower( *p );

   output[out] = '\0';
}

static bool geography_one_edit_apart( const char *left, const char *right )
{
   size_t left_len;
   size_t right_len;
   size_t i = 0;
   size_t j = 0;
   int edits = 0;

   if( !left || !right )
      return FALSE;

   left_len = strlen( left );
   right_len = strlen( right );

   if( left_len > right_len + 1 || right_len > left_len + 1 )
      return FALSE;

   while( i < left_len && j < right_len )
   {
      if( left[i] == right[j] )
      {
         ++i;
         ++j;
         continue;
      }

      if( ++edits > 1 )
         return FALSE;

      if( left_len > right_len )
         ++i;
      else if( right_len > left_len )
         ++j;
      else
      {
         ++i;
         ++j;
      }
   }

   if( i < left_len || j < right_len )
      ++edits;

   return edits <= 1;
}

static int geography_selector_score( const char *selector, const char *candidate )
{
   char wanted[MAX_INPUT_LENGTH];
   char available[MAX_INPUT_LENGTH];

   if( !selector || !candidate )
      return 0;

   if( !str_cmp( selector, candidate ) )
      return 5;

   geography_normalize_selector( selector, wanted, sizeof( wanted ) );
   geography_normalize_selector( candidate, available, sizeof( available ) );

   if( wanted[0] == '\0' || available[0] == '\0' )
      return 0;

   if( !strcmp( wanted, available ) )
      return 4;

   if( strstr( available, wanted ) )
      return 3;

   if( geography_one_edit_apart( wanted, available ) )
      return 2;

   return 0;
}

static SECTOR_DATA *find_sector_friendly( const char *selector )
{
   SECTOR_DATA *sector;
   SECTOR_DATA *best = NULL;
   int best_score = 0;
   bool ambiguous = FALSE;

   if( !selector || selector[0] == '\0' )
      return NULL;

   for( sector = first_sector; sector; sector = sector->next )
   {
      int score = geography_selector_score( selector, sector->name );
      int filename_score = geography_selector_score( selector, sector->filename );

      if( filename_score > score )
         score = filename_score;

      if( score > best_score )
      {
         best = sector;
         best_score = score;
         ambiguous = FALSE;
      }
      else if( score > 0 && score == best_score && best != sector )
         ambiguous = TRUE;
   }

   return ambiguous ? NULL : best;
}

static PLANET_DATA *find_planet_friendly( const char *selector )
{
   PLANET_DATA *planet;
   PLANET_DATA *best = NULL;
   int best_score = 0;
   bool ambiguous = FALSE;

   if( !selector || selector[0] == '\0' )
      return NULL;

   for( planet = first_planet; planet; planet = planet->next )
   {
      int score = geography_selector_score( selector, planet->name );
      int filename_score = geography_selector_score( selector, planet->filename );

      if( filename_score > score )
         score = filename_score;

      if( score > best_score )
      {
         best = planet;
         best_score = score;
         ambiguous = FALSE;
      }
      else if( score > 0 && score == best_score && best != planet )
         ambiguous = TRUE;
   }

   return ambiguous ? NULL : best;
}

static ZONE_DATA *find_zone_friendly( const char *selector )
{
   ZONE_DATA *zone;
   ZONE_DATA *best = NULL;
   int best_score = 0;
   bool ambiguous = FALSE;

   if( !selector || selector[0] == '\0' )
      return NULL;

   for( zone = first_zone; zone; zone = zone->next )
   {
      int score = geography_selector_score( selector, zone->name );
      int filename_score = geography_selector_score( selector, zone->filename );

      if( filename_score > score )
         score = filename_score;

      if( score > best_score )
      {
         best = zone;
         best_score = score;
         ambiguous = FALSE;
      }
      else if( score > 0 && score == best_score && best != zone )
         ambiguous = TRUE;
   }

   return ambiguous ? NULL : best;
}

void do_geo_showsector( CHAR_DATA *ch, const char *argument )
{
   SECTOR_DATA *sector;
   PLANET_DATA *planet;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: showsector <name|filename|partial name>\r\n", ch );
      return;
   }

   sector = find_sector_friendly( argument );
   if( !sector )
   {
      send_to_char( "No unique sector match. Use 'sectors' to see the available names.\r\n", ch );
      return;
   }

   ch_printf( ch, "&WSector: &w%s\r\n", sector->name ? sector->name : "" );
   ch_printf( ch, "&WFilename: &w%s\r\n", sector->filename ? sector->filename : "" );
   ch_printf( ch, "&WStarsystem: &w%s\r\n",
              sector->starsystem && sector->starsystem->name ? sector->starsystem->name : "(none)" );
   ch_printf( ch, "&WFlags: &w%d\r\n", sector->flags );
   send_to_char( "&WPlanets:\r\n", ch );
   if( !sector->first_planet )
      send_to_char( "  &w(none)\r\n", ch );
   for( planet = sector->first_planet; planet; planet = planet->next_in_sector )
      ch_printf( ch, "  &w%s\r\n", planet->name ? planet->name : "" );
}

void do_geo_showplanet( CHAR_DATA *ch, const char *argument )
{
   PLANET_DATA *planet;
   ZONE_DATA *zone;
   AREA_DATA *area;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: showplanet <name|filename|partial name>\r\n", ch );
      return;
   }

   planet = find_planet_friendly( argument );
   if( !planet )
   {
      send_to_char( "No unique planet match. Use 'planets' to see the available names.\r\n", ch );
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
   ch_printf( ch, "&WPopulation support: &w%.2f\r\n", planet->pop_support );
   ch_printf( ch, "&WFlags: &w%d\r\n", ( int )planet->flags );

   send_to_char( "&WZones:\r\n", ch );
   if( !planet->first_zone )
      send_to_char( "  &w(none)\r\n", ch );
   for( zone = planet->first_zone; zone; zone = zone->next_in_planet )
      ch_printf( ch, "  &w%s (%s)\r\n",
                 zone->name ? zone->name : "",
                 zone->filename ? zone->filename : "" );

   send_to_char( "&WAreas:\r\n", ch );
   if( !planet->first_area )
      send_to_char( "  &w(none)\r\n", ch );
   for( area = planet->first_area; area; area = area->next_on_planet )
      ch_printf( ch, "  &w%s (%s)\r\n",
                 area->name ? area->name : "",
                 area->filename ? area->filename : "" );
}

void do_geo_showzone( CHAR_DATA *ch, const char *argument )
{
   ZONE_DATA *zone;
   AREA_DATA *area;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: showzone <name|filename|partial name>\r\n", ch );
      return;
   }

   zone = find_zone_friendly( argument );
   if( !zone )
   {
      send_to_char( "No unique zone match. Use 'geozones' to see the available names.\r\n", ch );
      return;
   }

   ch_printf( ch, "&WZone: &w%s\r\n", zone->name ? zone->name : "" );
   ch_printf( ch, "&WFilename: &w%s\r\n", zone->filename ? zone->filename : "" );
   ch_printf( ch, "&WPlanet: &w%s\r\n", zone->planet && zone->planet->name ? zone->planet->name : "(none)" );
   ch_printf( ch, "&WFlags: &w%d\r\n", zone->flags );
   send_to_char( "&WAreas:\r\n", ch );
   if( !zone->first_area )
      send_to_char( "  &w(none)\r\n", ch );
   for( area = zone->first_area; area; area = area->next_on_zone )
      ch_printf( ch, "  &w%s (%s)\r\n",
                 area->name ? area->name : "",
                 area->filename ? area->filename : "" );
}

static CMDTYPE *geography_find_command( const char *name )
{
   CMDTYPE *command;
   int bucket;

   if( !name || name[0] == '\0' )
      return NULL;

   bucket = LOWER( name[0] ) % 126;
   for( command = command_hash[bucket]; command; command = command->next )
      if( command->name && !str_cmp( command->name, name ) )
         return command;

   return NULL;
}

static void geography_bind_show_command( const char *name, DO_FUN *function,
                                         const char *function_name, int level )
{
   CMDTYPE *command = geography_find_command( name );

   if( command )
   {
      command->do_fun = function;
      return;
   }

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
 * Preserve the geography builder loader, then bind the final human-facing
 * show commands. This keeps legacy commands/data intact while giving every
 * hierarchy level a consistent inspection command.
 */
void load_commands( void )
{
   geography_load_commands();

   geography_bind_show_command( "showsector", do_geo_showsector, "do_geo_showsector", 1 );
   geography_bind_show_command( "showplanet", do_geo_showplanet, "do_geo_showplanet", 1 );
   geography_bind_show_command( "showzone", do_geo_showzone, "do_geo_showzone", 1 );
}
