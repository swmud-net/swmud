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
 *			Specific object creation module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <time.h>
#include "mud.h"
#include "defines.h"
#include "classes/SWBounty.h"

/*
 * Make a trap.
 */
OBJ_DATA *make_trap(int v0, int v1, int v2, int v3)
{
	OBJ_DATA *trap;

	trap = create_object(get_obj_index(OBJ_VNUM_TRAP), 0);
	trap->timer = 0;
	trap->value[0] = v0;
	trap->value[1] = v1;
	trap->value[2] = v2;
	trap->value[3] = v3;
	return trap;
}

/*
 * Turn an object into scraps.		-Thoric
 */
void make_scraps(OBJ_DATA *obj)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *scraps, *tmpobj;
	CHAR_DATA *ch = NULL;
	int i;

	separate_obj(obj);
	scraps = create_object(get_obj_index(OBJ_VNUM_SCRAPS), 0);
	scraps->timer = number_range(5, 15);

	/* don't make scraps of scraps of scraps of ... */
	if (obj->pIndexData->vnum == OBJ_VNUM_SCRAPS)
	{
		STRDUP( scraps->przypadki[0], "szcz�tki" );
		STRDUP( scraps->przypadki[1], "szcz�tek" );
		STRDUP( scraps->przypadki[2], "szcz�tkom" );
		STRDUP( scraps->przypadki[3], "szcz�tki" );
		STRDUP( scraps->przypadki[4], "szcz�tkami" );
		STRDUP( scraps->przypadki[5], "szcz�tkach" );
		STRDUP( scraps->description , "Le�� tu niezydentyfikowane szcz�tki." );
	}
	else
	{
		for (i = 0; i < 6; i++)
		{
			sprintf(buf, scraps->przypadki[i], obj->przypadki[1]);
			STRDUP( scraps->przypadki[i], buf );
		}
		sprintf(buf, scraps->description, obj->przypadki[1]);
		STRDUP( scraps->description, buf );
	}

	if (obj->carried_by)
	{
		act(COL_OBJECT, "$p rozpada si� na szcz�tki!", obj->carried_by, obj,
				NULL, TO_CHAR);
		if (obj == get_eq_char(obj->carried_by, WEAR_WIELD) && (tmpobj
				= get_eq_char(obj->carried_by, WEAR_DUAL_WIELD)) != NULL)
			tmpobj->wear_loc = WEAR_WIELD;

		obj_to_room(scraps, obj->carried_by->in_room);
	}
	else if (obj->in_room)
	{
		if ((ch = obj->in_room->people.empty() ? nullptr : obj->in_room->people.front()) != NULL)
		{
			act(COL_OBJECT, "$p rozpada si� na szcz�tki!", ch, obj, NULL,
					TO_ROOM);
			act(COL_OBJECT, "$p rozpada si� na szcz�tki!", ch, obj, NULL,
					TO_CHAR);
		}
		obj_to_room(scraps, obj->in_room);
	}
	if ((obj->item_type == ITEM_CONTAINER || obj->item_type == ITEM_CORPSE_PC)
			&& !obj->contents.empty())
	{
		if (ch && ch->in_room)
		{
			act(COL_OBJECT, "Zawarto�� $p$1 spada na ziemi�.", ch, obj, NULL,
					TO_ROOM);
			act(COL_OBJECT, "Zawarto�� $p$1 spada na ziemi�.", ch, obj, NULL,
					TO_CHAR);
		}
		if (obj->carried_by)
			empty_obj(obj, NULL, obj->carried_by->in_room);
		else if (obj->in_room)
			empty_obj(obj, NULL, obj->in_room);
		else if (obj->in_obj)
			empty_obj(obj, obj->in_obj, NULL);
	}
	extract_obj(obj);
}

/*
 * Make a corpse out of a character.
 */
