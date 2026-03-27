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
 *			     Player skills module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <time.h>
#include "mud.h"

#define GET_DONE( )						\
	if( !str_prefix( arg1, "done" ) )	\
	{									\
		edit_done( ch, (char *)"" );			\
		return;							\
	}

#define GET_HELP( darg )				\
	if( !str_prefix( arg1, "?" ) )		\
	{									\
		show_help( ch, darg );			\
		return;							\
	}

#define GET_CREATE( cmd )				\
	if( !str_prefix( arg1, "create" ) )	\
	{									\
		cmd;							\
		return;							\
	}

const char * const spell_flag[] =
{  "water", "earth", "air", "astral", "area", "distant", "reverse",

		"save_half_dam", "save_negates", "accumulative", "recastable",
		"noscribe", "nobrew", "group", "object", "character", "secretskill",
		"pksensitive" };

const char *const spell_saves[] =
{ "none", "poison_death", "wands", "para_petri", "breath", "spell_staff" };

const char *const spell_damage[] =
{ "none", "fire", "cold", "electricity", "energy", "acid", "poison", "drain" };

const char *const spell_action[] =
{ "none", "create", "destroy", "resist", "suscept", "divinate", "obscure",
		"change" };

const char *const spell_power[] =
{ "none", "minor", "greater", "major" };

const char *const spell_class[] =
{ "none", "lunar", "solar", "travel", "summon", "life", "death", "illusion" };

const char *const target_type[] =
{ "ignore", "offensive", "defensive", "self", "objinv" };

void show_char_to_char(const std::list<CHAR_DATA*>& list, CHAR_DATA *ch);
void show_list_to_char(const std::list<OBJ_DATA*>& list, CHAR_DATA *ch, bool fShort, bool fShowN);

int ris_save(CHAR_DATA *ch, int chance, int ris);
bool check_illegal_psteal(CHAR_DATA *ch, CHAR_DATA *victim);

/* from force.c */
void failed_casting(struct skill_type *skill, CHAR_DATA *ch, CHAR_DATA *victim,
		OBJ_DATA *obj);

int xp_compute args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
const char* affect_loc_name args( ( int location ) );

ROOM_INDEX_DATA* generate_exit(ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit);

void skedit_show args( ( CHAR_DATA *ch, SKILLTYPE *skill, int sn ) );

/*
 * Dummy function
 */
void skill_notfound(CHAR_DATA *ch, char *argument)
{
	huh(ch);
	return;
}

/* Thanos */
bool check_knowledge(CHAR_DATA *ch, int sn)
{

	if (!ch || IS_NPC(ch) || (!IS_NPC(ch) && ch->pcdata->learned[sn] <= 0))
	{
		send_to_char("Nie masz zielonego poj�cia jak to si� robi." NL, ch);
		return false;
	}
	return true;
}

void break_skill(const char *txt, CHAR_DATA *ch)
{
	STRDUP(ch->dest_buf, "");
	STRDUP(ch->dest_buf_2, "");
	ch->substate = SUB_NONE;

	send_to_char(txt, ch);
	return;
}

void mount_msg(CHAR_DATA *ch)
{
	send_to_char("Mo�e najpierw zejd� z wierzchowca..." NL, ch);
	return;
}

void concentrate_msg(CHAR_DATA *ch)
{
	send_to_char("Nie mo�esz si� wystarczaj�co skupi� by to zrobi�." NL, ch);
	return;
}

void hes_not_here_msg(CHAR_DATA *ch)
{
	send_to_char("Nie ma tu nikogo takiego." NL, ch);
	return;
}

void its_not_here_msg(CHAR_DATA *ch)
{
	send_to_char("Nie ma tu niczego takiego." NL, ch);
	return;
}

void notfight_msg(CHAR_DATA *ch)
{
	send_to_char("Nie walczysz z nikim." NL, ch);
	return;
}

int get_ssave(char *name)
{
	int x;

	for (x = 0; x < ALEN(spell_saves); x++)
		if (!str_cmp(name, spell_saves[x]))
			return x;
	return -1;
}

int get_starget(char *name)
{
	int x;

	for (x = 0; x < ALEN(target_type); x++)
		if (!str_cmp(name, target_type[x]))
			return x;
	return -1;
}

int get_sflag(char *name)
{
	int x;

	for (x = 0; x < ALEN(spell_flag); x++)
		if (!str_cmp(name, spell_flag[x]))
			return x;
	return -1;
}

int get_sdamage(char *name)
{
	int x;

	for (x = 0; x < ALEN(spell_damage); x++)
		if (!str_cmp(name, spell_damage[x]))
			return x;
	return -1;
}

int get_saction(char *name)
{
	int x;

	for (x = 0; x < ALEN(spell_action); x++)
		if (!str_cmp(name, spell_action[x]))
			return x;
	return -1;
}

int get_spower(char *name)
{
	int x;

	for (x = 0; x < ALEN(spell_power); x++)
		if (!str_cmp(name, spell_power[x]))
			return x;
	return -1;
}

int get_sclass(char *name)
{
	int x;

	for (x = 0; x < ALEN(spell_class); x++)
		if (!str_cmp(name, spell_class[x]))
			return x;
	return -1;
}

bool is_legal_kill(CHAR_DATA *ch, CHAR_DATA *vch)
{
	if ( IS_NPC(ch) || IS_NPC(vch))
		return true;
	if (is_safe(ch, vch))
		return false;
	return true;
}

extern char *target_name; /* from force.c */

/*
 * Perform a binary search on a section of the skill table
 * Each different section of the skill table is sorted alphabetically
 * Only match skills player knows				-Thoric
 */
bool check_skill(CHAR_DATA *ch, char *command, char *argument)
{
	int sn;
	int first = gsn_first_skill;
	int top = gsn_first_weapon - 1;
	struct timeval time_used;
	int mana;

	/* bsearch for the skill */
	for (;;)
	{
		sn = (first + top) >> 1;

		if ( LOWER(command[0]) == LOWER(skill_table[sn]->name[0])
				&& !str_prefix(command, skill_table[sn]->name)
				&& (skill_table[sn]->skill_fun
						|| skill_table[sn]->spell_fun != spell_null)
				&& (IS_NPC(ch) || (ch->pcdata->learned[sn] > 0)))
			break;
		if (first >= top)
			return false;
		if (strcmp(command, skill_table[sn]->name) < 1)
			top = sn - 1;
		else
			first = sn + 1;
	}

	if (!check_pos(ch, skill_table[sn]->minimum_position))
		return true;

	if ( IS_NPC(ch)
			&& (IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED(ch, AFF_POSSESS)))
	{
		send_to_char("Z jakiego� powodu, nie mo�esz tego zrobi�..." NL, ch);
		act( PLAIN, "$n rozgl�da si� dooko�a.", ch, NULL, NULL, TO_ROOM);
		return true;
	}

	/* check if mana is required */
	if (skill_table[sn]->min_mana)
	{
		mana = IS_NPC(ch) ? 0 : skill_table[sn]->min_mana;

		if (!IS_NPC(ch) && ch->mana < mana)
		{
			send_to_char("Musisz odpocz�� aby u�y� mocy ponownie.\n\r", ch);
			return true;
		}
	}
	else
	{
		mana = 0;
	}

	/*
	 * Is this a real do-fun, or a really a spell?
	 */
	if (!skill_table[sn]->skill_fun)
	{
		ch_ret retcode = rNONE;
		void *vo = NULL;
		CHAR_DATA *victim = NULL;
		OBJ_DATA *obj = NULL;

		target_name = (char*) "";

		switch (skill_table[sn]->target)
		{
		default:
			bug("Check_skill: bad target for sn %d.", sn);
			send_to_char("Co� jest nie tak...\n\r", ch);
			return true;

		case TAR_IGNORE:
			vo = NULL;
			if (argument[0] == '\0')
			{
				if ((victim = who_fighting(ch)) != NULL)
					target_name = victim->name;
			}
			else
				target_name = argument;
			break;

		case TAR_CHAR_OFFENSIVE:
			if (argument[0] == '\0' && (victim = who_fighting(ch)) == NULL)
			{
				ch_printf(ch, "%s kogo?\n\r",
						capitalize(skill_table[sn]->name));
				return true;
			}
			else if (argument[0] != '\0'
					&& (victim = get_char_room(ch, argument)) == NULL)
			{
				hes_not_here_msg(ch);
				return true;
			}
			if (is_safe(ch, victim))
				return true;
			vo = (void*) victim;
			break;

		case TAR_CHAR_DEFENSIVE:
			if (argument[0] != '\0'
					&& (victim = get_char_room(ch, argument)) == NULL)
			{
				hes_not_here_msg(ch);
				return true;
			}
			if (!victim)
				victim = ch;
			vo = (void*) victim;
			break;

		case TAR_CHAR_SELF:
			vo = (void*) ch;
			break;

		case TAR_OBJ_INV:
			if ((obj = get_obj_carry(ch, argument)) == NULL)
			{
				its_not_here_msg(ch);
				return true;
			}
			vo = (void*) obj;
			break;
		}

		/* waitstate */
		WAIT_STATE(ch, skill_table[sn]->beats);
		/* check for failure */
		if ((number_percent() + skill_table[sn]->difficulty * 5)
				> (IS_NPC(ch) ? 75 : ch->pcdata->learned[sn]))
		{
			failed_casting(skill_table[sn], ch, (CD*) vo, obj);
			learn_from_failure(ch, sn);
			if (mana)
			{
				ch->mana -= mana / 2;
			}
			return true;
		}
		if (mana)
		{
			ch->mana -= mana;
		}
		start_timer(&time_used);
		retcode = (*skill_table[sn]->spell_fun)(sn, ch->top_level, ch, vo);
		end_timer(&time_used);
		update_userec(&time_used, &skill_table[sn]->userec);

		if (retcode == rCHAR_DIED || retcode == rERROR)
			return true;

		if (char_died(ch))
			return true;

		if (retcode == rSPELL_FAILED)
		{
			learn_from_failure(ch, sn);
			retcode = rNONE;
		}
		else
			learn_from_success(ch, sn);

		if (skill_table[sn]->target == TAR_CHAR_OFFENSIVE && victim != ch
				&& !char_died(victim))
		{
			for (auto* vch : ch->in_room->people)
			{
				if (victim == vch && !victim->fighting && victim->master != ch)
				{
					retcode = multi_hit(victim, ch, TYPE_UNDEFINED);
					break;
				}
			}
		}
		return true;
	}

	if (mana)
	{
		ch->mana -= mana;
	}
	ch->last_cmd = skill_table[sn]->skill_fun;
	start_timer(&time_used);
	(*skill_table[sn]->skill_fun)(ch, argument);
	end_timer(&time_used);
	update_userec(&time_used, &skill_table[sn]->userec);

	tail_chain();
	return true;
}

DEF_DO_FUN( sset )
{
	CHAR_DATA *victim;
	char arg1[MIL];
	char arg2[MIL];
	int value, sn, ability;
	bool fAll;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (*arg1 == '\0' || *arg2 == '\0' || *argument == '\0')
	{
		send_to_char(
				"Syntax: sset <victim> <value> <abilityname | skillname> | all" NL,
				ch);
		return;
	}

	if (!(victim = get_char_world(ch, arg1)) || IS_NPC(victim))
	{
		send_to_char("Nie ma takiego gracza." NL, ch);
		return;
	}

	ability = flag_value(class_types_list, argument);
	fAll = !str_cmp(argument, "all");
	sn = 0;
	if (!fAll && (sn = skill_lookup(argument)) < 0 && ability == NO_FLAG)
	{
		send_to_char("No such skill or ability." NL, ch);
		return;
	}
	if (!is_number(arg2) || (value = atoi(arg2)) < 0 || value > 100)
	{
		send_to_char("Value range is 0 - 100." NL, ch);
		return;
	}
	if (fAll)
		for (sn = 0; sn < top_sn; sn++)
		{
			/* Fix by Narn to prevent ssetting skills the player shouldn't have. */
			if (skill_table[sn]->guild
					< 0|| skill_table[sn]->guild >= MAX_ABILITY)
				continue;
			if (skill_table[sn]->name
					&& (victim->skill_level[skill_table[sn]->guild]
							>= skill_table[sn]->min_level || value == 0))
				victim->pcdata->learned[sn] = value;
		}
	else if (ability != NO_FLAG)
		for (sn = 0; sn < top_sn; sn++)
		{
			if (skill_table[sn]->guild != ability)
				continue;
			if (skill_table[sn]->name
					&& (victim->skill_level[ability]
							>= skill_table[sn]->min_level || value == 0))
				victim->pcdata->learned[sn] = value;
		}
	else
		victim->pcdata->learned[sn] = value;

	return;
}

#if defined (ARMAGEDDON)
/* From Rom24b6 					-- Thanos */
void check_improve( CHAR_DATA *ch, int sn, bool success )
{
    int		adept;
    int 	chance;
    int		ch_level;
    int		gain=0;
    int		lrn;

    if (IS_NPC(ch))
	return;

    if (skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY )
            return;

    if ( sn == skill_lookup( "meditate" ) && ch->skill_level[FORCE_ABILITY] < 2 )
         gain_exp( ch, 25, FORCE_ABILITY );

    ch_level = ch->skill_level[skill_table[sn]->guild];

    adept = ( ch_level - skill_table[sn]->min_level )* 5 + 50;
    adept = UMIN(adept, 100);

    lrn   = ch->pcdata->learned[sn];

    if ( ch_level < skill_table[sn]->min_level
    ||  lrn == 0 || lrn == adept)
	return;

    /* Jak nie immo to skille innych prof tylko na 99 % */
    if( skill_table[sn]->guild != ch->main_ability
    && !IS_IMMORTAL( ch )  &&  lrn >= 99 )
	return;


    chance = 10 * int_app[get_curr_int(ch)].learn;


    if( lrn > 1 )     /* Floating point exception preventing :P  Thanos */
	chance/= lrn/2;
    chance += ch_level;

    if( !skill_table[sn]->skill_fun /* skille automatyczne */
    &&   skill_table[sn]->spell_fun == spell_null  )
    {
	if (number_range(1,(lrn*lrn)/5) > chance)
	    return;
    }
    else
    {
	if (number_range(1,(lrn*lrn)/7) > chance)
	    return;
    }

    if (success)
    {
	chance = URANGE(5, 100-lrn, 95);
	if (number_percent() < chance)
	{
	    ch_printf( ch, FG_BLUE
		"Stajesz si� lepsz%s w skillu " FB_BLUE "%s" FG_BLUE "!" EOL,
			SEX_SUFFIX_YAE( ch ),
			skill_table[sn]->name);
	    ch->pcdata->learned[sn]++;
	    ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn],100);
	    gain = 100 * skill_table[sn]->min_level;
	}
    }
    else
    {
	chance = URANGE(5, lrn, 30);
	if (number_percent() < chance)
	{
	    ch_printf(ch, FG_BLUE
		"Mimo b��du twoja znajomo�� " FB_BLUE "%s" FG_BLUE " wzrasta!" EOL,
		    skill_table[sn]->name );
	    ch->pcdata->learned[sn]++;
	    ch->pcdata->learned[sn] = UMIN(ch->pcdata->learned[sn],100);
	    gain = 70 * skill_table[sn]->min_level;
	}
    }

	fevent_trigger( ch, FE_SKILL );

	if( !gain )
	return;

    gain+=number_range( gain/10, 2*gain/10 );
    ch_printf( ch, FG_BLUE "Zdobywasz " FB_BLUE "%d" FG_BLUE " punk%s do�wiadczenia." EOL,
	gain, NUMBER_SUFF( gain, "t", "ty", "t�w" ) );

    gain_exp(ch,gain, skill_table[sn]->guild );

    if ( ch->pcdata->learned[sn] == 100 )
    {
        ch_printf( ch, FB_BLUE
	    "Jeste� teraz ADEPTEM skillu %s!  "			NL
	    "Otrzymujesz dodatkowe " FB_BLUE "%d" FG_BLUE " punkt�w do�wiadczenia." 	EOL,
		skill_table[sn]->name, gain*2 );
	gain_exp(ch,gain*2, skill_table[sn]->guild );
    }
    return;
}

