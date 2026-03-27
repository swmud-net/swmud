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
*			     Special boards module			   *
****************************************************************************/

/*
 * Ok. Notki s� totalnie do kitu.
 *
 * Wcze�niej notki identyfikowa�y si� z terminalem do kt�rego zosta�y wys�ane i
 * tylko w nim mo�na by�o je przeczyta�. My niceo 'uglobalnili�my' t�
 * niedogodno�� i wys�anie notki w jednym terminalu automatycznie pozwala na
 * odczytanie ich w innych. Niestety jednak sporo syfu w kodzie zosta�o.
 *
 * Druga sprawa to ten niby dysk, na kt�rym gracz pisze notke. To jest totalnie
 * do niczego. Gracz powinien mie� w pc_data wska�nik na notk�, a przy wysy�aniu
 * notka ta powinna by� wypi�ta z pc_data i wpi�ta do globalnej listy.
 *
 * Dlatego 2 wielkie TODO:
 * - wywali� beznadziejn� obs�ug� terminali (zostawi� tylko typ: terminal), ale
 *   dalej wymaga� obecno�ci terminala w lokacji gdzie czytamy/wysy�amy notk�.
 * - wywali� ten denerwuj�cy dysk i przenie�� ca�o�� do pc_data
 *
 * Thanos
 *
 * Na chuj wywalac dysk, skoro on jest zajebi�cie wygodny? (mo�na mie� kilka kopii
 * roboczych notek)
 *
 * Trog
 */


#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include "mud.h"

/* Defines for voting on notes. -- Narn */
#define VOTE_NONE 	0
#define VOTE_OPEN 	1
#define VOTE_CLOSED 	2

std::list<BOARD_DATA *> board_list;

bool	is_note_to	args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
bool	is_note_sender args( ( CHAR_DATA *ch, NOTE_DATA *pnote ) );
void	note_attach	args( ( CHAR_DATA *ch ) );
void	note_remove	args( ( CHAR_DATA *ch, BOARD_DATA *board,
							NOTE_DATA *pnote ) );
void  	do_note		args( ( CHAR_DATA *ch, char *arg_passed, bool IS_MAIL) );
char * 	string_delete_last_line
					args( ( char * old ) );


//added by Thanos
//funkcja liczy ile gracz ma nieprzeczytanych notek
void note_count( CHAR_DATA *ch )
{
	BOARD_DATA *	board = nullptr;
	int				notes;

	if( !ch->pcdata || NOT_AUTHED( ch ) )
		return;

	for ( auto* b : board_list )
		if ( b->board_obj == 10414 )
		{
			board = b;
			break;
		}

	IF_BUG( board==NULL, "Obj vnum 10414 MUSTS EXIST !!!" )
			return;

	notes = 0;

	for ( auto* pnote : board->notes )
		if( is_note_to( ch, pnote )
		&& !is_note_sender( ch, pnote )
		&& pnote->stamp > ch->pcdata->last_note )
			notes++;

	if( notes )
	{
		if( notes == 1 )
			ch_printf( ch, NL FB_WHITE
					"                             * Masz now� notk�! *" EOL );
		else
			ch_printf( ch, NL FB_WHITE
					"                           * Masz %d now%s not%s! *" EOL,
					notes,
					NUMBER_SUFF( notes, "�", "e", "ych" ),
					NUMBER_SUFF( notes, "k�", "ki", "ek" ) );
	}
	return;
}




bool can_remove( CHAR_DATA *ch, BOARD_DATA *board )
{
	/* If your trust is high enough, you can remove it. */
	if ( get_trust( ch ) >= board->min_remove_level )
		return true;

	if ( board->extra_removers[0] != '\0' )
	{
		if ( is_name( ch->name, board->extra_removers ) )
			return true;
	}

	return false;
}


/*
 * board commands.
 */
void write_boards_txt( )
{
	FILE *fpout;

	fpout = fopen( BOARD_FILE, "w" );
	IF_BUG( !fpout, "FATAL: cannot open board.txt for writing" )
	{
		perror( BOARD_FILE );
		return;
	}

	for ( auto* tboard : board_list )
	{
		fprintf( fpout, "Filename          %s~\n", tboard->note_file	    );
		fprintf( fpout, "Vnum              %d\n",  tboard->board_obj	    );
		fprintf( fpout, "Min_read_level    %d\n",  tboard->min_read_level   );
		fprintf( fpout, "Min_post_level    %d\n",  tboard->min_post_level   );
		fprintf( fpout, "Min_remove_level  %d\n",  tboard->min_remove_level );
		fprintf( fpout, "Max_posts         %d\n",  tboard->max_posts	    );
		fprintf( fpout, "Type 	           %d\n",  tboard->type				);
		fprintf( fpout, "Read_group        %s~\n", tboard->read_group       );
		fprintf( fpout, "Post_group        %s~\n", tboard->post_group       );
		fprintf( fpout, "Extra_readers     %s~\n", tboard->extra_readers    );
		fprintf( fpout, "Extra_removers    %s~\n", tboard->extra_removers    );

		fprintf( fpout, "End\n" );
	}

	fclose( fpout );
}

//added by Thanos (ta funkcja 'podmienia' terminal
//tak, aby zapisane wiadomo�ci 'lecia�y' tylko do jednego pliku
// ../boards/student.board
BOARD_DATA *change_board_to_default()
{
	for ( auto* board : board_list )
    /* !UWAGA! 10414 to vnum terminala w mudszkole!
		Nie ma terminala - nie ma notek!!!  */
		if ( board->board_obj == 10414 )
			return board;

	return NULL;
}

BOARD_DATA *get_board( OBJ_DATA *obj )
{
	for ( auto* board : board_list )
		if ( board->board_obj == obj->pIndexData->vnum )
			return board;
	return NULL;
}

BOARD_DATA *find_board( CHAR_DATA *ch )
{
	BOARD_DATA  *board;

	for ( auto* obj : ch->in_room->contents )
	{
		if ( (board = get_board(obj)) != NULL )
		{
	    //added by Thanos (ta funkcja 'podmienia' wszystkie terminale
	    //na jeden domyslny
	    //tak, aby zapisane wiadomo�ci 'lecia�y' tylko do jednego pliku
			board = change_board_to_default();
			return board;
		}
	}
	return NULL;
}

