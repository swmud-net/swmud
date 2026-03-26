/***************************************************************************
 * Star Wars Reality 1.0						           *
 * copyright (c) 1997, 1998 by Sean Cooper                                  *
 * Starwars and Starwars Names copyright(c) Lucasfilm Ltd.                  *
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
 * (c) 2001, 2002            M       U        D                Ver 1.1		*
 * ------------------------------------------------------------------------	*
 *						Auction routines									*
 ****************************************************************************/

#include <time.h>
#include <iterator>
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )				\
				if ( !str_cmp( word, literal ) )	\
				{									\
				    field  = value;					\
				    fMatch = true;					\
				    break;							\
				}
#include "auction.h"
//#include "mud.h"

/*extern time_t auction_pulse;
 void sort_auction_bets(AUCTION_DATA 	*pAuction);
 void process_auctions();
 void send_auction_notes( AUCTION_DATA * pAuction, auction_ends typ );
 char * advitoa( int val, bool coma);*/

const SWString prepare_time(time_t end_time, bool short_form, bool minus)
{
	time_t time_diff = difftime(end_time, current_time);
	if (time_diff < 0)
	{
		return SWString("KONIEC");
	}

	char buf[MSL] =
	{ 0 };
	char buf1[MSL] =
	{ 0 };
	int day, hour, min;
	int sek;

	min = time_diff / 60;
	sek = time_diff - min * 60;	//ile sekund up�yne�o
	hour = min / 60;		//ile godziny
	min = min - hour * 60;
	if (hour > 24)
	{
		day = hour / 24;
		hour = hour - day * 24;
		if (!short_form)
			sprintf(buf1, "%d d%s, %d godzin%s", day, NUMBER_SUFF(day, "zie�", "ni", "ni"), hour, NUMBER_SUFF(hour, "�", "y", ""));
		else
			sprintf(buf1, "%d d, %d h", day, hour);
	}
	else if (hour > 0)
	{
		if (!short_form)
			sprintf(buf1, "%d godzin%s, %d minut%s", hour, NUMBER_SUFF(hour, "�", "y", ""), min, NUMBER_SUFF(min, "�", "y", ""));
		else
			sprintf(buf1, "%d h, %d m", hour, min);
	}
	else if (min > 0)
	{
		if (!short_form)
			sprintf(buf1, "%d minut%s, %d sekund%s", min, NUMBER_SUFF(min, "�", "y", ""), sek, NUMBER_SUFF(sek, "a", "y", ""));
		else
			sprintf(buf1, "%d m, %d s", min, sek);
	}
	else
	{
		if (!short_form)
			sprintf(buf1, "%d sekund%s", sek, NUMBER_SUFF(sek, "a", "y", ""));
		else
			sprintf(buf1, "%d s", sek);
	}
	if (minus)
		swsnprintf(buf, MSL, "+ %s", buf1);
	else
		strncpy(buf, buf1, MSL);
	return SWString(buf);
}

bool auction_creator(char *name, AUCTION_DATA *pAuction)
{
	return (!str_cmp(pAuction->seller_name, name));
}

bool is_auction_member(char *name, AUCTION_DATA *pAuction)
{
	for (auto* pBets : pAuction->bets)
		if (!str_cmp(pBets->name, name))
			return true;
	return false;
}

BETS_PROGRES* get_auction_member(char *name, AUCTION_DATA *pAuction)
{
	for (auto* pBets : pAuction->bets)
		if (!str_cmp(pBets->name, name))
			return pBets;
	return NULL;
}

AUCTION_DATA* get_auction_nr(CHAR_DATA *ch, int nr, STOCK_EXCHANGE_DATA *pStock)
{
	int index = 1;
	auction_type type;

	if (!pStock || pStock->auctions.empty())
		return NULL;
	type = get_auction_type(ch);
	if (nr < 0)
		return NULL;
	for (auto* pAuction : pStock->auctions)
	{
		if (pAuction->type != type)
			continue;
		if (index++ == nr)
			return pAuction;
	}
	return NULL;
}

bool auction_finished(AUCTION_DATA *pAuction)
{
	time_t time_diff = difftime(pAuction->end_time, current_time);
	return (time_diff <= 0) ? true : false;
}

bool auction_started(AUCTION_DATA *pAuction)
{
	time_t time_diff = difftime(current_time, pAuction->start_time);
	return (time_diff <= 0) ? false : true;
}

bool won_auction(char *name, AUCTION_DATA *pAuction)
{
	if (pAuction->bets.empty())
		return false;
	if (!str_cmp(name, pAuction->bets.front()->name))
		return true;
	return false;
}

bool auction_ship(AUCTION_DATA *pAuction)
{
	return (!pAuction->item && !pAuction->item_collected);
}

auction_type get_auction_type(CHAR_DATA *ch)
{
	switch (ch->pcdata->line_nr)
	{
	case STOCK_STATE_SHIPS:
		return AUCTION_SHIP;
	case STOCK_STATE_ITEMS:
		return AUCTION_ITEM;
	case STOCK_STATE_HOUSE:
		return AUCTION_HOUSE;
	case STOCK_STATE_PRODUCTS:
		return AUCTION_PRODUCT;
	case STOCK_STATE_MATERIALS:
		return AUCTION_MATERIAL;
	default:
		break;
	}
	return AUCTION_TYPES_MAX;
}

