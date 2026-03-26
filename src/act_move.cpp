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
 *			   Player movement module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"

const int movement_loss[SECT_MAX] =
{ 1, 2, 2, 3, 4, 6, 4, 1, 6, 10, 6, 5, 7, 4 };

const char *const dir_name_orig[] =
{ "north", "east", "south", "west", "up", "down", "north-east", "north-west", "south-east", "south-west", "somewhere" };

/*
 * added by Thanos (te dwie tablice r�ni� si� tylko jednym wyrazem
 * 'g�ra' i g�r�') niemniej jednak s� to mianowniki i celowniki
 * nazw kierunk�w
 */
const char *const dir_name[] =
{ "p�noc", "wsch�d", "po�udnie", "zach�d", "g�ra", "d�", "p�nocny-wsch�d", "p�nocny-zach�d", "po�udniowy-wsch�d", "po�udniowy-zach�d",
		"gdzie�" };

const char *const dir_where_name[] =
{ "p�noc", "wsch�d", "po�udnie", "zach�d", "g�r�", "d�", "p�nocny-wsch�d", "p�nocny-zach�d", "po�udniowy-wsch�d", "po�udniowy-zach�d",
		"gdzie�" };

/* kierunki 'powrotne' --> inna kolejno�� !!!*/
const char *const dir_rev_name[] =
{ "z po�udnia", "z zachodu", "z p�nocy", "ze wschodu", "z do�u", "z g�ry", "z po�udniowego-zachodu", "z po�udniowego-wschodu",
		"z p�nocnego-zachodu", "z p�nocnego-wschodu", "sk�d�" };

//Rozdaje kierunk�w, na potrzebe skilli
const char *const dir_type_name[] =
{ "p�nocny", "wschodni", "po�udniowy", "zachodni", "ku g�rze", "ku dole", "p�nocno-wschodni", "p�nocno-zachodni", "po�udniowo-wschodni",
		"po�udniowo-zachodni", "nieokre�lony" };

const int trap_door[] =
{
TRAP_N, TRAP_E, TRAP_S, TRAP_W, TRAP_U, TRAP_D,
TRAP_NE, TRAP_NW, TRAP_SE, TRAP_SW };

const int rev_dir[] =
{ 2, 3, 0, 1, 5, 4, 9, 8, 7, 6, 10 };

std::forward_list<ROOM_INDEX_DATA*> vroom_hash[64];

/*
 * Local functions.
 */
bool has_key args( ( CHAR_DATA *ch, int key ) );

const char *const sect_names[SECT_MAX][2] =
{
{ "Wn�trze", "inside" },
{ "Miejska ulica", "cities" },
{ "Otwarte Pole", "fields" },
{ "W lesie", "forests" },
{ "Wzg�rze", "hills" },
{ "Na szczycie g�ry", "mountains" },
{ "W wodzie", "waters" },
{ "W g��bokiej wodzie", "waters" },
{ "Pod wod�", "underwaters" },
{ "W powietrzu", "air" },
{ "Na pustyni", "deserts" },
{ "Gdzie�", "unknown" },
{ "Tafla oceanu", "ocean floor" },
{ "Pod ziemi�", "underground" } };

const int sent_total[SECT_MAX] =
{ 4, 24, 4, 4, 1, 1, 1, 1, 1, 2, 2, 1, 1, 1 };

const char *const room_sents[SECT_MAX][25] =
{
{ "The rough hewn walls are made of granite.", "You see an occasional spider crawling around.",
		"You notice signs of a recent battle from the bloodstains on the floor.",
		"This place hasa damp musty odour not unlike rotting vegetation." },

{ "Kilku podejrzanych osobnik�w chowa przed tob� zawarto�� ma�ej torebki, kt�r� jeszcze przed chwil� nami�tnie upychali w lufce.",
		"W tym rejonie niebo cz�ciej zas�aniaj� wielkie frachtowce, to zapewne tutaj znajduj� si� najwi�ksze l�dowiska miasta.",
		"Przechodz�c dostrzegasz swoje odbicie w transpalistalowych oknach budykn�w."
				"Przez iluminatory dostrzegasz jakiego� transportowca osiadaj�cego na jednym z pobliskich l�dowisk.",
		"Grupka ludzi od d�u�szego czasu pr�buje przedosta� si� na drug� stron� ulicy tworz�c do�� zabawne widowisko.",
		"Widzisz jak jaki� wyg�odzony Chadra-Fan rozgl�da si� w poszukiwaniu po�ywienia.",
		"Wysokie wie�owce strzelaj� kopu�ami prosto w niebo mijaj�c chmury w po�owie swojej wysoko�ci.",
		"Widzisz jak ma�a grupka osobnik�w przer�nych ras pr�buje zarobi� kilka kredytek pokazuj�c cyrkowe niemal sztuczki.",
		"Dwoje Ewok�w naprzeciwko ciebie k��ci si� niemal nie zauwa�aj�c twojej obecno�ci.",
		"Ch�odna bryza delikatnie muska twoje policzki by po chwili zn�w znikn�� w g�szczu drapaczy chmur.",
		"Chmury nad twoj� g�ow� zas�aniaj� niekt�re budynki nie daj�c ci dostrzec ich wysoko�ci.",
		"Tu� obok ciebie wolno i ostro�nie przelatuje �cigacz pilotowany prze jakiego� Rodianina, dziwi ci� jego dba�o�� o przechodni�w.",
		"Powietrze na tej wysoko�ci jest g�ste i mimo klimatyzacji w tunelach ci�ko si� nim oddycha.",
		"Ogromna ilo�� zapach�w miasta wystawia twoje zmys�y na ci�k� pr�b�.",
		"G�o�ny wrzask dobiega ci� z daleka, daje ci to do zrozumienia, �e nie jeste� tu do ko�ca bezpieczny.",
		"Gdy tylko mijasz jaki� budynek, na twojej drodze zaraz pojawia si� nast�pny i nast�pny, tak w niesko�czono��.",
		"Miasto rozci�ga si� na wszystkie kierunki w niesko�czono�� tworz�c imponuj�c� arteri� poch�aniaj�c� przestrzeni� ca�� planet�.",
		"Ulica tutaj jest prosta jak strza�a i d�uga, na tyle bo osi�gn�� �cigaczem konkretn� pr�dko��.",
		"Pod�o�e jest gor�ce od panuj�cego tutaj ruchu, czujesz jak kr�ci ci si� w g�owie przy ka�dym g��bszym wdechu.",
		"Zdajesz sobie spraw�, jak �atwo jest zab��dzi� w tak wielkim i zat�oczonym mie�cie.",
		"Nad i pod tob� widzisz pl�tanin� innych ulic zamkni�tych w durastalowych tunelach.",
		"Na tym poziomie jest wiele wej�� do r�nych budynk�w, to zapewne mieszkaniowa cz�� miasta.",
		"Mimo, �e iluminatory tuneli ulicznych rzadko s� otwarte, niskie barierki wstrz�sowe postawiono tu ze wzgl�d�w bezpiecze�stwa.",
		"Wygl�daj�c przez duraplastowe os�ony ulicy zauwa�asz ni�sze, r�wnie imponuj�ce obszary miasta.",
		"Nie wiesz co to za krzyki, kt�re s�yszysz, ale id�c dalej masz wra�enie jakby si� nasila�y.", },

{ "You notice sparce patches of brush and shrubs.", "There is a small cluster of trees far off in the distance.",
		"Around you are grassy fields as far as the eye can see.",
		"Throughout the plains a wide variety of weeds and wildflowers are scattered." },

{ "D�ugie ciemne zaro�la nie pozwalaj� ci dostrzec co jest dalej.", "Jest tu bardzo wiele starych i wielkich drzew przypominaj�cych d�by.",
		"Samotna wierzba stoi tutaj sprawiaj�c wra�enie, �e jest pani� tego lasu.",
		"To your left is a patch of bright white birch trees, slender and tall." },

{ "The rolling hills are lightly speckled with violet wildflowers." },

{ "W tej skalistej g�rze dostrzec mo�na wiele ciekawych kryj�wek." },

{ "Tafla wody jest g�adka jak powierzchnia duraplastowej szyby." },

{ "Rough waves splash about angrily." },

{ "Stadko ma�ych rybek przep�ywa obok praktycznie ci� nie dostrzegaj�c." },

{ "Ziemia jest daleko daleko pod twoimi stopami.", "Ma�a chmurka szybuje obok ciebie." },

{ "Wsz�dzie, jak daleko si�gasz wzrokiem, piasek.", "Zdaje ci si�, �e widzisz oaz� gdzie� oddali." },

{ "Nie zauwa�asz nic szczeg�lnego." },

{ "Dno oceanu pokrywa rafa koralowa i jakie� pojedyncze ska�ki." },

{ "Stoisz w d�ugim skalistym tunelu." }

};

