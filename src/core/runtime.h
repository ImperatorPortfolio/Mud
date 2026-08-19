#ifndef ZEROPOINT_CORE_RUNTIME_H
#define ZEROPOINT_CORE_RUNTIME_H

typedef enum
{
   LOG_NORMAL, LOG_ALWAYS, LOG_NEVER, LOG_BUILD, LOG_HIGH, LOG_COMM, LOG_ALL
} log_types;

/* short cut crash bug fix provided by gfinello@mail.karmanet.it*/
typedef enum
{
   relMSET_ON, relOSET_ON
} relation_type;

typedef struct rel_data REL_DATA;

struct rel_data
{
   void *Actor;
   void *Subject;
   REL_DATA *next;
   REL_DATA *prev;
   relation_type Type;
};

/*
* Return types for move_char, damage, greet_trigger, etc, etc
* Added by Thoric to get rid of bugs
*/
typedef enum
{
   rNONE, rCHAR_DIED, rVICT_DIED, rBOTH_DIED, rCHAR_QUIT, rVICT_QUIT,
   rBOTH_QUIT, rSPELL_FAILED, rOBJ_SCRAPPED, rOBJ_EATEN, rOBJ_EXPIRED,
   rOBJ_TIMER, rOBJ_SACCED, rOBJ_QUAFFED, rOBJ_USED, rOBJ_EXTRACTED,
   rOBJ_DRUNK, rCHAR_IMMUNE, rVICT_IMMUNE, rCHAR_AND_OBJ_EXTRACTED = 128,
   rERROR = 255
} ret_types;

/* Echo types for echo_to_all */
#define ECHOTAR_ALL	0
#define ECHOTAR_PC	1
#define ECHOTAR_IMM	2

/* defines for new do_who */
#define WT_MORTAL 0
#define WT_IMM    2
#define WT_AVATAR 1
#define WT_NEWBIE 3

#include <stdio.h>
#include "../dns.h"
#include "../color.h"

/*
* do_who output structure -- Narn
*/
struct who_data
{
   WHO_DATA *prev;
   WHO_DATA *next;
   const char *text;
   int type;
};

/*
* Site ban structure.
*/
struct ban_data
{
   BAN_DATA *next;
   BAN_DATA *prev;
   const char *name;
   int level;
   const char *ban_time;
};

/*
* Time and weather stuff.
*/
typedef enum
{
   SUN_DARK, SUN_RISE, SUN_LIGHT, SUN_SET
} sun_positions;

typedef enum
{
   SKY_CLOUDLESS, SKY_CLOUDY, SKY_RAINING, SKY_LIGHTNING
} sky_conditions;

struct time_info_data
{
   int hour;
   int day;
   int month;
   int year;
};

struct hour_min_sec
{
   int hour;
   int min;
   int sec;
   int manual;
};

struct weather_data
{
   int mmhg;
   int change;
   int sky;
   int sunlight;
};

/*
* Structure used to build wizlist
*/
struct wizent
{
   WIZENT *next;
   WIZENT *last;
   const char *name;
   short level;
};

/*
* Connected state for a channel.
*/
typedef enum
{
   CON_GET_NAME = -99,
   CON_GET_OLD_PASSWORD, CON_CONFIRM_NEW_NAME,
   CON_GET_NEW_PASSWORD, CON_CONFIRM_NEW_PASSWORD,
   CON_GET_NEW_SEX, CON_GET_NEW_CLASS, CON_READ_MOTD,
   CON_GET_NEW_RACE, CON_GET_EMULATION, CON_GET_MSP,
   CON_ROLL_STATS, CON_STATS_OK,
   CON_GET_WANT_RIPANSI, CON_TITLE, CON_PRESS_ENTER,
   CON_WAIT_1, CON_WAIT_2, CON_WAIT_3,
   CON_ACCEPTED, CON_GET_PKILL, CON_READ_IMOTD,
   CON_COPYOVER_RECOVER, CON_PLAYING = 0, CON_EDITING
} connection_types;

/*
* Character substates
*/
typedef enum
{
   SUB_NONE, SUB_PAUSE, SUB_PERSONAL_DESC, SUB_OBJ_SHORT, SUB_OBJ_LONG,
   SUB_OBJ_EXTRA, SUB_MOB_LONG, SUB_MOB_DESC, SUB_ROOM_DESC, SUB_ROOM_EXTRA,
   SUB_ROOM_EXIT_DESC, SUB_WRITING_NOTE, SUB_MPROG_EDIT, SUB_HELP_EDIT,
   SUB_WRITING_MAP, SUB_PERSONAL_BIO, SUB_REPEATCMD, SUB_RESTRICTED,
   /*
   * timer types ONLY below this point
   */
   SUB_TIMER_DO_ABORT = 128, SUB_TIMER_CANT_ABORT
} char_substates;

/*
* Descriptor (channel) structure.
*/
#define MSDP_INPUT_MAX 512

struct descriptor_data
{
   DESCRIPTOR_DATA *next;
   DESCRIPTOR_DATA *prev;
   DESCRIPTOR_DATA *snoop_by;
   CHAR_DATA *character;
   CHAR_DATA *original;
   struct mccp_data *mccp; /* Mud Client Compression Protocol */
   bool can_compress;
   bool msdp_announced; /* MSDP option has been offered for this connection */
   bool msdp_enabled; /* Client accepted WILL MSDP with DO MSDP */
   bool msdp_report_room; /* Client subscribed to REPORT ROOM */
   unsigned char telnet_state; /* Incremental telnet parser state */
   unsigned char telnet_command;
   unsigned char telnet_option;
   unsigned short msdp_input_length;
   unsigned char msdp_input[MSDP_INPUT_MAX];
   const char *host;
   const char *hostip;
   int port;
   int descriptor;
   short connected;
   short idle;
   short lines;
   short scrlen;
   bool fcommand;
   char inbuf[MAX_INBUF_SIZE];
   char incomm[MAX_INPUT_LENGTH];
   char inlast[MAX_INPUT_LENGTH];
   int repeat;
   char *outbuf;
   unsigned long outsize;
   int outtop;
   char *pagebuf;
   unsigned long pagesize;
   int pagetop;
   const char *pagepoint;
   char pagecmd;
   char pagecolor;
   int newstate;
   unsigned char prevcolor;
   int ifd;
   pid_t ipid;
};

/*
* Attribute bonus structures.
*/

#endif /* ZEROPOINT_CORE_RUNTIME_H */
