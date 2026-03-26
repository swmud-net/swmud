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
*                        Flags Module added By Trog                        *
****************************************************************************/

#include "mud.h"

/*
 *  UWAGA !!!
 *
 *  typy:     np. (ch->sex = SEX_NEUTRAL)
 *      *_types_list                   			true
 *
 *  flagi:    np. SET_BIT( ch->act, ACT_IS_NPC)
 *      *_flags_list                   			false
 *
 *  Chodzi tutaj o 3 pole struktury.
 */
const	struct	flag_stat_type	flag_stat_table	[ ]	=
{
	/*                                                 OLC?	*/
	/* TYPY */
	{	"abilities",	class_types_list,		true,	true	},//Trog
	{	"apply_types",	apply_types_list,		true,	true	},
	{	"class_types",	class_types_list,		true,	true	},
	{	"constate_types",constate_types_list,	true,	false	}, /*Thanos*/
	{	"crystal",		crystal_types_list,		true,	true	},
	{	"force_types",	forcetype_list,			true,	false	}, //byTrog
	{	"gender",		gender_types_list,		true,	true	},
	{	"grenade_types",granadetypes_list,		true,	true	}, //byThanos
	{	"help_types",	help_types_list,		true,	true	}, /*Thanos*/
	{	"liquid_types",	liquid_types_list,		true,	true	},
	{	"log_types",	log_types_list,			true,	true	}, /*Thanos*/
	{	"material_form",material_form_list,		true,	true    }, //Tanglor
	{	"material_rarity",material_rarity_list,	true,	true    }, //Tanglor
	{	"buildings",	planet_buildings_list,	true,	true	}, //Tanglor
	{	"mont_types",	mont_types_list,		true,	true	}, /*Thanos&Onyx*/
	{	"obj_types",	obj_types_list,			true,	true	},
	{	"position",		position_types_list,	true,	true	},
	{	"quest_inits",	quest_init_types,		true,	true	}, /* Thanos */
	{	"quest_events",	quest_event_types,		true,	true	}, /* questy */
	{	"quest_actions",quest_action_types,		true,	true	},
	{	"quest_chapters",quest_chapter_types,	true,	true	}, /* questy */
	{	"req_types",	req_types_list,			true,	true	}, /*Thanos*/
	{	"sectors",		sector_types_list,		true,	true	},
	{	"sex",			sex_types_list,			true,	true	},
	{	"spice",		spice_types_list,		true,  true	},
	{	"trap_types",	trap_types_list,		true,  true	},
	{	"ship_types",	ship_types_list,		true,  true	}, //Thanos
	{	"ship_classes",	ship_classes_list,		true,  true	}, //Thanos
	{	"ship_classes_names",	ship_classes_name_list,	true,  true	}, //Aldegard
	{	"weapon_types",	weapon_types_list,		true,  true	},
	{	"wear_locs",	wear_types_list,		true,  true	},
	{	"force_events",	fevent_types_list,		true,  false},

	/* FLAGI */
	{	"affects",		aff_flags_list,			false,	true	},
	{	"act_flags",	act_flags_list,			false,	true	},
	{	"area_flags",	area_flags_list,		false,	true	},
	{	"attack_flags",	attack_flags_list,		false,	true	},
	{	"autosave_flags",save_flags_list,		false,	false	},
	{	"clan_flags",	clan_flags_list,		false,	true	},
	{	"command_flags",command_flags_list,		false,	false	},
	{	"container_flags",container_flags_list,	false,	true	},
	{	"crime_flags",	crime_flags_list,		false,	true	},
	{	"defence_flags",def_flags_list,			false,	true	},
	{	"dial_flags",	dial_flags_list,		false,	true	},
	{	"ex_flags",		exit_flags_list,		false,	true	},
	{	"force",		force_flags_list,		false,	true	},
	{	"mudprog_flags",mprog_flags_list,		false,	true	},
	{	"obj_flags",	obj_flags_list,			false,	true	},
	{	"part_flags",	part_flags_list, 		false,	true	},
	{	"plr_flags",	plr_flags_list,			false,	false	},
	{	"pl_wear_flags",pl_wear_flags_list,		false,	false	},/*Thanos*/
	{	"pc_flags",		pc_flags_list,			false,	false	},
	{	"quest_flags",	quest_flags_list,		false,	true	}, //by Trog
	{	"race_flags",	race_flags_list,		false,	true	},
	{	"ris_flags",	ris_flags_list,			false,	true	},
	{	"room_flags",	room_flags_list,		false,	true	},
	{	"ship_flags",	ship_flags_list,		false,	true	}, //Aldegard
	{	"shop_flags",	shop_flags_list,		false,	true	}, /*Trog*/
	{	"ssystem_flags",ssystem_flags_list,		false,	true	}, //Aldegard
	{	"trap_flags",	trap_flags_list,		false,	true	},
	{	"trig_flags",	trig_flags_list,		false,	true	},
	{	"wear_flags",	wear_flags_list,		false,	true	},
	{	"",				0,						0,		0		}
};

bool is_stat( const struct flag_type *flag_table )
{
	int64	flag;

	for ( flag = 0; flag_stat_table[flag].structure; flag++ )
	{
		if ( flag_stat_table[flag].structure == flag_table
				   && flag_stat_table[flag].stat )
			return true;
	}
	return false;
}

int64 flag_lookup( const char *name, const struct flag_type *flag_table )
{
	int64	flag;

	for ( flag = 0; *flag_table[flag].name; flag++ )
	{
		if ( !str_cmp( name, flag_table[flag].name ) )
			return flag_table[flag].bit;
	}

	return NO_FLAG;
}

int64 flag_value( const struct flag_type *flag_table, const char *argument )
{
	return flag_value( flag_table, (char*)argument );
}

int64 flag_value( const struct flag_type *flag_table, char *argument )
{
	char	word[MIL];
	int64	bit;
	int64	marked	= 0;
	bool	found	= false;

	if ( is_stat( flag_table ) )
	{

//	one_argument( argument, word ); Trog

		if ( ( bit = flag_lookup( argument, flag_table ) ) != NO_FLAG )
			return bit;
		else
			return NO_FLAG;
	}

	for ( ; ; )
	{
		argument = one_argument( argument, word );

		if ( word[0] == '\0' )
			break;

		if ( ( bit = flag_lookup( word, flag_table ) ) != NO_FLAG )
		{
			SET_BIT( marked, bit );
			found = true;
		}
	}

	if ( found )
		return marked;
	else
		return NO_FLAG;
}

char *bit_name( const struct flag_type *flag_table, int64 bit )
{
	int64	flag;

	for ( flag = 0; *flag_table[flag].name; flag++ )
	{
		if( flag_table[flag].bit == bit )
			return (char *)flag_table[flag].name;
	}
	return (char *)"nieznany";
}

char *flag_string( const struct flag_type *flag_table, int64 bits )
{
	static char	buf[MSL];
	int64		flag;

	buf[0] = '\0';

	for ( flag = 0; *flag_table[flag].name; flag++ )
	{
		if ( !is_stat( flag_table ) && IS_SET( bits, flag_table[flag].bit ) )
		{
			strcat( buf, " " );
			strcat( buf, flag_table[flag].name );
		}
		else if ( flag_table[flag].bit == bits )
		{
			strcat( buf, " " );
			strcat( buf, flag_table[flag].name );
			break;
		}
	}
	return ( buf[0] != '\0' ) ? buf + 1 : (char *)"none";
}

/* Trog */
const   struct  flag_type       relation_types_list [ ] =
{
	{   FB_RED "wrog\xb1" PLAIN,		POLITICS_ENEMY		},
	{   FB_BLUE "neutraln\xb1" PLAIN,	POLITICS_NEUTRAL        },
	{   FB_GREEN "pokojow\xb1" PLAIN,	POLITICS_ALLY           },
	{	"",				POLITICS_MAX		}
};

/* Trog */
const	struct	flag_type	shop_flags_list	[ ]	=
{
	{	"rawcost", 		SHOP_RAWCOST, 		},
	{	"", 			0 					}
};

const	struct	flag_type	area_flags_list	[ ]	=
{
	{	"nopkill", 		AFLAG_NOPKILL, 		},
	{	"nightdesc", 	AFLAG_NIGHTDESCS	},
	{	"nohail", 		AFLAG_NOHAIL		},
	{	"hidden",		AFLAG_HIDDEN		},
	{	"dontshowplanet",AFLAG_DONTSHOWPLANET},
	{	"prototype",	AFLAG_PROTOTYPE		},
	{	"for_ships",	AFLAG_FOR_SHIPS		},
	{	"", 			0 					}
};

const	struct	flag_type	sex_types_list[ ]	=
{
	{	"neutral", 		SEX_NEUTRAL		},
	{	"male", 		SEX_MALE   		},
	{	"female", 		SEX_FEMALE  		},
	{	"", 			0 			}
};

const	struct	flag_type	position_types_list[ ]	=
{
	{	"dead",			POS_DEAD, 		},
	{	"mortal",		POS_MORTAL, 		},
	{	"incap",		POS_INCAP, 		},
	{	"stunned",		POS_STUNNED, 		},
	{	"sleeping",		POS_SLEEPING, 		},
	{	"resting",		POS_RESTING, 		},
	{	"sitting",		POS_SITTING, 		},
	{	"fighting",		POS_FIGHTING, 		},
	{	"standing",		POS_STANDING, 		},
	{	"mounted",		POS_MOUNTED, 		},
	{	"shove",		POS_SHOVE, 		},
	{	"drag",			POS_DRAG, 		},
	{	"", 			0 			}
};

const	struct	flag_type	gender_types_list[ ]	=
{
	{	"neutral", 		GENDER_NEUTRAL		},
	{	"male", 		GENDER_MALE   		},
	{	"female", 		GENDER_FEMALE  		},
	{	"plural", 		GENDER_PLURAL		},
	{	"", 			0 			}
};

const	struct	flag_type	weapon_types_list[ ]	=
{
	{	"none", 		WEAPON_NONE		},
	{	"vibro_axe", 		WEAPON_VIBRO_AXE	},
	{	"vibro_blade", 		WEAPON_VIBRO_BLADE	},
	{	"lightsaber", 		WEAPON_LIGHTSABER	},
	{	"whip", 		WEAPON_WHIP		},
	{	"claw", 		WEAPON_CLAW		},
	{	"blaster", 		WEAPON_BLASTER		},
	{	"bludgeon", 		WEAPON_BLUDGEON		},
	{	"bowcaster", 		WEAPON_BOWCASTER	},
	{	"force_pike", 		WEAPON_FORCE_PIKE	},
	{	"", 			0 			}
};

/*
const	struct	flag_type	race_types_list[ ]	=
{
	{	"human",		RACE_HUMAN		},
	{	"wookiee",		RACE_WOOKIEE 		},
	{	"twi'lek",		RACE_TWI_LEK		},
	{	"rodian",		RACE_RODIAN		},
	{	"hutt",			RACE_HUTT		},
	{	"mon-calamari",		RACE_MON_CALAMARI	},
	{	"noghri",		RACE_NOGHRI 		},
	{	"gamorrean",		RACE_GAMORREAN		},
	{	"jawa",			RACE_JAWA 		},
	{	"adarian",		RACE_ADARIAN		},
	{	"ewok",			RACE_EWOK		},
	{	"verpine",		RACE_VERPINE		},
	{	"defel",		RACE_DEFEL		},
	{	"trandoshan",		RACE_TRANDOSHAN		},
	{	"chadra-fan",		RACE_CHADRA_FAN		},
	{	"quarren",		RACE_QUARREN		},
	{	"duinuogwuin",		RACE_DUINUOGWUIN	},
	{	"animal",		RACE_ANIMAL		},
	{	"insect",		RACE_INSECT		},
	{	"object",		RACE_OBJECT		},
	{	"yuuzhan-vong",		RACE_YUUZHAN_VONG	},

	{	"gados",		RACE_GADOS      },
	{	"moocher",		RACE_MOOCHER	},
	{	"talz",			RACE_TALZ		},
	{	"tchuukthai",	RACE_TCHUUKTHAI	},
	{	"aqualianin",	RACE_AQUALIANIN	},
	{	"gotal",		RACE_GOTAL		},
	{	"chubbit",		RACE_CHUBBIT	},
	{	"arkanianin",	RACE_ARKANIANIN	},
	{	"meeki",		RACE_MEEKI		},
	{	"kurtzen",		RACE_KURTZEN	},
	{	"yaka",			RACE_YAKA		},
	{	"barabel",		RACE_BARABEL	},
	{	"mulk",			RACE_MULK		},
	{	"ughnaught",	RACE_UGHNAUGHT	},
	{	"bimm",			RACE_BIMM		},
	{	"duros",		RACE_DUROS		},
	{	"bothanin",		RACE_BOTHANIN	},
	{	"caridanin",	RACE_CARIDANIN	},
	{	"selonianin",	RACE_SELONIANIN	},
	{	"yoda",			RACE_YODA		},
	{	"dantarianin",	RACE_DANTARIANIN},
	{	"ixel",			RACE_IXEL		},
	{	"plywak",		RACE_PLYWAK		},
	{	"teek",			RACE_TEEK		},
	{	"drall",		RACE_DRALL		},
	{	"yuzzum",		RACE_YUZZUM		},
	{	"gorax",		RACE_GORAX		},
	{	"etti",			RACE_ETTI		},
	{	"firrerreonianin",RACE_FIRRERREONIANIN},
	{	"fia",			RACE_FIA		},
	{	"ithorisnin",	RACE_ITHORISNIN	},
	{	"khommowin",	RACE_KHOMMOWIN	},
	{	"duch-lorda-sith",RACE_DUCH_SITH},
	{	"duch-mistrza-jedi",RACE_DUCH_JEDI},
	{	"ssi-ruuk",		RACE_SSI_RUUK	},
	{	"p'w'eck",		RACE_P_W_ECK	},
	{	"h'drach",		RACE_H_DRACH	},
	{	"coway",		RACE_COWAY		},
	{	"zieleniak",	RACE_ZIELENIAK	},
	{	"mrlss",		RACE_MRLSS		},
	{	"wyrwulf",		RACE_WYRWULF	},
	{	"codru-ji",		RACE_CODRU_JI	},
	{	"net",			RACE_NET		},
	{	"gops",			RACE_GOPS		},
	{	"drovianin",	RACE_DROVIANIN	},
	{	"yaveth",		RACE_YAVETH		},
	{	"ysanna",		RACE_YSANNA		},
	{	"pydyrian",		RACE_PYDYRIAN	},
	{	"toka",			RACE_TOKA		},
	{	"sullustianin",RACE_SULLUSTIANIN},
	{	"tusken",		RACE_TUSKEN		},
	{	"vratix",		RACE_VRATIX		},
	{	"tynnanin",		RACE_TYNNANIN	},
	{	"vors",			RACE_VORS		},
	{	"psadan",		RACE_PSADAN		},
	{	"myneyrsh",		RACE_MYNEYRSH	},
	{	"massassi",		RACE_MASSASSI	},
	{	"melodianin",	RACE_MELODIANIN	},
	{	"t'landa-til",	RACE_T_LANDA_TIL},

	{	"", 			0				}
};
*/