void learn_from_success( CHAR_DATA *ch, int sn )
{
    check_improve( ch, sn, true );
    return;
}

void learn_from_failure( CHAR_DATA *ch, int sn )
{
    check_improve( ch, sn, false );
    return;
}

#else

void learn_from_success(CHAR_DATA *ch, int sn)
{
	int adept, gain, sklvl, learn, percent, chance;

	if ( IS_NPC(ch) || ch->pcdata->learned[sn] == 0
			|| ch->pcdata->learned[sn] >= 100)
		return;

	if (sn == skill_lookup("meditate") && ch->skill_level[FORCE_ABILITY] < 2)
		gain_exp(ch, 25, FORCE_ABILITY);

	sklvl = skill_table[sn]->min_level;

	if (skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY)
		return;

	adept = (ch->skill_level[skill_table[sn]->guild]
			- skill_table[sn]->min_level) * 5 + 50;
	adept = UMIN(adept, 100);

	if (ch->pcdata->learned[sn] >= adept
			&& ch->skill_level[skill_table[sn]->guild] < 100)
		return;

	if (sklvl == 0 || sklvl > ch->skill_level[skill_table[sn]->guild])
		sklvl = ch->skill_level[skill_table[sn]->guild];
	if (ch->pcdata->learned[sn] < 100)
	{
		chance = ch->pcdata->learned[sn] + (5 * skill_table[sn]->difficulty);
		percent = number_percent();
		if (percent >= chance)
			learn = 2;
		else if (chance - percent > 25)
			return;
		else
			learn = 1;

		if (number_percent() > 30)
			return;

		ch->pcdata->learned[sn] = UMIN(adept, ch->pcdata->learned[sn] + learn);
		if (ch->pcdata->learned[sn] == 100) /* fully learned! */
		{
			gain = 50 * sklvl;
			ch_printf(ch, FB_BLUE
			"Zostajesz ADEPTEM skillu %s!  " EOL
			"Otrzymujesz dodatkowe %d punkt�w do�wiadczenia." EOL,
					skill_table[sn]->name, gain);
		}
		else
		{
			gain = 10 * sklvl;
			ch_printf(ch, FB_BLUE
			"Stajesz si� lepsz%s w skillu %s !!!   " EOL
			"Zdobywasz %d punkt�w do�wiadczenia." EOL, SEX_SUFFIX_YAE(ch),
					skill_table[sn]->name, gain);
		}
		gain_exp(ch, gain, skill_table[sn]->guild);
	}
}

//added by Thanos (tej funkcji nie by�o nie wiedzie� czemu...)
void learn_from_failure(CHAR_DATA *ch, int sn)
{
	int adept, gain, sklvl, learn, percent, chance;

	if (number_percent() > 75) //zeby nie zdarzalo sie zbyt czesto
		return;

	if ( IS_NPC(ch) || ch->pcdata->learned[sn] == 0)
		return;

	if (sn == skill_lookup("meditate"))
		return;

	sklvl = skill_table[sn]->min_level;

	if (skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY)
		return;

	adept = (ch->skill_level[skill_table[sn]->guild]
			- skill_table[sn]->min_level) * 5 + 50;
	adept = UMIN(adept, 100);

	if (ch->pcdata->learned[sn] >= adept
			&& ch->skill_level[skill_table[sn]->guild] < 100)
		return;

	if (sklvl == 0 || sklvl > ch->skill_level[skill_table[sn]->guild])
		sklvl = ch->skill_level[skill_table[sn]->guild];
	if (ch->pcdata->learned[sn] < 100)
	{
		chance = ch->pcdata->learned[sn] + (skill_table[sn]->difficulty);
		percent = number_percent();
		if (percent >= chance)
			learn = 2;
		else if (chance - percent > 10)
			return;
		else
			learn = 1;

		if (number_percent() > 20)
			return;

		ch->pcdata->learned[sn] = UMIN(adept, ch->pcdata->learned[sn] + learn);
		if (ch->pcdata->learned[sn] == 100) /* fully learned! */
		{
			gain = 10 * sklvl;
			ch_printf(ch, FB_BLUE
			"Mimo b��du zostajesz ADEPTEM skillu %s !!!" EOL
			"Otrzymujesz dodatkowe %d punkt�w do�wiadczenia." EOL,
					skill_table[sn]->name, gain);
		}
		else
		{
			gain = 2 * sklvl;
			ch_printf(ch,
			FG_BLUE
			"Uczysz si� na b��dach!" NL
			"Stajesz si� lepsz%s w skillu %s. " EOL
			"Zdobywasz %d punkt�w do�wiadczenia." EOL, SEX_SUFFIX_YAE(ch),
					skill_table[sn]->name, gain);
		}
		gain_exp(ch, gain, skill_table[sn]->guild);
	}
	return;
}
#endif

DEF_DO_FUN( gouge )
{
	CHAR_DATA *victim;
	AFFECT_DATA af;
	int dam;
	int percent;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	if (!IS_NPC(ch) && !ch->pcdata->learned[gsn_gouge])
	{
		send_to_char("Musisz si� najpierw tego nauczy�.\n\r", ch);
		return;
	}

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	if ((victim = who_fighting(ch)) == NULL)
	{
		notfight_msg(ch);
		return;
	}

	// Thanos; bugfix
	if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_DROID))
	{
		send_to_char("Nie mo�esz wyd�uba� oczu droidowi." NL, ch);
		return;
	}

	percent = number_percent() - (get_curr_lck(ch) - 13);

	if ( IS_NPC(ch) || percent < ch->pcdata->learned[gsn_gouge])
	{
		dam = number_range(1, ch->skill_level[COMBAT_ABILITY]);
		global_retcode = damage(ch, victim, dam, gsn_gouge);
		if (global_retcode == rNONE)
		{
			if (!IS_AFFECTED(victim, AFF_BLIND))
			{
				af.type = gsn_blindness;
				af.location = APPLY_HITROLL;
				af.modifier = -6;
				af.duration = 3 + (ch->skill_level[COMBAT_ABILITY] / 20);
				af.bitvector = AFF_BLIND;
				affect_to_char(victim, &af);
				act( COL_ACTION, "Nic nie widzisz!", victim, NULL, NULL,
						TO_CHAR);
			}
			WAIT_STATE(ch, PULSE_VIOLENCE);
			WAIT_STATE(victim, PULSE_VIOLENCE);
			/* Taken out by request - put back in by Thoric
			 * This is how it was designed.  You'd be a tad stunned
			 * if someone gouged you in the eye.
			 */
		}
		else if (global_retcode == rVICT_DIED)
		{
			act( FG_RED,
					"Wbi�$a� palce tak g��boko, �e naruszy�$a� m�zg powoduj�c natychmiastow� �mier�!",
					ch, NULL, NULL, TO_CHAR);
		}
		if (global_retcode != rCHAR_DIED && global_retcode != rBOTH_DIED)
			learn_from_success(ch, gsn_gouge);
	}
	else
	{
		WAIT_STATE(ch, skill_table[gsn_gouge]->beats);
		global_retcode = damage(ch, victim, 0, gsn_gouge);
		learn_from_failure(ch, gsn_gouge);
	}

	return;
}

DEF_DO_FUN( detrap )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj = nullptr;
	OBJ_DATA *trap;
	int percent;
	bool found;

	switch (ch->substate)
	{
	default:
		if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
		{
			concentrate_msg(ch);
			return;
		}
		argument = one_argument(argument, arg);
		if (!IS_NPC(ch) && !ch->pcdata->learned[gsn_detrap])
		{
			send_to_char("Jeszcze tego nie umiesz." NL, ch);
			return;
		}
		if (arg[0] == '\0')
		{
			send_to_char("Jak� pu�apk� chcesz rozbroi�?" NL, ch);
			return;
		}
		if (ms_find_obj(ch))
			return;
		found = false;
		if (ch->mount)
		{
			mount_msg(ch);
			return;
		}
		if (ch->in_room->contents.empty())
		{
			its_not_here_msg(ch);
			return;
		}
		for (auto* o : ch->in_room->contents)
		{
			if (can_see_obj(ch, o) && nifty_is_name(arg, o->name))
			{
				obj = o;
				found = true;
				break;
			}
		}
		if (!found)
		{
			its_not_here_msg(ch);
			return;
		}
		act( COL_ACTION, "Pr�bujesz ostro�nie zdemontowa� pu�apk� z $p$1...",
				ch, obj->przypadki[1], NULL, TO_CHAR);
		act( COL_ACTION, "$n ostro�nie pr�buje zdj�� pu�apk� z $p$1...", ch,
				obj->przypadki[1], NULL, TO_ROOM);
		STRDUP(ch->dest_buf, obj->name);
		add_timer(ch, TIMER_DO_FUN, 3, do_detrap, 1);
		return;
	case 1:
		if (!*ch->dest_buf)
		{
			send_to_char("Demontowanie pu�apki zosta�o przerwane!\n\r", ch);
			bug("do_detrap: ch->dest_buf NULL!", 0);
			return;
		}
		snprintf(arg, sizeof(arg), "%s", ch->dest_buf);
		STRDUP(ch->dest_buf, "");
		ch->substate = SUB_NONE;
		break;
	case SUB_TIMER_DO_ABORT:
		break_skill("Ostro�nie przestajesz zajmowa� si� pu�apk�.\n\r", ch);
		return;
	}

	if (ch->in_room->contents.empty())
	{
		send_to_char("Nie mo�esz tutaj tego znale��.\n\r", ch);
		return;
	}
	for (auto* obj : ch->in_room->contents)
	{
		if (can_see_obj(ch, obj) && nifty_is_name(arg, obj->name))
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		its_not_here_msg(ch);
		return;
	}
	if ((trap = get_trap(obj)) == NULL)
	{
		send_to_char("To nie zawiera �adnej pu�apki.\n\r", ch);
		return;
	}

	percent = number_percent() - (ch->skill_level[SMUGGLING_ABILITY] / 20)
			- (get_curr_lck(ch) - 16);

	separate_obj(obj);
	if (!IS_NPC(ch) || percent > ch->pcdata->learned[gsn_detrap])
	{
		send_to_char("Ooops!\n\r", ch);
		spring_trap(ch, trap);
		learn_from_failure(ch, gsn_detrap);
		return;
	}

	extract_obj(trap);

	send_to_char("Uda�o si�... Pomy�lnie demontujesz pu�apk�.\n\r", ch);
	learn_from_success(ch, gsn_detrap);
	return;
}

DEF_DO_FUN( dig )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	bool shovel;
	EXIT_DATA *pexit;

	switch (ch->substate)
	{
	default:
		if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
		{
			concentrate_msg(ch);
			return;
		}
		if (ch->mount)
		{
			mount_msg(ch);
			return;
		}
		one_argument(argument, arg);
		if (arg[0] != '\0')
		{
			if ((pexit = find_door(ch, arg, true)) == NULL
					&& get_dir(arg) == -1)
			{
				send_to_char("A co to za kierunek?\n\r", ch);
				return;
			}
			if (pexit)
			{
				if (!IS_SET(pexit->flags,
						EX_DIG) && !IS_SET(pexit->flags, EX_CLOSED))
				{
					send_to_char(
							"Nie ma potrzeby robi� tego w tym kierunku.\n\r",
							ch);
					return;
				}
			}
		}
		else
		{
			switch (ch->in_room->sector_type)
			{
			case SECT_CITY:
			case SECT_INSIDE:
				send_to_char(
						"Pod�oga jest zbyt twarda aby si� przez ni� przekopa�.\n\r",
						ch);
				return;
			case SECT_WATER_SWIM:
			case SECT_WATER_NOSWIM:
			case SECT_UNDERWATER:
				send_to_char("Kopa� w wodzie?!\n\r", ch);
				return;
			case SECT_AIR:
				send_to_char("Co? W powietrzu?!\n\r", ch);
				return;
			}
		}
		add_timer(ch, TIMER_DO_FUN, UMIN(skill_table[gsn_dig]->beats / 10, 3),
				do_dig, 1);
		STRDUP(ch->dest_buf, arg);
		send_to_char("Zaczynasz kopa�...\n\r", ch);
		act( PLAIN, "$n zaczyna kopa�...", ch, NULL, NULL, TO_ROOM);
		return;

	case 1:
		if (!ch->dest_buf)
		{
			send_to_char("Kopanie zosta�o przerwane!\n\r", ch);
			act( PLAIN, "Kopanie $n$1 zosta�o przerwane!", ch, NULL, NULL,
					TO_ROOM);
			bug("do_dig: dest_buf NULL", 0);
			return;
		}
		snprintf(arg, sizeof(arg), "%s", ch->dest_buf);
		STRDUP(ch->dest_buf, "");
		break;

	case SUB_TIMER_DO_ABORT:
		break_skill("Przestajesz kopa�...\n\r", ch);
		act( PLAIN, "$n przestaje kopa�...", ch, NULL, NULL, TO_ROOM);
		return;
	}

	ch->substate = SUB_NONE;

	/* not having a shovel makes it harder to succeed */
	shovel = false;
	for (auto* obj : ch->carrying)
		if (obj->item_type == ITEM_SHOVEL)
		{
			shovel = true;
			break;
		}

	/* dig out an EX_DIG exit... */
	if (arg[0] != '\0')
	{
		if ((pexit = find_door(ch, arg, true))
				!= NULL && IS_SET( pexit->flags, EX_DIG )
				&& IS_SET( pexit->flags, EX_CLOSED ))
		{
			/* 4 times harder to dig open a passage without a shovel */
			if ((number_percent() * (shovel ? 1 : 4))
					< (IS_NPC(ch) ? 80 : ch->pcdata->learned[gsn_dig]))
			{
				REMOVE_BIT(pexit->flags, EX_CLOSED);
				send_to_char("Przekopujesz si�!\n\r", ch);
				act( PLAIN, "$n przekopuje si�!", ch, NULL, NULL, TO_ROOM);
				learn_from_success(ch, gsn_dig);
				return;
			}
		}
		learn_from_failure(ch, gsn_dig);
		send_to_char("Kopanie nie znalaz�o �adnego przej�cia...\n\r", ch);
		act( PLAIN, "Kopanie $n$1 nie znalaz�o �adnego przej�cia...", ch, NULL,
				NULL, TO_ROOM);
		return;
	}

	OBJ_DATA *found_obj = nullptr;

	for (auto* o : ch->in_room->contents)
	{
		/* twice as hard to find something without a shovel */
		if ( IS_OBJ_STAT(o, ITEM_BURRIED)
				&& (number_percent() * (shovel ? 1 : 2))
						< (IS_NPC(ch) ? 80 : ch->pcdata->learned[gsn_dig]))
		{
			found_obj = o;
			break;
		}
	}

	if (!found_obj)
	{
		send_to_char("Nic nie znajdujesz kopi�c.\n\r", ch);
		act( PLAIN, "Kopanie $n$1 nic nie znalaz�o.", ch, NULL, NULL, TO_ROOM);
		learn_from_failure(ch, gsn_dig);
		return;
	}

	obj = found_obj;
	separate_obj(obj);
	REMOVE_BIT(obj->extra_flags, ITEM_BURRIED);
	act( COL_ACTION, "Kopanie odkry�o $p!", ch, obj, NULL, TO_CHAR);
	act( COL_ACTION, "Kopanie $n$1 odkry�o $p$3!", ch, obj, NULL, TO_ROOM);
	learn_from_success(ch, gsn_dig);

	return;
}

