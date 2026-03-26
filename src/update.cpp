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
 *			      Regular update module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include "mud.h"

/* from swskills.c */
void add_reinforcements args( ( CHAR_DATA *ch ) );
const SWString name_expand args( (CHAR_DATA *ch ) );
void scripts_update args( ( void ) );

/*
 * Local functions.
 */
int hit_gain args( ( CHAR_DATA *ch ) );
int mana_gain args( ( CHAR_DATA *ch ) );
int move_gain args( ( CHAR_DATA *ch ) );
void gain_addiction args( ( CHAR_DATA *ch ) );
void mobile_update args( ( void ) );
void weather_update args( ( void ) );
void bacta_update args( ( void ) );
void update_taxes args( ( void ) );
void char_update args( ( void ) );
void obj_update args( ( void ) );
void aggr_update args( ( void ) );
void room_act_update args( ( void ) );
void obj_act_update args( ( void ) );
void char_check args( ( void ) );
void update_explosives args( ( void ) ); /* by Lomion */
void update_mining args( ( void ) ); /* by Aldegard */
void drunk_randoms args( ( CHAR_DATA *ch ) );
void halucinations args( ( CHAR_DATA *ch ) );
void subtract_times args( ( struct timeval *etime,
				struct timeval *stime ) );
void clan_update args( ( void ) );

void add_material_warehouse(PLANET_DATA *planet, MATERIAL_DATA *pMat, int quant);

/*
 * Global Variables
 */
/* gch_prev and gobj_prev removed - using snapshot iteration instead */
CHAR_DATA *timechar;

/*
 * Global functions
 */
void starsystems_update args( ( void ) ); /* Thanos (space_reset.c) */
void crime_update args( ( void ) );/* Thanos --> crimes.c */
void send_auction_notes(AUCTION_DATA *pAuction, auction_ends typ);
void send_auction_info(AUCTION_DATA *pAuction);
void auction_cleanup(AUCTION_DATA *pAuction);
MATERIAL_DATA* get_material(char *name, material_type typ);

const char *const corpse_descs[] =
{ "Martwe cia�o %s rozk�ada si� tutaj.", "Martwe cia�o %s le�y tutaj.", "Martwe cia�o %s le�y tutaj.", "Martwe cia�o %s le�y tutaj.",
		"Martwe cia�o %s le�y tutaj." };

const char *const d_corpse_descs[] =
{ "Zmasakrowane cz�ci %s rdzewiej� tutaj.", "Le�� tu zmasakrowane cz�ci %s.", "Le�� tu zmasakrowane cz�ci %s.",
		"Le�� tu zmasakrowane cz�ci %s.", "Le�� tu zmasakrowane cz�ci %s." };

extern int top_exit;

/*
 * Advancement stuff.
 */
int max_level(CHAR_DATA *ch, int ability)
{
	int level = 0;

	if (IS_NPC(ch))
		return 100;

	if (IS_IMMORTAL(ch))
		return 200;

	if (ability == FORCE_ABILITY)
	{
		if (ch->main_ability == ability)
			level = 20;
		else
			level = 0;

		level = ch->perm_frc * 5;

		return level;
	}

	if (ch->main_ability == ability)
		return 100;

	if (ability == COMBAT_ABILITY)
	{
		if (ch->main_ability == FORCE_ABILITY)
			level = 25;
		if (ch->main_ability == HUNTING_ABILITY)
			level = 20;
		if (IS_RACE(ch, "Wookiee"))
			level += 20;
		if (IS_RACE(ch, "Noghri"))
			level += 15;
		if (IS_RACE(ch, "Gamorrean"))
			level += 10;
		if (IS_RACE(ch, "Defel"))
			level += 15;
		if (IS_RACE(ch, "Trandoshan"))
			level += 15;
		if (IS_RACE(ch, "Duinuogwuin"))
			level += 20;
		if (IS_RACE(ch, "Yuuzhan Vong"))
			level += 50;
		if (IS_RACE(ch, "Aqualish"))
			level += 5;
		if (IS_RACE(ch, "Barabel"))
			level += 20;
		if (IS_RACE(ch, "Iktotchi"))
			level += 10;
		level += ch->perm_con + ch->perm_dex + ch->perm_str;
	}

	if (ability == PILOTING_ABILITY)
	{
		if (ch->main_ability == ENGINEERING_ABILITY)
			level = 30;
		if (ch->main_ability == HUNTING_ABILITY)
			level = 20;
		if (ch->main_ability == SMUGGLING_ABILITY)
			level = 40;
		if (ch->main_ability == FORCE_ABILITY)
			level = 25;
		if (IS_RACE(ch, "Hutt"))
			level -= 50;
		if (IS_RACE(ch, "Noghri"))
			level -= 15;
		if (IS_RACE(ch, "Gamorrean"))
			level -= 10;
		if (IS_RACE(ch, "Quarren"))
			level += 10;
		if (IS_RACE(ch, "Duinuogwuin"))
			level -= 30;
		if (IS_RACE(ch, "Yuuzhan Vong"))
			level += 50;
		if (IS_RACE(ch, "Barabel"))
			level -= 10;
		if (IS_RACE(ch, "Devaronian"))
			level += 10;
		if (IS_RACE(ch, "Duros"))
			level += 25;
		if (IS_RACE(ch, "Sullustan"))
			level += 20;
		if (IS_RACE(ch, "Iktotchi"))
			level += 15;
		level += ch->perm_dex * 2;
	}

	if (ability == ENGINEERING_ABILITY)
	{
		if (ch->main_ability == PILOTING_ABILITY)
			level = 20;
		if (IS_RACE(ch, "Wookiee"))
			level += 20;
		if (IS_RACE(ch, "Rodian"))
			level -= 10;
		if (IS_RACE(ch, "Hutt"))
			level -= 20;
		if (IS_RACE(ch, "Mon Calamari"))
			level += 10;
		if (IS_RACE(ch, "Gamorrean"))
			level -= 20;
		if (IS_RACE(ch, "Verpine"))
			level += 50;
		if (IS_RACE(ch, "Trandoshan"))
			level -= 10;
		if (IS_RACE(ch, "Chadra-Fan"))
			level += 20;
		if (IS_RACE(ch, "Yuuzhan Vong"))
			level -= 50;
		if (IS_RACE(ch, "Bothan"))
			level += 10;
		if (IS_RACE(ch, "Duros"))
			level += 15;
		if (IS_RACE(ch, "Sullustan"))
			level += 20;
		if (IS_RACE(ch, "Iktotchi"))
			level -= 15;
		level += ch->perm_int * 2;
	}

	if (ability == HUNTING_ABILITY)
	{
		if (IS_RACE(ch, "Wookiee"))
			level += 10;
		if (IS_RACE(ch, "Rodian"))
			level += 40;
		if (IS_RACE(ch, "Twi'lek"))
			level += 10;
		if (IS_RACE(ch, "Hutt"))
			level -= 20;
		if (IS_RACE(ch, "Jawa"))
			level -= 10;
		if (IS_RACE(ch, "Adarian"))
			level -= 30;
		if (IS_RACE(ch, "Trandoshan"))
			level += 20;
		if (IS_RACE(ch, "Chadra-Fan"))
			level -= 25;
		if (IS_RACE(ch, "Yuuzhan Vong"))
			level += 50;
		if (IS_RACE(ch, "Barabel"))
			level += 10;
		if (IS_RACE(ch, "Bith"))
			level -= 15;
		if (IS_RACE(ch, "Duros"))
			level -= 15;
		if (IS_RACE(ch, "Gran"))
			level += 10;
		if (IS_RACE(ch, "Ithorian"))
			level -= 10;
		if (IS_RACE(ch, "Sullustan"))
			level -= 10;
		level += ch->perm_wis;
	}

	if (ability == SMUGGLING_ABILITY)
	{
		if (ch->main_ability == PILOTING_ABILITY)
			level = 20;
		if (ch->main_ability == ENGINEERING_ABILITY)
			level = 25;
		if (IS_RACE(ch, "Twi'lek"))
			level += 30;
		if (IS_RACE(ch, "Mon Calamari"))
			level -= 20;
		if (IS_RACE(ch, "Jawa"))
			level += 30;
		if (IS_RACE(ch, "Adarian"))
			level -= 20;
		if (IS_RACE(ch, "Verpine"))
			level -= 20;
		if (IS_RACE(ch, "Defel"))
			level += 25;
		if (IS_RACE(ch, "Quarren"))
			level += 20;
		if (IS_RACE(ch, "Chadra-Fan"))
			level += 10;
		if (IS_RACE(ch, "Duinuogwuin"))
			level -= 10;
		if (IS_RACE(ch, "Aqualish"))
			level += 10;
		if (IS_RACE(ch, "Bith"))
			level += 10;
		if (IS_RACE(ch, "Bothan"))
			level += 15;
		if (IS_RACE(ch, "Devaronian"))
			level += 15;
		level += ch->perm_lck + ch->perm_dex;
	}

	if (ability == LEADERSHIP_ABILITY)
	{
		if (ch->main_ability == COMBAT_ABILITY)
			level = 50;
		if (ch->main_ability == DIPLOMACY_ABILITY)
			level = 50;
		if (IS_RACE(ch, "Twi'lek"))
			level -= 25;
		if (IS_RACE(ch, "Rodian"))
			level -= 20;
		if (IS_RACE(ch, "Hutt"))
			level += 50;
		if (IS_RACE(ch, "Mon Calamari"))
			level += 30;
		if (IS_RACE(ch, "Jawa"))
			level -= 10;
		if (IS_RACE(ch, "Adarian"))
			level += 10;
		if (IS_RACE(ch, "Trandoshan"))
			level -= 10;
		if (IS_RACE(ch, "Quarren"))
			level -= 10;
		if (IS_RACE(ch, "Yuuzhan Vong"))
			level += 50;
		if (IS_RACE(ch, "Cerean"))
			level += 20;
		if (IS_RACE(ch, "Iktotchi"))
			level += 10;
		level += ch->perm_wis + ch->perm_cha + ch->perm_int;
	}

	if (ability == DIPLOMACY_ABILITY)
	{
		if (ch->main_ability == PILOTING_ABILITY)
			level = 10;
		if (ch->main_ability == LEADERSHIP_ABILITY)
			level = 35;
		if (IS_RACE(ch, "Wookiee"))
			level -= 50;
		if (IS_RACE(ch, "Twi'lek"))
			level -= 25;
		if (IS_RACE(ch, "Rodian"))
			level -= 20;
		if (IS_RACE(ch, "Hutt"))
			level -= 10;
		if (IS_RACE(ch, "Mon Calamari"))
			level += 10;
		if (IS_RACE(ch, "Noghri"))
			level -= 50;
		if (IS_RACE(ch, "Gamorrean"))
			level -= 10;
		if (IS_RACE(ch, "Jawa"))
			level -= 10;
		if (IS_RACE(ch, "Adarian"))
			level += 40;
		if (IS_RACE(ch, "Verpine"))
			level -= 10;
		if (IS_RACE(ch, "Defel"))
			level -= 30;
		if (IS_RACE(ch, "Trandoshan"))
			level -= 30;
		if (IS_RACE(ch, "Quarren"))
			level -= 20;
		if (IS_RACE(ch, "Duinuogwuin"))
			level += 20;
		if (IS_RACE(ch, "Yuuzhan Vong"))
			level -= 10;
		if (IS_RACE(ch, "Aqualish"))
			level -= 10;
		if (IS_RACE(ch, "Arcona"))
			level += 10;
		if (IS_RACE(ch, "Bith"))
			level += 25;
		if (IS_RACE(ch, "Cerean"))
			level += 10;
		if (IS_RACE(ch, "Gotal"))
			level += 30;
		if (IS_RACE(ch, "Zabrak"))
			level -= 10;
		level += ch->perm_cha * 3;
	}

#if 0
	level/=1.5;
#endif
	level = URANGE(1, level, 100);
	return level;
}

void advance_level(CHAR_DATA *ch, int ability)
{
//	Changed by Thanos

	if (ch->top_level < ch->skill_level[ability] && ch->top_level < 100)
	{
		/* ch->top_level = URANGE( 1 , ch->skill_level[ability] , 100 ); */
		ch->top_level = URANGE(1, ch->skill_level[ch->main_ability], 100);
		fevent_trigger(ch, FE_LEVEL);
	}

	if (!IS_NPC(ch))
		REMOVE_BIT(ch->act, PLR_BOUGHT_PET);

	if (ability == ch->main_ability)
	{
		ch->max_move++;
		ch->max_hit++;
	}

	if (ability == FORCE_ABILITY)
		ch->max_mana++;

#if defined( ARMAGEDDON )
	if( ability == ch->main_ability )
	{
		int prac;

		prac = 5 + wis_app[get_curr_wis( ch )].practice;
		ch->pcdata->practices += prac;

		ch_printf( ch, "Otrzymujesz " FB_WHITE "%d" PLAIN "(%d) prakty%s." NL,
			prac, ch->pcdata->practices,
			prac==1 ? "k�" :
			(prac > 1  && prac<5)? "ki"  : prac < 22 ? "k":
			(prac%10 > 1  && prac%10<5)? "ki" :"k" );
	}
#endif
	fevent_trigger(ch, FE_SKLVL);
	return;
}

