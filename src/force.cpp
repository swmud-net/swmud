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
 *							Force handling module						   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <dirent.h>
#include "mud.h"
#include "classes/RandomIndices.h"

/*
 * Local functions.
 */
void say_spell args( ( CHAR_DATA *ch, int sn ) );
CHAR_DATA* make_poly_mob args( (CHAR_DATA *ch, int vnum) );
ch_ret spell_affect args( ( int sn, int level, CHAR_DATA *ch, void *vo ) );
ch_ret spell_affectchar args( ( int sn, int level, CHAR_DATA *ch, void *vo ) );
void hatred args( ( CHAR_DATA *ch ) );

/*
 * Is immune to a damage type
 */
bool is_immune(CHAR_DATA *ch, int damtype)
{
	switch (damtype)
	{
	case SD_FIRE:
		if (IS_SET(ch->immune, RIS_FIRE))
			return true;
	case SD_COLD:
		if (IS_SET(ch->immune, RIS_COLD))
			return true;
	case SD_ELECTRICITY:
		if (IS_SET(ch->immune, RIS_ELECTRICITY))
			return true;
	case SD_ENERGY:
		if (IS_SET(ch->immune, RIS_ENERGY))
			return true;
	case SD_ACID:
		if (IS_SET(ch->immune, RIS_ACID))
			return true;
	case SD_POISON:
		if (IS_SET(ch->immune, RIS_POISON))
			return true;
	case SD_DRAIN:
		if (IS_SET(ch->immune, RIS_DRAIN))
			return true;
	}
	return false;
}

/*
 * Lookup a skill by name, only stopping at skills the player has.
 */
int ch_slookup(CHAR_DATA *ch, const char *name)
{
	int sn;

	if (IS_NPC(ch))
		return skill_lookup(name);
	for (sn = 0; sn < top_sn; sn++)
	{
		if (!skill_table[sn]->name)
			break;
		if (ch->pcdata->learned[sn] > 0 && LOWER(name[0]) == LOWER(skill_table[sn]->name[0]) && !str_prefix(name, skill_table[sn]->name))
			return sn;
	}

	return -1;
}

/*
 * Lookup an herb by name.
 */
int herb_lookup(const char *name)
{
	int sn;

	for (sn = 0; sn < top_herb; sn++)
	{
		if (!herb_table[sn] || !herb_table[sn]->name)
			return -1;
		if ( LOWER(name[0]) == LOWER(herb_table[sn]->name[0]) && !str_prefix(name, herb_table[sn]->name))
			return sn;
	}
	return -1;
}

/*
 * Lookup a personal skill
 */
int personal_lookup(CHAR_DATA *ch, const char *name)
{
	return -1;
}

/*
 * Lookup a skill by name.
 */
int skill_lookup(const char *name)
{
	int sn;

	if ((sn = bsearch_skill(name, gsn_first_spell, gsn_first_skill - 1)) == -1)
		if ((sn = bsearch_skill(name, gsn_first_skill, gsn_first_weapon - 1)) == -1)
			if ((sn = bsearch_skill(name, gsn_first_weapon, gsn_first_tongue - 1)) == -1)
				if ((sn = bsearch_skill(name, gsn_first_tongue, gsn_top_sn - 1)) == -1 && gsn_top_sn < top_sn)
				{
					for (sn = gsn_top_sn; sn < top_sn; sn++)
					{
						if (!skill_table[sn] || !skill_table[sn]->name)
							return -1;
						if ( LOWER(name[0]) == LOWER(skill_table[sn]->name[0]) && !str_prefix(name, skill_table[sn]->name))
							return sn;
					}
					return -1;
				}
	return sn;
}

/*
 * Return a skilltype pointer based on sn			-Thoric
 * Returns NULL if bad, unused or personal sn.
 */
SKILLTYPE* get_skilltype(int sn)
{
	if (sn >= TYPE_PERSONAL)
		return NULL;
	if (sn >= TYPE_HERB)
		return IS_VALID_HERB(sn-TYPE_HERB) ? herb_table[sn - TYPE_HERB] : NULL;
	if (sn >= TYPE_HIT)
		return NULL;
	return IS_VALID_SN(sn) ? skill_table[sn] : NULL;
}

/*
 * Perform a binary search on a section of the skill table	-Thoric
 * Each different section of the skill table is sorted alphabetically
 */
int bsearch_skill(const char *name, int first, int top)
{
	int sn;

	for (;;)
	{
		sn = (first + top) >> 1;

		if ( LOWER(name[0]) == LOWER(skill_table[sn]->name[0]) && !str_prefix(name, skill_table[sn]->name))
			return sn;
		if (first >= top)
			return -1;
		if (strcmp(name, skill_table[sn]->name) < 1)
			top = sn - 1;
		else
			first = sn + 1;
	}
	return -1;
}

/*
 * Perform a binary search on a section of the skill table	-Thoric
 * Each different section of the skill table is sorted alphabetically
 * Check for exact matches only
 */
int bsearch_skill_exact(const char *name, int first, int top)
{
	int sn;

	for (;;)
	{
		sn = (first + top) >> 1;

		if (!str_prefix(name, skill_table[sn]->name))
			return sn;
		if (first >= top)
			return -1;
		if (strcmp(name, skill_table[sn]->name) < 1)
			top = sn - 1;
		else
			first = sn + 1;
	}
	return -1;
}

/*
 * Perform a binary search on a section of the skill table
 * Each different section of the skill table is sorted alphabetically
 * Only match skills player knows				-Thoric
 */
int ch_bsearch_skill(CHAR_DATA *ch, const char *name, int first, int top)
{
	int sn;

	for (;;)
	{
		sn = (first + top) >> 1;

		if ( LOWER(name[0]) == LOWER(skill_table[sn]->name[0]) && !str_prefix(name, skill_table[sn]->name) && ch->pcdata->learned[sn] > 0)
			return sn;
		if (first >= top)
			return -1;
		if (strcmp(name, skill_table[sn]->name) < 1)
			top = sn - 1;
		else
			first = sn + 1;
	}
	return -1;
}

int find_spell(CHAR_DATA *ch, const char *name, bool know)
{
	if ( IS_NPC(ch) || !know)
		return bsearch_skill(name, gsn_first_spell, gsn_first_skill - 1);
	else
		return ch_bsearch_skill(ch, name, gsn_first_spell, gsn_first_skill - 1);
}

int find_skill(CHAR_DATA *ch, const char *name, bool know)
{
	if ( IS_NPC(ch) || !know)
		return bsearch_skill(name, gsn_first_skill, gsn_first_weapon - 1);
	else
		return ch_bsearch_skill(ch, name, gsn_first_skill, gsn_first_weapon - 1);
}

int find_weapon(CHAR_DATA *ch, const char *name, bool know)
{
	if ( IS_NPC(ch) || !know)
		return bsearch_skill(name, gsn_first_weapon, gsn_first_tongue - 1);
	else
		return ch_bsearch_skill(ch, name, gsn_first_weapon, gsn_first_tongue - 1);
}

int find_tongue(CHAR_DATA *ch, const char *name, bool know)
{
	if ( IS_NPC(ch) || !know)
		return bsearch_skill(name, gsn_first_tongue, gsn_top_sn - 1);
	else
		return ch_bsearch_skill(ch, name, gsn_first_tongue, gsn_top_sn - 1);
}

/*
 * Lookup a skill by slot number.
 * Used for object loading.
 */
int slot_lookup(int slot)
{
	extern bool fBootDb;
	int sn;

	if (slot <= 0)
		return -1;

	for (sn = 0; sn < top_sn; sn++)
		if (slot == skill_table[sn]->slot)
			return sn;

	if (fBootDb)
	{
		bug("bad slot %d.", slot);
		abort();
	}

	return -1;
}

/*
 * Fancy message handling for a successful casting		-Thoric
 */
void successful_casting(SKILLTYPE *skill, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj)
{
	const char *chitroom = (skill->type == SKILL_SPELL ? COL_FORCE : COL_ACTION);
	const char *chit = (skill->type == SKILL_SPELL ? COL_FORCE : COL_HIT);
	const char *chitme = (skill->type == SKILL_SPELL ? COL_FORCE : COL_HITME);

	if (skill->target != TAR_CHAR_OFFENSIVE)
	{
		chit = chitroom;
		chitme = chitroom;
	}

	if (ch && ch != victim)
	{
		if (skill->hit_char && *skill->hit_char != '\0')
			act(chit, skill->hit_char, ch, obj, victim, TO_CHAR);
		else if (skill->type == SKILL_SPELL)
			act(chit, "Ok.", ch, NULL, NULL, TO_CHAR);
	}
	if (ch && skill->hit_room && *skill->hit_room != '\0')
		act(chitroom, skill->hit_room, ch, obj, victim, TO_NOTVICT);
	if (ch && victim && skill->hit_vict && *skill->hit_vict != '\0')
	{
		if (ch != victim)
			act(chitme, skill->hit_vict, ch, obj, victim, TO_VICT);
		else
			act(chitme, skill->hit_vict, ch, obj, victim, TO_CHAR);
	}
	else if (ch && ch == victim && skill->type == SKILL_SPELL)
		act(chitme, "Ok.", ch, NULL, NULL, TO_CHAR);
}

/*
 * Fancy message handling for a failed casting			-Thoric
 */
void failed_casting(SKILLTYPE *skill, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj)
{
	const char *chitroom = (skill->type == SKILL_SPELL ? COL_FORCE : COL_ACTION);
	const char *chit = (skill->type == SKILL_SPELL ? COL_FORCE : COL_HIT);
	const char *chitme = (skill->type == SKILL_SPELL ? COL_FORCE : COL_HITME);

	if (skill->target != TAR_CHAR_OFFENSIVE)
	{
		chit = chitroom;
		chitme = chitroom;
	}

	if (ch && ch != victim)
	{
		if (skill->miss_char && *skill->miss_char != '\0')
			act(chit, skill->miss_char, ch, obj, victim, TO_CHAR);
		else if (skill->type == SKILL_SPELL)
			act(chit, "Nie uda�o ci si�.", ch, NULL, NULL, TO_CHAR);
	}

	if (ch && skill->miss_room && *skill->miss_room != '\0')
		act(chitroom, skill->miss_room, ch, obj, victim, TO_NOTVICT);

	if (ch && victim && skill->miss_vict && *skill->miss_vict != '\0')
	{
		if (ch != victim)
			act(chitme, skill->miss_vict, ch, obj, victim, TO_VICT);
		else
			act(chitme, skill->miss_vict, ch, obj, victim, TO_CHAR);
	}
	else if (ch && ch == victim)
	{
		if (skill->miss_char && *skill->miss_char != '\0')
			act(chitme, skill->miss_char, ch, obj, victim, TO_CHAR);
		else if (skill->type == SKILL_SPELL)
			act(chitme, "Nie uda�o ci si�.", ch, NULL, NULL, TO_CHAR);
	}
}

/*
 * Fancy message handling for being immune to something		-Thoric
 */
void immune_casting(SKILLTYPE *skill, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj)
{
	const char *chitroom = (skill->type == SKILL_SPELL ? COL_FORCE : COL_ACTION);
	const char *chit = (skill->type == SKILL_SPELL ? COL_FORCE : COL_HIT);
	const char *chitme = (skill->type == SKILL_SPELL ? COL_FORCE : COL_HITME);

	if (skill->target != TAR_CHAR_OFFENSIVE)
	{
		chit = chitroom;
		chitme = chitroom;
	}

	if (ch && ch != victim)
	{
		if (skill->imm_char && *skill->imm_char != '\0')
			act(chit, skill->imm_char, ch, obj, victim, TO_CHAR);
		else if (skill->miss_char && *skill->miss_char != '\0')
			act(chit, skill->hit_char, ch, obj, victim, TO_CHAR);
		else if (skill->type == SKILL_SPELL || skill->type == SKILL_SKILL)
			act(chit, "To chyba nie da�o �adnych efekt�w.", ch, NULL, NULL, TO_CHAR);
	}
	if (ch && skill->imm_room && *skill->imm_room != '\0')
		act(chitroom, skill->imm_room, ch, obj, victim, TO_NOTVICT);
	else if (ch && skill->miss_room && *skill->miss_room != '\0')
		act(chitroom, skill->miss_room, ch, obj, victim, TO_NOTVICT);
	if (ch && victim && skill->imm_vict && *skill->imm_vict != '\0')
	{
		if (ch != victim)
			act(chitme, skill->imm_vict, ch, obj, victim, TO_VICT);
		else
			act(chitme, skill->imm_vict, ch, obj, victim, TO_CHAR);
	}
	else if (ch && victim && skill->miss_vict && *skill->miss_vict != '\0')
	{
		if (ch != victim)
			act(chitme, skill->miss_vict, ch, obj, victim, TO_VICT);
		else
			act(chitme, skill->miss_vict, ch, obj, victim, TO_CHAR);
	}
	else if (ch && ch == victim)
	{
		if (skill->imm_char && *skill->imm_char != '\0')
			act(chit, skill->imm_char, ch, obj, victim, TO_CHAR);
		else if (skill->miss_char && *skill->miss_char != '\0')
			act(chit, skill->hit_char, ch, obj, victim, TO_CHAR);
		else if (skill->type == SKILL_SPELL || skill->type == SKILL_SKILL)
			act(chit, "To chyba nie da�o �adnych efekt�w.", ch, NULL, NULL, TO_CHAR);
	}
}

/*
 * Utter mystical words for an sn.
 */
void say_spell(CHAR_DATA *ch, int sn)
{
	for (auto* rch : ch->in_room->people)
	{
		if (rch != ch)
			act( COL_FORCE, "$n zatrzymuje si� i koncentruje na moment.", ch, NULL, rch, TO_VICT);
	}

	return;
}

void hatred(CHAR_DATA *ch)
{
	ch_printf(ch, sith_hatred[number_range(0, 9)]);
	return;
}

/*
 * Make adjustments to saving throw based in RIS		-Thoric
 */
int ris_save(CHAR_DATA *ch, int chance, int ris)
{
	int modifier;

	modifier = 10;
	if (IS_SET(ch->immune, ris))
		modifier -= 10;
	if (IS_SET(ch->resistant, ris))
		modifier -= 2;
	if (IS_SET(ch->susceptible, ris))
		modifier += 2;
	if (modifier <= 0)
		return 1000;
	if (modifier == 10)
		return chance;
	return (chance * modifier) / 10;
}

/*								    -Thoric
 * Fancy dice expression parsing complete with order of operations,
 * simple exponent support, dice support as well as a few extra
 * variables: L = level, H = hp, M = mana, V = move, S = str, X = dex
 *            I = int, W = wis, C = con, A = cha, U = luck, A = age
 *
 * Used for spell dice parsing, ie: 3d8+L-6
 *
 */
int rd_parse(CHAR_DATA *ch, int level, char *exp)
{
	int x, lop = 0, gop = 0, eop = 0;
	char operation;
	char *sexp[2];
	int total = 0, len = 0;

	/* take care of nulls coming in */
	if (!exp || !strlen(exp))
		return 0;

	/* get rid of brackets if they surround the entire expresion */
	if ((*exp == '(') && !index(exp + 1, '(') && exp[strlen(exp) - 1] == ')')
	{
		exp[strlen(exp) - 1] = '\0';
		exp++;
	}

	/* check if the expresion is just a number */
	len = strlen(exp);
	if (len == 1 && isalpha(exp[0]))
		switch (exp[0])
		{
		case 'L':
		case 'l':
			return level;
		case 'H':
		case 'h':
			return ch->hit;
		case 'M':
		case 'm':
			return ch->mana;
		case 'V':
		case 'v':
			return ch->move;
		case 'S':
		case 's':
			return get_curr_str(ch);
		case 'I':
		case 'i':
			return get_curr_int(ch);
		case 'W':
		case 'w':
			return get_curr_wis(ch);
		case 'X':
		case 'x':
			return get_curr_dex(ch);
		case 'C':
		case 'c':
			return get_curr_con(ch);
		case 'A':
		case 'a':
			return get_curr_cha(ch);
		case 'U':
		case 'u':
			return get_curr_lck(ch);
		case 'Y':
		case 'y':
			return get_age(ch);
		}

	for (x = 0; x < len; ++x)
		if (!isdigit(exp[x]) && !isspace(exp[x]))
			break;
	if (x == len)
		return (atoi(exp));

	/* break it into 2 parts */
	for (x = 0; x < (int) strlen(exp); ++x)
		switch (exp[x])
		{
		case '^':
			if (!total)
				eop = x;
			break;
		case '-':
		case '+':
			if (!total)
				lop = x;
			break;
		case '*':
		case '/':
		case '%':
		case 'd':
		case 'D':
			if (!total)
				gop = x;
			break;
		case '(':
			++total;
			break;
		case ')':
			--total;
			break;
		}
	if (lop)
		x = lop;
	else if (gop)
		x = gop;
	else
		x = eop;
	operation = exp[x];
	exp[x] = '\0';
	sexp[0] = exp;
	sexp[1] = (char*) (exp + x + 1);

	/* work it out */
	total = rd_parse(ch, level, sexp[0]);
	switch (operation)
	{
	case '-':
		total -= rd_parse(ch, level, sexp[1]);
		break;
	case '+':
		total += rd_parse(ch, level, sexp[1]);
		break;
	case '*':
		total *= rd_parse(ch, level, sexp[1]);
		break;
	case '/':
		total /= rd_parse(ch, level, sexp[1]);
		break;
	case '%':
		total %= rd_parse(ch, level, sexp[1]);
		break;
	case 'd':
	case 'D':
		total = dice(total, rd_parse(ch, level, sexp[1]));
		break;
	case '^':
	{
		int y = rd_parse(ch, level, sexp[1]), z = total;

		for (x = 1; x < y; ++x, z *= total)
			;
		total = z;
		break;
	}
	}
	return total;
}

