#ifndef ZEROPOINT_GAME_API_H
#define ZEROPOINT_GAME_API_H

{
   SPEC_LIST *next;
   SPEC_LIST *prev;
   const char *name;
};

/*
* Global constants.
*/
extern time_t last_restore_all_time;
extern time_t boot_time;   /* this should be moved down */
extern HOUR_MIN_SEC *set_boot_time;
extern struct tm *new_boot_time;
extern time_t new_boot_time_t;
extern bool mud_down;
extern bool DONT_UPPER;
extern FILE *fpArea;
extern char strArea[MAX_INPUT_LENGTH];

extern const struct str_app_type str_app[26];
extern const struct int_app_type int_app[26];
extern const struct wis_app_type wis_app[26];
extern const struct dex_app_type dex_app[26];
extern const struct con_app_type con_app[26];
extern const struct cha_app_type cha_app[26];
extern const struct lck_app_type lck_app[26];
extern const struct frc_app_type frc_app[26];

extern const struct race_type race_table[MAX_RACE];
extern const struct liq_type liq_table[LIQ_MAX];
extern const char *const attack_table[13];
extern const char *const ability_name[MAX_ABILITY];
extern const char *const skill_tname[];
extern short const movement_loss[SECT_MAX];
extern const char *const dir_name[];
extern const char *const where_name[];
extern const short rev_dir[];
extern const int trap_door[];
extern const char *const r_flags[];
extern const char *const w_flags[];
extern const char *const o_flags[];
extern const char *const a_flags[];
extern const char *const o_types[];
extern const char *const a_types[];
extern const char *const act_flags[];
extern const char *const planet_flags[];
extern const char *const weapon_table[13];
extern const char *const spice_table[];
extern const char *const plr_flags[];
extern const char *const pc_flags[];
extern const char *const trap_flags[];
extern const char *const ris_flags[];
extern const char *const trig_flags[];
extern const char *const part_flags[];
extern const char *const npc_race[];
extern const char *const defense_flags[];
extern const char *const attack_flags[];
extern const char *const area_flags[];
extern const char *const wear_locs[];
extern int const lang_array[];
extern const char *const lang_names[];
extern const char *const lang_names_save[];
extern const char *sector_name[SECT_MAX];

/*
* Global variables.
*/
extern int numobjsloaded;
extern int nummobsloaded;
extern int physicalobjects;
extern int num_descriptors;
extern struct system_data sysdata;
extern int top_sn;
extern int top_vroom;
extern int top_herb;

extern CMDTYPE *command_hash[126];

extern SKILLTYPE *skill_table[MAX_SKILL];
extern SOCIALTYPE *social_index[27];
extern CHAR_DATA *cur_char;
extern ROOM_INDEX_DATA *cur_room;
extern bool cur_char_died;
extern ch_ret global_retcode;
extern SKILLTYPE *herb_table[MAX_HERB];

extern int cur_obj;
extern int cur_obj_serial;
extern bool cur_obj_extracted;
extern obj_ret global_objcode;

extern HELP_DATA *first_help;
extern HELP_DATA *last_help;
extern SHOP_DATA *first_shop;
extern SHOP_DATA *last_shop;
extern REPAIR_DATA *first_repair;
extern REPAIR_DATA *last_repair;
extern SPEC_LIST *first_specfun;
extern SPEC_LIST *last_specfun;
extern BAN_DATA *first_ban;
extern BAN_DATA *last_ban;
extern CHAR_DATA *first_char;
extern CHAR_DATA *last_char;
extern DESCRIPTOR_DATA *first_descriptor;
extern DESCRIPTOR_DATA *last_descriptor;
extern BOARD_DATA *first_board;
extern BOARD_DATA *last_board;
extern OBJ_DATA *first_object;
extern OBJ_DATA *last_object;
extern CLAN_DATA *first_clan;
extern CLAN_DATA *last_clan;
extern GUARD_DATA *first_guard;
extern GUARD_DATA *last_guard;
extern SHIP_DATA *first_ship;
extern SHIP_DATA *last_ship;
extern SPACE_DATA *first_starsystem;
extern SPACE_DATA *last_starsystem;
extern PLANET_DATA *first_planet;
extern PLANET_DATA *last_planet;
extern SENATE_DATA *first_senator;
extern SENATE_DATA *last_senator;
extern BOUNTY_DATA *first_bounty;
extern BOUNTY_DATA *last_bounty;
extern BOUNTY_DATA *first_disintigration;
extern BOUNTY_DATA *last_disintigration;
extern AREA_DATA *first_area;
extern AREA_DATA *last_area;
extern AREA_DATA *first_build;
extern AREA_DATA *last_build;
extern AREA_DATA *first_asort;
extern AREA_DATA *last_asort;
extern AREA_DATA *first_bsort;
extern AREA_DATA *last_bsort;
extern AREA_DATA *first_area_name;  /*alphanum. sort */
extern AREA_DATA *last_area_name;   /* Fireblade */
extern TELEPORT_DATA *first_teleport;
extern TELEPORT_DATA *last_teleport;
extern OBJ_DATA *extracted_obj_queue;
extern EXTRACT_CHAR_DATA *extracted_char_queue;
extern OBJ_DATA *save_equipment[MAX_WEAR][MAX_LAYERS];
extern CHAR_DATA *quitting_char;
extern CHAR_DATA *loading_char;
extern CHAR_DATA *saving_char;
extern OBJ_DATA *all_obj;

extern time_t current_time;
extern bool fLogAll;
extern char log_buf[];
extern TIME_INFO_DATA time_info;
extern WEATHER_DATA weather_info;

extern AUCTION_DATA *auction;
extern struct act_prog_data *mob_act_list;