void gain_exp(CHAR_DATA *ch, int gain, int ability)
{
	if (IS_NPC(ch))
		return;

	ch->experience[ability] = UMAX(0, ch->experience[ability] + gain);

	/*  Removed by Thanos - fuj!
	 if (NOT_AUTHED(ch) && ch->experience[ability] >= exp_level(ch->skill_level[ability]+1))
	 {
	 send_to_char("Dop�ki twoja posta� nie dostanie autotoryzacji, nie b�dzie mog�a zdoby� poziomu.\n\r", ch);
	 ch->experience[ability] = (exp_level( ch->skill_level[ability]+1 ) - 1);
	 return;
	 }*/

	while (ch->experience[ability] >= exp_level(ch->skill_level[ability] + 1))
	{
		if (ch->skill_level[ability] >= max_level(ch, ability))
		{
			ch->experience[ability] = (exp_level(ch->skill_level[ability] + 1) - 1);
			return;
		}
		ch_printf(ch, FB_WHITE
		"* Opanowujesz nowe tajniki %s !!! Znasz je ju� na %d poziomie! *" EOL, class_table[ability].przypadki[1],
				++ch->skill_level[ability]);
		advance_level(ch, ability);
	}
	return;
}

/*
 * Regeneration stuff.
 */
int hit_gain(CHAR_DATA *ch)
{
	int gain;

	if (IS_NPC(ch))
		gain = ch->top_level;
	else
	{
		gain = UMIN(5, ch->top_level);
		switch (ch->position)
		{
		case POS_DEAD:
			return 0;
		case POS_MORTAL:
			return -25;
		case POS_INCAP:
			return -20;
		case POS_STUNNED:
			return get_curr_con(ch) * 2;
		case POS_SLEEPING:
			gain += get_curr_con(ch) * 1.5;
			break;
		case POS_RESTING:
			gain += get_curr_con(ch);
			break;
		}
		if (ch->pcdata->condition[COND_FULL] == 0)
			gain /= 2;
		if (ch->pcdata->condition[COND_THIRST] == 0)
			gain /= 2;
	}

	if (IS_AFFECTED(ch, AFF_POISON))
		gain /= 4;

	if ((get_age(ch) > 800) && !IS_IMMORTAL(ch))
	{
		ch_printf(ch, "Jeste� ju� star%s." NL
		"Stajesz si� s�absz%s z ka�d� chwil�." NL
		"Nied�ugo umrzesz." NL, SEX_SUFFIX_YAE(ch), SEX_SUFFIX_YAE(ch));
		return -10;
	}
	else if (get_age(ch) > 500)
		gain /= 10;
	else if (get_age(ch) > 400)
		gain /= 5;
	else if (get_age(ch) > 300)
		gain /= 2;

	if (IS_RACE(ch, "TRANDOSHAN"))
		gain *= 2;
	return UMIN(gain, ch->max_hit - ch->hit);
}

int mana_gain(CHAR_DATA *ch)
{
	int gain;

	if (IS_NPC(ch))
		gain = ch->top_level;
	else
	{
		if (ch->skill_level[FORCE_ABILITY] <= 1)
			return (0 - ch->mana);

		gain = UMIN(5, ch->skill_level[FORCE_ABILITY] / 2);

		if (ch->position < POS_SLEEPING)
			return 0;
		switch (ch->position)
		{
		case POS_SLEEPING:
			gain += get_curr_int(ch) * 3;
			break;
		case POS_RESTING:
			gain += get_curr_int(ch) * 1.5;
			break;
		}

		if (ch->pcdata->condition[COND_FULL] == 0)
			gain /= 2;

		if (ch->pcdata->condition[COND_THIRST] == 0)
			gain /= 2;
	}

	if (IS_AFFECTED(ch, AFF_POISON))
		gain /= 4;
	return UMIN(gain, ch->max_mana - ch->mana);
}

int move_gain(CHAR_DATA *ch)
{
	int gain;

	if (IS_NPC(ch))
		gain = ch->top_level;
	else
	{
		gain = UMAX(15, 2 * ch->top_level);

		switch (ch->position)
		{
		case POS_DEAD:
			return 0;
		case POS_MORTAL:
			return -1;
		case POS_INCAP:
			return -1;
		case POS_STUNNED:
			return 1;
		case POS_SLEEPING:
			gain += get_curr_dex(ch) * 2;
			break;
		case POS_RESTING:
			gain += get_curr_dex(ch);
			break;
		}
		if (ch->pcdata->condition[COND_FULL] == 0)
			gain /= 2;

		if (ch->pcdata->condition[COND_THIRST] == 0)
			gain /= 2;
	}

	if (IS_AFFECTED(ch, AFF_POISON))
		gain /= 4;
	if (get_age(ch) > 500)
		gain /= 10;
	else if (get_age(ch) > 300)
		gain /= 5;
	else if (get_age(ch) > 200)
		gain /= 2;
	return UMIN(gain, ch->max_move - ch->move);
}

void gain_addiction(CHAR_DATA *ch)
{
	short drug;
	AFFECT_DATA af;

	for (drug = 0; drug <= 9; drug++)
	{

		if (ch->pcdata->addiction[drug] < ch->pcdata->drug_level[drug])
			ch->pcdata->addiction[drug]++;

		if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 150)
		{
			switch (ch->pcdata->addiction[drug])
			{
			default:
			case SPICE_GLITTERSTIM:
				if (!IS_AFFECTED(ch, AFF_BLIND))
				{
					af.type = gsn_blindness;
					af.location = APPLY_AC;
					af.modifier = 10;
					af.duration = ch->pcdata->addiction[drug];
					af.bitvector = AFF_BLIND;
					affect_to_char(ch, &af);
				}
			case SPICE_CARSANUM:
				if (!IS_AFFECTED(ch, AFF_WEAKEN))
				{
					af.type = -1;
					af.location = APPLY_DAMROLL;
					af.modifier = -10;
					af.duration = ch->pcdata->addiction[drug];
					af.bitvector = AFF_WEAKEN;
					affect_to_char(ch, &af);
				}
			case SPICE_RYLL:
				if (!IS_AFFECTED(ch, AFF_WEAKEN))
				{
					af.type = -1;
					af.location = APPLY_DEX;
					af.modifier = -5;
					af.duration = ch->pcdata->addiction[drug];
					af.bitvector = AFF_WEAKEN;
					affect_to_char(ch, &af);
				}
			case SPICE_ANDRIS:
				if (!IS_AFFECTED(ch, AFF_WEAKEN))
				{
					af.type = -1;
					af.location = APPLY_CON;
					af.modifier = -5;
					af.duration = ch->pcdata->addiction[drug];
					af.bitvector = AFF_WEAKEN;
					affect_to_char(ch, &af);
				}
			}
		}

		if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 200)
		{
			ch_printf(ch, "Czujesz, �e za chwil� umrzesz. POTRZEBUJESZ %s." NL, spice_types_list[drug]);
			worsen_mental_state(ch, 2);
			damage(ch, ch, 5, TYPE_UNDEFINED);
		}
		else if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 100)
		{
			ch_printf(ch, "Potrzebujesz %s." NL, spice_types_list[drug]);
			worsen_mental_state(ch, 2);
		}
		else if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 50)
		{
			ch_printf(ch, "Naprawd� przyda�oby si� troch� %s." NL, spice_types_list[drug]);
			worsen_mental_state(ch, 1);
		}
		else if (ch->pcdata->addiction[drug] > ch->pcdata->drug_level[drug] + 25)
		{
			ch_printf(ch, "Troszk� %s i twoje samopoczucie by�oby znacznie lepsze." NL, spice_types_list[drug]);
		}
		else if (ch->pcdata->addiction[drug] < ch->pcdata->drug_level[drug] - 50)
		{
			act( COL_POISON, "$n wymiotuje." NL, ch, NULL, NULL, TO_ROOM);
			act( COL_POISON, "Wymiotujesz." NL, ch, NULL, NULL, TO_CHAR);
			ch->pcdata->drug_level[drug] -= 10;
		}

		if (ch->pcdata->drug_level[drug] > 1)
			ch->pcdata->drug_level[drug] -= 2;
		else if (ch->pcdata->drug_level[drug] > 0)
			ch->pcdata->drug_level[drug] -= 1;
		else if (ch->pcdata->addiction[drug] > 0 && ch->pcdata->drug_level[drug] <= 0)
			ch->pcdata->addiction[drug]--;
	}
}

void gain_condition(CHAR_DATA *ch, int iCond, int value)
{
	int condition;
	ch_ret retcode = 0;

	if (value == 0 || IS_NPC(ch) || get_trust(ch) >= LEVEL_IMMORTAL)
		/*|| NOT_AUTHED(ch))*/ //Removed by Thanos
		return;

	/* Thanos the lito�ciwy */
	if (value < 0 && number_percent() > 80)
		return;

	condition = ch->pcdata->condition[iCond];
	if (iCond != COND_DRUNK)
		ch->pcdata->condition[iCond] = URANGE(0, condition + value, COND_MAX);
	else
		ch->pcdata->condition[iCond] = URANGE(0, condition + value, 68);

	if (ch->pcdata->condition[iCond] == 0)
	{
		switch (iCond)
		{
		case COND_FULL:
			if (ch->top_level <= LEVEL_AVATAR)
			{
				send_to_char( COL_HUNGRY "UMIERASZ z g�odu!" EOL, ch);
				act( COL_HUNGRY, "$n jest strasznie wyg�odzon$y!", ch, NULL, NULL, TO_ROOM);
				worsen_mental_state(ch, 1);
				retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
			}
			break;

		case COND_THIRST:
			if (ch->top_level <= LEVEL_AVATAR)
			{
				send_to_char( COL_THIRSTY "Umierasz z PRAGNIENIA!" EOL, ch);
				act( COL_THIRSTY, "$n umiera z pragnienia!", ch, NULL, NULL,
				TO_ROOM);
				worsen_mental_state(ch, 2);
				retcode = damage(ch, ch, 5, TYPE_UNDEFINED);
			}
			break;

		case COND_DRUNK:
			if (condition != 0)
				send_to_char("Trze�wiejesz." NL, ch);
			retcode = rNONE;
			break;
		default:
			bug("invalid condition type %d", iCond);
			retcode = rNONE;
			break;
		}
	}

	if (retcode != rNONE)
		return;

	if (ch->pcdata->condition[iCond] == COND_MAX / 16)
	{
		switch (iCond)
		{
		case COND_FULL:
			if (ch->top_level <= LEVEL_AVATAR)
			{
				send_to_char( COL_HUNGRY "G��d staje si� coraz silniejszy." EOL, ch);
				act( COL_HUNGRY, "S�yszysz burczenie w brzuchu $n$1.", ch, NULL, NULL, TO_ROOM);
				if (number_bits(1) == 0)
					worsen_mental_state(ch, 1);
			}
			break;

		case COND_THIRST:
			if (ch->top_level <= LEVEL_AVATAR)
			{
				send_to_char( COL_THIRSTY "Twoje pragnienie wzmaga si�." EOL, ch);
				worsen_mental_state(ch, 1);
				act( COL_THIRSTY, "$n wygl�da jakby potrzebowa�$o czego� do picia.", ch, NULL, NULL, TO_ROOM);
			}
			break;

		case COND_DRUNK:
			if (condition != 0)
				send_to_char("Twoja g�owa staje si� nieco ci�sza." NL, ch);
			break;
		}
	}

	if (ch->pcdata->condition[iCond] == COND_MAX / 8)
	{
		switch (iCond)
		{
		case COND_FULL:
			if (ch->top_level <= LEVEL_AVATAR)
				ch_printf(ch, COL_HUNGRY "Jeste� g�odn%s." EOL, SEX_SUFFIX_YAE(ch));
			break;
		case COND_THIRST:
			if (ch->top_level <= LEVEL_AVATAR)
				ch_printf(ch, COL_THIRSTY "Jeste� spragnion%s." EOL, SEX_SUFFIX_YAE(ch));
			break;

		}
	}

	if (ch->pcdata->condition[iCond] == COND_MAX / 4)
	{
		switch (iCond)
		{
		case COND_FULL:
			if (ch->top_level <= LEVEL_AVATAR)
				ch_printf(ch, COL_HUNGRY "Przegryz�%sby� co�." EOL, SEX_SUFFIX__AO(ch));
			break;

		case COND_THIRST:
			if (ch->top_level <= LEVEL_AVATAR)
				ch_printf(ch, COL_THIRSTY "Ch�tnie by� si� czego� napi�%s." EOL, SEX_SUFFIX__AO(ch));
			break;

		}
	}
	return;
}

/*
 * Mob autonomous action.
 * This function takes 25% to 35% of ALL Mud cpu time.
 */
