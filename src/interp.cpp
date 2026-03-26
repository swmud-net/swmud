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
 *			 Command interpretation module			   *
 ****************************************************************************/

#include <sys/types.h>
#if !defined( WIN32 )
#include <sys/time.h>
#else
#include <winsock.h>
#include <sys/timeb.h> /*for _ftime(), uses _timeb struct*/
#endif
#include <ctype.h>
#include <time.h>
#include "mud.h"

/*
 * Externals
 */

void subtract_times(struct timeval *etime, struct timeval *stime);

/*
 * Log-all switch.
 */
bool fLogAll = false;

CMDTYPE *command_hash[MAX_CMD_HASH]; /* hash table for cmd_table */
SOCIALTYPE *social_index[27]; /* hash table for socials   */

/* Added by Thanos -- szybciej sie koduje ;) */
void huh(CHAR_DATA *ch)
{
	if (ch)
		send_to_char("Co?" NL, ch);
	return;
}
/* Trog: no, o wiele szybciej :-) */
void donemsg(CHAR_DATA *ch)
{
	if (ch)
		send_to_char("Done." NL, ch);
	return;
}

/*
 * Character not in position for command?
 */
bool check_pos(CHAR_DATA *ch, int position)
{
	if (ch->position < position)
	{
		switch (ch->position)
		{
		case POS_DEAD:
			ch_printf(ch,
					"To b�dzie trudne, zwa�ywszy na to, �e jeste� MARTW%s..." NL,
					capitalize(SEX_SUFFIX_YAE(ch)));
			break;

		case POS_MORTAL:
		case POS_INCAP:
			ch_printf(ch, "Jeste� zbyt ci�zko rann%s, by to zrobi�..." NL,
					SEX_SUFFIX_YAE(ch));
			break;

		case POS_STUNNED:
			ch_printf(ch, "Jeste� zbyt mocno og�uszon%s, by to zrobi�..." NL,
					SEX_SUFFIX_YAE(ch));
			break;

		case POS_SLEEPING:
			ch_printf(ch, "Co ci si� �ni?" NL);
			break;

		case POS_RESTING:
			ch_printf(ch,
					"Niee. Jeste� zbyt odpr�on%s, by zajmowa� si� takimi drobnostkami." NL,
					SEX_SUFFIX_YAE(ch));
			break;

		case POS_SITTING:
			send_to_char("Nie mo�esz tego zrobi� siedz�c." NL, ch);
			break;

		case POS_FIGHTING:
			send_to_char("Nie ma mowy! W�a�nie walczysz!" NL, ch);
			break;

		}
		return false;
	}
	return true;
}

extern char lastplayercmd[MAX_INPUT_LENGTH * 2];

/*
 * Thanos: Teraz substitute..() zwraca wska�nik na stringa, a nie tak jak
 * wtedy  - interpretuje komend�, komendy s� rozdzielane i wykonywane kolejno
 * w zale�no�ci od waitstate gracza w comm.c, a przechowuje je d->incomm.
 */
char* substitute_alias(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch;
	char *point;
	char *rest;
	char *pntr;
	static char buf[ MAX_STRING_LENGTH];
	char name[ MAX_STRING_LENGTH];
	char arg[ MAX_STRING_LENGTH];

	ch = (d->original ? d->original : d->character);

//    sprintf( buf, argument );
	strcpy(buf, argument);

	if ( IS_NPC( ch ) || !ch->pcdata || ch->pcdata->aliases.empty())
		return (buf);

	for (auto* alias : ch->pcdata->aliases)
	{
		if (!str_prefix(alias->name, argument))
		{
			point = one_argument(argument, name);
			if (!str_cmp(alias->name, name))
			{
				strcpy(arg, alias->sub);
				buf[0] = '\0';

				//Added by Ratm( aliasy na wz�r SD )
				pntr = arg;
				rest = buf;
				while (*pntr != '\0')
				{
					if (*pntr == '$')
					{
						pntr++;
						*rest = '\0';
						strcat(buf, point);
						rest += strlen(point);
					}
					/*
					 * Thanos: dzi�ki multikomendom nie musimy ju� interpretowa�
					 *         ca�ej grupy polece�. Wystarczy zakomunikowa�, �e
					 *         u�ywamy multikomendy.
					 */
					else if (*pntr == ';')
					{
						*rest++ = *pntr++ = '|';
					}
					else
						*(rest++) = *(pntr++);
				}
				*(rest) = '\0';
				/* done by Thanos */
			}

			if (strlen(buf) > MAX_INPUT_LENGTH)
			{
				send_to_char("Alias za d�ugi, zostanie obci�ty." NL, ch);
				buf[ MAX_INPUT_LENGTH - 1] = '\0';
			}
		}
	}
//    interpret( d->character, buf );
	return (buf);
}
//done by Thanos