int wherehome(CHAR_DATA *ch)
{
	if (ch->plr_home)
		return ch->plr_home->vnum;

	if (get_trust(ch) >= LEVEL_IMMORTAL)
		return ROOM_START_IMMORTAL;
	if (IS_RACE(ch, "HUMAN"))
		return ROOM_START_HUMAN;
	if (IS_RACE(ch, "WOOKIEE"))
		return ROOM_START_WOOKIEE;
	if (IS_RACE(ch, "RODIAN"))
		return ROOM_START_RODIAN;
	if (IS_RACE(ch, "MON CALAMARI"))
		return ROOM_START_MON_CALAMARIAN;
	if (IS_RACE(ch, "TWI'LEK"))
		return ROOM_START_TWILEK;
	if (IS_RACE(ch, "HUTT"))
		return ROOM_START_HUTT;
	if (IS_RACE(ch, "GAMORREAN"))
		return ROOM_START_GAMORREAN;
	if (IS_RACE(ch, "JAWA"))
		return ROOM_START_JAWA;
	if (IS_RACE(ch, "ADARIAN"))
		return ROOM_START_ADARIAN;
	if (IS_RACE(ch, "EWOK"))
		return ROOM_START_EWOK;
	if (IS_RACE(ch, "VERPINE"))
		return ROOM_START_VERPINE;
	if (IS_RACE(ch, "DEFEL"))
		return ROOM_START_DEFEL;
	if (IS_RACE(ch, "TRANDOSHAN"))
		return ROOM_START_TRANDOSHAN;
	if (IS_RACE(ch, "CHADRA-FAN"))
		return ROOM_START_CHADRA_FAN;
	if (IS_RACE(ch, "QUARREN"))
		return ROOM_START_QUARREN;
	if (IS_RACE(ch, "DUINUOGWUIN"))
		return ROOM_START_DUINUOGWUIN;
	if (IS_RACE(ch, "NOGHRI"))
		return ROOM_START_NOGHRI;

	return ROOM_VNUM_TEMPLE;
}

char* grab_word(char *argument, char *arg_first)
{
	char cEnd;
	int count;

	count = 0;

	while (isspace(*argument))
		argument++;

	cEnd = ' ';
	if (*argument == '\'' || *argument == '"')
		cEnd = *argument++;

	while (*argument != '\0' || ++count >= 255)
	{
		if (*argument == cEnd)
		{
			argument++;
			break;
		}
		*arg_first++ = *argument++;
	}
	*arg_first = '\0';

	while (isspace(*argument))
		argument++;

	return argument;
}

char* wordwrap(char *txt, int wrap)
{
	static char buf[MSL];
	char *bufp;

	buf[0] = '\0';
	bufp = buf;
	if (txt != NULL)
	{
		char line[MSL];
		char temp[MSL];
		char *ptr, *p;
		int ln, x;

		++bufp;
		line[0] = '\0';
		ptr = txt;
		while (*ptr)
		{
			ptr = grab_word(ptr, temp);
			ln = strlen(line);
			x = strlen(temp);
			if ((ln + x + 1) < wrap)
			{
				if (line[ln - 1] == '.')
					strcat(line, "  ");
				else
					strcat(line, " ");
				strcat(line, temp);
				p = strchr(line, '\n');
				if (!p)
					p = strchr(line, '\r');
				if (p)
				{
					strcat(buf, line);
					line[0] = '\0';
				}
			}
			else
			{
				strcat(line, "\r\n");
				strcat(buf, line);
				strcpy(line, temp);
			}
		}
		if (line[0] != '\0')
			strcat(buf, line);
	}
	return bufp;
}

void decorate_room(ROOM_INDEX_DATA *room)
{
	char buf[MSL];
	char buf2[MSL];
	int nRand;
	int iRand, len;
	int previous[8];
	int sector = room->sector_type;

	STRDUP(room->name, sect_names[sector][0]);

	buf[0] = '\0';

	nRand = number_range(1, UMIN(8, sent_total[sector]));

	for (iRand = 0; iRand < nRand; iRand++)
		previous[iRand] = -1;

	for (iRand = 0; iRand < nRand; iRand++)
	{
		while (previous[iRand] == -1)
		{
			int x, z;

			x = number_range(0, sent_total[sector] - 1);

			for (z = 0; z < iRand; z++)
				if (previous[z] == x)
					break;

			if (z < iRand)
				continue;

			previous[iRand] = x;

			len = strlen(buf);
			sprintf(buf2, "%s", room_sents[sector][x]);
			if (len > 5 && buf[len - 1] == '.')
			{
				strcat(buf, "  ");
				buf2[0] = UPPER(buf2[0]);
			}
			else if (len == 0)
				buf2[0] = UPPER(buf2[0]);
			strcat(buf, buf2);
		}
	}
	sprintf(buf2, "%s" NL, wordwrap(buf, 78));
	STRDUP(room->description, buf2);
	STRDUP(room->nightdesc, "");
}

/*
 * Remove any unused virtual rooms				-Thoric
 */
void clear_vrooms()
{
	int hash;

	bug("clear_vrooms used");

	for (hash = 0; hash < 64; hash++)
	{
		vroom_hash[hash].remove_if([&](ROOM_INDEX_DATA* room) {
			if (room->people.empty() && room->contents.empty())
			{
				clean_room(room);
				free_room(room);
				--top_vroom;
				return true;
			}
			return false;
		});
	}
}

/*
 * Function to get the equivelant exit of DIR 0-MAXDIR out of linked list.
 * Made to allow old-style diku-merc exit functions to work.	-Thoric
 */
ED* get_exit( RID *room, int dir)
{
	if (!room)
	{
		bug("NULL room");
		return NULL;
	}

	for (auto* xit : room->exits)
		if (xit->vdir == dir)
			return xit;
	return NULL;
}

/*
 * Function to get an exit, leading the the specified room
 */
ED* get_exit_to( RID *room, int dir, int vnum)
{
	if (!room)
	{
		bug("NULL room");
		return NULL;
	}

	for (auto* xit : room->exits)
		if (xit->vdir == dir && xit->vnum == vnum)
			return xit;
	return NULL;
}

/*
 * Function to get the nth exit of a room			-Thoric
 */
ED* get_exit_num( RID *room, int count)
{
	int cnt;

	if (!room)
	{
		bug("NULL room");
		return NULL;
	}

	cnt = 0; for (auto* xit : room->exits)
		if (++cnt == count)
			return xit;
	return NULL;
}

/*
 * Modify movement due to encumbrance				-Thoric
 */
int encumbrance(CHAR_DATA *ch, int move)
{
	int cur, max;

	max = can_carry_w(ch);
	cur = ch->carry_weight;
	if (cur >= max)
		return move * 4;
	else if (cur >= max * 0.95)
		return move * 3.5;
	else if (cur >= max * 0.90)
		return move * 3;
	else if (cur >= max * 0.85)
		return move * 2.5;
	else if (cur >= max * 0.80)
		return move * 2;
	else if (cur >= max * 0.75)
		return move * 1.5;
	else
		return move;
}

/*
 * Check to see if a character can fall down, checks for looping   -Thoric
 */
bool will_fall(CHAR_DATA *ch, int fall)
{
	if (ch == supermob)
		return false;

	if (!ch->in_room)
	{
		bug("ch: %s, NULL room", ch->name);
		return false;
	}

	if ( IS_SET(ch->in_room->room_flags, ROOM_NOFLOOR) && CAN_GO(ch, DIR_DOWN)
			&& (!IS_AFFECTED(ch, AFF_FLYING) || (ch->mount && !IS_AFFECTED(ch->mount, AFF_FLYING))))
	{
		if (fall > 80)
		{
			bug("Falling (in a loop?) more than 80 rooms: vnum %d", ch->in_room->vnum);
			char_from_room(ch);
			char_to_room(ch, get_room_index(wherehome(ch)));
			fall = 0;
			return true;
		}
		send_to_char( COL_ACTION "Spadasz w d�..." EOL, ch);
		move_char(ch, get_exit(ch->in_room, DIR_DOWN), ++fall);
		return true;
	}
	return false;
}

/*
 * create a 'virtual' room					-Thoric
 */
RID* generate_exit( RID *in_room, ED **pexit)
{
	ED *xit;
	ED *bxit;
	ED *orig_exit = (ED*) *pexit;
	RID *room;
	RID *backroom;
	int brvnum;
	int serial;
	int roomnum;
	int distance = -1;
	int vdir = orig_exit->vdir;
	int hash;
	bool found = false;

	if (in_room->vnum > MAX_VNUM) /* room is virtual */
	{
		serial = in_room->vnum;
		roomnum = in_room->tele_vnum;
		if ((serial & 65535) == orig_exit->vnum)
		{
			brvnum = serial >> 16;
			--roomnum;
			distance = roomnum;
		}
		else
		{
			brvnum = serial & 65535;
			++roomnum;
			distance = orig_exit->distance - 1;
		}
		backroom = get_room_index(brvnum);
	}
	else
	{
		int r1 = in_room->vnum;
		int r2 = orig_exit->vnum;

		brvnum = r1;
		backroom = in_room;
		serial = (UMAX( r1, r2 ) << 16) | UMIN(r1, r2);
		distance = orig_exit->distance - 1;
		roomnum = r1 < r2 ? 1 : distance;
	}
	hash = serial % 64;

	for (auto* r : vroom_hash[hash])
		if (r->vnum == serial && r->tele_vnum == roomnum)
		{
			room = r;
			found = true;
			break;
		}
	if (!found)
	{
		CREATE(room, ROOM_INDEX_DATA, 1);
		room->area = in_room->area;
		room->vnum = serial;
		room->tele_vnum = roomnum;
		room->sector_type = in_room->sector_type;

		room->room_flags = in_room->room_flags;
		/* Added by Thanos - lepiej, �eby vroom nie by� plr_home */
		/* Kto by chcia� mieszka� w pokoju, kt�ry istnieje tylko CZASEM ;) */
		REMOVE_BIT(room->room_flags, ROOM_PLR_HOME);

		decorate_room(room);
		vroom_hash[hash].push_front(room);
		++top_vroom;
	}
	if (!found || (xit = get_exit(room, vdir)) == NULL)
	{
		xit = make_exit(room, orig_exit->to_room, vdir);
		STRDUP(xit->keyword, "");
		STRDUP(xit->description, "");
		xit->key = -1;
		xit->distance = distance;
	}
	if (!found)
	{
		bxit = make_exit(room, backroom, rev_dir[vdir]);
		STRDUP(bxit->keyword, "");
		STRDUP(bxit->description, "");
		bxit->key = -1;
		if ((serial & 65535) != orig_exit->vnum)
			bxit->distance = roomnum;
		else
		{
			ED *tmp = get_exit(backroom, vdir);
			int fulldist = tmp->distance;
			bxit->distance = fulldist - distance;
		}
	}
	*pexit = xit;
	return room;
}

