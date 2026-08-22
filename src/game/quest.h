#ifndef ZEROPOINT_GAME_QUEST_H
#define ZEROPOINT_GAME_QUEST_H

#define MAX_ACTIVE_QUESTS       32
#define MAX_QUEST_HISTORY      512
#define MAX_QUEST_OBJECTIVES    12
#define MAX_QUEST_REPUTATIONS   64

typedef enum
{
   QUEST_STATE_EMPTY = 0,
   QUEST_STATE_AVAILABLE,
   QUEST_STATE_ACTIVE,
   QUEST_STATE_COMPLETED,
   QUEST_STATE_FAILED
} quest_state_type;

typedef enum
{
   QUEST_OBJECTIVE_NONE = 0,
   QUEST_OBJECTIVE_KILL,
   QUEST_OBJECTIVE_GET,
   QUEST_OBJECTIVE_GIVE,
   QUEST_OBJECTIVE_ENTER,
   QUEST_OBJECTIVE_EXAMINE,
   QUEST_OBJECTIVE_INTERACT,
   QUEST_OBJECTIVE_CUSTOM
} quest_objective_type;

typedef struct quest_objective_definition QUEST_OBJECTIVE_DEFINITION;
typedef struct quest_definition QUEST_DEFINITION;
typedef struct player_quest_state PLAYER_QUEST_STATE;
typedef struct player_quest_history PLAYER_QUEST_HISTORY;
typedef struct player_reputation PLAYER_REPUTATION;

struct quest_objective_definition
{
   short type;
   short stage;
   int target_vnum;
   int aux_vnum;
   int required;
   const char *description;
};

struct quest_definition
{
   QUEST_DEFINITION *next;
   QUEST_DEFINITION *prev;
   int id;
   const char *name;
   const char *description;
   int min_level;
   int max_level;
   int prerequisite_id;
   int prerequisite_outcome;
   bool repeatable;
   int cooldown_seconds;
   int reward_xp_ability;
   int reward_xp_amount;
   int reward_credits;
   int reward_item_vnum;
   int reward_reputation_id;
   int reward_reputation_amount;
   int reward_skill_sn;
   int reward_skill_value;
   int reward_quest_points;
   short objective_count;
   QUEST_OBJECTIVE_DEFINITION objectives[MAX_QUEST_OBJECTIVES];
};

struct player_quest_state
{
   int quest_id;
   short status;
   short objective_count;
   int progress[MAX_QUEST_OBJECTIVES];
   int outcome;
   time_t started_at;
   time_t updated_at;
};

struct player_quest_history
{
   int quest_id;
   int completion_count;
   int outcome;
   time_t completed_at;
   time_t cooldown_until;
};

struct player_reputation
{
   int id;
   int value;
};

extern QUEST_DEFINITION *first_quest;
extern QUEST_DEFINITION *last_quest;

void load_quests( void );
QUEST_DEFINITION *get_quest_definition( int id );
QUEST_DEFINITION *find_quest_definition( const char *name_or_id );

bool quest_is_available( CHAR_DATA *ch, int quest_id );
bool quest_is_active( CHAR_DATA *ch, int quest_id );
bool quest_is_complete( CHAR_DATA *ch, int quest_id );
bool quest_is_failed( CHAR_DATA *ch, int quest_id );
int quest_stage( CHAR_DATA *ch, int quest_id );
int quest_outcome( CHAR_DATA *ch, int quest_id );
int quest_get_reputation( CHAR_DATA *ch, int reputation_id );

bool quest_start( CHAR_DATA *ch, int quest_id );
bool quest_advance_objective( CHAR_DATA *ch, int quest_id, int objective_index, int amount );
bool quest_complete( CHAR_DATA *ch, int quest_id, int outcome );
bool quest_fail( CHAR_DATA *ch, int quest_id );
bool quest_abandon( CHAR_DATA *ch, int quest_id );

void quest_event( CHAR_DATA *ch, int objective_type, int target_vnum, int aux_vnum );
void quest_save_player( FILE *fp, CHAR_DATA *ch );
bool quest_load_player_field( CHAR_DATA *ch, const char *word, FILE *fp );

#endif /* ZEROPOINT_GAME_QUEST_H */
