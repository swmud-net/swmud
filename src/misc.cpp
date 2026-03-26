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
 *	    Misc module for general commands: not skills or spells	   *
 ****************************************************************************
 * Note: Most of the stuff in here would go in act_obj.c, but act_obj was   *
 * getting big.								   *
 ****************************************************************************/

#include <sys/types.h>
#include <time.h>
#include <list>
#include <ranges>
#include "mud.h"
#include "classes/SWInvite.h"

extern int top_exit;
static CLONING_TMP_DATA tmp_cloning;
std::list<SWInvite*> invites_list;

/*added by Thanos*/
bool get_comlink(CHAR_DATA *ch)
{
	if (IS_NPC(ch))
		return true;

	if (IS_IMMORTAL(ch))
		return true;

	if (IS_ADMIN(ch->name)) /*mo�e by�, �e admin nie jest immo*/
		return true;

	for (auto* obj : std::ranges::reverse_view(ch->carrying))
	{
		if (obj->pIndexData->item_type == ITEM_COMLINK)
			return true;
	}

	return false;
}
/*done*/

DEF_DO_FUN( buyhome )
{
	ROOM_INDEX_DATA *room;

	if (!ch->in_room)
		return;

	if ( IS_NPC(ch) || !ch->pcdata)
		return;

	if (ch->plr_home != NULL)
	{
		send_to_char("Przecie� masz ju� dom." NL, ch);
		return;
	}

	room = ch->in_room;

	for (auto* pArea : bsort_list)
	{
		if (room->area == pArea)
		{
			send_to_char("Ta krainka nie jest jeszcze cz�ci� tego �wiata." NL, ch);
			return;
		}
	}

	if (!IS_SET(room->room_flags, ROOM_EMPTY_HOME))
	{
		send_to_char("Te pomieszczenie nie jest na sprzeda�!" NL, ch);
		return;
	}

	if (ch->gold < 100000)
	{
		send_to_char("Ten pok�j kosztuje 100000 kredytek. Nie masz tyle." NL, ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char("Ustal nazw� pomieszczenia. Taki jednolinijkowy opisik." NL, ch);
		send_to_char("Sk�adnia: Buyhome <nazwa_pokoju>" NL, ch);
		return;
	}

	STRDUP(room->homename, argument);

	ch->gold -= 100000;

	REMOVE_BIT(room->room_flags, ROOM_EMPTY_HOME);
	SET_BIT(room->room_flags, ROOM_PLR_HOME);

	/*     fold_area( room->area, room->area->filename, false ); Trog: to juz niepotrzebne */

	ch->plr_home = room;
	do_save(ch, (char*) "");

}

/* Proste sprawdzenie czy dana osoba zostala zaproszona do danego mieszkania.
 * Sprawdza tez, czy wlasciciel jest w srodku - Ganis*/
const bool is_invited(const CHAR_DATA *ch, const ROOM_INDEX_DATA *home)
{

	if (!IS_SET(home->room_flags, ROOM_PLR_HOME))
		return true; //Tak na wszelki wypadek

	bool has_invitation = false;
	list<SWInvite*>::const_iterator it;

	for (it = invites_list.begin(); it != invites_list.end(); ++it)
	{
		if ((*it)->home == home && (*it)->name == ch->name)
		{
			has_invitation = true;
			break;
		}
	}

	//test, sprawdzajacy czy wlasciciel jest w domu
	if (has_invitation)
		for (auto* cha : home->people)
			if (cha->plr_home == home)
				return true;

	return false;
}

/*Przenosi graczy do pokojow z ktorych przyszli  - Ganis*/
void evacuate_guests(ROOM_INDEX_DATA *home)
{
	list<CHAR_DATA*> evac_list;
	CHAR_DATA *ch;

	if (!home)
	{
		bug("Trying to evacuate guests from null room." NL);
		return;
	}

	if (!IS_SET(home->room_flags, ROOM_PLR_HOME))
	{
		bug("Trying to evacuate room that isn't players home (%d)." NL, home->vnum);
		return;
	}

	for (auto* ch : home->people)
	{
		if (ch->plr_home == home)
			continue;
		send_to_char("Niestety nie mo�esz pozosta� w mieszkaniu pod nieobecno�� w�a�ciciela." NL, ch);
		evac_list.push_back(ch);
	}

	while (!evac_list.empty())
	{
		ch = evac_list.front();
		evac_list.pop_front();
		if (IS_NPC(ch))
			continue;
		char_from_room(ch);
		if (!ch->was_in_room)
			bug("Can't evacuate %s from home (Room vnum: %d)." NL, ch->name, home->vnum);
		char_to_room(ch, ch->was_in_room);
		do_look(ch, (char*) "auto");
	}
}

/*Zaprasz danego gracza do mieszkania - Ganis */
DEF_DO_FUN( invite )
{
	char arg1[MIL];
	ROOM_INDEX_DATA *home;
	CHAR_DATA *victim;
	SWInvite *invite;

	if (!ch->plr_home)
	{
		send_to_char("Przecie� nie masz mieszkania!" NL, ch);
		return;
	}

	home = ch->plr_home;

	if (home != ch->in_room)
	{
		send_to_char("Aby kogo� zaprosi� do swojego mieszkania musisz si� w nim znajdowa�." NL, ch);
		return;
	}

	argument = one_argument(argument, arg1);

	if (!*arg1)
	{
		send_to_char("Kogo chcesz zaprosi�?" NL, ch);
		return;
	}

	if (!(victim = get_player_world(ch, arg1)))
	{
		send_to_char("Nie ma takiej osoby w grze." NL, ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Jak mi�o, zapraszasz si� do swojego mieszkania." NL, ch);
		return;
	}

	if (is_invited(victim, home))
	{
		send_to_char("Ta osoba jest ju� zaproszona do Twojego mieszkania." NL, ch);
		return;
	}

	invite = new SWInvite(home, victim->name);
	invites_list.push_back(invite);
	act(COL_INVITE, "$n zaprasza Ci� do swojego mieszkania.", ch, NULL, victim,
	TO_VICT);
	act(COL_INVITE, "Zapraszasz $N$3 do swojego mieszkania.", ch, NULL, victim,
	TO_CHAR);
}

/*Wycofuje zaproszenie - Ganis */
DEF_DO_FUN( uninvite )
{
	char arg1[MIL];
	char buf[MSL];
	ROOM_INDEX_DATA *home;
	std::list<SWInvite*>::iterator it;
	SWInvite *tmp;
	bool all;
	std::list<SWInvite*> tmp_list;
	CHAR_DATA *guest;

	if (!ch->plr_home)
	{
		send_to_char("Przecie� nie masz mieszkania!" NL, ch);
		return;
	}

	home = ch->plr_home;
	argument = one_argument(argument, arg1);

	if (!*arg1)
	{
		send_to_char("U�ycie: uninvite <nick> lub uninvite all" NL, ch);
		return;
	}

	all = !strcmp(arg1, "all");

	if (!all)
		arg1[0] = UPPER(arg1[0]); //bo one_argument splaszczal imie

	if (!strcmp(arg1, ch->name))
	{
		send_to_char("Chcesz si� wyprosi� ze swojego mieszkania? Tak si� nie da." NL, ch);
		return;
	}

	for (it = invites_list.begin(); it != invites_list.end(); ++it)
	{
		if ((*it)->home == home && (all || (*it)->name == arg1))
		{
			tmp_list.push_back(*it);
		}
	}

	if (tmp_list.empty())
	{
		if (all)
			send_to_char("Lista os�b zaproszonych do Twojego mieszkania jest ju� pusta!" NL, ch);
		else
			send_to_char("Nikogo takiego nie ma na liscie os�b zaproszonych do Twojego mieszkania." NL, ch);
		return;
	}

	while (!tmp_list.empty())
	{
		tmp = tmp_list.front();
		tmp_list.pop_front();
		invites_list.remove(tmp);
		sprintf(buf, "%s nie jest ju� mile widzinym go�ciem w Twoim mieszkaniu." NL, tmp->name.c_str());
		send_to_char(buf, ch);
		strcpy(buf, tmp->name.c_str());
		if ((guest = get_player_world(ch, buf)))
		{
			sprintf(buf, "Nie jeste� ju� mile widzian%c w mieszkaniu $n$1.",
			FEMALE( guest ) ? 'a' : 'y');
			act(FB_RED, buf, ch, NULL, guest, TO_VICT);
		}
		delete tmp;
	}
}

/*Wy�wietla list� zaproszonych os�b - Ganis*/
DEF_DO_FUN( invites )
{
	char buf[MSL];
	std::list<SWInvite*>::const_iterator it;

	buf[MSL - 1] = '\0';
	buf[0] = '\0';

	if (!ch->plr_home)
	{
		strcat(buf, "Przecie� nie masz mieszkania, do kt�rego ");
		strcat(buf, FEMALE( ch ) ? "mog�aby� " : "m�g�by� ");
		strcat(buf, "kogokolwiek zaprasza�." NL);
		send_to_char(buf, ch);
		return;
	}

	strcat(buf, "Lista os�b zaproszonych do Twojego mieszkania:" NL);
	for (it = invites_list.begin(); it != invites_list.end(); ++it)
	{
		if ((*it)->home == ch->plr_home)
		{
			strcat(buf, (*it)->name.c_str());
			strcat(buf, NL);
		}
	}

	send_to_char(buf, ch);
}

bool does_knows_name(CHAR_DATA *ch, char *vict_name)
{
	if (IS_NPC(ch)) // Pixel: Wszystkie moby maj� by� znane graczom, decyzja spowodowana
		return true; // brakiem sensownej alternatywy, a gracze maj� problemy z interakcj�

	if (IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;

	if (!strcmp(ch->name, vict_name))
		return true;
	//jesli victim ma ustawiona flage to kazdy ja zna
	//jesli obaj sa koderami - troche to toporne, ale nie bede grzabal
	for (auto* known : ch->known)
	{
		if (!strcmp(known->name, vict_name))
			return true;
	}
	return false;
}

//Tanglor - sprawdza czy ch zna victim
bool does_knows(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if ( IS_NPC( ch ) || IS_NPC(victim)) // Pixel: Wszystkie moby maj� by� znane graczom, decyzja spowodowana
		return true; // brakiem sensownej alternatywy, a gracze maj� problemy z interakcj�

	if (!strcmp(ch->name, victim->name))
		return true;
	if (IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;
	if (ch == supermob || victim == supermob)
		return true;

	//jesli victim ma ustawiona flage to kazdy ja zna
	if (IS_SET(victim->act, PLR_KNOWN4ALL))
		return true;

	//jesli obaj sa koderami - troche to toporne, ale nie bede grzabal
	//w act bez potrzeby
	if ((IS_CODER(ch->name) && IS_CODER(victim->name)) && (IS_ADMIN(ch->name) && IS_ADMIN(victim->name)))
		return true;
	//jesli obaj sa olcmanemi
	if ( IS_OLCMAN(ch) && IS_OLCMAN(victim))
		return true;

	// Pixel: nh i newbie sie znaja nawzajem
	if (IS_NEWBIE(ch) && IS_NH(victim))
		return true;

	if (IS_NH(ch) && IS_NEWBIE(victim))
		return true;

	for (auto* known : ch->known)
	{
		if (!strcmp(known->name, victim->name))
			return true;
	}
	return false;
}

//Tanglor
const SWString char_attribute(desc_type desc_type, int index, int gender, int przypadek)
{
	char buf[MSL] =
	{ 0 };
	snprintf(buf, MSL, "%s", desc_table[desc_type][index].base);

	if (desc_table[desc_type][index].type != FUR)
	{
		SWString tmpbuf(buf);
		swsnprintf(buf, MSL, "%s%s", tmpbuf.c_str(),
				desc_table[desc_type][index].end == Y_END ?
						(gender == SEX_MALE ? male_desc_y[przypadek] : female_desc_y[przypadek]) :
						(gender == SEX_MALE ? male_desc_i[przypadek] : female_desc_i[przypadek]));
	}

	return SWString(buf);
}
//Tanglor
const SWString format_char_attribute(CHAR_DATA *ch, int przypadek)
{
	char buf[MSL] =
	{ 0 };
	if (!IS_DESCRIBED(ch))
	{
		sprintf(buf, "%s", CH_RACE(ch, przypadek)); // Pixel: przypadkiF
		return SWString(buf);
	}

	if (ch->attribute1 >= 0)
	{
		const SWString &attrib = char_attribute((desc_type) (ch->attribute1 == 0 ? 0 : ch->attribute1 / 100), ch->attribute1 % 100, ch->sex,
				przypadek);
		strcpy(buf, attrib.c_str());
	}

	SWString tmpbuf(buf);
	if (ch->attribute2 >= 0)
	{
		if (desc_table[ch->attribute2 == 0 ? 0 : ch->attribute2 / 100][ch->attribute2 % 100].type == FUR)
		{
			const SWString &attrib = char_attribute((desc_type) (ch->attribute2 == 0 ? 0 : ch->attribute2 / 100), ch->attribute2 % 100,
					ch->sex, przypadek);
			swsnprintf(buf, MSL, "%s %s z %s futrem", tmpbuf.c_str(), CH_RACE(ch, przypadek), // Pixel: przypadkiF
			attrib.c_str());
		}
		else
		{
			const SWString &attrib = char_attribute((desc_type) (ch->attribute2 == 0 ? 0 : ch->attribute2 / 100), ch->attribute2 % 100,
					ch->sex, przypadek);
			swsnprintf(buf, MSL, "%s, %s %s", tmpbuf.c_str(), attrib.c_str(), CH_RACE(ch, przypadek)); // Pixel: przypadkiF
		}
	}
	else
	{
		swsnprintf(buf, MSL, "%s %s", tmpbuf.c_str(), CH_RACE(ch, przypadek)); // Pixel: przypadkiF
	}
	return SWString(buf);
}

/* Trog: only for use in PERS macro, also: not thread safe */
char* format_char_attribute_wrapper(CHAR_DATA *ch, int przypadek)
{
	static char buf[MSL] =
	{ 0 };
	const SWString &attrib = format_char_attribute(ch, przypadek);
	swstrncpy(buf, attrib.c_str(), MSL - 1);
	buf[MSL - 1] = '\0';
	return buf;
}

CLONING_DATA* get_cloning(ROOM_INDEX_DATA *room)
{
	for (auto* cloning : cloning_list)
	{
		if (cloning->entrance == room)
			return cloning;
	}

	return NULL;
}

CLONING_DATA* get_cloning_by_cyl(ROOM_INDEX_DATA *room)
{
	for (auto* cloning : cloning_list)
	{
		if (cloning->cylinder == room)
			return cloning;
	}

	return NULL;
}

//Trog
void clone_cut_credits(CHAR_DATA *ch)
{
	tmp_cloning.credits = ch->gold;
	ch->gold = 0;
}

//Trog
void clone_restore_credits(CHAR_DATA *ch)
{
	ch->gold = tmp_cloning.credits;
}

/* Trog: sciecie gracza przed sklonowaniem (klonownie nie sa doskonale :>) */
void clone_cut(CHAR_DATA *ch)
{
	int ability;

	if (!IS_IMMORTAL(ch))
	{
		tmp_cloning.top_level = ch->top_level;
		ch->top_level = UMAX(1, ch->top_level - ch->top_level / 5);

		for (ability = 0; ability < MAX_ABILITY; ability++)
		{
			tmp_cloning.skill_level[ability] = ch->skill_level[ability];
			ch->skill_level[ability] = UMAX(1, ch->skill_level[ability] - ch->skill_level[ability] / 5);
			tmp_cloning.experience[ability] = ch->experience[ability];
			ch->experience[ability] = exp_level(ch->skill_level[ability]);
		}

		clone_cut_credits(ch);
	}
}

/* Trog: przywracanie gracza po scieciu */
void clone_restore(CHAR_DATA *ch)
{
	int ability;

	if (!IS_IMMORTAL(ch))
	{
		ch->top_level = tmp_cloning.top_level;

		for (ability = 0; ability < MAX_ABILITY; ability++)
		{
			ch->skill_level[ability] = tmp_cloning.skill_level[ability];
			ch->experience[ability] = tmp_cloning.experience[ability];
		}

		clone_restore_credits(ch);

		memset(&tmp_cloning, 0, sizeof(tmp_cloning));
	}
}

/* Improved by Thanos */
DEF_DO_FUN( clone )
{
	long amt = 0;
	CLONING_DATA *cloning;

	if (IS_NPC(ch))
	{
		ch_printf(ch, "Tak, pewnie!" NL);
		return;
	}

	/* clone <gracz> dla immosow -- Thanos */
	if (*argument && get_trust(ch) > 103)
	{
		char arg[MAX_INPUT_LENGTH];
		CHAR_DATA *victim;
		ROOM_INDEX_DATA *room;

		argument = one_argument(argument, arg);

		if ((victim = get_char_room(ch, arg)) == NULL)
		{
			send_to_char("They aren't here." NL, ch);
			return;
		}

		if ( IS_NPC(victim) || NOT_AUTHED(victim) || cloning_list.empty())
		{
			send_to_char("You failed." NL, ch);
			return;
		}

		room = victim->in_room;

		char_from_room(victim);
		char_to_room(victim, cloning_list.front()->cylinder);

		/* Trog: scinamy levele klonowi (by Pixel) */
		clone_cut(victim);

		send_to_char("Saving clone..." NL, ch);
		save_clone(victim);

		clone_restore(victim);

		char_from_room(victim);
		char_to_room(victim, room);

		send_to_char("Ok." NL, ch);
		return;
	}

	switch (ch->substate)
	{
	default:
		if (NOT_AUTHED(ch))
		{
			ch_printf(ch, "Dop�ki nie masz autoryzacji, nie jeste� pe�noprawnym cz�onkiem tego �wiata." NL);
			return;
		}

		if (IS_SET(ch->act, PLR_KILLER))
		{
			ch_printf(ch, "Nie starczy ci kredytek... Potrzebujesz %d." NL, ch->top_level + 2000000000 + number_range(-10000, 10000));
			return;
		}

		if ((cloning = get_cloning(ch->in_room)) == NULL)
		{
			ch_printf(ch, "Nie mo�esz zrobi� tego tutaj." NL);
			return;
		}

		amt = (ch->top_level < 51) ? ch->top_level * 199 : ch->top_level * 401;
		if (IS_HERO(ch))
			amt *= 2;

		if (ch->gold < amt)
		{
			ch_printf(ch, "Nie starczy ci kredytek... Potrzebujesz %d." NL, amt);
			return;
		}
		else
		{
			ch->gold -= amt;

			ch_printf(ch, "P�acisz %d kredytek za sklonowanie si�." NL, amt);
			ch_printf(ch, "Zostajesz odprowadzon%s do ma�ego pomieszczenia." NL, SEX_SUFFIX_YAE(ch));
		}

		char_from_room(ch);
		char_to_room(ch, cloning->cylinder);
		ch_printf(ch, NL
		"Pobrano ci z ramienia ma�� pr�bk� twojego kodu genetycznego." NL);
		add_timer(ch, TIMER_DO_FUN, 5, do_clone, 1);
		return;

	case SUB_TIMER_DO_ABORT:
		ch_printf(ch, "Jeste� teraz klonowan%s! Lepiej poczekaj do ko�ca zabiegu." NL, SEX_SUFFIX_YAE(ch));
		ch->substate = SUB_TIMER_CANT_ABORT;
		return;

	case 1:
		break;
	}
	ch->substate = SUB_NONE;

	if ((cloning = get_cloning_by_cyl(ch->in_room)) == NULL)
	{
		bug("HEEEEYYYY!!!! %s's cloning FAILED!", ch->name);
		ch->gold += amt;
		ch_printf(ch, "Nie uda�o si� ciebie sklonowa�. Twoje pieni�dze zosta�y zwr�cone." NL);
		return;
	}

	/* Trog: scinamy levele klonowi (by Pixel) */
	if (cloning->entrance->vnum != WAYLAND_CLONING) // Wyjatek na Wayland
	{
		clone_cut(ch);
		save_clone(ch);
		clone_restore(ch);
	}
	else
	{
		clone_cut_credits(ch);
		save_clone(ch);
		clone_restore_credits(ch);
	}

	char_from_room(ch);
	char_to_room(ch, cloning->leaving);
	do_look(ch, (char*) "");

	ch_printf(ch, NL FB_RED "Au�!" EOL NL);
	ch_printf(ch, "Zosta�%s� pomy�lnie sklonowan%s." NL, SEX_SUFFIX_EAE(ch), SEX_SUFFIX_YAE(ch));

	ch->hit--;
}

/*
 * A tutaj dopisze takie cos, zeby zapalnik w granatach mozna bylo
 * ustawic na okreslony czas. np 'ARM 120' uatawia zapalnik na 2 godziny
 * (MUDa)
 */
DEF_DO_FUN( arm )
{
	OBJ_DATA *obj;
	int time; /* by Lomion */

	if ( IS_NPC(ch) || !ch->pcdata || ch->pcdata->learned[gsn_grenades] <= 0)
	{
		ch_printf(ch, "Nie masz poj�cia jak to si� robi." NL);
		return;
	}

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		ch_printf(ch, "Ochronne pole si�owe w tym miejscu nie pozwala ci manipulowa� zapalnikiem." NL);
		return;
	}

	obj = get_eq_char(ch, WEAR_HOLD);

	if (!obj || obj->item_type != ITEM_GRENADE)
	{
		ch_printf(ch, "Nie trzymasz w r�ku granatu!" NL);
		return;
	}

	if (obj->value[2] > 0)
	{
		ch_printf(ch, "Ten granat jest UZBROJONY!!" NL);
		ch_printf(ch, "Wybuchnie za %dmin.!" NL, obj->value[2] * 60 / PULSE_TICK);
		return;
	}

	if (argument[0] == '\0')
	{
		ch_printf(ch, "Na ile minut ustawi� zapalnik granatu?" NL);
		return;
	}

	time = atoi(argument);

	if (time == 0)
	{
		ch_printf(ch, "Na ile minut ustawi� zapalnik granatu?" NL);
		return;
	}

//    obj->timer = 1; (by Lomion)
	time = time * PULSE_TICK / 60;
	obj->value[2] = time;

	STRDUP(obj->armed_by, ch->name);

	if (!IS_NPC(ch) && ch->pcdata)
		ch->pcdata->hotel_safe_time = 0;

	ch_printf(ch, "Uzbrajasz %s." NL, obj->przypadki[3]);
	act( PLAIN, "$n uzbraja $p$3.", ch, obj, NULL, TO_ROOM);

	learn_from_success(ch, gsn_grenades);
}

DEF_DO_FUN( ammo )
{
	OBJ_DATA *wield;
	OBJ_DATA *obj;
	bool checkammo = false;
	int charge = 0;

	obj = NULL;
	wield = get_eq_char(ch, WEAR_WIELD);
	if (wield)
	{
		obj = get_eq_char(ch, WEAR_DUAL_WIELD);
		if (!obj)
			obj = get_eq_char(ch, WEAR_HOLD);
	}
	else
	{
		wield = get_eq_char(ch, WEAR_HOLD);
		obj = NULL;
	}

	if (!wield || wield->item_type != ITEM_WEAPON)
	{
		send_to_char("Chyba nie masz �adnej broni w r�ku." NL, ch);
		return;
	}

	if (wield->value[3] == WEAPON_BLASTER)
	{

		if (obj && obj->item_type != ITEM_AMMO)
		{
			send_to_char("Masz zbyt pe�ne r�ce by prze�adowa� blaster." NL, ch);
			return;
		}

		if (obj)
		{
			if (obj->value[0] > wield->value[5])
			{
				send_to_char("Ten magazynek jest za du�y dla twojego blastera." NL, ch);
				return;
			}
			unequip_char(ch, obj);
			checkammo = true;
			charge = obj->value[0];
			separate_obj(obj);
			extract_obj(obj);
		}
		else
		{
			for (auto* obj : std::ranges::reverse_view(ch->carrying))
			{
				if (obj->item_type == ITEM_AMMO)
				{
					if (obj->value[0] > wield->value[5])
					{
						send_to_char("Ten magazynek jest za du�y dla twojego blastera." NL, ch);
						continue;
					}
					checkammo = true;
					charge = obj->value[0];
					separate_obj(obj);
					extract_obj(obj);
					break;
				}
			}
		}

		if (!checkammo)
		{
			send_to_char("Nie masz �adnej amunicji, nici z prze�adowania." NL, ch);
			return;
		}

		ch_printf(ch, "*Click* Prze�adowujesz blaster." NL
		"Masz teraz %d pocisk�w przy wysokiej mocy, wi�c %d na niskiej." NL, charge / 5, charge);
		act( PLAIN, "*Click*! $n prze�adowuje $p$3.", ch, wield, NULL, TO_ROOM);

	}
	else if (wield->value[3] == WEAPON_BOWCASTER)
	{

		if (obj && obj->item_type != ITEM_BOLT)
		{
			send_to_char("Masz zbyt pe�ne r�ce �eby to zrobi�." NL, ch);
			return;
		}

		if (obj)
		{
			if (obj->value[0] > wield->value[5])
			{
				send_to_char("Ten magazynek jest za du�y dla twojej kuszy." NL, ch);
				return;
			}
			unequip_char(ch, obj);
			checkammo = true;
			charge = obj->value[0];
			separate_obj(obj);
			extract_obj(obj);
		}
		else
		{
			for (auto* obj : std::ranges::reverse_view(ch->carrying))
			{
				if (obj->item_type == ITEM_BOLT)
				{
					if (obj->value[0] > wield->value[5])
					{
						send_to_char("Ten magazynek jest za du�y dla twojej kuszy." NL, ch);
						continue;
					}
					checkammo = true;
					charge = obj->value[0];
					separate_obj(obj);
					extract_obj(obj);
					break;
				}
			}
		}

		if (!checkammo)
		{
			send_to_char("&RNie masz �adnych be�t�w kt�rymi mo�na za�adowa� kusz�.\n\r&w", ch);
			return;
		}

		ch_printf(ch, "Wymieniasz magazynek.\n\rTwoja kusza posiada %d �adunk�w energetycznych.\n\r", charge);
		act( PLAIN, "$n wymienia magazynek w $p$5.", ch, wield, NULL, TO_ROOM);

	}
	else
	{

		if (obj && obj->item_type != ITEM_BATTERY)
		{
			send_to_char("&RMasz pe�ne r�ce, nie dasz rady wymieni� baterii.\n\r&w", ch);
			return;
		}

		if (obj)
		{
			unequip_char(ch, obj);
			checkammo = true;
			charge = obj->value[0];
			separate_obj(obj);
			extract_obj(obj);
		}
		else
		{
			for (auto* obj : std::ranges::reverse_view(ch->carrying))
			{
				if (obj->item_type == ITEM_BATTERY)
				{
					checkammo = true;
					charge = obj->value[0];
					separate_obj(obj);
					extract_obj(obj);
					break;
				}
			}
		}

		if (!checkammo)
		{
			send_to_char("&RNie masz baterii.\n\r&w", ch);
			return;
		}

		if (wield->value[3] == WEAPON_LIGHTSABER)
		{
			ch_printf(ch, "Wymieniasz bateri�.\n\rTw�j miecz �wietlny ma teraz %d/%d jednostek.\n\r", charge, charge);
			act( PLAIN, "$n wymienia bateri� w $p$5.", ch, wield, NULL,
			TO_ROOM);
			act( PLAIN, "$p rozb�yska jasnym blaskiem.", ch, wield, NULL,
			TO_ROOM);
		}
		else if (wield->value[3] == WEAPON_VIBRO_BLADE)
		{
			ch_printf(ch, "Wymieniasz bateri�.\n\rTwoje vibro-ostrze jest ma teraz %d/%d jednostek.\n\r", charge, charge);
			act( PLAIN, "$n wymienia bateri� w $p$5.", ch, wield, NULL,
			TO_ROOM);
		}
		else if (wield->value[3] == WEAPON_FORCE_PIKE)
		{
			ch_printf(ch, "Wymieniasz bateri�.\n\rTwoja force-pike ma teraz %d/%d jednostek.\n\r", charge, charge);
			act( PLAIN, "$n wymienia bateri� w $p$5.", ch, wield, NULL,
			TO_ROOM);
		}
		else
		{
			ch_printf(ch, "Czujesz si� bardzo g�upio.\n\r");
			act( PLAIN, "$n pr�buje wepchn�� bateri� w $p$3.", ch, wield, NULL,
			TO_ROOM);
		}
	}

	wield->value[4] = charge;

}

DEF_DO_FUN( setblaster )
{
	OBJ_DATA *wield;
	OBJ_DATA *wield2;

	wield = get_eq_char(ch, WEAR_WIELD);
	if (wield && !(wield->item_type == ITEM_WEAPON && wield->value[3] == WEAPON_BLASTER))
		wield = NULL;
	wield2 = get_eq_char(ch, WEAR_DUAL_WIELD);
	if (wield2 && !(wield2->item_type == ITEM_WEAPON && wield2->value[3] == WEAPON_BLASTER))
		wield2 = NULL;

	if (!wield && !wield2)
	{
		send_to_char("Nie dzier�ysz �adnego blastera." NL, ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char("Sk�adnia: setblaster <full|high|normal|half|low|stun>" NL, ch);
		return;
	}

	if (wield)
		act( PLAIN, "$n" PLAIN " zmienia ustawienie $p$1" PLAIN ".", ch, wield, NULL, TO_ROOM);

	if (wield2)
		act( PLAIN, "$n" PLAIN " zmienia ustawienie $p$1" PLAIN ".", ch, wield2, NULL, TO_ROOM);

	if (!str_cmp(argument, "full"))
	{
		if (wield)
		{
			wield->blaster_setting = BLASTER_FULL;
			send_to_char("Blaster ustawiony na PE�N� moc." NL, ch);
		}
		if (wield2)
		{
			wield2->blaster_setting = BLASTER_FULL;
			send_to_char("Tw�j drugi blaster ustawiony zosta� na PE�N� moc." NL, ch);
		}
		return;
	}
	if (!str_cmp(argument, "high"))
	{
		if (wield)
		{
			wield->blaster_setting = BLASTER_HIGH;
			send_to_char("Blaster ustawiony na WYSOK� moc." NL, ch);
		}
		if (wield2)
		{
			wield2->blaster_setting = BLASTER_HIGH;
			send_to_char("Tw�j drugi blaster ustawiony zosta� na WYSOK� moc." NL, ch);
		}
		return;
	}
	if (!str_cmp(argument, "normal"))
	{
		if (wield)
		{
			wield->blaster_setting = BLASTER_NORMAL;
			send_to_char("Blaster ustawiony na NORMALN� moc." NL, ch);
		}
		if (wield2)
		{
			wield2->blaster_setting = BLASTER_NORMAL;
			send_to_char("Tw�j drugi blaster ustawiony zosta� na NORMALN� moc." NL, ch);
		}
		return;
	}
	if (!str_cmp(argument, "half"))
	{
		if (wield)
		{
			wield->blaster_setting = BLASTER_HALF;
			send_to_char("Blaster ustawiony na PO�OW� mocy." NL, ch);
		}
		if (wield2)
		{
			wield2->blaster_setting = BLASTER_HALF;
			send_to_char("Tw�j drugi blaster ustawiony zosta� na PO�OW� mocy." NL, ch);
		}
		return;
	}
	if (!str_cmp(argument, "low"))
	{
		if (wield)
		{
			wield->blaster_setting = BLASTER_LOW;
			send_to_char("Blaster ustawiony na NISK� moc." NL, ch);
		}
		if (wield2)
		{
			wield2->blaster_setting = BLASTER_LOW;
			send_to_char("Tw�j drugi blaster ustawiony zosta� na NISK� moc." NL, ch);
		}
		return;
	}
	if (!str_cmp(argument, "stun"))
	{
		if (wield)
		{
			wield->blaster_setting = BLASTER_STUN;
			send_to_char("Blaster ustwiony na OG�USZANIE." NL, ch);
		}
		if (wield2)
		{
			wield2->blaster_setting = BLASTER_STUN;
			send_to_char("Tw�j drugi blaster zosta� ustawiony na OG�USZANIE." NL, ch);
		}
		return;
	}
	else
		do_setblaster(ch, (char*) "");

}

DEF_DO_FUN( use )
{
	char arg[MAX_INPUT_LENGTH];
	char argd[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	OBJ_DATA *device;
	OBJ_DATA *obj;
	ch_ret retcode;

	argument = one_argument(argument, argd);
	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "on"))
		argument = one_argument(argument, arg);

	if (argd[0] == '\0')
	{
		send_to_char("U�y� czego?" NL, ch);
		return;
	}

	if ((device = get_eq_char(ch, WEAR_HOLD)) == NULL || !nifty_is_name(argd, device->name))
	{
		do_takedrug(ch, argd);
		return;
	}

	if (device->item_type == ITEM_SPICE)
	{
		do_takedrug(ch, argd);
		return;
	}

	if (device->item_type != ITEM_DEVICE)
	{
		send_to_char("Nie masz poj�cia co z tym zrobi�." NL, ch);
		return;
	}

	if (device->value[2] <= 0)
	{
		ch_printf(ch, "%s %s" PLAIN " nie ma%s ju� �adunk�w.",
				device->gender == GENDER_NEUTRAL ? "To" : device->gender == GENDER_MALE ? "Ten" :
				device->gender == GENDER_FEMALE ? "Ta" : "Te", device->przypadki[0], device->gender == GENDER_PLURAL ? "j�" : "");
		return;
	}

	obj = NULL;
	if (arg[0] == '\0')
	{
		if (ch->fighting)
		{
			victim = who_fighting(ch);
		}
		else
		{
			send_to_char("U�y� na kim, lub na czym?" NL, ch);
			return;
		}
	}
	else
	{
		if ((victim = get_char_room(ch, arg)) == NULL && (obj = get_obj_here(ch, arg)) == NULL)
		{
			send_to_char("Nie widzisz tu niczego ani nikogo takiego." NL, ch);
			return;
		}
	}

	/* Thanos: requirements */
	if (!req_check(ch, obj))
		return;

	WAIT_STATE(ch, 1 * PULSE_VIOLENCE);

	if (device->value[2] > 0)
	{
		device->value[2]--;
		if (victim)
		{
			if (!oprog_use_trigger(ch, device, victim, NULL, NULL))
			{
				act( PLAIN, "$n" PLAIN " u�ywa $p$1" PLAIN " na $N$4" PLAIN ".", ch, device, victim, TO_ROOM);
				act( PLAIN, "U�ywasz $p$1" PLAIN " na $N$4" PLAIN ".", ch, device, victim, TO_CHAR);
			}
		}
		else
		{
			if (!oprog_use_trigger(ch, device, NULL, obj, NULL))
			{
				act( PLAIN, "$n" PLAIN " u�ywa $p$1" PLAIN " na $P$4" PLAIN ".", ch, device, obj, TO_ROOM);
				act( PLAIN, "U�ywasz $p$1" PLAIN " na $P$4" PLAIN ".", ch, device, obj, TO_CHAR);
			}
		}

		retcode = obj_cast_spell(device->value[3], device->value[0], ch, victim, obj);
		if (retcode == rCHAR_DIED || retcode == rBOTH_DIED)
		{
			bug("do_use: char died", 0);
			return;
		}
	}

	return;
}

DEF_DO_FUN( takedrug )
{
	OBJ_DATA *obj;
	AFFECT_DATA af;
	int drug;
	int sn;
	char buf[MSL];

	if (argument[0] == '\0' || !str_cmp(argument, ""))
	{
		send_to_char("Wzi�� co?" NL, ch);
		return;
	}

	if ((obj = find_obj(ch, argument, true)) == NULL)
		return;

	if (obj->item_type == ITEM_DEVICE)
	{
		send_to_char("Spr�buj najpierw wzi�� to do r�ki." NL, ch);
		return;
	}

	if (obj->item_type != ITEM_SPICE)
	{
		act( COL_ACTION, "$n patrzy na $p$3 drapi�c si� w g�ow�.", ch, obj, NULL, TO_ROOM);
		act( COL_ACTION, "Nie bardzo wiesz co zrobi� z $p$4.", ch, obj, NULL,
		TO_CHAR);
		return;
	}

	separate_obj(obj);
	if (obj->in_obj)
	{
		act( PLAIN, "Wyci�gasz $p$3 z $P$1.", ch, obj, obj->in_obj, TO_CHAR);
		act( PLAIN, "$n wyci�ga $p$3 z $P$1.", ch, obj, obj->in_obj, TO_ROOM);
	}

	if (ch->fighting && number_percent() > (get_curr_dex(ch) * 2 + 48))
	{
		act( COL_FORCE, "$n niechc�co upuszcza $p$3 niszcz�c to.", ch, obj, NULL, TO_ROOM);
		act( COL_FORCE, "Uups... $p wypada ci z r�k i psuje si�!", ch, obj, NULL, TO_CHAR);
	}
	else
	{
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			act( COL_ACTION, "$n bierze $p$3.", ch, obj, NULL, TO_ROOM);
			act( COL_ACTION, "Bierzesz $p$3.", ch, obj, NULL, TO_CHAR);
		}

		if (IS_NPC(ch))
		{
			extract_obj(obj);
			return;
		}

		drug = obj->value[0];

		WAIT_STATE(ch, PULSE_PER_SECOND/4);

		gain_condition(ch, COND_THIRST, 1);

		ch->pcdata->drug_level[drug] = UMIN(ch->pcdata->drug_level[drug] + obj->value[1], 255);
		if (ch->pcdata->drug_level[drug] >= 255 || ch->pcdata->drug_level[drug] > (ch->pcdata->addiction[drug] + 100))
		{
			act( COL_POISON, "$n krzywi si� i pluje na ziemi�.", ch, NULL, NULL,
			TO_ROOM);
			sprintf(buf, "Czujesz si� bardzo �le. Chyba wzi�%s� za du�o.", SEX_SUFFIX_EAE(ch));
			act( COL_POISON, buf, ch, NULL, NULL, TO_CHAR);
			ch->mental_state = URANGE(20, ch->mental_state + 5, 100);
			af.type = gsn_poison;
			af.location = APPLY_INT;
			af.modifier = -5;
			af.duration = ch->pcdata->drug_level[drug];
			af.bitvector = AFF_POISON;
			affect_to_char(ch, &af);
			ch->hit = 1;
		}

		switch (drug)
		{
		default:
		case SPICE_GLITTERSTIM:

			sn = skill_lookup("true sight");
			if (sn < MAX_SKILL && !IS_AFFECTED(ch, AFF_TRUESIGHT))
			{
				af.type = sn;
				af.location = APPLY_AC;
				af.modifier = -10;
				af.duration = URANGE(1, ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug], obj->value[1]);
				af.bitvector = AFF_TRUESIGHT;
				affect_to_char(ch, &af);
			}
			break;

		case SPICE_CARSANUM:

			sn = skill_lookup("sanctuary");
			if (sn < MAX_SKILL && !IS_AFFECTED(ch, AFF_SANCTUARY))
			{
				af.type = sn;
				af.location = APPLY_NONE;
				af.modifier = 0;
				af.duration = URANGE(1, ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug], obj->value[1]);
				af.bitvector = AFF_SANCTUARY;
				affect_to_char(ch, &af);
			}
			break;

		case SPICE_RYLL:

			af.type = -1;
			af.location = APPLY_DEX;
			af.modifier = 1;
			af.duration = URANGE(1, 2 * (ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]), 2 * obj->value[1]);
			af.bitvector = AFF_NONE;
			affect_to_char(ch, &af);

			af.type = -1;
			af.location = APPLY_HITROLL;
			af.modifier = 1;
			af.duration = URANGE(1, 2 * (ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]), 2 * obj->value[1]);
			af.bitvector = AFF_NONE;
			affect_to_char(ch, &af);

			break;

		case SPICE_ANDRIS:

			af.type = -1;
			af.location = APPLY_HIT;
			af.modifier = 10;
			af.duration = URANGE(1, 2 * (ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]), 2 * obj->value[1]);
			af.bitvector = AFF_NONE;
			affect_to_char(ch, &af);

			af.type = -1;
			af.location = APPLY_CON;
			af.modifier = 1;
			af.duration = URANGE(1, 2 * (ch->pcdata->drug_level[drug] - ch->pcdata->addiction[drug]), 2 * obj->value[1]);
			af.bitvector = AFF_NONE;
			affect_to_char(ch, &af);

			break;

		}

	}
	if (cur_obj == obj->serial)
		global_objcode = rOBJ_EATEN;
	extract_obj(obj);
	return;
}

