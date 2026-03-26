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
*       		Space OLC Module Added by Thanos		   *
****************************************************************************/

#include <math.h>
#include "olc.h"


void pedit_dock( PLANET_DATA *pPlanet, CHAR_DATA *ch, char *argument )
{
    DOCK_DATA *		dock;
    char       		arg1	[ MAX_STRING_LENGTH ];
    char		arg2	[ MAX_STRING_LENGTH ];
    char       		buf 	[ MAX_STRING_LENGTH ];

	argument = one_argument( argument, arg1 );
    	argument = one_argument( argument, arg2 );

    	if ( !str_cmp( arg1, "add" ) )
    	{
		CREATE( dock, DOCK_DATA, 1 );
		sprintf(buf , "L�dowisko");
		STRDUP( dock->name, buf );
		dock->vnum = 0;
		dock->hidden = 0;
		dock->capacity = 1000;
		pPlanet->docks.push_back( dock );
    	}
    	else
    	if ( is_number(arg1) )
    	{
		if( ( dock = get_dock( pPlanet, atoi( arg1) ) ) == NULL )
		{
	    		send_to_char( "No such dock." NL, ch );
	    		return;
		}
		if( !str_cmp( arg2, "name" ) )
			STRDUP( dock->name,argument);
		else
		if( !str_cmp( arg2, "capacity" ) )
		{
	    		dock->capacity = atoi( argument );
		}
		else
		if( !str_cmp( arg2, "vnum" ) )
		{
			if ( is_dock_area( pPlanet , atoi( argument ) ) == false )
			{
			    send_to_char(FB_RED "Dock must be on one of planet's area" NL,ch);
			    return;
			}
	    		dock->vnum = atoi( argument );
		}
		else
		if( !str_cmp( arg2, "hidden" ) )
		{
	    		dock->hidden = atoi( argument );
		}
    		else
    		if ( !str_cmp( arg2, "delete" ) )
    		{
			int 	nr;

			nr = atoi( arg1 );

			if( ( dock = get_dock( pPlanet, nr ) ) == NULL )
			{
	    			send_to_char( "No such dock." NL, ch );
	    			return;
			}

			pPlanet->docks.remove( dock );
			free_dock( dock );
   		}
		else
		{
			send_to_char(FB_RED "dock {nr} name {teks},  vnum {numvnum}, capacity {nr}, hidden {0 lub 1}" NL,ch);
			return;
		}
    	}
	else
	{
		huh(ch);
		return;
	}
	save_planet( pPlanet );
	send_to_char( "Dock set!" NL, ch );
	return;
}

void pedit_deposit( PLANET_DATA *pPlanet, CHAR_DATA *ch, char *argument )
{
	char		arg1	[ MAX_STRING_LENGTH ];
	char		arg2	[ MAX_STRING_LENGTH ];
	DEPOSIT_DATA	* pDeposit;
	MATERIAL_DATA	* pMat;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !str_cmp( arg1, "add" )  )
	{
		pMat = get_material( arg2, U_ORE_FORM );
		if (!pMat)
		{
			ch_printf( ch, "Nie ma takiego materia�u" NL);
			return;
		}
		pDeposit = new_deposit();
		STRDUP( pDeposit->material_name, pMat->name );
		pDeposit->daily_mining = 0;
		pPlanet->deposits.push_back( pDeposit );
	}
	else if ( is_number(arg1) )
	{
		if( ( pDeposit = get_deposit( pPlanet, atoi( arg1) ) ) == NULL )
		{
			send_to_char( "No such deposit." NL, ch );
			return;
		}
		if( !str_cmp( arg2, "name" ) )
		{
			pMat = get_material(argument, U_ORE_FORM);
			if (!pMat)
			{
				ch_printf( ch, "Can't find this material" NL);
				return;
			}
			STRDUP( pDeposit->material_name,pMat->name);
		}
		else if( !str_prefix( arg2, "capacity" )
				|| !str_prefix( arg2, "quantity")  )
		{
			pDeposit->daily_mining = atoi( argument );
		}
		else if(!str_cmp( arg2, "delete")
			   || !str_cmp( arg2, "remove" ) )
		{
			pPlanet->deposits.remove( pDeposit );
			free_deposit(pDeposit);
		}
		else
		{
			huh(ch);
			return;
		}
	}
	else
	{
		huh(ch);
		return;
	}
	save_planet( pPlanet );
	send_to_char( "Deposit set!" NL, ch);
	return;
}

void pedit_warehouse( PLANET_DATA *pPlanet, CHAR_DATA *ch, char *argument )
{
	char		arg1	[ MAX_STRING_LENGTH ];
	char		arg2	[ MAX_STRING_LENGTH ];

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !str_prefix( arg1, "capacity" )
			|| !str_prefix( arg1, "quantity") )
	{
		if ( is_number( arg2 ) )
		{
			pPlanet->pWarehouse->max_capacity = atoi( arg2 );
			send_to_char( "Pojemno�� magazyn�w zmieniona" NL, ch );
			return;
		}
		else
		{
			send_to_char( "Podaj pojemno�� magazyn�w" NL, ch );
			return;
		}
	}
}

bool pedit_create( CHAR_DATA *ch, char *argument )
{
    PLANET_DATA *	pPlanet;
    SEASON_DATA *	season;
    char	 	arg	[MSL];

/* Trog: okazalo sie (chwilowo) bez sensu
    if( get_trust( ch ) < 104 )
    {
	send_to_char( "No permission. Sorry." NL, ch );
	return false;
    }
*/
    if( argument[0] == '\0' )
    {
	send_to_char( "create <filename>" NL, ch );
	return false;
    }

    one_argument( argument, arg );

    CREATE( pPlanet, PLANET_DATA, 1 );
    planet_list.push_back( pPlanet );
    pPlanet->governed_by 	= NULL;
    pPlanet->starsystem 	= NULL ;
    STRDUP( pPlanet->filename, 		arg );
    STRDUP( pPlanet->name, 		"New Planet" );
    STRDUP( pPlanet->description, 	"Planeta jak planeta" );
    STRDUP( pPlanet->description2, 	"Planeta jak planeta" );
    pPlanet->flags              = 0;
    pPlanet->import             = 0;
    pPlanet->_export             = 0;
    pPlanet->daylen		= 12;
    pPlanet->nightlen		= 12;
    pPlanet->monthlen		= 30;

    season = new_season( pPlanet, "no_name" );
    pPlanet->seasons.push_back( season );

    reset_planet( pPlanet );

    ch->desc->olc_editing =   (void *) pPlanet;

    send_to_char( "Planet created." NL, ch );
    return true;
}

SEASON_DATA *get_season( PLANET_DATA *planet, int nr )
{
    int 		i=1;
    SEASON_DATA *	season;

    for( auto* season : planet->seasons )
    {
	if( i == nr )
	    return season;
	i++;
    }

    return NULL;
}

