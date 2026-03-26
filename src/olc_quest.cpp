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
*       	    OLC-4-QUESTS Module Added by Thanos			   *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"

extern int	top_quest_index;

/* olc.c */
AREA_DATA *	get_vnum_area		args( ( int vnum ) );


bool quest_create( CHAR_DATA *ch, char *argument )
{
	QUEST_INDEX_DATA *	pQuest;
	AREA_DATA *		pArea;
	int               	value;

	value = atoi( argument );

	if ( argument[0] == '\0'  || value == 0 )
	{
		send_to_char( "Syntax: create <quest_id>" NL, ch );
		return false;
	}

	pArea = get_vnum_area( value );

	if ( !pArea )
	{
		send_to_char( "This number depends to no area." NL, ch );
		return false;
	}

	for( auto* pq : quest_index_list )
		if ( pq->quest_id == value )
	{
		send_to_char( "There is a quest with that ID." NL, ch );
		return false;
	}

	CREATE( pQuest, QUEST_INDEX_DATA, 1 );
	STRDUP( pQuest->name,	 "Nowy Quest" );
	STRDUP( pQuest->author,	 ch->name );
	STRDUP( pQuest->description, "" );
	pQuest->questor 		= MOB_VNUM_QUEST_MASTER;
	pQuest->quest_id 		= value;
	pArea->quest 		= pQuest;
	quest_index_list.push_back( pQuest );
	top_quest_index++;

	ch->desc->olc_editing   	= (void *)pQuest;
	send_to_char( " Quest created." NL, ch );

	ch->pcdata->line_nr 	= 0;
	ch->desc->connected 	= CON_QEDITOR;
	ch->desc->olc_editing   	= (void *)pQuest;

	return true;
}


DEF_DO_FUN( qedit )
{
	QUEST_INDEX_DATA *	pQuest;
	int              	value;
	char             	arg1 [MAX_STRING_LENGTH];

	if( !*argument )
	{
		send_to_char( "Syntax:  qedit <vnum>" NL
				"         qedit create <vnum>" NL, ch );
		return;
	}

	argument = one_argument( argument, arg1 );

	if ( is_number( arg1 ) )
	{
		value = atoi( arg1 );
		pQuest = nullptr;
		for( auto* pq : quest_index_list )
			if ( pq->quest_id == value )
		{
			pQuest = pq;
			send_to_char( " Editing quest." NL, ch );
			break;
		}
		if( !pQuest )
		{
			send_to_char( "No quest with that ID." NL, ch );
			return;
		}
	}
	else if ( !str_cmp( arg1, "create" ) )
	{
		if ( quest_create( ch, argument ) )
			send_to_char( "Editing quest." NL, ch );
		return;
	}
	else
	{
		send_to_char( "Syntax: qedit [create] <vnum>" NL, ch );
		return;
	}

	ch->pcdata->line_nr 	= 0;
	ch->desc->connected 	= CON_QEDITOR;
	ch->desc->olc_editing   	= (void *)pQuest;
	return;
}

CHAPTER_INDEX_DATA *get_chapter_index( QUEST_INDEX_DATA *quest, int nr )
{
	int				i 	= 1;

	if( nr <= 0 )
		return NULL;

	if( !quest )
		return NULL;

	for( auto* chptr : quest->chapters )
	{
		if( i == nr )
			return chptr;
		i++;
	}

	return NULL;
}

/* do do_qstat */
void show_chapter( CHAR_DATA *ch, QUEST_INDEX_DATA *quest, int nr )
{
	CHAPTER_INDEX_DATA *	pChapter;
	QUEST_CMND_DATA *		pCmnd;
	QUEST_ACTION_DATA *		pAction;
	int				i;

	if( (i = nr) == 0 )
		return;

	pChapter = get_chapter_index( quest, nr );

	if( !pChapter )
	{
		send_to_char( "No such Chapter" NL, ch );
		return;
	}

	pager_printf( ch,
				  FB_CYAN "[" PLAIN "%d" FB_CYAN "] Chapter '%s'   Type: %s" EOL,
				  i, pChapter->name, bit_name( quest_chapter_types, pChapter->type ) );

	if( !pChapter->init_cmds.empty() )
		for( auto* pCmnd : pChapter->init_cmds )
			pager_printf( ch, FB_WHITE
					FG_CYAN " Init   %10s" PLAIN " %6d %6d %6d %s" EOL,
					bit_name(quest_init_types, pCmnd->command),
					pCmnd->arg1, 	pCmnd->arg2,
					pCmnd->arg3, 	pCmnd->arg4 );
	else
		send_to_pager( " No init Cmnds" NL, ch );

	if( !pChapter->events.empty() )
		for( auto* pCmnd : pChapter->events )
			pager_printf( ch, FB_WHITE
					FG_CYAN " Event  %10s" PLAIN " %6d %6d %6d %s" EOL,
					bit_name(quest_event_types, pCmnd->command),
					pCmnd->arg1, 	pCmnd->arg2,
					pCmnd->arg3, 	pCmnd->arg4 );
	else
		send_to_pager( " No Events" NL, ch );

	if( !pChapter->actions.empty() )
		for( auto* pAction : pChapter->actions )
			pager_printf( ch, FB_WHITE
					FG_CYAN " Action %10s" PLAIN " %6d %6d %6d %s" EOL,
					bit_name(quest_action_types, pAction->command),
					pAction->arg1, 	pAction->arg2,
					pAction->arg3, 	pAction->arg3 );
	else
		send_to_pager( " No Actions" NL, ch );

	return;
}


