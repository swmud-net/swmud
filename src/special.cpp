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
 *			   "Special procedure" module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <time.h>
#include <map>
#include <classes/SWString.h>
#include "mud.h"

using std::map;

bool remove_obj args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
void jail_char args( ( CHAR_DATA *victim, CHAR_DATA *policeman,
				CRIME_DATA *crime ) );

/*
 * The following special functions are available for mobiles.
 */
DECLARE_SPEC_FUN(spec_jedi);
DECLARE_SPEC_FUN(spec_dark_jedi);
DECLARE_SPEC_FUN(spec_fido);
DECLARE_SPEC_FUN(spec_guardian);
DECLARE_SPEC_FUN(spec_janitor);
DECLARE_SPEC_FUN(spec_poison);
DECLARE_SPEC_FUN(spec_thief);
DECLARE_SPEC_FUN(spec_auth);
DECLARE_SPEC_FUN(spec_stormtrooper);
DECLARE_SPEC_FUN(spec_new_republic_trooper);
DECLARE_SPEC_FUN(spec_customs_smut);
DECLARE_SPEC_FUN(spec_customs_alcohol);
DECLARE_SPEC_FUN(spec_customs_weapons);
DECLARE_SPEC_FUN(spec_customs_spice);
DECLARE_SPEC_FUN(spec_police_attack);
DECLARE_SPEC_FUN(spec_police_jail);
DECLARE_SPEC_FUN(spec_police_fine);
DECLARE_SPEC_FUN(spec_police);
DECLARE_SPEC_FUN(spec_clan_guard);
DECLARE_SPEC_FUN(spec_newbie_pilot);
DECLARE_SPEC_FUN(spec_prosecutor); /* Thanos */

struct spec_type
{
	const char *name;
	SPEC_FUN *spec;
};

const struct spec_type spec_table[] =
{
{ "spec_jedi", spec_jedi },
{ "spec_dark_jedi", spec_dark_jedi },
{ "spec_fido", spec_fido },
{ "spec_guardian", spec_guardian },
{ "spec_janitor", spec_janitor },
{ "spec_poison", spec_poison },
{ "spec_thief", spec_thief },
{ "spec_auth", spec_auth },
{ "spec_stormtrooper", spec_stormtrooper },
{ "spec_new_republic_trooper", spec_new_republic_trooper },
{ "spec_customs_smut", spec_customs_smut },
{ "spec_customs_alcohol", spec_customs_alcohol },
{ "spec_customs_weapons", spec_customs_weapons },
{ "spec_customs_spice", spec_customs_spice },
{ "spec_police_attack", spec_police_attack },
{ "spec_police_jail", spec_police_jail },
{ "spec_police_fine", spec_police_fine },
{ "spec_police", spec_police },
{ "spec_clan_guard", spec_clan_guard },
{ "spec_newbie_pilot", spec_newbie_pilot },
{ "spec_prosecutor", spec_prosecutor },

/* Dla zgodno�ci z Envy 	-- Thanos */
{ "spec_guard", spec_guardian },
{ "spec_cast_adept", spec_jedi },
{ "spec_mayor", spec_fido }, /* ;) */
{ "spec_executioner", spec_police_attack },

/* NIE DOTYKA� */
{ "", 0 }, };

/*
 * Given a name, return the appropriate spec fun.
 */
SPEC_FUN* spec_lookup(const char *name)
{
	int fn;

	for (fn = 0; *spec_table[fn].name; fn++)
	{
		if (!str_cmp(name, spec_table[fn].name))
			return spec_table[fn].spec;
	}

	return 0;
}

/*
 * Given a pointer, return the appropriate spec fun text.
 */
const char* lookup_spec(SPEC_FUN *spec)
{
	int fn;

	for (fn = 0; *spec_table[fn].name; fn++)
	{
		if (spec_table[fn].spec == spec)
			return spec_table[fn].name;
	}
	return "unknown";
}

