/***************************************************************************
* Star Wars Reality 1.0						           *
* copyright (c) 1997, 1998 by Sean Cooper                                  *
* Starwars and Starwars Names copyright(c) Lucasfilm Ltd.                  *
* SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider                           *
* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,                    *
* Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops                *
* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
* Chastain, Michael Quan, and Mitchell Tse.                                *
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
* Michael Seifert, Hans Henrik Strfeldt, Tom Madsen, and Katja Nyboe.     *
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
*					Dialogs editor and routins 							   *
****************************************************************************/
#include "olc.h"
#include "mud.h"


/*! \fn bool dialog_leaf( DIALOG_NODE * pNode )
*	\brief sprawdza czy dany wezel jest tekstem czy etapem
*	\param pNode - wezel dialogu
*/
bool dialog_leaf( DIALOG_NODE * pNode )
{
	return pNode->children.empty();
}

/*! \fn int free_dialog_ID()
*	\brief znajdz pierwszy wolny numer etapu
*/
int free_dialog_ID()
{
	int				val=0;
	bool			fMatch;

	for(;;)
	{
		fMatch = false;
		for(auto* pData : dialog_list)
			if ( pData->dialog_ID == val)
			{
				fMatch = true;
				break;
			}
		if ( !fMatch )
			return val;
		val++;
	}
	return 0;
}

/*! \fn DIALOG_DATA * get_dialog( char * name )
*	\brief pobierz dialog o podanej nazwie
*	\param name - nazwa dialogu
*/
DIALOG_DATA * get_dialog( char * name )
{
	for(auto* pData : dialog_list)
		if ( !str_cmp( pData->name, name ) )
				return pData;

	for(auto* pData : dialog_list)
		if ( !str_prefix( pData->name, name ) )
				return pData;
	return NULL;
}

/*! \fn DIALOG_NODE * get_answer( DIALOG_NODE * pBase, int nr)
*	\brief zwroc odpowiedz o podanym numerze
*	\param pBase - etap dialogu
*	\param nr - numer wybranej odpowiedzi
*/
DIALOG_NODE * get_answer( DIALOG_NODE * pBase, int nr)
{
	if ( pBase->children.empty() )
		return NULL;
	int idx = nr - 1;
	if ( idx < 0 || idx >= (int)pBase->children.size() )
		return NULL;
	auto it = pBase->children.begin();
	std::advance(it, idx);
	return *it;
}

/*! \fn int find_free_nr( DIALOG_DATA * pBase)
*	\brief znajdz pierwszy wolny numer odpowiedzi
*	\param pBase - nazwa dialogu
*/
int find_free_nr( DIALOG_DATA * pBase)
{
	int			val = 0;
	bool		fMatch;
	if ( pBase->nodes.empty() )
		return 0;

	for(;;)
	{
		fMatch = false;
		for(auto* pNode : pBase->nodes)
			if ( pNode->nr == val)
			{
				fMatch = true;
				break;
			}
		if ( !fMatch )
			return val;
		val++;
	}

	return 0;
}

DIALOG_NODE * get_dialog_target( DIALOG_DATA * pBase, int nr)
{
	for(auto* pNode : pBase->nodes)
		if ( pNode->nr == nr )
			return pNode;
	return NULL;
}

void save_dialogs_list()
{
	FILE			* fp;
	RESERVE_CLOSE;

	if ( ( fp = fopen( DIALOG_LIST_FILE, "w" ) ) == NULL )
	{
		bug("fopen");
		perror( DIALOG_LIST_FILE );
		RESERVE_OPEN;
		return;
	}

	for(auto* pData : dialog_list)
		fprintf( fp, "%d\n", pData->dialog_ID );

	fprintf( fp,"-1\n$\n" );
	fclose( fp );
	RESERVE_OPEN;
}

