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
*		Edytor tekstowy added by Thanos	from Envy        	   *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"


void string_edit( CHAR_DATA *ch, char **prdesc )
{
    if ( ch->substate == SUB_RESTRICTED )
    {
        send_to_char( NL FB_WHITE "Nie mo�esz w��cza� edytora z wewn�trz edytora !!!" EOL, ch );
	//musimy wywali� by oczy�ci� �mieci
        send_to_char( "Za kar� wylatujesz z edytora!" NL, ch );
        ch->desc->str_editing = NULL;
        return;
    }

    send_to_char( FB_WHITE
    "Edytor w��czony" PLAIN " (Wpisz "
	FB_WHITE "/?" PLAIN "=pomoc "
	FB_WHITE "@" PLAIN "=stop "
	FB_WHITE "/c" PLAIN "=czy�� "
	FB_WHITE "/l" PLAIN "=listuj "
	FB_WHITE "/f" PLAIN "=formatuj)" NL
    "--------------------------------------------------------------------------" NL, ch );

    if ( !*prdesc )
        STRDUP( *prdesc, "" );
    else
        **prdesc = '\0';

    ch->desc->str_editing = prdesc;
    return;
}


void string_append( CHAR_DATA *ch, char **prdesc )
{
    if ( ch->substate == SUB_RESTRICTED )
    {
        send_to_char( NL FB_WHITE "Nie mo�esz w��cza� edytora z wewn�trz edytora !!!" EOL, ch );
	//musimy wywali� by oczy�ci� �mieci
        send_to_char( "Za kar� wylatujesz z edytora!\n\r", ch );
        ch->desc->str_editing = NULL;
        return;
    }

    send_to_char( FB_WHITE
    "Edytor w��czony" PLAIN " (Wpisz "
	FB_WHITE "/?" PLAIN "=pomoc "
	FB_WHITE "@" PLAIN "=stop "
	FB_WHITE "/c" PLAIN "=czy�� "
	FB_WHITE "/l" PLAIN "=listuj "
	FB_WHITE "/f" PLAIN "=formatuj)" NL
    "--------------------------------------------------------------------------" NL, ch );

    if ( !*prdesc )
        STRDUP( *prdesc, "" );

    ch->desc->str_editing = prdesc;

    if ( *prdesc[0] != '\0' )
	string_add( ch, (char *)"/l" );

    return;
}

char * string_replace( char * orig, char * old, char * _new, bool sensitive )
{
    char xbuf [ MAX_STRING_LENGTH ];
    int i;

    xbuf[0] = '\0';
    strcpy( xbuf, orig );

    if( sensitive )
    {
	if( strstr( orig, old ) )
	{
    	    i = strlen( orig ) - strlen( strstr( orig, old ) );
    	    xbuf[i] = '\0';
    	    strcat( xbuf, _new );
    	    strcat( xbuf, &orig[i+strlen( old )] );
	}
    	    STRFREE( orig );
    }
    else
    {
	if( str_str( old, orig ) )
	{
    	    i = strlen( orig ) - strlen( str_str( old, orig ) );
    	    xbuf[i] = '\0';
    	    strcat( xbuf, _new );
    	    strcat( xbuf, &orig[i+strlen( old )] );
	}
    	    STRFREE( orig );
    }


    return STRALLOC( xbuf );
}

char * string_delete_last_line( char * old )
{
    char *ptr;
    char  buf [ MAX_STRING_LENGTH ];

    strcpy( buf, old );
    ptr = strrchr( buf, '\n' );
    if ( ptr )
	*ptr = '\0';
    ptr = strrchr( buf, '\n' );
    if ( ptr )
	*(ptr + 2) = '\0';
    else
	buf[0] = '\0';

    STRFREE( old );

    return ( STRALLOC( buf ) );
}