bool spec_newbie_pilot(CHAR_DATA *ch)
{
	int home = 32149;
	char buf[MAX_STRING_LENGTH];
	bool diploma = false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{

		if ( IS_NPC(victim) || victim->position == POS_FIGHTING)
			continue;

		for (auto* obj : victim->carrying)
			if (obj->pIndexData->vnum == OBJ_VNUM_SCHOOL_DIPLOMA)
				diploma = true;

		if (!diploma)
			continue;

		if (IS_RACE(victim, "MON CALAMARI"))
		{
			home = 21100;
			strcpy(buf,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na Mon-Calamari.\n\r\n\r");
			echo_to_room(ch->in_room, buf);
		}
		else if (IS_RACE(victim, "QUARREN"))
		{
			home = 6904;
			strcpy(buf,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na Mon-Calamari.\n\r\n\r");
			echo_to_room(ch->in_room, buf);
		}
		else if (IS_RACE(victim, "GAMORREAN"))
		{
			home = 28038;
			strcpy(buf,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na Gamorr.\n\r\n\r");
			echo_to_room(ch->in_room, buf);
		}
		else if (IS_RACE(victim, "ADARIAN"))
		{
			home = 29006;
			strcpy(buf,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na Adari.\n\r\n\r");
			echo_to_room(ch->in_room, buf);
		}
		else if (IS_RACE(victim, "JAWA"))
		{
			home = 31821;
			strcpy(buf,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na Tatooine.\n\r\n\r");
			echo_to_room(ch->in_room, buf);
		}
		else if (IS_RACE(victim, "WOOKIEE"))
		{
			home = 28600;
			strcpy(buf,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na Kashyyyk.\n\r\n\r");
			echo_to_room(ch->in_room, buf);
		}
		else if (IS_RACE(victim, "HUMAN"))
		{
			home = 201;
			strcpy(buf,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na Coruscant do Portu Kosmicznego Menari.\n\r\n\r");
			echo_to_room(ch->in_room, buf);
		}
		else if (IS_RACE(victim, "NOGHRI"))
		{
			home = 1001;
			strcpy(buf,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na Honoghr do Portu Kosmicznego Nystao.\n\r\n\r");
			echo_to_room(ch->in_room, buf);
		}
		else
		{
			sprintf(buf, "Hmm, %s.",
					victim->sex == SEX_MALE ?
							victim->race->przypadki[0] :
							victim->race->inflectsFemale[0]);
			do_look(ch, victim->name);
			do_say(ch, buf);
			do_say(ch,
					(char*) "Troszk� ci�ko b�dzie dolecie� na twoj� ojczyst� planet�.");
			do_say(ch, (char*) "Zabior� ci� wi�c na Pluogusa.");
			echo_to_room(ch->in_room,
					COL_ACTION "Po d�ugim i wyczerpuj�cym locie docierasz na pok�ad statku 'Serin Pluogus'.\n\r\n\r");
		}

		char_from_room(victim);
		char_to_room(victim, get_room_index(home));

		do_look(victim, (char*) "");

		sprintf(buf,
				COL_ACTION "%s wysiada, a kapsu�a momentalnie startuje by wr�cic do akademi.\n\r",
				victim->name);
		echo_to_room(ch->in_room, buf);
	}

	return false;
}

bool spec_jedi(CHAR_DATA *ch)
{
	return false;
}

bool spec_clan_guard(CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch) || ch->fighting)
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{
		if (!can_see(ch, victim))
			continue;
		if (get_timer(victim, TIMER_RECENTFIGHT) > 0)
			continue;
		if (!IS_NPC(victim) && victim->pcdata && victim->pcdata->clan
				&& IS_AWAKE(victim)
				&& str_cmp(ch->mob_clan, victim->pcdata->clan->name))
		{
			do_yell(ch, (char*) "Hej Nie masz prawa by tu przebywa�!");
			multi_hit(ch, victim, TYPE_UNDEFINED);
			return true;
		}
	}

	return false;
}

bool spec_customs_smut(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	long ch_exp;

	if (!IS_AWAKE(ch) || ch->position == POS_FIGHTING)
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{

		if ( IS_NPC(victim) || victim->position == POS_FIGHTING)
			continue;

		for (auto* obj : victim->carrying)
		{
			if (obj->pIndexData->item_type == ITEM_SMUT)
			{
				if (victim != ch && can_see(ch, victim) && can_see_obj(ch, obj))
				{
					sprintf(buf,
							"Posiadanie %s jest u nas nielegalne. Konfiskuj� to w imieniu prawa.",
							obj->przypadki[1]);
					do_say(ch, buf);
					if (obj->wear_loc != WEAR_NONE)
						remove_obj(victim, obj->wear_loc, true);
					separate_obj(obj);
					obj_from_char(obj);
					act( COL_ACTION, "$n konfiskuje $N$2 $p$3.", ch, obj,
							victim, TO_NOTVICT);
					act( COL_ACTION, "$n zabiera ci $p$3.", ch, obj, victim,
							TO_VICT);
					obj = obj_to_char(obj, ch);
					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim, "Tracisz %ld punkt�w do�wiadczenia.\n\r ",
							ch_exp);
					gain_exp(victim, 0 - ch_exp, SMUGGLING_ABILITY);
					return true;
				}
				else if (can_see(ch,
						victim) && !IS_SET( obj->extra_flags , ITEM_CONTRABAND))
				{
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim,
							"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r",
							ch_exp, obj->przypadki[1]);
					gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

					act( COL_ACTION, "$n zerka na $N$3 podejrzliwie.", ch, NULL,
							victim, TO_NOTVICT);
					act( COL_ACTION, "$n zerka na ciebie podejrzliwie.", ch,
							NULL, victim, TO_VICT);
					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);

					return true;
				}
				else if (!IS_SET(obj->extra_flags, ITEM_CONTRABAND))
				{
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim,
							"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r",
							ch_exp, obj->przypadki[1]);
					gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
					return true;
				}
			}
			else if (obj->item_type == ITEM_CONTAINER)
			{
				for (auto* content : obj->contents)
				{
					if (content->pIndexData->item_type
							== ITEM_SMUT && !IS_SET( content->extra_flags , ITEM_CONTRABAND ))
					{
						ch_exp =
								UMIN(content->cost * 10,
										( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
						ch_printf(victim,
								"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r ",
								ch_exp, content->przypadki[1]);
						gain_exp(victim, ch_exp, SMUGGLING_ABILITY);
						SET_BIT(content->extra_flags, ITEM_CONTRABAND);
						return true;
					}
				}
			}
		}

	}

	return false;
}

bool spec_customs_weapons(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	long ch_exp;

	if (!IS_AWAKE(ch) || ch->position == POS_FIGHTING)
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{

		if ( IS_NPC(victim) || victim->position == POS_FIGHTING)
			continue;

		if (victim->pcdata && victim->pcdata->clan
				&& !str_cmp(victim->pcdata->clan->name, ch->mob_clan))
			continue;

		for (auto* obj : victim->carrying)
		{
			if (obj->pIndexData->item_type
					== ITEM_WEAPON && obj->value[3] != WEAPON_LIGHTSABER)
			{
				if (victim != ch && can_see(ch, victim) && can_see_obj(ch, obj))
				{
					sprintf(buf,
							"Bro� jest u nas niedozwolona. Konfiskuj� ten %s w imieniu prawa.",
							obj->przypadki[3]);
					do_say(ch, buf);
					if (obj->wear_loc != WEAR_NONE)
						remove_obj(victim, obj->wear_loc, true);
					separate_obj(obj);
					obj_from_char(obj);
					act( COL_ACTION, "$n konfiskuje $N$2 $p$3.", ch, obj,
							victim, TO_NOTVICT);
					act( COL_ACTION, "$n zabiera ci $p$3.", ch, obj, victim,
							TO_VICT);
					obj = obj_to_char(obj, ch);
					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim, "Tracisz %ld punkt�w do�wiadczenia.\n\r ",
							ch_exp);
					gain_exp(victim, 0 - ch_exp, SMUGGLING_ABILITY);
					return true;
				}
				else if (can_see(ch,
						victim) && !IS_SET( obj->extra_flags , ITEM_CONTRABAND))
				{
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim,
							"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s..\n\r",
							ch_exp, obj->przypadki[1]);
					gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

					act( COL_ACTION, "$n zerka na $N$2 podejrzliwie.", ch, NULL,
							victim, TO_NOTVICT);
					act( COL_ACTION, "$n zerka na ciebie podejrzliwie.", ch,
							NULL, victim, TO_VICT);
					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
					return true;
				}
				else if (!IS_SET(obj->extra_flags, ITEM_CONTRABAND))
				{
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim,
							"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r",
							ch_exp, obj->przypadki[1]);
					gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
					return true;
				}
			}
			else if (obj->item_type == ITEM_CONTAINER)
			{
				for (auto* content : obj->contents)
				{
					if (content->pIndexData->item_type
							== ITEM_WEAPON && !IS_SET( content->extra_flags , ITEM_CONTRABAND ))
					{
						ch_exp =
								UMIN(content->cost * 10,
										( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
						ch_printf(victim,
								"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r",
								ch_exp, content->przypadki[1]);
						gain_exp(victim, ch_exp, SMUGGLING_ABILITY);
						SET_BIT(content->extra_flags, ITEM_CONTRABAND);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool spec_customs_alcohol(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	int liquid;
	long ch_exp;

	if (!IS_AWAKE(ch) || ch->position == POS_FIGHTING)
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{

		if ( IS_NPC(victim) || victim->position == POS_FIGHTING)
			continue;

		for (auto* obj : victim->carrying)
		{
			if (obj->pIndexData->item_type == ITEM_DRINK_CON)
			{
				if ((liquid = obj->value[2]) >= LIQ_MAX)
					liquid = obj->value[2] = 0;

				if (liq_table[liquid].liq_affect[COND_DRUNK] > 0)
				{
					if (victim != ch && can_see(ch, victim)
							&& can_see_obj(ch, obj))
					{
						sprintf(buf,
								"Posiadanie %s jest u nas nielegalne. Konfiskuj� ci to w imieniu prawa",
								obj->przypadki[1]);
						do_say(ch, buf);
						if (obj->wear_loc != WEAR_NONE)
							remove_obj(victim, obj->wear_loc, true);
						separate_obj(obj);
						obj_from_char(obj);
						act( COL_ACTION, "$n konfiskuje $N$2 $p$3.", ch, obj,
								victim, TO_NOTVICT);
						act( COL_ACTION, "$n zabiera ci $p$3.", ch, obj, victim,
								TO_VICT);
						obj = obj_to_char(obj, ch);
						SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
						ch_exp =
								UMIN(obj->cost * 10,
										( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
						ch_printf(victim,
								"Tracisz %ld punkt�w do�wiadczenia. \n\r",
								ch_exp);
						gain_exp(victim, 0 - ch_exp, SMUGGLING_ABILITY);
						return true;
					}
					else if (can_see(ch,
							victim) && !IS_SET( obj->extra_flags , ITEM_CONTRABAND))
					{
						ch_exp =
								UMIN(obj->cost * 10,
										( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
						ch_printf(victim,
								"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r",
								ch_exp, obj->przypadki[1]);
						gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

						act( COL_ACTION, "$n zerka na $N$3 podejrzliwie.", ch,
								NULL, victim, TO_NOTVICT);
						act( COL_ACTION, "$n zerka na ciebie podejrzliwie.", ch,
								NULL, victim, TO_VICT);
						SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
						return true;
					}
					else if (!IS_SET(obj->extra_flags, ITEM_CONTRABAND))
					{
						ch_exp =
								UMIN(obj->cost * 10,
										( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
						ch_printf(victim,
								"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s. \n\r",
								ch_exp, obj->przypadki[1]);
						gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

						SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
						return true;
					}
				}
			}
			else if (obj->item_type == ITEM_CONTAINER)
			{
				for (auto* content : obj->contents)
				{
					if (content->pIndexData->item_type
							== ITEM_DRINK_CON && !IS_SET( content->extra_flags , ITEM_CONTRABAND ))
					{
						if ((liquid = obj->value[2]) >= LIQ_MAX)
							liquid = obj->value[2] = 0;
						if (liq_table[liquid].liq_affect[COND_DRUNK] <= 0)
							continue;
						ch_exp =
								UMIN(content->cost * 10,
										( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
						ch_printf(victim,
								"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r",
								ch_exp, content->przypadki[1]);
						gain_exp(victim, ch_exp, SMUGGLING_ABILITY);
						SET_BIT(content->extra_flags, ITEM_CONTRABAND);
						return true;
					}
				}
			}
		}

	}

	return false;
}

bool spec_customs_spice(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	long ch_exp;

	if (!IS_AWAKE(ch) || ch->position == POS_FIGHTING)
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{

		if ( IS_NPC(victim) || victim->position == POS_FIGHTING)
			continue;

		for (auto* obj : victim->carrying)
		{
			if (obj->pIndexData->item_type == ITEM_SPICE
					|| obj->pIndexData->item_type == ITEM_RAWSPICE)
			{
				if (victim != ch && can_see(ch, victim) && can_see_obj(ch, obj))
				{
					sprintf(buf,
							"Posiadanie %s jest u nas nielegalne. Konfiskuj� to w imieniu prawa.",
							obj->przypadki[1]);
					do_say(ch, buf);
					if (obj->wear_loc != WEAR_NONE)
						remove_obj(victim, obj->wear_loc, true);
					separate_obj(obj);
					obj_from_char(obj);
					act( COL_ACTION, "$n konfiskuje $N$2 $p$3.", ch, obj,
							victim, TO_NOTVICT);
					act( COL_ACTION, "$n zabiera ci $p$4.", ch, obj, victim,
							TO_VICT);
					obj = obj_to_char(obj, ch);
					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim, "Tracisz %ld punkt�w do�wiadczenia. \n\r",
							ch_exp);
					gain_exp(victim, 0 - ch_exp, SMUGGLING_ABILITY);
					return true;
				}
				else if (can_see(ch,
						victim) && !IS_SET( obj->extra_flags , ITEM_CONTRABAND))
				{
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim,
							"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r",
							ch_exp, obj->przypadki[1]);
					gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

					act( COL_ACTION, "$n zerka na $N$3 podejrzliwie.", ch, NULL,
							victim, TO_NOTVICT);
					act( COL_ACTION, "$n zerka na ciebie podejrzliwie.", ch,
							NULL, victim, TO_VICT);
					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
					return true;
				}
				else if (!IS_SET(obj->extra_flags, ITEM_CONTRABAND))
				{
					ch_exp =
							UMIN(obj->cost * 10,
									( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
					ch_printf(victim,
							"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r",
							ch_exp, obj->przypadki[1]);
					gain_exp(victim, ch_exp, SMUGGLING_ABILITY);

					SET_BIT(obj->extra_flags, ITEM_CONTRABAND);
					return true;
				}
			}
			else if (obj->item_type == ITEM_CONTAINER)
			{
				for (auto* content : obj->contents)
				{
					if (content->pIndexData->item_type
							== ITEM_SPICE && !IS_SET( content->extra_flags , ITEM_CONTRABAND ))
					{
						ch_exp =
								UMIN(content->cost * 10,
										( exp_level( victim->skill_level[SMUGGLING_ABILITY]+1) - exp_level( victim->skill_level[SMUGGLING_ABILITY]) ));
						ch_printf(victim,
								"Otrzymujesz %ld punkt�w do�wiadczenia za przemyt %s.\n\r ",
								ch_exp, content->przypadki[1]);
						gain_exp(victim, ch_exp, SMUGGLING_ABILITY);
						SET_BIT(content->extra_flags, ITEM_CONTRABAND);
						return true;
					}
				}
			}
		}

	}

	return false;
}

SUSPECT_DATA* get_suspect(CHAR_DATA *victim, CHAR_DATA *ch)
{
	for (auto* sus : ch->suspects)
		if (!str_cmp(sus->name, victim->name))
			return sus;

	return NULL;
}

void start_suspecting(CHAR_DATA *ch, CHAR_DATA *victim, int status,
		CRIME_DATA *crime)
{
	SUSPECT_DATA *sus;

	CREATE(sus, SUSPECT_DATA, 1);
	STRDUP(sus->name, victim->name);
	sus->age = 2 + number_range(0, 1);
	sus->status = status;
	sus->crime = crime;
	ch->suspects.push_back(sus);
	return;
}

void police_bribe_trigger(CHAR_DATA *mob, CHAR_DATA *ch, int amount)
{
	SUSPECT_DATA *sus;
	char buf[MSL];

	if (!IS_NPC(mob))
		return;

	if (IS_NPC(ch))
		return;

	if (!(sus = get_suspect(ch, mob)))
		return;

	/* czy mob w ogole jest przekupywalny? */
	if (ch->top_level * 2 + get_curr_cha(ch)
			< mob->top_level * 2 + get_curr_cha(mob))
	{
		ch_tell(mob, ch, "Co ??? Pr�bujesz mnie przekupi�?");
		ch_tell(mob, ch, "Ty MENDO !!!");
		multi_hit(mob, ch, TYPE_UNDEFINED);
		return; /* Kasy oczywi�cie nie oddajemy :P */
	}

	/* ale gdyby da� si� przekupi�, to oczywi�cie za odpowiedni� kwot� */
	if (amount < sus->age * ch->top_level)
	{
		int cash = sus->age * ch->top_level + sus->age * ch->top_level / 10;

		sprintf(buf, "Wiesz co? Daj mi %d kredyt%s i zapomnijmy o tym...", cash,
				NUMBER_SUFF(cash, "k�", "ki", "ek"));
		ch_tell(mob, ch, buf);

		sus->age = UMAX(sus->age, number_range(2, 4));

		ch->gold += amount;
		mob->gold -= amount;

		act( PLAIN, "$n daje $N$2 kilka kredytek.", mob, NULL, ch, TO_NOTVICT);
		act( PLAIN, "Dajesz $N$2 kilka kredytek.", mob, NULL, ch, TO_CHAR);
		act( PLAIN, "$N oddaje ci twoje pieni�dze.", ch, NULL, mob, TO_CHAR);
		return;
	}

	sprintf(buf, "No. Od razu wiedzia�%sm, �e si� dogadamy, %s.",
			SEX_SUFFIX_EAE(mob), ch->name);
	ch_tell(mob, ch, buf);
	check_social(mob, "oczko", ch->name);

	sus->status = SUS_FINED;
	return;
}

void end_hunting(CHAR_DATA *victim, CHAR_DATA *ch, CRIME_DATA *crime)
{
	jail_char(victim, ch, crime);

	/* alarm odwo�any */
	for (auto* hch : char_list)
	{
		SUSPECT_DATA *hsus;

		if (!IS_NPC(hch))
			continue;

		if (!hch->in_room || !hch->in_room->area
				|| (hch->in_room->area != victim->in_room->area))
			continue;

		if ((hsus = get_suspect(victim, hch)) != NULL)
			free_suspect(hch, hsus);
	}
}
/*
 Hmmm Troch� dziwne, �e ten spec_fun nie r�ni si� niczym od
 spec_police_jail				-- Thanos
 */
bool spec_police(CHAR_DATA *ch)
{
	CHAR_DATA *victim;
	SUSPECT_DATA *sus;
	char buf[MAX_STRING_LENGTH];

	if (!IS_AWAKE(ch) || ch->fighting)
		return false;

	/* najpierw update ewentualnych spraw do wyja�nienia */
	for (auto* sus : ch->suspects)
	{
		if (sus->status == SUS_FINED || sus->status == SUS_CRITICAL)
			continue;

		if (sus->age)
			sus->age--;

		/* cierpliwo�� si� sko�czy�a */
		if (!sus->age)
		{
			sus->status = SUS_CRITICAL;

			/* a gracz zwia�??? */
			if ((victim = get_char_room(ch, sus->name)) == NULL)
			{
				bool got_comlink = false;

				/* czy policjant ma komlink ? */
				for (auto* obj : ch->carrying)
				{
					if (obj->pIndexData->item_type == ITEM_COMLINK)
					{
						got_comlink = true;
						break;
					}
				}

				/* farciarz - wyszed� z gry (to na d�u�sz� met� nic nie da) */
				if ((victim = get_char_world(ch, sus->name)))
				{
					sprintf(buf, "%s do wszystkich jednostek!" NL
					"Cel akcji %s!" NL
					"Uwaga! Przeciwnik mo�e by� uzbrojony!" NL, NAME(ch, 0),
							sus->name);
					echo_to_area(ch->in_room->area, buf);

					/* gracz sobie przechlapa� */
					start_hating(ch, victim);
					start_hunting(ch, victim);

					/* ma komlink? no to meldujemy */
					if (got_comlink) /* Je�li policjant ma nadajnik
					 alarmuje innych */
						for (auto* hch : char_list)
						{
							if (!IS_NPC(hch) || !hch->in_room
									|| !hch->in_room->area
									|| (hch->in_room->area
											!= victim->in_room->area)
									|| (ch->pIndexData->vnum
											!= hch->pIndexData->vnum))
								continue;

							if (get_suspect(victim, hch))
								continue;

							start_suspecting(hch, victim, SUS_CRITICAL,
									sus->crime);

							sprintf(buf,
									"Przyj%sm. Obiekt %s" COL_SAY ". Wzmamagam czujno��.",
									FEMALE( hch ) ? "�a" : "��e",
									NAME(victim, 0));
							do_say(hch, buf);
						}
				}
				break;
			}
			else /* nie zwia� -- legalny areszt */
			{
				ch_tell(ch, victim,
						"W imieniu prawa aresztuj� ci�. Masz prawo nic nie m�wi�.");
				end_hunting(victim, ch, sus->crime);
			}
		}
		else if (sus->age == 1)
			do_emote(ch, (char*) "wyra�nie si� niecierpliwi.");
	}

	if (!IS_VIP1(ch))
		return false;

	/* poszukajny nowych podejrzanych */
	auto people_snapshot2 = ch->in_room->people;
	for (auto* victim : people_snapshot2)
	{
		CRIME_DATA *crime;

		if (IS_NPC(victim))
			continue;

		if (victim->crimes.empty())
			continue;

		if (!can_see(ch, victim))
			continue;

		for (auto* planet : planet_list)
		{
			if ( IS_VIP2(ch, planet)
					&& (crime = find_crime(victim, planet)) != NULL)
			{
				/* nowo spotkany podejrzany */
				if (!(sus = get_suspect(victim, ch)))
				{
					sprintf(buf, "Hej %s! Poszukujemy ci� na %s! R�ce do g�ry!",
							victim->name, capitalize(planet->name));
					do_say(ch, buf);
					act( PLAIN,
							"$n momentalnie wyci�ga blaster i mierzy w ciebie.",
							ch, NULL, victim, TO_VICT);
					act( PLAIN,
							"$n momentalnie wyci�ga blaster i mierzy w $N$3.",
							ch, NULL, victim, TO_NOTVICT);
					act( PLAIN, "Mierzysz w $N$3 blasterem.", ch, NULL, victim,
							TO_CHAR);
					start_suspecting(ch, victim, SUS_NEW, crime);
					break;
				}
				else /* lub stary wyga co ma ju� ob�aw� na planecie */
				if (sus->status == SUS_CRITICAL)
				{
					if (victim->hit > 0)
					{
						ch_tell(ch, victim, "I na co ci to by�o?");
						ch_tell(ch, victim, "Nie ma lito�ci dla z�oczy�c�w!");
						multi_hit(ch, victim, TYPE_UNDEFINED);
					}
					else/* jak ledwie zipie to do celi z nim! */
					{
						do_say(ch,
								(char*) "Mam ci� ptaszku. Teraz nie uciekniesz...");
						end_hunting(victim, ch, sus->crime);
					}
				}
			}
		}
	}

	return false;
}

bool spec_police_attack(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];

	if (!IS_AWAKE(ch) || ch->fighting)
		return false;

	if (!IS_VIP1(ch))
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{
		CRIME_DATA *crime;

		if (IS_NPC(victim))
			continue;

		if (victim->crimes.empty())
			continue;

		if (!can_see(ch, victim))
			continue;

		if (number_bits(1) == 0)
			continue;

		for (auto* planet : planet_list)
		{
			if ( IS_VIP2(ch, planet)
					&& (crime = find_crime(victim, planet)) != NULL)
			{
				sprintf(buf, "Hej! Poszukujemy ci� na %s! O �ESZ TY !!!",
						capitalize(planet->name));
				do_say(ch, buf);

				free_crime(victim, crime);

				multi_hit(ch, victim, TYPE_UNDEFINED);
				return true;
			}
		}
	}
	return false;
}

/*Przerobione zupe�nie przez Ganisa*/
bool spec_police_fine(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	map<SWString, bool> *my_memory;
	map<SWString, bool>::iterator it;

	/*Kazdy NPC bedzie mial swoja pamiec, zeby nie gadac caly czas osobie obok
	 niego stojacej, ze ta moze u niej wymazac swoje przestepstwa. Memory_flag
	 bedzie uzywane po to, zeby zauwazyc, ze ktos wyszedl z pomieszczenia i
	 usunac tego ktosia z pamieci */
	static map<CHAR_DATA*, map<SWString, bool> > memory;
	static bool memory_flag = true;

	my_memory = &(memory[ch]);
	memory_flag = !memory_flag;

	if (!IS_AWAKE(ch) || ch->fighting)
		return false;

	for (auto* victim : ch->in_room->people)
	{
		if (IS_NPC(victim))
			continue;

		if (victim->crimes.empty())
			continue;

		if (!can_see(ch, victim))
			continue;

		if (my_memory->find(victim->name) != my_memory->end())
		{
			(*my_memory)[victim->name] = memory_flag;
			continue;
		}

		//Widzimy tego ktosia, wiec dajmy mu znac, �e mo�e odkupic swoje winy
		sprintf(buf, "Hej Ty!!! Widzia�%sm Ci� na jednym z list�w go�czych!",
				SEX_SUFFIX_EAE(ch));
		do_say(ch, buf);
		sprintf(buf,
				"Je�li chcesz, to mog�, za drobn� op�at�, sprawi�, �e �wiat"
						" zapomni o Twojej kryminalnej przesz�o�ci...");
		do_say(ch, buf);
		sprintf(buf,
				"Wymazanie Twoich przest�pstw b�dzie kosztowa�o %d kredytek." NL,
				calculate_wanted_price(victim));
		send_to_char(buf, victim);

		//Zapamietac trzeba, ze jemu juz mowilismy!
		(*my_memory)[victim->name] = memory_flag;
		return true;
	}

	//Wymazac z pamieci nieobecnych
	for (it = my_memory->begin(); it != my_memory->end(); ++it)
	{
		if (it->second xor memory_flag)
			my_memory->erase(it);
	}

	if (my_memory->empty())
	{
		memory.erase(ch);
		my_memory = NULL;
	}
	return false;
}

bool spec_police_jail(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];

	if (!IS_AWAKE(ch) || ch->fighting)
		return false;

	if (!IS_VIP1(ch))
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{
		CRIME_DATA *crime;

		if (IS_NPC(victim))
			continue;

		if (victim->crimes.empty())
			continue;

		if (!can_see(ch, victim))
			continue;

		if (number_bits(1) == 0)
			continue;

		for (auto* planet : planet_list)
		{
			if ( IS_VIP2(ch, planet)
					&& (crime = find_crime(victim, planet)) != NULL)
			{
				sprintf(buf, "Hej! Poszukujemy ci� na %s!",
						capitalize(planet->name));
				do_say(ch, buf);

				if (ch->top_level >= victim->top_level)
					multi_hit(ch, victim, TYPE_UNDEFINED);

				jail_char(victim, ch, crime);
				return true;
			}
		}
	}

	return false;
}

bool spec_jedi_healer(CHAR_DATA *ch)
{
	CHAR_DATA *victim = nullptr;

	if (!IS_AWAKE(ch))
		return false;

	for (auto* vch : ch->in_room->people)
	{
		if (vch != ch && can_see(ch, vch) && number_bits(1) == 0)
		{
			victim = vch;
			break;
		}
	}

	if (!victim)
		return false;

	switch (number_bits(12))
	{
	case 0:
		act( COL_FORCE, "$n zatrzymuje si� i koncentruje na moment.", ch, NULL,
				NULL, TO_ROOM);
		spell_smaug(skill_lookup("armor"), ch->top_level, ch, victim);
		return true;

	case 1:
		act( COL_FORCE, "$n zatrzymuje si� i koncentruje na moment.", ch, NULL,
				NULL, TO_ROOM);
		spell_smaug(skill_lookup("good fortune"), ch->top_level, ch, victim);
		return true;

	case 2:
		act( COL_FORCE, "$n zatrzymuje si� i koncentruje na moment.", ch, NULL,
				NULL, TO_ROOM);
		spell_cure_blindness(skill_lookup("cure blindness"), ch->top_level, ch,
				victim);
		return true;

	case 3:
		act( COL_FORCE, "$n zatrzymuje si� i koncentruje na moment.", ch, NULL,
				NULL, TO_ROOM);
		spell_smaug(skill_lookup("cure light"), ch->top_level, ch, victim);
		return true;

	case 4:
		act( COL_FORCE, "$n zatrzymuje si� i koncentruje na moment.", ch, NULL,
				NULL, TO_ROOM);
		spell_cure_poison(skill_lookup("cure poison"), ch->top_level, ch,
				victim);
		return true;

	case 5:
		act( COL_FORCE, "$n zatrzymuje si� i koncentruje na moment.", ch, NULL,
				NULL, TO_ROOM);
		spell_smaug(skill_lookup("refresh"), ch->top_level, ch, victim);
		return true;

	}

	return false;
}

bool spec_dark_jedi(CHAR_DATA *ch)
{
	CHAR_DATA *victim = nullptr;
	const char *spell;
	int sn;

	if (ch->position != POS_FIGHTING)
		return false;

	for (auto* vch : ch->in_room->people)
	{
		if (who_fighting(vch) && number_bits(2) == 0)
		{
			victim = vch;
			break;
		}
	}

	if (!victim || victim == ch)
		return false;

	for (;;)
	{
		int min_level;

		switch (number_bits(4))
		{
		case 0:
			min_level = 5;
			spell = "blindness";
			break;
		case 1:
			min_level = 5;
			spell = "fingers of the force";
			break;
		case 2:
			min_level = 9;
			spell = "choke";
			break;
		case 3:
			min_level = 8;
			spell = "invade essence";
			break;
		case 4:
			min_level = 11;
			spell = "force projectile";
			break;
		case 6:
			min_level = 13;
			spell = "drain essence";
			break;
		case 7:
			min_level = 4;
			spell = "force whip";
			break;
		case 8:
			min_level = 13;
			spell = "harm";
			break;
		case 9:
			min_level = 9;
			spell = "force bolt";
			break;
		case 10:
			min_level = 1;
			spell = "force spray";
			break;
		default:
			return false;
		}

		if (ch->top_level >= min_level)
			break;
	}

	if ((sn = skill_lookup(spell)) < 0)
		return false;
	(*skill_table[sn]->spell_fun)(sn, ch->top_level, ch, victim);
	return true;
}

bool spec_fido(CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
		return false;

	auto contents_snapshot = ch->in_room->contents;
	for (auto* corpse : contents_snapshot)
	{
		if (corpse->item_type != ITEM_CORPSE_NPC)
			continue;

		act( COL_ACTION, "$n �apczywie rozszarpuje cia�o.", ch, NULL, NULL,
				TO_ROOM);
		auto corpse_contents_snapshot = corpse->contents;
		for (auto* obj : corpse_contents_snapshot)
		{
			obj_from_obj(obj);
			obj_to_room(obj, ch->in_room);
		}
		extract_obj(corpse);
		return true;
	}

	return false;
}

bool spec_stormtrooper(CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch) || ch->fighting)
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{
		if (!can_see(ch, victim))
			continue;
		if (get_timer(victim, TIMER_RECENTFIGHT) > 0)
			continue;
		if (( IS_NPC(victim)
				&& nifty_is_name((char*) "republic", victim->name)
				&& victim->fighting && who_fighting(victim) != ch)
				|| (!IS_NPC(victim) && victim->pcdata && victim->pcdata->clan
						&& IS_AWAKE(victim)
						&& nifty_is_name((char*) "Nowa Republika",
								victim->pcdata->clan->name)))
		{
			do_yell(ch, (char*) "Gi� Rebeliancka Gnido!");
			multi_hit(ch, victim, TYPE_UNDEFINED);
			return true;
		}

	}

	return false;

}

bool spec_new_republic_trooper(CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch) || ch->fighting)
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{
		if (!can_see(ch, victim))
			continue;
		if (get_timer(victim, TIMER_RECENTFIGHT) > 0)
			continue;
		if (( IS_NPC(victim)
				&& nifty_is_name((char*) "imperial", victim->name)
				&& victim->fighting && who_fighting(victim) != ch)
				|| (!IS_NPC(victim) && victim->pcdata && victim->pcdata->clan
						&& IS_AWAKE(victim)
						&& nifty_is_name((char*) "Imperium",
								victim->pcdata->clan->name)))
		{
			do_yell(ch, (char*) "W imi� Nowej Republiki!");
			multi_hit(ch, victim, TYPE_UNDEFINED);
			return true;
		}

	}

	return false;

}

bool spec_guardian(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim = nullptr;
	CHAR_DATA *ech;
	const char *crime;
	int max_evil;

	if (!IS_AWAKE(ch) || ch->fighting)
		return false;

	max_evil = 300;
	ech = NULL;
	crime = "";

	for (auto* victim : ch->in_room->people)
	{
		if (victim->fighting && who_fighting(victim) != ch
				&& victim->alignment < max_evil)
		{
			max_evil = victim->alignment;
			ech = victim;
		}
	}

	if (victim && IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		sprintf(buf, "%s to zasrany %s! No i oczywi�cie TCH�RZ!", victim->name,
				crime);
		do_yell(ch, buf);
		return true;
	}

	if (victim)
	{
		sprintf(buf, "%s to zasrany %s! CHRONI� NIEWINNYCH !!!", victim->name,
				crime);
		do_shout(ch, buf);
		multi_hit(ch, victim, TYPE_UNDEFINED);
		return true;
	}

	if (ech)
	{
		act( COL_YELL, "$n krzyczy 'CHRONI� NIEWINNYCH !!!'", ch, NULL, NULL,
				TO_ROOM);
		multi_hit(ch, ech, TYPE_UNDEFINED);
		return true;
	}

	return false;
}

bool spec_janitor(CHAR_DATA *ch)
{
	if (!IS_AWAKE(ch))
		return false;

	auto contents_snapshot = ch->in_room->contents;
	for (auto* trash : contents_snapshot)
	{
		if (!IS_SET(trash->wear_flags, ITEM_TAKE)
				|| IS_OBJ_STAT(trash, ITEM_BURRIED) || trash->inquest)/* <-- bug fix by Thanos */
			continue;
		if (trash->item_type == ITEM_DRINK_CON || trash->item_type == ITEM_TRASH
				|| trash->cost < 10)
		{
			act( COL_ACTION, "$n podnosi z ziemi jakie� �mieci.", ch, NULL,
					NULL, TO_ROOM);
			obj_from_room(trash);
			obj_to_char(trash, ch);
			return true;
		}
	}

	return false;
}

bool spec_poison(CHAR_DATA *ch)
{
	CHAR_DATA *victim;

	if (ch->position != POS_FIGHTING || (victim = who_fighting(ch)) == NULL
			|| number_percent() > 2 * ch->top_level)
		return false;

	act( COL_HIT, "Gryziesz $N$3!", ch, NULL, victim, TO_CHAR);
	act( COL_ACTION, "$n gryzie $N$3!", ch, NULL, victim, TO_NOTVICT);
	act( COL_POISON, "$n gryzie ci�!", ch, NULL, victim, TO_VICT);
	spell_poison(gsn_poison, ch->top_level, ch, victim);
	return true;
}

bool spec_thief(CHAR_DATA *ch)
{
	int gold, maxgold;

	if (ch->position != POS_STANDING)
		return false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{

		if ( IS_NPC(victim) || get_trust(victim) >= LEVEL_IMMORTAL
				|| number_bits(2) != 0 || !can_see(ch, victim)) /* Thx Glop */
			continue;

		if ( IS_AWAKE(victim) && number_range(0, ch->top_level) == 0)
		{
			act( COL_ACTION, "Odkrywasz r�k� $n$1 w swojej kieszeni!", ch, NULL,
					victim, TO_VICT);
			act( COL_ACTION, "$N odkrywa r�k� $n$1 w swojej kieszeni!", ch,
					NULL, victim, TO_NOTVICT);
			return true;
		}
		else
		{
			maxgold = ch->top_level * ch->top_level * 1000;
			gold = victim->gold
					* number_range(1, URANGE(2, ch->top_level / 4, 10)) / 100;
			ch->gold += 9 * gold / 10;
			victim->gold -= gold;
			if (ch->gold > maxgold)
			{
				boost_economy(ch->in_room->area, ch->gold - maxgold / 2);
				ch->gold = maxgold / 2;
			}
			return true;
		}
	}

	return false;
}

bool spec_auth(CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_INDEX_DATA *pObjIndex;
	OBJ_DATA *obj;
	bool hasdiploma;
	bool ban = false;

	auto people_snapshot = ch->in_room->people;
	for (auto* victim : people_snapshot)
	{

		if (!victim->desc)
			continue;

		/* Thanos:          wieczni newbie za kar� */
		for (auto* pban : ban_list)
		{
			if ((!str_suffix(pban->name, victim->desc->host))
					&& pban->level == BAN_SOFT)
			{
				ban = true;
				ch_tell(ch, victim, "O nie. Nie licz na autoryzacj�!");
				ch_tell(ch, victim, "Tacy jak ty jej ode mnie nie dostan�.");
				interpret(victim, (char*) "u"); /* :) */
			}
		}

		if (ban)
			continue;

		if (!IS_NPC(victim)
				&& (pObjIndex = get_obj_index( OBJ_VNUM_SCHOOL_DIPLOMA))
						!= NULL)
		{
			hasdiploma = false;

			for (auto* obj : victim->carrying)
				if (obj->pIndexData == get_obj_index( OBJ_VNUM_SCHOOL_DIPLOMA))
					hasdiploma = true;

			if (!hasdiploma)
			{
				obj = create_object(pObjIndex, 1);
				obj = obj_to_char(obj, victim);
				act( FG_CYAN, "$n wr�cza ci $p$3 i �ciska twoj� r�k�.", ch, obj,
						victim, TO_VICT);
				act( FG_CYAN, "$n wr�cza $N$2 $p$3 i �ciska $G r�k�.", ch, obj,
						victim, TO_NOTVICT);
			}
		}

		if ( IS_NPC(victim) || !IS_SET(victim->pcdata->flags, PCFLAG_UNAUTHED))
			continue;

		victim->pcdata->auth_state = 3;
		REMOVE_BIT(victim->pcdata->flags, PCFLAG_UNAUTHED);
		STRDUP(victim->pcdata->authed_by, ch->przypadki[0]);
		sprintf(buf, "%s (#%d) authorized %s", ch->przypadki[0],
				ch->pIndexData->vnum, victim->przypadki[0]);
		to_channel(buf, CHANNEL_MONITOR, "Monitor", ch->top_level);
	}
	return false;
}

bool spec_prosecutor(CHAR_DATA *ch)
{
	char buf[MSL];
	int gest;

	gest = number_range(1, 1500);

	if (gest <= 5)
	{
		do_emote(ch, (char*) "przerzuca jakie� papiery warcz�c pod nosem.");
	}
	else if (gest <= 10)
	{
		act( PLAIN, "$n klnie jak szewc. To nie jest $s dzie�.", ch, NULL, NULL,
				TO_ROOM);
	}
	else if (gest == 75)
	{
		do_emote(ch, (char*) "wyci�ga komunikator.");
		sprintf(buf, "Tu %s. Macie jeszcze tego Kalamaria�skiego przemytnika?",
				ch->przypadki[0]);
		do_say(ch, buf);
		do_say(ch, (char*) "Ahh, stracony.. C�. Bywa i tak.");
	}

	return true;
}
