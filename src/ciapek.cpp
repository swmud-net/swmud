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
 *			  Made by Thanos 4 SW-Mud			   *
 *  Plik ten zawierac ma funkcje, kt�re u�ywane by�y na starym engine'ie    *
 *                                sw-muda                                   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"
#include <SWTimeStamp.h>

/* Taken from Rom24 by Thanos
 Fajna rzecz. Te dwa ostatnie pliki, ktore zrzuca szczegolnie
 przydatne przy tropieniu garbage collection.

 Oczywiscie te wyniki ile ramu jest zajete to pokazuja tylko
 zajetosc przez wskazniki - stringi to inna bajka
 */
DEF_DO_FUN( dump )
{
	int count, num_pcs, aff_count, req_count, n;
	CHAR_DATA *fch;
	MOB_INDEX_DATA *pMobIndex;
	PC_DATA *pc;
	OBJ_DATA *obj;
	OBJ_INDEX_DATA *pObjIndex = 0;
	ROOM_INDEX_DATA *room;
	EXIT_DATA *exit;
	DESCRIPTOR_DATA *d;
	AFFECT_DATA *af;
	REQUIREMENT_DATA *req;

	SHIP_DATA *sh;
	SHIP_ROOM_DATA *shroom;
	QUEST_INDEX_DATA *qid;
	QUEST_DATA *qd;
	SHOP_DATA *shop;
	REPAIR_DATA *rep;

	FILE *fp;
	int vnum, nMatch = 0;
	int Mem, TotMem = 0;

	ch_printf(ch, "Memory dumping..." NL);

	/* open file */
	RESERVE_CLOSE;
	fp = fopen(TMP_DIR "mem.dmp", "w");

	/* report use of data structures */

	num_pcs = 0;
	aff_count = 0;
	req_count = 0;

	/* mobile prototypes */
	Mem = top_mob_index * (sizeof(*pMobIndex));
	TotMem += Mem;
	fprintf(fp, "MobProt	%5d    [%8d bytes (%5d KB)]\n", top_mob_index, Mem,
			Mem / 1024);

	/* mobs */
	count = 0;
	for (auto* fch : char_list)
	{
		count++;
		if (fch->pcdata != NULL)
			num_pcs++;
		aff_count += fch->affects.size();
	}

	Mem = count * (sizeof(*fch));
	TotMem += Mem;
	fprintf(fp, "Mobs	%5d    [%8d bytes (%5d KB)]\n", count, Mem, Mem / 1024);

	Mem = num_pcs * (sizeof(*pc));
	TotMem += Mem;
	fprintf(fp, "Pcdata	%5d    [%8d bytes (%5d KB)]\n", num_pcs, Mem,
			Mem / 1024);

	/* descriptors */
	count = descriptor_list.size();

	Mem = count * (sizeof(*d));
	TotMem += Mem;
	fprintf(fp, "Descs	%5d    [%8d bytes (%5d KB)]\n", count, Mem, Mem / 1024);

	/* quests */
	Mem = top_quest_index * (sizeof(*qid));
	TotMem += Mem;
	fprintf(fp, "QuesIdx %5d    [%8d bytes (%5d KB)]\n", top_quest_index, Mem,
			Mem / 1024);

	Mem = top_quest * (sizeof(*qd));
	TotMem += Mem;
	fprintf(fp, "Quests  %5d    [%8d bytes (%5d KB)]\n", top_quest, Mem,
			Mem / 1024);

	/* shops */
	Mem = top_shop * (sizeof(*shop));
	TotMem += Mem;
	fprintf(fp, "Shops   %5d    [%8d bytes (%5d KB)]\n", top_repair, Mem,
			Mem / 1024);

	Mem = top_quest * (sizeof(*rep));
	TotMem += Mem;
	fprintf(fp, "Repairs %5d    [%8d bytes (%5d KB)]\n", top_repair, Mem,
			Mem / 1024);

	/* object prototypes */
	for (vnum = 0; nMatch < top_obj_index; vnum++)
		if ((pObjIndex = get_obj_index(vnum)) != NULL)
		{
			aff_count += pObjIndex->affects.size();
			req_count += pObjIndex->requirements.size();
			nMatch++;
		}

	Mem = top_obj_index * (sizeof(*pObjIndex));
	TotMem += Mem;
	fprintf(fp, "ObjProt	%5d    [%8d bytes (%5d KB)]\n", top_obj_index, Mem,
			Mem / 1024);

	/* objects */
	count = 0;
	for (auto* obj : object_list)
	{
		count++;
		aff_count += obj->affects.size();

		req_count += pObjIndex->requirements.size();
	}

	Mem = count * (sizeof(*obj));
	TotMem += Mem;
	fprintf(fp, "Objs	%5d    [%8d bytes (%5d KB)]\n", count, Mem, Mem / 1024);

	Mem = aff_count * (sizeof(*af));
	TotMem += Mem;
	fprintf(fp, "Affects	%5d    [%8d bytes (%5d KB)]\n", aff_count, Mem,
			Mem / 1024);

	Mem = req_count * (sizeof(*req));
	TotMem += Mem;
	fprintf(fp, "Reqs    %5d    [%8d bytes (%5d KB)]\n", req_count, Mem,
			Mem / 1024);

	Mem = top_help * (sizeof(HELP_DATA));
	TotMem += Mem;
	fprintf(fp, "Helps   %5d    [%8d bytes (%5d KB)]\n", top_help, Mem,
			Mem / 1024);

	Mem = top_reset * (sizeof(RESET_DATA));
	TotMem += Mem;
	fprintf(fp, "Resets  %5d    [%8d bytes (%5d KB)]\n", top_reset, Mem,
			Mem / 1024);

	Mem = top_area * (sizeof(AREA_DATA));
	TotMem += Mem;
	fprintf(fp, "Areas   %5d    [%8d bytes (%5d KB)]\n", top_area, Mem,
			Mem / 1024);

	Mem = top_ed * (sizeof(EXTRA_DESCR_DATA));
	TotMem += Mem;
	fprintf(fp, "ExtrDes %5d    [%8d bytes (%5d KB)]\n", top_ed, Mem,
			Mem / 1024);

	n = (int)ship_list.size();
	Mem = n * (sizeof(*sh));
	TotMem += Mem;
	fprintf(fp, "Ships   %5d    [%8d bytes (%5d KB)]\n", n, Mem, Mem / 1024);

	/* rooms */
	Mem = top_room * (sizeof(*room));
	TotMem += Mem;
	fprintf(fp, "Rooms	%5d    [%8d bytes (%5d KB)]\n", top_room, Mem,
			Mem / 1024);

	Mem = top_sroom * (sizeof(*shroom));
	TotMem += Mem;
	fprintf(fp, "ShpRoom %5d    [%8d bytes (%5d KB)]\n", top_sroom, Mem,
			Mem / 1024);

	Mem = top_vroom * (sizeof(*room));
	TotMem += Mem;
	fprintf(fp, "VRooms	%5d    [%8d bytes (%5d KB)]\n", top_vroom, Mem,
			Mem / 1024);

	/* exits */
	Mem = top_exit * (sizeof(*exit));
	TotMem += Mem;
	fprintf(fp, "Exits	%5d    [%8d bytes (%5d KB)]\n", top_exit, Mem,
			Mem / 1024);

	fprintf(fp, "-----------------------------------------\n"
			"Total:            %8d bytes (%5d KB)\n", TotMem, TotMem / 1024);
	fclose(fp);

	/* start printing out mobile data */
	fp = fopen(TMP_DIR"mob.dmp", "w");

	fprintf(fp, "\nMobile Analysis\n");
	fprintf(fp, "---------------\n");
	nMatch = 0;
	for (vnum = 0; nMatch < top_mob_index; vnum++)
		if ((pMobIndex = get_mob_index(vnum)) != NULL)
		{
			nMatch++;
			fprintf(fp, "#%-5d %3d active %3d killed     %s\n", pMobIndex->vnum,
					pMobIndex->count, pMobIndex->killed,
					pMobIndex->przypadki[0]);
		}
	fclose(fp);

	/* start printing out object data */
	fp = fopen(TMP_DIR"obj.dmp", "w");

	fprintf(fp, "\nObject Analysis\n");
	fprintf(fp, "---------------\n");
	nMatch = 0;
	for (vnum = 0; nMatch < top_obj_index; vnum++)
		if ((pObjIndex = get_obj_index(vnum)) != NULL)
		{
			nMatch++;
			fprintf(fp, "#%-5d %3d active       %s\n", pObjIndex->vnum,
					pObjIndex->count,
					/*pObjIndex->reset_num,*/pObjIndex->przypadki[0]);
		}

	/* close file */
	fclose(fp);
	RESERVE_OPEN;

	ch_printf(ch, "Done." NL);
	return;
}

