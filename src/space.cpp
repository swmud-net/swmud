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
 *								Space Module			    			   *
 *							Tutaj rzadzi Pixel							   *
 ****************************************************************************/

#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"
#include <iterator>

std::list<SHIP_DATA*> ship_list;

std::list<MISSILE_DATA*> missile_list;

std::list<SPACE_DATA*> starsystem_list;

std::list<HANGAR_DATA*> hangar_list;

std::list<TURRET_DATA*> turret_list;

std::list<MODULE_DATA*> module_list;

std::list<TURBOCAR*> turbocar_list;

std::list<CREW_DATA*> cmember_list;

std::list<ASTRO_DATA*> astro_list;

char *pubshipname[MAX_SHIP];	//added by Thanos

int turbocar_stop = 0;
int corus_shuttle = 0;

#define STOP_PLANET	202
#define STOP_SHIPYARD	32015

#define SENATEPAD	10196
#define OUTERPAD	10195

// Obrazenia od laserow, wiezyczek laserowych etc.
#define MIN_LASER_DAMAGE 8
#define MAX_LASER_DAMAGE 30 //80

// Obrazenia wiezyczek turbolaserowych
#define MIN_TURBO_DAMAGE 35
#define MAX_TURBO_DAMAGE 100 //350

// Obrazenia ciezkich wiezyczek turbolaserowych
#define MIN_HTURBO_DAMAGE 45
#define MAX_HTURBO_DAMAGE 150 //450

// Pixel: Obrazenia od pociskow	
#define MIN_MISSILE_DAMAGE							\
missile->missiletype == CONCUSSION_MISSILE ? 16 :	\
missile->missiletype == PROTON_TORPEDO ? 18 :		\
missile->missiletype == HEAVY_BOMB ? 20 : 19

#define MAX_MISSILE_DAMAGE							\
missile->missiletype == CONCUSSION_MISSILE ? 75 :	\
missile->missiletype == PROTON_TORPEDO ? 100 :		\
missile->missiletype == HEAVY_BOMB ? 200 : 140

/* local routines */
void fread_ship args((SHIP_DATA * ship, FILE * fp));
bool load_ship_file args((const char *shipfile));
void write_ship_list args((void));
void fread_starsystem args((SPACE_DATA * starsystem, FILE * fp));
bool load_starsystem args((char *starsystemfile));
void write_starsystem_list args((void));
void resetship args((SHIP_DATA * ship));
void landship args((SHIP_DATA * ship, char *arg));
void launchship args((SHIP_DATA * ship));
bool land_bus args((SHIP_DATA * ship, int destination));
void launch_bus args((SHIP_DATA * ship));
ch_ret drive_ship
		args((CHAR_DATA * ch, SHIP_DATA * ship, EXIT_DATA * exit, int fall))
		;
bool autofly args((SHIP_DATA * ship));
bool ship_in_closebay args((SHIP_DATA * ship));
bool is_facing args((SHIP_DATA * ship, SHIP_DATA * target));
void sound_to_ship args((SHIP_DATA * ship, char *argument));
void purge_ship args((SHIP_DATA * ship, CHAR_DATA * ch));

// added by Thanos: (space_reset.cpp)
void fread_space_resets args((SPACE_DATA * starsystem, FILE * fp));
void save_space_resets args((SPACE_DATA * starsystem, FILE * fp));

/* from comm.cpp */
bool write_to_descriptor args((int desc, char *txt, int length));

/* from space2.cpp */
bool has_map args((SPACE_DATA * starsystem, CHAR_DATA * ch));
void new_astro args((SHIP_DATA * ship, int astrotype));
void astro_atack args((SHIP_DATA * ship, ASTRO_DATA * astro));
void calculate_number
		args((CHAR_DATA * ch, char *arg_x, char *arg_y, SHIP_DATA * ship))
		;
void new_starsystem args((char *x, char *y));
bool same_star_pos
		args((float system, float system2, float arg, float arg2, int doklad, int doklad2))
		;
void damage_ship_energy
		args((SHIP_DATA * ship, int min, int max, CHAR_DATA * ch, SHIP_DATA * killer))
		;
void extract_starsystem args((SPACE_DATA * starsystem));
void dockship args((SHIP_DATA * ship, char *arg));
void undockship args((SHIP_DATA * ship));
bool is_aggressive_to args((SHIP_DATA * ship, SHIP_DATA * target));
bool isnt_in_realspace args((SHIP_DATA * ship));
bool is_turbocar args((int vnum));
//int     get_shipflag(char *flag ); //Dremor
bool is_metaaggressive_to args((SHIP_DATA * ship, SHIP_DATA * target));

/* shipconstr.c */
ROOM_INDEX_DATA* get_sroom(SHIP_DATA *ship, int svnum);

/* Zwraca wartosc bezwzgledna liczby
 float abs( float f )
 {
 return (float)abs((int)f);
 }
 */
bool land_bus(SHIP_DATA *ship, int destination)
{
	char buf[MAX_STRING_LENGTH];

	if (!ship_to_room(ship, get_room_index(destination)))
	{
		return false;
	}

	echo_to_ship(ship,
			FB_YELLOW "Czujesz delikatny wstrz�s. Statek osiada na l�dowisku.");

	ship->location = get_room_index(destination);
	ship->lastdoc = ship->location;
	ship->shipstate = SHIP_DOCKED;

	if (ship->starsystem)
		ship_from_starsystem(ship, ship->starsystem);

	sprintf(buf, FB_YELLOW "%s l�duje na platformie.", ship->name);
	echo_to_room(ship->location, buf);
	sprintf(buf, FB_YELLOW "%s otwiera klap�.", ship->name);
	echo_to_room(ship->location, buf);
	echo_to_room(ship->entrance, FB_YELLOW "Klapa otwiera si�.");

	ship->hatchopen = true;
	return true;
}

void launch_bus(SHIP_DATA *ship)
{
	char buf[MAX_STRING_LENGTH];

	sprintf(buf, FB_YELLOW "%s zamyka klap� i zaczyna startowa�.", ship->name);
	echo_to_room(ship->location, buf);
	echo_to_room(ship->entrance, FB_YELLOW "Klapa statku zamyka si�.");

	ship->hatchopen = false;

	extract_ship(ship);
	echo_to_ship(ship,
			FB_YELLOW "Statek zaczyna startowa�, s�yszysz jak silniki nabieraj� obrot�w.");

	ship->lastdoc = ship->location;
	ship->location = NULL;
	ship->shipstate = SHIP_READY;
}

bool is_turbocar(int vnum)
{
	for (auto* tc : turbocar_list)
		if (tc->vnum == vnum)
			return true;

	return false;
}

//TODO: Pixel - wypieprzyc promy, zastapic je busami
void update_traffic()
{
	//SHIP_DATA  *shuttle,
	//           *senate;
	SHIP_DATA *turbocar;
	RID *room;
	char buf[MSL];

	/*
	 shuttle = ship_from_cockpit(get_room_index(ROOM_CORUSCANT_SHUTTLE));
	 senate = ship_from_cockpit(get_room_index(ROOM_SENATE_SHUTTLE));
	 if(senate != NULL && shuttle != NULL)
	 {
	 switch (corus_shuttle)
	 {
	 default:
	 corus_shuttle++;
	 break;

	 case 0:
	 land_bus(shuttle, STOP_PLANET);
	 land_bus(senate, SENATEPAD);
	 corus_shuttle++;
	 echo_to_ship(shuttle, FG_CYAN "Witamy w porcie Menari.");
	 echo_to_ship(senate, FG_CYAN "Witamy w The Senate Halls.");
	 break;

	 case 4:
	 launch_bus(shuttle);
	 launch_bus(senate);
	 corus_shuttle++;
	 break;

	 case 5:
	 land_bus(shuttle, STOP_SHIPYARD);
	 land_bus(senate, OUTERPAD);
	 echo_to_ship(shuttle,
	 FG_CYAN "Welcome to Coruscant Shipyard.");
	 echo_to_ship(senate,
	 FG_CYAN
	 "Welcome to The Outer System Landing Area.");
	 corus_shuttle++;
	 break;

	 case 9:
	 launch_bus(shuttle);
	 launch_bus(senate);
	 corus_shuttle++;
	 break;

	 }

	 if(corus_shuttle >= 10)
	 corus_shuttle = 0;
	 }*/

	for (auto* tc : turbocar_list)
		if ((turbocar = ship_from_cockpit(get_room_index(tc->vnum))))
		{
			if (!tc->current_station)
			{
				if (tc->stations.empty())
					continue;
				tc->current_station = tc->stations.front();
			}
			sprintf(buf,
			FB_YELLOW
			"Drzwi %s zamykaj� si� i odje�d�a w po�piechu ze stacji.",
					tc->name);
			echo_to_room(turbocar->location, buf);
			extract_ship(turbocar);
			turbocar->location = NULL;
			room = get_room_index(tc->current_station->vnum);
			ship_to_room(turbocar, room);
			sprintf(buf,
			FB_YELLOW "%s pokonuje szybko tras� do nast�pnej stacji.",
					tc->name);
			echo_to_ship(turbocar, buf);
			turbocar->location = room;
			turbocar->lastdoc = room;
			turbocar->shipstate = SHIP_DOCKED;
			if (turbocar->starsystem)
				ship_from_starsystem(turbocar, turbocar->starsystem);
			sprintf(buf,
			FB_YELLOW
			"%s wje�dza na platform� i drzwi powoli si� otwieraj�.", tc->name);
			echo_to_room(turbocar->location, buf);
			sprintf(buf, FG_CYAN "Witamy w %s.", tc->current_station->name);
			echo_to_ship(turbocar, buf);
			turbocar->hatchopen = true;

			{
				auto st_it = std::find(tc->stations.begin(), tc->stations.end(), tc->current_station);
				if (st_it != tc->stations.end())
					++st_it;
				tc->current_station = (st_it != tc->stations.end()) ? *st_it : tc->stations.front();
			}
		}

}

void change_bus_planet(SHIP_DATA *bus)
{
	if (bus->stops.empty())
	{
		bug("Ship has no stops! Making it not public!");
		bus->ship_public = false;
		return;
	}

	if (!bus->curr_stop)
	{
		if (bus->stops.empty())
		{
			bug("Ship has no curr_stop and no stops! Disabling.");
			bus->ship_public = false;
			return;
		}
		bug("Ship has no curr_stop! Resetting course.");
		bus->curr_stop = bus->stops.front();
	}
	auto it = std::find(bus->stops.begin(), bus->stops.end(), bus->curr_stop);
	if (it != bus->stops.end())
		++it;
	bus->curr_stop = (it != bus->stops.end()) ? *it : bus->stops.front();
	return;
}

void update_bus()
{
	SHIP_DATA *target;
	int destination;
	char buf[MAX_STRING_LENGTH];

	for (auto* ship : ship_list)
	{
		if (ship->ship_public)
		{
			if (ship->stops.empty())
			{
				bug("Update bus: Ship: %s - is public but no course set.",
						ship->name);
				continue;
			}

			if (!ship->curr_stop)
				ship->curr_stop = ship->stops.front();

			switch (ship->bus_pos)
			{
			case 0: /* PREP_TO_LAND */
				target = ship_from_hangar(
						get_room_index(ship->curr_stop->stop_vnum));
				if (target != NULL && !target->starsystem)
				{
					sprintf(buf, FG_CYAN
					"Elektroniczny g�os nadaje 'Nie mo�emy wyl�dowa� na %s ..."
					" Prosz� o spok�j, ale to miejsce chyba znikn�o.'",
							ship->curr_stop->stop_name);
					echo_to_ship(ship, buf);
					bug("Update_bus: %s cannot land!", ship->name);
					ship->bus_pos = 5;
				}
				ship->bus_pos++;
				break;
			case 6: /* LAUNCH */
				launch_bus(ship);
				ship->bus_pos++;
				break;
			case 7: /* HYP_START */
				if (ship->public_subclass == 3)
				{
					echo_to_ship(ship,
							FB_YELLOW
							"Statek unosi si� ponad l�dowisko i skr�ca gwa�townie przyspieszaj�c.");
				}
				else if (ship->public_subclass == 2)
				{
					echo_to_ship(ship, FB_YELLOW
					"Czujesz wibracje ca�ego kad�uba" NL
					"Statek podkr�ca obroty silnik�w i oddala si� od portu.");
				}
				else
				{
					echo_to_ship(ship,
							FB_YELLOW
							"S�yszysz potworny huk!" NL
							"Statek podkr�ca obroty turbonap�d�w i robi skok w hiperprzestrze�.");
				}

				ship->bus_pos++;
				break;
			case 9: /* HYP_STOP */
				if (ship->public_subclass == 3)
				{
					echo_to_ship(ship,
							FB_YELLOW
							"Statek gwa�townie zwalnia i delikatnie podchodzi do l�dowania.");
				}
				else if (ship->public_subclass == 2)
				{
					echo_to_ship(ship, FB_YELLOW
					"Ponownie czujesz wibracje ca�ego kad�uba"
					NL
					"Statek gwa�townie zwalnia zbli�aj�c si� do portu.");
				}
				else
				{
					echo_to_ship(ship,
							FB_YELLOW
							"Co za ha�as!" NL
							"Czujesz jak statek gwa�townie zwalnia wychodz�c z hiperprzestrzeni.");
				}

				ship->bus_pos++;
				break;
			case 1: /*LAND */
				destination = ship->curr_stop->stop_vnum;
				if (!land_bus(ship, destination))
				{
					sprintf(buf, FG_CYAN
					"Elektroniczny g�os nadaje 'Ojej, %s chyba nie istnieje.'",
							ship->curr_stop->stop_name);
					echo_to_ship(ship, buf);
					echo_to_ship(ship,
							FG_CYAN
							"Elektroniczny g�os nadaje 'Mam nadziej�, �e to nie superlaser, prosz� o spok�j. L�dowanie odwo�ane.'");
					bug("Update_bus: %s cannot land!", ship->name);
				}
				else
				{
					sprintf(buf, FG_CYAN
					"Elektroniczny g�os nadaje 'Witamy w porcie %s'",
							ship->curr_stop->stop_name);
					echo_to_ship(ship, buf);
					echo_to_ship(ship,
							FG_CYAN
							"Elektroniczny g�os nadaje 'Prosz� wychodzic przez g��wn� ramp�. Mi�ego pobytu.'");
				}
				ship->bus_pos++;
				break;
			case 5: /*PREP_TO_LAUNCH */
			{
				auto stop_it = std::find(ship->stops.begin(), ship->stops.end(), ship->curr_stop);
				const char *next_stop_name;
				if (stop_it != ship->stops.end() && std::next(stop_it) != ship->stops.end())
					next_stop_name = (*std::next(stop_it))->stop_name;
				else
					next_stop_name = ship->stops.front()->stop_name;
				sprintf(buf,
				FG_CYAN
				"Elektroniczny g�os nadaje 'Nast�pna stacja, %s'",
						next_stop_name);
				echo_to_ship(ship,
				FG_CYAN
				"Elektroniczny g�os nadaje 'Prosz� zapi�� pasy, startujemy.'");
				echo_to_ship(ship, buf);
				ship->bus_pos++;
			}
				break;
			default:
				/* dodajmy nieco dynamiki ;)        -- Thanos */
				if (number_percent() > 3)
					ship->bus_pos++;
				break;
			}

			/* zmieniamy polo�enie statq */
			if (ship->bus_pos >= 10)
			{
				ship->bus_pos = 0;
				change_bus_planet(ship);
			}
		}
	}
}

float srange(float x, float y, float z)
{
	if (x == 0.0 && y == 0.0 && z == 0.0)
		return (0.0);
	return sqrtf(x * x + y * y + z * z);
}

void move_ships()
{
	SHIP_DATA *ship;
	SHIP_DATA *target;
	float dx, dy, dz, change;
	char buf[MAX_STRING_LENGTH];
	bool ch_found = false;
	int damage;
	SPACE_DATA *system;

	auto missile_snapshot = missile_list;
	for (auto* missile : missile_snapshot)
	{
		ship = missile->fired_from;
		target = missile->target;

		if (target->starsystem && target->starsystem == missile->starsystem)
		{
			if (missile->mx < target->vx)
				missile->mx += UMIN(missile->speed / 5,
						target->vx - missile->mx);
			else if (missile->mx > target->vx)
				missile->mx -= UMIN(missile->speed / 5,
						missile->mx - target->vx);

			if (missile->my < target->vy)
				missile->my += UMIN(missile->speed / 5,
						target->vy - missile->my);
			else if (missile->my > target->vy)
				missile->my -= UMIN(missile->speed / 5,
						missile->my - target->vy);

			if (missile->mz < target->vz)
				missile->mz += UMIN(missile->speed / 5,
						target->vz - missile->mz);
			else if (missile->mz > target->vz)
				missile->mz -= UMIN(missile->speed / 5,
						missile->mz - target->vz);

			if (ship)
			{
				if (abs(missile->mx) - abs(target->vx) <= 20
						&& abs(missile->mx) - abs(target->vx) >= -20
						&& abs(missile->my) - abs(target->vy) <= 20
						&& abs(missile->my) - abs(target->vy) >= -20
						&& abs(missile->mz) - abs(target->vz) <= 20
						&& abs(missile->mz) - abs(target->vz) >= -20)
				{
					if (target->chaff_released <= 0)
					{
						echo_to_room(ship->gunseat, FB_YELLOW
						"Tw�j pocisk trafia prosto w cel!");
						echo_to_cockpit(target,
						FG_RED "Statek zosta� trafiony.");
						echo_to_ship_nospam(target,
								"Pot�na eksplozja wstrz�sa statkiem!");
						sprintf(buf,
								FG_YELLOW
								"Widzisz ma�� chmur� ognia. %s zosta� trafiony pociskiem.",
								target->sslook);
						echo_to_system(target, buf, ship);

						for (auto* ch : char_list)
							if (!IS_NPC(ch)
									&& nifty_is_name(missile->fired_by,
											ch->name))
							{
								ch_found = true;
								damage_ship_ch(target, MIN_MISSILE_DAMAGE,
										MAX_MISSILE_DAMAGE, ch);
							}

						if (!ch_found)
							damage_ship(target, MIN_MISSILE_DAMAGE,
									MAX_MISSILE_DAMAGE);
						extract_missile(missile);
					}
					else
					{
						echo_to_room(ship->gunseat, FB_YELLOW
						"Tw�j pocisk eksploduje w �rodku dzia�a!");
						echo_to_cockpit(target,
						FB_YELLOW
						"Pocisk eksploduje w dziale.");
						extract_missile(missile);
					}
					continue;
				}
				else
				{
					missile->age++;
					if (missile->age >= 50)
					{
						extract_missile(missile);
						continue;
					}
				}
			}

			if (!ship)
			{
				if (abs(missile->mx) - abs(target->vx) <= 20
						&& abs(missile->mx) - abs(target->vx) >= -20
						&& abs(missile->my) - abs(target->vy) <= 20
						&& abs(missile->my) - abs(target->vy) >= -20
						&& abs(missile->mz) - abs(target->vz) <= 20
						&& abs(missile->mz) - abs(target->vz) >= -20)
				{
					if (target->chaff_released <= 0)//Pixel: TODO inny system wabikow (skill i manuvr)
					{
						damage = 50;

						if (target->trawler > 0)
							damage = 10;

						echo_to_cockpit(target,
						FG_RED "Statek zosta� trafiony.");
						echo_to_ship_nospam(target,
						FB_RED
						"Pot�na eksplozja wstrz�sa statkiem!");
						echo_to_cockpit(target,
						FB_GREEN
						"Tra� przejmuje wieksz� cz�� wybuchu!");
						sprintf(buf,
								FG_YELLOW
								"Widzisz ma�� chmur� ognia. %s zosta� trafiony pociskiem.",
								target->sslook);
						echo_to_system(target, buf, ship);
						damage_ship(target, damage, damage * 2);
						extract_missile(missile);
					}
					else
					{
						echo_to_cockpit(target,
						FB_YELLOW "Pocisk trafia flare.");
						extract_missile(missile);
					}
					continue;
				}
				else
				{
					missile->age++;
					if (missile->age >= 50)
					{
						extract_missile(missile);
						continue;
					}
				}
			}
		}
		else
		{
			extract_missile(missile);
			continue;
		}
	}

// KURWAAA JAKIE TO JEST LAMERSKIE !!! -- Thanos
// NO QRWA NIEZLE LAMERSKIE -- Aldegard
	auto ship_snapshot = ship_list;
	for (auto* ship : ship_snapshot)
	{

		if (!ship->starsystem)
			continue;

		if (ship->currspeed > ship->realspeed)
			--ship->currspeed;
		if (ship->currspeed > 0)
		{
			change = sqrtf(
					ship->hx * ship->hx + ship->hy * ship->hy
							+ ship->hz * ship->hz);
			if (change > 0)
			{
				dx = ship->hx / change;
				dy = ship->hy / change;
				dz = ship->hz / change;
				ship->vx = floorf(ship->vx + (dx * ship->currspeed / 5));
				ship->vy = floorf(ship->vy + (dy * ship->currspeed / 5));
				ship->vz = floorf(ship->vz + (dz * ship->currspeed / 5));
			}
		}

		// Pixel: what?!
		if (autofly(ship))
			continue;

		if (ship->currspeed > 0)
		{
			system = ship->starsystem;
			for (auto* star : system->stars)
			{
				if (srange((ship->vx - star->xpos), (ship->vy - star->ypos),
						(ship->vz - star->zpos))
						< star->radius + star->gravity * 2)
				{
					echo_to_cockpit(ship,
					FG_RED MOD_BLINK "Wpadasz w s�o�ce."
					RESET);
					sprintf(buf, FG_YELLOW "%s wpada wprost na %s!",
							ship->sslook, star->name);
					echo_to_system(ship, buf, NULL);
					purge_ship(ship, NULL);
				}

				if (!ship)
					break;
			}

			if (!ship)
				continue;

			for (auto* planet : system->planets)
			{
				if (srange((ship->vx - planet->xpos), (ship->vy - planet->ypos),
						(ship->vz - planet->zpos)) < planet->radius)
				{
					sprintf(buf, FB_YELLOW "Rozbijasz statek o %s.",
							planet->name);
					echo_to_cockpit(ship, buf);
					sprintf(buf, FG_YELLOW "%s rozbija si� o %s.", ship->sslook,
							planet->name);
					echo_to_system(ship, buf, NULL);
					purge_ship(ship, NULL);
				}
				else if ((srange((ship->vx - planet->xpos),
						(ship->vy - planet->ypos), (ship->vz - planet->zpos))
						< planet->radius + planet->gravity)
						&& ship->currspeed < 50)
				{
					// Dorobic tu kiedys prawdziwe orbity
					sprintf(buf, FB_YELLOW "Orbitujesz doko�a %s.",
							planet->name);
					echo_to_cockpit(ship, buf);
					sprintf(buf, FG_YELLOW "%s zaczyna orbitowa� doko�a %s.",
							ship->sslook, planet->name);
					echo_to_system(ship, buf, NULL);
					ship->currspeed = 0;
				}

				if (!ship)
					break;
			}

			if (!ship)
				continue;

			for (auto* moon : system->moons)
			{
				if ((srange((ship->vx - moon->xpos), (ship->vy - moon->ypos),
						(ship->vz - moon->zpos)) < moon->radius + moon->gravity)
						&& ship->currspeed < 50)
				{
					sprintf(buf, FB_YELLOW "Orbitujesz doko�a %s." RESET,
							moon->name);
					echo_to_cockpit(ship, buf);
					sprintf(buf, FG_YELLOW "%s zaczyna orbitowa� doko�a %s.",
							ship->sslook, moon->name);
					echo_to_system(ship, buf, NULL);
					ship->currspeed = 0;
				}
			}

		}
	}

	for (auto* ship : ship_list)
	{
		if (ship->collision)
		{
			echo_to_cockpit(ship,
			FB_WHITE MOD_BLINK "Uderzasz burt� w inny statek!"
			RESET);
			echo_to_ship_nospam(ship,
					FB_RED "Pot�na eksplozja wstrz�sa statkiem!");
			damage_ship(ship, ship->collision, ship->collision);

			if (!ship)
				continue;

			ship->collision = 0;
		}
	}
	return;
}

void recharge_ships()
{
	char buf[MAX_STRING_LENGTH];

	for (auto* ship : ship_list)
	{

		if (ship->statet0 > 0)
		{
			ship->energy -= ship->statet0;
			ship->statet0 = 0;
		}

		for (auto* turret : ship->turrets)
		{
			if (turret->statet0 > 0)
			{
				if (turret->type < 5)		// Wiezyczki laserowe
				{
					ship->energy -= ship->statet0;
					turret->statet0 = 0;
				}
				else if (turret->type == SUPERLASER_TURRET)	// Superlaser
				{
					ship->energy -= 100;
					turret->statet0 -= 1;

					if (turret->statet0 <= 0)
						echo_to_room(get_room_index(turret->vnum),
								FB_GREEN "Superlaser prze�adowany!");
				}
				else						// Baterie Turbolaserowe
				{
					ship->energy -= 25;
					turret->statet0 -= 1;

					if (turret->statet0 <= 0)
						echo_to_room(get_room_index(turret->vnum),
								FB_GREEN "[Komputer celowniczy]: Beep!");
				}
			}
		}

		if (ship->missilestate == MISSILE_RELOAD_2)
		{
			ship->missilestate = MISSILE_READY;
			echo_to_room(ship->gunseat,
					FB_YELLOW "Wyrzutnia pocisk�w prze�adowana.");
		}

		if (ship->missilestate == MISSILE_RELOAD)
			ship->missilestate = MISSILE_RELOAD_2;

		if (ship->missilestate == MISSILE_FIRED)
			ship->missilestate = MISSILE_RELOAD;

		/* Autofly by Pixel -- Czeste ataki (laserowe), 1/3 PULSE_SPACE */
		if (autofly(ship))
		{
			if (ship->starsystem)
			{
				if (ship->target0 && ship->statet0 != LASER_DAMAGED
						&& ship->lasers &&
						//ship->shipstate == SHIP_BUSY_1 &&
						srange((ship->target0->vx - ship->vx),
								(ship->target0->vy - ship->vy),
								(ship->target0->vz - ship->vz)) <= 1000)
				{
					/*
					 if(!is_facing(ship, ship->target0))
					 {
					 ship->hx = ship->target0->vx - ship->vx;
					 ship->hy = ship->target0->vy - ship->vy;
					 ship->hz = ship->target0->vz - ship->vz;
					 ship->energy -= (ship->currspeed * ship->size)/100;

					 if(ship->manuever >= 100)
					 ship->shipstate = SHIP_BUSY_1;
					 else if(ship->manuever >= 90)
					 ship->shipstate = SHIP_BUSY_2;
					 else if(ship->manuever >= 80)
					 ship->shipstate = SHIP_BUSY_3;
					 else if(ship->manuever >= 70)
					 ship->shipstate = SHIP_BUSY_4;
					 else if(ship->manuever >= 60)
					 ship->shipstate = SHIP_BUSY_5;
					 else if(ship->manuever >= 50)
					 ship->shipstate = SHIP_BUSY_6;
					 else if(ship->manuever >= 40)
					 ship->shipstate = SHIP_BUSY_7;
					 else if(ship->manuever >= 30)
					 ship->shipstate = SHIP_BUSY_8;
					 else if(ship->manuever >= 20)
					 ship->shipstate = SHIP_BUSY_9;
					 else if(ship->manuever >= 10)
					 ship->shipstate = SHIP_BUSY_10;
					 else
					 ship->shipstate = SHIP_BUSY_10;
					 }
					 else
					 {*/

					int chance = ship->level;
					SHIP_DATA *target = ship->target0;
					int shots_ava;
					int shots_wystrzelone = 0;
					int shots_trafione = 0;

					for (shots_ava = 0; shots_ava < ship->lasers; shots_ava++)
					{
						if (ship->shipstate != SHIP_HYPERSPACE
								&& ship->energy > 25
								&& ship->target0->starsystem == ship->starsystem
								&& ship->statet0 < ship->lasers
								&& is_facing(ship, target))
						{
							chance += target->size - ship->size;
							chance -= target->manuever - ship->manuever;
							chance -= srange((target->vx - ship->vx),
									(target->vy - ship->vy),
									(target->vz - ship->vz)) / 70;

							chance = URANGE(3, chance, 90);
						}

						shots_wystrzelone++;
						ship->statet0++;

						if (number_percent() <= chance)
							shots_trafione++;
					}

					if (shots_trafione == 0)
					{
						sprintf(buf,
								FB_RED "%s " FG_YELLOW "strzela %s w twoim kierunku, ale chybia.",
								know_trans(target, ship) ?
										SHIPNAME(ship) : ship->sslook,
								shots_wystrzelone == 1 ? "laserem" :
								shots_wystrzelone == 2 ? "dwukrotnie" :
								shots_wystrzelone == 3 ? "trzykrotnie" :
								shots_wystrzelone == 4 ? "czterokrotnie" :
								shots_wystrzelone == 5 ?
										"pi�ciokrotnie" : "wielokrotnie");
						echo_to_cockpit(target, buf);

						sprintf(buf,
								FG_GREEN "%s z " FB_RED "%s " FG_GREEN "chybia " FB_RED "%s" FG_GREEN ".",
								shots_wystrzelone == 1 ? "Wi�zka laserowa" :
								shots_wystrzelone == 2 ?
										"Podw�jna wi�zka laserowa" :
								shots_wystrzelone == 3 ?
										"Potr�jna wi�zka laserowa" :
								shots_wystrzelone == 4 ?
										"Poczw�rna wi�zka laserowa" :
										"Kanonada laserowa", ship->sslook,
								target->sslook);
						echo_to_system(target, buf, NULL);
					}
					else
					{

						sprintf(buf,
								FB_RED "%s " FG_YELLOW "strzela %s w twoim kierunku, " FB_RED "%s.",
								ship->sslook,
								shots_wystrzelone == 1 ? "laserem" :
								shots_wystrzelone == 2 ? "dwukrotnie" :
								shots_wystrzelone == 3 ? "trzykrotnie" :
								shots_wystrzelone == 4 ? "czterokrotnie" :
								shots_wystrzelone == 5 ?
										"pi�ciokrotnie" : "wielokrotnie",

								shots_trafione == shots_wystrzelone ?
										"TRAFIA!" :
								shots_trafione == 1 ? "jedno trafienie" :
								shots_trafione == 2 ? "dwa trafienia" :
								shots_trafione == 3 ? "trzy trafienia" :
								shots_trafione == 4 ? "cztery trafienia" :
								shots_trafione == 5 ?
										"pi�� trafie�" : "wielokrotnie trafia");
						echo_to_cockpit(target, buf);

						sprintf(buf,
								FG_GREEN "Promie� lasera z " FB_RED "%s " FG_GREEN "trafia " FB_RED "%s " FG_GREEN "%s",
								ship->sslook, target->sslook,
								shots_trafione == 1 ? "" :
								shots_trafione == 2 ? "dwukrotnie." :
								shots_trafione == 3 ? "trzykrotnie" :
								shots_trafione == 4 ? "czterokrotnie" :
								shots_trafione == 5 ?
										"pi�ciokrotnie" : "wielokrotnie");
						echo_to_system(target, buf, NULL);
						echo_to_ship_nospam(target,
								FG_RED "Lekka eksplozja powoduje wibracje na pok�adzie.");

						for (; shots_trafione > 0; shots_trafione--)
							damage_ship(target, MIN_LASER_DAMAGE,
									MAX_LASER_DAMAGE);
					}
					//}

				}

			}

		}

		/* Old Autofly */

		/*
		 if(autofly(ship))
		 {
		 if(ship->starsystem)
		 {
		 if(ship->target0 && ship->statet0 != LASER_DAMAGED)
		 {
		 int         chance = 50;
		 SHIP_DATA  *target = ship->target0;
		 int         shots;

		 for(shots = 0; shots <= ship->lasers; shots++)
		 {
		 if(ship->shipstate != SHIP_HYPERSPACE
		 && ship->energy > 25
		 && ship->target0->starsystem == ship->starsystem
		 && abs(target->vx - ship->vx) <= 1000
		 && abs(target->vy - ship->vy) <= 1000
		 && abs(target->vz - ship->vz) <= 1000
		 && ship->statet0 < ship->lasers)
		 {
		 if(is_huge(ship) || is_capital(ship)
		 || is_platform(ship) || is_facing(ship, target))
		 {
		 chance += target->size / 2;
		 chance -= target->manuever / 10;
		 chance -= target->currspeed / 20;
		 chance -= (abs(target->vx - ship->vx) / 70);
		 chance -= (abs(target->vy - ship->vy) / 70);
		 chance -= (abs(target->vz - ship->vz) / 70);
		 chance = URANGE(10, chance, 90);
		 if(number_percent() > chance)
		 {
		 sprintf(buf,
		 FG_YELLOW
		 "%s strzela w twoim kierunku, ale chybia.",
		 ship->sslook);
		 echo_to_cockpit(target, buf);
		 sprintf(buf,
		 FG_YELLOW
		 "Promie� lasera z %s chybia %s.",
		 ship->sslook, target->sslook);
		 echo_to_system(target, buf, NULL);
		 }
		 else
		 {
		 sprintf(buf,
		 FG_YELLOW
		 "Promie� lasera z %s trafia %s.",
		 ship->sslook, target->sslook);
		 echo_to_system(target, buf, NULL);
		 sprintf(buf,
		 FG_RED "%s trafia ci� laserem!",
		 ship->sslook);
		 echo_to_cockpit(target, buf);
		 echo_to_ship(target,
		 FG_RED
		 "Lekka eksplozja powoduje wibracje na pok�adzie.");
		 damage_ship(target, 5, 10);
		 }
		 ship->statet0++;
		 }
		 }
		 }
		 }
		 }
		 }*/
	}
}

void update_space()
{
	SHIP_DATA *target;
	char buf[MAX_STRING_LENGTH];
	char descbuf[MAX_STRING_LENGTH];
	int too_close, target_too_close, recharge;
	char name[256];
	SPACE_DATA *egzist_system;

	for (auto* ship : ship_list)
	{
		if (ship->calctimer >= 1 && ship->starsystem)
		{
			--ship->calctimer;
			if (ship->calctimer == 0)
			{
				sprintf(buf, FG_MAGENTA "Koordynaty skoku przeliczone!" NL
				FB_GREEN "Statek gotowy do hiperskoku.");
				echo_to_cockpit(ship, buf);
			}
		}

		if (ship->shipstate1 < 0)
			ship->shipstate1 = 0;

		if (ship->shipstate1 > 0)
		{
			ship->autorecharge = false;
			ship->cloack = 0;
			ship->interdict = 0;
			ship->autopilot = false;
			ship->autotrack = false;
			ship->currjump = NULL;
			ship->target0 = NULL;
			for (auto* turret : ship->turrets)
				turret->target = NULL;

			ship->vXpos = 0;
			ship->vYpos = 0;
			echo_to_cockpit(ship, FB_RED "Pe�na jonizacja system�w.");

			if (ship->currspeed > 0)
				--ship->currspeed;

			if (ship->sXpos)
				STRDUP(ship->sXpos, "");

			if (ship->sYpos)
				STRDUP(ship->sYpos, "");

			if (--ship->shipstate1 == 0)
				echo_to_cockpit(ship, FB_GREEN "Kontrola system�w odzyskana.");
		}

		if (ship->timer > -1)
		{
			if (ship->timer-- == 0)
			{
				purge_ship(ship, ship->killer);
				continue;
			}
			echo_to_ship(ship, MOD_BLINK FB_RED
			"ALARM: Prosz� SPOKOJNIE uda� si� do kapsu� ratunkowych."
			RESET EOL);
		}

		if (ship->starsystem)
		{
			if (ship->energy > 0 && ship->shipstate == SHIP_DISABLED
					&& !is_platform(ship))
				ship->energy -= 100;

			/*            else if( ship->energy >= 0
			 &&  ship->shipstate1 == 0 )
			 ship->energy += ( 5 + ship->size );
			 else
			 ship->energy = 0;
			 */}

		if (ship->chaff_released)
			ship->chaff_released = 0;

		if (ship->shipstate == SHIP_HYPERSPACE)
		{
			ship->hyperdistance -= ship->hyperspeed * 5;

			if (ship->cargo > 1)
			{
				if (number_range(1, 1000) == 666)
				{
					echo_to_ship(ship, FB_RED "�adunek ulega uszkodzeniu." NL);
					ship->cargo = number_range(85, 100) * ship->cargo / 100;

				}
			}

			egzist_system = NULL;
			if (ship->hyperdistance <= 0)
			{
				if (ship->vXpos != 0 || ship->vYpos != 0)
				{
					for (auto* star_system : starsystem_list)
					{
						if (same_star_pos(star_system->xpos, star_system->ypos,
								ship->vXpos, ship->vYpos, 50, 50))
						{
							egzist_system = star_system;
						}
					}

					if (egzist_system == NULL)
					{
						new_starsystem(ship->sXpos, ship->sYpos);
						sprintf(name, "%s_%s", ship->sXpos, ship->sYpos);
						ship->currjump = starsystem_from_name(name);
					}
					else
					{
						ship->currjump = starsystem_from_name(
								egzist_system->name);
					}
				}
				ship_to_starsystem(ship, ship->currjump);

				if (ship->starsystem == NULL)
				{
					echo_to_cockpit(ship,
							FB_RED
							"Statek zagin�� w hiperprzestrzeni. Wylicz nowe wsp�rz�dne.");
				}
				else
				{
					echo_to_room(ship->pilotseat,
					FB_YELLOW "Hiperskok zako�czony.");
					echo_to_ship(ship,
					FB_YELLOW
					"Statek g�o�no piszczy wychodz�c z hiperprzestrzeni.");
					sprintf(descbuf, FB_GREEN "%s",
							ship->starsystem->description);
					echo_to_cockpit(ship, descbuf);
					if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
					{
						echo_to_cockpit(ship,
								FB_RED
								"W systemie panuje burza elektromagnetyczna, systemy zaczynaj� 'siada�'");
						ship->cloack = 0;
						ship->interdict = 0;
						ship->shield = 0;
						ship->autorecharge = false;
						ship->autotrack = false;
					}
					if (ship->cloack == 0)
					{
						sprintf(buf,
						FB_YELLOW "%s wchodzi w system gwiezdny ",
								ship->sslook);
						echo_to_system(ship, buf, NULL);
					}
					ship->shipstate = SHIP_READY;
					STRDUP(ship->home, ship->starsystem->name);
					if (ship->vXpos != 0 || ship->vYpos != 0)
					{
						ship->vXpos = 0;
						ship->vYpos = 0;
						STRDUP(ship->sXpos, "");
						STRDUP(ship->sYpos, "");
					}
					ship->currjump = NULL;
					ship->jx = 0;
					ship->jy = 0;
					ship->jz = 0;
					if (str_cmp("Public", ship->owner))
						save_ship(ship);
				}
			}
			else
			{
				sprintf(buf,
				FB_YELLOW "Szacowany dystans do ko�ca skoku: "
				FB_WHITE "%.0f" PLAIN, ship->hyperdistance);
				echo_to_room(ship->pilotseat, buf);
			}
		}

		/* following was originaly to fix ships that lost their pilot
		 in the middle of a manuever and are stuck in a busy state
		 but now used for timed manouevers such as turning */

		/* Pixel: Czas manewrowania statkiem, 10 poziomow */

		if (ship->shipstate == SHIP_BUSY_1)
		{
			echo_to_room(ship->pilotseat, FB_YELLOW "Manewr wykonany.");
			ship->shipstate = SHIP_READY;
		}

		if (ship->shipstate == SHIP_BUSY_2)
			ship->shipstate = SHIP_BUSY_1;

		if (ship->shipstate == SHIP_BUSY_3)
			ship->shipstate = SHIP_BUSY_2;

		if (ship->shipstate == SHIP_BUSY_4)
			ship->shipstate = SHIP_BUSY_3;

		if (ship->shipstate == SHIP_BUSY_5)
			ship->shipstate = SHIP_BUSY_4;

		if (ship->shipstate == SHIP_BUSY_6)
			ship->shipstate = SHIP_BUSY_5;

		if (ship->shipstate == SHIP_BUSY_7)
			ship->shipstate = SHIP_BUSY_6;

		if (ship->shipstate == SHIP_BUSY_8)
			ship->shipstate = SHIP_BUSY_7;

		if (ship->shipstate == SHIP_BUSY_9)
			ship->shipstate = SHIP_BUSY_8;

		if (ship->shipstate == SHIP_BUSY_10)
			ship->shipstate = SHIP_BUSY_9;

		if (ship->shipstate == SHIP_LAND_2)
			landship(ship, ship->dest);

		if (ship->shipstate == SHIP_LAND)
			ship->shipstate = SHIP_LAND_2;

		if (ship->shipstate == SHIP_LAUNCH_2)
			launchship(ship);

		if (ship->shipstate == SHIP_LAUNCH)
			ship->shipstate = SHIP_LAUNCH_2;

		if (ship->shipstate == SHIP_DOCKED2_2)
			dockship(ship, ship->dest);

		if (ship->shipstate == SHIP_DOCKED2)
			ship->shipstate = SHIP_DOCKED2_2;

		ship->shield = UMAX(0, ship->shield - 1 - (ship->size / 10));

		if (ship->autorecharge && ship->maxshield > ship->shield
				&& ship->energy > 100 && ship->shipstate1 == 0)
		{
			recharge = UMIN(ship->maxshield - ship->shield,
					10 + ship->size / 5);
			recharge = UMIN(recharge, ship->energy / 2 - 100);
			recharge = UMAX(1, recharge);
			ship->shield += recharge;

			if (str_cmp("Public", ship->owner) && ship->type != MOB_SHIP
					&& ship->type != SHIP_PIRATE
					&& ship->shield != ship->maxshield)
				ship->energy -= recharge / 10;

			if (ship->shield > ship->maxshield)
				ship->shield = ship->maxshield;
		}

		if (ship->interdict != 0)
		{
			if (number_range(1, 10000) == 666)
			{
				ship->interdict = 0;
				echo_to_cockpit(ship,
				FB_RED
				"Spi�cie w generatorze studni grawitacyjnej"
				NL "Pole wstrzymuj�ce deaktywuje si�!");
			}
			if (ship->energy < 100 + ship->size / 10)
			{
				ship->interdict = 0;
				echo_to_cockpit(ship,
				FB_RED
				"BRAK ENERGII!! Pole wstrzymuj�ce deaktywuje si�!");
			}
		}

		if (ship->cloack != 0)
		{
			if (number_range(1, 1000) == 666)
			{
				ship->cloack = 0;
				echo_to_cockpit(ship,
				FB_RED
				"Spi�cie w generatorze pola maskuj�cego" NL
				"System maskujacy deaktywuje sie!");
			}
			if (ship->energy < 50 + ship->size / 5)
			{
				ship->cloack = 0;
				echo_to_cockpit(ship,
				FB_RED
				"BRAK ENERGII!! System maskujacy deaktywuje sie!");
			}
		}

		if (ship->shield > 0)
		{
			if (ship->energy < 200)
			{
				ship->shield = 0;
				echo_to_cockpit(ship,
				FB_RED
				"Tarcze statku gasn� wydaj�c z siebie cichy �wist.");
				ship->autorecharge = false;
			}
		}
		if (ship->shield < 0)
		{
			ship->shield = 0;
		}

		//	Wydarzenia losowe

		if (ship->starsystem && IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
		{
			ship->cloack = 0;
			ship->interdict = 0;
			ship->shield = 0;
			ship->autorecharge = false;
			ship->autotrack = false;
			if (number_range(1, 1000) == 666)
			{
				echo_to_cockpit(ship, "Komputer: BEEP!! BLUP!   BeeeP.");
			}
		}
		if (ship->starsystem
				&& IS_SET(ship->starsystem->flags, STARS_EVENT_METEOR))
		{
			if (number_range(1, 10000) == 666)
			{
				damage_ship(ship, 1, 2);
				echo_to_cockpit(ship, FB_RED "Mikrometeor uderza w statek.");
			}
		}
		if (ship->starsystem && IS_SET(ship->starsystem->flags, STARS_EVENT_ION))
		{
			if (number_range(1, 100000) == 666)
			{
				damage_ship_energy(ship, 1, 10, NULL, NULL);
				if (number_range(0, 1) == 0) /* Trog */
				{
					echo_to_cockpit(ship,
					FB_RED
					"Zjonizowany oddech Jota omiata statek.");
				}
				else
				{
					echo_to_cockpit(ship,
					FB_RED
					"Chmura zjonizowanych gaz�w omiata statek.");
				}
			}

		}

		/*	if ( ship->starsystem )
		 {
		 sprintf( buf , FB_BLUE "Hull: %s%d" FB_BLUE "/%d Tarcze: %s%d " FB_BLUE "Energia: %s%d" NL,
		 ship->hull < ship->maxhull/4 ? FB_RED: (ship->hull > ship->maxhull ? FB_CYAN: FB_GREEN ),
		 ship->hull,
		 ship->maxhull,
		 ship->shield < ship->maxshield/4 ? FB_RED: (ship->shield > ship->maxshield ? FB_GREEN:  FB_CYAN),
		 ship->shield,
		 ship->energy < ship->maxenergy/4 ? FB_RED: (ship->energy < ship->maxenergy/2 ? FB_CYAN:  FB_GREEN),
		 ship->energy);
		 echo_to_crew( ship , buf );
		 }
		 */
		if (ship->starsystem && ship->currspeed > 0 && ship->shipstate1 == 0
				&& ship->shipstate != SHIP_HYPERSPACE)
		{
			sprintf(buf,
			FB_BLUE "Pr�dko��: " FB_CYAN "%.0f" FB_BLUE
			"   Koordynaty solarne: " FB_CYAN "%.0f %.0f %.0f", ship->currspeed,
					ship->vx, ship->vy, ship->vz);
			echo_to_cockpit(ship, buf);

			/*          if ( ship->vPilot == NULL && ship->vHijacker == NULL && ship->type != MOB_SHIP && !is_linked(ship,NULL) )
			 {
			 ship->currspeed=0;
			 sprintf( buf, FB_RED "Statek utraci� pilota." NL " AUTOPILOT: Silniki stop!");
			 echo_to_ship ( ship, buf);
			 }
			 */}

		if (ship->starsystem && ship->currspeed > 0 && ship->shipstate1 == 0
				&& ship->shipstate == SHIP_HYPERSPACE)
		{
			bug("Speed & solar coord. in hyper: %s: dis: %.0f ", ship->name,
					ship->hyperdistance);
		}

		if (ship->starsystem && ship->shipstate1 == 0)
		{
			too_close = ship->currspeed + ship->size + 50;
			for (auto* target : ship->starsystem->ships)
			{
				target_too_close = too_close + target->currspeed + target->size;
				if (target != ship
						&& (srange((ship->vx - target->vx),
								(ship->vy - target->vy),
								(ship->vz - target->vz)) < target_too_close)
						&& ship->shipstate1 == 0
						&& !is_linked_with(ship, target))
				{
					sprintf(buf,
					FB_RED
					"Alarm o zbli�eniu: %s  %.0f %.0f %-10.0f %.0f",
							know_trans(ship, target) ?
									SHIPNAME(target) : target->transponder,
							target->vx - ship->vx, target->vy - ship->vy,
							target->vz - ship->vz,
							srange((ship->vx - target->vx),
									(ship->vy - target->vy),
									(ship->vz - target->vz)));
					echo_to_room(ship->pilotseat, buf);
				}
			}

			too_close = ship->currspeed + ship->size + 300;
			for (auto* moon : ship->starsystem->moons)
			{
				if ((srange((ship->vx - moon->xpos), (ship->vy - moon->ypos),
						(ship->vz - moon->zpos)) < too_close + moon->gravity * 2)
						&& ship->currspeed > 0 && ship->shipstate1 == 0)
				{
					sprintf(buf,
					FB_RED
					"Alarm o zbli�eniu: %s  %.0f %.0f %-10.0f %.0f", moon->name,
							moon->xpos - ship->vx, moon->ypos - ship->vy,
							moon->zpos - ship->vz,
							srange((ship->vx - moon->xpos),
									(ship->vy - moon->ypos),
									(ship->vz - moon->zpos)));
					echo_to_room(ship->pilotseat, buf);
				}
			}

			for (auto* planet : ship->starsystem->planets)
			{
				if ((srange((ship->vx - planet->xpos),
						(ship->vy - planet->ypos), (ship->vz - planet->zpos))
						< too_close + planet->radius + planet->gravity * 3)
				//&& ship->currspeed > 0
						&& ship->shipstate1 == 0)
				{
					sprintf(buf,
					FB_RED
					"Alarm o zbli�eniu: %s  %.0f %.0f %-10.0f %.0f",
							planet->name, planet->xpos - ship->vx,
							planet->ypos - ship->vy, planet->zpos - ship->vz,
							(srange((planet->xpos - ship->vx),
									(planet->ypos - ship->vy),
									(planet->zpos - ship->vz)) - planet->radius));
					echo_to_room(ship->pilotseat, buf);
				}
			}

			for (auto* star : ship->starsystem->stars)
			{
				if ((srange((ship->vx - star->xpos), (ship->vy - star->ypos),
						(ship->vz - star->zpos))
						< too_close + star->radius + star->gravity * 4)
						&& ship->currspeed > 0 && ship->shipstate1 == 0)
				{
					sprintf(buf,
					FB_RED
					"Alarm o zbli�eniu: %s  %.0f %.0f %-10.0f %.0f", star->name,
							star->xpos - ship->vx, star->ypos - ship->vy,
							star->zpos - ship->vz,
							srange((ship->vx - star->xpos),
									(ship->vy - star->ypos),
									(ship->vz - star->zpos)));
					echo_to_room(ship->pilotseat, buf);
				}
			}
		}

		if (ship->target0)
		{
			sprintf(buf,
			FB_BLUE "Cel: " FB_CYAN "%s   %.0f %.0f %-10.0f %.0f" EOL,
					know_trans(ship, ship->target0) ?
							SHIPNAME(ship->target0) :
							ship->target0->transponder,
					ship->target0->vx - ship->vx, ship->target0->vy - ship->vy,
					ship->target0->vz - ship->vz,
					srange((ship->target0->vx - ship->vx),
							(ship->target0->vy - ship->vy),
							(ship->target0->vz - ship->vz)));
			echo_to_room(ship->gunseat, buf);
			if (ship->starsystem != ship->target0->starsystem)
				ship->target0 = NULL;
		}

		for (auto* turret : ship->turrets)
		{
			if (turret->target)
			{
				sprintf(buf,
				FB_BLUE "Cel: " FB_CYAN "%s   %.0f %.0f %-10.0f %.0f"
				EOL,
						know_trans(ship, turret->target) ?
								SHIPNAME(turret->target) :
								turret->target->transponder,
						turret->target->vx - ship->vx,
						turret->target->vy - ship->vy,
						turret->target->vz - ship->vz,
						srange((turret->target->vx - ship->vx),
								(turret->target->vy - ship->vy),
								(turret->target->vz - ship->vz)));
				echo_to_room(get_sroom(ship, turret->vnum), buf);
				if (ship->starsystem != turret->target->starsystem)
					turret->target = NULL;
			}
		}

		if (ship->energy < 100 && ship->starsystem && ship->shipstate1 == 0)
			echo_to_cockpit(ship, FB_RED "Ostrze�enie: Niski poziom paliwa.");

	}

	for (auto* ship : ship_list)
	{

		if (ship->autotrack && ship->target0
				&& (is_scout(ship) || is_fighter(ship) || is_freighter(ship)
						|| is_midship(ship) || is_capital(ship)))
		{
			/* 											//Pixel: przeciez i tak nie ma kolizji...?
			 target = ship->target0;
			 too_close = ship->currspeed + 10;
			 target_too_close = too_close + target->currspeed;
			 if(target != ship && ship->shipstate == SHIP_READY &&
			 srange((ship->vx - target->vx), (ship->vy - target->vy),
			 (ship->vz - target->vz)) < target_too_close)
			 {
			 ship->hx = 0 - (ship->target0->vx - ship->vx);
			 ship->hy = 0 - (ship->target0->vy - ship->vy);
			 ship->hz = 0 - (ship->target0->vz - ship->vz);
			 ship->energy -= (ship->currspeed * ship->size)/100;
			 echo_to_room(ship->pilotseat, FB_RED
			 "Autonaprowadzanie: Wykonanie uniku w celu unikni�cia kolizji!"
			 EOL);

			 if(ship->manuever >= 100)
			 ship->shipstate = SHIP_BUSY_1;
			 else if(ship->manuever >= 90)
			 ship->shipstate = SHIP_BUSY_2;
			 else if(ship->manuever >= 80)
			 ship->shipstate = SHIP_BUSY_3;
			 else if(ship->manuever >= 70)
			 ship->shipstate = SHIP_BUSY_4;
			 else if(ship->manuever >= 60)
			 ship->shipstate = SHIP_BUSY_5;
			 else if(ship->manuever >= 50)
			 ship->shipstate = SHIP_BUSY_6;
			 else if(ship->manuever >= 40)
			 ship->shipstate = SHIP_BUSY_7;
			 else if(ship->manuever >= 30)
			 ship->shipstate = SHIP_BUSY_8;
			 else if(ship->manuever >= 20)
			 ship->shipstate = SHIP_BUSY_9;
			 else if(ship->manuever >= 10)
			 ship->shipstate = SHIP_BUSY_10;
			 else
			 ship->shipstate = SHIP_BUSY_10;
			 }
			 else*/if (!is_facing(ship, ship->target0))
			{
				ship->hx = ship->target0->vx - ship->vx;
				ship->hy = ship->target0->vy - ship->vy;
				ship->hz = ship->target0->vz - ship->vz;
				ship->energy -= (ship->currspeed * ship->size) / 100;
				echo_to_room(ship->pilotseat, FB_BLUE
				"Autonaprowadzanie na cel ... ustalam nowy kurs."
				EOL);

				if (ship->manuever >= 100)
					ship->shipstate = SHIP_BUSY_1;
				else if (ship->manuever >= 90)
					ship->shipstate = SHIP_BUSY_2;
				else if (ship->manuever >= 80)
					ship->shipstate = SHIP_BUSY_3;
				else if (ship->manuever >= 70)
					ship->shipstate = SHIP_BUSY_4;
				else if (ship->manuever >= 60)
					ship->shipstate = SHIP_BUSY_5;
				else if (ship->manuever >= 50)
					ship->shipstate = SHIP_BUSY_6;
				else if (ship->manuever >= 40)
					ship->shipstate = SHIP_BUSY_7;
				else if (ship->manuever >= 30)
					ship->shipstate = SHIP_BUSY_8;
				else if (ship->manuever >= 20)
					ship->shipstate = SHIP_BUSY_9;
				else if (ship->manuever >= 10)
					ship->shipstate = SHIP_BUSY_10;
				else
					ship->shipstate = SHIP_BUSY_10;
			}
		}

		if (autofly(ship))
		{
			if (ship->starsystem)
			{
				if (ship->target0)
				{
					int chance = ship->level;

					/* auto assist ships */

					for (auto* target : ship->starsystem->ships)
					{
						if (autofly(target))
							if (!str_cmp(target->owner, ship->owner)
									&& target != ship)
								if (target->target0 == NULL
										&& ship->target0 != target)
								{
									target->target0 = ship->target0;
									sprintf(buf, FG_RED "%s namierza ci�.",
											know_trans(ship, target) ?
													SHIPNAME(target) :
													target->transponder);
									echo_to_cockpit(target->target0, buf);
									break;
								}
					}

					target = ship->target0;
					ship->autotrack = true;
					if (!is_platform(ship))
						ship->currspeed = ship->realspeed;
					if (ship->energy > 200)
						ship->autorecharge = true;

					if (ship->shipstate != SHIP_HYPERSPACE && ship->energy > 25
							&& ship->missilestate == MISSILE_READY
							&& ship->target0->starsystem == ship->starsystem
							&& srange((target->vx - ship->vx),
									(target->vy - ship->vy),
									(target->vz - ship->vz)) <= 1200
							&& ship->missiles > 0)
					{
						if (is_facing(ship, target))
						{
							chance += target->size - ship->size;
							chance -= target->manuever - ship->manuever;
							chance -= (abs(target->vx - ship->vx) / 100);
							chance -= (abs(target->vy - ship->vy) / 100);
							chance -= (abs(target->vz - ship->vz) / 100);
							chance = URANGE(5, chance, 95);

							if (number_percent() > chance)
							{
							}
							else
							{
								new_missile(ship, target, NULL,
										CONCUSSION_MISSILE);
								ship->missiles--;
								sprintf(buf, FG_RED "Nadlatuje pocisk z %s.",
										know_trans(target, ship) ?
												SHIPNAME(ship) :
												ship->transponder);
								echo_to_cockpit(target, buf);
								sprintf(buf, FG_YELLOW "%s wypuszcza pocisk",
										ship->sslook);
								echo_to_system(target, buf, NULL);

								//ship->target0 == NULL;
								/*
								 if(is_huge(ship) || is_capital(ship)
								 || is_platform(ship))
								 ship->missilestate = MISSILE_RELOAD_2;
								 else
								 ship->missilestate = MISSILE_FIRED;
								 */
							}
						}
					}

					if (ship->missilestate == MISSILE_DAMAGED)
						ship->missilestate = MISSILE_READY;
					if (ship->statet0 == LASER_DAMAGED)
						ship->statet0 = LASER_READY;
					if (ship->shipstate == SHIP_DISABLED)
						ship->shipstate = SHIP_READY;

				}
				else
				{
					ship->currspeed = 0;
//pixprac
					if (IS_SET(ship->flags, SHIP_METAAGGRESSIVE))
						for (auto* target : ship_list)
							if (is_metaaggressive_to(ship, target))
							{
								ship->target0 = target;
								sprintf(buf, FG_RED "%s namierzy� ci�.",
										know_trans(target, ship) ?
												SHIPNAME(ship) :
												ship->transponder);
								echo_to_cockpit(target, buf);
								break;
							}

					if (IS_SET(ship->flags,
							SHIP_AGGRESSIVE) || IS_SET(ship->flags, SHIP_METAAGGRESSIVE))
						for (auto* target : ship_list)
							if (is_aggressive_to(ship, target))
							{
								ship->target0 = target;
								sprintf(buf, FG_RED "%s namierzy� ci�.",
										know_trans(target, ship) ?
												SHIPNAME(ship) :
												ship->transponder);
								echo_to_cockpit(target, buf);
								break;
							}

				}
			}
			else
			{
				if (number_range(1, 25) == 25
						&& ship->shipstate != SHIP_REPOSITORY)
				{
					ship_to_starsystem(ship, starsystem_from_name(ship->home));
					ship->vx = number_range(-5000, 5000);
					ship->vy = number_range(-5000, 5000);
					ship->vz = number_range(-5000, 5000);
					ship->hx = 1;
					ship->hy = 1;
					ship->hz = 1;
				}
			}
		}

		/*
		 if((is_capital(ship) || is_platform(ship) || is_huge(ship))
		 && ship->target0 == NULL)
		 {
		 if(ship->missiles < ship->maxmissiles)
		 ship->missiles++;
		 if(ship->torpedos < ship->maxtorpedos)
		 ship->torpedos++;
		 if(ship->rockets < ship->maxrockets)
		 ship->rockets++;
		 }
		 */
	}
	{
		int dam;

		for (auto* ship : ship_list)
		{
			for (auto* dock : ship->docks)
			{
				if (dock != NULL)
				{
					if (dock->status == 0 && dock->opentimer > 0)
					{
						ship->efflux -= dock->opentimer;
						--dock->opentimer;
						if (dock->opentimer < 0)
							dock->opentimer = 0;
						if (ship->efflux < 0)
							ship->efflux = 0;
					}
					if (dock->status > 0 && dock->target == NULL)
					{
						++dock->opentimer;
						ship->efflux += dock->opentimer;
						if (dock->opentimer > 200)
							dock->opentimer = 200;
					}
					if (dock->status > 0 && dock->target != NULL
							&& dock->opentimer > 0)
					{
						ship->efflux -= dock->opentimer;
						--dock->opentimer;
						if (dock->opentimer < 0)
							dock->opentimer = 0;
						if (ship->efflux < 0)
							ship->efflux = 0;
					}
				}
			}

			if (ship->efflux > 0)
			{
				echo_to_ship(ship,
				FB_RED
				"ALARM!!!      Naruszona struktura poszycia." NL
				"Tracimy atmosfer�.");
				for (auto* room : ship->locations)
				{
					auto people_snapshot = room->people;
					for (auto* rch : people_snapshot)
					{
						dam = number_range(ship->efflux, 2 * ship->efflux);
						if (!IS_RACE(rch, "DUINUOGWUIN"))
							damage(rch, rch, dam, TYPE_UNDEFINED);
					}
				}
			}
		}

	}

}

void write_starsystem_list()
{
	FILE *fpout;

	if (!(fpout = fopen(SPACE_LIST, "w")))
	{
		bug("FATAL: cannot open starsystem.lst for writing!" NL, 0);
		return;
	}
	for (auto* tstarsystem : starsystem_list)
		fprintf(fpout, "%s\n", tstarsystem->filename);
	fprintf(fpout, "$\n");
	fclose(fpout);
}

/*
 * Get pointer to space structure from starsystem name.
 */
SPACE_DATA* starsystem_from_name(char *name)
{
	for (auto* starsystem : starsystem_list)
		if (!str_cmp(name, starsystem->name))
			return starsystem;

	for (auto* starsystem : starsystem_list)
		if (!str_prefix(name, starsystem->name))
			return starsystem;

	return NULL;
}

/*
 * Get pointer to space structure from the dock vnun.
 */
SPACE_DATA* starsystem_from_vnum(int vnum)
{
	for (auto* starsystem : starsystem_list)
	{
		for (auto* planet : starsystem->planets)
			for (auto* dock : planet->docks)
				if (vnum == dock->vnum)
					return starsystem;
		for (auto* moon : starsystem->moons)
			if (vnum == moon->vnum)
				return starsystem;
	}

	for (auto* ship : ship_list)
		for (auto* hangar : ship->hangars)
			if (vnum == hangar->vnum)
				return ship->starsystem;

	return NULL;
}

/*
 * Save a starsystem's data to its data file
 */
void save_starsystem(SPACE_DATA *starsystem)
{
	FILE *fp;
	char filename[256];

	if (!starsystem)
	{
		bug("save_starsystem: null starsystem pointer!", 0);
		return;
	}

	if (!starsystem->filename || starsystem->filename[0] == '\0')
	{
		bug("%s has no filename", starsystem->name);
		return;
	}

	snprintf(filename, sizeof(filename), "%s%s", STARS_DIR, starsystem->filename);

	RESERVE_CLOSE;
	if ((fp = fopen(filename, "w")) == NULL)
	{
		bug("save_starsystem: fopen", 0);
		perror(filename);
		return;
	}
	else
	{
		fprintf(fp, "#SPACE\n");
		fprintf(fp, "Name            %s~\n", starsystem->name);
		fprintf(fp, "Filename        %s~\n", starsystem->filename);
		fprintf(fp, "Xpos            %.0f\n", starsystem->xpos);
		fprintf(fp, "Ypos            %.0f\n", starsystem->ypos);
		fprintf(fp, "Hidden	      %d\n", starsystem->hidden);
		fprintf(fp, "Flags	          %lld\n", starsystem->flags);
		fprintf(fp, "Version	      %d\n", starsystem->version);
		fprintf(fp, "ResetFreq	      %d\n", starsystem->reset_frequency);
		fprintf(fp, "ResetMsg	      %s~\n", starsystem->resetmsg);
		for (auto* star : starsystem->stars)
		{
			fprintf(fp, "Star %.0f %.0f %.0f %.0f %d %s~ %.0f %.0f %.0f\n",
					star->gravity, star->xpos, star->ypos, star->zpos,
					star->type, star->name, star->radius, star->star_radius,
					star->star_radius2);
		}
		for (auto* moon : starsystem->moons)
		{
			fprintf(fp,
					"Moon %.0f %.0f %.0f %.0f %d %d %.0f %s~ %.0f %.0f %.0f\n",
					moon->gravity, moon->xpos, moon->ypos, moon->zpos,
					moon->type, moon->vnum, moon->capacity, moon->name,
					moon->radius, moon->star_radius, moon->star_radius2);
		}
		fprintf(fp, "Description	      %s~\n", starsystem->description);
		fprintf(fp, "Description2	      %s~\n", starsystem->description2);
		fprintf(fp, "Description3	      %s~\n", starsystem->description3);
		fprintf(fp, "End\n\n");

		// added by Thanos
		save_space_resets(starsystem, fp);

		fprintf(fp, "#END\n");
	}
	fclose(fp);
	RESERVE_OPEN;
	return;
}

/*
 * Read in actual starsystem data.
 */

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

void fread_starsystem(SPACE_DATA *starsystem, FILE *fp)
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

		case 'D':
			SKEY("Description", starsystem->description, fread_string(fp))
			;
			SKEY("Description2", starsystem->description2, fread_string(fp))
			;
			SKEY("Description3", starsystem->description3, fread_string(fp))
			;
			break;

		case 'E':
			if (!str_cmp(word, "End"))
			{
				starsystem->reset_frequency = UMAX(DEF_SPACE_RESETFREQ,
						starsystem->reset_frequency);
				starsystem->age = number_range(1,
						starsystem->reset_frequency / 5);

				if (!starsystem->name)
					STRDUP(starsystem->name, "");
				if (!starsystem->description)
					STRDUP(starsystem->description, "");
				if (!starsystem->description2)
					STRDUP(starsystem->description2, "");
				if (!starsystem->description3)
					STRDUP(starsystem->description3, "");
				if (starsystem->version < 2)
				{
					for (auto* star : starsystem->stars)
					{
						if (star->radius == 0)
							star->radius = 1000;
						if (star->zpos >= 0)
							star->zpos = number_range(0, 20);
						if (star->zpos < 0)
							star->zpos = number_range(-20, -1);
						star->star_radius = sqrtf(
								(star->xpos * star->xpos)
										+ (star->ypos * star->ypos));
					}
					for (auto* moon : starsystem->moons)
					{
						if (moon->radius == 0)
							moon->radius = 60;
						if (moon->zpos >= 0)
							moon->zpos = number_range(10, 200);
						if (moon->zpos < 0)
							moon->zpos = number_range(-200, 10);
						moon->star_radius = sqrtf(
								(moon->xpos * moon->xpos)
										+ (moon->ypos * moon->ypos));
					}
					starsystem->version = 2;
				}
				return;
			}
			break;

		case 'F':
			SKEY("Filename", starsystem->filename, fread_string(fp))
			;
			KEY("Flags", starsystem->flags, fread_number(fp))
			;
			break;

		case 'H':
			KEY("Hidden", starsystem->hidden, fread_number(fp))
			;
			break;

		case 'M':
			if (!str_cmp(word, "Moon"))
			{
				MOON_DATA *moon;

				CREATE(moon, MOON_DATA, 1);
				moon->gravity = fread_number(fp);
				moon->xpos = fread_number(fp);
				moon->ypos = fread_number(fp);
				moon->zpos = fread_number(fp);
				moon->type = fread_number(fp);
				moon->vnum = fread_number(fp);
				moon->capacity = fread_number(fp);
				moon->name = fread_string(fp);
//              moon->radius    = fread_number( fp );
//              moon->star_radius       = fread_number( fp );
//              moon->star_radius2      = fread_number( fp );
				starsystem->moons.push_back(moon);

				fread_to_eol(fp);
				fMatch = true;
			}
			break;

		case 'N':
			SKEY("Name", starsystem->name, fread_string(fp))
			;
			break;

		case 'R':
			SKEY("ResetMsg", starsystem->resetmsg, fread_string(fp))
			;
			KEY("ResetFreq", starsystem->reset_frequency, fread_number(fp))
			;
			break;

		case 'S':
			if (!str_cmp(word, "Star"))
			{
				STAR_DATA *star;

				CREATE(star, STAR_DATA, 1);
				star->gravity = fread_number(fp);
				star->xpos = fread_number(fp);
				star->ypos = fread_number(fp);
				star->zpos = fread_number(fp);
				star->type = fread_number(fp);
				star->name = fread_string(fp);
				star->radius = fread_number(fp);
				star->star_radius = fread_number(fp);
				star->star_radius2 = fread_number(fp);
				starsystem->stars.push_back(star);

				fread_to_eol(fp);
				fMatch = true;
			}
			break;

		case 'V':
			KEY("Version", starsystem->version, fread_number(fp))
			;
			break;

		case 'X':
			KEY("Xpos", starsystem->xpos, fread_number(fp))
			;
			break;

		case 'Y':
			KEY("Ypos", starsystem->ypos, fread_number(fp))
			;
			break;

		}

		if (!fMatch)
		{
			bug("no match: %s", word);
		}
	}
}

/*
 * Load a starsystem file
 */
bool load_starsystem(const char *starsystemfile)
{
	char filename[256];
	SPACE_DATA *starsystem;
	FILE *fp;
	bool found;

	CREATE(starsystem, SPACE_DATA, 1);
	STRDUP(starsystem->resetmsg, "");
	starsystem->reset_frequency = DEF_SPACE_RESETFREQ;
	starsystem->age = 1;

	found = false;
	snprintf(filename, sizeof(filename), "%s%s", STARS_DIR, starsystemfile);
	if ((fp = fopen(filename, "r")) != NULL)
	{

		found = true;
		starsystem_list.push_back(starsystem);
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
				bug("Load_starsystem_file: # not found.", 0);
				break;
			}

			word = fread_word(fp);
			if (!str_cmp(word, "SPACE"))
			{
				fread_starsystem(starsystem, fp);
			}
			else if (!str_cmp(word, " RESETS"))
			{
				fread_space_resets(starsystem, fp);
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
		DISPOSE(starsystem);

	return found;
}

/*
 * Load in all the starsystem files.
 */
void load_space()
{
	FILE *fpList;
	const char *filename;

	starsystem_list.clear();

	/*    log_string( "Loading space..." );*/

	RESERVE_CLOSE;
	if ((fpList = fopen(SPACE_LIST, "r")) == NULL)
	{
		perror(SPACE_LIST);
		exit(1);
	}

	for (;;)
	{
		filename = feof(fpList) ? "$" : fread_word(fpList);
		if (filename[0] == '$')
			break;

		if (!load_starsystem(filename))
		{
			bug("Cannot load starsystem file: %s", filename);
		}
	}
	fclose(fpList);
	if (!sysdata.silent)
		log_string(" Done starsystems ");
	RESERVE_OPEN;
	return;
}

void can_land_toggle(int old, int _new)	//by Trog
{
	ROOM_INDEX_DATA *room;

	room = get_room_index(old);
	if (room && IS_SET(room->room_flags, ROOM_CAN_LAND))
		REMOVE_BIT(room->room_flags, ROOM_CAN_LAND);

	room = get_room_index(_new);
	if (room && !IS_SET(room->room_flags, ROOM_CAN_LAND))
		SET_BIT(room->room_flags, ROOM_CAN_LAND);

	return;
}

DEF_DO_FUN( setstarsystem )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	SPACE_DATA *starsystem;
	MOON_DATA *moon;
	STAR_DATA *star;
	int count;

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}
	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg2[0] == '\0' || arg1[0] == '\0')
	{
		send_to_char("Usage: setstarsystem <starsystem> <field> <values>" NL,
				ch);
		send_to_char(NL "Field being one of:" NL, ch);
		send_to_char("name filename xpos ypos hidden desc desc2 desc3," NL, ch);
		send_to_char("star[add edit delete] moon[add edit delete]" NL
		"delete [yes]" NL, ch);
		send_to_char("save flags" NL, ch);
		return;
	}

	starsystem = starsystem_from_name(arg1);
	if (!starsystem)
	{
		send_to_char("No such starsystem." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "flags"))
	{
		char farg[MAX_INPUT_LENGTH];

		argument = one_argument(argument, farg);

		if (farg[0] == '\0')
		{
			send_to_char("Possible flags: temp ion meteor EMP" NL, ch);
			return;
		}

		for (; farg[0] != '\0'; argument = one_argument(argument, farg))
		{
			if (!str_cmp(farg, "ion"))
				TOGGLE_BIT(starsystem->flags, STARS_EVENT_ION);
			else if (!str_cmp(farg, "meteor"))
				TOGGLE_BIT(starsystem->flags, STARS_EVENT_METEOR);
			else if (!str_cmp(farg, "temp"))
				TOGGLE_BIT(starsystem->flags, STARS_FLAG_TEMP);
			else if (!str_cmp(farg, "EMP"))
				TOGGLE_BIT(starsystem->flags, STARS_FLAG_EMP);
			else
				ch_printf(ch, "No such flag: %s" NL, farg);
		}
		donemsg(ch);
		return;
	}

	/*	if ( !str_cmp( arg2, "flags" ) )
	 {
	 starsystem->flags = URANGE( 0, atoi(argument) , 100);
	 send_to_char( "Done." NL, ch );
	 return;
	 }
	 */
	if (!str_cmp(arg2, "delete"))
	{
		if (str_cmp(argument, "yes"))
		{
			send_to_char(
					"Napisz 'yes' na ko�cu je�li jeste� pewien swego dzialania!"
					NL, ch);
			return;
		}
		if (!IS_SET(starsystem->flags, STARS_FLAG_TEMP))
		{
			send_to_char(
					"Tego systemu nie mozna usun��. - nie jest systemem 'TEMP'"
					NL, ch);
			return;
		}
		count = starsystem->ships.size();
		if (count != 0)
		{
			send_to_char(
					"Tego systemu nie mozna usun��. - s� w nim zaparkowane statki"
					NL, ch);
			return;
		}
		extract_starsystem(starsystem);
		write_starsystem_list();
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "save"))
	{
		save_starsystem(starsystem);
		send_to_char("Starsystem Saved." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "desc"))
	{
		string_append(ch, &starsystem->description);
		return;
	}

	if (!str_cmp(arg2, "desc2"))
	{
		string_append(ch, &starsystem->description2);
		return;
	}

	if (!str_cmp(arg2, "desc3"))
	{
		string_append(ch, &starsystem->description3);
		return;
	}

	if (!str_cmp(arg2, "star"))
	{
		argument = one_argument(argument, arg3);

		if (!str_cmp(arg3, "add"))
		{
			if (argument[0] == '\0')
			{
				send_to_char(FB_RED "... star add [name]" NL, ch);
				return;
			}
			CREATE(star, STAR_DATA, 1);
			STRDUP(star->name, argument);
			star->type = 0;
			star->xpos = 100000;
			star->ypos = 0;
			star->zpos = number_range(-2000, 2000);
			star->gravity = 300;
			star->radius = 1000;
			star->star_radius = sqrtf(
					(star->xpos * star->xpos) + (star->ypos * star->ypos));
			starsystem->stars.push_back(star);
		}
		else if (!str_cmp(arg3, "edit"))
		{
			argument = one_argument(argument, arg4);
			argument = one_argument(argument, arg5);

			if ((star = get_star(starsystem, atoi(arg4))) == NULL)
			{
				send_to_char("No such star." NL, ch);
				return;
			}

			if (!str_cmp(arg5, "type"))
			{
				star->type = atoi(argument);
			}
			else if (!str_cmp(arg5, "radius"))
			{
				star->radius = atoi(argument);
			}
			else if (!str_cmp(arg5, "xpos"))
			{
				star->xpos = atoi(argument);
				star->ypos = 0;
				star->star_radius = sqrtf(
						(star->xpos * star->xpos) + (star->ypos * star->ypos));
			}
			else if (!str_cmp(arg5, "ypos"))
			{
				star->ypos = atoi(argument);
				star->star_radius = sqrtf(
						(star->xpos * star->xpos) + (star->ypos * star->ypos));
			}
			else if (!str_cmp(arg5, "zpos"))
			{
				star->zpos = atoi(argument);
			}
			else if (!str_cmp(arg5, "gravity"))
			{
				star->gravity = atoi(argument);
			}
			else if (!str_cmp(arg5, "name"))
				STRDUP(star->name, argument);
			else
			{
				send_to_char(FB_RED
				"Edit: name, type, xpos, ypos, zpos, gravity, radius"
				NL, ch);
				return;
			}
		}
		else if (!str_cmp(arg3, "delete"))
		{
			int nr;

			nr = atoi(argument);

			if ((star = get_star(starsystem, nr)) == NULL)
			{
				send_to_char("No such star." NL, ch);
				return;
			}

			starsystem->stars.remove(star);
			free_star(star);
		}
		else
		{
			do_setstarsystem(ch, (char*) "");
			return;
		}
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "moon"))
	{
		argument = one_argument(argument, arg3);

		if (!str_cmp(arg3, "add"))
		{
			if (argument[0] == '\0')
			{
				send_to_char(FB_RED "... moon add [name]" NL, ch);
				return;
			}
			CREATE(moon, MOON_DATA, 1);
			STRDUP(moon->name, argument);
			moon->type = 0;
			moon->xpos = 100000;
			moon->ypos = 0;
			moon->zpos = number_range(-2000, 2000);
			moon->gravity = 10;
			moon->vnum = 0;
			moon->capacity = 0;
			moon->radius = 30;
			moon->star_radius = sqrtf(
					(moon->xpos * moon->xpos) + (moon->ypos * moon->ypos));
			starsystem->moons.push_back(moon);
		}
		else if (!str_cmp(arg3, "edit"))
		{
			argument = one_argument(argument, arg4);
			argument = one_argument(argument, arg5);

			if ((moon = get_moon(starsystem, atoi(arg4))) == NULL)
			{
				send_to_char("No such moon." NL, ch);
				return;
			}

			if (!str_cmp(arg5, "type"))
			{
				moon->type = atoi(argument);
			}
			else if (!str_cmp(arg5, "radius"))
			{
				moon->radius = atoi(argument);
			}
			else if (!str_cmp(arg5, "xpos"))
			{
				moon->xpos = atoi(argument);
				moon->ypos = 0;
				moon->star_radius = sqrtf(
						(moon->xpos * moon->xpos) + (moon->ypos * moon->ypos));
			}
			else if (!str_cmp(arg5, "ypos"))
			{
				moon->ypos = atoi(argument);
				moon->star_radius = sqrtf(
						(moon->xpos * moon->xpos) + (moon->ypos * moon->ypos));
			}
			else if (!str_cmp(arg5, "zpos"))
			{
				moon->zpos = atoi(argument);
			}
			else if (!str_cmp(arg5, "gravity"))
			{
				moon->gravity = atoi(argument);
			}
			else if (!str_cmp(arg5, "vnum"))
			{
				moon->vnum = atoi(argument);
			}
			else if (!str_cmp(arg5, "capacity"))
			{
				moon->capacity = atoi(argument);
			}
			else if (!str_cmp(arg5, "name"))
				STRDUP(moon->name, argument);
			else if (!str_cmp(arg2, "hidden"))
			{
				starsystem->hidden = URANGE(0, atoi(argument), 100);
				donemsg(ch);
				return;
			}
			else

			{
				send_to_char(
						FB_RED
						"Edit: name, type, xpos, ypos, zpos, vnum, capacity, gravity, radius"
						NL, ch);
				return;
			}
		}
		else if (!str_cmp(arg3, "delete"))
		{
			int nr;

			nr = atoi(argument);

			if ((moon = get_moon(starsystem, nr)) == NULL)
			{
				send_to_char("No such moon." NL, ch);
				return;
			}

			starsystem->moons.remove(moon);
			free_moon(moon);
		}
		else
		{
			do_setstarsystem(ch, (char*) "");
			return;
		}
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "xpos"))
	{
		starsystem->xpos = atoi(argument);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "ypos"))
	{
		starsystem->ypos = atoi(argument);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "hidden"))
	{
		starsystem->hidden = URANGE(0, atoi(argument), 100);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "name"))
	{
		STRDUP(starsystem->name, argument);
		donemsg(ch);
		return;
	}

	do_setstarsystem(ch, (char*) "");
	return;
}

void showstarsystem(CHAR_DATA *ch, SPACE_DATA *starsystem)
{
	int i, j;

	ch_printf(ch,
			"Starsystem:%s     Filename: %s    Xpos: %.0f   Ypos: %.0f" NL,
			starsystem->name, starsystem->filename, starsystem->xpos,
			starsystem->ypos);
	if (starsystem->flags != 0)
	{
		ch_printf(ch, "Flags:" FB_RED "%s" PLAIN "%s%s%s" EOL,
		IS_SET(starsystem->flags,
				STARS_FLAG_TEMP) ? " TEMPORARY" : "",
		IS_SET(starsystem->flags,
				STARS_EVENT_METEOR) ? " Meteor_Shower" : "",
		IS_SET(starsystem->flags,
				STARS_EVENT_ION) ? " Ion_Storm" : "",
		IS_SET(starsystem->flags,
				STARS_FLAG_EMP) ? " EMP_field" : "");
	}
	ch_printf(ch, "Stars:" NL);
	i = 0;
	for (auto* star : starsystem->stars)
	{
		ch_printf(ch,
				"%d  %-20s  Pos: %-7.0f %-7.0f %-8.0f Grav: %-4.0f rad: %.0f"
				NL, i, star->name, star->xpos, star->ypos, star->zpos,
				star->gravity, star->radius);
		i++;
	}
	ch_printf(ch, "Planets:" NL);
	i = 0;
	for (auto* planet : starsystem->planets)
	{
		ch_printf(ch,
				"%d  %-20s  Pos: %-7.0f %-7.0f %-8.0f Grav: %-4.0f rad: %.0f"
				NL, i, planet->name, planet->xpos, planet->ypos, planet->zpos,
				planet->gravity, planet->radius);
		j = 0;
		for (auto* dock : planet->docks)
		{
			ch_printf(ch, "   %d %d (%s) %s" NL, j, dock->vnum, dock->name,
					dock->hidden != 0 ? "{Hidden}" : "");
			j++;
		}
		i++;
	}
	ch_printf(ch, "Moons:" NL);
	ch_printf(ch, "Nr  name xpos ypos zpos grav vnum capacity type Grav" NL);
	i = 0;
	for (auto* moon : starsystem->moons)
	{
		ch_printf(ch,
				"%-2d %-20s %-8.0f %-8.0f %-8.0f %-4.0f %-7d %.0f/%-5.0f %-2d %.0f"
				NL, i, moon->name, moon->xpos, moon->ypos, moon->zpos,
				moon->gravity, moon->vnum, check_capacity(moon->vnum),
				moon->capacity, moon->type, moon->gravity);
		i++;
	}
	if (starsystem->hidden != 0)
	{
		ch_printf(ch, NL "!!!  Invisible to players  !!! v0=1 v1=%d" NL,
				starsystem->hidden);
		ch_printf(ch,
				"Tymczasowo jest tutaj: object musi miec typ shipdevice " NL
				"v0=1 to mapa gwizdna" NL
				"v1 objectu musi byc rowny liczbie hidden starsystemu" NL
				" w wolnej chwili wywale i dopisze helpa" NL);
	}
	ch_printf(ch, NL "Versja: %d" NL, starsystem->version);
	if (*starsystem->description)
	{
		ch_printf(ch, FB_WHITE "Opis 1:" EOL);
		ch_printf(ch, "%s" NL, starsystem->description);
	}
	if (*starsystem->description2)
	{
		ch_printf(ch, FB_WHITE "Opis 2:" EOL);
		ch_printf(ch, "%s" NL, starsystem->description2);
	}
	if (*starsystem->description3)
	{
		ch_printf(ch, FB_WHITE "Opis 3:" EOL);
		ch_printf(ch, "%s" NL, starsystem->description3);
	}
	return;
}

DEF_DO_FUN( showstarsystem )
{
	SPACE_DATA *starsystem;

	starsystem = starsystem_from_name(argument);

	if (starsystem == NULL)
		send_to_char(FB_RED "No such starsystem." NL, ch);
	else
		showstarsystem(ch, starsystem);
}

DEF_DO_FUN( makestarsystem )
{
	char arg[MAX_INPUT_LENGTH];
	char filename[256];
	SPACE_DATA *starsystem;

	if (!argument || argument[0] == '\0')
	{
		send_to_char("Usage: makestarsystem <starsystem name>" NL, ch);
		return;
	}

	CREATE(starsystem, SPACE_DATA, 1);
	STRDUP(starsystem->resetmsg, "");
	starsystem_list.push_back(starsystem);

	STRDUP(starsystem->name, argument);

	starsystem->hidden = 101;
	starsystem->flags = 0;

	argument = one_argument(argument, arg);

	snprintf(filename, sizeof(filename), "%s.system", strlower(arg));
	STRDUP(starsystem->filename, filename);
	STRDUP(starsystem->description, "");
	STRDUP(starsystem->description2, "");
	STRDUP(starsystem->description3, "");
	save_starsystem(starsystem);
	write_starsystem_list();
}

DEF_DO_FUN( starsystems )
{
	SPACE_DATA **starsystems = NULL;
	int i, j, count = get_sorted_systems(&starsystems);

	if (IS_SET(ch->act, PLR_HOLYLIGHT))
	{
		for (i = 0, j = 0; i < count; i++)
		{
			if ((*argument && !str_prefix(argument, starsystems[i]->name))
					|| !*argument)
			{
				ch_printf(ch, "%s%-25s %8.0f %8.0f %s" EOL,
				IS_SET(starsystems[i]->flags,
						STARS_FLAG_TEMP) ? FB_YELLOW : "", starsystems[i]->name,
						starsystems[i]->xpos, starsystems[i]->ypos,
						IS_SET(starsystems[i]->flags,
								STARS_FLAG_TEMP) ? "Temp" : "");
			}
		}
	}
	else
	{
		for (i = 0, j = 0; i < count; i++)
		{
			if ((starsystems[i]->hidden == 0 || has_map(starsystems[i], ch))
					&& !IS_SET(starsystems[i]->flags, STARS_FLAG_TEMP)
					&& ((*argument
							&& !str_prefix(argument, starsystems[i]->name))
							|| !*argument))
			{
				ch_printf(ch, "%-25s", starsystems[i]->name);
				if (!(++j % 3))
					send_to_char( NL, ch);
			}
		}
		send_to_char(NL, ch);
	}

	DISPOSE(starsystems);

	if (!count)
	{
		send_to_char("There are no starsystems currently formed." NL, ch);
		return;
	}
}

void echo_to_ship(SHIP_DATA *ship, const char *argument)
{
//     int room;
//     for ( room = ship->firstroom ; room <= ship->lastroom ;room++ )
//     {
//         echo_to_room( get_room_index(room) , argument );
//     }
//Thanos:
	for (auto* room : ship->locations)
		echo_to_room(room, argument);
}

void echo_to_ship_nospam(SHIP_DATA *ship, const char *argument)
{
	for (auto* room : ship->locations)
	{
		if (!(room == ship->cockpit) || !(room == ship->navseat)
				|| !(room == ship->pilotseat) || !(room == ship->coseat)
				|| !(room == ship->gunseat) || !(room == ship->engineroom))
			echo_to_room(room, argument);
	}
}

void sound_to_ship(SHIP_DATA *ship, const char *argument)
{
	for (auto* room : ship->locations)
	{
		for (auto* vic : room->people)
		{
			if (!IS_NPC(vic) && IS_SET(vic->act, PLR_SOUND))
				send_to_char(argument, vic);
		}
	}

}

void echo_to_cockpit(SHIP_DATA *ship, const char *argument)
{
	for (auto* room : ship->locations)
	{
		if (room == ship->cockpit || room == ship->navseat
				|| room == ship->pilotseat || room == ship->coseat
				|| room == ship->gunseat || room == ship->engineroom)
			echo_to_room(room, argument);
	}

	for (auto* turret : ship->turrets)
		echo_to_room(get_sroom(ship, turret->vnum), argument);

	for (auto* hangar : ship->hangars)
		echo_to_room(get_sroom(ship, hangar->vnum), argument);
}

void echo_to_system(SHIP_DATA *ship, const char *argument, SHIP_DATA *ignore)
{
	if (!ship->starsystem)
		return;

	for (auto* target : ship->starsystem->ships)
	{
		if (target == ship || target == ignore)
			continue;
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz))
				<= target->sensor * 3900 + ship->size * 9)
			echo_to_cockpit(target, argument);
	}
}

bool is_facing(SHIP_DATA *ship, SHIP_DATA *target)
{
	float dy, dx, dz, hx, hy, hz;
	float cosofa;

	hx = ship->hx;
	hy = ship->hy;
	hz = ship->hz;

	dx = target->vx - ship->vx;
	dy = target->vy - ship->vy;
	dz = target->vz - ship->vz;

	cosofa = (hx * dx + hy * dy + hz * dz)
			/ (sqrtf(hx * hx + hy * hy + hz * hz)
					+ sqrtf(dx * dx + dy * dy + dz * dz));

	if (cosofa > 0.75)
		return true;

	return false;
}

long int get_ship_value(SHIP_DATA *ship)
{
	long int price;

	if (is_scout(ship))
		price = 30000;
	else if (is_fighter(ship))
		price = 50000;
	else if (is_midship(ship))
		price = 500000;
	else if (is_freighter(ship))
		price = 700000;
	else if (is_capital(ship))
		price = 5000000;
	else if (is_huge(ship))
		price = 8500000;
	else if (is_platform(ship))
		price = 15000000;
	else
		price = 2000;

	/*     if ( ship->clazz <= CAPITAL_SHIP )
	 price += ( ship->manuever*100*(1+ship->clazz) );

	 price += ( ship->tractorbeam * 1000 );
	 price += ( ship->realspeed * 10 );
	 price += ( ship->astro_array *5 );
	 price += ( 5 * ship->maxhull );
	 price += ( 2 * ship->maxenergy );
	 price += ( 100 * ship->maxchaff );
	 price += ( ship->target_array * 1000);
	 price += ( ship->sensor * 1000);
	 price += ( ship->maxcloack * 100000);

	 if (ship->maxinterdict != 0 )
	 price += (10000000);

	 if (ship->sensor > 1 )
	 price += 20000;

	 if (ship->sensor > 2 )
	 price += 30000;

	 if (ship->sensor > 3 )
	 price += 40000;

	 if (ship->sensor > 4 )
	 price += 50000;

	 if (ship->sensor > 5 )
	 price += 60000;

	 if (ship->sensor > 6 )
	 price += 70000;

	 if (ship->sensor > 7 )
	 price += 80000;

	 if (ship->sensor > 8 )
	 price += 90000;

	 if (ship->sensor > 9 )
	 price += 100000;

	 if (ship->maxenergy > 5000 )
	 price += ( (ship->maxenergy-5000)*20 ) ;

	 if (ship->maxenergy > 10000 )
	 price += ( (ship->maxenergy-10000)*50 );

	 if (ship->maxhull > 1000)
	 price += ( (ship->maxhull-1000)*10 );

	 if (ship->maxhull > 10000)
	 price += ( (ship->maxhull-10000)*20 );

	 if (ship->maxshield > 200)
	 price += ( (ship->maxshield-200)*50 );

	 if (ship->maxshield > 1000)
	 price += ( (ship->maxshield-1000)*100 );

	 if (ship->realspeed > 100 )
	 price += ( (ship->realspeed-100)*500 ) ;

	 if (ship->lasers > 5 )
	 price += ( (ship->lasers-5)*500 );

	 if (ship->maxshield)
	 price += ( 1000 + 10 * ship->maxshield);

	 if (ship->lasers)
	 price += ( 500 + 500 * ship->lasers );

	 if (ship->trawler)
	 price += 20000;

	 if (ship->ioncannons)
	 price += ( 1500 + 1500 * ship->ioncannons );

	 if (ship->maxmissiles)
	 price += ( 1000 + 1000 * ship->maxmissiles );
	 if (ship->maxrockets)
	 price += ( 2000 + 2000 * ship->maxmissiles );
	 if (ship->maxtorpedos)
	 price += ( 1500 + 1500 * ship->maxmissiles );

	 if (ship->missiles )
	 price += ( 250 * ship->missiles );
	 else if (ship->torpedos )
	 price += ( 500 * ship->torpedos );
	 else if (ship->rockets )
	 price += ( 1000 * ship->rockets );
	 */
	for (auto* turret : ship->turrets)
	{
		price += 20000;

		if (turret->type == 0)
			price += 3000;

		if (turret->type == 1)
			price += 9000;

		if (turret->type > 1)
			price += 100000;
	}

	if (ship->hyperspeed)
		price += (1000 + ship->hyperspeed * 10);

	price += 100000 * ship->hangars.size();

	for (auto* module : ship->modules)
		price += module->cost * module->status / module->crs;
//     price *= ship->maxhull/ship->hull
//     price *= 1.5;

	return price;
}

void write_ship_list()
{
	FILE *fpout;

	fpout = fopen(SHIP_LIST, "w");
	if (!fpout)
	{
		bug("FATAL: cannot open ship.lst for writing!" NL, 0);
		return;
	}
	for (auto* tship : ship_list)
		fprintf(fpout, "%s\n", tship->filename);
	fprintf(fpout, "$\n");
	fclose(fpout);
}
//pix0
void remove_ship_from_list(const char *shipfile)
{
	FILE *fpout;
	char filename[256];

	snprintf(filename, sizeof(filename), "%s", shipfile);
	fpout = fopen(SHIP_LIST, "w");
	if (!fpout)
	{
		bug("FATAL: cannot open ship.lst for writing!" NL, 0);
		return;
	}
	for (auto* tship : ship_list)
	{
		if (!str_cmp(filename, tship->filename))
			continue;
		fprintf(fpout, "%s\n", tship->filename);
	}
	fprintf(fpout, "$\n");
	fclose(fpout);
}

SHIP_DATA* ship_from_room(ROOM_INDEX_DATA *room)
{
// SHIP_DATA *ship;
//  for (auto* ship : ship_list)
//  if ( vnum >= ship->firstroom && vnum <= ship->lastroom )
//    return ship;

	return (room && room->ship) ? room->ship : NULL;
}

/*
 * Totally rebuild by Thanos. Teraz ta f-cja pozwala na wyb�r o kt�ry
 * statek nam chodzi. Np. 'openhatch 3.tie' znajduje i otwiera trzeciego
 * tie-fightera w lokacji
 */
SHIP_DATA* ship_in_room(ROOM_INDEX_DATA *room, char *argument)
{
	char arg[MAX_INPUT_LENGTH];
	int number, count;

	if (room == NULL)
		return NULL;

	number = number_argument(argument, arg);

	count = 0;
	for (auto* ship : room->ships)
	{
		if (!nifty_is_name(arg, ship->name))
			continue;

		if (++count == number)
			return ship;
	}
	count = 0;
	for (auto* ship : room->ships)
	{
		if (!nifty_is_name(arg, ship->ship_title))
			continue;

		if (++count == number)
			return ship;
	}

	// teraz po prefixach
	count = 0;
	for (auto* ship : room->ships)
	{
		if (!nifty_is_name_prefix(arg, ship->name))
			continue;

		if (++count == number)
			return ship;
	}
	count = 0;
	for (auto* ship : room->ships)
	{
		if (!nifty_is_name_prefix(arg, ship->ship_title))
			continue;

		if (++count == number)
			return ship;
	}

	return NULL;
}

/*
 * Get pointer to ship structure from ship name.
 */
SHIP_DATA* get_ship(char *argument)
{
	char name[MAX_INPUT_LENGTH];
	int count;
	int number = number_argument(argument, name);

	count = 0;
	for (auto* ship : ship_list)
		if (!str_cmp(name, ship->filename))
			if ((++count) >= number)
				return ship;

	count = 0;
	for (auto* ship : ship_list)
		if (!str_cmp(name, ship->name))
			if ((++count) >= number)
				return ship;

	count = 0;
	for (auto* ship : ship_list)
		if (!str_cmp(name, ship->transponder))
			if ((++count) >= number)
				return ship;

	count = 0;
	for (auto* ship : ship_list)
		if (!str_cmp(name, ship->ship_title))
			if ((++count) >= number)
				return ship;

	count = 0;
	for (auto* ship : ship_list)
		if (nifty_is_name_prefix(name, ship->name))
			if ((++count) >= number)
				return ship;

	count = 0;
	for (auto* ship : ship_list)
		if (nifty_is_name_prefix(name, ship->ship_title))
			if ((++count) >= number)
				return ship;

	return NULL;
}

/*
 * Checks if ships in a starsystem and returns poiner if it is.
 */
SHIP_DATA* get_ship_here(char *name, SPACE_DATA *starsystem)
{
	if (starsystem == NULL)
		return NULL;

	for (auto* ship : starsystem->ships)
		if (!str_cmp(name, ship->sslook))
			return ship;

	for (auto* ship : starsystem->ships)
		if (!str_cmp(name, ship->transponder))
			return ship;

	for (auto* ship : starsystem->ships)
		if (!str_cmp(name, ship->name))
			return ship;

	for (auto* ship : starsystem->ships)
		if (!str_cmp(name, ship->ship_title))
			return ship;

	for (auto* ship : starsystem->ships)
		if (nifty_is_name_prefix(name, ship->sslook))
			return ship;

	for (auto* ship : starsystem->ships)
		if (nifty_is_name_prefix(name, ship->name))
			return ship;

	for (auto* ship : starsystem->ships)
		if (nifty_is_name_prefix(name, ship->ship_title))
			return ship;

	return NULL;
}

/*
 * Get pointer to ship structure from ship name.
 */

/* Zobacz. Do czego odnosi sie ten for? Tylko do pierwszego ifa? Jesli tak
 to reszta jest niepoprawna. A jesli deo wszystkich to nie ma klamerek :)
 -- Thanos */
SHIP_DATA* ship_from_pilot(char *name)
{
	for (auto* ship : ship_list)
	{
		if (!str_cmp(name, ship->pilot))
			return ship;
		if (!str_cmp(name, ship->copilot))
			return ship;
		if (!str_cmp(name, ship->owner))
			return ship;
		if (!str_cmp(name, ship->engineer))
			return ship;
	}
	return NULL;
}

/*
 * Get pointer to ship structure from cockpit, turret, or entrance ramp vnum.
 */

SHIP_DATA* ship_from_cockpit(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	if (!(ship = room->ship))
		return NULL;

	if (room == ship->cockpit || room == ship->pilotseat || room == ship->coseat
			|| room == ship->navseat || room == ship->gunseat
			|| room == ship->engineroom)
		return ship;

	for (auto* turret : ship->turrets)
		if (VNUM(room) == turret->vnum)
			return ship;

	return NULL;
}

SHIP_DATA* ship_from_pilotseat(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	if (!(ship = room->ship))
		return NULL;

	if (room == ship->pilotseat)
		return ship;

	return NULL;
}

SHIP_DATA* ship_from_coseat(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	if (!(ship = room->ship))
		return NULL;

	if (room == ship->coseat)
		return ship;

	return NULL;
}

SHIP_DATA* ship_from_navseat(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	if (!(ship = room->ship))
		return NULL;

	if (room == ship->navseat)
		return ship;

	return NULL;
}

SHIP_DATA* ship_from_gunseat(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	if (!(ship = room->ship))
		return NULL;

	if (room == ship->gunseat)
		return ship;

	return NULL;
}

SHIP_DATA* ship_from_engine(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	if (!(ship = room->ship))
		return NULL;

	if (ship->engineroom)
	{
		if (room == ship->engineroom)
			return ship;
	}
	else
	{
		if (room == ship->cockpit)
			return ship;
	}

	return NULL;
}

SHIP_DATA* ship_from_turret(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	if (!(ship = room->ship))
		return NULL;

	if (room == ship->gunseat)
		return ship;

	for (auto* turret : ship->turrets)
		if (VNUM(room) == turret->vnum)
			return ship;

	return NULL;
}

SHIP_DATA* ship_from_entrance(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	if (!(ship = room->ship))
		return NULL;

	if (room == ship->entrance)
		return ship;

	return NULL;
}

SHIP_DATA* ship_from_hangar(ROOM_INDEX_DATA *room)
{
	SHIP_DATA *ship;

	IF_BUG(room == NULL, "")
		return NULL;

	if (room->vnum)
	{
		for (auto* ship : ship_list)
			for (auto* hangar : ship->hangars)
				if (room->vnum == hangar->vnum)
					return ship;
	}
	else
	{
		if ((ship = room->ship) == NULL)
			return NULL;

		for (auto* hangar : ship->hangars)
			if (room->svnum == hangar->vnum)
				return ship;
	}
	return NULL;
}

void save_ship(SHIP_DATA *ship)
{
	FILE *fp;
	char filename[256];

	IF_BUG(ship == NULL, "")
		return;

	/* Thanos - nowe statki zapisuj� tylko sw�j stan */
	if (ship->vnum)
	{
		save_ship_state(ship);
		return;
	}

	if (!ship->filename || ship->filename[0] == '\0')
	{
		bug("%s has no filename", ship->name);
		return;
	}

	snprintf(filename, sizeof(filename), "%s%s", SHIP_DIR, ship->filename);

	RESERVE_CLOSE;
	if ((fp = fopen(filename, "w")) == NULL)
	{
		bug("save_ship: fopen", 0);
		perror(filename);
	}
	else
	{
		fprintf(fp, "#SHIP\n");
		fprintf(fp, "Name         %s~\n", ship->name);
		fprintf(fp, "Filename     %s~\n", ship->filename);
		fprintf(fp, "Description  %s~\n", ship->description);
		fprintf(fp, "Title	   %s~\n", ship->ship_title);
		fprintf(fp, "Owner        %s~\n", ship->owner);
		fprintf(fp, "Pilot        %s~\n", ship->pilot);
		fprintf(fp, "Copilot      %s~\n", ship->copilot);
		fprintf(fp, "Engineer     %s~\n", ship->engineer);
		fprintf(fp, "Class        %d\n", ship->clazz);
		fprintf(fp, "Tractorbeam  %d\n", ship->tractorbeam);
		fprintf(fp, "Shipyard     %d\n", VNUM(ship->shipyard));
		fprintf(fp, "Statet0      %d\n", ship->statet0);
		fprintf(fp, "Lasers       %d\n", ship->lasers);
		fprintf(fp, "Ioncannons   %d\n", ship->ioncannons);
		fprintf(fp, "Missiles     %d\n", ship->missiles);
		fprintf(fp, "Maxmissiles  %d\n", ship->maxmissiles);
		fprintf(fp, "Rockets      %d\n", ship->rockets);
		fprintf(fp, "Maxrockets   %d\n", ship->maxrockets);
		fprintf(fp, "Torpedos     %d\n", ship->torpedos);
		fprintf(fp, "Maxtorpedos  %d\n", ship->maxtorpedos);
		fprintf(fp, "Lastdoc      %d\n", VNUM(ship->lastdoc));
		fprintf(fp, "Firstroom    %d\n", ship->firstroom);
		fprintf(fp, "Lastroom     %d\n", ship->lastroom);
		fprintf(fp, "Shield       %d\n", ship->shield);
		fprintf(fp, "Maxshield    %d\n", ship->maxshield);
		fprintf(fp, "Hull         %d\n", ship->hull);
		fprintf(fp, "Maxhull      %d\n", ship->maxhull);
		fprintf(fp, "Maxenergy    %d\n", ship->maxenergy);
		fprintf(fp, "Hyperspeed   %.0f\n", ship->hyperspeed);
		fprintf(fp, "Comm         %d\n", ship->comm);
		fprintf(fp, "Chaff        %d\n", ship->chaff);
		fprintf(fp, "Maxchaff     %d\n", ship->maxchaff);
		fprintf(fp, "Sensor       %d\n", ship->sensor);
		fprintf(fp, "Astro_array  %d\n", ship->astro_array);
		fprintf(fp, "Target_array %d\n", ship->target_array);
		fprintf(fp, "Realspeed    %.0f\n", ship->realspeed);
		fprintf(fp, "Type         %d\n", ship->type);
		fprintf(fp, "Pub_Subcl    %d\n", ship->public_subclass);
		fprintf(fp, "Cockpit      %d\n", VNUM(ship->cockpit));
		fprintf(fp, "Coseat       %d\n", VNUM(ship->coseat));
		fprintf(fp, "Pilotseat    %d\n", VNUM(ship->pilotseat));
		fprintf(fp, "Gunseat      %d\n", VNUM(ship->gunseat));
		fprintf(fp, "Navseat      %d\n", VNUM(ship->navseat));
		fprintf(fp, "Flags        %lld\n", ship->flags);	//Dremor
		fprintf(fp, "Engineroom   %d\n", VNUM(ship->engineroom));
		fprintf(fp, "Entrance     %d\n", VNUM(ship->entrance));
		fprintf(fp, "Shipstate    %d\n", ship->shipstate);
		fprintf(fp, "Missilestate %d\n", ship->missilestate);
		fprintf(fp, "Energy       %d\n", ship->energy);
		fprintf(fp, "Manuever     %d\n", ship->manuever);
		fprintf(fp, "Cloack       %d\n", ship->cloack);
		fprintf(fp, "Maxcloack    %d\n", ship->maxcloack);
		fprintf(fp, "Interdict    %d\n", ship->interdict);
		fprintf(fp, "Maxinterdict %d\n", ship->maxinterdict);
		fprintf(fp, "Home         %s~\n", ship->home);
		for (auto* hangar : ship->hangars)
			fprintf(fp, "Hangar       %d %.0f %d %d\n", hangar->vnum,
					hangar->capacity, hangar->status, hangar->type);
		for (auto* turret : ship->turrets)
			fprintf(fp, "Turret       %d %d %d\n", turret->vnum, turret->type,
					turret->status);
		for (auto* transponder : ship->transponders)
			fprintf(fp, "Trans        %s~ %d %s~\n", transponder->number,
					transponder->shipclas, transponder->shipname);
		for (auto* dock : ship->docks)
			fprintf(fp, "Dock         %d %d %d\n", dock->type, dock->vnum,
					dock->status);

		/*	for( module = ship->first_module; module ; module = module->next ) // Narazie zakomentowane bo nie skonczylem
		 fprintf( fp, "Module       %d %d %d %d %d %d %d %d %d %s~\n",  // jeszcze tego - Alde
		 module->type, module->value, module->value2, module->value3, module->size,
		 module->status, module->crs, module->cost, module->vnum, module->spyname );
		 */fprintf(fp, "MaxCargo     %d\n", ship->maxcargo);
		if (ship->cargo > 0)
		{
			fprintf(fp, "Cargo     %d\n", ship->cargo);
			fprintf(fp, "CargoType %d\n", ship->cargotype);
			fprintf(fp, "CargoFrom %s~\n", ship->cargofrom);
		}
		fprintf(fp, "Trawler      %d\n", ship->trawler);
		fprintf(fp, "Size	   %.0f\n", ship->size);
		fprintf(fp, "Sx	   %.0f\n", ship->sx);
		fprintf(fp, "Sy	   %.0f\n", ship->sy);
		fprintf(fp, "Sz	   %.0f\n", ship->sz);
		fprintf(fp, "Transponder  %s~\n", ship->transponder);
		fprintf(fp, "Maxmodule    %d\n", ship->maxmodule);
		fprintf(fp, "Lock_key     %s~\n", ship->lock_key);
		fprintf(fp, "Level    %d\n", ship->level);
		fprintf(fp, "Version    %d\n", ship->version);
		fprintf(fp, "End\n\n");
		fprintf(fp, "#END\n");
	}
	fclose(fp);
	RESERVE_OPEN;
	return;
}

/*
 * Read in actual ship data.
 */

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

void fread_ship(SHIP_DATA *ship, FILE *fp)
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

		case 'A':
			KEY("Astro_array", ship->astro_array, fread_number(fp))
			;
			break;

		case 'C':
			KEY("Cockpit", ship->cockpit, get_room_index(fread_number(fp)))
			;
			KEY("Coseat", ship->coseat, get_room_index(fread_number(fp)))
			;
			KEY("Class", ship->clazz, fread_number(fp))
			;
			SKEY("Copilot", ship->copilot, fread_string(fp))
			;
			KEY("Comm", ship->comm, fread_number(fp))
			;
			KEY("Chaff", ship->chaff, fread_number(fp))
			;
			KEY("Cloack", ship->cloack, fread_number(fp))
			;
			KEY("Cargo", ship->cargo, fread_number(fp))
			;
			KEY("CargoType", ship->cargotype, fread_number(fp))
			;
			SKEY("CargoFrom", ship->cargofrom, fread_string(fp))
			;
			break;

		case 'D':
			SKEY("Description", ship->description, fread_string(fp))
			;
			if (!str_cmp(word, "Dock"))
			{
				SHIPDOCK_DATA *dock;

				CREATE(dock, SHIPDOCK_DATA, 1);
				dock->type = fread_number(fp);
				dock->vnum = fread_number(fp);
				dock->status = fread_number(fp);
				dock->target = NULL;
				dock->targetvnum = 0;
				dock->master_slave = -1;
				ship->docks.push_back(dock);

				fMatch = true;
			}
			break;

		case 'E':
			SKEY("Engineer", ship->engineer, fread_string(fp))
			;
			KEY("Engineroom", ship->engineroom,
					get_room_index(fread_number(fp)))
			;
			KEY("Entrance", ship->entrance, get_room_index(fread_number(fp)))
			;
			KEY("Energy", ship->energy, fread_number(fp))
			;
			if (!str_cmp(word, "End"))
			{
				if (!ship->home)
					STRDUP(ship->home, "");
				if (!ship->name)
					STRDUP(ship->name, "");
				if (!ship->owner)
					STRDUP(ship->owner, "");
				if (!ship->description)
					STRDUP(ship->description, "");
				if (!ship->ship_title)
					STRDUP(ship->ship_title, "");
				if (!ship->copilot)
					STRDUP(ship->copilot, "");
				if (!ship->pilot)
					STRDUP(ship->pilot, "");
				if (!ship->engineer)
					STRDUP(ship->engineer, "");
				if (!ship->transponder)
					STRDUP(ship->transponder, "");
				if (ship->shipstate != SHIP_DISABLED)
					ship->shipstate = SHIP_DOCKED;
				if (ship->statet0 != LASER_DAMAGED)
					ship->statet0 = LASER_READY;
				if (ship->missilestate != MISSILE_DAMAGED)
					ship->missilestate = MISSILE_READY;
				if (!ship->shipyard)
					ship->shipyard = get_room_index(ROOM_LIMBO_SHIPYARD);
				if (!ship->lastdoc)
					ship->lastdoc = ship->shipyard;
				ship->autopilot = false;
				ship->hatchopen = false;

				if (!ship->navseat)
					ship->navseat = ship->cockpit;
				if (!ship->gunseat)
					ship->gunseat = ship->cockpit;
				if (!ship->coseat)
					ship->coseat = ship->cockpit;
				if (!ship->pilotseat)
					ship->pilotseat = ship->cockpit;
				if (ship->missiletype == 1)
				{
					ship->torpedos = ship->missiles; /* for back compatability */
					ship->missiles = 0;
				}
				ship->starsystem = NULL;
				ship->energy = ship->maxenergy;
				ship->hull = ship->maxhull;
				ship->in_room = NULL;
				for (auto* module : ship->modules)
				{
					module->timer = -1;
					if (!module->spyname)
						STRDUP(module->spyname, "");
				}
				if (!ship->sslook)
					STRDUP(ship->sslook, "");
				if (!ship->lock_key || !str_cmp(ship->lock_key, "(null)"))
					STRDUP(ship->lock_key, "0000");
				return;
			}
			break;

		case 'F':
			SKEY("Filename", ship->filename, fread_string(fp))
			;
			KEY("Firstroom", ship->firstroom, fread_number(fp))
			;
			KEY("Flags", ship->flags, fread_number(fp))
			;	//Dremor
			break;

		case 'G':
			KEY("Gunseat", ship->gunseat, get_room_index(fread_number(fp)))
			;
			break;

		case 'H':
			SKEY("Home", ship->home, fread_string(fp))
			;
			KEY("Hyperspeed", ship->hyperspeed, atof(fread_word(fp)))
			;
			KEY("Hull", ship->hull, fread_number(fp))
			;
			if (!str_cmp(word, "Hangar"))
			{
				HANGAR_DATA *hangar;

				CREATE(hangar, HANGAR_DATA, 1);
				hangar->vnum = fread_number(fp);
				hangar->capacity = atof(fread_word(fp));
				hangar->status = fread_number(fp);
				hangar->type = fread_number(fp);
				ship->hangars.push_back(hangar);

				fMatch = true;
			}
			break;

		case 'I':
			KEY("Interdict", ship->interdict, fread_number(fp))
			;
			KEY("Ioncannons", ship->ioncannons, fread_number(fp))
			;
			break;

		case 'L':
			KEY("Laserstr", ship->lasers, (int ) (fread_number(fp) / 10))
			;
			KEY("Lasers", ship->lasers, fread_number(fp))
			;
			KEY("Lastdoc", ship->lastdoc, get_room_index(fread_number(fp)))
			;
			KEY("Lastroom", ship->lastroom, fread_number(fp))
			;
			SKEY("Lock_key", ship->lock_key, fread_string(fp))
			;
			KEY("Level", ship->level, fread_number(fp))
			;
			break;

		case 'M':
			KEY("Manuever", ship->manuever, fread_number(fp))
			;
			if (ship->manuever <= 0)
				ship->manuever = 1;
			KEY("Maxmissiles", ship->maxmissiles, fread_number(fp))
			;
			KEY("Maxtorpedos", ship->maxtorpedos, fread_number(fp))
			;
			KEY("Maxrockets", ship->maxrockets, fread_number(fp))
			;
			KEY("Missiles", ship->missiles, fread_number(fp))
			;
			KEY("Missiletype", ship->missiletype, fread_number(fp))
			;
			KEY("Maxshield", ship->maxshield, fread_number(fp))
			;
			KEY("Maxenergy", ship->maxenergy, fread_number(fp))
			;
			KEY("Missilestate", ship->missilestate, fread_number(fp))
			;
			KEY("Maxhull", ship->maxhull, fread_number(fp))
			;
			KEY("Maxchaff", ship->maxchaff, fread_number(fp))
			;
			KEY("Maxcloack", ship->maxcloack, fread_number(fp))
			;
			KEY("Maxmodule", ship->maxmodule, fread_number(fp))
			;
			KEY("Maxinterdict", ship->maxinterdict, fread_number(fp))
			;
			KEY("MaxCargo", ship->maxcargo, fread_number(fp))
			;
			if (ship->cargotype != CARGO_NONE && ship->cargo < 1)
				ship->cargotype = CARGO_NONE;
			if (!str_cmp(word, "Module"))
			{
				MODULE_DATA *module;

				CREATE(module, MODULE_DATA, 1);
				module->type = fread_number(fp);
				module->value = fread_number(fp);
				module->value2 = fread_number(fp);
				module->value3 = fread_number(fp);
				module->size = fread_number(fp);
				module->status = fread_number(fp);
				module->crs = fread_number(fp);
				module->cost = fread_number(fp);
				module->vnum = fread_number(fp);
				module->spyname = fread_string(fp);
				ship->modules.push_back(module);

				fread_to_eol(fp);
				fMatch = true;
			}
			break;

		case 'N':
			SKEY("Name", ship->name, fread_string(fp))
			;
			KEY("Navseat", ship->navseat, get_room_index(fread_number(fp)))
			;
			break;

		case 'O':
			SKEY("Owner", ship->owner, fread_string(fp))
			;
			if (!str_cmp(word, "Objectnum"))
			{
				fread_number(fp);
				fMatch = true;
				break;
			}
			break;

		case 'P':
			SKEY("Pilot", ship->pilot, fread_string(fp))
			;
			KEY("Pilotseat", ship->pilotseat, get_room_index(fread_number(fp)))
			;
			KEY("Pub_Subcl", ship->public_subclass, fread_number(fp))
			;
			break;

		case 'R':
			KEY("Realspeed", ship->realspeed, fread_number(fp))
			;
			KEY("Rockets", ship->rockets, fread_number(fp))
			;
			break;

		case 'S':
			KEY("Shipyard", ship->shipyard, get_room_index(fread_number(fp)))
			;
			KEY("Sensor", ship->sensor, fread_number(fp))
			;
			KEY("Shield", ship->shield, fread_number(fp))
			;
			KEY("Shipstate", ship->shipstate, fread_number(fp))
			;
			KEY("Statet0", ship->statet0, fread_number(fp))
			;
			KEY("Size", ship->size, atof(fread_word(fp)))
			;
			KEY("Sx", ship->sx, atof(fread_word(fp)))
			;
			KEY("Sy", ship->sy, atof(fread_word(fp)))
			;
			KEY("Sz", ship->sz, atof(fread_word(fp)))
			;
			break;

		case 'T':
			SKEY("Title", ship->ship_title, fread_string(fp))
			;
			KEY("Type", ship->type, fread_number(fp))
			;
			KEY("Tractorbeam", ship->tractorbeam, fread_number(fp))
			;
			KEY("Torpedos", ship->torpedos, fread_number(fp))
			;
			KEY("Target_array", ship->target_array, fread_number(fp))
			;
			KEY("Trawler", ship->trawler, fread_number(fp))
			;
			SKEY("Transponder", ship->transponder, fread_string(fp))
			;
			if (!str_cmp(word, "Trans"))
			{
				TRANSPONDER_DATA *transponder;

				CREATE(transponder, TRANSPONDER_DATA, 1);
				transponder->number = fread_string(fp);
				transponder->shipclas = fread_number(fp);
				transponder->shipname = fread_string(fp);
				ship->transponders.push_back(transponder);

				fMatch = true;
			}
			if (!str_cmp(word, "Turret"))
			{
				TURRET_DATA *turret;

				CREATE(turret, TURRET_DATA, 1);
				turret->vnum = fread_number(fp);
				turret->type = fread_number(fp);
				turret->status = fread_number(fp);
				ship->turrets.push_back(turret);

				fMatch = true;
			}
			break;
		case 'V':
			KEY("Version", ship->version, fread_number(fp))
			;
			break;
		}

		if (!fMatch)
		{
			bug("no match: %s %s", word, ship->name);
		}
	}
}

//added by Thanos (do �adowania kursu statku publicznego)
bool load_course(SHIP_DATA *ship)
{
	FILE *fp;
	COURSE_DATA *course;
	char buf[MAX_STRING_LENGTH];
	int crsnum = 0;
	int nr;

	sprintf(buf, "Loading course for %s", ship->name);
	if (!sysdata.silent)
		log_string(buf);
	sprintf(buf, "%s%s.course", SHIP_DIR, ship->filename);

	if ((fp = fopen(buf, "r")) == NULL)
	{
		bug("Cannot open course file !!! Making ship not public");
		ship->ship_public = false;
		return false;
	}
	for (;;)
	{
		int vnum;

		if ((vnum = fread_number(fp)) == 0)
			break;

		CREATE(course, COURSE_DATA, 1);
		crsnum++;
		course->stop_vnum = vnum;
		course->stop_name = fread_string(fp);
		ship->stops.push_back(course);
	}

	fclose(fp);

	if (crsnum)
	{
		nr = number_range(1, crsnum);
		if (!ship->stops.empty())
		{
			auto stop_it = ship->stops.begin();
			std::advance(stop_it, nr < (int)ship->stops.size() ? nr : 0);
			course = (stop_it != ship->stops.end()) ? *stop_it : ship->stops.front();
		}
		/* ustalamy miejsce startu (losowo co reboot) */
		ship->curr_stop = course;
		ship->bus_pos = number_range(1, 10);

	}
	else
		ship->ship_public = false;

	return true;
}

void prepare_ship(SHIP_DATA *ship)
{
	MODULE_DATA *module;
	ROOM_INDEX_DATA *pRoomIndex;
	CLAN_DATA *clan;
	int a;

	if (!ship->cockpit)
	{
		bug("prepare_ship: ship %s has no cockpit!", ship->name);
		return;
	}

	if (ship->lasers > 0)
	{
		for (a = ship->lasers; a; a--)
		{
			CREATE(module, MODULE_DATA, ship->lasers);
			module->type = 0;
			module->value = 10;
			module->value2 = 3;
			module->value3 = 0;
			module->size = 1;
			module->status = 100;
			module->crs = 100;
			module->cost = 3000;
			module->vnum = 0;
			STRDUP(module->spyname, "");
			ship->modules.push_back(module);
		}
//          ship->lasers = 0;
	}

	if (ship->ioncannons > 0)
	{
		for (a = ship->ioncannons; a; a--)
		{
			CREATE(module, MODULE_DATA, ship->ioncannons);
			module->type = 1;
			module->value = 60;
			module->value2 = 5;
			module->value3 = 0;
			module->size = 1;
			module->status = 100;
			module->crs = 100;
			module->cost = 3000;
			module->vnum = 0;
			STRDUP(module->spyname, "");
			ship->modules.push_back(module);
		}
//          ship->ioncannons = 0;
	}
	if (ship->maxshield > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 20;
		module->value = ship->maxshield;
		module->value2 = 1;
		module->value3 = 0;
		module->size = ship->maxshield / 1000;
		module->status = 100;
		module->crs = 100;
		module->cost = 1000 + 10 * ship->maxshield;
		STRDUP(module->spyname, "");
		if (ship->engineroom)
		{
			module->vnum = VNUM(ship->engineroom);
		}
		else
		{
			module->vnum = VNUM(ship->pilotseat);
		}
		ship->modules.push_back(module);
//          ship->maxshield = 0;
	}

	if (ship->maxmissiles > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 2;	//wyrzutnia
		module->value = 1;
		module->value2 = 1;
		module->value3 = 0;
		module->size = 1;
		module->status = 100;
		module->crs = 100;
		module->cost = 3750;
		module->vnum = 0;
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);

		CREATE(module, MODULE_DATA, 1);
		module->type = 30;	//zasobnik
		module->value = ship->maxmissiles;
		module->value2 = ship->maxmissiles;
		module->value3 = 0;
		module->size = ship->maxmissiles / 5;
		module->status = 100;
		module->crs = 100;
		module->cost = 1000 * ship->maxmissiles;
		module->vnum = VNUM(ship->gunseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->maxmissiles = 0;
//          ship->missiles = 0;
	}

	if (ship->maxtorpedos > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 3;	//wyrzutnia
		module->value = 1;
		module->value2 = 1;
		module->value3 = 0;
		module->size = 1;
		module->status = 100;
		module->crs = 100;
		module->cost = 3500;
		module->vnum = 0;
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);

		CREATE(module, MODULE_DATA, 1);
		module->type = 31;	//zasobnik
		module->value = ship->maxtorpedos;
		module->value2 = ship->maxtorpedos;
		module->value3 = 0;
		module->size = ship->maxtorpedos / 5;
		module->status = 100;
		module->crs = 100;
		module->cost = 900 * ship->maxtorpedos;
		module->vnum = VNUM(ship->gunseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->maxtorpedos = 0;
//          ship->torpedos = 0;
	}

	if (ship->maxrockets > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 4;	//wyrzutnia
		module->value = 1;
		module->value2 = 1;
		module->value3 = 0;
		module->size = 1;
		module->status = 100;
		module->crs = 100;
		module->cost = 3500;
		module->vnum = 0;
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);

		CREATE(module, MODULE_DATA, 1);
		module->type = 32;	//zasobnik
		module->value = ship->maxrockets;
		module->value2 = ship->maxrockets;
		module->value3 = 0;
		module->size = ship->maxrockets / 5;
		module->status = 100;
		module->crs = 100;
		module->cost = 500 * ship->maxrockets;
		module->vnum = VNUM(ship->gunseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->maxrockets = 0;
//          ship->rockets = 0;
	}

	if (ship->maxchaff > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 33;	//zasobnik wabikow
		module->value = ship->maxchaff;
		module->value2 = ship->maxchaff;
		module->value3 = 0;
		module->size = 1;
		module->status = 100;
		module->crs = 100;
		module->cost = 100 * ship->maxchaff;
		module->vnum = 0;
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->maxchaff = 0;
//          ship->chaff = 0;
	}

	if (ship->maxenergy > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 13;
		module->value = ship->maxenergy;
		module->value2 = ship->maxenergy;
		module->value3 = 0;
		module->size = ship->maxenergy / 1000;
		module->status = 100;
		module->crs = 100;
		module->cost = ship->maxenergy / 5;
		STRDUP(module->spyname, "");
		if (ship->engineroom)
		{
			module->vnum = VNUM(ship->engineroom);
		}
		else
		{
			module->vnum = VNUM(ship->pilotseat);
		}
		ship->modules.push_back(module);
//          ship->maxenergy = 0;
	}

	if (ship->tractorbeam > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 23;
		module->value = ship->tractorbeam;
		module->value2 = 0;
		module->value3 = 0;
		module->size = 1 + ship->tractorbeam / 10;
		module->status = 100;
		module->crs = 100;
		module->cost = 8000 * ship->tractorbeam;
		module->vnum = VNUM(ship->coseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->tractorbeam = 0;
	}

	if (ship->maxcloack > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 21;
		module->value = ship->maxcloack * (1 + (ship->size / 10));
		module->value2 = 0;
		module->value3 = 0;
		module->size = 1 + (ship->size / 10);
		module->status = 100;
		module->crs = 100;
		module->cost = 50000 * ship->maxcloack;
		module->vnum = VNUM(ship->coseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->maxcloack = 0;
	}

	if (ship->maxinterdict > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 22;
		module->value = 1000 * ship->maxinterdict;
		module->value2 = 0;
		module->value3 = 0;
		module->size = 1 + (ship->size / 10);
		module->status = 100;
		module->crs = 100;
		module->cost = 100000 * ship->maxinterdict;
		module->vnum = VNUM(ship->coseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->maxinterdict = 0;
	}

	if (ship->trawler > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 24;
		module->value = 80;
		module->value2 = 99;
		module->value3 = 1;
		module->size = 1 + (ship->size / 10);
		module->status = 100;
		module->crs = 100;
		module->cost = 10000 * ship->trawler;
		module->vnum = VNUM(ship->gunseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->trawler = 0;
	}

	if (ship->realspeed > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 27;
		module->value = ship->realspeed;
		module->value2 = 10;
		module->value3 = 0;
		module->size = 1 + (ship->size / 10);
		module->status = 100;
		module->crs = 100;
		module->cost = 100 * (10 + (ship->size / 10)) * ship->realspeed;
		STRDUP(module->spyname, "");
		if (ship->engineroom)
		{
			module->vnum = VNUM(ship->engineroom);
		}
		else
		{
			module->vnum = VNUM(ship->pilotseat);
		}
		ship->modules.push_back(module);
//          ship->realspeed = 0;
	}

	if (ship->manuever > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 28;
		module->value = ship->manuever;
		module->value2 = 10;
		module->value3 = 0;
		module->size = 1;
		module->status = 100;
		module->crs = 100;
		module->cost = 100 * (10 + (ship->size / 10)) * ship->manuever;
		STRDUP(module->spyname, "");
		if (ship->engineroom)
		{
			module->vnum = VNUM(ship->engineroom);
		}
		else
		{
			module->vnum = VNUM(ship->pilotseat);
		}
		ship->modules.push_back(module);
//          ship->manuever = 0;
	}

	if (ship->hyperspeed > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 29;
		module->value = 100 + (ship->hyperspeed / 100 - 1);
		module->value2 = 1000;
		module->value3 = 0;
		module->size = (1 + (ship->size / 10)) * 2;
		module->status = 100;
		module->crs = 100;
		module->cost = 100 * (15 + (ship->size / 10)) * ship->hyperspeed;
		STRDUP(module->spyname, "");
		if (ship->engineroom)
		{
			module->vnum = VNUM(ship->engineroom);
		}
		else
		{
			module->vnum = VNUM(ship->pilotseat);
		}
		ship->modules.push_back(module);
//          ship->hyperspeed = 0;
	}

	if (ship->sensor > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 7;
		module->value = ship->sensor;
		module->value2 = 0;
		module->value3 = 0;
		module->size = ship->sensor / 10;
		module->status = 100;
		module->crs = 100;
		module->cost = 1000 * ship->sensor;
		module->vnum = VNUM(ship->pilotseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->sensor = 0;
	}

	if (ship->target_array > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 8;
		module->value = ship->target_array;
		module->value2 = 0;
		module->value3 = 0;
		module->size = ship->target_array / 10;
		module->status = 100;
		module->crs = 100;
		module->cost = 1000 * ship->target_array;
		module->vnum = VNUM(ship->gunseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->target_array = 0;
	}

	if (ship->astro_array > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 9;
		module->value = ship->astro_array;
		module->value2 = 0;
		module->value3 = 0;
		module->size = ship->astro_array / 10;
		module->status = 100;
		module->crs = 100;
		module->cost = 1000 * ship->astro_array;
		module->vnum = VNUM(ship->navseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->astro_array = 0;
	}

	if (ship->comm > 0)
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 10;
		module->value = ship->comm;
		module->value2 = 0;
		module->value3 = 0;
		module->size = ship->comm / 10;
		module->status = 100;
		module->crs = 100;
		module->cost = 1000 * ship->comm;
		module->vnum = VNUM(ship->navseat);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
//          ship->comm = 0;
	}

	if (is_ship(ship))
	{
		CREATE(module, MODULE_DATA, 1);
		module->type = 26;	//reaktor
		module->value = 100;
		module->value2 = 10;
		module->value3 = 0;
		module->size = (1 + (ship->size / 10)) * 2;
		module->status = 100;
		module->crs = 100;
		module->cost = 20000 * (1 + (ship->size / 10));
		STRDUP(module->spyname, "");
		if (ship->engineroom)
		{
			module->vnum = VNUM(ship->engineroom);
		}
		else
		{
			module->vnum = VNUM(ship->pilotseat);
		}
		ship->modules.push_back(module);

		CREATE(module, MODULE_DATA, 1);
		module->type = 6;
		module->value = 1;
		module->value2 = 0;
		module->value3 = 0;
		module->size = 1;
		module->status = 100;
		module->crs = 100;
		module->cost = 1000;
		module->vnum = VNUM(ship->cockpit);
		STRDUP(module->spyname, "");
		ship->modules.push_back(module);
	}

	a = 0;
	for (auto* module : ship->modules)
		a += module->size;
	ship->maxmodule = a;

	if (ship->comm <= 0)
		ship->comm = 1;

	if (ship->target_array <= 0)
		ship->target_array = 1;

	if (ship->astro_array <= 0)
		ship->astro_array = 1;

	if (ship->sensor <= 0)
		ship->sensor = 1;
	/*
	 if(ship->size <= 0)
	 {
	 if(is_scout(ship))
	 ship->size = 10;

	 if(is_fighter(ship))
	 ship->size = 15;

	 if(is_midship(ship))
	 ship->size = 20;

	 if(is_freighter(ship))
	 ship->size = 100;

	 if(is_capital(ship))
	 ship->size = 1000;

	 if(is_huge(ship))
	 ship->size = 1900;

	 if(is_platform(ship))
	 ship->size = 2000;
	 }
	 */

	if (is_ship(ship) && !ship->vnum)
	{
		if (ship->transponder[0] == '\0'
				|| !str_cmp(ship->transponder, "(null)")
				|| ship->transponder[0] != 't' || ship->transponder[0] != 'T')
			generate_transponder(ship);

		if (ship->ship_title[0] == '\0')
			STRDUP(ship->ship_title, ship->name);
	}
//                           !!!!
	if (is_ship(ship))
	{
		ship->hypertimer = 0;
		ship->hyperready = -1;
		ship->calctimer = 0;

		for (auto* turret : ship->turrets)
		{
			turret->gunner = NULL;
			turret->target = NULL;
		}

		for (auto* hangar : ship->hangars)
		{
			hangar->status = -1;
		}
	}

	if (!str_cmp("Public", ship->owner) || ship->type == MOB_SHIP
			|| ship->type == SHIP_PIRATE)	//publiczne lub moby
	{
		if (!is_platform(ship) && ship->type != MOB_SHIP
				&& ship->type != SHIP_PIRATE && !is_capital(ship)
				&& !is_huge(ship))	//plat i cap nie moby
		{
			extract_ship(ship);
			ship_to_room(ship, ship->shipyard);
			ship->location = ship->shipyard;
			ship->lastdoc = ship->shipyard;
			ship->shipstate = SHIP_DOCKED;
		}
		ship->currspeed = 0;
		ship->energy = ship->maxenergy;
		ship->chaff = ship->maxchaff;
//              ship->hull              = ship->maxhull; Aldegard: tymczasowo
		ship->shield = 0;
		ship->statet0 = LASER_READY;
		ship->missilestate = LASER_READY;
		ship->currjump = NULL;
		ship->target0 = NULL;
		ship->vXpos = 0;
		ship->vYpos = 0;
		STRDUP(ship->sXpos, "");
		STRDUP(ship->sYpos, "");
		ship->hatchopen = false;
		ship->missiles = ship->maxmissiles;
		ship->torpedos = ship->maxtorpedos;
		ship->rockets = ship->maxrockets;
		ship->autorecharge = false;
		ship->autotrack = false;
		ship->autospeed = false;
	}
	else if (ship->cockpit->vnum == ROOM_SENATE_SHUTTLE
			|| is_turbocar(ship->cockpit->vnum)
			|| ship->cockpit->vnum == ROOM_CORUSCANT_SHUTTLE
			|| (ship->ship_public && !ship->stops.empty()))
	{
		/* BUGFIX by Thanos  PubShipy powinny startowa� w kosmosie */
		extract_ship(ship);
	}
	else if ((pRoomIndex = ship->lastdoc) != NULL && !is_capital(ship)
			&& !is_platform(ship) && !is_huge(ship) && ship->type != MOB_SHIP
			&& ship->type != SHIP_PIRATE)
	{
		pRoomIndex->ships.push_back(ship);
		ship->in_room = pRoomIndex;
		ship->location = ship->lastdoc;
	}

	if (is_platform(ship) || ship->type == MOB_SHIP || is_huge(ship)
			|| is_capital(ship) || ship->type == SHIP_PIRATE)
	{
		extract_ship(ship);
		ship_to_starsystem(ship, starsystem_from_name(ship->home));
		generate_sslook(ship);

		if (ship->sx != 0 || ship->sy != 0 || ship->sz != 0)
		{
			ship->vx = ship->sx;
			ship->vy = ship->sy;
			ship->vz = ship->sz;
		}
		else
		{
			ship->vx = number_range(-50000, 50000);
			ship->vy = number_range(-50000, 50000);
			ship->vz = number_range(-50000, 50000);
		}

		ship->hx = 1;
		ship->hy = 1;
		ship->hz = 1;
		ship->shipstate = SHIP_READY;
		ship->autopilot = true;
		ship->autorecharge = true;
		ship->shield = ship->maxshield;
	}

	if (ship->type != MOB_SHIP && (clan = get_clan(ship->owner)) != NULL)
	{
		if (is_ship(ship))
			clan->spacecrafts++;
		else
			clan->vehicles++;
	}

}

/*
 * Load a ship file
 */

bool load_ship_file(const char *shipfile)
{
	char filename[256];
	SHIP_DATA *ship;
	FILE *fp;
	bool found;
	int i;

	ship = new_ship();
	ship->timer = -1;

	found = false;
	snprintf(filename, sizeof(filename), "%s%s", SHIP_DIR, shipfile);

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
				bug("Load_ship_file: # not found.", 0);
				break;
			}

			word = fread_word(fp);
			if (!str_cmp(word, "SHIP"))
			{
				fread_ship(ship, fp);
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
	{
		DISPOSE(ship);
		return false;
	}
	else
	{
		ship_list.push_back(ship);
		prepare_ship(ship);
	}

	//added by Thanos (statki publiczne)
	//sprawdzamy, czy statek jest publiczny (znajduje si� na li�cie w pliku
	// bus.lst)
	ship->ship_public = false;
	for (i = 0; i < MAX_SHIP; i++)
	{
		if (pubshipname[i] && (!strcmp(pubshipname[i], ship->filename)))
		{
			if (!load_course(ship))
				bug("Load_ship_file: Couldn't load course!");
			else
			{
				ship->ship_public = true;
				extract_ship(ship);
			}
			break;
		}
	}
	return found;
}

/*
 * Load in all the ship files.
 */
void load_ships()
{
	FILE *fpList;
	const char *filename;
	int i;
	FILE *fpub;

	ship_list.clear();
	missile_list.clear();

	RESERVE_CLOSE;

	//added by Thanos - �aduje list� statk�w publicznych
	log_string("Reading public ships list");
	if ((fpub = fopen(BUS_LIST, "r")) == NULL)
		log_string("No public ships list or cannot open BUS_LIST file");
	else
	{
		for (i = 0; i < MAX_SHIP; i++)
		{
			char letter;
			letter = fread_letter(fpub);
			if (letter == '$')
				break;
			pubshipname[i] = fread_string(fpub);
		}
		fclose(fpub);
		if (!sysdata.silent)
			log_string(" Done Publist ");
	}
	//done

	if ((fpList = fopen(SHIP_LIST, "r")) == NULL)
	{
		perror(SHIP_LIST);
		exit(1);
	}

	for (;;)
	{
		filename = feof(fpList) ? "$" : fread_word(fpList);

		if (filename[0] == '$')
			break;

		if (!load_ship_file(filename))
		{
			bug("Cannot load ship file: %s", filename);
		}
	}
	fclose(fpList);

	if (!sysdata.silent)
		log_string(" Done ships ");

	RESERVE_OPEN;

	// Thanos -- narazie tutaj
	log_string("Setting ship room pointers");
	for (auto* ship : ship_list)
	{
		ship->vnum = 0;
		for (i = ship->firstroom; i <= ship->lastroom; i++)
		{
			ROOM_INDEX_DATA *room;

			if ((room = get_room_index(i)) == NULL)
				continue;

			room->ship = ship;
			ship->locations.push_back(room);
		}
		ship->roomcount = ship->lastroom - ship->firstroom + 1;
	}
	if (!sysdata.silent)
		log_string(" Done ship room pointers");

	return;
}

void resetship(SHIP_DATA *ship)
{
	SHIPDOCK_DATA *targetdock;

	ship->shipstate = SHIP_READY;

	if (!is_platform(ship) && ship->type != MOB_SHIP)
	{
		extract_ship(ship);
		ship_to_room(ship, ship->shipyard);

		ship->location = ship->shipyard;
		ship->lastdoc = ship->shipyard;
		ship->shipstate = SHIP_DOCKED;
	}

	if (ship->starsystem)
		ship_from_starsystem(ship, ship->starsystem);
	ship->shipstate1 = 0;
	ship->currspeed = 0;
	ship->energy = ship->maxenergy;
	ship->chaff = ship->maxchaff;
	ship->hull = ship->maxhull;
	ship->shield = 0;
	ship->cloack = 0;
	ship->interdict = 0;
	ship->hyperready = -1;
	ship->hypertimer = 0;
	ship->calctimer = 0;
	ship->statet0 = LASER_READY;
	ship->missilestate = LASER_READY;
	ship->currjump = NULL;
	ship->target0 = NULL;
	ship->last_dock_with = NULL;
	ship->vXpos = 0;
	ship->vYpos = 0;
	STRDUP(ship->sXpos, "");
	STRDUP(ship->sYpos, "");
	ship->hatchopen = false;
	ship->missiles = ship->maxmissiles;
	ship->torpedos = ship->maxtorpedos;
	ship->rockets = ship->maxrockets;
	ship->autorecharge = false;
	ship->autotrack = false;
	ship->autospeed = false;
	ship->efflux = 0;		// Pixel
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
		STRDUP(ship->engineer, "");
		ship->sx = 0;
		ship->sy = 0;
		ship->sz = 0;
	}
	// mobshipy
	if (ship->type == SHIP_REPUBLIC
			&& (ship->type == MOB_SHIP
					&& !str_cmp(ship->owner, "Nowa Republika")))
		STRDUP(ship->home, "coruscant");
	else if (ship->type == SHIP_IMPERIAL
			&& (ship->type == MOB_SHIP && !str_cmp(ship->owner, "Imperium")))
		STRDUP(ship->home, "kuat");
	else if (ship->type == SHIP_CIVILIAN)
		STRDUP(ship->home, "corperate");

	STRDUP(ship->ship_title, ship->name);

	for (auto* turret : ship->turrets)
	{
		turret->gunner = NULL;
		turret->target = NULL;
		turret->status = 0;
	}
	for (auto* hangar : ship->hangars)
	{
		hangar->status = -1;
	}
	for (auto* dock : ship->docks)
	{
		if (dock->master_slave == 0)
		{
			targetdock = shipdock_from_room(dock->target, dock->targetvnum);
			targetdock->targetvnum = 0;
			targetdock->target = NULL;
			targetdock->master_slave = -1;
			dock->targetvnum = 0;
			dock->target = NULL;
			dock->master_slave = -1;
		}
		if (dock->master_slave == 1)
		{
			targetdock = shipdock_from_room(dock->target, dock->targetvnum);
			targetdock->targetvnum = 0;
			targetdock->target = NULL;
			targetdock->master_slave = -1;
			dock->target->shipstate = SHIP_READY;
			dock->targetvnum = 0;
			dock->target = NULL;
			dock->master_slave = -1;
		}
		dock->status = 0;
		targetdock = 0;
	}
	clear_transponders(ship);
	generate_transponder(ship);
	save_ship(ship);
}

DEF_DO_FUN( resetship )
{
	SHIP_DATA *ship;

	ship = get_ship(argument);
	if (ship == NULL)
	{
		send_to_char(FB_RED "No such ship!", ch);
		return;
	}

	resetship(ship);

	if ((is_platform(ship) || ship->type == MOB_SHIP || is_huge(ship)
			|| is_capital(ship)) && ship->home)
	{
		extract_ship(ship);
		ship_to_starsystem(ship, starsystem_from_name(ship->home));
		if (ship->sx != 0 || ship->sx != 0 || ship->sx != 0)
		{
			ship->vx = ship->sx;
			ship->vy = ship->sy;
			ship->vz = ship->sz;
		}
		else
		{
			ship->vx = number_range(1000, 10000);
			ship->vy = number_range(1000, 10000);
			ship->vz = number_range(-10000, -1000);
		}
		ship->shipstate = SHIP_READY;
		ship->autopilot = true;
		ship->autorecharge = true;
		ship->shield = ship->maxshield;
	}
}

void do_describe_ship(CHAR_DATA *ch, char *argument)	//byTrog 4 Arian
{
	SHIP_DATA *ship;
	char arg[MAX_STRING_LENGTH];

	argument = one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		ch_printf(ch, "Sk�adnia: describeship | dsh <akcja> <statek>" NL
		NL "Akcje: clear save set show" NL
		NL "clear - czy�ci opis statku,"
		NL "save  - zapisuje opis statku,"
		NL "set   - ustawia opis statku (w��cza edytor),"
		NL "show  - wy�wietla opis statku.," NL, ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char("Musisz poda� nazw� statku." NL, ch);
		return;
	}

	if ((ship = get_ship(argument)) == NULL)
	{
		send_to_char("Nie ma takiego statku." NL, ch);
		return;
	}

	if (!str_cmp(arg, "show"))
	{
		if (*ship->description)
			ch_printf(ch, "Opis statku" MOD_BOLD " %s " PLAIN ":" NL
			"%s" PLAIN, SHIPNAME(ship), ship->description);
		return;
	}

	if (!str_cmp(arg, "save"))
	{
		ch_printf(ch, "Opis statku" MOD_BOLD " %s " PLAIN "zapisany." NL,
				SHIPNAME(ship));
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg, "set"))
	{
		string_append(ch, &ship->description);
		return;
	}

	if (!str_cmp(arg, "clear"))
	{
		STRDUP(ship->description, "");
		save_ship(ship);
		return;
	}

	do_describe_ship(ch, (char*) "");
	return;
}

DEF_DO_FUN( setship )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	char arg4[MAX_INPUT_LENGTH];
	char arg5[MAX_INPUT_LENGTH];
	SHIP_DATA *ship;
	SHIPDOCK_DATA *dock;
	TURRET_DATA *turret;
	HANGAR_DATA *hangar;
	int tempnum;
	ROOM_INDEX_DATA *roomindex;
	char filename[256];

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);

	if (arg1[0] == '\0' || arg2[0] == '\0' || arg1[0] == '\0')
	{
		send_to_char("Sk�adnia: setship <statek> <pole> <warto�ci>" NL, ch);
		send_to_char(NL "Gdzie polem mo�e by�:" NL, ch);
		send_to_char(FG_CYAN "filename name title desc size" NL, ch);
		send_to_char(PLAIN "owner pilot copilot engineer" NL, ch);
		send_to_char(FG_CYAN "home shipyard type class classname flags" NL, ch);
		send_to_char(
				PLAIN "firstroom lastroom cockpit entrance pilotseat coseat navseat" NL,
				ch);
		send_to_char(FG_CYAN "gunseat engineroom " NL, ch);
		send_to_char(PLAIN "turret hangar dock -> editable" NL, ch);
		send_to_char(FG_CYAN "manuever speed hyperspeed shield hull energy" NL,
				ch);
		send_to_char(PLAIN "sensor comm targetarray astroarray" NL, ch);
		send_to_char(FG_CYAN "lasers ioncannons missiles torpedos rockets" NL,
				ch);
		send_to_char(PLAIN "chaff cloack interdict trawler" NL, ch);
		send_to_char(FG_CYAN "tractorbeam maxcargo maxmodule" NL, ch);
		send_to_char(PLAIN "lock_key level public pubsubcl" NL, ch);
		send_to_char(FG_CYAN "destroy purge version" NL, ch);
		return;
	}

	ship = get_ship(arg1);
	if (!str_cmp(arg1, "all ships on mud"))
	{
		setallships(ch, arg2, argument);
		return;
	}
	if (!ship)
	{
		send_to_char("No such ship." NL, ch);
		return;
	}
	if (!str_cmp(arg2, "owner"))
	{
		CLAN_DATA *clan;
		if (ship->type != MOB_SHIP && (clan = get_clan(ship->owner)) != NULL)
		{
			if (is_ship(ship))
				clan->spacecrafts--;
			else
				clan->vehicles--;
		}
		STRDUP(ship->owner, argument);
		donemsg(ch);
		save_ship(ship);
		if (ship->type != MOB_SHIP && (clan = get_clan(ship->owner)) != NULL)
		{
			if (is_ship(ship))
				clan->spacecrafts++;
			else
				clan->vehicles++;
		}
		return;
	}

	if (!str_cmp(arg2, "classname"))
	{
		if ((tempnum = flag_value(ship_classes_list, argument)) != -1)
		{
			ship->clazz = tempnum;
			ch_printf(ch, "Ship class set to %s." NL,
					bit_name(ship_classes_list, tempnum));
			save_ship(ship);
			return;
		}
	}

	if (!str_cmp(arg2, "flags"))
	{
		char farg[MAX_INPUT_LENGTH];

		argument = one_argument(argument, farg);

		if (farg[0] == '\0')
		{
			send_to_char(
					"Mo�liwe flagi: metaaggressive aggressive nowander wimpy hidden descless done"
					NL, ch);
			return;
		}

		for (; farg[0] != '\0'; argument = one_argument(argument, farg))
		{
			if (!str_cmp(farg, "aggressive"))
				TOGGLE_BIT(ship->flags, SHIP_AGGRESSIVE);
			else if (!str_cmp(farg, "hidden"))
				TOGGLE_BIT(ship->flags, SHIP_HIDDEN);
			else if (!str_cmp(farg, "nowander"))
				TOGGLE_BIT(ship->flags, SHIP_NOWANDER);
			else if (!str_cmp(farg, "metaaggressive"))
				TOGGLE_BIT(ship->flags, SHIP_METAAGGRESSIVE);
			else if (!str_cmp(farg, "wimpy"))
				TOGGLE_BIT(ship->flags, SHIP_WIMPY);
			else if (!str_cmp(farg, "descless"))
				TOGGLE_BIT(ship->flags, SHIP_DESCLESS);
			else if (!str_cmp(farg, "done"))
				TOGGLE_BIT(ship->flags, SHIP_DONE);
			else
				ch_printf(ch, "No such flag: %s" NL, farg);
		}
		save_ship(ship);
		donemsg(ch);
		return;
	}

	if (!str_cmp(arg2, "home"))
	{
		STRDUP(ship->home, argument);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "lock_key"))
	{
		char digit1[MIL] =
		{ 0 };
		char digit2[MIL] =
		{ 0 };
		char digit3[MIL] =
		{ 0 };
		char digit4[MIL] =
		{ 0 };
		char code[MIL] =
		{ 0 };

		argument = one_argument(argument, digit1);
		argument = one_argument(argument, digit2);
		argument = one_argument(argument, digit3);
		argument = one_argument(argument, digit4);

		if (str_cmp(argument, ""))
		{
			send_to_char("1. kod musi byc 4 cyfrowy." EOL, ch);
			return;
		}
		if (!is_number(digit1) || !is_number(digit2) || !is_number(digit3)
				|| !is_number(digit4))
		{
			send_to_char("2. kod musi byc 4 cyfrowy." EOL, ch);
			return;
		}
		if (atoi(digit1) > 9 || atoi(digit1) < 0 || atoi(digit2) > 9
				|| atoi(digit2) < 0 || atoi(digit3) > 9 || atoi(digit3) < 0
				|| atoi(digit4) > 9 || atoi(digit4) < 0)
		{
			send_to_char("3. kod musi byc 4 cyfrowy." EOL, ch);
			return;
		}
		swsnprintf(code, MIL, "%s%s%s%s", digit1, digit2, digit3, digit4);
		STRDUP(ship->lock_key, code);
		send_to_char("Done." EOL, ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "destroy"))
	{
		if (str_cmp(argument, "yes"))
		{
			send_to_char(FB_RED
			"Chcesz zniszczyc ten statek - wpisz 'yes' na koncu."
			NL, ch);
			return;
		}
		destroy_ship(ship, NULL);
		return;
	}

	if (!str_cmp(arg2, "purge"))
	{
		if (str_cmp(argument, "yes"))
		{
			send_to_char(FB_RED
			"Chcesz zniszczyc ten statek - wpisz 'yes' na koncu."
			NL, ch);
			return;
		}
		purge_ship(ship, NULL);
		return;
	}

	// Kasowanie pliku ze statkiem, calkowite "niszczenie" -- Pixel
	if (!str_cmp(arg2, "delete"))
	{
		char filename[MAX_STRING_LENGTH];

		if (str_cmp(argument, "yes"))
		{
			send_to_char(FB_RED
			"Chcesz skasowa� ten statek - wpisz 'yes' na ko�cu."
			NL, ch);
			return;
		}

		purge_ship(ship, NULL);
		snprintf(filename, sizeof(filename), "%s%s", SHIP_DIR, ship->filename);
		unlink(filename);
		remove_ship_from_list(ship->filename);
		free_ship(ship);
		write_ship_list();
		send_to_char(FB_RED "Skasowany!" NL, ch);
		return;
	}

	if (!str_cmp(arg2, "pilot"))
	{
		STRDUP(ship->pilot, argument);
		donemsg(ch);
		save_ship(ship);
		return;
	}
	//added by Thanos
	if (!str_cmp(arg2, "public"))
	{
		if (!str_cmp(argument, "yes") || !str_cmp(argument, "on"))
		{
			ship->ship_public = true;
			ship->public_subclass = 1;
			send_to_char(
					"Done. You must now to set the course of the ship using 'setcourse' command."
					NL, ch);
			save_ship(ship);
			return;
		}
		if (!str_cmp(argument, "no") || !str_cmp(argument, "off"))
		{
			char filename[MAX_STRING_LENGTH];
			char bkpfile[MAX_STRING_LENGTH];

			snprintf(filename, sizeof(filename), "%s%s.course", SPACE_DIR, ship->filename);
			sprintf(bkpfile, "%s%s.course.bkp", SPACE_DIR, ship->filename);
			if (!rename(filename, bkpfile))
				ch_printf(ch, "File backed up as %s" NL, bkpfile);
			else
				send_to_char("Couldn't make a backup of course-file. Sorry."
				NL, ch);

			send_to_char("Saving bus list." NL, ch);
			do_setcourse(ch, (char*) "savelist");
			send_to_char("Docking ship." NL, ch);
			ship->location = ship->lastdoc;
			if (ship->shipstate != SHIP_DISABLED)
				ship->shipstate = SHIP_DOCKED;
			ship_from_starsystem(ship, ship->starsystem);
			ship_to_room(ship, ship->location);
			ch_printf(ch, "Ship docked in its last-dock {%d}." NL,
					ship->location);
			send_to_char("Cleaning and freeing course." NL, ch);
			ship->ship_public = false;
			ship->bus_pos = 0;
			free_courses(ship);
			save_ship(ship);
			ch_printf(ch, "Done. Ship saved." NL);
			return;
		}
		send_to_char("setship <ship> public YES or NO ???" NL, ch);
		return;
	}
	//done
	if (!str_cmp(arg2, "copilot"))
	{
		STRDUP(ship->copilot, argument);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "engineer"))
	{
		STRDUP(ship->engineer, argument);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "firstroom"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist." NL, ch);
			return;
		}
		ship->firstroom = tempnum;
		ship->lastroom = tempnum;
		ship->cockpit = get_room_index(tempnum);
		ship->coseat = get_room_index(tempnum);
		ship->pilotseat = get_room_index(tempnum);
		ship->gunseat = get_room_index(tempnum);
		ship->navseat = get_room_index(tempnum);
		ship->entrance = get_room_index(tempnum);
		send_to_char("You will now need to set the other rooms in the ship."
		NL, ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "lastroom"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist." NL, ch);
			return;
		}
		if (tempnum < ship->firstroom)
		{
			send_to_char(
					"The last room on a ship must be greater than or equal to the first room."
					NL, ch);
			return;
		}
		if ((is_scout(ship) || is_fighter(ship))
				&& (tempnum - ship->firstroom) > 10)
		{
			send_to_char("Starfighters may have up to 10 rooms only." NL, ch);
			return;
		}
		if (is_midship(ship) && (tempnum - ship->firstroom) > 50)
		{
			send_to_char("Midships may have up to 50 rooms only." NL, ch);
			return;
		}
		if (is_capital(ship) && (tempnum - ship->firstroom) > 300)
		{
			send_to_char("Capital Ships may have up to 300 rooms only." NL, ch);
			return;
		}
		ship->lastroom = tempnum;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	/*rekawy dokujace Aldegard*/
	if (!str_cmp(arg2, "dock"))
	{
		argument = one_argument(argument, arg3);
		argument = one_argument(argument, arg4);

		if (!str_cmp(arg3, "add"))
		{
			if (atoi(arg4) < ship->firstroom || atoi(arg4) > ship->lastroom)
			{
				send_to_char("Shipdock must be on ship", ch);
				return;
			}
			if (is_vnum_ship_system(ship, atoi(arg4), "dock"))
			{
				send_to_char(
						"That room is already being used by another part of the ship"
						NL, ch);
				return;
			}
			CREATE(dock, SHIPDOCK_DATA, 1);
			dock->vnum = atoi(arg4);
			dock->type = atoi(argument);
			dock->target = NULL;
			dock->targetvnum = 0;
			dock->master_slave = -1;
			ship->docks.push_back(dock);
		}
		else if (!str_cmp(arg3, "edit"))
		{
			int nr;

			nr = atoi(arg4);
			argument = one_argument(argument, arg5);

			if ((dock = get_shipdock(ship, nr)) == NULL)
			{
				send_to_char("No such dock." NL, ch);
				return;
			}

			if (!str_cmp(arg5, "type"))
			{
				dock->type = atoi(argument);
			}
			else if (!str_cmp(arg5, "vnum"))
			{
				if (atoi(argument) < ship->firstroom
						|| atoi(argument) > ship->lastroom)
				{
					send_to_char("Shipdock must be on ship", ch);
					return;
				}
				if (is_vnum_ship_system(ship, atoi(argument), "dock"))
				{
					send_to_char(
							"That room is already being used by another part of the ship"
							NL, ch);
					return;
				}
				dock->vnum = atoi(argument);
			}
		}
		else if (!str_cmp(arg3, "delete"))
		{
			int nr;

			nr = atoi(arg4);

			if ((dock = get_shipdock(ship, nr)) == NULL)
			{
				send_to_char("No such dock." NL, ch);
				return;
			}

			ship->docks.remove(dock);
			DISPOSE(dock);
		}
		else
		{
			do_setship(ch, (char*) "");
			return;
		}
		save_ship(ship);
		send_to_char("Done.", ch);
		return;
	}

	/*nowy zapis wieZyczek Aldegard -- ort, Pixel :)*/
	if (!str_cmp(arg2, "turret"))
	{
		argument = one_argument(argument, arg3);
		argument = one_argument(argument, arg4);

		if (!str_cmp(arg3, "add"))
		{
			int val;

			if (atoi(arg4) < ship->firstroom || atoi(arg4) > ship->lastroom)
			{
				send_to_char("Turret must be on ship", ch);
				return;
			}
			if (is_vnum_ship_system(ship, atoi(arg4), "turret"))
			{
				send_to_char(
						"That room is already being used by another part of the ship" NL,
						ch);
				return;
			}

			!str_cmp(argument, "laser") ? val = 1 :
			!str_cmp(argument, "double") ? val = 2 :
			!str_cmp(argument, "triple") ? val = 3 :
			!str_cmp(argument, "quad") ? val = 4 :
			!str_cmp(argument, "turbo") ? val = 5 :
			!str_cmp(argument, "htrubo") ? val = 6 :
			!str_cmp(argument, "super") ? val = 7 : val = atoi(argument);

			if (val < 1 || val > 7)
			{
				send_to_char("Turret type must be in range <1;7> or one of:" NL
				"laser, double, triple, quad, trubo, htrubo, super" NL, ch);
				return;
			}

			CREATE(turret, TURRET_DATA, 1);
			turret->vnum = atoi(arg4);
			turret->type = val;
			turret->status = 0;
			ship->turrets.push_back(turret);
		}
		else if (!str_cmp(arg3, "edit"))
		{
			int nr;
			int val;

			nr = atoi(arg4);
			argument = one_argument(argument, arg5);
			!str_cmp(argument, "laser") ? val = 1 :
			!str_cmp(argument, "double") ? val = 2 :
			!str_cmp(argument, "triple") ? val = 3 :
			!str_cmp(argument, "quad") ? val = 4 :
			!str_cmp(argument, "turbo") ? val = 5 :
			!str_cmp(argument, "htrubo") ? val = 6 :
			!str_cmp(argument, "super") ? val = 7 : val = atoi(argument);

			if ((turret = get_turret(ship, nr)) == NULL)
			{
				send_to_char("No such turret." NL, ch);
				return;
			}

			if (!str_cmp(arg5, "type"))
			{
				if (val < 1 || val > 7)
				{
					send_to_char(
							"Turret type must be in range <1;7> or one of:" NL
							"laser, double, triple, quad, turbo, hturbo, super" NL,
							ch);
					return;
				}

				turret->type = val;
			}
			else if (!str_cmp(arg5, "vnum"))
			{
				if (val < ship->firstroom || val > ship->lastroom)
				{
					send_to_char("Turret must be on ship", ch);
					return;
				}
				if (is_vnum_ship_system(ship, atoi(argument), "turret"))
				{
					send_to_char(
							"That room is already being used by another part of the ship"
							NL, ch);
					return;
				}
				turret->vnum = val;
			}
			else if (!str_cmp(arg5, "status"))
			{
				if (val < -1 || val > 1)
				{
					send_to_char("Turret status must be in range <-1;1>" NL,
							ch);
					return;
				}

				turret->status = val;
			}
			else
			{
				do_setship(ch, (char*) "a turret");
				return;
			}
		}
		else if (!str_cmp(arg3, "delete"))
		{
			int nr;

			nr = atoi(arg4);

			if ((turret = get_turret(ship, nr)) == NULL)
			{
				send_to_char("No such turret." NL, ch);
				return;
			}

			ship->turrets.remove(turret);
			DISPOSE(turret);
		}
		else
		{
			send_to_char("Sytnax: setship <ship> turret add <vnum>" NL
			"        setship <ship> turret edit <nr> vnum <vnum>" NL
			"        setship <ship> turret edit <nr> type <type>" NL
			"        setship <ship> turret edit <nr> status <status>" NL
			"        setship <ship> turret delete <nr>" NL, ch);
			return;
		}
		save_ship(ship);
		donemsg(ch);
		return;
	}

	/*nowy zapis hangar�w Aldegard*/
	if (!str_cmp(arg2, "hangar"))
	{
		argument = one_argument(argument, arg3);
		argument = one_argument(argument, arg4);

		if (!str_cmp(arg3, "add"))
		{
			if (atoi(arg4) < ship->firstroom || atoi(arg4) > ship->lastroom)
			{
				send_to_char("Hangar must be on ship" NL, ch);
				return;
			}
			if (is_vnum_ship_system(ship, atoi(arg4), "hangar"))
			{
				send_to_char(
						"That room is already being used by another part of the ship"
						NL, ch);
				return;
			}
			CREATE(hangar, HANGAR_DATA, 1);
			hangar->vnum = atoi(arg4);
			hangar->capacity = 10;
			hangar->status = 0;
			ship->hangars.push_back(hangar);
		}
		else if (!str_cmp(arg3, "edit"))
		{
			int nr;

			nr = atoi(arg4);
			argument = one_argument(argument, arg5);

			if ((hangar = get_hangar(ship, nr)) == NULL)
			{
				send_to_char("No such hangar." NL, ch);
				return;
			}
			if (!str_cmp(arg5, "capacity"))
			{
				hangar->capacity = URANGE(0, atoi(argument), 1000);
			}
			if (!str_cmp(arg5, "vnum"))
			{
				if (atoi(argument) < ship->firstroom
						|| atoi(argument) > ship->lastroom)
				{
					send_to_char("Hangar must be on ship", ch);
					return;
				}
				if (is_vnum_ship_system(ship, atoi(argument), "hangar"))
				{
					send_to_char(
							"That room is already being used by another part of the ship"
							NL, ch);
					return;
				}
				hangar->vnum = atoi(argument);
			}
		}
		else if (!str_cmp(arg3, "delete"))
		{
			int nr;

			nr = atoi(arg4);

			if ((hangar = get_hangar(ship, nr)) == NULL)
			{
				send_to_char("No such hangar." NL, ch);
				return;
			}

			ship->hangars.remove(hangar);
			DISPOSE(hangar);
		}
		else
		{
			do_setship(ch, (char*) "");
			return;
		}
		save_ship(ship);
		send_to_char("Done.", ch);
		return;
	}	// poprawke trzeba zrobic do syntaxa

	if (!str_cmp(arg2, "cockpit"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist." NL, ch);
			return;
		}
		if (tempnum < ship->firstroom || tempnum > ship->lastroom)
		{
			send_to_char("That room number is not in that ship .. " NL
			"It must be between Firstroom and Lastroom." NL, ch);
			return;
		}
		if (is_vnum_ship_system(ship, tempnum, "cockpit"))
		{
			send_to_char(
					"That room is already being used by another part of the ship"
					NL, ch);
			return;
		}
		ship->cockpit = roomindex;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "pilotseat"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist." NL, ch);
			return;
		}
		if (tempnum < ship->firstroom || tempnum > ship->lastroom)
		{
			send_to_char("That room number is not in that ship .. " NL
			"It must be between Firstroom and Lastroom." NL, ch);
			return;
		}
		if (is_vnum_ship_system(ship, tempnum, "pilotseat"))
		{
			send_to_char(
					"That room is already being used by another part of the ship"
					NL, ch);
			return;
		}
		ship->pilotseat = roomindex;
		donemsg(ch);
		save_ship(ship);
		return;
	}
	if (!str_cmp(arg2, "coseat"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist." NL, ch);
			return;
		}
		if (tempnum < ship->firstroom || tempnum > ship->lastroom)
		{
			send_to_char("That room number is not in that ship .. " NL
			"It must be between Firstroom and Lastroom." NL, ch);
			return;
		}
		if (is_vnum_ship_system(ship, tempnum, "coseat"))
		{
			send_to_char(
					"That room is already being used by another part of the ship"
					NL, ch);
			return;
		}
		ship->coseat = roomindex;
		donemsg(ch);
		save_ship(ship);
		return;
	}
	if (!str_cmp(arg2, "navseat"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist." NL, ch);
			return;
		}
		if (tempnum < ship->firstroom || tempnum > ship->lastroom)
		{
			send_to_char("That room number is not in that ship .. " NL
			"It must be between Firstroom and Lastroom." NL, ch);
			return;
		}
		if (is_vnum_ship_system(ship, tempnum, "navseat"))
		{
			send_to_char(
					"That room is already being used by another part of the ship"
					NL, ch);
			return;
		}
		ship->navseat = roomindex;
		donemsg(ch);
		save_ship(ship);
		return;
	}
	if (!str_cmp(arg2, "gunseat"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist." NL, ch);
			return;
		}
		if (tempnum < ship->firstroom || tempnum > ship->lastroom)
		{
			send_to_char("That room number is not in that ship .. " NL
			"It must be between Firstroom and Lastroom." NL, ch);
			return;
		}
		if (is_vnum_ship_system(ship, tempnum, "gunseat"))
		{
			send_to_char(
					"That room is already being used by another part of the ship"
					NL, ch);
			return;
		}
		ship->gunseat = roomindex;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "entrance"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist." NL, ch);
			return;
		}
		if (tempnum < ship->firstroom || tempnum > ship->lastroom)
		{
			send_to_char("That room number is not in that ship .. " NL
			"It must be between Firstroom and Lastroom." NL, ch);
			return;
		}
		if (is_vnum_ship_system(ship, tempnum, "entrance"))
		{
			send_to_char(
					"That room is already being used by another part of the ship"
					NL, ch);
			return;
		}
		ship->entrance = roomindex;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "engineroom"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			ship->engineroom = roomindex;
			donemsg(ch);
			save_ship(ship);
			return;
		}
		if (tempnum < ship->firstroom || tempnum > ship->lastroom)
		{
			send_to_char("That room number is not in that ship .. " NL
			"It must be between Firstroom and Lastroom." NL, ch);
			return;
		}

		/* nie widze przeciwskazan
		 if(is_vnum_ship_system(ship, tempnum, "engineroom"))
		 {
		 send_to_char
		 ("That room is already being used by another part of the ship"
		 NL, ch);
		 return;
		 }
		 */

		ship->engineroom = roomindex;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "shipyard"))
	{
		tempnum = atoi(argument);
		roomindex = get_room_index(tempnum);
		if (roomindex == NULL)
		{
			send_to_char("That room doesn't exist.", ch);
			return;
		}
		ship->shipyard = roomindex;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "type"))
	{
		if (!str_cmp(argument, "republic"))
			ship->type = SHIP_REPUBLIC;
		else if (!str_cmp(argument, "imperial"))
			ship->type = SHIP_IMPERIAL;
		else if (!str_cmp(argument, "civilian"))
			ship->type = SHIP_CIVILIAN;
		else if (!str_cmp(argument, "yuuzhan"))
			ship->type = SHIP_YUUZHAN;
		else if (!str_cmp(argument, "pirate"))
			ship->type = SHIP_PIRATE;
		else if (!str_cmp(argument, "mob"))
			ship->type = MOB_SHIP;
		else
		{
			send_to_char(
					"Ship type must be either: republic, imperial, civilian pirate yuuzhan or mob."
					NL, ch);
			return;
		}
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "name"))
	{
		STRDUP(ship->name, argument);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "filename"))
	{
		for (auto* target : ship_list)
		{
			if (!str_cmp(target->filename, argument))
			{
				send_to_char(FB_RED "EEe jest juz statek z takim filename."
				NL, ch);
				return;
			}
		}
		if (ship->filename)
		{
			snprintf(filename, sizeof(filename), "%s%s", SHIP_DIR, ship->filename);
			unlink(filename);
			send_to_char(FB_RED "Old file deleted." NL "New created.", ch);
		}
		STRDUP(ship->filename, argument);
		send_to_char("All Done." NL, ch);
		save_ship(ship);
		write_ship_list();
		return;
	}

	if (!str_cmp(arg2, "desc"))
	{
		if (!str_cmp(argument, "save"))
		{
			save_ship(ship);
			send_to_char("Done" NL, ch);
			return;
		}
		if (!str_cmp(argument, "clear"))
		{
			STRDUP(ship->description, "");
			send_to_char("Done" NL, ch);
			save_ship(ship);
			return;
		}
		string_append(ch, &ship->description);
		return;
	}

	if (!str_cmp(arg2, "title"))
	{
		STRDUP(ship->ship_title, argument);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "manuever"))
	{
		ship->manuever = URANGE(0, atoi(argument), 100);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "maxmodule"))
	{
		ship->maxmodule = URANGE(0, atoi(argument), 1000);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "lasers"))
	{
		ship->lasers = URANGE(0, atoi(argument), 30);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "size"))
	{
		ship->size = URANGE(1, atoi(argument), 2500);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "ioncannons"))
	{
		ship->ioncannons = URANGE(0, atoi(argument), 10);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "class"))
	{
		ship->clazz = URANGE(0, atoi(argument), 180);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "missiles"))
	{
		ship->maxmissiles = URANGE(0, atoi(argument), 255);
		ship->missiles = URANGE(0, atoi(argument), 255);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "torpedos"))
	{
		ship->maxtorpedos = URANGE(0, atoi(argument), 255);
		ship->torpedos = URANGE(0, atoi(argument), 255);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "rockets"))
	{
		ship->maxrockets = URANGE(0, atoi(argument), 255);
		ship->rockets = URANGE(0, atoi(argument), 255);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "speed"))
	{
		ship->realspeed = URANGE(0, atoi(argument), 500);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "tractorbeam"))
	{
		ship->tractorbeam = URANGE(0, atoi(argument), 100);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "hyperspeed"))
	{
		ship->hyperspeed = URANGE(0, atoi(argument), 500);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "shield"))
	{
		ship->maxshield = URANGE(0, atoi(argument), 50000);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "hull"))
	{
		ship->hull = URANGE(1, atoi(argument), 2000000);
		ship->maxhull = URANGE(1, atoi(argument), 2000000);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "energy"))
	{
		ship->energy = URANGE(1, atoi(argument), 300000);
		ship->maxenergy = URANGE(1, atoi(argument), 300000);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "sensor"))
	{
		ship->sensor = URANGE(1, atoi(argument), 150);
		if (ship->target_array > ship->sensor)
			ship->target_array = ship->sensor;
		if (ship->astro_array > ship->sensor)
			ship->astro_array = ship->sensor;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "targetarray"))
	{
		ship->target_array = URANGE(0, atoi(argument), 150);
		if (ship->target_array > ship->sensor)
			ship->target_array = ship->sensor;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "astroarray"))
	{
		ship->astro_array = URANGE(1, atoi(argument), 150);
		if (ship->astro_array > ship->sensor)
			ship->astro_array = ship->sensor;
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "interdict"))
	{
		ship->maxinterdict = URANGE(0, atoi(argument), 1);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "cloack"))
	{
		ship->maxcloack = URANGE(0, atoi(argument), 1);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "comm"))
	{
		ship->comm = URANGE(1, atoi(argument), 100);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "chaff"))
	{
		ship->chaff = URANGE(0, atoi(argument), 50);
		ship->maxchaff = URANGE(0, atoi(argument), 50);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "maxcargo"))
	{
		ship->maxcargo = URANGE(0, atoi(argument), 25000000);
		if (ship->maxcargo == 0 && ship->cargo > 0)
		{
			ship->cargo = 0;
			STRDUP(ship->cargofrom, "");
			ship->cargotype = 0;
		}
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "trawler") || !str_cmp(arg2, "traw"))
	{
		ship->trawler = URANGE(0, atoi(argument), 1);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "pubsubcl"))
	{
		ship->public_subclass = URANGE(0, atoi(argument), 3);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "version"))
	{
		ship->version = atoi(argument);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(arg2, "level"))
	{
		ship->level = atoi(argument);
		donemsg(ch);
		save_ship(ship);
		return;
	}

	do_setship(ch, (char*) "");
	return;
}

DEF_DO_FUN( showship )
{
	SHIP_DATA *ship;
	int a;

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char("Usage: showship <ship>" NL, ch);
		return;
	}

	ship = get_ship(argument);
	if (!ship)
	{
		char arg1[MSL];

		argument = one_argument(argument, arg1);
		if (!str_cmp("vnum", arg1))
			ship = ship_from_room(get_room_index(atoi(argument)));
		if (!ship)
		{
			send_to_char("No such ship." NL, ch);
			return;
		}
	}
	pager_printf(ch,
			FB_WHITE "Vnum: %d" FB_YELLOW " %s" FB_RED " %s " FB_YELLOW "(" FB_RED "%d" FB_YELLOW "): " FB_RED "%s" EOL FB_YELLOW
			"Filename: " FG_YELLOW "%s" EOL, ship->vnum,
			bit_name(ship_types_list, ship->type),
			bit_name(ship_classes_list, ship->clazz), ship->clazz, ship->name,
			ship->filename);
	pager_printf(ch, FB_YELLOW "Flags: " FB_WHITE "%s" EOL,
			ship->flags > 0 ?
					flag_string(ship_flags_list, ship->flags) : "none");
	pager_printf(ch,
	FB_YELLOW "Size: " FG_YELLOW "%.0f          " FB_YELLOW
	"Ionize: " FG_YELLOW "%d     ", ship->size, ship->shipstate1);
	pager_printf(ch,
	FB_YELLOW "LEVEL: " FG_YELLOW "%d          " FB_YELLOW
	"Version: " FG_YELLOW "%d" EOL, ship->level, ship->version);
	pager_printf(ch,
	FB_YELLOW "Home: " FG_YELLOW "%s   " FB_YELLOW "System: "
	FG_YELLOW "%s" EOL FB_GREEN "Owner:   " FG_GREEN "%-20s "
	FB_GREEN "Pilot: " FG_GREEN "%s   " EOL FB_GREEN
	"Copilot: " FG_GREEN "%-20s " FB_GREEN "Engineer: "
	FG_YELLOW "%s" EOL, ship->home,
			ship->starsystem ? ship->starsystem->name : "", ship->owner,
			ship->pilot, ship->copilot, ship->engineer);
	pager_printf(ch, FB_YELLOW "Title: " FG_YELLOW "%s" EOL, ship->ship_title);
	pager_printf(ch,
	FB_RED "Firstroom: " FG_RED "%d   " FB_RED
	"Lastroom: " FG_RED "%d " EOL, ship->firstroom, ship->lastroom);
	pager_printf(ch,
	FB_YELLOW "Entrance:   " FG_YELLOW "%-9d  " FB_YELLOW "key:"
	FG_YELLOW " %s " EOL FB_GREEN "Engineroom: " FG_GREEN
	"%-8d   ", VNUM(ship->entrance), ship->lock_key, VNUM(ship->engineroom));
	pager_printf(ch,
	FB_GREEN "Cockpit:    " FG_GREEN "%-8d    " FB_GREEN
	"Pilotseat:  " FG_GREEN "%-8d" EOL FB_GREEN "Coseat:     "
	FG_GREEN "%-8d   " FB_GREEN "Navseat:    " FG_GREEN
	"%-8d    " FB_GREEN "Gunseat:    " FG_GREEN "%-8d" EOL, VNUM(ship->cockpit),
			VNUM(ship->pilotseat), VNUM(ship->coseat), VNUM(ship->navseat),
			VNUM(ship->gunseat));
	pager_printf(ch,
	FB_YELLOW "Location: " FG_YELLOW "%d   " FB_YELLOW "Lastdoc: "
	FG_YELLOW "%d   " FB_YELLOW "Shipyard: " FG_YELLOW "%d   " EOL,
			VNUM(ship->location), VNUM(ship->lastdoc), VNUM(ship->shipyard));
	pager_printf(ch,
			FB_YELLOW
			"Tractor Beam: " FG_YELLOW "%d   " FB_YELLOW "Comm: " FG_YELLOW "%d   " FB_YELLOW
			"Sensor: " FG_YELLOW "%d   " FB_YELLOW "Astro Array: " FG_YELLOW "%d " FB_YELLOW
			"Target Array: " FG_YELLOW "%d"
			EOL, ship->tractorbeam, ship->comm, ship->sensor, ship->astro_array,
			ship->target_array);
	pager_printf(ch,
	FB_YELLOW
	"Lasers: " FG_YELLOW "%d %d " FB_YELLOW "Laser Condition: " FG_YELLOW
	"%s     " FB_YELLOW "/ Ioncannons: " FG_YELLOW "%d %d"
	EOL, ship->lasers, count_modules(ship, 0),
			ship->statet0 == LASER_DAMAGED ? "Damaged" : "Good",
			ship->ioncannons, count_modules(ship, 1));
	pager_printf(ch,
			FB_GREEN
			"Missiles: " FG_GREEN "%d" FB_GREEN "/" FG_GREEN "%d  " FB_YELLOW
			"Torpedos: " FG_YELLOW "%d" FB_YELLOW "/" FG_YELLOW "%d  " FB_RED
			"Rockets: " FG_RED "%d" FB_RED "/" FG_RED "%d  " FB_YELLOW "Condition: " FG_YELLOW "%s"
			EOL, ship->missiles, ship->maxmissiles, ship->torpedos,
			ship->maxtorpedos, ship->rockets, ship->maxrockets,
			ship->missilestate == MISSILE_DAMAGED ? "Damaged" : "Good");
	pager_printf(ch, FB_RED "Turrets:" EOL);
	pager_printf(ch, FB_CYAN "Num Vnum     Type    Status  Gunner" EOL);
	a = 0;
	for (auto* turret : ship->turrets)
	{
		pager_printf(ch, FB_CYAN "%-3d " FB_YELLOW "%-8d %s %-7d %s" EOL, a,
				turret->vnum,
				turret->type == 1 ? "laser  " : turret->type == 2 ? "double " :
				turret->type == 3 ? "triple " : turret->type == 4 ? "quad   " :
				turret->type == 5 ? "turbo  " : turret->type == 6 ? "hturbo " :
				turret->type == 7 ? "super  " : "bug!!! ", turret->status,
				turret->gunner ? turret->gunner->name : "-");
		a++;
	}
	pager_printf(ch, FB_RED "Hangars:" EOL);
	pager_printf(ch, FB_CYAN "Num Vnum           Status  Capacity  Type" EOL);
	a = 0;
	for (auto* hangar : ship->hangars)
	{
		pager_printf(ch,
		FB_CYAN "%-3d " FB_YELLOW "%-14d %-7d %.0f/%-6.0f %d"
		EOL, a, hangar->vnum, hangar->status, check_capacity(hangar->vnum),
				hangar->capacity, hangar->type);
		a++;
	}
	pager_printf(ch, FB_RED "Docks:		[efflux: %d]" EOL, ship->efflux);
	pager_printf(ch, FB_CYAN "Num Type           Vnum" EOL);
	a = 0;
	for (auto* dock : ship->docks)
	{
		pager_printf(ch,
		FB_CYAN "%-3d " FB_YELLOW
		"%-14d %-7d %s %-9d %s stat: %d" EOL, a, dock->type, dock->vnum,
				dock->target != NULL ? dock->target->name : "N.D.",
				dock->targetvnum,
				dock->master_slave == -1 ?
						"" : (dock->master_slave == 0 ? "Slave" : "Master"),
				dock->status);
		a++;
	}
	a = 0;
	for (auto* module : ship->modules)
		a += module->size;

	/*    pager_printf( ch, FB_RED "Modules:  " FB_YELLOW "%d/%d" EOL,a,ship->maxmodule ); //Narazie tylko flooduje ekran
	 pager_printf( ch, FB_CYAN "Num type        val    v2     v3   size  st  cost    vnum" EOL );
	 for( a=0,module = ship->first_module; module ; module = module->next,a++ )
	 pager_printf( ch, FB_CYAN "%-3d " FB_YELLOW "%-11s %-6d %-6d %-4d %-5d %-3d %-7d %-6d %s" EOL,
	 a, module_type_name[module->type], module->value , module->value2, module->value3, module->size,
	 module->crs*100/module->status, module->cost, module->vnum, module->spyname[0] != '\0' ? module->spyname
	 : "N/a" );
	 */pager_printf(ch,
			FB_YELLOW "Hull: " FG_YELLOW "%d" FB_YELLOW "/" FG_YELLOW "%d  " FB_YELLOW
			"Ship Condition: " FG_YELLOW "%s" EOL, ship->hull, ship->maxhull,
			ship->shipstate == SHIP_DISABLED ? "Disabled" : "Running");

	pager_printf(ch,
	FB_YELLOW
	"Shields: " FG_YELLOW "%d" FB_YELLOW "/" FG_YELLOW "%d   " FB_YELLOW
	"Energy(fuel): " FG_YELLOW "%d" FB_YELLOW "/" FG_YELLOW "%d   " FB_YELLOW
	"Chaff: " FG_YELLOW "%d" FB_YELLOW "/" FG_YELLOW "%d " FB_YELLOW
	"Kamuflage: " FG_YELLOW "%d" FB_YELLOW "/" FG_YELLOW "%d"
	EOL, ship->shield, ship->maxshield, ship->energy, ship->maxenergy,
			ship->chaff, ship->maxchaff, ship->cloack, ship->maxcloack);
	pager_printf(ch,
			FB_YELLOW "Interdict range: " FG_YELLOW "%d" FB_YELLOW "/" FG_YELLOW "%d" EOL,
			ship->interdict * 100000, ship->maxinterdict * 100000);
	pager_printf(ch,
			FB_YELLOW "Current Coordinates: " FG_YELLOW "%.0f %.0f %.0f" EOL,
			ship->vx, ship->vy, ship->vz);
	pager_printf(ch,
			FB_YELLOW "  Load  Coordinates: " FG_YELLOW "%.0f %.0f %.0f" EOL,
			ship->sx, ship->sy, ship->sz);
	pager_printf(ch,
			FB_YELLOW "  Current Heading: " FG_YELLOW "%.0f %.0f %.0f" EOL,
			ship->hx, ship->hy, ship->hz);
	if (ship->currjump != NULL)
	{
		pager_printf(ch,
				FB_YELLOW "Current Jump: " FG_YELLOW "%s  %.0f %.0f %.0f" EOL,
				ship->currjump->name, ship->jx, ship->jy, ship->jz);
	}
	pager_printf(ch,
			FB_YELLOW "vXpos: " FG_YELLOW "%.0f     " FB_YELLOW "vYpos: " FG_YELLOW "%.0f" EOL,
			ship->vXpos, ship->vYpos);
	pager_printf(ch,
			FB_YELLOW "Speed: " FG_YELLOW "%.0f" FB_YELLOW "/" FG_YELLOW "%.0f   " FB_YELLOW
			"Hyperspeed: " FG_YELLOW "%.0f" EOL FB_YELLOW
			"Manueverability: " FG_YELLOW "%d", ship->currspeed,
			ship->realspeed, ship->hyperspeed, ship->manuever);
	pager_printf(ch, FB_YELLOW " Public: " FG_YELLOW "%s" EOL,
			ship->ship_public ? "Yes" : "No");
	pager_printf(ch, FB_YELLOW "From: %s Cargo: %d/%d, Cargo Type: %s" EOL,
			!ship->cargofrom ? "---" : ship->cargofrom, ship->cargo,
			ship->maxcargo, cargo_names[ship->cargotype]);
	pager_printf(ch, FB_YELLOW "Prise:          %d Trawler: %d" EOL,
			get_ship_value(ship), ship->trawler);
	/*Aldegard, a Pixel maczal palce */

	if (ship->ship_public && !ship->stops.empty())
	{
		int i = 0;

		pager_printf(ch, FG_GREEN "Course:" EOL);

		for (auto* course : ship->stops)
		{
			pager_printf(ch, "%d) %s  %8d - %s" EOL, i,
					course == ship->curr_stop ? FB_WHITE : PLAIN,
					course->stop_vnum, course->stop_name);
			i++;
		}
	}

	pager_printf(ch, FB_YELLOW "Description:" EOL "%s" PLAIN,
			ship->description ? ship->description : "");
	pager_printf(ch, FB_YELLOW "TRANSPONDER: %s  SSLOOK: %s  PSC: %d" EOL,
			ship->transponder, ship->starsystem ? ship->sslook : "S.N.I.S.",
			ship->public_subclass);

	return;
}

DEF_DO_FUN( makeship )
{
	SHIP_DATA *ship;
	char arg1[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *location;

	argument = one_argument(argument, arg1);

	if (!argument || argument[0] == '\0')
	{
		send_to_char("Usage: makeship <filename> <first vnum>" NL, ch);
		return;
	}

	for (auto* target : ship_list)
	{
		if (!str_cmp(target->filename, arg1))
		{
			send_to_char(FB_RED "EEe jest juz statek z takim filename." NL, ch);
			return;
		}
	}
	if (!is_number(argument))
	{
		send_to_char(FB_RED "Vnum must be a number!!!." NL, ch);
		send_to_char(NL "Usage: makeship <filename> <first vnum>" EOL, ch);
		return;
	}

	if ((location = get_room_index(atoi(argument))) == NULL)
	{
		send_to_char(FB_RED "No such room - bad vnum." NL, ch);
		return;
	}
	ship = new_ship();
	STRDUP(ship->name, argument);
	STRDUP(ship->description, "");
	STRDUP(ship->ship_title, argument);
	STRDUP(ship->owner, "");
	STRDUP(ship->copilot, "");
	STRDUP(ship->engineer, "");
	STRDUP(ship->pilot, "");
	STRDUP(ship->home, "");
	ship->type = SHIP_CIVILIAN;
	ship->public_subclass = 0;
	ship->starsystem = NULL;
	ship->energy = ship->maxenergy;
	ship->hull = ship->maxhull;
	ship->in_room = NULL;
	ship->shipyard = get_room_index(198);
	ship->currjump = NULL;
	ship->target0 = NULL;
	ship->last_dock_with = NULL;
	ship->astro_array = 1;
	ship->comm = 1;
	ship->sensor = 1;
	ship->target_array = 1;
	ship->maxcargo = 0;
	ship->cargo = 0;
	ship->flags = 0;
	ship->cargotype = 0;
	STRDUP(ship->cargofrom, "");
	ship->size = 10;
	ship->maxmodule = 10;
	STRDUP(ship->transponder, "");
	generate_transponder(ship);
	STRDUP(ship->filename, arg1);
	STRDUP(ship->sslook, "");
	ship->firstroom = atoi(argument);
	ship->lastroom = atoi(argument);
	ship->cockpit = location;
	ship->coseat = location;
	ship->pilotseat = location;
	ship->gunseat = location;
	ship->navseat = location;
	ship->engineroom = location;
	ship->entrance = location;

	ship_list.push_back(ship);
	save_ship(ship);
	write_ship_list();

}

DEF_DO_FUN( copyship )
{
	SHIP_DATA *ship;
	SHIP_DATA *old;
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);

	if (!argument || argument[0] == '\0')
	{
		send_to_char("Usage: copyship <oldshipname> <filename> <newshipname>"
		NL, ch);
		return;
	}

	old = get_ship(arg);

	if (!old)
	{
		send_to_char("Thats not a ship!" NL, ch);
		return;
	}
	for (auto* target : ship_list)
	{
		if (!str_cmp(target->filename, arg2))
		{
			send_to_char(FB_RED "EEe jest juz statek z takim filename." NL, ch);
			return;
		}
	}

	ship = new_ship();
	STRDUP(ship->name, argument);
	STRDUP(ship->description, "");
	STRDUP(ship->ship_title, argument);
	STRDUP(ship->owner, "");
	STRDUP(ship->copilot, "");
	STRDUP(ship->engineer, "");
	STRDUP(ship->pilot, "");
	STRDUP(ship->home, "");
	ship->type = old->type;
	ship->public_subclass = old->public_subclass;
	ship->shipyard = old->shipyard;
	ship->clazz = old->clazz;
	ship->lasers = old->lasers;
	ship->ioncannons = old->ioncannons;
	ship->maxmissiles = old->maxmissiles;
	ship->maxrockets = old->maxrockets;
	ship->maxtorpedos = old->maxtorpedos;
	ship->maxshield = old->maxshield;
	ship->maxhull = old->maxhull;
	ship->maxenergy = old->maxenergy;
	ship->hyperspeed = old->hyperspeed;
	ship->maxchaff = old->maxchaff;
	ship->realspeed = old->realspeed;
	ship->comm = old->comm;
	ship->flags = old->flags;
	ship->manuever = old->manuever;
	ship->sensor = old->sensor;
	ship->maxcloack = old->maxcloack;
	ship->maxmodule = old->maxmodule;
	ship->maxinterdict = old->maxinterdict;
	ship->target_array = old->target_array;
	ship->astro_array = old->astro_array;
	ship->in_room = NULL;
	ship->currjump = NULL;
	ship->target0 = NULL;
	ship->last_dock_with = NULL;
	ship->size = old->size;
	ship->maxcargo = old->maxcargo;
	ship->trawler = old->trawler;
	STRDUP(ship->filename, arg2);
	STRDUP(ship->sslook, "");
	STRDUP(ship->transponder, "");
	generate_transponder(ship);
	ship_list.push_back(ship);

	save_ship(ship);
	write_ship_list();
}

DEF_DO_FUN( copystats )
{
	SHIP_DATA *ship;
	SHIP_DATA *old;
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);

	if (arg[0] == '\0' || arg2[0] == '\0')
	{
		send_to_char("Usage: copystats <sampleship> <ship>"
		NL, ch);
		return;
	}

	old = get_ship(arg);

	if (!old)
	{
		send_to_char("sampleship: thats not a ship!" NL, ch);
		return;
	}

	ship = get_ship(arg2);

	if (!ship)
	{
		send_to_char("ship: thats not a ship!" NL, ch);
		return;
	}

	ship->type = old->type;
	ship->public_subclass = old->public_subclass;
	ship->shipyard = old->shipyard;
	ship->clazz = old->clazz;
	ship->lasers = old->lasers;
	ship->ioncannons = old->ioncannons;
	ship->maxmissiles = old->maxmissiles;
	ship->maxrockets = old->maxrockets;
	ship->maxtorpedos = old->maxtorpedos;
	ship->maxshield = old->maxshield;
	ship->maxhull = old->maxhull;
	ship->maxenergy = old->maxenergy;
	ship->hyperspeed = old->hyperspeed;
	ship->maxchaff = old->maxchaff;
	ship->realspeed = old->realspeed;
	ship->comm = old->comm;
	ship->flags = old->flags;
	ship->manuever = old->manuever;
	ship->sensor = old->sensor;
	ship->maxcloack = old->maxcloack;
	ship->maxmodule = old->maxmodule;
	ship->maxinterdict = old->maxinterdict;
	ship->target_array = old->target_array;
	ship->astro_array = old->astro_array;
	//ship->in_room = NULL;
	//ship->currjump = NULL;
	//ship->target0 = NULL;
	//ship->last_dock_with = NULL;
	ship->size = old->size;
	ship->maxcargo = old->maxcargo;
	ship->trawler = old->trawler;
	//STRDUP(ship->sslook, "");

	save_ship(ship);
	write_ship_list();

}

bool shiptitleowner(CHAR_DATA *ch, SHIP_DATA *ship)
{
	CLAN_DATA *clan;
	MEMBER_DATA *member;

	if (!str_cmp(ch->name, ship->owner))
		return true;

	if (!str_cmp("Public", ship->owner))
		return false;

	if ((clan = get_clan(ship->owner)) && (member = get_member(clan, ch->name))
			&& member->status == CLAN_LEADER)
		return true;

	return false;
}

DEF_DO_FUN( add_ship_title )
{	//by Trog

	SHIP_DATA *ship;
	int price, i;
	char arg[MIL];
	char spsv[MSL];

	i = 0;
	while (argument[i] != '\0')
	{
		arg[i] = argument[i];
		if ((arg[i] == __FGMOD[0]) || (arg[i] == __BGMOD[0]))
		{
			send_to_char("Tytu� statku nie mo�e zawiera� kolor�w!" NL, ch);
			return;
		}
		i++;
	}
	arg[i] = '\0';

	switch (ch->substate)
	{
	default:

		if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
		{
			send_to_char(FB_RED
			"Musisz by� w kokpicie statku, aby to zrobi�!"
			EOL, ch);
			return;
		}

		if (!str_cmp(ship->owner, "Public"))
		{
			send_to_char("Statkom publicznym nazwy zmieniaj� w�adze!" NL, ch);
			return;
		}

		if (!shiptitleowner(ch, ship))
		{
			send_to_char("Nie jeste� w�a�cicielem tego statku!" NL, ch);
			return;
		}

		if (argument[0] == '\0')
		{
			send_to_char("Jaki tytu� chcesz nada� temu statkowi?" NL, ch);
			return;
		}

		send_to_char(FB_GREEN "��czysz si� z g��wn� baz� danych..." EOL, ch);
		act(PLAIN, "$n manipuluje komputerem pok�adowym.", ch, NULL, NULL,
		TO_ROOM);
		add_timer(ch, TIMER_DO_FUN, 7, do_add_ship_title, 1);
		STRDUP(ch->dest_buf, argument);
		return;

	case 1:
		if (!*ch->dest_buf)
			return;
		snprintf(argument, MIL, "%s", ch->dest_buf);
		STRDUP(ch->dest_buf, "");
		break;

	case SUB_TIMER_DO_ABORT:
		break_skill(FB_RED "Przerywasz po��czenie..." EOL, ch);
		return;
	}
	ch->substate = SUB_NONE;

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku, aby to zrobi�!"
		EOL, ch);
		return;
	}

	if (!shiptitleowner(ch, ship))
	{
		send_to_char("Nie jeste� w�a�cicielem tego statku!" NL, ch);
		return;
	}

	if (!str_cmp(ship->owner, "Public"))
	{
		send_to_char("Statkom publicznym nazwy zmieniaj� w�adze!" NL, ch);
		return;
	}

	if (argument[0] == '\0')
	{
		send_to_char("Jaki tytu� chcesz nada� temu statkowi?" NL, ch);
		return;
	}

	/* Zmiana istniejacego jest tansza. */
	if (ship->ship_title && (ship->ship_title[0] != ship->name[0]))
		price = (get_ship_value(ship) / 1000) * number_range(2, 3);
	else
		//tutaj juz drozej
		price = (get_ship_value(ship) / 1000) * number_range(3, 5);

	if (ch->gold < price)
	{
		ch_printf(ch, MOD_BLINK FB_WHITE "G��wna baza danych:^x" EOL
		"Nadanie tytu�u temu statkowi kosztuje oko�o %d kredyt%s."
		NL "Niestety nie masz tyle." NL, price,
				NUMBER_SUFF(price, "k�", "ki", "ek"));
		return;
	}
	else
	{
		sprintf(spsv, "%s %d", argument, VNUM(ship->pilotseat));
		STRDUP(ship->ship_title, spsv);
		save_ship(ship);
		ch->gold -= price;
		ch_printf(ch, MOD_BLINK FB_WHITE "G��wna baza danych:^x" EOL
		"Statek: %s" NL
		"Zmieniono tytu�. Zap�acono: %d kredyt%s." NL
		"Obecny tytu� statku przypisany jako:" NL
		"%s" NL, ship->name, price, NUMBER_SUFF(price, "k�", "ki", "ek"),
				ship->ship_title);
	}
	return;

}

DEF_DO_FUN( ships )
{
	int count;
	char color[MSL];
	int price;

	if (!IS_NPC(ch)
			&& (!str_cmp(argument, "own") || !str_cmp(argument, "moje")))
	{
		count = 0;
		send_to_pager(
				"Oto lista statk�w posiadanych przez ciebie lub twoj� organiacj�:"
				NL, ch);
		send_to_pager(FB_WHITE
		"Statek                              W�a�ciciel" NL, ch);

		for (auto* ship : ship_list)
		{
			if (str_cmp(ship->owner, ch->name))
			{
				if (!ch->pcdata || !ch->pcdata->clan
						|| str_cmp(ship->owner, ch->pcdata->clan->name)
						|| !is_ship(ship))
					continue;
			}

			if (ship->type == MOB_SHIP)
				sprintf(color, PLAIN);
			else if (ship->type == SHIP_REPUBLIC)
				sprintf(color, FG_RED);
			else if (ship->type == SHIP_IMPERIAL)
				sprintf(color, FG_GREEN);
			else if (ship->type == SHIP_YUUZHAN)
				sprintf(color, FG_YELLOW);
			else if (ship->type == SHIP_PIRATE)
				sprintf(color, FG_MAGENTA);
			else
				sprintf(color, FB_BLUE);

			if (ship->in_room)
				pager_printf(ch, "%s%s - %s" EOL, color, SHIPNAME(ship),
						ship->in_room->name);
			else
				pager_printf(ch, "%s%s" EOL, color, SHIPNAME(ship));
			count++;
		}

		if (!count)
		{
			send_to_pager("Nie posiadasz �adnego statku." NL, ch);
		}
	}

	count = 0;
	send_to_pager(EOL "Lista statk�w zadokowanych tutaj:" NL, ch);

	send_to_pager(FB_WHITE
	"Statek                              W�a�ciciel     Cena wynajmu"
	EOL, ch);
	for (auto* ship : ship_list)
	{
		if (ship->location != ch->in_room || !is_ship(ship))
			continue;

		if (ship->type == MOB_SHIP)
			sprintf(color, PLAIN);
		else if (ship->type == SHIP_REPUBLIC)
			sprintf(color, FG_RED);
		else if (ship->type == SHIP_IMPERIAL)
			sprintf(color, FG_GREEN);
		else if (ship->type == SHIP_YUUZHAN)
			sprintf(color, FG_YELLOW);
		else if (ship->type == SHIP_PIRATE)
			sprintf(color, FG_MAGENTA);
		else
			sprintf(color, FB_BLUE);

		if (!str_cmp(argument, "names") && IS_IMMORTAL(ch))
		{
			pager_printf(ch, "%s%-35s %-15s", color, ship->name,
					!str_cmp(ship->owner, "Public") ?
							"Publiczny" : ship->owner);
		}
		else
		{
			pager_printf(ch, "%s%-35s %-15s", color, SHIPNAME(ship),
					!str_cmp(ship->owner, "Public") ? "Publiczny" :
					(!str_cmp(ship->owner, ch->name)
							|| !str_cmp(ship->owner, "") || IS_IMMORTAL(ch)) ?
							ship->owner : "KUPIONY");
		}
		if (ship->type == MOB_SHIP || is_platform(ship))
		{
			pager_printf(ch, EOL);
			continue;
		}
		if (!str_cmp(ship->owner, "Public"))
		{
			price = get_ship_value(ship) / 1000;
			if ((IS_NEWBIE(ch)
					|| (IS_ADMIN(ch->name) && IS_SET(ch->act, PLR_HOLYLIGHT)))
					&& is_scout(ship))
				price /= 10;
			pager_printf(ch, "%ld do wynaj�cia." EOL, price);
		}
		else if (str_cmp(ship->owner, ""))
			pager_printf(ch, EOL);
		else
		{
			pager_printf(ch, "%ld na sprzeda�." EOL, get_ship_value(ship));
		}
		count++;
	}

	if (!count)
	{
		send_to_pager("  Nie ma tu zadokowanych statk�w." NL, ch);
	}
}

DEF_DO_FUN( speeders )
{
	int count;

	if (!IS_NPC(ch))
	{
		count = 0;
		send_to_char(
				"Lista pojazd�w posiadanych przez ciebie lub twoj� organizacj�:"
				NL, ch);
		send_to_char(NL FB_WHITE
		"Pojazd                             W�a�ciciel" NL, ch);
		for (auto* ship : ship_list)
		{
			if (str_cmp(ship->owner, ch->name))
			{
				if (!ch->pcdata || !ch->pcdata->clan
						|| str_cmp(ship->owner, ch->pcdata->clan->name)
						|| is_ship(ship))
					continue;
			}
			if (ship->location != ch->in_room || is_ship(ship))
				continue;

			if (ship->type == MOB_SHIP)
				continue;
			else if (ship->type == SHIP_REPUBLIC)
				send_to_char(FG_RED, ch);
			else if (ship->type == SHIP_IMPERIAL)
				send_to_char(FG_GREEN, ch);
			else
				send_to_char(FB_BLUE, ch);

			ch_printf(ch, "%-35s %-15s" NL, ship->name, ship->owner);

			count++;
		}

		if (!count)
		{
			send_to_char(FB_WHITE "  Nie posiadasz �adnych pojazd�w." NL, ch);
		}

	}

	count = 0;
	send_to_char(FB_WHITE NL "Lista pojazd�w zaparkowanych tutaj:" NL, ch);

	send_to_char(FB_WHITE
	"Pojazd                             W�a�ciciel     Cena wynajmu"
	NL, ch);
	for (auto* ship : ship_list)
	{
		if (ship->location != ch->in_room || is_ship(ship))
			continue;

		if (ship->type == MOB_SHIP)
			continue;
		else if (ship->type == SHIP_REPUBLIC)
			send_to_char(FG_RED, ch);
		else if (ship->type == SHIP_IMPERIAL)
			send_to_char(FG_GREEN, ch);
		else
			send_to_char(FB_BLUE, ch);

		ch_printf(ch, "%-35s %-15s", ship->name,
				!str_cmp(ship->owner, "Public") ? "Publiczny" : ship->owner);

		if (!str_cmp(ship->owner, "Public"))
		{
			ch_printf(ch, "%ld do wynaj�cia." NL, get_ship_value(ship) / 1000);
		}
		else if (str_cmp(ship->owner, ""))
			ch_printf(ch, "%s", "" NL);
		else
			ch_printf(ch, "%ld na sprzeda�." NL, get_ship_value(ship));

		count++;
	}

	if (!count)
	{
		send_to_char(FB_WHITE "  Nie ma tu �adnych pojazd�w." NL, ch);
	}
}

DEF_DO_FUN( allspeeders )
{
	int count = 0;

	count = 0;
	send_to_char(NL FB_YELLOW
	"The following sea/land/air vehicles are currently formed:"
	NL, ch);

	send_to_char(NL FB_WHITE "Vehicle                            Owner" NL, ch);
	for (auto* ship : ship_list)
	{
		if (is_ship(ship))
			continue;

		if (ship->type == MOB_SHIP)
			continue;
		else if (ship->type == SHIP_REPUBLIC)
			send_to_char(FG_RED, ch);
		else if (ship->type == SHIP_IMPERIAL)
			send_to_char(FG_GREEN, ch);
		else
			send_to_char(FB_BLUE, ch);

		ch_printf(ch, "%-35s %-15s ", ship->name, ship->owner);

		if (!str_cmp(ship->owner, "Public"))
		{
			ch_printf(ch, "%ld to rent." NL, get_ship_value(ship) / 1000);
		}
		else if (str_cmp(ship->owner, ""))
			ch_printf(ch, "%s", "" NL);
		else
			ch_printf(ch, "%ld to buy." NL, get_ship_value(ship));

		count++;
	}

	if (!count)
	{
		send_to_char("There are none currently formed." NL, ch);
		return;
	}

}

DEF_DO_FUN( allships )
{
	SPACE_DATA *starsystem;
	int count = 0;
	char buf[MAX_STRING_LENGTH * 8];	//Wielki Bufor Patrzy ;)
	char mbuf[MAX_STRING_LENGTH];
	char minbuf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	int rep, imp, mob, plat, civ, des;
	rep = imp = mob = plat = civ = des = 0;
	count = 0;
	sprintf(buf, "Lista wszystkich aktualnie dzia�aj�cych statk�w:" NL);

	if (IS_IMMORTAL(ch))
	{
		if (!str_cmp(argument, "saveall"))
		{
			for (auto* ship : ship_list)
			{
				save_ship(ship);
			}
			send_to_char("Done" NL, ch);
			return;
		}

		argument = one_argument(argument, arg1);
		if (!str_cmp(arg1, "owner"))
		{
			pager_printf(ch,
					FB_WHITE
					"Statek                                             Cena                 Desc"
					NL);
			for (auto* ship : ship_list)
				if (!str_cmp(argument, ship->owner))
					pager_printf(ch,
							!IS_SET(ship->flags, SHIP_DONE) ?
									FG_CYAN "%-50s %-20d %s" EOL :
									FB_CYAN "%-50s %-20d %s" EOL,
							SHIPNAME(ship), get_ship_value(ship),
							IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			return;
		}
		if (!str_cmp(arg1, "classnum"))
		{
			pager_printf(ch,
					FB_WHITE
					"Statek                                             Cena                 Desc"
					NL);
			for (auto* ship : ship_list)
				if (atoi(argument) == ship->clazz)
					pager_printf(ch,
							!IS_SET(ship->flags, SHIP_DONE) ?
									FG_CYAN "%-50s %-20d %s" EOL :
									FB_CYAN "%-50s %-20d %s" EOL,
							SHIPNAME(ship), get_ship_value(ship),
							IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			return;
		}
		if (!str_cmp(arg1, "class"))
		{
			pager_printf(ch,
					FB_WHITE
					"Statek                                             Cena                 Desc"
					NL);
			for (auto* ship : ship_list)
				if (!str_cmp(argument,
						bit_name(ship_classes_list, ship->clazz)))
					pager_printf(ch,
							!IS_SET(ship->flags, SHIP_DONE) ?
									FG_CYAN "%-50s %-20d %s" EOL :
									FB_CYAN "%-50s %-20d %s" EOL,
							SHIPNAME(ship), get_ship_value(ship),
							IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			return;
		}

		if (!str_cmp(arg1, "vnum"))
		{
			pager_printf(ch,
					FB_WHITE
					"Statek                                             Cena                 Desc"
					NL);
			for (auto* ship : ship_list)
				if (ship->firstroom == atoi(argument))
					pager_printf(ch,
							!IS_SET(ship->flags, SHIP_DONE) ?
									FG_CYAN "%-50s %-20d %s" EOL :
									FB_CYAN "%-50s %-20d %s" EOL,
							SHIPNAME(ship), get_ship_value(ship),
							IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			return;
		}
		if (!str_cmp(arg1, "system"))
		{
			if ((starsystem = starsystem_from_name(argument)) == NULL)
			{
				ch_printf(ch, FB_RED "Nie ma takiego systemu!!" EOL);
				return;
			}
			pager_printf(ch,
					FB_WHITE
					"Statek                                  W�a�ciciel          Cena        Desc"
					NL);
			for (auto* ship : starsystem->ships)
				pager_printf(ch,
						!IS_SET(ship->flags, SHIP_DONE) ?
								FG_CYAN "%-40s %-20s %-12d %s" EOL :
								FB_CYAN "%-40s %-20s %-12d %s" EOL,
						SHIPNAME(ship), ship->owner, get_ship_value(ship),
						IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			return;
		}

		strcat(buf, FB_WHITE "Mob-Ships:" FG_WHITE NL
		FB_WHITE "Statek                              W�a�ciciel      Desc" NL);
		for (auto* ship : ship_list)
			if (ship->type == MOB_SHIP && ship->shipstate != SHIP_REPOSITORY)
			{
				sprintf(mbuf,
						!IS_SET(ship->flags, SHIP_DONE) ?
								FG_CYAN "%-35s %-15s %s" EOL :
								FB_CYAN "%-35s %-15s %s" EOL,
						(ship->ship_title && (ship->ship_title[0] != '\0')) ?
								ship->ship_title : ship->name,
						!str_cmp(ship->owner, "Public") ?
								"Publiczny" : ship->owner,
						IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
				strcat(buf, mbuf);
				mob++;
			}
		strcat(buf, FB_WHITE "Platforms:" FG_WHITE NL
		FB_WHITE "Statek                              W�a�ciciel      Desc" NL);
		for (auto* ship : ship_list)
			if (is_platform(ship) && ship->shipstate != SHIP_REPOSITORY)
			{
				sprintf(mbuf,
						!IS_SET(ship->flags, SHIP_DONE) ?
								FG_CYAN "%-35s %-15s %s" EOL :
								FB_CYAN "%-35s %-15s %s" EOL,
						(ship->ship_title && (ship->ship_title[0] != '\0')) ?
								ship->ship_title : ship->name,
						!str_cmp(ship->owner, "Public") ?
								"Publiczny" : ship->owner,
						IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
				strcat(buf, mbuf);
				plat++;
			}
	}
	send_to_pager(buf, ch);

	sprintf(buf,
			FB_WHITE "Republika:" EOL
			FB_WHITE
			"Statek                              W�a�ciciel      Desc           Cena" EOL);
	for (auto* ship : ship_list)
		if (ship->type == SHIP_REPUBLIC && ship->shipstate != SHIP_REPOSITORY)
		{
			rep++;
			sprintf(mbuf,
					!IS_SET(ship->flags, SHIP_DONE) ?
							FG_CYAN "%-35s %-15s %-15s" :
							FB_CYAN "%-35s %-15s %-15s",
					(ship->ship_title && (ship->ship_title[0] != '\0')) ?
							ship->ship_title : ship->name,
					!str_cmp(ship->owner, "Public") ? "Publiczny" : ship->owner,
					IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			if (ship->type == MOB_SHIP || is_platform(ship))
			{
				sprintf(minbuf, EOL);
				continue;
			}
			if (!str_cmp(ship->owner, "Public"))
				sprintf(minbuf, FG_CYAN "%ld do wynaj�cia." EOL,
						get_ship_value(ship) / 1000);
			else if (str_cmp(ship->owner, ""))
				sprintf(minbuf, EOL);
			else
				sprintf(minbuf, FG_CYAN "%ld na sprzeda�." EOL,
						get_ship_value(ship));
			strcat(mbuf, minbuf);
			strcat(buf, mbuf);
		}
	send_to_pager(buf, ch);
	sprintf(buf,
			FB_WHITE "Imperium:" EOL
			FB_WHITE
			"Statek                              W�a�ciciel      Desc           Cena" EOL);
	for (auto* ship : ship_list)
		if (ship->type == SHIP_IMPERIAL && ship->shipstate != SHIP_REPOSITORY)
		{
			imp++;
			sprintf(mbuf,
					!IS_SET(ship->flags, SHIP_DONE) ?
							FG_CYAN "%-35s %-15s %-15s" :
							FB_CYAN "%-35s %-15s %-15s",
					(ship->ship_title && (ship->ship_title[0] != '\0')) ?
							ship->ship_title : ship->name,
					!str_cmp(ship->owner, "Public") ? "Publiczny" : ship->owner,
					IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			if (ship->type == MOB_SHIP || is_platform(ship))
			{
				sprintf(minbuf, EOL);
				continue;
			}
			if (!str_cmp(ship->owner, "Public"))
				sprintf(minbuf, FG_CYAN "%ld do wynaj�cia." EOL,
						get_ship_value(ship) / 1000);
			else if (str_cmp(ship->owner, ""))
				sprintf(minbuf, EOL);
			else
				sprintf(minbuf, FG_CYAN "%ld na sprzeda�." EOL,
						get_ship_value(ship));
			strcat(mbuf, minbuf);
			strcat(buf, mbuf);
		}
	send_to_pager(buf, ch);
	sprintf(buf,
			FB_WHITE "Piraci:" EOL
			FB_WHITE
			"Statek                              W�a�ciciel      Desc           Cena" EOL);
	for (auto* ship : ship_list)
		if (ship->type == SHIP_PIRATE && ship->shipstate != SHIP_REPOSITORY)
		{
			imp++;
			sprintf(mbuf,
					!IS_SET(ship->flags, SHIP_DONE) ?
							FG_CYAN "%-35s %-15s %-15s" :
							FB_CYAN "%-35s %-15s %-15s",
					(ship->ship_title && (ship->ship_title[0] != '\0')) ?
							ship->ship_title : ship->name,
					!str_cmp(ship->owner, "Public") ? "Publiczny" : ship->owner,
					IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			if (ship->type == MOB_SHIP || is_platform(ship))
			{
				sprintf(minbuf, EOL);
				continue;
			}
			if (!str_cmp(ship->owner, "Public"))
				sprintf(minbuf, FG_CYAN "%ld do wynaj�cia." EOL,
						get_ship_value(ship) / 1000);
			else if (str_cmp(ship->owner, ""))
				sprintf(minbuf, EOL);
			else
				sprintf(minbuf, FG_CYAN "%ld na sprzeda�." EOL,
						get_ship_value(ship));
			strcat(mbuf, minbuf);
			strcat(buf, mbuf);
		}
	send_to_pager(buf, ch);
	sprintf(buf,
			FB_WHITE "Yuuzhanie:" EOL
			FB_WHITE
			"Statek                              W�a�ciciel      Desc           Cena" EOL);
	for (auto* ship : ship_list)
		if (ship->type == SHIP_YUUZHAN && ship->shipstate != SHIP_REPOSITORY)
		{
			imp++;
			sprintf(mbuf,
					!IS_SET(ship->flags, SHIP_DONE) ?
							FG_CYAN "%-35s %-15s %-15s" :
							FB_CYAN "%-35s %-15s %-15s",
					(ship->ship_title && (ship->ship_title[0] != '\0')) ?
							ship->ship_title : ship->name,
					!str_cmp(ship->owner, "Public") ? "Publiczny" : ship->owner,
					IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			if (ship->type == MOB_SHIP || is_platform(ship))
			{
				sprintf(minbuf, EOL);
				continue;
			}
			if (!str_cmp(ship->owner, "Public"))
				sprintf(minbuf, FG_CYAN "%ld do wynaj�cia." EOL,
						get_ship_value(ship) / 1000);
			else if (str_cmp(ship->owner, ""))
				sprintf(minbuf, EOL);
			else
				sprintf(minbuf, FG_CYAN "%ld na sprzeda�." EOL,
						get_ship_value(ship));
			strcat(mbuf, minbuf);
			strcat(buf, mbuf);
		}
	send_to_pager(buf, ch);
	sprintf(buf,
			FB_WHITE "Cywilne:" EOL
			FB_WHITE
			"Statek                              W�a�ciciel      Desc           Cena" EOL);
	for (auto* ship : ship_list)
		if (ship->type == SHIP_CIVILIAN && ship->shipstate != SHIP_REPOSITORY)
		{
			civ++;
			sprintf(mbuf,
					!IS_SET(ship->flags, SHIP_DONE) ?
							FG_CYAN "%-35s %-15s %-15s" :
							FB_CYAN "%-35s %-15s %-15s",
					(ship->ship_title && (ship->ship_title[0] != '\0')) ?
							ship->ship_title : ship->name,
					!str_cmp(ship->owner, "Public") ? "Publiczny" : ship->owner,
					IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");
			if (ship->type == MOB_SHIP || is_platform(ship))
			{
				sprintf(minbuf, EOL);
				continue;
			}
			if (!str_cmp(ship->owner, "Public"))
				sprintf(minbuf, FG_CYAN "%ld do wynaj�cia." EOL,
						get_ship_value(ship) / 1000);
			else if (str_cmp(ship->owner, ""))
				sprintf(minbuf, EOL);
			else
				sprintf(minbuf, FG_CYAN "%ld na sprzeda�." EOL,
						get_ship_value(ship));
			strcat(mbuf, minbuf);
			strcat(buf, mbuf);
		}
	send_to_pager(buf, ch);

	sprintf(buf,
			FB_WHITE "Zniszczone:" EOL
			FB_WHITE
			"Statek                              W�a�ciciel      Desc           Cena" EOL);
	for (auto* ship : ship_list)
		if (ship->shipstate == SHIP_REPOSITORY)
		{
			des++;
			sprintf(mbuf,
					!IS_SET(ship->flags, SHIP_DONE) ?
							FG_CYAN "%-35s %-15s %-15s" :
							FB_CYAN "%-35s %-15s %-15s",
					(ship->ship_title && (ship->ship_title[0] != '\0')) ?
							ship->ship_title : ship->name,
					!str_cmp(ship->owner, "Public") ? "Publiczny" : ship->owner,
					IS_SET(ship->flags, SHIP_DESCLESS) ? "Empty" : "");

			/*		if (ship->type == MOB_SHIP || is_platform(ship) )
			 {
			 sprintf( minbuf, "" NL);
			 continue;
			 }
			 */if (!str_cmp(ship->owner, "Public"))
				sprintf(minbuf, FG_CYAN "%ld do wynaj�cia." EOL,
						get_ship_value(ship) / 1000);
			else if (str_cmp(ship->owner, ""))
				sprintf(minbuf, EOL);
			else
				sprintf(minbuf, FG_CYAN "%ld na sprzeda�." EOL,
						get_ship_value(ship));
			strcat(mbuf, minbuf);
			strcat(buf, mbuf);
		}
	send_to_pager(buf, ch);

	if ((count = mob + plat + rep + imp + civ + des) == 0)
		ch_printf(ch, "Nie ma aktualnie �adnych statk�w." NL);
	else
		ch_printf(ch, FB_WHITE "��cznie: %d." EOL, count);

	return;
}

void ship_to_starsystem(SHIP_DATA *ship, SPACE_DATA *starsystem)
{
	if (!starsystem || !ship)
		return;

	starsystem->ships.push_back(ship);
	ship->starsystem = starsystem;
}

void new_missile(SHIP_DATA *ship, SHIP_DATA *target, CHAR_DATA *ch,
		int missiletype)
{
	SPACE_DATA *starsystem;
	MISSILE_DATA *missile;

	if (ship == NULL)
		return;

	if (target == NULL)
		return;

	if ((starsystem = ship->starsystem) == NULL)
		return;

	CREATE(missile, MISSILE_DATA, 1);
	missile_list.push_back(missile);

	missile->target = target;
	missile->fired_from = ship;
	if (ch)
		STRDUP(missile->fired_by, ch->name);
	else
		STRDUP(missile->fired_by, "");
	missile->missiletype = missiletype;
	missile->age = 0;
	if (missile->missiletype == HEAVY_BOMB)
		missile->speed = 20;
	else if (missile->missiletype == PROTON_TORPEDO)
		missile->speed = 200;
	else if (missile->missiletype == CONCUSSION_MISSILE)
		missile->speed = 300;
	else
		missile->speed = 50;

	missile->mx = ship->vx;
	missile->my = ship->vy;
	missile->mz = ship->vz;

	starsystem->missiles.push_back(missile);
	missile->starsystem = starsystem;

}

void ship_from_starsystem(SHIP_DATA *ship, SPACE_DATA *starsystem)
{
	if (!starsystem || !ship)
		return;

	starsystem->ships.remove(ship);
	ship->starsystem = NULL;
}

void extract_missile(MISSILE_DATA *missile)
{
	SPACE_DATA *starsystem;

	if (missile == NULL)
		return;

	if ((starsystem = missile->starsystem) != NULL)
	{
		starsystem->missiles.remove(missile);
		missile->starsystem = NULL;
	}

	missile_list.remove(missile);

	missile->target = NULL;
	missile->fired_from = NULL;
	free_missile(missile);
}

bool is_rental(CHAR_DATA *ch, SHIP_DATA *ship)
{
	if (!str_cmp("Public", ship->owner))
		return true;

	return false;
}

bool check_pilot(CHAR_DATA *ch, SHIP_DATA *ship)
{
	if (!str_cmp(ch->name, ship->owner) || !str_cmp(ch->name, ship->pilot)
			|| !str_cmp(ch->name, ship->copilot)
			|| !str_cmp("Public", ship->owner)
			|| !str_cmp(ch->name, ship->engineer))
		return true;

	if (!IS_NPC(ch) && ch->pcdata->clan
			&& !str_cmp(ch->pcdata->clan->name, ship->owner)
			&& (clan_bestow(ch, "pilot") || IS_FIRST(ch) || IS_SECOND(ch)))
		return true;

	if (IS_ADMIN(ch->name) && IS_SET(ch->act, PLR_HOLYLIGHT))
		return true;

	return false;
}

bool extract_ship(SHIP_DATA *ship)
{
	ROOM_INDEX_DATA *room;

	if ((room = ship->in_room) != NULL)
	{
		room->ships.remove(ship);
		ship->in_room = NULL;
	}
	return true;
}

void damage_ship_ch(SHIP_DATA *ship, int min, int max, CHAR_DATA *ch)
{
	int damage, shield_dmg;
	long xp;

	damage = number_range(min, max);

	xp = (exp_level(ch->skill_level[PILOTING_ABILITY] + 1)
			- exp_level(ch->skill_level[PILOTING_ABILITY])) / 25;
	xp = UMIN(get_ship_value(ship) / 100, xp);
	if (ship->timer == -1)
		gain_exp(ch, xp, PILOTING_ABILITY);

	if (ship->shield > 0)
	{
		shield_dmg = UMIN(ship->shield, damage);
		damage -= shield_dmg;
		ship->shield -= shield_dmg;
		if (ship->shield <= 0)
		{
			ship->shield = 0;
			echo_to_cockpit(ship, FG_RED "Tarcze uszkodzone...");
		}
	}

	if (damage > 0)
	{
		if (number_range(1, 100) <= 5 && ship->shipstate != SHIP_DISABLED)
		{
			echo_to_cockpit(ship,
			FG_RED MOD_BLINK "Nap�d statku USZKODZONY!" RESET);
			ship->shipstate = SHIP_DISABLED;
		}

		if (number_range(1, 100) <= 5)
		{
			for (auto* turret : ship->turrets)
				if (turret)
					if (number_range(1, 10) <= 3)
						turret->status = TURRET_DAMAGED; //dodac niszczenie -- Pixel
		}

		if (number_range(1, 100) <= 5 && ship->missilestate != MISSILE_DAMAGED
				&& ship->maxmissiles > 0)
		{
			echo_to_room(ship->gunseat,
			FG_RED MOD_BLINK "Wyrzutnia pocisk�w USZKODZONA!"
			RESET);
			ship->missilestate = MISSILE_DAMAGED;
		}

		if (number_range(1, 100) <= 2 && ship->statet0 != LASER_DAMAGED)
		{
			echo_to_room(ship->gunseat, "Lasery USZKODZONE!");
			ship->statet0 = LASER_DAMAGED;
		}
	}

	ship->hull -= damage;

	if (ship->hull <= 0)
	{
		xp = ((exp_level(ch->skill_level[PILOTING_ABILITY] + 1)
				- exp_level(ch->skill_level[PILOTING_ABILITY])) / 2);
		xp = UMIN(get_ship_value(ship), xp);
		if (ship->timer == -1)
		{
			gain_exp(ch, xp, PILOTING_ABILITY);
			ch_printf(ch,
			FB_WHITE
			"Zdobywasz %ld punkt�w do�wiadczenia w pilotowaniu!" NL, xp);
		}
		destroy_ship(ship, ch);

		return;
	}

	if (ship->hull <= ship->maxhull / 10)
		echo_to_cockpit(ship,
		FG_RED MOD_BLINK
		"OSTRZE�ENIE! Kad�ub statku powa�nie uszkodzony!"
		RESET);

}

void damage_ship(SHIP_DATA *ship, int min, int max)
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
			echo_to_cockpit(ship, FG_RED "Tarcze uszkodzone...");
		}
	}

	if (damage > 0)
	{

		if (number_range(1, 100) <= 5 && ship->shipstate != SHIP_DISABLED)
		{
			echo_to_cockpit(ship,
			FG_RED MOD_BLINK "Nap�d statku USZKODZONY!" RESET);
			ship->shipstate = SHIP_DISABLED;
		}

		if (number_range(1, 100) <= 5 && ship->missilestate != MISSILE_DAMAGED
				&& ship->maxmissiles > 0)
		{
			echo_to_room(ship->gunseat,
			FG_RED MOD_BLINK "Wyrzutnia pocisk�w USZKODZONA!"
			RESET);
			ship->missilestate = MISSILE_DAMAGED;
		}

	}

	ship->hull -= damage;

	if (ship->hull <= 0)
	{
		destroy_ship(ship, NULL);
		return;
	}

	if (ship->hull <= ship->maxhull / 10)
		echo_to_cockpit(ship,
		FG_RED MOD_BLINK
		"OSTRZE�ENIE! Kad�ub statku powa�nie uszkodzony!"
		RESET);

}

void purge_ship(SHIP_DATA *ship, CHAR_DATA *ch)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *rch;
	SHIPDOCK_DATA *targetdock;

	sprintf(buf, FB_WHITE MOD_BLINK
	"%s eksploduje w o�lepiaj�cym b�ysku!" RESET, ship->sslook);
	echo_to_system(ship, buf, ship);

	echo_to_ship(ship, FB_WHITE MOD_BLINK "Olbrzymi b�ysk o�lepia ci�!" RESET);
	echo_to_ship(ship,
	FB_WHITE
	"Ale zanim jeszcze nabierzesz powietrza by krzykn��..." EOL
	FB_WHITE "Zamieniasz si� w py� razem ze swoim statkiem...");

	if (!ship->vnum)	// statek starego typu
	{
		if (ship->firstroom == 0)
		{
			bug("ship->firstroom==0: %s , seting to 1", ship->name);
			ship->firstroom = 1;
			ship->lastroom = 1;
			ship->cockpit = get_room_index(1);
			ship->coseat = get_room_index(1);
			ship->pilotseat = get_room_index(1);
			ship->gunseat = get_room_index(1);
			ship->navseat = get_room_index(1);
		}
	}

	for (auto* dock : ship->docks)
	{
		if (dock->master_slave == 0)
		{
			targetdock = shipdock_from_room(dock->target, dock->targetvnum);
			targetdock->targetvnum = 0;
			targetdock->target = NULL;
			targetdock->master_slave = -1;
			dock->targetvnum = 0;
			dock->target = NULL;
			dock->master_slave = -1;
		}

		if (dock->master_slave == 1)
		{
			targetdock = shipdock_from_room(dock->target, dock->targetvnum);
			targetdock->targetvnum = 0;
			targetdock->target = NULL;
			targetdock->master_slave = -1;
			dock->target->shipstate = SHIP_READY;
			dock->targetvnum = 0;
			dock->target = NULL;
			dock->master_slave = -1;
		}
	}

	for (auto* room : ship->locations)
	{
		// wszystkie statki wewnatrz: spadowa
		auto ships_snapshot = room->ships;
		for (auto* target : ships_snapshot)
		{
			purge_ship(target, ch);
		}

		// wszystkie postaci wewnatrz: spadowa
		if (room != NULL)
		{
			while (!room->people.empty())
			{
				rch = room->people.front();
				int homeVnum;
				if (IS_IMMORTAL(rch)
						&& (homeVnum = wherehome(rch)) != rch->in_room->vnum)
				{
					char_from_room(rch);
					char_to_room(rch, get_room_index(homeVnum));
				}
				else
				{
					if (ch)
						raw_kill(ch, rch, 0);
					else
						raw_kill(rch, rch, 0);
				}
			}

			// wszystkie przedmioty wewnatrz: spadowa
			auto contents_snapshot = room->contents;
			for (auto* robj : contents_snapshot)
			{
				separate_obj(robj);
				extract_obj(robj);
			}
		}
	}

	if (ship->in_room)
	{

		int i;
		OBJ_DATA *scraps;

		sprintf(buf,
		FB_WHITE "Pot�na eksplozja rozrywa statek %s na strz�py!", ship->name);
		echo_to_room(ship->in_room, buf);
		scraps = create_object(get_obj_index(OBJ_VNUM_SCRAPS), 0);
		sprintf(buf, "szcz�tki statku %s ", SHIPNAME(ship));
		STRDUP(scraps->name, buf);
		sprintf(buf, "Le�� tu szcz�tki statku %s.", SHIPNAME(ship));
		STRDUP(scraps->description, buf);
		for (i = 0; i < 6; i++)
		{
			sprintf(buf, "Szcz�tki statku %s", SHIPNAME(ship));
			STRDUP(scraps->przypadki[i], buf);
		}
		scraps->timer = 5;
		scraps->weight = 10000;
		obj_to_room(scraps, ship->in_room);
	}
	else
		new_astro(ship, 0);

	if (!ship->vnum)	// stare statki do repozytorium
		ship_to_repository(ship);
	else	// nowe odlatuja do nieba :)
	{
		free_ship(ship); // Pixel: ja bym kasowal
	}
	return;
}

void destroy_ship(SHIP_DATA *ship, CHAR_DATA *ch)
{
	char buf[MSL];

	if (ch)
		fevent_trigger(ch, FE_KILL_SHIP, ship->name);

	if (ship->timer >= 0)
		return;

	if (is_scout(ship) || is_fighter(ship))
	{
		purge_ship(ship, ch);
		return;
	}

	sprintf(buf, FB_RED "%s staje w p�omieniach!" EOL, ship->sslook);
	echo_to_system(ship, buf, NULL);
	ship->timer = UMIN(6, ship->roomcount);

	/*	switch( ship->clazz )                           //Zmienic bo przybylo class shipow
	 {
	 default:
	 break;
	 case MIDSIZE_SHIP:
	 ship->timer += 6;
	 break;
	 case CAPITAL_SHIP:
	 ship->timer += 18;
	 break;
	 case SHIP_PLATFORM:
	 ship->timer += 24;
	 break;
	 }*/
	ship->timer += ship->size / 10;
	ship->timer *= 10;
	ship->timer /= number_range(10, 20);
	ship->killer = ch;
	ship->shipstate = SHIP_EXPLODE;
	return;
}

bool ship_to_room(SHIP_DATA *ship, ROOM_INDEX_DATA *room)
{
	if (room == NULL)
		return false;

	room->ships.push_back(ship);
	ship->in_room = room;
	return true;
}

DEF_DO_FUN( board )
{
	ROOM_INDEX_DATA *fromroom;
	ROOM_INDEX_DATA *toroom;
	SHIP_DATA *ship;

	if (*argument == '\0')
	{
		send_to_char("Wsi��� na co?" NL, ch);
		return;
	}

	if ((ship = ship_in_room(ch->in_room, argument)) == NULL)
	{
		act(PLAIN, "Nie widz� tutaj �adnego $T.", ch, NULL, argument, TO_CHAR);
		return;
	}

	/* Thanos: bugfix (wcze�niej sprawdza� ch->act nawet u gracza, a
	 ten bit u�ywany by� do PLR_LOG */
	if (ch->mount)
	{
		act(PLAIN, "Zejd� najpierw ze swojego wierzchowca.", ch, NULL, argument,
				TO_CHAR);
		return;
	}
	if (IS_NPC(ch) && IS_SET(ch->act, ACT_MOUNTED))
	{
		act(PLAIN, "Nie mo�esz wej�� do �rodka z obci��eniem na grzbiecie.", ch,
				NULL, argument, TO_CHAR);
		return;
	}

	fromroom = ch->in_room;

	if ((toroom = ship->entrance) != NULL)
	{
		if (!ship->hatchopen)
		{
			send_to_char(FB_RED "Klapa jest zamkni�ta!" NL, ch);
			return;
		}

		if (toroom->tunnel > 0 && !IS_SET(ch->act, PLR_HOLYLIGHT))
		{
			int count = 0;

			for (auto* ctmp : toroom->people)
			{
				if (!IS_IMMORTAL(ctmp))
					++count;
				if (count >= toroom->tunnel)
				{
					send_to_char("Nie starczy miejsca dla ciebie." NL, ch);
					return;
				}
			}
		}

		if (ship->shipstate == SHIP_LAUNCH || ship->shipstate == SHIP_LAUNCH_2)
		{
			send_to_char(FB_RED "Ten statek w�a�nie odlatuje!" NL, ch);
			return;
		}

		uncrew(ship_from_room(fromroom), ch);
		act(PLAIN, "$n wsiada na pok�ad $T.", ch, NULL, SHIPNAME(ship),
		TO_ROOM);
		act(PLAIN, "Wsiadasz na pok�ad $T.", ch, NULL, SHIPNAME(ship),
		TO_CHAR);
		char_from_room(ch);
		char_to_room(ch, toroom);
		act(PLAIN, "$n wchodzi na pok�ad.", ch, NULL, argument, TO_ROOM);
		do_look(ch, (char*) "auto");
		fevent_trigger(ch, FE_ENTER_SHIP);
	}
	else
		send_to_char("Ten statek nie ma wej�cia!" NL, ch);
}

bool rent_ship(CHAR_DATA *ch, SHIP_DATA *ship)
{

	long price;

	if (IS_NPC(ch))
		return false;

	price = get_ship_value(ship) / 1000;
	if ((IS_NEWBIE(ch) || (IS_ADMIN(ch->name) && IS_SET(ch->act, PLR_HOLYLIGHT)))
			&& is_scout(ship))
		price /= 10;

	if (ch->gold < price)
	{
		ch_printf(ch,
				FB_RED
				"Wynaj�cie tego statku b�dzie ci� kosztowa� %ld kredytek. Nie masz tyle!"
				NL, price);
		return false;
	}

	ch->gold -= price;
	ch_printf(ch, "&GP�acisz %ld kredytek za wynaj�cie statku." NL, price);
	return true;

}

DEF_DO_FUN( leaveship )
{
	ROOM_INDEX_DATA *fromroom;
	ROOM_INDEX_DATA *toroom;
	SHIP_DATA *ship;

	fromroom = ch->in_room;

	if ((ship = ship_from_entrance(fromroom)) == NULL)
	{
		if ((ship = ship_from_dock(fromroom->vnum)) != NULL)
		{
			leave_ship_by_dock(ship, ch);
			return;
		}
		else
		{
			send_to_char("Nie widz� tu �adnego wyj�cia." NL, ch);
			return;
		}
	}

	if (is_platform(ship))
	{
		send_to_char("Nie mo�esz zrobi� tego tutaj." NL, ch);
		return;
	}

	if (ship->lastdoc != ship->location)
	{
		send_to_char(FB_RED "Proponuj� poczeka� a� statek wyl�duje." NL, ch);
		return;
	}

	if (ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED)
	{
		send_to_char(FB_RED
		"Poczekaj a� statek porz�dnie osi�dzie i zadokuje." NL, ch);
		return;
	}

	if (!ship->hatchopen)
	{
		send_to_char(FB_RED "Najpierw otw�rz klap�." NL, ch);
		return;
	}

	if ((toroom = ship->location) != NULL)
	{
		if (toroom->tunnel > 0 && !IS_SET(ch->act, PLR_HOLYLIGHT))
		{
			int count = 0;

			for (auto* ctmp : toroom->people)
			{
				if (!IS_IMMORTAL(ctmp))
					++count;
				if (count >= toroom->tunnel)
				{
					send_to_char("Nie starczy miejsca dla ciebie." NL, ch);
					return;
				}
			}
		}
		uncrew(ship, ch);
		act(PLAIN, "$n opuszcza statek.", ch, NULL, argument, TO_ROOM);
		act(PLAIN, "Opuszczasz statek.", ch, NULL, argument, TO_CHAR);
		char_from_room(ch);
		char_to_room(ch, toroom);
		act(PLAIN, "$n wysiada ze statku.", ch, NULL, argument, TO_ROOM);
		do_look(ch, (char*) "auto");
	}
	else
		send_to_char("Hmm.. Wyj�cie chyba nie dzia�a." NL, ch);
}

DEF_DO_FUN( launch )
{
	int chance;
	long price = 0;
	SHIP_DATA *ship;
	char buf[MAX_STRING_LENGTH];

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�!" NL,
				ch);
		return;
	}

	if (!is_ship(ship))
	{
		send_to_char(FB_RED "Nie jeste� na statku kosmicznym!" NL, ch);
		return;
	}

	if (ship->clazz == ESCAPE_POD_SHIP)
	{
		send_to_char(FB_RED
		"To jest kapsu�a ratunkowa, jej silnik nie pokona grawitacji!"
		NL, ch);
		return;
	}

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		ch_printf(ch, FB_RED
		"Nie wygl�da na to, �eby� by�%s w kabinie pilota!" EOL,
				SEX_SUFFIX__AO(ch));
		return;
	}

	if (!check_crew(ship, ch, "pilot"))
	{
		send_to_char(FB_RED "Nie zajmujesz stanowiska pilota na tym statku."
		NL, ch);
		return;
	}

	if (ship->shipstate1 != 0)
	{
		send_to_char(FB_RED "BzZzzbBbbzZzz..." NL, ch);
		return;
	}

	if (autofly(ship))
	{
		send_to_char(FB_RED
		"Statek ma w��czonego autopilota. Mo�e najpierw go wy��cz."
		EOL, ch);
		return;
	}

	if (is_platform(ship))
	{
		send_to_char("Nie mo�esz zrobi� tego tutaj." NL, ch);
		return;
	}

	if (ship->lastdoc != ship->location)
	{
		send_to_char(FB_RED
		"Nie wygl�da na to, �eby statek by� zadokowany." EOL, ch);
		return;
	}

	if (ship_in_closebay(ship) == true)
	{
		send_to_char(FB_RED "Ups!!!  hangar jest zamkniety." EOL, ch);
		return;
	}

	if (ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED)
	{
		send_to_char("Statek nie jest zadokowany." NL, ch);
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
				price = 100;
			if (is_fighter(ship))
				price = 200;
			if (is_midship(ship))
				price = 500;
			if (is_capital(ship))
				price = 5000;
			if (is_huge(ship))
				price = 6000;

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
						FB_RED
						"%s nie ma wystarczaj�cej ilo�ci funduszy, by przygotowac ten statek do startu."
						NL, ch->pcdata->clan->name);
				return;
			}

			ch->pcdata->clan->funds -= price;
			ch_printf(ch,
					"&GPrzygotowanie do startu tego statku b�dzie kosztowa�o %s %ld kredytek."
					NL, ch->pcdata->clan->name, price);
		}
		else if (str_cmp(ship->owner, "Public"))
		{
			if (ch->gold < price)
			{
				ch_printf(ch,
						FB_RED
						"Nie masz wystarczaj�cych funduszy by przygotowac ten statek do startu."
						NL);
				return;
			}

			ch->gold -= price;
			ch_printf(ch,
					"&GP�acisz %ld kredytek by przygotowa� statek do startu."
					NL, price);
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
//                        ship->hull            = ship->maxhull; Aldegard: tymczasowo

		ship->missilestate = MISSILE_READY;
		ship->statet0 = LASER_READY;
		ship->shipstate = SHIP_DOCKED;

		if (ship->hatchopen)
		{
			ship->hatchopen = false;
			sprintf(buf, FB_YELLOW "Klapa na %s zamyka si�.", SHIPNAME(ship));
			echo_to_room(ship->location, buf);
			echo_to_room(ship->entrance,
			FB_YELLOW "Klapa zamyka si� z trzaskiem.");
			sound_to_room(ship->entrance, "!!SOUND(door)");
			sound_to_room(ship->location, "!!SOUND(door)");
		}
		send_to_char(FB_GREEN, ch);
		send_to_char("Sekwencja startuj�ca zainicjowana." NL, ch);
		act(PLAIN,
				"$n uruchamia silniki statku i rozpoczyna podej�cie do startu.",
				ch, NULL, argument, TO_ROOM);
		echo_to_ship(ship,
		FB_YELLOW "Statek buczy powoli odrywaj�c si� od ziemi.");
		sprintf(buf, FB_YELLOW "%s startuje.", SHIPNAME(ship));
		echo_to_room(ship->location, buf);
		ship->shipstate = SHIP_LAUNCH;
		if (ship->realspeed >= 40)
			ship->currspeed = ship->realspeed / 2;
		else
		{
			if (ship->realspeed < 20)
				ship->currspeed = ship->realspeed;
			if (ship->realspeed >= 20)
				ship->currspeed = 20;
		}

		learn_from_success_space(ship, ch);
		sound_to_ship(ship, "!!SOUND(xwing)");
		fevent_trigger(ch, FE_LAUNCH_SHIP, ship);
		return;
	}
	send_to_char(FB_RED, ch);
	send_to_char("Nie uda�o ci si� poprawnie obs�u�y� urz�dze�!" NL, ch);
	learn_from_failure_space(ship, ch);

	return;

}

void launchship(SHIP_DATA *ship)
{
	char buf[MAX_STRING_LENGTH];
	SPACE_DATA *starsystem;
	int plusminus;
	int radiusplus;

	ship_to_starsystem(ship, starsystem_from_vnum(VNUM(ship->location)));

	if ((starsystem = ship->starsystem) == NULL)
	{
		echo_to_room(ship->pilotseat, FB_YELLOW
		"Droga startowa zablokowana .. Start odwo�any.");
		echo_to_ship(ship,
		FB_YELLOW
		"Statek z g�o�nym hukiem zn�w opada na l�dowisku.");
		sprintf(buf, FB_YELLOW "%s powoli siada na l�dowisku.", SHIPNAME(ship));
		echo_to_room(ship->location, buf);
		ship->shipstate = SHIP_DOCKED;
		return;
	}
	if (is_midship(ship))
	{
		sound_to_room(ship->location, "!!SOUND(falcon)");
		sound_to_ship(ship, "!!SOUND(falcon)");
	}
	else if (ship->type == SHIP_IMPERIAL)
	{
		sound_to_ship(ship, "!!SOUND(tie)");
		sound_to_room(ship->location, "!!SOUND(tie)");
	}
	else
	{
		sound_to_ship(ship, "!!SOUND(xwing)");
		sound_to_room(ship->location, "!!SOUND(xwing)");
	}

	extract_ship(ship);

	ship->location = NULL;
	generate_sslook(ship);

	if (ship->shipstate != SHIP_DISABLED)
		ship->shipstate = SHIP_READY;
	/*
	 plusminus = number_range(-1, 2);
	 if(plusminus > 0)
	 ship->hx = 1;
	 else
	 ship->hx = -1;

	 plusminus = number_range(-1, 2);
	 if(plusminus > 0)
	 ship->hy = 1;
	 else
	 ship->hy = -1;
	 */
	plusminus = number_range(-1, 2);
	if (plusminus > 0)
		plusminus = 1;
	else
		plusminus = -1;

	for (auto* planet : starsystem->planets)
	{
		for (auto* dock : planet->docks)
		{
			if (dock->vnum == VNUM(ship->lastdoc)) // Start z planety
			{
				radiusplus = planet->radius + 10;

				ship->vx = number_range(planet->xpos - radiusplus,
						planet->xpos + radiusplus);
				ship->vy = number_range(
						-sqrtf(
								(radiusplus * radiusplus)
										- ((ship->vx - planet->xpos)
												* (ship->vx - planet->xpos)))
								+ planet->ypos,
						sqrtf(
								(radiusplus * radiusplus)
										- ((ship->vx - planet->xpos)
												* (ship->vx - planet->xpos)))
								+ planet->ypos);
				ship->vz = plusminus
						* (sqrtf(
								radiusplus * radiusplus
										- (ship->vx - planet->xpos)
												* (ship->vx - planet->xpos)
										- (ship->vy - planet->ypos)
												* (ship->vy - planet->ypos))
								+ planet->zpos);

				// Oddalamy sie od planety

				ship->hx = -(planet->xpos - ship->vx);
				ship->hy = -(planet->ypos - ship->vy);
				ship->hz = -(planet->zpos - ship->vz);
			}
		}
	}
	for (auto* moon : starsystem->moons)
	{
		if (moon->vnum == VNUM(ship->lastdoc)) // Start z ksiezyca
		{
			radiusplus = moon->radius + 10;

			ship->vx = number_range(moon->xpos - radiusplus,
					moon->xpos + radiusplus);
			ship->vy = number_range(
					-sqrtf(
							(radiusplus * radiusplus)
									- ((ship->vx - moon->xpos)
											* (ship->vx - moon->xpos)))
							+ moon->ypos,
					sqrtf(
							(radiusplus * radiusplus)
									- ((ship->vx - moon->xpos)
											* (ship->vx - moon->xpos)))
							+ moon->ypos);
			ship->vz = plusminus
					* (sqrtf(
							radiusplus * radiusplus
									- (ship->vx - moon->xpos)
											* (ship->vx - moon->xpos)
									- (ship->vy - moon->ypos)
											* (ship->vy - moon->ypos))
							+ moon->zpos);

			// Oddalamy sie od ksiezyca

			ship->hx = -(moon->xpos - ship->vx);
			ship->hy = -(moon->ypos - ship->vy);
			ship->hz = -(moon->zpos - ship->vz);

		}
	}
	for (auto* target : ship->starsystem->ships)
	{
		for (auto* hangar : target->hangars)
		{
			if (VNUM(ship->lastdoc) == hangar->vnum)
			{

				// Hangary domyslnie pod statkiem

				ship->vx = target->vx;
				ship->vy = target->vy;
				ship->vz = target->vz + 10 + target->size / 20;

				// Oddalamy sie od statku

				ship->hx = -(target->vx - ship->vx);
				ship->hy = -(target->vy - ship->vy);
				ship->hz = -(target->vz - ship->vz);
			}
		}
	}

	ship->energy -= (100 + 10 * (ship->size / 10));

	echo_to_room(ship->location,
	FB_GREEN "Sekwencja startowa zako�czona." EOL);
	echo_to_ship(ship,
	FB_YELLOW
	"Statek opuszcza teren platformy wzbijaj�c si� w stron� przestworzy.");
	sprintf(buf, FB_YELLOW "%s startuje z platformy", ship->sslook);
	echo_to_system(ship, buf, NULL);
	sprintf(buf, FB_YELLOW "%s znika w przestrzeni.", SHIPNAME(ship));
	echo_to_room(ship->lastdoc, buf);
	if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
	{
		echo_to_cockpit(ship,
				FB_RED
				"W systemie panuje burza elektromagnetyczna, systemy zaczynaj� 'siada�'");
		ship->cloack = 0;
		ship->interdict = 0;
		ship->shield = 0;
		ship->autorecharge = false;
	}

}

DEF_DO_FUN( land )
{
	char arg[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int chance, count, emp;
	SHIP_DATA *ship;
	SHIP_DATA *target;
	PLANET_DATA *planet;
	DOCK_DATA *dock;
	SPACE_DATA *starsystem;
	HANGAR_DATA *hangar;
	MOON_DATA *moon;

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
		send_to_char(FB_RED "Musisz siedzie� w fotelu pilota by to zrobi�!"
		EOL, ch);
		return;
	}

	if (!check_crew(ship, ch, "pilot") && arg[0] != '\0')
	{
		send_to_char(FB_RED "Nie zajmujesz stanowiska pilota na tym statku."
		EOL, ch);
		return;
	}

	if (ship->shipstate1 != 0)
	{
		send_to_char(FB_RED "BzZzzbBbbzZzz..." EOL, ch);
		return;
	}
	if (ship->shipstate == SHIP_LAND || ship->shipstate == SHIP_LAND_2)
	{
		send_to_char(FB_RED "Ten statek przeciez laduje!!" EOL, ch);
		return;
	}
	if (ship->shipstate == SHIP_TRACTORED && arg[0] != '\0')
	{
		send_to_char(FB_RED
		"Statek jest unieruchomiony promieiami �ci�gaj�cymi."
		EOL, ch);
		return;
	}

	if (autofly(ship) && arg[0] != '\0')
	{
		send_to_char(FB_GREEN "[Autopilot]: " FB_CYAN
		"Procedura l�dowania zainicjowana...." EOL, ch);
	}

	if (is_platform(ship) && arg[0] != '\0')
	{
		send_to_char(FB_RED "Nie mo�esz wyl�dowa� platform�!" EOL, ch);
		return;
	}

	if (is_huge(ship) && arg[0] != '\0')
	{
		send_to_char(FB_RED
		"Ten statek jest zbyt du�y by wyl�dowa�. We� lepiej szalup�."
		EOL, ch);
		return;
	}

	if (ship->size > 160)
	{
		send_to_char(FB_RED
		"Ten statek jest zbyt du�y by wyl�dowa�. We� lepiej szalup�."
		EOL, ch);
		return;
	}

	if (ship->shipstate == SHIP_DISABLED && arg[0] != '\0')
	{
		send_to_char(FB_RED
		"Nap�d statku jest uszkodzony. L�dowanie niemo�liwe."
		EOL, ch);
		return;
	}

	if (ship->shipstate == SHIP_DOCKED)
	{
		send_to_char(FB_RED "Ten statek jest ju� chyba zadokowany!" EOL, ch);
		return;
	}

	if (ship->shipstate == SHIP_HYPERSPACE)
	{
		send_to_char(FB_RED "Nie mo�esz zrobic tego w hiperprzestrzeni!" EOL,
				ch);
		return;
	}

	if (ship->shipstate != SHIP_READY && arg[0] != '\0')
	{
		send_to_char(FB_RED
		"Poczekaj, a� statek zako�czy sw�j aktualny manewr." EOL, ch);
		return;
	}

	if ((starsystem = ship->starsystem) == NULL)
	{
		send_to_char(FB_RED "Ale tu nie ma gdzie l�dowa�!" EOL, ch);
		return;
	}

	if (ship->energy < (25 + 2 * (ship->size / 10)))
	{
		send_to_char(FB_RED "Za ma�o paliwa!" EOL, ch);
		return;
	}

	emp = 1;
	if (IS_SET(starsystem->flags, STARS_FLAG_EMP))
		emp = 2;

	if (arg1[0] == '\0')
	{
		pager_printf(ch, "Wyl�dowa� gdzie?" NL NL "Oto dost�pne l�dowiska:" NL);

		for (auto* planet : starsystem->planets)
			for (auto* dock : planet->docks)
			{
				if (IS_SET(ch->act, PLR_HOLYLIGHT)
						|| (srange((ship->vx - planet->xpos),
								(ship->vy - planet->ypos),
								(ship->vz - planet->zpos))
								< (ship->comm * 3500 + planet->radius) / emp
								&& dock->hidden == 0))
				{
					pager_printf(ch, " %s %s (%s)  %.0f %.0f %.0f" EOL,
							dock->capacity - check_capacity(dock->vnum)
									- (ship->size / 10) > 0 ?
									FB_GREEN "***" PLAIN : FB_GREEN "*"
									FB_RED "F" FB_GREEN "*" PLAIN, dock->name,
							planet->name, planet->xpos - ship->vx,
							planet->ypos - ship->vy, planet->zpos - ship->vz);
				}
				else if (srange((ship->vx - planet->xpos),
						(ship->vy - planet->ypos), (ship->vz - planet->zpos))
						< (ship->comm * 7000 + planet->radius) / emp
						&& dock->hidden == 0)
				{
					pager_printf(ch,
							"     Planetarny sygna� ILS:  %.0f %.0f %.0f" NL,
							planet->xpos - ship->vx
									+ number_range(-20, 20) * 10,
							planet->ypos - ship->vy
									+ number_range(-20, 20) * 10,
							planet->zpos - ship->vz
									+ number_range(-20, 20) * 10);
				}
				else
					pager_printf(ch, " \r");
			}

		for (auto* moon : starsystem->moons)
			if (moon->vnum != 0)
			{
				if (IS_SET(ch->act, PLR_HOLYLIGHT)
						|| (srange((ship->vx - moon->xpos),
								(ship->vy - moon->ypos),
								(ship->vz - moon->zpos))
								< ship->comm * 3500 / emp))
				{
					pager_printf(ch, " %s %s %.0f %.0f %.0f" EOL,
							moon->capacity - check_capacity(moon->vnum) > 0 ?
									FB_GREEN "***" PLAIN : FB_GREEN "*"
									FB_RED "F" FB_GREEN "*" PLAIN, moon->name,
							moon->xpos - ship->vx, moon->ypos - ship->vy,
							moon->zpos - ship->vz);
				}
				else if (srange((ship->vx - moon->xpos),
						(ship->vy - moon->ypos), (ship->vz - moon->zpos))
						< ship->comm * 7000 / emp)
				{
					pager_printf(ch,
							"     Planetarny sygna� ILS:  %.0f %.0f %.0f" NL,
							moon->xpos - ship->vx + number_range(-20, 20) * 10,
							moon->ypos - ship->vy + number_range(-20, 20) * 10,
							moon->zpos - ship->vz + number_range(-20, 20) * 10);
				}
				else
					pager_printf(ch, " \r");
			}

		for (auto* target : ship->starsystem->ships)
		{
			if (target->cloack != 0
					|| (!has_hangar(target) && !has_dock(target))
					|| srange((target->vx - ship->vx), (target->vy - ship->vy),
							(target->vz - ship->vz)) > ship->comm * 3500 / emp)
				continue;

			if (srange((target->vx - ship->vx), (target->vy - ship->vy),
					(target->vz - ship->vz)) > ship->comm * 1500 / emp)
			{
				if (has_hangar(target) && target != ship)
					pager_printf(ch, "     Sygna� ILS:   %.0f %.0f %.0f" NL,
							target->vx - ship->vx + number_range(-20, 20) * 10,
							target->vy - ship->vy + number_range(-20, 20) * 10,
							target->vz - ship->vz + number_range(-20, 20) * 10);
				continue;
			}

			if ((has_hangar(target) || has_dock(target)) && target != ship)
			{
				pager_printf(ch, "     " PLAIN "%s    %.0f %.0f %.0f" EOL,
						know_trans(ship, target) ?
								SHIPNAME(target) : target->transponder,
						target->vx - ship->vx, target->vy - ship->vy,
						target->vz - ship->vz);
				count = 0;

				for (auto* hangar : target->hangars)
				{
					pager_printf(ch,
							" " FB_GREEN "*" PLAIN "%s%s    hangar nr " FB_YELLOW "%d" EOL,
							hangar->capacity - check_capacity(hangar->vnum)
									> 0 ?
							FB_GREEN "*" PLAIN :
											FB_RED "F" PLAIN,
							hangar->status == -1 ?
							FB_RED "C" PLAIN :
													FB_GREEN "*" PLAIN, count);
					++count;
				}

				count = 0;

				for (auto* shdock : target->docks)
				{
					pager_printf(ch,
							" " PLAIN "%s    cuma   nr " FB_YELLOW "%d" PLAIN "   typu: %s" EOL,
							shdock->target == NULL ?
							FB_GREEN "***" PLAIN :
														FB_RED "BFC" PLAIN,
							count, ship_dock_type[shdock->type]);
					++count;
				}
			}
		}
		pager_printf(ch, NL "B - zaj�ty, F - brak miejsc, C - zamkni�ty" NL);
		return;
	}

	if ((target = get_ship_here(arg1, ship->starsystem)) != NULL)
	{
		if (target == ship)
		{
			send_to_char(
					FB_RED "Nie mo�esz wyl�dowa� na statku, w kt�rym jeste�!" EOL,
					ch);
			return;
		}

		if (target->cloack != 0)
		{
			send_to_char(
					FB_RED "Nie ma tu takiej stacji. Wpisz 'land' by zobaczy� dost�pne l�dowiska!" EOL,
					ch);
			return;
		}

		if (!has_hangar(target))
		{
			send_to_char(FB_RED "W tym statku nie ma hangaru!" EOL, ch);
			return;
		}

		if (ship->size > target->size)
		{
			send_to_char(FB_RED "Ten statek jest mniejszy od Twojego!" EOL, ch);
			return;
		}

		/* Pixel: z zasady nie, ale to powinno byc ograniczone wylaczenie rozmiarem hangaru i statku
		 if(ship->clazz == target->clazz)
		 {
		 send_to_char(FB_RED "Niestety statki podobnej klasy nie mieszcz� si� w sobie nawzajem!" EOL, ch);
		 return;
		 }*/

		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > ship->comm * 1500 / emp)
		{
			send_to_char(
					FB_RED "Komputer nie mo�e poprawnie namierzy� sygna�u ILS" EOL,
					ch);
			return;
		}

		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz))
				> (200 + (ship->size + target->size) / 10) / emp)
		{
			send_to_char(
					FB_RED "To za daleko! Musisz podlecie� nieco bli�ej." EOL,
					ch);
			return;
		}

		if ((hangar = get_hangar(target, atoi(arg2))) == NULL)
		{
			send_to_char(FB_RED "Ten statek nie ma takiego hangaru." EOL, ch);
			return;
		}

		if (hangar->status == -1)
		{
			send_to_char(FB_RED "Hangar jest zamkni�ty" EOL, ch);
			return;
		}

		if (check_capacity(hangar->vnum) + ship->size / 10 > hangar->capacity)
		{
			sprintf(buf,
					FB_RED "Navigator" FB_CYAN " og�asza przez komunikator 'Brak wolnych miejsc.'" EOL);
			send_to_char(buf, ch);
			return;
		}

	}
	else if ((dock = dock_from_name(ship->starsystem, arg1)) != NULL)
	{
		if ((planet = planet_from_dock(dock, ch)) != NULL)
		{
			if (srange((planet->xpos - ship->vx), (planet->ypos - ship->vy),
					(planet->zpos - ship->vz))
					> (ship->comm * 3500 + planet->radius) / emp)
			{
				send_to_char(
						FB_RED "Komputer nie mo�e poprawnie namierzy� sygna�u ILS" EOL,
						ch);
				return;
			}

			if (srange((planet->xpos - ship->vx), (planet->ypos - ship->vy),
					(planet->zpos - ship->vz))
					> (30 + planet->gravity + ship->size / 10 + planet->radius)
							/ emp) //Pixel: 300+
			{
				send_to_char(
						FB_RED "To za daleko! Musisz podlecie� nieco bli�ej." EOL,
						ch);
				return;
			}

			if (check_capacity(dock->vnum) + ship->size / 10 > dock->capacity)
			{
				sprintf(buf,
						FB_CYAN "[Kontrola lot�w %s] :" FB_RED " Przykro nam nie mamy wolnych miejsc." EOL,
						planet->name);
				send_to_char(buf, ch);
				return;
			}

			if (dock->capacity <= 0 || dock->vnum == 0)
			{
				sprintf(buf,
						FB_CYAN "[Komputer pok�adowy] :" FB_RED "BzzzBzZyp" EOL);
				send_to_char(buf, ch);
				return;
			}
		}
	}
	else if ((moon = moon_from_name(ship->starsystem, arg1)) != NULL)
	{
		if (srange((moon->xpos - ship->vx), (moon->ypos - ship->vy),
				(moon->zpos - ship->vz)) > ship->comm * 3500 / emp)
		{
			send_to_char(
					FB_RED "Komputer nie mo�e poprawnie namierzy� sygna�u ILS" EOL,
					ch);
			return;
		}

		if (srange((moon->xpos - ship->vx), (moon->ypos - ship->vy),
				(moon->zpos - ship->vz))
				> (300 + moon->gravity + ship->size / 10) / emp)
		{
			send_to_char(
					FB_RED "To za daleko! Musisz podlecie� nieco bli�ej." EOL,
					ch);
			return;
		}

		if (check_capacity(moon->vnum) + ship->size / 10 > moon->capacity)
		{
			sprintf(buf,
					FB_CYAN "[Kontrola lot�w %s] :" FB_RED " Przykro nam nie mamy wolnych miejsc." EOL,
					moon->name);
			send_to_char(buf, ch);
			return;
		}

		if (moon->capacity <= 0 || moon->vnum == 0)
		{
			sprintf(buf,
					FB_CYAN "[Komputer pok�adowy] :" FB_RED "BzzzBzZyp" EOL);
			send_to_char(buf, ch);
			return;
		}
	}
	else
	{
		send_to_char(
				FB_RED "Nie ma tu takiego l�dowiska." NL "   Wpisz 'LAND' a uzyskasz spis dost�pnych l�dowisk" NL,
				ch);
		return;
	}

	chance = get_space_chance(ship, ch);
	if (number_percent() < chance)
	{
		send_to_char(FB_GREEN, ch);
		send_to_char("Procedura l�dowania zainicjowana." NL, ch);
		act(PLAIN, "$n kieruje statek do l�dowania.", ch, NULL, argument,
				TO_ROOM);
		echo_to_ship(ship, FB_YELLOW "Statek powoli podchodzi do l�dowania.");

		STRDUP(ship->dest, arg);
		ship->shipstate = SHIP_LAND;
		ship->currspeed = 0;

		learn_from_success_space(ship, ch);

		if (starsystem_from_vnum(VNUM(ship->lastdoc)) != ship->starsystem)
		{
			int xp = ((exp_level(ch->skill_level[PILOTING_ABILITY] + 1)
					- exp_level(ch->skill_level[PILOTING_ABILITY])) / 4);
			xp = UMIN(get_ship_value(ship) / 1000, xp);
			gain_exp(ch, xp, PILOTING_ABILITY);

			ch_printf(ch,
					FB_WHITE "Zdobywasz %ld punkt�w do�wiadczenia w pilotowaniu!" NL,
					UMIN(get_ship_value(ship), xp));
		}
		fevent_trigger(ch, FE_LAND_SHIP, ship);
		return;
	}
	send_to_char("Jako� nie uda�o ci si� poprawnie obs�u�y� urz�dze�." NL, ch);
	learn_from_failure_space(ship, ch);
	return;
}

void landship(SHIP_DATA *ship, char *argument)
{
	SHIP_DATA *target;
	DOCK_DATA *dock;
	HANGAR_DATA *hangar;
	MOON_DATA *moon;
	char buf[MAX_STRING_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	int destination = 0;
	float cur_cap = 0;
	float maxcap = 0;

	argument = one_argument(argument, arg1);
	if ((dock = dock_from_name(ship->starsystem, arg1)) != NULL)
	{
		destination = dock->vnum;
		maxcap = dock->capacity;
		cur_cap = check_capacity(dock->vnum);
	}
	if ((moon = moon_from_name(ship->starsystem, arg1)) != NULL)
	{
		destination = moon->vnum;
		maxcap = 1000;
		cur_cap = 1;
	}
	target = get_ship_here(arg1, ship->starsystem);

	if (target != ship && target != NULL
			&& ((hangar = get_hangar(target, atoi(argument))) != NULL))
	{
		destination = hangar->vnum;
		cur_cap = check_capacity(hangar->vnum);
		maxcap = hangar->capacity;
	}
	if (!ship_to_room(ship, get_room_index(destination))
			|| cur_cap + ship->size > maxcap)
	{
		echo_to_room(ship->pilotseat,
		FB_YELLOW
		"Nie mo�na wyl�dowa�! Sekwencja l�dowania przerwana.");
		echo_to_ship(ship,
		FB_YELLOW
		"Statek wzbija si� do g�ry przerywaj�c l�dowanie.");
		if (ship->shipstate != SHIP_DISABLED)
			ship->shipstate = SHIP_READY;
		return;
	}

	echo_to_room(ship->pilotseat, FB_YELLOW "Procedura l�dowania zako�czona.");
	echo_to_ship(ship,
	FB_YELLOW
	"Czujesz delikatny wstrz�s, gdy statek osiada na powierzchni.");
	ship->last_dock_with = NULL;
	if (ship->cloack == 0)
	{
		sprintf(buf, FB_YELLOW "%s l�duje na l�dowisku.", ship->sslook);
		echo_to_system(ship, buf, NULL);
	}
	if (ship->cloack != 0)
		ship->cloack = 0;
	if (ship->sx != 0 || ship->sy != 0 || ship->sz != 0)
	{
		ship->sx = 0;
		ship->sy = 0;
		ship->sz = 0;
	}
	if (ship->vx != 0 || ship->vy != 0 || ship->vz != 0)
	{
		ship->vx = 0;
		ship->vy = 0;
		ship->vz = 0;
	}
	STRDUP(ship->sslook, "");
	ship->location = get_room_index(destination);
	ship->lastdoc = get_room_index(destination);
	if (ship->shipstate != SHIP_DISABLED)
		ship->shipstate = SHIP_DOCKED;
	ship_from_starsystem(ship, ship->starsystem);

	sprintf(buf, FB_YELLOW "%s l�duje na platformie.", SHIPNAME(ship));
	echo_to_room(ship->location, buf);

	ship->energy = ship->energy - 25 - 2 * (ship->size / 10);

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
	save_ship(ship);
}

DEF_DO_FUN( accelerate )
{
	int chance;
	int change;
	int energychange;
	SHIP_DATA *ship, *target;
	char buf[MAX_STRING_LENGTH];

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�!" NL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz siedzie� w fotelu pilota..." NL, ch);
		return;
	}

	if (!check_crew(ship, ch, "pilot"))
	{
		send_to_char(FB_RED "Nie zajmujesz stanowiska pilota na tym statku."
		NL, ch);
		return;
	}

	/********/energychange = abs(
			(atoi(argument) - abs(ship->currspeed)) * ship->size / 100);
	if (ship->energy < energychange)
	{
		send_to_char(FB_RED "Za ma�o paliwa!" NL, ch);
		return;
	}
	chance = get_space_chance(ship, ch);
	if (number_percent() >= chance)
	{
		send_to_char(FB_RED "Nie uda�o ci si� poprawnie obs�u�y� urz�dze�."
		NL, ch);
		learn_from_failure_space(ship, ch);
		return;
	}

	change = atoi(argument);

	act(PLAIN, "$n wciska jakie� przyciski na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);

	if (autofly(ship))
		send_to_char(FB_YELLOW "[Autopilot]: ", ch);
	if (change > ship->currspeed)
	{
		send_to_char("&GPrzyspieszanie" NL, ch);
		echo_to_cockpit(ship, FB_YELLOW "Statek zaczyna przyspiesza�.");
		if (ship->cloack == 0)
		{
			sprintf(buf, FG_YELLOW "%s przyspiesza.", ship->sslook);
			echo_to_system(ship, buf, NULL);
		}
	}

	if (change < ship->currspeed)
	{
		send_to_char("&GZwolnienie" NL, ch);
		echo_to_cockpit(ship, FB_YELLOW "Statek zaczyna zwalnia�.");
		if (ship->cloack == 0)
		{
			sprintf(buf, FG_YELLOW "%s zwalnia.", ship->sslook);
			echo_to_system(ship, buf, NULL);
		}
	}

	if (autofly(ship))
	{
		send_to_char(FB_RED "    wy��czam si�..." EOL, ch);
		ship->autopilot = false;
	}
	ship->energy -= URANGE(1, energychange, 300);

	ship->currspeed = URANGE(0, change, ship->realspeed);

	for (auto* dock : ship->docks)
	{
		if ((target = dock->target) != NULL)
		{
			echo_to_cockpit(target, FB_YELLOW "Zmiana pr�dko�ci." NL);
			target->currspeed = ship->currspeed;
			ship->energy -= target->size / 100;
		}
	}
	learn_from_success_space(ship, ch);
}

DEF_DO_FUN( trajectory )
{
	char buf[MAX_STRING_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	int chance;
	float vx, vy, vz;
	SHIP_DATA *ship, *target;
	ASTRO_DATA *astro;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�!" NL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Nie siedzisz w fotelu pilota." NL, ch);
		return;
	}

	if (!check_crew(ship, ch, "pilot"))
	{
		send_to_char(FB_RED "Nie zajmujesz stanowiska pilota na tym statku."
		NL, ch);
		return;
	}

	if (ship->shipstate != SHIP_READY)
	{
		send_to_char(FB_RED "Poczekaj a� statek zako�czy aktualny manewr." NL,
				ch);
		return;
	}
	if (ship->energy < (ship->currspeed * ship->size) / 100)
	{
		send_to_char(FB_RED "Masz za ma�o paliwa!" NL, ch);
		return;
	}

	chance = get_space_chance(ship, ch);
	if (number_percent() > chance)
	{
		send_to_char(FB_RED "Nie uda�o ci si� poprawnie obs�u�y� urz�dze�."
		NL, ch);
		learn_from_failure_space(ship, ch);
		return;
	}

	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);
	if (!str_cmp(arg2, "back") || !str_cmp(arg2, "ty�"))
	{
		vx = -ship->hx + ship->vx;
		vy = -ship->hy + ship->vy;
		vz = -ship->hz + ship->vz;
	}
	else if (is_number(arg2))
	{
		vx = atoi(arg2) + ship->vx;
		vy = atoi(arg3) + ship->vy;
		vz = atoi(argument) + ship->vz;
	}
	else if (!is_number(arg2))
	{
		target = get_ship_here(arg2, ship->starsystem);
		astro = get_astro_here(arg2, ship->starsystem);
		if (target != NULL && target->cloack == 0
				&& srange((target->vz - ship->vz), (target->vy - ship->vy),
						(target->vx - ship->vx))
						<= ship->sensor * 3900 + target->size * 90)
		{
			vx = target->vx;
			vy = target->vy;
			vz = target->vz;
		}
		else if (astro != NULL
				&& srange((ship->vx - astro->ox), (ship->vy - astro->oy),
						(ship->vz - astro->oz))
						< (astro->value + ship->sensor) * 150)
		{
			vx = astro->ox;
			vy = astro->oy;
			vz = astro->oz;
		}
		else
		{
			ch_printf(ch,
			FB_RED "W kierunku czego chcesz obr�ci� sw�j statek?"
			NL);
			return;
		}
	}
	else
		return;
	if (vx == ship->vx && vy == ship->vy && vz == ship->vz)
	{
		ch_printf(ch, "Statek jest ju� w pozycji solarnej %.0f %.0f %.0f !" NL,
				vx, vy, vz);
	}

	ship->hx = vx - ship->vx;
	ship->hy = vy - ship->vy;
	ship->hz = vz - ship->vz;

	ship->energy -= ((ship->currspeed * ship->size) / 100);

	for (auto* dock : ship->docks)
	{
		if ((target = dock->target) != NULL)
		{
			echo_to_cockpit(target, FB_YELLOW "Zmiana kursu." NL);
			target->hx = ship->hx;
			target->hy = ship->hy;
			target->hz = ship->hz;
			ship->energy -= target->size / 100;
		}
	}
	if (autofly(ship))
		ch_printf(ch, FB_YELLOW "[Autopilot]: ");

	ch_printf(ch, FB_GREEN "Nowy kurs ustalony. Zwrot na: %.0f %.0f %.0f ." NL
	"Solarne wsp�rz�dne docelowe: %.0f %.0f %.0f ." NL, vx - ship->vx,
			vy - ship->vy, vz - ship->vz, vx, vy, vz);

	act(PLAIN, "$n manipuluje d�wigniami sterowania.", ch, NULL, argument,
	TO_ROOM);

	echo_to_cockpit(ship, FB_YELLOW "Statek zaczyna zmienia� kurs." NL);
	if (ship->cloack == 0)
	{
		sprintf(buf, FG_YELLOW "%s zmienia obecny kurs.", ship->sslook);
		echo_to_system(ship, buf, NULL);
	}

	if (ship->manuever >= 100)
		ship->shipstate = SHIP_BUSY_1;
	else if (ship->manuever >= 90)
		ship->shipstate = SHIP_BUSY_2;
	else if (ship->manuever >= 80)
		ship->shipstate = SHIP_BUSY_3;
	else if (ship->manuever >= 70)
		ship->shipstate = SHIP_BUSY_4;
	else if (ship->manuever >= 60)
		ship->shipstate = SHIP_BUSY_5;
	else if (ship->manuever >= 50)
		ship->shipstate = SHIP_BUSY_6;
	else if (ship->manuever >= 40)
		ship->shipstate = SHIP_BUSY_7;
	else if (ship->manuever >= 30)
		ship->shipstate = SHIP_BUSY_8;
	else if (ship->manuever >= 20)
		ship->shipstate = SHIP_BUSY_9;
	else if (ship->manuever >= 10)
		ship->shipstate = SHIP_BUSY_10;
	else
		ship->shipstate = SHIP_BUSY_10;

	learn_from_success_space(ship, ch);

}

DEF_DO_FUN( buyship )
{
	long price;
	SHIP_DATA *ship;

	if (IS_NPC(ch) || !ch->pcdata)
	{
		huh(ch);
		return;
	}

	ship = ship_in_room(ch->in_room, argument);
	if (!ship)
	{
		ship = ship_from_cockpit(ch->in_room);

		if (!ship)
		{
			act(PLAIN, "Nie ma tu �adnego $T.", ch, NULL, argument, TO_CHAR);
			return;
		}
	}

	if (str_cmp(ship->owner, "") || ship->type == MOB_SHIP)
	{
		send_to_char(FB_RED "Ten statek nie jest na sprzeda�." EOL, ch);
		return;
	}

	price = get_ship_value(ship);

	if (ch->gold < price)
	{
		ch_printf(ch,
		FB_RED "Ten statek kosztuje %ld. Nie masz tyle kredytek!"
		EOL, price);
		return;
	}

	ch->gold -= price;
	ch_printf(ch, FB_GREEN "P�acisz %ld kredytek za nabycie statku." EOL,
			price);

	act(PLAIN, "$n podchodzi do terminala i dokonuje transakcji finansowej.",
			ch, NULL, argument, TO_ROOM);

	STRDUP(ship->owner, ch->name);
	save_ship(ship);
}

DEF_DO_FUN( clanbuyship )
{
	long price;
	SHIP_DATA *ship;
	CLAN_DATA *clan;

	if (IS_NPC(ch) || !ch->pcdata)
	{
		huh(ch);
		return;
	}

	if (!ch->pcdata->clan)
	{
		huh(ch);
		return;
	}

	clan = ch->pcdata->clan;

	if (!clan_bestow(ch, "clanbuyship"))
	{
		huh(ch);
		return;
	}

	ship = ship_in_room(ch->in_room, argument);
	if (!ship)
	{
		ship = ship_from_cockpit(ch->in_room);

		if (!ship)
		{
			act(PLAIN, "Nie widz� tu $T.", ch, NULL, argument, TO_CHAR);
			return;
		}
	}

	if (str_cmp(ship->owner, "") || ship->type == MOB_SHIP)
	{
		send_to_char(FB_RED "Ten statek nie jest na sprzeda�." NL, ch);
		return;
	}

	price = get_ship_value(ship);

	if (ch->pcdata->clan->funds < price)
	{
		ch_printf(ch, FB_RED "Ten statek kosztuje %ld kredytek." EOL
		FB_RED " Tw�j klan nie dysponuje tak� sum�!" EOL, price);
		return;
	}

	clan->funds -= price;
	ch_printf(ch, FB_GREEN "%s p�aci %ld kredytek za zakup statku." EOL,
			clan->name, price);

	act(PLAIN, "$n podchodzi do terminala i wykonuje transakcje finansow�.", ch,
			NULL, argument, TO_ROOM);

	STRDUP(ship->owner, clan->name);
	save_ship(ship);

	if (is_ship(ship))
		clan->spacecrafts++;
	else
		clan->vehicles++;
}

DEF_DO_FUN( sellship )
{
	SHIP_DATA *ship;
	long price;
	bool owner = false;

	if (IS_NPC(ch))
	{
		huh(ch);
		return;
	}

	ship = ship_in_room(ch->in_room, argument);
	if (!ship)
	{
		act(PLAIN, "Nie widz� tu �adnego $T.", ch, NULL, argument, TO_CHAR);
		return;
	}

	if (!str_cmp(ship->owner, ch->name))
		owner = true;
	else if (IS_CLANNED(ch) && is_name("clansellship", ch->pcdata->bestowments)
			&& !str_cmp(ship->owner, ch->pcdata->clan->name))
		owner = true;
	else if (IS_LEADER(ch) && !str_cmp(ship->owner, ch->pcdata->clan->name))
		owner = true;
	else
		owner = false;

	if (!owner)
	{
		send_to_char(FB_RED "To nie tw�j statek!" EOL, ch);
		return;
	}

	price = get_ship_value(ship);

	ch->gold += (price - price / 10);
	ch_printf(ch, FB_GREEN "Otrzymujesz %ld kredytek za sprzeda� statku." EOL,
			price - price / 10);

	act(PLAIN, "$n podchodzi do terminala i dokonuje transakcji finansowej.",
			ch, NULL, argument, TO_ROOM);

	STRDUP(ship->owner, "");
	STRDUP(ship->pilot, "");
	STRDUP(ship->copilot, "");
	STRDUP(ship->engineer, "");
	save_ship(ship);
}

DEF_DO_FUN( info )
{
	SHIP_DATA *ship;
	SHIP_DATA *target;
	char buf[MSL];
	int emp;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		if (argument[0] == '\0')
		{
			act(PLAIN, "Informacje na temat jekiego statku chcesz otrzyma�?",
					ch, NULL, NULL, TO_CHAR);
			return;
		}

		if (!get_comlink(ch))
		{
			act(PLAIN, "By to zrobi� potrzebujesz urz�dzenia komunikacyjnego!",
					ch, NULL, NULL, TO_CHAR);
			return;
		}

		ship = ship_in_room(ch->in_room, argument);
		if (!ship)
		{
			ch_printf(ch, "Nie widzisz tu �adnego %s." EOL
			"Spr�buj 'radar'." NL, argument);
			return;
		}

		target = ship;
	}
	else if (argument[0] == '\0')
		target = ship;
	else
		target = get_ship_here(argument, ship->starsystem);

	if (!known_biotech(ship, ch))
	{
		send_to_char(
				FB_RED
				"Jeste� wewn�trz wytworu obcej biotechnologi, niestety nie masz o niej zielonego poj�cia."
				EOL, ch);
		return;
	}
	if (target == NULL)
	{
		send_to_char("Nie ma tu nic takiego." NL, ch);
		return;
	}
	if (target->cloack >= 1 && target != ship)
	{
		send_to_char("Nie ma tu nic takiego." NL, ch);
		return;
	}

	emp = 1;
	if (ship->starsystem
			!= NULL && IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
		emp = 2;
	if (srange((target->vx - ship->vx), (target->vy - ship->vy),
			(target->vz - ship->vz)) * emp
			> ship->target_array * 800 + target->size * 10)
	{
		send_to_char("Ten statek jest zbyt daleko, by si� dowiedzie�." NL, ch);
		return;
	}

	if (target->type != SHIP_YUUZHAN)
	{
		/* Thanos was here.      Hej Alde :) */
		{
			char _buf[MSL];

			strcpy(_buf, bit_name(ship_classes_name_list, target->clazz));

//      if( _buf[strlen(_buf-1)] != 'a' )
//      {
			sprintf(buf,
					target->type == SHIP_REPUBLIC ?
							"NowoRepublika�ski" :
							(target->type == SHIP_IMPERIAL ?
									"Imperialny" :
									(target->type == SHIP_PIRATE ?
											"Piracki" : "Cywilny")));

			/*	}
			 else
			 {
			 sprintf( buf, target->type == SHIP_REPUBLIC	? "NowoRepublika�sk�" :
			 (target->type == SHIP_IMPERIAL	? "Imperialn�" :
			 "Cywiln�" ) );
			 }
			 */
			pager_printf(ch, "Patrzysz na " FB_WHITE "%s" PLAIN " statek klasy:"
			FB_WHITE " %s" EOL "o nazwie :" FB_WHITE " %s" EOL
			"Nazwa fabryczna:" FB_WHITE " %s" EOL, buf, _buf, SHIPNAME(target),
					target->name);
		}

		pager_printf(ch, "Klasa: %s" EOL,
				is_scout(target) ?
						"szkoleniowy" :
						(is_fighter(target) ?
								"my�liwiec" :
								(is_midship(target) ?
										"�rednia" :
										(is_freighter(target) ?
												"frachtowiec" :
												(is_capital(target) ?
														"okr�t" :
														(is_huge(target) ?
																"supership" :
																(is_platform(
																		target) ?
																		"stacja kosmiczna" :
																		"Hmm jaki� b��d albo speeder")))))));

		if (*target->description) /* I tutaj ja -- Than */
			pager_printf(ch, "%s" EOL, target->description);
		pager_printf(ch, "Dzia�ka Laserowe:    %d    Dzia�ka Jonowe:    %d" NL,
				target->lasers, target->ioncannons);
		pager_printf(ch, "Max. Ilo�� pocisk�w: %d / %d / %d   %s" NL,
				target->maxmissiles, target->torpedos, target->rockets,
				target->trawler == 0 ? "" : "Zbieracz Min");
		pager_printf(ch, "Max. Ilo�� Flar:     %d" NL, target->maxchaff);
		pager_printf(ch, "Max. Wytrzyma�o�� Kad�ubu: %d" NL, target->maxhull);
		pager_printf(ch, "Max. Moc Tarczy: %d   Max. Energia(paliwo): %d" NL,
				target->maxshield, target->maxenergy);
		pager_printf(ch,
				"Max. Pr�dko��:   %.0f    Hiperpr�dko��:        %.0f" NL,
				target->realspeed, target->hyperspeed);
		pager_printf(ch, "Radar:           Mk%d  System rozpoznawczy:  Mk%d" NL,
				target->sensor, target->target_array);
		pager_printf(ch, "�adowno�� %d" NL, target->maxcargo);
	}
	else
	{
		yuuzhan_info(target, ch);
	}
	act(PLAIN,
			"$n sprawdza wska�niki kontrolek i stan monitor�w na panelu sterowania.",
			ch, NULL, argument, TO_ROOM);
}

DEF_DO_FUN( autorecharge )
{
	int chance;
	SHIP_DATA *ship;
	int recharge;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�." EOL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if ((ship = ship_from_coseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz siedzie� w fotelu drugiego pilota." EOL,
				ch);
		return;
	}

	if (autofly(ship))
	{
		send_to_char(FB_RED "Musisz wy��czy� najpierw autopilota." EOL, ch);
		return;
	}

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_shipsystems]);

	if (number_percent() > chance)
	{
		send_to_char(FB_RED "Nie uda�o ci si� poprawnie obs�u�y� urz�dze�."
		EOL, ch);
		learn_from_failure(ch, gsn_shipsystems);
		return;
	}

	act(PLAIN, "$n przestawia jak�� d�wigni� na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);

	if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
	{
		send_to_char(FB_RED
		"W systemie panuje burza elektromagnetyczna, tarcze s� sparali�owane!"
		NL, ch);
		return;
	}

	if (!str_cmp(argument, "on"))
	{
		ship->autorecharge = true;
		send_to_char(FB_GREEN "W��czasz tarcze." EOL, ch);
		echo_to_cockpit(ship,
		FB_YELLOW "Tarcze W��CZONE. Auto�adowanie W��CZONE.");
	}
	else if (!str_cmp(argument, "off"))
	{
		ship->autorecharge = false;
		send_to_char(FB_GREEN "Wy��czasz tarcze." EOL, ch);
		echo_to_cockpit(ship,
		FB_YELLOW
		"Tarcze WY��CZONE. Si�a pola 0. Energia przekazana do zasob�w"
		NL "Auto�adowanie WY��CZONE.");
		ship->energy += ship->shield / 2;
		ship->shield = 0;
		if (ship->energy > ship->maxenergy)
			ship->energy = ship->maxenergy;
	}
	else if (!str_cmp(argument, "idle"))
	{
		ship->autorecharge = false;
		send_to_char(FB_GREEN "Wy��czasz auto�adowanie." EOL, ch);
		echo_to_cockpit(ship,
		FB_YELLOW
		"Auto�adowanie WY��CZONE. Tarcze podtrzymane.");
	}
	else
	{
		if (ship->autorecharge == true)
		{
			ship->autorecharge = false;
			send_to_char(FB_GREEN "Prze��czasz tarcze." EOL, ch);
			echo_to_cockpit(ship,
			FB_YELLOW
			"Auto�adowanie WY��CZONE. Tarcze podtrzymane.");
		}
		else
		{
			ship->autorecharge = true;
			send_to_char(FB_GREEN "Prze��czasz tarcze." EOL, ch);
			echo_to_cockpit(ship,
			FB_YELLOW
			"Tarcze W��CZONE. Auto�adowanie W��CZONE.");
		}
	}

	if (ship->autorecharge)
	{
		recharge = URANGE(1, ship->maxshield - ship->shield,
				25 + ship->size / 2);
		recharge = UMIN(recharge, ship->energy * 5 + 100);
		ship->shield += recharge;
		ship->energy -= recharge / 10;

		if (ship->shield > ship->maxshield)
			ship->shield = ship->maxshield;
	}

	learn_from_success(ch, gsn_shipsystems);
}

DEF_DO_FUN( interdict )
{
	char buf[MAX_STRING_LENGTH];
	SHIP_DATA *ship;
	int chance;

	if ((ship = ship_from_coseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz siedzie� w fotelu drugiego pilota." EOL,
				ch);
		return;
	}

	if (!check_pilot(ch, ship))
	{
		send_to_char(FB_RED "Hej! To nie tw�j statek!" EOL, ch);
		return;
	}

	if (count_modules(ship, 22) == 0)
	{
		send_to_char(FB_RED "To nie jest Interdictor!" EOL, ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
	{
		send_to_char(FB_RED "W systemie panuje burza elektromagnetyczna," NL
		"generator studni grawitacyjnej jest sparali�owany!" NL, ch);
		return;
	}

	act(PLAIN, "$n wciska pare prze�acznik�w na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_advancedsystems]);

	if (number_percent() < chance)
	{
		if (ship->interdict != 0)
		{
			ship->interdict = false;
			send_to_char(FB_GREEN "Wy�aczasz pole wstrzymuj�ce." EOL, ch);
			echo_to_ship(ship, FB_YELLOW "Pole wsztrzymuj�ce WY��CZONE.");
			sprintf(buf,
			FG_YELLOW
			"Nienaturalne wibracje calego kadluba zanikaj�.");
			echo_to_system(ship, buf, NULL);
		}
		else
		{
			ship->interdict = ship->maxinterdict;
			send_to_char(FB_GREEN "W��czasz pole wstrzymuj�ce." EOL, ch);
			echo_to_ship(ship, FB_YELLOW "Pole wstrzymuj�ce W��CZONE.");
			learn_from_success(ch, gsn_advancedsystems);

			if (ship->cloack == true)
			{
				echo_to_system(ship,
						FG_YELLOW
						"!!!UWAGA wykryto sztuczne pole grawitacyjne. Kierunek NIEZNANY",
						NULL);
			}
			else
			{
				sprintf(buf,
						FG_YELLOW
						"Czujesz wibracje calego kadluba. Ich �r�d�o znajduje si� w podli�u %s.",
						ship->sslook);
				echo_to_system(ship, buf, NULL);
			}
		}
	}
	else
	{
		send_to_char(FB_GREEN
		"Manipulujesz przy pulpicie pola wstrzymuj�cego." EOL, ch);
		send_to_char(FB_RED "Nieskutecznie!!!." EOL, ch);

		ship->energy -= 1000;
		if (ship->energy < 0)
			ship->energy = 0;
		ship->maxenergy -= ship->size / 10;

		if (ship->maxenergy < ship->energy)
			ship->energy = ship->maxenergy;

		echo_to_ship(ship, FB_RED "!!!Eksplozja w reaktorze!!!");
		if (ship->maxenergy <= 0)
			destroy_ship(ship, ch);
		learn_from_failure(ch, gsn_advancedsystems);
	}
}

DEF_DO_FUN( camuflage )
{
	int chance;
	char buf[MAX_STRING_LENGTH];
	SHIP_DATA *ship;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�." EOL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if (!check_pilot(ch, ship))
	{
		send_to_char(FB_RED "Hej! To nie tw�j statek!" EOL, ch);
		return;
	}

	if ((ship = ship_from_coseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Nie siedzisz w fotelu drugiego pilota!" EOL, ch);
		return;
	}

	if (ship->maxcloack != 1)
	{
		send_to_char(FB_RED "Ten statek nie ma urz�dzenia maskuj�cego!" EOL,
				ch);
		return;
	}

	if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
	{
		send_to_char(
				FB_RED
				"W systemie panuje burza elektromagnetyczna, system maskuj�cy jest sparali�owany!"
				NL, ch);
		return;
	}

	act(PLAIN, "$n poci�ga za d�wigni� na przystawce do panelu sterowania.", ch,
			NULL, argument, TO_ROOM);

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_advancedsystems]);

	if (number_percent() < chance)
	{
		if (ship->cloack == true)
		{
			ship->cloack = false;
			send_to_char(FB_GREEN "Wy�aczasz system maskuj�cy." EOL, ch);
			echo_to_ship(ship, FB_YELLOW "System maskujacy WY��CZONY.");
			sprintf(buf,
			FB_YELLOW
			"%s powoli wy�ania si� z pod os�on system�w maskuj�cych.",
					ship->sslook);
			echo_to_system(ship, buf, NULL);
		}
		else
		{
			if (ship->energy < 50 * (ship->size / 10))
			{
				send_to_char(FB_RED "Za ma�o energi!" EOL, ch);
				return;
			}
			ship->cloack = true;
			send_to_char(FB_GREEN "W��czasz system maskuj�cy." EOL, ch);
			echo_to_ship(ship, FB_YELLOW "System maskujacy W��CZONY.");
			echo_to_ship(ship,
			FB_RED "Wstrzyma� OGIE�. PE�NA CISZA NA POK�ADZIE");
			sprintf(buf, FB_YELLOW "%s powoli zanika.", ship->sslook);
			echo_to_system(ship, buf, NULL);
			learn_from_success(ch, gsn_advancedsystems);
		}
	}
	else
	{
		send_to_char(FB_GREEN "Manipulujesz przy sytemie maskuj�cym." EOL, ch);
		send_to_char(FB_RED "Nieskutecznie!!!." EOL, ch);
		ship->energy -= (ship->size / 10);
		if (ship->energy < 0)
			ship->energy = 0;
		ship->maxenergy -= (ship->size / 10);
		if (ship->maxenergy < ship->energy)
			ship->energy = ship->maxenergy;
		echo_to_ship(ship, FB_RED "!!!Eksplozja w reaktorze!!!");
		if (ship->maxenergy <= 0)
			destroy_ship(ship, ch);

		learn_from_failure(ch, gsn_advancedsystems);
	}

}
DEF_DO_FUN( autopilot )
{
	SHIP_DATA *ship;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�." NL,
				ch);
		return;
	}

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Nie siedzisz w fotelu pilota!" NL, ch);
		return;
	}

	if (!check_pilot(ch, ship))
	{
		send_to_char(FB_RED "Hej! To nie tw�j statek!" NL, ch);
		return;
	}

	if (ship->autopilot == false && is_ship_fight(ship))
	{
		ch_printf(ch, FB_RED
		"Dop�ki statek jest otoczony przez wroga musisz radzi� sobie sam%s!"
		EOL, SEX_SUFFIX__AO(ch));
		return;
	}

	act(PLAIN, "$n poci�ga za d�wigni� na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);

	if (ship->autopilot == true)
	{
		ship->autopilot = false;
		send_to_char("&GPrze��czasz autopilota." NL, ch);
		echo_to_cockpit(ship, FB_YELLOW "Autopilot WY��CZONY.");
	}
	else
	{
		ship->autopilot = true;
		ship->autorecharge = true;
		send_to_char("&GPrze��czasz autopilota." NL, ch);
		echo_to_cockpit(ship, FB_YELLOW "Autopilot W��CZONY.");
		if (ship->sx != ship->vx || ship->sy != ship->vy
				|| ship->sz != ship->vz)
		{
			ship->sx = ship->vx;
			ship->sy = ship->vy;
			ship->sz = ship->vz;
			save_ship(ship);
		}
	}

}

DEF_DO_FUN( openhatch )
{
	SHIPDOCK_DATA *dock;
	SHIP_DATA *ship;
	char buf[MAX_STRING_LENGTH];
	char arg[MSL];

	if ((ship = ship_from_room(ch->in_room)) != NULL)
	{
		if ((dock = shipdock_from_room(ship, ch->in_room->vnum)) != NULL)
		{
			if (!str_cmp(argument, "dock") || !str_cmp(argument, "�luza")
					|| !str_cmp(argument, "�luz�"))
				;
			{
				do_openshipdock(ch, argument);
				return;
			}
		}
	}
	argument = one_argument(argument, arg);

	if (!*arg || !str_cmp(arg, "hatch"))
	{
		ship = ship_from_entrance(ch->in_room);
		if (ship == NULL)
		{
			send_to_char(FB_RED "Otworzy� co?" NL, ch);
			return;
		}
		else
		{
			if (!ship->hatchopen)
			{
				if (is_platform(ship))
				{
					send_to_char(FB_RED "Spr�buj otworzy� te w dokach!" EOL,
							ch);
					return;
				}

				if (ship->location != ship->lastdoc
						|| (ship->shipstate != SHIP_DOCKED
								&& ship->shipstate != SHIP_DISABLED))
				{
					send_to_char(FB_RED "A nie poczekasz a� statek wyl�duje?"
					EOL, ch);
					return;
				}

				if (!known_biotech(ship, ch))
				{
					ch_printf(ch,
							FB_RED
							"To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia."
							EOL);
					return;
				}

				ship->hatchopen = true;
				send_to_char(FB_GREEN "Otwierasz klap�." EOL, ch);
				act(PLAIN, "$n otwiera klap�.", ch, NULL, arg, TO_ROOM);
				sprintf(buf, FB_YELLOW "Klapa na %s otwiera si�.",
						SHIPNAME(ship));
				echo_to_room(ship->location, buf);
				sound_to_room(ship->entrance, "!!SOUND(door)");
				sound_to_room(ship->location, "!!SOUND(door)");
				return;
			}
			else
			{
				send_to_char(FB_RED "Klapa jest ju� otwarta." EOL, ch);
				return;
			}
		}
	}

	ship = ship_in_room(ch->in_room, arg);
	if (!ship)
	{
		act(PLAIN, "Nie ma tu �adnego $T.", ch, NULL, arg, TO_CHAR);
		return;
	}

	if (ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED)
	{
		send_to_char(FB_RED "Ten statek zacz�� ju� startowa�." EOL, ch);
		return;
	}

	if (!known_biotech(ship, ch))
	{
		ch_printf(ch,
				FB_RED
				"To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia."
				EOL);
		return;
	}

	if (!check_pilot(ch, ship))
	{
		send_to_char(FB_RED "Hej! To nie tw�j statek!" NL, ch);
		return;
	}

	if (!ship->hatchopen)
	{
		if (!str_cmp(ship->lock_key, "0000"))
		{
			ship->hatchopen = true;
			act(PLAIN, "Otwierasz klap� $T.", ch, NULL, SHIPNAME(ship),
			TO_CHAR);
			act(PLAIN, "$n otwiera klap� $T.", ch, NULL, SHIPNAME(ship),
			TO_ROOM);
			echo_to_room(ship->entrance,
			FB_YELLOW "Klapa otwiera si� od zewn�trz.");
			sound_to_room(ship->entrance, "!!SOUND(door)");
			sound_to_room(ship->location, "!!SOUND(door)");
			return;
		}
		else
		{
			if (str_cmp(ship->lock_key, argument))
			{
				act(PLAIN,
						"Wej�cie jest zabezpieczone. Wpisz poprawny kod po nazwie.",
						ch, NULL, SHIPNAME(ship), TO_CHAR);
				act(PLAIN, "$n pr�buje otworzy� $T, ale klapa nie ust�puje.",
						ch, NULL, SHIPNAME(ship), TO_ROOM);
				return;
			}
			else
			{
				ship->hatchopen = true;
				act(PLAIN, "Otwierasz klap� $T.", ch, NULL, SHIPNAME(ship),
				TO_CHAR);
				act(PLAIN, "$n otwiera klap� $T.", ch, NULL, SHIPNAME(ship),
				TO_ROOM);
				echo_to_room(ship->entrance,
				FB_YELLOW "Klapa otwiera si� od zewn�trz.");
				sound_to_room(ship->entrance, "!!SOUND(door)");
				sound_to_room(ship->location, "!!SOUND(door)");
				return;
			}
		}
	}
	send_to_char(FB_GREEN "Klapa jest ju� otwarta." EOL, ch);
	return;
}

DEF_DO_FUN( closehatch )
{
	SHIP_DATA *ship;
	char buf[MAX_STRING_LENGTH];
	SHIPDOCK_DATA *dock;

	if ((ship = ship_from_room(ch->in_room)) != NULL)
	{
		if ((dock = shipdock_from_room(ship, ch->in_room->vnum)) != NULL)
		{
			if (!str_cmp(argument, "dock") || !str_cmp(argument, "�luza")
					|| !str_cmp(argument, "�luz�"))
				;
			{
				do_closeshipdock(ch, argument);
				return;
			}
		}
	}

	if (!argument || argument[0] == '\0' || !str_cmp(argument, "hatch"))
	{
		ship = ship_from_entrance(ch->in_room);
		if (ship == NULL)
		{
			send_to_char(FB_RED "Zamkn�� co?" NL, ch);
			return;
		}
		else
		{
			if (is_platform(ship))
			{
				send_to_char(FB_RED "Spr�buj te w dokach!" NL, ch);
				return;
			}
			if (ship->hatchopen)
			{
				if (!known_biotech(ship, ch))
				{
					ch_printf(ch,
							FB_RED
							"To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia."
							EOL);
					return;
				}
				ship->hatchopen = false;
				send_to_char("&GZamykasz klap�." NL, ch);
				act(PLAIN, "$n zamyka klap�.", ch, NULL, argument, TO_ROOM);
				sprintf(buf, FB_YELLOW "Klapa %s zamyka si�.", SHIPNAME(ship));
				echo_to_room(ship->location, buf);
				sound_to_room(ship->entrance, "!!SOUND(door)");
				sound_to_room(ship->location, "!!SOUND(door)");
				return;
			}
			else
			{
				send_to_char(FB_RED "Klapa jest ju� zamkni�ta." NL, ch);
				return;
			}
		}
	}
	ship = ship_in_room(ch->in_room, argument);
	if (!ship)
	{
		act(PLAIN, "Nie ma tu �adnego $T.", ch, NULL, argument, TO_CHAR);
		return;
	}
	if (ship->shipstate != SHIP_DOCKED && ship->shipstate != SHIP_DISABLED)
	{
		send_to_char(FB_RED "Ten statek ju� wystartowa�.", ch);
		return;
	}
	else
	{
		if (!known_biotech(ship, ch))
		{
			ch_printf(ch,
					FB_RED
					"To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia."
					EOL);
			return;
		}
		if (ship->hatchopen)
		{
			ship->hatchopen = false;
			act(PLAIN, "Zamykasz klap� statku $T.", ch, NULL, SHIPNAME(ship),
			TO_CHAR);
			act(PLAIN, "$n zamyka klape statku $T.", ch, NULL, SHIPNAME(ship),
					TO_ROOM);
			echo_to_room(ship->entrance,
			FB_YELLOW "Klapa zamyka si� od zewn�trz.");
			sound_to_room(ship->entrance, "!!SOUND(door)");
			sound_to_room(ship->location, "!!SOUND(door)");
			return;
		}
		else
		{
			send_to_char(FB_RED "Klapa jest ju� zamkni�ta." NL, ch);
			return;
		}
	}
}

DEF_DO_FUN( hyperspace )
{
	int chance;
	SHIP_DATA *ship;
	char buf[MAX_STRING_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�!" NL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Nie siedzisz w fotelu pilota." NL, ch);
		return;
	}

	if (!check_crew(ship, ch, "pilot"))
	{
		send_to_char(FB_RED "Nie zajmujesz stanowiska pilota na tym statku."
		NL, ch);
		return;
	}

	if (ship->hyperspeed == 0)
	{
		send_to_char(FB_RED "Ten statek nie ma hipernap�du!" NL, ch);
		return;
	}
	if (is_linked(NULL, ship))
	{
		send_to_char(FB_RED
		"Nie zrobisz tego dop�ki nie pozbedziesz si� statk�w dokuj�cych"
		EOL, ch);
		return;
	}
	if (ship->shipstate != SHIP_READY)
	{
		send_to_char(FB_RED "Poczekaj, a� statek wykona aktualny manewr." NL,
				ch);
		return;
	}
	if (!ship->currjump && !ship->vXpos && !ship->vYpos)
	{
		send_to_char(FB_RED "Musisz najpierw wyliczy� sw�j skok!" NL, ch);
		return;
	}

	if (ship->calctimer >= 1)
	{
		send_to_char(FB_RED "Poczekaj do zako�czenia wyliczania kursu!" EOL,
				ch);
		return;
	}

	if (ship->energy < ((200 + ship->hyperdistance + (ship->size / 10)) / 6))
	{
		send_to_char(FB_RED "Za ma�o paliwa!" NL, ch);
		return;
	}

	if (ship->currspeed <= 0)
	{
		send_to_char(FB_RED "Musisz najpierw troszk� przyspieszy�!" NL, ch);
		return;
	}

	for (auto* eShip : ship->starsystem->ships)
	{
		if (eShip == ship)
			continue;
		/*no i interdictor w poblizu Aldegard */
		if (srange((eShip->vx - ship->vx), (eShip->vy - ship->vy),
				(eShip->vz - ship->vz)) < 100000 * eShip->interdict)
		{
			if (eShip->cloack == 0)
			{
				ch_printf(ch,
				FB_RED
				"Pole wstrzymuj�ce z %s uniemo�liwia dokonanie skoku."
				NL,
						know_trans(ship, eShip) ?
								SHIPNAME(eShip) : eShip->transponder);
				return;
			}
			else
			{
				ch_printf(ch,
				FB_RED
				"Nieznane pole grawitacyjne uniemo�liwia dokonanie skoku."
				NL);
				return;
			}
		}

		/*poprawka na wielko�� przeszkadzacza -Aldegard */
		if (srange((eShip->vx - ship->vx), (eShip->vy - ship->vy),
				(eShip->vz - ship->vz)) < 12 * (eShip->size + ship->size) / 10)
		{
			if (eShip->cloack == 0)
			{
				ch_printf(ch,
				FB_RED "Jeste� zbyt blisko %s by wykona� skok." NL,
						know_trans(ship, eShip) ?
								SHIPNAME(eShip) : eShip->transponder);
				return;
			}
			else
			{
				ch_printf(ch,
				FB_RED
				"Dokonanie skoku niemo�liwe. Interferencja grawitomotoryczna."
				NL);
				return;
			}
		}
	}

	if (autofly(ship))
	{
		send_to_char(FB_GREEN "[Autopilot]: kontrola manualna.", ch);
		ship->autopilot = false;
	}

	chance = get_space_chance(ship, ch);
	if (number_percent() > chance)
	{
		send_to_char(FB_RED "Nie za bardzo wiesz, kt�r� d�wigni� poci�gn��."
		NL, ch);
		learn_from_failure_space(ship, ch);
		return;
	}
	if (ship->cloack == 0)
	{
		sprintf(buf, FB_YELLOW "%s znika w hiperprzestrzeni.", ship->sslook);
		echo_to_system(ship, buf, NULL);
	}
	ship_from_starsystem(ship, ship->starsystem);
	ship->shipstate = SHIP_HYPERSPACE;
	ship->interdict = 0;

	send_to_char("&GWciskasz d�wigni� hipernap�du." NL, ch);
	act(PLAIN, "$n wciska d�wigni� na panelu sterowania.", ch, NULL, argument,
			TO_ROOM);
	echo_to_ship(ship,
			FB_YELLOW
			"Statek trz�sie si� na moment i z olbrzymim hukiem wchodzi w hiperprzestrze�.");
	echo_to_cockpit(ship,
	FB_YELLOW
	"Gwiazdy zamieniaj� si� w s�upki �wiat�a w jednej chwili.");

	ship->energy -= ((100 + ship->hyperdistance + (ship->size / 10)) / 6);

	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);
	if (is_number(arg2))
	{
		ship->hx = atoi(arg2);
		ship->hy = atoi(arg3);
		ship->hz = atoi(argument);
	}
	ship->vx = ship->jx;
	ship->vy = ship->jy;
	ship->vz = ship->jz;

	learn_from_success_space(ship, ch);

}

DEF_DO_FUN( target )
{
	char arg[MAX_INPUT_LENGTH];
	int chance, licznik1, emp;
	SHIP_DATA *ship;
	SHIP_DATA *target;
	char buf[MAX_STRING_LENGTH];
	TURRET_DATA *turret;

	strcpy(arg, argument);

	switch (ch->substate)
	{
	default:
		if ((ship = ship_from_turret(ch->in_room)) == NULL)
		{
			send_to_char(FB_RED
			"Musisz siedzie� w fotelu strzelca lub na wie�yczce by to zrobi�!"
			NL, ch);
			return;
		}

		if (!is_ship(ship))
		{
			send_to_char(FB_RED "Nie jeste� na statku kosmicznym!" NL, ch);
			return;
		}

		if (arg[0] == '\0')
		{
			licznik1 = 0;
			pager_printf(ch, "Aktualne cele:" NL);
			if (ship->target0)
			{
				pager_printf(ch, "G��wne stanowisko ogniowe: %s" NL,
						know_trans(ship, ship->target0) ?
								SHIPNAME(ship->target0) :
								ship->target0->transponder);
				++licznik1;
			}
			for (auto* turret : ship->turrets)
			{
				if (turret->target)
				{
					pager_printf(ch, "Stanowisko artylerii:      %s" NL,
							know_trans(ship, turret->target) ?
									SHIPNAME(turret->target) :
									turret->target->transponder);
					++licznik1;
				}
			}
			if (licznik1 == 0)
				pager_printf(ch, "               Brak.");

			return;
		}

		if (!check_crew(ship, ch, "gunner"))
		{
			send_to_char(FB_RED
			"Musisz by� strzelcem pok�adowym aby to zrobi�."
			NL, ch);
			return;
		}
		if (isnt_in_realspace(ship))
		{
			send_to_char(FB_RED
			"W obecnej chwili i miejscu nie mo�esz tego zrobi�!"
			NL, ch);
			return;
		}

		if (!IS_NPC(ch) && ch->pcdata->learned[gsn_weaponsystems] <= 0)
		{
			send_to_char(
					FB_RED
					"Taka zabawa mo�e si� �le sko�czy�. Nie masz zielonego poj�cia o uzbrojeniu."
					NL, ch);
			return;
		}
		if (autofly(ship) && !IS_IMMORTAL(ch))
		{
			send_to_char(FB_RED "Musisz najpierw wy��czy� autopilota."
			NL, ch);
			return;
		}
		if (ship->lasers <= 0 && ship->ioncannons <= 0 && ship->missiles <= 0
				&& ship->torpedos <= 0 && ship->rockets <= 0)
		{
			send_to_char(FB_RED "Tw�j statek nie ma system�w bojowych!"
			NL, ch);
			return;
		}

		if (!str_cmp(arg, "none") || !str_cmp(arg, "nic")
				|| !str_cmp(arg, "off"))
		{
			send_to_char("&GAutocelowanie uniewa�nione." NL, ch);
			if ((turret = get_turret_here(ship, ch->in_room)) != NULL)
				turret->target = NULL;
			if (ch->in_room == ship->gunseat)
				ship->target0 = NULL;
			return;
		}

		target = get_ship_here(arg, ship->starsystem);
		if (target == NULL)
		{
			send_to_char(FB_RED "Nie ma tu takiego statku!" NL, ch);
			return;
		}

		if (target->cloack != 0)
		{
			send_to_char(FB_RED "Nie ma tu takiego statku!" NL, ch);
			return;
		}

		if (target == ship)
		{
			send_to_char(FB_RED "Nie mo�esz namierzy� swojego statku!" NL, ch);
			return;
		}

		if (!str_cmp(target->owner, ship->owner) && str_cmp(target->owner, "")
				&& str_cmp(target->owner, "Public"))
		{
			send_to_char(
					FB_RED
					"Ten statek ma identycznego w�a�ciciela! Mo�e spr�bujesz namierzy� statek wroga?"
					NL, ch);
			return;
		}
		emp = 1;
		if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
			emp = 2;
		if (srange((ship->vx - target->vx), (ship->vy - target->vy),
				(ship->vz - target->vz)) * emp
				> 2000 + ship->target_array * 500)
		{
			send_to_char(FB_RED
			"Ten statek jest za daleko by go namierzy�." NL, ch);
			return;
		}
		if ((turret = get_turret_here(ship, ch->in_room)) != NULL)
			chance =
					IS_NPC(ch) ?
							ch->top_level :
							(int) (ch->pcdata->learned[gsn_shipturrets] - 1);
		else
			chance =
					IS_NPC(ch) ?
							ch->top_level :
							(int) (ch->pcdata->learned[gsn_weaponsystems] - 1);

		if (number_percent() < chance
				&& ((turret = get_turret_here(ship, ch->in_room)) != NULL))
		{
			send_to_char("&GCelowanie...." NL, ch);
			act(PLAIN,
					"$n wprowadza koordynaty celu do komputera celowniczego.",
					ch, NULL, argument, TO_ROOM);
			add_timer(ch, TIMER_DO_FUN, 1, do_target, 1);
			STRDUP(ch->dest_buf, arg);
			return;
		}
		else if (number_percent() < chance)
		{
			send_to_char("&GNamierzanie...." NL, ch);
			act(PLAIN, "$n wprowadza dane celu do komputera celowniczego.", ch,
					NULL, argument, TO_ROOM);
			add_timer(ch, TIMER_DO_FUN, 1, do_target, 1);
			STRDUP(ch->dest_buf, arg);
			return;
		}
		if ((turret = get_turret_here(ship, ch->in_room)) != NULL)
		{
			send_to_char(FB_RED
			"Wie�yczka obraca si� w zupe�nie inn� strone ni� wymagana."
			NL, ch);
			learn_from_failure(ch, gsn_shipturrets);
			return;
		}
		send_to_char(FB_RED "Komputer celowniczy odmawia pos�usze�stwa."
		NL, ch);
		learn_from_failure(ch, gsn_weaponsystems);
		return;

	case 1:
		if (!*ch->dest_buf)
			return;
		snprintf(arg, sizeof(arg), "%s", ch->dest_buf);
		STRDUP(ch->dest_buf, "");
		break;

	case SUB_TIMER_DO_ABORT:
		STRDUP(ch->dest_buf, "");
		ch->substate = SUB_NONE;
		if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
			return;
		send_to_char(FB_RED
		"Nie mo�esz namierzy� celu. Twoje skanery s� uszkodzone."
		NL, ch);
		return;
	}

	ch->substate = SUB_NONE;

	if ((ship = ship_from_turret(ch->in_room)) == NULL)
	{
		return;
	}

	target = get_ship_here(arg, ship->starsystem);
	if (target == NULL || target == ship)
	{
		send_to_char(FB_RED
		"Statek opu�ci� uk�ad gwiezdny. Namierzanie przerwane."
		NL, ch);
		return;
	}

	if ((turret = get_turret_here(ship, ch->in_room)) != NULL)
		turret->target = target;
	if (ch->in_room == ship->gunseat)
		ship->target0 = target;

	if ((turret = get_turret_here(ship, ch->in_room)) != NULL)
	{
		send_to_char("&GWie�yczka ustawiona." NL, ch);
		sprintf(buf, "Wie�yczka z %s namierza ci�.",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
		echo_to_cockpit(target, buf);

		sound_to_room(ch->in_room, "!!SOUND(targetlock)");
		learn_from_success(ch, gsn_shipturrets);
	}
	else
	{
		send_to_char("&GCel namierzony." NL, ch);
		sprintf(buf, "%s namierza ci�.",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
		echo_to_cockpit(target, buf);

		sound_to_room(ch->in_room, "!!SOUND(targetlock)");
		learn_from_success(ch, gsn_weaponsystems);
	}
	if (autofly(target) && !target->target0)
	{
		sprintf(buf, FG_RED "%s namierzy� ci�.",
				know_trans(ship, target) ?
						SHIPNAME(target) : target->transponder);
		echo_to_cockpit(ship, buf);
		target->target0 = ship;
	}
}

DEF_DO_FUN( fire )	//pix0
{
	int chance;
	SHIP_DATA *ship;
	SHIP_DATA *target;
	char buf[MAX_STRING_LENGTH];
//    CHAR_DATA *tch;
	TURRET_DATA *turret;

	if ((ship = ship_from_turret(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED
		"Musisz siedzie� w fotelu strzelca lub w wie�yczce by to zrobi�!"
		NL, ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if (ship->energy < 5)
	{
		send_to_char(FB_RED "Nie starczy ci energii na strza�!" NL, ch);
		return;
	}

	if (autofly(ship))
	{
		send_to_char(FB_RED "Musisz najpierw wy��czy� autopilota." NL, ch);
		return;
	}
	if (ship->cloack == true)
	{
		ship->cloack = false;
		echo_to_ship(ship,
		FB_YELLOW
		"Manipulowanie uzbrojeniem statku zdradza twoj� pozycje.");
		sprintf(buf, FB_YELLOW "%s pojawia sie w %.0f %.0f %.0f.", ship->sslook,
				ship->vx, ship->vy, ship->vz);
		echo_to_system(ship, buf, NULL);
	}

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->perm_dex + ch->perm_lck
							+ (ch->pcdata->learned[gsn_spacecombat] / 5 - 1) +	// 3 - 1
							(ch->pcdata->learned[gsn_spacecombat2] / 5 - 1) +// 3 - 1
							(ch->pcdata->learned[gsn_spacecombat3] / 5 - 1) +// 3 - 1
							ch->mental_state);	// mental / 10

	if (ch->in_room == ship->gunseat && !str_prefix(argument, "lasers"))
	{
		if (!check_crew(ship, ch, "gunner"))
		{
			send_to_char(FB_RED
			"Nie zajmujesz stanowiska strzelca na tym statku."
			NL, ch);
			return;
		}
		if (ship->statet0 == LASER_DAMAGED)
		{
			send_to_char(FB_RED "G��wny laser statku jest uszkodzony." NL, ch);
			return;
		}
		if (ship->statet0 >= ship->lasers)
		{
			send_to_char(FB_RED "Lasery wci�� si� jeszcze �aduj�." NL, ch);
			return;
		}
		if (ship->target0 == NULL)
		{
			send_to_char(FB_RED "Wybierz najpierw cel." NL, ch);
			return;
		}
		target = ship->target0;
		if (target->cloack == true)
		{
			send_to_char(FB_RED "Do czego chcesz strzelac?" NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (ship->target0->starsystem != ship->starsystem)
		{
			send_to_char(FB_RED "Tw�j cel jako� znikn��." NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > 1000)
		{
			send_to_char(FB_RED "Ten statek jest poza zasi�giem laser�w." NL,
					ch);
			return;
		}
		if ((is_scout(ship) || is_fighter(ship) || is_midship(ship)
				|| is_freighter(ship) || is_capital(ship))
				&& !is_facing(ship, target))
		{
			send_to_char(FB_RED
			"G��wny laser mo�e strzela� tylko do przodu. Musisz obr�ci� statek!"
			NL, ch);
			return;
		}

		/*    	     if ( ( tch=target->vPilot) != NULL )
		 chance -= IS_NPC(tch) ? tch->top_level/2
		 : (int) ( (tch->perm_dex + tch->perm_lck + (tch->pcdata->learned[gsn_spacecombat]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat2]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat3]/3-1)
		 + tch->mental_state/10)/2 );
		 *///	sprintf(buf, FB_RED "%s " FG_YELLOW "strzela %s w twoim kierunku, ale chybia.",

		ship->statet0++;
		chance += target->size - ship->size;
		chance -= target->manuever - ship->manuever;
		//chance -= target->currspeed / 12;
		chance -= srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) / 70;// Kto wymyslil ta wartosc? -- Pixel
		chance = URANGE(3, chance, 90);
		if (ch->position != POS_SITTING)
			chance /= 3;
		act(PLAIN, "$n naciska przycisk na dr��ku.", ch, NULL, argument,
		TO_ROOM);
		if (number_percent() > chance)
		{
			sprintf(buf,
					FB_RED "%s " FG_YELLOW "strzela do ciebie z lasera, ale chybia.",
					know_trans(target, ship) ?
							SHIPNAME(ship) : ship->transponder);
			echo_to_cockpit(target, buf);
			sprintf(buf, "Statek strzela z lasera w %s, ale chybia.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
			learn_from_failure(ch, gsn_spacecombat);
			learn_from_failure(ch, gsn_spacecombat2);
			learn_from_failure(ch, gsn_spacecombat3);
			sprintf(buf, FB_RED "%s " FG_GREEN "chybia laserem " FB_RED "%s.",
					ship->sslook, target->sslook);
			echo_to_system(ship, buf, target);
			return;
		}
		sprintf(buf, FB_RED "%s " FG_GREEN "trafia laserem " FB_RED "%s.",
				ship->sslook, target->sslook);
		echo_to_system(ship, buf, target);
		sprintf(buf,
				FG_YELLOW "Lasery z " FB_RED "%s " FG_YELLOW "trafiaj� ci�!",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
		echo_to_cockpit(target, buf);
		sprintf(buf, "Tw�j laser trafia w %s!.",
				know_trans(ship, target) ?
						SHIPNAME(target) : target->transponder);
		echo_to_cockpit(ship, buf);
		learn_from_success(ch, gsn_spacecombat);
		learn_from_success(ch, gsn_spacecombat2);
		learn_from_success(ch, gsn_spacecombat3);
		if (!IS_IMMORTAL(ch))
			ch->mental_state += number_range(0, 1);
		echo_to_ship(target, "Ma�a eksplozja lekko wstrz�sa statkiem.");// Lasery
		damage_ship_ch(target, MIN_LASER_DAMAGE, MAX_LASER_DAMAGE, ch);

		if (autofly(target) && target->target0 != ship
				&& (srange((ship->vx - target->vx), (ship->vy - target->vy),
						(ship->vz - target->vz))
						<= 2000 + target->target_array * 500))
		{
			target->target0 = ship;
			sprintf(buf, "%s namierza ci�.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
		}

		return;
	}

	/*******************(Dzialka Jonowe)*********************/

	if (ch->in_room == ship->gunseat
			&& (!str_prefix(argument, "ion") || !str_prefix(argument, "jon")))
	{
		if (!check_crew(ship, ch, "gunner"))
		{
			send_to_char(FB_RED
			"Nie zajmujesz stanowiska strzelca na tym statku."
			NL, ch);
			return;
		}
		if (ship->ioncannons == 0)
		{
			send_to_char(FB_RED "Ten statek nie ma dzia�ek jonowych." NL, ch);
			return;
		}
		if (ship->statet0 == LASER_DAMAGED)
		{
			send_to_char(FB_RED "Dzia�ko jonowe jest uszkodzone." NL, ch);
			return;
		}
		if (ship->statet0 >= ship->ioncannons)
		{
			send_to_char(FB_RED "Dzia�ko jonowe wci�� si� jeszcze �aduje." NL,
					ch);
			return;
		}
		if (ship->target0 == NULL)
		{
			send_to_char(FB_RED "Wybierz najpierw cel." NL, ch);
			return;
		}
		target = ship->target0;
		if (target->cloack == true)
		{
			send_to_char(FB_RED "Do czego chcesz strzelac?" NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (ship->target0->starsystem != ship->starsystem)
		{
			send_to_char(FB_RED "Tw�j cel jako� znikn��." NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > 1500)
		{
			send_to_char(FB_RED "Ten statek jest poza zasi�giem." NL, ch);
			return;
		}
		if ((is_scout(ship) || is_fighter(ship) || is_midship(ship)
				|| is_freighter(ship) || is_capital(ship))
				&& !is_facing(ship, target))
		{
			send_to_char(
					FB_RED
					"Przednie dzia�ko jonowe mo�e strzela� tylko do przodu. Musisz obr�ci� statek!"
					NL, ch);
			return;
		}

		/*    	     if ( ( tch=target->vPilot) != NULL )
		 chance -= IS_NPC(tch) ? tch->top_level/2
		 : (int) ( (tch->perm_dex + tch->perm_lck + (tch->pcdata->learned[gsn_spacecombat]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat2]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat3]/3-1)
		 + tch->mental_state/10)/2 );
		 */

		ship->statet0 += ship->ioncannons * 6;
		ship->energy -= 200;
		chance += target->size - ship->size;
		chance -= target->manuever - ship->manuever;
		chance -= srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) / 70;
		chance = URANGE(3, chance, 90);
		act(PLAIN, "$n naciska przycisk strza�u na dr��ku.", ch, NULL, argument,
				TO_ROOM);
		if (number_percent() > chance)
		{
			sprintf(buf, "%s strzela do ciebie wi�zk� jonizuj�c�, ale chybia.",
					know_trans(target, ship) ?
							SHIPNAME(ship) : ship->transponder);
			echo_to_cockpit(target, buf);
			sprintf(buf, "Statek strzela wi�zk� jonizuj�c� w %s, ale chybia.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
			learn_from_failure(ch, gsn_spacecombat);
			learn_from_failure(ch, gsn_spacecombat2);
			learn_from_failure(ch, gsn_spacecombat3);
			sprintf(buf, "%s chybia wi�zk� jonizuj�c� %s.", ship->sslook,
					target->sslook);
			echo_to_system(ship, buf, target);
			return;
		}
		sprintf(buf, "%s trafia wi�zk� jonizuj�c� %s.", ship->sslook,
				target->sslook);
		echo_to_system(ship, buf, target);
		sprintf(buf, "Wi�zka jonizuj�ca z %s trafia ci�!",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
		echo_to_cockpit(target, buf);
		sprintf(buf, "Trafiasz w %s wi�zk� jonizuj�c�!.",
				know_trans(ship, target) ?
						SHIPNAME(target) : target->transponder);
		echo_to_cockpit(ship, buf);
		learn_from_success(ch, gsn_spacecombat);
		learn_from_success(ch, gsn_spacecombat2);
		learn_from_success(ch, gsn_spacecombat3);
		if (target->shield > 0)
			echo_to_ship(target, "Lekkie wibracje wstrz�saj� statkiem.");
		else
			echo_to_ship(target,
					"Wy�adowania elektryczne przebiegaj� po �cianach z przera�liwym trzaskiem.");
		damage_ship_energy(target, 5, 20, ch, ship);

		if (autofly(target) && target->target0 != ship
				&& (srange((ship->vx - target->vx), (ship->vy - target->vy),
						(ship->vz - target->vz))
						<= 2000 + target->target_array * 500))
		{
			target->target0 = ship;
			sprintf(buf, "%s namierza ci�.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
		}

		return;
	}

	/*******************(Pociski)*********************/

	if (ch->in_room == ship->gunseat && !str_prefix(argument, "missile"))
	{
		if (!check_crew(ship, ch, "gunner"))
		{
			send_to_char(FB_RED
			"Nie zajmujesz stanowiska strzelca na tym statku."
			NL, ch);
			return;
		}
		if (ship->missilestate == MISSILE_DAMAGED)
		{
			send_to_char(FB_RED "Wyrzutnie pocisk�w statku s� uszkodzone." NL,
					ch);
			return;
		}
		if (ship->missiles <= 0)
		{
			send_to_char(FB_RED "Nie masz ju� pocisk�w!" NL, ch);
			return;
		}
		if (ship->missilestate != MISSILE_READY)
		{
			send_to_char(FB_RED "Pociski wci�� si� �aduj�." NL, ch);
			return;
		}
		if (ship->target0 == NULL)
		{
			send_to_char(FB_RED "Musisz najpierw obra� cel." NL, ch);
			return;
		}
		target = ship->target0;
		if (target->cloack == true)
		{
			send_to_char(FB_RED "Do czego chcesz strzelac?" NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (ship->target0->starsystem != ship->starsystem)
		{
			send_to_char(FB_RED "Tw�j cel chyba ci zwia�." NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > 5000)
		{
			send_to_char(FB_RED
			"Ten statek jest chyba poza zasi�giem pocisk�w." NL, ch);
			return;
		}
		if ((is_scout(ship) || is_fighter(ship) || is_midship(ship)
				|| is_freighter(ship)) // dodac frachtowiec
		&& !is_facing(ship, target))
		{
			send_to_char(
					FB_RED
					"Pociskiem mo�esz strzela� tylko przed siebie. Musisz najpierw obr�ci� statek."
					NL, ch);
			return;
		}

		/*    	     if ( ( tch=target->vPilot) != NULL )
		 chance -= IS_NPC(tch) ? tch->top_level/2
		 : (int) ( (tch->perm_dex + tch->perm_lck + (tch->pcdata->learned[gsn_spacecombat]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat2]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat3]/3-1)
		 + tch->mental_state/10)/2 );
		 */

		chance += target->size - ship->size;
		chance -= target->manuever - ship->manuever;
		// chance -= target->currspeed / 20;
		chance -= (abs(target->vx - ship->vx) / 100);
		chance -= (abs(target->vy - ship->vy) / 100);
		chance -= (abs(target->vz - ship->vz) / 100);
		chance = URANGE(5, chance, 95);
		if (ch->position != POS_SITTING)
			chance /= 3;
		act(PLAIN, "$n naciska przycisk strza�u na dr��ku.", ch, NULL, argument,
				TO_ROOM);
		if (number_percent() > chance)
		{
			send_to_char(FB_RED
			"Autonaprowadzanie przesta�o dzia�a�. Tracisz cel!", ch);
			//ship->target0 = NULL;
			ship->missilestate = MISSILE_RELOAD_2;
			return;
		}
		new_missile(ship, target, ch, CONCUSSION_MISSILE);
		ship->missiles--;
		act(PLAIN, "$n naciska przycisk strza�u na dr��ku.", ch, NULL, argument,
				TO_ROOM);
		echo_to_cockpit(ship, "Pocisk wystrzelony.");
		sprintf(buf, "Ostrze�enie: zbli�a si� pocisk z %s.",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
		echo_to_cockpit(target, buf);
		sprintf(buf, "Widzisz odpalany pocisk z %s.", ship->sslook);
		echo_to_system(ship, buf, target);
		learn_from_success(ch, gsn_weaponsystems);
		if (is_capital(ship) || is_platform(ship))
			ship->missilestate = MISSILE_RELOAD; // Pixel: z jakiej racji?
		else
			ship->missilestate = MISSILE_FIRED;

		if (autofly(target) && target->target0 != ship
				&& (srange((ship->vx - target->vx), (ship->vy - target->vy),
						(ship->vz - target->vz))
						<= 2000 + target->target_array * 500))
		{
			target->target0 = ship;
			sprintf(buf, FG_RED "%s namierza ci�.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
		}

		return;
	}

	/*******************(Torpedy)*********************/

	if (ch->in_room == ship->gunseat && !str_prefix(argument, "torpedo"))
	{
		if (!check_crew(ship, ch, "gunner"))
		{
			send_to_char(FB_RED
			"Nie zajmujesz stanowiska strzelca na tym statku."
			NL, ch);
			return;
		}
		if (ship->missilestate == MISSILE_DAMAGED)
		{
			send_to_char(FB_RED "Wyrzutnie pocisk�w statku s� uszkodzone." NL,
					ch);
			return;
		}
		if (ship->torpedos <= 0)
		{
			send_to_char(FB_RED "Nie masz �adnych torped!" NL, ch);
			return;
		}
		if (ship->missilestate != MISSILE_READY)
		{
			send_to_char(FB_RED "Torpedy wci�� si� �aduj�." NL, ch);
			return;
		}
		if (ship->target0 == NULL)
		{
			send_to_char(FB_RED "Musisz najpierw obra� cel." NL, ch);
			return;
		}
		target = ship->target0;
		if (target->cloack == true)
		{
			send_to_char(FB_RED "Do czego chcesz strzelac?" NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (ship->target0->starsystem != ship->starsystem)
		{
			send_to_char(FB_RED "Tw�j cel chyba zwia�." NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > 2000)
		{
			send_to_char(FB_RED "Ten statek jest poza zasi�giem torped." NL,
					ch);
			return;
		}
		if ((is_scout(ship) || is_fighter(ship) || is_midship(ship)
				|| is_freighter(ship)) // dodac frachtowiec
		&& !is_facing(ship, target))
		{
			send_to_char(FB_RED
			"Torpedy mog� strzela� tylko do przodu. Musisz obr�ci� statek."
			NL, ch);
			return;
		}

		/*    	     if ( ( tch=target->vPilot) != NULL )
		 chance -= IS_NPC(tch) ? tch->top_level/2
		 : (int) ( (tch->perm_dex + tch->perm_lck + (tch->pcdata->learned[gsn_spacecombat]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat2]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat3]/3-1)
		 + tch->mental_state/10)/2 );
		 */
		chance += target->size - ship->size;
		chance -= target->manuever - ship->manuever;
		// chance -= target->currspeed / 20;
		chance -= (abs(target->vx - ship->vx) / 100);
		chance -= (abs(target->vy - ship->vy) / 100);
		chance -= (abs(target->vz - ship->vz) / 100);
		chance -= 15;
		chance = URANGE(2, chance, 98);
		if (ch->position != POS_SITTING)
			chance /= 3;
		act(PLAIN, "$n naciska przycisk strza�u na dr��ku.", ch, NULL, argument,
				TO_ROOM);
		if (number_percent() > chance)
		{
			send_to_char(FB_RED "Autonamierzanie zosta�o zerwane!", ch);
			ship->missilestate = MISSILE_RELOAD_2;
			//ship->target0 = NULL;
			return;
		}
		new_missile(ship, target, ch, PROTON_TORPEDO);
		ship->torpedos--;
		act(PLAIN, "$n naciska przycisk strza�u na dr��ku.", ch, NULL, argument,
				TO_ROOM);
		echo_to_cockpit(ship, "Pocisk wystrzelony.");
		sprintf(buf, "Ostrze�enie: Nadlatuje torpeda z %s.",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
		echo_to_cockpit(target, buf);
		sprintf(buf, "%s wypuszca torped�.", ship->sslook);
		echo_to_system(ship, buf, target);
		learn_from_success(ch, gsn_weaponsystems);
		if (is_capital(ship) || is_platform(ship))
			ship->missilestate = MISSILE_RELOAD;
		else
			ship->missilestate = MISSILE_FIRED;

		if (autofly(target) && target->target0 != ship
				&& (srange((ship->vx - target->vx), (ship->vy - target->vy),
						(ship->vz - target->vz))
						<= 2000 + target->target_array * 500))
		{
			target->target0 = ship;
			sprintf(buf, FG_RED "%s namierza ci�.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
		}

		return;
	}

	/*******************(Rakiety)*********************/

	if (ch->in_room == ship->gunseat && !str_prefix(argument, "rocket"))
	{
		if (!check_crew(ship, ch, "gunner"))
		{
			send_to_char(FB_RED
			"Nie zajmujesz stanowiska strzelca na tym statku."
			NL, ch);
			return;
		}
		if (ship->missilestate == MISSILE_DAMAGED)
		{
			send_to_char(FB_RED "Wyrzutnia pocisk�w statku jest uszkodzona."
			NL, ch);
			return;
		}
		if (ship->rockets <= 0)
		{
			send_to_char(FB_RED "Nie masz �adnych rakiet!" NL, ch);
			return;
		}
		if (ship->missilestate != MISSILE_READY)
		{
			send_to_char(FB_RED "Rakiety wci�� si� prze�adowuj�." NL, ch);
			return;
		}
		if (ship->target0 == NULL)
		{
			send_to_char(FB_RED "Musisz najpierw obra� cel." NL, ch);
			return;
		}
		target = ship->target0;
		if (target->cloack == true)
		{
			send_to_char(FB_RED "Do czego chcesz strzelac?" NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (ship->target0->starsystem != ship->starsystem)
		{
			send_to_char(FB_RED "Tw�j cel chyba zwia�." NL, ch);
			ship->target0 = NULL;
			return;
		}
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > 800)
		{
			send_to_char(FB_RED "Ten statek jest poza zasi�giem rakiet." NL,
					ch);
			return;
		}
		if ((is_scout(ship) || is_fighter(ship) || is_midship(ship)
				|| is_freighter(ship)) && !is_facing(ship, target))
		{
			send_to_char(FB_RED
			"Mo�esz strzela� rakietami tylko do przodu. Musisz obr�ci� statek!"
			NL, ch);
			return;
		}

		/*    	     if ( ( tch=target->vPilot) != NULL )
		 chance -= IS_NPC(tch) ? tch->top_level/2
		 : (int) ( (tch->perm_dex + tch->perm_lck + (tch->pcdata->learned[gsn_spacecombat]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat2]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat3]/3-1)
		 + tch->mental_state/10)/2 );
		 */
		chance += target->size - ship->size;
		chance -= target->manuever - ship->manuever;
		// chance -= target->currspeed / 20;
		chance -= (abs(target->vx - ship->vx) / 100);
		chance -= (abs(target->vy - ship->vy) / 100);
		chance -= (abs(target->vz - ship->vz) / 100);
		chance -= 30;
		chance = URANGE(1, chance, 99);
		if (ch->position != POS_SITTING)
			chance /= 3;
		act(PLAIN, "$n naciska przycisk strza�u na dr��ku.", ch, NULL, argument,
				TO_ROOM);
		if (number_percent() > chance)
		{
			send_to_char(FB_RED "Autonamierzanie zerwane!", ch);
			ship->missilestate = MISSILE_RELOAD_2;
			//ship->target0 = NULL;
			return;
		}
		new_missile(ship, target, ch, HEAVY_ROCKET);
		ship->rockets--;
		act(PLAIN, "$n naciska przycisk strza�u na dr��ku.", ch, NULL, argument,
				TO_ROOM);
		echo_to_cockpit(ship, "Rakieta wystrzelona.");
		sprintf(buf, "Ostrze�enie: nadlatuje rakieta z %s.",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder);
		echo_to_cockpit(target, buf);
		sprintf(buf, "%s wypuszcza ci�k� rakiet�.", ship->sslook);
		echo_to_system(ship, buf, target);
		learn_from_success(ch, gsn_weaponsystems);
		if (is_capital(ship) || is_platform(ship))
			ship->missilestate = MISSILE_RELOAD;
		else
			ship->missilestate = MISSILE_FIRED;

		if (autofly(target) && target->target0 != ship
				&& (srange((ship->vx - target->vx), (ship->vy - target->vy),
						(ship->vz - target->vz))
						<= 2000 + target->target_array * 500))
		{
			target->target0 = ship;
			sprintf(buf, FG_RED "%s namierza ci�.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
		}

		return;
	}

	/*******************(Wiezyczki)*********************/

	if (((turret = get_turret_here(ship, ch->in_room)) != NULL)
			&& !str_prefix(argument, "lasers"))
	{
		if (!check_crew(ship, ch, "gunner"))
		{
			send_to_char(FB_RED "Nie nale�ysz do obs�ugi tego dzia�a." NL, ch);
			return;
		}
		if (turret->status == TURRET_DAMAGED)
		{
			send_to_char(FB_RED "Bateria jest uszkodzona." NL, ch);
			return;
		}

		if (turret->type < 5 && turret->statet0 >= turret->type)
		{
			send_to_char(FB_RED "Wie�yczka prze�adowuje si� teraz." NL, ch);
			return;
		}

		if ((turret->type == 5 || turret->type == 6) && turret->statet0 > 0)
		{
			send_to_char(FB_RED "Bateria prze�adowuje si� teraz." NL, ch);
			return;
		}

		if (turret->type == 7 && turret->statet0 > 0)
		{
			send_to_char(FB_RED "Superlaser prze�adowuje si� teraz." NL, ch);
			return;
		}

		if (turret->target == NULL)
		{
			send_to_char(FB_RED "Musisz najpierw obra� cel." NL, ch);
			return;
		}
		target = turret->target;
		if (target->cloack == true)
		{
			send_to_char(FB_RED "Do czego chcesz strzelac?" NL, ch);
			turret->target = NULL;
			return;
		}
		if (target->starsystem != ship->starsystem)
		{
			send_to_char(FB_RED "Tw�j cel chyba zwia�." NL, ch);
			turret->target = NULL;
			return;
		}
		if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > 1000 && turret->type < 6)
		{
			send_to_char(FB_RED "Ten statek jest poza zasi�giem laser�w." NL,
					ch);
			return;
		}
		else if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > 2000
				&& (turret->type == 5 || turret->type == 6))
		{
			send_to_char(
					FB_RED "Ten statek jest poza zasi�giem turbolaser�w." NL,
					ch);
			return;
		}
		else if (srange((target->vx - ship->vx), (target->vy - ship->vy),
				(target->vz - ship->vz)) > 5000 && turret->type == 7)
		{
			send_to_char(
					FB_RED "Ten statek jest poza zasi�giem twojego superlasera." NL,
					ch);
			return;
		}

		/*    	     if ( ( tch=target->vPilot) != NULL )
		 chance -= IS_NPC(tch) ? tch->top_level/2
		 : (int) ( (tch->perm_dex + tch->perm_lck + (tch->pcdata->learned[gsn_spacecombat]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat2]/3-1)
		 + (tch->pcdata->learned[gsn_spacecombat3]/3-1)
		 + tch->mental_state/10)/2 );
		 */

		switch (turret->type)
		{
		case LASER_TURRET:
			chance += target->size - 15;
			chance -= target->manuever - 90;	// Jak Medium Starfighter
			chance = URANGE(10, chance, 90);
			break;

		case DOUBLE_TURRET:
			chance += target->size - 20;
			chance -= target->manuever - 80;	// Jak Heavy Starfighter
			chance = URANGE(10, chance, 90);
			break;

		case TRIPLE_TURRET:
			chance += target->size - 25;
			chance -= target->manuever - 70;	// Jak Light Freighter
			chance = URANGE(10, chance, 90);
			break;

		case QUAD_TURRET:
			chance += target->size - 30;
			chance -= target->manuever - 60;	// Jak Medium Freighter
			chance = URANGE(10, chance, 90);
			break;

		case TURBOLASER_TURRET:
			turret->statet0 += 8;
			chance += target->size - 100;
			chance -= target->manuever - 100;
			chance = URANGE(5, chance, 85);
			break;

		case HEAVY_TURBOLASER_TURRET:
			turret->statet0 += 10;
			chance += target->size - 110;
			chance -= target->manuever - 120;
			chance = URANGE(5, chance, 85);
			break;

		case SUPERLASER_TURRET:
			turret->statet0 += 100;	// po testach proponuje 100
			chance += target->size - 100;
			chance -= target->manuever - 100;
			chance = URANGE(5, chance, 90);
			break;
		}

		turret->statet0++;

		/* Trzeba bedzie to zrobic dla indywidualnych dzial -- Pixel */
		chance -= (abs(target->vx - ship->vx) / 70);
		chance -= (abs(target->vy - ship->vy) / 70);
		chance -= (abs(target->vz - ship->vz) / 70);

		act(PLAIN, "$n naciska przycisk strza�u na dr��ku.", ch, NULL, argument,
				TO_ROOM);
		if (number_percent() > chance)
		{
			sprintf(buf, "%s z %s chybia.",
					turret->type < 5 ? "Wie�yczka laserowa" :
					turret->type == 7 ? "Superlaser" : "Bateria Turbolasera",
					know_trans(target, ship) ?
							SHIPNAME(ship) : ship->transponder);
			echo_to_cockpit(target, buf);
			sprintf(buf, "%s unika strza�u.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
			sprintf(buf, "%s strzela w %s z %s, ale chybia.", ship->sslook,
					target->sslook,
					turret->type < 5 ? "wie�yczki laserowej" :
					turret->type == 7 ? "superlasera" : "turbolasera");
			echo_to_system(ship, buf, target);
			learn_from_failure(ch, gsn_spacecombat);
			learn_from_failure(ch, gsn_spacecombat2);
			learn_from_failure(ch, gsn_spacecombat3);
			return;
		}
		sprintf(buf, "%s z %s trafia %s.",
				turret->type < 5 ? "Wie�yczka laserowa" :
				turret->type == 7 ? "Superlaser" : "Bateria Turbolasera",
				ship->sslook, target->sslook);
		echo_to_system(ship, buf, target);
		sprintf(buf, "%s trafia ci� %s!",
				know_trans(target, ship) ? SHIPNAME(ship) : ship->transponder,
				turret->type < 5 ? "laserem" :
				turret->type == 7 ? "superlaserem" : "turbolaserem");
		echo_to_cockpit(target, buf);
		sprintf(buf, "%s trafia %s!.",
				turret->type < 5 ? "Wie�yczka laserowa" :
				turret->type == 7 ? "Superlaser" : "Bateria turbolasera",
				know_trans(ship, target) ?
						SHIPNAME(target) : target->transponder);
		echo_to_cockpit(ship, buf);
		learn_from_success(ch, gsn_spacecombat);
		learn_from_success(ch, gsn_spacecombat2);
		learn_from_success(ch, gsn_spacecombat3);

		if (turret->type < 5)
		{
			echo_to_ship(target, "Ma�a eksplozja wstrz�sa statkiem.");
			damage_ship_ch(target, MIN_LASER_DAMAGE, MAX_LASER_DAMAGE, ch);
		}
		else if (turret->type == 7)
		{
			echo_to_ship(target,
					FB_GREEN "Pot�na eksplozja niszczy statek!!!");
			//damage_ship_ch(target, 5000, 10000, ch);
			purge_ship(target, ch);
		}
		else if (turret->type == 5)
		{
			echo_to_ship(target, "Eksplozja wstrz�sa statkiem!");
			damage_ship_ch(target, MIN_TURBO_DAMAGE, MAX_TURBO_DAMAGE, ch);
		}
		else if (turret->type == 6)
		{
			echo_to_ship(target, "Eksplozja wstrz�sa statkiem!");
			damage_ship_ch(target, MIN_HTURBO_DAMAGE, MAX_HTURBO_DAMAGE, ch);
		}
		else

		if (autofly(target) && target->target0 != ship
				&& (srange((ship->vx - target->vx), (ship->vy - target->vy),
						(ship->vz - target->vz))
						<= 2000 + target->target_array * 500))
		{
			target->target0 = ship;
			sprintf(buf, "%s namierza ci�.",
					know_trans(ship, target) ?
							SHIPNAME(target) : target->transponder);
			echo_to_cockpit(ship, buf);
		}

		return;
	}

	send_to_char(FB_RED "nie mo�esz strzela� z czego� takiego!" NL, ch);

}

int starsystem_cmp(SPACE_DATA **st1, SPACE_DATA **st2)
{
	return strcmp((*st1)->name, (*st2)->name);
}

/* Trog: pamietajcie, zeby pozniej koniecznie wywylywac DISPOSE() na tablicy
 * przekazanej w parametrze (po wywolaniu tej funkcji i skorzystaniu
 * z tablicy)!!! */
int get_sorted_systems(SPACE_DATA ***sorted_systems)
{
	int count = starsystem_list.size();

	if (count)
	{
		CREATE(*sorted_systems, SPACE_DATA*, count);
		count = 0;
		for (auto* starsystem : starsystem_list)
			(*sorted_systems)[count++] = starsystem;
		qsort(*sorted_systems, count, sizeof(SPACE_DATA*),
				(int (*)(const void*, const void*)) starsystem_cmp);
	}

	return count;
}

DEF_DO_FUN( calculate )
{
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char arg3[MAX_INPUT_LENGTH];
	int chance;
	SHIP_DATA *ship;

	argument = one_argument(argument, arg1);
	argument = one_argument(argument, arg2);
	argument = one_argument(argument, arg3);

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz byc w kokpicie statku by to zrobi�." NL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if ((ship = ship_from_navseat(ch->in_room)) == NULL)
	{
		send_to_char(
				FB_RED
				"Musisz sta� przy komputerze nawigacyjnym, by oblicza� wsp�rz�dne skoku."
				NL, ch);
		return;
	}

	if (!check_crew(ship, ch, "navigator"))
	{
		send_to_char(FB_RED "Nie jeste� nawigatorem na tym statku." NL, ch);
		return;
	}

	if (ship->hyperspeed == 0)
	{
		send_to_char(FB_RED "Ten statek nie jest wyposa�ony w hipernap�d!" NL,
				ch);
		return;
	}

	if (is_number(arg1))
	{
		calculate_number(ch, arg1, arg2, ship);
		return;
	}
	if (argument[0] == '\0')
	{
		SPACE_DATA **sorted_systems = NULL;
		int i, j, count = get_sorted_systems(&sorted_systems);

		pager_printf(ch, FB_WHITE
		"Sk�adnia: calculate <nazwa> <x wej�cia> <y wej�cia> <z wej�cia>"
		EOL NL);
		pager_printf(ch, FG_GREEN
		"Nazwa                   odl.      Nazwa                   odl."
		EOL);

		if (count)
		{
			for (i = 0, j = 0; i < count; i++)
			{
				if ((sorted_systems[i]->hidden == 0
						|| has_map(sorted_systems[i], ch))
						&& !IS_SET(sorted_systems[i]->flags, STARS_FLAG_TEMP))
				{
					pager_printf(ch, "%-20s %8.0f", sorted_systems[i]->name,
							(srange(
									(sorted_systems[i]->xpos
											- ship->starsystem->xpos),
									(sorted_systems[i]->ypos
											- ship->starsystem->ypos), 0)));
					if (!(++j % 2))
						pager_printf(ch, EOL);
					else
						pager_printf(ch, "     ");
				}
			}
			pager_printf(ch, EOL);
		}
		else
		{
			send_to_char("Nie znaleziono system�w." NL, ch);
		}

		DISPOSE(sorted_systems);

		return;
	}
	if (!is_number(argument))
	{
		send_to_char(FB_RED "B��dna trzecia wsp�rz�dna 'wyj�cia'!" NL, ch);
		return;
	}
	chance =
			IS_NPC(ch) ?
					ch->top_level : (int) (ch->pcdata->learned[gsn_navigation]);
	if (number_percent() > chance)
	{
		send_to_char(FB_RED "Nie udaje ci si� jako� nic wyliczy�." NL, ch);
		learn_from_failure(ch, gsn_navigation);
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

	ship->currjump = starsystem_from_name(arg1);
	ship->jx = atoi(arg2);
	ship->jy = atoi(arg3);
	ship->jz = atoi(argument);
	if (ship->currjump == NULL)
	{
		send_to_char(FB_RED
		"Nie mo�esz znale�� wsp�rz�dnych systemu na wykresach."
		NL, ch);
		return;
	}
	if (ship->starsystem == ship->currjump)
	{
		ship->jx = atoi(arg2) + ship->vx;
		ship->jy = atoi(arg3) + ship->vy;
		ship->jz = atoi(argument) + ship->vz;
	}
	if (ship->currjump->hidden != 0 && !has_map(ship->currjump, ch)
			&& ship->currjump != ship->starsystem)
	{
		send_to_char(FB_RED
		"Niestety komputer nawigacyjny nie znalazl tego systemu w pamieci."
		NL, ch);
		send_to_char(FB_RED "Poszukaj map gwiezdnych w Bibliotekach." NL, ch);
		ship->currjump = NULL;
		ship->jx = 0;
		ship->jy = 0;
		ship->jz = 0;
		return;
	}
	else
	{
		SPACE_DATA *starsystem;

		starsystem = ship->currjump;

		for (auto* star : starsystem->stars)
		{
			if (srange((ship->jx - star->xpos), (ship->jy - star->ypos),
					(ship->jz - star->zpos))
					< star->radius + star->gravity * 2 + 50)
			{
				echo_to_cockpit(ship,
						FB_RED
						"OSTRZE�ENIE.. Koordynaty skoku przecinaj� si� z obiektem w systemie docelowym.");
				echo_to_cockpit(ship,
				FB_RED
				"OSTRZE�ENIE.. Wyliczanie koordynat skoku przerwane.");
				ship->currjump = NULL;
				return;
			}
			if (srange((ship->jx - star->xpos), (ship->jy - star->ypos),
					(ship->jz - star->zpos))
					< star->radius + star->gravity * 4 + 50)
			{
				echo_to_cockpit(ship,
						FB_RED
						"OSTRZE�ENIE.. Koordynaty skoku w pobli�u obiektu w systemie docelowym.");
				echo_to_cockpit(ship,
				FB_RED "OSTRZE�ENIE.. Zalecana ostro�no��.");
			}
		}
		for (auto* planet : starsystem->planets)
		{
			if (srange((ship->jx - planet->xpos), (ship->jy - planet->ypos),
					(ship->jz - planet->zpos))
					< planet->radius + planet->gravity + 50)
			{
				echo_to_cockpit(ship,
						FB_RED
						"OSTRZE�ENIE.. Koordynaty skoku przecinaj� si� z obiektem w systemie docelowym.");
				echo_to_cockpit(ship,
				FB_RED
				"OSTRZE�ENIE.. Hiperskok NIE ustalony.");
				ship->currjump = NULL;
				return;
			}
			if (srange((ship->jx - planet->xpos), (ship->jy - planet->ypos),
					(ship->jz - planet->zpos))
					< planet->radius + planet->gravity * 2 + 50)
			{
				echo_to_cockpit(ship,
						FB_RED
						"OSTRZE�ENIE.. Koordynaty skoku w pobli�u obiektu w systemie docelowym.");
				echo_to_cockpit(ship,
				FB_RED "OSTRZE�ENIE.. Zalecana ostro�no��.");
			}
		}
		for (auto* moon : starsystem->moons)
		{
			if (srange((ship->jx - moon->xpos), (ship->jy - moon->ypos),
					(ship->jz - moon->zpos)) < moon->gravity + 50)
			{
				echo_to_cockpit(ship,
						FB_RED
						"OSTRZE�ENIE.. Koordynaty skoku przecinaj� si� z obiektem w systemie docelowym.");
				echo_to_cockpit(ship,
				FB_RED
				"OSTRZE�ENIE.. Hiperskok NIE ustalony.");
				ship->currjump = NULL;
				return;
			}
			if (srange((ship->jx - moon->xpos), (ship->jy - moon->ypos),
					(ship->jz - moon->zpos)) < moon->gravity * 2 + 50)
			{
				echo_to_cockpit(ship,
						FB_RED
						"OSTRZE�ENIE.. Koordynaty skoku w pobli�u obiektu w systemie docelowym.");
				echo_to_cockpit(ship,
				FB_RED "OSTRZE�ENIE.. Zalecana ostro�no��.");
			}
		}
		ship->jx += number_range(-10, 10);
		ship->jy += number_range(-10, 10);
		ship->jz += number_range(-10, 10);
	}

	ship->hyperdistance = srange(
			(ship->starsystem->xpos - ship->currjump->xpos),
			(ship->starsystem->ypos - ship->currjump->ypos), 0);

	ship->calctimer = number_range(5, 10);
	if (ship->hyperdistance < ship->hyperspeed * 5)
	{
		ship->hyperdistance = ship->hyperspeed * 5;
		ship->calctimer = number_range(2, 6);
	}
	sound_to_room(ch->in_room, "!!SOUND(computer)");

	send_to_char(FB_GREEN "Wsp�rz�dne hiperprzestrzenne wprowadzone." EOL, ch);
	send_to_char(FG_MAGENTA "Rozpoczynam przeliczanie kursu." EOL, ch);
	act(PLAIN,
			"$n wykonuje jakie� skomplikowane obliczenia na komputerze statku.",
			ch, NULL, NULL, TO_ROOM);

	learn_from_success(ch, gsn_navigation);

	WAIT_STATE(ch, 2 * PULSE_VIOLENCE);
}

DEF_DO_FUN( recharge )
{
	int recharge;
	int chance;
	SHIP_DATA *ship;
	int energy;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz by� w kokpicie statku by to zrobi�." NL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if ((ship = ship_from_coseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz sta� na stanowisku drugiego pilota." NL,
				ch);
		return;
	}

	if (autofly(ship))
	{
		send_to_char(FB_RED "Autopilot sam podtrzymuje tarcze" NL, ch);
		return;
	}

	energy = URANGE(ship->size / 90, ship->energy,
			ship->size / 10 + ship->size / 40);
	if (ship->energy < energy)
	{
		send_to_char(FB_RED "Masz za ma�o paliwa!" NL, ch);
		return;
	}

	if (ship->energy < 0)
	{
		send_to_char(FB_RED "Eksplozja w reaktorze!" NL, ch);
		return;
	}

	if (ship->energy < -100)
	{
		send_to_char(FB_RED "Reaktor nie wytrzymuje przeci��enia!" NL, ch);
		destroy_ship(ship, ch);
		return;
	}

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_shipsystems]);
	if (number_percent() > chance)
	{
		send_to_char(FB_RED "Nie uda�o ci si� poprawnie obs�u�y� urz�dze�."
		NL, ch);
		learn_from_failure(ch, gsn_shipsystems);
		return;
	}

	send_to_char("&GPrze�adowanie tarczy.." NL, ch);
	act(PLAIN, "$n poci�ga za d�wigni� na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);

	learn_from_success(ch, gsn_shipsystems);

	recharge = UMIN(ship->maxshield - ship->shield, ship->energy * 5 + 100);
	recharge = URANGE(1, recharge, 25 + ship->size / 10);
	ship->shield += recharge;
	ship->energy -= energy;
	if (ship->maxshield < ship->shield)
		ship->shield = ship->maxshield;
	if (ship->energy < 0)
		ship->energy = 0;
}

DEF_DO_FUN( repairship )
{
	char arg[MAX_INPUT_LENGTH];
	int chance, change, a = 0;
	SHIP_DATA *ship;
	TURRET_DATA *turret;

	strcpy(arg, argument);

	switch (ch->substate)
	{
	default:
		if ((ship = ship_from_room(ch->in_room)) == NULL)
		{
			send_to_char(FB_RED
			"Statek mozna naprawiac tylko od wewn�trz!" NL, ch);
			return;
		}

		if (!str_cmp(arg, "show"))
		{
			pager_printf(ch, FG_CYAN "Kad�ub             %d/%d" NL, ship->hull,
					ship->maxhull);
			pager_printf(ch, FG_CYAN "Nap�d              %s" NL,
					ship->shipstate == SHIP_DISABLED ?
							FG_YELLOW "Uszkodzony" : FB_CYAN
							"Sprawny");
			pager_printf(ch, FG_CYAN "Systemy            %s" NL,
					ship->shipstate1 > 0 ?
							FG_YELLOW "Zjonizowane" : FB_CYAN "Sprawne");
			pager_printf(ch, FG_CYAN "Lasery             %s" NL,
					ship->statet0 ==
					LASER_DAMAGED ? FG_YELLOW "Uszkodzone" : FB_CYAN
					"Sprawne");
			pager_printf(ch, FG_CYAN "Wyrzutnia          %s" NL,
					ship->missilestate != MISSILE_READY ?
							FG_YELLOW "Uszkodzona" : FB_CYAN
							"Sprawna");
			for (auto* turret : ship->turrets)
			{
				pager_printf(ch, FG_CYAN "Wie�yczka nr %-5d %s" NL, a,
						turret->status == 0 ?
								FB_CYAN "Sprawna" :
								(turret->status == -1 ? FB_RED "Zniszczona" :
								FG_YELLOW "Uszkodzona"));
				++a;
			}
			return;
		}
		if (argument[0] == '\0')
		{
			send_to_char(FB_RED "Musisz sprecyzowa� co chcesz naprawi�:"
			NL, ch);
			send_to_char(FG_RED "Spr�buj: Kad�ub (hull)" NL
			"         Nap�d (drive)" NL
			"         Systemy(systems)" NL, ch);
			send_to_char(FG_RED "         Wyrzutnia (launcher)" NL
			"         Laser" NL "         Dzia�o (Turret)"
			NL, ch);
			send_to_char(FB_RED "Aby zobaczy� stan podsystem�w:" NL FG_RED
			"wpisz:   Repairship Show" NL, ch);
			return;
		}
		else if (!str_cmp(arg, "hull") || !str_cmp(arg, "kad�ub"))
		{
			ch_printf(ch, FB_YELLOW "Kad�ub: %d z %d" NL, ship->hull,
					ship->maxhull);
		}
		else if (!str_cmp(arg, "drive") || !str_cmp(arg, "nap�d"))
		{
			if (ch->in_room != ship->engineroom)
			{
				ch_printf(ch,
				FB_RED
				"Musisz by� w pobli�u silnika �eby go naprawia�."
				NL);
				return;
			}
		}
		else if (!str_cmp(arg, "launcher") || !str_cmp(arg, "wyrzutnia")
				|| !str_cmp(arg, "laser"))
		{
			if (ch->in_room != ship->gunseat)
			{
				ch_printf(ch,
				FB_RED
				"Musisz by� w pobli�u uzbrojenia kt�re chcesz naprawia�."
				NL);
				return;
			}
		}
		else if (!str_cmp(arg, "systems") || !str_cmp(arg, "systemy"))
		{
			if (ch->in_room != ship->navseat)
			{
				ch_printf(ch,
				FB_RED
				"Konc�wka dejonizatora jest u nawigator�w." NL);
				return;
			}
		}
		else if (!str_cmp(arg, "dzia�o") || !str_cmp(arg, "turret"))
		{
			if ((turret = get_turret_here(ship, ch->in_room)) != NULL)
			{
				if (turret->status == TURRET_DESTROYED)
				{
					ch_printf(ch,
					FB_RED
					"To dzia�o jest doszcz�tnie zniszczone trzeba je wymieni�."
					NL);
					return;
				}
				else if (turret->status == TURRET_READY)
				{
					ch_printf(ch,
					FB_GREEN "Sprawno�� baterii 100 procent."
					NL);
					return;
				}
				/*else if(turret->status <= 10)
				 {
				 ch_printf(ch,
				 FB_RED
				 "Co chcesz naprawia�? To dzia�o si� prze�adowuje."
				 NL);
				 return;
				 }*/
			}
			else
			{
				ch_printf(ch, FB_RED "Nie jeste� w pobli�u dzia�a." NL);
				return;
			}
		}
		else
		{
			do_repairship(ch, (char*) "");
			return;
		}

		chance =
				IS_NPC(ch) ?
						ch->top_level :
						(int) (ch->pcdata->learned[gsn_shipmaintenance]);
		if (number_percent() < chance)
		{
			send_to_char("&GOstro bierzesz si� za naprawianie..." NL, ch);
			act(PLAIN, "$n naprawia jakie� elementy statku.", ch, NULL, NULL,
					TO_ROOM);
			if (!str_cmp(arg, "hull") || !str_cmp(arg, "kad�ub"))
				add_timer(ch, TIMER_DO_FUN, 15, do_repairship, 1);
			else
				add_timer(ch, TIMER_DO_FUN, 5, do_repairship, 1);
			STRDUP(ch->dest_buf, arg);
			return;
		}
		send_to_char(FB_RED
		"Nie uda�o ci si� zlokalizowa� �r�d�a problemu." NL, ch);
		learn_from_failure(ch, gsn_shipmaintenance);
		return;

	case 1:
		if (!*ch->dest_buf)
			return;
		snprintf(arg, sizeof(arg), "%s", ch->dest_buf);
		STRDUP(ch->dest_buf, "");
		break;

	case SUB_TIMER_DO_ABORT:
		STRDUP(ch->dest_buf, "");
		ch->substate = SUB_NONE;
		if ((ship = ship_from_room(ch->in_room)) == NULL)
			return;
		ch_printf(ch, FB_RED
		"Rozkojarzy�%s� si�. Nici z naprawy." EOL, SEX_SUFFIX_EAE(ch));
		return;
	}

	ch->substate = SUB_NONE;

	if ((ship = ship_from_room(ch->in_room)) == NULL)
	{
		return;
	}

	if (!str_cmp(arg, "kad�ub") || (!str_cmp(arg, "hull")))
	{
		change = URANGE(0,
				number_range((int )
				(ch->pcdata->learned[gsn_shipmaintenance] / 2),
						(int ) (ch->pcdata->learned[gsn_shipmaintenance])),
				(ship->maxhull - ship->hull));
		ship->hull += change;
		ch_printf(ch,
				"&GNaprawa zako�czona.. Si�a kad�ubu podniesiona o %d punkt%s."
				NL, change, NUMBER_SUFF(change, "", "y", "�w"));
	}

	if (!str_cmp(arg, "drive") || !str_cmp(arg, "nap�d"))
	{
		if (ship->location == ship->lastdoc)
			ship->shipstate = SHIP_DOCKED;
		else
			ship->shipstate = SHIP_READY;
		send_to_char("&GNap�d statku naprawiony." NL, ch);
	}

	if (!str_cmp(arg, "Systems") || !str_cmp(arg, "systemy"))
	{
		if (ship->location == ship->lastdoc)
		{
			ship->shipstate = SHIP_DOCKED;
			ship->shipstate1 = 0;
			send_to_char(FB_GREEN "Systemy zdejonizowane." NL, ch);
		}
		else
			send_to_char(FB_RED
			"NIC z tego nie masz gdzie odprowadzi� nadmiaru �adunk�w."
			NL "Musisz poczeka� mo�e same si� rozprosz�." NL, ch);
	}

	if (!str_cmp(arg, "launcher") || !str_cmp(arg, "wyrzutnia"))
	{
		ship->missilestate = MISSILE_READY;
		send_to_char("&GWyrzutnia pocisk�w naprawiona." NL, ch);
	}

	if (!str_cmp(arg, "laser"))
	{
		ship->statet0 = LASER_READY;
		send_to_char("&GG��wny laser naprawiony." NL, ch);
	}
	if (!str_cmp(arg, "dzia�o") || !str_cmp(arg, "turret"))
	{
		if ((turret = get_turret_here(ship, ch->in_room)) != NULL)
		{
			turret->status = TURRET_READY;
			send_to_char("&GDzia�o naprawione." NL, ch);
			save_ship(ship);
		}
		else
			send_to_char(FB_RED "UPS dzia�o gdzie� zwia�o." NL, ch);
	}

	act(PLAIN, "$n ko�czy napraw�.", ch, NULL, argument, TO_ROOM);

	learn_from_success(ch, gsn_shipmaintenance);

}

DEF_DO_FUN( refuel )
{
}

DEF_DO_FUN( addpilot )
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
		send_to_char(FB_RED "Nie mo�esz zrobi� tego tutaj." NL, ch);
		return;
	}

	if (str_cmp(ship->owner, ch->name))
	{

		if (!IS_NPC(ch) && ch->pcdata && ch->pcdata->clan
				&& !str_cmp(ch->pcdata->clan->name, ship->owner))
		{
			if (!IS_LEADER(ch) && !IS_FIRST(ch) && !IS_SECOND(ch))
			{
				send_to_char(FB_RED "To nie tw�j statek!", ch);
				return;
			}
		}
		else
		{
			send_to_char(FB_RED "To nie tw�j statek!", ch);
			return;
		}

	}

	if (argument[0] == '\0')
	{
		send_to_char(FB_RED "Kt�rego pilota doda�?" NL, ch);
		return;
	}

	if (str_cmp(ship->pilot, ""))
	{
		if (str_cmp(ship->copilot, ""))
		{
			send_to_char(FB_RED "Masz ju� obu pilot�w.." NL, ch);
			send_to_char(FB_RED
			"Je�li chcesz ich zmieni�, spr�buj najpierw 'rempilot'."
			NL, ch);
			return;
		}

		STRDUP(ship->copilot, argument);
		send_to_char("Drugi pilot dodany." NL, ch);
		save_ship(ship);
		return;

		return;
	}

	STRDUP(ship->pilot, argument);
	send_to_char("Pilot dodany." NL, ch);
	save_ship(ship);

}

DEF_DO_FUN( rempilot )
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
		send_to_char(FB_RED "Nie mo�esz zrobi� tego tutaj." NL, ch);
		return;
	}

	if (str_cmp(ship->owner, ch->name))
	{

		if (!IS_NPC(ch) && ch->pcdata && ch->pcdata->clan
				&& !str_cmp(ch->pcdata->clan->name, ship->owner))
		{
			if (!IS_LEADER(ch) && !IS_FIRST(ch) && !IS_SECOND(ch))
			{
				send_to_char(FB_RED "To nie tw�j statek!", ch);
				return;
			}
		}
		else
		{
			send_to_char(FB_RED "To nie tw�j statek!", ch);
			return;
		}

	}

	if (argument[0] == '\0')
	{
		send_to_char(FB_RED "Kt�rego pilota chcesz odj��?" NL, ch);
		return;
	}

	if (!str_cmp(ship->pilot, argument))
	{
		STRDUP(ship->pilot, "");
		send_to_char("Pilot usuni�ty." NL, ch);
		save_ship(ship);
		return;
	}

	if (!str_cmp(ship->copilot, argument))
	{
		STRDUP(ship->copilot, "");
		send_to_char("Drugi pilot usuni�ty." NL, ch);
		save_ship(ship);
		return;
	}

	send_to_char(FB_RED "Ta osoba nie figuruje na li�cie pilot�w tego statku."
	NL, ch);

}

DEF_DO_FUN( radar )
{
	int chance, emp;
	SHIP_DATA *ship;
	char buf[MSL];

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED
		"Musisz by� na mostku lub w wie�yczce by to zrobi�!" NL, ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if (!known_biotech(ship, ch))
	{
		ch_printf(ch,
				FB_RED
				"To jest wytw�r obcej biotechnologi, niestety nie masz o niej zielonego poj�cia."
				EOL);
		return;
	}

	emp = 1;
	if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
		emp = 2;

	chance =
			IS_NPC(ch) ?
					ch->top_level : (int) (ch->pcdata->learned[gsn_navigation]);
	if (number_percent() > chance)
	{
		send_to_char(FB_RED
		"Nie uda�o ci si� odczyta� wska�nik�w. Co to za be�kot?"
		NL, ch);
		learn_from_failure(ch, gsn_navigation);
		return;
	}

	act(PLAIN, "$n sprawdza radar.", ch, NULL, argument, TO_ROOM);

	pager_printf(ch, FB_CYAN " ___________________________" NL);
	pager_printf(ch, "( " FB_WHITE "%-25s" FB_CYAN " )" NL,
			!IS_SET(ship->starsystem->flags, STARS_FLAG_TEMP) ?
					ship->starsystem->name : "Gdzie� w galaktyce");
	pager_printf(ch,
			"|-----------------------------------------------------------------------------."
			NL);
	pager_printf(ch,
			"|        Transponder lub nazwa      |     Wsp�rz�dne lokalne    | Odleglo��  |"
			NL);
	pager_printf(ch,
			"|-----------------------------------------------------------------------------|"
			NL);
	for (auto* star : ship->starsystem->stars)
	{
		if (srange((star->xpos - ship->vx), (star->ypos - ship->vy),
				(star->zpos - ship->vz)) * emp < ship->astro_array * 150000)
			pager_printf(ch,
					"| %-23s (gwiazda) | %-8.0f %-8.0f %-8.0f | %-10.0f |"
					NL, star->name, star->xpos - ship->vx,
					star->ypos - ship->vy, star->zpos - ship->vz,
					(srange((star->xpos - ship->vx), (star->ypos - ship->vy),
							(star->zpos - ship->vz)) - star->radius));
	}
	for (auto* planet : ship->starsystem->planets)
	{
		if (srange((planet->xpos - ship->vx), (planet->ypos - ship->vy),
				(planet->zpos - ship->vz)) * emp < ship->astro_array * 90000)
			pager_printf(ch,
					"| %-23s (planeta) | %-8.0f %-8.0f %-8.0f | %-10.0f |"
					NL, planet->name, planet->xpos - ship->vx,
					planet->ypos - ship->vy, planet->zpos - ship->vz,
					(srange((planet->xpos - ship->vx),
							(planet->ypos - ship->vy),
							(planet->zpos - ship->vz)) - planet->radius)); // To albo do powierzchni albo do jadra, konsekwencji troche!!
	}
	for (auto* moon : ship->starsystem->moons)
	{
		if (srange((moon->xpos - ship->vx), (moon->ypos - ship->vy),
				(moon->zpos - ship->vz)) * emp < ship->astro_array * 50000)
			pager_printf(ch,
					"| %-23s (satelita)| %-8.0f %-8.0f %-8.0f | %-10.0f |"
					NL, moon->name, moon->xpos - ship->vx,
					moon->ypos - ship->vy, moon->zpos - ship->vz,
					srange((moon->xpos - ship->vx), (moon->ypos - ship->vy),
							(moon->zpos - ship->vz)));
	}
	pager_printf(ch,
			"|-----------------------------------------------------------------------------|"
			NL);
	for (auto* astro : ship->starsystem->astros)
	{
		if (srange((ship->vx - astro->ox), (ship->vy - astro->oy),
				(ship->vz - astro->oz)) * emp
				> (astro->value + ship->sensor) * 200)
			continue;
		if (srange((ship->vx - astro->ox), (ship->vy - astro->oy),
				(ship->vz - astro->oz)) * emp
				> (astro->value + ship->sensor) * 150)
		{
			pager_printf(ch, "| " FB_RED "%-33s" FB_CYAN " | " FB_RED
			"%-8.0f %-8.0f %-8.0f " FB_CYAN "|            |" NL,
					astro_names[astro->type],
					astro->ox - ship->vx + 10 * number_range(-20, 20),
					astro->oy - ship->vy + 10 * number_range(-20, 20),
					astro->oz - ship->vz + 10 * number_range(-20, 20));
			continue;
		}
		pager_printf(ch, "| %-33s | %-8.0f %-8.0f %-8.0f | %-10.0f |" NL,
				astro->name, astro->ox - ship->vx, astro->oy - ship->vy,
				astro->oz - ship->vz,
				srange((ship->vx - astro->ox), (ship->vy - astro->oy),
						(ship->vz - astro->oz)));
	}
	pager_printf(ch,
			"|-----------------------------------------------------------------------------|"
			NL);
	for (auto* target : ship->starsystem->ships)
	{
		snprintf(buf, 28, "%s", SHIPNAME(target));
		if (target->cloack != 0
				|| (srange((target->vz - ship->vz), (target->vy - ship->vy),
						(target->vx - ship->vx)) * emp
						> ship->sensor * 3000 + target->size * 10 + 2000))
		{
			continue;
		}
		if (srange((target->vz - ship->vz), (target->vy - ship->vy),
				(target->vx - ship->vx)) * emp
				> ship->sensor * 2900 + target->size * 9 + 1500)
		{
			pager_printf(ch, "| " FB_RED "Co� tam jest.                     "
			FB_CYAN "|" FB_RED " %-8.0f %-8.0f %-8.0f " FB_CYAN
			"|            |" NL,
					target->vx - ship->vx + 100 * number_range(-5, 5),
					target->vy - ship->vy + 100 * number_range(-5, 5),
					target->vz - ship->vz + 100 * number_range(-5, 5));
			continue;
		}
		if (target != ship)
		{
			// Przyjacielski statek
			if (!str_cmp(target->owner, ship->owner))
			{
				pager_printf(ch,
						"| " FB_GREEN "%-28s %s " FB_CYAN "| %-8.0f %-8.0f %-8.0f | %-10.0f |" NL,
						buf, target->sslook, target->vx - ship->vx,
						target->vy - ship->vy, target->vz - ship->vz,
						srange((target->vz - ship->vz), (target->vy - ship->vy),
								(target->vx - ship->vx)));
			}
			else
			{
				pager_printf(ch,
						"| %-28s %s | %-8.0f %-8.0f %-8.0f | %-10.0f |" NL,
						know_trans(ship, target) ? buf : target->transponder,
						target->sslook, target->vx - ship->vx,
						target->vy - ship->vy, target->vz - ship->vz,
						srange((target->vz - ship->vz), (target->vy - ship->vy),
								(target->vx - ship->vx)));

			}
		}
	}
	pager_printf(ch,
			"|-----------------------------------------------------------------------------|"
			NL);
	for (auto* missile : ship->starsystem->missiles)
	{
		if (srange((missile->mx - ship->vx), (missile->my - ship->vy),
				(missile->mz - ship->vz)) * emp > ship->sensor * 2000)
		{
			continue;
		}
		if (srange((missile->mx - ship->vx), (missile->my - ship->vy),
				(missile->mz - ship->vz)) * emp > ship->sensor * 1300)
		{
			send_to_char(
					FB_RED
					"| To chyba jaka� rakieta, ale gdzie ona leci?                                 |"
					NL, ch);
			continue;
		}
		pager_printf(ch, "| %-33s | %-8.0f %-8.0f %-8.0f | %-10.0f |" NL,
				missile->missiletype == CONCUSSION_MISSILE ?
						"Pocisk Samonaprowadzalny" :
						(missile->missiletype == PROTON_TORPEDO ?
								"Torpeda" :
								(missile->missiletype == HEAVY_ROCKET ?
										"Ci�ka Rakieta" : "Ci�ka Bomba")),
				missile->mx - ship->vx, missile->my - ship->vy,
				missile->mz - ship->vz,
				srange((missile->mx - ship->vx), (missile->my - ship->vy),
						(missile->mz - ship->vz)));
	}

	pager_printf(ch,
			"|-----------------------------------------------------------------------------|"
			NL);
	pager_printf(ch, "| Twoje wsp�rz�dne solarne: %-8.0f %-8.0f %-7.0f /" NL,
			ship->vx, ship->vy, ship->vz);
	pager_printf(ch,
			"`------------------------------------------------------" NL);

	learn_from_success(ch, gsn_navigation);

}

DEF_DO_FUN( autotrack )
{
	SHIP_DATA *ship;
	int chance;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz byc w kokpicie statku by to zrobi�." NL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if ((ship = ship_from_pilotseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Nie siedzisz w fotelu pilota!" NL, ch);
		return;
	}

	if (IS_SET(ship->starsystem->flags, STARS_FLAG_EMP))
	{
		echo_to_cockpit(ship,
				FB_RED
				"W systemie panuje burza elektromagnetyczna, komputer nie b�dzie w stanie �ledzi� celu");
		return;
	}
	if (is_platform(ship))
	{
		send_to_char(FB_RED "Platformy nie maj� systemu samonaprowadzalnego!"
		NL, ch);
		return;
	}
	if (is_capital(ship))
	{
		send_to_char(FB_RED "Ten statek jest zbyt du�y na autonaprowadzanie!"
		NL, ch);
		return;
	}
	if (is_huge(ship))
	{
		send_to_char(FB_RED "Ten statek jest zbyt du�y na autonaprowadzanie!"
		NL, ch);
		return;
	}

	if (autofly(ship))
	{
		send_to_char(FB_RED "Musisz najpierw wy��czy� autopilota..." NL, ch);
		return;
	}

	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_shipsystems]);
	if (number_percent() > chance)
	{
		send_to_char(FB_RED
		"Nie za bardzo wiesz, kt�r� by tu d�wigni� przestawi�."
		NL, ch);
		learn_from_failure(ch, gsn_shipsystems);
		return;
	}

	act(PLAIN, "$n prze��cza d�wigni� na panelu sterowania.", ch, NULL,
			argument, TO_ROOM);
	if (ship->autotrack)
	{
		ship->autotrack = false;
		echo_to_cockpit(ship, FB_YELLOW "Autonaprowadzanie WY��CZONE.");
	}
	else
	{
		ship->autotrack = true;
		echo_to_cockpit(ship, FB_YELLOW "Autonaprowadzanie W��CZONE.");
	}

	learn_from_success(ch, gsn_shipsystems);

}

//Modified by Thanos (zrobi�em tak, by wy�wietla�o list� wszystkich stak�w
//publicznych a nie tylko pluogusa i tocc�)
DEF_DO_FUN( pluogus )
{
	int itt;
	COURSE_DATA *course;
	char arg1[MAX_INPUT_LENGTH];
	bool found = false;
	bool all = false;

	if (!get_comlink(ch))
	{
		send_to_char("Nie masz przy sobie urz�dzenia komunikacyjnego!" NL, ch);
		return;
	}

	argument = one_argument(argument, arg1);
	all = !str_cmp("all", arg1);
	all |= !str_cmp("wszystkie", arg1);
	all |= !str_cmp("", arg1);

	for (auto* ship : ship_list)
	{
		if (ship->ship_public && !ship->stops.empty() && ship->curr_stop)
		{
			if (!all && argument && !nifty_is_name_prefix(arg1, ship->name))
				continue;
			found = true;
			pager_printf(ch, "Kurs statku " FB_WHITE "%s" PLAIN ":" NL,
					ship->name);
			/* current port */
			if (ship->bus_pos < 7 && ship->bus_pos > 1)
				pager_printf(ch,
						"Aktualnie zadokowany na stacji: " FB_WHITE "%s"
						PLAIN "." NL, ship->curr_stop->stop_name);
			/* destinations */
			send_to_pager("Nast�pne stacje: ", ch);
			if (ship->bus_pos <= 1)
				pager_printf(ch, "%s,", ship->curr_stop->stop_name);

			course = ship->curr_stop;

			for (itt = 0; itt < 3 && !ship->stops.empty(); itt++)
			{
				auto c_it = std::find(ship->stops.begin(), ship->stops.end(), course);
				if (c_it != ship->stops.end())
					++c_it;
				course = (c_it != ship->stops.end()) ? *c_it : ship->stops.front();
				if (itt)
					pager_printf(ch, ",");

				pager_printf(ch, " %s", course->stop_name);

			}
			pager_printf(ch, NL NL);
		}
	}
	if (!found && argument)
	{
		pager_printf(ch, "Nie ma takiego statku publicznego." NL);
	}
	//done
}

DEF_DO_FUN( fly )
{
}

DEF_DO_FUN( drive )
{
	int dir;
	SHIP_DATA *ship;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED
		"You must be in the drivers seat of a land vehicle to do that!"
		NL, ch);
		return;
	}

	if (ship->clazz < LAND_SPEEDER)
	{
		send_to_char(FB_RED "This isn't a land vehicle!" NL, ch);
		return;
	}

	if (ship->shipstate == SHIP_DISABLED)
	{
		send_to_char(FB_RED "The drive is disabled." NL, ch);
		return;
	}

	if (ship->energy < 1)
	{
		send_to_char(FB_RED "Theres not enough fuel!" NL, ch);
		return;
	}

	if ((dir = get_door(argument)) == -1)
	{
		send_to_char("Usage: drive <direction>" NL, ch);
		return;
	}

	drive_ship(ch, ship, get_exit(ship->location, dir), 0);

}

ch_ret drive_ship(CHAR_DATA *ch, SHIP_DATA *ship, EXIT_DATA *pexit, int fall)
{
	ROOM_INDEX_DATA *in_room;
	ROOM_INDEX_DATA *to_room;
	ROOM_INDEX_DATA *original;
	char buf[MAX_STRING_LENGTH];
	const char *txt;
	const char *dtxt;
	ch_ret retcode;
	int door;
	bool drunk = false;

	if (!IS_NPC(ch))
		if (IS_DRUNK(ch, 2) && (ch->position != POS_SHOVE)
				&& (ch->position != POS_DRAG))
			drunk = true;

	if (drunk && !fall)
	{
		door = number_door();
		pexit = get_exit(ship->location, door);
	}

#ifdef DEBUG
	if(pexit)
	{
		sprintf(buf, "drive_ship: %s to door %d", ch->name, pexit->vdir);
		log_string(buf);
	}
#endif

	retcode = rNONE;
	txt = NULL;

	in_room = ship->location;
	if (!pexit || (to_room = pexit->to_room) == NULL)
	{
		if (drunk)
			send_to_char("You drive into a wall in your drunken state." NL, ch);
		else
			send_to_char("Alas, you cannot go that way." NL, ch);
		return rNONE;
	}

	door = pexit->vdir;

	if (IS_SET(pexit->flags, EX_WINDOW) && !IS_SET(pexit->flags, EX_ISDOOR))
	{
		send_to_char("Alas, you cannot go that way." NL, ch);
		return rNONE;
	}

	if (IS_SET(pexit->flags, EX_PORTAL) && IS_NPC(ch))
	{
		act(PLAIN, "Mobs can't use portals.", ch, NULL, NULL, TO_CHAR);
		return rNONE;
	}

	if (IS_SET(pexit->flags, EX_NOMOB) && IS_NPC(ch))
	{
		act(PLAIN, "Mobs can't enter there.", ch, NULL, NULL, TO_CHAR);
		return rNONE;
	}

	if (IS_SET(pexit->flags, EX_CLOSED)
			&& (IS_SET(pexit->flags, EX_NOPASSDOOR)))
	{
		if (!IS_SET(pexit->flags, EX_SECRET) && !IS_SET(pexit->flags, EX_DIG))
		{
			if (drunk)
			{
				act(PLAIN, "$n drives into the $d in $s drunken state.", ch,
						NULL, pexit->keyword, TO_ROOM);
				act(PLAIN, "You drive into the $d in your drunken state.", ch,
						NULL, pexit->keyword, TO_CHAR);
			}
			else
				act(PLAIN, "The $d is closed.", ch, NULL, pexit->keyword,
				TO_CHAR);
		}
		else
		{
			if (drunk)
				send_to_char("You hit a wall in your drunken state." NL, ch);
			else
				send_to_char("Alas, you cannot go that way." NL, ch);
		}

		return rNONE;
	}

	/*
	 if ( distance > 1 )
	 if ( (to_room=generate_exit(in_room, &pexit)) == NULL )
	 send_to_char( "Alas, you cannot go that way." NL, ch );
	 */
	if (room_is_private(ch, to_room))
	{
		send_to_char("That room is private right now." NL, ch);
		return rNONE;
	}

	if (!fall)
	{
		if (IS_SET(to_room->room_flags, ROOM_INDOORS)
				|| IS_SET(to_room->room_flags, ROOM_SPACECRAFT)
				|| to_room->sector_type == SECT_INSIDE)
		{
			send_to_char("You can't drive indoors!" NL, ch);
			return rNONE;
		}

		if (IS_SET(to_room->room_flags, ROOM_NO_DRIVING))
		{
			send_to_char("You can't take a vehicle through there!" NL, ch);
			return rNONE;
		}

		if (in_room->sector_type
				== SECT_AIR || to_room->sector_type == SECT_AIR || IS_SET(pexit->flags, EX_FLY))
		{
			if (ship->clazz > CLOUD_CAR)
			{
				send_to_char("You'd need to fly to go there." NL, ch);
				return rNONE;
			}
		}

		if (in_room->sector_type == SECT_WATER_NOSWIM
				|| to_room->sector_type == SECT_WATER_NOSWIM
				|| to_room->sector_type == SECT_WATER_SWIM
				|| to_room->sector_type == SECT_UNDERWATER
				|| to_room->sector_type == SECT_OCEANFLOOR)
		{

			if (ship->clazz != OCEAN_SHIP)
			{
				send_to_char("You'd need a boat to go there." NL, ch);
				return rNONE;
			}

		}

		if (IS_SET(pexit->flags, EX_CLIMB))
		{

			if (ship->clazz < CLOUD_CAR)
			{
				send_to_char("You need to fly or climb to get up there." NL,
						ch);
				return rNONE;
			}
		}

	}

	if (to_room->tunnel > 0)
	{
		if ((int)to_room->people.size() >= to_room->tunnel)
		{
			send_to_char("There is no room for you in there." NL, ch);
			return rNONE;
		}
	}

	if (fall)
		txt = "falls";
	else if (!txt)
	{
		if (ship->clazz < OCEAN_SHIP)
			txt = "fly";
		else if (ship->clazz == OCEAN_SHIP)
		{
			txt = "float";
		}
		else if (ship->clazz > OCEAN_SHIP)
		{
			txt = "drive";
		}
	}
	sprintf(buf, "$n %ss the vehicle $T.", txt);
	act(COL_ACTION, buf, ch, NULL, dir_name[door], TO_ROOM);
	sprintf(buf, "You %s the vehicle $T.", txt);
	act(COL_ACTION, buf, ch, NULL, dir_name[door], TO_CHAR);
	sprintf(buf, FB_YELLOW "%s %ss %s.", ship->name, txt, dir_name[door]);
	echo_to_room(ship->location, buf);

	extract_ship(ship);
	ship_to_room(ship, to_room);

	ship->location = to_room;
	ship->lastdoc = ship->location;

	if (fall)
		txt = "falls";
	else if (ship->clazz < OCEAN_SHIP)
		txt = "flys in";
	else if (ship->clazz == OCEAN_SHIP)
	{
		txt = "floats in";
	}
	else if (ship->clazz > OCEAN_SHIP)
	{
		txt = "drives in";
	}

	switch (door)
	{
	default:
		dtxt = "somewhere";
		break;
	case 0:
		dtxt = "the south";
		break;
	case 1:
		dtxt = "the west";
		break;
	case 2:
		dtxt = "the north";
		break;
	case 3:
		dtxt = "the east";
		break;
	case 4:
		dtxt = "below";
		break;
	case 5:
		dtxt = "above";
		break;
	case 6:
		dtxt = "the south-west";
		break;
	case 7:
		dtxt = "the south-east";
		break;
	case 8:
		dtxt = "the north-west";
		break;
	case 9:
		dtxt = "the north-east";
		break;
	}

	sprintf(buf, FB_YELLOW "%s %s from %s.", ship->name, txt, dtxt);
	echo_to_room(ship->location, buf);

	auto people_snapshot = ch->in_room->people;
	for (auto* rch : people_snapshot)
	{
		original = rch->in_room;
		char_from_room(rch);
		char_to_room(rch, to_room);
		do_look(rch, (char*) "auto");
		char_from_room(rch);
		char_to_room(rch, original);
	}

	/*
	 if (  CHECK FOR FALLING HERE
	 &&   fall > 0 )
	 {
	 if (!IS_AFFECTED( ch, AFF_FLOATING )
	 || ( ch->mount && !IS_AFFECTED( ch->mount, AFF_FLOATING ) ) )
	 {
	 send_to_char( COL_HURT, ch );
	 send_to_char( "OUCH! You hit the ground!" NL, ch );
	 WAIT_STATE( ch, 20 );
	 retcode = damage( ch, ch, 50 * fall, TYPE_UNDEFINED );
	 }
	 else
	 {
	 send_to_char( COL_FORCE, ch );
	 send_to_char( "You lightly float down to the ground." NL, ch );
	 }
	 }

	 */
	return retcode;

}

DEF_DO_FUN( chaff )
{
	int chance;
	SHIP_DATA *ship;

	if ((ship = ship_from_cockpit(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Musisz byc w kokpicie statku by to zrobi�." NL,
				ch);
		return;
	}

	if (isnt_in_realspace(ship))
	{
		send_to_char(FB_RED
		"W obecnej chwili i miejscu nie mo�esz tego zrobi�!" NL, ch);
		return;
	}

	if ((ship = ship_from_coseat(ch->in_room)) == NULL)
	{
		send_to_char(FB_RED "Flary odpala sie z siedzenia drugiego pilota" NL,
				ch);
		return;
	}

	if (ship->chaff <= 0)
	{
		send_to_char(FB_RED "Nie masz flar" NL, ch);
		return;
	}
	chance =
			IS_NPC(ch) ?
					ch->top_level :
					(int) (ch->pcdata->learned[gsn_weaponsystems]);
	if (number_percent() > chance)
	{
		send_to_char(FB_RED "Nie wiesz kt�ry przycisk wcisn��" NL, ch);
		learn_from_failure(ch, gsn_weaponsystems);
		return;
	}

	ship->chaff--;

	ship->chaff_released++;

	send_to_char("Odpalasz flare" NL, ch);
	act(PLAIN, "$n naciska co� na pulpicie kontroli uzbrojenia" NL, ch, NULL,
			argument, TO_ROOM);
	echo_to_cockpit(ship,
	FB_YELLOW "Seria termicznych wabik�w ulatuje w kosmos."
	NL);

	learn_from_success(ch, gsn_weaponsystems);

}

bool ship_in_closebay(SHIP_DATA *ship)
{
	SHIP_DATA *target;
	HANGAR_DATA *hangar;

	if ((target = ship_from_room(ship->location)) != NULL)
		if ((hangar = hangar_from_room(target, VNUM(ship->location))) != NULL)
			if (hangar->status == -1)
				return true;
	return false;
}

bool autofly(SHIP_DATA *ship)
{

	if (!ship)
		return false;

	if (ship->type == MOB_SHIP)
		return true;

	if (ship->autopilot)
		return true;

	return false;

}

/***********************
 void  do_findship(CHAR_DATA *ch, char *argument ) //Dremor:D

 {
 SHIP_DATA *ship;
 char buf[MAX_STRING_LENGTH];

 if (argument[0] == '\0')
 {
 send_to_char("Syntax: findship <nazwa statku>" NL, ch );
 return;
 }

 for (auto* ship : ship_list)
 {
 if ( !str_cmp(ship->owner,ch->name) )
 {
 send_to_char(FB_RED "Posiadasz juz statek!" NL, ch );
 return;
 }
 }

 ship = get_ship( argument );

 if (ship == NULL)
 {
 send_to_char(FB_RED "Nie ma takiego statku!" NL,ch);
 return;
 }

 if (!IS_SET(ship->flags, SHIP_HIDDEN) )
 {
 send_to_char(FB_RED "Ten statek nie jest ukryty! Spr�buj wpisac allships!" NL, ch );
 return;
 }

 if (IS_SET(ship->flags, SHIP_HIDDEN) && (ch->in_room->vnum != ship->location) )
 {
 return;
 }
 else if ( str_cmp(ship->owner,"") )
 return;
 else
 {
 STRFREE( ship->owner );
 STRFREE( ship->pilot );
 STRFREE( ship->copilot );
 ship->owner = STRALLOC( ch->name );
 ship->pilot = STRALLOC( "" );
 ship->copilot = STRALLOC( "" );
 REMOVE_BIT(ship->flags, SHIP_HIDDEN);
 sprintf( buf, "%s Znaleziony przez: %s.", ship->name, ch->name );
 logfs( buf, 0 );
 ch_printf( ch, "Znalaz�e�: %s!" NL, ship->name );
 return;
 }
 }
 ****************************/

/* Generic Pilot Command To use as template

 DEF_DO_FUN( hmm )
 {
 char arg[MAX_INPUT_LENGTH];
 int chance;
 SHIP_DATA *ship;

 strcpy( arg, argument );

 switch( ch->substate )
 {
 default:
 if (  (ship = ship_from_cockpit(ch->in_room))  == NULL )
 {
 send_to_char(FB_RED "Musisz byc w kokpicie statku by to zrobi�." NL,ch);
 return;
 }
 if (ship->shipstate == SHIP_HYPERSPACE)
 {
 send_to_char(FB_RED "You can only do that in realspace!" NL,ch);
 return;
 }
 if (ship->shipstate == SHIP_DISABLED)
 {
 send_to_char(FB_RED "The ships drive is disabled. Unable to manuever." NL,ch);
 return;
 }
 if (ship->shipstate == SHIP_DOCKED)
 {
 send_to_char(FB_RED "You can't do that until after you've launched!" NL,ch);
 return;
 }
 if (ship->shipstate != SHIP_READY)
 {
 send_to_char(FB_RED "Please wait until the ship has finished its current manouver." NL,ch);
 return;
 }

 if ( ship->energy <1 )
 {
 send_to_char(FB_RED "Theres not enough fuel!" NL,ch);
 return;
 }

 if ( is_fighter(ship) )
 chance = IS_NPC(ch) ? ch->top_level
 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
 if ( is_midship(ship) )
 chance = IS_NPC(ch) ? ch->top_level
 : (int)  (ch->pcdata->learned[gsn_midships]) ;
 if ( is_capital(ship) )
 chance = IS_NPC(ch) ? ch->top_level
 : (int) (ch->pcdata->learned[gsn_capitalships]);
 if ( number_percent( ) < chance )
 {
 send_to_char( "&G" NL, ch);
 act( PLAIN, "$n does  ...", ch,
 NULL, argument , TO_ROOM );
 echo_to_room( get_room_index(ship->cockpit) , "");
 add_timer ( ch , TIMER_DO_FUN , 1 , do_hmm , 1 );
 STRDUP( ch->dest_buf,arg);
 return;
 }
 send_to_char(FB_RED "You fail to work the controls properly." NL,ch);
 if ( is_fighter(ship) )
 learn_from_failure( ch, gsn_starfighters );
 if ( is_midship(ship) )
 learn_from_failure( ch, gsn_midships );
 if ( is_capital(ship) )
 learn_from_failure( ch, gsn_capitalships );
 return;

 case 1:
 if ( !*ch->dest_buf )
 return;
 snprintf(arg, sizeof(arg), "%s", ch->dest_buf);
 DISPOSE( ch->dest_buf);
 break;

 case SUB_TIMER_DO_ABORT:
 DISPOSE( ch->dest_buf );
 ch->substate = SUB_NONE;
 if ( (ship = ship_from_cockpit(ch->in_room)) == NULL )
 return;
 send_to_char(FB_RED "aborted." NL, ch);
 echo_to_room( get_room_index(ship->cockpit) , "");
 if (ship->shipstate != SHIP_DISABLED)
 ship->shipstate = SHIP_READY;
 return;
 }

 ch->substate = SUB_NONE;

 if ( (ship = ship_from_cockpit(ch->in_room)) == NULL )
 {
 return;
 }

 send_to_char( "&G" NL, ch);
 act( PLAIN, "$n does  ...", ch,
 NULL, argument , TO_ROOM );
 echo_to_room( get_room_index(ship->cockpit) , "");


 if ( is_fighter(ship) )
 learn_from_success( ch, gsn_starfighters );
 if ( is_midship(ship) )
 learn_from_success( ch, gsn_midships );
 if ( is_capital(ship) )
 learn_from_success( ch, gsn_capitalships );

 }

 DEF_DO_FUN( hmm )
 {
 char arg[MAX_INPUT_LENGTH];
 int chance;
 SHIP_DATA *ship;


 if (  (ship = ship_from_cockpit(ch->in_room))  == NULL )
 {
 send_to_char(FB_RED "Musisz byc w kokpicie statku by to zrobi�." NL,ch);
 return;
 }

 if (ship->shipstate == SHIP_HYPERSPACE)
 {
 send_to_char(FB_RED "You can only do that in realspace!" NL,ch);
 return;
 }
 if (ship->shipstate == SHIP_DISABLED)
 {
 send_to_char(FB_RED "The ships drive is disabled. Unable to manuever." NL,ch);
 return;
 }
 if (ship->shipstate == SHIP_DOCKED)
 {
 send_to_char(FB_RED "You can't do that until after you've launched!" NL,ch);
 return;
 }
 if (ship->shipstate != SHIP_READY)
 {
 send_to_char(FB_RED "Please wait until the ship has finished its current manouver." NL,ch);
 return;
 }

 if ( ship->energy <1 )
 {
 send_to_char(FB_RED "Theres not enough fuel!" NL,ch);
 return;
 }

 if ( is_fighter(ship) )
 chance = IS_NPC(ch) ? ch->top_level
 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
 if ( is_midship(ship) )
 chance = IS_NPC(ch) ? ch->top_level
 : (int)  (ch->pcdata->learned[gsn_midships]) ;
 if ( is_capital(ship) )
 chance = IS_NPC(ch) ? ch->top_level
 : (int) (ch->pcdata->learned[gsn_capitalships]);
 if ( number_percent( ) > chance )
 {
 send_to_char(FB_RED "You fail to work the controls properly." NL,ch);
 if ( is_fighter(ship) )
 learn_from_failure( ch, gsn_starfighters );
 if ( is_midship(ship) )
 learn_from_failure( ch, gsn_midships );
 if ( is_capital(ship) )
 learn_from_failure( ch, gsn_capitalships );
 return;
 }

 send_to_char( "&G" NL, ch);
 act( PLAIN, "$n does  ...", ch,
 NULL, argument , TO_ROOM );
 echo_to_room( get_room_index(ship->cockpit) , "");



 if ( is_fighter(ship) )
 learn_from_success( ch, gsn_starfighters );
 if ( is_midship(ship) )
 learn_from_success( ch, gsn_midships );
 if ( is_capital(ship) )
 learn_from_success( ch, gsn_capitalships );

 }

 */