#define show_args( ch, type, pCmnd )								\
{																	\
    bool found = false;												\
																	\
    if( show_q_arg( type, pCmnd->command, 1 ) != NULL )				\
{																\
        pager_printf( ch, 											\
	       "        " FG_YELLOW "%10s:" PLAIN " %-6d   ",				\
		show_q_arg( type, pCmnd->command, 1 ),    pCmnd->arg1 );	\
		found = true;												\
}																\
    else															\
		pager_printf( ch, "                       " );				\
																	\
    if( show_q_arg( type, pCmnd->command, 2 ) != NULL )				\
{																\
        pager_printf( ch, 											\
	       FG_YELLOW "%10s:" PLAIN " %-6d   ",							\
		show_q_arg( type, pCmnd->command, 2 ),    pCmnd->arg2 );	\
		found = true;												\
}																\
    else															\
		pager_printf( ch, "                     " );				\
																	\
    if( show_q_arg( type, pCmnd->command, 3 ) != NULL )			\
        pager_printf( ch, 						\
	       FG_YELLOW "%10s:" PLAIN " %-6d" 	EOL,		\
    	show_q_arg( type, pCmnd->command, 3 ),    pCmnd->arg3 );	\
    else								\
{									\
	if( found ) 							\
    	    pager_printf( ch, EOL );					\
	else								\
    	    pager_printf( ch, "\r");					\
}									\
									\
    if( show_q_arg( type, pCmnd->command, 4 ) != NULL )			\
		pager_printf( ch, 						\
	       "        " FG_YELLOW "%10s:" PLAIN " %s" 	EOL,	\
	show_q_arg( type, pCmnd->command, 4 ),    pCmnd->arg4 );	\
}


void qprepare_comment( QUEST_ACTION_DATA *pCmnd, int type )
{
	char	buf		[MSL];

	switch( pCmnd->command )
	{
		default:
			return;
		case ACTION_TELL:
		case ACTION_PROG:
		case ACTION_STOPQUEST:
		case ACTION_NONE:
		case ACTION_RUNCHPT:
			return;
		case ACTION_REWARD:
		{
			OBJ_INDEX_DATA *	pObj;
			MOB_INDEX_DATA *	pMob;

			pObj = get_obj_index( pCmnd->arg1 );
			pMob = get_mob_index( pCmnd->arg2 );

			sprintf( buf, "# %s -> %s",
					 pMob ? pMob->przypadki[0] : "???",
					 pObj ? pObj->przypadki[0] : "???" );
		}
		break;
	}

	STRDUP( pCmnd->arg4, buf );
}

void qprepare_comment( QUEST_CMND_DATA *pCmnd, int type )
{
	char	buf		[MSL];

	if( !pCmnd )
		return;

	// nie nadpisujemy komentarzy buildera
	if( *pCmnd->arg4 && pCmnd->arg4[0] != '#' )
		return;

	if( type == CMD_INIT )
	{
		switch( pCmnd->command )
		{
			default:
				return;

			case INIT_PURGE:
			case INIT_TRANSFER:
			case INIT_ECHO:
			case INIT_HINT:
				return;
                        case INIT_GIVE_OBJ:
                        {
                            OBJ_INDEX_DATA *	pObj;
                            pObj = get_obj_index( pCmnd->arg1 );

                            sprintf( buf, "# %s",
                                             pObj ? pObj->przypadki[0] : "???");
                            return;
                        }

			case INIT_LOAD_MOB:
			{
				MOB_INDEX_DATA *	pMob;
				ROOM_INDEX_DATA *	pRoom;
				ROOM_INDEX_DATA *	pRoom2;

				pMob = get_mob_index( pCmnd->arg1 );
				pRoom = get_room_index( pCmnd->arg2 );
				pRoom2 = get_room_index( pCmnd->arg3 );

				if( pRoom == pRoom2 )
				{
					sprintf( buf, "# %s -> %s",
							 pMob ? pMob->przypadki[0] : "???",
							 pRoom ? pRoom->name : "???" );
				}
				else
					sprintf( buf, "# %s -> ( %s :: %s )",
							 pMob ? pMob->przypadki[0] : "???",
							 pRoom ? pRoom->name : "???",
							 pRoom2 ? pRoom2->name : "???" );

			}
			break;
			case INIT_LOAD_OBJ:
			{
				OBJ_INDEX_DATA *	pObj;
				ROOM_INDEX_DATA *	pRoom;

				pObj = get_obj_index( pCmnd->arg1 );
				pRoom = get_room_index( pCmnd->arg3 );

				sprintf( buf, "# %s -> %s",
						 pObj ? pObj->przypadki[0] : "???",
						 pRoom ? pRoom->name : "???" );
			}
			break;
			case INIT_PUT_OBJ:
			{
				OBJ_INDEX_DATA *	pObj;
				OBJ_INDEX_DATA *	pToObj;

				pObj = get_obj_index( pCmnd->arg1 );
				pToObj = get_obj_index( pCmnd->arg3 );

				sprintf( buf, "# %s -> %s",
						 pObj ? pObj->przypadki[0] : "???",
						 pToObj ? pToObj->przypadki[0] : "???" );
			}
			break;
			case INIT_EQUIP_MOB:
			{
				OBJ_INDEX_DATA *	pObj;

				pObj = get_obj_index( pCmnd->arg1 );

				sprintf( buf, "# %s -> last_mob (%s)",
						 pObj ? pObj->przypadki[0] : "???",
						 bit_name( wear_types_list, pCmnd->arg3 ) );

			}
			break;

			case INIT_INVITE_MOB:
			{
				MOB_INDEX_DATA *	pMob;

				pMob = get_mob_index( pCmnd->arg1 );

				sprintf( buf, "# -> %s",
						 pMob ? pMob->przypadki[0] : "???" );
			}
			break;
		}
	}
	else if( type == CMD_EVENT )
	{
		switch( pCmnd->command )
		{
			default:
				return;
			case EVENT_VISIT_MOB:
			case EVENT_KILL_MOB:
			{
				MOB_INDEX_DATA *	pMob;

				pMob = get_mob_index( pCmnd->arg1 );

				sprintf( buf, "# %s",
						 pMob ? pMob->przypadki[0] : "???" );
			}
			break;
			case EVENT_FIND_OBJ:
			{
				OBJ_INDEX_DATA *	pObj;

				pObj = get_obj_index( pCmnd->arg1 );

				sprintf( buf, "# %s",
						 pObj ? pObj->przypadki[0] : "???" );
			}
			break;
			case EVENT_BRING_OBJ:
			case EVENT_GIVE_OBJ:
			{
				OBJ_INDEX_DATA *	pObj;
				MOB_INDEX_DATA *	pMob;

				pObj = get_obj_index( pCmnd->arg1 );
				pMob = get_mob_index( pCmnd->arg2 );

				sprintf( buf, "# %s -> %s",
						 pObj ? pObj->przypadki[0] : "???",
						 pMob ? pMob->przypadki[0] : "???" );
			}
			break;
			case EVENT_VISIT_ROOM:
			{
				ROOM_INDEX_DATA *	pRoom;

				pRoom = get_room_index( pCmnd->arg1 );

				sprintf( buf, "# %s",
						 pRoom ? pRoom->name : "???" );
			}
			break;
			case EVENT_LOOK_OBJ:
			{
				OBJ_INDEX_DATA *	pObj;
				ROOM_INDEX_DATA *	pRoom;

				pObj = get_obj_index( pCmnd->arg1 );
				pRoom = get_room_index( pCmnd->arg2 );

				sprintf( buf, "# %s %s %s",
						pObj ? pObj->przypadki[0] : "???", "in",
						pRoom ? pRoom->name : "???" );
			}
			break;
			case EVENT_LOOK_MOB:
			{
				MOB_INDEX_DATA *	pMob;
				ROOM_INDEX_DATA *	pRoom;

				pMob = get_mob_index( pCmnd->arg1 );
				pRoom = get_room_index( pCmnd->arg2 );

				sprintf( buf, "# %s %s %s",
						 pMob ? pMob->przypadki[0] : "???", "in",
						 pRoom ? pRoom->name : "???" );
			}
			break;
		}
	}
	else if( type == CMD_ACTION )
	{
		switch( pCmnd->command )
		{
			default:
				return;
			case ACTION_TELL:
			case ACTION_PROG:
			case ACTION_STOPQUEST:
			case ACTION_NONE:
			case ACTION_RUNCHPT:
				return;
			case ACTION_REWARD:
			{
				OBJ_INDEX_DATA *	pObj;
				MOB_INDEX_DATA *	pMob;

				pObj = get_obj_index( pCmnd->arg1 );
				pMob = get_mob_index( pCmnd->arg2 );

				sprintf( buf, "# %s -> %s",
						 pMob ? pMob->przypadki[0] : "???",
						 pObj ? pObj->przypadki[0] : "???" );
			}
			break;
		}
	}
	else
		return;

	STRDUP( pCmnd->arg4, buf );
}


