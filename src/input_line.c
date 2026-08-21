/***************************************************************************
 * Zero Point canonical input-line parser.
 *
 * Replaces the inherited 254-byte command cap with the actual descriptor
 * command-buffer capacity.  This is important for builder commands that
 * legitimately contain dense RGB markup while preserving the old parsing,
 * repeat, telnet-IAC and line-shifting behavior.
 ***************************************************************************/

#include <ctype.h>
#include <string.h>
#include <arpa/telnet.h>
#include "mud.h"
#include "mccp.h"

bool write_to_descriptor( DESCRIPTOR_DATA *d, const char *txt, int length );

void read_from_buffer( DESCRIPTOR_DATA *d )
{
   int i, j, k, iac = 0;
   const int command_limit = ( int )sizeof( d->incomm ) - 2;

   /* Hold horses if pending command already. */
   if( d->incomm[0] != '\0' )
      return;

   /* Look for at least one new line. */
   for( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r' && i < MAX_INBUF_SIZE; i++ )
   {
      if( d->inbuf[i] == '\0' )
         return;
   }

   /* Canonical input processing. */
   for( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
   {
      if( k >= command_limit )
      {
         write_to_descriptor( d, "Line too long.\r\n", 0 );
         d->inbuf[i] = '\n';
         d->inbuf[i + 1] = '\0';
         break;
      }

      if( d->inbuf[i] == ( signed char )IAC )
         iac = 1;
      else if( iac == 1
               && ( d->inbuf[i] == ( signed char )DO || d->inbuf[i] == ( signed char )DONT
                    || d->inbuf[i] == ( signed char )WILL ) )
         iac = 2;
      else if( iac == 2 )
      {
         iac = 0;
         if( d->inbuf[i] == ( signed char )TELOPT_COMPRESS2 )
         {
            if( d->inbuf[i - 1] == ( signed char )DO )
               compressStart( d );
            else if( d->inbuf[i - 1] == ( signed char )DONT )
               compressEnd( d );
         }
      }
      else if( d->inbuf[i] == '\b' && k > 0 )
         --k;
      else if( isascii( d->inbuf[i] ) && isprint( d->inbuf[i] ) )
         d->incomm[k++] = d->inbuf[i];
   }

   /* Finish off the line. */
   if( k == 0 )
      d->incomm[k++] = ' ';
   d->incomm[k] = '\0';

   /* Deal with command-repeat spam exactly as the inherited parser does. */
   if( k > 1 || d->incomm[0] == '!' )
   {
      if( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
         d->repeat = 0;
      else if( ++d->repeat >= 20 )
         write_to_descriptor( d, "\r\n*** PUT A LID ON IT!!! ***\r\n", 0 );
   }

   /* Do '!' substitution. */
   if( d->incomm[0] == '!' )
      strlcpy( d->incomm, d->inlast, MAX_INPUT_LENGTH );
   else
      strlcpy( d->inlast, d->incomm, MAX_INPUT_LENGTH );

   /* Shift the input buffer. */
   while( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
      i++;
   for( j = 0; ( d->inbuf[j] = d->inbuf[i + j] ) != '\0'; j++ )
      ;
}