const	struct	flag_type	spice_types_list[ ]	=
{
	{	"glitterstim", 		SPICE_GLITTERSTIM	},
	{	"carsanum", 		SPICE_CARSANUM		},
	{	"ryll", 		SPICE_RYLL		},
	{	"andris", 		SPICE_ANDRIS		},
	{	"", 			0 			}
};

const	struct	flag_type	crystal_types_list[ ]	=
{
	{	"non_adegen", 		GEM_NON_ADEGEN		},
	{	"kathracite", 		GEM_KATHRACITE		},
	{	"relacite", 		GEM_RELACITE		},
	{	"danite", 		GEM_DANITE		},
	{	"mephite", 		GEM_MEPHITE		},
	{	"ponite", 		GEM_PONITE		},
	{	"illum", 		GEM_ILLUM		},
	{	"corusca", 		GEM_CORUSCA		},
	{	"", 			0 			}
};

const	struct	flag_type	sector_types_list[ ]	=
{
	{	"inside", 		SECT_INSIDE		},
	{	"city", 		SECT_CITY		},
	{	"field", 		SECT_FIELD		},
	{	"forest", 		SECT_FOREST		},
	{	"hills", 		SECT_HILLS		},
	{	"mountain", 		SECT_MOUNTAIN		},
	{	"water_swim",		SECT_WATER_SWIM		},
	{	"water_noswim", 	SECT_WATER_NOSWIM	},
	{	"underwater", 		SECT_UNDERWATER		},
	{	"air", 			SECT_AIR		},
	{	"desert",		SECT_DESERT		},
	{	"dunno",		SECT_DUNNO		},
	{	"oceanfloor",		SECT_OCEANFLOOR		},
	{	"underground",		SECT_UNDERGROUND	},
	{	"jail_corridor",	SECT_JAIL_CORRIDOR	},
	{	"jail_office", 		SECT_JAIL_OFFICE	},
	{	"jail_cell", 		SECT_JAIL_CELL		},
	{	"jail_entrance",	SECT_JAIL_ENTRANCE	},
	{	"", 			0 			}
};

const	struct	flag_type	exit_flags_list[ ]	=
{
	{	"door", 		EX_ISDOOR		},
	{	"closed", 		EX_CLOSED		},
	{	"locked", 		EX_LOCKED		},
	{	"secret", 		EX_SECRET		},
	{	"swim", 		EX_SWIM			},
	{	"pickproof", 		EX_PICKPROOF		},
	{	"fly", 			EX_FLY			},
	{	"climb", 		EX_CLIMB		},
	{	"dig", 			EX_DIG			},
	{	"nopass", 		EX_NOPASSDOOR		},
	{	"hidden", 		EX_HIDDEN		},
	{	"passage", 		EX_PASSAGE		},
	{	"portal", 		EX_PORTAL		},
	{	"res1", 		EX_RES1			},
	{	"res2", 		EX_RES2			},
	{	"res3", 		EX_RES3			},
	{	"res4", 		EX_RES4			},
	{	"can_climb", 		EX_xCLIMB		},
	{	"can_enter", 		EX_xENTER		},
	{	"can_leave", 		EX_xLEAVE		},
	{	"can_look", 		EX_xLOOK		},
	{	"auto", 		EX_xAUTO		},
	{	"searchable", 		EX_xSEARCHABLE		},
	{	"bashed", 		EX_BASHED		},
	{	"bashproof", 		EX_BASHPROOF		},
	{	"nomob", 		EX_NOMOB		},
	{	"window", 		EX_WINDOW		},
	{	"", 			0 			}
};

const	struct	flag_type	room_flags_list[ ]	=
{
	{	"dark",				ROOM_DARK		},
	{	"nofind",			ROOM_NO_FIND		},
	{	"nomob",	 		ROOM_NO_MOB		},
	{	"indoors",			ROOM_INDOORS		},
	{	"can_land", 		ROOM_CAN_LAND		},
	{	"can_fly",			ROOM_CAN_FLY		},
	{	"nodriving",		ROOM_NO_DRIVING		},
	{	"noforce",			ROOM_NO_FORCE		},
	{	"bank",				ROOM_BANK		},
	{	"private",			ROOM_PRIVATE		},
	{	"safe",				ROOM_SAFE		},
	{	"import",			ROOM_IMPORT		},
	{	"bacta",			ROOM_BACTA		},
	{	"donation", 		ROOM_DONATION		},
	{	"nodropall", 		ROOM_NODROPALL		},
	{	"silence",			ROOM_SILENCE		},
	{	"logspeech", 		ROOM_LOGSPEECH		},
	{	"nodrop", 			ROOM_NODROP		},
    {	"stock_market",		ROOM_STOCK_MARKET	},
	{	"plr_home", 		ROOM_PLR_HOME		},
	{	"empty_home", 		ROOM_EMPTY_HOME		},
	{	"teleport", 		ROOM_TELEPORT		},
	{	"hotel",			ROOM_HOTEL			},
	{	"nofloor",			ROOM_NOFLOOR		},
	{	"refinery", 		ROOM_REFINERY		},
	{	"factory",			ROOM_FACTORY		},
	{	"killarena",		ROOM_KILLARENA		},
	{	"safearena",		ROOM_SAFEARENA		},
	{	"spacecraft", 		ROOM_SPACECRAFT		},
	{	"prototype", 		ROOM_PROTOTYPE		},
	{	"auction",	 		ROOM_AUCTION		},
	{	"storage",			ROOM_STORAGE		},
	{	"",					0 					}
};

const	struct	flag_type	obj_flags_list[ ]	=
{
	{	"glow", 		ITEM_GLOW		},
	{	"hum", 			ITEM_HUM		},
	{	"hutt_size", 		ITEM_HUTT_SIZE		},
	{	"contraband", 		ITEM_CONTRABAND		},
	{	"invis", 		ITEM_INVIS		},
	{	"force", 		ITEM_FORCE		},
	{	"nodrop", 		ITEM_NODROP		},
	{	"bless", 		ITEM_BLESS		},
	{	"anti_good", 		ITEM_ANTI_GOOD		},
	{	"anti_evil", 		ITEM_ANTI_EVIL		},
	{	"anti_neutral", 	ITEM_ANTI_NEUTRAL	},
	{	"noremove", 		ITEM_NOREMOVE		},
	{	"inventory", 		ITEM_INVENTORY		},
	{	"anti_soldier",		ITEM_ANTI_SOLDIER	},
	{	"anti_thief",		ITEM_ANTI_THIEF		},
	{	"anti_hunter",		ITEM_ANTI_HUNTER	},
	{	"anti_jedi",		ITEM_ANTI_JEDI		},
	{	"anti_sith",		ITEM_ANTI_SITH		},
	{	"anti_pilot",		ITEM_ANTI_PILOT		},
	{	"small_size", 		ITEM_SMALL_SIZE		},
	{	"large_size", 		ITEM_LARGE_SIZE		},
	{	"donation", 		ITEM_DONATION		},
	{	"clan_obj", 		ITEM_CLANOBJECT		},
	{	"anti_citizen",		ITEM_ANTI_CITIZEN	},
	{	"hidden", 		ITEM_HIDDEN		},
	{	"poisoned",		ITEM_POISONED		},
	{	"covering",		ITEM_COVERING		},
	{	"personal",		ITEM_PERSONAL		},
	{	"burried",		ITEM_BURRIED		},
	{	"prototype",		ITEM_PROTOTYPE		},
	{	"quest",		ITEM_QUEST		},
	{	"human_size",		ITEM_HUMAN_SIZE		},
	{	"", 			0 			}
};

const	struct	flag_type	force_flags_list[ ]	=
{
	{	"returning", 		ITEM_RETURNING		},
	{	"backstabber", 		ITEM_BACKSTABBER	},
	{	"bane", 		ITEM_BANE		},
	{	"loyal", 		ITEM_LOYAL		},
	{	"haste", 		ITEM_HASTE		},
	{	"drain", 		ITEM_DRAIN		},
	{	"lightning_blade", 	ITEM_LIGHTNING_BLADE	},
	{	"", 			0 			}
};

const	struct	flag_type	obj_types_list[ ]	=
{
	{	"none", 		ITEM_NONE		},
	{	"toolkit", 		ITEM_TOOLKIT		},
	{	"durasteel", 		ITEM_DURASTEEL		},
	{	"light", 		ITEM_LIGHT		},
	{	"weapon", 		ITEM_WEAPON		},
	{	"treasure", 		ITEM_TREASURE		},
	{	"armor", 		ITEM_ARMOR		},
	{	"potion", 		ITEM_POTION		},
	{	"trash", 		ITEM_TRASH		},
	{	"container", 		ITEM_CONTAINER		},
	{	"drink_con", 		ITEM_DRINK_CON		},
	{	"key", 			ITEM_KEY		},
	{	"food", 		ITEM_FOOD		},
	{	"money", 		ITEM_MONEY		},
	{	"corpse_npc",		ITEM_CORPSE_NPC		},
	{	"corpse_pc",		ITEM_CORPSE_PC		},
	{	"fountain",		ITEM_FOUNTAIN		},
	{	"pill",			ITEM_PILL		},
	{	"scraps",		ITEM_SCRAPS		},
	{	"pipe",			ITEM_PIPE		},
	{	"herb_con",		ITEM_HERB_CON		},
	{	"herb",			ITEM_HERB		},
	{	"switch",		ITEM_SWITCH		},
	{	"lever",		ITEM_LEVER		},
	{	"pullchain",		ITEM_PULLCHAIN		},
	{	"button",		ITEM_BUTTON		},
	{	"trap",			ITEM_TRAP		},
	{	"portal",		ITEM_PORTAL		},
	{	"paper",		ITEM_PAPER		},
	{	"ammo",			ITEM_AMMO		},
	{	"shovel",		ITEM_SHOVEL		},
	{	"salve",		ITEM_SALVE		},
	{	"rawspice",		ITEM_RAWSPICE		},
	{	"lens",			ITEM_LENS		},
	{	"crystal",		ITEM_CRYSTAL		},
	{	"battery",		ITEM_BATTERY		},
	{	"oven",			ITEM_OVEN		},
	{	"duraplast",		ITEM_DURAPLAST		},
	{	"circuit",		ITEM_CIRCUIT		},
	{	"superconductor",	ITEM_SUPERCONDUCTOR	},
	{	"comlink",		ITEM_COMLINK		},
	{	"medpac",		ITEM_MEDPAC		},
	{	"fabric",		ITEM_FABRIC		},
	{	"rare_metal",		ITEM_RARE_METAL		},
	{	"thread",		ITEM_THREAD		},
	{	"spice",		ITEM_SPICE		},
	{	"smut",			ITEM_SMUT		},
	{	"device",		ITEM_DEVICE		},
	{	"shipmodule",		ITEM_SHIPMODULE		},
	{	"grenade",		ITEM_GRENADE		},
	{	"landmine",		ITEM_LANDMINE		},
	{	"droid_corpse",		ITEM_DROID_CORPSE	},
	{	"bolt",			ITEM_BOLT		},
	{	"chemical",		ITEM_CHEMICAL		},
	{	"piece",		ITEM_PIECE		},
	{	"shipdevice",		ITEM_SHIPDEVICE		},
	{	"computer",		ITEM_COMPUTER		},
	{	"shiphckdev",		ITEM_SHIPHCKDEV		},
	{	"pazaakcard",	ITEM_PAZAAK_CARD		},
	{	"pazaaksidedeck",	ITEM_PAZAAK_SIDE_DECK		},
	{	"", 			0 			}
};

const	struct	flag_type	aff_flags_list[ ]	=
{
	{	"blind",		AFF_BLIND		},
	{	"invis",		AFF_INVISIBLE		},
	{	"detect_evil",		AFF_DETECT_EVIL		},
	{	"detect_invis",		AFF_DETECT_INVIS	},
	{	"detect_force",		AFF_DETECT_FORCE	},
	{	"detect_hidden",	AFF_DETECT_HIDDEN	},
	{	"weaken",		AFF_WEAKEN		},
	{	"sanctuary",		AFF_SANCTUARY		},
	{	"faerie_fire",		AFF_FAERIE_FIRE		},
	{	"infrared",		AFF_INFRARED		},
	{	"curse",		AFF_CURSE		},
	{	"dominated",		AFF_DOMINATED		},
	{	"poison",		AFF_POISON		},
	{	"protect",		AFF_PROTECT		},
	{	"paralysis",		AFF_PARALYSIS		},
	{	"sneak",		AFF_SNEAK		},
	{	"hide",			AFF_HIDE		},
	{	"sleep",		AFF_SLEEP		},
	{	"charm",		AFF_CHARM		},
	{	"flying",		AFF_FLYING		},
	{	"passdoor",		AFF_PASS_DOOR		},
	{	"floating",		AFF_FLOATING		},
	{	"truesight",		AFF_TRUESIGHT		},
	{	"detect_traps",		AFF_DETECTTRAPS		},
	{	"scrying",		AFF_SCRYING		},
	{	"forceinvis",		AFF_FORCEINVIS		},
	{	"shockshiel",		AFF_SHOCKSHIELD		},
	{	"disguise",		AFF_DISGUISE		},
	{	"iceshield",		AFF_ICESHIELD		},
	{	"possess",		AFF_POSSESS		},
	{	"berserk",		AFF_BERSERK		},
	{	"aqua_breath",		AFF_AQUA_BREATH		},
	{	"", 			0 			}
};

