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
*			 Typos, ideas and bugs module	by Thanos	   *
****************************************************************************/

#include <time.h>
#include "mud.h"

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
extern FILE *			fpArea;

COMPLAIN_DATA *get_complain( int type, int nr )
{
    int			i = 1;

    for( auto* comp : complain_list )
    {
	if( comp->type != type )
	    continue;
	if( i++ == nr )
	    return comp;
    }
    return NULL;
}


DEF_DO_FUN( fix )
{
    char 		arg1	[MAX_INPUT_LENGTH];
    COMPLAIN_DATA *	comp;
    char *		strtime;
    int			type;
    int			nr;

    argument = one_argument( argument, arg1 );
    if( !str_cmp( arg1, "bug" ) || !str_cmp( arg1, "pbug" ) )
	type = complain_data::COMPLAIN_BUG;
    else
    if( !str_cmp( arg1, "idea" ) || !str_cmp( arg1, "pidea" ) )
	type = complain_data::COMPLAIN_IDEA;
    else
    if( !str_cmp( arg1, "typo" ) || !str_cmp( arg1, "ptypo" ) )
	type = complain_data::COMPLAIN_TYPO;
    else
    {
	ch_printf( ch, "Syntax: fix <bug|idea|typo> <nr>" NL );
	return;
    }

    if( !*argument || !is_number(argument) )
    {
	do_fix( ch, (char *)"" );
	return;
    }

    nr = atoi( argument );
    if( (comp=get_complain( type, nr )) == NULL )
    {
	ch_printf( ch, "No such %s." NL, arg1 );
	return;
    }

    strtime                    = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';

    STRDUP( comp->fixedby, ch->name );
    STRDUP( comp->fixdate, strtime );

    // zapisujemy plik <arg1>.fixed
    save_complains( true );
    // usuwamy <arg1>
    complain_list.remove( comp );
    free_complain( comp );
    // zapisujemy nowy stan <arg1>
    save_complains( false );
    ch_printf( ch, "%s marked as fixed." NL, capitalize( arg1 ) );
    return;
}

void clear_complain( COMPLAIN_DATA *comp )
{
    STRDUP( comp->author, "" );
    STRDUP( comp->text, "" );
    STRDUP( comp->fixedby, "" );
    STRDUP( comp->fixdate, "" );
    STRDUP( comp->date, "" );
}

COMPLAIN_DATA *fread_complain( FILE *fp )
{
    COMPLAIN_DATA *	comp;
    char *		word;
    bool 		fMatch;

    CREATE( comp, COMPLAIN_DATA, 1 );
    clear_complain( comp );

    for ( ; ; )
    {
	word   = feof( fp ) ? (char *)"End" : fread_word( fp );
	fMatch = false;

	switch ( UPPER(word[0]) )
	{
	    case '*':
		fMatch = true;
		fread_to_eol( fp );
		break;

	    case 'A':
		SKEY( "Author",	comp->author, fread_string( fp ) );
		break;

	    case 'D':
		SKEY( "Date",	comp->date, fread_string( fp ) );
		break;

	    case 'E':
		if ( !str_cmp( word, "End" ) )
		    return comp;
		break;

	    case 'F':
		SKEY( "FixedBy", comp->fixedby, fread_string( fp ) );
		SKEY( "Fixdate", comp->fixdate, fread_string( fp ) );
		break;

	    case 'R':
		KEY( "Room",	comp->location, fread_number( fp ) );
		break;

	    case 'T':
		SKEY( "Text",	comp->text, fread_string( fp ) );
		KEY( "Type",	comp->type, (complain_data::Type)fread_number( fp ) );
		break;
	}
	if ( !fMatch )
	{
            bug( "no match: %s", word );
	}
    }
}
void fread_complains(const char *filename)
{
//    FILE *	fp;

    RESERVE_CLOSE;
    if( (fpArea = fopen( filename, "r" )) == NULL )
    {
	perror( filename );
	RESERVE_OPEN;
    	return;
    }

    for( ; ; )
    {
	char letter;
	char *word;

	letter = fread_letter( fpArea );
        if ( letter == '*' )
        {
    	    fread_to_eol( fpArea );
	    continue;
	}

        if ( letter != '#' )
        {
	    bug( "# not found (%s).", filename );
	    break;
	}

	word = fread_word( fpArea );
	if ( !str_cmp( word, "END" ) )
	    break;
	else
        if ( !str_cmp( word, "COMPLAIN " ) )
	{
	    COMPLAIN_DATA *	comp;
	    comp = fread_complain( fpArea );
	    complain_list.push_back( comp );
	}
	else
	{
            bug( "bad section. (%s)", filename );
	    continue;
	}
    }
    fclose( fpArea );
// by Ratm bugfix: zerujemy fpArea jesli nie uzywane bo wywala muda na _bug()
    fpArea = NULL;
    RESERVE_OPEN;
}

