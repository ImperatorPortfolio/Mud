/***************************************************************************
 * Zero Point - Collective-control elevators
 *
 * Elevators are persistent car rooms with dynamic door exits. Calls and car
 * selections are queued; an elevator never reverses while an outstanding
 * request remains ahead in its current direction. Floor rooms and exit
 * directions are explicit configuration; no topology is inferred.
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mud.h"

enum elevator_state
{
   ELEVATOR_IDLE,
   ELEVATOR_MOVING_UP,
   ELEVATOR_MOVING_DOWN,
   ELEVATOR_DOORS_OPEN
};

struct elevator_stop
{
   int floor;
   int room_vnum;
   int legacy_car_vnum;
   short floor_door_dir;
   const char *label;
};

struct elevator_data
{
   const char *name;
   int car_room_vnum;
   short car_door_dir;
   elevator_stop *stops;
   int stop_count;
   int current_index;
   elevator_state state;
   bool isRising;
   bool initialized;
   bool requests[16];
   time_t next_action;
};

/* do_* declarations in mud.h have C linkage when built as C++. */
extern "C"
{
   void legacy_do_push( CHAR_DATA *ch, const char *argument );
   void legacy_do_look( CHAR_DATA *ch, const char *argument );
}

extern void natural_update_handler( void );
extern void msdp_send_room( CHAR_DATA *ch );

#define ELEVATOR_TRAVEL_SECONDS 2
#define ELEVATOR_DOOR_SECONDS   4

/*
 * First installation: one of Directorate City's existing three-level street
 * turbolifts. Static shaft rooms 409/410/411 are replaced by one persistent
 * car (427) serving the three street rooms below.
 */
static elevator_stop hague_stops[] =
{
   { 1, 398, 411, DIR_NORTHWEST, "Lower Directorate Street" },
   { 2, 322, 410, DIR_NORTHWEST, "Central Directorate Street" },
   { 3, 358, 409, DIR_NORTHWEST, "Upper Directorate Street" }
};

static elevator_data hague_elevator =
{
   "Directorate City Turbolift",
   427,
   DIR_SOUTHEAST,
   hague_stops,
   3,
   0,
   ELEVATOR_IDLE,
   true,
   false,
   { false },
   0
};

static elevator_data *all_elevators[] =
{
   &hague_elevator
};

static const int elevator_count = sizeof( all_elevators ) / sizeof( all_elevators[0] );

static void elevator_room_echo( ROOM_INDEX_DATA *room, const char *text )
{
   CHAR_DATA *ch;

   if( !room || !text )
      return;

   for( ch = room->first_person; ch; ch = ch->next_in_room )
      send_to_char( text, ch );
}

static void elevator_refresh_msdp( ROOM_INDEX_DATA *room )
{
   CHAR_DATA *ch;

   if( !room )
      return;

   for( ch = room->first_person; ch; ch = ch->next_in_room )
      if( !IS_NPC( ch ) && ch->desc )
         msdp_send_room( ch );
}

static int elevator_stop_index_for_room( elevator_data *lift, int room_vnum )
{
   int i;

   if( !lift )
      return -1;

   for( i = 0; i < lift->stop_count; ++i )
      if( lift->stops[i].room_vnum == room_vnum )
         return i;

   return -1;
}

static elevator_data *elevator_for_room( ROOM_INDEX_DATA *room, int *stop_index, bool *inside_car )
{
   int e;

   if( stop_index )
      *stop_index = -1;
   if( inside_car )
      *inside_car = false;
   if( !room )
      return NULL;

   for( e = 0; e < elevator_count; ++e )
   {
      elevator_data *lift = all_elevators[e];
      int stop;

      if( room->vnum == lift->car_room_vnum )
      {
         if( inside_car )
            *inside_car = true;
         if( stop_index )
            *stop_index = lift->current_index;
         return lift;
      }

      stop = elevator_stop_index_for_room( lift, room->vnum );
      if( stop >= 0 )
      {
         if( stop_index )
            *stop_index = stop;
         return lift;
      }
   }

   return NULL;
}