DEF_DO_FUN( search )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj = nullptr;
	OBJ_DATA *container;
	int percent, door;
	bool found;

	door = -1;
	switch (ch->substate)
	{
	default:
		if (IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
		{
			concentrate_msg(ch);
			return;
		}
		if (ch->mount)
		{
			mount_msg(ch);
			return;
		}
		argument = one_argument(argument, arg);
		if (arg[0] != '\0' && (door = get_door(arg)) == -1)
		{
			container = get_obj_here(ch, arg);
			if (!container)
			{
				its_not_here_msg(ch);
				return;
			}
			if (container->item_type != ITEM_CONTAINER)
			{
				send_to_char("Nie mo�esz szuka� w TYM!" NL, ch);
				return;
			}
			if (IS_SET(container->value[1], CONT_CLOSED))
			{
				send_to_char("Ten pojemnik jest zamkni�ty." NL, ch);
				return;
			}
		}
		add_timer(ch, TIMER_DO_FUN,
				UMIN(skill_table[gsn_search]->beats / 10, 3), do_search, 1);
		send_to_char("Rozpoczynasz �mudne przeszukiwanie..." NL, ch);
		STRDUP(ch->dest_buf, arg);
		return;

	case 1:
		if (!ch->dest_buf)
		{
			send_to_char("Twoje przeszukiwanie zosta�o przerwane!" NL, ch);
			bug("do_search: dest_buf NULL", 0);
			return;
		}
		snprintf(arg, sizeof(arg), "%s", ch->dest_buf);
		STRDUP(ch->dest_buf, "");
		break;
	case SUB_TIMER_DO_ABORT:
		break_skill("Przerywasz poszukiwania." NL, ch);
		return;
	}
	ch->substate = SUB_NONE;
	std::list<OBJ_DATA*> *search_list = nullptr;
	if (arg[0] == '\0')
	{
		search_list = &ch->in_room->contents;
	}
	else
	{
		if ((door = get_door(arg)) != -1)
			search_list = nullptr;
		else
		{
			container = get_obj_here(ch, arg);
			if (!container)
			{
				its_not_here_msg(ch);
				return;
			}
			search_list = &container->contents;
		}
	}

	found = false;

	if (((!search_list || search_list->empty()) && door == -1) || IS_NPC(ch))
	{
		send_to_char("Nie uda�o ci si� niczego znale��." NL, ch);
		learn_from_failure(ch, gsn_search);
		return;
	}

	percent = number_percent();

	if (door != -1)
	{
		EXIT_DATA *pexit;

		if ((pexit = get_exit(ch->in_room, door)) != NULL
				&& IS_SET(pexit->flags, EX_SECRET)
				&& IS_SET(pexit->flags, EX_xSEARCHABLE)
				&& percent
						< (IS_NPC(ch) ? 80 : ch->pcdata->learned[gsn_search]))
		{
			act(COL_ACTION, "Natrafiasz na $d!", ch, NULL, pexit->keyword,
					TO_CHAR);
			act(COL_ACTION, "$n znajduje $d!", ch, NULL, pexit->keyword,
					TO_ROOM);
			REMOVE_BIT(pexit->flags, EX_SECRET);
			learn_from_success(ch, gsn_search);
			return;
		}
	}
	else
	{
		for (auto* o : *search_list)
		{
			if (IS_OBJ_STAT(o, ITEM_HIDDEN)
					&& percent < ch->pcdata->learned[gsn_search])
			{
				obj = o;
				found = true;
				break;
			}
		}
	}

	if (!found)
	{
		send_to_char("Nie uda�o ci si� nic znale��." NL, ch);
		learn_from_failure(ch, gsn_search);
		return;
	}

	separate_obj(obj);
	REMOVE_BIT(obj->extra_flags, ITEM_HIDDEN);
	act(COL_ACTION, "Natrafi�$a� na $p$3!", ch, obj, NULL, TO_CHAR);
	act(COL_ACTION, "$n natrafi�$o na $p$3!", ch, obj, NULL, TO_ROOM);
	learn_from_success(ch, gsn_search);
	return;
}

DEF_DO_FUN( steal )
{
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	CHAR_DATA *victim, *mst;
	OBJ_DATA *obj, *obj_next;
	int percent, xp;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Ukra�� co i od kogo?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if ((victim = get_char_room(ch, arg2)) == NULL)
	{
		hes_not_here_msg(ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Nieeee, lepiej nie. Jeszcze si� domy�lisz." NL, ch);
		return;
	}

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char("To nie najlepsze miejsce na kradzie�e." NL, ch);
		return;
	}

	if (check_illegal_psteal(ch, victim))
	{
		send_to_char("Nie mo�esz okrada� graczy!" NL, ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_steal]->beats);
	percent = number_percent() + ( IS_AWAKE(victim) ? 10 : -50)
			- (get_curr_lck(ch) - 15) + (get_curr_lck(victim) - 13)
			+ times_killed(ch, victim) * 7;

	if (victim->position == POS_FIGHTING
			|| percent > ( IS_NPC(ch) ? 90 : ch->pcdata->learned[gsn_steal]))
	{
		/*
		 * Failure.
		 */
		send_to_char("Oops..." NL, ch);
		act( COL_ACTION, "$n pr�bowa�$o ci� okra��!", ch, NULL, victim,
				TO_VICT);
		act( COL_ACTION, "$n pr�bowa�$o okra�� $N$3.", ch, NULL, victim,
				TO_NOTVICT);

		sprintf(buf, "%s to smarkaty z�odziej !!!", ch->name);
		do_yell(victim, buf);

		learn_from_failure(ch, gsn_steal);
		if (!IS_NPC(ch))
		{
			if (legal_loot(ch, victim) && !IS_IMMORTAL(ch))
			{
				PLANET_DATA *planet;

//			global_retcode = multi_hit( victim, ch, TYPE_UNDEFINED );

				// Thanos: zalatalem, bo padal jak victim->...->planet == NULL
				// co w gruncie rzeczy jest bugiem (???)
				// �ata sama w sobie nie do konca jest pewna, bo first_planet
				// tez w sumie moze byc NULL, ale to mud SW, wiec.. ;)
				if (victim->in_room->area->planet)
					planet = victim->in_room->area->planet;
				else
					planet = planet_list.empty() ? nullptr : planet_list.front();

				if (find_crime(ch, planet))
					ch_printf(ch, FG_YELLOW
					"Kradn�c na %s pogarszasz swoj� sytuacj�." EOL,
							capitalize(planet->name));
				else
					ch_printf(ch, FG_YELLOW
					"Jeste� teraz poszukiwan%s na %s." EOL, SEX_SUFFIX_YAE(ch),
							capitalize(planet->name));

				crime_to_char(ch, planet->name, CRIME_STEAL);
			}
			else
			{
				/* log_string( buf ); */
				if (IS_NPC(ch))
				{
					if ((mst = ch->master) == NULL)
						return;
				}
				else
					mst = ch;
				if (IS_NPC(mst))
					return;

			}
		}

		return;
	}

	if (IS_NPC(victim))
		add_kill(ch, victim); /* makes it harder to steal from same char */

	if (!str_cmp(arg1, "credits") || !str_cmp(arg1, "kredytek")
			|| !str_cmp(arg1, "credit") || !str_cmp(arg1, "kredytki")
			|| !str_cmp(arg1, "money") || !str_cmp(arg1, "kredytka"))
	{
		int amount;

		amount = (int) (victim->gold * number_range(1, 10) / 100);
		if (amount <= 0)
		{
			send_to_char("Nie uda�o ci si� zw�dzi� �adnej kasy..." NL, ch);
			learn_from_failure(ch, gsn_steal);
			return;
		}

		ch->gold += amount;
		victim->gold -= amount;
		ch_printf(ch, "Aha! Zdobywasz %d kredyt%s." NL, amount,
				NUMBER_SUFF(amount, "k�", "ki", "ek"));
		learn_from_success(ch, gsn_steal);
		if (IS_NPC(victim))
			;
		{
			xp =
					UMIN(amount * 10,
							( exp_level( ch->skill_level[SMUGGLING_ABILITY]+1 ) - exp_level( ch->skill_level[SMUGGLING_ABILITY]) ) / 35);
			xp = UMIN(xp, xp_compute(ch, victim));
			gain_exp(ch, xp, SMUGGLING_ABILITY);
			ch_printf(ch,
					FB_WHITE "Zdobywasz %ld punkt�w do�wiadczenia przemytniczego!" EOL,
					xp);
		}
		return;
	}

	if ((obj = get_obj_carry(victim, arg1)) == NULL)
	{
		if (victim->position <= POS_SLEEPING)
		{
			if ((obj = get_obj_wear(victim, arg1)) != NULL)
			{
				if ((obj_next = get_eq_char(victim, obj->wear_loc)) != obj)
				{
					ch_printf(ch, "%s nosi %s na %s." NL, PERS(victim, ch, 0),
							obj_next->przypadki[3], obj->przypadki[5]);
					send_to_char("Najpierw musisz to zw�dzi�." NL, ch);
					learn_from_failure(ch, gsn_steal);
					return;
				}
				else
					unequip_char(victim, obj);
			}
		}

		send_to_char("Jako� nie mo�esz tego znale��." NL, ch);
		learn_from_failure(ch, gsn_steal);
		return;
	}

	if (!can_drop_obj(ch, obj) || IS_OBJ_STAT(obj, ITEM_PERSONAL)/*Thanos*/
	|| IS_OBJ_STAT(obj, ITEM_INVENTORY) || IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
	{
		send_to_char("Nie udaje ci si� tego zwin��." NL, ch);
		learn_from_failure(ch, gsn_steal);
		return;
	}

	if (ch->carry_number + (get_obj_number(obj) / obj->count) > can_carry_n(ch))
	{
		send_to_char("Hej! Masz pe�ne r�ce!" NL, ch);
		learn_from_failure(ch, gsn_steal);
		return;
	}

	if (ch->carry_weight + (get_obj_weight(obj) / obj->count) > can_carry_w(ch))
	{
		send_to_char("Nie ud�wigniesz tak wiele." NL, ch);
		learn_from_failure(ch, gsn_steal);
		return;
	}

	send_to_char("Ok." NL, ch);
	learn_from_success(ch, gsn_steal);
	if (IS_NPC(victim))
		;
	{
		xp =
				UMIN(obj->cost * 10,
						( exp_level( ch->skill_level[SMUGGLING_ABILITY]+1) - exp_level( ch->skill_level[SMUGGLING_ABILITY]) ) / 10);
		xp = UMIN(xp, xp_compute(ch, victim));
		gain_exp(ch, xp, SMUGGLING_ABILITY);
		ch_printf(ch,
				FB_WHITE "Zdobywasz %ld punkt�w do�wiadczenia przemytniczego!" EOL,
				xp);
	}
	separate_obj(obj);
	obj_from_char(obj);
	obj_to_char(obj, ch);

	return;
}

DEF_DO_FUN( backstab )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int percent;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	one_argument(argument, arg);

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz potraktowa� ciosem w plecy?" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		hes_not_here_msg(ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Nie uda ci si� zaskoczy� samego siebie!" NL, ch);
		return;
	}

	if (is_safe(ch, victim))
		return;

	/* Added stabbing weapon. -Narn */
	if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL
			|| (obj->value[3] != WEAPON_VIBRO_BLADE))
	{
		send_to_char("Potrzebujesz kr�tkiej tn�cej broni." NL, ch);
		return;
	}

	if (victim->fighting || IS_RACE(victim, "YUUZHAN VONG"))
	{
		send_to_char(
				"Twoja ofiara jest zbyt czujna. Nie da si� podej�� od ty�u." NL,
				ch);
		return;
	}

	/* Can backstab a char even if it's hurt as long as it's sleeping. -Narn */
	if (victim->hit < victim->max_hit && IS_AWAKE(victim))
	{
		act( PLAIN, "$N jest rann$y i podejrzliw$y... Nie uda ci si� zakra��.",
				ch, NULL, victim, TO_CHAR);
		return;
	}

	percent = number_percent() - (get_curr_lck(ch) - 14)
			+ (get_curr_lck(victim) - 13);

	WAIT_STATE(ch, skill_table[gsn_backstab]->beats);
	if (!IS_AWAKE(victim) || IS_NPC(ch)
			|| percent < ch->pcdata->learned[gsn_backstab])
	{
		learn_from_success(ch, gsn_backstab);
		global_retcode = multi_hit(ch, victim, gsn_backstab);

	}
	else
	{
		learn_from_failure(ch, gsn_backstab);
		global_retcode = damage(ch, victim, 0, gsn_backstab);
	}
	return;
}

