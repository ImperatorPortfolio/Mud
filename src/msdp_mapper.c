/***************************************************************************
 * Zero Point - MSDP authoritative room publication
 *
 * Publishes the standard MSDP ROOM table whenever a character enters a
 * room. handler.c remains the owner of room membership; this translation
 * unit wraps char_to_room() so every movement path shares one publication
 * boundary.
 ***************************************************************************/

#include <stdio.h>
#include <string>
#include "mud.h"

#define ZP_TELOPT_MSDP       69
#define MSDP_VAR              1
#define MSDP_VAL              2
#define MSDP_TABLE_OPEN       3
#define MSDP_TABLE_CLOSE      4

extern void legacy_char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room );
extern void write_to_buffer( DESCRIPTOR_DATA *d, const char *txt, size_t length );

static const char *msdp_direction_name( int direction )
{
   switch( direction )
   {
      case DIR_NORTH:     return "n";
      case DIR_EAST:      return "e";
      case DIR_SOUTH:     return "s";
      case DIR_WEST:      return "w";
      case DIR_UP:        return "u";
      case DIR_DOWN:      return "d";
      case DIR_NORTHEAST: return "ne";
      case DIR_NORTHWEST: return "nw";
      case DIR_SOUTHEAST: return "se";
      case DIR_SOUTHWEST: return "sw";
      default:            return NULL;
   }
}

static const char *msdp_terrain_name( int sector )
{
   switch( sector )
   {
      case SECT_INSIDE:       return "inside";
      case SECT_CITY:         return "city";
      case SECT_FIELD:        return "field";
      case SECT_FOREST:       return "forest";
      case SECT_HILLS:        return "hills";
      case SECT_MOUNTAIN:     return "mountain";
      case SECT_WATER_SWIM:   return "water";
      case SECT_WATER_NOSWIM: return "deep water";
      case SECT_UNDERWATER:   return "underwater";
      case SECT_AIR:          return "air";
      case SECT_DESERT:       return "desert";
      case SECT_OCEANFLOOR:   return "ocean floor";
      case SECT_UNDERGROUND:  return "underground";
      default:                return "unknown";
   }
}

static bool msdp_mapper_exit_visible( const EXIT_DATA *exit )
{
   if( !exit || !exit->to_room )
      return false;

   if( IS_SET( exit->exit_info, EX_HIDDEN ) )
      return false;

   /* Windows are visible room features, not traversable mapper edges. */
   if( IS_SET( exit->exit_info, EX_WINDOW ) )
      return false;

   /* xAUTO exits are keyword/special exits; do not misrepresent them as a
    * normal cardinal edge until they have a dedicated MSDP special-exit
    * contract. */
   if( IS_SET( exit->exit_info, EX_xAUTO ) )
      return false;

   return msdp_direction_name( exit->vdir ) != NULL;
}

static void msdp_append_byte( std::string &packet, unsigned char value )
{
   packet.push_back( ( char )value );
}

static void msdp_append_text( std::string &packet, const char *text )
{
   const unsigned char *p;
   if( !text )
      return;

   for( p = ( const unsigned char * )text; *p; ++p )
   {
      packet.push_back( ( char )*p );
      if( *p == 255 )
         packet.push_back( ( char )255 );
   }
}

static void msdp_pair( std::string &packet, const char *name, const char *value )
{
   msdp_append_byte( packet, MSDP_VAR );
   msdp_append_text( packet, name );
   msdp_append_byte( packet, MSDP_VAL );
   msdp_append_text( packet, value ? value : "" );
}

static void msdp_pair_number( std::string &packet, const char *name, int value )
{
   char number[32];
   snprintf( number, sizeof( number ), "%d", value );
   msdp_pair( packet, name, number );
}

static void msdp_offer( DESCRIPTOR_DATA *d )
{
   const char offer[] = { ( char )255, ( char )251, ( char )ZP_TELOPT_MSDP };
   if( !d || d->msdp_announced )
      return;

   write_to_buffer( d, offer, sizeof( offer ) );
   d->msdp_announced = TRUE;
}

void msdp_send_room( CHAR_DATA *ch )
{
   DESCRIPTOR_DATA *d;
   ROOM_INDEX_DATA *room;
   EXIT_DATA *exit;
   std::string packet;

   if( !ch || IS_NPC( ch ) || !( d = ch->desc ) || !( room = ch->in_room ) )
      return;

   msdp_offer( d );

   msdp_append_byte( packet, 255 );
   msdp_append_byte( packet, 250 );
   msdp_append_byte( packet, ZP_TELOPT_MSDP );

   msdp_append_byte( packet, MSDP_VAR );
   msdp_append_text( packet, "ROOM" );
   msdp_append_byte( packet, MSDP_VAL );
   msdp_append_byte( packet, MSDP_TABLE_OPEN );

   msdp_pair_number( packet, "VNUM", room->vnum );
   msdp_pair( packet, "NAME", room->name ? room->name : "" );
   msdp_pair( packet, "AREA", room->area && room->area->name ? room->area->name : "" );
   msdp_pair( packet, "TERRAIN", msdp_terrain_name( room->sector_type ) );

   msdp_append_byte( packet, MSDP_VAR );
   msdp_append_text( packet, "EXITS" );
   msdp_append_byte( packet, MSDP_VAL );
   msdp_append_byte( packet, MSDP_TABLE_OPEN );

   for( exit = room->first_exit; exit; exit = exit->next )
   {
      const char *direction;

      if( !msdp_mapper_exit_visible( exit ) )
         continue;

      direction = msdp_direction_name( exit->vdir );
      msdp_pair_number( packet, direction, exit->to_room->vnum );
   }

   msdp_append_byte( packet, MSDP_TABLE_CLOSE );
   msdp_append_byte( packet, MSDP_TABLE_CLOSE );
   msdp_append_byte( packet, 255 );
   msdp_append_byte( packet, 240 );

   write_to_buffer( d, packet.data(), packet.size() );
}

void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{
   legacy_char_to_room( ch, room );
   if( ch && ch->in_room == room )
      msdp_send_room( ch );
}