void fread_dialog( char * dialog_file, int ID )
{
	DIALOG_DATA		* pData = NULL;
	DIALOG_NODE		* pNode = NULL, * pLeaf = NULL;
	MPROG_DATA		* pMProg = NULL;
	FILE			* fp;
	const char		* word;
	bool			fMatch;

	RESERVE_CLOSE;

	if ( ( fp = fopen( dialog_file, "r") ) == NULL )
	{
		bug("fopen");
		perror( dialog_file );
		RESERVE_OPEN;
		return;
	}
	pData = new_dialog_data();
	pData->dialog_ID = ID;
	dialog_list.push_back( pData );
	if ( !pData->nodes.empty() )
	{
		pNode = pData->nodes.front();
		pData->nodes.erase( pData->nodes.begin() );
		free_dialog_node( pNode );
	}
	for(;;)
	{
		word = feof( fp ) ? "#END" : fread_word( fp );
		fMatch = false;
		switch( UPPER( word[0] ) )
		{
			case '#' :
			{
				fclose( fp );
				RESERVE_OPEN;
				return;
			}
			break;
			case 'A' :
			SKEY( "Answer", pLeaf->mob_answer, fread_string( fp ) );
			break;
			case 'N':
			SKEY( "Name", 	pData->name,	fread_string( fp ) );
			if ( !str_cmp( word, "NodeID" ) )
			{
				pNode->nr = fread_number( fp );
				fMatch = true;
			}
			SKEY( "NodeTekst",	pLeaf->text,fread_string( fp ) );
			break;
			case 'T':
			if ( !str_cmp( word, "Tekst" ) )
			{
				pNode = new_dialog_node();
				pData->nodes.push_back( pNode );
				pNode->text = fread_string( fp );
				pNode->pBase = pData;
				fMatch = true;
			}
			if ( !str_cmp( word, "TargetNR" ) )
			{
				pLeaf = new_dialog_node();
				pLeaf->pBase = pData;
				pData->nodes.push_back( pLeaf );
				pNode->children.push_back( pLeaf );
				pLeaf->target_nr = fread_number( fp );
				fMatch = true;
			}
			break;
			case '>' :
			{
				CREATE( pMProg, MPROG_DATA, 1 );
				pLeaf->prog		= pMProg;
				pMProg->type	= mprog_name_to_type( fread_word( fp ) );
				pMProg->arglist	= fread_string( fp );
				fread_to_eol( fp );
				pMProg->comlist	= fread_string( fp );
				fMatch = true;
			}
			break;
			case '|' :
				fMatch = true;
			break;
		}
		if (!fMatch)
			bug( "no match : %s", word );
	}
	fclose( fp );
	RESERVE_OPEN;
}

void load_dialogs_list()
{
	FILE			* fp;
	int				val;
	char			buf[MIL];
	char		*strval;

	RESERVE_CLOSE;

	if ( ( fp = fopen( DIALOG_LIST_FILE, "r") ) == NULL )
	{
		bug("fopen");
		perror( DIALOG_LIST_FILE );
		RESERVE_OPEN;
		return;
	}

	for(;;)
	{
		strval = feof( fp ) ? (char *)"$" : fread_word( fp );
		if( *strval == '$' )
			break;

		val = atoi( strval );

		sprintf( buf, "%s%d.dat",DIALOG_DIR, val );

		if ( val == -1 )
			break;
		fread_dialog( buf, val );
	}

	fclose( fp );
	RESERVE_OPEN;
}

void save_dialog( CHAR_DATA * ch )
{
	FILE			* fp;
	char			buf[MSL];
	DIALOG_DATA		* pData;

	if ( !(pData = ((DIALOG_NODE *) ch->desc->olc_editing )->pBase ) )
		return;
	RESERVE_CLOSE;
	sprintf( buf, "%s%d.dat",DIALOG_DIR, pData->dialog_ID );

	if ( ( fp = fopen( buf, "w" ) ) == NULL )
	{
		bug("fopen");
		perror( buf );
		RESERVE_OPEN;
		return;
	}
	fprintf( fp, "Name          %s~\n\n",pData->name );
	for(auto* pNode : pData->nodes)
		pNode->saved = false;

	for(auto* pNode : pData->nodes)
	{
		if ( pNode->saved ) continue;

		fprintf( fp, "Tekst         %s~\n", pNode->text );
		fprintf( fp, "NodeID        %d\n", pNode->nr );
		pNode->saved = true;
		for(auto* pNodeDialog : pNode->children)
		{
			fprintf( fp, "TargetNR      %d\n",pNodeDialog->target_nr );
			fprintf( fp, "NodeTekst     %s~ \n",pNodeDialog->text );
			fprintf( fp, "Answer        %s~\n",
			pNodeDialog->mob_answer);
			if ( pNodeDialog->prog )
				fprintf( fp, "> %s %s~\n%s~\n",
					mprog_type_to_name( pNodeDialog->prog->type ),
					pNodeDialog->prog->arglist,
					strip_cr( pNodeDialog->prog->comlist ) );
			fprintf( fp, "|\n");
			pNodeDialog->saved = true;
		}
	}
	fprintf( fp, "#END\n" );
	fclose( fp );
	RESERVE_OPEN;
}

