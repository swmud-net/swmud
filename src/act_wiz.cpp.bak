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
 *			   Wizard/god command module      		   *
 ****************************************************************************/

#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h> /*Potrzebne do cset	--Thanos*/
#include "mud.h"
#include <SWTimeStamp.h>

#define RESTORE_INTERVAL 21600

const char *const save_flag[] =
{ "death", "kill", "passwd", "drop", "put", "give", "auto", "zap", "auction",
		"get", "receive", "idle", "backup", "r13", "r14", "r15", "r16", "r17",
		"r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27",
		"r28", "r29", "r30", "r31" };

/* from comm.c */
bool check_parse_name args( ( char *name ) );

/*
 * Local functions.
 */
ROOM_INDEX_DATA* find_location args( ( CHAR_DATA *ch, char *arg ) );
void close_area args( ( AREA_DATA *pArea ) );

int get_color(char *argument); /* function proto */

void save_all_ship_states args( ( ) );
void deal_with_crash args( ( ) );

/*
 * Global variables.
 */

time_t new_boot_time_t;
extern struct tm new_boot_struct;
extern OBJ_INDEX_DATA *obj_index_hash[MAX_KEY_HASH];
extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];

int get_saveflag(char *name)
{
	int x;

	for (x = 0; x < (int) (sizeof(save_flag) / sizeof(save_flag[0])); x++)
		if (!str_cmp(name, save_flag[x]))
			return x;
	return -1;
}

//Added by Trog (z rzezni Buczera)
//Poprawki by Thanos
DEF_DO_FUN( wizinfo )
{
	CHAR_DATA *victim;
	char buf[ MAX_STRING_LENGTH];
	char buf1[ MAX_STRING_LENGTH * 2];
	char arg[ MIL];
	CMDTYPE *cmd;
	int level, cnt, hash;

	one_argument(argument, arg);

	if ((arg[0] == '\0'))
		victim = ch;
	else if (!(victim = get_char_world(ch, arg)))
	{
		send_to_char("Nie ma nikogo takiego." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Mob nie ma wizkomend." NL, ch);
		return;
	}

	if (get_trust(victim) > get_trust(ch))
	{
		send_to_char("Nie masz wystarczaj±cych uprawnieñ." NL, ch);
		return;
	}

	sprintf(buf1, "Immskille ustawione dla gracza: %s" NL,
			capitalize(victim->name));

	for (level = LEVEL_HERO; level <= MAX_LEVEL; level++)
	{
		sprintf(buf, NL "%sLevel %3d:" PLAIN,
				level == get_trust(victim) ? FB_WHITE : PLAIN, level);
		strcat(buf1, buf);

		for (cnt = hash = 0; hash < MAX_CMD_HASH; hash++)
		{
			for (cmd = command_hash[hash]; cmd; cmd = cmd->next)
			{
				if (IS_SET(cmd->flags, CMD_OLC))
					continue;

				if (cmd->level == level)
				{

					if ( IS_SET(cmd->flags, CMD_HIDDEN)
							|| (IS_SET(cmd->flags, CMD_ADMIN_ONLY)
									&& !IS_ADMIN(ch->name)))
						continue;

					if ((get_trust(victim) >= cmd->level
							|| is_name(cmd->name, victim->pcdata->bestowments))
							&& !is_name(cmd->name,
									victim->pcdata->forbidden_cmd))
						sprintf(buf, " " FB_WHITE "%-10.10s" PLAIN "%s",
								all_capitalize(cmd->name),
								((++cnt) % 6) ? "" : NL "          ");
					else
						sprintf(buf, " " PLAIN "%-10.10s%s", cmd->name,
								((++cnt) % 6) ? "" : NL "          ");
					strcat(buf1, buf);
				}

			}
		}
		strcat(buf1, NL);
	}
	send_to_pager(buf1, ch);
	return;
}

/*
 * Check if the name prefix uniquely identifies a char descriptor
 */
CHAR_DATA* get_waiting_desc(CHAR_DATA *ch, char *name)
{
	DESCRIPTOR_DATA *d;
	CHAR_DATA *ret_char = 0;
	static unsigned int number_of_hits;

	number_of_hits = 0;
	for (d = first_descriptor; d; d = d->next)
	{
		if (d->character && (!str_prefix(name, d->character->name))
				&& IS_WAITING_FOR_AUTH(d->character))
		{
			if (++number_of_hits > 1)
			{
				ch_printf(ch, "%s does not uniquely identify a char." NL, name);
				return NULL;
			}
			ret_char = d->character; /* return current char on exit */
		}
	}
	if (number_of_hits == 1)
		return ret_char;
	else
	{
		send_to_char("No one like that waiting for authorization." NL, ch);
		return NULL;
	}
}

DEF_DO_FUN( authorize )
{
	char arg1[MIL];
	char arg2[MIL];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Usage:  authorize <player> <yes|name|no/deny>" NL, ch);
		send_to_char("Pending authorizations:" NL, ch);
		send_to_char(" Chosen Character Name" NL, ch);
		send_to_char("---------------------------------------------" NL, ch);
		for (d = first_descriptor; d; d = d->next)
			if ((victim = d->character) != NULL && IS_WAITING_FOR_AUTH(victim))
				ch_printf(ch, " %s!%s@%s new %s..." NL, victim->name,
						victim->desc->user, victim->desc->host,
						victim->race->name);
		return;
	}

	victim = get_waiting_desc(ch, arg1);
	if (victim == NULL)
		return;

	if (arg2[0] == '\0' || !str_cmp(arg2, "accept") || !str_cmp(arg2, "yes"))
	{
		victim->pcdata->auth_state = 3;
		REMOVE_BIT(victim->pcdata->flags, PCFLAG_UNAUTHED);
		STRDUP(victim->pcdata->authed_by, ch->name);
		sprintf(buf, "%s authorized %s", ch->name, victim->name);
		to_channel(buf, CHANNEL_MONITOR, "Monitor", ch->top_level);
		ch_printf(ch, "You have authorized %s." NL, victim->name);

		/* Below sends a message to player when name is accepted - Brittany   */

		ch_printf(victim,
				"" NL /* B */
				"                                  &W%s!&w                                " NL /* B */
				"                  &WAdministratorzy zaakceptowali twoje imiê.&w          " NL /* B */
				"               Teraz masz ju¿ pe³ne prawo do gry na SW-Mudzie.           " NL
				"                               &WGratulacje!&w                           " NL,
				victim->name); /* B */
		return;
	}
	else if (!str_cmp(arg2, "no") || !str_cmp(arg2, "deny"))
	{
		send_to_char("Twój dostêp do muda zosta³ zabroniony." NL, victim);
		sprintf(buf, "%s denied authorization to %s", ch->name, victim->name);
		to_channel(buf, CHANNEL_MONITOR, "Monitor", ch->top_level);
		ch_printf(ch, "You have denied %s." NL, victim->name);
		do_quit(victim, (char*) "");
	}

	else if (!str_cmp(arg2, "name") || !str_cmp(arg2, "n"))
	{
		sprintf(buf, "%s has denied %s's name", ch->name, victim->name);
		to_channel(buf, CHANNEL_MONITOR, "Monitor", ch->top_level);
		ch_printf(victim,
				"Administratorzy muda nie zaakceptuj± takiego imienia jak %s" NL
				"Wymy¶l inne, bardziej odpowiednie i poinformuj o tym Administratorów." NL,
				victim->name);
		ch_printf(ch, "You requested %s change names." NL, victim->name);
		victim->pcdata->auth_state = 2;
		return;
	}

	else
	{
		send_to_char("Invalid argument." NL, ch);
		return;
	}
}

DEF_DO_FUN( bamfin )
{
	if (!IS_NPC(ch))
	{
		smash_tilde(argument);
		STRDUP(ch->pcdata->bamfin, argument);
		send_to_char("Ok." NL, ch);
	}
	return;
}

DEF_DO_FUN( bamfout )
{
	if (!IS_NPC(ch))
	{
		smash_tilde(argument);
		STRDUP(ch->pcdata->bamfout, argument);
		send_to_char("Ok." NL, ch);
	}
	return;
}

DEF_DO_FUN( rank )
{
	if (IS_NPC(ch))
		return;

	if (!argument || argument[0] == '\0')
	{
		send_to_char("Usage: rank <string>." NL, ch);
		send_to_char("   or: rank none." NL, ch);
		return;
	}

	smash_tilde(argument);
	if (!str_cmp(argument, "none"))
		STRDUP(ch->pcdata->rank, "");
	else
		STRDUP(ch->pcdata->rank, argument);
	send_to_char("Ok." NL, ch);

	return;
}

DEF_DO_FUN( retire )
{
	char arg[MIL];
	CHAR_DATA *victim;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Retire whom?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	if (get_trust(victim) >= get_trust(ch) && !IS_ADMIN(ch->name))
	{
		send_to_char("You failed." NL, ch);
		return;
	}

	if (victim->top_level < LEVEL_SAVIOR)
	{
		send_to_char("The minimum level for retirement is savior." NL, ch);
		return;
	}

	if (IS_RETIRED(victim))
	{
		REMOVE_BIT(victim->pcdata->flags, PCFLAG_RETIRED);
		ch_printf(ch, "%s returns from retirement." NL, victim->name);
		ch_printf(victim, "%s brings you back from retirement." NL, ch->name);
	}
	else
	{
		SET_BIT(victim->pcdata->flags, PCFLAG_RETIRED);
		ch_printf(ch, "%s is now a retired immortal." NL, victim->name);
		ch_printf(victim, "Courtesy of %s, you are now a retired immortal." NL,
				ch->name);
	}
	return;
}

DEF_DO_FUN( deny )
{
	char arg[MIL];
	CHAR_DATA *victim;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Deny whom?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	if (get_trust(victim) >= get_trust(ch))
	{
		send_to_char("You failed." NL, ch);
		return;
	}

	SET_BIT(victim->act, PLR_DENY);
	send_to_char("Masz zabroniony dostêp do muda!" NL, victim);
	send_to_char("OK." NL, ch);
	do_quit(victim, (char*) "");

	return;
}

DEF_DO_FUN( disconnect )
{
	char arg[MIL];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Disconnect whom?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (victim->desc == NULL)
	{
		act( PLAIN, "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR);
		return;
	}

	if (get_trust(ch) <= get_trust(victim))
	{
		send_to_char("They might not like that..." NL, ch);
		return;
	}

	for (d = first_descriptor; d; d = d->next)
	{
		if (d == victim->desc)
		{
			close_socket(d, false);
			send_to_char("Ok." NL, ch);
			return;
		}
	}

	bug("*** desc not found ***");
	send_to_char("Descriptor not found!" NL, ch);
	return;
}

/*
 * Force a level one player to quit.             Gorog
 * Trog: Gorog jest albo debilem, albo ma kosc
 *       z nieskonczona iloscia RAMu.
 */
DEF_DO_FUN( fquit )
{
	DESCRIPTOR_DATA *d;
	bool found;

	if (argument[0] == '\0')
	{
		send_to_char("Syntax: fquit <name>" NL, ch);
		return;
	}

	found = false;
	for (d = first_descriptor; d; d = d->next)
		if (d->character && !str_cmp(d->character->name, argument)
				&& d->character->top_level == 1)
		{
			found = true;
			break;
		}

	if (!found)
	{
		send_to_char("There is no player like that (at level 1)." NL, ch);
		return;
	}

	send_to_char("Administratorzy muda ka¿± Ci wyj¶æ z gry." NL, d->character);
	do_quit(d->character, (char*) "");
	donemsg(ch);
	return;
}

DEF_DO_FUN( forceclose )
{
	DESCRIPTOR_DATA *d;

	if (argument[0] == '\0')
	{
		send_to_char("Usage: forceclose <descriptor#>" NL, ch);
		return;
	}

	for (d = first_descriptor; d; d = d->next)
	{
		if (d->descriptor == atoi(argument))
		{
			close_socket(d, false);
			donemsg(ch);
			return;
		}
	}

	send_to_char("Descriptor not found!" NL, ch);
	return;
}

DEF_DO_FUN( pardon )
{
	char arg1[MIL];
	char arg2[MIL];
	CHAR_DATA *victim;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Syntax: pardon <character> <planet>." NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	send_to_char(
			"Syntax: pardon <character> <planet>.... But it doesn't work .... Tell Durga to hurry up and finish it :p" NL,
			ch);
	return;
}

void echo_to_all(const char *argument, int tar)
{
	DESCRIPTOR_DATA *d;

	if (!argument || argument[0] == '\0')
		return;

	for (d = first_descriptor; d; d = d->next)
	{
		/* Added showing echoes to players who are editing, so they won't
		 miss out on important info like upcoming reboots. --Narn */
		if (d->connected >= CON_PLAYING)
		{
			/* This one is kinda useless except for switched.. */
			if (tar == ECHOTAR_PC && IS_NPC(d->character))
				continue;
			else if (tar == ECHOTAR_IMM && !IS_IMMORTAL(d->character))
				continue;
			send_to_char( FB_WHITE, d->character);
			send_to_char(argument, d->character);
			send_to_char( EOL, d->character);
		}
	}
	return;
}

DEF_DO_FUN( echo )
{
	char arg[MIL];
	int target;
	char *parg;

	if (argument[0] == '\0')
	{
		send_to_char("Echo what?" NL, ch);
		return;
	}

	parg = argument;
	argument = one_argument(argument, arg);
	if (!str_cmp(arg, "PC") || !str_cmp(arg, "player"))
		target = ECHOTAR_PC;
	else if (!str_cmp(arg, "imm"))
		target = ECHOTAR_IMM;
	else
	{
		target = ECHOTAR_ALL;
		argument = parg;
	}

	one_argument(argument, arg);
	if (!str_cmp(arg, "Merth") || !str_cmp(arg, "Durga"))
	{
		ch_printf(ch, "I don't think %s would like that!" NL, arg);
		return;
	}
	echo_to_all(argument, target);
}

void do_echoat(CHAR_DATA *ch, char *argument) //byTrog
{
	CHAR_DATA *victim;
	char arg[MIL];

	argument = one_argument(argument, arg);

	if (*argument == '\0')
	{
		send_to_char("Echoat to whom and what?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("You can't find anyone like that." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPCs!" NL, ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("To yourself?" NL, ch);
		return;
	}

	send_to_char(argument, ch);
	send_to_char( EOL, ch);
	send_to_char(argument, victim);
	send_to_char( EOL, victim);
	return;
}

void echo_to_room(ROOM_INDEX_DATA *room, const char *argument)
{
	CHAR_DATA *vic;

	if (room == NULL)
		return;

	for (vic = room->first_person; vic; vic = vic->next_in_room)
	{
		send_to_char( FB_WHITE, vic);
		send_to_char(argument, vic);
		send_to_char( EOL, vic);
	}
}

/* Thanos */
void echo_to_area(AREA_DATA *area, char *argument)
{
	CHAR_DATA *pch;

	if (area == NULL)
		return;

	for (pch = first_char; pch; pch = pch->next)
	{
		if (!IS_NPC(pch) && IS_AWAKE(pch) && pch->in_room
				&& pch->in_room->area == area)
		{
			send_to_char( FB_WHITE, pch);
			send_to_char(argument, pch);
			send_to_char( EOL, pch);
		}
	}
}

DEF_DO_FUN( recho )
{
	char arg[MIL];
	int color;

	if (argument[0] == '\0')
	{
		send_to_char("Recho what?" NL, ch);
		return;
	}

	one_argument(argument, arg);
	if (!str_cmp(arg, "Thoric") || !str_cmp(arg, "Dominus")
			|| !str_cmp(arg, "Circe") || !str_cmp(arg, "Haus")
			|| !str_cmp(arg, "Narn") || !str_cmp(arg, "Scryn")
			|| !str_cmp(arg, "Blodkai") || !str_cmp(arg, "Damian"))
	{
		ch_printf(ch, "I don't think %s would like that!" NL, arg);
		return;
	}
	if ((color = get_color(argument)))
	{
		argument = one_argument(argument, arg);
		echo_to_room(ch->in_room, (const char*) argument);
	}
	else
		echo_to_room(ch->in_room, (const char*) argument);
}

ROOM_INDEX_DATA* find_location(CHAR_DATA *ch, char *arg)
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;

	if ( is_number(arg))
		return get_room_index(atoi(arg));

	if ((victim = get_char_world(ch, arg)) != NULL)
		return victim->in_room;

	if ((obj = get_obj_world(ch, arg)) != NULL)
		return obj->in_room;

	return NULL;
}

DEF_DO_FUN( transfer )
{
	char arg1[MIL];
	char arg2[MIL];
	ROOM_INDEX_DATA *location;
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Transfer whom (and where)?" NL, ch);
		return;
	}

	if (!str_cmp(arg1, "all"))
	{
		for (d = first_descriptor; d; d = d->next)
		{
			if (d->connected >= CON_PLAYING && d->character != ch
					&& d->character->in_room && d->newstate != 2
					&& can_see(ch, d->character))
			{
				char buf[MAX_STRING_LENGTH];
				sprintf(buf, "%s %s", d->character->name, arg2);
				do_transfer(ch, buf);
			}
		}
		return;
	}

	/*
	 * Thanks to Grodyn for the optional location parameter.
	 */
	if (arg2[0] == '\0')
	{
		location = ch->in_room;
	}
	else
	{
		if ((location = find_location(ch, arg2)) == NULL)
		{
			send_to_char("No such location." NL, ch);
			return;
		}

		if (room_is_private(ch, location))
		{
			if (get_trust(ch) < LEVEL_GREATER)
			{
				send_to_char("That room is private right now." NL, ch);
				return;
			}
			else
			{
				send_to_char("Overriding private flag!" NL, ch);
			}
		}
	}

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (NOT_AUTHED(victim))
		send_to_char("Warning: They are not authorized yet!" NL, ch);

	if (!victim->in_room)
	{
		send_to_char("They are in limbo." NL, ch);
		return;
	}

	uncrew(ship_from_room(victim->in_room), victim);
	if (victim->fighting)
		stop_fighting(victim, true);
	act( COL_FORCE, "$n znika rozpadaj±c siê na maleñkie cz±steczki.", victim,
			NULL, NULL, TO_ROOM);
	victim->retran = victim->in_room->vnum;
	char_from_room(victim);
	char_to_room(victim, location);
	act( COL_FORCE, "$n materializuje siê tutaj.", victim, NULL, NULL, TO_ROOM);
	if (ch != victim)
		act( COL_IMMORT, "$n przenosi ciê.", ch, NULL, victim, TO_VICT);
	do_look(victim, (char*) "auto");
	send_to_char("Ok." NL, ch);
	if (!IS_IMMORTAL(victim) && !IS_NPC(victim)
			&& !in_level_range(victim, location->area))
		send_to_char(
				"Warning: the player's level is not within the area's level range." NL,
				ch);
}

DEF_DO_FUN( retran )
{
	char arg[MIL];
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Retransfer whom?" NL, ch);
		return;
	}
	if (!(victim = get_char_world(ch, arg)))
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}
	sprintf(buf, "'%s' %d", victim->name, victim->retran);
	do_transfer(ch, buf);
	return;
}

DEF_DO_FUN( regoto )
{
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, "%d", ch->regoto);
	do_goto(ch, buf);

	return;
}

DEF_DO_FUN( at )
{
	char arg[MIL];
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *original;
	CHAR_DATA *wch, *ch_next;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char("At where what?" NL, ch);
		return;
	}

	if ((location = find_location(ch, arg)) == NULL)
	{
		send_to_char("No such location." NL, ch);
		return;
	}

	if (room_is_private(ch, location))
	{
		if (get_trust(ch) < LEVEL_GREATER)
		{
			send_to_char("That room is private right now." NL, ch);
			return;
		}
		else
		{
			send_to_char("Overriding private flag!" NL, ch);
		}

	}

	original = ch->in_room;
	ch_next = ch->next_in_room;
	char_from_room(ch);
	char_to_room(ch, location);
	interpret(ch, argument);

	/*
	 * See if 'ch' still exists before continuing!
	 * Handles 'at XXXX quit' case.
	 */
	for (wch = first_char; wch; wch = wch->next)
	{
		if (wch == ch)
		{
			char_from_room(ch);
			char_to_room(ch, original);

			/* BUGFIX by Thanos:
			 * po wykonaniu komendy progowej mob czêsto u¿ywa mpat, a co
			 * za tym idzie robi co¶ w innej lokacji, czyli wypina siê z listy
			 * lokacji aktualnej. Po powrocie zostaje wpiêty na sam jej koniec,
			 * a co za tym idzie, pêtla lec±ca po wszystkich w pomieszczeniu
			 * znów trafia na niego. Taki manewr grozi zapêtlaniem i duplikowaniem
			 * siê wykonañ progów. Mój sposób: wepnijmy mobka tam gdzie by³.
			 * O ile ofcoz sa jeszcze ci, obok ktorych stal.
			 */
			if (ch_next)
			{
				UNLINK(ch, original->first_person, original->last_person,
						next_in_room, prev_in_room);
				INSERT(ch, ch_next, original->first_person, next_in_room,
						prev_in_room);
			}
			break;
		}
	}

	return;
}

