/***************************************************************************
 * Star Wars Reality Code Additions and changes from the Smaug Code		 *
 * copyright (c) 1997 by Sean Cooper										*
 * Starwars and Starwars Names copyright(c) Lucas Film Ltd.				 *
 * SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider						   *
 * SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,					*
 * Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops				*
 * Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael		*
 * Chastain, Michael Quan, and Mitchell Tse.								*
 * Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,		  *
 * Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.	 *
 * ------------------------------------------------------------------------ *
 *					 ____________   __	 ______						 *
 *   Aldegard	Jot  (   ___   ___| /  \   |  __  )   Thanos	  Trog	 *
 *		______________\  \|   |  /  ^   \ |	 <_______________		  *
 *		\________________/|___|/___/"\___\|__|\________________/		  *
 *				  \   \/ \/   //  \   |  __  )(  ___/`					*
 *					\	   //  ^   \ |	 <__)  \					   *
 *					  \_!_//___/"\___\|__|\______/TM					  *
 * (c) 2001, 2002			M	   U		D				Ver 1.1	  *
 * ------------------------------------------------------------------------ *
 *			 Online Reset Editing Module			   *
 ****************************************************************************/

/*
 * This file relies heavily on the fact that your linked lists are correct,
 * and that pArea->reset_first is the first reset in pArea.  Likewise,
 * pArea->reset_last *MUST* be the last reset in pArea.  Weird and
 * wonderful things will happen if any of your lists are messed up, none
 * of them good.  The most important are your pRoom->contents,
 * pRoom->people, rch->carrying, obj->contains, and pArea->reset_first ..
 * pArea->reset_last.  -- Altrag
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif

#include <time.h>
#include <list>
#include "mud.h"

using std::list;

/* Externals */
extern int top_reset;
char* sprint_reset args( ( CHAR_DATA *ch, RESET_DATA *pReset,
				int num, bool rlist ) );
RESET_DATA* parse_reset args( ( ROOM_INDEX_DATA *tarea, char *argument,
				CHAR_DATA *ch ) );

ROOM_INDEX_DATA* get_sroom args( ( SHIP_DATA *ship, int svnum ) );

// shipconstr.c
SHIP_INDEX_DATA* get_ship_index args( ( char * ship_index ) );
void create_ship_in_room(SHIP_INDEX_DATA *shrec, ROOM_INDEX_DATA *pRoomIndex);

//save.cpp
void save_room_storage(ROOM_INDEX_DATA *room);

int get_wearloc args( ( char *type ) );
int get_trapflag args( ( char *flag ) );
int get_exflag args( ( char *flag ) );
int get_rflag args( ( char *flag ) );

// �eby komenda RESET odnawia�a WSZYSTKIE resety
bool forceReset = false;	// Thanos

