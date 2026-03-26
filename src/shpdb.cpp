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
 *			    Ship database module			   *
 ****************************************************************************/

#include <sys/types.h>
#include "mud.h"

void boot_log args( ( const char *str, ... ) );
SHIP_INDEX_DATA* get_ship_index args( ( char * ship_index ) );

AREA_DATA *shipTmpArea;

SHIP_ROOM_DATA* get_sroom_ind(SHIP_INDEX_DATA *ship, int vnum)
{
	for (auto* room : ship->rooms)
		if (room->vnum == vnum)
			return room;

	return NULL;
}

DEF_DO_FUN( shlist )
{
	char buf[MSL];
	char buf1[MSL];

	sprintf(buf1,
	FB_CYAN "[" FB_YELLOW "Vnum" FB_CYAN "]"
	"[" FB_YELLOW "Ship Name               " FB_CYAN "]"
	"[" FB_YELLOW "Rooms" FB_CYAN "]"
	"[" FB_YELLOW "Sec" FB_CYAN "]"
	"[" FB_YELLOW "Builders        " FB_CYAN "]"
	"[" FB_YELLOW "Filename        " FB_CYAN "]" EOL);

	for (auto* shrec : ship_index_list)
	{
		sprintf(buf,
		FB_CYAN "[" PLAIN "%4d" FB_CYAN "][" PLAIN "%-24s" FB_CYAN "]"
		"[" PLAIN "%5d" FB_CYAN "][" PLAIN "%3d" FB_CYAN "]", shrec->vnum,
				strip_colors(shrec->name, 24), shrec->roomcount,
				shrec->security);
		strcat(buf1, buf);
		sprintf(buf, "[" PLAIN "%-16s" FB_CYAN "]",
				strip_colors(shrec->builders, 16));
		strcat(buf1, buf);
		sprintf(buf, "[" PLAIN "%-16s" FB_CYAN "]" EOL,
				strip_colors(shrec->filename, 16));
		strcat(buf1, buf);
	}

	pager_printf(ch, buf1);
	return;
}

/*
 znajduje wolnych <ammount> vnumow w krainie <area> lub zwraca 0 jesli
 nie znaleziono. W przeciwnym wypadku zwraca vnum poczatkowy od ktorego
 mozemy zaczac budowac					-- by Aldegard
 */
int search_free_vnums(AREA_DATA *area, int ammount)
{
	int first, vnum, j;

	if (!area || !area->lvnum || !area->uvnum)
	{
		bug("no (or bad) area");
		return 0;
	}

	if (ammount < 1)
	{
		bug("bad ammount, must be > 0");
		return 0;
	}

	for (first = vnum = area->lvnum, j = 0; vnum <= area->uvnum && j <= ammount;
			vnum++, j++)
	{
		if (!first)
			first = vnum;
		if (get_room_index(vnum))
		{
			if (vnum == area->uvnum)
				return 0;
			else
			{
				j = 0;
				first = 0;
			}
		}
	}

	if (j != ammount + 1)
		return 0;
	else
		return first;
}

/* bleee.. to zzynka z rprog_read_programs, niestety niezgodnosci typow..*/
void ship_rprog_read_programs(FILE *fp, SHIP_ROOM_DATA *sRoom)
{
	MPROG_DATA *mprg;
	char letter;
	bool done = false;

	if ((letter = fread_letter(fp)) != '>')
	{
		bug("vnum %d ROOMPROG char", sRoom->vnum);
		exit(1);
	}
	CREATE(mprg, MPROG_DATA, 1);
	sRoom->mudprogs.push_back(mprg);

	while (!done)
	{
		mprg->type = mprog_name_to_type(fread_word(fp));
		switch (mprg->type)
		{
		case ERROR_PROG:
			bug("vnum %d ROOMPROG type.", sRoom->vnum);
			exit(1);
			break;
			/* IN_FILE_PROGi sobie odpuszczam -- szkoda zachodu
			 case IN_FILE_PROG:
			 break;
			 */
		default:
			sRoom->progtypes = sRoom->progtypes | mprg->type;
			mprg->arglist = fread_string(fp);
			fread_to_eol(fp);
			mprg->comlist = fread_string(fp);
			fread_to_eol(fp);
			switch (letter = fread_letter(fp))
			{
			case '>':
				CREATE(mprg, MPROG_DATA, 1);
				sRoom->mudprogs.push_back(mprg);
				break;
			case '|':
				fread_to_eol(fp);
				done = true;
				break;
			default:
				bug("vnum %d bad ROOMPROG.", sRoom->vnum);
				exit(1);
				break;
			}
			break;
		}
	}

	return;
}