/*
* Command functions.
* Defined in act_*.c (mostly).
*/
DECLARE_DO_FUN( do_setmssp );
DECLARE_DO_FUN( do_setplanet );
DECLARE_DO_FUN( do_makeplanet );
DECLARE_DO_FUN( do_planets );
DECLARE_DO_FUN( do_teach );
DECLARE_DO_FUN( do_gather_intelligence );
DECLARE_DO_FUN( do_add_patrol );
DECLARE_DO_FUN( do_special_forces );
DECLARE_DO_FUN( do_jail );
DECLARE_DO_FUN( do_elite_guard );
DECLARE_DO_FUN( do_smalltalk );
DECLARE_DO_FUN( do_propeganda );
DECLARE_DO_FUN( do_bribe );
DECLARE_DO_FUN( do_seduce );
DECLARE_DO_FUN( do_mass_propeganda );
DECLARE_DO_FUN( do_copyship );
DECLARE_DO_FUN( do_sound );
DECLARE_DO_FUN( do_autopilot );
DECLARE_DO_FUN( do_allspeeders );
DECLARE_DO_FUN( do_speeders );
DECLARE_DO_FUN( do_suicide );
DECLARE_DO_FUN( do_gain );
DECLARE_DO_FUN( do_train );
DECLARE_DO_FUN( do_traits );
DECLARE_DO_FUN( do_beg );
DECLARE_DO_FUN( do_bank );
DECLARE_DO_FUN( do_hijack );
DECLARE_DO_FUN( do_pickshiplock );
DECLARE_DO_FUN( do_shiptalk );
DECLARE_DO_FUN( do_clone );
DECLARE_DO_FUN( do_systemtalk );
DECLARE_DO_FUN( do_spacetalk );
DECLARE_DO_FUN( do_hail );
DECLARE_DO_FUN( do_allships );
DECLARE_DO_FUN( do_newclan );
DECLARE_DO_FUN( do_appoint );
DECLARE_DO_FUN( do_demote );
DECLARE_DO_FUN( do_empower );
DECLARE_DO_FUN( do_capture );
DECLARE_DO_FUN( do_arm );
DECLARE_DO_FUN( do_chaff );
DECLARE_DO_FUN( do_clan_donate );
DECLARE_DO_FUN( do_clan_withdraw );
DECLARE_DO_FUN( do_fly );
DECLARE_DO_FUN( do_drive );
DECLARE_DO_FUN( do_bomb );
DECLARE_DO_FUN( do_setblaster );
DECLARE_DO_FUN( do_ammo );
DECLARE_DO_FUN( do_takedrug );
DECLARE_DO_FUN( do_use );
DECLARE_DO_FUN( do_enlist );
DECLARE_DO_FUN( do_resign );
DECLARE_DO_FUN( do_pluogus );
DECLARE_DO_FUN( do_tractorbeam );
DECLARE_DO_FUN( do_makearmor );
DECLARE_DO_FUN( do_makejewelry );
DECLARE_DO_FUN( do_makegrenade );
DECLARE_DO_FUN( do_makelandmine );
DECLARE_DO_FUN( do_makelight );
DECLARE_DO_FUN( do_makecomlink );
DECLARE_DO_FUN( do_makeshield );
DECLARE_DO_FUN( do_makecontainer );
DECLARE_DO_FUN( do_gemcutting );
DECLARE_DO_FUN( do_reinforcements );
DECLARE_DO_FUN( do_postguard );
DECLARE_DO_FUN( do_torture );
DECLARE_DO_FUN( do_snipe );
DECLARE_DO_FUN( do_throw );
DECLARE_DO_FUN( do_disguise );
DECLARE_DO_FUN( do_mine );
DECLARE_DO_FUN( do_first_aid );
DECLARE_DO_FUN( do_makeblade );
DECLARE_DO_FUN( do_makeblaster );
DECLARE_DO_FUN( do_makelightsaber );
DECLARE_DO_FUN( do_makespice );
DECLARE_DO_FUN( do_closebay );
DECLARE_DO_FUN( do_openbay );
DECLARE_DO_FUN( do_autotrack );
DECLARE_DO_FUN( do_jumpvector );
DECLARE_DO_FUN( do_reload );
DECLARE_DO_FUN( do_radar );
DECLARE_DO_FUN( do_buyship );
DECLARE_DO_FUN( do_buyhome );
DECLARE_DO_FUN( do_clanbuyship );
DECLARE_DO_FUN( do_sellship );
DECLARE_DO_FUN( do_autorecharge );
DECLARE_DO_FUN( do_openhatch );
DECLARE_DO_FUN( do_closehatch );
DECLARE_DO_FUN( do_status );
DECLARE_DO_FUN( do_info );
DECLARE_DO_FUN( do_hyperspace );
DECLARE_DO_FUN( do_target );
DECLARE_DO_FUN( do_fire );
DECLARE_DO_FUN( do_calculate );
DECLARE_DO_FUN( do_recharge );
DECLARE_DO_FUN( do_repairship );
DECLARE_DO_FUN( do_refuel );
DECLARE_DO_FUN( do_addpilot );
DECLARE_DO_FUN( do_rempilot );
DECLARE_DO_FUN( do_trajectory );
DECLARE_DO_FUN( do_accelerate );
DECLARE_DO_FUN( do_launch );
DECLARE_DO_FUN( do_land );
DECLARE_DO_FUN( do_leaveship );
DECLARE_DO_FUN( do_setstarsystem );
DECLARE_DO_FUN( do_makestarsystem );
DECLARE_DO_FUN( do_starsystems );
DECLARE_DO_FUN( do_showstarsystem );
DECLARE_DO_FUN( skill_notfound );
DECLARE_DO_FUN( do_aassign );
DECLARE_DO_FUN( do_addbounty );
DECLARE_DO_FUN( do_vassign );
DECLARE_DO_FUN( do_advance );
DECLARE_DO_FUN( do_affected );
DECLARE_DO_FUN( do_afk );
DECLARE_DO_FUN( do_aid );
DECLARE_DO_FUN( do_allow );
DECLARE_DO_FUN( do_ansi );
DECLARE_DO_FUN( do_answer );
DECLARE_DO_FUN( do_apply );
DECLARE_DO_FUN( do_appraise );
DECLARE_DO_FUN( do_areas );
DECLARE_DO_FUN( do_aset );
DECLARE_DO_FUN( do_ask );
DECLARE_DO_FUN( do_astat );
DECLARE_DO_FUN( do_at );
DECLARE_DO_FUN( do_auction );
DECLARE_DO_FUN( do_authorize );
DECLARE_DO_FUN( do_avtalk );
DECLARE_DO_FUN( do_backstab );
DECLARE_DO_FUN( do_balzhur );
DECLARE_DO_FUN( do_bamfin );
DECLARE_DO_FUN( do_bamfout );
DECLARE_DO_FUN( do_ban );
DECLARE_DO_FUN( do_bash );
DECLARE_DO_FUN( do_bashdoor );
DECLARE_DO_FUN( do_beep );
DECLARE_DO_FUN( do_berserk );
DECLARE_DO_FUN( do_bestow );
DECLARE_DO_FUN( do_bestowarea );
DECLARE_DO_FUN( do_bio );
DECLARE_DO_FUN( do_bite );
DECLARE_DO_FUN( do_board );
DECLARE_DO_FUN( do_boards );
DECLARE_DO_FUN( do_bodybag );
DECLARE_DO_FUN( do_bounties );
DECLARE_DO_FUN( do_brandish );
DECLARE_DO_FUN( do_brew );
DECLARE_DO_FUN( do_bset );
DECLARE_DO_FUN( do_bstat );
DECLARE_DO_FUN( do_bug );
DECLARE_DO_FUN( do_bury );
DECLARE_DO_FUN( do_buy );
DECLARE_DO_FUN( do_cast );
DECLARE_DO_FUN( do_cedit );
DECLARE_DO_FUN( do_channels );
DECLARE_DO_FUN( do_chat );
DECLARE_DO_FUN( do_ooc );
DECLARE_DO_FUN( do_check_vnums );
DECLARE_DO_FUN( do_circle );
DECLARE_DO_FUN( do_clans );
DECLARE_DO_FUN( do_ships );
DECLARE_DO_FUN( do_clantalk );
DECLARE_DO_FUN( do_claw );
DECLARE_DO_FUN( do_climb );
DECLARE_DO_FUN( do_close );
DECLARE_DO_FUN( do_cmdtable );
DECLARE_DO_FUN( do_cmenu );
DECLARE_DO_FUN( do_commands );
DECLARE_DO_FUN( do_comment );
DECLARE_DO_FUN( do_compare );
DECLARE_DO_FUN( do_compress );
DECLARE_DO_FUN( do_config );
DECLARE_DO_FUN( do_consider );
DECLARE_DO_FUN( do_senate );
DECLARE_DO_FUN( do_addsenator );
DECLARE_DO_FUN( do_remsenator );
DECLARE_DO_FUN( do_credits );
DECLARE_DO_FUN( do_cset );
DECLARE_DO_FUN( do_deny );
DECLARE_DO_FUN( do_description );
DECLARE_DO_FUN( do_destro );
DECLARE_DO_FUN( do_destroy );
DECLARE_DO_FUN( do_detrap );
DECLARE_DO_FUN( do_devote );
DECLARE_DO_FUN( do_dig );
DECLARE_DO_FUN( do_disarm );
DECLARE_DO_FUN( do_disconnect );
DECLARE_DO_FUN( do_dismount );
DECLARE_DO_FUN( do_dmesg );
DECLARE_DO_FUN( do_down );
DECLARE_DO_FUN( do_drag );
DECLARE_DO_FUN( do_drink );
DECLARE_DO_FUN( do_drop );
DECLARE_DO_FUN( do_diagnose );
DECLARE_DO_FUN( do_east );
DECLARE_DO_FUN( do_eat );
DECLARE_DO_FUN( do_echo );
DECLARE_DO_FUN( do_emote );
DECLARE_DO_FUN( do_empty );
DECLARE_DO_FUN( do_enter );
DECLARE_DO_FUN( do_equipment );
DECLARE_DO_FUN( do_examine );
DECLARE_DO_FUN( do_exits );
DECLARE_DO_FUN( do_feed );
DECLARE_DO_FUN( do_fill );
DECLARE_DO_FUN( do_fixchar );
DECLARE_DO_FUN( do_flee );
DECLARE_DO_FUN( do_foldarea );
DECLARE_DO_FUN( do_follow );
DECLARE_DO_FUN( do_for );
DECLARE_DO_FUN( do_force );
DECLARE_DO_FUN( do_forceclose );
DECLARE_DO_FUN( do_fquit );   /* Gorog */
DECLARE_DO_FUN( do_form_password );
DECLARE_DO_FUN( do_freeze );
DECLARE_DO_FUN( do_get );
DECLARE_DO_FUN( do_give );
DECLARE_DO_FUN( do_glance );
DECLARE_DO_FUN( do_gold );
DECLARE_DO_FUN( do_goto );
DECLARE_DO_FUN( do_gouge );
DECLARE_DO_FUN( do_group );
DECLARE_DO_FUN( do_grub );
DECLARE_DO_FUN( do_gtell );
DECLARE_DO_FUN( do_guilds );
DECLARE_DO_FUN( do_guildtalk );
DECLARE_DO_FUN( do_hedit );
DECLARE_DO_FUN( do_hell );
DECLARE_DO_FUN( do_help );
DECLARE_DO_FUN( do_health );
DECLARE_DO_FUN( do_hide );
DECLARE_DO_FUN( do_hitall );
DECLARE_DO_FUN( do_hlist );
DECLARE_DO_FUN( do_holylight );
DECLARE_DO_FUN( do_homepage );
DECLARE_DO_FUN( do_hset );
DECLARE_DO_FUN( do_i103 );
DECLARE_DO_FUN( do_i104 );
DECLARE_DO_FUN( do_i105 );
DECLARE_DO_FUN( do_ide );
DECLARE_DO_FUN( do_idea );
DECLARE_DO_FUN( do_immortalize );
DECLARE_DO_FUN( do_immtalk );
DECLARE_DO_FUN( do_induct );
DECLARE_DO_FUN( do_installarea );
DECLARE_DO_FUN( do_instaroom );
DECLARE_DO_FUN( do_instazone );
DECLARE_DO_FUN( do_inventory );
DECLARE_DO_FUN( do_invis );
DECLARE_DO_FUN( do_kick );
DECLARE_DO_FUN( do_kill );
DECLARE_DO_FUN( do_languages );
DECLARE_DO_FUN( do_last );
DECLARE_DO_FUN( do_leave );
DECLARE_DO_FUN( do_level );
DECLARE_DO_FUN( do_light );
DECLARE_DO_FUN( do_list );
DECLARE_DO_FUN( do_litterbug );
DECLARE_DO_FUN( do_loadarea );
DECLARE_DO_FUN( do_loadup );
DECLARE_DO_FUN( do_lock );
DECLARE_DO_FUN( do_log );
DECLARE_DO_FUN( do_look );
DECLARE_DO_FUN( do_low_purge );
DECLARE_DO_FUN( do_mailroom );
DECLARE_DO_FUN( do_make );
DECLARE_DO_FUN( do_makeboard );
DECLARE_DO_FUN( do_makeclan );
DECLARE_DO_FUN( do_makeship );
DECLARE_DO_FUN( do_makeguild );
DECLARE_DO_FUN( do_makerepair );
DECLARE_DO_FUN( do_makeshop );
DECLARE_DO_FUN( do_makewizlist );
DECLARE_DO_FUN( do_memory );
DECLARE_DO_FUN( do_mcreate );
DECLARE_DO_FUN( do_mdelete );
DECLARE_DO_FUN( do_mfind );
DECLARE_DO_FUN( do_minvoke );
DECLARE_DO_FUN( do_mlist );
DECLARE_DO_FUN( do_mount );
DECLARE_DO_FUN( do_mset );
DECLARE_DO_FUN( do_mstat );
DECLARE_DO_FUN( do_murde );
DECLARE_DO_FUN( do_murder );
DECLARE_DO_FUN( do_music );
DECLARE_DO_FUN( do_mwhere );
DECLARE_DO_FUN( do_name );
DECLARE_DO_FUN( do_newbiechat );
DECLARE_DO_FUN( do_newbieset );
DECLARE_DO_FUN( do_newzones );
DECLARE_DO_FUN( do_noemote );
DECLARE_DO_FUN( do_noresolve );
DECLARE_DO_FUN( do_north );
DECLARE_DO_FUN( do_northeast );
DECLARE_DO_FUN( do_northwest );
DECLARE_DO_FUN( do_notell );
DECLARE_DO_FUN( do_notitle );
DECLARE_DO_FUN( do_noteroom );
DECLARE_DO_FUN( do_ocreate );
DECLARE_DO_FUN( do_odelete );
DECLARE_DO_FUN( do_ofind );
DECLARE_DO_FUN( do_ogrub );
DECLARE_DO_FUN( do_oinvoke );
DECLARE_DO_FUN( do_oldscore );
DECLARE_DO_FUN( do_olist );
DECLARE_DO_FUN( do_open );
DECLARE_DO_FUN( do_opentourney );
DECLARE_DO_FUN( do_order );
DECLARE_DO_FUN( do_orders );
DECLARE_DO_FUN( do_ordertalk );
DECLARE_DO_FUN( do_oset );
DECLARE_DO_FUN( do_ostat );
DECLARE_DO_FUN( do_ot );
DECLARE_DO_FUN( do_outcast );
DECLARE_DO_FUN( do_owhere );
DECLARE_DO_FUN( do_pager );
DECLARE_DO_FUN( do_pardon );
DECLARE_DO_FUN( do_password );
DECLARE_DO_FUN( do_peace );
DECLARE_DO_FUN( do_pick );
DECLARE_DO_FUN( do_poison_weapon );
DECLARE_DO_FUN( do_pose );
DECLARE_DO_FUN( do_practice );
DECLARE_DO_FUN( do_prompt );
DECLARE_DO_FUN( do_pull );
DECLARE_DO_FUN( do_punch );
DECLARE_DO_FUN( do_purge );
DECLARE_DO_FUN( do_push );
DECLARE_DO_FUN( do_put );
DECLARE_DO_FUN( do_qpset );
DECLARE_DO_FUN( do_quaff );
DECLARE_DO_FUN( do_quest );
DECLARE_DO_FUN( do_qui );
DECLARE_DO_FUN( do_quit );
DECLARE_DO_FUN( do_rank );
DECLARE_DO_FUN( do_rat );
DECLARE_DO_FUN( do_rdelete );
DECLARE_DO_FUN( do_reboo );
DECLARE_DO_FUN( do_reboot );
DECLARE_DO_FUN( do_recall );
DECLARE_DO_FUN( do_recho );
DECLARE_DO_FUN( do_recite );
DECLARE_DO_FUN( do_redit );
DECLARE_DO_FUN( do_regoto );
DECLARE_DO_FUN( do_remove );
DECLARE_DO_FUN( do_rent );
DECLARE_DO_FUN( do_repair );
DECLARE_DO_FUN( do_repairset );
DECLARE_DO_FUN( do_repairshops );
DECLARE_DO_FUN( do_repairstat );
DECLARE_DO_FUN( do_reply );
DECLARE_DO_FUN( do_report );
DECLARE_DO_FUN( do_rescue );
DECLARE_DO_FUN( do_rest );
DECLARE_DO_FUN( do_reset );
DECLARE_DO_FUN( do_resetship );
DECLARE_DO_FUN( do_restore );
DECLARE_DO_FUN( do_restoretime );
DECLARE_DO_FUN( do_restrict );
DECLARE_DO_FUN( do_retire );
DECLARE_DO_FUN( do_retran );
DECLARE_DO_FUN( do_return );
DECLARE_DO_FUN( do_revert );
DECLARE_DO_FUN( do_rip );
DECLARE_DO_FUN( do_rlist );
DECLARE_DO_FUN( do_rset );
DECLARE_DO_FUN( do_rstat );
DECLARE_DO_FUN( do_sacrifice );
DECLARE_DO_FUN( do_save );
DECLARE_DO_FUN( do_savearea );
DECLARE_DO_FUN( do_say );
DECLARE_DO_FUN( do_scan );
DECLARE_DO_FUN( do_score );
DECLARE_DO_FUN( do_scribe );
DECLARE_DO_FUN( do_search );
DECLARE_DO_FUN( do_sedit );
DECLARE_DO_FUN( do_sell );
DECLARE_DO_FUN( do_set_boot_time );
DECLARE_DO_FUN( do_setclan );
DECLARE_DO_FUN( do_setship );
DECLARE_DO_FUN( do_shops );
DECLARE_DO_FUN( do_shopset );
DECLARE_DO_FUN( do_shopstat );
DECLARE_DO_FUN( do_shout );
DECLARE_DO_FUN( do_shove );
DECLARE_DO_FUN( do_showclan );
DECLARE_DO_FUN( do_showship );
DECLARE_DO_FUN( do_shutdow );
DECLARE_DO_FUN( do_shutdown );
DECLARE_DO_FUN( do_silence );
DECLARE_DO_FUN( do_sit );
DECLARE_DO_FUN( do_sla );
DECLARE_DO_FUN( do_slay );
DECLARE_DO_FUN( do_sleep );
DECLARE_DO_FUN( do_slice );
DECLARE_DO_FUN( do_slist );
DECLARE_DO_FUN( do_slookup );
DECLARE_DO_FUN( do_smoke );
DECLARE_DO_FUN( do_sneak );
DECLARE_DO_FUN( do_snoop );
DECLARE_DO_FUN( do_sober );
DECLARE_DO_FUN( do_socials );
DECLARE_DO_FUN( do_south );
DECLARE_DO_FUN( do_southeast );
DECLARE_DO_FUN( do_southwest );
DECLARE_DO_FUN( do_speak );
DECLARE_DO_FUN( do_split );
DECLARE_DO_FUN( do_sset );
DECLARE_DO_FUN( do_stand );
DECLARE_DO_FUN( do_starttourney );
DECLARE_DO_FUN( do_steal );
DECLARE_DO_FUN( do_sting );
DECLARE_DO_FUN( do_stun );
DECLARE_DO_FUN( do_switch );
DECLARE_DO_FUN( do_tail );
DECLARE_DO_FUN( do_tamp );
DECLARE_DO_FUN( do_tell );
DECLARE_DO_FUN( do_time );
DECLARE_DO_FUN( do_timecmd );
DECLARE_DO_FUN( do_title );
DECLARE_DO_FUN( do_track );
DECLARE_DO_FUN( do_transfer );
DECLARE_DO_FUN( do_trust );
DECLARE_DO_FUN( do_typo );
DECLARE_DO_FUN( do_unfoldarea );
DECLARE_DO_FUN( do_unhell );
DECLARE_DO_FUN( do_unlock );
DECLARE_DO_FUN( do_unsilence );
DECLARE_DO_FUN( do_up );
DECLARE_DO_FUN( do_users );
DECLARE_DO_FUN( do_value );
DECLARE_DO_FUN( do_visible );
DECLARE_DO_FUN( do_vnums );
DECLARE_DO_FUN( do_vsearch );
DECLARE_DO_FUN( do_wake );
DECLARE_DO_FUN( do_wartalk );
DECLARE_DO_FUN( do_wear );
DECLARE_DO_FUN( do_weather );
DECLARE_DO_FUN( do_west );
DECLARE_DO_FUN( do_where );
DECLARE_DO_FUN( do_who );
DECLARE_DO_FUN( do_whois );
DECLARE_DO_FUN( do_wimpy );
DECLARE_DO_FUN( do_wizhelp );
DECLARE_DO_FUN( do_wizlist );
DECLARE_DO_FUN( do_wizlock );
DECLARE_DO_FUN( do_yell );
DECLARE_DO_FUN( do_zap );
DECLARE_DO_FUN( do_zones );

