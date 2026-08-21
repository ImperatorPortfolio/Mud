/***************************************************************************
 * Zero Point geography command UX layer.
 *
 * Keeps persisted geography/builders in geography_commands.c while
 * presenting Starsystems, Sectors, Planets and Zones as player-facing
 * Galactic Database entries. Raw builder metadata is appended only for
 * immortal/admin characters.
 ***************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "mud.h"

/* geography_commands.c retains its command-loader layer behind this symbol. */
void geography_load_commands( void );

DECLARE_DO_FUN( do_geo_showstarsystem );
DECLARE_DO_FUN( do_geo_showsector );
DECLARE_DO_FUN( do_geo_showplanet );
DECLARE_DO_FUN( do_geo_showzone );

static bool geography_is_admin( const CHAR_DATA *ch )
{
   return ch && ch->top_level >= LEVEL_IMMORTAL;
}

static void database_header( CHAR_DATA *ch, const char *kind, const char *name )
{
   send_to_char( "&z========================================================================\r\n", ch );
   ch_printf( ch, "&C GALACTIC DATABASE &z// &W%s\r\n", kind ? kind : "ENTRY" );
   send_to_char( "&z------------------------------------------------------------------------\r\n", ch );
   ch_printf( ch, "&W %s\r\n", name && name[0] != '\0' ? name : "Unknown" );
   send_to_char( "&z========================================================================\r\n", ch );
}

static void database_section( CHAR_DATA *ch, const char *title )
{
   ch_printf( ch, "\r\n&C[ %s ]\r\n", title );
}

static void database_field( CHAR_DATA *ch, const char *label, const char *value )
{
   ch_printf( ch, "&z  %-18.18s &w%s\r\n",
              label ? label : "",
              value && value[0] != '\0' ? value : "Unknown" );
}

static void database_none( CHAR_DATA *ch )
{
   send_to_char( "&z  > &wNone recorded\r\n", ch );
}

static void database_footer( CHAR_DATA *ch )
{
   send_to_char( "\r\n&z========================================================================\r\n", ch );
}

static void database_builder_section( CHAR_DATA *ch )
{
   send_to_char( "\r\n&Y[ BUILDER INFORMATION ]\r\n", ch );
}

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