char* my_one_argument(char *argument, char *arg_first)
{
	char cEnd;
	int count;

	count = 0;

	while (isspace(*argument))
		argument++;

	cEnd = ' ';
	if (*argument == '\'' || *argument == '"')
		cEnd = *argument++;

	while (*argument != '\0' || ++count >= 255)
	{
		if (*argument == cEnd)
		{
			argument++;
			break;
		}
		*arg_first = *argument;
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while (isspace(*argument))
		argument++;

	return argument;
}

//dwie poni�sze funkcje s�u�� do edycji kurs�w statk�w publicznych OLCem
bool save_bus_list()
{
	FILE *fp;
	char buf[MAX_STRING_LENGTH];
	int i = 0;

	if ((fp = fopen( BUS_LIST, "w")) == NULL)
	{
		bug("Cannot open BUS_LIST file for writing");
		perror( BUS_LIST);
		return false;
	}

	for (auto* ship : ship_list)
		if (ship->ship_public && !ship->stops.empty())
		{
			i++;
			fprintf(fp, "P %s~\n", ship->filename);
		}
	fprintf(fp, "$\n");
	fclose(fp);

	sprintf(buf, "Save_bus_list: %d ship%s written do %s", i, i == 1 ? "" : "s",
	BUS_LIST);

	log_string(buf);
	return true;
}

bool save_ship_course(SHIP_DATA *ship)
{
	FILE *fp;
	char filename[256];

	sprintf(filename, "%s%s.course", SHIP_DIR, ship->filename);

	if ((fp = fopen(filename, "w")) == NULL)
	{
		bug("couldn't write course file for ship");
		perror(filename);
		return false;
	}

	for (auto* course : ship->stops)
		fprintf(fp, "%d %s~\n", course->stop_vnum, course->stop_name);

	fprintf(fp, "0\n");
	fclose(fp);

	log_string("Save_ship_course: Ok");
	return true;
}

void showcourse(CHAR_DATA *ch, SHIP_DATA *ship)
{
	if (!ship->ship_public || ship->stops.empty())
		return;

	ch_printf(ch, FB_WHITE "Nr   StationName       StationVnum" EOL);
	int i = 0;
	for (auto* course : ship->stops)
	{
		ch_printf(ch, "%-2d]  %-20s" PLAIN "-%-8d (%s" PLAIN ")" EOL, i,
				course->stop_name, course->stop_vnum,
				get_room_index(course->stop_vnum) ?
						get_room_index(course->stop_vnum)->name :
						MOD_BLINK FB_GREEN "NO_SUCH_LOCATION" MOD_NORMAL PLAIN);
		i++;
	}
	return;
}

COURSE_DATA* get_course(SHIP_DATA *ship, int nr)
{
	if (ship->stops.empty())
		return NULL;

	int i = 0;
	for (auto* course : ship->stops)
		if (i++ == nr)
			return course;

	return NULL;
}

DEF_DO_FUN( setcourse )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	SHIP_DATA *ship;
	COURSE_DATA *course;

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char(
				"Syntax: setcourse <ship>  add      <station_vnum> <station_name>" NL
				"        setcourse <ship>  edit     <station_number> vnum <station_vnum>" NL
				"        setcourse <ship>  edit     <station_number> name <station_name>" NL
				"        setcourse <ship>  delete   <station_number>" NL
				"        setcourse <ship>  show" NL
				"        setcourse <ship>  save" NL
				"        setcourse savelist" NL, ch);
		return;
	}

	if (!str_cmp(arg1, "savelist"))
	{
		if (save_bus_list())
			send_to_char("Ok." NL, ch);
		else
			send_to_char("Note Coders about a bug." NL, ch);
		return;
	}

	ship = get_ship(arg1);
	if (!ship)
	{
		send_to_char("No such ship." NL, ch);
		return;
	}

	if (!ship->ship_public)
	{
		send_to_char("That is not a public ship." NL
		"You can change it using a 'setship' command." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "show") || arg2[0] == '\0')
	{
		showcourse(ch, ship);
		return;
	}

	argument = one_argument(argument, arg3);

	if (!str_cmp(arg2, "add")) //dodajemy stacj� na koniec listy
	{
		CREATE(course, COURSE_DATA, 1);
		STRDUP(course->stop_name, argument);
		course->stop_vnum = atoi(arg3);
		ship->stops.push_back(course);
	}
	else if (!str_cmp(arg2, "edit")) //edytujemy istniej�c� stacj�
	{
		int nr;

		nr = atoi(arg3);
		argument = one_argument(argument, arg4);

		if ((course = get_course(ship, nr)) == NULL)
		{
			send_to_char("No such station." NL, ch);
			return;
		}

		if (!str_cmp(arg4, "name"))
			STRDUP(course->stop_name, argument);
		else if (!str_cmp(arg4, "vnum"))
			course->stop_vnum = atoi(argument);
	}
	else

	if (!str_cmp(arg2, "delete")) //wywalamy stacj� z listy
	{
		int nr;

		nr = atoi(arg3);

		if ((course = get_course(ship, nr)) == NULL)
		{
			send_to_char("No such station." NL, ch);
			return;
		}

		ship->stops.remove(course);
		free_course(course);

		if (ship->stops.empty())
		{
			ship->curr_stop = NULL;
			send_to_char("Warning! Ship has no station!\n\r", ch);
		}
		else
		{
			ship->curr_stop = ship->stops.front();
			send_to_char("Done." NL "Resetting course" NL, ch);
		}
	}
	else if (!str_cmp(arg2, "save"))
	{
		if (save_ship_course(ship))
			send_to_char("Ok." NL, ch);
		else
			send_to_char("Note Coders about a bug." NL, ch);
	}
	else
	{
		do_setcourse(ch, (char*) "");
		return;
	}
	showcourse(ch, ship);
}