DEF_DO_FUN( rescue )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	CHAR_DATA *fch;
	int percent;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz wybawi� z opresji?" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		hes_not_here_msg(ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Pr�bujesz ratowa� sw� sk�r�, ale jako� ci nie idzie." NL,
				ch);
		return;
	}

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	if (!IS_NPC(ch) && IS_NPC(victim))
	{
		ch_printf(ch, "%s nie potrzebuje twojej pomocy." NL,
		MALE(victim) ? "On" : FEMALE(victim) ? "Ona" : "To");
		return;
	}

	if ((fch = who_fighting(victim)) == NULL)
	{
		ch_printf(ch, "%s nie potrzebuje twojej pomocy." NL,
		MALE(victim) ? "On" : FEMALE(victim) ? "Ona" : "To");
		return;
	}

	ch->alignment = ch->alignment + 5;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);

	percent = number_percent() - (get_curr_lck(ch) - 14)
			- (get_curr_lck(victim) - 16);

	WAIT_STATE(ch, skill_table[gsn_rescue]->beats);
	if (!IS_NPC(ch) && percent > ch->pcdata->learned[gsn_rescue])
	{
		send_to_char("Nie uda�o ci si� nikogo wyratowa�" NL, ch);
		act( COL_ACTION, "$n pr�bowa�$o ci� uratowa�!", ch, NULL, victim,
				TO_VICT);
		act( COL_ACTION, "$n bezskutecznie pr�buje uratowa� $N$3!", ch, NULL,
				victim, TO_NOTVICT);
		learn_from_failure(ch, gsn_rescue);
		return;
	}

	act( COL_ACTION, "Zas�aniasz $N$3 swoim cia�em!", ch, NULL, victim,
			TO_CHAR);
	act( COL_ACTION, "$n zas�ania $N$3 swoim cia�em!", ch, NULL, victim,
			TO_VICT);
	act( COL_ACTION, "$n zas�ania $N$3 swoim cia�em!", ch, NULL, victim,
			TO_NOTVICT);

	ch->alignment = ch->alignment + 50;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);

	learn_from_success(ch, gsn_rescue);
	stop_fighting(fch, false);
	stop_fighting(victim, false);
	if (ch->fighting)
		stop_fighting(ch, false);

	/* check_killer( ch, fch ); */
	set_fighting(ch, fch);
	set_fighting(fch, ch);
	return;
}

DEF_DO_FUN( kick )
{
	CHAR_DATA *victim;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	if ((victim = who_fighting(ch)) == NULL)
	{
		notfight_msg(ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_kick]->beats);
	if ( IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_kick])
	{
		learn_from_success(ch, gsn_kick);
		global_retcode = damage(ch, victim,
				number_range(1, ch->skill_level[COMBAT_ABILITY]), gsn_kick);
	}
	else
	{
		learn_from_failure(ch, gsn_kick);
		global_retcode = damage(ch, victim, 0, gsn_kick);
	}
	return;
}

DEF_DO_FUN( punch )
{
	CHAR_DATA *victim;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	if (!IS_NPC(ch) && ch->pcdata->learned[gsn_punch] <= 0)
	{
		send_to_char("Nie masz poj�cia jak to si� robi." NL, ch);
		return;
	}

	if ((victim = who_fighting(ch)) == NULL)
	{
		notfight_msg(ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_punch]->beats);
	if ( IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_punch])
	{
		learn_from_success(ch, gsn_punch);
		global_retcode = damage(ch, victim,
				number_range(1, ch->skill_level[COMBAT_ABILITY]), gsn_punch);
	}
	else
	{
		learn_from_failure(ch, gsn_punch);
		global_retcode = damage(ch, victim, 0, gsn_punch);
	}
	return;
}

DEF_DO_FUN( bite )
{
}

DEF_DO_FUN( claw )
{
}

DEF_DO_FUN( sting )
{
}

DEF_DO_FUN( tail )
{
}

DEF_DO_FUN( bash )
{
	CHAR_DATA *victim;
	int chance;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	if (!IS_NPC(ch) && ch->pcdata->learned[gsn_bash] <= 0)
	{
		send_to_char("Nie masz poj�cia jak to zrobi�" NL, ch);
		return;
	}

	if ((victim = who_fighting(ch)) == NULL)
	{
		notfight_msg(ch);
		return;
	}

	chance = (((get_curr_dex(victim) + get_curr_str(victim))
			- (get_curr_dex(ch) + get_curr_str(ch))) * 10) + 10;
	if (!IS_NPC(ch) && !IS_NPC(victim))
		chance += 25;
	if (victim->fighting && victim->fighting->who != ch)
		chance += 19;
	WAIT_STATE(ch, skill_table[gsn_bash]->beats);
	if ( IS_NPC(ch)
			|| (number_percent() + chance) < ch->pcdata->learned[gsn_bash])
	{
		learn_from_success(ch, gsn_bash);
		/* do not change anything here!  -Thoric */
		WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
		WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
		victim->position = POS_SITTING;
		fevent_trigger(ch, FE_POSITION);
		global_retcode = damage(ch, victim,
				number_range(1, ch->skill_level[COMBAT_ABILITY]), gsn_bash);
	}
	else
	{
		WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
		learn_from_failure(ch, gsn_bash);
		global_retcode = damage(ch, victim, 0, gsn_bash);
	}
	return;
}

DEF_DO_FUN( stun )
{
	CHAR_DATA *victim;
	AFFECT_DATA af;
	int chance;
	bool fail;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	if (!IS_NPC(ch) && ch->pcdata->learned[gsn_stun] <= 0)
	{
		send_to_char("Nie wiesz jak to si� robi." NL, ch);
		return;
	}

	if ((victim = who_fighting(ch)) == NULL)
	{
		notfight_msg(ch);
		return;
	}

	if (ch->move < 16)
	{
		send_to_char( COL_ACTION, ch);
		send_to_char("Nie starczy ci si� by to zrobi�." NL, ch);
		return; /* missing return fixed March 11/96 */
	}

	WAIT_STATE(ch, skill_table[gsn_stun]->beats);
	fail = false;
	chance = ris_save(victim, ch->skill_level[COMBAT_ABILITY], RIS_PARALYSIS);
	if (chance == 1000)
		fail = true;
	else
		fail = saves_para_petri(chance, victim);

	chance = (((get_curr_dex(victim) + get_curr_str(victim))
			- (get_curr_dex(ch) + get_curr_str(ch))) * 10) + 10;
	/* harder for player to stun another player */
	if (!IS_NPC(ch) && !IS_NPC(victim))
		chance += sysdata.stun_plr_vs_plr;
	else
		chance += sysdata.stun_regular;
	if (!fail
			&& ( IS_NPC(ch)
					|| (number_percent() + chance)
							< ch->pcdata->learned[gsn_stun]))
	{
		learn_from_success(ch, gsn_stun);
		/*    DO *NOT* CHANGE!    -Thoric    */
		ch->move -= 15;
		WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
		WAIT_STATE(victim, PULSE_VIOLENCE);
		act( COL_ACTION, "$N uderza w ciebie z ca�� si�� og�uszaj�c ci�!",
				victim, NULL, ch, TO_CHAR);
		act( COL_ACTION, "Uderzasz w $N$1 z ca�� si�� i powalasz $I!", ch, NULL,
				victim, TO_CHAR);
		act( COL_ACTION, "$n uderza w $N$1 z ca�� si�� i powala $I!", ch, NULL,
				victim, TO_NOTVICT);
		if (!IS_AFFECTED(victim, AFF_PARALYSIS))
		{
			af.type = gsn_stun;
			af.location = APPLY_AC;
			af.modifier = 20;
			af.duration = 3;
			af.bitvector = AFF_PARALYSIS;
			affect_to_char(victim, &af);
			update_pos(victim);
		}
	}
	else
	{
		WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
		ch->move -= 5;
		learn_from_failure(ch, gsn_stun);
		act( COL_ACTION,
				"$N napiera na ciebie z ca�� si��, ale zr�cznie odskakujesz.",
				victim, NULL, ch, TO_CHAR);
		act( COL_ACTION, "Napierasz na $N$1, ale $E odskakuje �miej�c si�.", ch,
				NULL, victim, TO_CHAR);
		act( COL_ACTION, "$n napiera na $N$1, ale $E odskakuje �miej�c si�.",
				ch, NULL, victim, TO_NOTVICT);
	}
	return;
}

DEF_DO_FUN( feed )
{
	huh(ch);
	return;
}

/*
 * Disarm a creature.
 * Caller must check for successful attack.
 * Check for loyalty flag (weapon disarms to inventory) for pkillers -Blodkai
 */
void disarm(CHAR_DATA *ch, CHAR_DATA *victim)
{
	OBJ_DATA *obj, *tmpobj;

	if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
		return;

	if ((tmpobj = get_eq_char(victim, WEAR_DUAL_WIELD)) != NULL
			&& number_bits(1) == 0)
		obj = tmpobj;

	if (get_eq_char(ch, WEAR_WIELD) == NULL && number_bits(1) == 0)
	{
		learn_from_failure(ch, gsn_disarm);
		return;
	}

	if ( IS_NPC( ch ) && !can_see_obj(ch, obj) && number_bits(1) == 0)
	{
		learn_from_failure(ch, gsn_disarm);
		return;
	}

	if (check_grip(ch, victim))
	{
		learn_from_failure(ch, gsn_disarm);
		return;
	}

	act( FB_WHITE, "$n ci� * ROZBRAJA * !", ch, NULL, victim, TO_VICT);
	act( FB_WHITE, "* ROZBRAJASZ * $N$3!", ch, NULL, victim, TO_CHAR);
	act( FB_WHITE, "$n rozbraja $N$3!", ch, NULL, victim, TO_NOTVICT);
	learn_from_success(ch, gsn_disarm);

	if (obj == get_eq_char(victim, WEAR_WIELD)
			&& (tmpobj = get_eq_char(victim, WEAR_DUAL_WIELD)) != NULL)
		tmpobj->wear_loc = WEAR_WIELD;

	obj_from_char(obj);
	obj_to_room(obj, victim->in_room);

	return;
}

DEF_DO_FUN( disarm )
{
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int percent;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	if (!IS_NPC(ch) && ch->pcdata->learned[gsn_disarm] <= 0)
	{
		send_to_char("Nie wiesz jak mo�esz to zrobi�." NL, ch);
		return;
	}

	if (get_eq_char(ch, WEAR_WIELD) == NULL)
	{
		send_to_char("Aby kogo� rozbroi� musisz dzier�y� bro�." NL, ch);
		return;
	}

	if ((victim = who_fighting(ch)) == NULL)
	{
		notfight_msg(ch);
		return;
	}

	if ((obj = get_eq_char(victim, WEAR_WIELD)) == NULL)
	{
		send_to_char("Tw�j przeciwnik nie ma broni w r�ku!" NL, ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_disarm]->beats);
	percent = number_percent() + victim->skill_level[COMBAT_ABILITY]
			- ch->skill_level[COMBAT_ABILITY] - (get_curr_lck(ch) - 15)
			+ (get_curr_lck(victim) - 15);
	if (!can_see_obj(ch, obj))
		percent += 10;
	if ( IS_NPC(ch) || percent < ch->pcdata->learned[gsn_disarm] * 2 / 3)
		disarm(ch, victim);
	else
	{
		send_to_char("Nie uda�o ci si�." NL, ch);
		learn_from_failure(ch, gsn_disarm);
	}
	return;
}

/*
 * Trip a creature.
 * Caller must check for successful attack.
 */
void trip(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if ( IS_AFFECTED( victim, AFF_FLYING ) || IS_AFFECTED(victim, AFF_FLOATING))
		return;

	if (victim->mount)
	{
		if ( IS_AFFECTED(victim->mount,
				AFF_FLYING) || IS_AFFECTED( victim->mount, AFF_FLOATING ))
			return;
		act( COL_ACTION,
				"$n rzuca si� na twojego wierzchowca i spadasz na ziemi�!", ch,
				NULL, victim, TO_VICT);
		act( COL_ACTION,
				"Rzucasz si� na wierzchowca $N$1, a $E spada na ziemi�!", ch,
				NULL, victim, TO_CHAR);
		act( COL_ACTION,
				"$n rzuca si� na wierzchowca $N$1, a $E spada na ziemi�!", ch,
				NULL, victim, TO_NOTVICT);
		REMOVE_BIT(victim->mount->act, ACT_MOUNTED);
		victim->mount = NULL;
		WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
		WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
		victim->position = POS_RESTING;
		fevent_trigger(ch, FE_POSITION);
		return;
	}
	if (victim->wait == 0)
	{
		act( PLAIN, "$n" PLAIN " rzuca si� na ciebie i upadasz na ziemi�!", ch,
				NULL, victim, TO_VICT);
		act( PLAIN,
				"Rzucasz si� na $N$3" PLAIN " i $N" PLAIN " upada na ziemi�!",
				ch, NULL, victim, TO_CHAR);
		act( PLAIN,
				"$n" PLAIN " rzuca si� na $N$3" PLAIN " i $N" PLAIN " upada na ziemi�!",
				ch, NULL, victim, TO_NOTVICT);

		WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
		WAIT_STATE(victim, 2 * PULSE_VIOLENCE);
		victim->position = POS_RESTING;
		fevent_trigger(ch, FE_POSITION);
	}

	return;
}