//byTrog
bool is_nhcmd(CHAR_DATA *ch, CMDTYPE *cmd)
{
	if ( IS_SET(cmd->flags, CMD_NH) && !IS_NPC( ch )
	&& ch->pcdata
	&& IS_SET( ch->pcdata->flags, PCFLAG_NH ))
		return true;

	return false;
}

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret(CHAR_DATA *ch, char *argument)
{
	char command[MIL] =
	{ 0 };
	char *all_argument;
	char buf[MSL] =
	{ 0 };
	char logline[MIL] =
	{ 0 };
	char logname[MIL] =
	{ 0 };
	TIMER *timer = NULL;
	CMDTYPE *cmd = NULL;
	int trust;
	int loglvl;
	bool found;
	struct timeval time_used;
	long tmptime;

	IF_BUG(ch == NULL, "")
		return;

	if (!str_cmp(argument, "prefix"))
	{
		do_prefix(ch, (char*) "");
		return;
	}

	if (ch->pcdata && *ch->pcdata->prefix)
		sprintf(buf, "%s %s", ch->pcdata->prefix, argument);
	else
		sprintf(buf, "%s", argument);

	all_argument = buf;

	found = false;
	if (ch->substate == SUB_REPEATCMD)
	{
		DO_FUN *fun;

		if ((fun = ch->last_cmd) == NULL)
		{
			ch->substate = SUB_NONE;
			bug("SUB_REPEATCMD with NULL last_cmd", 0);
			return;
		}
		else
		{
			int x;

			/*
			 * yes... we lose out on the hashing speediness here...
			 * but the only REPEATCMDS are wizcommands (currently)
			 */
			for (x = 0; x < MAX_CMD_HASH; x++)
			{
				for (cmd = command_hash[x]; cmd; cmd = cmd->next)
					if (cmd->do_fun == fun)
					{
						found = true;
						break;
					}
				if (found)
					break;
			}
			if (!found)
			{
				cmd = NULL;
				bug("SUB_REPEATCMD: last_cmd invalid");
				return;
			}
			swsnprintf(logline, MIL, "(%s) %s", cmd->name, all_argument);
		}
	}

	if (!cmd)
	{
		/* Changed the order of these ifchecks to prevent crashing. */
		if (!all_argument || *all_argument == '\0')
		{
			bug("null argument!");
			return;
		}

		/*
		 * Strip leading spaces.
		 */
		while (isspace(*all_argument))
			all_argument++;
		if (all_argument[0] == '\0')
			return;

		timer = get_timerptr(ch, TIMER_DO_FUN);

		/* REMOVE_BIT( ch->affected_by, AFF_HIDE ); */

		/*
		 * Implement freeze command.
		 */
		if (!IS_NPC(ch) && IS_SET(ch->act, PLR_FREEZE))
		{
			ch_printf(ch, "Jeste� zamro�on%s!" NL, SEX_SUFFIX_YAE(ch));
			return;
		}

		/*
		 * Grab the command word.
		 * Special parsing so ' can be a command,
		 *   also no spaces needed after punctuation.
		 */
		strcpy(logline, all_argument);
		if (!isalpha(all_argument[0]) && !isdigit(all_argument[0]))
		{
			command[0] = all_argument[0];
			command[1] = '\0';
			all_argument++;
			while (isspace(*all_argument))
				all_argument++;
		}
		else
			all_argument = one_argument(all_argument, command);

		/*
		 * Look for command in command table.
		 * Check for council powers and/or bestowments
		 */
		trust = get_trust(ch);

		cmd = find_command(command);

		if (cmd
				&& (cmd->level <= trust
						|| (!IS_NPC(ch) && ch->pcdata->bestowments
								&& ch->pcdata->bestowments[0] != '\0'
								&& is_name(cmd->name, ch->pcdata->bestowments)
								&& cmd->level <= (trust + 5))))
		{
			found = true;
		}
		if (cmd && is_nhcmd(ch, cmd))
			found = true;

		//Trog: nie moze byc order mob mpwithdraw 1000000.
		if (cmd && IS_NPC(ch) && IS_SET(cmd->flags, CMD_NOORDER)
				&& (IS_AFFECTED(ch, AFF_DOMINATED) || IS_AFFECTED(ch, AFF_CHARM)
						|| IS_AFFECTED(ch, AFF_POSSESS)))
			found = false;

		/* Thanos */
		if (cmd && IS_SET(cmd->flags, CMD_OLC) && IS_OLCMAN(ch))
			found = true;

		/* Trog */
		if (cmd && IS_SET(cmd->flags, CMD_HELP) && IS_HELPMASTER(ch->name))
			found = true;
		/*
		 * Turn off afk bit when any command performed.
		 */
		if (!IS_IMMORTAL(ch) && IS_SET(ch->act, PLR_AFK)
				&& (str_cmp(command, "AWAY")))
		{
			REMOVE_BIT(ch->act, PLR_AFK);
			act( PLAIN, "$n powraca do klawiaturki.", ch, NULL, NULL, TO_ROOM);
		}
	}

	//added by Thanos
	if (found && !IS_NPC(ch) && ch->pcdata && ch->pcdata->forbidden_cmd
			&& is_name(skill_name(cmd->do_fun), ch->pcdata->forbidden_cmd))
	{
		ch_printf(ch, "Niestety, komenda '%s' zosta�a ci odebrana." NL,
				cmd->name);
		return;
	}

	/*
	 * Log and snoop.
	 */
	sprintf(lastplayercmd, "** %s: %s", ch->name, logline);

	if (found && cmd->log == LOG_NEVER)
		strcpy(logline, "XXXXXXXX XXXXXXXX XXXXXXXX");

	loglvl = found ? cmd->log : LOG_NORMAL;

	if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_LOG)) || fLogAll
			|| loglvl == LOG_BUILD || loglvl == LOG_HIGH || loglvl == LOG_ALWAYS
			|| (loglvl == LOG_PROG && IS_NPC(ch)))
	{
		/* Added by Narn to show who is switched into a mob that executes
		 a logged command.  Check for descriptor in case force is used. */
		if (ch->desc && ch->desc->original)
			sprintf(log_buf, "Log %s (%s): %s", ch->name,
					ch->desc->original->name, logline);
		else
		{
			sprintf(log_buf, "Log %s: %s", ch->name, logline);
		}
		/*
		 * Make it so a 'log all' will send most output to the log
		 * file only, and not spam the log channel to death	-Thoric
		 */
		if (fLogAll && loglvl == LOG_NORMAL
				&& (IS_NPC(ch) || !IS_SET(ch->act, PLR_LOG)))
			loglvl = LOG_ALL;

		if (loglvl == LOG_PROG && IS_NPC(ch))
			sprintf(log_buf, "vnum: %d name: %s in_room: %d command: %s",
					ch->pIndexData->vnum, ch->name,
					ch->in_room ? ch->in_room->vnum : 0, logline);

		log_string_plus(log_buf, loglvl, get_trust(ch));
	}

	if (ch->desc && ch->desc->snoop_by)
	{
		sprintf(logname, "%s", ch->name);
		write_to_buffer(ch->desc->snoop_by, logname, 0);
		write_to_buffer(ch->desc->snoop_by, "% ", 2);
		write_to_buffer(ch->desc->snoop_by, logline, 0);
		write_to_buffer(ch->desc->snoop_by, NL, 2);
	}

	if (timer)
	{
		int tempsub;

		tempsub = ch->substate;
		ch->substate = SUB_TIMER_DO_ABORT;
		(timer->do_fun)(ch, (char*) "");
		if (char_died(ch))
			return;
		if (ch->substate != SUB_TIMER_CANT_ABORT)
		{
			ch->substate = tempsub;
			extract_timer(ch, timer);
		}
		else
		{
			ch->substate = tempsub;
			return;
		}
	}

	/*
	 * Look for command in skill and socials table.
	 */
	if (!found)
	{
		if (!check_skill(ch, command, all_argument)
				&& !check_social(ch, command, all_argument))
		{
			EXIT_DATA *pexit;

			/* check for an auto-matic exit command */
			if ((pexit = find_door(ch, command, true))
					!= NULL&& IS_SET( pexit->flags, EX_xAUTO )
					&& CAN_ENTER( ch, pexit->to_room ))
			{
				if ( IS_SET(pexit->flags, EX_CLOSED)
						&& (!IS_AFFECTED(ch, AFF_PASS_DOOR)
								|| IS_SET(pexit->flags, EX_NOPASSDOOR)))
				{
					if (!IS_SET(pexit->flags, EX_SECRET))
						act( PLAIN, "Te $d s� zamkni�te.", ch, NULL,
								pexit->keyword, TO_CHAR);
					else
						send_to_char(
								"Niestety nie mo�esz p�j�� w tym kierunku." NL,
								ch);
					return;
				}
				move_char(ch, pexit, 0);
				return;
			}
			huh(ch);
		}
		return;
	}

	/*
	 * Character not in position for command?
	 */
	if (!check_pos(ch, cmd->position))
		return;

	/* Berserk check for flee.. maybe add drunk to this?.. but too much
	 hardcoding is annoying.. -- Altrag */
	/* Kurwa... a nie dalo sie tego wkodowac w do_flee?? Hardcoding sux
	 -- Thanos */
	if (!str_cmp(cmd->name, "flee") && IS_AFFECTED(ch, AFF_BERSERK))
	{
		send_to_char("Co? Ucieczka? NIGDY! " FB_RED "KRWIII!" EOL, ch);
		return;
	}

	/*
	 * Dispatch the command.
	 */
	ch->last_cmd = cmd->do_fun;
	start_timer(&time_used);
	(*cmd->do_fun)(ch, all_argument);
	end_timer(&time_used);
	/*
	 * Update the record of how many times this command has been used (haus)
	 */
	update_userec(&time_used, &cmd->userec);
	tmptime = UMIN(time_used.tv_sec,19) * 1000000 + time_used.tv_usec;

	/* laggy command notice: command took longer than 1.5 seconds */
	if (tmptime > 1500000)
	{
		sprintf(log_buf, "[*****] LAG: %s: %s %s (R:%d S:%d.%06d)", ch->name,
				cmd->name, (cmd->log == LOG_NEVER ? "XXX" : all_argument),
				ch->in_room ? ch->in_room->vnum : 0, (int) (time_used.tv_sec),
				(int) (time_used.tv_usec));
		log_string_plus(log_buf, LOG_NORMAL, get_trust(ch));
	}

	tail_chain();
}

