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
 *		        Main structure manipulation module		   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <algorithm>
#include "mud.h"

#define BFS_MARK		BV63

extern int top_exit;
extern int top_ed;
extern int top_affect;
extern int cur_qobjs;
extern int cur_qchars;

CHAR_DATA *cur_char;
ROOM_INDEX_DATA *cur_room;
bool cur_char_died;
ch_ret global_retcode;

int cur_obj;
int cur_obj_serial;
bool cur_obj_extracted;
obj_ret global_objcode;

bool is_wizvis(CHAR_DATA *ch, CHAR_DATA *victim);

OBJ_DATA* group_object(OBJ_DATA *obj1, OBJ_DATA *obj2);

int ris_save(CHAR_DATA *ch, int chance, int ris);	// force.c

void room_explode(OBJ_DATA *obj, CHAR_DATA *xch, ROOM_INDEX_DATA *room);
void room_explode_1(OBJ_DATA *obj, CHAR_DATA *xch, ROOM_INDEX_DATA *room, int blast);
void room_explode_2(ROOM_INDEX_DATA *room, int blast);

void stopscript(SCRIPT_DATA *script); /* Thanos */

void free_suspect(CHAR_DATA *ch, SUSPECT_DATA *sus); /* Thanos */

bool belongs_to_quest args( ( CHAR_DATA *ch, QUEST_DATA *quest ) );	//Thanos
MEMBER_DATA* get_member args( ( CLAN_DATA *clan, char *name ) );

void logExplosion(const SWString &logStr)
{
	to_channel(logStr.c_str(), CHANNEL_MONITOR, "Explosive", LEVEL_LOG);
	log_string_plus(logStr.c_str(), LOG_ALL, LEVEL_LOG);
}

/*
 * Lomion: Dopisuje takie 'cos' zeby granat 'wybuchal' granat, mine, etc (narazie nie ma etc)
 * Uzywam do tego V4 ktore nie jest uzywane przy mat. wybuchowych do niczego...
 * v4 = 1 oznacza 'zdetonowany' mat. wybuch
 * dopisuje funkcje wyszukujaca statek na podstawie pokoju.
 */
void explode(OBJ_DATA *obj)
{
	if (obj->armed_by && obj->value[4] != 1)
	{
		ROOM_INDEX_DATA *room;
		bool held = false;
		/* Lomion: moje zmienne */
		SHIP_DATA *ship;
		SWString buf;
		int damage;
		/* by Lomion */
		obj->value[4] = 1;

		for (auto* xch : char_list)
			if (!IS_NPC(xch) && nifty_is_name(obj->armed_by, xch->name))
			{
				if (obj->carried_by)
				{
					act(FB_WHITE, "$p EXPLODUJE w r�kach $n$1!", obj->carried_by, obj, NULL, TO_ROOM);
					act(FB_WHITE, "$p EXPLODUJE w twoich r�kach!", obj->carried_by, obj, NULL, TO_CHAR);
					room = obj->carried_by->in_room;
					held = true;

					buf << SWInt::toString(obj->pIndexData->vnum) << " (" << nocolor(obj->przypadki[0]) << ") armed by " << obj->armed_by
							<< ", carried by " << obj->carried_by->name << " exploded in room " << SWInt::toString(room->vnum) << " ("
							<< nocolor(room->name) << ").";
				}
				else if (obj->in_room)
				{
					room = obj->in_room;

					buf << SWInt::toString(obj->pIndexData->vnum) << " (" << nocolor(obj->przypadki[0]) << ") armed by " << obj->armed_by
							<< " exploded in room " << SWInt::toString(room->vnum) << " (" << nocolor(room->name) << ").";
				}
				else
				{
					room = NULL;

					buf << SWInt::toString(obj->pIndexData->vnum) << " (" << nocolor(obj->przypadki[0]) << ") armed by " << obj->armed_by
							<< " exploded in NULL room.";
				}
				logExplosion(buf);

				if (room)
				{
					/* Eksplozja statku (by Lomion) [tworzy szczatki statku] */
					ship = ship_from_room(room);
					damage = number_range(obj->value[0] / 2, obj->value[0]);

					// Thanos: tylko granaty wybuchowe mog� udzkodzi� statek
					if (ship && obj->value[3] == GRANADE_EXPLOSIVE)
					{
						buf.clear();

						if (ship->engineroom != room)
						{
							damage /= 10;

							buf << SWInt::toString(obj->pIndexData->vnum) << " (" << nocolor(obj->przypadki[0]) << ") armed by "
									<< obj->armed_by << " exploded in ship room " << SWInt::toString(room->vnum) << " ("
									<< nocolor(room->name) << ") of " << ship->name << ".";
						}
						else
						{
							buf << SWInt::toString(obj->pIndexData->vnum) << " (" << nocolor(obj->przypadki[0]) << ") armed by "
									<< obj->armed_by << " exploded in engine room " << SWInt::toString(room->vnum) << " ("
									<< nocolor(room->name) << ") of " << ship->name << ".";
						}
						logExplosion(buf);

						ship->hull -= damage;

						if (ship->hull <= 0)
						{
							destroy_ship(ship, xch);

							buf.clear();
							buf << "ship " << ship->name << " blown up by " << obj->armed_by << "!";
							logExplosion(buf);
						}
					}
					/* koniec eksplozji statku */

					if (!held && !room->people.empty())
					{
						act(FB_WHITE, "$p EXPLODUJE!", room->people.front(), obj, NULL, TO_ROOM);
					}

					room_explode(obj, xch, room);
				}
			}
	}
	/* Lomion: zeby nie dawalo DZIWNYCH BUGow :) */
	if (obj)
		make_scraps(obj);
}

bool is_safe_grenade(CHAR_DATA *ch, CHAR_DATA *xch)
{
	if (!ch)
		return true;

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
		return true;

	if (!IS_NPC(ch) && ch->pcdata && ch->pcdata->hotel_safe_time > 0)
		return true;

	if ( IS_NPC( ch ) && ch->inquest && ch->inquest != xch->inquest)
		return true;

	if (!IS_NPC(ch) && get_trust(ch) > LEVEL_HERO && IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;

	return false;
}

void room_explode(OBJ_DATA *obj, CHAR_DATA *xch, ROOM_INDEX_DATA *room)
{
	int blast;

	blast = (int) (obj->value[1]);
	room_explode_1(obj, xch, room, 1);
	room_explode_2(room, blast);
}

// Wybucha pok�j i s�siaduj�ce z nim te� (w zale�no�ci od si�y eksplozji)
// ka�dy pok�j jest zaznaczany (BFS_MARK), �eby nie p�tli�
void room_explode_1(OBJ_DATA *obj, CHAR_DATA *xch, ROOM_INDEX_DATA *room, int blast)
{
	AFFECT_DATA af;
	int dam;
	int chance;
	SWString buf;

	if ( IS_SET(room->room_flags,
			BFS_MARK) || IS_SET(room->room_flags, ROOM_SAFE))
		return;

	buf << "shockwave of " << SWInt::toString(obj->pIndexData->vnum) << " (" << nocolor(obj->przypadki[0]) << ") armed by " << obj->armed_by
			<< " passes through room " << SWInt::toString(room->vnum) << " (" << nocolor(room->name) << ").";
	logExplosion(buf);

	// bugfix by Thanos -- nie ma explozji w saferoomach.
	auto people_snapshot = room->people;
	for (auto* rch : people_snapshot)
	{

		if (is_safe_grenade(rch, xch))
		{
			send_to_char( FB_WHITE "Pot�na EXPLOZJA wstrz�sa okolic�!" EOL, rch);
			continue;
		}

		switch (obj->value[3])
		{
		default:
			act( FB_WHITE, "Fala uderzeniowa olbrzymiej eksplozji przechodzi przez twoje cia�o!", rch, obj, NULL, TO_CHAR);
			if (blast > 1)
				dam = number_range(obj->value[0] / blast, obj->value[0] / (blast - 1));
			else
				dam = obj->value[0];
			damage(rch, rch, dam, TYPE_UNDEFINED);
			break;
		case GRANADE_POISON:
		{
			bool first = true;

			act( FB_WHITE, "Fala uderzeniowa olbrzymiej eksplozji truj�cego gazu"
					" przechodzi przez twoje cia�o!", rch, obj, NULL,
			TO_CHAR);
			if (blast > 1)
				dam = number_range(obj->value[0] / blast, obj->value[0] / (blast - 1));
			else
				dam = obj->value[0];

			/* ten kod jest wyci�ty z spell_poison */
			chance = ris_save(rch, obj->level, RIS_POISON);
			if (chance == 1000 || saves_poison_death(chance, rch))
				break;
			if (IS_AFFECTED(rch, AFF_POISON))
				first = false;
			af.type = gsn_poison;
			af.duration = number_fuzzy(dam);
			af.location = APPLY_STR;
			af.modifier = -2;
			af.bitvector = AFF_POISON;
			affect_join(rch, &af);
			send_to_char( COL_FORCE, rch);
			send_to_char("Czujesz, �e jest ci bardzo niedobrze." NL, rch);
			rch->mental_state = URANGE(20, rch->mental_state + (first ? 5 : 0), 100);
		}
			break;
		case GRANADE_STUN:
			act( FB_WHITE, "Huk o niezwykle wysokiej cz�stotliwo�ci wwierca si� w tw�j m�zg!", rch, obj, NULL, TO_CHAR);
			if (blast > 1)
				dam = number_range(obj->value[0] / blast, obj->value[0] / (blast - 1));
			else
				dam = obj->value[0];

			if (!IS_AFFECTED(rch, AFF_PARALYSIS) || rch->top_level > obj->level)
			{
				af.type = gsn_stun;
				af.location = APPLY_AC;
				af.modifier = 20;
				af.duration = number_fuzzy(dam / 3);
				af.bitvector = AFF_PARALYSIS;
				affect_to_char(rch, &af);
				update_pos(rch);
			}
			break;
		case GRANADE_BLIND:
			act( FB_WHITE, "B�ysk jasny jak wybuch supernovej o�lepia ci�!", rch, obj, NULL, TO_CHAR);
			if (blast > 1)
				dam = number_range(obj->value[0] / blast, obj->value[0] / (blast - 1));
			else
				dam = obj->value[0];

			if (!IS_AFFECTED(rch, AFF_BLIND))
			{
				af.type = gsn_blindness;
				af.location = APPLY_HITROLL;
				af.modifier = 13 + 4 * rch->top_level / 3;
				af.duration = number_fuzzy(dam / 2);
				af.bitvector = AFF_BLIND;
				affect_to_char(rch, &af);
			}
			break;
		}

		add_inform(rch, xch);

		if (!char_died(rch))
		{
			// huntuj� tylko moby w promieniu 3 lokacji od centrum wybuchu
			// -- Thanos
			if (IS_NPC( rch ) && blast < 3)
			{
				if (IS_SET(rch->act, ACT_SENTINEL))
				{
					rch->was_sentinel = rch->in_room;
					REMOVE_BIT(rch->act, ACT_SENTINEL);
				}
				start_hating(rch, xch);
				start_hunting(rch, xch);
			}
		}
	}

	// na przedmioty mo�e mie� wp�yw jedynie explozja dynamitu,
	// albo trucizna (fontanny i drink_containers)  -- Thanos
	auto contents_snapshot = room->contents;
	for (auto* robj : contents_snapshot)
	{
		separate_obj(robj); /* by Lomion */

		if (robj != obj && robj->item_type != ITEM_SHIPMODULE && robj->item_type != ITEM_SCRAPS && robj->item_type != ITEM_CORPSE_NPC
				&& robj->item_type != ITEM_CORPSE_PC && robj->item_type != ITEM_DROID_CORPSE)
		{
			/* Lomion: a teraz detonujemy inne granaty */
			// tylko dynamit odpala sie rekursywnie -- Thanos:
			if ((robj->item_type == ITEM_GRENADE || robj->item_type == ITEM_LANDMINE) && obj->value[3] == GRANADE_EXPLOSIVE)
			{
				STRDUP(robj->armed_by, obj->armed_by);
				explode(robj);
			}
			/* Lomion: koniec zabawy :) */
			else
			{
				if (obj->value[3] == GRANADE_EXPLOSIVE)
					make_scraps(robj);
				else
				// Thanos: trujemy
				if (obj->value[3] == GRANADE_POISON)
				{
					if (robj->item_type == ITEM_DRINK_CON || robj->item_type == ITEM_FOOD || robj->item_type == ITEM_FOUNTAIN)
						robj->value[3] = 1;
				}
			}
		}
	}

	/* other rooms */
	SET_BIT(room->room_flags, BFS_MARK);

	if (!room->exits.empty())
	{
		for (auto* pexit : room->exits)
		{
			if (pexit->to_room && pexit->to_room != room)
			{
				if (blast <= obj->value[1])
				{
					//int roomblast;
					//roomblast = blast - 1;
					room_explode_1(obj, xch, pexit->to_room, blast + 1);
				}
				else if (!IS_SET(pexit->to_room->room_flags, BFS_MARK))
					echo_to_room(pexit->to_room,
					FB_WHITE "S�yszysz potworn� EXPLOZJ� gdzie� niedaleko!");
			}
		}
	}
}

// Sprz�ta zaznaczenia (BFS_MARK) z pokoi dotkni�tych wybuchem
void room_explode_2(ROOM_INDEX_DATA *room, int blast)
{
	if (!IS_SET(room->room_flags, BFS_MARK))
		return;

	REMOVE_BIT(room->room_flags, BFS_MARK);

	if (blast > 0)
	{
		int roomblast;

		for (auto* pexit : room->exits)
		{
			if (pexit->to_room && pexit->to_room != room)
			{
				roomblast = blast - 1;
				room_explode_2(pexit->to_room, roomblast);
			}
		}
	}
	if (room->room_flags & ROOM_STORAGE)
	{
		save_room_storage(room);
	}
}

#if defined( ARMAGEDDON )
/*
* "Roll" players stats based on the character name           -Thoric
*/
void name_stamp_stats(CHAR_DATA * ch)
{
	int x, a, b, c;

	for (x = 0; x < 5; x++)
	{
		c = number_range(0, 13);
		/*	c = c + number_range(x, 2*x+13 ); */
		if ( x < strlen(ch->name) )
			c += ch->name[x];
		b = c % 14;
		a = (c % 3) - 1;
		switch (b)
		{
			case 0:
				ch->perm_str = UMAX(9, ch->perm_str - abs(a));
				break;
			case 1:
				ch->perm_dex = UMAX(9, ch->perm_dex - abs(a));
				break;
			case 2:
				ch->perm_wis = UMAX(9, ch->perm_wis - abs(a));
				break;
			case 3:
				ch->perm_int = UMAX(9, ch->perm_int - abs(a));
				break;
			case 4:
				ch->perm_con = UMAX(9, ch->perm_con - abs(a));
				break;
			case 5:
				ch->perm_cha = UMAX(9, ch->perm_cha - abs(a));
				break;
			case 6:
				ch->perm_lck = UMAX(9, ch->perm_lck - abs(a));
				break;
			case 7:
				ch->perm_str = UMIN(18, ch->perm_str + abs(a));
				break;
			case 8:
				ch->perm_dex = UMIN(18, ch->perm_dex + abs(a));
				break;
			case 9:
				ch->perm_wis = UMIN(18, ch->perm_wis + abs(a));
				break;
			case 10:
				ch->perm_int = UMIN(18, ch->perm_int + abs(a));
				break;
			case 11:
				ch->perm_con = UMIN(18, ch->perm_con + abs(a));
				break;
			case 12:
				ch->perm_cha = UMIN(18, ch->perm_cha + abs(a));
				break;
			case 13:
				ch->perm_lck = UMIN(18, ch->perm_lck + abs(a));
				break;
		}
	}
	ch->perm_str = URANGE( 9, ch->perm_str, 18 );
	ch->perm_int = URANGE( 9, ch->perm_int, 18 );
	ch->perm_wis = URANGE( 9, ch->perm_wis, 18 );
	ch->perm_dex = URANGE( 9, ch->perm_dex, 18 );
	ch->perm_con = URANGE( 9, ch->perm_con, 18 );
	ch->perm_cha = URANGE( 9, ch->perm_cha, 18 );
	ch->perm_lck = URANGE( 9, ch->perm_lck, 18 );
	fevent_trigger( ch, FE_GAIN_STAT );
}

void roll_stats( CHAR_DATA *ch )
{
	ch->perm_str = 13;
	ch->perm_dex = 13;
	ch->perm_int = 13;
	ch->perm_wis = 13;
	ch->perm_cha = 13;
	ch->perm_con = 13;
	ch->perm_lck = 13;

	switch (class_table[ch->main_ability].attr_prime)
	{
		case	REQ_STR: ch->perm_str = 16; break;
		case	REQ_INT: ch->perm_int = 16; break;
		case	REQ_WIS: ch->perm_wis = 16; break;
		case	REQ_DEX: ch->perm_dex = 16; break;
		case	REQ_CON: ch->perm_con = 16; break;
		case	REQ_CHA: ch->perm_cha = 16; break;
		case	REQ_LCK: ch->perm_lck = 16; break;
	}

	ch->perm_str += ch->race->str_plus;
	ch->perm_int += ch->race->int_plus;
	ch->perm_wis += ch->race->wis_plus;
	ch->perm_dex += ch->race->dex_plus;
	ch->perm_con += ch->race->con_plus;
	ch->perm_cha += ch->race->cha_plus;
	ch->perm_lck += ch->race->lck_plus;
	ch->perm_frc  = 0;
	name_stamp_stats(ch);
}
#endif

bool is_wizvis(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS) && get_trust(ch) < victim->pcdata->wizinvis
			&& !is_name(ch->name, victim->pcdata->invis_except))
		return false;

	return true;
}

/*
 * Return how much exp a char has
 */
int get_exp(CHAR_DATA *ch, int ability)
{
	if (ability >= MAX_ABILITY || ability < 0)
		return 0;

	return ch->experience[ability];
}

/*
 * Calculate roughly how much experience a character is worth
 */
int get_exp_worth(CHAR_DATA *ch)
{
	int exp;

	exp = ch->top_level;
	exp += ch->skill_level[COMBAT_ABILITY];
	exp += ch->max_hit;
	exp -= ch->armor;
	exp += (GET_HITROLL(ch) + GET_DAMROLL(ch)) / 2;
	if (IS_AFFECTED(ch, AFF_SANCTUARY))
		exp += exp * 1.5;
	if (IS_AFFECTED(ch, AFF_SHOCKSHIELD))
		exp += exp * 1.2;
	exp = URANGE(MIN_EXP_WORTH, exp, MAX_EXP_WORTH);

	return exp;
}

/*								-Thoric
 * Return how much experience is required for ch to get to a certain level
 */
int exp_level(int level)
{
	int lvl;

	lvl = UMAX(0, level - 1);

	return (lvl * lvl * 500);
}

/*
 * Get what level ch is based on exp
 */
int level_exp(CHAR_DATA *ch, int exp)
{
	int x, lastx, y, tmp;

	x = LEVEL_SUPREME;
	lastx = x;
	y = 0;
	while (!y)
	{
		tmp = exp_level(x);
		lastx = x;
		if (tmp > exp)
			x /= 2;
		else if (lastx != x)
			x += (x / 2);
		else
			y = x;
	}
	if (y < 1)
		y = 1;
	if (y > LEVEL_SUPREME)
		y = LEVEL_SUPREME;
	return y;
}

/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust(CHAR_DATA *ch)
{
	if (!ch)
		return 0;

	if (ch == supermob)
		return 104;

	if (ch->desc && ch->desc->original)
		ch = ch->desc->original;

	if (ch->trust != 0)
		return ch->trust;

	if ( IS_NPC(ch) && ch->top_level >= LEVEL_AVATAR)
		return LEVEL_AVATAR;

	/* Trog: po chuj to?!
	 if( ch->top_level >= LEVEL_NEOPHYTE && IS_RETIRED( ch ) )
	 return LEVEL_NEOPHYTE;
	 */
	return ch->top_level;
}