/* czyta wszystkie lokacje pojedynczego statku 
 (wlacznie z extraopisami, progami i exitami ) */
void fread_ship_rooms(SHIP_INDEX_DATA *shrec, FILE *fp)
{
	for (;;)
	{
		int vnum;
		char letter;
		int door;
		char *word;
		char *ln;
		int x1, x2, x3, x4, x5, x6;
		SHIP_ROOM_DATA *sRoom;

		letter = fread_letter(fp);
		if (letter != '#')
		{
			bug("# not found.");
			return;
		}

		word = fread_word(fp);
		if (!str_cmp(word, "End"))
			return;

		vnum = atoi(word);

		CREATE(sRoom, SHIP_ROOM_DATA, 1);
		top_sroom++;
		sRoom->vnum = vnum;
		sRoom->name = fread_string(fp);
		sRoom->description = fread_string(fp);

		ln = fread_line(fp);
		x1 = x2 = x3 = x4 = x5 = x6 = 0;
		sscanf(ln, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6);

		sRoom->room_flags = x2;
		sRoom->sector_type = x3;
		sRoom->tele_delay = x4;
		sRoom->tele_vnum = x5;
		sRoom->tunnel = x6;
		sRoom->light = x1;
		sRoom->exits.clear();

		for (;;)
		{
			letter = fread_letter(fp);

			if (letter == 'S')
				break;

			if (letter == 'D')
			{
				SHIP_EXIT_DATA *pexit;
				int locks;

				door = fread_number(fp);
				if (door < 0 || door > 10)
				{
					bug("vnum %d has bad door number %d.", vnum, door);
					continue;
				}
				else
				{
					CREATE(pexit, SHIP_EXIT_DATA, 1);
					pexit->vdir = door;
					pexit->description = fread_string(fp);
					pexit->keyword = fread_string(fp);
					pexit->flags = 0;
					pexit->rvnum = sRoom->vnum;

					ln = fread_line(fp);
					x1 = x2 = x3 = x4 = 0;
					sscanf(ln, "%d %d %d %d", &x1, &x2, &x3, &x4);

					locks = x1;
					pexit->key = x2;
					pexit->vnum = x3;
					pexit->vdir = door;
					pexit->distance = x4;

					switch (locks)
					{
					case 1:
						pexit->flags = EX_ISDOOR;
						break;
					case 2:
						pexit->flags = EX_ISDOOR | EX_PICKPROOF;
						break;
					default:
						pexit->flags = locks;
					}

					sRoom->exits.push_back(pexit);
				}
			}
			else if (letter == 'E')
			{
				EXTRA_DESCR_DATA *ed;

				CREATE(ed, EXTRA_DESCR_DATA, 1);
				ed->keyword = fread_string(fp);
				ed->description = fread_string(fp);
				sRoom->extradesc.push_back(ed);
			}
			else if (letter == '>')
			{
				ungetc(letter, fp);
				ship_rprog_read_programs(fp, sRoom);
			}
			else
			{
				bug("vnum %d has flag '%c' not 'DES'.", vnum, letter);
				return;
			}
		}
		shrec->rooms.push_back(sRoom);
		shrec->roomcount++;
	}
	return;
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = true;			\
				    break;				\
				}

