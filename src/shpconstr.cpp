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
*			Ship construction module			   *
****************************************************************************/

#include <sys/types.h>
#include "mud.h"

void prepare_ship		args( ( SHIP_DATA *ship ) ); //space.c
void reset_room			args( ( ROOM_INDEX_DATA *pRoom ) ); //reset.c
void create_ship_in_room	args( ( SHIP_INDEX_DATA *shrec, ROOM_INDEX_DATA *pRoomIndex ) );//space_reset.c

extern AREA_DATA *		shipTmpArea;

/* zwraca wskaznik na rekord w bazie o nazwie ship_index */
SHIP_INDEX_DATA * get_ship_index( char * ship_index )
{
    if( is_number( ship_index ) )
    {
	int vnum = atoi( ship_index );

	for( auto* shrec : ship_index_list )
	    if( shrec->vnum == vnum )
		return shrec;
    }
    else
    {
	for( auto* shrec : ship_index_list )
	    if( nifty_is_name_prefix( ship_index, shrec->name ) )
		return shrec;
    }
    return NULL;
}

/* to co get_room_index, ale szuka w statku i po svnumach */
ROOM_INDEX_DATA *get_sroom( SHIP_DATA *ship, int svnum )
{
    for( auto* room : ship->locations )
    {
	if( !room->vnum && room->svnum == svnum )
	    return room;

	if( !room->svnum && room->vnum == svnum )
	    return room;
    }
    return NULL;
}

/* przerabia SHIP_ROOM_DATA na ROOM_INDEX_DATA (vnumy = 0) */
void generate_ship_rooms( SHIP_DATA *ship, SHIP_INDEX_DATA *shrec )
{
    ROOM_INDEX_DATA *	pRoom;

    for( auto* sRoom : shrec->rooms )
    {
	EXTRA_DESCR_DATA *ed;
	MPROG_DATA     *pPrg;

	pRoom                   = make_room( 0, sRoom->vnum );
	pRoom->area		= shipTmpArea;

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
	    ed = SetRExtra( pRoom, sEd->keyword );
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
	    add_reset( pRoom, sRes->command, sRes->extra,
			sRes->arg1, sRes->arg2, sRes->arg3 );
	}

        ship->locations.push_back(pRoom);
	pRoom->ship = ship;
    }

    // drzwi generujemy w oddzielnej petli -- mamy wiec pewnosc, ze
    // wszystkie potrzebne lokacje zostaly juz stworzone
    for( auto* sRoom : shrec->rooms )
    {
	EXIT_DATA        *pExit;

	for( auto* sExit : sRoom->exits )
	{
	    pExit = make_exit( 	get_sroom( ship, sRoom->vnum ),
				get_sroom( ship, sExit->vnum ),
				sExit->vdir );

    	    STRDUP( pExit->keyword, 		sExit->keyword );
    	    STRDUP( pExit->description, 	sExit->description );
    	    pExit->key			= 	sExit->key;
    	    pExit->orig_flags		= 	sExit->flags;
    	    pExit->flags		= 	sExit->flags;
	}
    }
}

/*
   funkcja konstruuje statek. Posluguje sie 'schematem' zawartym w pliku
   ship.db
 */