//pokazuje exp w gwiazdkach (pomys� Troga)
char* show_exp_stars(CHAR_DATA *ch, int ability)
{
	int all;
	int curr;
	int difference;
	static char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	if (!IS_NPC(ch))
	{
		all = exp_level(ch->skill_level[ability] + 1)
				- exp_level(ch->skill_level[ability]);
		curr = exp_level(ch->skill_level[ability] + 1)
				- ch->experience[ability];

		difference = 100 - curr * 100 / all;
	}
	else
		difference = -1;

	sprintf(buf, "[%s%s" PLAIN "]",
#if defined(ARMAGEDDON)
    FG_YELLOW,
#else
			ability == COMBAT_ABILITY ? FG_YELLOW :
			ability == PILOTING_ABILITY ? FB_BLUE :
			ability == ENGINEERING_ABILITY ? FB_WHITE :
			ability == HUNTING_ABILITY ? FG_GREEN :
			ability == SMUGGLING_ABILITY ? FG_MAGENTA :
			ability == DIPLOMACY_ABILITY ? FG_RED :
			ability == LEADERSHIP_ABILITY ? FG_BLUE :
			FB_YELLOW,
#endif
			difference < 0 ? ".........." : difference < 10 ? "          " :
			difference < 20 ? "*         " : difference < 30 ? "**        " :
			difference < 40 ? "***       " : difference < 50 ? "****      " :
			difference < 60 ? "*****     " : difference < 70 ? "******    " :
			difference < 80 ? "*******   " : difference < 90 ? "********  " :
			difference < 100 ? "********* " : "**********");
	return buf;

}

void show_tick()
{
	for (auto* d : descriptor_list)
		if (d->connected == CON_PLAYING)
			if (IS_SET(d->character->act, PLR_TICK))
				send_to_char( EOL "Min�a godzina." NL, d->character);

	return;
}

/* LAST CHANNELS -- Originally by Ratm, ale VERY VERY MOCNO MODIFIED
 by Thanos
 Zmiany:
 -zwracanie uwagi na j�zyk m�wi�cego i s�uchacza
 -last zapami�tuje teraz invisa m�wi�cego (wraz z wizinvis)
 -zmieni�em ca�kowicie typ last_bufora - teraz jest to lista struktur
 a nie tak jak doytchczas - tablica string�w
 */