void jedi_bonus(CHAR_DATA *ch)
{
	if (number_range(1, 100) == 1)
	{
		ch->max_mana++;
		send_to_char(FG_GREEN "Coraz lepiej pos�ugujesz si� Moc�." EOL, ch);
		send_to_char(
		FG_GREEN "Czujesz, �e twoja wiedza o Mocy zwi�ksza si�." EOL, ch);
	}
}

void sith_penalty(CHAR_DATA *ch)
{
	if (number_range(1, 100) == 1)
	{
		ch->max_mana++;
		if (ch->max_hit > 100)
			ch->max_hit--;
		ch->hit--;
		send_to_char(
		FG_RED "Twoje cia�o s�abnie wraz ze wzrostem si�y Ciemnej Strony Mocy." EOL, ch);
	}
}

/*
 * Fill a container
 * Many enhancements added by Thoric (ie: filling non-drink containers)
 */
DEF_DO_FUN( fill )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	OBJ_DATA *source;
	int dest_item, src_item1, src_item2, src_item3, src_item4;
	int diff = 0;
	bool all = false;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	/* munch optional words */
	if ((!str_cmp(arg2, "from") || !str_cmp(arg2, "with")) && argument[0] != '\0')
		argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Nape�ni� co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if ((obj = get_obj_carry(ch, arg1)) == NULL)
	{
		send_to_char("Nie masz takiego przedmiotu." NL, ch);
		return;
	}
	else
		dest_item = obj->item_type;

	src_item1 = src_item2 = src_item3 = src_item4 = -1;
	switch (dest_item)
	{
	default:
		act( COL_ACTION, "$n pr�buje nape�ni� $p$3... (Nie pytaj jak)", ch, obj, NULL, TO_ROOM);
		send_to_char("Nie mo�esz tego nape�ni�!" NL, ch);
		return;
		/* place all fillable item types here */
	case ITEM_DRINK_CON:
		src_item1 = ITEM_FOUNTAIN;
		break;
	case ITEM_HERB_CON:
		src_item1 = ITEM_HERB;
		src_item2 = ITEM_HERB_CON;
		break;
	case ITEM_PIPE:
		src_item1 = ITEM_HERB;
		src_item2 = ITEM_HERB_CON;
		break;
	case ITEM_CONTAINER:
		src_item1 = ITEM_CONTAINER;
		src_item2 = ITEM_CORPSE_NPC;
		src_item3 = ITEM_CORPSE_PC;
		src_item4 = ITEM_CORPSE_NPC;
		break;
	}

	if (dest_item == ITEM_CONTAINER)
	{
		if (IS_SET(obj->value[1], CONT_CLOSED))
		{
			if (obj->gender == GENDER_MALE)
				act( PLAIN, "$p jest zamkni�ty.", ch, obj, NULL, TO_CHAR);
			else if (obj->gender == GENDER_FEMALE)
				act( PLAIN, "$p jest zamkni�ta.", ch, obj, NULL, TO_CHAR);
			else if (obj->gender == GENDER_NEUTRAL)
				act( PLAIN, "$p jest zamkni�te.", ch, obj, NULL, TO_CHAR);
			else
				act( PLAIN, "$p s� zamkni�te.", ch, obj, NULL, TO_CHAR);
			return;
		}
		if (get_obj_weight(obj) / obj->count >= obj->value[0])
		{
			send_to_char("Bardziej ju� tego nie nape�nisz." NL, ch);
			return;
		}
	}
	else
	{
		diff = obj->value[0] - obj->value[1];
		if (diff < 1 || obj->value[1] >= obj->value[0])
		{
			send_to_char("Bardziej ju� tego nie nape�nisz." NL, ch);
			return;
		}
	}

	if (dest_item == ITEM_PIPE && IS_SET(obj->value[3], PIPE_FULLOFASH))
	{
		ch_printf(ch, "W %s pe�no jest popio�u. Najpierw %s oczy��." NL, obj->przypadki[5],
				obj->gender == GENDER_MALE ? "go" : obj->gender == GENDER_FEMALE ? "j�" : "je");
		return;
	}

	if (arg2[0] != '\0')
	{
		if (dest_item == ITEM_CONTAINER && (!str_cmp(arg2, "all") || !str_prefix("all.", arg2)))
		{
			all = true;
			source = NULL;
		}
		else
		/* This used to let you fill a pipe from an object on the ground.  Seems
		 to me you should be holding whatever you want to fill a pipe with.
		 It's nitpicking, but I needed to change it to get a mobprog to work
		 right.  Check out Lord Fitzgibbon if you're curious.  -Narn */
		if (dest_item == ITEM_PIPE)
		{

			if ((source = get_obj_carry(ch, arg2)) == NULL)
			{
				send_to_char("Nie masz takiego przedmiotu." NL, ch);
				return;
			}
			if (source->item_type != src_item1 && source->item_type != src_item2 && source->item_type != src_item3
					&& source->item_type != src_item4)
			{
				act( PLAIN, "Nie mo�esz nape�ni� $p$1 $P$4!", ch, obj, source,
				TO_CHAR);
				return;
			}
		}
		else
		{
			if ((source = get_obj_here(ch, arg2)) == NULL)
			{
				send_to_char("Nie masz takiego przedmiotu." NL, ch);
				return;
			}
		}
	}
	else
		source = NULL;

	if (!source && dest_item == ITEM_PIPE)
	{
		send_to_char("Czym chcesz to nape�ni�?" NL, ch);
		return;
	}

	if (!source)
	{
		bool found = false;

		found = false;
		separate_obj(obj);
		auto contents_snap = ch->in_room->contents;
		for (auto* source : contents_snap)
		{

			if (dest_item == ITEM_CONTAINER)
			{
				if (!CAN_WEAR(source, ITEM_TAKE) || (IS_OBJ_STAT(source, ITEM_PROTOTYPE) && !can_take_proto(ch))
						|| ch->carry_weight + get_obj_weight(source) > can_carry_w(ch)
						|| (get_obj_weight(source) + get_obj_weight(obj) / obj->count) > obj->value[0])
					continue;
				if (all && arg2[3] == '.' && !nifty_is_name(&arg2[4], source->name))
					continue;
				obj_from_room(source);

				if (source->item_type == ITEM_MONEY)
				{
					ch->gold += source->value[0];
					extract_obj(source);
				}
				else
					obj_to_obj(source, obj);
				found = true;
			}
			else if (source->item_type == src_item1 || source->item_type == src_item2 || source->item_type == src_item3
					|| source->item_type == src_item4)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			switch (src_item1)
			{
			default:
				send_to_char("Nie ma tu nic pasuj�cego." NL, ch);
				return;
			case ITEM_FOUNTAIN:
				send_to_char("Nie ma tu ani fontanny, ani basenu." NL, ch);
				return;
			case ITEM_HERB_CON:
				send_to_char("Nie widzisz tu �adnych zi�." NL, ch);
				return;
			case ITEM_HERB:
				send_to_char("Nie ma tu �adnych zi� do palenia." NL, ch);
				return;
			}
		}
		if (dest_item == ITEM_CONTAINER)
		{
			act( COL_ACTION, "Nape�niasz $p$3.", ch, obj, NULL, TO_CHAR);
			act( COL_ACTION, "$n nape�nia $p$3.", ch, obj, NULL, TO_ROOM);
			return;
		}
	}

	if (dest_item == ITEM_CONTAINER)
	{
		char name[MAX_INPUT_LENGTH];
		char *pd;
		bool found = false;

		if (source == obj)
		{
			send_to_char("Nie mo�esz nape�ni� przedmiotu nim samym!" NL, ch);
			return;
		}

		/* Pixel: personale przy operacji "fill"  */
		if ( IS_OBJ_STAT( source, ITEM_PERSONAL) && !IS_NPC(ch))
		{
			if (source->owner_name[0] == '\0')
				obj_personalize(ch, source);

			if (str_cmp(source->owner_name, ch->name))
			{
				send_to_char("Nie mo�esz tego zrobi�, to nie tw�j przedmiot!" NL, ch);
				return;
			}
		}

// && *source->owner_name && !str_cmp( source->owner_name, ch->name ) && !IS_NPC( ch ) )

		switch (source->item_type)
		{
		default: /* put something in container */
			if (!source->in_room /* disallow inventory items */
			|| !CAN_WEAR(source, ITEM_TAKE) || (IS_OBJ_STAT(source, ITEM_PROTOTYPE) && !can_take_proto(ch))
					|| ch->carry_weight + get_obj_weight(source) > can_carry_w(ch)
					|| (get_obj_weight(source) + get_obj_weight(obj) / obj->count) > obj->value[0])
			{
				send_to_char("Nie mo�esz tego zrobi�." NL, ch);
				return;
			}
			separate_obj(obj);
			act( COL_ACTION, "Bierzesz do r�ki $P$3 i wk�adasz do $p$3.", ch, obj, source, TO_CHAR);
			act( COL_ACTION, "$n bierze do r�ki $P$3 i wk�ada do $p$3.", ch, obj, source, TO_ROOM);
			obj_from_room(source);
			obj_to_obj(source, obj);
			break;
		case ITEM_MONEY:
			send_to_char("Nie mo�esz tego zrobi�... jeszcze nie." NL, ch);
			break;
		case ITEM_CORPSE_PC:
			if (IS_NPC(ch))
			{
				send_to_char("Nie mo�esz tego zrobi�." NL, ch);
				return;
			}

			pd = source->przypadki[0];
			pd = one_argument(pd, name);
			pd = one_argument(pd, name);
			pd = one_argument(pd, name);
			pd = one_argument(pd, name);

			if (str_cmp(name, ch->name) && !IS_IMMORTAL(ch))
			{
				bool fGroup;

				fGroup = false;
				for (auto* gch : char_list)
				{
					if (!IS_NPC(gch) && is_same_group(ch, gch) && !str_cmp(name, gch->name))
					{
						fGroup = true;
						break;
					}
				}
				if (!fGroup)
				{
					send_to_char("To nie twoje cia�o!" NL, ch);
					return;
				}
			}

		case ITEM_CONTAINER:
			if (source->item_type == ITEM_CONTAINER /* don't remove */
			&& IS_SET(source->value[1], CONT_CLOSED))
			{
				act( PLAIN, "$p jest zamkni�ty.", ch, source, NULL, TO_CHAR);
				return;
			}
		case ITEM_DROID_CORPSE:
		case ITEM_CORPSE_NPC:
			if (source->contents.empty())
			{
				send_to_char("To jest puste." NL, ch);
				return;
			}
			separate_obj(obj);
			auto src_contents_snap = source->contents;
			for (auto* otmp : src_contents_snap)
			{

				if (!CAN_WEAR(otmp, ITEM_TAKE) || (IS_OBJ_STAT(otmp, ITEM_PROTOTYPE) && !can_take_proto(ch))
						|| ch->carry_number + otmp->count > can_carry_n(ch) || ch->carry_weight + get_obj_weight(otmp) > can_carry_w(ch)
						|| (get_obj_weight(source) + get_obj_weight(obj) / obj->count) > obj->value[0])
					continue;
				obj_from_obj(otmp);
				obj_to_obj(otmp, obj);
				found = true;
			}
			if (found)
			{
				act( COL_ACTION, "Wypa�niasz $p$3 zawarto�ci� $P$1.", ch, obj, source, TO_CHAR);
				act( COL_ACTION, "$n wype�nia $p$3 zawarto�ci� $P$1.", ch, obj, source, TO_ROOM);
			}
			else
				send_to_char("Nie ma tu nic pasuj�cego." NL, ch);
			break;
		}
		return;
	}

	if (source->value[1] < 1)
	{
		send_to_char("Nic ju� nie zosta�o!" NL, ch);
		return;
	}
	if (source->count > 1 && source->item_type != ITEM_FOUNTAIN)
		separate_obj(source);
	separate_obj(obj);

	switch (source->item_type)
	{
	default:
		bug("do_fill: got bad item type: %d", source->item_type);
		send_to_char("Co� posz�o nie tak..." NL, ch);
		return;
	case ITEM_FOUNTAIN:
		if (obj->value[1] != 0 && obj->value[2] != 0)
		{
			send_to_char("W tym jest ju� inna ciecz." NL, ch);
			return;
		}
		obj->value[2] = 0;
		obj->value[1] = obj->value[0];

		/* jesli zrodlo bylo zatrute, nasz bidon tez musi byc...
		 Thanos, thx 2 Weebacca */
		if (source->value[3])
			obj->value[3] = source->value[3];

		act( COL_ACTION, "Nape�niasz $p$3 wod� z $P$3.", ch, obj, source,
		TO_CHAR);
		act( COL_ACTION, "$n nape�nia $p$3 wod� z $P$3.", ch, obj, source,
		TO_ROOM);
		return;
	case ITEM_HERB:
		if (obj->value[1] != 0 && obj->value[2] != source->value[2])
		{
			send_to_char("W tym s� ju� inne zio�a!" NL, ch);
			return;
		}
		obj->value[2] = source->value[2];
		if (source->value[1] < diff)
			diff = source->value[1];
		obj->value[1] += diff;
		act( COL_ACTION, "Nape�niasz $p$3 $P$4.", ch, obj, source, TO_CHAR);
		act( COL_ACTION, "$n nape�nia $p$3 $P$4.", ch, obj, source, TO_ROOM);
		if ((source->value[1] -= diff) < 1)
			extract_obj(source);
		return;
	case ITEM_HERB_CON:
		if (obj->value[1] != 0 && obj->value[2] != source->value[2])
		{
			send_to_char("W tym s� ju� inne zio�a." NL, ch);
			return;
		}
		obj->value[2] = source->value[2];
		if (source->value[1] < diff)
			diff = source->value[1];
		obj->value[1] += diff;
		source->value[1] -= diff;
		act( COL_ACTION, "Nape�niasz $p$3 zio�ami z $P$1.", ch, obj, source,
		TO_CHAR);
		act( COL_ACTION, "$n nape�nia $p$3 zio�ami z $P$1.", ch, obj, source,
		TO_ROOM);
		return;
	case ITEM_DRINK_CON:
		if (obj->value[1] != 0 && obj->value[2] != source->value[2])
		{
			send_to_char("W tym jest ju� inna ciecz." NL, ch);
			return;
		}
		obj->value[2] = source->value[2];
		if (source->value[1] < diff)
			diff = source->value[1];
		obj->value[1] += diff;
		source->value[1] -= diff;

		/* jesli zrodlo bylo zatrute, nasz bidon tez musi byc...
		 Thanos, thx 2 Weebacca */
		if (source->value[3])
			obj->value[3] = source->value[3];

		act( COL_ACTION, "Nape�niasz $p$3 zawarto�ci� $P$1.", ch, obj, source,
		TO_CHAR);
		act( COL_ACTION, "$n nape�nia $p$3 zawarto�ci� $P$1.", ch, obj, source,
		TO_ROOM);
		return;
	}
}