static void auction_showaffect(CHAR_DATA *ch, AFFECT_DATA *paf)
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
			sprintf(buf, FG_CYAN "    Wp�ywa na " PLAIN "%s" FG_CYAN " o " PLAIN "%d" FG_CYAN "." EOL, affect_loc_name_pl(paf->location),
					paf->modifier);
			break;
		case APPLY_AFFECT:
			sprintf(buf, FG_CYAN "    Wp�ywa na " PLAIN "%s" FG_CYAN " o " PLAIN, affect_loc_name_pl(paf->location));
			strcat(buf, flag_string(aff_flags_list, paf->modifier));
			strcat(buf, EOL);
			break;
		case APPLY_WEAPONSPELL:
		case APPLY_WEARSPELL:
		case APPLY_REMOVESPELL:
			sprintf(buf, FG_CYAN "Potrafi wykona� '" PLAIN "%s" FG_CYAN "'" EOL,
			IS_VALID_SN(paf->modifier) ? skill_table[paf->modifier]->name : "nie wiadomo co");
			break;
		case APPLY_RESISTANT:
		case APPLY_IMMUNE:
		case APPLY_SUSCEPTIBLE:
			sprintf(buf, FG_CYAN "    Wp�ywa na " PLAIN "%s" FG_CYAN " o " PLAIN, affect_loc_name_pl(paf->location));
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

void auction_item_gather(CHAR_DATA *ch, AUCTION_DATA *pAuction)
{
	/*!	Aukcja zako�czy�a si�. Dany gracz zg�osi� si� po odbi�r
	 *	wylicytowanego przedmiotu lub got�wki (je�li to jej tw�rca) albo
	 *	te� jest to licytant, kt�ry postawi� za ma�o i przyszed� odebra�
	 *	zdeponowan� z g�ry kwot�.
	 */
	/*! \todo Trzeba obowi�zkowo doda� modyfikacje zale�ne od typu sprzedawanego
	 * przedmiotu	*/
	BETS_PROGRES *pBets;

	if (auction_creator(ch->name, pAuction))
	{
		// jesli ktos licytowal i odebra� ju� przedmiot
		if (pAuction->item_collected || (!pAuction->item_collected && !pAuction->bets.empty()))
		{
			int pay, tax;
			float percent;
			if (pAuction->bet < 1000)
				percent = 20;
			else if (pAuction->bet < 10000)
				percent = 15;
			else if (pAuction->bet < 100000)
				percent = 10;
			else if (pAuction->bet < 1000000)
				percent = 5;
			else
				percent = 2;
			tax = (int) pAuction->bet * (percent / 100);
			pay = pAuction->bet - tax;
			pAuction->payment_collected = true;

			boost_economy(ch->in_room->area, tax);
			ch->gold += pay;
			ch_printf(ch, "Aukcjoner wyp�aci� Ci %d kredyt%s, pobieraj�c"
					" prowizj� w wysoko�ci %d kredyt%s.\n", pay, NUMBER_SUFF(pay, "k�", "ki", "ek"), tax,
					NUMBER_SUFF(tax, "k�", "ki", "ek"));
		}
		else
		{
			obj_to_char(pAuction->item, ch);
			ch_printf(ch, "Aukcjoner oddaje Ci niesprzedany towar.\n" NL);
			pAuction->item = NULL;
			pAuction->payment_collected = true;
			pAuction->item_collected = true;
		}
	}
	else if (won_auction(ch->name, pAuction) && !pAuction->item_collected)
	{
		if ((ch->carry_weight + get_obj_weight(pAuction->item)) > can_carry_w(ch))
		{
			act( PLAIN, "$p to zbyt du�y ci�ar jak dla ciebie w "
					"tej chwili.\n", ch, pAuction->item, NULL, TO_CHAR);
			act( PLAIN, "$n nie mo�e ud�wign�� $p$1 i odrzuca to.\n", ch, pAuction->item, NULL, TO_ROOM);
			obj_to_room(pAuction->item, ch->in_room);
		}
		else
		{
			obj_to_char(pAuction->item, ch);
			act(PLAIN, "Aukcjoner daje jaki� przedmiot $n$2.\n", ch, NULL, NULL, TO_ROOM);
			ch_printf(ch, "Aukcjoner daje Ci wylicytowany przedmiot.\n" NL);
		}
		pAuction->item = NULL;
		pAuction->item_collected = true;
		pBets = pAuction->bets.front();
		pAuction->bets.pop_front();
		free_bets(pBets);
		pAuction->bidders_count--;
	}
	else if (is_auction_member(ch->name, pAuction))
	{
		pBets = get_auction_member(ch->name, pAuction);
		ch->gold += pBets->max_price;
		ch_printf(ch, "Odbierasz z�o�on� wcze�niej got�wk�.\n");
		act(PLAIN, "Aukcjoner wyp�aca got�wka $n$2.\n", ch, NULL, NULL, TO_ROOM);
		pAuction->bets.remove(pBets);
		free_bets(pBets);
		pAuction->bidders_count--;
	}
}

