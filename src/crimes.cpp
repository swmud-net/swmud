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
*  		           Crimes & Jails module    			   *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "mud.h"


/* przez ile tick�w w�adze planety nie zorientuj� si� o ucieczce */
#define JAIL_ACTIVITY		2

/* Pamietaj jeszcze o:
- depozyty - jak zrobi� by da�o si� wykra��?
- stra�nik ma fizycznie �azi� po celach, a depozyt wyci�ga�
- itd... :)
*/

const int crime_level_price = 400;

/* by Ganis */
int calculate_wanted_price( const CHAR_DATA *ch )
{
	int				price = 0;

	for (auto* crime : ch->crimes)
		price += crime->level * crime_level_price;

	return price;
}

/* by Ganis */
DEF_DO_FUN( corrupt )
{
	CHAR_DATA		* victim;
	int				price;
	char			buf[MSL];
	static SPEC_FUN* const spec_name = spec_lookup("spec_police_fine");

	if (!spec_name)
	{
		bug("spec_police_fine function got lost... Corrupt won't work!");
		send_to_char("Sorry, co� przesta�o dzia�a� w mudzie tak jak powinno :(" NL, ch);
		return;
	}

	if (ch->crimes.empty())
	{
		send_to_char("Przecie� nie masz nic na sumieniu." NL, ch);
		return;
	}

	//Poszukajmy, czy w tym pokoju jest ktos, kogo mozna skorumpowac
	victim = NULL;
	for (auto* xch : ch->in_room->people)
	{

		if (xch->spec_fun == spec_name || xch->spec_2 == spec_name)
		{
			victim = xch;
			break;
		}
	}

	if (!victim)
	{
		send_to_char("Nie ma tu nikogo, kogo mo�na by skorumpowa�." NL, ch);
		return;
	}

	price = calculate_wanted_price(ch);

	if (price > ch->gold)
	{
		sprintf(buf, "Potrzebujesz %d kredytek, aby wymaza� swoj� kryminaln� kartotek�." NL, price);
		send_to_char(buf, ch);
		return;
	}

	ch->gold -= price;
	victim->gold += price;
	sprintf(buf, "Przekazujesz %s %d kredyt�w w celu wymazania swojej kartoteki." NL,
				victim->przypadki[2], price);
	send_to_char(buf,ch);

	for (auto* crime : std::list<CRIME_DATA *>(ch->crimes))
	{
		free_crime(ch, crime);
	}
}

DEF_DO_FUN( bail )
{
	char 		buf 	[MAX_STRING_LENGTH];
	char 		arg1 	[MAX_INPUT_LENGTH];
	char 		arg2 	[MAX_INPUT_LENGTH];
	bool		found;
	int			cost;
	int			amount;
	CHAR_DATA *		victim;
	CRIME_DATA *	crime;
	CHAR_DATA *		mob;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Kogo chcesz wykupi� i za ile?" NL, ch );
		return;
	}

	if( ( victim = get_char_world(ch, arg1) ) == NULL )
	{
		send_to_char("Nie ma nikogo takiego w grze." NL, ch);
		return;
	}

	found = false;
	for ( auto* cr : victim->crimes)
	{
		if( cr->jail_end )
		{
			crime = cr;
			found = true;
			break;
		}
	}

	if ( !found || IS_NPC( victim ) )
	{
		send_to_char("Ta osoba nie odsiaduje �adnego wyroku." NL, ch);
		return;
	}

	found = false;
	for( auto* m : ch->in_room->people )
	{
		if( !IS_NPC( m ) || !can_see( ch, m ) )
			continue;
		if( ( m->spec_fun && m->spec_fun == spec_lookup( "spec_prosecutor" ) )
		||  ( m->spec_2   && m->spec_2   == spec_lookup( "spec_prosecutor" ) ) )
		{
			mob = m;
			found = true;
			break;
		}
	}

	if( !found )
	{
		send_to_char("Nie ma tu nikogo kto m�g�by przyj�� twoj� kaucj�." NL, ch);
		return;
	}

	amount = atoi( arg2 );
	if( amount <= 0 )
	{
		ch_tell( mob, ch, "Za tyle to ja mog� zapuszkowa� ciebie! Nie str�j sobie �art�w." );
		return;
	}

	if( amount > ch->gold )
	{
		ch_tell( mob, ch, "A masz tyle? Wr�� jak uzbierasz!" );
		return;
	}

	cost = 30 * victim->top_level + 100 * crime->level -
		3 * cha_app[ get_curr_cha( ch ) ].charm;

	if( amount < cost/10 )
	{
		cost+=cost/10;
		sprintf( buf,
			"Zwariowa�%s�? Za takie drobniaki nie chce mi si� nawet s�ucha� tego imienia!",
			SEX_SUFFIX_EAE( ch ) );
		ch_tell( mob, ch, buf );
		sprintf( buf,
			"Daj mi %d kredyt%s, a %s wyjdzie na wolno�� jeszcze dzi�.",
			cost, NUMBER_SUFF( cost, "k�", "ki", "ek" ), victim->name );
		ch_tell( mob, ch, buf );
		return;
	}

	if( amount < cost )
	{
		cost+=cost/10;
		ch_tell( mob, ch, "To za ma�o..." );
		sprintf( buf,
			"Daj mi %d kredyt%s, a %s wyjdzie na wolno�� jeszcze dzi�.",
			cost, NUMBER_SUFF( cost, "k�", "ki", "ek" ), victim->name );
		ch_tell( mob, ch, buf );
		return;
	}

	ch->gold -= amount;

	sprintf( buf, "Zgoda.. Uwolni� %s, ale pod warunkiem, �e wi�cej nie z�amie prawa.",
	victim->przypadki[3] );
	ch_tell( mob, ch, buf );
	do_emote( mob, (char *)"wyci�ga komunikator." );
	sprintf( buf, "Tu %s. Macie tam jeszcze %s %s?",
	mob->przypadki[0],
	SEX_STR( ch, "tego", "t�", "to" ),
	victim->przypadki[4] );
	do_say( mob, buf );
	do_say( mob, (char *)"Dobrze... Zwolni�." );

	/* Tak jest! */
	crime->released = true;

	return;
}