const	struct	flag_type	apply_types_list[ ]	=
{
	{	"none",			APPLY_NONE		},
	{	"strength",		APPLY_STR 		},
	{	"dexterity",		APPLY_DEX 		},
	{	"intelligence", 	APPLY_INT		},
	{	"wisdom",		APPLY_WIS 		},
	{	"constitution",		APPLY_CON 		},
	{	"charisma",		APPLY_CHA 		},
	{	"luck", 		APPLY_LCK 		},
	{	"sex", 			APPLY_SEX 		},
	{	"height",		APPLY_HEIGHT		},
	{	"weight",		APPLY_WEIGHT 		},
	{	"mana", 		APPLY_MANA 		},
	{	"hit", 			APPLY_HIT 		},
	{	"move",			APPLY_MOVE 		},
	{	"ac", 			APPLY_AC 		},
	{	"hitroll",		APPLY_HITROLL 		},
	{	"damroll",		APPLY_DAMROLL 		},
	{	"sav_poison",		APPLY_SAVING_POISON	},
	{	"sav_rod",		APPLY_SAVING_ROD 	},
	{	"sav_para",		APPLY_SAVING_PARA 	},
	{	"sav_breath",		APPLY_SAVING_BREATH 	},
	{	"sav_spell",		APPLY_SAVING_SPELL 	},
	{	"affect",		APPLY_AFFECT 		},
	{	"resistant", 		APPLY_RESISTANT 	},
	{	"immune",		APPLY_IMMUNE 		},
	{	"susceptible",		APPLY_SUSCEPTIBLE 	},
	{	"weaponspell",		APPLY_WEAPONSPELL 	},
	{	"backstab",		APPLY_BACKSTAB 		},
	{	"pick",			APPLY_PICK 		},
	{	"track",		APPLY_TRACK 		},
	{	"steal",		APPLY_STEAL 		},
	{	"sneak",		APPLY_SNEAK 		},
	{	"snipe",		APPLY_SNIPE 		},
	{	"hide",			APPLY_HIDE 		},
	{	"detrap",		APPLY_DETRAP 		},
	{	"dodge",		APPLY_DODGE 		},
	{	"peek",			APPLY_PEEK 		},
	{	"scan",			APPLY_SCAN 		},
	{	"gouge",		APPLY_GOUGE 		},
	{	"search",		APPLY_SEARCH 		},
	{	"mount",		APPLY_MOUNT 		},
	{	"disarm",		APPLY_DISARM 		},
	{	"kick",			APPLY_KICK 		},
	{	"parry",		APPLY_PARRY 		},
	{	"bash",			APPLY_BASH 		},
	{	"stun",			APPLY_STUN 		},
	{	"punch",		APPLY_PUNCH 		},
	{	"climb",		APPLY_CLIMB 		},
	{	"grip",			APPLY_GRIP 		},
	{	"scribe",		APPLY_SCRIBE 		},
	{	"brew",			APPLY_BREW 		},
	{	"wearspell",		APPLY_WEARSPELL 	},
	{	"removespell",		APPLY_REMOVESPELL 	},
	{	"emotion",		APPLY_EMOTION 		},
	{	"mentalstate",		APPLY_MENTALSTATE 	},
	{	"stripsn",		APPLY_STRIPSN 		},
	{	"remove", 		APPLY_REMOVE 		},
	{	"dig", 			APPLY_DIG 		},
	{	"full", 		APPLY_FULL 		},
	{	"thirst",		APPLY_THIRST 		},
	{	"drunk",		APPLY_DRUNK 		},
	{	"blood",		APPLY_BLOOD 		},
	{	"", 			0 			}
};

const	struct	flag_type	req_types_list[ ]	=
{
	{	"none",			REQ_NONE		},
	{	"strength",		REQ_STR			},
	{	"dexterity",		REQ_DEX			},
	{	"intelligence",		REQ_INT			},
	{	"wisdom",		REQ_WIS			},
	{	"constitution",		REQ_CON			},
	{	"charisma",		REQ_CHA			},
	{	"luck",			REQ_LCK 		},
	{	"force",		REQ_FRC			},
	{	"sex",			REQ_SEX			},
	{	"credits",		REQ_GOLD		},
	{	"age",			REQ_AGE			},
	{	"hp",			REQ_HIT 		},
	{	"force_points",		REQ_FORCE		},
	{	"move",			REQ_MOVE		},
	{	"level",		REQ_LEVEL		},
	{	"ac",			REQ_AC			},
	{	"hitroll",		REQ_HITROLL		},
	{	"damroll",		REQ_DAMROLL		},
	{	"full",			REQ_FULL 		},
	{	"thirst",		REQ_THIRST 		},
	{	"drunk",		REQ_DRUNK 		},
	{	"race",			REQ_RACE 		},
	{	"skill",		REQ_SKILL		},
	{	"affect",		REQ_AFFECT		},
	{	"class",		REQ_CLASS		},
	{	"", 			0 			}
};

const	struct	flag_type	class_types_list[ ]	=
{
	{	"none",			ABILITY_NONE		},
	{	"combat",		COMBAT_ABILITY 		},
	{	"pilotning",		PILOTING_ABILITY	},
	{	"engineering",		ENGINEERING_ABILITY	},
	{	"hunting",		HUNTING_ABILITY		},
	{	"smuggling",		SMUGGLING_ABILITY	},
	{	"diplomacy",		DIPLOMACY_ABILITY	},
	{	"leadership",		LEADERSHIP_ABILITY	},
	{	"force",		FORCE_ABILITY		},
	{	"", 			0 			}
};

const 	struct 	flag_type	log_types_list[ ] 	=
{
	{  	"normal",		LOG_NORMAL		},
	{  	"always",		LOG_ALWAYS		},
	{  	"never",		LOG_NEVER		},
	{  	"build",		LOG_BUILD		},
	{  	"high",			LOG_HIGH		},
	{  	"comm",			LOG_COMM		},
	{  	"prog",			LOG_PROG		},
	{  	"all",			LOG_ALL			},
	{  	"bug",			LOG_BUG			},
	{	"", 			0 			}
};

const 	struct 	flag_type	granadetypes_list[ ] 	=
{
	{  	"explosive",		GRANADE_EXPLOSIVE	},
	{  	"poison",		GRANADE_POISON		},
	{  	"stun",			GRANADE_STUN		},
	{  	"blind",		GRANADE_BLIND		},
	{	"", 			0 			}
};

const	struct	flag_type	act_flags_list[ ]	=
{
	{	"npc", 			ACT_IS_NPC		},
	{	"sentinel",		ACT_SENTINEL		},
	{	"scavenger",		ACT_SCAVENGER		},
	{	"aggressive",		ACT_AGGRESSIVE		},
	{	"stay_area", 		ACT_STAY_AREA		},
	{	"wimpy", 		ACT_WIMPY		},
	{	"pet", 			ACT_PET			},
	{	"train", 		ACT_TRAIN		},
	{	"practice", 		ACT_PRACTICE		},
	{	"immortal", 		ACT_IMMORTAL		},
	{	"deadly", 		ACT_DEADLY		},
	{	"bountyofficer",	ACT_BOUNTYOFFICER	},
	{	"meta_aggr", 		ACT_META_AGGR		},
	{	"guardian", 		ACT_GUARDIAN		},
	{	"running", 		ACT_RUNNING		},
	{	"nowander", 		ACT_NOWANDER		},
	{	"mountable", 		ACT_MOUNTABLE		},
	{	"mounted", 		ACT_MOUNTED		},
	{	"scholar", 		ACT_SCHOLAR		},
	{	"secretive", 		ACT_SECRETIVE		},
	{	"polymorphed", 		ACT_POLYMORPHED		},
	{	"mobinvis", 		ACT_MOBINVIS		},
	{	"noassist", 		ACT_NOASSIST		},
	{	"nokill", 		ACT_NOKILL		},
	{	"droid", 		ACT_DROID		},
	{	"nocorpse", 		ACT_NOCORPSE		},
	{	"quest", 		ACT_QUEST		},
	{	"prototype", 		ACT_PROTOTYPE		},
	{	"", 			0 			}
};

const	struct	flag_type	pc_flags_list[ ]	=
{
	{	"r1", 			PCFLAG_R1		},
	{	"nh",			PCFLAG_NH		},
	{	"unauthed", 		PCFLAG_UNAUTHED 	},
	{	"nointro",		PCFLAG_NOINTRO 		},
	{	"gag", 			PCFLAG_GAG 		},
	{	"retired",		PCFLAG_RETIRED 		},
	{	"guest",		PCFLAG_GUEST 		},
	{	"pageron",		PCFLAG_PAGERON 		},
	{	"room", 		PCFLAG_ROOM 		},
	{	"shipstat", 	PCFLAG_SHIPSTAT		},
	{	"", 			0 			}
};

const	struct	flag_type	plr_flags_list[ ]	=
{
	{	"npc", 			PLR_IS_NPC 		},
	{	"bought_pet",		PLR_BOUGHT_PET 		},
	{	"autoexit",		PLR_AUTOEXIT 		},
	{	"autoloot",		PLR_AUTOLOOT 		},
	{	"blank",		PLR_BLANK 		},
	{	"outcast",		PLR_OUTCAST 		},
	{	"brief",		PLR_BRIEF 		},
	{	"combine", 		PLR_COMBINE 		},
	{	"prompt",		PLR_PROMPT 		},
	{	"telnet_ga",		PLR_TELNET_GA 		},
	{	"holylight",		PLR_HOLYLIGHT 		},
	{	"wizinvis",		PLR_WIZINVIS 		},
	{	"roomvnum",		PLR_ROOMVNUM 		},
	{	"tick",			PLR_TICK 		},
	{	"attacker",		PLR_ATTACKER 		},
	{	"log", 			PLR_LOG 		},
	{	"deny",			PLR_DENY 		},
	{	"freeze",		PLR_FREEZE 		},
	{	"killer",		PLR_KILLER 		},
	{	"stbar", 		PLR_STATUS_BAR 		},
	{	"litterbug",		PLR_LITTERBUG 		},
	{	"ansi",			PLR_ANSI 		},
	{	"sound",		PLR_SOUND 		},
	{	"nice",			PLR_NICE 		},
	{	"flee",			PLR_FLEE 		},
	{	"autocred",		PLR_AUTOGOLD 		},
	{	"away",			PLR_AFK 		},
	{	"longexits",		PLR_LONGEXITS 		},
	{   "mapexits",		PLR_MAPEXITS	},
	{	"", 			0 			}
};

const	struct	flag_type	trap_flags_list[ ]	=
{
	{	"room",			TRAP_ROOM		},
	{	"obj", 			TRAP_OBJ		},
	{	"enter",		TRAP_ENTER_ROOM		},
	{	"leave",		TRAP_LEAVE_ROOM		},
	{	"open",			TRAP_OPEN		},
	{	"close", 		TRAP_CLOSE		},
	{	"get", 			TRAP_GET		},
	{	"put", 			TRAP_PUT		},
	{	"pick",			TRAP_PICK		},
	{	"unlock",		TRAP_UNLOCK		},
	{	"n", 			TRAP_N			},
	{	"s", 			TRAP_S			},
	{	"e", 			TRAP_E			},
	{	"w", 			TRAP_W			},
	{	"u", 			TRAP_U			},
	{	"d", 			TRAP_D			},
	{	"examine",		TRAP_EXAMINE		},
	{	"ne", 			TRAP_NE			},
	{	"nw", 			TRAP_NW			},
	{	"se", 			TRAP_SE			},
	{	"sw", 			TRAP_SW			},
	{	"", 			0 			}
};

const	struct	flag_type	trap_types_list[ ]	=
{
	{	"poison_gas",		TRAP_TYPE_POISON_GAS	},
	{	"poison_dart",		TRAP_TYPE_POISON_DART	},
	{	"poison_needle",	TRAP_TYPE_POISON_NEEDLE	},
	{	"poison_dagger",	TRAP_TYPE_POISON_DAGGER	},
	{	"poison_arrow", 	TRAP_TYPE_POISON_ARROW	},
	{	"blindness_gas",	TRAP_TYPE_BLINDNESS_GAS	},
	{	"sleeping_gas",		TRAP_TYPE_SLEEPING_GAS	},
	{	"flame",		TRAP_TYPE_FLAME		},
	{	"explosion",		TRAP_TYPE_EXPLOSION	},
	{	"acid_spray", 		TRAP_TYPE_ACID_SPRAY	},
	{	"electric_shock",	TRAP_TYPE_ELECTRIC_SHOCK},
	{	"blade",		TRAP_TYPE_BLADE		},
	{	"sex_change",		TRAP_TYPE_SEX_CHANGE	},
	{	"", 			0 			}
};

const	struct	flag_type	wear_types_list[ ]	=
{
	{	"none", 		WEAR_NONE		},
	{	"light",		WEAR_LIGHT		},
	{	"finger_l",		WEAR_FINGER_L		},
	{	"finger_r",		WEAR_FINGER_R		},
	{	"neck_1",		WEAR_NECK_1		},
	{	"neck_2",		WEAR_NECK_2		},
	{	"body",			WEAR_BODY		},
	{	"head",			WEAR_HEAD		},
	{	"legs",			WEAR_LEGS		},
	{	"feet",			WEAR_FEET		},
	{	"hands",		WEAR_HANDS		},
	{	"arms",			WEAR_ARMS		},
	{	"shield",		WEAR_SHIELD		},
	{	"about",		WEAR_ABOUT		},
	{	"waist",		WEAR_WAIST		},
	{	"wrist_l",		WEAR_WRIST_L		},
	{	"wrist_r",		WEAR_WRIST_R		},
	{	"wield", 		WEAR_WIELD		},
	{	"hold",			WEAR_HOLD		},
	{	"dual_wield",		WEAR_DUAL_WIELD		},
	{	"ears",			WEAR_EARS		},
	{	"eyes", 		WEAR_EYES		},
	{	"missile_wield",	WEAR_MISSILE_WIELD	},
	{	"", 			0 			}
};