static bool elevator_remove_exit_if_to( ROOM_INDEX_DATA *room, short dir, int destination_vnum )
{
   EXIT_DATA *exit;

   if( !room )
      return false;

   exit = get_exit( room, dir );
   if( !exit || !exit->to_room || exit->to_room->vnum != destination_vnum )
      return false;

   extract_exit( room, exit );
   return true;
}

static void elevator_disconnect_current_floor( elevator_data *lift, bool announce )
{
   ROOM_INDEX_DATA *car;
   ROOM_INDEX_DATA *floor;
   bool removed = false;

   if( !lift || lift->current_index < 0 || lift->current_index >= lift->stop_count )
      return;

   car = get_room_index( lift->car_room_vnum );
   floor = get_room_index( lift->stops[lift->current_index].room_vnum );
   if( !car || !floor )
      return;

   if( elevator_remove_exit_if_to( car, lift->car_door_dir, floor->vnum ) )
      removed = true;
   if( elevator_remove_exit_if_to( floor, lift->stops[lift->current_index].floor_door_dir, car->vnum ) )
      removed = true;

   if( removed && announce )
   {
      elevator_room_echo( car, "&wThe turbolift doors slide shut.\r\n" );
      elevator_room_echo( floor, "&wThe turbolift doors slide shut.\r\n" );
   }

   if( removed )
   {
      elevator_refresh_msdp( car );
      elevator_refresh_msdp( floor );
   }
}

static void elevator_open_doors( elevator_data *lift )
{
   ROOM_INDEX_DATA *car;
   ROOM_INDEX_DATA *floor;
   EXIT_DATA *car_exit;
   EXIT_DATA *floor_exit;
   char buf[MAX_STRING_LENGTH];

   if( !lift || lift->current_index < 0 || lift->current_index >= lift->stop_count )
      return;

   car = get_room_index( lift->car_room_vnum );
   floor = get_room_index( lift->stops[lift->current_index].room_vnum );
   if( !car || !floor )
      return;

   /* Reconcile silently so a stale edge can never survive at this stop. */
   elevator_disconnect_current_floor( lift, false );

   car_exit = make_exit( car, floor, lift->car_door_dir );
   floor_exit = make_exit( floor, car, lift->stops[lift->current_index].floor_door_dir );

   if( !car_exit || !floor_exit )
   {
      if( car_exit )
         extract_exit( car, car_exit );
      if( floor_exit )
         extract_exit( floor, floor_exit );
      bug( "%s: unable to create both door exits for %s floor %d", __func__, lift->name,
           lift->stops[lift->current_index].floor );
      lift->state = ELEVATOR_IDLE;
      return;
   }

   car_exit->rexit = floor_exit;
   floor_exit->rexit = car_exit;

   snprintf( buf, sizeof( buf ),
             "&WA soft chime sounds. The turbolift doors open at floor %d, %s.\r\n",
             lift->stops[lift->current_index].floor,
             lift->stops[lift->current_index].label );
   elevator_room_echo( car, buf );
   elevator_room_echo( floor, "&WA soft chime sounds as the turbolift doors slide open.\r\n" );

   lift->state = ELEVATOR_DOORS_OPEN;
   lift->requests[lift->current_index] = false;
   lift->next_action = current_time + ELEVATOR_DOOR_SECONDS;

   elevator_refresh_msdp( car );
   elevator_refresh_msdp( floor );
}

static bool elevator_has_requests( elevator_data *lift )
{
   int i;

   for( i = 0; lift && i < lift->stop_count; ++i )
      if( lift->requests[i] )
         return true;

   return false;
}

static bool elevator_has_ahead( elevator_data *lift, bool rising )
{
   int i;

   if( !lift )
      return false;

   if( rising )
   {
      for( i = lift->current_index + 1; i < lift->stop_count; ++i )
         if( lift->requests[i] )
            return true;
   }
   else
   {
      for( i = lift->current_index - 1; i >= 0; --i )
         if( lift->requests[i] )
            return true;
   }

   return false;
}