/* zwraca wska�nik na przest�pstwa pope�nione przez 'ch' na planecie 'planet' */
CRIME_DATA *find_crime( CHAR_DATA *ch, PLANET_DATA *planet )
{

	IF_BUG( ch==NULL, "" )
		return NULL;

	IF_BUG( planet==NULL, "(%s)", ch->name )
		return NULL;

	IF_BUG( IS_NPC(ch), "(%s)", ch->name )
		return NULL;

	if ( ch->crimes.empty() )
		return NULL;

	for ( auto* crime : ch->crimes )
		if ( !str_cmp( planet->name, crime->planet ) )
			return crime;

	return NULL;
}


void crime_to_char( CHAR_DATA *ch, char *planet, int type )
{
	CRIME_DATA 	*crime;

	IF_BUG( ch==NULL, "" )
		return;

	IF_BUG( !*planet, "(%s)", ch->name )
		return;

	/*
	* je�li gracz pope�ni� ju� przest�pstwo na tej planecie --
	* zwi�kszmy wyrok i ewentualnie dodajmy typ do listy pope�nionych
	* wykrocze�
	*/
	if ( !ch->crimes.empty() )
	{
		for ( auto* crime : ch->crimes )
		{
			if ( !str_cmp( crime->planet, planet ) )
			{
				SET_BIT( crime->type, type );
			/*
			* to o ile wskoczy nam wska�nik przegi�cia zale�y
			* od typu przest�pstwa - im wi�cej tym gorzej
			*/
				crime->level += type;
				return;
			}
		}
	}

	/*
	* A je�li nie jest recydywist� -- dopiszmy przest�pstwo do listy
	*/
	CREATE( crime, CRIME_DATA, 1 );
	ch->crimes.push_back( crime );
	STRDUP( crime->planet, capitalize( planet ) );
	crime->type		= type;
	crime->level	= type;
	crime->jail_vnum	= 0;
	crime->jail_end	= 0;
	crime->released	= false;
	return;
}

void crime_remove( CHAR_DATA *ch, char *planet, int type )
{
	IF_BUG( ch==NULL, "" )
		return;

	IF_BUG( !*planet, "(%s)", ch->name )
		return;

	if ( ch->crimes.empty() )
		return;

	for ( auto* crime : std::list<CRIME_DATA *>(ch->crimes) )
	{
		if ( !str_cmp( crime->planet, planet ) )
		{
		/*
			* UWAGA !!! tutaj specjalnie jest "==", a nie "IS_SET"
			* bo sprawdzamy, czy jest to OSTATNIA FLAGA
			*/
			if( crime->type == type
			||  crime->type <= 0 ) /* <-- np. CRIME_ANY albo.. bug ;) */
				free_crime( ch, crime );
			else
			{
				REMOVE_BIT( crime->type, type );
				crime->level -= type;
			}
		}
	}
	return;
}