DEF_DO_FUN( drink )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	int amount;
	int liquid;

	argument = one_argument(argument, arg);
	/* munch optional words */
	if (!str_cmp(arg, "from") && !str_cmp(arg, "z") && argument[0] != '\0')
		argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		for (auto* obj : ch->in_room->contents)
			if ((obj->item_type == ITEM_FOUNTAIN))
				break;

		if (!obj)
		{
			send_to_char("Napi� si� czego?" NL, ch);
			return;
		}
	}
	else
	{
		if ((obj = get_obj_here(ch, arg)) == NULL)
		{
			send_to_char("Nie ma tu niczego takiego." NL, ch);
			return;
		}
	}

	if (obj->count > 1 && obj->item_type != ITEM_FOUNTAIN)
		separate_obj(obj);

	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 40)
	{
		send_to_char("Nie trafiasz sobie do ust.  *Hick*" NL, ch);
		return;
	}

	/* Thanos: requirements */
	if (!req_check(ch, obj))
		return;

	switch (obj->item_type)
	{
	default:
		if (obj->carried_by == ch)
		{
			act( COL_ACTION, "$n przyk�ada sobie $p$3 do ust i stara si� napi�...", ch, obj, NULL, TO_ROOM);
			act( COL_ACTION, "Przyk�adasz sobie $p$3 do ust i starasz si� napi�...", ch, obj, NULL, TO_CHAR);
		}
		else
		{
			act( COL_ACTION, "$n pada na ziemi� i stara si� napi� z $p$1... (Czy $e si� dobrze czuje?)", ch, obj, NULL, TO_ROOM);
			act( COL_ACTION, "Padasz na ziemi� i starasz si� napi� z $p$1...", ch, obj, NULL, TO_CHAR);
		}
		break;

	case ITEM_POTION:
		if (obj->carried_by == ch)
			do_quaff(ch, obj->name);
		else
			send_to_char("Nie masz takiego przedmiotu." NL, ch);
		break;

	case ITEM_FOUNTAIN:
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			act( COL_ACTION, "$n pije wod� z $p$1.", ch, obj, NULL, TO_ROOM);
			send_to_char("Bierzesz wielkiego orze�wiaj�cego �yka." NL, ch);
		}

		if (!IS_NPC(ch))
			ch->pcdata->condition[COND_THIRST] = COND_MAX;

		if (obj->value[3] >= 1)
		{
			/* The drink was poisoned! */
			AFFECT_DATA af;

			act( COL_POISON, "$n krzywi si� i pluje.", ch, NULL, NULL, TO_ROOM);
			act( COL_POISON, "Krzywisz si� i plujesz.", ch, NULL, NULL,
			TO_CHAR);
			ch->mental_state = URANGE(20, ch->mental_state + 5, 100);
			af.type = gsn_poison;
			af.duration = obj->value[3];
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_POISON;
			affect_join(ch, &af);
		}

		break;

	case ITEM_DRINK_CON:
		if (obj->value[1] <= 0)
		{
			send_to_char("To jest ju� puste." NL, ch);
			return;
		}

		if ((liquid = obj->value[2]) >= LIQ_MAX)
		{
			bug("Do_drink: bad liquid number %d.", liquid);
			liquid = obj->value[2] = 0;
		}

		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			act( COL_ACTION, "$n pije $T z $p$1.", ch, obj, liq_table[liquid].liq_name, TO_ROOM);
			act( COL_ACTION, "Pijesz $T z $p$1.", ch, obj, liq_table[liquid].liq_name, TO_CHAR);
		}

		amount = 1; /* UMIN(amount, obj->value[1]); */
		/* what was this? concentrated drinks?  concentrated water
		 too I suppose... sheesh! */

		gain_condition(ch, COND_DRUNK, amount * liq_table[liquid].liq_affect[COND_DRUNK]);
		gain_condition(ch, COND_FULL, amount * liq_table[liquid].liq_affect[COND_FULL]);
		gain_condition(ch, COND_THIRST, amount * liq_table[liquid].liq_affect[COND_THIRST]);

		if (!IS_NPC(ch))
		{
			if (ch->pcdata->condition[COND_DRUNK] > 24)
				send_to_char("Czujesz, �e kolana lekko ci si� uginaj� i nie bardzo wiesz co si� dzieje." NL, ch);
			else if (ch->pcdata->condition[COND_DRUNK] > 18)
				ch_printf(ch, "Jeste� nachlan%s jak bela." NL, SEX_SUFFIX_YAE(ch));
			else if (ch->pcdata->condition[COND_DRUNK] > 12)
				ch_printf(ch, "Jeste� pijan%s." NL, SEX_SUFFIX_YAE(ch));
			else if (ch->pcdata->condition[COND_DRUNK] > 8)
				send_to_char("Troch� kr�ci ci si� w g�owie." NL, ch);
			else if (ch->pcdata->condition[COND_DRUNK] > 5)
				send_to_char("Czujesz jak lekko szumi ci w g�owie." NL, ch);

			if (ch->pcdata->condition[COND_THIRST] >= COND_MAX)
				ch_printf(ch, "Jeste� pe�n%s. Wi�cej nie zmie�cisz." NL, SEX_SUFFIX_YAE(ch));
			else if (ch->pcdata->condition[COND_FULL] > COND_MAX / 2)
				ch_printf(ch, "Jeste� pe�n%s." NL, SEX_SUFFIX_YAE(ch));
			else if (ch->pcdata->condition[COND_THIRST] > COND_MAX / 4)
				send_to_char("Nie chce ci si� ju� pi�." NL, ch);
			else if (ch->pcdata->condition[COND_THIRST] > 0)
				ch_printf(ch, "Ju� nie jeste� spragnion%s." NL, SEX_SUFFIX_YAE(ch));
		}

		if (obj->value[3] >= 1)
		{
			/* The drink was poisoned! */
			AFFECT_DATA af;

			act( COL_POISON, "$n krzywi si� i pluje.", ch, NULL, NULL, TO_ROOM);
			act( COL_POISON, "Krzywisz si� i plujesz.", ch, NULL, NULL,
			TO_CHAR);
			ch->mental_state = URANGE(20, ch->mental_state + 5, 100);
			af.type = gsn_poison;
			af.duration = obj->value[3];
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_POISON;
			affect_join(ch, &af);
		}

		obj->value[1] -= amount;
		break;
	}
	WAIT_STATE(ch, PULSE_PER_SECOND);
	return;
}