/* sprawdza czy gracz ma szans� zobaczy� autora textu */
bool check_invis(CHAR_DATA *ch, LAST_DATA *last)
{
	if (IS_NPC(ch))
		return false;

	if (!str_cmp(last->teller, ch->przypadki[0]))
		return true;

	if (last->invis_type == INVIS_NONE)
		return true;

	if (last->invis_type < 0) /* wizinvis */
	{
		if (get_trust(ch) < -1 * last->invis_type)
			return false;
		return true;
	}

	if (IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;

	/* invisy graczy */
	if (!IS_AFFECTED(ch, AFF_TRUESIGHT))
	{
		if (IS_AFFECTED(ch, AFF_BLIND))
			return false;

		if ( IS_RACE( ch, "DEFEL" ) && IS_SET(last->invis_type, INVIS_DEFEL))
			return true;

		if ( IS_SET(last->invis_type,
				INVIS_INVIS) && !IS_AFFECTED(ch, AFF_DETECT_INVIS))
			return false;

		if ( IS_SET(last->invis_type,
				INVIS_HIDE) && !IS_AFFECTED(ch, AFF_DETECT_HIDDEN))
			return false;

		if (IS_SET(last->invis_type, INVIS_FORCE))
			return false;
	}

	return true;
}

/* sprawdza jakie j�zyki zna s�uchacz */
LANG_DATA* get_speaking(CHAR_DATA *ch)
{
	return ch->speaking;
}

/* sprawdza jak dobrze s�uchacz zna j�zyk */
int check_lang(CHAR_DATA *ch, LANG_DATA *language)
{
	/* everyone KNOWS common tongue */
	if (!language)
		return 100;

	if (!IS_NPC(ch) && IS_IMMORTAL(ch))
		return 100;

	if ( IS_NPC(ch) && ch->klangs.empty()) /* No langs = knows all for npcs */
		return 100;

	if (!IS_NPC(ch))
	{
		KNOWN_LANG *klang;
		/* Racial languages for PCs */
		if (ch->race->language == language)
			return 100;

		if ((klang = find_klang(ch, language)))
			return klang->learned;

	}

	return 0;
}

/* obrabia stringa last->text je�li s�uchacz nie mia� prawa zrozumie� textu*/
char* convert_lang(LAST_DATA *last, CHAR_DATA *ch)
{
	static char sbuf[MSL];

//    sprintf( sbuf, last->text );
	strcpy(sbuf, last->text);

	if (find_klang(ch, last->language)
			&& str_cmp(last->teller, ch->przypadki[0]))
	{
		int speakswell = check_lang(ch, last->language);

		if (speakswell < 85)
		{
			/*    sprintf( sbuf, */
			strcpy(sbuf,
					translate(speakswell, last->text, last->language->name));
		}
	}

	return sbuf;
}

/*
 * Funkcja wy�wietla co by�o ostatnio m�wione na danym kanale
 */
DEF_DO_FUN( last )
{
	int i;
	int64 channel = 0;
	char buf[ MSL * (MAX_LAST / 20)];
	char buf1[ MIL * 2];

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char("Sk�adnia: last <nazwa_kana�u>" NL, ch);
		return;
	}

	if (!str_prefix(argument, "admintalk") && IS_ADMIN(ch->name)) /*Trog*/
		channel = CHANNEL_ADMINTALK;
	else if (!str_prefix(argument, "wiztalk") && get_trust(ch) >= LEVEL_HERO)
		channel = CHANNEL_IMMTALK;
	else if (!str_prefix(argument, "olctalk")
			&& ( IS_OLCMAN( ch ) || IS_ADMIN(ch->name) || get_trust(ch) > 102))
		channel = CHANNEL_OLCTALK;
	else if (!str_prefix(argument, "chat"))
		channel = CHANNEL_CHAT;
	else if (!str_prefix(argument, "tells"))
		channel = CHANNEL_TELLS;
	else if ((!str_prefix(argument, "clantalk")
			|| !str_prefix(argument, "ctalk")) && ch->pcdata
			&& ch->pcdata->clan)
		channel = CHANNEL_CLAN;
	else if (!str_prefix(argument, "codertalk") && IS_CODER(ch->name)) /*Trog*/
		channel = CHANNEL_CODERTALK;
	else
	{
		send_to_char("Co? Wpisz 'help last'" NL, ch);
		return;
	}

	sprintf(buf, "Ostatnio m�wiono %s:" NL,
			channel == CHANNEL_CHAT ? "na kanale chat" :
			channel == CHANNEL_IMMTALK ? "na wiztalku" :
			channel == CHANNEL_CLAN ? "do twojego klanu" :
			/*by Trog*/channel == CHANNEL_OLCTALK ? "na olctalku" :
			/*by Trog*/channel == CHANNEL_ADMINTALK ? "na admintalku" :
			/*by Trog*/channel == CHANNEL_CODERTALK ?
					"na codertalku" : "do ciebie");

	if (channel == CHANNEL_TELLS)
	{
		if (!ch->pcdata || ch->pcdata->last_tells.empty())
			strcat(buf, "Nic." NL);
		else
		{
			i = 0;
			for (auto* last : ch->pcdata->last_tells)
			{
				if (i >= MAX_LAST_TELL)
					break;
				char *sbuf = convert_lang(last, ch);

				if (*last->text == '\0')
					break;

				sprintf(buf1, "%s %s" PLAIN ": %s" NL, last->time,
						check_invis(ch, last) ?
								does_knows_name(ch, last->teller) ?
										last->teller : "Kto�"
								: "Kto�",

						sbuf);
				strcat(buf, buf1);
				i++;
			}
		}
	}
	else if (channel == CHANNEL_CLAN)
	{
		if (!ch->pcdata || !ch->pcdata->clan
				|| ch->pcdata->clan->ctalk_history.empty())
			strcat(buf, "Nic." NL);
		else
		{
			i = 0;
			for (auto* last : ch->pcdata->clan->ctalk_history)
			{
				if (i >= MAX_LAST_CTALK)
					break;
				if (*last->text == '\0')
					break;

				if (is_name(last->teller,
						ch->pcdata->ignorelist) && !IS_ADMIN( ch->name ))
					continue;

				sprintf(buf1, "%s %s" PLAIN ": %s" NL, last->time,
						check_invis(ch, last) ?
								does_knows_name(ch, last->teller) ?
										last->teller : "Kto�"
								: "Kto�", last->text);
				strcat(buf, buf1);
				i++;
			}
		}
	}
	else
	{
		auto& the_list =
				(channel == CHANNEL_ADMINTALK) ? last_admin_list :
				(channel == CHANNEL_OLCTALK) ? last_olctalk_list :
				(channel == CHANNEL_CODERTALK) ? last_codertalk_list :
				(channel == CHANNEL_IMMTALK) ?
						last_wiztalk_list : last_chat_list;
		i = 0;
		for (auto* last : the_list)
		{
			if (i >= MAX_LAST)
				break;
			char *sbuf =
					(channel == CHANNEL_CHAT) ?
							convert_lang(last, ch) : last->text;

			if (*last->text == '\0')
				break;

			if (is_name(last->teller,
					ch->pcdata->ignorelist) && !IS_ADMIN( ch->name ))
				continue;

			if (channel == CHANNEL_CHAT)
				sprintf(buf1, "%s %s %s '%s" PLAIN "'" NL, last->time,
						check_invis(ch, last) ?
								does_knows_name(ch, last->teller) ?
										last->teller : "Kto�"
								: "Kto�", last->verb, sbuf);
			else
				sprintf(buf1, "%s %s: %s" EOL, last->time,
						check_invis(ch, last) ?
								does_knows_name(ch, last->teller) ?
										last->teller : "Kto�"
								: "Kto�", sbuf);
			strcat(buf, buf1);
			i++;
		}

	}
	send_to_pager(buf, ch);
	return;
}

/* DODAJEMY DO LAST_BUFOW */

