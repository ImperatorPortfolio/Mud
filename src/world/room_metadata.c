/***************************************************************************
 * Zero Point - Authoritative room action metadata
 *
 * Keeps player-actionable room semantics in one backend layer so room text,
 * MSDP and client mapping consume the same labels and colours.
 ***************************************************************************/

#include "mud.h"

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
   { "Landing Pad",       "#00AFC8", ROOM_CAN_LAND,   0 },
   { "Bank",              "#D8AD3D", ROOM_BANK,       0 },
   { "Hotel",             "#A65353", ROOM_HOTEL,      0 },
   { "Workshop",          "#D97706", 0,               ROOM_SERVICE_WORKSHOP },
   { "Shop",              "#35A86B", 0,               ROOM_SERVICE_SHOP },
   { "Factory",           "#A95C32", ROOM_FACTORY,    0 },
   { "Refinery",          "#B47B26", ROOM_REFINERY,   0 },
   { "Pet Shop",          "#C06BAE", ROOM_PET_SHOP,   0 },
   { "Auction",           "#9B7CC1", ROOM_AUCTION,    0 },
   { "Donation",          "#7AAE7A", ROOM_DONATION,   0 },
   { "Republic Recruit",  "#6F8FC4", ROOM_R_RECRUIT,  0 },
   { "Empire Recruit",    "#B85C5C", ROOM_E_RECRUIT,  0 }
};

static void collect_reset_services( const RESET_DATA *reset, bool nested, int *services )
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

static int room_service_mask( const ROOM_INDEX_DATA *room )
{
   int services = 0;

   if( room )
      collect_reset_services( room->first_reset, false, &services );

   return services;
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
   return room_collect_action_tags( room, &tag, 1 ) == 1 ? tag.color : "";
}