CMDTYPE* find_command(const char *command)
{
	CMDTYPE *cmd;
	int hash;

	/*
	 * Thanos: bugfix
	 * NIE MA KOMEND ZACZYNAJ�CYCH SI� NA POLSK� LITER� !!!
	 * (i w sumie dobrze - trzebaby bylo przewalac cale hashowanie komend)
	 */
	if (ispolchar(command[0]))
		return NULL;

	hash = LOWER(command[0]) % MAX_CMD_HASH;

	for (cmd = command_hash[hash]; cmd; cmd = cmd->next)
		if (cmd && !str_prefix(command, cmd->name))
			return cmd;
	/*
	 * Thanos: dodane aliasy komend (ich polskie odpowiedniki)
	 * szuka si� tego 'troch�' d�u�ej, ale i tak najpierw brane s�
	 * oryginalne wersje komend
	 */
	for (hash = 0; hash < MAX_CMD_HASH; hash++)
		for (cmd = command_hash[hash]; cmd; cmd = cmd->next)
			if (is_name_prefix(command, cmd->alias))
				return cmd;

	return NULL;
}

SOCIALTYPE* find_social(const char *command)
{
	SOCIALTYPE *social;
	int hash;

	if (command[0] < 'a' || command[0] > 'z')
//added by Ratm
		switch (command[0])
		{
		case '\xb1':
			hash = 1;
			break;
		case '\xe6':
			hash = 3;
			break;
		case '\xea':
			hash = ('e' - 'a') + 1;
			break;
		case '\xb3':
			hash = ('l' - 'a') + 1;
			break;
		case '\xf1':
			hash = ('n' - 'a') + 1;
			break;
		case '\xf3':
			hash = ('o' - 'a') + 1;
			break;
		case '\xb6':
			hash = ('s' - 'a') + 1;
			break;
		case '\xbf':
			hash = ('z' - 'a') + 1;
			break;
		case '\xbc':
			hash = ('z' - 'a') + 1;
			break;
		default:
			hash = 0;
		}
	else
		hash = (command[0] - 'a') + 1;

	for (social = social_index[hash]; social; social = social->next)
		if (!str_prefix(command, social->name))
			return social;

	return NULL;
}