/* wrapper function so as not to destroy exp */
int dice_parse(CHAR_DATA *ch, int level, char *exp)
{
	char buf[MAX_INPUT_LENGTH];

	strcpy(buf, exp);
	return rd_parse(ch, level, buf);
}

/*
 * Compute a saving throw.
 * Negative apply's make saving throw better.
 */
bool saves_poison_death(int level, CHAR_DATA *victim)
{
	int save;

	save = 50 + (victim->top_level - level - victim->saving_poison_death) * 2;
	save = URANGE(5, save, 95);
	return chance(victim, save);
}
/*
 bool saves_wands( int level, CHAR_DATA *victim )
 {
 int save;

 if( IS_SET( victim->immune, RIS_FORCE ) )
 return true;

 save = 50 + ( victim->top_level - level - victim->saving_wand ) * 2;
 save = URANGE( 5, save, 95 );
 return chance( victim, save );
 }
 */
bool saves_para_petri(int level, CHAR_DATA *victim)
{
	int save;

	save = 50 + (victim->top_level - level - victim->saving_para_petri) * 2;
	save = URANGE(5, save, 95);
	return chance(victim, save);
}
bool saves_breath(int level, CHAR_DATA *victim)
{
	int save;

	save = 50 + (victim->top_level - level - victim->saving_breath) * 2;
	save = URANGE(5, save, 95);
	return chance(victim, save);
}
/*
 bool saves_spell_staff( int level, CHAR_DATA *victim )
 {
 int save;

 if( IS_SET( victim->immune, RIS_FORCE ) )
 return true;

 if( IS_NPC( victim ) && level > 10 )
 level -= 5;
 save = 50 + ( victim->top_level - level - victim->saving_spell_staff ) * 2;
 save = URANGE( 5, save, 95 );
 return chance( victim, save );
 }
 */

/*
 * Process the spell's required components, if any		-Thoric
 * -----------------------------------------------
 * T###		check for item of type ###
 * V#####	check for item of vnum #####
 * Kword	check for item with keyword 'word'
 * G#####	check if player has ##### amount of gold
 * H####	check if player has #### amount of hitpoints
 *
 * Special operators:
 * ! spell fails if player has this
 * + don't consume this component
 * @ decrease component's value[0], and extract if it reaches 0
 * # decrease component's value[1], and extract if it reaches 0
 * $ decrease component's value[2], and extract if it reaches 0
 * % decrease component's value[3], and extract if it reaches 0
 * ^ decrease component's value[4], and extract if it reaches 0
 * & decrease component's value[5], and extract if it reaches 0
 */
bool process_spell_components(CHAR_DATA *ch, int sn)
{
	SKILLTYPE *skill = get_skilltype(sn);
	char *comp = skill->components;
	char *check;
	char arg[MAX_INPUT_LENGTH];
	bool consume, fail, found;
	int val, value;
	OBJ_DATA *obj;

	/* if no components necessary, then everything is cool */
	if (!comp || comp[0] == '\0')
		return true;

	/* disable the whole damn shabang */

//      return true;
	while (comp[0] != '\0')
	{
		comp = one_argument(comp, arg);
		consume = true;
		fail = found = false;
		val = -1;
		switch (arg[1])
		{
		default:
			check = arg + 1;
			break;
		case '!':
			check = arg + 2;
			fail = true;
			break;
		case '+':
			check = arg + 2;
			consume = false;
			break;
		case '@':
			check = arg + 2;
			val = 0;
			break;
		case '#':
			check = arg + 2;
			val = 1;
			break;
		case '$':
			check = arg + 2;
			val = 2;
			break;
		case '%':
			check = arg + 2;
			val = 3;
			break;
		case '^':
			check = arg + 2;
			val = 4;
			break;
		case '&':
			check = arg + 2;
			val = 5;
			break;
		}
		value = atoi(check);
		obj = NULL;
		switch (UPPER(arg[0]))
		{
		case 'T':
			for (auto* o : ch->carrying)
				if (o->item_type == value)
				{
					obj = o;
					if (fail)
					{
						send_to_char("Co� nie pozwala ci u�y� tej mocy..." NL, ch);
						return false;
					}
					found = true;
					break;
				}
			break;
		case 'V':
			for (auto* o : ch->carrying)
				if (o->pIndexData->vnum == value)
				{
					obj = o;
					if (fail)
					{
						send_to_char("Co� nie pozwala ci u�y� tej mocy..." NL, ch);
						return false;
					}
					found = true;
					break;
				}
			break;
		case 'K':
			for (auto* o : ch->carrying)
				if (nifty_is_name(check, o->name))
				{
					obj = o;
					if (fail)
					{
						send_to_char("Co� nie pozwala ci u�y� tej mocy..." NL, ch);
						return false;
					}
					found = true;
					break;
				}
			break;
		case 'G':
			if (ch->gold >= value)
			{
				if (fail)
				{
					send_to_char("Co� nie pozwala ci u�y� tej mocy..." NL, ch);
					return false;
				}
				else
				{
					if (consume)
					{
						send_to_char( FG_YELLOW "Czujesz, �e czego� ci uby�o..." EOL, ch);
						ch->gold -= value;
					}
					continue;
				}
			}
			break;
		case 'H':
			if (ch->hit >= value)
			{
				if (fail)
				{
					send_to_char("Co� nie pozwala ci u�y� tej mocy..." NL, ch);
					return false;
				}
				else
				{
					if (consume)
					{
						send_to_char( FG_RED
						"Czujesz, �e czego� ci uby�o..." EOL, ch);
						ch->hit -= value;
						update_pos(ch);
					}
					continue;
				}
			}
			break;
		}
		/* having this component would make the spell fail... if we get
		 here, then the caster didn't have that component */
		if (fail)
			continue;
		if (!found)
		{
			send_to_char("Czego� ci jeszcze potrzeba..." NL, ch);
			return false;
		}
		if (obj)
		{
			if (val >= 0 && val < 6)
			{
				separate_obj(obj);
				if (obj->value[val] <= 0)
					return false;
				else if (--obj->value[val] == 0)
				{
					act( COL_FORCE, "$p jasno rozb�yskuje i znika w chmurze dymu!", ch, obj, NULL, TO_CHAR);
					act( COL_FORCE, "$p jasno rozb�yskuje i znika w chmurze dymu!", ch, obj, NULL, TO_ROOM);
					extract_obj(obj);
				}
				else
					act( COL_FORCE, "$p jasno rozb�yskuje i lekko dymi.", ch, obj, NULL, TO_CHAR);
			}
			else if (consume)
			{
				separate_obj(obj);
				act( COL_FORCE, "$p jasno rozb�yskuje i znika w chmurze dymu!", ch, obj, NULL, TO_CHAR);
				act( COL_FORCE, "$p jasno rozb�yskuje i znika w chmurze dymu!", ch, obj, NULL, TO_ROOM);
				extract_obj(obj);
			}
			else
			{
				int count = obj->count;

				obj->count = 1;
				act( COL_FORCE, "$p jasno rozb�yskuje.", ch, obj, NULL, TO_CHAR);
				obj->count = count;
			}
		}
	}
	return true;
}

int pAbort;

/*
 * Locate targets.
 */
void* locate_targets(CHAR_DATA *ch, char *arg, int sn, CHAR_DATA **victim, OBJ_DATA **obj)
{
	SKILLTYPE *skill = get_skilltype(sn);
	void *vo = NULL;

	*victim = NULL;
	*obj = NULL;

	switch (skill->target)
	{
	default:
		bug("bad target for sn %d.", sn);
		return &pAbort;

	case TAR_IGNORE:
		break;

	case TAR_CHAR_OFFENSIVE:
		if (arg[0] == '\0')
		{
			if ((*victim = who_fighting(ch)) == NULL)
			{
				send_to_char("Na kim chcesz u�y� MOCY?" NL, ch);
				return &pAbort;
			}
		}
		else
		{
			if ((*victim = get_char_room(ch, arg)) == NULL)
			{
				send_to_char("Nie ma tu nikogo takiego." NL, ch);
				return &pAbort;
			}
		}

		if (is_safe(ch, *victim))
			return &pAbort;

		if (ch == *victim)
		{
			send_to_char("Chcesz u�y� tego na sobie? W porz�dku..." NL, ch);
			/*
			 send_to_char( "You can't do that to yourself." NL, ch );
			 return &pAbort;
			 */
		}

		if (!IS_NPC(ch))
		{
			if (!IS_NPC(*victim))
			{
				/*  Sheesh! can't do anything
				 send_to_char( "You can't do that on a player." NL, ch );
				 return &pAbort;
				 */

				if (get_timer(ch, TIMER_PKILLED) > 0)
				{
					send_to_char("Zabito ci� ju� w czasie ostatnich pi�ciu minut." NL, ch);
					return &pAbort;
				}

				if (get_timer(*victim, TIMER_PKILLED) > 0)
				{
					send_to_char("Ten gracz by� ju� zabity w czasie ostatnich pi�ciu minut." NL, ch);
					return &pAbort;
				}

			}

			if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == *victim)
			{
				send_to_char("No co� ty! Przecie� to tw�j lojalny s�uga!" NL, ch);
				return &pAbort;
			}
		}

		vo = (void*) *victim;
		break;

	case TAR_CHAR_DEFENSIVE:
		if (arg[0] == '\0')
			*victim = ch;
		else
		{
			if ((*victim = get_char_room(ch, arg)) == NULL)
			{
				send_to_char("Nie ma tu nikogo takiego." NL, ch);
				return &pAbort;
			}
		}
		vo = (void*) *victim;
		break;

	case TAR_CHAR_SELF:
		if (arg[0] != '\0' && !nifty_is_name(arg, ch->name))
		{
			send_to_char("Nie mo�esz u�y� tej mocy na innych." NL, ch);
			return &pAbort;
		}

		vo = (void*) ch;
		break;

	case TAR_OBJ_INV:
		if (arg[0] == '\0')
		{
			send_to_char("Na czym chcesz u�y� tej mocy?" NL, ch);
			return &pAbort;
		}

		if ((*obj = get_obj_carry(ch, arg)) == NULL)
		{
			send_to_char("Nie masz niczego takiego." NL, ch);
			return &pAbort;
		}

		vo = (void*) *obj;
		break;
	}

	return vo;
}

/*
 * The kludgy global is for spells who want more stuff from command line.
 */
char *target_name;

/*
 * Cast a spell.  Multi-caster and component support by Thoric
 */