char *crime_flag_string( CRIME_DATA *crime )
{
	static char		crime_buf	[MSL];
	bool 			crime_found   = false;

	crime_buf[0] ='\0';
	if( IS_SET( crime->type, CRIME_SWINDLE ) )
	{
		strcat( crime_buf, "szwindle" );
		crime_found = true;
	}
	if( IS_SET( crime->type, CRIME_HACKING ) )
	{
		if( crime_found )
			strcat( crime_buf, ", " );
		strcat( crime_buf, "w�amania komputerowe" );
		crime_found = true;
	}
	if( IS_SET( crime->type, CRIME_STEAL ) )
	{
		if( crime_found )
			strcat( crime_buf, ", " );
		strcat( crime_buf, "kradzie�" );
		crime_found = true;
	}
	if( IS_SET( crime->type, CRIME_CONTRABAND ) )
	{
		if( crime_found )
			strcat( crime_buf, " i " );
		strcat( crime_buf, "kontraband�" );
		crime_found = true;
	}
	if( IS_SET( crime->type, CRIME_MURDER ) )
	{
		if( crime_found )
			strcat( crime_buf, " oraz " );
		strcat( crime_buf, "zab�jstwo" );
		crime_found = true;
	}

	if( !crime_found )
	{
		bug( "Did't find a valid crime->type" );
		strcat( crime_buf, "przekr�ty wszelkiego rodzaju" );
	}
	return crime_buf;
}

/*funkcja pokazuje, na jakich planetach gracz jest poszukiwany*/
DEF_DO_FUN( wanted )
{
	CHAR_DATA 	*victim;


	if( argument[0] == '\0' )
		victim = ch;
	else
	{
		if( IS_ADMIN( argument ) && !IS_ADMIN( ch->name ) )
		{
			ch_printf(ch,
			"%s jednym z W�ADC�W TEGO �WIATA i nikt nie ma prawa go �ciga� !!!" NL,
			capitalize(argument) );
			return;
		}
		else if( ( victim = get_char_world(ch, argument) ) == NULL )
		{
			send_to_char("Nie ma nikogo takiego w grze." NL, ch);
			return;
		}
	}

	if( !IS_NPC( victim ) )
	{
		if( IS_ADMIN( victim->name ) && !IS_ADMIN( ch->name ) )
		{
			ch_printf(ch,
			"%s jednym z W�ADC�W TEGO �WIATA i nikt nie ma prawa go �ciga� !!!" NL,
			capitalize(argument) );
			return;
		}

		if ( victim->crimes.empty() )
		{
			victim == ch ?
				send_to_char( "Nie poszukuj� ci� na �adnej planecie." NL, ch ) :
				send_to_char( "Ta osoba nie jest nigdzie poszukiwana." NL, ch );
		}
		else
		{
			ch_printf( ch, FB_WHITE
			"Oto lista planet, na kt�rych %s jest%s poszukiwan%s:" EOL,
			victim == ch ? ""   : victim->przypadki[0],
			victim == ch ? "e�" : "",
			SEX_SUFFIX_YAE( victim ) );

			for ( auto* crime : victim->crimes)
			{
				PLANET_DATA	*planet = NULL;

				if( crime->jail_vnum )
					planet = get_room_index( crime->jail_vnum )->area->planet;

				ch_printf( ch, "- %s za %s - przewiduje si�%s." NL,
				crime->planet,
				crime_flag_string( crime ),
				crime->level <= 4  ? " �agodn� kar� wi�zienia"            :
				crime->level <= 8  ? " kar� wi�zienia"                    :
				crime->level <= 16 ? " wysok� kar� wi�zienia"             :
				crime->level <= 32 ? " konkretn� odsiadk�"                :
				crime->level <= 64 ? ", �e troch� posiedzisz"  	      :
				crime->level <=128 ? " przygotowanie ci oddzielnej celi"  :
							" DO�YWOCIE"  );

				if( crime->jail_end )
					ch_printf( ch , NL FG_CYAN
				"Kara, kt�r� %s w�a�nie odsiaduje%s na %s "
				"zako�czy si� za %d godzi%s."			EOL,
					victim == ch ? "" 		: victim->przypadki[0],
					victim == ch ? "sz" 	: "",
					planet ? planet->name 	: "Generalnej Planecie Wi�ziennej",
					crime->jail_end,
					NUMBER_SUFF(crime->jail_end, "ne", "ny", "n" ) );
			}
		}
		return;
	}
	send_to_char( "Ta osoba nie jest nigdzie poszukiwana." NL, ch );
	return;
}