/* odczytuje dane statku i wrzuca do struktury rekordu w bazie danych */
void fread_ship_data(SHIP_INDEX_DATA *ship, FILE *fp)
{
	const char *word;
	bool fMatch;

	for (;;)
	{
		word = feof(fp) ? "End" : fread_word(fp);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;

		case 'A':
			KEY("Astro_array", ship->astro_array, fread_number(fp))
			;
			break;

		case 'B':
			SKEY("Builders", ship->builders, fread_string(fp))
			;
			break;

		case 'C':
			KEY("Cockpit", ship->cockpit, fread_number(fp))
			;
			KEY("Coseat", ship->coseat, fread_number(fp))
			;
			KEY("Class", ship->clazz, fread_number(fp))
			;
			KEY("Comm", ship->comm, fread_number(fp))
			;
			break;

		case 'D':
			SKEY("Description", ship->description, fread_string(fp))
			;
			if (!str_cmp(word, "Dock"))
			{
				SHIPDOCK_DATA *dock;

				CREATE(dock, SHIPDOCK_DATA, 1);
				dock->type = fread_number(fp);
				dock->vnum = fread_number(fp);
				dock->target = NULL;
				dock->targetvnum = 0;
				dock->master_slave = -1;
				ship->docks.push_back(dock);

				fMatch = true;
			}
			break;

		case 'E':
			KEY("Engineroom", ship->engineroom, fread_number(fp))
			;
			KEY("Entrance", ship->entrance, fread_number(fp))
			;
			if (!str_cmp(word, "End"))
			{
				if (!ship->name)
					STRDUP(ship->name, "");
				if (!ship->description)
					STRDUP(ship->description, "");
				if (ship->shipyard <= 0)
					ship->shipyard = ROOM_LIMBO_SHIPYARD;
				if (ship->navseat <= 0)
					ship->navseat = ship->cockpit;
				if (ship->gunseat <= 0)
					ship->gunseat = ship->cockpit;
				if (ship->coseat <= 0)
					ship->coseat = ship->cockpit;
				if (ship->pilotseat <= 0)
					ship->pilotseat = ship->cockpit;
				for (auto* module : ship->modules)
				{
					module->timer = -1;
					if (!module->spyname)
						STRDUP(module->spyname, "");
				}
				return;
			}
			break;

		case 'G':
			KEY("Gunseat", ship->gunseat, fread_number(fp))
			;
			break;

		case 'H':
			KEY("Hyperspeed", ship->hyperspeed, atof(fread_word(fp)))
			;
			if (!str_cmp(word, "Hangar"))
			{
				HANGAR_DATA *hangar;

				CREATE(hangar, HANGAR_DATA, 1);
				hangar->vnum = fread_number(fp);
				hangar->capacity = atof(fread_word(fp));
				hangar->status = fread_number(fp);
				hangar->type = fread_number(fp);
				ship->hangars.push_back(hangar);

				fMatch = true;
			}
			break;

		case 'I':
			KEY("Ioncannons", ship->ioncannons, fread_number(fp))
			;
			break;

		case 'L':
			KEY("Laserstr", ship->lasers, (int )(fread_number(fp) / 10))
			;
			KEY("Lasers", ship->lasers, fread_number(fp))
			;
			KEY("Level", ship->level, fread_number(fp))
			;
			break;

		case 'M':
			KEY("Manuever", ship->manuever, fread_number(fp))
			;
			KEY("Maxmissiles", ship->maxmissiles, fread_number(fp))
			;
			KEY("Maxtorpedos", ship->maxtorpedos, fread_number(fp))
			;
			KEY("Maxrockets", ship->maxrockets, fread_number(fp))
			;
			KEY("Missiletype", ship->missiletype, fread_number(fp))
			;
			KEY("Maxshield", ship->maxshield, fread_number(fp))
			;
			KEY("Maxenergy", ship->maxenergy, fread_number(fp))
			;
			KEY("Maxhull", ship->maxhull, fread_number(fp))
			;
			KEY("Maxchaff", ship->maxchaff, fread_number(fp))
			;
			KEY("Maxcloack", ship->maxcloack, fread_number(fp))
			;
			KEY("Maxmodule", ship->maxmodule, fread_number(fp))
			;
			KEY("Maxinterdict", ship->maxinterdict, fread_number(fp))
			;
			KEY("MaxCargo", ship->maxcargo, fread_number(fp))
			;
			if (!str_cmp(word, "Module"))
			{
				MODULE_DATA *module;

				CREATE(module, MODULE_DATA, 1);
				module->type = fread_number(fp);
				module->value = fread_number(fp);
				module->value2 = fread_number(fp);
				module->value3 = fread_number(fp);
				module->size = fread_number(fp);
				module->status = fread_number(fp);
				module->crs = fread_number(fp);
				module->cost = fread_number(fp);
				module->vnum = fread_number(fp);
				module->spyname = fread_string(fp);
				ship->modules.push_back(module);

				fread_to_eol(fp);
				fMatch = true;
			}
			break;

		case 'N':
			SKEY("Name", ship->name, fread_string(fp))
			;
			KEY("Navseat", ship->navseat, fread_number(fp))
			;
			break;

		case 'O':
			if (!str_cmp(word, "Objectnum"))
			{
				fread_number(fp);
				fMatch = true;
				break;
			}
			break;

		case 'P':
			KEY("Pilotseat", ship->pilotseat, fread_number(fp))
			;
			break;

		case 'R':
			KEY("Realspeed", ship->realspeed, fread_number(fp))
			;
			break;

		case 'S':
			KEY("Security", ship->security, fread_number(fp))
			;
			KEY("Shipyard", ship->shipyard, fread_number(fp))
			;
			KEY("Sensor", ship->sensor, fread_number(fp))
			;
			KEY("Size", ship->size, atof(fread_word(fp)))
			;
			break;

		case 'T':
			KEY("Target_array", ship->target_array, fread_number(fp))
			;
			KEY("Type", ship->type, fread_number(fp))
			;
			KEY("Tractorbeam", ship->tractorbeam, fread_number(fp))
			;
			KEY("Trawler", ship->trawler, fread_number(fp))
			;
			if (!str_cmp(word, "Turret"))
			{
				TURRET_DATA *turret;

				CREATE(turret, TURRET_DATA, 1);
				turret->vnum = fread_number(fp);
				turret->type = fread_number(fp);
				turret->status = fread_number(fp);
				ship->turrets.push_back(turret);

				fMatch = true;
			}
			break;
		case 'V':
			KEY("Version", ship->version, fread_number(fp))
			;
			KEY("Vnum", ship->vnum, fread_number(fp))
			;
			break;
		}

		if (!fMatch)
		{
			bug("no match: %s [%s]", word, ship->filename);
		}
	}

}