/*
 * Retrieve a character's age.
 */
int get_age(CHAR_DATA *ch)
{
	return 17 + (ch->played + (current_time - ch->logon)) / 14400;
}

/*
 * Retrieve character's current wisdom.
 */
#if defined (ARMAGEDDON )
int get_curr_wis( CHAR_DATA *ch )
{
	int mod;
	int value;

	mod		= ch->race->wis_plus;
	value	= 13 + mod;

	if ( IS_NPC( ch ) )
		return value;

	return URANGE( 3, ch->perm_wis + ch->mod_wis, get_max_wis( ch ) );
}

int get_max_wis( CHAR_DATA *ch )
{
	int max;
	int mod;
	int value;

	mod		= ch->race->wis_plus;
	value	= 13 + mod;

	if ( IS_NPC( ch ) )
		return value;

	max = UMIN( 20 + mod +
			class_table[ch->main_ability].attr_mod[REQ_WIS-1], 25 );
	return max;
}



/*
* Retrieve character's current dexterity.
*/
int get_curr_dex( CHAR_DATA *ch )
{
	int mod;
	int value;

	mod   = ch->race->dex_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	return URANGE( 3, ch->perm_dex + ch->mod_dex, get_max_dex( ch ) );
}

int get_max_dex( CHAR_DATA *ch )
{
	int max;
	int mod;
	int value;

	mod   = ch->race->dex_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	max = UMIN( 20 + mod + class_table[ch->main_ability].attr_mod[REQ_DEX-1],
		25 );
	return max;
}



/*
* Retrieve character's current constitution.
*/
int get_curr_con( CHAR_DATA *ch )
{
	int mod;
	int value;

	mod   = ch->race->con_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	return URANGE( 3, ch->perm_con + ch->mod_con, get_max_con( ch ) );
}

int get_max_con( CHAR_DATA *ch )
{
	int max;
	int mod;
	int value;

	mod   = ch->race->con_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	max = UMIN( 20 + mod + class_table[ch->main_ability].attr_mod[REQ_CON-1],
		25 );
	return max;
}


/*
* Retrieve character's luck
*/
int get_curr_lck( CHAR_DATA *ch )
{
	int mod;
	int value;

	mod   = ch->race->lck_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	return URANGE( 3, ch->perm_lck + ch->mod_lck, get_max_lck( ch ) );
}

int get_max_lck( CHAR_DATA *ch )
{
	int max;
	int mod;
	int value;

	mod   = ch->race->lck_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	max = UMIN( 20 + mod + class_table[ch->main_ability].attr_mod[REQ_LCK-1],
		25 );
	return max;
}


/*
* Retrieve character's force
*/
int get_curr_frc( CHAR_DATA *ch )
{
	int mod;
	int value;

	mod   = ch->race->frc_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	return URANGE( 3, ch->perm_frc + ch->mod_frc, get_max_frc( ch ) );
}

int get_max_frc( CHAR_DATA *ch )
{
	int max;
	int mod;
	int value;

	mod   = ch->race->frc_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	max = UMIN( 20 + mod + class_table[ch->main_ability].attr_mod[REQ_FRC-1],
		25 );
	return max;
}

/*
* Retrieve character's intelligence
*/
int get_curr_int( CHAR_DATA *ch )
{
	int mod;
	int value;

	mod   = ch->race->int_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	return URANGE( 3, ch->perm_int + ch->mod_int, get_max_int( ch ) );
}

int get_max_int( CHAR_DATA *ch )
{
	int max;
	int mod;
	int value;

	mod   = ch->race->int_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	max = UMIN( 20 + mod + class_table[ch->main_ability].attr_mod[REQ_INT-1],
		25 );
	return max;
}

/*
* Retrieve character's force
*/
int get_curr_cha( CHAR_DATA *ch )
{
	int mod;
	int value;

	mod   = ch->race->cha_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	return URANGE( 3, ch->perm_cha + ch->mod_cha, get_max_cha( ch ) );
}

int get_max_cha( CHAR_DATA *ch )
{
	int max;
	int mod;
	int value;

	mod   = ch->race->cha_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	max = UMIN( 20 + mod + class_table[ch->main_ability].attr_mod[REQ_CHA-1],
		25 );
	return max;
}

/*
* Retrieve character's force
*/
int get_curr_str( CHAR_DATA *ch )
{
	int mod;
	int value;

	mod   = ch->race->str_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	return URANGE( 3, ch->perm_str + ch->mod_str, get_max_str( ch ) );
}

int get_max_str( CHAR_DATA *ch )
{
	int max;
	int mod;
	int value;

	mod   = ch->race->str_plus;
	value = 13 + mod;

	if ( IS_NPC( ch ) )
	return value;

	max = UMIN( 20 + mod + class_table[ch->main_ability].attr_mod[REQ_STR-1],
		25 );
	return max;
}


#else

/*
 * Retrieve character's current strength.
 */
int get_curr_str(CHAR_DATA *ch)
{
	int max;

	max = 20 + ch->race->str_plus;
	max = UMIN(max, 25);
	return URANGE(3, ch->perm_str + ch->mod_str, max);
}

/*
 * Retrieve character's current intelligence.
 */
int get_curr_int(CHAR_DATA *ch)
{
	int max;

	max = 20 + ch->race->int_plus;
	max = UMIN(max, 25);

	return URANGE(3, ch->perm_int + ch->mod_int, max);
}

int get_curr_wis(CHAR_DATA *ch)
{
	int max;

	max = 20 + ch->race->wis_plus;
	max = UMIN(max, 25);

	return URANGE(3, ch->perm_wis + ch->mod_wis, max);
}

/*
 * Retrieve character's current dexterity.
 */
int get_curr_dex(CHAR_DATA *ch)
{
	int max = 1;
	max = UMIN(max, 25);

	max = 20 + ch->race->dex_plus;

	return URANGE(3, ch->perm_dex + ch->mod_dex, max);
}

/*
 * Retrieve character's current constitution.
 */
int get_curr_con(CHAR_DATA *ch)
{
	int max = 1;
	max = UMIN(max, 25);

	max = 20 + ch->race->con_plus;

	return URANGE(3, ch->perm_con + ch->mod_con, max);
}

/*
 * Retrieve character's current charisma.
 */
int get_curr_cha(CHAR_DATA *ch)
{
	int max = 1;
	max = UMIN(max, 25);

	max = 20 + ch->race->cha_plus;

	return URANGE(3, ch->perm_cha + ch->mod_cha, max);
}

/*
 * Retrieve character's current luck.
 */
int get_curr_lck(CHAR_DATA *ch)
{
	int max = 20 + ch->race->lck_plus;
	max = UMIN(max, 25);

	return URANGE(3, ch->perm_lck + ch->mod_lck, max);
}

int get_curr_frc(CHAR_DATA *ch)
{
	int max;

	max = 20 + ch->race->frc_plus;
	max = UMIN(max, 25);

	return URANGE(0, ch->perm_frc + ch->mod_frc, max);
}
#endif

/*
 * Retrieve a character's carry capacity.
 * Vastly reduced (finally) due to containers		-Thoric
 */
int can_carry_n(CHAR_DATA *ch)
{
	int penalty = 0;

	if (!IS_NPC(ch) && get_trust(ch) >= LEVEL_IMMORTAL)
		return get_trust(ch) * 200;

	if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET))
		return 0;

	if (get_eq_char(ch, WEAR_WIELD))
		++penalty;
	if (get_eq_char(ch, WEAR_DUAL_WIELD))
		++penalty;
	if (get_eq_char(ch, WEAR_MISSILE_WIELD))
		++penalty;
	if (get_eq_char(ch, WEAR_HOLD))
		++penalty;
	if (get_eq_char(ch, WEAR_SHIELD))
		++penalty;

#if defined( ARMAGEDDON )
	return URANGE(5, (ch->top_level+16)/4 + get_curr_dex(ch)-12 - penalty, 30);
#else
	return URANGE(5, (ch->top_level + 15) / 5 + get_curr_dex(ch) - 13 - penalty, 20);
#endif
}

/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w(CHAR_DATA *ch)
{
	if (!IS_NPC(ch) && get_trust(ch) >= LEVEL_IMMORTAL)
		return 1000000;

	if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PET))
		return 0;

	return str_app[get_curr_str(ch)].carry;
}

/*
 * See if a player/mob can take a piece of prototype eq		-Thoric
 */
bool can_take_proto(CHAR_DATA *ch)
{
	if (IS_IMMORTAL(ch))
		return true;
	else if ( IS_NPC(ch) && IS_SET(ch->act, ACT_PROTOTYPE))
		return true;
	else if ( IS_NPC(ch) && IS_SET(ch->act, ACT_QUEST))
	{
		bug("Taking PROTOTYPE obj --> QuestMob");
		return true;
	}
	else
		return false;
}

/*
 * See if a string is one of the names of an object.
 */
bool is_name(const char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	if (!namelist || namelist[0] == '\0')
		return false;

	for (;;)
	{
		namelist = one_argument(namelist, name);
		if (name[0] == '\0')
			return false;
		if (!str_cmp(str, name))
			return true;
	}
}

bool is_name(const char *str, const char *namelist)
{
	return is_name(str, (char*) namelist);
}

bool is_name_prefix(const char *str, const char *namelist)
{
	return is_name_prefix(str, (char*) namelist);
}

bool is_name_prefix(const char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	for (;;)
	{
		namelist = one_argument(namelist, name);
		if (name[0] == '\0')
			return false;
		if (!str_prefix(str, name))
			return true;
	}
}

/*
 * See if a string is one of the names of an object.		-Thoric
 * Treats a dash as a word delimiter as well as a space
 */
bool is_name2(const char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	for (;;)
	{
		namelist = one_argument2(namelist, name);
		if (name[0] == '\0')
			return false;
		if (!str_cmp(str, name))
			return true;
	}
}

bool is_name2_prefix(const char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	for (;;)
	{
		namelist = one_argument2(namelist, name);
		if (name[0] == '\0')
			return false;
		if (!str_prefix(str, name))
			return true;
	}
}

/*								-Thoric
 * Checks if str is a name in namelist supporting multiple keywords
 */
bool nifty_is_name(char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	if (!str || str[0] == '\0')
		return false;

	for (;;)
	{
		str = one_argument2(str, name);
		if (name[0] == '\0')
			return true;
		if (!is_name2(name, namelist))
			return false;
	}
}

bool nifty_is_name_prefix(char *str, char *namelist)
{
	char name[MAX_INPUT_LENGTH];

	if (!str || str[0] == '\0')
		return false;

	for (;;)
	{
		str = one_argument2(str, name);
		if (name[0] == '\0')
			return true;
		if (!is_name2_prefix(name, namelist))
			return false;
	}
}

/*
 * Apply or remove an affect to a character.
 */
void affect_modify(CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd)
{
	OBJ_DATA *wield;
	int mod;
	struct skill_type *skill;
	ch_ret retcode;

	mod = paf->modifier;

	if (fAdd)
	{
		SET_BIT(ch->affected_by, paf->bitvector);
	}
	else
	{
		REMOVE_BIT(ch->affected_by, paf->bitvector);
		/*
		 * might be an idea to have a duration removespell which returns
		 * the spell after the duration... but would have to store
		 * the removed spell's information somewhere...		-Thoric
		 */
		if ((paf->location % REVERSE_APPLY) == APPLY_REMOVESPELL)
			return;
		switch (paf->location % REVERSE_APPLY)
		{
		case APPLY_AFFECT:
			REMOVE_BIT(ch->affected_by, mod);
			return;
		case APPLY_RESISTANT:
			REMOVE_BIT(ch->resistant, mod);
			return;
		case APPLY_IMMUNE:
			REMOVE_BIT(ch->immune, mod);
			return;
		case APPLY_SUSCEPTIBLE:
			REMOVE_BIT(ch->susceptible, mod);
			return;
		case APPLY_WEARSPELL: /* affect only on wear */
			return;
		case APPLY_REMOVE:
			SET_BIT(ch->affected_by, mod);
			return;
		}
		mod = 0 - mod;
	}

	switch (paf->location % REVERSE_APPLY)
	{
	default:
		bug("unknown location %d.", paf->location);
		return;

	case APPLY_NONE:
		break;
	case APPLY_STR:
		ch->mod_str += mod;
		break;
	case APPLY_DEX:
		ch->mod_dex += mod;
		break;
	case APPLY_INT:
		ch->mod_int += mod;
		break;
	case APPLY_WIS:
		ch->mod_wis += mod;
		break;
	case APPLY_CON:
		ch->mod_con += mod;
		break;
	case APPLY_CHA:
		ch->mod_cha += mod;
		break;
	case APPLY_LCK:
		ch->mod_lck += mod;
		break;
	case APPLY_SEX:
		ch->sex = (ch->sex + mod) % 3;
		if (ch->sex < 0)
			ch->sex += 2;
		ch->sex = URANGE(0, ch->sex, 2);
		break;
	case APPLY_HEIGHT:
		ch->height += mod;
		break;
	case APPLY_WEIGHT:
		ch->weight += mod;
		break;
	case APPLY_MANA:
		ch->max_mana += mod;
		break;
	case APPLY_HIT:
		ch->max_hit += mod;
		break;
	case APPLY_MOVE:
		ch->max_move += mod;
		break;
	case APPLY_AC:
		ch->armor += mod;
		break;
	case APPLY_HITROLL:
		ch->hitroll += mod;
		break;
	case APPLY_DAMROLL:
		ch->damroll += mod;
		break;
	case APPLY_SAVING_POISON:
		ch->saving_poison_death += mod;
		break;
	case APPLY_SAVING_ROD:
		ch->saving_wand += mod;
		break;
	case APPLY_SAVING_PARA:
		ch->saving_para_petri += mod;
		break;
	case APPLY_SAVING_BREATH:
		ch->saving_breath += mod;
		break;
	case APPLY_SAVING_SPELL:
		ch->saving_spell_staff += mod;
		break;
	case APPLY_AFFECT:
		SET_BIT(ch->affected_by, mod);
		break;
	case APPLY_RESISTANT:
		SET_BIT(ch->resistant, mod);
		break;
	case APPLY_IMMUNE:
		SET_BIT(ch->immune, mod);
		break;
	case APPLY_SUSCEPTIBLE:
		SET_BIT(ch->susceptible, mod);
		break;
	case APPLY_WEAPONSPELL: /* see fight.c */
		break;
	case APPLY_REMOVE:
		REMOVE_BIT(ch->affected_by, mod);
		break;

	case APPLY_FULL:
		if (!IS_NPC(ch))
			ch->pcdata->condition[COND_FULL] = URANGE(0, ch->pcdata->condition[COND_FULL] + mod, COND_MAX);
		break;

	case APPLY_THIRST:
		if (!IS_NPC(ch))
			ch->pcdata->condition[COND_THIRST] = URANGE(0, ch->pcdata->condition[COND_THIRST] + mod, COND_MAX);
		break;

	case APPLY_DRUNK:
		if (!IS_NPC(ch))
			ch->pcdata->condition[COND_DRUNK] = URANGE(0, ch->pcdata->condition[COND_DRUNK] + mod, 48);
		break;

	case APPLY_MENTALSTATE:
		ch->mental_state = URANGE(-100, ch->mental_state + mod, 100);
		break;
	case APPLY_EMOTION:
		ch->emotional_state = URANGE(-100, ch->emotional_state + mod, 100);
		break;

	case APPLY_STRIPSN:
		if (IS_VALID_SN(mod))
			affect_strip(ch, mod);
		else
			bug("APPLY_STRIPSN invalid sn %d", mod);
		break;

		/* spell cast upon wear/removal of an object	-Thoric */
	case APPLY_WEARSPELL:
	case APPLY_REMOVESPELL:
		if ( IS_SET(ch->in_room->room_flags, ROOM_NO_FORCE) || IS_SET(ch->immune, RIS_FORCE) || saving_char == ch /* so save/quit doesn't trigger */
		|| loading_char == ch) /* so loading doesn't trigger */
			return;

		mod = abs(mod);
		if ( IS_VALID_SN(mod) && (skill = skill_table[mod]) != NULL && skill->type == SKILL_SPELL)
			if ((skill->spell_fun && (retcode = (*skill->spell_fun)(mod, ch->skill_level[FORCE_ABILITY], ch, ch)) == rCHAR_DIED)
					|| char_died(ch))
				return;
		break;

		/* skill apply types	-Thoric */
	case APPLY_TRACK:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_track] > 0)
			ch->pcdata->learned[gsn_track] = UMAX(1, ch->pcdata->learned[gsn_track] + mod);
		break;
	case APPLY_HIDE:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_hide] > 0)
			ch->pcdata->learned[gsn_hide] = UMAX(1, ch->pcdata->learned[gsn_hide] + mod);
		break;
	case APPLY_STEAL:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_steal] > 0)
			ch->pcdata->learned[gsn_steal] = UMAX(1, ch->pcdata->learned[gsn_steal] + mod);
		break;
	case APPLY_SNEAK:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_sneak] > 0)
			ch->pcdata->learned[gsn_sneak] = UMAX(1, ch->pcdata->learned[gsn_sneak] + mod);
		break;
	case APPLY_SNIPE:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_snipe] > 0)
			ch->pcdata->learned[gsn_snipe] = UMAX(1, ch->pcdata->learned[gsn_snipe] + mod);
		break;
	case APPLY_PICK:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_pick_lock] > 0)
			ch->pcdata->learned[gsn_pick_lock] = UMAX(1, ch->pcdata->learned[gsn_pick_lock] + mod);
		break;
	case APPLY_BACKSTAB:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_backstab] > 0)
			ch->pcdata->learned[gsn_backstab] = UMAX(1, ch->pcdata->learned[gsn_backstab] + mod);
		break;
	case APPLY_DETRAP:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_detrap] > 0)
			ch->pcdata->learned[gsn_detrap] = UMAX(1, ch->pcdata->learned[gsn_detrap] + mod);
		break;
	case APPLY_DODGE:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_dodge] > 0)
			ch->pcdata->learned[gsn_dodge] = UMAX(1, ch->pcdata->learned[gsn_dodge] + mod);
		break;
	case APPLY_PEEK:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_peek] > 0)
			ch->pcdata->learned[gsn_peek] = UMAX(1, ch->pcdata->learned[gsn_peek] + mod);
		break;
	case APPLY_SCAN:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_scan] > 0)
			ch->pcdata->learned[gsn_scan] = UMAX(1, ch->pcdata->learned[gsn_scan] + mod);
		break;
	case APPLY_GOUGE:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_gouge] > 0)
			ch->pcdata->learned[gsn_gouge] = UMAX(1, ch->pcdata->learned[gsn_gouge] + mod);
		break;
	case APPLY_SEARCH:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_search] > 0)
			ch->pcdata->learned[gsn_search] = UMAX(1, ch->pcdata->learned[gsn_search] + mod);
		break;
	case APPLY_DIG:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_dig] > 0)
			ch->pcdata->learned[gsn_dig] = UMAX(1, ch->pcdata->learned[gsn_dig] + mod);
		break;
	case APPLY_MOUNT:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_mount] > 0)
			ch->pcdata->learned[gsn_mount] = UMAX(1, ch->pcdata->learned[gsn_mount] + mod);
		break;
	case APPLY_DISARM:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_disarm] > 0)
			ch->pcdata->learned[gsn_disarm] = UMAX(1, ch->pcdata->learned[gsn_disarm] + mod);
		break;
	case APPLY_KICK:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_kick] > 0)
			ch->pcdata->learned[gsn_kick] = UMAX(1, ch->pcdata->learned[gsn_kick] + mod);
		break;
	case APPLY_PARRY:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_parry] > 0)
			ch->pcdata->learned[gsn_parry] = UMAX(1, ch->pcdata->learned[gsn_parry] + mod);
		break;
	case APPLY_BASH:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_bash] > 0)
			ch->pcdata->learned[gsn_bash] = UMAX(1, ch->pcdata->learned[gsn_bash] + mod);
		break;
	case APPLY_STUN:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_stun] > 0)
			ch->pcdata->learned[gsn_stun] = UMAX(1, ch->pcdata->learned[gsn_stun] + mod);
		break;
	case APPLY_PUNCH:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_punch] > 0)
			ch->pcdata->learned[gsn_punch] = UMAX(1, ch->pcdata->learned[gsn_punch] + mod);
		break;
	case APPLY_CLIMB:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_climb] > 0)
			ch->pcdata->learned[gsn_climb] = UMAX(1, ch->pcdata->learned[gsn_climb] + mod);
		break;
	case APPLY_GRIP:
		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_grip] > 0)
			ch->pcdata->learned[gsn_grip] = UMAX(1, ch->pcdata->learned[gsn_grip] + mod);
		break;
	}

	/*
	 * Check for weapon wielding.
	 * Guard against recursion (for weapons with affects).
	 */
	if (!IS_NPC(ch) && saving_char != ch && (wield = get_eq_char(ch, WEAR_WIELD)) != NULL
			&& get_obj_weight(wield) > str_app[get_curr_str(ch)].wield)
	{
		static int depth;

		if (depth == 0)
		{
			depth++;
			act( COL_ACTION, "Nie masz ju� si�y by d�wiga� $p$3.", ch, wield, NULL, TO_CHAR);
			act( COL_ACTION, "$n przestaje u�ywa� $p$3.", ch, wield, NULL,
			TO_ROOM);
			unequip_char(ch, wield);
			depth--;
		}
	}

	return;
}

