/***************************************************************************
 * Zero Point hierarchy-aware area builder command layer.
 *
 * Keeps the inherited aset implementation intact while making installed-area
 * filename changes safe for subdirectory-backed geography such as Earth/*.
 ***************************************************************************/

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mud.h"

DECLARE_DO_FUN( legacy_do_aset );
void write_area_list( void );

static bool area_filename_component_valid( const char *component )
{
   const unsigned char *p;

   if( !component || component[0] == '\0' )
      return FALSE;
   if( !strcmp( component, "." ) || !strcmp( component, ".." ) )
      return FALSE;

   for( p = ( const unsigned char * )component; *p; ++p )
      if( !isalnum( *p ) && *p != '_' && *p != '-' && *p != '.' )
         return FALSE;

   return TRUE;
}

static bool area_relative_path_valid( const char *path )
{
   char copy[MAX_INPUT_LENGTH];
   char *cursor;
   char *slash;

   if( !path || path[0] == '\0' )
      return FALSE;
   if( path[0] == '/' || path[0] == '\\' || strchr( path, '\\' ) || strchr( path, ':' ) )
      return FALSE;
   if( strlen( path ) >= sizeof( copy ) )
      return FALSE;

   strlcpy( copy, path, sizeof( copy ) );
   cursor = copy;

   for( ;; )
   {
      slash = strchr( cursor, '/' );
      if( slash )
         *slash = '\0';

      if( !area_filename_component_valid( cursor ) )
         return FALSE;

      if( !slash )
         break;
      cursor = slash + 1;
   }

   return TRUE;
}

static AREA_DATA *find_area_by_filename( const char *filename, bool *prototype )
{
   AREA_DATA *area;

   if( prototype )
      *prototype = FALSE;

   for( area = first_area; area; area = area->next )
      if( area->filename && !str_cmp( area->filename, filename ) )
         return area;

   for( area = first_build; area; area = area->next )
      if( area->filename && !str_cmp( area->filename, filename ) )
      {
         if( prototype )
            *prototype = TRUE;
         return area;
      }

   return NULL;
}

static bool area_filename_in_use( const AREA_DATA *ignore, const char *filename )
{
   AREA_DATA *area;

   for( area = first_area; area; area = area->next )
      if( area != ignore && area->filename && !str_cmp( area->filename, filename ) )
         return TRUE;

   for( area = first_build; area; area = area->next )
      if( area != ignore && area->filename && !str_cmp( area->filename, filename ) )
         return TRUE;

   return FALSE;
}

static void area_resolve_target_path( const char *current, const char *requested,
                                      char *target, size_t size )
{
   const char *slash;
   size_t prefix;

   if( !target || size == 0 )
      return;

   target[0] = '\0';
   if( !requested )
      return;

   if( strchr( requested, '/' ) || !current )
   {
      strlcpy( target, requested, size );
      return;
   }

   slash = strrchr( current, '/' );
   if( !slash )
   {
      strlcpy( target, requested, size );
      return;
   }

   prefix = ( size_t )( slash - current + 1 );
   if( prefix >= size )
      return;

   memcpy( target, current, prefix );
   target[prefix] = '\0';
   strlcat( target, requested, size );
}

void do_aset( CHAR_DATA *ch, const char *argument )
{
   const char *remainder;
   char area_name[MAX_INPUT_LENGTH];
   char field[MAX_INPUT_LENGTH];
   char target[MAX_INPUT_LENGTH];
   AREA_DATA *area;
   bool prototype = FALSE;

   remainder = one_argument( argument, area_name );
   remainder = one_argument( remainder, field );

   if( str_cmp( field, "filename" ) )
   {
      legacy_do_aset( ch, argument );
      return;
   }

   if( area_name[0] == '\0' || !remainder || remainder[0] == '\0' )
   {
      legacy_do_aset( ch, argument );
      return;
   }

   area = find_area_by_filename( area_name, &prototype );
   if( !area )
   {
      send_to_char( "Area not found.\r\n", ch );
      return;
   }

   if( prototype )
   {
      send_to_char( "You should only change the filename of installed areas.\r\n", ch );
      return;
   }

   area_resolve_target_path( area->filename, remainder, target, sizeof( target ) );

   if( target[0] == '\0' || !area_relative_path_valid( target ) )
   {
      send_to_char( "Area filenames must be safe relative paths and may not contain '..', '\\', drive prefixes, or invalid characters.\r\n", ch );
      return;
   }

   if( !str_cmp( area->filename, target ) )
   {
      send_to_char( "That is already the area's filename.\r\n", ch );
      return;
   }

   if( area_filename_in_use( area, target ) || access( target, F_OK ) == 0 )
   {
      send_to_char( "An area file with that name already exists.\r\n", ch );
      return;
   }

   if( rename( area->filename, target ) != 0 )
   {
      ch_printf( ch, "Unable to rename area file: %s\r\n", strerror( errno ) );
      return;
   }

   DISPOSE( area->filename );
   area->filename = strdup( target );

   write_area_list();
   if( area->planet )
      save_planet( area->planet );
   if( area->zone )
      save_zone( area->zone );

   ch_printf( ch, "Area renamed to %s.\r\n", area->filename );
}
