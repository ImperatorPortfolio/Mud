#ifndef ZEROPOINT_GAME_SOCIAL_H
#define ZEROPOINT_GAME_SOCIAL_H

struct tourney_data
{
   int open;
   int low_level;
   int hi_level;
};

/*
* Data structure for notes.
*/
struct note_data
{
   NOTE_DATA *next;
   NOTE_DATA *prev;
   const char *sender;
   const char *date;
   const char *to_list;
   const char *subject;
   int voting;
   const char *yesvotes;
   const char *novotes;
   const char *abstentions;
   const char *text;
};

struct board_data
{
   BOARD_DATA *next; /* Next board in list         */
   BOARD_DATA *prev; /* Previous board in list     */
   NOTE_DATA *first_note;  /* First note on board        */
   NOTE_DATA *last_note;   /* Last note on board         */
   const char *note_file;  /* Filename to save notes to     */
   const char *read_group; /* Can restrict a board to a       */
   const char *post_group; /* council, clan, guild etc        */
   const char *extra_readers; /* Can give read rights to players */
   const char *extra_removers;   /* Can give remove rights to players */
   int board_obj; /* Vnum of board object       */
   short num_posts;  /* Number of notes on this board   */
   short min_read_level;   /* Minimum level to read a note     */
   short min_post_level;   /* Minimum level to post a note    */
   short min_remove_level; /* Minimum level to remove a note  */
   short max_posts;  /* Maximum amount of notes allowed */
   int type;   /* Normal board or mail board? */
};

/*
* An affect.
*/

#endif /* ZEROPOINT_GAME_SOCIAL_H */