DEF_DO_FUN( eat )
{
	OBJ_DATA *obj;
	ch_ret retcode;
	int foodcond;

	if (argument[0] == '\0')
	{
		send_to_char("Zje�� co?" NL, ch);
		return;
	}

	if ( IS_NPC(ch) || ch->pcdata->condition[COND_FULL] > 5)
		if (ms_find_obj(ch))
			return;

	if ((obj = find_obj(ch, argument, true)) == NULL)
		return;

	if (!IS_IMMORTAL(ch))
	{
		if (obj->item_type != ITEM_FOOD && obj->item_type != ITEM_PILL)
		{
			act( COL_ACTION, "$n patrzy ze smakiem na $p$3... (musi by� bardzo g�odn$y)", ch, obj, NULL, TO_ROOM);
			act( COL_ACTION, "Patrzysz ze smakiem na $p$3...", ch, obj, NULL,
			TO_CHAR);
			return;
		}

		if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] >= COND_MAX)
		{
			send_to_char("Tw�j brzuch nie zmie�ci ju� nic wi�cej." NL, ch);
			return;
		}
	}

	/* required due to object grouping */
	separate_obj(obj);

	/* Thanos: requirements */
	if (!req_check(ch, obj))
		return;

	WAIT_STATE(ch, PULSE_PER_SECOND/2);

	if (obj->in_obj)
	{
		act( PLAIN, "Wyci�gasz $p$3 z $P$1.", ch, obj, obj->in_obj, TO_CHAR);
		act( PLAIN, "$n wyci�ga $p$3 z $P$1.", ch, obj, obj->in_obj, TO_ROOM);
	}
	if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
	{
		if (!obj->action_desc || obj->action_desc[0] == '\0')
			act( COL_ACTION, "Zjadasz $p$3.", ch, obj, NULL, TO_CHAR);
		else
			actiondesc(ch, obj, NULL);

		act( COL_ACTION, "$n zjada $p$3.", ch, obj, NULL, TO_ROOM);
	}

	switch (obj->item_type)
	{

	case ITEM_FOOD:
		if (obj->timer > 0 && obj->value[1] > 0)
			foodcond = (obj->timer * 10) / obj->value[1];
		else
			foodcond = 10;

		if (!IS_NPC(ch))
		{
			int condition;

			condition = ch->pcdata->condition[COND_FULL];
			gain_condition(ch, COND_FULL, (obj->value[0] * foodcond) / 10);

			if (condition <= 1 && ch->pcdata->condition[COND_FULL] > 1)
				ch_printf(ch, "Ju� nie jeste� g�odn%s." NL, SEX_SUFFIX_YAE(ch));
			else if (ch->pcdata->condition[COND_FULL] > COND_MAX)
				ch_printf(ch, "Jeste� pe�n%s." NL, SEX_SUFFIX_YAE(ch));
		}

		if (obj->value[3] != 0 || (foodcond < 4 && number_range(0, foodcond + 1) == 0))
		{
			/* The food was poisoned! */
			AFFECT_DATA af;

			if (obj->value[3] != 0)
			{
				act( COL_POISON, "$n d�awi si� i pluje.", ch, NULL, NULL,
				TO_ROOM);
				act( COL_POISON, "D�awisz si� i plujesz.", ch, NULL, NULL,
				TO_CHAR);
				ch->mental_state = URANGE(20, ch->mental_state + 5, 100);
			}
			else
			{
				act( COL_POISON, "$n krztusi si� $p$4.", ch, obj, NULL,
				TO_ROOM);
				act( COL_POISON, "Krztusisz si� $p$4.", ch, obj, NULL, TO_CHAR);
				ch->mental_state = URANGE(15, ch->mental_state + 5, 100);
			}

			af.type = gsn_poison;
			af.duration = 2 * obj->value[0] * (obj->value[3] > 0 ? obj->value[3] : 1);
			af.location = APPLY_NONE;
			af.modifier = 0;
			af.bitvector = AFF_POISON;
			affect_join(ch, &af);
		}
		break;

	case ITEM_PILL:
		/* allow pills to fill you, if so desired */
		if (!IS_NPC(ch) && obj->value[4])
		{
			int condition;

			condition = ch->pcdata->condition[COND_FULL];
			gain_condition(ch, COND_FULL, obj->value[4]);
			if (condition <= 1 && ch->pcdata->condition[COND_FULL] > 1)
				ch_printf(ch, "Ju� nie jeste� g�odn%s." NL, SEX_SUFFIX_YAE(ch));
			else if (ch->pcdata->condition[COND_FULL] > COND_MAX)
				ch_printf(ch, "Jeste� pe�n%s." NL, SEX_SUFFIX_YAE(ch));
		}
		retcode = obj_cast_spell(obj->value[1], obj->value[0], ch, ch, NULL);
		if (retcode == rNONE)
			retcode = obj_cast_spell(obj->value[2], obj->value[0], ch, ch, NULL);
		if (retcode == rNONE)
			retcode = obj_cast_spell(obj->value[3], obj->value[0], ch, ch, NULL);
		break;
	}

	if (obj->serial == cur_obj)
		global_objcode = rOBJ_EATEN;
	extract_obj(obj);
	return;
}

