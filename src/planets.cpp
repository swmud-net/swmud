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
*                           Special planet module                          *
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "mud.h"

std::list<PLANET_DATA *> planet_list;

/* local routines */
void	fread_planet		args( ( PLANET_DATA *planet, FILE *fp ) );
bool	load_planet_file	args( ( const char *planetfile ) );
void	write_planet_list	args( ( void ) );
void    do_imports                    args( ( CHAR_DATA *ch, char *argument ) );
SEASON_DATA *get_season		args( ( PLANET_DATA *planet, int nr ) );
void add_material_warehouse( PLANET_DATA * pPlanet,
			MATERIAL_DATA * pMat,  int quant);

const char 	* const cargo_names[CARGO_MAX] =
	{
  	"none", "all","lommite","Meleenium","Neutronium","Zersium",
  	"steel","rhyll","alazhi"
	};

/* from space2.c */
bool	has_map				args( ( SPACE_DATA *starsystem, CHAR_DATA *ch ) );

DEPOSIT_DATA * get_deposit( PLANET_DATA * planet, int nr )
{
	int i = 0;

	for( auto* pDeposit : planet->deposits )
	{
		if( i == nr )
			return pDeposit;
		i++;
	}

	return NULL;
}


PLANET_DATA *get_planet( const char *name )
{
    for ( auto* planet : planet_list )
       if ( !str_cmp( name, planet->name ) )
         return planet;
    for ( auto* planet : planet_list )
       if ( nifty_is_name_prefix( (char *)name, planet->name ) )
         return planet;
    return NULL;
}

void write_planet_list( )
{
    FILE *		fpout;

    fpout = fopen( PLANET_LIST, "w" );
    if ( !fpout )
    {
	bug( "FATAL: cannot open planet.lst for writing!" );
 	return;
    }

    for ( auto* tplanet : planet_list )
	fprintf	( fpout, "%s\n", tplanet->filename );
    fprintf	( fpout, "$\n" );
    fclose	( fpout );
}

void save_planet( PLANET_DATA *planet )
{
    FILE *	fp;
    char 	filename	[256];

    IF_BUG( planet==NULL, "" )
	return;

    if ( !planet->filename || planet->filename[0] == '\0' )
    {
	bug( "%s has no filename", planet->name );
	return;
    }

    sprintf( filename, "%s%s", PLANET_DIR, planet->filename );

    RESERVE_CLOSE;
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "fopen" );
    	perror( filename );
    }
    else
    {
	int		nr;

	fprintf( fp, "#PLANET\n" );
	fprintf( fp, "Name         %s~\n",	planet->name		);
	fprintf( fp, "Filename     %s~\n",	planet->filename        );
	fprintf( fp, "BaseValue    %ld\n",	planet->base_value      );
	fprintf( fp, "Flags        %lld\n",	planet->flags           );
	fprintf( fp, "PopSupport   %f\n",	planet->pop_support     );
	fprintf( fp, "Import       %d\n",	planet->import          );
	fprintf( fp, "Export       %d\n",	planet->_export	        );
	fprintf( fp, "Prisei       %d\n",	planet->prisei          );
	fprintf( fp, "Prisee       %d\n",	planet->prisee          );
	fprintf( fp, "Stock        %d\n",	planet->stock           );
	fprintf( fp, "Maxstock     %d\n",	planet->maxstock        );
       	if ( planet->starsystem != NULL )
	   fprintf( fp, "Starsystem   %s~\n",	planet->starsystem->name);
	fprintf( fp, "Gravity      %.0f\n",	planet->gravity         );
	fprintf( fp, "Xpos         %.0f\n",	planet->xpos            );
	fprintf( fp, "Ypos         %.0f\n",	planet->ypos            );
	fprintf( fp, "Zpos         %.0f\n",	planet->zpos            );
	fprintf( fp, "Radius       %.0f\n",	planet->radius          );
	fprintf( fp, "Star_radius  %.0f\n",	planet->star_radius     );
	fprintf( fp, "Star_radius2  %.0f\n",	planet->star_radius2     );

	if ( planet->governed_by && planet->governed_by->name )
        	fprintf( fp, "GovernedBy   %s~\n",	planet->governed_by->name);

	//added by Thanos - wiezienia
	fprintf( fp, "JailFirst    %d\n",	planet->first_jail      );
	fprintf( fp, "JailLast     %d\n",	planet->last_jail       );
	//done

	/* Thanos czas i pogoda: KOLEJNO�� OBOWI�ZKOWA */
	fprintf( fp, "TimeInfo     %d %d %d\n",	planet->daylen,
						planet->nightlen,
						planet->monthlen       	);

	for( auto* season : planet->seasons )
	fprintf( fp, "Season       %s~ %d %d %d %d %d %d %d\n",
						*season->name ? season->name : "",
						season->temperature,
						season->diff_temperature,
						season->mmhg,
						season->diff_mmhg,
						season->day_length - planet->daylen,
						season->length,
						season->windspeed      	);

	nr = 1; /* numer pory roku */
	for( auto* season : planet->seasons )
	{
	    if( planet->curr_season == season )
		break;
	    nr++;
	}
	fprintf( fp, "CurrTime     %d %d %d %d\n",  planet->hour,
						    planet->day,
						    planet->month,
						    nr );

// Aldegard
	for ( auto* dock : planet->docks )
	{
	    fprintf( fp, "Dock %d %.0f %d %s~\n",dock->vnum,
	                dock->capacity, dock->hidden, dock->name );
	}
		//added by Tanglor - listy materialow produkownych przez planete
		for( auto* pDeposit : planet->deposits )
			fprintf( fp, "Deposit       %d %s~\n",
					 pDeposit->daily_mining, pDeposit->material_name );
		if ( planet->pWarehouse )
		{
				fprintf( fp, "WarehouseData %d\n",
				planet->pWarehouse->max_capacity );
			for( auto* pCargo : planet->pWarehouse->cargo )
			if (pCargo->pMaterial)
				fprintf( fp, "Warehouse     %d %d %s~\n",
						 pCargo->amount,
						 pCargo->pMaterial->type,
						 pCargo->pMaterial->name );

		}
		fprintf( fp, "Description         %s~\n",	planet->description  	);
        fprintf( fp, "Description2         %s~\n",	planet->description2  	);
        fprintf( fp, "Version         %d\n",	planet->version  	);
	for( auto* pArea : planet->areas )
	if (pArea->filename)
        fprintf( fp, "Area         %s~\n",	pArea->filename  	);
	fprintf( fp, "End\n\n"						);
	fprintf( fp, "#END\n"						);
    }
    fclose( fp );
    RESERVE_OPEN;
    return;
}

