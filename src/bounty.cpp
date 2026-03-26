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
 *		            Bounty Hunter Module    			   *
 *                    (  and area capturing as well  )                      *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <list>
#include <SWMobBounty.h>
#include <SWPlayerBounty.h>
#include <SWDataBase.h>
#include <SWTimeStamp.h>
#include "mud.h"

using namespace std;

std::list<INFORM_DATA*> inform_list;
list<SWBounty *> bounties;

void load_informs()
{
	FILE * fp;
	char * target;
	INFORM_DATA * inform;

	inform_list.clear();

	RESERVE_CLOSE;
	if ((fp = fopen(INFORM_LIST, "r")) == NULL)
	{
		perror(INFORM_LIST);
		RESERVE_OPEN;
		return;
	}

	for (;;)
	{
		target = feof(fp) ? (char *) "$" : fread_word(fp);

		if (target[0] == '$')
			break;

		CREATE( inform, INFORM_DATA, 1 );
		STRDUP( inform->attacker, target );
		STRDUP( inform->victim, fread_word( fp ) );
		inform->time = fread_number(fp);
		inform->reported = fread_number(fp);
		inform_list.push_back(inform);
	}
	fclose(fp);
	if (!sysdata.silent)
		log_string(" Done informs" );
	RESERVE_OPEN;
	return;
}

void save_informs()
{
	INFORM_DATA * inform;
	FILE *fpout;

	fpout = fopen(INFORM_LIST, "w");
	if (!fpout)
	{
		bug( "FATAL: cannot open inform.lst for writing!" NL, 0 );
		perror(INFORM_LIST);
		return;
	}
	for (auto* inform : inform_list)
	{
		fprintf(fpout, "%s %s %ld %d\n", inform->attacker, inform->victim,
				(unsigned long) inform->time, inform->reported);
	}
	fprintf(fpout, "$\n");
	fclose(fpout);
}

INFORM_DATA *get_inform(char *attacker, char *vict)
{
	INFORM_DATA * inf;

	for (auto* inf : inform_list)
		if (!str_cmp(inf->attacker, attacker) && !str_cmp(inf->victim, vict))
			return inf;

	return NULL;
}

INFORM_DATA *add_inform(CHAR_DATA *victim, CHAR_DATA *attacker)
{
	INFORM_DATA * inform;
	char buf[MSL];

	if (!victim || !attacker)
		return NULL;

	if (victim == attacker)
		return NULL;

	if (IS_NPC( victim ))
	{
		if (victim->master && victim->master != attacker)
			victim = victim->master;
		else
			return NULL;
	}

	if (IS_NPC( attacker ))
	{
		if (attacker->master && attacker->master != victim)
			attacker = attacker->master;
		else
			return NULL;
	}

	/* Trog: Thanos nie ruszaj tego! Immo niech maja spokoj. */
	if (IS_IMMORTAL( attacker ) || IS_IMMORTAL( victim ))
		return NULL;

	/* Trog: Thanos nie ruszaj tego! Immo (i nie newbie) niech maja spokoj. */
        /*if ((IS_IMMORTAL( attacker ) || IS_IMMORTAL( victim ))
                        && !IS_NEWBIE( victim ))
                return NULL;
           Pixel: Albo jestem pijany albo warunek powyzej jest bez sensu */

        /* Pixel: Jezeli ofiara nie jest newbie to ciul mudowi do tego */
        if (!IS_NEWBIE(victim))
            return NULL;

	if ((victim->in_room && (IS_SET( victim->in_room->room_flags, ROOM_KILLARENA )
			|| IS_SET( victim->in_room->room_flags, ROOM_SAFEARENA ))) || (victim->pcdata && victim->pcdata->clan
			&& IS_ORDER( victim->pcdata->clan ) && attacker->pcdata && attacker->pcdata->clan
			&& IS_ORDER( attacker->pcdata->clan )))
	{
		sprintf(buf, "Legal PKill: Killer: %s, Victim: %s, Room: %d", attacker->name, victim->name,
				victim->in_room->vnum);
		log_string( buf );
		return NULL;
	}
	else
	{
		if (victim->in_room)
		{
			sprintf(buf, "Illegal PKill: Killer: %s, Victim: %s, Room: %d", attacker->name, victim->name,
					victim->in_room->vnum);
		}
		else
		{
			sprintf(buf, "Illegal PKill: Killer: %s, Victim: %s, Room: NULL", attacker->name, victim->name);
		}
		log_string( buf );
	}

	if (IS_SET( attacker->act, PLR_KILLER ) || IS_SET( victim->act, PLR_KILLER ))
		return NULL;

	if (get_inform(attacker->name, victim->name))
		return NULL;

	ch_printf(attacker, "Mo�esz teraz zosta� uznan%s MORDERC� !!!" NL,
			SEX_SUFFIX_YAE( attacker ));

	CREATE( inform, INFORM_DATA, 1 );
	STRDUP( inform->victim, victim->name );
	STRDUP( inform->attacker, attacker->name );

	inform->time = current_time;
	inform->reported = false;
	inform_list.push_back(inform);

	log_string( "Adding inform to list" );
	save_informs();
	return inform;
}

