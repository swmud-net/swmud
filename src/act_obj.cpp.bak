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
 *       		   Object manipulation module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <time.h>
#include "mud.h"
#include "bet.h"
#include "classes/SWPlayerBounty.h"
#include "classes/SWPazaak.h"

/*double sqrt( double x );*/

/*
 * External functions
 */

void show_list_to_char(OBJ_DATA *list, CHAR_DATA *ch, bool fShort, bool fShowNothing);
/*
 * Local functions.
 */
void get_obj(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container);
bool remove_obj(CHAR_DATA *ch, int iWear, bool fReplace);
void wear_obj(CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, int wear_bit);
void write_corpses(CHAR_DATA *ch, char *name);

/*
 * how resistant an object is to damage				-Thoric
 */
int get_obj_resistance(OBJ_DATA *obj)
{
	int resist;

	resist = number_fuzzy(MAX_ITEM_IMPACT);

	/* forceal items are more resistant */
	if (IS_OBJ_STAT( obj, ITEM_FORCE ))
		resist += number_fuzzy(12);
	/* blessed objects should have a little bonus */
	if (IS_OBJ_STAT( obj, ITEM_BLESS ))
		resist += number_fuzzy(5);
	/* lets make store inventory pretty tough */
	if (IS_OBJ_STAT( obj, ITEM_INVENTORY ))
		resist += 20;

	/* okay... let's add some bonus/penalty for item level... */
#if defined( ARMAGEDDON )
	resist += obj->level;
#else
	resist += (obj->level / 10);
#endif

	/* and lasty... take armor or weapon's condition into consideration */
	if (obj->item_type == ITEM_ARMOR || obj->item_type == ITEM_WEAPON)
		resist += (obj->value[0]);

	/* Thanos -->   ¿enada, ¿e komlinki rozpadaj± siê jako pierwsze */
	if (obj->item_type == ITEM_COMLINK)
		resist *= 2;

#if defined( ARMAGEDDON )
	return UMAX(10, resist);
#else
	return URANGE(10, resist, 99);
#endif
}

void show_obj_stats(OBJ_DATA *obj, CHAR_DATA *ch)
{
	AFFECT_DATA * paf;
	SKILLTYPE * sktmp;
#if defined(ARMAGEDDON)
	REQUIREMENT_DATA *req;
#endif

	ch_printf(ch, FG_CYAN "Przedmiot '" FB_CYAN "%s" FG_CYAN "' jest typu %s." EOL
	FG_CYAN "Dodatkowe flagi: " FB_CYAN "%s %s" EOL, obj->name, item_type_name(obj), flag_string(obj_flags_list,
			obj->extra_flags), !str_cmp(flag_string(force_flags_list, obj->force_flags), "none") ? "" : flag_string(
			force_flags_list, obj->force_flags));
	ch_printf(ch, FG_CYAN
	"Waga: " FB_CYAN "%d" FG_CYAN ", Poziom: " FB_CYAN "%d" EOL, obj->weight, obj->level);

	switch (obj->item_type)
	{
	case ITEM_WEAPON:
		ch_printf(ch, FG_CYAN
		"Zadawane obra¿enia w walce wrêcz " FB_CYAN "%d" FG_CYAN
		" do " FB_CYAN "%d" FG_CYAN " (¶rednio " FB_CYAN "%d" FG_CYAN ")." EOL, obj->value[1], obj->value[2],
				(obj->value[1] + obj->value[2]) / 2);

		if (obj->value[3] == WEAPON_BLASTER)
		{
			if (obj->blaster_setting == BLASTER_FULL)
				ch_printf(ch, FG_CYAN "Ustawiony na PE£N¡ moc." EOL);
			else if (obj->blaster_setting == BLASTER_HIGH)
				ch_printf(ch, FG_CYAN "Ustawiony na WYSOKI poziom mocy." EOL);
			else if (obj->blaster_setting == BLASTER_NORMAL)
				ch_printf(ch, FG_CYAN "Ustawiony na NORMALNY poziom mocy." EOL);
			else if (obj->blaster_setting == BLASTER_HALF)
				ch_printf(ch, FG_CYAN "Ustawiony na PO£OWÊ mocy." EOL);
			else if (obj->blaster_setting == BLASTER_LOW)
				ch_printf(ch, FG_CYAN "Ustawiony na NISKI poziom mocy." EOL);
			else if (obj->blaster_setting == BLASTER_STUN)
				ch_printf(ch, FG_CYAN "Ustawiony na OG£USZANIE." EOL);

			ch_printf(ch, FG_CYAN
			"Posiada jeszcze " FB_CYAN "%d" FG_CYAN " na " FB_CYAN "%d" FG_CYAN " pocisków." EOL, obj->value[4],
					obj->value[5]);
		}
		else if (obj->value[3] == WEAPON_LIGHTSABER || obj->value[3] == WEAPON_VIBRO_BLADE || obj->value[3]
				== WEAPON_FORCE_PIKE)
		{
			ch_printf(ch, FG_CYAN
			"Posiada jeszcze " FB_CYAN "%d" FG_CYAN " z " FB_CYAN
			"%d" FG_CYAN " jednostek energetycznych." EOL, obj->value[4], obj->value[5]);
		}
		else if (obj->value[3] == WEAPON_BOWCASTER)
		{
			ch_printf(ch, FG_CYAN
			"Posiada jeszcze " FB_CYAN "%d" FG_CYAN " z " FB_CYAN
			"%d" FG_CYAN " strza³ energetycznych." NL, obj->value[4], obj->value[5]);
		}
		break;

	case ITEM_DEVICE:
		ch_printf(ch, FG_CYAN
		"Posiada " FB_CYAN "%d" FG_CYAN "(" FB_CYAN "%d" FG_CYAN
		") ³adunków na poziomie " FB_CYAN "%d ", obj->value[1], obj->value[2], obj->value[0]);
		if (obj->value[3] >= 0 && (sktmp = get_skilltype(obj->value[3])) != NULL)
		{
			send_to_char(" '", ch);
			send_to_char(sktmp->name, ch);
			send_to_char("'", ch);
		}
		send_to_char(FG_CYAN "." EOL, ch);
		break;

	case ITEM_AMMO:
		ch_printf(ch, FG_CYAN
		"Posiada " PLAIN "%d" FG_CYAN " pocisków." EOL, obj->value[0]);
		break;

	case ITEM_BOLT:
		ch_printf(ch, FG_CYAN
		"Posiada " FB_CYAN "%d" FG_CYAN " strza³ energetycznych." EOL, obj->value[0]);
		break;

	case ITEM_BATTERY:
		ch_printf(ch, FG_CYAN
		"Posiada " FB_CYAN "%d" FG_CYAN " jednostek energii." EOL, obj->value[0]);
		break;

	case ITEM_ARMOR:
		ch_printf(ch, FG_CYAN
		"Aktualna klasa zbroi: " FB_CYAN "%d" FG_CYAN "/" FB_CYAN "%d" FG_CYAN "." EOL, obj->value[0], obj->value[1]);
		ch_printf(ch, FG_CYAN
		"Dodatkowy wp³yw na uzbrojenie: " FB_CYAN "%d" FG_CYAN "." EOL, apply_ac(obj, obj->wear_loc));
		break;

	case ITEM_PILL:
	case ITEM_POTION:
		ch_printf(ch, FG_CYAN "Daje:");

		if (obj->value[1] >= 0 && (sktmp = get_skilltype(obj->value[1])) != NULL)
		{
			send_to_char(" '" FB_CYAN, ch);
			send_to_char(sktmp->name, ch);
			send_to_char(FG_CYAN "'", ch);
		}

		if (obj->value[2] >= 0 && (sktmp = get_skilltype(obj->value[2])) != NULL)
		{
			send_to_char(" '" FB_CYAN, ch);
			send_to_char(sktmp->name, ch);
			send_to_char(FG_CYAN "'", ch);
		}

		if (obj->value[3] >= 0 && (sktmp = get_skilltype(obj->value[3])) != NULL)
		{
			send_to_char(" '" FB_CYAN, ch);
			send_to_char(sktmp->name, ch);
			send_to_char(FG_CYAN "'", ch);
		}

		ch_printf(ch, " na poziom " FB_CYAN "%d" FG_CYAN "." NL, obj->value[0]);
		break;
	}

	for (paf = obj->first_affect; paf; paf = paf->next)
		showaffect(ch, paf);
	for (paf = obj->pIndexData->first_affect; paf; paf = paf->next)
		showaffect(ch, paf);

#if defined(ARMAGEDDON)
	for( req = obj->first_requirement; req; req=req->next )
	explain( ch, req, obj, true );
	for( req = obj->pIndexData->first_requirement; req; req=req->next )
	explain( ch, req, obj, true );
#endif
	send_to_char(PLAIN, ch);

	return;
}

