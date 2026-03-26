/***************************************************************************
* Star Wars Reality Code Additions and changes from the Smaug Code	   *
* copyright (c) 1997, 1998 by Sean Cooper				   *
* Starwars and Starwars Names copyright (c) Lucasfilm Ltd.		   *
* SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider			   *
* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,			   *
* Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops		   *
* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael	   *
* Chastain, Michael Quan, and Mitchell Tse.				   *
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,	   *
* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.	   *
* ------------------------------------------------------------------------ *
*		      ____________   __	    ______			   *
*   Aldegard	Jot  (	 ___   ___| /  \   |  __  )   Thanos	  Trog	   *
*	 ______________\  \|   |  /  ^	 \ |	 <_______________	   *
*	 \________________/|___|/___/"\___\|__|\________________/	   *
*		   \   \/ \/   //  \   |  __  )(  ___/`			   *
*		     \	     //	 ^   \ |     <__)  \			   *
*		       \_!_//___/"\___\|__|\______/TM			   *
* (c) 2001, 2002	    M	    U	     D		      Ver 1.1	   *
* ------------------------------------------------------------------------ *
*			    Main mud header file			   *
****************************************************************************/
#ifndef MUD_HEADER_FILE
#define MUD_HEADER_FILE

#include <stdarg.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#if !defined( WIN32 )
#include <sys/time.h>
#include <sys/cdefs.h>
#include <netinet/in.h>
#endif
#if defined( WIN32 )
#include <winsock.h>
#include <sys/timeb.h> /*for _ftime(), uses _timeb struct*/
#endif
#include <string.h>
#include <regex.h>
#include <list>
#include <forward_list>
#include <algorithm>

#ifdef DMALLOC			/* Thanos  -- to dla mnie */
#include <dmalloc.h>
#endif

/*! Trog: XML */
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

/* #include <malloc_dbg.h> */

#include "files.h"


#if defined( __CYGWIN32__ )
#define __STRING(x)	#x
#endif


#include "defines.h"
#include "typedefs.h"

/* Trog: structures */

#include "structs/who_data.h"
#include "structs/last_data.h"
#include "structs/complain_data.h"
#include "structs/class_type.h"
#include "structs/flag_stat_type.h"
#include "structs/flag_type.h"
#include "structs/command_type.h"
#include "structs/ban_data.h"
#include "structs/season_data.h"
#include "structs/hour_min_sec.h"
#include "structs/wizent.h"
#include "structs/for_data.h"
#include "structs/rat_data.h"
#include "structs/descriptor_data.h"
#include "structs/str_app_type.h"
#include "structs/int_app_type.h"
#include "structs/wis_app_type.h"
#include "structs/dex_app_type.h"
#include "structs/con_app_type.h"
#include "structs/cha_app_type.h"
#include "structs/lck_app_type.h"
#include "structs/frc_app_type.h"
#include "structs/helps_file.h"
#include "structs/help_data.h"
#include "structs/shop_data.h"
#include "structs/repairshop_data.h"
#include "structs/act_prog_data.h"
#include "structs/mob_prog_act_list.h"
#include "structs/mob_prog_data.h"
#include "structs/script_data.h"
#include "structs/race_data.h"
#include "structs/lcnv_data.h"
#include "structs/lang_data.h"
#include "structs/known_lang.h"
#include "structs/race_type.h"
#include "structs/madule_data.h"
#include "structs/star_data.h"
#include "structs/moon_data.h"
#include "structs/dock_data.h"
#include "structs/crew_data.h"
#include "structs/shipdock_data.h"
#include "structs/hangar_data.h"
#include "structs/transponder_data.h"
#include "structs/smap_data.h"
#include "structs/space_data.h"
#include "structs/bounty_data.h"
#include "structs/inform_data.h"
#include "structs/guard_data.h"
#include "structs/planet_data.h"
#include "structs/member_data.h"
#include "structs/politics_data.h"
#include "structs/hq_room_desc.h"
#include "structs/hq_room.h"
#include "structs/hq_data.h"
#include "structs/clan_data.h"
#include "structs/course_data.h"
#include "structs/turret_data.h"
#include "structs/ship_data.h"
#include "structs/cargo_data.h"
#include "structs/material_data.h"
#include "structs/warehouse_data.h"
#include "structs/astro_data.h"
#include "structs/missile_data.h"
#include "structs/turbocar_data.h"
#include "structs/tc_station.h"
#include "structs/tourney_data.h"
#include "structs/note_data.h"
#include "structs/board_data.h"
#include "structs/affect_data.h"
#include "structs/smaug_affect.h"
#include "structs/requirement_data.h"
#include "structs/ilist_data.h"
#include "structs/timer_data.h"
#include "structs/quest_index_data.h"
#include "structs/quest_data.h"
#include "structs/chapter_index_data.h"
#include "structs/chapter_data.h"
#include "structs/q_command_data.h"
#include "structs/q_action_data.h"
#include "structs/event_data.h"
#include "structs/quest_mob_data.h"
#include "structs/quest_obj_data.h"
#include "structs/component_data.h"
#include "structs/part_data.h"
#include "structs/project_data.h"
#include "structs/mob_index_data.h"
#include "structs/cloning_data.h"
#include "structs/cloning_tmp_data.h"
#include "structs/ship_index_data.h"
#include "structs/ship_room_data.h"
#include "structs/ship_exit_data.h"
#include "structs/nameslist_data.h"
#include "structs/hunt_hate_fear.h"
#include "structs/fighting_data.h"
#include "structs/extracted_char_data.h"
#include "structs/crime_data.h"
#include "structs/suspect_data.h"
#include "structs/alias_data.h"
#include "structs/var_data.h"
#include "structs/fevent_data.h"
#include "structs/known_char_data.h"
#include "structs/char_desc.h"
#include "structs/dialog_data.h"
#include "structs/dialog_node.h"
#include "structs/char_data.h"
#include "structs/killed_data.h"
#include "structs/pc_data.h"
#include "structs/liq_type.h"
#include "structs/extra_descr_data.h"
#include "structs/obj_index_data.h"
#include "structs/obj_data.h"
#include "structs/exit_data.h"
#include "structs/reset_data.h"
#include "structs/area_data.h"
#include "structs/area_tmp_data.h"
#include "structs/godlist_data.h"
#include "structs/system_data.h"
#include "structs/room_index_data.h"
#include "structs/teleport_data.h"
#include "structs/timerset.h"
#include "structs/skill_type.h"
#include "structs/auction_data.h"
#include "structs/bets_progres.h"
#include "structs/stock_exchange_data.h"
#include "structs/deposit_data.h"
#include "structs/cmd_type.h"
#include "structs/social_type.h"


/*
* Short scalar types.
* Diavolo reports AIX compiler has bugs with short types.
*/

#ifndef BERR
#define BERR	 255
#endif

#ifndef TRUE
#define TRUE true
#endif //TRUE
#ifndef FALSE
#define FALSE false
#endif //FALSE


/* Trog: MAXINT i MAXSHORT */
#ifdef MAXINT
#undef MAXINT
#endif
#define MAXINT ~(1 << ((8 * (int)sizeof(int)) - 1))
#ifdef MAXSHORT
#undef MAXSHORT
#endif
#define MAXSHORT (short)~(1 << ((8 * (short)sizeof(short)) - 1))

#define MIN_PASS_LEN	5
#define MAX_PASS_TRY	2  /*Added by Thanos :maxymalna ilosc pomylek z haslem*/
#define MAX_LAST	30 /*Added by Ratm(ile lini pokazuje komenda last) */
#define MAX_LAST_TELL	15 /*Added by Thanos (do last_tell'a)*/
#define MAX_LAST_CTALK	20 /*Added by Thanos*/
#define MAX_ALIAS	20 /*added by Thanos*/
#define MAX_SCRIPTS	30 /*added by Thanos ile skrypt�w naraz mo�e mie� mob*/

#define MAX_VNUM	131070 /*128k*/


#define VNUM( room )	(room ? (room->vnum ? room->vnum : room->svnum) : 0)

#define DUR_CONV	23.333333333333333333333333
#define HIDDEN_TILDE	'*'

