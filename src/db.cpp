/***************************************************************************
 * Star Wars Reality Code Additions and changes from the Smaug Code         *
 * copyright (c) 1997 by Sean Cooper                                        *
 * Starwars and Starwars Names copyright(c) Lucas Film Ltd.                 *
 * SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider                           *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,                    *
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops                *
 * Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
 * Chastain, Michael Quan, and Mitchell Tse.                                *
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
 * Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
 * ------------------------------------------------------------------------ *
 *                     ____________   __     ______                         *
 *   Aldegard    Jot  (   ___   ___| /  \   |  __  )   Thanos      Trog     *
 *        ______________\  \|   |  /  ^   \ |     <_______________          *
 *        \________________/|___|/___/"\___\|__|\________________/          *
 *                  \   \/ \/   //  \   |  __  )(  ___/`                    *
 *                    \       //  ^   \ |     <__)  \                       *
 *                      \_!_//___/"\___\|__|\______/TM                      *
 * (c) 2001, 2002            M       U        D                Ver 1.1      *
 * ------------------------------------------------------------------------ *
 * 			Database management module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#if defined (__CYGWIN32__) || defined(__FreeBSD__)
#include <dirent.h>
#else
#include <sys/dir.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string>
#include <list>
#include <forward_list>
#include <algorithm>
#include "classes/SWPazaak.h"
#include "mud.h"

extern int _filbuf args( (FILE *) );
int area_id;

#if defined(KEY)
#undef KEY
#endif

extern void load_stock_markets();
extern void load_dialogs_list();

void init_supermob();
void save_sysdata(SYSTEM_DATA sys);

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
					field  = value;			\
					fMatch = true;			\
					break;				\
				}

/*
 * Globals.
 */

std::list<WIZENT*> wiz_list;

time_t last_restore_all_time = 0;

std::list<HELPS_FILE*> helps_file_list;

std::list<SHOP_DATA*> shop_list;

std::list<REPAIR_DATA*> repair_list;

std::list<TELEPORT_DATA*> teleport_list;

std::list<QUEST_DATA*> quest_list;
std::list<QUEST_INDEX_DATA*> quest_index_list;

std::list<SHIP_INDEX_DATA*> ship_index_list;

std::list<CLONING_DATA*> cloning_list;

std::list<OBJ_DATA*> extracted_obj_queue;
std::list<EXTRACT_CHAR_DATA> extracted_char_queue;

std::list<CHAR_DATA*> char_list;

std::list<OBJ_DATA*> object_list;

std::list<COMPLAIN_DATA*> complain_list;

std::list<STOCK_EXCHANGE_DATA*> stock_exchange_list;
time_t auction_pulse;

std::list<MATERIAL_DATA*> material_list;

std::list<DIALOG_DATA*> dialog_list;

std::list<LAST_DATA*> last_wiztalk_list;	//added by Thanos
std::list<LAST_DATA*> last_chat_list;		//zmienne do do_last (Ratma)
std::list<LAST_DATA*> last_admin_list;		//by Trog
std::list<LAST_DATA*> last_olctalk_list;	//by Trog
std::list<LAST_DATA*> last_codertalk_list;	//by Trog
std::list<SCRIPT_DATA*> script_prog_list; /* Thanos */

void load_materials();

char bug_buf[2 * MAX_INPUT_LENGTH];
char log_buf[2 * MAX_INPUT_LENGTH];
char *help_greeting;

//added by Thanos
FOR_DATA for_loop;
RAT_DATA rat_loop;
//done

int cur_qobjs;
int cur_qchars;
int nummobsloaded;
int numobjsloaded;
int physicalobjects;

//AUCTION_DATA		* auction;	/* auctions */

FILE *fpLOG = NULL;

/* criminals */
int gsn_torture;
int gsn_disguise;
int gsn_beg;
int gsn_pickshiplock;
int gsn_hijack;
int gsn_garota;

/* soldiers and officers */
int gsn_reinforcements;
int gsn_postguard;
int gsn_mine;
int gsn_grenades;
int gsn_first_aid;
int gsn_snipe;
int gsn_throw;

int gsn_eliteguard;
int gsn_specialforces;
int gsn_jail;
int gsn_smalltalk;
int gsn_propeganda;
int gsn_bribe;
int gsn_seduce;
int gsn_masspropeganda;
int gsn_gather_intelligence;

/* pilots and smugglers */
int gsn_starfighters;
int gsn_midships;
int gsn_capitalships;
int gsn_platforms;
int gsn_weaponsystems;
int gsn_navigation;
int gsn_shipsystems;
int gsn_shipturrets;
int gsn_tractorbeams;
int gsn_shipmaintenance;
int gsn_spacecombat;
int gsn_spacecombat2;
int gsn_spacecombat3;
int gsn_advancenavigation;
int gsn_spacetactics;
int gsn_smugglersworld;
int gsn_scoutships;
int gsn_freighters;
int gsn_hugeships;

/* player building skills */
int gsn_advancedsystems; /*added by Aldegard*/
int gsn_identify; /*added by Thanos*/
int gsn_hacking; /*added by Thanos*/
int gsn_detonate; /*added by Thanos*/
int gsn_holonet; /*added by Thanos*/
int gsn_lightsaber_crafting;
int gsn_spice_refining;
int gsn_makeblade;
int gsn_makeblaster;
int gsn_makelight;
int gsn_makecomlink;
int gsn_makegrenade;
int gsn_makelandmine;
int gsn_makearmor;
int gsn_makeshield;
int gsn_makecontainer;
int gsn_makemissile;
int gsn_gemcutting;
int gsn_makejewelry;
int gsn_yuuzhan_biotech; /*Aldegard*/
int gsn_makehackdev; /*Trog*/
int gsn_hackshiplock; /*Trog*/

/* weaponry */
int gsn_blasters;
int gsn_bowcasters;
int gsn_force_pikes;
int gsn_lightsabers;
int gsn_vibro_blades;
int gsn_flexible_arms;
int gsn_talonous_arms;
int gsn_bludgeons;
int gsn_shieldwork;

/* thief */
int gsn_detrap;
int gsn_backstab;
int gsn_circle;
int gsn_dodge;
int gsn_hide;
int gsn_peek;
int gsn_pick_lock;
int gsn_sneak;
int gsn_steal;
int gsn_gouge;
int gsn_poison_weapon;

/* thief & warrior */
int gsn_disarm;
int gsn_enhanced_damage;
int gsn_kick;
int gsn_parry;
int gsn_rescue;
int gsn_second_attack;
int gsn_third_attack;
int gsn_fourth_attack;
int gsn_fifth_attack;
int gsn_dual_wield;
int gsn_twohanded_wield;
int gsn_punch;
int gsn_bash;
int gsn_stun;
int gsn_bashdoor;
int gsn_grip;
int gsn_berserk;
int gsn_hitall;

/* vampire */
int gsn_feed;

/* other   */
int gsn_aid;
int gsn_track;
int gsn_search;
int gsn_dig;
int gsn_dominate;
int gsn_mount;
int gsn_bite;
int gsn_claw;
int gsn_sting;
int gsn_tail;
int gsn_scribe;
int gsn_brew;
int gsn_climb;
int gsn_scan;
int gsn_slice;

/* spells */
int gsn_aqua_breath;
int gsn_blindness;
int gsn_charm_person;
int gsn_curse;
int gsn_invis;
int gsn_forceinvis;
int gsn_poison;
int gsn_sleep;
int gsn_possess;
int gsn_lightning_bolt;

/* for searching */
int gsn_first_spell;
int gsn_first_skill;
int gsn_first_weapon;
int gsn_first_tongue;
int gsn_top_sn;

/*
 * Locals.
 */
std::forward_list<MOB_INDEX_DATA*> mob_index_hash[MAX_KEY_HASH];
std::forward_list<OBJ_INDEX_DATA*> obj_index_hash[MAX_KEY_HASH];
std::forward_list<ROOM_INDEX_DATA*> room_index_hash[MAX_KEY_HASH];

std::list<AREA_DATA*> area_list;
std::list<AREA_DATA*> build_list;
std::list<AREA_DATA*> asort_list;
std::list<AREA_DATA*> bsort_list;

SYSTEM_DATA sysdata;

bool CopyOver; /* czy mud ma si� zrebootowa�: Thanos */

int top_affect;
int top_area;
int top_ed;
int top_req;
int top_exit;
int top_helps_file;
int top_help; /* Trog: zamknac to w strukturze helps_file (zrobie to kiedystam) */
int top_mob_index;
int top_obj_index;
int top_reset;
int top_room;
int top_sroom;
int top_shop;
int top_repair;
int top_vroom;
int top_quest_index;
int top_quest;
int top_clan;
int top_cloning;
int top_race;
int top_turbocar;

/*
 * Semi-locals.
 */
bool fBootDb;
FILE *fpArea;
char strArea[MAX_INPUT_LENGTH];
bool noEcho;	// do mpNoEcho (Thanos)

/*
 * Local booting procedures.
 */
void init_mm args( ( void ) );

void boot_log args( ( const char *str, ... ) );
void load_helps_files args( ( void ) ); /* Trog */
void load_helps_section args( ( char *filename ) ); /* Trog */
void load_helps args( ( FILE *fpHelp, char *filename ) ); /* Trog */
void load_quests args( ( void ) );	//Thanos
void load_clonings args( ( void ) );	//Thanos
void load_shipdb args( ( void ) );	//Thanos
void load_astros args( ( void ) );
bool load_systemdata args( ( SYSTEM_DATA *sys ) );
void load_banlist args( ( void ) );
void load_projects args( ( void ) );
void load_races args( () ); /* Trog */
void load_race_section args( ( char *filename ) );
void fread_race args( ( FILE *fpRace, char *filename ) );
void load_turbocars args( () ); /* Trog */
void load_turbocar_section args( ( char *filename ) );
void fread_turbocar args( ( FILE *fpTurbocar, char *filename ) );
void load_languages args( () ); /* Trog */

void set_timeweather args( ( void ) );
void reset_starsystem args(( SPACE_DATA *starsystem ) );	//Thanos

void initialize_economy args( ( void ) );

void fix_exits args( ( void ) );

/* do CopyOvera 	--Thanos */
void copyover_recover args( ( void ) );
#if !defined( WIN32 )
void start_auth args( ( struct descriptor_data *d ) );
#endif
void new_descriptor args( ( int new_desc ) );

/*
 * External booting function
 */
void load_corpses args( ( void ) );

/*
 * MUDprogram locals
 */

void mprog_file_read args ( ( char* f, MOB_INDEX_DATA *pMobIndex ) );
void oprog_file_read args ( ( char* f, OBJ_INDEX_DATA *pObjIndex ) );
void rprog_file_read args ( ( char* f, ROOM_INDEX_DATA *pRoomIndex ) );
void load_mudprogs args ( ( AREA_DATA *tarea, FILE* fp ) );
void load_objprogs args ( ( AREA_DATA *tarea, FILE* fp ) );
void load_roomprogs args ( ( AREA_DATA *tarea, FILE* fp ) );
void mprog_read_programs args ( ( FILE* fp,
				MOB_INDEX_DATA *pMobIndex) );
void oprog_read_programs args ( ( FILE* fp,
				OBJ_INDEX_DATA *pObjIndex) );
void rprog_read_programs args ( ( FILE* fp,
				ROOM_INDEX_DATA *pRoomIndex) );

/* envy.c 		-- Thanos */
void load_nvy_hdr args( ( FILE *fp ) );
void load_nvy_mobs args( ( AREA_DATA *pArea, FILE *fp ) );
void load_nvy_objs args( ( AREA_DATA *pArea, FILE *fp ) );
void load_nvy_rooms args( ( AREA_DATA *pArea, FILE *fp ) );
void load_nvy_quest args( ( AREA_DATA *pArea, FILE *fp ) );

void shutdown_mud(char *reason)
{
	FILE *fp;

	if ((fp = fopen( SHUTDOWN_FILE, "a")) != NULL)
	{
		fprintf(fp, "%s\n", reason);
		fclose(fp);
	}
}

/*olac wszystko. Questy zczytaja sie w f-cji load_quests */
void old_load_quests(AREA_DATA *tarea, FILE *fp)
{
	for (;;)
		if (!str_cmp(fread_word(fp), "#END"))
			break;
	return;
}

/*
 * Added by Thanos: dzi�ki temu nie b�dzie ju� windowsiowatych
 * krainek na mudzie. Pliki konwertowane s� w locie.
 */
char __getc(FILE *fp)
{
	char letter;

	letter = getc(fp);

	if (sysdata.to_iso)
		letter = iso_table[(unsigned char) letter];

	return letter;
}

/*
 * Read a string from file fp
 */
char* st_fread_string(FILE *fp)
{
	static char buf[MAX_STRING_LENGTH];
	char c;
	int ln;

	buf[0] = '\0';
	ln = 0;

	/*
	 * Skip blanks.
	 * Read first char.
	 */
	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read.");
			if (fBootDb)
				exit(1);
			return (char*) "";
		}
		c = __getc(fp);
	} while (isspace(c));

	if ((buf[0] = c) == '~')
		return (char*) "";

	ln++;
	for (;;)
	{
		if (ln >= (MAX_STRING_LENGTH - 1))
		{
			bug("string too long");
			return buf;
		}
		switch (buf[ln] = __getc(fp))
		{
		default:
			ln++;
			break;

		case EOF:
			bug("EOF");
			if (fBootDb)
				exit(1);
			buf[ln] = '\0';
			return buf;
			break;

		case '\n':
			ln++;
			buf[ln] = '\r';
			ln++;
			break;

		case '\r':
			break;

		case '~':
			buf[ln] = '\0';
			return buf;
		}
	}
}

void reset_world()
{
	for (auto* pArea : area_list)
		reset_area(pArea);

	for (auto* pStarsystem : starsystem_list)
		reset_starsystem(pStarsystem);

	return;
}

void clear_sysdata()
{
	/* default values */
	sysdata.to_iso = false;
	sysdata.muse_level = LEVEL_DEMI;
	sysdata.think_level = LEVEL_HIGOD;
	sysdata.build_level = LEVEL_DEMI;
	sysdata.log_level = LEVEL_LOG;
	sysdata.level_modify_proto = LEVEL_LESSER;
	sysdata.level_override_private = LEVEL_GREATER;
	sysdata.level_mset_player = LEVEL_LESSER;
	sysdata.stun_plr_vs_plr = 15;
	sysdata.stun_regular = 15;
	sysdata.dam_plr_vs_plr = 100;
	sysdata.dam_plr_vs_mob = 100;
	sysdata.dam_mob_vs_plr = 100;
	sysdata.dam_mob_vs_mob = 100;
	sysdata.level_getobjnotake = LEVEL_GREATER;
	sysdata.save_frequency = 20; /* minutes */
	sysdata.save_flags = SV_DEATH | SV_PASSCHG | SV_AUTO | SV_PUT | SV_DROP
			| SV_GIVE | SV_AUCTION | SV_ZAPDROP | SV_IDLE;
	STRDUP(sysdata.admins, "");
	STRDUP(sysdata.council, "");
	STRDUP(sysdata.coders, "");
	STRDUP(sysdata.helpmasters, "");
	STRDUP(sysdata.time_of_max, "");
	STRDUP(sysdata.denied_nicks, "");
	sysdata.max_clan_id = 0;
}

/*
 * Big mama top level function.
 */
void boot_db(bool fCopyOver)
{
	AREA_DATA *area;
	char buf[MSL];
	int wear, x;

	fBootDb = true;
	noEcho = false;
	nummobsloaded = 0;
	numobjsloaded = 0;
	physicalobjects = 0;
	sysdata.maxplayers = 0;
	cur_qobjs = 0;
	cur_qchars = 0;
	cur_char = NULL;
	cur_obj = 0;
	cur_obj_serial = 0;
	cur_char_died = false;
	cur_obj_extracted = false;
	cur_room = NULL;
	quitting_char = NULL;
	loading_char = NULL;
	saving_char = NULL;
	// Ratm
	current_prog_line = -1;
	current_prog_number = -1;

	/** Trog */
	//xmlNewCharEncodingHandler("ISO-8859-2", NULL, NULL);
	//xmlNewCharEncodingHandler("ISO-8859-2", isolat2ToUTF8, UTF8Toisolat2);
	show_hash(32);
	unlink( BOOTLOG_FILE);

	boot_log("--------------------[ Boot Log: %s ]--------------------",
			static_cast<SWString>(SWTimeStamp(boot_time)).c_str());

	/*
	 * Main system data
	 */
	clear_sysdata();
	log_string("Loading sysdata configuration");
	if (!load_systemdata(&sysdata))
	{
		log_string("Not found.  Creating new configuration.");
		sysdata.alltimemax = 0;
		sysdata.downtype = DOWN_SHUTDOWN;
	}
	sysdata.tmpdowntype = sysdata.downtype;
	sysdata.downtype = DOWN_CRASHED; // domy�lnie, �e pad�
	save_sysdata(sysdata);		// od razu zapisz na wszelki

	/*
	 * Commands, socials & skills
	 */
	log_string("Loading commands");
	load_commands();

	log_string("Loading socials");
	load_socials();

	log_string("Loading skill table");
	load_skill_table();
	sort_skill_table();

	gsn_first_spell = 0;
	gsn_first_skill = 0;
	gsn_first_weapon = 0;
	gsn_first_tongue = 0;
	gsn_top_sn = top_sn;

	for (x = 0; x < top_sn; x++)
	{
		if (!gsn_first_spell && skill_table[x]->type == SKILL_SPELL)
			gsn_first_spell = x;
		else if (!gsn_first_skill && skill_table[x]->type == SKILL_SKILL)
			gsn_first_skill = x;
		else if (!gsn_first_weapon && skill_table[x]->type == SKILL_WEAPON)
			gsn_first_weapon = x;
		else if (!gsn_first_tongue && skill_table[x]->type == SKILL_TONGUE)
			gsn_first_tongue = x;
	}

	log_string("Loading herb table");
	load_herb_table();

//	CREATE( auction, AUCTION_DATA, 1);
//	auction->item 	= NULL;
	for (wear = 0; wear < MAX_WEAR; wear++)
		for (x = 0; x < MAX_LAYERS; x++)
			save_equipment[wear][x] = NULL;

	/*
	 * Init random number generator.
	 */
	log_string("Initializing random number generator");
	init_mm();

	/*
	 * Assign gsn's for skills which need them.
	 */
	{
		log_string("Assigning gsn's");

		ASSIGN_GSN(gsn_eliteguard, "elite_guard");
		ASSIGN_GSN(gsn_gather_intelligence, "gather_intelligence");
		ASSIGN_GSN(gsn_specialforces, "special_forces");
		ASSIGN_GSN(gsn_jail, "jail");
		ASSIGN_GSN(gsn_smalltalk, "smalltalk");
		ASSIGN_GSN(gsn_propeganda, "propeganda");
		ASSIGN_GSN(gsn_bribe, "bribe");
		ASSIGN_GSN(gsn_seduce, "seduce");
		ASSIGN_GSN(gsn_masspropeganda, "mass_propeganda");
		ASSIGN_GSN(gsn_beg, "beg");
		ASSIGN_GSN(gsn_hijack, "hijack");
		ASSIGN_GSN(gsn_makejewelry, "makejewelry");
		ASSIGN_GSN(gsn_grenades, "grenades");
		ASSIGN_GSN(gsn_makeblade, "makeblade");
		ASSIGN_GSN(gsn_makehackdev, "makehackdev");
		ASSIGN_GSN(gsn_hackshiplock, "hackshiplock");
		ASSIGN_GSN(gsn_makeblaster, "makeblaster");
		ASSIGN_GSN(gsn_makelight, "makeflashlight");
		ASSIGN_GSN(gsn_makecomlink, "makecomlink");
		ASSIGN_GSN(gsn_makegrenade, "makegrenade");
		ASSIGN_GSN(gsn_makelandmine, "makelandmine");
		ASSIGN_GSN(gsn_makearmor, "makearmor");
		ASSIGN_GSN(gsn_makeshield, "makeshield");
		ASSIGN_GSN(gsn_makecontainer, "makecontainer");
//        ASSIGN_GSN( gsn_makemissile, 		"makemissile" 		);
		ASSIGN_GSN(gsn_gemcutting, "gemcutting");
		ASSIGN_GSN(gsn_reinforcements, "reinforcements");
		ASSIGN_GSN(gsn_postguard, "post guard");
		ASSIGN_GSN(gsn_torture, "torture");
		ASSIGN_GSN(gsn_throw, "throw");
		ASSIGN_GSN(gsn_snipe, "snipe");
		ASSIGN_GSN(gsn_disguise, "disguise");
		ASSIGN_GSN(gsn_mine, "mine");
		ASSIGN_GSN(gsn_first_aid, "firstaid");
		ASSIGN_GSN(gsn_lightsaber_crafting, "lightsaber crafting");
		ASSIGN_GSN(gsn_spice_refining, "spice refining");
		ASSIGN_GSN(gsn_spacecombat, "space combat 1");
		ASSIGN_GSN(gsn_spacecombat2, "space combat 2");
		ASSIGN_GSN(gsn_spacecombat3, "space combat 3");
		ASSIGN_GSN(gsn_spacetactics, "space tactics");
		ASSIGN_GSN(gsn_weaponsystems, "weapon systems");
		ASSIGN_GSN(gsn_starfighters, "starfighters");
		ASSIGN_GSN(gsn_scoutships, "scoutships");
		ASSIGN_GSN(gsn_freighters, "freighters");
		ASSIGN_GSN(gsn_hugeships, "superships");
		ASSIGN_GSN(gsn_navigation, "navigation");
		ASSIGN_GSN(gsn_shipsystems, "ship systems");
		ASSIGN_GSN(gsn_shipturrets, "ship turrets");
		ASSIGN_GSN(gsn_advancedsystems, "advanced systems");
		ASSIGN_GSN(gsn_advancenavigation, "advanced navigation");
		ASSIGN_GSN(gsn_platforms, "platforms");
		ASSIGN_GSN(gsn_midships, "midships");
		ASSIGN_GSN(gsn_capitalships, "capital ships");
		ASSIGN_GSN(gsn_smugglersworld, "smugglers world");
		ASSIGN_GSN(gsn_tractorbeams, "tractor beams");
		ASSIGN_GSN(gsn_shipmaintenance, "ship maintenance");
		ASSIGN_GSN(gsn_blasters, "blasters");
		ASSIGN_GSN(gsn_bowcasters, "bowcasters");
		ASSIGN_GSN(gsn_force_pikes, "force pikes");
		ASSIGN_GSN(gsn_lightsabers, "lightsabers");
		ASSIGN_GSN(gsn_vibro_blades, "vibro-blades");
		ASSIGN_GSN(gsn_flexible_arms, "flexible arms");
		ASSIGN_GSN(gsn_talonous_arms, "talonous arms");
		ASSIGN_GSN(gsn_bludgeons, "bludgeons");
		//	ASSIGN_GSN( gsn_shieldwork,			"shieldwork" 		);
		ASSIGN_GSN(gsn_detrap, "detrap");
		ASSIGN_GSN(gsn_dominate, "dominate");
		ASSIGN_GSN(gsn_backstab, "backstab");
		ASSIGN_GSN(gsn_garota, "garota");
		ASSIGN_GSN(gsn_circle, "circle");
		ASSIGN_GSN(gsn_dodge, "dodge");
		ASSIGN_GSN(gsn_hide, "hide");
		ASSIGN_GSN(gsn_peek, "peek");
		ASSIGN_GSN(gsn_pick_lock, "pick lock");
		ASSIGN_GSN(gsn_pickshiplock, "pick ship lock");
		ASSIGN_GSN(gsn_sneak, "sneak");
		ASSIGN_GSN(gsn_steal, "steal");
		ASSIGN_GSN(gsn_gouge, "gouge");
		ASSIGN_GSN(gsn_poison_weapon, "poison weapon");
		ASSIGN_GSN(gsn_disarm, "disarm");
		ASSIGN_GSN(gsn_enhanced_damage, "enhanced damage");
		ASSIGN_GSN(gsn_kick, "kick");
		ASSIGN_GSN(gsn_parry, "parry");
		ASSIGN_GSN(gsn_rescue, "rescue");
		ASSIGN_GSN(gsn_second_attack, "second attack");
		ASSIGN_GSN(gsn_third_attack, "third attack");
		//	ASSIGN_GSN( gsn_fourth_attack, 		"fourth attack" 	);
		//	ASSIGN_GSN( gsn_fifth_attack, 		"fifth attack" 		);
		ASSIGN_GSN(gsn_dual_wield, "dual wield");
		ASSIGN_GSN(gsn_twohanded_wield, "two-handed weapons");
		ASSIGN_GSN(gsn_punch, "punch");
		ASSIGN_GSN(gsn_bash, "bash");
		ASSIGN_GSN(gsn_stun, "stun");
		ASSIGN_GSN(gsn_bashdoor, "doorbash");
		ASSIGN_GSN(gsn_grip, "grip");
		ASSIGN_GSN(gsn_berserk, "berserk");
		ASSIGN_GSN(gsn_hitall, "hitall");
		//	ASSIGN_GSN( gsn_feed,				"feed" 			);
		ASSIGN_GSN(gsn_aid, "aid");
		ASSIGN_GSN(gsn_track, "track");
		ASSIGN_GSN(gsn_search, "search");
		ASSIGN_GSN(gsn_dig, "dig");
		ASSIGN_GSN(gsn_mount, "mount");
		//	ASSIGN_GSN( gsn_bite,				"bite" 			);
		//	ASSIGN_GSN( gsn_claw,				"claw" 			);
		//	ASSIGN_GSN( gsn_sting,				"sting" 		);
		//	ASSIGN_GSN( gsn_tail,				"tail" 			);
		ASSIGN_GSN(gsn_scribe, "scribe");
		//	ASSIGN_GSN( gsn_brew,				"brew" 			);
		ASSIGN_GSN(gsn_climb, "climb");
		ASSIGN_GSN(gsn_scan, "scan");
		//	ASSIGN_GSN( gsn_slice,				"slice" 		);
		ASSIGN_GSN(gsn_lightning_bolt, "force bolt");
		ASSIGN_GSN(gsn_aqua_breath, "aqua breath");
		ASSIGN_GSN(gsn_blindness, "blindness");
		ASSIGN_GSN(gsn_charm_person, "affect mind");
		//	ASSIGN_GSN( gsn_curse,				"curse" 		);
		ASSIGN_GSN(gsn_forceinvis, "mask");
		ASSIGN_GSN(gsn_poison, "poison");
		ASSIGN_GSN(gsn_sleep, "sleep");
		ASSIGN_GSN(gsn_possess, "possess");
		//added by Thanos
		ASSIGN_GSN(gsn_identify, "identify");
		ASSIGN_GSN(gsn_hacking, "hacking");
		ASSIGN_GSN(gsn_holonet, "holonet");
		ASSIGN_GSN(gsn_detonate, "detonate");

		ASSIGN_GSN(gsn_yuuzhan_biotech, "yuuzhanbiotech");

	}

	//added by Thanos
	//domy�lne czyszczenie p�tli for i rat
	if (!sysdata.test_only)
	{
		log_string("Clearing For & Rat loops");
		clean_for_loop();
		clean_rat_loop();

#if !defined( WIN32 )
		//log_string("Making wizlist");
		//make_wizlist();

		log_string("Initializing request pipe");
		init_request_pipe();

		log_string("Loading bans");
		load_banlist();
#endif
	}

	log_string("Loading languages");
	load_languages();
	log_string("Loaded languages");

	log_string("Loading races");
	load_races();
	/*
	 ILIST( RACE_LISTXML )
	 if( (race = load_race( ilist_buf )) )
	 LINK( race, first_race, last_race, next, prev );
	 ICLEAN;
	 */

	/*
	 * Read in all the area files.
	 */
	{

		ILIST(AREA_LISTXML)
			if ((area = load_area2(ilist_buf)) && install_area(area))
				top_area++;
		ICLEAN
		;

		/*
		 FILE *fpList;
		 int i;

		 log_string("Reading in area files...");
		 if ( ( fpList = fopen( AREA_LIST, "r" ) ) == NULL )
		 {
		 shutdown_mud( "Unable to open area list" );
		 exit( 1 );
		 }

		 i=1;
		 for ( ; ; )
		 {
		 strcpy( strArea, fread_word( fpList ) );

		 if ( strArea[0] == '$' )
		 break;

		 load_area_file( last_area, strArea );

		 }
		 fclose( fpList );
		 */
	}

	/*
	 * Thanos: je�li mud by� odpalony z opcj� 'test', to po przeczytaniu
	 *         krain (je�li jeszcze nie pad�) mo�e beztrosko zako�czy�
	 *         dzia�anie.
	 */
	if (sysdata.test_only)
	{
		fprintf(stderr, "%d areas found. Ok. Exiting." NL, top_area);
		exit(0);
	}
	else
	{
		sprintf(buf, "...%d areas found.", top_area);
		log_string(buf);
	}

	/*
	 *   initialize supermob.
	 *    must be done before reset_area!
	 */
	init_supermob();

	/*
	 * Fix up exits.
	 * Declare db booting over.
	 * Reset all areas once.
	 * Load up the notes file.
	 * and many other stuff...
	 */
	{
		CLAN_DATA *clan;
		log_string("Fixing exits");
		fix_exits();

		log_string("Initializing economy");
		initialize_economy();

		log_string("Loading materials");
		load_materials();

		log_string("Loading projects");
		load_projects();

		log_string("Loading complains");
		load_complains();

		log_string("Loading boards");
		load_boards();

		log_string("Loading homes");
		load_home_flags();

		log_string("Loading clans");
//	load_clans();

		ILIST(CLAN_LISTXML)
			if ((clan = load_clan2(ilist_buf)))
				clan_list.push_back(clan);
		ICLEAN
		;

		log_string("Loading corpses");
		load_corpses();

		log_string("Loading ships");
		load_ships();

		log_string("Loading ShipDb");	// Thanos 4 Aldegard
		load_shipdb();

		log_string("Loading space starsystems");
		load_space();

		log_string("Loading astros");/*Aldegard*/
		load_astros();

		log_string("Loading turbocars");
		load_turbocars();

		log_string("Loading bounties");
		load_bounties();

		log_string("Loading informs");
		load_informs();

		log_string("Loading Quests");
		load_quests();

		log_string("Loading clonings");
		load_clonings();

		log_string("Loading planets");
		load_planets();

		log_string("Loading helps");
		load_helps_files();

		log_string("Setting time and weather");
		set_timeweather();

		log_string("Loading stock markets data");
		load_stock_markets();

		log_string("Loading dialogs");
		load_dialogs_list();

		log_string("Loading pazaak games");
		SWPazaak::loadGames();

		log_string("Loading storages");
		load_storages();

// Thanos: jeszcze nie czas...
//	log_string( "Restoring ship states" );
//	restore_ship_states();

		log_string("Resetting galaxy");
		reset_world();

		fBootDb = false;

		MOBtrigger = true;
	}

	/*
	 * St�d skaczemy do 'odzyskiwacza' (copyover_recover)
	 * i odnawiamy graczy
	 * (Ale tylko, je�li by� CopyOver)  		--Thanos
	 */
	if (fCopyOver)
		copyover_recover();

	return;
}

