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
 * Michael Seifert, Hans Henrik S trfeldt, Tom Madsen, and Katja Nyboe.     *
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
 *		                Space Module Part Two  			   *
 ****************************************************************************/

#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"

/* local routines */
void fread_astro args( ( ASTRO_DATA *astro, FILE *fp ) );
bool load_astro_file args( ( const char *astrofile ) );
void write_astro_list args( ( void ) );
void do_astro args( ( CHAR_DATA *ch, char *argument ) );
bool has_map args( ( SPACE_DATA *starsystem , CHAR_DATA *ch ) );
void new_astro args( ( SHIP_DATA *ship ,int astrotype) );
void tracastro args( ( CHAR_DATA *ch, SHIP_DATA *ship, ASTRO_DATA *astro ) );
void astro_atack args( ( SHIP_DATA *ship , ASTRO_DATA *astro ) );
void calculate_number
		args( (CHAR_DATA *ch, char *arg_x, char *arg_y, SHIP_DATA *ship) )
		;
void new_starsystem args( (char *x, char *y) );
bool same_star_pos
		args( (float system, float system2, float arg, float arg2, int doklad, int doklad2) )
		;
void damage_ship_energy
		args( ( SHIP_DATA *ship , int min , int max , CHAR_DATA *ch , SHIP_DATA *killer ) )
		;

const char *const astro_names[ASTRO_MAX] =
{ "Wrak", "Asteroida", "�adunek", "Mina", "Kapsu�a ratunkowa", "Nieznane" };

const char *const module_type_name[MODULE_MAX] =
{ "LASER", "IONCANNON", "LAUNCHER M", "LAUNCHER T", "LAUNCHER R", "MINE LAYER",
		"COMPUTER", "RADAR", "TARGET CMP", "ASTRO CMP", "COMM", "SPY DEVICE",
		"HULL", "TANK", "CARGO STD", "CARGO PEP", "CARGO LQ", "CARGO OZ",
		"CARGO ENE", "CARGO RAD", "SHIELD", "CAMUFLAGE", "INTERDICT",
		"TRAC_BEAM", "TRAW", "HOMING BICOMM", "REACTOR", "ENGINE STD",
		"ENGINE MAN", "HYPERDRIVE", "AMMOH M", "AMMOH T", "AMMOH R", "CHAFS",
		"AMMO LM", "AMMO RM" };

const char *const module_name_nice[MODULE_MAX] =
{ "Dzia�ko Laserowe", "Dzia�ko Jonowe", "Wyrzutnia Pocisk�w",
		"Wyrzutnia Torped", "Wyrzutnia Rakiet", "Stawiacz Min",
		"G��wny Komputer Pok�adowy", "Konsola Radaru",
		"G��wny Komputer Celowniczy", "Komputer Astronavigacyjny",
		"Konsola ��czno�ci", "Komputer Namiaru, Rozpoznania i Identyfikacji",
		"Plaster Pancerza", "Zbiornik Paliwa", "�adownia Standardowa",
		"Modu� Pasa�erski", "Zbiornik na P�yny", "Zbiornik Wysokoci�nieniowy",
		"Bateria Protoplazmowa", "Magazyn Materia��w Radioaktywnych",
		"Generator Os�on Energetycznych", "Generator Pola Maskuj�cego",
		"Generator Studni Grawitacyjnej", "Konsola Promieni �ci�gaj�cych",
		"Tra� Przeciw Minowy", "Pluskwa Lokalizatora", "G��wny Reaktor",
		"Silnik Nap�dowy", "Silnik Manewrowy", "Motywator Hipernap�du",
		"�adownica Pocisk�w", "�adownica Torped", "�adownica Rakiet",
		"Wyrzutnik Wabi Termicznych", "�adownica Min Laserowych",
		"�adownica Min Rakietowych" };

const char *const ship_dock_type[DOCK_MAX] =
{ "Canaon", "Canaon 2", "Pelta", "Rupie", "Hs'a LuniGh'e" };

void extract_starsystem args( ( SPACE_DATA *starsystem ) );
void dockship args( ( SHIP_DATA *ship, char *arg ) );
void undockship args( ( SHIP_DATA *ship ) );
void update_crew args( ( void ) );
bool is_aggressive_to args( ( SHIP_DATA *ship, SHIP_DATA *target) );
bool is_metaaggressive_to args( ( SHIP_DATA *ship, SHIP_DATA *target) );
bool isnt_in_realspace args( ( SHIP_DATA *ship) );

/* from comm.c */
bool write_to_descriptor args( ( int desc, char *txt, int length ) );

/* from space.c */
bool autofly(SHIP_DATA *ship);
void sound_to_ship(SHIP_DATA *ship, char *argument);
void write_starsystem_list(void);

SHIP_INDEX_DATA* get_ship_index args( ( char * ship_index ) ); //shpconstr.c
SHIP_DATA* constr_ship args( ( SHIP_INDEX_DATA *shrec ) ); //shpconstr.c
void reset_ship_rooms args( ( SHIP_DATA *ship ) ); //space_reset.c

//Added by Thanos (from sw-fote)
DEF_DO_FUN( transship )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;
	SHIP_DATA *ship;

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	ship = get_ship(arg1);

	if (!ship)
	{
		send_to_char("No such ship." NL, ch);
		return;
	}

	if (arg1[0] == '\0')
	{
		send_to_char("Usage: transship <ship> [location]" NL, ch);
		return;
	}

	ship->shipstate = SHIP_READY;

	if (arg2[0] == '\0')
	{
		location = ch->in_room;
	}
	else
	{
		if ((location = find_location(ch, arg2)) == NULL)
		{
			send_to_char("No such location." NL, ch);
			return;
		}

		if (room_is_private(ch, location))
		{
			if (get_trust(ch) < LEVEL_GREATER)
			{
				send_to_char("That room is private right now." NL, ch);
				return;
			}
			else
			{
				send_to_char("Overriding private flag!" NL, ch);
			}
		}
	}

//    if ( ship->clazz != SHIP_SPACE_STATION && ship->type != MOB_SHIP )
//    {
	ship->location = location;
	extract_ship(ship);
	ship_to_room(ship, location);

	ship->lastdoc = ship->location;
	ship->shipstate = SHIP_DOCKED;
//     }

	if (ship->starsystem)
		ship_from_starsystem(ship, ship->starsystem);

	if (!ship->vnum)
		save_ship(ship);

	send_to_char("Ship Transfered." NL, ch);
}

void clear_temp_systems()
{
	int count;

	auto ss_snapshot = starsystem_list;
	for (auto* starsystem : ss_snapshot)
		if (IS_SET(starsystem->flags, STARS_FLAG_TEMP))
		{
			count = starsystem->ships.size();
			if (count == 0)
			{
				bug("%s (ships==0)", starsystem->filename);
				extract_starsystem(starsystem);
			}
		}
	write_starsystem_list();
	return;

}

void extract_starsystem(SPACE_DATA *starsystem)
{
	char filename[256];

	if (starsystem == NULL)
		return;
	sprintf(filename, "%s%s", STARS_DIR, starsystem->filename);
	unlink(filename);

	starsystem_list.remove(starsystem);
	free_starsystem(starsystem);
}

ASTRO_DATA* get_astro(char *name)
{
	for (auto* astro : astro_list)
	{
		if (!str_cmp(name, astro->filename))
			return astro;
	}
	for (auto* astro : astro_list)
	{
		if (nifty_is_name_prefix(name, astro->filename))
			return astro;
	}
	for (auto* astro : astro_list)
	{
		if (!str_cmp(name, astro->name))
			return astro;
	}
	for (auto* astro : astro_list)
	{
		if (nifty_is_name_prefix(name, astro->name))
			return astro;
	}
	return NULL;
}

DEF_DO_FUN( status )
{
	int chance, a, emp;
	SHIP_DATA *ship;
	SHIP_DATA *target;
	float range;
	TRANSPONDER_DATA *transponder;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(
				FB_RED "Musisz by� w kokpicie, na mostku lub w maszynowni statku by to zrobi�!" EOL,
				ch);
		return;
	}

	if (argument[0] == '\0')
		target = ship;
	else
		target = get_ship_here(argument, ship->starsystem);

	if (!known_biotech(ship, ch))
	{
		send_to_char(
				FB_RED "Jeste� wewn�trz wytworu obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." EOL,
				ch);
		return;
	}
	if (ship->shipstate1 != 0)
	{
		send_to_char(FB_RED "BzZzBzbbBzzZ......", ch);
		return;
	}
	if (target == NULL)
	{
		send_to_char(
				FB_RED "Nie ma w okolicy �adnego takiego statku." NL "Wpisz 'radar; by zobaczy� jakie inne statki znajduj� si� wok�." EOL,
				ch);
		return;
	}
	if (target->cloack == true && target != ship)
	{
		send_to_char(
				FB_RED "Nie ma w okolicy �adnego takiego statku." NL "Wpisz 'radar; by zobaczy� jakie inne statki znajduj� si� wok�." EOL,
				ch);
		return;
	}

	emp = 1;
	if (ship->starsystem && IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
		emp = 2;

	if ((srange((target->vx - ship->vx), (target->vy - ship->vy),
			(target->vz - ship->vz))
			> (ship->target_array * 1000 + ship->size / 10 + target->size) / emp)
			&& target != ship)
	{
		send_to_char(
				FB_RED "Ten statek jest za daleko, by mo�na by�o go przeskanowa�." EOL,
				ch);
		return;
	}

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_shipsystems]);
	if (number_percent() > chance)
	{
		send_to_char(
				FB_RED "Nie mo�esz doj�� do tego co oznaczaj� te wszystkie wska�niki." EOL,
				ch);
		learn_from_failure(ch, gsn_shipsystems);
		return;
	}

	act( PLAIN, "$n sprawdza stan licznik�w na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);

	pager_printf(ch, FB_BLUE "Nazwa: " FB_WHITE "%s" EOL, SHIPNAME(target));
	pager_printf(ch,
			FG_YELLOW "Transponder: " FB_YELLOW "%s " FG_YELLOW "Lot nr: " FB_YELLOW "%s" NL,
			target->transponder, target->sslook);
	pager_printf(ch,
			FG_YELLOW "Aktualne wsp�rz�dne solarne:" FB_YELLOW " %.0f %.0f %.0f" NL,
			target->vx, target->vy, target->vz);
	if (target != ship)
		pager_printf(ch,
				FG_YELLOW "Aktualne wsp�rz�dne localne:" FB_YELLOW " %.0f %.0f %.0f" NL,
				target->vx - ship->vx, target->vy - ship->vy,
				target->vz - ship->vz);
	pager_printf(ch,
			FG_YELLOW "Aktualne ustawienie:" FB_YELLOW " %.0f %.0f %.0f" EOL,
			target->hx, target->hy, target->hz);
	pager_printf(ch,
			FG_YELLOW "Aktualna pr�dko��:" FB_YELLOW " %.0f" FG_YELLOW "/%.0f" EOL,
			target->currspeed, target->realspeed);

	if (target->type != SHIP_YUUZHAN)
	{
		if (target->trawler > 0)
			pager_printf(ch, FB_RED "Tra�owiec" NL);
		if (target == ship && target->currjump != NULL
				&& target->shipstate == SHIP_READY)
		{
			range = srange((target->currjump->xpos - target->starsystem->xpos),
					(target->currjump->ypos - target->starsystem->ypos), 0) / 2;
			pager_printf(ch,
					FG_YELLOW "Wsp. skoku:" FB_YELLOW " %s  %.0f %.0f %.0f ",
					target->currjump->name, target->jx, target->jy, target->jz);
			if (range != 0)
				pager_printf(ch, " odleg�o��: %.0f" NL, range);
			else
				pager_printf(ch, NL);
		}
		pager_printf(ch,
				FG_YELLOW "Kad�ub:" FB_YELLOW " %d" FG_YELLOW "/%d  Stan napedu:" FB_YELLOW " %s" EOL,
				target->hull, target->maxhull,
				target->shipstate == SHIP_DISABLED ? "Uszkodzony" : "Gotowy");
		pager_printf(ch,
				FG_YELLOW "Tarcze:" FB_YELLOW " %d" FG_YELLOW "/%d   Energia(paliwo):" FB_YELLOW " %d" FG_YELLOW "/%d" EOL,
				target->shield, target->maxshield, target->energy,
				target->maxenergy);
		if (target->lasers > 0)
			pager_printf(ch,
					FG_YELLOW "Stan laser�w:" FB_YELLOW " %s  " FG_YELLOW "Aktualny cel:" FB_YELLOW " %s" EOL,
					target->statet0 == LASER_DAMAGED ? "Uszkodzone" : "Sprawne",
					target->target0 ? target->target0->sslook : "brak");
		if (target->ioncannons > 0)
			pager_printf(ch,
					FG_YELLOW "Stan dzialek jonowych:" FB_YELLOW " %s  " FG_YELLOW "Aktualny cel:" FB_YELLOW " %s" NL,
					target->statet0 == LASER_DAMAGED ? "Uszkodzone" : "Sprawne",
					target->target0 ? target->target0->sslook : "brak");
		pager_printf(ch,
				"" NL "" FG_YELLOW "Pociski:" FB_YELLOW " %d" FG_YELLOW "/%d  Torpedy: " FB_YELLOW "%d" FG_YELLOW "/%d  Rakiety: " FB_YELLOW "%d" FG_YELLOW "/%d  Stan wyrzutni:" FB_YELLOW " %s " NL,
				target->missiles, target->maxmissiles, target->torpedos,
				target->maxtorpedos, target->rockets, target->maxrockets,
				target->missilestate == MISSILE_DAMAGED ?
						"Uszkodzona" : "Sprawna");
		if (check_pilot(ch, target))
			pager_printf(ch,
					FG_YELLOW "W�asciciel : " FB_YELLOW "%-15s " FG_YELLOW "Pilot   : " FB_YELLOW "%s" NL
					FG_YELLOW "Drugi Pilot: " FB_YELLOW "%-15s " FG_YELLOW "In�ynier: " FB_YELLOW "%s" EOL,
					target->owner, target->pilot, target->copilot,
					target->engineer);
		if (check_pilot(ch, target) && target->maxcargo > 0)
			pager_printf(ch,
					FG_YELLOW "�adunek: " FB_YELLOW "%d/" FG_YELLOW "%d " FG_YELLOW "Z: " FB_YELLOW "%s " FG_YELLOW "Typu: " FB_YELLOW "%s&w" EOL,
					ship->cargo, ship->maxcargo,
					!ship->cargofrom ? "---" : ship->cargofrom,
					ship->cargotype > 0 ? cargo_names[ship->cargotype] : "---");
		if (target == ship)
		{
			pager_printf(ch,
					FG_YELLOW "Radar typ:" FB_YELLOW " Mk%-8d    " FG_YELLOW "System celowniczy typ:" FB_YELLOW " Mk%d" NL
					FG_YELLOW "Radio typ:" FB_YELLOW " Mk%-8d    " FG_YELLOW "System astronavig typ:" FB_YELLOW " Mk%d" NL,
					target->sensor, target->target_array, target->comm,
					target->astro_array);
			for (a = 0; auto* hangar : target->hangars)
				pager_printf(ch,
						FG_YELLOW "Hangar nr: " FB_YELLOW "%-4d " FG_YELLOW "status: " FB_YELLOW "%s" NL,
						a++,
						hangar->status == -1 ?
								"zamkni�ty" :
								(hangar->status == 0 ?
										"otwarty" : FB_RED "uszkodzony" PLAIN));
			for (a = 0; auto* dock : target->docks)
				pager_printf(ch,
						FG_YELLOW "R�kaw cumowniczy nr: " FB_YELLOW "%-4d " FG_YELLOW "stan: " FB_YELLOW "%s " FG_YELLOW "go��: " FB_YELLOW "%s" NL,
						a++,
						dock->status == 0 ?
								"Zabezpieczony" :
								(dock->status == 1 ? "Otwarty" : "Wy�amany"),
						dock->target == NULL ?
								"brak" :
								(know_trans(ship, dock->target) ?
										SHIPNAME(dock->target) :
										dock->target->transponder));
		}
		if (target == ship && target->tractorbeam > 0)
			pager_printf(ch,
					FG_YELLOW "Promienie �ci�gaj�ce:" FB_YELLOW " Moc:%d" NL,
					target->tractorbeam);
		if (target->maxcloack > 0 && target == ship && target->cloack > 0)
			pager_printf(ch,
					FG_YELLOW "Statek wyposa�ony w " FB_YELLOW MOD_BLINK "AKTYWNY" MOD_NORMAL FG_YELLOW " system maskuj�cy" NL);
		if (target->maxcloack > 0 && target == ship && target->cloack == 0)
			pager_printf(ch,
					FG_YELLOW "Statek wyposa�ony w system maskuj�cy" NL);
		if (target->interdict > 0 && target == ship && target->maxinterdict > 0)
			pager_printf(ch,
					FG_YELLOW "Generator pola wstrzymuj�cego: " FB_YELLOW MOD_BLINK "AKTYWNY" MOD_NORMAL FG_YELLOW "" NL);
		if (target->interdict == 0 && target == ship
				&& target->maxinterdict > 0)
			pager_printf(ch,
					FG_YELLOW "Generator pola wstrzymuj�cego: " FB_RED "PASYWNY" FG_YELLOW "" NL);
		if (autofly(target) && target == ship)
			pager_printf(ch,
					FG_YELLOW "Statek prowadzi AUTOPILOT" FG_YELLOW "" NL);
		pager_printf(ch, NL);
	}
	else
	{
		if (known_biotech(target, ch))
		{
			if (target->trawler > 0)
				pager_printf(ch, FB_RED "Neutralizator" NL);
			if (target == ship && target->currjump != NULL
					&& target->shipstate == SHIP_READY)
			{
				range = srange(
						(target->currjump->xpos - target->starsystem->xpos),
						(target->currjump->ypos - target->starsystem->ypos), 0)
						/ 2;
				pager_printf(ch,
						FG_YELLOW "Wsp. skoku:" FB_YELLOW " %s  %.0f %.0f %.0f ",
						target->currjump->name, target->jx, target->jy,
						target->jz);
				if (range != 0)
					pager_printf(ch, " odleg�o��: %.0f" NL, range);
				else
					pager_printf(ch, NL);
			}
			pager_printf(ch,
					FG_YELLOW "Skorupa:" FB_YELLOW " %d" FG_YELLOW "/%d  Stan napedu:" FB_YELLOW " %s" EOL,
					target->hull, target->maxhull,
					target->shipstate == SHIP_DISABLED ?
							"Uszkodzony" : "Gotowy");
			pager_printf(ch,
					FG_YELLOW "Os�ony:" FB_YELLOW " %d" FG_YELLOW "/%d   Energia(biopaliwo):" FB_YELLOW " %d" FG_YELLOW "/%d" EOL,
					target->shield, target->maxshield, target->energy,
					target->maxenergy);
			if (target->lasers > 0)
				pager_printf(ch,
						FG_YELLOW "Stan wyrzutni:" FB_YELLOW " %s  " FG_YELLOW "Aktualny cel:" FB_YELLOW " %s" EOL,
						target->statet0 == LASER_DAMAGED ?
								"Uszkodzone" : "Sprawne",
						target->target0 ? target->target0->sslook : "brak");
			if (target->ioncannons > 0)
				pager_printf(ch,
						FG_YELLOW "Stan wyrzutni jonowych:" FB_YELLOW " %s  " FG_YELLOW "Aktualny cel:" FB_YELLOW " %s" NL,
						target->statet0 == LASER_DAMAGED ?
								"Uszkodzone" : "Sprawne",
						target->target0 ? target->target0->sslook : "brak");
			pager_printf(ch,
					"" NL "" FG_YELLOW "Ofens1:" FB_YELLOW " %d" FG_YELLOW "/%d  Ofens2: " FB_YELLOW "%d" FG_YELLOW "/%d  Ofens3: " FB_YELLOW "%d" FG_YELLOW "/%d  Stan wyrzutni:" FB_YELLOW " %s " NL,
					target->missiles, target->maxmissiles, target->torpedos,
					target->maxtorpedos, target->rockets, target->maxrockets,
					target->missilestate == MISSILE_DAMAGED ?
							"Uszkodzona" : "Sprawna");
			if (check_pilot(ch, target))
				pager_printf(ch,
						FG_YELLOW "W�asciciel : " FB_YELLOW "%-15s " FG_YELLOW "Pilot   : " FB_YELLOW "%s" NL
						FG_YELLOW "Drugi Pilot: " FB_YELLOW "%-15s " FG_YELLOW "CmbMed: " FB_YELLOW "%s" EOL,
						target->owner, target->pilot, target->copilot,
						target->engineer);
			if (check_pilot(ch, target) && target->maxcargo > 0)
				pager_printf(ch,
						FG_YELLOW "�adunek: " FB_YELLOW "%d/" FG_YELLOW "%d " FG_YELLOW "Z: " FB_YELLOW "%s " FG_YELLOW "Typu: " FB_YELLOW "%s&w" EOL,
						ship->cargo, ship->maxcargo,
						!ship->cargofrom ? "---" : ship->cargofrom,
						ship->cargotype > 0 ?
								cargo_names[ship->cargotype] : "---");
			if (target == ship)
			{
				pager_printf(ch,
						FG_YELLOW "Radar typ:" FB_YELLOW " Mk%-8d    " FG_YELLOW "System celowniczy typ:" FB_YELLOW " Mk%d" NL
						FG_YELLOW "Radio typ:" FB_YELLOW " Mk%-8d    " FG_YELLOW "System astronavig typ:" FB_YELLOW " Mk%d" NL,
						target->sensor, target->target_array, target->comm,
						target->astro_array);
				for (a = 0; auto* hangar : target->hangars)
					pager_printf(ch,
							FG_YELLOW "Hangar nr: " FB_YELLOW "%-4d " FG_YELLOW "status: " FB_YELLOW "%s" NL,
							a++,
							hangar->status == -1 ?
									"zamkni�ty" :
									(hangar->status == 0 ?
											"otwarty" :
											FB_RED "uszkodzony" PLAIN));
				for (a = 0; auto* dock : target->docks)
					pager_printf(ch,
							FG_YELLOW "B�ona cumownicza nr: " FB_YELLOW "%-4d " FG_YELLOW "stan: " FB_YELLOW "%s " FG_YELLOW "go��: " FB_YELLOW "%s" NL,
							a++,
							dock->status == 0 ?
									"Zabezpieczona" :
									(dock->status == 1 ? "Otwarta" : "Wy�amana"),
							dock->target == NULL ?
									"brak" :
									(know_trans(ship, dock->target) ?
											SHIPNAME(dock->target) :
											dock->target->transponder));
			}
			if (target == ship && target->tractorbeam > 0)
				pager_printf(ch,
						FG_YELLOW "Bassale �ci�gaj�ce:" FB_YELLOW " Moc:%d" NL,
						target->tractorbeam);
			if (target->maxcloack > 0 && target == ship && target->cloack > 0)
				pager_printf(ch,
						FG_YELLOW "Statek wyposa�ony w " FB_YELLOW MOD_BLINK "AKTYWNY" MOD_NORMAL FG_YELLOW " system maskuj�cy" NL);
			if (target->maxcloack > 0 && target == ship && target->cloack == 0)
				pager_printf(ch,
						FG_YELLOW "Statek wyposa�ony w system maskuj�cy" NL);
			if (target->interdict > 0 && target == ship
					&& target->maxinterdict > 0)
				pager_printf(ch,
						FG_YELLOW "Studnia grawitacyjna : " FB_YELLOW MOD_BLINK "AKTYWNA" MOD_NORMAL FG_YELLOW "" NL);
			if (target->interdict == 0 && target == ship
					&& target->maxinterdict > 0)
				pager_printf(ch,
						FG_YELLOW "Studnia grawitacyjna : " FB_RED "PASYWNA" FG_YELLOW "" NL);
			if (autofly(target) && target == ship)
				pager_printf(ch,
						FG_YELLOW "Statek prowadzi Yammamosk" FG_YELLOW "" NL);
			pager_printf(ch, NL);
		}
		else
		{
			pager_printf(ch,
					NL "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." NL);
		}
	}

	learn_from_success(ch, gsn_shipsystems);

	if (target != ship)
	{
		for (auto* oldtrans : ship->transponders)
		{
			if (!str_cmp(oldtrans->number, target->transponder))
			{
				if (strcmp(oldtrans->shipname, target->filename))
				{
					ship->transponders.remove(oldtrans);
					free_transponder(oldtrans);
					break;
				}
				else
				{
					return;
				}
			}
			if (!strcmp(oldtrans->shipname, target->filename))
			{
				ship->transponders.remove(oldtrans);
				free_transponder(oldtrans);
				break;
			}
		}
		CREATE(transponder, TRANSPONDER_DATA, 1);
		STRDUP(transponder->number, target->transponder);
		STRDUP(transponder->shipname, target->filename);
		ship->transponders.push_back(transponder);
		save_ship(ship);
	}
}