#define MAX_LAYERS			8	/* maximum clothing layers */
#define MAX_NEST			100	/* maximum container nesting */

#define MAX_PAGER			(MAXINT-23647)	/* maximum pager length */

/* Trog */
#define REG_MATCH			1
#define REG_CFLAGS			REG_EXTENDED | REG_ICASE | REG_NOSUB
#define REG_EFLAGS			0


#define LEVEL_SUPREME			MAX_LEVEL
#define LEVEL_HERO				(MAX_LEVEL - 5)
#define LEVEL_IMMORTAL			(MAX_LEVEL - 4)
#define LEVEL_INFINITE			(MAX_LEVEL - 1)
#define LEVEL_ETERNAL			(MAX_LEVEL - 1)
#define LEVEL_IMPLEMENTOR		(MAX_LEVEL - 1)
#define LEVEL_SUB_IMPLEM		(MAX_LEVEL - 1)
#define LEVEL_ASCENDANT			(MAX_LEVEL - 2)
#define LEVEL_GREATER			(MAX_LEVEL - 2)
#define LEVEL_GOD				(MAX_LEVEL - 2)
#define LEVEL_LESSER			(MAX_LEVEL - 3)
#define LEVEL_TRUEIMM			(MAX_LEVEL - 3)
#define LEVEL_DEMI				(MAX_LEVEL - 3)
#define LEVEL_SAVIOR			(MAX_LEVEL - 3)
#define LEVEL_CREATOR			(MAX_LEVEL - 3)
#define LEVEL_ACOLYTE			(MAX_LEVEL - 4)
#define LEVEL_NEOPHYTE			(MAX_LEVEL - 4)
#define LEVEL_AVATAR			(MAX_LEVEL - 5)

#define LEVEL_LOG				LEVEL_LESSER
#define LEVEL_HIGOD				LEVEL_GOD
#define LEVEL_QUESTSEE			LEVEL_GOD

#define PULSE_PER_SECOND		4
#if defined (ARMAGEDDON)
	#define PULSE_VIOLENCE	(	3	* PULSE_PER_SECOND	) /*3*/
#else
	#define PULSE_VIOLENCE	(	2	* PULSE_PER_SECOND	)
#endif
#define PULSE_MOBILE		(	4	* PULSE_PER_SECOND	)
#define PULSE_SPACE			(	5	* PULSE_PER_SECOND	)
#define PULSE_BACTA			(	15	* PULSE_PER_SECOND	)
#define PULSE_BUS			(	25	* PULSE_PER_SECOND	)

#define PULSE_MINUTE		(	60	* PULSE_PER_SECOND	)
#define PULSE_TICK			(	70	* PULSE_PER_SECOND	)
#define PULSE_AREA			(	120	* PULSE_PER_SECOND	) /*60*/
#define PULSE_TAXES			(	60	* PULSE_MINUTE		)
#define PULSE_CLAN			(	60	* PULSE_MINUTE		)
//added by Thanos (potrzebne do for i rat, mo�na b�dzie to potem zwolni�)
#define PULSE_FOR			(	PULSE_PER_SECOND / 2	)
#define PULSE_AUCTION		(	30	* PULSE_PER_SECOND	)

/* Trog: forcetypes */
#define FORCE_NONE		0
#define FORCE_NEUTRAL	1
#define FORCE_JEDI		2
#define FORCE_SITH		3
#define FORCE_MAX		4


extern	const	struct	flag_stat_type	flag_stat_table [ ];

extern	const	struct	flag_type	sex_types_list		[ ];
extern	const	struct	flag_type	position_types_list	[ ];
extern	const	struct	flag_type	gender_types_list	[ ];
extern	const	struct	flag_type	weapon_types_list	[ ];
extern	const	struct	flag_type	spice_types_list	[ ];
extern	const	struct	flag_type	crystal_types_list	[ ];
extern	const	struct	flag_type	obj_types_list		[ ];
extern	const	struct	flag_type	apply_types_list	[ ];
extern	const	struct	flag_type	trap_types_list		[ ];
extern	const	struct	flag_type	wear_types_list		[ ];
extern	const	struct	flag_type	fevent_types_list	[ ];
//extern	const	struct	flag_type	race_types_list		[ ];
extern	const	struct	flag_type	sector_types_list	[ ];
extern	const	struct	flag_type	liquid_types_list	[ ];
extern	const	struct	flag_type	forcetype_list		[ ];
extern	const	struct	flag_type	granadetypes_list	[ ];
extern	const	struct	flag_type	mont_types_list		[ ];
extern	const	struct	flag_type	constate_types_list	[ ];
extern	const	struct	flag_type	help_types_list		[ ];
extern	const	struct	flag_type	req_types_list		[ ];
extern	const	struct	flag_type	class_types_list	[ ];
extern	const	struct	flag_type	ship_types_list		[ ];
extern	const	struct	flag_type	ship_classes_list	[ ];
extern	const	struct	flag_type	ship_classes_name_list	[ ];
extern	const	struct	flag_type	log_types_list		[ ];
extern	const	struct	flag_type	ship_flags_list		[ ];
extern	const	struct	flag_type	ssystem_flags_list	[ ];

extern	const	struct	flag_type	area_flags_list		[ ];
extern	const	struct	flag_type	exit_flags_list		[ ];
extern	const	struct	flag_type	wear_flags_list		[ ];
extern	const	struct	flag_type	pl_wear_flags_list	[ ];
extern	const	struct	flag_type	room_flags_list		[ ];
extern	const	struct	flag_type	obj_flags_list		[ ];
extern	const	struct	flag_type	force_flags_list	[ ];
extern	const	struct	flag_type	aff_flags_list		[ ];
extern	const	struct	flag_type	act_flags_list		[ ];
extern	const	struct	flag_type	pc_flags_list		[ ];
extern	const	struct	flag_type	plr_flags_list		[ ];
extern	const	struct	flag_type	trap_flags_list		[ ];
extern	const	struct	flag_type	ris_flags_list		[ ];
extern	const	struct	flag_type	trig_flags_list		[ ];
extern	const	struct	flag_type	part_flags_list		[ ];
extern	const	struct	flag_type	attack_flags_list	[ ];
extern	const	struct	flag_type	def_flags_list		[ ];
extern	const	struct	flag_type	mprog_flags_list	[ ];
extern	const	struct	flag_type	save_flags_list		[ ];
extern	const	struct	flag_type	quest_flags_list	[ ]; //by Trog
extern	const	struct	flag_type	quest_init_types	[ ]; /*Thanos*/
extern	const	struct	flag_type	quest_event_types	[ ]; /*Thanos*/
extern	const	struct	flag_type	quest_action_types	[ ]; /*Thanos*/
extern	const	struct	flag_type	quest_chapter_types	[ ]; /*Thanos*/
extern	const	struct	flag_type	clan_flags_list		[ ];
extern	const	struct	flag_type	crime_flags_list	[ ];
extern	const	struct	flag_type	command_flags_list	[ ];
extern	const	struct	flag_type	container_flags_list	[ ];
extern	const	struct	flag_type	dial_flags_list		[ ];
extern	const	struct	flag_type	relation_types_list	[ ];
extern	const	struct	flag_type	shop_flags_list		[ ];
extern	const	struct	flag_type	race_flags_list		[ ]; /* Trog */
extern	const	struct	flag_type	material_form_list	[ ]; //Tanglor
extern	const	struct	flag_type	material_rarity_list	[ ]; //Tanglor
extern	const	struct	flag_type	planet_buildings_list	[ ]; //Tanglor

extern	const	struct	command_type	init_command_table	[ ];
extern	const	struct	command_type	event_command_table	[ ];
extern	const	struct	command_type	action_command_table	[ ];

/*
* Site ban structure.
*/
/* Thanos */

/*
* Structure used to build wizlist
*/
/* Added by Thanos
ulepszy�em dzia�anie f-cji 'for' i 'rat' tak, by dzia�a�y
jak skrypt, czyli wykonywa�y pojedyncz� czynno�� na jeden puls muda
stare 'for' i 'rat' robi�y wszystko w jednej ramce, co op�nia�o a nawet
wywala�o muda
*/
/* Te boole to dlatego, �e nie chce mi si� definiowa� kolejnych sta�ych, a
tak, �atwo jest doda� nast�pne fukcje dla for */
/*
* Descriptor (channel) structure.
*/