void auction_show_obj_stats(CHAR_DATA *ch, OBJ_DATA *obj)
{
	SKILLTYPE *sktmp;
#if defined(ARMAGEDDON)
    REQUIREMENT_DATA 	*req;
#endif

	ch_printf(ch,
	FG_CYAN "    Przedmiot '" PLAIN "%s" FG_CYAN "' jest typu %s." EOL
	FG_CYAN "    Dodatkowe flagi: " PLAIN "%s %s" EOL, obj->name, item_type_name(obj), flag_string(obj_flags_list, obj->extra_flags),
			!str_cmp(flag_string(force_flags_list, obj->force_flags), "none") ? "" : flag_string(force_flags_list, obj->force_flags));
	ch_printf(ch, FG_CYAN
	"    Waga: " PLAIN "%d" FG_CYAN ", Poziom: " PLAIN "%d" EOL, obj->weight, obj->level);

	switch (obj->item_type)
	{
	case ITEM_WEAPON:
		ch_printf(ch, FG_CYAN
		"    Zadawane obra�enia w walce wr�cz " PLAIN "%d" FG_CYAN
		" do " PLAIN "%d" FG_CYAN " (�rednio " PLAIN "%d" FG_CYAN ")." EOL, obj->value[1], obj->value[2],
				(obj->value[1] + obj->value[2]) / 2);

		if (obj->value[3] == WEAPON_BLASTER)
		{
			ch_printf(ch, FG_CYAN
			"    Posiada jeszcze " PLAIN "%d" FG_CYAN " na " PLAIN "%d" FG_CYAN " pocisk�w." EOL, obj->value[4], obj->value[5]);
		}
		else if (obj->value[3] == WEAPON_LIGHTSABER || obj->value[3] == WEAPON_VIBRO_BLADE || obj->value[3] == WEAPON_FORCE_PIKE)
		{
			ch_printf(ch, FG_CYAN
			"    Posiada jeszcze " PLAIN "%d" FG_CYAN " z " PLAIN
			"%d" FG_CYAN " jednostek energetycznych." EOL, obj->value[4], obj->value[5]);
		}
		else if (obj->value[3] == WEAPON_BOWCASTER)
		{
			ch_printf(ch, FG_CYAN
			"    Posiada jeszcze " PLAIN "%d" FG_CYAN " z " PLAIN
			"%d" FG_CYAN " strza� energetycznych." NL, obj->value[4], obj->value[5]);
		}
		break;
	case ITEM_DEVICE:
		ch_printf(ch, FG_CYAN
		"    Posiada " PLAIN "%d" FG_CYAN "(" PLAIN "%d" FG_CYAN
		") �adunk�w na poziomie " PLAIN "%d ", obj->value[1], obj->value[2], obj->value[0]);
		if (obj->value[3] >= 0 && (sktmp = get_skilltype(obj->value[3])) != NULL)
		{
			send_to_char(" '", ch);
			send_to_char(sktmp->name, ch);
			send_to_char("'", ch);
		}
		send_to_char( FG_CYAN "." EOL, ch);
		break;

	case ITEM_AMMO:
		ch_printf(ch, FG_CYAN "    Posiada " PLAIN "%d" FG_CYAN " pocisk�w." EOL, obj->value[0]);
		break;

	case ITEM_BOLT:
		ch_printf(ch, FG_CYAN "    Posiada " PLAIN "%d" FG_CYAN " strza� energetycznych." EOL, obj->value[0]);
		break;

	case ITEM_BATTERY:
		ch_printf(ch, FG_CYAN "    Posiada " PLAIN "%d" FG_CYAN " jednostek energii." EOL, obj->value[0]);
		break;

	case ITEM_ARMOR:
		ch_printf(ch, FG_CYAN "    Aktualna klasa zbroi: " PLAIN "%d" FG_CYAN "/" PLAIN "%d" FG_CYAN "." EOL, obj->value[0], obj->value[1]);
		ch_printf(ch, FG_CYAN "    Dodatkowy wp�yw na uzbrojenie: " PLAIN "%d" FG_CYAN "." EOL, apply_ac(obj, obj->wear_loc));
		break;

	case ITEM_PILL:
	case ITEM_POTION:
		ch_printf(ch, FG_CYAN "    Daje:");

		if (obj->value[1] >= 0 && (sktmp = get_skilltype(obj->value[1])) != NULL)
		{
			send_to_char(" '" PLAIN, ch);
			send_to_char(sktmp->name, ch);
			send_to_char( FG_CYAN "'", ch);
		}

		if (obj->value[2] >= 0 && (sktmp = get_skilltype(obj->value[2])) != NULL)
		{
			send_to_char(" '" PLAIN, ch);
			send_to_char(sktmp->name, ch);
			send_to_char( FG_CYAN "'", ch);
		}

		if (obj->value[3] >= 0 && (sktmp = get_skilltype(obj->value[3])) != NULL)
		{
			send_to_char(" '" PLAIN, ch);
			send_to_char(sktmp->name, ch);
			send_to_char( FG_CYAN "'", ch);
		}

		ch_printf(ch, " na poziom " PLAIN "%d" FG_CYAN "." NL, obj->value[0]);
		break;
	}

	for (auto* paf : obj->affects)
		auction_showaffect(ch, paf);
	for (auto* paf : obj->pIndexData->affects)
		auction_showaffect(ch, paf);

#if defined(ARMAGEDDON)
	for (auto* req : obj->requirements)
	explain( ch, req, obj, true );
	for (auto* req : obj->pIndexData->requirements)
	explain( ch, req, obj, true );
#endif
	send_to_char( PLAIN, ch);

	return;
}