DEF_DO_FUN( quaff )
{
	OBJ_DATA *obj;
	ch_ret retcode;

	if (argument[0] == '\0' || !str_cmp(argument, ""))
	{
		send_to_char("Po�kn�� co?" NL, ch);
		return;
	}

	if ((obj = find_obj(ch, argument, true)) == NULL)
		return;

	if (obj->item_type != ITEM_POTION)
	{
		if (obj->item_type == ITEM_DRINK_CON)
			do_drink(ch, obj->name);
		else
		{
			act( COL_ACTION, "$n przyk�ada sobie $p$3 do ust i stara si� pi�...", ch, obj, NULL, TO_ROOM);
			act( COL_ACTION, "Przyk�adasz sobie $p$3 do ust i starasz si� pi�...", ch, obj, NULL, TO_CHAR);
		}
		return;
	}

	/*
	 * Fullness checking					-Thoric
	 */
	if (!IS_NPC(ch) && (ch->pcdata->condition[COND_FULL] >= 11 * COND_MAX / 10 || ch->pcdata->condition[COND_THIRST] >= 11 * COND_MAX / 10))
	{
		send_to_char("Tw�j brzuch nie zmie�ci ju� nic wi�cej." NL, ch);
		return;
	}

	separate_obj(obj);
	/* Thanos: requirements */
	if (!req_check(ch, obj))
		return;

	if (obj->in_obj)
	{
		act( PLAIN, "Wyci�gasz $p$3 z $P$1.", ch, obj, obj->in_obj, TO_CHAR);
		act( PLAIN, "$n wyci�ga $p$3 z $P$1.", ch, obj, obj->in_obj, TO_ROOM);
	}

	/*
	 * If fighting, chance of dropping potion			-Thoric
	 */
	if (ch->fighting && number_percent() > (get_curr_dex(ch) * 2 + 48))
	{
		act( COL_FORCE, "$n niechc�c$y upuszcza $p$3 rozbijaj�c to na kawa�eczki.", ch, obj, NULL, TO_ROOM);
		act( COL_FORCE, "Uups... $p wypada ci z r�k i rozbija si� na kawa�eczki!", ch, obj, NULL, TO_CHAR);
	}
	else
	{
		if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
		{
			act( COL_ACTION, "$n po�yka $p$3.", ch, obj, NULL, TO_ROOM);
			act( COL_ACTION, "Po�ykasz $p$3.", ch, obj, NULL, TO_CHAR);
		}

		WAIT_STATE(ch, PULSE_PER_SECOND/4);

		gain_condition(ch, COND_THIRST, 1);
		retcode = obj_cast_spell(obj->value[1], obj->value[0], ch, ch, NULL);
		if (retcode == rNONE)
			retcode = obj_cast_spell(obj->value[2], obj->value[0], ch, ch, NULL);
		if (retcode == rNONE)
			retcode = obj_cast_spell(obj->value[3], obj->value[0], ch, ch, NULL);
	}
	if (cur_obj == obj->serial)
		global_objcode = rOBJ_QUAFFED;
	extract_obj(obj);
	return;
}

/*
 * Function to handle the state changing of a triggerobject (lever)  -Thoric
 */