/*Thanos: /i */
char * string_insertline (CHAR_DATA * ch, char *orig, int line, char *addrdesc)
{
    char   *rdesc;
    char    buf[MAX_STRING_LENGTH];
    int     current_line = 1;
    int     i;

    buf[0] = '\0';
    strcpy (buf, orig);

    i = 0;

    for (rdesc = orig; *rdesc; rdesc++)
    {
    	if (current_line == line)
	    break;
	i++;

	if (*rdesc == '\r')
	    current_line++;
    }

    if (!*rdesc)
    {
    	send_to_char ("Taka linia nie istnieje.\n\r", ch);
	STRFREE( orig );
    	return STRALLOC(buf);
    }

    buf[i] = '\0';

    if (*addrdesc)
	strcat (buf, addrdesc);
    strcat( buf, "\n\r");

    strcat( buf, &orig[i]);
    STRFREE( orig );

    send_to_char ("Linia wstawiona.\n\r", ch);

    return STRALLOC( buf );
}


/* oraz /d <nr> */
char * string_deleteline (char *orig, int line)
{
    char   *rdesc;
    char    buf[MAX_STRING_LENGTH];
    int     current_line = 1;
    int     i = 0;

    buf[0] = '\0';

    for (rdesc = orig; *rdesc; rdesc++)
    {
    	if (current_line != line)
    	{
    	    buf[i] = *rdesc;
	    i++;
	}

	if (*rdesc == '\r')
	    current_line++;
    }

    STRFREE(orig);
    buf[i] = 0;

    return STRALLOC(buf);
}