AREA_DATA* new_area()
{
	AREA_DATA *pArea;

	CREATE(pArea, AREA_DATA, 1);

	/* next_on_planet / prev_on_planet removed - using std::list */
	pArea->planet = NULL;
	STRDUP(pArea->author, "unknown");
	STRDUP(pArea->name, "New Area");
	STRDUP(pArea->builder, "");
	STRDUP(pArea->filename, strArea);
	STRDUP(pArea->resetmsg, "");
	pArea->age = 15;
	pArea->nplayer = 0;
	pArea->lvnum = 0;
	pArea->uvnum = 0;
	pArea->low_range = 1;
	pArea->high_range = LEVEL_AVATAR;
	pArea->security = 2;
	pArea->Envy = false; /* domy�lnie krainki s� smaugowe */
	pArea->area_id = ++area_id;
	pArea->area_tmp = NULL;

	return pArea;
}

/*
 * Adds a help page to the list if it is not a duplicate of an existing page.
 * Page is insert-sorted by keyword.			-Thoric
 * (The reason for sorting is to keep do_hlist looking nice)
 */
void add_help(HELPS_FILE *fHelp, HELP_DATA *pHelp, bool moveto)
{
	int match;
	bool inserted = false;

	for (auto it = fHelp->helps.begin(); it != fHelp->helps.end(); ++it)
	{
		HELP_DATA *tHelp = *it;
		if (pHelp->level == tHelp->level
				&& !str_cmp(pHelp->keyword, tHelp->keyword))
		{
			if (!sysdata.silent)
				bug("duplicate: %s.  Deleting.", pHelp->keyword);
			free_help(pHelp);
			if (moveto)
				top_help--;
			return;
		}
		else if ((match = strcmp(
				pHelp->keyword[0] == '\'' ? pHelp->keyword + 1 : pHelp->keyword,
				tHelp->keyword[0] == '\'' ? tHelp->keyword + 1 : tHelp->keyword))
				< 0 || (match == 0 && pHelp->level > tHelp->level))
		{
			fHelp->helps.insert(it, pHelp);
			inserted = true;
			break;
		}
	}

	if (!inserted)
		fHelp->helps.push_back(pHelp);

	pHelp->file = fHelp;
	if (!moveto)
		top_help++;

	return;
}

/* Trog: wczytywanie helpow */
void load_helps_files()
{
	FILE *fpList;
	char strHelp[MIL];
	char found_str[MIL];

	log_string("Reading in helps files...");
	RESERVE_CLOSE;

	if (!(fpList = fopen( HELPS_LIST, "r")))
	{
		log_string("Unable to open helps list.");
		RESERVE_OPEN;
		return;
	}

	for (;;)
	{
		strcpy(strHelp, fread_word(fpList));
		if (strHelp[0] == '$')
			break;
		load_helps_section(strHelp);
	}

	sprintf(found_str, "...%d helps found in %d files.", top_help,
			top_helps_file);
	log_string(found_str);
	fclose(fpList);
	RESERVE_OPEN;
	return;
}

/* Trog */
void load_helps_section(char *filename)
{
	FILE *fpHelp;
	char help_file[256];
	char *word;

	sprintf(help_file, "%s%s", HELPS_DIR, filename);
	if (!(fpHelp = fopen(help_file, "r")))
	{
		bug("error loading helps file (can't open %s)", help_file);
		return;
	}

	word = fread_word(fpHelp);

	if (!str_cmp(word, "#HELPS") || !str_cmp(word, "#SWHELPS"))
	{
		load_helps(fpHelp, filename);
		top_helps_file++;
	}
	else
	{
		bug("bad helps file format (%s)", filename);
		if (fBootDb)
			exit(1);
		else
		{
			fclose(fpHelp);
			return;
		}
	}

	fclose(fpHelp);
	return;
}

/* Trog: odmiana funkcji adv_load_helps Thanosa */
void load_helps(FILE *fpHelp, char *filename)
{
	HELPS_FILE *fHelp;
	HELP_DATA *pHelp;

	fHelp = new_helps_file();
	STRDUP(fHelp->name, filename);
	helps_file_list.push_back(fHelp);

	for (;;)
	{
		pHelp = new_help(fHelp);
		pHelp->level = fread_number(fpHelp);
		STRDUP(pHelp->keyword, st_fread_string(fpHelp));
		if (pHelp->keyword[0] == '$')
			break;
		pHelp->type = fread_number(fpHelp);
		STRDUP(pHelp->syntax, st_fread_string(fpHelp));
		STRDUP(pHelp->text, st_fread_string(fpHelp));
		if (pHelp->keyword[0] == '\0')
		{
			free_help(pHelp);
			continue;
		}

		if (!str_cmp(pHelp->keyword, "greeting"))
			help_greeting = pHelp->text;
		add_help(fHelp, pHelp, false);
	}

	return;
}

/*
 * Add a character to the list of all characters		-Thoric
 */
void add_char(CHAR_DATA *ch)
{
	char_list.push_back(ch);
}

/*
 * Go through all areas, and set up initial economy based on mob
 * levels and gold
 */
void initialize_economy(void)
{
	MOB_INDEX_DATA *mob;
	int idx, gold, rng;

	for (auto* tarea : area_list)
	{
		/* skip area if they already got some gold */
		if (tarea->high_economy > 0 || tarea->low_economy > 10000)
			continue;
		rng = tarea->high_range - tarea->low_range;
		if (rng)
			rng /= 2;
		else
			rng = 25;
		gold = rng * rng * 10000;
		boost_economy(tarea, gold);
		for (idx = tarea->lvnum; idx < tarea->uvnum; idx++)
			if ((mob = get_mob_index(idx)) != NULL)
				boost_economy(tarea, mob->gold * 10);
	}
}

/*
 * Translate all room exits from virtual to real.
 * Has to be done after all rooms are read in.
 * Check for bad reverse exits.
 */
void fix_exits(void)
{
	EXIT_DATA *rev_exit;
	int iHash;
	char buf[MSL];

	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (auto* pRoomIndex : room_index_hash[iHash])
		{
			bool fexit;

			fexit = false;
			auto exit_snapshot = pRoomIndex->exits;
			for (auto* pexit : exit_snapshot)
			{
				pexit->rvnum = pRoomIndex->vnum;
				if (pexit->vnum <= 0
						|| (pexit->to_room = get_room_index(pexit->vnum))
								== NULL)
				{
					if (fBootDb)
						boot_log(
								"Fix_exits: room %d, exit %s leads to bad vnum (%d)",
								pRoomIndex->vnum, dir_name[pexit->vdir],
								pexit->vnum);

					sprintf(buf, "Deleting %s exit in room %d",
							dir_name[pexit->vdir], pRoomIndex->vnum);
					log_string(buf);
					extract_exit(pRoomIndex, pexit);
				}
				else
					fexit = true;
			}
			if (!fexit)
				SET_BIT(pRoomIndex->room_flags, ROOM_NO_MOB);
		}
	}

	/* Set all the rexit pointers 	-Thoric */
	for (iHash = 0; iHash < MAX_KEY_HASH; iHash++)
	{
		for (auto* pRoomIndex : room_index_hash[iHash])
		{
			for (auto* pexit : pRoomIndex->exits)
			{
				if (pexit->to_room && !pexit->rexit)
				{
					rev_exit = get_exit_to(pexit->to_room, rev_dir[pexit->vdir],
							pRoomIndex->vnum);
					if (rev_exit)
					{
						pexit->rexit = rev_exit;
						rev_exit->rexit = pexit;
					}
				}
			}
		}
	}

	return;
}

/*
 * Get diku-compatable exit by number				-Thoric
 */
EXIT_DATA* get_exit_number(ROOM_INDEX_DATA *room, int xit)
{
	int count;

	count = 0;
	for (auto* pexit : room->exits)
		if (++count == xit)
			return pexit;
	return NULL;
}

/*
 * (prelude...) This is going to be fun... NOT!
 * (conclusion) QSort is f*cked!
 */
int exit_comp(EXIT_DATA **xit1, EXIT_DATA **xit2)
{
	int d1, d2;

	d1 = (*xit1)->vdir;
	d2 = (*xit2)->vdir;

	if (d1 < d2)
		return -1;
	if (d1 > d2)
		return 1;
	return 0;
}

void sort_exits(ROOM_INDEX_DATA *room)
{
	EXIT_DATA *exit_arr[MAX_REXITS];
	int x, nexits;

	nexits = 0;
	for (auto* pexit : room->exits)
	{
		exit_arr[nexits++] = pexit;
		if (nexits > MAX_REXITS)
		{
			bug("more than %d exits in room... fatal", nexits);
			return;
		}
	}
	qsort(&exit_arr[0], nexits, sizeof(EXIT_DATA*),
			(int (*)(const void*, const void*)) exit_comp);
	room->exits.clear();
	for (x = 0; x < nexits; x++)
		room->exits.push_back(exit_arr[x]);
}

void randomize_exits(ROOM_INDEX_DATA *room, int maxdir)
{
	int nexits, /* maxd, */d0, d1, count, door; /* Maxd unused */
	int vdirs[MAX_REXITS];

	nexits = 0;
	for (auto* pexit : room->exits)
		vdirs[nexits++] = pexit->vdir;

	for (d0 = 0; d0 < nexits; d0++)
	{
		if (vdirs[d0] > maxdir)
			continue;
		count = 0;
		while (vdirs[(d1 = number_range(d0, nexits - 1))] > maxdir
				|| ++count > 5)
			;
		if (vdirs[d1] > maxdir)
			continue;
		door = vdirs[d0];
		vdirs[d0] = vdirs[d1];
		vdirs[d1] = door;
	}
	count = 0;
	for (auto* pexit : room->exits)
		pexit->vdir = vdirs[count++];

	sort_exits(room);
}

/*
 * Repopulate areas periodically.
 */
void area_update(void)
{
	for (auto* pArea : area_list)
	{
		int reset_age;

		reset_age =
				pArea->reset_frequency ? pArea->reset_frequency : DEF_RESETFREQ;

		if ((reset_age == -1 && pArea->age == -1)
				|| ++pArea->age < (reset_age - 1))
			continue;

		/*
		 * Check for PC's.
		 */
		if (pArea->nplayer > 0 && pArea->age == (reset_age - 1))
		{
			char buf[MAX_STRING_LENGTH];

			/* Rennard */
			if (pArea->resetmsg && *pArea->resetmsg)
				sprintf(buf, NL "%s" EOL, pArea->resetmsg);
			else
				strcpy(buf,
						NL "S�yszysz jakie� dziwne �omotanie w oddali..." NL);

			for (auto* pch : char_list)
			{
				if (!IS_NPC(pch) && IS_AWAKE(pch)
				&& pch->in_room
				&& pch->in_room->area == pArea
				&& !IS_SET( pch->in_room->room_flags, ROOM_SPACECRAFT ))
					send_to_char(buf, pch);
			}
		}

		/*
		 * Check age and reset.
		 * Note: Mud Academy resets every 3 minutes (not 15).
		 */
		if (pArea->nplayer == 0 || pArea->age >= reset_age)
		{
			ROOM_INDEX_DATA *pRoomIndex;

			reset_area(pArea);

			if (reset_age == -1)
				pArea->age = -1;
			else
				pArea->age = number_range(0, reset_age / 5);

			pRoomIndex = get_room_index( ROOM_VNUM_SCHOOL);
			if (pRoomIndex && pArea == pRoomIndex->area)
				pArea->age = reset_age - 10;
//		pArea->age = reset_age - 3;
		}
	}
	return;
}

/*
 * Create an instance of a mobile.
 */
CHAR_DATA* create_mobile(MOB_INDEX_DATA *pMobIndex)
{
	CHAR_DATA *mob;
	int i;

	if (!pMobIndex)
	{
		bug("NULL pMobIndex.");
		exit(1);
	}

	CREATE(mob, CHAR_DATA, 1);
	clear_char(mob);
	mob->pIndexData = pMobIndex;

	CREATE(mob->variables, VAR_DATA, 1);	// Ratm mem alloc dla zmiennych
	bzero(mob->variables, sizeof(VAR_DATA)); // w progach

	STRDUP(mob->name, pMobIndex->player_name);

	for (i = 0; i < 6; i++)
		STRDUP(mob->przypadki[i], pMobIndex->przypadki[i]);

	STRDUP(mob->long_descr, pMobIndex->long_descr);
	STRDUP(mob->description, pMobIndex->description);
	STRDUP(mob->dialog_name, pMobIndex->dialog_name);
	mob->spec_fun = pMobIndex->spec_fun;
	mob->spec_2 = pMobIndex->spec_2;

	mob->top_level = number_fuzzy(pMobIndex->level);
	{
		int ability;
		for (ability = 0; ability < MAX_ABILITY; ability++)
			mob->skill_level[ability] = mob->top_level;
	}

	mob->act = pMobIndex->act;
	mob->affected_by = pMobIndex->affected_by;
	mob->alignment = pMobIndex->alignment;
	mob->sex = pMobIndex->sex;
	mob->main_ability = 0;
	STRDUP(mob->mob_clan, "");
	mob->was_sentinel = NULL;
	mob->plr_home = NULL;
	mob->guard_data = NULL;

#if defined( ARMAGEDDON )
	mob->armor		= interpolate( mob->top_level, 100, -100 );
	mob->max_hit	= 4 +   mob->top_level * 8 + number_range(
				mob->top_level * mob->top_level / 4,
				mob->top_level * mob->top_level );
	mob->hit		= mob->max_hit;

#else

	if (pMobIndex->ac != 0)
		mob->armor = pMobIndex->ac;
	else
		mob->armor = 500 - mob->top_level * 9;

	mob->max_hit = mob->top_level * 10
			+ number_range(mob->top_level, mob->top_level * 10);

	/* Trog: no bez jaj, ale to nie moze miec mniej niz w przypadku powyzszym */
	if (pMobIndex->hitnodice)
	{
		int max_hp = 0;

		for (int i = 0; i < pMobIndex->hitnodice; ++i)
		{
			max_hp += number_range(1, pMobIndex->hitsizedice);
		}
		max_hp += pMobIndex->hitplus;

		if (max_hp > mob->max_hit)
			mob->max_hit = max_hp;
	}

	mob->hit = mob->max_hit;

	if (!pMobIndex->damnodice)
		mob->barenumdie = mob->top_level / 10;
	else
		mob->barenumdie = pMobIndex->damnodice;
	if (!pMobIndex->damsizedice)
		mob->baresizedie = 4;
	else
		mob->baresizedie = pMobIndex->damsizedice;
#endif

	if (!pMobIndex->perm_str)
		mob->perm_str = 13;
	else
		mob->perm_str = pMobIndex->perm_str;
	if (!pMobIndex->perm_dex)
		mob->perm_dex = 13;
	else
		mob->perm_dex = pMobIndex->perm_dex;
	if (!pMobIndex->perm_wis)
		mob->perm_wis = 13;
	else
		mob->perm_wis = pMobIndex->perm_wis;
	if (!pMobIndex->perm_int)
		mob->perm_int = 13;
	else
		mob->perm_int = pMobIndex->perm_int;
	if (!pMobIndex->perm_con)
		mob->perm_con = 13;
	else
		mob->perm_con = pMobIndex->perm_con;
	if (!pMobIndex->perm_cha)
		mob->perm_cha = 13;
	else
		mob->perm_cha = pMobIndex->perm_cha;
	if (!pMobIndex->perm_lck)
		mob->perm_lck = 13;
	else
		mob->perm_lck = pMobIndex->perm_lck;
	//done

	/* lets put things back the way they used to be! -Thoric */
	mob->gold = pMobIndex->gold;
	mob->position = pMobIndex->position;
	mob->defposition = pMobIndex->defposition;

	mob->mobthac0 = pMobIndex->mobthac0; /*Nieu�ywane*/
	mob->hitplus = pMobIndex->hitplus;
	mob->damplus = pMobIndex->damplus; /*Nieu�ywane*/
	mob->hitroll = UMAX(mob->top_level / 5, pMobIndex->hitroll);
	mob->damroll = UMAX(mob->top_level / 5, pMobIndex->damroll);
	mob->race = pMobIndex->race;
	mob->xflags = pMobIndex->xflags;
	mob->saving_poison_death = pMobIndex->saving_poison_death;
	mob->saving_wand = pMobIndex->saving_wand;
	mob->saving_para_petri = pMobIndex->saving_para_petri;
	mob->saving_breath = pMobIndex->saving_breath;
	mob->saving_spell_staff = pMobIndex->saving_spell_staff;
	mob->height = pMobIndex->height;
	mob->weight = pMobIndex->weight;
	mob->resistant = pMobIndex->resistant;
	mob->immune = pMobIndex->immune;
	mob->susceptible = pMobIndex->susceptible;
	mob->attacks = pMobIndex->attacks;
	mob->defenses = pMobIndex->defenses;
	mob->numattacks = pMobIndex->numattacks;
	mob->speaking = pMobIndex->speaking;
	STRDUP(mob->s_vip_flags, pMobIndex->s_vip_flags);

	/*
	 * Insert in list.
	 */
	add_char(mob);
	pMobIndex->count++;
	nummobsloaded++;
	return mob;
}