DEF_DO_FUN( inform )
{
	INFORM_DATA * inf;

	if (IS_NPC( ch ))
	{
		send_to_char(
				"A co ty chcesz donosi�? Po to tu jeste� by ci� zabito ;-)." NL,
				ch);
		return;
	}

	if (*argument)
	{
		if ((inf = get_inform(argument, ch->name)) == NULL)
		{
			send_to_char(
					"Nikt taki ci� ostatnio nie skrzywdzi�. Panikujesz..." NL,
					ch);
			return;
		}
		if (inf->reported)
			send_to_char("Spokojnie. To jest ju� zg�oszone." NL, ch);
		else
		{
			inf->reported = true;
			/* reszt� zajmie si� update_informs() */
			send_to_char(
					"Ok. Mistrz Gry w�a�nie dowiedzia� si� o tym wyst�pku." NL,
					ch);
			save_informs();
		}
		return;
	}
	else
	{
		int i = 0;

		send_to_char(
				"Oto lista os�b, kt�re ostatnio pr�bowa�y ci� atakowa�:" NL, ch);

		for (auto* inf : inform_list)
		{
			if (!str_cmp(inf->victim, ch->name))
			{
				i++;
				ch_printf(ch, "   " FB_CYAN "%s" PLAIN ": (%s) %s", inf->attacker,
						inf->reported ? "zg�oszone" : "nie zg�oszone", SWTimeStamp(inf->time).getDescriptive().c_str());
			}
		}

		if (!i)
			send_to_char("   Nikt ci� nie atakowa� ostatnio." NL, ch);
	}

	return;
}

