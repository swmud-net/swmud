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
*  The MUDprograms are heavily based on the original MOBprogram code that  *
*  was written by N'Atas-ha.						   *
****************************************************************************/

#include <sys/types.h>
#include <unistd.h>
#include "mud.h"

extern	bool	noEcho;
extern  bool	forceReset;

ch_ret	simple_damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt );
CHAR_DATA * get_char_room_mp  args( ( CHAR_DATA *ch, char *argument ) );
bool 	belongs_to_quest	args( ( CHAR_DATA *ch, QUEST_DATA *quest ) );
void	reset_room		args( ( ROOM_INDEX_DATA *room ) );

#define MPCOMMAND						\
do								\
{								\
    IF_BUG( ch==NULL, "" )					\
	return;							\
								\
    if ( IS_AFFECTED( ch, AFF_CHARM ) )				\
      return;							\
								\
    if ( !IS_NPC( ch ) )					\
    {								\
	huh( ch );						\
	return;							\
    }								\
								\
} while( 0 )

const char *mprog_type_to_name( int64 type )
{
    switch ( type )
    {
	case IN_FILE_PROG:      return "in_file_prog";
	case ACT_PROG:          return "act_prog";
	case SPEECH_PROG:       return "speech_prog";
	case RAND_PROG:         return "rand_prog";
	case FIGHT_PROG:        return "fight_prog";
	case HITPRCNT_PROG:     return "hitprcnt_prog";
	case DEATH_PROG:        return "death_prog";
	case ENTRY_PROG:        return "entry_prog";
	case GREET_PROG:        return "greet_prog";
	case ALL_GREET_PROG:    return "all_greet_prog";
	case GIVE_PROG:         return "give_prog";
	case BRIBE_PROG:        return "bribe_prog";
	case HOUR_PROG:		return "hour_prog";
	case TIME_PROG:		return "time_prog";
	case WEAR_PROG:         return "wear_prog";
	case REMOVE_PROG:       return "remove_prog";
	case SAC_PROG :         return "sac_prog";
	case TELL_PROG:         return "tell_prog";
	case EXA_PROG:          return "exa_prog";
	case ZAP_PROG:          return "zap_prog";
	case GET_PROG:          return "get_prog";
	case DROP_PROG:         return "drop_prog";
	case REPAIR_PROG:       return "repair_prog";
	case DAMAGE_PROG:       return "damage_prog";
	case PULL_PROG:         return "pull_prog";
	case PUSH_PROG:         return "push_prog";
	case SCRIPT_PROG:	return "script_prog";
	case SLEEP_PROG:        return "sleep_prog";
	case REST_PROG:         return "rest_prog";
	case LEAVE_PROG:        return "leave_prog";
	case USE_PROG:          return "use_prog";
	case QUEST_PROG:        return "quest_prog";
	case EXTRADESC_PROG:    return "extradesc_prog";
	default:                return "ERROR_PROG";
    }
}

/* A trivial rehack of do_mstat.  This doesnt show all the data, but just
 * enough to identify the mob and give its basic condition.  It does however,
 * show the MUDprograms which are set.
 */
DEF_DO_FUN( mpstat )
{
    char        arg[MAX_INPUT_LENGTH];
    CHAR_DATA  *victim;
    int		i;


    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "MProg stat whom?" NL, ch );
	return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
	send_to_char( "They aren't here." NL, ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	send_to_char( "Only Mobiles can have MobPrograms!" NL, ch);
	return;
    }

    if ( !( victim->pIndexData->progtypes ) )
    {
	send_to_char( "That Mobile has no Programs set." NL, ch);
	return;
    }

    ch_printf( ch, "Name: %s.  Vnum: %d." NL,
	victim->name, victim->pIndexData->vnum );

    ch_printf( ch, "Short description: %s.\n\rLong  description: %s",
	    victim->przypadki[0],
	    victim->long_descr[0] != '\0' ?
	    victim->long_descr : "(none)." NL );

    ch_printf( ch, "Hp: %d/%d.  Mana: %d/%d.  Move: %d/%d. " NL,
	victim->hit,         victim->max_hit,
	victim->mana,        victim->max_mana,
	victim->move,        victim->max_move );

    ch_printf( ch,
	"Lv: %d.  Align: %d.  AC: %d.  Credits: %d." NL,
	victim->top_level,        victim->alignment,
	GET_AC( victim ),    victim->gold);

    i = 1;
    for ( auto* mprg : victim->pIndexData->mudprogs )
    {
	ch_printf( ch, "[" FB_WHITE "%d" PLAIN "]>%s %s" NL
		       "%s" NL, i,
		mprog_type_to_name( mprg->type ),
		mprg->arglist,
		mprg->comlist );
	i++;
    }
    return;
}

/* Opstat - Scryn 8/12*/
DEF_DO_FUN( opstat )
{
    char        arg[MAX_INPUT_LENGTH];
    OBJ_DATA   *obj;
    int		i;


    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	send_to_char( "OProg stat what?" NL, ch );
	return;
    }

    if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
	send_to_char( "You cannot find that." NL, ch );
	return;
    }

    if ( !( obj->pIndexData->progtypes ) )
    {
	send_to_char( "That object has no programs set." NL, ch);
	return;
    }

    ch_printf( ch, "Name: %s.  Vnum: %d." NL,
	obj->name, obj->pIndexData->vnum );

    ch_printf( ch, "Short description: %s@%s@%s@%s@%s@%s." NL,
	    obj->przypadki[0],
	    obj->przypadki[1],
	    obj->przypadki[2],
	    obj->przypadki[3],
	    obj->przypadki[4],
	    obj->przypadki[5]    );

    i = 1;
    for ( auto* mprg : obj->pIndexData->mudprogs )
    {
	ch_printf( ch, "[" FB_WHITE "%d" PLAIN "]>%s %s" NL
		       "%s" NL, i,
		mprog_type_to_name( mprg->type ),
		mprg->arglist,
		mprg->comlist );
	i++;
    }

    return;

}

/* Rpstat - Scryn 8/12 */
DEF_DO_FUN( rpstat )
{
    int		i;

    if ( !( ch->in_room->progtypes ) )
    {
	send_to_char( "This room has no programs set." NL, ch);
	return;
    }

    ch_printf( ch, "Name: %s.  Vnum: %d." NL,
	ch->in_room->name, ch->in_room->vnum );

    i = 1;
    for ( auto* mprg : ch->in_room->mudprogs )
    {
	ch_printf( ch, "[" FB_WHITE "%d" PLAIN "]>%s %s" NL
		       "%s" NL, i,
		mprog_type_to_name( mprg->type ),
		mprg->arglist,
		mprg->comlist );
	i++;
    }
    return;
}