void pullorpush(CHAR_DATA *ch, OBJ_DATA *obj, bool pull)
{
	char buf[MAX_STRING_LENGTH];
	bool isup;
	ROOM_INDEX_DATA *room, *to_room = 0;
	EXIT_DATA *pexit, *pexit_rev;
	int edir;
	const char *txt;

	/* Thanos: requirements */
	if (!req_check(ch, obj))
		return;

	if (IS_SET(obj->value[0], TRIG_UP))
		isup = true;
	else
		isup = false;
	switch (obj->item_type)
	{
	default:
		sprintf(buf, "Nie mo�esz tego %s!" NL, pull ? "wcisn��" : "wycisn��");
		send_to_char(buf, ch);
		return;
		break;
	case ITEM_SWITCH:
	case ITEM_LEVER:
	case ITEM_PULLCHAIN:
		if ((!pull && isup) || (pull && !isup))
		{
			sprintf(buf, "To jest ju� %s." NL, isup ? "w��czone" : "wy��czone");
			send_to_char(buf, ch);
			return;
		}
	case ITEM_BUTTON:
		if ((!pull && isup) || (pull & !isup))
		{
			sprintf(buf, "To jest ju� %s." NL, isup ? "wci�ni�te" : "wyci�ni�te");
			send_to_char(buf, ch);
			return;
		}
		break;
	}
	if ((pull) && IS_SET(obj->pIndexData->progtypes, PULL_PROG))
	{
		if (!IS_SET(obj->value[0], TRIG_AUTORETURN))
			REMOVE_BIT(obj->value[0], TRIG_UP);
		oprog_pull_trigger(ch, obj);
		return;
	}
	if ((!pull) && IS_SET(obj->pIndexData->progtypes, PUSH_PROG))
	{
		if (!IS_SET(obj->value[0], TRIG_AUTORETURN))
			SET_BIT(obj->value[0], TRIG_UP);
		oprog_push_trigger(ch, obj);
		return;
	}

	if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
	{
		sprintf(buf, "$n %s $p$3.", pull ? "wciska" : "wyciska");
		act( COL_ACTION, buf, ch, obj, NULL, TO_ROOM);
		sprintf(buf, "%s $p$3.", pull ? "Wciskasz" : "Wyciskasz");
		act( COL_ACTION, buf, ch, obj, NULL, TO_CHAR);
	}

	if (!IS_SET(obj->value[0], TRIG_AUTORETURN))
	{
		if (pull)
			REMOVE_BIT(obj->value[0], TRIG_UP);
		else
			SET_BIT(obj->value[0], TRIG_UP);
	}
	if ( IS_SET(obj->value[0],
			TRIG_TELEPORT) || IS_SET(obj->value[0], TRIG_TELEPORTALL) || IS_SET(obj->value[0], TRIG_TELEPORTPLUS))
	{
		int flags;

		if ((room = get_room_index(obj->value[1])) == NULL)
		{
			bug("PullOrPush: obj points to invalid room %d", obj->value[1]);
			return;
		}
		flags = 0;
		if (IS_SET(obj->value[0], TRIG_SHOWROOMDESC))
			SET_BIT(flags, TELE_SHOWDESC);
		if (IS_SET(obj->value[0], TRIG_TELEPORTALL))
			SET_BIT(flags, TELE_TRANSALL);
		if (IS_SET(obj->value[0], TRIG_TELEPORTPLUS))
			SET_BIT(flags, TELE_TRANSALLPLUS);

		teleport(ch, obj->value[1], flags);
		return;
	}

	if ( IS_SET(obj->value[0],
			TRIG_RAND4) || IS_SET(obj->value[0], TRIG_RAND6))
	{
		int maxd;

		if ((room = get_room_index(obj->value[1])) == NULL)
		{
			bug("PullOrPush: obj points to invalid room %d", obj->value[1]);
			return;
		}

		if (IS_SET(obj->value[0], TRIG_RAND4))
			maxd = 3;
		else
			maxd = 5;

		randomize_exits(room, maxd);
		for (auto* rch : room->people)
		{
			send_to_char("S�yszysz g�o�ne dudnienie." NL, rch);
			send_to_char("Co� si� chyba zmieni�o..." NL, rch);
		}
	}
	if (IS_SET(obj->value[0], TRIG_DOOR))
	{
		room = get_room_index(obj->value[1]);
		if (!room)
			room = obj->in_room;
		if (!room)
		{
			bug("PullOrPush: obj points to invalid room %d", obj->value[1]);
			return;
		}
		if (IS_SET(obj->value[0], TRIG_D_NORTH))
		{
			edir = DIR_NORTH;
			txt = "na p�nocy";
		}
		else if (IS_SET(obj->value[0], TRIG_D_SOUTH))
		{
			edir = DIR_SOUTH;
			txt = "na po�udniu";
		}
		else if (IS_SET(obj->value[0], TRIG_D_EAST))
		{
			edir = DIR_EAST;
			txt = "na wschodzie";
		}
		else if (IS_SET(obj->value[0], TRIG_D_WEST))
		{
			edir = DIR_WEST;
			txt = "na zachodzie";
		}
		else if (IS_SET(obj->value[0], TRIG_D_UP))
		{
			edir = DIR_UP;
			txt = "nad tob�";
		}
		else if (IS_SET(obj->value[0], TRIG_D_DOWN))
		{
			edir = DIR_DOWN;
			txt = "pod tob�";
		}
		else
		{
			bug("PullOrPush: door: no direction flag set.", 0);
			return;
		}
		pexit = get_exit(room, edir);
		if (!pexit)
		{
			if (!IS_SET(obj->value[0], TRIG_PASSAGE))
			{
				bug("PullOrPush: obj points to non-exit %d", obj->value[1]);
				return;
			}
			to_room = get_room_index(obj->value[2]);
			if (!to_room)
			{
				bug("PullOrPush: dest points to invalid room %d", obj->value[2]);
				return;
			}
			pexit = make_exit(room, to_room, edir);
			STRDUP(pexit->keyword, "");
			STRDUP(pexit->description, "");
			pexit->key = -1;
			pexit->flags = 0;
			top_exit++;
			act( PLAIN, "Przej�cie si� otwiera!", ch, NULL, NULL, TO_CHAR);
			act( PLAIN, "Przej�cie si� otwiera!", ch, NULL, NULL, TO_ROOM);
			return;
		}
		if ( IS_SET(obj->value[0],
				TRIG_UNLOCK) && IS_SET(pexit->flags, EX_LOCKED))
		{
			REMOVE_BIT(pexit->flags, EX_LOCKED);
			act( PLAIN, "S�yszysz g�uche klikni�cie $T.", ch, NULL, txt,
			TO_CHAR);
			act( PLAIN, "S�yszysz g�uche klikni�cie $T.", ch, NULL, txt,
			TO_ROOM);
			if ((pexit_rev = pexit->rexit) != NULL && pexit_rev->to_room == ch->in_room)
				REMOVE_BIT(pexit_rev->flags, EX_LOCKED);
			return;
		}
		if ( IS_SET(obj->value[0],
				TRIG_LOCK) && !IS_SET(pexit->flags, EX_LOCKED))
		{
			SET_BIT(pexit->flags, EX_LOCKED);
			act( PLAIN, "S�yszysz g�uche klikni�cie $T.", ch, NULL, txt,
			TO_CHAR);
			act( PLAIN, "S�yszysz g�uche klikni�cie $T.", ch, NULL, txt,
			TO_ROOM);
			if ((pexit_rev = pexit->rexit) != NULL && pexit_rev->to_room == ch->in_room)
				SET_BIT(pexit_rev->flags, EX_LOCKED);
			return;
		}
		if ( IS_SET(obj->value[0],
				TRIG_OPEN) && IS_SET(pexit->flags, EX_CLOSED))
		{
			REMOVE_BIT(pexit->flags, EX_CLOSED);
			for (auto* rch : room->people)
				act( COL_ACTION, "$d otwieraj� si�.", rch, NULL, pexit->keyword,
				TO_CHAR);
			if ((pexit_rev = pexit->rexit) != NULL && pexit_rev->to_room == ch->in_room)
			{
				REMOVE_BIT(pexit_rev->flags, EX_CLOSED);
				for (auto* rch : to_room->people)
					act( COL_ACTION, "$d otwieraj� si�.", rch, NULL, pexit_rev->keyword, TO_CHAR);
			}
			check_room_for_traps(ch, trap_door[edir]);
			return;
		}
		if ( IS_SET(obj->value[0],
				TRIG_CLOSE) && !IS_SET(pexit->flags, EX_CLOSED))
		{
			SET_BIT(pexit->flags, EX_CLOSED);
			for (auto* rch : room->people)
				act( COL_ACTION, "$d zamykaj� si�.", rch, NULL, pexit->keyword,
				TO_CHAR);
			if ((pexit_rev = pexit->rexit) != NULL && pexit_rev->to_room == ch->in_room)
			{
				SET_BIT(pexit_rev->flags, EX_CLOSED);
				for (auto* rch : to_room->people)
					act( COL_ACTION, "$d zamykaj� si�.", rch, NULL, pexit_rev->keyword, TO_CHAR);
			}
			check_room_for_traps(ch, trap_door[edir]);
			return;
		}
	}
}

DEF_DO_FUN( pull )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Poci�gn�� co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if ((obj = get_obj_here(ch, arg)) == NULL)
	{
		act( PLAIN, "Nie ma tu �adnego $T.", ch, NULL, arg, TO_CHAR);
		return;
	}

	pullorpush(ch, obj, true);
}

DEF_DO_FUN( push )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Wcisn�� co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if ((obj = get_obj_here(ch, arg)) == NULL)
	{
		act( PLAIN, "Nie ma tu �adnego $T.", ch, NULL, arg, TO_CHAR);
		return;
	}

	pullorpush(ch, obj, false);
}

/* pipe commands (light, tamp, smoke) by Thoric */
DEF_DO_FUN( tamp )
{
	OBJ_DATA *pipe;
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Nabi� co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if ((pipe = get_obj_carry(ch, arg)) == NULL)
	{
		send_to_char("Nie masz takiego przedmiotu." NL, ch);
		return;
	}
	if (pipe->item_type != ITEM_PIPE)
	{
		send_to_char("Nie mo�esz tego nabi�." NL, ch);
		return;
	}
	if (!IS_SET(pipe->value[3], PIPE_TAMPED))
	{
		act( COL_ACTION, "Delikatnie nabijasz $p$3.", ch, pipe, NULL, TO_CHAR);
		act( COL_ACTION, "$n delikatnie nabija $p$3.", ch, pipe, NULL, TO_ROOM);
		SET_BIT(pipe->value[3], PIPE_TAMPED);
		return;
	}
	send_to_char("Nie musisz tego nabija�." NL, ch);
}

DEF_DO_FUN( smoke )
{
	OBJ_DATA *pipe;
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Wypali� co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if ((pipe = get_obj_carry(ch, arg)) == NULL)
	{
		send_to_char("Nie masz takiego przedmiotu." NL, ch);
		return;
	}

	/* Thanos: requirements */
	if (!req_check(ch, pipe))
		return;

	if (pipe->item_type != ITEM_PIPE)
	{
		act( COL_ACTION, "Pr�bujesz pali� $p$3... ale jako� nic si� nie dzieje.", ch, pipe, NULL, TO_CHAR);
		act( COL_ACTION, "$n pr�buje pali� $p$3... (Ciekawe jak $e to nabi�$o?)", ch, pipe, NULL, TO_ROOM);
		return;
	}
	if (!IS_SET(pipe->value[3], PIPE_LIT))
	{
		act( COL_ACTION, "Pr�bujesz zaci�gn�� si� $p$4, ale to si� nie pali.", ch, pipe, NULL, TO_CHAR);
		act( COL_ACTION, "$n pr�buje zaci�gn�� si� $p$4, ale to si� nie pali.", ch, pipe, NULL, TO_ROOM);
		return;
	}
	if (pipe->value[1] > 0)
	{
		if (!oprog_use_trigger(ch, pipe, NULL, NULL, NULL))
		{
			act( COL_ACTION, "Bez opami�tania zaci�gasz si� $p$4.", ch, pipe, NULL, TO_CHAR);
			act( COL_ACTION, "$n bez opami�tania zaci�ga si� $p$4.", ch, pipe, NULL, TO_ROOM);
		}

		if ( IS_VALID_HERB( pipe->value[2] ) && pipe->value[2] < top_herb)
		{
			int sn = pipe->value[2] + TYPE_HERB;
			SKILLTYPE *skill = get_skilltype(sn);

			WAIT_STATE(ch, skill->beats);
			if (skill->spell_fun)
				obj_cast_spell(sn, UMIN(skill->min_level, ch->top_level), ch, ch, NULL);
			if (obj_extracted(pipe))
				return;
		}
		else
			bug("do_smoke: bad herb type %d", pipe->value[2]);

		SET_BIT(pipe->value[3], PIPE_HOT);
		if (--pipe->value[1] < 1)
		{
			REMOVE_BIT(pipe->value[3], PIPE_LIT);
			SET_BIT(pipe->value[3], PIPE_DIRTY);
			SET_BIT(pipe->value[3], PIPE_FULLOFASH);
		}
	}
}

DEF_DO_FUN( light )
{
	OBJ_DATA *pipe;
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Zapali� co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if ((pipe = get_obj_carry(ch, arg)) == NULL)
	{
		send_to_char("Nie masz takiego przedmiotu." NL, ch);
		return;
	}
	if (pipe->item_type != ITEM_PIPE)
	{
		send_to_char("Nie mo�esz tego zapali�." NL, ch);
		return;
	}
	if (!IS_SET(pipe->value[3], PIPE_LIT))
	{
		if (pipe->value[1] < 1)
		{
			act( COL_ACTION, "Pr�bujesz zapali� $p$3, ale to jest puste.", ch, pipe, NULL, TO_CHAR);
			act( COL_ACTION, "$n pr�buje zapali� $p$3, ale to jest puste.", ch, pipe, NULL, TO_ROOM);
			return;
		}
		act( COL_ACTION, "Ostro�nie przypalasz $p$3.", ch, pipe, NULL, TO_CHAR);
		act( COL_ACTION, "$n ostro�nie przypala $p$3.", ch, pipe, NULL,
		TO_ROOM);
		SET_BIT(pipe->value[3], PIPE_LIT);
		return;
	}
	send_to_char("To ju� si� pali." NL, ch);
}