const	struct	flag_type	wear_flags_list[ ]	=
{
	{	"take",			ITEM_TAKE		},
	{	"finger", 		ITEM_WEAR_FINGER	},
	{	"neck", 		ITEM_WEAR_NECK		},
	{	"body", 		ITEM_WEAR_BODY		},
	{	"head",			ITEM_WEAR_HEAD		},
	{	"legs",			ITEM_WEAR_LEGS		},
	{	"feet",			ITEM_WEAR_FEET		},
	{	"hands",		ITEM_WEAR_HANDS		},
	{	"arms",			ITEM_WEAR_ARMS		},
	{	"shield",		ITEM_WEAR_SHIELD	},
	{	"about",		ITEM_WEAR_ABOUT		},
	{	"waist",		ITEM_WEAR_WAIST		},
	{	"wrist",		ITEM_WEAR_WRIST		},
	{	"wield",		ITEM_WIELD		},
	{	"hold",			ITEM_HOLD		},
	{	"dual_wield",		ITEM_DUAL_WIELD		},
	{	"ears",			ITEM_WEAR_EARS		},
	{	"eyes",			ITEM_WEAR_EYES		},
	{	"wield",		ITEM_MISSILE_WIELD	},
	{	"", 			0 			}
};

/* mniej, bo to tylko do skilli in\xbfyniera */
const	struct	flag_type	pl_wear_flags_list[ ]	=
{
	{	"palce", 		ITEM_WEAR_FINGER	},
	{	"szyja", 		ITEM_WEAR_NECK		},
	{	"cia\xb3o", 		ITEM_WEAR_BODY		},
	{	"g\xb3owa",		ITEM_WEAR_HEAD		},
	{	"nogi",			ITEM_WEAR_LEGS		},
	{	"stopy",		ITEM_WEAR_FEET		},
	{	"r\xea" "ce",			ITEM_WEAR_HANDS		},
	{	"ramiona",		ITEM_WEAR_ARMS		},
	{	"tarcza",		ITEM_WEAR_SHIELD	},
	{	"pas",			ITEM_WEAR_WAIST		},
	{	"nadgarstek",		ITEM_WEAR_WRIST		},
	{	"bro\xf1",			ITEM_WIELD		},
	{	"trzymany",		ITEM_HOLD		},
	{	"uszy",			ITEM_WEAR_EARS		},
	{	"oczy",			ITEM_WEAR_EYES		},
	{	"", 			0 			}
};

const	struct	flag_type	ris_flags_list[ ]	=
{
	{	"fire",			RIS_FIRE		},
	{	"cold",			RIS_COLD		},
	{	"electricity",	RIS_ELECTRICITY	},
	{	"energy",		RIS_ENERGY		},
	{	"blunt",		RIS_BLUNT		},
	{	"pierce",		RIS_PIERCE		},
	{	"slash",		RIS_SLASH		},
	{	"acid",			RIS_ACID		},
	{	"poison",		RIS_POISON		},
	{	"drain", 		RIS_DRAIN		},
	{	"sleep", 		RIS_SLEEP		},
	{	"charm", 		RIS_CHARM		},
	{	"hold", 		RIS_HOLD		},
	{	"nonforce",		RIS_NONFORCE	},
	{	"plus1", 		RIS_PLUS1		},
	{	"plus2", 		RIS_PLUS2		},
	{	"plus3", 		RIS_PLUS3		},
	{	"plus4", 		RIS_PLUS4		},
	{	"plus5", 		RIS_PLUS5		},
	{	"plus6", 		RIS_PLUS6		},
	{	"force", 		RIS_FORCE		},
	{	"paralysis", 	RIS_PARALYSIS	},
	{	"", 			0 			}
};

const	struct	flag_type	trig_flags_list[ ]	=
{
	{	"up", 			TRIG_UP			},
	{	"unlock",		TRIG_UNLOCK		},
	{	"lock", 		TRIG_LOCK		},
	{	"d_north", 		TRIG_D_NORTH		},
	{	"d_south", 		TRIG_D_SOUTH		},
	{	"d_east", 		TRIG_D_EAST		},
	{	"d_west", 		TRIG_D_WEST		},
	{	"d_up", 		TRIG_D_UP		},
	{	"d_down", 		TRIG_D_DOWN		},
	{	"door", 		TRIG_DOOR		},
	{	"container", 		TRIG_CONTAINER		},
	{	"open", 		TRIG_OPEN		},
	{	"close", 		TRIG_CLOSE		},
	{	"passage", 		TRIG_PASSAGE		},
	{	"oload", 		TRIG_OLOAD		},
	{	"mload", 		TRIG_MLOAD		},
	{	"teleport", 		TRIG_TELEPORT		},
	{	"teleportall", 		TRIG_TELEPORTALL	},
	{	"teleportplus", 	TRIG_TELEPORTPLUS	},
	{	"death", 		TRIG_DEATH		},
	{	"cast", 		TRIG_CAST		},
	{	"fakeblade", 		TRIG_FAKEBLADE		},
	{	"rand4", 		TRIG_RAND4		},
	{	"rand6", 		TRIG_RAND6		},
	{	"trapdoor", 		TRIG_TRAPDOOR		},
	{	"anotheroom", 		TRIG_ANOTHEROOM		},
	{	"usedial", 		TRIG_USEDIAL		},
	{	"absolutevnum", 	TRIG_ABSOLUTEVNUM	},
	{	"showroomdesc", 	TRIG_SHOWROOMDESC	},
	{	"autoreturn", 		TRIG_AUTORETURN		},
	{	"", 			0 			}
};

const	struct	flag_type	part_flags_list[ ]	=
{
	{	"head", 		PART_HEAD		},
	{	"arms",			PART_ARMS		},
	{	"legs",			PART_LEGS		},
	{	"heart",		PART_HEART		},
	{	"brains",		PART_BRAINS		},
	{	"guts",			PART_GUTS		},
	{	"hands",		PART_HANDS		},
	{	"feet",			PART_FEET		},
	{	"fingers",		PART_FINGERS		},
	{	"ear", 			PART_EAR		},
	{	"eye", 			PART_EYE		},
	{	"long_tongue",		PART_LONG_TONGUE	},
	{	"eyestalks",		PART_EYESTALKS		},
	{	"tentacles", 		PART_TENTACLES		},
	{	"fins", 		PART_FINS		},
	{	"wings", 		PART_WINGS		},
	{	"tail",			PART_TAIL		},
	{	"scales",		PART_SCALES		},
	{	"", 			0 			}
};

const	struct	flag_type	attack_flags_list[ ]	=
{
	{	"bite", 		ATCK_BITE		},
	{	"claws",		ATCK_CLAWS		},
	{	"tail", 		ATCK_TAIL		},
	{	"sting",		ATCK_STING		},
	{	"punch", 		ATCK_PUNCH		},
	{	"kick",			ATCK_KICK		},
	{	"trip",			ATCK_TRIP		},
	{	"bash", 		ATCK_BASH		},
	{	"stun", 		ATCK_STUN		},
	{	"gouge",		ATCK_GOUGE		},
	{	"", 			0 			}
};

const	struct	flag_type	def_flags_list[ ]	=
{
	{	"parry",		DFND_PARRY 		},
	{	"dodge",		DFND_DODGE 		},
	{	"", 			0 			}
};

const	struct	flag_type	mprog_flags_list[ ]	=
{
	{	"in_file_prog", 	IN_FILE_PROG		},
	{	"act_prog", 		ACT_PROG		},
	{	"speech_prog", 		SPEECH_PROG		},
	{	"rand_prog", 		RAND_PROG		},
	{	"fight_prog", 		FIGHT_PROG		},
	{	"rfight_prog", 		RFIGHT_PROG		},
	{	"death_prog", 		DEATH_PROG		},
	{	"rdeath_prog", 		RDEATH_PROG		},
	{	"hitprcnt_prog",	HITPRCNT_PROG		},
	{	"entry_prog", 		ENTRY_PROG		},
	{	"enter_prog", 		ENTER_PROG		},
	{	"greet_prog", 		GREET_PROG		},
	{	"rgreet_prog", 		RGREET_PROG		},
	{	"ogreet_prog", 		OGREET_PROG		},
	{	"all_greet_prog",	ALL_GREET_PROG		},
	{	"give_prog", 		GIVE_PROG		},
	{	"bribe_prog", 		BRIBE_PROG		},
	{	"hour_prog", 		HOUR_PROG		},
	{	"time_prog", 		TIME_PROG		},
	{	"wear_prog", 		WEAR_PROG		},
	{	"remove_prog", 		REMOVE_PROG		},
	{	"sac_prog", 		SAC_PROG		},
	{	"tell_prog", 		TELL_PROG		},
	{	"exa_prog",	 	EXA_PROG		},
	{	"zap_prog",		ZAP_PROG		},
	{	"get_prog",		GET_PROG		},
	{	"drop_prog", 		DROP_PROG		},
	{	"damage_prog", 		DAMAGE_PROG		},
	{	"repair_prog", 		REPAIR_PROG		},
	{	"quest_prog", 		QUEST_PROG		},
	{	"speechiw_prog",	SPEECHIW_PROG		},
	{	"pull_prog", 		PULL_PROG		},
	{	"push_prog", 		PUSH_PROG		},
	{	"sleep_prog", 		SLEEP_PROG		},
	{	"rest_prog", 		REST_PROG		},
	{	"leave_prog", 		LEAVE_PROG		},
	{	"script_prog", 		SCRIPT_PROG		},
	{	"use_prog", 		USE_PROG		},
	{	"dialog_prog",		DIALOG_PROG		},
	{	"extradesc_prog",	EXTRADESC_PROG	},
	{	"", 			0 			}
};

const	struct	flag_type	save_flags_list[ ]	=
{
	{	"death",		SV_DEATH 		},
	{	"kill",			SV_KILL 		},
	{	"passchg",		SV_PASSCHG 		},
	{	"drop", 		SV_DROP 		},
	{	"put", 			SV_PUT 			},
	{	"give",			SV_GIVE 		},
	{	"auto",			SV_AUTO 		},
	{	"zapdrop",		SV_ZAPDROP 		},
	{	"auction", 		SV_AUCTION 		},
	{	"get", 			SV_GET 			},
	{	"receive",		SV_RECEIVE 		},
	{	"idle",			SV_IDLE 		},
	{	"backup",		SV_BACKUP 		},
	{	"", 			0 			}
};

const	struct	flag_type	quest_flags_list[ ]	=
{ //by Trog
	{	"prototype",		QUEST_PROTOTYPE		},
	{	"hidden",		QUEST_HIDDEN		},
	{	"notfor_combat",	QUEST_NF_COMBAT		},
	{	"notfor_engineer",	QUEST_NF_ENGINEER	},
	{	"notfor_pilot",		QUEST_NF_PILOT		},
	{	"notfor_bounty",	QUEST_NF_BOUNTY		},
	{	"notfor_leader",	QUEST_NF_LEADER		},
	{	"notfor_diplomat",	QUEST_NF_DIPLOMAT	},
	{	"notfor_smuggler",	QUEST_NF_SMUGGLER	},
	{	"notfor_forceuser",	QUEST_NF_FORCEUSER	},
	{	"noresign",			QUEST_NORESIGN		},
	{	"repeatable",		QUEST_REPEATABLE	},
	{	"",			0			}
};

const	struct	flag_type	quest_init_types[ ]	=
{
	{	"LoadMob",		INIT_LOAD_MOB	},
	{	"LoadObj",		INIT_LOAD_OBJ	},
	{	"PutObj",		INIT_PUT_OBJ	},
	{	"EquipMob",		INIT_EQUIP_MOB	},
	{	"Purge",		INIT_PURGE		},
	{	"Transfer",		INIT_TRANSFER	},
	{	"Echo",			INIT_ECHO		},
	{	"InviteMob",	INIT_INVITE_MOB	},
	{	"Hint",			INIT_HINT		},
	{	"GiveObj",		INIT_GIVE_OBJ	},
	{	"",				0				}
};

const	struct	flag_type	quest_event_types[ ]	=
{
	{	"KillMob",		EVENT_KILL_MOB		},
	{	"FindObj",		EVENT_FIND_OBJ		},
	{	"GiveObj",		EVENT_GIVE_OBJ		},
	{	"BringObj",		EVENT_BRING_OBJ		},
	{	"VisitMob",		EVENT_VISIT_MOB		},
	{	"VisitRoom",	EVENT_VISIT_ROOM	},
	{	"LookObj",		EVENT_LOOK_OBJ		},
	{	"LookMob",		EVENT_LOOK_MOB		},
	{	"",			0			}
};

const	struct	flag_type	quest_action_types[ ]	=
{
	{   "EndQuest",		ACTION_STOPQUEST	},
	{	"Prog",			ACTION_PROG		},
	{	"Echo",			ACTION_ECHO		},
	{	"Tell",			ACTION_TELL		},
	{	"RunChapt",		ACTION_RUNCHPT		},
	{	"Reward",		ACTION_REWARD		},
	{	"None",			ACTION_NONE		},
	{	"",			0			}
};

const	struct	flag_type	quest_chapter_types[ ]	=
{
	{	"Critical",		CHAPTER_CRITICAL	},
	{	"VeryCritical",		CHAPTER_VERY_CRITICAL	},
	{	"UnCritical",		CHAPTER_UNCRITICAL	},
	{	"VeryUnCritical",	CHAPTER_VERY_UNCRITICAL	},
	{	"",			0			}
};

/* Thanos&Onyx */
const	struct	flag_type	mont_types_list[ ]	=
{

	{	"screw",		MONT_SCREW		},
	{	"solder",		MONT_SOLDER		},
	{	"weld",			MONT_WELD		},
	{	"stick",		MONT_STICK		},
	{	"plug",			MONT_PLUG		},
	{	"rivet",		MONT_RIVET		},
	{	"sew",			MONT_SEW		},
	{	"cook",			MONT_COOK		},
	{	"tool",			MONT_TOOL		},	// obr\xf3bka
	{	"", 			0				}
};


const	struct	flag_type	clan_flags_list[ ]	=
{
	{	"no_quit",		CLAN_NOQUIT		},
	{	"penalty_quit",		CLAN_PENALTYQUIT	},
	{	"no_info",		CLAN_NOINFO		},
	{	"",			0			}
};

