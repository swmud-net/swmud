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
 *			     Informational module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "classes/SWPazaak.h"
#include "mud.h"

/* Thanos:	questy */
#define Q_INFO( target ) ( ( target->inquest 			\
			&& target->inquest->player ) 		\
			? target->inquest->player->name : "ERROR" )

ROOM_INDEX_DATA* generate_exit(ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit);

const char *const where_name[] =
{ FG_CYAN "<u�ywane jako �wiat�o>   " PLAIN,
FG_CYAN "<za�o�one na palec>      " PLAIN,
FG_CYAN "<za�o�one na palec>      " PLAIN,
FG_CYAN "<owini�te wok� szyi>    " PLAIN,
FG_CYAN "<narzucone na kark>      " PLAIN,
FG_CYAN "<za�o�one na cia�o>      " PLAIN,
FG_CYAN "<za�o�one na g�ow�>      " PLAIN,
FG_CYAN "<za�o�one na nogi>       " PLAIN,
FG_CYAN "<za�o�one na stopy>      " PLAIN,
FG_CYAN "<za�o�one na r�ce>       " PLAIN,
FG_CYAN "<za�o�one na ramiona>    " PLAIN,
FG_CYAN "<tarcza energetyczna>    " PLAIN,
FG_CYAN "<zarzucone wok� cia�a>  " PLAIN,
FG_CYAN "<owini�te wok� pasa>    " PLAIN,
FG_CYAN "<za�o�one na nadgarstek> " PLAIN,
FG_CYAN "<za�o�one na nadgarstek> " PLAIN,
FG_CYAN "<bro�>                   " PLAIN,
FG_CYAN "<trzymane w r�ku>        " PLAIN,
FG_CYAN "<bro� druga>             " PLAIN,
FG_CYAN "<wpi�te w uszy>          " PLAIN,
FG_CYAN "<za�o�one na oczy>       " PLAIN,
FG_CYAN "<bro� miotaj�ca>         " PLAIN };

/*
 * Local functions.
 */
void show_char_to_char_0(CHAR_DATA *victim, CHAR_DATA *ch);
void show_char_to_char_1(CHAR_DATA *victim, CHAR_DATA *ch);
void show_char_to_char(const std::list<CHAR_DATA*>& list, CHAR_DATA *ch);
void show_ships_to_char(const std::list<SHIP_DATA*>& list, CHAR_DATA *ch);
void show_ship_to_char(SHIP_DATA *ship, CHAR_DATA *ch);
bool check_blind(CHAR_DATA *ch);
void show_condition(CHAR_DATA *ch, CHAR_DATA *victim);

char* format_obj_to_char(OBJ_DATA *obj, CHAR_DATA *ch, bool fShort)
{
	static char buf[MAX_STRING_LENGTH];

	//added by Thanos (vnum przy nazwie przedmiotu)
	if (!IS_NPC(ch) && (IS_IMMORTAL( ch ) || IS_OLCMAN(ch)) && can_edit(ch, obj->pIndexData->vnum) && IS_SET(ch->act, PLR_ROOMVNUM))
		sprintf(buf, PLAIN " {" FG_YELLOW "%5d" PLAIN "} ", obj->pIndexData->vnum);
	else
		sprintf(buf, " ");

	/*Thanos: questy*/
	if (obj->inquest && get_trust(ch) >= LEVEL_QUESTSEE)
	{
		strcat(buf, FG_GREEN "(QUEST:");
		strcat(buf, Q_INFO(obj));
		strcat(buf, ")" PLAIN " ");
	}

	/*Flaga Uzbrojony by Ganis*/
	if (obj->item_type == ITEM_GRENADE && obj->value[2] > 0)
	{
		strcat(buf, FG_RED "(Uzbrojon");
		strcat(buf, OSEX_SUFFIX_YAEE(obj));
		strcat(buf, ") ");
	}
	if (IS_OBJ_STAT(obj, ITEM_INVIS))
		strcat(buf, FG_CYAN "(Niewidka) ");
	if ((IS_AFFECTED(ch, AFF_DETECT_FORCE) || IS_IMMORTAL(ch)) && IS_OBJ_STAT(obj, ITEM_FORCE))
		strcat(buf, FB_BLUE "(B��kitna Aura) ");
	if (IS_OBJ_STAT(obj, ITEM_GLOW))
		strcat(buf, FB_YELLOW "(�wieci) ");
	if (IS_OBJ_STAT(obj, ITEM_HUM))
		strcat(buf, FG_YELLOW "(Buczy) ");
	if (IS_OBJ_STAT(obj, ITEM_HIDDEN))
		strcat(buf, FG_BLACK "(Skryte) ");
	if (IS_OBJ_STAT(obj, ITEM_BURRIED))
		strcat(buf, FG_BLACK "(Zakopane) ");
	if (IS_IMMORTAL(ch) && IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
		strcat(buf, FG_GREEN "(PROTO) ");
	if (IS_AFFECTED(ch, AFF_DETECTTRAPS) && is_trapped(obj))
		strcat(buf, FB_YELLOW "(Pu�apka) ");

	strcat(buf, FG_CYAN);
	if (fShort && obj->przypadki[0])
		strcat(buf, obj->przypadki[0]);
	else if (obj->description)
		strcat(buf, obj->description);

	return buf;
}

/*
 * Some increasingly freaky halucinated objects		-Thoric
 */
const char* halucinated_object(int ms, bool fShort)
{
	int sms = URANGE(1, (ms + 10) / 5, 20);

	if (fShort)
		switch (number_range(6 - URANGE(1, sms / 2, 5), sms))
		{
		case 1:
			return "miecz";
		case 2:
			return "pr�t";
		case 3:
			return "co� b�yszcz�cego";
		case 4:
			return "co�";
		case 5:
			return "co� interesuj�cego";
		case 6:
			return "co� kolorowego";
		case 7:
			return "co� super fajnego";
		case 8:
			return "co� tajemniczego";
		case 9:
			return "mieni�cy sie kolorami p�aszcz";
		case 10:
			return "tajemniczy p�on�cy miecz";
		case 11:
			return "miecz z py�u";
		case 12:
			return "sztandar �mierci";
		case 13:
			return "wytw�r twojej wyobra�ni";
		case 14:
			return "tw�j nagrobek";
		case 15:
			return "d�ugie buty Dartha Vadera";
		case 16:
			return "po�yskuj�cy tom encyklopedii wiedzy tajemnych";
		case 17:
			return "d�ugo skrywany sekret";
		case 18:
			return "znaczenie wszystkiego";
		case 19:
			return "odpowied�";
		case 20:
			return "klucz do �ycia, wszech�wiata i reszty";
		}
	switch (number_range(6 - URANGE(1, sms / 2, 5), sms))
	{
	case 1:
		return "Pi�kny miecz przyci�ga twoj� uwag�.";
	case 2:
		return "Ziemia pokryta jest g�r� pr�t�w.";
	case 3:
		return "Co� b�yszcz�cego skupia twoj� uwag�.";
	case 4:
		return "Co� tu le�y.";
	case 5:
		return "Co� interesuj�cego le�y sobie na ziemi.";
	case 6:
		return "Co� mieni si� tu kolorami.";
	case 7:
		return "Co� super fajnego �wietnie le�y na ziemi.";
	case 8:
		return "Co� bardzo tajemniczego i wa�nego le�y tutaj.";
	case 9:
		return "P�aszcz mieni�cy si� kolorami le�y porzucony na ziemi.";
	case 10:
		return "Tajemniczy p�on�cy miecz pali si� pod twoimi stopami.";
	case 11:
		return "Kto� zostawi� tu miecz z py�u.";
	case 12:
		return "Czarny jak �mier� sztandar zwiastuje nieszcz�cie.";
	case 13:
		return "Na twoje �yczenie pojawia si� tu wytw�r twojej wyobra�ni.";
	case 14:
		return "Zauwa�asz nagrobek... z twoim imieniem.";
	case 15:
		return "Buty, d�ugie czarne buty Dartha Vadera le�� tutaj.";
	case 16:
		return "B�yszczy si� przed tob� po�yskuj�cy tom wiedzy tajemnej.";
	case 17:
		return "D�ugo skrywany sekret odkrywa przed tob� swoje tajemnice.";
	case 18:
		return "Znaczenie wszystkiego... Takie proste, oczywiste. W�a�nie!";
	case 19:
		return "Odpowied�. Jedna. Zawsze by�a Jedna i Jedyna.";
	case 20:
		return "Klucz do �ycia, wszech�wiata i reszty niemal sam wpada ci do r�ki.";
	}
	return "OooAAaa!!!";
}

/*
 * Show a list to a character.
 * Can coalesce duplicated items.
 */
void show_list_to_char(const std::list<OBJ_DATA*>& list, CHAR_DATA *ch, bool fShort, bool fShowNothing)
{
	char **prgpstrShow;
	int *prgnShow;
	int *pitShow;
	char *pstrShow;
	int nShow;
	int iShow;
	int count;
	int offcount;
	int tmp;
	int ms;
	int cnt;
	bool fCombine;

	if (!ch->desc)
		return;

	/*
	 * if there's no list... then don't do all this crap!  -Thoric
	 */
	if (list.empty())
	{
		if (fShowNothing)
		{
			if (IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
				send_to_char(" ", ch);
			send_to_char("Nic." NL, ch);
		}
		return;
	}
	/*
	 * Alloc space for output lines.
	 */
	count = static_cast<int>(list.size());

	ms = (ch->mental_state ? ch->mental_state : 1)
			* (IS_NPC(ch) ? 1 : (ch->pcdata->condition[COND_DRUNK] ? (ch->pcdata->condition[COND_DRUNK] / 12) : 1));

	/*
	 * If not mentally stable...
	 */
	if (abs(ms) > 40)
	{
		offcount = URANGE(-(count), (count * ms) / 100, count * 2);
		if (offcount < 0)
			offcount += number_range(0, abs(offcount));
		else if (offcount > 0)
			offcount -= number_range(0, offcount);
	}
	else
		offcount = 0;

	if (count + offcount <= 0)
	{
		if (fShowNothing)
		{
			if (IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
				send_to_char(" ", ch);
			send_to_char("Nic." NL, ch);
		}
		return;
	}

	CREATE(prgpstrShow, char*, count + ((offcount > 0) ? offcount : 0));
	CREATE(prgnShow, int, count + ((offcount > 0) ? offcount : 0));
	CREATE(pitShow, int, count + ((offcount > 0) ? offcount : 0));
	nShow = 0;
	tmp = (offcount > 0) ? offcount : 0;
	cnt = 0;

	/*
	 * Format the list of objects.
	 */
	for (auto* obj : list)
	{
		if (offcount < 0 && ++cnt > (count + offcount))
			break;
		if (tmp > 0 && number_bits(1) == 0)
		{
			STRDUP(prgpstrShow[nShow], halucinated_object(ms, fShort));
			prgnShow[nShow] = 1;
			pitShow[nShow] = number_range(ITEM_LIGHT, ITEM_HERB);
			nShow++;
			--tmp;
		}
		if (obj->wear_loc == WEAR_NONE && can_see_obj(ch, obj) && (obj->item_type != ITEM_TRAP || IS_AFFECTED(ch, AFF_DETECTTRAPS)))
		{
			pstrShow = format_obj_to_char(obj, ch, fShort);
			fCombine = false;

			if (IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
			{
				/*
				 * Look for duplicates, case sensitive.
				 * Matches tend to be near end so run loop backwords.
				 */
				for (iShow = nShow - 1; iShow >= 0; iShow--)
				{
					if (!strcmp(prgpstrShow[iShow], pstrShow))
					{
						prgnShow[iShow] += obj->count;
						fCombine = true;
						break;
					}
				}
			}
			pitShow[nShow] = obj->item_type;
			/*
			 * Couldn't combine, or didn't want to.
			 */
			if (!fCombine)
			{
				STRDUP(prgpstrShow[nShow], pstrShow);
				prgnShow[nShow] = obj->count;
				nShow++;
			}
		}
	}
	if (tmp > 0)
	{
		int x;
		for (x = 0; x < tmp; x++)
		{
			STRDUP(prgpstrShow[nShow], halucinated_object(ms, fShort));
			prgnShow[nShow] = 1;
			pitShow[nShow] = number_range(ITEM_LIGHT, ITEM_HERB);
			nShow++;
		}
	}

	/*
	 * Output the formatted list.
	 */
	for (iShow = 0; iShow < nShow; iShow++)
	{
		if (fShowNothing)
			send_to_char(" ", ch);

		send_to_char(prgpstrShow[iShow], ch);

		if (prgnShow[iShow] != 1)
			ch_printf(ch, FG_CYAN " (%d)" PLAIN, prgnShow[iShow]);

		send_to_char(EOL, ch);
		STRFREE(prgpstrShow[iShow]);
	}

	if (fShowNothing && nShow == 0)
	{
		if (IS_NPC(ch) || IS_SET(ch->act, PLR_COMBINE))
			send_to_char(" ", ch);
		send_to_char("Nic." EOL, ch);
	}

	/*
	 * Clean up.
	 */
	DISPOSE(prgpstrShow);
	DISPOSE(prgnShow);
	DISPOSE(pitShow);
	return;
}

/*
 * Show fancy descriptions for certain spell affects		-Thoric
 */
void show_visible_affects_to_char(CHAR_DATA *victim, CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_AFFECTED(victim, AFF_SANCTUARY))
	{
		if (IS_GOOD(victim))
			ch_printf(ch, FB_CYAN
			" %s" FB_CYAN " promieniuje moc�." EOL, capitalize(PERS(victim, ch, 0)));
		else if (IS_EVIL(victim))
			ch_printf(ch, FG_BLACK
			" Ciemna energia emanuje z %s" FG_BLACK "." EOL, capitalize(PERS(victim, ch, 1)));
		else
			ch_printf(ch, FB_YELLOW
			" %s" FB_YELLOW " okrywa si� ca�unem �wiate� i cieni." EOL, capitalize(PERS(victim, ch, 0)));
	}

	if (IS_AFFECTED(victim, AFF_SHOCKSHIELD))
	{
		ch_printf(ch, " " FB_BLUE
		"%s" FB_BLUE " jest otoczon%s kaskadami op�ywaj�cej %s energii." EOL, capitalize(PERS(victim, ch, 0)), SEX_SUFFIX_YAE(victim),
				victim->sex == SEX_FEMALE ? "j�" : "go");
	}

	if (IS_AFFECTED(victim, AFF_CHARM))
	{
		ch_printf(ch, " %s patrzy przed siebie pozbawion%s emocji." NL, capitalize(PERS(victim, ch, 0)), SEX_SUFFIX_YAE(victim));
	}
	if (!IS_NPC(victim) && !victim->desc && victim->switched && IS_AFFECTED(victim->switched, AFF_POSSESS))
	{
		strcpy(buf, PERS(victim, ch, 0));
		strcat(buf, " wydaje si� by� w g��bokim transie..." NL);
	}
}

void show_char_to_char_0(CHAR_DATA *victim, CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	char buf1[MAX_STRING_LENGTH];

	// Thanos: zeby nie bylo
	// Odkurzacz stoi tutaj
	// przy mpat $n mpforce $n look
	if (IS_NPC(victim) && victim == supermob)
		return;

	//added by Thanos (vnum przy mobie)
	if (IS_NPC(victim) && !IS_NPC( ch )
	&& (IS_IMMORTAL( ch ) || IS_OLCMAN( ch )) && can_edit(ch,
			victim->pIndexData->vnum) && IS_SET( ch->act, PLR_ROOMVNUM ))
		sprintf(buf, PLAIN " {" FG_YELLOW "%5d" PLAIN "} ", victim->pIndexData->vnum);
	else
		sprintf(buf, " ");

	if (!IS_NPC(victim) && !victim->desc && !IS_NPC(ch))
	{
		if (IS_IMMORTAL(ch))
		{
			if (!victim->switched)
				strcat(buf, PLAIN "(Link Dead) ");
			else if (!IS_AFFECTED(victim->switched, AFF_POSSESS))
				strcat(buf, PLAIN "(Switched) ");
		}
	}

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_AFK))
		strcat(buf, FG_YELLOW "(AWAY) " PLAIN);

	if (!IS_NPC(victim) && IS_IMMORTAL(victim) && victim->desc && victim->desc->olc_editing)
		strcat(buf, FB_BLUE "(Buduje)" PLAIN " ");

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_NOTE_WRITE))
		strcat(buf, FB_WHITE "(Notuje)" PLAIN " ");

	if ((!IS_NPC(victim) && IS_SET(victim->act, PLR_WIZINVIS)) || (IS_NPC(victim) && IS_SET(victim->act, ACT_MOBINVIS)))
	{
		if (!IS_NPC(victim))
			sprintf(buf1, FG_BLUE "(Invis %d)" PLAIN " ", victim->pcdata->wizinvis);
		else
			sprintf(buf1, FG_BLUE "(Mobinvis %d)" PLAIN " ", victim->mobinvis);
		strcat(buf, buf1);
	}

	if (!IS_NPC(victim) && IS_NEWBIE(victim))
		strcat(buf, FG_MAGENTA "(NEWBIE)" PLAIN " ");

	if (IS_AFFECTED(victim, AFF_INVISIBLE))
		strcat(buf,
				MALE(victim) ? FG_CYAN "(Niewidzialny)" PLAIN " " :
				FEMALE(victim) ? FG_CYAN "(Niewidzialna)" PLAIN " " : FG_CYAN "(Niewidzialne)" PLAIN " ");

	if (IS_AFFECTED(victim, AFF_HIDE))
		strcat(buf,
				MALE(victim) ? FG_BLACK "(Skryty)" PLAIN " " :
				FEMALE(victim) ? FG_BLACK "(Skryta)" PLAIN " " : FG_BLACK "(Skryte)" PLAIN " ");

	if (IS_AFFECTED(victim, AFF_PASS_DOOR))
		strcat(buf,
				MALE(victim) ? FG_CYAN "(Bezcielesny)" PLAIN " " :
				FEMALE(victim) ? FG_CYAN "(Bezcielesna)" PLAIN " " : FG_CYAN "(Bezcielesne)" PLAIN " ");

	if (get_curr_frc(victim) > 0 && (IS_AFFECTED(ch, AFF_DETECT_FORCE ) || IS_SET(ch->act, PLR_HOLYLIGHT)))
		strcat(buf, FB_CYAN "(Aura Mocy)" PLAIN " ");

	if (get_curr_frc(ch) > 0 && IS_EVIL(victim))
		strcat(buf, FG_RED "(Aura Z�a)" PLAIN " ");

	if (get_curr_frc(ch) > 0 && IS_GOOD(victim))
		strcat(buf, FG_GREY MOD_BOLD "(Aura Dobra)" PLAIN " ");

	if (IS_AFFECTED(victim, AFF_PROTECT) && (IS_AFFECTED(ch, AFF_DETECT_FORCE) || IS_SET(ch->act, PLR_HOLYLIGHT)))
		strcat(buf, FB_GREEN "(Aura Protekcji)" PLAIN " ");

	if (!IS_NPC(victim) && IS_SET(victim->act, PLR_LITTERBUG))
		strcat(buf, PLAIN "(�MIECIARZ) ");
	if (IS_NPC(victim) && IS_IMMORTAL(ch) && IS_SET(victim->act, ACT_PROTOTYPE))
		strcat(buf, FG_GREEN "(PROTOTYP)" PLAIN " ");

	if (!IS_NPC(victim) && IS_AFFECTED( victim, AFF_DISGUISE )
	&& victim->pcdata && *victim->pcdata->fake_name && !IS_NPC( ch )
	&& IS_IMMORTAL( ch ))
	{
		strcat(buf, "[");
		strcat(buf, victim->name);
		strcat(buf, "]");
	}

	if (IS_NPC(victim) && victim->inquest && get_trust(ch) >= LEVEL_QUESTSEE)
	{
		strcat(buf, FG_GREEN "(QUEST:");
		strcat(buf, Q_INFO(victim));
		strcat(buf, ")" PLAIN " ");
	}
	send_to_char(buf, ch);

	if (victim->position == POS_STANDING && victim->long_descr[0] != '\0')
	{	//Trog
		ch_printf(ch, FB_RED "%s" PLAIN, victim->long_descr);
		show_visible_affects_to_char(victim, ch);
		return;
	}

	/*   strcat( buf, PERS( victim, ch ) );       old system of titles
	 *    removed to prevent prepending of name to title     -Kuran
	 *
	 *    But added back bellow so that you can see mobs too :P   -Durga
	 */
	/* Thanos: disguise */
	if (!IS_NPC(victim) && IS_AFFECTED(victim, AFF_DISGUISE) && victim->pcdata)
	{
		if (victim->position == POS_STANDING && *victim->pcdata->fake_long)
		{
			ch_printf(ch, FB_RED "%s" PLAIN, victim->pcdata->fake_long);
			show_visible_affects_to_char(victim, ch);
			return;
		}

		if (IS_SET(ch->act, PLR_BRIEF) || !*victim->pcdata->fake_title)
			sprintf(buf, FB_RED "%s", victim->pcdata->fake_infl[0]);
		else
			sprintf(buf, FB_RED "%s %s", victim->pcdata->fake_infl[0], victim->pcdata->fake_title);
	}	// !disguise
	else
	{
		if (!IS_NPC(victim) && victim->pcdata && !IS_SET(ch->act, PLR_BRIEF))
			//Tanglor - przy patrzeniu w lokacji
			sprintf(buf, FB_RED "%s %s", fcapitalize(PERS(victim, ch, 0)), does_knows(ch, victim) ? victim->pcdata->title : "\0");
		//			sprintf( buf,FB_RED "%s %s", fcapitalize( PERS(victim, ch, 0) ),
		//				victim->pcdata->title );
		else
			sprintf(buf, FB_RED "%s", fcapitalize(PERS(victim, ch, 0)));
	}

	strcat(buf, PLAIN FB_RED);

	switch (victim->position)
	{
	case POS_DEAD:
		strcat(buf, MALE( victim ) ? " jest MARTWY!!" : FEMALE(victim) ? " jest MARTWA!!" : " jest MARTWE!!");
		break;
	case POS_MORTAL:
		strcat(buf, MALE( victim ) ? " jest �miertelnie ranny." : FEMALE(victim) ? " jest �miertelnie ranna." : " jest �miertelnie ranne.");
		break;
	case POS_INCAP:
		strcat(buf, MALE( victim ) ? " jest nieprzytomny." : FEMALE(victim) ? " jest nieprzytomna." : " jest nieprzytomne.");
		break;
	case POS_STUNNED:
		strcat(buf, MALE( victim ) ? " le�y tu og�uszony." : FEMALE(victim) ? " le�y tu og�uszona." : " le�y tu og�uszone.");
		break;
	case POS_SLEEPING:
		if (ch->position == POS_SITTING || ch->position == POS_RESTING)
			strcat(buf, " �pi nieopodal ciebie.");
		else
			strcat(buf,
					MALE(victim) ? " le�y tu pogr��ony we �nie." :
					FEMALE(victim) ? " le�y tu pogr��ona we �nie." : " le�y tu pogr��one we �nie.");
		break;
	case POS_RESTING:
		if (ch->position == POS_RESTING)
			strcat(buf, " odpoczywa nieopodal ciebie.");
		else if (ch->position == POS_MOUNTED)
			strcat(buf, " odpoczywa pod stopami twojego wierzchowca.");
		else
			strcat(buf, " odpoczywa tutaj.");
		break;
	case POS_SITTING:
		if (ch->position == POS_SITTING)
			strcat(buf, " siedzi tu z tob�.");
		else if (ch->position == POS_RESTING)
			strcat(buf, " siedzi obok ciebie.");
		else
			strcat(buf, " siedzi sobie tutaj.");
		break;
	case POS_STANDING:
		if (IS_IMMORTAL(victim))
			strcat(buf, " stoi tutaj.");
		else if ((victim->in_room->sector_type == SECT_UNDERWATER) && !IS_AFFECTED(victim, AFF_AQUA_BREATH) && !IS_NPC(victim))
			strcat(buf, " tonie na twoich oczach.");
		else if (victim->in_room->sector_type == SECT_UNDERWATER)
			strcat(buf, " jest tu w wodzie.");
		else if ((victim->in_room->sector_type == SECT_OCEANFLOOR) && !IS_AFFECTED(victim, AFF_AQUA_BREATH) && !IS_NPC(victim))
			strcat(buf, " tonie na twoich oczach.");
		else if (victim->in_room->sector_type == SECT_OCEANFLOOR)
			strcat(buf, " stoi tu w wodzie.");
		else if (IS_AFFECTED(victim, AFF_FLOATING) || IS_AFFECTED(victim, AFF_FLYING))
		{
			if (!IS_AFFECTED(ch, AFF_FLYING) && !IS_AFFECTED(ch, AFF_FLOATING))
				strcat(buf, " unosi si� lekko obok ciebie.");
			else
				strcat(buf, " unosi si� nad twoj� g�ow�.");
		}
		else
			strcat(buf, " stoi tutaj.");
		break;
	case POS_SHOVE:
		strcat(buf, " is being shoved around.");
		break;
	case POS_DRAG:
		strcat(buf, " is being dragged around.");
		break;
	case POS_MOUNTED:
		strcat(buf, " jest tu, dosiada ");
		if (!victim->mount)
			strcat(buf, "powietrza???");
		else if (victim->mount == ch)
			strcat(buf, "twojego grzbietu.");
		else if (victim->in_room == victim->mount->in_room)
		{
			strcat(buf, PERS(victim->mount, ch, 1));
			strcat(buf, ".");
		}
		else
			strcat(buf, "kogo� kto wyszed�???");
		break;
	case POS_FIGHTING:
		strcat(buf, " walczy tutaj z ");
		if (!victim->fighting)
			strcat(buf, "powietrzem???");
		else if (who_fighting(victim) == ch)
			strcat(buf, "TOB�!");
		else if (victim->in_room == victim->fighting->who->in_room)
		{
			strcat(buf, PERS(victim->fighting->who, ch, 4));
			strcat(buf, ".");
		}
		else
			strcat(buf, "kim� kto sobie poszed�???");
		break;
	}

	strcat(buf, EOL);
	buf[0] = UPPER(buf[0]);
	send_to_char(buf, ch);
	show_visible_affects_to_char(victim, ch);
	return;
}