void show_room_resets(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom)
{
	ROOM_INDEX_DATA *room;
	RESET_DATA *lo_reset;
	RESET_DATA *pReset;
	OBJ_INDEX_DATA *obj;
	OBJ_INDEX_DATA *obj2;
	OBJ_INDEX_DATA *lastobj;
	MOB_INDEX_DATA *mob;
	SHIP_INDEX_DATA *shrec;
	char buf[MAX_STRING_LENGTH];
	const char *rname;
	const char *mname = 0;
	const char *oname = 0;
	int num = 0;
	char *pbuf;

	if (pRoom->resets.empty())
	{
		send_to_pager("This room has no resets defined." NL, ch);
		return;
	}

	room = NULL;
	mob = NULL;
	obj = NULL;
	lastobj = NULL;
	lo_reset = NULL;

	num = 0;
	for (auto* pReset : pRoom->resets)
	{
		++num;
		sprintf(buf, "[" FB_WHITE "%2d" PLAIN "]", num);
		pbuf = buf + strlen(buf);

		switch (pReset->command)
		{
		default:
			sprintf(pbuf, "*** BAD RESET: %c %d %lld %lld %lld ***" NL,
					pReset->command, pReset->extra, pReset->arg1, pReset->arg2,
					pReset->arg3);
			break;
		case 'M':
			if (!(mob = get_mob_index(pReset->arg1)))
				mname = "Mobile: *BAD VNUM*";
			else
				mname = mob->player_name;

			if (!(room = get_room_index(pReset->arg3)))
				rname = "Room: *BAD VNUM*";
			else
				rname = room->name;

			sprintf(pbuf, "[Mob]:"
			FB_RED "[%5lld]%-20.20s" PLAIN " [%3lld][room	][%5lld]%-20.20s",
					pReset->arg1, mname, pReset->arg2, pReset->arg3, rname);

			if (!room)
				mob = NULL;
			strcat(buf, EOL);
			break;

		case 'C':
			if (!(shrec = get_ship_index(itoa(pReset->arg1))))
				mname = "Ship:  *BAD VNUM* ";
			else
				mname = shrec->name;

			if (!(room = get_room_index(pReset->arg3)))
				rname = "Room: *BAD VNUM*";
			else
				rname = room->name;

			sprintf(pbuf, "[Shp]:"
			FB_CYAN "[%5lld]%-20.20s" PLAIN " [%3lld][room	][%5lld]%-20.20s",
					pReset->arg1, mname, pReset->arg2, pReset->arg3, rname);

			strcat(buf, EOL);
			break;

		case 'G':
		case 'E':
			if (!mob)
				mname = "* ERROR: NO MOBILE! *";

			if (!(obj = get_obj_index(pReset->arg1)))
				oname = "Object: *BAD VNUM*";
			else
				oname = obj->name;

			sprintf(pbuf,
					"%s"
					FG_CYAN "[%5lld]%-20.20s" PLAIN " %s[%-8.8s]" FB_RED "[%5d]%-20.20s" EOL,
					pReset->command == 'G' ? "[Giv]:" : "[Equ]:", pReset->arg1,
					oname, (mob && mob->pShop) ? "[sho]" : "	 ",
					(pReset->command == 'G' ?
							"carry" : bit_name(wear_types_list, pReset->arg3)),
					(mob ? mob->vnum : -1), mname);

			lastobj = obj;
			lo_reset = pReset;
			break;
		case 'O':
			if (!(obj = get_obj_index(pReset->arg1)))
				oname = "Object: *BAD VNUM*";
			else
				oname = obj->name;

			if (!(room = get_room_index(pReset->arg3)))
				rname = "Room: *BAD VNUM*";
			else
				rname = room->name;

			sprintf(pbuf,
					"[Obj]:"
					FG_CYAN "[%5lld]%-20.20s " PLAIN "[%3lld][room	][%5lld]%-20.20s" EOL,
					pReset->arg1, oname, pReset->arg2, pReset->arg3, rname);

			if (!room)
				obj = NULL;

			lastobj = obj;
			lo_reset = pReset;
			break;

		case 'P':
			if (!(obj = get_obj_index(pReset->arg1)))
				oname = "Object1: *BAD VNUM*";
			else
				oname = obj->name;

			obj2 = NULL;
			if (pReset->arg3 > 0)
			{
				obj2 = get_obj_index(pReset->arg3);
				rname = (obj2 ? obj2->name : "Object2: *BAD VNUM*");
				lastobj = obj2;
			}
			else if (!lastobj)
				rname = "Object2: *NULL obj*";
			else if (pReset->extra == 0)
			{
				rname = lastobj->name;
				obj2 = lastobj;
			}
			else
			{
				int iNest;
				RESET_DATA *reset = NULL;

				auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), lo_reset);
				if (it != pRoom->resets.end()) ++it;
				for (iNest = 0; iNest < pReset->extra; iNest++)
				{
					reset = NULL;
					for (; it != pRoom->resets.end(); ++it)
					{
						reset = *it;
						if (reset->command == 'O' || reset->command == 'G'
								|| reset->command == 'E'
								|| (reset->command == 'P' && !reset->arg3
										&& reset->extra == iNest))
							break;
						reset = NULL;
					}

					if (!reset || reset->command != 'P')
						break;
				}

				if (!reset)
					rname = "Object2: *BAD NESTING*";
				else if (!(obj2 = get_obj_index(reset->arg1)))
					rname = "Object2: *NESTED BAD VNUM*";
				else
					rname = obj2->name;
			}

			sprintf(pbuf,
					"[Put]:"
					FG_CYAN "[%5lld]%-20.20s" PLAIN " [%3lld][in	  ]" FG_CYAN "[%5lld]%-20.20s" EOL,
					pReset->arg1, oname, pReset->arg2,
					(obj2 ? obj2->vnum : pReset->arg3), rname);
			break;

		case 'T':
			sprintf(pbuf,
					FG_GREEN "[TRAP]" PLAIN " %5d %5lld %5lld %5lld (%s)" NL,
					pReset->extra, pReset->arg1, pReset->arg2, pReset->arg3,
					flag_string(trap_flags_list, pReset->extra));
			break;

		case 'H':
			if (pReset->arg1 > 0)
				if (!(obj2 = get_obj_index(pReset->arg1)))
					rname = "Object: *BAD VNUM*";
				else
					rname = obj2->name;
			else if (!obj)
				rname = "Object: *NULL obj*";
			else
				rname = oname;
			sprintf(pbuf,
					"[Hide] [" FB_WHITE "%5lld" PLAIN "]" FG_CYAN "%-20.20s" EOL,
					(pReset->arg1 > 0 ? pReset->arg1 :
						obj ? (int64) obj->vnum : 0), rname);
			break;

		case 'B':
		{
			const struct flag_type *flagarray;

			strcpy(pbuf, "[Bit]: ");
			pbuf += 5;
			if (IS_SET(pReset->arg2, BIT_RESET_SET))
			{
				strcpy(pbuf, "Set: ");
				pbuf += 5;
			}
			else if (IS_SET(pReset->arg2, BIT_RESET_TOGGLE))
			{
				strcpy(pbuf, "Toggle: ");
				pbuf += 8;
			}
			else
			{
				strcpy(pbuf, "Remove: ");
				pbuf += 8;
			}

			switch (pReset->arg2 & BIT_RESET_TYPE_MASK)
			{
			case BIT_RESET_DOOR:
			{
				int door;

				if (!(room = get_room_index(pReset->arg1)))
					rname = "Room: *BAD VNUM*";
				else
					rname = room->name;
				door = (pReset->arg2 & BIT_RESET_DOOR_MASK)
						>> BIT_RESET_DOOR_THRESHOLD;
				door = URANGE(0, door, MAX_DIR+1);

				sprintf(pbuf, "[Exit] %s%s (%d), Room %s (%lld)",
						dir_name[door],
						(room && get_exit(room, door) ? "" : " (NO EXIT!)"),
						door, rname, pReset->arg1);
			}
				flagarray = exit_flags_list;
				break;

			case BIT_RESET_ROOM:
				if (!(room = get_room_index(pReset->arg1)))
					rname = "Room: *BAD VNUM*";
				else
					rname = room->name;
				sprintf(pbuf,
						"Room " PLAIN "[" FB_WHITE "%5lld" PLAIN "]" FG_CYAN "%-20.20s",
						pReset->arg1, rname);
				flagarray = room_flags_list;
				break;

			case BIT_RESET_OBJECT:
				if (pReset->arg1 > 0)
					if (!(obj2 = get_obj_index(pReset->arg1)))
						rname = "Object: *BAD VNUM*";
					else
						rname = obj2->name;
				else if (!obj)
					rname = "Object: *NULL obj*";
				else
					rname = oname;
				sprintf(pbuf,
						"Object " PLAIN "[" FB_WHITE "%5lld" PLAIN "]" FG_CYAN "%-20.20s",
						(pReset->arg1 > 0 ? pReset->arg1 :
							obj ? (int64) obj->vnum : 0), rname);
				flagarray = obj_flags_list;
				break;

			case BIT_RESET_MOBILE:
				if (pReset->arg1 > 0)
				{
					MOB_INDEX_DATA *mob2;

					if (!(mob2 = get_mob_index(pReset->arg1)))
						rname = "Mobile: *BAD VNUM*";
					else
						rname = mob2->player_name;
				}
				else if (!mob)
					rname = "Mobile: *NULL mob*";
				else
					rname = mname;
				sprintf(pbuf,
						"Mobile " PLAIN "[" FB_WHITE "%5lld" PLAIN "]" FG_CYAN "%-20.20s",
						(pReset->arg1 > 0 ? pReset->arg1 :
							mob ? (int64) mob->vnum : 0), rname);
				flagarray = area_flags_list;
				break;
			default:
				sprintf(pbuf, "bad type %lld",
						pReset->arg2 & BIT_RESET_TYPE_MASK);
				flagarray = NULL;
				break;
			}
			pbuf += strlen(pbuf);
			if (flagarray)
				sprintf(pbuf, "; flags: %s [%lld]" EOL,
						flag_string(flagarray, pReset->arg3), pReset->arg3);
			else
				sprintf(pbuf, "; flags %lld\n\r", pReset->arg3);
		}
			break;

		case 'D':
		{
			const char *ef_name;

			pReset->arg2 = URANGE(0, pReset->arg2, MAX_DIR+1);

			if (!(room = get_room_index(pReset->arg1)))
				rname = "Room: *BAD VNUM*";
			else
				rname = room->name;

			switch (pReset->arg3)
			{
			default:
				ef_name = "(* ERROR *)";
				break;
			case 0:
				ef_name = "Open";
				break;
			case 1:
				ef_name = "Close";
				break;
			case 2:
				ef_name = "Close and lock";
				break;
			}
			sprintf(pbuf, "[Door] %s [%lld] %s%s [%lld] door %s (%lld)" NL,
					ef_name, pReset->arg3, dir_name[pReset->arg2],
					(room && get_exit(room, pReset->arg2) ? "" : " (NO EXIT!)"),
					pReset->arg2, rname, pReset->arg1);
		}
			break;

		case 'R':
			if (!(room = get_room_index(pReset->arg1)))
				rname = "Room: *BAD VNUM*";
			else
				rname = room->name;

			sprintf(pbuf,
					"[Rand] exits 0 to %lld" FB_WHITE "->" PLAIN "%s (%lld)" NL,
					pReset->arg2, rname, pReset->arg1);
			break;
		}

		pager_printf(ch, buf);
	}
}