DEF_DO_FUN( cast )
{
	char arg1[MIL];
	char arg2[MIL];
	char staticbuf[MIL];
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	void *vo = 0;
	int mana;
	int sn;
	ch_ret retcode;
	bool dont_wait = false;
	SKILLTYPE *skill = NULL;
	struct timeval time_used;

	retcode = rNONE;

	if (ch->perm_frc <= 0)
	{
		send_to_char("Nie mo�esz jeszcze tego zrobi�." NL, ch);
		return;
	}

	switch (ch->substate)
	{
	default:
		/* no ordering charmed mobs to cast spells */
		if ( IS_NPC(ch) && IS_AFFECTED(ch, AFF_CHARM))
		{
			send_to_char("Jako� nie mo�esz si� na to teraz zdecydowa�..." NL, ch);
			return;
		}

		if (IS_SET(ch->in_room->room_flags, ROOM_NO_FORCE))
		{
			send_to_char( COL_FORCE "Nie uda�o ci si�." EOL, ch);
			return;
		}

		target_name = one_argument(argument, arg1);
		one_argument(target_name, arg2);

		if (arg1[0] == '\0')
		{
			send_to_char("Jak, na czym, gdzie i po co chcesz u�y� MOCY?" NL, ch);
			return;
		}

		if (get_trust(ch) < LEVEL_GOD)
		{
			if ((sn = find_spell(ch, arg1, true)) < 0 || (!IS_NPC(ch) && ch->pcdata->learned[sn] <= 0))
			{
				send_to_char("Nie mo�esz jeszcze tego zrobi�." NL, ch);
				return;
			}
			if ((skill = get_skilltype(sn)) == NULL)
			{
				send_to_char("Nie mo�esz teraz tego zrobi�..." NL, ch);
				return;
			}
		}
		else
		{
			if ((sn = skill_lookup(arg1)) < 0)
			{
				send_to_char("Nikt jeszcze o niczym takim nie s�ysza�..." NL, ch);
				return;
			}
			if (sn >= MAX_SKILL)
			{
				send_to_char("Hmm... to mo�e bole�." NL, ch);
				return;
			}
			if ((skill = get_skilltype(sn)) == NULL)
			{
				send_to_char("Z t� moc� mo�e by� akurat problem..." NL, ch);
				return;
			}
			if (skill->type != SKILL_SPELL)
			{
				send_to_char("To nie jest moc." NL, ch);
				return;
			}
			if (!skill->spell_fun)
			{
				send_to_char("Nikt nie s�ysza� jeszcze o czym� takim..." NL, ch);
				return;
			}
		}

		/*
		 * Something else removed by Merc			-Thoric
		 */
		if (ch->position < skill->minimum_position)
		{
			switch (ch->position)
			{
			default:
				send_to_char("Nie mo�esz si� wystarczaj�co skoncentrowa�." NL, ch);
				break;
			case POS_SITTING:
				send_to_char("Siedz�c, nie zgromadzisz w sobie wystarczaj�cej ilo�ci energii." NL, ch);
				break;
			case POS_RESTING:
				ch_printf(ch, "Jeste� zbyt rozlu�nion%s by u�ywa� mocy.." NL, SEX_SUFFIX_YAE(ch));
				break;
			case POS_FIGHTING:
				send_to_char("Walka nie pozwala ci si� wystarczaj�co skupi�." NL, ch);
				break;
			case POS_SLEEPING:
				send_to_char("Co ci si� �ni ???" NL, ch);
				break;
			}
			return;
		}

		if (skill->spell_fun == spell_null)
		{
			send_to_char("To nie jest moc!" NL, ch);
			return;
		}

		if (!skill->spell_fun)
		{
			send_to_char("Nie mo�esz tego u�y�... jeszcze." NL, ch);
			return;
		}

		mana = IS_NPC(ch) ? 0 : skill->min_mana;

		/*
		 * Locate targets.
		 */
		vo = locate_targets(ch, arg2, sn, &victim, &obj);
		if (vo == &pAbort)
			return;

		if (is_safe(ch, victim))
		{
			send_to_char( COL_FORCE "Nie mo�esz tego zrobi�." EOL, ch);
			return;
		}

		if (!IS_NPC(ch) && ch->mana < mana)
		{
			send_to_char("Moc w tobie jest za s�aba by to zrobi�." NL, ch);
			return;
		}
		if (skill->participants <= 1)
			break;
		/* multi-participant spells			-Thoric */
		add_timer(ch, TIMER_DO_FUN, UMIN(skill->beats / 10, 3), do_cast, 1);
		act( COL_FORCE, "Zaczynasz czu� moc w tobie i naoko�o siebie...", ch, NULL, NULL, TO_CHAR);
		act( COL_FORCE, "$n jednoczy si� moc� z otoczeniem...", ch, NULL, NULL, TO_ROOM);
		swsnprintf(staticbuf, MIL, "%s %s", arg2, target_name);
		STRDUP(ch->dest_buf, staticbuf);
		ch->tempnum = sn;
		return;

	case SUB_TIMER_DO_ABORT:
		STRDUP(ch->dest_buf, "");
		if (IS_VALID_SN((sn = ch->tempnum)))
		{
			if ((skill = get_skilltype(sn)) == NULL)
			{
				send_to_char("Co� posz�o nie tak..." NL, ch);
				bug("SUB_TIMER_DO_ABORT: bad sn %d", sn);
				return;
			}
			mana = IS_NPC(ch) ? 0 : skill->min_mana;

			if (get_trust(ch) < LEVEL_IMMORTAL) /* so imms dont lose mana */
				ch->mana -= mana / 3;
		}
		send_to_char( COL_FORCE "Przerywasz koncentracj�." NL, ch);
		/* should add chance of backfire here */
		return;

	case 1:
		sn = ch->tempnum;
		if ((skill = get_skilltype(sn)) == NULL)
		{
			send_to_char("Co� posz�o nie tak..." NL, ch);
			bug("substate 1: bad sn %d", sn);
			return;
		}
		if (!ch->dest_buf || !IS_VALID_SN(sn) || skill->type != SKILL_SPELL)
		{
			send_to_char("Co� neguje si�� mocy." NL, ch);
			bug("ch->dest_buf NULL or bad sn (%d)", sn);
			return;
		}
		mana = IS_NPC(ch) ? 0 : skill->min_mana;
		strcpy(staticbuf, ch->dest_buf);
		target_name = one_argument(staticbuf, arg2);
		STRDUP(ch->dest_buf, "");
		ch->substate = SUB_NONE;
		if (skill->participants > 1)
		{
			int cnt = 1;
			TIMER *t;

			for (auto* tmp : ch->in_room->people)
				if (tmp != ch && (t = get_timerptr(tmp, TIMER_DO_FUN)) != NULL && t->count >= 1 && t->do_fun == do_cast
						&& tmp->tempnum == sn && tmp->dest_buf && !str_cmp(tmp->dest_buf, staticbuf))
					++cnt;
			if (cnt >= skill->participants)
			{
				for (auto* tmp : ch->in_room->people)
					if (tmp != ch && (t = get_timerptr(tmp, TIMER_DO_FUN)) != NULL && t->count >= 1 && t->do_fun == do_cast
							&& tmp->tempnum == sn && tmp->dest_buf && !str_cmp(tmp->dest_buf, staticbuf))
					{
						extract_timer(tmp, t);
						act( COL_FORCE, "Zestrajasz sw� energi� z $n$4 pomagaj�c $m kierowa� moc�.", ch, NULL, tmp, TO_VICT);
						act( COL_FORCE, "$N zestraja z tob� swoj� energi�!", ch, NULL, tmp, TO_CHAR);
						act( COL_FORCE, "$N zestraja z $n$4 swoj� energi�!", ch, NULL, tmp, TO_NOTVICT);
						learn_from_success(tmp, sn);

						tmp->mana -= mana;
						tmp->substate = SUB_NONE;
						tmp->tempnum = -1;
						STRDUP(ch->dest_buf, "");
					}
				dont_wait = true;
				send_to_char("Koncentrujesz ca�� energi� w jeden strumie� Mocy!" NL, ch);
				vo = locate_targets(ch, arg2, sn, &victim, &obj);
				if (vo == &pAbort)
					return;
			}
			else
			{
				send_to_char("Niestety, nie wystarczy�o ci mocy..." NL, ch);

				if (get_trust(ch) < LEVEL_IMMORTAL) /* so imms dont lose mana */
					ch->mana -= mana / 2;
				learn_from_failure(ch, sn);
				return;
			}
		}
	}

	if (str_cmp(skill->name, "ventriloquate"))
		say_spell(ch, sn);

	if (!dont_wait)
		WAIT_STATE(ch, skill->beats);

	/*
	 * Getting ready to cast... check for spell components	-Thoric
	 */
	if (!process_spell_components(ch, sn))
	{

		if (get_trust(ch) < LEVEL_IMMORTAL) /* so imms dont lose mana */
			ch->mana -= mana / 2;
		learn_from_failure(ch, sn);
		return;
	}

	/*Immo nie musza sie przejmowac swoim alignem - Ganis */
	if (!IS_NPC(ch) && !IS_IMMORTAL(ch) && abs(ch->alignment - skill->alignment) > 1010)
	{
		if (ch->alignment > skill->alignment)
		{
			send_to_char("Za ma�o w tobie gniewu, by to zrobi�." NL, ch);
			if (get_trust(ch) < LEVEL_IMMORTAL) /* so imms dont lose mana */
				ch->mana -= mana / 2;
			return;
		}
		if (ch->alignment < skill->alignment)
		{
			send_to_char("Gniew i nienawi�� przeszkadzaj� ci w zrobieniu tego." NL, ch);
			if (get_trust(ch) < LEVEL_IMMORTAL) /* so imms dont lose mana */
				ch->mana -= mana / 2;
			return;
		}
	}
	if (!IS_NPC(ch) && (number_percent() + skill->difficulty * 5) > ch->pcdata->learned[sn])
	{
		/* Some more interesting loss of concentration messages  -Thoric */
		switch (number_bits(2))
		{
		case 0: /* too busy */
			if (ch->fighting)
				send_to_char("To nie by� dobry moment, nie uda�o ci si� odpowiednio skupi�." NL, ch);
			else
				ch_printf(ch, "Straci�%s� koncentracj�." NL, SEX_SUFFIX_EAE(ch));
			break;
		case 1: /* irritation */
			if (number_bits(2) == 0)
			{
				switch (number_bits(2))
				{
				case 0:
					send_to_char("Zasw�dzia� ci� nos i twoja koncentracja znikn�a." NL, ch);
					break;
				case 1:
					ch_printf(ch, "Poczu�%s� mrowienie na nodze i straci�%s� koncentracj�." NL, SEX_SUFFIX_EAE(ch), SEX_SUFFIX_EAE(ch));
					break;
				case 2:
					send_to_char("Jaka� spro�na my�l odwr�ci�a twoje skupienie." NL, ch);
					break;
				case 3:
					send_to_char("Paproch w oku uniemo�liwi� ci u�ycie mocy." NL, ch);
					break;
				}
			}
			else
				send_to_char("Co� odwr�ci�o twoj� uwag�. Tracisz koncentracj�." NL, ch);
			break;
		case 2: /* not enough time */
			if (ch->fighting)
				ch_printf(ch, "Mia�%s� za ma�o czasu na skupienie. Nie uda�o si�." NL, SEX_SUFFIX_EAE(ch));
			else
				ch_printf(ch, "Straci�%s� koncentracj�." NL, SEX_SUFFIX_EAE(ch));
			break;
		case 3:
			send_to_char("Zaburzenia mocy psuj� twoj� koncentracj�." NL, ch);
			break;
		}

		if (get_trust(ch) < LEVEL_IMMORTAL) /* so imms dont lose mana */
			ch->mana -= mana / 2;
		learn_from_failure(ch, sn);
		return;
	}
	else
	{

		ch->mana -= mana;

		/*
		 * check for immunity to force if victim is known...
		 * and it is a TAR_CHAR_DEFENSIVE/SELF spell
		 * otherwise spells will have to check themselves
		 */
		if ((skill->target == TAR_CHAR_DEFENSIVE || skill->target == TAR_CHAR_SELF) && victim && IS_SET(victim->immune, RIS_FORCE))
		{
			immune_casting(skill, ch, victim, NULL);
			retcode = rSPELL_FAILED;
		}
		else
		{
			// Thanos: ofensywne spelle pozwalaj� na danie informa.
			if (skill->target == TAR_CHAR_OFFENSIVE && victim && !char_died(victim) && victim != ch)
				add_inform(victim, ch);

			start_timer(&time_used);
			retcode = (*skill->spell_fun)(sn, ch->skill_level[FORCE_ABILITY], ch, vo);
			end_timer(&time_used);
			update_userec(&time_used, &skill->userec);
		}
	}

	if (retcode == rCHAR_DIED || retcode == rERROR || char_died(ch))
		return;
	if (retcode != rSPELL_FAILED)
	{
		int force_exp;

		if (IS_NPC(ch))
			force_exp = 0;
		else if (ch->pcdata->learned[sn] > 99)
			force_exp = 1;
		else
			force_exp = 100 - ch->pcdata->learned[sn]
					+ log(skill->min_level + ch->pcdata->learned[sn] + ch->skill_level[FORCE_ABILITY]) / log(2);
		force_exp = URANGE(0, force_exp, (exp_level(ch->skill_level[FORCE_ABILITY]+1)-exp_level(ch->skill_level[FORCE_ABILITY]))/20);
		if (!ch->fighting)
			ch_printf(ch, "Zdobywasz %d punkt%s do�wiadczenia we w�adaniu Moc�." NL, force_exp, NUMBER_SUFF(force_exp, "", "y", "�w"));
		gain_exp(ch, force_exp, FORCE_ABILITY);
		learn_from_success(ch, sn);
	}
	else
		learn_from_failure(ch, sn);

	/*
	 * Fixed up a weird mess here, and added double safeguards	-Thoric
	 */
	if (skill->target == TAR_CHAR_OFFENSIVE && victim && !char_died(victim) && victim != ch)
	{
		for (auto* vch : ch->in_room->people)
		{
			if (vch == victim)
			{
				if (victim->master != ch && !victim->fighting)
					// Thanos: podmiana argumentow (ch i victim) zeby inform dzialal ok
					retcode = multi_hit(ch, victim, TYPE_UNDEFINED);
				break;
			}
		}
	}

	return;
}

/*
 * Cast spells at targets using a forceal object.
 */
ch_ret obj_cast_spell(int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj)
{
	void *vo;
	ch_ret retcode = rNONE;
	int levdiff = ch->top_level - level;
	SKILLTYPE *skill = get_skilltype(sn);
	struct timeval time_used;

	if (sn == -1 || !sn) /* jak jest reserved to te� nic -- Thanos */
		return retcode;

	IF_BUG(ch == NULL, "")
		return rERROR;

	IF_BUG(victim == NULL, "(ch=%s)", ch->name)
		return rERROR;

	IF_BUG(!skill, "bad sn %d (ch=%s) (vict=%s)", sn, ch->name, victim->name)
		return rERROR;

	IF_BUG(!skill->spell_fun, "bad spell %s (ch=%s) (vict=%s)", skill->name, ch->name, victim->name)
		return rERROR;

	if (IS_SET(ch->in_room->room_flags, ROOM_NO_FORCE))
	{
		send_to_char( COL_FORCE "Jako� nic si� nie dzieje..." EOL, ch);
		return rNONE;
	}

	/*
	 * Basically this was added to cut down on level 5 players using level
	 * 40 scrolls in battle too often ;)		-Thoric
	 */
	if ((skill->target == TAR_CHAR_OFFENSIVE || number_bits(7) == 1) /* 1/128 chance if non-offensive */
	&& skill->type != SKILL_HERB && !chance(ch, 95 + levdiff))
	{
		switch (number_bits(2))
		{
		case 0:
			failed_casting(skill, ch, victim, NULL);
			break;
		case 1:
			act( COL_FORCE, "Lamersko u�ywasz $p$3 i niechc�cy kierujesz wszystko na siebie!", ch, obj, victim, TO_CHAR);
			if (victim)
				act( COL_FORCE, "$n lamersko u�ywa $p$3 i niechc�cy kieruje wszystko na siebie!", ch, obj, victim, TO_VICT);
			act( COL_FORCE, "$n lamersko u�ywa $p$3 i niechc�cy kieruje wszystko na siebie!", ch, obj, victim, TO_NOTVICT);
			return damage(ch, ch, number_range(1, level), TYPE_UNDEFINED);
		case 2:
			failed_casting(skill, ch, victim, NULL);
			break;
		case 3:
			act( COL_FORCE, "Lamersko u�ywasz $p$3 i niechc�cy kierujesz wszystko na siebie!", ch, obj, victim, TO_CHAR);
			if (victim)
				act( COL_FORCE, "$n lamersko u�ywa $p$3 i niechc�cy kieruje wszystko na siebie!", ch, obj, victim, TO_VICT);
			act( COL_FORCE, "$n lamersko u�ywa $p$3 i niechc�cy kieruje wszystko na siebie!", ch, obj, victim, TO_NOTVICT);
			return damage(ch, ch, number_range(1, level), TYPE_UNDEFINED);
		}
		return rNONE;
	}

	target_name = (char*) "";
	switch (skill->target)
	{
	default:
		bug("bad target for spell: %s.", skill->name);
		return rERROR;

	case TAR_IGNORE:
		vo = NULL;
		if (victim)
			target_name = victim->name;
		else if (obj)
			target_name = obj->name;
		break;

	case TAR_CHAR_OFFENSIVE:
		if (victim != ch)
		{
			if (!victim)
				victim = who_fighting(ch);
			if (!victim || !IS_NPC(victim))
			{
				send_to_char("Nie mo�esz tego zrobi�" NL, ch);
				return rNONE;
			}
		}
		if (ch != victim && is_safe(ch, victim))
			return rNONE;
		vo = (void*) victim;
		break;

	case TAR_CHAR_DEFENSIVE:
		if (victim == NULL)
			victim = ch;
		vo = (void*) victim;
		if (skill->type != SKILL_HERB && IS_SET(victim->immune, RIS_FORCE))
		{
			immune_casting(skill, ch, victim, NULL);
			return rNONE;
		}
		break;

	case TAR_CHAR_SELF:
		vo = (void*) ch;
		if (skill->type != SKILL_HERB && IS_SET(ch->immune, RIS_FORCE))
		{
			immune_casting(skill, ch, victim, NULL);
			return rNONE;
		}
		break;

	case TAR_OBJ_INV:
		if (obj == NULL)
		{
			send_to_char("Nie mo�esz tego zrobi�" NL, ch);
			return rNONE;
		}
		vo = (void*) obj;
		break;
	}

	start_timer(&time_used);
	retcode = (*skill->spell_fun)(sn, level, ch, vo);
	end_timer(&time_used);
	update_userec(&time_used, &skill->userec);

	if (retcode == rSPELL_FAILED)
		retcode = rNONE;

	if (retcode == rCHAR_DIED || retcode == rERROR)
		return retcode;

	if (char_died(ch))
		return rCHAR_DIED;

	if (skill->target == TAR_CHAR_OFFENSIVE && victim != ch && !char_died(victim))
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

	return retcode;
}

/*
 * Spell functions.
 */
ch_ret spell_blindness(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	AFFECT_DATA af;
	SKILLTYPE *skill = get_skilltype(sn);

	if (victim == ch)
	{
		send_to_char("Siebie chcesz o�lepi�?" NL, ch);
		return rSPELL_FAILED;
	}

	hatred(ch);
	sith_penalty(ch);
	ch->alignment -= 29;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}
	if (IS_AFFECTED(victim, AFF_BLIND))
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	af.type = sn;
	af.location = APPLY_HITROLL;
	af.modifier = -level;
	af.duration = (1 + (level / 3)) * DUR_CONV;
	af.bitvector = AFF_BLIND;
	affect_to_char(victim, &af);
	ch_printf(victim, "Zosta�%s� o�lepion%s!" NL, SEX_SUFFIX_EAE(victim), SEX_SUFFIX_YAE(victim));
	if (ch != victim)
		send_to_char("Ok." NL, ch);
	return rNONE;
}

ch_ret spell_cause_light(int sn, int level, CHAR_DATA *ch, void *vo)
{
	hatred(ch);
	ch->alignment -= 50;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	return damage(ch, (CHAR_DATA*) vo, dice(1, 8) + level / 3, sn);
}

ch_ret spell_cause_critical(int sn, int level, CHAR_DATA *ch, void *vo)
{
	hatred(ch);
	ch->alignment -= 70;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	return damage(ch, (CHAR_DATA*) vo, dice(3, 8) + level, sn);
}

ch_ret spell_cause_serious(int sn, int level, CHAR_DATA *ch, void *vo)
{
	hatred(ch);
	ch->alignment -= 90;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	return damage(ch, (CHAR_DATA*) vo, dice(level, 2), sn);
}

ch_ret spell_charm_person(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	AFFECT_DATA af;
	int chance;
	char buf[MAX_STRING_LENGTH];
	SKILLTYPE *skill = get_skilltype(sn);

	if (victim == ch)
	{
		send_to_char("Lubisz ju� siebie. Ooo i to nawet bardzo!" NL, ch);
		return rSPELL_FAILED;
	}

	if (!IS_NPC(victim) && !IS_NPC(ch))
	{
		send_to_char("Ten numer nie przejdzie..." NL, ch);
		send_to_char("Chyba kto� przed chwil� pr�bowa� namiesza� ci w g�owie." NL, victim);
		return rSPELL_FAILED;
	}

	if ( IS_SET( victim->immune, RIS_FORCE ) || IS_SET(victim->immune, RIS_CHARM))
	{
		/* Thanos was here :P -- I Epizod sie klania */
		if (number_percent() > 50)
			ch_tell(victim, ch, "Co ty? My�lisz, �e jeste� jaki� Jedi czy co?");
		else
			immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (level < victim->top_level)
	{
		ch_tell(victim, ch, "Co ty? My�lisz, �e jeste� jaki� Jedi czy co?");
		return rSPELL_FAILED;
	}

	chance = ris_save(victim, level, RIS_CHARM);

	if ( IS_AFFECTED(victim, AFF_CHARM) || chance == 1000 || IS_AFFECTED(ch, AFF_CHARM) || level < victim->top_level
			|| circle_follow(victim, ch))
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (victim->master)
		stop_follower(victim);
	add_follower(victim, ch);
	af.type = sn;
	af.duration = (number_fuzzy((level + 1) / 3) + 1) * DUR_CONV;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = AFF_CHARM;
	affect_to_char(victim, &af);
	act( COL_FORCE, "Czy� $n nie jest wspania�$y?", ch, NULL, victim, TO_VICT);
	act( COL_FORCE, "Oczy $N$1 trac� blask...", ch, NULL, victim, TO_ROOM);
	if (ch != victim)
		send_to_char("Ok." NL, ch);

	sprintf(buf, "%s has used mind trick on %s.", ch->name, victim->name);
	log_string_plus(buf, LOG_NORMAL, ch->top_level);
	return rNONE;
}

//Trog: nie wywalam bo sie moze przydac do czegos.
ch_ret spell_control_weather(int sn, int level, CHAR_DATA *ch, void *vo)
{
	SKILLTYPE *skill = get_skilltype(sn);

	if (!ch->in_room || !ch->in_room->area || !ch->in_room->area->planet)
	{
		send_to_char("Nie zmienisz pogody tutaj." NL, ch);
		return rNONE;
	}

	if (!str_cmp(target_name, "better") || !str_cmp(target_name, "lepsza"))
		ch->in_room->area->planet->change += dice(level / 3, 4);
	else if (!str_cmp(target_name, "worse") || !str_cmp(target_name, "gorsza"))
		ch->in_room->area->planet->change -= dice(level / 3, 4);
	else
	{
		send_to_char("Ma by� lepsza czy gorsza pogoda?" NL, ch);
		return rSPELL_FAILED;
	}
	successful_casting(skill, ch, NULL, NULL);
	return rNONE;
}

ch_ret spell_cure_blindness(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (!is_affected(victim, gsn_blindness))
		return rSPELL_FAILED;

	if (ch != victim)
	{
		send_to_char("Zgodnie z kodeksem Jedi u�ywasz mocy nios�c pomoc!" NL, ch);
		ch->alignment += 25;
		ch->alignment = URANGE(-1000, ch->alignment, 1000);
		jedi_bonus(ch);
	}

	affect_strip(victim, gsn_blindness);
	send_to_char( COL_FORCE "Tw�j wzrok powr�ci�!" EOL, victim);
	if (ch != victim)
		send_to_char("Ok." NL, ch);
	return rNONE;
}

ch_ret spell_cure_poison(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (is_affected(victim, gsn_poison))
	{
		if (ch != victim)
		{
			send_to_char("Zgodnie z kodeksem Jedi u�ywasz mocy nios�c pomoc!" NL, ch);
			ch->alignment += 25;
			ch->alignment = URANGE(-1000, ch->alignment, 1000);
			jedi_bonus(ch);
		}

		affect_strip(victim, gsn_poison);
		act( COL_FORCE, "$N wygl�da lepiej.", ch, NULL, victim, TO_NOTVICT);
		send_to_char( COL_FORCE "Ciep�e uczucie ogarnia twoje cia�o." EOL, victim);
		victim->mental_state = URANGE(-100, victim->mental_state, -10);
		send_to_char("Ok." NL, ch);
		return rNONE;
	}
	else
		return rSPELL_FAILED;
}

ch_ret spell_curse(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	AFFECT_DATA af;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (IS_AFFECTED(victim, AFF_CURSE))
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}
	af.type = sn;
	af.duration = (4 * level) * DUR_CONV;
	af.location = APPLY_HITROLL;
	af.modifier = -1;
	af.bitvector = AFF_CURSE;
	affect_to_char(victim, &af);

	af.location = APPLY_SAVING_SPELL;
	af.modifier = 1;
	affect_to_char(victim, &af);

	hatred(ch);
	ch->alignment -= 50;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	send_to_char( COL_FORCE "Czujesz brud na sobie i w sobie..." EOL, victim);
	if (ch != victim)
		send_to_char("Ok." NL, ch);
	return rNONE;
}