void get_obj(CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container)
{
	int weight;

	if (!CAN_WEAR(obj, ITEM_TAKE) && (ch->top_level < sysdata.level_getobjnotake))
	{
		send_to_char("Nie mo¿esz tego wzi±æ." NL, ch);
		return;
	}

	if (IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && !can_take_proto(ch))
	{
		send_to_char("Nieznana moc powstrzymuje ciê przed podej¶ciem by to wzi±æ." NL, ch);
		return;
	}

	/* Trog: tutaj tez sprawdzamy personale, bo jak ktos da get all, a sa co najmniej
	 * 2 personale nie jego, to sie mud zapetla dlatego, ze dropujemy na koniec
	 * listy, znowu tam dochodzimy i tak az do... */
	if (IS_OBJ_STAT( obj, ITEM_PERSONAL ) && *obj->owner_name && str_cmp(obj->owner_name, ch->name))
	{
		ch_printf(ch, "To chyba nie twoje. Spadówa!" NL "Nie mo¿esz tego wzi±æ." NL);
		return;
	}

	/* Thanos -- kasa to raczej jest ma³a i lekka nie ? */
	if (obj->item_type != ITEM_MONEY)
	{
		if (ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
		{
			act(PLAIN, "$d: Nie mo¿esz nosiæ tylu przedmiotów.", ch, NULL, obj->name, TO_CHAR);
			return;
		}

		if (IS_OBJ_STAT( obj, ITEM_COVERING ))
			weight = obj->weight;
		else
			weight = get_obj_weight(obj);

		if (ch->carry_weight + weight > can_carry_w(ch) && !is_bounty_object(obj))
		{
			act(PLAIN, "$d: Nie uniesiesz takiego ciê¿aru.", ch, NULL, obj->name, TO_CHAR);
			return;
		}
	}

	if (container)
	{
		act(PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ? "Wyci±gasz $p$3 spod $P$1." : "Wyci±gasz $p$3 z $P$1", ch,
				obj, container, TO_CHAR);
		act(PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ? "$n wyci±ga $p$3 spod $P$1." : "$n wyci±ga $p$3 z $P$1", ch,
				obj, container, TO_ROOM);
		obj_from_obj(obj);
	}
	else
	{
		act(PLAIN, "Podnosisz $p$3.", ch, obj, container, TO_CHAR);
		act(PLAIN, "$n podnosi $p$3.", ch, obj, container, TO_ROOM);
		obj_from_room(obj);
	}

	/* Clan storeroom checks here */

	if (obj->item_type != ITEM_CONTAINER)
		check_for_trap(ch, obj, TRAP_GET);
	if (char_died(ch))
		return;

	if (obj->item_type == ITEM_MONEY)
	{
		ch->gold += obj->value[0];
		extract_obj(obj);
	}
	else
	{
		obj = obj_to_char(obj, ch);
	}

	if (char_died(ch) || obj_extracted(obj))
		return;
	oprog_get_trigger(ch, obj);

	return;
}

DEF_DO_FUN( get )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	OBJ_DATA *container;
	int number;
	bool found;

	argument = one_argument(argument, arg1);
	if (is_number(arg1))
	{
		number = atoi(arg1);
		if (number < 1)
		{
			send_to_char("To by³o naprawdê ³atwe..." NL, ch);
			return;
		}
		if ((ch->carry_number + number) > can_carry_n(ch))
		{
			send_to_char("Nie uniesiesz tyle." NL, ch);
			return;
		}
		argument = one_argument(argument, arg1);
	}
	else
		number = 0;
	argument = one_argument(argument, arg2);
	/* munch optional words */
	if (!str_cmp(arg2, "from") && argument[0] != '\0')
		argument = one_argument(argument, arg2);

	/* Get type. */
	if (arg1[0] == '\0')
	{
		send_to_char("Wzi±æ co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if (arg2[0] == '\0')
	{
		if (number <= 1 && str_cmp(arg1, "all") && str_prefix("all.", arg1))
		{
			/* 'get obj' */
			obj = get_obj_list(ch, arg1, ch->in_room->first_content);
			if (!obj)
			{
				act(PLAIN, "Nie ma tu ¿adnego $T.", ch, NULL, arg1, TO_CHAR);
				return;
			}
			separate_obj(obj);
			get_obj(ch, obj, NULL);
			if (char_died(ch))
				return;
			if (IS_SET( sysdata.save_flags, SV_GET ))
				save_char_obj(ch);
		}
		else
		{
			int cnt = 0;
			bool fAll;
			char *chk;

			if (IS_SET( ch->in_room->room_flags, ROOM_DONATION ))
			{
				send_to_char("W³adcy ¶miej± siê z twojej chciwo¶ci!" NL, ch);
				return;
			}
			if (!str_cmp(arg1, "all"))
				fAll = true;
			else
				fAll = false;
			if (number > 1)
				chk = arg1;
			else
				chk = &arg1[4];
			/* 'get all' or 'get all.obj' */
			found = false;
			for (obj = ch->in_room->first_content; obj; obj = obj_next)
			{
				obj_next = obj->next_content;
				if ((fAll || nifty_is_name(chk, obj->name)) && can_see_obj(ch, obj))
				{
					found = true;
					if (number && (cnt + obj->count) > number)
						split_obj(obj, number - cnt);
					cnt += obj->count;
					get_obj(ch, obj, NULL);
					if (char_died(ch) || ch->carry_number >= can_carry_n(ch) || ch->carry_weight >= can_carry_w(ch)
							|| (number && cnt >= number))
					{
						if (IS_SET(sysdata.save_flags, SV_GET) && !char_died(ch))
							save_char_obj(ch);
                        if (ch->in_room && (ch->in_room->room_flags & ROOM_STORAGE)) {
                            save_room_storage(ch->in_room);
                        }
						return;
					}
				}
			}

			if (!found)
			{
				if (fAll)
					send_to_char("Nic tu nie widzisz." NL, ch);
				else
					act(PLAIN, "Nie widzisz tu ¿adnego $T.", ch, NULL, chk, TO_CHAR);
			}
			else if (IS_SET( sysdata.save_flags, SV_GET ))
				save_char_obj(ch);
		}
	}
	else
	{
		/* 'get ... container' */
		if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2))
		{
			send_to_char("Nie mo¿esz tego zrobiæ." NL, ch);
			return;
		}

		if ((container = get_obj_here(ch, arg2)) == NULL)
		{
			act(PLAIN, "Nie widzisz tu ¿adnego $T.", ch, NULL, arg2, TO_CHAR);
			return;
		}

		switch (container->item_type)
		{
		default:
			if (!IS_OBJ_STAT( container, ITEM_COVERING ))
			{
				send_to_char("To nie jest pojemnik." NL, ch);
				return;
			}
			if (ch->carry_weight + container->weight > can_carry_w(ch))
			{
				send_to_char("Zbyt ciê¿ko by ud¼wign±æ." NL, ch);
				return;
			}
			break;

		case ITEM_CONTAINER:
		case ITEM_PAZAAK_SIDE_DECK:
		case ITEM_DROID_CORPSE:
		case ITEM_CORPSE_PC:
		case ITEM_CORPSE_NPC:
			break;
		}

		if (!IS_OBJ_STAT(container, ITEM_COVERING ) && IS_SET(container->value[1], CONT_CLOSED))
		{
			act(PLAIN, "$d jest zamkniêty.", ch, NULL, container->name, TO_CHAR);
			return;
		}

		if (number <= 1 && str_cmp(arg1, "all") && str_prefix("all.", arg1))
		{
			/* 'get obj container' */

			if (container->item_type == ITEM_PAZAAK_SIDE_DECK)
			{
				SWDataBase db;
				SWPazaakCard *card = SWPazaak::getPlayerCard(ch->name, SWInt::fromString(arg1), &db);
				if (card)
				{
					SWPazaak::removeCard(*card, ch->name, &db);
					//because of pazaak, here the obj should be created conditionally
					obj = pazaak_create_card_obj(*card);
					if (obj)
					{
						obj_to_char(obj, ch);
						act(PLAIN, "Wyjmujesz $p$3 z $P$1.", ch, obj, container, TO_CHAR);
					}

					delete card;
				}
				else
				{
					act(PLAIN, "Nie widzisz nic takiego w $p$5.", ch, container, NULL, TO_CHAR);
					return;
				}
			}
			else
			{
				obj = get_obj_list(ch, arg1, container->first_content);
				if (!obj)
				{
					if (IS_OBJ_STAT(container, ITEM_COVERING))
						act(PLAIN, "Nie widzisz nic takiego pod $p$4.", ch, container, NULL, TO_CHAR);
					else
						act(PLAIN, "Nie widzisz nic takiego w $p$5.", ch, container, NULL, TO_CHAR);
					return;
				}
				separate_obj(obj);
				get_obj(ch, obj, container);
			}

			/* Thanos - no more corpse duplicate */
			if (container->item_type == ITEM_CORPSE_PC)
				write_corpses(NULL, container->action_desc);

			check_for_trap(ch, container, TRAP_GET);
			if (char_died(ch))
				return;
			if (IS_SET( sysdata.save_flags, SV_GET ))
				save_char_obj(ch);
		}
		else
		{
			int cnt = 0;
			bool fAll;
			char *chk;

			/* 'get all container' or 'get all.obj container' */
			if (IS_OBJ_STAT( container, ITEM_DONATION ))
			{
				send_to_char("Mistrzowie Jedi s± oburzeni twoj± chciwo¶ci±!" NL, ch);
				return;
			}
			if (!str_cmp(arg1, "all"))
				fAll = true;
			else
				fAll = false;
			if (number > 1)
				chk = arg1;
			else
				chk = &arg1[4];
			found = false;
			SWDataBase db;
			if (container->item_type == ITEM_PAZAAK_SIDE_DECK)
			{
				int i = 1;
				list<SWPazaakCard> *cards = SWPazaak::getCards(ch->name, &db);
				if (cards)
				{
					for (list<SWPazaakCard>::const_iterator it = cards->begin(); it != cards->end(); ++it, ++i)
					{
						if (fAll || i == SWInt::fromString(chk))
						{
							SWPazaak::removeCard(*it, ch->name, &db);
							//because of pazaak, here the obj should be created conditionally
							obj = pazaak_create_card_obj(*it);
							if (obj)
							{
								obj_to_char(obj, ch);
								act(PLAIN, "Wyjmujesz $p$3 z $P$1.", ch, obj, container, TO_CHAR);
							}

							found = true;
						}
					}
				}
				else
				{
					act(PLAIN, "Nie widzisz nic takiego w $p$5.", ch, container, NULL, TO_CHAR);
					return;
				}
			}
			else
			{
				for (obj = container->first_content; obj; obj = obj_next)
				{
					obj_next = obj->next_content;
					if ((fAll || nifty_is_name(chk, obj->name)) && can_see_obj(ch, obj))
					{
						found = true;
						if (number && (cnt + obj->count) > number)
							split_obj(obj, number - cnt);
						cnt += obj->count;
						get_obj(ch, obj, container);
						if (char_died(ch) || ch->carry_number >= can_carry_n(ch) || ch->carry_weight >= can_carry_w(ch)
								|| (number && cnt >= number)) {
                            if (ch->in_room && (ch->in_room->room_flags & ROOM_STORAGE)) {
                                save_room_storage(ch->in_room);
                            }
							return;
                        }
					}
				}
			}

			if (!found)
			{
				if (fAll)
					act(PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ? "Nie widzisz nic pod $T."
							: "Nie widzisz nic w $T.", ch, NULL, arg2, TO_CHAR);
				else
					act(PLAIN, IS_OBJ_STAT(container, ITEM_COVERING) ? "Nic takiego nie ma pod $T."
							: "Nic takiego nie ma w $T.", ch, NULL, arg2, TO_CHAR);
				return;
			}

			check_for_trap(ch, container, TRAP_GET);
			if (char_died(ch))
				return;

			/* Thanos - no more corpse duplicate */
			if (container->item_type == ITEM_CORPSE_PC)
				write_corpses(NULL, container->action_desc);

			if (found && IS_SET( sysdata.save_flags, SV_GET ))
				save_char_obj(ch);
		}
	}
    if (ch->in_room && (ch->in_room->room_flags & ROOM_STORAGE)) {
        save_room_storage(ch->in_room);
    }
	return;
}