/* added by Thanos (zmiany te same co przy do_for(), szukaj tam komentarza*/
DEF_DO_FUN( rat )
{
	char arg1[MIL];
	char arg2[MIL];
	int Start, End;

	if (IS_NPC(ch)) /*bez zbêdnego ryzyka*/
	{
		huh(ch);
		return;
	}

	/* zatrzymujemy pêtlê */
	if (argument[0] && !str_cmp(argument, "stop"))
	{
		if (rat_loop.rStopped)
			send_to_char("Rat Loop is not currently running!" NL, ch);
		else
		{
			ch_printf(rat_loop.rOwner,
					"Rat Loop Stopped by %s. Made %d iteration%s." NL,
					ch->przypadki[0], rat_loop.rCount,
					rat_loop.rCount == 1 ? "" : "s");

			if (ch != rat_loop.rOwner)
				send_to_char("Ok." NL, ch);

			clean_rat_loop();
		}
		return;
	}

	/* je¶li kto¶ zainteresowany chce sprawdziæ co tak zapierdala :) */
	if (argument[0] && !str_cmp(argument, "status"))
	{
		if (rat_loop.rStopped)
			send_to_char("Rat Loop is not currently running!" NL, ch);
		else
		{
			ch_printf(ch, FB_WHITE "Rat Status:" EOL);
			ch_printf(ch, "Owner:     %s" NL, rat_loop.rOwner->name);
			ch_printf(ch, "Command:   %s" NL, rat_loop.command);
			ch_printf(ch, "Iteration: %d" NL, rat_loop.rCount);
			ch_printf(ch, "Range:     %d-%d" NL, rat_loop.Start, rat_loop.End);
			ch_printf(ch, "Next number: %d" NL, rat_loop.rRoom);
		}
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0')
	{
		send_to_char("Syntax: rat <start> <end> <command>" NL, ch);
		return;
	}

	if (!rat_loop.rStopped)
	{
		send_to_char(
				"Sorry, Rat Loop is currently running. Stop it if you want." NL,
				ch);
		send_to_char(
				"Use 'rat stop' command, but check who is using it first.('rat status')." NL,
				ch);
		return;
	}

	Start = atoi(arg1);
	End = atoi(arg2);

	if (Start < 1|| End < Start || Start > End
	|| Start == End || End > MAX_VNUM)
	{
		send_to_char("Invalid range." NL, ch);
		return;
	}

	if (!str_cmp(argument, "quit") || is_name(argument, "for")
			|| is_name(argument, "rat"))
	{
		send_to_char("I don't think so!" NL, ch);
		return;
	}

	clean_rat_loop();
	rat_loop.rOwner = ch;
	rat_loop.Start = Start;
	rat_loop.End = End;
	STRDUP(rat_loop.command, argument);
	rat_loop.rRoom = Start;
	rat_loop.rStopped = false;
	send_to_char("Ok. Rat Loop Started." NL, ch);
	return;
}

DEF_DO_FUN( rstat )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MIL];
	ROOM_INDEX_DATA *location;
	OBJ_DATA *obj;
	CHAR_DATA *rch;
	EXIT_DATA *pexit;
	int cnt;
	static const char *dir_text[] =
	{ "n", "e", "s", "w", "u", "d", "ne", "nw", "se", "sw", "?" };

	one_argument(argument, arg);

	if (get_trust(ch) < LEVEL_IMMORTAL)
	{
		if (!can_edit(ch, ch->in_room->vnum))
		{
			send_to_char(
					"Mo¿esz u¿ywaæ rstat tylko w przydzielonej ci krainie." NL,
					ch);
			return;
		}
	}

	if (!str_cmp(arg, "exits"))
	{
		location = ch->in_room;

		ch_printf(ch, "&cExits for room '&w%s&c.' vnum&w %d" NL, location->name,
				location->vnum);

		for (cnt = 0, pexit = location->first_exit; pexit; pexit = pexit->next)
			ch_printf(ch,
					"%2d&c)&w %2s&c to&w %-5d&c Key:&w %d&c Flags:&w %d&c OrigFlags:&w %d&c  Keywords: '&w%s&c'.&w\n\r&cDescription:&w %s&cExit links back to vnum:&w %d&c  Exit's RoomVnum:&w %d&c  Distance:&w %d" NL,
					++cnt, dir_text[pexit->vdir],
					pexit->to_room ? VNUM(pexit->to_room) : 0, pexit->key,
					pexit->flags, pexit->orig_flags, pexit->keyword,
					pexit->description[0] != '\0' ?
							pexit->description : "(none)&c.&w" NL,
					pexit->rexit ? pexit->rexit->vnum : 0, pexit->rvnum,
					pexit->distance);
		return;
	}
	location = (arg[0] == '\0') ? ch->in_room : find_location(ch, arg);
	if (!location)
	{
		send_to_char("Nie ma takiego pomieszczenia." NL, ch);
		return;
	}

	if (ch->in_room != location && room_is_private(ch, location))
	{
		if (get_trust(ch) < LEVEL_GREATER)
		{
			send_to_char("Ta lokacja jest obecnie prywatna." NL, ch);
			return;
		}
		else
		{
			send_to_char("Omijam flagê 'private'!" NL, ch);
		}

	}

	ch_printf(ch,
			FG_CYAN "Name: " PLAIN "%s" FG_CYAN "." EOL
			FG_CYAN "Area: " PLAIN "%s" FG_CYAN "  Filename: " PLAIN "%s" FG_CYAN "." EOL,
			location->name, location->area ? location->area->name : "None????",
			location->area ? location->area->filename : "None????");

	ch_printf(ch,
			FG_CYAN "Vnum: " PLAIN "%d" FG_CYAN ".  Sector: " PLAIN "[%s]"
			FG_CYAN ".  Light: " PLAIN "%d" FG_CYAN ".  TeleDelay: " PLAIN "%d"
			FG_CYAN ".  TeleVnum: " PLAIN "%d" FG_CYAN "  Tunnel: " PLAIN "%d" FG_CYAN "." EOL,
			location->vnum, bit_name(sector_types_list, location->sector_type),
			location->light, location->tele_delay, location->tele_vnum,
			location->tunnel);

	ch_printf(ch,
	FG_CYAN "Room flags: " PLAIN "[%s]" EOL,
			flag_string(room_flags_list, location->room_flags));
	ch_printf(ch,
	FG_CYAN "Description:" EOL "%s" PLAIN, location->description);
	ch_printf(ch,
	FG_CYAN "Night description:"); //by Trog
	if (location->nightdesc && location->nightdesc[0] != '\0')
		ch_printf(ch, EOL "%s" PLAIN, location->nightdesc);
	else
		ch_printf(ch, PLAIN " (none)" NL);

	if (location->first_extradesc)
	{
		EXTRA_DESCR_DATA *ed;

		send_to_char( FG_CYAN "Extra description keywords: '" PLAIN, ch);
		for (ed = location->first_extradesc; ed; ed = ed->next)
		{
			send_to_char(ed->keyword, ch);
			if (ed->next)
				send_to_char(" ", ch);
		}
		send_to_char( FG_CYAN "'." EOL, ch);
	}

	send_to_char( FG_CYAN "Characters:" PLAIN, ch);
	for (rch = location->first_person; rch; rch = rch->next_in_room)
	{
		if (can_see(ch, rch))
		{
			send_to_char(" ", ch);
			one_argument(rch->name, buf);
			send_to_char(buf, ch);
		}
	}

	send_to_char( EOL FG_CYAN "Objs:" PLAIN, ch);
	for (obj = location->first_content; obj; obj = obj->next_content)
	{
		send_to_char(" ", ch);
		one_argument(obj->name, buf);
		send_to_char(buf, ch);
	}
	send_to_char( FG_CYAN "." EOL, ch);

	if (location->first_exit)
		send_to_char(
		FG_CYAN "------------------- EXITS -------------------" EOL, ch);
	for (cnt = 0, pexit = location->first_exit; pexit; pexit = pexit->next)
	{
		ch_printf(ch, "%2d"
		FG_CYAN ")" PLAIN " %-2s" FG_CYAN " to" PLAIN " %-5d"
		FG_CYAN ".  Key:" PLAIN " %d" FG_CYAN "  Keywords:" PLAIN " %10s" EOL
		FG_CYAN "      Flags:" PLAIN " %s", ++cnt, dir_text[pexit->vdir],
				pexit->to_room ? VNUM(pexit->to_room) : 0, pexit->key,
				pexit->keyword[0] != '\0' ? pexit->keyword : "(none)",
				flag_string(exit_flags_list, pexit->flags));
		ch_printf(ch,
		FG_CYAN "  OrigFlags:" PLAIN " %s" EOL,
				flag_string(exit_flags_list, pexit->orig_flags));
	}

	return;
}

DEF_DO_FUN( ostat )
{
	char arg[MIL];
	REQUIREMENT_DATA *req;
	AFFECT_DATA *paf;
	OBJ_DATA *obj;
	char *pdesc;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Ostat na co?" NL, ch);
		return;
	}
	if (arg[0] != '\'' && arg[0] != '"' && strlen(argument) > strlen(arg))
		strcpy(arg, argument);

	if ((obj = get_obj_world(ch, arg)) == NULL)
	{
		send_to_char("Nie ma nic takiego w piekle na ziemi ani na niebie." NL,
				ch);
		return;
	}

	ch_printf(ch, "&cName:&w %s. &cGender:&w %d &c(&w%s&c)&w" NL, obj->name,
			obj->gender, bit_name(gender_types_list, obj->gender));

	pdesc = get_extra_descr(arg, obj->first_extradesc);
	if (!pdesc)
		pdesc = get_extra_descr(arg, obj->pIndexData->first_extradesc);
	if (!pdesc)
		pdesc = get_extra_descr(obj->name, obj->first_extradesc);
	if (!pdesc)
		pdesc = get_extra_descr(obj->name, obj->pIndexData->first_extradesc);
	if (pdesc)
		send_to_char(pdesc, ch);

	ch_printf(ch,
			"&cVnum:&w %d&c.  Type:&w %s&c.  Count:&w %d &c Gcount:&w %d" NL,
			obj->pIndexData->vnum, bit_name(obj_types_list, obj->item_type),
			obj->pIndexData->count, obj->count);

	ch_printf(ch,
			"&cSerial#:&w %d&c  TopIdxSerial#:&w %d &c TopSerial#:&w %d" NL,
			obj->serial, obj->pIndexData->serial, cur_obj_serial);

	ch_printf(ch,
			"&cPrzypadki: 0.&w%-15s &c1.&w%-15s &c2.&w%-15s\n\r           &c3.&w%-15s &c4.&w%-15s &c5.&w%-15s" NL,
			obj->przypadki[0], obj->przypadki[1], obj->przypadki[2],
			obj->przypadki[3], obj->przypadki[4], obj->przypadki[5]);

	ch_printf(ch, "&cLong description:&w %s" NL, obj->description);

	if (obj->action_desc[0] != '\0')
		ch_printf(ch, "&cAction description:&c %s&c.&w" NL, obj->action_desc);

	ch_printf(ch, "&cWear flags :&w %s" NL,
			flag_string(wear_flags_list, obj->wear_flags));
	ch_printf(ch, "&cExtra flags:&w %s" NL,
			flag_string(obj_flags_list, obj->extra_flags));

	ch_printf(ch,
			"&cNumber:&w %d&c/&w%d&c.  Weight:&w %d&c/&w%d&c.  Layers:&w %d", 1,
			get_obj_number(obj), obj->weight, get_obj_weight(obj),
			obj->pIndexData->layers);

	/* Thanos -- personale */
	if (IS_OBJ_STAT(obj, ITEM_PERSONAL))
		ch_printf(ch, FG_CYAN " Owner:" PLAIN " %s" EOL,
				*obj->owner_name ? obj->owner_name : "none");
	else
		ch_printf(ch, EOL);

	ch_printf(ch,
			"&cCost:&w %d&c.  Rent:&w %d&c.  Timer:&w %d&c.  Level:&w %d&c.&w" NL,
			obj->cost, obj->pIndexData->rent, obj->timer, obj->level);

	ch_printf(ch,
			"&cIn room:&w %d&c.  In object:&w %s&c.  Carried by:&w %s &c{#&w%d&c}.  Wear_loc:&w %d&c.&w" NL,
			obj->in_room == NULL ? 0 : obj->in_room->vnum,
			obj->in_obj == NULL ? "(none)" : obj->in_obj->przypadki[0],
			obj->carried_by == NULL ? "(none)" : obj->carried_by->name,
			(obj->carried_by != NULL && IS_NPC(obj->carried_by)) ?
					obj->carried_by->pIndexData->vnum : 0, obj->wear_loc);

	show_obj_values(ch, obj->pIndexData);
	ch_printf(ch, "&cObject Values:&w %d %d %d %d %d %d&c.&w" NL, obj->value[0],
			obj->value[1], obj->value[2], obj->value[3], obj->value[4],
			obj->value[5]);

	if (obj->pIndexData->first_extradesc)
	{
		EXTRA_DESCR_DATA *ed;

		send_to_char("&cPrimary description keywords:" NL, ch);
		for (ed = obj->pIndexData->first_extradesc; ed; ed = ed->next)
		{
			send_to_char("&W'&w", ch);
			send_to_char(ed->keyword, ch);
			send_to_char("&W'&w" NL "\"", ch);
			send_to_char(ed->description, ch);
			send_to_char("&w\"" NL NL, ch);
		}
	}
	if (obj->first_extradesc)
	{
		EXTRA_DESCR_DATA *ed;

		send_to_char("&cSecondary description keywords: '&w", ch);
		for (ed = obj->first_extradesc; ed; ed = ed->next)
		{
			send_to_char(ed->keyword, ch);
			if (ed->next)
				send_to_char(" ", ch);
		}
		send_to_char("&c'.&w" NL, ch);
	}

	for (paf = obj->first_affect; paf; paf = paf->next)
		ch_printf(ch, FG_CYAN
		"Affects" PLAIN " %s" FG_CYAN " by" PLAIN " %s" FG_CYAN ". (extra)" EOL,
				bit_name(apply_types_list, paf->location),
				STRING_AFFECT(paf->location, paf->modifier));

	for (paf = obj->pIndexData->first_affect; paf; paf = paf->next)
		ch_printf(ch, FG_CYAN
		"Affects" PLAIN " %s" FG_CYAN " by" PLAIN " %s" FG_CYAN "." EOL,
				bit_name(apply_types_list, paf->location),
				STRING_AFFECT(paf->location, paf->modifier));

	for (req = obj->first_requirement; req; req = req->next)
	{
		send_to_char( FG_CYAN "Requires: ", ch);
		show_req(ch, req);
		send_to_char("(extra)" EOL, ch);
	}

	for (req = obj->pIndexData->first_requirement; req; req = req->next)
	{
		send_to_char( FG_CYAN "Requires: ", ch);
		show_req(ch, req);
		send_to_char( EOL, ch);
	}
	return;
}

DEF_DO_FUN( mstat )
{
	char arg[MIL];
	char *n;
	AFFECT_DATA *paf;
	CHAR_DATA *victim;
	SKILLTYPE *skill;
	CRIME_DATA *crime;
	NAMESLIST_DATA *nameslist;
	KNOWN_LANG *klang;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Mstat na kogo?" NL, ch);
		return;
	}

	if (arg[0] != '\'' && arg[0] != '"' && strlen(argument) > strlen(arg))
		strcpy(arg, argument);

	/* Thanos --> mstat na supermoba 4 adminz only */
	if (!str_cmp(arg, "supermob") && IS_ADMIN(ch->name))
	{
		victim = supermob;
	}
	else if ((victim = get_char_world(ch, arg)) == NULL)
	{
		ch_printf(ch, "Nie ma tu nikogo takiego jak %s." NL, arg);
		return;
	}

	if ((get_trust(ch) < get_trust(victim) && !IS_NPC(victim))
			|| ( IS_ADMIN( victim->name ) && !IS_ADMIN(ch->name))) //added by Thanos :)
	{
		ch_printf(ch, FB_WHITE
		"%s blask o¶lepia ciê nie pozwalaj±c przyjrzeæ siê bli¿ej." EOL,
		FEMALE( victim ) ? "Jej" : "Jego");
		return;
	}

	ch_printf(ch, "&cName:&w %s     &cOrganization:&w %s" NL, victim->name,
			( IS_NPC( victim ) || !victim->pcdata->clan) ?
					"(none)" : victim->pcdata->clan->name);

	if (get_trust(ch) >= LEVEL_GOD && !IS_NPC(victim) && victim->desc)
		ch_printf(ch,
				"&cUser:&w %s@%s   &cDescriptor:&w %d   &cTrust:&w %d   &cAuthedBy:&w %s" NL,
				victim->desc->user, victim->desc->host,
				victim->desc->descriptor, victim->trust,
				victim->pcdata->authed_by[0] != '\0' ?
						victim->pcdata->authed_by : "(unknown)");

	if (!IS_NPC(victim) && victim->pcdata->release_date != 0)
		ch_printf(ch, "&cHelled until&w %24.24s&c by&w %s&c.&w" NL,
				ctime(&victim->pcdata->release_date),
				victim->pcdata->isolated_by);

	if (!IS_NPC(victim))
	{
		ch_printf(ch,
				FG_CYAN
				"Quest points: " PLAIN "%d" FG_CYAN ".  Quests made: " PLAIN "%s" FG_CYAN "." EOL,
				victim->pcdata->quest_points, victim->pcdata->quest_done);
	}

	ch_printf(ch,
			"&cVnum:&w %d &c  Sex:&w %s  &c Room: &w%d &c  Count:&w %d &c Killed:&w %d" NL,
			IS_NPC(victim) ? victim->pIndexData->vnum : 0,
			MALE( victim ) ? "male" : FEMALE(victim) ? "female" : "neutral",
			victim->in_room == NULL ? 0 : victim->in_room->vnum,
			IS_NPC(victim) ? victim->pIndexData->count : 1,
			IS_NPC(victim) ?
					victim->pIndexData->killed :
					victim->pcdata->mdeaths + victim->pcdata->pdeaths);
	ch_printf(ch,
			"&cStr:&w %d  &cInt:&w %d  &cWis:&w %d  &cDex:&w %d  &cCon:&w %d  &cCha:&w %d  &cLck:&w %d  &cFrc:&w %d" NL,
			get_curr_str(victim), get_curr_int(victim), get_curr_wis(victim),
			get_curr_dex(victim), get_curr_con(victim), get_curr_cha(victim),
			get_curr_lck(victim), get_curr_frc(victim));
	ch_printf(ch,
			"&cHps:&w %d&c/&w%d  &cForce:&w %d&c/&w%d   &cMove:&w %d&c/&w%d" NL,
			victim->hit, victim->max_hit, victim->mana, victim->max_mana,
			victim->move, victim->max_move);
	if (!IS_NPC(victim))
	{
		int ability;

		for (ability = 0; ability < MAX_ABILITY; ability++)
			ch_printf(ch,
					"&c%-15s   Level:&w %-3d&c/&w%-3d&c   Exp:&w %-10ld   &cNext:&w %-10ld" NL,
					class_table[ability].name, victim->skill_level[ability],
					max_level(victim, ability), victim->experience[ability],
					exp_level(victim->skill_level[ability] + 1));
	}
	ch_printf(ch,
			"&cTop Level:&w %d     &cRace:&w %s &cAlign:&w %d  &cAC:&w %d  &cGold:&w %d" NL,
			victim->top_level, victim->race->name, victim->alignment,
			GET_AC(victim), victim->gold);
	ch_printf(ch,
			FG_CYAN "Security:" PLAIN " %d" FG_CYAN " Hitroll:&w %d&c   Damroll:&w %d&c   Position:&w %d  &c Wimpy:&w %d " NL,
			victim->pcdata ? victim->pcdata->security : 0, GET_HITROLL(victim),
			GET_DAMROLL(victim), victim->position, victim->wimpy);
	ch_printf(ch, "&cFighting:&w %s &c   Master:&w %s &c   Leader:&w %s" NL,
			victim->fighting ? victim->fighting->who->name : "(none)",
			victim->master ? victim->master->name : "(none)",
			victim->leader ? victim->leader->name : "(none)");
	if (!IS_NPC(victim))
		ch_printf(ch, "&cThirst:&w %d&c   Full: &w%d&c   Drunk:&w %d &c" NL,
				victim->pcdata->condition[COND_THIRST],
				victim->pcdata->condition[COND_FULL],
				victim->pcdata->condition[COND_DRUNK]);
	else
		ch_printf(ch,
				FG_CYAN "Hit dice: " PLAIN "%d" FG_CYAN "d" PLAIN "%d" FG_CYAN "+" PLAIN "%d" FG_CYAN "  Damage dice: " PLAIN "%d" FG_CYAN "d" PLAIN "%d" FG_CYAN "+" PLAIN "%d  ",
				victim->pIndexData->hitnodice, victim->pIndexData->hitsizedice,
				victim->pIndexData->hitplus, victim->pIndexData->damnodice,
				victim->pIndexData->damsizedice, victim->pIndexData->damplus);
	ch_printf(ch, "&cMentalState: &w%d &c  EmotionalState:&w %d" NL,
			victim->mental_state, victim->emotional_state);
	ch_printf(ch, "&cSaving throws:&w %d %d %d %d %d&c.&w ",
			victim->saving_poison_death, victim->saving_wand,
			victim->saving_para_petri, victim->saving_breath,
			victim->saving_spell_staff);
	if (!IS_NPC(victim))
	{
		n = victim->name;
		if (!check_clone(victim))
			ch_printf(ch, "%s nie posiada klona.", n);
		else
		{
			ch_printf(ch, "&c%s posiada klon.&w", n);
		}
	}

	ch_printf(ch,
			"\n\r&cCarry figures: items(%d/&w%d&c) weight(%d/&w%d&c) Numattacks:&w %d" NL,
			victim->carry_number, can_carry_n(victim), victim->carry_weight,
			can_carry_w(victim), victim->numattacks);
	ch_printf(ch,
			"&cYears:&w %d &c  Seconds Played: &w%d &c  Timer:&w %d  &c Act: &w%d" NL,
			get_age(victim), (int) victim->played, victim->timer, victim->act);
	if (IS_NPC(victim))
	{
		ch_printf(ch, "&cAct flags:&w %s" NL,
				flag_string(act_flags_list, victim->act));
		ch_printf(ch, "&cVip-flags:&w %s" NL,
				(victim->s_vip_flags && victim->s_vip_flags[0] != '\0') ?
						victim->s_vip_flags : "(none)");
		//done
	}
	else
	{
		ch_printf(ch, "&cPlayer flags:&w %s" NL,
				flag_string(plr_flags_list, victim->act));
		ch_printf(ch, "&cPcflags:&w %s" NL,
				flag_string(pc_flags_list, victim->pcdata->flags));

		if (victim->first_crime)
		{
			for (crime = victim->first_crime; crime; crime = crime->next)
				ch_printf(ch,
						"&cCrime data:&w Planet: %s  Lv: %d  for: %s (%d)" NL,
						crime->planet, crime->level,
						flag_string(crime_flags_list, crime->type),
						crime->type);
		}
		//done
	}

	ch_printf(ch, "&cAffected by:&w %s" NL,
			flag_string(aff_flags_list, victim->affected_by));
	ch_printf(ch, "&cSpeaking:&w %s" NL, victim->speaking->name);
	send_to_char("&cLanguages:&w", ch);
	if ( IS_NPC( victim ) && !victim->first_klang)
	{
		LANG_DATA *lang;

		FOREACH( lang, first_lang )
		{
			send_to_char(" ", ch);
			send_to_char(lang->name, ch);
			send_to_char( PLAIN, ch);
		}
	}
	else
		FOREACH( klang, victim->first_klang )
		{
			if (victim->speaking == klang->language)
				send_to_char( FB_RED, ch);
			send_to_char(" ", ch);
			send_to_char(klang->language->name, ch);
			send_to_char( PLAIN, ch);
		}
	send_to_char( NL, ch);
	if (victim->pcdata && victim->pcdata->bestowments
			&& victim->pcdata->bestowments[0] != '\0')
		ch_printf(ch, "&cBestowments:&w %s" NL, victim->pcdata->bestowments);

	ch_printf(ch,
			"&cPrzypadki: 0.&w%-15s &c1.&w%-15s &c2.&w%-15s\n\r           &c3.&w%-15s &c4.&w%-15s &c5.&w%-15s" NL,
			victim->przypadki[0], victim->przypadki[1], victim->przypadki[2],
			victim->przypadki[3], victim->przypadki[4], victim->przypadki[5]);

	ch_printf(ch, "&cLong  description:&w %s",
			victim->long_descr[0] != '\0' ? victim->long_descr : "(none)" NL);

	if ( IS_NPC(victim) && (victim->spec_fun || victim->spec_2))
		ch_printf(ch, "&cMobile has spec fun:&w %s %s" NL,
				lookup_spec(victim->spec_fun),
				victim->spec_2 ? lookup_spec(victim->spec_2) : "");
	ch_printf(ch, "&cBody Parts :&w %s" NL,
			flag_string(part_flags_list, victim->xflags));
	ch_printf(ch, "&cResistant  :&w %s" NL,
			flag_string(ris_flags_list, victim->resistant));
	ch_printf(ch, "&cImmune     :&w %s" NL,
			flag_string(ris_flags_list, victim->immune));
	ch_printf(ch, "&cSusceptible:&w %s" NL,
			flag_string(ris_flags_list, victim->susceptible));
	ch_printf(ch, "&cAttacks    :&w %s" NL,
			flag_string(attack_flags_list, victim->attacks));
	ch_printf(ch, "&cDefenses   :&w %s" NL,
			flag_string(def_flags_list, victim->defenses));
	if (IS_NPC(victim))
	{
		ch_printf(ch, FG_CYAN "Nameslist (" PLAIN "%d" FG_CYAN "):" PLAIN,
				victim->pIndexData->namescount);
		for (nameslist = victim->pIndexData->first_nameslist; nameslist;
				nameslist = nameslist->next)
			ch_printf(ch, " %s", nameslist->name);
		ch_printf(ch, FG_CYAN ".\n" PLAIN);
	}
	for (paf = victim->first_affect; paf; paf = paf->next)
		if ((skill = get_skilltype(paf->type)) != NULL)
			ch_printf(ch,
					"%s: '%s' modifies %s by %d for %d rounds with bits %s." NL,
					skill_tname[skill->type], skill->name,
					bit_name(apply_types_list, paf->location), paf->modifier,
					paf->duration, flag_string(aff_flags_list, paf->bitvector));

	if (get_trust(ch) > 103 && !IS_NPC(victim) && victim->pcdata->first_fevent)
	{
		FEVENT_DATA *fevent;

		ch_printf(ch, FG_CYAN "Force Events (" PLAIN "%d" FG_CYAN "):" EOL,
				victim->pcdata->fevents);
		FOREACH( fevent, victim->pcdata->first_fevent )
			ch_printf(ch, "%s" FG_CYAN ": (" PLAIN "%s" FG_CYAN ") (" PLAIN
			"%lld %lld %lld %lld %lld" FG_CYAN ")" EOL,
					bit_name(fevent_types_list, fevent->trigger), fevent->sattr,
					fevent->attr[0], fevent->attr[1], fevent->attr[2],
					fevent->attr[3], fevent->attr[4]);
	}
	return;
}