ch_ret spell_detect_poison(int sn, int level, CHAR_DATA *ch, void *vo)
{
	OBJ_DATA *obj = (OBJ_DATA*) vo;

	if (obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD)
	{
		if (obj->value[3] != 0)
			send_to_char( COL_FORCE
			"Wyczuwasz trucizn� a� kr�ci si� w g�owie." EOL, ch);
		else
			send_to_char( COL_FORCE
			"Hmm.. To wygl�da apetycznie." EOL, ch);
	}
	else
	{
		send_to_char("To nie wygl�da aby by�o zatrute." NL, ch);
	}

	return rNONE;
}

ch_ret spell_dispel_evil(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;
	SKILLTYPE *skill = get_skilltype(sn);

	if (!IS_NPC(ch) && IS_EVIL(ch))
		victim = ch;

	if (IS_GOOD(victim))
	{
		act( COL_FORCE, "�wiat�o zaczyna chroni� $N$3.", ch, NULL, victim, TO_ROOM);
		return rSPELL_FAILED;
	}

	if (IS_NEUTRAL(victim))
	{
		act( COL_FORCE, "$N$2 nic si� nie dzieje.", ch, NULL, victim, TO_CHAR);
		return rSPELL_FAILED;
	}

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	dam = dice(level, 4);
	return damage(ch, victim, dam, sn);
}

ch_ret spell_dispel_force(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int affected_by, cnt;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (victim->affected_by && ch == victim)
	{
		send_to_char( COL_FORCE "Oplatasz d�o�mi swoje cia�o..." EOL, ch);
		while (!victim->affects.empty())
			affect_remove(victim, victim->affects.front());
		victim->affected_by = victim->race->affected;
		return rNONE;
	}
	else if (victim->affected_by == victim->race->affected || level < victim->top_level)
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (!IS_NPC(victim))
	{
		send_to_char("Nie mo�esz tego zrobi�... jeszcze." NL, ch);
		return rSPELL_FAILED;
	}

	cnt = 0;
	for (;;)
	{
		affected_by = 1 << number_bits(5);
		if (IS_SET(victim->affected_by, affected_by))
			break;
		if (cnt++ > 30)
		{
			failed_casting(skill, ch, victim, NULL);
			return rNONE;
		}
	}
	REMOVE_BIT(victim->affected_by, affected_by);
	successful_casting(skill, ch, victim, NULL);

	return rNONE;
}

ch_ret spell_earthquake(int sn, int level, CHAR_DATA *ch, void *vo)
{
	bool ch_died;
	ch_ret retcode;
	SKILLTYPE *skill = get_skilltype(sn);

	ch_died = false;
	retcode = rNONE;

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		failed_casting(skill, ch, NULL, NULL);
		return rSPELL_FAILED;
	}

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	act( COL_FORCE, "Ziemia trz�sie si� pod twoimi stopami!", ch, NULL, NULL, TO_CHAR);
	act( COL_FORCE, "$n sprawia, �e ziemia trz�sie si� i dr�y!", ch, NULL, NULL, TO_ROOM);

	auto char_snapshot = char_list;
	for (auto* vch : char_snapshot)
	{
		if (!vch->in_room)
			continue;
		if (vch->in_room == ch->in_room)
		{
			if (!IS_NPC(vch) && IS_SET(vch->act, PLR_WIZINVIS) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL)
				continue;

			if ( IS_AFFECTED(vch, AFF_FLOATING) || IS_AFFECTED(vch, AFF_FLYING))
				continue;

			if (ch == vch)
				continue;

			retcode = damage(ch, vch, level + dice(2, 8), sn);
			if (retcode == rCHAR_DIED || char_died(ch))
			{
				ch_died = true;
				continue;
			}
			if (char_died(vch))
				continue;
		}

		if (!ch_died && vch->in_room->area == ch->in_room->area)
		{
			send_to_char( COL_FORCE "Ziemia trz�sie si� i dr�y." EOL, vch);
		}
	}

	if (ch_died)
		return rCHAR_DIED;
	else
		return rNONE;
}

//Trog: moze sie przydac do czegos.
ch_ret spell_enchant_weapon(int sn, int level, CHAR_DATA *ch, void *vo)
{
	OBJ_DATA *obj = (OBJ_DATA*) vo;
	AFFECT_DATA *paf;

	if (obj->item_type != ITEM_WEAPON || IS_OBJ_STAT(obj, ITEM_FORCE) || !obj->affects.empty())
		return rSPELL_FAILED;

	/* Bug fix here. -- Alty */
	separate_obj(obj);
	CREATE(paf, AFFECT_DATA, 1);
	paf->type = -1;
	paf->duration = -1;
	paf->location = APPLY_HITROLL;
	paf->modifier = level / 15;
	paf->bitvector = 0;
	obj->affects.push_back(paf);

	CREATE(paf, AFFECT_DATA, 1);
	paf->type = -1;
	paf->duration = -1;
	paf->location = APPLY_DAMROLL;
	paf->modifier = level / 15;
	paf->bitvector = 0;
	obj->affects.push_back(paf);

	if (IS_GOOD(ch))
	{
		SET_BIT(obj->extra_flags, ITEM_ANTI_EVIL);
		act( FB_BLUE, "$p rozb�yskuje na niebiesko.", ch, obj, NULL, TO_CHAR);
	}
	else if (IS_EVIL(ch))
	{
		SET_BIT(obj->extra_flags, ITEM_ANTI_GOOD);
		act( FB_RED, "$p rozb�yskuje na czerwono.", ch, obj, NULL, TO_CHAR);
	}
	else
	{
		SET_BIT(obj->extra_flags, ITEM_ANTI_EVIL);
		SET_BIT(obj->extra_flags, ITEM_ANTI_GOOD);
		act( FB_YELLOW, "$p rozb�yskuje ��tym �wiat�em.", ch, obj, NULL, TO_CHAR);
	}

	send_to_char("Ok." NL, ch);
	return rNONE;
}

/*
 * Drain XP, MANA, HP.
 * Caster gains HP.
 */

ch_ret spell_energy_drain(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;
	int chance;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	hatred(ch);
	ch->alignment -= 200;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	chance = ris_save(victim, victim->top_level, RIS_DRAIN);
	if (chance == 1000)
	{
		failed_casting(skill, ch, victim, NULL); /* SB */
		return rSPELL_FAILED;
	}

	if (victim->top_level <= 2)
		dam = ch->hit + 1;
	else
	{
		dam = number_range(level / 5, level * 2);
		if (victim->mana - dam / 2 <= 0 && victim->move - dam / 2 <= 0)
			dam = 0;
		victim->mana = URANGE(0, victim->mana - dam / 2, victim->mana);
		victim->move = URANGE(0, victim->move - dam / 2, victim->move);
		ch->hit = URANGE(0, ch->hit + dam, ch->max_hit);
		;
	}

	return damage(ch, victim, dam, sn);
}

ch_ret spell_faerie_fire(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	AFFECT_DATA af;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (IS_AFFECTED(victim, AFF_FAERIE_FIRE))
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}
	af.type = sn;
	af.duration = level * DUR_CONV;
	af.location = APPLY_AC;
	af.modifier = 2 * level;
	af.bitvector = AFF_FAERIE_FIRE;
	affect_to_char(victim, &af);
	act( FB_PINK, "Otaczasz si� r�ow� pow�ok�.", victim, NULL, NULL, TO_CHAR);
	act( FB_PINK, "$n otacza si� r�ow� pow�ok�.", victim, NULL, NULL, TO_ROOM);
	return rNONE;
}

ch_ret spell_faerie_fog(int sn, int level, CHAR_DATA *ch, void *vo)
{
	act( COL_FORCE, "$n otacza si� mgie�k� r�owego dymu.", ch, NULL, NULL, TO_ROOM);
	act( COL_FORCE, "Otaczasz si� mgie�k� r�owego dymu.", ch, NULL, NULL, TO_CHAR);

	for (auto* ich : ch->in_room->people)
	{
		if (!IS_NPC(ich) && IS_SET(ich->act, PLR_WIZINVIS))
			continue;

		if (ich == ch)
			continue;

		affect_strip(ich, gsn_invis);
		affect_strip(ich, gsn_sneak);
		REMOVE_BIT(ich->affected_by, AFF_HIDE);
		if (!isDefelInvisible(ch))
			REMOVE_BIT(ich->affected_by, AFF_INVISIBLE);
		if (!IS_RACE(ich, "NOGHRI"))
			REMOVE_BIT(ich->affected_by, AFF_SNEAK);
		act( COL_FORCE, "Kto� ujawnia $n$1!", ich, NULL, NULL, TO_ROOM);
		act( COL_FORCE, "Zostajesz ujawniony!", ich, NULL, NULL, TO_CHAR);
	}
	return rNONE;
}

ch_ret spell_harm(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	dam = UMAX(20, victim->hit - dice(1, 4));
	dam = UMIN(100, dam);
	return damage(ch, victim, dam, sn);
}

ch_ret spell_identify(int sn, int level, CHAR_DATA *ch, void *vo)
{
	/* Modified by Scryn to work on mobs/players/objs */
	OBJ_DATA *obj;
	CHAR_DATA *victim;
	SKILLTYPE *sktmp;
	SKILLTYPE *skill = get_skilltype(sn);

	if (target_name[0] == '\0')
	{
		send_to_char("Co chcesz zidentyfikowa�?" NL, ch);
		return rSPELL_FAILED;
	}

	if ((obj = get_obj_carry(ch, target_name)) != NULL)
	{
		show_obj_stats(obj, ch);
		return rNONE;
	}

	else if ((victim = get_char_room(ch, target_name)) != NULL)
	{

		if (IS_SET(victim->immune, RIS_FORCE))
		{
			immune_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}

		if (IS_NPC(victim))
		{
			ch_printf(ch, "Ofiara wydaje si� mie� poziom mi�dzy %d a %d." NL, victim->top_level - (victim->top_level % 5),
					victim->top_level - (victim->top_level % 5) + 5);
		}
		else
		{
			ch_printf(ch, "Ofiara wydaje si� mie� poziom %d." NL, victim->top_level);
		}

		ch_printf(ch, "Ofiara wygl�da jak %s." NL, get_race(victim));

		if ((chance(ch, 50) && ch->top_level >= victim->top_level + 10) || IS_IMMORTAL(ch))
		{
			ch_printf(ch, "Ofiara zdaje si� by� pod wp�ywem: ");

			if (victim->affects.empty())
			{
				send_to_char("niczego." NL, ch);
				return rNONE;
			}

			for (auto* paf : victim->affects)
			{
				if (victim->affects.size() > 1)
				{
					if (paf != victim->affects.back() && (sktmp = get_skilltype(paf->type)) != NULL)
						ch_printf(ch, "%s, ", sktmp->name);

					if (paf == victim->affects.back() && (sktmp = get_skilltype(paf->type)) != NULL)
					{
						ch_printf(ch, "i %s." NL, sktmp->name);
						return rNONE;
					}
				}
				else
				{
					if ((sktmp = get_skilltype(paf->type)) != NULL)
						ch_printf(ch, "%s." NL, sktmp->name);
					else
						send_to_char( NL, ch);
					return rNONE;
				}
			}
		}
	}

	else
	{
		ch_printf(ch, "Nie widzisz tutaj %s!" NL, target_name);
		return rSPELL_FAILED;
	}
	return rNONE;
}

ch_ret spell_invis(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim;
	SKILLTYPE *skill = get_skilltype(sn);

	/* Modifications on 1/2/96 to work on player/object - Scryn */

	if (target_name[0] == '\0')
		victim = ch;
	else
		victim = get_char_room(ch, target_name);

	if (victim)
	{
		AFFECT_DATA af;

		if (IS_SET(victim->immune, RIS_FORCE))
		{
			immune_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}

		if (IS_AFFECTED(victim, AFF_FORCEINVIS))
		{
			failed_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}

		act( COL_FORCE, "$n znika we mgle.", victim, NULL, NULL, TO_ROOM);
		af.type = sn;
		af.duration = ((level / 4) + 12) * DUR_CONV;
		af.location = APPLY_AFFECT;
		af.modifier = 0;
		af.bitvector = AFF_FORCEINVIS;
		affect_to_char(victim, &af);
		act( COL_FORCE, "Znikasz we mgle.", victim, NULL, NULL, TO_CHAR);
		return rNONE;
	}
	else
	{
		OBJ_DATA *obj;

		obj = get_obj_carry(ch, target_name);

		if (obj)
		{
			if ( IS_OBJ_STAT(obj, ITEM_INVIS) || chance(ch, 40 + level / 10))
			{
				failed_casting(skill, ch, NULL, NULL);
				return rSPELL_FAILED;
			}

			SET_BIT(obj->extra_flags, ITEM_INVIS);
			act( COL_FORCE, "$p znika we mgle.", ch, obj, NULL, TO_CHAR);
			return rNONE;
		}
	}
	ch_printf(ch, "Nie mo�esz znale�� %s!" NL, target_name);
	return rSPELL_FAILED;
}

ch_ret spell_know_alignment(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	const char *msg;
	int ap;
	SKILLTYPE *skill = get_skilltype(sn);

	if (!victim)
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	ap = victim->alignment;

	if (ap > 700)
		msg = "Aura bia�a jak �niegi Hoth otacza $N$3.";
	else if (ap > 350)
		msg = "$N jest wzorem moralno�ci i dobra.";
	else if (ap > 100)
		msg = "$N jest mi�$Y i uczynn$Y.";
	else if (ap > -100)
		msg = "$N nie jest najszlachetniejsz$Y.";
	else if (ap > -350)
		msg = "$N cz�sto ok�amuje przyjaci�.";
	else if (ap > -700)
		msg = "$N wygl�da tak, jakby mia� za chwil� ci� zabi�!";
	else
		msg = "$N to z�o, kt�re przechodzi ci� dreszczem.";

	act( COL_FORCE, msg, ch, NULL, victim, TO_CHAR);
	return rNONE;
}

ch_ret spell_lightning_bolt(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	static const int dam_each[] =
	{ 1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98,
			100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148,
			150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170 };

	int dam;

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	level = UMIN(level, (int )(sizeof(dam_each) / sizeof(dam_each[0]) - 1));
	level = UMAX(0, level);
	dam = number_range(dam_each[level] / 2, dam_each[level] * 2);
	return damage(ch, victim, dam, sn);
}

ch_ret spell_pass_door(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	AFFECT_DATA af;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (IS_AFFECTED(victim, AFF_PASS_DOOR))
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}
	af.type = sn;
	af.duration = number_fuzzy(level / 4) * DUR_CONV;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_PASS_DOOR;
	affect_to_char(victim, &af);
	act( COL_FORCE, "$n rozp�ywa si� zmieniaj�c swe cia�o w przezroczyst� plazm�.", victim, NULL, NULL, TO_ROOM);
	act( COL_FORCE, "Rozp�ywasz si� zmieniaj�c swe cia�o w przezroczyst� plazm�.", victim, NULL, NULL, TO_CHAR);
	return rNONE;
}