/*modified by Thanos
  Usprawnilem nieco to i owo tutaj
  teraz mozliwa jest skladnia 'note to':
    note to all -<niechciany_gracz>
    note to immo +<chciany_smiertelnik>
    note to admins
    note to 30 -<gracz_o_levelu_powyzej_30>
    note to clans (wszystkie klany)
    note to clan:1 -- notka do klanu o id =1
    note to builders -- notka do olcman�w
  oraz pozostale
*/
bool is_note_to( CHAR_DATA *ch, NOTE_DATA *pnote )
{
	MEMBER_DATA	*	member;
	char 			name	[MAX_STRING_LENGTH];
	int	 			i;

	if( !pnote )
		return false;

	if( IS_NPC( ch ) )
		return false;

	sprintf( name, "-%s" , ch->name );
	if( is_name( name, pnote->to_list ) )
		return false;

	if( is_name( pnote->sender, ch->pcdata->ignorelist ) )/* ignoranci */
		return false;

	sprintf( name, "+%s" , ch->name );
	if( is_name( name, pnote->to_list ) )
		return true;			    // '-' jest mocniejszy niz '+'.

	if ( is_name( ch->name, pnote->to_list ) )
		return true;

	if ( is_name( "all", pnote->to_list ) )
		return true;


    //notki do admin�w
	if( (is_name( "admins", pnote->to_list ) ||
			is_name( "admin",  pnote->to_list ) ) && IS_ADMIN( ch->name ) )
		return true;

	if( (is_name( "coders", pnote->to_list )
			|| is_name( "koders", pnote->to_list ) ) && IS_CODER( ch->name ) )
		return true;

    //notki do rady sw, Trog
	if( is_name( "council", pnote->to_list ) && IS_COUNCIL( ch->name ) )
		return true;

    //notki do olcman�w
	if( is_name( "builders", pnote->to_list )
		   && (IS_OLCMAN( ch )||get_trust(ch)>102||IS_ADMIN(ch->name) ) )
		return true;

    //notki do klanowicz�w
	if( ch->pcdata && ch->pcdata->clan )
	{
		if( is_name( "clans", pnote->to_list )
				  || is_name( "clan:all", pnote->to_list ) )
			return true;

		for( auto* clan : clan_list )
				if( (member = get_member( clan, ch->name ))
								 &&   member->status > CLAN_WAITING )
		{
			sprintf( name, "clan:%d", clan->clan_id );
			if( is_name( name, pnote->to_list ) )
				return true;
		}
	}

	if ( IS_HERO(ch) && is_name( "immo", pnote->to_list ) )
		return true;

	/* LAMA */
	for( i=0; i<105; i++ )
	{
		sprintf( name, "%d", i );
		if( is_name( name, pnote->to_list ) )
			if( get_trust(ch) >= atoi(name) )
				return true;
	}


	return false;
}

/*
 * Tak, je�li ch jest autorem notki
 */
bool is_note_sender( CHAR_DATA *ch, NOTE_DATA *pnote )
{
	if( *pnote->real_sender )
	{
		if( !str_cmp( ch->name, pnote->real_sender ) )
			return true;

		return false;
	}

	if( !str_cmp( ch->name, pnote->sender ) )
		return true;

	return false;
}

void note_attach( CHAR_DATA *ch )
{
	NOTE_DATA *pnote;

	if ( ch->pnote )
		return;

	CREATE( pnote, NOTE_DATA, 1 );

	STRDUP( pnote->sender , 	ch->name );
	STRDUP( pnote->real_sender, "" );
	STRDUP( pnote->date, 		"" );
	STRDUP( pnote->to_list, 	"" );
	STRDUP( pnote->subject, 	"" );
	STRDUP( pnote->text	  , 	"" );
	ch->pnote		= pnote;

	return;
}

void write_board( BOARD_DATA *board )
{
	FILE *		fp;
	NOTE_DATA *		pnote;
	char 		filename	[256];

    /*
	* Je�li przekroczymy ilo�� notek w terminalu - usu�my 1
	* zapisuj�c j� w old_notes.txt
	*/
	RESERVE_CLOSE;
	if( board->num_posts >= board->max_posts )
	{

		if ( !( fp = fopen( OLDNOTES_FILE, "a" ) ) )
		{
			bug( "Cannot open OLD_NOTES.TXT" );
			RESERVE_OPEN;
			return;
		}

		pnote = board->notes.front();

		if( *pnote->real_sender )
			fprintf( fp,	"Sender      %s~\n"
							"RealSender  %s~\n"
							"BrdFilename %s~\n"
							"Date        %s~\n"
							"To          %s~\n"
							"Subject     %s~\n"
							"Stamp       %ld\n"
							"Voting      %d\n"
							"Yesvotes    %s~\n"
							"Novotes     %s~\n"
							"Abstentions %s~\n"
							"Text\n%s~\n\n",
				pnote->sender,
				pnote->real_sender,
				board->note_file,
				pnote->date,
				pnote->to_list,
				pnote->subject,
				(unsigned long)pnote->stamp,
				pnote->voting,
				pnote->yesvotes,
				pnote->novotes,
				pnote->abstentions,
				pnote->text		);
		else
			fprintf( fp,    "Sender      %s~\n"
							"Date        %s~\n"
							"To          %s~\n"
							"Subject     %s~\n"
							"Stamp       %ld\n"
							"Voting      %d\n"
							"Yesvotes    %s~\n"
							"Novotes     %s~\n"
							"Abstentions %s~\n"
							"Text\n%s~\n\n",
					pnote->sender,
					pnote->date,
					pnote->to_list,
					pnote->subject,
					(unsigned long)pnote->stamp,
					pnote->voting,
					pnote->yesvotes,
					pnote->novotes,
					pnote->abstentions,
					pnote->text	);


		board->notes.remove( pnote );
		--board->num_posts;
		free_note( pnote );
		fclose( fp );
	}

    /*
	* Rewrite entire list.
	*/
	sprintf( filename, "%s%s", BOARD_DIR, board->note_file );
	if ( ( fp = fopen( filename, "w" ) ) == NULL )
	{
		perror( filename );
	}
	else
	{
		for ( auto* pnote : board->notes )
		{
			if( *pnote->real_sender )
				fprintf( fp,    "Sender      %s~\n"
								"RealSender  %s~\n"
								"Date        %s~\n"
								"To          %s~\n"
								"Subject     %s~\n"
								"Stamp       %ld\n"
								"Voting      %d\n"
								"Yesvotes    %s~\n"
								"Novotes     %s~\n"
								"Abstentions %s~\n"
								"Text\n%s~\n\n",
						pnote->sender,
						pnote->real_sender,
						pnote->date,
						pnote->to_list,
						pnote->subject,
						(unsigned long)pnote->stamp,
						pnote->voting,
						pnote->yesvotes,
						pnote->novotes,
						pnote->abstentions,
						pnote->text	);
			else
				fprintf( fp,    "Sender      %s~\n"
								"Date        %s~\n"
								"To          %s~\n"
								"Subject     %s~\n"
								"Stamp       %ld\n"
								"Voting      %d\n"
								"Yesvotes    %s~\n"
								"Novotes     %s~\n"
								"Abstentions %s~\n"
								"Text\n%s~\n\n",
						pnote->sender,
						pnote->date,
						pnote->to_list,
						pnote->subject,
						(unsigned long)pnote->stamp,
						pnote->voting,
						pnote->yesvotes,
						pnote->novotes,
						pnote->abstentions,
						pnote->text	);
		}
		fclose( fp );
	}

	RESERVE_OPEN;
	return;
}

void note_remove( CHAR_DATA *ch, BOARD_DATA *board, NOTE_DATA *pnote )
{

	IF_BUG( board==NULL, "" )
			return;

	IF_BUG( pnote==NULL, "" )
			return;


    /*
	* Remove note from linked list.
	*/
	board->notes.remove( pnote );
	--board->num_posts;

	free_note( pnote );
	write_board( board );
}