/*
 * Give an affect to a char.
 */
void affect_to_char(CHAR_DATA *ch, AFFECT_DATA *paf)
{
	AFFECT_DATA *paf_new;

	IF_BUG(ch == NULL, "")
		return;

	IF_BUG(paf == NULL, "(ch:%s)", ch->name)
		return;

	CREATE(paf_new, AFFECT_DATA, 1);
	ch->affects.push_back(paf_new);
	paf_new->type = paf->type;
	paf_new->duration = paf->duration;
	paf_new->location = paf->location;
	paf_new->modifier = paf->modifier;
	paf_new->bitvector = paf->bitvector;

	affect_modify(ch, paf_new, true);
	return;
}

/*
 * Remove an affect from a char.
 */
void affect_remove(CHAR_DATA *ch, AFFECT_DATA *paf)
{
	IF_BUG(ch->affects.empty(), "(ch:%s)", ch->name)
		return;

	affect_modify(ch, paf, false);

	ch->affects.remove(paf);
	DISPOSE(paf);
	return;
}

/*
 * Strip all affects of a given sn.
 */
void affect_strip(CHAR_DATA *ch, int sn)
{
	for (auto it = ch->affects.begin(); it != ch->affects.end(); )
	{
		AFFECT_DATA *paf = *it;
		++it;
		if (paf->type == sn)
			affect_remove(ch, paf);
	}

	return;
}

/*
 * Return true if a char is affected by a spell.
 */
bool is_affected(CHAR_DATA *ch, int sn)
{
	for (auto* paf : ch->affects)
		if (paf->type == sn)
			return true;

	return false;
}

/*
 * Add or enhance an affect.
 * Limitations put in place by Thoric, they may be high... but at least
 * they're there :)
 */
void affect_join(CHAR_DATA *ch, AFFECT_DATA *paf)
{
	AFFECT_DATA *paf_old = nullptr;

	for (auto* a : ch->affects)
		if (a->type == paf->type)
		{
			paf_old = a;
			break;
		}
	if (paf_old)
	{
		paf->duration = UMIN(1000000, paf->duration + paf_old->duration);
		if (paf->modifier)
			paf->modifier = UMIN(5000, paf->modifier + paf_old->modifier);
		else
			paf->modifier = paf_old->modifier;
		affect_remove(ch, paf_old);
	}

	affect_to_char(ch, paf);
	return;
}

/*
 * Move a char out of a room.
 */
void char_from_room(CHAR_DATA *ch)
{
	OBJ_DATA *obj;

	IF_BUG(ch == NULL, "")
		return;

	if (!ch->in_room)
	{
//	if( ch != supermob ) /* Trog */
		//bug( "char_from_room: NULL room: %s (hp: %d/%d)", ch->name, ch->hit, ch->max_hit ); bo wkurzalo
		return;
	}

	if (!IS_NPC(ch))
		--ch->in_room->area->nplayer;
	if ((obj = get_eq_char(ch, WEAR_LIGHT)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room->light > 0)
		--ch->in_room->light;

	ch->in_room->people.remove(ch);

	/*Jesli opuszcza swoje mieszkanie to wypraszamy go�ci - Ganis*/
	if (ch->plr_home && ch->plr_home == ch->in_room)
		evacuate_guests(ch->plr_home);

	ch->in_room = NULL;

	if (!IS_NPC(ch) && get_timer(ch, TIMER_SHOVEDRAG) > 0)
		remove_timer(ch, TIMER_SHOVEDRAG);

	if (!IS_NPC(ch) && ch->pcdata && ch->pcdata->hotel_safe_time > 0)
		ch->pcdata->hotel_safe_time = 0;

	return;
}

/*
 * Move a char into a room.
 */
void char_to_room(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex)
{
	OBJ_DATA *obj;

	IF_BUG(ch == NULL, "")
		return;

	if (!pRoomIndex)
	{
		bug("Char_to_room: %s -> NULL room!  Putting char in limbo (%d)", ch->name, ROOM_VNUM_LIMBO);
		/* This used to just return, but there was a problem with crashing
		 and I saw no reason not to just put the char in limbo. -Narn */
		pRoomIndex = get_room_index( ROOM_VNUM_LIMBO);
	}

	/*
	 * Trog: to (for) musi byc, bo przy script_prog'ach supermob ladowal
	 *       sie n razy do listy a przy 2 razie juz byl pad muda.
	 */

	ch->in_room = pRoomIndex;

	if (ch == supermob)
		for (auto* vch : pRoomIndex->people)
			if (vch == ch)
				return;

	pRoomIndex->people.push_back(ch);

	if (!IS_NPC(ch))
	{
		if (++ch->in_room->area->nplayer > ch->in_room->area->max_players)
			ch->in_room->area->max_players = ch->in_room->area->nplayer;
	}

	if ((obj = get_eq_char(ch, WEAR_LIGHT)) != NULL && obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
		++ch->in_room->light;

	if (!IS_NPC(ch) && IS_SET(ch->in_room->room_flags, ROOM_SAFE) && get_timer(ch, TIMER_SHOVEDRAG) <= 0)
		add_timer(ch, TIMER_SHOVEDRAG, 10, NULL, 0); /*-30 Seconds-*/

	/*
	 * Delayed Teleport rooms					-Thoric
	 * Should be the last thing checked in this function
	 */
	if ( IS_SET(ch->in_room->room_flags, ROOM_TELEPORT) && ch->in_room->tele_delay > 0)
	{
		for (auto* tele : teleport_list)
			if (tele->room == pRoomIndex)
				return;

		TELEPORT_DATA *tele;
		CREATE(tele, TELEPORT_DATA, 1);
		teleport_list.push_back(tele);
		tele->room = pRoomIndex;
		tele->timer = pRoomIndex->tele_delay;
	}
	return;
}

void obj_personalize(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (IS_NPC(ch))
		return;

	if (!IS_OBJ_STAT(obj, ITEM_PERSONAL))
		return;

	if (loading_char != ch)
		ch_printf(ch, "Czujesz, �e pokocha�%s� %s, ju� nigdy si� nie rozstaniecie!" NL, SEX_SUFFIX_EAE(ch), obj->przypadki[3]);

	STRDUP(obj->owner_name, ch->name);
	return;
}

/*
 * Give an obj to a char.
 */
OBJ_DATA* obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch)
{
	OBJ_DATA *oret = obj;
	bool skipgroup, grouped;
	int oweight = get_obj_weight(obj);
	int onum = get_obj_number(obj);
	int wear_loc = obj->wear_loc;
	int extra_flags = obj->extra_flags;

	skipgroup = false;
	grouped = false;

	if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
	{
		if (!IS_IMMORTAL(ch) && (IS_NPC(ch) && !IS_SET(ch->act, ACT_PROTOTYPE)))
			return obj_to_room(obj, ch->in_room);
	}

	/* crimes.c -- Thanos */
	if (is_deposit(obj))
	{
		CHAR_DATA *owner;
		PLANET_DATA *planet = 0;
		CRIME_DATA *crime;

		owner = deposit_owner(obj);

		if (!owner)
			bug("couldn't find the valid owner for %s", obj->przypadki[0]);

		if (ch != owner)
		{
			if (ch->in_room && ch->in_room->area && ch->in_room->area->planet)
				planet = ch->in_room->area->planet;

			if (!planet)
			{
				bug("Guy trying to steal deposit, and jail has no PLANET !!! (ch:%s)", ch->name);
				planet = planet_list.front();
			}

			if ((crime = find_crime(ch, planet)) != NULL)
				ch_printf(ch, FG_YELLOW
				"Pope�niaj�c kradzie�e na %s pogarszasz swoj� sytuacj�." EOL, capitalize(planet->name));
			else
				ch_printf(ch, FG_YELLOW
				"Jeste� teraz poszukiwan%s na %s." EOL,
				MALE( ch ) ? "y" : FEMALE(ch) ? "a" : "e", capitalize(planet->name));
			crime_to_char(ch, planet->name, CRIME_STEAL);
		}

		ch_printf(ch, "Dziwnym trafem %s wy�lizguje ci si� z r�k i spada na ziemi�" NL
		" rozpadaj�c si� na kawa�eczki..." NL, obj->przypadki[0]);

		if (!obj->contents.empty())
			empty_obj(obj, NULL, ch->in_room);

		if (owner)
		{
			destroy_deposit(owner);
			save_char_obj(owner);
		}
		return NULL;
	}

	/* personale -- Thanos */
	if (IS_OBJ_STAT( obj, ITEM_PERSONAL ) && !IS_NPC(ch))
	{
		if (obj->owner_name[0] == '\0')
			obj_personalize(ch, obj);
		else if (str_cmp(obj->owner_name, ch->name))
		{
			/* Trog: BUG: zapetla sie, jak ktos zrobi get all, to wezmie,
			 * wyrzuci (czyli idzie na koniec listy) i spowrotem wezmie i... */
			char buf[MSL];

			ch_printf(ch, "To chyba nie twoje. Spad�wa!" NL);

			sprintf(buf, "%s" PLAIN " upad%s na ziemi�.", obj->przypadki[0], obj->gender > GENDER_FEMALE ? "aj�" : "a");
			act( PLAIN, buf, ch, NULL, NULL, TO_ROOM);
			act( PLAIN, buf, ch, obj, NULL, TO_CHAR);

			return obj_to_room(obj, ch->in_room);
		}
	} /* reszta oka */

	if (loading_char == ch)
	{
		int x, y;
		for (x = 0; x < MAX_WEAR; x++)
			for (y = 0; y < MAX_LAYERS; y++)
				if (save_equipment[x][y] == obj)
				{
					skipgroup = true;
					break;
				}
	}

	if (!skipgroup)
		for (auto* otmp : ch->carrying)
			if ((oret = group_object(otmp, obj)) == otmp)
			{
				grouped = true;
				break;
			}

	if (!grouped)
	{
		ch->carrying.push_back(obj);
		obj->carried_by = ch;
		obj->in_room = NULL;
		obj->in_obj = NULL;
	}
	if (wear_loc == WEAR_NONE)
	{
		ch->carry_number += onum;
		ch->carry_weight += oweight;
	}
	else if (!IS_SET(extra_flags, ITEM_FORCE))
		ch->carry_weight += oweight;
	/* Thanos: questy */
	quest_trigger(ch, EVENT_FIND_OBJ, obj, NULL);
	return (oret ? oret : obj);
}

/*
 * Take an obj from its character.
 */
void obj_from_char(OBJ_DATA *obj)
{
	CHAR_DATA *ch;

	IF_BUG(obj == NULL, "")
		return;

	ch = obj->carried_by;
	IF_BUG(ch == NULL, "(obj:%d)", obj->pIndexData->vnum)
		return;

	if (obj->wear_loc != WEAR_NONE)
		unequip_char(ch, obj);

	/* obj may drop during unequip... */
	if (!obj->carried_by)
		return;

	ch->carrying.remove(obj);

	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) && !obj->contents.empty())
		empty_obj(obj, NULL, NULL);

	obj->in_room = NULL;
	obj->carried_by = NULL;
	ch->carry_number -= get_obj_number(obj);
	ch->carry_weight -= get_obj_weight(obj);
	return;
}

/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac(OBJ_DATA *obj, int iWear)
{
	if (obj->item_type != ITEM_ARMOR)
		return 0;

	if (obj->value[0] < 0)
		return 0;

	switch (iWear)
	{
	case WEAR_BODY:
		return 3 * obj->value[0];
	case WEAR_HEAD:
		return 2 * obj->value[0];
	case WEAR_LEGS:
		return 2 * obj->value[0];
	case WEAR_FEET:
		return obj->value[0];
	case WEAR_HANDS:
		return obj->value[0];
	case WEAR_ARMS:
		return obj->value[0];
	case WEAR_SHIELD:
		return obj->value[0];
	case WEAR_FINGER_L:
		return obj->value[0];
	case WEAR_FINGER_R:
		return obj->value[0];
	case WEAR_NECK_1:
		return obj->value[0];
	case WEAR_NECK_2:
		return obj->value[0];
	case WEAR_ABOUT:
		return 2 * obj->value[0];
	case WEAR_WAIST:
		return obj->value[0];
	case WEAR_WRIST_L:
		return obj->value[0];
	case WEAR_WRIST_R:
		return obj->value[0];
	case WEAR_HOLD:
		return obj->value[0];
	case WEAR_EYES:
		return obj->value[0];
	}

	return 0;
}

/*
 * Find a piece of eq on a character.
 * Will pick the top layer if clothing is layered.		-Thoric
 */
OBJ_DATA* get_eq_char(CHAR_DATA *ch, int iWear)
{
	OBJ_DATA *maxobj = NULL;

	for (auto* obj : ch->carrying)
		if (obj->wear_loc == iWear)
		{
			if (!obj->pIndexData->layers)
				return obj;
			else if (!maxobj || obj->pIndexData->layers > maxobj->pIndexData->layers)
				maxobj = obj;
		}
	return maxobj;
}

/*
 * Equip a char with an obj.
 */
void equip_char(CHAR_DATA *ch, OBJ_DATA *obj, int iWear)
{
	OBJ_DATA *otmp;

	IF_BUG(ch == NULL, "")
		return;

	IF_BUG(obj == NULL, "ch=%s", ch->name)
		return;

	if ((otmp = get_eq_char(ch, iWear)) != NULL && (!otmp->pIndexData->layers || !obj->pIndexData->layers))
	{
		bug("already equipped (%d) (ch=%s)(obj=%s)(inRoom=%d).", iWear, ch->name, obj->name, ch->in_room->vnum);
		return;
	}

	separate_obj(obj); /* just in case */
	if (( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch)) || ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch))
			|| ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch)))
	{
		/*
		 * Thanks to Morgenes for the bug fix here!
		 */
		if (loading_char != ch)
		{
			act( COL_FORCE, "You are zapped by $p and drop it.", ch, obj, NULL,
			TO_CHAR);
			act( COL_FORCE, "$n is zapped by $p and drops it.", ch, obj, NULL,
			TO_ROOM);
		}
		if (obj->carried_by)
			obj_from_char(obj);
		obj_to_room(obj, ch->in_room);
		oprog_zap_trigger(ch, obj);
		if ( IS_SET(sysdata.save_flags, SV_ZAPDROP) && !char_died(ch))
			save_char_obj(ch);
		return;
	}

	ch->armor -= apply_ac(obj, iWear);
	obj->wear_loc = iWear;

	ch->carry_number -= get_obj_number(obj);
	if (IS_SET(obj->extra_flags, ITEM_FORCE))
		ch->carry_weight -= get_obj_weight(obj);

	for (auto* paf : obj->pIndexData->affects)
		affect_modify(ch, paf, true);
	for (auto* paf : obj->affects)
		affect_modify(ch, paf, true);

	if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room)
		++ch->in_room->light;

	if (IS_RACE( ch, "DEFEL" ) && isDefelVisible(obj))
	{
		REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
	}

	return;
}

/*
 * Unequip a char with an obj.
 */
void unequip_char(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->wear_loc == WEAR_NONE)
	{
		bug("already unequipped.", 0);
		return;
	}

	ch->carry_number += get_obj_number(obj);
	if (IS_SET(obj->extra_flags, ITEM_FORCE))
		ch->carry_weight += get_obj_weight(obj);

	ch->armor += apply_ac(obj, obj->wear_loc);
	obj->wear_loc = WEAR_NONE;

	for (auto* paf : obj->pIndexData->affects)
		affect_modify(ch, paf, false);
	if (obj->carried_by)
		for (auto* paf : obj->affects)
			affect_modify(ch, paf, false);

	if (isDefelInvisible(ch))
	{
		SET_BIT(ch->affected_by, AFF_INVISIBLE);
	}
	else
	{
		REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
	}

	if (!obj->carried_by)
		return;

	if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0 && ch->in_room && ch->in_room->light > 0)
		--ch->in_room->light;

	return;
}

/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list(OBJ_INDEX_DATA *pObjIndex, const std::list<OBJ_DATA*>& list)
{
	int nMatch;

	nMatch = 0;
	for (auto* obj : list)
		if (obj->pIndexData == pObjIndex)
			nMatch++;

	return nMatch;
}

/*
 * Move an obj out of a room.
 */
void write_corpses args( ( CHAR_DATA *ch, char *name ) );

int falling;

void obj_from_room(OBJ_DATA *obj)
{
	ROOM_INDEX_DATA *in_room;

	IF_BUG(obj == NULL, "")
		return;

	in_room = obj->in_room;
	IF_BUG(in_room == NULL, "(obj:%d)", obj->pIndexData->vnum)
		return;

	in_room->contents.remove(obj);

	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) && !obj->contents.empty())
		empty_obj(obj, NULL, obj->in_room);

	if (IS_OBJ_STAT(obj, ITEM_GLOW))   // Thanos: glow dzia�a na �wiat�o
		obj->in_room->light -= obj->count;

	obj->carried_by = NULL;
	obj->in_obj = NULL;
	obj->in_room = NULL;

	if (obj->pIndexData->vnum == OBJ_VNUM_CORPSE_PC && falling == 0)
		write_corpses(NULL, obj->action_desc);
	return;
}

/*
 * Move an obj into a room.
 */
OBJ_DATA* obj_to_room(OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex)
{
	OBJ_DATA *oret;
	int count = obj->count;

	for (auto* otmp : pRoomIndex->contents)
		if ((oret = group_object(otmp, obj)) == otmp)
			return oret;

	pRoomIndex->contents.push_back(obj);

	obj->in_room = pRoomIndex;
	obj->carried_by = NULL;
	obj->in_obj = NULL;

	if (IS_OBJ_STAT(obj, ITEM_GLOW))   // Thanos: glow dzia�a na �wiat�o
		pRoomIndex->light += count;

	falling++;
	obj_fall(obj, false);
	falling--;

	if (obj->pIndexData->vnum == OBJ_VNUM_CORPSE_PC && falling == 0)
		write_corpses(NULL, obj->action_desc);

	return obj;
}

/*
 * Move an object into an object.
 */
