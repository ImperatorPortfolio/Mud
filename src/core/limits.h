#ifndef ZEROPOINT_CORE_LIMITS_H
#define ZEROPOINT_CORE_LIMITS_H

/*
* String and memory management parameters.
*/
#define MAX_KEY_HASH		 2048
#define MAX_STRING_LENGTH	 4096 /* buf */
#define MAX_INPUT_LENGTH	 1024 /* arg */
#define MAX_COMMAND_LENGTH	 4096 /* canonical socket/editor line */
#define MAX_INBUF_SIZE		16384 /* queued socket input */
#define MAX_LONG_TEXT_LENGTH	16384 /* persisted/editor text */
#define MAX_COLORIZED_LENGTH   ( MAX_LONG_TEXT_LENGTH * 10 )
#define MAX_EDITOR_LINES	   60
#define MAX_EDITOR_LINE_LENGTH MAX_COMMAND_LENGTH

#define HASHSTR   /* use string hashing */

#define	MAX_LAYERS		 8 /* maximum clothing layers */
#define MAX_NEST	       100  /* maximum container nesting */

#define MAX_KILLTRACK		20 /* track mob vnums killed */

/*
* Game parameters.
* Increase the max'es if you add more of something.
* Adjust the pulse numbers to suit yourself.
*/
#define MAX_EXP_WORTH	       500000
#define MIN_EXP_WORTH		   25

#define MAX_REXITS		   20 /* Maximum exits allowed in 1 room */
#define MAX_SKILL		  276
#define MAX_ABILITY		    8
#define MAX_RACE		   17
#define MAX_NPC_RACE		   91
#define MAX_LEVEL		  105
#define MAX_CLAN		   50
#define MAX_PLANET		  100
#define MAX_SHIP                 1000
#define MAX_BOUNTY                255
#define MAX_GOV                   255

#define	MAX_HERB		   20

#define LEVEL_HERO		   (MAX_LEVEL - 5)
#define LEVEL_IMMORTAL		   (MAX_LEVEL - 4)
#define LEVEL_SUPREME		   MAX_LEVEL
#define LEVEL_INFINITE		   (MAX_LEVEL - 1)
#define LEVEL_ETERNAL		   (MAX_LEVEL - 1)
#define LEVEL_IMPLEMENTOR	   (MAX_LEVEL - 1)
#define LEVEL_SUB_IMPLEM	   (MAX_LEVEL - 1)
#define LEVEL_ASCENDANT		   (MAX_LEVEL - 2)
#define LEVEL_GREATER		   (MAX_LEVEL - 2)
#define LEVEL_GOD		   (MAX_LEVEL - 2)
#define LEVEL_LESSER		   (MAX_LEVEL - 3)
#define LEVEL_TRUEIMM		   (MAX_LEVEL - 3)
#define LEVEL_DEMI		   (MAX_LEVEL - 3)
#define LEVEL_SAVIOR		   (MAX_LEVEL - 3)
#define LEVEL_CREATOR		   (MAX_LEVEL - 3)
#define LEVEL_ACOLYTE		   (MAX_LEVEL - 4)
#define LEVEL_NEOPHYTE		   (MAX_LEVEL - 4)
#define LEVEL_AVATAR		   (MAX_LEVEL - 5)

#include "../hotboot.h"

#define LEVEL_LOG		    LEVEL_LESSER
#define LEVEL_HIGOD		    LEVEL_GOD

#define PULSE_PER_SECOND	    4
#define PULSE_MINUTE              ( 60 * PULSE_PER_SECOND)
#define PULSE_VIOLENCE		  (  3 * PULSE_PER_SECOND)
#define PULSE_MOBILE		  (  4 * PULSE_PER_SECOND)
#define PULSE_TICK		  ( 70 * PULSE_PER_SECOND)
#define PULSE_AREA		  ( 60 * PULSE_PER_SECOND)
#define PULSE_AUCTION             ( 10 * PULSE_PER_SECOND)
#define PULSE_SPACE               ( 10 * PULSE_PER_SECOND)
#define PULSE_TAXES               ( 60 * PULSE_MINUTE)

/* 
 * Stuff for area versions --Shaddai
 */
#define HAS_SPELL_INDEX     -1

/*
Old Smaug area version identifiers:

Version 1: Stock 1.4a areas.
Version 2: Skipped - Probably won't ever see these, but originated from Smaug 1.8.
Version 3: Stock 1.8 areas.
*/

// This value has been reset due to the new KEY/Value based area format.
// It will not conflict with the above former area file versions.
#define AREA_VERSION_WRITE 1

/*
* Command logging types.
*/

#endif /* ZEROPOINT_CORE_LIMITS_H */
