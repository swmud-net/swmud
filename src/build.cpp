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
 *		       Online Building and Editing Module		   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "mud.h"

extern int top_affect;
extern int top_reset;
extern int top_ed;
extern bool fBootDb;

bool can_rmodify(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
{
	int vnum = room->vnum;
	AREA_DATA *pArea;

	if (IS_NPC(ch))
		return false;
	if (get_trust(ch) >= sysdata.level_modify_proto)
		return true;
	if (!ch->pcdata || !(pArea = ch->pcdata->area))
	{
		send_to_char("You must have an assigned area to modify this room." NL,
				ch);
		return false;
	}
	if (vnum >= pArea->lvnum && vnum <= pArea->uvnum)
		return true;

	send_to_char("That room is not in your allocated range." NL, ch);
	return false;
}

bool can_omodify(CHAR_DATA *ch, OBJ_DATA *obj)
{
	int vnum = obj->pIndexData->vnum;
	AREA_DATA *pArea;

	if (IS_NPC(ch))
		return false;

	if (get_trust(ch) >= sysdata.level_modify_proto)
		return true;

	if (!ch->pcdata || !(pArea = ch->pcdata->area))
	{
		send_to_char("You must have an assigned area to modify this object." NL,
				ch);
		return false;
	}

	if (vnum >= pArea->lvnum && vnum <= pArea->uvnum)
		return true;

	send_to_char("That object is not in your allocated range." NL, ch);
	return false;
}

bool can_oedit(CHAR_DATA *ch, OBJ_INDEX_DATA *obj)
{
	int vnum = obj->vnum;
	AREA_DATA *pArea;

	if (IS_NPC(ch))
		return false;
	if (get_trust(ch) >= LEVEL_GOD)
		return true;
	if (!ch->pcdata || !(pArea = ch->pcdata->area))
	{
		send_to_char("You must have an assigned area to modify this object." NL,
				ch);
		return false;
	}
	if (vnum >= pArea->lvnum && vnum <= pArea->uvnum)
		return true;

	send_to_char("That object is not in your allocated range." NL, ch);
	return false;
}

bool can_mmodify(CHAR_DATA *ch, CHAR_DATA *mob)
{
	int vnum;
	AREA_DATA *pArea;

	if (mob == ch)
		return true;

	if (!IS_NPC(mob))
	{
		if (get_trust(ch) >= sysdata.level_modify_proto
				&& get_trust(ch) > get_trust(mob))
			return true;
		else
			send_to_char("You can't do that." NL, ch);
		return false;
	}

	vnum = mob->pIndexData->vnum;

	if (IS_NPC(ch))
		return false;
	if (get_trust(ch) >= sysdata.level_modify_proto)
		return true;
	if (!ch->pcdata || !(pArea = ch->pcdata->area))
	{
		send_to_char("You must have an assigned area to modify this mobile." NL,
				ch);
		return false;
	}
	if (vnum >= pArea->lvnum && vnum <= pArea->uvnum)
		return true;

	send_to_char("That mobile is not in your allocated range." NL, ch);
	return false;
}

bool can_medit(CHAR_DATA *ch, MOB_INDEX_DATA *mob)
{
	int vnum = mob->vnum;
	AREA_DATA *pArea;

	if (IS_NPC(ch))
		return false;
	if (get_trust(ch) >= LEVEL_GOD)
		return true;
	if (!ch->pcdata || !(pArea = ch->pcdata->area))
	{
		send_to_char("You must have an assigned area to modify this mobile." NL,
				ch);
		return false;
	}
	if (vnum >= pArea->lvnum && vnum <= pArea->uvnum)
		return true;

	send_to_char("That mobile is not in your allocated range." NL, ch);
	return false;
}

/*
 * Returns value 0 - 9 based on directional text.
 */
int get_dir(char *txt)
{
	int edir;
	char c1, c2;

	if (!str_cmp(txt, "northeast"))
		return DIR_NORTHEAST;
	if (!str_cmp(txt, "northwest"))
		return DIR_NORTHWEST;
	if (!str_cmp(txt, "southeast"))
		return DIR_SOUTHEAST;
	if (!str_cmp(txt, "southwest"))
		return DIR_SOUTHWEST;
	if (!str_cmp(txt, "somewhere"))
		return 10;

	c1 = txt[0];
	if (c1 == '\0')
		return 0;
	c2 = txt[1];
	edir = 0;
	switch (c1)
	{
	case 'n':
		switch (c2)
		{
		default:
			edir = 0;
			break; /* north */
		case 'e':
			edir = 6;
			break; /* ne	 */
		case 'w':
			edir = 7;
			break; /* nw	 */
		}
		break;
	case '0':
		edir = 0;
		break; /* north */
	case 'e':
	case '1':
		edir = 1;
		break; /* east  */
	case 's':
		switch (c2)
		{
		default:
			edir = 2;
			break; /* south */
		case 'e':
			edir = 8;
			break; /* se	 */
		case 'w':
			edir = 9;
			break; /* sw	 */
		}
		break;
	case '2':
		edir = 2;
		break; /* south */
	case 'w':
	case '3':
		edir = 3;
		break; /* west	 */
	case 'u':
	case '4':
		edir = 4;
		break; /* up	 */
	case 'd':
	case '5':
		edir = 5;
		break; /* down	 */
	case '6':
		edir = 6;
		break; /* ne	 */
	case '7':
		edir = 7;
		break; /* nw	 */
	case '8':
		edir = 8;
		break; /* se	 */
	case '9':
		edir = 9;
		break; /* sw	 */
	case '?':
		edir = 10;
		break; /* somewhere */
	}
	return edir;
}

/*
 * Remove carriage returns from a line
 */
char* strip_cr(char *str)
{
	static char newstr[MAX_STRING_LENGTH];
	int i, j;

	for (i = j = 0; str[i] != '\0'; i++)
		if (str[i] != '\r')
		{
			newstr[j++] = str[i];
		}
	newstr[j] = '\0';
	return newstr;
}

DEF_DO_FUN( goto )
{
	char arg[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *in_room;
	AREA_DATA *pArea;
	int vnum;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Goto where?" NL, ch);
		return;
	}

	if ((location = find_location(ch, arg)) == NULL)
	{
		vnum = atoi(arg);
		if (vnum < 0 || get_room_index(vnum))
		{
			send_to_char("You cannot find that..." NL, ch);
			return;
		}

		if (vnum < 1 || IS_NPC(ch) || !ch->pcdata->area)
		{
			send_to_char("No such location." NL, ch);
			return;
		}
		if (get_trust(ch) < sysdata.level_modify_proto)
		{
			if (!ch->pcdata || !(pArea = ch->pcdata->area))
			{
				send_to_char(
						"You must have an assigned area to create rooms." NL,
						ch);
				return;
			}
			if (vnum < pArea->lvnum || vnum > pArea->uvnum)
			{
				send_to_char("That room is not within your assigned range." NL,
						ch);
				return;
			}
		}
		location = make_room(vnum, 0);
		if (!location)
		{
			bug("make_room failed");
			return;
		}
		location->area = ch->pcdata->area;
		send_to_char( FB_WHITE
		"Machasz r�k� i tworzysz porz�dek z czystego chaosu," NL
		"wkraczaj�c w now� rzeczywisto��..." EOL, ch);
	}

	if (room_is_private(ch, location) && !is_invited(ch, location))
	{
		if (get_trust(ch) < sysdata.level_override_private)
		{
			send_to_char("That room is private right now." NL, ch);
			return;
		}
		else
			send_to_char("Overriding private flag!" NL, ch);
	}

	if (get_trust(ch) < LEVEL_IMMORTAL)
	{
		vnum = atoi(arg);

		if (!ch->pcdata || !(pArea = ch->pcdata->area))
		{
			send_to_char("You must have an assigned area to goto." NL, ch);
			return;
		}

		if (vnum < pArea->lvnum || vnum > pArea->uvnum)
		{
			send_to_char("That room is not within your assigned range." NL, ch);
			return;
		}

		if ((ch->in_room->vnum < pArea->lvnum
				|| ch->in_room->vnum > pArea->uvnum)
				&& !IS_SET(ch->in_room->room_flags, ROOM_HOTEL))
		{
			send_to_char(
					"Builders can only use goto from a hotel or in their zone." NL,
					ch);
			return;
		}

	}

	in_room = ch->in_room;
	if (ch->fighting)
		stop_fighting(ch, true);

	if (!IS_SET(ch->act, PLR_WIZINVIS))
	{
		if (ch->pcdata && ch->pcdata->bamfout[0] != '\0')
			act( FB_WHITE, "$T", ch, NULL, ch->pcdata->bamfout, TO_ROOM);
		else
			act( FB_WHITE, "$n $T", ch, NULL, "znika w zawirowaniu MOCY.",
			TO_ROOM);
	}
	ch->regoto = ch->in_room->vnum;
	uncrew(ship_from_room(ch->in_room), ch);
	char_from_room(ch);
	if (ch->mount)
	{
		char_from_room(ch->mount);
		char_to_room(ch->mount, location);
	}
	char_to_room(ch, location);

	if (!IS_SET(ch->act, PLR_WIZINVIS))
	{
		if (ch->pcdata && ch->pcdata->bamfin[0] != '\0')
			act( COL_IMMORT, "$T", ch, NULL, ch->pcdata->bamfin, TO_ROOM);
		else
			act( COL_IMMORT, "$n $T", ch, NULL,
					"pojawia si� tu a Moc wiruje doko�a.", TO_ROOM);
	}

	do_look(ch, (char*) "auto");

	if (ch->in_room == in_room)
		return;
	for (auto* fch : in_room->people)
	{
		if (fch->master == ch && IS_IMMORTAL(fch))
		{
			act( COL_ACTION, "Pod��asz za $N$4.", fch, NULL, ch, TO_CHAR);
			do_goto(fch, argument);
		}
	}
	return;
}

DEF_DO_FUN( shgoto )
{
	char arg[MAX_INPUT_LENGTH];
	SHIP_DATA *ship;
	ROOM_INDEX_DATA *location = 0;

	argument = one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("syntax: shgoto <ship> [cockpit|entrance]" NL, ch);
		return;
	}
	ship = get_ship(arg);

	if (!ship)
	{
		send_to_char("No such ship." NL, ch);
		return;
	}

	if (!*argument || !str_cmp(argument, "cockpit"))
		location = ship->cockpit;

	if (!str_cmp(argument, "entrance"))
		location = ship->entrance;

	if (location == NULL)
	{
		if (ship->locations.empty() || (location = ship->locations.front()) == NULL)
		{
			send_to_char("Ship has no location." NL, ch);
			return;
		}
	}

	if (room_is_private(ch, location))
	{
		if (get_trust(ch) < sysdata.level_override_private)
		{
			send_to_char("That room is private right now." NL, ch);
			return;
		}
		else
			send_to_char("Overriding private flag!" NL, ch);
	}

	if (ch->fighting)
		stop_fighting(ch, true);

	if (!IS_SET(ch->act, PLR_WIZINVIS))
	{
		if (ch->pcdata && ch->pcdata->bamfout[0] != '\0')
			act( FB_WHITE, "$T", ch, NULL, ch->pcdata->bamfout, TO_ROOM);
		else
			act( FB_WHITE, "$n $T", ch, NULL, "znika w zawirowaniu MOCY.",
			TO_ROOM);
	}

	uncrew(ship_from_room(ch->in_room), ch);
	char_from_room(ch);
	if (ch->mount)
	{
		char_from_room(ch->mount);
		char_to_room(ch->mount, location);
	}
	char_to_room(ch, location);

	if (!IS_SET(ch->act, PLR_WIZINVIS))
	{
		if (ch->pcdata && ch->pcdata->bamfin[0] != '\0')
			act( COL_IMMORT, "$T", ch, NULL, ch->pcdata->bamfin, TO_ROOM);
		else
			act( COL_IMMORT, "$n $T", ch, NULL,
					"pojawia si� tu a Moc wiruje doko�a.", TO_ROOM);
	}

	do_look(ch, (char*) "auto");
	return;
}