void show_char_to_char_1(CHAR_DATA *victim, CHAR_DATA *ch)
{
	OBJ_DATA *obj;
	int iWear;
	bool found;

	if (ch == victim)
		act(COL_ACTION, "$n patrzy na siebie.", ch, NULL, victim, TO_NOTVICT);
	else if (can_see(victim, ch))
	{
		act(COL_ACTION, "$n patrzy na ciebie.", ch, NULL, victim, TO_VICT);
		act(COL_ACTION, "$n patrzy na $N$3.", ch, NULL, victim, TO_NOTVICT);
	}

	//added by Thanos (pokazuje rase zainteresowanej osoby)
	if ((!IS_NPC(victim) && !IS_IMMORTAL(victim)) || IS_NPC(victim) || IS_IMMORTAL(ch))
	{
		ch_printf(ch, FB_WHITE "%s - ", capitalize(PERS(victim, ch, 0)));
		ch_printf(ch, "%s." EOL, (IS_NPC( victim ) && IS_SET(victim->act, ACT_DROID)) ? "Droid" : CH_RACE(victim, 0)); // Pixel: przypadkiF
	}
	//done
	if (IS_DESCRIBED(victim))
	{
		const SWString &attrib = format_char_attribute(victim, 0);
		ch_printf(ch, "Wygl�da jak" FG_YELLOW " %s" EOL, attrib.c_str());
	}

	if (IS_AFFECTED( victim, AFF_DISGUISE ) && victim->pcdata && *victim->pcdata->fake_desc)
		send_to_char(victim->pcdata->fake_desc, ch);
	else if (victim->description[0] != '\0' && !IS_AFFECTED(victim, AFF_DISGUISE))
		send_to_char(victim->description, ch);
	else
		act(PLAIN, "Nie widzisz w $G nic specjalnego.", ch, NULL, victim,
		TO_CHAR);

	show_condition(ch, victim);

	found = false;
	for (iWear = 0; iWear < MAX_WEAR; iWear++)
	{
		if ((obj = get_eq_char(victim, iWear)) != NULL && can_see_obj(ch, obj))
		{
			if (!found)
			{
				send_to_char(EOL, ch);
				act(PLAIN, FB_CYAN "$t ma na sobie:" PLAIN, ch,
				MALE( victim ) ? "On" : FEMALE(victim) ? "Ona" : "Ono", victim, TO_CHAR);
				found = true;
			}
			/* by Ratm: Ugly haX00r, ale nie chce mi sie tego robi� poprawnie
			 bo bym musia� zmienia� w pizdu rzeczy i potem godzinami
			 testowa� czy dzia�a, wybaczcie ;) */
			if (iWear == WEAR_WIELD)
			{
				if (IS_SET(obj->wear_flags, ITEM_DUAL_WIELD))
					send_to_char(FG_CYAN "<bro� dwur�czna>         " PLAIN, ch);
				else
					send_to_char(where_name[iWear], ch);
			}
			else
				send_to_char(where_name[iWear], ch);

			send_to_char(format_obj_to_char(obj, ch, true), ch);
			send_to_char(EOL, ch);
		}
	}

	/*
	 * Crash fix here by Thoric
	 */
	if (IS_NPC(ch) || victim == ch)
		return;

	if (number_percent() < ch->pcdata->learned[gsn_peek])
	{
		ch_printf(ch, NL FB_CYAN "Zagl�dasz w je%s ekwipunek:" EOL,
		FEMALE( victim ) ? "j" : "go");
		show_list_to_char(victim->carrying, ch, true, true);
		learn_from_success(ch, gsn_peek);
	}
	else if (ch->pcdata->learned[gsn_peek])
		learn_from_failure(ch, gsn_peek);

	return;
}

void show_char_to_char(const std::list<CHAR_DATA*>& list, CHAR_DATA *ch)
{
	for (auto* rch : list)
	{
		/* Thanos: najpierw moby */
		if (rch == ch || !IS_NPC(rch))
			continue;

		if (can_see(ch, rch))
			show_char_to_char_0(rch, ch);
		else if (IS_RACE(rch, "DEFEL"))
			send_to_char(FG_RED
			"Para czerwonych oczu uwa�nie ci si� przygl�da." EOL, ch);
		else if (room_is_dark(ch->in_room) && IS_AFFECTED(ch, AFF_INFRARED))
			send_to_char(FB_RED
			"Ciemny kszta�t �ywej istoty rysuje si� przed tob�." EOL, ch);
	}

	for (auto* rch : list)
	{
		/* Thanos: potem gracze */
		if (rch == ch || IS_NPC(rch))
			continue;

		if (can_see(ch, rch))
			show_char_to_char_0(rch, ch);
		else if (IS_RACE( rch, "DEFEL" ) && (!IS_SET(rch->act, PLR_WIZINVIS) || rch->pcdata->wizinvis <= get_trust(ch)))
			send_to_char(FG_RED
			"Para czerwonych oczu uwa�nie ci si� przygl�da." EOL, ch);
		else if (room_is_dark(ch->in_room) && IS_AFFECTED(ch, AFF_INFRARED))
			send_to_char(FB_RED
			"Ciemny kszta�t �ywej istoty rysuje si� przed tob�." EOL, ch);
	}
	return;
}

/* Trog: zwraca wska�nik na pierwszego gracza na statku
 * i dodatkowo w polu newbie czy na statku jest co najmniej
 * jeden gracz NEWBIE */
CHAR_DATA* player_on_ship(SHIP_DATA *ship, bool *newbie)
{
	CHAR_DATA *first_ch = NULL;

	*newbie = false;
	for (auto* room : ship->locations)
	{
		for (auto* ch : room->people)
		{
			if (!IS_NPC(ch) && !first_ch)
				first_ch = NULL;

			if (!*newbie && !IS_NPC(ch) && IS_NEWBIE(ch))
				*newbie = true;
		}
	}

	return first_ch;
}

const char* player_ship_prefix(SH *ship)
{
	CD *ch;
	bool newbie;

	ch = player_on_ship(ship, &newbie);
	if (newbie)
		return FG_MAGENTA "(NEWBIE)" PLAIN;

	if (ch)
		return FG_GREEN "(GNP)" PLAIN;

	return "";
}

void show_ships_to_char(const std::list<SHIP_DATA*>& list, CD *ch)
{
	auto it = list.begin();
	while (it != list.end())
	{
		SH *rship = *it;
		ch_printf(ch, "%8.8s" FB_CYAN "%s%-33s     " FB_CYAN, player_ship_prefix(rship), rship->hatchopen ? "  " : FG_GREEN "][" FB_CYAN,
				SHIPNAME(rship));

		++it;
		if (it != list.end())
		{
			SH *nship = *it;
			ch_printf(ch, FB_CYAN "%s%-33s" FB_CYAN, nship->hatchopen ? "  " : FG_GREEN "][" FB_CYAN, SHIPNAME(nship));
			++it;
		}
		send_to_char(EOL, ch);
	}

	return;
}

void show_ship_to_char(SHIP_DATA *ship, CHAR_DATA *ch)
{
	ch_printf(ch, FB_CYAN "%s: %s" EOL "%s", SHIPNAME(ship), bit_name(ship_classes_name_list, ship->clazz),
			*ship->description ? ship->description : "Nie widzisz w tym statku nic specjalnego." NL);

	ch_printf(ch, "%s" PLAIN " ma %s klap�." NL, SHIPNAME(ship), ship->hatchopen ? "otwart�" : "zamkni�t�");
	if (IS_OLCMAN(ch))
		ch_printf(ch, "Ship Transponder: %s" NL, ship->transponder);
	return;
}

bool check_blind(CHAR_DATA *ch)
{
	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;

	if (IS_AFFECTED(ch, AFF_TRUESIGHT))
		return true;

	if (IS_AFFECTED(ch, AFF_BLIND))
	{
		send_to_char("Nic nie widzisz!" EOL, ch);
		return false;
	}

	return true;
}

/*
 * Returns classical DIKU door direction based on text in arg	-Thoric
 */
int get_door(char *arg)
{
	int door;

	if (!str_cmp(arg, "n") || !str_cmp(arg, "north"))
		door = 0;
	else if (!str_cmp(arg, "e") || !str_cmp(arg, "east"))
		door = 1;
	else if (!str_cmp(arg, "s") || !str_cmp(arg, "south"))
		door = 2;
	else if (!str_cmp(arg, "w") || !str_cmp(arg, "west"))
		door = 3;
	else if (!str_cmp(arg, "u") || !str_cmp(arg, "up"))
		door = 4;
	else if (!str_cmp(arg, "d") || !str_cmp(arg, "down"))
		door = 5;
	else if (!str_cmp(arg, "ne") || !str_cmp(arg, "northeast"))
		door = 6;
	else if (!str_cmp(arg, "nw") || !str_cmp(arg, "northwest"))
		door = 7;
	else if (!str_cmp(arg, "se") || !str_cmp(arg, "southeast"))
		door = 8;
	else if (!str_cmp(arg, "sw") || !str_cmp(arg, "southwest"))
		door = 9;
	else
		door = -1;
	return door;
}

