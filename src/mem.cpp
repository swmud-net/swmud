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
*			   Memory management module     		   *
****************************************************************************/


/*
    W tym pliq maj� byc TYLKO f-cje zaczynaj�ce si� na
    free_... lub
    new_...  inaczej --> zabij� :)            --Thanos
    No, mo�e jeszcze clean_... by� ;)
 */

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#if !defined( WIN32 )
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netdb.h>
#endif

#include "mud.h"


std::list<SHIP_DATA*> free_ship_list;

int	shrec_count;

void free_course( COURSE_DATA *course )
{
    STRFREE( course->stop_name );
    DISPOSE( course );
}

void free_courses( SHIP_DATA *ship )
{
    if( !ship || ship->stops.empty() )
	return;

    for( auto* course : ship->stops )
	free_course( course );
    ship->stops.clear();
    ship->curr_stop 	= NULL;

    return;
}

EDD *new_ed()
{
	EDD	*ed;

	CREATE( ed, EDD, 1 );
    STRDUP( ed->description, "" );
    STRDUP( ed->keyword, "" );
	top_ed++;
	return ed;
}

void free_ed( EDD *ed )
{
    STRFREE( ed->description );
    STRFREE( ed->keyword );
    DISPOSE( ed );
    top_ed--;
}

ED *new_exit()
{
	ED	*pExit;

	CREATE( pExit, ED, 1 );
	STRDUP( pExit->keyword, "" );
    STRDUP( pExit->description, "" );
    top_exit++;
	return pExit;
}

void free_exit( EXIT_DATA *pExit )
{
    STRFREE( pExit->keyword );
    STRFREE( pExit->description );
    DISPOSE( pExit );
    top_exit--;
}

MPROG_DATA *new_mprog()
{
	MPROG_DATA	*prog;

	CREATE( prog, MPROG_DATA, 1 );
	STRDUP( prog->arglist, "0" );
	STRDUP( prog->comlist, "break" NL );
	prog->type = 0|RAND_PROG;
	return prog;
}

void free_mprog( MPROG_DATA *prg )
{
    STRFREE( prg->arglist );
    STRFREE( prg->comlist );
    DISPOSE( prg );
}

AFFECT_DATA *new_affect()
{
	AFFECT_DATA	*pAf;
	CREATE( pAf, AFFECT_DATA, 1 );
	pAf->type		= 0;
	pAf->duration	= 0;
	pAf->location	= 0;
	pAf->modifier	= 0;
	pAf->bitvector	= 0;


	top_affect++;
	return pAf;
}

void free_affect( AFFECT_DATA *pAf )
{
    DISPOSE( pAf );
    top_affect--;
}

RESET_DATA *new_reset()
{
	RESET_DATA	*pReset;

	CREATE( pReset, RESET_DATA, 1 );
    top_reset++;
	return pReset;
}

void free_reset( RESET_DATA *pReset )
{
    DISPOSE( pReset );
    top_reset--;
}

void free_area( AREA_DATA *are )
{
    STRFREE( are->name 		);
    STRFREE( are->filename 	);
    STRFREE( are->author 	);
    STRFREE( are->resetmsg 	);

    DISPOSE( are );
    top_area--;
}

void free_note( NOTE_DATA *pnote )
{
    STRFREE( pnote->text    	);
    STRFREE( pnote->subject 	);
    STRFREE( pnote->to_list 	);
    STRFREE( pnote->date    	);
    STRFREE( pnote->sender  	);
    STRFREE( pnote->yesvotes 	);
    STRFREE( pnote->novotes 	);
    STRFREE( pnote->abstentions );

    DISPOSE( pnote );
}

/*
 * Free a command structure					-Thoric
 */
void free_command( CMDTYPE *command )
{
	STRFREE( command->name );
	if( command->alias )
		STRFREE( command->alias );
	DISPOSE( command );
}


void free_social( SOCIALTYPE *social )
{
    STRFREE( social->name 		);
    STRFREE( social->char_no_arg 	);
    STRFREE( social->others_no_arg 	);
    STRFREE( social->char_found 	);
    STRFREE( social->others_found 	);
    STRFREE( social->vict_found 	);
    STRFREE( social->char_auto 		);
    STRFREE( social->others_auto 	);

    DISPOSE( social );
}



void free_pcdata( PC_DATA *pcdata )
{
    int i;

    STRFREE( pcdata->prefix		);
    STRFREE( pcdata->homepage		);
    STRFREE( pcdata->pwd		);
    STRFREE( pcdata->email		);
    STRFREE( pcdata->bamfin		);
    STRFREE( pcdata->bamfout		);
    STRFREE( pcdata->rank		);
    STRFREE( pcdata->title		);
    STRFREE( pcdata->fake_title 	);/* * * * * * */
    STRFREE( pcdata->fake_name	 	);/* Thanos:   */
    STRFREE( pcdata->fake_desc	 	);/*           */
    STRFREE( pcdata->fake_long	 	);/* Nowy      */
    for( i=0; i<6; i++ )                  /* Disguise  */
    STRFREE( pcdata->fake_infl[i] 	);/* * * * * * */
    STRFREE( pcdata->bestowments 	);
    STRFREE( pcdata->invis_except	); /*Thanos*/
    STRFREE( pcdata->isolated_by	);
    STRFREE( pcdata->silenced_by	); /* Trog */
    STRFREE( pcdata->bio		);
    STRFREE( pcdata->authed_by		);
    STRFREE( pcdata->prompt		);
    STRFREE( pcdata->forbidden_cmd	);
    STRFREE( pcdata->afk_reason     	);
    STRFREE( pcdata->quest_done 	);
    STRFREE( pcdata->editinfo		); /*Thanos*/
    STRFREE( pcdata->ignorelist		); /*Thanos*/
    STRFREE( pcdata->tmp_site 		); /*Thanos*/
    STRFREE( pcdata->host 		); /*Thanos*/
    STRFREE( pcdata->user 		); /*Thanos*/

    for( auto* alias : pcdata->aliases )
    {
	STRFREE( alias->name );
	STRFREE( alias->sub );
	DISPOSE( alias );
    }
    pcdata->aliases.clear();

    for( auto* last : pcdata->last_tells )
    {
	STRFREE( last->teller 	);
	STRFREE( last->text		);
	STRFREE( last->time		);
	DISPOSE( last );
    }
    pcdata->last_tells.clear();
    DISPOSE( pcdata );
}