DEF_DO_FUN( mset )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char outbuf[MAX_STRING_LENGTH];
	int num, size, plus;
	char char1, char2;
	CHAR_DATA *victim;
	int value;
	int minattr, maxattr;
	char *origarg = argument;

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	if (!ch->desc)
	{
		send_to_char("You have no descriptor" NL, ch);
		return;
	}

	victim = NULL;
	smash_tilde(argument);
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	strcpy(arg3, argument);

	if (arg1[0] == '\0' || !str_cmp(arg1, "?"))
	{
		if (ch->substate == SUB_REPEATCMD)
		{
			if (victim)
				send_to_char("Syntax: <field>  <value>" NL, ch);
			else
				send_to_char("Syntax: <victim> <field>  <value>" NL, ch);
		}
		else
			send_to_char("Syntax: mset <victim> <field>  <value>" NL, ch);
		send_to_char("" NL, ch);
		send_to_char("Field being one of:" NL, ch);
		send_to_char("  str int wis dex con cha lck frc sex" NL, ch);
		send_to_char("  credits hp force move align race" NL, ch);
		send_to_char("  hitroll damroll armor affected level" NL, ch);
		send_to_char("  thirst drunk full blood flags security" NL, ch);
		send_to_char("  pos defpos part (see BODYPARTS)" NL, ch);
		send_to_char("  sav1 sav2 sav4 sav4 sav5 (see SAVINGTHROWS)" NL, ch);
		send_to_char("  resistant immune susceptible (see RIS)" NL, ch);
		send_to_char("  attack defense numattacks" NL, ch);
		send_to_char("  speaking speaks (see LANGUAGES)" NL, ch);
		send_to_char("  name short long description title spec spec2" NL, ch);
		send_to_char("  clan vip wanted" NL, ch);
		send_to_char("  attribute kin" NL, ch);
		send_to_char("" NL, ch);
		send_to_char("For editing index/prototype mobiles:" NL, ch);
		send_to_char("  hitnumdie hitsizedie hitplus (hit points)" NL, ch);
		send_to_char("  damnumdie damsizedie damplus (damage roll)" NL, ch);
		send_to_char("To toggle area flag: aloaded" NL, ch);
		return;
	}

	if (get_trust(ch) <= LEVEL_IMMORTAL)
	{
		if ((victim = get_char_room(ch, arg1)) == NULL)
		{
			send_to_char("They aren't here." NL, ch);
			return;
		}
	}
	else if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("No one like that in all the realms." NL, ch);
		return;
	}

	if (get_trust(ch) <= LEVEL_IMMORTAL && !IS_NPC(victim))
	{
		send_to_char("You can't do that!" NL, ch);
		return;
	}
	if (get_trust(ch) < get_trust(victim) && !IS_NPC(victim))
	{
		send_to_char("You can't do that!" NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		minattr = 1;
		maxattr = 25;
	}
	else
	{
		minattr = 3;
		maxattr = 35;
	}

	value = is_number(arg3) ? atoi(arg3) : -1;

	if (atoi(arg3) < -1 && value == -1)
		value = atoi(arg3);

	if (!str_cmp(arg2, "str"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < minattr || value > maxattr)
		{
			ch_printf(ch, "Strength range is %d to %d." NL, minattr, maxattr);
			return;
		}
		victim->perm_str = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->perm_str = value;
		return;
	}

	if (!str_cmp(arg2, "int"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < minattr || value > maxattr)
		{
			ch_printf(ch, "Intelligence range is %d to %d." NL, minattr,
					maxattr);
			return;
		}
		victim->perm_int = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->perm_int = value;
		return;
	}

	if (!str_cmp(arg2, "wis"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < minattr || value > maxattr)
		{
			ch_printf(ch, "Wisdom range is %d to %d." NL, minattr, maxattr);
			return;
		}
		victim->perm_wis = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->perm_wis = value;
		return;
	}

	if (!str_cmp(arg2, "dex"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < minattr || value > maxattr)
		{
			ch_printf(ch, "Dexterity range is %d to %d." NL, minattr, maxattr);
			return;
		}
		victim->perm_dex = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->perm_dex = value;
		return;
	}

	if (!str_cmp(arg2, "con"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < minattr || value > maxattr)
		{
			ch_printf(ch, "Constitution range is %d to %d." NL, minattr,
					maxattr);
			return;
		}
		victim->perm_con = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->perm_con = value;
		return;
	}

	if (!str_cmp(arg2, "cha"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < minattr || value > maxattr)
		{
			ch_printf(ch, "Charisma range is %d to %d." NL, minattr, maxattr);
			return;
		}
		victim->perm_cha = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->perm_cha = value;
		return;
	}

	if (!str_cmp(arg2, "lck"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < minattr || value > maxattr)
		{
			ch_printf(ch, "Luck range is %d to %d." NL, minattr, maxattr);
			return;
		}
		victim->perm_lck = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->perm_lck = value;
		return;
	}

	if (!str_cmp(arg2, "frc"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > maxattr)
		{
			ch_printf(ch, "Frc range is 0 to %d." NL, maxattr);
			return;
		}
		victim->perm_frc = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->perm_frc = value;
		return;
	}

	if (!str_cmp(arg2, "sav1"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < -30 || value > 30)
		{
			send_to_char("Saving throw range vs poison is -30 to 30." NL, ch);
			return;
		}
		victim->saving_poison_death = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->saving_poison_death = value;
		return;
	}

	if (!str_cmp(arg2, "sav2"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < -30 || value > 30)
		{
			send_to_char("Saving throw range vs wands is -30 to 30." NL, ch);
			return;
		}
		victim->saving_wand = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->saving_wand = value;
		return;
	}

	if (!str_cmp(arg2, "sav3"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < -30 || value > 30)
		{
			send_to_char("Saving throw range vs para is -30 to 30." NL, ch);
			return;
		}
		victim->saving_para_petri = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->saving_para_petri = value;
		return;
	}

	if (!str_cmp(arg2, "sav4"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < -30 || value > 30)
		{
			send_to_char("Saving throw range vs bad breath is -30 to 30." NL,
					ch);
			return;
		}
		victim->saving_breath = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->saving_breath = value;
		return;
	}

	if (!str_cmp(arg2, "sav5"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < -30 || value > 30)
		{
			send_to_char("Saving throw range vs force powers is -30 to 30." NL,
					ch);
			return;
		}
		victim->saving_spell_staff = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->saving_spell_staff = value;
		return;
	}

	if (!str_cmp(arg2, "sex"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 2)
		{
			send_to_char("Sex range is 0 to 2." NL, ch);
			return;
		}
		victim->sex = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->sex = value;
		return;
	}

	if (!str_cmp(arg2, "race"))
	{
		RACE_DATA *race;
		if (!can_mmodify(ch, victim))
			return;
		if (!(race = find_race(arg3)))
		{
			ch_printf(ch, "No such race: %s.\n", arg3);
			return;
		}
		victim->race = race;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->race = race;
		return;
	}

	if (!str_cmp(arg2, "armor"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < -300 || value > 300)
		{
			send_to_char("AC range is -300 to 300." NL, ch);
			return;
		}
		victim->armor = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->ac = value;
		return;
	}

	if (!str_cmp(arg2, "level"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (!IS_NPC(victim))
		{
			send_to_char("Not on PC's." NL, ch);
			return;
		}

		if (value < 0 || value > LEVEL_AVATAR + 5)
		{
			ch_printf(ch, "Level range is 0 to %d." NL, LEVEL_AVATAR + 5);
			return;
		}
		{
			int ability;
			for (ability = 0; ability < MAX_ABILITY; ability++)
				victim->skill_level[ability] = value;
		}
		victim->top_level = value;
		victim->armor = 0 - value * 2.5;
		victim->hitroll = value / 5;
		victim->damroll = value / 5;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
		{
			victim->pIndexData->level = value;
			victim->pIndexData->ac = 0 - value * 2.5;
			victim->pIndexData->hitroll = victim->hitroll;
			victim->pIndexData->damroll = victim->damroll;
		}
		sprintf(outbuf, "%s damnumdie %d", arg1, value / 10);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s damsizedie %d", arg1, 4);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s damplus %d", arg1, 2);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s hitnumdie %d", arg1, value / 5);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s hitsizedie %d", arg1, 10);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s hitplus %d", arg1, value * 10);
		do_mset(ch, outbuf);

		return;
	}

	if (!str_cmp(arg2, "numattacks"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (!IS_NPC(victim))
		{
			send_to_char("Not on PC's." NL, ch);
			return;
		}

		if (value < 0 || value > 20)
		{
			send_to_char("Attacks range is 0 to 20." NL, ch);
			return;
		}
		victim->numattacks = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->numattacks = value;
		return;
	}

	if (!str_cmp(arg2, "credits"))
	{
		if (!can_mmodify(ch, victim))
			return;
		victim->gold = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->gold = value;
		return;
	}

	if (!str_cmp(arg2, "hitroll"))
	{
		if (!can_mmodify(ch, victim))
			return;
		victim->hitroll = URANGE(0, value, 85);
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->hitroll = victim->hitroll;
		return;
	}

	if (!str_cmp(arg2, "damroll"))
	{
		if (!can_mmodify(ch, victim))
			return;
		victim->damroll = URANGE(0, value, 65);
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->damroll = victim->damroll;
		return;
	}

	if (!str_cmp(arg2, "hp"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < 1 || value > 32700)
		{
			send_to_char("Hp range is 1 to 32,700 hit points." NL, ch);
			return;
		}
		victim->max_hit = value;
		return;
	}

	if (!str_cmp(arg2, "force"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 30000)
		{
			send_to_char("Force range is 0 to 30,000 force points." NL, ch);
			return;
		}
		victim->max_mana = value;
		return;
	}

	if (!str_cmp(arg2, "move"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 30000)
		{
			send_to_char("Move range is 0 to 30,000 move points." NL, ch);
			return;
		}
		victim->max_move = value;
		return;
	}

	if (!str_cmp(arg2, "align"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (value < -1000 || value > 1000)
		{
			send_to_char("Alignment range is -1000 to 1000." NL, ch);
			return;
		}
		victim->alignment = value;
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->alignment = value;
		return;
	}

	if (!str_cmp(arg2, "password"))
	{
		char *pwdnew;
		char *p;

		if (get_trust(ch) < LEVEL_SUB_IMPLEM)
		{
			send_to_char("You can't do that." NL, ch);
			return;
		}
		if (IS_NPC(victim))
		{
			send_to_char("Mobs don't have passwords." NL, ch);
			return;
		}

		if (strlen(arg3) < 5)
		{
			send_to_char(
					"New password must be at least five characters long." NL,
					ch);
			return;
		}

		/*
		 * No tilde allowed because of player file format.
		 */
		pwdnew = crypt(arg3, ch->name);
		for (p = pwdnew; *p != '\0'; p++)
		{
			if (*p == '~')
			{
				send_to_char("New password not acceptable, try again." NL, ch);
				return;
			}
		}

		STRDUP(victim->pcdata->pwd, pwdnew);
		if (IS_SET(sysdata.save_flags, SV_PASSCHG))
			save_char_obj(victim);
		send_to_char("Ok." NL, ch);
		ch_printf(victim, "Twoje has�o zosta�o zmienione przez %s." NL,
				PERS(ch, victim, 3));
		return;
	}

	if (!str_cmp(arg2, "qp"))
	{
		if (IS_NPC(victim))
		{
			send_to_char("Not on NPC's." NL, ch);
			return;
		}

		victim->pcdata->quest_points = URANGE(1, value, 5000);
		return;
	}

	if (!str_cmp(arg2, "mentalstate"))
	{
		if (value < -100 || value > 100)
		{
			send_to_char("Value must be in range -100 to +100." NL, ch);
			return;
		}
		victim->mental_state = value;
		return;
	}

	if (!str_cmp(arg2, "emotion"))
	{
		if (value < -100 || value > 100)
		{
			send_to_char("Value must be in range -100 to +100." NL, ch);
			return;
		}
		victim->emotional_state = value;
		return;
	}

	if (!str_cmp(arg2, "thirst"))
	{
		if (IS_NPC(victim))
		{
			send_to_char("Not on NPC's." NL, ch);
			return;
		}

		if (value < 0 || value > 100)
		{
			send_to_char("Thirst range is 0 to 100." NL, ch);
			return;
		}

		victim->pcdata->condition[COND_THIRST] = value;
		return;
	}

	if (!str_cmp(arg2, "drunk"))
	{
		if (IS_NPC(victim))
		{
			send_to_char("Not on NPC's." NL, ch);
			return;
		}

		if (value < 0 || value > 100)
		{
			send_to_char("Drunk range is 0 to 100." NL, ch);
			return;
		}

		victim->pcdata->condition[COND_DRUNK] = value;
		return;
	}

	if (!str_cmp(arg2, "full"))
	{
		if (IS_NPC(victim))
		{
			send_to_char("Not on NPC's." NL, ch);
			return;
		}

		if (value < 0 || value > 100)
		{
			send_to_char("Full range is 0 to 100." NL, ch);
			return;
		}

		victim->pcdata->condition[COND_FULL] = value;
		return;
	}

	if (!str_cmp(arg2, "blood"))
	{
		if (IS_NPC(victim))
		{
			send_to_char("Not on NPC's." NL, ch);
			return;
		}

		if (value < 0 || value > MAX_LEVEL + 10)
		{
			ch_printf(ch, "Blood range is 0 to %d." NL, MAX_LEVEL + 10);
			return;
		}

		victim->pcdata->condition[COND_BLOODTHIRST] = value;
		return;
	}

	if (!str_cmp(arg2, "name"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_SUPREME)
		{
			send_to_char("Not on PC's." NL, ch);
			return;
		}

		STRDUP(victim->name, arg3);
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			STRDUP(victim->pIndexData->player_name, victim->name);
		return;
	}

	if (!str_cmp(arg2, "minsnoop"))
	{
		if (get_trust(ch) < LEVEL_SUB_IMPLEM)
		{
			send_to_char("You can't do that." NL, ch);
			return;
		}
		if (IS_NPC(victim))
		{
			send_to_char("Not on NPC's." NL, ch);
			return;
		}
		if (victim->pcdata)
		{
			victim->pcdata->min_snoop = value;
			return;
		}
	}

	if (!str_cmp(arg2, "clan"))
	{
		CLAN_DATA *clan;

		if (IS_NPC(victim))
			send_to_char("Not on NPC's." NL, ch);
		else if (!*arg3)
		{
			victim->pcdata->clan = NULL;
			save_char_obj(victim);
			donemsg(ch);
		}
		else if (!(clan = get_clan(arg3)))
			send_to_char("No such clan." NL, ch);
		else if (!get_member(clan, victim->name))
			ch_printf(ch, "%s not in %s." NL, victim->przypadki[0], clan->name);
		else
		{
			victim->pcdata->clan = clan;
			save_char_obj(victim);
			donemsg(ch);
		}

		return;
	}

	if (!str_cmp(arg2, "short"))
	{
		int i;
		int indexd = 0;
		int indexp;
		char bufor[MAX_INPUT_LENGTH];
		char short_descr[MSL];

		sprintf(short_descr, "%s", arg3);

		for (i = 0; i < 6; i++)
		{
			for (indexp = 0;
					short_descr[indexd] != '@' && short_descr[indexd] != '\0';)
				bufor[indexp++] = short_descr[indexd++];
			if (short_descr[indexd++] == '@')
			{
				bufor[indexp] = '\0';
				STRDUP(victim->przypadki[i], bufor);
				victim->przypadki[i][0] = UPPER(victim->przypadki[i][0]);
			}
			else
			{
				bufor[indexp] = '\0';
				for (; i < 6; i++)
				{
					STRDUP(victim->przypadki[i], bufor);
					victim->przypadki[i][0] = UPPER(victim->przypadki[i][0]);
				}
			}
		}

		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
		{
			for (i = 0; i < 6; i++)
				STRDUP(victim->pIndexData->przypadki[i], victim->przypadki[i]);
		}
		return;
	}

	if (!str_cmp(arg2, "long"))
	{
		if (!IS_NPC(victim) && !str_cmp(arg3, "none"))
		{
			send_to_char("Ok. Long Descr cleared." NL, ch);
			STRDUP(victim->long_descr, "");
			return;
		}
		strcpy(buf, arg3);
		strcat(buf, "" NL);
		STRDUP(victim->long_descr, buf);
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			STRDUP(victim->pIndexData->long_descr, victim->long_descr);
		return;
	}

	if (!str_cmp(arg2, "description"))
	{
		if (arg3[0])
		{
			STRDUP(victim->description, arg3);
			if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
				STRDUP(victim->pIndexData->description, victim->description);
			return;
		}
		CHECK_SUBRESTRICTED(ch);
		if (ch->substate == SUB_REPEATCMD)
			ch->tempnum = SUB_REPEATCMD;
		else
			ch->tempnum = SUB_NONE;
		string_append(ch, &victim->description);
		return;
	}

	if (!str_cmp(arg2, "title"))
	{
		if (IS_NPC(victim))
		{
			send_to_char("Not on NPC's." NL, ch);
			return;
		}

		set_title(victim, arg3);
		return;
	}

	if (!str_cmp(arg2, "spec"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (!IS_NPC(victim))
		{
			send_to_char("Not on PC's." NL, ch);
			return;
		}

		if (!str_cmp(arg3, "none"))
		{
			victim->spec_fun = NULL;
			send_to_char("Special function removed." NL, ch);
			if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
				victim->pIndexData->spec_fun = victim->spec_fun;
			return;
		}

		if ((victim->spec_fun = spec_lookup(arg3)) == 0)
		{
			send_to_char("No such spec fun." NL, ch);
			return;
		}
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->spec_fun = victim->spec_fun;
		return;
	}

	if (!str_cmp(arg2, "spec2"))
	{
		if (!can_mmodify(ch, victim))
			return;
		if (!IS_NPC(victim))
		{
			send_to_char("Not on PC's." NL, ch);
			return;
		}

		if (!str_cmp(arg3, "none"))
		{
			victim->spec_2 = NULL;
			send_to_char("Special function removed." NL, ch);
			if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
				victim->pIndexData->spec_2 = victim->spec_2;
			return;
		}

		if ((victim->spec_2 = spec_lookup(arg3)) == 0)
		{
			send_to_char("No such spec fun." NL, ch);
			return;
		}
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->spec_2 = victim->spec_2;
		return;
	}

	if (!str_cmp(arg2, "flags"))
	{
		bool pcflag;
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_GREATER)
		{
			send_to_char("You can only modify a mobile's flags." NL, ch);
			return;
		}

		if (!can_mmodify(ch, victim))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: mset <victim> flags <flag> [flag]..." NL, ch);
			send_to_char(
					"sentinal, scavenger, aggressive, stayarea, wimpy, practice, immortal," NL,
					ch);
			send_to_char(
					"deadly, mountable, guardian, nokill, scholar, noassist, droid, nocorpse," NL,
					ch);
			return;
		}
		while (argument[0] != '\0')
		{
			pcflag = false;
			argument = one_argument(argument, arg3);

			if (IS_NPC(victim))
				value = flag_value(act_flags_list, arg3);
			else
				value = flag_value(plr_flags_list, arg3);

			if (!IS_NPC(victim) && (value < 0))
			{
				pcflag = true;
				value = flag_value(pc_flags_list, arg3);
			}
			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
			{
				if ( IS_NPC(victim) && value == ACT_IS_NPC)
					send_to_char(
							"If that could be changed, it would cause many problems." NL,
							ch);
				else if ( IS_NPC(victim) && value == ACT_POLYMORPHED)
					send_to_char("Changing that would be a _bad_ thing." NL,
							ch);
				else
				{
					if (pcflag)
						TOGGLE_BIT(victim->pcdata->flags, value);
					else
					{
						TOGGLE_BIT(victim->act, value);
						/* NPC check added by Gorog */
						if ( IS_NPC(victim) && (value == ACT_PROTOTYPE))
							victim->pIndexData->act = victim->act;
					}
				}
			}
		}
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->act = victim->act;
		return;
	}

	if (!str_cmp(arg2, "security"))
	{
		if (value > 10 || value < 0)
		{
			send_to_char("Security range: 0 - 10." NL, ch);
			return;
		}

		if (IS_NPC(victim))
		{
			send_to_char("For PC's only!" NL, ch);
			return;
		}

		if (value > ch->pcdata->security && !IS_ADMIN(ch->name))
		{
			send_to_char("You can't set security greater than You have!" NL,
					ch);
			return;
		}

		if (!IS_ADMIN(ch->name) && value > 9)
		{
			send_to_char("Only Admins can set security greater than 9!" NL, ch);
			return;
		}

		if ( IS_ADMIN(victim->name) && !IS_ADMIN(ch->name))
		{
			send_to_char("You can't set Admin's security!" NL, ch);
			return;
		}

		victim->pcdata->security = value;
		send_to_char("Security set." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "wanted"))
	{

		if (IS_NPC(victim))
		{
			send_to_char("Wanted flags are for players only." NL, ch);
			return;
		}

		if (get_trust(ch) < LEVEL_GREATER)
		{
			send_to_char("You are not a high enough level to do that." NL, ch);
			return;
		}

		if (!can_mmodify(ch, victim))
			return;

		if (!argument || argument[0] == '\0')
		{
			send_to_char(
					"Usage: mset <victim> wanted <planet> <crime_type> <level>" NL
					"Or:    mset <victim> wanted <planet> remove" NL
					"Or:    mset <victim> wanted clear" NL, ch);

			return;
		}

		/* usuwamy wszystkie flagi */
		if (!str_cmp(argument, "clear"))
		{
			if (!victim->crimes.empty())
			{
				auto snapshot = victim->crimes;
				for (auto* crime : snapshot)
					free_crime(victim, crime);
			}

			ch_printf(ch, "Wanted-flags clean." NL);
			return;
		}

		argument = one_argument(argument, arg3);

		if (get_planet(arg3))
		{
			int flag;

			if (!argument || argument[0] == '\0')
			{
				send_to_char("No type specified" NL, ch);
				return;
			}

			if (!str_cmp(argument, "remove"))
			{

				if (!victim->crimes.empty())
				{
					for (auto* crime : victim->crimes)
					{
						if (!str_cmp(arg3, crime->planet))
						{
							free_crime(victim, crime);
							ch_printf(ch, "Wanted-flag: %s removed." NL, arg3);
							return;
						}
					}
					ch_printf(ch, "No such Crime." NL);
				}
				else
					ch_printf(ch, "That person has No Crimes set." NL);
				return;
			}

			if ((flag = flag_value(crime_flags_list, argument)) < 0)
			{
				ch_printf(ch, "No such crime_type." NL);
				return;
			}

			crime_to_char(victim, arg3, flag);

			ch_printf(ch, "Crime added." NL);
		}
		else
			ch_printf(ch, "No such planet." NL);
		return;
	}

	if (!str_cmp(arg2, "vip"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("VIP flags are for mobs only." NL, ch);
			return;
		}

		if (!can_mmodify(ch, victim))
			return;

		if (!argument || argument[0] == '\0')
		{
			send_to_char(
					"Usage: mset <victim> vip <planet> [planet]...\n\rOr:   mset <victim> vip clear\n\rOr:   mset <victim> vip all" NL,
					ch);
			return;
		}

		/* usuwamy wszystkie flagi */
		if (!str_cmp(argument, "clear"))
		{
			STRDUP(victim->s_vip_flags, "");
			ch_printf(ch, "All Vip-flags removed." NL);
			return;
		}

		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);

			if (!*arg3)
				break;

			if (!get_planet(arg3))
				ch_printf(ch,
						"!!! Warning !!! There is no such planet!\n\rLeave it if you're sure about that." NL);

			/* usuwamy jedn� flag� */
			if (is_name(arg3, victim->s_vip_flags))
			{
				strcpy(buf, victim->s_vip_flags);
				STRDUP(victim->s_vip_flags, (char* )cut_from_string(buf, arg3));
				ch_printf(ch, "Vip-flag: %s removed." NL, arg3);
			}
			/* dodajemy jedn� flag� */
			else
			{
				strcpy(buf, victim->s_vip_flags);
				if (victim->s_vip_flags && victim->s_vip_flags[0] != '\0')
					strcat(buf, " ");
				strcat(buf, capitalize(arg3));
				STRDUP(victim->s_vip_flags, buf);
				ch_printf(ch, "Vip-flag: %s added." NL, arg3);
			}
		}
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE))
			STRDUP(victim->pIndexData->s_vip_flags, victim->s_vip_flags);
		return;
	}

	if (!str_cmp(arg2, "affected"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's flags." NL, ch);
			return;
		}

		if (!can_mmodify(ch, victim))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: mset <victim> affected <flag> [flag]..." NL,
					ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(aff_flags_list, arg3);
			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
				TOGGLE_BIT(victim->affected_by, value);
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->affected_by = victim->affected_by;
		return;
	}

	/*
	 * save some more finger-leather for setting RIS stuff
	 */
	if (!str_cmp(arg2, "r"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's ris." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sprintf(outbuf, "%s resistant %s", arg1, arg3);
		do_mset(ch, outbuf);
		return;
	}
	if (!str_cmp(arg2, "i"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's ris." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sprintf(outbuf, "%s immune %s", arg1, arg3);
		do_mset(ch, outbuf);
		return;
	}
	if (!str_cmp(arg2, "s"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's ris." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sprintf(outbuf, "%s susceptible %s", arg1, arg3);
		do_mset(ch, outbuf);
		return;
	}
	if (!str_cmp(arg2, "ri"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's ris." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sprintf(outbuf, "%s resistant %s", arg1, arg3);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s immune %s", arg1, arg3);
		do_mset(ch, outbuf);
		return;
	}

	if (!str_cmp(arg2, "rs"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's ris." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sprintf(outbuf, "%s resistant %s", arg1, arg3);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s susceptible %s", arg1, arg3);
		do_mset(ch, outbuf);
		return;
	}
	if (!str_cmp(arg2, "is"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's ris." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sprintf(outbuf, "%s immune %s", arg1, arg3);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s susceptible %s", arg1, arg3);
		do_mset(ch, outbuf);
		return;
	}
	if (!str_cmp(arg2, "ris"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's ris." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sprintf(outbuf, "%s resistant %s", arg1, arg3);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s immune %s", arg1, arg3);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s susceptible %s", arg1, arg3);
		do_mset(ch, outbuf);
		return;
	}

	if (!str_cmp(arg2, "resistant"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's resistancies." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: mset <victim> resistant <flag> [flag]..." NL,
					ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(ris_flags_list, arg3);
			if (value < 0 || value > 31)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
				TOGGLE_BIT(victim->resistant, value);
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->resistant = victim->resistant;
		return;
	}

	if (!str_cmp(arg2, "immune"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's immunities." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: mset <victim> immune <flag> [flag]..." NL, ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(ris_flags_list, arg3);
			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
				TOGGLE_BIT(victim->immune, value);
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->immune = victim->immune;
		return;
	}

	if (!str_cmp(arg2, "susceptible"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's susceptibilities." NL,
					ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: mset <victim> susceptible <flag> [flag]..." NL,
					ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(ris_flags_list, arg3);
			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
				TOGGLE_BIT(victim->susceptible, value);
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->susceptible = victim->susceptible;
		return;
	}

	if (!str_cmp(arg2, "part"))
	{
		if (!IS_NPC(victim) && get_trust(ch) < LEVEL_LESSER)
		{
			send_to_char("You can only modify a mobile's parts." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: mset <victim> part <flag> [flag]..." NL, ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(part_flags_list, arg3);
			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
				TOGGLE_BIT(victim->xflags, value);
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->xflags = victim->xflags;
		return;
	}

	if (!str_cmp(arg2, "attack"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("You can only modify a mobile's attacks." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: mset <victim> attack <flag> [flag]..." NL, ch);
			send_to_char(
					"bite          claws        tail        sting      punch        kick" NL,
					ch);
			send_to_char(
					"trip          bash         stun        gouge      backstab" NL,
					ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(attack_flags_list, arg3);
			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
				TOGGLE_BIT(victim->attacks, value);
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->attacks = victim->attacks;
		return;
	}

	if (!str_cmp(arg2, "defense"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("You can only modify a mobile's defenses." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: mset <victim> defense <flag> [flag]..." NL,
					ch);
			send_to_char("parry        dodge" NL, ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(def_flags_list, arg3);
			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
				TOGGLE_BIT(victim->defenses, value);
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->defenses = victim->defenses;
		return;
	}

	if (!str_cmp(arg2, "pos"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > POS_STANDING)
		{
			ch_printf(ch, "Position range is 0 to %d." NL, POS_STANDING);
			return;
		}
		victim->position = value;
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->position = victim->position;
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "defpos"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > POS_STANDING)
		{
			ch_printf(ch, "Position range is 0 to %d." NL, POS_STANDING);
			return;
		}
		victim->defposition = value;
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->defposition = victim->defposition;
		send_to_char("Done." NL, ch);
		return;
	}

	/*
	 * save some finger-leather
	 */
	if (!str_cmp(arg2, "hitdie"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sscanf(arg3, "%d %c %d %c %d", &num, &char1, &size, &char2, &plus);
		sprintf(outbuf, "%s hitnumdie %d", arg1, num);
		do_mset(ch, outbuf);

		sprintf(outbuf, "%s hitsizedie %d", arg1, size);
		do_mset(ch, outbuf);

		sprintf(outbuf, "%s hitplus %d", arg1, plus);
		do_mset(ch, outbuf);
		return;
	}
	/*
	 * save some more finger-leather
	 */
	if (!str_cmp(arg2, "damdie"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;

		sscanf(arg3, "%d %c %d %c %d", &num, &char1, &size, &char2, &plus);
		sprintf(outbuf, "%s damnumdie %d", arg1, num);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s damsizedie %d", arg1, size);
		do_mset(ch, outbuf);
		sprintf(outbuf, "%s damplus %d", arg1, plus);
		do_mset(ch, outbuf);
		return;
	}

	if (!str_cmp(arg2, "hitnumdie"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 32767)
		{
			send_to_char("Number of hitpoint dice range is 0 to 30000." NL, ch);
			return;
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->hitnodice = value;
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "hitsizedie"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 32767)
		{
			send_to_char("Hitpoint dice size range is 0 to 30000." NL, ch);
			return;
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->hitsizedice = value;
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "hitplus"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 32767)
		{
			send_to_char("Hitpoint bonus range is 0 to 30000." NL, ch);
			return;
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->hitplus = value;
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "damnumdie"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 100)
		{
			send_to_char("Number of damage dice range is 0 to 100." NL, ch);
			return;
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->damnodice = value;
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "damsizedie"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 100)
		{
			send_to_char("Damage dice size range is 0 to 100." NL, ch);
			return;
		}
		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->damsizedice = value;
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "damplus"))
	{
		if (!IS_NPC(victim))
		{
			send_to_char("Mobiles only." NL, ch);
			return;
		}
		if (!can_mmodify(ch, victim))
			return;
		if (value < 0 || value > 1000)
		{
			send_to_char("Damage bonus range is 0 to 1000." NL, ch);
			return;
		}

		if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE))
			victim->pIndexData->damplus = value;
		send_to_char("Done." NL, ch);
		return;

	}

	if (!str_cmp(arg2, "aloaded"))
	{
		if (IS_NPC(victim))
		{
			send_to_char("Player Characters only." NL, ch);
			return;
		}

		if (!can_mmodify(ch, victim))
			return;

		if (!IS_SET(victim->pcdata->area->status, AREA_LOADED))
		{
			SET_BIT(victim->pcdata->area->status, AREA_LOADED);
			send_to_char("Your area set to LOADED!" NL, victim);
			if (ch != victim)
				send_to_char("Area set to LOADED!" NL, ch);
			return;
		}
		else
		{
			REMOVE_BIT(victim->pcdata->area->status, AREA_LOADED);
			send_to_char("Your area set to NOT-LOADED!" NL, victim);
			if (ch != victim)
				send_to_char("Area set to NON-LOADED!" NL, ch);
			return;
		}
	}
	/*
	 if ( !str_cmp( arg2, "speaks" ) )
	 {
	 if ( !can_mmodify( ch, victim ) )
	 return;
	 if ( !argument || argument[0] == '\0' )
	 {
	 send_to_char( "Usage: mset <victim> speaks <language> [language] ..." NL, ch );
	 return;
	 }
	 while ( argument[0] != '\0' )
	 {
	 argument = one_argument( argument, arg3 );
	 value = get_langflag( arg3 );
	 if ( value == LANG_UNKNOWN )
	 ch_printf( ch, "Unknown language: %s" NL, arg3 );
	 else
	 if ( !IS_NPC( victim ) )
	 {
	 int valid_langs = LANG_COMMON | LANG_WOOKIEE | LANG_TWI_LEK | LANG_RODIAN
	 | LANG_HUTT | LANG_MON_CALAMARI | LANG_NOGHRI | LANG_GAMORREAN
	 | LANG_JAWA | LANG_ADARIAN | LANG_EWOK | LANG_VERPINE | LANG_DEFEL
	 | LANG_TRANDOSHAN | LANG_CHADRA_FAN | LANG_QUARREN | LANG_DUINUOGWUIN
	 | LANG_YUUZHAN_VONG;

	 if ( !(value &= valid_langs) )
	 {
	 ch_printf( ch, "Players may not know %s." NL, arg3 );
	 continue;
	 }
	 }
	 TOGGLE_BIT( victim->speaks, value );
	 }
	 if ( !IS_NPC( victim ) )
	 {
	 REMOVE_BIT( victim->speaks, victim->race->language );
	 if ( !knows_language( victim, victim->speaking, victim ) )
	 victim->speaking = victim->race->language;
	 }
	 else
	 if ( IS_SET( victim->act, ACT_PROTOTYPE ) )
	 victim->pIndexData->speaks = victim->speaks;
	 send_to_char( "Done." NL, ch );
	 return;
	 }
	 */
	if (!str_cmp(arg2, "attribute") && !str_cmp(arg3, "clear"))
	{
		victim->attribute1 = INT_MIN;
		victim->attribute2 = INT_MIN;
		return;
	}
	if (!str_cmp(arg2, "kin") && !str_cmp(arg3, "clear"))
	{
		while (!victim->known.empty())
		{
			auto* _friend = victim->known.back();
			victim->known.pop_back();
			STRFREE(_friend->name);
			DISPOSE(_friend);
		}
		return;
	}
	/*
	 if ( !str_cmp( arg2, "speaking" ) )
	 {
	 if ( !IS_NPC( victim ) )
	 {
	 send_to_char( "Players must choose the language they speak themselves." NL, ch );
	 return;
	 }
	 if ( !can_mmodify( ch, victim ) )
	 return;
	 if ( !argument || argument[0] == '\0' )
	 {
	 send_to_char( "Usage: mset <victim> speaking <language> [language]..." NL, ch );
	 return;
	 }
	 while ( argument[0] != '\0' )
	 {
	 argument = one_argument( argument, arg3 );
	 value = get_langflag( arg3 );
	 if ( value == LANG_UNKNOWN )
	 ch_printf( ch, "Unknown language: %s" NL, arg3 );
	 else
	 TOGGLE_BIT( victim->speaking, value );
	 }
	 if ( IS_NPC(victim) && IS_SET( victim->act, ACT_PROTOTYPE ) )
	 victim->pIndexData->speaking = victim->speaking;
	 send_to_char( "Done." NL, ch );
	 return;
	 }
	 */
	/*
	 * Generate usage message.
	 */
	if (ch->substate == SUB_REPEATCMD)
	{
		ch->substate = SUB_RESTRICTED;
		interpret(ch, origarg);
		ch->substate = SUB_REPEATCMD;
		ch->last_cmd = do_mset;
	}
	else
		do_mset(ch, (char*) "");
	return;
}

DEF_DO_FUN( oset )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	char outbuf[MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	EXTRA_DESCR_DATA *ed;

	int value, tmp;

	if (IS_NPC(ch))
	{
		send_to_char("Mobs can't oset" NL, ch);
		return;
	}

	if (!ch->desc)
	{
		send_to_char("You have no descriptor" NL, ch);
		return;
	}
	obj = NULL;
	smash_tilde(argument);
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	strcpy(arg3, argument);

	if (arg1[0] == '\0' || arg2[0] == '\0' || !str_cmp(arg1, "?"))
	{
		if (ch->substate == SUB_REPEATCMD)
		{
			if (obj)
				send_to_char("Syntax: <field>  <value>" NL, ch);
			else
				send_to_char("Syntax: <object> <field>  <value>" NL, ch);
		}
		else
			send_to_char(
					"Syntax: oset <object> <field>  <value>" NL NL
					"Field being one of:" NL
					"  flags wear level weight cost rent timer" NL
					"  name short long desc ed rmed actiondesc" NL
					"  type value0 value1 value2 value3 value4" NL
					"  value5 affect rmaffect layers gender owner" NL
					"For weapons:             For armor:" NL
					"  weapontype condition     ac condition" NL
					"  numdamdie sizedamdie                  " NL
					"  charges   maxcharges                  " NL
					"For potions, pills:" NL
					"  slevel spell1 spell2 spell3" NL
					"For devices:" NL
					"  slevel spell maxcharges charges" NL
					"For salves:" NL
					"  slevel spell1 spell2 maxdoses delay (keep low - delay is anoying)" NL
					"For containers:          For levers and switches:" NL
					"  cflags key capacity      tflags" NL
					"For rawspice:            For ammo and batteries:" NL
					"  spicetype  grade         charges (at least 1000 for ammo)" NL
					"For crystals:" NL
					"  gemtype" NL, ch);
		return;
	}

	if (!obj && get_trust(ch) <= LEVEL_IMMORTAL)
	{
		if ((obj = get_obj_here(ch, arg1)) == NULL)
		{
			send_to_char("You can't find that here." NL, ch);
			return;
		}
	}
	else if (!obj)
	{
		if ((obj = get_obj_world(ch, arg1)) == NULL)
		{
			send_to_char("There is nothing like that in all the realms." NL,
					ch);
			return;
		}
	}

	separate_obj(obj);
	value = atoi(arg3);

	if (!str_cmp(arg2, "value0") || !str_cmp(arg2, "v0"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->value[0] = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->value[0] = value;
		return;
	}

	if (!str_cmp(arg2, "value1") || !str_cmp(arg2, "v1"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->value[1] = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->value[1] = value;
		return;
	}

	if (!str_cmp(arg2, "value2") || !str_cmp(arg2, "v2"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->value[2] = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
		{
			obj->pIndexData->value[2] = value;
			if (obj->item_type == ITEM_WEAPON && value != 0)
				obj->value[2] = obj->pIndexData->value[1]
						* obj->pIndexData->value[2];
		}
		return;
	}

	if (!str_cmp(arg2, "value3") || !str_cmp(arg2, "v3"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->value[3] = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->value[3] = value;
		return;
	}

	if (!str_cmp(arg2, "value4") || !str_cmp(arg2, "v4"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->value[4] = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->value[4] = value;
		return;
	}

	if (!str_cmp(arg2, "value5") || !str_cmp(arg2, "v5"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->value[5] = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->value[5] = value;
		return;
	}

	if (!str_cmp(arg2, "type"))
	{
		if (!can_omodify(ch, obj))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: oset <object> type <type>" NL, ch);
			send_to_char("Possible Types:" NL, ch);
			send_to_char("None        Light" NL, ch);
			send_to_char(
					"Treasure    Armor      Comlink    Fabric      Grenade" NL,
					ch);
			send_to_char(
					"Furniture   Trash      Container  Drink_con   Landmine" NL,
					ch);
			send_to_char(
					"Key         Food       Money      Pen         Fuel" NL,
					ch);
			send_to_char(
					"Fountain    Pill       Weapon     Medpac      Missile" NL,
					ch);
			send_to_char(
					"Fire        Book       Superconductor         Rare_metal" NL,
					ch);
			send_to_char(
					"Switch      Lever      Button     Dial        Government" NL,
					ch);
			send_to_char(
					"Trap        Map        Portal     Paper       Magnet" NL,
					ch);
			send_to_char(
					"Lockpick    Shovel     Thread     Smut        Ammo" NL,
					ch);
			send_to_char(
					"Rawspice    Lens       Crystal    Duraplast   Battery" NL,
					ch);
			send_to_char(
					"Toolkit     Durasteel  Oven       Mirror      Circuit" NL,
					ch);
			send_to_char(
					"Potion      Salve      Pill       Device      Shipmodule" NL,
					ch);
			send_to_char("Bolt        Chemical	 Shipdevice" NL, ch);
			return;
		}
		value = flag_value(obj_types_list, argument);

		if (value < 1)
		{
			ch_printf(ch, "Unknown type: %s" NL, arg3);
			return;
		}
		obj->item_type = (int) value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->item_type = obj->item_type;
		return;
	}

	if (!str_cmp(arg2, "flags"))
	{
		if (!can_omodify(ch, obj))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: oset <object> flags <flag> [flag]..." NL, ch);
			send_to_char(
					"glow, dark, force, bless, antievil, noremove, antisith, antisoldier," NL,
					ch);
			send_to_char(
					"donation, covering, hum, invis, nodrop, antigood, antipilot, anticitizen" NL,
					ch);
			send_to_char(
					"antineutral, inventory, antithief, antijedi, clanobject, antihunter" NL,
					ch);
			send_to_char(
					"small_size, human_size, large_size, hutt_size, contraband, personal" NL,
					ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(obj_flags_list, arg3);

			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
			{
				TOGGLE_BIT(obj->extra_flags, value);
				if (value == ITEM_PROTOTYPE)
					obj->pIndexData->extra_flags = obj->extra_flags;
				if (value == ITEM_PERSONAL
						&& !IS_SET(obj->extra_flags, ITEM_PERSONAL))
					STRDUP(obj->owner_name, "");
			}
		}
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->extra_flags = obj->extra_flags;
		return;
	}

	if (!str_cmp(arg2, "owner"))
	{
		if (!*argument)
			send_to_char("Syntax: owner <name>" NL, ch);
		else if (!str_cmp(argument, "none"))
			STRDUP(obj->owner_name, "");
		else
			STRDUP(obj->owner_name, argument);

		return;
	}

	if (!str_cmp(arg2, "wear"))
	{
		if (!can_omodify(ch, obj))
			return;
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: oset <object> wear <flag> [flag]..." NL, ch);
			send_to_char("Possible locations:" NL, ch);
			send_to_char("take   finger   neck    body    head   legs" NL, ch);
			send_to_char("feet   hands    arms    shield  about  waist" NL, ch);
			send_to_char("wrist  wield    hold    ears    eyes" NL, ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg3);
			value = flag_value(wear_flags_list, arg3);
			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg3);
			else
				TOGGLE_BIT(obj->wear_flags, value);
		}

		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->wear_flags = obj->wear_flags;
		return;
	}

	if (!str_cmp(arg2, "level"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->level = value;
		return;
	}

	if (!str_cmp(arg2, "weight"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->weight = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->weight = value;
		return;
	}

	//added by Thanos (ustalenie p�ci przedmiotu)
	if (!str_cmp(arg2, "gender"))
	{
		if (!can_omodify(ch, obj))
			return;
		if (value < GENDER_NEUTRAL || value > GENDER_PLURAL)
		{
			send_to_char("Unknown Gender" NL, ch);
			return;
		}
		obj->gender = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->gender = value;
		return;
	}
	//done

	if (!str_cmp(arg2, "cost"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->cost = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->cost = value;
		return;
	}

	if (!str_cmp(arg2, "rent"))
	{
		if (!can_omodify(ch, obj))
			return;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->rent = value;
		else
			send_to_char("Item must have prototype flag to set this value." NL,
					ch);
		return;
	}

	if (!str_cmp(arg2, "layers"))
	{
		if (!can_omodify(ch, obj))
			return;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->layers = value;
		else
			send_to_char("Item must have prototype flag to set this value." NL,
					ch);
		return;
	}

	if (!str_cmp(arg2, "timer"))
	{
		if (!can_omodify(ch, obj))
			return;
		obj->timer = value;
		return;
	}

	if (!str_cmp(arg2, "name"))
	{
		if (!can_omodify(ch, obj))
			return;
		STRDUP(obj->name, arg3);
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			STRDUP(obj->pIndexData->name, obj->name);
		return;
	}

	if (!str_cmp(arg2, "short"))
	{
		int i;
		int indexd = 0;
		int indexp;
		char bufor[MAX_INPUT_LENGTH];
		char short_descr[MSL];

		sprintf(short_descr, "%s", arg3);

		//added by Thanos (prze�adujmy przypadki)
		for (i = 0; i < 6; i++)
		{
			for (indexp = 0;
					short_descr[indexd] != '@' && short_descr[indexd] != '\0';)
				bufor[indexp++] = short_descr[indexd++];
			if (short_descr[indexd++] == '@')
			{
				bufor[indexp] = '\0';
				STRDUP(obj->przypadki[i], bufor);
			}
			else
			{
				bufor[indexp] = '\0';
				for (; i < 6; i++)
					STRDUP(obj->przypadki[i], bufor);
			}
		}
		//done by Thanos
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
		{
			for (i = 0; i < 6; i++)
				STRDUP(obj->pIndexData->przypadki[i], obj->przypadki[i]);
		}
		else
		/* Feature added by Narn, Apr/96
		 * If the item is not proto, add the word 'rename' to the keywords
		 * if it is not already there.
		 */
		{
			if (str_infix("rename", obj->name))
			{
				sprintf(buf, "%s %s", obj->name, "rename");
				STRDUP(obj->name, buf);
			}
		}
		return;
	}

	if (!str_cmp(arg2, "actiondesc"))
	{
		if (strstr(arg3, "%n") || strstr(arg3, "%d") || strstr(arg3, "%l"))
		{
			send_to_char("Illegal characters!" NL, ch);
			return;
		}
		STRDUP(obj->action_desc, arg3);
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			STRDUP(obj->pIndexData->action_desc, obj->action_desc);
		return;
	}

	if (!str_cmp(arg2, "long"))
	{
		if (arg3[0])
		{
			STRDUP(obj->description, arg3);
			if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
				STRDUP(obj->pIndexData->description, obj->description);
			return;
		}
		CHECK_SUBRESTRICTED(ch);
		string_append(ch, &obj->description);
		return;
	}

	if (!str_cmp(arg2, "affect"))
	{
		AFFECT_DATA *paf;
		int loc;
		int bitv;

		argument = one_argument(argument, arg2);
		if (!*arg2 || !*argument)
		{
			send_to_char("Usage: oset <object> affect <field> <value>" NL, ch);
			send_to_char("Affect Fields:" NL, ch);
			send_to_char(
					"none        strength    dexterity   intelligence  wisdom       constitution" NL,
					ch);
			send_to_char(
					"sex         level       age         height        weight       force" NL,
					ch);
			send_to_char(
					"hit         move        credits     experience    armor        hitroll" NL,
					ch);
			send_to_char(
					"damroll     save_para   save_rod    save_poison   save_breath  save_power" NL,
					ch);
			send_to_char(
					"charisma    resistant   immune      susceptible   affected     luck" NL,
					ch);
			send_to_char(
					"backstab    pick        track       steal         sneak        hide" NL,
					ch);
			send_to_char(
					"detrap      dodge       peek        scan          gouge        search" NL,
					ch);
			send_to_char(
					"mount       disarm      kick        parry         bash         stun" NL,
					ch);
			send_to_char(
					"punch       climb       grip        scribe        brew" NL,
					ch);
			return;
		}
		loc = flag_value(apply_types_list, arg2);
		if (loc < 1)
		{
			ch_printf(ch, "Unknown field: %s" NL, arg2);
			return;
		}
		if (loc >= APPLY_AFFECT && loc < APPLY_WEAPONSPELL)
		{
			bitv = 0;
			while (argument[0] != '\0')
			{
				argument = one_argument(argument, arg3);
				if (loc == APPLY_AFFECT)
					value = flag_value(aff_flags_list, arg3);
				else
					value = flag_value(ris_flags_list, arg3);
				if (value < 0)
					ch_printf(ch, "Unknown flag: %s" NL, arg3);
				else
					SET_BIT(bitv, value);
			}
			if (!bitv)
				return;
			value = bitv;
		}
		else
		{
			argument = one_argument(argument, arg3);
			value = atoi(arg3);
		}
		CREATE(paf, AFFECT_DATA, 1);
		paf->type = -1;
		paf->duration = -1;
		paf->location = loc;
		paf->modifier = value;
		paf->bitvector = 0;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->affects.push_back(paf);
		else
			obj->affects.push_back(paf);
		++top_affect;
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "rmaffect"))
	{
		int loc, count;

		if (!argument || argument[0] == '\0')
		{
			send_to_char("Usage: oset <object> rmaffect <affect#>" NL, ch);
			return;
		}
		loc = atoi(argument);
		if (loc < 1)
		{
			send_to_char("Invalid number." NL, ch);
			return;
		}

		count = 0;

		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
		{
			OBJ_INDEX_DATA *pObjIndex;

			pObjIndex = obj->pIndexData;
			for (auto* paf : pObjIndex->affects)
			{
				if (++count == loc)
				{
					pObjIndex->affects.remove(paf);
					DISPOSE(paf);
					send_to_char("Removed." NL, ch);
					--top_affect;
					return;
				}
			}
			send_to_char("Not found." NL, ch);
			return;
		}
		else
		{
			for (auto* paf : obj->affects)
			{
				if (++count == loc)
				{
					obj->affects.remove(paf);
					DISPOSE(paf);
					send_to_char("Removed." NL, ch);
					--top_affect;
					return;
				}
			}
			send_to_char("Not found." NL, ch);
			return;
		}
	}

	if (!str_cmp(arg2, "ed"))
	{
		if (!*arg3)
		{
			send_to_char("Syntax: oset <object> ed <keywords>" NL, ch);
			return;
		}
		CHECK_SUBRESTRICTED(ch);
		if (obj->timer)
		{
			send_to_char(
					"It's not safe to edit an extra description on an object with a timer.\n\rTurn it off first." NL,
					ch);
			return;
		}
		if (obj->item_type == ITEM_PAPER)
		{
			send_to_char(
					"You can not add an extra description to a note paper at the moment." NL,
					ch);
			return;
		}
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			ed = SetOExtraProto(obj->pIndexData, arg3);
		else
			ed = SetOExtra(obj, arg3);
		string_append(ch, &ed->description);
		return;
	}

	if (!str_cmp(arg2, "desc"))
	{
		CHECK_SUBRESTRICTED(ch);
		if (obj->timer)
		{
			send_to_char(
					"It's not safe to edit a description on an object with a timer.\n\rTurn it off first." NL,
					ch);
			return;
		}
		if (obj->item_type == ITEM_PAPER)
		{
			send_to_char(
					"You can not add a description to a note paper at the moment." NL,
					ch);
			return;
		}
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			ed = SetOExtraProto(obj->pIndexData, obj->name);
		else
			ed = SetOExtra(obj, obj->name);
		string_append(ch, &ed->description);
		return;
	}

	if (!str_cmp(arg2, "rmed"))
	{
		if (!*arg3)
		{
			send_to_char("Syntax: oset <object> rmed <keywords>" NL, ch);
			return;
		}
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
		{
			if (DelOExtraProto(obj->pIndexData, arg3))
				send_to_char("Deleted." NL, ch);
			else
				send_to_char("Not found." NL, ch);
			return;
		}
		if (DelOExtra(obj, arg3))
			send_to_char("Deleted." NL, ch);
		else
			send_to_char("Not found." NL, ch);
		return;
	}
	/*
	 * save some finger-leather
	 */
	if (!str_cmp(arg2, "ris"))
	{
		sprintf(outbuf, "%s affect resistant %s", arg1, arg3);
		do_oset(ch, outbuf);
		sprintf(outbuf, "%s affect immune %s", arg1, arg3);
		do_oset(ch, outbuf);
		sprintf(outbuf, "%s affect susceptible %s", arg1, arg3);
		do_oset(ch, outbuf);
		return;
	}

	if (!str_cmp(arg2, "r"))
	{
		sprintf(outbuf, "%s affect resistant %s", arg1, arg3);
		do_oset(ch, outbuf);
		return;
	}

	if (!str_cmp(arg2, "i"))
	{
		sprintf(outbuf, "%s affect immune %s", arg1, arg3);
		do_oset(ch, outbuf);
		return;
	}
	if (!str_cmp(arg2, "s"))
	{
		sprintf(outbuf, "%s affect susceptible %s", arg1, arg3);
		do_oset(ch, outbuf);
		return;
	}

	if (!str_cmp(arg2, "ri"))
	{
		sprintf(outbuf, "%s affect resistant %s", arg1, arg3);
		do_oset(ch, outbuf);
		sprintf(outbuf, "%s affect immune %s", arg1, arg3);
		do_oset(ch, outbuf);
		return;
	}

	if (!str_cmp(arg2, "rs"))
	{
		sprintf(outbuf, "%s affect resistant %s", arg1, arg3);
		do_oset(ch, outbuf);
		sprintf(outbuf, "%s affect susceptible %s", arg1, arg3);
		do_oset(ch, outbuf);
		return;
	}

	if (!str_cmp(arg2, "is"))
	{
		sprintf(outbuf, "%s affect immune %s", arg1, arg3);
		do_oset(ch, outbuf);
		sprintf(outbuf, "%s affect susceptible %s", arg1, arg3);
		do_oset(ch, outbuf);
		return;
	}

	/*
	 * Make it easier to set special object values by name than number
	 * 						-Thoric
	 */
	tmp = -1;
	switch (obj->item_type)
	{
	case ITEM_WEAPON:
		if (!str_cmp(arg2, "weapontype"))
		{
			value = flag_value(weapon_types_list, arg3);
			if (value < 0)
			{
				send_to_char("Unknown weapon type." NL, ch);
				send_to_char("\n\rChoices:" NL, ch);
				send_to_char(
						"   none, lightsaber, vibro-blade, blaster, force pike, bowcaster, bludgeon" NL,
						ch);
				return;
			}
			tmp = 3;
			break;
		}
		if (!str_cmp(arg2, "condition"))
			tmp = 0;
		if (!str_cmp(arg2, "numdamdie"))
			tmp = 1;
		if (!str_cmp(arg2, "sizedamdie"))
			tmp = 2;
		if (!str_cmp(arg2, "charges"))
			tmp = 4;
		if (!str_cmp(arg2, "maxcharges"))
			tmp = 5;
		if (!str_cmp(arg2, "charge"))
			tmp = 4;
		if (!str_cmp(arg2, "maxcharge"))
			tmp = 5;
		break;
	case ITEM_BOLT:
	case ITEM_AMMO:
		if (!str_cmp(arg2, "charges"))
			tmp = 0;
		if (!str_cmp(arg2, "charge"))
			tmp = 0;
		break;
	case ITEM_BATTERY:
		if (!str_cmp(arg2, "charges"))
			tmp = 0;
		if (!str_cmp(arg2, "charge"))
			tmp = 0;
		break;
	case ITEM_RAWSPICE:
	case ITEM_SPICE:
		if (!str_cmp(arg2, "grade"))
			tmp = 1;
		if (!str_cmp(arg2, "spicetype"))
		{
			value = flag_value(spice_types_list, arg3);
			if (value < 0)
			{
				send_to_char("Unknown spice type." NL, ch);
				send_to_char("\n\rChoices:" NL, ch);
				send_to_char("   glitterstim, carsanum, ryll, andris" NL, ch);
				return;
			}
			tmp = 0;
			break;
		}
		break;
	case ITEM_CRYSTAL:
		if (!str_cmp(arg2, "gemtype"))
		{
			value = flag_value(crystal_types_list, arg3);
			if (value < 0)
			{
				send_to_char("Unknown gem type." NL, ch);
				send_to_char("\n\rChoices:" NL, ch);
				send_to_char(
						"   non-adegan, kathracite, relacite, danite, mephite, ponite, illum, corusca" NL,
						ch);
				return;
			}
			tmp = 0;
			break;
		}
		break;
	case ITEM_ARMOR:
		if (!str_cmp(arg2, "condition"))
			tmp = 0;
		if (!str_cmp(arg2, "ac"))
			tmp = 1;
		break;
	case ITEM_SALVE:
		if (!str_cmp(arg2, "slevel"))
			tmp = 0;
		if (!str_cmp(arg2, "maxdoses"))
			tmp = 1;
		if (!str_cmp(arg2, "doses"))
			tmp = 2;
		if (!str_cmp(arg2, "delay"))
			tmp = 3;
		if (!str_cmp(arg2, "spell1"))
			tmp = 4;
		if (!str_cmp(arg2, "spell2"))
			tmp = 5;
		if (tmp >= 4 && tmp <= 5)
			value = skill_lookup(arg3);
		break;
	case ITEM_POTION:
	case ITEM_PILL:
		if (!str_cmp(arg2, "slevel"))
			tmp = 0;
		if (!str_cmp(arg2, "spell1"))
			tmp = 1;
		if (!str_cmp(arg2, "spell2"))
			tmp = 2;
		if (!str_cmp(arg2, "spell3"))
			tmp = 3;
		if (tmp >= 1 && tmp <= 3)
			value = skill_lookup(arg3);
		break;
	case ITEM_DEVICE:
		if (!str_cmp(arg2, "slevel"))
			tmp = 0;
		if (!str_cmp(arg2, "spell"))
		{
			tmp = 3;
			value = skill_lookup(arg3);
		}
		if (!str_cmp(arg2, "maxcharges"))
			tmp = 1;
		if (!str_cmp(arg2, "charges"))
			tmp = 2;
		break;
	case ITEM_CONTAINER:
		if (!str_cmp(arg2, "capacity"))
			tmp = 0;
		if (!str_cmp(arg2, "cflags"))
			tmp = 1;
		if (!str_cmp(arg2, "key"))
			tmp = 2;
		break;
	case ITEM_SWITCH:
	case ITEM_LEVER:
	case ITEM_BUTTON:
		if (!str_cmp(arg2, "tflags"))
		{
			tmp = 0;
			value = flag_value(trig_flags_list, arg3);
		}
		break;
	}
	if (tmp >= 0 && tmp <= 5)
	{
		if (!can_omodify(ch, obj))
			return;
		obj->value[tmp] = value;
		if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
			obj->pIndexData->value[tmp] = value;
		return;
	}

	/*
	 * Generate usage message.
	 */
	do_oset(ch, (char*) "");
	return;
}

const SWString sprint_reset(CHAR_DATA *ch, RESET_DATA *pReset, int num, bool rlist)
{
	char buf[MSL] =
	{ 0 };
	char mobname[MSL] =
	{ 0 };
	char roomname[MSL] =
	{ 0 };
	char objname[MSL] =
	{ 0 };
	static ROOM_INDEX_DATA *room;
	static OBJ_INDEX_DATA *obj, *obj2;
	static MOB_INDEX_DATA *mob;
	int rvnum = 0;

	if (ch->in_room)
		rvnum = ch->in_room->vnum;
	if (num == 1)
	{
		room = NULL;
		obj = NULL;
		obj2 = NULL;
		mob = NULL;
	}

	switch (pReset->command)
	{
	default:
		sprintf(buf, "%2d) *** BAD RESET: %c %d %lld %lld %lld ***" NL, num,
				pReset->command, pReset->extra, pReset->arg1, pReset->arg2,
				pReset->arg3);
		break;
	case 'M':
		mob = get_mob_index(pReset->arg1);
		room = get_room_index(pReset->arg3);
		if (mob)
			strcpy(mobname, mob->player_name);
		else
			strcpy(mobname, "Mobile: *BAD VNUM*");
		if (room)
			strcpy(roomname, room->name);
		else
			strcpy(roomname, "Room: *BAD VNUM*");
		swsnprintf(buf, MSL, "%2d) %s (%lld) -> %s (%lld) [%lld]" NL, num, mobname,
				pReset->arg1, roomname, pReset->arg3, pReset->arg2);
		break;
	case 'E':
		if (!mob)
			strcpy(mobname, "* ERROR: NO MOBILE! *");
		if ((obj = get_obj_index(pReset->arg1)) == NULL)
			strcpy(objname, "Object: *BAD VNUM*");
		else
			strcpy(objname, obj->name);
		swsnprintf(buf, MSL, "%2d) %s (%lld) -> %s (%s) [%lld]" NL, num, objname,
				pReset->arg1, mobname,
				bit_name(wear_types_list,/*(int)*/pReset->arg3),
				/* Trog */
				/* Tu trzeba zmienic! bit_name etc... ma dostawac int64 a nie int, na razie dalem (int)... */
				pReset->arg2);
		break;
	case 'H':
		if (pReset->arg1 > 0 && (obj = get_obj_index(pReset->arg1)) == NULL)
			strcpy(objname, "Object: *BAD VNUM*");
		else if (!obj)
			strcpy(objname, "Object: *NULL obj*");
		swsnprintf(buf, MSL, "%2d) Hide %s (%lld)" NL, num, objname,
				obj ? obj->vnum : pReset->arg1);
		break;
	case 'G':
		if (!mob)
			strcpy(mobname, "* ERROR: NO MOBILE! *");
		if ((obj = get_obj_index(pReset->arg1)) == NULL)
			strcpy(objname, "Object: *BAD VNUM*");
		else
			strcpy(objname, obj->name);
		swsnprintf(buf, MSL, "%2d) %s (%lld) -> %s (carry) [%lld]" NL, num, objname,
				pReset->arg1, mobname, pReset->arg2);
		break;
	case 'O':
		if ((obj = get_obj_index(pReset->arg1)) == NULL)
			strcpy(objname, "Object: *BAD VNUM*");
		else
			strcpy(objname, obj->name);
		room = get_room_index(pReset->arg3);
		if (!room)
			strcpy(roomname, "Room: *BAD VNUM*");
		else
			strcpy(roomname, room->name);
		swsnprintf(buf, MSL, "%2d) (object) %s (%lld) -> %s (%lld) [%lld]" NL, num,
				objname, pReset->arg1, roomname, pReset->arg3, pReset->arg2);
		break;
	case 'P':
		if ((obj2 = get_obj_index(pReset->arg1)) == NULL)
			strcpy(objname, "Object1: *BAD VNUM*");
		else
			strcpy(objname, obj2->name);
		if (pReset->arg3 > 0 && (obj = get_obj_index(pReset->arg3)) == NULL)
			strcpy(roomname, "Object2: *BAD VNUM*");
		else if (!obj)
			strcpy(roomname, "Object2: *NULL obj*");
		else
			strcpy(roomname, obj->name);
		swsnprintf(buf, MSL, "%2d) (Put) %s (%lld) -> %s (%lld) [%lld]" NL, num,
				objname, pReset->arg1, roomname, obj ? obj->vnum : pReset->arg3,
				pReset->arg2);
		break;
	case 'D':
		if (pReset->arg2 < 0 || pReset->arg2 > MAX_DIR + 1)
			pReset->arg2 = 0;
		if ((room = get_room_index(pReset->arg1)) == NULL)
		{
			strcpy(roomname, "Room: *BAD VNUM*");
			sprintf(objname, "%s (no exit)", dir_name[pReset->arg2]);
		}
		else
		{
			strcpy(roomname, room->name);
			sprintf(objname, "%s%s", dir_name[pReset->arg2],
					get_exit(room, pReset->arg2) ? "" : " (NO EXIT!)");
		}
		switch (pReset->arg3)
		{
		default:
			strcpy(mobname, "(* ERROR *)");
			break;
		case 0:
			strcpy(mobname, "Open");
			break;
		case 1:
			strcpy(mobname, "Close");
			break;
		case 2:
			strcpy(mobname, "Close and lock");
			break;
		}
		swsnprintf(buf, MSL, "%2d) %s [%lld] the %s [%lld] door %s (%lld)" NL, num,
				mobname, pReset->arg3, objname, pReset->arg2, roomname,
				pReset->arg1);
		break;
	case 'R':
		if ((room = get_room_index(pReset->arg1)) == NULL)
			strcpy(roomname, "Room: *BAD VNUM*");
		else
			strcpy(roomname, room->name);
		swsnprintf(buf, MSL, "%2d) Randomize exits 0 to %lld -> %s (%lld)" NL, num,
				pReset->arg2, roomname, pReset->arg1);
		break;
	case 'T':
		sprintf(buf, "%2d) TRAP: %d %lld %lld %lld (%s)" NL, num, pReset->extra,
				pReset->arg1, pReset->arg2, pReset->arg3,
				flag_string(trap_flags_list, pReset->extra));
		break;
	}
	if (rlist && (!room || (room && room->vnum != rvnum)))
		return NULL;
	return SWString(buf);
}

/*
 * Stary REDIT zamieni� si� w RSET na pro�b� Aldegarda
 * Stary RSET natomiast zamieni� si� w zawarto�� pliq /dev/null
 */
DEF_DO_FUN( rset )
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *tmp;
	EXTRA_DESCR_DATA *ed;
	EXIT_DATA *xit;
	EXIT_DATA *texit;
	int value;
	int edir, ekey, evnum;

	if (!ch->desc)
	{
		send_to_char("You have no descriptor." NL, ch);
		return;
	}
	location = ch->in_room;

	smash_tilde(argument);
	argument = one_argument(argument, arg);

	if (arg[0] == '\0' || !str_cmp(arg, "?"))
	{
		send_to_char("Syntax: rset <field> value" NL, ch);
		send_to_char("" NL, ch);
		send_to_char("Field being one of:" NL, ch);
		send_to_char("  name desc ed rmed" NL, ch);
		send_to_char("  exit bexit exdesc exflags exname exkey" NL, ch);
		send_to_char("  flags sector teledelay televnum tunnel" NL, ch);
		/*Trog*/send_to_char("  rlist exdistance nightdesc rmnightdesc" NL, ch);
		return;
	}

	if (!can_rmodify(ch, location))
		return;

	if (!str_cmp(arg, "name"))
	{
		if (argument[0] == '\0')
		{
			send_to_char(
					"Set the room name.  A very brief single line room description." NL,
					ch);
			send_to_char("Usage: rset name <Room summary>" NL, ch);
			return;
		}

		STRDUP(location->name, argument);
		return;
	}

	if (!str_cmp(arg, "desc"))
	{
		string_append(ch, &location->description);
		return;
	}

	if (!str_cmp(arg, "nightdesc")) //by Trog {
	{
		if (!IS_SET(location->area->flags, AFLAG_NIGHTDESCS))
		{
			send_to_char(
					"This areafile does not support nighttime descriptions" NL,
					ch);
			return;
		}
		STRDUP(location->nightdesc, "");
		string_append(ch, &location->nightdesc);

		return;
	}

	if (!str_cmp(arg, "rmnightdesc"))
	{
		if (!IS_SET(location->area->flags, AFLAG_NIGHTDESCS))
		{
			send_to_char(
					"This areafile does not support nighttime descriptions" NL,
					ch);
			return;
		}
		STRDUP(location->nightdesc, "");
		return;
	}

	if (!str_cmp(arg, "tunnel"))
	{
		if (!argument || argument[0] == '\0')
		{
			send_to_char(
					"Set the maximum characters allowed in the room at one time. (0 = unlimited)." NL,
					ch);
			send_to_char("Usage: rset tunnel <value>" NL, ch);
			return;
		}
		location->tunnel = URANGE(0, atoi(argument), 1000);
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg, "ed"))
	{
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Create an extra description." NL, ch);
			send_to_char("You must supply keyword(s)." NL, ch);
			return;
		}
		CHECK_SUBRESTRICTED(ch);
		ed = SetRExtra(location, argument);
		if (ch->substate == SUB_REPEATCMD)
			ch->tempnum = SUB_REPEATCMD;
		else
			ch->tempnum = SUB_NONE;
		string_append(ch, &ed->description);

		return;
	}

	if (!str_cmp(arg, "rmed"))
	{
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Remove an extra description." NL, ch);
			send_to_char("You must supply keyword(s)." NL, ch);
			return;
		}
		if (DelRExtra(location, argument))
			send_to_char("Deleted." NL, ch);
		else
			send_to_char("Not found." NL, ch);
		return;
	}

	if (!str_cmp(arg, "rlist"))
	{
		const char *bptr;
		int num;

		if (location->resets.empty())
		{
			send_to_char("This location has no resets to list." NL, ch);
			return;
		}
		num = 0;
		for (auto* pReset : location->resets)
		{
			num++;
			const SWString &sreset = sprint_reset(ch, pReset, num, true);
			if ((bptr = sreset.c_str()) == NULL)
				continue;
			send_to_char(bptr, ch);
		}
		return;
	}

	if (!str_cmp(arg, "flags"))
	{
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Toggle the room flags." NL, ch);
			send_to_char("Usage: rset flags <flag> [flag]..." NL, ch);
			send_to_char("\n\rPossible Flags: " NL, ch);
			send_to_char("dark, nomob, indoors, noforce, bank," NL, ch);
			send_to_char(
					"private, safe, petshop, norecall, donation, nodropall, silence," NL,
					ch);
			send_to_char(
					"logspeach, nodrop, clanstoreroom, plr_home, empty_home, teleport" NL,
					ch);
			send_to_char(
					"nofloor, prototype, refinery, factory, republic_recruit, empire_recruit" NL,
					ch);
			send_to_char(
					"spacecraft, auction, no_drive, can_land, can_fly, hotel, shipdevice" NL,
					ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg2);
			value = flag_value(room_flags_list, arg2);

			if (value < 0)
				ch_printf(ch, "Unknown flag: %s" NL, arg2);
			else if (value == ROOM_PLR_HOME && get_trust(ch) < LEVEL_SUPREME)
				send_to_char(
						"If you want to build a player home use the 'empty_home' flag instead." NL,
						ch);
			else
				TOGGLE_BIT(location->room_flags, value);
		}
		return;
	}

	if (!str_cmp(arg, "teledelay"))
	{
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Set the delay of the teleport. (0 = off)." NL, ch);
			send_to_char("Usage: rset teledelay <value>" NL, ch);
			return;
		}
		location->tele_delay = atoi(argument);
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg, "televnum"))
	{
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Set the vnum of the room to teleport to." NL, ch);
			send_to_char("Usage: rset televnum <vnum>" NL, ch);
			return;
		}
		location->tele_vnum = atoi(argument);
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg, "sector"))
	{
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Set the sector type." NL, ch);
			send_to_char("Usage: rset sector <value>" NL, ch);
			send_to_char("\n\rSector Values:" NL, ch);
			send_to_char(
					"0:dark, 1:city, 2:field, 3:forest, 4:hills, 5:mountain, 6:water_swim" NL,
					ch);
			send_to_char(
					"7:water_noswim, 8:underwater, 9:air, 10:desert, 11:unkown, 12:oceanfloor, 13:underground" NL,
					ch);

			return;
		}
		location->sector_type = atoi(argument);
		if (location->sector_type < 0 || location->sector_type >= SECT_MAX)
		{
			location->sector_type = 1;
			send_to_char("Out of range\n\r.", ch);
		}
		else
			send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg, "exkey"))
	{
		argument = one_argument(argument, arg2);
		argument = one_argument(argument, arg3);
		if (arg2[0] == '\0' || arg3[0] == '\0')
		{
			send_to_char("Usage: rset exkey <dir> <key vnum>" NL, ch);
			return;
		}
		if (arg2[0] == '#')
		{
			edir = atoi(arg2 + 1);
			xit = get_exit_num(location, edir);
		}
		else
		{
			edir = get_dir(arg2);
			xit = get_exit(location, edir);
		}
		value = atoi(arg3);
		if (!xit)
		{
			send_to_char(
					"No exit in that direction.  Use 'rset exit ...' first." NL,
					ch);
			return;
		}
		xit->key = value;
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg, "exname"))
	{
		argument = one_argument(argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char("Change or clear exit keywords." NL, ch);
			send_to_char("Usage: rset exname <dir> [keywords]" NL, ch);
			return;
		}
		if (arg2[0] == '#')
		{
			edir = atoi(arg2 + 1);
			xit = get_exit_num(location, edir);
		}
		else
		{
			edir = get_dir(arg2);
			xit = get_exit(location, edir);
		}
		if (!xit)
		{
			send_to_char(
					"No exit in that direction.  Use 'rset exit ...' first." NL,
					ch);
			return;
		}
		STRDUP(xit->keyword, argument);
		send_to_char("Done." NL, ch);
		return;
	}

	if (!str_cmp(arg, "exflags"))
	{
		if (!argument || argument[0] == '\0')
		{
			send_to_char("Toggle or display exit flags." NL, ch);
			send_to_char("Usage: rset exflags <dir> <flag> [flag]..." NL, ch);
			send_to_char("\n\rExit flags:" NL, ch);
			send_to_char(
					"isdoor, closed, locked, can_look, searchable, can_leave, can_climb," NL,
					ch);
			send_to_char(
					"nopassdoor, secret, pickproof, fly, climb, dig, window, auto, can_enter" NL,
					ch);
			send_to_char("hidden, no_mob, bashproof, bashed" NL, ch);

			return;
		}
		argument = one_argument(argument, arg2);
		if (arg2[0] == '#')
		{
			edir = atoi(arg2 + 1);
			xit = get_exit_num(location, edir);
		}
		else
		{
			edir = get_dir(arg2);
			xit = get_exit(location, edir);
		}
		if (!xit)
		{
			send_to_char(
					"No exit in that direction.  Use 'rset exit ...' first." NL,
					ch);
			return;
		}
		if (argument[0] == '\0')
		{
			sprintf(buf,
					"Flags for exit direction: %d  Keywords: %s  Key: %d\n\r[ ",
					xit->vdir, xit->keyword, xit->key);

			strcat(buf, flag_string(exit_flags_list, xit->orig_flags));
			strcat(buf, "]" NL);
			send_to_char(buf, ch);
			return;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg2);
			value = flag_value(exit_flags_list, arg2);

			if (value < 0 || value > MAX_EXFLAG)
				ch_printf(ch, "Unknown flag: %s" NL, arg2);
			else
				TOGGLE_BIT(xit->orig_flags, value);
		}
		return;
	}

	if (!str_cmp(arg, "exit"))
	{
		bool addexit, numnotdir;

		argument = one_argument(argument, arg2);
		argument = one_argument(argument, arg3);
		if (!*arg2)
		{
			send_to_char("Create, change or remove an exit." NL, ch);
			send_to_char(
					"Usage: rset exit <dir> [room] [flags] [key] [keywords]" NL,
					ch);
			return;
		}
		addexit = numnotdir = false;
		switch (arg2[0])
		{
		default:
			edir = get_dir(arg2);
			break;
		case '+':
			edir = get_dir(arg2 + 1);
			addexit = true;
			break;
		case '#':
			edir = atoi(arg2 + 1);
			numnotdir = true;
			break;
		}
		if (!*arg3)
			evnum = 0;
		else
			evnum = atoi(arg3);
		if (numnotdir)
		{
			if ((xit = get_exit_num(location, edir)) != NULL)
				edir = xit->vdir;
		}
		else
			xit = get_exit(location, edir);
		if (!evnum)
		{
			if (xit)
			{
				extract_exit(location, xit);
				send_to_char("Exit removed." NL, ch);
				return;
			}
			send_to_char("No exit in that direction." NL, ch);
			return;
		}
		if (evnum < 1 || evnum > MAX_VNUM)
		{
			send_to_char("Invalid room number." NL, ch);
			return;
		}
		if ((tmp = get_room_index(evnum)) == NULL)
		{
			send_to_char("Non-existant room." NL, ch);
			return;
		}
		if (get_trust(ch) <= LEVEL_IMMORTAL && tmp->area != location->area)
		{
			send_to_char("You can't make an exit to that room." NL, ch);
			return;
		}
		if (addexit || !xit)
		{
			if (numnotdir)
			{
				send_to_char("Cannot add an exit by number, sorry." NL, ch);
				return;
			}
			if (addexit && xit && get_exit_to(location, edir, tmp->vnum))
			{
				send_to_char(
						"There is already an exit in that direction leading to that location." NL,
						ch);
				return;
			}
			xit = make_exit(location, tmp, edir);
			STRDUP(xit->keyword, "");
			STRDUP(xit->description, "");
			xit->key = -1;
			xit->orig_flags = 0;
			act( COL_IMMORT, "$n reveals a hidden passage!", ch, NULL, NULL,
			TO_ROOM);
		}
		else
			act( COL_IMMORT, "Something is different...", ch, NULL, NULL,
			TO_ROOM);
		if (xit->to_room != tmp)
		{
			xit->to_room = tmp;
			xit->vnum = evnum;
			texit = get_exit_to(xit->to_room, rev_dir[edir], location->vnum);
			if (texit)
			{
				texit->rexit = xit;
				xit->rexit = texit;
			}
		}
		argument = one_argument(argument, arg3);
		if (*arg3)
//	    xit->orig_flags = atoll( arg3 ); To jakies zjebane bylo, kto by chcial ustawiac flagi jako liczbe o.O
			send_to_char(
					"Sorry, orig_flags weren't changed. Use redit like a normal builder." NL,
					ch);
		if (argument && argument[0] != '\0')
		{
			one_argument(argument, arg3);
			ekey = atoi(arg3);
			if (ekey != 0 || arg3[0] == '0')
			{
				argument = one_argument(argument, arg3);
				xit->key = ekey;
			}
			if (argument && argument[0] != '\0')
				STRDUP(xit->keyword, argument);
		}
		send_to_char("Done." NL, ch);
		return;
	}

	/*
	 * Twisted and evil, but works				-Thoric
	 * Makes an exit, and the reverse in one shot.
	 */
	if (!str_cmp(arg, "bexit"))
	{
		EXIT_DATA *xit, *rxit;
		char tmpcmd[MIL] = {0};
		ROOM_INDEX_DATA *tmploc;
		int vnum, exnum;
		char rvnum[MIL] = {0};
		bool numnotdir;

		argument = one_argument(argument, arg2);
		argument = one_argument(argument, arg3);
		if (!*arg2)
		{
			send_to_char("Create, change or remove a two-way exit." NL, ch);
			send_to_char(
					"Usage: rset bexit <dir> [room] [flags] [key] [keywords]" NL,
					ch);
			return;
		}
		numnotdir = false;
		switch (arg2[0])
		{
		default:
			edir = get_dir(arg2);
			break;
		case '#':
			numnotdir = true;
			edir = atoi(arg2 + 1);
			break;
		case '+':
			edir = get_dir(arg2 + 1);
			break;
		}
		tmploc = location;
		exnum = edir;
		if (numnotdir)
		{
			if ((xit = get_exit_num(tmploc, edir)) != NULL)
				edir = xit->vdir;
		}
		else
			xit = get_exit(tmploc, edir);
		rxit = NULL;
		vnum = 0;
		rvnum[0] = '\0';
		if (xit)
		{
			vnum = xit->vnum;
			if (arg3[0] != '\0')
				sprintf(rvnum, "%d", tmploc->vnum);
			if (xit->to_room)
				rxit = get_exit(xit->to_room, rev_dir[edir]);
			else
				rxit = NULL;
		}
		swsnprintf(tmpcmd, MIL, "exit %s %s %s", arg2, arg3, argument);
		do_rset(ch, tmpcmd);
		if (numnotdir)
			xit = get_exit_num(tmploc, exnum);
		else
			xit = get_exit(tmploc, edir);
		if (!rxit && xit)
		{
			vnum = xit->vnum;
			if (arg3[0] != '\0')
				sprintf(rvnum, "%d", tmploc->vnum);
			if (xit->to_room)
				rxit = get_exit(xit->to_room, rev_dir[edir]);
			else
				rxit = NULL;
		}
		if (vnum)
		{
			swsnprintf(tmpcmd, MIL, "%d rset exit %d %s %s", vnum, rev_dir[edir], rvnum,
					argument);
			do_at(ch, tmpcmd);
		}
		return;
	}

	if (!str_cmp(arg, "exdistance"))
	{
		argument = one_argument(argument, arg2);
		if (!*arg2)
		{
			send_to_char(
					"Set the distance (in rooms) between this room, and the destination room." NL,
					ch);
			send_to_char("Usage: rset exdistance <dir> [distance]" NL, ch);
			return;
		}
		if (arg2[0] == '#')
		{
			edir = atoi(arg2 + 1);
			xit = get_exit_num(location, edir);
		}
		else
		{
			edir = get_dir(arg2);
			xit = get_exit(location, edir);
		}
		if (xit)
		{
			xit->distance = URANGE(1, atoi(argument), 50);
			send_to_char("Done." NL, ch);
			return;
		}
		send_to_char(
				"No exit in that direction.  Use 'rset exit ...' first." NL,
				ch);
		return;
	}

	if (!str_cmp(arg, "exdesc"))
	{
		argument = one_argument(argument, arg2);
		if (!*arg2)
		{
			send_to_char("Create or clear a description for an exit." NL, ch);
			send_to_char("Usage: rset exdesc <dir> [description]" NL, ch);
			return;
		}
		if (arg2[0] == '#')
		{
			edir = atoi(arg2 + 1);
			xit = get_exit_num(location, edir);
		}
		else
		{
			edir = get_dir(arg2);
			xit = get_exit(location, edir);
		}
		if (xit)
		{
			if (!argument || argument[0] == '\0')
				STRDUP(xit->description, "");
			else
			{
				sprintf(buf, "%s" NL, argument);
				STRDUP(xit->description, buf);
			}
			send_to_char("Done." NL, ch);
			return;
		}
		send_to_char(
				"No exit in that direction.  Use 'rset exit ...' first." NL,
				ch);
		return;
	}

	do_rset(ch, (char*) "");
	return;
}

EXTRA_DESCR_DATA* SetRExtra(ROOM_INDEX_DATA *room, char *keywords)
{
	for (auto* ed : room->extradesc)
	{
		if (is_name(keywords, ed->keyword))
			return ed;
	}
	{
		EXTRA_DESCR_DATA *ed;
		CREATE(ed, EXTRA_DESCR_DATA, 1);
		room->extradesc.push_back(ed);
		STRDUP(ed->keyword, keywords);
		STRDUP(ed->description, "");
		top_ed++;
		return ed;
	}
}

bool DelRExtra(ROOM_INDEX_DATA *room, char *keywords)
{
	for (auto* rmed : room->extradesc)
	{
		if (is_name(keywords, rmed->keyword))
		{
			room->extradesc.remove(rmed);
			free_ed(rmed);
			return true;
		}
	}
	return false;
}

EXTRA_DESCR_DATA* SetOExtra(OBJ_DATA *obj, const char *keywords)
{
	return SetOExtra(obj, (char*) keywords);
}

EXTRA_DESCR_DATA* SetOExtra(OBJ_DATA *obj, char *keywords)
{
	for (auto* ed : obj->extradesc)
	{
		if (is_name(keywords, ed->keyword))
			return ed;
	}
	{
		EXTRA_DESCR_DATA *ed;
		CREATE(ed, EXTRA_DESCR_DATA, 1);
		obj->extradesc.push_back(ed);
		STRDUP(ed->keyword, keywords);
		STRDUP(ed->description, "");
		top_ed++;
		return ed;
	}
}

bool DelOExtra(OBJ_DATA *obj, char *keywords)
{
	for (auto* rmed : obj->extradesc)
	{
		if (is_name(keywords, rmed->keyword))
		{
			obj->extradesc.remove(rmed);
			free_ed(rmed);
			return true;
		}
	}
	return false;
}

EXTRA_DESCR_DATA* SetOExtraProto(OBJ_INDEX_DATA *obj, char *keywords)
{
	for (auto* ed : obj->extradesc)
	{
		if (is_name(keywords, ed->keyword))
			return ed;
	}
	{
		EXTRA_DESCR_DATA *ed;
		CREATE(ed, EXTRA_DESCR_DATA, 1);
		obj->extradesc.push_back(ed);
		STRDUP(ed->keyword, keywords);
		STRDUP(ed->description, "");
		top_ed++;
		return ed;
	}
}

bool DelOExtraProto(OBJ_INDEX_DATA *obj, char *keywords)
{
	for (auto* rmed : obj->extradesc)
	{
		if (is_name(keywords, rmed->keyword))
		{
			obj->extradesc.remove(rmed);
			free_ed(rmed);
			return true;
		}
	}
	return false;
}

/* Added by Thanos Nowy Format sekcji */
void adv_save_mobiles(AREA_DATA *tarea, FILE *fpout, bool install)
{
	int vnum;
	MOB_INDEX_DATA *pMobIndex;

	fprintf(fpout, "#MOBILES\n");
	for (vnum = tarea->lvnum; vnum <= tarea->uvnum; vnum++)
	{
		if ((pMobIndex = get_mob_index(vnum)) == NULL)
			continue;
		if (install)
			REMOVE_BIT(pMobIndex->act, ACT_PROTOTYPE);
		fprintf(fpout, "#%d\n", vnum);
		fprintf(fpout, "%s~\n", pMobIndex->player_name);
		fprintf(fpout, "%s@%s@%s@%s@%s@%s~\n", pMobIndex->przypadki[0],
				pMobIndex->przypadki[1], pMobIndex->przypadki[2],
				pMobIndex->przypadki[3], pMobIndex->przypadki[4],
				pMobIndex->przypadki[5]);
		fprintf(fpout, "%s~\n", strip_cr(pMobIndex->long_descr));
		fprintf(fpout, "%s~\n", strip_cr(pMobIndex->description));
		fprintf(fpout, "%lld %lld %d 0\n", pMobIndex->act,
				pMobIndex->affected_by, pMobIndex->alignment);
		fprintf(fpout, "%d %d\n", pMobIndex->gold, pMobIndex->level);
		fprintf(fpout, "%d %d %s~\n", pMobIndex->position, pMobIndex->sex,
				pMobIndex->race->name);

		if (pMobIndex->perm_str != 13 || pMobIndex->perm_int != 13
				|| pMobIndex->perm_wis != 13 || pMobIndex->perm_dex != 13
				|| pMobIndex->perm_con != 13 || pMobIndex->perm_cha != 13
				|| pMobIndex->perm_lck != 13)
			fprintf(fpout, "A %d %d %d %d %d %d %d\n", pMobIndex->perm_str,
					pMobIndex->perm_int, pMobIndex->perm_wis,
					pMobIndex->perm_dex, pMobIndex->perm_con,
					pMobIndex->perm_cha, pMobIndex->perm_lck);

		if (pMobIndex->saving_poison_death || pMobIndex->saving_wand
				|| pMobIndex->saving_para_petri || pMobIndex->saving_breath
				|| pMobIndex->saving_spell_staff)
			fprintf(fpout, "S %d %d %d %d %d\n", pMobIndex->saving_poison_death,
					pMobIndex->saving_wand, pMobIndex->saving_para_petri,
					pMobIndex->saving_breath, pMobIndex->saving_spell_staff);

		fprintf(fpout, "R %d %d %d %d %d\n", pMobIndex->height,
				pMobIndex->weight, 0, //pMobIndex->speaks,
				0, //pMobIndex->speaking,
				pMobIndex->numattacks);

		if (pMobIndex->hitroll || pMobIndex->damroll || pMobIndex->xflags
				|| pMobIndex->resistant || pMobIndex->immune
				|| pMobIndex->susceptible || pMobIndex->attacks
				|| pMobIndex->defenses)
			fprintf(fpout, "X %d %d %lld %lld %lld %lld %lld %lld\n",
					pMobIndex->hitroll, pMobIndex->damroll, pMobIndex->xflags,
					pMobIndex->resistant, pMobIndex->immune,
					pMobIndex->susceptible, pMobIndex->attacks,
					pMobIndex->defenses);

		if (pMobIndex->mobthac0 || pMobIndex->ac != 0 || pMobIndex->hitnodice
				|| pMobIndex->hitsizedice || pMobIndex->hitplus
				|| pMobIndex->damnodice || pMobIndex->damsizedice
				|| pMobIndex->damplus)
		{
			fprintf(fpout, "T %d %d ", pMobIndex->mobthac0, pMobIndex->ac);
			fprintf(fpout, "%dd%d+%d ", pMobIndex->hitnodice,
					pMobIndex->hitsizedice, pMobIndex->hitplus);
			fprintf(fpout, "%dd%d+%d\n", pMobIndex->damnodice,
					pMobIndex->damsizedice, pMobIndex->damplus);
		}

		fprintf(fpout, "0 0 0 0 0 0 0 0\n");

		if (pMobIndex->s_vip_flags && pMobIndex->s_vip_flags[0] != '\0')
			fprintf(fpout, "V %s~\n", pMobIndex->s_vip_flags);
		fprintf(fpout, "Dialog    %s~\n", pMobIndex->dialog_name);

		if (!pMobIndex->mudprogs.empty())
		{
			for (auto* mprog : pMobIndex->mudprogs)
				fprintf(fpout, "> %s %s~\n%s~\n",
						mprog_type_to_name(mprog->type), mprog->arglist,
						strip_cr(mprog->comlist));
			fprintf(fpout, "|\n");
		}
	}
	fprintf(fpout, "#0\n\n\n");
	if (install && vnum < tarea->uvnum)
		tarea->uvnum = vnum - 1;
	return;
}
/* done */

void vsave_resets(FILE *fp, AREA_DATA *pArea)
{
	ROOM_INDEX_DATA *pRoomIndex;
	int vnum;

	fprintf(fp, "#RESETS\n");

	for (vnum = pArea->lvnum; vnum <= pArea->uvnum; vnum++)
	{
		if ((pRoomIndex = get_room_index(vnum)))
		{
			if (pRoomIndex->area == pArea)
			{
				for (auto* pReset : pRoomIndex->resets)
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
								UPPER(pReset->command), pReset->extra,
								pReset->arg1, pReset->arg2, pReset->arg3);
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
		}
	}
	fprintf(fp, "S\n\n\n");
	return;
}

void fold_area(AREA_DATA *tarea, char *filename, bool install)
{
	ROOM_INDEX_DATA *room;
	MOB_INDEX_DATA *pMobIndex;
	OBJ_INDEX_DATA *pObjIndex;
	SHOP_DATA *pShop;
	REPAIR_DATA *pRepair;
	char buf[MAX_STRING_LENGTH];
	FILE *fpout;
	int vnum;
	int val0, val1, val2, val3, val4, val5;
	return;
	sprintf(buf, "Saving %s...", tarea->filename);
	log_string_plus(buf, LOG_NORMAL, LEVEL_GREATER);

	sprintf(buf, "%s%s", AREA_BAK_DIR, filename);
	rename(filename, buf);
	RESERVE_CLOSE;

	sprintf(buf, "%s%s", AREA_DIR, filename);
	if ((fpout = fopen(buf, "w")) == NULL)
	{
		bug("fopen");
		perror(filename);
		RESERVE_OPEN;
		return;
	}

	//byTrog
	fprintf(fpout, "#SWAREADATA\n");
	fprintf(fpout, "Name        %s~\n", tarea->name);
	fprintf(fpout, "Authors     %s~\n", tarea->author);
	fprintf(fpout, "Builders    %s~\n", tarea->builder ? tarea->builder : "");
//temporary builder check
	fprintf(fpout, "Security    %d\n", tarea->security);
	fprintf(fpout, "Vnums       %d %d\n", tarea->lvnum, tarea->uvnum);
	fprintf(fpout, "AreaFlags   %lld\n", tarea->flags);
	fprintf(fpout, "Economy     %d %d\n", tarea->low_economy,
			tarea->high_economy);
	fprintf(fpout, "Resetfreq   %d\n", tarea->reset_frequency);
	fprintf(fpout, "Resetmsg    %s~\n", tarea->resetmsg ? tarea->resetmsg : "");
//temporary resetmsg check
	fprintf(fpout, "Ranges      %d %d\n", tarea->low_range, tarea->high_range);
	fprintf(fpout, "End\n\n\n");

	/* save mobiles */
	adv_save_mobiles(tarea, fpout, install);

	/* save objects */
	/*
	 fprintf( fpout, "#OBJECTS\n" );
	 */
	fprintf(fpout, "#SWOBJECTS\n");
	for (vnum = tarea->lvnum; vnum <= tarea->uvnum; vnum++)
	{
		if ((pObjIndex = get_obj_index(vnum)) == NULL)
			continue;

		if (install)
			REMOVE_BIT(pObjIndex->extra_flags, ITEM_PROTOTYPE);

		fprintf(fpout, "#%d\n", vnum);
		fprintf(fpout, "%s~\n", pObjIndex->name);
		fprintf(fpout, "%s@%s@%s@%s@%s@%s~\n", pObjIndex->przypadki[0],
				pObjIndex->przypadki[1], pObjIndex->przypadki[2],
				pObjIndex->przypadki[3], pObjIndex->przypadki[4],
				pObjIndex->przypadki[5]);
		fprintf(fpout, "%s~\n", pObjIndex->description);
		fprintf(fpout, "%s~\n", pObjIndex->action_desc);
		fprintf(fpout, "%d %lld %lld %d 0 0\n", pObjIndex->item_type,
				pObjIndex->extra_flags, pObjIndex->wear_flags,
				pObjIndex->layers);

		val0 = pObjIndex->value[0];
		val1 = pObjIndex->value[1];
		val2 = pObjIndex->value[2];
		val3 = pObjIndex->value[3];
		val4 = pObjIndex->value[4];
		val5 = pObjIndex->value[5];

		switch (pObjIndex->item_type)
		{
		default:
			fprintf(fpout, "%d~ %d~ %d~ %d~ %d~ %d~\n", val0, val1, val2, val3,
					val4, val5);
			break;

		case ITEM_PILL:
		case ITEM_POTION:
		case ITEM_DEVICE:
			fprintf(fpout, "%d~ %d~ %d~ %s~ %d~ %d~\n", val0, val1, val2,
			IS_VALID_SN(val3) ? skill_table[val3]->name : " ", val4, val5);
			break;
		case ITEM_SALVE:
			fprintf(fpout, "%d~ %d~ %d~ %d~ %s~ %s~\n", val0, val1, val2, val3,
			IS_VALID_SN(val4) ? skill_table[val4]->name : " ",
			IS_VALID_SN(val5) ? skill_table[val5]->name : " ");
			break;
		}

		fprintf(fpout, "%d %d %d\n", pObjIndex->weight, pObjIndex->cost,
				pObjIndex->gender); //added by Thanos

		// Thanos: wprowadzamy levele przedmiot�w
		fprintf(fpout, "0 0 %d 0 0 ~\n", pObjIndex->level);

		for (auto* ed : pObjIndex->extradesc)
			fprintf(fpout, "E\n%s~\n%s~\n", ed->keyword,
					strip_cr(ed->description));

		for (auto* req : pObjIndex->requirements)
		{
			fprintf(fpout, "R %d 0 %d 0 %s~\n", req->location, req->modifier,
					(req->location == REQ_SKILL && IS_VALID_SN(req->type)) ?
							skill_table[req->type]->name : " ");
		}

		for (auto* paf : pObjIndex->affects)
			fprintf(fpout, "A\n%d %d\n", paf->location,
					((paf->location == APPLY_WEAPONSPELL
							|| paf->location == APPLY_WEARSPELL
							|| paf->location == APPLY_REMOVESPELL
							|| paf->location == APPLY_STRIPSN)
							&& IS_VALID_SN(paf->modifier)) ?
							skill_table[paf->modifier]->slot : paf->modifier);

		if (!pObjIndex->mudprogs.empty())
		{
			for (auto* mprog : pObjIndex->mudprogs)
				fprintf(fpout, "> %s %s~\n%s~\n",
						mprog_type_to_name(mprog->type), mprog->arglist,
						strip_cr(mprog->comlist));
			fprintf(fpout, "|\n");
		}
	}
	fprintf(fpout, "#0\n\n\n");
	if (install && vnum < tarea->uvnum)
		tarea->uvnum = vnum - 1;

	/* save rooms   */
	fprintf(fpout, "#ROOMS\n");
	for (vnum = tarea->lvnum; vnum <= tarea->uvnum; vnum++)
	{
		if ((room = get_room_index(vnum)) == NULL)
			continue;
		if (install)
		{
			/* remove prototype flag from room */
			REMOVE_BIT(room->room_flags, ROOM_PROTOTYPE);
			/* purge room of (prototyped) mobiles */
			{
				auto snapshot = room->people;
				for (auto* victim : snapshot)
					if (IS_NPC(victim))
						extract_char(victim, true);
			}
			/* purge room of (prototyped) objects */
			{
				auto snapshot = room->contents;
				for (auto* obj : snapshot)
					extract_obj(obj);
			}
		}
		fprintf(fpout, "#%d\n", vnum);
		fprintf(fpout, "%s~\n", room->name);
		fprintf(fpout, "%s~\n", strip_cr(room->description));

		if (IS_SET(tarea->flags, AFLAG_NIGHTDESCS)) //by Trog
			fprintf(fpout, "%s~\n",
					(room->nightdesc && room->nightdesc[0] != '\0') ?
							strip_cr(room->nightdesc) : "");

		fprintf(fpout, "%d %lld %d %d %d %d\n", room->light, room->room_flags,
				room->sector_type, room->tele_delay, room->tele_vnum,
				room->tunnel);

		for (auto* xit : room->exits)
		{
			if (IS_SET(xit->orig_flags, EX_PORTAL)) /* don't fold portals */
				continue;
			fprintf(fpout, "D%d\n", xit->vdir);
			fprintf(fpout, "%s~\n", strip_cr(xit->description));
			fprintf(fpout, "%s~\n", strip_cr(xit->keyword));
			fprintf(fpout, "%lld %d %d %d\n", xit->orig_flags & ~EX_BASHED,
					xit->key, xit->vnum, xit->distance);
		}

		for (auto* ed : room->extradesc)
			fprintf(fpout, "E\n%s~\n%s~\n", ed->keyword,
					strip_cr(ed->description));

		if (!room->mudprogs.empty())
		{
			for (auto* mprog : room->mudprogs)
				fprintf(fpout, "> %s %s~\n%s~\n",
						mprog_type_to_name(mprog->type), mprog->arglist,
						strip_cr(mprog->comlist));
			fprintf(fpout, "|\n");
		}

		fprintf(fpout, "S\n");
	}

	fprintf(fpout, "#0\n\n\n");
	if (install && vnum < tarea->uvnum)
		tarea->uvnum = vnum - 1;

	/* save resets   */
	vsave_resets(fpout, tarea);

	/* save shops */
	fprintf(fpout, "#SHOPS\n");
	for (vnum = tarea->lvnum; vnum <= tarea->uvnum; vnum++)
	{
		if ((pMobIndex = get_mob_index(vnum)) == NULL)
			continue;
		if ((pShop = pMobIndex->pShop) == NULL)
			continue;
		fprintf(fpout, " %d   %2d %2d %2d %2d %2d   %3d %3d", pShop->keeper,
				pShop->buy_type[0], pShop->buy_type[1], pShop->buy_type[2],
				pShop->buy_type[3], pShop->buy_type[4], pShop->profit_buy,
				pShop->profit_sell);
		fprintf(fpout, "        %2d %2d %5lld  ; %s\n", pShop->open_hour,
				pShop->close_hour, pShop->flags, pMobIndex->przypadki[0]); //changed by Thanos (short_descr/przypadki[0])
	}
	fprintf(fpout, "0\n\n\n");

	/* save repair shops */
	fprintf(fpout, "#REPAIRS\n");
	for (vnum = tarea->lvnum; vnum <= tarea->uvnum; vnum++)
	{
		if ((pMobIndex = get_mob_index(vnum)) == NULL)
			continue;
		if ((pRepair = pMobIndex->rShop) == NULL)
			continue;
		fprintf(fpout, " %d   %2d %2d %2d         %3d %3d", pRepair->keeper,
				pRepair->fix_type[0], pRepair->fix_type[1],
				pRepair->fix_type[2], pRepair->profit_fix, pRepair->shop_type);
		fprintf(fpout, "        %2d %2d    ; %s\n", pRepair->open_hour,
				pRepair->close_hour, pMobIndex->przypadki[0]);
	}
	fprintf(fpout, "0\n\n\n");

	/* save specials */
	fprintf(fpout, "#SPECIALS\n");
	for (vnum = tarea->lvnum; vnum <= tarea->uvnum; vnum++)
	{
		if ((pMobIndex = get_mob_index(vnum)) == NULL)
			continue;
		if (pMobIndex->spec_fun)
			fprintf(fpout, "M  %d %s\n", pMobIndex->vnum,
					lookup_spec(pMobIndex->spec_fun));
		if (pMobIndex->spec_2)
			fprintf(fpout, "M  %d %s\n", pMobIndex->vnum,
					lookup_spec(pMobIndex->spec_2));
	}
	fprintf(fpout, "S\n\n\n");
	/* END */
	fprintf(fpout, "#$\n");
	fclose(fpout);
	RESERVE_OPEN;
	return;
}

/* Zmiany by Thanos: folda bez argumentu saveuje krainke edytowana przez olcmana */
DEF_DO_FUN( foldarea )
{
	AREA_DATA *tarea = NULL;
	char arg[MAX_INPUT_LENGTH];
	void *ed;
	bool found = true; /* !!! */

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		if (ch->desc && (ed = ch->desc->olc_editing))
		{
			switch (ch->desc->connected)
			{
			default:
				break;
			case CON_MEDITOR:
			case CON_MPEDITOR:
				tarea = get_vnum_area(((MOB_INDEX_DATA*) ed)->vnum);
				break;
			case CON_OEDITOR:
			case CON_OPEDITOR:
				tarea = get_vnum_area(((OBJ_INDEX_DATA*) ed)->vnum);
				break;
			case CON_REDITOR:
			case CON_RPEDITOR:
				tarea = get_vnum_area(((ROOM_INDEX_DATA*) ed)->vnum);
				break;
			case CON_AEDITOR:
				tarea = (AREA_DATA*) ed;
				break;
			}
		}

		if (!tarea)
		{
			send_to_char("Usage: foldarea <filename> [remproto]" NL, ch);
			return;
		}
	}
	else
		for (auto* tarea_ : area_list)
		{
			found = false;
			tarea = tarea_;
			if (!str_cmp(tarea->filename, arg))
			{
				found = true;
				break;
			}
		}

	if (tarea && found && is_builder(ch, tarea))
	{
		send_to_char("Folding..." NL, ch);
		/*
		 if (!str_cmp( argument, "remproto") )
		 fold_area( tarea, tarea->filename, true );
		 else
		 fold_area( tarea, tarea->filename, false );
		 */
		save_area(tarea);
		send_to_char("Done." NL, ch);
		return;
	}

	send_to_char("Cannot save such area." NL, ch);
	return;
}

extern int top_area;

void write_area_list()
{
	FILE *fpout;

	fpout = fopen( AREA_LIST, "w");
	if (!fpout)
	{
		bug("FATAL: cannot open area.lst for writing!" NL, 0);
		perror( AREA_LIST);
		return;
	}
	for (auto* tarea : area_list)
		fprintf(fpout, "%s\n", tarea->filename);
	fprintf(fpout, "$\n");
	fclose(fpout);
	return;
}

/*
 * Parse a reset command string into a reset_data structure
 */
RESET_DATA* parse_reset(ROOM_INDEX_DATA *pRoom, char *argument, CHAR_DATA *ch)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char letter;
	int extra, val1, val2, val3;
	int value;
	ROOM_INDEX_DATA *room;
	EXIT_DATA *pexit;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);
	argument = one_argument(argument, arg4);
	extra = 0;
	letter = '*';
	val1 = atoi(arg2);
	val2 = atoi(arg3);
	val3 = atoi(arg4);
	if (arg1[0] == '\0')
	{
		send_to_char(
				"Reset commands: mob obj give equip door rand trap hide." NL,
				ch);
		return NULL;
	}

	if (!str_cmp(arg1, "hide"))
	{
		if (arg2[0] != '\0' && !get_obj_index(val1))
		{
			send_to_char("Reset: HIDE: no such object" NL, ch);
			return NULL;
		}
		else
			val1 = 0;
		extra = 1;
		val2 = 0;
		val3 = 0;
		letter = 'H';
	}
	else if (arg2[0] == '\0')
	{
		send_to_char("Reset: not enough arguments." NL, ch);
		return NULL;
	}
	else if (val1 < 1 || val1 > MAX_VNUM)
	{
		send_to_char("Reset: value out of range." NL, ch);
		return NULL;
	}
	else if (!str_cmp(arg1, "mob"))
	{
		if (!get_mob_index(val1))
		{
			send_to_char("Reset: MOB: no such mobile" NL, ch);
			return NULL;
		}
		val2 = pRoom->vnum;
		if (val3 < 1)
			val3 = 1;
		letter = 'M';
	}
	else if (!str_cmp(arg1, "obj"))
	{
		if (!get_obj_index(val1))
		{
			send_to_char("Reset: OBJ: no such object" NL, ch);
			return NULL;
		}
		val2 = pRoom->vnum;
		if (val3 < 1)
			val3 = 1;
		letter = 'O';
	}
	else if (!str_cmp(arg1, "give"))
	{
		if (!get_obj_index(val1))
		{
			send_to_char("Reset: GIVE: no such object" NL, ch);
			return NULL;
		}
		if (val2 < 1)
			val2 = 1;
		val3 = val2;
		val2 = 0;
		extra = 1;
		letter = 'G';
	}
	else if (!str_cmp(arg1, "equip"))
	{
		if (!get_obj_index(val1))
		{
			send_to_char("Reset: EQUIP: no such object" NL, ch);
			return NULL;
		}
		if (!is_number(arg3))
			val2 = flag_value(wear_types_list, arg3);
		if (val2 < 0 || val2 >= MAX_WEAR)
		{
			send_to_char("Reset: EQUIP: invalid wear location" NL, ch);
			return NULL;
		}
		if (val3 < 1)
			val3 = 1;
		extra = 1;
		letter = 'E';
	}
	else if (!str_cmp(arg1, "put"))
	{
		if (!get_obj_index(val1))
		{
			send_to_char("Reset: PUT: no such object" NL, ch);
			return NULL;
		}
		if (val2 > 0 && !get_obj_index(val2))
		{
			send_to_char("Reset: PUT: no such container" NL, ch);
			return NULL;
		}
		extra = UMAX(val3, 0);
		argument = one_argument(argument, arg4);
		val3 = (is_number(argument) ? atoi(arg4) : 0);
		if (val3 < 0)
			val3 = 0;
		letter = 'P';
	}
	else if (!str_cmp(arg1, "door"))
	{
		if ((room = get_room_index(val1)) == NULL)
		{
			send_to_char("Reset: DOOR: no such room" NL, ch);
			return NULL;
		}
		if (val2 < 0 || val2 > 9)
		{
			send_to_char("Reset: DOOR: invalid exit" NL, ch);
			return NULL;
		}
		if ((pexit = get_exit(room, val2))
				== NULL|| !IS_SET( pexit->orig_flags, EX_ISDOOR ))
		{
			send_to_char("Reset: DOOR: no such door" NL, ch);
			return NULL;
		}
		if (val3 < 0 || val3 > 2)
		{
			send_to_char(
					"Reset: DOOR: invalid door state (0 = open, 1 = close, 2 = lock)" NL,
					ch);
			return NULL;
		}
		letter = 'D';
		value = val3;
		val3 = val2;
		val2 = value;
	}
	else if (!str_cmp(arg1, "rand"))
	{
		if (!get_room_index(val1))
		{
			send_to_char("Reset: RAND: no such room" NL, ch);
			return NULL;
		}
		if (val2 < 0 || val2 > 9)
		{
			send_to_char("Reset: RAND: invalid max exit" NL, ch);
			return NULL;
		}
		val3 = val2;
		val2 = 0;
		letter = 'R';
	}
	else if (!str_cmp(arg1, "trap"))
	{
		if (val2 < 1 || val2 > MAX_TRAPTYPE)
		{
			send_to_char("Reset: TRAP: invalid trap type" NL, ch);
			return NULL;
		}
		if (val3 < 0 || val3 > 10000)
		{
			send_to_char("Reset: TRAP: invalid trap charges" NL, ch);
			return NULL;
		}
		while (argument[0] != '\0')
		{
			argument = one_argument(argument, arg4);
			value = flag_value(trap_flags_list, arg4);
			if (value >= 0)
				SET_BIT(extra, value);
			else
			{
				send_to_char("Reset: TRAP: bad flag" NL, ch);
				return NULL;
			}
		}
		if ( IS_SET(extra, TRAP_ROOM) && IS_SET(extra, TRAP_OBJ))
		{
			send_to_char(
					"Reset: TRAP: Must specify room OR object, not both!" NL,
					ch);
			return NULL;
		}
		if ( IS_SET(extra, TRAP_ROOM) && !get_room_index(val1))
		{
			send_to_char("Reset: TRAP: no such room" NL, ch);
			return NULL;
		}
		if ( IS_SET(extra, TRAP_OBJ) && val1 > 0 && !get_obj_index(val1))
		{
			send_to_char("Reset: TRAP: no such object" NL, ch);
			return NULL;
		}
		if (!IS_SET(extra, TRAP_ROOM) && !IS_SET(extra, TRAP_OBJ))
		{
			send_to_char("Reset: TRAP: Must specify ROOM or OBJECT" NL, ch);
			return NULL;
		}
		/* fix order */
		value = val1;
		val1 = val2;
		val2 = value;
		letter = 'T';
	}
	if (letter == '*')
		return NULL;
	else
		return make_reset(letter, extra, val1, val3, val2);
}

DEF_DO_FUN( astat )
{
	AREA_DATA *tarea;
	bool proto, found;

	found = false;
	for (auto* ta : area_list)
		if (!str_cmp(ta->filename, argument))
		{
			tarea = ta;
			found = true;
			break;
		}

	if (!found)
		for (auto* ta : build_list)
			if (!str_cmp(ta->filename, argument))
			{
				tarea = ta;
				found = true;
				proto = true;
				break;
			}

	if (!found)
	{
		if (argument && argument[0] != '\0')
		{
			send_to_char("Area not found.  Check 'zones'." NL, ch);
			return;
		}
		else
		{
			tarea = ch->in_room->area;
		}
	}

	ch_printf(ch,
	FG_CYAN "Name:" PLAIN " %s   " EOL
	FG_CYAN "Planet:" PLAIN, strip_colors(tarea->name, 40));

	if (tarea->planet && tarea->planet->name)
		ch_printf(ch, " %s   " EOL, tarea->planet->name);
	else
		ch_printf(ch, " brak planety " EOL);

	ch_printf(ch,
	FG_CYAN "Filename:" PLAIN " %-20s  "
	FG_CYAN "Prototype:" PLAIN " %s" NL, tarea->filename,
	IS_SET( tarea->flags, AFLAG_PROTOTYPE ) ? "yes" : "no");
	ch_printf(ch,
	FG_CYAN "Security:" PLAIN " %2d"
	FG_CYAN ", Author:" PLAIN " %s "
	FG_CYAN "Builders:" PLAIN " %s." NL, tarea->security, tarea->author,
			tarea->builder);
	if (!proto)
	{
		ch_printf(ch,
		FG_CYAN "Max players:" PLAIN " %d  "
		FG_CYAN "IllegalPks:" PLAIN " %d  "
		FG_CYAN "Credits Looted:" PLAIN " %d" NL, tarea->max_players,
				tarea->illegal_pk, tarea->gold_looted);
		if (tarea->high_economy)
			ch_printf(ch, "Area economy: %d billion and %d credits." NL,
					tarea->high_economy, tarea->low_economy);
		else
			ch_printf(ch, "Area economy: %d credits." NL, tarea->low_economy);
		if (tarea->planet)
			ch_printf(ch, "Planet: %s." NL, tarea->planet->name);
		ch_printf(ch,
		FG_CYAN "Mdeaths:" PLAIN " %d  "
		FG_CYAN "Mkills:" PLAIN " %d  "
		FG_CYAN "Pdeaths:" PLAIN " %d  "
		FG_CYAN "Pkills: " PLAIN "%d" NL, tarea->mdeaths, tarea->mkills,
				tarea->pdeaths, tarea->pkills);
	}
	ch_printf(ch,
	FG_CYAN "Age:" PLAIN " %d   " FG_CYAN "Number of players:" PLAIN " %d" NL,
			tarea->age, tarea->nplayer);

	ch_printf(ch, FG_CYAN "Area flags:" PLAIN " %s" NL,
			flag_string(area_flags_list, tarea->flags));
	ch_printf(ch, FG_CYAN "Vnums: " PLAIN "%5d " FG_CYAN "-" PLAIN " %d" NL,
			tarea->lvnum, tarea->uvnum);
	ch_printf(ch,
	FG_CYAN "Level range: " PLAIN "%d " FG_CYAN "-" PLAIN " %d" NL,
			tarea->low_range, tarea->high_range);
	ch_printf(ch, FG_CYAN "Resetmsg:" PLAIN " %s" EOL,
			*tarea->resetmsg ? tarea->resetmsg : "(default)"); /* Rennard */
	ch_printf(ch,
	FG_CYAN "Reset frequency: " PLAIN "%d" FG_CYAN " minutes." EOL,
			tarea->reset_frequency ? tarea->reset_frequency : DEF_RESETFREQ);
}

DEF_DO_FUN( rlist )
{
	ROOM_INDEX_DATA *room;
	AREA_DATA *tarea;
	int vnum;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int lrange;
	int trange;

	tarea = (AREA_DATA*) ch->desc->olc_editing;
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (tarea)
	{
		if (arg1[0] == '\0') /* cleaned a big scary mess */
			lrange = tarea->lvnum; /* here.	    -Thoric */
		else
			lrange = atoi(arg1);
		if (arg2[0] == '\0')
			trange = tarea->uvnum;
		else
			trange = atoi(arg2);

		if ((lrange < tarea->lvnum || trange > tarea->uvnum)
				&& get_trust(ch) < LEVEL_GREATER)
		{
			send_to_char("That is out of your vnum range." NL, ch);
			return;
		}
	}
	else
	{
		lrange = ( is_number(arg1) ? atoi(arg1) : 1);
		trange = ( is_number(arg2) ? atoi(arg2) : 1);
	}

	for (vnum = lrange; vnum <= trange; vnum++)
	{
		if (!(room = get_room_index(vnum)))
			continue;
		pager_printf(ch, "%5d) (%s) %s" EOL, vnum,
		IS_SET( room->room_flags, ROOM_PROTOTYPE ) ?
		FG_GREEN "P" PLAIN :
														" ", room->name);
	}
	return;
}

DEF_DO_FUN( olist )
{
	OBJ_INDEX_DATA *obj;
	int vnum;
	AREA_DATA *tarea;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int lrange;
	int trange;

	tarea = (AREA_DATA*) ch->desc->olc_editing;
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (tarea)
	{
		if (arg1[0] == '\0') /* cleaned a big scary mess */
			lrange = tarea->lvnum; /* here.	    -Thoric */
		else
			lrange = atoi(arg1);
		if (arg2[0] == '\0')
			trange = tarea->uvnum;
		else
			trange = atoi(arg2);

		if ((lrange < tarea->lvnum || trange > tarea->uvnum)
				&& get_trust(ch) < LEVEL_GREATER)
		{
			send_to_char("That is out of your vnum range." NL, ch);
			return;
		}
	}
	else
	{
		lrange = ( is_number(arg1) ? atoi(arg1) : 1);
		trange = ( is_number(arg2) ? atoi(arg2) : 3);
	}

	for (vnum = lrange; vnum <= trange; vnum++)
	{
		if (!(obj = get_obj_index(vnum)))
			continue;
		pager_printf(ch, "%5d) (%s) %-20s (%s)" NL, vnum,
		IS_SET( obj->extra_flags, ITEM_PROTOTYPE ) ?
		FG_GREEN "P" PLAIN :
														" ", obj->name,
				obj->przypadki[0]);
	}
	return;
}

DEF_DO_FUN( mlist )
{
	MOB_INDEX_DATA *mob;
	int vnum;
	AREA_DATA *tarea;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int lrange;
	int trange;

	tarea = (AREA_DATA*) ch->desc->olc_editing;
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (tarea)
	{
		if (arg1[0] == '\0') /* cleaned a big scary mess */
			lrange = tarea->lvnum; /* here.	    -Thoric */
		else
			lrange = atoi(arg1);
		if (arg2[0] == '\0')
			trange = tarea->uvnum;
		else
			trange = atoi(arg2);

		if ((lrange < tarea->lvnum || trange > tarea->uvnum)
				&& get_trust(ch) < LEVEL_GREATER)
		{
			send_to_char("That is out of your vnum range." NL, ch);
			return;
		}
	}
	else
	{
		lrange = ( is_number(arg1) ? atoi(arg1) : 1);
		trange = ( is_number(arg2) ? atoi(arg2) : 1);
	}

	for (vnum = lrange; vnum <= trange; vnum++)
	{
		if (!(mob = get_mob_index(vnum)))
			continue;
		pager_printf(ch, "%5d) (%s) %-20s '%s'" NL, vnum,
		IS_SET( mob->act, ACT_PROTOTYPE ) ?
		FG_GREEN "P" PLAIN :
												" ", mob->player_name,
				mob->przypadki[0]);
	}
}