/*
* WI�ZIENIA					(C) 2002 Thanos ;)
*/

/* zwraca losowo wybran� cel� w wi�zieniu */
ROOM_INDEX_DATA *find_jail_cell( PLANET_DATA *planet )
{
	ROOM_INDEX_DATA	*	cell;
	int					cell_vnum;
	int			 		one_cell;
	int			 		found		= 0;

	/* liczymy ile cel ma wi�zienie */
	if ( planet )
		for( cell_vnum = planet->first_jail; cell_vnum <= planet->last_jail; cell_vnum++ )
		{
			if( ( cell = get_room_index( cell_vnum ) ) != NULL
			&&    cell->sector_type == SECT_JAIL_CELL )
				found++;
		}
	/* printf( "found %d cells" NL, found ); */
	if( !found )
	{
		bug( "Ooops: NO CELLS ON PLANET'S JAIL !!!" );
		return get_room_index( ROOM_VNUM_DEFAULT_CELL );
	}

	/* losujemy jedn� */
	one_cell 	= number_range( 1, found );
	/* printf( "choosed %d of %d" NL, one_cell, found ); */
	found 	= 0;
	for( cell_vnum = planet->first_jail; cell_vnum <= planet->last_jail; cell_vnum++ )
	{
		if( ( cell = get_room_index( cell_vnum ) ) != NULL
		&&    cell->sector_type == SECT_JAIL_CELL )
		{
			found++;
			if( found == one_cell )
				return cell;
		}
	}

	/* tu nas by� nie powinno */
	bug( "Something's wrong" );
	return get_room_index( ROOM_VNUM_DEFAULT_CELL );
}

/* zwraca losowo wybrane biuro wi�zienia */
ROOM_INDEX_DATA *find_jail_office( PLANET_DATA *planet )
{
	ROOM_INDEX_DATA	*office;
	int			 office_vnum;
	int			 one_office;
	int			 found		= 0;

	/* liczymy ile cel ma wi�zienie */
	if ( planet )
		for( office_vnum = planet->first_jail; office_vnum <= planet->last_jail; office_vnum++ )
		{
			if( ( office = get_room_index( office_vnum ) ) != NULL
			&&    office->sector_type == SECT_JAIL_OFFICE )
				found++;
		}
	/* printf( "found %d offices" NL, found ); */
	if( !found )
	{
		bug( "Ooops: NO OFFICE ON PLANET'S JAIL !!!" );
		return get_room_index( ROOM_VNUM_DEFAULT_OFFICE );
	}

	/* losujemy jedno */
	one_office 	= number_range( 1, found );
	/* printf( "choosed %d of %d" NL, one_office, found ); */
	found 	= 0;
	for( office_vnum = planet->first_jail; office_vnum <= planet->last_jail; office_vnum++ )
	{
		if( ( office = get_room_index( office_vnum ) ) != NULL
		&&    office->sector_type == SECT_JAIL_OFFICE )
		{
			found++;
			if( found == one_office )
				return office;
		}
	}

	/* tu nas by� nie powinno */
	bug( "Something's wrong" );
	return get_room_index( ROOM_VNUM_DEFAULT_OFFICE );
}

