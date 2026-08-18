#ifndef ZEROPOINT_GAME_SKILLS_H
#define ZEROPOINT_GAME_SKILLS_H

typedef enum
{
   TAR_IGNORE, TAR_CHAR_OFFENSIVE, TAR_CHAR_DEFENSIVE, TAR_CHAR_SELF,
   TAR_OBJ_INV
} target_types;

typedef enum
{
   SKILL_UNKNOWN, SKILL_SPELL, SKILL_SKILL, SKILL_WEAPON, SKILL_TONGUE,
   SKILL_HERB
} skill_types;

struct timerset
{
   int num_uses;
   struct timeval total_time;
   struct timeval min_time;
   struct timeval max_time;
};

/*
* Skills include spells as a particular case.
*/
struct skill_type
{
   const char *name; /* Name of skill     */
   SPELL_FUN *spell_fun;   /* Spell pointer (for spells) */
   const char *spell_fun_name;   /* Spell function name - Trax */
   DO_FUN *skill_fun;      /* Skill pointer (for skills) */
   const char *skill_fun_name;   /* Skill function name - Trax */
   short target;  /* Legal targets     */
   short minimum_position; /* Position for caster / user */
   short slot; /* Slot for #OBJECT loading   */
   short min_mana;   /* Minimum mana used    */
   short beats;   /* Rounds required to use skill  */
   const char *noun_damage;   /* Damage message    */
   const char *msg_off; /* Wear off message     */
   short guild;   /* Which guild the skill belongs to */
   short min_level;  /* Minimum level to be able to cast */
   short type; /* Spell/Skill/Weapon/Tongue  */
   int flags;  /* extra stuff       */
   const char *hit_char;   /* Success message to caster  */
   const char *hit_vict;   /* Success message to victim  */
   const char *hit_room;   /* Success message to room */
   const char *miss_char;  /* Failure message to caster  */
   const char *miss_vict;  /* Failure message to victim  */
   const char *miss_room;  /* Failure message to room */
   const char *die_char;   /* Victim death msg to caster */
   const char *die_vict;   /* Victim death msg to victim */
   const char *die_room;   /* Victim death msg to room   */
   const char *imm_char;   /* Victim immune msg to caster   */
   const char *imm_vict;   /* Victim immune msg to victim   */
   const char *imm_room;   /* Victim immune msg to room  */
   const char *dice; /* Dice roll         */
   int value;  /* Misc value        */
   char saves; /* What saving spell applies  */
   char difficulty;  /* Difficulty of casting/learning */
   SMAUG_AFF *first_affect;  /* Spell affects, if any   */
   SMAUG_AFF *last_affect;
   const char *components; /* Spell components, if any   */
   const char *teachers;   /* Skill requires a special teacher */
   char participants;   /* # of required participants */
   struct timerset userec; /* Usage record         */
   int alignment; /* for jedi powers */
};

struct auction_data
{
   OBJ_DATA *item;   /* a pointer to the item */
   CHAR_DATA *seller;   /* a pointer to the seller - which may NOT quit */
   CHAR_DATA *buyer; /* a pointer to the buyer - which may NOT quit */
   int bet; /* last bet - or 0 if noone has bet anything */
   short going;   /* 1,2, sold */
   short pulse;   /* how many pulses (.25 sec) until another call-out ? */
   int starting;
};

/*
* These are skill_lookup return values for common skills and spells.
*/
extern short gsn_starfighters;
extern short gsn_midships;
extern short gsn_capitalships;
extern short gsn_weaponsystems;
extern short gsn_navigation;
extern short gsn_shipsystems;
extern short gsn_tractorbeams;
extern short gsn_shipmaintenance;
extern short gsn_spacecombat;
extern short gsn_spacecombat2;
extern short gsn_spacecombat3;

extern short gsn_reinforcements;
extern short gsn_postguard;

extern short gsn_eliteguard;
extern short gsn_specialforces;
extern short gsn_jail;
extern short gsn_smalltalk;
extern short gsn_propeganda;
extern short gsn_bribe;
extern short gsn_seduce;
extern short gsn_masspropeganda;
extern short gsn_gather_intelligence;