/* Prints the argument to all the rooms around the mobile */
DEF_DO_FUN( mpasound )
{
    ROOM_INDEX_DATA *was_in_room;
    int		     actflags;

    MPCOMMAND; //do_mpasound" );

    if ( argument[0] == '\0' )
    {
	progbug( "Mpasound - No argument", ch );
	return;
    }

    actflags = ch->act;
    REMOVE_BIT(ch->act, ACT_SECRETIVE);
    was_in_room = ch->in_room;
    for ( auto* pexit : was_in_room->exits )
    {
	if ( pexit->to_room
	&&   pexit->to_room != was_in_room )
	{
	   ch->in_room = pexit->to_room;
	   MOBtrigger  = false;
	   act( COL_SAY, argument, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->act = actflags;
    ch->in_room = was_in_room;
    return;
}

/* lets the mobile kill any player or mobile without murder*/

DEF_DO_FUN( mpkill )
{
    char      arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;

    MPCOMMAND; //do_mpkill" );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	progbug( "MpKill - no argument", ch );
	return;
    }

    if ( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
    {
	progbug( "MpKill - Victim not in room", ch );
	return;
    }

    if ( victim == ch )
    {
	progbug( "MpKill - Bad victim to attack", ch );
	return;
    }

    if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
    {
	progbug( "MpKill - Charmed mob attacking master", ch );
	return;
    }

    if ( ch->position == POS_FIGHTING )
    {
	progbug( "MpKill - Already fighting", ch );
	return;
    }

    multi_hit( ch, victim, TYPE_UNDEFINED );
    return;
}


/* lets the mobile destroy an object in its inventory
   it can also destroy a worn object and it can destroy
   items using all.xxxxx or just plain all of them */

DEF_DO_FUN( mpjunk )
{
    char      arg[ MAX_INPUT_LENGTH ];
    OBJ_DATA *obj;

    MPCOMMAND; //do_mpjunk" );

    one_argument( argument, arg );

    if ( arg[0] == '\0')
    {
	progbug( "Mpjunk - No argument", ch );
	return;
    }

    if ( str_cmp( arg, "all" ) && str_prefix( "all.", arg ) )
    {
	if ( ( obj = get_obj_wear( ch, arg ) ) != NULL )
	{
	   unequip_char( ch, obj );
	   extract_obj( obj );
	   return;
	}
	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	   return;
	extract_obj( obj );
    }
    else
    {
	auto carrying_snapshot = ch->carrying;
	for ( auto* obj : carrying_snapshot )
	{
	    if ( arg[3] == '\0' || is_name( &arg[4], obj->name ) )
	    {
		if ( obj->wear_loc != WEAR_NONE)
		    unequip_char( ch, obj );
		extract_obj( obj );
	    }
	}
    }

    return;

}

/*
 * This function examines a text string to see if the first "word" is a
 * color indicator (e.g. _red, _whi_, _blu).  -  Gorog
 */
int get_color(char *argument)    /* get color code from command string */
{
   char color[MAX_INPUT_LENGTH];
   char *cptr;
   static char const * color_list=
         "_bla_red_dgr_bro_dbl_pur_cya_cha_dch_ora_gre_yel_blu_pin_lbl_whi";
   static char const * blink_list=
         "*bla*red*dgr*bro*dbl*pur*cya*cha*dch*ora*gre*yel*blu*pin*lbl*whi";

   one_argument (argument, color);
   if (color[0]!='_' && color[0]!='*') return 0;
   if ( (cptr = strstr((char*)color_list, color)) )
     return (cptr - color_list) / 4;
   if ( (cptr = strstr((char*)blink_list, color)) )
     return (cptr - blink_list) / 4;
   return 0;
}


/* prints the message to everyone in the room other than the mob and victim */

DEF_DO_FUN( mpechoaround )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    int        actflags;
    int     color;

    MPCOMMAND; //do_mpechoaround" );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	progbug( "Mpechoaround - No argument", ch );
	return;
    }

    if ( !( victim=get_char_room_mp( ch, arg ) ) )
    {
	progbug( "Mpechoaround - victim does not exist", ch );
	return;
    }

    actflags = ch->act;
    REMOVE_BIT(ch->act, ACT_SECRETIVE);

    if ( (color = get_color(argument)) )
    {
	argument = one_argument( argument, arg );
	act( PLAIN, argument, ch, NULL, victim, TO_NOTVICT );
    }
    else
	act( PLAIN, argument, ch, NULL, victim, TO_NOTVICT );

    ch->act = actflags;
}


/* prints message only to victim */

DEF_DO_FUN( mpechoat )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    int        actflags;
    int     color;

    MPCOMMAND; //do_mpechoat" );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	progbug( "Mpechoat - No argument", ch );
	return;
    }

    if ( !( victim = get_char_room_mp( ch, arg ) ) )
    {
	progbug( "Mpechoat - victim does not exist", ch );
	return;
    }

    actflags = ch->act;
    REMOVE_BIT(ch->act, ACT_SECRETIVE);

    if ( (color = get_color(argument)) )
    {
	argument = one_argument( argument, arg );
	act( PLAIN, argument, ch, NULL, victim, TO_VICT );
    }
    else
	act( PLAIN, argument, ch, NULL, victim, TO_VICT );

    ch->act = actflags;
}


/* prints message to room at large. */

DEF_DO_FUN( mpecho )
{
    char       arg1 [MAX_INPUT_LENGTH];
    int     color;
    int        actflags;

    MPCOMMAND; //do_mpecho" );

    if ( argument[0] == '\0' )
    {
	progbug( "Mpecho - called w/o argument", ch );
	return;
    }

    actflags = ch->act;
    REMOVE_BIT(ch->act, ACT_SECRETIVE);

    if ( (color = get_color(argument)) )
    {
	argument = one_argument ( argument, arg1 );
	act( PLAIN, argument, ch, NULL, NULL, TO_ROOM );
    }
    else
	act( COL_ACTION, argument, ch, NULL, NULL, TO_ROOM );

    ch->act = actflags;
}

/* Thanos: pozwala mobowi wykonac czynnosc bez pokazywania komunikatu o tym */
/* Sk�adnia: mpnoecho <komenda> */
DEF_DO_FUN( mpnoecho )
{

    MPCOMMAND; //do_mpnoecho" );

    if ( argument[0] == '\0' )
    {
	progbug( "MpNoEcho - Bad argument", ch );
	return;
    }

    noEcho = true;
    interpret( ch, argument );
    noEcho = false;
    return;
}

/* Sk�adnia: mpresetroom -- resetuje pok�j */
DEF_DO_FUN( mpresetroom )
{
    MPCOMMAND; //do_mpresetroom" );

    if ( !ch->in_room )
    {
	progbug( "MpResetRoom - NULL ch->in_room!", ch );
	return;
    }

    forceReset=true;
    reset_room( ch->in_room );
    forceReset=false;
    return;
}

/* Sk�adnia: mpaddtowanted <ch> <type> [planet] */
DEF_DO_FUN( mpaddtowanted )
{
    PLANET_DATA *	planet = 0;
    CHAR_DATA *		victim;
    char      		arg1[ MAX_INPUT_LENGTH ];
    char      		arg2[ MAX_INPUT_LENGTH ];
    int			value;

    MPCOMMAND; //do_mpaddtowanted" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if( arg1[0] == '\0'
    ||  arg2[0] == '\0' )
    {
	progbug( "MpAddToWanted - too few arguments", ch );
	return;
    }

    if ( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
    {
	progbug( "MpAddToWanted - Victim not in room", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	progbug( "MpAddToWanted - victim is not a player", ch );
	return;
    }

    if( (value=flag_value( crime_flags_list, arg2 ))==NO_FLAG )
    {
	progbug( "MpAddToWanted - bad crime type", ch );
	return;
    }

    if( *argument )
    {
	planet = get_planet( argument );
    }
    else
    {
	if( victim->in_room
	&&  victim->in_room->area
	&&  victim->in_room->area->planet )
    	    planet = victim->in_room->area->planet;
    }

    if( !planet )
    {
	progbug( "MpAddToWanted - planet not exists", ch );
	return;
    }

    crime_to_char( victim, planet->name, value );
    return;
}

/* Sk�adnia: mpremfromwanted <ch> <type> [planet] */
DEF_DO_FUN( mpremfromwanted )
{
    PLANET_DATA *	planet = 0;
    CHAR_DATA *		victim;
    char      		arg1[ MAX_INPUT_LENGTH ];
    char      		arg2[ MAX_INPUT_LENGTH ];
    int			value;

    MPCOMMAND; //do_mpremfromwanded" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if( arg1[0] == '\0'
    ||  arg2[0] == '\0' )
    {
	progbug( "MpRemFromWanted - too few arguments", ch );
	return;
    }

    if ( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
    {
	progbug( "MpRemFromWanted - Victim not in room", ch );
	return;
    }

    if ( IS_NPC( victim ) )
    {
	progbug( "MpRemFromWanted - victim is not a player", ch );
	return;
    }

    if( (value=flag_value( crime_flags_list, arg2 ))==NO_FLAG )
    {
	progbug( "MpRemFromWanted - bad crime type", ch );
	return;
    }

    if( *argument )
    {
	planet = get_planet( argument );
    }
    else
    {
	if( victim->in_room
	&&  victim->in_room->area
	&&  victim->in_room->area->planet )
    	    planet = victim->in_room->area->planet;
    }

    if( !planet )
    {
	progbug( "MpRemFromWanted - planet not exists", ch );
	return;
    }

    crime_remove( victim, planet->name, value );
    return;
}

/* lets the mobile load an item or mobile.  All items
are loaded into inventory.  you can specify a level with
the load object portion as well. */
DEF_DO_FUN( mpmload )
{
    char            arg[ MAX_INPUT_LENGTH ];
    MOB_INDEX_DATA *pMobIndex;
    CHAR_DATA      *victim;

    MPCOMMAND; //do_mpmload" );

    one_argument( argument, arg );

    if ( arg[0] == '\0' || !is_number(arg) )
    {
	progbug( "Mpmload - Bad vnum as arg", ch );
	return;
    }

    if ( ( pMobIndex = get_mob_index( atoi( arg ) ) ) == NULL )
    {
	progbug( "Mpmload - Bad mob vnum", ch );
	return;
    }

    victim = create_mobile( pMobIndex );
    char_to_room( victim, ch->in_room );
    return;
}

DEF_DO_FUN( mpoload )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    OBJ_INDEX_DATA *pObjIndex;
    OBJ_DATA       *obj;
    int             level;
    int		    timer = 0;

    MPCOMMAND; //do_mpoload" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || !is_number( arg1 ) )
    {
	progbug( "Mpoload - Bad syntax", ch );
	return;
    }

    if ( arg2[0] == '\0' )
	level = get_trust( ch );
    else
    {
	/*
	 * New feature from Alander.
	 */
	if ( !is_number( arg2 ) )
	{
	    progbug( "Mpoload - Bad level syntax", ch );
	    return;
	}
	level = atoi( arg2 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    progbug( "Mpoload - Bad level", ch );
	    return;
	}

	/*
	 * New feature from Thoric.
	 */
	timer = atoi( argument );
	if ( timer < 0 )
	{
	   progbug( "Mpoload - Bad timer", ch );
	   return;
	}
    }

    if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
    {
	progbug( "Mpoload - Bad vnum arg", ch );
	return;
    }

    obj = create_object( pObjIndex, level );
    obj->timer = timer;
    if ( CAN_WEAR(obj, ITEM_TAKE) )
	obj_to_char( obj, ch );
    else
	obj_to_room( obj, ch->in_room );

    return;
}