void string_add( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    smash_tilde( argument );

    if ( *argument == '/' )
    {
        char arg1 [ MAX_INPUT_LENGTH ];
        char arg2 [ MAX_INPUT_LENGTH ];
        char arg3 [ MAX_INPUT_LENGTH ];



        argument = one_argument( argument, arg1 );

	if ( get_trust(ch) > LEVEL_IMMORTAL && !str_cmp( arg1, "/!" ) )
	{

	    DO_FUN *	last_cmd;
	    int 	substate = ch->substate;
	    bool	has_pager=false;

	    if( argument[0] == '\0' )
		return;

	    last_cmd = ch->last_cmd;
	    ch->substate = SUB_RESTRICTED;
	    if( IS_SET( ch->pcdata->flags, PCFLAG_PAGERON ) )
	    {
	    	has_pager = true;
		REMOVE_BIT( ch->pcdata->flags, PCFLAG_PAGERON );
	    }
	    interpret(ch, argument );
	    if( has_pager )
		SET_BIT( ch->pcdata->flags, PCFLAG_PAGERON );
	    ch->substate = substate;
	    ch->last_cmd = last_cmd;
	    return;
	}

        argument = first_arg( argument, arg2, false );
        argument = first_arg( argument, arg3, false );

        if ( !str_cmp( arg1, "/d" ) )
        {
            if ( arg2[0] == '\0' )
            {
        	*ch->desc->str_editing =
		    string_delete_last_line( *ch->desc->str_editing );
		send_to_char( "Ostatnia linia usuni�ta.\n\r", ch );
            }
	    else
	    if( !is_number (arg2) )
	    {
		send_to_char ( "Sk�adnia:  /d [nr_linii]\n\r", ch);
	        return;
	    }
	    else
	    {
	       *ch->desc->str_editing =
		    string_deleteline (*ch->desc->str_editing, atoi (arg2));
		ch_printf(ch, "Linia nr %d skasowana.\n\r", atoi (arg2));
	    }
	    return;
        }

	if (!str_cmp (arg1, "/i"))
	{
	   if (arg2[0] == '\0' || !is_number (arg2))
	   {
	      send_to_char ( "Sk�adnia:  /i <nr_linii> <text>\n\r", ch);
	      return;
	   }
	   smash_tilde (arg3);
	   *ch->desc->str_editing =
	       string_insertline (ch, *ch->desc->str_editing, atoi (arg2), arg3);
	   return;
	}

        if ( !str_cmp( arg1, "/c" ) )
        {
            send_to_char( "Ca�o�� usuni�ta.\n\r", ch );
            STRDUP( *ch->desc->str_editing, "" );
	    return;
        }

        if ( !str_cmp( arg1, "/l" ) )
        {
    	   char   *rdesc;
	   int     i = 1;

	   if( *ch->desc->str_editing[0] == '\0' )
	   {
		send_to_char( "Opis jest pusty\n\r", ch );
		return;
	   }

	   ch_printf(ch, FB_WHITE "%2d>" PLAIN " ", i);
	   for (rdesc = *ch->desc->str_editing; *rdesc; rdesc++)
	   {
	      if (*rdesc != '`')
		 ch_printf(ch, "%c", rdesc[0]);
	      else
	      {
	      	ch_printf(ch, "%c%c", rdesc[0], rdesc[1]);
		 rdesc++;
	      }
	      if (*rdesc == '\r' && *(rdesc + 1))
	      {
		 i++;
		 ch_printf(ch, FB_WHITE "%2d>" PLAIN " ", i);
	      }

	   }
	   return;

        }

        if ( !str_cmp( arg1, "/r" ) )
        {
            if ( arg2[0] == '\0' )
            {
                send_to_char(
                    "Sk�adnia:  /r 'stare pole tekstowe' \"nowe pole tekstowe\"\n\r", ch );
                return;
            }

            *ch->desc->str_editing =
                string_replace( *ch->desc->str_editing, arg2, arg3, true );
            sprintf( buf, "'%s' zamienione na \"%s\".\n\r", arg2, arg3 );
            send_to_char( buf, ch );
            return;
        }

        if ( !str_cmp( arg1, "/f" ) )
        {
	    int	linelen = 0;

            if( arg2[0] != 0 )
		linelen = atoi( arg2 );

	    if( linelen < 40 )
		linelen = 77;

	    *ch->desc->str_editing = ::format_string( *ch->desc->str_editing, linelen );
	    send_to_char( "Ca�o�� zag�szczona.\n\r", ch );
            return;
        }

        if ( !str_cmp( arg1, "/?" ) )
        {
	    ch_printf( ch, "Komendy Edytora:" NL
	    FB_WHITE ".----------------------------------------------------------------------." EOL
	    FB_WHITE "|" PLAIN "   /l                     wy�wietl tekst                              " FB_WHITE "|" EOL
	    FB_WHITE "|" PLAIN "   /c                     czy�� tekst                                 " FB_WHITE "|" EOL
	    FB_WHITE "|" PLAIN "   /d [linia]             skasuj lini� numer [linia]                  " FB_WHITE "|" EOL
	    FB_WHITE "|" PLAIN "   /i <linia> ['<tekst>'] wstaw lini�                                 " FB_WHITE "|" EOL
	    FB_WHITE "|" PLAIN "   /r '<stare>' \"<nowe>\"  zamie� <stare> na <nowe> (wymaga ''i \"\")    " FB_WHITE "|" EOL
	    FB_WHITE "|" PLAIN "   /f [d�ugo��]           sformatuj tekst (dopasuj do d�ugo�ci)       " FB_WHITE "|" EOL
	    "%s"
	    FB_WHITE "|" PLAIN "   @                      wyj�cie z edytora                           " FB_WHITE "|" EOL
	    FB_WHITE "`----------------------------------------------------------------------'" EOL,
	    (get_trust(ch) > LEVEL_IMMORTAL) ?
	    FB_WHITE "|" PLAIN "   /! <komenda>           wykonaj komend� zewn�trzn� (Ostro�nie!)     " FB_WHITE "|" EOL :
	    FB_WHITE "|" PLAIN "                                                                      " FB_WHITE "|" EOL );
            return;
        }


        send_to_char( "B��d:  Nieznana komenda (u�yj /?)" NL, ch );
        return;
    }

    if ( *argument == '@' )
    {
	REMOVE_BIT( ch->act, PLR_NOTE_WRITE );
        ch->desc->str_editing = NULL;
        return;
    }

    if ( strlen( buf ) + strlen( argument ) >= ( MAX_STRING_LENGTH - 4 ) )
    {
        send_to_char( "Za d�uga linia.\n\r", ch );
        ch->desc->str_editing = NULL;
        return;
    }

    strcpy( buf, *ch->desc->str_editing );

    strcat( buf, argument );
    strcat( buf, "\n\r" );
    STRDUP( *ch->desc->str_editing, buf );
    return;
}