void auction_show_ship_info(CHAR_DATA *ch, SHIP_DATA *pShip)
{
	pager_printf(ch, FG_CYAN "    Licytowany jest " PLAIN "%s",
			pShip->type == SHIP_REPUBLIC ?
					"NowoRepublika�ski" :
					(pShip->type == SHIP_IMPERIAL ? "Imperialny" : (pShip->type == SHIP_PIRATE ? "Piracki" : "Cywilny")));

	pager_printf(ch, FG_CYAN " statek klasy:" PLAIN " %s" EOL FG_CYAN "    o nazwie :" PLAIN " %s" EOL,
			bit_name(ship_classes_name_list, pShip->clazz), SHIPNAME(pShip));

	pager_printf(ch, FG_CYAN "    Nazwa fabryczna:" PLAIN " %s" EOL, pShip->name);

	pager_printf(ch, FG_CYAN "    Klasa skilla: " PLAIN "%s" EOL FG_CYAN,
			is_scout(pShip) ?
					"scoutships" :
					(is_fighter(pShip) ?
							"starfighters" :
							(is_midship(pShip) ?
									"midships" :
									(is_freighter(pShip) ?
											"freighters" :
											(is_capital(pShip) ?
													"capital ships" :
													(is_huge(pShip) ?
															"superships" :
															(is_platform(pShip) ? "platforms" : "Hmm jaki� b��d albo speeder")))))));

	pager_printf(ch, FG_CYAN "    Dzia�ka Laserowe:    " PLAIN "%d"
	FG_CYAN "    Dzia�ka Jonowe:    " PLAIN "%d" NL, pShip->lasers, pShip->ioncannons);
	pager_printf(ch, FG_CYAN "    Ilo�� pocisk�w: " PLAIN "%d" FG_CYAN " / "
	PLAIN "%d" FG_CYAN " / " PLAIN "%d" FG_CYAN "   %s" NL, pShip->maxmissiles, pShip->torpedos, pShip->rockets,
			pShip->trawler == 0 ? "" : "Zbieracz Min");
	pager_printf(ch, FG_CYAN "    Ilo�� Flar:     " PLAIN "%d" NL, pShip->maxchaff);
	pager_printf(ch, FG_CYAN "    Wytrzyma�o�� Kad�ubu: " PLAIN "%d" NL, pShip->maxhull);
	pager_printf(ch, FG_CYAN "    Moc Tarczy: " PLAIN "%d"
	FG_CYAN "   Energia(paliwo): " PLAIN "%d" NL, pShip->maxshield, pShip->maxenergy);
	pager_printf(ch, FG_CYAN "    Pr�dko��:   " PLAIN "%.0f"
	FG_CYAN "   Hiperpr�dko��:        " PLAIN "%.0f" NL, pShip->realspeed, pShip->hyperspeed);
	pager_printf(ch, FG_CYAN "    Radar:           " PLAIN "Mk%d"
	FG_CYAN "  System rozpoznawczy:  " PLAIN "Mk%d" NL, pShip->sensor, pShip->target_array);
	pager_printf(ch, FG_CYAN "    �adowno�� " PLAIN "%d" NL, pShip->maxcargo);
}

void show_all_betters(CHAR_DATA *ch, AUCTION_DATA *pAuction)
{
	const size_t bufsize = 150;
	char buf[bufsize], color[3];

	pager_printf(ch, PLAIN "      [" FG_CYAN "Nick           " PLAIN "][" FG_CYAN "Ilo��   " PLAIN "]["
	FG_CYAN "Cena    " PLAIN "]");
	//tutaj mozna doda� modyfikacje d�ugo�ci lini
	if ( IS_IMMORTAL(ch) || is_auction_member(ch->name, pAuction))
		ch_printf(ch, "[" FG_CYAN "Max cena" PLAIN "]" NL
		"     ------------------------------------------------" NL);
	else
		ch_printf(ch, NL FG_CYAN "     ----------------------------" NL);

	if (pAuction->bets.empty())
	{
		if (IS_IMMORTAL(ch))
			pager_printf(ch, NL "   ");
		ch_printf(ch, FG_CYAN "          Brak licytuj�cych" NL NL NL);
	}
	else
		for (auto* pBets : pAuction->bets)
		{
			if (pBets != pAuction->bets.front())
				sprintf(color, FG_CYAN);
			else
				sprintf(color, FB_YELLOW);
			swsnprintf(buf, bufsize, FG_YELLOW "     %s" PLAIN "[%s%-15s" PLAIN "]", (!str_cmp(pBets->name, ch->name)) ? "*" : " ", color, pBets->name);
			swsnprintf(buf, bufsize, "%s" PLAIN "[%s%-8lld" PLAIN "]", buf, color, pBets->quantity);
			swsnprintf(buf, bufsize, "%s" PLAIN "[%s%-8lld" PLAIN "]", buf, color, pBets->price);

			if ( IS_IMMORTAL ( ch ) || !str_cmp(pBets->name, ch->name))
				swsnprintf(buf, bufsize, "%s" PLAIN "[%s%8lld" PLAIN "]", buf, color, pBets->max_price);
			ch_printf(ch, buf);
			ch_printf(ch, NL);
		}
}