DEF_DO_FUN( garota )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *victim;

	if (!check_knowledge(ch, gsn_garota))
		return;

	argument = one_argument(argument, arg);

	switch (ch->substate)
	{
	default:
		if (arg[0] == '\0')
		{
			send_to_char("Kogo chcesz udusi�?" NL, ch);
			return;
		}

		if (!(victim = get_char_room(ch, arg)))
		{
			send_to_char("Nie ma tu nikogo takiego." NL, ch);
			return;
		}

		if (IS_NPC( ch ))
		{
			send_to_char(
					"Mob nie mo�e dusi�, ale za to mo�e by� uduszony :P" NL, ch);
			return;
		}

		if (ch == victim)
		{
			send_to_char("Chcesz pope�ni� samob�jstwo ???" NL, ch);
			return;
		}

		if (is_safe(ch, victim))
			return;

		if (IS_NPC( victim ) && IS_SET( victim->act, ACT_DROID ))
		{
			send_to_char(
					"Nie bardzo wiesz czy owijanie drutem szyi droida ma sens..." NL,
					ch);
			return;
		}

		if (ch->mount)
		{
			send_to_char(
					"Nie mo�esz tego zrobi� nie stoj�c o w�asnych si�ach." NL,
					ch);
			return;
		}

		if (victim->top_level + 10 >= ch->top_level)
		{
			sprintf(buf,
					"Nie uda ci si� podej�� %s. Za ma�o jeszcze umiesz..." NL,
					PERS(victim, ch,1));
			send_to_char(buf, ch);
			return;
		}

		if (ch->fighting)
		{
			send_to_char("Przecie� teraz walczysz!" NL, ch);
			return;
		}

		if (victim->fighting)
		{
			send_to_char(
					"Nie mo�esz tego zrobi�. Twoja ofiara ju� ma z kim walczy�." NL,
					ch);
			return;
		}

		act(FG_BLACK,
				"Przygotowujesz si� do zaci�ni�cia drutu na szyi $N$1...", ch,
				NULL, victim, TO_CHAR);
		act(FG_BLACK,
				"$n stara si� zacisn�� drut na twojej szyi! B�dzie bola�o!!!",
				ch, NULL, victim, TO_VICT);
		act(FG_BLACK, "$n pr�buje zacisn�� drut na szyi $N$1.", ch, NULL,
				victim, TO_NOTVICT);

		add_timer(ch, TIMER_DO_FUN, 3, do_garota, 1);
		STRDUP( ch->dest_buf, arg );
		return;

	case SUB_TIMER_DO_ABORT:
		ch_printf(ch, "Za p�no! Czujesz, �e musisz ju� zaatakowa�!" NL);
		ch->substate = SUB_TIMER_CANT_ABORT;
		return;

	case 1:
		if (!*ch->dest_buf)
			return;
		strcpy(arg, ch->dest_buf);
		STRDUP( ch->dest_buf, "" );
		break;
	}

	if (!(victim = get_char_room(ch, arg)))
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	if (!IS_NPC( victim ))
	{
		send_to_char(
				"Zabicie gracza w taki spos�b mog�oby mu si� nie spodoba�..." NL,
				ch);
		return;
	}

	if (number_percent() > ch->pcdata->learned[gsn_garota])
	{
		send_to_char("Nie uda�o si�." NL, ch);
		learn_from_failure(ch, gsn_garota);
		multi_hit(victim, ch, gsn_garota);
		return;
	}
	ch->substate = SUB_NONE;

	learn_from_success(ch, gsn_garota);
	act(FB_RED, "S�yszysz �miertelny j�k $N$1.", ch, NULL, victim, TO_CHAR);
	act(FB_RED, "S�yszysz �miertelny j�k $N$1.", ch, NULL, victim, TO_NOTVICT);
	set_cur_char(victim);
	check_killer(ch, victim);
	raw_kill(ch, victim, 0);
	return;
}

inline bool is_bounty_officer(CHAR_DATA *officer)
{
	return IS_NPC(officer) && IS_SET(officer->act, ACT_BOUNTYOFFICER);
}

void load_bounties()
{
	SWDataBase db;
	db.singleQuery("SELECT * FROM bounty ORDER BY id");
	list<SWString> columns;
	columns.push_back("id");
	columns.push_back("class");
	columns.push_back("reward");
	columns.push_back("placed_by");
	columns.push_back("hold_by");
	columns.push_back("wanted");
	SWQueryResult *result = db.getResult(columns);

	if (!result)
	{
		return;
	}

	for (SWQueryResult::size_type i = 0; i < result->size(); i++)
	{
		SWString swclass = (*result)[i]["class"];
		SWBounty *b;
		try
		{
			b
					= dynamic_cast<SWBounty *> (SWClassLoader::getFactory(
							swclass)->createInstance());
		} catch (const SWException &e)
		{
			bug( "%s", e.what() );
			continue;
		}
		b->setId(SWInt::fromString((*result)[i]["id"]));
		b->setReward(SWInt::fromString((*result)[i]["reward"]));
		b->setPlacedBy((*result)[i]["placed_by"]);
		b->setHoldBy(SWInt::fromString((*result)[i]["hold_by"]));
		b->setWanted((*result)[i]["wanted"]);

		bounties.push_back(b);
	}

	if (!sysdata.silent)
		log_string(" Done bounties " );
}

void add_bounty(SWBounty *bounty)
{
	bounty->persist();
}

void delete_bounty(SWBounty *bounty)
{
	bounty->purge();
}