/* mob prog stuff */
DECLARE_DO_FUN( do_mp_close_passage );
DECLARE_DO_FUN( do_mp_damage );
DECLARE_DO_FUN( do_mp_restore );
DECLARE_DO_FUN( do_mp_open_passage );
DECLARE_DO_FUN( do_mp_practice );
DECLARE_DO_FUN( do_mp_slay );
DECLARE_DO_FUN( do_mpadvance );
DECLARE_DO_FUN( do_mpasound );
DECLARE_DO_FUN( do_mpat );
DECLARE_DO_FUN( do_mpdream );
DECLARE_DO_FUN( do_mp_deposit );
DECLARE_DO_FUN( do_mp_withdraw );
DECLARE_DO_FUN( do_mpecho );
DECLARE_DO_FUN( do_mpechoaround );
DECLARE_DO_FUN( do_mpechoat );
DECLARE_DO_FUN( do_mpedit );
DECLARE_DO_FUN( do_mrange );
DECLARE_DO_FUN( do_opedit );
DECLARE_DO_FUN( do_orange );
DECLARE_DO_FUN( do_rpedit );
DECLARE_DO_FUN( do_mpforce );
DECLARE_DO_FUN( do_mpinvis );
DECLARE_DO_FUN( do_mpgoto );
DECLARE_DO_FUN( do_mpjunk );
DECLARE_DO_FUN( do_mpkill );
DECLARE_DO_FUN( do_mpmload );
DECLARE_DO_FUN( do_mpnothing );
DECLARE_DO_FUN( do_mpoload );
DECLARE_DO_FUN( do_mppurge );
DECLARE_DO_FUN( do_mpstat );
DECLARE_DO_FUN( do_opstat );
DECLARE_DO_FUN( do_rpstat );
DECLARE_DO_FUN( do_mptransfer );
DECLARE_DO_FUN( do_mpapply );
DECLARE_DO_FUN( do_mpapplyb );
DECLARE_DO_FUN( do_mppkset );
DECLARE_DO_FUN( do_mpgain );