/*
* Attribute bonus structures.
*/
/* By Ratm. Makro do sprawdzania czy ch jest rasy rozumnej.
Innymi s�owy czy nie jest przedmiotem, ro�lin�, owadem itp. */
#define IS_INTELLIGENT( ch )	(!(IS_RACE( ch, "Plant" ) || IS_RACE( ch, "Object" )	\
								|| IS_RACE( ch, "Insect" ) || IS_RACE( ch, "Animal" )))

#define GREET_QUESTION			"Podaj swoje imie: "

// domyslny (niezautoryzowany jeszcze) user, za jakiego sie laczy gracz
#if !defined( WIN32 )
	#define UNKNOWN_USER		"__unknown__"
#else
	#define UNKNOWN_USER		"noident4win"
#endif

#define DEF_SPACE_RESETFREQ	120
#define DEF_RESETFREQ		15

#if defined( ARMAGEDDON )
	#define NUM_PRACT_FOR_SKILL		7
	#define NUM_PRACT_FOR_ATTR		3
	#define NUM_PRACT_FOR_HP_MV		1
	#define DEF_HP					50
	#define DEF_MV					500
	#define DEF_MANA				1000
#else
	#define DEF_HP					500
	#define DEF_MV					150
	#define DEF_MANA				1000
#endif
#define INIT_WEAPON_CONDITION		12
#define MAX_ITEM_IMPACT				30

/* Trog */
#define DEFAULT_HELP_FILE			"misc.hlp"

/* Trog */

/* Mob program structures */
/* Thanos	--> wziete od Ratma i 'lekko' zmodyfikowane */

extern bool	MOBtrigger;

/* Trog */
/*
* Added by Thanos but written by Altrag (Smaug)
* Tongues / Languages structures
*/
/* Trog & Ratm */
/*
Jesli mamy jezyk A i rasa B ma ten jezyk przypisany jako macierzysty, to
gosc rasy B zna ten jezyk od razu na 100%.
Maksymalna wartosc dla max_learn: 99 (%). Wyjatek: zdanie wyzej.
U mobow robimy tak, ze zna (100%) albo nie zna (nie ma na liscie first,last_lang).
*/

/* Trog & Ratm */
/* to bedzie w liscie ch: first,last_klang, czyli lista jezykow, ktore zna gracz */

/* race dedicated stuff */
/* Aldegard */
/* Trog */
#define		CLAN_MINLOAN	10000
#define		CLAN_MAXLOAN( clan ) ( (((clan)->members+10) / 2 + 10) * 10000 )
#define		CLAN_LOANTIME( clan ) ( 8 )

#define		COST_ORGANIZATION	10000000
#define		COST_GUILD			1000

/* Thanos & Trog */
/* Trog */
/** for clan specific HQ rooms descriptions
 * each clan CAN create their own room
 */

/* Aldegard */
#define DEFAULT_ESCAPE_POD	1//index defaultowej kapsuly

//added by Aldegard ( cargo ze snipeta jest dretwe wiec przerabiam na Listy)
//szczego�y danego materia�u - nazwa, opis, wyst�powalno��, cena,
//wyamagania przewozowa, posta�
//mozna dorzucic specjalne wla�ciwo�ci materia�u

//added by Aldegard (objecty w przestrzeni np wraki i asteroidy)
/*
* Data structure for notes.
*/

/*
* An affect.
*/

/*
* A SMAUG spell
*/

/*
Thanos:	 wymagania przedmiotu. Np. nie za�o�ysz tarczy co wymaga 40 si�y
Kod niemal�e zer�ni�ty z AFFECT_DATA, a pomys� na wymagania ca�kowicie
zer�ni�ty z Diablo :P
*/
/*! Trog: do list XML'owych */
/***************************************************************************
*									  *
*		    VALUES OF INTEREST TO AREA BUILDERS			  *
*		    (Start of section ... start here)			  *
*									  *
***************************************************************************/

/*
* Well known mob virtual numbers.
* Defined in #MOBILES.
*/
#define MOB_VNUM_QUEST_MASTER		4	/* Thanos */
#define MOB_VNUM_ANIMATED_CORPSE	5

#define MOB_VNUM_STORMTROOPER		20
#define MOB_VNUM_IMP_GUARD			21
#define MOB_VNUM_NR_GUARD			22
#define MOB_VNUM_NR_TROOPER			23
#define MOB_VNUM_MERCINARY			24
#define MOB_VNUM_BOUNCER			25
//#define SHIP_HIDDEN		  BV00 //flaga\\\Dremor

/*
* Well known object virtual numbers.
* Defined in #OBJECTS.
*/
#define OBJ_VNUM_MONEY_ONE			2
#define OBJ_VNUM_MONEY_SOME			3

#define OBJ_VNUM_DROID_CORPSE		9
#define OBJ_VNUM_CORPSE_NPC			10
#define OBJ_VNUM_CORPSE_PC			11
#define OBJ_VNUM_SCRAPS				19

#define OBJ_VNUM_LIGHT_BALL			21

#define OBJ_VNUM_SHIPMODULE			26 /*Aldegard*/

#define OBJ_VNUM_TRAP				31

#define OBJ_VNUM_BLACK_POWDER		33
#define OBJ_VNUM_SCROLL_SCRIBING	34
#define OBJ_VNUM_FLASK_BREWING		35
#define OBJ_VNUM_NOTE				36

/* Academy eq */
#define OBJ_VNUM_SCHOOL_MACE		10315
#define OBJ_VNUM_SCHOOL_DAGGER		10312
#define OBJ_VNUM_SCHOOL_SWORD		10313
#define OBJ_VNUM_SCHOOL_VEST		10308
#define OBJ_VNUM_SCHOOL_SHIELD		10310
#define OBJ_VNUM_SCHOOL_BANNER		10311
#define OBJ_VNUM_SCHOOL_DIPLOMA		10321

#define OBJ_VNUM_BLASTECH_E11		50

#define OBJ_VNUM_UNFINISHED_PART	65  /* Thanos&Onyx */

#define OBJ_VNUM_JAIL_CLOSET		33000
#define OBJ_VNUM_DEPOSIT			33001
#define OBJ_VNUM_PAZAAK_CARD		86 /* Trog */

/*
* Well known room virtual numbers.
* Defined in #ROOMS.
*/
#define ROOM_VNUM_LIMBO				2
#define ROOM_VNUM_TEMPLE			11001
#define ROOM_VNUM_SCHOOL			12350
#define ROOM_START_HUMAN			211
#define ROOM_START_WOOKIEE			28600
#define ROOM_START_TWILEK			32148
#define ROOM_START_RODIAN			32148
#define ROOM_START_HUTT				32148
#define ROOM_START_MON_CALAMARIAN	21069
#define ROOM_START_NOGHRI			1015
#define ROOM_START_GAMORREAN		28100
#define ROOM_START_JAWA				31819
#define ROOM_START_ADARIAN			29000
#define ROOM_START_EWOK				32148
#define ROOM_START_VERPINE			32148
#define ROOM_START_DEFEL			32148
#define ROOM_START_TRANDOSHAN		32148
#define ROOM_START_CHADRA_FAN		32148
#define ROOM_START_DUINUOGWUIN		32148
#define ROOM_START_QUARREN			21069
#define ROOM_START_IMMORTAL			100
#define ROOM_LIMBO_SHIPYARD			45
#define ROOM_DEFAULT_CRASH			28025

#define ROOM_PLUOGUS_QUIT			32148

#define ROOM_CORUSCANT_SHUTTLE		199
#define ROOM_SENATE_SHUTTLE			10197

/* vnumy domy�lnego wi�zienia */
#define ROOM_VNUM_FIRST_JAIL		33000
#define ROOM_VNUM_LAST_JAIL			33004
#define ROOM_VNUM_DEFAULT_OFFICE	33000
#define ROOM_VNUM_DEFAULT_CELL		33002
#define ROOM_VNUM_DEFAULT_ENTRANCE	33004

/***************************************************************************
*									  *
*		    VALUES OF INTEREST TO AREA BUILDERS			  *
*		    (End of this section ... stop here)			  *
*									  *
***************************************************************************/