DEF_DO_FUN( bounties )
{
	if (!*argument)
	{
		send_to_char("Sk�adnia: bounties <oficer_bounty>" NL, ch);
		return;
	}

	CHAR_DATA *officer = get_char_room(ch, argument);
	if (!officer)
	{
		ch_printf(ch, "Nie ma tu nikogo takiego." NL);
		return;
	}
	else if (!is_bounty_officer(officer))
	{
		ch_tell(officer, ch,
				"Nie jestem zainteresowany takimi czarnymi, niehumanitarnymi interesami!");
		return;
	}

	bool found = false;
	for (list<SWBounty *>::iterator it = bounties.begin(); it != bounties.end(); it++)
	{
		if ((*it)->getHoldBy() == officer->pIndexData->vnum)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		ch_tell(officer, ch,
				"Aktualnie nie mam �adnych zlece� godnych twojego czasu.");
		return;
	}

	if (IS_SET(ch->act,PLR_HOLYLIGHT))
	{
		send_to_char(
				FB_CYAN "[" FB_YELLOW "Zleceniodawca" PLAIN "   " FB_CYAN "][" FB_YELLOW "Poszukiwany" PLAIN "                     " FB_CYAN "][" FB_YELLOW "Nagroda" PLAIN "             " FB_CYAN "]" EOL,
				ch);
		for (list<SWBounty *>::iterator it = bounties.begin(); it
				!= bounties.end(); it++)
		{
			if ((*it)->getHoldBy() == officer->pIndexData->vnum)
			{
				ch_printf(
						ch,
						FG_CYAN "[" PLAIN "%-16s" FG_CYAN "][" PLAIN "%-32s" FG_CYAN "][" PLAIN "%20ld" FG_CYAN "]" EOL,
						(*it)->getPlacedBy().c_str(), strip_colors(
								(*it)->getWantedName().c_str(), 32),
						(int) (*it)->getReward());
			}
		}
	}
	else
	{
		send_to_char(
				FB_CYAN "[" FB_YELLOW "Poszukiwany" PLAIN "                                       " FB_CYAN "][" FB_YELLOW "Nagroda" PLAIN "             " FB_CYAN "]" EOL,
				ch);
		for (list<SWBounty *>::iterator it = bounties.begin(); it
				!= bounties.end(); it++)
		{
			if ((*it)->getHoldBy() == officer->pIndexData->vnum)
			{
				ch_printf(ch,
						FG_CYAN "[" PLAIN "%-50s" FG_CYAN "][" PLAIN "%20ld]" EOL,
						strip_colors((*it)->getWantedName().c_str(), 50),
						(int) (*it)->getReward());
			}
		}
	}

}

/*!
 * To samo co bounties, ale pokazuje dla wszystkich oficerow
 * */
DEF_DO_FUN( allbounties )
{
	if (bounties.size() < 1)
	{
		send_to_char("Aktualnie nie ma �adnych zlece�." NL, ch);
		return;
	}

	send_to_char(
			FB_CYAN "[" FB_YELLOW "Oficer" PLAIN " " FB_CYAN "][" FB_YELLOW "Zleceniodawca" PLAIN " " FB_CYAN "][" FB_YELLOW "Poszukiwany" PLAIN "              " FB_CYAN "][" FB_YELLOW "Nagroda" PLAIN "             " FB_CYAN "]" EOL,
			ch);
	for (list<SWBounty *>::iterator it = bounties.begin(); it != bounties.end(); it++)
	{
		ch_printf(
				ch,
				FG_CYAN "[" PLAIN "%7d" FG_CYAN "][" PLAIN "%-14s" FG_CYAN "][" PLAIN "%-25s" FG_CYAN "][" PLAIN "%20ld" FG_CYAN "]" PLAIN "%d" NL,
				(int) (*it)->getHoldBy(), (*it)->getPlacedBy().c_str(),
				strip_colors((*it)->getWantedName().c_str(), 25),
				(int) (*it)->getReward(), (int) (*it)->getId());
	}
}

/*!
 * \a Trog
 * */