char *format_string( char *oldrdesc, int len )
{
  char *rdesc;
  char  xbuf	[ MAX_STRING_LENGTH ];
  char  xbuf2	[ MAX_STRING_LENGTH ];
  int   i	= 0;
  bool  cap	= true;

  xbuf[0] = xbuf2[0] = 0;

  i = 0;

  if ( strlen( oldrdesc ) >= ( MAX_STRING_LENGTH - 4 ) )
  {
     bug( "rdesc to format_string() longer than MAX_STRING_LENGTH.", 0 );
     return ( oldrdesc );
  }

  for ( rdesc = oldrdesc; *rdesc; rdesc++ )
  {
    if ( *rdesc == '\n' )
    {
      if ( xbuf[i-1] != ' ' )
      {
        xbuf[i] = ' ';
        i++;
      }
    }
    else if ( *rdesc == '\r' ) ;
    else if ( *rdesc == ' ' )
    {
      if ( xbuf[i-1] != ' ' )
      {
        xbuf[i] = ' ';
        i++;
      }
    }
    else if ( *rdesc == ')' )
    {
      if ( xbuf[i-1] == ' ' && xbuf[i-2] == ' ' &&
          ( xbuf[i-3] == '.' || xbuf[i-3] == '?' || xbuf[i-3] == '!' ) )
      {
        xbuf[i-2] = *rdesc;
        xbuf[i-1] = ' ';
        xbuf[i] = ' ';
        i++;
      }
      else
      {
        xbuf[i]=*rdesc;
        i++;
      }
    }
    else if (*rdesc=='.' || *rdesc=='?' || *rdesc=='!')
    {
	xbuf[i] = *rdesc;
	i++;
        cap = true;
    }
    else
    {
      xbuf[i] = *rdesc;
      if ( cap )
        {
          cap = false;
          xbuf[i] = UPPER( xbuf[i] );
        }
      i++;
    }
  }
  xbuf[i] = 0;
  strcpy( xbuf2, xbuf );

  rdesc = xbuf2;

  xbuf[0] = 0;

  for ( ; ; )
  {
    for (i = 0; i < len; i++ )
    {
      if ( !*( rdesc + i ) ) break;
    }
    if ( i < len )
    {
      break;
    }
    for ( i = ( xbuf[0] ? len - 1 : len - 4 ) ; i ; i-- )
      if (*( rdesc + i ) == ' ') break;

    if ( i )
    {
      *( rdesc + i ) = 0;
      strcat( xbuf, rdesc );
      strcat( xbuf, "\n\r" );
      rdesc += i + 1;
      while ( *rdesc == ' ' ) rdesc++;
    }
    else
    {
      bug ( "No spaces", 0 );
      *( rdesc + len - 2 ) = 0;
      strcat( xbuf, rdesc );
      strcat( xbuf, "-\n\r" );
      rdesc += len - 1;
    }
  }
  while (*( rdesc + i ) && (*( rdesc + i ) == ' '||
                        *( rdesc + i ) == '\n'||
                        *( rdesc + i ) == '\r' ) )
    i--;
  *( rdesc + i + 1 ) = 0;
  strcat( xbuf, rdesc );
  if ( xbuf[ strlen( xbuf ) - 2 ] != '\n' )
    strcat( xbuf, "\n\r" );

  STRFREE( oldrdesc );
  return( STRALLOC( xbuf ) );
}

char *first_arg( char *argument, char *arg_first, bool fCase )
{
    char cEnd;

    while ( *argument == ' ' )
	argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"'
      || *argument == '%'  || *argument == '(' )
    {
        if ( *argument == '(' )
        {
            cEnd = ')';
            argument++;
        }
        else cEnd = *argument++;
    }

    while ( *argument != '\0' )
    {
	if ( *argument == cEnd )
	{
	    argument++;
	    break;
	}
    if ( fCase ) *arg_first = LOWER(*argument);
            else *arg_first = *argument;
	arg_first++;
	argument++;
    }
    *arg_first = '\0';

    while ( *argument == ' ' )
	argument++;

    return argument;
}


/*
 * Used in olc_act.c for aedit_builders.
 */
char * string_unpad( char * argument )
{
    char buf[MAX_STRING_LENGTH];
    char *s;

    s = argument;

    while ( *s == ' ' )
        s++;

    strcpy( buf, s );
    s = buf;

    if ( *s != '\0' )
    {
        while ( *s != '\0' )
            s++;
        s--;

        while( *s == ' ' )
            s--;
        s++;
        *s = '\0';
    }

    STRFREE( argument );
    return STRALLOC( buf );
}