/*
 * Create an instance of an object.
 */
OBJ_DATA* create_object(OBJ_INDEX_DATA *pObjIndex, int level)
{
	OBJ_DATA *obj;
	int i;

	if (!pObjIndex)
	{
		bug("NULL pObjIndex.");
		exit(1);
	}

	CREATE(obj, OBJ_DATA, 1);

	obj->pIndexData = pObjIndex;
	obj->in_room = NULL;
	obj->inquest = NULL;
	STRDUP(obj->owner_name, ""); /* Thanos -- personale */
	obj->level = level;
	obj->wear_loc = -1;
	obj->count = 1;
	cur_obj_serial = UMAX((cur_obj_serial + 1 ) & (BV30-1), 1);
	obj->serial = obj->pIndexData->serial = cur_obj_serial;

	STRDUP(obj->armed_by, "");
	STRDUP(obj->name, pObjIndex->name);

	CREATE(obj->variables, VAR_DATA, 1);	// Ratm mem alloc dla zmiennych
	bzero(obj->variables, sizeof(VAR_DATA)); // w progach

	for (i = 0; i < 6; i++)
		STRDUP(obj->przypadki[i], pObjIndex->przypadki[i]);
	STRDUP(obj->description, pObjIndex->description);
	STRDUP(obj->action_desc, pObjIndex->action_desc);
	obj->item_type = pObjIndex->item_type;
	obj->extra_flags = pObjIndex->extra_flags;
	obj->wear_flags = pObjIndex->wear_flags;
	obj->value[0] = pObjIndex->value[0];
	obj->value[1] = pObjIndex->value[1];
	obj->value[2] = pObjIndex->value[2];
	obj->value[3] = pObjIndex->value[3];
	obj->value[4] = pObjIndex->value[4];
	obj->value[5] = pObjIndex->value[5];
	obj->weight = pObjIndex->weight;
	obj->cost = pObjIndex->cost;
	obj->gender = pObjIndex->gender;
	/*
	 obj->cost			= number_fuzzy( 10 )
	 * number_fuzzy( level ) * number_fuzzy( level );
	 */

	/*
	 * Mess with object properties.
	 */
	switch (obj->item_type)
	{
	default:
		bug("vnum %d bad type.", pObjIndex->vnum);
		bug("------------------------>  %d", obj->item_type);
		obj->item_type = ITEM_TRASH;
		break;

	case ITEM_GOVERNMENT:
	case ITEM_SHIPMODULE:
	case ITEM_RAWSPICE:
	case ITEM_LENS:
	case ITEM_CRYSTAL:
	case ITEM_DURAPLAST:
	case ITEM_DURASTEEL:
	case ITEM_SUPERCONDUCTOR:
	case ITEM_COMLINK:
	case ITEM_MEDPAC:
	case ITEM_FABRIC:
	case ITEM_RARE_METAL:
	case ITEM_MAGNET:
	case ITEM_THREAD:
	case ITEM_CHEMICAL:
	case ITEM_SPICE:
	case ITEM_SMUT:
	case ITEM_OVEN:
	case ITEM_MIRROR:
	case ITEM_CIRCUIT:
	case ITEM_TOOLKIT:
	case ITEM_LIGHT:
	case ITEM_TREASURE:
	case ITEM_FURNITURE:
	case ITEM_TRASH:
	case ITEM_CONTAINER:
	case ITEM_DRINK_CON:
	case ITEM_COMPUTER:
	case ITEM_KEY:
	case ITEM_PAZAAK_SIDE_DECK:
	case ITEM_PAZAAK_CARD:
		break;

	case ITEM_FOOD:
		/*
		 * optional food condition (rotting food)		-Thoric
		 * value1 is the max condition of the food
		 * value4 is the optional initial condition
		 */
		if (obj->value[4])
			obj->timer = obj->value[4];
		else
			obj->timer = obj->value[1];
		break;

	case ITEM_DROID_CORPSE:
	case ITEM_CORPSE_NPC:
	case ITEM_CORPSE_PC:
	case ITEM_FOUNTAIN:
	case ITEM_SCRAPS:
	case ITEM_GRENADE:
	case ITEM_LANDMINE:
	case ITEM_SWITCH:
	case ITEM_LEVER:
	case ITEM_BUTTON:
	case ITEM_DIAL:
	case ITEM_TRAP:
	case ITEM_MAP:
	case ITEM_PAPER:
	case ITEM_PEN:
	case ITEM_LOCKPICK:
	case ITEM_FUEL:
	case ITEM_MISSILE:
	case ITEM_SHOVEL:
	case ITEM_SHIPDEVICE:
	case ITEM_PIECE:
		break;

	case ITEM_SALVE:
		obj->value[3] = number_fuzzy(obj->value[3]);
		break;

	case ITEM_DEVICE:
		obj->value[0] = number_fuzzy(obj->value[0]);
		obj->value[1] = number_fuzzy(obj->value[1]);
		obj->value[2] = obj->value[1];
		break;

	case ITEM_BATTERY:
		if (obj->value[0] <= 0)
			obj->value[0] = number_fuzzy(95);
		break;

	case ITEM_BOLT:
		if (obj->value[0] <= 0)
			obj->value[0] = number_fuzzy(95);
		break;

	case ITEM_AMMO:
		if (obj->value[0] <= 0)
			obj->value[0] = number_fuzzy(495);
		break;

	case ITEM_WEAPON:
#if defined( ARMAGEDDON )
	obj->value[1]   = number_fuzzy( number_fuzzy( 1 * level / 4 + 2 ) );
	obj->value[2]	= number_fuzzy( number_fuzzy( 3 * level / 4 + 6 ) );
	break;
#else
		if (obj->value[1] && obj->value[2])
			obj->value[2] *= obj->value[1];
		else
		{
			obj->value[1] = number_fuzzy(number_fuzzy(1 + level / 20));
			obj->value[2] = number_fuzzy(number_fuzzy(10 + level / 10));
		}
		if (obj->value[1] > obj->value[2])
			obj->value[1] = obj->value[2] / 3;
#endif
		if (obj->value[0] == 0)
			obj->value[0] = INIT_WEAPON_CONDITION;
		switch (obj->value[3])
		{
		case WEAPON_BLASTER:
		case WEAPON_LIGHTSABER:
		case WEAPON_VIBRO_BLADE:
		case WEAPON_FORCE_PIKE:
		case WEAPON_BOWCASTER:
			if (obj->value[5] <= 0)
				obj->value[5] = number_fuzzy(1000);
		}
		obj->value[4] = obj->value[5];
		break;

	case ITEM_ARMOR:
#if defined( ARMAGEDDON )
	obj->value[0]   = number_fuzzy( level / 4 + 2 );
#else
		if (obj->value[0] == 0)
			obj->value[0] = obj->value[1];
#endif
		obj->timer = obj->value[3];
		break;

	case ITEM_POTION:
	case ITEM_PILL:
		obj->value[0] = number_fuzzy(number_fuzzy(obj->value[0]));
		break;

	case ITEM_MONEY:
		obj->value[0] = obj->cost;
		break;
	}

#if defined( ARMAGEDDON )
	if( obj->item_type == ITEM_WEAPON )
	{
	REQUIREMENT_DATA *	req;

	CREATE( req, REQUIREMENT_DATA, 1 );
		req->location   =   REQ_LEVEL;
	req->modifier 	=   UMAX( 0, obj->level );
		req->type       =   0;
	obj->requirements.push_back(req);
	}
#endif

	object_list.push_back(obj);
	++pObjIndex->count;
	++numobjsloaded;
	++physicalobjects;

	return obj;
}

/*
 * Clear a new character.
 */
void clear_char(CHAR_DATA *ch)
{
	int i;

	ch->hunting = NULL;
	ch->fearing = NULL;
	ch->hating = NULL;
	ch->name = NULL;
	ch->fighting = NULL;
	ch->switched = NULL;
	ch->last_cmd = NULL;
	ch->mount = NULL;
	ch->plr_home = NULL;
	ch->was_sentinel = NULL;
	ch->deposit = NULL; /* Thanos */
	ch->inquest = NULL; /* Thanos */
	ch->variables = NULL; /* Ratm */
	ch->attribute1 = INT_MIN;
	ch->attribute2 = INT_MIN; //Tanglor
	ch->kins = 0; //Tanglor

	ch->in_room = get_room_index( ROOM_VNUM_LIMBO);

	ch->charset = CHARSET_NOPOL;
	ch->affected_by = 0;
	ch->logon = current_time;
	ch->armor = 100;
	ch->position = POS_STANDING;
	ch->hit = DEF_HP;
	ch->max_hit = DEF_HP;
	ch->mana = DEF_MANA;
	ch->max_mana = 0;
	ch->move = DEF_MV;
	ch->max_move = DEF_MV;
	ch->height = 72;
	ch->weight = 180;
	ch->xflags = 0;
	ch->race = base_race;
	ch->speaking = lang_base;
	ch->barenumdie = 1;
	ch->baresizedie = 4;
	ch->substate = 0;
	ch->tempnum = 0;
	ch->perm_str = 10;
	ch->perm_dex = 10;
	ch->perm_int = 10;
	ch->perm_wis = 10;
	ch->perm_cha = 10;
	ch->perm_con = 10;
	ch->perm_lck = 10;
	ch->mod_str = 0;
	ch->mod_dex = 0;
	ch->mod_int = 0;
	ch->mod_wis = 0;
	ch->mod_cha = 0;
	ch->mod_con = 0;
	ch->mod_lck = 0;
	ch->act = PLR_BLANK | PLR_COMBINE | PLR_PROMPT;
	ch->mental_state = -10;
	ch->mobinvis = 0;

	ch->saving_poison_death = 0;
	ch->saving_wand = 0;
	ch->saving_para_petri = 0;
	ch->saving_breath = 0;
	ch->saving_spell_staff = 0;

	STRDUP(ch->name, "");
	STRDUP(ch->long_descr, "");
	STRDUP(ch->description, "");
	for (i = 0; i < 6; i++)
		STRDUP(ch->przypadki[i], "");

	STRDUP(ch->dest_buf, "");
	STRDUP(ch->dest_buf_2, "");
	STRDUP(ch->s_vip_flags, "");
	STRDUP(ch->mob_clan, "");
	STRDUP(ch->dialog_name, "");
	return;
}

void clear_pcdata(PC_DATA *pcdata)
{
	int i;

	STRDUP(pcdata->prefix, "");
	STRDUP(pcdata->homepage, "");
	STRDUP(pcdata->pwd, "");
	STRDUP(pcdata->email, "");
	STRDUP(pcdata->bamfin, "");
	STRDUP(pcdata->bamfout, "");
	STRDUP(pcdata->rank, "");
	STRDUP(pcdata->title, "");
	STRDUP(pcdata->fake_title, "");/*Thanos */
	STRDUP(pcdata->fake_name, "");/*Thanos */
	STRDUP(pcdata->fake_desc, "");/*Thanos */
	STRDUP(pcdata->fake_long, "");/*Thanos */
	for (i = 0; i < 6; i++)
		STRDUP(pcdata->fake_infl[i], "");/*Thanos */
	STRDUP(pcdata->bestowments, "");
	STRDUP(pcdata->invis_except, ""); /*Thanos*/
	STRDUP(pcdata->isolated_by, "");
	STRDUP(pcdata->silenced_by, ""); /* Trog */
	STRDUP(pcdata->bio, "");
	STRDUP(pcdata->authed_by, "");
	STRDUP(pcdata->prompt, "");
	STRDUP(pcdata->forbidden_cmd, "");
	STRDUP(pcdata->afk_reason, "");
	STRDUP(pcdata->quest_done, "");
	STRDUP(pcdata->editinfo, ""); /*Thanos*/
	STRDUP(pcdata->ignorelist, ""); /*Thanos*/
	STRDUP(pcdata->tmp_site, ""); /*Thanos*/
	STRDUP(pcdata->host, ""); /*Thanos*/
	STRDUP(pcdata->user, ""); /*Thanos*/
	STRDUP(pcdata->last_dialog, ""); /* Tanglor*/
	pcdata->pMob_speaking = NULL;

	pcdata->clan = NULL;
	pcdata->wizinvis = 0;
	pcdata->last_note = 0;
	pcdata->condition[COND_THIRST] = COND_MAX;
	pcdata->condition[COND_FULL] = COND_MAX;
	pcdata->condition[COND_BLOODTHIRST] = 10;
	pcdata->wizinvis = 0;
	pcdata->release_date = 0;
	for (i = 0; i < MAX_SKILL; i++)
		pcdata->learned[i] = 0;
#if defined( ARMAGEDDON )
	pcdata->practices			= 0;
#endif
	pcdata->pagerlen = 24;
	pcdata->fevents = 0;
	pcdata->remembered_mob_bounty = 0;
}

/*
 * Get an extra description from a list.
 */
char* get_extra_descr(const char *name, std::list<EXTRA_DESCR_DATA*>& edlist)
{
	for (auto* ed : edlist)
		//by Thanos: is_name na is_name_prefix
		if (*ed->keyword && is_name_prefix(name, ed->keyword))
			return ed->description;

	return NULL;
}

/*
 * Translates mob virtual number to its mob index struct.
 * Hash table lookup.
 */
MOB_INDEX_DATA* get_mob_index(int vnum)
{
	if (vnum < 0)
		vnum = 0;

	for (auto* pMobIndex : mob_index_hash[vnum % MAX_KEY_HASH])
		if (pMobIndex->vnum == vnum)
			return pMobIndex;

	if (fBootDb)
		bug("bad vnum %d.", vnum);

	return NULL;
}

/*
 * Translates obj virtual number to its obj index struct.
 * Hash table lookup.
 */
OBJ_INDEX_DATA* get_obj_index(int vnum)
{
	if (vnum < 0)
		vnum = 0;

	for (auto* pObjIndex : obj_index_hash[vnum % MAX_KEY_HASH])
		if (pObjIndex->vnum == vnum)
			return pObjIndex;

	if (fBootDb)
		bug("bad vnum %d.", vnum);

	return NULL;
}

/*
 * Translates room virtual number to its room index struct.
 * Hash table lookup.
 */
ROOM_INDEX_DATA* get_room_index(int vnum)
{
	if (vnum <= 0 || vnum > MAX_VNUM)
		return NULL;

	for (auto* pRoomIndex : room_index_hash[vnum % MAX_KEY_HASH])
		if (pRoomIndex->vnum == vnum)
			return pRoomIndex;
	/*
	 if ( fBootDb )
	 {
	 bug( "Get_room_index: bad vnum %d.", vnum );
	 fprintf( stderr, "File: %s, line %d\n", __FILE__, __LINE__ );
	 }
	 */
	return NULL;
}

/*
 * Added lots of EOF checks, as most of the file crashes are based on them.
 * If an area file encounters EOF, the fread_* functions will shutdown the
 * MUD, as all area files should be read in in full or bad things will
 * happen during the game.  Any files loaded in without fBootDb which
 * encounter EOF will return what they have read so far.   These files
 * should include player files, and in-progress areas that are not loaded
 * upon bootup.
 * -- Altrag
 */

/*
 * Read a letter from a file.
 */
char fread_letter(FILE *fp)
{
	char c;

	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			return '\0';
		}
		c = __getc(fp);
	} while (isspace(c));

	return c;
}

/*
 * Read a number from a file.
 */
int fread_number(FILE *fp)
{
	int number;
	bool sign;
	char c;

	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			return 0;
		}
		c = getc(fp);
	} while (isspace(c));

	number = 0;

	sign = false;
	if (c == '+')
	{
		c = getc(fp);
	}
	else if (c == '-')
	{
		sign = true;
		c = getc(fp);
	}

	if (!isdigit(c))
	{
		bug("bad format. (%c)", c);
		if (fBootDb)
			exit(1);
		return 0;
	}

	while (isdigit(c))
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			return number;
		}
		number = number * 10 + c - '0';
		c = getc(fp);
	}

	if (sign)
		number = 0 - number;

	if (c == '|')
		number += fread_number(fp);
	else if (c != ' ')
		ungetc(c, fp);

	return number;
}

/* Trog: do liczb 64-bitowych */
int fread_number64(FILE *fp)
{
	int64 number;
	bool sign;
	char c;

	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			return 0;
		}
		c = getc(fp);
	} while (isspace(c));

	number = 0;

	sign = false;
	if (c == '+')
	{
		c = getc(fp);
	}
	else if (c == '-')
	{
		sign = true;
		c = getc(fp);
	}

	if (!isdigit(c))
	{
		bug("bad format. (%c)", c);
		if (fBootDb)
			exit(1);
		return 0;
	}

	while (isdigit(c))
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			return number;
		}
		number = number * 10 + c - '0';
		c = getc(fp);
	}

	if (sign)
		number = 0 - number;

	if (c == '|')
		number += fread_number(fp);
	else if (c != ' ')
		ungetc(c, fp);

	return number;
}

/*
 * custom str_dup using create					-Thoric
 */
char* str_dup(char const *str)
{
	static char *ret;
	int len;

	/*
	 * Poinformujmy, �e to nie jest dobry pomys� 	--Thanos
	 */

	bug("*--> %s", str);

	if (!str)
		return NULL;

	len = strlen(str) + 1;

	CREATE(ret, char, len);
	strcpy(ret, str);
	return ret;
}

/*
 * Read a string from file fp
 */
char* fread_string(FILE *fp)
{
	static char buf[MAX_STRING_LENGTH];
	char *plast;
	char c;
	int ln;

	plast = buf;
	buf[0] = '\0';
	ln = 0;

	/*
	 * Skip blanks.
	 * Read first char.
	 */
	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			return STRALLOC("");
		}
		c = __getc(fp);
	} while (isspace(c));

	if ((*plast++ = c) == '~')
		return STRALLOC("");

	for (;;)
	{
		if (ln >= (MAX_STRING_LENGTH - 1))
		{
			bug("string too long");
			*plast = '\0';
			return STRALLOC(buf);
		}
		switch (*plast = __getc(fp))
		{
		default:
			plast++;
			ln++;
			break;

		case EOF:
			bug("EOF");
			if (fBootDb)
				exit(1);
			*plast = '\0';
			return STRALLOC(buf);
			break;

		case '\n':
			plast++;
			ln++;
			*plast++ = '\r';
			ln++;
			break;

		case '\r':
			break;

		case '~':
			*plast = '\0';
			return STRALLOC(buf);
		}
	}
}

/*
 * Read a string from file fp using str_dup (ie: no string hashing)
 */
char* fread_string_nohash(FILE *fp)
{
	char buf[MAX_STRING_LENGTH];
	char *plast;
	char c;
	int ln;

	plast = buf;
	buf[0] = '\0';
	ln = 0;

	/*
	 * Skip blanks.
	 * Read first char.
	 */
	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			return str_dup("");
		}
		c = __getc(fp);
	} while (isspace(c));

	if ((*plast++ = c) == '~')
		return str_dup("");

	for (;;)
	{
		if (ln >= (MAX_STRING_LENGTH - 1))
		{
			bug("string too long");
			*plast = '\0';
			return str_dup(buf);
		}
		switch (*plast = __getc(fp))
		{
		default:
			plast++;
			ln++;
			break;

		case EOF:
			bug("EOF");
			if (fBootDb)
				exit(1);
			*plast = '\0';
			return str_dup(buf);
			break;

		case '\n':
			plast++;
			ln++;
			*plast++ = '\r';
			ln++;
			break;

		case '\r':
			break;

		case '~':
			*plast = '\0';
			return str_dup(buf);
		}
	}
}

/*
 * Read to end of line (for comments).
 */
void fread_to_eol(FILE *fp)
{
	char c;

	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			return;
		}
		c = getc(fp);
	} while (c != '\n' && c != '\r');

	do
	{
		c = getc(fp);
	} while (c == '\n' || c == '\r');

	ungetc(c, fp);
	return;
}

/*
 * Read to end of line into static buffer			-Thoric
 */
char* fread_line(FILE *fp)
{
	static char line[MAX_STRING_LENGTH];
	char *pline;
	char c;
	int ln;

	pline = line;
	line[0] = '\0';
	ln = 0;

	/*
	 * Skip blanks.
	 * Read first char.
	 */
	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			strcpy(line, "");
			return line;
		}
		c = __getc(fp);
	} while (isspace(c));

	ungetc(c, fp);
	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			*pline = '\0';
			return line;
		}
		c = __getc(fp);
		*pline++ = c;
		ln++;
		if (ln >= (MAX_STRING_LENGTH - 1))
		{
			bug("line too long");
			break;
		}
	} while (c != '\n' && c != '\r');

	do
	{
		c = __getc(fp);
	} while (c == '\n' || c == '\r');

	ungetc(c, fp);
	*pline = '\0';
	return line;
}

/*
 * Read one word (into static buffer).
 */
char* fread_word(FILE *fp)
{
	static char word[MAX_INPUT_LENGTH];
	char *pword;
	char cEnd;

	do
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			word[0] = '\0';
			return word;
		}
		cEnd = __getc(fp);
	} while (isspace(cEnd));

	if (cEnd == '\'' || cEnd == '"')
	{
		pword = word;
	}
	else
	{
		word[0] = cEnd;
		pword = word + 1;
		cEnd = ' ';
	}

	for (; pword < word + MAX_INPUT_LENGTH; pword++)
	{
		if (feof(fp))
		{
			bug("EOF encountered on read." NL);
			if (fBootDb)
				exit(1);
			*pword = '\0';
			return word;
		}
		*pword = __getc(fp);
		if (cEnd == ' ' ? isspace(*pword) : *pword == cEnd)
		{
			if (cEnd == ' ')
				ungetc(*pword, fp);
			*pword = '\0';
			return word;
		}
	}

	bug("word too long");
	exit(1);
	return NULL;
}