/*
* Spell functions.
* Defined in magic.c.
*/
DECLARE_SPELL_FUN( spell_null );
DECLARE_SPELL_FUN( spell_notfound );
DECLARE_SPELL_FUN( spell_acid_blast );
DECLARE_SPELL_FUN( spell_animate_dead );
DECLARE_SPELL_FUN( spell_astral_walk );
DECLARE_SPELL_FUN( spell_blindness );
DECLARE_SPELL_FUN( spell_burning_hands );
DECLARE_SPELL_FUN( spell_call_lightning );
DECLARE_SPELL_FUN( spell_cause_critical );
DECLARE_SPELL_FUN( spell_cause_light );
DECLARE_SPELL_FUN( spell_cause_serious );
DECLARE_SPELL_FUN( spell_change_sex );
DECLARE_SPELL_FUN( spell_charm_person );
DECLARE_SPELL_FUN( spell_chill_touch );
DECLARE_SPELL_FUN( spell_colour_spray );
DECLARE_SPELL_FUN( spell_control_weather );
DECLARE_SPELL_FUN( spell_create_food );
DECLARE_SPELL_FUN( spell_create_water );
DECLARE_SPELL_FUN( spell_cure_blindness );
DECLARE_SPELL_FUN( spell_cure_poison );
DECLARE_SPELL_FUN( spell_curse );
DECLARE_SPELL_FUN( spell_detect_poison );
DECLARE_SPELL_FUN( spell_dispel_evil );
DECLARE_SPELL_FUN( spell_dispel_magic );
DECLARE_SPELL_FUN( spell_dream );
DECLARE_SPELL_FUN( spell_earthquake );
DECLARE_SPELL_FUN( spell_enchant_weapon );
DECLARE_SPELL_FUN( spell_energy_drain );
DECLARE_SPELL_FUN( spell_faerie_fire );
DECLARE_SPELL_FUN( spell_faerie_fog );
DECLARE_SPELL_FUN( spell_farsight );
DECLARE_SPELL_FUN( spell_fireball );
DECLARE_SPELL_FUN( spell_flamestrike );
DECLARE_SPELL_FUN( spell_gate );
DECLARE_SPELL_FUN( spell_knock );
DECLARE_SPELL_FUN( spell_harm );
DECLARE_SPELL_FUN( spell_identify );
DECLARE_SPELL_FUN( spell_invis );
DECLARE_SPELL_FUN( spell_know_alignment );
DECLARE_SPELL_FUN( spell_lightning_bolt );
DECLARE_SPELL_FUN( spell_locate_object );
DECLARE_SPELL_FUN( spell_magic_missile );
DECLARE_SPELL_FUN( spell_mist_walk );
DECLARE_SPELL_FUN( spell_pass_door );
DECLARE_SPELL_FUN( spell_plant_pass );
DECLARE_SPELL_FUN( spell_poison );
DECLARE_SPELL_FUN( spell_polymorph );
DECLARE_SPELL_FUN( spell_possess );
DECLARE_SPELL_FUN( spell_recharge );
DECLARE_SPELL_FUN( spell_remove_curse );
DECLARE_SPELL_FUN( spell_remove_invis );
DECLARE_SPELL_FUN( spell_remove_trap );
DECLARE_SPELL_FUN( spell_shocking_grasp );
DECLARE_SPELL_FUN( spell_sleep );
DECLARE_SPELL_FUN( spell_smaug );
DECLARE_SPELL_FUN( spell_solar_flight );
DECLARE_SPELL_FUN( spell_summon );
DECLARE_SPELL_FUN( spell_teleport );
DECLARE_SPELL_FUN( spell_ventriloquate );
DECLARE_SPELL_FUN( spell_weaken );
DECLARE_SPELL_FUN( spell_word_of_recall );
DECLARE_SPELL_FUN( spell_acid_breath );
DECLARE_SPELL_FUN( spell_fire_breath );
DECLARE_SPELL_FUN( spell_frost_breath );
DECLARE_SPELL_FUN( spell_gas_breath );
DECLARE_SPELL_FUN( spell_lightning_breath );
DECLARE_SPELL_FUN( spell_spiral_blast );
DECLARE_SPELL_FUN( spell_scorching_surge );
DECLARE_SPELL_FUN( spell_helical_flow );
DECLARE_SPELL_FUN( spell_transport );
DECLARE_SPELL_FUN( spell_portal );

DECLARE_SPELL_FUN( spell_ethereal_fist );
DECLARE_SPELL_FUN( spell_spectral_furor );
DECLARE_SPELL_FUN( spell_hand_of_chaos );
DECLARE_SPELL_FUN( spell_disruption );
DECLARE_SPELL_FUN( spell_sonic_resonance );
DECLARE_SPELL_FUN( spell_mind_wrack );
DECLARE_SPELL_FUN( spell_mind_wrench );
DECLARE_SPELL_FUN( spell_revive );
DECLARE_SPELL_FUN( spell_sulfurous_spray );
DECLARE_SPELL_FUN( spell_caustic_fount );
DECLARE_SPELL_FUN( spell_acetum_primus );
DECLARE_SPELL_FUN( spell_galvanic_whip );
DECLARE_SPELL_FUN( spell_magnetic_thrust );
DECLARE_SPELL_FUN( spell_quantum_spike );
DECLARE_SPELL_FUN( spell_black_hand );
DECLARE_SPELL_FUN( spell_black_fist );
DECLARE_SPELL_FUN( spell_black_lightning );
DECLARE_SPELL_FUN( spell_midas_touch );

DECLARE_SPELL_FUN( spell_suggest );
DECLARE_SPELL_FUN( spell_cure_addiction );

/*
* Data files used by the server.
*
* AREA_LIST contains a list of areas to boot.
* All files are read in completely at bootup.
* Most output files (bug, idea, typo, shutdown) are append-only.
*
* The NULL_FILE is held open so that we have a stream handle in reserve,
*   so players can go ahead and telnet to all the other descriptors.
* Then we close it whenever we need to open a file (e.g. a save file).
*/
#define PLAYER_DIR	"../player/"   /* Player files         */
#define BACKUP_DIR	"../backup/"   /* Backup Player files    */
#define GOD_DIR		"../gods/"  /* God Info Dir         */
#define BOARD_DIR	"../boards/"   /* Board data dir    */
#define CLAN_DIR	"../clans/" /* Clan data dir     */
#define SHIP_DIR        "../space/"
#define SPACE_DIR       "../space/"
#define PLANET_DIR      "../planets/"
#define GUARD_DIR       "../planets/"
#define GUILD_DIR       "../guilds/"   /* Guild data dir               */
#define BUILD_DIR       "../building/" /* Online building save dir     */
#define SYSTEM_DIR	"../system/"   /* Main system files    */
#define PROG_DIR	"../mudprogs/" /* MUDProg files     */
#define CORPSE_DIR	"../corpses/"  /* Corpses        */
#define AREA_LIST	"area.lst"  /* List of areas     */
#define BAN_LIST        "ban.lst"   /* List of bans                 */
#define CLAN_LIST	"clan.lst"  /* List of clans     */
#define SHIP_LIST       "ship.lst"
#define PLANET_LIST      "planet.lst"
#define SPACE_LIST      "space.lst"
#define BOUNTY_LIST     "bounty.lst"
#define DISINTIGRATION_LIST	"disintigration.lst"
#define SENATE_LIST	"senate.lst"   /* List of senators     */
#define GUILD_LIST      "guild.lst" /* List of guilds               */
#define GUARD_LIST	"guard.lst"
#define BOARD_FILE	"boards.txt"   /* For bulletin boards   */
#define SHUTDOWN_FILE	"shutdown.txt" /* For 'shutdown'  */
#define RIPSCREEN_FILE	SYSTEM_DIR "mudrip.rip"
#define RIPTITLE_FILE	SYSTEM_DIR "mudtitle.rip"
#define ANSITITLE_FILE	SYSTEM_DIR "mudtitle.ans"
#define ASCTITLE_FILE	SYSTEM_DIR "mudtitle.asc"
#define BOOTLOG_FILE	SYSTEM_DIR "boot.txt"   /* Boot up error file  */
#define BUG_FILE		SYSTEM_DIR "pbugs.txt"  /* For player usable 'bug' command */
#define IDEA_FILE	SYSTEM_DIR "ideas.txt"  /* For 'idea'       */
#define TYPO_FILE	SYSTEM_DIR "typos.txt"  /* For 'typo'       */
#define LOG_FILE	SYSTEM_DIR "log.txt" /* For talking in logged rooms */
#define WIZLIST_FILE	SYSTEM_DIR "WIZLIST" /* Wizlist       */
#define WHO_FILE	SYSTEM_DIR "WHO"  /* Who output file  */
#define WEBWHO_FILE	SYSTEM_DIR "WEBWHO"  /* WWW Who output file */
#define SKILL_FILE	SYSTEM_DIR "skills.dat" /* Skill table   */
#define HERB_FILE	SYSTEM_DIR "herbs.dat"  /* Herb table       */
#define SOCIAL_FILE	SYSTEM_DIR "socials.dat"   /* Socials       */
#define COMMAND_FILE	SYSTEM_DIR "commands.dat"  /* Commands      */
#define USAGE_FILE	SYSTEM_DIR "usage.txt"  /* How many people are on
* every half hour - trying to
* determine best reboot time */