DEF_DO_FUN( noteroom )
{
	BOARD_DATA *	board;
	char 			arg			[MAX_STRING_LENGTH];
	char 			arg_passed	[MAX_STRING_LENGTH];

	strcpy(arg_passed, argument);

	argument = one_argument(argument, arg);
	smash_tilde( argument );

	if (!str_prefix(arg, "write") || !str_prefix(arg, "to")
			||  !str_prefix(arg, "subject") || !str_prefix(arg, "show"))
	{
		do_note(ch, arg_passed, false);
		return;
	}

	board = find_board( ch );
	if ( !board )
	{
		send_to_char( "Nie ma tu biuletynu zawieraj�cego notki.\n\r", ch );
		return;
	}

	if (board->type != BOARD_NOTE)
	{
		send_to_char("Tych komend mo�esz u�ywa� jedynie przy terminalu.\n\r", ch);
		return;
	}
	else
	{
		do_note(ch, arg_passed, false);
		return;
	}

}

DEF_DO_FUN( mailroom )
{
	BOARD_DATA *board;
	char arg[MAX_STRING_LENGTH];
	char arg_passed[MAX_STRING_LENGTH];

	strcpy(arg_passed, argument);

	argument = one_argument(argument, arg);
	smash_tilde( argument );
	if (!str_cmp(arg, "write") || !str_cmp(arg, "to")
			||  !str_cmp(arg, "subject") || !str_cmp(arg, "show"))
	{
		do_note(ch, arg_passed, true);
		return;
	}

	board = find_board( ch );
	if ( !board )
	{
		send_to_char( "There is no mail facility here.\n\r", ch );
		return;
	}

	if (board->type != BOARD_MAIL)
	{
		send_to_char("You can only use mail commands in a post office.\n\r", ch);
		return;
	}
	else
	{
		do_note(ch, arg_passed, true);
		return;
	}
//  }
}

void show_note( CHAR_DATA *ch, NOTE_DATA *pnote, int vnum )
{
	if( !ch || !ch->pcdata || !pnote )
		return;

	if( *pnote->real_sender && get_trust( ch ) >= MAX_LEVEL )
	{
		pager_printf( ch, PLAIN
				"[" FB_WHITE "%3d" PLAIN "] %s" PLAIN " (%s): " FB_WHITE "%s" EOL
						"%s"		EOL
						"Do: %s"	EOL
						"%s" 		EOL,
				vnum,
				pnote->sender,
				pnote->real_sender,
				pnote->subject,
				pnote->date,
				pnote->to_list,
				pnote->text );
	}
	else
	{
		pager_printf( ch, PLAIN
			"[" FB_WHITE "%3d" PLAIN "] %s: " FB_WHITE "%s" EOL
					"%s"		EOL
					"Do: %s"	EOL
					"%s" 		EOL,
			vnum,
			pnote->sender,
			pnote->subject,
			pnote->date,
			pnote->to_list,
			pnote->text );
	}

	if( pnote->yesvotes[0] 		!= '\0'
		   ||  pnote->novotes[0] 		!= '\0'
		   ||  pnote->abstentions[0] 	!= '\0' )
	{
		pager_printf( ch,
					  "----------------------------------------------------------" NL
							  "G�osy:" NL
							  "Za:             %s" EOL
							  "Przeciw:        %s" EOL
							  "Wstrzymuje si�: %s" EOL,
					  pnote->yesvotes,
					  pnote->novotes,
					  pnote->abstentions );
	}

	ch->pcdata->last_note = UMAX( ch->pcdata->last_note, pnote->stamp );
	act( COL_ACTION, "$n czyta wiadomo��.", ch, NULL, NULL, TO_ROOM );

	return;
}

NOTE_DATA *get_note( CHAR_DATA *ch, BOARD_DATA *board, int anum )
{
	NOTE_DATA *	pnote = NULL;
	int 		vnum = 0;

	if( !ch || !board || anum < 1 )
		return NULL;

	for ( auto* p : board->notes )
	{
		if( is_note_to( ch, p )
		||  is_note_sender( ch, p ) )
			vnum++;

		if( vnum == anum )
		{
			pnote = p;
			break;
		}
	}

	return pnote;
}