ch_ret move_char(CHAR_DATA *ch, EXIT_DATA *pexit, int fall)
{
	RID *in_room;
	RID *to_room;
	RID *from_room;
	char buf[MSL];
	const char *txt;
	ch_ret retcode;
	int door, distance;
	bool drunk = false;
	bool brief = false;

	if (!IS_NPC(ch))
		if ( IS_DRUNK( ch, 2 ) && (ch->position != POS_SHOVE) && (ch->position != POS_DRAG))
			drunk = true;

	if (drunk && !fall)
	{
		door = number_door();
		pexit = get_exit(ch->in_room, door);
	}

#ifdef DEBUG
	if ( pexit )
	{
		sprintf( buf, "move_char: %s to door %d", ch->name, pexit->vdir );
		log_string( buf );
	}
#endif

	retcode = rNONE;
	txt = NULL;

	if ( IS_NPC(ch) && IS_SET(ch->act, ACT_MOUNTED))
		return retcode;

	in_room = ch->in_room;
	from_room = in_room;
	if (!pexit || (to_room = pexit->to_room) == NULL || !CAN_ENTER(ch, pexit->to_room))
	{
		if (drunk)
			send_to_char("Au�! Walisz g�ow� w �cian�. Nic dziwnego, tam nie ma drzwi." NL, ch);
		else
			send_to_char("Niestety nie mo�esz p�j�� w tym kierunku." NL, ch);
		return rNONE;
	}

	door = pexit->vdir;
	distance = pexit->distance;

	/*
	 * Exit is only a "window", there is no way to travel in that direction
	 * unless it's a door with a window in it		-Thoric
	 */
	if ( IS_SET( pexit->flags, EX_WINDOW ) && !IS_SET(pexit->flags, EX_ISDOOR))
	{
		send_to_char("Niestety nie mo�esz p�j�� w tym kierunku." NL, ch);
		return rNONE;
	}

	if ( IS_SET(pexit->flags, EX_PORTAL) && IS_NPC(ch))
	{
		act( PLAIN, "Moby nie mog� wchodzi� w portale.", ch, NULL, NULL, TO_CHAR);
		return rNONE;
	}

	if ( IS_SET(pexit->flags, EX_NOMOB) && IS_NPC(ch))
	{
		act( PLAIN, "Tam nie mo�esz wej��.", ch, NULL, NULL, TO_CHAR);
		return rNONE;
	}

	if ( IS_SET(pexit->flags, EX_CLOSED) && (!IS_AFFECTED(ch, AFF_PASS_DOOR) || IS_SET(pexit->flags, EX_NOPASSDOOR)))
	{
		if (!IS_SET(pexit->flags, EX_SECRET) && !IS_SET(pexit->flags, EX_DIG))
		{
			if (drunk)
			{
				act( PLAIN, "$n wali w $d z ca�ej si�y, ale nie mo�e ich przenikn��.", ch, NULL, pexit->keyword, TO_ROOM);
				act( PLAIN, "Walisz w $d z ca�ej si�y, ale nie mo�esz ich przenikn��.", ch, NULL, pexit->keyword, TO_CHAR);
			}
			else
				act( PLAIN, "Te $d s� zamkni�te.", ch, NULL, pexit->keyword, TO_CHAR);
		}
		else
		{
			if (drunk)
				send_to_char("Au�! Przez ten pijacki stan walisz si� g�ow� w �cian�." NL, ch);
			else
				send_to_char("Nie mo�esz p�j�� w tym kierunku." NL, ch);
		}

		/*Added by Thanos - Holylight umo�liwia prze�a�enie przez drzwi*/
		if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT)))
			send_to_char( EOL FB_WHITE "Passing throught closed door !!!" EOL NL, ch);
		else
			return rNONE;
	}

	/*
	 * Crazy virtual room idea, created upon demand.		-Thoric
	 */

	/*
	 * That's really crazy 					-Thanos
	 * Spr�bujmy powstrzyma� mobki przed generowaniem pokoi wirtualnych
	 * Zlitujmy si� nad RAMem
	 *
	 Wersja mo�e i bardziej chamska ale bezpieczniejsza:

	 if ( distance > 1 )
	 {
	 if( IS_NPC( ch ) )
	 distance  = 1;
	 else if ( (to_room=generate_exit(in_room, &pexit)) == NULL )
	 send_to_char( "Niestety nie mo�esz p�j�� w tamtym kierunku." NL, ch );
	 }
	 zamiast:
	 */

	/* Ale narazie niech zostanie tak: */
	if (distance > 1)
		if ((to_room = generate_exit(in_room, &pexit)) == NULL)
			send_to_char("Niestety nie mo�esz p�j�� w tamtym kierunku." NL, ch);

	if (!fall && IS_AFFECTED(ch, AFF_CHARM) && ch->master && in_room == ch->master->in_room)
	{
		send_to_char("Co? Chesz zostawi� swojego pana i dobroczy�c�?" NL, ch);
		return rNONE;
	}

	if (room_is_private(ch, to_room) && !is_invited(ch, to_room) && !IS_ADMIN(ch->name))
	{
		send_to_char("To miejsce jest obecnie prywatne." NL, ch);
		return rNONE;
	}

	if (!fall && !IS_NPC(ch))
	{
		int move;

		/* Prevent deadlies from entering a nopkill-flagged area from a
		 non-flagged area, but allow them to move around if already
		 inside a nopkill area. - Blodkai
		 */

		if (in_room->sector_type == SECT_AIR || to_room->sector_type == SECT_AIR || IS_SET(pexit->flags, EX_FLY))
		{
			if (ch->mount && !IS_AFFECTED(ch->mount, AFF_FLYING))
			{
				send_to_char("Tw�j pojazd nie umie lata�." NL, ch);
				return rNONE;
			}
			if (!ch->mount && !IS_AFFECTED(ch, AFF_FLYING))
			{
				send_to_char("Musisz umie� lata� by si� tam dosta�." NL, ch);
				return rNONE;
			}
		}

		if (in_room->sector_type == SECT_WATER_NOSWIM || to_room->sector_type == SECT_WATER_NOSWIM)
		{
			bool found;

			found = false;
			if (ch->mount)
			{
				if ( IS_AFFECTED( ch->mount, AFF_FLYING ) || IS_AFFECTED(ch->mount, AFF_FLOATING))
					found = true;
			}
			else if ( IS_AFFECTED(ch, AFF_FLYING) || IS_AFFECTED(ch, AFF_FLOATING))
				found = true;

			/*
			 * Look for a boat.
			 */
			if (!found)
				for (auto* obj : ch->carrying)
				{
					if (obj->item_type == ITEM_BOAT)
					{
						found = true;
						if (drunk)
							txt = "z impetem wpada na brzeg";
						else
							txt = "przybija";
						break;
					}
				}

			if (!found)
			{
				send_to_char("Potrzebujesz �odzi by si� tam dosta�." NL, ch);
				return rNONE;
			}
		}

		if (IS_SET(pexit->flags, EX_CLIMB))
		{
			bool found;

			found = false;
			if (ch->mount && IS_AFFECTED(ch->mount, AFF_FLYING))
				found = true;
			else if (IS_AFFECTED(ch, AFF_FLYING))
				found = true;

			if (!found && !ch->mount)
			{
				if ((!IS_NPC(ch) && number_percent() > ch->pcdata->learned[gsn_climb]) || drunk || ch->mental_state < -90)
				{
					send_to_char("Zaczynasz si� wspina�... ale tracisz uchwyt i spadasz!" NL, ch);
					learn_from_failure(ch, gsn_climb);
					if (pexit->vdir == DIR_DOWN)
					{
						retcode = move_char(ch, pexit, 1);
						return retcode;
					}
					send_to_char( COL_HURT, ch);
					send_to_char("OOouch! Walisz w ziemi�!" EOL, ch);
					WAIT_STATE(ch, 20);
					retcode = damage(ch, ch, (pexit->vdir == DIR_UP ? 10 : 5),
					TYPE_UNDEFINED);
					return retcode;
				}
				found = true;
				learn_from_success(ch, gsn_climb);
				WAIT_STATE(ch, skill_table[gsn_climb]->beats);
				txt = "wspina si�";
			}

			if (!found)
			{
				send_to_char("Nie umiesz si� wspina�." NL, ch);
				return rNONE;
			}
		}

		if (ch->mount)
		{
			switch (ch->mount->position)
			{
			case POS_DEAD:
				send_to_char("Tw�j pojazd nie �yje!" NL, ch);
				return rNONE;
				break;
			case POS_MORTAL:
			case POS_INCAP:
				send_to_char("Tw�j pojazd jest zbyt ranny by si� poruszy�." NL, ch);
				return rNONE;
				break;
			case POS_STUNNED:
				send_to_char("Tw�j pojazd jest zbyt og�uszony by si� poruszy�." NL, ch);
				return rNONE;
				break;
			case POS_SLEEPING:
				send_to_char("Tw�j pojazd �pi." NL, ch);
				return rNONE;
				break;

			case POS_RESTING:
				send_to_char("Tw�j pojazd w�a�nie sobie odpoczywa." NL, ch);
				return rNONE;
				break;
			case POS_SITTING:
				send_to_char("Tw�j pojazd siedzi sobie w najlepsze." NL, ch);
				return rNONE;
				break;
			default:
				break;
			}

			if (!IS_AFFECTED(ch->mount, AFF_FLYING) && !IS_AFFECTED(ch->mount, AFF_FLOATING))
				move = movement_loss[UMIN(SECT_MAX - 1, in_room->sector_type)];
			else
				move = 1;
			if (ch->mount->move < move)
			{
				send_to_char("Tw�j pojazd jest zbyt wyczerpany." NL, ch);
				return rNONE;
			}
		}
		else
		{
			if (!IS_AFFECTED(ch, AFF_FLYING) && !IS_AFFECTED(ch, AFF_FLOATING))
				move = encumbrance(ch, movement_loss[UMIN(SECT_MAX - 1, in_room->sector_type)]);
			else
				move = 1;
			if (ch->move < move)
			{
				ch_printf(ch, "Jeste� zbyt wyczerpan%s." NL, SEX_SUFFIX_YAE(ch));
				return rNONE;
			}
		}

		WAIT_STATE(ch, move);
		if (ch->mount)
			ch->mount->move -= move;
		else
			ch->move -= move;
	}

	/*
	 * Check if player can fit in the room
	 */
	if (to_room->tunnel > 0 && !IS_SET(ch->act, PLR_HOLYLIGHT))
	{
		int count = ch->mount ? 1 : 0;

		for (auto* ctmp : to_room->people)
		{
			if (!IS_IMMORTAL(ctmp))
				++count;
			if (count >= to_room->tunnel)
			{
				if (ch->mount && count == to_room->tunnel)
					send_to_char("Albo ty, albo pojazd." NL, ch);
				else
					send_to_char("Nie mie�cisz si�." NL, ch);
				return rNONE;
			}
		}
	}

	/* check for traps on exit - later */

	if (!IS_AFFECTED(ch, AFF_SNEAK) && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
	{
		if (fall)
			txt = "spada";
		else if (!txt)
		{
			if (ch->mount)
			{
				if (ch->in_room->sector_type == SECT_WATER_SWIM || ch->in_room->sector_type == SECT_WATER_NOSWIM
						|| ch->in_room->sector_type == SECT_UNDERWATER || ch->in_room->sector_type == SECT_OCEANFLOOR)
					txt = (char*) (drunk ? "ledwo odp�ywa" : "odp�ywa");
				else if (IS_AFFECTED(ch->mount, AFF_FLOATING))
					txt = "szybuje";
				else if (IS_AFFECTED(ch->mount, AFF_FLYING))
					txt = "odlatuje";
				else
					txt = "odje�d�a";
			}
			else
			{
				if (ch->in_room->sector_type == SECT_WATER_SWIM || ch->in_room->sector_type == SECT_WATER_NOSWIM
						|| ch->in_room->sector_type == SECT_UNDERWATER || ch->in_room->sector_type == SECT_OCEANFLOOR)
					txt = (char*) (drunk ? "ledwo odp�ywa" : "odp�ywa");
				else if (IS_AFFECTED(ch, AFF_FLOATING))
				{
					if (drunk)
						txt = "szybuje zakre�laj�c dziwne �uki";
					else
						txt = "szybuje";
				}
				else if (IS_AFFECTED(ch, AFF_FLYING))
				{
					if (drunk)
						txt = "odlatuje dziwnie zmieniaj�c kierunki lotu";
					else
						txt = "odlatuje";
				}
				else if (ch->position == POS_SHOVE)
					txt = "zosta�$o wypchni�t$y";
				else if (ch->position == POS_DRAG)
					txt = "zosta�$o wyrzucon$y";
				else
				{
					if (drunk)
						txt = "kieruje sw�j chwiejny krok";
					else
						txt = "odchodzi";
				}
			}
		}
		if (ch->mount)
		{
			sprintf(buf, "$n %s %s %s na $N$5.", txt, door < 10 ? "na" : "", dir_where_name[door]);
			act( COL_ACTION, buf, ch, NULL, ch->mount, TO_NOTVICT);
		}
		else
		{
			sprintf(buf, "$n %s %s $T.", txt, door < 10 ? "na" : "");
			act( COL_ACTION, buf, ch, NULL, dir_where_name[door], TO_ROOM);
		}
	}

	rprog_leave_trigger(ch);
	if (char_died(ch))
		return global_retcode;

	/* zmieniamy was_in_room TYLKO je�li wychodzimy z lokacji NIEVIRTUALNEJ */
	/* To jest __Konieczne__ do poprawnego zapisu mob�w questowych  -Thanos */
	if (ch->in_room->vnum <= MAX_VNUM)
		ch->was_in_room = ch->in_room;

	char_from_room(ch);
	if (ch->mount)
	{
		rprog_leave_trigger(ch->mount);
		if (char_died(ch))
			return global_retcode;
		if (ch->mount)
		{
			char_from_room(ch->mount);
			char_to_room(ch->mount, to_room);
		}
	}
	char_to_room(ch, to_room);

	if (!IS_AFFECTED(ch, AFF_SNEAK) && ( IS_NPC(ch) || !IS_SET(ch->act, PLR_WIZINVIS)))
	{
		if (fall)
			txt = "spad�$o";
		else if (ch->mount)
		{
			if (IS_AFFECTED(ch->mount, AFF_FLOATING))
				txt = "przyszybowa�$o";
			else if (IS_AFFECTED(ch->mount, AFF_FLYING))
				txt = "przylecia�$o";
			else
				txt = "przyjecha�$o";
		}
		else
		{
			if (IS_AFFECTED(ch, AFF_FLOATING))
			{
				if (drunk)
					txt = "przyszybowa�$o z pijackim impetem";
				else
					txt = "przyszybowa�$o";
			}
			else if (IS_AFFECTED(ch, AFF_FLYING))
			{
				if (drunk)
					txt = "przylecia�$o niemal nie skr�caj�c karku";
				else
					txt = "przylecia�$o";
			}
			else if (ch->position == POS_SHOVE)
				txt = "wpad�$o tutaj";
			else if (ch->position == POS_DRAG)
				txt = "wpad�$o tutaj";
			else
			{
				if (drunk)
					txt = "chwiejnie wgramoli�$o si� tutaj";
				else
					txt = "przyby�$o";
			}
		}

		if (ch->mount)
		{
			sprintf(buf, "$n %s %s na $N$5.", txt, dir_rev_name[door]);
			act( COL_ACTION, buf, ch, NULL, ch->mount, TO_ROOM);
		}
		else
		{
			sprintf(buf, "$n %s %s.", txt, dir_rev_name[door]);
			act( COL_ACTION, buf, ch, NULL, NULL, TO_ROOM);
		}
	}

	if (!IS_IMMORTAL(ch) && !IS_NPC(ch) && ch->in_room->area != to_room->area)
	{
		if (ch->top_level < to_room->area->low_range)
			send_to_char(COL_FORCE "Ten dziwny obszar wywo�uje u ciebie niezrozumia�e uczucia..." EOL, ch);
		else if (ch->top_level > to_room->area->high_range)
			send_to_char(COL_FORCE "Czujesz, �e nie skorzystasz wiele z pobytu tutaj..." EOL, ch);
	}

	if (ch->desc)
		do_look(ch, (char*) "auto");

	if (brief)
		SET_BIT(ch->act, PLR_BRIEF);

	/* BIG ugly looping problem here when the character is mptransed back
	 to the starting room.  To avoid this, check how many chars are in
	 the room at the start and stop processing followers after doing
	 the right number of them.  -- Narn
	 */
	if (!fall)
	{
		int chars = 0, count = 0;

		chars = static_cast<int>(from_room->people.size());

		{ auto snapshot = from_room->people; for (auto* fch : snapshot)
		{
			if (count > chars) break;
			count++;
			if (fch != ch /* loop room bug fix here by Thoric */
			&& fch->master == ch && fch->position == POS_STANDING)
			{
				act( COL_ACTION, "Pod��asz za $N$4.", fch, NULL, ch, TO_CHAR);
				move_char(fch, pexit, 0);
			}
		} }
	}

	if (!ch->in_room->contents.empty())
		retcode = check_room_for_traps(ch, TRAP_ENTER_ROOM);
	if (retcode != rNONE)
		return retcode;

	if (char_died(ch))
		return retcode;

	mprog_entry_trigger(ch);
	if (char_died(ch))
		return retcode;

	/*
	 * Trog: KURWA, skoro oba robia to samo
	 *       to po co 2 razy wywolywac je?
	 */

	/*
	 * Thanos: Ano po to, �eby dzia�a�y i MobProgi i RoomProgi
	 * 	   zdj��em tw�j rem. Nie wspominam ju� o tym, �e
	 *         pierwszy to 'entry' prog a drugi, 'enter'.
	 */
	rprog_enter_trigger(ch);
	if (char_died(ch))
		return retcode;

	mprog_greet_trigger(ch);
	if (char_died(ch))
		return retcode;

	/* Thanos */
	mprog_all_greet_trigger(ch);
	if (char_died(ch))
		return retcode;

	oprog_greet_trigger(ch);
	if (char_died(ch))
		return retcode;

	/*Thanos: questy*/
	quest_trigger(ch, EVENT_BRING_OBJ, NULL, NULL);
	if (char_died(ch))
		return retcode;

	quest_trigger(ch, EVENT_VISIT_MOB, NULL, NULL);
	if (char_died(ch))
		return retcode;

	quest_trigger(ch, EVENT_VISIT_ROOM, NULL, NULL);
	if (char_died(ch))
		return retcode;

	if (!will_fall(ch, fall) && fall > 0)
	{
		if (!IS_AFFECTED(ch, AFF_FLOATING) || (ch->mount && !IS_AFFECTED(ch->mount, AFF_FLOATING)))
		{
			send_to_char( FB_RED "OOoch! Walisz w ziemi�!" EOL, ch);
			WAIT_STATE(ch, 20);
			retcode = damage(ch, ch, 50 * fall, TYPE_UNDEFINED);
		}
		else
			send_to_char( FB_BLUE "Powolutku opadasz na ziemi�..." EOL, ch);
	}

	if ( IS_SET( to_room->room_flags, ROOM_PROTOTYPE ) && (IS_OLCMAN( ch ) || get_trust(ch) > 102 || IS_ADMIN(ch->name)))
		send_to_char( MOD_BOLD "Wchodzisz do pomieszczenia z flag� PROTOTYPE!" EOL, ch);

	fevent_trigger(ch, FE_FIND_ROOM);

	return retcode;
}