void show_ship_info(CHAR_DATA *ch, SHIP_DATA *pShip)
{
	if (!pShip)
	{
		bug("show_ship_info: NULL pShip");
		return;
	}

	pager_printf(ch, FG_CYAN "Licytowany jest " FB_CYAN "%s",
			pShip->type == SHIP_REPUBLIC ?
					"NowoRepublika�ski" :
					(pShip->type == SHIP_IMPERIAL ? "Imperialny" : (pShip->type == SHIP_PIRATE ? "Piracki" : "Cywilny")));

	pager_printf(ch, FG_CYAN " statek klasy:" FB_CYAN " %s" EOL FG_CYAN "o nazwie :" FB_CYAN " %s" EOL,
			bit_name(ship_classes_name_list, pShip->clazz), SHIPNAME(pShip));

	pager_printf(ch, FG_CYAN "Nazwa fabryczna:" FB_CYAN " %s" EOL, pShip->name);

	pager_printf(ch, FG_CYAN "Klasa skilla: " FB_CYAN "%s" EOL FG_CYAN,
			is_scout(pShip) ?
					"scoutships" :
					(is_fighter(pShip) ?
							"starfighters" :
							(is_midship(pShip) ?
									"midships" :
									(is_freighter(pShip) ?
											"freighters" :
											(is_capital(pShip) ?
													"capital ships" :
													(is_huge(pShip) ?
															"superships" :
															(is_platform(pShip) ? "platforms" : "Hmm jaki� b��d albo speeder")))))));

	if (*pShip->description)/* I tutaj ja -- Than */
		pager_printf(ch, "%s" EOL, pShip->description);
	pager_printf(ch, FG_CYAN "Dzia�ka Laserowe:    " FB_CYAN "%d"
	FG_CYAN "    Dzia�ka Jonowe:    " FB_CYAN "%d" NL, pShip->lasers, pShip->ioncannons);
	pager_printf(ch, FG_CYAN "Max. Ilo�� pocisk�w: " FB_CYAN "%d" FG_CYAN " / "
	FB_CYAN "%d" FG_CYAN " / " FB_CYAN "%d" FG_CYAN "   %s" NL, pShip->maxmissiles, pShip->torpedos, pShip->rockets,
			pShip->trawler == 0 ? "" : "Zbieracz Min");
	pager_printf(ch, FG_CYAN "Max. Ilo�� Flar:     " FB_CYAN "%d" NL, pShip->maxchaff);
	pager_printf(ch, FG_CYAN "Max. Wytrzyma�o�� Kad�ubu: " FB_CYAN "%d" NL, pShip->maxhull);
	pager_printf(ch, FG_CYAN "Max. Moc Tarczy: " FB_CYAN "%d"
	FG_CYAN "   Max. Energia(paliwo): " FB_CYAN "%d" NL, pShip->maxshield, pShip->maxenergy);
	pager_printf(ch, FG_CYAN "Max. Pr�dko��:   " FB_CYAN "%.0f"
	FG_CYAN "    Hiperpr�dko��:        " FB_CYAN "%.0f" NL, pShip->realspeed, pShip->hyperspeed);
	pager_printf(ch, FG_CYAN "Radar:           " FB_CYAN "Mk%d"
	FG_CYAN "  System rozpoznawczy:  " FB_CYAN "Mk%d" NL, pShip->sensor, pShip->target_array);
	pager_printf(ch, FG_CYAN "�adowno�� " FB_CYAN "%d" NL, pShip->maxcargo);
}