const	struct	flag_type	crime_flags_list[ ]	=
{
	{	"steal",		CRIME_STEAL		},
	{	"hacking",		CRIME_HACKING		},
	{	"contraband",		CRIME_CONTRABAND	},
	{	"murder",		CRIME_MURDER		},
	{	"swindle",		CRIME_SWINDLE		},
	{	"",			0			}
};

const	struct	flag_type	command_flags_list[ ]	=
{
	{	"hidden",		CMD_HIDDEN		},
	{	"olccmd",		CMD_OLC			},
	{	"adminonly",	CMD_ADMIN_ONLY			},
	{	"nh",			CMD_NH			},
	{	"noorder",		CMD_NOORDER		},
	{	"helpcmd",		CMD_HELP		},
	{	"",				0		}
};

const	struct	flag_type	container_flags_list[ ]	=
{
	{	"closeable", 		CONT_CLOSEABLE		},
	{	"pickproof", 		CONT_PICKPROOF 		},
	{	"closed", 		CONT_CLOSED 		},
	{	"locked", 		CONT_LOCKED		},
	{	"", 			0 			}
};

const	struct	flag_type	dial_flags_list[ ]	=
{
	{	"up", 			DIAL_UP			},
	{	"unlock", 		DIAL_UNLOCK		},
	{	"lock", 		DIAL_LOCK		},
	{	"d_north", 		DIAL_D_NORTH		},
	{	"d_south", 		DIAL_D_SOUTH		},
	{	"d_east", 		DIAL_D_EAST		},
	{	"d_west",		DIAL_D_WEST		},
	{	"d_up", 		DIAL_D_UP		},
	{	"d_down", 		DIAL_D_DOWN		},
	{	"door", 		DIAL_DOOR		},
	{	"container",		DIAL_CONTAINER		},
	{	"open", 		DIAL_OPEN		},
	{	"close", 		DIAL_CLOSE		},
	{	"passage", 		DIAL_PASSAGE		},
	{	"oload", 		DIAL_OLOAD		},
	{	"mload",		DIAL_MLOAD		},
	{	"teleport", 		DIAL_TELEPORT		},
	{	"teleportall", 		DIAL_TELEPORTALL	},
	{	"teleportplus", 	DIAL_TELEPORTPLUS	},
	{	"death", 		DIAL_DEATH		},
	{	"cast", 		DIAL_CAST		},
	{	"fakeblade",		DIAL_FAKEBLADE		},
	{	"rand4", 		DIAL_RAND4		},
	{	"rand6", 		DIAL_RAND6		},
	{	"trapdoor", 		DIAL_TRAPDOOR		},
	{  "", 			0 			}
};

const	struct	flag_type	liquid_types_list[ ]	=
{
	{	"water", 		0			},
	{	"beer", 		1			},
	{	"wine", 		2			},
	{	"ale", 			3			},
	{	"dark-ale", 		4			},
	{	"whisky", 		5			},
	{	"lemonade", 		6			},
	{	"firebreather", 	7			},
	{	"local-specialty", 	8			},
	{	"slime-mold-juice", 	9			},
	{	"milk", 		10			},
	{	"tea", 			11			},
	{	"coffee", 		12			},
	{	"blood", 		13			},
	{	"salt-water", 		14			},
	{	"cola", 		15			},
	{	"ponch", 		16			},
	{	"grog", 		17			},
	{	"shake", 		18			},
	{	"drink", 		19			},
	{	"", 			0			}
};

const	struct	flag_type	forcetype_list []	=
{
	{	"none",			FORCE_NONE		},
	{	"neutral",		FORCE_NEUTRAL		},
	{	"Jedi",			FORCE_JEDI		},
	{	"Sith",			FORCE_SITH		},
	{	"",			0			}
};//byTrog

const	struct	flag_type	race_flags_list []	=
{
	{	"prototype",	FRACE_PROTOTYPE	},
	{	"pc_avail",		FRACE_PC_AVAIL	},
	{	"mute",			FRACE_MUTE		},//Ganis
	{	"",				0				}
};//byTrog

const	struct	flag_type	constate_types_list[ ]	=
{
	{	"Holonet", 		CON_HOLONET		},
	{	"RaEdit",		CON_RAEDITOR		},
	{	"TCEdit",		CON_TCEDITOR		},
	{	"ProEdit",		CON_PROEDITOR		},
	{	"CEdit",		CON_CEDITOR		},
	{	"SkEdit",		CON_SKEDITOR		},
	{	"PEdit",   		CON_PEDITOR		},
	{	"SSEdit",   	CON_SSEDITOR		},
	{	"HEdit",  		CON_HEDITOR		},
	{	"AEdit",   		CON_AEDITOR		},
	{	"RPEdit",  		CON_RPEDITOR		},
	{	"OPEdit",  		CON_OPEDITOR		},
	{	"MPEdit",  		CON_MPEDITOR		},
	{	"REdit",   		CON_REDITOR		},
	{	"OEdit",   		CON_OEDITOR		},
	{	"MEdit",   		CON_MEDITOR		},
	{	"QEdit",   		CON_QEDITOR		},
	{	"MatEdit",   	CON_MATEDITOR	},
	{	"DialEdit",   	CON_DIALOGEDIT	},
	{	"MobSpeaking",  CON_SPEAKING	},
	{	"ShEdit",   		CON_SHEDITOR		},
	{	"Playing",   		CON_PLAYING   		},
	{	"get_name",  		CON_GET_NAME  		},
	{	"get_old_pas",		CON_GET_OLD_PASSWORD	},
	{	"MotdRead",		CON_READ_MOTD  		},
	{	"iMotdRead",  		CON_READ_IMOTD 		},
	{	"cnfrm_name",    	CON_CONFIRM_NEW_NAME	},
	{	"get_ansi",    		CON_GET_NEW_COLOUR	},
	{	"get_chrset",  		CON_GET_CHAR_SET	},
	{	"chk_chrset",    	CON_CHECK_CHAR_SET	},
	{	"get_msp", 		CON_GET_MSP  		},
	{	"intro",  		CON_WPROWADZENIE	},
	{	"get_race",    		CON_GET_NEW_RACE	},
	{	"cnfrm_race",    	CON_CONFIRM_NEW_RACE	},
	{	"get_sex",  		CON_GET_NEW_SEX		},
	{	"get_class",   	 	CON_GET_NEW_CLASS	},
	{	"cnfrm_class",    	CON_CONFIRM_CLASS	},
	{	"get_inf1",  		CON_GET_DOPELNIACZ	},
	{	"get_inf2",    		CON_GET_CELOWNIK	},
	{	"get_inf3",    		CON_GET_BIERNIK		},
	{	"get_inf4",  		CON_GET_NARZEDNIK	},
	{	"get_inf5",		CON_GET_MIEJSCOWNIK	},
	{	"chk_decl",		CON_CHECK_ODMIANE	},
	{	"roll_stats",  		CON_ROLL_STATS		},
	{	"chk_stats",		CON_STATS_OK  		},
	{	"get_n_pas",   		CON_GET_NEW_PASSWORD	},
	{	"cnfrm_pas",  		CON_CONFIRM_NEW_PASSWORD},
	{	"cnfrm_scde",           CON_CONFIRM_SUICIDE	},
	{	"gt_o_pas2chg",		CON_OLD_PASS_TO_CHANGE	},

	{	"chpass:old",		CON_OLD_PASS_TO_CHANGE	},
	{	"chpass:new",		CON_NEW_PASS_TO_CHANGE	},
	{	"chpass:crm",		CON_CONFIRM_NEW_PASS_TO_CHANGE	},

	{	"renaming",		CON_RENAMING		},
	{	"cpovr_rcvr",		CON_COPYOVER_RECOVER	},

	{	"",			0			}
};

const	struct	flag_type	help_types_list[ ]	=
{
	{	"phelp", 		HELP_PHELP		},
	{	"olchelp", 		HELP_OLCHELP		},
	{	"wizhelp", 		HELP_WIZHELP		},
	{	"", 			0			}
};

const	struct	flag_type	ship_flags_list[ ]	=
{
	{	"aggressive",		SHIP_AGGRESSIVE		},
	{	"metaaggressive",	SHIP_METAAGGRESSIVE	},
	{	"hidden",			SHIP_HIDDEN			},
	{	"nowander",			SHIP_NOWANDER		},
	{	"wimpy",			SHIP_WIMPY			},
	{	"descless",			SHIP_DESCLESS		},
	{	"done",				SHIP_DONE			},
	{	"", 				0					}
};

const	struct	flag_type	ssystem_flags_list[ ]	=
{
	{	"TEMPORARY",		STARS_FLAG_TEMP		},
	{	"meteor_Shower",	STARS_EVENT_METEOR	},
	{	"ion_Storm",		STARS_EVENT_ION		},
	{	"EMP_field",		STARS_FLAG_EMP		},
	{	"", 			0			}
};


const	struct	flag_type	ship_types_list[ ]	=
{
	{	"republic", 		SHIP_REPUBLIC		},
	{	"imperial", 		SHIP_IMPERIAL		},
	{	"civilian", 		SHIP_CIVILIAN		},
	{	"mobship", 		MOB_SHIP		},
	{	"pirate",		SHIP_PIRATE		},
	{	"yuuzhan",		SHIP_YUUZHAN		},
	{	"", 			0			}
};