/* zwraca losowo wybrane biuro wi�zienia */
ROOM_INDEX_DATA *find_jail_entrance( PLANET_DATA *planet )
{
	ROOM_INDEX_DATA	*entrance;
	int			 entrance_vnum;
	int			 one_entrance;
	int			 found		= 0;

	/* liczymy ile wej�� ma wi�zienie */

/* Chodzi o to planet->first_jail. Jest taka mozliwosc, ze do krainki
nie ma przypisanej zadnej planety, wtedy planet jest NULL i mud pada.
Do jail.are przypisalem planete i juz nie pada ale mimo wszystko trzeba
to jakos ciekawie w ponizszym kodzie rozwiazac a ja nie mam pomyslu
*/
	/* Ok - sprobujmy tak: */
	if ( planet )             /* i starczy :P */
		for( entrance_vnum = planet->first_jail; entrance_vnum <= planet->last_jail; entrance_vnum++ )
		{
			if( ( entrance = get_room_index( entrance_vnum ) ) != NULL
			&&    entrance->sector_type == SECT_JAIL_ENTRANCE )
				found++;
		}
	/* printf( "found %d offices" NL, found ); */
	if( !found )
	{
		bug( "Ooops: NO ENTRANCE ON PLANET'S JAIL !!!" );
		return get_room_index( ROOM_VNUM_DEFAULT_ENTRANCE );
	}

	/* losujemy jedno */
	one_entrance= number_range( 1, found );
	/* printf( "choosed %d of %d" NL, one_office, found ); */
	found 	= 0;
	for( entrance_vnum = planet->first_jail; entrance_vnum <= planet->last_jail; entrance_vnum++ )
	{
		if( ( entrance = get_room_index( entrance_vnum ) ) != NULL
		&&    entrance->sector_type == SECT_JAIL_ENTRANCE )
		{
			found++;
			if( found == one_entrance )
				return entrance;
		}
	}

	/* tu nas by� nie powinno */
	bug( "Something's wrong" );
	return get_room_index( ROOM_VNUM_DEFAULT_ENTRANCE );
}

/* zwraca szaf� w biurze */
OBJ_DATA *find_jail_closet( CHAR_DATA *ch, PLANET_DATA *planet )
{
	ROOM_INDEX_DATA	*office;
	OBJ_DATA		*closet;

	office = find_jail_office( planet );

	if( office )
	{
		for ( auto* c : office->contents )
		{
			if( c && c->pIndexData->vnum == OBJ_VNUM_JAIL_CLOSET )
				return c;
		}
	/* w razie gdyby kto� zniszczy� szaf� ;) */
		closet = create_object( get_obj_index( OBJ_VNUM_JAIL_CLOSET ), MAX_LEVEL );
		obj_to_room( closet, office );
		return closet;
	}

	/* je�li co� poszlo nie tak, spr�bujmy to naprawi� */
	bug( "Fatal --> NO OFFICE, putting closet at ch->in_room" );
	closet = create_object( get_obj_index( OBJ_VNUM_JAIL_CLOSET ), MAX_LEVEL );
	obj_to_room( closet, ch->in_room );
	return closet;
}

/* tak - je�li przedmiot jest depozytem */
bool is_deposit( OBJ_DATA *obj )
{
	for ( auto* d : descriptor_list )
	{
		CHAR_DATA *ch;
		if( ( ch = CH( d ) ) == NULL )
			continue;
		if( IS_NPC( ch ) ) /* ciekawe czy co� takiego mo�e si� zdarzy� ;) */
			continue;
		if( ch->crimes.empty() )
			continue;

		if( ch->deposit && ch->deposit == obj )
			return true;
	}
	return false;
}

/* zwraca w�a�ciciela danego depozytu */
CHAR_DATA *deposit_owner( OBJ_DATA *obj )
{
	for ( auto* d : descriptor_list )
	{
		CHAR_DATA *ch;
		if( ( ch = CH( d ) ) == NULL )
			continue;
		if( IS_NPC( ch ) ) /* ciekawe czy co� takiego mo�e si� zdarzy� ;) */
			continue;
		if( ch->crimes.empty() )
			continue;

		if( ch->deposit && ch->deposit == obj )
			return ch;
	}
	return NULL;
}

/*
* Jedna z bardziej lamerskich f-cji w tym kodzie :
*/
OBJ_DATA *prepare_deposit( CHAR_DATA *ch )
{
	OBJ_DATA	*deposit;
	char	buf		[MSL];

	deposit = create_object(
	get_obj_index( OBJ_VNUM_DEPOSIT ),
	ch->top_level );

	sprintf( buf, "depozyt %s", ch->przypadki[1] );
	STRDUP( deposit->name, buf );
	STRDUP( deposit->przypadki[0], buf );
	sprintf( buf, "depozytu %s", ch->przypadki[1] );
	STRDUP( deposit->przypadki[1], buf );
	sprintf( buf, "depozytowi %s", ch->przypadki[1] );
	STRDUP( deposit->przypadki[2], buf );
	sprintf( buf, "depozyt %s", ch->przypadki[1] );
	STRDUP( deposit->przypadki[3], buf );
	sprintf( buf, "depozytem %s", ch->przypadki[1] );
	STRDUP( deposit->przypadki[4], buf );
	sprintf( buf, "depozycie %s", ch->przypadki[1] );
	STRDUP( deposit->przypadki[5], buf );

	sprintf( buf,
	"Depozyt %s najwyra�niej si� tutaj zawieruszy�.", ch->przypadki[1] );
	STRDUP( deposit->description, buf );

	ch->deposit 		= deposit; /*zapami�tajmy wska�nik*/

	STRDUP( deposit->owner_name, ch->name );

	/* ufffff */
	return deposit;
}