DEF_DO_FUN( look )
{
	char arg[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	EXIT_DATA *pexit;
	CHAR_DATA *victim;
	OBJ_DATA *obj;
	ROOM_INDEX_DATA *original;
	SHIP_DATA *ship;
	char *pdesc;
	bool doProg;
	int door;
	int number, cnt;
	SPACE_DATA *starsystem;
	HANGAR_DATA *hangar;
	SHIPDOCK_DATA *dock;

	if (!ch->desc)
		return;

	if (ch->position < POS_SLEEPING)
	{
		send_to_char("Widzisz tylko gwiazdy... Ale� one pi�kne!" NL, ch);
		return;
	}

	if (ch->position == POS_SLEEPING)
	{
		send_to_char("Nic nie widzisz, przecie� �pisz! Zzzzz" NL, ch);
		return;
	}

	if (!check_blind(ch))
		return;

	if (!IS_NPC(ch) && !IS_SET(ch->act, PLR_HOLYLIGHT) && !IS_AFFECTED(ch, AFF_TRUESIGHT) && !IS_AFFECTED(ch, AFF_INFRARED)
			&& room_is_dark(ch->in_room))
	{
		send_to_char(FG_BLACK "Jest tylko czarna jak smo�a ciemno��..." EOL, ch);
		show_char_to_char(ch->in_room->people, ch);
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	doProg = str_cmp("noprog", arg2) && str_cmp("noprog", arg3);

	if (arg1[0] == '\0' || !str_cmp(arg1, "auto"))
	{
		SHIP_DATA *ship;

		if (!ch->desc->original)
		{
			if ((IS_IMMORTAL( ch ) || IS_OLCMAN(ch)) && can_edit(ch, ch->in_room->vnum) && (IS_SET(ch->act, PLR_ROOMVNUM)))
			{
				/* Added 10/17 by Kuran of */
				send_to_char(PLAIN "{" FG_YELLOW, ch); /* SWReality */
				ch_printf(ch, "%d" PLAIN, ch->in_room->vnum);
				if (ch->in_room->vnum > MAX_VNUM)
					send_to_char(":" FG_YELLOW "v_room" PLAIN, ch);
				else if (!ch->in_room->vnum)
					ch_printf(ch, "(SHIP):" FG_YELLOW "%d" PLAIN, ch->in_room->svnum);
				send_to_char("} ", ch);
			}
		}

		/* 'look' or 'look auto' */
		send_to_char(COL_RMNAME, ch);
		if (ch->in_room && ch->in_room->homename && *ch->in_room->homename != '\0')
			send_to_char(ch->in_room->homename, ch);
		else
			send_to_char(ch->in_room->name, ch);
		send_to_char(PLAIN, ch);

		if (!ch->desc->original)
		{

			if ((IS_IMMORTAL( ch ) || IS_OLCMAN(ch)) && can_edit(ch, ch->in_room->vnum) && (IS_SET(ch->pcdata->flags, PCFLAG_ROOM)))
			{
				send_to_char("  [", ch);
				send_to_char(bit_name(sector_types_list, ch->in_room->sector_type), ch);
				send_to_char("]" FG_CYAN "  [", ch);
				send_to_char(flag_string(room_flags_list, ch->in_room->room_flags), ch);
				send_to_char("]" PLAIN, ch);
			}

		}
		send_to_char(NL, ch);

		if (!IS_NPC(ch) && IS_SET(ch->act, PLR_AUTOEXIT))
			do_exits(ch, (char*) "auto");

		if (arg1[0] == '\0' || (!IS_NPC(ch) && !IS_SET(ch->act, PLR_BRIEF)))
		{ //modified by Trog
			if (ch->in_room->nightdesc && ch->in_room->nightdesc[0] != '\0' && ((GET_PLANET(ch))->hour > 19 || (GET_PLANET(ch))->hour < 5))
				send_to_char(ch->in_room->nightdesc, ch);
			else
				send_to_char(ch->in_room->description, ch);
		} //done

		if (!IS_NPC(ch) && IS_SET(ch->act, PLR_MAPEXITS))
			do_exits(ch, (char*) "maps");

		/* Thanos	4 Aldegard */
		if (!IS_NPC(ch) && IS_SET(ch->act, PLR_LONGEXITS))
			do_exits(ch, (char*) "");

		if ((ch->in_room->vnum && (ship = ship_from_room(ch->in_room)) != NULL))
		{
			if (ship->entrance == ch->in_room)
				ch_printf(ch, FG_CYAN "%s w�az" EOL, !ship->hatchopen ? "Zamkni�ty" : "Otwarty");

			for (auto* dock : ship->docks)
				if (dock->vnum == ch->in_room->vnum)
					ch_printf(ch, FG_CYAN "%s �luza %s" EOL,
							dock->status == 0 ? "Zabezpieczona" : (dock->status > 1 ? "Wy�amana" : "Otwarta"),
							dock->target != NULL ? SHIPNAME(dock->target) : "");

			if (!IS_NPC(ch)) /* <-- Thanos: bezpieczniczek */
				if (IS_SET(ch->pcdata->flags, PCFLAG_SHIPSTAT))
					show_module_to_char0(ship, ch, ch->in_room->vnum);
		}
		else if ((!ch->in_room->vnum && (ship = ch->in_room->ship)))
		{
			if (ship->entrance == ch->in_room)
				ch_printf(ch, FG_CYAN "%s w�az" EOL, !ship->hatchopen ? "Zamkni�ty" : "Otwarty");

			for (auto* dock : ship->docks)
				if (dock->vnum == ch->in_room->vnum)
					ch_printf(ch, FG_CYAN "%s �luza %s" EOL,
							dock->status == 0 ? "Zabezpieczona" : (dock->status > 1 ? "Wy�amana" : "Otwarta"),
							dock->target != NULL ? SHIPNAME(dock->target) : "");

			if (!IS_NPC(ch)) /* <-- Thanos: bezpieczniczek */
				if (IS_SET(ch->pcdata->flags, PCFLAG_SHIPSTAT))
					show_module_to_char0(ship, ch, ch->in_room->vnum);
		}

		show_ships_to_char(ch->in_room->ships, ch);
		show_list_to_char(ch->in_room->contents, ch, false, false);
		show_char_to_char(ch->in_room->people, ch);

		if (str_cmp(arg1, "auto"))
			if ((ship = ship_from_cockpit(ch->in_room)) != NULL || ((ship = ship_from_hangar(ch->in_room)) != NULL && (((hangar =
					hangar_from_room(ship, ch->in_room->vnum)) != NULL) && hangar->status == 0))
					|| ((ship = ship_from_room(ch->in_room)) != NULL
							&& (((dock = shipdock_from_room(ship, ch->in_room->vnum)) != NULL) && dock->status > 0 && dock->target == NULL)))
			{
				pager_printf(ch, NL FB_CYAN "Przez iluminator wida�:" EOL);

				if ((starsystem = ship->starsystem) != NULL)
				{
					int slma = 315 - 3 * ch->top_level;

					pager_printf(ch,
					FB_GREEN "%s" EOL, starsystem->description2[0] != '\0' ? starsystem->description2 : "");
					/*by Aldegard - jak cos jest dalej niz 'blabla' to tego po prostu nie widac*/
					for (auto* star : starsystem->stars)
						if (IS_SET(ch->act, PLR_HOLYLIGHT)
								|| (srange((star->xpos - ship->vx), (star->ypos - ship->vy), (star->zpos - ship->vz)) < 150000))
							pager_printf(ch,
							FB_YELLOW "%-40s %-8.0f %-8.0f %.0f" EOL, star->name, star->xpos - ship->vx + number_range(-slma, slma),
									star->ypos - ship->vy + number_range(-slma, slma), star->zpos - ship->vz + number_range(-slma, slma));

					for (auto* planet : starsystem->planets)
						if (IS_SET(ch->act, PLR_HOLYLIGHT)
								|| (srange((planet->xpos - ship->vx), (planet->ypos - ship->vy), (planet->zpos - ship->vz)) < 60000))
							pager_printf(ch,
							FB_RED "%-40s %-8.0f %-8.0f %-8.0f" EOL, planet->name, planet->xpos - ship->vx + number_range(-slma, slma),
									planet->ypos - ship->vy + number_range(-slma, slma),
									planet->zpos - ship->vz + number_range(-slma, slma));

					for (auto* moon : starsystem->moons)
						if (IS_SET(ch->act, PLR_HOLYLIGHT)
								|| (srange((moon->xpos - ship->vx), (moon->ypos - ship->vy), (moon->zpos - ship->vz)) < 40000))
							pager_printf(ch,
							FB_WHITE "%-40s %-8.0f %-8.0f %-8.0f" EOL, moon->name, moon->xpos - ship->vx + number_range(-slma, slma),
									moon->ypos - ship->vy + number_range(-slma, slma), moon->zpos - ship->vz + number_range(-slma, slma));

					for (auto* astro : ship->starsystem->astros)
					{
						if (!IS_SET(ch->act, PLR_HOLYLIGHT)
								&& srange((ship->vx - astro->ox), (ship->vy - astro->oy), (ship->vz - astro->oz))
										> (astro->value + 1) * 150)
							continue;
						pager_printf(ch, FB_GREEN "%-33s" NL, astro_names[astro->type]);
					}

					for (auto* target : ship->starsystem->ships)
					{
						if (!IS_SET(ch->act, PLR_HOLYLIGHT) /*Aldegard*/
								&& (target->cloack != 0
										|| (srange((target->vx - ship->vx), (target->vy - ship->vy), (target->vz - ship->vz))
												> 4000 + target->size * 100)))
						{
							continue;
						}
						if (!IS_SET(ch->act, PLR_HOLYLIGHT)
								&& (srange((target->vx - ship->vx), (target->vy - ship->vy), (target->vz - ship->vz))
										> 3900 + target->size * 90))
						{
							pager_printf(ch, FB_GREEN
							"Male�ki poruszaj�cy si� punkt." EOL);
							continue;
						} /*done*/

						if (target != ship)
							pager_printf(ch, "%8.8s" FB_GREEN "%s %s%s" EOL, player_ship_prefix(target), target->sslook,
									bit_name(ship_classes_name_list, target->clazz), target->timer >= 0 ? FB_RED " (P�onie)" FB_GREEN : "");
					}
					for (auto* missile : ship->starsystem->missiles)
					{
						if (!IS_SET(ch->act, PLR_HOLYLIGHT)
								&& (srange((missile->mx - ship->vx), (missile->my - ship->vy), (missile->mz - ship->vz)) > 2000))
						{
							continue;
						}
						pager_printf(ch, "%s" NL,
								missile->missiletype == CONCUSSION_MISSILE ?
										"         Pocisk Samonaprowadzaj�cy" :
										(missile->missiletype == PROTON_TORPEDO ?
												"         Torpeda" :
												(missile->missiletype == HEAVY_ROCKET ? "         Ci�ka Rakieta" : "         Ci�ka Bomba")));
					}

				}
				else if (ship->location == ship->lastdoc && ship->shipstate != SHIP_REPOSITORY)
				{
					ROOM_INDEX_DATA *to_room;

					if ((to_room = ship->location) != NULL)
					{
						ch_printf(ch, NL);
						original = ch->in_room;
						char_from_room(ch);
						char_to_room(ch, to_room);
						do_glance(ch, (char*) "");
						char_from_room(ch);
						char_to_room(ch, original);
					}

				}
				else if (ship->shipstate == SHIP_REPOSITORY)
				{
					ch_printf(ch, FB_GREEN "Wrak ISD Perll" EOL);
					ch_printf(ch, FB_GREEN "Wrak MC90 Windows 98" EOL);
					ch_printf(ch, FB_GREEN "Wrak YT 1300 Maluczka" EOL);
					ch_printf(ch, FB_GREEN "Wrak Blade Runer" EOL);
					ch_printf(ch, FB_GREEN "Wrak NF Horda nf123" EOL);
					ch_printf(ch, FB_GREEN "Wrak X-w0001" EOL);
					ch_printf(ch, FB_GREEN "Wrak AS0122 York" EOL);
					ch_printf(ch, FB_GREEN "Wrak B-w052 Strotoss" EOL);
					ch_printf(ch, FB_GREEN "Wrak SSD Enforcer One" EOL);
				}
				else
					ch_printf(ch, EOL " Tylko jakie� dziwne paski." NL);
			}
		send_to_char(PLAIN, ch);
		fevent_trigger(ch, FE_FIND_OBJECT);
		fevent_trigger(ch, FE_FIND_MOB);
		fevent_trigger(ch, FE_FIND_SHIP);
		fevent_trigger(ch, FE_FIND_PLAYER);
		return;
	}

	if (!str_cmp(arg1, "under"))
	{
		int count;

		/* 'look under' */
		if (arg2[0] == '\0')
		{
			send_to_char("Spojrze� pod co?" NL, ch);
			return;
		}

		if ((obj = get_obj_here(ch, arg2)) == NULL)
		{
			send_to_char("Nie widzisz tego tutaj." NL, ch);
			return;
		}
		if (ch->carry_weight + obj->weight > can_carry_w(ch))
		{
			send_to_char("Nie uniesiesz tego, by zajrze� pod sp�d." NL, ch);
			return;
		}
		count = obj->count;
		obj->count = 1;
		act(PLAIN, "Lekko podnosisz $p$3 i zagl�dasz pod sp�d:", ch, obj, NULL,
		TO_CHAR);
		act(PLAIN, "$n lekko unosi $p$3 i zagl�da pod sp�d:", ch, obj, NULL,
		TO_ROOM);
		obj->count = count;
		if (IS_OBJ_STAT(obj, ITEM_COVERING))
			show_list_to_char(obj->contents, ch, true, true);
		else
			send_to_char("Nic." NL, ch);
		if (doProg)
			oprog_examine_trigger(ch, obj);
		return;
	}

	if (!str_cmp(arg1, "i") || !str_cmp(arg1, "in"))
	{
		int count;

		/* 'look in' */
		if (arg2[0] == '\0')
		{
			send_to_char("Spojrze� w co?" NL, ch);
			return;
		}

		if ((obj = get_obj_here(ch, arg2)) == NULL)
		{
			send_to_char("Nie widzisz tego tutaj." NL, ch);
			return;
		}

		switch (obj->item_type)
		{
		default:
			send_to_char("To nie jest pojemnik." NL, ch);
			break;

		case ITEM_DRINK_CON:
			if (obj->value[1] <= 0)
			{
				send_to_char("To jest puste." NL, ch);
				if (doProg)
					oprog_examine_trigger(ch, obj);
				break;
			}

			ch_printf(ch, "To jest w %s po�owie wype�nione %s ciecz�." NL,
					obj->value[1] < obj->value[0] / 4 ? "prawie" : obj->value[1] < 3 * obj->value[0] / 4 ? "oko�o" : "ponad",
					liq_table[obj->value[2]].liq_color);

			if (doProg)
				oprog_examine_trigger(ch, obj);
			break;

		case ITEM_PORTAL:
			for (auto* pexit : ch->in_room->exits)
			{
				if (pexit->vdir == DIR_PORTAL && IS_SET(pexit->flags, EX_PORTAL))
				{
					if (room_is_private(ch, pexit->to_room) && get_trust(ch) < sysdata.level_override_private)
					{
						send_to_char(FB_WHITE, ch);
						send_to_char("To pomieszczenie jest przeznaczone dla oczu prywatnych." NL, ch);
						return;
					}
					original = ch->in_room;
					char_from_room(ch);
					char_to_room(ch, pexit->to_room);
					do_look(ch, (char*) "auto");
					char_from_room(ch);
					char_to_room(ch, original);
					send_to_char(PLAIN, ch);
					return;
				}
			}
			send_to_char("Widzisz wiruj�cy chaos..." NL, ch);
			break;
		case ITEM_CONTAINER:
		case ITEM_PAZAAK_SIDE_DECK:
		case ITEM_CORPSE_NPC:
		case ITEM_CORPSE_PC:
		case ITEM_DROID_CORPSE:
			if (IS_SET(obj->value[1], CONT_CLOSED))
			{
				ch_printf(ch, "%s %s %s zamkni�t%s." NL,
						obj->gender == GENDER_MALE ? "Ten" : obj->gender == GENDER_FEMALE ? "Ta" :
						obj->gender == GENDER_NEUTRAL ? "To" : "Te", obj->przypadki[0], obj->gender == GENDER_PLURAL ? "s�" : "jest",
						obj->gender == GENDER_MALE ? "y" : obj->gender == GENDER_FEMALE ? "a" : "e");
				break;
			}

			if (obj->item_type == ITEM_PAZAAK_SIDE_DECK)
			{
				send_to_pager(SWPazaak::listCards(ch->name).c_str(), ch);
			}
			else
			{
				count = obj->count;
				obj->count = 1;
				if (obj->gender == GENDER_PLURAL)
					act(PLAIN, "$p zawieraj�:", ch, obj, NULL, TO_CHAR);
				else
					act(PLAIN, "$p zawiera:", ch, obj, NULL, TO_CHAR);
				obj->count = count;

				show_list_to_char(obj->contents, ch, true, true);
				send_to_char(PLAIN, ch);
			}

			if (doProg)
				oprog_examine_trigger(ch, obj);
			break;
		}
		return;
	}

	for (auto* ed : ch->in_room->extradesc) /* Trog */
	{
		if (*ed->keyword && is_name_prefix(ed->keyword, arg1))
		{
			send_to_char(ed->description, ch);
			send_to_char(PLAIN, ch);
			if (doProg)
			{
				rprog_extradesc_trigger(ed, ch);
			}
			return;
		}
	}

	door = get_door(arg1);
	if ((pexit = find_door(ch, arg1, true)) != NULL)
	{
		if (pexit->keyword)
		{
			if (IS_SET(pexit->flags, EX_CLOSED) && !IS_SET(pexit->flags, EX_WINDOW))
			{
				if (IS_SET(pexit->flags, EX_SECRET) && door != -1)
					send_to_char("Nie dostrzegasz niczego specjalnego." NL, ch);
				else
					act(PLAIN, "$d s� zamkni�te.", ch, NULL, pexit->keyword,
					TO_CHAR);
				return;
			}
			if (IS_SET(pexit->flags, EX_BASHED))
				act(FB_RED, "$d zosta�y wyrwane z zawias�w!", ch, NULL, pexit->keyword, TO_CHAR);
		}

		if (pexit->description && pexit->description[0] != '\0')
			send_to_char(pexit->description, ch);
		else
			send_to_char("Nie dostrzegasz niczego specjalnego." NL, ch);

		/*
		 * Ability to look into the next room			-Thoric
		 */
		if (pexit->to_room && (IS_AFFECTED( ch, AFF_SCRYING ) || IS_SET(pexit->flags, EX_xLOOK) || get_trust(ch) >= LEVEL_IMMORTAL))
		{
			if (!IS_SET(pexit->flags, EX_xLOOK) && get_trust(ch) < LEVEL_IMMORTAL)
			{
				send_to_char(COL_FORCE, ch);
				send_to_char("Pr�bujesz przeczu� co jest w �rodku..." NL, ch);
				/* Change by Narn, Sept 96 to allow characters who don't have the
				 scry spell to benefit from objects that are affected by scry.
				 */
				if (!IS_NPC(ch))
				{
					int percent = ch->pcdata->learned[skill_lookup("scry")];
					if (!percent)
						percent = 99;

					if (number_percent() > percent)
					{
						send_to_char("Nie uda�o ci si�." NL, ch);
						return;
					}
				}
			}
			if (room_is_private(ch, pexit->to_room) && get_trust(ch) < sysdata.level_override_private)
			{
				send_to_char(PLAIN, ch);
				send_to_char("To pomieszczenie jest przeznaczone dla oczu prywatnych." NL, ch);
				return;
			}
			original = ch->in_room;
			if (pexit->distance > 1)
			{
				ROOM_INDEX_DATA *to_room;
				if ((to_room = generate_exit(ch->in_room, &pexit)) != NULL)
				{
					char_from_room(ch);
					char_to_room(ch, to_room);
				}
				else
				{
					char_from_room(ch);
					char_to_room(ch, pexit->to_room);
				}
			}
			else
			{
				char_from_room(ch);
				char_to_room(ch, pexit->to_room);
			}
			do_look(ch, (char*) "auto");
			char_from_room(ch);
			char_to_room(ch, original);
		}
		send_to_char(PLAIN, ch);
		return;
	}
	else if (door != -1)
	{
		send_to_char("Nie dostrzegasz niczego specjalnego." NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg1)) != NULL)
	{
		quest_trigger(ch, EVENT_LOOK_MOB, NULL, victim);
		show_char_to_char_1(victim, ch);
		send_to_char(PLAIN, ch);
		return;
	}

	if ((ship = ship_in_room(ch->in_room, arg1)))
	{
		show_ship_to_char(ship, ch);
		return;
	} //byTrog, modified by Thanos: get_ship zmienione na ship_in_room

	/* finally fixed the annoying look 2.obj desc bug	-Thoric */
	number = number_argument(arg1, arg);
	cnt = 0;
	for (auto rit = ch->carrying.rbegin(); rit != ch->carrying.rend(); ++rit)
	{
		OBJ_DATA *obj = *rit;
		if (can_see_obj(ch, obj))
		{
			if ((pdesc = get_extra_descr(arg, obj->extradesc)) != NULL)
			{
				if ((cnt += obj->count) < number)
					continue;
				quest_trigger(ch, EVENT_LOOK_OBJ, obj, NULL);
				send_to_char(pdesc, ch);
				if (doProg)
					oprog_examine_trigger(ch, obj);
				return;
			}

			if ((pdesc = get_extra_descr(arg, obj->pIndexData->extradesc)) != NULL)
			{
				if ((cnt += obj->count) < number)
					continue;
				quest_trigger(ch, EVENT_LOOK_OBJ, obj, NULL);
				send_to_char(pdesc, ch);
				if (doProg)
					oprog_examine_trigger(ch, obj);
				return;
			}

			if (nifty_is_name_prefix(arg, obj->name))
			{
				if ((cnt += obj->count) < number)
					continue;
				quest_trigger(ch, EVENT_LOOK_OBJ, obj, NULL);

				pdesc = get_extra_descr(obj->name, obj->pIndexData->extradesc);
				if (!pdesc)
					pdesc = get_extra_descr(obj->name, obj->extradesc);
				if (!pdesc)
					send_to_char("Nie dostrzegasz niczego specjalnego.\r\n", ch);
				else
					send_to_char(pdesc, ch);
				if (doProg)
					oprog_examine_trigger(ch, obj);
				return;
			}
		}
	}

	for (auto rit = ch->in_room->contents.rbegin(); rit != ch->in_room->contents.rend(); ++rit)
	{
		OBJ_DATA *obj = *rit;
		if (can_see_obj(ch, obj))
		{
			if ((pdesc = get_extra_descr(arg, obj->extradesc)) != NULL)
			{
				if ((cnt += obj->count) < number)
					continue;
				quest_trigger(ch, EVENT_LOOK_OBJ, obj, NULL);
				send_to_char(pdesc, ch);
				if (doProg)
					oprog_examine_trigger(ch, obj);
				return;
			}

			if ((pdesc = get_extra_descr(arg, obj->pIndexData->extradesc)) != NULL)
			{
				if ((cnt += obj->count) < number)
					continue;
				quest_trigger(ch, EVENT_LOOK_OBJ, obj, NULL);
				send_to_char(pdesc, ch);
				if (doProg)
					oprog_examine_trigger(ch, obj);
				return;
			}
			if (nifty_is_name_prefix(arg, obj->name))
			{
				if ((cnt += obj->count) < number)
					continue;
				quest_trigger(ch, EVENT_LOOK_OBJ, obj, NULL);
				pdesc = get_extra_descr(obj->name, obj->pIndexData->extradesc);
				if (!pdesc)
					pdesc = get_extra_descr(obj->name, obj->extradesc);
				if (!pdesc)
					send_to_char("Nie dostrzegasz niczego specjalnego.\r\n", ch);
				else
					send_to_char(pdesc, ch);
				if (doProg)
					oprog_examine_trigger(ch, obj);
				return;
			}
		}
	}

	send_to_char("Nie widzisz tego tutaj." NL, ch);
	return;
}

void show_condition(CHAR_DATA *ch, CHAR_DATA *victim)
{
	char buf[MAX_STRING_LENGTH];
	int percent;

	if (victim->max_hit > 0)
		percent = (100 * victim->hit) / victim->max_hit;
	else
		percent = -1;

	strcpy(buf, PERS(victim, ch, 0));

	if (IS_NPC ( victim ) && IS_SET(victim->act, ACT_DROID))
	{

		if (percent >= 100)
			strcat(buf, " jest w doskona�ym stanie." NL);
		else if (percent >= 90)
			strcat(buf, " ma lekko zadrapany pancerz." NL);
		else if (percent >= 80)
			strcat(buf, " ma kilka dra�ni�� na obudowie." NL);
		else if (percent >= 70)
			strcat(buf, " ma kilka naci�� w obudowie." NL);
		else if (percent >= 60)
			strcat(buf, " ma kilka dziur w pancerzu." NL);
		else if (percent >= 50)
			strcat(buf, " ma lekko pogruchotany pancerz." NL);
		else if (percent >= 40)
			strcat(buf, " ma powyginany pancerz." NL);
		else if (percent >= 30)
			strcat(buf, " ocieka olejem." NL);
		else if (percent >= 20)
			strcat(buf, " dymi." NL);
		else if (percent >= 10)
			strcat(buf,
					MALE(victim) ? " jest prawie niesprawny." NL :
					FEMALE(victim) ? " jest prawie niesprawna." NL : " jest prawie niesprawne." NL);
		else
			strcat(buf,
					MALE(victim) ? " jest bliski EKSPLOZJI." NL :
					FEMALE(victim) ? " jest bliska EKSPLOZJI." NL : " jest bliskie EKSPLOZJI." NL);

	}
	else
	{

		if (percent >= 100)
			strcat(buf, " jest w doskona�ym zdrowiu." NL);
		else if (percent >= 90)
			strcat(buf, " ma kilka zadrapa�." NL);
		else if (percent >= 80)
			strcat(buf, " ma kilka dra�ni��." NL);
		else if (percent >= 70)
			strcat(buf, " ma kilka naci��." NL);
		else if (percent >= 60)
			strcat(buf, " ma kilka ran." NL);
		else if (percent >= 50)
			strcat(buf, " ma wiele brzydkich ran." NL);
		else if (percent >= 40)
			strcat(buf, " krwawi." NL);
		else if (percent >= 30)
			strcat(buf, " krwawi obficie." NL);
		else if (percent >= 20)
			strcat(buf, " broczy posok�." NL);
		else if (percent >= 10)
			strcat(buf, " ledwie trzyma si� na nogach." NL);
		else
			strcat(buf, " UMIERA." NL);

	}
	buf[0] = UPPER(buf[0]);
	send_to_char("&z&w", ch);
	send_to_char(buf, ch);
	return;
}

/* A much simpler version of look, this function will show you only
 the condition of a mob or pc, or if used without an argument, the
 same you would see if you enter the room and have config +brief.
 -- Narn, winter '96
 */
DEF_DO_FUN( glance )
{
	char arg1[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	int save_act;

	if (!ch->desc)
		return;

	if (ch->position < POS_SLEEPING)
	{
		send_to_char("Widzisz tylko gwiazdy... Ale� one pi�kne!" NL, ch);
		return;
	}

	if (ch->position == POS_SLEEPING)
	{
		send_to_char("Nic nie widzisz, przecie� �pisz!" NL, ch);
		return;
	}

	if (!check_blind(ch))
		return;

	argument = one_argument(argument, arg1);

	if (arg1[0] == '\0')
	{
		save_act = ch->act;
		SET_BIT(ch->act, PLR_BRIEF);
		do_look(ch, (char*) "auto");
		ch->act = save_act;
		return;
	}

	if ((victim = get_char_room(ch, arg1)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego.", ch);
		return;
	}
	else
	{
		if (can_see(victim, ch))
		{
			act(COL_ACTION, "$n zerka na ciebie.", ch, NULL, victim, TO_VICT);
			act(COL_ACTION, "$n zerka na $N$3.", ch, NULL, victim, TO_NOTVICT);
		}

		show_condition(ch, victim);
		if (!IS_NPC(ch) && IS_NPC(victim))
		{
			ch->pcdata->remembered_mob_bounty = victim->pIndexData->vnum;
		}
	}

	return;
}

DEF_DO_FUN( examine )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	BOARD_DATA *board;
	int dam;

	IF_BUG(ch == NULL, "")
		return;

	IF_BUG(argument == NULL, "(ch=%s)", ch->name)
		return;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Zajrze� do czego?" NL, ch);
		return;
	}

	do_look(ch, arg);

	/*
	 * Support for looking at boards, checking equipment conditions,
	 * and support for trigger positions by Thoric
	 */
	if ((obj = get_obj_here(ch, arg)) != NULL)
	{
		if ((board = get_board(obj)) != NULL)
		{
			if (board->num_posts)
				ch_printf(ch, "Jest w nim oko�o %d pos�anych not. Wpisz 'note list' by zobaczy� ich list�." NL, board->num_posts);
			else
				send_to_char("Nie zawiera �adnych not." NL, ch);
		}

		switch (obj->item_type)
		{
		default:
			break;

		case ITEM_ARMOR:
			if (obj->value[1] == 0)
				obj->value[1] = obj->value[0];
			if (obj->value[1] == 0)
				obj->value[1] = 1;
			dam = (int) ((obj->value[0] * 10) / obj->value[1]);
			strcpy(buf, "Kiedy przygl�dasz si� uwa�niej, spostrzegasz, �e ");
			if (dam >= 10)
				strcat(buf, "jest w doskona�ym stanie.");
			else if (dam == 9)
				strcat(buf, "jest w bardzo dobrym stanie.");
			else if (dam == 8)
				strcat(buf, "jest w dobrym stanie.");
			else if (dam == 7)
				strcat(buf, "jest kr�tko u�ywany.");
			else if (dam == 6)
				strcat(buf, "jest lekko zu�yty.");
			else if (dam == 5)
				strcat(buf, "potrzebuje naprawy.");
			else if (dam == 4)
				strcat(buf, "bardzo potrzebuje naprawy.");
			else if (dam == 3)
				strcat(buf, "bez naprawy si� nie obejdzie.");
			else if (dam == 2)
				strcat(buf, "jest w okropnym stanie.");
			else if (dam == 1)
				strcat(buf, "jest praktycznie bezu�yteczny.");
			else if (dam <= 0)
				strcat(buf, "jest zepsuty.");
			strcat(buf, NL);
			send_to_char(buf, ch);
			break;

		case ITEM_WEAPON:
			dam = obj->value[0];
			strcpy(buf, "Kiedy przygl�dasz si� uwa�niej, spostrzegasz, �e ");
			if (dam >= 10)
				strcat(buf, "jest w doskona�ym stanie.");
			else if (dam == 9)
				strcat(buf, "jest w bardzo dobrym stanie.");
			else if (dam == 8)
				strcat(buf, "jest w dobrym stanie.");
			else if (dam == 7)
				strcat(buf, "jest kr�tko u�ywany.");
			else if (dam == 6)
				strcat(buf, "jest lekko zu�yty.");
			else if (dam == 5)
				strcat(buf, "potrzebuje naprawy.");
			else if (dam == 4)
				strcat(buf, "bardzo potrzebuje naprawy.");
			else if (dam == 3)
				strcat(buf, "bez naprawy rozsupie si� wkr�tce.");
			else if (dam == 2)
				strcat(buf, "jest w okropnym stanie.");
			else if (dam == 1)
				strcat(buf, "jest praktycznie bezu�yteczny.");
			else if (dam <= 0)
				strcat(buf, "jest zepsuty.");
			strcat(buf, NL);
			send_to_char(buf, ch);
			if (obj->value[3] == WEAPON_BLASTER)
			{
				if (obj->blaster_setting == BLASTER_FULL)
					ch_printf(ch, "Dysponuje PE�N� moc�." NL);
				else if (obj->blaster_setting == BLASTER_HIGH)
					ch_printf(ch, "Dysponuje DU�� ILO�CI� mocy." NL);
				else if (obj->blaster_setting == BLASTER_NORMAL)
					ch_printf(ch, "Dysponuje NORMALN� ILO�CI� mocy." NL);
				else if (obj->blaster_setting == BLASTER_HALF)
					ch_printf(ch, "Dysponuje PO�OW� mocy." NL);
				else if (obj->blaster_setting == BLASTER_LOW)
					ch_printf(ch, "Dysponuje MA�� ILO�CI� mocy." NL);
				else if (obj->blaster_setting == BLASTER_STUN)
					ch_printf(ch, "Jest ustawiony na OG�USZANIE." NL);
				ch_printf(ch, "Zosta�o mu %d do %d strza��w." NL, obj->value[4] / 5, obj->value[4]);
			}
			else if ((obj->value[3] == WEAPON_LIGHTSABER || obj->value[3] == WEAPON_VIBRO_BLADE || obj->value[3] == WEAPON_FORCE_PIKE))
			{
				ch_printf(ch, "Ma jeszcze %d/%d �adunk�w." NL, obj->value[4], obj->value[5]);
			}
			break;

		case ITEM_FOOD:
			if (obj->timer > 0 && obj->value[1] > 0)
				dam = (obj->timer * 10) / obj->value[1];
			else
				dam = 10;
			strcpy(buf, "Kiedy przygl�dasz si� uwa�niej zauwa�asz, �e ");
			if (dam >= 10)
				strcat(buf, "jest �wie�y.");
			else if (dam == 9)
				strcat(buf, "jest prawie �wie�y.");
			else if (dam == 8)
				strcat(buf, "wygl�da bardzo apetycznie.");
			else if (dam == 7)
				strcat(buf, "wygl�da dobrze.");
			else if (dam == 6)
				strcat(buf, "wygl�da ok.");
			else if (dam == 5)
				strcat(buf, "jest lekko nie�wie�y.");
			else if (dam == 4)
				strcat(buf, "jest troch� nie�wie�y.");
			else if (dam == 3)
				strcat(buf, "lekko zalatuje.");
			else if (dam == 2)
				strcat(buf, "troch� �mierdzi.");
			else if (dam == 1)
				strcat(buf, "�mierdzi.");
			else if (dam <= 0)
				strcat(buf, "jest pokryty drobinkami ple�ni i mocno �mierdzi.");
			strcat(buf, NL);
			send_to_char(buf, ch);
			break;

		case ITEM_SWITCH:
		case ITEM_LEVER:
		case ITEM_PULLCHAIN:
			if (IS_SET(obj->value[0], TRIG_UP))
				send_to_char("Zauwa�asz, �e jest wy��czony." NL, ch);
			else
				send_to_char("Zauwa�asz, �e jest w��czony." NL, ch);
			break;
		case ITEM_BUTTON:
			if (IS_SET(obj->value[0], TRIG_UP))
				send_to_char("Zauwa�asz, �e jest wci�ni�ty." NL, ch);
			else
				send_to_char("Zauwa�asz, �e jest wyci�ni�ty." NL, ch);
			break;

		case ITEM_CORPSE_PC:
		case ITEM_CORPSE_NPC:
		{
			int timerfrac = obj->timer;
			if (obj->item_type == ITEM_CORPSE_PC)
				timerfrac = (int) obj->timer / 8 + 1;

			switch (timerfrac)
			{
			default:
				send_to_char("Cia�o jest jeszcze ciep�e." NL, ch);
				break;
			case 4:
				send_to_char("Cia�o le�y tu ju� od jakiego� czasu." NL, ch);
				break;
			case 3:
				send_to_char("Cia�o zalatuje st�chlizn�." NL, ch);
				break;
			case 2:
				send_to_char("Cia�o jest ju� tak roz�o�one, �e ci�ko b�dzie obok niego przej��." NL, ch);
				break;
			case 1:
			case 0:
				send_to_char("To ju� prawie same ko�ci. I jeszcze ten smr�d!" NL, ch);
				break;
			}
		}
			if (IS_OBJ_STAT(obj, ITEM_COVERING))
				break;
			send_to_char("Kiedy zagl�dasz do �rodka widzisz:" NL, ch);
			sprintf(buf, "in %s noprog", arg);
			do_look(ch, buf);
			break;

		case ITEM_DROID_CORPSE:
		{
			int timerfrac = obj->timer;

			switch (timerfrac)
			{
			default:
				send_to_char("Te resztki jeszcze si� dymi�." NL, ch);
				break;
			case 4:
				send_to_char("Cz�ci ju� ca�kowicie wystyg�y." NL, ch);
				break;
			case 3:
				send_to_char("Cz�ci droida zacz�� ju� okrywa� kurz." NL, ch);
				break;
			case 2:
				send_to_char("Resztki sa ju� kompletnie zardzewia�e." NL, ch);
				break;
			case 1:
			case 0:
				send_to_char("Jedyne co pozosta�o to kupka rdzy." NL, ch);
				break;
			}
		}
			break;

		case ITEM_CONTAINER:
			if (IS_OBJ_STAT(obj, ITEM_COVERING))
				break;
				/* no break: to prawdopodobnie jest zle??? */

		case ITEM_DRINK_CON:
			send_to_char("Kiedy zagl�dasz do �rodka widzisz:" NL, ch);
			sprintf(buf, "in %s noprog", arg);
			do_look(ch, buf);
		}
		if (IS_OBJ_STAT(obj, ITEM_COVERING))
		{
			sprintf(buf, "under %s noprog", arg);
			do_look(ch, buf);
		}
		oprog_examine_trigger(ch, obj);
		if (char_died(ch) || obj_extracted(obj))
			return;

		check_for_trap(ch, obj, TRAP_EXAMINE);
	}
	return;
}

//added by Thanos (upodobni�em wygl�d wyj�� przy AUTOEXIT do ciapkowego )
DEF_DO_FUN( exits )
{
	char buf[MAX_STRING_LENGTH];
	bool found;
	bool fAuto;
	bool fMaps;
	bool mapDir[10];

	buf[0] = '\0';
	fAuto = !str_cmp(argument, "auto");
	fMaps = !str_cmp(argument, "maps");

	if (!check_blind(ch))
		return;

	strcpy(buf, fAuto ? PLAIN "( " FG_CYAN "Wyj�cia: " PLAIN : FB_CYAN "Dost�pne wyj�cia:" EOL FG_CYAN);

	for (int i = 0; i < 10; i++)
		mapDir[i] = false;

	found = false;
	for (auto* pexit : ch->in_room->exits)
	{
		if (pexit->to_room && CAN_ENTER(ch, pexit->to_room) && (!IS_SET(pexit->flags, EX_HIDDEN) || IS_IMMORTAL(ch)))
		{
			if (fMaps)
			{
				switch (pexit->vdir)
				{
				case DIR_NORTH:
					mapDir[0] = true;
					break;
				case DIR_EAST:
					mapDir[1] = true;
					break;
				case DIR_SOUTH:
					mapDir[2] = true;
					break;
				case DIR_WEST:
					mapDir[3] = true;
					break;
				case DIR_UP:
					mapDir[4] = true;
					break;
				case DIR_DOWN:
					mapDir[5] = true;
					break;
				case DIR_NORTHEAST:
					mapDir[6] = true;
					break;
				case DIR_NORTHWEST:
					mapDir[7] = true;
					break;
				case DIR_SOUTHEAST:
					mapDir[8] = true;
					break;
				case DIR_SOUTHWEST:
					mapDir[9] = true;
					break;
				}
			}

			if (!fAuto)
			{
				if (IS_SET(pexit->flags, EX_CLOSED))
				{
					sprintf(buf + strlen(buf),
					FG_CYAN "%-5s - (zamkni�te)" EOL, capitalize(dir_name[pexit->vdir]));
				}
				else if (IS_SET(pexit->flags, EX_WINDOW))
				{
					sprintf(buf + strlen(buf), FG_CYAN "%-5s - (okno)" EOL, capitalize(dir_name[pexit->vdir]));
				}
				else if (IS_SET(pexit->flags, EX_xAUTO))
				{
					sprintf(buf + strlen(buf), FG_CYAN "%-5s - %s" EOL, capitalize(pexit->keyword),
							room_is_dark(pexit->to_room) ? "Tam jest zbyt ciemno" : pexit->to_room->name);
				}
				else
					sprintf(buf + strlen(buf), FG_CYAN "%-5s - %s" EOL, capitalize(dir_name[pexit->vdir]),
							room_is_dark(pexit->to_room) ? "Tam jest zbyt ciemno" : pexit->to_room->name);
			}
			else
			{
				if (found)
					strcat(buf, " ");

				if ((IS_IMMORTAL( ch ) || IS_OLCMAN(ch)) && can_edit(ch, ch->in_room->vnum) && IS_SET(ch->act, PLR_ROOMVNUM))
					sprintf(buf + strlen(buf),
					FG_YELLOW "%s" PLAIN "%s%s" FG_YELLOW "%d" PLAIN, dir_name[pexit->vdir],
					IS_SET(pexit->flags, EX_WINDOW) ? FG_GREEN "##" PLAIN : IS_SET(pexit->flags, EX_CLOSED) ? FG_GREEN "][" PLAIN : PLAIN,
							(get_exit(pexit->to_room, rev_dir[pexit->vdir])
									&& get_exit(pexit->to_room, rev_dir[pexit->vdir])->vnum == ch->in_room->vnum) ? "->" : "@>",
							pexit->to_room->vnum);
				else
					sprintf(buf + strlen(buf),
					FG_YELLOW "%s" PLAIN "%s", dir_name[pexit->vdir],
					IS_SET(pexit->flags, EX_WINDOW) ? FG_GREEN "##" PLAIN : IS_SET(pexit->flags, EX_CLOSED) ? FG_GREEN "][" PLAIN : PLAIN);

			}
			found = true;
		}
	}

	if (!found)
		strcat(buf, fAuto ? FG_YELLOW "Brak" PLAIN " )" EOL : "Brak" EOL);
	else if (fAuto)
		strcat(buf, " )" EOL);

	if (fMaps)
	{
		sprintf(buf, "%s %s %s" EOL, mapDir[7] ? "\\" : " ", mapDir[0] ? "|" : " ", mapDir[6] ? "/" : " ");
		sprintf(buf + strlen(buf), " %s%s%s      %s" EOL, mapDir[7] ? "\\" : " ", mapDir[0] ? "|" : " ", mapDir[6] ? "/" : " ",
				mapDir[4] ? "/\\ (G�ra)" : " ");
		sprintf(buf + strlen(buf), "%so%s     %s" EOL, mapDir[3] ? "==" : "  ", mapDir[1] ? "==" : "  ", mapDir[5] ? "\\/ (D�)" : " ");
		sprintf(buf + strlen(buf), " %s%s%s " EOL, mapDir[9] ? "/" : " ", mapDir[2] ? "|" : " ", mapDir[8] ? "\\" : " ");
		sprintf(buf + strlen(buf), "%s %s %s" EOL, mapDir[9] ? "/" : " ", mapDir[2] ? "|" : " ", mapDir[8] ? "\\" : " ");
	}
	send_to_char(buf, ch);
	send_to_char(PLAIN, ch);
	return;
}

DEF_DO_FUN( time )
{
	PLANET_DATA *pPlanet = ch->in_room->area->planet;

	if (pPlanet)
	{
		char buf[MSL];

		switch (pPlanet->sunlight)
		{
		default:
			sprintf(buf, "(ni to dzie� ni noc)");
			bug("unknown sunlight at %s", ch->name);
			break;
		case SUN_LIGHT:
			sprintf(buf, "(dzie�)");
			break;
		case SUN_RISE:
			sprintf(buf, "(poranek)");
			break;
		case SUN_SET:
			sprintf(buf, "(wiecz�r)");
			break;
		case SUN_DARK:
			sprintf(buf, "(noc)");
			break;
		}

		if (pPlanet != planet_list.front())
			ch_printf(ch, "Na %s jest godzina %d %s, %d %s czasu standardowego Coruscant." NL
			"Mamy %d dzie� miesi�ca %d, %s." NL, pPlanet->name, pPlanet->hour, buf,
					(planet_list.front()->hour % 12 == 0) ? 12 : planet_list.front()->hour % 12,
					planet_list.front()->hour == 12 ? "w po�udnie" : planet_list.front()->hour < 3 ? "w nocy" : planet_list.front()->hour < 6 ? "nad ranem" :
					planet_list.front()->hour < 12 ? "rano" : planet_list.front()->hour < 19 ? "po po�udniu" : "wieczorem", pPlanet->day, pPlanet->month,
					pPlanet->curr_season->name);
		else
			ch_printf(ch, "Na %s jest godzina %d %s czasu standardowego." NL
			"Mamy %d dzie� miesi�ca %d, %s." NL, pPlanet->name, (planet_list.front()->hour % 12 == 0) ? 12 : planet_list.front()->hour % 12,
					planet_list.front()->hour == 12 ? "w po�udnie" : planet_list.front()->hour < 3 ? "w nocy" : planet_list.front()->hour < 6 ? "nad ranem" :
					planet_list.front()->hour < 12 ? "rano" : planet_list.front()->hour < 19 ? "po po�udniu" : "wieczorem", pPlanet->day, pPlanet->month,
					pPlanet->curr_season->name);

		ch_printf(ch, "O tej porze roku dzie� trwa tu %d godzi%s, a noc %d." NL, pPlanet->curr_season->day_length,
				NUMBER_SUFF(pPlanet->curr_season->day_length, "n�", "ny", "n"), pPlanet->curr_season->night_length);
	}
	else
		ch_printf(ch, "Jest godzina %d czasu standardowego (Coruscant)." NL, planet_list.front()->hour);

	ch_printf(ch, NL);

	SWDate *bootTime = IS_IMMORTAL(ch) ? new SWTimeStamp(boot_time) : new SWDate(boot_time);
	ch_printf(ch, "SW-Mud wystartowa�:       %s" NL, bootTime->getDescriptive().c_str());
	delete bootTime;
	ch_printf(ch, "Czas systemowy (E.S.T.):  %s" NL, SWTimeStamp(current_time).getDescriptive().c_str());

	bootTime = IS_IMMORTAL(ch) ? new SWTimeStamp(new_boot_time) : new SWDate(new_boot_time);
	if (sysdata.reboot_type == REB_COPYOVER)
		ch_printf(ch, "Gor�cy reboot b�dzie:     %s" NL, bootTime->getDescriptive().c_str());
	else if (sysdata.reboot_type == REB_SHUTDOWN)
		ch_printf(ch, "Shutdown b�dzie:          %s" NL, bootTime->getDescriptive().c_str());
	else
		ch_printf(ch, "Reboot b�dzie:            %s" NL, bootTime->getDescriptive().c_str());
	delete bootTime;
}

DEF_DO_FUN( weather )
{
	static const char *const sky_look[8] =
	{
	/* ciep�o */
	"bezchmurne", /* SKY_CLOUDLESS */
	"pochmurne", /* SKY_CLOUDY */
	"deszczowe", /* SKY_RAINING */
	"poprzecinane b�yskawicami", /* SKY_LIGHTNING */
	/* zimno */
	"bezchmurne", /* SKY_CLOUDLESS */
	"zapruszone p�atkami �niegu", /* SKY_CLOUDY */
	"wype�nione �niegiem", /* SKY_RAINING */
	"zakryte przez burz� snie�n�" /* SKY_LIGHTNING */
	};
	int wind;
	PLANET_DATA *pPlanet = ch->in_room->area->planet;

	if (!IS_OUTSIDE(ch) || !pPlanet)
	{
		send_to_char("Nie mo�esz sprawdzi� pogody tutaj." NL, ch);
		return;
	}

	wind = pPlanet->windspeed;

	if (*pPlanet->curr_season->name)
		ch_printf(ch, "Na %s jest teraz %s." NL, pPlanet->name, pPlanet->curr_season->name);

	ch_printf(ch, "Niebo jest %s, ", (pPlanet->temperature <= 4) ? sky_look[pPlanet->sky + 4] : sky_look[pPlanet->sky]);

	ch_printf(ch, "jest %s, %d stop%s",
			(pPlanet->temperature < -20) ? "potwornie zimno" : (pPlanet->temperature < -10) ? "bardzo zimno" :
			(pPlanet->temperature < 0) ? "zimno" : (pPlanet->temperature < 10) ? "ch�odno" : (pPlanet->temperature < 20) ? "ciep�o" :
			(pPlanet->temperature < 30) ? "gor�co" : "piekielnie gor�co", pPlanet->temperature,
			NUMBER_SUFF(pPlanet->temperature, "ie�", "nie", "ni"));

	if (wind)
		ch_printf(ch, "." NL "%s wieje %s." NL, capitalize(dir_rev_name[abs(pPlanet->winddir) % 3]),
				wind <= 20 ? "delikatny wietrzyk" : wind <= 50 ? "wiaterek" : wind <= 80 ? "wiatr" : "huragan");
	else
		ch_printf(ch, "." NL);

	if (IS_IMMORTAL(ch))
	{
		ch_printf(ch, "Pressure: %d, Wind: %d Winddir: %d" NL, pPlanet->mmhg, wind, pPlanet->winddir);
	}
	return;
}

/*
 * Moved into a separate function so it can be used for other things
 * ie: online help editing				-Thoric
 */
HELP_DATA* get_help(CHAR_DATA *ch, char *argument)
{
	char argall[MAX_INPUT_LENGTH];
	char argone[MAX_INPUT_LENGTH];
	char argnew[MAX_INPUT_LENGTH];
	int lev;

	if (argument[0] == '\0')
		argument = (char*) "help"; //changed by Thanos (zamiast summary jest help)

	if (isdigit(argument[0]))
	{
		lev = number_argument(argument, argnew);
		argument = argnew;
	}
	else
		lev = -2;
	/*
	 * Tricky argument handling so 'help a b' doesn't match a.
	 */
	argall[0] = '\0';
	while (argument[0] != '\0')
	{
		argument = one_argument(argument, argone);
		if (argall[0] != '\0')
			strcat(argall, " ");
		strcat(argall, argone);
	}

	for (auto* fHelp : helps_file_list)
		for (auto* pHelp : fHelp->helps)
		{
			if (pHelp->level > get_trust(ch))
				continue;
			if (lev != -2 && pHelp->level != lev)
				continue;
			//changed by Thanos (is_name / is_name_prefix)
			if (is_name_prefix(argall, pHelp->keyword))
				return pHelp;
		}

	return NULL;
}

/*
 *  Similar help Keywords  -- Added by Thanos from SW Rise In Power
 *
 *  Ranks by number of matches between two whole words. Coded for the Similar Helpfiles
 *  Snippet by Senir.
 */
int str_similarity(const char *astr, const char *bstr)
{
	int matches = 0;

	if (!astr || !bstr)
		return matches;

	for (; *astr; astr++)
	{
		if (LOWER(*astr) == LOWER(*bstr))
			matches++;

		if (*++bstr == '\0')
			return matches;
	}

	return matches / 2;
}

/*
 *  Ranks by number of matches until there's a nonmatching character between two words.
 *  Coded for the Similar Helpfiles Snippet by Senir.
 */
int str_prefix_level(const char *astr, const char *bstr)
{
	int matches = 0;

	if (!astr || !bstr)
		return matches;

	for (; *astr; astr++)
	{
		if (LOWER(*astr) == LOWER(*bstr))
			matches++;
		else
			return matches;

		if (*++bstr == '\0')
			return matches;
	}

	return matches / 2;
}

/*
 * Main function of Similar Helpfiles Snippet by Senir. It loops through all of the
 * helpfiles, using the string matching function defined to find the closest matching
 * helpfiles to the argument. It then checks for singles. Then, if matching helpfiles
 * are found at all, it loops through and prints out the closest matching helpfiles.
 * If its a single(there's only one), it opens the helpfile.
 */
void similar_help_files(CHAR_DATA *ch, char *argument)
{
	char buf[MAX_STRING_LENGTH];
	int lvl = 0;
	char *extension;
	int count = 0;

	send_to_pager_color("Oto lista s��w, kt�re mog� by� tymi, kt�rych szukasz:" NL, ch);

	for (auto* fHelp : helps_file_list)
		for (auto* pHelp : fHelp->helps)
		{
			buf[0] = '\0';
			extension = pHelp->keyword;

			if (pHelp->level > get_trust(ch))
				continue;

			while (extension[0] != '\0')
			{
				extension = one_argument(extension, buf);

				if (str_similarity(argument, buf) > lvl)
				{
					lvl = str_similarity(argument, buf);
				}
			}
		}

	if (lvl == 0)
	{
		send_to_pager_color("Nie znaleziono nic, co mog�oby pasowa�." NL, ch);
		return;
	}

	for (auto* fHelp : helps_file_list)
		for (auto* pHelp : fHelp->helps)
		{
			buf[0] = '\0';
			extension = pHelp->keyword;

			while (extension[0] != '\0')
			{
				extension = one_argument(extension, buf);

				if (str_similarity(argument, buf) >= lvl && pHelp->level <= get_trust(ch))
				{
					pager_printf(ch, FG_CYAN "%d. %s" EOL, ++count, pHelp->keyword);
					break;
				}
			}
		}

	return;
}

//added by Thanos - nowy help, zwraca uwag� na powtarzaj�ce si� s�owa kluczowe
DEF_DO_FUN( help )
{
	char bufk[MAX_STRING_LENGTH];
	char bufh[MAX_STRING_LENGTH];
	int count = 0;
	int level = 200;
	bool search = true;

	if (!*argument)
		argument = (char*) "help";

	bufk[0] = '\0';
	bufh[0] = '\0';

	for (auto* fHelp : helps_file_list) { if (!search) break;
		for (auto* pHelp : fHelp->helps)
		{
			if ((pHelp->level > get_trust(ch) && pHelp->type == HELP_PHELP)
					|| (pHelp->type == HELP_OLCHELP && get_trust(ch) < 103 && !IS_OLCMAN(ch))
					|| (pHelp->type == HELP_WIZHELP && !IS_IMMORTAL(ch)))
				continue;

			/* czy gracz poda� precyzyjnie s�owo kluczowe */
			if (!str_cmp(argument, pHelp->keyword) || is_name(argument, pHelp->keyword))
			{
				count = 1;
				if (str_cmp(pHelp->keyword, "motd") && str_cmp(pHelp->keyword, "amotd") && str_cmp(pHelp->keyword, "imotd")
						&& str_cmp(pHelp->keyword, "nmotd") && str_cmp(pHelp->keyword, "help") && pHelp->level >= 0)
					sprintf(bufk, FG_CYAN "1. %s" EOL, pHelp->keyword);
				sprintf(bufh, "%s%s", pHelp->syntax[0] != '\0' ? pHelp->syntax : "", pHelp->text[0] == '.' ? pHelp->text + 1 : pHelp->text);
				level = pHelp->level;

				search = false;
				break;
			}

			/* a mo�e zna� tylko pocz�tek wyra�enia */
			if ((is_name_prefix(argument, pHelp->keyword)))
			{
				count++;
				if (pHelp->level >= 0)
				{ //wszystkie s�owa kluczowe do bufora K
					strcat(bufk, FG_CYAN);
					strcat(bufk, itoa(count));
					strcat(bufk, ". ");
					strcat(bufk, pHelp->keyword);
					strcat(bufk, EOL);
				}

				if (count == 1) //do bufora H tylko pierwsza pasuj�ca tre�� helpa
				{
					level = pHelp->level;
					if (pHelp->syntax[0] != '\0')
						strcat(bufh, pHelp->syntax);
					if (pHelp->text[0] == '.')
						strcat(bufh, pHelp->text + 1);
					else
						strcat(bufh, pHelp->text);
				}
			}
		}
	}

	if (count)
	{ /* okaza�o si�, �e temat jest obszerny */
		if (count > 1)
		{
			send_to_pager("Do podanego s�owa kluczowego pasuje wi�cej ni� jedno wyra�enie:" NL, ch);
			send_to_pager(bufk, ch);
		}
		else /* je�li wyszukiwanie zako�czy�o si� sukcesem... */
		if (count == 1)
		{
			if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SOUND))
				send_to_pager("!!SOUND(help)", ch);

			if (level < 0) /* <--  helpy specjalne , bo np.*/
				send_to_char(bufh, ch); /* umieraj�cy gracz nie ma juz pagera.*/
			else
				/* <--  helpy pozosta�e */
				send_to_pager(bufh, ch);
		}
		return;
	}
	send_to_pager("Brak pomocy na ten temat." NL, ch);
	/* mo�e jeszcze jest jaka� nadzieja :P */
	similar_help_files(ch, argument);
	return;
}
//done