const	struct	flag_type	ship_classes_list[ ]	=
{
	{ 	"TIE_FIGHTER_TIE_LN", 		TIE_FIGHTER_TIE_LN_SHIP 	},
	{	"TIE_VANGUARD_TIE_RC",		TIE_VANGUARD_TIE_RC_SHIP	},
	{	"TIE_FIRE_CONTROL_TIE_FC", 	TIE_FIRE_CONTROL_TIE_FC_SHIP 	},
	{	"TIE_GROUND_ATTACK_TIE_GT",	TIE_GROUND_ATTACK_TIE_GT_SHIP	},
	{	"TIE_BOMBER_TIE_SA", 		TIE_BOMBER_TIE_SA_SHIP 		},
	{	"TIE_SHUTTLE_TIE_SH", 		TIE_SHUTTLE_TIE_SH_SHIP 	},
	{	"TIE_BOARDING_CRAFT_TIE_BC",	TIE_BOARDING_CRAFT_TIE_BC_SHIP 	},
	{	"TIE_INTERCEPTOR", 		TIE_INTERCEPTOR_SHIP 		},
	{	"ASSAULT_GUNBOAT", 		ASSAULT_GUNBOAT_SHIP 		},
	{	"TIE_X1_ADVANCED_TIE", 		TIE_X1_ADVANCED_TIE_SHIP	},
	{	"TIE_AVENGER_TIE_AD", 		TIE_AVENGER_TIE_AD_SHIP 	},
	{	"TIE_ADVANCED_AVENGER", 	TIE_ADVANCED_AVENGER_SHIP	},
	{	"XWING", 			XWING_SHIP 			},
	{	"XWING_RECON", 			XWING_RECON_SHIP 		},
	{	"YWING", 			YWING_SHIP 			},
	{	"YWING_LONGPROBE", 		YWING_LONGPROBE_SHIP 		},
	{	"AWING", 			AWING_SHIP 			},
	{	"BWING", 			BWING_SHIP 			},
	{	"TIE_RAPTOR", 			TIE_RAPTOR_SHIP 		},
	{	"TIE_DEFENDER", 		TIE_DEFENDER_SHIP 		},
	{	"SCIMITAR_ASSAULT_BOMBER", 	SCIMITAR_ASSAULT_BOMBER_SHIP 	},
	{	"A9_VIGILANCE", 		A9_VIGILANCE_SHIP 		},
	{	"I7_HOWLRUNNER", 		I7_HOWLRUNNER_SHIP 		},
	{	"PREYBIRD", 			PREYBIRD_SHIP 			},
	{	"TIE_DROID_TIE_D_TIE_DR", 	TIE_DROID_TIE_D_TIE_DR_SHIP 	},
	{	"MISSILE_BOAT", 		MISSILE_BOAT_SHIP 		},
	{	"TIE_PHANTOM", 			TIE_PHANTOM_SHIP 		},
	{	"TWING", 			TWING_SHIP 			},
	{	"EWING", 			EWING_SHIP 			},
	{	"BWING_E", 			BWING_E_SHIP 			},
	{	"XWING_II",			XWING_II_SHIP			},
	{	"KWING", 			KWING_SHIP			},
	{	"CLOAKSHAPE_FIGHTER", 		CLOAKSHAPE_FIGHTER_SHIP		},
	{	"TOCSAN_8Q_FIGHTER", 		TOCSAN_8Q_FIGHTER_SHIP		},
	{	"ZEBRA_STARFIGHTER", 		ZEBRA_STARFIGHTER_SHIP 		},
	{	"AUTHORITY_IRD", 		AUTHORITY_IRD_SHIP 		},
	{	"C73_TRACKER", 			C73_TRACKER_SHIP 		},
	{ 	"Z95_HEADHUNTER_MK_I", 		Z95_HEADHUNTER_MK_I_SHIP	},
	{	"Z95_HEADHUNTER", 		Z95_HEADHUNTER_SHIP 		},
	{ 	"Z95_HEADHUNTER_XT", 		Z95_HEADHUNTER_XT_SHIP 		},
	{ 	"Z95AF4_HEADHUNTER", 		Z95AF4_HEADHUNTER_SHIP 		},
	{ 	"R41_STARCHASER", 		R41_STARCHASER_SHIP 		},
	{	"TIEWING_TIEY_UGLY", 		TIEWING_TIEY_UGLY_SHIP 		},
	{ 	"YTIE_UGLY", 			YTIE_UGLY_SHIP 			},
	{ 	"XTIE_UGLY", 			XTIE_UGLY_SHIP 			},
	{ 	"XORCEPTOR_XT_I_UGLY", 		XORCEPTOR_XT_I_UGLY_SHIP	},
	{ 	"TRIFIGHTER", 			TRIFIGHTER_SHIP 		},
	{	"DEATHSEED_CHIR_DAKI", 		DEATHSEED_CHIR_DAKI_SHIP	},
	{ 	"LAMBDA_SHUTTLE", 		LAMBDA_SHUTTLE_SHIP 		},
	{ 	"TYDERIAN_SHUTTLE", 		TYDERIAN_SHUTTLE_SHIP 		},
	{ 	"MU2_SHUTTLE", 			MU2_SHUTTLE_SHIP 		},
	{	"ESCORT_SHUTTLE", 		ESCORT_SHUTTLE_SHIP 		},
	{ 	"STORMTROOPER_TRANSPORT", 	STORMTROOPER_TRANSPORT_SHIP 	},
	{ 	"GREK_TROOP_SHUTTLE", 		GREK_TROOP_SHUTTLE_SHIP 	},
	{ 	"GAMMA_ASSAULT_SHUTTLE", 	GAMMA_ASSAULT_SHUTTLE_SHIP 	},
	{	"KATARN_BOARDING_SHUTTLE", 	KATARN_BOARDING_SHUTTLE_SHIP 	},
	{ 	"ULIG_ABAHA_2_WARPOD", 		ULIG_ABAHA_2_WARPOD_SHIP 	},
	{ 	"ASSAULT_TRANSPORT", 		ASSAULT_TRANSPORT_SHIP 		},
	{ 	"MT_191_DROPSHIP",		MT_191_DROPSHIP_SHIP		},
	{	"KAPPA_TROOP_SHUTTLE", 		KAPPA_TROOP_SHUTTLE_SHIP	},
	{ 	"SENTINEL_LANDING_CRAFT", 	SENTINEL_LANDING_CRAFT_SHIP 	},
	{ 	"AEGIS_COMBAT_SHUTTLE", 	AEGIS_COMBAT_SHUTTLE_SHIP 	},
	{	"WARLORD_LANDING_BARGE", 	WARLORD_LANDING_BARGE_SHIP	},
	{ 	"Y4_RAPTOR", 			Y4_RAPTOR_SHIP 			},
	{ 	"VWING_TRANSPORT", 		VWING_TRANSPORT_SHIP 		},
	{ 	"LONE_SCOUT_A", 		LONE_SCOUT_A_SHIP 		},
	{	"IMPERIAL_SCOUT_CRAFT", 	IMPERIAL_SCOUT_CRAFT_SHIP	},
	{ 	"GYRE_SCOUT", 			GYRE_SCOUT_SHIP 		},
	{ 	"FIRESPRAY_PATROL_CRAFT", 	FIRESPRAY_PATROL_CRAFT_SHIP 	},
	{ 	"MANDALMOTORS_PURSUER", 	MANDALMOTORS_PURSUER_SHIP 	},
	{	"CONQUEROR_ASSAULT_SHIP",	CONQUEROR_ASSAULT_SHIP_SHIP	},
	{ 	"SKIPRAY_BLASTBOAT",		SKIPRAY_BLASTBOAT_SHIP		},
	{ 	"GUARDIAN_LIGHT_CRUISER", 	GUARDIAN_LIGHT_CRUISER_SHIP 	},
	{	"CUSTOMS_FRIGATE",		CUSTOMS_FRIGATE_SHIP		},
	{ 	"IPV1_SYSTEM_PATROL_CRAFT",	IPV1_SYSTEM_PATROL_CRAFT_SHIP	},
	{ 	"PASSENGER_LINER", 		PASSENGER_LINER_SHIP 		},
	{ 	"FERRYBOAT_LINER", 		FERRYBOAT_LINER_SHIP 		},
	{	"KLEEQUE_TRANSPORT",		KLEEQUE_TRANSPORT_SHIP		},
	{ 	"BR23_COURIER",			BR23_COURIER_SHIP		},
	{ 	"LUXURY_3000_YACHT",		LUXURY_3000_YACHT_SHIP		},
	{ 	"BAUDO_YACHT", 			BAUDO_YACHT_SHIP		},
	{	"YT1210_CORELLIAN_TRANSPORT",	YT1210_CORELLIAN_TRANSPORT_SHIP	},
	{ 	"YT1300_CORELLIAN_TRANSPORT",	YT1300_CORELLIAN_TRANSPORT_SHIP	},
	{ 	"YT2000_CORELLIAN_TRANSPORT",	YT2000_CORELLIAN_TRANSPORT_SHIP	},
	{	"YT2400_CORELLIAN_TRANSPORT",	YT2400_CORELLIAN_TRANSPORT_SHIP	},
	{ 	"GHTROC_FREIGHTER",		GHTROC_FREIGHTER_SHIP		},
	{ 	"AZZ3_FREIGHTER",		AZZ3_FREIGHTER_SHIP		},
	{ 	"Z10_SEEKER", 			Z10_SEEKER_SHIP 		},
	{	"HERMES_COURIER",		HERMES_COURIER_SHIP		},
	{ 	"MUURIAN_TRANSPORT",		MUURIAN_TRANSPORT_SHIP		},
	{ 	"BARLOZ_FREIGHTER",		BARLOZ_FREIGHTER_SHIP		},
	{ 	"HT2200_MEDIUM_FREIGHTER", 	HT2200_MEDIUM_FREIGHTER_SHIP 	},
	{	"XIYTIAR_TRANSPORT",		XIYTIAR_TRANSPORT_SHIP		},
	{ 	"BULK_BARGE",			BULK_BARGE_SHIP			},
	{ 	"MEDIUM_TRANSPORT",		MEDIUM_TRANSPORT_SHIP		},
	{ 	"CARGO_FERRY", 			CARGO_FERRY_SHIP 		},
	{	"ACTION_IV_TRANSPORT",		ACTION_IV_TRANSPORT_SHIP	},
	{ 	"BULK_FREIGHTER",		BULK_FREIGHTER_SHIP		},
	{ 	"STAR_GALLEON",			STAR_GALLEON_SHIP		},
	{ 	"SPACE_TANKER", 		SPACE_TANKER_SHIP 		},
	{	"CARGO_TRAIN",			CARGO_TRAIN_SHIP		},
	{ 	"CONTAINER_SHIP",		CONTAINER_SHIP_SHIP		},
	{ 	"MARAUDER_CORVETTE",		MARAUDER_CORVETTE_SHIP		},
	{ 	"CORELLIAN_CORVETTE",		CORELLIAN_CORVETTE_SHIP		},
	{	"MODIFIED_CORVETTE",		MODIFIED_CORVETTE_SHIP		},
	{ 	"ASSASIN_CORVETTE",		ASSASIN_CORVETTE_SHIP		},
	{ 	"CUSTOMS_CORVETTE",		CUSTOMS_CORVETTE_SHIP		},
	{ 	"CORELLIAN_GUNSHIP", 		CORELLIAN_GUNSHIP_SHIP 		},
	{	"VIBRE_CRUISER",		VIBRE_CRUISER_SHIP		},
	{ 	"NEBULON_ESCORT_FRIGATE",	NEBULON_ESCORT_FRIGATE_SHIP	},
	{ 	"NEBULON_ESCORT_FRIGATE_LIGHT", NEBULON_ESCORT_FRIGATE_LIGHT_SHIP},
	{	"MODIFIED_ESCORT_FRIGATE",	MODIFIED_ESCORT_FRIGATE_SHIP	},
	{ 	"LANCER_FRIGATE",		LANCER_FRIGATE_SHIP		},
	{ 	"CALAMARI_FRIGATE",		CALAMARI_FRIGATE_SHIP		},
	{ 	"QUASAR_FIRE_CRUISER", 		QUASAR_FIRE_CRUISER_SHIP 	},
	{	"IMPERIAL_ESCORT_CARRIER",	IMPERIAL_ESCORT_CARRIER_SHIP	},
	{ 	"IMPERIAL_FLEET_CARRIER",	IMPERIAL_FLEET_CARRIER_SHIP	},
	{ 	"INTERDICTOR_CRUISER", 		INTERDICTOR_CRUISER_SHIP	},
	{	"MEDICAL_FRIGATE",		MEDICAL_FRIGATE_SHIP		},
	{ 	"MC_EVACUATION_CRUISER",	MC_EVACUATION_CRUISER_SHIP	},
	{ 	"DUNGEON_SHIP",			DUNGEON_SHIP_SHIP		},
	{ 	"CARRACK_CRUISER", 		CARRACK_CRUISER_SHIP 		},
	{	"STRIKE_CRUISER",		STRIKE_CRUISER_SHIP		},
	{ 	"BULK_CRUISER",			BULK_CRUISER_SHIP		},
	{ 	"LIGHT_CALAMARI_CRUISER",	LIGHT_CALAMARI_CRUISER_SHIP	},
	{ 	"TASKFORCE_CRUISER", 		TASKFORCE_CRUISER_SHIP 		},
	{	"INVINCIBLE_DREADNAUGHT",	INVINCIBLE_DREADNAUGHT_SHIP	},
	{ 	"RENDILLI_DREADNAUGHT",		RENDILLI_DREADNAUGHT_SHIP	},
	{ 	"ASSAULT_FRIGATE", 		ASSAULT_FRIGATE_SHIP 		},
	{	"MC80_CALAMARI_CRUISER",	MC80_CALAMARI_CRUISER_SHIP	},
	{ 	"MC80A_CALAMARI_CRUISER",	MC80A_CALAMARI_CRUISER_SHIP	},
	{ 	"MC80B_CALAMARI_CRUISER", 	MC80B_CALAMARI_CRUISER_SHIP 	},
	{	"MC90_CALAMARI_CRUISER",	MC90_CALAMARI_CRUISER_SHIP	},
	{ 	"VICTORY_STAR_DESTROYER",	VICTORY_STAR_DESTROYER_SHIP	},
	{ 	"VICTORY_II_STAR_DESTROYER", 	VICTORY_II_STAR_DESTROYER_SHIP 	},
	{	"IMPERIAL_STAR_DESTROYER",	IMPERIAL_STAR_DESTROYER_SHIP	},
	{ 	"IMPERIAL_II_STAR_DESTROYER",	IMPERIAL_II_STAR_DESTROYER_SHIP	},
	{ 	"SUPER_STAR_DESTROYER", 	SUPER_STAR_DESTROYER_SHIP 	},
	{	"SOVEREIGN_SUPER_STAR_DESTROYER",SOVEREIGN_SUPER_STAR_DESTROYER_SHIP},
	{ 	"ECLIPSE_SUPER_STAR_DESTROYER",	ECLIPSE_SUPER_STAR_DESTROYER_SHIP},
	{ 	"SPACE_FACTORY_STATION", 	SPACE_FACTORY_STATION_SHIP 	},
	{	"EMPRESS_SPACE_PLATFORM",	EMPRESS_SPACE_PLATFORM_SHIP	},
	{ 	"GOLAN_I_SPACE_DEFENSE_STATION",GOLAN_I_SPACE_DEFENSE_STATION_SHIP},
	{ 	"GOLAN_II_SPACE_DEFENSE_STATION",GOLAN_II_SPACE_DEFENSE_STATION_SHIP},
	{	"GOLAN_III_SPACE_DEFENSE_STATION",GOLAN_III_SPACE_DEFENSE_STATION_SHIP},
	{ 	"TORPEDO_SPHERE",		TORPEDO_SPHERE_SHIP		},
	{ 	"DEATH_STAR",			DEATH_STAR_SHIP			},
	{ 	"SPACE_TUG", 			SPACE_TUG_SHIP 			},
	{	"UTILITY_TUG",			UTILITY_TUG_SHIP		},
	{ 	"HEAVY_LIFTER",			HEAVY_LIFTER_SHIP		},
	{ 	"SPACETROOPER",			SPACETROOPER_SHIP		},
	{ 	"PDV_MESSENGER_POD", 		PDV_MESSENGER_POD_SHIP 		},
	{	"IMPERIAL_PROBOT_POD",		IMPERIAL_PROBOT_POD_SHIP	},
	{ 	"CLOUD_CAR",			CLOUD_CAR			},
	{ 	"OCEAN",			OCEAN_SHIP			},
	{ 	"LAND_SPEEDER",			LAND_SPEEDER			},
	{ 	"WHEELED",			WHEELED				},
	{ 	"LAND_CRAWLER",			LAND_CRAWLER			},
	{ 	"WALKER", 			WALKER				},
	{	"NO_SPACE_SHIP",		NO_SPACE_SHIP			},
	{ 	"SCOUTSHIP",			SCOUT_SHIP			},
	{ 	"FIGHTER",			FIGHTER_SHIP			},
	{ 	"MIDSHIP",			MIDSHIP_SHIP			},
	{ 	"FREIGHTER",			FREIGHTER_SHIP			},
	{ 	"CAPITAL",			CAPITAL_SHIP			},
	{	"HUGE_SHIP",			HUGE_SHIP			},
	{ 	"PLATFORM",			PLATFORM_SHIP			},
	{	"CORAL_FIGHTER",			CORAL_FIGHTER_SHIP		},
	{	"CORAL_BOMBER",			CORAL_BOMBER_SHIP		},
	{	"SWARM",			SWARM_SHIP		},
	{	"SHRIWIRR",			SHRIWIRR_SHIP		},
	{	"JABITHA",		JABITHA_SHIP	},
	{	"KOROS-STRONHA",		KOROS_STRONHA_SPIRAL_ARMED_WORLDSHIP_SHIP	},
	{	"YORIK-VEC",		YORIK_VEC_ASSAULT_CRUISER_SHIP	},
	{	"RO'IK_CHUUN_M'ARH",		ROIK_CHUUN_MARH_FRIGATE_SHIP	},
	{	"KOR_CHOKK",		KOR_CHOKK_GRAND_CRUISER_SHIP	},
	{	"YORIK-TREMA",		YORIK_TREMA_TRANSPORT_CARRIER_SHIP	},
	{	"MIID_RO'IK",		MIID_ROIK_WARSHIP_SHIP	},
	{	"FW'SEN",		PICKET_SHIP_FWSEN_CLASS_SHIP	},
	{	"SH'NER",		SHNER_PLANETARY_ASSAULT_CARRIER_SHIP	},
	{	"D'KEE",		DKEE_CLASS_PWECK_LANDING_SHIP	},
	{	"LWHEKK",		LWHEKK_CLASS_MANUFACTURING_SHIP	},
	{	"WURRIF",		WURRIF_CLASS_CRUISER_SHIP	},
	{	"SH'NER-CLASS",		SHNER_CLASS_PLANETARY_ASSAULT_CARRIER_SHIP	},
	{	"SHREE",		SHREE_CLASS_BATTLE_CRUISER	},
	{	"ESCAPE_POD",		ESCAPE_POD_SHIP	},





	{	"", 				0				}
	/*, "\xa6wiat\xb3o Statek"*/
};
const	struct	flag_type	ship_classes_name_list[ ]	=
{
	{ 	"My\xb6liwiec TIE", 		TIE_FIGHTER_TIE_LN_SHIP 	},
	{	"My\xb6liwiec rozpoznawczy TIE",		TIE_VANGUARD_TIE_RC_SHIP	},
	{	"TIE_FIRE_CONTROL_TIE_FC", 	TIE_FIRE_CONTROL_TIE_FC_SHIP 	},
	{	"TIE_GROUND_ATTACK_TIE_GT",	TIE_GROUND_ATTACK_TIE_GT_SHIP	},
	{	"TIE_BOMBER_TIE_SA", 		TIE_BOMBER_TIE_SA_SHIP 		},
	{	"TIE_SHUTTLE_TIE_SH", 		TIE_SHUTTLE_TIE_SH_SHIP 	},
	{	"TIE_BOARDING_CRAFT_TIE_BC",	TIE_BOARDING_CRAFT_TIE_BC_SHIP 	},
	{	"My\xb6liwiec TIE Interceptor", 		TIE_INTERCEPTOR_SHIP 		},
	{	"ASSAULT_GUNBOAT", 		ASSAULT_GUNBOAT_SHIP 		},
	{	"TIE_X1_ADVANCED_TIE", 		TIE_X1_ADVANCED_TIE_SHIP	},
	{	"TIE_AVENGER_TIE_AD", 		TIE_AVENGER_TIE_AD_SHIP 	},
	{	"TIE_ADVANCED_AVENGER", 	TIE_ADVANCED_AVENGER_SHIP	},
	{	"My\xb6liwiec X-Wing", 			XWING_SHIP 			},
	{	"XWING_RECON", 			XWING_RECON_SHIP 		},
	{	"YWING", 			YWING_SHIP 			},
	{	"YWING_LONGPROBE", 		YWING_LONGPROBE_SHIP 		},
	{	"AWING", 			AWING_SHIP 			},
	{	"BWING", 			BWING_SHIP 			},
	{	"TIE_RAPTOR", 			TIE_RAPTOR_SHIP 		},
	{	"My\xb6liwiec TIE Defender", 		TIE_DEFENDER_SHIP 		},
	{	"SCIMITAR_ASSAULT_BOMBER", 	SCIMITAR_ASSAULT_BOMBER_SHIP 	},
	{	"A9_VIGILANCE", 		A9_VIGILANCE_SHIP 		},
	{	"I7_HOWLRUNNER", 		I7_HOWLRUNNER_SHIP 		},
	{	"PREYBIRD", 			PREYBIRD_SHIP 			},
	{	"TIE_DROID_TIE_D_TIE_DR", 	TIE_DROID_TIE_D_TIE_DR_SHIP 	},
	{	"MISSILE_BOAT", 		MISSILE_BOAT_SHIP 		},
	{	"TIE_PHANTOM", 			TIE_PHANTOM_SHIP 		},
	{	"TWING", 			TWING_SHIP 			},
	{	"EWING", 			EWING_SHIP 			},
	{	"BWING_E", 			BWING_E_SHIP 			},
	{	"XWING_II",			XWING_II_SHIP			},
	{	"KWING", 			KWING_SHIP			},
	{	"CLOAKSHAPE_FIGHTER", 		CLOAKSHAPE_FIGHTER_SHIP		},
	{	"TOCSAN_8Q_FIGHTER", 		TOCSAN_8Q_FIGHTER_SHIP		},
	{	"ZEBRA_STARFIGHTER", 		ZEBRA_STARFIGHTER_SHIP 		},
	{	"AUTHORITY_IRD", 		AUTHORITY_IRD_SHIP 		},
	{	"C73_TRACKER", 			C73_TRACKER_SHIP 		},
	{ 	"Z95_HEADHUNTER_MK_I", 		Z95_HEADHUNTER_MK_I_SHIP	},
	{	"Z95_HEADHUNTER", 		Z95_HEADHUNTER_SHIP 		},
	{ 	"Z95_HEADHUNTER_XT", 		Z95_HEADHUNTER_XT_SHIP 		},
	{ 	"Z95AF4_HEADHUNTER", 		Z95AF4_HEADHUNTER_SHIP 		},
	{ 	"R41_STARCHASER", 		R41_STARCHASER_SHIP 		},
	{	"TIEWING_TIEY_UGLY", 		TIEWING_TIEY_UGLY_SHIP 		},
	{ 	"YTIE Ugly", 			YTIE_UGLY_SHIP 			},
	{ 	"XTIE_UGLY", 			XTIE_UGLY_SHIP 			},
	{ 	"XORCEPTOR_XT_I_UGLY", 		XORCEPTOR_XT_I_UGLY_SHIP	},
	{ 	"TRIFIGHTER", 			TRIFIGHTER_SHIP 		},
	{	"DEATHSEED_CHIR_DAKI", 		DEATHSEED_CHIR_DAKI_SHIP	},
	{ 	"LAMBDA_SHUTTLE", 		LAMBDA_SHUTTLE_SHIP 		},
	{ 	"TYDERIAN_SHUTTLE", 		TYDERIAN_SHUTTLE_SHIP 		},
	{ 	"MU2_SHUTTLE", 			MU2_SHUTTLE_SHIP 		},
	{	"ESCORT_SHUTTLE", 		ESCORT_SHUTTLE_SHIP 		},
	{ 	"STORMTROOPER_TRANSPORT", 	STORMTROOPER_TRANSPORT_SHIP 	},
	{ 	"GREK_TROOP_SHUTTLE", 		GREK_TROOP_SHUTTLE_SHIP 	},
	{ 	"GAMMA_ASSAULT_SHUTTLE", 	GAMMA_ASSAULT_SHUTTLE_SHIP 	},
	{	"KATARN_BOARDING_SHUTTLE", 	KATARN_BOARDING_SHUTTLE_SHIP 	},
	{ 	"ULIG_ABAHA_2_WARPOD", 		ULIG_ABAHA_2_WARPOD_SHIP 	},
	{ 	"ASSAULT_TRANSPORT", 		ASSAULT_TRANSPORT_SHIP 		},
	{ 	"MT_191_DROPSHIP",		MT_191_DROPSHIP_SHIP		},
	{	"KAPPA_TROOP_SHUTTLE", 		KAPPA_TROOP_SHUTTLE_SHIP	},
	{ 	"SENTINEL_LANDING_CRAFT", 	SENTINEL_LANDING_CRAFT_SHIP 	},
	{ 	"AEGIS_COMBAT_SHUTTLE", 	AEGIS_COMBAT_SHUTTLE_SHIP 	},
	{	"WARLORD_LANDING_BARGE", 	WARLORD_LANDING_BARGE_SHIP	},
	{ 	"Y4_RAPTOR", 			Y4_RAPTOR_SHIP 			},
	{ 	"VWING_TRANSPORT", 		VWING_TRANSPORT_SHIP 		},
	{ 	"LONE_SCOUT_A", 		LONE_SCOUT_A_SHIP 		},
	{	"IMPERIAL_SCOUT_CRAFT", 	IMPERIAL_SCOUT_CRAFT_SHIP	},
	{ 	"GYRE_SCOUT", 			GYRE_SCOUT_SHIP 		},
	{ 	"FIRESPRAY_PATROL_CRAFT", 	FIRESPRAY_PATROL_CRAFT_SHIP 	},
	{ 	"MANDALMOTORS_PURSUER", 	MANDALMOTORS_PURSUER_SHIP 	},
	{	"CONQUEROR_ASSAULT_SHIP",	CONQUEROR_ASSAULT_SHIP_SHIP	},
	{ 	"SKIPRAY_BLASTBOAT",		SKIPRAY_BLASTBOAT_SHIP		},
	{ 	"GUARDIAN_LIGHT_CRUISER", 	GUARDIAN_LIGHT_CRUISER_SHIP 	},
	{	"CUSTOMS_FRIGATE",		CUSTOMS_FRIGATE_SHIP		},
	{ 	"IPV1_SYSTEM_PATROL_CRAFT",	IPV1_SYSTEM_PATROL_CRAFT_SHIP	},
	{ 	"PASSENGER_LINER", 		PASSENGER_LINER_SHIP 		},
	{ 	"FERRYBOAT_LINER", 		FERRYBOAT_LINER_SHIP 		},
	{	"KLEEQUE_TRANSPORT",		KLEEQUE_TRANSPORT_SHIP		},
	{ 	"BR23_COURIER",			BR23_COURIER_SHIP		},
	{ 	"LUXURY_3000_YACHT",		LUXURY_3000_YACHT_SHIP		},
	{ 	"BAUDO_YACHT", 			BAUDO_YACHT_SHIP		},
	{	"YT1210",	YT1210_CORELLIAN_TRANSPORT_SHIP	},
	{ 	"YT1300",	YT1300_CORELLIAN_TRANSPORT_SHIP	},
	{ 	"YT2000",	YT2000_CORELLIAN_TRANSPORT_SHIP	},
	{	"YT2400",	YT2400_CORELLIAN_TRANSPORT_SHIP	},
	{ 	"GHTROC_FREIGHTER",		GHTROC_FREIGHTER_SHIP		},
	{ 	"AZZ3_FREIGHTER",		AZZ3_FREIGHTER_SHIP		},
	{ 	"Z10_SEEKER", 			Z10_SEEKER_SHIP 		},
	{	"HERMES_COURIER",		HERMES_COURIER_SHIP		},
	{ 	"MUURIAN_TRANSPORT",		MUURIAN_TRANSPORT_SHIP		},
	{ 	"BARLOZ_FREIGHTER",		BARLOZ_FREIGHTER_SHIP		},
	{ 	"HT2200_MEDIUM_FREIGHTER", 	HT2200_MEDIUM_FREIGHTER_SHIP 	},
	{	"XIYTIAR_TRANSPORT",		XIYTIAR_TRANSPORT_SHIP		},
	{ 	"BULK_BARGE",			BULK_BARGE_SHIP			},
	{ 	"MEDIUM_TRANSPORT",		MEDIUM_TRANSPORT_SHIP		},
	{ 	"CARGO_FERRY", 			CARGO_FERRY_SHIP 		},
	{	"ACTION_IV_TRANSPORT",		ACTION_IV_TRANSPORT_SHIP	},
	{ 	"BULK_FREIGHTER",		BULK_FREIGHTER_SHIP		},
	{ 	"STAR_GALLEON",			STAR_GALLEON_SHIP		},
	{ 	"SPACE_TANKER", 		SPACE_TANKER_SHIP 		},
	{	"CARGO_TRAIN",			CARGO_TRAIN_SHIP		},
	{ 	"CONTAINER_SHIP",		CONTAINER_SHIP_SHIP		},
	{ 	"MARAUDER_CORVETTE",		MARAUDER_CORVETTE_SHIP		},
	{ 	"CORELLIAN_CORVETTE",		CORELLIAN_CORVETTE_SHIP		},
	{	"MODIFIED_CORVETTE",		MODIFIED_CORVETTE_SHIP		},
	{ 	"ASSASIN_CORVETTE",		ASSASIN_CORVETTE_SHIP		},
	{ 	"CUSTOMS_CORVETTE",		CUSTOMS_CORVETTE_SHIP		},
	{ 	"CORELLIAN_GUNSHIP", 		CORELLIAN_GUNSHIP_SHIP 		},
	{	"VIBRE_CRUISER",		VIBRE_CRUISER_SHIP		},
	{ 	"NEBULON_ESCORT_FRIGATE",	NEBULON_ESCORT_FRIGATE_SHIP	},
	{ 	"NEBULON_ESCORT_FRIGATE_LIGHT", NEBULON_ESCORT_FRIGATE_LIGHT_SHIP},
	{	"MODIFIED_ESCORT_FRIGATE",	MODIFIED_ESCORT_FRIGATE_SHIP	},
	{ 	"LANCER_FRIGATE",		LANCER_FRIGATE_SHIP		},
	{ 	"CALAMARI_FRIGATE",		CALAMARI_FRIGATE_SHIP		},
	{ 	"QUASAR_FIRE_CRUISER", 		QUASAR_FIRE_CRUISER_SHIP 	},
	{	"IMPERIAL_ESCORT_CARRIER",	IMPERIAL_ESCORT_CARRIER_SHIP	},
	{ 	"IMPERIAL_FLEET_CARRIER",	IMPERIAL_FLEET_CARRIER_SHIP	},
	{ 	"INTERDICTOR_CRUISER", 		INTERDICTOR_CRUISER_SHIP	},
	{	"MEDICAL_FRIGATE",		MEDICAL_FRIGATE_SHIP		},
	{ 	"MC_EVACUATION_CRUISER",	MC_EVACUATION_CRUISER_SHIP	},
	{ 	"DUNGEON_SHIP",			DUNGEON_SHIP_SHIP		},
	{ 	"CARRACK_CRUISER", 		CARRACK_CRUISER_SHIP 		},
	{	"STRIKE_CRUISER",		STRIKE_CRUISER_SHIP		},
	{ 	"BULK_CRUISER",			BULK_CRUISER_SHIP		},
	{ 	"LIGHT_CALAMARI_CRUISER",	LIGHT_CALAMARI_CRUISER_SHIP	},
	{ 	"TASKFORCE_CRUISER", 		TASKFORCE_CRUISER_SHIP 		},
	{	"INVINCIBLE_DREADNAUGHT",	INVINCIBLE_DREADNAUGHT_SHIP	},
	{ 	"RENDILLI_DREADNAUGHT",		RENDILLI_DREADNAUGHT_SHIP	},
	{ 	"ASSAULT_FRIGATE", 		ASSAULT_FRIGATE_SHIP 		},
	{	"MC80_CALAMARI_CRUISER",	MC80_CALAMARI_CRUISER_SHIP	},
	{ 	"MC80A_CALAMARI_CRUISER",	MC80A_CALAMARI_CRUISER_SHIP	},
	{ 	"MC80B_CALAMARI_CRUISER", 	MC80B_CALAMARI_CRUISER_SHIP 	},
	{	"MC90_CALAMARI_CRUISER",	MC90_CALAMARI_CRUISER_SHIP	},
	{ 	"VICTORY_STAR_DESTROYER",	VICTORY_STAR_DESTROYER_SHIP	},
	{ 	"VICTORY_II_STAR_DESTROYER", 	VICTORY_II_STAR_DESTROYER_SHIP 	},
	{	"IMPERIAL_STAR_DESTROYER",	IMPERIAL_STAR_DESTROYER_SHIP	},
	{ 	"IMPERIAL_II_STAR_DESTROYER",	IMPERIAL_II_STAR_DESTROYER_SHIP	},
	{ 	"SUPER_STAR_DESTROYER", 	SUPER_STAR_DESTROYER_SHIP 	},
	{	"SOVEREIGN_SUPER_STAR_DESTROYER",SOVEREIGN_SUPER_STAR_DESTROYER_SHIP},
	{ 	"ECLIPSE_SUPER_STAR_DESTROYER",	ECLIPSE_SUPER_STAR_DESTROYER_SHIP},
	{ 	"SPACE_FACTORY_STATION", 	SPACE_FACTORY_STATION_SHIP 	},
	{	"EMPRESS_SPACE_PLATFORM",	EMPRESS_SPACE_PLATFORM_SHIP	},
	{ 	"GOLAN_I_SPACE_DEFENSE_STATION",GOLAN_I_SPACE_DEFENSE_STATION_SHIP},
	{ 	"GOLAN_II_SPACE_DEFENSE_STATION",GOLAN_II_SPACE_DEFENSE_STATION_SHIP},
	{	"GOLAN_III_SPACE_DEFENSE_STATION",GOLAN_III_SPACE_DEFENSE_STATION_SHIP},
	{ 	"TORPEDO_SPHERE",		TORPEDO_SPHERE_SHIP		},
	{ 	"DEATH_STAR",			DEATH_STAR_SHIP			},
	{ 	"SPACE_TUG", 			SPACE_TUG_SHIP 			},
	{	"UTILITY_TUG",			UTILITY_TUG_SHIP		},
	{ 	"HEAVY_LIFTER",			HEAVY_LIFTER_SHIP		},
	{ 	"SPACETROOPER",			SPACETROOPER_SHIP		},
	{ 	"PDV_MESSENGER_POD", 		PDV_MESSENGER_POD_SHIP 		},
	{	"IMPERIAL_PROBOT_POD",		IMPERIAL_PROBOT_POD_SHIP	},
	{ 	"CLOUD_CAR",			CLOUD_CAR			},
	{ 	"OCEAN",			OCEAN_SHIP			},
	{ 	"LAND_SPEEDER",			LAND_SPEEDER			},
	{ 	"WHEELED",			WHEELED				},
	{ 	"LAND_CRAWLER",			LAND_CRAWLER			},
	{ 	"WALKER", 			WALKER				},
	{	"NO_SPACE_SHIP",		NO_SPACE_SHIP			},
	{ 	"Nieokre\xb6lony SCOUTSHIP",			SCOUT_SHIP			},
	{ 	"Nieokre\xb6lony FIGHTER",			FIGHTER_SHIP			},
	{ 	"Nieokre\xb6lony MIDSHIP",			MIDSHIP_SHIP			},
	{ 	"Nieokre\xb6lony FREIGHTER",			FREIGHTER_SHIP			},
	{ 	"Nieokre\xb6lony CAPITAL",			CAPITAL_SHIP			},
	{	"HUGE_SHIP",			HUGE_SHIP			},
	{ 	"PLATFORMA",			PLATFORM_SHIP			},
	{	"Koralowy Skoczek",		CORAL_FIGHTER_SHIP		},
	{	"Yuuzhanski tw\xf3r",		CORAL_BOMBER_SHIP		},
	{	"Ssiruuvi Swarm",			SWARM_SHIP		},
	{	"Ssiruuvi Shriwirr",			SHRIWIRR_SHIP		},
	{	"Zonama Sekot Seedship",		JABITHA_SHIP	},
	{	"Koros-Stronha",		KOROS_STRONHA_SPIRAL_ARMED_WORLDSHIP_SHIP	},
	{	"Yorik-Vec",		YORIK_VEC_ASSAULT_CRUISER_SHIP	},
	{	"Ro'ik Chuun_M'arh",		ROIK_CHUUN_MARH_FRIGATE_SHIP	},
	{	"Kor Chokk",		KOR_CHOKK_GRAND_CRUISER_SHIP	},
	{	"Yorik-Trema",		YORIK_TREMA_TRANSPORT_CARRIER_SHIP	},
	{	"Miid Ro'ik",		MIID_ROIK_WARSHIP_SHIP	},
	{	"Fw'Sen",		PICKET_SHIP_FWSEN_CLASS_SHIP	},
	{	"Sh'ner",		SHNER_PLANETARY_ASSAULT_CARRIER_SHIP	},
	{	"D'kee",		DKEE_CLASS_PWECK_LANDING_SHIP	},
	{	"Lwhekk",		LWHEKK_CLASS_MANUFACTURING_SHIP	},
	{	"Wurrif",		WURRIF_CLASS_CRUISER_SHIP	},
	{	"Sh'ner mk2",		SHNER_CLASS_PLANETARY_ASSAULT_CARRIER_SHIP	},
	{	"Shree",		SHREE_CLASS_BATTLE_CRUISER	},
	{	"Kapsu\xb3" "a ratunkowa",		ESCAPE_POD_SHIP	},
	{	"", 				0				}
	/*, "\xa6wiat\xb3o Statek"*/
};