void pedit( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *		ch	= d->character;
    PLANET_DATA *	pPlanet;
    char       		arg	[ MAX_STRING_LENGTH ];
    char       		arg1	[ MAX_STRING_LENGTH ];
    char       		arg2	[ MAX_STRING_LENGTH ];
    char       		buf 	[ MAX_STRING_LENGTH ];
/*
    const char	* const cargo_names[CARGO_MAX] =
    {
  	"none", "all","lommite","Meleenium","Neutronium","Zersium",
  	"steel","rhyll","alazhi"
    };
*/

    if( !(pPlanet = ( PLANET_DATA * ) ch->desc->olc_editing) )
    {
	bug( "null pPlanet." );
	edit_done( ch, (char *)"" );
	return;
    }

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );
/*
    if( get_trust( ch ) < 104 )
    {
	send_to_char( "No permission. Sorry." NL, ch );
	return;
    }
*/
    if ( !pPlanet )
    {
	send_to_char( "No such planet." NL, ch );
	return;
    }


    if ( !str_prefix( arg1, "show" ) )
    {
        sprintf( buf, "%s", pPlanet->name );
        do_showplanet( ch, buf );
        return;
    }


	if( !str_prefix( arg1, "save" )
	|| !str_prefix( arg1, "fold" ) )
	{
		save_planet(pPlanet);
		write_planet_list();
		send_to_char( "Planet saved." NL, ch );
		return;
	}

    GET_DONE( );

    GET_HELP( arg2 );

    SET_STR_EDIT( "desc", pPlanet->description, arg2 )
    SET_STR_EDIT( "desc2", pPlanet->description2, arg2 )


    if ( !str_cmp( arg1, "savelist" ) && get_trust( ch ) >= 104 )
    {
        send_to_char( "Writing planets.lst..." NL, ch );
	write_planet_list( );
	return;
    }

    if ( !str_cmp( arg1, "radius" ) )
    {
	pPlanet->radius = atoi( argument );
	send_to_char( "Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "xpos" ) )
    {
	pPlanet->xpos = atoi( argument );
	pPlanet->ypos = 0;
	pPlanet->zpos = 0;
	pPlanet->star_radius = sqrt( ( pPlanet->xpos * pPlanet->xpos ) +
	                              ( pPlanet->ypos * pPlanet->ypos ) );
	pPlanet->star_radius2 = pPlanet->star_radius*3/4;
	send_to_char( "Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "ypos" ) )
    {
	pPlanet->ypos = atoi( argument );
	pPlanet->star_radius = sqrt( ( pPlanet->xpos * pPlanet->xpos ) +
	                              ( pPlanet->ypos * pPlanet->ypos ) );
	pPlanet->star_radius2 = pPlanet->star_radius*3/4;
	send_to_char( "Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "zpos" ) )
    {
	pPlanet->zpos = atoi( argument );
	send_to_char( "Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "gravity" ) )
    {
	pPlanet->gravity = atoi( argument );
	send_to_char( "Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "name" ) )
    {
	STRDUP( pPlanet->name, argument );
	save_planet( pPlanet );
	send_to_char( "Planet name set." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "governed_by" ) )
    {
        CLAN_DATA *clan;
        clan = get_clan( argument );

        if ( clan )
        {
           pPlanet->governed_by = clan;
           send_to_char( "Clan governed planet set." NL, ch );
        }
        else if (!str_cmp( argument, "NULL" ))
	{
           pPlanet->governed_by = NULL;
           send_to_char( "NULL governed planet set." NL, ch );
	}
	else
           send_to_char( "No such clan." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "starsystem" ) )
    {
        SPACE_DATA *starsystem;

        if ((starsystem=pPlanet->starsystem) != NULL)
          starsystem->planets.remove(pPlanet);
	if ( (pPlanet->starsystem = starsystem_from_name(argument)) )
        {
           starsystem = pPlanet->starsystem;
           starsystem->planets.push_back(pPlanet);
           send_to_char( "Starsystem set." NL, ch );
	}
	else
	       	send_to_char( "No such starsystem." NL, ch );
	return;
    }

    GET_CREATE( pedit_create( ch, arg2 ) );

    if ( !str_cmp( arg1, "filename" ) )
    {
	STRDUP( pPlanet->filename, argument );
	send_to_char( "Filename set." NL, ch );
	save_planet( pPlanet );
	write_planet_list( );
	return;
    }


    if ( !str_cmp( arg1, "first_jail" ) )
    {
 	if( get_room_index( atoi( argument ) ) == NULL )
	{
	    send_to_char( "No such room." NL, ch );
	    return;
	}
    	pPlanet->first_jail = atoi( argument );
	send_to_char( "First jail vnum set." NL, ch );
	return;
    }

	if ( !str_cmp( arg1, "last_jail" ) )
    {
 	if( get_room_index( atoi( argument ) ) == NULL )
	{
	    send_to_char( "No such room." NL, ch );
	    return;
	}
    	pPlanet->last_jail = atoi( argument );
	send_to_char( "Last jail vnum set." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "base_value" ) || !str_cmp( arg1, "basevalue" ) )
    {
	pPlanet->base_value = atoi( argument );
	send_to_char( "Base value set." NL, ch );
	return;
    }

	if ( !str_cmp( arg1, "deposit") )
	{
		pedit_deposit( pPlanet, ch, argument);
		return;
	}
	if ( !str_cmp( arg1, "warehouse") )
	{
		pedit_warehouse( pPlanet, ch, argument );
		return;
	}
	if ( !str_cmp( arg1, "dock" ) )
    {
	pedit_dock( pPlanet, ch, argument );
	return;
    }

     if ( !str_cmp( arg1, "import"))
    {
	int i;
        for (i = 0; i < CARGO_MAX; i++)
        {
    	    if (!str_cmp( argument, cargo_names[i]))
            {
        	pPlanet->import = i;
	        if( i==0 )
		    pPlanet->prisei 	= 0;
	        else
		    pPlanet->prisei	= 100 + i * 10;

		send_to_char("Import set.", ch );
                return;
            }
        }
        send_to_char("Type must be one folow:" NL "none, all, lommite , Meleenium, Neutronium, Zersium, steel," NL "rhyll, alazhi" NL, ch);
        return;
    }

    if ( !str_cmp( arg1, "export"))
    {
	int i;
        for (i = 0; i < CARGO_MAX; i++)
        {
    	    if (!str_cmp( argument, cargo_names[i]) && (i != CARGO_ALL))
            {
		pPlanet->_export = i;
	        if( i==0 )
	        {
	     	    pPlanet->prisee 	= 0;
  		    pPlanet->stock 	= 0;
	        }
	        else
	        {
	     	    pPlanet->prisee 	= 100 + i * 5;
		    pPlanet->stock 	= 10  + i * 90;
		    pPlanet->maxstock 	= 100 + i * 100;
	     }
             send_to_char("Export set." NL, ch );
             return;
          }
       }
       send_to_char("Type must be one folow:" NL "none, all, lommite , Meleenium, Neutronium, Zersium, steel," NL "rhyll, alazhi" NL, ch);
       return;
    }

    if ( !str_cmp( arg1, "prisei" ) )
    {
	pPlanet->prisei = URANGE( 0, atoi(argument) , 1000 );
	send_to_char("Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "prisee" ) )
    {
	if( pPlanet->_export==0 )
	{
	    send_to_char("First set type of export" NL,ch);
	    return;
	}
	pPlanet->prisee = URANGE( 1, atoi(argument) , 5000 );
	send_to_char("Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "stock" ) )
    {
	if (pPlanet->_export==0)
	{
	    send_to_char("First set type of export" NL,ch);
	    return;
	}
	if (pPlanet->maxstock < atoi(argument))
	{
	    send_to_char("Stock overflow. Set equal to maxstock" NL,ch);
	    pPlanet->stock=pPlanet->maxstock;
	    return;
	}
	pPlanet->stock = URANGE( 0, atoi(argument) , 5000000 );
	send_to_char("Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "maxstock" ) )
    {
	pPlanet->maxstock = URANGE( 0, atoi(argument) , 5000000 );
	if (pPlanet->maxstock==0 && pPlanet->stock >0)
	  pPlanet->stock=0;
	send_to_char("Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "flags" ) )
    {
        char farg[MAX_INPUT_LENGTH];

        argument = one_argument( argument, farg);

        if ( farg[0] == '\0' )
        {
           send_to_char( "Possible flags: nocapture notshown" NL, ch );
           return;
        }

        for ( ; farg[0] != '\0' ; argument = one_argument( argument, farg) )
        {
            if ( !str_cmp( farg, "nocapture" ) )
               TOGGLE_BIT( pPlanet->flags, PLANET_NOCAPTURE );
            else
            if ( !str_cmp( farg, "notshown" ) )
               TOGGLE_BIT( pPlanet->flags, PLANET_NOTSHOWN );
            else
               ch_printf( ch , "No such flag: %s" NL , farg );
	}
	send_to_char( "Done." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "reset" ) )
    {
	reset_planet( pPlanet );
	send_to_char( "Done. Planet has been reset." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "hour" ) )
    {
	pPlanet->hour = UMAX( 0, atoi( argument ) );
	send_to_char( "Current hour set." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "day" ) )
    {
	pPlanet->day = UMAX( 1, atoi( argument ) );
	send_to_char( "Current day set." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "month" ) )
    {
	pPlanet->month = UMAX( 0, atoi( argument ) );
	send_to_char( "Current month set." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "daylen" ) )
    {
	pPlanet->daylen = UMAX( 1, atoi( argument ) );
	send_to_char( "Day length on planet set." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "nightlen" ) )
    {
	pPlanet->nightlen = UMAX( 1, atoi( argument ) );
	send_to_char( "Night length on planet set." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "monthlen" ) )
    {
	pPlanet->monthlen = UMAX( 1, atoi( argument ) );
	send_to_char( "Month length on planet set." NL, ch );
	return;
    }


    if ( !str_cmp( arg1, "season" ) )
    {
	SEASON_DATA *	season;
        char arg2	[MAX_INPUT_LENGTH];
        char arg3	[MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if( arg2[0] == '\0' || arg3[0] == '\0' )
	{
	    send_to_char( "Usage: season <nr> <cmd> <value>" NL, ch );
	    send_to_char( "Usage: season add <name>" NL, ch );
	    send_to_char( "Cmd can be:" NL, ch );
	    send_to_char( "   name, temp, difftemp, daylenmod," NL, ch );
	    send_to_char( "   length, mmhg, diffmmhg, delete." NL, ch );
	    return;
	}

	if( !str_cmp( arg2, "add" ) )
	{
	    season = new_season( pPlanet, (const char *)arg3 );
	    pPlanet->seasons.push_back( season );
	    send_to_char( "Season added." NL, ch );
	    return;
	}

	if( (season = get_season( pPlanet, atoi( arg2 ) ) ) == NULL )
	{
	    send_to_char( "No such season." NL, ch );
	    return;
	}

	if( !str_cmp( arg3, "delete" ) )
	{
	    if( pPlanet->seasons.size() <= 1 )
	    {
		send_to_char( "You cannot delete the only season of this planet." NL, ch );
		return;
	    }
	    pPlanet->seasons.remove( season );
	    free_season( season );
	    reset_planet( pPlanet );
	    send_to_char( "Season deleted. Planet has been reset." NL, ch );
	    return;
	}

	if( argument[0] == '\0' )
	{
	    pedit( ch->desc, (char *)"season" );
	    return;
	}

	if( !str_cmp( arg3, "name" ) )
	{
	    STRDUP( season->name, argument );
	    send_to_char( "Season name set." NL, ch );
	    return;
	}

	if( !str_cmp( arg3, "temp" ) )
	{
	    season->temperature 	= atoi( argument );
	    send_to_char( "Season temperature set." NL, ch );
	    return;
	}

	if( !str_cmp( arg3, "windspeed" ) )
	{
	    season->windspeed 	= UMAX( 0, atoi( argument ) );
	    send_to_char( "Season windspeed set." NL, ch );
	    return;
	}

	if( !str_cmp( arg3, "difftemp" ) )
	{
	    season->diff_temperature 	= atoi( argument );
	    send_to_char( "Season temperature differences set." NL, ch );
	    return;
	}

	if( !str_cmp( arg3, "mmhg" ) )
	{
	    season->mmhg 		= atoi( argument );
	    send_to_char( "Season pressure set." NL, ch );
	    return;
	}

	if( !str_cmp( arg3, "diffmmhg" ) )
	{
	    season->diff_mmhg 		= atoi( argument );
	    send_to_char( "Season pressure differences set." NL, ch );
	    return;
	}

	if( !str_cmp( arg3, "daylenmod" ) )
	{
	    season->day_length 		+= atoi( argument );
	    season->night_length 	-= atoi( argument );
	    send_to_char( "Season day length modifier set." NL, ch );
	    return;
	}

	if( !str_cmp( arg3, "length" ) )
	{
	    season->length 		= UMAX( 1, atoi( argument ) );
	    send_to_char( "Season length (in months) set." NL, ch );
	    return;
	}

	pedit( ch->desc, (char *)"season" );
    }
    interpret( ch, arg );
    return;
}




DEF_DO_FUN( pedit )
{
    PLANET_DATA *	pPlanet;
    char 		arg	[MIL];

    if( argument[0] == '\0' )
    {
	send_to_char( "Syntax: pedit [create] <name>" NL, ch );
	return;
    }

    argument = one_argument( argument, arg );

    if ( !str_cmp( arg, "create" ) )
    {
        if( argument[0] == '\0' )
        {
    	    send_to_char( "create <name>" NL, ch );
	    return;
	}
        pedit_create( ch, argument );
	ch->desc->connected = CON_PEDITOR;
	return;
    }
    else
    {
	if( (pPlanet = get_planet( arg ) )!=NULL )
	{
	    ch->desc->olc_editing = ( void * ) pPlanet;
	    ch->desc->connected = CON_PEDITOR;
	    pedit( ch->desc, (char *)"show" );
	    return;
	}
	send_to_char( "No such planet" NL, ch );
    }
    return;
}

DEF_DO_FUN( ssindex )
{
    SPACE_DATA *	starsystem;
    MOON_DATA *		moon;
    PLANET_DATA *	planet;
    STAR_DATA *		star;
    RESET_DATA *	reset;
    int 		stars, moons, planets, resets;

    starsystem = starsystem_from_name( argument );
    if ( starsystem == NULL )
    {
    	send_to_char("No such starsystem." NL,ch);
	return;
    }

    ch_printf( ch,
	FG_CYAN "Name: " FB_WHITE "%s" FG_CYAN " Filename: " FB_WHITE "%s"
	FG_CYAN "  Xpos: " FB_WHITE "%.0f" FG_CYAN " Ypos: " FB_WHITE "%.0f" EOL,
    			starsystem->name,
    			starsystem->filename,
    			starsystem->xpos, starsystem->ypos);
    pager_printf( ch,
	FG_CYAN "Flags: " FB_WHITE "%s" EOL,
		starsystem->flags > 0 ?
		flag_string( ssystem_flags_list, starsystem->flags):
		"none");
    stars = starsystem->stars.size();
    planets = starsystem->planets.size();
    moons = starsystem->moons.size();
    resets = starsystem->resets.size();

    ch_printf( ch, FG_CYAN
	  "Stars:  " FB_WHITE "%d" FG_CYAN "  Planets: " FB_WHITE "%d" FG_CYAN
	"  Moons:  " FB_WHITE "%d" FG_CYAN "  Version: " FB_WHITE "%d" FG_CYAN
	"  Hidden: " FB_WHITE "%d" FG_CYAN "  Resets: " FB_WHITE "%d" EOL,
	    stars, planets, moons,
	    starsystem->version,
	    starsystem->hidden,
	    resets );
    ch_printf( ch, FG_CYAN "ResetMsg:" PLAIN " %s" EOL,
	    *starsystem->resetmsg ? starsystem->resetmsg : "(default)" );
    ch_printf( ch, FG_CYAN "ResetFrequency: " PLAIN "%d" FG_CYAN " minutes.  Age: " PLAIN "%d" NL,
	    starsystem->reset_frequency ? starsystem->reset_frequency : DEF_SPACE_RESETFREQ,
	    starsystem->age );

    ch_printf( ch,FB_WHITE "Desc1:" EOL);
    ch_printf( ch,"%s" NL,starsystem->description);

    ch_printf( ch,FB_WHITE "Desc2:" EOL);
    ch_printf( ch,"%s" NL,starsystem->description2);

    ch_printf( ch,FB_WHITE "Desc3:" EOL);
    ch_printf( ch,"%s" NL,starsystem->description3);
    return;
}

bool ssedit_create( CHAR_DATA *ch, char *argument )
{
    char 	arg		[MAX_INPUT_LENGTH];
    char 	filename	[256];
    SPACE_DATA *starsystem;

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: create <name>" NL, ch );
	return false;
    }

    if( (starsystem_from_name( argument )!=NULL) )
    {
	send_to_char( "There is already a starsystem with that name!" NL, ch );
	return false;
    }

    CREATE( starsystem, SPACE_DATA, 1 );
    starsystem_list.push_back( starsystem );

    STRDUP( starsystem->name, argument );

    starsystem->hidden            = 101;
	starsystem->flags             = 0;
    argument = one_argument( argument, arg );

    sprintf( filename, "%s.system" , strlower(arg) );
    STRDUP( starsystem->filename, filename );
    STRDUP( starsystem->description, filename );
    STRDUP( starsystem->description2, filename );
    STRDUP( starsystem->description3, filename );
    save_starsystem( starsystem );
    write_starsystem_list();
    donemsg(ch);
    return true;
}

void ssedit( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *		ch	= d->character;
    SPACE_DATA *	pStarsystem;
    char       		arg	[ MAX_STRING_LENGTH ];
    char       		arg1	[ MAX_STRING_LENGTH ];
    char       		arg2	[ MAX_STRING_LENGTH ];
    char       		arg3	[ MAX_STRING_LENGTH ];
    char       		buf 	[ MAX_STRING_LENGTH ];
    int			i;

    if( !(pStarsystem = ( SPACE_DATA * ) ch->desc->olc_editing) )
    {
	bug( "null pStarsystem." );
	edit_done( ch, (char *)"" );
	return;
    }

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );

    if ( !str_prefix( arg1, "show" ) )
    {
        sprintf( buf, "%s", pStarsystem->name );
        do_ssindex( ch, buf );
        return;
    }

    GET_DONE( );

    GET_HELP( argument );
    GET_CREATE( ssedit_create( ch, argument ) );

    SET_STR_EDIT( "desc1", pStarsystem->description, argument )
    SET_STR_EDIT( "desc2", pStarsystem->description2, argument )
    SET_STR_EDIT( "desc3", pStarsystem->description3, argument )

    if ( !str_cmp( arg1, "next" ) || !str_cmp( arg1, ">" ) )
    {
	auto it = std::find(starsystem_list.begin(), starsystem_list.end(), pStarsystem);
	if( it == starsystem_list.end() || std::next(it) == starsystem_list.end() )
	{
	    send_to_char( "No next starsystem." NL, ch );
	    return;
	}
	ch->desc->olc_editing = ( void * ) *std::next(it);
	ssedit( ch->desc, (char *)"show" );
	send_to_char( "Editing next starsystem." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "prev" ) || !str_cmp( arg1, "<" ) )
    {
	auto it = std::find(starsystem_list.begin(), starsystem_list.end(), pStarsystem);
	if( it == starsystem_list.end() || it == starsystem_list.begin() )
	{
	    send_to_char( "No prev starsystem." NL, ch );
	    return;
	}
	ch->desc->olc_editing = ( void * ) *std::prev(it);
	ssedit( ch->desc, (char *)"show" );
	send_to_char( "Editing prev starsystem." NL, ch );
	return;
    }

    if( !str_cmp( arg1, "delete" ) )
    {
	int 		count;
	SHIP_DATA *	ship;
	SPACE_DATA *	jump;

	argument = one_argument( argument, arg2 );

	if ( str_cmp( arg2, "yes" )  )
	{
	   send_to_char( "Syntax 'delete yes'. Only if you are sure what you're doing!" NL, ch );
	   return;
	}
        if ( !IS_SET( pStarsystem->flags, STARS_FLAG_TEMP) )
        {
	   send_to_char( "Cannot delete. It is not a 'TEMP' system." NL, ch );
	   return;
	}

        if ( !pStarsystem->ships.empty() )
	{
	   send_to_char( "Cannot delete. There are ships parked in this system." NL, ch );
	   return;
	}

	if( starsystem_list.size() <= 1 )
	{
	   send_to_char( "Cannot delete. It is the last starsystem in galaxy." NL, ch );
	   return;
	}

	{
	    auto it = std::find(starsystem_list.begin(), starsystem_list.end(), pStarsystem);
	    if( it != starsystem_list.begin() )
	        jump = *std::prev(it);
	    else
	        jump = *std::next(it);
	}

	extract_starsystem(pStarsystem);
	write_starsystem_list();
	ch->desc->olc_editing = ( void * ) jump;
	send_to_char( "Starsystem deleted." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "fold" ) )
    {
	save_starsystem( pStarsystem );
	send_to_char( "Starsystem Saved." NL, ch );
	return;
    }

    argument = one_argument( argument, arg2 );

    if ( !str_cmp( arg1, "reset" ) )
    {
	RESET_DATA *	pReset;

	if( !str_cmp( arg2, "list" ) )
	{
	    if( !pStarsystem->resets.empty() )
		display_space_resets( pStarsystem, ch );
	    else
		send_to_char( "Starsystem has no reset" NL, ch );
	    return;
	}
	else
	if( !str_cmp( arg2, "now" ) )
	{
	    send_to_char( "Resetting starsystem." NL, ch );
	    reset_starsystem( pStarsystem );
	    send_to_char( "Ok." NL, ch );
	    return;
    	}
	else
	if( !str_cmp( arg2, "remove" )
	||  !str_cmp( arg2, "delete" ) )
	{
	    int insert_loc;

	    argument = one_argument( argument, arg3 );
	    if( !*arg3 || !is_number( arg3 ) )
	    {
		send_to_char( "Syntax: reset remove <nr>" NL, ch );
		return;
	    }
	    insert_loc = atoi( arg3 );

	    if ( pStarsystem->resets.empty() )
	    {
		send_to_char( "There are no resets in this starsystem." NL, ch );
		return;
	    }

    	    if ( !(pReset=get_space_reset(pStarsystem, insert_loc)) )
    	    {
		send_to_char( "Reset not found." NL, ch );
		return;
	    }

	    delete_space_reset( pStarsystem, pReset );
	    send_to_char( "Reset deleted." NL, ch );
	    return;
	}
	else
	if ( !str_cmp( arg2, "ship" ) )
	{
	    SHIP_INDEX_DATA *	shrec;
	    char       		arg4	[ MAX_STRING_LENGTH ];
	    char       		arg5	[ MAX_STRING_LENGTH ];
	    char       		arg6	[ MAX_STRING_LENGTH ];
	    char       		arg7	[ MAX_STRING_LENGTH ];
	    int			x, y, z, limit;

	    argument = one_argument( argument, arg3 );
	    argument = one_argument( argument, arg4 );
	    argument = one_argument( argument, arg5 );
	    argument = one_argument( argument, arg6 );
	    argument = one_argument( argument, arg7 );

	    if ( arg3[0] == '\0' || !is_number(arg3) )
	    {
    		send_to_char( "Reset which ship vnum?" NL, ch );
    		return;
	    }

	    if ( !(shrec = get_ship_index(arg3)) )
	    {
    		send_to_char( "Ship does not exist." NL, ch );
    		return;
	    }

	    if( arg4[0] == '\0'
	    ||  arg5[0] == '\0'
	    ||  arg6[0] == '\0' )
	    {
    		send_to_char( "Syntax: reset ship <vnum> <x> <y> <z>" NL, ch );
    		return;
	    }

	    if ( arg7[0] == '\0' )
    		limit = 1;
	    else
	    if ( !is_number(arg7) )
	    {
    		send_to_char( "Reset how many ships?" NL, ch );
    		return;
	    }
	    else
    		limit = atoi(arg7);

	    x = atoi( arg4 );
	    y = atoi( arg5 );
	    z = atoi( arg6 );
	    limit = atoi( arg7 );

	    add_space_reset( pStarsystem, 'S', atoi( arg3 ), x, y, z, limit );
	    send_to_char( "Reset added." NL, ch );
	    return;
	}
	else
	{
	    send_to_char( "Syntax: reset list" NL, ch );
	    send_to_char( "        reset ship <vnum> <x> <y> <z>" NL, ch );
	    send_to_char( "        reset remove <nr>" NL, ch );
	    send_to_char( "        reset now" NL, ch );
	    return;
	}
    }

    SET_STR( "name", 	 pStarsystem->name, arg2 );
    SET_STR( "filename", pStarsystem->filename, arg2 );
    SET_STR_NONE( "resetmsg", pStarsystem->resetmsg, arg2 );

    SET_INT( "xpos",  	pStarsystem->xpos, 	atoi( arg2 ) );
    SET_INT( "ypos",  	pStarsystem->ypos, 	atoi( arg2 ) );
    SET_INT( "version", pStarsystem->version, 	atoi( arg2 ) );
    SET_INT( "age", 	pStarsystem->age, 	atoi( arg2 ) );
    SET_INT_LIMIT( "hidden", pStarsystem->hidden, atoi( arg2 ), 0, 100 );

    if ( !str_prefix( arg1, "resetfrequency" ) )
    {
	int freq = atoi( arg2 );

	if( freq < DEF_SPACE_RESETFREQ && freq != -1 )
	{
	    ch_printf( ch, "ResetFreq must be >= %d. Or can be -1 to disable resetting in starsystem." NL, DEF_SPACE_RESETFREQ );
	    return;
	}

	pStarsystem->reset_frequency = freq;
	send_to_char( "ResetFreq set." NL, ch );
	return;
    }

    if ( !str_cmp( arg1, "star" ) )
    {
	STAR_DATA * star;

    	if ( !str_cmp( arg2, "list" ) )
    	{
	    pager_printf( ch, FB_CYAN
	        "[Nr] [        Name        ] [ Xpos  ] [ Ypos  ] [ Zpos   ] [Gravity ] [Radius  ]" EOL );
	    i = 0; for( auto* star : pStarsystem->stars )
	    {
		pager_printf( ch, FG_CYAN
		    "[" FB_CYAN "%2d" FG_CYAN "] [" FB_WHITE "%-20s" FG_CYAN "] [%-7.0f] [%-7.0f] [%-8.0f] [%-8.0f] [%8.0f]" EOL,
			i++, star->name, star->xpos, star->ypos, star->zpos,
			star->gravity, star->radius);
	    }
	    return;
	}
	else
    	if ( !str_cmp( arg2, "add" ) )
    	{
	    if (argument[0] == '\0')
	    {
	       send_to_char( "Syntax: star add <name>" NL, ch );
	       return;
	    }

	    CREATE( star, STAR_DATA, 1 );
	    STRDUP( star->name, argument );
	    star->type = 0;
	    star->xpos = 100000;
	    star->ypos = 0;
	    star->zpos = number_range( -2000, 2000 );
	    star->gravity = 300;
	    star->radius = 1000;
	    star->star_radius = sqrtf(  ( star->xpos * star->xpos ) +
					( star->ypos * star->ypos ) );
	    pStarsystem->stars.push_back( star );
	    send_to_char( "Star added." NL, ch );
    	}
    	else
    	if ( !str_cmp( arg2, "delete" ) )
    	{
    	    int 	nr;

	    argument = one_argument( argument, arg3 );
	    nr = atoi( arg3 );

	    if( ( star = get_star( pStarsystem, nr ) ) == NULL )
	    {
	    	send_to_char( "No such star." NL, ch );
	    	return;
	    }

	    pStarsystem->stars.remove( star );
	    free_star( star );
	    send_to_char( "Star deleted." NL, ch );
   	}
    	else
    	if( is_number( arg2 ) )
    	{
	    argument = one_argument( argument, arg3 );

	    if( ( star = get_star( pStarsystem, atoi( arg2 ) ) ) == NULL )
	    {
		send_to_char( "No such star." NL, ch );
	    	return;
	    }

	    SET_INT_ARG( "type", star->type, atoi( argument ), arg3 );
	    SET_INT_ARG( "radius", star->radius, atoi( argument ), arg3 );

	    if( !str_cmp( arg3, "xpos" ) )
	    {
		star->xpos = atoi( argument );
	    	star->ypos = 0;
		star->star_radius = sqrtf( ( star->xpos * star->xpos ) +
		                           ( star->ypos * star->ypos ) );
		send_to_char( "Xpos set." NL, ch );
		return;
	    }

	    if( !str_cmp( arg3, "ypos" ) )
	    {
		star->ypos = atoi( argument );
		star->star_radius = sqrtf( ( star->xpos * star->xpos ) +
		                           ( star->ypos * star->ypos ) );
		send_to_char( "Ypos set." NL, ch );
		return;
	    }

	    SET_INT_ARG( "zpos", star->zpos, atoi( argument ), arg3 );
	    SET_INT_ARG( "gravity", star->gravity, atoi( argument ), arg3 );
	    SET_STR_ARG( "name", star->name, argument, arg3 );

	    send_to_char( "Syntax: star <star> <field> <value>" NL
			  "Available fields: name, type, xpos, ypos, zpos, gravity, radius" NL,ch);
	    return;
    	}
	else
	{
	    send_to_char( "Syntax: star list" NL, ch );
	    send_to_char( "        star add <name>" NL, ch );
	    send_to_char( "        star <nr> <field> <value>" NL, ch );
	    send_to_char( "        star <delete> <nr>" NL, ch );
	}
	return;
    }

    if ( !str_cmp( arg1, "planet" ) )
    {
	PLANET_DATA * planet;

    	if ( !str_cmp( arg2, "list" ) )
    	{
	    pager_printf( ch, FB_CYAN
	        "[Nr] [        Name        ] [ Xpos  ] [ Ypos  ] [ Zpos   ] [Gravity ] [Radius  ]" EOL );
	    i = 0; for( auto* planet : pStarsystem->planets )
	    {
		pager_printf( ch, FG_CYAN
		    "[" FB_CYAN "%2d" FG_CYAN "] [" FB_WHITE "%-20s" FG_CYAN "] [%-7.0f] [%-7.0f] [%-8.0f] [%-8.0f] [%8.0f]" EOL,
			i++, planet->name,
			planet->xpos, planet->ypos, planet->zpos,
			planet->gravity, planet->radius);
	    }
	    return;
	}
	else
	{
	    send_to_char( "Syntax: planet list" NL, ch );
	    return;
	}
	return;
    }


	if ( !str_cmp( arg1, "flags" ) )
    {

        if ( arg2[0] == '\0' )
        {
           send_to_char( "Possible flags: temp ion meteor EMP" NL, ch );
           return;
        }

            if ( !str_cmp( arg2, "ion" ) )
               TOGGLE_BIT( pStarsystem->flags, STARS_EVENT_ION );
            else
            if ( !str_cmp( arg2, "meteor" ) )
               TOGGLE_BIT( pStarsystem->flags, STARS_EVENT_METEOR );
             else
            if ( !str_cmp( arg2, "temp" ) )
               TOGGLE_BIT( pStarsystem->flags, STARS_FLAG_TEMP );
            else
            if ( !str_cmp( arg2, "EMP" ) )
               TOGGLE_BIT( pStarsystem->flags, STARS_FLAG_EMP );
           else
		   {
               ch_printf( ch , "No such flag: %s" NL , arg2 );
			   return;
		   }
		send_to_char( "Done." NL, ch );
		return;
    }

    if ( !str_cmp( arg1, "moon" ) )
    {
	MOON_DATA * moon;

    	if ( !str_cmp( arg2, "list" ) )
    	{
	    pager_printf( ch, FB_CYAN
	        "Nr[    Name    ][ Xpos  ][ Ypos  ][ Zpos   ][Grav.][Radius ][Vnum][Capacity]Type" EOL );
	    i = 0; for( auto* moon : pStarsystem->moons )
	    {
		pager_printf( ch, FG_CYAN
		    FB_CYAN "%2d" FG_CYAN "[" FB_WHITE "%-12s" FG_CYAN "]"
		    "[%-7.0f][%-7.0f][%-8.0f]"
		    "[%-5.0f][%7.0f]"
		    "[%4d][%3.f/%-4.0f][%-2d]" EOL,
			i++, moon->name,
			moon->xpos, moon->ypos, moon->zpos,
			moon->gravity, moon->radius,
			moon->vnum, check_capacity(moon->vnum), moon->capacity, moon->type );
	    }
	    return;
	}
	else
    	if ( !str_cmp( arg2, "add" ) )
    	{
	    if (argument[0] == '\0')
	    {
	       send_to_char( "Syntax: moon add <name>" NL, ch );
	       return;
	    }

	    CREATE( moon, MOON_DATA, 1 );
	    STRDUP( moon->name, argument );
	    moon->type = 0;
	    moon->xpos = 100000;
	    moon->ypos = 0;
	    moon->zpos = number_range( -2000, 2000 );
	    moon->gravity = 10;
	    moon->vnum = 0;
	    moon->capacity = 0;
	    moon->radius = 30;
	    moon->star_radius = sqrtf( ( moon->xpos * moon->xpos ) +
	                               ( moon->ypos * moon->ypos ) );
	    pStarsystem->moons.push_back( moon );
	    send_to_char( "Moon added." NL, ch );
    	}
    	else
    	if ( !str_cmp( arg2, "delete" ) )
    	{
    	    int 	nr;

	    argument = one_argument( argument, arg3 );
	    nr = atoi( arg3 );

	    if( ( moon = get_moon( pStarsystem, nr ) ) == NULL )
	    {
	    	send_to_char( "No such moon." NL, ch );
	    	return;
	    }

	    pStarsystem->moons.remove( moon );
	    free_moon( moon );
	    send_to_char( "Moon deleted." NL, ch );
   	}
    	else
    	if( is_number( arg2 ) )
    	{
	    argument = one_argument( argument, arg3 );

	    if( ( moon = get_moon( pStarsystem, atoi( arg2 ) ) ) == NULL )
	    {
		send_to_char( "No such moon." NL, ch );
	    	return;
	    }

	    SET_INT_ARG( "type", moon->type, atoi( argument ), arg3 );
	    SET_INT_ARG( "radius", moon->radius, atoi( argument ), arg3 );

	    if( !str_cmp( arg3, "xpos" ) )
	    {
		moon->xpos = atoi( argument );
	    	moon->ypos = 0;
		moon->star_radius = sqrtf( ( moon->xpos * moon->xpos ) +
		                           ( moon->ypos * moon->ypos ) );
		send_to_char( "Xpos set." NL, ch );
		return;
	    }

	    if( !str_cmp( arg3, "ypos" ) )
	    {
		moon->ypos = atoi( argument );
		moon->star_radius = sqrtf( ( moon->xpos * moon->xpos ) +
		                           ( moon->ypos * moon->ypos ) );
		send_to_char( "Ypos set." NL, ch );
		return;
	    }

	    SET_INT_ARG( "zpos", moon->zpos, atoi( argument ), arg3 );
	    SET_INT_ARG( "vnum", moon->vnum, atoi( argument ), arg3 );
	    SET_INT_ARG( "capacity", moon->capacity, atoi( argument ), arg3 );
	    SET_INT_ARG( "type", moon->type, atoi( argument ), arg3 );
	    SET_INT_ARG( "gravity", moon->gravity, atoi( argument ), arg3 );
	    SET_STR_ARG( "name", moon->name, argument, arg3 );

	    send_to_char( "Syntax: moon <star> <field> <value>" NL
			  "Available fields: name, type, xpos, ypos, zpos, gravity, radius, vnum, type, capacity" NL,ch);
	    return;
    	}
	else
	{
	    send_to_char( "Syntax: moon list" NL, ch );
	    send_to_char( "        moon add <name>" NL, ch );
	    send_to_char( "        moon <nr> <field> <value>" NL, ch );
	    send_to_char( "        moon <delete> <nr>" NL, ch );
	}
	return;
    }

    interpret( ch, arg );
    return;
}

DEF_DO_FUN( ssedit )
{
    char            	arg1 [MAX_STRING_LENGTH];
    SPACE_DATA *	starsystem;

    argument = one_argument( argument, arg1 );

    if( !*arg1 )
    {
	send_to_char( 	"Syntax: ssedit <starsystem>" NL
			"        ssedit create <starsystem>" NL
			"        ssedit list" NL, ch );
	return;
    }

    if( !str_cmp( arg1, "create" ) )
    {
	if ( ssedit_create( ch, argument ) )
	    ch->desc->connected = CON_SSEDITOR;
        return;
    }
    else
    if( !str_cmp( arg1, "list" ) )
    {
	int count = 0;

        pager_printf( ch, FB_CYAN
	    "[NR] Name                 [Filename            ]    X       Y     Tmp  Age" EOL );

	for ( auto* starsystem : starsystem_list )
	{
	    count++;
            pager_printf( ch,
		"[%2d] " FB_CYAN "%s%-20s" PLAIN " [%-20s] %-8.0f %-8.0f %s %3d/%3d" EOL,
			count,
			IS_SET( starsystem->flags, STARS_FLAG_TEMP) ? FB_YELLOW : "",
			starsystem->name, starsystem->filename,
			starsystem->xpos, starsystem->ypos,
			IS_SET( starsystem->flags, STARS_FLAG_TEMP) ? "Y" : "N",
			starsystem->age, starsystem->reset_frequency);
	}
        return;
    }
    else
    {
	starsystem = starsystem_from_name( arg1 );
	if ( starsystem == NULL )
	{
    	    send_to_char("No such starsystem." NL,ch);
	    return;
	}

	ch->desc->olc_editing	= (void *) starsystem;
	ch->desc->connected		= CON_SSEDITOR;
	ssedit( ch->desc, (char *)"show" );
	return;
    }
    return;
}

/************************************************************************
 *  S H I P    O L C                     (c) 2003 Thanos
 */

bool can_edit_ship( SHIP_INDEX_DATA *ship, CHAR_DATA *ch )
{
    if( !ch->pcdata )
	return false;

    if( is_name( ch->name, ship->builders ) )
	return true;

    if( ch->pcdata->security >= ship->security )
	return true;

    return false;
}

DEF_DO_FUN( shstat )
{
    char            	arg  [ MAX_INPUT_LENGTH ];
    SHIP_INDEX_DATA *	ship;
    TURRET_DATA *	turret;
    HANGAR_DATA *	hangar;
    SHIPDOCK_DATA *	dock;
    MODULE_DATA *	module;
    int			a;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || !atoi( arg )  )
    {
	send_to_char( "Syntax: shstat <vnum>" NL, ch );
        return;
    }

    if ( !( ship = get_ship_index( arg ) ) )
    {
	send_to_char( "Invalid ship vnum." NL, ch );
        return;
    }

    if( !can_edit_ship( ship, ch ) )
    {
	send_to_char( "No permission. Sorry." NL, ch );
        return;
    }

    pager_printf( ch,
	PLAIN "Vnum: " FB_WHITE "%d  "
	PLAIN "Name: " FB_WHITE "%s  "
	PLAIN "Builders: " FB_WHITE "%s  " EOL
	PLAIN "Security: " FB_WHITE "%d  "
	PLAIN "Class: " FB_WHITE "%s  "
	PLAIN "Type: " FB_WHITE "%s  "
	PLAIN "Filename:  " FB_WHITE "%s" EOL,
		ship->vnum,
		ship->name,
		ship->builders,
		ship->security,
		strlower( bit_name( ship_classes_list, ship->clazz ) ),
		bit_name( ship_types_list, ship->type ),
		ship->filename);

    pager_printf( ch,
	PLAIN "Flags: " FB_WHITE "%s" EOL,
		ship->flags > 0 ?
		flag_string( ship_flags_list, ship->flags) :
		"none");
    pager_printf( ch,
	PLAIN "Size:   " FB_WHITE "%.0f "
	PLAIN "Lev:    " FB_WHITE "%d "
	PLAIN "Version:" FB_WHITE "%d" EOL,
		ship->size,		ship->level,
		ship->version );

    pager_printf( ch,
	PLAIN "TractBeam: " FB_WHITE "%d "
	PLAIN "Comm:      " FB_WHITE "%d "
	PLAIN "Sensor:    " FB_WHITE "%d "
	PLAIN "AstroArray:" FB_WHITE "%d "
	PLAIN "TargetArray:" FB_WHITE "%d " EOL
	PLAIN "Interdict: " FB_WHITE "%d" EOL,
		ship->tractorbeam,	ship->comm,
		ship->sensor,		ship->astro_array,
		ship->target_array,	ship->maxinterdict * 100000 );

    pager_printf( ch,
	PLAIN "Lasers:    " FB_WHITE "%d "
	PLAIN "Ioncannons:" FB_WHITE "%d "
	PLAIN "Missiles:  " FB_WHITE "%d "
	PLAIN "Torpedos:  " FB_WHITE "%d "
	PLAIN "Rockets:   " FB_WHITE "%d" EOL,
    	    ship->lasers,     		ship->ioncannons,
	    ship->maxmissiles,		ship->maxtorpedos,
    	    ship->maxrockets  );

    pager_printf( ch,
	PLAIN "Hull:      " FB_WHITE "%d "
	PLAIN "Shields:   " FB_WHITE "%d "
	PLAIN "Energy:    " FB_WHITE "%d "
	PLAIN "Chaff:     " FB_WHITE "%d "
	PLAIN "Kamuflage: " FB_WHITE "%d" EOL,
	    ship->maxhull,   	    ship->maxshield,
	    ship->maxenergy,   	    ship->maxchaff,
	    ship->maxcloack );

    a = 0; for( auto* module : ship->modules )
    	    a += module->size;
    pager_printf( ch,
	PLAIN "Speed: " FB_WHITE "%.0f "
	PLAIN "Hyperspeed: " FB_WHITE "%.0f "
	PLAIN "Manuever:" FB_WHITE "%d "
	PLAIN "Cargo: " FB_WHITE "%d "
	PLAIN "Trawler: " FB_WHITE "%d "
	PLAIN "MaxMod: " FB_WHITE "%d (%d)" EOL,
	    ship->realspeed, 	ship->hyperspeed,
	    ship->manuever,	ship->maxcargo,
    	    ship->trawler, 	ship->maxmodule, a );


    pager_printf( ch,
	PLAIN "Cockpit:   " FB_WHITE "%-8d "
	PLAIN "Entrance:  " FB_WHITE "%-8d "
	PLAIN "Pilotseat: " FB_WHITE "%-8d "
    	PLAIN "Engineroom:" FB_WHITE "%-8d " EOL,
    		ship->cockpit,		ship->entrance,
    		ship->pilotseat, 	ship->engineroom );

    pager_printf( ch,
	PLAIN "Coseat:    " FB_WHITE "%-8d "
	PLAIN "Navseat:   " FB_WHITE "%-8d "
	PLAIN "Gunseat:   " FB_WHITE "%-8d "
	PLAIN "Shipyard:  " FB_WHITE "%-8d" EOL,
		ship->coseat,  		ship->navseat,
		ship->gunseat,		ship->shipyard );

//    if( ship->first_reset )
//    pager_printf( ch, PLAIN "ResetCount: " FB_WHITE "%d " EOL,  ship->resetcount );

    if( !ship->rooms.empty() )
    {
    pager_printf( ch,
	PLAIN "sRooms: 	" FB_WHITE "%d "
	PLAIN "First:    " FB_WHITE "%d "
	PLAIN "Last:     " FB_WHITE "%d" EOL,
	    ship->roomcount, ship->rooms.front()->vnum,
			     ship->rooms.back()->vnum );
    }
    else
    if( ship->first_dumped )
    {
    pager_printf( ch,
	PLAIN "Rooms: " FB_WHITE "%d "
	PLAIN "Dumped to " FB_WHITE "%d - %d" EOL,
	    ship->roomcount, ship->first_dumped,
			     ship->last_dumped );
    }
    else
    pager_printf( ch,
	PLAIN "Rooms: " FB_WHITE "0" EOL );

    if( !ship->modules.empty() )
    {
	pager_printf( ch, FB_CYAN
	"  type        val    v2     v3   size  st  cost    vnum" EOL );

	a = 0; for( auto* module : ship->modules )
	{
	    pager_printf( ch,
		FB_CYAN "Module %-3d " PLAIN "%-11s %-6d %-6d %-4d %-5d %-3d %-7d %-6d %s" EOL,
			a++, module_type_name[module->type],
			module->value , module->value2,
			module->value3, module->size,
			module->crs*100/module->status,
			module->cost,   module->vnum,
			module->spyname[0] != '\0' ? module->spyname : "N/a" );
	}
    }


    if( !ship->turrets.empty() )
    {
    	pager_printf( ch,
		FB_CYAN "           Vnum     Type  Status  Gunner" EOL );

	a = 0; for( auto* turret : ship->turrets )
	{
	    pager_printf( ch,
		FB_RED "Turret %-3d " PLAIN "%-8d %-5d %-7d %s" EOL,
			a++, turret->vnum, turret->type, turret->status,
			turret->gunner ? turret->gunner->name : "-" );
	}
    }

    if( !ship->hangars.empty() )
    {
	pager_printf( ch,
	    FB_CYAN "           Vnum           Status  Capacity  Type" EOL );
	a = 0; for( auto* hangar : ship->hangars )
	{
	    pager_printf( ch,
		FB_GREEN "Hanger %-3d " PLAIN "%-14d %-7d %.0f/%-6.0f %d" EOL,
			a++, hangar->vnum, hangar->status,
			check_capacity( hangar->vnum ),
			hangar->capacity, hangar->type );

	}
    }

    if( !ship->docks.empty() )
    {
	pager_printf( ch, FB_CYAN "           Type           Vnum" EOL );
	a = 0; for( auto* dock : ship->docks )
	{
	    pager_printf( ch,
	    FB_CYAN "Dock %-3d" PLAIN "   %-14d %-7d %s %-9d %s" EOL,
			a++, dock->type, dock->vnum,
			dock->target != NULL ? dock->target->name : "N.D.",
			dock->targetvnum, dock->master_slave == -1 ? "" :
			(dock->master_slave == 0 ? "Slave" : "Master"));

	}
    }

    pager_printf( ch, PLAIN "Description:" NL "%s" EOL,
	    *ship->description ? ship->description : "(none)" );

}


void start_shedit( CHAR_DATA *ch, SHIP_INDEX_DATA *ship )
{
    if( !can_edit_ship( ship, ch ) )
    {
	send_to_pager( "No permission. Sorry." NL, ch );
        return;
    }

    ch->desc->olc_editing   = (void *)ship;
    ch->desc->connected = CON_SHEDITOR;
    shedit( ch->desc, (char *)"show" );
    ch_printf( ch, "Editing %s." NL, ship->name );
    return;
}

void shedit_done( CHAR_DATA *ch )
{
    edit_done( ch, (char *)"" );
    return;
}


/*
 * Zrzuca pokoje statku (sRooms) do jakiej� fizycznej krainki, by
 * mo�naby�o je edytowa�. Kiedy builder ko�czy edycj� pokoi, u�ywa
 * roomcopy, by pobra� zmienione lokacje zn�w do statku.
 */
int shedit_roomdump( CHAR_DATA *ch, SHIP_INDEX_DATA *ship )
{
    SHIP_ROOM_DATA *	sRoom;
    ROOM_INDEX_DATA *	pRoom;
    int			start;
    int			vector;

    if( !(start = search_free_vnums( shipTmpArea, ship->roomcount )) )
    {
	send_to_char( "Not enough free vnums in ship_area to edit ship!" NL, ch );
	return 0;
    }
    vector = ship->rooms.front()->vnum;

    log_string( "starting ship_room dumping..." );

    for( auto* sRoom : ship->rooms )
    {
	EXTRA_DESCR_DATA *	sEd;
	EXTRA_DESCR_DATA *	ed;
	MPROG_DATA *		sPrg;
	MPROG_DATA *		pPrg;
	RESET_DATA *		sRes;

	pRoom 		= make_room( start+(sRoom->vnum-vector), 0 );
	pRoom->area	= shipTmpArea;

	if( ship->cockpit 	== sRoom->vnum )
	    ship->cockpit 	=  pRoom->vnum;
	if( ship->entrance 	== sRoom->vnum )
	    ship->entrance 	=  pRoom->vnum;
	if( ship->engineroom 	== sRoom->vnum )
	    ship->engineroom 	=  pRoom->vnum;
	if( ship->pilotseat 	== sRoom->vnum )
	    ship->pilotseat 	=  pRoom->vnum;
	if( ship->coseat 	== sRoom->vnum )
	    ship->coseat 	=  pRoom->vnum;
	if( ship->navseat 	== sRoom->vnum )
	    ship->navseat 	=  pRoom->vnum;
	if( ship->gunseat 	== sRoom->vnum )
	    ship->gunseat 	=  pRoom->vnum;

    	STRDUP( pRoom->name, 		sRoom->name 		);
	STRDUP( pRoom->description, 	sRoom->description 	);

	pRoom->light		= sRoom->light;
	pRoom->sector_type 	= sRoom->sector_type;
	pRoom->tele_vnum	= sRoom->tele_vnum;
	pRoom->tele_delay	= sRoom->tele_delay;
	pRoom->tunnel		= pRoom->tunnel;
	pRoom->room_flags  	= sRoom->room_flags;
	REMOVE_BIT( pRoom->room_flags, 	ROOM_PLR_HOME );

    	//eds
	for( auto* sEd : sRoom->extradesc )
	{
	    auto* ed = SetRExtra( pRoom, sEd->keyword );
	    STRDUP( ed->description, sEd->description );
	}

	//progs
	for( auto* sPrg : sRoom->mudprogs )
	{
	    MPROG_DATA* pPrg;
	    CREATE( pPrg, MPROG_DATA, 1 );
	    pPrg->type		= sPrg->type;
	    pRoom->progtypes 	= pRoom->progtypes | pPrg->type;
	    STRDUP( pPrg->comlist, sPrg->comlist );
	    STRDUP( pPrg->arglist, sPrg->arglist );
	    pRoom->mudprogs.push_back( pPrg );
	}

	for( auto* sRes : sRoom->resets )
	{
	    // szybka konwersja reset�w
	    switch( sRes->command )
	    {
		default:
		    break;
		case 'M':
		case 'C':
		case 'O':
		    sRes->arg3=pRoom->vnum;
		    break;
		case 'D':
		case 'R':
		    sRes->arg1=pRoom->vnum;
		    break;
	    }

	    add_reset( pRoom, sRes->command, sRes->extra,
			sRes->arg1, sRes->arg2, sRes->arg3 );
	}
    }

    // drzwi generujemy w oddzielnej petli -- mamy wiec pewnosc, ze
    // wszystkie potrzebne lokacje zostaly juz stworzone
    for( auto* sRoom : ship->rooms )
    {
	for( auto* sExit : sRoom->exits )
	{
	    EXIT_DATA *pExit;
	    pExit = make_exit( 	get_room_index( start+(sRoom->vnum-vector) ),
				get_room_index( start+(sExit->vnum-vector) ),
				sExit->vdir );

    	    STRDUP( pExit->keyword, 		sExit->keyword );
    	    STRDUP( pExit->description, 	sExit->description );
    	    pExit->key			= 	sExit->key;
    	    pExit->orig_flags		= 	sExit->flags;
    	    pExit->flags		= 	pExit->orig_flags;
	}
    }

    ship->first_dumped		= start;
    ship->last_dumped		= start+ship->roomcount-1;

/*    for( res = ship->first_reset; res; res = res->next )
    {
	if( res->command == 'M' )
        place_reset( shipTmpArea, res->command,
	    res->extra, res->arg1, res->arg2, start+(res->arg3-vector));
	else
	if( res->command == 'O' )
        place_reset( shipTmpArea, res->command,
	    res->extra, res->arg1, res->arg2, start+(res->arg3-vector));
	else
        place_reset( shipTmpArea, res->command,
	    res->extra, res->arg1, res->arg2, res->arg3);
    }
*/
    send_to_char( "Ok." NL, ch );
    return start;
}


/*
 * Ci�gnie z fizycznej krainy okre�lon� ilo�� lokacji z resetami,
 * extradescriptionami, progami i innymi bzdetami.
 */
void ship_room_copy( SHIP_INDEX_DATA *shrec, int start, int end )
{
    int i;
    ROOM_INDEX_DATA *	pRoom;
    SHIP_ROOM_DATA *	sRoom;
    RESET_DATA *	res;
    RESET_DATA *	res_next;

    for( i = start; i <= end; i++ )
    {
	SHIP_EXIT_DATA *	sExit;
	EXIT_DATA *		pExit;
	EXTRA_DESCR_DATA *	sEd;
	EXTRA_DESCR_DATA *	pEd;
	MPROG_DATA *		pProg;
	MPROG_DATA *		sProg;

	if( !(pRoom = get_room_index( i ) ) )
	    continue;

	CREATE( sRoom, SHIP_ROOM_DATA, 1 );
	top_sroom++;
	STRDUP( sRoom->name, 		pRoom->name 		);
	STRDUP( sRoom->description, 	pRoom->description 	);
	sRoom->vnum 		= 	pRoom->vnum;
	sRoom->room_flags 	= 	pRoom->room_flags;
	sRoom->progtypes	= 	pRoom->progtypes;
	sRoom->light		= 	pRoom->light;
	sRoom->sector_type	= 	pRoom->sector_type;
	sRoom->tele_vnum	= 	pRoom->tele_vnum;
	sRoom->tele_delay	= 	pRoom->tele_delay;
	sRoom->tunnel		= 	pRoom->tunnel;

	for( auto* pExit : pRoom->exits )
	{
	    CREATE( sExit, SHIP_EXIT_DATA, 1 );
	    STRDUP( sExit->keyword, 	pExit->keyword 		);
	    STRDUP( sExit->description,	pExit->description 	);
	    sExit->vnum		=	pExit->vnum;
	    sExit->rvnum	=	pExit->rvnum;
	    sExit->key		=	pExit->key;
	    sExit->vdir		=	pExit->vdir;
	    sExit->distance	=	pExit->distance;
	    sExit->flags	= 	pExit->orig_flags;
	    sRoom->exits.push_back( sExit );
	}

	for( auto* pEd : pRoom->extradesc )
	{
	    CREATE( sEd, EXTRA_DESCR_DATA, 1 );
	    STRDUP( sEd->keyword, 	pEd->keyword 		);
	    STRDUP( sEd->description, 	pEd->description 	);
	    sRoom->extradesc.push_back( sEd );
	}

	for( auto* pProg : pRoom->mudprogs )
	{
	    MPROG_DATA* sProg;
	    CREATE( sProg, MPROG_DATA, 1 );
	    sProg->type 	= 	pProg->type;
	    STRDUP( sProg->arglist, pProg->arglist );
	    STRDUP( sProg->comlist, pProg->comlist );
	    sRoom->mudprogs.push_back( sProg );
	}


	//resety wrzucamy zn�w na statek
	{
	    auto snapshot = pRoom->resets;
	    for( auto* res : snapshot )
	    {
	        pRoom->resets.remove( res );
	        sRoom->resets.push_back( res );
	        --top_reset;
	    }
	}


	shrec->roomcount++;
	shrec->rooms.push_back( sRoom );
    }

    return;
}


bool shedit_create( CHAR_DATA *ch, char *argument )
{
    SHIP_INDEX_DATA *	pShrec;
    char 	    	arg1[MAX_INPUT_LENGTH];
    char 	    	arg2[MAX_INPUT_LENGTH];
    int               	value;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || (value = atoi( arg1 ))==0 )
    {
	send_to_char( "Sk�adnia: create <vnum>" NL, ch );
	return false;
    }

    if ( get_ship_index( arg1 ) )
    {
	send_to_char( "Taki prototyp statku ju� istnieje." NL, ch );
	return false;
    }

    pShrec		= new_ship_index( );

    if ( !pShrec )
    {
	send_to_char( "Error." NL, ch );
	log_string( "Shedit_create: new_ship_index failed." );
	return false;
    }

    pShrec->vnum	= value;
    ship_index_list.push_back( pShrec );

    start_shedit( ch, pShrec );
    send_to_char( "Prototyp statku stworzony." NL, ch );
    return true;
}

DEF_DO_FUN( shedit )
{
    SHIP_INDEX_DATA *	shrec;
    char             	arg1 [MAX_STRING_LENGTH];

    argument = one_argument( argument, arg1 );

    if( !*arg1 )
    {
        send_to_char( "Syntax: shedit [create] <vnum>." NL, ch );
	return;
    }

    if ( !(shrec=get_ship_index(arg1)) )
    {
        if ( !str_cmp( arg1, "create" ) )
	{
	    if ( shedit_create( ch, argument ) )
	    {
		return;
	    }
	}
	else
	{
	    send_to_char( "That ship prototype does not exist." NL, ch );
	    return;
	}
    }

    start_shedit( ch, shrec );
    return;
}

SHIP_INDEX_DATA *get_ship_index_fn( char *fn )
{
    SHIP_INDEX_DATA *    shp;

    for( auto* shp : ship_index_list )
    {
	if( !str_cmp( shp->filename, fn ) )
	    return shp;
    }
    return NULL;
}

void set_ship_filename( SHIP_INDEX_DATA *ship, char *filename )
{
    char	buf	[MSL];

    if( str_suffix( ".shp", filename ) )
    {
        char	_buf	[MSL];
	sprintf( _buf, "%s.shp", filename );
	sprintf( buf, "%s", _buf );
    }
    else
	sprintf( buf, "%s", filename );

    while( get_ship_index_fn( buf ) )
    {
        char	_buf	[MSL];
	int	i;

	for( i = 0; i < SLEN( buf )-4; i++ )
	    _buf[i] = buf[i];

	strcat( _buf, "1.shp" );
	strcpy( buf, _buf );
    }
    STRDUP( ship->filename, buf );
    return;
}

void shedit( DESCRIPTOR_DATA *d, char *argument )
{
    CHAR_DATA *		ch = d->character;
    SHIP_INDEX_DATA *	shrec;
    char              	arg	[ MAX_STRING_LENGTH ];
    char              	arg1	[ MAX_STRING_LENGTH ];
    char              	arg2	[ MAX_STRING_LENGTH ];
    char              	buf	[ MAX_STRING_LENGTH ];
    int			value;

    strcpy( arg, argument );
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    if( !(shrec = ( SHIP_INDEX_DATA * ) ch->desc->olc_editing) )
    {
	shedit_done( ch );
	return;
    }

    if( !can_edit_ship( shrec, ch ) )
    {
	send_to_char( "No permission. Sorry." NL, ch );
        return;
    }

    if ( !str_prefix( arg1, "show" ) )
    {
        sprintf( buf, "%d", shrec->vnum );
	do_shstat( ch, buf );
        return;
    }

    if ( !str_prefix( arg1, "savelist" ) )
    {
	save_ship_index_list( );
	send_to_char( "Ok." NL, ch );
	return;
    }

    if ( !str_prefix( arg1, "done" ) )
    {
	shedit_done( ch );
	return;
    }

    GET_HELP( arg2 );
    GET_CREATE( shedit_create( ch, arg2 ) );

    if ( !str_prefix( arg1, "roomdump" ) )
    {
	int	svnum, evnum;

	if( shrec->rooms.empty() )
	{
	    send_to_char( "This ship has no location yet!!!" NL, ch );
	    return;
	}
	if( !(svnum = shedit_roomdump( ch, shrec ) ) )
	{
	    send_to_char( "Ups. An error. Contact with coder :>" NL, ch );
	    return;
	}
	evnum = svnum + shrec->roomcount -1;
	free_srooms( shrec );
	shrec->roomcount = 0;
	ch_printf( ch,
	"Ok. Ship copied to rooms: " FB_WHITE "%d - %d" PLAIN ". You can now set the " NL
	"Navroom, Engineroom, Cockpit etc. " NL,
	    svnum, evnum );
	return;
    }

    if ( !str_prefix( arg1, "roomcopy" ) )
    {
	int	vstart, vend;

        argument = one_argument( argument, arg1 );
        argument = one_argument( argument, arg2 );

	if( !shrec->rooms.empty()
	&&( !*argument || str_cmp( argument, "sure" ) ) )
	{
	    send_to_char(
	    "Warning!!! Your ship has room(s)!" NL
	    "Use 'roomcopy <start> <end> sure' if you want to add new rooms" NL
	    "without deleting old ones. Or use 'roomdump' to free'em first." NL, ch );
	    return;
	}

	if( !str_cmp( arg1, "back" ) )
	{
	    char	str	[MIL];

	    sprintf( str, "roomcopy %d %d", shrec->first_dumped,
					    shrec->last_dumped );
	    shedit( d, str );
	    return;
	}

        if ( !is_number( arg1 ) || arg1[0] == '\0' )
        {
    	    send_to_char( "Syntax: roomcopy <start> <end>" NL, ch );
    	    send_to_char( "Syntax: roomcopy back" NL, ch );
            return;
        }

	vstart 	= atoi( arg1 );

        if( !is_number( arg2 ) || arg2[0] == '\0' )
	    vend	= vstart;
	else
	    vend 	= atoi( arg2 );

	ship_room_copy( shrec, vstart, vend );
	send_to_char( "Ok." NL, ch );
	return;
    }

    if ( !str_prefix( arg1, "oldcopy" ) )
    {
	SHIP_DATA *	ship;

	if( !*arg2 )
	{
	    send_to_char( "Syntax: oldcopy <oldship_name>." NL, ch );
	    return;
	}

	if( !(ship = get_ship( arg2 )) )
	{
	    send_to_char( "No such ship." NL, ch );
	    return;
	}

	STRDUP( shrec->name, 		ship->name 	);
	set_ship_filename( shrec, 	ship->filename 	);
	STRDUP( shrec->description, 	ship->description );
	shrec->type		=	ship->type;
	shrec->clazz		=	ship->clazz;
	shrec->size		=	ship->size;
	shrec->level		=	ship->level;
	shrec->version		=	ship->version;
	shrec->tractorbeam	=	ship->tractorbeam;
	shrec->comm		=	ship->comm;
	shrec->sensor		=	ship->sensor;
	shrec->astro_array	=	ship->astro_array;
	shrec->target_array	=	ship->target_array;
	shrec->maxinterdict	=	ship->maxinterdict;
	shrec->lasers		=	ship->lasers;
	shrec->ioncannons	=	ship->ioncannons;
	shrec->maxmissiles	=	ship->maxmissiles;
	shrec->maxtorpedos	=	ship->maxtorpedos;
	shrec->maxrockets	=	ship->maxrockets;
	shrec->maxhull		=	ship->maxhull;
	shrec->maxshield	=	ship->maxshield;
	shrec->maxenergy	=	ship->maxenergy;
	shrec->maxchaff		=	ship->maxchaff;
	shrec->maxcloack	=	ship->maxcloack;
	shrec->realspeed	=	ship->realspeed;
	shrec->manuever		=	ship->manuever;
	shrec->maxcargo		=	ship->maxcargo;
	shrec->trawler		=	ship->trawler;
    	shrec->maxmodule	=	ship->maxmodule;
		shrec->flags	=	ship->flags;

	shrec->entrance		=	VNUM( ship->entrance 	);
	shrec->navseat		=	VNUM( ship->navseat 	);
	shrec->pilotseat	=	VNUM( ship->pilotseat	);
	shrec->coseat		=	VNUM( ship->coseat	);
	shrec->gunseat		=	VNUM( ship->gunseat	);
	shrec->cockpit		=	VNUM( ship->cockpit	);
	shrec->engineroom	=	VNUM( ship->engineroom	);

	ship_room_copy( shrec, ship->firstroom, ship->lastroom );
	send_to_char( "OldShip (with rooms) copied. Check the filename." NL, ch );
	return;
    }

    if ( !str_prefix( arg1, "next" ) || !str_cmp( arg1, ">" ))
    {
	SHIP_INDEX_DATA *pSNext;

	if( (pSNext = get_ship_index( itoa(shrec->vnum+1) )) == NULL )
	    send_to_char( "I cannot see next vnum ship" NL, ch );
	else
	{
	    shedit_done( ch );
	    start_shedit( ch, pSNext );
	}
	return;
    }

    if ( !str_prefix( arg1, "previous" ) || !str_cmp( arg1, "<" ) )
    {
	SHIP_INDEX_DATA *pSPrev;

	if( (pSPrev = get_ship_index( itoa(shrec->vnum-1) )) == NULL )
	    send_to_char( "I cannot see prev vnum ship" NL, ch );
	else
	{
	    shedit_done( ch );
	    start_shedit( ch, pSPrev );
	}
	return;
    }

    if ( ( value = flag_value(ship_types_list, arg ) ) != -1 )
    {
	shrec->type = value;
	ch_printf( ch,"Ship type set to %s." NL, bit_name(ship_types_list,value ) );
	return;
    }

    if ( ( value = flag_value(ship_classes_list, arg ) ) != -1 )
    {
	shrec->clazz = value;
	ch_printf( ch,"Ship class set to %s." NL, bit_name(ship_classes_list,value ) );
	return;
    }

    if ( !str_prefix( arg1, "builder" ) )
    {
        argument = one_argument( argument, arg2 );

        if ( arg2[0] == '\0' )
        {
            send_to_char( "Syntax:  builder <name>" NL, ch );
            return;
        }

	toggle_string( &shrec->builders, arg2 );

	if( is_name( arg2, shrec->builders ) )
            send_to_char( "Builder added." NL, ch );
	else
            send_to_char( "Builder removed." NL, ch );

	return;

    }

    if ( !str_prefix( arg1, "vnum" ) )
    {
        if( arg2[0] == '\0' || !(value=atoi(arg2)) )
        {
	    send_to_char( "Syntax:  vnum <new_vnum>" NL, ch );
            return;
        }

	if( get_ship_index( arg2 ) )
        {
	    send_to_char( "That vnum is busy right now." NL, ch );
            return;
        }

	shrec->vnum = atoi( arg2 );
	send_to_char( "Vnum set." NL, ch );
        return;
    }

    SET_STR( "name", 		shrec->name, 			arg2 );
    SET_INT_LIMIT( "security", 	shrec->security, 	  atoi( arg2 ),
			     0, ch->pcdata->security );
    SET_STR( "builders", 	shrec->builders, 		arg2 );

    if ( !str_prefix( arg1, "filename" ) )
    {
	set_ship_filename( shrec, arg2 );
    	send_to_char( "Filename set." NL, ch );
	return;
    }

	if ( !str_cmp( arg1, "flags" ) )
    {

        if ( arg2[0] == '\0' )
        {
           send_to_char( "Mo�liwe flagi: metaaggressive aggressive nowander wimpy hidden descless done" NL, ch );
           return;
        }

            if ( !str_cmp( arg2, "aggressive" ) )
               TOGGLE_BIT( shrec->flags, SHIP_AGGRESSIVE );
            else
            if ( !str_cmp( arg2, "hidden" ) )
               TOGGLE_BIT( shrec->flags, SHIP_HIDDEN );
             else
            if ( !str_cmp( arg2, "nowander" ) )
               TOGGLE_BIT( shrec->flags, SHIP_NOWANDER );
            else
            if ( !str_cmp( arg2, "metaaggressive" ) )
               TOGGLE_BIT( shrec->flags, SHIP_METAAGGRESSIVE );
            else
            if ( !str_cmp( arg2, "wimpy" ) )
               TOGGLE_BIT( shrec->flags, SHIP_WIMPY );
 			else
            if ( !str_cmp( arg2, "descless" ) )
               TOGGLE_BIT( shrec->flags, SHIP_DESCLESS );
            else
            if ( !str_cmp( arg2, "done" ) )
               TOGGLE_BIT( shrec->flags, SHIP_DONE );
			else
		   	{
				ch_printf( ch , "No such flag: %s" NL , arg2 );
				return;
		   	}
		send_to_char( "Done." NL, ch );
		return;
    }

    SET_STR_EDIT( "description",shrec->description, 		arg2 );
    SET_INT( "size", 		shrec->size, 		atoi( arg2 ) );
    SET_INT( "level", 		shrec->level,		atoi( arg2 ) );
    SET_INT( "version",		shrec->version,		atoi( arg2 ) );
    SET_INT( "tractbeam",	shrec->tractorbeam,	atoi( arg2 ) );
    SET_INT( "tractorbeam",	shrec->tractorbeam,	atoi( arg2 ) );
    SET_INT( "comm",		shrec->comm,		atoi( arg2 ) );
    SET_INT( "sensor",		shrec->sensor,		atoi( arg2 ) );
    SET_INT( "astroarray",	shrec->astro_array,	atoi( arg2 ) );
    SET_INT( "targetarray",	shrec->target_array,	atoi( arg2 ) );
    SET_INT( "interdict",	shrec->maxinterdict,atoi( arg2 )/100000 );
    SET_INT( "lasers",		shrec->lasers,		atoi( arg2 ) );
    SET_INT( "ioncannons",	shrec->ioncannons,	atoi( arg2 ) );
    SET_INT( "hull",		shrec->maxhull,		atoi( arg2 ) );
    SET_INT( "shields",		shrec->maxshield,	atoi( arg2 ) );
    SET_INT( "energy",		shrec->maxenergy,	atoi( arg2 ) );
    SET_INT( "chaff",		shrec->maxchaff,	atoi( arg2 ) );
    SET_INT( "cloack",		shrec->maxcloack,	atoi( arg2 ) );
    SET_INT( "camuflage",	shrec->maxcloack,	atoi( arg2 ) );
    SET_INT( "camouflage",	shrec->maxcloack,	atoi( arg2 ) );
    SET_INT( "speed",		shrec->realspeed,	atoi( arg2 ) );
    SET_INT( "hyperspeed",	shrec->hyperspeed,	atoi( arg2 ) );
    SET_INT( "manuever",	shrec->manuever,	atoi( arg2 ) );
    SET_INT( "cargo",		shrec->maxcargo,	atoi( arg2 ) );
    SET_INT( "trawler",		shrec->trawler,		atoi( arg2 ) );
    SET_INT( "maxmodule",	shrec->maxmodule,	atoi( arg2 ) );
    SET_INT( "missiles",	shrec->maxmissiles,	atoi( arg2 ) );
    SET_INT( "torpedos",	shrec->maxtorpedos,	atoi( arg2 ) );
    SET_INT( "rockets",		shrec->maxrockets,	atoi( arg2 ) );

    SET_INT( "entrance",	shrec->entrance,	atoi( arg2 ) );
    SET_INT( "cockpit",		shrec->cockpit,		atoi( arg2 ) );
    SET_INT( "navseat",		shrec->navseat,		atoi( arg2 ) );
    SET_INT( "pilotseat",	shrec->pilotseat,	atoi( arg2 ) );
    SET_INT( "coseat",		shrec->coseat,		atoi( arg2 ) );
    SET_INT( "engineroom",	shrec->engineroom,	atoi( arg2 ) );
    SET_INT( "gunseat",		shrec->gunseat,		atoi( arg2 ) );

    if ( !str_prefix( arg1, "fold" ) )
    {
	save_ship_index( shrec );
	send_to_char( "Ok." NL, ch );
	return;
    }

    interpret( ch, arg );
    return;
}