/*
 * Show help topics in a level range				-Thoric
 * Idea suggested by Gorog
 * All totally changed by Trog :-)
 */
DEF_DO_FUN( hlist )
{
	HELPS_FILE *fHelp = 0;
	int min, max, minlimit, maxlimit, cnt;
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	bool by_file = false;
	bool by_prefix = false;
	bool np = false; /* is_name_prefix */

	maxlimit = IS_HELPMASTER( ch->name ) ? MAX_LEVEL : get_trust(ch);
	minlimit = maxlimit > 103 || IS_HELPMASTER(ch->name) ? -1 : 0;
	argument = one_argument(argument, arg1);

	if (*arg1 != '\0')
	{
		if (is_number(arg1))
		{
			argument = one_argument(argument, arg2);
			argument = one_argument(argument, arg3);

			min = URANGE(minlimit, atoi(arg1), maxlimit);
			if (*arg2 != '\0')
				max = URANGE(min, atoi(arg2), maxlimit);
			else
				max = maxlimit;

			if (*arg3 != '\0')
			{
				if (str_cmp(arg3, "all"))
				{
					for (auto* fHelp : helps_file_list)
						if (!str_cmp(arg3, fHelp->name))
						{
							by_file = true;
							break;
						}
					if (!by_file)
					{
						send_to_char("No such helps file found." NL, ch);
						return;
					}
				}

				if (*argument != '\0')
				{
					by_prefix = true;
					if (argument[0] == '-')
					{
						np = true;
						argument++;
					}
				}
			}
		}
		else
		{
			min = minlimit;
			max = maxlimit;

			if (str_cmp(arg1, "all"))
			{
				for (auto* fHelp : helps_file_list)
					if (!str_cmp(arg1, fHelp->name))
					{
						by_file = true;
						break;
					}
				if (!by_file)
				{
					send_to_char("No such helps file found." NL, ch);
					return;
				}
			}

			if (*argument != '\0')
			{
				by_prefix = true;
				if (argument[0] == '-')
				{
					np = true;
					argument++;
				}
			}
		}
	}
	else
	{
		min = minlimit;
		max = maxlimit;
	}

	pager_printf(ch, FB_WHITE "Help Topics in level range" PLAIN " %d"
	FB_WHITE " to" PLAIN " %d" FB_WHITE ":" EOL NL, min, max);

	if (by_file)
	{
		pager_printf(ch, MOD_BOLD "%s:" EOL, fHelp->name);
		cnt = 0;
		for (auto* help : fHelp->helps)
			if (help->level >= min && help->level <= max
					&& ((by_prefix && (np ? is_name_prefix(argument, help->keyword) : !str_prefix(argument, help->keyword))) || !by_prefix))
			{
				pager_printf(ch, FB_WHITE ".%3d" PLAIN " %s" NL, help->level, help->keyword);
				cnt++;
			}
	}
	else
		{ cnt = 0; for (auto* fHelp : helps_file_list)
		{
			pager_printf(ch, MOD_BOLD "%s:" EOL, fHelp->name);
			for (auto* help : fHelp->helps)
				if (help->level >= min && help->level <= max
						&& ((by_prefix && (np ? is_name_prefix(argument, help->keyword) : !str_prefix(argument, help->keyword)))
								|| !by_prefix))
				{
					pager_printf(ch, FB_WHITE ".%3d" PLAIN " %s" NL, help->level, help->keyword);
					++cnt;
				}
		}
		}

	if (cnt)
		pager_printf(ch, NL "%d help pages found." NL
		"Criteria: file: %s, %sprefix: %s." NL, cnt, by_file ? "yes" : "no", np ? "name_" : "", by_prefix ? "yes" : "no");
}

