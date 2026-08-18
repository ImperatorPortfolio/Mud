#ifndef ZEROPOINT_CORE_TYPES_H
#define ZEROPOINT_CORE_TYPES_H

typedef struct affect_data AFFECT_DATA;
typedef struct area_data AREA_DATA;
typedef struct auction_data AUCTION_DATA; /* auction data */
typedef struct ban_data BAN_DATA;
typedef struct extracted_char_data EXTRACT_CHAR_DATA;
typedef struct char_data CHAR_DATA;
typedef struct hunt_hate_fear HHF_DATA;
typedef struct fighting_data FIGHT_DATA;
typedef struct descriptor_data DESCRIPTOR_DATA;
typedef struct exit_data EXIT_DATA;
typedef struct extra_descr_data EXTRA_DESCR_DATA;
typedef struct help_data HELP_DATA;
typedef struct mob_index_data MOB_INDEX_DATA;
typedef struct note_data NOTE_DATA;
typedef struct board_data BOARD_DATA;
typedef struct obj_data OBJ_DATA;
typedef struct obj_index_data OBJ_INDEX_DATA;
typedef struct pc_data PC_DATA;
typedef struct reset_data RESET_DATA;
typedef struct room_index_data ROOM_INDEX_DATA;
typedef struct shop_data SHOP_DATA;
typedef struct repairshop_data REPAIR_DATA;
typedef struct time_info_data TIME_INFO_DATA;
typedef struct hour_min_sec HOUR_MIN_SEC;
typedef struct weather_data WEATHER_DATA;
typedef struct bounty_data BOUNTY_DATA;
typedef struct planet_data PLANET_DATA;
typedef struct guard_data GUARD_DATA;
typedef struct space_data SPACE_DATA;
typedef struct clan_data CLAN_DATA;
typedef struct senate_data SENATE_DATA;
typedef struct ship_data SHIP_DATA;
typedef struct missile_data MISSILE_DATA;
typedef struct mob_prog_data MPROG_DATA;
typedef struct mob_prog_act_list MPROG_ACT_LIST;
typedef struct editor_data EDITOR_DATA;
typedef struct teleport_data TELEPORT_DATA;
typedef struct timer_data TIMER;
typedef struct system_data SYSTEM_DATA;
typedef struct smaug_affect SMAUG_AFF;
typedef struct who_data WHO_DATA;
typedef struct skill_type SKILLTYPE;
typedef struct social_type SOCIALTYPE;
typedef struct cmd_type CMDTYPE;
typedef struct killed_data KILLED_DATA;
typedef struct wizent WIZENT;
typedef struct specfun_list SPEC_LIST;

/*
* Function types.
*/
typedef void DO_FUN( CHAR_DATA * ch, const char *argument );
typedef bool SPEC_FUN( CHAR_DATA * ch );
typedef ch_ret SPELL_FUN( int sn, int level, CHAR_DATA * ch, void *vo );


#endif /* ZEROPOINT_CORE_TYPES_H */