/*
 * Read in actual planet data.
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

void fread_planet( PLANET_DATA *planet, FILE *fp )
{
    const char *word;
    bool fMatch;
	MATERIAL_DATA	* pMat;

    /* w razie gdyby nie bylo wiezienia dla planety 	--Thanos*/
    planet->first_jail = ROOM_VNUM_FIRST_JAIL;
    planet->last_jail  = ROOM_VNUM_LAST_JAIL;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = false;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = true;
	    fread_to_eol( fp );
	    break;

	case 'A':
	    if ( !str_cmp( word, "Area" ) )
	    {
	        char aName[MAX_STRING_LENGTH];

	     	strcpy(aName, st_fread_string(fp));
		for( auto* pArea : area_list )
	          if (pArea->filename && !str_cmp(pArea->filename , aName ) )
	          {
	             pArea->planet = planet;
	             planet->areas.push_back( pArea );
	          }
                fMatch = true;
	    }
	    break;

	case 'B':
	    KEY( "BaseValue",	planet->base_value,		fread_number( fp ) );
	    break;

	case 'C':
	    if ( !str_cmp( word, "CurrTime" ) )
	    {
		int nr;

		planet->hour 	=		fread_number( fp );
		planet->day	=		fread_number( fp );
		planet->month 	=		fread_number( fp );
                nr		=		fread_number( fp );
		planet->curr_season = get_season( planet, nr );
		fMatch = true;
	    }

	case 'D':
	    SKEY( "Description",	planet->description,	fread_string( fp ) );
	    SKEY( "Description2",	planet->description2,	fread_string( fp ) );
		if( !str_cmp( word, "Deposit" ) )
		{
			DEPOSIT_DATA * pDeposit;
			pDeposit = new_deposit();
			pDeposit->daily_mining = fread_number( fp );
			pDeposit->material_name = fread_string( fp );
			planet->deposits.push_back( pDeposit );
			fMatch = true;
		}
		if( !str_cmp( word, "Dock" ) )
	    {
			DOCK_DATA *	dock;

			CREATE( dock, DOCK_DATA, 1 );
			dock->vnum 		= fread_number( fp );
			dock->capacity 		= atof( fread_word( fp ) );
			dock->hidden		= fread_number( fp );
			dock->name		= fread_string( fp );
			planet->docks.push_back( dock );
			fMatch = true;
	    }
	    break;

	case 'E':
	    KEY( "Export",	planet->_export,     fread_number( fp ) );
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!planet->name)
		  STRDUP( planet->name, "" );
		if (!planet->description)
		  STRDUP( planet->description, "Planeta jak planeta" );
		if (!planet->description2)
		  STRDUP( planet->description2, "Planeta jak planeta" );
		if( planet->version < 2 )
		{
		    planet->ypos = 0;
		    planet->xpos = number_range( 5000, 20000 );
		    if( planet->zpos >= 0 )
		        planet->zpos = number_range(10, 100);
		    if( planet->zpos < 0 )
		        planet->zpos = number_range(-100, 10);
		    planet->star_radius = sqrt( ( planet->xpos * planet->xpos ) +
		                                 ( planet->ypos * planet->ypos ) );
		    planet->star_radius2 = planet->star_radius*3/4;
		    if( planet->radius == 0 )
		        planet->radius = 200;
		    planet->version = 2;
		}
		return;
	    }
	    break;

	case 'F':
	    SKEY( "Filename",	planet->filename,		fread_string( fp ) );
	    KEY( "Flags",	planet->flags,		        fread_number( fp ) );
	    break;

	case 'G':
	    KEY( "Gravity",	planet->gravity,        atof( fread_word( fp ) ) );
	    if ( !str_cmp( word, "GovernedBy" ) )
	    {
	     	planet->governed_by = get_clan ( st_fread_string(fp) );
                fMatch = true;
	    }
	    break;

	case 'I':
	    KEY( "Import",	planet->import,     fread_number( fp ) );
	    break;

	//added by Thanos (wi�zienia)
	case 'J':
	    KEY( "JailFirst",	planet->first_jail,     fread_number( fp ) );
	    KEY( "JailLast",	planet->last_jail,	fread_number( fp ) );
	    break;
	//done

	case 'M':
	    KEY( "Maxstock",	planet->maxstock,		fread_number( fp ) );
	    break;

	case 'N':
	    SKEY( "Name",	planet->name,		fread_string( fp ) );
	    break;

	case 'R':
	    KEY( "Radius",	planet->radius,		atof( fread_word( fp ) ) );
	    break;

	case 'P':