OBJ_DATA* obj_to_obj(OBJ_DATA *obj, OBJ_DATA *obj_to)
{
	OBJ_DATA *oret;

	IF_BUG(obj == NULL, "")
		return NULL;

	IF_BUG(obj_to == NULL, "(obj:%d)", obj->pIndexData->vnum)
		return obj;

	IF_BUG(obj_to == obj, "(obj:%d)", obj->pIndexData->vnum)
		return obj;

	/* Big carry_weight bug fix here by Thoric */
	if (obj->carried_by != obj_to->carried_by)
	{
		if (obj->carried_by)
			obj->carried_by->carry_weight -= get_obj_weight(obj);
		if (obj_to->carried_by)
			obj_to->carried_by->carry_weight += get_obj_weight(obj);
	}

	for (auto* otmp : obj_to->contents)
		if ((oret = group_object(otmp, obj)) == otmp)
			return oret;

	obj_to->contents.push_back(obj);
	obj->in_obj = obj_to;
	obj->in_room = NULL;
	obj->carried_by = NULL;

	return obj;
}

/*
 * Move an object out of an object.
 */
void obj_from_obj(OBJ_DATA *obj)
{
	OBJ_DATA *obj_from;

	IF_BUG(obj == NULL, "")
		return;

	obj_from = obj->in_obj;
	IF_BUG(obj_from == NULL, "(obj:%d)", obj->pIndexData->vnum)
		return;

	obj_from->contents.remove(obj);

	if ( IS_OBJ_STAT( obj, ITEM_COVERING ) && !obj->contents.empty())
		empty_obj(obj, obj->in_obj, NULL);

	obj->in_obj = NULL;
	obj->in_room = NULL;
	obj->carried_by = NULL;

	for (; obj_from; obj_from = obj_from->in_obj)
		if (obj_from->carried_by)
			obj_from->carried_by->carry_weight -= get_obj_weight(obj);

	return;
}

/*
 * Extract an obj from the world.
 */
void extract_obj(OBJ_DATA *obj)
{
	OBJ_DATA *obj_content;
	OBJ_DATA *inobj = 0;
	CHAR_DATA *carry;
	ROOM_INDEX_DATA *room = 0;

	IF_BUG(obj == NULL, "")
		return;

	if (obj_extracted(obj))
	{
		bug("extract_obj: obj %d already extracted!", obj->pIndexData->vnum);
		return;
	}

	if (obj->item_type == ITEM_PORTAL)
		remove_portal(obj);

	if ((carry = obj->carried_by))
		obj_from_char(obj);
	else if ((room = obj->in_room))
		obj_from_room(obj);
	else if ((inobj = obj->in_obj))
		obj_from_obj(obj);

	while (!obj->contents.empty())
	{
		obj_content = obj->contents.back();
		/* Je�li przedmiot by� questowy, 'wypada' z pojemnika  -- Thanos */
		if (obj_content->inquest)
		{
			obj_from_obj(obj_content);
			if (room)
				obj_to_room(obj_content, room);
			else if (carry)
				obj_to_char(obj_content, carry);
			else if (inobj)
				obj_to_obj(obj_content, inobj);
			else
				/* na wszelki... */
				obj_to_room(obj_content, Quest_Master->in_room);
		}
		else
			extract_obj(obj_content);
	}

	{
		for (auto* paf : obj->affects)
		{
			DISPOSE(paf);
		}
		obj->affects.clear();
	}

	{
		for (auto* req : obj->requirements)
		{
			DISPOSE(req);
		}
		obj->requirements.clear();
	}

	{
		for (auto* ed : obj->extradesc)
		{
			free_ed(ed);
		}
		obj->extradesc.clear();
	}

	/* Thanos 	- quest cleanup */
	if (obj->inquest)
	{
		QUEST_DATA *quest = obj->inquest;

		for (auto* qObj : quest->objs)
			if (qObj->obj && qObj->obj == obj)
			{
				quest->objs.remove(qObj);
				DISPOSE(qObj);
				break;
			}
	}
	/* Thanos 	- script cleanup */
	if (obj->mpscriptrun)
	{
		auto scripts_snapshot = script_prog_list;
		for (auto* script : scripts_snapshot)
		{
			if (script->o_owner && script->o_owner == obj)
				stopscript(script);
		}
	}

	/* Thanos -- personale */

	object_list.remove(obj);
	/* shove onto extraction queue */
	queue_extracted_obj(obj);

	obj->pIndexData->count -= obj->count;
	numobjsloaded -= obj->count;
	--physicalobjects;
	if (obj->serial == cur_obj)
	{
		cur_obj_extracted = true;
		if (global_objcode == rNONE)
			global_objcode = rOBJ_EXTRACTED;
	}
	return;
}

/*
 * Extract a char from the world.
 */
void extract_char(CHAR_DATA *ch, bool fPull)
{
	ROOM_INDEX_DATA *location;

	IF_BUG(ch == NULL, "")
		return;

	IF_BUG(ch == supermob, "Uff.. survived ;)")
		return;

	IF_BUG(ch == Quest_Master, "Uff.. survived ;)")
		return;

	IF_BUG(ch->in_room == NULL, "no in_room")
		return;

	if (char_died(ch))
	{
		bug("%s already died!", ch->name);
		return;
	}

	if (!ch->suspects.empty())
	{
		auto suspects_snapshot = ch->suspects;
		for (auto* sus : suspects_snapshot)
		{
			free_suspect(ch, sus);
		}
	}
	/* Thanos 	- quest cleanup */
	if (ch->inquest)
	{
		if (IS_NPC(ch))
		{
			QUEST_DATA *quest = ch->inquest;

			for (auto* qMob : quest->mobs)
				if (qMob->mob && qMob->mob == ch)
				{
					quest->mobs.remove(qMob);
					DISPOSE(qMob);
					break;
				}
		}
		else
			quest_cleanup(ch, true);
	}

	/* Thanos 	- script cleanup */
	{
		auto scripts_snapshot = script_prog_list;
		for (auto* script : scripts_snapshot)
		{
			if ((script->m_owner && script->m_owner == ch) || (script->victim && script->victim == ch))
				stopscript(script);
		}
	}

	if (ch == cur_char)
		cur_char_died = true;
	/* shove onto extraction queue */
	queue_extracted_char(ch, fPull);

	if (fPull && !IS_SET(ch->act, ACT_POLYMORPHED))
		die_follower(ch);

	if (ch->fighting)
		stop_fighting(ch, true);

	if (ch->mount)
	{
		REMOVE_BIT(ch->mount->act, ACT_MOUNTED);
		ch->mount = NULL;
		ch->position = POS_STANDING;
	}

	if ( IS_NPC(ch) && IS_SET(ch->act, ACT_MOUNTED))
		for (auto* wch : char_list)
			if (wch->mount == ch)
			{
				wch->mount = NULL;
				wch->position = POS_STANDING;
			}

	REMOVE_BIT(ch->act, ACT_MOUNTED);

	while (!ch->carrying.empty())
		extract_obj(ch->carrying.back());

	if (!IS_NPC(ch) && ch->deposit)
	{
		extract_obj(ch->deposit);
	}

	char_from_room(ch);

	if (!fPull)
	{
		location = NULL;

		if (!location)
			location = get_room_index(wherehome(ch));

		if (!location)
			location = get_room_index(1);

		char_to_room(ch, location);

		act( COL_FORCE, "$n wy�ania si� z wiruj�cej mg�y!", ch, NULL, NULL,
		TO_ROOM);
		ch->position = POS_RESTING;
		return;
	}

	if (IS_NPC(ch))
	{
		--ch->pIndexData->count;
		--nummobsloaded;
	}
	/* Trog: a fe! :)
	 if ( ch->desc && ch->desc->original && IS_SET(ch->act, ACT_POLYMORPHED))
	 do_revert( ch, "" );
	 */
	if (ch->desc && ch->desc->original)
		do_return(ch, (char*) "");

	for (auto* wch : char_list)
		if (wch->reply == ch)
			wch->reply = NULL;
	//added by Thanos
	for (auto* wch : char_list)
		if (wch->retell == ch)
			wch->retell = NULL;

	char_list.remove(ch);

	if (ch->desc)
	{
		if (ch->desc->character != ch)
			bug("char's descriptor points to another char", 0);
		else
		{
			ch->desc->character = NULL;
			close_socket(ch->desc, false);
			ch->desc = NULL;
		}
	}
	return;
}

/*
 * Find a char in the room.
 */
CHAR_DATA* get_char_room(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	int number, count, vnum;
	char *name = NULL;

	number = number_argument(argument, arg);
	if (!str_cmp(arg, "self"))
		return ch;

	if (get_trust(ch) >= LEVEL_SAVIOR && is_number(arg))
		vnum = atoi(arg);
	else
		vnum = -1;

	count = 0;

	for (auto* rch : ch->in_room->people)
	{
		const SWString &attrib = format_char_attribute(rch, 0);
		if (IS_NPC(ch) || IS_NPC(rch) || does_knows(ch, rch))
			name = rch->name;
		else
			name = (char*) attrib.c_str();
		if (can_see(ch, rch)
				&& ((nifty_is_name(arg, name) || (IS_NPC(rch) && vnum == rch->pIndexData->vnum))
						|| (rch->pcdata && rch != ch && IS_AFFECTED(rch, AFF_DISGUISE) && nifty_is_name_prefix(arg, rch->pcdata->fake_name))))
		{
			if (number == 0 && !IS_NPC(rch))
				return rch;
			else if (++count == number)
				return rch;
		}
	}

	if (vnum != -1)
		return NULL;

	/* If we didn't find an exact match, run through the list of characters
	 again looking for prefix matching, ie gu == guard.
	 Added by Narn, Sept/96
	 */
	count = 0;
	for (auto* rch : ch->in_room->people)
	{
		const SWString &attrib = format_char_attribute(rch, 0);
		if (IS_NPC(ch) || IS_NPC(rch) || does_knows(ch, rch))
			name = rch->name;
		else
			name = (char*) attrib.c_str();
		if (!can_see(ch, rch) || !nifty_is_name_prefix(arg, name))
			continue;
		if (number == 0 && !IS_NPC(rch))
			return rch;
		else if (++count == number)
			return rch;
	}

	return NULL;
}

/*
 * Find a char in the world.
 */
CHAR_DATA* get_char_world(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	int number, count, vnum;
	char *name = NULL;

	number = number_argument(argument, arg);
	count = 0;
	if (!str_cmp(arg, "self"))
		return ch;

	/*
	 * Allow reference by vnum for saints+			-Thoric
	 */
	if (get_trust(ch) >= LEVEL_SAVIOR && is_number(arg))
		vnum = atoi(arg);
	else
		vnum = -1;

	/* check the room for an exact match */
	for (auto* wch : ch->in_room->people)
	{
		const SWString &attrib = format_char_attribute(wch, 0);
		if (IS_NPC(ch) || IS_NPC(wch) || does_knows(ch, wch))
			name = wch->name;
		else
			name = (char*) attrib.c_str();
		if ((nifty_is_name(arg, name) || (IS_NPC(wch) && vnum == wch->pIndexData->vnum)) && is_wizvis(ch, wch))
		{
			if (number == 0 && !IS_NPC(wch))
				return wch;
			else if (++count == number)
				return wch;
		}
	}

	count = 0;

	/* check the world for an exact match */
	for (auto* wch : char_list)
	{
		const SWString &attrib = format_char_attribute(wch, 0);
		if (IS_NPC(ch) || IS_NPC(wch) || does_knows(ch, wch))
			name = wch->name;
		else
			name = (char*) attrib.c_str();
		if ((nifty_is_name(arg, name) || (IS_NPC(wch) && vnum == wch->pIndexData->vnum)) && is_wizvis(ch, wch))
		{
			if (number == 0 && !IS_NPC(wch))
				return wch;
			else if (++count == number)
				return wch;
		}
	}

	/* bail out if looking for a vnum match */
	if (vnum != -1)
		return NULL;

	/*
	 * If we didn't find an exact match, check the room for
	 * for a prefix match, ie gu == guard.
	 * Added by Narn, Sept/96
	 */
	count = 0;
	for (auto* wch : ch->in_room->people)
	{
		const SWString &attrib = format_char_attribute(wch, 0);
		if (IS_NPC(ch) || IS_NPC(wch) || does_knows(ch, wch))
			name = wch->name;
		else
			name = (char*) attrib.c_str();
		if (!nifty_is_name_prefix(arg, name))
			continue;
		if (number == 0 && !IS_NPC(wch) && is_wizvis(ch, wch))
			return wch;
		else if (++count == number && is_wizvis(ch, wch))
			return wch;
	}

	/*
	 * If we didn't find a prefix match in the room, run through the full list
	 * of characters looking for prefix matching, ie gu == guard.
	 * Added by Narn, Sept/96
	 */
	count = 0;
	for (auto* wch : char_list)
	{
		const SWString &attrib = format_char_attribute(wch, 0);
		if (IS_NPC(ch) || IS_NPC(wch) || does_knows(ch, wch))
			name = wch->name;
		else
			name = (char*) attrib.c_str();
		if (!nifty_is_name_prefix(arg, name))
			continue;
		if (number == 0 && !IS_NPC(wch) && is_wizvis(ch, wch))
			return wch;
		else if (++count == number && is_wizvis(ch, wch))
			return wch;
	}

	return NULL;
}

/*
 * Finds player in world 								- wojo
 * added support for global lookup
 */

//Trog: zle dzialalo, wiec poprawilem
CHAR_DATA* get_player_world(CHAR_DATA *ch, char *argument)
{
	CHAR_DATA *wch;
	char arg[MIL];
	int number, count;

	number = number_argument(argument, arg);

	if (ch && !str_cmp(arg, "self"))
		return ch;

	if (number == 0)
		number = 1;

	/* check the world for an exact match */
	if (number == 1)
	{
		for (auto* d : descriptor_list)
		{
			if (!(wch = CH(d)) || IS_NPC(wch))
				continue;

			const SWString &attrib = format_char_attribute(wch, 0);
			if ((ch) && !str_cmp(arg, does_knows(ch, wch) ? wch->name : attrib.c_str()))
				return wch;
		}
	}

	/* now the prefixes */
	count = 1;
	for (auto* d : descriptor_list)
	{
		if (!(wch = CH(d)) || IS_NPC(wch))
			continue;

		const SWString &attrib = format_char_attribute(wch, 0);
		if ((ch) && !str_prefix(arg, does_knows(ch, wch) ? wch->name : attrib.c_str()) && count == number)
			return wch;

		count++;
	}

	return NULL;
}

/*
 * Find some object with a given index data.
 * Used by area-reset 'P', 'T' and 'H' commands.
 */
OBJ_DATA* get_obj_type(OBJ_INDEX_DATA *pObjIndex)
{
	for (auto it = object_list.rbegin(); it != object_list.rend(); ++it)
		if ((*it)->pIndexData == pObjIndex)
			return *it;

	return NULL;
}

/*
 * Find an obj in a list.
 */
OBJ_DATA* get_obj_list(CHAR_DATA *ch, char *argument, const std::list<OBJ_DATA*>& list)
{
	char arg[MAX_INPUT_LENGTH];
	int number;
	int count;

	number = number_argument(argument, arg);
	count = 0;
	for (auto* obj : list)
		if (can_see_obj(ch, obj) && nifty_is_name(arg, obj->name))
			if ((count += obj->count) >= number)
				return obj;

	/* If we didn't find an exact match, run through the list of objects
	 again looking for prefix matching, ie swo == sword.
	 Added by Narn, Sept/96
	 */
	count = 0;
	for (auto* obj : list)
		if (can_see_obj(ch, obj) && nifty_is_name_prefix(arg, obj->name))
			if ((count += obj->count) >= number)
				return obj;

	return NULL;
}

/*
 * Find an obj in a list...going the other way			-Thoric
 */
OBJ_DATA* get_obj_list_rev(CHAR_DATA *ch, char *argument, std::list<OBJ_DATA*>& list)
{
	char arg[MAX_INPUT_LENGTH];
	int number;
	int count;

	number = number_argument(argument, arg);
	count = 0;
	for (auto it = list.rbegin(); it != list.rend(); ++it)
	{
		auto* obj = *it;
		if (can_see_obj(ch, obj) && nifty_is_name(arg, obj->name))
			if ((count += obj->count) >= number)
				return obj;
	}

	/* If we didn't find an exact match, run through the list of objects
	 again looking for prefix matching, ie swo == sword.
	 Added by Narn, Sept/96
	 */
	count = 0;
	for (auto it = list.rbegin(); it != list.rend(); ++it)
	{
		auto* obj = *it;
		if (can_see_obj(ch, obj) && nifty_is_name_prefix(arg, obj->name))
			if ((count += obj->count) >= number)
				return obj;
	}

	return NULL;
}

/*
 * Find an obj in player's inventory.
 */
OBJ_DATA* get_obj_carry(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int number, count, vnum;

	number = number_argument(argument, arg);
	if (get_trust(ch) >= LEVEL_SAVIOR && is_number(arg))
		vnum = atoi(arg);
	else
		vnum = -1;

	count = 0;
	for (auto it = ch->carrying.rbegin(); it != ch->carrying.rend(); ++it)
	{
		obj = *it;
		if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && (nifty_is_name(arg, obj->name) || obj->pIndexData->vnum == vnum))
			if ((count += obj->count) >= number)
				return obj;
	}

	if (vnum != -1)
		return NULL;

	/* If we didn't find an exact match, run through the list of objects
	 again looking for prefix matching, ie swo == sword.
	 Added by Narn, Sept/96
	 */
	count = 0;
	for (auto it = ch->carrying.rbegin(); it != ch->carrying.rend(); ++it)
	{
		obj = *it;
		if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && nifty_is_name_prefix(arg, obj->name))
			if ((count += obj->count) >= number)
				return obj;
	}

	return NULL;
}

/*
 * Find an obj in player's equipment.
 */
OBJ_DATA* get_obj_wear(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int number, count, vnum;

	IF_BUG(ch == NULL, "")
		return NULL;

	number = number_argument(argument, arg);

	if (get_trust(ch) >= LEVEL_SAVIOR && is_number(arg))
		vnum = atoi(arg);
	else
		vnum = -1;

	count = 0;
	for (auto it = ch->carrying.rbegin(); it != ch->carrying.rend(); ++it)
	{
		obj = *it;
		if (obj->wear_loc != WEAR_NONE && can_see_obj(ch, obj) && (nifty_is_name(arg, obj->name) || obj->pIndexData->vnum == vnum))
			if (++count == number)
				return obj;
	}

	if (vnum != -1)
		return NULL;

	/* If we didn't find an exact match, run through the list of objects
	 again looking for prefix matching, ie swo == sword.
	 Added by Narn, Sept/96
	 */
	count = 0;
	for (auto it = ch->carrying.rbegin(); it != ch->carrying.rend(); ++it)
	{
		obj = *it;
		if (obj->wear_loc != WEAR_NONE && can_see_obj(ch, obj) && nifty_is_name_prefix(arg, obj->name))
			if (++count == number)
				return obj;
	}

	return NULL;
}

/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA* get_obj_here(CHAR_DATA *ch, char *argument)
{
	OBJ_DATA *obj;

	if (!ch || !ch->in_room)
		return NULL;

	obj = get_obj_list_rev(ch, argument, ch->in_room->contents);
	if (obj)
		return obj;

	if ((obj = get_obj_carry(ch, argument)) != NULL)
		return obj;

	if ((obj = get_obj_wear(ch, argument)) != NULL)
		return obj;

	return NULL;
}

/*
 * Find an obj in the world.
 */