DEF_DO_FUN( put )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *container;
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	int count;
	int number;
	bool save_char = false;

	argument = one_argument(argument, arg1);
	if (is_number(arg1))
	{
		number = atoi(arg1);
		if (number < 1)
		{
			send_to_char("To by³o naprawdê ³atwe..." NL, ch);
			return;
		}
		argument = one_argument(argument, arg1);
	}
	else
		number = 0;
	argument = one_argument(argument, arg2);
	/* munch optional words */
	if ((!str_cmp(arg2, "into") || !str_cmp(arg2, "inside") || !str_cmp(arg2, "in") || !str_cmp(arg2, "w") || !str_cmp(
			arg2, "do")) && argument[0] != '\0')
		argument = one_argument(argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("W³o¿yæ co do czego?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if (!str_cmp(arg2, "all") || !str_prefix("all.", arg2))
	{
		send_to_char("Nie mo¿esz tego zrobiæ." NL, ch);
		return;
	}

	if ((container = get_obj_here(ch, arg2)) == NULL)
	{
		act(PLAIN, "Nie widzisz tu ¿adnego $T.", ch, NULL, arg2, TO_CHAR);
		return;
	}

	if (!container->carried_by && IS_SET( sysdata.save_flags, SV_PUT ))
		save_char = true;

	if (IS_OBJ_STAT(container, ITEM_COVERING))
	{
		if (ch->carry_weight + container->weight > can_carry_w(ch))
		{
			send_to_char("Nie uniesiesz takiego ciê¿aru." NL, ch);
			return;
		}
	}
	else
	{
		if (container->item_type != ITEM_CONTAINER && container->item_type != ITEM_PAZAAK_SIDE_DECK)
		{
			send_to_char("To nie jest pojemnik." NL, ch);
			return;
		}

		if (IS_SET(container->value[1], CONT_CLOSED))
		{
			act(PLAIN, "$d jest zamkniêty.", ch, NULL, container->name, TO_CHAR);
			return;
		}
	}

	if (number <= 1 && str_cmp(arg1, "all") && str_prefix("all.", arg1))
	{
		/* 'put obj container' */
		if ((obj = get_obj_carry(ch, arg1)) == NULL)
		{
			send_to_char("Nie masz tego przedmiotu." NL, ch);
			return;
		}

		if (obj == container)
		{
			send_to_char("Hmm.. Nie da siê w³o¿yæ przedmiotu do niego samego." NL, ch);
			return;
		}

		if (!can_drop_obj(ch, obj))
		{
			send_to_char("Nie mo¿esz tego zrobiæ." NL, ch);
			return;
		}

		/* Thanos: -- personale,
		 *if( IS_OBJ_STAT( obj, ITEM_PERSONAL )
		 *&& *obj->owner_name && !str_cmp( obj->owner_name, ch->name )
		 *&& !IS_NPC( ch ) )
		 *{
		 *    ch_printf( ch,
		 *    "Przecie¿ ty i %s" PLAIN " to tak zgrany duet. Nie rozstaniecie siê ani na chwilê!" NL,
		 *	obj->przypadki[0] );
		 *    return;
		 *}
		 *   		Pixel -- poprawka, chowanie i wyrzucanie personali dozwolone, zeby nie zasmiecaly inv.
		 */

		if ((IS_OBJ_STAT(container, ITEM_COVERING) && (get_obj_weight(obj) / obj->count) > ((get_obj_weight(container)
				/ container->count) - container->weight)))
		{
			send_to_char("To siê tam nie zmie¶ci." NL, ch);
			return;
		}

		if ((get_obj_weight(obj) / obj->count) + (get_obj_weight(container) / container->count) > container->value[0])
		{
			send_to_char("To siê nie zmie¶ci." NL, ch);
			return;
		}

		separate_obj(obj);
		separate_obj(container);
		obj_from_char(obj);
		if (obj->item_type == ITEM_PAZAAK_CARD)
		{
			SWPazaakCard *card = SWPazaakCard::newInstance(obj);
			if (card)
			{
				SWPazaak::addCard(*card, ch->name, 0);
				delete card;
			}

			act(PLAIN, "Wk³adasz $p$3 do $P$1.", ch, obj, container, TO_CHAR);

			//because of pazaak, here the obj should be extracted/removed conditionally
			extract_obj(obj);
		}
		else
		{
			obj = obj_to_obj(obj, container);
			check_for_trap(ch, container, TRAP_PUT);
			if (char_died(ch))
				return;
			count = obj->count;
			obj->count = 1;
			act(PLAIN, IS_OBJ_STAT( container, ITEM_COVERING ) ? "$n chowa $p$3 pod $P$3." : "$n wk³ada $p$3 do $P$1.",
					ch, obj, container, TO_ROOM);
			act(PLAIN, IS_OBJ_STAT( container, ITEM_COVERING ) ? "Chowasz $p$3 pod $P$3." : "Wk³adasz $p$3 do $P$1.",
					ch, obj, container, TO_CHAR);
			obj->count = count;

			/* Thanos - no more corpse duplicate */
			if (container->item_type == ITEM_CORPSE_PC)
				write_corpses(NULL, container->action_desc);
		}

		if (save_char)
			save_char_obj(ch);

		/* Clan storeroom check here */

	}
	else
	{
		bool found = false;
		int cnt = 0;
		bool fAll;
		char *chk;

		if (!str_cmp(arg1, "all"))
			fAll = true;
		else
			fAll = false;
		if (number > 1)
			chk = arg1;
		else
			chk = &arg1[4];

		separate_obj(container);
		SWDataBase db;
		/* 'put all container' or 'put all.obj container' */
		for (obj = ch->first_carrying; obj; obj = obj_next)
		{
			obj_next = obj->next_content;

			if ((fAll || nifty_is_name(chk, obj->name)) && can_see_obj(ch, obj) && obj->wear_loc == WEAR_NONE && obj
					!= container && can_drop_obj(ch, obj) && get_obj_weight(obj) + get_obj_weight(container)
					<= container->value[0])
			{
				if (number && (cnt + obj->count) > number)
					split_obj(obj, number - cnt);
				cnt += obj->count;
				obj_from_char(obj);
				act(COL_ACTION, "$n wk³ada $p$3 do $P$1.", ch, obj, container, TO_ROOM);
				act(COL_ACTION, "Wk³adasz $p$3 do $P$1.", ch, obj, container, TO_CHAR);
				if (obj->count == 1 && obj->item_type == ITEM_PAZAAK_CARD)
				{
					SWPazaakCard *card = SWPazaakCard::newInstance(obj);
					if (card)
					{
						SWPazaak::addCard(*card, ch->name, &db);
						delete card;
					}
					//because of pazaak, here the obj should be extracted/removed conditionally
					extract_obj(obj);
				}
				else
				{
					obj = obj_to_obj(obj, container);
				}
				found = true;

				check_for_trap(ch, container, TRAP_PUT);
				if (char_died(ch))
					return;

				if (number && cnt >= number)
					break;
			}
		}

		/*
		 * Don't bother to save anything if nothing was dropped   -Thoric
		 */
		if (!found)
		{
			if (fAll)
				act(PLAIN, "Nic nie niesiesz.", ch, NULL, NULL, TO_CHAR);
			else
				act(PLAIN, "Nie niesiesz ¿adnego $T.", ch, NULL, chk, TO_CHAR);
			return;
		}

		if (save_char)
			save_char_obj(ch);

		/* Thanos - no more corpse duplicate */
		if (container->item_type == ITEM_CORPSE_PC)
			write_corpses(NULL, container->action_desc);

		/* Clan storeroom check here */
	}
}

DEF_DO_FUN( drop )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *obj_next;
	bool found;
	int number;

	argument = one_argument(argument, arg);
	if (is_number(arg))
	{
		number = atoi(arg);
		if (number < 1)
		{
			send_to_char("To by³o naprawdê ³atwe..." NL, ch);
			return;
		}
		argument = one_argument(argument, arg);
	}
	else
		number = 0;

	if (arg[0] == '\0')
	{
		send_to_char("Odrzuciæ co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if (IS_SET( ch->in_room->room_flags, ROOM_NODROP ) || (!IS_NPC(ch) && IS_SET( ch->act, PLR_LITTERBUG )))
	{
		send_to_char(COL_FORCE, ch);
		send_to_char("Moc powstrzymuje ciê!" EOL, ch);
		send_to_char(COL_TELL, ch);
		send_to_char("Kto¶ mówi ci 'Bez ¶miecenia proszê!'" EOL, ch);
		return;
	}

	if (number > 0)
	{
		/* 'drop NNNN coins' */

		if (!str_cmp(arg, "credits") || !str_cmp(arg, "credit") || !str_cmp(arg, "kredytek") || !str_cmp(arg,
				"kredytka") || !str_cmp(arg, "kredytki") || !str_cmp(arg, "kredytke"))
		{
			if (ch->gold < number)
			{
				send_to_char("Nie masz tylu kredytek." NL, ch);
				return;
			}

			ch->gold -= number;

			for (obj = ch->in_room->first_content; obj; obj = obj_next)
			{
				obj_next = obj->next_content;

				switch (obj->pIndexData->vnum)
				{
				case OBJ_VNUM_MONEY_ONE:
					number += 1;
					extract_obj(obj);
					break;

				case OBJ_VNUM_MONEY_SOME:
					number += obj->value[0];
					extract_obj(obj);
					break;
				}
			}

			act(PLAIN, "$n odrzuca kilka kredytek.", ch, NULL, NULL, TO_ROOM);
			obj_to_room(create_money(number), ch->in_room);
			send_to_char("OK." NL, ch);
			if (IS_SET( sysdata.save_flags, SV_DROP ))
				save_char_obj(ch);
			return;
		}
	}

	if (number <= 1 && str_cmp(arg, "all") && str_prefix("all.", arg))
	{
		/* 'drop obj' */
		if ((obj = get_obj_carry(ch, arg)) == NULL)
		{
			send_to_char("Nie masz takiego przedmiotu." NL, ch);
			return;
		}

		if (!can_drop_obj(ch, obj))
		{
			send_to_char("Nie mo¿esz go odrzuciæ." NL, ch);
			return;
		}

		separate_obj(obj);
		act(PLAIN, "$n odrzuca $p$3.", ch, obj, NULL, TO_ROOM);
		act(PLAIN, "Odrzucasz $p$3.", ch, obj, NULL, TO_CHAR);

		obj_from_char(obj);
		obj = obj_to_room(obj, ch->in_room);
		oprog_drop_trigger(ch, obj); /* mudprogs */

		if (char_died(ch) || obj_extracted(obj))
			return;

		/* Clan storeroom saving here */

	}
	else
	{
		int cnt = 0;
		char *chk;
		bool fAll;

		if (!str_cmp(arg, "all"))
			fAll = true;
		else
			fAll = false;
		if (number > 1)
			chk = arg;
		else
			chk = &arg[4];
		/* 'drop all' or 'drop all.obj' */
		if (IS_SET( ch->in_room->room_flags, ROOM_NODROPALL ))
		{
			send_to_char("Nie mo¿esz zrobiæ tego tutaj..." NL, ch);
			return;
		}
		found = false;
		for (obj = ch->first_carrying; obj; obj = obj_next)
		{
			obj_next = obj->next_content;

			if ((fAll || nifty_is_name(chk, obj->name)) && can_see_obj(ch, obj) && obj->wear_loc == WEAR_NONE
					&& can_drop_obj(ch, obj))
			{
				found = true;
				if (obj->pIndexData->progtypes & DROP_PROG && obj->count > 1)
				{
					++cnt;
					separate_obj(obj);
					obj_from_char(obj);
					if (!obj_next)
						obj_next = ch->first_carrying;
				}
				else
				{
					if (number && (cnt + obj->count) > number)
						split_obj(obj, number - cnt);
					cnt += obj->count;
					obj_from_char(obj);
				}
				act(PLAIN, "$n odrzuca $p$3.", ch, obj, NULL, TO_ROOM);
				act(PLAIN, "Odrzucasz $p$3.", ch, obj, NULL, TO_CHAR);
				obj = obj_to_room(obj, ch->in_room);
				oprog_drop_trigger(ch, obj); /* mudprogs */
				if (char_died(ch))
					return;
				if (number && cnt >= number)
					break;
			}
		}

		/* Clan storeroom chech here */

		if (!found)
		{
			if (fAll)
				act(PLAIN, "Nic nie niesiesz!", ch, NULL, NULL, TO_CHAR);
			else
				act(PLAIN, "Nie niesiesz ¿adnego $T.", ch, NULL, chk, TO_CHAR);
		}
	}
	if (ch->in_room && (ch->in_room->room_flags & ROOM_STORAGE)) {
		save_room_storage(ch->in_room);
	}
	if (IS_SET( sysdata.save_flags, SV_DROP ))
		save_char_obj(ch); /* duping protector */
	return;
}

/* special.c -- Thanos */
void police_bribe_trigger(CHAR_DATA *mob, CHAR_DATA *ch, int amount);

DEF_DO_FUN( give )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *obj;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	if (!str_cmp(arg2, "to") && argument[0] != '\0')
		argument = one_argument(argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Daæ co komu?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if (is_number(arg1))
	{
		/* 'give NNNN coins victim' */
		int amount;

		amount = atoi(arg1);
		if (amount <= 0 || (str_cmp(arg2, "kredytek") && str_cmp(arg2, "kredytki") && str_cmp(arg2, "kredytke")
				&& str_cmp(arg2, "kredytka") && str_cmp(arg2, "kasa") && str_cmp(arg2, "kasy") && str_cmp(arg2,
				"pieniadz") && str_cmp(arg2, "pineniadze") && str_cmp(arg2, "money") && str_prefix(arg2, "credits")))
		{
			send_to_char("Przykro mi, nie mo¿esz tego zrobiæ." NL, ch);
			return;
		}

		argument = one_argument(argument, arg2);
		if (!str_cmp(arg2, "to") && argument[0] != '\0')
			argument = one_argument(argument, arg2);
		if (arg2[0] == '\0')
		{
			send_to_char("Daæ co komu?" NL, ch);
			return;
		}

		if ((victim = get_char_room(ch, arg2)) == NULL)
		{
			send_to_char("Nie ma tu nikogo takiego." NL, ch);
			return;
		}

		if (ch->gold < amount)
		{
			send_to_char("Bardzo mi³o z twojej strony i chojnie, ale nie masz tyle." NL, ch);
			return;
		}

		/* Trog: tutaj nadal jest ten bug, ze gosc robi postac, oddaje od razu te 10k
		 * z questu jakiemus mobowi, ktorego pozniej klepie silniejszym graczem.
		 * Poki co nie wiem jak to obejsc.
		 */
		if (IS_NEWBIE( ch ) && !IS_NPC( victim ))
		{
			send_to_char("Póki co sam potrzebujesz tej gotówki. To nie sk±pstwo, to zdrowy rozs±dek!" NL, ch);
			return;
		}

		ch->gold -= amount;
		victim->gold += amount;
		strcpy(buf, "$n daje ci ");
		strcat(buf, arg1);
		strcat(buf, (amount > 1) ? (amount < 5) ? " kredytki." : " kredytek." : " kredytkê.");

		act(PLAIN, buf, ch, NULL, victim, TO_VICT);
		act(PLAIN, "$n daje $N$2 kilka kredytek.", ch, NULL, victim, TO_NOTVICT);
		act(PLAIN, "Dajesz $N$2 kilka kredytek.", ch, NULL, victim, TO_CHAR);
		send_to_char("OK." NL, ch);
		mprog_bribe_trigger(victim, ch, amount);
		police_bribe_trigger(victim, ch, amount); /* Thanos */
		if (IS_SET( sysdata.save_flags, SV_GIVE ) && !char_died(ch))
			save_char_obj(ch);
		if (IS_SET( sysdata.save_flags, SV_RECEIVE ) && !char_died(victim))
			save_char_obj(victim);
		return;
	}

	if ((obj = get_obj_carry(ch, arg1)) == NULL)
	{
		send_to_char("Nie masz takiego przedmiotu." NL, ch);
		return;
	}

	if (obj->wear_loc != WEAR_NONE)
	{
		send_to_char("Musisz to najpierw zdj±æ." NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg2)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	/* Thanos: 	questy; (can_drop_obj nie pozwala pozbyæ siê przedmiotu)
	 Tutaj mo¿emy to zignorowaæ bo odbiorca ¿eby wzi±æ przedmiot
	 questowy MUSI byæ w que¶cie */
	if (!obj->inquest && !can_drop_obj(ch, obj))
	{
		send_to_char("Nie mo¿esz tego zrobiæ." NL, ch);
		return;
	}

	if (victim->carry_number + (get_obj_number(obj) / obj->count) > can_carry_n(victim))
	{
		act(PLAIN, "$N ma ju¿ pe³ne rêce.", ch, NULL, victim, TO_CHAR);
		return;
	}

	SWBounty *bounty = is_bounty_completed(obj, victim);
	/* Trog: jesli przedmiot jest za ciezki to nie victim nie moze go uniesc,
	 * ale jesli to przedmiot bounty, to robimy wyjatek */
	if (victim->carry_weight + (get_obj_weight(obj) / obj->count) > can_carry_w(victim) && !bounty)
	{
		act(PLAIN, "$N nie uniesie takiego ciê¿aru.", ch, NULL, victim, TO_CHAR);
		return;
	}

	if (!can_see_obj(victim, obj))
	{
		act(PLAIN, "$N nie widzi $p$1.", ch, obj, victim, TO_CHAR);
		return;
	}

	if (IS_OBJ_STAT( obj, ITEM_PROTOTYPE ) && !can_take_proto(victim))
	{
		act(PLAIN, "Nie mo¿esz daæ $p$1 $N$2!", ch, obj, victim, TO_CHAR);
		return;
	}

	separate_obj(obj);
	obj_from_char(obj);
	act(PLAIN, "$n daje $N$2 $p$3.", ch, obj, victim, TO_NOTVICT);
	act(PLAIN, "$n daje ci $p$3.", ch, obj, victim, TO_VICT);
	act(PLAIN, "Dajesz $N$2 $p$3.", ch, obj, victim, TO_CHAR);
	obj = obj_to_char(obj, victim);

	mprog_give_trigger(victim, ch, obj);

	/*Thanos: questy*/
	quest_trigger(ch, EVENT_GIVE_OBJ, obj, victim);

	/* Trog: bounty */
	if (bounty)
	{
		finalize_bounty(bounty, ch, victim, obj);
		obj_from_char(obj);
		extract_obj(obj);
	}

	if (IS_SET( sysdata.save_flags, SV_GIVE ) && !char_died(ch))
		save_char_obj(ch);
	if (IS_SET( sysdata.save_flags, SV_RECEIVE ) && !char_died(victim))
		save_char_obj(victim);
	return;
}

/*
 * Damage an object.						-Thoric
 * Affect player's AC if necessary.
 * Make object into scraps if necessary.
 * Send message about damaged object.
 */
obj_ret damage_obj(OBJ_DATA *obj)
{
	CHAR_DATA *ch;
	obj_ret objcode;

	ch = obj->carried_by;
	objcode = rNONE;

	separate_obj(obj);
	if (ch)
		act(COL_OBJECT, obj->gender == GENDER_NEUTRAL ? "$p zostaje uszkodzone"
				: obj->gender == GENDER_MALE ? "$p zostaje uszkodzony"
						: obj->gender == GENDER_FEMALE ? "$p zostaje uszkodzona" : "$p zostaj± uszkodzone", ch, obj,
				NULL, TO_CHAR);
	else if (obj->in_room && (ch = obj->in_room->first_person) != NULL)
	{
		act(COL_OBJECT, obj->gender == GENDER_NEUTRAL ? "$p zostaje uszkodzone"
				: obj->gender == GENDER_MALE ? "$p zostaje uszkodzony"
						: obj->gender == GENDER_FEMALE ? "$p zostaje uszkodzona" : "$p zostaj± uszkodzone", ch, obj,
				NULL, TO_ROOM);
		act(COL_OBJECT, obj->gender == GENDER_NEUTRAL ? "$p zostaje uszkodzone"
				: obj->gender == GENDER_MALE ? "$p zostaje uszkodzony"
						: obj->gender == GENDER_FEMALE ? "$p zostaje uszkodzona" : "$p zostaj± uszkodzone", ch, obj,
				NULL, TO_CHAR);
		ch = NULL;
	}

	oprog_damage_trigger(ch, obj);
	if (obj_extracted(obj))
		return global_objcode;

	switch (obj->item_type)
	{
	default:
		make_scraps(obj);
		objcode = rOBJ_SCRAPPED;
		break;
	case ITEM_CONTAINER:
		if (--obj->value[3] <= 0)
		{
			make_scraps(obj);
			objcode = rOBJ_SCRAPPED;
		}
		break;
	case ITEM_ARMOR:
		if (ch && obj->value[0] >= 1)
			ch->armor += apply_ac(obj, obj->wear_loc);
		if (--obj->value[0] <= 0)
		{
			make_scraps(obj);
			objcode = rOBJ_SCRAPPED;
		}
		else if (ch && obj->value[0] >= 1)
			ch->armor -= apply_ac(obj, obj->wear_loc);
		break;
	case ITEM_WEAPON:
		if (--obj->value[0] <= 0)
		{
			make_scraps(obj);
			objcode = rOBJ_SCRAPPED;
		}
		break;
	}
	return objcode;
}

/*
 * Remove an object.
 */
bool remove_obj(CHAR_DATA *ch, int iWear, bool fReplace)
{
	OBJ_DATA *obj, *tmpobj;

	if ((obj = get_eq_char(ch, iWear)) == NULL)
		return true;

	if (!fReplace && ch->carry_number + get_obj_number(obj) > can_carry_n(ch))
	{
		act(PLAIN, "$d: Nie mo¿esz nie¶æ tylu przedmiotów.", ch, NULL, obj->name, TO_CHAR);
		return false;
	}

	if (!fReplace)
		return false;

	if (IS_OBJ_STAT(obj, ITEM_NOREMOVE))
	{
		act(PLAIN, "Nie mo¿esz zdj±æ $p$1.", ch, obj, NULL, TO_CHAR);
		return false;
	}

	if (obj == get_eq_char(ch, WEAR_WIELD) && (tmpobj = get_eq_char(ch, WEAR_DUAL_WIELD)) != NULL)
		tmpobj->wear_loc = WEAR_WIELD;

	unequip_char(ch, obj);

	act(PLAIN, "$n zdejmuje $p$3.", ch, obj, NULL, TO_ROOM);
	act(PLAIN, "Zdejmujesz $p$3.", ch, obj, NULL, TO_CHAR);
	oprog_remove_trigger(ch, obj);
	return true;
}

/*
 * See if char could be capable of dual-wielding		-Thoric
 */
bool could_dual(CHAR_DATA *ch)
{
	if (IS_NPC(ch))
		return true;
	if (ch->pcdata->learned[gsn_dual_wield])
		return true;

	return false;
}

/*
 * See if char can dual wield at this time			-Thoric
 */
bool can_dual(CHAR_DATA *ch)
{
	OBJ_DATA* tmpobj;

	if (!could_dual(ch))
		return false;

	if (get_eq_char(ch, WEAR_DUAL_WIELD))
	{
		send_to_char("Ju¿ dzier¿ysz dwie broni!" NL, ch);
		return false;
	}
	if (get_eq_char(ch, WEAR_HOLD))
	{
		send_to_char("Nie mo¿esz dzier¿yæ dwóch broni nosz±c co¶ w rêku!" NL, ch);
		return false;
	}

	if ((tmpobj = get_eq_char(ch, WEAR_WIELD)) != NULL && IS_SET( tmpobj->wear_flags, ITEM_DUAL_WIELD ))
	{
		send_to_char("Masz zajête obie rêce!" NL, ch);
		return false;
	}
	return true;
}

/*
 * See if char can wield twohanded weapons at this time			-Ratm
 */
bool can_twohanded(CHAR_DATA *ch)
{
	if (IS_NPC(ch))
		return true;
	if (ch->pcdata->learned[gsn_twohanded_wield])
		return true;

	return false;
}

/*
 * Check to see if there is room to wear another object on this location
 * (Layered clothing support)
 */
bool can_layer(CHAR_DATA *ch, OBJ_DATA *obj, int wear_loc)
{
	OBJ_DATA *otmp;
	int bitlayers = 0;
	int objlayers = obj->pIndexData->layers;

	for (otmp = ch->first_carrying; otmp; otmp = otmp->next_content)
		if (otmp->wear_loc == wear_loc)
		{
			if (!otmp->pIndexData->layers)
				return false;
			else
				bitlayers |= otmp->pIndexData->layers;
		}
	if ((bitlayers && !objlayers) || bitlayers > objlayers)
		return false;
	if (!bitlayers || ((bitlayers & ~objlayers) == bitlayers))
		return true;
	return false;
}

/*! true, jessli obj jest przedmiotem, ktory uczyni Defela widzialnym */
bool isDefelVisible(OD *obj)
{
	return (!IS_SET(obj->extra_flags, ITEM_INVIS) && obj->wear_loc != WEAR_NONE && obj->wear_loc != WEAR_WIELD
			&& obj->wear_loc != WEAR_DUAL_WIELD && obj->wear_loc != WEAR_MISSILE_WIELD && obj->wear_loc != WEAR_SHIELD);
}

/*! true, jessli ch jest rasy Defel i wszystkie przedmioty, ktore ma na sobie, maja flage invis */
bool isDefelInvisible(CD *ch)
{
	if (IS_RACE( ch, "DEFEL" ))
	{
		for (OD *obj = ch->last_carrying; obj; obj = obj->prev_content)
		{
			if (isDefelVisible(obj))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 * Restructured a bit to allow for specifying body location	-Thoric
 */
void wear_obj(CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace, int wear_bit)
{
	char buf[MAX_STRING_LENGTH];
	OBJ_DATA *tmpobj;
	int bit, tmp;
	bool check_size;

	separate_obj(obj);

	if (wear_bit > -1)
	{
		bit = wear_bit;
		if (!CAN_WEAR(obj, 1 << bit))
		{
			if (fReplace)
			{
				switch (1 << bit)
				{
				case ITEM_HOLD:
					send_to_char("Nie mo¿esz tego trzymaæ." NL, ch);
					break;
				case ITEM_WIELD:
				case ITEM_DUAL_WIELD:
					send_to_char("Nie mo¿esz tego dzier¿yæ." NL, ch);
					break;
				default:
					sprintf(buf, "Nie mo¿esz tego za³o¿yæ.\n\r");
					send_to_char(buf, ch);
				}
			}
			return;
		}
	}
	else
	{
		for (bit = -1, tmp = 1; tmp < 31; tmp++)
		{
			if (CAN_WEAR(obj, 1 << tmp))
			{
				bit = tmp;
				break;
			}
		}
	}

	/* Thanos: requirements */
	if (!req_check(ch, obj))
		return;

	check_size = false;

	if (1 << bit == ITEM_WIELD || 1 << bit == ITEM_HOLD || obj->item_type == ITEM_LIGHT || 1 << bit == ITEM_WEAR_SHIELD
			|| 1 << bit == ITEM_DUAL_WIELD)
		check_size = false;
	else if (IS_RACE( ch, "DUINUOGWUIN" ))
		check_size = true;
	else if (!IS_NPC(ch))
	{
		if ((IS_RACE( ch, "TRANDOSHAN" ) || IS_RACE( ch, "VERPINE" ) || IS_RACE( ch, "HUMAN" )
				|| IS_RACE( ch, "ADARIAN" ) || IS_RACE( ch, "RODIAN" ) || IS_RACE( ch, "TWI'LEK" )
				|| IS_RACE( ch, "YUUZHAN VONG" )) && !IS_OBJ_STAT(obj, ITEM_HUMAN_SIZE))
			check_size = true;
		else if (IS_RACE( ch, "HUTT" ) && !IS_OBJ_STAT(obj, ITEM_HUTT_SIZE))
			check_size = true;
		else if ((IS_RACE( ch, "GAMORREAN" ) || IS_RACE( ch, "MON CALAMARI" ) || IS_RACE( ch, "QUARREN" )
				|| IS_RACE( ch, "WOOKIEE" )) && !IS_OBJ_STAT(obj, ITEM_LARGE_SIZE))
			check_size = true;
		else if ((IS_RACE( ch, "CHADRA-FAN" ) || IS_RACE( ch, "EWOK" ) || IS_RACE( ch, "NOGHRI" )
				|| IS_RACE( ch, "JAWA" )) && !IS_OBJ_STAT(obj, ITEM_SMALL_SIZE))
			check_size = true;
	}

	/*
	 this seems redundant but it enables both multiple sized objects to be
	 used as well as objects with no size flags at all
	 */

	if (check_size)
	{
		if (IS_RACE( ch, "DUINUOGWUIN" ))
		{
			act(COL_FORCE,
					"Prawdopodobnie niemo¿liwo¶ci± bêdzie znale¼æ co¶ co pasowa³oby na twoje\n\rnienaturalne cia³o.",
					ch, NULL, NULL, TO_CHAR);
			act(PLAIN, "$n próbuje u¿yæ $p$1, ale jest zbyt du¿$y.", ch, obj, NULL, TO_ROOM);
			return;
		}

		if (IS_OBJ_STAT(obj, ITEM_HUTT_SIZE))
		{
			act(COL_FORCE, "Ten przedmiot jest dla ciebie za du¿y.", ch, NULL, NULL, TO_CHAR);
			act(PLAIN, "$n próbuje u¿yæ $p$1, ale jest za ma³$y.", ch, obj, NULL, TO_ROOM);
			return;
		}

		if (IS_OBJ_STAT(obj, ITEM_LARGE_SIZE) || IS_OBJ_STAT(obj, ITEM_HUMAN_SIZE))
		{
			act(COL_FORCE, "To nie twój rozmiar.", ch, NULL, NULL, TO_CHAR);
			act(PLAIN, "$n próbuje u¿yæ $p$1, ale jako¶ nie mo¿e.", ch, obj, NULL, TO_ROOM);
			return;
		}

		if (IS_OBJ_STAT(obj, ITEM_SMALL_SIZE))
		{
			act(COL_FORCE, "Ten przedmiot jest dla ciebie za ma³y.", ch, NULL, NULL, TO_CHAR);
			act(PLAIN, "$n próbuje u¿yæ $p$1, ale jest zbyt du¿$y.", ch, obj, NULL, TO_ROOM);
			return;
		}
	}

	/* currently cannot have a light in non-light position */
	if (obj->item_type == ITEM_LIGHT)
	{
		if (!remove_obj(ch, WEAR_LIGHT, fReplace))
			return;
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zaczepiasz sobie $p$3 jako ¶wiat³o.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zaczepia sobie $p$3 jako ¶wiat³o.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_LIGHT);
		oprog_wear_trigger(ch, obj);
		return;
	}

	if (bit == -1)
	{
		if (fReplace)
			send_to_char("Nie mo¿esz tego za³o¿yæ, dzier¿yæ ani trzymaæ." NL, ch);
		return;
	}

	switch (1 << bit)
	{
	default:
		bug( "uknown/unused item_wear bit %d (ch=%s)", bit, ch->name );
		if (fReplace)
			send_to_char("Nie mo¿esz tego za³o¿yæ, dzier¿yæ ani trzymaæ." NL, ch);
		return;

	case ITEM_WEAR_FINGER:
		if (get_eq_char(ch, WEAR_FINGER_L) && get_eq_char(ch, WEAR_FINGER_R)
				&& !remove_obj(ch, WEAR_FINGER_L, fReplace) && !remove_obj(ch, WEAR_FINGER_R, fReplace))
			return;

		if (!get_eq_char(ch, WEAR_FINGER_L))
		{
			if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
			{
				if (!obj->action_desc || obj->action_desc[0] == '\0')
					act(PLAIN, "Wsuwasz $p$3 na palec lewej rêki.", ch, obj, NULL, TO_CHAR);
				else
					actiondesc(ch, obj, NULL);

				act(PLAIN, "$n wsuwa $p$3 na palec lewej rêki.", ch, obj, NULL, TO_ROOM);
			}
			equip_char(ch, obj, WEAR_FINGER_L);
			oprog_wear_trigger(ch, obj);
			return;
		}

		if (!get_eq_char(ch, WEAR_FINGER_R))
		{
			if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
			{
				if (!obj->action_desc || obj->action_desc[0] == '\0')
					act(PLAIN, "Wsuwasz $p$3 na palec prawej rêki.", ch, obj, NULL, TO_CHAR);
				else
					actiondesc(ch, obj, NULL);

				act(PLAIN, "$n wsuwa $p$3 na palec prawej rêki.", ch, obj, NULL, TO_ROOM);
			}
			equip_char(ch, obj, WEAR_FINGER_R);
			oprog_wear_trigger(ch, obj);
			return;
		}

		bug( "no free finger." );
		send_to_char("Masz ju¿ co¶ na obu palcach." NL, ch);
		return;

	case ITEM_WEAR_NECK:
		if (get_eq_char(ch, WEAR_NECK_1) != NULL && get_eq_char(ch, WEAR_NECK_2) != NULL && !remove_obj(ch,
				WEAR_NECK_1, fReplace) && !remove_obj(ch, WEAR_NECK_2, fReplace))
			return;

		if (!get_eq_char(ch, WEAR_NECK_1))
		{
			if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
			{
				if (!obj->action_desc || obj->action_desc[0] == '\0')
					act(PLAIN, "Zak³adasz $p$3 naoko³o szyi.", ch, obj, NULL, TO_CHAR);
				else
					actiondesc(ch, obj, NULL);

				act(PLAIN, "$n zak³ada $p$3 naoko³o szyi.", ch, obj, NULL, TO_ROOM);
			}
			equip_char(ch, obj, WEAR_NECK_1);
			oprog_wear_trigger(ch, obj);
			return;
		}

		if (!get_eq_char(ch, WEAR_NECK_2))
		{
			if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
			{
				if (!obj->action_desc || obj->action_desc[0] == '\0')
					act(PLAIN, "Zak³adasz $p$3 naoko³o szyi.", ch, obj, NULL, TO_CHAR);
				else
					actiondesc(ch, obj, NULL);

				act(PLAIN, "$n zak³ada $p$3 naoko³o szyi.", ch, obj, NULL, TO_ROOM);
			}
			equip_char(ch, obj, WEAR_NECK_2);
			oprog_wear_trigger(ch, obj);
			return;
		}

		bug( "no free neck. (ch=%s)", ch->name );
		send_to_char("Masz na szyi ju¿ dwa przedmioty." NL, ch);
		return;

	case ITEM_WEAR_BODY:
		/*
		 if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
		 return;
		 */
		if (!can_layer(ch, obj, WEAR_BODY))
		{
			send_to_char("Nie mo¿esz za³o¿yæ tego na to, co ju¿ nosisz." NL, ch);
			return;
		}
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zak³adasz $p$3 na cia³o.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zak³ada $p$3 na cia³o.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_BODY);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_HEAD:
		if (IS_RACE( ch, "VERPINE" ) || IS_RACE( ch, "TWI'LEK" ))
		{
			send_to_char("Nie mo¿esz nosiæ nic na g³owie." NL, ch);
			return;
		}
		if (!remove_obj(ch, WEAR_HEAD, fReplace))
			return;
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Nak³adasz sobie $p$3 na g³owê.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n nak³ada sobie $p$3 na g³owê.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_HEAD);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_EYES:
		if (!remove_obj(ch, WEAR_EYES, fReplace))
			return;
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Wsuwasz sobie $p$3 na oczy.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n wsuwa sobie $p$3 na oczy.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_EYES);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_EARS:
		if (IS_RACE( ch, "VERPINE" ))
		{
			send_to_char("Nie mo¿esz za³o¿yæ tego na uszy bo... nie masz uszu. :(" NL, ch);
			return;
		}
		if (!remove_obj(ch, WEAR_EARS, fReplace))
			return;
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Wpinasz $p$3 w uszy.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n wpina $p$3 w uszy.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_EARS);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_LEGS:
		/*
		 if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
		 return;
		 */
		if (IS_RACE( ch, "HUTT" ))
		{
			send_to_char("Wiesz... Huttowie nie maj± nóg, wiêc..." NL, ch);
			return;
		}
		if (!can_layer(ch, obj, WEAR_LEGS))
		{
			send_to_char("Nie za³o¿ysz tego na co¶ co ju¿ nosisz." NL, ch);
			return;
		}
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Wskakujesz w $p.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(COL_ACTION, "$n wskakuje w $p$3.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_LEGS);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_FEET:
		/*
		 if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
		 return;
		 */
		if (IS_RACE( ch, "HUTT" ))
		{
			send_to_char("Wiesz... Huttowie nie maj± stóp..." NL, ch);
			return;
		}
		if (!can_layer(ch, obj, WEAR_FEET))
		{
			send_to_char("Nie za³o¿ysz tego na co¶ co ju¿ nosisz." NL, ch);
			return;
		}
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zak³adasz $p$3 na stopy.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zak³ada $p$3 na stopy.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_FEET);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_HANDS:
		/*
		 if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
		 return;
		 */
		if (!can_layer(ch, obj, WEAR_HANDS))
		{
			send_to_char("Nie mo¿esz za³o¿yæ tego na co¶ co ju¿ nosisz." NL, ch);
			return;
		}
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zak³adasz $p$3 na rêce.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zak³ada $p$3 na rêce.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_HANDS);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_ARMS:
		/*
		 if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
		 return;
		 */
		if (!can_layer(ch, obj, WEAR_ARMS))
		{
			send_to_char("Nie za³o¿ysz tego na co¶ co ju¿ nosisz." NL, ch);
			return;
		}
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zak³adasz $p$3 na ramiona.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zak³ada $p$3 na ramiona.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_ARMS);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_ABOUT:
		/*
		 if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
		 return;
		 */
		if (!can_layer(ch, obj, WEAR_ABOUT))
		{
			send_to_char("Nie za³o¿ysz tego na co¶ co ju¿ nosisz." NL, ch);
			return;
		}
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zarzucasz $p$3 naoko³o cia³a.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zarzuca $p$3 naoko³o cia³a.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_ABOUT);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_WAIST:
		/*
		 if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
		 return;
		 */
		if (!can_layer(ch, obj, WEAR_WAIST))
		{
			send_to_char("Nie za³o¿ysz tego na co¶ co ju¿ nosisz." NL, ch);
			return;
		}
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Owijasz pas $p$4.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n owija pas $p$4.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_WAIST);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WEAR_WRIST:
		if (get_eq_char(ch, WEAR_WRIST_L) && get_eq_char(ch, WEAR_WRIST_R) && !remove_obj(ch, WEAR_WRIST_L, fReplace)
				&& !remove_obj(ch, WEAR_WRIST_R, fReplace))
			return;

		if (!get_eq_char(ch, WEAR_WRIST_L))
		{
			if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
			{
				if (!obj->action_desc || obj->action_desc[0] == '\0')
					act(PLAIN, "Zak³adasz $p$3 na lewy nadgarstek.", ch, obj, NULL, TO_CHAR);
				else
					actiondesc(ch, obj, NULL);

				act(PLAIN, "$n zak³ada $p$3 na lewy nadgarstek.", ch, obj, NULL, TO_ROOM);
			}
			equip_char(ch, obj, WEAR_WRIST_L);
			oprog_wear_trigger(ch, obj);
			return;
		}

		if (!get_eq_char(ch, WEAR_WRIST_R))
		{
			if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
			{
				if (!obj->action_desc || obj->action_desc[0] == '\0')
					act(PLAIN, "Zak³adasz $p$3 na prawy nadgarstek.", ch, obj, NULL, TO_CHAR);
				else
					actiondesc(ch, obj, NULL);

				act(PLAIN, "$n zak³ada $p$3 na prawy nadgarstek.", ch, obj, NULL, TO_ROOM);
			}
			equip_char(ch, obj, WEAR_WRIST_R);
			oprog_wear_trigger(ch, obj);
			return;
		}

		bug( "no free wrist. (ch=%s)", ch->name );
		send_to_char("Nosisz ju¿ co¶ na obu nadgarstkach." NL, ch);
		return;

	case ITEM_WEAR_SHIELD:
		if (!remove_obj(ch, WEAR_SHIELD, fReplace))
			return;
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zak³adasz $p$3 jako tarczê energetyczn±.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zak³ada $p$3 jako tarczê energetyczn±.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_SHIELD);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_WIELD:
		if ((tmpobj = get_eq_char(ch, WEAR_WIELD)) != NULL && !could_dual(ch))
		{
			send_to_char("Ju¿ dzier¿ysz broñ." NL, ch);
			return;
		}

		if (tmpobj)
		{
			if (can_dual(ch))
			{
				if (get_obj_weight(obj) + get_obj_weight(tmpobj) > str_app[get_curr_str(ch)].wield)
				{
					send_to_char("Nie uniesiesz takiego ciê¿aru." NL, ch);
					return;
				}
				if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
				{
					if (!obj->action_desc || obj->action_desc[0] == '\0')
						act(PLAIN, "Zak³adasz $p$3 jako drug± broñ.", ch, obj, NULL, TO_CHAR);
					else
						actiondesc(ch, obj, NULL);

					act(PLAIN, "$n zak³ada $p$3 jako drug± broñ.", ch, obj, NULL, TO_ROOM);
				}
				equip_char(ch, obj, WEAR_DUAL_WIELD);
				oprog_wear_trigger(ch, obj);
			}
			return;
		}

		if (get_obj_weight(obj) > str_app[get_curr_str(ch)].wield)
		{
			send_to_char("Nie uniesiesz takiego ciê¿aru." NL, ch);
			return;
		}

		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zaczynasz u¿ywaæ $p$1 jako broni.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zaczyna u¿ywaæ $p$1 jako broni.", ch, obj, NULL, TO_ROOM);
		}
		equip_char(ch, obj, WEAR_WIELD);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_DUAL_WIELD:
		if (!can_twohanded(ch))
		{
			send_to_char("Nie umiesz u¿ywaæ broni dwurêcznych." NL, ch);
			return;
		}

		if ((tmpobj = get_eq_char(ch, WEAR_WIELD)) != NULL)
		{
			send_to_char("Ju¿ dzier¿ysz broñ." NL, ch);
			return;
		}

		if (get_eq_char(ch, WEAR_HOLD))
		{
			send_to_char("Nie mo¿esz dzier¿yæ broni dwurêcznej nosz±c co¶ w rêku!" NL, ch);
			return;
		}

		if (get_obj_weight(obj) > str_app[get_curr_str(ch)].wield)
		{
			send_to_char("Nie uniesiesz takiego ciê¿aru." NL, ch);
			return;
		}
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act(PLAIN, "Zak³adasz $p$3 jako broñ dwurêczn±.", ch, obj, NULL, TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act(PLAIN, "$n zaczyna u¿ywaæ $p$1 jako broni.", ch, obj, NULL, TO_ROOM);
		}

		equip_char(ch, obj, WEAR_WIELD);
		oprog_wear_trigger(ch, obj);
		return;

	case ITEM_HOLD:
		if (get_eq_char(ch, WEAR_DUAL_WIELD))
		{
			send_to_char("Nie mo¿esz nosiæ czego¶ w rêce kiedy dzier¿ysz dwie broni!" NL, ch);
			return;
		}
		if ((tmpobj = get_eq_char(ch, WEAR_WIELD)) != NULL && IS_SET( tmpobj->wear_flags, ITEM_DUAL_WIELD))
		{
			send_to_char("Nie mo¿esz nosiæ czego¶ w rêce kiedy dzier¿ysz broñ dwurêczn±!" NL, ch);
			return;
		}
		if (!remove_obj(ch, WEAR_HOLD, fReplace))
			return;
		if (obj->item_type == ITEM_DEVICE || obj->item_type == ITEM_GRENADE || obj->item_type == ITEM_FOOD
				|| obj->item_type == ITEM_PILL || obj->item_type == ITEM_POTION || obj->item_type == ITEM_DRINK_CON
				|| obj->item_type == ITEM_SALVE || obj->item_type == ITEM_KEY || !oprog_use_trigger(ch, obj, NULL,
				NULL, NULL))
		{
			act(PLAIN, "$n bierze w rêce $p$3.", ch, obj, NULL, TO_ROOM);
			act(PLAIN, "Bierzesz w rêce $p$3.", ch, obj, NULL, TO_CHAR);
		}
		equip_char(ch, obj, WEAR_HOLD);
		oprog_wear_trigger(ch, obj);
		return;
	}
}