/* Sk�adnia: mpqoload $n <vnum> [level] [timer] */
DEF_DO_FUN( mpqoload )
{
    char 		arg1	[ MAX_INPUT_LENGTH ];
    char 		arg2	[ MAX_INPUT_LENGTH ];
    char 		arg3	[ MAX_INPUT_LENGTH ];
    char 		arg4	[ MAX_INPUT_LENGTH ];
    int			level;
    int			timer;
    int			vnum;
    CHAR_DATA *		victim;
    OBJ_INDEX_DATA *	pObjIndex;
    OBJ_DATA *		obj;
    QUEST_OBJ_DATA *	qObj;
    QUEST_DATA *	quest;

    MPCOMMAND; //do_mpqoload" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if( !*arg1 || !*arg2 || !is_number( arg2 ) )
    {
	progbug( "MpQOload - Bad syntax", ch );
	return;
    }

    if ( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
    {
	progbug( "MpQOload - QuestPlayer not in room", ch );
	return;
    }
    if( IS_NPC( victim ) )
    {
	progbug( "MpQOload - Victim is not a player", ch );
	return;
    }
    if( !(quest = victim->inquest) )
    {
	progbug( "MpQOload - Player not in quest", ch );
	return;
    }

    if( !belongs_to_quest( ch, quest ) )
    {
        progbug( "MpQMload - Mob doesn't belong to Player's quest", ch );
        return;
    }

    vnum = atoi( arg2 );

    if ( ( pObjIndex = get_obj_index( vnum ) ) == NULL )
    {
	progbug( "MpQOload - Bad vnum arg", ch );
	return;
    }

    argument = one_argument( argument, arg3 );
    if( *arg3 )
    {
    	if ( !is_number( arg3 ) )
	{
	    progbug( "MpQOload - Bad level syntax", ch );
	    return;
	}
	level = atoi( arg3 );
	if ( level < 0 || level > get_trust( ch ) )
	{
	    progbug( "MpQOload - Bad level", ch );
	    return;
	}
    }
    else
    	level = get_trust( ch );

    argument = one_argument( argument, arg4 );

    timer = atoi( arg4 );
    if ( timer < 0 )
    {
       progbug( "Mpoload - Bad timer", ch );
       return;
    }

    qObj = qobj_invoke( vnum, quest, level );
    obj = qObj->obj;

    if( timer == 1 )
        SET_BIT( obj->extra_flags, ITEM_QUEST );
    else
        obj->timer = timer;

    if( can_see_obj( ch, obj ) )
    {
	if ( CAN_WEAR(obj, ITEM_TAKE) )
	    obj_to_char( obj, ch );
	else
	    obj_to_room( obj, ch->in_room );
    }
/* JESLI MOBY ZAPROSZONE NIE WIDZA KLAMOTOW QUESTOWYCH NIE MOGA MIEC ICH
   W INVENTORY !!! */
    else
        obj_to_room( obj, ch->in_room );

    return;
}

/* Sk�adnia: mpqmload $n <vnum> */
DEF_DO_FUN( mpqmload )
{
    char            	arg	[ MAX_INPUT_LENGTH ];
    int			vnum;
    MOB_INDEX_DATA *	pMobIndex;
    QUEST_MOB_DATA *	qMob;
    CHAR_DATA *		mob;
    CHAR_DATA *	    	victim;
    QUEST_DATA *	quest;


    MPCOMMAND; //do_mpqmload" );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' || !is_number(argument) )
    {
	progbug( "MpQMload - Bad syntax", ch );
	return;
    }

    vnum = atoi( argument );
    if ( ( pMobIndex = get_mob_index( vnum ) ) == NULL )
    {
	progbug( "MpQMload - Bad mob vnum", ch );
	return;
    }

    if ( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
    {
        progbug( "MpQMload - QuestPlayer not in room", ch );
        return;
    }
    if( IS_NPC( victim ) )
    {
        progbug( "MpQMload - Victim is not a player", ch );
        return;
    }
    if( !(quest = victim->inquest) )
    {
        progbug( "MpQMload - Player not in quest", ch );
        return;
    }

    if( !belongs_to_quest( ch, quest ) )
    {
        progbug( "MpQMload - Mob doesn't belong to Player's quest", ch );
        return;
    }

    qMob = qmob_invoke( vnum, quest );
    mob = qMob->mob;

    char_to_room( mob, ch->in_room );
    return;
}

/* lets the mobile purge all objects and other npcs in the room,
   or purge a specified object or mob in the room.  It can purge
   itself, but this had best be the last command in the MUDprogram
   otherwise ugly stuff will happen */

DEF_DO_FUN( mppurge )
{
    char       arg[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    MPCOMMAND; //do_mppurge" );

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
	/* 'purge' */
	auto people_snapshot = ch->in_room->people;
	for ( auto* victim : people_snapshot )
	{
	    if ( IS_NPC( victim ) && victim != ch )
	      extract_char( victim, true );
	}
	while ( !ch->in_room->contents.empty() )
	   extract_obj( ch->in_room->contents.front() );

	return;
    }

    if ( (victim = get_char_room( ch, arg )) == NULL )
    {
	if ( (obj = get_obj_here( ch, arg )) != NULL )
	    extract_obj( obj );
	else
	    progbug( "Mppurge - Bad argument", ch );
	return;
    }

    if ( !IS_NPC( victim ) )
    {
	progbug( "Mppurge - Trying to purge a PC", ch );
	return;
    }


/* 	Eeee
	Niech sie mobki ucz� purgeowa� -
	kto to je ma purgeowa� jak nie one   ;)
    							    -- Thanos
    if ( victim == ch )
    {
    	progbug( "Mppurge - Trying to purge oneself", ch );
    	return;
    }
*/

    if ( IS_NPC( victim ) && victim->pIndexData->vnum == 3 )
    {
        progbug( "Mppurge: trying to purge supermob", ch );
	return;
    }

    extract_char( victim, true );
    return;
}


/* Allow mobiles to go wizinvis with programs -- SB */

DEF_DO_FUN( mpinvis )
{
    char arg[MAX_INPUT_LENGTH];
    int level;

    MPCOMMAND; //do_mpinvis" );

    argument = one_argument( argument, arg );
    if ( *arg )
    {
        if ( !is_number( arg ) )
        {
           progbug( "Mpinvis - Non numeric argument ", ch );
           return;
        }
        level = atoi( arg );
		level = URANGE( 0, level, MAX_LEVEL );

		ch->mobinvis = level;
		ch_printf( ch, "Mobinvis level set to %d." NL, level );
		return;
    }

    if ( ch->mobinvis < 2 )
      ch->mobinvis = ch->top_level;

    if ( IS_SET(ch->act, ACT_MOBINVIS) )
    {
        REMOVE_BIT(ch->act, ACT_MOBINVIS);
	act(COL_IMMORT, "$n slowly fades into existence.", ch, NULL, NULL,TO_ROOM );
	send_to_char( "You slowly fade back into existence." NL, ch );
    }
    else
    {
        SET_BIT(ch->act, ACT_MOBINVIS);
	act( COL_IMMORT, "$n slowly fades into thin air.", ch, NULL, NULL, TO_ROOM );
        send_to_char( "You slowly vanish into thin air." NL, ch );
    }
    return;
}

/* lets the mobile goto any location it wishes that is not private */

DEF_DO_FUN( mpgoto )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;

    MPCOMMAND; //do_mpgoto" );

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
	progbug( "Mpgoto - No argument", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	progbug( "Mpgoto - No such location", ch );
	return;
    }

    if ( ch->fighting )
	stop_fighting( ch, true );

    char_from_room( ch );
    char_to_room( ch, location );

    return;
}

/* lets the mobile do a command at another location. Very useful */

DEF_DO_FUN( mpat )
{
    char             arg[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *location;
    ROOM_INDEX_DATA *original;
    MPCOMMAND; //do_mpat" );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	progbug( "Mpat - Bad argument", ch );
	return;
    }

    if ( ( location = find_location( ch, arg ) ) == NULL )
    {
	progbug( "Mpat - No such location", ch );
	return;
    }

    original = ch->in_room;
    /* Save position in room's people list so we can restore it later */
    auto it_pos = std::find(original->people.begin(), original->people.end(), ch);
    auto it_next = (it_pos != original->people.end()) ? std::next(it_pos) : original->people.end();
    char_from_room( ch );
    char_to_room( ch, location );

    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( auto* wch : char_list )
	if ( wch == ch )
	{
	    char_from_room( ch );
	    char_to_room( ch, original );

        /* BUGFIX by Thanos:
         * po wykonaniu komendy progowej mob cz�sto u�ywa mpat, a co
         * za tym idzie robi co� w innej lokacji, czyli wypina si� z listy
         * lokacji aktualnej. Po powrocie zostaje wpi�ty na sam jej koniec,
         * a co za tym idzie, p�tla lec�ca po wszystkich w pomieszczeniu
         * zn�w trafia na niego. Taki manewr grozi zap�tlaniem i duplikowaniem
         * si� wykona� prog�w. M�j spos�b: wepnijmy mobka tam gdzie by�.
         * O ile ofcoz sa jeszcze ci, obok ktorych stal.
         */
	    if( it_next != original->people.end() )
	    {
		original->people.remove( ch );
		original->people.insert( it_next, ch );
	    }
	    break;
	}

    return;
}

/* by Trog, listy u mobow, tu sie zaczyna */

void save_nameslist( CHAR_DATA *ch )
{
    FILE *fp;
    char buf[MSL];

    sprintf( buf, "%snl_%d", NAMESLIST_DIR, ch->pIndexData->vnum );
    if( ch->pIndexData->namescount == 0 )
    {
	unlink( buf );
	return;
    }

    if( !(fp=fopen( buf, "w" )) )
    {
	bug( "save_nameslist: can't open file for writing.", 0 );
	return;
    }

    fprintf( fp, "%d", ch->pIndexData->namescount );
    for( auto* nameslist : ch->pIndexData->nameslists )
	fprintf( fp, " %s~", nameslist->name );
    fprintf( fp, "\n" );
    fclose(fp);

    return;
}

void fread_nameslist( MOB_INDEX_DATA *pMobIndex )
{
    FILE *fp;
    char buf[MSL];
    NAMESLIST_DATA *nameslist;
    int i;

    sprintf( buf, "%snl_%d", NAMESLIST_DIR, pMobIndex->vnum );
    if( !(fp=fopen( buf, "r" )) )
        return;

    pMobIndex->namescount = fread_number( fp );
    for( i = 1; i<= pMobIndex->namescount; i++ )
    {
	CREATE( nameslist, NAMESLIST_DATA, 1 );
	pMobIndex->nameslists.push_back( nameslist );
	nameslist->name = fread_string( fp );
    }
    fclose(fp);

    return;
}


void clear_nameslist( CHAR_DATA *ch )
{
    for( auto* nameslist : ch->pIndexData->nameslists )
    {
	free_nameslist( nameslist );
    }
    ch->pIndexData->nameslists.clear();
    ch->pIndexData->namescount = 0;

    return;
}

DEF_DO_FUN( mpclrnameslist )
{
    MPCOMMAND; //do_mpclrnameslist" );

    if( ch == supermob )
    {
	progbug( "Mpclrnameslist used in object or room", ch );
	return;
    }

    if( nameslistempty( ch ) )
	return;

    clear_nameslist( ch );
    save_nameslist( ch );
    return;
}

bool nameslistfull( CHAR_DATA *ch )
{
    if( ch->pIndexData->namescount == 10 )
	return true;
    return false;
}

bool nameslistempty( CHAR_DATA *ch )
{
    if( ch->pIndexData->namescount > 0 )
	return false;
    else
	return true;
}

bool isinnameslist( CHAR_DATA *ch, char *arg )
{
    for( auto* nameslist : ch->pIndexData->nameslists )
	if( !str_cmp( nameslist->name, arg ) )
	    return true;
    return false;
}

DEF_DO_FUN( mpaddtolist )
{
    CHAR_DATA		*victim;
    NAMESLIST_DATA	*nameslist;
    char		arg[MAX_INPUT_LENGTH];

    MPCOMMAND; //do_mpaddtolist" );

    argument = one_argument( argument, arg );

    if( ch == supermob )
    {
	progbug( "Mpaddtolist used in object or room", ch );
	return;
    }

    if( !( victim = get_char_room_mp( ch, arg ) ) || IS_NPC( victim ) )
    {
	progbug( "mpaddtolist: victim not in room or victim is a mobile", ch );
	return;
    }

    if( isinnameslist( ch, arg ) )
	return;


    CREATE( nameslist, NAMESLIST_DATA, 1 );
    ch->pIndexData->nameslists.push_front( nameslist );
    if( ch->pIndexData->namescount == 10 )
    {
	NAMESLIST_DATA *tmp = ch->pIndexData->nameslists.back();
	ch->pIndexData->nameslists.pop_back();
	free_nameslist( tmp );
    }
    else
	ch->pIndexData->namescount++;
    STRDUP( nameslist->name, arg );
    save_nameslist( ch );
    return;
}

DEF_DO_FUN( mpremfromlist )
{
    CHAR_DATA		*victim;
    char		arg[MAX_INPUT_LENGTH];

    MPCOMMAND; //do_mpremfromist" );

    if( nameslistempty( ch ) )
	return;

    argument = one_argument( argument, arg );

    if( ch == supermob )
    {
	progbug("Mpremfromlist used in object or room", ch );
	return;
    }

    if( !( victim = get_char_room_mp( ch, arg ) ) || IS_NPC( victim ) )
    {
	progbug("mpremfromlist: victim not in room or victim is a mobile", ch );
	return;
    }

    for( auto it = ch->pIndexData->nameslists.begin(); it != ch->pIndexData->nameslists.end(); ++it )
	if( !str_cmp( (*it)->name, arg ) )
	{
	    free_nameslist( *it );
	    ch->pIndexData->nameslists.erase( it );
	    ch->pIndexData->namescount--;
	    save_nameslist( ch );
	    break;
	}

    return;
}

/* by Trog, listy u mobow, koniec */

DEF_DO_FUN( mpadvance )
{ //by Trog, bylo tu tylko return i dlatego nie dzialalo :P
    CHAR_DATA *victim;
    ROOM_INDEX_DATA *location;
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    int ability;

    buf[0]='\0';
    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    location=ch->in_room;

    MPCOMMAND; //do_mpaddvance" );

    if( arg[0] == '\0' )
	{
	sprintf( buf, "mpadvance: null arg1, Room: %d",
		location->vnum );
	progbug( buf, ch );
	return;
	}
    if( arg2[0] == '\0' )
        {
        sprintf( buf, "mpadvance: null arg2, Room: %d",
                location->vnum );
        progbug( buf, ch );
        return;
        }
    if( ( victim=get_char_room_mp( ch, arg ) )==NULL )
	{
	sprintf( buf, "mpadvance: victim not in room, Room: %d",
		location->vnum );
	progbug( buf, ch );
	return;
	}

    for( ability=0; ability < MAX_ABILITY; ability++ )
	if( ability == victim->main_ability )
	    {
	    if( ( victim->skill_level[ability] < atoi(arg2) )
		&& ( max_level( victim, ability ) > atoi(arg2) ) )
		{
	    	victim->skill_level[ability] = atoi(arg2);
		if( victim->top_level < victim->skill_level[ability] )
		    victim->top_level = victim->skill_level[ability];
		victim->experience[ability]=exp_level( victim->skill_level[ability]+(atoi(arg2)-1) );
	    	ch_printf( victim, "&W* Opanowujesz nowe tajniki %s !!! Znasz je ju� na %d poziomie! *&w" NL,
		class_table[ability].przypadki[1], victim->skill_level[ability] );
		return;
		}
	    break;
	    }

    return;
}

void do_mpinduct( CHAR_DATA *ch, char *argument ) //byTrog
{
	CHAR_DATA	*victim;
	CLAN_DATA	*clan;
	char		arg[MIL];
	char		buf[MSL];
	char		buf2[MSL];

	MPCOMMAND; //do_mpinduct" );

	argument = one_argument( argument, arg );

	if( !*arg )
	{
		progbug( "Mpinduct: arg1 not specified", ch );
		return;
	}

	if( !*argument )
	{
		progbug( "Mpinduct: arg2 not specified", ch );
		return;
	}

	if( !(victim = get_char_room( ch, arg )) )
	{
		progbug( "Mpinduct: victim not in room", ch );
		return;
	}

	if( !(clan = get_clan( argument )) )
	{
		progbug( "Mpinduct: no such clan", ch );
		return;
	}

	if( IS_NPC( victim ) || get_member( clan, victim->name ) )
		return;

	induct_member( clan, victim );
	buf[0]=buf2[0]='\0';
	sprintf( buf2, "Council %s", LEADER_NAME( clan ) );
	sprintf( buf, "W szeregach Twoje%s %s pojawia si� nowa osoba." NL
		"Powinna by� ona powitana w spos�b, kt�ry uwa�asz za najlepszy." NL
		"Imi� tej osoby brzmi: %s." NL, CLANSUFFIX( clan, "go", "j" ), CLANTYPE( clan, 1 ),
		victim->przypadki[0] );
	note( ch->przypadki[0], buf2, "Nowy cz�onek.", buf );
}

void do_mpoutcast( CHAR_DATA *ch, char *argument ) //byTrog
{
	CHAR_DATA	*victim;
	CLAN_DATA	*clan;
	char		arg[MIL];
	char		buf[MSL];
	char		buf2[MSL];

	MPCOMMAND; //do_mpoutcast" );

	argument = one_argument( argument, arg );

	if( !*arg )
	{
		progbug( "Mpoutcast: arg1 not specified", ch );
		return;
	}

	if( !*argument )
	{
		progbug( "Mpoutcast: arg2 not specified", ch );
		return;
	}

	if( !(victim = get_char_room( ch, arg )) )
	{
		progbug( "Mpoutcast: victim not in room", ch );
		return;
	}

	if( !(clan = get_clan( argument )) )
	{
		progbug( "Mpoutcast: no such clan", ch );
		return;
	}

	if( IS_NPC( victim ) || !get_member( clan, victim->name ) )
		return;

	outcast_member( victim->pcdata->clan, victim );
	buf2[0]=buf[0]='\0';
	sprintf( buf2, "Council %s", LEADER_NAME( clan ) );
	sprintf( buf, "Z szereg�w twoje%s %s zosta�a usuni�ta pewna osoba." NL
		"Powinna si� liczy� z wszelkimi nast�pstwami." NL
		"Imi� tej osoby brzmi: %s." NL, CLANSUFFIX( clan, "go", "j" ), CLANTYPE( clan, 1 ),
		victim->przypadki[0] );
	note( ch->przypadki[0], buf2, "Usuni�ty cz�onek.", buf );
}


/* Modyfikuje liczbe punktow mv gracza - Ganis */
DEF_DO_FUN( mptire )
{
	char			arg1[ MAX_INPUT_LENGTH ];
	char			arg2[ MAX_INPUT_LENGTH ];
	CHAR_DATA		*victim;
	int				mv;

	MPCOMMAND;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !*arg1 || !*arg2 )
	{
		progbug( "Mptire - Missing argument", ch );
		return;
	}

	if ( !(victim = get_char_room_mp( ch, arg1 )) )
	{
		progbug( "Mptire - Victim not in room", ch );
		return;
	}

	if ( victim == ch ) return;

	if ( !is_number( arg2 ) )
	{
		progbug( "Mptire - Syntax error", ch );
		return;
	}

	mv = -atoi(arg2) + victim->move;

	victim->move = mv > victim->max_move ? victim->max_move :
					mv < 0 ? 0 : mv;
}