void display_resets(CHAR_DATA *ch, bool area)
{
	ROOM_INDEX_DATA *pRoom;
	int i;

	pager_printf(ch,
			FB_WHITE
			"[NR][Type][Vnum ][Description		][Max][Location][Vnum ][Description	   ]" EOL);
	if (!area)
	{
		if (ch->desc->connected != CON_REDITOR || !(pRoom =
				(ROOM_INDEX_DATA*) ch->desc->olc_editing))
			pRoom = ch->in_room;

		show_room_resets(ch, pRoom);
		return;
	}
	else
	{
		AREA_DATA *pArea;

		if (ch->desc->connected != CON_AEDITOR
				|| !(pArea = (AREA_DATA*) ch->desc->olc_editing))
			pArea = ch->in_room->area;

		for (i = pArea->lvnum; i <= pArea->uvnum; i++)
		{
			if (!(pRoom = get_room_index(i)))
				continue;
			if (!pRoom->resets.empty())
				show_room_resets(ch, pRoom);
		}
		return;
	}
}

RESET_DATA* get_room_reset(ROOM_INDEX_DATA *pRoom, int num)
{
	int nr = 0;

	for (auto* pReset : pRoom->resets)
		if (++nr == num)
			return pReset;

	return NULL;
}

int generate_itemlevel(AREA_DATA *pArea, OBJ_INDEX_DATA *pObjIndex)
{
	int olevel;
	int min = UMAX(pArea->low_range, 1);
	int max = UMIN(pArea->high_range, min + 15);

	if (pObjIndex->level > 0)
		olevel = UMIN(pObjIndex->level, MAX_LEVEL);
	else
		switch (pObjIndex->item_type)
		{
		default:
			olevel = 0;
			break;
		case ITEM_PILL:
			olevel = number_range(min, max);
			break;
		case ITEM_POTION:
			olevel = number_range(min, max);
			break;
		case ITEM_DEVICE:
			olevel = number_range(min, max);
			break;
		case ITEM_ARMOR:
			olevel = number_range(min + 4, max + 1);
			break;
		case ITEM_WEAPON:
			olevel = number_range(min + 4, max + 1);
			break;
		}
	return olevel;
}

ROOM_INDEX_DATA* find_room(ROOM_INDEX_DATA *pRoom, int vnum)
{
	if (pRoom->vnum)
		return get_room_index(vnum);

	if (pRoom->ship)
		return get_sroom(pRoom->ship, vnum);

	return NULL;
}

int mobs_in_area( MID *mob, AREA_DATA *area)
{
	RID *room;
	int count = 0;

	for (int vnum = area->lvnum; vnum <= area->uvnum; vnum++)
		if ((room = get_room_index(vnum)))
			for (auto* ch : room->people)
				if ( IS_NPC( ch ) && ch->pIndexData == mob)
					count++;

	return count;
}