DEF_DO_FUN( wear )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int wear_bit;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	if (((!str_cmp(arg2, "on") || !str_cmp(arg2, "upon") || !str_cmp(arg2, "around") || !str_cmp(arg2, "na")
			|| !str_cmp(arg2, "w") || !str_cmp(arg2, "wokó³")) && argument[0] != '\0'))
		argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Za³o¿yæ, ubraæ co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if (!str_cmp(arg1, "all"))
	{
		OBJ_DATA *obj_next;

		for (obj = ch->first_carrying; obj; obj = obj_next)
		{
			obj_next = obj->next_content;
			if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj))
				wear_obj(ch, obj, false, -1);
		}
		return;
	}
	else
	{
		if ((obj = get_obj_carry(ch, arg1)) == NULL)
		{
			send_to_char("Nie masz takiego przedmiotu." NL, ch);
			return;
		}
		if (arg2[0] != '\0')
			wear_bit = flag_value(wear_flags_list, arg2);

		else
			wear_bit = -1;
		wear_obj(ch, obj, true, wear_bit);
	}

	return;
}

DEF_DO_FUN( remove )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj, *obj_next;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Zdj±æ co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if (!str_cmp(arg, "all")) /* SB Remove all */
	{
		for (obj = ch->first_carrying; obj != NULL; obj = obj_next)
		{
			obj_next = obj->next_content;
			if (obj->wear_loc != WEAR_NONE && can_see_obj(ch, obj))
				remove_obj(ch, obj->wear_loc, true);
		}
		return;
	}

	if ((obj = get_obj_wear(ch, arg)) == NULL)
	{
		send_to_char("Nie nosisz tego." NL, ch);
		return;
	}
	if ((obj_next = get_eq_char(ch, obj->wear_loc)) != obj)
	{
		act(PLAIN, "Najpierw zdejmij $p$3.", ch, obj_next, NULL, TO_CHAR);
		return;
	}

	remove_obj(ch, obj->wear_loc, true);
	return;
}