DEF_DO_FUN( closebay )
{
	int chance;
	SHIP_DATA *ship;
	SHIP_DATA *target = 0;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	HANGAR_DATA *hangar = 0;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if ((ship = ship_from_coseat(ch->in_room)) == NULL && (ship =
			ship_from_hangar(ch->in_room)) == NULL)
	{
		send_to_char(
				FB_RED "Musisz siedzi� w fotelu drugiego pilota lub sta� w hangarze!" EOL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(
				FB_RED "W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL,
				ch);
		return;
	}

	if (arg1[0] == '\0' || is_number(arg1))
	{
		if (arg1[0] == '\0')
		{
			target = ship;
			hangar = hangar_from_room(target, ch->in_room->vnum);
		}
		else if ( is_number(arg1))
		{
			target = ship;
			hangar = get_hangar(target, atoi(arg1));
		}

		if (!hangar)
		{
			send_to_char(FB_RED "Kt�ry hangar chcesz zamkn��?" EOL, ch);
			return;
		}
	}
	else if ((target = get_ship_here(arg1, ship->starsystem)) != NULL)
	{
		if ((hangar = get_hangar(target, atoi(arg2))) == NULL)
		{
			send_to_char(FB_RED "Ten statek nie ma hangaru o tym numerze!" EOL,
					ch);
			return;
		}
	}
	else
	{
		send_to_char(FB_RED "Nie ma w okolicy �adnego takiego statku." NL
		"Wpisz 'radar; by zobaczy� jakie inne statki znajduj� si� wok�." EOL,
				ch);
		return;
	}

	if (target->cloack == true && target != ship)
	{
		send_to_char(FB_RED "Nie ma w okolicy �adnego takiego statku." NL
		"Wpisz 'radar; by zobaczy� jakie inne statki znajduj� si� wok�." EOL,
				ch);
		return;
	}

	if ((srange((target->vx - ship->vx), (target->vy - ship->vy),
			(target->vz - ship->vz)) > 1500) && target != ship)
	{
		send_to_char(
				FB_RED "Ten statek jest za daleko, by twoje rozkazy zostaly zrozumiane." EOL,
				ch);
		return;
	}

	if (!get_comlink(ch) && target != ship)
	{
		send_to_char(
				"Potrzebujesz urz�dzenia komunikacyjnego by to zrobi�!" EOL,
				ch);
		return;
	}

	if (!check_pilot(ch, target))
	{
		send_to_char(FB_RED "Hej! To nie tw�j statek!" EOL, ch);
		return;
	}

	if (target->shipstate1 != 0)
	{
		send_to_char(FB_RED "W odpowiedzi s�yszysz: BzZzzbBbbzZzz..." NL, ch);
		return;
	}

	if (!has_hangar(target))
	{
		send_to_char(FB_RED "Ten statek nie ma hangaru. Co chcesz zamkn��!" EOL,
				ch);
		return;
	}

	if (hangar->status == -1)
	{
		send_to_char(FB_RED "Ten hangar jest juz zamkni�ty!" EOL, ch);
		return;
	}

	act( PLAIN, "$n poci�ga za d�wigni� na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_shipsystems]);

	if (number_percent() < chance)
	{
		hangar->status = -1;
		send_to_char("&GZamykasz hangar." EOL, ch);
		echo_to_ship(target, FB_YELLOW "Hangar zamkniety.");

		if (!target->cloack)
		{
			sprintf(buf, FB_YELLOW "%s zamyka hangar.", target->sslook);
			echo_to_system(target, buf, NULL);
		}

		learn_from_success(ch, gsn_shipsystems);
	}
	else
	{
		send_to_char("&GTo nie twoj dzien. Hangar nadal jest otwarty." EOL, ch);
		learn_from_failure(ch, gsn_shipsystems);
	}

	return;
}

DEF_DO_FUN( openbay )
{
	int chance;
	SHIP_DATA *ship;
	SHIP_DATA *target = 0;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	HANGAR_DATA *hangar = 0;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if ((ship = ship_from_coseat(ch->in_room)) == NULL && (ship =
			ship_from_hangar(ch->in_room)) == NULL)
	{
		send_to_char(
				FB_RED "Musisz siedzi� w fotelu drugiego pilota lub sta� w hangarze!" EOL,
				ch);
		return;
	}
	if (isnt_in_realspace(ship))
	{
		send_to_char(
				FB_RED "W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL,
				ch);
		return;
	}

	if (arg1[0] == '\0' || is_number(arg1))
	{
		if (arg1[0] == '\0')
		{
			target = ship;
			hangar = hangar_from_room(target, ch->in_room->vnum);
		}
		else if ( is_number(arg1))
		{
			target = ship;
			hangar = get_hangar(target, atoi(arg1));
		}

		if (!hangar)
		{
			send_to_char(FB_RED "Kt�ry hangar chcesz otworzy�?" EOL, ch);
			return;
		}
	}
	else if ((target = get_ship_here(arg1, ship->starsystem)) != NULL)
	{
		if ((hangar = get_hangar(target, atoi(arg2))) == NULL)
		{
			send_to_char(FB_RED "Ten statek nie ma hangaru o tym numerze!" EOL,
					ch);
			return;
		}
	}
	else
	{
		send_to_char(FB_RED "Nie ma w okolicy �adnego takiego statku." NL
		"Wpisz 'radar; by zobaczy� jakie inne statki znajduj� si� wok�." EOL,
				ch);
		return;
	}

	if (target->cloack == true && target != ship)
	{
		send_to_char(FB_RED "Nie ma w okolicy �adnego takiego statku." NL
		"Wpisz 'radar; by zobaczy� jakie inne statki znajduj� si� wok�." EOL,
				ch);
		return;
	}

	if ((srange((target->vx - ship->vx), (target->vy - ship->vy),
			(target->vz - ship->vz)) > 1500) && target != ship)
	{
		send_to_char(
				FB_RED "Ten statek jest za daleko, by twoje rozkazy zostaly zrozumiane." EOL,
				ch);
		return;
	}

	if (!get_comlink(ch) && target != ship)
	{
		send_to_char(
				"Potrzebujesz urz�dzenia komunikacyjnego by to zrobi�!" EOL,
				ch);
		return;
	}

	if (!check_pilot(ch, target) && target != ship)
	{
		send_to_char(FB_RED "Hej! To nie tw�j statek!" EOL, ch);
		return;
	}

	if (!check_pilot(ch, target) && target == ship)
	{
		send_to_char(FB_RED "Za�oga odmawia otwarcia hangaru!" EOL, ch);
		return;
	}

	if (target->shipstate1 != 0)
	{
		send_to_char(FB_RED "W odpowiedzi s�yszysz: BzZzzbBbbzZzz..." NL, ch);
		return;
	}

	if (!has_hangar(target))
	{
		send_to_char(
				FB_RED "Ten statek nie ma hangaru. Co chcesz otworzyc!" EOL,
				ch);
		return;
	}

	if (hangar->status == 0)
	{
		send_to_char(FB_RED "Hangar jest juz otwarty!" EOL, ch);
		return;
	}

	act( PLAIN, "$n poci�ga za d�wigni� na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_shipsystems]);

	if (number_percent() < chance)
	{
		hangar->status = 0;
		send_to_char("&GOtwierasz hangar." EOL, ch);
		echo_to_ship(target, FB_YELLOW "Hangar otwarty.");

		if (!target->cloack)
		{
			sprintf(buf, FB_YELLOW "%s otwiera hangar.", target->sslook);
			echo_to_system(target, buf, NULL);
		}
		learn_from_success(ch, gsn_shipsystems);
	}
	else
	{
		send_to_char("&GTo nie twoj dzien. Hangar nadal jest zamkniety." EOL,
				ch);
		learn_from_failure(ch, gsn_shipsystems);
	}
}

DEF_DO_FUN( tractorbeam )
{

	char arg[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	int chance;
	SHIP_DATA *ship;
	SHIP_DATA *target;
	ASTRO_DATA *astro;
	HANGAR_DATA *hangar;
	char buf[MAX_STRING_LENGTH];
	char destbuf[MAX_STRING_LENGTH];

	strcpy(arg, argument);
	argument = one_argument(argument, arg1);

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char( FB_RED "Musisz byc w kokpicie statku!" NL, ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(
				FB_RED "W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL,
				ch);
		return;
	}

	if ((ship = ship_from_coseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz siedziec w fotelu drugiego pilota" NL, ch);
		return;
	}

	if (!check_pilot(ch, ship))
	{
		send_to_char( FB_RED "HEY!! To nie Tw�j statek!" NL, ch);
		return;
	}

	if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
	{
		send_to_char(FB_RED "W systemie panuje burza elektromagnetyczna," NL
		"promienie �ci�gaj�ce odmawiaj� posusze�stwa!" NL, ch);
		return;
	}

	if (ship->tractorbeam == 0)
	{
		send_to_char(
				FB_RED "Musia�by� pokusi� si� o kupno promieni �ci�gaj�cych!" NL,
				ch);
		return;
	}

	if (!has_hangar(ship))
	{
		send_to_char(FB_RED "Twoj statek nie ma hangaru." NL, ch);
		return;
	}

	if ((hangar = get_hangar(ship, atoi(argument))) == NULL)
	{
		send_to_char(FB_RED "Musisz wskazac hangar!" NL, ch);
		return;
	}

	if (hangar->status == -1)
	{
		send_to_char(FB_RED "Wskazany hangar musi by� otwarty!" NL, ch);
		return;
	}

	if (hangar->status > 10)
	{
		send_to_char(FB_RED "Wskazany hangar jest uszkodzony!" NL, ch);
		return;
	}

	if (ship->shipstate != SHIP_READY)
	{
		send_to_char(FB_RED "Poczekaj az statek zakonczy aktualny manewr" NL,
				ch);
		return;
	}

	if (arg1[0] == '\0')
	{
		send_to_char(FB_RED "Z�apa� co?" NL, ch);
		return;
	}

	target = get_ship_here(arg1, ship->starsystem);
	astro = get_astro_here(arg1, ship->starsystem);

	if (astro != NULL)
	{
		tracastro(ch, ship, astro);
		return;
	}

	if (target == NULL || target->cloack == true)
	{
		send_to_char(FB_RED "Nie ma tu nic takiego" NL, ch);
		return;
	}

	if (target == ship)
	{
		send_to_char(
				FB_RED "Ciekawy pomys�, podobny do tego z po�kni�ciem samego siebie" NL,
				ch);
		return;
	}

	if (target->shipstate == SHIP_LAND)
	{
		send_to_char(FB_RED "Niestety cel rozpocz�� procedure l�dowania" NL,
				ch);
		return;
	}

	if (srange((target->vx - ship->vx), (target->vy - ship->vy),
			(target->vz - ship->vz)) > 1000 + (target->size + ship->size) / 10)
	{
		send_to_char(
				FB_RED "Cel jest zbyt daleko musisz podlecie� nieco bli�ej" NL,
				ch);
		return;
	}

	if (check_capacity(hangar->vnum) + target->size > hangar->capacity)
	{
		send_to_char(FB_RED "Cel nie zmie�ci si� w twoim hangarze" NL, ch);
		return;
	}

	if (is_platform(target))
	{
		send_to_char( FB_RED "Czego� tak wielkiego nigdy nie z�apiesz!!" NL,
				ch);
		return;
	}

	if (is_capital(target) || is_huge(target))
	{
		send_to_char(FB_RED "No nie przesadzaj !! On jest za wielki." NL, ch);
		return;
	}

	if (ship->energy < (25 + target->size))
	{
		send_to_char(FB_RED "Za ma�o energii" NL, ch);
		return;
	}

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_tractorbeams]);

	chance = chance * (ship->tractorbeam / (target->currspeed + 1));

	if (number_percent() < chance)
	{
		send_to_char( FB_GREEN, ch);
		send_to_char("Sekwencja przechwycenia zainicjowana" EOL, ch);
		act( PLAIN, "$n rozpocz�� sekwencje przechwycenia.", ch, NULL, argument,
				TO_ROOM);

		echo_to_ship(ship,
				FB_YELLOW "ALARM: cel przechwycony!! Komandosi na pok�ad hangarowy!!");
		echo_to_ship(target,
				FB_YELLOW "Statek zadr�a� w u�cisku promieni �ci�gaj�cych.");

		sprintf(buf, FG_RED "Zosta�e� z�apany przez %s.",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
		echo_to_cockpit(target, buf);

		if (autofly(target) && !target->target0)
			target->target0 = ship;

		sprintf(destbuf, "'%s' %d", ship->name, atoi(argument));
		STRDUP(target->dest, destbuf);
		target->shipstate = SHIP_LAND;
		target->currspeed = 0;
		target->shipstate1 = 10 * ship->tractorbeam;

		learn_from_success(ch, gsn_tractorbeams);
		return;

	}

	send_to_char("Nie uda�o Ci si� obs�u�y� urz�dze�." EOL, ch);
	echo_to_ship(target, FB_YELLOW "Statek zadr�a� przez moment.");

	sprintf(buf, FG_RED "%s pr�bowa� z�apa� Tw�j statek!",
			know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
	echo_to_cockpit(target, buf);

	if (autofly(target) && !target->target0)
		target->target0 = ship;

	learn_from_failure(ch, gsn_tractorbeams);
	return;
}

void tracastro(CHAR_DATA *ch, SHIP_DATA *ship, ASTRO_DATA *astro)
{
	int chance;

	if (astro->type == 1)
	{
		send_to_char(FB_RED "Asteroida nie zmiesci sie w hangarze" NL, ch);
		return;
	}

	if (astro->type == 2)
	{
		send_to_char(FB_RED "LADUNEK" NL, ch);
		return;
	}

	if (astro->type == 0)
	{
		send_to_char(FB_RED "WRAK" NL, ch);
		return;
	}

	if (astro->type > 4)
	{
		send_to_char(FB_RED "UPS!!! b��d budowniczego MUDa" NL, ch);
		return;
	}

	if (astro->type == 3 && ship->trawler <= 0)
	{
		send_to_char(FB_RED "Statek nie posiada tra�u." NL, ch);
		return;
	}

	if (srange((astro->ox - ship->vx), (astro->oy - ship->vy),
			(astro->oz - ship->vz)) > 1000)
	{
		send_to_char(
				FB_RED "Obiekt jest zbyt daleko musisz podlecie� bli�ej" NL,
				ch);
		return;
	}

	if (ship->energy < (25 + 25 * astro->value))
	{
		send_to_char(FB_RED "Za ma�o energii" NL, ch);
		return;
	}

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_tractorbeams]);

	chance = chance * (ship->tractorbeam / (astro->speed + 1));

	if (number_percent() < chance)
	{
		if (astro->type == 3)
		{
			send_to_char( FB_GREEN, ch);
			send_to_char("Sekwencja przechwycenia miny zainicjowana" NL, ch);

			act( PLAIN, "$n rozpocz�� sekwencje rozbrajania miny.", ch, NULL,
					NULL, TO_ROOM);
			echo_to_ship(ship,
					FB_YELLOW "ALARM: mina rozbrojona!! Saperzy do hangaru!!");
		}

		if (astro->type == 4)
		{
			send_to_char(
					FB_YELLOW "Kierujesz promienie �cigaj�ce w kierunku kapsu�y." NL,
					ch);
			act( PLAIN, "$n pr�buje uratow� rozbitka.", ch, NULL, NULL,
					TO_ROOM);
			echo_to_ship(ship,
					FG_RED "Kapsu�a ratunkowa �ci�gni�ta. Personel medyczny zamelduje si� w izolatce!!");
		}

		astro_from_starsystem(astro, astro->starsystem);
		learn_from_success(ch, gsn_tractorbeams);
		return;
	}

	send_to_char("Nie uda�o Ci si� obs�u�y� urz�dze�." EOL, ch);
	learn_from_failure(ch, gsn_tractorbeams);
	return;
}

void update_astro()
{
	char buf[MSL];

	auto astro_snapshot = astro_list;
	for (auto* astro : astro_snapshot)
	{
		if (astro->starsystem)
		{
			if (astro->value <= 0)
			{
				extract_astro(astro);
				bug("extract: %s", astro->name);
			}
			if (astro->type == 3)
			{
				if (astro->timer > -1)
				{
					if (--astro->timer == 0)
					{
						sprintf(buf, FB_RED MOD_BLINK "%s eksploduje." RESET,
								astro->name);
						echo_to_system_astro(astro, buf, NULL);
						extract_astro(astro);
						return;
					}
				}
				for (auto* target : ship_list)
				{
					if (astro->starsystem == target->starsystem)
						if (srange((astro->ox - target->vx),
								(astro->oy - target->vy),
								(astro->oz - target->vz)) < 1000)
							astro_atack(target, astro);

					if (astro->target0 == target
							&& (astro->starsystem != target->starsystem
									|| srange((astro->ox - target->vx),
											(astro->oy - target->vy),
											(astro->oz - target->vz)) > 1500))
					{
						astro->target0 = NULL;
						sprintf(buf, FB_YELLOW "%s deaktywuje si�.",
								astro->name);
						echo_to_system_astro(astro, buf, NULL);
					}
				}
			}
			if (astro->type == 4)
			{
				if (astro->timer < 0)
					astro->timer = 0;
				if (++astro->timer >= 15)
				{
					astro->timer = 0;
					for (auto* target : ship_list)
					{
						if (!target->starsystem)
							continue;
						if (target->starsystem != astro->starsystem)
						{
							sprintf(buf, FG_RED "SOS!!!! odbierasz sygnal z %s",
									astro->starsystem->name);
							echo_to_cockpit(target, buf);
						}
						if (target->starsystem == astro->starsystem)
						{
							if (srange((target->vx - astro->ox),
									(target->vy - astro->oy),
									(target->vz - astro->oz))
									<= (target->sensor + astro->value) * 150)
							{
								sprintf(buf,
										FG_RED "SOS!!!! odbierasz sygna� z %.0f %.0f %.0f",
										astro->ox - target->vx,
										astro->oy - target->vy,
										astro->oz - target->vz);
								echo_to_cockpit(target, buf);
							}
							else
							{
								sprintf(buf,
										FB_YELLOW "Odbierasz sygna� SOS z %.0f %.0f %.0f",
										astro->ox - target->vx
												+ number_range(-100, 100),
										astro->oy - target->vy
												+ number_range(-100, 100),
										astro->oz - target->vz
												+ number_range(-100, 100));
								echo_to_cockpit(target, buf);
							}
						}
					}
				}
			}
		}
	}
}

void astro_atack(SHIP_DATA *ship, ASTRO_DATA *astro)
{
	char buf[MSL];
	int chance, damage;

	if (astro->target0 == NULL && astro->energy > 0)
	{
		astro->target0 = ship;
		sprintf(buf, FG_RED "UWAGA !!! %s uzbraja si�." NL, astro->name);
		echo_to_system_astro(astro, buf, NULL);
		return;
	}
	if (astro->target0 == ship
			&& srange((astro->ox - ship->vx), (astro->oy - ship->vy),
					(astro->oz - ship->vz)) > 1500)
	{
		astro->target0 = NULL;
		sprintf(buf, FB_YELLOW "%s deaktywuje si�." NL, astro->name);
		echo_to_system_astro(astro, buf, NULL);
		return;
	}

	if (astro->lasers > 0 && astro->missiles > 0)
	{
		bug("ASTRO: %s has missiles and lasers at the same time.", astro->name);
		astro->lasers = 0;
		astro->missiles = 0;
		return;
	}

	if (astro->target0 == ship && astro->lasers > 0)
	{
		if (astro->energy < 10)
		{
			sprintf(buf, FB_YELLOW "%s rozpoczyna sekwencje autodestrukcji." NL,
					astro->name);
			echo_to_system_astro(astro, buf, NULL);
			astro->energy = 0;
			astro->timer = 2;
			astro->target0 = NULL;
			return;
		}
		if (astro->energy >= 10)
		{
			astro->energy -= 5 * astro->lasers;
			chance = 90;
			chance += ship->clazz * 25;
			chance -= ship->manuever / 10;
			chance -= ship->currspeed / 20;
			chance -= (abs(astro->ox - ship->vx) / 70);
			chance -= (abs(astro->oy - ship->vy) / 70);
			chance -= (abs(astro->oz - ship->vz) / 70);
			chance = URANGE(10, chance, 90);
			if (number_percent() > chance)
			{
				sprintf(buf, FG_YELLOW "%s strzela do ciebie, ale chybia.",
						astro->name);
				echo_to_cockpit(ship, buf);
				sprintf(buf, FG_YELLOW "%s chybia laserem %s.", astro->name,
						ship->sslook);
				echo_to_system_astro(astro, buf, ship);
				return;
			}
			sprintf(buf, FG_YELLOW "%s trafia laserem %s.", astro->name,
					ship->sslook);
			echo_to_system_astro(astro, buf, ship);
			sprintf(buf, FG_RED "%s trafia ci�!", astro->name);
			echo_to_cockpit(ship, buf);
			echo_to_ship(ship,
					FB_RED "Ma�a eksplozja lekko wstrz�sa statkiem.");
			damage = number_range(5 * astro->lasers, 10 * astro->lasers);
			if (ship->trawler != 0)
			{
				damage = number_range(1, 5);
				echo_to_cockpit(ship,
						FB_GREEN "Tra� poch�ania wi�ksz� cz�� energi strza�u.");
			}
			damage_ship(ship, damage, damage);
			return;
		}
	}
	if (astro->target0 == ship && astro->missiles > 0)
	{
		sprintf(buf, FG_YELLOW "%s odpala pocisk.", astro->name);
		echo_to_system_astro(astro, buf, ship);
		sprintf(buf, FG_RED "%s odpala pocisk w twoim kierunku", astro->name);
		echo_to_cockpit(ship, buf);
		new_missile_astro(astro, ship, CONCUSSION_MISSILE);
		astro_from_starsystem(astro, astro->starsystem);
		return;
	}
	return;
}

DEF_DO_FUN( unload_cargo )
{
	SHIP_DATA *ship;
	SHIP_DATA *target;
	int cost, hold;
	PLANET_DATA *planet;

	if (argument[0] == '\0')
	{
		act( PLAIN, "Z kt�rego statku chcesz wy�adowywa�?.", ch, NULL, NULL,
				TO_CHAR);
		return;
	}

	target = ship_in_room(ch->in_room, argument);

	if (!target)
	{
		act( PLAIN, "Nie ma tu �adnego $T.", ch, NULL, argument, TO_CHAR);
		return;
	}
	if (!check_pilot(ch, target))
	{
		send_to_char("Hey!! To nie tw�j statek!\r\n", ch);
		return;
	}

	if (target->cargo == 0)
	{
		send_to_char("Nie ma�adnego �adunku w �adowni.\r\n", ch);
		return;
	}

	if (!IS_SET(ch->in_room->room_flags,
			ROOM_CAN_LAND)
			/* bylo room_import && !IS_SET( ch->in_room->room_flags, ROOM_SPACECRAFT )*/)
	{
		send_to_char("Tutaj tego nie mozna zrobi�!", ch);
		return;
	}
	planet = ch->in_room->area->planet;

	if (!str_cmp(planet->name, target->cargofrom))
	{
		send_to_char(FB_RED "No co� ty przecie� tutaj to kupi�e�!" EOL, ch);
		return;
	}
	if (!planet)
	{
		ship = ship_from_hangar(ch->in_room);

		if (!ship)
		{
			send_to_char("You can't do that here!", ch);
			return;
		}
		if ((ship->maxcargo - ship->cargo) < 1)
		{
			send_to_char("There is no room for anymore cargo\r\n", ch);
			return;
		}
		if (ship->cargo == 0)
			ship->cargotype = CARGO_NONE;

		if ((ship->cargo > 0) && (ship->cargotype != target->cargo))
		{
			send_to_char("They have a differnt type of cargo." EOL, ch);
			return;
		}
		if (ship->cargotype == CARGO_NONE)
			ship->cargotype = target->cargotype;
		if ((ship->maxcargo - ship->cargo) >= target->cargo)
		{
			ship->cargo += target->cargo;
			target->cargo = 0;
			target->cargo = CARGO_NONE;
			send_to_char("Cargo unloaded.\r\n", ch);
			return;
		}
		else
		{
			target->cargo -= ship->maxcargo - ship->cargo;
			ship->cargo = ship->maxcargo;
			ch_printf(ch, "%s Loaded, %d tons still in %s hold.\r\n",
					SHIPNAME(ship), target->cargo, SHIPNAME(target));
			return;
		}
	}
	if ((target->cargotype != CARGO_NONE)
			&& (target->cargotype != planet->import)
			&& (planet->import != CARGO_ALL))
	{
		send_to_char("Tutaj nie potrzebuj� twojego �adunku.\r\n", ch);
		return;
	}
	hold = target->cargo;
	cost = hold * planet->prisei;

	ch->gold += cost;
	STRDUP(target->cargofrom, "");
	target->cargo = 0;
	target->cargotype = CARGO_NONE;
	ch_printf(ch, "Otrzymujesz %d kredyt�w za %s.\r\n", cost,
			cargo_names[planet->import]);
	return;
}

DEF_DO_FUN( load_cargo )
{
	SHIP_DATA *ship;
	SHIP_DATA *target;
	int cost, hold;
	PLANET_DATA *planet;

	if (argument[0] == '\0')
	{
		act( PLAIN, "Na kt�ry statek chcesz za�adowa� towar?.", ch, NULL, NULL,
				TO_CHAR);
		return;
	}

	target = ship_in_room(ch->in_room, argument);

	if (!target)
	{
		act( PLAIN, "Nie widze tutaj $T .", ch, NULL, argument, TO_CHAR);
		return;
	}

	if (!check_pilot(ch, target))
	{
		send_to_char("Hey!! To nie Tw�j statek!\r\n", ch);
		return;
	}

	if (!IS_SET(ch->in_room->room_flags, ROOM_CAN_LAND)
	/* bylo rom_import && !IS_SET( ch->in_room->room_flags, ROOM_SPACECRAFT )*/)
	{
		send_to_char("Tutaj nie mozesz tego zrobi�!", ch);
		return;
	}
	planet = ch->in_room->area->planet;

	if (!planet)
	{
		ship = ship_from_hangar(ch->in_room);
		if (!ship)
		{
			send_to_char("You can't do that here!", ch);
			return;
		}
		if (ship->cargo == 0)
		{
			send_to_char("They don't have any cargo" EOL, ch);
			return;
		}
		if ((target->maxcargo - target->cargo) < 1)
		{
			send_to_char("There is no room for anymore cargo\r\n", ch);
			return;
		}
		if ((target->cargotype = ! CARGO_NONE)
				&& (ship->cargotype != target->cargotype))
			;
		{
			send_to_char("Maybe you should deliver your cargo first." EOL, ch);
			return;
		}
		if (target->cargotype == CARGO_NONE)
			target->cargotype = ship->cargotype;

		if ((target->maxcargo - target->cargo) >= ship->cargo)
		{
			target->cargo += ship->cargo;
			ship->cargo = 0;
			send_to_char("Cargo loaded.\r\n", ch);
			return;
		}
		else
		{
			ship->cargo -= target->maxcargo - target->cargo;
			target->cargo = target->maxcargo;
			send_to_char("Cargo Loaded.\r\n", ch);
			return;
		}

	}
	if (target->maxcargo - target->cargo <= 0)
	{
		send_to_char("W �adowni nie zmie�ci si� wiecej towaru." NL, ch);
		return;
	}
	if ((target->cargotype != CARGO_NONE) && (target->cargo != planet->_export))
	{
		send_to_char("Wiesz co? Dostarcz najpierw obecny �adunek." NL, ch);
		return;
	}
	if (planet->_export == CARGO_NONE)
	{
		send_to_char("Nie mamy nic do wyeksortowania." NL, ch);
		return;
	}

	if (planet->stock <= 0)
	{
		send_to_char("Magazyny s� puste, poczekaj na dostaw� poprodukcyjn�." NL,
				ch);
		return;
	}
	hold = (target->maxcargo - target->cargo);
	if (planet->stock < hold)
	{
		hold = planet->stock;
		send_to_char("Zapasy planety s� mniejsze ni� pojemno�� �adowni." NL,
				ch);
	}
	cost = hold * planet->prisee;
	if (ch->gold < cost)
	{
		send_to_char("Nie sta� Ci�\r\n", ch);
		return;
	}
	ch->gold -= cost;
	planet->stock -= hold;
	planet->prisee += UMIN(1, hold / 100);
	save_planet(planet);
	target->cargo += hold;
	target->cargotype = planet->_export;
	target->cargofrom = planet->name;
	ch_printf(ch, "P�acisz %d kredyt�w za �adunek %s.\r\n", cost,
			cargo_names[planet->_export]);
	return;
}

void ship_to_repository(SHIP_DATA *ship)
{
	ship->shipstate = SHIP_REPOSITORY;
	extract_ship(ship);

	ship->location = ship->shipyard;
	ship->lastdoc = ship->shipyard;
	generate_transponder(ship);

	if (ship->starsystem)
		ship_from_starsystem(ship, ship->starsystem);

	ship->currspeed = 0;
	ship->hull = ship->maxhull;
	ship->shield = 0;
	ship->cloack = 0;
	ship->interdict = 0;

	ship->currjump = NULL;
	ship->target0 = NULL;
	for (auto* turret : ship->turrets)
		turret->target = NULL;

	ship->hatchopen = false;

	ship->autorecharge = false;
	ship->autotrack = false;
	ship->autospeed = false;
	STRDUP(ship->lock_key, "0000");
	if (ship->maxcargo > 0)
	{
		ship->cargo = 0;
		ship->cargotype = 0;
		STRDUP(ship->cargofrom, "");
	}
	if (str_cmp("Public", ship->owner) && ship->type != MOB_SHIP)
	{
		CLAN_DATA *clan;

		if (ship->type != MOB_SHIP && (clan = get_clan(ship->owner)) != NULL)
		{
			if (is_ship(ship))
				clan->spacecrafts--;
			else
				clan->vehicles--;
		}
		STRDUP(ship->owner, "");
		STRDUP(ship->pilot, "");
		STRDUP(ship->copilot, "");
	}

	if (str_cmp(ship->ship_title, ship->name))
		STRDUP(ship->ship_title, ship->name);

	clear_transponders(ship);
	save_ship(ship);
}

/****************************************************************************/

DEF_DO_FUN( astro )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	ASTRO_DATA *astro, *tastro;
	int tempnum, licznik;
	ROOM_INDEX_DATA *roomindex;
	SPACE_DATA *stars;

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);
	argument = one_argument(argument, arg4);

	if (arg1[0] == '\0')
	{
		send_to_char(
				"Sk�adnia: astro <komenda> <astro name> <pole> <warto�ci>" NL,
				ch);
		send_to_char( NL "Gdzie komend� mo�e by�:" NL, ch);
		send_to_char("make set show showall move add rem del" NL, ch);
		send_to_char( NL "Pole zale�y od komendy" NL, ch);
		return;
	}

	if (!str_cmp(arg1, "showall"))
	{
		licznik = 0;
		pager_printf(ch,
				FB_WHITE "Nazwa                         Plik            Home         Obecny system" NL);
		for (auto* astro : astro_list)
		{
			licznik++;
			pager_printf(ch, FG_CYAN "%-27s   %-13s   %-10s   ", astro->name,
					astro->filename,
					!str_cmp(astro->home, "") ? "" : astro->home);
			if (astro->starsystem)
				pager_printf(ch, FG_CYAN "%s", astro->starsystem->name);
			pager_printf(ch, NL);
		}
		pager_printf(ch, FB_WHITE "��cznie: %d." NL, licznik);
		for (auto* member : cmember_list)
			pager_printf(ch, "%-20s %s" NL, member->name->name,
					SHIPNAME(member->ship));

		return;
	}

	if (!str_cmp(arg1, "show"))
	{
		if (arg2[0] == '\0')
		{
			send_to_char(
					"Sk�adnia: astro <komenda> <astro name> <pole> <warto�ci>" EOL,
					ch);
			return;
		}

		astro = get_astro(arg2);
		if (!astro)
		{
			send_to_char("No such astro." EOL, ch);
			return;
		}
		pager_printf(ch, FB_YELLOW "Name: %s" NL "Filename: %s" NL "Typ: %s" NL,
				astro->name, astro->filename, astro_names[astro->type]);
		pager_printf(ch, "Home: %s " NL "lasers: %d missiles: %d",
				!str_cmp(astro->home, "") ? "Brak" : astro->home, astro->lasers,
				astro->missiles);
		pager_printf(ch, NL);
		pager_printf(ch, "speed: %d value: %d koord: %.0f %.0f %.0f" NL,
				astro->speed, astro->value, astro->ox, astro->oy, astro->oz);
		pager_printf(ch, "Vnum: %d energy: %d kierunek: %.0f %.0f %.0f" NL,
				astro->vnum, astro->energy, astro->cx, astro->cy, astro->cz);
		pager_printf(ch, "Timer: %d " NL, astro->timer);
		pager_printf(ch, EOL);
		return;
	} // end of show

	if (!str_cmp(arg1, "make"))
	{
		if (arg2[0] == '\0' || arg3[0] == '\0')
		{
			send_to_char(
					"Sk�adnia: astro make <astro filename> <'astro name'>" NL,
					ch);
			return;
		}
		for (auto* tastro : astro_list)
		{
			if (!str_cmp(tastro->filename, arg2))
			{
				send_to_char( FB_RED "EEe jest juz Obiekt z takim filename." NL,
						ch);
				return;
			}
		}
		CREATE(astro, ASTRO_DATA, 1);
		STRDUP(astro->filename, arg2);
		STRDUP(astro->name, arg3);
		STRDUP(astro->home, "");
		astro->timer = -1;
		astro->value = 1;
		astro->energy = 1;
		astro->ox = 4000 + number_range(-1000, 1000);
		astro->oy = 4000 + number_range(-1000, 1000);
		astro->oz = 4000 + number_range(-1000, 1000);
		astro_list.push_back(astro);
		save_astro(astro);
		write_astro_list();
		send_to_char("Done." NL, ch);
		return;
	} //end of make

	if (!str_cmp(arg1, "set"))
	{
		if (arg2[0] == '\0' || arg3[0] == '\0')
		{
			send_to_char("Sk�adnia: astro set <astro name> <pole> <warto��>" NL,
					ch);
			send_to_char(
					"Gdzie pole to: type value vnum home energy ox oy oz lasers missiles" NL,
					ch);
			return;
		}
		astro = get_astro(arg2);
		if (!astro)
		{
			send_to_char( FB_RED "No such object." NL, ch);
			return;
		}
		if (!str_cmp(arg3, "type"))
		{
			astro->type = URANGE(0, atoi(arg4), 4);
			send_to_char("Done." NL, ch);
			save_astro(astro);
			return;
		}
		if (!str_cmp(arg3, "name"))
		{
			STRDUP(astro->name, arg4);
			send_to_char("Done." NL, ch);
			save_astro(astro);
			return;
		}
		if (!str_cmp(arg3, "home"))
		{
			stars = starsystem_from_name(arg4);
			if (stars == NULL)
			{
				send_to_char( FB_RED "Nie ma takiego Systemu." NL, ch);
				return;
			}
			STRDUP(astro->home, arg4);
			send_to_char("Done." NL, ch);
			save_astro(astro);
			return;
		}
		if (!str_cmp(arg3, "ox"))
		{
			astro->ox = atoi(arg4);
			save_astro(astro);
			send_to_char("Done." NL, ch);
			return;
		}
		if (!str_cmp(arg3, "oy"))
		{
			astro->oy = atoi(arg4);
			save_astro(astro);
			send_to_char("Done." NL, ch);
			return;
		}
		if (!str_cmp(arg3, "oz"))
		{
			astro->oz = atoi(arg4);
			save_astro(astro);
			send_to_char("Done." NL, ch);
			return;
		}
		if (!str_cmp(arg3, "lasers"))
		{
			astro->lasers = URANGE(0, atoi(arg4), 10);
			astro->missiles = 0;
			send_to_char("Done." NL, ch);
			save_astro(astro);
			return;
		}
		if (!str_cmp(arg3, "missiles"))
		{
			astro->missiles = URANGE(0, atoi(arg4), 1);
			astro->lasers = 0;
			send_to_char("Done." NL, ch);
			save_astro(astro);
			return;
		}
		if (!str_cmp(arg3, "energy"))
		{
			astro->energy = URANGE(0, atoi(arg4), 1000);
			send_to_char("Done." NL, ch);
			save_astro(astro);
			return;
		}
		if (!str_cmp(arg3, "value"))
		{
			astro->value = URANGE(0, atoi(arg4), 30000);
			send_to_char("Done." NL, ch);
			save_astro(astro);
			return;
		}
		if (!str_cmp(arg3, "vnum"))
		{
			tempnum = atoi(arg4);
			roomindex = get_room_index(tempnum);
			if (roomindex == NULL)
			{
				send_to_char("That room doesn't exist." EOL, ch);
				return;
			}
			astro->vnum = tempnum;
			send_to_char("Done." NL, ch);
			save_astro(astro);
			return;
		}
		return;
	} // end of set

	if (!str_cmp(arg1, "add"))
	{
		if (arg2[0] == '\0' || arg3[0] == '\0')
		{
			send_to_char("Sk�adnia: astro add <astro name> <starsystem>" NL,
					ch);
			return;
		}
		astro = get_astro(arg2);
		if (!astro)
		{
			send_to_char( FB_RED "Nie ma takiego Obiektu." NL, ch);
			return;
		}
		stars = starsystem_from_name(arg3);
		if (stars == NULL)
		{
			send_to_char( FB_RED "Nie ma takiego Systemu." NL, ch);
			return;
		}
		for (auto* starsystem : starsystem_list)
		{
			tastro = get_astro_here(arg2, starsystem);
			if (tastro != NULL)
			{
				ch_printf(ch, FB_RED "Obiekt jest juz dodany do systemu: %s" NL,
						starsystem->name);
				return;
			}
		}
		astro_to_starsystem(astro, stars);
		send_to_char("Done." NL, ch);
		return;
	} //end of add
	if (!str_cmp(arg1, "del"))
	{
		if (arg2[0] == '\0')
		{
			send_to_char("Sk�adnia: astro del <astro name> yes" NL, ch);
			send_to_char("UWAGA: komenda DEL usuwa plik obiektu!" NL, ch);
			return;
		}
		if (!str_cmp(arg3, "yes") || !str_cmp(arg3, "tak"))
		{
			astro = get_astro(arg2);
			if (!astro)
			{
				send_to_char( FB_RED "Nie ma takiego Obiektu." NL, ch);
				return;
			}
			extract_astro(astro);
			write_astro_list();
			send_to_char("Usuniete." NL, ch);
			return;
		}
		send_to_char(
				FB_RED "Jak chcesz to usun�� wpisz koniecznie 'yes' na koncu." NL,
				ch);
		return;
	} // end of del
	if (!str_cmp(arg1, "rem"))
	{
		if (arg2[0] == '\0')
		{
			send_to_char("Sk�adnia: astro rem <astro name>" NL, ch);
			return;
		}
		astro = get_astro(arg2);
		if (!astro)
		{
			send_to_char( FB_RED "Nie ma takiego Obiektu." NL, ch);
			return;
		}
		if (!astro->starsystem)
		{
			send_to_char(
					FB_RED "Obiekt obecnie nie jest w �adnym systemie." NL, ch);
			return;
		}
		astro_from_starsystem(astro, astro->starsystem);
		send_to_char("Usuni�te z obecnego systemu." NL, ch);
		return;
	} // end of rem

	do_astro(ch, (char*) "");
	return;
}

/****************************************************************************/

void save_astro(ASTRO_DATA *astro)
{
	FILE *fp;
	char filename[256];

	IF_BUG(astro == NULL, "")
		return;

	if (!astro->filename || astro->filename[0] == '\0')
	{
		bug("%s has no filename", astro->name);
		return;
	}

	sprintf(filename, "%s%s", ASTRO_DIR, astro->filename);

	RESERVE_CLOSE;
	if ((fp = fopen(filename, "w")) == NULL)
	{
		bug("fopen", 0);
		perror(filename);
	}
	else
	{
		fprintf(fp, "#ASTRO\n");
		fprintf(fp, "Name         	%s~\n", astro->name);
		fprintf(fp, "Filename     	%s~\n", astro->filename);
		fprintf(fp, "Type  		%d\n", astro->type);
		fprintf(fp, "Value	   	%d\n", astro->value);
		fprintf(fp, "Home	   	%s~\n", astro->home);
		fprintf(fp, "Lasers   		%d\n", astro->lasers);
		fprintf(fp, "Missiles        	%d\n", astro->missiles);
		fprintf(fp, "Vnum        	%d\n", astro->vnum);
		fprintf(fp, "Energy        	%d\n", astro->energy);
		fprintf(fp, "Ox        	%.0f\n", astro->ox);
		fprintf(fp, "Oy        	%.0f\n", astro->oy);
		fprintf(fp, "Oz        	%.0f\n", astro->oz);
		fprintf(fp, "End\n\n");
		fprintf(fp, "#END\n");
	}
	fclose(fp);
	RESERVE_OPEN;
	return;
}

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = true;			\
				    break;				\
				}