/* do do_qedit */
void adv_show_chapter( CHAR_DATA *ch, QUEST_INDEX_DATA *quest, int nr )
{
	CHAPTER_INDEX_DATA *	pChapter;
	QUEST_CMND_DATA *		pCmnd;
	QUEST_ACTION_DATA *		pAction;
	int				i;

	if( (i = nr) == 0 )
		return;

	pChapter = get_chapter_index( quest, nr );

	if( !pChapter )
	{
		send_to_char( "No such Chapter" NL, ch );
		return;
	}

	pager_printf( ch,
				  FB_CYAN "[" PLAIN "%d" FB_CYAN "] Chapter:        Name: '%s'   Type: %s" EOL
						  "--------------------------------------------------------" NL,
				  i, pChapter->name, bit_name( quest_chapter_types, pChapter->type ) );

	if( !pChapter->init_cmds.empty() )
	{
		i = 1;
		for( auto* pCmnd : pChapter->init_cmds )
		{
			pager_printf( ch, FG_CYAN
				" Init   " FB_WHITE "%d" FG_CYAN "      Cmd: " FB_CYAN "%s" EOL,
				i++, bit_name(quest_init_types, pCmnd->command) );

			show_args( ch, CMD_INIT, pCmnd );
		}
	}
	else
		send_to_pager( " No init Cmnds" NL, ch );

	if( !pChapter->events.empty() )
	{
		i = 1;
		for( auto* pCmnd : pChapter->events )
		{
			pager_printf( ch, FG_CYAN
				" Event  " FB_WHITE "%d" FG_CYAN "      Cmd: " FB_CYAN "%s" EOL,
				i++, bit_name(quest_event_types, pCmnd->command) );

			show_args( ch, CMD_EVENT, pCmnd );
		}
	}
	else
		send_to_pager( " No Events" NL, ch );

	if( !pChapter->actions.empty() )
	{
		i = 1;
		for( auto* pAction : pChapter->actions )
		{
			pager_printf( ch, FG_CYAN
				" Action " FB_WHITE "%d" FG_CYAN "      Cmd: " FB_CYAN "%s" EOL,
				i++, bit_name(quest_action_types, pAction->command) );

			show_args( ch, CMD_ACTION, pAction );
		}
	}
	else
		send_to_pager( " No Actions" NL, ch );

	return;
}