extern short gsn_torture;
extern short gsn_snipe;
extern short gsn_throw;
extern short gsn_disguise;
extern short gsn_mine;
extern short gsn_grenades;
extern short gsn_first_aid;

extern short gsn_beg;
extern short gsn_makeblade;
extern short gsn_makejewelry;
extern short gsn_makeblaster;
extern short gsn_makelight;
extern short gsn_makecomlink;
extern short gsn_makegrenade;
extern short gsn_makelandmine;
extern short gsn_makearmor;
extern short gsn_makeshield;
extern short gsn_makecontainer;
extern short gsn_gemcutting;
extern short gsn_lightsaber_crafting;
extern short gsn_spice_refining;

extern short gsn_detrap;
extern short gsn_backstab;
extern short gsn_circle;
extern short gsn_dodge;
extern short gsn_hide;
extern short gsn_peek;
extern short gsn_pick_lock;
extern short gsn_scan;
extern short gsn_sneak;
extern short gsn_steal;
extern short gsn_gouge;
extern short gsn_track;
extern short gsn_search;
extern short gsn_dig;
extern short gsn_mount;
extern short gsn_bashdoor;
extern short gsn_berserk;
extern short gsn_hitall;
extern short gsn_pickshiplock;
extern short gsn_hijack;

extern short gsn_disarm;
extern short gsn_enhanced_damage;
extern short gsn_kick;
extern short gsn_parry;
extern short gsn_rescue;
extern short gsn_second_attack;
extern short gsn_third_attack;
extern short gsn_dual_wield;

extern short gsn_aid;

/* used to do specific lookups */
extern short gsn_first_spell;
extern short gsn_first_skill;
extern short gsn_first_weapon;
extern short gsn_first_tongue;
extern short gsn_top_sn;

/* spells */
extern short gsn_blindness;
extern short gsn_charm_person;
extern short gsn_aqua_breath;
extern short gsn_invis;
extern short gsn_mass_invis;
extern short gsn_poison;
extern short gsn_sleep;
extern short gsn_possess;
extern short gsn_fireball; /* for fireshield  */
extern short gsn_lightning_bolt; /* for shockshield */

/* newer attack skills */
extern short gsn_punch;
extern short gsn_bash;
extern short gsn_stun;

extern short gsn_poison_weapon;
extern short gsn_climb;

extern short gsn_blasters;
extern short gsn_force_pikes;
extern short gsn_bowcasters;
extern short gsn_lightsabers;
extern short gsn_vibro_blades;
extern short gsn_flexible_arms;
extern short gsn_talonous_arms;
extern short gsn_bludgeons;

extern short gsn_grip;

/* languages */
extern short gsn_common;
extern short gsn_wookiee;
extern short gsn_twilek;
extern short gsn_rodian;
extern short gsn_hutt;
extern short gsn_mon_calamari;
extern short gsn_noghri;
extern short gsn_gamorrean;
extern short gsn_jawa;
extern short gsn_adarian;
extern short gsn_ewok;
extern short gsn_verpine;
extern short gsn_defel;
extern short gsn_trandoshan;
extern short gsn_chadra_fan;
extern short gsn_quarren;
extern short gsn_duinduogwuin;

// Utility macros.
int umin( int check, int ncheck );
int umax( int check, int ncheck );
int urange( int mincheck, int check, int maxcheck );

#define UMIN( a, b )      ( umin( (a), (b) ) )
#define UMAX( a, b )      ( umax( (a), (b) ) )
#define URANGE(a, b, c )  ( urange( (a), (b), (c) ) )
#define LOWER( c )        ( (c) >= 'A' && (c) <= 'Z' ? (c) + 'a' - 'A' : (c) )
#define UPPER( c )        ( (c) >= 'a' && (c) <= 'z' ? (c) + 'A' - 'a' : (c) )

#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) ^= (bit))