void show_auction_data(CHAR_DATA *ch, AUCTION_DATA *pAuction, int number)
{
	/*
	 BETS_PROGRES	* pBets;
	 OBJ_DATA		* pObject;
	 pObject			= pAuction->item;
	 SHIP_DATA		* pShip;

	 if ( auction_finished( pAuction ) && !auction_creator( ch->name, pAuction )
	 && !is_auction_member( ch->name, pAuction ) )
	 {
	 ch_printf( ch, "Ta aukcja jest ju� zako�czona i nie masz do"
	 " niej dost�pu." NL );
	 return;
	 }
	 if ( !auction_started( pAuction ) && !auction_creator( ch->name, pAuction) )
	 {
	 ch_printf( ch, "Ta aukcja jeszcze si� nie rozpocze�a." NL);
	 return;
	 }
	 if ( auction_ship( pAuction ) )
	 pShip = get_ship(pAuction->item_name);
	 else
	 pShip = NULL;

	 time_t time_diff;
	 time_diff = difftime( pAuction->end_time, current_time );


	 ch_printf( ch,	NL FG_CYAN "LICYTACJA nr. " FB_CYAN "%d" NL
	 FG_CYAN "---------------------------------------" NL,number );
	 ch_printf( ch, FG_CYAN "Sprzedawca " FB_CYAN "%s" FG_CYAN "  %s" EOL EOL,
	 pAuction->seller_name,
	 (auction_creator(ch->name,pAuction)) ? "(czyli Ty)":"");
	 ch_printf( ch, FG_CYAN "Planeta : " FB_CYAN "%s" EOL,pAuction->planet);

	 if ( pObject )
	 show_obj_stats( pObject, ch );
	 else if ( auction_ship( pAuction ) )
	 show_ship_info(ch, pShip );
	 else
	 ch_printf( ch, FG_CYAN "Przedmiot licytacji zosta� ju� odebrany" NL);

	 ch_printf( ch, FG_CYAN "Opis :" FB_CYAN "%s" EOL,pAuction->desc );

	 if ( pAuction->bet > 0 )
	 ch_printf ( ch, NL FG_CYAN "Aktualna stawka  : " FB_CYAN "%s" FG_CYAN " kredyt%s." NL NL,
	 advitoa( pAuction->bet, true),
	 NUMBER_SUFF( pAuction->bet, "k�", "ki", "ek" ) );



	 if ( pAuction->bets.empty() )
	 ch_printf( ch, NL FG_CYAN "Brak licytuj�cych" NL);
	 else
	 {
	 if ( IS_IMMORTAL(ch) )
	 ch_printf( ch, FG_CYAN " [Nick           ][Cena    ][Max cena]" NL
	 " -------------------------------------" NL);
	 else
	 ch_printf( ch, FG_CYAN " --------------------------------" NL);
	 for (auto* pBets : pAuction->bets)
	 {
	 if ( pBets != pAuction->bets.front() )
	 ch_printf( ch, FG_YELLOW "%s" FB_CYAN "[" PLAIN "%-15s" FB_CYAN "]["
	 PLAIN "%8lld" FB_CYAN "]",(!str_cmp(pBets->name,ch->name)) ? "*":" ",
	 pBets->name,pBets->price);
	 else
	 ch_printf( ch, FG_YELLOW "%s" FB_CYAN "[" FB_YELLOW "%-15s"
	 FB_CYAN "][" FB_YELLOW "%8lld" FB_CYAN "]" PLAIN,
	 (!str_cmp(pBets->name,ch->name)) ? "*":" ",
	 pBets->name,pBets->price);

	 if ( IS_IMMORTAL ( ch ) )
	 ch_printf( ch, FB_CYAN "[" PLAIN "%8lld" FB_CYAN "]" PLAIN NL,
	 pBets->max_price);
	 else    ch_printf( ch, NL);
	 }
	 }

	 if ( auction_creator( ch->name, pAuction ) )
	 ch_printf( ch, NL FG_CYAN "  Twoja aukcja ");
	 else    ch_printf( ch, NL FG_CYAN "  Aukcja ");
	 if ( !auction_started( pAuction ) )
	 ch_printf( ch, FG_CYAN "rozpocznie si� za %s" NL,
	 prepare_time(pAuction->start_time,false,false));
	 else if ( !auction_finished( pAuction ) )
	 ch_printf( ch, FG_CYAN "zako�czy si� za %s" NL,
	 prepare_time(pAuction->end_time,false,false));
	 else 	ch_printf( ch, FG_CYAN "zako�czy�a si�" NL);
	 */
}

void show_1_auction_list(CHAR_DATA *ch, AUCTION_DATA *pAuction, int index)
{
	//UWAGA funkcja bedzie rozszerzana - na razie ma dzialac tylko dla obiekt�w
	OBJ_DATA *pObject = NULL;
	SHIP_DATA *pShip = NULL;
	char buf[15];

	if (pAuction->type == AUCTION_SHIP)
		pShip = get_ship(pAuction->stock_name);
	else if (pAuction->type == AUCTION_ITEM)
		pObject = pAuction->item;
	if (pAuction->buy_out_now)
		sprintf(buf, "%lld", pAuction->buy_out_now);
	else
		sprintf(buf, "BRAK");

	//ustawiamy znacznik udzialu w tej aukcji oraz jej numer
	pager_printf(ch, FG_YELLOW "    %s" PLAIN "[" MOD_BOLD "%-2d" PLAIN "]",
			(auction_creator(ch->name, pAuction) || is_auction_member(ch->name, pAuction)) ? "*" : " ", index);
	//ustawimy nazwe licytowanego przedmiotu
	if (pAuction->type == AUCTION_ITEM && !pAuction->item_collected && !pObject)
	{
		//mozna sie pokusic o automatyczny fixing
		bug("Pointer to object being auctioned is broken");
	}
	if (pAuction->item_collected)
		pager_printf(ch, "[" FB_YELLOW "%-20s" PLAIN "]", "ODEBRANY");
	else if (pAuction->type == AUCTION_ITEM && pObject)
		pager_printf(ch, "[" FB_YELLOW "%-20s" PLAIN "]", strip_colors(pObject->przypadki[0], 20));
	else if (pAuction->type == AUCTION_SHIP && pShip)
		pager_printf(ch, "[" FB_YELLOW "%-20s" PLAIN "]", strip_colors(pShip->name, 20));
	//ustawiamy ilosc licytantow i kwote
	pager_printf(ch, "[" MOD_BOLD "%-3d" PLAIN "]", pAuction->quantity);
	pager_printf(ch, "[" MOD_BOLD "%-3d" PLAIN "]", pAuction->bidders_count);
	pager_printf(ch, "[" MOD_BOLD "%-11s" PLAIN "]", advitoa(pAuction->bet, true));
	//ustawimy znacznik BuyOut oraz podajemy czas do zako�czenia trwania aukcji
	const SWString &prepTime = prepare_time(pAuction->end_time, true, false);
	pager_printf(ch, "[" PLAIN MOD_BOLD "%s" PLAIN "][" PLAIN
	MOD_BOLD "%-12s" PLAIN "]" PLAIN NL, pAuction->buy_out_now ? "T" : "N", prepTime.c_str());
}