DEF_DO_FUN( memory )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	int hash;

#ifdef DMALLOC
	dmalloc_log_heap_map();
#endif

	argument = one_argument(argument, arg);

	/*
	 * Added by Thanos:
	 * ps sprawdza jak du�o pami�ci aktualnie z�era mud.
	 * Szkoda tylko, �e ta komenda te� swoje zajmuje, wi�c ci�ko
	 * z obiektywn� ocen� online.
	 */
	if (!str_cmp(arg, "status") && IS_CODER(ch->name))
	{
		char cmd[MSL];

		sprintf(cmd, "ps u %d > %s", pid, TMP_FILE);
		if (system(cmd) == -1)
			bug("failed to execute shell command: %s", cmd);
		else
		{
			show_file(ch, TMP_FILE);
			unlink(TMP_FILE);
		}
		return;
	}

	if (!str_cmp(arg, "proc"))
	{
		pid_t pid;

		/* nie dawajmy tej opcji wszystkim -- memory eater */
		if (!IS_CODER(ch->name))
		{
			send_to_pager("You cannot use this command with 'proc' option." NL,
					ch);
			return;
		}

		pid = getpid();

		/* Trog: to nie zadziala na FreeBSD - tam (juz) nie ma /proc (podmontowanego) */
		sprintf(buf, "/proc/%d/status", pid);

		send_to_pager("Memory stats:" NL, ch);

		show_file(ch, buf);
		send_to_pager( NL, ch);
		return;
	}
	/* done */

	ch_printf(ch, "Affects   %5d    Areas     %5d     Clonings:  %d" NL,
			top_affect, top_area, top_cloning);
	ch_printf(ch, "ExtDes    %5d    Exits     %5d" NL, top_ed, top_exit);
	ch_printf(ch, "Helps(%2d) %5d    Resets    %5d" NL, top_helps_file,
			top_help, top_reset);
	ch_printf(ch, "IdxMobs   %5d    Mobs      %5d" NL, top_mob_index,
			nummobsloaded);
	ch_printf(ch, "IdxObjs   %5d    Objs      %5d (%d)" NL, top_obj_index,
			numobjsloaded, physicalobjects);
	ch_printf(ch, "IdxQuests %5d    RunQuests %5d" NL, top_quest_index,
			top_quest);
	ch_printf(ch, "Rooms     %5d    VRooms    %5d" NL, top_room, top_vroom);
	ch_printf(ch, "Shops     %5d    RepShps   %5d" NL, top_shop, top_repair);
	ch_printf(ch, "CurOq's   %5d    CurCq's   %5d" NL, cur_qobjs, cur_qchars);
	ch_printf(ch, "Players   %5d    Maxplrs   %5d" NL, num_descriptors,
			sysdata.maxplayers);
	ch_printf(ch, "MaxEver   %5d    Topsn     %5d (%d)" NL, sysdata.alltimemax,
			top_sn, MAX_SKILL);
	ch_printf(ch, "MaxEver time recorded at:  %s" NL, sysdata.time_of_max);
	if (!str_cmp(arg, "check"))
	{
#ifdef HASHSTR
		send_to_char(check_hash(argument), ch);
#else
	send_to_char( "Hash strings not enabled." NL, ch );
#endif
		return;
	}
	if (!str_cmp(arg, "showhigh"))
	{
#ifdef HASHSTR
		show_high_hash(atoi(argument));
#else
	send_to_char( "Hash strings not enabled." NL, ch );
#endif
		return;
	}
	if (argument[0] != '\0')
		hash = atoi(argument);
	else
		hash = -1;
	if (!str_cmp(arg, "hash"))
	{
#ifdef HASHSTR
		ch_printf(ch, FG_CYAN "Hash statistics:" EOL "%s", hash_stats());
		if (hash != -1)
			hash_dump(hash);
#else
	send_to_char( "Hash strings not enabled." NL, ch );
#endif
	}
	return;
}

/*
 * Stick a little fuzz on a number.
 */
int number_fuzzy(int number)
{
	switch (number_bits(2))
	{
	case 0:
		number -= 1;
		break;
	case 3:
		number += 1;
		break;
	}

	return UMAX(1, number);
}

/*
 * Generate a random number.
 */
int number_range(int from, int to)
{
	/*    int power;
	 int number;*/

	if ((to = to - from + 1) <= 1)
		return from;

	/*    for ( power = 2; power < to; power <<= 1 )
	 ;

	 while ( ( number = number_mm() & (power - 1) ) >= to )
	 ;

	 return from + number;*/
	return (number_mm() % to) + from;
}

/*
 * Generate a percentile roll.
 */
int number_percent(void)
{
	/*    int percent;

	 while ( ( percent = number_mm() & (128-1) ) > 99 )
	 ;

	 return 1 + percent;*/
	return number_mm() % 100;
}

/*
 * Generate a random door.
 */
int number_door(void)
{
	int door;

	while ((door = number_mm() & (16 - 1)) > 9)
		;

	return door;
	/*    return number_mm() & 10; */
}

int number_bits(int width)
{
	return number_mm() & ((1 << width) - 1);
}

/*
 * I've gotten too many bad reports on OS-supplied random number generators.
 * This is the Mitchell-Moore algorithm from Knuth Volume II.
 * Best to leave the constants alone unless you've read Knuth.
 * -- Furey
 */
static int rgiState[2 + 55];

void init_mm()
{
	int *piState;
	int iState;

	piState = &rgiState[2];

	piState[-2] = 55 - 55;
	piState[-1] = 55 - 24;

	piState[0] = ((int) current_time) & ((1 << 30) - 1);
	piState[1] = 1;
	for (iState = 2; iState < 55; iState++)
	{
		piState[iState] = (piState[iState - 1] + piState[iState - 2])
				& ((1 << 30) - 1);
	}
	return;
}

int number_mm(void)
{
	int *piState;
	int iState1;
	int iState2;
	int iRand;

	piState = &rgiState[2];
	iState1 = piState[-2];
	iState2 = piState[-1];
	iRand = (piState[iState1] + piState[iState2]) & ((1 << 30) - 1);
	piState[iState1] = iRand;
	if (++iState1 == 55)
		iState1 = 0;
	if (++iState2 == 55)
		iState2 = 0;
	piState[-2] = iState1;
	piState[-1] = iState2;
	return iRand >> 6;
}

/*
 * Roll some dice.						-Thoric
 */
int dice(int number, int size)
{
	int idice;
	int sum;

	switch (size)
	{
	case 0:
		return 0;
	case 1:
		return number;
	}

	for (idice = 0, sum = 0; idice < number; idice++)
		sum += number_range(1, size);

	return sum;
}

/*
 * Simple linear interpolation.
 */
#if defined( ARMAGEDDON )
int interpolate( int level, int value_00, int value_100 )
{
	return value_00 + level * (value_100 - value_00) / 100;
}
#else
int interpolate(int level, int value_00, int value_32)
{
	return value_00 + level * (value_32 - value_00) / 32;
}
#endif

/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde(char *str)
{
	for (; *str != '\0'; str++)
		if (*str == '~')
			*str = '-';

	return;
}

/*
 * Encodes the tildes in a string.				-Thoric
 * Used for player-entered strings that go into disk files.
 */
void hide_tilde(char *str)
{
	for (; *str != '\0'; str++)
		if (*str == '~')
			*str = HIDDEN_TILDE;

	return;
}

char* show_tilde(char *str)
{
	static char buf[MAX_STRING_LENGTH];
	char *bufptr;

	bufptr = buf;
	for (; *str != '\0'; str++, bufptr++)
	{
		if (*str == HIDDEN_TILDE)
			*bufptr = '~';
		else
			*bufptr = *str;
	}
	*bufptr = '\0';

	return buf;
}

//Added by Ratm (zamienia polskie znaki na polskawe)
char change_to_nopol(const char letter)
{
	switch (letter)
	{
	case '\xb1':
		return 'a';
	case '\xe6':
		return 'c';
	case '\xea':
		return 'e';
	case '\xb3':
		return 'l';
	case '\xf1':
		return 'n';
	case '\xf3':
		return 'o';
	case '\xb6':
		return 's';
	case '\xbc':
		return 'z';
	case '\xbf':
		return 'z';
	case '\xa1':
		return 'A';
	case '\xc6':
		return 'C';
	case '\xca':
		return 'E';
	case '\xa3':
		return 'L';
	case '\xd1':
		return 'N';
	case '\xd3':
		return 'O';
	case '\xa6':
		return 'S';
	case '\xac':
		return 'Z';
	case '\xaf':
		return 'Z';
	default:
		return letter;
	}
}

/* Trog
 * Compare strings, case sensitive.
 * Return true if different
 */
bool sw_strcmp(const char *astr, const char *bstr)
{
	char a, b;

	if (!astr)
	{
		bug("null astr.");
		if (bstr)
			fprintf(stderr, "astr: (null)  bstr: %s\n", bstr);
		abort();
		return true;
	}

	if (!bstr)
	{
		bug("null bstr.");
		if (astr)
			fprintf(stderr, "astr: %s  bstr: (null)\n", astr);
		return true;
	}

	for (; *astr || *bstr; astr++, bstr++)
	{
		if ((a = *astr) != (b = *bstr))
		{
			if (ispolchar(a))
				a = change_to_nopol(a);
			if (ispolchar(b))
				b = change_to_nopol(b);
			if (a != b)
				return true;
		}
	}

	return false;
}

/*
 * Compare strings, case insensitive.
 * Return true if different
 *   (compatibility with historical functions).
 */
bool str_cmp(const char *astr, const char *bstr)
{
	char a, b;

	if (!astr)
	{
		bug("null astr.");
		if (bstr)
			fprintf(stderr, "str_cmp: astr: (null)  bstr: %s\n", bstr);
		abort();
		return true;
	}

	if (!bstr)
	{
		bug("null bstr.");
		if (astr)
			fprintf(stderr, "str_cmp: astr: %s  bstr: (null)\n", astr);
		return true;
	}

	for (; *astr || *bstr; astr++, bstr++)
	{
// Added by Ratm (doda�em poni�sze aby mo�na by�o gra� bez polskich znak�w)
		if ((a = LOWER(*astr)) != (b = LOWER(*bstr)))
		{
			if (ispolchar(a))
				a = change_to_nopol(a);
			if (ispolchar(b))
				b = change_to_nopol(b);
			if (a != b)
				return true;
		}
// done
	}

	return false;
}

/*
 * Compare strings, case insensitive, for prefix matching.
 * Return true if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix(const char *astr, const char *bstr)
{
	char a, b;
	if (!astr)
	{
		bug("null astr.");
		return true;
	}

	if (!bstr)
	{
		bug("null bstr.");
		return true;
	}

	for (; *astr; astr++, bstr++)
	{
// Added by Ratm (doda�em poni�sze aby mo�na by�o gra� bez polskich znak�w)
		if ((a = LOWER(*astr)) != (b = LOWER(*bstr)))
		{
			if (ispolchar(a))
				a = change_to_nopol(a);
			if (ispolchar(b))
				b = change_to_nopol(b);
			if (a != b)
				return true;
// done
		}
	}

	return false;
}

/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns true is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix(const char *astr, const char *bstr)
{
	int sstr1;
	int sstr2;
	int ichar;
	char c0;

	if ((c0 = LOWER(astr[0])) == '\0')
		return false;

	sstr1 = strlen(astr);
	sstr2 = strlen(bstr);

	for (ichar = 0; ichar <= sstr2 - sstr1; ichar++)
		if (c0 == LOWER(bstr[ichar]) && !str_prefix(astr, bstr + ichar))
			return false;

	return true;
}

/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns pointer to the begging of maching part of bstr.
 *   (like strstr).			-- from ROM 24
 * (dodane sprawdzanie polskich znak�w -- Thanos )
 * Ta f-cja jest prwaie identyko jak str_infix, ale
 * zwraca wskaznik zamiast boola.
 * (nie chcia�o mi sie przerabiac str_infix - kod juz zbyt czesto tego uzywal)
 */
char* str_str(const char *astr, char *bstr)
{
	char c0;
	char *ichar;

	if (!astr)
	{
		if (bstr)
			bug("null astr. Bstr: '%s'", bstr);
		else
			bug("null astr and bstr");
		return NULL;
	}

	if (!bstr)
	{
		bug("null bstr. Astr: '%s'", astr);
		return NULL;
	}

	if ((c0 = LOWER(change_to_nopol(astr[0]))) == '\0')
		return NULL;

	for (ichar = bstr; *ichar != '\0'; ichar++)
	{
		if (c0 == LOWER(change_to_nopol(*ichar)) && !str_prefix(astr, ichar))
			return ichar;
	}

	return NULL;
}

/*
 * Zamienia wyraz <old> wyrazem <new> w stringu <orig> i zwraca nowy str.
 * R�ni si� od 'string_replace' (string.c), �e nie robi STRDUP
 * -- Thanos
 */
char* str_repl(char *orig, const char *old, const char *_new, bool rAll)
{
	static char xbuf[ MAX_STRING_LENGTH];
	int i;
	char *x;

	xbuf[0] = '\0';
	strcpy(xbuf, orig);

	if (rAll)
	{
		x = orig;
		while ((x = str_str(old, x)))
		{
			i = strlen(xbuf) - strlen(x);
			xbuf[i] = '\0';
			strcat(xbuf, _new);
			x += strlen(old);
			strcat(xbuf, x);
		}
	}
	else
	{
		if (str_str(old, orig))
		{
			i = strlen(orig) - strlen(str_str(old, orig));
			xbuf[i] = '\0';
			strcat(xbuf, _new);
			strcat(xbuf, &orig[i + strlen(old)]);
		}
	}

	return xbuf;
}

/*
 * Compare strings, case insensitive, for suffix matching.
 * Return true if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix(const char *astr, const char *bstr)
{
	int sstr1;
	int sstr2;

	sstr1 = strlen(astr);
	sstr2 = strlen(bstr);
	if (sstr1 <= sstr2 && !str_cmp(astr, bstr + sstr2 - sstr1))
		return false;
	else
		return true;
}

/* Trog: comparing string with regular expression (case insensitive) */
/* Returns false if str matches regexp. */
bool reg_cmp(const char *regexp, const char *str)
{
	regex_t preg;
	regmatch_t match[REG_MATCH];

	bzero(&preg, sizeof(regex_t));
	bzero(&match, REG_MATCH * sizeof(regmatch_t));

	if (regcomp(&preg, regexp, REG_CFLAGS))
	{
		bug("malformed regular expression: %s", regexp);
		return true;
	}

	if (!regexec(&preg, str, REG_MATCH, match, REG_EFLAGS))
		return false;

	return true;
}

/*
 * Returns an initial-capped string.
 */
char* capitalize(const char *str)
{
	static char strcap[MAX_STRING_LENGTH];
	int i;

	for (i = 0; str[i] != '\0'; i++)
		strcap[i] = LOWER(str[i]);
	strcap[i] = '\0';
	strcap[0] = UPPER(strcap[0]);
	return strcap;
}

/*
 * Returns the same string, but first letter is upper.
 * Thanos
 */
char* fcapitalize(const char *str)
{
	static char strcap[MAX_STRING_LENGTH];
	int i;

	for (i = 0; str[i] != '\0'; i++)
		strcap[i] = str[i];
	strcap[i] = '\0';
	strcap[0] = UPPER(strcap[0]);
	return strcap;
}

/*
 * Returns a lowercase string.
 */
char* strlower(const char *str)
{
	static char strlow[MAX_STRING_LENGTH];
	int i;

	for (i = 0; str[i] != '\0'; i++)
		strlow[i] = LOWER(str[i]);
	strlow[i] = '\0';
	return strlow;
}

/*
 * Returns an uppercase string.
 */
char* strupper(const char *str)
{
	static char strup[MAX_STRING_LENGTH];
	int i;

	for (i = 0; str[i] != '\0'; i++)
		strup[i] = UPPER(str[i]);
	strup[i] = '\0';
	return strup;
}

/*
 * Returns true or false if a letter is a vowel			-Thoric
 */
bool isavowel(char letter)
{
	char c;

	c = tolower(letter);
	if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y')
		return true;
	else
		return false;
}

/*
 * Append a string to a file.
 */
void append_file(CHAR_DATA *ch, const char *file, char *str)
{
	FILE *fp;

	if ( IS_NPC(ch) || str[0] == '\0')
		return;

	LOG_CLOSE;
	if ((fp = fopen(file, "a")) == NULL)
	{
		send_to_char("Could not open the file!" NL, ch);
		perror(file);
	}
	else
	{
		fprintf(fp, "[%5d] %s: %s\n", ch->in_room ? ch->in_room->vnum : 0,
				ch->name, str);

		fclose(fp);
	}

	LOG_OPEN;
	return;
}

/*
 * Append a string to a file.
 */
void append_to_file(const char *file, char *str)
{
	FILE *fp;

	if ((fp = fopen(file, "a")) == NULL)
	{
	}
	else
	{
		fprintf(fp, "%s\n", str);
		fclose(fp);
	}

	return;
}

void _perror(const char *fmt, ...)
{
	char buf[MAX_STRING_LENGTH]; /* better safe than sorry */
	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	_bug("%s: %s", buf, strerror(errno));
}

/*
 * Reports a bug.
 */
void _bug(const char *str, ...)
{
	char buf[MAX_STRING_LENGTH];
	FILE *fp;
	struct stat fst;

	if (fpArea != NULL)
	{
		int iLine;
		int iChar;

		if (fpArea == stdin)
		{
			iLine = 0;
		}
		else
		{
			iChar = ftell(fpArea);
			fseek(fpArea, 0, 0);
			for (iLine = 0; ftell(fpArea) < iChar; iLine++)
			{
				while (getc(fpArea) != '\n')
					;
			}
			fseek(fpArea, iChar, 0);
		}

		sprintf(buf, "[*****] FILE: %s LINE: %d", strArea, iLine);
		log_string(buf);

		if (stat( SHUTDOWN_FILE, &fst) != -1) /* file exists */
		{
			if ((fp = fopen( SHUTDOWN_FILE, "a")) != NULL)
			{
				fprintf(fp, "[*****] %s\n", buf);
				fclose(fp);
			}
		}
	}

	strcpy(buf, "[*****] BUG: ");
	{
		va_list param;

		va_start(param, str);
		vsprintf(buf + strlen(buf), str, param);
		va_end(param);
	}
//    log_string_plus( buf, LOG_BUG, LEVEL_LOG );
	to_channel(buf, CHANNEL_PROGBUG, FG_GREY MOD_BOLD, 103);

	if (fBootDb)
		boot_log(buf);
	else
		fprintf(stderr, "%s\n", buf);

	LOG_CLOSE;
	if ((fp = fopen( BUG_FILE, "a")) != NULL)
	{
		fprintf(fp, "%s\n", buf);
		fclose(fp);
	}
	LOG_OPEN;

}

/*
 * Add a string to the boot-up log				-Thoric
 */
void boot_log(const char *str, ...)
{
	char buf[MAX_STRING_LENGTH];
	FILE *fp;
	va_list param;

	if (!str_prefix(str, "[*****] BUG: "))
		strcpy(buf, "[*****] BOOT: ");
	else
		strcpy(buf, "");

	va_start(param, str);
	vsprintf(buf + strlen(buf), str, param);
	va_end(param);
	log_string(buf);

	LOG_CLOSE;
	if ((fp = fopen( BOOTLOG_FILE, "a")) != NULL)
	{
		fprintf(fp, "%s\n", buf);
		fclose(fp);
	}
	LOG_OPEN;

	return;
}

/*
 * Dump a text file to a player, a line at a time		-Thoric
 */
void show_file(CHAR_DATA *ch, const char *filename)
{
	FILE *fp;
	char buf[MAX_STRING_LENGTH];
	int c;
	int num = 0;

	if ((fp = fopen(filename, "r")) != NULL)
	{
		while (!feof(fp))
		{
			while ((buf[num] = fgetc(fp)) != EOF && buf[num] != '\n'
					&& buf[num] != '\r' && num < (MAX_STRING_LENGTH - 2))
				num++;

			c = fgetc(fp);

			if ((c != '\n' && c != '\r') || c == buf[num])
				ungetc(c, fp);

			buf[num++] = '\n';
			buf[num++] = '\r';
			buf[num] = '\0';

			send_to_pager(buf, ch);
			num = 0;
		}
		fclose(fp);
	}
}

/*
 * Writes a string to the log, extended version			-Thoric
 */
void log_string_plus(const char *str, int log_type, int level)
{
	int offset;

	if (log_type != LOG_PROG)
		fprintf(stderr, "%s-%s\n",
				static_cast<SWString>(SWTimeStamp(current_time)).c_str(), str);

	if (strncmp(str, "Log ", 4) == 0)
		offset = 4;
	else
		offset = 0;
	switch (log_type)
	{
	default:
		to_channel(str + offset, CHANNEL_LOG, "Log", level);
		break;
	case LOG_BUILD:
		to_channel(str + offset, CHANNEL_BUILD, "Build", level);
		break;
	case LOG_PROG:
		to_channel(str + offset, CHANNEL_PROG, "Prog", level);
		break;
	case LOG_COMM:
		to_channel(str + offset, CHANNEL_COMM, "Comm", level);
		break;
	case LOG_BUG:
		to_channel(str + offset, CHANNEL_LOG, FB_WHITE, level);
		break;
	case LOG_ALL:
		break;
	}
	return;
}