void do_note( CHAR_DATA *ch, char *arg_passed, bool IS_MAIL )
{
	char 				buf		[MAX_STRING_LENGTH];
	char 				arg		[MAX_INPUT_LENGTH];
	NOTE_DATA *			pnote = nullptr;
	BOARD_DATA *		board;
	int 				vnum;
	int 				anum;
	OBJ_DATA *			paper;
	OBJ_DATA *			tmpobj 		= NULL;
	EXTRA_DESCR_DATA *	ed 		= NULL;
	char 				notebuf		[MAX_STRING_LENGTH];
	char 				short_desc_buf	[MAX_STRING_LENGTH];
	char 				long_desc_buf	[MAX_STRING_LENGTH];
	char 				keyword_buf	[MAX_STRING_LENGTH];
	bool 				mfound 		= false;
	bool 				wasfound 	= false;


	if ( IS_NPC(ch) )
		return;

	IF_BUG( !ch->desc, "" )
			return;

	arg_passed = one_argument( arg_passed, arg );
	smash_tilde( arg_passed );

	if ( arg[0] == '\0' )
	{
		do_note( ch, (char *)"read", false );
		return;
	}


	// Thanos; oznacz notki jako przeczytane
	if ( !str_prefix( arg, "catchup" ) )
	{
		board = find_board( ch );
		if ( !board )
		{
			send_to_char( "Nie ma tu tablicy z og�oszeniami, kt�re chcesz przejrze�." NL, ch );
			return;
		}

		ch->pcdata->last_note = UMAX( ch->pcdata->last_note, board->notes.back()->stamp );
		send_to_char( "Ok. Wszystkie notki zosta�y oznaczone jako przeczytane." NL, ch );
		return;
	}

	if ( !str_prefix( arg, "list" ) )
	{
		board = find_board( ch );
		if ( !board )
		{
			send_to_char( "Nie ma tu tablicy z og�oszeniami, kt�re chcesz przejrze�." NL, ch );
			return;
		}

		if (!IS_MAIL)
		{
			vnum = 0;
			for ( auto* pnote : board->notes )
			{
		 		//modified by Thanos (wstawilem ponizszy warunek if( is_note_to... )
	         	//po to by nie bylo widac notek, ktore nie sa do nas lub od nas
				if( is_note_to( ch, pnote )
				||  is_note_sender( ch, pnote ) )
				{
					vnum++;
					pager_printf( ch,
								  "[" FB_BLUE "%s" FB_WHITE "%2d" PLAIN "] %-12s: %s%s" EOL,
								  ( pnote->stamp > ch->pcdata->last_note
										  && !is_note_sender( ch, pnote ) ) ?
										  "N" : " ",
								  vnum,
								  pnote->sender,
								  (pnote->voting != VOTE_NONE) ?
										  (pnote->voting == VOTE_OPEN ?
										  "[V]" : "[C]") : "   ",
								  pnote->subject );
				}
			}

			act( COL_ACTION, "$n ogl�da najnowsze notki.", ch, NULL, NULL, TO_ROOM );
			return;
		}
		else
		{
			vnum = 0;

			if (IS_MAIL) /* SB Mail check for Brit */
			{
				for ( auto* pnote : board->notes )
					if (is_note_to( ch, pnote ))
						mfound = true;

				if ( !mfound )
				{
					ch_printf( ch, "Nie ma dla ciebie wiadomo�ci." NL);
					return;
				}
			}

			for ( auto* pnote : board->notes )
				if (is_note_to( ch, pnote ) )
					ch_printf( ch,
							   "%2d%c %s: %s" NL,
							   ++vnum,
							   is_note_to( ch, pnote ) ?
									   '-' : '>',
							   pnote->sender,
							   pnote->subject );
			return;
		}
	}

	/* to co list, tylko �e od ko�ca --Thanos*/
	if ( !str_prefix( arg, "-list" ) )
	{
		board = find_board( ch );
		if ( !board )
		{
			send_to_char( "Nie ma tu tablicy z og�oszeniami, kt�re chcesz przejrze�.\n\r", ch );
			return;
		}

		/* najpierw musimy policzy� ile jest notek*/
		vnum = 0;
		for ( auto* pnote : board->notes )
			if( is_note_to( ch, pnote ) || is_note_sender( ch, pnote ) )
				vnum++;

		/* a teraz wy�wietli� je od ty�u */
		for ( auto it = board->notes.rbegin(); it != board->notes.rend(); ++it )
		{
			auto* pnote = *it;
			if( is_note_to( ch, pnote ) || is_note_sender( ch, pnote ) )
			{
				pager_printf( ch, "[" FB_BLUE "%s" FB_WHITE "%2d" PLAIN "] %-12s: %s%s" EOL,
							  ( pnote->stamp > ch->pcdata->last_note
									  && !is_note_sender( ch, pnote ) ) ? "N" : " ",
							  vnum,
							  pnote->sender,
							  (pnote->voting != VOTE_NONE) ? (pnote->voting == VOTE_OPEN ? "[V]" : "[C]") : "   ",
							  pnote->subject );
				vnum--;
			}
		}
		act( COL_ACTION, "$n ogl�da najnowsze notki.", ch, NULL, NULL, TO_ROOM );
		return;
	}

	if ( !str_prefix( arg, "read" ) )
	{
		bool fAll;

		board = find_board( ch );
		if ( !board )
		{
			send_to_char( "Nie ma tu tablicy z notkami." NL, ch );
			return;
		}
		if ( !str_cmp( arg_passed, "all" ) )
		{
			fAll = true;
			anum = 0;
		}
		//added by Thanos (auomatyczne wyszukiwanie nieprzeczytanej noty)
		else
		if ( arg_passed[0] == '\0' || !str_cmp( arg_passed, "next" ) )
		{
			vnum    = 0;
			pnote   = nullptr;
			for ( auto* p : board->notes )
			{
				if ( is_note_to( ch, p )
								 && !is_note_sender( ch, p )
								 && ch->pcdata->last_note < p->stamp )
				{
					pnote = p;
					vnum++;
					break;
				}
				else
				{
					if( is_note_to( ch, p )
									   || is_note_sender( ch, p ))
						vnum++;
				}
			}

			if ( pnote )
			{
				show_note( ch, pnote, vnum );
				return;
			}
			send_to_char( "Nie masz nieprzeczytanych notek." NL, ch );
			return;
		}
		//done
		else
			if ( is_number( arg_passed ) )
		{
			fAll = false;
			anum = atoi( arg_passed );
		}
		else
		{
			send_to_char( "Kt�r� notk� chcesz przeczyta�?" NL, ch );
			return;
		}

		if (!IS_MAIL)
		{
			if( (pnote = get_note( ch, board, anum)) != NULL )
				show_note( ch, pnote, anum );
			else
				ch_printf( ch, "Nie ma takiej wiadomo�ci jak %d." NL, anum);
			return;
		}
		else
		{
			vnum = 0;
			for ( auto* pnote : board->notes )
			{
				if (is_note_to(ch, pnote) )
				{
					vnum++;
					if ( vnum == anum || fAll )
					{
						wasfound = true;
						if ( ch->gold < 10 )
						{
							send_to_char("Przeczytanie wiadomo�ci kosztuje 10 kredytek." NL, ch);
							return;
						}

						ch->gold -= 10;
						ch->gold = UMAX( 0, ch->gold );

						pager_printf( ch, "[%3d]%s\n\r%s: %s\n\rDo: %s\n\r%s",
									  vnum,
									  pnote->date,
									  pnote->sender,
									  pnote->subject,
									  pnote->to_list,
									  pnote->text );
					}
				}
			}
			if (!wasfound)
				ch_printf( ch, "Nie ma takiej wiadomo�ci jak %d." NL, anum);
			return;
		}
	}

	/* Voting added by Narn, June '96 */
	if ( !str_prefix( arg, "vote" ) )
	{
		char 		arg2[MAX_INPUT_LENGTH];

		arg_passed = one_argument( arg_passed, arg2 );

		board = find_board( ch );
		if ( !board )
		{
			send_to_char( "Nie ma tu tablicy z notkami.\n\r", ch );
			return;
		}
		if ( is_number( arg2 ) )
			anum = atoi( arg2 );
		else
		{
			send_to_char( "Na kt�rej notce chcesz umie�ci� sw�j g�os?\n\r", ch );
			return;
		}

		vnum = 1;
		if ( (pnote=get_note( ch, board, anum)) == NULL )
		{
			send_to_char( "Nie ma takiej notki.\n\r", ch );
			return;
		}

		/* Options: open close yes no abstain */
		/* If you're the author of the note and can read the board you can open
		and close voting, if you can read it and voting is open you can vote.
		*/
		if ( !str_cmp( arg_passed, "open" ) )
		{
			if ( !is_note_sender( ch, pnote ) )
			{
				send_to_char( "Nie jeste� autorem tej notki.\n\r", ch );
				return;
			}
			pnote->voting = VOTE_OPEN;
			act( COL_ACTION, "$n otwiera g�osowanie na temat tre�ci notki.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "G�osowanie otwarte.\n\r", ch );
			write_board( board );
			return;
		}
		if ( !str_cmp( arg_passed, "close" ) )
		{
			if ( !is_note_sender( ch, pnote ) )
			{
				send_to_char( "Nie jeste� autorem tej notki.\n\r", ch );
				return;
			}
			pnote->voting = VOTE_CLOSED;
			act( COL_ACTION, "$n zamyka g�osowanie na temat tre�ci notki.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "G�osowanie zamkni�te.\n\r", ch );
			write_board( board );
			return;
		}

		/* Make sure the note is open for voting before going on. */
		if ( pnote->voting != VOTE_OPEN )
		{
			send_to_char( "Na temat tre�ci tej notki nie mo�na g�osowa�.\n\r", ch );
			return;
		}

		/* Can only vote once on a note. */
		sprintf( buf, "%s %s %s",
				 pnote->yesvotes, pnote->novotes, pnote->abstentions );

		if ( is_name( ch->name, buf ) )
		{
			ch_printf( ch, "Ju� g�osowa�%s� na temat tre�ci tej notki." NL,
					   SEX_SUFFIX_EAE( ch ) );
			return;
		}

		if ( !str_prefix( arg_passed, "yes" ) || !str_prefix( arg_passed, "za" )
					|| !str_prefix( arg_passed, "tak" )
		   )
		{
			sprintf( buf, "%s %s", pnote->yesvotes, ch->name );
			STRDUP( pnote->yesvotes, buf );
			act( COL_ACTION, "$n oddaje sw�j g�os w notce.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Ok.\n\r", ch );
			write_board( board );
			return;
		}

		if ( !str_prefix( arg_passed, "no" ) || !str_prefix( arg_passed, "przeciw" )
					|| !str_prefix( arg_passed, "nie" ) )
		{
			sprintf( buf, "%s %s", pnote->novotes, ch->name );
			STRDUP( pnote->novotes, buf );
			act( COL_ACTION, "$n oddaje sw�j g�os w notce.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Ok.\n\r", ch );
			write_board( board );
			return;
		}

		if( !str_prefix( arg_passed, "abstain" )
				   ||  !str_prefix( arg_passed, "wstrzymaj" )
				   ||  !str_prefix( arg_passed, "wstrzymuj� si�" ))
		{
			sprintf( buf, "%s %s", pnote->abstentions, ch->name );
			STRDUP( pnote->abstentions, buf );
			act( COL_ACTION, "$n oddaje sw�j g�os w notce.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Ok.\n\r", ch );
			write_board( board );
			return;
		}

		do_note( ch, (char*)"", false );
	}

	if ( !str_prefix( arg, "write" ) )
	{
		if ( ch->substate == SUB_RESTRICTED )
		{
			send_to_char( "Nie mo�esz pisa� notki, b�d�c w innym edytorze.\n\r", ch );
			return;
		}

		if ( ( paper = get_eq_char(ch, WEAR_HOLD) ) == NULL
					 ||     paper->item_type != ITEM_PAPER )
		{
			paper = create_object( get_obj_index(OBJ_VNUM_NOTE), 0 );
			if ((tmpobj = get_eq_char(ch, WEAR_HOLD)) != NULL)
				unequip_char(ch, tmpobj);
			paper = obj_to_char(paper, ch);
			equip_char(ch, paper, WEAR_HOLD);
			act(PLAIN, "$n zapisuje wiadomo��.",
				ch, NULL, NULL, TO_ROOM);
			act(PLAIN, "Zapisujesz wiadomo��.",
				ch, NULL, NULL, TO_CHAR);
		}
		paper->value[0] = 1;
		ed = SetOExtra(paper, (char *)"_text_");
		SET_BIT( ch->act, PLR_NOTE_WRITE );
		string_append( ch, &ed->description );
		return;
	}

	if ( !str_prefix( arg, "as" )
			 && ( get_trust( ch ) >= MAX_LEVEL || BESTOW( ch, "noteas" ) ) )
	{
		if (!arg_passed || arg_passed[0] == '\0')
		{
			send_to_char("Jako kto chcesz wys�a� t� wiadomo��?" NL, ch);
			return;
		}

		if ( ( paper = get_eq_char(ch, WEAR_HOLD) ) == NULL
					 ||     paper->item_type != ITEM_PAPER )
		{
			paper = create_object( get_obj_index(OBJ_VNUM_NOTE), 0 );
			if ((tmpobj = get_eq_char(ch, WEAR_HOLD)) != NULL)
				unequip_char(ch, tmpobj);
			paper = obj_to_char(paper, ch);
			equip_char(ch, paper, WEAR_HOLD);
		}

		ed = SetOExtra(paper, (char *)"_sender_");
		STRDUP( ed->description, arg_passed );
		send_to_char("Ok." NL, ch);
		return;
	}

	if ( !str_prefix( arg, "subject" ) )
	{
		if (!arg_passed || arg_passed[0] == '\0')
		{
			send_to_char("Jaki ma by� temat wiadomo�ci?\n\r", ch);
			return;
		}
		if ( ( paper = get_eq_char(ch, WEAR_HOLD) ) == NULL
					 ||     paper->item_type != ITEM_PAPER )
		{
			paper = create_object( get_obj_index(OBJ_VNUM_NOTE), 0 );
			if ((tmpobj = get_eq_char(ch, WEAR_HOLD)) != NULL)
				unequip_char(ch, tmpobj);
			paper = obj_to_char(paper, ch);
			equip_char(ch, paper, WEAR_HOLD);
			act(PLAIN, "$n zapisuje temat wiadomo�ci na dysku.",
				ch, NULL, NULL, TO_ROOM);
			act(PLAIN, "Zapisujesz temat wiadomo�ci.",
				ch, NULL, NULL, TO_CHAR);
		}
		if (paper->value[1] > 1 )
		{
			send_to_char("Nie mo�esz modyfikowa� tej wiadomo�ci.\n\r", ch);
			return;
		}
		else
		{
			paper->value[1] = 1;
			ed = SetOExtra(paper, (char *)"_subject_");
			STRDUP( ed->description, arg_passed );
			send_to_char("Ok.\n\r", ch);
			return;
		}
	}

	if ( !str_cmp( arg, "+" ) )
	{
		char	buf	[MSL];

		if ( ( paper = get_eq_char(ch, WEAR_HOLD) ) == NULL
					 ||     paper->item_type != ITEM_PAPER )
		{
			paper = create_object( get_obj_index(OBJ_VNUM_NOTE), 0 );
			if ((tmpobj = get_eq_char(ch, WEAR_HOLD)) != NULL)
				unequip_char(ch, tmpobj);
			paper = obj_to_char(paper, ch);
			equip_char(ch, paper, WEAR_HOLD);
		}

		paper->value[0] = 1;
		ed 		= SetOExtra(paper, (char *)"_text_");
		strcpy( buf, ed->description );
		strcat( buf, arg_passed );
		strcat( buf, NL );
		STRDUP( ed->description, buf );
		send_to_char("Ok. Linia dodana do noty." NL, ch);
		return;
	}
/*
	if ( !str_cmp( arg, "-" ) )
	{
	if ( ( paper = get_eq_char(ch, WEAR_HOLD) ) == NULL
	||     paper->item_type != ITEM_PAPER )
	{
	paper = create_object( get_obj_index(OBJ_VNUM_NOTE), 0 );
	if ((tmpobj = get_eq_char(ch, WEAR_HOLD)) != NULL)
	unequip_char(ch, tmpobj);
	paper = obj_to_char(paper, ch);
	equip_char(ch, paper, WEAR_HOLD);
}

	paper->value[0] = 1;
	ed 		= SetOExtra(paper, "_text_");
	ed->description = string_delete_last_line( ed->description );
	send_to_char("Ok. Ostatnia linia usuni�ta z noty." NL, ch);
	return;
}
*/

	if ( !str_prefix( arg, "to" ) )
	{
		struct stat fst;
		char fname[1024];

		if (!arg_passed || arg_passed[0] == '\0')
		{
			send_to_char("Sprecyzuj adresata.\n\r", ch);
			return;
		}
		if ( ( paper = get_eq_char(ch, WEAR_HOLD) ) == NULL
					 ||     paper->item_type != ITEM_PAPER )
		{
			paper = create_object( get_obj_index(OBJ_VNUM_NOTE), 0 );
			if ((tmpobj = get_eq_char(ch, WEAR_HOLD)) != NULL)
				unequip_char(ch, tmpobj);
			paper = obj_to_char(paper, ch);
			equip_char(ch, paper, WEAR_HOLD);
			act(PLAIN, "$n zapisuje wiadomo�� na dysku.",
				ch, NULL, NULL, TO_ROOM);
			act(PLAIN, "Zapisujesz adresata notki na dysku.",
				ch, NULL, NULL, TO_CHAR);
		}

		if (paper->value[2] > 1)
		{
			send_to_char("Nie mo�esz modyfikowa� tej wiadomo�ci.\n\r",ch);
			return;
		}

		arg_passed[0] = UPPER(arg_passed[0]);

		sprintf( fname, "%s%s", PDATA_DIR, capitalize( arg_passed ) );

		if ( !IS_MAIL || stat( fname, &fst ) != -1 || !str_cmp(arg_passed, "all") )
		{
			paper->value[2] = 1;
			ed = SetOExtra(paper, (char *)"_to_");
			STRDUP( ed->description, arg_passed );
			send_to_char("Ok.\n\r",ch);
			return;
		}
		else
		{
			send_to_char("Gracz o takim imieniu nie istnieje.\n\r",ch);
			return;
		}

	}

	if ( !str_prefix( arg, "show" ) )
	{
		const char *subject, *to_list, *text, *sender;


		if ( ( paper = get_eq_char(ch, WEAR_HOLD) ) == NULL
					 ||     paper->item_type != ITEM_PAPER )
		{
			send_to_char("Nie masz w r�ku dysku.\n\r", ch);
			return;
		}

		sender = get_extra_descr( "_sender_", paper->extradesc );

		if ( (subject = get_extra_descr( "_subject_", paper->extradesc )) == NULL )
			subject = "(brak tematu)";
		if ( (to_list = get_extra_descr( "_to_", paper->extradesc )) == NULL )
			to_list = "(nikogo)";
		sprintf( buf,
				 "%s: %s" EOL
				 "Do: %s" NL,
				 sender ? sender : ch->name,
				 subject,
				 to_list );
		send_to_pager( buf, ch );
		if ( (text = get_extra_descr( "_text_", paper->extradesc )) == NULL )
			text = "Dysk jest czysty.\n\r";
		send_to_pager( text, ch );
		return;
	}

	if ( !str_prefix( arg, "post" ) )
	{
		char *strtime, *text;

		if ( ( paper = get_eq_char(ch, WEAR_HOLD) ) == NULL
					 ||     paper->item_type != ITEM_PAPER )
		{
			send_to_char("Nie masz w r�ku �adnego dysku.\n\r", ch);
			return;
		}

		if ( paper->value[0] == 0 )
		{
			send_to_char("Tw�j dysk jest pusty, nie ma co wysy�a�.\n\r", ch);
			return;
		}

		if ( paper->value[1] == 0 )
		{
			send_to_char("Notka nie mia�a tematu. Podstawiam 'Bez tematu'.\n\r", ch);
			paper->value[1] = 1;
			ed = SetOExtra(paper, (char *)"_subject_");
			STRDUP( ed->description, "Bez tematu" );
		}

		if (paper->value[2] == 0)
		{
			send_to_char("Musisz poda� adresata notki.\n\rNote to <osoba> lub Note to all. W og�le polecam 'help note'\n\r", ch);
			return;
		}

		board = find_board( ch );
		if ( !board )
		{
			send_to_char( "Nie ma tu terminala, z kt�rego mo�na wys�a� wiadomo��.\n\r", ch );
			return;
		}

		if ( board->num_posts >= board->max_posts )
		{
			send_to_char( "Ten terminal jest perzepe�niony. Nie ma miejsca na wiadomo�ci.\n\r", ch );
			return;
		}

		act( COL_ACTION, "$n �aduje wiadomo�� do terminala.", ch, NULL, NULL, TO_ROOM );

		strtime				= ctime( &current_time );
		strtime[strlen(strtime)-1]	= '\0';
		CREATE( pnote, NOTE_DATA, 1 );
		STRDUP( pnote->date, strtime );
		//added by Thanos
		pnote->stamp                    = current_time;

		text = get_extra_descr( "_text_", paper->extradesc );
		STRDUP( pnote->text, text ? text : "" );
		text = get_extra_descr( "_to_", paper->extradesc );
		STRDUP( pnote->to_list, text ? text : "all" );
		text = get_extra_descr( "_subject_", paper->extradesc );
		STRDUP( pnote->subject, text ? text : "" );

		text = get_extra_descr( "_sender_", paper->extradesc );
		if( text )
		{
			STRDUP( pnote->sender, text );
			STRDUP( pnote->real_sender, ch->name );
		}
		else
		{
			STRDUP( pnote->sender, ch->name );
			STRDUP( pnote->real_sender, "" );
		}

		pnote->voting      	= 0;
		STRDUP( pnote->yesvotes   , "" );
		STRDUP( pnote->novotes    , "" );
		STRDUP( pnote->abstentions, "" );

		board->notes.push_back( pnote );
		board->num_posts++;
		write_board( board );
		send_to_char( "�adujesz wiadomo�� do terminala.\n\r", ch );
		extract_obj( paper );
		return;
	}

	if ( !str_cmp( arg, "remove" )
			 ||   !str_cmp( arg, "take" )
			 ||   !str_cmp( arg, "copy" ) )
	{
		char take;

		board = find_board( ch );
		if ( !board )
		{
			send_to_char( "Nie ma tu terminala do �ci�gania wiadomo�ci!\n\r", ch );
			return;
		}

		if ( !str_cmp( arg, "take" ) )
			take = 1;
		else if ( !str_cmp( arg, "copy" ) )
		{
			if ( !IS_IMMORTAL(ch) )
			{
				send_to_char( "Co?  Wpisz 'help note' by dowiedzie� si� czego� o notkach.\n\r", ch );
				return;
			}
			take = 2;
		}
		else
			take = 0;

		if ( !is_number( arg_passed ) )
		{
			send_to_char( "Kt�r� notk� usun��?\n\r", ch );
			return;
		}

		anum = atoi( arg_passed );
		vnum = 0;
		if( (pnote = get_note( ch, board, anum )) != NULL )
		{
			if ( !is_note_sender( ch, pnote )
			&&  !IS_ADMIN( ch->name ) && take != 2 )
			{
				send_to_char( "Nie jeste� autorem tej notki!" NL, ch );
				return;
			}

			if ( take != 0 )
			{
				if ( ch->gold < 50 )
				{
					if ( take == 1 )
						send_to_char("Wzi�cie wiadomo�ci kosztuje 50 kredytek.\n\r", ch);
					else
						send_to_char("Skopiowanie wiadomo�ci kosztuje 50 kredytek.\n\r", ch);
					return;
				}

				if ( get_trust(ch) )
					ch->gold -= 50;

				paper = create_object( get_obj_index(OBJ_VNUM_NOTE), 0 );


				ed = SetOExtra( paper, "_sender_" );
				STRDUP( ed->description, pnote->sender);


				ed = SetOExtra( paper, "_text_" );
				STRDUP( ed->description, pnote->text);


				ed = SetOExtra( paper, "_to_" );
				STRDUP( ed->description, pnote->to_list );


				ed = SetOExtra( paper, "_subject_" );
				STRDUP( ed->description, pnote->subject );


				ed = SetOExtra( paper, "_date_" );
				STRDUP( ed->description, pnote->date );



				ed = SetOExtra( paper, "notka" );

				sprintf(notebuf, "Od: " 		);
				strcat( notebuf, pnote->sender	);
				strcat( notebuf, "          "	);
				strcat( notebuf, pnote->date	);
				strcat( notebuf, "\n\rDo: "		);
				strcat( notebuf, pnote->to_list	);
				strcat( notebuf, "\n\rTemat: "	);
				strcat( notebuf, pnote->subject	);
				strcat( notebuf, "\n\r\n\r"		);
				strcat( notebuf, pnote->text	);
				strcat( notebuf, "\n\r"		);

				STRDUP( ed->description, notebuf);




				paper->value[0] = 2;
				paper->value[1] = 2;
				paper->value[2] = 2;

				sprintf(short_desc_buf, "notka: %s do %s",
						pnote->sender, pnote->to_list);
				STRDUP( paper->przypadki[0], short_desc_buf);

				sprintf(long_desc_buf, "Notka: %s do %s le�y na ziemi.",
						pnote->sender, pnote->to_list);
				STRDUP( paper->description, long_desc_buf);

				sprintf(keyword_buf, "nota notka papier" );
				STRDUP( paper->name, keyword_buf);

			}

			if ( take != 2 )
				note_remove( ch, board, pnote );

			send_to_char( "Ok.\n\r", ch );

			if ( take == 1 )
			{
				act( COL_ACTION, "$n �ci�ga wiadomo��.", ch, NULL, NULL, TO_ROOM );
				obj_to_char(paper, ch);
			}
			else if ( take == 2 )
			{
				act( COL_ACTION, "$n kopiuje wiadomo��.", ch, NULL, NULL, TO_ROOM );
				obj_to_char(paper, ch);
			}
			else
				act( COL_ACTION, "$n usuwa wiadomo��.", ch, NULL, NULL, TO_ROOM );

			return;
		}

		send_to_char( "Nie ma takiej wiadomo�ci.\n\r", ch );
		return;
	}

	send_to_char( "Co?  Wpisz 'help note' by dowiedzie� si� czego� o notkach.\n\r", ch );
	return;
}



BOARD_DATA *read_board( const char *boardfile, FILE *fp )
{
	BOARD_DATA *board;
	char *word;
	bool fMatch;
	char letter;

	do
	{
		letter = getc( fp );
		if ( feof(fp) )
		{
			fclose( fp );
			return NULL;
		}
	}
	while ( isspace(letter) );
	ungetc( letter, fp );

	CREATE( board, BOARD_DATA, 1 );

#ifdef KEY
#undef KEY
#endif
#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
{					\
				    field  = value;			\
				    fMatch = true;			\
				    break;				\
}


	for( ; ; )
	{
		word   = feof( fp ) ? (char *)"End" : fread_word( fp );
		fMatch = false;

		switch ( UPPER(word[0]) )
		{
			case '*':
				fMatch = true;
				fread_to_eol( fp );
				break;
			case 'E':
				SKEY( "Extra_readers",	board->extra_readers,	fread_string( fp ) );
				SKEY( "Extra_removers",       board->extra_removers,   fread_string( fp ) );
				if ( !str_cmp( word, "End" ) )
				{
					board->num_posts	= 0;
					if ( !board->read_group )
						STRDUP( board->read_group, "" );
					if ( !board->post_group )
						STRDUP( board->post_group, "" );
					if ( !board->extra_readers )
						STRDUP( board->extra_readers, "" );
					if ( !board->extra_removers )
						STRDUP( board->extra_removers, "" );
					return board;
				}
			case 'F':
				SKEY( "Filename",	board->note_file,	fread_string( fp ) );
			case 'M':
				KEY( "Min_read_level",	board->min_read_level,	fread_number( fp ) );
				KEY( "Min_post_level",	board->min_post_level,	fread_number( fp ) );
				KEY( "Min_remove_level",	board->min_remove_level,fread_number( fp ) );
				KEY( "Max_posts",		board->max_posts,	fread_number( fp ) );
			case 'P':
				SKEY( "Post_group",	board->post_group,	fread_string( fp ) );
			case 'R':
				SKEY( "Read_group",	board->read_group,	fread_string( fp ) );
			case 'T':
				KEY( "Type",	board->type,		fread_number( fp ) );
			case 'V':
				KEY( "Vnum",	board->board_obj,	fread_number( fp ) );
		}

		if ( !fMatch )
		{
			bug( "no match: %s", word );
		}
	}

  return board;
}

NOTE_DATA *read_note( char *notefile, FILE *fp )
{
	NOTE_DATA *pnote;
	char *word = 0;

	for ( ; ; )
	{
		char letter;

		do
		{
			letter = getc( fp );
			if ( feof(fp) )
			{
				fclose( fp );
				return NULL;
			}
		}
		while ( isspace(letter) );

		ungetc( letter, fp );

		CREATE( pnote, NOTE_DATA, 1 );

		if ( str_cmp( fread_word( fp ), "sender" ) )
			break;
		pnote->sender	= fread_string( fp );

		// Thanos: odczyt prawdziwego nadawcy notki
		word = fread_word( fp );
		if ( !str_cmp( word, "realsender" ) )
		{
			pnote->real_sender	= fread_string( fp );
			word = fread_word( fp );
		}

		if ( str_cmp( word, "date" ) )
			break;
		pnote->date	= fread_string( fp );

		if ( str_cmp( fread_word( fp ), "to" ) )
			break;
		pnote->to_list	= fread_string( fp );

		if ( str_cmp( fread_word( fp ), "subject" ) )
			break;
		pnote->subject	= fread_string( fp );

		word = fread_word( fp );

		//added by Thanos
		if ( !str_cmp( word, "stamp" ) )
		{
			pnote->stamp		= fread_number( fp );
			word = fread_word( fp );
		}
		else
			pnote->stamp                = current_time;
		//done

		if ( !str_cmp( word, "voting" ) )
		{
			pnote->voting = fread_number( fp );

			if ( str_cmp( fread_word( fp ), "yesvotes" ) )
				break;
			pnote->yesvotes	= fread_string( fp );

			if ( str_cmp( fread_word( fp ), "novotes" ) )
				break;
			pnote->novotes	= fread_string( fp );

			if ( str_cmp( fread_word( fp ), "abstentions" ) )
				break;
			pnote->abstentions	= fread_string( fp );

			word = fread_word( fp );
		}

		if ( str_cmp( word, "text" ) )
			break;
		pnote->text	= fread_string( fp );

		if ( !pnote->yesvotes )    STRDUP( pnote->yesvotes   , "" );
		if ( !pnote->novotes )     STRDUP( pnote->novotes    , "" );
		if ( !pnote->abstentions ) STRDUP( pnote->abstentions, "" );
		if ( !pnote->real_sender ) STRDUP( pnote->real_sender, "" );
		return pnote;
	}

	bug( "%s", word );
	bug( "bad key word." );
	exit( 1 );
}

/*
 * Load boards file.
 */
void load_boards( void )
{
	FILE *		board_fp;
	FILE *		note_fp;
	BOARD_DATA *	board;
	NOTE_DATA *		pnote;
	char		notefile	[256];

	board_list.clear();

	if ( ( board_fp = fopen( BOARD_FILE, "r" ) ) == NULL )
		return;

	while ( (board = read_board( BOARD_FILE, board_fp )) != NULL )
	{
		board_list.push_back( board );
		sprintf( notefile, "%s%s", BOARD_DIR, board->note_file );
		if( !sysdata.silent )
			log_string( notefile );
		if ( ( note_fp = fopen( notefile, "r" ) ) != NULL )
		{
			while ( (pnote = read_note( notefile, note_fp )) != NULL )
			{
				board->notes.push_back( pnote );
				board->num_posts++;
			}
		}
	}
	return;
}


DEF_DO_FUN( makeboard )
{
	BOARD_DATA *board;

	if ( !argument || argument[0] == '\0' )
	{
		send_to_char( "Usage: makeboard <filename>\n\r", ch );
		return;
	}

	smash_tilde( argument );

	CREATE( board, BOARD_DATA, 1 );

	board_list.push_back( board );
	STRDUP( board->note_file	 , strlower( argument ) );
	STRDUP( board->read_group    , "" );
	STRDUP( board->post_group    , "" );
	STRDUP( board->extra_readers , "" );
	STRDUP( board->extra_removers, "" );
}

DEF_DO_FUN( bset )
{
	BOARD_DATA *board = nullptr;
	bool found;
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int value;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if ( arg1[0] == '\0' || arg2[0] == '\0' )
	{
		send_to_char( "Usage: bset <board filename> <field> value\n\r", ch );
		send_to_char( "\n\rField being one of:\n\r", ch );
		send_to_char( "  vnum read post remove maxpost filename type\n\r", ch );
		send_to_char( "  read_group post_group extra_readers extra_removers\n\r", ch );
		return;
	}

	value = atoi( argument );
	found = false;
	for ( auto* b : board_list )
		if ( !str_cmp( arg1, b->note_file ) )
	{
		board = b;
		found = true;
		break;
	}
	if ( !found )
	{
		send_to_char( "Board not found.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "vnum" ) )
	{
		if ( !get_obj_index(value) )
		{
			send_to_char( "No such object.\n\r", ch );
			return;
		}
		board->board_obj = value;
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "read" ) )
	{
		if ( value < 0 || value > MAX_LEVEL )
		{
			send_to_char( "Value out of range.\n\r", ch );
			return;
		}
		board->min_read_level = value;
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "read_group" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "No group specified.\n\r", ch );
			return;
		}
		if ( !str_cmp( argument, "none" ) )
			STRDUP( board->read_group, "" );
		else
			STRDUP( board->read_group, argument );
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "post_group" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "No group specified.\n\r", ch );
			return;
		}
		if ( !str_cmp( argument, "none" ) )
			STRDUP( board->post_group, "" );
		else
			STRDUP( board->post_group, argument );
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "extra_removers" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "No names specified.\n\r", ch );
			return;
		}
		if ( !str_cmp( argument, "none" ) )
			buf[0] = '\0';
		else
			snprintf( buf, MAX_STRING_LENGTH, "%s %s", board->extra_removers, argument );
		STRDUP( board->extra_removers, buf );
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "extra_readers" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "No names specified.\n\r", ch );
			return;
		}
		if ( !str_cmp( argument, "none" ) )
			buf[0] = '\0';
		else
			snprintf( buf, MAX_STRING_LENGTH, "%s %s", board->extra_readers, argument );
		STRDUP( board->extra_readers, buf );
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "filename" ) )
	{
		if ( !argument || argument[0] == '\0' )
		{
			send_to_char( "No filename specified.\n\r", ch );
			return;
		}
		STRDUP( board->note_file, argument );
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "post" ) )
	{
		if ( value < 0 || value > MAX_LEVEL )
		{
			send_to_char( "Value out of range.\n\r", ch );
			return;
		}
		board->min_post_level = value;
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "remove" ) )
	{
		if ( value < 0 || value > MAX_LEVEL )
		{
			send_to_char( "Value out of range.\n\r", ch );
			return;
		}
		board->min_remove_level = value;
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	if ( !str_cmp( arg2, "maxpost" ) )
	{
		if ( value < 1 || value > 1000 )
		{
			send_to_char( "Value out of range.\n\r", ch );
			return;
		}
		board->max_posts = value;
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}
	if ( !str_cmp( arg2, "type" ) )
	{
		if ( value < 0 || value > 1 )
		{
			send_to_char( "Value out of range.\n\r", ch );
			return;
		}
		board->type = value;
		write_boards_txt( );
		send_to_char( "Done.\n\r", ch );
		return;
	}

	do_bset( ch, (char *)"" );
	return;
}