/*
    BUG! fread_number zwraca integer, a pop_support to float
    -- Thanos
	    KEY( "PopSupport",	planet->pop_support,	fread_number( fp ) );
 */
	    KEY( "PopSupport",	planet->pop_support,	atof( fread_word( fp ) ) );
	    KEY( "Prisei",	planet->prisei,	        fread_number( fp ) );
	    KEY( "Prisee",	planet->prisee,	        fread_number( fp ) );
	    break;

	case 'S':
	    KEY( "Stock",	planet->stock,		fread_number( fp ) );
	    KEY( "Star_radius",	planet->star_radius,	atof( fread_word( fp ) ) );
	    KEY( "Star_radius2",	planet->star_radius2,	atof( fread_word( fp ) ) );
	    if ( !str_cmp( word, "Starsystem" ) )
	    {
	     	planet->starsystem = starsystem_from_name ( st_fread_string(fp) );
                if (planet->starsystem)
                {
                     SPACE_DATA *starsystem = planet->starsystem;

                     starsystem->planets.push_back( planet );
                }
                fMatch = true;
	    }
	    if ( !str_cmp( word, "Season" ) )
	    {
		SEASON_DATA *	season;
		int		diff;

		season = new_season( planet, "no_name" );
		STRDUP( season->name,  st_fread_string( fp ) );
		season->temperature	= fread_number( fp );
		season->diff_temperature= fread_number( fp );
		season->mmhg		= fread_number( fp );
		season->diff_mmhg	= fread_number( fp );
		diff 			= fread_number( fp );
		season->day_length     	=
		    ( planet->daylen 	? planet->daylen   : 12 ) + diff;
		season->night_length   	=
		    ( planet->nightlen 	? planet->nightlen : 12 ) - diff;
		season->length 		= fread_number( fp );
		season->windspeed	= fread_number( fp );

		planet->seasons.push_back( season );
                fMatch = true;
	    }
	    break;

	case 'T':
	    KEY( "Taxes",	planet->base_value,		fread_number( fp ) );

	    if ( !str_cmp( word, "TimeInfo" ) )
	    {
		planet->daylen 		= fread_number( fp );
		planet->nightlen 	= fread_number( fp );
		planet->monthlen	= fread_number( fp );
                fMatch = true;
	    }
	    break;

	case 'W':
	    if ( !str_cmp( word, "Warehouse" ) )
		{
			int quant	= fread_number( fp );
			material_type type	= (material_type)fread_number( fp );
			char * name = fread_string( fp );

			pMat = get_material( name, type );
			fMatch = true;
			if ( !pMat )
			{
				bug( "%s - warehouse material doesnt exist");
				break;
			}
			add_material_warehouse( planet, pMat, quant);
		}
		KEY( "WarehouseData", planet->pWarehouse->max_capacity ,fread_number( fp ) );
		break;
	case 'V':
		     KEY( "Version",	planet->version,	fread_number ( fp ) );
	    break;

	case 'X':
		     KEY( "Xpos",	planet->xpos,	        atof( fread_word( fp ) ) );
	    break;

	case 'Y':
		     KEY( "Ypos",	planet->ypos,	        atof( fread_word( fp ) ) );
	    break;

	case 'Z':
		     KEY( "Zpos",	planet->zpos,	        atof( fread_word( fp ) ) );
	    break;

	}

	if ( !fMatch )
	{
	    bug( "no match: %s", word );
	}
    }
}