DEF_DO_FUN( qstat )
{
	QUEST_CMND_DATA *	pCmnd;
	QUEST_ACTION_DATA *	pAction;
	QUEST_INDEX_DATA *	pQuest;
	CHAPTER_INDEX_DATA *pChapter;
	char            	arg  [ MAX_INPUT_LENGTH  ];
	int			value, i;

	argument = one_argument( argument, arg );
	if( arg[0] == '\0' )
		pQuest = ch->in_room->area->quest;
	else
	{
		value = atoi( arg );
		pQuest = nullptr;
		for( auto* pq : quest_index_list )
			if( pq->quest_id == value )
			{
				pQuest = pq;
				break;
			}
	}

	if ( !pQuest )
	{
		send_to_char( "Syntax: qstat <quest id> [silent]. Type 'quest list' for list." NL, ch );
		return;
	}

	if( pQuest->security > ch->pcdata->security )
	{
		send_to_char( "Nieodpowiedni poziom security." NL, ch );
		return;
	}

	pager_printf( ch,
				  FG_CYAN "Name:    '" PLAIN "%s" FG_CYAN "'" EOL
						  FG_CYAN "Author:   " PLAIN "%s" EOL
						  FG_CYAN "Security: " PLAIN "%d" EOL,
				  pQuest->name, pQuest->author, pQuest->security );

	pager_printf( ch,
				  FG_CYAN "Quest_id: "   PLAIN "%d"
						  FG_CYAN ",    Minlev: " PLAIN "%d"
						  FG_CYAN ",    Maxlev: " PLAIN "%d" NL,
				  pQuest->quest_id, pQuest->min_lev, pQuest->max_lev );

	pager_printf( ch,
				  FG_CYAN "Time:     " PLAIN "%d" FG_CYAN " %s "
						  FG_CYAN "Penalty:  " PLAIN "%d" FG_CYAN "." EOL,
				  pQuest->time, pQuest->time == -1 ? "(infinite)" : "RL-hour(s)",
				  pQuest->penalty );

	pager_printf( ch,
				  FG_CYAN "Flags:    [" PLAIN "%s" FG_CYAN "]." EOL,
				  flag_string( quest_flags_list, pQuest->flags ) );

	pager_printf( ch,
				  FG_CYAN "Questor:  " PLAIN "%d " FG_CYAN "(%s)." EOL,
				  pQuest->questor, get_mob_index( pQuest->questor ) ?
						  get_mob_index( pQuest->questor )->player_name : "none" );

	pager_printf( ch,
				  FG_CYAN "Desc:" EOL "%s" EOL, pQuest->description );


	pager_printf( ch, FB_CYAN "Quest commands:" EOL );
	if( ch->desc->olc_editing
		   &&  ch->desc->olc_editing == pQuest
		   &&  ch->pcdata->line_nr   == 0 )
	{

		if( !pQuest->init_cmds.empty() )
		{
			i = 1;
			for( auto* pCmnd : pQuest->init_cmds )
			{
				pager_printf( ch, FG_CYAN
					" Init   " FB_WHITE "%d" FG_CYAN "      Cmd: " FB_CYAN "%s" EOL,
					i++, bit_name(quest_init_types, pCmnd->command) );

				show_args( ch, CMD_INIT, pCmnd );
			}
		}
		else
			send_to_pager( " No init Cmnds" NL, ch );

		if( !pQuest->actions.empty() )
		{
			i = 1;
			for( auto* pAction : pQuest->actions )
			{
				pager_printf( ch, FG_CYAN
					" Action " FB_WHITE "%d" FG_CYAN "      Cmd: " FB_CYAN "%s" EOL,
					i++, bit_name(quest_action_types, pAction->command) );

				show_args( ch, CMD_ACTION, pAction );
			}
		}
		else
			send_to_pager( " No Actions" NL, ch );
	}
	else
	{
		for( auto* pCmnd : pQuest->init_cmds )
			pager_printf( ch, FB_WHITE
					FG_CYAN " Init   %10s" PLAIN " %6d %6d %6d %s" EOL,
					bit_name(quest_init_types, pCmnd->command),
					pCmnd->arg1, 	pCmnd->arg2,
					pCmnd->arg3, 	pCmnd->arg4 );

		for( auto* pAction : pQuest->actions )
			pager_printf( ch, FB_WHITE
					FG_CYAN " Action %10s" PLAIN " %6d %6d %6d %s" EOL,
					bit_name(quest_action_types, pAction->command),
					pAction->arg1, 	pAction->arg2,
					pAction->arg3, 	pAction->arg4 );

	}
	if( argument[0] != '\0' )
	{
		if( str_cmp( argument, "silent" ) )
		{
			send_to_char( "Syntax: qstat <quest id> [silent]." NL, ch );
			return;
		}
		return;
	}

	i=1;
	for( auto* pChapter : pQuest->chapters )
		show_chapter( ch, pQuest, i++ );
	return;
}