/* Thanos   -- pojemno�� �o��dka gracza */
#define COND_MAX					80




/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* Added by Thanos
* Questy
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Prototyp questa */
/* Trog */
/* Dwie poni�sze listy pomagaj� w oczyszczaniu questu, przydaj�
si� te� przy zapisie do pliq, gdy gracz wychodzi z gry */




/* Thanos&Onyx -- projekty Technik�w */
/*
* Prototype for a mob.
* This is the in-memory version of #MOBILES.
*/
/* Klonownie: Thanos */
/* Trog: przed klonowaniem scinamy gracza (wady genetyczne :>)
 * a potem go przywracamy i ponizsza strukturka sluzy do tego,
 * zeby tam zapisac (tymczasowo przed save_clone) dane oryginalne
 * gracza */
/***** *** * * * * * * *  *  *	 *   *	   *	  *	  *	 *
*  SHIP DB	    Thanos 4 Aldegard
*/
//pojedynczy wpis w bazie danych statku
// praktycznie to co room_index_data (im bardziej podobne tym lepiej)
// z tych prototypow lokacji powstana lokacje prawdziwe dla pomieszczen
// dla generowanych statkow
/*
* End of SHIP DB
***** *** * * * * * * *	 *  *	*   *	  *	 *	 *	*/



/* Added by Thanos --> przest�pstwa gracza */
/* Thanos */

// Ratm - struktura zawiera tablice zmiennych dla prog�w

/*! Trog: Force Events (pomysl Thanos'a) */

// Tanglor - struktur zapamietujaca poznane przez gracza postacie
#define IS_DESCRIBED( ch ) ( (ch->attribute1 >=0) || (ch->attribute2 >=0))

//ko�c�wki fleksyjne przy odmianie przymiotnik�w m�skich i �e�skich
//odmiany sa uzale�nione od ostatniego znaku podstawy
extern const char				* male_desc_y[6];
extern const char				* male_desc_i[6];
extern const char				* female_desc_y[6];
extern const char				* female_desc_i[6];
extern const char				* category_desc[10];

//caly dialog
/*! jeden ekran dialogu zawieraj�cy wybrane teksty jakimi moze posluzyc
 * sie gracz
 */
/*
* One character (PC or NPC).
*/

/*
* Data which only PC's have.
*/


/*
* Liquids.
*/



/*
* Extra description data for a room or object.
*/
/*
* Prototype for an object.
*/
/*
* One object.
*/

/*
* Exit data.
*/
/*
* Reset commands:
*   '*': comment
*   'M': read a mobile
*   'O': read an object
*   'P': put object in object
*   'G': give object to mobile
*   'E': equip object to mobile
*   'H': hide an object
*   'B': set a bitvector
*   'T': trap an object
*   'D': set state of door
*   'R': randomize room exits
*   'C': construct ship in room	  -- Thanos
*   'S': stop (end of list)
*/

/*
* Area-reset definition.
*/
/*
* Area definition.
*/
/*!
 * Potrzebne przy wczytywaniu krainy ale przed jej
 * instalacja, po to aby nie zamazywac obj_index_hash,
 * etc... przy samym ladowaniu krainki z pliq.
 */

/*
* Load in the gods building data. -- Altrag
*/
/*
* Used to keep track of system settings and statistics		-Thoric
*/

/*
* Room type.
*/
/*
* Delayed teleport type.
*/

/*
* Skills include spells as a particular case.
*/

/*! \struct bets_progres
 *  \brief
 *
 *
 *
 * */

/**
* @brief dane dotyczace gieldy na danej planecie
*/

/*
* These are skill_lookup return values for common skills and spells.
*/
extern	int		gsn_advancedsystems;	/*added by Aldegard*/
extern	int		gsn_advancenavigation;	/*added by Aldegard*/
extern	int		gsn_capitalships;
extern	int		gsn_freighters;			/*added by Aldegard*/
extern	int		gsn_hugeships;			/*added by Aldegard*/
extern	int		gsn_midships;
extern	int		gsn_navigation;
extern	int		gsn_platforms;			/*Aldegard*/
extern	int		gsn_scoutships;			/*added by Aldegard*/
extern	int		gsn_shipmaintenance;
extern	int		gsn_shipsystems;
extern	int		gsn_shipturrets;		/*added by Aldegard*/
extern	int		gsn_smugglersworld;		/*Aldegard*/
extern	int		gsn_spacecombat;
extern	int		gsn_spacecombat2;
extern	int		gsn_spacecombat3;
extern	int		gsn_spacetactics;		/*added by Aldegard*/
extern	int		gsn_starfighters;
extern	int		gsn_tractorbeams;
extern	int		gsn_weaponsystems;
extern	int		gsn_yuuzhan_biotech;	/*added by Aldegard*/

extern	int		gsn_addpatrol;
extern	int		gsn_bribe;
extern	int		gsn_eliteguard;
extern	int		gsn_first_aid;
extern	int		gsn_gather_intelligence;
extern	int		gsn_jail;
extern	int		gsn_masspropeganda;
extern	int		gsn_postguard;
extern	int		gsn_propeganda;
extern	int		gsn_reinforcements;
extern	int		gsn_seduce;
extern	int		gsn_smalltalk;
extern	int		gsn_specialforces;
extern	int		gsn_torture;

extern	int		gsn_disguise;
extern	int		gsn_grenades;
extern	int		gsn_mine;
extern	int		gsn_snipe;
extern	int		gsn_throw;

extern	int		gsn_beg;
extern	int		gsn_gemcutting;
extern	int		gsn_makearmor;
extern	int		gsn_makeblade;
extern	int		gsn_makeblaster;
extern	int		gsn_makecomlink;
extern	int		gsn_makecontainer;
extern	int		gsn_makegrenade;
extern	int		gsn_makejewelry;
extern	int		gsn_makelandmine;
extern	int		gsn_makelight;
extern	int		gsn_makeshield;
extern	int		gsn_makehackdev;/*Trog*/
extern	int		gsn_hackshiplock;/*Trog*/
extern	int		gsn_lightsaber_crafting;
extern	int		gsn_spice_refining;
extern	int		gsn_detonate;			//added by Thanos
extern	int		gsn_hacking;			//added by Thanos
extern	int		gsn_holonet;			//added by Thanos
extern	int		gsn_identify;			//added by Thanos

extern	int		gsn_backstab;
extern	int		gsn_bashdoor;
extern	int		gsn_berserk;
extern	int		gsn_circle;
extern	int		gsn_detrap;
extern	int		gsn_dig;
extern	int		gsn_dodge;
extern	int		gsn_dominate;
extern	int		gsn_garota;			//added by Thanos
extern	int		gsn_gouge;
extern	int		gsn_hide;
extern	int		gsn_hijack;
extern	int		gsn_hitall;
extern	int		gsn_mount;
extern	int		gsn_peek;
extern	int		gsn_pick_lock;
extern	int		gsn_pickshiplock;
extern	int		gsn_scan;
extern	int		gsn_search;
extern	int		gsn_sneak;
extern	int		gsn_steal;
extern	int		gsn_track;

extern	int		gsn_disarm;
extern	int		gsn_dual_wield;
extern	int		gsn_enhanced_damage;
extern	int		gsn_kick;
extern	int		gsn_parry;
extern	int		gsn_rescue;
extern	int		gsn_second_attack;
extern	int		gsn_third_attack;
extern	int		gsn_twohanded_wield;

extern	int		gsn_aid;

/* used to do specific lookups */
extern	int		gsn_first_skill;
extern	int		gsn_first_spell;
extern	int		gsn_first_tongue;
extern	int		gsn_first_weapon;
extern	int		gsn_top_sn;

/* spells */
extern	int		gsn_aqua_breath;
extern	int		gsn_blindness;
extern	int		gsn_charm_person;
extern	int		gsn_forceinvis;
extern	int		gsn_invis;
extern	int		gsn_lightning_bolt;	/* for shockshield */
extern	int		gsn_poison;
extern	int		gsn_possess;
extern	int		gsn_sleep;

/* newer attack skills */
extern	int		gsn_punch;
extern	int		gsn_bash;
extern	int		gsn_stun;