DEF_DO_FUN( north )
{
	move_char(ch, get_exit(ch->in_room, DIR_NORTH), 0);
	return;
}

DEF_DO_FUN( east )
{
	move_char(ch, get_exit(ch->in_room, DIR_EAST), 0);
	return;
}

DEF_DO_FUN( south )
{
	move_char(ch, get_exit(ch->in_room, DIR_SOUTH), 0);
	return;
}

DEF_DO_FUN( west )
{
	move_char(ch, get_exit(ch->in_room, DIR_WEST), 0);
	return;
}

DEF_DO_FUN( up )
{
	move_char(ch, get_exit(ch->in_room, DIR_UP), 0);
	return;
}

DEF_DO_FUN( down )
{
	move_char(ch, get_exit(ch->in_room, DIR_DOWN), 0);
	return;
}

DEF_DO_FUN( northeast )
{
	move_char(ch, get_exit(ch->in_room, DIR_NORTHEAST), 0);
	return;
}

DEF_DO_FUN( northwest )
{
	move_char(ch, get_exit(ch->in_room, DIR_NORTHWEST), 0);
	return;
}

DEF_DO_FUN( southeast )
{
	move_char(ch, get_exit(ch->in_room, DIR_SOUTHEAST), 0);
	return;
}

DEF_DO_FUN( southwest )
{
	move_char(ch, get_exit(ch->in_room, DIR_SOUTHWEST), 0);
	return;
}