/* lets the mobile transfer people.  the all argument transfers
   everyone in the current room to the specified location */
DEF_DO_FUN( mptransfer )
{
    char             arg1[ MAX_INPUT_LENGTH ];
    char             arg2[ MAX_INPUT_LENGTH ];
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *location;
    CHAR_DATA       *victim;

    MPCOMMAND; //do_mptransfer" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
	progbug( "Mptransfer - Bad syntax", ch );
	return;
    }

    /* Put in the variable nextinroom to make this work right. -Narn */
    if ( !str_cmp( arg1, "all" ) )
    {
	auto people_snapshot = ch->in_room->people;
	for ( auto* victim : people_snapshot )
	{
	    if ( victim != ch
	    &&   !NOT_AUTHED(victim)
	    &&   can_see( ch, victim ) )
	    {
		sprintf( buf, "%s %s", victim->name, arg2 );
		do_mptransfer( ch, buf );
	    }
	}
	return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    if ( arg2[0] == '\0' )
    {
	location = ch->in_room;
    }
    else
    {
	if ( ( location = find_location( ch, arg2 ) ) == NULL )
	{
	    progbug( "Mptransfer - No such location", ch );
	    return;
	}

	if ( room_is_private( ch, location ) )
	{
	    progbug( "Mptransfer - Private room", ch );
	    return;
	}
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
	progbug( "Mptransfer - No such person", ch );
	return;
    }

    if ( !victim->in_room )
    {
	progbug( "Mptransfer - Victim in Limbo", ch );
	return;
    }

    if (NOT_AUTHED(victim) && location->area != victim->in_room->area)
    {
 	progbug( "Mptransfer - transferring unauthorized player", ch);
	return;
    }


/* Trog: jesli lokacja docelowa jest prototype to nie transferujemy */
	if( IS_SET( location->room_flags, ROOM_PROTOTYPE ) )
	{
		progbug( "Mptransfer - Destination location is PROTOTYPE", ch );
		return;
	}

    if ( victim->fighting )
	stop_fighting( victim, true );

    uncrew( ship_from_room( victim->in_room ),victim);
    char_from_room( victim );
    char_to_room( victim, location );

    return;
}