DEF_DO_FUN( mfind )
{
	char arg[MIL];
	MOB_INDEX_DATA *pMobIndex;
	int hash;
	int nMatch;
	bool fAll;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Mfind whom?" NL, ch);
		return;
	}

	fAll = !str_cmp(arg, "all");
	nMatch = 0;

	/*
	 * Yeah, so iterating over all vnum's takes 10,000 loops.
	 * Get_mob_index is fast, and I don't feel like threading another link.
	 * Do you?
	 * -- Furey
	 */
	/*  for ( vnum = 0; nMatch < top_mob_index; vnum++ )
	 {
	 if ( ( pMobIndex = get_mob_index( vnum ) ) != NULL )
	 {
	 if ( fAll || is_name( arg, pMobIndex->player_name ) )
	 {
	 nMatch++;
	 sprintf( buf, "[%5d] %s" NL,
	 pMobIndex->vnum, capitalize( pMobIndex->short_descr ) );
	 send_to_char( buf, ch );
	 }
	 }
	 }
	 */

	/*
	 * This goes through all the hash entry points (1024), and is therefore
	 * much faster, though you won't get your vnums in order... oh well. :)
	 *
	 * Tests show that Furey's method will usually loop 32,000 times, calling
	 * get_mob_index()... which loops itself, an average of 1-2 times...
	 * So theoretically, the above routine may loop well over 40,000 times,
	 * and my routine bellow will loop for as many index_mobiles are on
	 * your mud... likely under 3000 times.
	 * -Thoric
	 */
	for (hash = 0; hash < MAX_KEY_HASH; hash++)
		for (pMobIndex = mob_index_hash[hash]; pMobIndex;
				pMobIndex = pMobIndex->next)
			if (fAll || nifty_is_name(arg, pMobIndex->player_name))
			{
				nMatch++;
				pager_printf(ch, "[%5d] %s&z&w" NL, pMobIndex->vnum,
						capitalize(pMobIndex->przypadki[0]));
			}

	if (nMatch)
		pager_printf(ch, "Number of matches: %d\n", nMatch);
	else
		send_to_char("Nothing like that in hell, earth, or heaven." NL, ch);

	return;
}

DEF_DO_FUN( ofind )
{
	char arg[MIL];
	char arg2[MIL];
	char arg3[MIL];
	OBJ_INDEX_DATA *pObjIndex;
	int hash;
	int nMatch;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char("ofind <name|flag|wear> <arg>" NL
		"ofind type <type> [valuenr] [value]" NL, ch);
		return;
	}

	nMatch = 0;
	if (!str_cmp(arg, "name"))
	{
		for (hash = 0; hash < MAX_KEY_HASH; hash++)
			for (pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex =
					pObjIndex->next)
				if (nifty_is_name(argument, pObjIndex->name))
				{
					nMatch++;
					pager_printf(ch, "[%5d] %s" EOL, pObjIndex->vnum,
							capitalize(pObjIndex->przypadki[0]));
				}
	}
	else if (!str_cmp(arg, "type"))
	{
		int type, valnr, value;

		argument = one_argument(argument, arg2);

		if ((type = flag_value(obj_types_list, arg2)) < 0)
		{
			send_to_char("No such type" NL, ch);
			return;
		}

		argument = one_argument(argument, arg3);
		valnr = atoi(arg3);
		value = atoi(argument);

		if (valnr < 0 || valnr > 5)
		{
			send_to_char("valuenr range is 0-5." NL, ch);
			return;
		}

		for (hash = 0; hash < MAX_KEY_HASH; hash++)
			for (pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex =
					pObjIndex->next)
				if (*arg3 == '\0' && pObjIndex->item_type == type)
				{
					nMatch++;
					pager_printf(ch, "[%5d] %s" EOL, pObjIndex->vnum,
							capitalize(pObjIndex->przypadki[0]));
				}
				else if (pObjIndex->item_type == type
						&& pObjIndex->value[valnr] == value)
				{
					nMatch++;
					pager_printf(ch, "[%5d] %s" EOL, pObjIndex->vnum,
							capitalize(pObjIndex->przypadki[0]));
				}
	}
	else if (!str_cmp(arg, "flag"))
	{
		int flag;

		if ((flag = flag_value(obj_flags_list, argument)) < 0)
		{
			send_to_char("No such flag" NL, ch);
			return;
		}

		for (hash = 0; hash < MAX_KEY_HASH; hash++)
			for (pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex =
					pObjIndex->next)
				if (IS_SET(pObjIndex->extra_flags, flag))
				{
					nMatch++;
					pager_printf(ch, "[%5d] %s" EOL, pObjIndex->vnum,
							capitalize(pObjIndex->przypadki[0]));
				}

	}
	else if (!str_cmp(arg, "wear"))
	{
		int flag;

		if ((flag = flag_value(wear_flags_list, argument)) < 0)
		{
			send_to_char("No such wear flag" NL, ch);
			return;
		}

		for (hash = 0; hash < MAX_KEY_HASH; hash++)
			for (pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex =
					pObjIndex->next)
				if (IS_SET(pObjIndex->wear_flags, flag))
				{
					nMatch++;
					pager_printf(ch, "[%5d] %s" EOL, pObjIndex->vnum,
							capitalize(pObjIndex->przypadki[0]));
				}

	}
	else if (!str_cmp(arg, "type"))
	{
		for (hash = 0; hash < MAX_KEY_HASH; hash++)
			for (pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex =
					pObjIndex->next)
				if (nifty_is_name(argument, pObjIndex->name))
				{
					nMatch++;
					pager_printf(ch, "[%5d] %s" EOL, pObjIndex->vnum,
							capitalize(pObjIndex->przypadki[0]));
				}
	}

	if (nMatch)
		pager_printf(ch, "Number of matches: %d\n", nMatch);
	else
		send_to_char("Nothing like that in hell, earth, or heaven." NL, ch);

	return;
}

DEF_DO_FUN( mwhere )
{
	char arg[MIL];
	CHAR_DATA *victim;
	bool found;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Mwhere whom?" NL, ch);
		return;
	}

	found = false;
	for (victim = first_char; victim; victim = victim->next)
	{
		if ( IS_NPC(victim) && victim->in_room
				&& nifty_is_name(arg, victim->name))
		{
			found = true;
			pager_printf(ch, "[%5d] %-28s [%5d] %s&z&w" NL,
					victim->pIndexData->vnum, victim->przypadki[0],
					victim->in_room->vnum, victim->in_room->name);
		}
	}

	if (!found)
		act( PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR);

	return;
}

DEF_DO_FUN( bodybag )
{
	char buf2[MAX_STRING_LENGTH];
	char buf3[MAX_STRING_LENGTH];
	char arg[MIL];
	OBJ_DATA *obj;
	bool found;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Bodybag whom?" NL, ch);
		return;
	}

	/* make sure the buf3 is clear? */
	sprintf(buf3, " ");
	/* check to see if vict is playing? */
	sprintf(buf2, "the corpse of %s", arg);
	found = false;
	for (obj = first_object; obj; obj = obj->next)
	{
		if (obj->in_room && !str_cmp(buf2, obj->przypadki[0])
				&& (obj->pIndexData->vnum == 11))
		{
			found = true;
			ch_printf(ch, "Bagging body: [%5d] %-28s [%5d] %s" NL,
					obj->pIndexData->vnum, obj->przypadki[0],
					obj->in_room->vnum, obj->in_room->name);
			obj_from_room(obj);
			obj = obj_to_char(obj, ch);
			obj->timer = -1;
			save_char_obj(ch);
		}
	}

	if (!found)
		ch_printf(ch, " You couldn't find any %s" NL, buf2);
	return;
}

/* New owhere by Altrag, 03/14/96 */
DEF_DO_FUN( owhere )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MIL];
	char arg1[MIL];
	OBJ_DATA *obj;
	bool found;
	int icnt = 0;

	argument = one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Owhere what?" NL, ch);
		return;
	}
	argument = one_argument(argument, arg1);

	if (arg1[0] != '\0' && !str_prefix(arg1, "nesthunt"))
	{
		if (!(obj = get_obj_world(ch, arg)))
		{
			send_to_char("Nesthunt for what object?" NL, ch);
			return;
		}
		for (; obj->in_obj; obj = obj->in_obj)
		{
			pager_printf(ch, "[%5d] %-28s in object [%5d] %s" NL,
					obj->pIndexData->vnum, obj_short(obj),
					obj->in_obj->pIndexData->vnum, obj->in_obj->przypadki[0]);
			++icnt;
		}
		sprintf(buf, "[%5d] %-28s in ", obj->pIndexData->vnum, obj_short(obj));
		if (obj->carried_by)
			sprintf(buf + strlen(buf), "invent [%5d] %s" NL,
					(IS_NPC(obj->carried_by) ?
							obj->carried_by->pIndexData->vnum : 0),
					PERS(obj->carried_by, ch, 0));
		else if (obj->in_room)
			sprintf(buf + strlen(buf), "room   [%5d] %s" NL, obj->in_room->vnum,
					obj->in_room->name);
		else if (obj->in_obj)
		{
			bug("obj->in_obj after NULL!");
			strcat(buf, "object??" NL);
		}
		else
		{
			bug("object doesnt have location!");
			strcat(buf, "nowhere??" NL);
		}
		send_to_pager(buf, ch);
		++icnt;
		pager_printf(ch, "Nested %d levels deep." NL, icnt);
		return;
	}

	found = false;
	for (obj = first_object; obj; obj = obj->next)
	{
		if (!nifty_is_name(arg, obj->name))
			continue;
		found = true;

		sprintf(buf, "(%3d) [%5d] %-28s in ", ++icnt, obj->pIndexData->vnum,
				obj_short(obj));
		if (obj->carried_by)
			sprintf(buf + strlen(buf), "invent [%5d] %s" NL,
					(IS_NPC(obj->carried_by) ?
							obj->carried_by->pIndexData->vnum : 0),
					PERS(obj->carried_by, ch, 0));
		else if (obj->in_room)
			sprintf(buf + strlen(buf), "room   [%5d] %s" NL, obj->in_room->vnum,
					obj->in_room->name);
		else if (obj->in_obj)
			sprintf(buf + strlen(buf), "object [%5d] %s" NL,
					obj->in_obj->pIndexData->vnum, obj_short(obj->in_obj));
		else
		{
			bug("object doesnt have location!");
			strcat(buf, "nowhere??" NL);
		}
		send_to_pager(buf, ch);
	}

	if (!found)
		act( PLAIN, "You didn't find any $T.", ch, NULL, arg, TO_CHAR);
	else
		pager_printf(ch, "%d matches." NL, icnt);

	return;
}

DEF_DO_FUN( reboo )
{
	send_to_char("If you want to REBOOT, spell it out." NL, ch);
	return;
}

DEF_DO_FUN( reboot )
{
	char buf[MAX_STRING_LENGTH];
	extern bool mud_down;
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;

	if (str_cmp(argument, "mud now") && str_cmp(argument, "nosave")
			&& str_cmp(argument, "and sort skill table"))
	{
		send_to_char("Syntax: 'reboot mud now' or 'reboot nosave'" NL, ch);
		return;
	}

	echo_to_all( EOL NL "Wyczuwasz, ¿e olbrzymia si³a nie pozwala ci"
	NL NL "dostrzec"
	NL NL "jak odradza siê ¿ycie ¶wiata..." NL, ECHOTAR_ALL);

	sprintf(buf, "Reboot by %s." NL, ch->name);
	echo_to_all(buf, ECHOTAR_IMM);

	if (!str_cmp(argument, "and sort skill table"))
	{
		sort_skill_table();
		save_skill_table();
	}

	for (d = first_descriptor; d; d = d->next)
	{
		CHAR_DATA *ch = CH(d);
		if (!ch)
			continue;
		if (IS_SET(ch->in_room->room_flags, ROOM_PLR_HOME)
				&& ch->plr_home != ch->in_room)
			evacuate_guests(ch->in_room);
	}

	/* Save all characters before booting. */
	if (str_cmp(argument, "nosave"))
		for (vch = first_char; vch; vch = vch->next)
			if (!IS_NPC(vch))
				save_char_obj(vch);

	sysdata.downtype = DOWN_REBOOT;
	save_sysdata(sysdata);
	mud_down = true;
	return;
}

DEF_DO_FUN( shutdow )
{
	send_to_char("If you want to SHUTDOWN, spell it out." NL, ch);
	return;
}

DEF_DO_FUN( shutdown )
{
	char buf[MAX_STRING_LENGTH];
	extern bool mud_down;
	CHAR_DATA *vch;
	DESCRIPTOR_DATA *d;

	if (str_cmp(argument, "mud now") && str_cmp(argument, "nosave"))
	{
		send_to_char("Syntax: 'shutdown mud now' or 'shutdown nosave'" NL, ch);
		return;
	}

	sprintf(buf, "Shutdown by %s.", ch->name);
	append_file(ch, SHUTDOWN_FILE, buf);
	strcat(buf, EOL);
	echo_to_all(buf, ECHOTAR_IMM);

	for (d = first_descriptor; d; d = d->next)
	{
		CHAR_DATA *ch = CH(d);
		if (!ch)
			continue;
		if (IS_SET(ch->in_room->room_flags, ROOM_PLR_HOME)
				&& ch->plr_home != ch->in_room)
			evacuate_guests(ch->in_room);
	}
	/* Save all characters before booting. */
	if (str_cmp(argument, "nosave"))
		for (vch = first_char; vch; vch = vch->next)
			if (!IS_NPC(vch))
				save_char_obj(vch);

	sysdata.downtype = DOWN_SHUTDOWN;
	save_sysdata(sysdata);
	mud_down = true;
	return;
}

void write_clan_all();
void write_ship_all();
void write_planet_all();
void write_systems_all();
void write_starsystem_list();
void write_astro_list();
void write_ship_list();

DEF_DO_FUN( saveall )
{
	char arg[MIL];
	AREA_DATA *area;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Syntax: saveall <arg>" NL NL
		"Values of arg:" NL
		" clans areas quests ships shipstates systems" NL
		" planets races lists storages" NL, ch);
		return;
	}

	if (!str_cmp(arg, "clans"))
	{
		write_clan_all();
		send_to_char("Clans saved." NL, ch);
		return;
	}

	if (!str_cmp(arg, "areas"))
	{
		FOREACH( area, first_area )
		{
//		fold_area( area, area->filename, false );
			save_area(area);
		}
		save_area_list();
		donemsg(ch);
		return;
	}

	/* Thanos */
	if (!str_cmp(arg, "quests"))
	{
		QUEST_INDEX_DATA *quest;

		for (quest = first_quest_index; quest; quest = quest->next)
			save_quest(quest);
		donemsg(ch);
		ch_printf(ch, "Updating %s..." NL, QUEST_LIST);
		write_quest_list();
		send_to_char("Ok." NL, ch);
		return;
	}

	/* Aldegard */
	if (!str_cmp(arg, "ships"))
	{
		write_ship_all();
		donemsg(ch);
		return;
	}

	/* Thanos */
	if (!str_cmp(arg, "shipstates"))
	{
		save_all_ship_states();
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg, "planets"))
	{
		write_planet_all();
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg, "systems"))
	{
		write_systems_all();
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg, "races"))
	{
		RACE_DATA *race;

		FOREACH( race, first_race )
			save_race(race);
		save_races_list();
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg, "lists"))
	{
		save_races_list();
		save_area_list();
		write_quest_list();
		write_starsystem_list();
		write_astro_list();
		write_ship_list();
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg, "storages"))
	{
		AREA_DATA *area;
		ROOM_INDEX_DATA *room;
		int vnum;

		for (area = first_area; area; area = area->next)
		{
			for (vnum = area->lvnum; vnum <= area->uvnum; ++vnum)
			{
				room = get_room_index(vnum);
				if (!room)
					continue;
				if (IS_SET(room->room_flags, ROOM_STORAGE))
				{
					save_room_storage(room);
				}
			}
		}
		return;
	}

	do_saveall(ch, (char*) "");
	return;
}