//Tanglor - zapamietujemy naszego znajomego
DEF_DO_FUN( remember )
{
	int count;
	char arg[MAX_INPUT_LENGTH];

	count = 0;
	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char(NL " Niedawno Ci si� przedstawiali:" NL, ch);
		for (auto* _friend : ch->known)
			if (_friend->is_remembered < 0)
			{
				send_to_char(_friend->name, ch);
				send_to_char(NL, ch);
				count++;
			}

		if (count == 0)
			send_to_char("Nikt Ci si� ostatnio nie przedstawia�" EOL, ch);
	}
	else
	{
		if (ch->kins == MAX_KNOWN(ch))
		{
			send_to_char("Tw�j umys� wiecej nie pomiesci" EOL NL, ch);
			return;
		}
		for (auto* _friend : ch->known)
			if (is_name_prefix(arg, _friend->name))
			{
				if (_friend->is_remembered < 0)
				{
					_friend->is_remembered = 1;
					ch_printf(ch, "%s - ju� %s nie zapomnisz", _friend->name, _friend->sex == SEX_MALE ? "go " : "jej ");
					ch->kins++;
				}
				else
					send_to_char("Przecie� t� osob� ju� znasz." NL, ch);
				return;
			}
	}
}

//Pixel - zapominanie
DEF_DO_FUN( forget )
{
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char(NL "Kogo chcesz zapomnie�?" NL, ch);
		return;
	}

	for (auto* _friend : ch->known)
		if (is_name_prefix(arg, _friend->name))
		{
			if (_friend->is_remembered == 1)
			{
				_friend->is_remembered = 0;
				ch_printf(ch, "%s ? Istnia� kto� taki?", _friend->name);
				ch->kins--;
			}
			else
			{
				send_to_char("Nie znasz takiej osoby." NL, ch);
				return;
			}
		}
}

//Tanglor - lista osob ktore juz znamy i zapmietalismy
DEF_DO_FUN( kin )
{
	send_to_char(NL " Oto lista Twoich znajomych" NL, ch);
	for (auto* _friend : ch->known)
		if (_friend->is_remembered > 0)
		{
			ch_printf(ch, " %s, %s" NL, _friend->name,
			//			_friend->race->przypadki[0]);
					" ");
		}
}
//Tanglor - przedstawiamy sie
DEF_DO_FUN( introduce )
{
	char arg[MIL];
	CHAR_DATA *someone;
	KNOWN_CHAR_DATA *_friend;
	ROOM_INDEX_DATA *room;

	if (!ch)
		return;
	//czy  w pomieszczeniu ktos jest?
	room = ch->in_room;
	if (room->people.empty())
		return;
	// w pomieszczeniu jest ktos poza toba
	if (room->people.size() == 1)
	{
		send_to_char("Nikogo poza Tob� tu nie ma" NL, ch);
		return;
	}

	if (IS_AFFECTED(ch, AFF_HIDE)) //jak z ukryciem ?
		REMOVE_BIT(ch->affected_by, AFF_HIDE);

	one_argument(argument, arg);

	//przedstawiamy sie wszystkim w lokacji
	if (arg[0] == '\0')
	{
		send_to_char(COL_SAY "Przedstawiasz si� wszystkim w pomieszczeniu" NL, ch);
		for (auto* someone : room->people)
		{
			//samemu sobie nie bedziemy sie przedstawia
			if (someone == ch)
				continue;
			//czy someone nas juz nie zna ?
			//if (does_knows(someone,ch) )
			//	continue;                 Mimo �e Trog mnie zna, mog� mu powiedzie�, �e jestem Pixel.

			//zdejmujemy tutaj mask, hide i cala reszte
			if (IS_AFFECTED(ch, AFF_HIDE))
				REMOVE_BIT(ch->affected_by, AFF_HIDE);
			if (IS_AFFECTED(ch, AFF_INVISIBLE) && !isDefelInvisible(ch))
				REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);

			CREATE(_friend, KNOWN_CHAR_DATA, 1);
			STRDUP(_friend->name, ch->name);
			_friend->race = ch->race;
			_friend->is_remembered = -1;
			_friend->sex = ch->sex;
			someone->known.push_back(_friend);
			//tutaj wypadaloby poinformowac gracza, ze 'ciemnowlosy, zielonooki
			//ktos sie przedstawil jako 'name
			const SWString &attrib = format_char_attribute(ch, 0);
			ch_printf(someone,
			COL_SAY "%s przedstawia si� wszystkim, jako %s." NL, fcapitalize(attrib.c_str()), ch->name);
		}
	}
	else
	{
		//szukamy w lokacji moba jak argument
		if ((someone = get_char_room(ch, arg)) == NULL)
		{
			send_to_char("Nie ma tu nikogo takiego" NL, ch);
			return;
		}

		if (does_knows(someone, ch) == false)
		{

			CREATE(_friend, KNOWN_CHAR_DATA, 1);
			STRDUP(_friend->name, ch->name);
			_friend->race = ch->race;
			_friend->is_remembered = -1;
			_friend->sex = ch->sex;
			someone->known.push_back(_friend);
			//informujemy gracza, ze 'ciemnowlosy, zielonooki' sie przedstawil
			const SWString &attrib = format_char_attribute(someone, 2);
			ch_printf(ch, "Przedstawiasz si� %s." NL, does_knows(ch, someone) ? someone->przypadki[2] : attrib.c_str());

			const SWString &attrib2 = format_char_attribute(ch, 0);
			ch_printf(someone, COL_SAY "%s przedstawia si� Tobie jako %s." NL, // Pixel: brakowa�o
					fcapitalize(attrib2.c_str()), ch->name);

		}
		for (auto* someone1 : room->people)
		{
			//samemu sobie nie bedziemy sie przedstawia
			if (someone1 == ch || someone1 == someone)
				continue;

			const SWString &attrib = format_char_attribute(ch, 0);
			sprintf(arg, "Widzisz jak %s przedstawia sie " NL, does_knows(someone1, ch) ? ch->przypadki[0] : attrib.c_str());
			const SWString &attrib2 = format_char_attribute(someone, 2);
			ch_printf(someone1, "%s%s." NL, arg, does_knows(someone1, someone) ? someone->przypadki[2] : attrib2.c_str());
		}
	}
}
//Tanglor - zmiana wizerunku
DEF_DO_FUN( changeattribute )
{
	char arg[MIL];
	char arg1[MIL];
	char buf[MSL];
	int index, index1, count, count1;
	bool wybor;
	RACE_DATA *pRace;

	//gracz musi posiadac nie zdefiniowane jeszcze opisy
	if (ch->attribute1 >= 0 && ch->attribute2 >= 0)
		return;

	wybor = false;
	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg1);
	if (ch->race == NULL)
		return;
	pRace = ch->race;
	//�adnego argumentu - pokazujemy liste kategorii
	if (arg[0] == '\0')
	{
		if ((ch->attribute1 >= 0) || (ch->attribute2 >= 0))
			send_to_char("Wybra�e� ju� jedn� z cech" EOL, ch);
		index1 = 0;
		send_to_char("Oto lista kategorii na jakie podzielone s�" NL
		"attrybuty opisujace wygl�d Twojej postaci" NL NL, ch);
		for (index = 0; index < MAX_DESC_TYPES; index++)
		{
			if (pRace->desc_restrictions[index] == 0)
				continue;
			index1++;
			sprintf(buf, "[" FG_YELLOW "%s" PLAIN "]", category_desc[index]);
			count = 18 - strlen(buf);
			for (; count > 0; count--)
				strcat(buf, " ");
			if ((index1) % 4 == 0)
				strcat(buf, NL);
			send_to_char(buf, ch);
		}
		if (index1 > 0)
		{
			send_to_char(NL, ch);
		}
	}
	else if (arg1[0] == '\0')
	{
		//czy podano prawidlowa kategorie
		for (index = 0;; index++)
		{
			if (!strcmp(category_desc[index], "Empty") || index >= MAX_DESC_TYPES)
			{
				send_to_char("Nie ma takiej kategorii" NL, ch);
				return;
			}
			if (pRace->desc_restrictions[index] == 0)
				continue;
			if (is_name_prefix(arg, (char*) category_desc[index]))
				break;
		}
		//pokazujemy liste cech w danej kategorii
		ch_printf(ch, "Wybrana kategoria to " FG_YELLOW "%s." EOL NL, category_desc[index]);
		count = 0;
		for (index1 = 0; index1 < 61; index1++)
		{
			if (pRace->desc_restrictions[index] == 0)
				continue;
			if (!IS_SET(pRace->desc_restrictions[index], (1 << (index1 + 1))))
				continue;

			if (desc_table[index][index1].type == index)
			{
				const SWString &attrib = char_attribute((desc_type) index, index1, ch->sex, 0);
				sprintf(buf, "%s", attrib.c_str());
				count1 = 20 - strlen(buf);
				if ((count + 1) % 4 != 0)
					for (; count1 > 0; count1--)
						strcat(buf, " ");
				else
					strcat(buf, NL);
				send_to_char(buf, ch);
				count++;
			}
		}
		send_to_char(NL, ch);
	}
	else
	//tutaj jest zatwierdzenie wyboru cechy gracza
	{
		//czy podano prawidlowa kategorie
		for (index = 0;; index++)
		{
			if (!strcmp(category_desc[index], "Empty") || index >= MAX_DESC_TYPES)
			{
				send_to_char("Nie ma takiej kategorii" NL, ch);
				return;
			}
			if (pRace->desc_restrictions[index] == 0)
				continue;
			if (is_name_prefix(arg, (char*) category_desc[index]))
				break;
		}
		for (index1 = 0; index1 < 61; index1++)
		{
			if (pRace->desc_restrictions[index] == 0)
				continue;
			if (!IS_SET(pRace->desc_restrictions[index], (1 << (index1 + 1))))
				continue;
			//tworzymy opis gracza
			const SWString &attrib = char_attribute((desc_type) index, index1, ch->sex, 0);
			sprintf(buf, " %s", attrib.c_str());

			if (desc_table[index][index1].type == index && is_name_prefix(arg1, buf))
			{
				int markIndex = -index * 100 - index1 - 1;
				int finalIndex = index * 100 + index1;
				//trzeba zapytac gracza czy to jest pewien wyboru
				//czyli zapamietac trzeba wybor
				if (ch->attribute1 == finalIndex || ch->attribute2 == finalIndex)
				{
					send_to_char("Juz masz taka cech� - wybierz inna jako druga" NL, ch);
					return;
				}
				//najpierw sprawdzamy czy to jest futro
				if (index == FUR)
				{
					if (ch->attribute2 == markIndex)
					{
						ch->attribute2 = finalIndex;
						wybor = true;
					}
					else
						ch->attribute2 = markIndex;
				}
				//teraz czy zadeklarowane zostalo dla pierwszego atrybutu
				else if (ch->attribute1 == markIndex)
				{
					ch->attribute1 = finalIndex;
					wybor = true;
				}
				else if (ch->attribute2 == markIndex)
				{
					ch->attribute2 = finalIndex;
					wybor = true;
				}
				else if (ch->attribute1 >= 0)
					ch->attribute2 = markIndex;
				else
					ch->attribute1 = markIndex;

				if (wybor)
				{
					send_to_char("Od teraz b�dziesz rozpoznawany jako :" NL, ch);
					const SWString &attrib = format_char_attribute(ch, 0);
					send_to_char(attrib.c_str(), ch);
					send_to_char(NL, ch);
				}
				else
				{
					const SWString &attrib = char_attribute((desc_type) index, index1, ch->sex, 0);
					sprintf(buf, "Czy chcesz by� widziany jako :"
					FG_YELLOW " %s %s" EOL, attrib.c_str(), CH_RACE(ch, 0)); // Pixel: przypadkiF
					send_to_char(buf, ch);
					send_to_char("By to potwierdzi� ponownie wydaj to samo polecenie" NL, ch);
				}
			}
		}
	}
}