EXIT_DATA* find_door(CHAR_DATA *ch, char *arg, bool quiet)
{
	EXIT_DATA *pexit;
	int door;

	if (arg == NULL || !str_cmp(arg, ""))
		return NULL;

	pexit = NULL;
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
	{
		for (auto* pexit : ch->in_room->exits)
		{
			if ((quiet || IS_SET(pexit->flags, EX_ISDOOR)) && pexit->keyword && is_name_prefix(arg, pexit->keyword))
				return pexit;
		}
		if (!quiet)
			act( PLAIN, "Nie widz� tu �adnego $T.", ch, NULL, arg, TO_CHAR);
		return NULL;
	}

	if ((pexit = get_exit(ch->in_room, door)) == NULL)
	{
		if (!quiet)
			act( PLAIN, "Nie widzisz tu �adnego $T.", ch, NULL, arg, TO_CHAR);
		return NULL;
	}

	if (quiet)
		return pexit;

	if (IS_SET(pexit->flags, EX_SECRET))
	{
		act( PLAIN, "Nie widzisz tu �adnego $T.", ch, NULL, arg, TO_CHAR);
		return NULL;
	}

	if (!IS_SET(pexit->flags, EX_ISDOOR))
	{
		send_to_char("Nie mo�esz tego zrobi�." NL, ch);
		return NULL;
	}

	return pexit;
}

void toggle_bexit_flag(EXIT_DATA *pexit, int flag)
{
	EXIT_DATA *pexit_rev;

	TOGGLE_BIT(pexit->flags, flag);
	if ((pexit_rev = pexit->rexit) != NULL && pexit_rev != pexit)
		TOGGLE_BIT(pexit_rev->flags, flag);
}

void set_bexit_flag(EXIT_DATA *pexit, int flag)
{
	EXIT_DATA *pexit_rev;

	SET_BIT(pexit->flags, flag);
	if ((pexit_rev = pexit->rexit) != NULL && pexit_rev != pexit)
		SET_BIT(pexit_rev->flags, flag);
}

void remove_bexit_flag(EXIT_DATA *pexit, int flag)
{
	EXIT_DATA *pexit_rev;

	REMOVE_BIT(pexit->flags, flag);
	if ((pexit_rev = pexit->rexit) != NULL && pexit_rev != pexit)
		REMOVE_BIT(pexit_rev->flags, flag);
}