/*
 * wizlist builder!						-Thoric
 *

 void towizfile( const char *line )
 {
 int filler, xx;
 char outline[MAX_STRING_LENGTH];
 FILE *wfp;

 outline[0] = '\0';

 if ( line && line[0] != '\0' )
 {
 filler = ( 78-strlen( line ) );
 if ( filler < 1 )
 filler = 1;
 filler /= 2;
 for ( xx = 0; xx < filler; xx++ )
 strcat( outline, " " );
 strcat( outline, line );
 }
 strcat( outline, NL );
 wfp = fopen( WIZLIST_FILE, "a" );
 if ( wfp )
 {
 fputs( outline, wfp );
 fclose( wfp );
 }
 }

 void add_to_wizlist( char *name, int level, int64 pcflags )
 {
 WIZENT *wiz;

 #ifdef DEBUG
 log_string( "Adding to wizlist..." );
 #endif

 CREATE( wiz, WIZENT, 1 );
 STRDUP( wiz->name, name );
 wiz->level	= level;
 wiz->pcflags	= pcflags;

 //  insert sort, of sorts
 auto it = wiz_list.begin();
 for ( ; it != wiz_list.end(); ++it )
 if ( level > (*it)->level )
 {
 wiz_list.insert(it, wiz);
 return;
 }

 wiz_list.push_back(wiz);
 return;
 }

 *
 * Wizlist builder						-Thoric
 *
 void make_wizlist()
 {
 #if !defined( WIN32 )
 DIR *dp;
 struct dirent *dentry;
 FILE *gfp;
 char *word;
 int ilevel;
 int64 iflags;
 char buf[MAX_STRING_LENGTH];

 wiz_list.clear();

 dp = opendir( GOD_DIR );

 ilevel = 0;
 dentry = readdir( dp );
 while ( dentry )
 {
 if ( dentry->d_name[0] != '.' )
 {
 sprintf( buf, "%s%s", GOD_DIR, dentry->d_name );
 gfp = fopen( buf, "r" );
 if ( gfp )
 {
 word = feof( gfp ) ? (char *)"End" : fread_word( gfp );
 ilevel = fread_number( gfp );
 fread_to_eol( gfp );
 word = feof( gfp ) ? (char *)"End" : fread_word( gfp );
 if ( !str_cmp( word, "Pcflags" ) )
 iflags = fread_number64( gfp );
 else
 iflags = 0;
 fclose( gfp );
 if ( IS_SET( iflags, PCFLAG_GUEST ) )
 ilevel = MAX_LEVEL - 4;
 add_to_wizlist( dentry->d_name, ilevel, iflags );
 }
 }
 dentry = readdir( dp );
 }
 closedir( dp );

 buf[0] = '\0';
 unlink( WIZLIST_FILE );
 towizfile( FG_YELLOW );
 towizfile( "  -= Wielcy SWMUDa! =-" MOD_BOLD );
 towizfile( " " );
 ilevel = 65535;
 towizfile( " Emeryci Muda" );
 for( auto* wiz : wiz_list )
 {
 if( wiz->pcflags & PCFLAG_RETIRED )
 {
 if ( strlen( buf ) + strlen( wiz->name ) > 76 )
 {
 towizfile( buf );
 buf[0] = '\0';
 }
 strcat( buf, " " );
 strcat( buf, wiz->name );
 if ( strlen( buf ) > 70 )
 {
 towizfile( buf );
 buf[0] = '\0';
 }
 }
 }
 for ( auto* wiz : wiz_list )
 {
 if( wiz->pcflags & PCFLAG_RETIRED )
 continue;
 if ( wiz->level > LEVEL_AVATAR )
 {
 if ( wiz->level < ilevel )
 {
 if ( buf[0] )
 {
 towizfile( buf );
 buf[0] = '\0';
 }
 towizfile( "" );
 ilevel = wiz->level;
 switch(ilevel)
 {
 case MAX_LEVEL -  0: towizfile( " W�adcy Absolutni" );		break;
 case MAX_LEVEL -  1: towizfile( " Najwy�szy Konsulat" );	break;
 case MAX_LEVEL -  2: towizfile( " Senat" );			break;
 case MAX_LEVEL -  3: towizfile( " Galaktyczni Zwiadowcy" );	break;
 case MAX_LEVEL -  4: towizfile( " Mo�ni Muda" );		break;
 default:             towizfile( " Zas�u�eni Muda" );		break;
 }
 }
 if ( strlen( buf ) + strlen( wiz->name ) > 76 )
 {
 towizfile( buf );
 buf[0] = '\0';
 }
 strcat( buf, " " );
 strcat( buf, wiz->name );
 if ( strlen( buf ) > 70 )
 {
 towizfile( buf );
 buf[0] = '\0';
 }
 }
 }

 if ( buf[0] )
 towizfile( buf );

 for ( auto* wiz : wiz_list )
 free_wiz( wiz );
 wiz_list.clear();
 #endif
 }


 DEF_DO_FUN( makewizlist )
 {
 make_wizlist();
 }
 */

/* mud prog functions */

/* This routine reads in scripts of MUDprograms from a file */

int64 mprog_name_to_type(char *name)
{
	if (!str_cmp(name, "in_file_prog"))
		return IN_FILE_PROG;
	if (!str_cmp(name, "act_prog"))
		return ACT_PROG;
	if (!str_cmp(name, "speech_prog"))
		return SPEECH_PROG;
	if (!str_cmp(name, "rand_prog"))
		return RAND_PROG;
	if (!str_cmp(name, "fight_prog"))
		return FIGHT_PROG;
	if (!str_cmp(name, "hitprcnt_prog"))
		return HITPRCNT_PROG;
	if (!str_cmp(name, "death_prog"))
		return DEATH_PROG;
	if (!str_cmp(name, "entry_prog"))
		return ENTRY_PROG;
	if (!str_cmp(name, "all_greet_prog"))
		return ALL_GREET_PROG;
	if (!str_cmp(name, "give_prog"))
		return GIVE_PROG;
	if (!str_cmp(name, "bribe_prog"))
		return BRIBE_PROG;
	if (!str_cmp(name, "time_prog"))
		return TIME_PROG;
	if (!str_cmp(name, "hour_prog"))
		return HOUR_PROG;
	if (!str_cmp(name, "wear_prog"))
		return WEAR_PROG;
	if (!str_cmp(name, "remove_prog"))
		return REMOVE_PROG;
	if (!str_cmp(name, "sac_prog"))
		return SAC_PROG;
	if (!str_cmp(name, "tell_prog"))
		return TELL_PROG;
	if (!str_cmp(name, "exa_prog"))
		return EXA_PROG;
	if (!str_cmp(name, "zap_prog"))
		return ZAP_PROG;
	if (!str_cmp(name, "get_prog"))
		return GET_PROG;
	if (!str_cmp(name, "drop_prog"))
		return DROP_PROG;
	if (!str_cmp(name, "damage_prog"))
		return DAMAGE_PROG;
	if (!str_cmp(name, "repair_prog"))
		return REPAIR_PROG;
	if (!str_cmp(name, "greet_prog"))
		return GREET_PROG;
	if (!str_cmp(name, "quest_prog"))
		return QUEST_PROG;
	if (!str_cmp(name, "speechiw_prog"))
		return SPEECHIW_PROG;
	if (!str_cmp(name, "pull_prog"))
		return PULL_PROG;
	if (!str_cmp(name, "push_prog"))
		return PUSH_PROG;
	if (!str_cmp(name, "sleep_prog"))
		return SLEEP_PROG;
	if (!str_cmp(name, "rest_prog"))
		return REST_PROG;
	if (!str_cmp(name, "rfight_prog"))
		return FIGHT_PROG;
	if (!str_cmp(name, "enter_prog"))
		return ENTRY_PROG;
	if (!str_cmp(name, "leave_prog"))
		return LEAVE_PROG;
	if (!str_cmp(name, "rdeath_prog"))
		return DEATH_PROG;
	if (!str_cmp(name, "script_prog"))
		return SCRIPT_PROG;
	if (!str_cmp(name, "use_prog"))
		return USE_PROG;
	if (!str_cmp(name, "extradesc_prog"))
		return EXTRADESC_PROG;
	/* UWAGA !!! Dodane ze wzgl�du na kompatybilno�� z Envy !!! */
	if (!str_cmp(name, "singlehitprcnt_prog"))
		return SPEECH_PROG;
	/* Thanos */
	return ( ERROR_PROG);
}

void mprog_file_read(char *f, MOB_INDEX_DATA *pMobIndex)
{

	char MUDProgfile[ MAX_INPUT_LENGTH];
	FILE *progfile;
	char letter;
	bool done = false;

	sprintf(MUDProgfile, "%s%s", PROG_DIR, f);

	progfile = fopen(MUDProgfile, "r");
	if (!progfile)
	{
		bug("Mob: %d couldn't open mudprog file", pMobIndex->vnum);
		exit(1);
	}

	switch (letter = fread_letter(progfile))
	{
	case '>':
		break;
	case '|':
		bug("empty mudprog file.");
		exit(1);
		break;
	default:
		bug("in mudprog file syntax error.");
		exit(1);
		break;
	}

	while (!done)
	{
		MPROG_DATA *mprg;
		CREATE(mprg, MPROG_DATA, 1);
		mprg->type = mprog_name_to_type(fread_word(progfile));
		switch (mprg->type)
		{
		case ERROR_PROG:
			bug("mudprog file type error");
			exit(1);
			break;
		case IN_FILE_PROG:
			bug("mprog file contains a call to file.");
			exit(1);
			break;
		default:
			pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
			STRDUP(mprg->arglist, st_fread_string(progfile));
			STRDUP(mprg->comlist, st_fread_string(progfile));
			pMobIndex->mudprogs.push_back(mprg);
			switch (letter = fread_letter(progfile))
			{
			case '>':
				break;
			case '|':
				done = true;
				break;
			default:
				bug("in mudprog file syntax error.");
				exit(1);
				break;
			}
			break;
		}
	}
	fclose(progfile);
}

/* Load a MUDprogram section from the area file.
 */
void load_mudprogs(AREA_DATA *tarea, FILE *fp)
{
	MOB_INDEX_DATA *iMob;
	char letter;
	int value;

	for (;;)
		switch (letter = fread_letter(fp))
		{
		default:
			bug("bad command '%c'.", letter);
			exit(1);
			break;
		case 'S':
		case 's':
			fread_to_eol(fp);
			return;
		case '*':
			fread_to_eol(fp);
			break;
		case 'M':
		case 'm':
			value = fread_number(fp);
			if ((iMob = get_mob_index(value)) == NULL)
			{
				bug("vnum %d doesnt exist", value);
				exit(1);
			}

			mprog_file_read(fread_word(fp), iMob);
			fread_to_eol(fp);
			break;
		}

	return;

}

/* This procedure is responsible for reading any in_file MUDprograms.
 */

void mprog_read_programs(FILE *fp, MOB_INDEX_DATA *pMobIndex)
{
	MPROG_DATA *mprg;
	char letter;
	bool done = false;

	if ((letter = fread_letter(fp)) != '>')
	{
		bug("vnum %d MUDPROG char", pMobIndex->vnum);
		exit(1);
	}

	while (!done)
	{
		CREATE(mprg, MPROG_DATA, 1);
		mprg->type = mprog_name_to_type(fread_word(fp));
		switch (mprg->type)
		{
		case ERROR_PROG:
			bug("vnum %d MUDPROG type.", pMobIndex->vnum);
			exit(1);
			break;
		case IN_FILE_PROG:
			mprog_file_read(st_fread_string(fp), pMobIndex);
			fread_to_eol(fp);
			switch (letter = fread_letter(fp))
			{
			case '>':
				break;
			case '|':
				fread_to_eol(fp);
				done = true;
				break;
			default:
				bug("vnum %d bad MUDPROG.", pMobIndex->vnum);
				exit(1);
				break;
			}
			break;
		default:
			pMobIndex->progtypes = pMobIndex->progtypes | mprg->type;
			mprg->arglist = fread_string(fp);
			fread_to_eol(fp);
			mprg->comlist = fread_string(fp);
			fread_to_eol(fp);
			pMobIndex->mudprogs.push_back(mprg);
			switch (letter = fread_letter(fp))
			{
			case '>':
				break;
			case '|':
				fread_to_eol(fp);
				done = true;
				break;
			default:
				bug("vnum %d bad MUDPROG.", pMobIndex->vnum);
				exit(1);
				break;
			}
			break;
		}
	}
	return;
}

/*************************************************************/
/* obj prog functions */
/* This routine transfers between alpha and numeric forms of the
 *  mob_prog bitvector types. This allows the use of the words in the
 *  mob/script files.
 */

/* This routine reads in scripts of OBJprograms from a file */

void oprog_file_read(char *f, OBJ_INDEX_DATA *pObjIndex)
{

	char MUDProgfile[ MAX_INPUT_LENGTH];
	FILE *progfile;
	char letter;
	bool done = false;

	sprintf(MUDProgfile, "%s%s", PROG_DIR, f);

	progfile = fopen(MUDProgfile, "r");
	if (!progfile)
	{
		bug("Obj: %d couldnt open mudprog file", pObjIndex->vnum);
		exit(1);
	}

	switch (letter = fread_letter(progfile))
	{
	case '>':
		break;
	case '|':
		bug("empty objprog file.");
		exit(1);
		break;
	default:
		bug("in objprog file syntax error.");
		exit(1);
		break;
	}

	while (!done)
	{
		MPROG_DATA *mprg;
		CREATE(mprg, MPROG_DATA, 1);
		mprg->type = mprog_name_to_type(fread_word(progfile));
		switch (mprg->type)
		{
		case ERROR_PROG:
			bug("objprog file type error");
			exit(1);
			break;
		case IN_FILE_PROG:
			bug("objprog file contains a call to file.");
			exit(1);
			break;
		default:
			pObjIndex->progtypes = pObjIndex->progtypes | mprg->type;
			STRDUP(mprg->arglist, st_fread_string(progfile));
			STRDUP(mprg->comlist, st_fread_string(progfile));
			pObjIndex->mudprogs.push_back(mprg);
			switch (letter = fread_letter(progfile))
			{
			case '>':
				break;
			case '|':
				done = true;
				break;
			default:
				bug("in objprog file syntax error.");
				exit(1);
				break;
			}
			break;
		}
	}
	fclose(progfile);
}

/* Load a MUDprogram section from the area file.
 */
void load_objprogs(AREA_DATA *tarea, FILE *fp)
{
	OBJ_INDEX_DATA *iObj;
	char letter;
	int value;

	for (;;)
		switch (letter = fread_letter(fp))
		{
		default:
			bug("bad command '%c'.", letter);
			exit(1);
			break;
		case 'S':
		case 's':
			fread_to_eol(fp);
			return;
		case '*':
			fread_to_eol(fp);
			break;
		case 'M':
		case 'm':
			value = fread_number(fp);
			if ((iObj = get_obj_index(value)) == NULL)
			{
				bug("vnum %d doesnt exist", value);
				exit(1);
			}

			oprog_file_read(fread_word(fp), iObj);
			fread_to_eol(fp);
			break;
		}

	return;

}

/* This procedure is responsible for reading any in_file OBJprograms.
 */

void oprog_read_programs(FILE *fp, OBJ_INDEX_DATA *pObjIndex)
{
	MPROG_DATA *mprg;
	char letter;
	bool done = false;

	if ((letter = fread_letter(fp)) != '>')
	{
		bug("Load_objects: vnum %d OBJPROG char", pObjIndex->vnum);
		exit(1);
	}

	while (!done)
	{
		CREATE(mprg, MPROG_DATA, 1);
		mprg->type = mprog_name_to_type(fread_word(fp));
		switch (mprg->type)
		{
		case ERROR_PROG:
			bug("Load_objects: vnum %d OBJPROG type.", pObjIndex->vnum);
			exit(1);
			break;
		case IN_FILE_PROG:
			oprog_file_read(st_fread_string(fp), pObjIndex);
			fread_to_eol(fp);
			switch (letter = fread_letter(fp))
			{
			case '>':
				break;
			case '|':
				fread_to_eol(fp);
				done = true;
				break;
			default:
				bug("Load_objects: vnum %d bad OBJPROG.", pObjIndex->vnum);
				exit(1);
				break;
			}
			break;
		default:
			pObjIndex->progtypes = pObjIndex->progtypes | mprg->type;
			mprg->arglist = fread_string(fp);
			fread_to_eol(fp);
			mprg->comlist = fread_string(fp);
			fread_to_eol(fp);
			pObjIndex->mudprogs.push_back(mprg);
			switch (letter = fread_letter(fp))
			{
			case '>':
				break;
			case '|':
				fread_to_eol(fp);
				done = true;
				break;
			default:
				bug("Load_objects: vnum %d bad OBJPROG.", pObjIndex->vnum);
				exit(1);
				break;
			}
			break;
		}
	}

	return;

}

/*************************************************************/
/* room prog functions */
/* This routine transfers between alpha and numeric forms of the
 *  mob_prog bitvector types. This allows the use of the words in the
 *  mob/script files.
 */

/* This routine reads in scripts of OBJprograms from a file */
void rprog_file_read(char *f, ROOM_INDEX_DATA *RoomIndex)
{

	char MUDProgfile[ MAX_INPUT_LENGTH];
	FILE *progfile;
	char letter;
	bool done = false;

	sprintf(MUDProgfile, "%s%s", PROG_DIR, f);

	progfile = fopen(MUDProgfile, "r");
	if (!progfile)
	{
		bug("Room: %d couldnt open roomprog file", RoomIndex->vnum);
		exit(1);
	}

	switch (letter = fread_letter(progfile))
	{
	case '>':
		break;
	case '|':
		bug("empty roomprog file.");
		exit(1);
		break;
	default:
		bug("in roomprog file syntax error.");
		exit(1);
		break;
	}

	while (!done)
	{
		MPROG_DATA *mprg;
		CREATE(mprg, MPROG_DATA, 1);
		mprg->type = mprog_name_to_type(fread_word(progfile));
		switch (mprg->type)
		{
		case ERROR_PROG:
			bug("roomprog file type error");
			exit(1);
			break;
		case IN_FILE_PROG:
			bug("roomprog file contains a call to file.");
			exit(1);
			break;
		default:
			RoomIndex->progtypes = RoomIndex->progtypes | mprg->type;
			STRDUP(mprg->arglist, st_fread_string(progfile));
			STRDUP(mprg->comlist, st_fread_string(progfile));
			RoomIndex->mudprogs.push_back(mprg);
			switch (letter = fread_letter(progfile))
			{
			case '>':
				break;
			case '|':
				done = true;
				break;
			default:
				bug("in roomprog file syntax error.");
				exit(1);
				break;
			}
			break;
		}
	}
	fclose(progfile);
}

/* Load a ROOMprogram section from the area file.
 */
void load_roomprogs(AREA_DATA *tarea, FILE *fp)
{
	ROOM_INDEX_DATA *iRoom;
	char letter;
	int value;

	for (;;)
		switch (letter = fread_letter(fp))
		{
		default:
			bug("bad command '%c'.", letter);
			exit(1);
			break;
		case 'S':
		case 's':
			fread_to_eol(fp);
			return;
		case '*':
			fread_to_eol(fp);
			break;
		case 'M':
		case 'm':
			value = fread_number(fp);
			if ((iRoom = get_room_index(value)) == NULL)
			{
				bug("vnum %d doesnt exist", value);
				exit(1);
			}

			rprog_file_read(fread_word(fp), iRoom);
			fread_to_eol(fp);
			break;
		}

	return;

}

/* This procedure is responsible for reading any in_file ROOMprograms.
 */

void rprog_read_programs(FILE *fp, ROOM_INDEX_DATA *pRoomIndex)
{
	MPROG_DATA *mprg;
	char letter;
	bool done = false;

	if ((letter = fread_letter(fp)) != '>')
	{
		bug("vnum %d ROOMPROG char!='>' (%c)", pRoomIndex->vnum, letter);
		exit(1);
	}

	while (!done)
	{
		CREATE(mprg, MPROG_DATA, 1);
		mprg->type = mprog_name_to_type(fread_word(fp));
		switch (mprg->type)
		{
		case ERROR_PROG:
			bug("vnum %d ROOMPROG type.", pRoomIndex->vnum);
			exit(1);
			break;
		case IN_FILE_PROG:
			rprog_file_read(st_fread_string(fp), pRoomIndex);
			fread_to_eol(fp);
			switch (letter = fread_letter(fp))
			{
			case '>':
				break;
			case '|':
				fread_to_eol(fp);
				done = true;
				break;
			default:
				bug("vnum %d bad ROOMPROG.", pRoomIndex->vnum);
				exit(1);
				break;
			}
			break;
		default:
			pRoomIndex->progtypes = pRoomIndex->progtypes | mprg->type;
			mprg->arglist = fread_string(fp);
			fread_to_eol(fp);
			mprg->comlist = fread_string(fp);
			fread_to_eol(fp);
			pRoomIndex->mudprogs.push_back(mprg);
			switch (letter = fread_letter(fp))
			{
			case '>':
				break;
			case '|':
				fread_to_eol(fp);
				done = true;
				break;
			default:
				bug("vnum %d bad ROOMPROG.", pRoomIndex->vnum);
				exit(1);
				break;
			}
			break;
		}
	}

	return;

}

/*! Trog: usuwanie room'a z room_index_hash */
void unlink_room(RID *room)
{
	IF_BUG(room == NULL, "NULL room")
		return;

	int iHash = room->vnum % MAX_KEY_HASH;
	room_index_hash[iHash].remove(room);
}

/*! Trog: sprawdzanie czy room jest w room_index_hash */
bool is_room_unlinked(RID *room)
{
	IF_BUG(room == NULL, "NULL room")
		return true;

	int iHash = room->vnum % MAX_KEY_HASH;
	for (auto* rid : room_index_hash[iHash])
		if (rid == room)
			return false;

	return true;
}

/*************************************************************/
/* Function to delete a room index.  Called from do_rdelete in build.c
 Narn, May/96
 */
bool delete_room(ROOM_INDEX_DATA *room)
{
	int iHash;

	bug("delete_room used");

	iHash = room->vnum % MAX_KEY_HASH;

	/* Take the room index out of the hash list. */
	auto& bucket = room_index_hash[iHash];
	auto prev = bucket.before_begin();
	bool found = false;
	for (auto it = bucket.begin(); it != bucket.end(); ++it)
	{
		if (*it == room)
		{
			bucket.erase_after(prev);
			found = true;
			break;
		}
		prev = it;
	}
	if (!found)
	{
		bug("room not found");
		return false;
	}

	free_room(room);
	top_room--;
	return true;
}

/* See comment on delete_room. */
bool delete_obj(OBJ_INDEX_DATA *obj)
{
	return true;
}

/* See comment on delete_room. */
bool delete_mob(MOB_INDEX_DATA *mob)
{
	return true;
}

/*
 * Creat a new room (for online building)			-Thoric
 */
ROOM_INDEX_DATA* make_room(int vnum, int svnum)
{
	ROOM_INDEX_DATA *pRoomIndex;
	int iHash;

	CREATE(pRoomIndex, ROOM_INDEX_DATA, 1);
	pRoomIndex->area = NULL;
	pRoomIndex->vnum = vnum;
	pRoomIndex->svnum = svnum;	//Thanos -- do statkow
	STRDUP(pRoomIndex->name, "Pustka");
	STRDUP(pRoomIndex->homename, "");
	STRDUP(pRoomIndex->description, "");
	STRDUP(pRoomIndex->nightdesc, ""); //added by Trog
	pRoomIndex->room_flags = ROOM_PROTOTYPE;
	pRoomIndex->sector_type = 1;
	pRoomIndex->light = 0;

	CREATE(pRoomIndex->variables, VAR_DATA, 1);	// Ratm mem alloc dla zmiennych
	bzero(pRoomIndex->variables, sizeof(VAR_DATA)); // w progach

	iHash = vnum % MAX_KEY_HASH;
	room_index_hash[iHash].push_front(pRoomIndex);

	if (vnum)
		top_room++;
	else
		top_vroom++;

	return pRoomIndex;
}

/*
 * Create a new INDEX object (for online building)		-Thoric
 * Option to clone an existing index object.
 */