/*
* Our function prototypes.
* One big lump ... this is every function in Merc.
*/
#define CD	CHAR_DATA
#define MID	MOB_INDEX_DATA
#define OD	OBJ_DATA
#define OID	OBJ_INDEX_DATA
#define RID	ROOM_INDEX_DATA
#define SF	SPEC_FUN
#define BD	BOARD_DATA
#define CL	CLAN_DATA
#define EDD	EXTRA_DESCR_DATA
#define RD	RESET_DATA
#define ED	EXIT_DATA
#define	ST	SOCIALTYPE
#define SK	SKILLTYPE
#define SH      SHIP_DATA

/* act_comm.c */
void sound_to_room( ROOM_INDEX_DATA * room, const char *argument );
bool circle_follow args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void add_follower args( ( CHAR_DATA * ch, CHAR_DATA * master ) );
void stop_follower args( ( CHAR_DATA * ch ) );
void die_follower args( ( CHAR_DATA * ch ) );
bool is_same_group args( ( CHAR_DATA * ach, CHAR_DATA * bch ) );
void send_rip_screen args( ( CHAR_DATA * ch ) );
void send_rip_title args( ( CHAR_DATA * ch ) );
void send_ansi_title args( ( CHAR_DATA * ch ) );
void send_ascii_title args( ( CHAR_DATA * ch ) );
void to_channel args( ( const char *argument, int channel, const char *verb, short level ) );
void talk_auction args( ( char *argument ) );
bool knows_language args( ( CHAR_DATA * ch, int language, CHAR_DATA * cch ) );
bool can_learn_lang args( ( CHAR_DATA * ch, int language ) );
int countlangs args( ( int languages ) );
const char *translate args( ( CHAR_DATA * ch, CHAR_DATA * victim, const char *argument ) );
const char *obj_short( OBJ_DATA * obj );

/* act_info.c */
int get_door( const char *arg );
char *format_obj_to_char args( ( OBJ_DATA * obj, CHAR_DATA * ch, bool fShort ) );
void show_list_to_char args( ( OBJ_DATA * list, CHAR_DATA * ch, bool fShort, bool fShowNothing ) );

/* act_move.c */
ROOM_INDEX_DATA *generate_exit( ROOM_INDEX_DATA * in_room, EXIT_DATA ** pexit );
void clear_vrooms( void );
ED *find_door( CHAR_DATA * ch, const char *arg, bool quiet );
ED *get_exit args( ( ROOM_INDEX_DATA * room, short dir ) );
ED *get_exit_to args( ( ROOM_INDEX_DATA * room, short dir, int vnum ) );
ED *get_exit_num args( ( ROOM_INDEX_DATA * room, short count ) );
ch_ret move_char args( ( CHAR_DATA * ch, EXIT_DATA * pexit, int fall ) );
void teleport( CHAR_DATA * ch, int room, int flags );
short encumbrance args( ( CHAR_DATA * ch, short move ) );
bool will_fall args( ( CHAR_DATA * ch, int fall ) );
int wherehome args( ( CHAR_DATA * ch ) );

/* act_obj.c */

obj_ret damage_obj args( ( OBJ_DATA * obj ) );
short get_obj_resistance args( ( OBJ_DATA * obj ) );
void save_clan_storeroom args( ( CHAR_DATA * ch, CLAN_DATA * clan ) );
void obj_fall args( ( OBJ_DATA * obj, bool through ) );

/* act_wiz.c */
void close_area args( ( AREA_DATA * pArea ) );
RID *find_location( CHAR_DATA * ch, const char *arg );
void echo_to_room( short AT_COLOR, ROOM_INDEX_DATA * room, const char *argument );
void echo_to_all( short AT_COLOR, const char *argument, short tar );
void get_reboot_string args( ( void ) );
struct tm *update_time args( ( struct tm * old_time ) );
void free_social args( ( SOCIALTYPE * social ) );
void add_social args( ( SOCIALTYPE * social ) );
void free_command args( ( CMDTYPE * command ) );
void unlink_command args( ( CMDTYPE * command ) );
void add_command args( ( CMDTYPE * command ) );

/* boards.c */
void load_boards args( ( void ) );
BD *get_board args( ( OBJ_DATA * obj ) );
void free_note args( ( NOTE_DATA * pnote ) );

/* build.c */
void RelCreate( relation_type, void *, void * );
void RelDestroy( relation_type, void *, void * );
const char *flag_string( int bitvector, const char *const flagarray[] );
int get_dir( const char *txt );
char *strip_cr( const char *str );
#define VCHECK_ROOM 0
#define VCHECK_OBJ 1
#define VCHECK_MOB 2
bool is_valid_vnum( int vnum, short type );
void start_editing args( ( CHAR_DATA * ch, const char *data ) );
void stop_editing args( ( CHAR_DATA * ch ) );
void edit_buffer args( ( CHAR_DATA * ch, char *argument ) );
const char *copy_buffer( CHAR_DATA * ch );
bool can_rmodify args( ( CHAR_DATA * ch, ROOM_INDEX_DATA * room ) );
bool can_omodify args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool can_mmodify args( ( CHAR_DATA * ch, CHAR_DATA * mob ) );
bool can_medit args( ( CHAR_DATA * ch, MOB_INDEX_DATA * mob ) );
void free_reset args( ( AREA_DATA * are, RESET_DATA * res ) );
void free_area args( ( AREA_DATA * are ) );
void assign_area args( ( CHAR_DATA * ch ) );
EDD *SetRExtra( ROOM_INDEX_DATA * room, const char *keywords );
bool DelRExtra( ROOM_INDEX_DATA * room, const char *keywords );
EDD *SetOExtra( OBJ_DATA * obj, const char *keywords );
bool DelOExtra( OBJ_DATA * obj, const char *keywords );
EDD *SetOExtraProto( OBJ_INDEX_DATA * obj, const char *keywords );
bool DelOExtraProto( OBJ_INDEX_DATA * obj, const char *keywords );
void fold_area( AREA_DATA * tarea, const char *filename, bool install );
int get_otype( const char *type );
int get_aflag( const char *flag );
int get_trapflag( const char *flag );
int get_atype( const char *type );
int get_npc_race( const char *type );
int get_wearloc( const char *type );
int get_exflag( const char *flag );
int get_rflag( const char *flag );
int get_mpflag( const char *flag );
int get_oflag( const char *flag );
int get_areaflag( const char *flag );
int get_wflag( const char *flag );
int get_actflag( const char *flag );
int get_vip_flag( const char *flag );
int get_wanted_flag( const char *flag );
int get_pcflag( const char *flag );
int get_plrflag( const char *flag );
int get_risflag( const char *flag );
int get_trigflag( const char *flag );
int get_partflag( const char *flag );
int get_attackflag( const char *flag );
int get_defenseflag( const char *flag );
int get_langflag( const char *flag );
int get_langnum( const char *flag );
int get_langnum_save( const char *flag );
int get_secflag( const char *flag );
int get_npc_position( const char *position );
int get_npc_sex( const char *sex );

/* clans.c */
CL *get_clan( const char *name );
void load_clans args( ( void ) );
void save_clan args( ( CLAN_DATA * clan ) );
void load_senate args( ( void ) );
void save_senate args( ( void ) );
PLANET_DATA *get_planet( const char *name );
void load_planets args( ( void ) );
void save_planet args( ( PLANET_DATA * planet ) );
long get_taxes args( ( PLANET_DATA * planet ) );

/* bounty.c */
BOUNTY_DATA *get_disintigration( const char *target );
void load_bounties args( ( void ) );
void save_bounties args( ( void ) );
void save_disintigrations args( ( void ) );
void remove_disintigration args( ( BOUNTY_DATA * bounty ) );
void claim_disintigration args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_disintigration args( ( CHAR_DATA * victim ) );

/* space.c */
SH *get_ship( const char *name );
void load_ships args( ( void ) );
void save_ship args( ( SHIP_DATA * ship ) );
void load_space args( ( void ) );
void save_starsystem args( ( SPACE_DATA * starsystem ) );
SPACE_DATA *starsystem_from_name( const char *name );
SPACE_DATA *starsystem_from_vnum args( ( int vnum ) );
SHIP_DATA *ship_from_obj args( ( int vnum ) );
SHIP_DATA *ship_from_entrance args( ( int vnum ) );
SHIP_DATA *ship_from_hanger args( ( int vnum ) );
SHIP_DATA *ship_from_cockpit args( ( int vnum ) );
SHIP_DATA *ship_from_navseat args( ( int vnum ) );
SHIP_DATA *ship_from_coseat args( ( int vnum ) );
SHIP_DATA *ship_from_pilotseat args( ( int vnum ) );
SHIP_DATA *ship_from_gunseat args( ( int vnum ) );
SHIP_DATA *ship_from_turret args( ( int vnum ) );
SHIP_DATA *ship_from_engine args( ( int vnum ) );
SHIP_DATA *ship_from_pilot( const char *name );
SHIP_DATA *get_ship_here( const char *name, SPACE_DATA * starsystem );
void showstarsystem args( ( CHAR_DATA * ch, SPACE_DATA * starsystem ) );
void update_space args( ( void ) );
void recharge_ships args( ( void ) );
void move_ships args( ( void ) );
void update_bus args( ( void ) );
void update_traffic args( ( void ) );
bool check_pilot args( ( CHAR_DATA * ch, SHIP_DATA * ship ) );
bool is_rental args( ( CHAR_DATA * ch, SHIP_DATA * ship ) );
void echo_to_ship( int color, SHIP_DATA * ship, const char *argument );
void echo_to_cockpit( int color, SHIP_DATA * ship, const char *argument );
void echo_to_system( int color, SHIP_DATA * ship,
                    const char *argument, SHIP_DATA * ignore );