static void elevator_choose_direction( elevator_data *lift )
{
   int best = -1;
   int best_distance = 9999;
   int i;

   if( !lift || !elevator_has_requests( lift ) )
   {
      if( lift )
         lift->state = ELEVATOR_IDLE;
      return;
   }

   if( lift->requests[lift->current_index] )
   {
      elevator_open_doors( lift );
      return;
   }

   /* Preserve directional inertia while any requested stop remains ahead. */
   if( elevator_has_ahead( lift, lift->isRising ) )
   {
      lift->state = lift->isRising ? ELEVATOR_MOVING_UP : ELEVATOR_MOVING_DOWN;
      lift->next_action = current_time + ELEVATOR_TRAVEL_SECONDS;
      return;
   }

   for( i = 0; i < lift->stop_count; ++i )
   {
      int distance;

      if( !lift->requests[i] )
         continue;

      distance = abs( i - lift->current_index );
      if( distance < best_distance )
      {
         best = i;
         best_distance = distance;
      }
   }

   if( best < 0 )
   {
      lift->state = ELEVATOR_IDLE;
      return;
   }

   lift->isRising = best > lift->current_index;
   lift->state = lift->isRising ? ELEVATOR_MOVING_UP : ELEVATOR_MOVING_DOWN;
   lift->next_action = current_time + ELEVATOR_TRAVEL_SECONDS;
}

static void elevator_begin_after_doors( elevator_data *lift )
{
   if( !lift )
      return;

   elevator_disconnect_current_floor( lift, true );

   if( !elevator_has_requests( lift ) )
   {
      lift->state = ELEVATOR_IDLE;
      lift->next_action = 0;
      return;
   }

   if( !elevator_has_ahead( lift, lift->isRising ) )
      lift->isRising = !lift->isRising;

   if( !elevator_has_ahead( lift, lift->isRising ) )
   {
      elevator_choose_direction( lift );
      return;
   }

   lift->state = lift->isRising ? ELEVATOR_MOVING_UP : ELEVATOR_MOVING_DOWN;
   lift->next_action = current_time + ELEVATOR_TRAVEL_SECONDS;
}

static void elevator_move_one_floor( elevator_data *lift )
{
   ROOM_INDEX_DATA *car;
   char buf[MAX_STRING_LENGTH];
   int next_index;

   if( !lift )
      return;

   if( !elevator_has_ahead( lift, lift->isRising ) )
   {
      lift->isRising = !lift->isRising;
      if( !elevator_has_ahead( lift, lift->isRising ) )
      {
         elevator_choose_direction( lift );
         return;
      }
   }

   next_index = lift->current_index + ( lift->isRising ? 1 : -1 );
   if( next_index < 0 || next_index >= lift->stop_count )
   {
      lift->isRising = !lift->isRising;
      elevator_choose_direction( lift );
      return;
   }

   lift->current_index = next_index;
   car = get_room_index( lift->car_room_vnum );

   snprintf( buf, sizeof( buf ),
             "&wThe floor indicator changes to &W%d&w as the turbolift continues %s.\r\n",
             lift->stops[lift->current_index].floor,
             lift->isRising ? "upward" : "downward" );
   elevator_room_echo( car, buf );

   /* Requests encountered in the current sweep are served immediately. */
   if( lift->requests[lift->current_index] )
   {
      elevator_open_doors( lift );
      return;
   }

   if( !elevator_has_ahead( lift, lift->isRising ) )
   {
      if( elevator_has_ahead( lift, !lift->isRising ) )
         lift->isRising = !lift->isRising;
      else if( !elevator_has_requests( lift ) )
      {
         lift->state = ELEVATOR_IDLE;
         lift->next_action = 0;
         return;
      }
   }

   lift->state = lift->isRising ? ELEVATOR_MOVING_UP : ELEVATOR_MOVING_DOWN;
   lift->next_action = current_time + ELEVATOR_TRAVEL_SECONDS;
}