DEF_DO_FUN( open )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	EXIT_DATA *pexit;
	int door;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		do_openhatch(ch, (char*) "");
		return;
	}

	if ((pexit = find_door(ch, arg, true)) != NULL)
	{
		/* 'open door' */
		EXIT_DATA *pexit_rev;

		if (!IS_SET(pexit->flags, EX_ISDOOR))
		{
			send_to_char("Nie mo�esz tego zrobi�." NL, ch);
			return;
		}
		if (!IS_SET(pexit->flags, EX_CLOSED))
		{
			send_to_char("To jest ju� otwarte." NL, ch);
			return;
		}
		if (IS_SET(pexit->flags, EX_LOCKED))
		{
			send_to_char("To jest zamkni�te na klucz." NL, ch);
			return;
		}

		if (!IS_SET(pexit->flags, EX_SECRET) || (pexit->keyword && nifty_is_name(arg, pexit->keyword)))
		{
			act( COL_ACTION, "$n otwiera $d.", ch, NULL, pexit->keyword, TO_ROOM);
			act( COL_ACTION, "Otwierasz $d.", ch, NULL, pexit->keyword, TO_CHAR);
			if ((pexit_rev = pexit->rexit) != NULL && pexit_rev->to_room == ch->in_room)
			{
				for (auto* rch : pexit->to_room->people)
					act( COL_ACTION, "$d otwieraj� si�.", rch, NULL, pexit_rev->keyword, TO_CHAR);
				sound_to_room(pexit->to_room, "!!SOUND(door)");
			}
			remove_bexit_flag(pexit, EX_CLOSED);
			if ((door = pexit->vdir) >= 0 && door < 10)
				check_room_for_traps(ch, trap_door[door]);

			sound_to_room(ch->in_room, "!!SOUND(door)");
			return;
		}
	}

	if ((obj = get_obj_here(ch, arg)) != NULL)
	{
		/* 'open object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			ch_printf(ch, "%s nie jest pojemnikiem." NL, capitalize(obj->przypadki[0]));
			return;
		}
		if (!IS_SET(obj->value[1], CONT_CLOSED))
		{
			ch_printf(ch, "%s jest ju� otwarte." NL, capitalize(obj->przypadki[0]));
			return;
		}
		if (!IS_SET(obj->value[1], CONT_CLOSEABLE))
		{
			ch_printf(ch, "%s nie mo�e by� zamkni�te ani otwarte." NL, capitalize(obj->przypadki[0]));
			return;
		}
		if (IS_SET(obj->value[1], CONT_LOCKED))
		{
			ch_printf(ch, "%s jest zamkni�te na klucz." NL, capitalize(obj->przypadki[0]));
			return;
		}

		REMOVE_BIT(obj->value[1], CONT_CLOSED);
		act( COL_ACTION, "Otwierasz $p$3.", ch, obj, NULL, TO_CHAR);
		act( COL_ACTION, "$n otwiera $p$3.", ch, obj, NULL, TO_ROOM);
		check_for_trap(ch, obj, TRAP_OPEN);
		return;
	}

	if (!str_cmp(arg, "hatch"))
		;
	{
		do_openhatch(ch, argument);
		return;
	}

	do_openhatch(ch, arg);
	return;
}

DEF_DO_FUN( close )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	EXIT_DATA *pexit;
	int door;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		do_closehatch(ch, (char*) "");
		return;
	}

	if ((pexit = find_door(ch, arg, true)) != NULL)
	{
		/* 'close door' */
		EXIT_DATA *pexit_rev;

		if (!IS_SET(pexit->flags, EX_ISDOOR))
		{
			send_to_char("Nie mo�esz tego zrobi�." NL, ch);
			return;
		}
		if (IS_SET(pexit->flags, EX_CLOSED))
		{
			send_to_char("To jest ju� zamkni�te." NL, ch);
			return;
		}

		act( COL_ACTION, "$n zamyka $d.", ch, NULL, pexit->keyword, TO_ROOM);
		act( COL_ACTION, "Zamykasz $d.", ch, NULL, pexit->keyword, TO_CHAR);

		/* close the other side */
		if ((pexit_rev = pexit->rexit) != NULL && pexit_rev->to_room == ch->in_room)
		{
			SET_BIT(pexit_rev->flags, EX_CLOSED);
			for (auto* rch : pexit->to_room->people)
				act( COL_ACTION, "$d zamykaj� si�.", rch, NULL, pexit_rev->keyword, TO_CHAR);
		}
		set_bexit_flag(pexit, EX_CLOSED);
		if ((door = pexit->vdir) >= 0 && door < 10)
			check_room_for_traps(ch, trap_door[door]);
		return;
	}

	if ((obj = get_obj_here(ch, arg)) != NULL)
	{
		/* 'close object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			ch_printf(ch, "%s nie jest pojemnikiem." NL, capitalize(obj->przypadki[0]));
			return;
		}
		if (IS_SET(obj->value[1], CONT_CLOSED))
		{
			ch_printf(ch, "%s jest ju� zamkni�t%s." NL, capitalize(obj->przypadki[0]), OSEX_SUFFIX_YAEE(obj));
			return;
		}
		if (!IS_SET(obj->value[1], CONT_CLOSEABLE))
		{
			ch_printf(ch, "%s nie mo�na zamkn�� ani otworzy�." NL, capitalize(obj->przypadki[1]));
			return;
		}

		SET_BIT(obj->value[1], CONT_CLOSED);
		act( COL_ACTION, "Zamykasz $p$3.", ch, obj, NULL, TO_CHAR);
		act( COL_ACTION, "$n zamyka $p$3.", ch, obj, NULL, TO_ROOM);
		check_for_trap(ch, obj, TRAP_CLOSE);
		return;
	}

	if (!str_cmp(arg, "hatch"))
	{
		do_closehatch(ch, argument);
		return;
	}

	do_closehatch(ch, arg);
	return;
}

bool has_key(CHAR_DATA *ch, int key)
{
	if (key == 0)
		return true;
	for (auto* obj : ch->carrying)
		if (obj->pIndexData->vnum == key || obj->value[0] == key)
			return true;

	return false;
}

DEF_DO_FUN( lock )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	EXIT_DATA *pexit;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Zakluczy� co?" NL, ch);
		return;
	}

	if ((pexit = find_door(ch, arg, true)) != NULL)
	{
		/* 'lock door' */

		if (!IS_SET(pexit->flags, EX_ISDOOR))
		{
			send_to_char("Nie mo�esz tego zrobi�." NL, ch);
			return;
		}
		if (!IS_SET(pexit->flags, EX_CLOSED))
		{
			send_to_char("To nie jest zamkni�te." NL, ch);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char("To nie mo�e zosta� zakluczone." NL, ch);
			return;
		}
		if (!has_key(ch, pexit->key))
		{
			send_to_char("Nie masz klucza." NL, ch);
			return;
		}
		if (IS_SET(pexit->flags, EX_LOCKED))
		{
			send_to_char("To jest ju� zakluczone." NL, ch);
			return;
		}

		if (!IS_SET(pexit->flags, EX_SECRET) || (pexit->keyword && nifty_is_name(arg, pexit->keyword)))
		{
			send_to_char("*Click*" NL, ch);
			act( COL_ACTION, "$n zaklucza $d.", ch, NULL, pexit->keyword, TO_ROOM);
			set_bexit_flag(pexit, EX_LOCKED);
			return;
		}
	}

	if ((obj = get_obj_here(ch, arg)) != NULL)
	{
		/* 'lock object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char("To nie jest pojemnik." NL, ch);
			return;
		}
		if (!IS_SET(obj->value[1], CONT_CLOSED))
		{
			send_to_char("To nie jest zamkni�te." NL, ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char("To nie mo�e by� zakluczone." NL, ch);
			return;
		}
		if (!has_key(ch, obj->value[2]))
		{
			send_to_char("Nie masz klucza." NL, ch);
			return;
		}
		if (IS_SET(obj->value[1], CONT_LOCKED))
		{
			send_to_char("To jest ju� zakluczone." NL, ch);
			return;
		}

		SET_BIT(obj->value[1], CONT_LOCKED);
		send_to_char("*Click*" NL, ch);
		act( COL_ACTION, "$n zaklucza $p$3.", ch, obj, NULL, TO_ROOM);
		return;
	}

	ch_printf(ch, "Nie widzisz tu �adnego %s." NL, arg);
	return;
}

DEF_DO_FUN( unlock )
{
	char arg[MAX_INPUT_LENGTH];
	OBJ_DATA *obj;
	EXIT_DATA *pexit;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Odkluczy� co?" NL, ch);
		return;
	}

	if ((pexit = find_door(ch, arg, true)) != NULL)
	{
		/* 'unlock door' */

		if (!IS_SET(pexit->flags, EX_ISDOOR))
		{
			send_to_char("Nie mo�esz tego zrobi�." NL, ch);
			return;
		}
		if (!IS_SET(pexit->flags, EX_CLOSED))
		{
			send_to_char("To nie jest zamkni�te." NL, ch);
			return;
		}
		if (pexit->key < 0)
		{
			send_to_char("To nie mo�e by� odkluczone." NL, ch);
			return;
		}
		if (!has_key(ch, pexit->key))
		{
			send_to_char("Nie masz klucza." NL, ch);
			return;
		}
		if (!IS_SET(pexit->flags, EX_LOCKED))
		{
			send_to_char("To jest ju� odkluczone." NL, ch);
			return;
		}

		if (!IS_SET(pexit->flags, EX_SECRET) || (pexit->keyword && nifty_is_name(arg, pexit->keyword)))
		{
			send_to_char("*Click*" NL, ch);
			act( COL_ACTION, "$n odklucza $d.", ch, NULL, pexit->keyword, TO_ROOM);
			remove_bexit_flag(pexit, EX_LOCKED);
			return;
		}
	}

	if ((obj = get_obj_here(ch, arg)) != NULL)
	{
		/* 'unlock object' */
		if (obj->item_type != ITEM_CONTAINER)
		{
			send_to_char("To nie jest pojemnik." NL, ch);
			return;
		}
		if (!IS_SET(obj->value[1], CONT_CLOSED))
		{
			send_to_char("To nie jest zamkni�te." NL, ch);
			return;
		}
		if (obj->value[2] < 0)
		{
			send_to_char("To nie mo�e byc odkluczone." NL, ch);
			return;
		}
		if (!has_key(ch, obj->value[2]))
		{
			send_to_char("Nie masz klucza." NL, ch);
			return;
		}
		if (!IS_SET(obj->value[1], CONT_LOCKED))
		{
			send_to_char("To jest ju� odkluczone." NL, ch);
			return;
		}

		REMOVE_BIT(obj->value[1], CONT_LOCKED);
		send_to_char("*Click*" NL, ch);
		act( COL_ACTION, "$n odklucza $p$3.", ch, obj, NULL, TO_ROOM);
		return;
	}

	ch_printf(ch, "Nie widzisz tu �adnego %s." NL, arg);
	return;
}

DEF_DO_FUN( bashdoor )
{
	EXIT_DATA *pexit;
	char arg[ MAX_INPUT_LENGTH];

	if (!IS_NPC(ch) && ch->pcdata->learned[gsn_bashdoor] <= 0)
	{
		send_to_char("Za ma�o w tobie wojownika by to zrobi�!" NL, ch);
		return;
	}

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Rozwali� kt�re drzwi?" NL, ch);
		return;
	}

	if (ch->fighting)
	{
		send_to_char("Nie mo�esz przerwa� walki." NL, ch);
		return;
	}

	if ((pexit = find_door(ch, arg, false)) != NULL)
	{
		ROOM_INDEX_DATA *to_room;
		EXIT_DATA *pexit_rev;
		int chance;
		const char *keyword;

		if (!IS_SET(pexit->flags, EX_CLOSED))
		{
			send_to_char("Spokojnie. To jest ju� otwarte." NL, ch);
			return;
		}

		WAIT_STATE(ch, skill_table[gsn_bashdoor]->beats);

		if (IS_SET(pexit->flags, EX_SECRET))
			keyword = "�cian�";
		else
			keyword = pexit->keyword;
		if (!IS_NPC(ch))
			chance = ch->pcdata->learned[gsn_bashdoor] / 2;
		else
			chance = 90;

		if (!IS_SET(pexit->flags, EX_BASHPROOF) && ch->move >= 15 && number_percent() < (chance + 4 * (get_curr_str(ch) - 19)))
		{
			REMOVE_BIT(pexit->flags, EX_CLOSED);
			if (IS_SET(pexit->flags, EX_LOCKED))
				REMOVE_BIT(pexit->flags, EX_LOCKED);
			SET_BIT(pexit->flags, EX_BASHED);

			act(COL_ACTION, "Baaam!  Rozwalasz z impetem $d!", ch, NULL, keyword, TO_CHAR);
			act(COL_ACTION, "$n rozwala z impetem $d!", ch, NULL, keyword, TO_ROOM);
			learn_from_success(ch, gsn_bashdoor);

			if ((to_room = pexit->to_room) != NULL && (pexit_rev = pexit->rexit) != NULL && pexit_rev->to_room == ch->in_room)
			{
				REMOVE_BIT(pexit_rev->flags, EX_CLOSED);
				if (IS_SET(pexit_rev->flags, EX_LOCKED))
					REMOVE_BIT(pexit_rev->flags, EX_LOCKED);
				SET_BIT(pexit_rev->flags, EX_BASHED);

				for (auto* rch : to_room->people)
				{
					act(COL_ACTION, "Kto� rozwala $d!", rch, NULL, pexit_rev->keyword, TO_CHAR);
				}
			}
			damage(ch, ch, (ch->max_hit / 20), gsn_bashdoor);
		}
		else
		{
			act(COL_ACTION, "WHAAAAM!!!  Walisz z impetem w $d, ale nie udaje ci si� tego rozwali�.", ch, NULL, keyword, TO_CHAR);
			act(COL_ACTION, "WHAAAAM!!!  $n wali z impetem w $d, ale nie udaje $m si� tego rozwali�.", ch, NULL, keyword, TO_ROOM);
			damage(ch, ch, (ch->max_hit / 20) + 10, gsn_bashdoor);
			learn_from_failure(ch, gsn_bashdoor);
		}
	}
	else
	{
		act(COL_ACTION, "WHAAAAM!!!  Walisz z impetem w �cian�, ale nie udaje ci si� tego rozwali�.", ch, NULL, NULL, TO_CHAR);
		act(COL_ACTION, "WHAAAAM!!!  $n wali z impetem w $d, ale nie udaje $m si� tego rozwali�.", ch, NULL, NULL, TO_ROOM);
		damage(ch, ch, (ch->max_hit / 20) + 10, gsn_bashdoor);
		learn_from_failure(ch, gsn_bashdoor);
	}
	if (!char_died(ch))
		for (auto* gch : ch->in_room->people)
		{
			if ( IS_AWAKE(gch) && !gch->fighting && ( IS_NPC( gch ) && !IS_AFFECTED(gch, AFF_CHARM))
					&& (ch->top_level - gch->top_level <= 4) && number_bits(2) == 0)
				multi_hit(gch, ch, TYPE_UNDEFINED);
		}

	return;
}