/* lets the mobile force someone to do something.  must be mortal level
   and the all argument only affects those in the room with the mobile */

DEF_DO_FUN( mpforce )
{
    char arg[ MAX_INPUT_LENGTH ];

    MPCOMMAND; //do_mpforce" );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	progbug( "Mpforce - Bad syntax", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	for ( auto* vch : ch->in_room->people )
	    if ( get_trust( vch ) < get_trust( ch ) && can_see( ch, vch ) )
		interpret( vch, argument );
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
	{
	    progbug( "Mpforce - No such victim", ch );
	    return;
	}

	if ( victim == ch )
	{
	    progbug( "Mpforce - Forcing oneself", ch );
	    return;
	}

	if ( !IS_NPC( victim )
	&& ( !victim->desc )
	&& IS_IMMORTAL( victim ) )
	{
	    progbug( "Mpforce - Attempting to force link dead immortal", ch );
	    return;
	}

        interpret( victim, argument );
    }

    return;
}

DEF_DO_FUN( mpsuggest )
{
    char arg[ MAX_INPUT_LENGTH ];

    MPCOMMAND; //do_mpforce" );

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
	progbug( "Mpsuggest - Bad syntax", ch );
	return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
	for ( auto* vch : ch->in_room->people )
	    if ( get_trust( vch ) < get_trust( ch ) && can_see( ch, vch ) )
		interpret( vch, argument );
    }
    else
    {
	CHAR_DATA *victim;

	if ( ( victim = get_char_room_mp( ch, arg ) ) == NULL )
	{
	    progbug( "Mpsuggest - No such victim", ch );
	    return;
	}

	if ( victim == ch )
	{
	    progbug( "Mpsuggest - Forcing oneself", ch );
	    return;
	}

	if ( !IS_NPC( victim )
	&& ( !victim->desc )
	&& IS_IMMORTAL( victim ) )
	{
	    progbug( "Mpsuggest - Attempting to force link dead immortal", ch );
	    return;
	}


	/*
	 * Thanos: taka zmiana.
	 * Gracz po mpforce nie wykonuje komendy automatyczne, tylko
	 * czeka az minie mu ws. Zeby tak by�o wstawiamy przed jego
	 * incomm komende zadana przez moba - fajne to jest przy
	 * mpdelay. Np.
         *
	 * mpechoat $n Zastanawiasz sie i...
	 * mpdelay $n 1
	 * mpforce $n s
	 * mpechoat $n odchodzisz st�d.
	 */
	if( victim->desc )
	{
	    char _buf[MSL];

	    _buf[0] = '|';
	    _buf[1] = '\0';
	    strcat( _buf, victim->desc->incomm );

	    victim->desc->incomm[0] = '\0';
	    strcat( victim->desc->incomm, argument );
	    strcat( victim->desc->incomm, _buf );
	}
	else
	    interpret( victim, argument );
    }

    return;
}



/*
 *  Haus' toys follow:
 */


/*
 * syntax:  mppractice victim spell_name max%
 *
 */