void dialog_show_phase( CHAR_DATA * ch, DIALOG_NODE * pNode )
{
	int				number = 1;

	if ( !pNode || IS_NPC( ch ) )
		return;


	pager_printf( ch, FG_CYAN "%s m�wi :",
		fcapitalize( ch->pcdata->pMob_speaking->przypadki[0] ) );

	if ( ch->pcdata->last_dialog[0] =='\0' )
		pager_printf( ch, PLAIN "%s" EOL EOL, pNode->text );
	else
		pager_printf( ch, PLAIN "%s" EOL EOL, ch->pcdata->last_dialog );

	if ( pNode->children.empty() )
	{
		pager_printf( ch, PLAIN " Brak tekst�w " EOL EOL );
	}
	else
	{
		for(auto* pLeaf : pNode->children)
			pager_printf( ch, FG_CYAN "%-2d : " MOD_BOLD "%s" EOL,
				number++, pLeaf->text );
	}
}


void dialog_show( CHAR_DATA * ch, DIALOG_NODE * pNode )
{
	int				number = 1;

	if ( !pNode )
		return;

	pager_printf( ch, FG_CYAN "Nr listy :" PLAIN "%d" EOL, pNode->nr );
	pager_printf( ch, FG_CYAN "Tekst    :" PLAIN "%s" EOL EOL, pNode->text);
	if ( pNode->children.empty() )
	{
		pager_printf( ch, PLAIN " Brak tekst�w " EOL EOL );
	}
	else
	{
		pager_printf( ch, FG_CYAN "-------------------------------" EOL );
		for(auto* pLeaf : pNode->children)
		{
			pager_printf( ch, FG_CYAN "Test prog - " PLAIN "%s" NL,
				pLeaf->prog ? "TAK" : "BRAK" );
			pager_printf( ch, FG_CYAN "Text. %-2d : " MOD_BOLD "%s" EOL,
				number, pLeaf->text );
			pager_printf( ch, FG_CYAN "Mob answer :" PLAIN "%s" EOL,
				pLeaf->mob_answer );
			pager_printf( ch, FG_CYAN "Execute Prog. - " PLAIN "none" NL);
			pager_printf( ch, FG_CYAN "Lista - ");
			if ( pLeaf->target_nr < 0)
				pager_printf( ch, MOD_BOLD "%d" EOL,pLeaf->target_nr );
			else
				pager_printf( ch, PLAIN "%d" EOL,pLeaf->target_nr );
			pager_printf( ch,
			FG_CYAN "-------------------------------" EOL );
			number++;
		}
	}
}

void dialog_show_all( CHAR_DATA * ch, DIALOG_NODE * pShowNode )
{
	DIALOG_DATA			* pData;

	pData = pShowNode->pBase;

	pager_printf( ch, FG_CYAN "Oto lista wszystkich etap�w rozmowy" NL EOL, ch );
	pager_printf( ch, FG_CYAN "[" PLAIN "Nr" FG_CYAN "]["
	PLAIN "Tekst                    " FG_CYAN "]" NL );
	pager_printf( ch, "-------------------------------" NL );
	for(auto* pNode : pData->nodes)
		if ( pNode->nr >= 0	)
		{
			pager_printf( ch, FG_CYAN "[" MOD_BOLD "%2d" FG_CYAN "]",
				pNode->nr );
			pager_printf( ch, FG_CYAN "[" PLAIN "%-45s" FG_CYAN "]" NL,
				strip_colors( pNode->text, 25 ) );
		}
	pager_printf(ch,NL);
}

DEF_DO_FUN( diallist )
{
	send_to_char( "Lista wszystkich dialog�w" NL NL, ch );
	for(auto* pData : dialog_list)
		pager_printf( ch, "%i - %s" NL,pData->dialog_ID, pData->name );
	pager_printf( ch, NL );
}