bool check_social(CHAR_DATA *ch, const char *command, const char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	SOCIALTYPE *social;

	if ((social = find_social(command)) == NULL)
		return false;
	/*removed by Thanos
	 if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
	 {
	 send_to_char( "You are anti-social!\n\r", ch );
	 return true;
	 }
	 */
	switch (ch->position)
	{
	case POS_DEAD:
		ch_printf(ch, "Le� spokojnie. Jeste� MARTW%s!" NL,
				capitalize(SEX_SUFFIX_YAE(ch)));
		return true;

	case POS_INCAP:
	case POS_MORTAL:
		ch_printf(ch, "Jeste� zbyt ci�ko rann%s, by to zrobi�." NL,
				SEX_SUFFIX_YAE(ch));
		return true;

	case POS_STUNNED:
		ch_printf(ch, "Jeste� zbyt mocno og�uszon%s, by to zrobi�." NL,
				SEX_SUFFIX_YAE(ch));
		return true;

	case POS_SLEEPING:
		/*
		 * I just know this is the path to a 12" 'if' statement.  :(
		 * But two players asked for it already!  -- Furey
		 */
		if (!str_cmp(social->name, "snore"))
			break;
		send_to_char("Co ci si� �ni ???" NL, ch);
		return true;

	}

	strcpy(arg2, argument);
	one_argument(arg2, arg);
	victim = NULL;
	if (arg[0] == '\0')
	{
		act( COL_SOCIAL, social->others_no_arg, ch, NULL, victim, TO_ROOM);
		act( COL_SOCIAL, social->char_no_arg, ch, NULL, victim, TO_CHAR);
	}
	else if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
	}
	else if (victim == ch)
	{
		act( COL_SOCIAL, social->others_auto, ch, NULL, victim, TO_ROOM);
		act( COL_SOCIAL, social->char_auto, ch, NULL, victim, TO_CHAR);
	}
	else
	{
		act( COL_SOCIAL, social->others_found, ch, NULL, victim, TO_NOTVICT);
		act( COL_SOCIAL, social->char_found, ch, NULL, victim, TO_CHAR);
		act( COL_SOCIAL, social->vict_found, ch, NULL, victim, TO_VICT);

		if (!IS_NPC(ch) && IS_NPC(victim)
		&& !IS_AFFECTED(victim, AFF_CHARM)
		&& IS_AWAKE(victim)
		&& !IS_SET( victim->pIndexData->progtypes, ACT_PROG ))
		{
			switch (number_bits(4))
			{
			case 0:
				if (!IS_SET(ch->in_room->room_flags, ROOM_SAFE) || IS_EVIL(ch))
					multi_hit(victim, ch, TYPE_UNDEFINED);
				else if (IS_NEUTRAL(ch))
				{
					act( COL_ACTION, "$n klepie $N$3 w pup�.", victim, NULL, ch,
					TO_NOTVICT);
					act( COL_ACTION, "Klepiesz $N$3 w pup�.", victim, NULL, ch,
					TO_CHAR);
					act( COL_ACTION, "$n klepie ci� w pup�.", victim, NULL, ch,
					TO_VICT);
				}
				else
				{
					act( COL_ACTION,
							"$n zachowuje si� tak, jakby $N w og�le nie istnia�$o.",
							victim, NULL, ch, TO_NOTVICT);
					act( COL_ACTION, "Po prostu ignorujesz $N$3.", victim, NULL,
							ch, TO_CHAR);
					act( COL_ACTION, "Wygl�da na to, �e $n ma ci� gdzie�.",
							victim, NULL, ch, TO_VICT);
				}
				break;

			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
				act( COL_SOCIAL, social->others_found, victim, NULL, ch,
				TO_NOTVICT);
				act( COL_SOCIAL, social->char_found, victim, NULL, ch, TO_CHAR);
				act( COL_SOCIAL, social->vict_found, victim, NULL, ch, TO_VICT);
				break;

			case 9:
			case 10:
			case 11:
			case 12:
				act( COL_ACTION, "$n klepie $N$3 w pup�.", victim, NULL, ch,
				TO_NOTVICT);
				act( COL_ACTION, "Klepiesz $N$3 w pup�.", victim, NULL, ch,
				TO_CHAR);
				act( COL_ACTION, "$n klepie ci� w pup�.", victim, NULL, ch,
				TO_VICT);
				break;
			}
		}
	}

	return true;
}