extern	int		gsn_poison_weapon;
extern	int		gsn_climb;

extern	int		gsn_blasters;
extern	int		gsn_force_pikes;
extern	int		gsn_bowcasters;
extern	int		gsn_lightsabers;
extern	int		gsn_vibro_blades;
extern	int		gsn_flexible_arms;
extern	int		gsn_talonous_arms;
extern	int		gsn_bludgeons;

extern	int		gsn_grip;




/* Added by Thanos ( Ratm: zmieni�em 2 nastepne makra tak aby dzialaly z
polskimi literami (patrz funkcja shiftpl() w ciapek.c) */
#define LOWER( c )		( ( c ) >= 'A' && ( c ) <= 'Z'		      \
						? ( c ) + 'a' - 'A' : ( shiftpl(0,c) ) )
#define UPPER( c )		( ( c ) >= 'a' && ( c ) <= 'z'		      \
						? ( c ) + 'A' - 'a' : ( shiftpl(1,c) ) )

#define IS_SET(flag, bit)	((flag) & (bit))
#define SET_BIT(var, bit)	((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) ^= (bit))

#define IF_BUG(EXPR,__format,...)				\
if( EXPR )							\
	bug( #EXPR" " __format, ##__VA_ARGS__);			\
if( EXPR )							\


#define SKEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
					if( field )				\
					STRFREE( field );		\
					field  = value;			\
					fMatch = TRUE;			\
					break;				\
				}

/*
* Memory allocation macros.
*/
#define CREATE(result, type, number)				\
do								\
{								\
if (!((result) = (type *) calloc ((number), sizeof(type))))	\
	{ perror("calloc failure"); abort(); }			\
} while(0)


#define RECREATE(result,type,number)				\
do								\
{								\
if (!((result) = (type *) realloc ((result), sizeof(type) * (number))))\
	{ perror("realloc failure"); abort(); }			\
} while(0)

#define DISPOSE(point)						\
do								\
{								\
if (!(point))							\
{								\
	bug( "Freeing null pointer" ); \
	fprintf( stderr, "DISPOSEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
}								\
else								\
	free(point);						\
point = NULL;							\
} while(0)

#define QDISPOSE(point)		((point) && (free((point)), point = NULL))

#ifdef HASHSTR
#define STRALLOC(point)		str_alloc((point))

/* Teraz STRDUP obsluguje formatowanie stringow. -- Thanos */
//void STRDUP( char *dest, char *fmt, ... ); <-- NIE DZIALA JESZCZE
#define STRDUP( str, point )					\
do								\
{								\
	if(( str ))							\
	STRFREE(( str ));					\
	str = str_alloc(( point ));				\
} while(0)							\

#define SQFREE(point)						\
do											\
{											\
	if( point )							\
		str_free( point );		\
} while(0)

#define STRFREE(point)						\
do											\
{											\
	if (!(point))							\
	{										\
		bug( "Freeing null pointer" );				\
		fprintf( stderr, "STRFREEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
	}										\
	else if (str_free((point))==-1)			\
		fprintf( stderr, "STRFREEing bad pointer in %s, line %d\n", __FILE__, __LINE__ ); \
} while(0)
#else
#define STRALLOC(point)		str_dup((point))
#define QUICKLINK(point)	str_dup((point))
#define QUICKMATCH(p1, p2)	strcmp((p1), (p2)) == 0
#define STRFREE(point)						\
do								\
{								\
if (!(point))							\
{								\
	bug( "Freeing null pointer" );				\
	fprintf( stderr, "STRFREEing NULL in %s, line %d\n", __FILE__, __LINE__ ); \
}								\
else free((point));						\
} while(0)
#endif

#define __strdup( pnt ) strdup( pnt )


#define ASSIGN_GSN(gsn, skill)					\
do								\
{								\
	if ( ((gsn) = skill_lookup((skill))) == -1 )		\
	bug( "ASSIGN_GSN: Skill %s not found.",			\
		(skill) );					\
} while(0)

#define CHECK_SUBRESTRICTED(ch)					\
do								\
{								\
	if ( (ch)->substate == SUB_RESTRICTED )			\
	{								\
	send_to_char( "You cannot use this command from within another command.\n\r", ch );	\
	return;							\
	}								\
} while(0)


/*
* Character macros.
*/
/* Trog */
#define BESTOW( ch, cmd ) ( (ch)->pcdata && (ch)->pcdata->bestowments \
				&& is_name( cmd, (ch)->pcdata->bestowments ) )

//added by Thanos -> admini
#define IS_ADMIN(name) ( is_name( name, sysdata.admins ) )
//done

#define IS_COUNCIL(name)(IS_ADMIN( (name) )		||	\
			is_name( name, sysdata.council ) )

//by Trog, skopiowalem od Thanosa :)

#define IS_CODER(name) ( is_name( name, sysdata.coders ) )

#define IS_OLCMAN(ch) ( ch->pcdata &&				  \
			is_name( "olc", ch->pcdata->bestowments ) )

#define IS_HELPMASTER(name) (is_name( name, sysdata.helpmasters ) )

#define GET_PLANET(ch)	(ch->in_room && ch->in_room->area	\
			&& ch->in_room->area->planet	)	\
			?  ch->in_room->area->planet	:	\
				planet_list.front()

/*
Thanos -> fajne makro zalatwilem :P
:
* This is the handy CH() macro.
I think that it was Tom Adriansen (sp?)
*/
#define CH(descriptor)		( (descriptor)->original ? \
				(descriptor)->original : \
				(descriptor)->character	 )

/*
* A te sam wymy�li�em --> przydaj� si�		Thanos
*/
#define	 FEMALE( ch )		( ch )->sex == SEX_FEMALE
#define	   MALE( ch )		( ch )->sex == SEX_MALE
#define NEUTRAL( ch )		( ch )->sex == SEX_NEUTRAL
#define SEX_SUFFIX_IAE( ch )	( MALE( ch )   ? "i" : FEMALE( ch ) ? "a" : "ie" )
#define SEX_SUFFIX_YAE( ch )	( MALE( ch )   ? "y" : FEMALE( ch ) ? "a" : "e" )
#define SEX_SUFFIX_EAO( ch )	( MALE( ch )   ? "e" : FEMALE( ch ) ? "a" : "o" )
#define SEX_SUFFIX__AO( ch )	( MALE( ch )   ? ""  : FEMALE( ch ) ? "a" : "o" )
#define SEX_SUFFIX_EAE( ch )	( FEMALE( ch ) ? "a" : "e" )
#define SEX_STR( ch, formale, forfemale, forneutral )	\
				( MALE( ch ) ? formale : FEMALE( ch ) ? forfemale : forneutral )
/*
 * Do uzywania przy itemach
 * Ganis
 */
#define OFEMALE( obj )				( obj )->gender == GENDER_FEMALE
#define OMALE( obj )				( obj )->gender == GENDER_MALE
#define ONEUTRAL( obj )				( obj )->gender == GENDER_NEUTRAL
#define OPLURAL( obj )				( obj )->gender == GENDER_PLURAL
#define OSEX_SUFFIX_YAEE( obj )		( OMALE( obj ) ? "y" : OFEMALE( obj ) ? "a" : "e" )

/* number_suffix  by m�dry Thanos
* np.
*   ch_printf( ch, "%d liter%s", nr, NUMBER_SUFF( nr, "ka", "ki", "ek" ) );
* zwr�ci:
* '1 literka', '2 literki', lub '135 literek' w zale�no�ci od watro�ci.
*/
#define NUMBER_SUFF( nr, for1, for2, for3 )				\
				( ::abs(nr)==1 ? for1 :			\
					(::abs(nr)>1 && ::abs(nr)<5)	? for2: \
				::abs(nr) < 22 ? for3 :			\
					(::abs(nr)%10>1 && ::abs(nr)%10<5)?for2	\
				: for3 )

#define STRING_AFFECT( loc, mod )					\
			(						\
				loc == APPLY_AFFECT			\
				? flag_string( aff_flags_list, mod ) :	\
				(loc == APPLY_RESISTANT ||		\
				loc == APPLY_IMMUNE	||		\
				loc == APPLY_SUSCEPTIBLE)			\
				? flag_string( ris_flags_list, mod ) :	\
				itoa(paf->modifier)			\
			)

