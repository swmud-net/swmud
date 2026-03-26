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
*			     Mud constants module			   *
****************************************************************************/

#include <sys/types.h>
#include <time.h>
#include "mud.h"

const char* inv_pers_i[6] = {	"Kto�",
				"Kogo�",
				"Komu�",
				"Kogo�",
				"Kim�",
				"Kim�"	};


/* Trog: rasa bazowa (u nas czlowiek) */
const RACE_DATA human_race =
{
	(char *)"Human",
	(char *)"",
	{ (char *)"M�czyzna", (char *)"M�czyzny", (char *)"M�czy�nie",(char *) "M�czyzn�",
			(char *)"M�czyzn�", (char *)"M�czy�nie"
	},
	{ (char *)"Kobieta", (char *)"Kobiety", (char *)"Kobiecie",(char *) "Kobiet�",
			(char *)"Kobiet�", (char *)"Kobiecie"                                 // Pixel
	},
	(char *)"S� najliczniejsz� i najzwyklejsz� z ras zamieszkuj�cych gataktyke. Nie" NL
	"posiadaj� �adnych specjalnych umiej�tno�ci czy bonus�w, ale tak�e nie maj�" NL
	"wielu wad. Ludzie �atwo przystosowuj� sie do wszystkich warunk�w i s� zdolni" NL
	"do obrania wiekszo�ci klas. Ludzie posiadaj� dwa ramiona, dwie nogi i" NL
	"okr�g�e g�owy. Zaskakuj�co ludzie maj� tendencj� wygl�dac inaczej od siebie." NL,
	FRACE_PC_AVAIL,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	(LANG_DATA *)&lang_common
  , {524286,	16776958,	1022,	1179666,	16777214,	44922488,	2145222630,	3850174,	0}
};

/** Trog: jezyk bazowy (u nas common) */
const LANG_DATA lang_common =
{
	{},
	{},
	(char *)"Basic",
	(char *)"",
	(char *)"Nie mo�esz m�wi� we wsp�lnym.",
	99,
	75,
	10,
	0
};

const	struct	class_type	class_table	[MAX_ABILITY]	=
{
	{	"combat", 		"Com",
		{
		    "Walka",
		    "walki",
		    "walce",
		    "walk�",
		    "walk�",
		    "walce",
		    "w walce"
		},
#if defined (ARMAGEDDON)
		{ 5, 0, 0, 0, 2, 0, 0 },
		18,
		1,
#endif
		REQ_STR
	},
	{	"piloting", 		"Pil",
		{
		    "Pilota�",
		    "pilota�u",
		    "pilota�owi",
		    "pilota�",
		    "pilota�em",
		    "pilota�u",
		    "w pilotowaniu"
		},
#if defined (ARMAGEDDON)
		{ 2, 2, 1, 0, 0, 0, 0 },
		18,
		4,
#endif
		REQ_WIS
	},
	{	"engineering", 		"Eng",
		{
		    "In�ynieria",
		    "in�ynierii",
		    "in�ynierii",
		    "in�ynieri�",
		    "in�ynieri�",
		    "in�ynierii",
		    "w in�ynierii"
		},
#if defined (ARMAGEDDON)
		{ 0, 3, 4, 0, 0, 0, 0 },
		18,
		6,
#endif
		REQ_INT
	},
	{	"bounty hunting", 	"Bou",
		{
		    "�owienie nagr�d",
		    "�owienia nagr�d",
		    "�owieniu nagr�d",
		    "�owienie nagr�d",
		    "�owieniem nagr�d",
		    "�owieniu nagr�d",
		    "w �owieniu nagr�d"
		},
#if defined (ARMAGEDDON)
		{ 3, 3, 0, 0, 1, 0, 0 },
		18,
		0,
#endif
		REQ_CON
	},
	{	"smuggling", 		"Smu",
		{
		    "Przemyt",
		    "przemytu",
		    "przemytowi",
		    "przemyt",
		    "przemytem",
		    "przemycie",
		    "w przemycie"
		},
#if defined (ARMAGEDDON)
		{ 0, 1, 0, 4, 1, 1, 0 },
		18,
		3,
#endif
		REQ_DEX
	},
	{
		"diplomacy", 		"Dip",
		{
		    "Dyplomacja",
		    "dyplomacji",
		    "dyplomacji",
		    "dyplomacj�",
		    "dyplomacj�",
		    "dyplomacji",
		    "w dyplomacji"
		},
#if defined (ARMAGEDDON)
		{ 0, 5, 1, 1, 0, 0, 0 },
		18,
		6,
#endif
		REQ_CHA
	},
	{	"leadership", 		"Lea",
		{
		    "Przyw�dczo��",
		    "przyw�dczo�ci",
		    "przyw�dczo�ci",
		    "przyw�dczo��",
		    "przyw�dczo�ci�",
		    "przyw�dczo�ci",
		    "w przyw�dztwie"
		},
#if defined (ARMAGEDDON)
		{ 1, 2, 3, 0, 1, 0, 0 },
		18,
		5,
#endif
		REQ_WIS
	},
	{	"force", 		"For",
		{
		    "W�adanie Moc�",
		    "w�adania Moc�",
		    "w�adaniu Moc�",
		    "w�adanie Moc�",
		    "w�adaniem Moc�",
		    "w�adaniu Moc�",
		    "we w�adaniu Moc�"
		},
#if defined (ARMAGEDDON)
		{ 0, 5, 1, 1, 0, 0, 0 },
		18,
		2,
#endif
		REQ_FRC
	}
};