void load_complains( )
{
    fread_complains( PBUG_FILE );
    fread_complains( PIDEA_FILE );
    fread_complains( PTYPO_FILE );
    if( !sysdata.silent )
	log_string( " Done complains" );
    return;
}

void fwrite_complain( COMPLAIN_DATA *comp, int type, bool fixed, FILE *fp )
{
    fprintf( fp, "#COMPLAIN\n" );

    if( fixed )
    fprintf( fp,
	"Author  %s~\n"
	"Type    %d\n"
	"Date    %s~\n"
	"FixedBy %s~\n"
	"FixDate %s~\n"
	"Room    %d\n"
	"Text    %s~\n",
		    comp->author,     	type,
		    comp->date, 	comp->fixedby,
		    comp->fixdate, 	comp->location,
		    comp->text );
    else
    fprintf( fp,
	"Author  %s~\n"
	"Type    %d\n"
	"Date    %s~\n"
	"Room    %d\n"
	"Text    %s~\n",
		    comp->author,     	type,
		    comp->date, 	comp->location,
		    comp->text );

    fprintf( fp, "End\n\n" );
    return;
}

void save_bugs( bool fixed )
{
    FILE *		fp;
    char *		fname = (char *)(fixed ? PBUG_FIX_FILE : PBUG_FILE);

    RESERVE_CLOSE;

    if( (fp=fopen( fname, (fixed?"a":"w") )) == NULL )
    {
        bug( "Cannot open %s", fname );
        perror( fname );
        RESERVE_OPEN;
        return;
    }

    for( auto* comp : complain_list )
    {
	if( comp->type != complain_data::COMPLAIN_BUG )
	    continue;

	fwrite_complain( comp, complain_data::COMPLAIN_BUG, fixed, fp );
    }
    if( !fixed )
	fprintf( fp, "#END\n\n" );
    fclose( fp );
    RESERVE_OPEN;
}

void save_typos( bool fixed )
{
    FILE *		fp;
    char *		fname = (char *)(fixed ? PTYPO_FIX_FILE : PTYPO_FILE);

    RESERVE_CLOSE;

    if( (fp=fopen( fname, (fixed?"a":"w") )) == NULL )
    {
        bug( "Cannot open %s", fname );
        perror( fname );
        RESERVE_OPEN;
        return;
    }

    for( auto* comp : complain_list )
    {
	if( comp->type != complain_data::COMPLAIN_TYPO )
	    continue;

	fwrite_complain( comp, complain_data::COMPLAIN_TYPO, fixed, fp );
    }
    if( !fixed )
	fprintf( fp, "#END\n\n" );
    fclose( fp );
    RESERVE_OPEN;
}

void save_ideas( bool fixed )
{
    FILE *		fp;
    char *		fname = (char *)(fixed ? PIDEA_FIX_FILE : PIDEA_FILE);

    RESERVE_CLOSE;

    if( (fp=fopen( fname, (fixed?"a":"w") )) == NULL )
    {
        bug( "Cannot open %s", fname );
        perror( fname );
        RESERVE_OPEN;
        return;
    }

    for( auto* comp : complain_list )
    {
	if( comp->type != complain_data::COMPLAIN_IDEA )
	    continue;

	fwrite_complain( comp, complain_data::COMPLAIN_IDEA, fixed, fp );
    }
    if( !fixed )
        fprintf( fp, "#END\n\n" );
    fclose( fp );
    RESERVE_OPEN;
}

