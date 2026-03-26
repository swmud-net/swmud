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
 *			    Space Resets Module by Thanos (2004)           *
 ****************************************************************************/

#include <time.h>
#include "mud.h"

void boot_log args( ( const char *str, ... ) ); // db.c
SHIP_INDEX_DATA* get_ship_index args( ( char * ship_index ) ); //shpconstr.c
SHIP_DATA* constr_ship args( ( SHIP_INDEX_DATA *shrec ) ); //shpconstr.c
void reset_room args( ( ROOM_INDEX_DATA *pRoom ) );

void reset_ship_rooms(SHIP_DATA *ship)
{
	for (auto* room : ship->locations)
		reset_room(room);
}

void place_ship_in_room(SHIP_DATA *ship, ROOM_INDEX_DATA *pRoomIndex)
{
	extract_ship(ship);
	ship_to_room(ship, pRoomIndex);
	ship->location = pRoomIndex;
	ship->shipyard = pRoomIndex;
	ship->in_room = pRoomIndex;
	ship->lastdoc = ship->location;
	ship->shipstate = SHIP_DOCKED;

	if (!*ship->transponder)
		generate_transponder(ship);

	reset_ship_rooms(ship);
}

void create_ship_in_room(SHIP_INDEX_DATA *shrec, ROOM_INDEX_DATA *pRoomIndex)
{
	SHIP_DATA *ship;

	if (!(ship = constr_ship(shrec)))
		return;

	place_ship_in_room(ship, pRoomIndex);

}

void place_ship_in_space(SHIP_DATA *ship, SPACE_DATA *starsystem, int arg1,
		int arg2, int arg3)
{
	int plusminus;
	// wyci�gni�te z launchship
	ship->location = NULL;
	ship->lastdoc = ship->location;
//    ship->type = MOB_SHIP;
	ship->autopilot = true;
	ship->autorecharge = true;

	extract_ship(ship);
	ship_to_starsystem(ship, starsystem);
	STRDUP(ship->home, starsystem->name);

	plusminus = number_range(-1, 2);
	if (plusminus > 0)
		ship->hx = 1;
	else
		ship->hx = -1;
	plusminus = number_range(-1, 2);
	if (plusminus > 0)
		ship->hy = 1;
	else
		ship->hy = -1;
	plusminus = number_range(-1, 2);
	if (plusminus > 0)
		ship->hz = 1;
	else
		ship->hz = -1;

	ship->energy -= (100 + 10 * ship->size);

	ship->vx = number_fuzzy(arg1);
	ship->vy = number_fuzzy(arg2);
	ship->vz = number_fuzzy(arg3);

	ship->shield = ship->maxshield;

	if (ship->vXpos != 0 || ship->vYpos != 0)
	{
		ship->vXpos = 0;
		ship->vYpos = 0;
		STRDUP(ship->sXpos, "");
		STRDUP(ship->sYpos, "");
	}
	ship->currjump = NULL;
	ship->jx = 0;
	ship->jy = 0;
	ship->jz = 0;
	ship->hx = 1;
	ship->hy = 1;
	ship->hz = 1;

	ship->statet0 = LASER_READY;
	ship->missilestate = MISSILE_READY;
	ship->shipstate = SHIP_READY;

	generate_sslook(ship);

	if (!*ship->transponder)
		generate_transponder(ship);

	reset_ship_rooms(ship);

}

void create_ship_in_space(SHIP_INDEX_DATA *shrec, SPACE_DATA *starsystem,
		int arg1, int arg2, int arg3)
{
	SHIP_DATA *ship;

	if (!(ship = constr_ship(shrec)))
		return;

	place_ship_in_space(ship, starsystem, arg1, arg2, arg3);
}

void display_space_resets(SPACE_DATA *pStarsystem, CHAR_DATA *ch)
{
	SHIP_INDEX_DATA *shrec;
	int i = 0;
	const char *mname;
	char pbuf[MIL];
	char buf[MAX_STRING_LENGTH];

	buf[0] = '\0';

	pager_printf(ch,
			FB_WHITE
			"[NR] [Type] [Vnum ] [Description          ] [      X] [      Y] [      Z] [Max]" EOL);

	for (auto* pReset : pStarsystem->resets)
	{
		++i;
		switch (pReset->command)
		{
		default:
			sprintf(pbuf, "*** BAD RESET: %c %d %lld %lld %lld ***" NL,
					pReset->command, pReset->extra, pReset->arg1, pReset->arg2,
					pReset->arg3);
			break;

		case 'S':
			if (!(shrec = get_ship_index(itoa(pReset->extra))))
				mname = "Ship:  *BAD VNUM* ";
			else
				mname = shrec->name;

			sprintf(pbuf,
					"[%2d] [Ship] "
					FG_GREEN "[%5d] [%-21.21s" FG_GREEN "] [%7lld] [%7lld] [%7lld] [%3lld]" EOL,
					i, pReset->extra, mname, pReset->arg1, pReset->arg2,
					pReset->arg3, pReset->arg4);
			strcat(buf, pbuf);
			break;
		}
	}
	pager_printf(ch, buf);
	return;
}

/*
 * Create a new space reset (for online building)		-Thanos
 */