void fread_astro(ASTRO_DATA *astro, FILE *fp)
{
	const char *word;
	bool fMatch;

	for (;;)
	{
		word = feof(fp) ? "End" : fread_word(fp);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;

		case 'E':
			KEY("Energy", astro->energy, fread_number(fp))
			;
			if (!str_cmp(word, "End"))
			{
				if (!astro->home)
					STRDUP(astro->home, "");
				if (!astro->name)
					STRDUP(astro->name, "");
				if (astro->vnum < 0)
					astro->vnum = 0;
				astro->starsystem = NULL;

				return;
			}
			break;

		case 'F':
			SKEY("Filename", astro->filename, fread_string(fp))
			;
			break;

		case 'H':
			SKEY("Home", astro->home, fread_string(fp))
			;
			break;

		case 'L':
			KEY("Lasers", astro->lasers, fread_number(fp))
			;
			break;

		case 'M':
			KEY("Missiles", astro->missiles, fread_number(fp))
			;
			break;

		case 'N':
			SKEY("Name", astro->name, fread_string(fp))
			;
			break;

		case 'O':
			KEY("Ox", astro->ox, fread_number(fp))
			;
			KEY("Oy", astro->oy, fread_number(fp))
			;
			KEY("Oz", astro->oz, fread_number(fp))
			;

		case 'T':
			KEY("Type", astro->type, fread_number(fp))
			;
			break;

		case 'V':
			KEY("Value", astro->value, fread_number(fp))
			;
			KEY("Vnum", astro->vnum, fread_number(fp))
			;
			break;

		}

		if (!fMatch)
		{
			bug("no match: %s", word);
		}
	}
}

