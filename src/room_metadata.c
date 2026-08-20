/***************************************************************************
 * Zero Point - Authoritative room metadata and builder integration
 *
 * Keeps player-actionable room semantics, map coordinates, decorated room
 * names and builder synchronization in one backend layer. Coordinates are
 * persisted through a reserved room extra-description so both legacy and FUSS
 * area serializers retain them without a second world-file format.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mud.h"

#define ROOM_COORDS_KEY "__zeropoint_map_coordinates__"

extern void msdp_send_room( CHAR_DATA *ch );

namespace
{
enum room_service_bits
{
   ROOM_SERVICE_SHOP = 1 << 0,
   ROOM_SERVICE_WORKSHOP = 1 << 1
};

struct room_tag_definition
{
   const char *label;
   const char *color;
   int room_flag;
   int service_bit;
};

const room_tag_definition room_tag_definitions[] = {
   { "Landing Pad",      "#00AFC8", ROOM_CAN_LAND,  0 },
   { "Bank",             "#D8AD3D", ROOM_BANK,      0 },
   { "Hotel",            "#A65353", ROOM_HOTEL,     0 },
   { "Workshop",         "#D97706", 0,              ROOM_SERVICE_WORKSHOP },
   { "Shop",             "#35A86B", 0,              ROOM_SERVICE_SHOP },
   { "Factory",          "#A95C32", ROOM_FACTORY,   0 },
   { "Refinery",         "#B47B26", ROOM_REFINERY,  0 },
   { "Pet Shop",         "#C06BAE", ROOM_PET_SHOP,  0 },
   { "Auction",          "#9B7CC1", ROOM_AUCTION,   0 },
   { "Donation",         "#7AAE7A", ROOM_DONATION,  0 },
   { "Republic Recruit", "#6F8FC4", ROOM_R_RECRUIT, 0 },
   { "Empire Recruit",   "#B85C5C", ROOM_E_RECRUIT, 0 }
};

void collect_reset_services( const RESET_DATA *reset, bool nested, int *services )
{
   for( ; reset; reset = nested ? reset->next_reset : reset->next )
   {
      if( UPPER( reset->command ) == 'M' )
      {
         MOB_INDEX_DATA *mob = get_mob_index( reset->arg1 );
         if( mob )
         {
            if( mob->pShop )
               *services |= ROOM_SERVICE_SHOP;
            if( mob->rShop )
               *services |= ROOM_SERVICE_WORKSHOP;
         }
      }

      if( reset->first_reset )
         collect_reset_services( reset->first_reset, true, services );
   }
}

int room_service_mask( const ROOM_INDEX_DATA *room )
{
   int services = 0;

   if( room )
   {
      const CHAR_DATA *person;

      collect_reset_services( room->first_reset, false, &services );
      for( person = room->first_person; person; person = person->next_in_room )
      {
         if( !IS_NPC( person ) || !person->pIndexData )
            continue;
         if( person->pIndexData->pShop )
            services |= ROOM_SERVICE_SHOP;
         if( person->pIndexData->rShop )
            services |= ROOM_SERVICE_WORKSHOP;
      }
   }

   return services;
}

EXTRA_DESCR_DATA *room_coordinate_extra( ROOM_INDEX_DATA *room )
{
   EXTRA_DESCR_DATA *ed;

   if( !room )
      return NULL;

   for( ed = room->first_extradesc; ed; ed = ed->next )
      if( ed->keyword && !str_cmp( ed->keyword, ROOM_COORDS_KEY ) )
         return ed;

   return NULL;
}

void room_restore_coordinates( ROOM_INDEX_DATA *room )
{
   EXTRA_DESCR_DATA *ed;
   int x, y, z;

   if( !room )
      return;

   room->map_x = 0;
   room->map_y = 0;
   room->map_z = 0;
   room->map_coords_set = false;

   ed = room_coordinate_extra( room );
   if( !ed || !ed->description )
      return;

   x = y = z = 0;
   if( sscanf( ed->description, "%d %d %d", &x, &y, &z ) != 3 )
      return;

   room->map_x = x;
   room->map_y = y;
   room->map_z = z;
   room->map_coords_set = true;
}

void room_store_coordinates( ROOM_INDEX_DATA *room, int x, int y, int z )
{
   EXTRA_DESCR_DATA *ed;
   char value[96];

   if( !room )
      return;

   ed = SetRExtra( room, ROOM_COORDS_KEY );
   if( !ed )
      return;

   snprintf( value, sizeof( value ), "%d %d %d\r\n", x, y, z );
   STRFREE( ed->description );
   ed->description = STRALLOC( value );

   room->map_x = x;
   room->map_y = y;
   room->map_z = z;
   room->map_coords_set = true;
}

void room_clear_coordinates( ROOM_INDEX_DATA *room )
{
   if( !room )
      return;

   DelRExtra( room, ROOM_COORDS_KEY );
   room->map_x = 0;
   room->map_y = 0;
   room->map_z = 0;
   room->map_coords_set = false;
}

void room_refresh_msdp( ROOM_INDEX_DATA *room )
{
   CHAR_DATA *person;

   if( !room )
      return;

   for( person = room->first_person; person; person = person->next_in_room )
      if( !IS_NPC( person ) && person->desc )
         msdp_send_room( person );
}

bool room_look_shows_room( const char *argument )
{
   char arg[MAX_INPUT_LENGTH];

   if( !argument || argument[0] == '\0' )
      return true;

   one_argument( argument, arg );
   return arg[0] == '\0' || !str_cmp( arg, "auto" );
}

void room_decorated_name( const ROOM_INDEX_DATA *room, char *out, size_t out_size )
{
   ROOM_ACTION_TAG tags[ROOM_ACTION_TAG_MAX];
   char tag[MAX_INPUT_LENGTH];
   int count;

   if( !out || out_size == 0 )
      return;

   out[0] = '\0';
   if( !room )
      return;

   strlcpy( out, room->name ? room->name : "", out_size );
   count = room_collect_action_tags( room, tags, ROOM_ACTION_TAG_MAX );

   for( int i = 0; i < count; ++i )
   {
      snprintf( tag, sizeof( tag ), " &%s[%s]&w", tags[i].color, tags[i].label );
      strlcat( out, tag, out_size );
   }
}
}

int room_collect_action_tags( const ROOM_INDEX_DATA *room, ROOM_ACTION_TAG *tags, int max_tags )
{
   int count = 0;
   int services;

   if( !room || !tags || max_tags <= 0 )
      return 0;

   services = room_service_mask( room );

   for( const room_tag_definition &definition : room_tag_definitions )
   {
      const bool present =
         ( definition.room_flag && IS_SET( room->room_flags, definition.room_flag ) ) ||
         ( definition.service_bit && ( services & definition.service_bit ) );

      if( !present )
         continue;

      tags[count].label = definition.label;
      tags[count].color = definition.color;
      if( ++count >= max_tags )
         break;
   }

   return count;
}

const char *room_primary_action_color( const ROOM_INDEX_DATA *room )
{
   ROOM_ACTION_TAG tag;

   /* MSDP asks for the primary colour before serializing coordinates, making
    * this the common publication boundary for lazy coordinate restoration. */
   room_restore_coordinates( const_cast<ROOM_INDEX_DATA *>( room ) );
   return room_collect_action_tags( room, &tag, 1 ) == 1 ? tag.color : "";
}