RESET_DATA* make_space_reset(char letter, int extra, int64 arg1, int64 arg2,
		int64 arg3, int64 arg4)
{
	RESET_DATA *pReset;

	CREATE(pReset, RESET_DATA, 1);
	pReset->command = letter;
	pReset->extra = extra;
	pReset->arg1 = arg1;
	pReset->arg2 = arg2;
	pReset->arg3 = arg3;
	pReset->arg4 = arg4;
	return pReset;
}

/*
 * Add a reset to a starsystem		-Thanos
 */
RESET_DATA* add_space_reset(SPACE_DATA *starsystem, char letter, int extra,
		int64 arg1, int64 arg2,
		int64 arg3, int64 arg4)
{
	RESET_DATA *pReset;

	IF_BUG(starsystem == NULL, "")
		return NULL;

	letter = UPPER(letter);
	pReset = make_space_reset(letter, extra, arg1, arg2, arg3, arg4);

	starsystem->resets.push_back(pReset);
	return pReset;
}

void fread_space_resets(SPACE_DATA *starsystem, FILE *fp)
{
	SHIP_INDEX_DATA *shrec;
	bool bad;
	int count = 0;

	for (;;)
	{
		char letter;
		int extra;
		int64 arg1, arg2, arg3, arg4;

		if ((letter = fread_letter(fp)) == '0')
			break;

		if (letter == '*')
		{
			fread_to_eol(fp);
			continue;
		}

		extra = fread_number(fp);
		arg1 = fread_number64(fp);
		arg2 = fread_number64(fp);
		arg3 = fread_number64(fp);
		arg4 = fread_number64(fp);
		fread_to_eol(fp);

		count++;
		bad = false;
		/*
		 * Validate parameters.
		 * We're calling the index functions for the side effect.
		 */
		switch (letter)
		{
		default:
			boot_log("Load_space_resets: %s (%d) bad command '%c'.",
					starsystem->filename, count, letter);
			bad = true;
			break;

		case 'S':
			if (!(shrec = get_ship_index(itoa(extra))))
			{
				bug("'S': bad ship vnum %d. (%s)", extra, starsystem->filename);
				bad = true;
				break;
			}
			break;
		}

		/* finally, add the reset */
		if (!bad)
			add_space_reset(starsystem, letter, extra, arg1, arg2, arg3, arg4);
	}
	return;
}

void save_space_resets(SPACE_DATA *starsystem, FILE *fp)
{
	if (starsystem->resets.empty())
		return;

	fprintf(fp, "#RESETS\n");
	for (auto* reset : starsystem->resets)
	{
		switch (reset->command)
		/* extra arg1 arg2 arg3 arg4 */
		{
		case 's':
		case 'S':
			fprintf(fp, "%c %d %lld %lld %lld %lld\n", UPPER(reset->command),
					reset->extra, reset->arg1, reset->arg2, reset->arg3,
					reset->arg4);
			break;
		}
	}
	fprintf(fp, "0\n\n");
	return;
}

RESET_DATA* get_space_reset(SPACE_DATA *pStarsystem, int num)
{
	int nr = 0;

	for (auto* pReset : pStarsystem->resets)
		if (++nr == num)
			return pReset;

	return NULL;
}

void delete_space_reset(SPACE_DATA *pStarsystem, RESET_DATA *pReset)
{
	pStarsystem->resets.remove(pReset);
	free_reset(pReset);
	return;
}

void reset_starsystem(SPACE_DATA *starsystem)
{
	SHIP_INDEX_DATA *shrec;

	if (!starsystem)
		return;

	for (auto* pReset : starsystem->resets)
	{
		switch (pReset->command)
		{
		default:
		{
			bug("bad command %c. (%s)", pReset->command, starsystem->filename);
		}
			break;

			/* Ship to Space */
		case 'S':
		{
			int count;

			if (!(shrec = get_ship_index(itoa(pReset->extra))))
			{
				bug("'S': bad ship vnum %d. (%s)", pReset->extra,
						starsystem->filename);
				break;
			}

			// limity
			count = 0;
			for (auto* ship : starsystem->ships)
				if (ship->pIndexData == shrec)
					count++;
			if (count >= pReset->arg2)
			{
				shrec = NULL;
				break;
			}
			if (!shrec)
				break;

			create_ship_in_space(shrec, starsystem, pReset->arg1, pReset->arg1,
					pReset->arg1);
		}
			break;
		}
	}
}

/*
 * Update starsystems -- sprawdza, czy system gwiezdny si� zestarza�.
 */
void starsystems_update(void)
{
	for (auto* pStarsystem : starsystem_list)
	{
		char buf[MSL];

		int reset_age;

		reset_age =
				pStarsystem->reset_frequency ?
						pStarsystem->reset_frequency : DEF_SPACE_RESETFREQ;

		if ((reset_age == -1 && pStarsystem->age == -1)
				|| ++pStarsystem->age < (reset_age - 1))
			continue;

		if (pStarsystem->age >= reset_age)
		{
			SHIP_DATA *target;

			if (*pStarsystem->resetmsg)
				sprintf(buf, NL "%s" EOL, pStarsystem->resetmsg);
			else
				strcpy(buf,
						NL "Widzisz jak powiew gwiezdnego py�u odbija promienie gwiazd..." NL);

			for (auto* target : pStarsystem->ships)
				echo_to_cockpit(target, buf);

			reset_starsystem(pStarsystem);

			if (reset_age == -1)
				pStarsystem->age = -1;
			else
				pStarsystem->age = number_range(0, reset_age / 5);
		}
	}
	return;
}