OBJ_DATA* get_obj_world(CHAR_DATA *ch, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int number, count, vnum;

	if (!ch)
		return NULL;

	if ((obj = get_obj_here(ch, argument)) != NULL)
		return obj;

	number = number_argument(argument, arg);

	/*
	 * Allow reference by vnum for saints+			-Thoric
	 */
	if (get_trust(ch) >= LEVEL_SAVIOR && is_number(arg))
		vnum = atoi(arg);
	else
		vnum = -1;

	count = 0;
	for (auto* obj : object_list)
		if (can_see_obj(ch, obj) && (nifty_is_name(arg, obj->name) || vnum == obj->pIndexData->vnum))
			if ((count += obj->count) >= number)
				return obj;

	/* bail out if looking for a vnum */
	if (vnum != -1)
		return NULL;

	/* If we didn't find an exact match, run through the list of objects
	 again looking for prefix matching, ie swo == sword.
	 Added by Narn, Sept/96
	 */
	count = 0;
	for (auto* obj : object_list)
		if (can_see_obj(ch, obj) && nifty_is_name_prefix(arg, obj->name))
			if ((count += obj->count) >= number)
				return obj;

	return NULL;
}

/*
 * How mental state could affect finding an object		-Thoric
 * Used by get/drop/put/quaff/recite/etc
 * Increasingly freaky based on mental state and drunkeness
 */
bool ms_find_obj(CHAR_DATA *ch)
{
	int ms = ch->mental_state;
	int drunk = IS_NPC(ch) ? 0 : ch->pcdata->condition[COND_DRUNK];
	const char *t;

	//added by Thanos BUG!!! supermob cz�sto bywa w takim stanie
	// (jak si� okazuje) a nie powinien
	if (ch == supermob)
		return false;

	/*
	 * we're going to be nice and let nothing weird happen unless
	 * you're a tad messed up
	 */
	drunk = UMAX(1, drunk);
	if (abs(ms) + (drunk / 3) < 30)
		return false;
	if ((number_percent() + (ms < 0 ? 15 : 5)) > abs(ms) / 2 + drunk / 4)
		return false;
	if (ms > 15) /* range 1 to 20 */
		switch (number_range(UMAX(1, (ms / 5 - 15)), (ms + 4) / 5))
		{
		default:
		case 1:
			t = "Zaraz zaraz. Ale co to mia�o by�?" NL;
			break;
		case 2:
			t = "Ju� ju� to masz, ale... co� ci� powstrzyma�o." NL;
			break;
		case 3:
			t = "Ju� si�gasz, ale co to? To si� rusza!" NL;
			break;
		case 4:
			t = "Hmmm... jako� nie mo�esz tego utrzyma� w r�kach.NL ";
			break;
		case 5:
			t = "Ju� to masz, a potem nie masz..." NL;
			break;
		case 6:
			t = "Nie mo�esz jako� sta� prosto by to zrobi�!" NL;
			break;
		case 7:
			t = "�aaa! To jest ca�e we krwi! Pfuj! Bleh!" NL;
			break;
		case 8:
			t = "Wow... Ale masz jazd�!" NL;
			break;
		case 9:
			t = "W miar� jak po to si�gasz, dostrzegasz, �e na twojej d�oni co� wyrasta! Co to?" NL;
			break;
		case 10:
			t = "Obserwujesz jak wszystko doko�a ciebie zamienia si� w olbrzymie stado male�kich robak�w!" NL;
			break;
		case 11:
			t = "A co powiesz temu wielkiemu droidowi stoj�cemu za twoimi plecami?" NL;
			break;
		case 12:
			t = "Bu! Co to by�o ???" NL;
			break;
		case 13:
			t = "Ale przecie� teraz trzymasz w d�oni losy wszech�wiata. Od�� je najpierw gdzie�." NL;
			break;
		case 14:
			t = "Za bardzo si� boisz..." NL;
			break;
		case 15:
			t = "Twoja mama karci ci� m�wi�c 'Nie ma mowy!'" NL;
			break;
		case 16:
			t = "Co? Ale o co chodzi?" NL;
			break;
		case 17:
			t = "Rezygnujesz, gdy tylko widzisz, �e to �yje! I b�aga ci� o lito��!" NL;
			break;
		case 18:
			t = "A co z tym milionem ma�ych robak�w pa�aszuj�cych twoje rami�?!?!" NL;
			break;
		case 19:
			t = "Eee tam, to ju� niewa�ne. Znasz odpowied� na ka�de pytanie!" NL;
			break;
		case 20:
			t = "Ehh. Przecie� w�adca wszech�wiata nie musi zajmowa� si� takimi b�ahostkami." NL;
			break;
		}
	else
	{
		int sub = URANGE(1, abs(ms) / 2 + drunk, 60);
		switch (number_range(1, sub / 10))
		{
		default:
		case 1:
			t = "Tak Tak, ju� momencik..." NL;
			break;
		case 2:
			t = "Jako� nie mo�esz tego znale��, a mo�e mo�esz..." NL;
			break;
		case 3:
			t = "To poza twoimi aspiracjami..." NL;
			break;
		case 4:
			t = "...ale czym jest jaka� tam g�upia rzecz w por�wnaniu..." NL;
			break;
		case 5:
			t = "Ju� to masz... Masz? Nie masz." NL;
			break;
		case 6:
			t = "Kt�re?!? Tu s� dwa... nie, trzy..." NL;
			break;
		}
	}
	send_to_char(t, ch);
	return true;
}

/*
 * Generic get obj function that supports optional containers.	-Thoric
 * currently only used for "eat" and "quaff".
 */
OBJ_DATA* find_obj(CHAR_DATA *ch, char *argument, bool carryonly)
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (!str_cmp(arg2, "from") && argument[0] != '\0')
		argument = one_argument(argument, arg2);

	if (arg2[0] == '\0')
	{
		if (carryonly && (obj = get_obj_carry(ch, arg1)) == NULL)
		{
			send_to_char("Nie masz takiego przedmiotu." NL, ch);
			return NULL;
		}
		else if (!carryonly && (obj = get_obj_here(ch, arg1)) == NULL)
		{
			act( PLAIN, "Nie widz� tu $T.", ch, NULL, arg1, TO_CHAR);
			return NULL;
		}
		return obj;
	}
	else
	{
		OBJ_DATA *container;

		if (carryonly && (container = get_obj_carry(ch, arg2)) == NULL && (container = get_obj_wear(ch, arg2)) == NULL)
		{
			send_to_char("Nie masz nic takiego." NL, ch);
			return NULL;
		}
		if (!carryonly && (container = get_obj_here(ch, arg2)) == NULL)
		{
			act( PLAIN, "Nie widz� tu $T.", ch, NULL, arg2, TO_CHAR);
			return NULL;
		}

		if (!IS_OBJ_STAT(container, ITEM_COVERING) && IS_SET(container->value[1], CONT_CLOSED))
		{
			if (container->gender == GENDER_MALE)
				act( PLAIN, "$d jest zamkni�ty.", ch, NULL, container->name,
				TO_CHAR);
			else if (container->gender == GENDER_FEMALE)
				act( PLAIN, "$d jest zamkni�ta.", ch, NULL, container->name,
				TO_CHAR);
			else if (container->gender == GENDER_NEUTRAL)
				act( PLAIN, "$d jest zamkni�te.", ch, NULL, container->name,
				TO_CHAR);
			else
				act( PLAIN, "$d s� zamkni�te.", ch, NULL, container->name,
				TO_CHAR);
			return NULL;
		}

		obj = get_obj_list(ch, arg1, container->contents);
		if (!obj)
			act( PLAIN,
			IS_OBJ_STAT(container, ITEM_COVERING) ? "Nie widz� nic takiego pod $p$4." : "Nie widz� nic takiego w $p$5.", ch, container,
					NULL, TO_CHAR);
		return obj;
	}
	return NULL;
}

int get_obj_number(OBJ_DATA *obj)
{
	return obj->count;
}

/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight(OBJ_DATA *obj)
{
	int weight;

	weight = obj->count * obj->weight;
	for (auto* child : obj->contents)
		weight += get_obj_weight(child);

	return weight;
}

/*
 * True if room is dark.
 */
bool room_is_dark(ROOM_INDEX_DATA *pRoomIndex)
{
	IF_BUG(pRoomIndex == NULL, "")
		return true;

	if (pRoomIndex->light > 0)
		return false;

	if (IS_SET(pRoomIndex->room_flags, ROOM_DARK))
		return true;

	if (pRoomIndex->sector_type == SECT_INSIDE || pRoomIndex->sector_type == SECT_CITY)
		return false;

	if ((pRoomIndex->area && pRoomIndex->area->planet)
			&& (pRoomIndex->area->planet->sunlight == SUN_SET || pRoomIndex->area->planet->sunlight == SUN_DARK))
		return true;

	return false;
}

/*
 * True if room is private.
 */
bool room_is_private(CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex)
{
	int count;

	IF_BUG(ch == NULL, "")
		return false;

	IF_BUG(pRoomIndex == NULL, "")
		return false;

	if ( IS_SET(pRoomIndex->room_flags, ROOM_PLR_HOME) && ch->plr_home != pRoomIndex)
		return true;

	count = pRoomIndex->people.size();

	if ( IS_SET(pRoomIndex->room_flags, ROOM_PRIVATE) && count >= 2)
		return true;

	return false;
}

/*
 * True if char can see victim.
 */
bool can_see(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (!victim)
		return false;

	if (ch == victim)
		return true;

	if (ch == supermob) /* Ostro�nie z tym --Thanos */
		return true;

	/* Trog, prototypow gracze nie widza. */
	if (!IS_NPC(ch) && IS_NPC(victim) && IS_SET(victim->act, ACT_PROTOTYPE) && !IS_OLCMAN(ch) && get_trust(ch) < 103)
		return false;

	/* All mobiles cannot see wizinvised immorts */
	if (IS_NPC( ch ) && !IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS))
		return false;

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS) && get_trust(ch) < get_trust(victim)
			&& get_trust(ch) < victim->pcdata->wizinvis)
	{
		if (!is_name(ch->name, victim->pcdata->invis_except)) /*Thanos*/
			return false;
	}

	/* Thanos */
	/* mob questowy widzi tylko gracza bior�cego udzia� w jego que�cie */
	if (IS_NPC( ch ) && ch->inquest)
	{
		if (IS_NPC( victim ) || ch->inquest != victim->inquest)
			return false;
	}

	/* gracze spoza questu nie widz� questowych mob�w */
	if (!IS_NPC(ch))
	{
		if (IS_NPC(
				victim) && victim->inquest && victim->inquest != ch->inquest && get_trust(ch) < LEVEL_QUESTSEE) /* no tam jeden wyj�tek ;) */
			return false;
	}

	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;

	if (victim->position == POS_FIGHTING || victim->position < POS_SLEEPING)
		return true;

	if (!ch)
	{
		if (IS_AFFECTED(victim,
				AFF_INVISIBLE) || IS_AFFECTED(victim, AFF_HIDE) || IS_AFFECTED(victim, AFF_FORCEINVIS) || IS_SET(victim->act, PLR_WIZINVIS))
			return false;
		else
			return true;
	}

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS) && get_trust(ch) < victim->pcdata->wizinvis)
	{
		if (!is_name(ch->name, victim->pcdata->invis_except)) /*Thanos*/
			return false;
	}

	if (victim->position == POS_FIGHTING || victim->position < POS_SLEEPING)
		return true;

	if (victim->position == POS_FIGHTING || victim->position < POS_SLEEPING)
		return true;

	/* SB */
	if (IS_NPC(victim) && IS_SET(victim->act, ACT_MOBINVIS) && get_trust(ch) < victim->mobinvis)
		return false;

	if (!IS_IMMORTAL(ch) && !IS_NPC(victim) && !victim->desc && get_timer(victim, TIMER_RECENTFIGHT) > 0
			&& (!victim->switched || !IS_AFFECTED(victim->switched, AFF_POSSESS)))
		return false;

	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;

	/* The miracle cure for blindness? -- Altrag */
	if (!IS_AFFECTED(ch, AFF_TRUESIGHT))
	{
		if (IS_AFFECTED(ch, AFF_BLIND))
			return false;

		if (room_is_dark(ch->in_room) && !IS_AFFECTED(ch, AFF_INFRARED))
			return false;

		if (IS_AFFECTED(victim, AFF_HIDE) && !IS_AFFECTED(ch, AFF_DETECT_HIDDEN) && !victim->fighting)
			return false;

		if (IS_RACE( victim, "Defel" ) && IS_RACE(ch, "Defel"))
			return true;

		if (!IS_AFFECTED(ch, AFF_DETECT_INVIS))
		{
			if (IS_AFFECTED(victim, AFF_INVISIBLE))
				return false;

			if (isDefelInvisible(victim))
				return false;
		}

		if (IS_AFFECTED(victim, AFF_FORCEINVIS)) //byTrog
			return false;

	}

	return true;
}

/*
 * True if char can see obj.
 */
bool can_see_obj(CHAR_DATA *ch, OBJ_DATA *obj)
{

	if (!ch || !obj)
		return false;

	/* Trog, prototypow gracze nie widza. */
	if (!IS_NPC(ch) && IS_OBJ_STAT(obj, ITEM_PROTOTYPE) && !IS_OLCMAN(ch) && get_trust(ch) < 103)
		return false;

	/* Thanos: gracze i moby spoza questu nie widz� questowych obj */
	if (IS_NPC( ch ) && obj->inquest && belongs_to_quest(ch, obj->inquest))
		; // tutaj jest specjalnie pusto!!!
	// ponizszy warunek ma omijac questora i inne zaproszone
	// mobki
	else if (obj->inquest != NULL && obj->inquest != ch->inquest && get_trust(ch) < LEVEL_QUESTSEE) /* no tam jeden wyj�tek ;) */
		return false;

	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;

	if (IS_OBJ_STAT(obj, ITEM_BURRIED))
		return false;

	if (IS_AFFECTED(ch, AFF_TRUESIGHT))
		return true;

	if (IS_AFFECTED(ch, AFF_BLIND))
		return false;

	if (IS_OBJ_STAT(obj, ITEM_HIDDEN) && !IS_AFFECTED(ch, AFF_DETECT_HIDDEN))
		return false;

	if (obj->item_type == ITEM_LIGHT && obj->value[2] != 0)
		return true;

	if (room_is_dark(ch->in_room) && !IS_AFFECTED(ch, AFF_INFRARED))
		return false;

	if (IS_OBJ_STAT(obj, ITEM_INVIS) && !IS_AFFECTED(ch, AFF_DETECT_INVIS))
		return false;

	return true;
}

/*
 * True if char can drop obj.
 */
bool can_drop_obj(CHAR_DATA *ch, OBJ_DATA *obj)
{
	/* Thanos   -- nie pozbywamy si� przedmiot�w questowych. Trog - BZDURA!!!, wycialem
	 if( obj->inquest )
	 {
	 send_to_char(
	 "Ten przedmiot jest wa�nym elementem twojego zadania." NL
	 "Nie chcesz chyba straci� go z oczu?" NL, ch );
	 return false;
	 }
	 */
	/* Thanos -- personale */
	/*    if( IS_OBJ_STAT( obj, ITEM_PERSONAL )
	 && *obj->owner_name && !str_cmp( obj->owner_name, ch->name )
	 && !IS_NPC( ch ) )
	 {
	 ch_printf( ch,
	 "Przecie� ty i %s" PLAIN " to tak zgrany duet. Nie rozstaniecie si� ani na chwil�!" NL,
	 obj->przypadki[0] );
	 return false;
	 }
	 */
	if (!IS_OBJ_STAT(obj, ITEM_NODROP))
		return true;

	if (!IS_NPC(ch) && get_trust(ch) >= LEVEL_IMMORTAL)
		return true;

	if ( IS_NPC(ch) && ch->pIndexData->vnum == 3)
		return true;

	return false;
}

//added by Thanos (potrzebne np. do identify)
//Nie chc� tutaj celowo korzysta� z tablicy o_types
// bo a. nie mam ochoty robi� kolejnej tak wielkiej tablicy
//    b. interesuj� nas tylko niekt�re jej elementy
char* item_type_name(OBJ_DATA *obj)
{
	static char buf[MAX_STRING_LENGTH];

	switch (obj->item_type)
	{
	default:
		sprintf(buf, "nieznany");
		break;
	case ITEM_GRENADE:
		sprintf(buf, "granat");
		break;
	case ITEM_LIGHT:
		sprintf(buf, "�wiat�o");
		break;
	case ITEM_WEAPON:
		sprintf(buf, "bro�");
		break;
	case ITEM_MISSILE:
		sprintf(buf, "pocisk");
		break;
	case ITEM_TREASURE:
		sprintf(buf, "skarb");
		break;
	case ITEM_ARMOR:
		sprintf(buf, "zbroja");
		break;
	case ITEM_POTION:
		sprintf(buf, "mikstura");
		break;
	case ITEM_FURNITURE:
		sprintf(buf, "mebel");
		break;
	case ITEM_TRASH:
		sprintf(buf, "�mie�");
		break;
	case ITEM_CONTAINER:
		sprintf(buf, "pojemnik");
		break;
	case ITEM_DRINK_CON:
		sprintf(buf, "pojemnik na nap�j");
		break;
	case ITEM_KEY:
		sprintf(buf, "klucz");
		break;
	case ITEM_FOOD:
		sprintf(buf, "jedzenie");
		break;
	case ITEM_MONEY:
		sprintf(buf, "pieni�dze");
		break;
	case ITEM_PEN:
		sprintf(buf, "pisak");
		break;
	case ITEM_CORPSE_NPC:
		sprintf(buf, "cia�o");
		break;
	case ITEM_CORPSE_PC:
		sprintf(buf, "cia�o");
		break;
	case ITEM_FOUNTAIN:
		sprintf(buf, "fontanna");
		break;
	case ITEM_PILL:
		sprintf(buf, "pigu�ka");
		break;
	case ITEM_SCRAPS:
		sprintf(buf, "resztki");
		break;
	case ITEM_SWITCH:
		sprintf(buf, "prze��cznik");
		break;
	case ITEM_LEVER:
		sprintf(buf, "d�wignia");
		break;
	case ITEM_BUTTON:
		sprintf(buf, "przycisk");
		break;
	case ITEM_DIAL:
		sprintf(buf, "komunikator");
		break;	//?????
	case ITEM_TRAP:
		sprintf(buf, "pu�apka");
		break;
	case ITEM_MAP:
		sprintf(buf, "mapa");
		break;
	case ITEM_PAPER:
		sprintf(buf, "papier");
		break;
	case ITEM_LOCKPICK:
		sprintf(buf, "wytrych");
		break;
	case ITEM_FUEL:
		sprintf(buf, "paliwo");
		break;
	case ITEM_AMMO:
		sprintf(buf, "amunicja");
		break;
	case ITEM_SHOVEL:
		sprintf(buf, "�opata");
		break;
	case ITEM_SALVE:
		sprintf(buf, "salve");
		break;	//?????
	case ITEM_RAWSPICE:
		sprintf(buf, "ostrze");
		break;
	case ITEM_LENS:
		sprintf(buf, "soczewka");
		break;
	case ITEM_CRYSTAL:
		sprintf(buf, "kryszta�");
		break;
	case ITEM_DURAPLAST:
		sprintf(buf, "duraplast");
		break;
	case ITEM_BATTERY:
		sprintf(buf, "bateria");
		break;
	case ITEM_TOOLKIT:
		sprintf(buf, "niezb�dnik");
		break;	// ;)
	case ITEM_DURASTEEL:
		sprintf(buf, "durastal");
		break;
	case ITEM_OVEN:
		sprintf(buf, "oven");
		break;	//?????
	case ITEM_MIRROR:
		sprintf(buf, "lustro");
		break;
	case ITEM_CIRCUIT:
		sprintf(buf, "obw�d");
		break;
	case ITEM_SUPERCONDUCTOR:
		sprintf(buf, "nadprzewodnik");
		break;
	case ITEM_COMLINK:
		sprintf(buf, "urz�dzenie komunikacyjne");
		break;
	case ITEM_MEDPAC:
		sprintf(buf, "medpak");
		break;
	case ITEM_FABRIC:
		sprintf(buf, "fabrykat");
		break;
	case ITEM_RARE_METAL:
		sprintf(buf, "czysty metal");
		break;
	case ITEM_MAGNET:
		sprintf(buf, "magnet");
		break;	//?????
	case ITEM_THREAD:
		sprintf(buf, "ig�a z nitk�");
		break;
	case ITEM_PIECE:
		sprintf(buf, "czesc");
		break;
	}
	return (buf);
}