DEF_DO_FUN( bury )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	bool shovel;
	int move;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Co sobie ¿yczysz zakopaæ?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	shovel = false;
	for (obj = ch->first_carrying; obj; obj = obj->next_content)
		if (obj->item_type == ITEM_SHOVEL)
		{
			shovel = true;
			break;
		}

	obj = get_obj_list_rev(ch, arg, ch->in_room->last_content);
	if (!obj)
	{
		send_to_char("Nie ma tu takiego przedmiotu." NL, ch);
		return;
	}

	separate_obj(obj);
	if (!CAN_WEAR(obj, ITEM_TAKE))
	{
		act(PLAIN, "Nie mo¿esz zakopaæ $p$1.", ch, obj, 0, TO_CHAR);
		return;
	}

	switch (ch->in_room->sector_type)
	{
	case SECT_CITY:
	case SECT_INSIDE:
		send_to_char("Pod³oga tu jest zbyt twarda. Nie przekopiesz jej." NL, ch);
		return;
	case SECT_WATER_SWIM:
	case SECT_WATER_NOSWIM:
	case SECT_UNDERWATER:
		send_to_char("Nie mo¿esz zakopaæ czego¶ pod wod±!" NL, ch);
		return;
	case SECT_AIR:
		send_to_char("Gdzie?  W powietrzu???" NL, ch);
		return;
	}

	if (obj->weight > (UMAX(5, (can_carry_w(ch) / 10))) && !shovel)
	{
		send_to_char("¯eby zakopaæ co¶ tak wielkiego, potrzebujesz ³opaty." NL, ch);
		return;
	}

	move = (obj->weight * 50 * (shovel ? 1 : 5)) / UMAX(1, can_carry_w(ch));
	move = URANGE( 2, move, 1000 );
	if (move > ch->move)
	{
		send_to_char("Nie masz w sobie tyle energii by to zakopaæ." NL, ch);
		return;
	}
	ch->move -= move;

	act(PLAIN, "Ukradkiem zakopujesz $p...", ch, obj, NULL, TO_CHAR);
	act(PLAIN, "$n ukradkiem zakopuje $p...", ch, obj, NULL, TO_ROOM);
	SET_BIT( obj->extra_flags, ITEM_BURRIED );
	WAIT_STATE( ch, URANGE( 10, move / 2, 100 ) );
	return;
}