static bool elevator_initialize( elevator_data *lift )
{
   ROOM_INDEX_DATA *car;
   int i;

   if( !lift )
      return false;
   if( lift->initialized )
      return true;

   car = get_room_index( lift->car_room_vnum );
   if( !car )
      return false;

   for( i = 0; i < lift->stop_count; ++i )
      if( !get_room_index( lift->stops[i].room_vnum ) )
         return false;

   /* Retire only the legacy floor->shaft edges this installation replaces. */
   for( i = 0; i < lift->stop_count; ++i )
   {
      ROOM_INDEX_DATA *floor = get_room_index( lift->stops[i].room_vnum );
      if( elevator_remove_exit_if_to( floor,
                                      lift->stops[i].floor_door_dir,
                                      lift->stops[i].legacy_car_vnum ) )
         elevator_refresh_msdp( floor );
   }

   /* Isolate room 427 from the inherited shaft before making it the car. */
   elevator_remove_exit_if_to( get_room_index( 411 ), DIR_DOWN, lift->car_room_vnum );
   elevator_remove_exit_if_to( get_room_index( 428 ), DIR_UP, lift->car_room_vnum );
   while( car->first_exit )
      extract_exit( car, car->first_exit );

   if( car->name )
      STRFREE( car->name );
   car->name = STRALLOC( "Inside a Directorate City Turbolift" );

   if( car->description )
      STRFREE( car->description );
   car->description = STRALLOC(
      "A clean composite-metal elevator car hums softly around you. A luminous control panel is set beside the doors, while a narrow indicator above it shows the current street level. The car is designed to move vertically through Directorate City's stacked transport levels.\r\n" );

   lift->initialized = true;
   lift->state = ELEVATOR_IDLE;
   lift->current_index = 0;
   lift->isRising = true;
   lift->next_action = 0;
   elevator_refresh_msdp( car );
   return true;
}

static void elevator_request( elevator_data *lift, int stop_index )
{
   if( !lift || stop_index < 0 || stop_index >= lift->stop_count )
      return;

   if( lift->state == ELEVATOR_DOORS_OPEN && stop_index == lift->current_index )
   {
      lift->next_action = current_time + ELEVATOR_DOOR_SECONDS;
      return;
   }

   lift->requests[stop_index] = true;

   if( lift->state == ELEVATOR_IDLE )
      elevator_choose_direction( lift );
}

static void elevator_show_panel( CHAR_DATA *ch, elevator_data *lift, bool inside_car )
{
   int i;

   if( !ch || !lift )
      return;

   if( inside_car )
   {
      ch_printf( ch, "&W%s control panel&D\r\n", lift->name );
      ch_printf( ch, "&wCurrent floor: &W%d&w   Direction: &W%s&w   State: &W%s\r\n\r\n",
                 lift->stops[lift->current_index].floor,
                 lift->isRising ? "UP" : "DOWN",
                 lift->state == ELEVATOR_DOORS_OPEN ? "DOORS OPEN" :
                 lift->state == ELEVATOR_IDLE ? "IDLE" : "MOVING" );

      for( i = lift->stop_count - 1; i >= 0; --i )
      {
         ch_printf( ch, "  [%d] %-28s %s\r\n",
                    lift->stops[i].floor,
                    lift->stops[i].label,
                    lift->requests[i] ? "&YREQUESTED&w" :
                    i == lift->current_index ? "&GCURRENT&w" : "" );
      }

      send_to_char( "\r\n&wUse &Wpush <floor>&w, &Wpush open&w or &Wpush close&w.\r\n", ch );
      return;
   }

   i = elevator_stop_index_for_room( lift, ch->in_room->vnum );
   if( i < 0 )
      return;

   ch_printf( ch,
              "&wA recessed turbolift call panel marks this as floor &W%d&w. The indicator currently reads &W%d&w%s.\r\nUse &Wpush call&w to summon the car.\r\n",
              lift->stops[i].floor,
              lift->stops[lift->current_index].floor,
              lift->isRising ? " with an upward arrow" : " with a downward arrow" );
}