/*
 * Return ascii name of an affect location.
 */
const char* affect_loc_name(int location)
{
	switch (location)
	{
	case APPLY_NONE:
		return "none";
	case APPLY_STR:
		return "strength";
	case APPLY_DEX:
		return "dexterity";
	case APPLY_INT:
		return "intelligence";
	case APPLY_WIS:
		return "wisdom";
	case APPLY_CON:
		return "constitution";
	case APPLY_CHA:
		return "charisma";
	case APPLY_LCK:
		return "luck";
	case APPLY_SEX:
		return "sex";
	case APPLY_MANA:
		return "mana";
	case APPLY_HIT:
		return "hp";
	case APPLY_MOVE:
		return "moves";
	case APPLY_AC:
		return "armor class";
	case APPLY_HITROLL:
		return "hit roll";
	case APPLY_DAMROLL:
		return "damage roll";
	case APPLY_SAVING_POISON:
		return "save vs poison";
	case APPLY_SAVING_ROD:
		return "save vs rod";
	case APPLY_SAVING_PARA:
		return "save vs paralysis";
	case APPLY_SAVING_BREATH:
		return "save vs breath";
	case APPLY_SAVING_SPELL:
		return "save vs spell";
	case APPLY_HEIGHT:
		return "height";
	case APPLY_WEIGHT:
		return "weight";
	case APPLY_AFFECT:
		return "affected_by";
	case APPLY_RESISTANT:
		return "resistant";
	case APPLY_IMMUNE:
		return "immune";
	case APPLY_SUSCEPTIBLE:
		return "susceptible";
	case APPLY_BACKSTAB:
		return "backstab";
	case APPLY_PICK:
		return "pick";
	case APPLY_TRACK:
		return "track";
	case APPLY_STEAL:
		return "steal";
	case APPLY_SNEAK:
		return "sneak";
	case APPLY_SNIPE:
		return "snipe";
	case APPLY_HIDE:
		return "hide";
	case APPLY_DETRAP:
		return "detrap";
	case APPLY_DODGE:
		return "dodge";
	case APPLY_PEEK:
		return "peek";
	case APPLY_SCAN:
		return "scan";
	case APPLY_GOUGE:
		return "gouge";
	case APPLY_SEARCH:
		return "search";
	case APPLY_MOUNT:
		return "mount";
	case APPLY_DISARM:
		return "disarm";
	case APPLY_KICK:
		return "kick";
	case APPLY_PARRY:
		return "parry";
	case APPLY_BASH:
		return "bash";
	case APPLY_STUN:
		return "stun";
	case APPLY_PUNCH:
		return "punch";
	case APPLY_CLIMB:
		return "climb";
	case APPLY_GRIP:
		return "grip";
	case APPLY_SCRIBE:
		return "scribe";
	case APPLY_BREW:
		return "brew";
	case APPLY_WEAPONSPELL:
		return "weapon spell";
	case APPLY_WEARSPELL:
		return "wear spell";
	case APPLY_REMOVESPELL:
		return "remove spell";
	case APPLY_MENTALSTATE:
		return "mental state";
	case APPLY_EMOTION:
		return "emotional state";
	case APPLY_STRIPSN:
		return "dispel";
	case APPLY_REMOVE:
		return "remove";
	case APPLY_DIG:
		return "dig";
	case APPLY_FULL:
		return "hunger";
	case APPLY_THIRST:
		return "thirst";
	case APPLY_DRUNK:
		return "drunk";
	case APPLY_BLOOD:
		return "blood";
	}

	bug("unknown location %d.", location);
	return "(unknown)";
}

//added by Thanos
const char* affect_loc_name_pl(int location)
{
	switch (location)
	{
	case APPLY_NONE:
		return "nic";
	case APPLY_STR:
		return "si��";
	case APPLY_DEX:
		return "zr�czno��";
	case APPLY_INT:
		return "inteligencj�";
	case APPLY_WIS:
		return "m�dro��";
	case APPLY_CON:
		return "kondycj�";
	case APPLY_CHA:
		return "charyzm�";
	case APPLY_LCK:
		return "szcz�cie";
	case APPLY_SEX:
		return "p�e�";
	case APPLY_LEVEL:
		return "poziom";
	case APPLY_AGE:
		return "wiek";
	case APPLY_MANA:
		return "frc";
	case APPLY_HIT:
		return "hp";
	case APPLY_MOVE:
		return "mv";
	case APPLY_GOLD:
		return "ilo�� kredytek";
	case APPLY_EXP:
		return "do�wiadczenie";
	case APPLY_AC:
		return "klas� zbroi";
	case APPLY_HITROLL:
		return "hitroll";
	case APPLY_DAMROLL:
		return "damroll";
	case APPLY_SAVING_POISON:
		return "odporno�� na trucizny";
	case APPLY_SAVING_ROD:
		return "odporno�� na rod";
	case APPLY_SAVING_PARA:
		return "odporno�� na parali�";
	case APPLY_SAVING_BREATH:
		return "odporno�� na gaz";
	case APPLY_SAVING_SPELL:
		return "ochron� przed moc�";
	case APPLY_HEIGHT:
		return "wysoko��";
	case APPLY_WEIGHT:
		return "wag�";
	case APPLY_AFFECT:
		return "wp�ywy";
	case APPLY_RESISTANT:
		return "odporno�� na moc";
	case APPLY_IMMUNE:
		return "nietykalno��";
	case APPLY_SUSCEPTIBLE:
		return "podatno��";
	case APPLY_WEAPONSPELL:
		return "pos�ugiwanie si� broni�";
	case APPLY_BACKSTAB:
		return "backstab";
	case APPLY_PICK:
		return "pick";
	case APPLY_TRACK:
		return "track";
	case APPLY_STEAL:
		return "steal";
	case APPLY_SNEAK:
		return "sneak";
	case APPLY_SNIPE:
		return "snipe";
	case APPLY_HIDE:
		return "hide";
	case APPLY_DETRAP:
		return "detrap";
	case APPLY_DODGE:
		return "dodge";
	case APPLY_PEEK:
		return "peek";
	case APPLY_SCAN:
		return "scan";
	case APPLY_GOUGE:
		return "gouge";
	case APPLY_SEARCH:
		return "search";
	case APPLY_MOUNT:
		return "mount";
	case APPLY_DISARM:
		return "disarm";
	case APPLY_KICK:
		return "kick";
	case APPLY_PARRY:
		return "parry";
	case APPLY_BASH:
		return "bash";
	case APPLY_STUN:
		return "stun";
	case APPLY_PUNCH:
		return "punch";
	case APPLY_CLIMB:
		return "climb";
	case APPLY_GRIP:
		return "grip";
	case APPLY_SCRIBE:
		return "scribe";
	case APPLY_BREW:
		return "brew";
	case APPLY_WEARSPELL:
		return "moc ubrania";
	case APPLY_REMOVESPELL:
		return "usuni�cie mocy";
	case APPLY_EMOTION:
		return "stan emocjonalny";
	case APPLY_MENTALSTATE:
		return "stan psychiczny";
	case APPLY_REMOVE:
		return "remove";
	case APPLY_DIG:
		return "dig";
	case APPLY_FULL:
		return "g��d";
	case APPLY_THIRST:
		return "pragnienie";
	case APPLY_DRUNK:
		return "upicie";
	case APPLY_BLOOD:
		return "krew";
	}

	bug("unknown location %d.", location);
	return "(nie wiadomo co)";
}

char* weapon_type_pl(int value)
{
	static char buf[MSL];

	switch (value)
	{
	default:
		sprintf(buf, "nieznany typ");
		break;
	case WEAPON_NONE:
		sprintf(buf, "zwyk�a bro�");
		break;
	case WEAPON_VIBRO_AXE:
		sprintf(buf, "wibrotop�r");
		break;
	case WEAPON_VIBRO_BLADE:
		sprintf(buf, "wibroostrze");
		break;
	case WEAPON_LIGHTSABER:
		sprintf(buf, "miecz �wietlny");
		break;
	case WEAPON_WHIP:
		sprintf(buf, "bicz");
		break;
	case WEAPON_CLAW:
		sprintf(buf, "pazur");
		break;
	case WEAPON_BLASTER:
		sprintf(buf, "blaster");
		break;
	case WEAPON_BLUDGEON:
		sprintf(buf, "bludgeon");
		break;
	case WEAPON_BOWCASTER:
		sprintf(buf, "kusza energetyczna");
		break;
	case WEAPON_FORCE_PIKE:
		sprintf(buf, "pika �wietlna");
		break;
	}

	return buf;
}

/*
 * Return ascii name of force flags vector. - Scryn
 */
char* force_bit_name(int force_flags)
{
	static char buf[512];

	buf[0] = '\0';
	if (force_flags & ITEM_RETURNING)
		strcat(buf, " returning");
	return (buf[0] != '\0') ? buf + 1 : (char*) "none";
}

/*
 * Set off a trap (obj) upon character (ch)			-Thoric
 */
ch_ret spring_trap(CHAR_DATA *ch, OBJ_DATA *obj)
{
	int dam;
	int typ;
	int lev;
	const char *txt;
	char buf[MAX_STRING_LENGTH];
	ch_ret retcode;

	typ = obj->value[1];
	lev = obj->value[2];

	retcode = rNONE;

	switch (typ)
	{
	default:
		txt = "zranion$y w py�apce";
		break;
	case TRAP_TYPE_POISON_GAS:
		txt = "otoczon$y przez zielon� chmur� gazu";
		break;
	case TRAP_TYPE_POISON_DART:
		txt = "trafion$y truj�c� strza�k�";
		break;
	case TRAP_TYPE_POISON_NEEDLE:
		txt = "uk�ut$y truj�c� ig��";
		break;
	case TRAP_TYPE_POISON_DAGGER:
		txt = "zaatakowan$y sztyletem";
		break;
	case TRAP_TYPE_POISON_ARROW:
		txt = "trafion$y strza��";
		break;
	case TRAP_TYPE_BLINDNESS_GAS:
		txt = "otoczon$y przez czerwon� chmur� gazu";
		break;
	case TRAP_TYPE_SLEEPING_GAS:
		txt = "otoczon$y przez z�t� chmur� gazu";
		break;
	case TRAP_TYPE_FLAME:
		txt = "oparzon$y strumieniem p�omieni";
		break;
	case TRAP_TYPE_EXPLOSION:
		txt = "uderzon$y eksplozj�";
		break;
	case TRAP_TYPE_ACID_SPRAY:
		txt = "ochlapan$y strumieniem kwasu";
		break;
	case TRAP_TYPE_ELECTRIC_SHOCK:
		txt = "pora�on$y pr�dem";
		break;
	case TRAP_TYPE_BLADE:
		txt = "okaleczon$y ostrzem";
		break;
	case TRAP_TYPE_SEX_CHANGE:
		txt = "otoczon$y dziwn� mgie�k�";
		break;
	}

	dam = number_range(obj->value[2], obj->value[2] * 2);
	sprintf(buf, "Zosta�$o� %s!", txt);
	act( COL_HITME, buf, ch, NULL, NULL, TO_CHAR);
	sprintf(buf, "$n zosta�$o %s.", txt);
	act( COL_ACTION, buf, ch, NULL, NULL, TO_ROOM);
	--obj->value[0];
	if (obj->value[0] <= 0)
		extract_obj(obj);
	switch (typ)
	{
	default:
	case TRAP_TYPE_POISON_DART:
	case TRAP_TYPE_POISON_NEEDLE:
	case TRAP_TYPE_POISON_DAGGER:
	case TRAP_TYPE_POISON_ARROW:
		/* hmm... why not use spell_poison() here? */
		retcode = obj_cast_spell(gsn_poison, lev, ch, ch, NULL);
		if (retcode == rNONE)
			retcode = damage(ch, ch, dam, TYPE_UNDEFINED);
		break;
	case TRAP_TYPE_POISON_GAS:
		retcode = obj_cast_spell(gsn_poison, lev, ch, ch, NULL);
		break;
	case TRAP_TYPE_BLINDNESS_GAS:
		retcode = obj_cast_spell(gsn_blindness, lev, ch, ch, NULL);
		break;
	case TRAP_TYPE_SLEEPING_GAS:
		retcode = obj_cast_spell(skill_lookup("sleep"), lev, ch, ch, NULL);
		break;
	case TRAP_TYPE_ACID_SPRAY:
		retcode = obj_cast_spell(skill_lookup("acid blast"), lev, ch, ch, NULL);
		break;
		/*
		 case TRAP_TYPE_SEX_CHANGE:
		 retcode = obj_cast_spell( skill_lookup("change sex"), lev, ch, ch, NULL );
		 break;
		 case TRAP_TYPE_FLAME:
		 case TRAP_TYPE_EXPLOSION:
		 retcode = obj_cast_spell( gsn_fireball, lev, ch, ch, NULL );
		 break;
		 Trog: przy wicieciu fireballa wywalilem to, jak cos to sie doda... */
	case TRAP_TYPE_ELECTRIC_SHOCK:
	case TRAP_TYPE_BLADE:
		retcode = damage(ch, ch, dam, TYPE_UNDEFINED);
	}
	return retcode;
}

/*
 * Check an object for a trap					-Thoric
 */
ch_ret check_for_trap(CHAR_DATA *ch, OBJ_DATA *obj, int flag)
{
	ch_ret retcode;

	if (obj->contents.empty())
		return rNONE;

	retcode = rNONE;

	for (auto* check : obj->contents)
		if (check->item_type == ITEM_TRAP && IS_SET(check->value[3], flag))
		{
			retcode = spring_trap(ch, check);
			if (retcode != rNONE)
				return retcode;
		}
	return retcode;
}

/*
 * Check the room for a trap					-Thoric
 */
ch_ret check_room_for_traps(CHAR_DATA *ch, int flag)
{
	ch_ret retcode;

	retcode = rNONE;

	if (!ch)
		return rERROR;
	if (!ch->in_room || ch->in_room->contents.empty())
		return rNONE;

	for (auto* check : ch->in_room->contents)
	{
		if (check->item_type == ITEM_LANDMINE && flag == TRAP_ENTER_ROOM)
		{
			explode(check);
			return rNONE;
		}
		else if (check->item_type == ITEM_TRAP && IS_SET(check->value[3], flag))
		{
			retcode = spring_trap(ch, check);
			if (retcode != rNONE)
				return retcode;
		}
	}
	return retcode;
}

/*
 * return true if an object contains a trap			-Thoric
 */
bool is_trapped(OBJ_DATA *obj)
{
	if (obj->contents.empty())
		return false;

	for (auto* check : obj->contents)
		if (check->item_type == ITEM_TRAP)
			return true;

	return false;
}

/*
 * If an object contains a trap, return the pointer to the trap	-Thoric
 */
OBJ_DATA* get_trap(OBJ_DATA *obj)
{
	if (obj->contents.empty())
		return NULL;

	for (auto* check : obj->contents)
		if (check->item_type == ITEM_TRAP)
			return check;

	return NULL;
}

/*
 * Remove an exit from a room					-Thoric
 */
void extract_exit(ROOM_INDEX_DATA *room, EXIT_DATA *pexit)
{
	room->exits.remove(pexit);
	if (pexit->rexit)
		pexit->rexit->rexit = NULL;
	free_exit(pexit);
}

/*
 * Remove a room
 */
void extract_room(ROOM_INDEX_DATA *room)
{
	bug("not implemented");
	/*  (remove room from hash table)
	 clean_room( room )
	 DISPOSE( room );*/
	return;
}

/*
 * clean out a room (leave list pointers intact )		-Thoric
 */
void clean_room(ROOM_INDEX_DATA *room)
{
	STRDUP(room->name, "");
	STRDUP(room->description, "");
	STRDUP(room->nightdesc, "");

	for (auto* ed : room->extradesc)
		free_ed(ed);
	room->extradesc.clear();

	for (auto* pexit : room->exits)
		free_exit(pexit);
	room->exits.clear();

	for (auto* rprg : room->mudprogs)
		free_mprog(rprg);
	room->mudprogs.clear();

	room->room_flags = 0;
	room->sector_type = 0;
	room->light = 0;
}

/*
 * clean out an object (index) (leave list pointers intact )	-Thoric
 */
void clean_obj(OBJ_INDEX_DATA *obj)
{
	int i;
	STRDUP(obj->name, "");
	for (i = 0; i < 6; i++)
		STRDUP(obj->przypadki[i], "");

	STRDUP(obj->description, "");
	STRDUP(obj->action_desc, "");
	obj->item_type = 0;
	obj->extra_flags = 0;
	obj->wear_flags = 0;
	obj->count = 0;
	obj->weight = 0;
	obj->cost = 0;
	obj->gender = GENDER_MALE;
	obj->value[0] = 0;
	obj->value[1] = 0;
	obj->value[2] = 0;
	obj->value[3] = 0;
	obj->value[4] = 0;
	obj->value[5] = 0;

	for (auto* paf : obj->affects)
	{
		DISPOSE(paf);
		top_affect--;
	}
	obj->affects.clear();

	for (auto* ed : obj->extradesc)
		free_ed(ed);
	obj->extradesc.clear();
}

/*
 * clean out a mobile (index) (leave list pointers intact )	-Thoric
 */
void clean_mob(MOB_INDEX_DATA *mob)
{
	int i;

	STRDUP(mob->player_name, "");
//added by Thanos
	for (i = 0; i < 6; i++)
		STRDUP(mob->przypadki[i], "");

	STRDUP(mob->s_vip_flags, "");

	STRDUP(mob->long_descr, "");
	STRDUP(mob->description, "");
	STRDUP(mob->dialog_name, "");
	mob->spec_fun = NULL;
	mob->spec_2 = NULL;
	mob->pShop = NULL;
	mob->rShop = NULL;
	mob->progtypes = 0;

	for (auto* mp : mob->mudprogs)
		free_mprog(mp);
	mob->mudprogs.clear();
	mob->count = 0;
	mob->killed = 0;
	mob->sex = 0;
	mob->level = 0;
	mob->act = 0;
	mob->affected_by = 0;
	mob->alignment = 0;
	mob->mobthac0 = 0;
	mob->ac = 0;
	mob->hitnodice = 0;
	mob->hitsizedice = 0;
	mob->hitplus = 0;
	mob->damnodice = 0;
	mob->damsizedice = 0;
	mob->damplus = 0;
	mob->gold = 0;
	mob->position = 0;
	mob->defposition = 0;
	mob->height = 0;
	mob->weight = 0;
}

extern int top_reset;

/*
 * "Fix" a character's stats					-Thoric
 */