DEF_DO_FUN( stand )
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		if (IS_AFFECTED(ch, AFF_SLEEP))
		{
			send_to_char("Jako� nie mo�esz si� obudzi�. ZZzzz.. " NL, ch);
			return;
		}
		send_to_char("Budzisz si� i szybko stajesz na nogach." NL, ch);
		act( COL_ACTION, "$n budzi si� ze snu.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_STANDING;
		break;
	case POS_RESTING:
		send_to_char("Podnosisz si� i wstajesz." NL, ch);
		act( COL_ACTION, "$n podnosi si� i wstaje.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_STANDING;
		break;
	case POS_SITTING:
		send_to_char("Szybko podnosisz si� i wstajesz." NL, ch);
		act( COL_ACTION, "$n wstaje.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_STANDING;
		break;
	case POS_STANDING:
		send_to_char("Ju� stoisz." NL, ch);
		break;
	case POS_FIGHTING:
		send_to_char("W�a�nie walczysz!" NL, ch);
		break;
	}

	return;
}

DEF_DO_FUN( sit )
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		if (IS_AFFECTED(ch, AFF_SLEEP))
		{
			send_to_char("Jako� nie mo�esz si� obudzi�. ZZzzz.." NL, ch);
			return;
		}
		send_to_char("Budzisz si� i siadasz." NL, ch);
		act( COL_ACTION, "$n budzi si� i siada sobie wygodnie.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_SITTING;
		break;

	case POS_RESTING:
		send_to_char("Przestajesz odpoczywa� i siadasz." NL, ch);
		act( COL_ACTION, "$n przestaje odpoczywac i siada sobie wygodnie.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_SITTING;
		break;
	case POS_STANDING:
		send_to_char("Siadasz sobie." NL, ch);
		act( COL_ACTION, "$n siada sobie wygodnie.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_SITTING;
		break;
	case POS_SITTING:
		send_to_char("Ju� siedzisz." NL, ch);
		return;
	case POS_FIGHTING:
		ch_printf(ch, "Jeste� teraz zaj�t%s walk�!" NL, SEX_SUFFIX_YAE(ch));
		return;
	case POS_MOUNTED:
		send_to_char("Przecie� ju� siedzisz - na swoim poje�dzie." NL, ch);
		return;
	}
	return;
}

DEF_DO_FUN( rest )
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		if (IS_AFFECTED(ch, AFF_SLEEP))
		{
			send_to_char("Jako� nie mo�esz si� obudzi�. ZZzzz.. " NL, ch);
			return;
		}
		send_to_char("Budzisz si� z drzemki." NL, ch);
		act( COL_ACTION, "$n budzi si� ze swojej drzemki.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_RESTING;
		break;
	case POS_RESTING:
		send_to_char("Przecie� ju� odpoczywasz." NL, ch);
		return;

	case POS_STANDING:
		send_to_char("Rozwalasz si� wygodnie do odpoczynku." NL, ch);
		act( COL_ACTION, "$n rozwala si� wygodnie do odpoczynku.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_RESTING;
		break;
	case POS_SITTING:
		send_to_char("Rozwalasz si� jeszcze wygodniej i zaczynasz odpoczywa�." NL, ch);
		act( COL_ACTION, "$n rozwala si� wygodnie do odpoczynku.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_RESTING;
		break;
	case POS_FIGHTING:
		send_to_char("Chyba masz teraz inne zaj�cie!" NL, ch);
		return;
	case POS_MOUNTED:
		send_to_char("Najpierw lepiej zsi�d� z wierzchowca." NL, ch);
		return;
	}
	rprog_rest_trigger(ch);
	return;
}

DEF_DO_FUN( sleep )
{
	switch (ch->position)
	{
	case POS_SLEEPING:
		send_to_char("Przecie� ju� �pisz. ZZzzz" NL, ch);
		return;

	case POS_RESTING:
		if (ch->mental_state > 30 && (number_percent() + 10) < ch->mental_state)
		{
			send_to_char("Nie mo�esz si� wystarczaj�co uspokoi� by zasn��." NL, ch);
			act( COL_ACTION, "$n zamyka oczy na kilka chwil, ale nie mo�e zasn��.", ch, NULL, NULL, TO_ROOM);
			return;
		}
		send_to_char("Zamykasz oczy i pogr��asz si� we �nie..." NL, ch);
		act( COL_ACTION, "$n zamyka oczy i pogr��a si� w g��bokim �nie.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_SLEEPING;
		break;

	case POS_SITTING:
		if (ch->mental_state > 30 && (number_percent() + 5) < ch->mental_state)
		{
			send_to_char("Nie mo�esz si� wystarczaj�co uspokoi� by zasn��." NL, ch);
			act( COL_ACTION, "$n zamyka oczy na kilka chwil, ale nie mo�e zasn��.", ch, NULL, NULL, TO_ROOM);
			return;
		}
		send_to_char("Padasz na ziemi� i zasypiasz..." NL, ch);
		act( COL_ACTION, "$n pada na ziemi� i zasypia.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_SLEEPING;
		break;

	case POS_STANDING:
		if (ch->mental_state > 30 && number_percent() < ch->mental_state)
		{
			send_to_char("Nie mo�esz si� wystarczaj�co uspokoi� by zasn��.." NL, ch);
			act( COL_ACTION, "$n zamyka oczy na kilka chwil, ale nie mo�e zasn��.", ch, NULL, NULL, TO_ROOM);
			return;
		}
		send_to_char("K�adziesz si� i zasypiasz." NL, ch);
		act( COL_ACTION, "$n k�adzie si� i zasypia.", ch, NULL, NULL, TO_ROOM);
		ch->position = POS_SLEEPING;
		break;

	case POS_FIGHTING:
		send_to_char("Chyba masz teraz co robi�!" NL, ch);
		return;
	case POS_MOUNTED:
		send_to_char("Chyba najpierw zejd� z wierzchowca." NL, ch);
		return;
	}

	rprog_sleep_trigger(ch);
	return;
}

DEF_DO_FUN( wake )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		do_stand(ch, argument);
		return;
	}

	if (!IS_AWAKE(ch))
	{
		send_to_char("Mo�e najpierw ty wsta�!" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	if (IS_AWAKE(victim))
	{
		act( PLAIN, "$N ju� nie �pi.", ch, NULL, victim, TO_CHAR);
		return;
	}

	if ( IS_AFFECTED(victim, AFF_SLEEP) || victim->position < POS_SLEEPING)
	{
		act( PLAIN, "Jako� nie mo�esz $I obudzi�!", ch, NULL, victim, TO_CHAR);
		return;
	}

	act( COL_ACTION, "Budzisz $I.", ch, NULL, victim, TO_CHAR);
	victim->position = POS_STANDING;
	act( COL_ACTION, "$n budzi ci�.", ch, NULL, victim, TO_VICT);
	act( COL_ACTION, "$n budzi $N$3.", ch, NULL, victim, TO_NOTVICT);
	return;
}

/*
 * teleport a character to another room
 */
void teleportch(CHAR_DATA *ch, ROOM_INDEX_DATA *room, bool show)
{
	if (room_is_private(ch, room))
		return;
	act( COL_ACTION, "$n znika nagle!", ch, NULL, NULL, TO_ROOM);
	char_from_room(ch);
	char_to_room(ch, room);
	act( COL_ACTION, "$n pojawia si� nagle!", ch, NULL, NULL, TO_ROOM);
	if (show)
		do_look(ch, (char*) "auto");
}

void teleport(CHAR_DATA *ch, int room, int64 flags)
{
	ROOM_INDEX_DATA *pRoomIndex;
	bool show;

	pRoomIndex = get_room_index(room);

	IF_BUG(pRoomIndex == NULL, "bad room vnum")
		return;

	if (IS_SET(flags, TELE_SHOWDESC))
		show = true;
	else
		show = false;
	if (!IS_SET(flags, TELE_TRANSALL))
	{
		teleportch(ch, pRoomIndex, show);
		return;
	}
	{ auto snapshot = ch->in_room->people; for (auto* nch : snapshot)
	{
		teleportch(nch, pRoomIndex, show);
	} }
}

/*
 * "Climb" in a certain direction.				-Thoric
 */
DEF_DO_FUN( climb )
{
	EXIT_DATA *pexit;

	if (argument[0] == '\0')
	{
		for (auto* pexit : ch->in_room->exits)
			if ( IS_SET( pexit->flags, EX_xCLIMB ) && CAN_ENTER(ch, pexit->to_room))
			{
				move_char(ch, pexit, 0);
				return;
			}
		send_to_char("Nie mo�esz si� tu wspina�." NL, ch);
		return;
	}

	if ((pexit = find_door(ch, argument, true)) != NULL && IS_SET(pexit->flags, EX_xCLIMB) && CAN_ENTER(ch, pexit->to_room))
	{
		move_char(ch, pexit, 0);
		return;
	}
	send_to_char("Nie mo�esz si� tam wspi��." NL, ch);
	return;
}

/*
 * "enter" something (moves through an exit)			-Thoric
 */
DEF_DO_FUN( enter )
{
	EXIT_DATA *pexit;

	if (argument[0] == '\0')
	{
		for (auto* pexit : ch->in_room->exits)
			if ( IS_SET( pexit->flags, EX_xENTER ) && CAN_ENTER(ch, pexit->to_room))
			{
				move_char(ch, pexit, 0);
				return;
			}
		send_to_char("Nie mo�esz nigdzie znale�� wej��ia." NL, ch);
		return;
	}

	if ((pexit = find_door(ch, argument, true)) != NULL && IS_SET(pexit->flags, EX_xENTER))
	{
		move_char(ch, pexit, 0);
		return;
	}

	do_board(ch, argument);
	return;
}

/*
 * Leave through an exit.					-Thoric
 */
DEF_DO_FUN( leave )
{
	EXIT_DATA *pexit;

	if (argument[0] == '\0')
	{
		for (auto* pexit : ch->in_room->exits)
			if (IS_SET(pexit->flags, EX_xLEAVE))
			{
				move_char(ch, pexit, 0);
				return;
			}
		do_leaveship(ch, (char*) "");
		return;
	}

	if ((pexit = find_door(ch, argument, true)) != NULL && IS_SET(pexit->flags, EX_xLEAVE))
	{
		move_char(ch, pexit, 0);
		return;
	}
	do_leaveship(ch, (char*) "");
	return;
}

DEF_DO_FUN( shove )
{
	EXIT_DATA *pexit;
	CHAR_DATA *victim;
	char arg[MIL];
	char arg2[MIL];
	int exit_dir;
	bool nogo;
	int chance;

	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);

	if (!*arg)
	{
		send_to_char("Wypchn�� kogo?" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Pchasz si� jak w kolejce przed hipermarketem." NL, ch);
		return;
	}

	if ((victim->position) != POS_STANDING)
	{
		act( PLAIN, "$N nie stoi. Jak zamierzasz $I wypchn�� ???", ch, NULL, victim, TO_CHAR);
		return;
	}

	if (!*arg2)
	{
		send_to_char("W kt�r� stron� �yczysz sobie popchn�� delikwenta?" NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Mo�esz pcha� tylko graczy." NL, ch);
//#warning Trog: dlaczego?
// bo tak :P  -- Thanos
		return;
	}

	exit_dir = get_dir(arg2);
	if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE) && get_timer(victim, TIMER_SHOVEDRAG) <= 0)
	{
		send_to_char("Ta osoba nie mo�e by� teraz wypchana. Jest zbyt bezpieczna." NL, ch);
		return;
	}
	victim->position = POS_SHOVE;
	nogo = false;
	if ((pexit = get_exit(ch->in_room, exit_dir)) == NULL)
		nogo = true;
	else if ( IS_SET(pexit->flags, EX_CLOSED) && (!IS_AFFECTED(victim, AFF_PASS_DOOR) || IS_SET(pexit->flags, EX_NOPASSDOOR)))
		nogo = true;

	if (nogo)
	{
		send_to_char("Mi�o, �e chcesz popchn�� w t� stron�. Ale widzisz... Tam nie ma drzwi." NL, ch);
		victim->position = POS_STANDING;
		return;
	}

	chance = 50;

	/* Add 3 points to chance for every str point above 15, subtract for below 15 */

	chance += ((get_curr_str(ch) - 15) * 3);
	chance += (ch->top_level - victim->top_level);

	/* Debugging purposes - show percentage for testing */

	/* sprintf(buf, "Shove percentage of %s = %d", ch->name, chance);
	 act( COL_ACTION, buf, ch, NULL, NULL, TO_ROOM );
	 */

	if (chance < number_percent())
	{
		send_to_char("Nie uda�o ci si�." NL, ch);
		victim->position = POS_STANDING;
		return;
	}

	act( COL_ACTION, "Wypychasz $N$3.", ch, NULL, victim, TO_CHAR);
	act( COL_ACTION, "$n wypycha ci�.", ch, NULL, victim, TO_VICT);
	move_char(victim, get_exit(ch->in_room, exit_dir), 0);
	if (!char_died(victim))
		victim->position = POS_STANDING;
	WAIT_STATE(ch, 12);
	/* Remove protection from shove/drag if char shoves -- Blodkai */
	if ( IS_SET(ch->in_room->room_flags, ROOM_SAFE) && get_timer(ch, TIMER_SHOVEDRAG) <= 0)
		add_timer(ch, TIMER_SHOVEDRAG, 10, NULL, 0);
}

DEF_DO_FUN( drag )
{
	CHAR_DATA *victim;
	EXIT_DATA *pexit;
	char arg[MIL];
	char arg2[MIL];
	int exit_dir;
	bool nogo;
	int chance;

	argument = one_argument(argument, arg);
	argument = one_argument(argument, arg2);

	if (!*arg)
	{
		send_to_char("Kogo chcesz bezlito�nie wyci�gn�� za chabety w celu zapewne wymiany argument�w?" NL, ch);
		return;
	}

	if (!(victim = get_char_room(ch, arg)))
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("�apiesz si� za �eb i ci�gniesz po ca�ej okolicy. Kopnij si� jeszcze! ;-)" NL, ch);
		return;
	}

	if (IS_NPC(victim))
	{
		send_to_char("Mo�esz ci�ga� za �eb tylko graczy." NL, ch);
		/* Trog: a moby mozesz za inne czesci ciala? :) */
		return;
	}

	if (victim->fighting)
	{
		send_to_char("Starasz si� jak mo�esz, ale w ferworze walki nie mo�esz podej�� tak blisko." NL, ch);
		return;
	}

	if (victim->position <= POS_SLEEPING || IS_AFFECTED(victim, AFF_SLEEP))
	{
		send_to_char("Twoja ofiara jest zbyt bezw�adna, wy�lizguje ci si� z r�k." NL, ch);
		return;
	}

	if (!*arg2)
	{
		send_to_char("A gdzie� to chcesz bezlito�nie wyci�gn�� sw� ofiar�?" NL, ch);
		return;
	}

	exit_dir = get_dir(arg2);
	if ( IS_SET(victim->in_room->room_flags, ROOM_SAFE) && get_timer(victim, TIMER_SHOVEDRAG) <= 0)
	{
		send_to_char("Ta osoba jest ju� zbyt bezpieczna." NL, ch);
		return;
	}

	nogo = false;
	if ((pexit = get_exit(ch->in_room, exit_dir)) == NULL)
		nogo = true;
	else if ( IS_SET(pexit->flags, EX_CLOSED) && (!IS_AFFECTED(victim, AFF_PASS_DOOR) || IS_SET(pexit->flags, EX_NOPASSDOOR)))
		nogo = true;

	if (nogo)
	{
		send_to_char("Tam nie ma drzwi! Chcesz rozmaza� sw� ofiar� na �cianie ???" NL, ch);
		return;
	}

	chance = 50;

	/*
	 sprintf(buf, "Drag percentage of %s = %d", ch->name, chance);
	 act( COL_ACTION, buf, ch, NULL, NULL, TO_ROOM );
	 */

	if (chance < number_percent())
	{
		send_to_char("Nie uda�o ci si�." NL, ch);
		victim->position = POS_STANDING;
		return;
	}

	if (victim->position < POS_STANDING)
	{
		int temp;

		temp = victim->position;
		victim->position = POS_DRAG;
		act( COL_ACTION, "�apiesz $N$3 za �eb i wyci�gasz w inne miejsce.", ch, NULL, victim, TO_CHAR);
		act( COL_ACTION, "$n �apie ci� za �eb i wyci�ga w nieznane.", ch, NULL, victim, TO_VICT);
		move_char(victim, get_exit(ch->in_room, exit_dir), 0);
		if (!char_died(victim))
			victim->position = temp;

		/* Move ch to the room too.. they are doing dragging - Scryn */
		move_char(ch, get_exit(ch->in_room, exit_dir), 0);
		WAIT_STATE(ch, 12);
		return;
	}

	send_to_char("Ta osoba stoi, nie dasz rady jej z�apa� za �eb." NL, ch);
}