DEF_DO_FUN( pick )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	EXIT_DATA *pexit;
	SHIP_DATA *ship;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Co chcesz odkluczy�?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_pick_lock]->beats);

	/* look for guards */
	for (auto* gch : ch->in_room->people)
	{
		if ( IS_NPC(gch) && IS_AWAKE(gch)
				&& ch->skill_level[SMUGGLING_ABILITY] < gch->top_level)
		{
			act( PLAIN, "$N stoi za blisko zamka. Wyczuje ci�.", ch, NULL, gch,
					TO_CHAR);
			return;
		}
	}

	if ((pexit = find_door(ch, arg, true)) != NULL)
	{
		/* 'pick door' */
		/*	ROOM_INDEX_DATA *to_room; *//* Unused */
		EXIT_DATA *pexit_rev;

		if (!IS_SET(pexit->flags, EX_CLOSED))
		{
			send_to_char("Hej. To ju� jest otwarte." NL, ch);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char("Nie da si� tego otworzy�." NL, ch);
			return;
		}
		if (!IS_SET(pexit->flags, EX_LOCKED))
		{
			send_to_char("Hej. To jest ju� odkluczone." NL, ch);
			return;
		}
		if (IS_SET(pexit->flags, EX_PICKPROOF))
		{
			send_to_char("Nie uda�o ci si�." NL, ch);
			learn_from_failure(ch, gsn_pick_lock);
			check_room_for_traps(ch, TRAP_PICK | trap_door[pexit->vdir]);
			return;
		}

		if ( IS_NPC(ch)
				|| number_percent() > ch->pcdata->learned[gsn_pick_lock])
		{
			send_to_char("Nie uda�o ci si�." NL, ch);
			learn_from_failure(ch, gsn_pick_lock);
			return;
		}

		REMOVE_BIT(pexit->flags, EX_LOCKED);
		send_to_char("*Click*" NL, ch);
		act( COL_ACTION, "$n wy�amuje $d.", ch, NULL, pexit->keyword, TO_ROOM);
		learn_from_success(ch, gsn_pick_lock);
		/* pick the other side */
		if ((pexit_rev = pexit->rexit) != NULL
				&& pexit_rev->to_room == ch->in_room)
		{
			REMOVE_BIT(pexit_rev->flags, EX_LOCKED);
		}
		check_room_for_traps(ch, TRAP_PICK | trap_door[pexit->vdir]);
		return;
	}

	if ((obj = get_obj_here(ch, arg)) != NULL)
	{
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char("Nie mo�esz tego wy�ama�" NL, ch);
			return;
		}
		if (!IS_SET(obj->value[1], CONT_CLOSED))
		{
			send_to_char("Hej. To ju� jest otwarte." NL, ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char("Tego nie da si� wy�ama�." NL, ch);
			return;
		}
		if (!IS_SET(obj->value[1], CONT_LOCKED))
		{
			send_to_char("Hej. To jest ju� odkluczone." NL, ch);
			return;
		}
		if (IS_SET(obj->value[1], CONT_PICKPROOF))
		{
			send_to_char("Nie uda�o ci si�." NL, ch);
			learn_from_failure(ch, gsn_pick_lock);
			check_for_trap(ch, obj, TRAP_PICK);
			return;
		}

		if ( IS_NPC(ch)
				|| number_percent() > ch->pcdata->learned[gsn_pick_lock])
		{
			send_to_char("Nie uda�o ci si�." NL, ch);
			learn_from_failure(ch, gsn_pick_lock);
			return;
		}

		separate_obj(obj);
		REMOVE_BIT(obj->value[1], CONT_LOCKED);
		send_to_char("*Click*" NL, ch);
		act( COL_ACTION, "$n wy�amuje $p$3.", ch, obj, NULL, TO_ROOM);
		learn_from_success(ch, gsn_pick_lock);
		check_for_trap(ch, obj, TRAP_PICK);
		return;
	}

	if ((ship = ship_in_room(ch->in_room, arg)) != NULL)
	{
		char buf[MAX_STRING_LENGTH];

		if ( IS_NPC(ch) || !ch->pcdata->learned[gsn_pickshiplock])
		{
			send_to_char("Co?" NL, ch);
			return;
		}

		if (!known_biotech(ship, ch))
		{
			ch_printf(ch,
					FB_RED "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." EOL);
			return;
		}
		if (check_pilot(ch, ship))
		{
			send_to_char("Hmm.. Chcesz si� w�ama� do swojego statku???" NL, ch);
			return;
		}

		if (ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED)
		{
			send_to_char("Ten statek w�a�nie startuje." NL, ch);
			return;
		}

		WAIT_STATE(ch, skill_table[gsn_pickshiplock]->beats);

		if ( IS_NPC(ch) || !ch->pcdata
				|| number_percent() > ch->pcdata->learned[gsn_pickshiplock])
		{
			send_to_char("Nie uda�o ci si�." NL, ch);
			sprintf(buf, "%s pr�buje w�ama� si� do %s!", ch->name, ship->name);
			log_string(buf);
			learn_from_failure(ch, gsn_pickshiplock);
			return;
		}

		if (!ship->hatchopen)
		{
			if (!str_cmp(ship->lock_key, "0000"))
			{
				ship->hatchopen = true;
				act( PLAIN, "Wy�amujesz zamek i otwierasz klap� statku $T.", ch,
						NULL, ship->name, TO_CHAR);
				act( PLAIN, "$n otwiera klap� statku $T.", ch, NULL, ship->name,
						TO_ROOM);
				echo_to_room(ship->entrance, FB_YELLOW
				"Klapa owtera si� od zewn�trz." EOL);
				learn_from_success(ch, gsn_pickshiplock);
			}
			else
			{
				/* Trog: Alde jak to skonczysz to odkomentuj, wqrza mnie podczas kompilacji. :-)
				 char kode[MSL];
				 char digit1[MSL];
				 char digit2[MSL];
				 char digit3[MSL];
				 char digit4[MSL];
				 */
				act( PLAIN,
						"Zamek $T jest dobrze zabezpieczony, nie udaje ci si�",
						ch, NULL, ship->name, TO_CHAR);
				learn_from_failure(ch, gsn_pickshiplock);

			}
		}
		return;
	}

	ch_printf(ch, "Nie ma tu �adnego %s." NL, arg);
	return;
}

DEF_DO_FUN( sneak )
{
	AFFECT_DATA af;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	send_to_char("Zaczynasz porusza� si� niebywale cicho." NL, ch);
	affect_strip(ch, gsn_sneak);

	if ( IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_sneak])
	{
		af.type = gsn_sneak;
		af.duration = ch->skill_level[SMUGGLING_ABILITY] * DUR_CONV;
		af.location = APPLY_NONE;
		af.modifier = 0;
		af.bitvector = AFF_SNEAK;
		affect_to_char(ch, &af);
		learn_from_success(ch, gsn_sneak);
	}
	else
		learn_from_failure(ch, gsn_sneak);

	return;
}

DEF_DO_FUN( hide )
{
	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_HIDE))
		REMOVE_BIT(ch->affected_by, AFF_HIDE);

	if ( IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_hide])
	{
		send_to_char("Wtapiasz si� w otoczenie." NL, ch);
		SET_BIT(ch->affected_by, AFF_HIDE);
		learn_from_success(ch, gsn_hide);
	}
	else
	{
		send_to_char("Pr�bujesz wtopi� si� w otoczenie, ale bezskutecznie." NL,
				ch);
		learn_from_failure(ch, gsn_hide);
	}
	WAIT_STATE(ch, 8);
	return;
}

/*
 * Contributed by Alander.
 */
DEF_DO_FUN( visible )
{
	affect_strip(ch, gsn_invis);
	affect_strip(ch, gsn_forceinvis);
	affect_strip(ch, gsn_sneak);
	REMOVE_BIT(ch->affected_by, AFF_HIDE);
	if (!IS_RACE(ch, "DEFEL")) /* Defel has perm invis */
		REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
	if (!IS_RACE(ch, "NOGHRI")) /* Noghri has perm sneak */
		REMOVE_BIT(ch->affected_by, AFF_SNEAK);
	REMOVE_BIT(ch->affected_by, AFF_FORCEINVIS);
	send_to_char("Ok." NL, ch);
	return;
}

DEF_DO_FUN( recall )
{
	ROOM_INDEX_DATA *location;

	location = NULL;
	location = get_room_index( ROOM_VNUM_TEMPLE);

//    location = get_room_index( wherehome(ch) );

	if (!can_edit(ch, ch->in_room->vnum))
	{
		send_to_char("No permission. Sorry." NL, ch);
		return;
	}

	if (!location)
	{
		send_to_char("Zupe�nie nie wiesz gdzie jeste�." NL, ch);
		return;
	}

	if (ch->in_room == location)
		return;

	if (IS_SET(ch->affected_by, AFF_CURSE))
	{
		send_to_char("You are cursed and cannot recall!\n\r", ch);
		return;
	}

	if ((who_fighting(ch)) != NULL)
	{

		ch_printf(ch, "You recall from combat!\n\r");
		stop_fighting(ch, true);
	}
	char_from_room(ch);
	char_to_room(ch, location);
	/*    if ( ch->mount )
	 {
	 char_from_room( ch->mount );
	 char_to_room( ch->mount, location );
	 }
	 */
	do_look(ch, (char*) "auto");
	return;
}

DEF_DO_FUN( aid )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int percent;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz uleczy�?" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		hes_not_here_msg(ch);
		return;
	}

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Siebie? Nie dasz rady." NL, ch);
		return;
	}

	if (victim->position > POS_STUNNED)
	{
		act( PLAIN, "$N nie potrzebuje chyba twojej pomocy.", ch, NULL, victim,
		TO_CHAR);
		return;
	}

	if (victim->hit <= -400)
	{
		act( PLAIN, "$N jest w takim stanie, �e tw�j wysi�ek p�jdzie na marne.",
				ch, NULL, victim, TO_CHAR);
		return;
	}

	ch->alignment = ch->alignment + 20;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);

	percent = number_percent() - (get_curr_lck(ch) - 13);
	WAIT_STATE(ch, skill_table[gsn_aid]->beats);
	if (!IS_NPC(ch) && percent > ch->pcdata->learned[gsn_aid])
	{
		send_to_char("Nie uda�o ci si�." NL, ch);
		learn_from_failure(ch, gsn_aid);
		return;
	}

	ch->alignment = ch->alignment + 20;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);

	act( COL_ACTION, "Przywracasz $N$2 przytomno��!", ch, NULL, victim,
			TO_CHAR);
	act( COL_ACTION, "$n przywraca $N$2 przytomno��!", ch, NULL, victim,
			TO_NOTVICT);
	learn_from_success(ch, gsn_aid);
	if (victim->hit < 1)
		victim->hit = 1;

	update_pos(victim);
	act( COL_ACTION, "$n przywraca ci przytomno��!", ch, NULL, victim, TO_VICT);
	return;
}

DEF_DO_FUN( mount )
{
	CHAR_DATA *victim;

	if (!IS_NPC(ch) && ch->pcdata->learned[gsn_mount] <= 0)
	{
		send_to_char(
				"Lepiej nie pr�buj tego dop�ki si� tego nie nauczysz..." NL,
				ch);
		return;
	}

	if (ch->mount)
	{
		act( PLAIN, "No przecie� ju� dosiadasz $N$3!", ch, NULL, ch->mount,
				TO_CHAR);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char("Kogo chcesz dosi���?\n\r", ch);
		return;
	}

	if ((victim = get_char_room(ch, argument)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego" NL, ch);
		return;
	}

	if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_MOUNTABLE))
	{
		act( PLAIN, "Nie mo�esz dosi��� $N$1!", ch, NULL, victim, TO_CHAR);
		return;
	}

	if (IS_SET(victim->act, ACT_MOUNTED))
	{
		send_to_char("Przykro mi. Zaj�te." NL, ch);
		return;
	}

	if (victim->position < POS_STANDING)
	{
		act( PLAIN, "$E musi sta�, inaczej nici z dosiadania.", ch, NULL,
				victim, TO_CHAR);
		return;
	}

	if (victim->position == POS_FIGHTING || victim->fighting)
	{
		send_to_char("Twoja ofiara za bardzo si� teraz wierzga." NL, ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_mount]->beats);
	if ( IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_mount])
	{
		SET_BIT(victim->act, ACT_MOUNTED);
		ch->mount = victim;
		act( COL_ACTION, "Dosiadasz $N$3.", ch, NULL, victim, TO_CHAR);
		act( COL_ACTION, "$n zr�cznie wskakuje na $N$3.", ch, NULL, victim,
				TO_NOTVICT);
		act( COL_ACTION, "$n dosiada ci�.", ch, NULL, victim, TO_VICT);
		learn_from_success(ch, gsn_mount);
		ch->position = POS_MOUNTED;
		fevent_trigger(ch, FE_POSITION);
	}
	else
	{
		act( COL_ACTION,
				"Twoja nieudolna pr�ba wskoczenia na $N$3 �le si� ko�czy, spadasz na �eb.",
				ch, NULL, victim, TO_CHAR);
		act( COL_ACTION, "$n nieudolnie pr�buje dosi��� $N$3.", ch, NULL,
				victim, TO_NOTVICT);
		act( COL_ACTION, "$n pr�buje ci� dosi���.", ch, NULL, victim, TO_VICT);
		learn_from_failure(ch, gsn_mount);
	}
	return;
}

DEF_DO_FUN( dismount )
{
	CHAR_DATA *victim;

	if ((victim = ch->mount) == NULL)
	{
		send_to_char("Nie dosiadasz nikogo." NL, ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_mount]->beats);
	if ( IS_NPC(ch) || number_percent() < ch->pcdata->learned[gsn_mount])
	{
		act( COL_ACTION, "Zsiadasz z $N$1.", ch, NULL, victim, TO_CHAR);
		act( COL_ACTION, "$n zr�cznie zeskakuje z $N$1.", ch, NULL, victim,
				TO_NOTVICT);
		act( COL_ACTION, "$n zeskakuje z ciebie. Ufff!", ch, NULL, victim,
				TO_VICT);
		REMOVE_BIT(victim->act, ACT_MOUNTED);
		ch->mount = NULL;
		ch->position = POS_STANDING;
		fevent_trigger(ch, FE_POSITION);
		learn_from_success(ch, gsn_mount);
	}
	else
	{
		act( COL_ACTION, "Spadasz na �eb pr�buj�c zsi��� z $N$1. Ouch!", ch,
				NULL, victim, TO_CHAR);
		act( COL_ACTION, "$n spada na �eb pr�buj�c zsi��� z $N$1.", ch, NULL,
				victim, TO_NOTVICT);
		act( COL_ACTION, "$n spada z twoich plec�w. Ufff!", ch, NULL, victim,
				TO_VICT);
		learn_from_failure(ch, gsn_mount);
		REMOVE_BIT(victim->act, ACT_MOUNTED);
		ch->mount = NULL;
		ch->position = POS_SITTING;
		fevent_trigger(ch, FE_POSITION);
		global_retcode = damage(ch, ch, 1, TYPE_UNDEFINED);
	}
	return;
}

/**************************************************************************/

/*
 * Check for parry.
 */