/* ustalamy z jakim invisem jest dany last */
void set_last_invis(LAST_DATA *last, CHAR_DATA *sender)
{
	if (!IS_NPC(sender) && IS_SET(sender->act, PLR_WIZINVIS))
		last->invis_type = sender->pcdata->wizinvis * -1;
	else if ( IS_NPC(sender) && IS_SET(sender->act, ACT_MOBINVIS))
		last->invis_type = sender->mobinvis * -1;
	else
	{
		last->invis_type = INVIS_NONE;

		if (IS_AFFECTED(sender, AFF_INVISIBLE))
			SET_BIT(last->invis_type, INVIS_INVIS);

		if (IS_AFFECTED(sender, AFF_FORCEINVIS))
			SET_BIT(last->invis_type, INVIS_FORCE);

		if (IS_AFFECTED(sender, AFF_HIDE))
			SET_BIT(last->invis_type, INVIS_HIDE);

		if (IS_RACE(sender, "DEFEL"))
			SET_BIT(last->invis_type, INVIS_DEFEL);
	}
	return;
}

/* ustalamy w jakim j�zyku jest dany last */
LANG_DATA* set_last_lang(CHAR_DATA *ch)
{
	if (IS_NPC(ch) && ch->klangs.empty())
		return NULL;

	return ch->speaking;
}

void add_to_last_tell(CHAR_DATA *ch, CHAR_DATA *sender, const char *linia)
{
	LAST_DATA *last;

	if (IS_NPC(ch))
	{
		bug("mob's getting a tell");
		return;
	}

	if ((int)ch->pcdata->last_tells.size() >= MAX_LAST_TELL)
	{
		last = ch->pcdata->last_tells.front();
		ch->pcdata->last_tells.pop_front();
		free_last(last);
	}

	CREATE(last, LAST_DATA, 1);
	set_last_invis(last, sender);
	STRDUP(last->time, static_cast<SWString>(SWTime(current_time)).c_str());
	STRDUP(last->text, linia);
	STRDUP(last->verb, "");
	STRDUP(last->teller, sender->przypadki[0]);
	last->language = set_last_lang(sender);

	ch->pcdata->last_tells.push_back(last);
	return;
}

void add_to_last_ctalk(CLAN_DATA *clan, CHAR_DATA *sender, const char *linia)
{
	LAST_DATA *last;

	if ((int)clan->ctalk_history.size() >= MAX_LAST_CTALK)
	{
		last = clan->ctalk_history.front();
		clan->ctalk_history.pop_front();
		free_last(last);
	}

	CREATE(last, LAST_DATA, 1);
	set_last_invis(last, sender);
	STRDUP(last->time, static_cast<SWString>(SWTime(current_time)).c_str());
	STRDUP(last->text, linia);
	STRDUP(last->verb, "");
	STRDUP(last->teller, NAME(sender,0));
	last->language = set_last_lang(sender);

	clan->ctalk_history.push_back(last);
	return;
}

/*
 * Funkcja dopisuje tekst do bufora przechowuj�cego tekst z kana�u
 */
void add_to_last_buf(CHAR_DATA *ch, char *tekst, const char *verb,
		int64 channel)
{
	LAST_DATA *last;

	auto& the_buf_list =
			(channel == CHANNEL_ADMINTALK) ? last_admin_list :
			(channel == CHANNEL_OLCTALK) ? last_olctalk_list :
			(channel == CHANNEL_CODERTALK) ? last_codertalk_list :
			(channel == CHANNEL_IMMTALK) ? last_wiztalk_list : last_chat_list;

	if ((int)the_buf_list.size() >= MAX_LAST)
	{
		last = the_buf_list.front();
		the_buf_list.pop_front();
		free_last(last);
	}

	SWString buf(tekst);
	buf << PLAIN;

	CREATE(last, LAST_DATA, 1);
	set_last_invis(last, ch);
	STRDUP(last->time, static_cast<SWString>(SWTime(current_time)).c_str());
	STRDUP(last->text, buf.c_str());

	if (channel == CHANNEL_CHAT)
		STRDUP(last->teller, NAME(ch,0));
	else
		STRDUP(last->teller, ch->przypadki[0]);

	STRDUP(last->verb, (char* )verb);
	last->language = set_last_lang(ch);

	switch (channel)
	{
	case CHANNEL_IMMTALK:
		last_wiztalk_list.push_back(last);
		break;
	case CHANNEL_ADMINTALK:
		last_admin_list.push_back(last);
		break;
	case CHANNEL_CODERTALK:
		last_codertalk_list.push_back(last);
		break;
	case CHANNEL_OLCTALK:
		last_olctalk_list.push_back(last);
		break;
	case CHANNEL_CHAT:
	case CHANNEL_OOC:
		last_chat_list.push_back(last);
		break;
	default:
		bug("unknown channel: %d.", channel);
		return;
	}
	return;
}

DEF_DO_FUN( prefi )
{
	send_to_char(
			"Je�li chcesz u�y� prefixu, wpisz komend� 'prefix' w ca�o�ci." NL,
			ch);
	return;
}

DEF_DO_FUN( prefix )
{
	if (!ch->pcdata)
	{
		send_to_char("Eee tam prefixy.. obejdzie si�.." NL, ch);
		return;
	}

	if (!*argument)
	{
		if (!*ch->pcdata->prefix)
		{
			send_to_char("Nie u�ywasz obecnie �adnego prefixu." NL, ch);
			return;
		}
		else
		{
			STRDUP(ch->pcdata->prefix, "");
			send_to_char("Tw�j prefix zosta� usuni�ty." NL, ch);
			return;
		}
	}

	STRDUP(ch->pcdata->prefix, argument);

	ch_printf(ch, "Tw�j prefix zosta� ustawiony jako '%s" RESET PLAIN "'." NL,
			argument);
	return;
}

//Aliasy:
/*
 * "alia" & "unalia" commands are a trap to the "(un)alias" commands.
 */
DEF_DO_FUN( alia )
{
	send_to_char("Je�li chcesz zrobi� ALIAS, napisz to w ca�o�ci." NL, ch);
	return;
}

DEF_DO_FUN( unalia )
{
	send_to_char("Jesli chcesz wykona� UNALIAS, napisz to w ca�o�ci." NL, ch);
	return;
}