DEF_DO_FUN( addbounty )
{
	char arg[MIL];
	char arg1[MIL];

	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg1);
	if (!*arg || !*arg1 || (!*argument && ch->pcdata->remembered_mob_bounty
			< SWMobBounty::MIN_MOB_VNUM_TO_BOUNTY))
	{
		send_to_char(
				"Sk�adnia: addbounty <oficer_bounty> <nagroda> [ofiara]" NL, ch);
		return;
	}

	CHAR_DATA *officer = nullptr;
	for (auto* och : ch->in_room->people)
	{
		if (nifty_is_name_prefix(arg, och->name))
		{
			if (is_bounty_officer(och))
			{
				officer = och;
				break;
			}
			else
			{
				char buf[MSL];
				sprintf(
						buf,
						"Nie jestem zainteresowan%s takimi czarnymi, niehumanitarnymi interesami!",
						SEX_SUFFIX_YAE(och));
				ch_tell(och, ch, buf);
				return;
			}
		}
	}

	if (!officer)
	{
		send_to_char("Zlecic bounty mo�na jedynie oficerowi bounty." NL, ch);
		return;
	}

	int ammount = SWInt::fromString(arg1);
	if (ammount <= 0)
	{
		send_to_char("Pewnie! Tyle to jest warta TWOJA g�owa!!!" NL, ch);
		return;
	}
	if (ammount < SWBounty::MIN_REWARD)
	{
		ch_printf(ch, "Bounty musi by� warte przynajmniej %d kredytek." NL,
				(int) SWBounty::MIN_REWARD);
		return;
	}
	/* obliczamy prowizje */
	int agio = ammount * (10 + UMAX((officer->top_level / 10), 1)) / 100;
	if (ch->gold < ammount + agio)
	{
		if (ch->gold < ammount)
		{
			send_to_char("Nie masz tyle!" NL, ch);
		}
		else
		{
			send_to_char("Nie sta� ci� na prowizj�!" NL, ch);
		}
		return;
	}
	ch->gold -= ammount + agio;

	found = false;
	SWBounty *bounty = 0;
	SWString placedBy(ch->name);
	/* nie pozawlajmy na wystawianie bounty za moby, ktorych vnum < 11 */
	if (ch->pcdata->remembered_mob_bounty
			>= SWMobBounty::MIN_MOB_VNUM_TO_BOUNTY)
	{
		MID *mob = get_mob_index(ch->pcdata->remembered_mob_bounty);
		if (mob)
		{
			bounty = new SWMobBounty(ch->pcdata->remembered_mob_bounty,
					ammount, placedBy, officer->pIndexData->vnum);
			found = true;
			ch->pcdata->remembered_mob_bounty = 0;
			strcpy(arg1, mob->przypadki[1]);
		}
	}
	else
	{
		/* bounty za gracza, po prostu przeszukajmy katalog z plikami graczy */
		DIR *dir = opendir(PDATA_DIR);
		struct dirent *entry;

		if (dir)
		{
			while ((entry = readdir(dir)))
				if (entry->d_name[0] != '.')
					if (!str_cmp(argument, entry->d_name))
					{
						found = true;
						break;
					}

			if (found)
			{
				SWString wanted(argument);
				bounty = new SWPlayerBounty(wanted.makePersonName(), ammount,
						placedBy, officer->pIndexData->vnum);
			}
		}

		closedir(dir);
	}

	if (found)
	{
		bounties.push_back(bounty);
		add_bounty(bounty);
		ch_printf(ch, "Wystawiasz %d kredyt%s za g�ow�: %s." NL, (int) ammount,
				NUMBER_SUFF( (int)ammount, "ek", "ki", "ek" ),
				*argument ? SWString(argument).makePersonName().c_str() : arg1);
		ch_printf(ch, "%s pobiera %d kredyt%s prowizji." NL,
				officer->przypadki[0], agio,
				NUMBER_SUFF(agio, "ek", "ki", "ek"));
		ch_tell(officer, ch, "Mamy umow�...");
	}
	else
	{
		if (*argument)
		{
			ch_printf(ch, "Nie znasz nikogo takiego jak %s." NL, argument);
		}
		else
		{
			ch_printf(ch, "Nie znasz nikogo takiego." NL);
		}
	}
}