void reset_room(ROOM_INDEX_DATA *pRoom)
{
	RESET_DATA *pReset;
	CHAR_DATA *pMob;
	OBJ_DATA *pObj;
	OBJ_DATA *LastObj = NULL;
	OBJ_DATA *to_obj = NULL;
	AREA_DATA *pArea;
	EXIT_DATA *pexit;
	EXIT_DATA *rexit;
	int level = 0;
	int64 *plc = 0;

	if (!pRoom)
		return;

	if (!(pArea = pRoom->area))
		return;

	pMob = NULL;
	pObj = NULL;
	LastObj = NULL;

	for (auto* pexit : pRoom->exits)
	{
		if (!IS_SET(pexit->flags, EX_BASHED))
			pexit->flags = pexit->orig_flags;

		if ((pexit->to_room)
				&& ((rexit = get_exit(pexit->to_room, rev_dir[pexit->vdir]))))
			rexit->flags = rexit->orig_flags;
	}
	pexit = NULL;

	for (auto* pReset : pRoom->resets)
	{
		MOB_INDEX_DATA *pMobIndex;
		OBJ_INDEX_DATA *pObjIndex;
		OBJ_INDEX_DATA *pObjToIndex;
		ROOM_INDEX_DATA *pRoomIndex;

		switch (pReset->command)
		{
		default:
		{
			bug("bad command %c. (%s)", pReset->command, pArea->filename);
		}
			break;

			/* Mob To Room */
		case 'M':
		{
			if (!(pMobIndex = get_mob_index(pReset->arg1)))
			{
				bug("'M': bad mob vnum %d. (%s)", pReset->arg1,
						pArea->filename);
				continue;
			}

			if (!(pRoomIndex = find_room(pRoom, pReset->arg3)))
			{
				bug("'M': bad room vnum %d. (%s)", pReset->arg3,
						pArea->filename);
				continue;
			}

			// moby na nowych statkach sa poza tymi ograniczeniami
			if (!pRoomIndex->ship || !pRoomIndex->ship->vnum)
			{
				/* If sentinel, then maximum number indicates in room instead
				 of in world. -Kahn */
				/* Trog: not in room, but in area as of Dungal's request */
				if (IS_SET(pMobIndex->act, ACT_SENTINEL))
				{
					CHAR_DATA *ch;
					int count = 0;

					if (pRoomIndex->area)
						count = mobs_in_area(pMobIndex, pRoomIndex->area);
					else
						for (auto* ch : pRoomIndex->people)
							if ( IS_NPC( ch ) && ch->pIndexData == pMobIndex)
								count++;

					if (count >= pReset->arg2)
					{
						pMob = NULL;
						break;
					}
				}
				else if (pMobIndex->count >= pReset->arg2)
				{
					pMob = NULL;
					break;
				}
			}
			pMob = create_mobile(pMobIndex);

			if (pRoomIndex->ship && pRoomIndex->ship->vnum)
				pMobIndex->count--;

			//Added by Thanos : zapisuje wskaznik do quest mastera
			if (pMobIndex->vnum == MOB_VNUM_QUEST_MASTER)
				Quest_Master = pMob;

			if (room_is_dark(pRoomIndex))
				SET_BIT(pMob->affected_by, AFF_INFRARED);

			char_to_room(pMob, pRoomIndex);
			economize_mobgold(pMob);
			level = URANGE(0, pMob->top_level - 2, LEVEL_AVATAR);
			if (*pMob->s_vip_flags && pArea->planet)
				pArea->planet->population++;
		}
			break;

			/* Give Obj to Mob */
			/* Equip Mob */
		case 'G':
		case 'E':
		{
			if (!(pObjIndex = get_obj_index(pReset->arg1)))
			{
				bug("'E' or 'G': bad obj vnum %d. (%s)", pReset->arg1,
						pArea->filename);
				continue;
			}

			if (!pMob)
			{
				LastObj = NULL;
				break;
			}

			if (pMob->pIndexData->pShop)
			{
				int olevel = generate_itemlevel(pArea, pObjIndex);
				pObj = create_object(pObjIndex, olevel);
				SET_BIT(pObj->extra_flags, ITEM_INVENTORY);
			}
			else
				pObj = create_object(pObjIndex, number_fuzzy(level));
			pObj->level = URANGE(0, pObj->level, LEVEL_AVATAR);
			pObj = obj_to_char(pObj, pMob);

			if (pReset->command == 'E')
				equip_char(pMob, pObj, pReset->arg3);

			LastObj = pObj;
		}
			break;

			/* Obj to Room */
		case 'O':
		{
			if (!(pObjIndex = get_obj_index(pReset->arg1)))
			{
				bug("'O': bad obj vnum %d. (%s)", pReset->arg1,
						pArea->filename);
				continue;
			}

			if (!(pRoomIndex = find_room(pRoom, pReset->arg3)))
			{
				bug("'O': bad room vnum %d. (%s)", pReset->arg3,
						pArea->filename);
				continue;
			}

			pObj = NULL;
			/* Tanglor - to takie uproszecznie dal limitu obiekt�w
			 * count_obj_list jakos nie radzi sobie z tym limitem
			 * */
			pObj = NULL;
			for (auto* tObj : pRoomIndex->contents)
				if (tObj->pIndexData == pObjIndex
						&& tObj->count >= pReset->arg2)
				{ pObj = tObj; break; }
			if (pObj)
			{
				LastObj = pObj;
				to_obj = pObj;
				pObj = NULL;
				break;

			}

			if (count_obj_list(pObjIndex, pRoomIndex->contents)
					>= pReset->arg2)
			{
				/* Bugfix by Thanos: je�li obj jest ju� w pokoju, reset nie
				 �aduje go ju�. Niby ok, ale powinni�my zapami�ta� go jako
				 za�adowany, by poprawnie zadzia�a�y komendy 'P' */
				pObj = NULL;
				for (auto* tObj : pRoomIndex->contents)
					if (tObj->pIndexData == pObjIndex)
					{ pObj = tObj; break; }

				LastObj = pObj;
				to_obj = pObj;
				pObj = NULL;
				break;
			}

			pObj = create_object(pObjIndex,
					number_fuzzy(generate_itemlevel(pArea, pObjIndex)));
			pObj->level = UMIN(pObj->level, LEVEL_AVATAR);
			//pObj->cost 		= 0; //<-Po cholere to tutaj bylo?! - Ganis 
			obj_to_room(pObj, pRoomIndex);
			LastObj = pObj;
			to_obj = pObj;
		}
			break;

			/* Put Obj to Obj */
		case 'P':
		{
			if (!(pObjIndex = get_obj_index(pReset->arg1)))
			{
				bug("'P': bad obj vnum %d. (%s)", pReset->arg1,
						pArea->filename);
				continue;
			}
			if (pReset->arg3 > 0)
			{
				if (!(pObjToIndex = get_obj_index(pReset->arg3)))
				{
					bug("'P': bad objto vnum %d. (%s)", pReset->arg3,
							pArea->filename);
					continue;
				}

				if ((pArea->nplayer > 0 && !forceReset)
						//|| !(to_obj = get_obj_type(pObjToIndex))
						|| !to_obj->in_room
						|| count_obj_list(pObjIndex, to_obj->contents)
								>= pReset->arg2)
				{
					pObj = NULL;
					break;
				}
				LastObj = to_obj;
			}
			else
			{
				int iNest;

				if (!LastObj)
					break;

				to_obj = LastObj;

				for (iNest = 0; iNest < pReset->extra; iNest++)
					if (to_obj->contents.empty() || !(to_obj = to_obj->contents.back()))
					{
						bug("'P': Invalid nesting obj %d. (%s)", pReset->arg1,
								pArea->filename);
						iNest = -1;
						break;
					}

				if (iNest < 0)
					continue;
			}
			pObj = create_object(pObjIndex,
					number_fuzzy(
							UMAX(generate_itemlevel(pArea, pObjIndex),
									to_obj->level)));
			pObj->level = UMIN(pObj->level, LEVEL_AVATAR);
			obj_to_obj(pObj, to_obj);

			/*
			 * Ensure that the container gets reset.	OLC 1.1b
			 */
			if (to_obj->item_type == ITEM_CONTAINER)
			{
				to_obj->value[1] = to_obj->pIndexData->value[1];
			}
			else
			{
				/* THIS SPACE INTENTIONALLY LEFT BLANK */
			}
		}
			break;

			/* Trap */
		case 'T':
		{
			if (IS_SET(pReset->extra, TRAP_OBJ))
			{
				/* We need to preserve obj for future 'T' and 'H' checks */
				OBJ_DATA *pobj;

				if (pReset->arg3 > 0)
				{
					if (!(pObjToIndex = get_obj_index(pReset->arg3)))
					{
						bug("'T': bad objto vnum %d. (%s)", pReset->arg3,
								pArea->filename);
						continue;
					}

					if ((pArea->nplayer > 0 && !forceReset) || !(to_obj =
							get_obj_type(pObjToIndex))
							|| (to_obj->carried_by
									&& !IS_NPC(to_obj->carried_by))
							|| is_trapped(to_obj))
						break;
				}
				else
				{
					if (!LastObj || !pObj)
						break;

					to_obj = pObj;
				}

				pobj = make_trap(pReset->arg2, pReset->arg1,
						number_fuzzy(to_obj->level), pReset->extra);
				obj_to_obj(pobj, to_obj);
			}
			else
			{
				if (!(pRoomIndex = find_room(pRoom, pReset->arg3)))
				{
					bug("'T': bad room %d. (%s)", pReset->arg3,
							pArea->filename);
					continue;
				}

				if ((pArea->nplayer > 0 && !forceReset)
						|| count_obj_list(get_obj_index(OBJ_VNUM_TRAP),
								pRoomIndex->contents) > 0)
					break;

				to_obj = make_trap(pReset->arg1, pReset->arg1, 10,
						pReset->extra);
				obj_to_room(to_obj, pRoomIndex);
			}
		}
			break;

			/* Hide Obj */
		case 'H':
		{
			if (pReset->arg1 > 0)
			{
				if (!(pObjToIndex = get_obj_index(pReset->arg1)))
				{
					bug("'H': bad objto vnum %d. (%s)", pReset->arg1,
							pArea->filename);
					continue;
				}

				if ((pArea->nplayer > 0 && !forceReset) || !(to_obj =
						get_obj_type(pObjToIndex)) || !to_obj->in_room
						|| to_obj->in_room->area
								!= pArea || IS_OBJ_STAT(to_obj, ITEM_HIDDEN))
					break;
			}
			else
			{
				if (!LastObj || !pObj)
					break;

				to_obj = pObj;
			}
			SET_BIT(to_obj->extra_flags, ITEM_HIDDEN);
		}
			break;

			/* Bit Reset */
		case 'B':
		{
			switch (pReset->arg2 & BIT_RESET_TYPE_MASK)
			{
			case BIT_RESET_DOOR:
			{
				int doornum;

				if (!(pRoomIndex = find_room(pRoom, pReset->arg1)))
				{
					bug("'B': door: bad room vnum %d. (%s)", pReset->arg1,
							pArea->filename);
					continue;
				}
				doornum = (pReset->arg2 & BIT_RESET_DOOR_MASK)
						>> BIT_RESET_DOOR_THRESHOLD;

				if (!(pexit = get_exit(pRoomIndex, doornum)))
					break;

				plc = &pexit->orig_flags;
			}
				break;

			case BIT_RESET_ROOM:
			{
				if (!(pRoomIndex = find_room(pRoom, pReset->arg1)))
				{
					bug("'B': room: bad room vnum %d. (%s)", pReset->arg1,
							pArea->filename);
					continue;
				}
				plc = &pRoomIndex->room_flags;
			}
				break;

			case BIT_RESET_OBJECT:
			{
				if (pReset->arg1 > 0)
				{
					if (!(pObjToIndex = get_obj_index(pReset->arg1)))
					{
						bug("'B': object: bad objto vnum %d. (%s)",
								pReset->arg1, pArea->filename);
						continue;
					}

					if (!(to_obj = get_obj_type(pObjToIndex))
							|| !to_obj->in_room
							|| to_obj->in_room->area != pArea)
						continue;
				}
				else
				{
					if (!LastObj || !pObj)
						continue;

					to_obj = pObj;
				}
				plc = &to_obj->extra_flags;
			}
				break;

			case BIT_RESET_MOBILE:
			{
				if (!pMob)
					continue;
				plc = &pMob->affected_by;
			}
				break;

			default:
			{
				bug("'B': bad options %d. (%s)", pReset->arg2, pArea->filename);
			}
				continue;
			}

			if (IS_SET(pReset->arg2, BIT_RESET_SET))
				SET_BIT(*plc, pReset->arg3);
			else if (IS_SET(pReset->arg2, BIT_RESET_TOGGLE))
				TOGGLE_BIT(*plc, pReset->arg3);
			else
				REMOVE_BIT(*plc, pReset->arg3);
		}
			break;

			/* Door flag */
		case 'D':
		{
			if (!(pRoomIndex = find_room(pRoom, pReset->arg1)))
			{
				bug("'D': bad room vnum %d. (%s)", pReset->arg1,
						pArea->filename);
				continue;
			}

			if (!(pexit = get_exit(pRoomIndex, pReset->arg2)))
				break;

			switch (pReset->arg3)
			{
			case 0:
				REMOVE_BIT(pexit->orig_flags, EX_CLOSED);
				REMOVE_BIT(pexit->orig_flags, EX_LOCKED);
				break;
			case 1:
				SET_BIT(pexit->orig_flags, EX_CLOSED);
				REMOVE_BIT(pexit->orig_flags, EX_LOCKED);
				if (IS_SET(pexit->orig_flags, EX_xSEARCHABLE))
					SET_BIT(pexit->orig_flags, EX_SECRET);
				break;
			case 2:
				SET_BIT(pexit->orig_flags, EX_CLOSED);
				SET_BIT(pexit->orig_flags, EX_LOCKED);
				if (IS_SET(pexit->orig_flags, EX_xSEARCHABLE))
					SET_BIT(pexit->orig_flags, EX_SECRET);
				break;
			}
		}
			break;

			/* Randomize exits */
		case 'R':
		{
			if (!(pRoomIndex = find_room(pRoom, pReset->arg1)))
			{
				bug("'R': bad room vnum %d. (%s)", pReset->arg1,
						pArea->filename);
				continue;
			}

			randomize_exits(pRoomIndex, pReset->arg2 - 1);
		}
			break;

			/* Costruct ship in room  -- added by Thanos */
		case 'C':
		{
			SHIP_INDEX_DATA *shrec;
			SHIP_DATA *ship;
			int count;
			bool loop;

			if (!(shrec = get_ship_index(itoa(pReset->arg1))))
			{
				bug("'C': bad ship vnum %d. (%s)", pReset->arg1,
						pArea->filename);
				continue;
			}

			if (pRoom->ship && pRoom->ship->pIndexData
					&& pRoom->ship->pIndexData->vnum == shrec->vnum)
			{
				bug("'C': Ship #%d trying to load itself from reset. (%s)",
						pReset->arg1, pArea->filename);
				continue;
			}

			/*
			 * Zabezpieczenie przed zapetleniem.
			 * Dziala tak, ze jak statek jest zaladowany do lokacji innego
			 * statku, a ten statek z kolei stoi w innym statku, a ten..
			 * (chyba wiadomo o co chodzi)
			 * to ten stateczek najbardziej wewnatrz nie bedzie mogl
			 * zaladowac resetem ani jednego statku.
			 * Przeciwdziala takiej sytuacji:
			 * Statek A ma reset, ktory w pewnej lokacji laduje statek B.
			 * Statek B ma z kolei reset ladujacy do niego statek A.
			 * �adna p�telka i trudna do wykrycia (bo przecie� mo�na zaanga�owa�
			 * do niej jeszcze statek C), dlatego pozw�lmy na zagnie�dzenie do,
			 * powiedzmy, 3.
			 */
			count = 0;
			loop = false;
			for (pRoomIndex = pRoom;;)
			{
				if (count++ > 3)
				{
					loop = true;
					break;
				}

				if (pRoomIndex->ship && pRoomIndex->ship->in_room)
					pRoomIndex = pRoomIndex->ship->in_room;
				else
					break;
			}
			if (loop)
			{
				bug("'C': Ship #%d. Reset loop with other ship.(%s)",
						pReset->arg1, pArea->filename);
				continue;
			}

			if (!(pRoomIndex = find_room(pRoom, pReset->arg3)))
			{
				bug("'C': bad room vnum %d. (%s)", pReset->arg3,
						pArea->filename);
				continue;
			}

			// limity (tylko do lokacji)
			count = 0;
			for (auto* ship : pRoomIndex->ships)
			{
				if (ship->pIndexData == shrec)
					count++;
				if (count >= pReset->arg2)
				{
					shrec = NULL;
					break;
				}
			}
			if (!shrec)
				break;

			create_ship_in_room(shrec, pRoomIndex);
		}
			break;
		}
	}

	if (pRoom->room_flags & ROOM_STORAGE)
	{
		save_room_storage(pRoom);
	}
	pRoom = get_room_index( ROOM_VNUM_SCHOOL);
	if (pRoom && pArea == pRoom->area)
		pArea->age = UMAX(pArea->age, pArea->reset_frequency - 10);

	return;
}