DEF_DO_FUN( mp_practice )
{
}

/*
 * syntax: mpslay (character)
 */
DEF_DO_FUN( mp_slay )
{
    return;
}

DEF_DO_FUN( mpdelay )
{
    char	arg	[MIL];
    CHAR_DATA *	victim;
    int		delay;

    MPCOMMAND; //do_mpdelay" );

    argument = one_argument( argument, arg );

    if( arg[0] == '\0' )
    {
	progbug( "MpDelay - NULL argument", ch );
	return;
    }

    if( (victim=get_char_room_mp( ch, arg )) == NULL )
    {
	progbug( "MpDelay - Victim not in room", ch );
	return;
    }

    if( victim == ch || IS_NPC(victim) )
    {
	progbug( "MpDelay - Bad victim to delay", ch );
	return;
    }

    if( IS_SET(victim->act, PLR_HOLYLIGHT) )
	return;

    if( IS_AFFECTED( ch, AFF_CHARM ) && victim == ch->master )
    {
	progbug( "MpDelay - Charmed Victim attacking Master", ch );
	return;
    }

    if( argument[0] == '\0' )
    {
	progbug( "MpDelay - Delay not specified", ch );
	return;
    }

    if( (delay=atoi( argument )) <=0 || delay >= 100 )
    {
	progbug( "MpDelay - Bad delay: (0:100)", ch );
	return;
    }

    WAIT_STATE( victim, delay );
    return;
}


/* Added by Thanos */
/* Dodaje opcjonalny drugi argument - czas trwania stuna - Ganis */
DEF_DO_FUN( mpstun )
{
	char			arg1[ MAX_INPUT_LENGTH ];
	char			arg2[ MAX_INPUT_LENGTH ];
	CHAR_DATA *		victim;
	AFFECT_DATA 	af;
	int 			duration;

	MPCOMMAND; //do_mpstun" );

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( !*arg1 )
	{
		progbug( "MpStun - no argument", ch );
		return;
	}

	if ( *arg2 && !is_number( arg2 ) )
	{
		progbug( "MpStun - Bad syntax", ch);
		return;
	}

	if ( !*arg2 ) duration = 3;
	else duration = atoi(arg2);

	if ( duration < 0 )
	{
		progbug( "MpStun - Negative duration", ch);
		return;
	}

	if ( !(victim = get_char_room_mp( ch, arg1 )) )
	{
		progbug( "MpStun - Victim not in room", ch );
		return;
	}

	if ( victim == ch )
	{
		progbug( "MpStun - Bad victim to Stun", ch );
		return;
	}

	if ( !IS_NPC(victim) && IS_SET(victim->act, PLR_HOLYLIGHT) )
		return;

	if ( IS_AFFECTED( ch, AFF_CHARM ) && ch->master == victim )
	{
		progbug( "MpStun - Charmed mob attacking master", ch );
		return;
	}

	if ( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
	{
		af.type      	= gsn_stun;
		af.location  	= APPLY_AC;
		af.modifier  	= 20;
		af.duration  	= duration;
		af.bitvector 	= AFF_PARALYSIS;
		affect_to_char	( victim, &af );
		update_pos	( victim );
	}
	return;
}

/*
 * syntax: mpdamage (character) (#hps)
 */
DEF_DO_FUN( mp_damage )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    int dam;

    MPCOMMAND; //do_mp_damage" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
       send_to_char( "mpdamage whom?" NL, ch );
       progbug( "Mpdamage: invalid argument1", ch );
       return;
    }

    if ( arg2[0] == '\0' )
    {
       send_to_char( "mpdamage inflict how many hps?" NL, ch );
       progbug( "Mpdamage: invalid argument2", ch );
       return;
    }

    if ( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
    {
        send_to_char( "Victim must be in room." NL, ch );
        progbug( "Mpdamage: victim not in room", ch );
	return;
    }

    if ( victim == ch )
    {
        send_to_char( "You can't mpdamage yourself." NL, ch );
        progbug( "Mpdamage: trying to damage self", ch );
	return;
    }

    dam = atoi(arg2);

    if( (dam<0) || (dam>32000) )
    {
       send_to_char( "Mpdamage how much?" NL, ch );
       progbug( "Mpdamage: invalid (nonexistent?) argument", ch );
       return;
    }

   /* this is kinda begging for trouble        */
   /*
    * Note from Thoric to whoever put this in...
    * Wouldn't it be better to call damage(ch, ch, dam, dt)?
    * I hate redundant code
    */
    if ( simple_damage(ch, victim, dam, TYPE_UNDEFINED ) == rVICT_DIED )
    {
	stop_fighting( ch, false );
	stop_hating( ch );
	stop_fearing( ch );
	stop_hunting( ch );
    }

    return;
}


/*
 * syntax: mprestore (character) (#hps)                Gorog
 */
DEF_DO_FUN( mp_restore )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    int hp;

    MPCOMMAND; //do_mp_restore" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
       send_to_char( "mprestore whom?" NL, ch );
       progbug( "Mprestore: invalid argument1", ch );
       return;
    }

    if ( arg2[0] == '\0' )
    {
       send_to_char( "mprestore how many hps?" NL, ch );
       progbug( "Mprestore: invalid argument2", ch );
       return;
    }

    if ( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
    {
        send_to_char( "Victim must be in room." NL, ch );
        progbug( "Mprestore: victim not in room", ch );
	return;
    }

    hp = atoi(arg2);

    if( (hp<0) || (hp>32000) )
    {
       send_to_char( "Mprestore how much?" NL, ch );
       progbug( "Mprestore: invalid (nonexistent?) argument", ch );
       return;
    }
    hp += victim->hit;
    victim->hit = (hp > 32000 || hp < 0 || hp > victim->max_hit) ?
                  victim->max_hit : hp;
}

DEF_DO_FUN( mpgain )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    CHAR_DATA *victim;
    long exp;
    int  ability;

    MPCOMMAND; //do_mpgain" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' )
    {
       send_to_char( "mpgain whom?" NL, ch );
       progbug( "Mpgain: invalid argument1", ch );
       return;
    }

    if ( arg2[0] == '\0' )
    {
       send_to_char( "mpgain in what ability?" NL, ch );
       progbug( "Mpgain: invalid argument2", ch );
       return;
    }

    if ( arg3[0] == '\0' )
    {
       send_to_char( "mpgain how much exp?" NL, ch );
       progbug( "Mpgain: invalid argument3", ch );
       return;
    }

    if ( ( victim = get_char_room_mp( ch, arg1 ) ) == NULL )
    {
        send_to_char( "Victim must be in room." NL, ch );
        progbug( "Mpgain: victim not in room", ch );
	return;
    }

    ability = atoi(arg2);
    exp = atoi(arg3);

    if( ability < 0 || ability >= MAX_ABILITY )
    {
       send_to_char( "Mpgain which ability?" NL, ch );
       progbug( "Mpgain: ability out of range", ch );
       return;
    }

    if( (exp < 1) )
    {
       send_to_char( "Mpgain how much?" NL, ch );
       progbug( "Mpgain: experience out of range", ch );
       return;
    }

    exp =  URANGE(1, exp, ( exp_level( victim->skill_level[ability]+1 ) - exp_level( victim->skill_level[ability]) ) );

    ch_printf( victim, "Zdobywasz %ld punkt�w do�wiadczenia %s." NL, exp,
	class_table[ability].przypadki[6]  );
    gain_exp( victim , exp , ability);
    return;
}

/*
 * Syntax mp_open_passage x y z
 *
 * opens a 1-way passage from room x to room y in direction z
 *
 *  won't mess with existing exits
 */
DEF_DO_FUN( mp_open_passage )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *targetRoom, *fromRoom;
    int targetRoomVnum, fromRoomVnum, exit_num;
    EXIT_DATA *pexit;

    MPCOMMAND; //do_mp_open_passage" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
	progbug( "MpOpenPassage - Bad syntax", ch );
	return;
    }

    if( !is_number(arg1) )
    {
	progbug( "MpOpenPassage - Bad syntax", ch );
	return;
    }

    fromRoomVnum = atoi(arg1);
    if(  (fromRoom = get_room_index( fromRoomVnum ) )  ==NULL)
    {
	progbug( "MpOpenPassage - Bad syntax", ch );
	return;
    }

    if( !is_number(arg2) )
    {
	progbug( "MpOpenPassage - Bad syntax", ch );
	return;
    }

    targetRoomVnum = atoi(arg2);
    if(  (targetRoom = get_room_index( targetRoomVnum ) )  ==NULL)
    {
	progbug( "MpOpenPassage - Bad syntax", ch );
	return;
    }

    if( !is_number(arg3) )
    {
	progbug( "MpOpenPassage - Bad syntax", ch );
	return;
    }

    exit_num = atoi(arg3);
    if( (exit_num < 0) || (exit_num > MAX_DIR) )
    {
	progbug( "MpOpenPassage - Bad syntax", ch );
	return;
    }

    if( (pexit = get_exit( fromRoom, exit_num )) != NULL )
    {
	if( !IS_SET( pexit->flags, EX_PASSAGE) )
	  return;
	progbug( "MpOpenPassage - Exit exists", ch );
	return;
    }

    pexit = make_exit( fromRoom, targetRoom, exit_num );
    STRDUP( pexit->keyword    , "" );
    STRDUP( pexit->description, "" );
    pexit->key     		= -1;
    pexit->flags		= EX_PASSAGE;

    /* act( PLAIN, "A passage opens!", ch, NULL, NULL, TO_CHAR ); */
    /* act( PLAIN, "A passage opens!", ch, NULL, NULL, TO_ROOM ); */

    return;
}