/*
 * Free a character.
 */
void free_char( CHAR_DATA *ch )
{
    MPROG_ACT_LIST 	*mpact, *mpact_next;
    int	i;

    if ( !ch )
    {
      bug( "Free_char: null ch!" );
      return;
    }

    if ( ch->desc )
      bug( "Free_char: char (%s) still has descriptor.", ch->name );

    while ( !ch->carrying.empty() )
	extract_obj( ch->carrying.back() );

    while ( !ch->affects.empty() )
	affect_remove( ch, ch->affects.back() );

    while ( !ch->crimes.empty() )
	free_crime( ch, ch->crimes.back() );

    while ( !ch->timers.empty() )
	extract_timer( ch, ch->timers.front() );

    STRFREE( ch->name		);
    STRFREE( ch->long_descr	);
    STRFREE( ch->description	);
    for( i=0; i<6; i++ )
    STRFREE( ch->przypadki[i]	);

    STRFREE( ch->dest_buf 	);
    STRFREE( ch->dest_buf_2 	);

    STRFREE( ch->s_vip_flags    );
    STRFREE( ch->mob_clan 	);

    // Ratm - zwalnianie pamieci zajetej przez zmienne
    if( ch->variables )
    {
	for( i=0; i<10; i++ )
	{
	    if( ch->variables->global_vars[i] )
		STRFREE( ch->variables->global_vars[i] );
	    if( ch->variables->local_vars[i] )
		STRFREE( ch->variables->local_vars[i] );
	    if( ch->variables->prog_args[i] )
		STRFREE( ch->variables->prog_args[i] );
	}
	DISPOSE( ch->variables );
    }

    stop_hunting( ch );
    stop_hating ( ch );
    stop_fearing( ch );
    free_fight  ( ch );

    if ( ch->pnote )
	free_note( ch->pnote );
    ch->pnote =	 NULL;

    if( ch->pcdata )
	free_pcdata( ch->pcdata );
    ch->pcdata 	= NULL;

    for ( mpact = ch->mpact; mpact; mpact = mpact_next )
    {
	mpact_next = mpact->next;
	free_mpact( mpact );
    }
    ch->mpact 	= NULL;
    ch->inquest = NULL;
	for( auto* _friend : ch->known )
	{
		STRFREE(_friend->name);
		DISPOSE(_friend);
	}
	ch->known.clear();

    char_from_room( ch );

    DISPOSE( ch );
    return;
}

CLAN_DATA *new_clan()
{
	CLAN_DATA	*clan;
	int			i;

	CREATE( clan, CLAN_DATA, 1 );
	STRDUP( clan->name,			"" );
	STRDUP( clan->filename,		"" );
	STRDUP( clan->description,	"" );
	STRDUP( clan->vYes,			"" );
//	STRDUP( clan->vNo,			"" );
	STRDUP( clan->tmpstr,		"" );
	clan->mainorg			= NULL;
	clan->vClan				= NULL;
	clan->leader			= NULL;
	clan->first				= NULL;
	clan->second			= NULL;
	clan->diplomat			= NULL;
	clan->type				= CLAN_CONSTRUCTION;
	clan->clan_id			= 0;
	clan->flags				= 0;
	clan->penalty			= 0;
	clan->vehicles			= 0;
	clan->spacecrafts		= 0;
	clan->branches			= 0;
	clan->members			= 0;
	clan->funds				= 0;
	clan->loan				= 0;
	clan->repay_date		= 0;
	clan->pkills			= 0;
	clan->pdeaths			= 0;
	clan->mkills			= 0;
	clan->mdeaths			= 0;
	for( i = 0; i <= CLAN_LEADER; i++ )
		STRDUP( clan->rank[i],	"" );
	top_clan++;

	return clan;
}

void free_clan( CLAN_DATA *clan )
{
	int			i;

	STRFREE( clan->name );
	STRFREE( clan->filename );
	STRFREE( clan->description );
	STRFREE( clan->vYes );
//	STRFREE( clan->vNo );
	STRFREE( clan->tmpstr );
	for( i = 0; i <= CLAN_LEADER; i++ )
		STRFREE( clan->rank[i] );

	for( auto* suborg : clan->suborgs )
	{
		suborg->mainorg = NULL;
		suborg->type = CLAN_ORGANIZATION;
		save_clan( suborg );
	}
	clan->suborgs.clear();

	/* free HQ here */

	for( auto* member : clan->member_list )
		free_member( member );
	clan->member_list.clear();

	DISPOSE( clan );
	top_clan--;
}

MEMBER_DATA *new_member( int status )
{
	MEMBER_DATA	*member;

	CREATE( member, MEMBER_DATA, 1 );
	STRDUP( member->name,		"" );
	STRDUP( member->bestowments,"" );
	member->status = status;

	return member;
}