/*
 * Reset one area.
 */
void reset_area(AREA_DATA *pArea)
{
	ROOM_INDEX_DATA *pRoom;
	int vnum;

	for (vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++)
	{
		if ((pRoom = get_room_index(vnum)))
			reset_room(pRoom);
	}
}

/* Separate function for recursive purposes */
void delete_reset(ROOM_INDEX_DATA *pRoom, RESET_DATA *pReset)
{
	auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), pReset);
	if (it != pRoom->resets.end())
		++it;

	if (pReset->command == 'M')
	{
		while (it != pRoom->resets.end())
		{
			RESET_DATA *reset = *it;

			/* Break when a new mob found */
			if (reset->command == 'M')
				break;

			/* Delete anything mob is holding */
			if (reset->command == 'G' || reset->command == 'E')
			{
				it = pRoom->resets.erase(it);
				delete_reset(pRoom, reset);
				continue;
			}

			if (reset->command == 'B'
					&& (reset->arg2 & BIT_RESET_TYPE_MASK) == BIT_RESET_MOBILE
					&& (!reset->arg1 || reset->arg1 == pReset->arg1))
			{
				it = pRoom->resets.erase(it);
				delete_reset(pRoom, reset);
				continue;
			}
			++it;
		}
	}
	else if (pReset->command == 'O' || pReset->command == 'P'
			|| pReset->command == 'G' || pReset->command == 'E')
	{
		while (it != pRoom->resets.end())
		{
			RESET_DATA *reset = *it;

			if (reset->command == 'T'
					&& (!reset->arg3 || reset->arg3 == pReset->arg1))
			{
				it = pRoom->resets.erase(it);
				delete_reset(pRoom, reset);
				continue;
			}

			if (reset->command == 'H'
					&& (!reset->arg1 || reset->arg1 == pReset->arg1))
			{
				it = pRoom->resets.erase(it);
				delete_reset(pRoom, reset);
				continue;
			}

			/* Delete nested objects, even if they are the same object. */
			if (reset->command == 'P'
					&& (reset->arg3 > 0 || pReset->command != 'P'
							|| reset->extra - 1 == pReset->extra)
					&& (!reset->arg3 || reset->arg3 == pReset->arg1))
			{
				it = pRoom->resets.erase(it);
				delete_reset(pRoom, reset);
				continue;
			}

			if (reset->command == 'B'
					&& (reset->arg2 & BIT_RESET_TYPE_MASK) == BIT_RESET_OBJECT
					&& (!reset->arg1 || reset->arg1 == pReset->arg1))
			{
				it = pRoom->resets.erase(it);
				delete_reset(pRoom, reset);
				continue;
			}

			/* Break when a new object of same type is found */
			if ((reset->command == 'O' || reset->command == 'P'
					|| reset->command == 'G' || reset->command == 'E')
					&& reset->arg1 == pReset->arg1)
				break;
			++it;
		}
	}

	pRoom->resets.remove(pReset);
	free_reset(pReset);
	return;
}