DEF_DO_FUN( bstat )
{
	BOARD_DATA *board = nullptr;
	bool found;
	char arg[MAX_INPUT_LENGTH];

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Usage: bstat <board filename>\n\r", ch );
		return;
	}

	found = false;
	for ( auto* b : board_list )
		if ( !str_cmp( arg, b->note_file ) )
	{
		board = b;
		found = true;
		break;
	}
	if ( !found )
	{
		send_to_char( "Board not found.\n\r", ch );
		return;
	}

	ch_printf( ch, "%-12s Vnum: %5d Read: %2d Post: %2d Rmv: %2d Max: %2d Posts: %d Type: %d\n\r",
			   board->note_file,	 board->board_obj,
			   board->min_read_level,	 board->min_post_level,
			   board->min_remove_level, board->max_posts,
			   board->num_posts, board->type );

	ch_printf( ch, "Read_group: %-15s Post_group: %-15s \n\rExtra_readers: %-10s\n\r",
			   board->read_group, board->post_group, board->extra_readers );
	return;
}


DEF_DO_FUN( boards )
{
	if ( board_list.empty() )
	{
		send_to_char( "There are no boards.\n\r", ch );
		return;
	}

	for ( auto* board : board_list )
		ch_printf( ch, "%-16s Vnum: %5d Read: %2d Post: %2d Rmv: %2d Max: %2d Posts: %d Type: %d\n\r",
				   board->note_file,	 board->board_obj,
				   board->min_read_level,	 board->min_post_level,
				   board->min_remove_level, board->max_posts, board->num_posts,
				   board->type);
}