DEF_DO_FUN( snoop )
{
	char arg[MIL];
	DESCRIPTOR_DATA *d;
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Snoop whom?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (!victim->desc)
	{
		send_to_char("No descriptor to snoop." NL, ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Cancelling all snoops." NL, ch);
		for (d = first_descriptor; d; d = d->next)
			if (d->snoop_by == ch->desc)
				d->snoop_by = NULL;
		return;
	}

	if (victim->desc->snoop_by)
	{
		send_to_char("Busy already." NL, ch);
		return;
	}

	/*
	 * Minimum snoop level... a secret mset value
	 * makes the snooper think that the victim is already being snooped
	 */
	if (get_trust(victim) >= get_trust(ch)
			|| (victim->pcdata && victim->pcdata->min_snoop > get_trust(ch)))
	{
		send_to_char("Busy already." NL, ch);
		return;
	}

	if (ch->desc)
	{
		for (d = ch->desc->snoop_by; d; d = d->snoop_by)
			if (d->character == victim || d->original == victim)
			{
				send_to_char("No snoop loops." NL, ch);
				return;
			}
	}

	/*  Snoop notification for higher imms, if desired, uncomment this
	 if ( get_trust(victim) > LEVEL_GOD && get_trust(ch) < LEVEL_SUPREME )
	 write_to_descriptor( victim->desc->descriptor, "\n\rYou feel like someone is watching your every move..." NL, 0 );
	 */
	victim->desc->snoop_by = ch->desc;
	send_to_char("Ok." NL, ch);
	return;
}

DEF_DO_FUN( switch )
{
	char arg[MIL];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Switch into whom?" NL, ch);
		return;
	}

	if (!ch->desc)
		return;

	if (ch->desc->original)
	{
		send_to_char("You are already switched." NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Ok." NL, ch);
		return;
	}

	if (victim->desc)
	{
		send_to_char("Character in use." NL, ch);
		return;
	}

	if (!IS_NPC(victim) && get_trust(ch) < LEVEL_GREATER)
	{
		send_to_char("You cannot switch into a player!" NL, ch);
		return;
	}

	ch->desc->character = victim;
	ch->desc->original = ch;
	victim->desc = ch->desc;
	ch->desc = NULL;
	ch->switched = victim;
	send_to_char("Ok." NL, victim);
	return;
}

DEF_DO_FUN( return )
{
	if (!ch->desc)
		return;

	if (!ch->desc->original)
	{
		send_to_char("You aren't switched." NL, ch);
		return;
	}

	if (IS_SET(ch->act, ACT_POLYMORPHED))
	{
		send_to_char("Use revert to return from a polymorphed mob." NL, ch);
		return;
	}

	send_to_char("Twój umys³ powraca do swojego cia³a." NL, ch);
	if ( IS_NPC( ch ) && IS_AFFECTED(ch, AFF_POSSESS))
	{
		affect_strip(ch, gsn_possess);
		REMOVE_BIT(ch->affected_by, AFF_POSSESS);
	}
	/*    if ( IS_NPC( ch->desc->character ) )
	 REMOVE_BIT( ch->desc->character->affected_by, AFF_POSSESS );*/
	ch->desc->character = ch->desc->original;
	ch->desc->original = NULL;
	ch->desc->character->desc = ch->desc;
	ch->desc->character->switched = NULL;
	ch->desc = NULL;
	return;
}

DEF_DO_FUN( minvoke )
{
	char arg[MIL];
	MOB_INDEX_DATA *pMobIndex;
	CHAR_DATA *victim;
	int vnum;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Syntax: mload <vnum>." NL, ch);
		return;
	}

	if (!is_number(arg))
	{
		char arg2[MIL];
		int hash, cnt;
		int count = number_argument(arg, arg2);

		vnum = -1;
		for (hash = cnt = 0; hash < MAX_KEY_HASH; hash++)
			for (pMobIndex = mob_index_hash[hash]; pMobIndex; pMobIndex =
					pMobIndex->next)
				if (nifty_is_name(arg2, pMobIndex->player_name)
						&& ++cnt == count)
				{
					vnum = pMobIndex->vnum;
					break;
				}
		if (vnum == -1)
		{
			send_to_char("No such mobile exists." NL, ch);
			return;
		}
	}
	else
		vnum = atoi(arg);

	if (get_trust(ch) < LEVEL_DEMI || IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	if ((pMobIndex = get_mob_index(vnum)) == NULL)
	{
		send_to_char("No mobile has that vnum." NL, ch);
		return;
	}

	victim = create_mobile(pMobIndex);
	char_to_room(victim, ch->in_room);
	act( COL_IMMORT, "$n stworzy³$o $N$3!", ch, NULL, victim, TO_ROOM);
	send_to_char("Ok." NL, ch);
	return;
}

DEF_DO_FUN( oinvoke )
{
	char arg1[MIL];
	char arg2[MIL];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	int vnum;
	int level;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Syntax: oload <vnum> <level>." NL, ch);
		return;
	}

	if (arg2[0] == '\0')
	{
		level = get_trust(ch);
	}
	else
	{
		if (!is_number(arg2))
		{
			send_to_char("Syntax: oload <vnum> <level>." NL, ch);
			return;
		}
		level = atoi(arg2);
		if (level < 0 || level > get_trust(ch))
		{
			send_to_char("Limited to your trust level." NL, ch);
			return;
		}
	}

	if (!is_number(arg1))
	{
		char arg[MIL];
		int hash, cnt;
		int count = number_argument(arg1, arg);

		vnum = -1;
		for (hash = cnt = 0; hash < MAX_KEY_HASH; hash++)
			for (pObjIndex = obj_index_hash[hash]; pObjIndex; pObjIndex =
					pObjIndex->next)
				if (nifty_is_name(arg, pObjIndex->name) && ++cnt == count)
				{
					vnum = pObjIndex->vnum;
					break;
				}
		if (vnum == -1)
		{
			send_to_char("No such object exists." NL, ch);
			return;
		}
	}
	else
		vnum = atoi(arg1);

	if (get_trust(ch) < LEVEL_DEMI || IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	if ((pObjIndex = get_obj_index(vnum)) == NULL)
	{
		send_to_char("No object has that vnum." NL, ch);
		return;
	}

	obj = create_object(pObjIndex, level);
	if (CAN_WEAR(obj, ITEM_TAKE))
	{
		obj = obj_to_char(obj, ch);
	}
	else
	{
		obj = obj_to_room(obj, ch->in_room);
		act( COL_IMMORT, "$n stworzy³$o $p$3!", ch, obj, NULL, TO_ROOM);
	}
	send_to_char("Ok." NL, ch);
	return;
}

bool purge_room(CHAR_DATA *ch, ROOM_INDEX_DATA *room)
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	CHAR_DATA *vnext;
	OBJ_DATA *obj_next;

	if (!can_edit(ch, room->vnum))
		return false;

	for (victim = room->first_person; victim; victim = vnext)
	{
		vnext = victim->next_in_room;
		if ( IS_NPC( victim ) && victim != ch && !victim->inquest)
			extract_char(victim, true);
	}

	for (obj = room->first_content; obj; obj = obj_next)
	{
		obj_next = obj->next_content;
		if (obj->item_type == ITEM_SHIPMODULE|| obj->inquest
		|| IS_OBJ_STAT( obj, ITEM_BURRIED ))
			continue;

		extract_obj(obj);
	}
	return true;
}

/*
 Thanos: poprawilem tak, by 'purge area' omijalo pokoje prywatne,
 Dodalem tez sprawdzenie czy przedmiot purge'owany nie jest przypadkiem
 zakopany (Burried), wtedy omijamy go
 */
DEF_DO_FUN( purge )
{
	char arg[MIL];
	AREA_DATA *area;
	int vnum;
	ROOM_INDEX_DATA *room;
	CHAR_DATA *victim;
	OBJ_DATA *obj;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		/* 'purge' */
		if (room_is_private(ch, ch->in_room))
		{
			send_to_char("You can't purge a private room!!!", ch);
			return;
		}

		if (purge_room(ch, ch->in_room))
			send_to_char("Ok." NL, ch);
		else
			send_to_char("You failed." NL, ch);
		return;
	}

	if (!str_cmp(arg, "area"))
	{
		if (!(area = ch->in_room->area))
		{
			send_to_char("No area!" NL, ch);
			return;
		}

		if (!is_builder(ch, area))
		{
			send_to_char("No permission. Sorry." NL, ch);
			return;
		}

		victim = NULL;
		obj = NULL;

		for (vnum = area->lvnum; vnum <= area->uvnum; vnum++)
		{
			if (!(room = get_room_index(vnum)))
				continue;

			if (room_is_private(ch, room))
				continue;

			purge_room(ch, room);
		}
		send_to_char("Area purged." NL, ch);
		return;
	}

	victim = NULL;
	obj = NULL;

	if (!can_edit(ch, ch->in_room->vnum))
	{
		send_to_char("You failed." NL, ch);
		return;
	}

	/* fixed to get things in room first -- i.e., purge portal (obj),
	 * no more purging mobs with that keyword in another room first
	 * -- Tri */
	if ((victim = get_char_room(ch, arg)) == NULL
			&& (obj = get_obj_here(ch, arg)) == NULL)
	{
// removed by Thanos. Ze wzglêdu na bezpieczeñstwo (OLC)
// purge robimy tylko w lokacji, w której jeste¶my.
//	if ( ( victim = get_char_world( ch, arg ) ) == NULL
//        &&   ( obj = get_obj_world( ch, arg ) ) == NULL )  /* no get_obj_room */
//        {
		send_to_char("They aren't here." NL, ch);
		return;
//        }

	}

	/* Single object purge in room for high level purge - Scryn 8/12*/
	if (obj && obj->inquest)
	{
		send_to_char("This is a quest obj!" NL, ch);
		return;
	}

	if (obj)
	{
		separate_obj(obj);
//blee	act( COL_IMMORT, "$n dotyka $p$3 palcem, i $p znika!&z&w", ch, obj, NULL, TO_ROOM);
		act( COL_IMMORT, "You make $p disappear in a puff of smoke!", ch, obj,
				NULL, TO_CHAR);
		extract_obj(obj);
		return;
	}

	if (!IS_NPC(victim))
	{
		send_to_char("Not on PC's." NL, ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("You cannot purge yourself!" NL, ch);
		return;
	}

	if (victim->inquest)
	{
		send_to_char("You cannot purge a quest mob!" NL, ch);
		return;
	}

	if (IS_SET(victim->act, ACT_POLYMORPHED))
	{
		send_to_char("You cannot purge a polymorphed player." NL, ch);
		return;
	}
//blee    act( COL_IMMORT, "$n dotyka d³oni± $N$3. $N rozpada siê na ma³e kawa³eczki!", ch, NULL, victim, TO_NOTVICT );
	act( COL_IMMORT, "You make $N disappear in a puff of smoke!", ch, NULL,
			victim, TO_CHAR);
	extract_char(victim, true);
	return;
}

DEF_DO_FUN( balzhur )
{
	char arg[MIL] =
	{ 0 };
	char buf[MAX_STRING_LENGTH] =
	{ 0 };
	char buf2[MAX_STRING_LENGTH] =
	{ 0 };
	CHAR_DATA *victim;
	int sn;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Who is deserving of such a fate?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't playing." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	if (get_trust(victim) >= get_trust(ch))
	{
		send_to_char("I wouldn't even think of that if I were you..." NL, ch);
		return;
	}

	send_to_char( FB_WHITE, ch);
	send_to_char("You summon the demon Balzhur to wreak your wrath!" NL, ch);
	send_to_char(
			"Balzhur sneers at you evilly, then vanishes in a puff of smoke." NL,
			ch);
	send_to_char( COL_IMMORT, victim);
	send_to_char(
			"You hear an ungodly sound in the distance that makes your blood run cold!" NL,
			victim);
	sprintf(buf, "Balzhur screams, 'You are MINE %s!!!'", victim->name);
	echo_to_all(buf, ECHOTAR_ALL);
	victim->top_level = 1;
	victim->trust = 0;
	{
		int ability;

		for (ability = 0; ability < MAX_ABILITY; ability++)
		{
			victim->experience[ability] = 1;
			victim->skill_level[ability] = 1;
		}
	}
	victim->max_hit = DEF_HP;
	victim->max_mana = 0;
	victim->max_move = DEF_MV;
	for (sn = 0; sn < top_sn; sn++)
		victim->pcdata->learned[sn] = 0;
	victim->hit = victim->max_hit;
	victim->mana = victim->max_mana;
	victim->move = victim->max_move;

	sprintf(buf, "%s%s", GOD_DIR, capitalize(victim->name));

	if (!remove(buf))
		send_to_char("Player's immortal data destroyed." NL, ch);
	else if (errno != ENOENT)
	{
		ch_printf(ch, "Unknown error #%d - %s (immortal data).  Report to "
				"Thoric" NL, errno, strerror(errno));
		swsnprintf(buf2, MSL, "%s balzhuring %s", ch->name, buf);
		perror(buf2);
	}
	sprintf(buf2, "%s.are", capitalize(arg));

	//make_wizlist();
	do_help(victim, (char*) "M_BALZHUR_");
	send_to_char( FB_WHITE, victim);
	send_to_char("Budzisz siê po d³ugiej chwili..." NL, victim);

	while (victim->first_carrying)
		extract_obj(victim->first_carrying);
	return;
}

DEF_DO_FUN( advance )
{
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	CHAR_DATA *victim;
	int level, ability;
	int iLevel, iAbility;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg3);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0'
			|| !is_number(arg2))
	{
		send_to_char("Syntax: advance <char> <ability> <level>." NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg1)) == NULL)
	{
		send_to_char("That player is not here." NL, ch);
		return;
	}

	ability = -1;
	for (iAbility = 0; iAbility < MAX_ABILITY; iAbility++)
	{
		if (!str_prefix(arg3, class_table[iAbility].name))
		{
			ability = iAbility;
			break;
		}
	}

	if (ability == -1)
	{
		send_to_char("No Such Ability." NL, ch);
		do_advance(ch, (char*) "");
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	/* You can demote yourself but not someone else at your own trust. -- Narn */
	if (get_trust(ch) <= get_trust(victim) && ch != victim)
	{
		send_to_char("Nie mo¿esz tego zrobiæ." NL, ch);
		return;
	}

	if ((level = atoi(arg2)) < 1 || level > 500)
	{
		send_to_char("Poziom musi byæ z przedzia³u 1 do 500." NL, ch);
		return;
	}

	/*
	 * Lower level:
	 *   Reset to level 1.
	 *   Then raise again.
	 *   Currently, an imp can lower another imp.
	 *   -- Swiftest
	 */
	if (level <= victim->skill_level[ability])
	{
		send_to_char("Lowering a player's level!" NL, ch);
		send_to_char( COL_IMMORT, victim);
		ch_printf(victim, "Przeklêt%s! W³adcy obni¿aj± twój poziom!" EOL,
				SEX_SUFFIX_YAE(victim));
		victim->experience[ability] = 0;
		victim->top_level = 1; //added by Thanos
		victim->skill_level[ability] = 1;
		if (ability == COMBAT_ABILITY)
			victim->max_hit = DEF_HP;
		if (ability == FORCE_ABILITY)
			victim->max_mana = 0;
	}
	else
	{
		send_to_char("Raising a player's level!" NL, ch);
		send_to_char("W³adcy nagradzaj± ciê poziomem!" NL, victim);
	}

	for (iLevel = victim->skill_level[ability]; iLevel < level; iLevel++)
	{
		victim->experience[ability] = exp_level(iLevel + 1);
		gain_exp(victim, 0, ability);
	}
	return;
}

DEF_DO_FUN( immadvance )
{
	char arg[MIL];
	char buf[MSL];
	CHAR_DATA *victim;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Syntax: immadvance <name>" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("That player is not here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	if (victim->top_level < LEVEL_AVATAR)
	{
		send_to_char("This player is not worthy of becoming immortal yet." NL,
				ch);
		return;
	}

	if (victim->top_level > 104)
	{
		send_to_char("That player cannot be more powerful." NL, ch);
		return;
	}

	send_to_char("Empowering a player with immortal powers..." NL, ch);

	act( COL_IMMORT,
			"$n przygl±da ci siê z uwag±... potem wznosi swe rêce do góry...",
			ch, NULL, NULL, TO_ROOM);
	send_to_char( FB_WHITE, victim);
	send_to_char("Wszystko staje siê takie dziwne..." NL NL, victim);
	send_to_char( FB_CYAN, victim);

	sprintf(buf, "M_GODLVL%d_", victim->top_level - 99);
	do_help(victim, buf);
	send_to_char( FB_WHITE, victim);
	send_to_char("Budzisz siê... a wszystkie niedoskona³o¶ci znikaj±." EOL,
			victim);
	while (victim->first_carrying)
		extract_obj(victim->first_carrying);

	victim->top_level++;

	victim->trust = 0;
	return;
}

DEF_DO_FUN( trust )
{
	char arg1[MIL];
	char arg2[MIL];
	CHAR_DATA *victim;
	int level;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2))
	{
		send_to_char("Syntax: trust <char> <level>." NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg1)) == NULL)
	{
		send_to_char("That player is not here." NL, ch);
		return;
	}

	if ((level = atoi(arg2)) < 0 || level > MAX_LEVEL)
	{
		send_to_char("Level must be 0 (reset) or 1 to 105." NL, ch);
		return;
	}

	if (level > get_trust(ch))
	{
		send_to_char("Limited to your own trust." NL, ch);
		return;
	}

	if (get_trust(victim) >= get_trust(ch))
	{
		send_to_char("You can't do that." NL, ch);
		return;
	}

	victim->trust = level;
	save_char_obj(victim);
	send_to_char("Ok." NL, ch);
	return;
}

/* Moim zdaniem restore powinien od¶wie¿aæ postaæ a nie tylko hp, force i mv
 Bzdur± jest, ¿e np. menatl state zostaje taki sam, albo thirst   -Thanos
 Ja tez tak mysle wiec wez i zrob to tak jak ma byc -Aldegard
 */
/* Alde:
 * to ju¿ jest od dawna zmienione. Dlatego napisa³em tamten komentarz :)
 * Thanos
 */
void restore_char(CHAR_DATA *victim)
{
	victim->hit = victim->max_hit;
	victim->mana = victim->max_mana;
	victim->move = victim->max_move;
	victim->mental_state = 0;

	if (victim->pcdata)
	{
		victim->pcdata->condition[COND_BLOODTHIRST] = (10 + victim->top_level);
		victim->pcdata->condition[COND_FULL] = COND_MAX;
		victim->pcdata->condition[COND_THIRST] = COND_MAX;
	}
	update_pos(victim);
	return;
}

DEF_DO_FUN( restore )
{
	char arg[MIL];

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Restore whom?" NL, ch);
		return;
	}

	if (!str_cmp(arg, "all"))
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;

		if (!ch->pcdata)
			return;

		if (get_trust(ch) < LEVEL_SUB_IMPLEM)
		{
			if (IS_NPC(ch))
			{
				send_to_char("You can't do that." NL, ch);
				return;
			}
			else
			{
				/* Check if the player did a restore all within the last 18 hours. */
				if (current_time - last_restore_all_time < RESTORE_INTERVAL)
				{
					send_to_char("Sorry, you can't do a restore all yet." NL,
							ch);
					do_restoretime(ch, (char*) "");
					return;
				}
			}
		}
		last_restore_all_time = current_time;
		ch->pcdata->restore_time = current_time;
		save_char_obj(ch);
		send_to_char("Ok." NL, ch);
		for (vch = first_char; vch; vch = vch_next)
		{
			vch_next = vch->next;

			if (!IS_NPC(vch) && !IS_IMMORTAL(vch))
			{
				restore_char(vch);
				ch_printf(vch, "%s przywraca ci pe³niê si³." EOL,
						PERS(ch, vch, 0));
			}
		}
	}
	else
	{

		CHAR_DATA *victim;

		if ((victim = get_char_world(ch, arg)) == NULL)
		{
			send_to_char("They aren't here." NL, ch);
			return;
		}

		if (get_trust(ch) < LEVEL_LESSER && victim != ch
				&& !( IS_NPC( victim ) && IS_SET(victim->act, ACT_PROTOTYPE)))
		{
			send_to_char("You can't do that." NL, ch);
			return;
		}

		restore_char(victim);
		if (ch != victim)
			ch_printf(victim, "%s przywraca ci pe³niê si³." EOL,
					PERS(ch, victim, 0));
		send_to_char("Ok." NL, ch);
		return;
	}
}

DEF_DO_FUN( restoretime )
{
	long int time_passed;
	int hour, minute;

	if (!last_restore_all_time)
		ch_printf(ch, "There has been no restore all since reboot" NL);
	else
	{
		time_passed = current_time - last_restore_all_time;
		hour = (int) (time_passed / 3600);
		minute = (int) ((time_passed - (hour * 3600)) / 60);
		ch_printf(ch,
				"The  last restore all was %d hours and %d minutes ago." NL,
				hour, minute);
	}

	if (!ch->pcdata)
		return;

	if (!ch->pcdata->restore_time)
	{
		send_to_char("You have never done a restore all." NL, ch);
		return;
	}

	time_passed = current_time - ch->pcdata->restore_time;
	hour = (int) (time_passed / 3600);
	minute = (int) ((time_passed - (hour * 3600)) / 60);
	ch_printf(ch, "Your last restore all was %d hours and %d minutes ago." NL,
			hour, minute);
	return;
}

DEF_DO_FUN( freeze )
{
	char arg[MIL];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Freeze whom?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	if (get_trust(victim) >= get_trust(ch))
	{
		send_to_char("You failed." NL, ch);
		return;
	}

	if (IS_SET(victim->act, PLR_FREEZE))
	{
		REMOVE_BIT(victim->act, PLR_FREEZE);
		send_to_char("Mo¿esz znów graæ." NL, victim);
		send_to_char("FREEZE usuniêty." NL, ch);
	}
	else
	{
		SET_BIT(victim->act, PLR_FREEZE);
		send_to_char("Nie mo¿esz zrobiæ NICZEGO!" NL, victim);
		send_to_char("FREEZE ustawiony." NL, ch);
	}

	save_char_obj(victim);

	return;
}

DEF_DO_FUN( log )
{
	char arg[MIL];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Log whom?" NL, ch);
		return;
	}

	if (!str_cmp(arg, "all"))
	{
		if (fLogAll)
		{
			fLogAll = false;
			send_to_char("Log ALL off." NL, ch);
		}
		else
		{
			fLogAll = true;
			send_to_char("Log ALL on." NL, ch);
		}
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	/*
	 * No level check, gods can log anyone.
	 */
	if (IS_SET(victim->act, PLR_LOG))
	{
		REMOVE_BIT(victim->act, PLR_LOG);
		send_to_char("LOG removed." NL, ch);
	}
	else
	{
		SET_BIT(victim->act, PLR_LOG);
		send_to_char("LOG set." NL, ch);
	}

	return;
}

DEF_DO_FUN( litterbug )
{
	char arg[MIL];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Set litterbug flag on whom?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	if (get_trust(victim) >= get_trust(ch))
	{
		send_to_char("You failed." NL, ch);
		return;
	}

	if (IS_SET(victim->act, PLR_LITTERBUG))
	{
		REMOVE_BIT(victim->act, PLR_LITTERBUG);
		send_to_char("Mo¿esz znów odrzucaæ ¶mieci." NL, victim);
		send_to_char("LITTERBUG removed." NL, ch);
	}
	else
	{
		SET_BIT(victim->act, PLR_LITTERBUG);
		send_to_char("Co¶ nie pozwala ci odrzucaæ przedmiotów!" NL, victim);
		send_to_char("LITTERBUG set." NL, ch);
	}

	return;
}

DEF_DO_FUN( peace )
{
	CHAR_DATA *rch;

	act( COL_IMMORT, "$n krzyczy 'SPOKÓJ!'&z&w", ch, NULL, NULL, TO_ROOM);
	for (rch = ch->in_room->first_person; rch; rch = rch->next_in_room)
	{
		if (rch->fighting)
			stop_fighting(rch, true);

		/* Added by Narn, Nov 28/95 */
		stop_hating(rch);
		stop_hunting(rch);
		stop_fearing(rch);
	}

	send_to_char("Ok." NL, ch);
	return;
}