/*
 * Syntax mp_close_passage x y
 *
 * closes a passage in room x leading in direction y
 *
 * the exit must have EX_PASSAGE set
 */
DEF_DO_FUN( mp_close_passage )
{
    char arg1[ MAX_INPUT_LENGTH ];
    char arg2[ MAX_INPUT_LENGTH ];
    char arg3[ MAX_INPUT_LENGTH ];
    ROOM_INDEX_DATA *fromRoom;
    int fromRoomVnum, exit_num;
    EXIT_DATA *pexit;

    MPCOMMAND; //do_mp_close_passage" );

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg2[0] == '\0' )
    {
	progbug( "MpClosePassage - Bad syntax", ch );
	return;
    }

    if( !is_number(arg1) )
    {
	progbug( "MpClosePassage - Bad syntax", ch );
	return;
    }

    fromRoomVnum = atoi(arg1);
    if(  (fromRoom = get_room_index( fromRoomVnum ) )  ==NULL)
    {
	progbug( "MpClosePassage - Bad syntax", ch );
	return;
    }

    if( !is_number(arg2) )
    {
	progbug( "MpClosePassage - Bad syntax", ch );
	return;
    }

    exit_num = atoi(arg2);
    if( (exit_num < 0) || (exit_num > MAX_DIR) )
    {
	progbug( "MpClosePassage - Bad syntax", ch );
	return;
    }

    if( ( pexit = get_exit(fromRoom, exit_num) ) == NULL )
    {
	  return;    /* already closed, ignore...  so rand_progs */
		     /*                            can close without spam */
    }

    if( !IS_SET( pexit->flags, EX_PASSAGE) )
    {
	progbug( "MpClosePassage - Exit not a passage", ch );
	return;
    }

    extract_exit( fromRoom, pexit );

    /* act( PLAIN, "A passage closes!", ch, NULL, NULL, TO_CHAR ); */
    /* act( PLAIN, "A passage closes!", ch, NULL, NULL, TO_ROOM ); */

    return;
}



/*
 * Does nothing.  Used for scripts.
 */
DEF_DO_FUN( mpnothing )
{
    MPCOMMAND; //do_mpnothing" );
    return;
}


/*
 *   Sends a message to sleeping character.  Should be fun
 *    with room sleep_progs
 *
 */
DEF_DO_FUN( mpdream )
{
    char arg1[MAX_STRING_LENGTH];
    CHAR_DATA *vict;

    MPCOMMAND; //do_mpdream" );

    argument = one_argument( argument, arg1 );

    if (  (vict =get_char_world(ch, arg1)) == NULL )
    {
        progbug( "Mpdream: No such character", ch );
        return;
    }

    if( vict->position <= POS_SLEEPING)
    {
      send_to_char(argument, vict);
      send_to_char("" NL,   vict);
    }
    return;
}

DEF_DO_FUN( mpapply )
{
    CHAR_DATA *victim;

    MPCOMMAND; //do_mpapply" );

  if (argument[0] == '\0')
  {
    progbug("Mpapply - bad syntax", ch );
    return;
  }

  if ( (victim = get_char_room_mp( ch, argument ) ) == NULL )
  {
    progbug("Mpapply - no such player in room.", ch );
    return;
  }

  if ( !victim->desc )
  {
   send_to_char( "Not on linkdeads." NL, ch );
   return;
  }

  if( !NOT_AUTHED(victim) )
    return;

  if( victim->pcdata->auth_state >= 1 )
    return;

  sprintf( log_buf, "%s@%s new %s applying for authorization...",
           victim->name, victim->desc->host,
           victim->race->name);
  log_string( log_buf );
  to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
  victim->pcdata->auth_state = 1;
  return;
}