/*
 * The build and elevator translation units are compiled with their exported
 * command symbols renamed. These wrappers add metadata behavior without
 * duplicating either subsystem.
 */
extern "C"
{
   void elevator_do_look( CHAR_DATA *ch, const char *argument );
   void legacy_do_redit( CHAR_DATA *ch, const char *argument );
   void legacy_do_rstat( CHAR_DATA *ch, const char *argument );
}

extern "C" void do_look( CHAR_DATA *ch, const char *argument )
{
   ROOM_INDEX_DATA *room = ch ? ch->in_room : NULL;
   const char *original_name = NULL;
   char decorated[MAX_STRING_LENGTH];
   const bool decorate = room && room_look_shows_room( argument );

   if( room )
      room_restore_coordinates( room );

   if( decorate )
   {
      original_name = room->name;
      room_decorated_name( room, decorated, sizeof( decorated ) );
      room->name = decorated;
   }

   elevator_do_look( ch, argument );

   if( decorate )
      room->name = original_name;
}

extern "C" void do_rstat( CHAR_DATA *ch, const char *argument )
{
   ROOM_INDEX_DATA *room = NULL;

   if( ch )
   {
      if( argument && argument[0] != '\0' )
         room = find_location( ch, argument );
      if( !room )
         room = ch->in_room;
      if( room )
         room_restore_coordinates( room );
   }

   legacy_do_rstat( ch, argument );

   if( !ch || !room )
      return;

   if( room->map_coords_set )
      ch_printf( ch, "Map coordinates: %d %d %d\r\n", room->map_x, room->map_y, room->map_z );
   else
      send_to_char( "Map coordinates: unset\r\n", ch );
}

