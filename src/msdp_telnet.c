/***************************************************************************
 * Zero Point - bounded telnet/MSDP input handling
 *
 * comm.c keeps ownership of descriptor IO and canonical command parsing.
 * This reader filters telnet control traffic before command text reaches the
 * inherited line parser, preserving fragmented negotiations across reads.
 ***************************************************************************/

#include <errno.h>
#include <unistd.h>
#include <vector>
#include <arpa/telnet.h>
#include "mud.h"
#include "mccp.h"

#define ZP_TELOPT_MSDP 69

enum telnet_input_state
{
   TELNET_DATA = 0,
   TELNET_IAC,
   TELNET_NEGOTIATE,
   TELNET_SB_OPTION,
   TELNET_SB_DATA,
   TELNET_SB_IAC
};

extern DESCRIPTOR_DATA *first_descriptor;
extern void msdp_set_enabled( DESCRIPTOR_DATA *d, bool enabled );
extern void msdp_handle_subnegotiation( DESCRIPTOR_DATA *d, const unsigned char *data, size_t length );

static DESCRIPTOR_DATA *descriptor_for_fd( int fd )
{
   DESCRIPTOR_DATA *d;
   for( d = first_descriptor; d; d = d->next )
      if( d->descriptor == fd )
         return d;
   return NULL;
}

static void handle_negotiation( DESCRIPTOR_DATA *d, unsigned char command, unsigned char option )
{
   if( !d )
      return;

   if( option == TELOPT_COMPRESS2 )
   {
      if( command == DO )
         compressStart( d );
      else if( command == DONT )
         compressEnd( d );
      return;
   }

   if( option == ZP_TELOPT_MSDP )
   {
      if( command == DO )
         msdp_set_enabled( d, true );
      else if( command == DONT )
         msdp_set_enabled( d, false );
   }
}

static size_t filter_telnet_bytes( DESCRIPTOR_DATA *d, const unsigned char *input, size_t input_length,
                                   unsigned char *output )
{
   size_t out = 0;

   for( size_t i = 0; i < input_length; ++i )
   {
      const unsigned char byte = input[i];

      switch( d->telnet_state )
      {
         case TELNET_DATA:
            if( byte == IAC )
               d->telnet_state = TELNET_IAC;
            else
               output[out++] = byte;
            break;

         case TELNET_IAC:
            if( byte == IAC )
            {
               output[out++] = IAC;
               d->telnet_state = TELNET_DATA;
            }
            else if( byte == DO || byte == DONT || byte == WILL || byte == WONT )
            {
               d->telnet_command = byte;
               d->telnet_state = TELNET_NEGOTIATE;
            }
            else if( byte == SB )
            {
               d->telnet_option = 0;
               d->msdp_input_length = 0;
               d->telnet_state = TELNET_SB_OPTION;
            }
            else
               d->telnet_state = TELNET_DATA;
            break;

         case TELNET_NEGOTIATE:
            handle_negotiation( d, d->telnet_command, byte );
            d->telnet_command = 0;
            d->telnet_state = TELNET_DATA;
            break;

         case TELNET_SB_OPTION:
            d->telnet_option = byte;
            d->msdp_input_length = 0;
            d->telnet_state = TELNET_SB_DATA;
            break;

         case TELNET_SB_DATA:
            if( byte == IAC )
               d->telnet_state = TELNET_SB_IAC;
            else if( d->telnet_option == ZP_TELOPT_MSDP && d->msdp_input_length < MSDP_INPUT_MAX )
               d->msdp_input[d->msdp_input_length++] = byte;
            break;

         case TELNET_SB_IAC:
            if( byte == SE )
            {
               if( d->telnet_option == ZP_TELOPT_MSDP )
                  msdp_handle_subnegotiation( d, d->msdp_input, d->msdp_input_length );
               d->telnet_option = 0;
               d->msdp_input_length = 0;
               d->telnet_state = TELNET_DATA;
            }
            else if( byte == IAC )
            {
               if( d->telnet_option == ZP_TELOPT_MSDP && d->msdp_input_length < MSDP_INPUT_MAX )
                  d->msdp_input[d->msdp_input_length++] = IAC;
               d->telnet_state = TELNET_SB_DATA;
            }
            else
               d->telnet_state = TELNET_SB_DATA;
            break;

         default:
            d->telnet_state = TELNET_DATA;
            break;
      }
   }

   return out;
}

ssize_t zeropoint_telnet_read( int fd, void *buf, size_t count )
{
   DESCRIPTOR_DATA *d = descriptor_for_fd( fd );

   if( !d )
      return ::read( fd, buf, count );

   if( count == 0 )
      return 0;

   std::vector<unsigned char> raw( count );

   for( ;; )
   {
      ssize_t received = ::read( fd, raw.data(), count );
      if( received <= 0 )
         return received;

      size_t visible = filter_telnet_bytes( d, raw.data(), ( size_t )received, ( unsigned char * )buf );
      if( visible > 0 )
         return ( ssize_t )visible;

      /* A read containing only telnet control traffic is not EOF. Try once
       * more; on a nonblocking socket EWOULDBLOCK returns control to comm.c. */
   }
}