void mail_count(CHAR_DATA *ch)
{
	int cnt = 0;

	for ( auto* board : board_list )
		if ( board->type == BOARD_MAIL )
			for ( auto* note : board->notes )
				if ( is_note_to(ch, note) )
					++cnt;
	if ( cnt )
		ch_printf(ch, "You have %d mail messages waiting.\n\r", cnt);
	return;
}

/*
 * Added by Thanos: a oto moja implementacja wersja note,
 * pomyslu pozwalajacego na pisanie notek graczom, mobom, a nawet mudowi.
 * Wiem ze KD ma taki system (np notki od ClanMastera) i chyba ROM
 */
void note( const char *name, const char *to, const char *subject, const char *text )
{
	NOTE_DATA *		pnote;
	BOARD_DATA *	board;
	char *			time;

	CREATE( pnote, NOTE_DATA, 1 	);

	STRDUP( pnote->sender , name	);

	STRDUP( pnote->real_sender, "" );

	STRDUP( pnote->to_list, to 		);
	STRDUP( pnote->subject, subject 	);
	STRDUP( pnote->text	  , text 	);
	time		= ctime( &current_time );
	time[strlen(time)-1] = '\0';
	STRDUP( pnote->date, time );
	pnote->stamp	= current_time;
	pnote->voting      	= 0;
	STRDUP( pnote->yesvotes   , "" );
	STRDUP( pnote->novotes    , "" );
	STRDUP( pnote->abstentions, "" );

	board = change_board_to_default( );

	board->notes.push_back( pnote );
	board->num_posts++;
	write_board( board );

	return;
}