bool load_astro_file(const char *astrofile)
{
	char filename[256];
	ASTRO_DATA *astro;
	FILE *fp;
	bool found;

	CREATE(astro, ASTRO_DATA, 1);
	astro->timer = -1;

	found = false;
	sprintf(filename, "%s%s", ASTRO_DIR, astrofile);

	if ((fp = fopen(filename, "r")) != NULL)
	{

		found = true;
		for (;;)
		{
			char letter;
			char *word;

			letter = fread_letter(fp);
			if (letter == '*')
			{
				fread_to_eol(fp);
				continue;
			}

			if (letter != '#')
			{
				bug(" # not found.", 0);
				break;
			}

			word = fread_word(fp);
			if (!str_cmp(word, "ASTRO"))
			{
				fread_astro(astro, fp);
				break;
			}
			else if (!str_cmp(word, "END"))
				break;
			else
			{
				bug("bad section: %s.", word);
				break;
			}
		}
		fclose(fp);
	}
	if (!(found))
		DISPOSE(astro);
	else
	{
		astro_list.push_back(astro);

		if (str_cmp(astro->home, ""))
			astro_to_starsystem(astro, starsystem_from_name(astro->home));
		astro->ox += number_range(-10, 10);
		astro->oy += number_range(-10, 10);
		astro->oz += number_range(-10, 10);
	}

	return found;
}

void write_astro_list()
{
	FILE *fpout;

	fpout = fopen( ASTRO_LIST, "w");
	if (!fpout)
	{
		bug("FATAL: cannot open astro.lst for writing!" EOL, 0);
		return;
	}
	for (auto* tastro : astro_list)
	{
		fprintf(fpout, "%s\n", tastro->filename);
	}
	fprintf(fpout, "$\n");
	fclose(fpout);
}

void load_astros()
{
	FILE *fpList;
	const char *filename;

	astro_list.clear();

	RESERVE_CLOSE;

	if ((fpList = fopen( ASTRO_LIST, "r")) == NULL)
	{
		perror( ASTRO_LIST);
		exit(1);
	}

	for (;;)
	{

		filename = feof(fpList) ? "$" : fread_word(fpList);

		if (filename[0] == '$')
			break;

		if (!load_astro_file(filename))
		{
			bug("Cannot load astro file: %s", filename);
		}
	}
	fclose(fpList);

	if (!sysdata.silent)
		log_string(" Done astros ");
	RESERVE_OPEN;
	return;
}

void astro_to_starsystem(ASTRO_DATA *astro, SPACE_DATA *starsystem)
{
	if (starsystem == NULL)
		return;

	if (astro == NULL)
		return;

	starsystem->astros.push_back(astro);

	astro->starsystem = starsystem;

}

void astro_from_starsystem(ASTRO_DATA *astro, SPACE_DATA *starsystem)
{
	if (astro == NULL)
		return;

	if ((starsystem = astro->starsystem) != NULL)
	{
		starsystem->astros.remove(astro);
		astro->starsystem = NULL;
	}
}

bool has_map(SPACE_DATA *starsystem, CHAR_DATA *ch)
{
	for (auto* obj : ch->carrying)
	{
		if (obj->item_type == ITEM_SHIPDEVICE && obj->value[0] == 1
				&& obj->value[1] == starsystem->hidden)
		{
			return true;
		}
	}
	return false;
}

void new_astro(SHIP_DATA *ship, int astrotype)
{
	SPACE_DATA *starsystem;
	ASTRO_DATA *astro;

	if (ship == NULL)
		return;

	if ((starsystem = ship->starsystem) == NULL)
		return;

	CREATE(astro, ASTRO_DATA, 1);

	STRDUP(astro->name, "Wrak");
	STRDUP(astro->filename, "");
	STRDUP(astro->home, "");

	astro->type = astrotype;
	astro->value = ship->maxhull / 2;
	astro->speed = 0;

	astro->ox = ship->vx;
	astro->oy = ship->vy;
	astro->oz = ship->vz;
	astro_list.push_back(astro);

	starsystem->astros.push_back(astro);

	astro->starsystem = starsystem;

}

/*
 * Checks if astro in a starsystem and returns poiner if it is.
 */
ASTRO_DATA* get_astro_here(char *name, SPACE_DATA *starsystem)
{
	if (starsystem == NULL)
		return NULL;

	for (auto* astro : starsystem->astros)
	{
		if (!str_cmp(name, astro->name))
			return astro;
	}
	for (auto* astro : starsystem->astros)
	{
		if (nifty_is_name_prefix(name, astro->name))
			return astro;
	}
	return NULL;
}

void extract_astro(ASTRO_DATA *astro)
{
	SPACE_DATA *starsystem;
	char filename[256];

	if (astro == NULL)
		return;

	if ((starsystem = astro->starsystem) != NULL)
	{
		starsystem->astros.remove(astro);
		astro->starsystem = NULL;
	}
	if (astro->filename)
	{
		sprintf(filename, "%s%s", ASTRO_DIR, astro->filename);
		unlink(filename);
	}
	astro_list.remove(astro);
	free_astro(astro);
}

void echo_to_system_astro(ASTRO_DATA *astro, char *argument, SHIP_DATA *ignore)
{
	if (!astro->starsystem)
		return;

	for (auto* target : astro->starsystem->ships)
	{
		if (target != ignore)
			echo_to_cockpit(target, argument);
	}
}

void new_missile_astro(ASTRO_DATA *astro, SHIP_DATA *target, int missiletype)
{
	SPACE_DATA *starsystem;
	MISSILE_DATA *missile;

	if (astro == NULL)
		return;

	if (target == NULL)
		return;

	if ((starsystem = astro->starsystem) == NULL)
		return;

	CREATE(missile, MISSILE_DATA, 1);
	missile_list.push_back(missile);

	missile->fired_from = NULL;
	missile->target = target;
	missile->missiletype = missiletype;
	missile->age = 0;
	missile->speed = 300;
	missile->mx = astro->ox;
	missile->my = astro->oy;
	missile->mz = astro->oz;
	STRDUP(missile->fired_by, "");

	starsystem->missiles.push_back(missile);

	missile->starsystem = starsystem;

}

bool is_scout(SHIP_DATA *ship)
{
	if (ship->clazz == ESCAPE_POD_SHIP)
		return true;
	if (ship->clazz == SCOUT_SHIP)
		return true;
	return false;
}

// Pixel: co to ma byc?!?!
bool is_fighter(SHIP_DATA *ship)
{
	if (ship->clazz == TIE_FIGHTER_TIE_LN_SHIP)
		return true;
	if (ship->clazz == TIE_VANGUARD_TIE_RC_SHIP)
		return true;
	if (ship->clazz == TIE_FIRE_CONTROL_TIE_FC_SHIP)
		return true;
	if (ship->clazz == FIGHTER_SHIP)
		return true;
	if (ship->clazz == TIE_FIGHTER_TIE_LN_SHIP)
		return true;
	if (ship->clazz == TIE_VANGUARD_TIE_RC_SHIP)
		return true;
	if (ship->clazz == TIE_FIRE_CONTROL_TIE_FC_SHIP)
		return true;
	if (ship->clazz == TIE_GROUND_ATTACK_TIE_GT_SHIP)
		return true;
	if (ship->clazz == TIE_BOMBER_TIE_SA_SHIP)
		return true;
	if (ship->clazz == TIE_SHUTTLE_TIE_SH_SHIP)
		return true;
	if (ship->clazz == TIE_BOARDING_CRAFT_TIE_BC_SHIP)
		return true;
	if (ship->clazz == TIE_INTERCEPTOR_SHIP)
		return true;
	if (ship->clazz == ASSAULT_GUNBOAT_SHIP)
		return true;
	if (ship->clazz == TIE_X1_ADVANCED_TIE_SHIP)
		return true;
	if (ship->clazz == TIE_AVENGER_TIE_AD_SHIP)
		return true;
	if (ship->clazz == TIE_ADVANCED_AVENGER_SHIP)
		return true;
	if (ship->clazz == XWING_SHIP)
		return true;
	if (ship->clazz == XWING_RECON_SHIP)
		return true;
	if (ship->clazz == YWING_SHIP)
		return true;
	if (ship->clazz == YWING_LONGPROBE_SHIP)
		return true;
	if (ship->clazz == AWING_SHIP)
		return true;
	if (ship->clazz == BWING_SHIP)
		return true;
	if (ship->clazz == TIE_RAPTOR_SHIP)
		return true;
	if (ship->clazz == TIE_DEFENDER_SHIP)
		return true;
	if (ship->clazz == SCIMITAR_ASSAULT_BOMBER_SHIP)
		return true;
	if (ship->clazz == A9_VIGILANCE_SHIP)
		return true;
	if (ship->clazz == I7_HOWLRUNNER_SHIP)
		return true;
	if (ship->clazz == PREYBIRD_SHIP)
		return true;
	if (ship->clazz == TIE_DROID_TIE_D_TIE_DR_SHIP)
		return true;
	if (ship->clazz == MISSILE_BOAT_SHIP)
		return true;
	if (ship->clazz == TIE_PHANTOM_SHIP)
		return true;
	if (ship->clazz == TWING_SHIP)
		return true;
	if (ship->clazz == EWING_SHIP)
		return true;
	if (ship->clazz == BWING_E_SHIP)
		return true;
	if (ship->clazz == XWING_II_SHIP)
		return true;
	if (ship->clazz == KWING_SHIP)
		return true;
	if (ship->clazz == CLOAKSHAPE_FIGHTER_SHIP)
		return true;
	if (ship->clazz == TOCSAN_8Q_FIGHTER_SHIP)
		return true;
	if (ship->clazz == ZEBRA_STARFIGHTER_SHIP)
		return true;
	if (ship->clazz == AUTHORITY_IRD_SHIP)
		return true;
	if (ship->clazz == C73_TRACKER_SHIP)
		return true;
	if (ship->clazz == Z95_HEADHUNTER_MK_I_SHIP)
		return true;
	if (ship->clazz == Z95_HEADHUNTER_SHIP)
		return true;
	if (ship->clazz == Z95_HEADHUNTER_XT_SHIP)
		return true;
	if (ship->clazz == Z95AF4_HEADHUNTER_SHIP)
		return true;
	if (ship->clazz == R41_STARCHASER_SHIP)
		return true;
	if (ship->clazz == TIEWING_TIEY_UGLY_SHIP)
		return true;
	if (ship->clazz == YTIE_UGLY_SHIP)
		return true;
	if (ship->clazz == XTIE_UGLY_SHIP)
		return true;
	if (ship->clazz == XORCEPTOR_XT_I_UGLY_SHIP)
		return true;
	if (ship->clazz == TRIFIGHTER_SHIP)
		return true;
	if (ship->clazz == DEATHSEED_CHIR_DAKI_SHIP)
		return true;
	if (ship->clazz == CORAL_FIGHTER_SHIP)
		return true;
	if (ship->clazz == CORAL_BOMBER_SHIP)
		return true;
	if (ship->clazz == SWARM_SHIP)
		return true;
	return false;
}

bool is_midship(SHIP_DATA *ship)
{
	if (ship->clazz == MIDSHIP_SHIP)
		return true;
	if (ship->clazz == LAMBDA_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == TYDERIAN_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == MU2_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == ESCORT_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == STORMTROOPER_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == GREK_TROOP_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == GAMMA_ASSAULT_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == KATARN_BOARDING_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == ULIG_ABAHA_2_WARPOD_SHIP)
		return true;
	if (ship->clazz == ASSAULT_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == KAPPA_TROOP_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == SENTINEL_LANDING_CRAFT_SHIP)
		return true;
	if (ship->clazz == AEGIS_COMBAT_SHUTTLE_SHIP)
		return true;
	if (ship->clazz == WARLORD_LANDING_BARGE_SHIP)
		return true;
	if (ship->clazz == Y4_RAPTOR_SHIP)
		return true;
	if (ship->clazz == VWING_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == LONE_SCOUT_A_SHIP)
		return true;
	if (ship->clazz == IMPERIAL_SCOUT_CRAFT_SHIP)
		return true;
	if (ship->clazz == GYRE_SCOUT_SHIP)
		return true;
	if (ship->clazz == FIRESPRAY_PATROL_CRAFT_SHIP)
		return true;
	if (ship->clazz == MANDALMOTORS_PURSUER_SHIP)
		return true;
	if (ship->clazz == CONQUEROR_ASSAULT_SHIP_SHIP)
		return true;
	if (ship->clazz == SKIPRAY_BLASTBOAT_SHIP)
		return true;
	if (ship->clazz == GUARDIAN_LIGHT_CRUISER_SHIP)
		return true;
	if (ship->clazz == CUSTOMS_FRIGATE_SHIP)
		return true;
	if (ship->clazz == IPV1_SYSTEM_PATROL_CRAFT_SHIP)
		return true;
	if (ship->clazz == PASSENGER_LINER_SHIP)
		return true;
	if (ship->clazz == FERRYBOAT_LINER_SHIP)
		return true;
	if (ship->clazz == KLEEQUE_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == BR23_COURIER_SHIP)
		return true;
	if (ship->clazz == LUXURY_3000_YACHT_SHIP)
		return true;
	if (ship->clazz == BAUDO_YACHT_SHIP)
		return true;
	if (ship->clazz == Z10_SEEKER_SHIP)
		return true;
	if (ship->clazz == HERMES_COURIER_SHIP)
		return true;
	return false;
}

bool is_freighter(SHIP_DATA *ship)
{
	if (ship->clazz == FREIGHTER_SHIP)
		return true;
	if (ship->clazz == MT_191_DROPSHIP_SHIP)
		return true;
	if (ship->clazz == YT1210_CORELLIAN_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == YT1300_CORELLIAN_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == YT2000_CORELLIAN_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == YT2400_CORELLIAN_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == GHTROC_FREIGHTER_SHIP)
		return true;
	if (ship->clazz == AZZ3_FREIGHTER_SHIP)
		return true;
	if (ship->clazz == MUURIAN_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == BARLOZ_FREIGHTER_SHIP)
		return true;
	if (ship->clazz == HT2200_MEDIUM_FREIGHTER_SHIP)
		return true;
	if (ship->clazz == XIYTIAR_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == BULK_BARGE_SHIP)
		return true;
	if (ship->clazz == MEDIUM_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == CARGO_FERRY_SHIP)
		return true;
	if (ship->clazz == ACTION_IV_TRANSPORT_SHIP)
		return true;
	if (ship->clazz == BULK_FREIGHTER_SHIP)
		return true;
	if (ship->clazz == YORIK_TREMA_TRANSPORT_CARRIER_SHIP)
		return true;
	if (ship->clazz == LWHEKK_CLASS_MANUFACTURING_SHIP)
		return true;
	if (ship->clazz == SHNER_PLANETARY_ASSAULT_CARRIER_SHIP)
		return true;
	if (ship->clazz == DKEE_CLASS_PWECK_LANDING_SHIP)
		return true;
	if (ship->clazz == SHNER_CLASS_PLANETARY_ASSAULT_CARRIER_SHIP)
		return true;
	return false;
}

bool is_capital(SHIP_DATA *ship)
{
	if (ship->clazz == CAPITAL_SHIP)
		return true;
	if (ship->clazz == STAR_GALLEON_SHIP)
		return true;
	if (ship->clazz == SPACE_TANKER_SHIP)
		return true;
	if (ship->clazz == CARGO_TRAIN_SHIP)
		return true;
	if (ship->clazz == CONTAINER_SHIP_SHIP)
		return true;
	if (ship->clazz == MARAUDER_CORVETTE_SHIP)
		return true;
	if (ship->clazz == CORELLIAN_CORVETTE_SHIP)
		return true;
	if (ship->clazz == MODIFIED_CORVETTE_SHIP)
		return true;
	if (ship->clazz == ASSASIN_CORVETTE_SHIP)
		return true;
	if (ship->clazz == CUSTOMS_CORVETTE_SHIP)
		return true;
	if (ship->clazz == CORELLIAN_GUNSHIP_SHIP)
		return true;
	if (ship->clazz == VIBRE_CRUISER_SHIP)
		return true;
	if (ship->clazz == NEBULON_ESCORT_FRIGATE_SHIP)
		return true;
	if (ship->clazz == NEBULON_ESCORT_FRIGATE_LIGHT_SHIP)
		return true;
	if (ship->clazz == MODIFIED_ESCORT_FRIGATE_SHIP)
		return true;
	if (ship->clazz == LANCER_FRIGATE_SHIP)
		return true;
	if (ship->clazz == CALAMARI_FRIGATE_SHIP)
		return true;
	if (ship->clazz == QUASAR_FIRE_CRUISER_SHIP)
		return true;
	if (ship->clazz == IMPERIAL_ESCORT_CARRIER_SHIP)
		return true;
	if (ship->clazz == IMPERIAL_FLEET_CARRIER_SHIP)
		return true;
	if (ship->clazz == INTERDICTOR_CRUISER_SHIP)
		return true;
	if (ship->clazz == MEDICAL_FRIGATE_SHIP)
		return true;
	if (ship->clazz == MC_EVACUATION_CRUISER_SHIP)
		return true;
	if (ship->clazz == DUNGEON_SHIP_SHIP)
		return true;
	if (ship->clazz == CARRACK_CRUISER_SHIP)
		return true;
	if (ship->clazz == STRIKE_CRUISER_SHIP)
		return true;
	if (ship->clazz == BULK_CRUISER_SHIP)
		return true;
	if (ship->clazz == LIGHT_CALAMARI_CRUISER_SHIP)
		return true;
	if (ship->clazz == TASKFORCE_CRUISER_SHIP)
		return true;
	if (ship->clazz == INVINCIBLE_DREADNAUGHT_SHIP)
		return true;
	if (ship->clazz == RENDILLI_DREADNAUGHT_SHIP)
		return true;
	if (ship->clazz == ASSAULT_FRIGATE_SHIP)
		return true;
	if (ship->clazz == MC80_CALAMARI_CRUISER_SHIP)
		return true;
	if (ship->clazz == MC80A_CALAMARI_CRUISER_SHIP)
		return true;
	if (ship->clazz == MC80B_CALAMARI_CRUISER_SHIP)
		return true;
	if (ship->clazz == MC90_CALAMARI_CRUISER_SHIP)
		return true;
	if (ship->clazz == VICTORY_STAR_DESTROYER_SHIP)
		return true;
	if (ship->clazz == VICTORY_II_STAR_DESTROYER_SHIP)
		return true;
	if (ship->clazz == IMPERIAL_STAR_DESTROYER_SHIP)
		return true;
	if (ship->clazz == IMPERIAL_II_STAR_DESTROYER_SHIP)
		return true;
	if (ship->clazz == SHRIWIRR_SHIP)
		return true;
	if (ship->clazz == YORIK_VEC_ASSAULT_CRUISER_SHIP)
		return true;
	if (ship->clazz == ROIK_CHUUN_MARH_FRIGATE_SHIP)
		return true;
	if (ship->clazz == KOR_CHOKK_GRAND_CRUISER_SHIP)
		return true;
	if (ship->clazz == MIID_ROIK_WARSHIP_SHIP)
		return true;
	if (ship->clazz == PICKET_SHIP_FWSEN_CLASS_SHIP)
		return true;
	if (ship->clazz == WURRIF_CLASS_CRUISER_SHIP)
		return true;
	if (ship->clazz == SHREE_CLASS_BATTLE_CRUISER)
		return true;
	return false;
}