void fread_ship_resets(SHIP_INDEX_DATA *shrec, FILE *fp)
{
	int iLastRoom = 0;
	int iLastObj = 0;
	int count = 0;
	bool bad;

	if (!shrec)
	{
		bug("no #SHIP seen yet.");
		return;
	}

	for (;;)
	{
		SHIP_ROOM_DATA *sRoom = 0;
		char letter;
		int extra, arg1, arg2, arg3;
		RESET_DATA *pReset;

		if ((letter = fread_letter(fp)) == 'S')
			break;

		if (letter == '*')
		{
			fread_to_eol(fp);
			continue;
		}

		extra = fread_number(fp);
		arg1 = fread_number(fp);
		arg2 = fread_number(fp);
		arg3 = (letter == 'G' || letter == 'R') ? 0 : fread_number(fp);
		fread_to_eol(fp);

		++count;

		/*
		 * Validate parameters.
		 * We're calling the index functions for the side effect.
		 */
		bad = false;
		switch (letter)
		{
		default:
			bug("%s (%d) bad command '%c'.", shrec->filename, count, letter);
			bad = true;
			fread_to_eol(fp);
			break;

		case 'C':
			if ((sRoom = get_sroom_ind(shrec, arg3)) == NULL)
			{
				bug("%s (%d) 'C': room %d doesn't exist.", shrec->filename,
						count, arg3);
				bad = true;
			}
			else
				iLastRoom = arg3;
			break;

		case 'M':
			if ((sRoom = get_sroom_ind(shrec, arg3)) == NULL)
			{
				bug("%s (%d) 'M': room %d doesn't exist.", shrec->filename,
						count, arg3);
				bad = true;
			}
			else
				iLastRoom = arg3;
			break;

		case 'O':
			if ((sRoom = get_sroom_ind(shrec, arg3)) == NULL)
			{
				bug("%s (%d) '%c': room %d doesn't exist.", shrec->filename,
						count, letter, arg3);
				bad = true;
			}
			else
				iLastObj = arg3;
			break;

		case 'P':
			if (get_obj_index(arg3) == NULL)
			{
				bug("%s (%d) 'P': destination object %d doesn't exist.",
						shrec->filename, count, arg3);
				bad = true;
			}
			sRoom = get_sroom_ind(shrec, iLastObj);
			break;

		case 'G':
		case 'E':
			sRoom = get_sroom_ind(shrec, iLastRoom);
			break;

		case 'T':
		case 'H':
			sRoom = get_sroom_ind(shrec, iLastObj);
			break;

		case 'D':
			bad = true;
			break;

		case 'R':
			bad = true;
			break;
		}

		/* finally, add the reset */
		letter = UPPER(letter);

		if (!bad)
		{
			pReset = make_reset(letter, extra, arg1, arg2, arg3);
			sRoom->resets.push_back(pReset);
		}
	}

	return;
}