/* Thanos, rename */
DEF_DO_FUN( rename )
{
	CHAR_DATA *victim;
	struct stat fst;
	char filename[MIL];
	char arg1[MIL];
	char arg2[MIL];
	int i;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (!ch->desc || IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	if (!*arg1 || !*arg2)
	{
		send_to_char("Syntax: rename <who> <new_name>" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("No such player." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		ch_printf(ch, "%s is a mob! Cannot rename." NL, victim->przypadki[0]);
		return;
	}

	if (get_trust(victim) > get_trust(ch) || ch == victim)
	{
		send_to_char("You failed." NL, ch);
		return;
	}

	if (!check_parse_name(arg2))
	{
		send_to_char("New name not acceptable." NL, ch);
		return;
	}

	sprintf(filename, "%s%s", PDATA_DIR, capitalize(arg2));
	if (stat(filename, &fst) != -1)
	{
		send_to_char("There is a player with that name." NL, ch);
		return;
	}

	/*
	 * Teraz tak:
	 * ¯eby by³o ³adnie i stabilnie, nie zmieniamy imienia graczowi od
	 * razu. Tworzymy sobie postaæ tymczasow± i zapamiêtujemy na ni± wska¼nik.
	 * Immo mo¿e teraz bezstresowo edytowaæ postaæ tymczasow±, a jak skoñczy,
	 * jej dane zostan± skopiowane na gracza.
	 * Dodatkowo, ¿eby by³o wygodnie, nasz edytor bêdzie pyta³ o przypadki po
	 * kolei. Numer aktualnie edytowanego przypadku mamy zapamiêtany w ...
	 * .. levelu postaci tymczasowej :)
	 */
	CREATE(ch->desc->tmp_ch, CHAR_DATA, 1);
	clear_char(ch->desc->tmp_ch);
	STRDUP(ch->desc->tmp_ch->name, capitalize(victim->name));
	STRDUP(ch->desc->tmp_ch->przypadki[0], capitalize(arg2));
	for (i = 1; i < 6; i++)
		STRDUP(ch->desc->tmp_ch->przypadki[i], victim->przypadki[i]);

	ch->desc->tmp_ch->top_level = 0;

	ch->desc->connected = CON_RENAMING;

	ch_printf(ch, "Renaming " FB_WHITE "%s" PLAIN " to " FB_WHITE "%s" EOL,
			victim->name, capitalize(arg2));
	ch_printf(ch, "Use "
	FB_WHITE "<enter>" PLAIN " to set an old value, "
	FB_WHITE "@" PLAIN " to abort, "
	FB_WHITE "?" PLAIN " to show state, "
	FB_WHITE "/" PLAIN " to step back." NL);

	process_rename(ch->desc, capitalize(arg2));
	return;
}

BAN_DATA *first_ban;
BAN_DATA *last_ban;

void save_banlist(void)
{
	BAN_DATA *pban;
	FILE *fp;

	RESERVE_CLOSE;
	if (!(fp = fopen( BAN_LIST, "w")))
	{
		bug("Cannot open " BAN_LIST);
		perror(BAN_LIST);
		RESERVE_OPEN;
		return;
	}
	for (pban = first_ban; pban; pban = pban->next)
		fprintf(fp, "%d %s~~%s~\n", pban->level, pban->name, pban->ban_time);
	fprintf(fp, "-1\n");
	fclose(fp);
	RESERVE_OPEN;
	return;
}

DEF_DO_FUN( ban )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MIL];
	BAN_DATA *pban;
	int bnum;
	regex_t preg;

	if (IS_NPC(ch))
		return;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_pager("Banned sites:" NL, ch);
		send_to_pager("[ #]  (Lv)  Time                     Site" NL, ch);
		send_to_pager("---- ------ ------------------------ ---------------" NL,
				ch);
		for (pban = first_ban, bnum = 1; pban; pban = pban->next, bnum++)
		{
			pager_printf(ch, "[%2d] (%4s) %-24s %s" NL, bnum,
					pban->level == BAN_SOFT ? "soft" : itoa(pban->level),
					pban->ban_time, pban->name);
		}
		return;
	}

	/* People are gonna need .# instead of just # to ban by just last
	 number in the site ip.                               -- Altrag */
	if ( is_number(arg))
	{
		for (pban = first_ban, bnum = 1; pban; pban = pban->next, bnum++)
			if (bnum == atoi(arg))
				break;
		if (!pban)
		{
			do_ban(ch, (char*) "");
			return;
		}
		argument = one_argument(argument, arg);
		if (arg[0] == '\0')
		{
			do_ban(ch, (char*) "help");
			return;
		}
		if (!str_cmp(arg, "level"))
		{
			argument = one_argument(argument, arg);
			if (arg[0] == '\0' || !is_number(arg))
			{
				do_ban(ch, (char*) "help");
				return;
			}
			if (atoi(arg) < 1 || atoi(arg) > LEVEL_SUPREME)
			{
				ch_printf(ch, "Level range: 1 - %d." NL, LEVEL_SUPREME);
				return;
			}
			pban->level = atoi(arg);
			send_to_char("Ban level set." NL, ch);
		}
		else if (!str_cmp(arg, "newban"))
		{
			pban->level = 1;
			send_to_char("New characters banned." NL, ch);
		}
		else if (!str_cmp(arg, "mortal"))
		{
			pban->level = LEVEL_AVATAR;
			send_to_char("All mortals banned." NL, ch);
		}
		else if (!str_cmp(arg, "total"))
		{
			pban->level = LEVEL_SUPREME;
			send_to_char("Everyone banned." NL, ch);
		}
		else if (!str_cmp(arg, "soft"))
		{
			pban->level = BAN_SOFT;
			send_to_char("Ok. No autoauth for this site." NL, ch);
		}
		else
		{
			do_ban(ch, (char*) "help");
			return;
		}
		save_banlist();
		return;
	}

	if (!str_cmp(arg, "help"))
	{
		send_to_char("Syntax: ban <regexp>" NL, ch);
		send_to_char(
				"Syntax: ban <number> <level> | newban | mortal | total" NL,
				ch);
		return;
	}

	if (regcomp(&preg, arg, REG_CFLAGS))
	{
		bug("malformed regular expression: %s", arg);
		return;
	}

	for (pban = first_ban; pban; pban = pban->next)
		if (!str_cmp(arg, pban->name))
		{
			send_to_char("That site is already banned!" NL, ch);
			return;
		}

	CREATE(pban, BAN_DATA, 1);
	LINK(pban, first_ban, last_ban, next, prev);
	STRDUP(pban->name, arg);
	pban->level = LEVEL_AVATAR;
	sprintf(buf, "%24.24s", ctime(&current_time));
	STRDUP(pban->ban_time, buf);
	save_banlist();
	send_to_char("Ban created.  Mortals banned from site." NL, ch);
	return;
}

DEF_DO_FUN( allow )
{
	char arg[MIL];
	BAN_DATA *pban;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Remove which site from the ban list?" NL, ch);
		return;
	}

	for (pban = first_ban; pban; pban = pban->next)
	{
		if (!str_cmp(arg, pban->name))
		{
			UNLINK(pban, first_ban, last_ban, next, prev);
			free_ban(pban);
			save_banlist();
			send_to_char("Site no longer banned." NL, ch);
			return;
		}
	}

	send_to_char("Site is not banned." NL, ch);
	return;
}

DEF_DO_FUN( wizlock )
{
	wizlock = !wizlock;

	if (wizlock)
		send_to_char("Game wizlocked." NL, ch);
	else
		send_to_char("Game un-wizlocked." NL, ch);

	return;
}

DEF_DO_FUN( noresolve )
{
	sysdata.NO_NAME_RESOLVING = !sysdata.NO_NAME_RESOLVING;

	if (sysdata.NO_NAME_RESOLVING)
		send_to_char("Name resolving disabled." NL, ch);
	else
		send_to_char("Name resolving enabled." NL, ch);

	return;
}

DEF_DO_FUN( users )
{
	char buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA *d;
	int count;
	char arg[MIL];

	one_argument(argument, arg);
	count = 0;
	buf[0] = '\0';

	if (arg[0] != '\0' && !str_prefix(arg, "constates"))
		sprintf(buf, PLAIN
		"Constate     | Player        HostIP" NL
		"-------------+------------------------------------------------" NL);
	else
		sprintf(buf, PLAIN
		"Desc|Con|Idle| Port | IPv6 | Player       | HostIP              " NL
		"----+---+----+------+------+--------------+---------------------" NL);
	send_to_pager(buf, ch);

	for (d = first_descriptor; d; d = d->next)
	{
		if (arg[0] == '\0')
		{
			if (get_trust(ch) >= LEVEL_SUPREME
					|| (d->character && can_see(ch, d->character)))
			{
				count++;
				sprintf(buf, " %3d| %2d|%4d|%6d|%6s| %-12s ", d->descriptor,
						d->connected, d->idle / 4, d->port,
						d->ipv6 ? "yes" : "no",
						(d->original && d->original->name) ? d->original->name :
						(d->character && d->character->name) ?
								d->character->name : "(none)");
				if (get_trust(ch) >= LEVEL_GOD)
					sprintf(buf + strlen(buf), "| %s@%s", d->user, d->host);
				strcat(buf, "" NL);
				send_to_pager(buf, ch);
			}
		}
		else
		{
			if (!str_prefix(arg, "constates"))
			{
				if ((get_trust(ch) >= LEVEL_SUPREME
						|| (d->character && can_see(ch, d->character))))
				{
					count++;
					pager_printf(ch, " %-11s | %-12s ",
							bit_name(constate_types_list, d->connected),
							d->original ? d->original->name :
							d->character ? d->character->name : "(none)");
					buf[0] = '\0';
					if (get_trust(ch) >= LEVEL_GOD)
						sprintf(buf, "| %s@%s", d->user, d->host);
					strcat(buf, "" NL);
					send_to_pager(buf, ch);
				}
			}
			else
			{
				if ((get_trust(ch) >= LEVEL_SUPREME
						|| (d->character && can_see(ch, d->character)))
						&& (!str_prefix(arg, d->host)
								|| (d->character
										&& !str_prefix(arg, d->character->name))))
				{
					count++;
					pager_printf(ch, " %3d| %2d|%4d|%6d|%6s| %-12s ",
							d->descriptor, d->connected, d->idle / 4, d->port,
							d->ipv6 ? "yes" : "no",
							d->original ? d->original->name :
							d->character ? d->character->name : "(none)");
					buf[0] = '\0';
					if (get_trust(ch) >= LEVEL_GOD)
						sprintf(buf, "| %s@%s", d->user, d->host);
					strcat(buf, "" NL);
					send_to_pager(buf, ch);
				}
			}
		}
	}
	pager_printf(ch, "%d user%s." NL, count, count == 1 ? "" : "s");
	return;
}

/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
DEF_DO_FUN( force )
{
	char arg[MIL];
	bool mobsonly;
	argument = one_argument(argument, arg);

	if (arg[0] == '\0' || argument[0] == '\0')
	{
		send_to_char("Force whom to do what?" NL, ch);
		return;
	}

	mobsonly = get_trust(ch) < sysdata.level_forcepc;

	if (!str_cmp(arg, "all"))
	{
		CHAR_DATA *vch;
		CHAR_DATA *vch_next;

		if (mobsonly)
		{
			send_to_char("Force whom to do what?" NL, ch);
			return;
		}

		for (vch = first_char; vch; vch = vch_next)
		{
			vch_next = vch->next;

			if (!IS_NPC(vch) && get_trust(vch) < get_trust(ch))
			{
				act( COL_IMMORT,
						"Za spraw± $n$1 czujesz, ¿e dobrze by³oby zrobiæ: '$t'.",
						ch, argument, vch, TO_VICT);
				interpret(vch, argument);
			}
		}
	}
	else
	{
		CHAR_DATA *victim;

		if ((victim = get_char_world(ch, arg)) == NULL)
		{
			send_to_char("They aren't here." NL, ch);
			return;
		}

		if (victim == ch)
		{
			send_to_char("Aye aye, right away!" NL, ch);
			return;
		}

		if ((get_trust(victim) >= get_trust(ch))
				|| (mobsonly && !IS_NPC(victim)))
		{
			send_to_char("Do it yourself!" NL, ch);
			return;
		}

		act( COL_IMMORT,
				"Za spraw± $n$1 czujesz, ¿e dobrze by³oby zrobiæ: '$t'.", ch,
				argument, victim, TO_VICT);
		interpret(victim, argument);
	}

	send_to_char("Ok." NL, ch);
	return;
}

DEF_DO_FUN( invis )
{
	char arg[MIL];
	int level;

	/*
	 if ( IS_NPC(ch))
	 return;
	 */

	argument = one_argument(argument, arg);
	if (*arg)
	{
		if ( is_number(arg))
		{
			level = atoi(arg);
			if (level < 2 || level > get_trust(ch))
			{
				send_to_char("Invalid level." NL, ch);
				return;
			}

			if (!IS_NPC(ch))
			{
				ch->pcdata->wizinvis = level;
				ch_printf(ch, "Wizinvis level set to %d." NL, level);
			}

			if (IS_NPC(ch))
			{
				ch->mobinvis = level;
				ch_printf(ch, "Mobinvis level set to %d." NL, level);
			}
			return;
		}
		else if (!str_cmp(arg, "on"))
		{
			SET_BIT(ch->act, PLR_WIZINVIS);
			act( COL_IMMORT, "$n powoli zlewa siê z otoczeniem.", ch, NULL,
					NULL, TO_ROOM);
			send_to_char("Wizinvis set." NL, ch);
		}
		else if (!str_cmp(arg, "off"))
		{
			REMOVE_BIT(ch->act, PLR_WIZINVIS);
			act( COL_IMMORT, "$n powoli materializuje siê.", ch, NULL, NULL,
			TO_ROOM);
			send_to_char("Wizinvis removed." NL, ch);
		}
		else if (!str_cmp(arg, "except"))
		{
			if (IS_NPC(ch))
			{
				send_to_char("Players only..." NL, ch);
				return;
			}

			if (argument[0] == '\0')
			{
				if (ch->pcdata->invis_except[0] != '\0')
					ch_printf(ch,
							"List of people who can see you in spite of your invis:" NL "%s" EOL,
							ch->pcdata->invis_except);
				else
					ch_printf(ch, "Nobody can see you." NL);

				return;
			}

			one_argument(argument, arg);
			arg[0] = UPPER(arg[0]);

			if (is_name(arg, ch->pcdata->invis_except))
			{
				char buf[MSL];

				strcpy(buf, ch->pcdata->invis_except);
				STRDUP(ch->pcdata->invis_except,
						(char* )cut_from_string(buf, arg));

				ch_printf(ch, "'%s" PLAIN "' can no longer see you." NL, arg);
			}
			else
			{
				char buf[MSL];

				buf[0] = '\0';
				if (ch->pcdata->invis_except)
				{
					strcat(buf, ch->pcdata->invis_except);
					strcat(buf, " ");
				}
				strcat(buf, arg);
				STRDUP(ch->pcdata->invis_except, buf);
				ch_printf(ch,
						"'%s" PLAIN "' can see you in spite of your invis." NL,
						arg);
			}
			return;
		}
		else
			send_to_char("Syntax: invis" NL
			"or      invis  on" NL
			"or      invis  off" NL
			"or      invis  except" NL
			"or      invis  except <name>" NL
			"or      invis  except none" NL, ch);
		return;
	}

	if (!IS_NPC(ch))
	{
		if (ch->pcdata->wizinvis < 2)
			ch->pcdata->wizinvis = ch->top_level;
	}

	if (IS_NPC(ch))
	{
		if (ch->mobinvis < 2)
			ch->mobinvis = ch->top_level;
	}

	if (IS_SET(ch->act, PLR_WIZINVIS))
	{
		REMOVE_BIT(ch->act, PLR_WIZINVIS);
		act( COL_IMMORT, "$n powoli materializuje siê.", ch, NULL, NULL,
		TO_ROOM);
		send_to_char("Wizinvis removed." NL, ch);
	}
	else
	{
		SET_BIT(ch->act, PLR_WIZINVIS);
		act( COL_IMMORT, "$n powoli zlewa siê z otoczeniem.", ch, NULL, NULL,
		TO_ROOM);
		send_to_char("Wizinvis set." NL, ch);
	}

	return;
}

DEF_DO_FUN( holylight )
{
	if (IS_NPC(ch))
		return;

	if (IS_SET(ch->act, PLR_HOLYLIGHT))
	{
		REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
		send_to_char("Holy light mode OFF." NL, ch);
	}
	else
	{
		SET_BIT(ch->act, PLR_HOLYLIGHT);
		send_to_char("Holy light mode ON." NL, ch);
	}

	return;
}

DEF_DO_FUN( cmdtable )
{
	int hash;
	CMDTYPE *cmd;
	int i = 0;

	send_to_pager(
			FB_WHITE " Nr Command     Level    Log     Position  Used  Flags     Aliases" EOL,
			ch);

	for (hash = 0; hash < MAX_CMD_HASH; hash++)
		for (cmd = command_hash[hash]; cmd; cmd = cmd->next)
		{
			const char *col;

			if (!str_cmp(skill_name(cmd->do_fun), "reserved"))
				col = FB_YELLOW;
			else
				col = PLAIN;

			i++;
			pager_printf(ch, "%s%3d %-12.12s  %3d  %8.8s  %8.8s  %4d  ", col, i,
					cmd->name, cmd->level,
					flag_string(log_types_list, cmd->log),
					bit_name(position_types_list, cmd->position),
					cmd->userec.num_uses);
			pager_printf(ch, "%-8.8s  %-20.20s" EOL,
					flag_string(command_flags_list, cmd->flags), cmd->alias);
		}
	ch_printf(ch,
			NL "You should remove commands shown in the yellow lines. (if there are some)" NL);
	return;
}

/*
 * Load up a player file
 */
DEF_DO_FUN( loadup )
{
	char fname[1024];
	char name[256];
	struct stat fst;
	DESCRIPTOR_DATA *d;
	int old_room_vnum;

	one_argument(argument, name);
	if (name[0] == '\0')
	{
		send_to_char("Usage: loadup <playername>" NL, ch);
		return;
	}

	name[0] = UPPER(name[0]);

	sprintf(fname, "%s%s", PDATA_DIR, capitalize(name));
	if (stat(fname, &fst) != -1)
	{
		CREATE(d, DESCRIPTOR_DATA, 1);
		d->next = NULL;
		d->prev = NULL;
		d->connected = CON_GET_NAME;
		d->outsize = 2000;
		CREATE(d->outbuf, char, d->outsize);

		load_char_obj(d, name, false);
		add_char(d->character);
		old_room_vnum = d->character->in_room->vnum;
		char_to_room(d->character, ch->in_room);
		if (get_trust(d->character) >= get_trust(ch))
		{
			do_say(d->character, (char*) "*NIE* przeszkadzaæ!");
			send_to_char("T± osobê lepiej zostawiæ w spokoju!" NL, ch);
			d->character->desc = NULL;
			do_quit(d->character, (char*) "");
			return;
		}
		d->character->desc = NULL;
		d->character->retran = old_room_vnum;
		d->character = NULL;
		DISPOSE(d->outbuf);
		DISPOSE(d);
		ch_printf(ch, "Player %s loaded from room %d." NL, capitalize(name),
				old_room_vnum);
		donemsg(ch);
		return;
	}
	/* else no player file */
	send_to_char("No such player." NL, ch);
	return;
}

DEF_DO_FUN( fixchar )
{
	char name[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, name);
	if (name[0] == '\0')
	{
		send_to_char("Usage: fixchar <playername>" NL, ch);
		return;
	}
	victim = get_char_room(ch, name);
	if (!victim)
	{
		send_to_char("They're not here." NL, ch);
		return;
	}
	fix_char(victim);
	/*  victim->armor	= 100;
	 victim->mod_str	= 0;
	 victim->mod_dex	= 0;
	 victim->mod_wis	= 0;
	 victim->mod_int	= 0;
	 victim->mod_con	= 0;
	 victim->mod_cha	= 0;
	 victim->mod_lck	= 0;
	 victim->damroll	= 0;
	 victim->hitroll	= 0;
	 victim->alignment	= URANGE( -1000, victim->alignment, 1000 );
	 victim->saving_spell_staff = 0; */
	donemsg(ch);
}

DEF_DO_FUN( newbieset )
{
	char arg1[MIL];
	char arg2[MIL];
	OBJ_DATA *obj;
	CHAR_DATA *victim;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Syntax: newbieset <char>." NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg1)) == NULL)
	{
		send_to_char("That player is not here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on NPC's." NL, ch);
		return;
	}

	if ((victim->top_level < 1) || (victim->top_level > 5))
	{
		send_to_char("Level of victim must be 1 to 5." NL, ch);
		return;
	}
	obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 1);
	obj_to_char(obj, victim);

	obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_DAGGER), 1);
	obj_to_char(obj, victim);

	/* Added by Brittany, on Nov. 24, 1996. The object is the adventurer's
	 guide to the realms of despair, part of academy.are. */
	{
		OBJ_INDEX_DATA *obj_ind = get_obj_index(10333);
		if (obj_ind != NULL)
		{
			obj = create_object(obj_ind, 1);
			obj_to_char(obj, victim);
		}
	}

	/* Added the burlap sack to the newbieset.  The sack is part of sgate.are
	 called Spectral Gate.  Brittany */

	{

		OBJ_INDEX_DATA *obj_ind = get_obj_index(123);
		if (obj_ind != NULL)
		{
			obj = create_object(obj_ind, 1);
			obj_to_char(obj, victim);
		}
	}

	act( COL_IMMORT, "$n wyposa¿y³$o ciê w zestaw m³odego gracza.", ch, NULL,
			victim, TO_VICT);
	ch_printf(ch, "You have re-equipped %s." NL, victim->name);
	return;
}

/*
 * Extract area names from "input" string and place result in "output" string
 * e.g. "aset joe.are sedit susan.are cset" --> "joe.are susan.are"
 * - Gorog
 */
void extract_area_names(char *inp, char *out)
{
	char buf[MIL], *pbuf = buf;
	int len;

	*out = '\0';
	while (inp && *inp)
	{
		inp = one_argument(inp, buf);
		if ((len = strlen(buf)) >= 5 && !strcmp(".are", pbuf + len - 4))
		{
			if (*out)
				strcat(out, " ");
			strcat(out, buf);
		}
	}
}

/*
 * Remove area names from "input" string and place result in "output" string
 * e.g. "aset joe.are sedit susan.are cset" --> "aset sedit cset"
 * - Gorog
 */
void remove_area_names(char *inp, char *out)
{
	char buf[MIL], *pbuf = buf;
	int len;

	*out = '\0';
	while (inp && *inp)
	{
		inp = one_argument(inp, buf);
		if ((len = strlen(buf)) < 5 || strcmp(".are", pbuf + len - 4))
		{
			if (*out)
				strcat(out, " ");
			strcat(out, buf);
		}
	}
}