void mobile_update(void)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *ch;
	EXIT_DATA *pexit;
	int door;
	ch_ret retcode;

	retcode = rNONE;

	/* Examine all mobs. */
	auto char_snapshot = char_list;
	for (auto it = char_snapshot.rbegin(); it != char_snapshot.rend(); ++it)
	{
		ch = *it;
		set_cur_char(ch);

		if (char_died(ch))
			continue;

		if (!IS_NPC(ch))
		{
			drunk_randoms(ch);
			halucinations(ch);
			continue;
		}
		if (!ch->in_room || IS_AFFECTED(ch, AFF_CHARM) || IS_AFFECTED(ch, AFF_PARALYSIS))
			continue;

		/* Clean up 'animated corpses' that are not charmed' - Scryn */

		if (ch->pIndexData->vnum == 5 && !IS_AFFECTED(ch, AFF_CHARM))
		{
			if (!ch->in_room->people.empty())
				act(COL_FORCE, "$n obraca si� w proch i wraca tam, sk�d przyby�$o.", ch, NULL, NULL, TO_ROOM);

			if (IS_NPC(ch)) /* Guard against purging switched? */
				extract_char(ch, true);
			continue;
		}

		if (!IS_SET(ch->act, ACT_RUNNING) && !IS_SET(ch->act, ACT_SENTINEL) && !ch->fighting && ch->hunting)
		{
			if (ch->top_level < 20)
				WAIT_STATE(ch, 6 * PULSE_PER_SECOND);
			else if (ch->top_level < 40)
				WAIT_STATE(ch, 5 * PULSE_PER_SECOND);
			else if (ch->top_level < 60)
				WAIT_STATE(ch, 4 * PULSE_PER_SECOND);
			else if (ch->top_level < 80)
				WAIT_STATE(ch, 3 * PULSE_PER_SECOND);
			else if (ch->top_level < 100)
				WAIT_STATE(ch, 2 * PULSE_PER_SECOND);
			else
				WAIT_STATE(ch, 1 * PULSE_PER_SECOND);
			hunt_victim(ch);
			continue;
		}
		else if (!ch->fighting && !ch->hunting && !IS_SET(ch->act, ACT_RUNNING) && ch->was_sentinel && ch->position >= POS_STANDING)
		{
			act( COL_ACTION, "$n odchodzi.", ch, NULL, NULL, TO_ROOM);
			char_from_room(ch);
			char_to_room(ch, ch->was_sentinel);
			act( COL_ACTION, "$n w�a�nie przyby�$o.", ch, NULL, NULL, TO_ROOM);
			SET_BIT(ch->act, ACT_SENTINEL);
			ch->was_sentinel = NULL;
		}

		/* Examine call for special procedure */
		if (!IS_SET(ch->act, ACT_RUNNING) && ch->spec_fun)
		{
			if ((*ch->spec_fun)(ch))
				continue;
			if (char_died(ch))
				continue;
		}
		if (!IS_SET(ch->act, ACT_RUNNING) && ch->spec_2)
		{
			if ((*ch->spec_2)(ch))
				continue;
			if (char_died(ch))
				continue;
		}

		if (ch != cur_char)
		{
			bug("ch != cur_char after spec_fun", 0);
			continue;
		}

		/* That's all for sleeping / busy monster */
		if (ch->position != POS_STANDING)
			continue;

		if (IS_SET(ch->act, ACT_MOUNTED))
		{
			if (IS_SET(ch->act, ACT_AGGRESSIVE))
				do_emote(ch, (char*) "szarpie si� i warczy.");
			continue;
		}

		if ( IS_SET(ch->in_room->room_flags,
				ROOM_SAFE) && IS_SET(ch->act, ACT_AGGRESSIVE))
			do_emote(ch, (char*) "szarpie si� i szamocze.");

		/* MOBprogram random trigger */
		if (ch->in_room->area->nplayer > 0)
		{
			mprog_random_trigger(ch);
			if (char_died(ch))
				continue;
			if (ch->position < POS_STANDING)
				continue;
		}

		if (char_died(ch))
			continue;

		/* MOBprogram hour trigger: do something for an hour */
		mprog_hour_trigger(ch);

		if (char_died(ch))
			continue;

		rprog_hour_trigger(ch);

		if (char_died(ch))
			continue;

		if (ch->position < POS_STANDING)
			continue;

		/* Scavenge */
		if ( IS_SET(ch->act, ACT_SCAVENGER) && !ch->in_room->contents.empty() && number_bits(2) == 0)
		{
			OBJ_DATA *obj_best;
			int max;

			max = 1;
			obj_best = NULL;
			for (auto* obj : ch->in_room->contents)
			{
				if ( CAN_WEAR(obj, ITEM_TAKE) && obj->cost > max && !IS_OBJ_STAT(obj, ITEM_BURRIED))
				{
					obj_best = obj;
					max = obj->cost;
				}
			}

			if (obj_best)
			{
				obj_from_room(obj_best);
				obj_to_char(obj_best, ch);
				act( COL_ACTION, "$n podnosi $p$3.", ch, obj_best, NULL,
				TO_ROOM);
			}
		}

		/*
		 * Tutaj pies jest pogrzebany:
		 * Mobki �a��c u�ywaj� f-cji generate_exit wywolywanej w
		 * move_char() i w ten spos�b sprawiaj�, �e mud alokuje sobie
		 * pami�� w�a�ciwie w niesko�czono�� -> trza by j� potem zwolnic
		 * jako� nie? 	--Thanos
		 */
		/* Wander */
		if (!IS_SET(ch->act, ACT_RUNNING) && !IS_SET(ch->act, ACT_SENTINEL) && !IS_SET(ch->act, ACT_PROTOTYPE)
				&& (door = number_bits(5)) <= 9 && (pexit = get_exit(ch->in_room, door)) != NULL && pexit->to_room
				&& !IS_SET(pexit->flags, EX_CLOSED) && !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
				&& (!IS_SET(ch->act, ACT_STAY_AREA) || pexit->to_room->area == ch->in_room->area))
		{
			/* o prosz�: */
			retcode = move_char(ch, pexit, 0);
			//	If ch changes position due
			//	to it's or someother mob's
			//	movement via MOBProgs,
			//	continue - Kahn
			if (char_died(ch))
				continue;
			if (retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL) || ch->position < POS_STANDING)
				continue;
		}

		/* Flee */
		if (ch->hit < ch->max_hit / 2&& ( door = number_bits( 4 ) ) <= 9
		&& ( pexit = get_exit(ch->in_room,door) ) != NULL
		&& pexit->to_room
		&& !IS_SET(pexit->flags, EX_CLOSED)
		&& !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))
		{
			CHAR_DATA *rch;
			bool found;
			found = false;
			for (auto* rch : ch->in_room->people)
			{
				if (is_fearing(ch, rch))
				{
					switch (number_bits(2))
					{
					default:
						sprintf(buf, "Mo�e si� dogadamy jako� %s?", PERS(rch, ch, 0));
						break;
					case 0:
						sprintf(buf, "Spadaj ode mnie %s!", PERS(rch, ch, 0));
						break;
					case 1:
						sprintf(buf, "Zostaw mnie w spokoju %s!", PERS(rch, ch, 0));
						break;
					case 2:
						sprintf(buf, "%s chce mnie zabi�! Pomocy!", PERS(rch, ch, 0));
						break;
					case 3:
						sprintf(buf, "Niech mnie kto� uratuje od %s!", PERS(rch, ch, 0));
						break;
					}
					do_yell(ch, buf);
					found = true;
					break;
				}
			}
			if (found)
				retcode = move_char(ch, pexit, 0);
		}
	}
	return;
}

void update_mining()
{
	PLANET_DATA *pPlanet;
	DEPOSIT_DATA *pDeposit;
	MATERIAL_DATA *pMat;

	for (auto* pPlanet : planet_list)
	{
		if (!pPlanet->deposits.empty())
		{
			for (auto* pDeposit : pPlanet->deposits)
			{
				if ((pMat = get_material(pDeposit->material_name, U_ORE_FORM)) == NULL)
				{
					bug("%s -  deposit material doesnt exist", pDeposit->material_name);
					continue;
				}
				add_material_warehouse(pPlanet, pMat, pDeposit->daily_mining / 24);
			}
			save_planet(pPlanet);
		}
	}
}

void update_taxes(void)
{
	PLANET_DATA *planet;
	CLAN_DATA *clan;

	for (auto* planet : planet_list)
	{
		if ((clan = planet->governed_by))
		{
			CLAN_DATA *suborg = NULL;
			int sCount = 0;

			/* Trog: tu raczej trzeba dorobic, ze org czerpie kase z suborg */
			if (!clan->suborgs.empty())
			{
				sCount = clan->suborgs.size();

				for (auto* suborg : clan->suborgs)
				{
					suborg->funds += get_taxes(planet) / 840 / sCount;
					save_clan(suborg);
				}

				clan->funds += get_taxes(planet) / 840;
				save_clan(clan);
			}
			else
			{
				clan->funds += get_taxes(planet) / 420;
				save_clan(clan);
			}
			save_planet(planet);
		}
	}
}

/*
 * Update the weather.
 */
void weather_update(void)
{
	DESCRIPTOR_DATA *d;
	PLANET_DATA *pPlanet;
	const char *AT_TEMP = PLAIN;
	char buf[MAX_STRING_LENGTH];
	int diff;

	for (auto* pPlanet : planet_list)
	{
		SEASON_DATA *pSeason = pPlanet->curr_season;
		int night = pPlanet->curr_season->night_length;
		int day = pPlanet->curr_season->day_length;
		int hour = (++pPlanet->hour);
		int tempdiff = pPlanet->curr_season->diff_temperature;
		int mon;

		buf[0] = '\0';

		if (hour == night / 2) /* np. 6 dla 24 godz. doby */
		{
			pPlanet->sunlight = SUN_RISE;
			strcat(buf, "S�o�ce wschodzi powoli.");
			AT_TEMP = FG_YELLOW;
			pPlanet->temperature += number_range(1, tempdiff / 3);
		}
		else if (hour == night / 2 + night / 4) /* np. 9 dla 24 godz. doby */
		{
			pPlanet->sunlight = SUN_LIGHT;
			strcat(buf, "Dzie� si� rozpocz��.");
			AT_TEMP = FG_CYAN;

			pPlanet->temperature += number_range(1, tempdiff / 2);

			mon = 0;
			/* w ostatni miesi�c pory zaczynamy zbli�a� si�
			 * temperaturowo do nast�pnej */
			for (auto* seas : pPlanet->seasons)
			{
				mon += seas->length;
				if (seas == pPlanet->curr_season)
					break;
			}

			/* find next season after pSeason */
			SEASON_DATA *nextSeason = NULL;
			{
				auto sit = std::find(pPlanet->seasons.begin(), pPlanet->seasons.end(), pSeason);
				if (sit != pPlanet->seasons.end() && ++sit != pPlanet->seasons.end())
					nextSeason = *sit;
			}

			if ((nextSeason && pSeason->temperature != nextSeason->temperature))
			{
				if (pPlanet->month == mon)
				{
					if (pPlanet->temperature > nextSeason->temperature)
						pPlanet->temperature--;
					else
						pPlanet->temperature++;
				}
			}
			else
			{
				if (pPlanet->month == mon)
				{
					if (pPlanet->temperature > pPlanet->seasons.front()->temperature)
						pPlanet->temperature--;
					else
						pPlanet->temperature++;
				}
			}
		}
		else if (hour == (night + day) / 2) /* np. 12 dla 24 godz. doby */
		{
			pPlanet->sunlight = SUN_LIGHT;
			strcat(buf, "Jest po�udnie.");
			AT_TEMP = FB_YELLOW;
			pPlanet->temperature += number_range(1, tempdiff / 3);
		}
		else if (hour == (night / 2 + day)) /* np. 18 dla 24 godz. doby */
		{
			pPlanet->sunlight = SUN_SET;
			strcat(buf, "S�o�ce powoli chyli si� ku zachodowi.");
			AT_TEMP = FG_RED;
			pPlanet->temperature -= number_range(1, tempdiff / 3);
		}
		else if (hour == night / 2 + day + night / 3)/* np. 22 dla 24 godz. doby */
		{
			pPlanet->sunlight = SUN_DARK;
			strcat(buf, "Zapada noc.");
			AT_TEMP = FG_GREEN;
			pPlanet->temperature -= number_range(1, tempdiff / 2);
		}
		else if (hour >= night + day)
		{
			pPlanet->hour = 0;
			pPlanet->day++;
		}

		if (pPlanet->day >= pPlanet->monthlen)
		{
			pPlanet->day = 1;
			pPlanet->month++;
		}

		mon = 0;/* Liczymy czy up�yn�a pora roku */
		for (auto* seas : pPlanet->seasons)
		{
			mon += seas->length;
			if (seas == pPlanet->curr_season)
				break;
		}

		if (pPlanet->month >= mon)
		{
			/* advance to next season or wrap around */
			auto sit = std::find(pPlanet->seasons.begin(), pPlanet->seasons.end(), pPlanet->curr_season);
			if (sit != pPlanet->seasons.end() && ++sit != pPlanet->seasons.end())
				pPlanet->curr_season = *sit;
			else
			{ /* reset ca�ego roku */
				pPlanet->curr_season = pPlanet->seasons.front();
				pPlanet->month = 1;
			}
		}

		mon = 0;/* Liczymy czy up�yn�� rok (na wszelki) */
		for (auto* seas : pPlanet->seasons)
			mon += seas->length;

		if (pPlanet->month >= mon)
			pPlanet->month = 1;

		if (buf[0] != '\0')
		{
			strcat(buf, PLAIN);
			for (auto* d : descriptor_list)
			{
				if (d->connected == CON_PLAYING && IS_OUTSIDE(d->character) && IS_AWAKE(d->character) && d->character->in_room->area->planet
						&& d->character->in_room->area->planet == pPlanet)
					act(AT_TEMP, buf, d->character, 0, 0, TO_CHAR);
			}
		}

		/*
		 * Weather change.
		 */
		buf[0] = '\0';
		pPlanet->winddir += number_range(0, 2) - 1;

		diff = (pPlanet->mmhg > pPlanet->curr_season->mmhg) ? -2 : 2;

		pPlanet->change += diff * dice(1, 4) + dice(2, 6) - dice(2, 6);
		pPlanet->change = UMAX(pPlanet->change, -12);
		pPlanet->change = UMIN(pPlanet->change, 12);

		pPlanet->mmhg += pPlanet->change;
		pPlanet->mmhg = UMAX(pPlanet->mmhg, pPlanet->curr_season->mmhg - pPlanet->curr_season->diff_mmhg);
		pPlanet->mmhg = UMIN(pPlanet->mmhg, pPlanet->curr_season->mmhg + pPlanet->curr_season->diff_mmhg);

		AT_TEMP = PLAIN;
		switch (pPlanet->sky)
		{
		default:
			bug("bad sky %d.", pPlanet->sky);
			pPlanet->sky = SKY_CLOUDLESS;
			break;

		case SKY_CLOUDLESS:
			if (pPlanet->mmhg < 990 || (pPlanet->mmhg < 1010 && number_bits(2) == 0))
			{
				if (pPlanet->temperature <= 4)
					strcat(buf, "Zaczyna pada� drobny �nieg.");
				else
					strcat(buf, "Niebo zaczyna si� chmurzy�.");
				AT_TEMP = PLAIN;
				pPlanet->temperature -= 2;
				pPlanet->sky = SKY_CLOUDY;
				pPlanet->windspeed += 10;
			}
			break;

		case SKY_CLOUDY:
			if (pPlanet->mmhg < 970 || (pPlanet->mmhg < 990 && number_bits(2) == 0))
			{
				if (pPlanet->temperature <= 4)
					strcat(buf, "Zaczyna sypa� �nieg.");
				else
					strcat(buf, "Zaczyna pada� deszcz.");
				pPlanet->temperature -= 2;
				pPlanet->sky = SKY_RAINING;
				pPlanet->windspeed += 10;
				AT_TEMP = FB_BLUE;
			}
			if (pPlanet->mmhg > 1030 && number_bits(2) == 0)
			{
				if (pPlanet->temperature <= 5)
					strcat(buf, "�nieg przestaje sypa�.");
				else
					strcat(buf, "Chmury rozwiewaj� si�.");
				pPlanet->temperature += 2;
				pPlanet->sky = SKY_CLOUDLESS;
				pPlanet->windspeed -= 10;
				AT_TEMP = FB_WHITE;
			}
			break;

		case SKY_RAINING:
			if (pPlanet->mmhg < 970 && number_bits(2) == 0)
			{
				if (pPlanet->temperature <= 4)
					strcat(buf, "S�yszysz odg�osy burzy �nie�nej w oddali.");
				else
					strcat(buf, "B�yskawica przeszywa niebo.");
				pPlanet->temperature -= 3;
				pPlanet->sky = SKY_LIGHTNING;
				pPlanet->windspeed += 10;
				AT_TEMP = FB_YELLOW;
			}

			if (pPlanet->mmhg > 1030 || (pPlanet->mmhg > 1010 && number_bits(2) == 0))
			{
				if (pPlanet->temperature <= 4)
					strcat(buf, "�nieg przestaje sypa�.");
				else
					strcat(buf, "Przesta�o pada�.");
				pPlanet->temperature += 6;
				pPlanet->sky = SKY_CLOUDY;
				pPlanet->windspeed -= 10;
				AT_TEMP = FB_WHITE;
			}
			break;

		case SKY_LIGHTNING:
			if (pPlanet->mmhg > 1010 || (pPlanet->mmhg > 990 && number_bits(2) == 0))
			{
				if (pPlanet->temperature <= 4)
					strcat(buf, "Burza oddala si�.");
				else
					strcat(buf, "B�yskawice ustaj�.");
				pPlanet->temperature += 2;
				pPlanet->sky = SKY_RAINING;
				pPlanet->windspeed -= 10;
				AT_TEMP = PLAIN;
				break;
			}
			break;
		}

		pPlanet->temperature = UMAX(pPlanet->temperature, pPlanet->curr_season->temperature - pPlanet->curr_season->diff_temperature);
		pPlanet->temperature = UMIN(pPlanet->temperature, pPlanet->curr_season->temperature + pPlanet->curr_season->diff_temperature);

		if (buf[0] != '\0')
		{
			strcat(buf, PLAIN);
			for (auto* d : descriptor_list)
			{
				if (d->connected == CON_PLAYING && IS_OUTSIDE(d->character) && IS_AWAKE(d->character) && d->character->in_room->area->planet
						&& d->character->in_room->area->planet == pPlanet)
					act(AT_TEMP, buf, d->character, 0, 0, TO_CHAR);
			}
		}
	}
	return;
}