OBJ_INDEX_DATA* make_object(int vnum, int cvnum, char *name)
{
	OBJ_INDEX_DATA *pObjIndex, *cObjIndex;
	char buf[MAX_STRING_LENGTH];
	int iHash;
	int i;
	if (cvnum > 0)
		cObjIndex = get_obj_index(cvnum);
	else
		cObjIndex = NULL;
	CREATE(pObjIndex, OBJ_INDEX_DATA, 1);
	pObjIndex->vnum = vnum;
	STRDUP(pObjIndex->name, name);
	if (!cObjIndex)
	{
		sprintf(buf, "%s", name);

		for (i = 0; i < 6; i++)
			STRDUP(pObjIndex->przypadki[i], buf);

		sprintf(buf, "Le�y tu %s.", name);
		STRDUP(pObjIndex->description, buf);
		STRDUP(pObjIndex->action_desc, "");
		pObjIndex->przypadki[0][0] = LOWER(pObjIndex->przypadki[0][0]);
		pObjIndex->description[0] = UPPER(pObjIndex->description[0]);
		pObjIndex->item_type = ITEM_TRASH;
		pObjIndex->extra_flags = ITEM_PROTOTYPE;
		pObjIndex->wear_flags = 0;
		pObjIndex->value[0] = 0;
		pObjIndex->value[1] = 0;
		pObjIndex->value[2] = 0;
		pObjIndex->value[3] = 0;
		pObjIndex->value[4] = 0;
		pObjIndex->value[5] = 0;
		pObjIndex->weight = 1;
		pObjIndex->cost = 0;
		pObjIndex->gender = GENDER_MALE; //added by Thanos
	}
	else
	{
		EXTRA_DESCR_DATA *ed;
		AFFECT_DATA *paf;

		for (i = 0; i < 6; i++)
			STRDUP(pObjIndex->przypadki[i], cObjIndex->przypadki[i]);
		STRDUP(pObjIndex->description, cObjIndex->description);
		STRDUP(pObjIndex->action_desc, cObjIndex->action_desc);
		pObjIndex->item_type = cObjIndex->item_type;
		pObjIndex->extra_flags = cObjIndex->extra_flags | ITEM_PROTOTYPE;
		pObjIndex->wear_flags = cObjIndex->wear_flags;
		pObjIndex->value[0] = cObjIndex->value[0];
		pObjIndex->value[1] = cObjIndex->value[1];
		pObjIndex->value[2] = cObjIndex->value[2];
		pObjIndex->value[3] = cObjIndex->value[3];
		pObjIndex->value[4] = cObjIndex->value[4];
		pObjIndex->value[5] = cObjIndex->value[5];
		pObjIndex->weight = cObjIndex->weight;
		pObjIndex->cost = cObjIndex->cost;
		//added by Thanos
		pObjIndex->gender = cObjIndex->gender;
		for (auto* ced : cObjIndex->extradesc)
		{
			CREATE(ed, EXTRA_DESCR_DATA, 1);
			STRDUP(ed->keyword, ced->keyword);
			STRDUP(ed->description, ced->description);
			pObjIndex->extradesc.push_back(ed);
			top_ed++;
		}
		for (auto* cpaf : cObjIndex->affects)
		{
			CREATE(paf, AFFECT_DATA, 1);
			paf->type = cpaf->type;
			paf->duration = cpaf->duration;
			paf->location = cpaf->location;
			paf->modifier = cpaf->modifier;
			paf->bitvector = cpaf->bitvector;
			pObjIndex->affects.push_back(paf);
			top_affect++;
		}
	}
	pObjIndex->count = 0;
	iHash = vnum % MAX_KEY_HASH;
	obj_index_hash[iHash].push_front(pObjIndex);
	top_obj_index++;

	return pObjIndex;
}

/*
 * Create a new INDEX mobile (for online building)		-Thoric
 * Option to clone an existing index mobile.
 */
MOB_INDEX_DATA* make_mobile(int vnum, int cvnum, char *name)
{
	MOB_INDEX_DATA *pMobIndex, *cMobIndex;
	char buf[MAX_STRING_LENGTH];
	int iHash;
	int i;

	if (cvnum > 0)
		cMobIndex = get_mob_index(cvnum);
	else
		cMobIndex = NULL;
	CREATE(pMobIndex, MOB_INDEX_DATA, 1);
	pMobIndex->vnum = vnum;
	pMobIndex->count = 0;
	pMobIndex->killed = 0;
	STRDUP(pMobIndex->player_name, name);
	if (!cMobIndex)
	{
		sprintf(buf, "%s", name);
		for (i = 0; i < 6; i++)
			STRDUP(pMobIndex->przypadki[i], buf);

		sprintf(buf, "Stoi tu %s i niebardzo wie co ma pocz��." NL, name);
		STRDUP(pMobIndex->long_descr, buf);
		STRDUP(pMobIndex->s_vip_flags, "");

		STRDUP(pMobIndex->description, "");
		pMobIndex->przypadki[0][0] = LOWER(pMobIndex->przypadki[0][0]);
		pMobIndex->long_descr[0] = UPPER(pMobIndex->long_descr[0]);
		pMobIndex->description[0] = UPPER(pMobIndex->description[0]);
		pMobIndex->act = ACT_IS_NPC | ACT_PROTOTYPE;
		pMobIndex->affected_by = 0;
		pMobIndex->pShop = NULL;
		pMobIndex->rShop = NULL;
		pMobIndex->spec_fun = NULL;
		pMobIndex->spec_2 = NULL;
		pMobIndex->progtypes = 0;
		pMobIndex->alignment = 0;
		pMobIndex->level = 1;
		pMobIndex->mobthac0 = 0;
		pMobIndex->ac = 0;
		pMobIndex->hitnodice = 0;
		pMobIndex->hitsizedice = 0;
		pMobIndex->hitplus = 0;
		pMobIndex->damnodice = 0;
		pMobIndex->damsizedice = 0;
		pMobIndex->damplus = 0;
		pMobIndex->gold = 0;
		pMobIndex->exp = 0;
		pMobIndex->position = 8;
		pMobIndex->defposition = 8;
		pMobIndex->sex = 0;
		pMobIndex->perm_str = 13;
		pMobIndex->perm_dex = 13;
		pMobIndex->perm_int = 13;
		pMobIndex->perm_wis = 13;
		pMobIndex->perm_cha = 13;
		pMobIndex->perm_con = 13;
		pMobIndex->perm_lck = 13;
		pMobIndex->race = base_race;
		pMobIndex->xflags = 0;
		pMobIndex->resistant = 0;
		pMobIndex->immune = 0;
		pMobIndex->susceptible = 0;
		pMobIndex->numattacks = 0;
		pMobIndex->attacks = 0;
		pMobIndex->defenses = 0 | DFND_DODGE | DFND_PARRY;
		pMobIndex->speaking = lang_base;
	}
	else
	{
		for (i = 0; i < 6; i++)
			STRDUP(pMobIndex->przypadki[i], cMobIndex->przypadki[i]);
		STRDUP(pMobIndex->long_descr, cMobIndex->long_descr);
		STRDUP(pMobIndex->description, cMobIndex->description);
		STRDUP(pMobIndex->s_vip_flags, cMobIndex->s_vip_flags);
		pMobIndex->act = cMobIndex->act | ACT_PROTOTYPE;
		pMobIndex->affected_by = cMobIndex->affected_by;
		pMobIndex->pShop = NULL;
		pMobIndex->rShop = NULL;
		pMobIndex->spec_fun = cMobIndex->spec_fun;
		pMobIndex->spec_2 = cMobIndex->spec_2;
		pMobIndex->progtypes = 0;
		pMobIndex->alignment = cMobIndex->alignment;
		pMobIndex->level = cMobIndex->level;
		pMobIndex->mobthac0 = cMobIndex->mobthac0;
		pMobIndex->ac = cMobIndex->ac;
		pMobIndex->hitnodice = cMobIndex->hitnodice;
		pMobIndex->hitsizedice = cMobIndex->hitsizedice;
		pMobIndex->hitplus = cMobIndex->hitplus;
		pMobIndex->damnodice = cMobIndex->damnodice;
		pMobIndex->damsizedice = cMobIndex->damsizedice;
		pMobIndex->damplus = cMobIndex->damplus;
		pMobIndex->gold = cMobIndex->gold;
		pMobIndex->exp = cMobIndex->exp;
		pMobIndex->position = cMobIndex->position;
		pMobIndex->defposition = cMobIndex->defposition;
		pMobIndex->sex = cMobIndex->sex;
		pMobIndex->perm_str = cMobIndex->perm_str;
		pMobIndex->perm_dex = cMobIndex->perm_dex;
		pMobIndex->perm_int = cMobIndex->perm_int;
		pMobIndex->perm_wis = cMobIndex->perm_wis;
		pMobIndex->perm_cha = cMobIndex->perm_cha;
		pMobIndex->perm_con = cMobIndex->perm_con;
		pMobIndex->perm_lck = cMobIndex->perm_lck;
		pMobIndex->race = cMobIndex->race;
		pMobIndex->xflags = cMobIndex->xflags;
		pMobIndex->resistant = cMobIndex->resistant;
		pMobIndex->immune = cMobIndex->immune;
		pMobIndex->susceptible = cMobIndex->susceptible;
		pMobIndex->numattacks = cMobIndex->numattacks;
		pMobIndex->attacks = cMobIndex->attacks;
		pMobIndex->defenses = cMobIndex->defenses;
		pMobIndex->speaking = cMobIndex->speaking;
	}
	iHash = vnum % MAX_KEY_HASH;
	mob_index_hash[iHash].push_front(pMobIndex);
	top_mob_index++;

	/* Trog
	 #warning Tanglor! alokuj tutaj pamiec na rzeczy typu dialog_name
	 */
	STRDUP(pMobIndex->dialog_name, "");

	return pMobIndex;
}

/*
 * Creates a simple exit with no fields filled but rvnum and optionally
 * to_room and vnum.						-Thoric
 * Exits are inserted into the linked list based on vdir.
 */
EXIT_DATA* make_exit(ROOM_INDEX_DATA *pRoomIndex, ROOM_INDEX_DATA *to_room,
		int door)
{
	EXIT_DATA *pexit, *texit;

	CREATE(pexit, EXIT_DATA, 1);
	pexit->vdir = door;
	pexit->rvnum = pRoomIndex->vnum;
	pexit->to_room = to_room;
	pexit->distance = 1;
	if (to_room)
	{
		pexit->vnum = to_room->vnum;
		texit = get_exit_to(to_room, rev_dir[door], pRoomIndex->vnum);
		if (texit) /* assign reverse exit pointers */
		{
			texit->rexit = pexit;
			pexit->rexit = texit;
		}
	}
	/* keep exits in incremental order - insert exit into list */
	for (auto it = pRoomIndex->exits.begin(); it != pRoomIndex->exits.end(); ++it)
	{
		if (door < (*it)->vdir)
		{
			pRoomIndex->exits.insert(it, pexit);
			top_exit++;
			return pexit;
		}
	}
	pRoomIndex->exits.push_back(pexit);
	top_exit++;
	return pexit;
}

void fix_area_exits(AREA_DATA *tarea)
{
	ROOM_INDEX_DATA *pRoomIndex;
	EXIT_DATA *rev_exit;
	int rnum;
	bool fexit;

	for (rnum = tarea->uvnum; rnum <= tarea->uvnum; rnum++)
	{
		if ((pRoomIndex = get_room_index(rnum)) == NULL)
			continue;

		fexit = false;
		for (auto* pexit : pRoomIndex->exits)
		{
			fexit = true;
			pexit->rvnum = pRoomIndex->vnum;
			if (pexit->vnum <= 0)
				pexit->to_room = NULL;
			else
				pexit->to_room = get_room_index(pexit->vnum);
		}
		if (!fexit)
			SET_BIT(pRoomIndex->room_flags, ROOM_NO_MOB);
	}

	for (rnum = tarea->lvnum; rnum <= tarea->uvnum; rnum++)
	{
		if ((pRoomIndex = get_room_index(rnum)) == NULL)
			continue;

		for (auto* pexit : pRoomIndex->exits)
		{
			if (pexit->to_room && !pexit->rexit)
			{
				rev_exit = get_exit_to(pexit->to_room, rev_dir[pexit->vdir],
						pRoomIndex->vnum);
				if (rev_exit)
				{
					pexit->rexit = rev_exit;
					rev_exit->rexit = pexit;
				}
			}
		}
	}
}

/*Added by Thanos*/
void fread_init_cmd(QUEST_CMND_DATA *init_cmd, FILE *fp)
{
	char *word;
	int cmd;

	word = fread_word(fp);

	if ((cmd = flag_value(quest_init_types, word)) < 0)
	{
		fread_to_eol(fp);
		bug("Unknown Init Cmd: %s", word);
		return;
	}

	init_cmd->command = cmd;
	init_cmd->arg1 = fread_number(fp);
	init_cmd->arg2 = fread_number(fp);
	init_cmd->arg3 = fread_number(fp);
	init_cmd->arg4 = fread_string(fp);

	return;
}

void fread_event_cmd(QUEST_CMND_DATA *event_cmd, FILE *fp)
{
	char *word;
	int cmd;

	word = fread_word(fp);

	if ((cmd = flag_value(quest_event_types, word)) < 0)
	{
		fread_to_eol(fp);
		bug("Unknown Event: %s", word);
		return;
	}

	event_cmd->command = cmd;
	event_cmd->arg1 = fread_number(fp);
	event_cmd->arg2 = fread_number(fp);
	event_cmd->arg3 = fread_number(fp);
	event_cmd->arg4 = fread_string(fp);

	return;
}

void fread_action_cmd(QUEST_ACTION_DATA *action_cmd, FILE *fp)
{
	char *word;
	int cmd;

	word = fread_word(fp);

	if ((cmd = flag_value(quest_action_types, word)) < 0)
	{
		fread_to_eol(fp);
		bug("Unknown Action: %s", word);
		return;
	}

	action_cmd->command = cmd;
	action_cmd->arg1 = fread_number(fp);
	action_cmd->arg2 = fread_number(fp);
	action_cmd->arg3 = fread_number(fp);
	action_cmd->arg4 = fread_string(fp);
	action_cmd->done = false;

	return;
}

void fread_quest_header(QUEST_INDEX_DATA *quest, FILE *fp)
{
	char *word;
	bool fMatch;

	for (;;)
	{
		word = feof(fp) ? (char*) "End" : fread_word(fp);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;
		case 'A':
			SKEY("Author", quest->author, fread_string(fp))
			;
			if (!str_cmp(word, "Action"))
			{
				QUEST_ACTION_DATA *action_cmd;

				CREATE(action_cmd, QUEST_ACTION_DATA, 1);
				fread_action_cmd(action_cmd, fp);
				quest->actions.push_back(action_cmd);
				fMatch = true;
				break;
			}
			break;
		case 'D':
			SKEY("Desc", quest->description, fread_string(fp))
			;
			break;
		case 'E':
			if (!str_cmp(word, "End"))
				return;
			break;
		case 'F':
			KEY("Flags", quest->flags, fread_number64(fp))
			;
			break;
		case 'I':
			if (!str_cmp(word, "Init"))
			{
				QUEST_CMND_DATA *init_cmd;

				CREATE(init_cmd, QUEST_CMND_DATA, 1);
				fread_init_cmd(init_cmd, fp);
				quest->init_cmds.push_back(init_cmd);
				fMatch = true;
				break;
			}
		case 'M':
			KEY("MaxLevel", quest->max_lev, fread_number(fp))
			;
			KEY("MinLevel", quest->min_lev, fread_number(fp))
			;
			break;
		case 'N':
			SKEY("Name", quest->name, fread_string(fp))
			;
			break;
		case 'P':
			KEY("Penalty", quest->penalty, fread_number(fp))
			;
			break;
		case 'Q':
			KEY("Questor", quest->questor, fread_number(fp))
			;
			break;
		case 'S':
			KEY("Security", quest->security, fread_number(fp))
			;
			break;
		case 'T':
			KEY("Time", quest->time, fread_number(fp))
			;
			break;
		}

		if (!fMatch)
		{
			bug("no match: %s", word);
			fread_to_eol(fp);
		}

	}
	return;
}

void fread_quest_chapter(QUEST_INDEX_DATA *quest, FILE *fp)
{
	char *word;
	bool fMatch;
	CHAPTER_INDEX_DATA *chapter;

	CREATE(chapter, CHAPTER_INDEX_DATA, 1);
	chapter->type = CHAPTER_CRITICAL;
	STRDUP(chapter->name, fread_word(fp));

	for (;;)
	{
		word = feof(fp) ? (char*) "End" : fread_word(fp);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;
		case 'A':
			if (!str_cmp(word, "Action"))
			{
				QUEST_ACTION_DATA *action_cmd;

				CREATE(action_cmd, QUEST_ACTION_DATA, 1);
				fread_action_cmd(action_cmd, fp);
				chapter->actions.push_back(action_cmd);
				fMatch = true;
				break;
			}
		case 'E':
			if (!str_cmp(word, "End"))
			{
				quest->chapters.push_back(chapter);
				return;
			}
			if (!str_cmp(word, "Event"))
			{
				QUEST_CMND_DATA *event_cmd;

				CREATE(event_cmd, QUEST_CMND_DATA, 1);
				fread_event_cmd(event_cmd, fp);
				chapter->events.push_back(event_cmd);
				fMatch = true;
				break;
			}
			break;
		case 'I':
			if (!str_cmp(word, "Init"))
			{
				QUEST_CMND_DATA *init_cmd;

				CREATE(init_cmd, QUEST_CMND_DATA, 1);
				fread_init_cmd(init_cmd, fp);
				chapter->init_cmds.push_back(init_cmd);
				fMatch = true;
				break;
			}
		case 'T':
			KEY("Type", chapter->type, fread_number(fp))
			;
			break;
		}

		if (!fMatch)
		{
			bug("no match: '%s' (quest: %d)", word, quest->quest_id);
			fread_to_eol(fp);
		}

	}
	return;
}

bool load_quest_file(char *filename)
{
	QUEST_INDEX_DATA *quest;
	FILE *fp;
	char patch[256];

	sprintf(patch, "%s%s", QINDEX_DIR, filename);
	if ((fp = fopen(patch, "r")) == NULL)
	{
		log_string("Coulnd't open quest file");
		perror(patch);
		return false;
	}

	CREATE(quest, QUEST_INDEX_DATA, 1);
	for (;;)
	{
		char letter;
		char *word;
		bool fMatch = false;

		letter = fread_letter(fp);
		if (letter == '*')
		{
			fread_to_eol(fp);
			continue;
		}

		if (letter != '#')
		{
			bug("No '#' in #QUESTS", 0);
			exit(1);
		}

		word = fread_word(fp);

		if ( is_number(word))
		{
			quest->quest_id = atoi(word);
			fread_quest_header(quest, fp);
			fMatch = true;
		}
		else if (!str_cmp(word, "QUEST"))
		{
			fMatch = true;
		}
		else if (!str_cmp(word, "CHAPTER"))
		{
			fread_quest_chapter(quest, fp);
			fMatch = true;
		}
		else if (!str_cmp(word, "ENDING"))
		{
			fread_quest_chapter(quest, fp);
			fMatch = true;
		}
		else if (!str_cmp(word, "END"))
		{
			fclose(fp);
			quest_index_list.push_back(quest);
			top_quest_index++;
			return true;
		}

		if (!fMatch)
		{
			bug("no match: %s", word);
		}
	}
	fclose(fp);
	return false;
}

void load_quests()
{
	char *filename;
	FILE *fpList;
	char buf[MSL];

	RESERVE_CLOSE;
	if ((fpList = fopen( QUEST_LIST, "r")) == NULL)
	{
		log_string("Coulnd't open QUEST_LIST file");
		perror( QUEST_LIST);
	}
	else
	{
		for (;;)
		{
			filename = feof(fpList) ? (char*) "$" : fread_word(fpList);

			if (!sysdata.silent)
				log_string(filename);
			if (filename[0] == '$')
				break;

			if (!load_quest_file(filename))
			{
				sprintf(buf, "Cannot load quest file: %s", filename);
				log_string(buf);
			}
		}
		fclose(fpList);
		if (!sysdata.silent)
			log_string(" Done Quests");
	}
	RESERVE_OPEN;
	return;
}
//done

void load_clonings(void)
{
	FILE *fp;
	CLONING_DATA *cloning;

	if ((fp = fopen( CLONINGS_FILE, "r")) == NULL)
	{
		log_string("No Clonings File - NOT GOOD!");
		return;
	}

	for (;;)
	{
		char letter;

		letter = feof(fp) ? '$' : fread_letter(fp);

		if (letter == '*')
		{
			fread_to_eol(fp);
			continue;
		}
		else if (letter == '$')
			break;
		else if (letter == '#')
		{
			ROOM_INDEX_DATA *entrance;
			ROOM_INDEX_DATA *cylinder;
			ROOM_INDEX_DATA *leaving;

			if ((entrance = get_room_index(fread_number(fp))) != NULL)
			{
				if ((cylinder = get_room_index(fread_number(fp))) != NULL)
				{
					if ((leaving = get_room_index(fread_number(fp))) != NULL)
					{
						CREATE(cloning, CLONING_DATA, 1);
						cloning->entrance = entrance;
						cloning->cylinder = cylinder;
						cloning->leaving = leaving;
						cloning_list.push_back(cloning);
						top_cloning++;
					}
				}
			}
			fread_to_eol(fp);
		}
		else
		{
			log_string("UNKNOWN CHARACTER FOUND");
			fread_to_eol(fp);
		}
	}
	fclose(fp);
	if (!sysdata.silent)
		log_string(" Done clonings");
	return;
}

/*
 * Sort by room vnums					-Altrag & Thoric
 */
void sort_area(AREA_DATA *pArea, bool proto)
{
	if (!pArea)
	{
		bug("NULL pArea");
		return;
	}

	std::list<AREA_DATA*>& sort_list = proto ? bsort_list : asort_list;

	/* Insert in sorted order by lvnum */
	for (auto it = sort_list.begin(); it != sort_list.end(); ++it)
	{
		if (pArea->lvnum < (*it)->lvnum)
		{
			sort_list.insert(it, pArea);
			return;
		}
	}
	sort_list.push_back(pArea);
}

/*
 * Display vnums currently assigned to areas		-Altrag & Thoric
 * Sorted, and flagged if loaded.
 */
void show_vnums(CHAR_DATA *ch, int low, int high, bool proto, bool shownl,
		char *loadst, char *notloadst)
{
	int count, loaded;

	count = 0;
	loaded = 0;
	auto& sort_list_ref = proto ? bsort_list : asort_list;
	for (auto* pArea : sort_list_ref)
	{
		if (IS_SET(pArea->status, AREA_DELETED))
			continue;
		if (pArea->lvnum < low)
			continue;
		if (pArea->uvnum > high)
			break;
		if (IS_SET(pArea->status, AREA_LOADED))
			loaded++;
		else if (!shownl)
			continue;
		pager_printf(ch, "%-15s| Rooms: %5d - %-5d"
				" Objs: %5d - %-5d Mobs: %5d - %-5d%s" NL,
				(pArea->filename ? pArea->filename : "(invalid)"), pArea->lvnum,
				pArea->uvnum, pArea->lvnum, pArea->uvnum, pArea->lvnum,
				pArea->uvnum,
				IS_SET(pArea->status, AREA_LOADED) ? loadst : notloadst);
		count++;
	}
	pager_printf(ch, "Areas listed: %d  Loaded: %d" NL, count, loaded);
	return;
}