bool extract_ship args( ( SHIP_DATA * ship ) );
bool ship_to_room args( ( SHIP_DATA * ship, int vnum ) );
long get_ship_value args( ( SHIP_DATA * ship ) );
bool rent_ship args( ( CHAR_DATA * ch, SHIP_DATA * ship ) );
void damage_ship args( ( SHIP_DATA * ship, int min, int max ) );
void damage_ship_ch args( ( SHIP_DATA * ship, int min, int max, CHAR_DATA * ch ) );
void destroy_ship args( ( SHIP_DATA * ship, CHAR_DATA * ch ) );
void ship_to_starsystem args( ( SHIP_DATA * ship, SPACE_DATA * starsystem ) );
void ship_from_starsystem args( ( SHIP_DATA * ship, SPACE_DATA * starsystem ) );
void new_missile args( ( SHIP_DATA * ship, SHIP_DATA * target, CHAR_DATA * ch, int missiletype ) );
void extract_missile args( ( MISSILE_DATA * missile ) );
SHIP_DATA *ship_in_room( ROOM_INDEX_DATA * room, const char *name );

/* comm.c */
void close_socket( DESCRIPTOR_DATA * dclose, bool force );
bool write_to_descriptor( DESCRIPTOR_DATA * d, const char *txt, int length );
void write_to_buffer( DESCRIPTOR_DATA * d, const char *txt, size_t length );
void write_to_pager( DESCRIPTOR_DATA * d, const char *txt, size_t length );
void send_to_char( const char *txt, CHAR_DATA * ch );
void send_to_char_color( const char *txt, CHAR_DATA * ch );
void send_to_desc_color( const char *txt, DESCRIPTOR_DATA *d );
void send_to_pager( const char *txt, CHAR_DATA * ch );
void send_to_pager_color( const char *txt, CHAR_DATA * ch );
void ch_printf( CHAR_DATA * ch, const char *fmt, ... ) __attribute__ ( ( format( printf, 2, 3 ) ) );
void pager_printf( CHAR_DATA * ch, const char *fmt, ... ) __attribute__ ( ( format( printf, 2, 3 ) ) );
void descriptor_printf( DESCRIPTOR_DATA * d, const char *fmt, ... ) __attribute__ ( ( format( printf, 2, 3 ) ) );
void buffer_printf( DESCRIPTOR_DATA * d, const char *fmt, ... ) __attribute__ ( ( format( printf, 2, 3 ) ) );
void act( short AType, const char *format, CHAR_DATA * ch, const void *arg1, const void *arg2, int type );

/* reset.c */
RD *make_reset args( ( char letter, int extra, int arg1, int arg2, int arg3 ) );
RD *add_reset( ROOM_INDEX_DATA *room, char letter, int extra, int arg1, int arg2, int arg3 );
void reset_area args( ( AREA_DATA * pArea ) );

/* db.c */
char *fread_flagstring( FILE * fp );
void show_file( CHAR_DATA * ch, const char *filename );
bool is_valid_filename( CHAR_DATA *ch, const char *direct, const char *filename );
void boot_db args( ( bool fCopyOver ) );
void area_update args( ( void ) );
void add_char args( ( CHAR_DATA * ch ) );
CD *create_mobile args( ( MOB_INDEX_DATA * pMobIndex ) );
OD *create_object args( ( OBJ_INDEX_DATA * pObjIndex, int level ) );
void clear_char args( ( CHAR_DATA * ch ) );
void free_char args( ( CHAR_DATA * ch ) );
const char *get_extra_descr( const char *name, EXTRA_DESCR_DATA * ed );
MID *get_mob_index args( ( int vnum ) );
OID *get_obj_index args( ( int vnum ) );
RID *get_room_index args( ( int vnum ) );
char fread_letter args( ( FILE * fp ) );
int fread_number args( ( FILE * fp ) );
float fread_float( FILE * fp );
const char *fread_string( FILE * fp );
char *fread_string_nohash( FILE * fp );
void fread_to_eol args( ( FILE * fp ) );
char *fread_word args( ( FILE * fp ) );
char *fread_line args( ( FILE * fp ) );
int number_fuzzy args( ( int number ) );
int number_range args( ( int from, int to ) );
int number_percent args( ( void ) );
int number_door args( ( void ) );
int number_bits args( ( int width ) );
int number_mm args( ( void ) );
int dice args( ( int number, int size ) );
int interpolate args( ( int level, int value_00, int value_32 ) );
void smash_tilde( char *str );
const char *smash_tilde_static( const char *str );
char *smash_tilde_copy( const char *str );
void hide_tilde args( ( char *str ) );
const char *show_tilde( const char *str );
bool str_cmp( const char *astr, const char *bstr );
bool str_prefix( const char *astr, const char *bstr );
bool str_infix( const char *astr, const char *bstr );
bool str_suffix( const char *astr, const char *bstr );
char *capitalize( const char *str );
char *strlower args( ( const char *str ) );
char *strupper args( ( const char *str ) );
const char *aoran args( ( const char *str ) );
void append_file( CHAR_DATA * ch, const char *file, const char *str );
void append_to_file( const char *file, const char *str );
void bug( const char *str, ... );
void log_string_plus args( ( const char *str, short log_type, short level ) );
void log_printf_plus( short log_type, short level, const char *fmt, ... ) __attribute__ ( ( format( printf, 3, 4 ) ) );
void log_printf( const char *fmt, ... ) __attribute__ ( ( format( printf, 1, 2 ) ) );
RID *make_room( int vnum, AREA_DATA *area );
OID *make_object( int vnum, int cvnum, const char *name );
MID *make_mobile( int vnum, int cvnum, const char *name );
ED *make_exit args( ( ROOM_INDEX_DATA * pRoomIndex, ROOM_INDEX_DATA * to_room, short door ) );
void add_help args( ( HELP_DATA * pHelp ) );
void fix_area_exits args( ( AREA_DATA * tarea ) );
void load_area_file args( ( AREA_DATA * tarea, const char *filename ) );
void randomize_exits args( ( ROOM_INDEX_DATA * room, short maxdir ) );
void make_wizlist args( ( void ) );
void tail_chain args( ( void ) );
void delete_room args( ( ROOM_INDEX_DATA * room ) );
void delete_obj args( ( OBJ_INDEX_DATA * obj ) );
void delete_mob args( ( MOB_INDEX_DATA * mob ) );
void sort_area args( ( AREA_DATA * pArea, bool proto ) );
void sort_area_by_name args( ( AREA_DATA * pArea ) ); /* Fireblade */

/* fight.c */
int max_fight args( ( CHAR_DATA * ch ) );
void violence_update args( ( void ) );
ch_ret multi_hit args( ( CHAR_DATA * ch, CHAR_DATA * victim, int dt ) );
short ris_damage args( ( CHAR_DATA * ch, short dam, int ris ) );
ch_ret damage args( ( CHAR_DATA * ch, CHAR_DATA * victim, int dam, int dt ) );
void update_pos args( ( CHAR_DATA * victim ) );
void set_fighting args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void stop_fighting args( ( CHAR_DATA * ch, bool fBoth ) );
void free_fight args( ( CHAR_DATA * ch ) );
CD *who_fighting args( ( CHAR_DATA * ch ) );
void check_killer args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void check_attacker args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void death_cry args( ( CHAR_DATA * ch ) );
void stop_hunting args( ( CHAR_DATA * ch ) );
void stop_hating args( ( CHAR_DATA * ch ) );
void stop_fearing args( ( CHAR_DATA * ch ) );
void start_hunting args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void start_hating args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void start_fearing args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_hunting args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_hating args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_fearing args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_safe args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool is_safe_nm args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool legal_loot args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool check_illegal_pk args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
OBJ_DATA *raw_kill( CHAR_DATA *ch, CHAR_DATA *victim );
bool in_arena args( ( CHAR_DATA * ch ) );

/* makeobjs.c */
OBJ_DATA *make_corpse( CHAR_DATA * ch, CHAR_DATA * killer );
void make_blood args( ( CHAR_DATA * ch ) );
void make_bloodstain args( ( CHAR_DATA * ch ) );
void make_scraps args( ( OBJ_DATA * obj ) );
void make_fire args( ( ROOM_INDEX_DATA * in_room, short timer ) );
OD *make_trap args( ( int v0, int v1, int v2, int v3 ) );
OD *create_money args( ( int amount ) );

/* misc.c */
void actiondesc args( ( CHAR_DATA * ch, OBJ_DATA * obj, void *vo ) );
void jedi_checks args( ( CHAR_DATA * ch ) );
void jedi_bonus args( ( CHAR_DATA * ch ) );
void sith_penalty args( ( CHAR_DATA * ch ) );

/* mud_comm.c */
const char *mprog_type_to_name( int type );