ch_ret spell_poison(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	AFFECT_DATA af;
	int chance;
	bool first = true;

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	chance = ris_save(victim, level, RIS_POISON);
	if (chance == 1000 || saves_poison_death(chance, victim))
		return rSPELL_FAILED;
	if (IS_AFFECTED(victim, AFF_POISON))
		first = false;
	af.type = sn;
	af.duration = level * DUR_CONV;
	af.location = APPLY_STR;
	af.modifier = -2;
	af.bitvector = AFF_POISON;
	affect_join(victim, &af);
	send_to_char( COL_FORCE, victim);
	send_to_char("Czujesz, �e jest ci bardzo niedobrze." NL, victim);
	victim->mental_state = URANGE(20, victim->mental_state + (first ? 5 : 0), 100);
	if (ch != victim)
		send_to_char("Ok." NL, ch);
	return rNONE;
}

//Trog: to trzeba zrobic.
ch_ret spell_remove_curse(int sn, int level, CHAR_DATA *ch, void *vo)
{
	return rNONE;
}

ch_ret spell_remove_trap(int sn, int level, CHAR_DATA *ch, void *vo)
{
	OBJ_DATA *obj;
	OBJ_DATA *trap;
	bool found;
	int retcode;
	SKILLTYPE *skill = get_skilltype(sn);

	if (!target_name || target_name[0] == '\0')
	{
		send_to_char("Jak� pu�apk� chcesz rozbroi�?" NL, ch);
		return rSPELL_FAILED;
	}

	found = false;

	if (ch->in_room->contents.empty())
	{
		send_to_char("Nie widzisz niczego takiego." NL, ch);
		return rNONE;
	}

	for (auto* o : ch->in_room->contents)
		if (can_see_obj(ch, o) && nifty_is_name(target_name, o->name))
		{
			obj = o;
			found = true;
			break;
		}

	if (!found)
	{
		send_to_char("Nie widzisz niczego takiego." NL, ch);
		return rSPELL_FAILED;
	}

	if ((trap = get_trap(obj)) == NULL)
	{
		failed_casting(skill, ch, NULL, NULL);
		return rSPELL_FAILED;
	}

	if (chance(ch, 70 + get_curr_wis(ch)))
	{
		send_to_char("Ooops!" NL, ch);
		retcode = spring_trap(ch, trap);
		if (retcode == rNONE)
			retcode = rSPELL_FAILED;
		return retcode;
	}

	extract_obj(trap);

	successful_casting(skill, ch, NULL, NULL);
	return rNONE;
}

ch_ret spell_shocking_grasp(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	static const int dam_each[] =
	{ 1, 2, 4, 6, 8, 10, 15, 20, 25, 29, 33, 36, 39, 39, 39, 40, 40, 41, 41, 42, 42, 43, 43, 44, 44, 45, 45, 46, 46, 47, 47, 48, 48, 49, 49,
			50, 50, 51, 51, 52, 52, 53, 53, 54, 54, 55, 55, 56, 56, 57, 57, 58, 58, 59, 59, 60, 60, 61, 61, 62, 62, 63, 63, 64, 64, 65, 65,
			66, 66, 67, 67 };
	int dam;

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	level = UMIN(level, (int )(sizeof(dam_each) / sizeof(dam_each[0]) - 1));
	level = UMAX(0, level);
	dam = number_range(dam_each[level] / 2, dam_each[level] * 2);
	return damage(ch, victim, dam, sn);
}

ch_ret spell_sleep(int sn, int level, CHAR_DATA *ch, void *vo)
{
	AFFECT_DATA af;
	int retcode;
	int chance;
	int tmp;
	CHAR_DATA *victim;
	SKILLTYPE *skill = get_skilltype(sn);

	if ((victim = get_char_room(ch, target_name)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return rSPELL_FAILED;
	}

	if (!IS_NPC(victim) && victim->fighting)
	{
		send_to_char("Nie uda ci si� u�pi� gracza podczas walki." NL, ch);
		return rSPELL_FAILED;
	}

	if (is_safe(ch, victim))
		return rSPELL_FAILED;

	if ( IS_NPC( victim ) && IS_SET(victim->act, ACT_DROID))
	{
		send_to_char("Droid nie potrzebuje snu." NL, ch);
		return rSPELL_FAILED;
	}

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if ( SPELL_FLAG(skill, SF_PKSENSITIVE) && !IS_NPC(ch) && !IS_NPC(victim))
		tmp = level;
	else
		tmp = level;

	add_inform(victim, ch);

	if ( IS_AFFECTED(victim, AFF_SLEEP) || (chance = ris_save(victim, tmp, RIS_SLEEP)) == 1000 || level < victim->top_level
			|| (victim != ch && IS_SET(victim->in_room->room_flags, ROOM_SAFE)))
	{
		failed_casting(skill, ch, victim, NULL);
		if (ch == victim)
			return rSPELL_FAILED;
		if (!victim->fighting)
		{
			retcode = multi_hit(victim, ch, TYPE_UNDEFINED);
			if (retcode == rNONE)
				retcode = rSPELL_FAILED;
			return retcode;
		}
	}
	af.type = sn;
	af.duration = (4 + level) * DUR_CONV;
	af.location = APPLY_NONE;
	af.modifier = 0;
	af.bitvector = AFF_SLEEP;
	affect_join(victim, &af);

	/* Added by Narn at the request of Dominus. */
	if (!IS_NPC(victim))
	{
		sprintf(log_buf, "%s has cast sleep on %s.", ch->name, victim->name);
		log_string_plus(log_buf, LOG_NORMAL, ch->top_level);
		to_channel(log_buf, CHANNEL_MONITOR, "Monitor", UMAX(LEVEL_IMMORTAL, ch->top_level));
	}

	if (IS_AWAKE(victim))
	{
		act( COL_FORCE, "Czujesz, �e    chce...  ci.. si�. ..   spa�.. .. . Zzzzzz.", victim, NULL, NULL, TO_CHAR);
		act( COL_FORCE, "$n k�adzie si� do snu i zasypia.", victim, NULL, NULL, TO_ROOM);
		victim->position = POS_SLEEPING;
		fevent_trigger(victim, FE_POSITION);
	}
	if (IS_NPC(victim))
		start_hating(victim, ch);

	return rNONE;
}

ch_ret spell_ventriloquate(int sn, int level, CHAR_DATA *ch, void *vo)
{
	char buf1[MAX_STRING_LENGTH];
	char speaker[MAX_INPUT_LENGTH];

	target_name = one_argument(target_name, speaker);

	sprintf(buf1, "%s m�wi '%s'." NL, speaker, target_name);
	buf1[0] = UPPER(buf1[0]);

	for (auto* vch : ch->in_room->people)
	{
		if (!is_name(speaker, vch->name))
		{
			send_to_char( FG_CYAN, vch);
			send_to_char(buf1, vch);
		}
	}

	return rNONE;
}

ch_ret spell_weaken(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	AFFECT_DATA af;
	SKILLTYPE *skill = get_skilltype(sn);

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}
	if (is_affected(victim, sn))
		return rSPELL_FAILED;
	af.type = sn;
	af.duration = level / 2 * DUR_CONV;
	af.location = APPLY_STR;
	af.modifier = -2;
	af.bitvector = 0;
	affect_to_char(victim, &af);
	send_to_char( COL_FORCE, victim);
	ch_printf(victim, "Czujesz si� s�absz%s." NL, SEX_SUFFIX_YAE(victim));
	if (ch != victim)
		send_to_char("Ok." NL, ch);
	return rNONE;
}

/*
 * A spell as it should be				-Thoric
 *
 * Trog: gowno a nie spells :)
 * NPC spells.
 */
ch_ret spell_acid_breath(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;
	int hpch;

	if (chance(ch, 2 * level) && !saves_breath(level, victim))
	{
		auto carrying_snapshot = victim->carrying;
		for (auto* obj_lose : carrying_snapshot)
		{
			int iWear;

			if (number_bits(2) != 0)
				continue;

			switch (obj_lose->item_type)
			{
			case ITEM_ARMOR:
				if (obj_lose->value[0] > 0)
				{
					separate_obj(obj_lose);
					act( COL_ACTION, "$p topi si�!", victim, obj_lose, NULL, TO_CHAR);
					if ((iWear = obj_lose->wear_loc) != WEAR_NONE)
						victim->armor -= apply_ac(obj_lose, iWear);
					obj_lose->value[0] -= 1;
					obj_lose->cost = 0;
					if (iWear != WEAR_NONE)
						victim->armor += apply_ac(obj_lose, iWear);
				}
				break;

			case ITEM_CONTAINER:
				separate_obj(obj_lose);
				act( COL_ACTION, "$p syczy i rozsypuje si� na kawa�ki!", victim, obj_lose, NULL, TO_CHAR);
				act( COL_OBJECT, "Zawarto�� $p$1 wypada na ziemi�.", victim, obj_lose, NULL, TO_ROOM);
				act( COL_OBJECT, "Zawarto�� $p$1 wypada na ziemi�.", victim, obj_lose, NULL, TO_CHAR);
				empty_obj(obj_lose, NULL, victim->in_room);
				extract_obj(obj_lose);
				break;
			}
		}
	}

	hpch = UMAX(10, ch->hit);
	dam = number_range(hpch / 16 + 1, hpch / 8);
	if (saves_breath(level, victim))
		dam /= 2;
	return damage(ch, victim, dam, sn);
}

ch_ret spell_fire_breath(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;
	int hpch;

	if (chance(ch, 2 * level) && !saves_breath(level, victim))
	{
		auto carrying_snapshot2 = victim->carrying;
		for (auto* obj_lose : carrying_snapshot2)
		{
			const char *msg;

			if (number_bits(2) != 0)
				continue;

			switch (obj_lose->item_type)
			{
			default:
				continue;
			case ITEM_CONTAINER:
				msg = "$p ignites and burns!";
				break;
			case ITEM_POTION:
				msg = "$p bubbles and boils!";
				break;
			case ITEM_DEVICE:
				msg = "$p sparks and sputters!";
				break;
			case ITEM_FOOD:
				msg = "$p blackens and crisps!";
				break;
			case ITEM_PILL:
				msg = "$p melts and drips!";
				break;
			}

			separate_obj(obj_lose);
			act( COL_ACTION, msg, victim, obj_lose, NULL, TO_CHAR);
			if (obj_lose->item_type == ITEM_CONTAINER)
			{
				act( COL_OBJECT, "The contents of $p spill out onto the ground.", victim, obj_lose, NULL, TO_ROOM);
				act( COL_OBJECT, "The contents of $p spill out onto the ground.", victim, obj_lose, NULL, TO_CHAR);
				empty_obj(obj_lose, NULL, victim->in_room);
			}
			extract_obj(obj_lose);
		}
	}

	hpch = UMAX(10, ch->hit);
	dam = number_range(hpch / 16 + 1, hpch / 8);
	if (saves_breath(level, victim))
		dam /= 2;
	return damage(ch, victim, dam, sn);
}

ch_ret spell_frost_breath(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;
	int hpch;

	if (chance(ch, 2 * level) && !saves_breath(level, victim))
	{
		auto carrying_snapshot3 = victim->carrying;
		for (auto* obj_lose : carrying_snapshot3)
		{
			const char *msg;

			if (number_bits(2) != 0)
				continue;

			switch (obj_lose->item_type)
			{
			default:
				continue;
			case ITEM_CONTAINER:
			case ITEM_DRINK_CON:
			case ITEM_POTION:
				msg = "$p freezes and shatters!";
				break;
			}

			separate_obj(obj_lose);
			act( COL_ACTION, msg, victim, obj_lose, NULL, TO_CHAR);
			if (obj_lose->item_type == ITEM_CONTAINER)
			{
				act( COL_OBJECT, "The contents of $p spill out onto the ground.", victim, obj_lose, NULL, TO_ROOM);
				act( COL_OBJECT, "The contents of $p spill out onto the ground.", victim, obj_lose, NULL, TO_CHAR);
				empty_obj(obj_lose, NULL, victim->in_room);
			}
			extract_obj(obj_lose);
		}
	}

	hpch = UMAX(10, ch->hit);
	dam = number_range(hpch / 16 + 1, hpch / 8);
	if (saves_breath(level, victim))
		dam /= 2;
	return damage(ch, victim, dam, sn);
}

ch_ret spell_gas_breath(int sn, int level, CHAR_DATA *ch, void *vo)
{
	int dam;
	int hpch;
	bool ch_died;

	ch_died = false;

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char( COL_FORCE, ch);
		send_to_char("You fail to breathe." NL, ch);
		return rNONE;
	}

	auto people_snapshot = ch->in_room->people;
	for (auto* vch : people_snapshot)
	{
		if (!IS_NPC(vch) && IS_SET(vch->act, PLR_WIZINVIS) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL)
			continue;

		if ( IS_NPC(ch) ? !IS_NPC(vch) : IS_NPC(vch))
		{
			hpch = UMAX(10, ch->hit);
			dam = number_range(hpch / 16 + 1, hpch / 8);
			if (saves_breath(level, vch))
				dam /= 2;
			if (damage(ch, vch, dam, sn) == rCHAR_DIED || char_died(ch))
				ch_died = true;
		}
	}
	if (ch_died)
		return rCHAR_DIED;
	else
		return rNONE;
}

ch_ret spell_lightning_breath(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;
	int hpch;

	hpch = UMAX(10, ch->hit);
	dam = number_range(hpch / 16 + 1, hpch / 8);
	if (saves_breath(level, victim))
		dam /= 2;
	return damage(ch, victim, dam, sn);
}

ch_ret spell_null(int sn, int level, CHAR_DATA *ch, void *vo)
{
	send_to_char("That's not a spell!" NL, ch);
	return rNONE;
}

/* don't remove, may look redundant, but is important */
ch_ret spell_notfound(int sn, int level, CHAR_DATA *ch, void *vo)
{
	send_to_char("That's not a spell!" NL, ch);
	return rNONE;
}

/*
 *   Haus' Spell Additions
 *
 */

/* to do: portal           (like mpcreatepassage)
 *        enchant armour?  (say -1/-2/-3 ac )
 *        sharpness        (makes weapon of caster's level)
 *        repair           (repairs armor)
 *        blood burn       (offensive)  * name: net book of spells *
 *        spirit scream    (offensive)  * name: net book of spells *
 *        something about saltpeter or brimstone
 */

ch_ret spell_farsight(int sn, int level, CHAR_DATA *ch, void *vo)
{
	ROOM_INDEX_DATA *location;
	ROOM_INDEX_DATA *original;
	CHAR_DATA *victim;
	SKILLTYPE *skill = get_skilltype(sn);

	/* The spell fails if the victim isn't playing, the victim is the caster,
	 the target room has private, solitary, noastral, death or proto flags,
	 the caster's room is norecall, the victim is too high in level, the
	 victim is a proto mob, the victim makes the saving throw or the pkill
	 flag on the caster is not the same as on the victim.
	 No albo jesli ofiara jest na innej planecie niz farsighciarz. :-)
	 */
	if (!(victim = get_char_world(ch, target_name)) || victim == ch || !(location = victim->in_room)
			|| (!victim->in_room->area->planet && !ch->in_room->area->planet) || victim->in_room->area->planet != ch->in_room->area->planet
			|| IS_SET(victim->in_room->room_flags, ROOM_PRIVATE) || IS_SET(victim->in_room->room_flags, ROOM_PROTOTYPE)
			|| IS_SET(victim->in_room->room_flags, ROOM_NO_FIND) || victim->top_level >= level + 15
			|| (IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE)))
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (IS_IMMORTAL(victim))
	{
		ch_printf(victim, "%s spieguje Ci� za po�rednictwem mocy." NL, ch->przypadki[0]);

		if (!IS_IMMORTAL(ch))
		{
			AFFECT_DATA af;
			af.type = sn;
			af.location = APPLY_HITROLL;
			af.modifier = -level;
			af.duration = level * DUR_CONV;
			af.bitvector = AFF_BLIND;

			failed_casting(skill, ch, victim, NULL);
			affect_to_char(ch, &af);
			ch_printf(ch, "Blask %s o�lepia Ci�!" NL, victim->przypadki[1]);
			return rSPELL_FAILED;
		}
	}

	successful_casting(skill, ch, victim, NULL);
	original = ch->in_room;
	char_from_room(ch);
	char_to_room(ch, location);
	do_look(ch, (char*) "auto");
	char_from_room(ch);
	char_to_room(ch, original);
	return rNONE;
}