DEF_DO_FUN( mpapplyb )
{
    CHAR_DATA *victim;

    MPCOMMAND; //do_mpapplyb" );

  if (argument[0] == '\0')
  {
    progbug("Mpapplyb - bad syntax", ch );
    return;
  }

  if ( (victim = get_char_room_mp( ch, argument ) ) == NULL )
  {
    progbug("Mpapplyb - no such player in room.", ch );
    return;
  }

  if ( !victim->desc )
  {
   send_to_char( "Not on linkdeads." NL, ch );
   return;
  }

  if( !NOT_AUTHED(victim) )
    return;

  if ( get_timer(victim, TIMER_APPLIED) >= 1)
    return;

  switch( victim->pcdata->auth_state )
  {
  case 0:
  case 1:
  default:
  send_to_char( "Zwracasz na siebie uwage Bog�w." NL, victim);
  sprintf( log_buf, "%s@%s new %s applying for authorization...",
                    victim->name, victim->desc->host,
                    victim->race->name);
  log_string( log_buf );
  to_channel( log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
  add_timer(victim, TIMER_APPLIED, 10, NULL, 0);
  victim->pcdata->auth_state = 1;
  break;

  case 2:
  send_to_char("Twoje imi� zosta�o przekl�te przez Bog�w. Popro� ich, �eby ci je zmienili." NL, victim);
  add_timer(victim, TIMER_APPLIED, 10, NULL, 0);
  break;

  case 3:
  send_to_char( "Bogowie zabraniaj� ci wej�cia do tego �wiata." NL, victim);
        REMOVE_BIT(victim->pcdata->flags, PCFLAG_UNAUTHED);
        if ( victim->fighting )
          stop_fighting( victim, true );
        char_from_room(victim);
        char_to_room(victim, get_room_index(ROOM_VNUM_SCHOOL));
        act( FB_WHITE, "$n wkracza do tego �wiata w s�upie o�lepiaj�cego �wiat�a!",
            victim, NULL, NULL, TO_ROOM );
        do_look(victim, (char *)"auto");
  break;
  }

  return;
}

/*
 * Deposit some gold into the current area's economy		-Thoric
 */
DEF_DO_FUN( mp_deposit )
{
    char arg[MAX_STRING_LENGTH];
    int gold;

    MPCOMMAND; //do_mpdeposit" );

    one_argument(argument, arg);

    if ( arg[0] == '\0' )
    {
	progbug("Mpdeposit - bad syntax", ch );
	return;
    }
    gold = atoi( arg );
    if ( gold <= ch->gold && ch->in_room )
    {
	ch->gold -= gold;
	boost_economy( ch->in_room->area, gold );
    }
}


/*
 * Withdraw some gold from the current area's economy		-Thoric
 */
DEF_DO_FUN( mp_withdraw )
{
    char arg[MAX_STRING_LENGTH];
    int gold;

    MPCOMMAND; //do_mpwithdraw" );

    one_argument(argument, arg);

    if ( arg[0] == '\0' )
    {
	progbug("Mpwithdraw - bad syntax", ch );
	return;
    }
    gold = atoi( arg );
    if ( ch->gold < 1000000000 && gold < 1000000000 && ch->in_room
    &&   economy_has( ch->in_room->area, gold ) )
    {
	ch->gold += gold;
	lower_economy( ch->in_room->area, gold );
    }
}

/* Aby na mudzie sie cos dzialo, dodalem taka mpkomende - Trog */
DEF_DO_FUN( mpaddfrc )
{
	CD	*victim;
    char arg1[MIL];
    char arg2[MIL];
	char buf[MIL];
    int frc;

    MPCOMMAND;

    argument = one_argument(argument, arg1);
    one_argument(argument, arg2);

	if( !(victim = get_char_room_mp( ch, arg1 )) )
    {
		send_to_char( "Victim must be in room." NL, ch );
		progbug( "Mpaddfrc: victim not in room", ch );
		return;
    }

	if( IS_NPC( victim ) )
		return;

	if( (frc = abs(atoi(arg2))) < 1 || frc > 5 )
		return;

	victim->perm_frc += frc;
	victim->perm_frc = URANGE( 0, victim->perm_frc, 20 );

	sprintf( buf, "Mpaddfrc: giving +%d frc to %s", frc, victim->name );
	log_string( buf );
	to_channel( buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL );
}

/* Trog: mpcommand for authorizing players through mudprogs  */
DEF_DO_FUN( mpauth )
{
	CD	*victim;
	char buf[MIL];

    MPCOMMAND;

	if( !(victim = get_char_room_mp( ch, argument )) )
    {
		progbug( "Mpauth: victim not in room", ch );
		return;
    }

	if( IS_NPC(victim) || !NOT_AUTHED(victim) )
		return;

	victim->pcdata->auth_state = 3;
	REMOVE_BIT(victim->pcdata->flags, PCFLAG_UNAUTHED);
	STRDUP( victim->pcdata->authed_by, ch->przypadki[0] );
	sprintf( buf, "%s authorized %s", ch->przypadki[0], victim->name );
	to_channel( buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
}

/*
 * Inflict damage from a mudprogram
 *
 *  note: should be careful about using victim afterwards
 */
ch_ret simple_damage(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt)
{
	int dameq;
	bool npcvict;
	OBJ_DATA *damobj;

	if (!ch)
	{
		bug( "Damage: null ch!", 0 );
		return rERROR;
	}
	if (!victim)
	{
		progbug("Damage: null victim!", ch);
		return rVICT_DIED;
	}

	if (victim->position == POS_DEAD)
	{
		return rVICT_DIED;
	}

	npcvict = IS_NPC(victim);

	if (dam)
	{
		if (IS_FIRE(dt))
			dam = ris_damage(victim, dam, RIS_FIRE);
		else if (IS_COLD(dt))
			dam = ris_damage(victim, dam, RIS_COLD);
		else if (IS_ACID(dt))
			dam = ris_damage(victim, dam, RIS_ACID);
		else if (IS_ELECTRICITY(dt))
			dam = ris_damage(victim, dam, RIS_ELECTRICITY);
		else if (IS_ENERGY(dt))
			dam = ris_damage(victim, dam, RIS_ENERGY);
		else if (dt == gsn_poison)
			dam = ris_damage(victim, dam, RIS_POISON);
		else if (dt == (TYPE_HIT + 7) || dt == (TYPE_HIT + 8))
			dam = ris_damage(victim, dam, RIS_BLUNT);
		else if (dt == (TYPE_HIT + 2) || dt == (TYPE_HIT + 11))
			dam = ris_damage(victim, dam, RIS_PIERCE);
		else if (dt == (TYPE_HIT + 1) || dt == (TYPE_HIT + 3))
			dam = ris_damage(victim, dam, RIS_SLASH);
		if (dam < 0)
			dam = 0;
	}

	if (victim != ch)
	{
		/*
		 * Damage modifiers.
		 */
		if (IS_AFFECTED(victim, AFF_SANCTUARY))
			dam /= 2;

		if (IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch))
			dam -= (int) (dam / 4);

		if (dam < 0)
			dam = 0;

		/* dam_message( ch, victim, dam, dt ); */
	}

	/*
	 * Check for EQ damage.... ;)
	 */

	if (dam > 10)
	{
		/* get a random body eq part */
		dameq = number_range(WEAR_LIGHT, WEAR_EYES);
		damobj = get_eq_char(victim, dameq);
		if (damobj)
		{
			if (dam > get_obj_resistance(damobj))
			{
				set_cur_obj(damobj);
				damage_obj(damobj);
			}
		}
	}

	/*
	 * Hurt the victim.
	 * Inform the victim of his new state.
	 */
	victim->hit -= dam;
	if (!IS_NPC(victim) && get_trust(victim) >= LEVEL_IMMORTAL && victim->hit < 1)
		victim->hit = 1;

	if (!npcvict && get_trust(victim) >= LEVEL_IMMORTAL && get_trust(ch) >= LEVEL_IMMORTAL && victim->hit < 1)
		victim->hit = 1;
	update_pos(victim);

	switch (victim->position)
	{
	case POS_MORTAL:
		act(COL_DYING, "$n jest �miertelnie rann$e i szbko umrze bez pomocy.", victim, NULL, NULL, TO_ROOM);
		act(COL_DANGER, "Jaste� �miertelnie rann$e i bez pomocy szybko umrzesz.", victim, NULL, NULL, TO_CHAR);
		break;

	case POS_INCAP:
		act(COL_DYING, "$n jest og�uszon$e, mo�e umrze� bez niczyjej pomocy.", victim, NULL, NULL, TO_ROOM);
		act(COL_DANGER, "Jeste� og�uszon$e i mo�esz umrze� je�li nikt ci nie pomo�e.", victim, NULL, NULL, TO_CHAR);
		break;

	case POS_STUNNED:
		if (!IS_AFFECTED( victim, AFF_PARALYSIS ))
		{
			act(COL_ACTION, "$n jest og�uszon$e, ale prawdopodobnie z tego wyjdzie.", victim, NULL, NULL, TO_ROOM);
			act(COL_HURT, "Jeste� og�uszon$e ale prawdopodobnie z tego wyjdziesz.", victim, NULL, NULL, TO_CHAR);
		}
		break;

	case POS_DEAD:
		act(COL_DEAD, "$n UMIERA !!!", victim, 0, 0, TO_ROOM);
		act(COL_DEAD, "UMIERASZ !!!" NL, victim, 0, 0, TO_CHAR);
		break;

	default:
		if (dam > victim->max_hit / 4)
			act(COL_HURT, "Ale� oberwa�$o! To musia�o BOLE� !!!", victim, 0, 0, TO_CHAR);
		if (victim->hit < victim->max_hit / 4)
			act(COL_DANGER, "Chcia�$oby�, by twoje rany przesta�y tak KRWAWI� !!!", victim, 0, 0, TO_CHAR);
		break;
	}

	/*
	 * Payoff for killing things.
	 */
	if (victim->position == POS_DEAD)
	{
		if (!npcvict)
		{
			sprintf(log_buf, "%s killed by %s at %d", victim->name, (IS_NPC(ch) ? ch->przypadki[0] : ch->name),
					victim->in_room->vnum);
			log_string( log_buf );
			to_channel(log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL);

		}
		set_cur_char(victim);
		raw_kill(ch, victim, 0);
		victim = NULL;

		return rVICT_DIED;
	}

	if (victim == ch)
		return rNONE;

	/*
	 * Take care of link dead people.
	 * Trog: kuzwa, ale do_recall jest dla olcmanow.
	 */
	/*
	 if ( !npcvict && !victim->desc )
	 {
	 if ( number_range( 0, victim->wait ) == 0 )
	 {
	 do_recall( victim, "" );
	 return rNONE;
	 }
	 }
	 */
	/*
	 * Wimp out?
	 */
	if (npcvict && dam > 0)
	{
		if ((IS_SET(victim->act, ACT_WIMPY) && number_bits(1) == 0 && victim->hit < victim->max_hit / 2)
				|| (IS_AFFECTED(victim, AFF_CHARM) && victim->master && victim->master->in_room != victim->in_room))
		{
			start_fearing(victim, ch);
			stop_hunting(victim);
			do_flee(victim, (char *) "");
		}
	}

	if (!npcvict && victim->hit > 0 && victim->hit <= victim->wimpy && victim->wait == 0)
		do_flee(victim, (char *) "");
	else if (!npcvict && IS_SET( victim->act, PLR_FLEE ))
		do_flee(victim, (char *) "");

	tail_chain();
	return rNONE;
}

CHAR_DATA *get_char_room_mp( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int number, count, vnum;

    number = number_argument( argument, arg );
    if ( !str_cmp( arg, "self" ) )
	return ch;

    if ( get_trust(ch) >= LEVEL_SAVIOR && is_number( arg ) )
	vnum = atoi( arg );
    else
	vnum = -1;

    count  = 0;

    for ( auto* rch : ch->in_room->people )
	if ( (nifty_is_name( arg, rch->name )
	||  (IS_NPC(rch) && vnum == rch->pIndexData->vnum)) )
	{
	    if ( number == 0 && !IS_NPC(rch) )
		return rch;
	    else
	    if ( ++count == number )
		return rch;
	}

    if ( vnum != -1 )
	return NULL;
    count  = 0;
    for ( auto* rch : ch->in_room->people )
    {
	if ( !nifty_is_name_prefix( arg, rch->name ) )
	    continue;
	if ( number == 0 && !IS_NPC(rch) )
	    return rch;
	else
	if ( ++count == number )
	    return rch;
    }

    return NULL;
}