/* mud_prog.c */
void mprog_wordlist_check args( ( const char *arg, CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * object, void *vo, int type ) );
void mprog_percent_check args( ( CHAR_DATA * mob, CHAR_DATA * actor, OBJ_DATA * object, void *vo, int type ) );
void mprog_act_trigger( const char *buf, CHAR_DATA * mob, CHAR_DATA * ch,
                       OBJ_DATA * obj, void *vo );
void mprog_bribe_trigger args( ( CHAR_DATA * mob, CHAR_DATA * ch, int amount ) );
void mprog_entry_trigger args( ( CHAR_DATA * mob ) );
void mprog_give_trigger args( ( CHAR_DATA * mob, CHAR_DATA * ch, OBJ_DATA * obj ) );
void mprog_greet_trigger args( ( CHAR_DATA * mob ) );
void mprog_fight_trigger args( ( CHAR_DATA * mob, CHAR_DATA * ch ) );
void mprog_hitprcnt_trigger args( ( CHAR_DATA * mob, CHAR_DATA * ch ) );
void mprog_death_trigger args( ( CHAR_DATA * killer, CHAR_DATA * mob ) );
void mprog_random_trigger args( ( CHAR_DATA * mob ) );
void mprog_speech_trigger args( ( const char *txt, CHAR_DATA * mob ) );
void mprog_script_trigger args( ( CHAR_DATA * mob ) );
void mprog_hour_trigger args( ( CHAR_DATA * mob ) );
void mprog_time_trigger args( ( CHAR_DATA * mob ) );
void progbug args( ( const char *str, CHAR_DATA * mob ) );
void rset_supermob args( ( ROOM_INDEX_DATA * room ) );
void release_supermob( void );

/* player.c */
void set_title( CHAR_DATA * ch, const char *title );

/* skills.c */
bool check_skill( CHAR_DATA * ch, const char *command, const char *argument );
void learn_from_success( CHAR_DATA * ch, int sn );
void learn_from_failure( CHAR_DATA * ch, int sn );
bool check_parry( CHAR_DATA * ch, CHAR_DATA * victim );
bool check_dodge( CHAR_DATA * ch, CHAR_DATA * victim );
bool check_grip( CHAR_DATA * ch, CHAR_DATA * victim );
void disarm( CHAR_DATA * ch, CHAR_DATA * victim );
void trip( CHAR_DATA * ch, CHAR_DATA * victim );

/* handler.c */
void free_obj( OBJ_DATA * obj );
void explode args( ( OBJ_DATA * obj ) );
int get_exp args( ( CHAR_DATA * ch, int ability ) );
int get_exp_worth args( ( CHAR_DATA * ch ) );
int exp_level args( ( short level ) );
short get_trust args( ( CHAR_DATA * ch ) );
short get_age args( ( CHAR_DATA * ch ) );
short get_curr_str args( ( CHAR_DATA * ch ) );
short get_curr_int args( ( CHAR_DATA * ch ) );
short get_curr_wis args( ( CHAR_DATA * ch ) );
short get_curr_dex args( ( CHAR_DATA * ch ) );
short get_curr_con args( ( CHAR_DATA * ch ) );
short get_curr_cha args( ( CHAR_DATA * ch ) );
short get_curr_lck args( ( CHAR_DATA * ch ) );
short get_curr_frc args( ( CHAR_DATA * ch ) );
bool can_take_proto args( ( CHAR_DATA * ch ) );
int can_carry_n args( ( CHAR_DATA * ch ) );
int can_carry_w args( ( CHAR_DATA * ch ) );
bool is_name( const char *str, const char *namelist );
bool is_name_prefix( const char *str, const char *namelist );
bool nifty_is_name( const char *str, const char *namelist );
bool nifty_is_name_prefix( const char *str, const char *namelist );
void affect_modify args( ( CHAR_DATA * ch, AFFECT_DATA * paf, bool fAdd ) );
void affect_to_char args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void affect_remove args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void affect_strip args( ( CHAR_DATA * ch, int sn ) );
bool is_affected args( ( CHAR_DATA * ch, int sn ) );
void affect_join args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void char_from_room args( ( CHAR_DATA * ch ) );
void char_to_room args( ( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ) );
OD *obj_to_char args( ( OBJ_DATA * obj, CHAR_DATA * ch ) );
void obj_from_char args( ( OBJ_DATA * obj ) );
int apply_ac args( ( OBJ_DATA * obj, int iWear ) );
OD *get_eq_char args( ( CHAR_DATA * ch, int iWear ) );
void equip_char args( ( CHAR_DATA * ch, OBJ_DATA * obj, int iWear ) );
void unequip_char args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list );
void obj_from_room args( ( OBJ_DATA * obj ) );
OD *obj_to_room args( ( OBJ_DATA * obj, ROOM_INDEX_DATA * pRoomIndex ) );
OD *obj_to_obj args( ( OBJ_DATA * obj, OBJ_DATA * obj_to ) );
void obj_from_obj args( ( OBJ_DATA * obj ) );
void extract_obj args( ( OBJ_DATA * obj ) );
void extract_exit args( ( ROOM_INDEX_DATA * room, EXIT_DATA * pexit ) );
void extract_room args( ( ROOM_INDEX_DATA * room ) );
void clean_room args( ( ROOM_INDEX_DATA * room ) );
void clean_obj args( ( OBJ_INDEX_DATA * obj ) );
void clean_mob args( ( MOB_INDEX_DATA * mob ) );
void clean_resets( ROOM_INDEX_DATA *room );
void extract_char args( ( CHAR_DATA * ch, bool fPull ) );
CD *get_char_room( CHAR_DATA * ch, const char *argument );
CD *get_char_world( CHAR_DATA * ch, const char *argument );
OD *get_obj_type( OBJ_INDEX_DATA * pObjIndexData );
OD *get_obj_list( CHAR_DATA * ch, const char *argument, OBJ_DATA * list );
OD *get_obj_list_rev( CHAR_DATA * ch, const char *argument, OBJ_DATA * list );
OD *get_obj_carry( CHAR_DATA * ch, const char *argument );
OD *get_obj_wear( CHAR_DATA * ch, const char *argument );
OD *get_obj_here( CHAR_DATA * ch, const char *argument );
OD *get_obj_world( CHAR_DATA * ch, const char *argument );
int get_obj_number( OBJ_DATA * obj );
int get_obj_weight( OBJ_DATA * obj );
bool room_is_dark args( ( ROOM_INDEX_DATA * pRoomIndex ) );
bool room_is_private args( ( CHAR_DATA * ch, ROOM_INDEX_DATA * pRoomIndex ) );
bool can_see args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
bool can_see_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
bool can_drop_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
const char *item_type_name args( ( OBJ_DATA * obj ) );
const char *affect_loc_name args( ( int location ) );
const char *affect_bit_name args( ( int vector ) );
const char *extra_bit_name args( ( int extra_flags ) );
const char *magic_bit_name args( ( int magic_flags ) );
ch_ret check_for_trap args( ( CHAR_DATA * ch, OBJ_DATA * obj, int flag ) );
ch_ret check_room_for_traps args( ( CHAR_DATA * ch, int flag ) );
bool is_trapped args( ( OBJ_DATA * obj ) );
OD *get_trap args( ( OBJ_DATA * obj ) );
ch_ret spring_trap args( ( CHAR_DATA * ch, OBJ_DATA * obj ) );
void name_stamp_stats args( ( CHAR_DATA * ch ) );
void fix_char args( ( CHAR_DATA * ch ) );
void showaffect args( ( CHAR_DATA * ch, AFFECT_DATA * paf ) );
void set_cur_obj args( ( OBJ_DATA * obj ) );
bool obj_extracted args( ( OBJ_DATA * obj ) );
void queue_extracted_obj args( ( OBJ_DATA * obj ) );
void clean_obj_queue args( ( void ) );
void set_cur_char args( ( CHAR_DATA * ch ) );
bool char_died args( ( CHAR_DATA * ch ) );
void queue_extracted_char args( ( CHAR_DATA * ch, bool extract ) );
void clean_char_queue args( ( void ) );
void add_timer args( ( CHAR_DATA * ch, short type, short count, DO_FUN * fun, int value ) );
TIMER *get_timerptr args( ( CHAR_DATA * ch, short type ) );
short get_timer args( ( CHAR_DATA * ch, short type ) );
void extract_timer args( ( CHAR_DATA * ch, TIMER * timer ) );
void remove_timer args( ( CHAR_DATA * ch, short type ) );
bool in_soft_range args( ( CHAR_DATA * ch, AREA_DATA * tarea ) );
bool in_hard_range args( ( CHAR_DATA * ch, AREA_DATA * tarea ) );
bool chance args( ( CHAR_DATA * ch, short percent ) );
bool chance_attrib args( ( CHAR_DATA * ch, short percent, short attrib ) );
OD *clone_object args( ( OBJ_DATA * obj ) );
void split_obj args( ( OBJ_DATA * obj, int num ) );
void separate_obj args( ( OBJ_DATA * obj ) );
bool empty_obj args( ( OBJ_DATA * obj, OBJ_DATA * destobj, ROOM_INDEX_DATA * destroom ) );
OD *find_obj( CHAR_DATA * ch, const char *argument, bool carryonly );
bool ms_find_obj args( ( CHAR_DATA * ch ) );
void worsen_mental_state args( ( CHAR_DATA * ch, int mod ) );
void better_mental_state args( ( CHAR_DATA * ch, int mod ) );
void boost_economy args( ( AREA_DATA * tarea, int gold ) );
void lower_economy args( ( AREA_DATA * tarea, int gold ) );
void economize_mobgold args( ( CHAR_DATA * mob ) );
bool economy_has args( ( AREA_DATA * tarea, int gold ) );
void add_kill args( ( CHAR_DATA * ch, CHAR_DATA * mob ) );
int times_killed args( ( CHAR_DATA * ch, CHAR_DATA * mob ) );
void check_switches( bool possess );
void check_switch( CHAR_DATA *ch, bool possess );

