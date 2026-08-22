#ifndef ZEROPOINT_GAME_DIALOGUE_H
#define ZEROPOINT_GAME_DIALOGUE_H

#define MAX_PLAYER_MEMORIES       128
#define MAX_MEMORY_KEY_LENGTH      64
#define MAX_MEMORY_VALUE_LENGTH    48
#define MAX_DIALOGUE_CHOICES        4

typedef struct player_memory PLAYER_MEMORY;
typedef struct dialogue_session DIALOGUE_SESSION;

struct player_memory
{
   char key[MAX_MEMORY_KEY_LENGTH];
   char value[MAX_MEMORY_VALUE_LENGTH];
};

struct dialogue_session
{
   int node_id;
   int mob_vnum;
};

void load_dialogues( void );

const char *dialogue_memory_get( CHAR_DATA *ch, const char *key );
bool dialogue_memory_matches( CHAR_DATA *ch, const char *key, const char *value );
bool dialogue_memory_set( CHAR_DATA *ch, const char *key, const char *value );
bool dialogue_memory_clear( CHAR_DATA *ch, const char *key );

#endif /* ZEROPOINT_GAME_DIALOGUE_H */