bool check_parry(CHAR_DATA *ch, CHAR_DATA *victim)
{
	int chances;
	OBJ_DATA *wield;

	if (!IS_AWAKE(victim))
		return false;

	if ( IS_NPC(victim)
			&& (!IS_SET(victim->defenses, DFND_PARRY) && number_percent() > 20))
		return false;

	if (IS_NPC(victim))
	{
		/* Tuan was here.  :)   *** so was Durga :p *** */
		/* And Thanos :P */
//	chances	= UMIN( 60, victim->skill_level[COMBAT_ABILITY]  );
		chances = UMIN(60, 2 * victim->top_level);
		if (!get_eq_char(victim, WEAR_WIELD))
		{
			if (!get_eq_char(victim, WEAR_DUAL_WIELD))
				chances /= 2;
			else
				chances = 3 * chances / 4;
		}
	}
	else
	{
		if ((wield = get_eq_char(victim, WEAR_WIELD)) == NULL)		    //
//	||   ( wield->value[3] != WEAPON_LIGHTSABER ) )
		{
			if ((wield = get_eq_char(victim, WEAR_DUAL_WIELD)) == NULL)
//	   ||   ( wield->value[3] != WEAPON_LIGHTSABER ) )
				return false;
			chances = (int) (victim->pcdata->learned[gsn_parry] / 4);
		}
		else
			chances = (int) (victim->pcdata->learned[gsn_parry] / 2);
	}

//    chances = URANGE ( 10 , chances , 90 );

	if (number_percent() >= chances + fight_lev(victim) - fight_lev(ch))
	{
		learn_from_failure(victim, gsn_parry);
		return false;
	}

	if (number_range(1, 2) == 1)
	{
		act( PLAIN, "Parujesz atak $n$1" PLAIN ".", ch, NULL, victim, TO_VICT);
		act( PLAIN, "$N" PLAIN " paruje tw�j atak.", ch, NULL, victim, TO_CHAR);
	}
	else
	{
		act( PLAIN, "Bezlito�nie odbijasz atak $n$1" PLAIN ".", ch, NULL,
				victim, TO_VICT);
		act( PLAIN, "$N" PLAIN " bezlito�nie odbija tw�j atak.", ch, NULL,
				victim, TO_CHAR);
	}
	learn_from_success(victim, gsn_parry);
	return true;
}

/*
 * Check for dodge.
 */
bool check_dodge(CHAR_DATA *ch, CHAR_DATA *victim)
{
	int chances;

	if (!IS_AWAKE(victim))
		return false;

	if ( IS_NPC(victim) && (!IS_SET(victim->defenses, DFND_DODGE))
			&& number_percent() > 20)
		return false;

	if (IS_NPC(victim))
		chances = UMIN(60, 2 * victim->top_level);
	else
		chances = (int) (victim->pcdata->learned[gsn_dodge] / 2);

	if (number_range(1, 100) > chances)
	{
		learn_from_failure(victim, gsn_dodge);
		return false;
	}

	if (number_range(1, 2) == 1)
	{
		act( PLAIN, "Uchylasz si� przed atakiem $n$1" PLAIN ".", ch, NULL,
				victim, TO_VICT);
		act( PLAIN, "$N" PLAIN " uchyla si� przed twoim atakiem.", ch, NULL,
				victim, TO_CHAR);
		act( PLAIN, "$N" PLAIN " uchyla si� przed atakiem $n$1.", ch, NULL,
				victim, TO_NOTVICT);
	}
	else
	{
		act( PLAIN, "Robisz szybki zw�d i $n" PLAIN " nie mo�e ci� trafi�.", ch,
				NULL, victim, TO_VICT);
		act( PLAIN,
				"$N" PLAIN " robi szybki zw�d,a tw�j atak trafia w powietrze.",
				ch, NULL, victim, TO_CHAR);
		act( PLAIN,
				"$N" PLAIN " robi szybki zw�d, a atak $n$1 trafia w powietrze.",
				ch, NULL, victim, TO_NOTVICT);
	}
	learn_from_success(victim, gsn_dodge);
	return true;
}

DEF_DO_FUN( poison_weapon )
{
	OBJ_DATA *obj;
	OBJ_DATA *pobj = nullptr;
	OBJ_DATA *wobj = nullptr;
	char arg[ MAX_INPUT_LENGTH];
	int percent;

	if (!IS_NPC(ch) && ch->pcdata->learned[gsn_poison_weapon] <= 0)
	{
		send_to_char("Ale ty nie wiesz jak to si� robi!" NL, ch);
		return;
	}

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Co chcesz zatru�?" NL, ch);
		return;
	}
	if (ch->fighting)
	{
		send_to_char(
				"Podczas walki? Zastan�w si� jak zatru� �ycie swojemu przeciwnikowi!" NL,
				ch);
		return;
	}
	if (ms_find_obj(ch))
		return;

	if (!(obj = get_obj_carry(ch, arg)))
	{
		send_to_char("Nie masz takiego przedmiotu" NL, ch);
		return;
	}
	if (obj->item_type != ITEM_WEAPON)
	{
		send_to_char("Ale� to nie jest bro�!" NL, ch);
		return;
	}

	if (obj->value[3] == WEAPON_LIGHTSABER)
	{
		send_to_char("Nie pr�buj takich sztuczek z mieczem swietlnym!" NL, ch);
		return;
	}

	if (IS_OBJ_STAT(obj, ITEM_POISONED))
	{
		send_to_char("Ta bro� jest ju� zatruta!" NL, ch);
		return;
	}
	/* Now we have a valid weapon...check to see if we have the powder. */
	for (auto* po : ch->carrying)
	{
		if (po->pIndexData->vnum == OBJ_VNUM_BLACK_POWDER)
		{
			pobj = po;
			break;
		}
	}
	if (!pobj)
	{
		send_to_char("Potrzebujesz czarnego truj�cego proszku." NL, ch);
		return;
	}
	/* Okay, we have the powder...do we have water? */
	for (auto* wo : ch->carrying)
	{
		if (wo->item_type == ITEM_DRINK_CON && wo->value[1] > 0
				&& wo->value[2] == 0)
		{
			wobj = wo;
			break;
		}
	}
	if (!wobj)
	{
		send_to_char("Proszek musisz zmiesza� z wod�. Nie masz jej." NL, ch);
		return;
	}
	/* And does the thief have steady enough hands? */
	if (!IS_NPC(ch) && (ch->pcdata->condition[COND_DRUNK] > 0))
	{
		send_to_char(
				"R�ce za bardzo ci si� trz�s�. W tym stanie nie zatrujesz niczego." NL,
				ch);
		return;
	}
	WAIT_STATE(ch, skill_table[gsn_poison_weapon]->beats);

	percent = (number_percent() - get_curr_lck(ch) - 14);

	/* Check the skill percentage */
	separate_obj(pobj);
	separate_obj(wobj);
	if (!IS_NPC(ch) && percent > ch->pcdata->learned[gsn_poison_weapon])
	{
		send_to_char("Nie uda�o ci si�. Trucizna wylewa si� na ciebie! Ou�!" NL,
				ch);
		damage(ch, ch, ch->skill_level[HUNTING_ABILITY], gsn_poison_weapon);
		act(PLAIN, "$n oblewa si� trucizn�!", ch, NULL, NULL, TO_ROOM);
		extract_obj(pobj);
		extract_obj(wobj);
		learn_from_failure(ch, gsn_poison_weapon);
		return;
	}
	separate_obj(obj);
	/* Well, I'm tired of waiting.  Are you? */
	act(FB_RED, "Mieszasz $p$3 w $P$5, tworzysz �mierteln� trucizn�!", ch, pobj,
			wobj, TO_CHAR);
	act(FB_RED, "$n miesza $p$3 w $P$5, tworzy �mierteln� trucizn�!", ch, pobj,
			wobj, TO_ROOM);
	act(FG_GREEN, "Nas�czasz $p$3 trucizn�. Widzisz ob�ok pary i s�yszysz syk!",
			ch, obj, NULL, TO_CHAR);
	act(FG_GREEN,
			"$n nas�cza $p$3 trucizn�. Widzisz ob�ok pary i s�yszysz syk!", ch,
			obj, NULL, TO_ROOM);
	SET_BIT(obj->extra_flags, ITEM_POISONED);
	obj->cost *= ch->skill_level[HUNTING_ABILITY] / 2;
	/* Set an object timer.  Don't want proliferation of poisoned weapons */
	obj->timer = 10 + ch->skill_level[HUNTING_ABILITY];

	if (IS_OBJ_STAT(obj, ITEM_BLESS))
		obj->timer *= 2;

	if (IS_OBJ_STAT(obj, ITEM_FORCE))
		obj->timer *= 2;

	/* WHAT?  All of that, just for that one bit?  How lame. ;) */
	act(FB_BLUE, "Resztki trucizny prze�eraj� $p$3.", ch, wobj, NULL, TO_CHAR);
	act(FB_BLUE, "Resztki trucizny prze�eraj� $p$3.", ch, wobj, NULL, TO_ROOM);
	extract_obj(pobj);
	extract_obj(wobj);
	learn_from_success(ch, gsn_poison_weapon);
	return;
}

DEF_DO_FUN( scribe )
{
}

bool check_grip(CHAR_DATA *ch, CHAR_DATA *victim)
{
	int chance;

	if (!IS_AWAKE(victim))
		return false;

	if ( IS_NPC(victim) && !IS_SET(victim->defenses, DFND_GRIP))
		return false;

	if (IS_NPC(victim))
		chance = UMIN(60, 2 * victim->top_level);
	else
		chance = (int) (victim->pcdata->learned[gsn_grip] / 2);

	/* Consider luck as a factor */
	chance += (2 * (get_curr_lck(victim) - 13));

	if (number_percent() >= chance + victim->top_level - ch->top_level)
	{
		learn_from_failure(victim, gsn_grip);
		return false;
	}
	act( COL_ACTION, "Robisz odskok i $n nie mo�e ci� rozbroi�.", ch, NULL,
			victim, TO_VICT);
	act( COL_ACTION,
			"$N trzyma bro� z ca�ych si�. Odskakuje i nie udaje ci si�.", ch,
			NULL, victim, TO_CHAR);
	learn_from_success(victim, gsn_grip);
	return true;
}

DEF_DO_FUN( circle )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	int percent;

	if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
	{
		concentrate_msg(ch);
		return;
	}

	one_argument(argument, arg);

	if (ch->mount)
	{
		mount_msg(ch);
		return;
	}

	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz zaj�� od ty�u?" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		hes_not_here_msg(ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Zachodzisz si� w g�ow� jakby tu zaj�� si� od ty�u." NL,
				ch);
		return;
	}

	if (is_safe(ch, victim))
		return;

	if ((obj = get_eq_char(ch, WEAR_WIELD)) == NULL
			|| (obj->value[3] != 11 && obj->value[3] != 2))
	{
		send_to_char("Potrzebujesz kr�tkiej bia�ej broni..." NL, ch);
		return;
	}

	if (!ch->fighting)
	{
		notfight_msg(ch);
		return;
	}

	if (!victim->fighting)
	{
		send_to_char(
				"Nie zakradniesz si� dop�ki twoja ofiara nie zacznie walki." NL,
				ch);
		return;
	}

	if (victim->num_fighting < 2)
	{
		act( PLAIN,
				"W walce bierze udzia� zbyt ma�o os�b. Nie uda ci si� skorzysta� z nieuwagi." NL,
				ch, NULL, victim, TO_CHAR);
		return;
	}

	if (get_curr_dex(victim) > get_curr_dex(ch))
	{
		send_to_char("Nie zakradniesz si�, twoja ofiara jest zbyt szybka." NL,
				ch);
		return;
	}

	percent = number_percent() - (get_curr_lck(ch) - 16)
			+ (get_curr_lck(victim) - 13);

	WAIT_STATE(ch, skill_table[gsn_circle]->beats);
	if (percent
			< (IS_NPC(ch) ?
					(ch->skill_level[HUNTING_ABILITY] * 1.5) :
					ch->pcdata->learned[gsn_circle]))
	{
		learn_from_success(ch, gsn_circle);
		global_retcode = multi_hit(ch, victim, gsn_circle);

	}
	else
	{
		learn_from_failure(ch, gsn_circle);
		global_retcode = damage(ch, victim, 0, gsn_circle);
	}
	return;
}

/* Berserk and HitAll. -- Altrag */
DEF_DO_FUN( berserk )
{
	int percent;
	AFFECT_DATA af;

	if (!ch->fighting)
	{
		notfight_msg(ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_BERSERK))
	{
		send_to_char("Twoja w�ciek�o�� jest ju� NiEoGrAnIcZoNa!" NL, ch);
		return;
	}

	percent = IS_NPC(ch) ? 80 : ch->pcdata->learned[gsn_berserk];
	WAIT_STATE(ch, skill_table[gsn_berserk]->beats);
	if (!chance(ch, percent))
	{
		send_to_char(
				"Zbierasz w sobie ca�y gniew i... nic jako� si� nie dzieje." NL,
				ch);
		act(PLAIN, "$n zbiera sw�j ca�y gniew i... Jako� nic si� nie dzieje.",
				ch, NULL, NULL, TO_ROOM);
		learn_from_failure(ch, gsn_berserk);
		return;
	}
	af.type = gsn_berserk;
	/* Hmmm.. 10-20 combat rounds at level 50.. good enough for most mobs,
	 and if not they can always go berserk again.. shrug.. maybe even
	 too high. -- Altrag */
	af.duration = number_range(ch->top_level / 5, ch->top_level * 2 / 5);
	/* Hmm.. you get stronger when yer really enraged.. mind over matter
	 type thing.. */
	af.location = APPLY_STR;
	af.modifier = 1;
	af.bitvector = AFF_BERSERK;
	affect_to_char(ch, &af);
	send_to_char("Zbierasz w sobie ca�y gniew i... Tracisz nad sob� kontrol�! "
	FB_RED "KRWII!" EOL, ch);
	act(PLAIN, "$n zbiera sw�j ca�y gniew i... Wpada w SzA�!.", ch, NULL, NULL,
			TO_ROOM);
	learn_from_success(ch, gsn_berserk);
	return;
}

/* External from fight.c */
ch_ret one_hit args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
DEF_DO_FUN( hitall )
{
	int nvict = 0;
	int nhit = 0;
	int percent;

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char("Nie mo�esz zrobi� tego tutaj." NL, ch);
		return;
	}

	if (ch->in_room->people.empty())
	{
		send_to_char("Ale� tu nikogo nie ma!" NL, ch);
		return;
	}
	percent = IS_NPC(ch) ? 80 : ch->pcdata->learned[gsn_hitall];
	auto people_snap = ch->in_room->people;
	for (auto* vch : people_snap)
	{
		if (is_same_group(ch, vch) || !is_legal_kill(ch, vch)
				|| !can_see(ch, vch) || is_safe(ch, vch))
			continue;
		if (++nvict > ch->skill_level[COMBAT_ABILITY] / 5)
			break;
		if (chance(ch, percent))
		{
			nhit++;
			global_retcode = one_hit(ch, vch, TYPE_UNDEFINED);
		}
		else
			global_retcode = damage(ch, vch, 0, TYPE_UNDEFINED);
		/* Fireshield, etc. could kill ch too.. :>.. -- Altrag */
		if (global_retcode == rCHAR_DIED || global_retcode == rBOTH_DIED
				|| char_died(ch))
			return;
	}
	if (!nvict)
	{
		send_to_char("Ale� tu nikogo nie ma!" NL, ch);
		return;
	}
	ch->move = UMAX(0, ch->move - nvict * 3 + nhit);
	if (nhit)
		learn_from_success(ch, gsn_hitall);
	else
		learn_from_failure(ch, gsn_hitall);
	return;
}