char * string_proper( char * argument )
{
    char *s;

    s = argument;

    while ( *s != '\0' )
    {
        if ( *s != ' ' )
        {
            *s = UPPER(*s);
            while ( *s != ' ' && *s != '\0' )
                s++;
        }
        else
            s++;
    }

    return argument;
}


/* F-cja �cina znaki kolork�w w stringu  (chyba by Ackbar)*/
char *nocolor( const char *src )
{
  /* This function will remove all & color codes out of a string */
    int 		srclen,in_p,i;
    static char 	newstr		[MAX_STRING_LENGTH];
    int 		looking;

    srclen 	= strlen( src );
    in_p	= 0;
    looking 	= false;

    for( i=0, in_p=0; i < srclen; i++, in_p++ )
    {
	if( src[i] == __FGMOD[0] )
	    looking = 1;

	if( src[i] == __BGMOD[0] )
	    looking = 2;

	if( looking == 1 )
	{
    	    /* in case someone descides to put in && or maybe more */
    	    if( src[i+1] != __FGMOD[0] )
    		i += 2;
	    else
	    {
		newstr[in_p] = src[i];
		in_p++; i++;
	    }

    	    looking = 0;
    	}
	else
	if( looking == 2 )
	{
    	    /* in case someone descides to put in && or maybe more */
    	    if( src[i+1] != __BGMOD[0] )
    		i += 2;
	    else
	    {
		newstr[in_p] = src[i];
		in_p++; i++;
	    }

    	    looking = 0;
    	}
	newstr[in_p] = src[i];
    }
    newstr[in_p] = '\0';
    return newstr;
}


/* f - cja dopasowuje kolorowego stringa do d�ugo�ci len */
const char *strip_colors( const char *str, int length )
{
    int		len	= 0;
    int		rlen	= 0;
    static char	strout	[MSL];

    if( str[0] == '\0' )
    {
//	bug( "Strip_color: NULL string." );
	return "";
    }

    while( ( *str ) != '\0' && len < length )
    {
        strout[rlen] = *str;
	if( *str == __FGMOD[0] || *str == __BGMOD[0] )
	{
	    str++; rlen++;
	    strout[rlen] = *str;
	}
	else
	    len++;
	str++; rlen++;
    }
    /* reszt� (je�li jest dope�niamy spacjami */
    while( len < length )
    {
	strout[rlen] = ' ';
	len++;
	rlen++;
    }
    strout[rlen] = '\0';
    return strout;
}

/* wycina ze stringa lub wciska do niego wyraz w zale�no�ci czy odpowiednio
  by� on jego cz�ci� czy nie  -- Allokuje! */
void toggle_string( char **string, char *flag )
{
    char	buf	[MSL];

    if( !flag[0] )
	return;

    flag[0] = UPPER( flag[0] );

    if ( is_name( flag, *string ) )
    {
        strcpy( buf, *string );
        STRDUP( *string, cut_from_string( buf, flag ) );
    }
    else
    {
        buf[0] = '\0';
        if ( *string[0] != '\0' )
        {
            strcat( buf, *string );
            strcat( buf, " " );
        }
        strcat( buf, capitalize(flag) );
        STRDUP( *string, buf );
    }
    return;
}


//funkcja wycina ze stringa jedno wybrane s�owo - u�ywane do zdejmowania flag
char *cut_from_string( char *s_flag, const char *name )
{
    static char buf	[MAX_STRING_LENGTH];
    char 	buf2	[MAX_STRING_LENGTH];
    char 	minibuf	[MAX_STRING_LENGTH];

    buf[0] = '\0';
    strcpy( buf2, s_flag );
    for ( ; ; )
    {
        strcpy( buf2, my_one_argument( buf2, minibuf ) );

        if ( minibuf[0] == '\0' )
	    break;

	if ( str_cmp( minibuf, name ) )
	{
	    if( buf[0] != '\0' )
	        strcat( buf, " " );
	    strcat( buf, minibuf );
	}
    }
    return ( buf );
}

int swsnprintf(char *str, size_t size, const char *format, ...)
{
    va_list args;
    va_start(args, format);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-truncation"
    int ret = vsnprintf(str, size, format, args);
#pragma GCC diagnostic pop
    va_end(args);
    return ret;
}

char *swstrncpy(char *dest, const char *src, size_t n)
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
    return strncpy(dest, src, n);
#pragma GCC diagnostic pop
}