//AUCTION_DATA * find_auction_nr( int nr)
//{
//	return NULL;
//}

/*! \brief Posortuj aukcje wedle czasu ich uko�czenia */
void sort_stock_market_auctions(STOCK_EXCHANGE_DATA *pStock)
{
	pStock->auctions.sort([](const AUCTION_DATA *a, const AUCTION_DATA *b)
	{
		return a->end_time < b->end_time;
	});
}

/*! \brief Posortuj stawki licytant�w	*/
void sort_auction_bets(AUCTION_DATA *pAuction)
{
	/*!
	 *	Posortuj stawki licytant�w wedle maksymalnej kwoty jak� postawili
	 *	oraz oblicz ile tak naprawd� b�d� musieli zap�aci�
	 */
	//sortujemy licytantow po max_stawkach (najwyzsza na poczatku)
	pAuction->bets.sort([](const BETS_PROGRES *a, const BETS_PROGRES *b)
	{
		return a->max_price > b->max_price;
	});

	//teraz ustalamy, ile kto placi - mamy licytantow posortowanych
	//wedle tego ile placa
	for (auto it = pAuction->bets.begin(); it != pAuction->bets.end(); ++it)
	{
		BETS_PROGRES *pBets = *it;
		bool hasPrev = (it != pAuction->bets.begin());
		auto nextIt = std::next(it);
		bool hasNext = (nextIt != pAuction->bets.end());
		BETS_PROGRES *pNextBets = hasNext ? *nextIt : nullptr;

		if (!hasNext)		//czy to koniec listy
		{
			if (!hasPrev)	//czy to jedyna stawka
			{
				if (pBets->buy_out) //
				{
					pBets->price = pAuction->buy_out_now;
					pAuction->bet = pAuction->buy_out_now;
				}
				else if (pAuction->starting_price) //aukcja ma stawke inna niz 0
					pBets->price = pAuction->starting_price;
				else
					pBets->price = pAuction->starting_price + 5;
			}
			else
				pBets->price = pBets->max_price;
		}
		//je�li to �rodek listy - poprzednik dal wiecej ni� ta stawka - czyli max_price
		else if (hasPrev)
			pBets->price = pBets->max_price;
		else if (pBets->buy_out)
			pBets->price = pAuction->buy_out_now;
		//a to znaczy ze jestesmy na poczatku i mamy nastepce
		else
		{
			pBets->price = pNextBets->max_price + 5;
			pAuction->bet = pBets->price;
		}
	}
	if (pAuction->bets.empty())
		pAuction->bet = pAuction->starting_price;
	else if (pAuction->bets.front()->buy_out)
		pAuction->bet = pAuction->buy_out_now;
	else
		pAuction->bet = pAuction->bets.front()->price;
}

void auction_buy_out(CHAR_DATA *ch, AUCTION_DATA *pAuction)
{
	/*! Gracz dokona� wykupu natychmiastowego. Zmodyfikuj wpis
	 *	w li�cie bior�cych w niej licytant�w (dodaj�c wedle potrzeby
	 *	tego gracza), posortuj t� list�, zako�cz sam� aukcj� oraz
	 *	wy�lij do zainteresowanych notki
	 */
	BETS_PROGRES *pBets;
	if (!pAuction->buy_out_now)
	{
		ch_printf(ch, "Ta aukcja nie ma opcji natychmiastowego wykupu" NL);
		return;
	}

	if (auction_finished(pAuction))
	{
		ch_printf(ch, "Ta aukcja ju� si� zako�czy�a." NL);
		return;
	}
	if (!auction_started(pAuction) && !auction_creator(ch->name, pAuction))
	{
		ch_printf(ch, "Ta aukcja jeszcze si� nie rozpocze�a." NL);
		return;
	}
	if (!auction_started(pAuction))
	{
		ch_printf(ch, "Ta aukcja jeszcze si� nie rozpocze�a." NL);
		return;
	}

	if (pAuction->bet >= pAuction->buy_out_now || (!pAuction->bets.empty() && pAuction->bets.front()->max_price >= pAuction->buy_out_now))
	{
		ch_printf(ch, "Juz za p�no na natychmiastowy wykup, "
				"aktualna stawka jest wy�sza." NL);
		return;
	}

	if (auction_creator(ch->name, pAuction))
	{
		ch_printf(ch, "Jeste� tw�rc� tej aukcji, nie mo�esz dokona�"
				"natychmiastowego wykupu." NL);
		return;
	}

	if (is_auction_member(ch->name, pAuction))
	{
		pBets = get_auction_member(ch->name, pAuction);

		if ((ch->gold + pBets->max_price) < pAuction->buy_out_now)
		{
			ch_printf(ch, "Przykro mi, ale nie masz przy sobie odpowiedniej kwoty." NL);
			return;
		}
		int diff = pAuction->buy_out_now - pBets->max_price;
		act( PLAIN, "$n realizuje jak�� transakcje z Aukcjonerem", ch, NULL, NULL, TO_ROOM);
		ch_printf(ch, "Wp�acasz Aukcjonerowi %d kredyt%s dokonuj�c "
				"natychmiastowego wykupu.", diff, NUMBER_SUFF(diff, "ke", "ki", "ek"));
		pAuction->end_time = current_time;
		ch->gold -= diff;
		pBets->max_price = pAuction->buy_out_now;
		pAuction->bets.remove(pBets);
		pAuction->bets.push_front(pBets);
		pBets->buy_out = true;
		pAuction->bidders_count++;
		sort_auction_bets(pAuction);
	}
	else
	{
		if (ch->gold < pAuction->buy_out_now)
		{
			ch_printf(ch, "Przykro mi, ale nie masz przy sobie"
					" odpowiedniej kwoty." NL);
			return;
		}
		pAuction->end_time = current_time;
		ch->gold -= pAuction->buy_out_now;
		act(PLAIN, "$n realizuje jak�� transakcje z "
				"Aukcjonerem", ch, NULL, NULL, TO_ROOM);
		ch_printf(ch, "Wp�acasz Aukcjonerowi %lld kredyt%s dokonuj�c"
				"natychmiastowego wykupu.", pAuction->buy_out_now, NUMBER_SUFF(pAuction->buy_out_now, "ke", "ki", "ek"));
		pBets = new_bets();
		STRDUP(pBets->name, ch->name);
		pBets->max_price = pAuction->buy_out_now;
		pBets->quantity = 1;
		pBets->buy_out = true;
		pAuction->bidders_count++;
		//potrzebuje wstawic jako pierwsza pozycje
		pAuction->bets.push_front(pBets);
		sort_auction_bets(pAuction);
	}
}