DEF_DO_FUN( empty )
{
	OBJ_DATA *obj;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	if (!str_cmp(arg2, "into") && argument[0] != '\0')
		argument = one_argument(argument, arg2);

	if (arg1[0] == '\0')
	{
		send_to_char("Opr�ni� co?" NL, ch);
		return;
	}
	if (ms_find_obj(ch))
		return;

	if ((obj = get_obj_carry(ch, arg1)) == NULL)
	{
		send_to_char("Nie masz takiego przedmiotu." NL, ch);
		return;
	}
	if (obj->count > 1)
		separate_obj(obj);

	switch (obj->item_type)
	{
	default:
		act( COL_ACTION, "Potrz�sasz $p$4 pr�buj�c to opr�ni�...", ch, obj, NULL, TO_CHAR);
		act( COL_ACTION, "$n zaczyna trz��� $p$4 pr�buj�c to opr�ni�...", ch, obj, NULL, TO_ROOM);
		return;
	case ITEM_PIPE:
		act( COL_ACTION, "Delikatnie stukasz w $p$3 i opr�niasz to.", ch, obj, NULL, TO_CHAR);
		act( COL_ACTION, "$n delikatnie stuka w $p$3 i opr�nia to.", ch, obj, NULL, TO_ROOM);
		REMOVE_BIT(obj->value[3], PIPE_FULLOFASH);
		REMOVE_BIT(obj->value[3], PIPE_LIT);
		obj->value[1] = 0;
		return;
	case ITEM_DRINK_CON:
		if (obj->value[1] < 1)
		{
			send_to_char("To jest ju� puste." NL, ch);
			return;
		}
		act( COL_ACTION, "Opr�niasz $p$3.", ch, obj, NULL, TO_CHAR);
		act( COL_ACTION, "$n opr�nia $p$3.", ch, obj, NULL, TO_ROOM);
		obj->value[1] = 0;
		return;
	case ITEM_CONTAINER:
		if (IS_SET(obj->value[1], CONT_CLOSED))
		{
			if (obj->gender == GENDER_MALE)
				act( PLAIN, "$p jest zamkni�ty.", ch, obj, NULL, TO_CHAR);
			else if (obj->gender == GENDER_FEMALE)
				act( PLAIN, "$p jest zamkni�ta.", ch, obj, NULL, TO_CHAR);
			else if (obj->gender == GENDER_NEUTRAL)
				act( PLAIN, "$p jest zamkni�te.", ch, obj, NULL, TO_CHAR);
			else
				act( PLAIN, "$p s� zamkni�te.", ch, obj, NULL, TO_CHAR);
			return;
		}
		if (obj->contents.empty())
		{
			send_to_char("To jest ju� puste." NL, ch);
			return;
		}
		if (arg2[0] == '\0')
		{
			if ( IS_SET(ch->in_room->room_flags, ROOM_NODROP) || (!IS_NPC(ch) && IS_SET(ch->act, PLR_LITTERBUG)))
			{
				send_to_char("Moc powstrzymuje ci�!" NL, ch);
				send_to_char(
				FB_BLUE "Kto� m�wi ci 'Bez �miecenia prosz�!'" EOL, ch);
				return;
			}
			if (IS_SET(ch->in_room->room_flags, ROOM_NODROPALL))
			{
				send_to_char("Nie mo�esz zrobi� tego tutaj..." NL, ch);
				return;
			}
			if (empty_obj(obj, NULL, ch->in_room))
			{
				act( COL_ACTION, "Opr�niasz $p.", ch, obj, NULL, TO_CHAR);
				act( COL_ACTION, "$n opr�nia $p.", ch, obj, NULL, TO_ROOM);
				if (IS_SET(sysdata.save_flags, SV_DROP))
					save_char_obj(ch);
			}
			else
				send_to_char("Hmmm... nie uda�o ci si�." NL, ch);
		}
		else
		{
			OBJ_DATA *dest = get_obj_here(ch, arg2);

			if (!dest)
			{
				send_to_char("Nie masz takiego przedmiotu." NL, ch);
				return;
			}
			if (dest == obj)
			{
				send_to_char("Nie mo�esz wypr�ni� przedmiotu do niego samego!" NL, ch);
				return;
			}
			if (dest->item_type != ITEM_CONTAINER)
			{
				send_to_char("To nie jest pojemnik" NL, ch);
				return;
			}
			if (IS_SET(dest->value[1], CONT_CLOSED))
			{
				if (dest->gender == GENDER_MALE)
					act( PLAIN, "$p jest zamkni�ty.", ch, dest, NULL, TO_CHAR);
				else if (dest->gender == GENDER_FEMALE)
					act( PLAIN, "$p jest zamkni�ta.", ch, dest, NULL, TO_CHAR);
				else if (dest->gender == GENDER_NEUTRAL)
					act( PLAIN, "$p jest zamkni�te.", ch, dest, NULL, TO_CHAR);
				else
					act( PLAIN, "$p s� zamkni�te.", ch, dest, NULL, TO_CHAR);
				return;
			}
			separate_obj(dest);
			if (empty_obj(obj, dest, NULL))
			{
				act( COL_ACTION, "Przek�adasz zawarto�� $p$1 do $P$1.", ch, obj, dest, TO_CHAR);
				act( COL_ACTION, "$n przek�ada zawarto�� $p$1 do $P$1.", ch, obj, dest, TO_ROOM);
				if (!dest->carried_by && IS_SET(sysdata.save_flags, SV_PUT))
					save_char_obj(ch);
			}
			else
				act( COL_ACTION, "$P nie pomie�ci niczego wi�cej.", ch, obj, dest, TO_CHAR);
		}
		return;
	}
}

/*
 * Apply a salve/ointment					-Thoric
 */
DEF_DO_FUN( apply )
{
	OBJ_DATA *obj;
	ch_ret retcode;

	if (argument[0] == '\0')
	{
		send_to_char("Wetrze� sobie co?" NL, ch);
		return;
	}

	if (ms_find_obj(ch))
		return;

	if ((obj = get_obj_carry(ch, argument)) == NULL)
	{
		send_to_char("Nie masz niczego takiego." NL, ch);
		return;
	}

	/* Thanos: requirements */
	if (!req_check(ch, obj))
		return;

	if (obj->item_type != ITEM_SALVE)
	{
		act( COL_ACTION, "$n pr�buje wetrze� sobie $p$3 w sk�r�... Dziwna osoba", ch, obj, NULL, TO_ROOM);
		act( COL_ACTION, "Pr�bujesz wciera� sobie $p$3 w sk�r�... Zwariowa�$a�?", ch, obj, NULL, TO_CHAR);
		return;
	}

	separate_obj(obj);

	--obj->value[1];
	if (!oprog_use_trigger(ch, obj, NULL, NULL, NULL))
	{
		if (obj->value[1] <= 0)
		{
			act( COL_ACTION, "Puszka z $p$4 jest ju� pusta.", ch, obj, NULL,
			TO_CHAR);
		}
		else
		{
			if (!obj->action_desc || obj->action_desc[0] == '\0')
				act( COL_ACTION, "Wcierasz sobie $p$3 w sk�r�.", ch, obj, NULL,
				TO_CHAR);
			else
				actiondesc(ch, obj, NULL);

			act( COL_ACTION, "$n wciera sobie w sk�r� $p$3.", ch, obj, NULL,
			TO_ROOM);
		}

	}

	WAIT_STATE(ch, obj->value[2]);
	retcode = obj_cast_spell(obj->value[4], obj->value[0], ch, ch, NULL);
	if (retcode == rNONE)
		retcode = obj_cast_spell(obj->value[5], obj->value[0], ch, ch, NULL);

	if (!obj_extracted(obj) && obj->value[1] <= 0)
		extract_obj(obj);

	return;
}

void actiondesc(CHAR_DATA *ch, OBJ_DATA *obj, void *vo)
{
	int liq_num = (obj->value[2] < LIQ_MAX) ? obj->value[2] : 0;
	act( COL_ACTION, obj->action_desc, ch, obj, liq_table[liq_num].liq_name,
	TO_CHAR);
	return;
}

DEF_DO_FUN( hail )
{
	int vnum;
	ROOM_INDEX_DATA *room;

	if (!ch->in_room)
		return;

	if (ch->position == POS_FIGHTING)
	{
		send_to_char("Mo�e najpierw sko�czysz walk�?!?" NL, ch);
		return;
	}

	if (ch->position < POS_STANDING)
	{
		send_to_char("Mo�e najpierw wstaniesz..." NL, ch);
		return;
	}

	if (IS_SET(ch->act, PLR_KILLER))
	{
		send_to_char("�adna taks�wka nie zatrzyma si� dla "
		FB_RED "MORDERCY" PLAIN NL, ch);
		return;

	}

	if (ch->in_room->sector_type != SECT_CITY)
	{
		send_to_char("Nie jeste� w mie�cie, �adna taks�wka nie przyleci po ciebie tutaj." NL, ch);
		return;
	}

	if (IS_SET(ch->in_room->room_flags, ROOM_INDOORS))
	{
		send_to_char("�eby to zrobi� musisz wyj�� na zewn�trz." NL, ch);
		return;
	}

	if (IS_SET(ch->in_room->room_flags, ROOM_SPACECRAFT))
	{
		send_to_char("Nie mo�esz zrobi� tego na statku!" NL, ch);
		return;
	}

	if (ch->gold < UMAX(ch->top_level - 9, 20))
	{
		send_to_char("Nie sta� ci� na taki luksus." NL, ch);
		return;
	}

	room = NULL;
	if (!IS_SET(ch->in_room->area->flags, AFLAG_NOHAIL))
	{
		vnum = ch->in_room->vnum;
		for (vnum = ch->in_room->area->lvnum; vnum <= ch->in_room->area->uvnum; vnum++)
		{
			if ((room = get_room_index(vnum)))
			{
				if (IS_SET(room->room_flags, ROOM_HOTEL))
					break;
				else
					room = NULL;
			}
		}
	}

	if (room == NULL)
	{
		send_to_char("W pobli�u nie ma �adnych taks�wek." NL, ch);
		return;
	}

	int cost = UMAX(ch->top_level - 9, 20);
	ch->gold -= cost; //UMAX(ch->top_level-9 , 20);

	act( COL_ACTION, "$n odje�dza szuka� schronienia.", ch, NULL, NULL,
	TO_ROOM);

	char_from_room(ch);
	char_to_room(ch, room);

	ch_printf(ch, "Taks�wka nadlatuje i zabiera ci� do bezpiecznego miejsca." NL
	"P�acisz szoferowi %d kredytek." NL NL, cost);
	act( COL_ACTION, "$n $T", ch, NULL, "nadlatuje taks�wk�, p�aci szoferowi i wysiada.", TO_ROOM);

	do_look(ch, (char*) "auto");

}
#if defined( ARMAGEDDON )
#define ATR_STR 1
#define ATR_INT 2
#define ATR_WIS 3
#define ATR_DEX 4
#define ATR_CON 5
#define ATR_CHA 6
#define ATR_HP_MV 7
bool check_cost( CHAR_DATA *ch, CHAR_DATA *mob, int money, int cost, int atr )
{
	int bonus=0;

	if( ch->gold < money )
	{
		ch_tell( mob, ch, "Nie sta� ci�. Nie ma trenowania." );
		return false;
	}

	if( ch->main_ability == FORCE_ABILITY
			|| (atr==ATR_STR && ch->main_ability == COMBAT_ABILITY)
			|| (atr==ATR_INT && ch->main_ability == ENGINEERING_ABILITY)
			|| (atr==ATR_DEX && ch->main_ability == SMUGGLING_ABILITY)
			|| (atr==ATR_WIS && ch->main_ability == PILOTING_ABILITY)
			|| (atr==ATR_CON && ch->main_ability == HUNTING_ABILITY)
			|| (atr==ATR_CHA && ch->main_ability == DIPLOMACY_ABILITY)
			|| (atr==ATR_CHA && ch->main_ability == LEADERSHIP_ABILITY) )
	bonus=1;

	cost-=bonus;

	if( ch->pcdata->practices < cost )
	{
		ch_tell( mob, ch, "Za ma�o masz praktyk. Nie ma trenowania." );
		return false;
	}
	ch->gold -= money;
	ch->pcdata->practices -= cost;
	return true;
}
#endif
DEF_DO_FUN( train )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *mob;
	bool tfound = false;
	bool successful = false;
#if defined( ARMAGEDDON )
    int		prac;
    int		money;
    int		cost;
    int		tmr = 5;

    money = ch->top_level*ch->top_level/20;
    money = UMAX( 1, money );