bool is_huge(SHIP_DATA *ship)
{
	if (ship->clazz == HUGE_SHIP)
		return true;
	if (ship->clazz == SUPER_STAR_DESTROYER_SHIP)
		return true;
	if (ship->clazz == SOVEREIGN_SUPER_STAR_DESTROYER_SHIP)
		return true;
	if (ship->clazz == ECLIPSE_SUPER_STAR_DESTROYER_SHIP)
		return true;
	if (ship->clazz == KOROS_STRONHA_SPIRAL_ARMED_WORLDSHIP_SHIP)
		return true;
	return false;
}

bool is_platform(SHIP_DATA *ship)
{
	if (ship->clazz == PLATFORM_SHIP)
		return true;
	if (ship->clazz == SPACE_FACTORY_STATION_SHIP)
		return true;
	if (ship->clazz == EMPRESS_SPACE_PLATFORM_SHIP)
		return true;
	if (ship->clazz == GOLAN_I_SPACE_DEFENSE_STATION_SHIP)
		return true;
	if (ship->clazz == GOLAN_II_SPACE_DEFENSE_STATION_SHIP)
		return true;
	if (ship->clazz == GOLAN_III_SPACE_DEFENSE_STATION_SHIP)
		return true;
	if (ship->clazz == TORPEDO_SPHERE_SHIP)
		return true;
	if (ship->clazz == DEATH_STAR_SHIP)
		return true;
	if (ship->clazz == SPACE_TUG_SHIP)
		return true;
	if (ship->clazz == UTILITY_TUG_SHIP)
		return true;
	if (ship->clazz == HEAVY_LIFTER_SHIP)
		return true;
	if (ship->clazz == SPACETROOPER_SHIP)
		return true;
	if (ship->clazz == PDV_MESSENGER_POD_SHIP)
		return true;
	if (ship->clazz == IMPERIAL_PROBOT_POD_SHIP)
		return true;
	if (ship->clazz == JABITHA_SHIP)
		return true;
	return false;
}

bool is_ship(SHIP_DATA *ship)
{
	if (is_scout(ship))
		return true;
	if (is_fighter(ship))
		return true;
	if (is_midship(ship))
		return true;
	if (is_freighter(ship))
		return true;
	if (is_capital(ship))
		return true;
	if (is_huge(ship))
		return true;
	if (is_platform(ship))
		return true;
	return false;
}

bool same_star_pos(float system, float system2, float arg, float arg2,
		int doklad, int doklad2)
{

	int a;

	for (a = -abs(doklad); a < abs(doklad) + 1; a = a + 1)
	{
		if (system == arg + a)
		{
			for (a = -abs(doklad2); a < abs(doklad2) + 1; a = a + 1)
			{
				if (system2 == arg2 + a)
					return true;
			}
		}
	}
	return false;
}

void calculate_number(CHAR_DATA *ch, char *arg_x, char *arg_y, SHIP_DATA *ship)
{
	char dane[256];
	int chance;

	if (ch->main_ability != 1 && !IS_SET(ch->act, PLR_HOLYLIGHT))
	{
		send_to_char(
				FB_RED "Przykro mi ale tylko pilot z krwi i ko�ci mo�e dokona� takich cud�w." NL,
				ch);
		return;
	}
	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_advancenavigation]);
	if (number_percent() > chance)
	{
		send_to_char(FB_RED "Nie uda�o Ci si� nic wyliczy�." NL
		"By� mo�e to lepiej, jeszcze by� wyl�dowa� w �rodku supernowej." NL,
				ch);
		learn_from_failure(ch, gsn_advancenavigation);
		if (ship->currjump)
		{
			ship->currjump = NULL;
			ship->jx = 0;
			ship->jy = 0;
			ship->jz = 0;
		}
		if (ship->vXpos != 0 || ship->vYpos != 0)
		{
			ship->vXpos = 0;
			ship->vYpos = 0;
			STRDUP(ship->sXpos, "");
			STRDUP(ship->sYpos, "");
		}
		return;
	}
	for (auto* starsystem : starsystem_list)
	{
		if (same_star_pos(starsystem->xpos, starsystem->ypos, atoi(arg_x),
				atoi(arg_y), 50, 50))
		{
			sprintf(dane, "'%s' 10000 %d %d", starsystem->name,
					number_range(-500, 500), number_range(-1000, 1000));
			do_calculate(ch, dane);
			return;
		}
	}
	if (ship->currjump)
		ship->currjump = NULL;
	if (atoi(arg_x) == 0 && atoi(arg_y) == 0)
	{
		send_to_char(
				FB_RED "�rodek galaktyki jest zbyt g�sty by mo�na w nim by�o podr�owa�." NL,
				ch);
		return;
	}
	ship->vXpos = atoi(arg_x);
	ship->vYpos = atoi(arg_y);
	STRDUP(ship->sXpos, arg_x);
	STRDUP(ship->sYpos, arg_y);
	ship->jx = number_range(-10000, 10000);
	ship->jy = number_range(-10000, 10000);
	ship->jz = number_range(-10000, 10000);

	ship->hyperdistance = srange((ship->starsystem->xpos - ship->vXpos),
			(ship->starsystem->ypos - ship->vYpos), 0);

	ship->calctimer = number_range(8, 13);
	if (ship->hyperdistance < ship->hyperspeed * 5)
	{
		ship->hyperdistance = ship->hyperspeed * 5;
		ship->calctimer = number_range(5, 9);
	}
	sound_to_room(ch->in_room, "!!SOUND(computer)");

	send_to_char( FB_GREEN "Wsp�rz�dne hiperprzestrzenne wprowadzone." EOL, ch);
	send_to_char( FG_MAGENTA "Rozpoczynam przeliczanie kursu." EOL, ch);
	act( PLAIN,
			"$n wykonuje jakie� skomplikowane obliczenia na komputerze statku.",
			ch, NULL, NULL, TO_ROOM);

	learn_from_success(ch, gsn_advancenavigation);

	WAIT_STATE(ch, 2*PULSE_VIOLENCE);
}

void new_starsystem(char *x, char *y)
{

	SPACE_DATA *starsystem;
	STAR_DATA *star;
	MOON_DATA *moon;
	char name[256];
	char filename[256];

	CREATE(starsystem, SPACE_DATA, 1);
	starsystem_list.push_back(starsystem);

	sprintf(name, "%s_%s", x, y);
	STRDUP(starsystem->resetmsg, "");
	STRDUP(starsystem->name, name);
	STRDUP(starsystem->description, "");
	STRDUP(starsystem->description2, "");
	STRDUP(starsystem->description3, "");
	if (number_range(1, 10) == 1)
	{
		CREATE(star, STAR_DATA, 1);
		STRDUP(star->name, "Jaka� Gwiazda");
		star->type = 0;
		star->xpos = 0;
		star->ypos = 0;
		star->zpos = 0;
		star->gravity = 300;
		starsystem->stars.push_back(star);
	}
	if (number_range(1, 60) == 6)
	{
		CREATE(moon, MOON_DATA, 1);
		STRDUP(moon->name, "Niezamieszkana planeta");
		moon->type = 0;
		moon->xpos = number_range(1000, 10000);
		moon->ypos = number_range(-10000, -1000);
		moon->zpos = number_range(-10000, 10000);
		moon->vnum = 0;
		moon->gravity = 100 + number_range(-10, 100);
		starsystem->moons.push_back(moon);
	}
	if (number_range(1, 10) == 6)
	{
		TOGGLE_BIT(starsystem->flags, STARS_EVENT_ION);
	}
	if (number_range(1, 10) == 6)
	{
		TOGGLE_BIT(starsystem->flags, STARS_EVENT_METEOR);
	}
	if (number_range(1, 20) == 6)
	{
		TOGGLE_BIT(starsystem->flags, STARS_FLAG_EMP);
	}
	TOGGLE_BIT(starsystem->flags, STARS_FLAG_TEMP);
	starsystem->xpos = atoi(x);
	starsystem->ypos = atoi(y);

	sprintf(filename, "%s_%s.system", x, y);
	STRDUP(starsystem->filename, filename);
	save_starsystem(starsystem);
	write_starsystem_list();
	return;
}

void damage_ship_energy(SHIP_DATA *ship, int min, int max, CHAR_DATA *ch,
		SHIP_DATA *killer)
{
	int damage, shield_dmg;

	damage = number_range(min, max);

	if (ship->shield > 0)
	{
		shield_dmg = UMIN(ship->shield, damage);
		damage -= shield_dmg;
		ship->shield -= shield_dmg;
		if (ship->shield <= 0)
		{
			ship->shield = 0;
			echo_to_cockpit(ship, FG_RED "Tarcze prze�amane...");
		}
	}

	ship->energy -= damage * 50;

	if (ship->energy <= 0)
	{
		ship->shipstate1 = damage;
		ch_printf(ch, FB_WHITE "%s unieruchomiony." NL,
				know_trans(killer, ship) ? SHIPNAME(ship) : ship->transponder);
		return;
	}

	if (ship->energy <= ship->maxenergy / 10)
		echo_to_cockpit(ship,
				FG_RED MOD_BLINK "UWAGA!!! Zagro�enie jonizacj� system�w!" RESET);
}

bool check_crew(SHIP_DATA *ship, CHAR_DATA *ch, const char *argument)
{
	if (!str_cmp(argument, "all") || !str_cmp(argument, "capitan")
			|| !str_cmp(argument, "pilot") || !str_cmp(argument, "copilot")
			|| !str_cmp(argument, "navigator") || !str_cmp(argument, "gunner"))
		for (auto* member : cmember_list)
			if (member->ship == ship)
				if (member->name == ch)
					return true;
	return false;
}

void echo_to_crew(SHIP_DATA *ship, char *argument)
{
	for (auto* member : cmember_list)
		if (member->ship == ship)
			if (member->name != NULL)
				send_to_char(argument, member->name);
	return;
}

TURRET_DATA* get_turret(SHIP_DATA *ship, int nr)
{
	int i = 0;

	for (auto* turret : ship->turrets)
	{
		if (i == nr)
			return turret;
		i++;
	}

	return NULL;
}

TURRET_DATA* get_turret_here(SHIP_DATA *ship, ROOM_INDEX_DATA *room)
{
	for (auto* turret : ship->turrets)
		if (turret->vnum == VNUM(room))
			return turret;
	return NULL;
}

bool is_vnum_ship_system(SHIP_DATA *ship, int vnum, const char *argument)
{
	char arg[MSL];

	strcpy(arg, argument);

	for (auto* turret : ship->turrets)
		if (turret->vnum == vnum)
			return true;
	for (auto* hangar : ship->hangars)
		if (hangar->vnum == vnum)
			return true;
	for (auto* dock : ship->docks)
		if (dock->vnum == vnum)
			return true;
	if (!str_cmp(arg, "cockpit") || !str_cmp(arg, "pilotseat")
			|| !str_cmp(arg, "coseat") || !str_cmp(arg, "navseat")
			|| !str_cmp(arg, "gunseat") || !str_cmp(arg, "entrance")
			|| !str_cmp(arg, "dock"))
	{
		if (vnum == VNUM(ship->engineroom))
			return true;
	}
	if (!str_cmp(arg, "turret") || //!str_cmp(arg,"hanger") || Pixel: nie ma hangErow!!!
			!str_cmp(arg, "engineroom") || !str_cmp(arg, "hangar"))
	{
		if (vnum == VNUM(ship->engineroom) || vnum == VNUM(ship->cockpit)
				|| vnum == VNUM(ship->pilotseat) || vnum == VNUM(ship->coseat)
				|| vnum == VNUM(ship->navseat) || vnum == VNUM(ship->gunseat)
				|| vnum == VNUM(ship->entrance))
			return true;
	}

	return false;
}

void write_ship_all()
{
	for (auto* ship : ship_list)
		save_ship(ship);
}

void write_planet_all()
{
	for (auto* planet : planet_list)
		save_planet(planet);
}

void write_systems_all()
{
	for (auto* starsystem : starsystem_list)
		save_starsystem(starsystem);
}

MOON_DATA* get_moon(SPACE_DATA *system, int nr)
{
	int i = 0;

	for (auto* moon : system->moons)
	{
		if (i == nr)
			return moon;
		i++;
	}

	return NULL;
}

MOON_DATA* moon_from_name(SPACE_DATA *system, char *name)
{
	for (auto* moon : system->moons)
	{
		if (!str_cmp(name, moon->name))
			return moon;
	}
	for (auto* moon : system->moons)
	{
		if (nifty_is_name_prefix(name, moon->name))
			return moon;
	}
	return NULL;
}

DOCK_DATA* get_dock(PLANET_DATA *planet, int nr)
{
	int i = 0;

	for (auto* dock : planet->docks)
	{
		if (i == nr)
			return dock;
		i++;
	}

	return NULL;
}

DOCK_DATA* dock_from_name(SPACE_DATA *starsystem, char *argument)
{
	for (auto* planet : starsystem->planets)
	{
		for (auto* dock : planet->docks)
		{
			if (!str_cmp(argument, dock->name))
				return dock;
		}
		for (auto* dock : planet->docks)
		{
			if (dock->hidden == 0)
				if (nifty_is_name_prefix(argument, dock->name))
					return dock;
		}
	}
	return NULL;
}

PLANET_DATA* planet_from_dock(DOCK_DATA *dock, CHAR_DATA *ch)
{
	SPACE_DATA *starsystem;
	SHIP_DATA *ship;

	if ((ship = ship_from_room(ch->in_room)) != NULL)
	{
		if ((starsystem = ship->starsystem) == NULL)
			return NULL;
	}
	else
	{
		if ((starsystem = ch->in_room->area->planet->starsystem) == NULL)
			return NULL;
	}
	for (auto* planet : starsystem->planets)
	{
		for (auto* vdock : planet->docks)
		{
			if (vdock == dock)
				return planet;
		}
	}
	return NULL;
}

HANGAR_DATA* get_hangar(SHIP_DATA *ship, int nr)
{
	int i = 0;

	for (auto* hangar : ship->hangars)
	{
		if (i == nr)
			return hangar;
		i++;
	}

	return NULL;
}

HANGAR_DATA* hangar_from_room(SHIP_DATA *ship, int vnum)
{
	for (auto* hangar : ship->hangars)
		if (hangar->vnum == vnum)
			return hangar;

	return NULL;
}

CARGO_DATA* get_cargo(SHIP_DATA *ship, int nr)
{
	int i = 0;

	for (auto* cargo : ship->cargo_list)
	{
		if (i == nr)
			return cargo;
		i++;
	}

	return NULL;
}

float check_capacity(int vnum)
{
	float cur_cap = 0;
	ROOM_INDEX_DATA *room;

	room = get_room_index(vnum);
	if (vnum <= 0 || !room)
		return -1;

	for (auto* tship : room->ships)
		cur_cap += tship->size;
	return cur_cap;
}

bool has_hangar(SHIP_DATA *ship)
{
	return !ship->hangars.empty();
}

STAR_DATA* get_star(SPACE_DATA *system, int nr)
{
	int i = 0;

	for (auto* star : system->stars)
	{
		if (i == nr)
			return star;
		i++;
	}

	return NULL;
}

bool is_dock_area(PLANET_DATA *planet, int dock)
{
	int vnum;

	for (auto* area : planet->areas)
		for (vnum = area->lvnum; vnum <= area->uvnum; vnum++)
			if (vnum == dock)
				return true;
	return false;
}

TRANSPONDER_DATA* get_transponder(SHIP_DATA *ship, int nr)
{
	int i = 0;

	for (auto* transponder : ship->transponders)
	{
		if (i == nr)
			return transponder;
		i++;
	}

	return NULL;

}

void generate_transponder(SHIP_DATA *ship)
{
	char n_transponder[MSL];

	sprintf(n_transponder, "T%d", number_range(100000, 999999));
	for (auto* target : ship_list)
		if (!str_cmp(target->transponder, n_transponder))
			generate_transponder(ship);
	STRDUP(ship->transponder, n_transponder);
	/* Thanos: awaryjnie daje to:
	 STRDUP( ship->transponder, ship->name );*/
	return;
}

DEF_DO_FUN( crack_transponder )
{
	ch_printf(ch, FB_RED "Przykro Nam ta komenda jeszcze nie dzia�a" EOL);
	return;
}

DEF_DO_FUN( transponders )
{
	SHIP_DATA *ship, *target;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�!" EOL,
				ch);
		return;
	}

	if (!is_ship(ship))
	{
		send_to_char(FB_RED "Nie jeste� na statku kosmicznym!" EOL, ch);
		return;
	}

	pager_printf(ch, "Rozpoznane statki:" EOL);
	for (auto* transponder : ship->transponders)
	{
		if ((target = get_ship(transponder->shipname)) == NULL)
			continue;
		pager_printf(ch, "%s  %-20s %s" EOL, transponder->number,
				bit_name(ship_classes_list, target->clazz), SHIPNAME(target));
	}
	return;
}

bool know_trans(SHIP_DATA *ship, SHIP_DATA *target)
{
	for (auto* transponder : ship->transponders)
		if (!str_cmp(transponder->number, target->transponder))
			return true;
	return false;
}

void clear_transponders(SHIP_DATA *ship)
{
	auto snapshot = ship->transponders;
	for (auto* transponder : snapshot)
	{
		ship->transponders.remove(transponder);
		free_transponder(transponder);
	}
	return;
}

bool is_ship_fight(SHIP_DATA *ship)
{
	SPACE_DATA *starsystem;

	if ((starsystem = ship->starsystem) == NULL)
		return false;
	for (auto* target : starsystem->ships)
	{
		if (target->target0 == ship)
			return true;
		for (auto* turret : target->turrets)
			if (turret->target == ship)
				return true;
	}
	return false;
}

MODULE_DATA* get_module(SHIP_DATA *ship, int nr)
{
	int i = 0;

	for (auto* module : ship->modules)
	{
		if (i == nr)
			return module;
		i++;
	}

	return NULL;
}

DEF_DO_FUN( clear_modules )
{
	for (auto* target : ship_list)
	{
		auto mod_snapshot = target->modules;
		for (auto* module : mod_snapshot)
		{
			target->modules.remove(module);
			STRDUP(module->spyname, "");
			DISPOSE(module);
		}
	}
	send_to_char(FB_GREEN "Done!" NL, ch);
	return;
}

// liczymy moduly z wyjatkiem pluskiew
int count_modules_all(SHIP_DATA *ship)
{
	int count = 0;

	for (auto* module : ship->modules)
	{
		if (module->type == 25)
			continue;
		count++;
	}
	return count;
}

// liczymy moduly danego typu
int count_modules(SHIP_DATA *ship, int nr)
{
	int count = 0;

	for (auto* module : ship->modules)
		if (module->type == nr)
			count++;
	return count;
}

//szukamy modulu mogacego zadzialac ? 
MODULE_DATA* get_module_ready(SHIP_DATA *ship, int type)
{
	for (auto* module : ship->modules)
		if (module->type == type && module->timer <= 0
				&& module->status - module->crs < module->status)
			return module;

	return NULL;
}