/* Scryn 2/2/96 */
ch_ret spell_remove_invis(int sn, int level, CHAR_DATA *ch, void *vo)
{
	OBJ_DATA *obj;
	SKILLTYPE *skill = get_skilltype(sn);

	if (target_name[0] == '\0')
	{
		send_to_char("Co lub kto ma przesta� by� niewidzialnym?" NL, ch);
		return rSPELL_FAILED;
	}

	obj = get_obj_carry(ch, target_name);

	if (obj)
	{
		if (!IS_OBJ_STAT(obj, ITEM_INVIS))
			return rSPELL_FAILED;

		REMOVE_BIT(obj->extra_flags, ITEM_INVIS);
		act( COL_FORCE, "$p delikatnie pojawia si� w twoich d�oniach.", ch, obj, NULL, TO_CHAR);

		send_to_char("Ok." NL, ch);
		return rNONE;
	}
	else
	{
		CHAR_DATA *victim;

		victim = get_char_room(ch, target_name);

		if (victim)
		{
			if (!can_see(ch, victim))
			{
				ch_printf(ch, "Nie widzisz �adnego %s!" NL, target_name);
				return rSPELL_FAILED;
			}

			if (isDefelInvisible(victim))
				return rSPELL_FAILED;

			if (!IS_AFFECTED(victim, AFF_INVISIBLE)) //Trog: moze tutaj trzeba forceinvis, MOZE.
			{
				ch_printf(ch, "Ale� twoja ofiara jest jak najbardziej widzialna!" NL);
				return rSPELL_FAILED;
			}

			if (is_safe(ch, victim))
			{
				failed_casting(skill, ch, victim, NULL);
				return rSPELL_FAILED;
			}

			if (IS_SET(victim->immune, RIS_FORCE))
			{
				immune_casting(skill, ch, victim, NULL);
				return rSPELL_FAILED;
			}
			if (!IS_NPC(victim))
			{
				if (chance(ch, 50) && ch->skill_level[FORCE_ABILITY] < victim->top_level)
				{
					failed_casting(skill, ch, victim, NULL);
					return rSPELL_FAILED;
				}

			}
			else
			{
				if (chance(ch, 50) && ch->skill_level[FORCE_ABILITY] + 15 < victim->top_level)
				{
					failed_casting(skill, ch, victim, NULL);
					return rSPELL_FAILED;
				}
			}

			affect_strip(victim, gsn_invis);
//    	affect_strip ( victim, gsn_forceinvis                   );
			REMOVE_BIT(victim->affected_by, AFF_INVISIBLE);

			/* BTW: to tak bez komunikatu, �e ofiara jest widzialna? Mo�e ofiara nie
			 widzi tego od razu, ale OTOCZENIE? Chyba powinno si� kapn��...
			 No chyba, �e affect_strip() to za�atwia... 		-- Than */

			send_to_char("Ok." NL, ch);
			return rNONE;
		}

		ch_printf(ch, "Nie widzisz tu �adnego %s!" NL, target_name);
		return rSPELL_FAILED;
	}
}

/*
 * Animate Dead: Scryn 3/2/96
 * Modifications by Altrag 16/2/96
 * Trog: to trzeba na skill przerobic i bedzie fajnie :-)
 */
ch_ret spell_animate_dead(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *mob;
	OBJ_DATA *corpse;
	bool found;
	MOB_INDEX_DATA *pMobIndex;
	AFFECT_DATA af;
	char buf[MAX_STRING_LENGTH];
	SKILLTYPE *skill = get_skilltype(sn);
	int i;
	found = false;

	for (auto* c : ch->in_room->contents)
	{
		if (c->item_type == ITEM_CORPSE_NPC && c->cost != -5)
		{
			corpse = c;
			found = true;
			break;
		}
	}

	if (!found)
	{
		send_to_char("You cannot find a suitable corpse here." NL, ch);
		return rSPELL_FAILED;
	}

	if (get_mob_index(MOB_VNUM_ANIMATED_CORPSE) == NULL)
	{
		bug("Vnum 5 not found for spell_animate_dead!", 0);
		return rNONE;
	}

	if ((pMobIndex = get_mob_index((int) abs(corpse->cost))) == NULL)
	{
		bug("Can not find mob for cost of corpse, spell_animate_dead", 0);
		return rSPELL_FAILED;
	}

	if (!IS_NPC(ch))
	{
		if (ch->mana - (pMobIndex->level * 4) < 0)
		{
			send_to_char("You do not have enough mana to reanimate this "
					"corpse." NL, ch);
			return rSPELL_FAILED;
		}
		else
			ch->mana -= (pMobIndex->level * 4);
	}

	if ( IS_IMMORTAL(ch) || (chance(ch, 75) && pMobIndex->level - ch->top_level < 10))
	{
		mob = create_mobile(get_mob_index(MOB_VNUM_ANIMATED_CORPSE));
		char_to_room(mob, ch->in_room);
		mob->top_level = UMIN(ch->top_level / 2, pMobIndex->level);
		mob->race = pMobIndex->race; /* should be undead */

		/* Fix so mobs wont have 0 hps and crash mud - Scryn 2/20/96 */
		if (!pMobIndex->hitnodice)
			mob->max_hit = pMobIndex->level * 8
					+ number_range(pMobIndex->level * pMobIndex->level / 4, pMobIndex->level * pMobIndex->level);
		else
			mob->max_hit = dice(pMobIndex->hitnodice, pMobIndex->hitsizedice) + pMobIndex->hitplus;
		mob->max_hit = UMAX(URANGE( mob->max_hit / 4, (mob->max_hit * corpse->value[3]) / 100, ch->top_level * dice(20,10)), 1);

		mob->hit = mob->max_hit;
		mob->damroll = ch->top_level / 8;
		mob->hitroll = ch->top_level / 6;
		mob->alignment = ch->alignment;

		act(COL_FORCE, "$n makes $T rise from the grave!", ch, NULL, pMobIndex->przypadki[0], TO_ROOM);
		act(COL_FORCE, "You make $T rise from the grave!", ch, NULL, pMobIndex->przypadki[0], TO_CHAR);

		sprintf(buf, "animated corpse %s", pMobIndex->player_name);
		STRDUP(mob->name, buf);

		sprintf(buf, "The animated corpse of %s", pMobIndex->przypadki[1]);
		for (i = 0; i < 6; i++)
			STRDUP(mob->przypadki[i], buf);
		sprintf(buf, "An animated corpse of %s struggles with the horror of its undeath." NL, pMobIndex->przypadki[1]);
		STRDUP(mob->long_descr, buf);
		add_follower(mob, ch);
		af.type = sn;
		af.duration = (number_fuzzy((level + 1) / 4) + 1) * DUR_CONV;
		af.location = 0;
		af.modifier = 0;
		af.bitvector = AFF_CHARM;
		affect_to_char(mob, &af);

		if (!corpse->contents.empty())
		{
			auto corpse_snapshot = corpse->contents;
			for (auto* obj : corpse_snapshot)
			{
				obj_from_obj(obj);
				obj_to_room(obj, corpse->in_room);
			}
		}

		separate_obj(corpse);
		extract_obj(corpse);
		return rNONE;
	}
	else
	{
		failed_casting(skill, ch, NULL, NULL);
		return rSPELL_FAILED;
	}
}

/* Works now.. -- Altrag */
ch_ret spell_possess(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	AFFECT_DATA af;
	SKILLTYPE *skill = get_skilltype(sn);

	if (!ch->desc || ch->desc->original)
	{
		send_to_char("Tw�j stan nie pozwala ci na to." NL, ch);
		return rSPELL_FAILED;
	}

	if ((victim = get_char_room(ch, target_name)) == NULL)
	{
		hes_not_here_msg(ch);
		return rSPELL_FAILED;
	}

	if (victim == ch)
	{
		send_to_char("Hmm.. Jak chcesz posi��� swoj� w�asn� wol�?" NL, ch);
		return rSPELL_FAILED;
	}

	if (!IS_NPC(victim))
	{
		send_to_char("Nie mo�esz posi��� woli innego gracza!" NL, ch);
		return rSPELL_FAILED;
	}

	if (IS_SET(victim->act, ACT_DROID))
	{
		send_to_char("Maszyny nie maj� umys�u." NL, ch);
		return rSPELL_FAILED;
	}

	if (victim->desc)
	{
		ch_printf(ch, "Kto� ju� kontroluje zachowanie %s" PLAIN "." NL, victim->przypadki[1]);
		return rSPELL_FAILED;
	}

	if (IS_SET(victim->immune, RIS_FORCE))
	{
		immune_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if ( IS_AFFECTED(victim, AFF_POSSESS) || level < (victim->top_level + get_curr_int(victim)) || victim->desc || !chance(ch, 25))
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	hatred(ch);
	ch->alignment -= 50;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	af.type = sn;
	af.duration = (20 + get_curr_wis(ch) / 2) * get_curr_int(ch) / 2 - get_curr_int(victim) * 2;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = AFF_POSSESS;
	affect_to_char(victim, &af);

	sprintf(buf, "Tw�j umys� przejmuje kontrol� nad cia�em %s" PLAIN "!" NL, NAME(victim, 1));

	ch->desc->character = victim;
	ch->desc->original = ch;
	victim->desc = ch->desc;
	ch->desc = NULL;
	ch->switched = victim;
	send_to_char(buf, victim);

	return rNONE;
}

/* Ignores pickproofs, but can't unlock containers. -- Altrag 17/2/96
 * Trog: fajne, trzeba tylko skeditem stworzyc. */
ch_ret spell_knock(int sn, int level, CHAR_DATA *ch, void *vo)
{
	EXIT_DATA *pexit;
	SKILLTYPE *skill = get_skilltype(sn);

	send_to_char(COL_FORCE, ch);
	/*
	 * shouldn't know why it didn't work, and shouldn't work on pickproof
	 * exits.  -Thoric
	 */
	if (!(pexit = find_door(ch, target_name, false)) || !IS_SET(pexit->flags, EX_CLOSED) || !IS_SET(pexit->flags, EX_LOCKED)
	|| IS_SET(pexit->flags, EX_PICKPROOF))
	{
		failed_casting(skill, ch, NULL, NULL);
		return rSPELL_FAILED;
	}

	REMOVE_BIT(pexit->flags, EX_LOCKED);
	send_to_char("*Click*" NL, ch);

	if (pexit->rexit && pexit->rexit->to_room == ch->in_room)
		REMOVE_BIT(pexit->rexit->flags, EX_LOCKED);

	check_room_for_traps(ch, TRAP_UNLOCK | trap_door[pexit->vdir]);
	return rNONE;
}

/* Tells to sleepers in are. -- Altrag 17/2/96 */
ch_ret spell_dream(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim;
	char arg[MAX_INPUT_LENGTH];

	target_name = one_argument(target_name, arg);
	send_to_char(COL_FORCE, ch);

	if (!(victim = get_char_world(ch, arg)))
	{
		hes_not_here_msg(ch);
		return rSPELL_FAILED;
	}

	if (victim->position != POS_SLEEPING)
	{
		ch_printf(ch, "%s" PLAIN " nie �pi." NL, victim->przypadki[0]);
		return rSPELL_FAILED;
	}

	if (!target_name)
	{
		send_to_char("O czym ma �ni� ta osoba?" NL, ch);
		return rSPELL_FAILED;
	}

	send_to_char(COL_TELL, victim);

	ch_printf(victim, FB_WHITE "�ni ci si�, �e %s" FB_WHITE " m�wi ci '%s" FB_WHITE "'." NL, PERS(ch, victim, 0), target_name);

	send_to_char("Ok." NL, ch);
	return rNONE;
}

ch_ret spell_scorching_surge(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	static const int dam_each[] =
	{ 1, 1, 2, 3, 4, 5, 6, 8, 10, 12, 14, 16, 18, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 82, 84, 86, 88, 90, 92, 94, 96, 98,
			100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126, 128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148,
			150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170 };
	int dam;

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	level = UMIN(level / 2, (int )(sizeof(dam_each) / sizeof(dam_each[0]) - 1));
	level = UMAX(0, level);
	dam = number_range(dam_each[level], dam_each[level] * 5);

	act( COL_FORCE, "Ognista furia w postaci czystej energii emanuje z $n$1!", ch, NULL, NULL, TO_ROOM);
	act( COL_FORCE, "Ognista furia w postaci czystej energii emanuje z ciebie!", ch, NULL, NULL, TO_CHAR);
	return damage(ch, victim, dam, sn);
}

/*******************************************************
 * Everything after this point is part of SMAUG SPELLS *
 *******************************************************/
/*
 * saving throw check						-Thoric
 */
bool check_save(int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim)
{
	SKILLTYPE *skill = get_skilltype(sn);
	bool saved = false;

	if ( SPELL_FLAG(skill, SF_PKSENSITIVE) && !IS_NPC(ch) && !IS_NPC(victim))
		level /= 2;

	if (skill->saves)
		switch (skill->saves)
		{
		case SS_POISON_DEATH:
			saved = saves_poison_death(level, victim);
			break;
		case SS_PARA_PETRI:
			saved = saves_para_petri(level, victim);
			break;
		case SS_BREATH:
			saved = saves_breath(level, victim);
			break;
		}
	return saved;
}

/*
 * Generic offensive spell damage attack			-Thoric
 */
ch_ret spell_attack(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	SKILLTYPE *skill = get_skilltype(sn);
	bool saved = check_save(sn, level, ch, victim);
	int dam;
	ch_ret retcode;

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	if (saved && !SPELL_FLAG(skill, SF_SAVE_HALF_DAMAGE))
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}
	if (skill->dice)
		dam = UMAX(0, dice_parse(ch, level, skill->dice));
	else
		dam = dice(1, level);
	if (saved)
		dam /= 2;
	retcode = damage(ch, victim, dam, sn);
	if (retcode == rNONE && skill->affects && !char_died(ch) && !char_died(victim))
		retcode = spell_affectchar(sn, level, ch, victim);

	return retcode;
}

/*
 * Generic area attack						-Thoric
 */
ch_ret spell_area_attack(int sn, int level, CHAR_DATA *ch, void *vo)
{
	SKILLTYPE *skill = get_skilltype(sn);
	bool saved;
	bool affects;
	int dam;
	ch_ret retcode = 0;

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		failed_casting(skill, ch, NULL, NULL);
		return rSPELL_FAILED;
	}

	affects = (skill->affects ? true : false);
	if (skill->hit_char && *skill->hit_char != '\0')
		act( COL_FORCE, skill->hit_char, ch, NULL, NULL, TO_CHAR);
	if (skill->hit_room && *skill->hit_room != '\0')
		act( COL_FORCE, skill->hit_room, ch, NULL, NULL, TO_ROOM);

	auto people_snapshot2 = ch->in_room->people;
	for (auto* vch : people_snapshot2)
	{
		if (!IS_NPC(vch) && IS_SET(vch->act, PLR_WIZINVIS) && vch->pcdata->wizinvis >= LEVEL_IMMORTAL)
			continue;

		if (vch != ch && !is_same_group(ch, vch))
		{
			saved = check_save(sn, level, ch, vch);
			if (saved && !SPELL_FLAG(skill, SF_SAVE_HALF_DAMAGE))
			{
				failed_casting(skill, ch, vch, NULL);
				dam = 0;
			}
			else if (skill->dice)
				dam = dice_parse(ch, level, skill->dice);
			else
				dam = dice(1, level);
			if (saved && SPELL_FLAG(skill, SF_SAVE_HALF_DAMAGE))
				dam /= 2;
			retcode = damage(ch, vch, dam, sn);
		}
		if (retcode == rNONE && affects && !char_died(ch) && !char_died(vch))
			retcode = spell_affectchar(sn, level, ch, vch);
		if (retcode == rCHAR_DIED || char_died(ch))
		{
			break;
		}
	}
	return retcode;
}

ch_ret spell_affectchar(int sn, int level, CHAR_DATA *ch, void *vo)
{
	AFFECT_DATA af;
	SMAUG_AFF *saf;
	SKILLTYPE *skill = get_skilltype(sn);
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int chance;
	ch_ret retcode = rNONE;

	if (SPELL_FLAG(skill, SF_RECASTABLE))
		affect_strip(victim, sn);
	for (saf = skill->affects; saf; saf = saf->next)
	{
		if (saf->location >= REVERSE_APPLY)
			victim = ch;
		else
			victim = (CHAR_DATA*) vo;
		/* Check if char has this bitvector already */
		if ((af.bitvector = saf->bitvector) != 0 && IS_AFFECTED(victim, af.bitvector) && !SPELL_FLAG(skill, SF_ACCUMULATIVE))
			continue;
		/*
		 * necessary for affect_strip to work properly...
		 */
		switch (af.bitvector)
		{
		default:
			af.type = sn;
			break;

		case AFF_POISON:
			af.type = gsn_poison;
			hatred(ch);
			ch->alignment -= 100;
			ch->alignment = URANGE(-1000, ch->alignment, 1000);
			sith_penalty(ch);
			chance = ris_save(victim, level, RIS_POISON);
			if (chance == 1000)
			{
				retcode = rVICT_IMMUNE;
				if (SPELL_FLAG(skill, SF_STOPONFAIL))
					return retcode;
				continue;
			}
			if (saves_poison_death(chance, victim))
			{
				if (SPELL_FLAG(skill, SF_STOPONFAIL))
					return retcode;
				continue;
			}
			victim->mental_state = URANGE(30, victim->mental_state + 2, 100);
			break;

		case AFF_BLIND:
			af.type = gsn_blindness;
			break;
		case AFF_INVISIBLE:
			af.type = gsn_invis;
			break;
		case AFF_SLEEP:
			af.type = gsn_sleep;
			chance = ris_save(victim, level, RIS_SLEEP);
			if (chance == 1000)
			{
				retcode = rVICT_IMMUNE;
				if (SPELL_FLAG(skill, SF_STOPONFAIL))
					return retcode;
				continue;
			}
			break;
		case AFF_CHARM:
			af.type = gsn_charm_person;
			chance = ris_save(victim, level, RIS_CHARM);
			if (chance == 1000)
			{
				retcode = rVICT_IMMUNE;
				if (SPELL_FLAG(skill, SF_STOPONFAIL))
					return retcode;
				continue;
			}
			break;
		case AFF_POSSESS:
			af.type = gsn_possess;
			break;
		case AFF_FORCEINVIS:
			af.type = gsn_forceinvis;
			break;
		}

		af.duration = dice_parse(ch, level, saf->duration);
		af.modifier = dice_parse(ch, level, saf->modifier);
		af.location = saf->location % REVERSE_APPLY;
		if (af.duration == 0)
		{
			switch (af.location)
			{
			case APPLY_HIT:
				if (ch != victim && victim->hit < victim->max_hit && af.modifier > 0)
				{
					send_to_char("Zgodnie z kodeksem Jedi u�ywasz mocy nios�c pomoc!" NL, ch);
					ch->alignment = ch->alignment + 20;
					ch->alignment = URANGE(-1000, ch->alignment, 1000);
					jedi_bonus(ch);
				}
				if (af.modifier > 0 && victim->hit >= victim->max_hit)
					return rSPELL_FAILED;
				victim->hit = URANGE(0, victim->hit + af.modifier, victim->max_hit);
				update_pos(victim);
				break;
			case APPLY_MANA:
				if (af.modifier > 0 && victim->mana >= victim->max_mana)
					return rSPELL_FAILED;
				if (ch != victim)
				{
					send_to_char("Zgodnie z kodeksem Jedi u�ywasz mocy nios�c pomoc!" NL, ch);
					ch->alignment = ch->alignment + 25;
					ch->alignment = URANGE(-1000, ch->alignment, 1000);
					jedi_bonus(ch);
				}
				victim->mana = URANGE(0, victim->mana + af.modifier, victim->max_mana);
				update_pos(victim);
				break;
			case APPLY_MOVE:
				if (af.modifier > 0 && victim->move >= victim->max_move)
					return rSPELL_FAILED;
				victim->move = URANGE(0, victim->move + af.modifier, victim->max_move);
				update_pos(victim);
				break;
			default:
				affect_modify(victim, &af, true);
				break;
			}
		}
		else if (SPELL_FLAG(skill, SF_ACCUMULATIVE))
			affect_join(victim, &af);
		else
			affect_to_char(victim, &af);
	}
	update_pos(victim);
	return retcode;
}