void free_member( MEMBER_DATA *member )
{
    STRFREE( member->name );
    STRFREE( member->bestowments );
    DISPOSE( member );
}

HELPS_FILE *new_helps_file()
{
	HELPS_FILE 	*fHelp;

	CREATE( fHelp, HELPS_FILE, 1 );
	STRDUP( fHelp->name, DEFAULT_HELP_FILE );

    return fHelp;
}

void free_helps_file( HELPS_FILE *fHelp )
{
	for( auto* pHelp : fHelp->helps )
		free_help( pHelp );
	fHelp->helps.clear();

	clear_helps_file( fHelp->name );
	STRFREE( fHelp->name );
	DISPOSE( fHelp );
}

void clear_helps_file( char *filename )
{
	char	filepath[MIL];

	sprintf( filepath, "%s%s", HELPS_DIR, filename );
	unlink( filepath );
}

HELP_DATA *new_help( HELPS_FILE *fHelp )
{
    HELP_DATA *	pHelp;

    CREATE( pHelp, HELP_DATA, 1 );
	pHelp->file = fHelp;
    STRDUP( pHelp->keyword,	"" );
    STRDUP( pHelp->syntax, 	"" );
    STRDUP( pHelp->text, 	"" );
    pHelp->type 	= HELP_PHELP;
    pHelp->level	= 0;

    return pHelp;
}

void free_help( HELP_DATA *pHelp )
{
    STRFREE( pHelp->keyword 	);
    STRFREE( pHelp->syntax 	);
    STRFREE( pHelp->text 	);
    DISPOSE( pHelp );
}

SEASON_DATA *new_season( PLANET_DATA *planet, const char *name )
{
    SEASON_DATA *season;

    CREATE( season, SEASON_DATA, 1 );
    STRDUP( season->name, name );
    season->temperature 	= 0;
    season->diff_temperature 	= 0;
    season->mmhg	 	= 1000;
    season->diff_mmhg	 	= 0;
    season->day_length	 	= planet->daylen;
    season->night_length	= planet->nightlen;
    season->length		= 3;
    season->windspeed		= 10;

    if( season->day_length     <= 0)
	season->day_length	= 12;

    if( season->night_length   <= 0)
	season->night_length	= 12;

    return season;
}

void free_who( WHO_DATA *who )
{
    STRFREE( who->text );
    DISPOSE( who );
}

void free_ban( BAN_DATA *pban )
{
    STRFREE(pban->ban_time);
    STRFREE( pban->name );
    DISPOSE( pban );
}

void free_mpact( MPROG_ACT_LIST * mpact )
{
    STRFREE( mpact->buf );
    DISPOSE( mpact );
}

void free_bounty( BOUNTY_DATA *bounty )
{
    STRFREE( bounty->target   );
    DISPOSE( bounty );
}

void free_last( LAST_DATA *last )
{
    STRFREE( last->teller);
    STRFREE( last->text	);
    STRFREE( last->time	);
    STRFREE( last->verb	);
    DISPOSE( last );
}

void free_alias( ALIAS_DATA *alias )
{
    STRFREE( alias->name );
    STRFREE( alias->sub );
    DISPOSE( alias );
}

void free_desc( DESCRIPTOR_DATA *d )
{
#if !defined( WIN32 )
    close( d->descriptor );
#else
    closesocket( d->descriptor );
#endif
    STRFREE( d->host 		);
    DISPOSE( d->outbuf 		);
    STRFREE( d->user 		);    /* identd */
    if ( d->pagebuf )
    DISPOSE( d->pagebuf 	);

    if( d->tmp_ch )
	DISPOSE( d->tmp_ch );

    DISPOSE( d );
    --num_descriptors;
    return;
}

void free_crime( CHAR_DATA *ch, CRIME_DATA *crime )
{
    if ( ch->crimes.empty() )
    {
	bug( "Free_crime: %s-->no crime.", ch->name );
	return;
    }

    ch->crimes.remove( crime );
    STRFREE( crime->planet );
    DISPOSE( crime );
    return;
}

void free_wiz( WIZENT *wiz )
{
    STRFREE(wiz->name);
    DISPOSE(wiz);
}

RID *new_room()
{
	RID	*room;

	CREATE( room, RID, 1 );
	top_room++;
    STRDUP( room->name ,"" );
    STRDUP( room->homename, "" );
    STRDUP( room->description, "" );
    STRDUP( room->nightdesc, "" );
	CREATE( room->variables, VAR_DATA, 1 );
	bzero( room->variables, sizeof( VAR_DATA ) );

	return room;
}

void free_room( RID *room )
{
    RESET_DATA *res;

    if (!is_room_unlinked(room))
	{
		bug( "room is being freed, but it's NOT unlinked!" );
		/* Trog: wywalam muda, bo muszae miec core'a */
		res = NULL;
		res->extra = 1;
	}

    if( room->vnum > 0 && room->vnum < MAX_VNUM )
	top_room--;
    else
	top_vroom--;

    for( auto* res : room->resets )
    	free_reset( res );
    room->resets.clear();

    // Ratm - zwalnianie pamieci zajetej przez zmienne
    if( room->variables )
    {
	int i;

	for( i=0; i<10; i++ )
	{
	    if( room->variables->global_vars[i] )
		STRFREE( room->variables->global_vars[i] );
	    if( room->variables->local_vars[i] )
		STRFREE( room->variables->local_vars[i] );
	    if( room->variables->prog_args[i] )
		STRFREE( room->variables->prog_args[i] );
	}
	DISPOSE( room->variables );
    }


    STRFREE( room->name 	);
    STRFREE( room->homename 	);
    STRFREE( room->description 	);
    STRFREE( room->nightdesc 	);
    DISPOSE( room );
}