void dialedit( DESCRIPTOR_DATA * d, char * argument )
{
	CHAR_DATA			* ch = d->character;
	DIALOG_NODE			* pEditNode;
	DIALOG_NODE			* pNode, * pNewNode;
	char				arg[MSL];
	char				arg1[MSL];
	char				arg2[MSL];
	char				arg3[MIL];
	int					value;

	strcpy( arg, argument );
	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	strcpy( arg2, argument );

	if ( !(pEditNode = (DIALOG_NODE *) ch->desc->olc_editing ) )
	{
		bug("dialedit: null dialog editing struct");
		edit_done(ch, (char *)"");
		return;
	}

	if ( !str_prefix( arg1, "show" ) )
	{
		argument = one_argument( argument, arg2 );
		if ( arg2[0] != '\0' && !str_cmp( arg2, "all" ) )
		{
			dialog_show_all( ch , pEditNode );
			return;
		}
		dialog_show( ch, pEditNode);
		return;
	}
	GET_HELP( argument );
	SET_STR( "name", pEditNode->pBase->name, argument);
	if ( !str_prefix( arg1, "fold" ) )
	{
		save_dialog( ch );
		send_to_char( "Dialog saved." NL, ch );
		return;
	}
	if ( !str_prefix( arg1, "foldlist") )
	{
		save_dialogs_list();
		donemsg( ch );
		return;
	}

	if ( !str_prefix( arg1, "text") )
	{
		argument = one_argument( argument, arg1 );
		if ( arg1[0] == '\0' )
		{
      string_append(ch, &pEditNode->text);
			return;
		}
		if ( !str_prefix( arg1, "add") )
		{
			pNode = new_dialog_node();
			pEditNode->pBase->nodes.push_back( pNode );
			pEditNode->children.push_back( pNode );
			pNode->pBase = pEditNode->pBase;
			send_to_char("New answer add" NL, ch );
			return;
		}
		else if ( !str_prefix( arg1, "edit" ) )
		{
			string_append(ch, &pEditNode->text);
			return;
		}
		else if ( is_number( arg1 ) )
		{
			argument = one_argument( argument, arg3 );
			pNode = get_answer( pEditNode, atoi(arg1) );
			if (!pNode)
			{
				send_to_char( "Nie ma tekstu o takim nr." NL, ch );
				return;
			}
			if ( arg3[0] == '\0' )
			{
				/*send_to_char(
				"Syntax : <text>" NL
				"         prog" NL
				"         prog show" NL
				"         follow" NL
				"         create" NL
				"         delete" NL
				"         mob <tekst>" NL
				"         target <nr>" NL, ch);*/
        string_append(ch, &pNode->text);
				return;
			}
			else if ( !str_prefix( arg3, "prog" ) )
			{
				strcpy( arg2, argument );
				argument = one_argument( argument, arg1 );
				if ( arg1[0] == '\0' )
				{

				}
				return;
			}
			else if ( !str_prefix( arg3, "mob" ) )
			{
				STRDUP(	pNode->mob_answer, argument );
				send_to_char( "Odpowiedz moba zapisana" NL, ch );
				return;
			}
			else if ( !str_prefix( arg3, "delete") )
			{
				value = atoi( arg1);
				pNode = get_answer( pEditNode, value );
				if (!pNode)
				{
					send_to_char( "Nie ma tekstu o takim nr." NL, ch );
					return;
				}
				pEditNode->pBase->nodes.erase(
					std::remove( pEditNode->pBase->nodes.begin(), pEditNode->pBase->nodes.end(), pNode ),
					pEditNode->pBase->nodes.end() );
				pEditNode->children.erase(
					std::remove( pEditNode->children.begin(), pEditNode->children.end(), pNode ),
					pEditNode->children.end() );
				free_dialog_node( pNode );
				send_to_char("Tekst usuni�ty" NL, ch );
				return;
			}
			else if ( !str_prefix( arg3, "follow" ) )
			{
				if ( pNode->target_nr < 0 )
				{
					send_to_char("Ustaw najpierw gdzie chcesz przej�� ." NL , ch );
					return;
				}
				pNewNode = get_dialog_target( pNode->pBase,
				pNode->target_nr);
				if ( !pNewNode )
				{
					send_to_char("Target node isn't existing" NL
					"Check is he realy existing" NL, ch );
					return;
				}
				else
				{
					ch->desc->olc_editing	= (void *)pNewNode;
					dialog_show( ch, pNewNode );
					return;
				}
			}
			else if ( !str_prefix( arg3, "create") )
			{
				if ( pNode->target_nr >=0 )
				{
					send_to_char("Ten tekst juz wskazuje na inny etap"
					" rozmowy." NL, ch );
					return;
				}
				pNewNode = new_dialog_node();
				pNewNode->pBase = pEditNode->pBase;
				pNewNode->nr = find_free_nr( pNewNode->pBase );
				pNewNode->pBase->nodes.push_back( pNewNode );
				pNode->target_nr = pNewNode->nr;
				send_to_char("Target node created." NL, ch );
				return;
			}
			else if ( !str_prefix( arg3, "target" ) )
			{
				value = atoi( argument);
				pNewNode = get_dialog_target( pNode->pBase, value);
				if ( !pNewNode )
				{
					send_to_char("Etap rozmowy o tym numerze nie"
					" istnieje" NL, ch );
					return;
				}
				pNode->target_nr = value;
				send_to_char( "Nr etapu docelowego ustawiony" NL, ch );
				return;

			}
			else
			{
        string_append(ch, &pNode->text);
				//argument = one_argument( arg2, arg1 );
				//STRDUP(	pNode->text, argument );
				//send_to_char( "Tekst zosta� zapisany." NL, ch );
				return;
			}
		}
		else if ( !str_prefix( arg1, "clear") )
		{
			STRDUP( pEditNode->text, "");
			send_to_char("Tekst zosta� usuni�ty" NL, ch);
			return;
		}
		else
		{
      string_append(ch, &pEditNode->text);
			//STRDUP( pEditNode->text,	arg2);
			//send_to_char("Tekst zosta� ustawiony" NL, ch);
			return;
		}
	}
	if ( !str_prefix( arg1, "dialog") )
	{
		argument = one_argument( argument, arg2 );
		argument = one_argument( argument, arg3 );
		if ( arg2[0] == '\0' )
		{
			send_to_char("Syntax : dialog [nr]" NL, ch );
			send_to_char("         dialog [nr] delete" NL, ch );
			return;
		}
		if ( is_number( arg2 ) )
		{
			value = atoi( arg2 );
			pNewNode = get_dialog_target( pEditNode->pBase, value);
			if ( !pNewNode )
			{
				send_to_char("Target node isn't existing" NL
				"Check is he realy existing" NL, ch );
				return;
			}
			if ( arg3[0] == '\0' )
			{
				ch->desc->olc_editing	= (void *)pNewNode;
				send_to_char( "Dialog node changed." NL, ch );
				dialog_show( ch, pNewNode );
				return;
			}
			else if ( !str_cmp( arg3, "delete" ) )
			{
				if ( pNewNode->pBase->nodes.size() <= 1 )
				{
					send_to_char( "Nie mo�esz usun�� jedynego"
					" istniej�cego etapu w tym dialogu" NL, ch );
					return;
				}
				pNode = pNewNode->pBase->nodes.front();
				ch->desc->olc_editing	= (void *)pNode;
				pNewNode->pBase->nodes.erase(
					std::remove( pNewNode->pBase->nodes.begin(), pNewNode->pBase->nodes.end(), pNewNode ),
					pNewNode->pBase->nodes.end() );
				free_dialog_node( pNewNode );
				ch_printf( ch , "Dialog %s zosta� usuniety" NL NL, value );
				dialog_show( ch, pNode );
				return;
			}
		}
		else
		{
			send_to_char( "Podaj nr dialogu" NL, ch );
			return;
		}
		return;
	}
	GET_DONE();

	interpret( ch, arg );
}