/*
 * Load a planet file
 */

bool load_planet_file( const char *planetfile )
{
    char 		filename	[256];
    PLANET_DATA *	planet;
    FILE *		fp;
    bool 		found;

    CREATE( planet, PLANET_DATA, 1 );

    planet->governed_by 	= NULL;
    planet->starsystem 		= NULL ;
	planet->pWarehouse		= NULL;
	planet->hour		= -1;
    planet->day			= -1;
    planet->month		= -1;

	CREATE( planet->pWarehouse, WAREHOUSE_DATA, 1);

    found = false;
    sprintf( filename, "%s%s", PLANET_DIR, planetfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = true;
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
		bug( "# not found." );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "PLANET"	) )
	    {
	    	fread_planet( planet, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		bug( "Load_planet_file: bad section: %s.", word );
		break;
	    }
	}
	fclose( fp );
    }

    if ( !found )
    {
        DISPOSE( planet );
	return found;
    }
    else

      planet_list.push_back( planet );

    return found;
}

/*
 * Load in all the planet files.
 */

void load_planets( )
{
    FILE *		fpList;
	const char *		filename;

    planet_list.clear();

    RESERVE_CLOSE;
    if ( ( fpList = fopen( PLANET_LIST, "r" ) ) == NULL )
    {
		perror( PLANET_LIST );
		exit( 1 );
    }

    for ( ; ; )
    {
		filename = feof( fpList ) ? "$" : fread_word( fpList );
		if( !sysdata.silent )
		    log_string( filename );
		if ( filename[0] == '$' )
			break;

		if ( !load_planet_file( filename ) )
		{
			bug( "Cannot load planet file: %s", filename );
		}
	}
	fclose( fpList );
	if( !sysdata.silent )
		log_string(" Done planets " );
    RESERVE_OPEN;
    return;
}