DEF_DO_FUN( uninstal_module )
{
	MODULE_DATA *module;
	SHIP_DATA *ship;
	char arg[MSL];

	argument = one_argument(argument, arg);

	if (argument[0] == '\0')
	{
		if ((ship = ship_from_room(ch->in_room)) == NULL)
		{
			send_to_char(
					FB_RED "W�a�ciwa sk�adnia komendy:  unmod nr 'nazwa statku'" NL
					"   gdzie nr - to numer modu�u do deinstalacji!" NL, ch);
			return;
		}
		if (!is_number(arg))
		{
			send_to_char(FB_RED "Musisz poda� numer modu�u." NL, ch);
			return;
		}
		if ((module = get_module_ch(ship, ch->in_room->vnum, atoi(arg)))
				== NULL)
		{
			send_to_char(
					FB_RED "Sprawdz 'modules' i sprecyzuj, kt�ry modu� chcesz odinstalowa�." NL,
					ch);
			return;
		}
		bug("%s", module_type_name[module->type]);
		return;
	}
	else
	{
		if ((ship = ship_in_room(ch->in_room, argument)) == NULL)
		{
			send_to_char(FB_RED "Nie ma tu takiego statku." NL, ch);
			return;
		}
		if (!is_number(arg))
		{
			send_to_char(FB_RED "Musisz poda� numer modu�u." NL, ch);
			return;
		}
		if ((module = get_module_ch(ship, 0, atoi(arg))) == NULL)
		{
			send_to_char(
					FB_RED "Sprawdz 'modules 'nazwa statku'' i sprecyzuj, kt�ry modu� chcesz odinstalowa�." NL,
					ch);
			return;
		}
		bug("%s", module_type_name[module->type]);
		return;
	}
}

DEF_DO_FUN( modules )
{
	SHIP_DATA *ship;

	if (argument[0] == '\0')
	{
		if ((ship = ship_from_room(ch->in_room)) == NULL)
		{
			send_to_char(
					FB_RED "Musisz sta� na statku lub b�d�c w jego pobli�u poda� nazw�." NL,
					ch);
			return;
		}
		show_module_to_char(ship, ch, ch->in_room->vnum);
	}
	else
	{
		if ((ship = ship_in_room(ch->in_room, argument)) == NULL)
		{
			send_to_char(FB_RED "Nie ma tu takiego statku" NL, ch);
			return;
		}
		show_module_to_char(ship, ch, 0);
	}
	return;
}

//pokazujemy inzynierowi moduly
void show_module_to_char(SHIP_DATA *ship, CHAR_DATA *ch, int nvnum)
{
	int a = 0;

	for (auto* module : ship->modules)
		if (module->vnum == nvnum)
		{
			pager_printf(ch, "%-3d %-35s %d" NL, a + 1,
					module_name_nice[module->type],
					module->crs * 100 / module->status);
			a++;
		}
	if (a == 0)
		send_to_char(FB_RED "Nie zainstalowano tu modu��w." NL, ch);
	return;
}

//pokazujemy ladne nazwy modulow graczowi z conf +shipstat
void show_module_to_char0(SHIP_DATA *ship, CHAR_DATA *ch, int nvnum)
{
	int a = 0;
	bool found;

	found = false;
	pager_printf(ch, FB_CYAN "Modu�y Statku:" EOL);
	for (auto* module : ship->modules)
	{
		if (module->vnum == nvnum)
		{
			pager_printf(ch, FG_CYAN "%-35s %s", module_name_nice[module->type],
					a == 1 ? "" EOL : "");
			a++;
			found = true;
		}
		if (a > 1)
			a = 0;
	}
	if (!found)
		pager_printf(ch, FG_CYAN "Brak" EOL);
	else
		pager_printf(ch, EOL);
	return;
}

MODULE_DATA* get_module_ch(SHIP_DATA *ship, int nvnum, int nr)
{
	int a = 1;

	for (auto* module : ship->modules)
		if (module->vnum == nvnum)
		{
			if (a == nr)
				return module;
			a++;
		}
	return NULL;
}

DEF_DO_FUN( addengineer )
{
	SHIP_DATA *ship;
	CHAR_DATA *victim;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz byc w kokpicie statku by to zrobi�." NL,
				ch);
		return;
	}

	if (!is_ship(ship))
	{
		send_to_char( FB_RED "Nie mo�esz zrobi� tego tutaj." NL, ch);
		return;
	}

	if (str_cmp(ship->owner, ch->name))
	{

		if (!IS_NPC(ch) && ch->pcdata && ch->pcdata->clan
				&& !str_cmp(ch->pcdata->clan->name, ship->owner))
		{
			if (!IS_LEADER(ch) && !IS_FIRST(ch) && !IS_SECOND(ch))
			{
				send_to_char( FB_RED "To nie tw�j statek!" NL, ch);
				return;
			}
		}
		else
		{
			send_to_char( FB_RED "To nie tw�j statek!" NL, ch);
			return;
		}

	}

	if (argument[0] == '\0')
	{
		send_to_char( FB_RED "No to kto ma by� tym in�ynierem pok�adowym?" NL,
				ch);
		return;
	}

	if (str_cmp(ship->engineer, ""))
	{
		send_to_char( FB_RED "Masz ju� in�yniera pok�adowego.." NL, ch);
		send_to_char(
				FB_RED "Je�li chcesz go zmieni�, spr�buj najpierw 'remengineer'." NL,
				ch);
		return;
	}

	if ((victim = get_char_world(ch, argument)) == NULL)
	{
		ch_printf(ch, "Nie ma nikogo takiego jak %s" NL, argument);
		return;
	}
	if (IS_NPC(victim))
	{
		ch_printf(ch, "Mob nie mo�e by� in�ynierem na tym statku" NL);
		return;
	}
	if (victim->main_ability != ENGINEERING_ABILITY)
	{
		ch_printf(ch,
				"In�ynierem pok�adowym mo�e by� tylko gracz o technicznej profesji podstawowej" NL);
		return;
	}
	STRDUP(ship->engineer, argument);
	send_to_char("In�ynier zatrudniony." NL, ch);
	save_ship(ship);

}

DEF_DO_FUN( remengineer )
{
	SHIP_DATA *ship;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz byc w kokpicie statku by to zrobi�." NL,
				ch);
		return;
	}

	if (!is_ship(ship))
	{
		send_to_char( FB_RED "Nie mo�esz zrobi� tego tutaj." NL, ch);
		return;
	}

	if (str_cmp(ship->owner, ch->name))
	{

		if (!IS_NPC(ch) && ch->pcdata && ch->pcdata->clan
				&& !str_cmp(ch->pcdata->clan->name, ship->owner))
		{
			if (!IS_LEADER(ch) && !IS_FIRST(ch) && !IS_SECOND(ch))
			{
				send_to_char( FB_RED "To nie tw�j statek!" NL, ch);
				return;
			}
		}
		else
		{
			send_to_char( FB_RED "To nie tw�j statek!" NL, ch);
			return;
		}

	}

	if (argument[0] == '\0')
	{
		send_to_char( FB_RED "No to podasz mi to imi� czy nie?" NL, ch);
		return;
	}

	if (!str_cmp(ship->engineer, argument))
	{
		STRDUP(ship->engineer, "");
		send_to_char("In�ynier zwolniony ze stanowiska.", ch);
		save_ship(ship);
		return;
	}

	send_to_char( FB_RED "Ta osoba nie jest in�ynierem tego statku." NL, ch);

}

void generate_sslook(SHIP_DATA *ship)
{
	char nSslook[MSL];
	bool ok = false;

	while (!ok)
	{
		ok = true;
		sprintf(nSslook, "s%d%d%d", number_range(0, 9), number_range(0, 9),
				number_range(0, 9));
		for (auto* target : ship_list)
		{
			if (target == ship)
				continue;

			if (target->sslook[0] && !str_cmp(target->sslook, nSslook))
				ok = false;
		}
	}
	STRDUP(ship->sslook, nSslook);
	/* Thanos: awaryjnie daje to:
	 STRDUP( ship->sslook, ship->name );*/
	return;
}

SHIPDOCK_DATA* get_shipdock(SHIP_DATA *ship, int nr)
{
	int i = 0;

	for (auto* dock : ship->docks)
	{
		if (i == nr)
			return dock;
		i++;
	}

	return NULL;
}

SHIPDOCK_DATA* shipdock_from_room(SHIP_DATA *ship, int vnum)
{
	for (auto* dock : ship->docks)
		if (dock->vnum == vnum)
			return dock;

	return NULL;
}

DEF_DO_FUN( dock )
{
	char arg[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int chance, a;
	SHIP_DATA *ship;
	SHIP_DATA *target;
	SPACE_DATA *starsystem;
	SHIPDOCK_DATA *dock;
	SHIPDOCK_DATA *targetdock;

	strcpy(arg, argument);
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�!" EOL,
				ch);
		return;
	}

	if (!is_ship(ship))
	{
		send_to_char(FB_RED "Nie jeste� na statku kosmicznym!" EOL, ch);
		return;
	}

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz siedzie� w fotelu pilota by to zrobi�!" EOL,
				ch);
		return;
	}

	if (!str_cmp(arg, "show"))
	{
		if (!known_biotech(ship, ch))
		{
			pager_printf(ch,
					NL "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." NL);
			return;
		}
		pager_printf(ch,
				"Nr   Typ Zamka      Stan Zamka     Go��(nazwa/transponder)" EOL);
		pager_printf(ch,
				"-------------------------------------------------------------------------------" EOL);
		for (a = 0; auto* dock : ship->docks)
		{
			pager_printf(ch, "%-4d %-14s %-14s %s" EOL, a++,
					ship_dock_type[dock->type],
					dock->status == 0 ?
							"Zabezpieczony" :
							(dock->status == 1 ? "Otwarty" : "Wy�amany"),
					dock->target == NULL ?
							"" :
							(know_trans(ship, dock->target) ?
									SHIPNAME(dock->target) :
									dock->target->transponder));
		}
		return;
	}
	if (!check_crew(ship, ch, "pilot") && arg[0] != '\0')
	{
		send_to_char(FB_RED "Nie zajmujesz stanowiska pilota na tym statku." NL,
				ch);
		return;
	}

	if (ship->shipstate1 != 0)
	{
		send_to_char(FB_RED "BzZzzbBbbzZzz..." NL, ch);
		return;
	}
	if (!has_dock(ship))
	{
		send_to_char(FB_RED "Tw�j statek nie ma r�kaw�w dokuj�cych!" EOL, ch);
		return;
	}

	if (ship->shipstate == SHIP_LAND || ship->shipstate == SHIP_LAND_2)
	{
		send_to_char(FB_RED "Ten statek przeciez laduje!!" NL, ch);
		return;
	}
	if (ship->shipstate == SHIP_TRACTORED && arg[0] != '\0')
	{
		send_to_char(
				FB_RED "Statek jest unieruchomiony promieniami �ci�gaj�cymi." EOL,
				ch);
		return;
	}
	if (ship->shipstate == SHIP_DISABLED && arg[0] != '\0')
	{
		send_to_char(
				FB_RED "Nap�d statku jest uszkodzony. L�dowanie niemo�liwe." EOL,
				ch);
		return;
	}

	if (ship->shipstate == SHIP_DOCKED)
	{
		send_to_char(FB_RED "Statek na l�dowisku." EOL, ch);
		return;
	}

	if (ship->shipstate == SHIP_HYPERSPACE)
	{
		send_to_char(FB_RED "Nie mo�esz zrobic tego w hiperprzestrzeni!" EOL,
				ch);
		return;
	}

	if (is_linked(ship, NULL))
	{
		send_to_char(
				FB_RED "Nie mo�na w jednej chwili cumowa� do dw�ch statk�w." EOL,
				ch);
		return;
	}
	if (ship->shipstate != SHIP_READY && arg[0] != '\0')
	{
		send_to_char(
				FB_RED "Poczekaj, a� statek zako�czy sw�j aktualny manewr." EOL,
				ch);
		return;
	}

	if ((starsystem = ship->starsystem) == NULL)
	{
		send_to_char(FB_RED "Ale tu nie ma do czego przycumowa�!" EOL, ch);
		return;
	}

	if (ship->energy < (25 + ship->size / 5))
	{
		send_to_char(FB_RED "Za ma�o paliwa!" EOL, ch);
		return;
	}

	if (arg1[0] == '\0')
	{
		do_land(ch, (char*) "");
		return;
	}

	if ((dock = get_shipdock(ship, atoi(argument))) == NULL)
	{
		send_to_char(
				FB_RED "Musisz poda� prawid�owy numer w�asnego r�kawa cumowniczego." EOL,
				ch);
		return;
	}
	if (dock->target != NULL)
	{
		sprintf(buf, FB_RED "Ten r�kaw jest ju� u�ywany przez %s" EOL,
				know_trans(ship, dock->target) ?
						SHIPNAME(dock->target) : dock->target->transponder);
		send_to_char(buf, ch);
		return;
	}
	if ((target = get_ship_here(arg1, ship->starsystem)) != NULL)
	{
		if (target == ship)
		{
			send_to_char(
					FB_RED "Nie mo�na dokowa� do statku, w kt�rym jeste�!" EOL,
					ch);
			return;
		}
		if (target->cloack != 0)
		{
			send_to_char(
					FB_RED "Nie ma tu takiej stacji. Wpisz 'dock' by zobaczy� dost�pne l�dowiska!" EOL,
					ch);
			return;
		}
		if (!has_dock(target))
		{
			send_to_char(FB_RED "Ten statek nie ma r�kaw�w cumowniczych!" EOL,
					ch);
			return;
		}
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > ship->comm * 1500)
		{
			send_to_char(
					FB_RED "Komputer nie mo�e poprawnie namierzy� sygna�u ILS" NL,
					ch);
			return;
		}
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz))
				> 200 + (ship->size + target->size) / 10)
		{
			send_to_char(
					FB_RED "To za daleko! Musisz podlecie� nieco bli�ej." EOL,
					ch);
			return;
		}
		if ((targetdock = get_shipdock(target, atoi(arg2))) == NULL)
		{
			send_to_char(
					FB_RED "Musisz poda� prawid�owy numer r�kawa cumowniczego celu." EOL,
					ch);
			return;
		}
		if (targetdock->target != NULL)
		{
			sprintf(buf,
					FB_RED "Navigator" FB_CYAN " og�asza przez komunikator 'Ten r�kaw jest ju� u�ywany'" EOL);
			send_to_char(buf, ch);
			return;
		}
		if (targetdock->type != dock->type)
		{
			sprintf(buf, FB_RED "Niezgodno�� ��czy cumowniczych" EOL);
			send_to_char(buf, ch);
			return;
		}
	}
	else
	{
		send_to_char(FB_RED "Nie ma tu takiego l�dowiska." NL
		"   Wpisz 'DOCK' a uzyskasz spis dost�pnych l�dowisk" NL, ch);
		return;
	}

	chance = get_space_chance(ship, ch);
	if (number_percent() < chance)
	{
		send_to_char( FB_GREEN, ch);
		send_to_char("Procedura dokowania zainicjowana." EOL, ch);
		act( PLAIN,
				"$n rozpoczyna wykonywanie sekwencji dokowania zewn�trznego.",
				ch, NULL, argument, TO_ROOM);
		if (autofly(ship) && arg[0] != '\0')
		{
			send_to_char(
					FB_GREEN "[Autopilot]: " FB_CYAN "Potwierdzam.... Wy��czam si�." EOL,
					ch);
			ship->autopilot = false;
		}
		ship->shipstate = SHIP_DOCKED2;
		ship->currspeed = target->currspeed;
		ship->hx = target->hx;
		ship->hy = target->hy;
		ship->hz = target->hz;
		STRDUP(ship->dest, arg);
		learn_from_success_space(ship, ch);
		if (ship->last_dock_with != target)
		{
			int xp = ((exp_level(ch->skill_level[PILOTING_ABILITY] + 1)
					- exp_level(ch->skill_level[PILOTING_ABILITY])) / 8);
			xp = UMIN(get_ship_value(ship) / 1000, xp);
			gain_exp(ch, xp / 10, PILOTING_ABILITY);
			ch_printf(ch,
					"&WZdobywasz %ld punkt�w do�wiadczenia w pilotowaniu!" EOL,
					xp / 10);
		}
		return;
	}
	send_to_char("Jako� nie uda�o ci si� poprawnie obs�u�y� urz�dze�." EOL, ch);
	learn_from_failure_space(ship, ch);
	return;
}

void dockship(SHIP_DATA *ship, char *argument)
{
	SHIP_DATA *target;
	SHIPDOCK_DATA *dock, *targetdock;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	target = get_ship_here(arg1, ship->starsystem);
	targetdock = get_shipdock(target, atoi(arg2));
	dock = get_shipdock(ship, atoi(argument));

	if (target != ship && target != NULL && targetdock != NULL && dock != NULL
			&& dock->type == targetdock->type && dock->target == NULL
			&& targetdock->target == NULL)
	{
	}
	else
	{
		echo_to_room(ship->pilotseat,
		FB_YELLOW "Nie mo�na zacumowa�! Sekwencja dokuj�ca przerwana.");
		echo_to_ship(ship, FB_YELLOW "Statek gwa�townie zmiena kurs.");
		if (ship->shipstate != SHIP_DISABLED)
			ship->shipstate = SHIP_READY;
		return;
	}

	echo_to_room(ship->pilotseat, FB_YELLOW "Procedura dokowania zako�czona.");
	echo_to_ship(ship, FB_YELLOW "Czujesz delikatny wstrz�s.");
	dock->target = target;
	targetdock->target = ship;
	dock->targetvnum = targetdock->vnum;
	dock->master_slave = 0;
	targetdock->targetvnum = dock->vnum;
	targetdock->master_slave = 1;
	if (ship->cloack != 0)
		ship->cloack = 0;
	sprintf(buf, FB_YELLOW "%s dokuje do %s.", ship->sslook, target->sslook);
	echo_to_system(ship, buf, target);
	if (ship->shipstate != SHIP_DISABLED)
		ship->shipstate = SHIP_DOCKED2_FIN;

	ship->energy = ship->energy - 25 - ship->size / 5;

	if (!str_cmp("Public", ship->owner))
	{
		ship->energy = ship->maxenergy;
		ship->chaff = ship->maxchaff;
		ship->missiles = ship->maxmissiles;
		ship->torpedos = ship->maxtorpedos;
		ship->rockets = ship->maxrockets;
		ship->shield = 0;
		ship->autorecharge = false;
		ship->autotrack = false;
		ship->autospeed = false;
		ship->hull = ship->maxhull;
		ship->missilestate = MISSILE_READY;
		ship->statet0 = LASER_READY;
		ship->shipstate = SHIP_DOCKED;
		ship->hyperready = -1;
		ship->hypertimer = 0;
		ship->calctimer = 0;

		echo_to_cockpit(ship, FB_YELLOW "Naprawa i przetankowanie...");
	}

}

DEF_DO_FUN( undock )
{
	int chance;
	long price = 0;
	SHIP_DATA *ship, *target;
	SHIPDOCK_DATA *dock, *targetdock;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�!" EOL,
				ch);
		return;
	}

	if (!is_ship(ship))
	{
		send_to_char(FB_RED "Nie jeste� na statku kosmicznym!" EOL, ch);
		return;
	}

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		ch_printf(ch,
				FB_RED "Nie wygl�da na to, �eby� by�%s w kabinie pilota!" EOL,
				SEX_SUFFIX__AO(ch));
		return;
	}

	if (!check_crew(ship, ch, "pilot"))
	{
		send_to_char(FB_RED "Nie zajmujesz stanowiska pilota na tym statku." NL,
				ch);
		return;
	}

	if (ship->shipstate1 != 0)
	{
		send_to_char(FB_RED "BzZzzbBbbzZzz..." NL, ch);
		return;
	}
	if (autofly(ship))
	{
		send_to_char(
				FB_RED "Statek ma w��czonego autopilota. Mo�e najpierw go wy��cz." EOL,
				ch);
		return;
	}

	if (ship->shipstate != SHIP_DOCKED2_FIN
			|| (dock = shipdock_from_slave(ship)) == NULL)
	{
		send_to_char(
				FB_RED "Nie wygl�da na to, �eby statek by� zacumowany." EOL,
				ch);
		return;
	}
	chance = get_space_chance(ship, ch);
	if (number_percent() < chance)
	{
		if (is_rental(ch, ship))
			if (!rent_ship(ch, ship))
				return;
		if (!is_rental(ch, ship))
		{
			if (is_scout(ship))
				price = 15;
			if (is_fighter(ship))
				price = 20;
			if (is_midship(ship))
				price = 50;
			if (is_freighter(ship))
				price = 100;
			if (is_capital(ship))
				price = 500;
			if (is_huge(ship))
				price = 1000;
			price += (ship->maxhull - ship->hull);
			if (ship->missiles)
				price += (50 * (ship->maxmissiles - ship->missiles));
			else if (ship->torpedos)
				price += (75 * (ship->maxtorpedos - ship->torpedos));
			else if (ship->rockets)
				price += (150 * (ship->maxrockets - ship->rockets));
			if (ship->shipstate == SHIP_DISABLED)
				price += 200;
			if (ship->missilestate == MISSILE_DAMAGED)
				price += 100;
			if (ship->statet0 == LASER_DAMAGED)
				price += 50;
		}
		if (ch->pcdata && ch->pcdata->clan
				&& !str_cmp(ch->pcdata->clan->name, ship->owner))
		{
			if (ch->pcdata->clan->funds < price)
			{
				ch_printf(ch,
						FB_RED "%s nie ma wystarczaj�cej ilo�ci funduszy, by przygotowac ten statek do startu." EOL,
						ch->pcdata->clan->name);
				return;
			}
			ch->pcdata->clan->funds -= price;
			ch_printf(ch,
					"&GPrzygotowanie do lotu tego statku b�dzie kosztowa�o %s %ld kredytek." EOL,
					ch->pcdata->clan->name, price);
		}
		else if (str_cmp(ship->owner, "Public"))
		{
			if (ch->gold < price)
			{
				ch_printf(ch,
						FB_RED "Nie masz wystarczaj�cych funduszy by przygotowac ten statek do lotu." EOL);
				return;
			}
			ch->gold -= price;
			ch_printf(ch,
					"&GP�acisz %ld kredytek by przygotowa� statek do lotu." EOL,
					price);
		}
		ship->energy = ship->maxenergy;
		ship->chaff = ship->maxchaff;
		ship->missiles = ship->maxmissiles;
		ship->torpedos = ship->maxtorpedos;
		ship->rockets = ship->maxrockets;
		ship->shield = 0;
		ship->autorecharge = false;
		ship->autotrack = false;
		ship->autospeed = false;
		ship->hull = ship->maxhull;
		ship->missilestate = MISSILE_READY;
		ship->statet0 = LASER_READY;
		ship->shipstate = SHIP_DOCKED2_FIN;
		if ((target = dock->target) == NULL)
		{
			bug("DO_UNDOCK:/target/ Sh:%s, Dv:%d", SHIPNAME(ship), dock->vnum);
			send_to_char(
					FB_RED "Bzyk Bzyk.... cos sie popsu�o" NL "Zg�osi�em ju� ten b��d." EOL,
					ch);
			return;
		}
		if ((targetdock = shipdock_from_room(target, dock->targetvnum)) == NULL)
		{
			bug("DO_UNDOCK:/targetvnum/ Sh:%s, Dv:%d, T:%s", SHIPNAME(ship),
					dock->vnum, SHIPNAME(target));
			send_to_char(
					FB_RED "Bzyk Bzyk.... cos sie popsu�o" NL "Zg�osi�em ju� ten b��d." EOL,
					ch);
			return;
		}
		send_to_char( FB_GREEN, ch);
		send_to_char("Sekwencja uwolnienia cumy zainicjowana." EOL, ch);
		act( PLAIN, "$n odcumowuje i uruchamia silniki statku.", ch, NULL,
				argument, TO_ROOM);
		dock->targetvnum = 0;
		dock->target = NULL;
		dock->master_slave = -1;
		targetdock->targetvnum = 0;
		targetdock->target = NULL;
		targetdock->master_slave = -1;
		ship->last_dock_with = target;
		ship->shipstate = SHIP_READY;
		ship->currspeed = ship->realspeed / 2;
		ship->hx = ship->vx - target->vx - ship->vx;
		ship->hy = ship->vy - target->vy - ship->vy;
		ship->hz = ship->vz - target->vz - ship->vz;
		if (dock->status == 1)
		{
			echo_to_room(get_room_index(dock->vnum),
					FB_YELLOW "�luza r�kawa cumowniczego zamyka si�.");
			sound_to_room(get_room_index(dock->vnum), "!!SOUND(door)");
			dock->status = 0;
		}
		if (targetdock->status == 1)
		{
			echo_to_room(get_room_index(targetdock->vnum),
					FB_YELLOW "�luza r�kawa cumowniczego zamyka si�.");
			sound_to_room(get_room_index(targetdock->vnum), "!!SOUND(door)");
			targetdock->status = 0;
		}
// Aldegard dopisa� wybijanie gosci w niedomkni�tej �luzie :)
		learn_from_success_space(ship, ch);
		return;
	}
	send_to_char( FB_RED, ch);
	send_to_char("Nie uda�o ci si� poprawnie obs�u�y� urz�dze�!" EOL, ch);
	learn_from_failure_space(ship, ch);
	return;
}