const struct flag_type	material_rarity_list [] =
{
		{	"common",		COMMON_MAT			},
		{	"occasional",	OCCASIONAL_MAT		},
		{	"rare",			RARE_MAT			},
		{	"unique",		UNIQUE_MAT			},
		{	"",				0					}
};

const struct flag_type          material_form_list [] =
{
		{   "unrefined ore",	U_ORE_FORM		},
		{   "refined ore",		R_ORE_FORM		},
		{   "alloy",			ALLOY_FORM		},
		{   "",					0				}
};

const struct flag_type			planet_buildings_list [] =
{
		{	"magazyn",			WAREHOUSE		},
		{	"",					0				}
};

/*! Trog: Force Events */
const	struct	flag_type	fevent_types_list[ ]	=
{
		{	"fe_find_room",		FE_FIND_ROOM		},
		{	"fe_find_object",	FE_FIND_OBJECT		},
		{	"fe_find_mob",		FE_FIND_MOB			},
		{	"fe_find_player",	FE_FIND_PLAYER		},
		{	"fe_find_ship",		FE_FIND_SHIP		},
		{	"fe_kill_mob",		FE_KILL_MOB			},
		{	"fe_kill_player",	FE_KILL_PLAYER		},
		{	"fe_kill_ship",		FE_KILL_SHIP		},
		{	"fe_gain_stat",		FE_GAIN_STAT		},
		{	"fe_start_quest",	FE_START_QUEST		},
		{	"fe_fail_quest",	FE_FAIL_QUEST		},
		{	"fe_finish_quest",	FE_FINISH_QUEST		},
		{	"fe_enter_ship",	FE_ENTER_SHIP		},
		{	"fe_launch_ship",	FE_LAUNCH_SHIP		},
		{	"fe_land_ship",		FE_LAND_SHIP		},
		{	"fe_level",			FE_LEVEL			},
		{	"fe_sklvl",			FE_SKLVL			},
		{	"fe_position",		FE_POSITION			},
		{	"fe_mkills",		FE_MKILLS			},
		{	"fe_pkills",		FE_PKILLS			},
		{	"fe_skill",			FE_SKILL			}
};