void free_hhf( HHF_DATA *hhf )
{
    STRFREE( hhf->name );
    DISPOSE( hhf );
}

OID *new_obj_index()
{
	OID	*obj;
	int	i;

	CREATE( obj, OID, 1 );
    STRDUP( obj->name, "" );
    STRDUP( obj->description, "" );
    STRDUP( obj->action_desc, "" );
    for( i = 0; i < 6; i++ )
		STRDUP( obj->przypadki[i], "" );

	return obj;
}

void free_obj_index( OID *obj )
{
	int	i;

	STRFREE( obj->name );
	STRFREE( obj->description );
	STRFREE( obj->action_desc );

	for( i = 0; i < 6; i++ )
		STRFREE( obj->przypadki[i] );
	DISPOSE( obj );
}

void free_obj( OBJ_DATA *obj )
{
    int i;
    STRFREE( obj->name        	);
    STRFREE( obj->description 	);
    STRFREE( obj->action_desc 	);
    if( obj->armed_by 	)
    STRFREE( obj->armed_by 	);
    STRFREE( obj->owner_name 	);

    for( i=0; i<6; i++ )
        STRFREE( obj->przypadki[i] );

    // Ratm - zwalnianie pamieci zajetej przez zmienne
    if( obj->variables )
    {
	for( i=0; i<10; i++ )
	{
	    if( obj->variables->global_vars[i] )
		STRFREE( obj->variables->global_vars[i] );
	    if( obj->variables->local_vars[i] )
		STRFREE( obj->variables->local_vars[i] );
	    if( obj->variables->prog_args[i] )
		STRFREE( obj->variables->prog_args[i] );
	}
	DISPOSE( obj->variables );
    }

    DISPOSE( obj );
}


void free_nameslist( NAMESLIST_DATA *nameslist )
{
    STRFREE( nameslist->name );
    DISPOSE( nameslist );
}

void free_season( SEASON_DATA *season )
{
    STRFREE( season->name );
    DISPOSE( season );
}

void free_dock( DOCK_DATA *dock )
{
    STRFREE( dock->name );
    DISPOSE( dock );
}

void free_chapter( CHAPTER_INDEX_DATA *chapt )
{
	for( auto* pCmnd : chapt->init_cmds )
		free_qcmd( pCmnd );
	chapt->init_cmds.clear();

	for( auto* pCmnd : chapt->events )
		free_qcmd( pCmnd );
	chapt->events.clear();

	for( auto* pAction : chapt->actions )
		free_qaction( pAction );
	chapt->actions.clear();

	STRFREE( chapt->name );
	DISPOSE( chapt );
}

void free_qcmd( QUEST_CMND_DATA *pCmnd )
{
    STRFREE( pCmnd->arg4 );
    DISPOSE( pCmnd );
}

void free_qaction( QUEST_ACTION_DATA *action )
{
    STRFREE( action->arg4 );
    DISPOSE( action );
}

void free_smaug_aff( SMAUG_AFF *aff )
{
    STRFREE( aff->modifier );
    STRFREE( aff->duration );
    DISPOSE( aff );
}

void free_star( STAR_DATA *star )
{
    STRFREE( star->name );
    DISPOSE( star );
}

void free_moon( MOON_DATA *moon )
{
    STRFREE( moon->name );
    DISPOSE( moon );
}

void free_missile( MISSILE_DATA *missile )
{
    STRFREE( missile->fired_by );
    DISPOSE( missile );
}

void free_starsystem( SPACE_DATA *starsystem )
{
    STRFREE( starsystem->name );
    STRFREE(starsystem->filename);
    DISPOSE( starsystem );
}

void free_transponder( TRANSPONDER_DATA *trans )
{
    STRFREE( trans->number );
    STRFREE( trans->shipname );
    DISPOSE( trans );
}

void free_astro( ASTRO_DATA *astro )
{
    STRFREE(astro->name);
    STRFREE(astro->filename);
    STRFREE(astro->home);
    DISPOSE( astro );
}

void free_suspect( CHAR_DATA *ch, SUSPECT_DATA *sus )
{
    ch->suspects.remove( sus );
    STRFREE( sus->name );
    DISPOSE( sus );
    return;
}

void free_script( SCRIPT_DATA *script )
{
    STRFREE( script->command );
    DISPOSE( script );
}

void free_mid( MOB_INDEX_DATA *mid )
{
    int i;

    for( i=0; i<6; i++ )
    STRFREE( mid->przypadki[i] 	);
    STRFREE( mid->player_name	);
    STRFREE( mid->long_descr 	);
    STRFREE( mid->description	);
    STRFREE( mid->s_vip_flags	);
    DISPOSE( mid );
}

void free_oid( OBJ_INDEX_DATA *oid )
{
    int i;

    for( i=0; i<6; i++ )
    STRFREE( oid->przypadki[i] 	);

    STRFREE( oid->name 		);
    STRFREE( oid->description 	);
    STRFREE( oid->action_desc 	);

    DISPOSE( oid );
}

void free_inform( INFORM_DATA *inform )
{
    STRFREE( inform->attacker );
    STRFREE( inform->victim );
    DISPOSE( inform );
}