/* Trog: wyswietlanie krain. */
DEF_DO_FUN( alist )
{
	int i, low, high;
	char arg1[MIL];
	char arg2[MIL];
	bool sort = false;
	bool author = false;
	bool name = false;
	bool builder = false;
	bool file = false;
	SWString arg2str;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	low = 0;
	high = MAX_VNUM - 1;

	if (*arg1)
	{
		SWString cmd(arg1);

		if (cmd == "sort"
				|| (cmd == "range" && (low = atoi(arg2)) > 0
						&& (high = atoi(argument)) > low))
		{
			sort = true;
		}
		else if (cmd == "name" && *arg2)
		{
			name = true;
			arg2str = SWString(arg2).toLower();
		}
		else if (cmd == "author" && *arg2)
		{
			author = true;
			arg2str = SWString(arg2).toLower();
		}
		else if (cmd == "builder" && *arg2)
		{
			builder = true;
			arg2str = SWString(arg2).toLower();
		}
		else if (cmd == "file" && *arg2)
		{
			file = true;
			arg2str = SWString(arg2);
		}
		else
		{
			send_to_char("Syntax: alist" NL
			"        alist sort" NL
			"        alist range <lvnum> <uvnum>" NL
			"        alist name <name>" NL
			"        alist author <author>" NL
			"        alist builder <builder>" NL
			"        alist file <fileName>" NL NL
			"Note: lvnum < uvnum!" NL, ch);
			return;
		}
	}

	pager_printf(ch,
	FB_CYAN "[" FB_YELLOW "Nr " FB_CYAN "]"
	"[" FB_YELLOW "Area Name           " FB_CYAN "]"
	"[" FB_YELLOW "Vnums      " FB_CYAN "]"
	"[" FB_YELLOW "Sec" FB_CYAN "]"
	"[" FB_YELLOW "Builders        " FB_CYAN "]"
	"[" FB_YELLOW "Filename      " FB_CYAN "]" EOL);

	{
	auto& alist_ref = sort ? asort_list : area_list;
	i = 1;
	for (auto* area : alist_ref)
	{
		if (area->lvnum < low || area->uvnum > high
				|| (name
						&& SWString(area->name).toLower().find(arg2str)
								== SWString::npos)
				|| (author
						&& SWString(area->author).toLower().find(arg2str)
								== SWString::npos)
				|| (builder
						&& SWString(area->builder).toLower().find(arg2str)
								== SWString::npos)
				|| (file
						&& SWString(area->filename).find(arg2str)
								== SWString::npos))
			continue;

		pager_printf(ch,
				FB_CYAN "[" PLAIN "%3d" FB_CYAN "][" PLAIN "%-20s" FB_CYAN "][" PLAIN "%-5d"
				FB_CYAN "-" PLAIN "%5d" FB_CYAN "][" PLAIN "%3d" FB_CYAN "]", i,
				strip_colors(area->name, 20), area->lvnum, area->uvnum,
				area->security);
		pager_printf(ch, "[" PLAIN "%-16s" FB_CYAN "]",
				strip_colors(area->builder, 16));
		pager_printf(ch, "[" PLAIN "%-14s" FB_CYAN "]" EOL,
				strip_colors(area->filename, 14));
		i++;
	}
	}

	/* Trog: niezainstalowane (uarea) krainki */
	for (auto* area : uarea_list)
	{
		if (area->lvnum < low || area->uvnum > high
				|| (name
						&& SWString(area->name).toLower().find(arg2str)
								== SWString::npos)
				|| (author
						&& SWString(area->author).toLower().find(arg2str)
								== SWString::npos)
				|| (builder
						&& SWString(area->builder).toLower().find(arg2str)
								== SWString::npos)
				|| (file
						&& SWString(area->filename).find(arg2str)
								== SWString::npos))
			continue;

		pager_printf(ch, "[%3d][%-20s][%-5d-%5d][%3d]", i++,
				strip_colors(area->name, 20), area->lvnum, area->uvnum,
				area->security);
		pager_printf(ch, "[%-16s]", strip_colors(area->builder, 16));
		pager_printf(ch, "[%-14s]" EOL, strip_colors(area->filename, 14));
	}
}

/*
 * Save system info to data file
 */
void save_sysdata(SYSTEM_DATA sys)
{
	FILE *fp;
	char filename[MAX_INPUT_LENGTH];

	sprintf(filename, SYSDATA_FILE);

	RESERVE_CLOSE;
	if ((fp = fopen(filename, "w")) == NULL)
	{
		bug("fopen");
	}
	else
	{
		fprintf(fp, "#SYSTEM\n");
		fprintf(fp, "Highplayers    %d\n", sys.alltimemax);
		fprintf(fp, "Highplayertime %s~\n", sys.time_of_max);
		fprintf(fp, "Nameresolving  %d\n", sys.NO_NAME_RESOLVING);
		fprintf(fp, "Waitforauth    %d\n", sys.WAIT_FOR_AUTH);
		fprintf(fp, "Muse           %d\n", sys.muse_level);
		fprintf(fp, "Prog           %d\n", sys.prog_level);
		fprintf(fp, "Build          %d\n", sys.build_level);
		fprintf(fp, "Log            %d\n", sys.log_level);
		fprintf(fp, "Comm           %d\n", sys.comm_level);
		fprintf(fp, "Protoflag      %d\n", sys.level_modify_proto);
		fprintf(fp, "Overridepriv   %d\n", sys.level_override_private);
		fprintf(fp, "Msetplayer     %d\n", sys.level_mset_player);
		fprintf(fp, "Stunplrvsplr   %d\n", sys.stun_plr_vs_plr);
		fprintf(fp, "Stunregular    %d\n", sys.stun_regular);
		fprintf(fp, "Damplrvsplr    %d\n", sys.dam_plr_vs_plr);
		fprintf(fp, "Damplrvsmob    %d\n", sys.dam_plr_vs_mob);
		fprintf(fp, "Dammobvsplr    %d\n", sys.dam_mob_vs_plr);
		fprintf(fp, "Dammobvsmob    %d\n", sys.dam_mob_vs_mob);
		fprintf(fp, "Forcepc        %d\n", sys.level_forcepc);
		fprintf(fp, "Saveflags      %lld\n", sys.save_flags);
		fprintf(fp, "Savefreq       %d\n", sys.save_frequency);
		/*Added by Thanos */
		fprintf(fp, "LogAll         %d\n", sys.log_all);
		fprintf(fp, "LastCmdLog     %d\n", sys.lastcmd_log);
		fprintf(fp, "Downtype       %d\n", sys.downtype);
		fprintf(fp, "CrashLog       %d\n", sys.crashlog);
		fprintf(fp, "Silent         %d\n", sys.silent);
		fprintf(fp, "RebootType     %d\n", sys.reboot_type);
		fprintf(fp, "ToIso          %d\n", sys.to_iso);
		fprintf(fp, "Admins         %s~\n", sys.admins);
		fprintf(fp, "Coders         %s~\n", sys.coders);
		fprintf(fp, "Helpmasters    %s~\n", sys.helpmasters);
		fprintf(fp, "Council        %s~\n", sys.council);
		fprintf(fp, "DeniedNicks    %s~\n", sys.denied_nicks);
		fprintf(fp, "MaxClanId      %d\n", sys.max_clan_id); /* Trog */
		fprintf(fp, "End\n\n");
		fprintf(fp, "#END\n");
	}
	fclose(fp);
	RESERVE_OPEN;
	sys.downtype = DOWN_CRASHED;
	return;
}

void fread_sysdata(SYSTEM_DATA *sys, FILE *fp)
{
	char *word;
	bool fMatch;

	sys->time_of_max = NULL;
	for (;;)
	{
		word = feof(fp) ? (char*) "End" : fread_word(fp);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;

		case 'A':
			SKEY("Admins", sys->admins, fread_string(fp))
			;
			break;

		case 'B':
			KEY("Build", sys->build_level, fread_number(fp))
			;
			break;

		case 'C':
			KEY("Comm", sys->comm_level, fread_number(fp))
			;
			SKEY("Coders", sys->coders, fread_string(fp))
			;
			SKEY("Council", sys->council, fread_string(fp))
			;
			KEY("CrashLog", sys->crashlog, fread_number(fp))
			;
			break;

		case 'D':
			KEY("Damplrvsplr", sys->dam_plr_vs_plr, fread_number(fp))
			;
			KEY("Damplrvsmob", sys->dam_plr_vs_mob, fread_number(fp))
			;
			KEY("Dammobvsplr", sys->dam_mob_vs_plr, fread_number(fp))
			;
			KEY("Dammobvsmob", sys->dam_mob_vs_mob, fread_number(fp))
			;
			SKEY("DeniedNicks", sys->denied_nicks, fread_string(fp))
			;
			KEY("Downtype", sys->downtype, fread_number(fp))
			;
			break;

		case 'E':
			if (!str_cmp(word, "End"))
			{
				if (!sys->time_of_max)
					STRDUP(sys->time_of_max, "(not recorded)");

				/* Thanos      domy�lnie ustawiamy na reboot */
				if (sys->reboot_type == REB_SHUTDOWN)
					sys->reboot_type = REB_REBOOT;
				return;
			}
			break;

		case 'F':
			KEY("Forcepc", sys->level_forcepc, fread_number(fp))
			;
			break;

		case 'H':
			SKEY("Helpmasters", sys->helpmasters, fread_string(fp))
			;
			KEY("Highplayers", sys->alltimemax, fread_number(fp))
			;
			SKEY("Highplayertime", sys->time_of_max, fread_string(fp))
			;
			break;

		case 'L':
			KEY("Log", sys->log_level, fread_number(fp))
			;
			KEY("LogAll", sys->log_all, fread_number(fp))
			;
			KEY("LastCmdLog", sys->lastcmd_log, fread_number(fp))
			;
			break;

		case 'M':
			KEY("Msetplayer", sys->level_mset_player, fread_number(fp))
			;
			KEY("Muse", sys->muse_level, fread_number(fp))
			;
			KEY("MaxClanId", sys->max_clan_id, fread_number(fp))
			;
			break;

		case 'N':
			KEY("Nameresolving", sys->NO_NAME_RESOLVING, fread_number(fp))
			;
			break;

		case 'O':
			KEY("Overridepriv", sys->level_override_private, fread_number(fp))
			;
			break;

		case 'P':
			KEY("Prog", sys->prog_level, fread_number(fp))
			;
			KEY("Protoflag", sys->level_modify_proto, fread_number(fp))
			;
			break;

		case 'R':
			KEY("RebootType", sys->reboot_type, fread_number(fp))
			;
			break;

		case 'S':
			KEY("Stunplrvsplr", sys->stun_plr_vs_plr, fread_number(fp))
			;
			KEY("Stunregular", sys->stun_regular, fread_number(fp))
			;
			KEY("Saveflags", sys->save_flags, fread_number64(fp))
			;
			KEY("Savefreq", sys->save_frequency, fread_number(fp))
			;
			KEY("Silent", sys->silent, fread_number(fp))
			;
			break;

		case 'T':
			KEY("Think", sys->think_level, fread_number(fp))
			;
			KEY("ToIso", sys->to_iso, fread_number(fp))
			;
			break;

		case 'W':
			KEY("Waitforauth", sys->WAIT_FOR_AUTH, fread_number(fp))
			;
			break;
		}

		if (!fMatch)
		{
			bug("no match: %s", word);
		}
	}
}

/*
 * Load the sysdata file
 */
bool load_systemdata(SYSTEM_DATA *sys)
{
	char filename[MAX_INPUT_LENGTH];
	FILE *fp;
	bool found;

	found = false;
	sprintf(filename, SYSDATA_FILE);

	if ((fp = fopen(filename, "r")) != NULL)
	{

		found = true;
		for (;;)
		{
			char letter;
			char *word;

			letter = fread_letter(fp);
			if (letter == '*')
			{
				fread_to_eol(fp);
				continue;
			}

			if (letter != '#')
			{
				bug("# not found.");
				break;
			}

			word = fread_word(fp);
			if (!str_cmp(word, "SYSTEM"))
			{
				fread_sysdata(sys, fp);
				break;
			}
			else if (!str_cmp(word, "END"))
				break;
			else
			{
				bug("bad section.");
				break;
			}
		}
		fclose(fp);
	}

	return found;
}

void load_banlist(void)
{
	BAN_DATA *pban;
	FILE *fp;
	int number;
	char letter;

	if (!(fp = fopen( BAN_LIST, "r")))
		return;

	for (;;)
	{
		if (feof(fp))
		{
			bug("no -1 found.");
			fclose(fp);
			return;
		}
		number = fread_number(fp);
		if (number == -1)
		{
			fclose(fp);
			return;
		}
		CREATE(pban, BAN_DATA, 1);
		pban->level = number;
		pban->name = fread_string(fp);
		if ((letter = fread_letter(fp)) == '~')
			pban->ban_time = fread_string(fp);
		else
		{
			ungetc(letter, fp);
			STRDUP(pban->ban_time, "(unrecorded)");
		}
		ban_list.push_back(pban);
	}
}

/* Check to make sure range of vnums is free - Scryn 2/27/96 */

DEF_DO_FUN( check_vnums )
{
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];
	char arg1[MAX_STRING_LENGTH];
	char arg2[MAX_STRING_LENGTH];
	bool area_conflict;
	int low_range, high_range;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (!*arg1 || !*arg2)
	{
		send_to_char("Syntax: checkvnums <lvnum> <uvnum> [prototype]" NL, ch);
		return;
	}

	low_range = atoi(arg1);
	high_range = atoi(arg2);

	if (low_range < 1 || low_range > MAX_VNUM)
	{
		send_to_char("Invalid argument for bottom of range." NL, ch);
		return;
	}

	if (high_range < 1 || high_range > MAX_VNUM)
	{
		send_to_char("Invalid argument for top of range." NL, ch);
		return;
	}

	if (high_range < low_range)
	{
		send_to_char("Bottom of range must be below top of range." NL, ch);
		return;
	}

	for (auto* pArea : asort_list)
	{
		area_conflict = false;

		if (low_range < pArea->lvnum && pArea->lvnum < high_range)
			area_conflict = true;

		if (low_range < pArea->uvnum && pArea->uvnum < high_range)
			area_conflict = true;

		if ((low_range >= pArea->lvnum) && (low_range <= pArea->uvnum))
			area_conflict = true;

		if ((high_range <= pArea->uvnum) && (high_range >= pArea->lvnum))
			area_conflict = true;

		if (area_conflict)
		{
			if (!str_cmp(argument,
					"prototype") && !IS_SET( pArea->flags, AFLAG_PROTOTYPE ))
				continue;

			sprintf(buf, "Conflict:%-15s| ",
					(pArea->filename ? pArea->filename : "(invalid)"));
			sprintf(buf2, "Vnums: %5d - %-5d" NL, pArea->lvnum, pArea->uvnum);
			strcat(buf, buf2);
			send_to_pager(buf, ch);
		}
	}

	for (auto* pArea : bsort_list)
	{
		area_conflict = false;

		if (low_range < pArea->lvnum && pArea->lvnum < high_range)
			area_conflict = true;

		if (low_range < pArea->uvnum && pArea->uvnum < high_range)
			area_conflict = true;

		if ((low_range >= pArea->lvnum) && (low_range <= pArea->uvnum))
			area_conflict = true;

		if ((high_range <= pArea->uvnum) && (high_range >= pArea->lvnum))
			area_conflict = true;

		if (area_conflict)
		{
			if (!str_cmp(argument,
					"prototype") && !IS_SET( pArea->flags, AFLAG_PROTOTYPE ))
				continue;

			sprintf(buf, "Conflict:%-15s| ",
					(pArea->filename ? pArea->filename : "(invalid)"));
			sprintf(buf2, "Vnums: %5d - %-5d" NL, pArea->lvnum, pArea->uvnum);
			strcat(buf, buf2);
			send_to_pager(buf, ch);
		}
	}
	return;
}

/*
 * This function is here to aid in debugging.
 * If the last expression in a function is another function call,
 *   gcc likes to generate a JMP instead of a CALL.
 * This is called "tail chaining."
 * It hoses the debugger call stack for that call.
 * So I make this the last call in certain critical functions,
 *   where I really need the call stack to be right for debugging!
 *
 * If you don't understand this, then LEAVE IT ALONE.
 * Don't remove any calls to tail_chain anywhere.
 *
 * -- Furey
 */
void tail_chain(void)
{
	return;
}

/*
 * Ratm napisa� t� funkcj�, a ja j� troszk� pozmienia�em i prosz�
 * jaki �adny copyoverek wyszed� :P			--Thanos
 */
DEF_DO_FUN( copyover )
{
	int down;

	if (str_cmp(argument, "mud now"))
	{
		send_to_char("Syntax: 'copyover mud now'" NL, ch);
		return;
	}

	log_string("Copyover start:");
	CopyOver = true;
	down = sysdata.downtype;
	sysdata.downtype = DOWN_COPYOVER;
	save_sysdata(sysdata);
	copyover();

	/*
	 * Tutaj nie powinni�my doj��...
	 */
	send_to_char("CopyOver failed." NL, ch);
	sysdata.downtype = down;
	save_sysdata(sysdata);
	return;
}

/*
 * Added by Ratm te funkcje pochodz� z kodu copyover napisanego przez:
 * (chwilowo nie wiem przez kogo)
 *
 *                                                 Copied & Pasted by
 *                                                      Thanos :)
 */
void copyover(void)
{
	FILE *fp;
	char buf[MSL];
	char cbuf[MSL];
	char buf2[100];
	char buf3[100];

	//Get everybody out of homes if they are guests
	for (auto* d : descriptor_list)
	{
		CHAR_DATA *ch = CH(d);
		if (!ch)
			continue;
		if (IS_SET(ch->in_room->room_flags, ROOM_PLR_HOME)
				&& ch->plr_home != ch->in_room)
			evacuate_guests(ch->in_room);
	}
	/*
	 * write_do_descriptor nie uznaje kolork�w w naszym kodowaniu
	 * hardcoding sucks...
	 */
	sprintf(cbuf,
			NL NL NL NL "       "
			"[1;32m* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *[0m" NL "       "
			"[1;32m*[0m                       [1;37m!!! UWAGA !!!                         [1;32m*[0m" NL "       "
			"[1;32m*[0m                                                             [1;32m*[0m" NL "       "
			"[1;32m*[0m        SW-Mud wlasnie sie restartuje. Odradza sie caly      [1;32m*[0m" NL "       "
			"[1;32m*[0m        swiat i wszechswiat. Nie przejmuj si� wiec tym       [1;32m*[0m" NL "       "
			"[1;32m*[0m        malym lagiem, bo zaraz bedzie mozna grac dalej.      [1;32m*[0m" NL "       "
			"[1;32m*[0m                                                             [1;32m*[0m" NL "       "
			"[1;32m*[0m                       Administratorzy                       [1;32m*[0m" NL "       "
			"[1;32m* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *[0m" NL NL NL NL);

	sprintf(buf,
			NL NL NL NL "      "
			"* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" NL "      "
			"*                       !!! UWAGA !!!                         *" NL "      "
			"*                                                             *" NL "      "
			"*        SW-Mud wlasnie sie restartuje. Odradza sie caly      *" NL "      "
			"*        swiat i wszechswiat. Nie przejmuj sie wiec tym       *" NL "      "
			"*        malym lagiem, bo zaraz bedzie mozna grac dalej.      *" NL "      "
			"*                                                             *" NL "      "
			"*                       Administratorzy                       *" NL "      "
			"* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *" NL NL NL NL);

	fp = fopen(COPYOVER_FILE, "w");

	if (!fp)
	{
		log_string("Copyover file not writeable, aborted.\n");
		bug("Could not write to copyover file: %s", COPYOVER_FILE);
		perror("do_copyover:fopen");
		return;
	}

	/* For each playing descriptor, save its state */
	{
	auto desc_snapshot = descriptor_list;
	for (auto* d : desc_snapshot)
	{
		CHAR_DATA *och = CH(d);

		if (!d->character || d->connected < CON_PLAYING)
		{
			write_to_descriptor(d->descriptor, NL
			"Przykro nam ale mud wlasnie sie restartuje." NL
			"Przepraszamy za utrudnienia. Za kilka sekund" NL
			"bedzie mozna znow sie zalogowac." NL
			"Administratorzy." NL, 0);
			close_socket(d, true); /* throw'em out */
		}
		else
		{
			if (IS_SET(och->act, PLR_STATUS_BAR))
			{
				write_to_descriptor(d->descriptor, VT_SAVECURSOR, 0);
				sprintf(buf2, "\e[%d;1H", och->pcdata->status_height - 1);
				write_to_descriptor(d->descriptor, buf2, 0);
			}

			fprintf(fp, "%d %d %s %s %d\n", d->descriptor, d->port, och->name,
					d->host, d->ipv6 ? 1 : 0);

			if (IS_SET(CH( d )->act, PLR_ANSI))
				write_to_descriptor(d->descriptor, cbuf, 0);
			else
				write_to_descriptor(d->descriptor, buf, 0);

			if (NOT_AUTHED(och))
			{
				och->pcdata->auth_state = 3;
				REMOVE_BIT(och->pcdata->flags, PCFLAG_UNAUTHED);

				STRDUP(och->pcdata->authed_by, "ADMINS:CopyOver");

				sprintf(buf, "SYSTEM: authorized %s Bcoz of copyover.",
						och->name);
				to_channel(buf, CHANNEL_MONITOR, "Monitor", 104);
			}
			save_char_obj(och);
		}
	}
	}

	/* Save time and weather */
	for (auto* planet : planet_list)
		save_planet(planet);

	fprintf(fp, "-1\n");
	fclose(fp);

	fflush(fpReserve);
	RESERVE_CLOSE;
	fflush(fpLOG);
	LOG_CLOSE;

	sprintf(buf, "%d", port);
	sprintf(buf2, "%d", control);
	sprintf(buf3, "%d", control6);

	/*
	 * Ten rozkaz ponizej (execl) dziala tak:
	 * execl( <program>, <nazwa_procesu>, <arg1>, <arg2>, NULL );
	 * jesli nie ma arg2 --> mud wstanie normalnie,
	 * a jesli jest      --> copyover sie zrobi
	 * arg1 	     --> port_number MUSI BYC !!!
	 * arg2 lepiej samemu nie ustawiac -- mud zrobi to za nas
	 * arg3 -> tez nie ustawiac samemu
	 * podsumowujac, zostanie uruchomiony:
	 * swmud <nr_portu> copyover <deskryptor portu ipv4> <deskryptor portu ipv6>
	 */
	execl( BIN_FILE, "swmud", buf, "copyover", buf2, buf3, (char*) NULL);

	/* Failed:  sucessful exec will not return */
	perror("do_copyover: execl %s", BIN_FILE);
	log_string("Copyover FAILED!" NL);

	RESERVE_OPEN;
	LOG_OPEN;

	for (auto* d : descriptor_list)
	{
		CHAR_DATA *och = CH(d);

		if (IS_SET(och->act, PLR_STATUS_BAR))
			write_to_descriptor(d->descriptor, VT_RESTORECURSOR, 0);
	}
}