/*
* Zapisujemy depozyt gracza w pliku ../deposit/<imie_gracza>.deposit
*/
void save_deposit( CHAR_DATA *ch )
{
	if ( ch && ch->deposit )
	{
		FILE *	fp;
		char 	filename	[256];
		int 	templvl;

		sprintf( filename, "%s%s.deposit", DEPOSIT_DIR,
			capitalize( ch->name ) );

		if ( ( fp = fopen( filename, "w" ) ) == NULL )
		{
			bug( "fopen" );
			perror( filename );
		}
		else
		{
			templvl = ch->top_level;
			ch->top_level = LEVEL_HERO;
			if( ch->deposit )
				fwrite_obj(ch, ch->deposit, fp, 0, OS_CARRY );
			fprintf( fp, "#END\n" );
			ch->top_level = templvl;
			fclose( fp );
		}
	}
	return;
}

/* �aduje depozyt z pliku */
void fread_deposit( CHAR_DATA *ch, OBJ_DATA *closet )
{
	FILE 	*fp;
	char 	 filename	[256];
	int	 	 found		= 0;

	IF_BUG( closet==NULL, "" )
		return;

	IF_BUG( ch==NULL, "" )
		return;


	sprintf( filename, "%s%s.deposit", DEPOSIT_DIR, capitalize( ch->name ) );

	if ( ( fp = fopen( filename, "r" ) ) != NULL )
	{
		for ( ; ; )
		{
			char letter;
			char *word;

			letter = fread_letter( fp );
			if ( letter == '*' )
			{
				fread_to_eol( fp );
				continue;
			}

			if ( letter != '#' )
			{
				bug( "# not found. (%s)", ch->name );
				break;
			}

			word = fread_word( fp );
			if ( !str_cmp( word, "OBJECT" ) )
			{
				/*
				* niech nam pomo�e ten, kt�ry wiecznie si� obija  ;)
				*/
				set_supermob( closet );
				fread_obj  ( supermob, fp, OS_CARRY );

				for ( auto* obj : std::list<OBJ_DATA *>(supermob->carrying) )
				{
					obj_from_char( obj );
					obj_to_obj( obj, closet );

					if( obj->pIndexData->vnum == OBJ_VNUM_DEPOSIT )
					{
						found++;
						ch->deposit = obj;
					}
				}

				if( found > 1 )
					bug( "Duplicate deposit!" );
				release_supermob();
			}
			else if ( !str_cmp( word, "END"    ) )
				break;
			else
			{
				bug( "bad section. %s", ch->name );
				break;
			}
		}
		fclose( fp );
	}
	return;
}

void load_deposit( CHAR_DATA *ch )
{
	if( !ch || IS_NPC( ch ) || ch->crimes.empty() )
		return;

	for( auto* crime : ch->crimes )
	{
		if( crime->jail_vnum && crime->jail_end )
		{
			fread_deposit( ch,
			find_jail_closet( ch, get_planet(crime->planet) ) );
			return;
		}
	}
	return;
}

/* niszczy wska�nik na depozyt gracza oraz plik, w kt�rym by� zapisany
dobytek */
void destroy_deposit( CHAR_DATA *ch )
{
	char 	buf	[256];

	IF_BUG( ch==NULL, "" )
		return;

	if( !ch->deposit )
		return;

	sprintf( buf, "%s%s.deposit", DEPOSIT_DIR, capitalize( ch->name ) );
	unlink ( buf );
	extract_obj ( ch->deposit ); /* wi�c lepiej to najpierw opr�ni� */
	ch->deposit = NULL;
	save_char_obj( ch );
	return;
}