/*
 * Bacta update from Fall Of The Empire (fote.dune.net 6666)
 * 						-- copied by Thanos
 */
void bacta_update()
{
	CHAR_DATA *ch;
	auto bacta_snapshot = char_list;
	for (auto it = bacta_snapshot.rbegin(); it != bacta_snapshot.rend(); ++it)
	{
		ch = *it;
		set_cur_char(ch);

		if (char_died(ch))
			continue;

		if (IS_SET(ch->in_room->room_flags, ROOM_BACTA))
		{
			if (ch->hit < ch->max_hit)
			{

				ch->hit += abs(ch->hit * 40 / 100);
				ch->hit += number_range(0, 3);
				if (ch->hit > ch->max_hit)
					ch->hit = ch->max_hit;
				ch_printf(ch, "Czujesz, �e twoje rany goj� si� du�o "
						"szybciej dzi�ki aromatowi bacty." NL);
			}

			if (ch->move < ch->max_move)
			{
				ch->move += abs(ch->move * 40 / 100);
				ch->move += number_range(0, 3);
				if (ch->move > ch->max_move)
					ch->move = ch->max_move;
				ch_printf(ch, "Aromat bacty dzia�a na ciebie koj�co,"
						" czujesz jak zm�czenie odchodzi." NL);
			}
		}
	}
	return;
}

/*
 * Update all chars, including mobs.
 * This function is performance sensitive.
 */
void char_update(void)
{
	CHAR_DATA *ch;
	CHAR_DATA *ch_save;
	int save_count = 0;
	int break_control = 0;

	ch_save = NULL;
	auto charupd_snapshot = char_list;
	for (auto it = charupd_snapshot.rbegin(); it != charupd_snapshot.rend(); ++it)
	{
		ch = *it;
		set_cur_char(ch);

		if (char_died(ch))
			continue;

		/*
		 *  Do a room_prog rand check right off the bat
		 *   if ch disappears (rprog might wax npc's), continue
		 */
		if (!IS_NPC(ch))
			rprog_random_trigger(ch);
		if (char_died(ch))
			continue;
		if (IS_NPC(ch))
			mprog_time_trigger(ch);
		if (char_died(ch))
			continue;

		rprog_time_trigger(ch);
		if (char_died(ch))
			continue;

		/*
		 * See if player should be auto-saved.
		 */
		if (!IS_NPC(ch) && current_time - ch->save_time > (sysdata.save_frequency * 60))
			ch_save = ch;
		else
			ch_save = NULL;

		if (ch->position >= POS_STUNNED)
		{
			if (ch->hit < ch->max_hit)
				ch->hit += hit_gain(ch);
			if (ch->mana < ch->max_mana || ch->skill_level[FORCE_ABILITY] == 1)
				ch->mana += mana_gain(ch);
			if (ch->move < ch->max_move)
				ch->move += move_gain(ch);
		}

		if (ch->position == POS_STUNNED)
			update_pos(ch);

		if (ch->pcdata)
			gain_addiction(ch);

		if ( IS_NPC(ch) //by Trog - do dominacji.
		&& IS_AFFECTED(ch, AFF_DOMINATED) && ch->master && ch->master->in_room == ch->in_room)
		{
			break_control = (int) (ch->master->pcdata->learned[gsn_dominate]);
			if ((get_curr_int(ch) + 5 > get_curr_int(ch->master)) || (get_curr_cha(ch) + 6 > get_curr_cha(ch->master))
					|| (break_control < number_percent()))
			{
				affect_strip(ch, gsn_dominate);
				stop_follower(ch);
				send_to_char("Czujesz si� jako� swobodniej." NL, ch);
			}
		}

		if (!IS_NPC(ch) && ch->top_level < LEVEL_IMMORTAL)
		{
			OBJ_DATA *obj;
			if ((obj = get_eq_char(ch, WEAR_LIGHT)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] > 0)
			{
				if (--obj->value[2] == 0 && ch->in_room)
				{
					ch->in_room->light -= obj->count;
					act( COL_ACTION, "$p ga�nie.", ch, obj, NULL, TO_ROOM);
					act( COL_ACTION, "$p ga�nie.", ch, obj, NULL, TO_CHAR);
					if (obj->serial == cur_obj)
						global_objcode = rOBJ_EXPIRED;
					extract_obj(obj);
				}
			}

			if (ch->pcdata->condition[COND_DRUNK] > 8)
				worsen_mental_state(ch, ch->pcdata->condition[COND_DRUNK] / 8);

			if (ch->pcdata->condition[COND_FULL] > 1)
				switch (ch->position)
				{
				case POS_SLEEPING:
					better_mental_state(ch, 4);
					break;
				case POS_RESTING:
					better_mental_state(ch, 3);
					break;
				case POS_SITTING:
				case POS_MOUNTED:
					better_mental_state(ch, 2);
					break;
				case POS_STANDING:
					better_mental_state(ch, 1);
					break;
				case POS_FIGHTING:
					if (number_bits(2) == 0)
						better_mental_state(ch, 1);
					break;
				}
			if (ch->pcdata->condition[COND_THIRST] > 1)
				switch (ch->position)
				{
				case POS_SLEEPING:
					better_mental_state(ch, 5);
					break;
				case POS_RESTING:
					better_mental_state(ch, 3);
					break;
				case POS_SITTING:
				case POS_MOUNTED:
					better_mental_state(ch, 2);
					break;
				case POS_STANDING:
					better_mental_state(ch, 1);
					break;
				case POS_FIGHTING:
					if (number_bits(2) == 0)
						better_mental_state(ch, 1);
					break;
				}
			gain_condition(ch, COND_DRUNK, -1);
			gain_condition(ch, COND_FULL, -1);
			if (ch->in_room)
				switch (ch->in_room->sector_type)
				{
				default:
					gain_condition(ch, COND_THIRST, -1);
					break;
				case SECT_DESERT:
					gain_condition(ch, COND_THIRST, -2);
					break;
				case SECT_UNDERWATER:
				case SECT_OCEANFLOOR:
					if (number_bits(1) == 0)
						gain_condition(ch, COND_THIRST, -1);
					break;
				}
		}

		if (!char_died(ch))
		{
			/*
			 * Careful with the damages here,
			 *   MUST NOT refer to ch after damage taken,
			 *   as it may be lethal damage (on NPC).
			 */
			if (IS_AFFECTED(ch, AFF_POISON))
			{
				act( COL_POISON, "$n trz�sie si� i cierpi.", ch, NULL, NULL,
				TO_ROOM);
				act( COL_POISON, "Trz�siesz si� i cierpisz.", ch, NULL, NULL,
				TO_CHAR);
				ch->mental_state = URANGE(20, ch->mental_state + 4, 100);
				damage(ch, ch, 6, gsn_poison);
			}
			else if (ch->position == POS_INCAP)
				damage(ch, ch, 1, TYPE_UNDEFINED);
			else if (ch->position == POS_MORTAL)
				damage(ch, ch, 4, TYPE_UNDEFINED);

			if (char_died(ch))
				continue;
			if (ch->mental_state >= 30)
				switch ((ch->mental_state + 5) / 10)
				{
				case 3:
					send_to_char("Nienajlepiej si� czujesz." NL, ch);
					act( COL_ACTION, "$n nie wygl�da najlepiej.", ch, NULL, NULL, TO_ROOM);
					break;
				case 4:
					send_to_char("�le si� czujesz." NL, ch);
					act( COL_ACTION, "$n �le si� czuje.", ch, NULL, NULL,
					TO_ROOM);
					break;
				case 5:
					send_to_char("Potrzebujesz pomocy!" NL, ch);
					act( COL_ACTION, "$n wygl�da jakby potrzebowa�$o pomocy.", ch, NULL, NULL, TO_ROOM);
					break;
				case 6:
					send_to_char("Szukaj lepiej lekarza." NL, ch);
					act( COL_ACTION, "Niech kto� wezwie po lekarza dla $n$1.", ch, NULL, NULL, TO_ROOM);
					break;
				case 7:
					send_to_char("Czujesz jak �wiat rozmywa ci si� przed oczyma..." NL, ch);
					act( COL_ACTION, "$n wygl�da jakby traci�$o poczucie rzeczywisto�ci.", ch, NULL, NULL, TO_ROOM);
					break;
				case 8:
					send_to_char("Zaczynasz rozumie�... wszystko." NL, ch);
					act( COL_ACTION, "$n zaczyna zachowywa� si� jak szaleniec!", ch, NULL, NULL, TO_ROOM);
					break;
				case 9:
					send_to_char("W ca�ym wszech�wiecie jeste� tylko TY." NL, ch);
					act( COL_ACTION, "$n be�kocze co� o 'odpowiedzi', 'JEDYNYM' i"
							" jakie� tam mumbo-jumbo...", ch, NULL, NULL, TO_ROOM);
					break;
				case 10:
					send_to_char("Czujesz, �e koniec jest ju� bliski." NL, ch);
					act( COL_ACTION, "$n poci si� strasznie i wije w konwulsjach...", ch, NULL, NULL, TO_ROOM);
					break;
				}

			if (ch->mental_state <= -30)
				switch ((abs(ch->mental_state) + 5) / 10)
				{
				case 10:
					if (ch->position > POS_SLEEPING)
					{
						if ((ch->position == POS_STANDING || ch->position < POS_FIGHTING) && number_percent() + 10 < abs(ch->mental_state))
							do_sleep(ch, (char*) "");
						else
							send_to_char("Ledwie zipiesz." NL, ch);
					}
					break;
				case 9:
					if (ch->position > POS_SLEEPING)
					{
						if ((ch->position == POS_STANDING || ch->position < POS_FIGHTING)
								&& (number_percent() + 20) < abs(ch->mental_state))
							do_sleep(ch, (char*) "");
						else
							send_to_char("Ledwie otwierasz oczy." NL, ch);
					}
					break;
				case 8:
					if (ch->position > POS_SLEEPING)
					{
						if (ch->position < POS_SITTING && (number_percent() + 30) < abs(ch->mental_state))
							do_sleep(ch, (char*) "");
						else
							ch_printf(ch, "Jeste� mocno wyczerpan%s." NL, SEX_SUFFIX_YAE(ch));
					}
					break;
				case 7:
					if (ch->position > POS_RESTING)
						send_to_char("Nie chce ci si� nic robi�." NL, ch);
					break;
				case 6:
					if (ch->position > POS_RESTING)
						ch_printf(ch, "Jeste� bardzo senn%s." NL, SEX_SUFFIX_YAE(ch));
					break;
				case 5:
					if (ch->position > POS_RESTING)
						ch_printf(ch, "Jeste� senn%s." NL, SEX_SUFFIX_YAE(ch));
					break;
				case 4:
					if (ch->position > POS_RESTING)
						ch_printf(ch, "Jeste� zm�czon%s." NL, SEX_SUFFIX_YAE(ch));
					break;
				case 3:
					if (ch->position > POS_RESTING)
						send_to_char("Przyda�by ci si� wypoczynek." NL, ch);
					break;
				}

			if (ch->backup_wait > 0)
			{
				--ch->backup_wait;
				if (ch->backup_wait == 0)
					add_reinforcements(ch);
			}

			if (!IS_NPC(ch))
			{
				if (++ch->timer > 15 && !ch->desc)
				{
					if (ch->in_room)
						char_from_room(ch);
					char_to_room(ch, get_room_index( ROOM_PLUOGUS_QUIT));
					ch->position = POS_RESTING;
					ch->hit = UMAX(1, ch->hit);
					save_char_obj(ch);
					do_quit(ch, (char*) "");
				}
				else if (ch == ch_save && IS_SET(sysdata.save_flags, SV_AUTO) && ++save_count < 10) /* save max of 10 per tick */
					save_char_obj(ch);
			}
		}

		if ( IS_ISOLATED( ch ) && ch->pcdata->release_date < current_time)
		{
			STRDUP(ch->pcdata->isolated_by, "");
			ch->pcdata->release_date = 0;
			char_from_room(ch);
			char_to_room(ch, get_room_index( ROOM_VNUM_TEMPLE));
			save_char_obj(ch);
			send_to_char("Koniec izolacji. Wychodzisz na wolno��." NL, ch);
		}

		/* Trog */
		if ( IS_SILENCED( ch ) && ch->pcdata->unsilence_date < current_time)
		{
			STRDUP(ch->pcdata->silenced_by, "");
			ch->pcdata->unsilence_date = 0;
			save_char_obj(ch);
			send_to_char("Koniec ciszy. Mo�esz ju� rozmawia�." NL, ch);
		}
	}
	return;
}