DEF_DO_FUN( bestow )
{
	char arg1[MIL];
	char arg2[MIL];
	CHAR_DATA *victim;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Bestow whom with what?" NL, ch);
		return;
	}

	if ((victim = get_char_world(ch, arg1)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("You can't give special abilities to a mob!" NL, ch);
		return;
	}

	if (get_trust(victim) > get_trust(ch))
	{
		send_to_char("You aren't powerful enough..." NL, ch);
		return;
	}

	if (!victim->pcdata->bestowments)
		STRDUP(victim->pcdata->bestowments, "");

	if (arg2[0] == '\0' || !str_cmp(arg2, "list"))
	{
		ch_printf(ch, "Current bestowed commands on %s: %s." NL, victim->name,
				victim->pcdata->bestowments);
		return;
	}

	if (!str_cmp(arg2, "none"))
	{
		STRDUP(victim->pcdata->bestowments, "");
		ch_printf(ch, "Bestowments removed from %s." NL, victim->name);
//        ch_printf( victim, "%s has removed your bestowed commands." NL, ch->name );
		return;
	}

	toggle_string(&victim->pcdata->bestowments, arg2);

//    ch_printf( victim, "%s has bestowed on you the command(s): %s" NL,
//             ch->name, argument );
	donemsg(ch);
}

struct tm* update_time(struct tm *old_time)
{
	time_t time;

	time = mktime(old_time);
	return localtime(&time);
}

DEF_DO_FUN( set_boot_time )
{
	char arg[MIL];
	char arg1[MIL];
	bool check;

	check = false;

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char(
				"Syntax: setboot time {hour minute <day> <month> <year>}" NL,
				ch);
		send_to_char("        setboot manual {0/1}" NL, ch);
		send_to_char("        setboot <reboot|copyover|shutdown>" NL, ch);
		send_to_char("        setboot default" NL, ch);
		ch_printf(ch, EOL
		"Boot time is currently set to " FB_WHITE "%s" PLAIN
		"Mud will " FB_WHITE "%s" PLAIN " at this time." EOL
		"Manual bit is set to " FB_WHITE "%d" EOL,
				SWTimeStamp(new_boot_time).getDescriptive().c_str(),
				sysdata.reboot_type == REB_SHUTDOWN ? "shutdown" :
				sysdata.reboot_type == REB_COPYOVER ? "copyover" : "reboot",
				set_boot_time->manual);
		return;
	}

	if (!str_cmp(arg, "time"))
	{
		struct tm *now_time;

		argument = one_argument(argument, arg);
		argument = one_argument(argument, arg1);

		if (!*arg || !*arg1 || !is_number(arg) || !is_number(arg1))
		{
			send_to_char("You must input a value for hour and minute." NL, ch);
			return;
		}

		now_time = localtime(&current_time);

		if ((now_time->tm_hour = atoi(arg)) < 0 || now_time->tm_hour > 23)
		{
			send_to_char("Valid range for hour is 0 to 23." NL, ch);
			return;
		}

		if ((now_time->tm_min = atoi(arg1)) < 0 || now_time->tm_min > 59)
		{
			send_to_char("Valid range for minute is 0 to 59." NL, ch);
			return;
		}

		argument = one_argument(argument, arg);
		if (*arg != '\0' && is_number(arg))
		{
			if ((now_time->tm_mday = atoi(arg)) < 1 || now_time->tm_mday > 31)
			{
				send_to_char("Valid range for day is 1 to 31." NL, ch);
				return;
			}
			argument = one_argument(argument, arg);
			if (*arg != '\0' && is_number(arg))
			{
				if ((now_time->tm_mon = atoi(arg)) < 1 || now_time->tm_mon > 12)
				{
					send_to_char("Valid range for month is 1 to 12." NL, ch);
					return;
				}
				now_time->tm_mon--;
				argument = one_argument(argument, arg);
				if ((now_time->tm_year = atoi(arg) - 1900) < 0
						|| now_time->tm_year > 199)
				{
					send_to_char("Valid range for year is 1900 to 2099." NL,
							ch);
					return;
				}
			}
		}
		now_time->tm_sec = 0;
		if (mktime(now_time) < current_time)
		{
			send_to_char("You can't set a time previous to today!" NL, ch);
			return;
		}
		if (set_boot_time->manual == 0)
			set_boot_time->manual = 1;
		new_boot_time = update_time(now_time);
		new_boot_struct = *new_boot_time;
		new_boot_time = &new_boot_struct;
		reboot_check(mktime(new_boot_time));

		ch_printf(ch, "Boot time set to %s" NL,
				SWTimeStamp(new_boot_time).getDescriptive().c_str());
		check = true;
	}
	else if (!str_cmp(arg, "manual"))
	{
		argument = one_argument(argument, arg1);
		if (arg1[0] == '\0')
		{
			send_to_char("Please enter a value for manual boot on/off" NL, ch);
			return;
		}

		if (!is_number(arg1))
		{
			send_to_char("Value for manual must be 0 (off) or 1 (on)" NL, ch);
			return;
		}

		if (atoi(arg1) < 0 || atoi(arg1) > 1)
		{
			send_to_char("Value for manual must be 0 (off) or 1 (on)" NL, ch);
			return;
		}

		set_boot_time->manual = atoi(arg1);
		ch_printf(ch, "Manual bit set to %s" NL, arg1);
		check = true;
		return;
	}

	else if (!str_cmp(arg, "default"))
	{
		set_boot_time->manual = 0;
		/* Reinitialize new_boot_time */
		new_boot_time = localtime(&current_time);
		new_boot_time->tm_mday += 5;
		if (new_boot_time->tm_hour > 12)
			new_boot_time->tm_mday += 1;
		new_boot_time->tm_hour = 6;
		new_boot_time->tm_min = 0;
		new_boot_time->tm_sec = 0;
		new_boot_time = update_time(new_boot_time);

		sysdata.DENY_NEW_PLAYERS = false;

		send_to_char("Reboot time set back to normal." NL, ch);
		check = true;
	}

	/* Typy rebootów		-- Thanos */
	else if (!str_cmp(arg, "reboot"))
	{
		sysdata.reboot_type = REB_REBOOT;
		save_sysdata(sysdata);
		send_to_char("Reboot type set to " FB_YELLOW "REBOOT" EOL, ch);
		check = true;
	}
	else if (!str_cmp(arg, "copyover"))
	{
		sysdata.reboot_type = REB_COPYOVER;
		save_sysdata(sysdata);
		send_to_char("Reboot type set to " FB_YELLOW "COPYOVER" EOL, ch);
		check = true;
	}
	else if (!str_cmp(arg, "shutdown"))
	{
		sysdata.reboot_type = REB_SHUTDOWN;
		save_sysdata(sysdata);
		send_to_char("Reboot type set to " FB_YELLOW "SHUTDOWN" EOL, ch);
		check = true;
	}
	else if (!str_cmp(arg, "after"))
	{
		struct tm *now_time;

		argument = one_argument(argument, arg);
		argument = one_argument(argument, arg1);

		if (!*arg || !*arg1
				|| (str_prefix(arg1, "minutes") && str_prefix(arg1, "hours")))
		{
			if (ch)
				send_to_char(
						"Syntax: setboot after <how_many> "
								"< " FB_WHITE "m" PLAIN "inutes | " FB_WHITE "h" PLAIN "ours >" NL,
						ch);
			return;
		}

		now_time = localtime(&current_time);

		now_time->tm_hour += !str_prefix(arg1, "hours") ? atoi(arg) : 0;
		now_time->tm_min += !str_prefix(arg1, "minutes") ? atoi(arg) : 0;

		if (set_boot_time->manual == 0)
			set_boot_time->manual = 1;

		new_boot_time = update_time(now_time);
		new_boot_struct = *new_boot_time;
		new_boot_time = &new_boot_struct;
		reboot_check(mktime(new_boot_time));

		if (ch)
			ch_printf(ch, "Ok. Setboot set to %2d:%.2d." NL, now_time->tm_hour,
					now_time->tm_min);

		check = true;
	}
	/* End of Typy rebootów */

	if (!check)
	{
		if (ch)
			send_to_char("Invalid argument for setboot." NL, ch);
		return;
	}

	else
	{
		new_boot_time_t = mktime(new_boot_time);
	}
}

void do_set_password(CHAR_DATA *ch, char *argument) //Trog
{
	CHAR_DATA *victim;
	char arg[MIL];

	argument = one_argument(argument, arg);

	if (!(victim = get_char_world(ch, arg)) || IS_NPC(victim))
	{
		send_to_char("Nie ma takiego gracza w grze." NL, ch);
		return;
	}

	if (strlen(argument) < MIN_PASS_LEN)
	{
		ch_printf(ch, "Has³o musi mieæ co najmniej %d znaków." NL,
		MIN_PASS_LEN);
		return;
	}

	STRDUP(victim->pcdata->pwd, mkpasswd(argument));
	save_pdata(victim);
	donemsg(ch);
	return;
}

/*
 * Purge a player file.  No more player.  -- Altrag
 */
DEF_DO_FUN( destro )
{
	send_to_char( FB_RED, ch);
	send_to_char("If you want to destroy a character, spell it out!" NL, ch);
	return;
}

/*
 * This could have other applications too.. move if needed. -- Altrag
 */
void close_area(AREA_DATA *pArea)
{
	CHAR_DATA *ech;
	CHAR_DATA *ech_next;
	OBJ_DATA *eobj;
	OBJ_DATA *eobj_next;
	int icnt;
	ROOM_INDEX_DATA *rid;
	ROOM_INDEX_DATA *rid_next;
	OBJ_INDEX_DATA *oid;
	OBJ_INDEX_DATA *oid_next;
	MOB_INDEX_DATA *mid;
	MOB_INDEX_DATA *mid_next;
	EXTRA_DESCR_DATA *eed;
	EXTRA_DESCR_DATA *eed_next;
	EXIT_DATA *exit;
	EXIT_DATA *exit_next;
	MPROG_ACT_LIST *mpact;
	MPROG_ACT_LIST *mpact_next;
	MPROG_DATA *mprog;
	MPROG_DATA *mprog_next;
	AFFECT_DATA *paf;
	AFFECT_DATA *paf_next;

	bug("close_area used");

	for (ech = first_char; ech; ech = ech_next)
	{
		ech_next = ech->next;

		if (ech->fighting)
			stop_fighting(ech, true);

		if (IS_NPC(ech))
		{
			/* if mob is in area, or part of area. */
			if ( URANGE(pArea->lvnum, ech->pIndexData->vnum,
					pArea->uvnum) == ech->pIndexData->vnum
					|| (ech->in_room && ech->in_room->area == pArea))
				extract_char(ech, true);
			continue;
		}

		if (ech->in_room && ech->in_room->area == pArea)
			do_recall(ech, (char*) "");
	}

	for (eobj = first_object; eobj; eobj = eobj_next)
	{
		eobj_next = eobj->next;
		/* if obj is in area, or part of area. */
		if ( URANGE(pArea->lvnum, eobj->pIndexData->vnum,
				pArea->uvnum) == eobj->pIndexData->vnum
				|| (eobj->in_room && eobj->in_room->area == pArea))
			extract_obj(eobj);
	}

	for (icnt = 0; icnt < MAX_KEY_HASH; icnt++)
	{
		for (rid = room_index_hash[icnt]; rid; rid = rid_next)
		{
			rid_next = rid->next;

			for (exit = rid->first_exit; exit; exit = exit_next)
			{
				exit_next = exit->next;
				if (rid->area == pArea || exit->to_room->area == pArea)
				{
					UNLINK(exit, rid->first_exit, rid->last_exit, next, prev);
					free_exit(exit);
				}
			}
			if (rid->area != pArea)
				continue;

			if (rid->first_person)
			{
				bug("room with people #%d", rid->vnum);
				for (ech = rid->first_person; ech; ech = ech_next)
				{
					ech_next = ech->next_in_room;
					if (ech->fighting)
						stop_fighting(ech, true);
					if (IS_NPC(ech))
						extract_char(ech, true);
					else
						do_recall(ech, (char*) "");
				}
			}
			if (rid->first_content)
			{
				bug("room with contents #%d", rid->vnum);
				for (eobj = rid->first_content; eobj; eobj = eobj_next)
				{
					eobj_next = eobj->next_content;
					extract_obj(eobj);
				}
			}
			for (eed = rid->first_extradesc; eed; eed = eed_next)
			{
				eed_next = eed->next;
				free_ed(eed);
			}
			for (mpact = rid->mpact; mpact; mpact = mpact_next)
			{
				mpact_next = mpact->next;
				free_mpact(mpact);
			}
			for (mprog = rid->mudprogs; mprog; mprog = mprog_next)
			{
				mprog_next = mprog->next;
				free_mprog(mprog);
			}
			if (rid == room_index_hash[icnt])
				room_index_hash[icnt] = rid->next;
			else
			{
				ROOM_INDEX_DATA *trid;

				for (trid = room_index_hash[icnt]; trid; trid = trid->next)
					if (trid->next == rid)
						break;
				if (!trid)
					bug("rid not in hash list %d", rid->vnum);
				else
					trid->next = rid->next;
			}
			free_room(rid);
		}

		for (mid = mob_index_hash[icnt]; mid; mid = mid_next)
		{
			mid_next = mid->next;

			if (mid->vnum < pArea->lvnum || mid->vnum > pArea->uvnum)
				continue;

			if (mid->pShop)
			{
				UNLINK(mid->pShop, first_shop, last_shop, next, prev);
				DISPOSE(mid->pShop);
			}
			if (mid->rShop)
			{
				UNLINK(mid->rShop, first_repair, last_repair, next, prev);
				DISPOSE(mid->rShop);
			}
			for (mprog = mid->mudprogs; mprog; mprog = mprog_next)
			{
				mprog_next = mprog->next;
				free_mprog(mprog);
			}
			if (mid == mob_index_hash[icnt])
				mob_index_hash[icnt] = mid->next;
			else
			{
				MOB_INDEX_DATA *tmid;

				for (tmid = mob_index_hash[icnt]; tmid; tmid = tmid->next)
					if (tmid->next == mid)
						break;
				if (!tmid)
					bug("mid not in hash list %s", mid->vnum);
				else
					tmid->next = mid->next;
			}
			free_mid(mid);
		}

		for (oid = obj_index_hash[icnt]; oid; oid = oid_next)
		{
			oid_next = oid->next;

			if (oid->vnum < pArea->lvnum || oid->vnum > pArea->uvnum)
				continue;

			for (eed = oid->first_extradesc; eed; eed = eed_next)
			{
				eed_next = eed->next;
				free_ed(eed);
			}
			for (paf = oid->first_affect; paf; paf = paf_next)
			{
				paf_next = paf->next;
				DISPOSE(paf);
			}
			for (mprog = oid->mudprogs; mprog; mprog = mprog_next)
			{
				mprog_next = mprog->next;
				free_mprog(mprog);
			}
			if (oid == obj_index_hash[icnt])
				obj_index_hash[icnt] = oid->next;
			else
			{
				OBJ_INDEX_DATA *toid;

				for (toid = obj_index_hash[icnt]; toid; toid = toid->next)
					if (toid->next == oid)
						break;
				if (!toid)
					bug("oid not in hash list %s", oid->vnum);
				else
					toid->next = oid->next;
			}
			free_oid(oid);
		}
	}
	UNLINK(pArea, first_build, last_build, next, prev);
	UNLINK(pArea, first_asort, last_asort, next_sort, prev_sort);
	free_area(pArea);
}

DEF_DO_FUN( destroy )
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	char arg[MIL];

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Destroy what player file?" NL, ch);
		return;
	}

	for (victim = first_char; victim; victim = victim->next)
		if (!IS_NPC(victim) && !str_cmp(victim->name, arg))
			break;
	if (!victim)
	{
		DESCRIPTOR_DATA *d;

		/* Make sure they aren't halfway logged in. */
		for (d = first_descriptor; d; d = d->next)
			if ((victim = d->character) && !IS_NPC(victim)
					&& !str_cmp(victim->name, arg))
				break;
		if (d)
			close_socket(d, true);
	}
	else
	{
		int x, y;

		quitting_char = victim;
		save_char_obj(victim);
		saving_char = NULL;
		extract_char(victim, true);
		for (x = 0; x < MAX_WEAR; x++)
			for (y = 0; y < MAX_LAYERS; y++)
				save_equipment[x][y] = NULL;
	}

	sprintf(buf, "%s%s", PDATA_DIR, capitalize(arg));
	if (!(fopen(buf, "r")))
	{
		send_to_char( PLAIN, ch);
		send_to_char("Player file does not exist." NL, ch);
		return;
	}
	destroy_char(arg);
	return;
}

/* Super-AT command:

 FOR ALL <action>
 FOR MORTALS <action>
 FOR GODS <action>
 FOR MOBS <action>
 FOR EVERYWHERE <action>


 Executes action several times, either on ALL players (not including yourself),
 MORTALS (including trusted characters), GODS (characters with level higher than
 L_HERO), MOBS (Not recommended) or every room (not recommended either!)

 If you insert a # in the action, it will be replaced by the name of the target.

 If # is a part of the action, the action will be executed for every target
 in game. If there is no #, the action will be executed for every room containg
 at least one target, but only once per room. # cannot be used with FOR EVERY-
 WHERE. # can be anywhere in the action.

 Example:

 FOR ALL SMILE -> you will only smile once in a room with 2 players.
 FOR ALL TWIDDLE # -> In a room with A and B, you will twiddle A then B.

 Destroying the characters this command acts upon MAY cause it to fail. Try to
 avoid something like FOR MOBS PURGE (although it actually works at my MUD).

 FOR MOBS TRANS 3054 (transfer ALL the mobs to Midgaard temple) does NOT work
 though :)

 The command works by transporting the character to each of the rooms with
 target in them. Private rooms are not violated.

 */

/* Expand the name of a character into a string that identifies THAT
 character within a room. E.g. the second 'guard' -> 2. guard
 */
const SWString name_expand(CHAR_DATA *ch)
{
	int count = 1;
	CHAR_DATA *rch;
	char name[MIL] = {0}; /*  HOPEFULLY no mob has a name longer than THAT */

	char outbuf[MIL] = {0};

	if (!IS_NPC(ch))
		return ch->name;

	one_argument(ch->name, name); /* copy the first word into name */

	if (!name[0]) /* weird mob .. no keywords */
	{
		strcpy(outbuf, ""); /* Do not return NULL, just an empty buffer */
		return outbuf;
	}

	/* ->people changed to ->first_person -- TRI */
	for (rch = ch->in_room->first_person; rch && (rch != ch);
			rch = rch->next_in_room)
		if (is_name(name, rch->name))
			count++;
	swsnprintf(outbuf, MIL, "%d.%s", count, name);
	return SWString(outbuf);
}

/*changed by Thanos:
 zmieni³em tê funkcjê tak, by dzia³a³a fragmentami, co pulse_for (ok. 1s),a
 nie naraz tak jak by³o dotychczas. Teraz funkcja pulsuje sobie w tle nie
 laguj±c a gracz który wykona³ tê komendê ma wolny prompt.
 patrz update_for() w update.c
 */