void free_ship( SHIP_DATA *ship )
{
    char		buf		[MSL];


    sprintf( buf, "Freeing ship: %s", ship->name );
    log_string( buf );

    extract_ship( ship );

    if( ship->starsystem )
	ship_from_starsystem( ship, ship->starsystem );

    ship_list.remove( ship );

    for( auto* stop : ship->stops )
	DISPOSE( stop );
    ship->stops.clear();

    for( auto* turret : ship->turrets )
	DISPOSE( turret );
    ship->turrets.clear();

    for( auto* hanger : ship->hangars )
	DISPOSE( hanger );
    ship->hangars.clear();

/*    for( auto* cargo : ship->cargo )
	DISPOSE( cargo );
    ship->cargo.clear();    */

    for( auto* module : ship->modules )
    {
	STRFREE( module->spyname );
	DISPOSE( module );
    }
    ship->modules.clear();

/*
    for( auto* smap : ship->smaps )
    {
	STRFREE( smap->name );
	DISPOSE( smap );
    }
    ship->smaps.clear();
*/
    for( auto* trans : ship->transponders )
	free_transponder( trans );
    ship->transponders.clear();

    for( auto* dock : ship->docks )
	DISPOSE( dock );
    ship->docks.clear();

    while (!ship->locations.empty())
	{
		auto* room = ship->locations.front();
		ship->locations.pop_front();

		if (!room->people.empty())
		{
			bug( "free_ship: FATAL: room with people (#%d on %s)",
					VNUM(room), ship->name );

			auto people_snapshot = room->people;
			for (auto* ech : people_snapshot)
			{
				if (ech->fighting)
					stop_fighting(ech, true);

				if (IS_NPC(ech))
					extract_char(ech, true);
				else
				{
					char_from_room(ech);
					char_to_room(ech, get_room_index(ROOM_VNUM_LIMBO));
				}
			}
		}

		for( auto* ed : room->extradesc )
			free_ed(ed);
		room->extradesc.clear();

		while( !room->exits.empty() )
		{
			auto* xit = room->exits.front();
			room->exits.pop_front();
			extract_exit(room, xit);
		}

		{
			for (auto* mprog : room->mudprogs)
				free_mprog(mprog);
			room->mudprogs.clear();
		}

		{
			MPROG_ACT_LIST * mpact;
			MPROG_ACT_LIST * mpact_next;
			for (mpact = room->mpact; mpact; mpact = mpact_next)
			{
				mpact_next = mpact->next;
				free_mpact(mpact);
			}
		}

		unlink_room(room);
		free_room(room);
	}

/* Nie wiem czemu to kurwa nie dziala!
   Jak tylko uzywam ponizszych STRFREE, a nastepnie DISPOSE
   - mud pada na nasepnej probie zaalokowania jakiejkolwiek innej pamieci!
   Ale ok, mam sposob ;)   -- Thanos */
/*
    STRFREE( ship->description 	);
    STRFREE( ship->transponder 	);
    STRFREE( ship->filename 	);
    STRFREE( ship->name 	);
    STRFREE( ship->home 	);
    STRFREE( ship->transponder 	);
    STRFREE( ship->ship_title 	);
    STRFREE( ship->owner 	);
    STRFREE( ship->pilot 	);
    STRFREE( ship->transponder 	);
    STRFREE( ship->copilot 	);
    STRFREE( ship->engineer 	);
    STRFREE( ship->dest 	);
    STRFREE( ship->cargofrom 	);
    STRFREE( ship->sXpos 	);
    STRFREE( ship->sYpos 	);
    STRFREE( ship->sslook 	);
    STRFREE( ship->lock_key 	);


    DISPOSE( ship );    */

/* zrobimy cos ala smietnik - tam beda lezaly statki nieuzywane
   acz ladnie oczyszczone. Teraz jesli bedziemy chcieli stworzyc
   nowy statek nie bedziemy uzywac CREATE, tylko wyciagniemy jakis
   stateczek ze smietnika */
    free_ship_list.push_back( ship );
    return;
}

SHIP_DATA *new_ship( )
{
    SHIP_DATA *	ship;

    if( !free_ship_list.empty() )
    {
	ship = free_ship_list.front();
	free_ship_list.pop_front();
    }
    else
	CREATE( ship, SHIP_DATA, 1 );

    STRDUP( ship->description,  "" );
    STRDUP( ship->transponder, 	"" );
    STRDUP( ship->filename, 	"" );
    STRDUP( ship->name, 	"" );
    STRDUP( ship->home, 	"" );
    STRDUP( ship->transponder, 	"" );
    STRDUP( ship->ship_title, 	"" );
    STRDUP( ship->owner, 	"" );
    STRDUP( ship->pilot, 	"" );
    STRDUP( ship->transponder, 	"" );
    STRDUP( ship->copilot, 	"" );
    STRDUP( ship->engineer, 	"" );
    STRDUP( ship->dest, 	"" );
    STRDUP( ship->cargofrom, 	"" );
    STRDUP( ship->sXpos, 	"" );
    STRDUP( ship->sYpos, 	"" );
    STRDUP( ship->sslook, 	"" );
    STRDUP( ship->lock_key, 	"" );

    ship->pIndexData 		= NULL;
    ship->starsystem		= NULL;
    ship->stops.clear();
    ship->curr_stop		= NULL;
    ship->turrets.clear();
    ship->hangars.clear();
    ship->in_room		= NULL;
    ship->cargo.clear();
    ship->modules.clear();
    ship->smaps.clear();
    ship->transponders.clear();
    ship->docks.clear();
    ship->locations.clear();
    ship->cockpit		= NULL;
    ship->location		= NULL;
    ship->lastdoc		= NULL;
    ship->shipyard		= NULL;
    ship->navseat		= NULL;
    ship->pilotseat		= NULL;
    ship->coseat		= NULL;
    ship->gunseat		= NULL;
    ship->target0		= NULL;
    ship->currjump		= NULL;
    ship->killer		= NULL;
    ship->last_dock_with	= NULL;

    ship->flags 		= 0;
    ship->tmp_flags 		= 0;
    return ship;
}