#define GET_AFF_MOD( loc, arg )						\
			(						\
				loc == APPLY_AFFECT			\
				? flag_value( aff_flags_list, arg) :	\
			(loc == APPLY_RESISTANT ||		\
				loc == APPLY_IMMUNE		||		\
				loc == APPLY_SUSCEPTIBLE)			\
				? flag_value( ris_flags_list, arg) :	\
				::atoi( arg )				\
			)

#define STRING_REQUIREMENT( loc, mod, type )				\
			(						\
				loc == REQ_AFFECT				\
				? flag_string( aff_flags_list, mod ) :	\
				loc == REQ_SKILL ?				\
				(						\
				IS_VALID_SN( type )			\
					? skill_table[type]->name	     :	\
				"none"					\
				) :						\
				loc == REQ_CLASS				\
				? bit_name( class_types_list, mod )  :	\
				loc == REQ_SEX				\
				? bit_name( sex_types_list, mod )  :	\
				itoa(mod)				\
			)

#define IS_NEWBIE(ch)		(!IS_NPC( ch ) && ch->top_level <= 10 && ch->skill_level[COMBAT_ABILITY] <= 10)
#define IS_NPC(ch)			(IS_SET((ch)->act, ACT_IS_NPC))
#define IS_CREW(ch)			(!IS_NPC(ch) && check_crew(ship_from_room(ch->in_room),ch,"all"))
#define IS_IMMORTAL(ch)		(get_trust((ch)) >= LEVEL_IMMORTAL)
#define IS_HERO(ch)			(get_trust((ch)) >= LEVEL_HERO)
#define IS_NH(ch)			(ch->pcdata && IS_SET( ch->pcdata->flags, PCFLAG_NH ) )
#define IS_AFFECTED(ch, sn)	(IS_SET((ch)->affected_by, (sn)))
#define HAS_BODYPART(ch, part)						\
			(						\
						(ch)->xflags == 0		\
				|| IS_SET( (ch)->xflags, (part) )		\
			)
/* Trog */
#define NEXT_LEVEL(ch,ability)			\
				( IS_NPC((ch)) ? -1 :	\
				(exp_level((ch)->skill_level[(ability)]+1)-exp_level((ch)->skill_level[(ability)])) )


/* Thanos */
#define IS_VIP1( ch )		( IS_NPC( ch )				\
				&& ch->s_vip_flags			\
				&& ch->s_vip_flags[0] != '\0' )

#if !defined( WIN32 ) && !defined( __CYGWIN32__ )
#define IS_VIP2( ch, planet )	( IS_NPC( ch )				\
				&& ch->s_vip_flags			\
				&& ch->s_vip_flags[0] != '\0'		\
				&& strcasestr( ch->s_vip_flags, planet->name ) )
#else
#define IS_VIP2( ch, planet )	( IS_NPC( ch )				\
				&& ch->s_vip_flags			\
				&& ch->s_vip_flags[0] != '\0'		\
				&& is_name( ch->s_vip_flags, planet->name ) )
#endif

/* zwraca ilo�� ugranych sekund na mudzie			-- Thanos */
#define SEC_PLAYED( ch )	( IS_NPC( ch ) ? 0			\
				: ( (int) ch->played +			\
					(int) ( current_time - ch->logon ) ) )

/* i ilo�� minut */
#define MIN_PLAYED( ch )	( IS_NPC( ch ) ? 0			\
				: ( (int) ch->played +			\
					(int) ( current_time - ch->logon ) ) / 60 )

#define IS_GOOD(ch)		((ch)->alignment >= 350)
#define IS_EVIL(ch)		((ch)->alignment <= -350)
#define IS_NEUTRAL(ch)		(!IS_GOOD(ch) && !IS_EVIL(ch))

#define IS_AWAKE(ch)		((ch)->position > POS_SLEEPING)
#define GET_AC(ch)		( (ch)->armor + ( IS_AWAKE(ch) ? dex_app[get_curr_dex(ch)].defensive : 0 ) \
				- ( IS_RACE( (ch), "Defel" ) ? (ch)->skill_level[COMBAT_ABILITY]*2+5 :	\
								(ch)->skill_level[COMBAT_ABILITY]/2 ) )
#define GET_HITROLL(ch)		((ch)->hitroll				    \
					+str_app[get_curr_str(ch)].tohit	    \
					+(2-(abs((ch)->mental_state)/10)))
#define GET_DAMROLL(ch)		((ch)->damroll				    \
					+str_app[get_curr_str(ch)].todam	    \
					+(((ch)->mental_state > 5		    \
					&&(ch)->mental_state < 15) ? 1 : 0) )

#define IS_OUTSIDE(ch)		(!IS_SET(				    \
					(ch)->in_room->room_flags,		    \
					ROOM_INDOORS) && !IS_SET(		\
					(ch)->in_room->room_flags,		\
					ROOM_SPACECRAFT) )

#define IS_DRUNK(ch, drunk)	(number_percent() < \
					( (ch)->pcdata->condition[COND_DRUNK] \
				* 2 / (drunk) ) )

#define IS_CLANNED(ch)		(!IS_NPC((ch))				    \
				&& (ch)->pcdata->clan			    )

#define WAIT_STATE(ch, npulse)	((ch)->wait = UMAX((ch)->wait, (npulse)))


#define EXIT(ch, door)		( get_exit( (ch)->in_room, door ) )

#define CAN_GO(ch, door)	(EXIT((ch),(door))			 \
				&& (EXIT((ch),(door))->to_room != NULL)	 \
							&& !IS_SET(EXIT((ch), (door))->flags, EX_CLOSED))

#define CAN_ENTER(ch, room)	((room) && (!IS_SET( (room)->room_flags, ROOM_PROTOTYPE )	\
							|| IS_OLCMAN( (ch) ) || get_trust( (ch) ) > 102))

#define IS_VALID_SN(sn)		( (sn) >=0 && (sn) < MAX_SKILL		     \
				&& skill_table[(sn)]			     \
				&& skill_table[(sn)]->name )

#define IS_VALID_HERB(sn)	( (sn) >=0 && (sn) < MAX_HERB		     \
				&& herb_table[(sn)]			     \
				&& herb_table[(sn)]->name )

#define IS_RACE( ch, race_name )	( !str_cmp( (ch)->race->name, race_name ) )

#define CH_RACE( ch, infNo )		((ch)->sex == SEX_MALE ? (ch)->race->przypadki[infNo] : (ch)->race->inflectsFemale[infNo])
#define SPELL_FLAG(skill, flag) ( IS_SET((skill)->flags, (flag)) )
#define SPELL_DAMAGE(skill)	( ((skill)->flags     ) & 7 )
#define SPELL_ACTION(skill)	( ((skill)->flags >> 3) & 7 )
#define SPELL_CLASS(skill)	( ((skill)->flags >> 6) & 7 )
#define SPELL_POWER(skill)	( ((skill)->flags >> 9) & 3 )
#define SET_SDAM(skill, val)	( (skill)->flags =  ((skill)->flags & SDAM_MASK) + ((val) & 7) )
#define SET_SACT(skill, val)	( (skill)->flags =  ((skill)->flags & SACT_MASK) + (((val) & 7) << 3) )
#define SET_SCLA(skill, val)	( (skill)->flags =  ((skill)->flags & SCLA_MASK) + (((val) & 7) << 6) )
#define SET_SPOW(skill, val)	( (skill)->flags =  ((skill)->flags & SPOW_MASK) + (((val) & 3) << 9) )

/* Retired and guest imms. Trog += silenced & isolated mortals */
#define IS_RETIRED(ch) (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_RETIRED) )
#define IS_GUEST(ch)   (ch->pcdata && IS_SET(ch->pcdata->flags,PCFLAG_GUEST  ) )
#define IS_SILENCED(ch) ((ch)->pcdata && (ch)->pcdata->unsilence_date != 0)
#define IS_ISOLATED(ch) ((ch)->pcdata && (ch)->pcdata->release_date != 0)

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