bool check_illegal_psteal(CHAR_DATA *ch, CHAR_DATA *victim)
{
	return false;
}

DEF_DO_FUN( scan )
{
	ROOM_INDEX_DATA *was_in_room;
	ROOM_INDEX_DATA *to_room;
	EXIT_DATA *pexit;
	int dir = -1;
	int dist;
	int max_dist = 5;

	if (argument[0] == '\0')
	{
		send_to_char("Jaki kierunek chcesz przeskanowa�?" NL, ch);
		return;
	}

	if ((dir = get_door(argument)) == -1)
	{
		send_to_char("Skanowa� JAKI kierunek?" NL, ch);
		return;
	}

	was_in_room = ch->in_room;
	act( PLAIN, "Skanujesz kierunek $t...", ch, dir_type_name[dir], NULL,
			TO_CHAR);
	act( PLAIN, "$n skanuje kierunek $t.", ch, dir_type_name[dir], NULL,
			TO_ROOM);

	if ( IS_NPC( ch ) || (number_percent() > ch->pcdata->learned[gsn_scan]))
	{
		act( PLAIN,
				"Przestajesz skanowa� kierunek $t, obraz jest jaki� zamazany.",
				ch, dir_type_name[dir], NULL, TO_CHAR);
		learn_from_failure(ch, gsn_scan);
		return;
	}

	if (!(pexit = get_exit(ch->in_room, dir)) || !CAN_ENTER(ch, pexit->to_room))
	{
		act( PLAIN, "Nie widzisz tu $t.", ch, dir_name[dir], NULL, TO_CHAR);
		return;
	}

	if (ch->top_level < 50)
		max_dist--;
	if (ch->top_level < 20)
		max_dist--;

	for (dist = 1; dist <= max_dist;)
	{
		if (IS_SET(pexit->flags, EX_CLOSED))
		{
			if (IS_SET(pexit->flags, EX_SECRET))
				act( PLAIN, "Widok na $t blokuje �ciana.", ch,
						dir_where_name[dir], NULL, TO_CHAR);
			else
				act( PLAIN, "Widok na $t blokuj� drzwi.", ch,
						dir_where_name[dir], NULL, TO_CHAR);
			break;
		}

		to_room = NULL;
		if (pexit->distance > 1)
			to_room = generate_exit(ch->in_room, &pexit);

		if (to_room == NULL)
			to_room = pexit->to_room;

		if (!CAN_ENTER(ch, to_room))
			break;

		if (room_is_private(ch, to_room) && get_trust(ch) < LEVEL_GREATER)
		{
			act( PLAIN,
					"Na $t st�d jest prywatne pomieszczenie. Kto� je dyskretnie skrywa.",
					ch, dir_where_name[dir], NULL, TO_CHAR);
			break;
		}
		char_from_room(ch);
		char_to_room(ch, to_room);
		send_to_char( COL_RMNAME, ch);
		send_to_char(ch->in_room->name, ch);
		send_to_char( EOL, ch);
		show_list_to_char(ch->in_room->contents, ch, false, false);
		show_char_to_char(ch->in_room->people, ch);

		switch (ch->in_room->sector_type)
		{
		default:
			dist++;
			break;
		case SECT_AIR:
			if (number_percent() < 80)
				dist++;
			break;
		case SECT_INSIDE:
		case SECT_FIELD:
		case SECT_UNDERGROUND:
			dist++;
			break;
		case SECT_FOREST:
		case SECT_CITY:
		case SECT_DESERT:
		case SECT_HILLS:
			dist += 2;
			break;
		case SECT_WATER_SWIM:
		case SECT_WATER_NOSWIM:
			dist += 3;
			break;
		case SECT_MOUNTAIN:
		case SECT_UNDERWATER:
		case SECT_OCEANFLOOR:
			dist += 4;
			break;
		}

		if (dist >= max_dist)
		{
			act( PLAIN, "Obraz zamazuje si� i nie widzisz nic dalej.", ch, NULL,
					NULL, TO_CHAR);
			break;
		}
		if ((pexit = get_exit(ch->in_room, dir)) == NULL)
		{
			act( PLAIN, "Dalszy widok na $t blokuje �ciana.", ch,
					dir_where_name[dir], NULL, TO_CHAR);
			break;
		}
	}

	char_from_room(ch);
	char_to_room(ch, was_in_room);
	learn_from_success(ch, gsn_scan);

	return;
}

DEF_DO_FUN( slice )
{
}

DEF_DO_FUN( skedit )
{
	SKILLTYPE *pSkill;
	int sn;
	char arg1[MIL];

	argument = one_argument(argument, arg1);

	if (*arg1 == '\0')
	{
		send_to_char("Syntax: skedit <skillname | herbname>" NL
		"Syntax: skedit create skill | herb <name>" NL, ch);
		return;
	}

	if (!str_cmp(arg1, "create"))
	{
		char arg2[MIL];
		int type, max, x;

		argument = one_argument(argument, arg2);

		if (*argument == '\0')
		{
			send_to_char("Syntax: skedit create skill | herb <name>" NL, ch);
			return;
		}
		if (!str_cmp(arg2, "herb"))
		{
			type = SKILL_HERB;
			if (top_herb >= MAX_HERB)
			{
				ch_printf(ch,
						"top_herb == %d == MAX_HERB. Trzeba zwiekszyc MAX_HERB w mud.h!" NL,
						top_herb);
				return;
			}
		}
		else if (!str_cmp(arg2, "skill"))
		{
			type = SKILL_UNKNOWN;
			if (top_sn >= MAX_SKILL)
			{
				ch_printf(ch,
						"top_sn == %d == MAX_SKILL. Trzeba zwiekszyc MAX_SKILL w mud.h!" NL,
						top_sn);
				return;
			}
		}
		else
		{
			send_to_char("Syntax: skedit create skill | herb <name>" NL, ch);
			return;
		}

		CREATE(pSkill, SKILLTYPE, 1);
		if (type == SKILL_HERB)
		{
			herb_table[top_herb++] = pSkill;
			for (max = x = 0; x < top_herb - 1; x++)
				if (herb_table[x] && herb_table[x]->slot > max)
					max = herb_table[x]->slot;
			pSkill->slot = max + 1;
		}
		else
		{
			skill_table[top_sn++] = pSkill;
			for (max = x = 0; x < top_sn - 1; x++)
				if (skill_table[x] && skill_table[x]->slot > max)
					max = skill_table[x]->slot;
			pSkill->slot = max + 1;
		}
		STRDUP(pSkill->name, argument);
		STRDUP(pSkill->noun_damage, "");
		STRDUP(pSkill->msg_off, "");
		pSkill->spell_fun = spell_smaug;
		pSkill->type = type;

		ch->desc->olc_editing = (void*) pSkill;
		ch->desc->connected = CON_SKEDITOR;
		skedit_show(ch, pSkill,
				pSkill->type == SKILL_HERB ? top_herb - 1 : top_sn - 1);
		if (pSkill->type == SKILL_HERB)
			send_to_char("Herb created." NL, ch);
		else
			send_to_char("Skill created." NL, ch);
		return;
	}
	if (arg1[0] == 'h' && is_number(arg1 + 1))
	{
		sn = atoi(arg1 + 1);
		if (!IS_VALID_HERB(sn))
		{
			send_to_char("Invalid herb." NL, ch);
			return;
		}
		pSkill = herb_table[sn];
	}
	else if ( is_number(arg1))
	{
		sn = atoi(arg1);
		if (!IS_VALID_SN(sn))
		{
			send_to_char("Invalid skill." NL, ch);
			return;

		}
		pSkill = skill_table[sn];
	}
	else if (!is_number(arg1))
	{
		if ((sn = skill_lookup(arg1)) >= 0)
			pSkill = skill_table[sn];
		else if ((sn = herb_lookup(arg1)) >= 0)
			pSkill = herb_table[sn];
		else
		{
			send_to_char("No such skill or herb." NL, ch);
			return;
		}
	}
	else
	{
		bug("do_skedit: no such skill or herb.");
		send_to_char("No such skill or herb." NL, ch);
		return;
	}

	ch->desc->olc_editing = (void*) pSkill;
	ch->desc->connected = CON_SKEDITOR;
	skedit_show(ch, pSkill, sn);

	return;
}