void save_complains( bool fixed )
{
    save_bugs( fixed );
    save_ideas( fixed );
    save_typos( fixed );
}

COMPLAIN_DATA * new_complain( CHAR_DATA *ch, complain_data::Type type, char *txt )
{
    COMPLAIN_DATA *	comp;
    char *		strtime;

    strtime                    = ctime( &current_time );
    strtime[strlen(strtime)-1] = '\0';

    CREATE( comp, COMPLAIN_DATA, 1 );
    clear_complain( comp );
    STRDUP( comp->author, ch->name );
    STRDUP( comp->text, txt );
    STRDUP( comp->date, strtime );
    comp->type 		= type;
    comp->location 	= ch->in_room->vnum;
    return comp;
}

void show_complains( CHAR_DATA *ch, int type )
{
    int 		i = 1;

    for( auto* comp : complain_list )
    {
	if( comp->type != type )
	    continue;

	pager_printf( ch, FG_CYAN "[" FB_WHITE "%2d" FG_CYAN "] "
	    FB_WHITE "%s" FG_CYAN " Room:#" FB_WHITE "%d " FG_CYAN "[" FB_WHITE "%s" FG_CYAN "]" EOL
	    " %s" EOL
	    "------------------------------------------------------------------" NL,
		i++, comp->author, comp->location,
		comp->date, comp->text );
    }
}

DEF_DO_FUN( ide )
{
    send_to_char("Je�li masz jaki� pomys�, kt�ry chcesz zg�osi� administratorom,napisz" NL, ch);
    send_to_char("'idea <tre��_pomys�u>', a je�li chcesz zidentyfikowa� przedmiot... c�..." NL, ch);
    send_to_char("wymy�l co� :>." NL, ch);
    return;
}

DEF_DO_FUN( idea )
{
    COMPLAIN_DATA *	comp;
    char 		buf		[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
	send_to_char( "Jakim pomys�em chcesz si� podzieli� z kochanymi administratorami?" NL, ch );
	return;
    }

    comp = new_complain( ch, complain_data::COMPLAIN_IDEA, argument );
    complain_list.push_back( comp );
    save_complains( false );

    sprintf( buf, FB_YELLOW "%s ->" PLAIN " %s",
	ch->name, argument );
    to_channel( buf, CHANNEL_MONITOR, "P_IDEA ", 1);

    send_to_char( "Ok. Dzi�ki." NL, ch );
    return;
}

DEF_DO_FUN( bug )
{
    COMPLAIN_DATA *	comp;
    char 		buf		[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
	send_to_char( "O jakim b��dzie chcesz poinformowa� kochanych administrator�w?" NL, ch );
	return;
    }

    comp = new_complain( ch, complain_data::COMPLAIN_BUG, argument );
    complain_list.push_back( comp );
    save_complains( false );

    sprintf( buf, FB_YELLOW "%s ->" PLAIN " %s",
	ch->name, argument );
    to_channel( buf, CHANNEL_MONITOR, "P_BUG ", 1);

    send_to_char( "Ok. Dzi�ki." NL, ch );
    return;
}



DEF_DO_FUN( typo )
{
    COMPLAIN_DATA *	comp;
    char 		buf		[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
	send_to_char( "Jak� usterk� chcesz zg�osi� kochanym administratorom?" NL, ch );
	return;
    }

    comp = new_complain( ch, complain_data::COMPLAIN_TYPO, argument );
    complain_list.push_back( comp );
    save_complains( false );

    sprintf( buf, FB_YELLOW "%s ->" PLAIN " %s",
	ch->name, argument );
    to_channel( buf, CHANNEL_MONITOR, "P_TYPO ", 1);

    send_to_char( "Ok. Dzi�ki." NL, ch );
    return;
}