/*!
 * \a Trog
 * */
DEF_DO_FUN( delbounty )
{
	if (!*argument)
	{
		send_to_char("Sk�adnia: delbounty <id>" NL, ch);
		return;
	}

	int id = SWInt::fromString(argument);
	if (id < 1)
	{
		send_to_char("id must be greater than: 0." NL, ch);
		return;
	}
	for (list<SWBounty *>::iterator it = bounties.begin(); it != bounties.end(); it++)
	{
		if (id == (*it)->getId())
		{
			delete_bounty(*it);
			ch_printf(ch, "Bounty (%d) on %s withdrawn." NL, id,
					(*it)->getWantedName().c_str());
			delete *it;
			bounties.erase(it);
			return;
		}
	}

	ch_printf(ch, "No such bounty (%d)." NL, id);
}

SWBounty *is_bounty_victim_killed(CHAR_DATA *victim)
{
	for (list<SWBounty *>::iterator it = bounties.begin(); it != bounties.end(); it++)
	{
		if ((*it)->isBountyHunted(victim))
		{
			return *it;
		}
	}

	return 0;
}

SWBounty *is_bounty_completed(OBJ_DATA *proof, CHAR_DATA *officer)
{
	if (!is_bounty_officer(officer))
	{
		return 0;
	}

	for (list<SWBounty *>::iterator it = bounties.begin(); it != bounties.end(); it++)
	{
		if ((*it)->getHoldBy() == officer->pIndexData->vnum
				&& (*it)->completed(proof))
		{
			return *it;
		}
	}

	return 0;
}

SWBounty *is_bounty_object(OBJ_DATA *proof)
{
	for (list<SWBounty *>::iterator it = bounties.begin(); it != bounties.end(); it++)
	{
		if ((*it)->completed(proof))
		{
			return *it;
		}
	}

	return 0;
}

void finalize_bounty(SWBounty *bounty, CHAR_DATA *ch, CHAR_DATA *victim,
		OBJ_DATA *proof)
{
	char buf[MSL];

	int xp = proof->value[5];
	if (xp && !str_cmp(proof->owner_name, ch->name)
			&& bounty->getPlacedBy().find(ch->name) == string::npos)
	{
		gain_exp(ch, xp, HUNTING_ABILITY);
		sprintf(buf, "Zdobywasz %d punkt%s do�wiadczenia w �owieniu nagr�d." NL,
				xp, NUMBER_SUFF(xp, "", "y", "�w"));
		send_to_char(buf, ch);
	}
	sprintf(buf,
			"Zadanie wykonane. Nagroda wysoko�ci %ld kredytek jest twoja.",
			bounty->getReward());
	ch_tell(victim, ch, buf);
	ch->gold += bounty->getReward();

	sprintf(buf, "%s completed bounty placed by: %s on: %s, reward:  %ld",
			ch->name, bounty->getPlacedBy().c_str(),
			bounty->getWantedName().c_str(), bounty->getReward());
	to_channel(buf, CHANNEL_MONITOR, "Bounty", LEVEL_IMMORTAL);

	delete_bounty(bounty);
	bounties.remove(bounty);
	delete bounty;
}

int get_bounty_xp(CHAR_DATA *ch, CHAR_DATA *killer)
{
	int xp = 0;
	if (!IS_NPC(ch) && IS_NEWBIE(ch))
	{
		xp = exp_level(killer->skill_level[HUNTING_ABILITY]) - exp_level(
				killer->skill_level[HUNTING_ABILITY] + 2);
	}
	else
	{
		xp = (1 + ch->top_level) - killer->skill_level[HUNTING_ABILITY];
		if (xp < 1 && xp > -9)
		{
			xp = (1 + 8 - ::abs(xp)) * (IS_NPC(ch) ? 10 : 100);
		}
		else if (xp > 0)
		{
			xp *= IS_NPC(ch) ? 100 : 1000;
		}
		xp = UMAX(0,xp);
	}

	/* Trog: chwilowo zwiekszamy expa, zeby gracze polubili nowy system */
	xp *= 10;

	return xp;
}