void skedit(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = d->character;
	SKILLTYPE *pSkill;
	char arg[MIL];
	char arg1[MIL];
	int sn;
	bool found = false;

	if (!(pSkill = (SKILLTYPE*) ch->desc->olc_editing))
	{
		bug("skedit: null pSkill.");
		edit_done(ch, (char*) "");
		return;
	}

	strcpy(arg, argument);
	argument = one_argument(argument, arg1);

	while (!found)
	{
		for (sn = 0; sn < top_sn; sn++)
			if (pSkill == skill_table[sn])
			{
				found = true;
				break;
			}
		if (found)
			break;
		for (sn = 0; sn < top_herb; sn++)
			if (pSkill == herb_table[sn])
			{
				found = true;
				break;
			}
		break;
	}

	if (!found)
	{
		bug("skedit: sn for pSkill not found.");
		edit_done(ch, (char*) "");
		return;
	}

	if (!str_prefix(arg1, "show"))
	{
		skedit_show(ch, pSkill, sn);
		return;
	}
	GET_DONE();
	GET_HELP(argument);
	GET_CREATE(do_skedit(ch, arg));
	if (!str_cmp(arg1, "help"))
	{
		char buf[MSL];

		sprintf(buf,
				"Syntax: create skill | herb <name>" NL
				"Syntax: <field> <value>" NL NL
				"Fields:" NL
				" name code target minpos slot mana beats dammsg wearoff guild minlevel" NL
				" type damtype acttype classtype powertype flag dice value difficulty" NL
				" addaffect rmaffect hitchar hitvict hitroom misschar missvict missroom" NL
				" diechar dievict dieroom immchar immvict immroom components teachers" NL
				" save list" NL NL
				"Addaffect syntax: addaffect <location> <modfifier> [duration] [bitvector]" NL NL
				"See: ? apply_types for location and: ? affects for bitvector." NL);
		send_to_char(buf, ch);
		return;
	}
	if (!str_cmp(arg1, "save"))
	{
		if (!str_prefix(argument, "skills"))
		{
			save_skill_table();
			send_to_char("Skills saved." NL, ch);
		}
		else if (!str_prefix(argument, "herbs"))
		{
			save_herb_table();
			send_to_char("Herbs saved." NL, ch);
		}
		else
			send_to_char("Syntax: save skills | herbs" NL, ch);
		return;
	}
	/* Trog: kiedys dokoncze
	 if( !str_cmp( arg1, "delete" ) )
	 {
	 int x;

	 if( pSkill->type == SKILL_HERB )
	 {
	 for( x = 0; x < top_herb; x++ )
	 if( herb_table[x] == pSkill )
	 {
	 herb_table[x] = NULL;
	 if( x == top_herb-1 )
	 top_herb--;
	 break;
	 }
	 for( ; x < top_herb-1; x++ )
	 if( !herb_table[x] && herb_table[x+1] )
	 {
	 herb_table[x] = herb_table[x+1];
	 herb_table[x+1] = NULL;
	 }

	 }
	 else
	 {
	 for( x = 0; x < top_sn; x++ )
	 if( skill_table[x] == pSkill )
	 {
	 skill_table[x] = NULL;
	 if( x == top_sn-1 )
	 top_sn--;
	 break;
	 }
	 for( ; x < top_sn-1; x++ )
	 if( !skill_table[x] && skill_table[x+1] )
	 {
	 skill_table[x] = skill_table[x+1];
	 skill_table[x+1] = NULL;
	 }
	 }
	 free_skill( pSkill );
	 edit_done( ch, "" );
	 return;
	 }
	 */

	if (!str_cmp(arg1, "list"))
	{
		if (!str_prefix(argument, "skills"))
		{
			pager_printf(ch,
					FB_WHITE " Sn  Slot  Skill/Spell          Damtype   Beats  Gld  Lvl  Type" EOL
					FB_WHITE "---------------------------------------------------------------" EOL);
			for (sn = 0; sn < top_sn && IS_VALID_SN(sn); sn++)
			{
				SKILLTYPE *skill = skill_table[sn];

				pager_printf(ch,
						"%4d %4d  %-20.20s %-10.10s  %-3d  %3.3s  %3d  %-7.7s" NL,
						sn, skill->slot, skill->name,
						spell_damage[SPELL_DAMAGE(skill)], skill->beats,
						(skill->guild >= 0 && skill->guild < MAX_ABILITY) ?
								class_table[skill->guild].who_name : "---",
						skill->min_level,
						skill_tname_orig[skill->type] ?
								skill_tname_orig[skill->type] : "----");
			}
			return;
		}
		else if (!str_prefix(argument, "herbs"))
		{
			for (sn = 0; sn < top_herb && IS_VALID_HERB(sn); sn++)
				pager_printf(ch, COL_SCORE "%2d." PLAIN " %s" NL, sn,
						herb_table[sn]->name);
			return;
		}
		else
			send_to_char("Syntax: list skills | herbs" NL, ch);
		return;
	}
	if (!str_cmp(arg1, "difficulty"))
	{
		pSkill->difficulty = atoi(argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "participants"))
	{
		pSkill->participants = atoi(argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "alignment"))
	{
		pSkill->alignment = atoi(argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "forcetype"))
	{
		int f = atoi(argument);
		if (f < 0 || f > FORCE_MAX - 1)
		{
			ch_printf(ch, "Forcetype range is: 0 - %d" NL, FORCE_MAX - 1);
			return;
		}
		pSkill->forcetype = f;
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "damtype"))
	{
		int x = get_sdamage(argument);

		if (x == -1)
			send_to_char("Invalid spell damage type." NL, ch);
		else
		{
			SET_SDAM(pSkill, x);
			donemsg(ch);
		}
		return;
	}
	if (!str_cmp(arg1, "acttype"))
	{
		int x = get_saction(argument);

		if (x == -1)
			send_to_char("Invalid spell action type." NL, ch);
		else
		{
			SET_SACT(pSkill, x);
			donemsg(ch);
		}
		return;
	}
	if (!str_cmp(arg1, "classtype"))
	{
		int x = get_sclass(argument);

		if (x == -1)
			send_to_char("Invalid spell class type." NL, ch);
		else
		{
			SET_SCLA(pSkill, x);
			donemsg(ch);
		}
		return;
	}
	if (!str_cmp(arg1, "powertype"))
	{
		int x = get_spower(argument);

		if (x == -1)
			send_to_char("Invalid spell power type." NL, ch);
		else
		{
			SET_SPOW(pSkill, x);
			donemsg(ch);
		}
		return;
	}
	if (!str_cmp(arg1, "flag"))
	{
		int i, x = get_sflag(argument);

		if (!str_cmp(argument, "list"))
		{
			send_to_char("Flags:" NL " ", ch);
			for (i = 0; i < 18; i++)
			{
				ch_printf(ch, " %-15s", spell_flag[i]);
				if (i % 2 == 0)
					send_to_char("\t", ch);
				else
					send_to_char( NL " ", ch);
			}
			return;
		}

		if (x == -1)
			send_to_char("Invalid spell flag." NL, ch);
		else
		{
			TOGGLE_BIT(pSkill->flags, 1 << (x + 11));
			donemsg(ch);
		}
		return;
	}
	if (!str_cmp(arg1, "saves"))
	{
		int x = get_ssave(argument);

		if (x == -1)
			send_to_char("Not a saving type." NL, ch);
		else
		{
			pSkill->saves = x;
			donemsg(ch);
		}
		return;
	}
	if (!str_cmp(arg1, "code"))
	{
		SPELL_FUN *spellfun;
		DO_FUN *dofun;

		if ((spellfun = spell_function(argument)) != spell_notfound)
		{
			pSkill->spell_fun = spellfun;
			pSkill->skill_fun = NULL;
		}
		else if ((dofun = skill_function(argument)) != skill_notfound)
		{
			pSkill->skill_fun = dofun;
			pSkill->spell_fun = NULL;
		}
		else
		{
			send_to_char("Invalid spell or skill." NL, ch);
			return;
		}
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "target"))
	{
		int x = get_starget(argument);

		if (x == -1)
			send_to_char("Invalid target type." NL, ch);
		else
		{
			pSkill->target = x;
			donemsg(ch);
		}
		return;
	}
	if (!str_cmp(arg1, "minpos"))
	{
		int position = flag_value(position_types_list, argument);

		if (position < 0 || position > POS_DRAG)
		{
			ch_printf(ch, "Invalid position." NL);
			return;
		}

		pSkill->minimum_position = position;
		send_to_char("Minimum position set." NL, ch);
		return;
	}
	if (!str_cmp(arg1, "minlevel") || !str_cmp(arg1, "level"))
	{
		pSkill->min_level = URANGE(1, atoi(argument), MAX_LEVEL);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "slot"))
	{
		pSkill->slot = URANGE(0, atoi(argument), 30000);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "mana"))
	{
		pSkill->min_mana = URANGE(0, atoi(argument), 2000);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "beats"))
	{
		pSkill->beats = URANGE(0, atoi(argument), 120);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "guild"))
	{
		pSkill->guild = atoi(argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "value"))
	{
		pSkill->value = atoi(argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "type"))
	{
		pSkill->type = get_skill(argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "rmaffect"))
	{
		SMAUG_AFF *aff = pSkill->affects;
		SMAUG_AFF *aff_next;
		int num = atoi(argument);
		int cnt = 1;

		if (!aff)
		{
			send_to_char("This spell has no special affects to remove." NL, ch);
			return;
		}
		if (num == 1)
		{
			pSkill->affects = aff->next;
			free_smaug_aff(aff);
			send_to_char("Affect removed." NL, ch);
			return;
		}
		for (; aff; aff = aff->next)
			if (++cnt == num && (aff_next = aff->next) != NULL)
			{
				aff->next = aff_next->next;
				free_smaug_aff(aff_next);
				send_to_char("Affect removed." NL, ch);
				return;
			}
		send_to_char("Affect not found." NL, ch);
		return;
	}
	/*
	 * affect <location> <modifier> <duration> <bitvector>
	 */
	if (!str_cmp(arg1, "affect") || !str_cmp(arg1, "addaffect"))
	{
		char location[MIL];
		char modifier[MIL];
		char duration[MIL];
		char bitvector[MIL];
		int loc, bit, tmpbit;
		SMAUG_AFF *aff;

		argument = one_argument(argument, location);
		argument = one_argument(argument, modifier);
		argument = one_argument(argument, duration);

		if (location[0] == '!')
			loc = flag_value(apply_types_list, location + 1) + REVERSE_APPLY;
		else
			loc = flag_value(apply_types_list, location);
		if ((loc % REVERSE_APPLY) < 0
				|| (loc % REVERSE_APPLY) >= MAX_APPLY_TYPE)
		{
			send_to_char("Unknown affect location. See AFFECTTYPES." NL, ch);
			return;
		}
		bit = 0;
		while (argument[0] != 0)
		{
			argument = one_argument(argument, bitvector);
			if ((tmpbit = flag_value(aff_flags_list, bitvector)) == -1)
				ch_printf(ch, "Unknown bitvector: %s. See AFFECTED_BY" NL,
						bitvector);
			else
				bit |= (tmpbit);
		}
		CREATE(aff, SMAUG_AFF, 1);
		if (!str_cmp(duration, "0"))
			duration[0] = '\0';
		if (!str_cmp(modifier, "0"))
			modifier[0] = '\0';
		STRDUP(aff->duration, duration);
		aff->location = loc;
		STRDUP(aff->modifier, modifier);
		aff->bitvector = bit;
		aff->next = pSkill->affects;
		pSkill->affects = aff;
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "name"))
	{
		STRDUP(pSkill->name, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "dammsg"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->noun_damage, "");
		else
			STRDUP(pSkill->noun_damage, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "wearoff"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->msg_off, "");
		else
			STRDUP(pSkill->msg_off, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "hitchar"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->hit_char, "");
		else
			STRDUP(pSkill->hit_char, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "hitvict"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->hit_vict, "");
		else
			STRDUP(pSkill->hit_vict, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "hitroom"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->hit_room, "");
		else
			STRDUP(pSkill->hit_room, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "misschar"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->miss_char, "");
		else
			STRDUP(pSkill->miss_char, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "missvict"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->miss_vict, "");
		else
			STRDUP(pSkill->miss_vict, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "missroom"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->miss_room, "");
		else
			STRDUP(pSkill->miss_room, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "diechar"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->die_char, "");
		else
			STRDUP(pSkill->die_char, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "dievict"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->die_vict, "");
		else
			STRDUP(pSkill->die_vict, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "dieroom"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->die_room, "");
		else
			STRDUP(pSkill->die_room, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "immchar"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->imm_char, "");
		else
			STRDUP(pSkill->imm_char, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "immvict"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->imm_vict, "");
		else
			STRDUP(pSkill->imm_vict, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "immroom"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->imm_room, "");
		else
			STRDUP(pSkill->imm_room, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "dice"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->dice, "");
		else
			STRDUP(pSkill->dice, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "components"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->components, "");
		else
			STRDUP(pSkill->components, argument);
		donemsg(ch);
		return;
	}
	if (!str_cmp(arg1, "teachers"))
	{
		if (!str_cmp(argument, "clear"))
			STRDUP(pSkill->teachers, "");
		else
			STRDUP(pSkill->teachers, argument);
		donemsg(ch);
		return;
	}

	interpret(ch, arg);
	return;
}

void skedit_show(CHAR_DATA *ch, SKILLTYPE *skill, int sn)
{
	char buf[MAX_STRING_LENGTH];
	SMAUG_AFF *aff;
	int cnt = 0;

	ch_printf(ch, COL_SCORE "Sn:" PLAIN " %4d " COL_SCORE "Slot:"
	PLAIN " %4d" COL_SCORE ": '" PLAIN "%-20s" COL_SCORE "'" EOL, sn,
			skill->slot, skill->name);
	if (skill->flags)
	{
		int64 x;

		ch_printf(ch,
				COL_SCORE "Damtype:" PLAIN " %s " NL COL_SCORE "Acttype:" PLAIN " %s " NL COL_SCORE "Classtype:" PLAIN
				" %s " NL COL_SCORE "Powertype:" PLAIN " %s" NL,
				spell_damage[SPELL_DAMAGE(skill)],
				spell_action[SPELL_ACTION(skill)],
				spell_class[SPELL_CLASS(skill)],
				spell_power[SPELL_POWER(skill)]);
		strcpy(buf, COL_SCORE "Flags:" PLAIN);
		for (x = 11; x < 32; x++)
			if (SPELL_FLAG(skill, INT64_1 << x))
			{
				strcat(buf, " ");
				strcat(buf, spell_flag[x - 11]);
			}
		strcat(buf, NL);
		send_to_char(buf, ch);
	}
	ch_printf(ch, COL_SCORE "Saves:" PLAIN " %s" NL,
			spell_saves[(int) skill->saves]);

	if (skill->difficulty != '\0')
		ch_printf(ch, COL_SCORE "Difficulty:" PLAIN " %d" NL,
				(int) skill->difficulty);

	ch_printf(ch,
			COL_SCORE "Type:" PLAIN " %s " NL COL_SCORE "Target:" PLAIN " %s " NL COL_SCORE
			"Minpos:" PLAIN " %s " NL COL_SCORE "Mana:" PLAIN " %d " NL COL_SCORE "Beats:" PLAIN " %d" NL,
			skill_tname_orig[skill->type],
			target_type[URANGE(TAR_IGNORE, skill->target, TAR_OBJ_INV)],
			flag_string(position_types_list, skill->minimum_position),
			skill->min_mana, skill->beats);
	ch_printf(ch,
			COL_SCORE "Flags:" PLAIN " %lld  " COL_SCORE "Guild:" PLAIN " %d  " COL_SCORE "Code:" PLAIN " %s" NL,
			skill->flags, skill->guild,
			skill->skill_fun ?
					skill_name(skill->skill_fun) :
					spell_name(skill->spell_fun));
	ch_printf(ch,
			COL_SCORE "Dammsg:" PLAIN " %s" NL COL_SCORE "Wearoff:" PLAIN " %s" NL,
			skill->noun_damage,
			skill->msg_off ? skill->msg_off : PLAIN "(none)");
	if (skill->dice && skill->dice[0] != '\0')
		ch_printf(ch, COL_SCORE "Dice:" PLAIN " %s" NL, skill->dice);
	if (skill->teachers && skill->teachers[0] != '\0')
		ch_printf(ch, COL_SCORE "Teachers:" PLAIN " %s" NL, skill->teachers);
	if (skill->components && skill->components[0] != '\0')
		ch_printf(ch, COL_SCORE "Components:" PLAIN " %s" NL,
				skill->components);
	if (skill->participants)
		ch_printf(ch, COL_SCORE "Participants:" PLAIN " %d" NL,
				(int) skill->participants);
	if (skill->userec.num_uses)
		send_timer(&skill->userec, ch);
	for (aff = skill->affects; aff; aff = aff->next)
	{
		if (aff == skill->affects)
			send_to_char( NL, ch);
		sprintf(buf, COL_SCORE "Affect:" PLAIN " %d", ++cnt);
		if (aff->location)
		{
			strcat(buf, COL_SCORE " modifies " PLAIN);
			strcat(buf, affect_loc_name(aff->location));
			strcat(buf, COL_SCORE " by '" PLAIN);
			strcat(buf, aff->modifier);
			if (aff->bitvector)
				strcat(buf, COL_SCORE "' and" PLAIN);
			else
				strcat(buf, COL_SCORE "'" PLAIN);
		}
		if (aff->bitvector)
		{
			int64 x;

			strcat(buf, COL_SCORE " applies" PLAIN);
			for (x = 0; x < 64; x++)
				if (IS_SET(aff->bitvector, INT64_1 << x))
				{
					strcat(buf, " ");
					strcat(buf, aff_flags_list[x].name);
				}
		}
		if (aff->duration[0] != '\0' && aff->duration[0] != '0')
		{
			strcat(buf, COL_SCORE " for " PLAIN "'");
			strcat(buf, aff->duration);
			strcat(buf, COL_SCORE "' rounds" PLAIN);
		}
		if (aff->location >= REVERSE_APPLY)
			strcat(buf, PLAIN " (affects caster only)");
		strcat(buf, NL);
		send_to_char(buf, ch);
		if (!aff->next)
			send_to_char("" NL, ch);
	}
	if (skill->hit_char && skill->hit_char[0] != '\0')
		ch_printf(ch, COL_SCORE "Hitchar   :" PLAIN " %s" NL, skill->hit_char);
	if (skill->hit_vict && skill->hit_vict[0] != '\0')
		ch_printf(ch, COL_SCORE "Hitvict   :" PLAIN " %s" NL, skill->hit_vict);
	if (skill->hit_room && skill->hit_room[0] != '\0')
		ch_printf(ch, COL_SCORE "Hitroom   :" PLAIN " %s" NL, skill->hit_room);
	if (skill->miss_char && skill->miss_char[0] != '\0')
		ch_printf(ch, COL_SCORE "Misschar  :" PLAIN " %s" NL, skill->miss_char);
	if (skill->miss_vict && skill->miss_vict[0] != '\0')
		ch_printf(ch, COL_SCORE "Missvict  :" PLAIN " %s" NL, skill->miss_vict);
	if (skill->miss_room && skill->miss_room[0] != '\0')
		ch_printf(ch, COL_SCORE "Missroom  :" PLAIN " %s" NL, skill->miss_room);
	if (skill->die_char && skill->die_char[0] != '\0')
		ch_printf(ch, COL_SCORE "Diechar   :" PLAIN " %s" NL, skill->die_char);
	if (skill->die_vict && skill->die_vict[0] != '\0')
		ch_printf(ch, COL_SCORE "Dievict   :" PLAIN " %s" NL, skill->die_vict);
	if (skill->die_room && skill->die_room[0] != '\0')
		ch_printf(ch, COL_SCORE "Dieroom   :" PLAIN " %s" NL, skill->die_room);
	if (skill->imm_char && skill->imm_char[0] != '\0')
		ch_printf(ch, COL_SCORE "Immchar   :" PLAIN " %s" NL, skill->imm_char);
	if (skill->imm_vict && skill->imm_vict[0] != '\0')
		ch_printf(ch, COL_SCORE "Immvict   :" PLAIN " %s" NL, skill->imm_vict);
	if (skill->imm_room && skill->imm_room[0] != '\0')
		ch_printf(ch, COL_SCORE "Immroom   :" PLAIN " %s" NL, skill->imm_room);
	if (skill->type
			!= SKILL_HERB&& skill->guild >= 0 && skill->guild < MAX_ABILITY)
	{
		sprintf(buf,
				COL_SCORE "Guild:" PLAIN " %s   " COL_SCORE "Align:" PLAIN " %4d  " COL_SCORE "lvl:" PLAIN " %3d" NL,
				class_table[skill->guild].name, skill->alignment,
				skill->min_level);
		send_to_char(buf, ch);
	}
	ch_printf(ch,
			FG_CYAN "ForceType:" PLAIN " %d " FG_CYAN "(" PLAIN "%s" FG_CYAN ")." EOL,
			skill->forcetype, bit_name(forcetype_list, skill->forcetype));
	send_to_char( EOL, ch);

	return;
}