/*
 * Generic spell affect						-Thoric
 */
ch_ret spell_affect(int sn, int level, CHAR_DATA *ch, void *vo)
{
	SMAUG_AFF *saf;
	SKILLTYPE *skill = get_skilltype(sn);
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	bool groupsp;
	bool areasp;
	bool hitchar, hitroom, hitvict = false;
	ch_ret retcode;

	IF_BUG(!skill->affects, "sn: %d", sn)
		return rNONE;

	if (SPELL_FLAG(skill, SF_GROUPSPELL))
		groupsp = true;
	else
		groupsp = false;

	if (SPELL_FLAG(skill, SF_AREA))
		areasp = true;
	else
		areasp = false;
	if (!groupsp && !areasp)
	{
		/* Can't find a victim */
		if (!victim)
		{
			failed_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}

		if ((skill->type != SKILL_HERB && IS_SET(victim->immune, RIS_FORCE)) || is_immune(victim, SPELL_DAMAGE(skill)))
		{
			immune_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}

		/* Spell is already on this guy */
		if (is_affected(victim, sn) && !SPELL_FLAG(skill, SF_ACCUMULATIVE) && !SPELL_FLAG(skill, SF_RECASTABLE))
		{
			failed_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}

		if ((saf = skill->affects) && !saf->next && saf->location == APPLY_STRIPSN
				&& !is_affected(victim, dice_parse(ch, level, saf->modifier)))
		{
			failed_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}

		if (check_save(sn, level, ch, victim))
		{
			failed_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}
	}
	else
	{
		if (skill->hit_char && skill->hit_char[0] != '\0')
		{
			if (strstr(skill->hit_char, "$N"))
				hitchar = true;
			else
				act( COL_FORCE, skill->hit_char, ch, NULL, NULL, TO_CHAR);
		}
		if (skill->hit_room && skill->hit_room[0] != '\0')
		{
			if (strstr(skill->hit_room, "$N"))
				hitroom = true;
			else
				act( COL_FORCE, skill->hit_room, ch, NULL, NULL, TO_ROOM);
		}
		if (skill->hit_vict && skill->hit_vict[0] != '\0')
			hitvict = true;
		/* group/area spell: iterate all room people */
	}

	IF_BUG(victim == NULL && ch->in_room->people.empty(), "sn: %d", sn)
	{
		failed_casting(skill, ch, victim, NULL);
		return rSPELL_FAILED;
	}

	if (victim)
	{
		/* Single-target path */
		retcode = spell_affectchar(sn, level, ch, victim);
		if (retcode == rSPELL_FAILED)
		{
			failed_casting(skill, ch, victim, NULL);
			return rSPELL_FAILED;
		}
		if (retcode == rVICT_IMMUNE)
			immune_casting(skill, ch, victim, NULL);
		else
			successful_casting(skill, ch, victim, NULL);
	}
	else
	{
		/* Group/area spell: iterate all people in the room */
		for (auto* vch : ch->in_room->people)
		{
			if (groupsp || areasp)
			{
				if ((groupsp && !is_same_group(vch, ch)) || IS_SET(vch->immune, RIS_FORCE) || is_immune(vch, SPELL_DAMAGE(skill))
						|| check_save(sn, level, ch, vch) || (!SPELL_FLAG(skill, SF_RECASTABLE) && is_affected(vch, sn)))
					continue;

				if (hitvict && ch != vch)
				{
					act( COL_FORCE, skill->hit_vict, ch, NULL, vch, TO_VICT);
					if (hitroom)
					{
						act( COL_FORCE, skill->hit_room, ch, NULL, vch, TO_NOTVICT);
						act( COL_FORCE, skill->hit_room, ch, NULL, vch, TO_CHAR);
					}
				}
				else if (hitroom)
					act( COL_FORCE, skill->hit_room, ch, NULL, vch, TO_ROOM);
				if (ch == vch)
				{
					if (hitvict)
						act( COL_FORCE, skill->hit_vict, ch, NULL, ch, TO_CHAR);
					else if (hitchar)
						act( COL_FORCE, skill->hit_char, ch, NULL, ch, TO_CHAR);
				}
				else if (hitchar)
					act( COL_FORCE, skill->hit_char, ch, NULL, vch, TO_CHAR);
			}
			retcode = spell_affectchar(sn, level, ch, vch);
		}
	}
	return rNONE;
}

/*
 * Generic inventory object spell				-Thoric
 */
ch_ret spell_obj_inv(int sn, int level, CHAR_DATA *ch, void *vo)
{
	OBJ_DATA *obj = (OBJ_DATA*) vo;
	SKILLTYPE *skill = get_skilltype(sn);
	int weather_mod;

	if (!obj)
	{
		failed_casting(skill, ch, NULL, NULL);
		return rNONE;
	}

	switch (SPELL_ACTION(skill))
	{
	default:
	case SA_NONE:
		return rNONE;

	case SA_CREATE:
		if (SPELL_FLAG(skill, SF_WATER)) /* create water */
		{
			int water;

			if (obj->item_type != ITEM_DRINK_CON)
			{
				send_to_char("It is unable to hold water." NL, ch);
				return rSPELL_FAILED;
			}

			if (obj->value[2] != LIQ_WATER && obj->value[1] != 0)
			{
				send_to_char("It contains some other liquid." NL, ch);
				return rSPELL_FAILED;
			}

			if (ch->in_room && ch->in_room->area && ch->in_room->area->planet && ch->in_room->area->planet->sky >= SKY_RAINING)
				weather_mod = 2;
			else
				weather_mod = 1;

			water = UMIN((skill->dice ? dice_parse(ch, level, skill->dice) : level) * weather_mod, obj->value[0] - obj->value[1]);

			if (water > 0)
			{
				separate_obj(obj);
				obj->value[2] = LIQ_WATER;
				obj->value[1] += water;
				if (!is_name("water", obj->name))
				{
					char buf[MAX_STRING_LENGTH];

					sprintf(buf, "%s water", obj->name);
					STRDUP(obj->name, buf);
				}
			}
			successful_casting(skill, ch, NULL, obj);
			return rNONE;
		}
		if ( SPELL_DAMAGE(skill) == SD_FIRE) /* burn object */
		{
			/* return rNONE; */
		}
		if ( SPELL_DAMAGE(skill) == SD_POISON /* poison object */
		|| SPELL_CLASS(skill) == SC_DEATH)
		{
			switch (obj->item_type)
			{
			default:
				failed_casting(skill, ch, NULL, obj);
				break;
			case ITEM_FOOD:
			case ITEM_DRINK_CON:
				separate_obj(obj);
				obj->value[3] = 1;
				successful_casting(skill, ch, NULL, obj);
				break;
			}
			return rNONE;
		}
		if ( SPELL_CLASS(skill) == SC_LIFE /* purify food/water */
		&& (obj->item_type == ITEM_FOOD || obj->item_type == ITEM_DRINK_CON))
		{
			switch (obj->item_type)
			{
			default:
				failed_casting(skill, ch, NULL, obj);
				break;
			case ITEM_FOOD:
			case ITEM_DRINK_CON:
				separate_obj(obj);
				obj->value[3] = 0;
				successful_casting(skill, ch, NULL, obj);
				break;
			}
			return rNONE;
		}

		if ( SPELL_CLASS(skill) != SC_NONE)
		{
			failed_casting(skill, ch, NULL, obj);
			return rNONE;
		}
		switch (SPELL_POWER(skill))
		/* clone object */
		{
		OBJ_DATA *clone;

	default:
	case SP_NONE:
		if (obj->cost > ch->skill_level[FORCE_ABILITY] * get_curr_int(ch) * get_curr_wis(ch))
		{
			failed_casting(skill, ch, NULL, obj);
			return rNONE;
		}
		break;
	case SP_MINOR:
		if (ch->skill_level[FORCE_ABILITY] - obj->level < 20 || obj->cost > ch->skill_level[FORCE_ABILITY] * get_curr_int(ch) / 5)
		{
			failed_casting(skill, ch, NULL, obj);
			return rNONE;
		}
		break;
	case SP_GREATER:
		if (ch->skill_level[FORCE_ABILITY] - obj->level < 5
				|| obj->cost > ch->skill_level[FORCE_ABILITY] * 10 * get_curr_int(ch) * get_curr_wis(ch))
		{
			failed_casting(skill, ch, NULL, obj);
			return rNONE;
		}
		break;
	case SP_MAJOR:
		if (ch->skill_level[FORCE_ABILITY] - obj->level < 0
				|| obj->cost > ch->skill_level[FORCE_ABILITY] * 50 * get_curr_int(ch) * get_curr_wis(ch))
		{
			failed_casting(skill, ch, NULL, obj);
			return rNONE;
		}
		break;
		clone = clone_object(obj);
		clone->timer = skill->dice ? dice_parse(ch, level, skill->dice) : 0;
		obj_to_char(clone, ch);
		successful_casting(skill, ch, NULL, obj);
		}
		return rNONE;

	case SA_DESTROY:
	case SA_RESIST:
	case SA_SUSCEPT:
	case SA_DIVINATE:
		if ( SPELL_DAMAGE(skill) == SD_POISON) /* detect poison */
		{
			if (obj->item_type == ITEM_DRINK_CON || obj->item_type == ITEM_FOOD)
			{
				if (obj->value[3] != 0)
					send_to_char("You smell poisonous fumes." NL, ch);
				else
					send_to_char("It looks very delicious." NL, ch);
			}
			else
				send_to_char("It doesn't look poisoned." NL, ch);
			return rNONE;
		}
		return rNONE;
	case SA_OBSCURE: /* make obj invis */
		if ( IS_OBJ_STAT(obj, ITEM_INVIS) || chance(ch, skill->dice ? dice_parse(ch, level, skill->dice) : 20))
		{
			failed_casting(skill, ch, NULL, NULL);
			return rSPELL_FAILED;
		}
		successful_casting(skill, ch, NULL, obj);
		SET_BIT(obj->extra_flags, ITEM_INVIS);
		return rNONE;

	case SA_CHANGE:
		return rNONE;
	}
	return rNONE;
}

/*
 * Generic object creating spell				-Thoric
 */
ch_ret spell_create_obj(int sn, int level, CHAR_DATA *ch, void *vo)
{
	SKILLTYPE *skill = get_skilltype(sn);
	int lvl;
	int vnum = skill->value;
	OBJ_DATA *obj;
	OBJ_INDEX_DATA *oi;

	switch (SPELL_POWER(skill))
	{
	default:
	case SP_NONE:
		lvl = 10;
		break;
	case SP_MINOR:
		lvl = 0;
		break;
	case SP_GREATER:
		lvl = level / 2;
		break;
	case SP_MAJOR:
		lvl = level;
		break;
	}

	/*
	 * Add predetermined objects here
	 */
	if (vnum == 0)
	{
		if (!str_cmp(target_name, "sword"))
			vnum = OBJ_VNUM_SCHOOL_SWORD;
		if (!str_cmp(target_name, "shield"))
			vnum = OBJ_VNUM_SCHOOL_SHIELD;
	}

	if ((oi = get_obj_index(vnum)) == NULL || (obj = create_object(oi, lvl)) == NULL)
	{
		failed_casting(skill, ch, NULL, NULL);
		return rNONE;
	}
	obj->timer = skill->dice ? dice_parse(ch, level, skill->dice) : 0;
	successful_casting(skill, ch, NULL, obj);
	if (CAN_WEAR(obj, ITEM_TAKE))
		obj_to_char(obj, ch);
	else
		obj_to_room(obj, ch->in_room);
	return rNONE;
}

/*
 * Generic mob creating spell					-Thoric
 */
ch_ret spell_create_mob(int sn, int level, CHAR_DATA *ch, void *vo)
{
	SKILLTYPE *skill = get_skilltype(sn);
	int lvl;
	int vnum = skill->value;
	CHAR_DATA *mob;
	MOB_INDEX_DATA *mi;
	AFFECT_DATA af;

	/* set maximum mob level */
	switch (SPELL_POWER(skill))
	{
	default:
	case SP_NONE:
		lvl = 20;
		break;
	case SP_MINOR:
		lvl = 5;
		break;
	case SP_GREATER:
		lvl = level / 2;
		break;
	case SP_MAJOR:
		lvl = level;
		break;
	}

	/*
	 * Add predetermined mobiles here
	 */
	if (vnum == 0)
	{
		return rNONE;
	}

	if ((mi = get_mob_index(vnum)) == NULL || (mob = create_mobile(mi)) == NULL)
	{
		failed_casting(skill, ch, NULL, NULL);
		return rNONE;
	}
	mob->top_level = UMIN(lvl, skill->dice ? dice_parse(ch, level, skill->dice) : mob->top_level);
	mob->armor = interpolate(mob->top_level, 100, -100);

	mob->max_hit = mob->top_level * 8 + number_range(mob->top_level * mob->top_level / 4, mob->top_level * mob->top_level);
	mob->hit = mob->max_hit;
	mob->gold = 0;
	successful_casting(skill, ch, mob, NULL);
	char_to_room(mob, ch->in_room);
	add_follower(mob, ch);
	af.type = sn;
	af.duration = (number_fuzzy((level + 1) / 3) + 1) * DUR_CONV;
	af.location = 0;
	af.modifier = 0;
	af.bitvector = AFF_CHARM;
	affect_to_char(mob, &af);
	return rNONE;
}

/*
 * Generic handler for new "SMAUG" spells			-Thoric
 */
ch_ret spell_smaug(int sn, int level, CHAR_DATA *ch, void *vo)
{
	SKILLTYPE *skill = get_skilltype(sn);

	switch (skill->target)
	{
	case TAR_IGNORE:

		/* offensive area spell */
		if ( SPELL_FLAG(skill, SF_AREA)
				&& ((SPELL_ACTION(skill) == SA_DESTROY && SPELL_CLASS(skill) == SC_LIFE)
						|| (SPELL_ACTION(skill) == SA_CREATE && SPELL_CLASS(skill) == SC_DEATH)))
			return spell_area_attack(sn, level, ch, vo);

		if ( SPELL_ACTION(skill) == SA_CREATE)
		{
			if (SPELL_FLAG(skill, SF_OBJECT)) /* create object */
				return spell_create_obj(sn, level, ch, vo);
			if ( SPELL_CLASS(skill) == SC_LIFE) /* create mob */
				return spell_create_mob(sn, level, ch, vo);
		}

		/* affect a distant player */
		if ( SPELL_FLAG(skill, SF_DISTANT) && SPELL_FLAG(skill, SF_CHARACTER))
			return spell_affect(sn, level, ch, get_char_world(ch, target_name));

		/* affect a player in this room (should have been TAR_CHAR_XXX) */
		if (SPELL_FLAG(skill, SF_CHARACTER))
			return spell_affect(sn, level, ch, get_char_room(ch, target_name));

		/* will fail, or be an area/group affect */
		return spell_affect(sn, level, ch, vo);

	case TAR_CHAR_OFFENSIVE:
		/* a regular damage inflicting spell attack */
		if ((SPELL_ACTION(skill) == SA_DESTROY && SPELL_CLASS(skill) == SC_LIFE)
				|| (SPELL_ACTION(skill) == SA_CREATE && SPELL_CLASS(skill) == SC_DEATH))
			return spell_attack(sn, level, ch, vo);

		/* a nasty spell affect */
		return spell_affect(sn, level, ch, vo);

	case TAR_CHAR_DEFENSIVE:
	case TAR_CHAR_SELF:
		if (vo && SPELL_ACTION(skill) == SA_DESTROY)
		{
			CHAR_DATA *victim = (CHAR_DATA*) vo;

			/* cure poison */
			if ( SPELL_DAMAGE(skill) == SD_POISON)
			{
				if (is_affected(victim, gsn_poison))
				{
					affect_strip(victim, gsn_poison);
					victim->mental_state = URANGE(-100, victim->mental_state, -10);
					successful_casting(skill, ch, victim, NULL);
					return rNONE;
				}
				failed_casting(skill, ch, victim, NULL);
				return rSPELL_FAILED;
			}
			/* cure blindness */
			if ( SPELL_CLASS(skill) == SC_ILLUSION)
			{
				if (is_affected(victim, gsn_blindness))
				{
					affect_strip(victim, gsn_blindness);
					successful_casting(skill, ch, victim, NULL);
					return rNONE;
				}
				failed_casting(skill, ch, victim, NULL);
				return rSPELL_FAILED;
			}
		}
		return spell_affect(sn, level, ch, vo);

	case TAR_OBJ_INV:
		return spell_obj_inv(sn, level, ch, vo);
	}
	return rNONE;
}