/*
* Funkcja 'aresztuje' gracza. Przenosi go do wi�zienia,
* ustala wyrok, przerzuca inventory gracza do depozytu. 	Thanos
*/
void jail_char( CHAR_DATA *victim, CHAR_DATA *policeman, CRIME_DATA *crime )
{
	int 	         time;
	OBJ_DATA 		*closet;
	OBJ_DATA 		*deposit;
	ROOM_INDEX_DATA 	*jail;
	PLANET_DATA		*planet;

	if( !victim || IS_NPC( victim ) || !policeman || !crime )
		return;

	if( ( planet = get_planet( crime->planet ) ) == NULL )
		return;

	act( COL_ACTION,
		"$n aresztuje $N$3 i zabiera $I do aresztu w imieniu prawa.",
		policeman, NULL, victim, TO_NOTVICT );
	act( COL_ACTION,
		"$n zabiera ci� do aresztu w imieniu prawa.",
		policeman, NULL, victim, TO_VICT    );

	/* najpierw przenie�my gracza */
	jail 			= find_jail_cell( planet );
	char_from_room		( victim );
	char_to_room		( victim, jail );

	do_look( victim, (char *)"auto" );

	deposit = prepare_deposit( victim );

	/* rozbieramy gracza */
	for ( auto* obj : std::list<OBJ_DATA *>(victim->carrying) )
	{
		obj_from_char( obj );
		if ( IS_OBJ_STAT( obj, ITEM_INVENTORY )  )
			extract_obj( obj );  		/* niekt�re rzeczy musz� znikn�� */
		else
		{
			deposit->value[0] 	+= obj->weight;	/* zwi�kszamy pojemno�� */
			obj_to_obj( obj, deposit ); 	/* wsio do depozytu */
		}
	}
	/* i do szafy;) */
	closet 			= find_jail_closet( victim, planet );
	closet->value[0]   	       += deposit->weight;/* zwi�kszamy pojemno�� */
	obj_to_obj			( deposit, closet );

	/* jak wk�adaj� depozyt to niech zamkn� na wszelki szafe */
	SET_BIT			( closet->value[1], CONT_CLOSEABLE );
	SET_BIT			( closet->value[1], CONT_CLOSED );
	SET_BIT			( closet->value[1], CONT_LOCKED );
	SET_BIT			( closet->value[1], CONT_PICKPROOF );

	send_to_char( NL "Twoje rzeczy zosta�y zabrane do depozytu." NL, victim );

	/* �egnajcie pieni��ki */
	victim->gold 		= 0;
	send_to_char( "Twoje kredytki zosta�y skonfiskowane." NL, victim );

	/* kara zale�y od poziomu przewinienia ( w tickach )*/
	time			= crime->level + 1 * number_range( 45, 75 );
	time 			= UMAX( 1, time/4 );
	time		       /= 2;
	crime->jail_end		= time;
	crime->jail_vnum 		= jail->vnum;


	ch_printf( victim, "Zostaniesz w areszcie przez %d godzi%s." NL,
	time, NUMBER_SUFF( time, "n�", "ny", "n"  ) );

	save_char_obj( victim ); /* z tej f-cji zapisze si� te� depozyt */
	/* po sprawie  :) */
	return;
}