/* laduje caly rekord do bazy (lokacje + dane statku) */
void load_ship_index(char *ship_file)
{
	FILE *fp;
	char filename[256];

	sprintf(filename, "%s%s", SHIPDB_DIR, ship_file);

	if ((fp = fopen(filename, "r")) == NULL)
	{
		log_string("Cannot open ship_index file");
		perror(filename);
		return;
	}

	strcpy(filename, ship_file);
	for (;;)
	{
		SHIP_INDEX_DATA *shrec = 0;
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
			bug("No '#' in ship.db: '%c' (filename: %s)", letter, filename);
			exit(1);
		}

		word = fread_word(fp);

		if (!str_cmp(word, "$"))
		{
			ship_index_list.push_back(shrec);
			break;
		}
		else if (!str_cmp(word, "SHIP"))
		{
			shrec = new_ship_index();
			STRDUP(shrec->filename, filename);
			shrec->security = 2;
			shrec->resetcount = 0;
			fread_ship_data(shrec, fp);
		}
		else if (!str_cmp(word, "ROOMS"))
		{
			shrec->roomcount = 0;
			fread_ship_rooms(shrec, fp);
		}
		else if (!str_cmp(word, " RESETS"))
		{
			fread_ship_resets(shrec, fp);
		}
		else
		{
			bug("Unknown section in ship.db", 0);
			fread_to_eol(fp);
			continue;
		}
	}
	fclose(fp);

}

void load_shipdb()
{
	FILE *fp;

	// lamerskie wyszukanie odpowiedniej krainy (lepiej byloby dac
	// krainie flage np. AREA_FOR_SHIPS -- wtedy mogloby byc kilka
	// krainek na statki i szukaloby sie miejsca na statek w kazdej po
	// kolei) no, ale jak sie ne ma co sie lubi.. ;)
	shipTmpArea = nullptr;
	for (auto* a : area_list)
		if (!str_cmp(a->filename, SHIP_TMP_AREA))
			{ shipTmpArea = a; break; }

	IF_BUG(shipTmpArea == NULL, "You need an area: " SHIP_TMP_AREA)
		return;

	RESERVE_CLOSE;
	if ((fp = fopen( SHIP_DB, "r")) == NULL)
	{
		log_string("Coulnd't open ship.db file");
		perror( SHIP_DB);
		RESERVE_OPEN;
		return;
	}

	for (;;)
	{
		char *ship_file;

		ship_file = fread_word(fp);

		if (ship_file[0] == '$')
			break;

		load_ship_index(ship_file);
	}

	fclose(fp);
	if (!sysdata.silent)
		log_string(" Done ShipDb");
	RESERVE_OPEN;
	return;
}

