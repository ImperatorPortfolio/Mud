/***************************************************************************
 * Zero Point - MSDP authoritative room publication
 *
 * Publishes the standard MSDP ROOM table whenever a character enters a
 * room. handler.c remains the owner of room membership; this translation
 * unit wraps char_to_room() so every movement path shares one publication
 * boundary.
 ***************************************************************************/

#include <ctype.h>
#include <cstring>
#include <stdio.h>
#include <string>
#include "mud.h"

#define ZP_TELOPT_MSDP       69
#define MSDP_VAR              1
#define MSDP_VAL              2
#define MSDP_TABLE_OPEN       3
#define MSDP_TABLE_CLOSE      4

extern void legacy_char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room );

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

   if( IS_SET( exit->exit_info, EX_WINDOW ) )
      return false;

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
   char escaped[8];

   if( !text )
      return;

   for( p = ( const unsigned char * )text; *p; ++p )
   {
      switch( *p )
      {
         case '\\':
            packet.append( "\\\\" );
            break;
         case '\b':
            packet.append( "\\b" );
            break;
         case '\f':
            packet.append( "\\f" );
            break;
         case '\n':
            packet.append( "\\n" );
            break;
         case '\r':
            packet.append( "\\r" );
            break;
         case '\t':
            packet.append( "\\t" );
            break;
         default:
            if( *p < 32 )
            {
               snprintf( escaped, sizeof( escaped ), "\\u%04x", *p );
               packet.append( escaped );
            }
            else
            {
               packet.push_back( ( char )*p );
               if( *p == 255 )
                  packet.push_back( ( char )255 );
            }
            break;
      }
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

static void msdp_table_begin( std::string &packet, const char *name )
{
   msdp_append_byte( packet, MSDP_VAR );
   msdp_append_text( packet, name );
   msdp_append_byte( packet, MSDP_VAL );
   msdp_append_byte( packet, MSDP_TABLE_OPEN );
}

static void msdp_table_end( std::string &packet )
{
   msdp_append_byte( packet, MSDP_TABLE_CLOSE );
}

static void msdp_table_item( std::string &packet, int index, const char *value )
{
   char key[16];
   snprintf( key, sizeof( key ), "%d", index );
   msdp_pair( packet, key, value ? value : "" );
}

static void msdp_offer( DESCRIPTOR_DATA *d )
{
   const char offer[] = { ( char )255, ( char )251, ( char )ZP_TELOPT_MSDP };
   if( !d || d->msdp_announced )
      return;

   write_to_buffer( d, offer, sizeof( offer ) );
   d->msdp_announced = true;
}

static bool msdp_token_equal( const unsigned char *data, size_t length, const char *text )
{
   size_t text_length = strlen( text );
   if( length != text_length )
      return false;

   for( size_t i = 0; i < length; ++i )
      if( toupper( data[i] ) != toupper( ( unsigned char )text[i] ) )
         return false;

   return true;
}

void msdp_send_room( CHAR_DATA *ch );

void msdp_set_enabled( DESCRIPTOR_DATA *d, bool enabled )
{
   if( !d )
      return;

   d->msdp_enabled = enabled;
   if( !enabled )
   {
      d->msdp_report_room = false;
      return;
   }

   if( d->msdp_report_room && d->character )
      msdp_send_room( d->character );
}

void msdp_handle_subnegotiation( DESCRIPTOR_DATA *d, const unsigned char *data, size_t length )
{
   size_t i = 0;

   if( !d || !data || length == 0 )
      return;

   while( i < length )
   {
      const unsigned char *name;
      const unsigned char *value;
      size_t name_length = 0;
      size_t value_length = 0;

      if( data[i] != MSDP_VAR )
      {
         ++i;
         continue;
      }

      ++i;
      name = data + i;
      while( i < length && data[i] != MSDP_VAL && data[i] != MSDP_VAR )
      {
         ++name_length;
         ++i;
      }

      if( i >= length || data[i] != MSDP_VAL )
         continue;

      ++i;
      value = data + i;
      while( i < length && data[i] != MSDP_VAR )
      {
         ++value_length;
         ++i;
      }

      if( !msdp_token_equal( value, value_length, "ROOM" ) )
         continue;

      if( msdp_token_equal( name, name_length, "REPORT" ) )
      {
         d->msdp_report_room = true;
         if( d->msdp_enabled && d->character )
            msdp_send_room( d->character );
      }
      else if( msdp_token_equal( name, name_length, "UNREPORT" ) )
         d->msdp_report_room = false;
      else if( msdp_token_equal( name, name_length, "SEND" ) )
      {
         if( d->msdp_enabled && d->character )
            msdp_send_room( d->character );
      }
   }
}

static void msdp_room_tags( std::string &packet, ROOM_INDEX_DATA *room )
{
   ROOM_ACTION_TAG tags[ROOM_ACTION_TAG_MAX];
   const int count = room_collect_action_tags( room, tags, ROOM_ACTION_TAG_MAX );

   msdp_table_begin( packet, "TAGS" );
   for( int i = 0; i < count; ++i )
      msdp_pair( packet, tags[i].label, tags[i].color );
   msdp_table_end( packet );
}

static void msdp_room_windows( std::string &packet, ROOM_INDEX_DATA *room )
{
   EXIT_DATA *exit;
   int index = 1;

   msdp_table_begin( packet, "WINDOWS" );
   for( exit = room->first_exit; exit; exit = exit->next )
   {
      char line[MAX_STRING_LENGTH];
      const char *direction;
      const char *destination;

      if( !IS_SET( exit->exit_info, EX_WINDOW ) || IS_SET( exit->exit_info, EX_HIDDEN ) )
         continue;

      direction = msdp_direction_name( exit->vdir );
      destination = exit->to_room && exit->to_room->name ? exit->to_room->name : "beyond";
      if( direction )
         snprintf( line, sizeof( line ), "%s - %s", direction, destination );
      else
         snprintf( line, sizeof( line ), "%s", destination );

      msdp_table_item( packet, index++, line );
   }
   msdp_table_end( packet );
}

static void msdp_room_people( std::string &packet, ROOM_INDEX_DATA *room )
{
   CHAR_DATA *person;
   int index = 1;

   msdp_table_begin( packet, "PEOPLE" );
   for( person = room->first_person; person; person = person->next_in_room )
   {
      const char *name = IS_NPC( person ) ? person->short_descr : person->name;
      if( name && name[0] != '\0' )
         msdp_table_item( packet, index++, name );
   }
   msdp_table_end( packet );
}

static void msdp_room_objects( std::string &packet, ROOM_INDEX_DATA *room, bool furniture )
{
   OBJ_DATA *obj;
   int index = 1;

   msdp_table_begin( packet, furniture ? "FURNITURE" : "OBJECTS" );
   for( obj = room->first_content; obj; obj = obj->next_content )
   {
      bool is_furniture = obj->item_type == ITEM_FURNITURE;
      if( is_furniture != furniture )
         continue;

      if( obj->short_descr && obj->short_descr[0] != '\0' )
         msdp_table_item( packet, index++, obj->short_descr );
   }
   msdp_table_end( packet );
}

static void msdp_room_ships( std::string &packet, ROOM_INDEX_DATA *room )
{
   SHIP_DATA *ship;
   int index = 1;

   msdp_table_begin( packet, "SHIPS" );
   for( ship = room->first_ship; ship; ship = ship->next_in_room )
   {
      if( ship->name && ship->name[0] != '\0' )
         msdp_table_item( packet, index++, ship->name );
   }
   msdp_table_end( packet );
}

void msdp_send_room( CHAR_DATA *ch )
{
   DESCRIPTOR_DATA *d;
   ROOM_INDEX_DATA *room;
   EXIT_DATA *exit;
   std::string packet;
   const char *room_color;

   if( !ch || IS_NPC( ch ) || !( d = ch->desc ) || !( room = ch->in_room ) )
      return;

   msdp_offer( d );

   if( !d->msdp_enabled || !d->msdp_report_room )
      return;

   room_color = room_primary_action_color( room );

   msdp_append_byte( packet, 255 );
   msdp_append_byte( packet, 250 );
   msdp_append_byte( packet, ZP_TELOPT_MSDP );

   msdp_table_begin( packet, "ROOM" );

   msdp_pair_number( packet, "VNUM", room->vnum );
   msdp_pair( packet, "NAME", room->name ? room->name : "" );
   msdp_pair( packet, "AREA", room->area && room->area->name ? room->area->name : "" );
   msdp_pair( packet, "TERRAIN", msdp_terrain_name( room->sector_type ) );
   msdp_pair( packet, "DESCRIPTION", room->description ? room->description : "" );
   msdp_pair_number( packet, "X", room->map_x );
   msdp_pair_number( packet, "Y", room->map_y );
   msdp_pair_number( packet, "Z", room->map_z );
   msdp_pair_number( packet, "COORDS_SET", room->map_coords_set ? 1 : 0 );
   msdp_pair( packet, "COLOR", room_color );
   msdp_room_tags( packet, room );

   msdp_table_begin( packet, "EXITS" );
   for( exit = room->first_exit; exit; exit = exit->next )
   {
      const char *direction;

      if( !msdp_mapper_exit_visible( exit ) )
         continue;

      direction = msdp_direction_name( exit->vdir );
      msdp_pair_number( packet, direction, exit->to_room->vnum );
   }
   msdp_table_end( packet );

   msdp_room_windows( packet, room );
   msdp_room_people( packet, room );
   msdp_room_objects( packet, room, false );
   msdp_room_objects( packet, room, true );
   msdp_room_ships( packet, room );

   msdp_table_end( packet );
   msdp_append_byte( packet, 255 );
   msdp_append_byte( packet, 240 );
   write_to_buffer( d, packet.data(), packet.size() );

   /* Scalar events keep Mudlet integrations simple while ROOM remains the
    * authoritative structured document. */
   packet.clear();
   msdp_append_byte( packet, 255 );
   msdp_append_byte( packet, 250 );
   msdp_append_byte( packet, ZP_TELOPT_MSDP );
   msdp_pair_number( packet, "ROOM_VNUM", room->vnum );
   msdp_pair_number( packet, "ROOM_X", room->map_x );
   msdp_pair_number( packet, "ROOM_Y", room->map_y );
   msdp_pair_number( packet, "ROOM_Z", room->map_z );
   msdp_pair_number( packet, "ROOM_COORDS_SET", room->map_coords_set ? 1 : 0 );
   msdp_pair( packet, "ROOM_COLOR", room_color );
   msdp_pair_number( packet, "IS_IMMORTAL", IS_IMMORTAL( ch ) ? 1 : 0 );
   msdp_pair_number( packet, "BUILD_LEVEL", get_trust( ch ) );
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
