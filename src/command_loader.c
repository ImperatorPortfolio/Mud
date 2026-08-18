/***************************************************************************
 * Zero Point - Source-backed command extensions
 *
 * Keep the persistent command file authoritative for inherited commands,
 * then register bounded source-backed additions that are part of the engine.
 ***************************************************************************/

#include "mud.h"

void legacy_load_commands( void );
void add_command( CMDTYPE *command );
extern "C" void do_exercise( CHAR_DATA *ch, const char *argument );

static bool command_exists( const char *name )
{
   CMDTYPE *command;
   int hash;

   if( !name || name[0] == '\0' )
      return FALSE;

   hash = LOWER( name[0] ) % 126;

   for( command = command_hash[hash]; command; command = command->next )
      if( command->name && !str_cmp( command->name, name ) )
         return TRUE;

   return FALSE;
}

void load_commands( void )
{
   CMDTYPE *command;

   legacy_load_commands();

   if( command_exists( "exercise" ) )
      return;

   CREATE( command, CMDTYPE, 1 );

   command->name = strdup( "exercise" );
   command->fun_name = strdup( "do_exercise" );
   command->do_fun = do_exercise;
   command->position = POS_STANDING;
   command->level = 1;
   command->log = 0;

   add_command( command );
}