void undockship(SHIP_DATA *ship)
{
}

bool has_dock(SHIP_DATA *ship)
{
	return !ship->docks.empty();
}

SHIP_DATA* ship_from_dock(int vnum)
{
	for (auto* ship : ship_list)
		for (auto* dock : ship->docks)
			if (dock->vnum == vnum)
				return ship;
	return NULL;
}

void leave_ship_by_dock(SHIP_DATA *ship, CHAR_DATA *ch)
{
	ROOM_INDEX_DATA *fromroom;
	ROOM_INDEX_DATA *toroom;
	SHIPDOCK_DATA *dock;

	fromroom = ch->in_room;

	if ((ship = ship_from_dock(fromroom->vnum)) == NULL)
	{
		send_to_char( FB_RED "Nie widz� tu �adnego wyj�cia." EOL, ch);
		return;
	}

	if ((dock = shipdock_from_room(ship, fromroom->vnum)) == NULL)
	{
		send_to_char( FB_RED "Nie widz� tu �adnego wyj�cia." EOL, ch);
		return;
	}
	if (dock->target == NULL || dock->targetvnum == 0)
	{
		send_to_char(
				FB_RED "Ten r�kaw cumowniczy nie jest z niczym po��czony." NL
				"   Poszukaj innego." EOL, ch);
		return;
	}
	if (dock->status == 0)
	{
		send_to_char(FB_RED "Ten r�kaw cumowniczy jest zabezpieczony." NL
		"   Poszukaj innego." EOL, ch);
		return;
	}
	if ((toroom = get_room_index(dock->targetvnum)) != NULL)
	{
		uncrew(ship_from_room(fromroom), ch);
		act( PLAIN, "$n opuszcza statek.", ch, NULL, NULL, TO_ROOM);
		act( PLAIN, "Przechodzisz ze statku na statek.", ch, NULL, NULL,
				TO_CHAR);
		char_from_room(ch);
		char_to_room(ch, toroom);
		act( PLAIN, "$n wchodzi na pok�ad statku.", ch, NULL, NULL, TO_ROOM);
		do_look(ch, (char*) "auto");
	}
	else
		send_to_char("Hmm.. Wyj�cie chyba nie dzia�a." EOL, ch);
}

bool is_linked(SHIP_DATA *ship, SHIP_DATA *target)
{
	if (target == NULL)
	{
		if (ship->shipstate == SHIP_DOCKED2)
			return true;
		if (ship->shipstate == SHIP_DOCKED2_2)
			return true;
		if (ship->shipstate == SHIP_DOCKED2_FIN)
			return true;
		return false;
	}
	if (ship == NULL)
	{
		for (auto* dock : target->docks)
			if (dock->target != NULL)
				return true;
		return false;
	}
	return false;
}

SHIPDOCK_DATA* shipdock_from_slave(SHIP_DATA *ship)
{
	for (auto* dock : ship->docks)
		if (dock->master_slave == 0)
			return dock;

	return NULL;
}