/*
 * Attribute bonus tables.
 */
const	struct	str_app_type	str_app		[26]		=
{
    { -5, -4,   0,  0 },  /* 0  */
    { -5, -4,   3,  1 },  /* 1  */
    { -3, -2,   3,  2 },
    { -3, -1,  10,  3 },  /* 3  */
    { -2, -1,  25,  4 },
    { -2, -1,  55,  5 },  /* 5  */
    { -1,  0,  80,  6 },
    { -1,  0,  90,  8 },
    {  0,  0, 100, 10 },
    {  0,  0, 100, 12 },
    {  0,  0, 115, 14 }, /* 10  */
    {  0,  0, 115, 15 },
    {  0,  0, 140, 16 },
    {  0,  0, 140, 17 }, /* 13  */
    {  0,  1, 170, 18 },
    {  1,  1, 170, 19 }, /* 15  */
    {  1,  2, 195, 20 },
    {  2,  3, 220, 22 },
    {  2,  4, 250, 25 }, /* 18  */
    {  3,  5, 400, 30 },
    {  3,  6, 500, 35 }, /* 20  */
    {  4,  7, 600, 40 },
    {  5,  7, 700, 45 },
    {  6,  8, 800, 50 },
    {  8, 10, 900, 55 },
    { 10, 12, 999, 60 }  /* 25   */
};



const	struct	int_app_type	int_app		[26]		=
{
    {  3 },	/*  0 */
    {  5 },	/*  1 */
    {  7 },
    {  8 },	/*  3 */
    {  9 },
    { 10 },	/*  5 */
    { 11 },
    { 12 },
    { 13 },
    { 15 },
    { 17 },	/* 10 */
    { 19 },
    { 22 },
    { 25 },
    { 28 },
    { 31 },	/* 15 */
    { 34 },
    { 37 },
    { 40 },	/* 18 */
    { 44 },
    { 49 },	/* 20 */
    { 55 },
    { 60 },
    { 70 },
    { 85 },
    { 99 }	/* 25 */
};



const	struct	wis_app_type	wis_app		[26]		=
{
    { 0 },	/*  0 */
    { 0 },	/*  1 */
    { 0 },
    { 0 },	/*  3 */
    { 0 },
    { 1 },	/*  5 */
    { 1 },
    { 1 },
    { 1 },
    { 2 },
    { 2 },	/* 10 */
    { 2 },
    { 2 },
    { 2 },
    { 2 },
    { 3 },	/* 15 */
    { 3 },
    { 4 },
    { 5 },	/* 18 */
    { 5 },
    { 5 },	/* 20 */
    { 6 },
    { 6 },
    { 6 },
    { 6 },
    { 7 }	/* 25 */
};



const	struct	dex_app_type	dex_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};



const	struct	con_app_type	con_app		[26]		=
{
    { -4, 20 },   /*  0 */
    { -3, 25 },   /*  1 */
    { -2, 30 },
    { -2, 35 },	  /*  3 */
    { -1, 40 },
    { -1, 45 },   /*  5 */
    { -1, 50 },
    {  0, 55 },
    {  0, 60 },
    {  0, 65 },
    {  0, 70 },   /* 10 */
    {  0, 75 },
    {  0, 80 },
    {  0, 85 },
    {  0, 88 },
    {  1, 90 },   /* 15 */
    {  2, 95 },
    {  2, 97 },
    {  3, 99 },   /* 18 */
    {  3, 99 },
    {  4, 99 },   /* 20 */
    {  4, 99 },
    {  5, 99 },
    {  6, 99 },
    {  7, 99 },
    {  8, 99 }    /* 25 */
};