/** Trog: jezyki */
#define SPEAKS( ch, lang )	( (ch)->speaks == (lang) )
#define SPEAKSN( ch, lang_name )	( !str_cmp( (ch)->speaks->name, (lang_name) ) )

/*
* Object macros.
*/
#define CAN_WEAR(obj, part)	(IS_SET((obj)->wear_flags,  (part)))
#define IS_OBJ_STAT(obj, stat)	(IS_SET((obj)->extra_flags, (stat)))


//byTrog
#define SHIPNAME( ship )   ( (ship->ship_title && ship->ship_title[0] != '\0') \
				? ship->ship_title : (ship->name && ship->name[0] != '\0') \
				? ship->name : "(null)" )

#define CLAN( ch ) ( (!IS_NPC( (ch) ) && (ch)->pcdata->clan) ? (ch)->pcdata->clan : NULL )
#define CLANNAME( clan )   ( clan->name )
#define LEADER_NAME( clan )		( clan->leader ? clan->leader->name : "" )
#define DIPLOMAT_NAME( clan )	( clan->diplomat ? clan->diplomat->name : "" )
#define FIRST_NAME( clan )		( clan->first ? clan->first->name : "" )
#define SECOND_NAME( clan )		( clan->second ? clan->second->name : "" )

#define IS_LEADER( ch )	 ( ch->pcdata->clan && \
				!str_cmp( ch->name, LEADER_NAME( ch->pcdata->clan ) ) )
#define IS_DIPLOMAT( ch ) ( ch->pcdata->clan && \
				!str_cmp( ch->name, DIPLOMAT_NAME( ch->pcdata->clan ) ) )
#define IS_FIRST( ch )	  ( ch->pcdata->clan && \
				!str_cmp( ch->name, FIRST_NAME( ch->pcdata->clan ) ) )
#define IS_SECOND( ch )	  ( ch->pcdata->clan && \
				!str_cmp( ch->name, SECOND_NAME( ch->pcdata->clan ) ) )
#define IS_MEMBER( ch, clan ) ( ch->pcdata->clan && ch->pcdata->clan == clan )
#define CLANTYPE( clan, inf ) ( clantypename[clan->type][inf] )
#define CRANK( clan, crank ) ( *(clan)->rank[(crank)] ? (clan)->rank[(crank)] : defaultrank[(crank)] )
#define IS_WAITING( clan ) ( clan->type == CLAN_WAITING )
#define IS_ORG( clan ) ( clan->type == CLAN_ORGANIZATION )
#define IS_GUILD( clan ) ( clan->type == CLAN_GUILD )
#define IS_ORDER( clan ) ( clan->type == CLAN_ORDER )
#define IS_SUBORG( clan ) ( clan->type == CLAN_SUBORG )
#define CLANSUFFIX( clan, s1, s2 ) ( IS_ORDER((clan)) ? (s1) : (s2) )
#define MEMBER_BESTOW( member, cmd ) ( (member)->status == CLAN_LEADER \
									|| is_name( (cmd), (member)->bestowments ) )


/** Trog: In Vnum Range - sprawdza czy vnum jest w zakresie vnumow z krainki */
#define IVR( vnum, area )	( (vnum) >= (area)->lvnum && (vnum) <= (area)->uvnum )

/*
* Description macros.
*/

#define NAME(ch,i)							\
	(i >=0 && i <=5) ? \
		( ( IS_AFFECTED(ch, AFF_DISGUISE) && ch->pcdata ) ?	\
			ch->pcdata->fake_infl[i] : \
			( ch->przypadki[i] ) ) : "kto�"


//#if defined (FAMILIARITY)	//Tanglor makro odpowiada za info o postaci
#define PERS( ch, looker, i )	\
	( can_see( looker, ( ch ) ) ?		\
		( ( IS_AFFECTED(ch, AFF_DISGUISE) && ch->pcdata ) ?	\
			( ch->pcdata->fake_infl[i] ) : \
			( does_knows(looker, ch ) ? \
				ch->przypadki[i] : format_char_attribute_wrapper(ch,i) ) ) \
		: inv_pers_i[i] )
/*#else
#define PERS( ch, looker, i )	\
  ( can_see( looker, ( ch ) ) ?		\
			NAME(ch,i) : inv_pers_i[i] )


#endif*/


#define log_string( txt )	( log_string_plus( (txt), LOG_NORMAL, LEVEL_LOG ) )


/* Trog: makra do obslugi plikow

Makra te otwieraja pliki jesli NIE SA OTWARTE (fp==NULL)
i zamykaja jesli SA OTWARTE (fp!=NULL). Zastanow sie
zanim uzyjesz takiego makra (w szczegolnosci do otwierania)
czy na pewno o takie dzialanie Ci chodzi!!!
*/