static SPACE_DATA *find_starsystem_friendly( const char *selector )
{
   SPACE_DATA *starsystem;
   SPACE_DATA *best = NULL;
   int best_score = 0;
   bool ambiguous = FALSE;

   if( !selector || selector[0] == '\0' )
      return NULL;

   for( starsystem = first_starsystem; starsystem; starsystem = starsystem->next )
   {
      int score = geography_selector_score( selector, starsystem->name );
      int filename_score = geography_selector_score( selector, starsystem->filename );

      if( filename_score > score )
         score = filename_score;

      if( score > best_score )
      {
         best = starsystem;
         best_score = score;
         ambiguous = FALSE;
      }
      else if( score > 0 && score == best_score && best != starsystem )
         ambiguous = TRUE;
   }

   return ambiguous ? NULL : best;
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

static int starsystem_sector_count( const SPACE_DATA *starsystem )
{
   SECTOR_DATA *sector;
   int count = 0;

   if( !starsystem )
      return 0;

   for( sector = starsystem->first_sector; sector; sector = sector->next_in_system )
      ++count;

   return count;
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

static int planet_zone_count( const PLANET_DATA *planet )
{
   ZONE_DATA *zone;
   int count = 0;

   if( !planet )
      return 0;

   for( zone = planet->first_zone; zone; zone = zone->next_in_planet )
      ++count;

   return count;
}

static int planet_area_count( const PLANET_DATA *planet )
{
   AREA_DATA *area;
   int count = 0;

   if( !planet )
      return 0;

   for( area = planet->first_area; area; area = area->next_on_planet )
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

static bool starsystem_has_linked_world( const SPACE_DATA *starsystem, const char *name )
{
   PLANET_DATA *planet;

   if( !starsystem || !name || name[0] == '\0' )
      return FALSE;

   for( planet = starsystem->first_planet; planet; planet = planet->next_in_system )
      if( planet->name && !str_cmp( planet->name, name ) )
         return TRUE;

   return FALSE;
}

static bool starsystem_legacy_world_duplicate( const SPACE_DATA *starsystem, int slot, const char *name )
{
   const char *legacy[3];
   int index;

   if( !starsystem || !name || name[0] == '\0' )
      return TRUE;

   legacy[0] = starsystem->planet1;
   legacy[1] = starsystem->planet2;
   legacy[2] = starsystem->planet3;

   if( starsystem_has_linked_world( starsystem, name ) )
      return TRUE;

   for( index = 0; index < slot; ++index )
      if( legacy[index] && legacy[index][0] != '\0' && !str_cmp( legacy[index], name ) )
         return TRUE;

   return FALSE;
}

static int starsystem_known_world_count( const SPACE_DATA *starsystem )
{
   PLANET_DATA *planet;
   const char *legacy[3];
   int count = 0;
   int slot;

   if( !starsystem )
      return 0;

   for( planet = starsystem->first_planet; planet; planet = planet->next_in_system )
      ++count;

   legacy[0] = starsystem->planet1;
   legacy[1] = starsystem->planet2;
   legacy[2] = starsystem->planet3;

   for( slot = 0; slot < 3; ++slot )
      if( legacy[slot] && legacy[slot][0] != '\0'
          && !starsystem_legacy_world_duplicate( starsystem, slot, legacy[slot] ) )
         ++count;

   return count;
}

static void show_starsystem_worlds( CHAR_DATA *ch, const SPACE_DATA *starsystem )
{
   PLANET_DATA *planet;
   const char *legacy[3];
   bool shown = FALSE;
   int slot;

   for( planet = starsystem->first_planet; planet; planet = planet->next_in_system )
   {
      ch_printf( ch, "&C  > &w%s\r\n", planet->name ? planet->name : "Unnamed world" );
      shown = TRUE;
   }

   legacy[0] = starsystem->planet1;
   legacy[1] = starsystem->planet2;
   legacy[2] = starsystem->planet3;

   for( slot = 0; slot < 3; ++slot )
      if( legacy[slot] && legacy[slot][0] != '\0'
          && !starsystem_legacy_world_duplicate( starsystem, slot, legacy[slot] ) )
      {
         ch_printf( ch, "&C  > &w%s\r\n", legacy[slot] );
         shown = TRUE;
      }

   if( !shown )
      database_none( ch );
}

static void show_starsystem_builder_info( CHAR_DATA *ch, const SPACE_DATA *starsystem )
{
   if( !geography_is_admin( ch ) )
      return;

   database_builder_section( ch );
   database_field( ch, "Filename", starsystem->filename );

   ch_printf( ch, "&z  %-18.18s &w%d, %d\r\n", "Map position", starsystem->xpos, starsystem->ypos );

   if( starsystem->star1 && starsystem->star1[0] != '\0' )
      ch_printf( ch, "&z  %-18.18s &w%d, %d, %d  (%s)\r\n",
                 "Star 1 coords", starsystem->s1x, starsystem->s1y, starsystem->s1z,
                 starsystem->star1 );

   if( starsystem->star2 && starsystem->star2[0] != '\0' )
      ch_printf( ch, "&z  %-18.18s &w%d, %d, %d  (%s)\r\n",
                 "Star 2 coords", starsystem->s2x, starsystem->s2y, starsystem->s2z,
                 starsystem->star2 );

   if( starsystem->planet1 && starsystem->planet1[0] != '\0' )
   {
      ch_printf( ch, "&z  %-18.18s &w%s @ %d, %d, %d  gravity %d\r\n",
                 "Legacy planet 1", starsystem->planet1,
                 starsystem->p1x, starsystem->p1y, starsystem->p1z, starsystem->gravityp1 );
      ch_printf( ch, "&z  %-18.18s &w%d / %d / %d\r\n",
                 "Legacy docks 1", starsystem->doc1a, starsystem->doc1b, starsystem->doc1c );
   }

   if( starsystem->planet2 && starsystem->planet2[0] != '\0' )
   {
      ch_printf( ch, "&z  %-18.18s &w%s @ %d, %d, %d  gravity %d\r\n",
                 "Legacy planet 2", starsystem->planet2,
                 starsystem->p2x, starsystem->p2y, starsystem->p2z, starsystem->gravityp2 );
      ch_printf( ch, "&z  %-18.18s &w%d / %d / %d\r\n",
                 "Legacy docks 2", starsystem->doc2a, starsystem->doc2b, starsystem->doc2c );
   }

   if( starsystem->planet3 && starsystem->planet3[0] != '\0' )
   {
      ch_printf( ch, "&z  %-18.18s &w%s @ %d, %d, %d  gravity %d\r\n",
                 "Legacy planet 3", starsystem->planet3,
                 starsystem->p3x, starsystem->p3y, starsystem->p3z, starsystem->gravityp3 );
      ch_printf( ch, "&z  %-18.18s &w%d / %d / %d\r\n",
                 "Legacy docks 3", starsystem->doc3a, starsystem->doc3b, starsystem->doc3c );
   }

   ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Crash setting", starsystem->crash );
}

void do_geo_showstarsystem( CHAR_DATA *ch, const char *argument )
{
   SPACE_DATA *starsystem;
   SECTOR_DATA *sector;

   if( !argument || argument[0] == '\0' )
   {
      send_to_char( "Usage: showstarsystem <name|filename|partial name>\r\n", ch );
      return;
   }

   starsystem = find_starsystem_friendly( argument );
   if( !starsystem )
   {
      send_to_char( "No unique star system match. Use 'starsystems' to see the available names.\r\n", ch );
      return;
   }

   database_header( ch, "STAR SYSTEM", starsystem->name );

   database_section( ch, "SYSTEM PROFILE" );
   database_field( ch, "Primary star", starsystem->star1 );
   if( starsystem->star2 && starsystem->star2[0] != '\0' )
      database_field( ch, "Secondary star", starsystem->star2 );
   ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Known sectors", starsystem_sector_count( starsystem ) );
   ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Known worlds", starsystem_known_world_count( starsystem ) );

   database_section( ch, "KNOWN SECTORS" );
   if( !starsystem->first_sector )
      database_none( ch );
   for( sector = starsystem->first_sector; sector; sector = sector->next_in_system )
      ch_printf( ch, "&C  > &w%s\r\n", sector->name ? sector->name : "Unnamed sector" );

   database_section( ch, "KNOWN WORLDS" );
   show_starsystem_worlds( ch, starsystem );

   show_starsystem_builder_info( ch, starsystem );
   database_footer( ch );
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

   database_header( ch, "SECTOR", sector->name );

   database_section( ch, "LOCATION" );
   database_field( ch, "Star system",
                   sector->starsystem && sector->starsystem->name ? sector->starsystem->name : NULL );
   ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Known worlds", sector_planet_count( sector ) );

   database_section( ch, "KNOWN WORLDS" );
   if( !sector->first_planet )
      database_none( ch );
   for( planet = sector->first_planet; planet; planet = planet->next_in_sector )
      ch_printf( ch, "&C  > &w%s\r\n", planet->name ? planet->name : "Unnamed world" );

   if( geography_is_admin( ch ) )
   {
      database_builder_section( ch );
      database_field( ch, "Filename", sector->filename );
      ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Flags", sector->flags );
   }

   database_footer( ch );
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

   database_header( ch, "PLANET", planet->name );

   database_section( ch, "LOCATION" );
   database_field( ch, "Star system",
                   planet->starsystem && planet->starsystem->name ? planet->starsystem->name : NULL );
   database_field( ch, "Sector",
                   planet->sector && planet->sector->name ? planet->sector->name : NULL );

   database_section( ch, "CIVIC PROFILE" );
   database_field( ch, "Government",
                   planet->governed_by && planet->governed_by->name ? planet->governed_by->name : NULL );
   ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Population", planet->population );
   ch_printf( ch, "&z  %-18.18s &w%.2f\r\n", "Public support", planet->pop_support );
   ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Known zones", planet_zone_count( planet ) );
   ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Known areas", planet_area_count( planet ) );

   database_section( ch, "KNOWN ZONES" );
   if( !planet->first_zone )
      database_none( ch );
   for( zone = planet->first_zone; zone; zone = zone->next_in_planet )
      ch_printf( ch, "&C  > &w%s\r\n", zone->name ? zone->name : "Unnamed zone" );

   database_section( ch, "KNOWN AREAS" );
   if( !planet->first_area )
      database_none( ch );
   for( area = planet->first_area; area; area = area->next_on_planet )
      ch_printf( ch, "&C  > &w%s%s%s\r\n",
                 area->name ? area->name : "Unnamed area",
                 area->zone && area->zone->name ? "  //  " : "",
                 area->zone && area->zone->name ? area->zone->name : "" );

   if( geography_is_admin( ch ) )
   {
      database_builder_section( ch );
      database_field( ch, "Filename", planet->filename );
      ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Flags", ( int )planet->flags );
      ch_printf( ch, "&z  %-18.18s &w%ld\r\n", "Base value", planet->base_value );
      ch_printf( ch, "&z  %-18.18s &w%.6f\r\n", "Raw support", planet->pop_support );
   }

   database_footer( ch );
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

   database_header( ch, "ZONE", zone->name );

   database_section( ch, "LOCATION" );
   database_field( ch, "Planet",
                   zone->planet && zone->planet->name ? zone->planet->name : NULL );
   ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Known areas", zone_area_count( zone ) );

   database_section( ch, "KNOWN AREAS" );
   if( !zone->first_area )
      database_none( ch );
   for( area = zone->first_area; area; area = area->next_on_zone )
      ch_printf( ch, "&C  > &w%s\r\n", area->name ? area->name : "Unnamed area" );

   if( geography_is_admin( ch ) )
   {
      database_builder_section( ch );
      database_field( ch, "Filename", zone->filename );
      ch_printf( ch, "&z  %-18.18s &w%d\r\n", "Flags", zone->flags );
   }

   database_footer( ch );
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
      command->level = level;
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
 * show commands. The database views are player-accessible; builder metadata
 * inside each view remains gated by LEVEL_IMMORTAL.
 */
void load_commands( void )
{
   geography_load_commands();

   geography_bind_show_command( "showstarsystem", do_geo_showstarsystem, "do_geo_showstarsystem", 1 );
   geography_bind_show_command( "showsector", do_geo_showsector, "do_geo_showsector", 1 );
   geography_bind_show_command( "showplanet", do_geo_showplanet, "do_geo_showplanet", 1 );
   geography_bind_show_command( "showzone", do_geo_showzone, "do_geo_showzone", 1 );
}