/*
 * New do_who with WHO REQUEST, clan, race and homepage support.  -Thoric
 *
 * Latest version of do_who eliminates redundant code by using linked lists.
 * Shows imms separately, indicates guest and retired immortals.
 * Narn, Oct/96
 */
DEF_DO_FUN( who )
{
	char buf[MSL] =
	{ 0 };
	char buf1[MSL] =
	{ 0 };
	char invis_str[MIL] =
	{ 0 };
	char namebuf[MSL] =
	{ 0 };
	char truststr[MSL] =
	{ 0 };
	char race_text[MIL] =
	{ 0 };
	int iLevelLower;
	int iLevelUpper;
	int nNumber;
	int nMatch;
	RACE_DATA *rgfRace = 0;
	bool fRaceRestrict;
	bool fImmortalOnly;
	bool fSegregateUp = false; //For Jocik (Thanos)
	bool fSegregateDown = false;
	int nPlayersCount = 0;

	WHO_DATA *who = NULL;
	std::list<WHO_DATA*> who_list;

	/*
	 * Set default arguments.
	 */
	iLevelLower = 0;
	iLevelUpper = MAX_LEVEL;
	fRaceRestrict = false;
	fImmortalOnly = false;
	/*
	 * Parse arguments.
	 */
	nNumber = 0;
	for (;;)
	{
		char arg[MAX_STRING_LENGTH];

		argument = one_argument(argument, arg);
		if (arg[0] == '\0')
			break;

		if (is_number(arg))
		{
			switch (++nNumber)
			{
			case 1:
				iLevelLower = atoi(arg);
				break;
			case 2:
				iLevelUpper = atoi(arg);
				break;
			default:
				send_to_char("Tylko dwa numery s� wymagane." NL, ch);
				return;
			}
		}
		else if (!str_cmp(arg, "-s"))
			fSegregateUp = true;
		else if (!str_cmp(arg, "s"))
			fSegregateDown = true;
		else
		{
			if (strlen(arg) < 3)
			{
				send_to_char("Sprecyzuj prosz�." NL, ch);
				return;
			}

			/*
			 * Look for classes to turn on.
			 */

			if (!str_prefix(arg, "immo") || !str_prefix(arg, "gods"))
				fImmortalOnly = true;
			else
			{
				if ((rgfRace = find_race(arg)))
				{
					fRaceRestrict = true;
				}

				if (!rgfRace)
				{
					send_to_char("To nie jest nazwa rasy." NL, ch);
					return;
				}
			}
		}
	}

	/*
	 * Now find matching chars.
	 */
	nMatch = 0;
	buf[0] = '\0';

	for (auto* d : descriptor_list)
	{
		CHAR_DATA *wch;
		char const *race;

		if (d->connected < CON_PLAYING || d->original)
			continue;
		nPlayersCount++;
		if (!can_see(ch, d->character) && IS_IMMORTAL(d->character))
			continue;

		wch = d->original ? d->original : d->character;
		if (wch->top_level < iLevelLower || wch->top_level > iLevelUpper || (fImmortalOnly && wch->top_level < LEVEL_IMMORTAL)
				|| (fRaceRestrict && wch->race != rgfRace))
			continue;

		//pokazujemy tylko tych ktorych znamy
		if (does_knows(ch, wch) == 0)
			continue;
		nMatch++;

		sprintf(race_text, PLAIN "%-15s" PLAIN, !strcmp(wch->race->name, "Human") ? "Cz�owiek" : wch->race->przypadki[0]);
		race = race_text;

		switch (wch->top_level)
		{
		default:
			break;
		case 200:
			race = "Dusza w Kapsule";
			break;
		case MAX_LEVEL - 0:
			race = "  " FG_BLACK "*" FG_GREEN "*" FB_GREEN "*" FB_YELLOW "*"
			FB_WHITE "*" FB_YELLOW "*" FB_WHITE "*" FB_YELLOW "*"
			FB_GREEN "*" FG_GREEN "*" FG_BLACK "*" PLAIN "  ";
			break;
		case MAX_LEVEL - 1:
			race = "   " FG_BLACK "*" FG_GREEN "*" FB_GREEN "*" FB_YELLOW "*"
			FB_WHITE "*" FB_YELLOW "*" FB_GREEN "*" FG_GREEN "*"
			FG_BLACK "*" PLAIN "   ";
			break;
		case MAX_LEVEL - 2:
			race = "    " FG_BLACK "*" FG_GREEN "*" FB_GREEN "*" FB_WHITE "*"
			FB_GREEN "*" FG_GREEN "*" FG_BLACK "*" PLAIN "    ";
			break;
		case MAX_LEVEL - 3:
			race = "     " FG_BLACK "*" FG_GREEN "*" FB_GREEN "*" FG_GREEN "*"
			FG_BLACK "*" PLAIN "     ";
			break;
		case MAX_LEVEL - 4:
			race = "      " FG_BLACK "*" FG_GREEN "*" FG_BLACK "*"
			PLAIN "      ";
			break;
		}

		if (IS_RETIRED(wch))
			race = "     " FG_BLACK "*" FG_GREEN "*" FG_BLACK "*" PLAIN "     ";
		else if (IS_GUEST(wch))
			race = "      " FG_GREEN "*" PLAIN "      ";
		else if (wch->pcdata->rank && wch->pcdata->rank[0] != '\0')
			race = wch->pcdata->rank;

		if (IS_SET(wch->act, PLR_KILLER))
			race = FB_RED "M O R D E R C A" PLAIN;

		if (IS_SET(wch->act, PLR_WIZINVIS))
			sprintf(invis_str, FB_BLUE "(" PLAIN "%d" FB_BLUE ")" PLAIN " ", wch->pcdata->wizinvis);
		else
			invis_str[0] = '\0';

		buf[0] = '\0';
		if (get_trust(ch) >= LEVEL_IMMORTAL)
			sprintf(buf, "[%3d %3.3s]", wch->top_level,
					get_trust(ch) >= get_trust(wch) ? capitalize(class_table[wch->main_ability].who_name) : "***");

		/* Thanos -- trusciaki */
		if (IS_ADMIN( ch->name ) && get_trust(wch) != wch->top_level)
			sprintf(truststr, FG_CYAN "[Trust:%d]" PLAIN " ", get_trust(wch));
		else
			truststr[0] = '\0';

		sprintf(namebuf, "%s %s", wch->name, wch->pcdata->title);

		swsnprintf(buf1, MSL, "[%s" RESET PLAIN "] %s%s%s%s%s%s%s" RESET EOL, strip_colors(race, 15),
				(IS_ADMIN( ch->name ) && IS_OLCMAN(wch)) ? FG_GREEN "{OLC}" PLAIN " " : "",/*Thanos*/
				truststr, invis_str,
				wch->pcdata && IS_SET(wch->pcdata->flags, PCFLAG_NH) ? FB_BLUE "(" FB_RED "NH" FB_BLUE ")" PLAIN " " : "",
				IS_IMMORTAL(wch) && wch->desc && wch->desc->olc_editing ? FB_BLUE "(Buduje) " PLAIN : "",
				IS_SET(wch->act, PLR_AFK) ? FG_YELLOW "(AWAY) " PLAIN : "", namebuf);

		strcat(buf, buf1);

		/*
		 * This is where the old code would display the found player to the ch.
		 * What we do instead is put the found data into a linked list
		 */

		/* First make the structure. */
		CREATE(who, WHO_DATA, 1);
		STRDUP(who->text, buf);
		// NIE get_trust() !
		who->level = wch->top_level;
		who_list.push_back(who);
	}

	/* Ok, now we have three separate linked lists and what remains is to
	 * display the information and clean up.
	 */
	pager_printf(ch, EOL "Na mudzie jest %d gracz%s." NL, nPlayersCount, nPlayersCount == 1 ? "" : "y");
	pager_printf(ch, "Widzisz %d znajom%s gracz%s." NL, nMatch, nMatch == 1 ? "ego" : "ych", nMatch == 1 ? "a" : "y");
	if (fSegregateUp)
	{
		int i;
		for (i = 0; i <= MAX_LEVEL; i++)
		{
			for (auto* who : who_list)
			{
				if (who->level == i)
					send_to_pager(who->text, ch);
			}
		}
	}
	else if (fSegregateDown)
	{
		int i;
		for (i = MAX_LEVEL; i >= 1; i--)
		{
			for (auto* who : who_list)
			{
				if (who->level == i)
					send_to_pager(who->text, ch);
			}
		}
	}
	else
		for (auto* who : who_list)
		{
			send_to_pager(who->text, ch);
		}

	for (auto* who : who_list)
	{
		free_who(who);
	}
	who_list.clear();
	return;
}

DEF_DO_FUN( compare )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	OBJ_DATA *obj1;
	OBJ_DATA *obj2 = nullptr;
	int value1;
	int value2;
	const char *msg;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	if (arg1[0] == '\0')
	{
		send_to_char("Por�wna� co z czym?" NL, ch);
		return;
	}

	if ((obj1 = get_obj_carry(ch, arg1)) == NULL)
	{
		send_to_char("Nie masz niczego takiego." NL, ch);
		return;
	}

	if (arg2[0] == '\0')
	{
		for (auto* o : ch->carrying)
		{
			if (o->wear_loc != WEAR_NONE && can_see_obj(ch, o) && obj1->item_type == o->item_type
					&& (obj1->wear_flags & o->wear_flags & ~ITEM_TAKE) != 0)
			{
				obj2 = o;
				break;
			}
		}

		if (!obj2)
		{
			send_to_char("Nie nosisz nic por�wnywalnego." NL, ch);
			return;
		}
	}
	else
	{
		if ((obj2 = get_obj_carry(ch, arg2)) == NULL)
		{
			send_to_char("Nie masz niczego takiego." NL, ch);
			return;
		}
	}

	msg = NULL;
	value1 = 0;
	value2 = 0;

	if (obj1 == obj2)
	{
		msg = "Por�wnujesz $p$3 z nim samym. Wygl�da tak samo :)";
	}
	else if (obj1->item_type != obj2->item_type)
	{
		msg = "Nie mo�esz por�wna� $p$3 z $P$4.";
	}
	else
	{
		switch (obj1->item_type)
		{
		default:
			msg = "Nie mo�esz por�wna� $p$3 z $P$4.";
			break;

		case ITEM_ARMOR:
			value1 = obj1->value[0];
			value2 = obj2->value[0];
			break;

		case ITEM_WEAPON:
			value1 = obj1->value[1] + obj1->value[2];
			value2 = obj2->value[1] + obj2->value[2];
			break;
		}
	}

	if (!msg)
	{
		if (value1 == value2)
			msg = "$p i $P wygl�daj� podobnie.";
		else if (value1 > value2)
			msg = "$p wygl�da lepiej ni� $P.";
		else
			msg = "$p wygl�da gorzej ni� $P.";
	}

	act(PLAIN, msg, ch, obj1, obj2, TO_CHAR);
	return;
}

DEF_DO_FUN( where )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	bool found;

	if (get_trust(ch) < LEVEL_HERO)
	{
		send_to_char("Gdyby tylko �ycie by�o tak proste..." NL, ch);
		return;
	}

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		if (get_trust(ch) >= LEVEL_HERO)
			send_to_pager("Zalogowani gracze:" NL, ch);
		else
			pager_printf(ch, "Gracze wok� ciebie w %s:" NL, ch->in_room->area->name);
		found = false;
		for (auto* d : descriptor_list)
			if ((d->connected >= CON_PLAYING) && (victim = d->character) != NULL && !IS_NPC(victim) && victim->in_room
					&& (victim->in_room->area == ch->in_room->area || get_trust(ch) >= LEVEL_HERO) && can_see(ch, victim))
			{
				found = true;
				if ((IS_IMMORTAL( ch ) || IS_OLCMAN(ch)) && can_edit(ch, victim->in_room->vnum) && (IS_SET(ch->act, PLR_ROOMVNUM)))
					pager_printf(ch, "%-28s " PLAIN "%-8d %s" EOL, PERS(victim, ch, 0), victim->in_room->vnum, victim->in_room->name);
				else if (CAN_ENTER( ch, victim->in_room ) && !IS_SET(victim->in_room->room_flags, ROOM_NO_FIND))
					pager_printf(ch, "%-28s " PLAIN "%s" EOL, victim->name, victim->in_room->name);
				else if (ch != victim)
					found = false;
			}
		if (!found)
			send_to_char("Nikt." NL, ch);
	}
	else
	{
		found = false;
		for (auto* victim : char_list)
			if (victim->in_room && victim->in_room->area == ch->in_room->area && !IS_AFFECTED(victim, AFF_HIDE)
					&& !IS_AFFECTED(victim, AFF_SNEAK) && can_see(ch, victim) && is_name(arg, victim->name))
			{
				found = true;
				if ((IS_IMMORTAL( ch ) || IS_OLCMAN(ch)) && can_edit(ch, victim->in_room->vnum) && (IS_SET(ch->act, PLR_ROOMVNUM)))
					pager_printf(ch, "%-28s " PLAIN "%-8d %s" EOL, PERS(victim, ch, 0), victim->in_room->vnum, victim->in_room->name);
				else
					pager_printf(ch, "%-28s " PLAIN "%s" EOL, PERS(victim, ch, 0), victim->in_room->name);
				break;
			}
		if (!found)
			act(PLAIN, "Nie mo�esz znale�� �adnego $T.", ch, NULL, arg, TO_CHAR);
	}

	return;
}

DEF_DO_FUN( consider )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	const char *msg;
	const char *buf = "\0";
	int diff;
	int hpdiff;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Por�wna� si� z kim?" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	diff = fight_lev(victim) - fight_lev(ch);

	if (diff <= -10)
		msg = "Hej! A gdzie $E w�a�ciwie jest?";
	else if (diff <= -7)
		msg = "Kaszka z mleczkiem, nawet nie marnuj czasu!";
	else if (diff <= -5)
		msg = "To b�dzie prostsze ni� �apanie szczur�w w dolinie �ebrak�w!";
	else if (diff <= -2)
		msg = "$N to pionek. Zgnieciesz $I.";
	else if (diff <= 1)
		msg = "$N wygl�da jakby by�$O r�wn$Y z tob�.";
	else if (diff <= 3)
		msg = "Je�li ci si� poszcz�ci, byc mo�e prze�yjesz...";
	else if (diff <= 6)
		msg = "Potrzebne b�dzie duuu�o szcz�cia i OGROMNIASTY blaster!";
	else if (diff <= 9)
		msg = "W�a�nie, czemu nie zaatakow� Gwiazdy �mierci wibroostrzem?";
	else
		msg = "$N jest ogromn$Y jak Jot!"; /* Trog: zamiast Jot bylo: AT-AT */
	act(PLAIN, msg, ch, NULL, victim, TO_CHAR);

	hpdiff = (ch->hit - (victim->hit + 150));

	if (((diff >= 0) && (hpdiff <= 0)) || ((diff <= 0) && (hpdiff >= 0)))
	{
		send_to_char("Poza tym,", ch);
	}
	else
	{
		send_to_char("Jednak�e,", ch);
	}

	if (hpdiff >= 101)
		buf = " jeste� du�o silniejsz$y ni� $E.";
	if (hpdiff <= 100)
		buf = " jeste� silniejsz$y ni� $E.";
	if (hpdiff <= 50)
		buf = " jeste� troch� silniejsz$y ni� $E.";
	if (hpdiff <= 25)
		buf = " jeste� minimalnie silniejsz$y ni� $E.";
	if (hpdiff <= 0)
		buf = " $E jest minimalnie silniejsz$Y ni� ty.";
	if (hpdiff <= -25)
		buf = " $E jest troch� silniejsz$Y ni� ty.";
	if (hpdiff <= -50)
		buf = " $E jest silniejsz$Y ni� ty.";
	if (hpdiff <= -100)
		buf = " $E jest du�o silniejsz$Y ni� ty.";

	act(PLAIN, buf, ch, NULL, victim, TO_CHAR);
	return;
}

/*
 * Place any skill types you don't want them to be able to practice
 * normally in this list.  Separate each with a space.
 * (Uses an is_name check). -- Altrag
 */
#define CANT_PRAC (char *)"Tongue"