DEF_DO_FUN( sacrifice )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	one_argument(argument, arg);

	/*    if ( arg[0] == '\0' || !str_cmp( arg, ch->name ) )
	 {
	 act( COL_ACTION, "$n offers $mself to $s deity, who graciously declines.",
	 ch, NULL, NULL, TO_ROOM );
	 send_to_char( "Your deity appreciates your offer and may accept it later.", ch );
	 return;
	 }*/

	if (ms_find_obj(ch))
		return;

	obj = get_obj_list_rev(ch, arg, ch->in_room->last_content);
	if (!obj)
	{
		send_to_char("You can't find it." NL, ch);
		return;
	}

	separate_obj(obj);
	if (!CAN_WEAR(obj, ITEM_TAKE))
	{
		act(PLAIN, "$p is not an acceptable sacrifice.", ch, obj, 0, TO_CHAR);
		return;
	}

	oprog_sac_trigger(ch, obj);
	if (obj_extracted(obj))
		return;
	if (cur_obj == obj->serial)
		global_objcode = rOBJ_SACCED;
	extract_obj(obj);
	return;
}

/*
 * Save items in a clan storage room			-Scryn & Thoric
 * Trog: dzieki chlopaki - przyda mi sie juz niedlugo :)
 */
/*
 void save_clan_storeroom( CHAR_DATA *ch, CLAN_DATA *clan )
 {
 FILE *	fp;
 char 	filename	[256];
 int 	templvl;
 OBJ_DATA *	contents;

 IF_BUG( clan==NULL, "" )
 return;

 IF_BUG( ch==NULL, "" )
 return;

 RESERVE_CLOSE;
 sprintf( filename, "%s%s.vault", CLAN_DIR, clan->filename );
 if ( ( fp = fopen( filename, "w" ) ) == NULL )
 {
 bug( "fopen" );
 perror( filename );
 }
 else
 {
 templvl = ch->top_level;
 ch->top_level = LEVEL_HERO;		// make sure EQ doesn't get lost
 contents = ch->in_room->last_content;
 if (contents)
 fwrite_obj(ch, contents, fp, 0, OS_CARRY );
 fprintf( fp, "#END\n" );
 ch->top_level = templvl;
 fclose( fp );
 }
 RESERVE_OPEN;
 return;
 }
 */