const	struct	cha_app_type	cha_app		[26]		=
{
    { - 60 },   /* 0 */
    { - 50 },   /* 1 */
    { - 50 },
    { - 40 },
    { - 30 },
    { - 20 },   /* 5 */
    { - 10 },
    { -  5 },
    { -  1 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    1 },
    {    5 },   /* 15 */
    {   10 },
    {   20 },
    {   30 },
    {   40 },
    {   50 },   /* 20 */
    {   60 },
    {   70 },
    {   80 },
    {   90 },
    {   99 }    /* 25 */
};

/* Have to fix this up - not exactly sure how it works (Scryn) */
const	struct	lck_app_type	lck_app		[26]		=
{
    {   60 },   /* 0 */
    {   50 },   /* 1 */
    {   50 },
    {   40 },
    {   30 },
    {   20 },   /* 5 */
    {   10 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    { - 10 },   /* 15 */
    { - 15 },
    { - 20 },
    { - 30 },
    { - 40 },
    { - 50 },   /* 20 */
    { - 60 },
    { - 75 },
    { - 90 },
    { -105 },
    { -120 }    /* 25 */
};

const	struct	frc_app_type	frc_app		[26]		=
{
    {    0 },   /* 0 */
    {    0 },   /* 1 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 5 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 10 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 15 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    {    0 },   /* 20 */
    {    0 },
    {    0 },
    {    0 },
    {    0 },
    {    0 }    /* 25 */
};



/*
 * Liquid properties.
 * Used in #OBJECT section of area file.
 */
const	struct	liq_type	liq_table	[LIQ_MAX]	=
{
    { "wod�",			"prze�roczyst�",{  0, 1, 10 }	},  /*  0 */
    { "piwo",			"br�zow�",	{  3, 2,  5 }	},
    { "wino",			"r�owaw�",	{  5, 2,  5 }	},
    { "ale",			"br�zow�",	{  2, 2,  5 }	},
    { "ciemne ale",		"ciemnobr�zow�",{  1, 2,  5 }	},

    { "whisky",			"z�otaw�",	{  6, 1,  4 }	},  /*  5 */
    { "lemoniad�",		"r�ow�",	{  0, 1,  8 }	},
    { "gorza��",			"m�tn�",	{ 10, 0,  0 }	},
    { "lokalny specja�",	"m�tniaw�",	{  3, 3,  3 }	},
    { "sok �urawinowy",		"zielon�",	{  0, 4, -8 }	},

    { "mleko",			"bia��",	{  0, 3,  6 }	},  /* 10 */
    { "herbat�",		"zielonobr�zow�",{  0, 1,  6 }	},
    { "kaw�",			"czarn�",	{  0, 1,  6 }	},
    { "krew",			"czerwon�",	{  0, 2, -1 }	},
    { "morsk� wod�",		"prze�roczyst�",{  0, 1, -2 }	},

    { "col�",			"wi�niow�",	{  0, 1,  5 }	},  /* 15 */
    { "poncz",			"jasnobr�zow�",	{  4, 2,  5 }	},  /* 16 */
    { "grog",			"br�zow�",	{  3, 2,  5 }	},  /* 17 */
    { "shake'a",              	"kremow�",      {  0, 8,  5 }   },   /* 18 */
    { "drinka",              	"przezroczyst�",{  0, 8,  5 }   }   /* 19 */
};

const char *	const	attack_table	[13] =
{
    "uderzenie",
    "ci�cie",  "pchni�cie",  "palni�cie", "smagni�cie", "darpni�cie",
    "buchni�cie",  "r�bni�cie", "trza�ni�cie", "postrzelenie", "ugryzienie",
    "d�gni�cie", "ssanie"
};

const char *	const	sith_hatred	[10] =
{
    FG_RED "Czujesz wzrastaj�c� w sobie nienawi��!" EOL,
    FG_RED "Zaczynasz coraz bardziej nienawidzi� �ywych istot!" EOL,
    FG_RED "Twoja nienawi�� staje si� coraz wi�ksza!" EOL,
    FG_RED "Strach prowadzi Ci� prosto do Ciemnej Strony Mocy!" EOL,
    FG_RED "Z ka�d� chwil� Tw�j gniew staje si� silniejszy!" EOL,
    FG_RED "Strach staje si� Twoim sprzymierze�cem!" EOL,
    FG_RED "Powoli odczuwasz pot�g� Ciemnej Strony Mocy!" EOL,
    FG_RED "Gniew przyczynia si� do Twojej si�y!" EOL,
    FG_RED "Czujesz jak Ciemna Strona Mocy wspomaga Ci�!" EOL,
    FG_RED "Dzi�ki nienawi�ci zyskujesz si��!" EOL
};
const char *  male_desc_y[6]={"y","ego","emu","ego","ym","ym" };
const char *	male_desc_i[6]={"i","iego","iemu","iego","im","im"};
const char *  female_desc_y[6]={"a","ej","ej","�","�","ej"};
const char *  female_desc_i[6]={"a","iej","iej","�","�","iej"};
const char *  category_desc[]={"W�osy","Budowa","Wiek","Sk�ra","Zarost","Twarz","Og�lne","Oczy","Futro","Empty"};
const	struct	char_desc	desc_table1[]=
{
	{	HAIR,"bia�ow�os",				Y_END	},
	{	HAIR,"ciemnow�os",				Y_END	},
	{	HAIR,"czarnow�os",				Y_END	},
	{	HAIR,"czerwonow�os",			Y_END	},
	{	HAIR,"d�ugow�os",				Y_END	},
	{	HAIR,"jasnow�os",				Y_END	},
	{	HAIR,"k�dzierzaw",				Y_END	},
	{	HAIR,"kr�tkow�os",				Y_END	},
	{	HAIR,"kud�at",					Y_END	},
	{	HAIR,"�ysiej�c",				Y_END	},
	{	HAIR,"�ys",						Y_END	},
	{	HAIR,"niebieskowlos",			Y_END	},
	{	HAIR,"ognistow�os",				Y_END	},
	{	HAIR,"pomara�czowow�os",		Y_END	},
	{	HAIR,"rudow�os",				Y_END	},
	{	HAIR,"rud",						Y_END	},
	{	HAIR,"ry�",						Y_END	},
	{	HAIR,"siw",						Y_END	},
	{	HAIR,"siwow�os",				Y_END	},
	{ (desc_type)0 , "",Y_END }
};
const	struct	char_desc	desc_table2[]=
{
	{	BUILD,"barczyst",			Y_END	}, //20
	{	BUILD,"bary�kowat",			Y_END	},
	{	BUILD,"beczu�kowat",		Y_END	},
	{	BUILD,"brzuchat",			Y_END	},
	{	BUILD,"chud",				Y_END	},
	{	BUILD,"d�ugonog",			I_END	},
	{	BUILD,"grub",				Y_END	},
	{	BUILD,"insektoidaln",		Y_END	},
	{	BUILD,"kr�p",				Y_END	},
	{	BUILD,"kr�tkonog",			I_END	},
	{	BUILD,"krzywonog",			I_END	}, //30
	{	BUILD,"masywn",				Y_END	},
	{	BUILD,"muskularn",			Y_END	},
	{	BUILD,"niewysok",			I_END	},
	{	BUILD,"nisk",				I_END	},
	{	BUILD,"okaza�",				Y_END	},
	{	BUILD,"p�kat",				Y_END	},
	{	BUILD,"przysadzist",		Y_END	},
	{	BUILD,"szczup�",			Y_END	},
	{	BUILD,"t�u�ciutk",			I_END	},
	{	BUILD,"umi�nion",			Y_END	}, //40
	{	BUILD,"wychudzon",			Y_END	},
	{	BUILD,"wysok",				I_END	},
	{ (desc_type)0 , "",Y_END }
};
const	struct	char_desc	desc_table3[]=
{
	{	AGE,"dojrza�",            Y_END		},
	{	AGE,"leciw",              Y_END		},
	{	AGE,"ma�oletn",           I_END		}, //40
	{	AGE,"m�od",               Y_END		},
	{	AGE,"podrastaj�c",        Y_END		},
	{	AGE,"podstarza�",         Y_END		},
	{	AGE,"star",               Y_END		},
	{	AGE,"szczawikowat",       Y_END		},
	{	AGE,"wiekow",             Y_END		},
	{ (desc_type)0 , "",Y_END }
};
const	struct	char_desc	desc_table4[]=
{
	{	SKIN,"bladosk�r",			Y_END	},
	{	SKIN,"br�zowosk�r",			Y_END 	},
	{	SKIN,"czerwonosk�r",		Y_END 	},
	{	SKIN,"ciemnosk�r",			Y_END 	},
	{	SKIN,"fioletowosk�r",		Y_END 	},
	{	SKIN,"jaskrawosk�r",		Y_END 	},
	{	SKIN,"jasnosk�r",			Y_END 	},
	{	SKIN,"gadosk�r",			Y_END 	},
	{	SKIN,"karmazynowosk�r",		Y_END 	},
	{	SKIN,"kosmat",				Y_END 	},
	{	SKIN,"letha�sk",			I_END 	},
	{	SKIN,"�ososiowosk�r",		Y_END 	},
	{	SKIN,"niebieskosk�r",		Y_END 	},
	{	SKIN,"pomara�czowosk�r",	Y_END 	},
	{	SKIN,"rutia�sk",			I_END 	},
	{	SKIN,"turkosowosk�r",		Y_END 	},
	{	SKIN,"opalon",				Y_END 	},
	{	SKIN,"stalowosk�r",			Y_END 	},
	{	SKIN,"szarosk�r",			Y_END 	},
	{	SKIN,"�niad",				Y_END 	},
	{	SKIN,"w�ochat",				Y_END 	},
	{	SKIN,"zielonosk�r",			Y_END 	},
	{	SKIN,"��tosk�r",			Y_END 	},
	{ (desc_type)0 , "",Y_END }
  };
const	struct	char_desc	desc_table5[]=
{
	{	BEARD,"bia�obrod",				Y_END	},
	{	BEARD,"b��kitnobrod",			Y_END	},
	{	BEARD,"brodat",					Y_END	},
	{	BEARD,"ciemnobrod",				Y_END	},
	{	BEARD,"czarnobrod",				Y_END	},
	{	BEARD,"czerwonobrod",			Y_END	},
	{	BEARD,"d�ugobrod",				Y_END	},
	{	BEARD,"jasnobrod",				Y_END	},
	{	BEARD,"kr�tkobrod",				Y_END	}, //60
	{	BEARD,"kruczobrod",				Y_END	},
	{	BEARD,"krzaczast",				Y_END	},
	{	BEARD,"ognistobrod",			Y_END	},
	{	BEARD,"p�omiennobrod",			Y_END	},
	{	BEARD,"pomara�czowobrod",		Y_END	},
	{	BEARD,"rudobrod",				Y_END	},
	{	BEARD,"sinobrod",				Y_END	},
	{	BEARD,"siwobrod",				Y_END	},
	{	BEARD,"sko�tunion",				Y_END	},
	{	BEARD,"�nie�nobrod",			Y_END	}, //70
	{	BEARD,"w�sat",					Y_END	},
	{	BEARD,"zaro�ni�t",				Y_END	},
	{	BEARD,"zielonobrod",			Y_END	},
	{	BEARD,"��tobrod",				Y_END	},
	{ (desc_type)0 , "",Y_END }
  };
const	struct	char_desc	desc_table6[]=
{
	{	FACE,"beznos",				Y_END	},
	{	FACE,"bezw�os",				Y_END	},
	{	FACE,"czerwononos",			Y_END	},
	{	FACE,"d�ugonos",			Y_END	},
	{	FACE,"d�ugouch",			Y_END	},
	{	FACE,"dziobat",				Y_END	},
	{	FACE,"g�adkolic",			Y_END	},
	{	FACE,"jednorog",			I_END	},
	{	FACE,"kr�tkonos",			Y_END	},
	{	FACE,"krzywonos",			Y_END	}, //80
	{	FACE,"mackog�ow",			Y_END	},
	{	FACE,"nakrapian",			Y_END	},
	{	FACE,"ogorza�",				Y_END	},
	{	FACE,"ospowat",				Y_END	},
	{	FACE,"ostronos",			Y_END	},
	{	FACE,"piegowat",			Y_END	},
	{	FACE,"p�askonos",			Y_END	},
	{	FACE,"pryszczat",			Y_END	},
	{	FACE,"pucu�owat",			Y_END	},
	{	FACE,"rogat",				Y_END	},
	{	FACE,"rumian",				Y_END	},
	{	FACE,"szpiczastouch",		Y_END	},
	{	FACE,"szczerbat",			Y_END	},
	{	FACE,"wielkonos",			Y_END	},
	{	FACE,"wielkog�ow",			Y_END	},
	{ (desc_type)0 , "",Y_END }
  };
const	struct	char_desc	desc_table7[]=
{
	{	GENERAL,"bezwzgl�dn",		Y_END	}, //90
	{	GENERAL,"brudn",			Y_END	},
	{ 	GENERAL,"chciw",			Y_END	},
	{ 	GENERAL,"cwan",				Y_END	},
	{	GENERAL,"czujn",			Y_END	},
	{	GENERAL,"dostojn",			Y_END	},
	{	GENERAL,"dumn",				Y_END	},
	{	GENERAL,"energiczn",		Y_END	},
	{	GENERAL,"gro�n",			Y_END	},
	{	GENERAL,"hard",				Y_END	},
	{ 	GENERAL,"inteligentn",		Y_END	},
	{	GENERAL,"ma�om�wn",			Y_END	},
	{	GENERAL,"melancholijn",		Y_END	},
	{	GENERAL,"nerwow",			Y_END	}, //100
	{ 	GENERAL,"oble�n",			Y_END	},
	{	GENERAL,"opanowan",			Y_END	},
	{	GENERAL,"paskudn",			Y_END	},
	{	GENERAL,"ponur",			Y_END	},
	{	GENERAL,"porywcz",			Y_END	},
	{	GENERAL,"powa�n",			Y_END	},
	{	GENERAL,"przyg�up",			I_END	},
	{	GENERAL,"przyjacielsk",		I_END	},
	{	GENERAL,"rozmown",			Y_END	},
	{	GENERAL,"spokojn",			Y_END	},
	{	GENERAL,"�mierdz�c",		Y_END	},
	{	GENERAL,"t�p",				Y_END	},
	{	GENERAL,"weso�",			Y_END	}, //110
	{	GENERAL,"wynios�",			Y_END	},
	{	GENERAL,"zawadiack",		I_END	},
	{	GENERAL,"z�o�liw",			Y_END	},
	{ (desc_type)0 , "",Y_END }
  };
const	struct	char_desc	desc_table8[]=
{
	{	EYES,"b��kitnook",			I_END	},
	{	EYES,"br�zowook",			I_END	},
	{	EYES,"ciemnook",			I_END	},
	{	EYES,"czarnook",			I_END	},
	{	EYES,"czerwonook",			I_END	},
	{	EYES,"gadook",				I_END	},
	{	EYES,"jasnook",				I_END	},
	{	EYES,"jednook",				I_END	}, //120
	{	EYES,"krwistook",			I_END	},
	{	EYES,"m�tnook",				I_END	},
	{	EYES,"niebieskook",			I_END	},
	{	EYES,"pomara�czowook",		I_END	},
	{	EYES,"piwnook",				I_END	},
	{	EYES,"rybiook",				I_END	},
	{	EYES,"szarook",				I_END	},
	{	EYES,"�wi�skook",			I_END	},
	{	EYES,"wielkook",			I_END	},
	{	EYES,"wy�upiastook",		I_END	},
	{	EYES,"zezowat",				Y_END	},
	{	EYES,"zielonook",			I_END	},
	{	EYES,"z�took",				I_END	},
	{ (desc_type)0 , "",Y_END }
  };
const	struct	char_desc	desc_table9[]=
{
	{	FUR,"br�zowym",				Y_END	},
	{	FUR,"ciemnobr�zowym",		Y_END	},
	{	FUR,"czarnym",				Y_END	},
	{	FUR,"jasnobr�zowym",		Y_END	},
	{	FUR,"kosmatym",				Y_END	},
	{	FUR,"rozczochranym",		Y_END	},
	{	FUR,"rudym",				Y_END	},
	{	FUR,"srebrzystym",			Y_END	},
	{	FUR,"siwym",				Y_END	},
	{	(desc_type)0,"",Y_END	}
  };

//dodawac kolejne pozycje na ko�cu, nie sortowac
const	struct	char_desc	 *desc_table [MAX_DESC_TYPES]=
//const	struct	char_desc	 desc_table [MAX_DESC_TYPES][]=
{
	desc_table1,
	desc_table2,
	desc_table3,
	desc_table4,
	desc_table5,
	desc_table6,
	desc_table7,
	desc_table8,
	desc_table9
};