bool dialedit_create( CHAR_DATA * ch, char * argument )
{
	char				arg1[MSL];
	DIALOG_DATA			* pData;

	argument = one_argument( argument, arg1 );

	if ( arg1[0] != '\0' )
	{
		pData = new_dialog_data();
		dialog_list.push_back( pData );
		STRDUP( pData->name, arg1 );
		pData->nodes.front()->nr		= 0;
		dialog_show(ch, pData->nodes.front());
	}
	else return false;

	ch->pcdata->line_nr		= 0;
	ch->desc->connected		= CON_DIALOGEDIT;
	ch->desc->olc_editing	= (void *)pData->nodes.front();
	return true;
}

DEF_DO_FUN( dialedit )
{
	char			arg1[MSL];
	DIALOG_DATA		* pData;

	if ( !*argument )
	{
		ch_printf( ch,	"Syntax : dialedit <nazwa>" NL
						"Syntax : dialedit create <nazwa>" NL);
		return;
	}


	if ( !str_prefix( "create", argument ) )
	{
		argument = one_argument( argument, arg1 );
		if ( dialedit_create( ch, argument ) )
			ch_printf( ch, "Editing dialog" NL);
		return;
	}
	else
	{
		pData = get_dialog( argument );
		if ( !pData )
		{
			ch_printf( ch, "Brak takiego dialogu" EOL );
			return;
		}
		dialog_show(ch, pData->nodes.front());

		ch->pcdata->line_nr		= 0;
		ch->desc->connected		= CON_DIALOGEDIT;
		ch->desc->olc_editing	= (void *)pData->nodes.front();
	}
}