#define COLUMNS		3	/*3*/
DEF_DO_FUN( practice )
{
	char buf[MAX_STRING_LENGTH];
	int sn;
	char color[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;

	if (argument[0] == '\0')
	{
		int col;
		int lasttype, cnt;

		col = cnt = 0;
		lasttype = SKILL_SPELL;

		for (sn = 0; sn < top_sn; sn++)
		{
			if (!skill_table[sn]->name)
				break;

			if (skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY)
				continue;

			if (strcmp(skill_table[sn]->name, "reserved") == 0 && (IS_IMMORTAL(ch)))
			{
				if (col % COLUMNS != 0)
					send_to_pager(NL, ch);
				send_to_pager(FB_WHITE "Moc" EOL, ch);
				col = 0;
			}
			if (skill_table[sn]->type != lasttype)
			{
				if (cnt)
					if (col % COLUMNS != 0)
						send_to_pager(NL, ch);

				if (strcmp(skill_tname[skill_table[sn]->type], "unknown"))
					pager_printf(ch, NL FB_WHITE "%s:" EOL, skill_tname[skill_table[sn]->type]);

				col = cnt = 0;
			}
			lasttype = skill_table[sn]->type;

			if (skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY)
				continue;

			if (ch->pcdata->learned[sn] <= 0 && ch->skill_level[skill_table[sn]->guild] < skill_table[sn]->min_level)
				continue;

			if (ch->pcdata->learned[sn] == 0 && SPELL_FLAG(skill_table[sn], SF_SECRETSKILL))
				continue;

			++cnt;
			sprintf(color,
					skill_table[sn]->forcetype == 1 ? FB_BLUE : skill_table[sn]->forcetype == 2 ? FG_GREEN :
					skill_table[sn]->forcetype == 3 ? FB_RED : PLAIN);
			pager_printf(ch, "%s%20.20s " FG_CYAN "%3d%%" PLAIN " ", color, skill_table[sn]->name, ch->pcdata->learned[sn]);
			if (++col % COLUMNS == 0)
				send_to_pager(NL, ch);
		}

		if (col % COLUMNS != 0)
			send_to_pager(NL, ch);

#if defined (ARMAGEDDON)
		pager_printf( ch, "Koszt �wiczenia umiej�tno�ci spoza twojej profesji: %d prakty%s." NL
				"Masz jeszcze " FB_WHITE "%d" PLAIN " prakty%s." NL,
				NUM_PRACT_FOR_SKILL, NUMBER_SUFF( NUM_PRACT_FOR_SKILL, "k�", "ki", "k" ),
				ch->pcdata->practices, NUMBER_SUFF( ch->pcdata->practices, "k�", "ki", "k" ) );
#endif
	}
	else
	{
		CHAR_DATA *mob = nullptr;
		int adept;
		bool can_prac = true;

		if (!IS_AWAKE(ch))
		{
			send_to_char("Co ci si� �ni?" NL, ch);
			return;
		}

		for (auto* m : ch->in_room->people)
			if (IS_NPC(m) && IS_SET(m->act, ACT_PRACTICE))
			{
				mob = m;
				break;
			}

		if (!mob)
		{
			send_to_char("Nie ma tu nikogo, kto m�g�by ci� czego� nauczy�." NL, ch);
			return;
		}

		sn = skill_lookup(argument);

		if (sn == -1)
		{
			ch_tell(mob, ch, (char*) "Nie znam takiej umiej�tno�ci...");
			return;
		}

		if (skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY)
		{
			ch_tell(mob, ch, (char*) "Nie umiem uczy� czego� takiego...");
			return;
		}

		if (can_prac && !IS_NPC(ch) && ch->skill_level[skill_table[sn]->guild] < skill_table[sn]->min_level)
		{
			sprintf(buf, "Nie jeste� jeszcze na to gotow%s...", SEX_SUFFIX_YAE(ch));
			ch_tell(mob, ch, buf);
			return;
		}

		if (is_name(skill_tname[skill_table[sn]->type], CANT_PRAC))
		{
			ch_tell(mob, ch, (char*) "Nie umiem uczyc czego� takiego...");
			return;
		}

		/*
		 * Skill requires a special teacher
		 */
		if (skill_table[sn]->teachers && skill_table[sn]->teachers[0] != '\0')
		{
			sprintf(buf, "%d", mob->pIndexData->vnum);
			if (!is_name(buf, skill_table[sn]->teachers))
			{
				ch_tell(mob, ch, (char*) "Nie umiem uczyc czego� takiego...");
				return;
			}
		}
		else
		{
			ch_tell(mob, ch, (char*) "Nie umiem uczyc czego� takiego...");
			return;
		}

		if (skill_table[sn]->guild == FORCE_ABILITY)
			adept = 20;
		else
			adept = 50;

		if (ch->gold < skill_table[sn]->min_level * 10)
		{
			sprintf(buf, "Chc� %d kredytek za nauczenie ci� tego.", skill_table[sn]->min_level * 10);
			ch_tell(mob, ch, buf);
			ch_tell(mob, ch, (char*) "Nie masz tyle.");
			return;
		}
#if defined(ARMAGEDDON)
		if( skill_table[sn]->guild != ch->main_ability )
		{
			if( ch->pcdata->practices < NUM_PRACT_FOR_SKILL )
			{
				ch_tell( mob, ch, "Nie masz praktyk by wy�wiczy� t� umiej�tno��..." );
				return;
			}
		}
#endif
		if (ch->pcdata->learned[sn] >= adept)
		{
			sprintf(buf, "Nie umiem nauczy� ci� niczego wi�cej na temat %s.", skill_table[sn]->name);
			ch_tell(mob, ch, buf);
			sprintf(buf, "Reszt� musisz wy�wiczy� sam%s...", SEX_SUFFIX__AO(ch));
			ch_tell(mob, ch, buf);
		}
		else
		{
			ch->gold -= skill_table[sn]->min_level * 10;
#if defined(ARMAGEDDON)
			ch->pcdata->practices-=NUM_PRACT_FOR_SKILL;
#endif
			ch->pcdata->learned[sn] += int_app[get_curr_int(ch)].learn;
			act(COL_ACTION, "�wiczysz $T.", ch, NULL, skill_table[sn]->name,
			TO_CHAR);
			act(COL_ACTION, "$n �wiczy $T.", ch, NULL, skill_table[sn]->name,
			TO_ROOM);
			if (ch->pcdata->learned[sn] >= adept)
			{
				ch->pcdata->learned[sn] = adept;
				sprintf(buf, "Wi�cej na temat %s ci� ju� nie naucz�.", skill_table[sn]->name);
				ch_tell(mob, ch, buf);
			}
		}
	}
	return;
}
#undef COLUMNS

DEF_DO_FUN( teach )
{
	char buf[MAX_STRING_LENGTH];
	int sn;
	char arg[MAX_INPUT_LENGTH];

	if (IS_NPC(ch))
		return;

	argument = one_argument(argument, arg);

	if (argument[0] == '\0')
	{
		send_to_char("Nauczy� kogo, czego?" NL, ch);
		return;
	}
	else
	{
		CHAR_DATA *victim;
		int adept;

		if (!IS_AWAKE(ch))
		{
			send_to_char("Co ci si� �ni?" NL, ch);
			return;
		}

		if ((victim = get_char_room(ch, arg)) == NULL)
		{
			send_to_char("Nie ma tu nikogo takiego." NL, ch);
			return;
		}

		if (IS_NPC(victim))
		{
			ch_printf(ch, "%s nie nauczy si� od ciebie niczego..." NL, PERS(victim, ch, 0));
			return;
		}

		sn = skill_lookup(argument);

		if (sn == -1)
		{
			act(PLAIN, "Nawet ty nie masz poj�cia co to jest.", victim, NULL, ch, TO_VICT);
			return;
		}

		if (skill_table[sn]->guild < 0 || skill_table[sn]->guild >= MAX_ABILITY)
		{
			act(COL_TELL, "Ten numer nie przejdzie.", victim, NULL, ch, TO_VICT);
			return;
		}

#if defined(ARMAGEDDON)
		// wiecej praktyk przy teach -- tak, zeby gracz mogl pokombinowac
		// i poszukac moba. Zreszta, zwykli gracze sa gorszymi nauczycielami
		// niz fachowe moby do tego stworzone, wiec sesje sa oporniejsze.
		if( victim->pcdata->practices < NUM_PRACT_FOR_SKILL+1 )
		{
			act( COL_TELL, "$n nie ma wystarczaj�cej ilo�ci praktyk.",
					victim, NULL, ch, TO_VICT );
			return;
		}
#endif
		if (victim->skill_level[skill_table[sn]->guild] < skill_table[sn]->min_level)
		{
			act(COL_TELL, "$n nie jest jeszcze gotow$y by si� tego uczy�.", victim, NULL, ch, TO_VICT);
			return;
		}

		if (is_name(skill_tname[skill_table[sn]->type], CANT_PRAC))
		{
			act(COL_TELL, "Tej umiej�tno�ci nie mo�esz uczy�.", victim, NULL, ch, TO_VICT);
			return;
		}

		adept = 20;

		if (victim->pcdata->learned[sn] >= adept)
		{
			act(COL_TELL, "$n musi to wy�wiczy� sam$x.", victim, NULL, ch,
			TO_VICT);
			return;
		}
		if (ch->pcdata->learned[sn] < 100)
		{
			act(
			COL_TELL, "Najpierw ty opanuj t� umiej�tno�� w ca�o�ci, dopiero potem ucz innych.", victim, NULL, ch, TO_VICT);
			return;
		}
		else
		{
			victim->pcdata->learned[sn] += int_app[(get_curr_wis(ch) + get_curr_int(victim)) / 3].learn;

#if defined(ARMAGEDDON)
			victim->pcdata->practices -= (NUM_PRACT_FOR_SKILL+1);
#endif

			sprintf(buf, "Pokazujesz %s tajniki $T.", PERS(victim, ch, 2));
			act(COL_ACTION, buf, ch, NULL, skill_table[sn]->name, TO_CHAR);
			sprintf(buf, "%s pokazuje ci tajniki $T.", PERS(ch, victim, 0));
			act(COL_ACTION, buf, victim, NULL, skill_table[sn]->name, TO_CHAR);
		}
	}
	return;
}

DEF_DO_FUN( wimpy )
{
	char arg[MAX_INPUT_LENGTH];
	int wimpy;

	one_argument(argument, arg);

	if (arg[0] == '\0')
		wimpy = (int) ch->max_hit / 5;
	else
		wimpy = atoi(arg);

	if (wimpy < 0)
	{
		send_to_char("Twoja odwaga przewy�sza nawet twoj� m�dro��." NL, ch);
		return;
	}

	if (wimpy > ch->max_hit)
	{
		send_to_char("No nie b�d� a� takim tch�rzem." NL, ch);
		return;
	}

	ch->wimpy = wimpy;
	ch_printf(ch, "Uciekasz z walki poni�ej %d hp." NL, wimpy);
	return;
}

DEF_DO_FUN( socials )
{
	int iHash;
	int col = 0;
	SOCIALTYPE *social;

	for (iHash = 0; iHash < 27; iHash++)
		for (social = social_index[iHash]; social; social = social->next)
		{
			pager_printf(ch, "%-12s", social->name);
			if (++col % 6 == 0)
				send_to_pager(NL, ch);
		}

	if (col % 6 != 0)
		send_to_pager(NL, ch);
	return;
}

DEF_DO_FUN( commands )
{
	int columns = 2;
	int col;
	bool found;
	int hash;
	CMDTYPE *command;

	col = 0;
	if (argument[0] == '\0')
	{
		pager_printf(ch, FB_WHITE "%-14.14s %-24.24s ", "Komenda", "Odpowiedniki");
		pager_printf(ch, FB_WHITE "%-14.14s %-24.24s" EOL, "Komenda", "Odpowiedniki");
		for (hash = 0; hash < MAX_CMD_HASH; hash++)
			for (command = command_hash[hash]; command; command = command->next)
				if (command->level < LEVEL_HERO && command->level <= get_trust(ch))
				{
					if (strlen(command->name) == 1 || (command->name[0] == 'm' && command->name[1] == 'p')
							|| (IS_SET(command->flags, CMD_ADMIN_ONLY) && !IS_ADMIN(ch->name)))
						continue;

					pager_printf(ch, "%-14.14s %-24.24s", command->name, *command->alias ? command->alias : "");
					//		    pager_printf( ch, "%-12s", command->name );
					if (++col % columns == 0)
						send_to_pager(NL, ch);
					else
						send_to_pager("|", ch);
				}
		if (col % 6 != 0)
			send_to_pager(NL, ch);
	}
	else
	{
		found = false;
		pager_printf(ch, FB_WHITE "%-14.14s %-24.24s ", "Komenda", "Odpowiedniki");
		pager_printf(ch, FB_WHITE "%-14.14s %-24.24s" EOL, "Komenda", "Odpowiedniki");
		for (hash = 0; hash < MAX_CMD_HASH; hash++)
			for (command = command_hash[hash]; command; command = command->next)
				if (command->level < LEVEL_HERO && command->level <= get_trust(ch)
						&& (!str_prefix(argument, command->name) || is_name_prefix(argument, command->alias)))
				{
					if ((command->name[0] == 'm' && command->name[1] == 'p')
							|| (IS_SET(command->flags, CMD_ADMIN_ONLY) && !IS_ADMIN(ch->name)))
						continue;
					pager_printf(ch, "%-14.14s %-24.24s", command->name, *command->alias ? command->alias : "");
					found = true;
					if (++col % columns == 0)
						send_to_pager(NL, ch);
					else
						send_to_pager("|", ch);
				}

		if (col % columns != 0)
			send_to_pager(NL, ch);
		if (!found)
			ch_printf(ch, "Pod skr�tem '%s' nie znaleziono komendy." NL, argument);
	}
	return;
}

DEF_DO_FUN( channels )
{
	char arg[MAX_INPUT_LENGTH];

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		if (!IS_NPC(ch) && IS_SILENCED(ch))
		{
			send_to_char("Nie mo�esz u�ywa� kana��w." NL, ch);
			return;
		}

		send_to_char("Kana�y:", ch);

		if (get_trust(ch) > 2 && !NOT_AUTHED(ch))
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_AUCTION) ? "&G +AUCTION&w" : "&g -auction&w", ch);
		}

		send_to_char(!IS_SET(ch->deaf, CHANNEL_CHAT) ? "&G +CHAT&w" : "&g -chat&w", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_FLAME) ? "&G +FLAME&w" : "&g -flame&w", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_OOC) ? "&G +OOC&w" : "&g -ooc&w", ch);

		if (!IS_NPC(ch) && ch->pcdata->clan)
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_CLAN) ? "&G +CLAN&w" : "&g -clan&w", ch);
		}

		send_to_char(!IS_SET(ch->deaf, CHANNEL_QUEST) ? "&G +QUEST&w" : "&g -quest&w", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_TELLS) ? "&G +TELLS&w" : "&g -tells&w", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_WARTALK) ? "&G +WARTALK&w" : "&g -wartalk&w", ch);

		if (IS_OLCMAN( ch ) || IS_ADMIN(ch->name) || (get_trust(ch) > 102))		// by Trog
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_OLCTALK) ? "&G +OLCTALK&w" : "&g -olctalk&w", ch);
		}

		if (IS_CODER(ch->name)) //Trog
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_CODERTALK) ? "&G +CODERTALK&w" : "&g -codertalk&w", ch);
		}

		if (IS_HERO(ch))
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_IMMTALK) ? "&G +IMMTALK&w" : "&g -immtalk&w", ch);
		}

		if (IS_IMMORTAL(ch))
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_PRAY) ? "&G +PRAY&w" : "&g -pray&w", ch);
		}

		send_to_char(!IS_SET(ch->deaf, CHANNEL_MUSIC) ? "&G +MUSIC&w" : "&g -music&w", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_ASK) ? "&G +ASK&w" : "&g -ask&w", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_SHOUT) ? "&G +SHOUT&w" : "&g -shout&w", ch);

		send_to_char(!IS_SET(ch->deaf, CHANNEL_YELL) ? "&G +YELL&w" : "&g -yell&w", ch);

		if (IS_IMMORTAL(ch))
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_MONITOR) ? "&G +MONITOR&w" : "&g -monitor&w", ch);
		}

		send_to_char(!IS_SET(ch->deaf, CHANNEL_NEWBIE) ? "&G +NEWBIE&w" : "&g -newbie&w", ch);

		if (get_trust(ch) >= sysdata.log_level)
		{
			send_to_char(!IS_SET(ch->deaf, CHANNEL_LOG) ? "&G +LOG&w" : "&g -log&w", ch);

			send_to_char(!IS_SET(ch->deaf, CHANNEL_BUILD) ? "&G +BUILD&w" : "&g -build&w", ch);
		}
		if (get_trust(ch) >= sysdata.comm_level)
			send_to_char(!IS_SET(ch->deaf, CHANNEL_COMM) ? "&G +COMM&w" : "&g -comm&w", ch);
		if (get_trust(ch) >= sysdata.prog_level)
			send_to_char(!IS_SET(ch->deaf, CHANNEL_PROG) ? "&G +PROG&w" : "&g -prog&w", ch);
		send_to_char("." NL, ch);
	}
	else
	{
		bool fClear;
		bool ClearAll;
		int bit;

		bit = 0;
		ClearAll = false;

		if (arg[0] == '+')
			fClear = true;
		else if (arg[0] == '-')
			fClear = false;
		else
		{
			send_to_char("Sk�adnia: Channels +|- <kana�>" NL, ch);
			return;
		}

		if (!str_cmp(arg + 1, "auction"))
			bit = CHANNEL_AUCTION;
		else if (!str_cmp(arg + 1, "chat"))
			bit = CHANNEL_CHAT;
		else if (!str_cmp(arg + 1, "flame"))
			bit = CHANNEL_FLAME;
		else if (!str_cmp(arg + 1, "ooc"))
			bit = CHANNEL_OOC;
		else if (!str_cmp(arg + 1, "clan"))
			bit = CHANNEL_CLAN;
		else if (!str_cmp(arg + 1, "guild"))
			bit = CHANNEL_GUILD;
		else if (!str_cmp(arg + 1, "quest"))
			bit = CHANNEL_QUEST;
		else if (!str_cmp(arg + 1, "tells"))
			bit = CHANNEL_TELLS;
		else if (!str_cmp(arg + 1, "immtalk"))
			bit = CHANNEL_IMMTALK;
		else if (!str_cmp(arg + 1, "admintalk"))
			bit = CHANNEL_ADMINTALK; //Trog
		else if (!str_cmp(arg + 1, "log"))
			bit = CHANNEL_LOG;
		else if (!str_cmp(arg + 1, "build"))
			bit = CHANNEL_BUILD;
		else if (!str_cmp(arg + 1, "pray"))
			bit = CHANNEL_PRAY;
		else if (!str_cmp(arg + 1, "olctalk"))
			bit = (int) CHANNEL_OLCTALK;
		else if (!str_cmp(arg + 1, "codertalk"))
			bit = CHANNEL_CODERTALK; //Trog
		else if (!str_cmp(arg + 1, "monitor"))
			bit = CHANNEL_MONITOR;
		else if (!str_cmp(arg + 1, "newbie"))
			bit = CHANNEL_NEWBIE;
		else if (!str_cmp(arg + 1, "music"))
			bit = CHANNEL_MUSIC;
		else if (!str_cmp(arg + 1, "ask"))
			bit = CHANNEL_ASK;
		else if (!str_cmp(arg + 1, "shout"))
			bit = CHANNEL_SHOUT;
		else if (!str_cmp(arg + 1, "yell"))
			bit = CHANNEL_YELL;
		else if (!str_cmp(arg + 1, "comm"))
			bit = CHANNEL_COMM;
		else if (!str_cmp(arg + 1, "order"))
			bit = CHANNEL_ORDER;
		else if (!str_cmp(arg + 1, "wartalk"))
			bit = CHANNEL_WARTALK;
		else if (!str_cmp(arg + 1, "prog"))
			bit = CHANNEL_PROG;
		else if (!str_cmp(arg + 1, "all"))
			ClearAll = true;
		else
		{
			send_to_char("Kt�ry kana� ustawi�?" NL, ch);
			return;
		}

		if ((fClear) && (ClearAll))
		{
			REMOVE_BIT(ch->deaf, CHANNEL_AUCTION);
			REMOVE_BIT(ch->deaf, CHANNEL_CHAT);
			REMOVE_BIT(ch->deaf, CHANNEL_FLAME);
			REMOVE_BIT(ch->deaf, CHANNEL_QUEST);
			REMOVE_BIT(ch->deaf, CHANNEL_IMMTALK);
			REMOVE_BIT(ch->deaf, CHANNEL_PRAY);
			REMOVE_BIT(ch->deaf, CHANNEL_MUSIC);
			REMOVE_BIT(ch->deaf, CHANNEL_ASK);
			REMOVE_BIT(ch->deaf, CHANNEL_SHOUT);
			REMOVE_BIT(ch->deaf, CHANNEL_YELL);
			REMOVE_BIT(ch->deaf, CHANNEL_OOC);
			REMOVE_BIT(ch->deaf, CHANNEL_CLAN);
			REMOVE_BIT(ch->deaf, CHANNEL_LOG);
			REMOVE_BIT(ch->deaf, CHANNEL_BUILD);
			REMOVE_BIT(ch->deaf, CHANNEL_OLCTALK);
			REMOVE_BIT(ch->deaf, CHANNEL_MONITOR);
			REMOVE_BIT(ch->deaf, CHANNEL_NEWBIE);
			REMOVE_BIT(ch->deaf, CHANNEL_ORDER);
			REMOVE_BIT(ch->deaf, CHANNEL_CODERTALK);

			/*     if (ch->pcdata->clan)
			 REMOVE_BIT (ch->deaf, CHANNEL_CLAN);
			 */

			if (ch->top_level >= sysdata.prog_level)
				REMOVE_BIT(ch->deaf, CHANNEL_PROG);

			if (ch->top_level >= sysdata.comm_level)
				REMOVE_BIT(ch->deaf, CHANNEL_COMM);

		}
		else if ((!fClear) && (ClearAll))
		{
			SET_BIT(ch->deaf, CHANNEL_AUCTION);
			SET_BIT(ch->deaf, CHANNEL_CHAT);
			SET_BIT(ch->deaf, CHANNEL_FLAME);
			SET_BIT(ch->deaf, CHANNEL_QUEST);
			SET_BIT(ch->deaf, CHANNEL_IMMTALK);
			SET_BIT(ch->deaf, CHANNEL_PRAY);
			SET_BIT(ch->deaf, CHANNEL_MUSIC);
			SET_BIT(ch->deaf, CHANNEL_ASK);
			SET_BIT(ch->deaf, CHANNEL_SHOUT);
			SET_BIT(ch->deaf, CHANNEL_YELL);
			SET_BIT(ch->deaf, CHANNEL_OOC);
			SET_BIT(ch->deaf, CHANNEL_CLAN);
			SET_BIT(ch->deaf, CHANNEL_LOG);
			SET_BIT(ch->deaf, CHANNEL_BUILD);
			SET_BIT(ch->deaf, CHANNEL_OLCTALK);
			SET_BIT(ch->deaf, CHANNEL_MONITOR);
			SET_BIT(ch->deaf, CHANNEL_NEWBIE);
			SET_BIT(ch->deaf, CHANNEL_ORDER);
			SET_BIT(ch->deaf, CHANNEL_CODERTALK);

			if (ch->top_level >= sysdata.prog_level)
				SET_BIT(ch->deaf, CHANNEL_PROG);

			if (ch->top_level >= sysdata.comm_level)
				SET_BIT(ch->deaf, CHANNEL_COMM);

		}
		else if (fClear)
			REMOVE_BIT(ch->deaf, bit);
		else
			SET_BIT(ch->deaf, bit);

		send_to_char("Ok." NL, ch);
	}

	return;
}