RESET_DATA* find_oreset(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom, char *name)
{
	RESET_DATA *reset = NULL;

	if (!*name)
	{
		for (auto it = pRoom->resets.rbegin(); it != pRoom->resets.rend(); ++it)
		{
			switch ((*it)->command)
			{
			default:
				continue;
			case 'O':
			case 'E':
			case 'G':
			case 'P':
				break;
			}
			reset = *it;
			break;
		}
		if (!reset)
			send_to_char("No object resets in list.\n\r", ch);

		return reset;
	}
	else
	{
		char arg[MAX_INPUT_LENGTH];
		int cnt = 0, num = number_argument(name, arg);
		OBJ_INDEX_DATA *pObjTo = NULL;

		for (auto* r : pRoom->resets)
		{
			switch (r->command)
			{
			default:
				continue;
			case 'O':
			case 'E':
			case 'G':
			case 'P':
				break;
			}

			if ((pObjTo = get_obj_index(r->arg1))
					&& is_name(arg, pObjTo->name) && ++cnt == num)
			{ reset = r; break; }
		}

		if (!pObjTo || !reset)
		{
			send_to_char("To object not in reset list.\n\r", ch);
			return NULL;
		}
	}

	return reset;
}

RESET_DATA* find_mreset(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom, char *name)
{
	RESET_DATA *reset = NULL;

	if (!*name)
	{
		for (auto it = pRoom->resets.rbegin(); it != pRoom->resets.rend(); ++it)
		{
			switch ((*it)->command)
			{
			default:
				continue;
			case 'M':
				break;
			}
			reset = *it;
			break;
		}
		if (!reset)
			send_to_char("No mobile resets in list.\n\r", ch);

		return reset;
	}
	else
	{
		char arg[MAX_INPUT_LENGTH];
		int cnt = 0, num = number_argument(name, arg);
		MOB_INDEX_DATA *pMob = NULL;

		for (auto* r : pRoom->resets)
		{
			switch (r->command)
			{
			default:
				continue;
			case 'M':
				break;
			}
			if ((pMob = get_mob_index(r->arg1))
					&& is_name(arg, pMob->player_name) && ++cnt == num)
			{ reset = r; break; }
		}

		if (!pMob || !reset)
		{
			send_to_char("Mobile not in reset list.\n\r", ch);
			return NULL;
		}
	}

	return reset;
}

