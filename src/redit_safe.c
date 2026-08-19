/***************************************************************************
 * Zero Point - safe bidirectional room-exit editing
 *
 * The inherited `redit bexit <dir>` removal path delegates the reverse-side
 * delete through `do_at` and identifies that side by direction alone. In a
 * dense room graph that can select an unrelated exit and leave stale exit
 * ownership behind. This wrapper intercepts deletion only and removes the
 * exact source/destination pair directly. All other redit behaviour remains
 * in build.c under legacy_do_redit().
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "mud.h"

extern void legacy_do_redit( CHAR_DATA *ch, const char *argument );

static void safe_remove_bexit( CHAR_DATA *ch, const char *direction_arg )
{
   ROOM_INDEX_DATA *source;
   ROOM_INDEX_DATA *destination;
   EXIT_DATA *forward;
   EXIT_DATA *reverse;
   short direction;

   if( !ch || !ch->desc || !( source = ch->in_room ) )
      return;

   if( !can_rmodify( ch, source ) )
      return;

   if( direction_arg[0] == '#' )
   {
      forward = get_exit_num( source, atoi( direction_arg + 1 ) );
      if( !forward )
      {
         send_to_char( "No exit with that number.\r\n", ch );
         return;
      }
      direction = forward->vdir;
   }
   else
   {
      const char *dir_text = direction_arg[0] == '+' ? direction_arg + 1 : direction_arg;
      direction = get_dir( dir_text );
      forward = get_exit( source, direction );
      if( !forward )
      {
         send_to_char( "No exit in that direction.\r\n", ch );
         return;
      }
   }

   destination = forward->to_room;
   reverse = NULL;

   /* Match the reverse edge by BOTH direction and source VNUM. Never remove
    * an arbitrary exit merely because it occupies the reverse direction. */
   if( destination )
      reverse = get_exit_to( destination, rev_dir[direction], source->vnum );

   /* Preserve builder permissions on both rooms. The inherited implementation
    * checked the destination when it executed the reverse edit through do_at. */
   if( reverse && !can_rmodify( ch, destination ) )
      return;

   /* Remove the counterpart first. extract_exit() clears its peer's rexit
    * pointer, so the subsequent source removal cannot dereference freed data. */
   if( reverse && reverse != forward )
      extract_exit( destination, reverse );

   extract_exit( source, forward );

   if( reverse )
      send_to_char( "Two-way exit removed.\r\n", ch );
   else
      send_to_char( "Exit removed; no matching reverse exit existed.\r\n", ch );
}

void do_redit( CHAR_DATA *ch, const char *argument )
{
   char command[MAX_INPUT_LENGTH];
   char direction[MAX_INPUT_LENGTH];
   char destination[MAX_INPUT_LENGTH];
   const char *rest;

   if( !argument )
   {
      legacy_do_redit( ch, "" );
      return;
   }

   rest = one_argument( argument, command );
   if( str_cmp( command, "bexit" ) )
   {
      legacy_do_redit( ch, argument );
      return;
   }

   rest = one_argument( rest, direction );
   one_argument( rest, destination );

   /* Only the no-room form is deletion. Creation/change remains entirely in
    * the inherited editor to preserve flags, keys, keywords and builder UX. */
   if( direction[0] == '\0' || destination[0] != '\0' )
   {
      legacy_do_redit( ch, argument );
      return;
   }

   safe_remove_bexit( ch, direction );
}