void send_auction_notes(AUCTION_DATA *pAuction, auction_ends typ)
{
	/*
	 char			buf[MSL];
	 BETS_PROGRES	* pBets;

	 switch (typ)
	 {
	 case ITEM_SOLD:
	 {
	 sprintf( buf, "Twoja aukcja zako�czy�a si�. Zaoferowany przez Ciebie" NL
	 "%s zosta� kupiony za %lld kredyt%s." NL
	 "Zg�o� si� do najbli�szego domu aukcyjnego celem odebrania got�wki",
	 pAuction->item_name,pAuction->bet, NUMBER_SUFF(pAuction->bet,"ke","ki","ek"));
	 note("Aukcjoner",pAuction->seller_name,"Zako�czenie aukcji",buf);

	 //teraz notka dla zwyciezcy licytacji
	 sprintf( buf,   "Aukcja zako�czy�a si� Twoim zwyci�stwem." NL
	 "Zg�o� si� do najbli�szego domu aukcyjnego, by odebra��" NL
	 "wygrany  %s.",pAuction->item_name);
	 note("Aukcjoner",pAuction->bets.front()->name,"Zako�czenie aukcji",buf);

	 { auto it = pAuction->bets.begin(); ++it;
	 for (; it != pAuction->bets.end(); ++it)
	 { BETS_PROGRES *pBets = *it;
	 sprintf( buf,
	 "Aukcja zako�czy�a, lecz to nie Twoim zwyci�stwem." NL
	 "Nast�pnym razem postaw wi�cej, jesli chcesz wygra�." NL);
	 note("Aukcjoner",pBets->name,"Zako�czenie aukcji",buf);
	 } }
	 pAuction->notes_send = true;
	 break;
	 }
	 case SHIP_SOLD:
	 break;
	 case NO_BIDDERS:
	 {
	 sprintf( buf, "Przykro mi, ale Twoja nie cieszy�a"
	 " si� popularno�ci�." NL
	 "Nikt nie chcia� kupi� zaoferowanego przez Ciebie %s." NL
	 "Zglos si� po odbi�r licytowanego towaru.",
	 pAuction->item_name);
	 note("Aukcjoner",pAuction->seller_name,"Zako�czenie aukcji",buf);
	 pAuction->notes_send = true;
	 return;
	 }
	 case IMMO_IMPACT:
	 {
	 sprintf( buf, "Wpl�ta�y si� si�y wy�sze. Twoja aukcja zosta�a "
	 "natychmiast zako�czona" NL);
	 note("Aukcjoner",pAuction->seller_name,"Zako�czenie aukcji",buf);

	 sprintf( buf, "Wpl�ta�y si� si�y wy�sze. Aukcja zosta�a natychmiast zako�czona," NL);
	 { auto it = pAuction->bets.begin(); ++it;
	 for (; it != pAuction->bets.end(); ++it)
	 note("Aukcjoner",(*it)->name,"Zako�czenie aukcji",buf); }
	 pAuction->notes_send = true;
	 break;
	 }
	 default:
	 pAuction->notes_send = true;
	 break;
	 }*/
}

void send_auction_info(AUCTION_DATA *pAuction)
{
	/*
	 char buf[MSL];
	 if ( auction_ship( pAuction ) )
	 sprintf (buf,
	 "Na aukcj� zostaje wystawiony statek kosmiczny" NL
	 "%s za %lld kredyt%s.",
	 pAuction->item_name, pAuction->starting_price,
	 NUMBER_SUFF( pAuction->starting_price, "k�", "ki", "ek" ) );
	 else
	 sprintf (buf,
	 "Na aukcj� zostaje wystawiony nowy"
	 " przedmiot: %s za %lld kredyt%s.",
	 pAuction->item->przypadki[0], pAuction->starting_price,
	 NUMBER_SUFF( pAuction->starting_price, "k�", "ki", "ek" ) );
	 talk_auction (buf);
	 */
}