/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update(void)
{
	OBJ_DATA *obj;
	const char *AT_TEMP;

	auto obj_snapshot = object_list;
	for (auto oit = obj_snapshot.rbegin(); oit != obj_snapshot.rend(); ++oit)
	{
		obj = *oit;
		CHAR_DATA *rch;
		const char *message;

		set_cur_obj(obj);
		if (obj->carried_by)
			oprog_random_trigger(obj);
		else if (obj->in_room && obj->in_room->area->nplayer > 0)
			oprog_random_trigger(obj);

		if (obj_extracted(obj))
			continue;

		if (obj->item_type == ITEM_WEAPON&& obj->carried_by &&
		( obj->wear_loc == WEAR_WIELD || obj->wear_loc == WEAR_DUAL_WIELD ) &&
		obj->value[3] != WEAPON_BLASTER && obj->value[4] > 0 &&
		obj->value[3] != WEAPON_BOWCASTER && obj->value[3] != WEAPON_FORCE_PIKE)
		{
			obj->value[4]--;
			if (obj->value[4] <= 0)
			{
				if (obj->value[3] == WEAPON_LIGHTSABER)
				{
					act( PLAIN, "$p zaczyna bucze�, a �wiat�o w nim obumiera.", obj->carried_by, obj, NULL, TO_CHAR);
					act( PLAIN, "$n zaczyna bucze�, a �wiat�o w nim obumiera.", obj->carried_by, NULL, NULL, TO_ROOM);
				}
				else if (obj->value[3] == WEAPON_VIBRO_BLADE)
					act( PLAIN, "$p przestaje wibrowa�.", obj->carried_by, obj, NULL, TO_CHAR);
			}
		}

		if (obj->item_type == ITEM_PIPE)
		{
			if (IS_SET(obj->value[3], PIPE_LIT))
			{
				if (--obj->value[1] <= 0)
				{
					obj->value[1] = 0;
					REMOVE_BIT(obj->value[3], PIPE_LIT);
				}
				else if (IS_SET(obj->value[3], PIPE_HOT))
					REMOVE_BIT(obj->value[3], PIPE_HOT);
				else
				{
					if (IS_SET(obj->value[3], PIPE_GOINGOUT))
					{
						REMOVE_BIT(obj->value[3], PIPE_LIT);
						REMOVE_BIT(obj->value[3], PIPE_GOINGOUT);
					}
					else
						SET_BIT(obj->value[3], PIPE_GOINGOUT);
				}
				if (!IS_SET(obj->value[3], PIPE_LIT))
					SET_BIT(obj->value[3], PIPE_FULLOFASH);
			}
			else
				REMOVE_BIT(obj->value[3], PIPE_HOT);
		}

		/* Corpse decay (npc corpses decay at 8 times the rate of pc corpses) - Narn */

		if (obj->item_type == ITEM_CORPSE_PC || obj->item_type == ITEM_CORPSE_NPC || obj->item_type == ITEM_DROID_CORPSE)
		{
			int timerfrac = UMAX(1, obj->timer - 1);
			if (obj->item_type == ITEM_CORPSE_PC)
				timerfrac = (int) (obj->timer / 8 + 1);

			if (obj->timer > 0 && obj->value[2] > timerfrac)
			{
				char buf[MAX_STRING_LENGTH];
				char name[MAX_STRING_LENGTH];
				char *bufptr;

				bufptr = one_argument(obj->przypadki[1], name);

				separate_obj(obj);
				obj->value[2] = timerfrac;
				if (obj->item_type == ITEM_DROID_CORPSE)
				{
					/* Pogruchotane cz�ci %s rdzewiej� tutaj. */
					bufptr = one_argument(bufptr, name);
					sprintf(buf, d_corpse_descs[UMIN(timerfrac - 1, 4)], bufptr);
				}
				else
				{
					/* Cia�o %s le�y tutaj. */
					sprintf(buf, corpse_descs[UMIN(timerfrac - 1, 4)], capitalize(bufptr));
				}

				STRDUP(obj->description, buf);
			}
		}

		/* don't let inventory decay */
		if (IS_OBJ_STAT(obj, ITEM_INVENTORY))
			continue;

		if (obj->timer > 0 && obj->timer < 5 && obj->item_type == ITEM_ARMOR)
			if (obj->carried_by)
			{
				act( COL_ACTION, "$p jest niemal bezu�yteczne.", obj->carried_by, obj, NULL, TO_CHAR);
			}

		if ((obj->timer <= 0 || --obj->timer > 0))
			continue;

		/* if we get this far, object's timer has expired. */

		AT_TEMP = PLAIN;
		switch (obj->item_type)
		{
		default:
			message = "$p zu�ywa si�.";
			AT_TEMP = PLAIN;
			break;
		case ITEM_GRENADE:
			explode(obj);
			return;
// 				break;
		case ITEM_PORTAL:
			message = "$p zamyka si� i znika.";
			remove_portal(obj);
			obj->item_type = ITEM_TRASH; /* so extract_obj	 */
			AT_TEMP = COL_FORCE; /* doesn't remove_portal */
			break;
		case ITEM_FOUNTAIN:
			message = "$p wysycha.";
			AT_TEMP = FB_BLUE;
			break;
		case ITEM_CORPSE_NPC:
			message = "$p zamienia si� w kupk� prochu i znika.";
			AT_TEMP = COL_OBJECT;
			break;
		case ITEM_DROID_CORPSE:
			message = "$p rdzewieje na drobny py�.";
			AT_TEMP = COL_OBJECT;
			break;
		case ITEM_CORPSE_PC:
			message = "$p zamienia si� w kupk� prochu i znika...";
			AT_TEMP = COL_FORCE;
			break;
		case ITEM_FOOD:
			message = "Gromada robak�w ob�azi $p$3.";
			AT_TEMP = COL_HUNGRY;
			break;
		case ITEM_BLOODSTAIN:
			message = "$p wysycha i znika.";
			AT_TEMP = FG_RED;
			break;
		case ITEM_SCRAPS:
			message = "$p rozpadaj� si� i znikaj�.";
			AT_TEMP = COL_OBJECT;
			break;
		}

		if (obj->carried_by)
			act(AT_TEMP, message, obj->carried_by, obj, NULL, TO_CHAR);
		else if (obj->in_room && !obj->in_room->people.empty() && (rch = obj->in_room->people.front()) != NULL && !IS_OBJ_STAT(obj, ITEM_BURRIED))
		{
			act(AT_TEMP, message, rch, obj, NULL, TO_ROOM);
			act(AT_TEMP, message, rch, obj, NULL, TO_CHAR);
		}

		if (obj->serial == cur_obj)
			global_objcode = rOBJ_EXPIRED;
		extract_obj(obj);
	}
	return;
}

/*
 * Function to check important stuff happening to a player
 * This function should take about 5% of mud cpu time
 */
void char_check(void)
{
	EXIT_DATA *pexit;
	static int cnt = 0;
	int door, retcode;

	cnt = (cnt + 1) % 2;

	auto charchk_snapshot = char_list;
	for (auto* ch : charchk_snapshot)
	{
		set_cur_char(ch);
		will_fall(ch, 0);

		if (char_died(ch))
			continue;

		if (IS_NPC(ch))
		{
			if (cnt != 0)
				continue;

			/* running mobs	-Thoric */
			if (IS_SET(ch->act, ACT_RUNNING))
			{
				if (!IS_SET(ch->act, ACT_SENTINEL) && !ch->fighting && ch->hunting)
				{
					WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
					hunt_victim(ch);
					continue;
				}

				if (ch->spec_fun)
				{
					if ((*ch->spec_fun)(ch))
						continue;
					if (char_died(ch))
						continue;
				}
				if (ch->spec_2)
				{
					if ((*ch->spec_2)(ch))
						continue;
					if (char_died(ch))
						continue;
				}

				if (!IS_SET(ch->act, ACT_SENTINEL) && !IS_SET(ch->act, ACT_PROTOTYPE) && (door = number_bits(4)) <= 9
						&& (pexit = get_exit(ch->in_room, door)) != NULL && pexit->to_room && !IS_SET(pexit->flags, EX_CLOSED)
						&& !IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)
						&& (!IS_SET(ch->act, ACT_STAY_AREA) || pexit->to_room->area == ch->in_room->area))
				{
					retcode = move_char(ch, pexit, 0);
					if (char_died(ch))
						continue;
					if (retcode != rNONE || IS_SET(ch->act, ACT_SENTINEL) || ch->position < POS_STANDING)
						continue;
				}
			}
			continue;
		}
		else
		{
			if (ch->mount && ch->in_room != ch->mount->in_room)
			{
				REMOVE_BIT(ch->mount->act, ACT_MOUNTED);
				ch->mount = NULL;
				ch->position = POS_STANDING;
				send_to_char("Spadasz z wierzchowca na...ty�ek." NL "OUCH!" NL, ch);
				fevent_trigger(ch, FE_POSITION);
			}

			if ((ch->in_room && ch->in_room->sector_type == SECT_UNDERWATER)
					|| (ch->in_room && ch->in_room->sector_type == SECT_OCEANFLOOR))
			{
				if (!IS_AFFECTED(ch, AFF_AQUA_BREATH))
				{
					if (get_trust(ch) < LEVEL_IMMORTAL)
					{
						int dam;
						dam = number_range(ch->max_hit / 50, ch->max_hit / 30);
						dam = UMAX(1, dam);
						if (ch->hit <= 0)
							dam = UMIN(10, dam);
						if (number_bits(3) == 0)
							send_to_char("D�awisz si� i krztusisz pr�buj�c oddycha� WOD�!" NL, ch);
						damage(ch, ch, dam, TYPE_UNDEFINED);
					}
				}
			}

			if (char_died(ch))
				continue;

			if (ch->in_room && ((ch->in_room->sector_type == SECT_WATER_NOSWIM) || (ch->in_room->sector_type == SECT_WATER_SWIM)))
			{
				if (!IS_AFFECTED(ch, AFF_FLYING) && !IS_AFFECTED(ch, AFF_FLOATING) && !IS_AFFECTED(ch, AFF_AQUA_BREATH) && !ch->mount)
				{
					if (get_trust(ch) < LEVEL_IMMORTAL)
					{
						int dam;

						if (ch->move > 0)
							ch->move--;
						else
						{
							dam = number_range(ch->max_hit / 50, ch->max_hit / 30);
							dam = UMAX(1, dam);
							if (ch->hit <= 0)
								dam = UMIN(10, dam);
							if (number_bits(3) == 0)
								send_to_char("Ledwie unikaj�c �mierci, krztusisz si� wod�." NL, ch);
							damage(ch, ch, dam, TYPE_UNDEFINED);
						}
					}
				}
			}
		}
	}
}