/* Haus' new, new mage spells follow */

/*
 *  4 Energy Spells
 */
ch_ret spell_ethereal_fist(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;

	level = UMAX(0, level);
	level = UMIN(35, level);
	dam = level * number_range(1, 6) - 31;
	dam = UMAX(0, dam);

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	act( COL_FORCE, "A fist of black, otherworldly ether rams into $N, leaving $M looking stunned!", ch, NULL, victim, TO_NOTVICT);
	return damage(ch, victim, dam, sn);
}

ch_ret spell_hand_of_darkness(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;

	level = UMAX(0, level);
	if (!IS_NPC(ch))
	{
		dam = (int) ((level / 10) * ((victim->alignment + 1000) / 20) * (((int) ch->pcdata->learned[sn] == 100) ? 2 : 1));
		if ((int) ch->pcdata->learned[sn] < 50)
			dam /= 3;
		else if ((int) ch->pcdata->learned[sn] < 75)
			dam /= 2;
	}
	else
		dam = (int) ((level / 8) * ((victim->alignment + 1000) / 20));

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	act( COL_FORCE, "$N is grasped by an incomprehensible hand of darkness!", ch, NULL, victim, TO_NOTVICT);
	return damage(ch, victim, dam, sn);
}

/* Mentalstate spells */

/*
 * 3 Mentalstate spells
 */
ch_ret spell_mind_wrack(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;

	/* decrement mentalstate by up to 50 */

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	level = UMAX(0, level);
	dam = number_range(0, 0);
	act( COL_FORCE, "$n stares intently at $N, causing $N to seem very lethargic.", ch, NULL, victim, TO_NOTVICT);
	return damage(ch, victim, dam, sn);
}

ch_ret spell_mind_wrench(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;

	/* increment mentalstate by up to 50 */

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);

	level = UMAX(0, level);
	dam = number_range(0, 0);
	act( COL_FORCE, "$n stares intently at $N, causing $N to seem very hyperactive.", ch, NULL, victim, TO_NOTVICT);
	return damage(ch, victim, dam, sn);
}

/* Non-offensive spell! */
ch_ret spell_revive(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;

	/* set mentalstate to mentalstate/2 */
	level = UMAX(0, level);
	dam = number_range(0, 0);
	act( COL_FORCE, "$n concentrates intently, and begins looking more centered.", ch, NULL, victim, TO_NOTVICT);
	return damage(ch, victim, dam, sn);
}

ch_ret spell_black_lightning(int sn, int level, CHAR_DATA *ch, void *vo)
{
	CHAR_DATA *victim = (CHAR_DATA*) vo;
	int dam;

	dam = (victim->alignment + 1000) / 5 + number_range(0, 20);

	hatred(ch);
	ch->alignment -= 100;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	sith_penalty(ch);
	act( FB_BLUE, "Wi�zki elektryczno�ci strzelaj� z palc�w $n$1, posy�aj�c $N$3 na ziemi� w bolesnych spazmach.", ch, NULL, victim,
			TO_NOTVICT);
	act( FB_BLUE, "Wi�zki elektryczno�ci strzelaj� z Twoich palc�w, posy�aj�c $N$3 na ziemi� w bolesnych spazmach.", ch, NULL, victim,
			TO_CHAR);
	act( FB_BLUE, "Pot�ny b� przechodzi przez Twoje cia�o podczas, gdy wi�zki elektryczno�ci wydobywaj�ce si� z" NL
	"palc�w $N$1 anihiluj� Cie.", victim, NULL, ch, TO_CHAR);

	if (saves_poison_death(level, victim))
		return damage(ch, victim, dam, sn);
	else
	{
		damage(ch, victim, dam, sn);
		if (char_died(victim) || spell_black_lightning(sn, level, ch, vo) == rCHAR_DIED)
			return rCHAR_DIED;
		return rNONE;
	}
}

/*! Trog: sprawdzamy czy nastapilo zdarzenie u gracza
 * i jesli to bylo ostatnie, dajemu graczowi moc */
void fevent_trigger(CHAR_DATA *ch, int trigger_arg, ...)
{
	fe_trigger trigger = static_cast<fe_trigger>(trigger_arg);
	bool found = false;
	va_list args;

	if ( IS_NPC( ch ) || !ch->pcdata || ch->pcdata->fevents_list.empty())
		return;

	auto fevents_snapshot = ch->pcdata->fevents_list;
	for (auto* fevent : fevents_snapshot)
	{
		found = false;

		if (fevent->trigger != trigger)
			continue;

		va_start(args, trigger_arg);

		switch (fevent->trigger)
		{
		case FE_FIND_ROOM:
			if (ch->in_room->vnum == fevent->attr[0])
				found = true;
			break;

		case FE_FIND_OBJECT:
		{
			for (auto* obj : ch->in_room->contents)
				if (obj->pIndexData->vnum == fevent->attr[0])
				{
					found = true;
					break;
				}
		}
			break;

		case FE_FIND_MOB:
		{
			for (auto* mob : ch->in_room->people)
				if ( IS_NPC( mob ) && mob->pIndexData->vnum == fevent->attr[0])
				{
					found = true;
					break;
				}
		}
			break;

		case FE_FIND_PLAYER:
		{
			for (auto* player : ch->in_room->people)
				if (!IS_NPC(player) && ((*fevent->sattr && !str_cmp(player->name, fevent->sattr)) || !*fevent->sattr))
				{
					found = true;
					break;
				}
		}
			break;

		case FE_FIND_SHIP:
		{
			for (auto* ship : ch->in_room->ships)
				if (((*fevent->sattr && !str_cmp(ship->name, fevent->sattr)) || !*fevent->sattr))
				{
					found = true;
					break;
				}
		}
			break;

		case FE_KILL_MOB:
		{
			int vnum = va_arg( args, int );

			if ((fevent->attr[0] && vnum == fevent->attr[0]) || !fevent->attr[0])
				found = true;
		}
			break;

		case FE_KILL_PLAYER:
		{
			char *playerName = va_arg( args, char * );

			if (!playerName)
			{
				bug("fevent_trigger: KILL_PLAYER: no playerName");
				break;
			}

			if ((*fevent->sattr && !str_cmp(playerName, fevent->sattr)) || !*fevent->sattr)
				found = true;
		}
			break;

		case FE_KILL_SHIP:
		{
			char *shipName = va_arg( args, char * );

			if (!shipName)
			{
				bug("fevent_trigger: KILL_SHIP: no shipName");
				break;
			}

			if ((*fevent->sattr && !str_cmp(shipName, fevent->sattr)) || !*fevent->sattr)
				found = true;
		}
			break;

		case FE_GAIN_STAT:
		{
			int amt;

			switch (fevent->attr[2])
			{
			case REQ_STR:
				amt = ch->perm_str;
				break;

			case REQ_DEX:
				amt = ch->perm_dex;
				break;

			case REQ_INT:
				amt = ch->perm_int;
				break;

			case REQ_WIS:
				amt = ch->perm_wis;
				break;

			case REQ_CON:
				amt = ch->perm_con;
				break;

			case REQ_CHA:
				amt = ch->perm_cha;
				break;

			case REQ_LCK:
				amt = ch->perm_lck;
				break;

			default:
				bug("fevent_trigger: GAIN_STAT: no such stat");
				amt = -1;
				break;
			}

			/* != 0 => wymagane minimum attr[0], else wymagane dokladnie attr[0] */
			if ((fevent->attr[1] && amt >= fevent->attr[0]) || (!fevent->attr[1] && amt == fevent->attr[0]))
				found = true;
		}
			break;

		case FE_START_QUEST:
		case FE_FAIL_QUEST:
		case FE_FINISH_QUEST:
		{
			QUEST_INDEX_DATA *quest = va_arg( args, QUEST_INDEX_DATA * );

			if (!quest)
			{
				bug("fevent_trigger: *_QUEST: no quest");
				break;
			}

			if ((fevent->attr[0] && quest->quest_id == fevent->attr[0]) || !fevent->attr[0])
				found = true;
		}
			break;

		case FE_ENTER_SHIP:
			if ((*fevent->sattr && !str_cmp(ch->in_room->ship->name, fevent->sattr)) || !*fevent->sattr)
				found = true;
			break;

		case FE_LAUNCH_SHIP:
		case FE_LAND_SHIP:
		{
			SHIP_DATA *ship = ship_from_cockpit(ch->in_room);

			if (!ship)
			{
				bug("fevent_trigger: *_SHIP: ch not in ship");
				break;
			}

			if ((*fevent->sattr && !str_cmp(ship->name, fevent->sattr)) || !*fevent->sattr)
				found = true;
		}
			break;

		case FE_LEVEL:
			if ((fevent->attr[1] && ch->top_level >= fevent->attr[0]) || (!fevent->attr[1] && ch->top_level == fevent->attr[0]))
				found = true;
			break;

		case FE_SKLVL:
			/* attr[0] - ile, 1 - minimum tyle, 2 - ktora profesja */
			if ((fevent->attr[1] && ch->skill_level[fevent->attr[2]] >= fevent->attr[0])
					|| (!fevent->attr[1] && ch->skill_level[fevent->attr[2]] == fevent->attr[0]))
				found = true;
			break;

		case FE_POSITION:
			if (ch->position == fevent->attr[0])
				found = true;
			break;

		case FE_MKILLS:
			if ((fevent->attr[1] && ch->pcdata->mkills >= fevent->attr[0]) || (!fevent->attr[1] && ch->pcdata->mkills == fevent->attr[0]))
				found = true;
			break;

		case FE_PKILLS:
			if ((fevent->attr[1] && ch->pcdata->pkills >= fevent->attr[0]) || (!fevent->attr[1] && ch->pcdata->pkills == fevent->attr[0]))
				found = true;
			break;

		case FE_SKILL:
		{
			int sn;

			if (!*fevent->sattr)
			{
				bug("fevent_trigger: SKILL: no skill");
				break;
			}

			if ((sn = skill_lookup(fevent->sattr)) == NO_FLAG)
			{
				bug("fevent_trigger: SKILL: no such skill: %s", fevent->sattr);
				break;
			}

			if ((fevent->attr[1] && ch->pcdata->learned[sn] >= fevent->attr[0])
					|| (!fevent->attr[1] && ch->pcdata->learned[sn] == fevent->attr[0]))
				found = true;
		}
			break;

		default:
			bug("fevent_trigger: player: %s - no such force event: %d", ch->name, fevent->trigger);
			break;
		}

		if (found)
		{
			SWString buf(ch->name);
			buf << " completed force event: " << bit_name(fevent_types_list, fevent->trigger);
			log_string(buf.c_str());

			ch->pcdata->fevents_list.remove(fevent);
			free_fevent(fevent);
		}

		va_end(args);
	}

	/* jesli tak, to gracz dostaje moc */
	if (ch->pcdata->fevents_list.empty())
	{
		char buf[MSL];

		ch->perm_frc = URANGE(1, ch->pcdata->fevents + ch->race->frc_plus, 20);
		ch->max_mana = (get_curr_int(ch) * 10) + get_curr_wis(ch);

		send_to_char( FB_WHITE "Czujesz si�... inaczej, tak jakby to �wiat zale�a� od Ciebie." EOL, ch);
		sprintf(buf, "giving %d (+%d) frc to player %s due to force events completed", ch->pcdata->fevents, ch->race->frc_plus, ch->name);
		log_string(buf);
		ch->pcdata->fevents = 0;
	}
}

/*! losujemy Force Events, ile, jakie, ... */
void drawlots_fevents(CHAR_DATA *ch)
{
	FEVENT_DATA *fevent;
	int e;
	bool link = true;

	/* jak widac moc dajemy BARDZO rzadko */
//	if( number_range( 0, 100 ) > 5
//	&& number_range( 0, 100 ) > 5 )
	/* chwilowo, bardzo czesto :-) */
	if (number_range(0, 100) > 55)
		return;

	ch->pcdata->fevents = number_range(1, number_range(0, 100) < 90 ? number_range(1, 5) : number_range(1, 100));

	for (e = 0; e < ch->pcdata->fevents; e++, link = true)
	{
		fevent = new_fevent();
		fevent->trigger = (fe_trigger) number_range(0, FE_TRIGGER_MAX - 1);

		switch (fevent->trigger)
		{
		case FE_FIND_ROOM:
		{
			RID *room = RandomIndices::getRoomIndex();

			if (!room)
			{
				e--;
				free_fevent(fevent);
				link = false;
				break;
			}

			fevent->attr[0] = room->vnum;
		}
			break;

		case FE_FIND_OBJECT:
		{
			OID *obj = RandomIndices::getObjIndex();

			if (!obj)
			{
				e--;
				free_fevent(fevent);
				link = false;
				break;
			}

			fevent->attr[0] = obj->vnum;
		}
			break;

		case FE_KILL_MOB:
		case FE_FIND_MOB:
		{
			MID *mob = RandomIndices::getMobIndex();

			if (!mob)
			{
				e--;
				free_fevent(fevent);
				link = false;
				break;
			}

			fevent->attr[0] = mob->vnum;
		}
			break;

		case FE_KILL_PLAYER:
		case FE_FIND_PLAYER:
		{
			DIR *dir = opendir( PDATA_DIR);
			struct dirent *entry;
			int i = 0;
			int num;

			if (!dir)
			{
				e--;
				free_fevent(fevent);
				link = false;
				break;
			}

			while ((entry = readdir(dir)))
				if (entry->d_name[0] != '.')
					i++;

			num = number_range(0, i - 1);
			rewinddir(dir);

			while ((entry = readdir(dir)))
			{
				if (entry->d_name[0] == '.')
					continue;

				if (i++ == num)
				{
					STRDUP(fevent->sattr, entry->d_name);
					break;
				}
			}

			if (!*fevent->sattr)
			{
				e--;
				free_fevent(fevent);
				link = false;
			}

			closedir(dir);
		}
			break;

		case FE_GAIN_STAT:
			fevent->attr[2] = number_range( REQ_STR, REQ_LCK);
			fevent->attr[1] = number_range(0, 1);
			fevent->attr[0] = number_range(18, 30);
			break;

		case FE_START_QUEST:
		case FE_FAIL_QUEST:
		case FE_FINISH_QUEST:
		{
			int i = 0;
			int num;

			num = number_range(0, (int)quest_index_list.size() - 1);

			for (auto* quest : quest_index_list)
				if (i++ == num)
				{
					fevent->attr[0] = quest->quest_id;
					break;
				}
		}
			break;

		case FE_FIND_SHIP:
		case FE_KILL_SHIP:
		case FE_ENTER_SHIP:
		case FE_LAUNCH_SHIP:
		case FE_LAND_SHIP:
		{
			int i = 0;
			int num;

			num = number_range(0, (int)ship_list.size() - 1);

			for (auto* ship : ship_list)
				if (i++ == num)
				{
					STRDUP(fevent->sattr, ship->name);
					break;
				}
		}
			break;

		case FE_LEVEL:
			fevent->attr[0] = number_range(2, 100);
			fevent->attr[1] = number_range(0, 1);
			break;

		case FE_SKLVL:
			fevent->attr[0] = number_range(2, 100);
			fevent->attr[1] = number_range(0, 1);
			fevent->attr[2] = number_range(0, MAX_ABILITY - 1);
			break;

		case FE_POSITION:
			fevent->attr[0] = number_range(0, POS_MAX - 1);
			break;

		case FE_MKILLS:
			fevent->attr[0] = number_range(1000, 20000);
			fevent->attr[1] = number_range(0, 1);
			break;

		case FE_PKILLS:
			fevent->attr[0] = number_range(50, 100);
			fevent->attr[1] = number_range(0, 1);
			break;

		case FE_SKILL:
		{
			int no = number_range(1, MAX_SKILL - 1);

			if (!skill_table[no] || !skill_table[no]->name || !*skill_table[no]->name)
			{
				e--;
				free_fevent(fevent);
				link = false;
				bug("fevent_trigger: player: %s - no such skill sn: %d", ch->name, no);
			}
			else
			{
				STRDUP(fevent->sattr, skill_table[no]->name);
				fevent->attr[1] = number_range(0, 1);
				fevent->attr[0] = number_range(76, 100);
			}
		}
			break;

		default:
			bug("fevent_trigger: player: %s - no such force event: %d", ch->name, fevent->trigger);
			break;
		}

		if (link)
			ch->pcdata->fevents_list.push_back(fevent);
	}
}