DEF_DO_FUN( reset )
{
	ROOM_INDEX_DATA *pRoom;
	AREA_DATA *pArea;
	RESET_DATA *pReset;
	char arg1[ MAX_INPUT_LENGTH];
	char arg2[ MAX_INPUT_LENGTH];
	char arg3[ MAX_INPUT_LENGTH];
	char arg4[ MAX_INPUT_LENGTH];
	char arg5[ MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);
	argument = one_argument(argument, arg4);
	argument = one_argument(argument, arg5);

	pReset = NULL;
	pRoom = ch->in_room;
	pArea = pRoom->area;

	if (!is_builder(ch, pArea))
	{
		send_to_char("Invalid security for editing this area." NL, ch);
		return;
	}

	if (arg1[0] == '\0')
	{
		ch_printf(ch, "Syntax: reset remove <#>" NL);
		ch_printf(ch, "	reset room" NL);
		ch_printf(ch, "	reset area" NL);
		ch_printf(ch, "	reset list" NL);
		ch_printf(ch, "	reset showall" NL);
		ch_printf(ch, "		reset mobile <mob#> [limit]" NL);
		ch_printf(ch, "		reset object <obj#> [limit]" NL);
		ch_printf(ch, "		reset object <obj#> give <mob name> [limit]" NL);
		ch_printf(ch,
				"		reset object <obj#> equip <mob name> <location> [limit]" NL);
		ch_printf(ch, "		reset object <obj#> put <to_obj name> [limit]" NL);
		ch_printf(ch, "		reset ship <shp#> [limit]" NL);
		ch_printf(ch, "		reset insert <number> <command>" NL);

		return;
	}

	if (!str_cmp(arg1, "list"))
	{
		if (!pRoom->resets.empty())
		{
			display_resets(ch, false);
		}
		else
			send_to_char("There are no resets in this room." EOL, ch);

		return;
	}

	if (!str_cmp(arg1, "showall"))
	{
		display_resets(ch, true);
		return;
	}

	if (!str_cmp(arg1, "area"))
	{
		forceReset = true;
		reset_area(ch->in_room->area);
		forceReset = false;

		send_to_char("Area reset." EOL, ch);
		return;
	}

	if (!str_cmp(arg1, "room"))
	{
		forceReset = true;
		reset_room(ch->in_room);
		forceReset = false;

		send_to_char("Room reset." EOL, ch);
		return;
	}

	if (!str_cmp(arg1, "insert"))
	{
		RESET_DATA *reset;
		int num;
		char buf[MSL];

		if (!*arg2 || !is_number(arg2))
		{
			send_to_char("Usage: reset insert <number> <command>" NL, ch);
			return;
		}

		num = atoi(arg2);

		if ((reset = get_room_reset(pRoom, num)) == NULL)
		{
			send_to_char("Reset not found." NL, ch);
			return;
		}

		sprintf(buf, "%s %s %s %s", arg3, arg4, arg5, argument);

		if ((pReset = parse_reset(pRoom, buf, ch)) == NULL)
		{
			send_to_char("Error in reset. Reset not inserted." NL, ch);
			return;
		}
		{
			auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), reset);
			pRoom->resets.insert(it, pReset);
		}
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg1, "remove") || !str_cmp(arg1, "delete"))
	{
		int insert_loc = atoi(arg2);

		if (pRoom->resets.empty())
		{
			send_to_char("There are no resets in this room." NL, ch);
			return;
		}

		if (!(pReset = get_room_reset(pRoom, insert_loc)))
		{
			send_to_char("Reset not found." NL, ch);
			return;
		}

		delete_reset(pRoom, pReset);
		send_to_char("Reset deleted." NL, ch);
		return;
	}

	if (!str_prefix(arg1, "mobile"))
	{
		MOB_INDEX_DATA *pMob;
		int num;

		if (arg2[0] == '\0' || !is_number(arg2))
		{
			send_to_char("Reset which mobile vnum?" NL, ch);
			return;
		}

		if (!(pMob = get_mob_index(atoi(arg2))))
		{
			send_to_char("Mobile does not exist." NL, ch);
			return;
		}

		if (arg3[0] == '\0')
			num = 1;
		else if (!is_number(arg3))
		{
			send_to_char("Reset how many mobiles?" NL, ch);
			return;
		}
		else
			num = atoi(arg3);

		add_reset(pRoom, 'M', 0, pMob->vnum, num, pRoom->vnum);
		send_to_char("Reset added." NL, ch);
		return;
	}

	if (!str_prefix(arg1, "ship"))
	{
		SHIP_INDEX_DATA *shrec;
		int num;

		if (arg2[0] == '\0' || !is_number(arg2))
		{
			send_to_char("Reset which ship vnum?" NL, ch);
			return;
		}

		if (!(shrec = get_ship_index(arg2)))
		{
			send_to_char("Ship does not exist." NL, ch);
			return;
		}

		if (arg3[0] == '\0')
			num = 1;
		else if (!is_number(arg3))
		{
			send_to_char("Reset how many ships?" NL, ch);
			return;
		}
		else
			num = atoi(arg3);

		add_reset(pRoom, 'C', 0, shrec->vnum, num, pRoom->vnum);
		send_to_char("Reset added." NL, ch);
		return;
	}

	if (!str_prefix(arg1, "object"))
	{
		OBJ_INDEX_DATA *pObj;
		RESET_DATA *reset;
		int vnum;

		if (arg2[0] == '\0' || !is_number(arg2))
		{
			send_to_char("Reset which object vnum?" NL, ch);
			return;
		}

		if (!(pObj = get_obj_index(atoi(arg2))))
		{
			send_to_char("Object does not exist." NL, ch);
			return;
		}

		if (!str_cmp(arg3, "put"))
		{

			if (!(reset = find_oreset(ch, pRoom, arg4)))
				return;

			/* Put in_objects after hide and trap resets */
			{
				auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), reset);
				auto next_it = std::next(it);
				while (next_it != pRoom->resets.end()
						&& ((*next_it)->command == 'H'
								|| (*next_it)->command == 'T'
								|| (*next_it)->command == 'B'))
				{
					it = next_it;
					reset = *it;
					++next_it;
				}
			}

			if (atoi(arg5) < 1)
				vnum = 1;
			else
				vnum = atoi(arg5);

			pReset = make_reset('P', reset->extra + 1, pObj->vnum, vnum,
					reset->arg1);

			/* Insert after reset */
			{
				auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), reset);
				if (it != pRoom->resets.end()) ++it;
				pRoom->resets.insert(it, pReset);
			}
			send_to_char("Object reset in object created." NL, ch);
			return;
		}
		else if (!str_cmp(arg3, "give"))
		{
			if (!(reset = find_mreset(ch, pRoom, arg4)))
				return;

			{
				auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), reset);
				auto next_it = std::next(it);
				while (next_it != pRoom->resets.end() && (*next_it)->command == 'B')
				{ it = next_it; reset = *it; ++next_it; }
			}

			if ((vnum = atoi(arg4)) < 1)
				vnum = 1;

			pReset = make_reset('G', 1, pObj->vnum, vnum, 0);
			{
				auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), reset);
				if (it != pRoom->resets.end()) ++it;
				pRoom->resets.insert(it, pReset);
			}
			send_to_char("Object reset to mobile created." NL, ch);
			return;
		}
		else if (!str_cmp(arg3, "equip"))
		{
			int num;

			if (!(reset = find_mreset(ch, pRoom, arg4)))
				return;

			{
				auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), reset);
				auto next_it = std::next(it);
				while (next_it != pRoom->resets.end() && (*next_it)->command == 'B')
				{ it = next_it; reset = *it; ++next_it; }
			}

			num = flag_value(wear_types_list, arg5);

			if (num < 0)
			{
				send_to_char("Reset object to which location?" NL, ch);
				return;
			}

			{
				auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), reset);
				if (it != pRoom->resets.end()) ++it;
				for (; it != pRoom->resets.end(); ++it)
				{
					pReset = *it;
					if (pReset->command == 'M')
						break;

					if (pReset->command == 'E' && pReset->arg3 == num)
				{
					send_to_char(
							"Mobile already has an item equipped there." NL,
							ch);
					return;
				}
			}
			}

			if ((vnum = atoi(arg4)) < 1)
				vnum = 1;

			pReset = make_reset('E', 1, pObj->vnum, vnum, num);
			{
				auto it = std::find(pRoom->resets.begin(), pRoom->resets.end(), reset);
				if (it != pRoom->resets.end()) ++it;
				pRoom->resets.insert(it, pReset);
			}
			send_to_char("Object reset equipped by mobile created." NL, ch);
			return;
		}
		else
		{
			if ((vnum = atoi(arg3)) < 1)
				vnum = 1;

			pReset = make_reset('O', 0, pObj->vnum, vnum, pRoom->vnum);
			pRoom->resets.push_back(pReset);
			send_to_char("Object reset added." NL, ch);
			return;
		}
	}

	return;
}

/*
 * Create a new reset (for online building)			-Thoric
 */
RESET_DATA* make_reset(char letter, int extra, int arg1, int arg2, int arg3)
{
	RESET_DATA *pReset;

	CREATE(pReset, RESET_DATA, 1);
	pReset->command = letter;
	pReset->extra = extra;
	pReset->arg1 = arg1;
	pReset->arg2 = arg2;
	pReset->arg3 = arg3;
	top_reset++;
	return pReset;
}

/*
 * Add a reset to a room				-Thanos
 */
RESET_DATA* add_reset(ROOM_INDEX_DATA *room, char letter, int extra, int arg1,
		int arg2, int arg3)
{
	RESET_DATA *pReset;

	IF_BUG(room == NULL, "")
		return NULL;

	letter = UPPER(letter);
	pReset = make_reset(letter, extra, arg1, arg2, arg3);

	room->resets.push_back(pReset);
	return pReset;
}

