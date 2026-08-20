#ifndef ZEROPOINT_WORLD_ROOM_METADATA_H
#define ZEROPOINT_WORLD_ROOM_METADATA_H

#define ROOM_ACTION_TAG_MAX 12

struct room_action_tag
{
   const char *label;
   const char *color;
};

typedef struct room_action_tag ROOM_ACTION_TAG;

int room_collect_action_tags( const ROOM_INDEX_DATA *room, ROOM_ACTION_TAG *tags, int max_tags );
const char *room_primary_action_color( const ROOM_INDEX_DATA *room );

#endif /* ZEROPOINT_WORLD_ROOM_METADATA_H */