void make_corpse(CHAR_DATA *ch, CHAR_DATA *killer, int suicide)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *corpse;
	char *name;
	char *pname;
	int i;

	if (IS_NPC(ch))
	{
		if (IS_SET ( ch->act , ACT_DROID ))
			corpse = create_object(get_obj_index(OBJ_VNUM_DROID_CORPSE), 0);
		else
			corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_NPC), 0);

		name = ch->przypadki[1];

		if (ch->gold > 0 && (!suicide || !IS_NEWBIE( ch )))
		{
			if (ch->in_room)
				ch->in_room->area->gold_looted += ch->gold;
			obj_to_obj(create_money(ch->gold), corpse);
			ch->gold = 0;
		}

		/* Cannot use these!  They are used.
		 corpse->value[0] = (int)ch->pIndexData->vnum;
		 corpse->value[1] = (int)ch->max_hit;
		 */
		/*	Using corpse cost to cheat, since corpses not sellable */
		corpse->cost = -ch->pIndexData->vnum;
		corpse->gender = GENDER_NEUTRAL;
		corpse->value[2] = corpse->timer;

		/* Thanos 	-- niewidzialne poza questem zw�oki */
		if (ch->inquest)
		{
			QUEST_OBJ_DATA * qObj;
			QUEST_DATA * quest = ch->inquest;

			/* zw�oki odziedziczaj� w�a�ciwo�ci questowe postaci ;) */
			corpse->inquest = quest;
			CREATE( qObj, QUEST_OBJ_DATA, 1 );
			qObj->obj = corpse;
			quest->objs.push_back(qObj);

			corpse->timer = 10;
		}
		else
			corpse->timer = 6;

		/* Added corpse name - make locate easier , other skills */
		sprintf(buf, "cia�o %s corpse m%d", name, ch->pIndexData->vnum);
		STRDUP( corpse->name, buf );
	}
	else
	{
		name = ch->przypadki[1];
		pname = ch->name;
		corpse = create_object(get_obj_index(OBJ_VNUM_CORPSE_PC), 0);
		corpse->timer = 40;
		corpse->value[2] = (int) (corpse->timer / 8);
		corpse->value[3] = 0;
		if (ch->gold > 0)
		{
			if (ch->in_room)
				ch->in_room->area->gold_looted += ch->gold;
			obj_to_obj(create_money(ch->gold), corpse);
			ch->gold = 0;
		}

		STRDUP( corpse->action_desc, pname );

		/* Added corpse name - make locate easier , other skills */
		sprintf(buf, "cia�o %s corpse %s", name, ch->name);
		STRDUP( corpse->name, buf );
	}

	for (i = 0; i < 6; i++)
	{
		sprintf(buf, corpse->przypadki[i], name);
		STRDUP( corpse->przypadki[i], buf );
	}

	sprintf(buf, corpse->description, name);
	STRDUP( corpse->description, buf );

	auto carrying_snapshot = ch->carrying;
	for (auto* obj : carrying_snapshot)
	{
		obj_from_char(obj);
		if (IS_OBJ_STAT( obj, ITEM_INVENTORY ))
			extract_obj(obj);
		else if (!suicide || !IS_NEWBIE( ch ))
			obj_to_obj(obj, corpse);
	}

	/* Trog: jesli to jest cialo dla istoty bedacej podmiotem bounty,
	 * to troche szlifujemy timer  i pozwalamy podniesc takie cialo.
	 * W koncu gracz musi je jakos zatargac do oficera bounty.
	 * Dodatkowy zapisujemy przedmiotowi imie gracza, ktory zabil
	 * ofiare oraz wyznaczamy expa. */
	SWBounty *bounty;
	if (!IS_NPC(killer) && (bounty = is_bounty_victim_killed(ch)))
	{
		corpse->timer = 1000000;
		SET_BIT(corpse->wear_flags,ITEM_TAKE);
		STRDUP(corpse->owner_name,killer->name);
		corpse->value[5] = get_bounty_xp(ch, killer);
	}

	obj_to_room(corpse, ch->in_room);
}

/*
 * make some coinage
 */
OBJ_DATA *create_money(int amount)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;

	if (amount <= 0)
	{
		bug( "zero or negative money %d.", amount );
		amount = 1;
	}

	if (amount == 1)
	{
		obj = create_object(get_obj_index(OBJ_VNUM_MONEY_ONE), 0);
	}
	else
	{
		int i;
		obj = create_object(get_obj_index(OBJ_VNUM_MONEY_SOME), 0);
		for (i = 0; i < 6; i++)
		{
			sprintf(buf, obj->przypadki[i], amount);
			STRDUP( obj->przypadki[i], buf );
		}
		obj->value[0] = amount;
	}

	return obj;
}