SHIP_DATA *constr_ship( SHIP_INDEX_DATA *shrec )
{
    SHIP_DATA *		ship;

	if( !shrec )
	{
		bug( "no shrec" );
		return NULL;
	}

	if( !shrec )
	{
		bug( "constr_ship: no shrec" );
		return NULL;
	}

    if( shrec->roomcount <= 0 )
    {
		bug( "inclompleted Ship record. Class %s", shrec->name );
		return NULL;
    }

    if( !shipTmpArea )
    {
	bug( "No Area to build ship." );
	return NULL;
    }

    //Tutaj sie zaczyna
    //statek
    ship 			= new_ship( );
    ship->pIndexData		= shrec;
    ship->vnum 			= shrec->vnum;
    ship->timer 		= -1;
    ship->autopilot   		= false;
    ship->hatchopen 		= false;
    ship->roomcount		= shrec->roomcount;

    STRDUP( ship->name, 	shrec->name );
    STRDUP( ship->ship_title, 	"" );
    STRDUP( ship->description, 	shrec->description );
    STRDUP( ship->owner,	"" );
    ship->type          	= shrec->type;
    ship->astro_array		= shrec->astro_array;
    ship->comm			= shrec->comm;
    ship->clazz			= shrec->clazz;
    ship->sensor		= shrec->sensor;
    ship->target_array		= shrec->target_array;
    ship->size 			= shrec->size;
    ship->maxcargo		= shrec->maxcargo;
    ship->maxshield		= shrec->maxshield;
    ship->manuever		= shrec->manuever;

    ship->currspeed		= 0;
    ship->hyperspeed		= shrec->hyperspeed;
    ship->maxmodule 		= shrec->maxmodule;
    ship->maxmissiles		= shrec->maxmissiles;
    ship->missiles		= shrec->maxmissiles;
    ship->maxrockets		= shrec->maxrockets;
    ship->rockets		= shrec->maxrockets;
    ship->maxtorpedos		= shrec->maxtorpedos;
    ship->torpedos		= shrec->maxtorpedos;
    ship->maxenergy 		= shrec->maxenergy;
    ship->energy 		= shrec->maxenergy;
    ship->realspeed		= shrec->realspeed;
    ship->maxhull		= shrec->maxhull;
    ship->hull 			= shrec->maxhull;
    ship->maxchaff		= shrec->maxchaff;
    ship->chaff			= shrec->maxchaff;
    ship->maxcloack		= shrec->maxcloack;
    ship->cloack		= shrec->maxcloack;
    ship->interdict		= shrec->maxinterdict;
    ship->lasers		= shrec->lasers;
    ship->ioncannons		= shrec->ioncannons;

    STRDUP( ship->filename, 	"TESTOWIEC.SHP" );
    STRDUP( ship->lock_key, 	"0000" );


/*
 *  NIE RUSZAC TEGO PONIZEJ!!! - jak ktos ma pytania, to do Thanosa
 */
    ship->shipyard 		= NULL;
    ship->lastdoc		= ship->shipyard;
    ship->location		= ship->shipyard;
    ship->in_room		= ship->shipyard;
    ship->starsystem 		= NULL;
    ship->shipstate 		= SHIP_DISABLED;

    generate_ship_rooms( ship, shrec );

    /* wskazniki na najwazniejsze lokacje */
    ship->pilotseat		= get_sroom( ship, shrec->pilotseat );
    ship->cockpit		= get_sroom( ship, shrec->cockpit   );
    ship->entrance		= get_sroom( ship, shrec->entrance  );
    ship->engineroom		= get_sroom( ship, shrec->engineroom);
    ship->coseat		= get_sroom( ship, shrec->coseat    );
    ship->navseat		= get_sroom( ship, shrec->navseat   );
    ship->gunseat		= get_sroom( ship, shrec->gunseat   );

    //robi turrety i inne smiecie :P
    prepare_ship( ship );

    ship_list.push_back(ship);
    shrec->count++;
    return ship;
}

DEF_DO_FUN( shload )
{
//    SHIP_DATA *		ship;
    SHIP_INDEX_DATA *	shrec;

    if ( !*argument )
    {
	send_to_char( "Syntax: shload <name> or shload list -- to get list" NL, ch );
	return;
    }

    if( !str_cmp( argument, "list" ) )
    {
	do_shlist( ch, (char *)"" );
	return;
    }

    if( !(shrec = get_ship_index( argument )) )
    {
	send_to_char( "No such ship in ShipDb" NL, ch );
	return;
    }

    create_ship_in_room( shrec, ch->in_room );

    send_to_char( "Ok." NL, ch );

    act( COL_IMMORT, "$n sprawia, �e $t nagle wpada tu z g�o�nym hukiem!",
	ch, shrec->name, NULL, TO_ROOM );
    return;
}