void qedit( DESCRIPTOR_DATA *d, char *argument )
{
	CHAR_DATA *		ch = d->character;
	AREA_DATA        *	pArea;
	QUEST_INDEX_DATA *	pQuest;
	char              	arg	[ MAX_STRING_LENGTH ];
	char              	arg1	[ MAX_STRING_LENGTH ];
	char              	arg2	[ MAX_STRING_LENGTH ];
	char              	buf	[ MAX_STRING_LENGTH ];
	int               	value;

	strcpy( arg, argument );
	smash_tilde( argument );
	argument = one_argument( argument, arg1 );


	if( !ch->desc->olc_editing )
	{
		pArea = ch->in_room->area;
		pQuest = pArea->quest;

		if( !pQuest )
		{
			send_to_char( "There is no quests in this area. Use 'create'." NL, ch );
			edit_done( ch, (char *)"" );
			return;
		}

		ch->desc->olc_editing = (void *)pQuest;
	}
	else
		pQuest = (QUEST_INDEX_DATA *)ch->desc->olc_editing;

	if( pQuest->security > ch->pcdata->security )
	{
		send_to_char( "Nieodpowiedni poziom security." NL, ch );
		edit_done( ch, (char *)"" );
		return;
	}

	if ( !str_prefix( arg1, "show" ) )
	{
		if ( argument[0] != '\0' )
		{
			if( !str_cmp( argument, "all" ) )
			{
				sprintf( buf, "%d", pQuest->quest_id );
				do_qstat( ch, buf );
			}
			else
				send_to_char( "Syntax: show [all]" NL, ch );
			return;
		}

		if( ch->pcdata->line_nr == 0 )
		{
			sprintf( buf, "%d silent", pQuest->quest_id );
			do_qstat( ch, buf );
			return;
		}
		else
			adv_show_chapter( ch, pQuest, ch->pcdata->line_nr );

		return;
	}

	if ( !str_prefix( arg1, "done" ) )
	{
		edit_done( ch, (char *)"" );
		return;
	}

	if ( !str_prefix( arg1, "security" ) )
	{
		if ( !is_number( argument ) || argument[0] == '\0' )
		{
			send_to_char( "Sk�adnia:  security <poziom>" NL, ch );
			return;
		}

		value = atoi( argument );

		if ( value > ch->pcdata->security || value < 0 )
		{
			if ( ch->pcdata->security != 0 )
			{
				sprintf( buf, "W�a�ciwe security:  0-%d." NL, ch->pcdata->security );
				send_to_char( buf, ch );
			}
			else
				send_to_char( "Jedyne w�a�ciwe security to 0." NL, ch );
			return;
		}

		pQuest->security = value;

		send_to_char( "Security ustalone." NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "folda" ) )
	{
		save_quest( pQuest );
		send_to_char( "Ok." NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "?" ) )
	{
		show_help( ch, argument );
		return;
	}

	if ( !str_prefix( arg1, "name" ) )
	{
		if ( argument[0] == '\0' )
		{
			send_to_char( "Syntax: name <name>" NL, ch );
			return;
		}

		if( ch->pcdata->line_nr == 0 )
		{
			STRDUP( pQuest->name, argument );
			send_to_char( "Quest Name set." NL, ch );
		}
		else
		{
			CHAPTER_INDEX_DATA *chapt;

			if( (chapt=get_chapter_index( pQuest, ch->pcdata->line_nr)) == NULL )
			{
				send_to_char( "No such chapter" NL, ch );
				return;
			}

			argument 	= one_argument( argument, arg2 );
			STRDUP( chapt->name, arg2 );
			send_to_char( "Chapter Name set." NL, ch );
		}
		return;
	}

	if ( !str_prefix( arg1, "type" ) )
	{
		CHAPTER_INDEX_DATA *chapt;

		if( (chapt=get_chapter_index( pQuest, ch->pcdata->line_nr)) == NULL )
		{
			send_to_char( "You are not editing a valid chapter. Use 'chapter' cmd" NL, ch );
			return;
		}

		if ( ( value = flag_value( quest_chapter_types, argument ) ) < 0 )
		{
			send_to_char( "That type of chapter is not known." NL, ch );
			return;
		}
		chapt->type = value;
		send_to_char( "Chapter type set." NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "author" ) )
	{
		if ( argument[0] == '\0' )
		{
			send_to_char( "Syntax: author <name>" NL, ch );
			return;
		}
		STRDUP( pQuest->author, argument );
		send_to_char( "Author set." NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "create" ) )
	{
		quest_create( ch, argument );
		return;
	}


	if ( !str_prefix( arg1, "id" ) )
	{
		QUEST_INDEX_DATA* pQu;
		int	nr;

		if ( argument[0] == '\0' || !is_number( argument ) )
		{
			send_to_char( "Syntax: id <quest_id>" NL, ch );
			return;
		}

		nr = atoi( argument );
		for( auto* pQu : quest_index_list )
			if ( pQu->quest_id == nr && pQu != pQuest)
		{
			send_to_char( "There is a quest with that ID. Choose another." NL, ch );
			return;
		}
		pQuest->quest_id = nr;
		send_to_char( "Quest ID set." NL, ch );
		return;
	}

	if ( ( value = flag_value( quest_flags_list, arg ) ) != -1 )
	{
		TOGGLE_BIT( pQuest->flags, value );
		send_to_char( "Quest flag toggled." NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "description" ) )
	{
		if ( argument[0] == '\0' )
		{
			string_append( ch, &pQuest->description );
			return;
		}

		send_to_char( "Syntax:  desc    - enters the editor" NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "time" ) )
	{
		pQuest->time = atoi( argument );
		send_to_char( "Time limit set" NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "penalty" ) )
	{
		pQuest->penalty = atoi( argument );
		send_to_char( "Exp Penalty set" NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "minlevel" ) )
	{
		int lev;

		if( !( lev = atoi( argument ) ) || lev > MAX_LEVEL )
		{
			ch_printf( ch, "Minlevel vnum can be a number: 1 - %d" NL, MAX_LEVEL );
			return;
		}

		pQuest->min_lev = lev;
		send_to_char( "Minlevel set" NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "maxlevel" ) )
	{
		int lev;

		if( !( lev = atoi( argument ) ) || lev > MAX_LEVEL )
		{
			ch_printf( ch, "Maxlevel vnum can be a number: 1 - %d" NL, MAX_LEVEL );
			return;
		}

		pQuest->max_lev = lev;
		send_to_char( "Maxlevel set" NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "questor" ) )
	{
		int lev;

		lev = atoi( argument );
		if( lev < 0 || lev > MAX_VNUM )
		{
			ch_printf( ch, "Questor vnum can be a number: 1 - %d" NL, MAX_VNUM );
			return;
		}
		pQuest->questor = lev;
		send_to_char( "Questor set." NL, ch );
		return;
	}

	if ( !str_prefix( arg1, "next" ) || !str_cmp( arg1, ">" ) )
	{
		if( get_chapter_index(pQuest, ch->pcdata->line_nr+1 ) )
		{
			ch->pcdata->line_nr++;
			send_to_char( "Ok. Jumps to next chapter" NL, ch );
		}
		else
			send_to_char( "Cannot jump to next chapter!" NL, ch );

		return;
	}

	if ( !str_prefix( arg1, "prev" ) || !str_cmp( arg1, "<" ) )
	{
		if( get_chapter_index(pQuest, ch->pcdata->line_nr-1 ) )
		{
			ch->pcdata->line_nr--;
			send_to_char( "Ok. Jumps to previous chapter" NL, ch );
		}
		else
			send_to_char( "Cannot jump to prev chapter!" NL, ch );

		return;
	}

	if ( !str_prefix( arg1, "out" ) )
	{
		if( ch->pcdata->line_nr == 0 )
		{
			send_to_char( "You're out already. Don't panic ;-)" NL, ch );
			return;
		}
		ch->pcdata->line_nr = 0;
		send_to_char( "Ok. We're back to raw Qeditor." NL, ch );
		return;
	}


	argument 	= one_argument( argument, arg2 );
	if ( !str_prefix( arg1, "chapter" ) )
	{
		char 	arg3	[MIL];

		if( arg2[0] == '\0' )
		{
			send_to_char( "Syntax: chapter add      [nr]" NL, ch );
			send_to_char( "        chapter delete   [nr]" NL, ch );
			send_to_char( "        chapter          <nr>" NL, ch );
			return;
		}

		argument = one_argument( argument, arg3 );

		if ( arg2[0] != '\0' && is_number( arg2 ) )
		{
			if( get_chapter_index( pQuest, atoi( arg2 ) ) )
			{
				ch->pcdata->line_nr = atoi( arg2 );
				send_to_char( "Cursor moved." NL, ch );
			}
			else
				send_to_char( "No such chapter." NL, ch );

			return;
		}

		if ( !str_prefix( arg2, "add" ) )
		{
			CHAPTER_INDEX_DATA *chapt;
			CHAPTER_INDEX_DATA *chapt_new;
			int 		nr = 0;

			if( arg3[0] != '\0' && !is_number( arg3 ) )
			{
				send_to_char( "Syntax: chapter add [nr]." NL, ch );
				return;
			}

			if( arg3[0] == '\0')	nr = ch->pcdata->line_nr+1;
			else 			nr = atoi( arg3 );

			CREATE( chapt_new, CHAPTER_INDEX_DATA, 1 );
			STRDUP( chapt_new->name, "no_name" );
			chapt_new->type	= CHAPTER_CRITICAL;

			if ( (chapt = get_chapter_index(pQuest, nr)) != NULL )
			{
				auto it = std::find(pQuest->chapters.begin(), pQuest->chapters.end(), chapt);
				pQuest->chapters.insert(it, chapt_new);
			}
			else
				pQuest->chapters.push_back( chapt_new );

			send_to_char( "Chapter added." NL, ch );
			ch->pcdata->line_nr = nr;
			return;
		}

		if ( !str_prefix( arg2, "delete" ) )
		{
			CHAPTER_INDEX_DATA *chapt;
			int 		nr = 0;

			if( arg3[0] != '\0' && !is_number( arg3 ) )
			{
				send_to_char( "Syntax: chapter delete [nr]." NL, ch );
				return;
			}

			if( arg3[0] == '\0')
				nr = ch->pcdata->line_nr+1;
			else
				nr = atoi( arg3 );

			if ( (chapt = get_chapter_index(pQuest, nr)) == NULL )
			{
				send_to_char( "No such chapter!" NL, ch );
				return;
			}

			pQuest->chapters.remove( chapt );
			free_chapter( chapt );

			if( ch->pcdata->line_nr == nr )
			{
				send_to_char( "Leaving chapter editor." NL, ch );
				ch->pcdata->line_nr = 0;
			}
			send_to_char( "Chapter deleted." NL, ch );
			return;
		}

	}


    /*
	init   <nr>  <add|delete|command|arg>
	event  <nr>  <add|delete|command|arg>
	action <nr>  <add|delete|command|arg>


	Jest 2.00 w nocy -- to MO�E by� lamerskie :P
	*/
	if ( !str_prefix( arg1, "init" ) )
	{
		CHAPTER_INDEX_DATA *	pChapter;
		QUEST_CMND_DATA *	pCmnd;
		QUEST_CMND_DATA *	pCmnd_new;
		int			ii;
		int			nr;
		char			arg3	[MIL];

		argument = one_argument( argument, arg3 );

#define get_init_cmd( src, nr )					        \
{												\
			pCmnd = nullptr; ii = 1;				\
			for( auto* _ic : (src)->init_cmds ) {	\
		    		if( ii == (nr) ) { pCmnd = _ic; break; }	\
					ii++;							\
			}										\
	}


		if( arg2[0] == '\0' || arg3[0] == '\0' )
{
	send_to_char( "Syntax: init add <nr>" NL, ch );
	send_to_char( "        init <nr> delete" NL, ch );
	send_to_char( "        init <nr> cmd | command" NL, ch );
	send_to_char( "        init <nr> (arg) <value>" NL, ch );
	send_to_char( "Where " FG_YELLOW "(arg)" PLAIN " is what you see in orange." NL, ch );
	return;
}

		if( ch->pcdata->line_nr == 0 )
			pChapter = NULL;
		else
			pChapter = get_chapter_index( pQuest, ch->pcdata->line_nr);

		if( !str_cmp( arg2, "add" ) )
{
	if( arg3[0] == '\0' )
	{
		send_to_char( "Syntax: init add <nr>." NL, ch );
		return;
	}

	nr = atoi(arg3);

	CREATE( pCmnd_new, QUEST_CMND_DATA, 1 );
	pCmnd_new->arg1 	= 0;
	pCmnd_new->arg2 	= 0;
	pCmnd_new->arg3 	= 0;
	STRDUP( pCmnd_new->arg4, "" );
	pCmnd_new->command 	= INIT_LOAD_MOB;

	if( pChapter )
	{
		get_init_cmd(pChapter, nr);
		if ( pCmnd )
		{
			auto it = std::find(pChapter->init_cmds.begin(), pChapter->init_cmds.end(), pCmnd);
			pChapter->init_cmds.insert(it, pCmnd_new);
		}
		else
			pChapter->init_cmds.push_back( pCmnd_new );
	}
	else
	{
		get_init_cmd(pQuest, nr);
		if ( pCmnd )
		{
			auto it = std::find(pQuest->init_cmds.begin(), pQuest->init_cmds.end(), pCmnd);
			pQuest->init_cmds.insert(it, pCmnd_new);
		}
		else
			pQuest->init_cmds.push_back( pCmnd_new );
	}

	send_to_char( "Init Command Added." NL, ch );
	return;
}


	nr = atoi(arg2);

	pCmnd = NULL;

	if ( pChapter )
{
	get_init_cmd( pChapter, nr );
}
	else
{
	get_init_cmd( pQuest,   nr );
}

	if( !pCmnd )
{
	send_to_char( "Init command not found." NL, ch );
	return;
}

	if( (nr = find_q_arg( CMD_INIT, pCmnd->command, arg3 )) != CMD_NOTFOUND )
{
	if( argument[0] == '\0' && nr != 4 )
	{
		qedit( ch->desc, (char *)"init" );
		return;
	}
	if	( nr == 1 )	pCmnd->arg1 = atoi( argument );
	else
		if	( nr == 2 )	pCmnd->arg2 = atoi( argument );
	else
		if	( nr == 3 )	pCmnd->arg3 = atoi( argument );
	else
		if	( nr == 4 )
	{
		if( argument[0] == '\0' )
		{
			string_append( ch, &pCmnd->arg4 );
			return;
		}
		else
			STRDUP( pCmnd->arg4, argument );
	}

	ch_printf( ch, "%s set." NL,
			   capitalize(show_q_arg( CMD_INIT, pCmnd->command, nr )) );
	qprepare_comment( pCmnd, CMD_INIT );
	return;
}

	if( !str_cmp( arg3, "cmd" ) || !str_cmp( arg3, "command" ) )
{
	if ( ( value = flag_value(quest_init_types, argument ) ) != -1 )
	{
		pCmnd->command = value;
		ch_printf( ch,"Init command cmd set." NL );
		qprepare_comment( pCmnd, CMD_INIT );
		return;
	}
}

	if( !str_cmp( arg3, "delete" ) )
{

	if( pChapter )
		pChapter->init_cmds.remove( pCmnd );
	else
		pQuest->init_cmds.remove( pCmnd );

	free_qcmd( pCmnd );
	send_to_char( "Init command deleted." NL, ch );
	return;
}
	send_to_char( "What? Type 'init' to see the syntax." NL, ch );
	return;

#undef get_init_cmd
	}


	if ( !str_prefix( arg1, "event" ) )
{
	CHAPTER_INDEX_DATA *	pChapter;
	QUEST_CMND_DATA *	pCmnd;
	QUEST_CMND_DATA *	pCmnd_new;
	int			ii;
	int			nr;
	char			arg3	[MIL];

	argument = one_argument( argument, arg3 );

#define get_event( src, nr )					        \
{								\
		pCmnd = nullptr; ii = 1;			\
		for( auto* _ev : (src)->events ) {	\
		    if( ii == (nr) ) { pCmnd = _ev; break; }	\
			ii++;						\
		}							\
}


	if( arg2[0] == '\0' || arg3[0] == '\0' )
{
	send_to_char( "Syntax: event add <nr>" NL, ch );
	send_to_char( "        event <nr> delete" NL, ch );
	send_to_char( "        event <nr> cmd | command" NL, ch );
	send_to_char( "        event <nr> (arg) <value>" NL, ch );
	send_to_char( "Where " FG_YELLOW "(arg)" PLAIN " is what you see in orange." NL, ch );
	return;
}

	if( ch->pcdata->line_nr == 0 )
{
	send_to_char( "You aren't editing a chapter." NL, ch );
	return;
}
	else
		pChapter = get_chapter_index( pQuest, ch->pcdata->line_nr);

	if( !str_cmp( arg2, "add" ) )
{
	if( arg3[0] == '\0' )
	{
		send_to_char( "Syntax: event add <nr>." NL, ch );
		return;
	}

	nr = atoi(arg3);

	CREATE( pCmnd_new, QUEST_CMND_DATA, 1 );
	pCmnd_new->arg1 	= 0;
	pCmnd_new->arg2 	= 0;
	pCmnd_new->arg3 	= 0;
	STRDUP( pCmnd_new->arg4, "" );
	pCmnd_new->command 	= EVENT_VISIT_MOB;


	get_event(pChapter, nr);

	if ( pCmnd )
	{
		auto it = std::find(pChapter->events.begin(), pChapter->events.end(), pCmnd);
		pChapter->events.insert(it, pCmnd_new);
	}
	else
		pChapter->events.push_back( pCmnd_new );

	send_to_char( "Event Added." NL, ch );
	return;
}


	nr = atoi(arg2);

	pCmnd = NULL;

	get_event( pChapter, nr );

	if( !pCmnd )
{
	send_to_char( "Event not found." NL, ch );
	return;
}

	if( (nr = find_q_arg( CMD_EVENT, pCmnd->command, arg3 )) != CMD_NOTFOUND )
{
	if( argument[0] == '\0' && nr != 4 )
	{
		qedit( ch->desc, (char *)"event" );
		return;
	}
	if	( nr == 1 )	pCmnd->arg1 = atoi( argument );
	else
		if	( nr == 2 )	pCmnd->arg2 = atoi( argument );
	else
		if	( nr == 3 )	pCmnd->arg3 = atoi( argument );
	else
		if	( nr == 4 )
	{
		if( argument[0] == '\0' )
		{
			string_append( ch, &pCmnd->arg4 );
			return;
		}
		else
			STRDUP( pCmnd->arg4, argument );
	}

	ch_printf( ch, "%s set." NL,
			   capitalize(show_q_arg( CMD_EVENT, pCmnd->command, nr )) );
	qprepare_comment( pCmnd, CMD_EVENT );
	return;
}

	if( !str_cmp( arg3, "cmd" ) || !str_cmp( arg3, "command" ) )
{
	if ( ( value = flag_value(quest_event_types, argument ) ) != -1 )
	{
		pCmnd->command = value;
		ch_printf( ch,"Event cmd set." NL );
		qprepare_comment( pCmnd, CMD_EVENT );
		return;
	}
}

	if( !str_cmp( arg3, "delete" ) )
{
	pChapter->events.remove( pCmnd );
	free_qcmd( pCmnd );
	send_to_char( "Event deleted." NL, ch );
	return;
}

	send_to_char( "What? Type 'event' to see the syntax." NL, ch );
	return;

#undef get_event
}


    if ( !str_prefix( arg1, "action" ) )
{
	CHAPTER_INDEX_DATA *	pChapter;
	QUEST_ACTION_DATA *	pCmnd;
	QUEST_ACTION_DATA *	pCmnd_new;
	int			ii;
	int			nr;
	char			arg3	[MIL];

	argument = one_argument( argument, arg3 );

#define get_action( src, nr )					        \
{								\
		pCmnd = nullptr; ii = 1;			\
		for( auto* _ac : (src)->actions ) {	\
		    if( ii == (nr) ) { pCmnd = _ac; break; }	\
			ii++;						\
		}							\
}


	if( arg2[0] == '\0' || arg3[0] == '\0' )
{
	send_to_char( "Syntax: action add <nr>" NL, ch );
	send_to_char( "        action <nr> delete" NL, ch );
	send_to_char( "        action <nr> cmd | command" NL, ch );
	send_to_char( "        action <nr> (arg) <value>" NL, ch );
	send_to_char( "Where " FG_YELLOW "(arg)" PLAIN " is what you see in orange." NL, ch );
	return;
}

	if( ch->pcdata->line_nr == 0 )
		pChapter = NULL;
	else
		pChapter = get_chapter_index( pQuest, ch->pcdata->line_nr);

	if( !str_cmp( arg2, "add" ) )
{
	if( arg3[0] == '\0' )
	{
		send_to_char( "Syntax: action add <nr>." NL, ch );
		return;
	}

	nr = atoi(arg3);

	CREATE( pCmnd_new, QUEST_ACTION_DATA, 1 );
	pCmnd_new->arg1 	= 0;
	pCmnd_new->arg2 	= 0;
	pCmnd_new->arg3 	= 0;
	STRDUP( pCmnd_new->arg4, "" );
	pCmnd_new->command 	= ACTION_TELL;

	if( pChapter )
	{
		get_action(pChapter, nr);
		if ( pCmnd )
		{
			auto it = std::find(pChapter->actions.begin(), pChapter->actions.end(), pCmnd);
			pChapter->actions.insert(it, pCmnd_new);
		}
		else
			pChapter->actions.push_back( pCmnd_new );
	}
	else
	{
		get_action(pQuest, nr);
		if ( pCmnd )
		{
			auto it = std::find(pQuest->actions.begin(), pQuest->actions.end(), pCmnd);
			pQuest->actions.insert(it, pCmnd_new);
		}
		else
			pQuest->actions.push_back( pCmnd_new );
	}

	send_to_char( "Action Added." NL, ch );
	return;
}


	nr = atoi(arg2);

	pCmnd = NULL;

	if ( pChapter )
{
	get_action( pChapter, nr );
}
	else
{
	get_action( pQuest,   nr );
}

	if( !pCmnd )
{
	send_to_char( "Action not found." NL, ch );
	return;
}

	if( (nr = find_q_arg( CMD_ACTION, pCmnd->command, arg3 )) != CMD_NOTFOUND )
{
	if( argument[0] == '\0' && nr != 4 )
	{
		qedit( ch->desc, (char *)"action" );
		return;
	}
	if	( nr == 1 )	pCmnd->arg1 = atoi( argument );
	else
		if	( nr == 2 )	pCmnd->arg2 = atoi( argument );
	else
		if	( nr == 3 )	pCmnd->arg3 = atoi( argument );
	else
		if	( nr == 4 )
	{
		if( argument[0] == '\0' )
		{
			string_append( ch, &pCmnd->arg4 );
			return;
		}
		else
			STRDUP( pCmnd->arg4, argument );
	}

	ch_printf( ch, "%s set." NL,
			   capitalize(show_q_arg( CMD_ACTION, pCmnd->command, nr )) );
	qprepare_comment( pCmnd, CMD_ACTION );
	return;
}

	if( !str_cmp( arg3, "cmd" ) || !str_cmp( arg3, "command" ) )
{
	if ( ( value = flag_value(quest_action_types, argument ) ) != -1 )
	{
		pCmnd->command = value;
		ch_printf( ch,"Action cmd set." NL );
		qprepare_comment( pCmnd, CMD_ACTION );
		return;
	}
}

	if( !str_cmp( arg3, "delete" ) )
{

	if( pChapter )
		pChapter->actions.remove( pCmnd );
	else
		pQuest->actions.remove( pCmnd );

	free_qaction( pCmnd );
	send_to_char( "Action deleted." NL, ch );
	return;
}

	send_to_char( "What? Type 'action' to see the syntax." NL, ch );
	return;

#undef get_action
}

    interpret( ch, arg );
	return;
}