/* QUEST OLC */
const struct command_type 	init_command_table []	=
{
	{ INIT_LOAD_MOB,
	"mobvnum",	"roomstart","roomend",	"comment"	},
	{ INIT_LOAD_OBJ,
	"objvnum",	"timer",	"room",		"comment"	},
	{ INIT_PUT_OBJ,
	"objvnum",	"timer",	"toobjvnum","comment"	},
	{ INIT_EQUIP_MOB,
	"objvnum",	"timer",	"wearloc",	"comment"	},
	{ INIT_PURGE,
	NULL,		NULL,		NULL,		"place"		},
	{ INIT_TRANSFER,
	NULL,		NULL,		NULL,		"place"		},
	{ INIT_ECHO,
	NULL,		NULL,		NULL,		"text"		},
	{ INIT_HINT,
	NULL,		NULL,		NULL,		"text"		},
	{ INIT_INVITE_MOB,
	"mobvnum",	NULL,		NULL,		"comment"	},
	{ INIT_GIVE_OBJ,
	"objvnum",	"timer",	NULL,		"comment"	},
	{ -999,
	NULL,		NULL,		NULL,		NULL		}
};

const struct command_type 	event_command_table []	=
{
	{ EVENT_KILL_MOB,
	"mobvnum", 	NULL, 		"prize", 	"comment" 	},
	{ EVENT_FIND_OBJ,
	"objvnum", 	NULL, 		"prize",	"comment" 	},
	{ EVENT_GIVE_OBJ,
	"objvnum", 	"mobvnum", 	"prize",	"comment" 	},
	{ EVENT_BRING_OBJ,
	"objvnum", 	"mobvnum", 	"prize",	"comment" 	},
	{ EVENT_VISIT_MOB,
	"mobvnum", 	NULL, 		"prize",	"comment" 	},
	{ EVENT_VISIT_ROOM,
	"roomvnum",	NULL, 		"prize",	"comment" 	},
	{ EVENT_LOOK_OBJ,
	"objvnum",	"roomvnum",	"prize",	"comment" 	},
	{ EVENT_LOOK_MOB,
	"mobvnum",	"roomvnum",	"prize",	"comment" 	},
	{ -999,
	NULL, 	NULL, 		NULL, 		NULL 		}
};

const struct command_type 	action_command_table []	=
{
	{ ACTION_STOPQUEST,
	"failed",	NULL, 		NULL, 		"comment" 	},
	{ ACTION_PROG,
	"mobvnum",	NULL,		NULL, 		"progname" 	},
	{ ACTION_ECHO,
	NULL,	NULL, 		NULL, 		"text" 		},
	{ ACTION_TELL,
	"mobvnum",	NULL, 		NULL, 		"text" 		},
	{ ACTION_RUNCHPT,
	NULL,	NULL, 		NULL, 		"chaptname" 	},
	{ ACTION_REWARD,
	"mobvnum",	"objvnum",	NULL,		"comment" 	},
	{ ACTION_NONE,
	NULL,	NULL, 		NULL, 		"comment" 	},
	{ -999,
	NULL, 	NULL, 		NULL, 		NULL 		}
};

int find_q_arg( int cmdtype, int cmd, char * txt )
{
	int		command;

	switch ( cmdtype )
	{
		default:
			break;
		case CMD_INIT:
			for( command = 0; init_command_table[command].cmd > -999; command ++ )
			{
				if( init_command_table[command].cmd == cmd )
				{
					if( init_command_table[command].arg1
									   &&  !str_cmp( init_command_table[command].arg1, txt ) )
						return 1;
					if( init_command_table[command].arg2
									   &&  !str_cmp( init_command_table[command].arg2, txt ) )
						return 2;
					if( init_command_table[command].arg3
									   &&  !str_cmp( init_command_table[command].arg3, txt ) )
						return 3;
					if( init_command_table[command].arg4
									   &&  !str_cmp( init_command_table[command].arg4, txt ) )
						return 4;
				}
			}
			break;
		case CMD_EVENT:
			for( command = 0; event_command_table[command].cmd > -999; command ++ )
			{
				if( event_command_table[command].cmd == cmd )
				{
					if( event_command_table[command].arg1
									   && !str_cmp( event_command_table[command].arg1, txt ) )
						return 1;
					if( event_command_table[command].arg2
									   && !str_cmp( event_command_table[command].arg2, txt ) )
						return 2;
					if( event_command_table[command].arg3
									   && !str_cmp( event_command_table[command].arg3, txt ) )
						return 3;
					if( event_command_table[command].arg4
									   && !str_cmp( event_command_table[command].arg4, txt ) )
						return 4;
				}
			}
			break;
		case CMD_ACTION:
			for( command = 0; action_command_table[command].cmd > -999; command ++ )
			{
				if( action_command_table[command].cmd == cmd )
				{
					if( action_command_table[command].arg1
									   && !str_cmp( action_command_table[command].arg1, txt ) )
						return 1;
					if( action_command_table[command].arg2
									   &&  !str_cmp( action_command_table[command].arg2, txt ) )
						return 2;
					if( action_command_table[command].arg3
									   &&  !str_cmp( action_command_table[command].arg3, txt ) )
						return 3;
					if( action_command_table[command].arg4
									   &&  !str_cmp( action_command_table[command].arg4, txt ) )
						return 4;
				}
			}
			break;
	}
	return CMD_NOTFOUND;
}


const char *show_q_arg( int cmdtype, int cmd, int argnr )
{
	int		command;

	switch ( cmdtype )
	{
		default:
			break;
		case CMD_INIT:
			for( command = 0; init_command_table[command].cmd > -999; command ++ )
			{
				if( init_command_table[command].cmd == cmd )
				{
					if( argnr == 1 )
						return init_command_table[command].arg1;
					if( argnr == 2 )
						return init_command_table[command].arg2;
					if( argnr == 3 )
						return init_command_table[command].arg3;
					if( argnr == 4 )
						return init_command_table[command].arg4;
				}
			}
			break;
		case CMD_EVENT:
			for( command = 0; event_command_table[command].cmd > -999; command ++ )
			{
				if( event_command_table[command].cmd == cmd )
				{
					if( argnr == 1 )
						return event_command_table[command].arg1;
					if( argnr == 2 )
						return event_command_table[command].arg2;
					if( argnr == 3 )
						return event_command_table[command].arg3;
					if( argnr == 4 )
						return event_command_table[command].arg4;
				}
			}
			break;
		case CMD_ACTION:
			for( command = 0; action_command_table[command].cmd > -999; command ++ )
			{
				if( action_command_table[command].cmd == cmd )
				{
					if( argnr == 1 )
						return action_command_table[command].arg1;
					if( argnr == 2 )
						return action_command_table[command].arg2;
					if( argnr == 3 )
						return action_command_table[command].arg3;
					if( argnr == 4 )
						return action_command_table[command].arg4;
				}
			}
			break;
	}

	return (char *)"unknown";
}