void fix_char(CHAR_DATA *ch)
{
	OBJ_DATA *carry[MAX_LEVEL * 200];
	OBJ_DATA *obj;
	int x, ncarry;

	de_equip_char(ch);

	ncarry = 0;
	while (!ch->carrying.empty())
	{
		obj = ch->carrying.front();
		carry[ncarry++] = obj;
		obj_from_char(obj);
	}

	for (auto* aff : ch->affects)
		affect_modify(ch, aff, false);

	ch->affected_by = ch->race->affected;
	ch->mental_state = 0;
	ch->hit = UMAX(1, ch->hit);
	ch->mana = UMAX(1, ch->mana);
	ch->move = UMAX(1, ch->move);
	ch->armor = 100;
	ch->mod_str = 0;
	ch->mod_dex = 0;
	ch->mod_wis = 0;
	ch->mod_int = 0;
	ch->mod_con = 0;
	ch->mod_cha = 0;
	ch->mod_lck = 0;
	ch->damroll = 0;
	ch->hitroll = 0;
	ch->alignment = URANGE(-1000, ch->alignment, 1000);
	ch->saving_breath = 0;
	ch->saving_wand = 0;
	ch->saving_para_petri = 0;
	ch->saving_spell_staff = 0;
	ch->saving_poison_death = 0;

	ch->carry_weight = 0;
	ch->carry_number = 0;

	for (auto* aff : ch->affects)
		affect_modify(ch, aff, true);

	for (x = 0; x < ncarry; x++)
		obj_to_char(carry[x], ch);

	re_equip_char(ch);
}

/*
 * Show an affect verbosely to a character			-Thoric
 */
void showaffect(CHAR_DATA *ch, AFFECT_DATA *paf)
{
	char buf[MAX_STRING_LENGTH];
	int x;

	IF_BUG(paf == NULL, "")
		return;

	if (paf->location != APPLY_NONE && paf->modifier != 0)
	{
		switch (paf->location)
		{
		default:
			sprintf(buf,
			FG_CYAN
			"Wp�ywa na " FB_CYAN "%s" FG_CYAN " o " FB_CYAN "%d" FG_CYAN "." EOL, affect_loc_name_pl(paf->location), paf->modifier);
			break;
		case APPLY_AFFECT:
			sprintf(buf, "Wp�ywa na " FB_CYAN "%s" FG_CYAN " o " FB_CYAN, affect_loc_name_pl(paf->location));
			strcat(buf, flag_string(aff_flags_list, paf->modifier));
			strcat(buf, EOL);
			break;
		case APPLY_WEAPONSPELL:
		case APPLY_WEARSPELL:
		case APPLY_REMOVESPELL:
			sprintf(buf, "Potrafi wykona� '" FB_CYAN "%s" FG_CYAN "'" EOL,
			IS_VALID_SN(paf->modifier) ? skill_table[paf->modifier]->name : "nie wiadomo co");
			break;
		case APPLY_RESISTANT:
		case APPLY_IMMUNE:
		case APPLY_SUSCEPTIBLE:
			sprintf(buf, "Wp�ywa na " FB_CYAN "%s" FG_CYAN " o " FB_CYAN, affect_loc_name_pl(paf->location));
			for (x = 0; x < 32; x++)
				if (IS_SET(paf->modifier, 1 << x))
				{
					strcat(buf, " ");
					strcat(buf, ris_flags_list[x].name);
				}
			strcat(buf, EOL);
			break;
		}
		send_to_char(buf, ch);
	}
}

/*
 * Set the current global object to obj				-Thoric
 */
void set_cur_obj(OBJ_DATA *obj)
{
	cur_obj = obj->serial;
	cur_obj_extracted = false;
	global_objcode = rNONE;
}

/*
 * Check the recently extracted object queue for obj		-Thoric
 */
bool obj_extracted(OBJ_DATA *obj)
{
	if (!obj)
		return true;

	if (obj->serial == cur_obj && cur_obj_extracted)
		return true;

	for (auto* cod : extracted_obj_queue)
		if (obj == cod)
			return true;
	return false;
}

/*
 * Stick obj onto extraction queue
 */
void queue_extracted_obj(OBJ_DATA *obj)
{

	++cur_qobjs;
	extracted_obj_queue.push_front(obj);
}

/*
 * Clean out the extracted object queue
 */
void clean_obj_queue()
{
	OBJ_DATA *obj;

	while (!extracted_obj_queue.empty())
	{
		obj = extracted_obj_queue.front();
		extracted_obj_queue.pop_front();
		free_obj(obj);
		--cur_qobjs;
	}
}

/*
 * Set the current global character to ch			-Thoric
 */
void set_cur_char(CHAR_DATA *ch)
{
	cur_char = ch;
	cur_char_died = false;
	cur_room = ch->in_room;
	global_retcode = rNONE;
}

/*
 * Check to see if ch died recently				-Thoric
 */
bool char_died(CHAR_DATA *ch)
{
	if (ch == cur_char && cur_char_died)
		return true;

	for (auto& ccd : extracted_char_queue)
		if (ccd.ch == ch)
			return true;
	return false;
}

/*
 * Add ch to the queue of recently extracted characters		-Thoric
 */
void queue_extracted_char(CHAR_DATA *ch, bool extract)
{
	IF_BUG(ch == NULL, "")
		return;

	EXTRACT_CHAR_DATA ecd;
	ecd.ch = ch;
	ecd.room = ch->in_room;
	ecd.extract = extract;
	if (ch == cur_char)
		ecd.retcode = global_retcode;
	else
		ecd.retcode = rCHAR_DIED;
	extracted_char_queue.push_front(ecd);
	cur_qchars++;

}

/*
 * clean out the extracted character queue
 */
void clean_char_queue()
{
	while (!extracted_char_queue.empty())
	{
		auto& ccd = extracted_char_queue.front();
		if (ccd.extract)
			free_char(ccd.ch);
		extracted_char_queue.pop_front();
		--cur_qchars;
	}
}

/*
 * Add a timer to ch						-Thoric
 * Support for "call back" time delayed commands
 */
void add_timer(CHAR_DATA *ch, int type, int count, DO_FUN *fun, int value)
{
	TIMER *timer;

	for (auto* t : ch->timers)
		if (t->type == type)
		{
			t->count = count;
			t->do_fun = fun;
			t->value = value;
			return;
		}
	CREATE(timer, TIMER, 1);
	timer->count = count;
	timer->type = type;
	timer->do_fun = fun;
	timer->value = value;
	ch->timers.push_back(timer);
}

TIMER* get_timerptr(CHAR_DATA *ch, int type)
{
	for (auto* timer : ch->timers)
		if (timer->type == type)
			return timer;
	return NULL;
}

int get_timer(CHAR_DATA *ch, int type)
{
	TIMER *timer;

	if ((timer = get_timerptr(ch, type)) != NULL)
		return timer->count;
	else
		return 0;
}

void extract_timer(CHAR_DATA *ch, TIMER *timer)
{
	IF_BUG(ch == NULL, "")
		return;

	IF_BUG(timer == NULL, "(ch:%s)", ch->name)
		return;

	ch->timers.remove(timer);
	DISPOSE(timer);
	return;
}

void remove_timer(CHAR_DATA *ch, int type)
{
	for (auto* t : ch->timers)
		if (t->type == type)
		{
			extract_timer(ch, t);
			return;
		}
}

bool in_level_range(CHAR_DATA *ch, AREA_DATA *tarea)
{
	if (IS_IMMORTAL(ch))
		return true;
	else if (IS_NPC(ch))
		return true;
	else if (ch->top_level >= tarea->low_range || ch->top_level <= tarea->high_range)
		return true;
	else
		return false;
}

/*
 * Scryn, standard luck check 2/2/96
 */
bool chance(CHAR_DATA *ch, int percent)
{
	/*  int clan_factor, ms;*/
	int deity_factor, ms;

	IF_BUG(ch == NULL, "")
		return false;

	/* Code for clan stuff put in by Narn, Feb/96.  The idea is to punish clan
	 members who don't keep their alignment in tune with that of their clan by
	 making it harder for them to succeed at pretty much everything.  Clan_factor
	 will vary from 1 to 3, with 1 meaning there is no effect on the player's
	 change of success, and with 3 meaning they have half the chance of doing
	 whatever they're trying to do.

	 Note that since the neutral clannies can only be off by 1000 points, their
	 maximum penalty will only be half that of the other clan types.

	 if ( IS_CLANNED( ch ) )
	 clan_factor = 1 + abs( ch->alignment - ch->pcdata->clan->alignment ) / 1000;
	 else
	 clan_factor = 1;
	 */
	/* Mental state bonus/penalty:  Your mental state is a ranged value with
	 * zero (0) being at a perfect mental state (bonus of 10).
	 * negative values would reflect how sedated one is, and
	 * positive values would reflect how stimulated one is.
	 * In most circumstances you'd do best at a perfectly balanced state.
	 */

	deity_factor = 0;

	ms = 10 - abs(ch->mental_state);

	if ((number_percent() - get_curr_lck(ch) + 13 - ms) + deity_factor <= percent)
		return true;
	else
		return false;
}

bool chance_attrib(CHAR_DATA *ch, int percent, int attrib)
{
	/* Scryn, standard luck check + consideration of 1 attrib 2/2/96*/
	int deity_factor;

	IF_BUG(ch == NULL, "")
		return false;

	deity_factor = 0;

	if (number_percent() - get_curr_lck(ch) + 13 - attrib + 13 + deity_factor <= percent)
		return true;
	else
		return false;

}

/*
 * Make a simple clone of an object (no extras...yet)		-Thoric
 */
OBJ_DATA* clone_object(OBJ_DATA *obj)
{
	OBJ_DATA *clone;
	int i;

	CREATE(clone, OBJ_DATA, 1);
	clone->pIndexData = obj->pIndexData;
	STRDUP(clone->name, obj->name);
	for (i = 0; i < 6; i++)
		STRDUP(clone->przypadki[i], obj->przypadki[i]);
	STRDUP(clone->description, obj->description);
	STRDUP(clone->action_desc, obj->action_desc);
	STRDUP(clone->owner_name, obj->owner_name);
	clone->item_type = obj->item_type;
	clone->extra_flags = obj->extra_flags;
	clone->force_flags = obj->force_flags;
	clone->wear_flags = obj->wear_flags;
	clone->wear_loc = obj->wear_loc;
	clone->weight = obj->weight;
	clone->cost = obj->cost;
	clone->gender = obj->gender;	//added by Thanos
	clone->level = obj->level;
	clone->timer = obj->timer;
	clone->value[0] = obj->value[0];
	clone->value[1] = obj->value[1];
	clone->value[2] = obj->value[2];
	clone->value[3] = obj->value[3];
	clone->value[4] = obj->value[4];
	clone->value[5] = obj->value[5];
	clone->count = 1;
	++obj->pIndexData->count;
	++numobjsloaded;
	++physicalobjects;
	cur_obj_serial = UMAX((cur_obj_serial + 1 ) & (BV30-1), 1);
	clone->serial = clone->pIndexData->serial = cur_obj_serial;
	object_list.push_back(clone);
	return clone;
}

/*
 * If possible group obj2 into obj1				-Thoric
 * This code, along with clone_object, obj->count, and special support
 * for it implemented throughout handler.c and save.c should show improved
 * performance on MUDs with players that hoard tons of potions and scrolls
 * as this will allow them to be grouped together both in memory, and in
 * the player files.
 */
OBJ_DATA* group_object(OBJ_DATA *obj1, OBJ_DATA *obj2)
{
	/* Thanos -- tak bedzie bezpieczniej */
	if (obj1->inquest || obj2->inquest)
		return NULL;

	if (!obj1 || !obj2)
		return NULL;
	if (obj1 == obj2)
		return obj1;

	if (obj1->pIndexData == obj2->pIndexData
	/*
	 &&	!obj1->pIndexData->mudprogs
	 &&  !obj2->pIndexData->mudprogs
	 */
	&& !str_cmp(obj1->name, obj2->name) && !str_cmp(obj1->przypadki[0], obj2->przypadki[0])
			&& !str_cmp(obj1->description, obj2->description) && !str_cmp(obj1->action_desc, obj2->action_desc)
			&& !str_cmp(obj1->owner_name, obj2->owner_name) && obj1->item_type == obj2->item_type && obj1->extra_flags == obj2->extra_flags
			&& obj1->force_flags == obj2->force_flags && obj1->wear_flags == obj2->wear_flags && obj1->wear_loc == obj2->wear_loc
			&& obj1->weight == obj2->weight && obj1->cost == obj2->cost && obj1->gender == obj2->gender /*added by Thanos*/
			&& obj1->level == obj2->level && obj1->timer == obj2->timer && obj1->value[0] == obj2->value[0]
			&& obj1->value[1] == obj2->value[1] && obj1->value[2] == obj2->value[2] && obj1->value[3] == obj2->value[3]
			&& obj1->value[4] == obj2->value[4] && obj1->value[5] == obj2->value[5] && obj1->extradesc.empty() && obj2->extradesc.empty()
			&& obj1->affects.empty() && obj2->affects.empty() && obj1->contents.empty() && obj2->contents.empty())
	{
		obj1->count += obj2->count;
		obj1->pIndexData->count += obj2->count; /* to be decremented in */
		numobjsloaded += obj2->count; /* extract_obj */
		extract_obj(obj2);
		return obj1;
	}
	return obj2;
}

/*
 * Split off a grouped object					-Thoric
 * decreased obj's count to num, and creates a new object containing the rest
 */
void split_obj(OBJ_DATA *obj, int num)
{
	int count;
	OBJ_DATA *rest;

	if (!obj)
		return;

	count = obj->count;

	if (count <= num || num == 0)
		return;

	rest = clone_object(obj);
	--obj->pIndexData->count; /* since clone_object() ups this value */
	--numobjsloaded;
	rest->count = obj->count - num;
	obj->count = num;

	if (obj->carried_by)
	{
		obj->carried_by->carrying.push_back(rest);
		rest->carried_by = obj->carried_by;
		rest->in_room = NULL;
		rest->in_obj = NULL;
	}
	else if (obj->in_room)
	{
		obj->in_room->contents.push_back(rest);
		rest->carried_by = NULL;
		rest->in_room = obj->in_room;
		rest->in_obj = NULL;
	}
	else if (obj->in_obj)
	{
		obj->in_obj->contents.push_back(rest);
		rest->in_obj = obj->in_obj;
		rest->in_room = NULL;
		rest->carried_by = NULL;
	}
}

void separate_obj(OBJ_DATA *obj)
{
	split_obj(obj, 1);
}

/*
 * Empty an obj's contents... optionally into another obj, or a room
 */
bool empty_obj(OBJ_DATA *obj, OBJ_DATA *destobj, ROOM_INDEX_DATA *destroom)
{
	CHAR_DATA *ch = obj->carried_by;
	bool movedsome = false;

	IF_BUG(obj == NULL, "")
		return false;

	if (destobj || (!destroom && !ch && (destobj = obj->in_obj) != NULL))
	{
		auto snapshot = obj->contents;
		for (auto* otmp : snapshot)
		{
			if (destobj->item_type == ITEM_CONTAINER && get_obj_weight(otmp) + get_obj_weight(destobj) > destobj->value[0])
				continue;
			obj_from_obj(otmp);
			obj_to_obj(otmp, destobj);
			movedsome = true;
		}
		return movedsome;
	}
	if (destroom || (!ch && (destroom = obj->in_room) != NULL))
	{
		auto snapshot = obj->contents;
		for (auto* otmp : snapshot)
		{
			if (ch && (otmp->pIndexData->progtypes & DROP_PROG) && otmp->count > 1)
			{
				separate_obj(otmp);
				obj_from_obj(otmp);
			}
			else
				obj_from_obj(otmp);
			otmp = obj_to_room(otmp, destroom);
			if (ch)
			{
				oprog_drop_trigger(ch, otmp); /* mudprogs */
				if (char_died(ch))
					ch = NULL;
			}
			movedsome = true;
		}
		return movedsome;
	}
	if (ch)
	{
		auto snapshot = obj->contents;
		for (auto* otmp : snapshot)
		{
			obj_from_obj(otmp);
			obj_to_char(otmp, ch);
			movedsome = true;
		}
		return movedsome;
	}
	bug("could not determine a destination for vnum %d", obj->pIndexData->vnum);
	return false;
}

/*
 * Improve mental state						-Thoric
 */
void better_mental_state(CHAR_DATA *ch, int mod)
{
	int c = URANGE(0, abs(mod), 20);
	int con = get_curr_con(ch);

	c += number_percent() < con ? 1 : 0;

	if (ch->mental_state < 0)
		ch->mental_state = URANGE(-100, ch->mental_state + c, 0);
	else if (ch->mental_state > 0)
		ch->mental_state = URANGE(0, ch->mental_state - c, 100);
}

/*
 * Deteriorate mental state					-Thoric
 */
void worsen_mental_state(CHAR_DATA *ch, int mod)
{
	int c = URANGE(0, abs(mod), 20);
	int con = get_curr_con(ch);

	/* Thanos */
	if (IS_IMMORTAL(ch))
		return;

	c -= number_percent() < con ? 1 : 0;
	if (c < 1)
		return;

	if (ch->mental_state < 0)
		ch->mental_state = URANGE(-100, ch->mental_state - c, 100);
	else if (ch->mental_state > 0)
		ch->mental_state = URANGE(-100, ch->mental_state + c, 100);
	else
		ch->mental_state -= c;
}

/*
 * Add gold to an area's economy				-Thoric
 */
void boost_economy(AREA_DATA *tarea, int gold)
{
	while (gold >= 1000000000)
	{
		++tarea->high_economy;
		gold -= 1000000000;
	}
	tarea->low_economy += gold;
	while (tarea->low_economy >= 1000000000)
	{
		++tarea->high_economy;
		tarea->low_economy -= 1000000000;
	}
}

/*
 * Take gold from an area's economy				-Thoric
 */
void lower_economy(AREA_DATA *tarea, int gold)
{
	while (gold >= 1000000000)
	{
		tarea->high_economy -= 1;
		gold -= 1000000000;
	}
	tarea->low_economy -= gold;
	while (tarea->low_economy < 0)
	{
		tarea->high_economy -= 1;
		tarea->low_economy += 1000000000;
	}
}

/*
 * Check to see if economy has at least this much gold		   -Thoric
 */
bool economy_has(AREA_DATA *tarea, int gold)
{
	int hasgold = ((tarea->high_economy > 0) ? 1 : 0) * 1000000000 + tarea->low_economy;

	if (hasgold >= gold)
		return true;
	return false;
}

/*
 * Used in db.c when resetting a mob into an area		    -Thoric
 * Makes sure mob doesn't get more than 10% of that area's gold,
 * and reduces area economy by the amount of gold given to the mob
 */
void economize_mobgold(CHAR_DATA *mob)
{
	int gold;
	AREA_DATA *tarea;

	/* make sure it isn't way too much */
	mob->gold = UMIN(mob->gold, mob->top_level * mob->top_level * 400);
	if (!mob->in_room)
		return;
	tarea = mob->in_room->area;

	gold = ((tarea->high_economy > 0) ? 1 : 0) * 1000000000 + tarea->low_economy;
	mob->gold = URANGE(0, mob->gold, gold / 10);
	if (mob->gold)
		lower_economy(tarea, mob->gold);
}

/*
 * Add another notch on that there belt... ;)
 * Keep track of the last so many kills by vnum			-Thoric
 */
void add_kill(CHAR_DATA *ch, CHAR_DATA *mob)
{
	int x;
	int vnum, track;

	if (IS_NPC(ch))
		return;

	if (!IS_NPC(mob))
		return;

	vnum = mob->pIndexData->vnum;
	track = URANGE(2, ((ch->skill_level[COMBAT_ABILITY]+3) * MAX_KILLTRACK)/LEVEL_AVATAR, MAX_KILLTRACK);
	for (x = 0; x < track; x++)
		if (ch->pcdata->killed[x].vnum == vnum)
		{
			if (ch->pcdata->killed[x].count < 50)
				++ch->pcdata->killed[x].count;
			return;
		}
		else if (ch->pcdata->killed[x].vnum == 0)
			break;
	memmove((char*) ch->pcdata->killed + sizeof(KILLED_DATA), ch->pcdata->killed, (track - 1) * sizeof(KILLED_DATA));
	ch->pcdata->killed[0].vnum = vnum;
	ch->pcdata->killed[0].count = 1;
	if (track < MAX_KILLTRACK)
		ch->pcdata->killed[track].vnum = 0;
}