/*
 * Aggress.
 *
 * for each descriptor
 *     for each mob in room
 *         aggress on some random PC
 *
 * This function should take 5% to 10% of ALL mud cpu time.
 * Unfortunately, checking on each PC move is too tricky,
 *   because we don't the mob to just attack the first PC
 *   who leads the party into the room.
 *
 */
void aggr_update(void)
{
	CHAR_DATA *wch;
	CHAR_DATA *victim;
	struct act_prog_data *apdtmp;

#ifdef UNDEFD
	/*
	*  GRUNT!  To do
	*
	*/
	if ( IS_NPC( wch ) && wch->mpactnum > 0
	&& wch->in_room->area->nplayer > 0 )
	{
		MPROG_ACT_LIST * tmp_act, *tmp2_act;
		for ( tmp_act = wch->mpact; tmp_act;
			tmp_act = tmp_act->next )
		{
			oprog_wordlist_check( tmp_act->buf,wch, tmp_act->ch,
				tmp_act->obj, tmp_act->vo, ACT_PROG );
			STRDUP( tmp_act->buf, "" );
		}
		for ( tmp_act = wch->mpact; tmp_act; tmp_act = tmp2_act )
		{
			tmp2_act = tmp_act->next;
			free_mpact( tmp_act );
		}
		wch->mpactnum = 0;
		wch->mpact    = NULL;
	}
#endif

	/* check mobprog act queue */
	while ((apdtmp = mob_act_list) != NULL)
	{
		wch = (CD*) mob_act_list->vo;
		if (!char_died(wch) && wch->mpactnum > 0)
		{
			MPROG_ACT_LIST *tmp_act;

			while ((tmp_act = wch->mpact) != NULL)
			{
				if (tmp_act->obj && obj_extracted(tmp_act->obj))
					tmp_act->obj = NULL;
				if (tmp_act->ch && !char_died(tmp_act->ch))
					mprog_wordlist_check(tmp_act->buf, wch, tmp_act->ch, tmp_act->obj, tmp_act->vo, ACT_PROG);
				wch->mpact = tmp_act->next;
				free_mpact(tmp_act);
			}
			wch->mpactnum = 0;
			wch->mpact = NULL;
		}
		mob_act_list = apdtmp->next;
		DISPOSE(apdtmp);
	}

	/*
	 * Just check descriptors here for victims to aggressive mobs
	 * We can check for linkdead victims to mobile_update	-Thoric
	 */
	auto desc_snapshot = descriptor_list;
	for (auto* d : desc_snapshot)
	{
		if (d->connected != CON_PLAYING || (wch = d->character) == NULL)
			continue;

		if (char_died(wch) || IS_NPC(wch) || wch->top_level >= LEVEL_IMMORTAL || !wch->in_room)
			continue;

		auto people_snapshot = wch->in_room->people;
		for (auto* ch : people_snapshot)
		{

			if (!IS_NPC(ch) || ch->fighting || IS_AFFECTED(ch, AFF_CHARM) || !IS_AWAKE(ch) || (IS_SET(ch->act, ACT_WIMPY))
					|| !can_see(ch, wch))
				continue;

			if (is_hating(ch, wch))
			{
				found_prey(ch, wch);
				continue;
			}

			if (!IS_SET(ch->act, ACT_AGGRESSIVE) || IS_SET(ch->act, ACT_MOUNTED) || IS_SET(ch->in_room->room_flags, ROOM_SAFE))
				continue;

			victim = wch;

			IF_BUG(victim == NULL, "")
				continue;

			if (get_timer(victim, TIMER_RECENTFIGHT) > 0)
				continue;

			if ( IS_NPC(ch) && IS_SET(ch->attacks, ATCK_BACKSTAB))
			{
				OBJ_DATA *obj;

				if (!ch->mount && (obj = get_eq_char(ch, WEAR_WIELD)) != NULL && obj->value[3] == 11 && !victim->fighting
						&& victim->hit >= victim->max_hit)
				{
					WAIT_STATE(ch, skill_table[gsn_backstab]->beats);
					if (!IS_AWAKE(victim) || number_percent() + 5 < ch->top_level)
					{
						global_retcode = multi_hit(ch, victim, gsn_backstab);
						continue;
					}
					else
					{
						global_retcode = damage(ch, victim, 0, gsn_backstab);
						continue;
					}
				}
			}
			global_retcode = multi_hit(ch, victim, TYPE_UNDEFINED);
		}
	}
	return;
}

/* From interp.c */
/*
 * drunk randoms	- Tricops
 * (Made part of mobile_update	-Thoric)
 */
void drunk_randoms(CHAR_DATA *ch)
{
	CHAR_DATA *rvch = NULL;
	int drunk;
	int position;

	if ( IS_NPC( ch ) || ch->pcdata->condition[COND_DRUNK] <= 0)
		return;

	if (number_percent() < 30)
		return;

	drunk = ch->pcdata->condition[COND_DRUNK];
	position = ch->position;
	ch->position = POS_STANDING;

	if (number_percent() < (2 * drunk / 20))
		check_social(ch, "burp", "");
	else if (number_percent() < (2 * drunk / 20))
		check_social(ch, "hiccup", "");
	else if (number_percent() < (2 * drunk / 20))
		check_social(ch, "drool", "");
	else if (number_percent() < (2 * drunk / 20))
		check_social(ch, "fart", "");
	else if (drunk > (10 + (get_curr_con(ch) / 5)) && number_percent() < (2 * drunk / 18))
	{
		for (auto* vch : ch->in_room->people)
			if (number_percent() < 10)
				rvch = vch;
		check_social(ch, "puke", (char*) (rvch ? rvch->name : ""));
	}

	ch->position = position;
	return;
}

void halucinations(CHAR_DATA *ch)
{
	if (ch->mental_state >= 30 && number_bits(5 - (ch->mental_state >= 50) - (ch->mental_state >= 75)) == 0)
	{
		const char *t;

		switch (number_range(1, UMIN(20, (ch->mental_state + 5) / 5)))
		{
		default:
		case 1:
			t = "Energia ci� rozpiera. Nie mo�esz usiedzie� w jednym miejscu." NL;
			break;
		case 2:
			t = "�askoczesz wszystkich dooko�a." NL;
			break;
		case 3:
			t = "Czujesz jak dreszcze przechodz� twoje cia�o." NL;
			break;
		case 4:
			t = "Nagle zdajesz sobie spraw�, ze jednak wcale nie jest tak weso�o..." NL;
			break;
		case 5:
			t = "Hej! Ci tam �miej� si� z ciebie" NL;
			break;
		case 6:
			t = "S�yszysz p�acz. To twoja mama..." NL;
			break;
		case 7:
			t = "By�$a� ju� tutaj?" NL " Nie." NL "   Tak!" NL " Nie..." NL;
			break;
		case 8:
			t = "Bolesne wspomnienia z dzieci�stwa migaj� w twojej g�owie" NL;
			break;
		case 9:
			t = "S�yszysz jakby kto� wo�a� twoje imi� z daleka..." NL;
			break;
		case 10:
			t = "G�owa ci p�ka, nie mo�esz si� na niczym skupi�.";
			break;
		case 11:
			t = "Ziemia... Ona chyba si� rozst�puje!" NL;
			break;
		case 12:
			t = "Nie bardzo wiesz co jest prawd�, a co nie." NL;
			break;
		case 13:
			t = "To wszystko jest snem. A mo�e nie jest?" NL;
			break;
		case 14:
			t = "Oni nadchodz�. Oni s� coraz bli�ej. Widz� ci�." NL;
			break;
		case 15:
			t = "MOC wype�nia ca�e twoje cia�o!" NL;
			break;
		case 16:
			t = "Jeste� lekk$z jak pi�rko... niebo stoi dla ciebie otworem." NL;
			break;
		case 17:
			t = "Ca�e twoje �ycie mign�o ci przed oczyma. O, a teraz twoja przysz�o��." NL;
			break;
		case 18:
			t = "Jeste� wsz�dzie i wszystkim. Znasz wszystko i wszystkich." NL;
			break;
		case 19:
			t = "Czy jeste� nie�mierteln$y?" NL;
			break;
		case 20:
			t = "Ahhh... W�adza Absolutna... Co zrobi�... Co zrobi�?" NL;
			break;
		}
		act( PLAIN, t, ch, NULL, NULL, TO_CHAR);
	}
	return;
}

void tele_update(void)
{
	if (teleport_list.empty())
		return;

	auto tele_snapshot = teleport_list;
	for (auto* tele : tele_snapshot)
	{
		if (--tele->timer <= 0)
		{
			if (!tele->room->people.empty())
			{
				teleport(tele->room->people.front(), tele->room->tele_vnum,
				TELE_TRANSALL);
			}
			teleport_list.remove(tele);
			DISPOSE(tele);
		}
	}
}

#if 0
/*
 * Write all outstanding authorization requests to Log channel - Gorog
 */
void auth_update( void )
{
	CHAR_DATA *			victim;
	DESCRIPTOR_DATA *	d;
	char 				log_buf[MAX_INPUT_LENGTH];
	bool 				first_time 	= true;
			/* so titles are only done once */

	for (auto* d : descriptor_list)
	{
		victim = d->character;

		if ( victim && IS_WAITING_FOR_AUTH(victim) )
		{
			if ( first_time )
			{
				first_time = false;
				strcpy (log_buf, "Pending authorizations:" );
				to_channel( log_buf, CHANNEL_MONITOR, "Monitor", 101);
			}
			sprintf( log_buf, " %s@%s new %s", victim->name,
			victim->desc->host, victim->race->name);
			to_channel( log_buf, CHANNEL_MONITOR, "Monitor", 101);
		}
	}
}
#endif

void auth_update(void)
{
	CHAR_DATA *victim;
	DESCRIPTOR_DATA *d;
	char buf[MAX_INPUT_LENGTH];
	bool found = false; /* was at least one found? */

#if defined(__FreeBSD__) && __FreeBSD__ == 5
	cpu_usage();
#endif
	for (auto* d : descriptor_list)
	{
		if ((victim = d->character) && IS_WAITING_FOR_AUTH(victim))
		{
			if (!found)
			{
				strcpy(buf, "Pending authorizations:");
				to_channel(buf, CHANNEL_MONITOR, "Monitor", 101);
				found = true;
			}

			sprintf(buf, "\r %s@%s new %s", victim->name, victim->desc->host, victim->race->name);
			to_channel(buf, CHANNEL_MONITOR, "Monitor", 101);
		}
	}
}

void update_informs()
{
	DESCRIPTOR_DATA *d = 0;
	CHAR_DATA *killer = 0;
	INFORM_DATA *inf;
	char buf[MSL];
	bool found = false;

	for (auto* inf : inform_list)
	{
		if (!inf->reported)
		{
			struct tm *start;

			/* Tutaj sprawdzenie przeterminowania */
			start = localtime(&inf->time);
			start->tm_hour += 24;
			start = update_time(start);

			if (mktime(start) < current_time)
			{
				inform_list.remove(inf);
				free_inform(inf);
				save_informs();
				return;
			}
		}
		else
		{
			for (auto* d : descriptor_list)
			{
				if (d->connected > CON_PLAYING)
					continue;

				if (!CH(d) || IS_NPC(CH( d )))
					continue;

				if (!str_cmp( CH( d )->name, inf->attacker))
				{
					killer = CH(d);
					found = true;
					break;
				}
			}
			if (found)
				break;
		}
	}

	if (!killer || !found)
		return;

	inform_list.remove(inf);
	free_inform(inf);
	save_informs();

	// no.. tylko, zeby teraz immosi nie biegali jak debile po mudzie
	// i nie killowali ...   -- Thanos
	if (IS_IMMORTAL(killer))
		return;

	killer = CH(d);

	sprintf(buf, "Ha! %s to paskudny MORDERCA!", killer->name);
	do_chat(Quest_Master, buf);
	do_chat(Quest_Master, (char*) "Nie ma sprawiedliwo�ci na tym mudzie!");
	do_chat(Quest_Master, (char*) "Bracia i Siostry! Do Broni! Zniszczmy ten pomiot!");

	SET_BIT(killer->act, PLR_KILLER);
	sprintf(buf, "Setting KILLER flag to %s", killer->name);
	log_string(buf);
	return;
}