/*
* Memory allocation macros.
*/
#define CREATE(result, type, number)                                    \
   do                                                                      \
{                                                                       \
   if (!((result) = (type *) calloc ((number), sizeof(type))))          \
{                                                                    \
   perror("malloc failure");                                         \
   fprintf(stderr, "Malloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
   abort();                                                          \
}                                                                    \
} while(0)

#define RECREATE(result,type,number)                                    \
   do                                                                      \
{                                                                       \
   if(!((result) = (type *)realloc((result), sizeof(type) * (number)))) \
{                                                                    \
   perror("realloc failure");                                        \
   fprintf(stderr, "Realloc failure @ %s:%d\n", __FILE__, __LINE__); \
   abort();                                                          \
}                                                                    \
} while(0)

#define DISPOSE(point)                      \
   do                                          \
{                                           \
   if( (point) )                            \
{                                        \
   free( (void*)(point) );		    \
   (point) = NULL;                       \
}                                        \
} while(0)

#ifdef HASHSTR
#define STRALLOC(point)		str_alloc((point))
#define QUICKLINK(point)	quick_link((point))
#define STRFREE(point)                          \
   do                                              \
{                                               \
   if((point))                                  \
{                                            \
   if( str_free((point)) == -1 )             \
   bug( "&RSTRFREEing bad pointer: %s, line %d", __FILE__, __LINE__ ); \
   (point) = NULL;                           \
}                                            \
} while(0)
#else
#define STRALLOC(point)		strdup((point))
#define QUICKLINK(point)	strdup((point))
#define STRFREE(point)		DISPOSE((point))
#endif

/* double-linked list handling macros -Thoric */
/* Updated by Scion 8/6/1999 */
#define LINK(link, first, last, next, prev) \
   do                                          \
{                                           \
   if ( !(first) )                          \
{                                        \
   (first) = (link);                     \
   (last) = (link);                      \
}                                        \
   else                                     \
   (last)->next = (link);                \
   (link)->next = NULL;                     \
   if ((first) == (link))                   \
   (link)->prev = NULL;                  \
   else                                     \
   (link)->prev = (last);                \
   (last) = (link);                         \
} while(0)

#define INSERT(link, insert, first, next, prev) \
   do                                              \
{                                               \
   (link)->prev = (insert)->prev;               \
   if ( !(insert)->prev )                       \
   (first) = (link);                         \
   else                                         \
   (insert)->prev->next = (link);            \
   (insert)->prev = (link);                     \
   (link)->next = (insert);                     \
} while(0)

#define UNLINK(link, first, last, next, prev)   \
   do                                              \
{                                               \
   if ( !(link)->prev )                         \
{                                            \
   (first) = (link)->next;                   \
   if ((first))                              \
   (first)->prev = NULL;                  \
}                                            \
   else                                         \
{                                            \
   (link)->prev->next = (link)->next;        \
}                                            \
   if ( !(link)->next )                         \
{                                            \
   (last) = (link)->prev;                    \
   if((last))                                \
   (last)->next = NULL;                   \
}                                            \
   else                                         \
{                                            \
   (link)->next->prev = (link)->prev;        \
}                                            \
} while(0)

#define CHECK_LINKS(first, last, next, prev, type)		\
   do {								\
   type *ptr, *pptr = NULL;					\
   if ( !(first) && !(last) )					\
   break;							\
   if ( !(first) )						\
{								\
   bug( "CHECK_LINKS: last with NULL first!  %s.",		\
   __STRING(first) );					\
   for ( ptr = (last); ptr->prev; ptr = ptr->prev );		\
   (first) = ptr;						\
}								\
   else if ( !(last) )						\
{								\
   bug( "CHECK_LINKS: first with NULL last!  %s.",		\
   __STRING(first) );					\
   for ( ptr = (first); ptr->next; ptr = ptr->next );		\
   (last) = ptr;						\
}								\
   if ( (first) )						\
{								\
   for ( ptr = (first); ptr; ptr = ptr->next )			\
{								\
   if ( ptr->prev != pptr )					\
{								\
   bug( "CHECK_LINKS(%s): %p:->prev != %p.  Fixing.",	\
   __STRING(first), ptr, pptr );			\
   ptr->prev = pptr;					\
}								\
   if ( ptr->prev && ptr->prev->next != ptr )		\
{								\
   bug( "CHECK_LINKS(%s): %p:->prev->next != %p.  Fixing.",\
   __STRING(first), ptr, ptr );			\
   ptr->prev->next = ptr;					\
}								\
   pptr = ptr;						\
}								\
   pptr = NULL;						\
}								\
   if ( (last) )							\
{								\
   for ( ptr = (last); ptr; ptr = ptr->prev )			\
{								\
   if ( ptr->next != pptr )					\
{								\
   bug( "CHECK_LINKS (%s): %p:->next != %p.  Fixing.",	\
   __STRING(first), ptr, pptr );			\
   ptr->next = pptr;					\
}								\
   if ( ptr->next && ptr->next->prev != ptr )		\
{								\
   bug( "CHECK_LINKS(%s): %p:->next->prev != %p.  Fixing.",\
   __STRING(first), ptr, ptr );			\
   ptr->next->prev = ptr;					\
}								\
   pptr = ptr;						\
}								\
}								\
   } while(0)

#define ASSIGN_GSN(gsn, skill)					\
   do								\
{								\
   if ( ((gsn) = skill_lookup((skill))) == -1 )		\
   fprintf( stderr, "ASSIGN_GSN: Skill %s not found.\n",	\
   (skill) );					\
} while(0)

#define CHECK_SUBRESTRICTED(ch)					\
   do								\
{								\
   if ( (ch)->substate == SUB_RESTRICTED )			\
{								\
   send_to_char( "You cannot use this command from within another command.\r\n", ch );	\
   return;							\
}								\
} while(0)

/*
* Character macros.
*/
#define IS_NPC(ch)		(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_IMMORTAL(ch)		(get_trust((ch)) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)		(get_trust((ch)) >= LEVEL_HERO)
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define HAS_BODYPART(ch, part)	((ch)->xflags == 0 || IS_SET((ch)->xflags, (part)))

#define IS_GOOD(ch)		((ch)->alignment >= 350)
#define IS_EVIL(ch)		((ch)->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		((ch)->position > POS_SLEEPING)
#define GET_AC(ch)        ( (ch)->armor + ( IS_AWAKE(ch) ? get_dex_defensive_bonus( get_curr_dex(ch) ) : 0 ) \
   - ( (ch)->race == RACE_DEFEL ? (ch)->skill_level[COMBAT_ABILITY]*2+5 : (ch)->skill_level[COMBAT_ABILITY]/2 ) )