DEF_DO_FUN( start_speak )
{
	CHAR_DATA		* pChar;
	ROOM_INDEX_DATA	* pRoom;
	char			arg[MIL];
	DIALOG_DATA		* pData;

	pRoom = ch->in_room;
	if ( pRoom->people.empty() )
		return;
	if ( pRoom->people.size() == 1 && pRoom->people.front() == ch )
		return;

	one_argument( argument, arg );
	if ( arg[0] == '\0' )
	{
		send_to_char( "Z kim chcesz rozmawia� ?", ch );
		return;
	}

	pChar = get_char_room( ch, arg );
	if ( pChar == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego" NL, ch );
		return;
	}
	if ( !IS_NPC( pChar ) )
		return;
	if ( pChar->dialog_name[0] == '\0' )
	{
		send_to_char( "Ten mob nie posiada dialogu" NL, ch );
		return;
	}

	pData = get_dialog( pChar->dialog_name );
	if ( !pData )
	{
		bug("Brak dialogu %s [ mob : %d] ", pChar->dialog_name,
			pChar->pIndexData->vnum);
		return;
	}
	ch->pcdata->line_nr		= 0;
	ch->desc->connected		= CON_SPEAKING;
	ch->desc->olc_editing	= (void *)pData->nodes.front();
	ch->pcdata->pMob_speaking	= pChar;
	act( COL_ACTION, "$n rozpoczyna rozmowe z $N$4.", ch, NULL, pChar, TO_ROOM );
	mob_speak( ch->desc, (char *)"show" );
	return;
}

void mob_speak( DESCRIPTOR_DATA * d, char * argument )
{
	CHAR_DATA			* ch = d->character;
	DIALOG_NODE			* pNode, * pNewNode;
	char				arg[MSL];
	char				arg1[MSL];
	char				arg2[MSL];

	strcpy( arg, argument );
	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	strcpy( arg2, argument );

	if ( !(pNode = (DIALOG_NODE *) ch->desc->olc_editing ) )
	{
		bug("dialedit: null dialog editing struct");
		send_to_char( "Wystapi� b��d - rozmowa przerwana" NL, ch );
		STRDUP(	ch->pcdata->last_dialog, "" );
		ch->pcdata->pMob_speaking	= NULL;
		ch->desc->olc_editing		= NULL;
		ch->desc->connected			= CON_PLAYING;
		return;
	}

	if ( !str_prefix( arg1, "show" ) )
	{
		argument = one_argument( argument, arg2 );
		dialog_show_phase( ch, pNode );
		return;
	}
	if ( !str_prefix( arg1, "done") || !str_prefix( arg1, "koniec" ) )
	{
		act( COL_ACTION, "$n ko�czy rozmow� z $N$4.", ch, NULL, ch->pcdata->pMob_speaking, TO_ROOM );
		STRDUP(	ch->pcdata->last_dialog, "" );
		ch->pcdata->pMob_speaking	= NULL;
		ch->desc->olc_editing		= NULL;
		ch->desc->connected			= CON_PLAYING;
		return;
	}
	if ( is_number( arg1 ) )
	{
		pNewNode = get_answer( pNode, atoi(arg1) );
		if ( !pNewNode )
			return;
		if ( pNewNode->mob_answer[0] == '\0')
			STRDUP(	ch->pcdata->last_dialog, "" );
		else
			STRDUP(	ch->pcdata->last_dialog, pNewNode->mob_answer );

		if ( pNewNode->target_nr == -1 )
		{
			act( COL_ACTION, "$N ko�czy rozmow� z $n$4.", ch, NULL, ch->pcdata->pMob_speaking, TO_ROOM );
			ch_printf( ch, "%s ko�czy z Tob� rozmowe" NL,
		fcapitalize( ch->pcdata->pMob_speaking->przypadki[0] ) );
			STRDUP(	ch->pcdata->last_dialog, "" );
			ch->pcdata->pMob_speaking	= NULL;
			ch->desc->olc_editing		= NULL;
			ch->desc->connected			= CON_PLAYING;
			return;
		}
		act( COL_ACTION, "$n rozmawia z $N$4.", ch, NULL, ch->pcdata->pMob_speaking, TO_ROOM );
		pNewNode = get_dialog_target( pNode->pBase, pNewNode->target_nr );
		ch->desc->olc_editing	= (void *)pNewNode;
		dialog_show_phase( ch, pNewNode );
		return;
	}

}

