#ifndef ZEROPOINT_WORLD_AFFECT_H
#define ZEROPOINT_WORLD_AFFECT_H

struct affect_data
{
   AFFECT_DATA *next;
   AFFECT_DATA *prev;
   short type;
   int duration;
   short location;
   int modifier;
   int bitvector;
};

/*
* A SMAUG spell
*/
struct smaug_affect
{
   SMAUG_AFF *next;
   SMAUG_AFF *prev;
   const char *duration;
   short location;
   const char *modifier;
   int bitvector;
};

/***************************************************************************
*                                                                         *
*                   VALUES OF INTEREST TO AREA BUILDERS                   *
*                   (Start of section ... start here)                     *
*                                                                         *
***************************************************************************/


#endif /* ZEROPOINT_WORLD_AFFECT_H */