DEF_DO_FUN( setplanet )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    PLANET_DATA *planet;

    if ( IS_NPC( ch ) )
    {
	huh( ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	send_to_char( "Usage: setplanet <planet> <field> [value]\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( " base_value flags import prisei export prisee stock maxstock\n\r", ch );
	send_to_char( " name filename starsystem governed_by first_jail last_jail\n\r", ch );
	return;
    }

    planet = get_planet( arg1 );
    if ( !planet )
    {
	send_to_char( "No such planet.\n\r", ch );
	return;
    }


    if ( !str_cmp( arg2, "name" ) )
    {
	STRDUP( planet->name, argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }

    if ( !str_cmp( arg2, "governed_by" ) )
    {
        CLAN_DATA *clan;
        clan = get_clan( argument );
        if ( clan )
        {
           planet->governed_by = clan;
           send_to_char( "Done.\n\r", ch );
       	   save_planet( planet );
        }
        else
           send_to_char( "No such clan.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "starsystem" ) )
    {
        SPACE_DATA *starsystem;

        if ((starsystem=planet->starsystem) != NULL)
          starsystem->planets.remove(planet);
	if ( (planet->starsystem = starsystem_from_name(argument)) )
        {
           starsystem = planet->starsystem;
           starsystem->planets.push_back(planet);
           send_to_char( "Done.\n\r", ch );
	}
	else
	       	send_to_char( "No such starsystem.\n\r", ch );
	save_planet( planet );
	return;
    }

    if ( !str_cmp( arg2, "filename" ) )
    {
	STRDUP( planet->filename, argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	write_planet_list( );
	return;
    }

    //added by Thanos (wi�zienia)
    if ( !str_cmp( arg2, "first_jail" ) )
    {
 	if( get_room_index( atoi( argument ) ) == NULL )
	{
	    send_to_char( "No such room." NL, ch );
	    return;
	}
    	planet->first_jail = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }
    if ( !str_cmp( arg2, "last_jail" ) )
    {
 	if( get_room_index( atoi( argument ) ) == NULL )
	{
	    send_to_char( "No such room." NL, ch );
	    return;
	}
    	planet->last_jail = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }
    //done

    if ( !str_cmp( arg2, "base_value" ) )
    {
	planet->base_value = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_planet( planet );
	return;
    }
   if ( !str_cmp( arg2, "import"))
    {	int i;
       for (i = 0; i < CARGO_MAX; i++)
       {
          if (!str_cmp( argument, cargo_names[i]))
          {
             planet->import = i;
	     if (i==0) planet->prisei = 0;
	     else planet->prisei=100+i*10;
             send_to_char("Done.\n\r", ch );
             save_planet( planet );
             return;
          }
       }
       send_to_char("Type must be one folow:" NL "none, all, lommite , Meleenium, Neutronium, Zersium, steel," NL "rhyll, alazhi" NL, ch);
       return;
    }
     if ( !str_cmp( arg2, "export"))
    {	int i;
       for (i = 0; i < CARGO_MAX; i++)
       {
          if (!str_cmp( argument, cargo_names[i]) && (i != CARGO_ALL))
          {
	     planet->_export = i;
	     if (i==0)
	     {
	     	planet->prisee = 0;
  		planet->stock = 0;
	     }
	     else
	     {
	     	planet->prisee = 100+i*5;
		planet->stock = 10+i*90;
		planet->maxstock = 100+i*100;
	     }
             send_to_char("Done.\n\r", ch );
             save_planet( planet );
             return;
          }
       }
       send_to_char("Type must be one folow:" NL "none, all, lommite , Meleenium, Neutronium, Zersium, steel," NL "rhyll, alazhi" NL, ch);
       return;
    }
    if ( !str_cmp( arg2, "prisei" ) )
    {
	planet->prisei = URANGE( 0, atoi(argument) , 1000 );
	send_to_char("Done." NL, ch );
	save_planet( planet );
	return;
    }
    if ( !str_cmp( arg2, "prisee" ) )
    {
	if (planet->_export==0)
	{
		send_to_char("First set type of export" NL,ch);
		return;
	}
	planet->prisee = URANGE( 1, atoi(argument) , 5000 );
	send_to_char("Done." NL, ch );
	save_planet( planet );
	return;
    }
    if ( !str_cmp( arg2, "stock" ) )
    {
	if (planet->_export==0)
	{
		send_to_char("First set type of export" NL,ch);
		return;
	}
	if (planet->maxstock < atoi(argument))
	{
		send_to_char("Stock overflow. Set equal to maxstock" NL,ch);
		planet->stock=planet->maxstock;
		save_planet( planet );
		return;
	}
	planet->stock = URANGE( 0, atoi(argument) , 5000000 );
	send_to_char("Done." NL, ch );
	save_planet( planet );
	return;
    }
    if ( !str_cmp( arg2, "maxstock" ) )
    {
	planet->maxstock = URANGE( 0, atoi(argument) , 5000000 );
	if (planet->maxstock==0 && planet->stock >0)
	  planet->stock=0;
	send_to_char("Done." NL, ch );
	save_planet( planet );
	return;
    }
    if ( !str_cmp( arg2, "flags" ) )
    {
        char farg[MAX_INPUT_LENGTH];

        argument = one_argument( argument, farg);

        if ( farg[0] == '\0' )
        {
           send_to_char( "Possible flags: nocapture notshown\n\r", ch );
           return;
        }

        for ( ; farg[0] != '\0' ; argument = one_argument( argument, farg) )
        {
            if ( !str_cmp( farg, "nocapture" ) )
               TOGGLE_BIT( planet->flags, PLANET_NOCAPTURE );
            else
            if ( !str_cmp( farg, "notshown" ) )
               TOGGLE_BIT( planet->flags, PLANET_NOTSHOWN );
            else
               ch_printf( ch , "No such flag: %s\n\r" , farg );
	}
	send_to_char( "Done.\n\r", ch );
        save_planet( planet );
	return;
    }

    do_setplanet( ch, (char *)"" );
    return;
}

DEF_DO_FUN( showplanet )
{ //very highly extremely hard modified by Trog :))))
    PLANET_DATA *	planet;
    char 		buf 	[MAX_STRING_LENGTH];
    char        	buf1	[MAX_STRING_LENGTH];
    int			i, a=0;

    if ( IS_NPC( ch ) )
    {
		huh( ch );
		return;
    }

    if ( argument[0] == '\0' )
    {
		send_to_char( "Usage: showplanet <planet>\n\r", ch );
		return;
    }

    planet = get_planet( argument );
    if ( !planet )
    {
		send_to_char( "No such planet.\n\r", ch );
		return;
    }

    buf	[0]='\0';
    buf1[0]='\0';

    sprintf( buf1,
    		FG_CYAN "Name:  " PLAIN "%-10s" FG_CYAN " Filename: " PLAIN "%10s"
			FG_CYAN " Starsystem: " PLAIN "%s" NL,
			planet->name, planet->filename ? planet->filename : "(none)",
			planet->starsystem ? planet->starsystem->name : "(none)" );
    strcat( buf, buf1 );
    sprintf ( buf1,
    		FG_CYAN "Xpos: " PLAIN "%-10.0f" FG_CYAN "Ypos: " PLAIN "%-10.0f"
			FG_CYAN "Zpos: " PLAIN "%-10.0f"
    		FG_CYAN "Gravity: " PLAIN "%-5.0f" FG_CYAN "Radius: " PLAIN "%.0f" NL
			FG_CYAN "System Radius : " PLAIN "%-10.0f"
			FG_CYAN "System Radius2: " PLAIN "%-10.0f" NL,
			planet->xpos, planet->ypos, planet->zpos, planet->gravity,
			planet->radius, planet->star_radius, planet->star_radius2 );
    strcat( buf, buf1 );
    sprintf( buf1,
    		FG_CYAN "Base value:  " PLAIN " %10ld" FG_CYAN "/" PLAIN "%-10ld    "
		    FG_CYAN "Population: " PLAIN "%-5d " FG_CYAN "Pop support: " PLAIN "%.3f" NL
		    FG_CYAN "Governed by:" PLAIN " %25s" PLAIN,
			get_taxes(planet), planet->base_value,
			planet->population, planet->pop_support,
			planet->governed_by ? planet->governed_by->name : "(none)" );
    strcat( buf, buf1 );
    strcat( buf, " " FG_CYAN "Flags:      " PLAIN );
    if( IS_SET( planet->flags, PLANET_NOCAPTURE ) )
		strcat( buf, " nocapture" );
    if( IS_SET( planet->flags, PLANET_NOTSHOWN ) )
		strcat( buf, " notshown" );
    if( !IS_SET( planet->flags, PLANET_NOCAPTURE ) && !IS_SET( planet->flags, PLANET_NOTSHOWN ) )
		strcat( buf, "none" );
    /*added by Thanos (wi�zienia) Trog: a gdzie sprawdzanie czy
      first/last->jail istnieje? :-) */
    strcat( buf, NL FG_CYAN "Jail:        " PLAIN  );
    if( !planet->first_jail || !planet->last_jail )
		strcat( buf, "Unspecified." );
    else
    {
    	sprintf( buf1, "%d - %d (%s)",
	    planet->first_jail,
	    planet->last_jail,
	    get_room_index( planet->first_jail )->area->filename    );
	    strcat( buf, buf1 );
    }
	i = 0;
	strcat( buf, NL FG_CYAN "Production and mining" PLAIN NL);
	if ( !planet->deposits.empty() )
		for( auto* pDeposit : planet->deposits )
	{
		sprintf( buf1, FG_CYAN "%d   - Name  : " PLAIN "%-15s"
				FG_CYAN "  Quant.  :" PLAIN "%d" EOL, i,
				pDeposit->material_name,
				pDeposit->daily_mining);
		strcat( buf, buf1 );
		i++;
	}
	else
		strcat( buf, FG_CYAN "      NONE" EOL);
    //done
//    sprintf(buf1,NL
//    FG_CYAN "Import: " PLAIN "%s  " FG_CYAN "x/100: " PLAIN "%d    "
//     FG_CYAN "Export: " PLAIN "%s  " FG_CYAN "Prise: " PLAIN "%d "
//     FG_CYAN "Stock: " PLAIN "%d" FG_CYAN "/" PLAIN "%d" NL,
//               cargo_names[planet->import], planet->prisei, cargo_names[planet->_export], planet->prisee,
// 	      planet->stock, planet->maxstock);
// 	    strcat( buf, buf1 );
    strcat( buf, FG_CYAN "Areas:" EOL );
    for( auto* area : planet->areas )
    {
		if( area )
		{
		    sprintf( buf1,
			FG_CYAN "* " PLAIN "%s " FG_CYAN "(" PLAIN "%s" FG_CYAN ")" EOL,
			    area->name, area->filename );
		    strcat( buf, buf1 );
		}
    }
    strcat( buf, FG_CYAN "Docks:" EOL );
    for ( auto* dock : planet->docks )
    {
       	sprintf( buf1,
			FG_CYAN "%-3d - dock: " PLAIN "%d %s %.0f/%.0f %s" EOL,a,
   			dock->vnum,
			dock->name ? dock->name : "brak",
			check_capacity(dock->vnum), dock->capacity ,
			dock->hidden==1 ?"Ukryty" : "" );
		++a;
		strcat( buf, buf1 );
    }
    send_to_char( buf, ch );

    ch_printf( ch, NL FG_CYAN "Now:   "
	    "Hour: " PLAIN " %d  " FG_CYAN
	    "Day:  " PLAIN " %d  " FG_CYAN
	    "Month:" PLAIN " %d  " NL,
		planet->hour, planet->day, planet->month );
    ch_printf( ch, FG_CYAN "DayLen:   " PLAIN "%d  "
		FG_CYAN "NightLen: " PLAIN "%d  "
		FG_CYAN "MonthLen: " PLAIN "%d" NL,	planet->daylen,
		planet->nightlen, planet->monthlen );
    i=1;
    for( auto* season : planet->seasons )
    {
		ch_printf( ch,
			FG_CYAN "Season%s[" FB_WHITE "%d" FG_CYAN "]: Name: " PLAIN "%-20s  "
			FG_CYAN "DaylenMod: " PLAIN "%d" NL "            "
			FG_CYAN "Temp: " PLAIN "%5d  " FG_CYAN "DiffTemp: " PLAIN "%3d  "
			FG_CYAN "Length:    " PLAIN "%d" NL "            "
			FG_CYAN "Mmhg: " PLAIN "%5d  " FG_CYAN "DiffMmhg: " PLAIN "%3d  "
			FG_CYAN "Windspeed: " PLAIN "%d" NL,
			season == planet->curr_season ? FB_YELLOW "*" FG_CYAN : " ",
			i, *season->name ? season->name : "(none)",
			season->day_length - planet->daylen,
			season->temperature, season->diff_temperature,
			season->length,
			season->mmhg, season->diff_mmhg, season->windspeed );

		i++;
    }
	ch_printf( ch, NL "Versja: %d" NL,planet->version);
	if (planet->description[0]!='\0')
		ch_printf( ch,FB_WHITE "Opis 1:" NL FB_GREEN "%s" NL,planet->description);
	if (planet->description2[0]!='\0')
		ch_printf( ch,FB_WHITE "Opis 2:" NL FB_GREEN "%s" NL,planet->description2);

	return;
}//done

DEF_DO_FUN( makeplanet )
{
    char 		filename	[256];
    PLANET_DATA *	planet;

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeplanet <planet name>\n\r", ch );
	return;
    }

    sprintf( filename, "%s%s", PLANET_DIR, strlower(argument) );

    CREATE( planet, PLANET_DATA, 1 );
    STRDUP( planet->name, argument );
    planet->governed_by 	= NULL;
    planet->starsystem 		= NULL;
	planet->pWarehouse		= NULL;
	planet->flags               = 3;
    planet->import               = 0;
    planet->_export               = 0;
    planet_list.push_back( planet );
}

DEF_DO_FUN( planets )
{
    int 		 count 		= 0;

    for ( auto* planet : planet_list )
    {
        if ( planet->starsystem )
	if ( !IS_SET(planet->flags,PLANET_NOTSHOWN ) || IS_SET(ch->act, PLR_HOLYLIGHT) )
	if ( planet->starsystem->hidden==0 || has_map(planet->starsystem,ch) || IS_SET(ch->act, PLR_HOLYLIGHT ) )
	{
	   pager_printf( ch,
	   FB_WHITE "Planeta: " PLAIN "%-15s         " FB_WHITE "Pod Rz�dami: " PLAIN "%s %s" NL,
                   planet->name ,
                   planet->governed_by ? CLANNAME(planet->governed_by) : "",
                   IS_SET(planet->flags, PLANET_NOCAPTURE ) ? "(nie 'do przej�cia')" : "" );
	   if ( IS_SET(planet->flags,PLANET_NOTSHOWN ) )
	   	pager_printf( ch,"Not shown to players - it is part of the 'Aldegards Solars Sytems'" NL);
           pager_printf( ch,
	   FB_WHITE "Warto��: " PLAIN "%10ld" FB_WHITE "/" PLAIN "%-10d   ",
                   get_taxes(planet) , planet->base_value);
           pager_printf( ch,
	   FB_WHITE "Populacja: " PLAIN "  %-5d " FB_WHITE "Przyrost Poparcia: " PLAIN "%.1f" NL,
                   planet->population , planet->pop_support );
           pager_printf( ch,
	   FB_WHITE "Import:  " PLAIN "%-22s  " FB_WHITE "Eksport:     " PLAIN "%s" NL,
                   !str_cmp(cargo_names[planet->import], "none") ? "brak" :
			    cargo_names[planet->import],
                   !str_cmp(cargo_names[planet->_export], "none") ? "brak" :
			    cargo_names[planet->_export]);
           if ( IS_IMMORTAL(ch) )
           {
          	pager_printf( ch, FB_WHITE "Krainy:  " PLAIN);
          	for ( auto* area : planet->areas )
          	   pager_printf( ch , "%s" NL "         ", area->filename );

	  	if( planet->areas.empty() )	     pager_printf( ch, NL );
    	   }
           pager_printf( ch, NL );

           count++;
	}
    }

    if ( !count )
    {
        send_to_char( "Jako� nie ma na tym mudzie �adnych planet. Dziwne co?" NL, ch );
    }
    return;
}

long get_taxes( PLANET_DATA *planet )
{
      long gain;

      gain = planet->base_value;
      gain += planet->base_value*planet->pop_support/100;
      gain += UMAX(0, planet->pop_support/10 * planet->population);

      return gain;
}

DEF_DO_FUN( imports )
{
   PLANET_DATA *planet;

   if (argument[0] == '\0')
   {
	pager_printf(ch,FB_CYAN "  ________________________________________________________________________" EOL);
	pager_printf(ch,FB_CYAN " / Planeta          )    Import    )  p/c        )   Export     )  cena/T )_." EOL);
	pager_printf(ch,FB_CYAN "|----------------------------------------------------------------------------." EOL);
	for ( auto* planet : planet_list )
   	{
	  if ( !planet->starsystem )
	       continue;
	  if (planet->starsystem->hidden==0 || has_map(planet->starsystem,ch))
	  {
		pager_printf(ch,FB_CYAN "|  %-14s        %s%-12s " FB_CYAN "%-5d          %s%-12s" FB_CYAN "  %-9d ||" EOL,
		planet->name, planet->import!=0 ? FB_RED: "", cargo_names[planet->import],
		planet->prisei, planet->_export!=0 ? FB_RED: "", cargo_names[planet->_export], planet->prisee);
	  }
	}
	pager_printf(ch,FB_CYAN "'----------------------------------------------------------------------------'|" EOL);
	pager_printf(ch,FB_CYAN " '----------------------------------------------------------------------------'" EOL);
	return;
   }

   planet = get_planet( argument );

   if (!planet || (planet->starsystem->hidden!=0 && !has_map(planet->starsystem,ch)))
   {
      send_to_char(FB_RED "No such planet" NL, ch);
      return;
   }
   pager_printf(ch,FB_BLUE "Import i Export dla " FB_WHITE "%s" NL, planet->name);
   pager_printf(ch,FB_BLUE "Import:         " FB_WHITE "%-12s  " FB_BLUE "Export: " FB_WHITE "%s" NL,
              cargo_names[planet->import], cargo_names[planet->_export]);
   pager_printf(ch,FB_BLUE "Procent zwrotu: " FB_WHITE "%-12d  " FB_BLUE "Cena: " FB_WHITE "%d " FB_BLUE "kredyt�w za ton�." NL,
              planet->prisei, planet->prisee);
   pager_printf(ch,FB_BLUE "                              Ilo�� towaru w magazynach: " FB_WHITE "%d " FB_BLUE "ton." EOL,
              planet->stock);
	pager_printf(ch, FB_BLUE "Wydobycie :" EOL);

	for( auto* pDeposit : planet->deposits )
		pager_printf(ch, FB_BLUE "Ruda :" FB_WHITE "%s" EOL,
			pDeposit->material_name);

	pager_printf(ch, NL NL FB_BLUE "Magazyny :" EOL);
	for( auto* pCargo : planet->pWarehouse->cargo )
	{
		pager_printf(ch, FB_BLUE "Minera� : " FB_WHITE "%s" EOL,
			pCargo->pMaterial->name);
		pager_printf(ch, FB_BLUE "Ilo��   : " FB_WHITE "%d" EOL EOL,
			pCargo->amount);
	}

   return;
}

void add_material_warehouse( PLANET_DATA * pPlanet,
			MATERIAL_DATA * pMat,  int quant)
{
	WAREHOUSE_DATA		* pWarehouse;
	bool				fMatch = false;


	if ( !pMat || quant < 0)
		return;
	pWarehouse = pPlanet->pWarehouse;
	if ( !pWarehouse )
	{
		bug( "Planet %s existing without warehouse",pPlanet->name);
		return;
	}

	for( auto* pCargo : pWarehouse->cargo )
	{
		if ( !pCargo->pMaterial )
			continue;
		if ( pCargo->pMaterial == pMat )
		{
			fMatch = true;
			pCargo->amount += quant;
			break;
		}
	}
	if ( !fMatch )
	{
		CARGO_DATA* pCargo = new_cargo();
		pCargo->pMaterial = pMat;
		pCargo->amount = quant;
		pWarehouse->cargo.push_back( pCargo );
	}
}