/*
 * Return true if an argument is completely numeric.
 */
/*
 bool is_number( char *arg )
 {
 if ( *arg == '\0' )
 return false;

 for ( ; *arg != '\0'; arg++ )
 {
 if ( !isdigit(*arg) )
 return false;
 }

 return true;
 }
 Removed by Thanos -- p. bit.c
 */

/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument(char *argument, char *arg)
{
	char *pdot;
	int number;

	for (pdot = argument; *pdot != '\0'; pdot++)
	{
		if (*pdot == '.')
		{
			*pdot = '\0';
			number = atoi(argument);
			*pdot = '.';
			strcpy(arg, pdot + 1);
			return number;
		}
	}

	strcpy(arg, argument);
	return 1;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char* one_argument(char *argument, char *arg_first)
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
		*arg_first = LOWER(*argument);
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while (isspace(*argument))
		argument++;

	return argument;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.  Delimiters = { ' ', '-' }
 */
char* one_argument2(char *argument, char *arg_first)
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
		if (*argument == cEnd || *argument == '-')
		{
			argument++;
			break;
		}
		*arg_first = LOWER(*argument);
		arg_first++;
		argument++;
	}
	*arg_first = '\0';

	while (isspace(*argument))
		argument++;

	return argument;
}

DEF_DO_FUN( timecmd )
{
	struct timeval stime;
	struct timeval etime;
	static bool timing;
	extern CHAR_DATA *timechar;
	char arg[MAX_INPUT_LENGTH];

	send_to_char("Timing" NL, ch);
	if (timing)
		return;
	one_argument(argument, arg);
	if (!*arg)
	{
		send_to_char("No command to time." NL, ch);
		return;
	}
	if (!str_cmp(arg, "update"))
	{
		if (timechar)
			send_to_char("Another person is already timing updates." NL, ch);
		else
		{
			timechar = ch;
			send_to_char("Setting up to record next update loop." NL, ch);
		}
		return;
	}

	send_to_char("Starting timer." NL, ch);
	timing = true;
	gettimeofday(&stime, NULL);
	interpret(ch, argument);
	gettimeofday(&etime, NULL);
	timing = false;

	send_to_char("Timing complete." NL, ch);
	subtract_times(&etime, &stime);
	ch_printf(ch, "Timing took %d.%06d seconds." NL, etime.tv_sec,
			etime.tv_usec);
	return;
}