/*
 * Return how many times this player has killed this mob	-Thoric
 * Only keeps track of so many (MAX_KILLTRACK), and keeps track by vnum
 */
int times_killed(CHAR_DATA *ch, CHAR_DATA *mob)
{
	int x;
	int vnum, track;

	if (IS_NPC(ch))
		return 0;

	if (!IS_NPC(mob))
		return 0;

	vnum = mob->pIndexData->vnum;
	track = URANGE(2, ((ch->skill_level[COMBAT_ABILITY]+3) * MAX_KILLTRACK)/LEVEL_AVATAR, MAX_KILLTRACK);
	for (x = 0; x < track; x++)
		if (ch->pcdata->killed[x].vnum == vnum)
			return ch->pcdata->killed[x].count;
		else if (ch->pcdata->killed[x].vnum == 0)
			break;
	return 0;
}

char* get_race(CHAR_DATA *ch)
{
	static char buf[MSL];

	strcpy(buf, CH_RACE(ch, 0));
	return buf;
}

void rename_char(CHAR_DATA *old, CHAR_DATA *tmp)
{
	char buf[MIL];
	char buf2[MIL];
	int i;

	IF_BUG(old == NULL, "")
		return;

	IF_BUG(IS_NPC(old), "")
		return;

	IF_BUG(tmp == NULL, "")
		return;

	sprintf(buf, "Rename: %s -> %s", old->name, capitalize(tmp->przypadki[0]));
	log_string(buf);

	sprintf(buf, "%s%s", DEPOSIT_DIR, capitalize(old->name));
	sprintf(buf2, "%s%s", DEPOSIT_DIR, capitalize(tmp->przypadki[0]));
	rename(buf, buf2);

	sprintf(buf, "%s%s", QDATA_DIR, capitalize(old->name));
	sprintf(buf2, "%s%s", QDATA_DIR, capitalize(tmp->przypadki[0]));
	rename(buf, buf2);

	sprintf(buf, "%s%s", PDATA_DIR, capitalize(old->name));
	sprintf(buf2, "%s%s", PDATA_DIR, capitalize(tmp->przypadki[0]));
	rename(buf, buf2);

	sprintf(buf, "%s%s", PCHAR_DIR, capitalize(old->name));
	sprintf(buf2, "%s%s", PCHAR_DIR, capitalize(tmp->przypadki[0]));
	rename(buf, buf2);

	sprintf(buf, "%s%s", CLONE_DIR, capitalize(old->name));
	sprintf(buf2, "%s%s", CLONE_DIR, capitalize(tmp->przypadki[0]));
	rename(buf, buf2);

	for (auto* inform : inform_list)
	{
		if (!str_cmp(inform->attacker, old->name))
			STRDUP(inform->attacker, capitalize(tmp->przypadki[0]));

		if (!str_cmp(inform->victim, old->name))
			STRDUP(inform->victim, capitalize(tmp->przypadki[0]));
	}
	save_informs();

	for (auto* ship : ship_list)
	{
		bool found = false;

		if (!str_cmp(ship->owner, old->name))
		{
			STRDUP(ship->owner, capitalize(tmp->przypadki[0]));
			found = true;
		}

		if (!str_cmp(ship->pilot, old->name))
		{
			STRDUP(ship->pilot, capitalize(tmp->przypadki[0]));
			found = true;
		}

		if (!str_cmp(ship->copilot, old->name))
		{
			STRDUP(ship->copilot, capitalize(tmp->przypadki[0]));
			found = true;
		}

		if (found)
			save_ship(ship);
	}

	/* Trog: w klanach */
	rename_member(old->name, capitalize(tmp->przypadki[0]));

	for (auto* obj : object_list)
	{
		if (obj->armed_by && !str_cmp(obj->armed_by, old->name))
			STRDUP(obj->armed_by, capitalize(tmp->przypadki[0]));

		/* obj->owner_name na wszelki wypadek, ale bez tego tez dziala */
		if (obj->owner_name && !str_cmp(obj->owner_name, old->name))
			STRDUP(obj->owner_name, capitalize(tmp->przypadki[0]));
	}

	for (i = 0; i < 6; i++)
		STRDUP(old->przypadki[i], tmp->przypadki[i]);
	STRDUP(old->name, tmp->przypadki[0]);

	save_char_obj(old);
}

void destroy_char(char *name)
{
	char buf[MSL];
	char buf2[MSL];

	sprintf(buf, "Deleting char: %s", capitalize(name));
	log_string(buf);

	/* Questy i Depozyt gin� nieodwracalnie niezale�nie od klona */
	sprintf(buf2, "%s%s", DEPOSIT_DIR, capitalize(name));
	unlink(buf2);
	sprintf(buf2, "%s%s", QDATA_DIR, capitalize(name));
	unlink(buf2);

	sprintf(buf, "%s%s", PCHAR_DIR, capitalize(name));
	sprintf(buf2, "%s%s", CLONE_DIR, capitalize(name));

	unlink(buf);
	rename(buf2, buf);

	if (!(fopen(buf, "r")))
	{
		log_string("Death: No clone found. Deleting Char");
		sprintf(buf2, "%s%s", PDATA_DIR, capitalize(name));
		unlink(buf2);
		sprintf(buf2, "%s%s", PHOME_DIR, capitalize(name));
		unlink(buf2);
	}
	else
	{
		log_string("Clone found.");
		strcat(buf2, ": Renaming clone to ");
		strcat(buf2, buf);
		log_string(buf2);
		return;
	}

	/*
	 *	Tutaj umie�ci� f-cje do zwalniania statk�w i klan�w
	 *  Po graczu, kt�rego ju� nie ma.
	 */
	for (auto* ship : ship_list)
		if (!str_cmp(ship->owner, name))
		{
			STRDUP(ship->owner, "");
			STRDUP(ship->pilot, "");
			STRDUP(ship->copilot, "");

			save_ship(ship);
		}

	for (auto* clan : clan_list)
		remove_member(clan, name);

	return;
}

/*
 R E Q U I R E M E N T S

 Sprawdzenie, czy gracz spe�nia wymagania by u�y� odpowiedniego klamotu.
 (c) Thanos
 */

#define REQ_LT 	-1		//czy cecha gracza ma by� MNIEJSZA NI� wymagana
#define REQ_EQ 	 0		//czy cecha gracza ma by� R�WNA wymaganej
#define REQ_GT 	 1		//czy cecha gracza ma by� MNIEJSZA NI� wymagana
#define REQ_BOOL 2		//czy ma poprostu istnie�

/* zwraca warto�� cechy w zale�no�ci od zapytania */
int req_query(CHAR_DATA *ch, REQUIREMENT_DATA *req)
{
	switch (req->location)
	{
	default:
		return 0;
	case REQ_STR:
		return get_curr_str(ch);
	case REQ_DEX:
		return get_curr_dex(ch);
	case REQ_INT:
		return get_curr_int(ch);
	case REQ_WIS:
		return get_curr_wis(ch);
	case REQ_CON:
		return get_curr_con(ch);
	case REQ_CHA:
		return get_curr_cha(ch);
	case REQ_LCK:
		return get_curr_lck(ch);
	case REQ_FRC:
		return get_curr_frc(ch);
	case REQ_CLASS:
		return ch->main_ability;
	case REQ_SEX:
		return ch->sex;
	case REQ_GOLD:
		return ch->gold;
	case REQ_AGE:
		return get_age(ch);
	case REQ_HIT:
		return ch->max_hit;
	case REQ_FORCE:
		return ch->max_mana;
	case REQ_MOVE:
		return ch->max_move;
	case REQ_LEVEL:
		return ch->top_level;
	case REQ_AC:
		return GET_AC(ch);
	case REQ_HITROLL:
		return GET_HITROLL(ch);
	case REQ_DAMROLL:
		return GET_DAMROLL(ch);
	case REQ_FULL:
		return IS_NPC(ch) ? 0 : ch->pcdata->condition[COND_FULL];
	case REQ_THIRST:
		return IS_NPC(ch) ? 0 : ch->pcdata->condition[COND_THIRST];
	case REQ_DRUNK:
		return IS_NPC(ch) ? 0 : ch->pcdata->condition[COND_DRUNK];
	case REQ_RACE:
		return 0; /* Trog: od mojej przerobki ras, to nie bedzie dzialac */
	case REQ_SKILL:
		return IS_NPC(ch) ? 0 : ch->pcdata->learned[req->type];
	case REQ_AFFECT:
		return IS_AFFECTED(ch, req->modifier);
	}
}

/* sprawdza typ por�wnania */
int req_eq(REQUIREMENT_DATA *req)
{
	switch (req->location)
	{
	case REQ_AC:
		return REQ_LT; /* musisz mie� mniej */
	case REQ_SEX:
	case REQ_CLASS:
	case REQ_RACE:
		return REQ_EQ; /* musisz mie� dok�adnie takie*/
	case REQ_AFFECT:
		return REQ_BOOL;/* wystarczy, �e masz */
	default:
		return REQ_GT; /* dla reszty: wiecej */
	}
}

void explain(CHAR_DATA *ch, REQUIREMENT_DATA *req, OBJ_DATA *obj, bool iden)
{
	const char *expl_table[] =
	{ "", "si�y", "zr�czno�ci", "inteligencji", "m�dro�ci", "kondycji", "charyzmy", "szcz�cia", "mocy", "p�ci", "kredytek", "lat",
			"punkt�w uderzeniowych", "punkt�w mocy", "punkt�w ruchu", "poziomu", "klasy zbroi", "hitrolla", "damrolla", "poziomu g�odu",
			"poziomu pragnienia", "poziomu upicia", "rasy", "znajomo�ci umiej�tno�ci", "bycia pod wp�ywem", "profesji podstawowej" };

	char expl[MSL];

	switch (req->location)
	{
	default:
		if (iden)
			sprintf(expl,
			FB_CYAN "%s" FG_CYAN " (" FB_CYAN "%d lub wi�cej" FG_CYAN ")", expl_table[req->location], req->modifier);
		else
			sprintf(expl, "Masz za ma�o %s", expl_table[req->location]);
		break;
	case REQ_SEX:
	case REQ_RACE:
	case REQ_CLASS:
		if (iden)
		{
			sprintf(expl, FB_CYAN "%s" FG_CYAN " (" FB_CYAN "%s" FG_CYAN ")", expl_table[req->location],
//			req->location == REQ_RACE ? race_table[req->modifier].przypadki[0] :
					req->location == REQ_CLASS ? class_table[req->modifier].przypadki[0] :
					/* Trog: powyzsze (rasy) nie bedzie dzialac */
					(req->modifier == SEX_MALE ? "m�skiej" : req->modifier == SEX_FEMALE ? "�e�skiej" : "nijakiej"));
		}
		else
			sprintf(expl, "Nie masz odpowiedniej %s", expl_table[req->location]);
		break;
	case REQ_LEVEL:
		if (iden)
			sprintf(expl,
			FB_CYAN "%s " FG_CYAN "(" FB_CYAN "%d lub wi�cej" FG_CYAN ")", expl_table[req->location], req->modifier);
		else
			sprintf(expl, "Musisz by� conajmniej %d %s", req->modifier, expl_table[req->location]);
		break;
	case REQ_SKILL:
		if (iden)
			sprintf(expl,
			FB_CYAN "%s " FG_CYAN "'" FB_CYAN "%s" FG_CYAN "' (" FB_CYAN "%d%%" FG_CYAN ")", expl_table[req->location],
					skill_table[req->type]->name, req->modifier);
		else
			sprintf(expl, "Za s�abo znasz umiej�tno�� '%s'", skill_table[req->type]->name);
		break;
	case REQ_AFFECT:
		if (iden)
			sprintf(expl, FB_CYAN "%s %s", expl_table[req->location], flag_string(aff_flags_list, req->modifier));
		else
			sprintf(expl, "Musisz by� pod wp�ywem %s", flag_string(aff_flags_list, req->modifier));
		break;
	case REQ_AC:
		if (iden)
			sprintf(expl, "%s " FG_CYAN "(" FB_CYAN "%d lub mniej" FG_CYAN ")", expl_table[req->location], req->modifier);
		else
			sprintf(expl, "Twoja klasa zbroi jest za wysoka");
		break;
	}

	if (iden)
	{
		ch_printf(ch, FG_CYAN "Wymaga: %s." EOL, expl);
	}
	else
	{
		ch_printf(ch, FB_CYAN "%s by u�ywa� %s %s." EOL, expl,
				obj->gender == GENDER_NEUTRAL ? "tego" : obj->gender == GENDER_MALE ? "tego" :
				obj->gender == GENDER_FEMALE ? "tej" : "tych", obj->przypadki[1]);
	}
	return;
}
/*
 przeszukuje wszystkie wymogi klamota i jak znajdzie pierwszy niespe�niony
 zwraca false
 */
#if defined (ARMAGEDDON)
bool req_check( CHAR_DATA *ch, OBJ_DATA *obj )
{
	REQUIREMENT_DATA *	req;
	int			eq;

	/* wymogi aktualne klamota */
	for( auto* req : obj->requirements )
	{
	eq = req_eq( req );

	if( ( 	eq == REQ_EQ ? req_query(ch, req) != req->modifier
	:	eq == REQ_LT ? req_query(ch, req) > req->modifier
	:       eq == REQ_GT ? req_query(ch, req) < req->modifier
	:    		      !req_query(ch, req) ) )
	{
		explain( ch, req, obj, false );
		return false;
	}
	}

	/* wymogi indexu */
	for( auto* req : obj->pIndexData->requirements )
	{
	eq = req_eq( req );

	if( ( 	eq == REQ_EQ ? req_query(ch, req) != req->modifier
	:	eq == REQ_LT ? req_query(ch, req) > req->modifier
	:       eq == REQ_GT ? req_query(ch, req) < req->modifier
	:    		      !req_query(ch, req) ) )
	{
		explain( ch, req, obj, false );
			return false;
	}
	}
	return true;
}
#else
bool req_check(CHAR_DATA *ch, OBJ_DATA *obj)
{
	return true;
}
#endif

void show_req(CHAR_DATA *ch, REQUIREMENT_DATA *req)
{
	req_eq(req);

	ch_printf(ch, "%-10s " FG_CYAN, bit_name(req_types_list, req->location));
	if (req->location == REQ_SKILL)
	{
		char buf[MSL];
		sprintf(buf, "'%s'", STRING_REQUIREMENT(req->location, req->modifier, req->type));
		ch_printf(ch, "%-30s %s", buf, req->location == REQ_SKILL ? itoa(req->modifier) : "");
	}
	else
		ch_printf(ch, "%-30s %s", " ", STRING_REQUIREMENT(req->location, req->modifier, req->type));

}
#undef REQ_LT
#undef REQ_EQ
#undef REQ_GT
#undef REQ_BOOL
/*
 R E Q U I R E M E N T S
 End of Section
 */

/*
 * The following code was written by Erwin Andreasen
 * Completely cleaned up by Thoric
 *
 util function, converts an 'advanced' ASCII-number-string into a number.
 Advanced strings can contain 'k' (or 'K') and 'm' ('M') in them, not just
 numbers. The letters multiply whatever is left of them by 1,000 and
 1,000,000 respectively. Example:

 14k = 14 * 1,000 = 14,000
 23m = 23 * 1,000,0000 = 23,000,000

 If any digits follow the 'k' or 'm', the are also added, but the number
 which they are multiplied is divided by ten, each time we get one left. This
 is best illustrated in an example :)

 14k42 = 14 * 1000 + 14 * 100 + 2 * 10 = 14420

 Of course, it only pays off to use that notation when you can skip many 0's.
 There is not much point in writing 66k666 instead of 66666, except maybe
 when you want to make sure that you get 66,666.

 More than 3 (in case of 'k') or 6 ('m') digits after 'k'/'m' are automatically
 disregarded. Example:

 14k1234 = 14,123

 If the number contains any other characters than digits, 'k' or 'm', the
 function returns 0. It also returns 0 if 'k' or 'm' appear more than
 once.
 */

/* Thanos: potrzebne do advatoi */
bool adv_is_number(char *arg)
{
	if (*arg == '\0')
		return false;

	for (; *arg != '\0'; arg++)
	{
		if (arg[0] == '-' || arg[0] == '+')
			continue;

		if (!isdigit(*arg) && *arg != 'k' && *arg != 'K' && *arg != 'm' && *arg != 'M')
			return false;
	}

	return true;
}

/* doda�em tutaj obs�ug� '+' i '-' bo nie by�o */
int adv_atoi(char *s)
{
	int number = 0;
	int multiplier = 0;
	bool minus = false;

	if (s[0] != '\0')
	{
		if (s[0] == '-')
		{
			minus = true;
			++s;
		}
		else /* plusik niby nam nie zmienia znaku */
		if (s[0] == '+') /* ale wska�nik musimy przesun��     */
		{
			minus = false;
			++s;
		}
	}

	/*
	 * as long as the current character is a digit add to current number
	 */
	while (isdigit(s[0]))
		number = (number * 10) + (*s++ - '0');

	switch (UPPER(s[0]))
	{
	case 'K':
		number *= (multiplier = 1000);
		++s;
		break;
	case 'M':
		number *= (multiplier = 1000000);
		++s;
		break;
	case '\0':
		break;
	default:
		return 0;
	}

	/* if any digits follow k/m, add those too */
	while (isdigit(s[0]) && (multiplier > 1))
	{
		/* the further we get to right, the less the digit 'worth' */
		multiplier /= 10;
		number = number + ((*s++ - '0') * multiplier);
	}

	/* return 0 if non-digit character was found, other than NULL */
	if (s[0] != '\0' && !isdigit(s[0]) && s[0] != '-' && s[0] != '+')
		return 0;

	/*
	 hmmm, a mo�e by da� '0 - abs( number )' ?
	 Eeeee tam :>
	 */
	if (minus)
		number *= -1;

	/* anything left is likely extra digits (ie: 14k4443  -> 3 is extra) */
	return number;
}

/* Trog: wersja 64-bitowa */
int64 atoi64(char *s)
{
	int64 number = 0;
	int multiplier = 0;
	bool minus = false;

	if (s[0] != '\0')
	{
		if (s[0] == '-')
		{
			minus = true;
			++s;
		}
		else /* plusik niby nam nie zmienia znaku */
		if (s[0] == '+') /* ale wska�nik musimy przesun��     */
		{
			minus = false;
			++s;
		}
	}

	/*
	 * as long as the current character is a digit add to current number
	 */
	while (isdigit(s[0]))
		number = (number * 10) + (*s++ - '0');

	switch (UPPER(s[0]))
	{
	case 'K':
		number *= (multiplier = 1000);
		++s;
		break;
	case 'M':
		number *= (multiplier = 1000000);
		++s;
		break;
	case '\0':
		break;
	default:
		return 0;
	}

	/* if any digits follow k/m, add those too */
	while (isdigit(s[0]) && (multiplier > 1))
	{
		/* the further we get to right, the less the digit 'worth' */
		multiplier /= 10;
		number = number + ((*s++ - '0') * multiplier);
	}

	/* return 0 if non-digit character was found, other than NULL */
	if (s[0] != '\0' && !isdigit(s[0]) && s[0] != '-' && s[0] != '+')
		return 0;

	/*
	 hmmm, a mo�e by da� '0 - abs( number )' ?
	 Eeeee tam :>
	 */
	if (minus)
		number *= -1;

	/* anything left is likely extra digits (ie: 14k4443  -> 3 is extra) */
	return number;
}

/* Trog */

/*
 int64 abs(int64 i)
 {
 return (i < 0) ? -i : i;
 }

 long int abs(long int i)
 {
 return (i < 0) ? -i : i;
 }
 */
int atoi(const char *s)
{
	return adv_atoi((char*) s);
}

int atoi(char *s)
{
	return adv_atoi(s);
}