static bool elevator_handle_push( CHAR_DATA *ch, const char *argument )
{
   elevator_data *lift;
   int stop_index;
   bool inside_car;
   char arg[MAX_INPUT_LENGTH];
   const char *rest;
   int floor;
   int i;

   if( !ch || !ch->in_room )
      return false;

   lift = elevator_for_room( ch->in_room, &stop_index, &inside_car );
   if( !lift || !elevator_initialize( lift ) )
      return false;

   rest = one_argument( argument, arg );
   if( arg[0] == '\0' )
      return false;

   if( !inside_car )
   {
      if( str_cmp( arg, "call" ) && str_cmp( arg, "button" )
          && str_cmp( arg, "elevator" ) && str_cmp( arg, "lift" )
          && str_cmp( arg, "turbolift" ) )
         return false;

      if( lift->state == ELEVATOR_DOORS_OPEN && lift->current_index == stop_index )
      {
         send_to_char( "The turbolift is already here with its doors open.\r\n", ch );
         lift->next_action = current_time + ELEVATOR_DOOR_SECONDS;
         return true;
      }

      elevator_request( lift, stop_index );
      send_to_char( "You press the turbolift call button. It illuminates softly.\r\n", ch );
      return true;
   }

   if( !str_cmp( arg, "floor" ) )
      rest = one_argument( rest, arg );

   if( !str_cmp( arg, "open" ) )
   {
      if( lift->state == ELEVATOR_MOVING_UP || lift->state == ELEVATOR_MOVING_DOWN )
      {
         send_to_char( "The door control refuses to respond while the turbolift is moving.\r\n", ch );
         return true;
      }

      elevator_open_doors( lift );
      return true;
   }

   if( !str_cmp( arg, "close" ) )
   {
      if( lift->state != ELEVATOR_DOORS_OPEN )
      {
         send_to_char( "The doors are already closed.\r\n", ch );
         return true;
      }

      elevator_begin_after_doors( lift );
      return true;
   }

   if( !is_number( arg ) )
      return false;

   floor = atoi( arg );
   for( i = 0; i < lift->stop_count; ++i )
      if( lift->stops[i].floor == floor )
         break;

   if( i >= lift->stop_count )
   {
      send_to_char( "There is no button for that floor. Look at the panel for available stops.\r\n", ch );
      return true;
   }

   if( i == lift->current_index && lift->state == ELEVATOR_DOORS_OPEN )
   {
      send_to_char( "That floor is already selected; the doors remain open.\r\n", ch );
      lift->next_action = current_time + ELEVATOR_DOOR_SECONDS;
      return true;
   }

   if( lift->requests[i] )
   {
      send_to_char( "That floor button is already illuminated.\r\n", ch );
      return true;
   }

   elevator_request( lift, i );
   ch_printf( ch, "You press floor %d. The button illuminates.\r\n", floor );
   return true;
}

static bool elevator_handle_look( CHAR_DATA *ch, const char *argument )
{
   elevator_data *lift;
   int stop_index;
   bool inside_car;
   char arg[MAX_INPUT_LENGTH];

   if( !ch || !ch->in_room )
      return false;

   one_argument( argument, arg );
   if( str_cmp( arg, "panel" ) && str_cmp( arg, "controls" )
       && str_cmp( arg, "button" ) && str_cmp( arg, "buttons" ) )
      return false;

   lift = elevator_for_room( ch->in_room, &stop_index, &inside_car );
   if( !lift || !elevator_initialize( lift ) )
      return false;

   elevator_show_panel( ch, lift, inside_car );
   return true;
}

void elevator_update( void )
{
   int e;

   for( e = 0; e < elevator_count; ++e )
   {
      elevator_data *lift = all_elevators[e];

      if( !elevator_initialize( lift ) )
         continue;

      if( lift->state == ELEVATOR_IDLE )
      {
         if( elevator_has_requests( lift ) )
            elevator_choose_direction( lift );
         continue;
      }

      if( lift->next_action > current_time )
         continue;

      if( lift->state == ELEVATOR_DOORS_OPEN )
         elevator_begin_after_doors( lift );
      else
         elevator_move_one_floor( lift );
   }
}

extern "C" void do_push( CHAR_DATA *ch, const char *argument )
{
   if( elevator_handle_push( ch, argument ) )
      return;

   legacy_do_push( ch, argument );
}

extern "C" void do_look( CHAR_DATA *ch, const char *argument )
{
   if( !elevator_handle_look( ch, argument ) )
      legacy_do_look( ch, argument );

   /* A look is the reliable initial synchronization point for stationary clients. */
   msdp_send_room( ch );
}

void update_handler( void )
{
   natural_update_handler();
   elevator_update();
}