/* uwalniamy gracza -- je�li ju� odsiedzia� swoje */
void unjail_char( CHAR_DATA *ch, CRIME_DATA *crime )
{
	PLANET_DATA	*planet = NULL;

	if( !ch || !crime )
	return;

	if( crime->jail_vnum )
		planet = get_room_index( crime->jail_vnum )->area->planet;

	ch->position = POS_STANDING;

	echo_to_room( ch->in_room, PLAIN
	"Stra�nik wi�zienia podchodzi do drzwi twojej celi..." );

	echo_to_room( ch->in_room, EOL
	"Drzwi otwieraj� si�." EOL );

	act( COL_SAY, "Stra�nik m�wi '$n - p�jdziesz ze mn�.'", ch, NULL, NULL, TO_ROOM );
	act( COL_SAY, "Stra�nik m�wi '$n - p�jdziesz ze mn�.'", ch, NULL, NULL, TO_CHAR );

	act( PLAIN,"Stra�nik wyprowadza $n$3 przed gmach wi�zienia.", ch, NULL, NULL, TO_ROOM );
	act( PLAIN,"Stra�nik wyprowadza ci� przed gmach wi�zienia.", ch, NULL, NULL, TO_CHAR );

	char_from_room	( ch );

	echo_to_room( get_room_index( crime->jail_vnum ), EOL
	"Drzwi zamykaj� si� si�." EOL );

	char_to_room	( ch, find_jail_entrance( planet ) );
	do_look		( ch, (char *)"auto" );

	act( COL_SAY, "Stra�nik m�wi 'Jeste� woln$y $n.'", ch, NULL, NULL, TO_ROOM );
	act( COL_SAY, "Stra�nik m�wi 'Jeste� woln$y $n.'", ch, NULL, NULL, TO_CHAR );

	if( crime->released )
	{
		act( COL_SAY, "Stra�nik m�wi 'Kto� wp�aci� za ciebie kaucj�.'", ch, NULL, NULL, TO_ROOM );
		act( COL_SAY, "Stra�nik m�wi 'Kto� wp�aci� za ciebie kaucj�.'", ch, NULL, NULL, TO_CHAR );
	}
	else
	{
		act( COL_SAY, "Stra�nik m�wi 'Twoja odsiadka u nas si� sko�czy�a.'", ch, NULL, NULL, TO_ROOM );
		act( COL_SAY, "Stra�nik m�wi 'Twoja odsiadka u nas si� sko�czy�a.'", ch, NULL, NULL, TO_CHAR );
	}

	act( COL_SAY, "Stra�nik m�wi 'Oby� nie wraca�$o do nas wi�cej.'", ch, NULL, NULL, TO_ROOM );
	act( COL_SAY, "Stra�nik m�wi 'Oby� nie wraca�$o do nas wi�cej.'", ch, NULL, NULL, TO_CHAR );

	ch_printf( ch, NL FB_WHITE "Wychodzisz na WOLNO��!" NL EOL );

	/* chwila napi�cia */
	if( ch->deposit )
	{
		act( COL_SAY,
		"Stra�nik m�wi 'By�bym zapomnia�. Tw�j depozyt zostawiam ci do dyspozycji.'",
		ch, NULL, NULL, TO_CHAR );
		empty_obj( ch->deposit, NULL, ch->in_room );
	}
	else
	{
		act( COL_SAY,
		"Stra�nik m�wi 'A je�li chodzi o tw�j depozyt... C�. Gdzie� si� zawieruszy�.'",
		ch, NULL, NULL, TO_CHAR );
	}

	act( PLAIN, "Stra�nik odchodzi by dalej wykonywa� swoje obowi�zki.", ch, NULL, NULL, TO_ROOM );
	act( PLAIN, "Stra�nik odchodzi by dalej wykonywa� swoje obowi�zki.", ch, NULL, NULL, TO_CHAR );

	free_crime		( ch, crime );
	destroy_deposit	( ch );
	save_char_obj	( ch );
	/* i po sprawie ;) */
	return;
}



/* sprawdza czy gracz 'dobrze' odsiaduje ;) */
void crime_update( void )
{
	for ( auto* d : descriptor_list )
	{
		CHAR_DATA *ch;
		if( ( ch = CH( d ) ) == NULL )
			continue;

		if( IS_NPC( ch ) )
			continue;

		if( ch->crimes.empty() )
			continue;

		for ( auto* crime : std::list<CRIME_DATA *>(ch->crimes) )
		{
			if( crime->jail_end > 0 )
				crime->jail_end--;

			if( crime->jail_end < 0 )
				crime->jail_end = 0;

			/* koniec kary - freedom */
			if( ( crime->jail_vnum && ch->in_room
			&&  crime->jail_vnum == ch->in_room->vnum
			&& !crime->jail_end ) || crime->released )
			{
				unjail_char( ch, crime );
				break;
			}

			/* te� freedom, ale inny ;) */
			if( crime->jail_end  %  JAIL_ACTIVITY == 0
			&&  crime->jail_vnum && ch->in_room
			&&  crime->jail_vnum != ch->in_room->vnum )
			{
				ch_printf( ch, FB_WHITE
					"W�adze %s w�a�nie zorientowa�y si� o twojej ucieczce!" EOL,
					crime->planet );

				crime->jail_vnum = 0;
				crime->jail_end  = 0;
				crime->level	*= 2;	/* recydywa */

				/* a co to? ucieka i jeszcze chce rzeczy? ;) */
				destroy_deposit( ch );
				break;
			}

		}
	}
}