DEF_DO_FUN( alias )
{
	CHAR_DATA *rch;
	ALIAS_DATA *alias;
	int pos;
	char arg[ MAX_INPUT_LENGTH];

	smash_tilde(argument);

	rch = (!ch->pcdata ? ch->desc->original : ch);

	if (IS_NPC(rch))
		return;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		if (rch->pcdata->aliases.empty())
		{
			send_to_char("Nie masz zdefiniowanych �adnych alias�w." NL, ch);
			return;
		}
		send_to_pager("Masz zdefiniowane nast�puj�ce aliasy:" NL, ch);

		for (auto* alias : rch->pcdata->aliases)
		{
			pager_printf(rch,
					"Alias '" FB_WHITE "%s" PLAIN "' dzia�a jako: '" FG_CYAN "%s" PLAIN "'" NL,
					alias->name, alias->sub);
		}

		return;
	}

	if (!str_prefix("unalias", arg) || !str_cmp("alias", arg))
	{
		send_to_char("Przykro mi, ale to s�owo jest zarezerwowane." NL, ch);
		return;
	}

	if (argument[0] == '\0')
	{
		for (auto* alias : rch->pcdata->aliases)
		{
			if (!str_cmp(arg, alias->name))
			{
				ch_printf(rch,
				FB_WHITE "%s" PLAIN " dzia�a jako " FG_CYAN "%s" PLAIN "." NL,
						alias->name, alias->sub);
				return;
			}
		}

		send_to_char("Nie masz zdefiniowanego takiego aliasa." NL, ch);
		return;
	}

	if (!str_prefix(argument, "delete") || !str_prefix(argument, "suicide")
			|| !str_prefix(argument, "quit"))
	{
		send_to_char("To nie mo�e by� wykonane!" NL, ch);
		return;
	}

	pos = 0;
	for (auto* alias : rch->pcdata->aliases)
	{
		pos++;
		if (!str_cmp(arg, alias->name)) /* redefine an alias */
		{
			STRDUP(alias->sub, argument);
			ch_printf(rch,
					FB_WHITE "%s" PLAIN " zosta�o przedefiniowane na: " FG_CYAN "%s" PLAIN "." NL,
					arg, argument);
			return;
		}
	}

	if (pos >= MAX_ALIAS && !IS_IMMORTAL(ch))
	{
		send_to_char(
				"Przykro mi, ale tw�j limit alias�w zosta� przekroczny." NL,
				ch);
		return;
	}

	/* make a new alias */
	CREATE(alias, ALIAS_DATA, 1);
	STRDUP(alias->name, arg);
	STRDUP(alias->sub, argument);
	ch->pcdata->aliases.push_back(alias);

	ch_printf(rch, FB_WHITE
	"%s" PLAIN " dzia�a od teraz jako: " FG_CYAN "%s" PLAIN "." NL, arg,
			argument);
	return;
}

DEF_DO_FUN( unalias )
{
	CHAR_DATA *rch;
	char arg[ MAX_INPUT_LENGTH];

	rch = (!ch->pcdata ? ch->desc->original : ch);

	if (IS_NPC(rch))
		return;

	argument = one_argument(argument, arg);

	if (*arg == '\0')
	{
		send_to_char("Sk�adnia: unalias <nazwa_aliasa>." NL, ch);
		return;
	}

	for (auto* alias : rch->pcdata->aliases)
	{
		if (!str_cmp(arg, alias->name))
		{
			rch->pcdata->aliases.remove(alias);
			free_alias(alias);
			send_to_char("Alias usuni�ty." NL, ch);
			return;
		}
	}

	send_to_char("Nie masz aliasa o takiej nazwie." NL, ch);

	return;
}