RID* validate_reset(AREA_DATA *area, RESET_DATA *reset, int64 *iLastRoom,
		int64 *iLastObj)
{
	RID *pRoomIndex = NULL;
	ED *pexit;

	switch (reset->command)
	{
	default:
		bug("bad command %d '%c'.", reset->command, reset->command);
		return NULL;

	case 'M':
		if (!(pRoomIndex = get_room_tmp(reset->arg3, area)))
			return NULL;
		else
			*iLastRoom = reset->arg3;
		break;

	case 'O':
		if (!(pRoomIndex = get_room_tmp(reset->arg3, area)))
			return NULL;
		else
			*iLastObj = reset->arg3;
		break;

	case 'P':
		pRoomIndex = get_room_tmp(*iLastObj, area);
		break;

	case 'G':
	case 'E':
		pRoomIndex = get_room_tmp(*iLastRoom, area);
		break;

	case 'T':
	case 'H':
		pRoomIndex = get_room_tmp(*iLastObj, area);
		break;

	case 'D':
		if (!(pRoomIndex = get_room_tmp(reset->arg1, area)))
		{
			bug("'D': room %d doesn't exist.", reset->arg1);
			bug("Reset: %c %d %d %d %d", reset->command, reset->extra,
					reset->arg1, reset->arg2, reset->arg3);
			return NULL;
		}

		if (reset->arg2 < 0 || reset->arg2 > MAX_DIR+1
		|| !(pexit = get_exit(pRoomIndex, reset->arg2))
		|| !IS_SET( pexit->orig_flags, EX_ISDOOR ))
		{
			bug("'D': exit %d not door.", reset->arg2);
			bug("Reset: %c %d %d %d %d", reset->command, reset->extra,
					reset->arg1, reset->arg2, reset->arg3);
			return NULL;
		}

		if (reset->arg3 < 0 || reset->arg3 > 2)
		{
			bug(" 'D': bad 'locks': %d.", reset->arg3);
			return NULL;
		}
		break;

	case 'R':
		if (!(pRoomIndex = get_room_tmp(reset->arg1, area)))
		{
			bug("'R': room %d doesn't exist.", reset->arg1);
			return NULL;
		}

		if (reset->arg2 < 0 || reset->arg2 > 6)
		{
			bug("'R': bad exit %d.", reset->arg2);
			return NULL;
		}
		break;

		/* Thanos: �aduje statek do lokacji */
	case 'C':
		if (!(pRoomIndex = get_room_tmp(reset->arg3, area)))
		{
			bug("'C': room %d doesn't exist.", reset->arg3);
			return NULL;
		}
		else
			*iLastRoom = reset->arg3;
		break;
	}

	return pRoomIndex;
}

/* Uzywane przez findresets - Ganis */
enum LookingFor
{
	Mobile, Object
};

void find_resets_in_area(const AREA_DATA *const area, const int64 vnum,
		const LookingFor target, list<ROOM_INDEX_DATA*> &results)
{
	ROOM_INDEX_DATA *room;
	RESET_DATA *reset;
	bool loop_break = false;

	if (!area)
		return;
	if (vnum <= 0)
		return;

	for (int64 i = area->lvnum; i <= area->uvnum; ++i)
	{
		room = get_room_index(i);
		if (!room)
			continue;
		for (auto* reset : room->resets)
		{
			switch (reset->command)
			{
			default:
				continue;
			case 'M':
				if (target != Mobile)
					continue;
				if (reset->arg1 == vnum)
				{
					results.push_back(room);
					loop_break = true;
				}
				break;
			case 'O':
			case 'P':
			case 'E':
			case 'G':
				if (target != Object)
					continue;
				if (reset->arg1 == vnum)
				{
					results.push_back(room);
					loop_break = true;
				}
				break;
			}
			if (loop_break)
			{
				loop_break = false;
				break;
			}
		}
	}
}

/* Zeby posortowac wyniki findresets - Ganis */
bool room_comp(ROOM_INDEX_DATA *r1, ROOM_INDEX_DATA *r2)
{
	if (!r1)
		return true;
	if (!r2)
		return false;
	return r1->vnum < r2->vnum;
}

/*Znajduje resety dotyczace danego moba lub itemu - Ganis */
DEF_DO_FUN( findresets )
{
	char arg1[MIL];	//mob/obj
	char arg2[MIL];	//vnum
	char arg3[MIL];	//world? - domyslnie area
	int64 vnum;
	LookingFor target;
	list<ROOM_INDEX_DATA*> results;
	list<ROOM_INDEX_DATA*>::const_iterator it;

	if (!ch)
		return;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	if (!*arg1 || !*arg2 || !is_number(arg2))
	{
		send_to_char("Usage: findresets <mob|obj> <vnum> [world]" NL, ch);
		return;
	}

	vnum = atoi(arg2);

	if (vnum < 1)
	{
		send_to_char("Vnum must be a positive number." NL, ch);
		return;
	}

	if (!str_prefix(arg1, "mobile"))
	{
		target = Mobile;
		if (!get_mob_index(vnum))
		{
			send_to_char("There is no mobile with this vnum." NL, ch);
			return;
		}
		if (!is_builder(ch, get_vnum_area(vnum)))
		{
			send_to_char("You can't search for mobile that you can't edit." NL,
					ch);
			return;
		}

	}
	else if (!str_prefix(arg1, "object"))
	{
		target = Object;
		if (!get_obj_index(vnum))
		{
			send_to_char("There is no object with this vnum." NL, ch);
			return;
		}
		if (!is_builder(ch, get_vnum_area(vnum)))
		{
			send_to_char("You can't search for object that you can't edit." NL,
					ch);
			return;
		}
	}
	else
	{
		send_to_char("Looking for mobile or object?" NL, ch);
		send_to_char("Usage: findresets <mob|obj> <vnum> [world]" NL, ch);
		return;
	}

	//Lets look...
	if (!strcmp(arg3, "world"))
	{
		for (auto* area : area_list)
		{
			if (!is_builder(ch, area))
				continue;
			find_resets_in_area(area, vnum, target, results);
		}
	}
	else
	{
		if (is_builder(ch, ch->in_room->area))
			find_resets_in_area(ch->in_room->area, vnum, target, results);
		else
		{
			send_to_char(
					"You can't search resets in area that you can't edit." NL,
					ch);
			return;
		}
	}

	results.sort(room_comp);

	//Display results
	pager_printf(ch,
			FB_CYAN "[" FB_YELLOW "%-6s" FB_CYAN "][" FB_YELLOW "%-44s" FB_CYAN "][" FB_YELLOW "%-30s" FB_CYAN "]" NL,
			"Vnum", "Room name", "Area name");
	for (it = results.begin(); it != results.end(); ++it)
	{
		pager_printf(ch,
				FB_CYAN "[" PLAIN "%6d" FB_CYAN "][" PLAIN "%-44s" FB_CYAN "][" PLAIN "%-30s" FB_CYAN "]" NL,
				(*it)->vnum, strip_colors((*it)->name, 44), (*it)->area->name);
	}
}