SHIP_INDEX_DATA *new_ship_index( )
{
    SHIP_INDEX_DATA *	ship;

    CREATE( ship, SHIP_INDEX_DATA, 1 );
    STRDUP( ship->filename, 	"" );
    STRDUP( ship->name, 	"" );
    STRDUP( ship->builders, 	"" );
    STRDUP( ship->description, 	"" );
    ship->security 		= 2;
    ship->count			= 0;
    shrec_count++;
    return ship;
}

void free_sroom( SHIP_ROOM_DATA * sRoom )
{
    MPROG_DATA *    	mprog;
    MPROG_DATA *    	mprog_next;

    for( auto* ed : sRoom->extradesc )
        free_ed( ed );
    sRoom->extradesc.clear();

    for( auto* xit : sRoom->exits )
    {
        STRFREE( xit->keyword 		);
        STRFREE( xit->description 	);
	DISPOSE( xit );
    }
    sRoom->exits.clear();

    for( auto* mprog : sRoom->mudprogs )
        free_mprog( mprog );
    sRoom->mudprogs.clear();

    for( auto* res : sRoom->resets )
    	free_reset( res );
    sRoom->resets.clear();

    STRFREE( sRoom->name );
    STRFREE( sRoom->description );
    top_sroom--;
    DISPOSE( sRoom );
}

void free_srooms( SHIP_INDEX_DATA *ship )
{
    for( auto* sRoom : ship->rooms )
	free_sroom( sRoom );
    ship->rooms.clear();

    return;
}

void free_part( PART_DATA *part )
{
    for( auto* comp : part->components )
        DISPOSE( comp );
    part->components.clear();
    DISPOSE( part );
    return;
}

void free_project( PROJECT_DATA *pro )
{
    for( auto* part : pro->parts )
	free_part( part );
    pro->parts.clear();
    DISPOSE( pro );
    return;
}

void free_complain( COMPLAIN_DATA *complain )
{
    STRFREE( complain->text    	);
    STRFREE( complain->author 	);
    STRFREE( complain->date 	);
    STRFREE( complain->fixedby    	);
    STRFREE( complain->fixdate  	);

    DISPOSE( complain );
}

void free_skill( SKILLTYPE *skill )
{
	SMAUG_AFF	*aff, *aff_next;

	STRFREE( skill->name );
	STRFREE( skill->noun_damage );
	STRFREE( skill->msg_off );
	if( skill->hit_char )
		STRFREE( skill->hit_char );
	if( skill->hit_vict )
		STRFREE( skill->hit_vict );
	if( skill->hit_room )
		STRFREE( skill->hit_room );
	if( skill->miss_char )
		STRFREE( skill->miss_char );
	if( skill->miss_vict )
		STRFREE( skill->miss_vict );
	if( skill->miss_room )
		STRFREE( skill->miss_room );
	if( skill->die_char )
		STRFREE( skill->die_char );
	if( skill->die_vict )
		STRFREE( skill->die_vict );
	if( skill->die_room )
		STRFREE( skill->die_room );
	if( skill->imm_char )
		STRFREE( skill->imm_char );
	if( skill->imm_vict )
		STRFREE( skill->imm_vict );
	if( skill->imm_room )
		STRFREE( skill->imm_room );
	if( skill->dice )
		STRFREE( skill->dice );
	if( skill->components )
		STRFREE( skill->components );
	if( skill->teachers )
		STRFREE( skill->teachers );
	for( aff = skill->affects; aff; aff = aff_next )
	{
		aff_next = aff->next;
		free_smaug_aff( aff );
	}

	DISPOSE( skill );
}

RACE_DATA *new_race()
{
	RACE_DATA	*race;
	int			i;

	CREATE( race, RACE_DATA, 1 );
	STRDUP( race->name, "" );
	STRDUP( race->filename, "" );
	STRDUP( race->description, "" );
	for( i = 0; i < 6; i++ )
		STRDUP( race->przypadki[i], "" );
        for( i = 0; i < 6; i++ )
		STRDUP( race->inflectsFemale[i], "" );
	race->flags				= FRACE_PROTOTYPE;
	race->affected			= 0;
	race->str_plus			= 0;
	race->dex_plus			= 0;
	race->wis_plus			= 0;
	race->int_plus			= 0;
	race->con_plus			= 0;
	race->cha_plus			= 0;
	race->lck_plus			= 0;
	race->frc_plus			= 0;
	race->hp_plus			= 0;
	race->force_plus		= 0;
	race->frc_plus			= 0;
	race->susceptible		= 0;
	race->resistant			= 0;
	race->immune			= 0;
	race->language			= NULL;
	for( i = 0; i < MAX_DESC_TYPES; i++ )
		race->desc_restrictions[i]=0;

	return race;
}

void free_race( RACE_DATA *race )
{
	int	i;

	STRFREE( race->name );
	STRFREE( race->filename );
	STRFREE( race->description );
	STRFREE( race->name );
	for( i = 0; i < 6; i++ )
		STRFREE( race->przypadki[i] );
	for( i = 0; i < 6; i++ )
		STRFREE( race->inflectsFemale[i] );

	DISPOSE( race );
}