//Forbid: do zabierania komend graczom (by Thanos)
DEF_DO_FUN( forbid )
{
	CHAR_DATA *victim;
	CHAR_DATA *rch;
	char arg1[ MAX_INPUT_LENGTH];
	char arg2[ MAX_INPUT_LENGTH];
	char buf[ MAX_STRING_LENGTH];
	CMDTYPE *cmd;
	char hash;
	bool found = false;

	rch = (!ch->pcdata ? ch->desc->original : ch);

	smash_tilde(argument);

	if (!argument || argument[0] == '\0')
	{
		send_to_char("Sk�adnia: forbid <ofiara> [komenda]\n\r"
				"Lub:      forbid <ofiara> clear\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg1);

	if ((victim = get_char_world(rch, arg1)) == NULL)
	{
		send_to_char("Nie ma nikogo takiego.\n\r", ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Nie mo�esz odebra� komendy mobowi.\n\r", ch);
		return;
	}

	if ( IS_ADMIN( victim->name ) && !IS_ADMIN(ch->name))
	{
		send_to_char("ZAPOMNIJ !!!\n\r", ch);
		return;
	}

	argument = one_argument(argument, arg2);

	if (!*arg2)
	{
		ch_printf(ch, "Komendy zabrane graczowi %s: %s\n\r", victim->name,
				victim->pcdata->forbidden_cmd[0] != '\0' ?
						victim->pcdata->forbidden_cmd : "(brak)");
		return;
	}

	if (ch == victim && !IS_ADMIN(ch->name))
	{
		send_to_char("Nie mo�esz dodawa� ani zabiera� komend sobie.\n\r", ch);
		return;
	}

	if (get_trust(rch) <= get_trust(victim) && rch != victim)
	{
		send_to_char(
				"Nie mo�esz odebra� komendy w�adcom lub graczom r�wnym rang�.\n\r",
				ch);
		return;
	}

	/* usuwamy wszystkie komendy z listy */
	if (!str_cmp(arg2, "clear"))
	{
		if (victim->pcdata->forbidden_cmd)
		{
			send_to_char(
					"Mo�esz zn�w pos�ugiwa� si� odebranymi ci wcze�niej komendami.\n\rU�ywaj ich z rozwag�.\n\r",
					victim);
			STRDUP(victim->pcdata->forbidden_cmd, "");
		}
		ch_printf(ch, "Ok.\n\r");
		return;
	}

	/* szukamy komendy */
	hash = arg2[0];
	for (cmd = command_hash[(int) hash]; cmd; cmd = cmd->next)
	{
		if (!str_cmp(arg2, cmd->name))
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		send_to_char("Nie ma takiej komendy.\n\r", ch);
		return;
	}

	if (!IS_ADMIN(ch->name) && (cmd->level > get_trust(ch)))
	{
		send_to_char("Tej komendy nie mo�esz odebra�!\n\r", ch);
		return;
	}

	/* usuwamy jedn� komend� z listy */
	if (victim->pcdata->forbidden_cmd
			&& is_name(skill_name(cmd->do_fun), victim->pcdata->forbidden_cmd))
	{
		strcpy(buf, victim->pcdata->forbidden_cmd);
		STRDUP(victim->pcdata->forbidden_cmd,
				cut_from_string(buf, skill_name(cmd->do_fun)));
		ch_printf(ch, "Komenda '%s' (%s) zwr�cona.\n\r", cmd->name,
				skill_name(cmd->do_fun));
		ch_printf(victim, "Komenda '%s' zosta�a ci przywr�cona.\n\r"
				"Nast�pnym razem u�ywaj jej z rozwag�.\n\r", cmd->name);
		save_char_obj(victim);
		do_forbid(ch, victim->name);
	}
	/* dodajemy jedn� komend� do listy */
	else
	{
		if (*victim->pcdata->forbidden_cmd)
		{
			strcpy(buf, victim->pcdata->forbidden_cmd);
			strcat(buf, " ");
		}
		else
			buf[0] = '\0';

		strcat(buf, skill_name(cmd->do_fun));
		STRDUP(victim->pcdata->forbidden_cmd, buf);
		ch_printf(ch, "Komenda '%s' (%s) odebrana.\n\r", cmd->name,
				skill_name(cmd->do_fun));
//        ch_printf( victim, "Zosta�a ci odebrana komenda '%s'.\n\r"
//	                   "W razie pretensji zg�o� si� do administrator�w muda.\n\r",
//			   cmd->name );
		save_char_obj(victim);
		do_forbid(ch, victim->name);
	}
	return;
}

//Linia statusu (ofcoz te� Ratma ;)
void update_status_bar(CHAR_DATA *ch)
{
	char bufor[MAX_STATUS_WIDTH + 1];
	char buf[120];
	char *msg;

	int st_width;
	int st_height;
	int i = 0;

	st_width = ch->pcdata->status_width;
	st_height = ch->pcdata->status_height;

	//added by Thanos(typy statusow)
	if (ch->pcdata->status_type == STATUS_TYPE_ONE)
	{
		st_width += 0; //+30 (bo tyle znakow zajmuja oznaczenia kolorkow)

		while (i < st_width)
			bufor[i++] = '-';

		bufor[i] = '\0';

		if (ch->skill_level[FORCE_ABILITY]
				> 1|| get_trust(ch) >= LEVEL_IMMORTAL)
			sprintf(buf,
					"&W[&O %d&c/&O%d&cHp &O%d&c/&O%d&cFrc &O%d&c/&O%d&cMv &W]&c",
					ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
					ch->max_move);
		else
			sprintf(buf, "&W&W&W&W&W[&O %d&c/&O%d &chp  &O%d&c/&O%d&c mv &W]&c",
					ch->hit, ch->max_hit, ch->move, ch->max_move);

		i = (st_width - strlen(buf)) / 2;
		strcpy(&bufor[i], buf);
		bufor[i + strlen(buf)] = '-';
//        send_to_char( "&c", ch);
		ch_printf(ch, "\e[%d;1H\e[2K%s\e[%d;1H", st_height, bufor,
				st_height - 1);
	}
	else if (ch->pcdata->status_type == STATUS_TYPE_TWO)
	{
		st_width += 30;

		while (i < 16)
			bufor[i++] = '-';
		bufor[i++] = '.';
		while (i < st_width - 79)
			bufor[i++] = ' ';
		bufor[i++] = '.';
		while (i < st_width)
			bufor[i++] = '-';

		bufor[i] = '\0';

		if (ch->skill_level[FORCE_ABILITY]
				> 1|| get_trust(ch) >= LEVEL_IMMORTAL)
			sprintf(buf,
					"-&W[&O %d&c/&O%d&cHp &O%d&c/&O%d&cFrc &O%d&c/&O%d&cMv &W]&c-",
					ch->hit, ch->max_hit, ch->mana, ch->max_mana, ch->move,
					ch->max_move);
		else
			sprintf(buf,
					"-&W&W&W&W&W[&O %d&c/&O%d &chp  &O%d&c/&O%d&c mv &W]&c-",
					ch->hit, ch->max_hit, ch->move, ch->max_move);

		i = (st_width - (strlen(buf)));
		strcpy(&bufor[i], buf);
		send_to_char("&c", ch);
		ch_printf(ch, "\e[%d;1H%s\e[%d;1H", st_height, bufor, st_height - 1);
	}
	else if (ch->pcdata->status_type == STATUS_TYPE_THREE)
	{
		int a;

		st_width += 8;

		while (i < st_width)
			bufor[i++] = '-';

		bufor[i] = '\0';

		sprintf(buf, "&W[&Y * S t a r - W a r s * &W]&c");

		i = (st_width - strlen(buf)) / 2;
		strcpy(&bufor[i], buf);
		bufor[i + strlen(buf)] = '-';

		msg = ctime(&current_time);
		for (a = 0; a < 5; a++)
		{
			msg[a] = msg[11 + a];
		}
		msg[5] = '\0';

		sprintf(buf, "&W[ &O%s&W ]&c-", msg);

		i = (st_width - 10);
		strcpy(&bufor[i], buf);
		send_to_char("&c", ch);
		ch_printf(ch, "\e[%d;1H%s\e[%d;1H", st_height, bufor, st_height - 1);

	}
	else
	{
		bug("Unknown StBar type");
		ch->pcdata->status_type = STATUS_TYPE;
	}

	sprintf(buf, "\e[%d;1H\e[J", ch->pcdata->status_height + 1);
	write_to_buffer(ch->desc, buf, 0);

//    send_to_char( "&w", ch);
	return;
}

void init_status_bar(CHAR_DATA *ch)
{
	int st_height = ch->pcdata->status_height;

	//added by Thanos:
	if (ch->pcdata->status_type == STATUS_TYPE_NONE)
		ch->pcdata->status_type = STATUS_TYPE;
	//done

	if ((ch->pcdata->status_width < 40)
			|| (ch->pcdata->status_width > MAX_STATUS_WIDTH))
		ch->pcdata->status_width = STATUS_WIDTH;

	if ((ch->pcdata->status_height < 15)
			|| (ch->pcdata->status_height > MAX_STATUS_HEIGHT))
		st_height = ch->pcdata->status_height = STATUS_HEIGHT;

	ch_printf(ch, "\e[%d;1H%s\e[1;1H%s%s\e[1;%dr%s", st_height + 1,
	VT_SAVECURSOR, VT_SETWIN_CLEAR, VT_CLEAR_SCREEN, st_height - 1,
			VT_RESTORECURSOR);

	update_status_bar(ch);

	SET_BIT(ch->act, PLR_STATUS_BAR);

	return;
}

DEF_DO_FUN( stbar )
{
	char buf[MAX_INPUT_LENGTH];
	char msg[MAX_INPUT_LENGTH];

	if (IS_NPC(ch))
		return;

	if (*argument == '\0')
	{
		send_to_char("Sk�adnia: stbar on/off\n\r", ch);
		return;
	}

	argument = one_argument(argument, buf);

	if (!str_cmp("on", buf))
	{
		init_status_bar(ch);
		send_to_char("\n\rLinia statusu w��czona\n\r", ch);
		return;
	}

	if (!str_cmp("off", buf))
	{
		ch->pcdata->status_type = STATUS_TYPE_NONE; //added by Thanos
		REMOVE_BIT(ch->act, PLR_STATUS_BAR);
		ch_printf(ch, "\e[r%s", VT_CLEAR_SCREEN);
		send_to_char("Linia statusu wy��czona\n\r", ch);
		return;
	}
	if (!str_prefix(buf, "width"))
	{
		int i;
		i = atoi(argument);

		if ((i < 40) || (i > MAX_STATUS_WIDTH))
		{
			send_to_char(
					"Szeroko�� linii statusu musi by� z przedzia�u: 40 - 160.\n\r",
					ch);
			return;
		}
		else
		{
			ch->pcdata->status_width = i;
			init_status_bar(ch);
			send_to_char("Szeroko�� linii statusu ustawiona.\n\r", ch);
			return;
		}

		return;
	}
	if (!str_prefix(buf, "height"))
	{
		int i;
		i = atoi(argument);

		if ((i < 15) || (i > MAX_STATUS_HEIGHT))
		{
			send_to_char(
					"Wysoko�� linii statusu musi by� z przedzia�u: 15 - 60.\n\r",
					ch);
			return;
		}
		else
		{
			ch->pcdata->status_height = i;
			if (ch->pcdata->status_type == STATUS_TYPE_TWO)
			{
				ch->pcdata->status_height -= 1;
			}

			init_status_bar(ch);
			send_to_char("Wysoko�� linii statusu ustawiona.\n\r", ch);
			return;
		}
		return;
	}
	else if (!str_prefix(buf, "type"))	//added by thanos
	{
		int i;
		i = atoi(argument);

		if (i >= STATUS_TYPE_ONE && i <= MAX_STATUS_TYPES)
			ch->pcdata->status_type = i;
		else
			ch->pcdata->status_type = STATUS_TYPE_ONE;

		if (ch->pcdata->status_type == STATUS_TYPE_TWO)
		{
			if (ch->pcdata->status_width < 60)
				ch->pcdata->status_width = 60;
		}

		init_status_bar(ch);

		sprintf(msg, "Obecny typ linii statusu: %d\n\r",
				ch->pcdata->status_type);
		send_to_char(msg, ch);
		return;
	}					//done by thanos
	send_to_char("Co? Wpisz: help stbar\n\r", ch);
}

/* Funkcja zamienia wszystkie ma�e litery wyra�enia na wielkie */
/* potrzebna by�a mi do do_wizinfo, ale mo�e jeszcze si� przyda*/
char* all_capitalize(const char *str)
{
	static char strcap[ MAX_STRING_LENGTH];
	int i;

	for (i = 0; str[i] != '\0'; i++)
		strcap[i] = UPPER(str[i]);
	strcap[i] = '\0';
	return strcap;
}

/* Funkcje Ratma do zmiany znak�w CP/ISO/NOPOL */
bool ispolchar(const char letter)
{
	int i;
	const char polchars[22] =
	{ '\xb1', '\xe6', '\xea', '\xb3', '\xf1', '\xf3', '\xb6', '\xbc', '\xbf',
	  '\xa1', '\xc6', '\xca', '\xa3', '\xd1', '\xd3', '\xa6', '\xac', '\xaf',
	  '\xb9', '\xa5', '\xbf', '\xaf' };

	if (letter >= 'A' && letter <= 'z')
		return false;

	for (i = 0; i < 22; i++)
	{
		if (letter == polchars[i])
			return true;
	}
	return false;
}

/*
 * Funkcja zamienia polskie znaki z "ogonkami" z du�ych na ma�e i na odwr�t
 */
char shiftpl(bool shift, char letter)
{
	int i;
	const char uchars[9] =
	{ '\xa1', '\xc6', '\xca', '\xa3', '\xd1', '\xd3', '\xa6', '\xac', '\xaf' };
	const char lchars[9] =
	{ '\xb1', '\xe6', '\xea', '\xb3', '\xf1', '\xf3', '\xb6', '\xbc', '\xbf' };

	if (shift) //zamie� z ma�ych na du�e
	{
		for (i = 0; i < 9; i++)
			if (letter == lchars[i])
				return (uchars[i]);

		return letter;
	}
	else
	{
		for (i = 0; i < 9; i++)
			if (letter == uchars[i])
				return (lchars[i]);

		return letter;
	}
}

DEF_DO_FUN( iso )
{
	if (IS_NPC(ch))
		return;

	ch->charset = CHARSET_ISO;
	send_to_char(
			"Standard kodowania polskich liter zosta� ustawiony jako: ISO-8859-2\n\r",
			ch);
	send_to_char(
			"Test polskich liter: -�-�-�-�-�-�-�-�-�- -�-�-�-�-�-�-�-�-�-\n\r",
			ch);
	return;
}

DEF_DO_FUN( win )
{
	if (IS_NPC(ch))
		return;
	ch->charset = CHARSET_WIN;
	send_to_char(
			"Standard kodowania polskich liter zosta� ustawiony jako: WIN CP-1250\n\r",
			ch);
	send_to_char(
			"Test polskich liter: -�-�-�-�-�-�-�-�-�- -�-�-�-�-�-�-�-�-�-\n\r",
			ch);
	return;
}

DEF_DO_FUN( nopol )
{
	if (IS_NPC(ch))
		return;
	ch->charset = CHARSET_NOPOL;
	send_to_char(
			"Standard kodowania polskich liter zosta� ustawiony jako: NOPOL\n\r",
			ch);
	send_to_char(
			"Test polskich liter: -�-�-�-�-�-�-�-�-�- -�-�-�-�-�-�-�-�-�-\n\r",
			ch);
	return;
}

//wsio