/* interp.c */
bool check_pos( CHAR_DATA * ch, short position );
void interpret( CHAR_DATA * ch, const char *argument );
bool is_number( const char *arg );
int number_argument( const char *argument, char *arg );
char *one_argument( char *argument, char *arg_first );
const char *one_argument( const char *argument, char *arg_first );
const char *one_argument2( const char *argument, char *arg_first );
ST *find_social( const char *command );
CMDTYPE *find_command( const char *command );
void hash_commands( void );
void start_timer( struct timeval * sttime );
time_t end_timer( struct timeval * sttime );
void send_timer( struct timerset * vtime, CHAR_DATA * ch );
void update_userec( struct timeval * time_used, struct timerset * userec );

/* magic.c */
bool process_spell_components args( ( CHAR_DATA * ch, int sn ) );
int ch_slookup args( ( CHAR_DATA * ch, const char *name ) );
int find_spell args( ( CHAR_DATA * ch, const char *name, bool know ) );
int find_skill args( ( CHAR_DATA * ch, const char *name, bool know ) );
int find_weapon args( ( CHAR_DATA * ch, const char *name, bool know ) );
int find_tongue args( ( CHAR_DATA * ch, const char *name, bool know ) );
int skill_lookup args( ( const char *name ) );
int herb_lookup args( ( const char *name ) );
int personal_lookup args( ( CHAR_DATA * ch, const char *name ) );
int slot_lookup args( ( int slot ) );
int bsearch_skill args( ( const char *name, int first, int top ) );
int bsearch_skill_exact args( ( const char *name, int first, int top ) );
bool saves_poison_death args( ( int level, CHAR_DATA * victim ) );
bool saves_wand args( ( int level, CHAR_DATA * victim ) );
bool saves_para_petri args( ( int level, CHAR_DATA * victim ) );
bool saves_breath args( ( int level, CHAR_DATA * victim ) );
bool saves_spell_staff args( ( int level, CHAR_DATA * victim ) );
ch_ret obj_cast_spell args( ( int sn, int level, CHAR_DATA * ch, CHAR_DATA * victim, OBJ_DATA * obj ) );
int dice_parse( CHAR_DATA * ch, int level, const char *texp );
SK *get_skilltype args( ( int sn ) );

/* request.c */
void init_request_pipe args( ( void ) );
void check_requests args( ( void ) );

/* save.c */
/* object saving defines for fread/write_obj. -- Altrag */
#define OS_CARRY	0
#define OS_CORPSE	1
void write_corpses( CHAR_DATA * ch, const char *name );
void save_char_obj args( ( CHAR_DATA * ch ) );
void save_clone args( ( CHAR_DATA * ch ) );
bool load_char_obj args( ( DESCRIPTOR_DATA * d, char *name, bool preload, bool copyover ) );
void set_alarm args( ( long seconds ) );
void requip_char args( ( CHAR_DATA * ch ) );
void fwrite_obj args( ( CHAR_DATA * ch, OBJ_DATA * obj, FILE * fp, int iNest, short os_type, bool hotboot ) );
void fread_obj args( ( CHAR_DATA * ch, FILE * fp, short os_type ) );
void de_equip_char args( ( CHAR_DATA * ch ) );
void re_equip_char args( ( CHAR_DATA * ch ) );
void save_home args( ( CHAR_DATA * ch ) );

/* shops.c */

/* special.c */
SF *spec_lookup( const char *name );

/* tables.c */
int get_skill( const char *skilltype );
char *spell_name args( ( SPELL_FUN * spell ) );
char *skill_name args( ( DO_FUN * skill ) );
void load_skill_table args( ( void ) );
void save_skill_table args( ( void ) );
void sort_skill_table args( ( void ) );
void load_socials args( ( void ) );
void save_socials args( ( void ) );
void load_commands args( ( void ) );
void save_commands args( ( void ) );
SPELL_FUN *spell_function( const char *name );
DO_FUN *skill_function( const char *name );
void load_herb_table args( ( void ) );
void save_herb_table args( ( void ) );

/* track.c */
void found_prey args( ( CHAR_DATA * ch, CHAR_DATA * victim ) );
void hunt_victim args( ( CHAR_DATA * ch ) );

/* update.c */
void advance_level args( ( CHAR_DATA * ch, int ability ) );
void gain_exp args( ( CHAR_DATA * ch, int gain, int ability ) );
void gain_condition args( ( CHAR_DATA * ch, int iCond, int value ) );
void update_handler args( ( void ) );
void reboot_check args( ( time_t reset ) );
#if 0
void reboot_check args( ( char *arg ) );
#endif
void auction_update args( ( void ) );
void remove_portal args( ( OBJ_DATA * portal ) );
int max_level( CHAR_DATA * ch, int ability );

/* hashstr.c */
const char *str_alloc( const char *str );
const char *quick_link( const char *str );
int str_free( const char *str );
void show_hash( int count );
char *hash_stats( void );
char *check_hash( const char *str );
void hash_dump( int hash );
void show_high_hash( int top );
bool in_hash_table( const char *str );

/* newscore.c */
const char *get_race( CHAR_DATA * ch );

#undef	SK
#undef	ST
#undef	CD
#undef	MID
#undef	OD
#undef	OID
#undef	RID
#undef	SF
#undef	BD
#undef	CL
#undef	EDD
#undef	RD
#undef	ED

/*
* mudprograms stuff
*/
extern CHAR_DATA *supermob;
extern OBJ_DATA *supermob_obj;

void oprog_speech_trigger( const char *txt, CHAR_DATA * ch );
void oprog_random_trigger( OBJ_DATA * obj );
void oprog_wear_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
bool oprog_use_trigger( CHAR_DATA * ch, OBJ_DATA * obj, CHAR_DATA * vict, OBJ_DATA * targ, void *vo );
void oprog_remove_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_sac_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_damage_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_repair_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_drop_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_zap_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
char *oprog_type_to_name( int type );
int rprog_custom_trigger( char *command, char *argument, CHAR_DATA * ch );
int mprog_custom_trigger( char *command, char *argument, CHAR_DATA * ch );
int oprog_custom_trigger( char *command, char *argument, CHAR_DATA * ch );
void oprog_greet_trigger( CHAR_DATA * ch );
void oprog_get_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_examine_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_pull_trigger( CHAR_DATA * ch, OBJ_DATA * obj );
void oprog_push_trigger( CHAR_DATA * ch, OBJ_DATA * obj );

/* mud prog defines */

#define ERROR_PROG        -1
#define IN_FILE_PROG       0
#define ACT_PROG           BV00
#define SPEECH_PROG        BV01
#define RAND_PROG          BV02
#define FIGHT_PROG         BV03
#define RFIGHT_PROG        BV03
#define DEATH_PROG         BV04
#define RDEATH_PROG        BV04
#define HITPRCNT_PROG      BV05
#define ENTRY_PROG         BV06
#define ENTER_PROG         BV06
#define GREET_PROG         BV07
#define RGREET_PROG	   BV07
#define OGREET_PROG        BV07
#define ALL_GREET_PROG	   BV08
#define GIVE_PROG	   BV09
#define BRIBE_PROG	   BV10
#define HOUR_PROG	   BV11
#define TIME_PROG	   BV12
#define WEAR_PROG          BV13
#define REMOVE_PROG        BV14
#define SAC_PROG           BV15
#define LOOK_PROG          BV16
#define EXA_PROG           BV17
#define ZAP_PROG           BV18
#define GET_PROG 	   BV19
#define DROP_PROG	   BV20
#define DAMAGE_PROG	   BV21
#define REPAIR_PROG	   BV22
#define RANDIW_PROG	   BV23
#define SPEECHIW_PROG	   BV24
#define PULL_PROG	   BV25
#define PUSH_PROG	   BV26
#define SLEEP_PROG         BV27
#define REST_PROG          BV28
#define LEAVE_PROG         BV29
#define SCRIPT_PROG	   BV30
#define USE_PROG           BV31

void rprog_leave_trigger( CHAR_DATA * ch );
void rprog_enter_trigger( CHAR_DATA * ch );
void rprog_sleep_trigger( CHAR_DATA * ch );
void rprog_rest_trigger( CHAR_DATA * ch );
void rprog_rfight_trigger( CHAR_DATA * ch );
void rprog_death_trigger( CHAR_DATA * killer, CHAR_DATA * ch );
void rprog_speech_trigger( const char *txt, CHAR_DATA * ch );
void rprog_random_trigger( CHAR_DATA * ch );
void rprog_time_trigger( CHAR_DATA * ch );
void rprog_hour_trigger( CHAR_DATA * ch );
char *rprog_type_to_name( int type );

#define OPROG_ACT_TRIGGER
#ifdef OPROG_ACT_TRIGGER
void oprog_act_trigger( const char *buf, OBJ_DATA * mobj, CHAR_DATA * ch, OBJ_DATA * obj, void *vo );
#endif
#define RPROG_ACT_TRIGGER
#ifdef RPROG_ACT_TRIGGER
void rprog_act_trigger( const char *buf, ROOM_INDEX_DATA * room, CHAR_DATA * ch, OBJ_DATA * obj, void *vo );
#endif

#endif /* ZEROPOINT_GAME_API_H */