extern "C" void do_redit( CHAR_DATA *ch, const char *argument )
{
   ROOM_INDEX_DATA *room;
   char command[MAX_INPUT_LENGTH];
   const char *rest;

   if( !ch )
      return;

   if( !( room = ch->in_room ) )
   {
      legacy_do_redit( ch, argument );
      return;
   }

   room_restore_coordinates( room );
   rest = one_argument( argument ? argument : "", command );

   if( !str_cmp( command, "coords" ) || !str_cmp( command, "coordinates" ) )
   {
      char sx[MAX_INPUT_LENGTH], sy[MAX_INPUT_LENGTH], sz[MAX_INPUT_LENGTH];
      int x, y, z;

      if( !can_rmodify( ch, room ) )
         return;

      if( !rest || rest[0] == '\0' )
      {
         if( room->map_coords_set )
            ch_printf( ch, "Map coordinates: %d %d %d\r\n", room->map_x, room->map_y, room->map_z );
         else
            send_to_char( "Map coordinates are unset.\r\n", ch );
         send_to_char( "Usage: redit coords <x> <y> <z> | redit coords clear\r\n", ch );
         return;
      }

      rest = one_argument( rest, sx );
      rest = one_argument( rest, sy );
      rest = one_argument( rest, sz );

      if( !str_cmp( sx, "clear" ) )
      {
         room_clear_coordinates( room );
         room_refresh_msdp( room );
         send_to_char( "Map coordinates cleared.\r\n", ch );
         return;
      }

      if( sx[0] == '\0' || sy[0] == '\0' || sz[0] == '\0'
          || !is_number( sx ) || !is_number( sy ) || !is_number( sz ) )
      {
         send_to_char( "Usage: redit coords <x> <y> <z> | redit coords clear\r\n", ch );
         return;
      }

      x = atoi( sx );
      y = atoi( sy );
      z = atoi( sz );
      room_store_coordinates( room, x, y, z );
      room_refresh_msdp( room );
      ch_printf( ch, "Map coordinates set to %d %d %d.\r\n", x, y, z );
      return;
   }

   /*
    * descset is the plugin-friendly single-command description path. The
    * inherited `redit desc` multiline editor remains available unchanged.
    */
   if( !str_cmp( command, "descset" ) )
   {
      char value[MAX_STRING_LENGTH];

      if( !can_rmodify( ch, room ) )
         return;

      if( !rest || rest[0] == '\0' )
      {
         send_to_char( "Usage: redit descset <room description>\r\n", ch );
         return;
      }

      snprintf( value, sizeof( value ), "%s\r\n", rest );
      STRFREE( room->description );
      room->description = STRALLOC( value );
      room_refresh_msdp( room );
      send_to_char( "Room description updated.\r\n", ch );
      return;
   }

   legacy_do_redit( ch, argument );

   /*
    * Name, flags, sector, exits and multiline description edits all affect
    * client presentation. Publishing after every redit return is cheap and
    * guarantees the plugin never waits for movement to see an admin edit.
    */
   room_restore_coordinates( room );
   room_refresh_msdp( room );
}