DEF_DO_FUN( for )
{
	char range[MIL];
	ROOM_INDEX_DATA *fRoom = 0;
	CHAR_DATA *fVictim = 0;
	bool found = false;
	bool fMortals = false;
	bool fGods = false;
	bool fMobs = false;
	bool fEverywhere = false;
	bool fAreaMobs = false;
	bool fAreaRooms = false;
	int i;

	if (IS_NPC(ch)) /*bez zbêdnego ryzyka*/
	{
		huh(ch);
		return;
	}

	/* zatrzymujemy pêtlê */
	if (argument[0] && !str_cmp(argument, "stop"))
	{
		if (for_loop.fStopped)
			send_to_char("For Loop is not currently running!" NL, ch);
		else
		{
			ch_printf(for_loop.fOwner,
					"For Loop Stopped by %s. Made %d iteration%s." NL,
					ch->przypadki[0], for_loop.fCount,
					for_loop.fCount == 1 ? "" : "s");

			if (ch != for_loop.fOwner)
				send_to_char("Ok." NL, ch);

			clean_for_loop();
		}
		return;
	}

	/* je¶li kto¶ zainteresowany chce sprawdziæ co tak zapierdala :) */
	if (argument[0] && !str_cmp(argument, "status"))
	{
		if (for_loop.fStopped)
			send_to_char("For Loop is not currently running!" NL, ch);
		else
		{
			ch_printf(ch, "&WFor Status:&w" NL);
			ch_printf(ch, "Owner:     %s" NL, for_loop.fOwner->przypadki[0]);
			ch_printf(ch, "Command:   %s" NL, for_loop.command);
			ch_printf(ch, "Iteration: %d" NL, for_loop.fCount);
			ch_printf(ch, "Type:      %s" NL,
					(for_loop.fMortals && for_loop.fGods) ? "All" :
					for_loop.fMortals ? "Mortals" : for_loop.fGods ? "Gods" :
					for_loop.fMobs ? "Mobs" :
					for_loop.fEverywhere ? "Everywhere" :
					for_loop.fAreaMobs ? "AreaMobs" :
					for_loop.fAreaMobs ? "AreaRooms" : "????");

			if (for_loop.fEverywhere || for_loop.fAreaRooms)
				ch_printf(ch, "Next room: %s {%d}" NL,
						get_room_index(for_loop.fRoom) ?
								get_room_index(for_loop.fRoom)->name :
								"no_room", for_loop.fRoom);
			else
				ch_printf(ch, "Next char: %s &z&w%s {%d}" NL,
						for_loop.fVictim->przypadki[0],
						IS_NPC( for_loop.fVictim ) ? "(NPC)" : "(PC)",
						IS_NPC( for_loop.fVictim ) ?
								for_loop.fVictim->pIndexData->vnum : 0);
		}
		return;
	}

	argument = one_argument(argument, range);

	if (!range[0] || !argument[0])
	{
		do_help(ch, (char*) "for");
		return;
	}

	if (!for_loop.fStopped)
	{
		send_to_char(
				"Sorry, For Loop is currently running. Stop it if you want." NL,
				ch);
		send_to_char(
				"Use 'for stop' command, but check who is using it first.('for status')." NL,
				ch);
		return;
	}

	if (!str_prefix("quit", argument) || is_name("for", argument)
			|| is_name("rat", argument))
	{
		send_to_char("Are you trying to crash the MUD or something?" NL, ch);
		return;
	}

	if (!str_cmp(range, "all"))
	{
		fMortals = true;
		fGods = true;
	}
	else if (!str_cmp(range, "gods"))
		fGods = true;
	else if (!str_cmp(range, "mortals"))
		fMortals = true;
	else if (!str_cmp(range, "mobs"))
		fMobs = true;
	else if (!str_cmp(range, "everywhere"))
		fEverywhere = true;
	else if (!str_cmp(range, "areamobs"))
		fAreaMobs = true;
	else if (!str_cmp(range, "arearooms"))
		fAreaRooms = true;
	else
	{
		do_help(ch, (char*) "for");
		return;
	}

	/* znajd¼my pierwsz± ofiarê/pokój */
	if (fMobs || fMortals || fGods)
	{
		for (fVictim = last_char; fVictim; fVictim = fVictim->prev)
		{
			if (fVictim == ch)
				continue;

			if ((IS_NPC(fVictim)) && (fMobs == true))
			{
				found = true;
				break;
			}
			else if ((!IS_NPC(fVictim))
					&& (get_trust(fVictim) >= LEVEL_IMMORTAL)
					&& (fGods == true))
			{
				found = true;
				break;
			}
			else if ((!IS_NPC(fVictim)) && (get_trust(fVictim) < LEVEL_IMMORTAL)
					&& (fMortals == true))
			{
				found = true;
				break;
			}

		}
	}
//Trog: od tad do komentarza powyzej jest to syfiaste i zle dziala, poprawie niedlugo.
	else if (fAreaMobs)
	{
		for (fVictim = first_char; fVictim; fVictim = fVictim->next)
		{
			if (fVictim && IS_NPC(fVictim) && fVictim->in_room
					&& fVictim->in_room->area && ch->in_room
					&& ch->in_room->area
					&& fVictim->in_room->area == ch->in_room->area)
			{
				found = true;
				break;
			}
		}
	}
	else if (fEverywhere)
	{
		for (i = 0; i <= MAX_VNUM; i++)
		{
			fRoom = get_room_index(i);
			if (fRoom)
			{
				found = true;
				break;
			}
		}
	}
	else if (fAreaRooms)
	{
		for (i = 0; i <= MAX_VNUM; i++)
		{
			fRoom = get_room_index(i);
			if (fRoom && ch->in_room && ch->in_room->area
					&& ch->in_room->area == fRoom->area)
			{
				found = true;
				break;
			}
		}
	}
	else
	{
		send_to_char("Something's wrong. Aborting. Sorry." NL, ch);
		return;
	}

	if (!found)
	{
		send_to_char("Couldn't find a target. Aborting." NL, ch);
		return;
	}

	if (((fMobs || fMortals || fGods || fAreaMobs) && !fVictim)
			|| ((fEverywhere || fAreaRooms) && !fRoom)
			|| (!(ch->in_room && ch->in_room->area)))
	{
		send_to_char("Couldn't find nessesary values. Sorry." NL, ch);
		return;
	}

	/* czy¶cimy star± pêtlê (je¶li jest) tylko w przypadku, gdy mamy ju¿
	 wszystkie potrzebne nam warto¶ci */
	clean_for_loop();
	/* kopiujemy warto¶ci */
	/*ustalamy zakres pêtli*/
	for_loop.fGods = fGods;
	for_loop.fMortals = fMortals;
	for_loop.fMobs = fMobs;
	for_loop.fEverywhere = fEverywhere;
	for_loop.fAreaMobs = fAreaMobs;
	for_loop.fAreaRooms = fAreaRooms;
	for_loop.fArea = ch->in_room->area;
	/* potencjalny cel ;) */
	if ((fAreaRooms || fEverywhere) && fRoom)
		for_loop.fRoom = fRoom->vnum;
	if ((fMobs || fMortals || fGods || fAreaMobs) && fVictim)
		for_loop.fVictim = fVictim;
	/* ustalamy komendê */
	STRDUP(for_loop.command, argument);
	/*w³a¶ciciela*/
	for_loop.fOwner = ch;
	/* puszczamy pêtlê */
	for_loop.fStopped = false;
	send_to_char("Ok. For Loop started." NL, ch);
	return;
}
/*done by Thanos*/

DEF_DO_FUN( cset )
{
	char arg[MAX_STRING_LENGTH];
	int level;

	if (argument[0] == '\0')
	{

		ch_printf(ch, FB_YELLOW "Channels:" EOL
		"  Muse: %d. Comm: %d. Log: %d. Build: %d. Prog: %d." NL,
				sysdata.muse_level, sysdata.comm_level, sysdata.log_level,
				sysdata.build_level, sysdata.prog_level);

		ch_printf(ch,
				FB_YELLOW "Building:" EOL
				"  Prototype modification:         %3d.  Player msetting:             %d." NL,
				sysdata.level_modify_proto, sysdata.level_mset_player);

		ch_printf(ch,
				FB_YELLOW "Other:" EOL
				"  Force on players:               %3d. Private room override:        %d." NL
				"  Penalty to regular stun chance: %3d. Penalty to stun plr vs. plr:  %d." NL
				"  Percent damage plr vs. plr:     %3d. Percent damage plr vs. mob:   %d." NL
				"  Percent damage mob vs. plr:     %3d. Percent damage mob vs. mob:   %d." NL
				"  Get object without take flag:   %3d. Autosave frequency (minutes): %d." NL
				"  MaxClanId:                      %3d." NL
				"  Save flags: %s" NL, sysdata.level_forcepc,
				sysdata.level_override_private, sysdata.stun_regular,
				sysdata.stun_plr_vs_plr, sysdata.dam_plr_vs_plr,
				sysdata.dam_plr_vs_mob, sysdata.dam_mob_vs_plr,
				sysdata.dam_mob_vs_mob, sysdata.level_getobjnotake,
				sysdata.save_frequency, sysdata.max_clan_id,
				flag_string(save_flags_list, sysdata.save_flags));

		/*Added by Thanos*/
		ch_printf(ch, FB_YELLOW "Game:" EOL
		"  Admins:        %s." NL
		"  Coders:        %s." NL
		"  Council:       %s." NL
		"  Helpmasters:   %s." NL, sysdata.admins, sysdata.coders,
				sysdata.council, sysdata.helpmasters);

		/*Added by Thanos*/
		ch_printf(ch, FB_YELLOW "System:" EOL
		"  Last boot was:                                  %s." NL
		"  Log All Players (including Imms) to LOGALL.LOG: %s." NL
		"  Log last command to LASTCMD.LOG:                %s." NL
		"  CrashLog set to:                                %s." NL
		"  Silent_booting:                                 %s." NL
		"  To_iso conversion files while reading:          %s." NL,
				sysdata.tmpdowntype == DOWN_REBOOT ? "Reboot" :
				sysdata.tmpdowntype == DOWN_SHUTDOWN ? "Shutdown" :
				sysdata.tmpdowntype == DOWN_COPYOVER ? "Copyover" :
				sysdata.tmpdowntype == DOWN_KILLED ? "Killed" :
				sysdata.tmpdowntype == DOWN_CRASHED ?
				FB_RED "Crashed" PLAIN :
														"*>HGW!<*",
				sysdata.log_all ? "Yes" : "No",
				sysdata.lastcmd_log ? "Yes" : "No",
				sysdata.crashlog ? "Yes" : "No", sysdata.silent ? "Yes" : "No",
				sysdata.to_iso ? "Yes" : "No");

		ch_printf(ch, "See also 'cset denies'." NL);
		return;
	}

	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "help"))
	{
		do_help(ch, (char*) "controls");
		return;
	}

	/*Added by Thanos - LogAll - do sprawdzania na czym pada mudzik */
	if (!str_cmp(arg, "logall"))
	{
		char buf[MAX_STRING_LENGTH];

		if (sysdata.log_all)
			sysdata.log_all = false;
		else
			sysdata.log_all = true;
		sprintf(buf, "*** %s LOG: --------------------- %s",
				sysdata.log_all ? "STARTING" : "STOPPING",
				ctime(&current_time));
		if (sysdata.log_all)
			buf[strlen(buf) - 1] = '\0';
		append_file(ch, LOGALL_FILE, buf);
		do_cset(ch, (char*) "save");
		do_cset(ch, (char*) "");
		return;
	}
	/* done */

	/*Added by Thanos: czy mud ma pamietac ostatnio wykonywana komende */
	if (!str_cmp(arg, "lastcmd"))
	{

		if (sysdata.lastcmd_log)
			sysdata.lastcmd_log = false;
		else
			sysdata.lastcmd_log = true;
		do_cset(ch, (char*) "save");
		do_cset(ch, (char*) "");
		return;
	}
	/* done */

	/*Added by Thanos: czy mud ma konwertowac krainy na iso przy odczycie */
	if (!str_cmp(arg, "to_iso"))
	{

		if (sysdata.to_iso)
			sysdata.to_iso = false;
		else
			sysdata.to_iso = true;
		do_cset(ch, (char*) "save");
		do_cset(ch, (char*) "");
		return;
	}
	/* done */

	/*Added by Thanos: czy ma reagowac na pada */
	if (!str_cmp(arg, "crashlog"))
	{
		if (sysdata.crashlog)
		{
			sysdata.crashlog = false;
			signal(SIGSEGV, SIG_DFL);
		}
		else
		{
			sysdata.crashlog = true;
			signal(SIGSEGV, (void (*)(int)) deal_with_crash);
		}
		ch_printf(ch, "Ok. Crash reaction %s." NL,
				sysdata.crashlog ? "ON" : "OFF");
		return;
	}

	/*Added by Thanos: czy mud ma logowaæ np. resety krain */
	if (!str_cmp(arg, "silent") || !str_cmp(arg, "silent_booting"))
	{
		if (sysdata.silent)
			sysdata.silent = false;
		else
			sysdata.silent = true;

		ch_printf(ch, "Ok. Mud will %s log the unnecessary messages." NL,
				sysdata.silent ? "NOT" : "now");
		return;
	}

	if (!str_cmp(arg, "save"))
	{
		save_sysdata(sysdata);
		return;
	}

	if (!str_cmp(arg, "admins") && (IS_ADMIN( ch->name ) || !*sysdata.admins))
	{
		char arg2[MIL];

		argument = one_argument(argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char("Syntax: cset admins <name>" NL, ch);
			return;
		}

		toggle_string(&sysdata.admins, arg2);

		if (is_name(arg2, sysdata.admins))
			send_to_char("Admin added." NL, ch);
		else
			send_to_char("Admin removed." NL, ch);
		return;
	}

	if (!str_cmp(arg, "coders") && IS_ADMIN(ch->name))
	{
		char arg2[MIL];

		argument = one_argument(argument, arg2);

		if (arg2[0] == '\0')
		{
			send_to_char("Syntax: cset coders <name>" NL, ch);
			return;
		}

		toggle_string(&sysdata.coders, arg2);

		if (is_name(arg2, sysdata.coders))
			send_to_char("Coder added." NL, ch);
		else
			send_to_char("Coder removed." NL, ch);

		return;
	}

	if (!str_cmp(arg, "council") && IS_ADMIN(ch->name))
	{
		char arg2[MIL];

		argument = one_argument(argument, arg2);

		if (arg2[0] == '\0')
		{
			send_to_char("Syntax: cset council <name>" NL, ch);
			return;
		}

		toggle_string(&sysdata.council, arg2);

		if (is_name(arg2, sysdata.council))
			send_to_char("Council member added." NL, ch);
		else
			send_to_char("Council member removed." NL, ch);
		return;
	}

	if (!str_cmp(arg, "helpmasters") && IS_ADMIN(ch->name))
	{
		char arg2[MIL];

		argument = one_argument(argument, arg2);

		if (arg2[0] == '\0')
		{
			send_to_char("Syntax: cset helpmasters <name>" NL, ch);
			return;
		}

		toggle_string(&sysdata.helpmasters, arg2);

		if (is_name(arg2, sysdata.helpmasters))
			send_to_char("Helpmaster added." NL, ch);
		else
			send_to_char("Helpmaster removed." NL, ch);

		return;
	}

	if (!str_cmp(arg, "denies") || !str_cmp(arg, "deny")
			|| !str_cmp(arg, "denied"))
	{
		char arg2[MIL];

		argument = one_argument(argument, arg2);

		if (!*arg2)
		{
			pager_printf(ch, "List of denied nicks:" NL "%s" EOL NL,
					sysdata.denied_nicks);
			pager_printf(ch, "To add/remove nick type: cset denies <nick>" NL);
			return;
		}

		toggle_string(&sysdata.denied_nicks, arg2);

		if (is_name(arg2, sysdata.denied_nicks))
			send_to_char("Denied nick added." NL, ch);
		else
			send_to_char("Denied nick removed." NL, ch);

		return;
	}

	if (!str_cmp(arg, "saveflag"))
	{
		int x = get_saveflag(argument);

		if (x == -1)
			send_to_char("Not a save flag." NL, ch);
		else
		{
			TOGGLE_BIT(sysdata.save_flags, 1 << x);
			send_to_char("Ok." NL, ch);
		}
		return;
	}

	level = (int) atoi(argument);

	if (!str_prefix(arg, "savefrequency"))
	{
		sysdata.save_frequency = level;
		send_to_char("Ok." NL, ch);
		return;
	}

	if (!str_cmp(arg, "stun"))
	{
		sysdata.stun_regular = level;
		send_to_char("Ok." NL, ch);
		return;
	}

	if (!str_cmp(arg, "stun_pvp"))
	{
		sysdata.stun_plr_vs_plr = level;
		send_to_char("Ok." NL, ch);
		return;
	}

	if (!str_cmp(arg, "dam_pvp"))
	{
		sysdata.dam_plr_vs_plr = level;
		send_to_char("Ok." NL, ch);
		return;
	}

	if (!str_cmp(arg, "get_notake"))
	{
		sysdata.level_getobjnotake = level;
		send_to_char("Ok." NL, ch);
		return;
	}

	if (!str_cmp(arg, "dam_pvm"))
	{
		sysdata.dam_plr_vs_mob = level;
		send_to_char("Ok." NL, ch);
		return;
	}

	if (!str_cmp(arg, "dam_mvp"))
	{
		sysdata.dam_mob_vs_plr = level;
		send_to_char("Ok." NL, ch);
		return;
	}

	if (!str_cmp(arg, "dam_mvm"))
	{
		sysdata.dam_mob_vs_mob = level;
		send_to_char("Ok." NL, ch);
		return;
	}

	if (level < 0 || level > MAX_LEVEL)
	{
		send_to_char("Invalid value for new control." NL, ch);
		return;
	}

	if (!str_cmp(arg, "muse"))
	{
		sysdata.muse_level = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	if (!str_cmp(arg, "prog"))
	{
		sysdata.prog_level = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	if (!str_cmp(arg, "comm"))
	{
		sysdata.comm_level = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	if (!str_cmp(arg, "log"))
	{
		sysdata.log_level = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	if (!str_cmp(arg, "build"))
	{
		sysdata.build_level = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	if (!str_cmp(arg, "proto_modify"))
	{
		sysdata.level_modify_proto = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	if (!str_cmp(arg, "override_private"))
	{
		sysdata.level_override_private = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	if (!str_cmp(arg, "forcepc"))
	{
		sysdata.level_forcepc = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	if (!str_cmp(arg, "mset_player"))
	{
		sysdata.level_mset_player = level;
		send_to_char("Ok." NL, ch);
		return;
	}
	else
	{
		send_to_char("Invalid argument." NL, ch);
		return;
	}
}

DEF_DO_FUN( isolate )
{
	CHAR_DATA *victim;
	char arg[MIL];
	int time;
	bool h_d = false;
	struct tm *tms;

	argument = one_argument(argument, arg);

	if (!*arg)
	{
		send_to_char("Syntax: isolate <player> <number> days | hours" NL, ch);
		return;
	}

	if (!(victim = get_char_world(ch, arg)) || IS_NPC(victim))
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_IMMORTAL(victim))
	{
		send_to_char("Immortals cannot be isolated." NL, ch);
		return;
	}

	if (IS_ISOLATED(victim))
	{
		ch_printf(ch, "They are already isolated until %24.24s, by %s." NL,
				ctime(&victim->pcdata->release_date),
				victim->pcdata->isolated_by);
		return;
	}

	argument = one_argument(argument, arg);
	if (!*arg || !is_number(arg))
	{
		send_to_char("Isolate for how long?" NL, ch);
		return;
	}

	if ((time = atoi(arg)) <= 0)
	{
		send_to_char("Future time please. :-)" NL, ch);
		return;
	}

	argument = one_argument(argument, arg);
	if (!*arg || !str_prefix(arg, "hours"))
		h_d = true;

	else if (str_prefix(arg, "days"))
	{
		send_to_char("Values: hours, days." NL, ch);
		return;
	}
	else if (time > 30)
	{
		send_to_char("Maximum isolate time: 30 days." NL, ch);
		return;
	}

	tms = localtime(&current_time);
	if (h_d)
		tms->tm_hour += time;
	else
		tms->tm_mday += time;

	victim->pcdata->release_date = mktime(tms);
	STRDUP(victim->pcdata->isolated_by, ch->name);
	ch_printf(ch, "%s will be unisolted at %24.24s." NL, victim->name,
			ctime(&victim->pcdata->release_date));
	act(COL_FORCE, "$n znika zabran$y za spraw± mocy.", victim, NULL, ch,
	TO_NOTVICT);
	char_from_room(victim);
	char_to_room(victim, get_room_index(6));
	act(COL_FORCE, "$n przylatuje tu wiezion$y przez moc.", victim, NULL, ch,
	TO_NOTVICT);
	do_look(victim, (char*) "auto");
	ch_printf(victim,
			"Mistrzowie Jedi nie s± zadowoleni z twojego postêpowania." NL
			"Zostaniesz w izolatce przez %d %s." NL, time,
			h_d ? (time == 1 ? "godzinê" : "godzin") : (
					time == 1 ? "dzieñ" : "dni"));

	save_char_obj(victim);
	return;
}

DEF_DO_FUN( unisolate )
{
	CHAR_DATA *victim;
	char arg[MIL];
	ROOM_INDEX_DATA *location;

	argument = one_argument(argument, arg);
	if (!*arg)
	{
		send_to_char("Syntax: unisolate <player>" NL, ch);
		return;
	}

	location = ch->in_room;
	ch->in_room = get_room_index(6);
	victim = get_char_room(ch, arg);
	ch->in_room = location; /* The case of unisolate self, etc. */
	if (!victim || IS_NPC(victim) || victim->in_room->vnum != 6)
	{
		send_to_char("No such player isolated." NL, ch);
		return;
	}

	location = get_room_index(wherehome(victim));

	if (!location)
		location = ch->in_room;

	MOBtrigger = false;
	act( COL_FORCE, "Moc zabiera st±d $n$3.", victim, NULL, ch, TO_NOTVICT);
	char_from_room(victim);
	char_to_room(victim, location);
	send_to_char(
			"Mistrzowie Jedi przebaczaj± Ci! Wyjdziesz z izolatki wcze¶niej." NL,
			victim);
	do_look(victim, (char*) "auto");
	send_to_char("Player unisolated." NL, ch);

	if (!*victim->pcdata->isolated_by)
	{
		if (str_cmp(ch->name, victim->pcdata->isolated_by))
			ch_printf(ch,
					"(You should probably write an explanation note to %s.)" NL,
					victim->pcdata->isolated_by);
		STRDUP(victim->pcdata->isolated_by, "");
	}

	MOBtrigger = false;
	act( COL_FORCE, "Moc zabiera st±d $n$3.", victim, NULL, ch, TO_NOTVICT);
	victim->pcdata->release_date = 0;
	save_char_obj(victim);

	return;
}

/* Trog - Copy & Paste method :) */
DEF_DO_FUN( silence )
{
	CHAR_DATA *victim;
	char arg[MIL];
	int time;
	bool h_d = false;
	struct tm *tms;

	argument = one_argument(argument, arg);

	if (!*arg)
	{
		send_to_char("Syntax: silence <player> <number> days | hours" NL, ch);
		return;
	}

	if (!(victim = get_char_world(ch, arg)) || IS_NPC(victim))
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_IMMORTAL(victim))
	{
		send_to_char("Immortals cannot be silenced." NL, ch);
		return;
	}

	if (IS_SILENCED(victim))
	{
		ch_printf(ch, "They are already silenced until %24.24s, by %s." NL,
				ctime(&victim->pcdata->unsilence_date),
				victim->pcdata->silenced_by);
		return;
	}

	argument = one_argument(argument, arg);
	if (!*arg || !is_number(arg))
	{
		send_to_char("Silence for how long?" NL, ch);
		return;
	}

	if ((time = atoi(arg)) <= 0)
	{
		send_to_char("Future time please. :-)" NL, ch);
		return;
	}

	argument = one_argument(argument, arg);
	if (!*arg || !str_prefix(arg, "hours"))
		h_d = true;

	else if (str_prefix(arg, "days"))
	{
		send_to_char("Values: hours, days." NL, ch);
		return;
	}
	else if (!h_d && time > 14)
	{
		send_to_char("Maximum silence time: 14 days." NL, ch);
		return;
	}

	tms = localtime(&current_time);
	if (h_d)
		tms->tm_hour += time;
	else
		tms->tm_mday += time;

	victim->pcdata->unsilence_date = mktime(tms);
	STRDUP(victim->pcdata->silenced_by, ch->name);
	ch_printf(ch, "%s will be unsilenced at %24.24s." NL, victim->name,
			ctime(&victim->pcdata->unsilence_date));
	ch_printf(victim,
			"Mistrzowie Jedi nie s± zadowoleni z twoich wypowiedzi." NL
			"Tracisz prawo g³osu na %d %s." NL, time,
			h_d ? (time == 1 ? "godzinê" : "godzin") : (
					time == 1 ? "dzieñ" : "dni"));

	save_char_obj(victim);
	return;
}

DEF_DO_FUN( unsilence )
{
	CHAR_DATA *victim;
	char arg[MIL];

	argument = one_argument(argument, arg);
	if (!*arg)
	{
		send_to_char("Syntax: unsilence <player>" NL, ch);
		return;
	}

	if (!(victim = get_char_world(ch, arg)) || IS_NPC(victim))
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (!IS_SILENCED(victim))
	{
		send_to_char("That player is not silenced." NL, ch);
		return;
	}

	send_to_char("Mistrzowie Jedi przebaczaj± Ci! Pozwalaj± Ci przemówiæ." NL,
			victim);
	send_to_char("Player unsilenced." NL, ch);

	if (!*victim->pcdata->silenced_by)
	{
		if (str_cmp(ch->name, victim->pcdata->silenced_by))
			ch_printf(ch,
					"(You should probably write an explanation note to %s.)" NL,
					victim->pcdata->silenced_by);
		STRDUP(victim->pcdata->silenced_by, "");
	}

	victim->pcdata->unsilence_date = 0;
	save_char_obj(victim);
	return;
}

/* Vnum search command by Swordbearer */
DEF_DO_FUN( vsearch )
{
	char arg[MIL];
	bool found = false;
	OBJ_DATA *obj;
	OBJ_DATA *in_obj;
	int obj_counter = 1;
	int argi;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Syntax:  vsearch <vnum>." NL, ch);
		return;
	}

	argi = atoi(arg);
	if (argi < 0 && argi > MAX_VNUM)
	{
		send_to_char("Vnum out of range." NL, ch);
		return;
	}
	for (obj = first_object; obj != NULL; obj = obj->next)
	{
		if (!can_see_obj(ch, obj) || !(argi == obj->pIndexData->vnum))
			continue;

		found = true;
		for (in_obj = obj; in_obj->in_obj != NULL; in_obj = in_obj->in_obj)
			;

		if (in_obj->carried_by != NULL)
			pager_printf(ch, "[%2d] Level %d %s carried by %s." NL, obj_counter,
					obj->level, obj_short(obj),
					PERS(in_obj->carried_by, ch, 0));
		else
			pager_printf(ch, "[%2d] [%-5d] %s in %s." NL, obj_counter,
					((in_obj->in_room) ? in_obj->in_room->vnum : 0),
					obj_short(obj),
					(in_obj->in_room == NULL) ?
							"somewhere" : in_obj->in_room->name);

		obj_counter++;
	}

	if (!found)
		send_to_char("Nothing like that in hell, earth, or heaven." NL, ch);

	return;
}

/*
 * Simple function to let any imm make any player instantly sober.
 * Saw no need for level restrictions on this.
 * Written by Narn, Apr/96
 */
DEF_DO_FUN( sober )
{
	CHAR_DATA *victim;
	char arg1[MIL];

	smash_tilde(argument);
	argument = one_argument(argument, arg1);
	if ((victim = get_char_room(ch, arg1)) == NULL)
	{
		send_to_char("They aren't here." NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Not on mobs." NL, ch);
		return;
	}

	if (victim->pcdata)
		victim->pcdata->condition[COND_DRUNK] = 0;
	send_to_char("Ok." NL, ch);
	send_to_char("Wraca ci trze¼we spojrzenie na ¶wiat." NL, victim);
	return;
}

/*
 * Remove a social from it's hash index				-Thoric
 */
void unlink_social(SOCIALTYPE *social)
{
	SOCIALTYPE *tmp, *tmp_next;
	int hash;

	IF_BUG(social == NULL, "")
		return;

	if (social->name[0] < 'a' || social->name[0] > 'z')
		hash = 0;
	else
		hash = (social->name[0] - 'a') + 1;

	if (social == (tmp = social_index[hash]))
	{
		social_index[hash] = tmp->next;
		return;
	}
	for (; tmp; tmp = tmp_next)
	{
		tmp_next = tmp->next;
		if (social == tmp_next)
		{
			tmp->next = tmp_next->next;
			return;
		}
	}
}

/*
 * Add a social to the social index table			-Thoric
 * Hashed and insert sorted
 */
void add_social(SOCIALTYPE *social)
{
	int hash, x;
	SOCIALTYPE *tmp, *prev;

	IF_BUG(social == NULL, "")
		return;

	IF_BUG(!social->name, "")
		return;

	IF_BUG(!social->char_no_arg, "")
		return;

	/* make sure the name is all lowercase */
	for (x = 0; social->name[x] != '\0'; x++)
		social->name[x] = LOWER(social->name[x]);

	if (social->name[0] < 'a' || social->name[0] > 'z')
//Added by Ratm
		switch (social->name[0])
		{
		case '±':
			hash = 1;
			break;
		case 'æ':
			hash = 3;
			break;
		case 'ê':
			hash = ('e' - 'a') + 1;
			break;
		case '³':
			hash = ('l' - 'a') + 1;
			break;
		case 'ñ':
			hash = ('n' - 'a') + 1;
			break;
		case 'ó':
			hash = ('o' - 'a') + 1;
			break;
		case '¶':
			hash = ('s' - 'a') + 1;
			break;
		case '¿':
			hash = ('z' - 'a') + 1;
			break;
		case '¼':
			hash = ('z' - 'a') + 1;
			break;
		default:
			hash = 0;
		}
	else
		hash = (social->name[0] - 'a') + 1;

	if ((prev = tmp = social_index[hash]) == NULL)
	{
		social->next = social_index[hash];
		social_index[hash] = social;
		return;
	}

	for (; tmp; tmp = tmp->next)
	{
		if ((x = strcmp(social->name, tmp->name)) == 0)
		{
			bug("trying to add duplicate name to bucket %d", hash);
			free_social(social);
			return;
		}
		else if (x < 0)
		{
			if (tmp == social_index[hash])
			{
				social->next = social_index[hash];
				social_index[hash] = social;
				return;
			}
			prev->next = social;
			social->next = tmp;
			return;
		}
		prev = tmp;
	}

	/* add to end */
	prev->next = social;
	social->next = NULL;
	return;
}

/*
 * Social editor/displayer/save/delete				-Thoric
 */
DEF_DO_FUN( sedit )
{
	SOCIALTYPE *social;
	char arg1[MIL] = {0};
	char arg2[MIL] = {0};

	smash_tilde(argument);
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Syntax: sedit <social> [field]" NL, ch);
		send_to_char("Syntax: sedit <social> create" NL, ch);
		if (get_trust(ch) > LEVEL_GOD)
			send_to_char("Syntax: sedit <social> delete" NL, ch);
		if (get_trust(ch) > LEVEL_LESSER)
			send_to_char("Syntax: sedit <save>" NL, ch);
		send_to_char("\n\rField being one of:" NL, ch);
		send_to_char("  cnoarg onoarg cfound ofound vfound cauto oauto" NL, ch);
		return;
	}

	if (get_trust(ch) > LEVEL_LESSER && !str_cmp(arg1, "save"))
	{
		save_socials();
		send_to_char("Saved." NL, ch);
		return;
	}

	social = find_social(arg1);

	if (!str_cmp(arg2, "create"))
	{
		if (social)
		{
			send_to_char("That social already exists!" NL, ch);
			return;
		}
		CREATE(social, SOCIALTYPE, 1);
		STRDUP(social->name, arg1);
		swsnprintf(arg2, MIL, "You %s.", arg1);
		STRDUP(social->char_no_arg, arg2);
		add_social(social);
		send_to_char("Social added." NL, ch);
		return;
	}

	if (!social)
	{
		send_to_char("Social not found." NL, ch);
		return;
	}

	if (arg2[0] == '\0' || !str_cmp(arg2, "show"))
	{
		ch_printf(ch, "Social: %s\n\r\n\rCNoArg: %s" NL, social->name,
				social->char_no_arg);
		ch_printf(ch, "ONoArg: %s\n\rCFound: %s\n\rOFound: %s" NL,
				social->others_no_arg ? social->others_no_arg : "(not set)",
				social->char_found ? social->char_found : "(not set)",
				social->others_found ? social->others_found : "(not set)");
		ch_printf(ch, "VFound: %s\n\rCAuto : %s\n\rOAuto : %s" NL,
				social->vict_found ? social->vict_found : "(not set)",
				social->char_auto ? social->char_auto : "(not set)",
				social->others_auto ? social->others_auto : "(not set)");
		return;
	}

	if (get_trust(ch) > LEVEL_GOD && !str_cmp(arg2, "delete"))
	{
		unlink_social(social);
		free_social(social);
		send_to_char("Deleted." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "cnoarg"))
	{
		if (argument[0] == '\0' || !str_cmp(argument, "clear"))
		{
			send_to_char(
					"You cannot clear this field.  It must have a message." NL,
					ch);
			return;
		}
		STRDUP(social->char_no_arg, argument);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "onoarg"))
	{
		if (argument[0] != '\0' && str_cmp(argument, "clear"))
			STRDUP(social->others_no_arg, argument);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "cfound"))
	{
		if (argument[0] != '\0' && str_cmp(argument, "clear"))
			STRDUP(social->char_found, argument);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "ofound"))
	{
		if (argument[0] != '\0' && str_cmp(argument, "clear"))
			STRDUP(social->others_found, argument);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "vfound"))
	{
		if (argument[0] != '\0' && str_cmp(argument, "clear"))
			STRDUP(social->vict_found, argument);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "cauto"))
	{
		if (argument[0] != '\0' && str_cmp(argument, "clear"))
			STRDUP(social->char_auto, argument);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "oauto"))
	{
		if (argument[0] != '\0' && str_cmp(argument, "clear"))
			STRDUP(social->others_auto, argument);
		donemsg(ch);
		return;
	}

	if (get_trust(ch) > LEVEL_GREATER && !str_cmp(arg2, "name"))
	{
		bool relocate;

		one_argument(argument, arg1);
		if (arg1[0] == '\0')
		{
			send_to_char("Cannot clear name field!" NL, ch);
			return;
		}
		if (arg1[0] != social->name[0])
		{
			unlink_social(social);
			relocate = true;
		}
		else
			relocate = false;
		STRDUP(social->name, arg1);
		if (relocate)
			add_social(social);
		donemsg(ch);
		return;
	}

	/* display usage message */
	do_sedit(ch, (char*) "");
}