#define GET_HITROLL(ch)   ((ch)->hitroll \
   + get_str_tohit_bonus( get_curr_str(ch) ) \
   + (2-(abs((ch)->mental_state)/10)))

#define GET_DAMROLL(ch)   ((ch)->damroll \
   + get_str_todam_bonus( get_curr_str(ch) ) \
   + (((ch)->mental_state > 5 \
   && (ch)->mental_state < 15) ? 1 : 0))

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
   (ch)->in_room->room_flags,		    \
   ROOM_INDOORS) && !IS_SET(               \
   (ch)->in_room->room_flags,              \
   ROOM_SPACECRAFT) )

#define IS_DRUNK(ch, drunk)     (number_percent() < \
   ( (ch)->pcdata->condition[COND_DRUNK] \
   * 2 / (drunk) ) )

#define IS_CLANNED(ch)		(!IS_NPC((ch))				    \
   && (ch)->pcdata->clan			    )

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))


#define EXIT(ch, door)		( get_exit( (ch)->in_room, door ) )

#define CAN_GO(ch, door)	(EXIT((ch),(door))			 \
   && (EXIT((ch),(door))->to_room != NULL)  \
   && !IS_SET(EXIT((ch), (door))->exit_info, EX_CLOSED))

#define IS_VALID_SN(sn)		( (sn) >=0 && (sn) < MAX_SKILL		     \
   && skill_table[(sn)]			     \
   && skill_table[(sn)]->name )

#define IS_VALID_HERB(sn)	( (sn) >=0 && (sn) < MAX_HERB		     \
   && herb_table[(sn)]			     \
   && herb_table[(sn)]->name )