LANG_DATA *new_lang()
{
	LANG_DATA	*lang;

	CREATE( lang, LANG_DATA, 1 );
	STRDUP( lang->name, "" );
	STRDUP( lang->alphabet, "" );
	STRDUP( lang->deny_text, "" );
	lang->max_learn		= 99;
	lang->max_practice	= 75;
	lang->min_int		= 0;
	lang->difficulty	= 0;

	return lang;
}

void free_lang( LANG_DATA *lang )
{
	STRFREE( lang->name );
	STRFREE( lang->alphabet );
	STRFREE( lang->deny_text );

	DISPOSE( lang );
}

KNOWN_LANG *new_known_lang()
{
	KNOWN_LANG	*lang;

	CREATE( lang, KNOWN_LANG, 1 );
	lang->language		= NULL;
	lang->learned		= 1;
/*
	lang->times_used	= 0;
*/
	return lang;
}

void free_known_lang( KNOWN_LANG *lang )
{
	DISPOSE( lang );
}

BETS_PROGRES * new_bets()
{
	BETS_PROGRES *	pBets;
	CREATE( pBets,BETS_PROGRES,1);
	STRDUP( pBets->name, "" );
	pBets->buy_out		= false;
	pBets->price		= 0;
	pBets->max_price	= 0;
	pBets->quantity		= 1;

	return pBets;
}

void free_bets(BETS_PROGRES *	pBets)
{
	STRFREE( pBets->name );
	DISPOSE( pBets );
}

AUCTION_DATA * new_auction()
{
	struct	tm *	tms;
	AUCTION_DATA *	pAuction;
	CREATE(pAuction,AUCTION_DATA,1);
	STRDUP( pAuction->seller_name, "" );
	STRDUP( pAuction->stock_name, "" );
	STRDUP( pAuction->desc, "" );
	pAuction->planet			= NULL;
	pAuction->item				= NULL;
	pAuction->buy_out_now		= 0;
	pAuction->starting_price	= 0;
	pAuction->bet				= 0;
	pAuction->bidders_count		= 0;
	pAuction->item_collected	= false;
	pAuction->payment_collected	= false;
	pAuction->notes_send		= false;
	pAuction->type				= (auction_type)0;
	pAuction->quantity			= 1;
	pAuction->duration			= 0;

	tms = localtime(&current_time);
	tms->tm_min+=10;
	pAuction->start_time 		= mktime(tms);

	tms = localtime(&current_time);
	tms->tm_mday++;
	tms->tm_min+=10;
	pAuction->end_time			= mktime(tms);

	return pAuction;
}

void free_auction(AUCTION_DATA *	pAuction)
{
	STRFREE( pAuction->seller_name );
	STRFREE( pAuction->stock_name );
	STRFREE( pAuction->desc );
	for( auto* pBets : pAuction->bets )
		free_bets( pBets);
	pAuction->bets.clear();
	if (pAuction->item)
	{
		object_list.remove( pAuction->item );
//		extract_obj( pAuction->item );
		free_obj(pAuction->item);
	}
	DISPOSE( pAuction );
}

STOCK_EXCHANGE_DATA * new_stock_exchange()
{
	STOCK_EXCHANGE_DATA * pStock;
	CREATE(pStock, STOCK_EXCHANGE_DATA, 1);
	pStock->pPlanet	= NULL;

	return pStock;
}

void free_stock_exchange(STOCK_EXCHANGE_DATA * pStock)
{
	for( auto* pAuction : pStock->auctions )
		free_auction( pAuction );
	pStock->auctions.clear();
	for( auto* pAuction : pStock->new_offers )
		free_auction( pAuction );
	pStock->new_offers.clear();

	stock_exchange_list.remove( pStock );
	DISPOSE( pStock );
}



MATERIAL_DATA * new_material()
{
	MATERIAL_DATA			* pMaterial;
	CREATE( pMaterial, MATERIAL_DATA, 1 );
	STRDUP( pMaterial->name, "" );
	STRDUP( pMaterial->desc, "(brak)" );
	pMaterial->price			= 0;
	pMaterial->min_price		= 0;
	pMaterial->max_price		= 0;
	pMaterial->type				= U_ORE_FORM;
	pMaterial->rarity			= COMMON_MAT;
	return pMaterial;
}

void free_material( MATERIAL_DATA * pMaterial )
{
	STRFREE( pMaterial->name );
	STRFREE( pMaterial->desc );
	DISPOSE( pMaterial );
}

DIALOG_NODE *new_dialog_node()
{
	DIALOG_NODE *pNode;
	CREATE(pNode,DIALOG_NODE,1);
	STRDUP( pNode->text, "(brak)" );
	STRDUP( pNode->mob_answer, "");
	pNode->pBase			= NULL;
	pNode->first			= NULL;
	pNode->prog				= NULL;
	pNode->nr				= -1;
	pNode->target_nr		= -1;
	pNode->saved			= false;
	return pNode;
}

void free_dialog_node( DIALOG_NODE *pNode )
{
	DIALOG_DATA		* pData = pNode->pBase;
	DIALOG_NODE		* pLeaf = NULL, * pNextLeaf = NULL;

	if ( pData  )
	{
		auto snapshot = pNode->children;
		for( auto* pLeaf : snapshot )
		{
			pData->nodes.remove( pLeaf );
			pNode->children.remove( pLeaf );
			free_dialog_node( pLeaf );
		}
	}

	if (pNode->prog)
		free_mprog( pNode->prog );

	STRFREE( pNode->text );
	STRFREE( pNode->mob_answer );
	DISPOSE( pNode );
}