void hotel_safe_update()
{
	DESCRIPTOR_DATA *d;

	for (auto* d : descriptor_list)
	{
		if (d->connected < CON_PLAYING)
			continue;
		if (!d->character || !d->character->pcdata)
			continue;
		if (d->character->pcdata->hotel_safe_time)
			--d->character->pcdata->hotel_safe_time;
	}
}

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler(void)
{
	static int pulse_taxes;
	static int pulse_area;
	static int pulse_mobile;
	static int pulse_bacta;
	static int pulse_violence;
	static int pulse_point;
	static int pulse_second;
	static int pulse_space;
	static int pulse_ship;
	static int pulse_recharge;
	static int pulse_for;
	static int pulse_clan;
	static int pulse_bus;
	struct timeval stime;
	struct timeval etime;

	if (timechar)
	{
		send_to_char("Starting update timer." NL, timechar);
		gettimeofday(&stime, NULL);
	}

	if (--pulse_area <= 0)
	{
		pulse_area = number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2);
		area_update();
		starsystems_update();	// Thanos
	}

	if (--pulse_taxes <= 0)
	{
		pulse_taxes = PULSE_TAXES;
		update_taxes();
		update_mining();
	}

	if (--pulse_mobile <= 0)
	{
		pulse_mobile = PULSE_MOBILE;
		mobile_update();
		scripts_update(); /* Thanos */
		if (!quest_list.empty())
			quest_update(); /* Thanos */
	}

	if (--pulse_bacta <= 0) /* Thanos */
	{
		pulse_bacta = PULSE_BACTA;
		bacta_update();
	}

	if (--pulse_space <= 0)
	{
		pulse_space = PULSE_SPACE;
		update_space();
		update_astro();
		update_traffic();
		recharge_ships(); //Pixel tymczasowo do testow
		update_informs(); /* Thanos, niech tu b�dzie */
	}

	if (--pulse_bus <= 0) /* busy niech lataja WOLNIEJ */
	{
		pulse_bus = PULSE_BUS;
		update_bus();
	}

	if (--pulse_recharge <= 0)
	{
		pulse_recharge = PULSE_SPACE / 3;
		//recharge_ships	( );
	}

	if (--pulse_ship <= 0)
	{
		pulse_ship = PULSE_SPACE / 10;
		move_ships();
	}

	if (--pulse_violence <= 0)
	{
		pulse_violence = PULSE_VIOLENCE;
		violence_update();
	}

	//added by Thanos
	if (--pulse_for <= 0)
	{
		pulse_for = PULSE_FOR / 4;
		update_for();
		update_rat();
	}
	//done

	if (--pulse_point <= 0)
	{
		pulse_point = number_range( PULSE_TICK * 0.75, PULSE_TICK * 1.25);

		show_tick(); /*added by Thanos (tick) 	*/
		auth_update(); /* Gorog 	*/
		weather_update();
		char_update();
		obj_update();
		crime_update(); /* przest�pstwa	-- Thanos 	*/
		//star_update		( );	/* ruchy planet no i narazie..  */
		/* .."stop" dla pustych statkow */
	}

	if (--pulse_second <= 0)
	{
		pulse_second = PULSE_PER_SECOND;
		char_check();
		hotel_safe_update();
		reboot_check(0);
	}

	if (--auction_pulse <= 0)
	{
		auction_pulse = PULSE_AUCTION;
		auction_update();
	}

	if (--pulse_clan <= 0)
	{
		pulse_clan = PULSE_CLAN;
		clan_update();
	}

	update_explosives(); /* by Lomion */
	tele_update();
	aggr_update();
	obj_act_update();
	room_act_update();
	clean_obj_queue(); /* dispose of extracted objects */
	clean_char_queue(); /* dispose of dead mobs/quitting chars */

	if (timechar)
	{
		gettimeofday(&etime, NULL);
		send_to_char("Update timing complete." NL, timechar);
		subtract_times(&etime, &stime);
		ch_printf(timechar, "Timing took %d.%06d seconds." NL, etime.tv_sec, etime.tv_usec);
		timechar = NULL;
	}
	tail_chain();
	return;
}

void remove_portal(OBJ_DATA *portal)
{
	ROOM_INDEX_DATA *fromRoom, *toRoom;
	CHAR_DATA *ch;
	EXIT_DATA *pexit;
	bool found;

	IF_BUG(portal == NULL, "")
		return;

	fromRoom = portal->in_room;
	found = false;

	IF_BUG(portal->in_room == NULL, "")
		return;

	EXIT_DATA *found_exit = NULL;
	for (auto* pexit : fromRoom->exits)
		if (IS_SET(pexit->flags, EX_PORTAL))
		{
			found_exit = pexit;
			found = true;
			break;
		}

	if (!found)
	{
		bug("portal not found in room %d!", fromRoom->vnum);
		return;
	}

	if (found_exit->vdir != DIR_PORTAL)
		bug("exit in dir %d != DIR_PORTAL", found_exit->vdir);

	if ((toRoom = found_exit->to_room) == NULL)
		bug("toRoom is NULL", 0);

	extract_exit(fromRoom, found_exit);
	/* rendunancy */
	/* send a message to fromRoom */
	/* ch = fromRoom->first_person; */
	/* if(ch!=NULL) */
	/* act( PLAIN, "A forceal portal below winks from existence.", ch, NULL, NULL, TO_ROOM ); */

	/* send a message to toRoom */
	if (toRoom && !toRoom->people.empty() && (ch = toRoom->people.front()) != NULL)
		act( PLAIN, "Portal zamyka si� i znika.", ch, NULL, NULL, TO_ROOM);

	/* remove the portal obj: looks better to let update_obj do this */
	/* extract_obj(portal);  */

	return;
}

void reboot_check(time_t reset)
{
	static const char *tmsg[] =
	{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "UWAGA !!! Za 10 sekund nast�pi %s.", "UWAGA !!! Za 30 sekund nast�pi %s.",
			"UWAGA !!! Za 1 minut� nast�pi %s.", "UWAGA !!! Za 2 minuty nast�pi %s.", "UWAGA !!! Za 3 minuty nast�pi %s.",
			"UWAGA !!! Za 4 minuty nast�pi %s.", "UWAGA !!! Za 5 minut nast�pi %s.", "UWAGA !!! Za 10 minut nast�pi %s." };
	static const int times[] =
	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 60, 120, 180, 240, 300, 600 };
	static const int timesize = UMIN(sizeof(times) / sizeof(*times), sizeof(tmsg) / sizeof(*tmsg));
	char buf[MAX_STRING_LENGTH];
	static int trun;
	static bool init;

	if (!init || reset >= current_time)
	{
		for (trun = timesize - 1; trun >= 0; trun--)
			if (reset >= current_time + times[trun])
				break;
		init = true;
		return;
	}

	if ((current_time % 1800) == 0)
	{
		sprintf(buf, "%.24s: %d players", ctime(&current_time), num_descriptors);
		append_to_file(USAGE_FILE, buf);
	}

	/* Ten ifcheck jest g�upi i ma go nie by�     -- Thanos

	 if ( new_boot_time_t - boot_time < 60*60*18
	 &&  !set_boot_time->manual )
	 return;                       */

	if (new_boot_time_t < boot_time)
		return;

	if (new_boot_time_t <= current_time)
	{
		CHAR_DATA *vch;
		extern bool mud_down;
		PLANET_DATA *planet;
		DESCRIPTOR_DATA *d;

		echo_to_all( NL NL
		"Wyczuwasz, �e olbrzymia si�a nie pozwala ci" NL NL
		"dostrzec" NL NL
		"jak odradza si� �ycie �wiata..." NL, ECHOTAR_ALL);

		for (auto* d : descriptor_list)
		{
			CHAR_DATA *ch = CH(d);
			if (!ch)
				continue;
			if (IS_SET(ch->in_room->room_flags, ROOM_PLR_HOME) && ch->plr_home != ch->in_room)
				evacuate_guests(ch->in_room);
		}

		for (auto* vch : char_list)
			if (!IS_NPC(vch))
				save_char_obj(vch);

		/* Added by Thanos */
		sysdata.downtype = sysdata.reboot_type == REB_COPYOVER ? DOWN_COPYOVER : sysdata.reboot_type == REB_SHUTDOWN ? DOWN_SHUTDOWN :
		DOWN_REBOOT;
		save_sysdata(sysdata);

		/* Save time and weather */
		for (auto* planet : planet_list)
			save_planet(planet);

		if (sysdata.reboot_type == REB_COPYOVER)
			copyover();
		else
		{
			if (sysdata.reboot_type == REB_SHUTDOWN)
				append_to_file( SHUTDOWN_FILE, (char*) "[-----] SYSTEM: Shutdown by system.");
			mud_down = true;
		}
		return;
	}

	if (trun != -1 && new_boot_time_t - current_time <= times[trun])
	{
		char msg[MSL];

		sprintf(msg, tmsg[trun],
				sysdata.reboot_type == REB_COPYOVER ? "gor�cy reboot" : sysdata.reboot_type == REB_SHUTDOWN ? "shutdown" : "reboot");
		echo_to_all(msg, ECHOTAR_ALL);
		if (trun <= 15)
			sysdata.DENY_NEW_PLAYERS = true;
		--trun;
		return;
	}
	return;
}