#define SPELL_FLAG(skill, flag)	( IS_SET((skill)->flags, (flag)) )
#define SPELL_DAMAGE(skill)	( ((skill)->flags     ) & 7 )
#define SPELL_ACTION(skill)	( ((skill)->flags >> 3) & 7 )
#define SPELL_CLASS(skill)	( ((skill)->flags >> 6) & 7 )
#define SPELL_POWER(skill)	( ((skill)->flags >> 9) & 3 )
#define SET_SDAM(skill, val)	( (skill)->flags =  ((skill)->flags & SDAM_MASK) + ((val) & 7) )
#define SET_SACT(skill, val)	( (skill)->flags =  ((skill)->flags & SACT_MASK) + (((val) & 7) << 3) )
#define SET_SCLA(skill, val)	( (skill)->flags =  ((skill)->flags & SCLA_MASK) + (((val) & 7) << 6) )
#define SET_SPOW(skill, val)	( (skill)->flags =  ((skill)->flags & SPOW_MASK) + (((val) & 3) << 9) )

/* Retired and guest imms. */
#define IS_RETIRED(ch) (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_RETIRED))
#define IS_GUEST(ch) (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_GUEST))

/* RIS by gsn lookups. -- Altrag.
Will need to add some || stuff for spells that need a special GSN. */

#define IS_FIRE(dt)		( IS_VALID_SN(dt) &&			     \
   SPELL_DAMAGE(skill_table[(dt)]) == SD_FIRE )
#define IS_COLD(dt)		( IS_VALID_SN(dt) &&			     \
   SPELL_DAMAGE(skill_table[(dt)]) == SD_COLD )
#define IS_ACID(dt)		( IS_VALID_SN(dt) &&			     \
   SPELL_DAMAGE(skill_table[(dt)]) == SD_ACID )
#define IS_ELECTRICITY(dt)	( IS_VALID_SN(dt) &&			     \
   SPELL_DAMAGE(skill_table[(dt)]) == SD_ELECTRICITY )
#define IS_ENERGY(dt)		( IS_VALID_SN(dt) &&			     \
   SPELL_DAMAGE(skill_table[(dt)]) == SD_ENERGY )

#define IS_DRAIN(dt)		( IS_VALID_SN(dt) &&			     \
   SPELL_DAMAGE(skill_table[(dt)]) == SD_DRAIN )

#define IS_POISON(dt)		( IS_VALID_SN(dt) &&			     \
   SPELL_DAMAGE(skill_table[(dt)]) == SD_POISON )

#define NOT_AUTHED(ch)		(!IS_NPC(ch) && ch->pcdata->auth_state <= 3  \
   && IS_SET(ch->pcdata->flags, PCFLAG_UNAUTHED) )

#define IS_WAITING_FOR_AUTH(ch) (!IS_NPC(ch) && ch->desc		     \
   && ch->pcdata->auth_state == 1		     \
   && IS_SET(ch->pcdata->flags, PCFLAG_UNAUTHED) )

/*
* Object macros.
*/
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))

/*
* Description macros.
*/
#define PERS(ch, looker)	( can_see( (looker), (ch) ) ?		\
   ( IS_NPC(ch) ? (ch)->short_descr	\
   : (ch)->name ) : "someone" )

#define log_string( txt )	( log_string_plus( (txt), LOG_NORMAL, LEVEL_LOG ) )

/*
* Structure for a command in the command lookup table.
*/
struct cmd_type
{
   CMDTYPE *next;
   const char *name;
   DO_FUN *do_fun;
   const char *fun_name;
   short position;
   short level;
   short log;
   struct timerset userec;
};

/*
* Structure for a social in the socials table.
*/
struct social_type
{
   SOCIALTYPE *next;
   const char *name;
   const char *char_no_arg;
   const char *others_no_arg;
   const char *char_found;
   const char *others_found;
   const char *vict_found;
   const char *char_auto;
   const char *others_auto;
};

struct specfun_list

#endif /* ZEROPOINT_GAME_SKILLS_H */