/*
 * display WIZLIST file						-Thoric
 */
DEF_DO_FUN( wizlist )
{
	send_to_char(FB_WHITE, ch);
	show_file(ch, WIZLIST_FILE);
	send_to_char(EOL, ch);
}

/*
 * Contributed by Grodyn.
 */
DEF_DO_FUN( config )
{
	char arg[MAX_INPUT_LENGTH];
	int x;

	struct conf_type
	{
		const char *name;
		int bit;
		bool immo;
		const char *text_on;
		const char *text_off;
	};

	const struct conf_type conf_act_table[] =
	{
	{ "flee", PLR_FLEE, false, "Automatycznie uciekasz, gdy kto� ci� zaatakuje.", "Gdy kto� ci� zaatakuje, walczysz." },
	{ "autoexit", PLR_AUTOEXIT, false, "Automatycznie widzisz wyj�cia z lokacji.", "Nie widzisz automatycznie wyj�� z lokacji." },
	{ "autoloot", PLR_AUTOLOOT, false, "Automatycznie opr�niasz cia�a po walce.", "Nie opr�niasz cia� automatycznie po walce." },
	{ "autocred",
	PLR_AUTOGOLD, false, "Automatycznie opr�niasz cia�a przeciwnik�w z kredytek.",
			"Nie opr�niasz automatycznie cia� przeciwnik�w z kredytek." },
	{ "blank", PLR_BLANK, false, "Masz pust� lini� przed promptem.", "Nie masz pustej linii przed promptem." },
	{ "combine", PLR_COMBINE, false, "Widzisz przedmioty w formie po��czonej.", "Widzisz ka�dy przedmiot osobno." },
	{ "brief", PLR_BRIEF, false, "Widzisz skr�cone opisy lokacji (fuj).", "Widzisz ca�e opisy lokacji." },
	{ "prompt", PLR_PROMPT, false, "Masz prompt.", "Nie masz promptu." },
	{ "telnetga", PLR_TELNET_GA, false, "Dostajesz sekwencj� telnetu GO-AHEAD.", "Nie dostajesz sekwencji telnetu GO-AHEAD." },
	{ "mapexits", PLR_MAPEXITS, false, "Widzisz graficznie przedstawione kierunki wyj�cia z lokacji.",
			"Nie widzisz graficznych kierunk�w wyj�cia z lokacji." },
	{ "longexits", PLR_LONGEXITS, false, "Widzisz d�ugie opisy wyj�� z lokacji.", "Nie widzisz d�ugich opis�w wyj�� z lokacji." },
	{ "ansi", PLR_ANSI, false, "Masz w��czone kolory ANSI.", "Nie u�ywasz kolor�w ANSI." },
	{ "tick", PLR_TICK, false, "Widzisz kiedy mija godzina.", "Nie widzisz kiedy mija godzina." },
	{ "sound", PLR_SOUND, false, "Dostajesz wsparcie d�wi�kiem.", "Grasz w ciszy." },
	{ "vnum", PLR_ROOMVNUM, true, "Widzisz vnumy lokacji, mob�w i obj.", "Nie widzisz vnum�w lokacji, mob�w i obj." },
	//Tanglor - nowa flaga dla IMMO
			{ "known4all", PLR_KNOWN4ALL, true, "Dla wszystkich jestes widziany jako znajomy.",
					"Obowi�zuj� Cie te same ograniczenia jak innych znajomych." },
			{ "", 0, true, "", "" } };

	const struct conf_type conf_pcflag_table[] =
	{
	{ "gag", PCFLAG_GAG, false, "Widzisz skr�cony opis walki.", "Widzisz ca�y opis walki." },
	{ "pager", PCFLAG_PAGERON, false, "Masz pager.", "Nie masz pagera." },
	{ "nointro", PCFLAG_NOINTRO, false, "Nie dostajesz dodatkowych informacji przy wej�ciu do gry.",
			"Dostajesz dodatkowe informacje przy wej�ciu do gry." },
	{ "roomflags", PCFLAG_ROOM, true, "Widzisz flagi lokacji.", "Nie widzisz flag lokacji." },
	{ "shipstat", PCFLAG_SHIPSTAT, false, "Widzisz dodatkowe informacje o swoim statku.", "Nie widzisz danych statku, kt�ry prowadzisz." },
	{ "", 0, true, "", "" } };

	if (IS_NPC(ch))
	{
		send_to_char("Tak tak. Wszystko pi�knie dzia�a." NL, ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_pager(FG_GREEN "[ Opcja    ] Stan" EOL, ch);

		for (x = 0; *conf_act_table[x].name; x++)
		{
			if (!IS_IMMORTAL(ch) && conf_act_table[x].immo && !IS_OLCMAN(ch))
				continue;

			if (IS_SET(ch->act, conf_act_table[x].bit))
				pager_printf(ch, FB_GREEN "[+%-9.9s]" PLAIN " %s" NL, all_capitalize(conf_act_table[x].name), conf_act_table[x].text_on);
			else
				pager_printf(ch, FG_GREEN "[-%-9.9s]" PLAIN " %s" NL, conf_act_table[x].name, conf_act_table[x].text_off);
		}

		for (x = 0; *conf_pcflag_table[x].name; x++)
		{
			if (!IS_IMMORTAL(ch) && conf_pcflag_table[x].immo && !IS_OLCMAN(ch))
				continue;

			if (IS_SET(ch->pcdata->flags, conf_pcflag_table[x].bit))
				pager_printf(ch, FB_GREEN "[+%-9.9s]" PLAIN " %s" NL, all_capitalize(conf_pcflag_table[x].name),
						conf_pcflag_table[x].text_on);
			else
				pager_printf(ch, FG_GREEN "[-%-9.9s]" PLAIN " %s" NL, conf_pcflag_table[x].name, conf_pcflag_table[x].text_off);
		}
		send_to_char(
		IS_SILENCED( ch ) ? FB_GREEN "[+SILENCE  ]" PLAIN " Nie mo�esz m�wi�." NL : "", ch);
		send_to_char(!IS_SET(ch->act, PLR_LITTERBUG) ? "" : FG_GREEN "[-litter   ]" PLAIN " Nie wolno ci �mieci�." NL, ch);
	}
	else
	{
		bool fSet;
		int bit = 0;
		int i = 1;
		char buf[MSL];

		if (argument[0] == '+')
			fSet = true;
		else if (argument[0] == '-')
			fSet = false;
		else
		{
			send_to_char("Sk�adnia: Config +|- <opcja>" NL, ch);
			return;
		}

		while (isspace(argument[i]))
			i++;
		strcpy(arg, argument + i);

		for (x = 0; *conf_act_table[x].name; x++)
		{
			if (conf_act_table[x].immo && !IS_IMMORTAL(ch) && !IS_OLCMAN(ch))
				continue;

			if (!str_cmp(arg, conf_act_table[x].name))
			{
				bit = conf_act_table[x].bit;
				break;
			}
		}

		if (bit)
		{

			if (fSet)
			{
				strcpy(buf, conf_act_table[x].text_on);
				buf[0] = LOWER(buf[0]);
				SET_BIT(ch->act, bit);
			}
			else
			{
				strcpy(buf, conf_act_table[x].text_off);
				buf[0] = LOWER(buf[0]);
				REMOVE_BIT(ch->act, bit);
			}

			ch_printf(ch, "Ok." NL FB_YELLOW "%s" PLAIN " zosta�o " FB_YELLOW "%s" EOL, all_capitalize(arg),
					fSet ? "W��CZONE" : "WY��CZONE");
			ch_printf(ch, "Od teraz %s" NL, buf);

			return;
		}
		else
		{
			for (x = 0; *conf_pcflag_table[x].name; x++)
			{
				if (conf_pcflag_table[x].immo && !IS_IMMORTAL(ch) && !IS_OLCMAN(ch))
					continue;

				if (!str_cmp(arg, conf_pcflag_table[x].name))
				{
					bit = conf_pcflag_table[x].bit;
					break;
				}
			}

			if (!bit)
			{
				send_to_char("Kt�r� opcj� chcesz skonfigurowa�?" NL, ch);
				return;
			}

			if (fSet)
			{
				strcpy(buf, conf_pcflag_table[x].text_on);
				buf[0] = LOWER(buf[0]);
				SET_BIT(ch->pcdata->flags, bit);
			}
			else
			{
				strcpy(buf, conf_pcflag_table[x].text_off);
				buf[0] = LOWER(buf[0]);
				REMOVE_BIT(ch->pcdata->flags, bit);
			}
			ch_printf(ch, "Ok." NL FB_YELLOW "%s" PLAIN " zosta�o " FB_YELLOW "%s" EOL, all_capitalize(arg),
					fSet ? "W��CZONE" : "WY��CZONE");
			ch_printf(ch, "Od teraz %s" NL, buf);
			return;
		}
	}

	return;
}

DEF_DO_FUN( credits )
{
	do_help(ch, (char*) "credits");
}

/*
 * New do_areas with level ranges
 */

DEF_DO_FUN( areas )
{
	int lev;
	int maxlev;

	send_to_pager(FG_GREY MOD_BOLD
	" Poziom    Autor            Kraina                              Planeta"
	EOL, ch);

	for (lev = 0; lev <= MAX_LEVEL; lev++)
	{
		for (maxlev = (lev - 1); maxlev <= MAX_LEVEL; maxlev++)
		{
			for (auto* pArea : area_list)
			{
				if (IS_SET( pArea->flags, AFLAG_HIDDEN ) && !IS_SET(ch->act, PLR_HOLYLIGHT))
					continue;

				if (pArea->low_range == lev && pArea->high_range == maxlev)
				{
					pager_printf(ch, "[%3d-%-3d] %s %-16s %-35s", pArea->low_range, pArea->high_range,
					IS_SET( pArea->flags, AFLAG_HIDDEN ) ? FG_BLACK : FG_CYAN, pArea->author, pArea->name);

					if (!pArea->planet || (IS_SET( pArea->flags, AFLAG_DONTSHOWPLANET ) && !IS_SET(ch->act, PLR_HOLYLIGHT)))
						pager_printf(ch, EOL);
					else
						pager_printf(ch, "%s %s" EOL,
						IS_SET( pArea->flags, AFLAG_DONTSHOWPLANET ) ? FG_BLACK : FG_CYAN, pArea->planet->name);
				}
			}
		}
	}
	return;
}

//added by Thanos (doda�em pow�d AWAYa)
DEF_DO_FUN( afk )
{
	if (IS_NPC(ch))
		return;

	if (IS_SET(ch->act, PLR_AFK))
	{
		REMOVE_BIT(ch->act, PLR_AFK);
		send_to_char("Powracasz do klawiaturki." NL, ch);
		act(PLAIN, "$n$R powraca do klawiaturki.", ch, NULL, NULL, TO_ROOM);
		STRDUP(ch->pcdata->afk_reason, "");
		if (!ch->pcdata->last_tells.empty())
			send_to_char("Masz nagrane jakie� wiadomo�ci do ciebie." NL
			"Wpisz 'last tell' by je zobaczy�." NL, ch);

	}
	else
	{
		SET_BIT(ch->act, PLR_AFK);
		ch_printf(ch, "Odchodzisz od klawiaturki." NL FG_YELLOW "(%s)" EOL, argument[0] ? argument : "Bez powodu");
		act(PLAIN, "$n$R odchodzi od klawiaturki." NL FG_YELLOW "($T" FG_YELLOW ")", ch, NULL, argument[0] ? argument : "Bez powodu",
				TO_ROOM);
		STRDUP(ch->pcdata->afk_reason, argument);
	}
	return;
}

DEF_DO_FUN( slist )
{
	int sn, i;
	char skn[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int lowlev, hilev;
	int col = 0;
	int ability;
	CHAR_DATA *vch;

	if (*argument && !is_number(argument) && !IS_NPC(ch) && IS_ADMIN(ch->name))
	{
		if ((vch = get_char_world(ch, argument)) == NULL)
		{
			send_to_char("Nie ma nikogo takiego" NL, ch);
			return;
		}
		lowlev = 1;
		hilev = 100;
	}
	else
	{
		vch = ch;

		argument = one_argument(argument, arg1);
		argument = one_argument(argument, arg2);

		lowlev = 1;
		hilev = 100;

		if (arg1[0] != '\0')
			lowlev = atoi(arg1);

		if ((lowlev < 1) || (lowlev > LEVEL_IMMORTAL))
			lowlev = 1;

		if (arg2[0] != '\0')
			hilev = atoi(arg2);

		if ((hilev < 0) || (hilev >= LEVEL_IMMORTAL))
			hilev = LEVEL_HERO;

		if (lowlev > hilev)
			lowlev = hilev;
	}

	if (IS_NPC(vch))
		return;

	pager_printf(ch, "Oto dost�pne %s umiej�tno�ci:" NL, ch == vch ? "ci" : FEMALE(vch) ? "jej" : "mu");

	for (ability = -1; ability < MAX_ABILITY; ability++)
	{
		if (ability == FORCE_ABILITY && !IS_IMMORTAL(ch))
			continue;

		if (ability >= 0)
			sprintf(skn, NL FB_WHITE "%s" EOL, class_table[ability].przypadki[0]);
		else
			sprintf(skn, NL FB_WHITE "J�zyki:" EOL);
		send_to_pager(skn, ch);

		for (i = lowlev; i <= hilev; i++)
		{
			for (sn = 0; sn < top_sn; sn++)
			{
				if (!skill_table[sn]->name)
					break;

				if (skill_table[sn]->guild != ability)
					continue;

				if (vch->pcdata->learned[sn] == 0 && SPELL_FLAG(skill_table[sn], SF_SECRETSKILL))
					continue;

				/* �eby gracze nie widzieli wszystkich skilli, a tylko
				 te, kt�rymi powinni si� interesowa�	    -- Thanos */
				if (!IS_HERO(ch) && max_level(ch, ability) < skill_table[sn]->min_level)
					continue;

				sprintf(buf, FG_GREEN "%d%%", vch->pcdata->learned[sn]);

				if (i == skill_table[sn]->min_level)
				{
					pager_printf(ch,
					FG_CYAN " [%3d]" PLAIN "%-16.16s %-6s " PLAIN, i, skill_table[sn]->name,
							max_level(vch, ability) < skill_table[sn]->min_level ? FG_RED "--- " :
							vch->skill_level[ability] < skill_table[sn]->min_level ? FB_RED "n/a " : buf);
					if (++col == 2)
					{
						pager_printf(ch, EOL);
						col = 0;
					}
				}

			}
		}
		if (col != 0)
		{
			pager_printf(ch, NL);
			col = 0;
		}

	}
	return;
}

DEF_DO_FUN( whois )
{
	CHAR_DATA *victim;
	char buf[MAX_STRING_LENGTH];
	char buf2[MAX_STRING_LENGTH];

	buf[0] = '\0';

	if (IS_NPC(ch))
		return;

	if (argument[0] == '\0')
	{
		send_to_char("Musisz umie�ci� imi� gracza." NL, ch);
		return;
	}

	//added by Thanos (tajemniczy w�adcy ;) )
	if (IS_ADMIN( argument ) && !IS_ADMIN(ch->name))
	{
		ch_printf(ch, "%s jest jednym z W�ADC�W TEGO �WIATA !!!" NL, capitalize(argument));
		return;
	}

	strcat(buf, "0.");
	strcat(buf, argument);

	if ((victim = get_char_world(ch, buf)) == NULL)
	{
		send_to_char("Nie ma takiego gracza w grze." NL, ch);
		return;
	}

	//added by Thanos (tajemniczy w�adcy ;) )
	if (IS_ADMIN( victim->name ) && !IS_ADMIN(ch->name))
	{
		ch_printf(ch, "%s jednym z W�ADC�W TEGO �WIATA !!!" NL, capitalize(argument));
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("To nie jest gracz!" NL, ch);
		return;
	}

	// Poprawka na bug omijajacy system znajomosci - Pixel
	if (str_cmp(victim->name, argument))
	{
		send_to_char("Nie ma takiego gracza w grze." NL, ch);
		return;
	}

	ch_printf(ch, "%s jest %s rasy %s na poziomie %d." NL, victim->name,
	MALE( victim ) ? "m�czyzn�" : FEMALE(victim) ? "kobiet�" : "czym� nieokre�lonym",
			!strcmp(victim->race->name, "Human") ? "ludzkiej" : victim->race->name, victim->top_level);
	if (victim->pcdata->clan)
	{
		MEMBER_DATA *member;
		int i = 0;

		sprintf(buf, "Nale�y do:" NL);
		for (auto* clan : clan_list)
			if ((!IS_SET(clan->flags, CLAN_NOINFO) || get_member(clan, ch->name) || IS_IMMORTAL(ch))
					&& (member = get_member(clan, victim->name)) && member->status >= CLAN_MEMBER)
			{
				i++;
				sprintf(buf2, "-%s: %s" NL, CLANTYPE(clan, 1), clan->name);
				strcat(buf, buf2);
			}
		if (i)
			send_to_char(buf, ch);

	}

	/*
	 if(victim->pcdata->homepage)
	 ch_printf(ch, "Strona domowa %s znajduje si� pod adresem %s.\n\r",
	 victim->przypadki[1],
	 victim->pcdata->homepage);*/

	if (*victim->pcdata->bio)
		ch_printf(ch, "Dane osobowe %s:" EOL "%s" EOL, victim->przypadki[1], victim->pcdata->bio);

	if (IS_IMMORTAL(ch))
	{
		send_to_char("----------------------------------------------------" NL, ch);

		send_to_char("Info for immortals:" NL, ch);

		ch_printf(ch, NL "Location: %d (%s)." NL, victim->in_room->vnum, victim->in_room->name);

		if (victim->pcdata->authed_by && victim->pcdata->authed_by[0] != '\0')
			ch_printf(ch, "Authorized by %s." NL, victim->pcdata->authed_by);

		ch_printf(ch, "Killed %d mobiles, died from a hand of mobile %d times." NL, victim->pcdata->mkills, victim->pcdata->mdeaths);

		if (victim->pcdata->pkills || victim->pcdata->pdeaths)
			ch_printf(ch, "Killed %d players, and died from a hand of player %d times." NL, victim->pcdata->pkills,
					victim->pcdata->pdeaths);

		if (victim->pcdata->illegal_pk)
			ch_printf(ch, "Committed %d illegal player kills." NL, victim->pcdata->illegal_pk);

		ch_printf(ch, "%selled." NL, (victim->pcdata->release_date == 0) ? "Not h" : "H");

		if (victim->pcdata->release_date != 0)
			ch_printf(ch, "Isolated by %s. Released date: %24.24s." NL, victim->pcdata->isolated_by, ctime(&victim->pcdata->release_date));

		if (victim->pcdata->unsilence_date != 0)
			ch_printf(ch, "Silenced by %s. Unsilence date: %24.24s." NL, victim->pcdata->silenced_by,
					ctime(&victim->pcdata->unsilence_date));

		if (victim->desc && victim->desc->host) /* added by Gorog */
		{
			sprintf(buf2, "%s's IP info: %s ", victim->name, victim->desc->hostip);
			if (get_trust(ch) >= LEVEL_GOD)
			{
				strcat(buf2, victim->desc->user);
				strcat(buf2, "@");
				strcat(buf2, victim->desc->host);
			}
			strcat(buf2, NL);
			send_to_char(buf2, ch);
		}

	}
}

DEF_DO_FUN( pager )
{
	char arg[MAX_INPUT_LENGTH];

	if (IS_NPC(ch))
		return;
	argument = one_argument(argument, arg);
	if (!*arg)
	{
		if (IS_SET(ch->pcdata->flags, PCFLAG_PAGERON))
			do_config(ch, (char*) "-pager");
		else
			do_config(ch, (char*) "+pager");
		return;
	}
	if (!is_number(arg))
	{
		send_to_char("Ustawi� pager na ile linii?" NL, ch);
		return;
	}
	ch->pcdata->pagerlen = atoi(arg);
	if (ch->pcdata->pagerlen < 5)
		ch->pcdata->pagerlen = 5;
	ch_printf(ch, "Ekran zatrzymuje si� po wy�wietleniu %d linii." NL, ch->pcdata->pagerlen);
	return;
}