DEF_DO_FUN( crew )
{
	CREW_DATA *member;
	SHIP_DATA *ship, *tship;
	CHAR_DATA *tch;
	char arg1[MSL];

	strcpy(arg1, argument);

	if (IS_NPC(ch))
	{
		send_to_char(FB_RED "Narazie MOBy nie mog� nale�e� do za�ogi." NL, ch);
		return;
	}
	if ((ship = ship_from_room(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Wsi�d� najpierw na jaki� statek!" EOL, ch);
		return;
	}
	if (arg1[0] != '\0')
	{
		if (!check_pilot(ch, ship))
		{
			send_to_char(
					FB_RED "Hej, to nie tw�j statek!" NL "Kup sobie jaki� i wtedy zatrudniaj za�og�!" EOL,
					ch);
			return;
		}
		if ((tch = get_char_world(ch, arg1)) == NULL)
		{
			ch_printf(ch, "Nie ma nikogo takiego jak %s" NL, arg1);
			return;
		}
		if (IS_NPC(tch))
		{
			send_to_char(FB_RED "Narazie MOBy nie mog� nale�e� do za�ogi." NL,
					ch);
			return;
		}
		if ((tship = ship_from_room(tch->in_room)) != ship)
		{
			ch_printf(ch,
					FB_RED "Musisz najpierw zaprosic %s na pok�ad statku." EOL,
					PERS(tch, ch, 3));
			return;
		}
		if (!known_biotech(ship, tch))
		{
			ch_printf(ch,
					FB_RED "To jest wytw�r obcej biotechnologi," NL " niestety %s nie ma o niej zielonego poj�cia." EOL,
					PERS(tch, ch, 0));
			return;
		}
		if (check_crew(ship, tch, "all"))
		{
			ch_printf(ch, FB_RED "Ok! %s usuni�t%s z za�ogi." EOL, tch->name,
					SEX_SUFFIX_YAE(tch));
			uncrew(ship, tch);
			return;
		}
	}
	else
	{
		if (!check_pilot(ch, ship))
		{
			send_to_char( FB_RED "Hej, to nie tw�j statek!" NL
			"Wynajmij sobie jaki� !" EOL, ch);
			return;
		}
		if (!known_biotech(ship, ch))
		{
			ch_printf(ch,
					FB_RED "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." EOL);
			return;
		}
		if (check_crew(ship, ch, "all"))
		{
			if (IS_IMMORTAL(ch))
			{
				ch_printf(ch, FB_RED "Ok! Zwalniasz swoje miejsce." EOL);
				uncrew(ship, ch);
				return;
			}
			else
			{
				ch_printf(ch, FB_RED "Ju� stanowisz cz�� za�ogi." EOL);
				return;
			}
		}
		tch = ch;
	}
	CREATE(member, CREW_DATA, 1);
	member->name = tch;
	member->ship = ship;
	member->rank = 0;
	cmember_list.push_back(member);
	if (arg1[0] != '\0')
	{
		ch_printf(ch, FB_GREEN "%s dopisan%s do za�ogi." EOL, tch->name,
				SEX_SUFFIX_YAE(tch));
		ch_printf(tch, FB_GREEN "%s dopisa�%s ci� do za�ogi." EOL, ch->name,
				SEX_SUFFIX__AO(ch));
	}
	else
	{
		ch_printf(ch, FB_GREEN "Dopisa�%s� si� do za�ogi." EOL,
				SEX_SUFFIX_EAO(tch));
	}
	return;
}

void uncrew(SHIP_DATA *ship, CHAR_DATA *ch)
{
	bool found;

	if (IS_NPC(ch))
		return;
	found = false;
	for (auto* member : cmember_list)
	{
		if (member->ship == ship)
			if (member->name == ch)
			{
				send_to_char(FB_RED "Przestajesz by� cz�ci� za�ogi" EOL, ch);
				cmember_list.remove(member);
				member->ship = NULL;
				member->name = NULL;
				DISPOSE(member);
				found = true;
				break;
			}
	}
	if (found)
		uncrew(ship, ch);
	return;
}

bool are_crew_onboard(SHIP_DATA *ship)
{
	for (auto* member : cmember_list)
		if (member->ship == ship)
			return true;
	return false;
}

void star_update(void)
{
//    MOON_DATA *moon;
//    STAR_DATA *star;
	float x, y, z, r, step, old_x, old_y, old_z/*, circumference*/;
	int yearlen_t, monthlen_t, yearlen_m;

	update_crew();
	for (auto* ship : ship_list)
	{
		if (!ship->starsystem)
			continue;
		if (!are_crew_onboard(ship) && ship->shipstate != SHIP_DOCKED2_FIN)
		{
			ship->currspeed = 0;
			for (auto* dock : ship->docks)
				if (dock->master_slave == 1)
					dock->target->currspeed = 0;
		}
	}
	for (auto* planet : planet_list)
	{
		yearlen_m = 0;
		monthlen_t = planet->monthlen * (planet->daylen + planet->nightlen);
		for (auto* season : planet->seasons)
			yearlen_m += season->length;
		yearlen_t = yearlen_m * monthlen_t;
		x = planet->xpos;
		y = planet->ypos;
		z = planet->zpos;

		old_x = x;
		old_y = y;
		old_z = z;

		r = planet->star_radius;
//	circumference = 2*M_PI*r;
		step = 4 * r / yearlen_t; //circumference/yearlen_t;
		if (step < 1)
			step = 1;
//	if ( !str_cmp( planet->name, "Coruscant" ) )
//	   bug("%-12s x%-6.0f y%-6.0f z%-5.0f r%-6.0f st%-3.0f y%d",planet->name,x,y,z,r,step,yearlen_t);
//  nie wyrzucac zrobie z tego kanal na podobienstwo tego CHANNEL_PROG
		if (x >= 0 && y > 0)
		{
			x += step;
			y = sqrt((r * r) - (x * x));
		}
		else if (x > 0 && y <= 0)
		{
			x -= step;
			y = 0 - sqrt((r * r) - (x * x));
		}
		else if (x <= 0 && y < 0)
		{
			x -= step;
			y = 0 - sqrt((r * r) - (x * x));
		}
		else if (x < 0 && y >= 0)
		{
			x += step;
			y = sqrt((r * r) - (x * x));
		}
		if (x > r)
		{
			x = r;
			y = 0;
		}
		if (x < 0 - r)
		{
			x = 0 - r;
			y = 0;
		}
		planet->xpos = x;
		planet->ypos = y;

		for (auto* ship : ship_list)
		{
			if (srange((ship->vx - x), (ship->vy - y), (ship->vz - z))
					< planet->radius + planet->gravity)
			{
				ship->vx += x - old_x;
				ship->vy += y - old_y;
				ship->vz += z - old_z;

				ship->hx += x - old_x;
				ship->hy += y - old_y;
				ship->hz += z - old_z;
			}
		}
	}
}

void update_crew(void)
{
	bool found;

	for (auto* member : cmember_list)
	{
		found = false;
		for (auto* ch : char_list)
		{
			if (IS_NPC(ch))
				continue;
			if (ch->desc == NULL)
				continue;
			if (ch->desc->connected < CON_PLAYING)
				continue;
			if (ch == member->name)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			cmember_list.remove(member);
			member->ship = NULL;
			member->name = NULL;
			DISPOSE(member);
			update_crew();
			return;
		}
	}
}

void setallships(CHAR_DATA *ch, char *arg2, char *argument)
{
	if (!str_cmp(arg2, "manuever"))
	{
		for (auto* ship : ship_list)
		{
			ship->manuever = URANGE(0, atoi(argument), 120);
		}
		send_to_char("All ships done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "speed"))
	{
		for (auto* ship : ship_list)
		{
			ship->realspeed = URANGE(0, atoi(argument), 500);
		}
		send_to_char("All ships done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "hyperspeed"))
	{
		for (auto* ship : ship_list)
		{
			ship->hyperspeed = URANGE(0, atoi(argument), 500);
		}
		send_to_char("All ships done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "sensor"))
	{
		for (auto* ship : ship_list)
		{
			ship->sensor = URANGE(1, atoi(argument), 150);
			if (ship->target_array > ship->sensor)
				ship->target_array = ship->sensor;
			if (ship->astro_array > ship->sensor)
				ship->astro_array = ship->sensor;
		}
		send_to_char("All ships done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "targetarray"))
	{
		for (auto* ship : ship_list)
		{
			ship->target_array = URANGE(0, atoi(argument), 150);
			if (ship->target_array > ship->sensor)
				ship->target_array = ship->sensor;
		}
		send_to_char("All ships done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "astroarray"))
	{
		for (auto* ship : ship_list)
		{
			ship->astro_array = URANGE(1, atoi(argument), 150);
			if (ship->astro_array > ship->sensor)
				ship->astro_array = ship->sensor;
		}
		send_to_char("All ships done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "comm"))
	{
		for (auto* ship : ship_list)
		{
			ship->comm = URANGE(1, atoi(argument), 100);
		}
		send_to_char("All ships done." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "conversion"))
	{
		for (auto* ship : ship_list)
		{
			if (ship->version < 1)
			{
				if (ship->clazz < 3)
				{
					ship->clazz = FIGHTER_SHIP;
					ship->version = 1;
					continue;
				}
				if (ship->clazz < 11)
				{
					ship->clazz = MIDSHIP_SHIP;
					ship->version = 1;
					continue;
				}
				if (ship->clazz < 19)
				{
					ship->clazz = CAPITAL_SHIP;
					ship->version = 1;
					continue;
				}
				if (ship->clazz < 22)
				{
					ship->clazz = PLATFORM_SHIP;
					ship->version = 1;
					continue;
				}
				if (ship->clazz >= 22)
				{
					ship->clazz = NO_SPACE_SHIP;
					ship->version = 1;
					continue;
				}
			}
		}
		send_to_char("All ships done." NL, ch);
		return;
	}

	return;
}

bool is_linked_with(SHIP_DATA *ship, SHIP_DATA *target)
{
	for (auto* dock : ship->docks)
		if (dock->target == target)
			return true;
	return false;
}

int get_space_chance(SHIP_DATA *ship, CHAR_DATA *ch)
{
	int chance = 0;

	if (is_fighter(ship))
		chance =
				IS_NPC(ch) ?
						ch->top_level :
						(int) (ch->pcdata->learned[gsn_starfighters]);
	if (is_scout(ship))
		chance =
				IS_NPC(ch) ?
						ch->top_level :
						(int) (ch->pcdata->learned[gsn_scoutships]);
	if (is_freighter(ship))
		chance =
				IS_NPC(ch) ?
						ch->top_level :
						(int) (ch->pcdata->learned[gsn_freighters]);
	if (is_huge(ship))
		chance =
				IS_NPC(ch) ?
						ch->top_level :
						(int) (ch->pcdata->learned[gsn_hugeships]);
	if (is_midship(ship))
		chance =
				IS_NPC(ch) ?
						ch->top_level :
						(int) (ch->pcdata->learned[gsn_midships]);
	if (is_capital(ship))
	{
		if ((int) (ch->pcdata->learned[gsn_smugglersworld])
				>= (int) (ch->pcdata->learned[gsn_capitalships]))
		{
			chance =
					IS_NPC(ch) ?
							ch->top_level :
							(int) (ch->pcdata->learned[gsn_smugglersworld]);
		}
		else
		{
			chance =
					IS_NPC(ch) ?
							ch->top_level :
							(int) (ch->pcdata->learned[gsn_capitalships]);
		}
	}
	if (is_platform(ship))
		chance =
				IS_NPC(ch) ?
						ch->top_level :
						(int) (ch->pcdata->learned[gsn_platforms]);

	return chance;
}

void learn_from_failure_space(SHIP_DATA *ship, CHAR_DATA *ch)
{
	if (is_scout(ship))
		learn_from_failure(ch, gsn_scoutships);
	if (is_fighter(ship))
		learn_from_failure(ch, gsn_starfighters);
	if (is_midship(ship))
		learn_from_failure(ch, gsn_midships);
	if (is_capital(ship))
	{
		if ((int) (ch->pcdata->learned[gsn_smugglersworld])
				>= (int) (ch->pcdata->learned[gsn_capitalships]))
			learn_from_failure(ch, gsn_smugglersworld);
		else
			learn_from_failure(ch, gsn_capitalships);
	}
	if (is_freighter(ship))
		learn_from_failure(ch, gsn_freighters);
	if (is_huge(ship))
		learn_from_failure(ch, gsn_hugeships);
	if (is_platform(ship))
		learn_from_failure(ch, gsn_platforms);
	return;
}

void learn_from_success_space(SHIP_DATA *ship, CHAR_DATA *ch)
{
	if (is_scout(ship))
		learn_from_success(ch, gsn_scoutships);
	if (is_fighter(ship))
		learn_from_success(ch, gsn_starfighters);
	if (is_midship(ship))
		learn_from_success(ch, gsn_midships);
	if (is_capital(ship))
	{
		if ((int) (ch->pcdata->learned[gsn_smugglersworld])
				>= (int) (ch->pcdata->learned[gsn_capitalships]))
			learn_from_success(ch, gsn_smugglersworld);
		else
			learn_from_success(ch, gsn_capitalships);
	}
	if (is_freighter(ship))
		learn_from_success(ch, gsn_freighters);
	if (is_huge(ship))
		learn_from_success(ch, gsn_hugeships);
	if (is_platform(ship))
		learn_from_success(ch, gsn_platforms);
	return;
}

bool known_biotech(SHIP_DATA *ship, CHAR_DATA *ch)
{
	int chance;

	if (ship->type != SHIP_YUUZHAN)
		return true;
	if (IS_RACE(ch, "Yuuzhan Vong"))
		return true;
	if (IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;
	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_yuuzhan_biotech]);
	if (chance < 95)
		learn_from_success(ch, gsn_yuuzhan_biotech);
	if (number_percent() < chance)
		return true;
	return false;
}

DEF_DO_FUN( distres )
{
	long price = 0;
	int cap;
	int chance;
	SHIP_DATA *ship;
	char arg[MAX_INPUT_LENGTH];

	switch (ch->substate)
	{
	default:
		if (!get_comlink(ch))
		{
			send_to_char(
					FB_RED "Potrzebujesz urz�dzenia komunikacyjnego by to zrobi�!" EOL,
					ch);
			return;
		}
		if (argument[0] != '\0' && !is_number(argument))
		{
			send_to_char(FB_RED "sk�adnia: distres" NL
			"        : distres <ilo�� energii>" EOL, ch);
			return;
		}
		if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
		{
			send_to_char(
					FB_RED "Musisz by� w kokpicie statku by to zrobi�!" EOL,
					ch);
			return;
		}
		if (isnt_in_realspace(ship))
		{
			send_to_char(
					FB_RED "W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL,
					ch);
			return;
		}
		if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
		{
			ch_printf(ch,
					FB_RED "Nie wygl�da na to, �eby� by�%s w kabinie pilota!" EOL,
					SEX_SUFFIX__AO(ch));
			return;
		}
		if (!check_crew(ship, ch, "pilot"))
		{
			send_to_char(
					FB_RED "Nie zajmujesz stanowiska pilota na tym statku." EOL,
					ch);
			return;
		}
		if (ship->clazz == ESCAPE_POD_SHIP)
		{
			send_to_char(
					FB_RED "Kapsu�y nie mo�na dotankowa� musisz wezwa� pomoc w inny spos�b!" NL,
					ch);
			return;
		}
		if (!autofly(ship))
		{
			send_to_char(
					FB_RED "Tankowanie w przestrzeni przeprowadza autopilot musisz go w��czy�." EOL,
					ch);
			return;
		}
		chance = get_space_chance(ship, ch);
		if (number_percent() < chance)
		{
			if (argument[0] == '\0' || !is_number(argument))
				cap = ship->maxenergy - ship->energy;
			else
				cap = atoi(argument);
			if (cap == 0)
			{
				send_to_char(
						FB_RED "Nie zawracaj g�owy ekipom ratowniczym!" EOL,
						ch);
				return;
			}
		}
		else
		{
			send_to_char(
					FB_RED "Nie uda�o ci si� przekaza� swoich wsp�rz�dnych." NL,
					ch);
			learn_from_failure_space(ship, ch);
			return;

		}
		send_to_char(
				FB_GREEN "��czysz si� z za�og� tankowca ratunkowego..." EOL,
				ch);
		act( PLAIN, "$n manipuluje komputerem pok�adowym.", ch, NULL, NULL,
				TO_ROOM);
		add_timer(ch, TIMER_DO_FUN, 7, do_distres, 1);
		sprintf(arg, "%d", cap);
		STRDUP(ch->dest_buf, arg);
		return;

	case 1:
		if (!*ch->dest_buf)
			return;
		strcpy(arg, ch->dest_buf);
		STRDUP(ch->dest_buf, "");
		break;

	case SUB_TIMER_DO_ABORT:
		break_skill( FB_RED "Przerywasz po��czenie..." EOL, ch);
		return;
	}
	ch->substate = SUB_NONE;

	if (!get_comlink(ch))
	{
		send_to_char(
				FB_RED "Potrzebujesz urz�dzenia komunikacyjnego by to zrobi�!" EOL,
				ch);
		return;
	}
	if (arg[0] != '\0' && !is_number(arg))
	{
		send_to_char(
				FB_RED "sk�adnia: distres" NL "        : distres <ilo�� energii>" EOL,
				ch);
		return;
	}
	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�!" EOL,
				ch);
		return;
	}
	if (isnt_in_realspace(ship))
	{
		send_to_char(
				FB_RED "W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL,
				ch);
		return;
	}
	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		ch_printf(ch,
				FB_RED "Nie wygl�da na to, �eby� by�%s w kabinie pilota!" EOL,
				SEX_SUFFIX__AO(ch));
		return;
	}
	if (!check_crew(ship, ch, "pilot"))
	{
		send_to_char(
				FB_RED "Nie zajmujesz stanowiska pilota na tym statku." EOL,
				ch);
		return;
	}
	if (ship->clazz == ESCAPE_POD_SHIP)
	{
		send_to_char(
				FB_RED "Kapsu�y nie mo�na dotankowa� musisz wezwa� pomoc w inny spos�b!" NL,
				ch);
		return;
	}
	if (!autofly(ship))
	{
		send_to_char(
				FB_RED "Tankowanie w przestrzeni przeprowadza autopilot musisz go w��czy�." EOL,
				ch);
		return;
	}
	if (arg[0] == '\0' || !is_number(arg))
		cap = ship->maxenergy - ship->energy;
	else
		cap = atoi(arg);
	if (cap == 0)
	{
		send_to_char(FB_RED "Nie zawracaj g�owy ekipom ratowniczym!" EOL, ch);
		return;
	}
	price = cap;
	if (ch->pcdata && ch->pcdata->clan
			&& !str_cmp(ch->pcdata->clan->name, ship->owner))
	{
		if (ch->pcdata->clan->funds < price)
		{
			ch_printf(ch,
					FB_RED "%s nie ma wystarczaj�cej ilo�ci funduszy, by zap�aci� za energi�." EOL,
					ch->pcdata->clan->name);
			return;
		}

		ch->pcdata->clan->funds -= price;
		ch_printf(ch,
				FB_GREEN "Zatankowanie tego statku b�dzie kosztowa�o %s %ld kredytek." EOL,
				ch->pcdata->clan->name, price);
	}
	else
	{
		if (ch->gold < price)
		{
			ch_printf(ch,
					FB_RED "Nie masz wystarczaj�cych funduszy, by zap�aci� za energi�." EOL);
			return;
		}

		ch->gold -= price;
		ch_printf(ch,
				FB_GREEN "P�acisz %ld kredytek za zatankowanie statku" EOL,
				price);
	}
	learn_from_success_space(ship, ch);
	ship->energy += cap;
	if (ship->energy > ship->maxenergy)
		ship->energy = ship->maxenergy;
	return;

}
/****88888888**********************/
DEF_DO_FUN( openshipdock )
{
	SHIP_DATA *ship, *target;
	SHIPDOCK_DATA *dock, *targetdock;
	ROOM_INDEX_DATA *room;

	if (!str_cmp(argument, "dock") || !str_cmp(argument, "�luza")
			|| !str_cmp(argument, "�luz�"))
	{
		ship = ship_from_room(ch->in_room);
		if (ship == NULL)
		{
			send_to_char( FB_RED "Musisz by� na statku." EOL, ch);
			return;
		}
		if ((dock = shipdock_from_room(ship, ch->in_room->vnum)) == NULL)
		{
			send_to_char( FB_RED "Musisz sta� w pomieszczeniu ze �luz�." EOL,
					ch);
			return;
		}
		if (dock->status == 0)
		{
			if ((target = dock->target) != NULL)
			{
				if ((targetdock = shipdock_from_room(target, dock->targetvnum))
						== NULL)
				{
					bug("DO_OPENSHIPDOCK:/targetvnum/ Sh:%s, Dv:%d, T:%s",
							SHIPNAME(ship), dock->vnum, SHIPNAME(target));
					send_to_char(
							FB_RED "Bzyk Bzyk.... cos sie popsu�o" NL "Zg�osi�em ju� ten b��d." EOL,
							ch);
					return;
				}
				if (!known_biotech(ship, ch) || !known_biotech(target, ch))
				{
					ch_printf(ch,
							FB_RED "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." EOL);
					return;
				}
				if (!check_pilot(ch, ship) || !check_pilot(ch, target))
				{
					send_to_char(FB_RED "Hej! To nie tw�j statek!" EOL, ch);
					return;
				}
				echo_to_room(get_room_index(dock->vnum),
						FB_YELLOW "�luza r�kawa cumowniczego otwiera si�.");
				echo_to_room(get_room_index(targetdock->vnum),
						FB_YELLOW "�luza r�kawa cumowniczego otwiera si�.");
				sound_to_room(get_room_index(dock->vnum), "!!SOUND(door)");
				sound_to_room(get_room_index(targetdock->vnum),
						"!!SOUND(door)");
				dock->status = 1;
				targetdock->status = 1;
			}
			else
			{
				if (ship->starsystem == NULL || ship->shipstate == SHIP_DOCKED
						|| ship->shipstate == SHIP_HYPERSPACE)
				{
					ch_printf(ch,
							FB_RED "Obecny stan statku nie pozwala na otwieranie �luz." EOL);
					return;
				}
				if (!known_biotech(ship, ch))
				{
					ch_printf(ch,
							FB_RED "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." EOL);
					return;
				}
				echo_to_room(get_room_index(dock->vnum),
						FB_YELLOW "�luza r�kawa cumowniczego otwiera si�."
						EOL FB_YELLOW "Ukazuj�c czarn� przestrze�."
						EOL FB_WHITE "Ale zanim jeszcze nabierzesz powietrza by krzykn��..."
						EOL FB_WHITE "Krew wrze ci w �y�ach a cia�o zastyga zmro�one kosmiczn� pustk�.");
				sound_to_room(get_room_index(dock->vnum), "!!SOUND(door)");
				dock->status = 1;
				room = ch->in_room;
				auto people_snapshot = room->people;
				for (auto* rch : people_snapshot)
				{
					if ( IS_IMMORTAL(rch) || IS_RACE(rch, "DUINUOGWUIN"))
					{
						char_from_room(rch);
						char_to_room(rch, get_room_index(wherehome(rch)));
					}
					else
					{
						if (ch)
							raw_kill(ch, rch, 0);
						else
							raw_kill(rch, rch, 0);
					}
				}
			}
		}
		else if (dock->status > 1)
		{
			send_to_char( FB_RED "�luza jest wy�amana." EOL, ch);
			return;
		}
		else
		{
			send_to_char( FB_RED "�luza jest ju� otwarta" EOL, ch);
			return;
		}
	}
	else
		send_to_char( FB_RED "Sk�adnia: open �luz�" NL
		"               �luza" NL
		"               dock" EOL, ch);
	return;
}

DEF_DO_FUN( closeshipdock )
{
	SHIP_DATA *ship, *target;
	SHIPDOCK_DATA *dock, *targetdock;

	if (!str_cmp(argument, "dock") || !str_cmp(argument, "�luza")
			|| !str_cmp(argument, "�luz�"))
	{
		ship = ship_from_room(ch->in_room);
		if (ship == NULL)
		{
			send_to_char( FB_RED "Musisz by� na statku." EOL, ch);
			return;
		}
		if ((dock = shipdock_from_room(ship, ch->in_room->vnum)) == NULL)
		{
			send_to_char( FB_RED "Musisz sta� w pomieszczeniu ze �luz�." EOL,
					ch);
			return;
		}
		if (dock->status == 1)
		{
			if ((target = dock->target) != NULL)
			{
				if ((targetdock = shipdock_from_room(target, dock->targetvnum))
						== NULL)
				{
					bug("DO_OPENSHIPDOCK:/targetvnum/ Sh:%s, Dv:%d, T:%s",
							SHIPNAME(ship), dock->vnum, SHIPNAME(target));
					send_to_char( FB_RED "Bzyk Bzyk.... cos sie popsu�o" NL
					"Zg�osi�em ju� ten b��d." EOL, ch);
					return;
				}
				if (!known_biotech(ship, ch) || !known_biotech(target, ch))
				{
					ch_printf(ch,
							FB_RED "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." EOL);
					return;
				}
				if (!check_pilot(ch, ship) || !check_pilot(ch, target))
				{
					send_to_char(FB_RED "Hej! To nie tw�j statek!" EOL, ch);
					return;
				}
				echo_to_room(get_room_index(dock->vnum),
						FB_YELLOW "�luza r�kawa cumowniczego zamyka si�.");
				echo_to_room(get_room_index(targetdock->vnum),
						FB_YELLOW "�luza r�kawa cumowniczego zamyka si�.");
				sound_to_room(get_room_index(dock->vnum), "!!SOUND(door)");
				sound_to_room(get_room_index(targetdock->vnum),
						"!!SOUND(door)");
				dock->status = 0;
				targetdock->status = 0;
			}
			else
			{
				if (ship->starsystem == NULL || ship->shipstate == SHIP_DOCKED
						|| ship->shipstate == SHIP_HYPERSPACE)
				{
					ch_printf(ch,
							FB_RED "Obecny stan statku nie pozwala na otwieranie �luz." EOL);
					return;
				}
				if (!known_biotech(ship, ch))
				{
					ch_printf(ch,
							FB_RED "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." EOL);
					return;
				}
				echo_to_room(get_room_index(dock->vnum),
						FB_YELLOW "�luza r�kawa cumowniczego zamyka si�.");
				sound_to_room(get_room_index(dock->vnum), "!!SOUND(door)");
				dock->status = 0;
			}
		}
		else if (dock->status > 1)
		{
			send_to_char(
					FB_RED "�luza jest wy�amana, nie mo�na jej zamkn��." EOL,
					ch);
			return;
		}
		else
		{
			send_to_char( FB_RED "�luza jest ju� zamkni�ta." EOL, ch);
			return;
		}
	}
	else
		send_to_char(
				FB_RED "Sk�adnia: close �luz�" NL "                �luza" NL "                dock" EOL,
				ch);
	return;
}

void yuuzhan_info(SHIP_DATA *ship, CHAR_DATA *ch)
{
	SHIP_DATA *target = ship;

	if (known_biotech(ship, ch))
	{
		{
			char _buf[MSL];

			strcpy(_buf, bit_name(ship_classes_name_list, target->clazz));

			pager_printf(ch,
					"Patrzysz na " FB_WHITE "Yuuzha�ski" PLAIN " statek klasy:" FB_WHITE " %s" EOL
					"o nazwie :" FB_WHITE " %s" EOL "Nazwa fabryczna:" FB_WHITE " %s" EOL,
					_buf, SHIPNAME(target), target->name);
		}

		pager_printf(ch, "Klasa skilla: %s" EOL,
				is_scout(target) ?
						"initiate" :
						(is_fighter(target) ?
								"jumper" :
								(is_midship(target) ?
										"slave" :
										(is_freighter(target) ?
												"transport" :
												(is_capital(target) ?
														"destructor" :
														(is_huge(target) ?
																"light" :
																(is_platform(
																		target) ?
																		"world" :
																		"Hmm jaki� b��d albo speeder")))))));

		if (*target->description)/* I tutaj ja -- Than */
			pager_printf(ch, "%s" EOL, target->description);
		pager_printf(ch, "Wyrzutnie Plazmy:    %d    Wyrzutnie Jon�w:    %d" NL,
				target->lasers, target->ioncannons);
		pager_printf(ch, "Max. Ilo�� pocisk�w: %d / %d / %d   %s" NL,
				target->maxmissiles, target->torpedos, target->rockets,
				target->trawler == 0 ? "" : "Neutralizator");
		pager_printf(ch, "Max. Ilo�� Kontur:     %d" NL, target->maxchaff);
		pager_printf(ch, "Max. Wytrzyma�o�� Skorupy: %d" NL, target->maxhull);
		pager_printf(ch,
				"Max. Moc Bassali: %d   Max. Energia(biopaliwo): %d" NL,
				target->maxshield, target->maxenergy);
		pager_printf(ch,
				"Max. Pr�dko��:   %.0f    Hiperpr�dko��:        %.0f" NL,
				target->realspeed, target->hyperspeed);
		pager_printf(ch, "Zakres wykrycia:   %d  Zakres rozpoznania:     %d" NL,
				target->sensor, target->target_array);
		pager_printf(ch, "�adowno�� %d" NL, target->maxcargo);
		return;
	}
	else
	{
		pager_printf(ch,
				NL "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." NL);
		return;
	}
}
DEF_DO_FUN( eject )
{
	SHIP_INDEX_DATA *shrec;
	SHIP_DATA *ship;
	SHIP_DATA *target;
	SPACE_DATA *starsystem;
	int plusminus;
	char buf[MAX_STRING_LENGTH];
	CREW_DATA *member;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(
				FB_RED "Musisz by� na mostku lub w wie�yczce by to zrobi�!" NL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(
				FB_RED "W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL,
				ch);
		return;
	}

	if (!known_biotech(ship, ch))
	{
		ch_printf(ch,
				FB_RED "To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia." EOL);
		return;
	}

	if (ship->clazz == ESCAPE_POD_SHIP)
	{
		send_to_char(FB_RED "Kapsu�y nie maj� kapsu�!" NL, ch);
		return;
	}
	shrec = get_ship_index(itoa(DEFAULT_ESCAPE_POD));
	starsystem = ship->starsystem;

	sprintf(buf, FG_YELLOW "Kapsu�a ratunkowa oddziela si� od %s!",
			ship->sslook);
	echo_to_system(ship, buf, NULL);

	if (!(target = constr_ship(shrec)))
		return;

	target->location = NULL;
	target->lastdoc = ship->location;
	target->autopilot = false;
	target->autorecharge = true;

	extract_ship(target);
	ship_to_starsystem(target, starsystem);
	STRDUP(target->home, starsystem->name);

	plusminus = number_range(-1, 2);
	if (plusminus > 0)
	{
		target->hx = 1;
		target->vx = ship->vx + ship->size + 50;
	}
	else
	{
		target->hx = -1;
		target->vx = ship->vx - ship->size - 50;
	}
	plusminus = number_range(-1, 2);
	if (plusminus > 0)
	{
		target->hy = 1;
		target->vy = ship->vy + ship->size + 50;
	}
	else
	{
		target->hy = -1;
		target->vy = ship->vy - ship->size - 50;
	}
	plusminus = number_range(-1, 2);
	if (plusminus > 0)
	{
		target->hz = 1;
		target->vz = ship->vz + ship->size + 50;
	}
	else
	{
		target->hz = -1;
		target->vz = ship->vz - ship->size - 50;
	}

	if (target->vXpos != 0 || target->vYpos != 0)
	{
		target->vXpos = 0;
		target->vYpos = 0;
		STRDUP(target->sXpos, "");
		STRDUP(target->sYpos, "");
	}
	target->currjump = NULL;
	target->jx = 0;
	target->jy = 0;
	target->jz = 0;
	target->hx = 1;
	target->hy = 1;
	target->hz = 1;
	target->type = ship->type;
	STRDUP(target->owner, "Public");
	target->currspeed = target->realspeed + 50;

	target->statet0 = LASER_READY;
	target->missilestate = MISSILE_READY;
	target->shipstate = SHIP_READY;

	generate_sslook(target);

	if (!*target->transponder)
		generate_transponder(target);

	reset_ship_rooms(target);

	if (is_scout(ship) || is_fighter(ship))
	{
		act( PLAIN, "$n uruchamia katapult�.", ch, NULL, argument, TO_ROOM);
		for (auto* room : ship->locations)
		{
			if (room != NULL)
			{
				auto people_snapshot = room->people;
				for (auto* rch : people_snapshot)
				{
					uncrew(ship, rch);
					act( FB_RED,
							"Czujesz potworne przeci��enie gdy kapsu�a od��cza si� od statku.",
							rch, NULL, argument, TO_CHAR);
					char_from_room(rch);
					char_to_room(rch, target->cockpit);
				}
			}
		}
		destroy_ship(ship, ch);
	}
	else
	{
		uncrew(ship, ch);
		act( PLAIN, "$n ewakuuje si�.", ch, NULL, argument, TO_ROOM);
		act( PLAIN, "Uruchamiasz silniki kapsu�y.", ch, NULL, argument,
				TO_CHAR);
		char_from_room(ch);
		char_to_room(ch, target->cockpit);
	}
	CREATE(member, CREW_DATA, 1);
	member->name = ch;
	member->ship = target;
	member->rank = 0;
	cmember_list.push_back(member);

//	bug("EJECTING %s sslook %s", target->transponder, target->sslook);
	return;
}

bool is_aggressive_to(SHIP_DATA *ship, SHIP_DATA *target)
{

	if (ship->clazz == ESCAPE_POD_SHIP || target->clazz == ESCAPE_POD_SHIP)
		return false;
	if (ship->type == SHIP_PIRATE && target->type == SHIP_PIRATE)
		return false;
	if (ship->type == SHIP_PIRATE && target->type == SHIP_YUUZHAN)
		return false;
	if (ship->starsystem != target->starsystem)
		return false;
	if (srange((ship->vx - target->vx), (ship->vy - target->vy),
			(ship->vz - target->vz))
			> target->size / 10 + 2000 + ship->target_array * 500)
		return false;
	if ( IS_SET( ship->flags , SHIP_AGGRESSIVE) && target->type == MOB_SHIP)
		return false;
	if (!str_cmp(ship->owner, "Imperium"))
		if (!str_cmp(target->owner, "Nowa Republika"))
			return true;
	if (!str_cmp(ship->owner, "Nowa Republika"))
		if (!str_cmp(target->owner, "Imperium"))
			return true;
	if (!str_cmp(ship->owner, "Sith"))
		if (!str_cmp(target->owner, "Jedi"))
			return true;
	if (!str_cmp(ship->owner, "Jedi"))
		if (!str_cmp(target->owner, "Sith"))
			return true;
	return false;
}

bool is_metaaggressive_to(SHIP_DATA *ship, SHIP_DATA *target)
{

//	METAAGGRESSIVE atakuje takze bezbronnych
//	if ( ship->clazz == ESCAPE_POD_SHIP || target->clazz == ESCAPE_POD_SHIP )
//		return false;

//  METAAGGRESSIVE to nie interesuje
//	if ( ship->type == SHIP_PIRATE && target->type == SHIP_PIRATE )
//		return false;
//	if ( ship->type == SHIP_PIRATE && target->type == SHIP_YUUZHAN )
//		return false;

	if (ship->starsystem != target->starsystem)
		return false;
	if (srange((ship->vx - target->vx), (ship->vy - target->vy),
			(ship->vz - target->vz))
			> target->size / 10 + 2000 + ship->target_array * 500)
		return false;

//  METAAGGRESSIVE nie atakuje swoich
	if (!str_cmp(ship->owner, target->owner))
		return false;

// METAAGGRESSIVE atakuje takze mobshipy jezeli nie maja wspolnych wlascicieli	
//	if ( IS_SET( ship->flags , SHIP_AGGRESSIVE) && target->type == MOB_SHIP )
//		return false;

	return true;
}

bool isnt_in_realspace(SHIP_DATA *ship)
{
	if (!is_ship(ship))
		return true;
	if (ship->shipstate == SHIP_HYPERSPACE)
		return true;
	if (ship->shipstate1 != 0)
		if (!is_ship(ship))
			return true;
	if (ship->shipstate == SHIP_HYPERSPACE)
		return true;
	if (ship->shipstate1 != 0)
		return true;
	if (ship->shipstate == SHIP_DISABLED)
		return true;
	if (ship->shipstate == SHIP_DOCKED)
		return true;
	if (is_linked(ship, NULL))
		return true;
	if (ship->starsystem == NULL)
		return true;

	return false;
}