void save_ship_index(SHIP_INDEX_DATA *ship)
{
	char filename[256];
	FILE *fp;

	IF_BUG(ship == NULL, "")
		return;

	if (!ship->filename || ship->filename[0] == '\0')
	{
		bug("%s has no filename", ship->name);
		return;
	}

	sprintf(filename, "Saving ship index: %s.", ship->name);
	log_string(filename);

	sprintf(filename, "%s%s", SHIPDB_DIR, ship->filename);
	RESERVE_CLOSE;

	if ((fp = fopen(filename, "w")) == NULL)
	{
		bug("fopen (%s)", filename);
		perror(filename);
	}
	else
	{
		fprintf(fp, "#SHIP\n");
		fprintf(fp, "Name         %s~\n", ship->name);
		fprintf(fp, "Vnum         %d\n", ship->vnum);
		fprintf(fp, "Builders     %s~\n", ship->builders);
		fprintf(fp, "Security     %d\n", ship->security);
		fprintf(fp, "Description  %s~\n", ship->description);
		fprintf(fp, "Class        %d\n", ship->clazz);
		fprintf(fp, "Tractorbeam  %d\n", ship->tractorbeam);
		fprintf(fp, "Shipyard     %d\n", ship->shipyard);
		fprintf(fp, "Lasers       %d\n", ship->lasers);
		fprintf(fp, "Ioncannons   %d\n", ship->ioncannons);
		fprintf(fp, "Maxmissiles  %d\n", ship->maxmissiles);
		fprintf(fp, "Maxrockets   %d\n", ship->maxrockets);
		fprintf(fp, "Maxtorpedos  %d\n", ship->maxtorpedos);
		fprintf(fp, "Maxshield    %d\n", ship->maxshield);
		fprintf(fp, "Maxhull      %d\n", ship->maxhull);
		fprintf(fp, "Maxenergy    %d\n", ship->maxenergy);
		fprintf(fp, "Hyperspeed   %.0f\n", ship->hyperspeed);
		fprintf(fp, "Comm         %d\n", ship->comm);
		fprintf(fp, "Maxchaff     %d\n", ship->maxchaff);
		fprintf(fp, "Sensor       %d\n", ship->sensor);
		fprintf(fp, "Astro_array  %d\n", ship->astro_array);
		fprintf(fp, "Target_array %d\n", ship->target_array);
		fprintf(fp, "Realspeed    %.0f\n", ship->realspeed);
		fprintf(fp, "Type         %d\n", ship->type);
		fprintf(fp, "Cockpit      %d\n", ship->cockpit);
		fprintf(fp, "Coseat       %d\n", ship->coseat);
		fprintf(fp, "Pilotseat    %d\n", ship->pilotseat);
		fprintf(fp, "Gunseat      %d\n", ship->gunseat);
		fprintf(fp, "Navseat      %d\n", ship->navseat);
		fprintf(fp, "Engineroom   %d\n", ship->engineroom);
		fprintf(fp, "Entrance     %d\n", ship->entrance);
		fprintf(fp, "Manuever     %d\n", ship->manuever);
		fprintf(fp, "Maxcloack    %d\n", ship->maxcloack);
		fprintf(fp, "Maxinterdict %d\n", ship->maxinterdict);
		for (auto* hangar : ship->hangars)
			fprintf(fp, "Hangar       %d %.0f %d %d\n", hangar->vnum,
					hangar->capacity, hangar->status, hangar->type);
		for (auto* turret : ship->turrets)
			fprintf(fp, "Turret       %d %d %d\n", turret->vnum, turret->type,
					turret->status);
		for (auto* dock : ship->docks)
			fprintf(fp, "Dock         %d %d \n", dock->type, dock->vnum);
		/*	for( module = ship->first_module; module ; module = module->next ) // Narazie zakomentowane bo nie skonczylem
		 fprintf( fp, "Module       %d %d %d %d %d %d %d %d %d %s~\n",  // jeszcze tego - Alde
		 module->type, module->value, module->value2, module->value3, module->size,
		 module->status, module->crs, module->cost, module->vnum, module->spyname );
		 */fprintf(fp, "MaxCargo     %d\n", ship->maxcargo);
		fprintf(fp, "Trawler      %d\n", ship->trawler);
		fprintf(fp, "Size         %.0f\n", ship->size);
		fprintf(fp, "Maxmodule    %d\n", ship->maxmodule);
		fprintf(fp, "Level        %d\n", ship->level);
		fprintf(fp, "Version      %d\n", ship->version);
		fprintf(fp, "End\n\n");

		fprintf(fp, "#ROOMS\n");

		for (auto* sRoom : ship->rooms)
		{
			fprintf(fp, "#%d\n", sRoom->vnum);
			fprintf(fp, "%s~\n", sRoom->name);
			fprintf(fp, "%s~\n", strip_cr(sRoom->description));
			fprintf(fp, "%d %lld %d %d %d %d\n", sRoom->light,
					sRoom->room_flags, sRoom->sector_type, sRoom->tele_delay,
					sRoom->tele_vnum, sRoom->tunnel);

			for (auto* xit : sRoom->exits)
			{
				if (IS_SET(xit->flags, EX_PORTAL)) /* don't fold portals */
					continue;
				fprintf(fp, "D%d\n", xit->vdir);
				fprintf(fp, "%s~\n", strip_cr(xit->description));
				fprintf(fp, "%s~\n", strip_cr(xit->keyword));
				fprintf(fp, "%lld %d %d %d\n", xit->flags & ~EX_BASHED,
						xit->key, xit->vnum, xit->distance);
			}

			for (auto* ed : sRoom->extradesc)
				fprintf(fp, "E\n%s~\n%s~\n", ed->keyword,
						strip_cr(ed->description));

			if (!sRoom->mudprogs.empty())
			{
				for (auto* mprog : sRoom->mudprogs)
					fprintf(fp, "> %s %s~\n%s~\n",
							mprog_type_to_name(mprog->type), mprog->arglist,
							strip_cr(mprog->comlist));
				fprintf(fp, "|\n");
			}

			fprintf(fp, "S\n");
		}
		fprintf(fp, "#End\n\n\n");

		fprintf(fp, "#RESETS\n");
		for (auto* sRoom : ship->rooms)
		{
			for (auto* pReset : sRoom->resets)
			{
				switch (pReset->command)
				/* extra arg1 arg2 arg3 */
				{
				default:
				case '*':
					break;
				case 'm':
				case 'M':
				case 'o':
				case 'O':
				case 'p':
				case 'P':
				case 'e':
				case 'E':
				case 'd':
				case 'D':
				case 't':
				case 'T':
				case 'c':
				case 'C':
					fprintf(fp, "%c %d %lld %lld %lld\n",
							UPPER(pReset->command), pReset->extra, pReset->arg1,
							pReset->arg2, pReset->arg3);
					break;

				case 'g':
				case 'G':
				case 'r':
				case 'R':
					fprintf(fp, "%c %d %lld %lld\n", UPPER(pReset->command),
							pReset->extra, pReset->arg1, pReset->arg2);
					break;
				}
			}
		}
		fprintf(fp, "S\n\n");

		fprintf(fp, "#$\n\n");
		fclose(fp);
	}
	RESERVE_OPEN;
	return;

}

void save_ship_index_list()
{
	FILE *fp;

	if ((fp = fopen( SHIP_DB, "w")) == NULL)
	{
		log_string("Save_ship_index_list: Coulnd't open ship.db file");
		perror( SHIP_DB);
		return;
	}

	log_string("Saving ship index list.");

	for (auto* ship : ship_index_list)
		fprintf(fp, "%s\n", ship->filename);

	fprintf(fp, "$\n");

	fclose(fp);
	return;
}

DEF_DO_FUN( shsave )
{
	SHIP_INDEX_DATA *ship;

	if (!*argument)
	{
		send_to_char("Syntax: shsave <name> or shsave all" NL, ch);
		return;
	}

	if (!str_cmp(argument, "all"))
	{
		for (auto* ship : ship_index_list)
			save_ship_index(ship);
		send_to_char("Ok. Saving list..." NL, ch);
		save_ship_index_list();
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(argument, "list"))
	{
		save_ship_index_list();
		send_to_char("Done." NL, ch);
		return;
	}

	if (!(ship = get_ship_index(argument)))
	{
		send_to_char("No such ship in ShipDb" NL, ch);
		return;
	}

	save_ship_index(ship);
	send_to_char("Ok." NL, ch);
	return;
}