#undef show_args


void save_quest( QUEST_INDEX_DATA * pQuest )
{
	QUEST_CMND_DATA	*	pCmnd;
	QUEST_ACTION_DATA	*	pAction;
	CHAPTER_INDEX_DATA *	pChapter;
	char			buf		[MSL];
	FILE *			fpout;

	sprintf( buf, "Saving Quest: %s...", pQuest->name );
	log_string_plus( buf, LOG_NORMAL, LEVEL_GREATER );

	sprintf( buf, "%s%d.dat", QINDEX_DIR, pQuest->quest_id );

	RESERVE_CLOSE;
	if ( ( fpout = fopen( buf, "w" ) ) == NULL )
	{
		bug( "fopen" );
		perror( buf );
		RESERVE_OPEN;
		return;
	}

	fprintf( fpout, "#QUEST\n" );
	fprintf( fpout, "#%d\n",		    pQuest->quest_id	);
	fprintf( fpout, "Name         %s~\n",   pQuest->name    	);
	fprintf( fpout, "Author       %s~\n",   pQuest->author  	);
	fprintf( fpout, "MinLevel     %d\n",    pQuest->min_lev 	);
	fprintf( fpout, "MaxLevel     %d\n",    pQuest->max_lev 	);
	fprintf( fpout, "Questor      %d\n",    pQuest->questor 	);
	fprintf( fpout, "Penalty      %d\n",    pQuest->penalty 	);
	fprintf( fpout, "Time         %d\n",    pQuest->time    	);
	fprintf( fpout, "Flags        %lld\n",  pQuest->flags		); /* Trog */
	fprintf( fpout, "Security     %d\n",    pQuest->security	);
	fprintf( fpout, "Desc\n%s~\n",strip_cr( pQuest->description ) );

	for( auto* pCmnd : pQuest->init_cmds )
		fprintf( fpout, " Init    %-8s %6d %6d %6d %s~\n",
				 bit_name( quest_init_types, pCmnd->command ),
				 pCmnd->arg1,     pCmnd->arg2,
				 pCmnd->arg3,     pCmnd->arg4 );

	for( auto* pAction : pQuest->actions )
		fprintf( fpout, " Action  %-8s %6d %6d %6d %s~\n",
				 bit_name( quest_action_types, pAction->command ),
				 pAction->arg1,     pAction->arg2,
				 pAction->arg3,     pAction->arg4 );

	fprintf( fpout, "End\n" );

	for( auto* pChapter : pQuest->chapters )
	{

		fprintf( fpout, "#CHAPTER %s\n", *pChapter->name ?
				pChapter->name : "no_name" );
		fprintf( fpout, " Type    %d\n",  pChapter->type );

		for( auto* pCmnd : pChapter->init_cmds )
			fprintf( fpout, " Init    %-8s %6d %6d %6d %s~\n",
					 bit_name( quest_init_types, pCmnd->command ),
					 pCmnd->arg1,     pCmnd->arg2,
					 pCmnd->arg3,     pCmnd->arg4 );

		for( auto* pCmnd : pChapter->events )
			fprintf( fpout, " Event   %-8s %6d %6d %6d %s~\n",
					 bit_name( quest_event_types, pCmnd->command ),
					 pCmnd->arg1,     pCmnd->arg2,
					 pCmnd->arg3,     pCmnd->arg4 );

		for( auto* pAction : pChapter->actions )
			fprintf( fpout, " Action  %-8s %6d %6d %6d %s~\n",
					 bit_name( quest_action_types, pAction->command ),
					 pAction->arg1,     pAction->arg2,
					 pAction->arg3,     pAction->arg4 );

		fprintf( fpout, " End\n" );
	}
	fprintf( fpout, "#END\n\n" );

	fclose( fpout );
	RESERVE_OPEN;
	return;
}

void write_quest_list( )
{
	QUEST_INDEX_DATA *	quest;
	FILE *		fpout;

	fpout = fopen( QUEST_LIST, "w" );
	if ( !fpout )
	{
		bug( "Cannot open quest.lst for writing!" );
		return;
	}
	for ( auto* quest : quest_index_list )
		fprintf( fpout, "%d.dat\n", quest->quest_id );
	fprintf( fpout, "$\n" );
	fclose( fpout );
	return;
}