#endif

	if (IS_NPC(ch))
		return;

	strcpy(arg, argument);

	switch (ch->substate)
	{
	default:

		if (!IS_AWAKE(ch))
		{
			send_to_char("Co ci si� �ni?" NL, ch);
			return;
		}

		if (arg[0] == '\0')
		{
			send_to_char("Co chcesz trenowa�?" NL NL, ch);
			send_to_char("Masz do wyboru: " NL
			FB_WHITE "SI�" PLAIN "�, " NL
			FB_WHITE "INT" PLAIN "eligencj�, " NL
			FB_WHITE "M�D" PLAIN "ro��, " NL
			FB_WHITE "ZR�" PLAIN "czno��, " NL
			FB_WHITE "KON" PLAIN "dycj� lub " NL
			FB_WHITE "CHA" PLAIN "ryzm�", ch);
#if defined( ARMAGEDDON )
			prac = ch->pcdata->practices;

			send_to_char( NL
					FB_WHITE "HP" PLAIN " i "
					FB_WHITE "MV" PLAIN "." NL, ch );
			ch_printf( ch, NL
					"Masz do wykorzystania " FB_WHITE "%d" PLAIN " prakty%s." NL,
					prac, NUMBER_SUFF( prac, "k�", "ki", "k" ) );

			ch_printf( ch,
					"Koszt trenowania atrybut�w: %d kredyt%s i %d prakty%s." NL
					"Koszt trenowania hp/mv: %d kredyt%s i %d prakty%s." NL,
					money*5, NUMBER_SUFF( money*5, "ka", "ki", "ek" ),
					NUM_PRACT_FOR_ATTR, NUMBER_SUFF( NUM_PRACT_FOR_ATTR, "ka", "ki", "k" ),
					money, NUMBER_SUFF( money, "ka", "ki", "ek" ),
					NUM_PRACT_FOR_HP_MV, NUMBER_SUFF( NUM_PRACT_FOR_HP_MV, "ka", "ki", "k" ) );
#else
			send_to_char("." NL, ch);
#endif
			return;
		}

		for (auto* mob : ch->in_room->people)
			if ( IS_NPC(mob) && IS_SET(mob->act, ACT_TRAIN) && can_see(ch, mob))
			{
				tfound = true;
				break;
			}

		if ((!mob) || (!tfound))
		{
			send_to_char("Nie mo�esz zrobi� tego tutaj." NL, ch);
			return;
		}

		if (str_cmp(arg, "si�") && str_cmp(arg, "si��") && str_cmp(arg, "zr�") && str_cmp(arg, "zr�czno��") && str_cmp(arg, "kon")
				&& str_cmp(arg, "kondycj�") && str_cmp(arg, "cha") && str_cmp(arg, "charyzm�") && str_cmp(arg, "m�d")
				&& str_cmp(arg, "m�dro��")
#if defined( ARMAGEDDON )
                && str_cmp( arg, "hp" ) && str_cmp( arg, "mv" )
#endif
				&& str_cmp(arg, "int") && str_cmp(arg, "inteligencj�"))
		{
			do_train(ch, (char*) "");
			return;
		}

#if defined( ARMAGEDDON )
                if( !str_cmp( arg, "hp" ) || !str_cmp( arg, "mv" ) )
		    cost= NUM_PRACT_FOR_HP_MV;
		else
		    cost=NUM_PRACT_FOR_ATTR;

		if( ch->pcdata->practices < cost )
		{
		    ch_tell(mob, ch, "Nie masz wystarczaj�cej ilo�ci praktyk! Wr�� jak uzbierasz." );
		    return;
		}
#endif

		if (!str_cmp(arg, "si�") || !str_cmp(arg, "si��"))
		{
			if (mob->perm_str <= ch->perm_str || ch->perm_str >= 20 + ch->race->str_plus || ch->perm_str >= 25)
			{
				act( COL_TELL, "$n m�wi ci 'Nie pomog� ci. Jeste� ju� silniejsz$Y ni� ja sam$o.'", mob, NULL, ch, TO_VICT);
				return;
			}
#if defined( ARMAGEDDON )
		      if( !check_cost( ch, mob, money*5, cost, ATR_STR ))
		        return;
#endif
			send_to_char(FG_GREEN "Zaczynasz �wiczy� na si�owni." EOL, ch);
		}
		if (!str_cmp(arg, "zr�") || !str_cmp(arg, "zr�czno��"))
		{
			if (mob->perm_dex <= ch->perm_dex || ch->perm_dex >= 20 + ch->race->dex_plus || ch->perm_dex >= 25)
			{
				act( COL_TELL, "$n m�wi ci 'nie pomog� ci. Jeste� ju� zr�czniejsz$Y ni� ja sam$o'", mob, NULL, ch, TO_VICT);
				return;
			}
#if defined( ARMAGEDDON )
		      if( !check_cost( ch, mob, money*5, cost, ATR_DEX ))
		        return;
#endif
			send_to_char(
			FG_GREEN "Podchodzisz do test�w zwi�kszaj�cych twoj� koordynacj� ruchow�." EOL, ch);
		}
		if (!str_cmp(arg, "int") || !str_cmp(arg, "inteligencj�"))
		{
			if (mob->perm_int <= ch->perm_int || ch->perm_int >= 20 + ch->race->int_plus || ch->perm_int >= 25)
			{
				act( COL_TELL, "$n m�wi ci 'Nie pomog� ci. Jeste� ju� inteligentniejsz$Y ni� ja sam$o.'", mob, NULL, ch, TO_VICT);
				return;
			}
#if defined( ARMAGEDDON )
		      if( !check_cost( ch, mob, money*5, cost, ATR_INT ))
		        return;
#endif
			send_to_char(
			FG_GREEN "Zaczynasz studiowa� jakie� wzory matematyczne." EOL, ch);
		}
		if (!str_cmp(arg, "m�d") || !str_cmp(arg, "m�dro��"))
		{
			if (mob->perm_wis <= ch->perm_wis || ch->perm_wis >= 20 + ch->race->wis_plus || ch->perm_wis >= 25)
			{
				act( COL_TELL, "$n m�wi ci 'Nie pomog� ci. Jeste� ju� m�drzejsz$Y ode mnie.'", mob, NULL, ch, TO_VICT);
				return;
			}
#if defined( ARMAGEDDON )
		      if( !check_cost( ch, mob, money*5, cost, ATR_WIS ))
		        return;
#endif
			send_to_char(FG_GREEN "Czytasz z pasj� staro�ytne kroniki..." NL, ch);
		}
		if (!str_cmp(arg, "kon") || !str_cmp(arg, "kondycj�"))
		{
			if (mob->perm_con <= ch->perm_con || ch->perm_con >= 20 + ch->race->con_plus || ch->perm_con >= 25)
			{
				act( COL_TELL, "$n m�wi ci 'Nie pomog� ci. Twoje cia�o ju� wygl�da na wytrzymalsze ni� moje.'", mob, NULL, ch, TO_VICT);
				return;
			}
#if defined( ARMAGEDDON )
		      if( !check_cost( ch, mob, money*5, cost, ATR_CON ))
		        return;
#endif
			send_to_char(FG_GREEN "Odbywasz trening wytrzyma�o�ci." EOL, ch);
		}
		if (!str_cmp(arg, "cha") || !str_cmp(arg, "charyzm�"))
		{
			if (mob->perm_cha <= ch->perm_cha || ch->perm_cha >= 20 + ch->race->cha_plus || ch->perm_cha >= 25)
			{
				act( COL_TELL, "$n m�wi ci 'Nie pomog� ci. Twoja elokwencja ju� przerasta moj�.'", mob, NULL, ch, TO_VICT);
				return;
			}
#if defined( ARMAGEDDON )
		      if( !check_cost( ch, mob, money*5, cost, ATR_CHA ))
		        return;
#endif
			send_to_char(
			FG_GREEN "Podchodzisz do lekcji manier i etykiety." EOL, ch);
		}
#if defined( ARMAGEDDON )
          	if ( !str_cmp( arg, "hp" ) )
		{
		    tmr = 2;
	    	    if( !check_cost( ch, mob, money, cost,ATR_HP_MV ) )
		        return;
                      send_to_char(FG_GREEN "Zaczynasz zaciekle �wiczy� swoje hapy." EOL, ch);
          	}
		if ( !str_cmp( arg, "mv" ) )
		{
		    tmr = 2;
	    	    if( !check_cost( ch, mob, money, cost,ATR_HP_MV ) )
		        return;
                      send_to_char(FG_GREEN "Zaczynasz zaciekle �wiczy� swoje punkty ruchu." EOL, ch);
          	}

            	add_timer ( ch , TIMER_DO_FUN , tmr , do_train , 1 );
#else
		add_timer(ch, TIMER_DO_FUN, 10, do_train, 1);
#endif
		STRDUP(ch->dest_buf, arg);
		return;

	case 1:
		if (!*ch->dest_buf)
			return;
		strcpy(arg, ch->dest_buf);
		STRDUP(ch->dest_buf, "");
		break;

	case SUB_TIMER_DO_ABORT:
		break_skill(FG_GREEN "Tw�j trening nie zosta� zako�czony." EOL, ch);
		return;
	}

	ch->substate = SUB_NONE;
	/*---------------------------------------------------------------------*/
#if defined( ARMAGEDDON )
    if( number_percent() > 5 )
#else
	if (number_bits(2) == 0)
#endif
	{
		successful = true;
	}

	if (!str_cmp(arg, "si�") || !str_cmp(arg, "si��"))
	{
		if (!successful)
		{
			ch_printf(ch,
			FB_RED "Czujesz, �e zmarnowa�%s� tyle energii i wszystko na nic..." EOL, SEX_SUFFIX_EAE(ch));
			return;
		}
		send_to_char(
		FG_GREEN "Po wszystkich �wiczeniach, czujesz, �e twoja si�a nieco wzros�a." EOL, ch);
		ch->perm_str++;
		fevent_trigger(ch, FE_GAIN_STAT);
		return;
	}

	if (!str_cmp(arg, "zre") || !str_cmp(arg, "zr�czno��"))
	{
		if (!successful)
		{
			send_to_char(
			FB_RED "Po ca�ym tym treningu wci�� czujesz si� jak powolny klocek..." EOL, ch);
			return;
		}
		send_to_char(
		FG_GREEN "Po ci�kiej pracy nad swoj� gibko�ci�, czujesz, �e twoje cia�o sta�o si� elastyczniejsze." EOL, ch);
		ch->perm_dex++;
		fevent_trigger(ch, FE_GAIN_STAT);
		return;
	}

	if (!str_cmp(arg, "int") || !str_cmp(arg, "inteligencj�"))
	{
		if (!successful)
		{
			send_to_char(
			FB_RED "Po od�o�eniu o��wka nie czujesz nic opr�cz b�lu oczu..." EOL, ch);
			return;
		}
		send_to_char(
		FG_GREEN "Po od�o�eniu o��wka czujesz, �e twoja g�owa sta�a si� nieco pojemniejsza." EOL, ch);
		ch->perm_int++;
		fevent_trigger(ch, FE_GAIN_STAT);
		return;
	}

	if (!str_cmp(arg, "m�d") || !str_cmp(arg, "m�dro��"))
	{
		if (!successful)
		{
			send_to_char(
			FB_RED "Gdy odk�adasz ksi��ki stwierdzasz tylko 'Ale nudy...'" EOL, ch);
			return;
		}
		send_to_char(
		FG_GREEN "Po takiej lekturze, czujesz, �e jeste� w stanie poj�� o wiele wi�cej." EOL, ch);
		ch->perm_wis++;
		fevent_trigger(ch, FE_GAIN_STAT);
		return;
	}

	if (!str_cmp(arg, "kon") || !str_cmp(arg, "kondycj�"))
	{
		if (!successful)
		{
			send_to_char(
			FB_RED "Ci�ki trening nie da� ci nic opr�cz zadyszki...." EOL, ch);
			return;
		}
		send_to_char(
		FG_GREEN "Po takim wysi�ku wiesz ju�, �e twoje cia�o b�dzie potrafi�o znie�� wi�cej." EOL, ch);
		ch->perm_con++;
		fevent_trigger(ch, FE_GAIN_STAT);
		return;
	}

	if (!str_cmp(arg, "cha") || !str_cmp(arg, "charyzm�"))
	{
		if (!successful)
		{
			send_to_char(
			FB_RED "Wpadasz w depresj�. Nic nie wysz�o z nauki." EOL, ch);
			return;
		}
		send_to_char(
		FG_GREEN "Po takiej lekcji zaczynasz bardziej w siebie wierzy�." EOL, ch);
		ch->perm_cha++;
		fevent_trigger(ch, FE_GAIN_STAT);
		return;
	}
#if defined( ARMAGEDDON )
    if ( !str_cmp( arg, "hp" ) )
    {
	int add_hp      =
	    UMAX(1, con_app[get_curr_con( ch )].hitp/2);

	send_to_char(FB_GREEN "Ilo�� twoich punkt�w uderzeniowych ro�nie." EOL, ch);
        ch->max_hit+=add_hp;
	if ( !successful )
             return;
        ch->max_hit+=number_range( 0, 2 );
    	return;
    }
    if ( !str_cmp( arg, "mv" ) )
    {
	int add_move      = add_move    =
    	    number_range( 2, ( get_curr_con( ch ) + get_curr_dex( ch ) ) / 6 );

	send_to_char(FB_GREEN "Ilo�� twoich punkt�w ruchu ro�nie." EOL, ch);
        ch->max_move+=add_move;
	if ( !successful )
             return;
        ch->max_move+=number_range( 0, 2 );
    	return;
    }
#endif
}

DEF_DO_FUN( bank )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MIL];
	char buf[MSL];
	long amount = 0;
	CHAR_DATA *victim;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if ( IS_NPC(ch) || !ch->pcdata)
		return;

	if (!ch->in_room || !IS_SET(ch->in_room->room_flags, ROOM_BANK))
	{
		send_to_char("By to zrobi� musisz by� w banku." NL, ch);
		return;
	}

	if (arg1[0] == '\0')
	{
		send_to_char("Sk�adnia: BANK <deposit|withdraw|ballance|transfer> [suma] [adresat]" NL, ch);
		send_to_char("Lub:      BANK <wp�a�|wyp�a�|sprawd�|przelej> [suma] [adresat]" NL, ch);
		return;
	}

	if (arg2[0] != '\0')
		amount = atoi(arg2);

	if (!str_prefix(arg1, "ballance") || !str_prefix(arg1, "sprawd�"))
	{
		ch_printf(ch, "Na twoim koncie jest obecnie %ld kredytek." NL, ch->pcdata->bank);
		return;
	}
	else if (!str_prefix(arg1, "deposit") || !str_prefix(arg1, "wp�a�"))
	{
		if (amount <= 0)
		{
			send_to_char("Niestety bankier nie uznaje tej sumy za godn� uwagi." NL, ch);
			do_bank(ch, (char*) "");
			return;
		}

		if (ch->gold < amount)
		{
			send_to_char("Nie masz tylu kredytek." NL, ch);
			return;
		}

		ch->gold -= amount;
		ch->pcdata->bank += amount;

		ch_printf(ch, "Deponujesz na swoje konto %ld kredytek." NL, amount);
		do_bank(ch, (char*) "sprawd�");
		return;
	}
	else if (!str_prefix(arg1, "withdraw") || !str_prefix(arg1, "wyp�a�"))
	{
		if (amount <= 0)
		{
			send_to_char("Bankierowi nie chce si� i�� do sejfu po tak� ma�� sum�." NL, ch);
			do_bank(ch, (char*) "");
			return;
		}

		if (ch->pcdata->bank < amount)
		{
			send_to_char("Na twoim koncie nie ma tylu kredytek." NL, ch);
			return;
		}

		ch->gold += amount;
		ch->pcdata->bank -= amount;

		ch_printf(ch, "Wyp�acasz %ld kredytek ze swojego konta." NL, amount);
		do_bank(ch, (char*) "sprawd�");
		return;

	}
	/*Przelewy by Ganis*/
	else if (!str_prefix(arg1, "transfer") || !str_prefix(arg1, "przelej"))
	{
		if (amount <= 0)
		{
			send_to_char("Bank nie jest w stanie przela� takiej kwoty." NL, ch);
			return;
		}

		argument = one_argument(argument, arg3);

		if (!*arg3)
		{
			ch_printf(ch, "Nie poda�%c� adresata przelewu." NL,
			FEMALE( ch ) ? 'a' : 'e');
			do_bank(ch, (char*) "");
			return;
		}

		if (!(victim = get_player_world(ch, arg3)))
		{
			send_to_char("Nie ma takiej osoby w grze." NL, ch);
			return;
		}

		if (victim == ch)
		{
			send_to_char("Nie ma sensu przelewa� pieni�dzy ze swojego konta na swoje konto." NL, ch);
			return;
		}

		if (ch->pcdata->bank < amount)
		{
			send_to_char("Nie posiadasz wystarczaj�cej liczby kredytek na swoim koncie." NL, ch);
			return;
		}

		ch->pcdata->bank -= amount;
		victim->pcdata->bank += amount;

		sprintf(buf, COL_BANK "Zlecasz przelew %ld kredyt%s na konto %s." EOL, amount, NUMBER_SUFF(amount, "ki", "ek", "ek"),
				victim->przypadki[1]);
		send_to_char(buf, ch);

		sprintf(buf, "Na Twoje konto wp�yn�� w�a�nie przelew." NL\
 "Nadawca: %s" NL\
 "Kwota: %ld kredyt%s" NL, ch->name, amount,
				NUMBER_SUFF(amount, "ka", "ki", "ek"));
		note("Bank", victim->name, "Informacja o stanie konta", buf);
		/*Tak, zeby nie bylo, ze na bankach nie mozna polegac.
		 * notka poszla, wiec kasa musi na pewno wyladowac na koncie*/
		save_char_obj(ch);
		save_char_obj(victim);
		do_bank(ch, (char*) "sprawd�");
		return;
	}
	else
	{
		do_bank(ch, (char*) "");
		return;
	}

}

/* Trog: czyszczenie ekranu */
DEF_DO_FUN( cls )
{
	send_to_char(VT_CLEAR_SCREEN, ch);
}