/* Make objects in rooms that are nofloor fall - Scryn 1/23/96 */

void obj_fall(OBJ_DATA *obj, bool through)
{
	EXIT_DATA *pexit;
	ROOM_INDEX_DATA *to_room;
	static int fall_count = 0;
	static bool is_falling = 0; /* Stop loops from the call to obj_to_room()  -- Altrag */

	if (!obj->in_room || is_falling)
		return;

	if (fall_count > 30)
	{
		bug( "object falling in loop more than 30 times" );
		extract_obj(obj);
		fall_count = 0;
		return;
	}

	if (IS_SET( obj->in_room->room_flags, ROOM_NOFLOOR ) && CAN_GO( obj, DIR_DOWN ) && !IS_OBJ_STAT( obj, ITEM_FORCE ))
	{

		pexit = get_exit(obj->in_room, DIR_DOWN);
		to_room = pexit->to_room;

		if (through)
			fall_count++;
		else
			fall_count = 0;

		if (obj->in_room == to_room)
		{
			bug( "Object falling into same room, room %d", to_room->vnum);
			extract_obj(obj);
			return;
		}

		if (obj->in_room->first_person)
		{
			act(PLAIN, "$p spada gdzie¶ w dó³...", obj->in_room->first_person, obj, NULL, TO_ROOM);
			act(PLAIN, "$p spada gdzie¶ w dó³...", obj->in_room->first_person, obj, NULL, TO_CHAR);
		}
		obj_from_room(obj);
		is_falling = true;
		obj = obj_to_room(obj, to_room);
		is_falling = false;

		if (obj->in_room->first_person)
		{
			act(PLAIN, "$p spada sk±d¶ z góry...", obj->in_room->first_person, obj, NULL, TO_ROOM);
			act(PLAIN, "$p spada sk±d¶ z góry...", obj->in_room->first_person, obj, NULL, TO_CHAR);
		}

		if (!IS_SET( obj->in_room->room_flags, ROOM_NOFLOOR ) && through)
		{
			/*		int dam = (int)9.81*sqrt(fall_count*2/9.81)*obj->weight/2;
			 */int dam = fall_count * obj->weight / 2;
			/* Damage players */
			if (obj->in_room->first_person && number_percent() > 15)
			{
				CHAR_DATA *rch;
				CHAR_DATA *vch = NULL;
				int chcnt = 0;

				for (rch = obj->in_room->first_person; rch; rch = rch->next_in_room, chcnt++)
					if (number_range(0, chcnt) == 0)
						vch = rch;
				act(FB_WHITE, "$p spada na $n$3!", vch, obj, NULL, TO_ROOM);
				act(FB_WHITE, "$p spada na ciebie!", vch, obj, NULL, TO_CHAR);
				damage(vch, vch, dam * vch->top_level, TYPE_UNDEFINED);
			}
			/* Damage objects */
			switch (obj->item_type)
			{
			case ITEM_WEAPON:
			case ITEM_ARMOR:
				if ((obj->value[0] - dam) <= 0)
				{
					if (obj->in_room->first_person)
					{
						act(PLAIN, "$p niszczy siê przez upadek!", obj->in_room->first_person, obj, NULL, TO_ROOM);
						act(PLAIN, "$p niszczy siê przez upadek!", obj->in_room->first_person, obj, NULL, TO_CHAR);
					}
					make_scraps(obj);
				}
				else
					obj->value[0] -= dam;
				break;
			default:
				if ((dam * 15) > get_obj_resistance(obj))
				{
					if (obj->in_room->first_person)
					{
						act(PLAIN, "$p niszczy siê przez upadek!", obj->in_room->first_person, obj, NULL, TO_ROOM);
						act(PLAIN, "$p niszczy siê przez upadek!", obj->in_room->first_person, obj, NULL, TO_CHAR);
					}
					make_scraps(obj);
				}
				break;
			}
		}
		obj_fall(obj, true);
	}
	return;
}