DIALOG_DATA * new_dialog_data()
{
	DIALOG_DATA			* pData;	//pierwszy tekst jaki gracz moze wystosowac do moba
	DIALOG_NODE			* pNode;
	CREATE( pData,DIALOG_DATA,1);
	STRDUP( pData->name, "" );
	pData->dialog_ID	= free_dialog_ID();

	pNode 				= new_dialog_node();
	pData->nodes.push_back( pNode );
	pNode->pBase		= pData;

	return pData;
}

void free_dialog_data( DIALOG_DATA * pData )
{
	while ( !pData->nodes.empty())
	{
		auto* pNode = pData->nodes.front();
		pData->nodes.remove( pNode );
		free_dialog_node( pNode );
	}
	STRFREE( pData->name );
	DISPOSE( pData );
}
DEPOSIT_DATA * new_deposit()
{
	DEPOSIT_DATA	* pDeposit;
	CREATE( pDeposit, DEPOSIT_DATA, 1);
	STRDUP( pDeposit->material_name,	"" );
	pDeposit->daily_mining = 0;
	return pDeposit;
}

void free_deposit( DEPOSIT_DATA * pDeposit )
{
	STRFREE( pDeposit->material_name );
	DISPOSE( pDeposit )	;
}

CARGO_DATA * new_cargo()
{
	CARGO_DATA			* pCargo;
	CREATE( pCargo, CARGO_DATA, 1);
	pCargo->pMaterial	= NULL;
	return pCargo;
}

void free_cargo( CARGO_DATA * pCargo )
{
	if ( pCargo->pMaterial )
		free_material( pCargo->pMaterial );
	DISPOSE( pCargo );
}

HQ_DATA * new_hq_data()
{
	HQ_DATA * pHQ=NULL;

	CREATE( pHQ, HQ_DATA, 1);
	pHQ->main=true;

	return pHQ;
}

TURBOCAR *new_turbocar()
{
	TURBOCAR	*tc;

	CREATE( tc, TURBOCAR, 1 );
	STRDUP( tc->name, "" );
	STRDUP( tc->filename, "" );
	tc->vnum = 0;
	tc->current_station = NULL;

	return tc;
}

void free_turbocar( TURBOCAR *tc )
{
	TC_STATION	*station;
	TC_STATION	*station_next;

	STRFREE( tc->name );
	STRFREE( tc->filename );
	while( !tc->stations.empty() )
	{
		auto* station = tc->stations.front();
		tc->stations.remove( station );
		free_station( station );
	}
	DISPOSE( tc );
}

TC_STATION *new_station()
{
	TC_STATION	*station;

	CREATE( station, TC_STATION, 1 );
	STRDUP( station->name, "" );
	station->vnum = 0;

	return station;
}

void free_station( TC_STATION *station )
{
	STRFREE( station->name );
	DISPOSE( station );
}

FEVENT_DATA	*new_fevent()
{
	FEVENT_DATA	*fevent;

	CREATE( fevent, FEVENT_DATA, 1 );
	STRDUP( fevent->sattr, "" );
	fevent->trigger = (fe_trigger)0;
	memset( &fevent->attr, 0, FE_MAX_ATTR*sizeof(int64) );

	return fevent;
}

void free_fevent( FEVENT_DATA *fevent )
{
	STRFREE( fevent->sattr );
	DISPOSE( fevent );
}

MID	*new_mob()
{
	MID	*pMobIndex;

	CREATE( pMobIndex, MID, 1 );
	clean_mob( pMobIndex );
	return pMobIndex;
}

void free_mob( MID *mob )
{
	MPROG_DATA	*mprog;
	MPROG_DATA	*mprog_next;
	int			i;

	STRFREE( mob->player_name );
	for( i = 0; i < 6; i++ )
		STRFREE( mob->przypadki[i] );
	STRFREE( mob->s_vip_flags );
	STRFREE( mob->long_descr );
	STRFREE( mob->description );
	STRFREE( mob->dialog_name );

	for( mprog = mob->mudprogs; mprog; mprog = mprog_next )
	{
		mprog_next = mprog->next;
		free_mprog( mprog );
	}
}

REQ *new_requirement()
{
	REQ	*req;

	CREATE( req, REQ, 1 );
	req->type = 0;
	req->location = 0;
	req->modifier = 0;
	top_req++;
	return req;
}

void free_requirement( REQ *req )
{
	DISPOSE( req );
}

SHD	*new_shop()
{
	SHD	*shop;

	CREATE( shop, SHD, 1 );
	top_shop++;
	return shop;
}

void free_shop( SHD *shop )
{
	top_shop--;
	DISPOSE( shop );
}

RHD *new_repair()
{
	RHD	*repair;

	CREATE( repair, RHD, 1 );
	return repair;
}

void free_repair( RHD *repair )
{
	DISPOSE( repair );
}

AREA_TMP_DATA *new_area_tmp()
{
	AREA_TMP_DATA	*area_tmp;

	CREATE( area_tmp, AREA_TMP_DATA, 1 );
	return area_tmp;
}

void free_area_tmp( AREA_TMP_DATA *area_tmp )
{
	DISPOSE( area_tmp );
}

ILD *new_ild()
{
	ILD	*ild;

	CREATE( ild, ILD, 1 );
	STRDUP( ild->data, "" );
	return ild;
}

void free_ild( ILD *ild )
{
	STRFREE( ild->data );
	DISPOSE( ild );
}

POLITICS_DATA *new_politics()
{
	POLITICS_DATA	*politics;

	CREATE( politics, POLITICS_DATA, 1 );
	politics->clan			= NULL;
	politics->relations		= 0;

	return politics;
}

void free_politics( POLITICS_DATA *politics )
{
	DISPOSE( politics );
}