/* Recover from a copyover - load players */
void copyover_recover(void)
{
	DESCRIPTOR_DATA *d;
	FILE *fp;
	char name[100];
	char host[MAX_STRING_LENGTH];
	int desc;
	int port;
	bool fOld;
	int ipv6;

	log_string("Copyover recovery initiated");

	fp = fopen(COPYOVER_FILE, "r");

	if (!fp)
	{
		perror("copyover_recover:fopen");
		log_string("Copyover file not found. Exitting.\n");
		exit(1);
	}

	for (;;)
	{
		int fsfRet = fscanf(fp, "%d %d %s %s %d\n", &desc, &port, name, host,
				&ipv6);
		if (fsfRet == EOF || desc == -1)
			break;

		/* Write something, and check if it goes error-free */
		if (!write_to_descriptor(desc, NL
		"Gor�cy reboot zako�czony." NL, 0))
		{
#if !defined( WIN32 )
			close(desc); /* nope */
#else
			closesocket( desc );
#endif

			continue;
		}

		CREATE(d, DESCRIPTOR_DATA, 1);
		init_descriptor(d, desc);
		STRDUP(d->host, host);
		STRDUP(d->user, UNKNOWN_USER);
		d->port = (unsigned int) port;
		d->ipv6 = ipv6 ? true : false;

		d->connected = CON_COPYOVER_RECOVER;
		descriptor_list.push_back(d);

		fOld = load_char_obj(d, name, false);
		if (!fOld)
		{
			write_to_descriptor(desc,
					"Jakim� cudem twoja posta� uleg�a zniszczeniu podczas rebootu." NL
					"Skontaktuj si� z w�adcami muda by j� odzyska�: mud@swmud.pl." NL,
					0);
			close_socket(d, true);
		}
		else
		{
			STRDUP(d->character->pcdata->host, d->host);
			STRDUP(d->character->pcdata->user, d->user);

			if (++num_descriptors > sysdata.maxplayers)
				sysdata.maxplayers = num_descriptors;

			if (sysdata.maxplayers > sysdata.alltimemax)
			{
				char buf[MIL];

				sprintf(buf, "%24.24s", ctime(&current_time));
				STRDUP(sysdata.time_of_max, buf);
				sysdata.alltimemax = sysdata.maxplayers;
				sprintf(log_buf, "Broke all-time maximum player record: %d",
						sysdata.alltimemax);
				log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
				to_channel(log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL);
				save_sysdata(sysdata);
			}

			if (!d->character->in_room)
				d->character->in_room = get_room_index(ROOM_VNUM_LIMBO);

			char_list.push_back(d->character);

			load_home(CH(d));

			char_to_room(d->character, d->character->in_room);
			do_look(d->character, (char*) "");
			act(COL_FORCE, "$n materializuje si� tutaj!", d->character, NULL,
					NULL, TO_ROOM);
			d->connected = CON_PLAYING;

			load_deposit(CH(d));

			send_to_char(NL NL NL FB_WHITE "                    "
			"Ju� w porz�dku. Mo�esz gra� dalej."
			EOL NL NL, d->character);

			if (IS_SET(d->character->act, PLR_STATUS_BAR))
				write_to_descriptor(d->descriptor, VT_RESTORECURSOR, 0);

			if (CH( d )->inquest)
			{
				MOB_INDEX_DATA *questor;

				if ((questor = get_mob_index(
						CH( d )->inquest->pIndexData->questor)) != NULL)
					ch_printf(CH(d), NL "                "
					FB_GREEN "*** Pami�taj o zadaniu dla %s ***" EOL,
							questor->przypadki[1]);
				else
					bug("No questor found for %s's quest", CH( d )->name);
			}

#if !defined( WIN32 )
			start_auth(d);
#endif
		}
	}

	fclose(fp);
	unlink( COPYOVER_FILE);
	log_string("Copyover recovery finished");
}

void init_descriptor(DESCRIPTOR_DATA *dnew, int desc)
{
// removed by Thanos
//    static DESCRIPTOR_DATA d_zero;
//    *dnew = d_zero;
	dnew->descriptor = desc;
	dnew->connected = CON_GET_NAME;
	dnew->outsize = 2000;
	dnew->idle = 0;
	dnew->lines = 0;
	dnew->auth_fd = -1;
	dnew->auth_inc = 0;
	dnew->auth_state = 0;
	dnew->newstate = 0;
	dnew->prevcolor = 0x07;
	dnew->original = NULL;
	dnew->character = NULL;
	dnew->tmp_ch = NULL;
	dnew->str_editing = NULL;
	STRDUP(dnew->user, "unknown");

	CREATE(dnew->outbuf, char, dnew->outsize);

}
/* End Of CopyOver Stuff Here */

/*
 * Added by Thanos:      ustala nowy czas na planecie lub przywraca
 *			 stan sprzed rebootu. Tak samo z pogod�.
 */
void reset_planet(PLANET_DATA *planet)
{
	long lhour, lday, lmonth;
	int hours, months;
	SEASON_DATA *season;

	if (planet->seasons.empty())
	{
		season = new_season(planet, "pora nieznana");
		planet->seasons.push_back(season);
	}

	if (!planet->curr_season)
		planet->curr_season = planet->seasons.front();

	hours = planet->curr_season->day_length + planet->curr_season->night_length;

	if (!planet->monthlen)
		planet->monthlen = 30;

	if (!planet->daylen)
		planet->daylen = planet->curr_season->day_length;
	;

	if (!planet->nightlen)
		planet->nightlen = planet->curr_season->night_length;
	;

	months = planet->curr_season->length;

	lhour = (current_time - 650336715) / ( PULSE_TICK / PULSE_PER_SECOND);

	if (planet->hour < 0)
		planet->hour = lhour % hours;
	lday = lhour / hours;

	if (planet->day < 0)
		planet->day = lday % planet->monthlen;
	lmonth = lday / planet->monthlen;

	if (planet->month < 0)
		planet->month = lmonth % months;

	if (planet->hour < 5)
		planet->sunlight = SUN_DARK;
	else if (planet->hour < 6)
		planet->sunlight = SUN_RISE;
	else if (planet->hour < 19)
		planet->sunlight = SUN_LIGHT;
	else if (planet->hour < 20)
		planet->sunlight = SUN_SET;
	else
		planet->sunlight = SUN_DARK;

	planet->windspeed = planet->curr_season->windspeed;
	planet->change = 0;
	planet->mmhg = planet->curr_season->mmhg;
	planet->mmhg += (number_range(1, planet->curr_season->diff_mmhg * 2)
			- planet->curr_season->diff_mmhg);
	planet->temperature = planet->curr_season->temperature;
	planet->temperature += (number_range(1,
			planet->curr_season->diff_temperature * 2)
			- planet->curr_season->diff_temperature);

	if (planet->mmhg <= 980)
		planet->sky = SKY_LIGHTNING;
	else if (planet->mmhg <= 1000)
		planet->sky = SKY_RAINING;
	else if (planet->mmhg <= 1020)
		planet->sky = SKY_CLOUDY;
	else
		planet->sky = SKY_CLOUDLESS;

	return;
}

void set_timeweather()
{
	for (auto* planet : planet_list)
		reset_planet(planet);

	if (!sysdata.silent)
		log_string(" Done time and weather");
}

/* Trog: wczytywanie ras */
void load_races()
{
	FILE *fpList;
	char strRace[MIL];
	char found_str[MIL];

	/* Ustalamy base_race */
	base_race = (RACE_DATA*) &human_race;

	log_string("Reading in races files...");
	RESERVE_CLOSE;

	if (!(fpList = fopen( RACE_LIST, "r")))
	{
		log_string("Unable to open race list.");
		RESERVE_OPEN;
		return;
	}

	for (;;)
	{
		strcpy(strRace, fread_word(fpList));
		if (strRace[0] == '$')
			break;
		load_race_section(strRace);
	}

	sprintf(found_str, "...%d races found.", top_race);
	log_string(found_str);
	fclose(fpList);
	RESERVE_OPEN;
	return;
}

void load_race_section(char *filename)
{
	FILE *fpRace;
	char race_file[MFL];
	char *word;

	sprintf(race_file, "%s%s", RACE_DIR, filename);
	if (!(fpRace = fopen(race_file, "r")))
	{
		bug("error loading race file (can't open %s)", race_file);
		return;
	}

	word = fread_word(fpRace);

	if (!str_cmp(word, "#SWRACE"))
	{
		fread_race(fpRace, filename);
		top_race++;
	}
	else
	{
		bug("bad race file format (%s)", filename);
		if (fBootDb)
			exit(1);
	}

	fclose(fpRace);
	return;
}

void fread_race(FILE *fpRace, char *filename)
{
	RACE_DATA *pRace;
	char *word;
	char buf[MIL];
	bool fMatch;

	pRace = new_race();
	STRDUP(pRace->filename, filename);
	race_list.push_back(pRace);

	for (;;)
	{
		word = feof(fpRace) ? (char*) "End" : fread_word(fpRace);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fpRace);
			break;
		case 'A':
			KEY("Affected", pRace->affected, fread_number64(fpRace))
			;
			break;
		case 'C':
			KEY("Cha_plus", pRace->cha_plus, fread_number(fpRace))
			;
			KEY("Con_plus", pRace->con_plus, fread_number(fpRace))
			;
			break;
		case 'D':
			SKEY("Description", pRace->description, fread_string(fpRace))
			;
			KEY("Dex_plus", pRace->dex_plus, fread_number(fpRace))
			;
			break;
		case 'E':
			if (!str_cmp(word, "End"))
				return;
			break;
		case 'F':
			KEY("Flags", pRace->flags, fread_number64(fpRace))
			;
			KEY("Force_plus", pRace->force_plus, fread_number(fpRace))
			;
			KEY("Frc_plus", pRace->frc_plus, fread_number(fpRace))
			;
			break;
		case 'H':
			KEY("Hp_plus", pRace->hp_plus, fread_number(fpRace))
			;
			break;
		case 'I':
			KEY("Immune", pRace->immune, fread_number64(fpRace))
			;
			if (!str_cmp(word, "Inflects"))
			{
				int i;

				for (i = 0; i < 6; i++)
					STRDUP(pRace->przypadki[i], st_fread_string(fpRace));
				fMatch = true;
			}
			if (!str_cmp(word, "InflectsF"))          // Pixel: przypadkiF
			{
				int i;

				for (i = 0; i < 6; i++)
					STRDUP(pRace->inflectsFemale[i], st_fread_string(fpRace));
				fMatch = true;
			}
			KEY("Int_plus", pRace->int_plus, fread_number(fpRace))
			;
			break;
		case 'L':
			if (!str_cmp(word, "Language"))
			{
				fread_to_eol(fpRace);
				pRace->language = lang_base;
				fMatch = true;
			}
//				KEY( "Language",	pRace->language,	fread_number( fpRace ) );
			KEY("Lck_plus", pRace->lck_plus, fread_number(fpRace))
			;
			break;
		case 'N':
			SKEY("Name", pRace->name, fread_string(fpRace))
			;
			if (!str_cmp(pRace->name, base_race->name))
			{
				race_list.remove(pRace);
				free_race(pRace);
				return;
			}
			break;
		case 'R':
			KEY("Resistant", pRace->resistant, fread_number64(fpRace))
			;
			if (!str_cmp(word, "Restrictions"))
			{
				int i;
				for (i = 0; i < MAX_DESC_TYPES; i++)
					pRace->desc_restrictions[i] = fread_number64(fpRace);
				fMatch = true;
				break;
			}
			break;
		case 'S':
			KEY("Str_plus", pRace->str_plus, fread_number(fpRace))
			;
			KEY("Susceptible", pRace->susceptible, fread_number64(fpRace))
			;
			break;
		case 'W':
			KEY("Wis_plus", pRace->wis_plus, fread_number(fpRace))
			;
			break;
		}

		if (!fMatch)
		{
			sprintf(buf, "fread_race: no match: %s", word);
			log_string(buf);
		}
	}
}
/* Trog: koniec wczytywania ras */

/*! Trog: rasy XML'owe */
void save_race2(RACE_DATA *race)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr node;
	char buf[MSL];
	int i;

	swXmlInitIO();
	doc = xmlNewDoc( BC"1.0" );
	root = xmlNewNode( NULL, BC"race" );
	xmlNewNs( root, BC"http://swmud.pl/ns/swmud/1.0/race", NULL );
	xmlNewNs( root, BC"http://www.w3.org/2001/XMLSchema-instance", BC"xsi" );
	swNewProp(root, "xsi:schemaLocation",
			"http://swmud.pl/ns/swmud/1.0/race race.xsd");
	xmlDocSetRootElement(doc, root);

	swNewChildText(root, NULL, "name", race->name);
	node = xmlNewChild( root, NULL, BC"short", NULL );
	for (i = 0; i < 6; i++)
	{
		sprintf(buf, "inflect%d", i);
		swNewChildText(node, NULL, buf, race->przypadki[i]);
	}
	swNewChildText(root, NULL, "description", race->description);
	swNewChildInt64(root, NULL, "flags", race->flags);
	swNewChildInt64(root, NULL, "affected", race->affected);
	swNewChildInt(root, NULL, "str_plus", race->str_plus);
	swNewChildInt(root, NULL, "int_plus", race->int_plus);
	swNewChildInt(root, NULL, "wis_plus", race->wis_plus);
	swNewChildInt(root, NULL, "dex_plus", race->dex_plus);
	swNewChildInt(root, NULL, "con_plus", race->con_plus);
	swNewChildInt(root, NULL, "cha_plus", race->cha_plus);
	swNewChildInt(root, NULL, "lck_plus", race->lck_plus);
	swNewChildInt(root, NULL, "frc_plus", race->frc_plus);
	swNewChildInt(root, NULL, "hp_plus", race->hp_plus);
	swNewChildInt(root, NULL, "force_plus", race->force_plus);
	swNewChildInt64(root, NULL, "susceptiple", race->susceptible);
	swNewChildInt64(root, NULL, "resistant", race->resistant);
	swNewChildInt64(root, NULL, "immune", race->immune);
	node = xmlNewChild( root, NULL, BC"languages", NULL );
	/*
	 FOREACH( lang, race->first_lang )
	 swNewChildText( node, NULL, "language", lang->name );
	 */

	/* saving stuff */
	sprintf(buf, "Saving %s.xml...", race->filename);
	log_string_plus(buf, LOG_NORMAL, LEVEL_GREATER);

	sprintf(buf, "%s%s.xml", RACE_DIR, race->filename);

	RESERVE_CLOSE;
	xmlSaveFormatFileEnc(buf, doc, "ISO-8859-2", 1);
	RESERVE_OPEN;

	swXmlCleanIO();
	xmlFreeDoc(doc);
	swCleanupParser();
}

RACE_DATA* load_race(const char *filename)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr node;
	xmlNodePtr child;
	RACE_DATA *race;

	swXmlInitIO();

	if (!(doc = xsd_validate("race", SCHEMA_RACE, RACE_DIR, filename)))
		return NULL;
	/*! tutaj jestesmy juz PEWNI, ze plik rasy jest poprawny */

	race = new_race();
	STRDUP(race->filename, filename);

	root = xmlDocGetRootElement(doc);

	for (node = root->children; node; node = node->next)
	{
		EONLY(node);

		if (!swXmlStrcmp(node->name, "name"))
			swGetContent(&race->name, node);
		else if (!swXmlStrcmp(node->name, "short"))
		{
			for (child = node->children; child; child = child->next)
			{
				EONLY(child);

				if (!swXmlStrcmp(child->name, "inflect0"))
					swGetContent(&race->przypadki[0], child);
				else if (!swXmlStrcmp(child->name, "inflect1"))
					swGetContent(&race->przypadki[1], child);
				else if (!swXmlStrcmp(child->name, "inflect2"))
					swGetContent(&race->przypadki[2], child);
				else if (!swXmlStrcmp(child->name, "inflect3"))
					swGetContent(&race->przypadki[3], child);
				else if (!swXmlStrcmp(child->name, "inflect4"))
					swGetContent(&race->przypadki[4], child);
				else if (!swXmlStrcmp(child->name, "inflect5"))
					swGetContent(&race->przypadki[5], child);
			}
		}
		else if (!swXmlStrcmp(node->name, "description"))
			swGetContent(&race->description, node);
		else if (!swXmlStrcmp(node->name, "flags"))
			swGetContentInt64(&race->flags, node);
		else if (!swXmlStrcmp(node->name, "affected"))
			swGetContentInt64(&race->affected, node);
		else if (!swXmlStrcmp(node->name, "str_plus"))
			swGetContentInt(&race->str_plus, node);
		else if (!swXmlStrcmp(node->name, "int_plus"))
			swGetContentInt(&race->int_plus, node);
		else if (!swXmlStrcmp(node->name, "wis_plus"))
			swGetContentInt(&race->wis_plus, node);
		else if (!swXmlStrcmp(node->name, "dex_plus"))
			swGetContentInt(&race->dex_plus, node);
		else if (!swXmlStrcmp(node->name, "con_plus"))
			swGetContentInt(&race->con_plus, node);
		else if (!swXmlStrcmp(node->name, "cha_plus"))
			swGetContentInt(&race->cha_plus, node);
		else if (!swXmlStrcmp(node->name, "lck_plus"))
			swGetContentInt(&race->lck_plus, node);
		else if (!swXmlStrcmp(node->name, "frc_plus"))
			swGetContentInt(&race->frc_plus, node);
		else if (!swXmlStrcmp(node->name, "hp_plus"))
			swGetContentInt(&race->hp_plus, node);
		else if (!swXmlStrcmp(node->name, "force_plus"))
			swGetContentInt(&race->force_plus, node);
		else if (!swXmlStrcmp(node->name, "susceptible"))
			swGetContentInt64(&race->susceptible, node);
		else if (!swXmlStrcmp(node->name, "resistant"))
			swGetContentInt64(&race->resistant, node);
		else if (!swXmlStrcmp(node->name, "immune"))
			swGetContentInt64(&race->immune, node);
		else if (!swXmlStrcmp(node->name, "languages"))
		{
			for (child = node->children; child; child = child->next)
			{
				EONLY(child);

				if (!swXmlStrcmp(child->name, "language"))
				{
					/* jezyki */
				}
			}
		}
	}

	swXmlCleanIO();
	xmlFreeDoc(doc);
	swCleanupParser();
	return race;
}

/* Trog: wczytywanie turbocar'ow */
void load_turbocars()
{
	FILE *fpList;
	char strTurbocar[MIL];
	char found_str[MIL];

	log_string("Reading in turbocars files...");
	RESERVE_CLOSE;

	if (!(fpList = fopen( TURBOCAR_LIST, "r")))
	{
		log_string("Unable to open turbocar list.");
		RESERVE_OPEN;
		return;
	}

	for (;;)
	{
		strcpy(strTurbocar, fread_word(fpList));
		if (strTurbocar[0] == '$')
			break;
		load_turbocar_section(strTurbocar);
	}

	sprintf(found_str, "...%d turbocars found.", top_turbocar);
	log_string(found_str);
	fclose(fpList);
	RESERVE_OPEN;
	return;
}

void load_turbocar_section(char *filename)
{
	FILE *fpTurbocar;
	char turbocar_file[MFL];
	char *word;

	sprintf(turbocar_file, "%s%s", TURBOCAR_DIR, filename);
	if (!(fpTurbocar = fopen(turbocar_file, "r")))
	{
		bug("error loading turbocar file (can't open %s)", turbocar_file);
		return;
	}

	word = fread_word(fpTurbocar);

	if (!str_cmp(word, "#SWTURBOCAR"))
	{
		fread_turbocar(fpTurbocar, filename);
		top_turbocar++;
	}
	else
	{
		bug("bad turbocar file format (%s)", filename);
		if (fBootDb)
			exit(1);
	}

	fclose(fpTurbocar);
	return;
}

void fread_turbocar(FILE *fpTurbocar, char *filename)
{
	TURBOCAR *pTurbocar;
	TC_STATION *station;
	char *word;
	char buf[MIL];
	bool fMatch;

	pTurbocar = new_turbocar();
	STRDUP(pTurbocar->filename, filename);
	turbocar_list.push_back(pTurbocar);

	for (;;)
	{
		word = feof(fpTurbocar) ? (char*) "End" : fread_word(fpTurbocar);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fpTurbocar);
			break;
		case 'E':
			if (!str_cmp(word, "End"))
				return;
			break;
		case 'N':
			SKEY("Name", pTurbocar->name, fread_string(fpTurbocar))
			;
			break;
		case 'S':
			if (!str_cmp(word, "Station"))
			{
				station = new_station();
				pTurbocar->stations.push_back(station);
				station->vnum = fread_number(fpTurbocar);
				STRDUP(station->name, st_fread_string(fpTurbocar));
			}
			fMatch = true;
			break;
		case 'V':
			KEY("Vnum", pTurbocar->vnum, fread_number(fpTurbocar))
			;
			break;
		}

		if (!fMatch)
		{
			sprintf(buf, "fread_turbocar: no match: %s", word);
			log_string(buf);
		}
	}
}
/* Trog: koniec wczytywania turbocar'wo */

/*! Trog: jezyki */
void save_languages()
{
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr node;

	swXmlInitIO();
	doc = xmlNewDoc( BC"1.0" );
	root = xmlNewNode( NULL, BC"languages" );
	xmlNewNs( root, BC"http://swmud.pl/ns/swmud/1.0/languages", NULL );
	xmlNewNs( root, BC"http://www.w3.org/2001/XMLSchema-instance", BC"xsi" );
	swNewProp(root, "xsi:schemaLocation",
			"http://swmud.pl/ns/swmud/1.0/languages languages.xsd");
	xmlDocSetRootElement(doc, root);

	for (auto* lang : lang_list)
	{
		node = xmlNewChild( root, NULL, BC"language", NULL );
		swNewChildText(node, NULL, "name", lang->name);
		swNewChildText(node, NULL, "alphabet", lang->alphabet);
		swNewChildText(node, NULL, "deny_text", lang->deny_text);
		swNewChildInt(node, NULL, "max_learn", lang->max_learn);
		swNewChildInt(node, NULL, "max_practice", lang->max_practice);
		swNewChildInt(node, NULL, "min_int", lang->min_int);
		swNewChildInt(node, NULL, "difficulty", lang->difficulty);
	}

	/* saving stuff */
	log_string_plus("Saving " LANGUAGES_FILE "...", LOG_NORMAL, LEVEL_GREATER);

	RESERVE_CLOSE;
	xmlSaveFormatFileEnc( LANGUAGES_FILE, doc, "ISO-8859-2", 1);
	RESERVE_OPEN;

	swXmlCleanIO();
	xmlFreeDoc(doc);
	swCleanupParser();
}

void load_languages()
{
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr node;
	xmlNodePtr child;
	LANG_DATA *lang;

	lang_base = (LANG_DATA*) &lang_common;

	swXmlInitIO();

	if (!(doc = xsd_validate("languages", SCHEMA_LANGUAGES, RACE_DIR,
			"languages.xml")))
		return;
	/*! tutaj jestesmy juz PEWNI, ze plik z jezykami jest poprawny */

	root = xmlDocGetRootElement(doc);

	for (node = root->children; node; node = node->next)
	{
		EONLY(node);

		lang = new_lang();
		for (child = node->children; child; child = child->next)
		{
			EONLY(child);

			if (!swXmlStrcmp(child->name, "name"))
				swGetContent(&lang->name, child);
			else if (!swXmlStrcmp(child->name, "alphabet"))
				swGetContent(&lang->alphabet, child);
			else if (!swXmlStrcmp(child->name, "deny_text"))
				swGetContent(&lang->deny_text, child);
			else if (!swXmlStrcmp(child->name, "max_learn"))
				swGetContentInt(&lang->max_learn, child);
			else if (!swXmlStrcmp(child->name, "max_practice"))
				swGetContentInt(&lang->max_practice, child);
			else if (!swXmlStrcmp(child->name, "min_int"))
				swGetContentInt(&lang->min_int, child);
			else if (!swXmlStrcmp(child->name, "difficulty"))
				swGetContentInt(&lang->difficulty, child);
		}
		lang_list.push_back(lang);
	}

	swXmlCleanIO();
	xmlFreeDoc(doc);
	swCleanupParser();
	return;
}