void start_timer(struct timeval *stime)
{
	IF_BUG(stime == NULL, "")
		return;

	gettimeofday(stime, NULL);
	return;
}

time_t end_timer(struct timeval *stime)
{
	struct timeval etime;

	/* Mark etime before checking stime, so that we get a better reading.. */
	gettimeofday(&etime, NULL);
	if (!stime || (!stime->tv_sec && !stime->tv_usec))
	{
		bug("bad stime.", 0);
		return 0;
	}
	subtract_times(&etime, stime);
	/* stime becomes time used */
	*stime = etime;
	return (etime.tv_sec * 1000000) + etime.tv_usec;
}

void send_timer(struct timerset *vtime, CHAR_DATA *ch)
{
	struct timeval ntime;
	int carry;

	if (vtime->num_uses == 0)
		return;
	ntime.tv_sec = vtime->total_time.tv_sec / vtime->num_uses;
	carry = (vtime->total_time.tv_sec % vtime->num_uses) * 1000000;
	ntime.tv_usec = (vtime->total_time.tv_usec + carry) / vtime->num_uses;
	ch_printf(ch, "Has been used %d times this boot." NL, vtime->num_uses);
	ch_printf(ch,
			"Time (in secs): min %d.%0.6d; avg: %d.%0.6d; max %d.%0.6d" NL,
			vtime->min_time.tv_sec, vtime->min_time.tv_usec, ntime.tv_sec,
			ntime.tv_usec, vtime->max_time.tv_sec, vtime->max_time.tv_usec);
	return;
}

void update_userec(struct timeval *time_used, struct timerset *userec)
{
	userec->num_uses++;
	if ( !timerisset(&userec->min_time)
	|| timercmp(time_used, &userec->min_time, <))
	{
		userec->min_time.tv_sec = time_used->tv_sec;
		userec->min_time.tv_usec = time_used->tv_usec;
	}
	if ( !timerisset(&userec->max_time)
	|| timercmp(time_used, &userec->max_time, >))
	{
		userec->max_time.tv_sec = time_used->tv_sec;
		userec->max_time.tv_usec = time_used->tv_usec;
	}
	userec->total_time.tv_sec += time_used->tv_sec;
	userec->total_time.tv_usec += time_used->tv_usec;
	while (userec->total_time.tv_usec >= 1000000)
	{
		userec->total_time.tv_sec++;
		userec->total_time.tv_usec -= 1000000;
	}
	return;
}