#define FHOPEN( con, fp, path, mode, txt )	\
do											\
{											\
	if( !(con) )							\
		bug( #fp " " txt );					\
	else									\
	if(	!((fp) = fopen( (path), (mode) )) )	\
	{										\
		bug( #fp " cannot open" );			\
		perror( (path) );					\
	}										\
} while(0)

#define FHCLOSE( con, fp, txt )		\
do									\
{									\
	if( !(con) )					\
		bug( #fp " " txt );			\
	else							\
	if( !fclose( fp ) )				\
		(fp) = NULL;				\
	else							\
	{								\
		bug( #fp " cannot close" );	\
		perror( #fp );				\
	}								\
} while(0)

#define _FOPEN( fp, path, mode, txt ) FHOPEN( !fp, fp, path, mode, txt )
#define _FCLOSE( fp, txt ) FHCLOSE( fp, fp, txt )

#define FOPEN( fp, path, mode ) _FOPEN( fp, path, mode, "is not null" )
#define FCLOSE( fp ) _FCLOSE( fp, "is null" )

#define RESERVE_OPEN _FOPEN( fpReserve, NULL_FILE, "r", "already opened" )
#define RESERVE_CLOSE FCLOSE( fpReserve )

#define LOG_OPEN _FOPEN( fpLOG, NULL_FILE, "r", "already opened" )
#define LOG_CLOSE FCLOSE( fpLOG )

/* Trog: makro do sprawdzenia poprawnosci nazwy pliku */
#define NAMEREGEXP "^[a-zA-Z0-9_-]+\\."
#define VALID_FILENAME( name, ext )	( !reg_cmp( NAMEREGEXP ext "$", name ) )

/* Ratm: makro przeszukuje wszystkie progi i dodaje ich typy do listy typow */
#define BUILD_PROG_TYPES( obj ) \
{ \
	(obj)->progtypes = 0; \
	for ( auto* mprg : (obj)->mudprogs ) \
	{ \
	SET_BIT( (obj)->progtypes, mprg->type ); \
	} \
}

/* Trog: zwraca ilosc elementow tablicy zrzutowana na int */
#define ALEN( array )		((int)(sizeof(array) / sizeof(array[0])))
/* to samo tylko dla napisow */
#define SLEN( str )			((int)strlen(str))


/*
* Structure for a command in the command lookup table.
*/


/*
* Structure for a social in the socials table.
*/


/*
* Global constants.
*/
extern	time_t			last_restore_all_time;
extern	time_t			boot_time;  /* this should be moved down */
extern	HOUR_MIN_SEC	* set_boot_time;
extern	struct	tm		* new_boot_time;
extern	time_t			new_boot_time_t;

extern	const	struct	str_app_type	str_app		[26];
extern	const	struct	int_app_type	int_app		[26];
extern	const	struct	wis_app_type	wis_app		[26];
extern	const	struct	dex_app_type	dex_app		[26];
extern	const	struct	con_app_type	con_app		[26];
extern	const	struct	cha_app_type	cha_app		[26];
extern	const	struct	lck_app_type	lck_app		[26];
extern	const	struct	frc_app_type	frc_app		[26];


//extern	const	struct	char_desc	    desc_table [][MAX_DESC_TYPES]; //Tanglor
extern const	struct	char_desc	 *desc_table [MAX_DESC_TYPES];
//extern const	struct	char_desc	 desc_table [MAX_DESC_TYPES][];

//extern	const	struct	race_type	race_table			[MAX_RACE];
extern	const	struct	class_type	class_table			[MAX_ABILITY];
extern	const	struct	liq_type	liq_table			[LIQ_MAX];
extern	const char *	const				attack_table		[13];
extern	const char *	const				sith_hatred			[10];

extern	const char *	const				skill_tname			[  ];
extern	const char *	const				skill_tname_orig	[  ];

extern	int		const				movement_loss		[SECT_MAX];
extern	const char *	const				dir_name			[  ];
extern	const char *	const				dir_name_orig		[  ];
extern	const char *	const				dir_where_name		[  ];
extern	const char *	const				dir_rev_name		[  ];
extern	const char *	const				dir_type_name		[  ];

extern	const char *	const				where_name			[  ];
extern	const	int					rev_dir				[  ];
extern	const	int					trap_door			[  ];

extern	int		const				lang_array			[  ];
extern	const char *	const				lang_names			[  ];
extern	const char *	const				lang_names_pl		[  ];

extern	const	char *				inv_pers_i			[ 6];

extern	const	char *				clantypename		[CLAN_MAXTYPE][6];
extern	const	char *				defaultrank			[CLAN_LEADER+1];

extern const char *const cargo_names[CARGO_MAX];
extern const char *const module_type_name[MODULE_MAX];
extern const char *const astro_names[ASTRO_MAX];
extern const char *const ship_dock_type[DOCK_MAX];

extern char const iso_table [];

/*
* Global variables.
*/
extern std::list<LAST_DATA*>		last_wiztalk_list;
extern std::list<LAST_DATA*>		last_chat_list;
extern std::list<LAST_DATA*>		last_admin_list;
extern std::list<LAST_DATA*>		last_olctalk_list;
extern std::list<LAST_DATA*>		last_codertalk_list;
extern std::list<SCRIPT_DATA*>		script_prog_list;


extern	bool			fCopyOver;
#if defined( WIN32 )
	extern	unsigned short	port; /* potrzebne do copyover */
#else
	extern	u_short			port; /* potrzebne do copyover */
#endif
extern	int				control; /* i to te� */
extern	int				control6; /* i jeszcze to ;) */
extern	pid_t			pid;
extern	int				numobjsloaded;
extern	int				nummobsloaded;
extern	int				physicalobjects;
extern	int				num_descriptors;
extern	struct	system_data	sysdata;
extern	int				top_sn;
extern	int				top_vroom;
extern	int				top_herb;
// Ratm aktualna linia i numer wykonywanego mob proga
extern	int				current_prog_line;
extern	int				current_prog_number;

extern	CMDTYPE			* command_hash	[MAX_CMD_HASH];

extern	SKILLTYPE		* skill_table	[MAX_SKILL];
extern	SOCIALTYPE		* social_index	[27];
extern	CHAR_DATA		* cur_char;
extern	ROOM_INDEX_DATA * cur_room;
extern	bool			cur_char_died;
extern	ch_ret			global_retcode;
extern	SKILLTYPE		* herb_table	[MAX_HERB];

extern	int				cur_obj;
extern	int				cur_obj_serial;
extern	bool			cur_obj_extracted;
extern	obj_ret			global_objcode;

extern	std::list<HELPS_FILE*>		helps_file_list;
extern	std::list<SHOP_DATA*>		shop_list;
extern	std::list<REPAIR_DATA*>		repair_list;
extern	std::list<BAN_DATA*>		ban_list;
extern	std::list<CHAR_DATA*>		char_list;
extern	std::list<DESCRIPTOR_DATA*>	descriptor_list;
extern	std::list<BOARD_DATA*>		board_list;
extern	std::list<OBJ_DATA*>		object_list;
extern	std::list<CLAN_DATA*>		clan_list;
extern	std::list<HQ_ROOM_DESC*>	hq_room_desc_list;
extern	std::list<GUARD_DATA*>		guard_list;
extern	std::list<SHIP_DATA*>		ship_list;
extern	std::list<SHIP_DATA*>		free_ship_list;
extern	std::list<ASTRO_DATA*>		astro_list;
extern	std::list<MISSILE_DATA*>	missile_list;
extern	std::list<HANGAR_DATA*>		hangar_list;
extern	std::list<TURRET_DATA*>		turret_list;
extern	std::list<MODULE_DATA*>		module_list;
extern	std::list<CREW_DATA*>		cmember_list;
extern	std::list<SPACE_DATA*>		starsystem_list;
extern	std::list<PLANET_DATA*>		planet_list;
extern	std::list<BOUNTY_DATA*>		bounty_list;
extern	std::list<BOUNTY_DATA*>		disintigration_list;
extern	std::list<AREA_DATA*>		area_list;
extern	std::list<AREA_DATA*>		build_list;
extern	std::list<AREA_DATA*>		asort_list;
extern	std::list<AREA_DATA*>		bsort_list;
extern	std::list<AREA_DATA*>		uarea_list;
extern	std::list<TELEPORT_DATA*>	teleport_list;
extern	std::list<LANG_DATA*>		lang_list;
extern	std::list<INFORM_DATA*>		inform_list;
extern	std::list<QUEST_INDEX_DATA*>	quest_index_list;
extern	std::list<QUEST_DATA*>		quest_list;
extern	std::list<PROJECT_DATA*>	project_list;
extern	std::list<CLONING_DATA*>	cloning_list;
extern	std::list<SHIP_INDEX_DATA*>	ship_index_list;
extern	std::list<COMPLAIN_DATA*>	complain_list;
extern	std::list<STOCK_EXCHANGE_DATA*>	stock_exchange_list;
extern	time_t			auction_pulse;

extern	std::list<DIALOG_DATA*>		dialog_list;
extern	std::list<MATERIAL_DATA*>	material_list;
extern	std::list<RACE_DATA*>		race_list;
extern	RACE_DATA		* base_race;
extern	const	RACE_DATA	human_race;
extern	LANG_DATA		* lang_base;
extern	const	LANG_DATA	lang_common;

extern	std::list<OBJ_DATA*>		extracted_obj_queue;
extern	std::list<EXTRACT_CHAR_DATA>	extracted_char_queue;
extern	OBJ_DATA		* save_equipment[MAX_WEAR][MAX_LAYERS];
extern	CHAR_DATA		* quitting_char;
extern	CHAR_DATA		* loading_char;
extern	CHAR_DATA		* saving_char;
extern	OBJ_DATA		* all_obj;

extern	char			bug_buf [ ];
extern	time_t			current_time;
extern	bool			fLogAll;
extern	FILE			* fpReserve;
extern	FILE			* fpLOG;
extern	char			log_buf [ ];

extern	AUCTION_DATA	* auction;
extern	struct	act_prog_data * mob_act_list;

//added by Thanos
extern	FOR_DATA		for_loop;
extern	RAT_DATA		rat_loop;
extern	CHAR_DATA		* Quest_Master;
extern	int				area_id;

extern	std::list<TURBOCAR*>		turbocar_list;

extern	std::forward_list<MOB_INDEX_DATA*>	mob_index_hash[MAX_KEY_HASH];
extern	std::forward_list<OBJ_INDEX_DATA*>	obj_index_hash[MAX_KEY_HASH];
extern	std::forward_list<ROOM_INDEX_DATA*>	room_index_hash[MAX_KEY_HASH];

extern	int			top_affect;
extern	int			top_area;
extern	int			top_ed;
extern	int			top_req;
extern	int			top_exit;
extern	int			top_helps_file;
extern	int			top_help;	/* Trog: zamknac to w strukturze helps_file (zrobie to kiedystam) */
extern	int			top_mob_index;
extern	int			top_obj_index;
extern	int			top_reset;
extern	int			top_room;
extern	int			top_sroom;
extern	int			top_shop;
extern	int			top_repair;
extern	int			top_vroom;
extern	int			top_quest_index;
extern	int			top_quest;
extern	int			top_clan;
extern	int			top_cloning;
extern	int			top_race;
extern	int			top_turbocar;

extern	bool		wizlock;


#include "fun_decls.h"

#if defined __GNUC__ && __GNUC__ >= 3
#define bug( __format,... ) _bug( "[%s,%s:%d]: " __format, __FUNCTION__, __FILE__, __LINE__, ##__VA_ARGS__ )
#else
#define bug _bug
#endif

#if defined perror
#undef perror
#endif
#define perror	_perror

#endif