/*
 * Remove a command from it's hash index			-Thoric
 */
void unlink_command(CMDTYPE *command)
{
	CMDTYPE *tmp, *tmp_next;
	int hash;

	IF_BUG(command == NULL, "")
		; // @suppress("Suspicious semicolon")

	hash = command->name[0] % MAX_CMD_HASH;

	if (command == (tmp = command_hash[hash]))
	{
		command_hash[hash] = tmp->next;
		return;
	}
	for (; tmp; tmp = tmp_next)
	{
		tmp_next = tmp->next;
		if (command == tmp_next)
		{
			tmp->next = tmp_next->next;
			return;
		}
	}
}

/*
 * Add a command to the command hash table			-Thoric
 */
void add_command(CMDTYPE *command)
{
	int hash, x;
	CMDTYPE *tmp, *prev;

	IF_BUG(command == NULL, "")
		; // @suppress("Suspicious semicolon")
	IF_BUG(!command->name, "")
		; // @suppress("Suspicious semicolon")
	IF_BUG(!command->do_fun, "")
		; // @suppress("Suspicious semicolon")

	/* make sure the name is all lowercase */
	for (x = 0; command->name[x] != '\0'; x++)
		command->name[x] = LOWER(command->name[x]);

	hash = command->name[0] % MAX_CMD_HASH;

	if ((prev = tmp = command_hash[hash]) == NULL)
	{
		command->next = command_hash[hash];
		command_hash[hash] = command;
		return;
	}

	/* add to the END of the list */
	for (; tmp; tmp = tmp->next)
		if (!tmp->next)
		{
			tmp->next = command;
			command->next = NULL;
		}
	return;
}

//Command editor: Thoric+Thanos+Trog
// heh, mafia 3xT :P  -- Thanos
DEF_DO_FUN( cedit )
{
	CMDTYPE *pCmd;
	char arg1[MIL];

	if (*argument == '\0')
	{
		send_to_char("Syntax: cedit <command>" NL
		"Syntax: cedit create <command>" NL, ch);
		return;
	}

	argument = one_argument(argument, arg1);
	pCmd = find_command(arg1);

	if (!str_cmp(arg1, "create"))
	{
		if (*argument == '\0')
			send_to_char("Syntax: cedit create <command>" NL, ch);
		else
		{
			char buf[MIL];

			if (pCmd)
			{
				send_to_char("That command already exists!" NL, ch);
				return;
			}

			smash_tilde(argument);
			CREATE(pCmd, CMDTYPE, 1);
			STRDUP(pCmd->name, argument);
			STRDUP(pCmd->alias, "");
			pCmd->level = get_trust(ch);
			sprintf(buf, "do_%s", argument);
			if ((pCmd->do_fun = (DO_FUN*) skill_function(buf))
					== skill_notfound)
				send_to_char("Code not set." NL, ch);
			else
				send_to_char("Code set." NL, ch);
			add_command(pCmd);
			ch->desc->olc_editing = (void*) pCmd;
			ch->desc->connected = CON_CEDITOR;
			cedit(ch->desc, (char*) "show");
		}

		return;
	}

	if (!pCmd
			|| ((!str_prefix(pCmd->name, "admintalk")
					|| !str_cmp(pCmd->name, "\"")) && !IS_ADMIN(ch->name)))
	{
		send_to_char("No such command." NL, ch);
		return;
	}

	ch->desc->olc_editing = (void*) pCmd;
	ch->desc->connected = CON_CEDITOR;
	cedit(ch->desc, (char*) "show");

	return;
}

void cedit(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = d->character;
	CMDTYPE *pCmd;
	char arg[MIL];
	char arg1[MIL];

	if (!(pCmd = (CMDTYPE*) ch->desc->olc_editing))
	{
		bug("null pCmd.");
		edit_done(ch, (char*) "");
		return;
	}

	strcpy(arg, argument);
	argument = one_argument(argument, arg1);

	if (!str_prefix(arg1, "show"))
	{
		ch_printf(ch,
		COL_SCORE "Command:  " PLAIN "%s" COL_SCORE "." NL
		COL_SCORE "Alias:    " PLAIN "%s" COL_SCORE "." NL
		COL_SCORE "Level:    " PLAIN "%d" COL_SCORE "." NL
		COL_SCORE "Position: " PLAIN "%s" COL_SCORE "." NL
		COL_SCORE "Log:      " PLAIN "%s" COL_SCORE "." NL
		COL_SCORE "Code:     " PLAIN "%s" COL_SCORE "." NL
		COL_SCORE "Flags:    " PLAIN "%s" COL_SCORE "." NL, pCmd->name,
				(pCmd->alias[0] != '\0') ? pCmd->alias : "none", pCmd->level,
				bit_name(position_types_list, pCmd->position),
				bit_name(log_types_list, pCmd->log), skill_name(pCmd->do_fun),
				flag_string(command_flags_list, pCmd->flags));
		if (pCmd->userec.num_uses)
			send_timer(&pCmd->userec, ch);

		return;
	}

	if (!str_prefix(arg1, "?"))
	{
		show_help(ch, argument);
		return;
	}

	if (!str_prefix(arg1, "done"))
	{
		edit_done(ch, (char*) "");
		return;
	}

	if (!str_cmp(arg1, "help"))
	{
		send_to_char(
				"Commands:" NL "? create delete edit show save/fold raise lower list" NL
				NL "Fields:" NL " name code flags level position log alias" NL
				NL "Flags:" NL " hidden olccmd adminonly nh noorder" NL
				NL "Log types:" NL "See '? log_types' for detail" NL, ch);
		return;
	}

	if (!str_cmp(arg1, "create"))
	{
		do_cedit(ch, arg);
		return;
	}

	if (!str_cmp(arg1, "save") || !str_cmp(arg1, "fold"))
	{
		save_commands();
		send_to_char("Saved." NL, ch);
		return;
	}

	if (!str_cmp(arg1, "name"))
	{
		char arg2[MIL];

		one_argument(argument, arg2);

		if (*arg2 == '\0')
		{
			send_to_char("Command must have a name!" NL, ch);
			return;
		}

		unlink_command(pCmd);
		STRDUP(pCmd->name, arg2);
		add_command(pCmd);
		send_to_char("Name set." NL, ch);
		return;
	}

	if (!str_cmp(arg1, "code"))
	{
		DO_FUN *fun;

		if ((fun = skill_function(argument)) == skill_notfound)
			ch_printf(ch, "Code %s not found." NL, argument);
		else
		{
			pCmd->do_fun = fun;
			send_to_char("Code set." NL, ch);
		}

		return;
	}

	if (!str_cmp(arg1, "edit"))
	{
		do_cedit(ch, argument);
		return;
	}

	if (!str_cmp(arg1, "delete"))
	{
		unlink_command(pCmd);
		free_command(pCmd);
		send_to_char("Command deleted." NL, ch);
		edit_done(ch, (char*) "");
	}

	if (!str_cmp(arg1, "alias"))
	{
		if (*argument == '\0')
			send_to_char("Syntax: alias <alias> | delete", ch);
		else if (!str_cmp(argument, "none"))
		{
			STRDUP(pCmd->alias, "");
			send_to_char("Alias deleted." NL, ch);
		}
		else
		{
			STRDUP(pCmd->alias, argument);
			send_to_char("Alias set." NL, ch);
		}

		return;
	}

	if (!str_cmp(arg1, "list"))
	{
		CMDTYPE *tmp;
		int hash = pCmd->name[0] % MAX_CMD_HASH;

		pager_printf(ch, "Priority placement for [%s]:" NL, pCmd->name);
		for (tmp = command_hash[hash]; tmp; tmp = tmp->next)
			pager_printf(ch, "%s%s" EOL,
					tmp == pCmd ? FB_YELLOW "->" : PLAIN "  ", tmp->name);

		return;
	}

	if (!str_cmp(arg1, "raise"))
	{
		CMDTYPE *tmp, *tmp_next;
		int hash = pCmd->name[0] % MAX_CMD_HASH;

		if ((tmp = command_hash[hash]) == pCmd)
		{
			send_to_char("That command is already at the top." NL, ch);
			return;
		}

		if (tmp->next == pCmd)
		{
			command_hash[hash] = pCmd;
			tmp_next = tmp->next;
			tmp->next = pCmd->next;
			pCmd->next = tmp;
			ch_printf(ch, PLAIN
			"Moved " FB_WHITE "%s" PLAIN " above " FB_WHITE "%s" EOL,
					pCmd->name, pCmd->next->name);

			return;
		}

		for (; tmp; tmp = tmp->next)
		{
			tmp_next = tmp->next;
			if (tmp_next->next == pCmd)
			{
				tmp->next = pCmd;
				tmp_next->next = pCmd->next;
				pCmd->next = tmp_next;
				ch_printf(ch, PLAIN
				"Moved " FB_WHITE "%s" PLAIN " above " FB_WHITE "%s" EOL,
						pCmd->name, pCmd->next->name);

				return;
			}
		}
		send_to_char("ERROR -- Not Found!" NL, ch);

		return;
	}

	if (!str_cmp(arg1, "lower"))
	{
		CMDTYPE *tmp, *tmp_next;
		int hash = pCmd->name[0] % MAX_CMD_HASH;

		if (pCmd->next == NULL)
		{
			send_to_char("That command is already at the bottom." NL, ch);
			return;
		}

		tmp = command_hash[hash];
		if (tmp == pCmd)
		{
			tmp_next = tmp->next;
			command_hash[hash] = pCmd->next;
			pCmd->next = tmp_next->next;
			tmp_next->next = pCmd;
			ch_printf(ch, PLAIN
			"Moved " FB_WHITE "%s" PLAIN " below " FB_WHITE "%s" EOL,
					pCmd->name, tmp_next->name);

			return;
		}

		for (; tmp; tmp = tmp->next)
		{
			if (tmp->next == pCmd)
			{
				tmp_next = pCmd->next;
				tmp->next = tmp_next;
				pCmd->next = tmp_next->next;
				tmp_next->next = pCmd;
				ch_printf(ch, PLAIN
				"Moved " FB_WHITE "%s" PLAIN " below " FB_WHITE "%s" EOL,
						pCmd->name, tmp_next->name);

				return;
			}
		}
		send_to_char("ERROR -- Not Found!" NL, ch);

		return;
	}

	if (!str_cmp(arg1, "flags"))
	{
		int value;

		if (*argument == '\0')
		{
			send_to_char("Syntax: flags <flag>" NL, ch);
			return;
		}

		if ((value = flag_value(command_flags_list, argument)) < 0)
		{
			ch_printf(ch, "Unknown flag: %s" NL, argument);
			return;
		}

		TOGGLE_BIT(pCmd->flags, value);
		send_to_char("Command flag toggled." NL, ch);

		return;
	}

	if (!str_cmp(arg1, "level"))
	{
		int level = atoi(argument);
		int trust = get_trust(ch);

		if (level < 0 || level > trust)
		{
			ch_printf(ch, "Level range is: 0 - %d." NL, trust);
			return;
		}

		pCmd->level = level;
		send_to_char("Level set." NL, ch);
		return;
	}

	if (!str_cmp(arg1, "log"))
	{
		int log = flag_value(log_types_list, argument);

		if (log < 0 || log > LOG_PROG)
		{
			ch_printf(ch, "Invalid log type." NL);
			return;
		}

		pCmd->log = log;
		send_to_char("Log type set." NL, ch);
		return;
	}

	if (!str_cmp(arg1, "position"))
	{
		int position = flag_value(position_types_list, argument);

		if (position < 0 || position > POS_DRAG)
		{
			ch_printf(ch, "Invalid position." NL);
			return;
		}

		pCmd->position = position;
		send_to_char("Required position set." NL, ch);
		return;
	}

	interpret(ch, arg);
	return;
}

/*
 * Thanos - równie¿ ze Smauga
 * funkcja pokazuje zawarto¶ci ró¿norakich plików
 *
 * Original author: Thoric
 */
DEF_DO_FUN( fshow )
{
	char arg[MIL];

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char(
				"Syntax:  fshow < pbug | ptypo | pidea | dmesg | qbug >" NL,
				ch);
		return;
	}

	if (!str_cmp(arg, "pidea"))
	{
		show_complains(ch, complain_data::COMPLAIN_IDEA);
		return;
	}

	if (!str_cmp(arg, "pbug"))
	{
		show_complains(ch, complain_data::COMPLAIN_BUG);
		return;
	}

	if (!str_cmp(arg, "ptypo"))
	{
		show_complains(ch, complain_data::COMPLAIN_TYPO);
		return;
	}

	if (!str_cmp(arg, "dmesg"))
	{
		show_file(ch, BOOTLOG_FILE);
		return;
	}

	if (!str_cmp(arg, "qbug"))
	{
		show_file(ch, QBUG_FILE);
		return;
	}

	send_to_char("No such file." NL, ch);
	return;
}