#if 0
void reboot_check( char *arg )
{
	char buf[MAX_STRING_LENGTH];
	extern bool mud_down;
	/*struct tm *timestruct;
	int timecheck;*/
	CHAR_DATA *vch;

	/*Bools to show which pre-boot echoes we've done. */
	static bool thirty	= false;
	static bool fifteen	= false;
	static bool ten		= false;
	static bool five	= false;
	static bool four	= false;
	static bool three	= false;
	static bool two		= false;
	static bool one		= false;

    /* This function can be called by do_setboot when the reboot time
       is being manually set to reset all the bools. */
	if ( !str_cmp( arg, "reset" ) )
	{
		thirty	= false;
		fifteen	= false;
		ten		= false;
		five	= false;
		four	= false;
		three	= false;
		two		= false;
		one		= false;
		return;
	}

	/* If the mud has been up less than 18 hours and the boot time
		wasn't set manually, forget it. */
	/* Usage monitor */

	if ((current_time % 1800) == 0)
	{
		sprintf(buf, "%s: %d players", ctime(&current_time), num_descriptors);
		append_to_file(USAGE_FILE, buf);
	}

	/* Change by Scryn - if mud has not been up 18 hours at boot time - still
	* allow for warnings even if not up 18 hours
 	*/
	if ( new_boot_time_t - boot_time < 60*60*18
		&& set_boot_time->manual == 0 )
		return;
	/*
	timestruct = localtime( &current_time);
	if ( timestruct->tm_hour == set_boot_time->hour
		&& timestruct->tm_min  == set_boot_time->min )*/
	if ( new_boot_time_t <= current_time )
	{
	    echo_to_all(PLAIN, NL NL
		"Wyczuwasz, �e olbrzymia si�a nie pozwala ci" NL NL
		"dostrzec" NL NL
		"jak odradza si� �ycie �wiata..." NL, ECHOTAR_ALL);

		/* Save all characters before booting. */
		for ( auto* vch : char_list )
			if ( !IS_NPC( vch ) )
				save_char_obj( vch );
		mud_down = true;
	}

	/* How many minutes to the scheduled boot? */
	/*  timecheck = ( set_boot_time->hour * 60 + set_boot_time->min )
					- ( timestruct->tm_hour * 60 + timestruct->tm_min );
		if ( timecheck > 30  || timecheck < 0 ) return;

		if ( timecheck <= 1 ) */
	if ( new_boot_time_t - current_time <= 60 )
	{
		if ( one == false )
		{
			sprintf( buf, "Czujesz, �e ziemia pod twoimi nogami trz�sie si�! Koniec jest BLISKI!" );
			echo_to_all( PLAIN, buf, ECHOTAR_ALL );
			one = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 2 )*/
	if ( new_boot_time_t - current_time <= 120 )
	{
		if ( two == false )
		{
			sprintf( buf, "B�yskawice strzelaj� tn�c niebo !!!" );
			echo_to_all( PLAIN, buf, ECHOTAR_ALL );
			two = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 3 )*/
	if (new_boot_time_t - current_time <= 180 )
	{
		if ( three == false )
		{
			sprintf( buf, "S�yszysz, �e zerwa� si� pot�ny wicher. J�czy zag�uszaj�c wszystko!" );
			echo_to_all( PLAIN, buf, ECHOTAR_ALL );
			three = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 4 )*/
	if( new_boot_time_t - current_time <= 240 )
	{
		if ( four == false )
		{
			sprintf( buf, "Niebo przybiera nagle barw� najciemniejszej czerni." );
			echo_to_all( PLAIN, buf, ECHOTAR_ALL );
			four = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 5 )*/
	if( new_boot_time_t - current_time <= 300 )
	{
		if ( five == false )
		{
			sprintf( buf, "Zauwa�asz jak wszystko co �yje doko�a obumiera momentalnie." );
			echo_to_all( PLAIN, buf, ECHOTAR_ALL );
			five = true;
			sysdata.DENY_NEW_PLAYERS = true;
		}
		return;
	}

	/*  if ( timecheck == 10 )*/
	if( new_boot_time_t - current_time <= 600 )
	{
		if ( ten == false )
		{
			sprintf( buf, "S�yszysz jak trzaska l�d na zmro�onych morzach"
			" i oceanach." );
			echo_to_all( PLAIN, buf, ECHOTAR_ALL );
			ten = true;
		}
		return;
	}

	/*  if ( timecheck == 15 )*/
	if( new_boot_time_t - current_time <= 900 )
	{
		if ( fifteen == false )
		{
			sprintf( buf, "�wiat�a doko�a b�yskaj� po to za chwil� porzuci� �wiat"
					" w bezkresnej ciemno�ci." );
			echo_to_all( PLAIN, buf, ECHOTAR_ALL );
			fifteen = true;
		}
		return;
	}

	/*  if ( timecheck == 30 )*/
	if( new_boot_time_t - current_time <= 1800 )
	{
		if ( thirty == false )
		{
		sprintf( buf, "Czujesz jak MOC zmienia �wiat doko�a ciebie." );
		echo_to_all( PLAIN, buf, ECHOTAR_ALL );
		thirty = true;
		}
		return;
	}

	return;
}
#endif

/* the auction update*/

void auction_update(void)
{
	//sprawdzamy czy ktoras z aukcji sie nie konczy
//	AUCTION_DATA	*pAuction;
//	AUCTION_DATA	*pNextAuction;
//	time_t			time_diff;
//	time_t			time_diff1;
	return;
	/*

	 for(pAuction = first_auction;pAuction;pAuction = pNextAuction)
	 {
	 pNextAuction = pAuction->next;
	 time_diff	= difftime(pAuction->end_time,current_time);
	 time_diff1	= difftime(pAuction->start_time,current_time);
	 if (time_diff1 < 0 && time_diff1 > -30)
	 send_auction_info (pAuction );
	 if (time_diff < 0 && time_diff > -30)
	 {
	 if ( !pAuction->notes_send && pAuction->first )
	 send_auction_notes( pAuction,ITEM_SOLD );
	 else if ( !pAuction->notes_send && !pAuction->first )
	 send_auction_notes( pAuction,NO_BIDDERS );
	 else if ( !pAuction->notes_send && !pAuction->item && !pAuction->item_collected )
	 send_auction_notes( pAuction, SHIP_SOLD );
	 }
	 else if (time_diff < -1209600) //jesli min�y 2 tygodnie
	 auction_cleanup(pAuction);
	 }*/
}

void subtract_times(struct timeval *etime, struct timeval *stime)
{
	etime->tv_sec -= stime->tv_sec;
	etime->tv_usec -= stime->tv_usec;
	while (etime->tv_usec < 0)
	{
		etime->tv_usec += 1000000;
		etime->tv_sec--;
	}
	return;
}

/* Trog */
void clan_update()
{
	HQ_DATA *hq;
	RID *room;

	auto clan_snapshot = clan_list;
	for (auto* clan : clan_snapshot)
	{
		adjust_clan(clan);

		if ((IS_ORG( clan ) || IS_SUBORG(clan)) && clan->repay_date != 0)
		{
			clan->loan = labs(clan->loan + clan->loan / 10);

			if (clan->repay_date < current_time)
			{
				DESCRIPTOR_DATA *d;
				CHAR_DATA *ch;

				/* rozwiazujemy organizacje z powodu nie splacenia pozyczki */
				for (auto* d : descriptor_list)
					if ((ch = d->original) && ch->pcdata && ch->pcdata->clan && ch->pcdata->clan == clan)
					{
						ch->pcdata->clan = NULL;
						save_char_obj(ch);
					}
				clan_list.remove(clan);
				free_clan(clan);
			}
		}

		for (auto* hq : clan->hqs)
		{
			for (auto* room : hq->rooms)
				if (room->clan_room.completion_date != 0 && room->clan_room.completion_date < current_time)
				{
					room->clan_room.completion_date = 0;
					/* budujemy pomieszczenie */
				}
		}
	}
}

/*
 added by Thanos:
 poni�sze funkcje pozwalaj� na stopniowe wykonywanie komend for i rat
 */
void clean_for_loop()
{
	for_loop.fOwner = NULL;
	for_loop.fVictim = NULL;
	for_loop.fArea = NULL;
	for_loop.fRoom = 0;
	for_loop.fMortals = false;
	for_loop.fGods = false;
	for_loop.fMobs = false;
	for_loop.fEverywhere = false;
	for_loop.fAreaMobs = false;
	for_loop.fAreaRooms = false;
	for_loop.fStopped = true;
	for_loop.fCount = 0;
	STRDUP(for_loop.command, "");
}

/* no �eby itoa nie by�o... */
char* __itoa(int num)	// defined as itoa (mud.h)
{
	static char buf[256];

	sprintf(buf, "%d", num);
	return (buf);
}

/*
 * ta funkcja przenosi gracza do miejsca, w ktorym p�tla for wykona
 * komend� i 'zwraca' go na miejsce ;) oraz interpretuje komend�
 */
void exec_command_in_loop(CHAR_DATA *owner, ROOM_INDEX_DATA *room, char *pSource, const char *namebuf)
{
	ROOM_INDEX_DATA *old_room;
	char buf[MAX_STRING_LENGTH];
	char *pDest = buf;
	int len = 0;

	while (*pSource)
	{
		/*
		 podmianka pod '#' nazwy warto�ci aktualnej zmiennej
		 BUGFIX: teraz ju� mo�na stosowa� zmienn� kilkakrotnie
		 w jednej komendzie (podmieniamy wszystkie '#')
		 */
		if (*pSource == '#')
		{
			if (namebuf)
			{
				len = 0;
				while (*namebuf)
				{
					*(pDest++) = *(namebuf++);
					len++;
				}
				namebuf -= len;
			}
			pSource++;
		}
		else
			*(pDest++) = *(pSource++);
	}
	*pDest = '\0';

	old_room = owner->in_room;
	char_from_room(owner);
	char_to_room(owner, room);
	interpret(owner, buf);
	char_from_room(owner);
	char_to_room(owner, old_room);
	return;
}

void update_for()
{
	CHAR_DATA *owner;
	CHAR_DATA *victim;
	bool found = false;
	ROOM_INDEX_DATA *room;

	if (for_loop.fStopped)
		return;

	/* czy w�a�ciciel jest jescze w grze? */
	for (auto* ch : char_list)
		if (ch == for_loop.fOwner)
		{
			found = true;
			break;
		}

	if ((!found)	//je�li wyszed� z gry.
	|| (!for_loop.command || for_loop.command[0] == '\0'))
	{
		clean_for_loop();
		log_string("For Loop aborted.");
		return;
	}

	// ten warunek raczej nie ma szans na wywo�anie (u�ywa�em go do debugowania)
	// ale zostawiam, bo jak mawiaj� 'strze�onego Yoda strze�e ;)
	if (!for_loop.fRoom && !for_loop.fVictim)
	{
		bug("No victim or room specified, cleaning For Loop");
		clean_for_loop();
		return;
	}

	/*wykonujemy komend�*/
	if (for_loop.fMobs || for_loop.fMortals || for_loop.fGods)
	{
		found = false;
		victim = NULL;
		{
			/* iterate backwards from fVictim's position in char_list */
			auto vit = std::find(char_list.begin(), char_list.end(), for_loop.fVictim);
			while (vit != char_list.begin())
			{
				--vit;
				CHAR_DATA *v = *vit;
				if ((!v->in_room) || (v == owner))
					continue;

				if (IS_NPC(v))
				{
					if (for_loop.fMobs)
					{
						victim = v;
						found = true;
						break;
					}
					else
						continue;
				}

				if (!IS_NPC(v))
				{
					if (for_loop.fGods && !for_loop.fMortals && get_trust(v) < LEVEL_IMMORTAL)
						continue;

					if (!for_loop.fGods && for_loop.fMortals && get_trust(v) >= LEVEL_IMMORTAL)
						continue;
				}
				victim = v;
				found = true;
				break;
			}
		}

		if (found && victim)
		{
			const SWString &expanded = name_expand(victim);
			exec_command_in_loop(owner, victim->in_room, for_loop.command, expanded.c_str());
			/* set fVictim to the element before victim */
			auto vit = std::find(char_list.begin(), char_list.end(), victim);
			if (vit != char_list.begin())
			{
				--vit;
				for_loop.fVictim = *vit;
			}
			else
				for_loop.fVictim = NULL;
		}
		else
		{
			ch_printf(owner, "For Loop finished. Made %d iteration%s." NL, for_loop.fCount, for_loop.fCount == 1 ? "" : "s");
			clean_for_loop();
			return;
		}
	}
	else if (for_loop.fAreaMobs)
	{
		/* iterate forward from fVictim in char_list */
		auto vit = std::find(char_list.begin(), char_list.end(), for_loop.fVictim);
		bool area_found = false;
		for (; vit != char_list.end(); ++vit)
		{
			victim = *vit;
			if (victim && victim->in_room && victim->in_room->area && victim->in_room->area == for_loop.fArea
					&& !room_is_private(victim, victim->in_room))
			{
				const SWString &expanded = name_expand(victim);
				exec_command_in_loop(owner, victim->in_room, for_loop.command, expanded.c_str());
				++vit;
				for_loop.fVictim = (vit != char_list.end()) ? *vit : NULL;
				area_found = true;
				break;
			}
		}
		if (!area_found)
		{
			ch_printf(owner, "For Loop finished. Made %d iteration%s." NL, for_loop.fCount, for_loop.fCount == 1 ? "" : "s");
			clean_for_loop();
			return;
		}
	}
	else if (for_loop.fEverywhere)
	{
		for (; for_loop.fRoom <= MAX_VNUM; for_loop.fRoom++)
		{
			room = get_room_index(for_loop.fRoom);
			if (room && !room_is_private(owner, room))
			{
				exec_command_in_loop(owner, room, for_loop.command,
				itoa(for_loop.fRoom));
				for_loop.fRoom++;
				break;
			}
		}
		if (for_loop.fRoom >= MAX_VNUM)
		/*
		 * bleh -- MAX_VNUM nie zapewnia nam, �e polecimy po wszystkich
		 * 	   zostaj� jeszcze pokoje virtualne
		 */
		{
			ch_printf(owner, "For Loop finished. Made %d iteration%s." NL, for_loop.fCount, for_loop.fCount == 1 ? "" : "s");
			clean_for_loop();
			return;
		}
	}
	else if (for_loop.fAreaRooms)
	{
		for (; for_loop.fRoom <= MAX_VNUM; for_loop.fRoom++)
		{
			room = get_room_index(for_loop.fRoom);
			if (room && room->area && room->area == for_loop.fArea && !room_is_private(owner, room))
			{
				exec_command_in_loop(owner, room, for_loop.command,
				itoa(for_loop.fRoom));
				for_loop.fRoom++;
				break;
			}
		}

		if (for_loop.fRoom >= MAX_VNUM)
		/*
		 * bleh -- MAX_VNUM nie zapewnia nam, �e polecimy po wszystkich
		 * 	   zostaj� jeszcze pokoje virtualne
		 */
		{
			ch_printf(owner, "For Loop finished. Made %d iteration%s." NL, for_loop.fCount, for_loop.fCount == 1 ? "" : "s");
			clean_for_loop();
			return;
		}
	}
	for_loop.fCount++;
	return;
}

void clean_rat_loop()
{
	rat_loop.rOwner = NULL;
	rat_loop.rRoom = 0;
	rat_loop.Start = 0;
	rat_loop.End = 0;
	rat_loop.rCount = 0;
	rat_loop.rStopped = true;
	STRDUP(rat_loop.command, "");
	return;
}

void update_rat()
{
	CHAR_DATA *owner;
	bool found;

	if (rat_loop.rStopped)
		return;

	/* czy w�a�ciciel jest jescze w grze? */
	for (auto* ch : char_list)
		if (ch == rat_loop.rOwner)
		{
			owner = ch;
			found = true;
			break;
		}

	if ((!found)	//je�li wyszed� z gry.
	|| (!rat_loop.command || rat_loop.command[0] == '\0'))
	{
		clean_rat_loop();
		log_string("Rat Loop aborted.");
		return;
	}

	rat_loop.rCount++;

	for (; rat_loop.rRoom <= rat_loop.End; rat_loop.rRoom++)
	{
		exec_command_in_loop(rat_loop.rOwner, owner->in_room, rat_loop.command,
		itoa(rat_loop.rRoom));
		rat_loop.rRoom++;
		break;
	}

	if (rat_loop.rRoom > rat_loop.End)
	{
		ch_printf(owner, "Rat Loop finished. Made %d iteration%s." NL, rat_loop.rCount, rat_loop.rCount == 1 ? "" : "s");
		clean_rat_loop();
	}
	return;
}
/* For & Rat update done by Thanos */

/*
 * Procedura detonujaca granaty (by Lomion) wywolywana w kazdej petli muda
 * mozna oszczedniej,
 ale dzieki temu granaty dzialaja jakos... realniej??
 * no i ciekawiej :)
 */
void update_explosives(void)
{
	OBJ_DATA *obj;

	for (auto* obj : object_list)
	{
		CHAR_DATA *ch;
		ROOM_INDEX_DATA *room;

		if (obj->item_type != ITEM_GRENADE && obj->item_type != ITEM_LANDMINE)
			continue;

		if (obj->value[2] > 0)
		{
			// Thanos : w saferoomach liczniki granat�w zatrzymuj� si�
			room = NULL;
			if ((ch = obj->carried_by))
				room = ch->in_room;
			else if (obj->in_room)
				room = obj->in_room;

			if (room && !room->people.empty() && IS_SET(room->room_flags, ROOM_SAFE))
			{
				act( FB_WHITE, "Ochronne pole si�owe zatrzymuje zapalnik $p$1.", room->people.front(), obj, NULL, TO_ROOM);
				act( FB_WHITE, "Ochronne pole si�owe zatrzymuje zapalnik $p$1.", room->people.front(), obj, NULL, TO_CHAR);
				obj->value[2] = 0;
			}
			// done by Thanos
			if (obj->value[2] == 1)
				explode(obj);
			else
				obj->value[2]--;
		}
	}
	return;
}
