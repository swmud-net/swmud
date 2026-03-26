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
*			   Player communication module     		   *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"

/* Thanos -- cenzura :P  --> patrz 'censore'*/
#define MAX_NICE_WORDS		20
struct  uglyword_list
{
	const char		* word;
	const char		* nice_word [MAX_NICE_WORDS];
};

bool MOBtrigger = false;
/*
*  Externals
*/
void send_obj_page_to_char		(CHAR_DATA * ch, OBJ_INDEX_DATA  * idx, char page);
void send_room_page_to_char		(CHAR_DATA * ch, ROOM_INDEX_DATA * idx, char page);
void send_page_to_char			(CHAR_DATA * ch, MOB_INDEX_DATA  * idx, char page);
void send_control_page_to_char	(CHAR_DATA * ch, char page);

/*
* Local functions.
*/
char	* scramble		args( ( const char *argument, int modifier ) );
char	* drunk_speech	args( ( const char *argument, CHAR_DATA *ch ) );
void	talk_channel	args( ( CHAR_DATA *ch, char *argument,
				int64 channel, const char *verb, const char *ask_verb ) );


void sound_to_room( ROOM_INDEX_DATA *room , const char *argument )
{
	CHAR_DATA *	vic;

	if ( room == NULL ) return;

	for ( vic = room->first_person; vic; vic = vic->next_in_room )
		if ( !IS_NPC(vic) && IS_SET( vic->act, PLR_SOUND ) )
			send_to_char( argument, vic );

}


bool is_ignored( CHAR_DATA *ch, CHAR_DATA *by_who )
{
	if( IS_NPC( ch ) || IS_NPC( by_who ) || !by_who->pcdata )
		return false;

	if ( is_name( ch->name, by_who->pcdata->ignorelist ) )
		return true;

	return false;
}

DEF_DO_FUN( beep )
{
	CHAR_DATA	*victim;
	char		arg[MSL];
	
	/*Milczace rasy by Ganis*/
	if( IS_SET(ch->race->flags, FRACE_MUTE) )
	{
		send_to_char("Przecie¿ przedstawiciele Twojej rasy nie umiej± mówiæ!" NL, ch);
		return;
	}

	argument = one_argument( argument, arg );

	REMOVE_BIT( ch->deaf, CHANNEL_TELLS );

	if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
	{
		send_to_char( "Nie mo¿esz zrobiæ tego tutaj." NL, ch );
		return;
	}

	if ( !IS_NPC(ch) && IS_SILENCED( ch ) )
	{
		send_to_char( "Nie mo¿esz tego zrobiæ." NL, ch );
		return;
	}

	if ( arg[0] == '\0' )
	{
		send_to_char( "Beepn±æ kogo?" NL, ch );
		return;
	}

	if ( ( victim = get_char_world( ch, arg ) ) == NULL
	|| (  IS_NPC(victim) && victim->in_room != ch->in_room ) )
	{
		send_to_char( "Nie ma tu nikogo takiego." NL, ch );
		return;
	}

	if( is_ignored( victim, ch ) )
	{
		ch_printf( ch, "Przecie¿ %s ignorujesz!" NL,
			MALE(victim)?"go":"j±" );

		if( !IS_ADMIN( ch->name ) )
			return;
	}

	if( is_ignored( ch, victim ) )
	{
		ch_printf( ch, "Jeste¶ ignorowan%s przez %s." NL,
		SEX_SUFFIX_YAE( ch ), FEMALE(victim)?"ni±":"niego" );

		if( !IS_ADMIN( ch->name ) )
			return;
	}


/* Thanos : platny tell i beep */
	if( ch->in_room && victim->in_room && !IS_NPC( ch ) && !IS_IMMORTAL( ch )
	&&  ch->in_room != victim->in_room )
	{

		if ( !get_comlink( ch ) )
		{
			send_to_char( "Potrzebujesz urz±dzenia komunikacyjnego by to zrobiæ!" NL, ch);
				return;
		}

		if ( !get_comlink( victim ) )
		{
			send_to_char( "Twój rozmówca nie ma urz±dzenia komunikacyjnego!" NL, ch);
			return;
		}

		if ( ch->gold < 1 )
		{
			send_to_char("Nie masz wystarczaj±cej ilo¶ci kredytek!" NL,ch);
			return;
		}

		ch->gold -= 1;
	}

	if ( !IS_NPC( victim ) && ( victim->switched )
	&& ( get_trust( ch ) > LEVEL_AVATAR ) )
	{
		send_to_char( "Ten gracz jest obecnie zajêty." NL, ch );
		return;
	}
	else if ( !IS_NPC( victim ) && ( !victim->desc ) )
	{
		ch_printf( ch, "%s ma zerwany link do muda." NL, FEMALE(victim)?"Ona":"On" );
		return;
	}

	if ( IS_SET( victim->deaf, CHANNEL_TELLS )
	&& ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
	{
		act( PLAIN, "$E ma wy³±czony kana³ TELLS.", ch, NULL, victim,
			TO_CHAR );
		return;
	}

	if( !IS_NPC (victim) && IS_SILENCED( victim ) )
	{
		send_to_char(
			"Ten gracz jest UCISZONY. Us³yszy twoj± wiadomo¶æ, ale nie odpowie." NL, ch );
	}

	if ( (!IS_IMMORTAL(ch) && !IS_AWAKE(victim) )
	|| (!IS_NPC(victim)&&IS_SET(victim->in_room->room_flags, ROOM_SILENCE ) ) )
	{
		act( PLAIN, "$E nie mo¿e ciê us³yszeæ.", ch, 0, victim, TO_CHAR );
		return;
	}

	argument = drunk_speech( argument, ch );
	ch_printf(ch , COL_TELL "Beepasz %s: " COL_TELL "%s\a" EOL ,
	can_see( ch, victim ) ? victim->przypadki[3] : "kogo¶", argument );
	send_to_char("\a",victim);

	if ( !IS_NPC(ch) || ch->speaking )
	{
		int speakswell = UMIN(knows_language(victim, ch->speaking, ch),
							knows_language(ch, ch->speaking, victim));

		if ( speakswell < 85 )
			act( COL_TELL, "$n$R beepa ciê '$t'", ch,
				translate(speakswell, argument, ch->speaking->name), victim, TO_VICT );
		else
			act( COL_TELL, "$n$R beepa ciê '$t'", ch, argument, victim, TO_VICT );
	}
	else
		act( COL_TELL, "$n$R beepa ciê '$t'", ch, argument, victim, TO_VICT );

	victim->reply	= ch;
	ch->retell		= victim;//added by Thanos

	//added by Thanos (last_tell)
	if( !IS_NPC( ch ) && !IS_NPC( victim ) )
	{
		if( !str_cmp( argument, "" ) )
			add_to_last_tell( victim, ch, PLAIN );
		else
			add_to_last_tell( victim, ch, argument );
	}
	//done

	mprog_tell_trigger( ch, argument, victim );
}

/* Text scrambler -- Altrag */
char *scramble( const char *argument, int modifier )
{
	static char arg[MIL];
	int			position = 0;
	int			conversion = 0;

	modifier %= number_range( 80, 300 ); /* Bitvectors get way too large #s */
	for ( ; position < MAX_INPUT_LENGTH; position++ )
	{
		if ( argument[position] == '\0' )
		{
			arg[position] = '\0';
			return arg;
		}
		else if ( argument[position] >= 'A' && argument[position] <= 'Z' )
		{
			conversion = -conversion + position - modifier + argument[position] - 'A';
			conversion = number_range( conversion - 5, conversion + 5 );
			while ( conversion > 25 )
				conversion -= 26;
			while ( conversion < 0 )
				conversion += 26;
			arg[position] = conversion + 'A';
		}
		else if ( argument[position] >= 'a' && argument[position] <= 'z' )
		{
			conversion = -conversion + position - modifier + argument[position] - 'a';
			conversion = number_range( conversion - 5, conversion + 5 );
			while ( conversion > 25 )
				conversion -= 26;
			while ( conversion < 0 )
				conversion += 26;
			arg[position] = conversion + 'a';
		}
		else if ( argument[position] >= '0' && argument[position] <= '9' )
		{
			conversion = -conversion + position - modifier + argument[position] - '0';
			conversion = number_range( conversion - 2, conversion + 2 );
			while ( conversion > 9 )
				conversion -= 10;
			while ( conversion < 0 )
				conversion += 10;
			arg[position] = conversion + '0';
		}
		else
			arg[position] = argument[position];
	}
	if (position >= MIL) {
		--position;
	}
	arg[position] = '\0';
	return arg;
}

/* percent = percent knowing the language. */
char *translate(int percent, const char *in, const char *name)
{
	LCNV_DATA		* cnv;
	LANG_DATA		* lng;
	const char		* pbuf;
	static char		buf	[256];
	char			buf2	[256];
	char			* pbuf2 = buf2;

	if ( percent > 99 || !str_cmp(name, "common") )
		return (char *) in;

	/* If we don't know this language... use "default" */
	if ( !(lng=find_lang((char *)name)) )
		if ( !(lng = find_lang("default")) )
			return (char *) in;

	for (pbuf = in; *pbuf;)
	{
		for (cnv = lng->first_precnv; cnv; cnv = cnv->next)
		{
			if (!str_prefix(cnv->old, pbuf))
			{
				if ( percent && (rand() % 100) < percent )
				{
					strncpy(pbuf2, pbuf, cnv->olen);
					pbuf2[cnv->olen] = '\0';
					pbuf2 += cnv->olen;
				}
				else
				{
					strcpy(pbuf2, cnv->_new);
					pbuf2 += cnv->nlen;
				}
				pbuf += cnv->olen;
				break;
			}
		}
		if (!cnv)
		{
			if (isalpha(*pbuf) && (!percent || (rand() % 100) > percent) )
			{
				*pbuf2 = lng->alphabet[LOWER(*pbuf) - 'a'];
				if ( isupper(*pbuf) )
					*pbuf2 = UPPER(*pbuf2);
			}
			else
				*pbuf2 = *pbuf;
			pbuf++;
			pbuf2++;
		}
	}
	*pbuf2 = '\0';
	for (pbuf = buf2, pbuf2 = buf; *pbuf;)
	{
		for (cnv = lng->first_cnv; cnv; cnv = cnv->next)
			if (!str_prefix(cnv->old, pbuf))
			{
				strcpy(pbuf2, cnv->_new);
				pbuf += cnv->olen;
				pbuf2 += cnv->nlen;
				break;
			}
		if (!cnv)
			*(pbuf2++) = *(pbuf++);
	}
	*pbuf2 = '\0';
#if 0
	for (pbuf = in, pbuf2 = buf; *pbuf && *pbuf2; pbuf++, pbuf2++)
		if (isupper(*pbuf))
			*pbuf2 = UPPER(*pbuf2);
		/* Attempt to align spacing.. */
		else if (isspace(*pbuf))
			while (*pbuf2 && !isspace(*pbuf2))
			pbuf2++;
#endif
	return buf;
}

char *drunk_speech( const char *argument, CHAR_DATA *ch )
{
	const 	char 	*arg 	= argument;
	static 	char 	 buf	[MAX_INPUT_LENGTH*2];
	char 			 buf1	[MAX_INPUT_LENGTH*2];
	int 		 drunk;
	char 			*txt;
	char 			*txt1;


	if ( IS_NPC( ch ) || !ch->pcdata ) return (char *)argument;

	drunk = ch->pcdata->condition[COND_DRUNK];

	if ( drunk <= 0 )
		return (char *)argument;

	buf[0]		= '\0';
	buf1[0]		= '\0';

	if ( !argument )
	{
		bug( "NULL argument" );
		return (char*)"";
	}

	/*
	if ( *arg == '\0' )
		return (char *) argument;
	*/

	txt 	= buf;
	txt1 	= buf1;

	while ( *arg != '\0' )
	{
		if ( toupper(*arg) == 'S' )
		{
			if ( number_percent() < ( drunk * 2 ) )		/* add 'h' after an 's' */
			{
				*txt++ = *arg;
				*txt++ = 'h';
			}
			else
				*txt++ = *arg;
		}
		else if ( toupper(*arg) == 'X' )
		{
			if ( number_percent() < ( drunk * 2 / 2 ) )
			{
				*txt++ = 'c', *txt++ = 's', *txt++ = 'h';
			}
			else
				*txt++ = *arg;
		}
		else /* Poprzekrêcane numerki by Thanos (z Envy) */
		if ( ( toupper(*arg) >= '0' ) && ( toupper(*arg) <= '9' ) )
		{
			char 	temp;
			int		i, max;
			max = number_range( 1, 2 );

			for( i=0; i<max; i++ )
			{
				temp   = '0' + number_range( 0, 9 );
				*txt++ = temp;
			}
		}
		else if ( number_percent() < ( drunk * 2 / 5 ) )  /* slurred letters */
		{
			int slurn = number_range( 1, 2 );
				int currslur = 0;

				while ( currslur < slurn )
			*txt++ = *arg, currslur++;
		}
		else
			*txt++ = *arg;

		arg++;
	};

	*txt = '\0';

	txt = buf;

	while ( *txt != '\0' )   /* Let's mess with the string's caps */
	{
		if ( number_percent() < ( 2 * drunk / 2.5 ) )
		{
			if ( isupper(*txt) )
				*txt1 = tolower( *txt );
			else if ( islower(*txt) )
				*txt1 = toupper( *txt );
			else
				*txt1 = *txt;
		}
		else
			*txt1 = *txt;
		txt1++, txt++;
	}

	*txt1 = '\0';
	txt1 = buf1;
	txt = buf;

	while ( *txt1 != '\0' )   /* Let's make them stutter */
	{
		if ( *txt1 == ' ' )  /* If there's a space, then there's gotta be a */
		{			 /* along there somewhere soon */

			while ( *txt1 == ' ' )  /* Don't stutter on spaces */
				*txt++ = *txt1++;

			if ( ( number_percent() < ( 2 * drunk / 4 ) ) && *txt1 != '\0' )
			{
				int offset = number_range( 0, 2 );
				int pos = 0;

				while ( *txt1 != '\0' && pos < offset )
					*txt++ = *txt1++, pos++;

				if ( *txt1 == ' ' )  /* Make sure not to stutter a space after */
				{		     /* the initial offset into the word */
					*txt++ = *txt1++;
					continue;
				}

				pos = 0;
				offset = number_range( 2, 4 );
				while (	*txt1 != '\0' && pos < offset )
				{
					*txt++ = *txt1;
					pos++;
					if ( *txt1 == ' ' || pos == offset )  /* Make sure we don't stick */
					{		               /* A hyphen right before a space	*/
						txt1--;
						break;
					}
					*txt++ = '-';
				}
				if ( *txt1 != '\0' )
					txt1++;
			}
		}
		else
			*txt++ = *txt1++;
	}

	*txt = '\0';

	return buf;
}


/* podmienia przekleñstwa na '³adniejsze' wyrazy 	-- Thanos */
char *censore( char *argument )
{
	char		arg	[MSL];
	int			i;

const struct uglyword_list uglywords[]=
{
	{ "dupa",	{ "odbyt", "kiszka", "pupa", "pupcia", "ty³eczek", "ty³ek", "po¶ladki", "" } },
	{ "dupe",	{ "odbyt", "kiszkê", "pupê", "pupciê", "ty³eczek", "ty³ek", "po¶ladki", "" } },
	{ "dupy",	{ "odbytu", "kiszki", "pupy", "pupci", "ty³eczka", "ty³ka", "po¶ladków", "" } },
	{ "cipa",	{ "dziura", "szpara", "szrama","dziura", "" } },
	{ "pipa",	{ "wnêka", "otwór", "otch³añ", "dziura","" } },
	{ "pizda",	{ "czarna dziura", "picza", "dziura", "" } },
	{ "chuj",	{ "banan", "trzonek", "siorek", "ogonek", "jasiek", "" } },
	{ "chuju",	{ "bananq", "trzoneczku", "siorku", "ogonku", "" } },
	{ "chujowy",{ "bananowy", "taki sobie", "niezbyt fajny", "nienajlepszy", "" } },
	{ "chujowa",{ "bananowa", "taka sobie", "niezbyt fajna", "nienajlepsza", "" } },
	{ "chujowe",{ "bananowe", "takie sobie", "niezbyt fajne", "nienajlepsze", "" } },
	{ "chujowo",{ "bananowe", "takie sobie", "niezbyt fajne", "nienajlepsze", "" } },
	{ "huj",	{ "fajfus", "trzonek", "siorek", "ogonek", "" } },
	{ "huju",	{ "fajfusie", "trzoneczku", "siorku", "ogonku", "" } },
	{ "hujowy",	{ "fajfusowaty", "taki sobie", "niezbyt fajny", "nienajlepszy", "" } },
	{ "hujowa",	{ "fajfusowata", "taka sobie", "niezbyt fajna", "nienajlepsza", "" } },
	{ "hujowe",	{ "fajfusowate", "takie sobie", "niezbyt fajne", "nienajlepsze", "" } },
	{ "kurwa",	{ "pani", "prostytutka", "zajêta kobieta", "kobieta pracuj±ca", "panna", "" } },
	{ "kuwa",	{ "pani", "prostytutka", "zajêta kobieta", "kobieta pracuj±ca", "panna", "" } },
	{ "qrwa",	{ "pani", "prostytutka", "zajêta kobieta", "kobieta pracuj±ca", "panna", "" } },
	{ "qwa",	{ "pani", "prostytutka", "zajêta kobieta", "kobieta pracuj±ca", "panna", "" } },
	{ "shit",	{ "qpa", "kupa", "ojej", "qpeñka","kupka",  "" } },
	{ "fuck",	{ "jasna ciasna", "duck", "ciak", "siak", "fook", "kciuk", "" } },
	{ "spierdalaj",{ "id¼ sobie", "odejd¼", "uciekaj", "zostaw mnie", "a we¼ ty",  "" } },
	{ "spierdalamy",{ "idziemy sobie", "odchodzimy", "uciekamy", "zostawiamy was", "" } },
	{ "spierdalajcie",{ "id¼cie sobie", "odejd¼cie", "uciekajcie", "zostawcie mnie", "a we¼cie",  "" } },
	{ "spierdala",{ "idzie sobie", "odchodzi", "ucieka", "zostawia mnie", "" } },
	{ "spierdalaja",{ "id± sobie", "odchodz±", "uciekaj±", "zostawiaj± mnie", "" } },
	{ "cholera",{ "no nie", "choroba", "epidemia", "do diaska", "ohoho",  "" } },
	{ "pierdol",{ "kituj", "¶ciemniaj", "pracuj", "" } },
	{ "pierdole",{ "kitujê", "¶ciemniam", "pracujê","mam gdzie¶", "" } },
	{ "pierdolony",{ "przepracowany", "" } },
	{ "pierdolona",{ "przepracowana", "" } },
	{ "pierdolone",{ "przepracowane", "" } },
	{ "jeb",    { "kot³uj", "rad¼", "" } },
	{ "jebany", { "skot³owany", "" } },
	{ "jebana", { "skot³owana", "" } },
	{ "jebane", { "skot³owane", "" } },
	{ "pieprz", { "sól", "przyprawa", "na ostro", "" } },
	{ "pieprze",{ "solê","przyprawiam","nie dbam", "mam gdzie¶", "" } },
	{ "" ,    	{ "" } }
};

	if( !*argument )
		return argument;


	for ( i=0; *uglywords[i].word; i++ )
	{
		while( true )
		{
			if( is_name( uglywords[i].word, argument ) )
			{
				int howmany;
				int range = -1;

				for( howmany = 0; *uglywords[i].nice_word[howmany]; howmany++)
					range = number_range( -1, howmany );

				if( range >= 0 )
					sprintf( arg, "[%s]", uglywords[i].nice_word[range] );
				else
					sprintf( arg, "[BEEP]" );

				argument = str_repl( argument, uglywords[i].word, arg, true );
			}
			else
				break;
		}
	}

	return argument;
}

/*
* Generic channel function.
*/
void talk_channel( CHAR_DATA *ch, char *argument, int64 channel, const char *verb, const char *ask_verb )
{
	char			buf	[MSL];
	char			buf2[MSL];
	char			orig_arg[MSL];
	DESCRIPTOR_DATA	*d;
	int				position;
	CLAN_DATA		*clan = NULL;
	char			nverb[MSL];
	const char		*color = PLAIN;

	/* milczace rasy by Ganis */
	if ( IS_SET(ch->race->flags, FRACE_MUTE) )
	{
		send_to_char("Przecie¿ przedstawiciele Twojej rasy nie umiej± mówiæ!" NL, ch);
		return;
	}

	/* cenzura by Thanos */
	if( channel == CHANNEL_CHAT    ||  channel == CHANNEL_OOC
	||  channel == CHANNEL_MUSIC   ||  channel == CHANNEL_NEWBIE )
		argument = censore( argument );

	strcpy( nverb, verb );
	nverb[0] 	= UPPER( nverb[0] );

	if( channel != CHANNEL_SHOUT   && channel != CHANNEL_YELL
	&&  channel != CHANNEL_IMMTALK && channel != CHANNEL_OOC
	&&  channel != CHANNEL_ASK     && channel != CHANNEL_NEWBIE
	&&  channel != CHANNEL_SHIP    && channel != CHANNEL_SYSTEM
	&&  channel != CHANNEL_SPACE   && channel != CHANNEL_QUEST
	&&  channel != CHANNEL_OLCTALK && channel != CHANNEL_ADMINTALK
	&&  channel != CHANNEL_CODERTALK
	&& !get_comlink( ch ) )
	{
		send_to_char( "By to zrobiæ potrzebujesz urz±dzenia komunikacyjnego!" NL, ch);
		return;
	}

	if ( IS_NPC( ch ) && (channel == CHANNEL_CLAN || channel == CHANNEL_ORDER || channel == CHANNEL_ORDER) )
	{
		send_to_char( "Przecie¿ nie jeste¶ w klanie. Jeste¶ mobem, nie masz ¿adnych praw." NL, ch );
		return;
	}

	/* Trog: nie podoba mi sie to. */
	if ( channel == CHANNEL_CLAN )
	{
		if ( ch->pcdata->clan->mainorg )
			clan = ch->pcdata->clan->mainorg;
		else
			clan = ch->pcdata->clan;
	}

	if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE )
	&& channel != CHANNEL_QUEST )/*Thanos*/
	{
		send_to_char( "Nie mo¿esz zrobiæ tego tutaj." NL, ch );
		return;
	}

	if ( IS_NPC( ch ) && IS_AFFECTED( ch, AFF_CHARM ) )
	{
		if ( ch->master )
			send_to_char( "Albo jednak nie..." NL, ch->master );
		return;
	}

	if ( argument[0] == '\0' )
	{//added by Thanos (ask_verb)
		sprintf( buf, "Na jaki temat chcesz %s?" NL, ask_verb );
		send_to_char( buf, ch );	/* where'd this line go? */
		return;
	}

	if( !IS_NPC(ch) && IS_SILENCED( ch )
	&& channel != CHANNEL_QUEST )/*Thanos*/
	{
		ch_printf( ch, "Nie mo¿esz u¿ywaæ kana³ów." NL );
		return;
	}

	/* A kto to wywali³ ??? 	ubije - Thanos ;) */
	strcpy( orig_arg, argument );
	argument 	= drunk_speech( argument, ch );

	REMOVE_BIT(ch->deaf, channel);

	switch ( channel )
	{
	default:
		color = FB_CYAN;
		ch_printf( ch, "%s%ssz przez sieæ publiczn± '%s" RESET FB_CYAN "'" EOL,
			color, nverb, argument );
		sprintf( buf, "$n$0 %s przez sieæ publiczn± '$t" RESET "'", verb );
		add_to_last_buf( ch, argument, verb, CHANNEL_CHAT );

	break;

	case CHANNEL_QUEST:
		color = FB_GREEN;
		ch_printf( ch, "%s%ssz '%s" RESET FB_GREEN "'" EOL,
			color, nverb, argument );
		sprintf( buf, "$n %s '$t" RESET "'", verb );
		add_to_last_buf( ch, argument, verb, CHANNEL_CHAT );
	break;

	case CHANNEL_FLAME:
		color = FG_BLACK;
		ch_printf( ch, "%s%ssz przez sieæ publiczn± '%s" RESET FG_BLACK "'" EOL,
			color, nverb, argument );
		sprintf( buf,  "$n$0 %s przez sieæ publiczn± '$t" RESET "'", verb );
		/* NIE DODAWAÆ DO LASTBUFA !!! */
	break;

	case CHANNEL_CLANTALK:
		color = FB_WHITE;
		ch_printf( ch, "%sNadajesz przez prywatn± sieæ %s '%s" RESET FB_WHITE "'" EOL,
			color, CLANTYPE( clan, 1 ), argument );
		sprintf( buf, "$n$0 nadaje przez prywatn± sieæ %s '$t" RESET "'", CLANTYPE( clan, 1 ) );
		add_to_last_ctalk( ch->pcdata->clan, ch, argument );
	break;

	case CHANNEL_SHIP:
		color = FG_PINK;
		ch_printf( ch, "%sNadajesz przez komunikator pok³adowy '%s" RESET FG_PINK "'" EOL,
			color, argument );
		sprintf( buf, "$n$0 nadaje przez komunikator pok³adowy '$t" RESET "'"  );
	break;

	case CHANNEL_SYSTEM:
	case CHANNEL_SPACE:
		color = FB_CYAN;
		ch_printf( ch, "%s%ssz przez komunikator '%s" RESET FB_CYAN "'" EOL,
			color, nverb, argument );
		sprintf( buf,  "$n$0 %s przez komunikator '$t" RESET "'", verb );
	break;

	case CHANNEL_YELL:
	case CHANNEL_SHOUT:
		color = COL_YELL;
		ch_printf( ch, "%s%ssz '%s" RESET COL_YELL "'" EOL, color, nverb, argument );
		sprintf( buf, "$n$0 %s '$t" RESET "'", verb );
	break;

	case CHANNEL_ASK:
		color = COL_OOC;
		ch_printf( ch, "%s(OOC) %ssz '%s" RESET "%s" RESET COL_OOC "'" EOL, color, nverb, argument, color );
		sprintf( buf, "(OOC) %s %s '$t" RESET "'", ch->przypadki[0], verb );
	break;

	case CHANNEL_NEWBIE:
		color = FB_PINK;
		ch_printf( ch, "%s(NEWBIE) %s: %s" RESET EOL, color, ch->name, argument );
		sprintf( buf, "(NEWBIE) %s: $t" RESET, ch->przypadki[0] );
			//added by Thanos (dodaje liniê do bufora (do_last) ) Ratma
		add_to_last_buf( ch, argument, "(NEWBIE)", CHANNEL_CHAT );
	break;

	case CHANNEL_OOC:
		color = COL_OOC;
		ch_printf(ch, "%s(OOC) %s: %s" RESET EOL, color, ch->name, orig_arg );
		sprintf( buf, "(OOC) $n$x: $t" RESET );
		add_to_last_buf( ch, orig_arg, "(OOC)", CHANNEL_OOC );
	break;

	case CHANNEL_WARTALK:
		color = FB_RED;
		ch_printf( ch, "%s%ssz do klanów '%s" RESET "%s" RESET FB_RED "'" EOL,
		color, nverb, argument, color );
		sprintf( buf,  "$n %s do klanów '$t" RESET "'", verb );
	break;

	case CHANNEL_CODERTALK:
	case CHANNEL_OLCTALK:
	case CHANNEL_ADMINTALK: //added by Trog
	case CHANNEL_IMMTALK:
	if( channel == CHANNEL_OLCTALK )
	{
		sprintf( buf, FB_BLUE "(" FB_WHITE "$n$R" FB_BLUE ")" FB_GREEN " $t" RESET PLAIN );
	}
	else if ( channel == CHANNEL_ADMINTALK ) //added by Trog
	{
		sprintf( buf, FB_YELLOW "[" PLAIN "$n$0" FB_YELLOW "]" FB_RED ": " FB_YELLOW "$t" RESET PLAIN );
	}
	else if ( channel == CHANNEL_IMMTALK )
	{
		sprintf( buf, "%s" FB_CYAN ">" FB_WHITE " $t" RESET PLAIN, ch->przypadki[0] ); //tanglor
	}
	else if ( channel == CHANNEL_CODERTALK )
	{
		sprintf( buf, FG_CYAN "$n$0" FG_CYAN " << \"$t" FG_CYAN RESET "\";" PLAIN );
	}

		position	= ch->position;
		ch->position	= POS_STANDING;
		act( (char*)(channel==CHANNEL_ADMINTALK ? COL_ADMINTALK : COL_IMMORT),
			buf, ch, argument, NULL, TO_CHAR );
		ch->position	= position;

		// by Ratm Nie wszystkie kana³y powinny pokazywaæ pijack± gadkê
		if ( channel == CHANNEL_IMMTALK ) // na tym moze zostac
			add_to_last_buf( ch, argument, verb, channel );
		else	// na reszcie przeszkadza
			add_to_last_buf( ch, orig_arg, verb, channel );
		break;
	}

	if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
	{
		sprintf( buf2, "%s (%s): %s", ch->przypadki[0],
			verb, argument );
		append_to_file( LOG_FILE, buf2 );
	}

	for ( d = first_descriptor; d; d = d->next )
	{
		CHAR_DATA *och;
		CHAR_DATA *vch;

		och = d->original ? d->original : d->character;
		vch = d->character;

		if ( d->connected >= CON_PLAYING
		&&   vch != ch
		&&  !IS_SET(och->deaf, channel) )
		{
			char *sbuf;

		// by Ratm Nie wszystkie kana³y powinny pokazywaæ pijack± gadkê
		if( channel == CHANNEL_OLCTALK
		|| channel == CHANNEL_CODERTALK
		|| channel == CHANNEL_ADMINTALK
		|| channel == CHANNEL_OOC )
			sbuf = orig_arg;
		else
			sbuf = argument;

		if( is_ignored( ch, d->character ) )
			continue;

		if(channel != CHANNEL_SHOUT   && channel != CHANNEL_YELL
		&& channel != CHANNEL_IMMTALK && channel != CHANNEL_OOC
		&& channel != CHANNEL_ASK     && channel != CHANNEL_NEWBIE
		&& channel != CHANNEL_OLCTALK && channel != CHANNEL_SHIP
		&& channel != CHANNEL_SYSTEM  && channel != CHANNEL_SPACE
		&& channel != CHANNEL_CODERTALK && channel != CHANNEL_ADMINTALK
		&& !get_comlink( ch ) ) //added by Trog
			continue;

		if( channel == CHANNEL_IMMTALK && !IS_HERO(och) )
			continue;//mod by Trog
		if( channel == CHANNEL_ADMINTALK && !IS_ADMIN(och->name) )
			continue;  //added by Trog
		if( channel == CHANNEL_WARTALK && NOT_AUTHED( och ) )
			continue;
		if( channel == CHANNEL_OLCTALK && !IS_OLCMAN(och)
		&& !IS_ADMIN( och->name )
		&& (get_trust( och ) <103 ) )
			continue;
		if( channel == CHANNEL_CODERTALK && !
		IS_CODER( och->name ) )
			continue;

		if ( IS_SET( vch->in_room->room_flags, ROOM_SILENCE )
		&& channel != CHANNEL_QUEST )/*Thanos*/
			continue;

		if ( channel == CHANNEL_YELL || channel == CHANNEL_SHOUT )
		{
			if ( ch->in_room && och->in_room
					&& ch->in_room->area != och->in_room->area )
				continue;
		}

		if( channel == CHANNEL_CLAN || channel == CHANNEL_ORDER
		|| channel == CHANNEL_GUILD )
		{
			MEMBER_DATA *member;

			if( IS_NPC( vch ) || !(member = get_member( clan, vch->name )) || member->status < CLAN_MEMBER )
				continue;
		}

		if ( channel == CHANNEL_SHIP || channel == CHANNEL_SPACE || channel == CHANNEL_SYSTEM )
		{
			SHIP_DATA *ship = ship_from_cockpit( ch->in_room );
			SHIP_DATA *target;

			if ( !ship )
				continue;

			if ( !vch->in_room )
				continue;

			if ( channel == CHANNEL_SHIP )
				if ( !vch->in_room->ship || vch->in_room->ship != ship )
					continue;

			target = ship_from_cockpit( vch->in_room );

			if (!target) continue;

			if ( channel == CHANNEL_SYSTEM )
				if (target->starsystem != ship->starsystem )
					continue;
		}

		position		= vch->position;
		if ( channel != CHANNEL_SHOUT && channel != CHANNEL_YELL )
			vch->position	= POS_STANDING;

		if( (!IS_NPC(ch) || ch->speaking)
		&& ( channel != CHANNEL_OOC
		&&  channel != CHANNEL_ASK
		&&  channel != CHANNEL_QUEST
		&&	 channel != CHANNEL_NEWBIE
		&&  channel != CHANNEL_AUCTION
		&&  channel != CHANNEL_OLCTALK
		&&  channel != CHANNEL_CODERTALK
		&&  channel != CHANNEL_ADMINTALK )  )
		{
			int speakswell = UMIN(knows_language(vch, ch->speaking, ch),
						knows_language(ch, ch->speaking, vch) );

			if ( speakswell < 85 )
				sbuf = translate(speakswell, argument, ch->speaking->name);
		}

		MOBtrigger = false;
		buf[0] = UPPER( buf[0] );

		act( color, buf, ch, sbuf, vch, TO_VICT );
		vch->position	= position;
		}
	}
	return;
}

void to_channel( const char *argument, int64 channel, const char *verb, int level )
{
	char			buf[MAX_STRING_LENGTH];
	DESCRIPTOR_DATA	* d;

	if ( !first_descriptor || argument[0] == '\0' )
		return;

	if( isalnum( argument[0] ) || channel == CHANNEL_COMM )
		sprintf(buf, "%s: %s" EOL, 	verb, 	argument );
	else
		sprintf(buf, "%s%s"   EOL, 	verb, 	argument );

	for ( d = first_descriptor; d; d = d->next )
	{
		CHAR_DATA *och;
		CHAR_DATA *vch;

		och = d->original ? d->original : d->character;
		vch = d->character;

		if ( !och || !vch )
			continue;

		if((!IS_IMMORTAL(vch) && !IS_OLCMAN( vch ) )
		|| ( get_trust(vch) < sysdata.build_level && channel == CHANNEL_BUILD )
		|| ( get_trust(vch) < sysdata.prog_level && channel == CHANNEL_PROG )
		|| ( get_trust(vch) < sysdata.log_level
		&& ( channel == CHANNEL_LOG || channel == CHANNEL_COMM) ))
			continue;

		if ( d->connected >= CON_PLAYING )
		{
			if( channel == CHANNEL_PROGBUG )
				ch_printf( vch, FB_BLUE "%s" PLAIN, buf, vch );
			else
			if( !IS_SET(och->deaf, channel)
			&& get_trust( vch ) >= level )
				ch_printf( vch, FG_PINK "%s" PLAIN, buf, vch );
		}
	}
	return;
}


DEF_DO_FUN( chat )
{
	talk_channel( ch, argument, CHANNEL_CHAT, "gawêdzi", "pogawêdziæ" );
	return;
}

DEF_DO_FUN( flame )
{
	if ( ch->gold < 1 )
	{
		send_to_char("Nie masz wystarczaj±cej ilo¶ci kredytek!" NL ,ch);
		return;
	}

	ch->gold -= 1;

	talk_channel( ch, argument, CHANNEL_FLAME, "przeklina", "przekl±æ" );
	return;
}

DEF_DO_FUN( questchat )
{
	talk_channel( ch, argument, CHANNEL_QUEST, "questuje", "questowaæ" );
	return;
}

DEF_DO_FUN( shiptalk )
{
	SHIP_DATA *ship;

	if ( (ship = ship_from_cockpit(ch->in_room)) == NULL )
	{
		send_to_char("By to zrobiæ musisz byæ w kokpicie statku!" NL,ch);
		return;
	}
	talk_channel( ch, argument, CHANNEL_SHIP, "nadaje", "nadawaæ" );
	return;
}

DEF_DO_FUN( systemtalk )
{
	SHIP_DATA *ship;

	if ( (ship = ship_from_cockpit(ch->in_room)) == NULL )
	{
		send_to_char("By to zrobiæ musisz byæ w kokpicie statku!" NL,ch);
		return;
	}
	talk_channel( ch, argument, CHANNEL_SYSTEM, "og³asza", "og³osiæ" );
	return;
}

DEF_DO_FUN( spacetalk )
{
	SHIP_DATA *ship;

	if ( (ship = ship_from_cockpit(ch->in_room)) == NULL )
	{
		send_to_char("By to zrobiæ musisz byæ w kokpicie statku!" NL,ch);
		return;
	}
	talk_channel( ch, argument, CHANNEL_SPACE, "nadaje" , "nadawaæ");
	return;
}

DEF_DO_FUN( ooc )
{
	talk_channel( ch, argument, CHANNEL_OOC, "ooc", "mówiæ na kanale OOC" );
	return;
}

DEF_DO_FUN( clantalk )
{
	if (NOT_AUTHED(ch))
	{
		huh( ch );
		return;
	}

	if ( IS_NPC( ch ) || !ch->pcdata->clan )
	{
		huh( ch );
		return;
	}

	talk_channel( ch, argument, CHANNEL_CLAN, "mówi", "mówiæ do klanu" );
	/* by Ratm Przenios³em poni¿sz± liniê do talk_channel(), tam jej miejsce ;)
	add_to_last_ctalk( ch->pcdata->clan, ch, argument ); */
	return;
}

DEF_DO_FUN( newbiechat )
{
	//added by Thanos & Trog (immosi & NH mog± gadac na newbie)
	/*added by Pixel, wszyscy korzystaja z kanalu pomocy, bo sa debilami
	if ( !IS_NEWBIE( ch ) && !IS_IMMORTAL( ch ) && !IS_NH( ch ) )
	{
		send_to_char( "Czy nie za pó¼no ju¿ na ten kana³?" NL, ch );
		return;
	}
	*/
	talk_channel( ch, argument, CHANNEL_NEWBIE, "gawêdzi", "pogawêdziæ" );
	return;
}

DEF_DO_FUN( ot )
{
	do_ordertalk( ch, argument );
}

DEF_DO_FUN( ordertalk )
{
	huh( ch );
	return;
}

DEF_DO_FUN( guildtalk )
{
	huh( ch );
	return;
}

DEF_DO_FUN( music )
{
	if (NOT_AUTHED(ch))
	{
		send_to_char("Nie masz jeszcze autoryzacji. Nie mo¿esz korzystaæ z tego kana³u." NL, ch);
		return;
	}
	talk_channel( ch, argument, CHANNEL_MUSIC, "¶piewa", "za¶piewaæ" );
	return;
}

DEF_DO_FUN( ask )
{
	talk_channel( ch, argument, CHANNEL_ASK, "pyta", "zapytaæ" );
	return;
}

DEF_DO_FUN( answer )
{
	talk_channel( ch, argument, CHANNEL_ASK, "odpowiada", "odpowiedzieæ" );
	return;
}

DEF_DO_FUN( shout )
{
	// by Ratm ( wywalamy krzyczenie na calego muda przez graczy )
	if( !IS_NPC( ch ) )
	{
		huh( ch );
		return;
	}

	if (NOT_AUTHED(ch))
	{
		send_to_char("Nie krzycz. Nie masz autoryzacji, a chcesz ha³asowaæ?" NL, ch);
		return;
	}

	talk_channel( ch, argument, CHANNEL_SHOUT, "krzyczy", "krzyczeæ" );
	WAIT_STATE( ch, 12 );
	return;
}

DEF_DO_FUN( yell )
{
	if(!IS_NPC( ch ) && NOT_AUTHED(ch) )
	{
		send_to_char("Nie masz autoryzacji, wiêc nie mo¿esz wrzeszczeæ." NL, ch);
		return;
	}

	talk_channel( ch, argument, CHANNEL_YELL, "wrzeszczy", "wrzeszczeæ"  );
	WAIT_STATE( ch, 10 );
	return;
}

DEF_DO_FUN( immtalk )
{
	if (NOT_AUTHED(ch))
	{
		huh( ch );
		return;
	}
	talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk", "mówiæ na immtalku"  );
	return;
}

//added by Trog
DEF_DO_FUN( admintalk )
{
	if( !IS_ADMIN( ch->name ) )
	{
		huh( ch );
		return;
	}

	talk_channel( ch, argument, CHANNEL_ADMINTALK, "admintalk", "mówiæ na admintalku" );
	return;
}

//Trog
DEF_DO_FUN( codertalk )
{
	if( !IS_CODER( ch->name ) )
	{
		huh( ch );
		return;
	}
		talk_channel( ch, argument, CHANNEL_CODERTALK,
				"codertalk", "kodowaæ (tzn. mówiæ :-)) na codertalku" );
	return;
}

DEF_DO_FUN( olctalk )
{
	if( !IS_OLCMAN( ch )
	&&  !IS_ADMIN( ch->name )
	&&  get_trust( ch ) < 103 )
	{
		huh( ch );
		return;
	}

	talk_channel( ch, argument, CHANNEL_OLCTALK, "olctalk", "rozmawiaæ na olctalku" );
	return;
}

DEF_DO_FUN( say )
{
	char			buf [MAX_STRING_LENGTH];
	CHAR_DATA		* vch;
	int				actflags;

	/*Milczace rasy by Ganis*/
	if ( IS_SET(ch->race->flags, FRACE_MUTE) )
	{
		send_to_char("Przecie¿ przedstawiciele Twojej rasy nie umiej± mówiæ!" NL, ch);
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Powiedzieæ co?" NL, ch );
		return;
	}

	if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) )
	{
		send_to_char( "Nie mo¿esz zrobiæ tego tutaj." NL, ch );
		return;
	}

	actflags = ch->act;

	if ( IS_NPC( ch ) ) REMOVE_BIT( ch->act, ACT_SECRETIVE );
	for ( vch = ch->in_room->first_person; vch; vch = vch->next_in_room )
	{
		char *sbuf = argument;

		if ( vch == ch )
			continue;

		if ( (!IS_NPC(ch) || ch->speaking) )
		{
			int speakswell = UMIN(knows_language(vch, ch->speaking, ch ),
					knows_language( ch, ch->speaking, vch));

			if ( speakswell < 75 )
				sbuf = translate(speakswell, argument, ch->speaking->name);
		}
		sbuf = drunk_speech( sbuf, ch );

		MOBtrigger = false;

		if( is_ignored( ch, vch ) )
			act( COL_SAY, "$n co¶ tam glêdzi.",
			ch, sbuf, vch, TO_VICT );
		else
			act( COL_SAY, "$n mówi '$t'", ch, sbuf, vch, TO_VICT );
	}

	ch->act = actflags;
	MOBtrigger = false;
	act( COL_SAY, "Mówisz '$T'" PLAIN,
	ch, NULL, drunk_speech( argument, ch ), TO_CHAR );

	if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
	{
		sprintf( buf, "%s: %s", IS_NPC( ch ) ? ch->przypadki[0] : ch->name,
			argument );
		append_to_file( LOG_FILE, buf );
	}

	mprog_speech_trigger( argument, ch );
	if ( char_died(ch) )
		return;
	oprog_speech_trigger( argument, ch );
	if ( char_died(ch) )
		return;
	rprog_speech_trigger( argument, ch );
	return;
}

void ch_tell( CHAR_DATA *ch, CHAR_DATA *victim, const char *argument )
{
	ch_tell( ch, victim, (char*)argument );
}

void ch_tell( CHAR_DATA *ch, CHAR_DATA *victim, char *argument )
{
	char		buf [MAX_INPUT_LENGTH];
	int			position;
	CHAR_DATA	* switched_victim;

	/*Milczace rasy by Ganis*/
	if( IS_SET(ch->race->flags, FRACE_MUTE) )
	{
		send_to_char("Przecie¿ przedstawiciele Twojej rasy nie umiej± mówiæ!" NL, ch);
		return;
	}

	switched_victim = NULL;

	if( is_ignored( victim, ch ) )
	{
		ch_printf( ch, "Przecie¿ %s ignorujesz!" NL,
			FEMALE(victim) ? "j±" : "go" );

		if( !IS_ADMIN( ch->name ) )
			return;
	}

	if( is_ignored( ch, victim ) )
	{
		ch_printf( ch, "Jeste¶ przez %s ignorowan%s." NL,
		FEMALE( victim ) ? "ni±" : "niego", SEX_SUFFIX_YAE( ch ) );

		if( !IS_ADMIN( ch->name ) )
			return;
	}

	if ( !IS_NPC( ch ) && IS_SET( ch->deaf, CHANNEL_TELLS )
	&& !IS_IMMORTAL( ch ) )
	{
		act( PLAIN, "Masz przecie¿ wy³±czony kana³ TELLS. Spróbuj najpierw 'channels +tells'.", ch, NULL, NULL,
		TO_CHAR );
		return;
	}

	if ( IS_SET( ch->in_room->room_flags, ROOM_SILENCE ) && !IS_NPC( ch ) )
	{
		send_to_char( "Nie mo¿esz zrobiæ tego tutaj." NL, ch );
		return;
	}

	if( !IS_NPC(ch) && IS_SILENCED( ch ) )
	{
		send_to_char( "Nie mo¿esz tego zrobiæ." NL, ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Powiedzieæ komu co?" NL, ch );
		return;
	}

	if ( !victim || ( IS_NPC(victim) && victim->in_room != ch->in_room )  )
	{
		send_to_char( "Nie ma tu nikogo takiego." NL, ch );
		return;
	}

	if ( ch == victim )
	{
		send_to_char( "Prowadzisz ze sob± mi³± i pouczaj±c± konwersacjê." NL, ch );
		return;
	}


/* Thanos : platny tell i beep */
	if( ch->in_room && victim->in_room && !IS_NPC( ch ) && !IS_IMMORTAL( ch )
	&&  ch->in_room != victim->in_room )
	{

		if ( !get_comlink( ch ) )
		{
			send_to_char( "Potrzebujesz urz±dzenia komunikacyjnego by to zrobiæ!" NL, ch);
			return;
		}

		if ( !get_comlink( victim ) )
		{
			send_to_char( "Twój rozmówca nie ma urz±dzenia komunikacyjnego!" NL, ch);
			return;
		}

		if ( ch->gold < 1 )
		{
				send_to_char("Nie masz wystarczaj±cej ilo¶ci kredytek!" NL,ch);
				return;
		}

		ch->gold -= 1;
	}


	if ( !IS_NPC( victim ) && ( victim->switched )
		&& ( get_trust( ch ) > LEVEL_AVATAR )
		&& !IS_SET(victim->switched->act, ACT_POLYMORPHED)
		&& !IS_AFFECTED(victim->switched, AFF_POSSESS) )
	{
		send_to_char( "Ten gracz nie mo¿e siê z tob± w tej chwili porozumieæ." NL, ch );
		return;
	}
	else if ( !IS_NPC( victim ) && ( victim->switched )
		&& (IS_SET(victim->switched->act, ACT_POLYMORPHED)
		||  IS_AFFECTED(victim->switched, AFF_POSSESS) ) )
		switched_victim = victim->switched;
	else if ( !IS_NPC( victim ) && ( !victim->desc ) )
	{
		ch_printf( ch, "%s ma zerwany link do muda." NL,
			FEMALE(victim) ? "Ona" : "On" );
		return;
	}

	if ( IS_SET( victim->deaf, CHANNEL_TELLS )
		&& ( !IS_IMMORTAL( ch ) || ( get_trust( ch ) < get_trust( victim ) ) ) )
	{
		act( PLAIN, "Twój rozmówca ma wy³±czony kana³ TELLS.", ch, NULL, victim,
			TO_CHAR );
		return;
	}

	if ( !IS_NPC (victim) && IS_SILENCED( victim ) )
		send_to_char( "Ten gracz jest UCISZONY. Us³yszy twoj± wiadomo¶æ, ale nie odpowie." NL, ch );

	if ( ( !IS_IMMORTAL(ch)
	&&     !IS_AWAKE(victim) )
	|| (   !IS_NPC(victim)
		&& IS_SET(victim->in_room->room_flags, ROOM_SILENCE ) ) )
	{
		act( PLAIN, "Twój rozmówca nie mo¿e ciê us³yszeæ.", ch, 0, victim, TO_CHAR );
		return;
	}

	if(switched_victim)
		victim = switched_victim;

	argument = drunk_speech( argument, ch );

	ch_printf( ch, COL_TELL "Mówisz %s" COL_TELL " '%s" COL_TELL "'" EOL,
	can_see( ch, victim ) ?
   does_knows(ch, victim) ? victim->przypadki[2] :
    format_char_attribute(victim,2) : "komu¶", argument );
	position		= victim->position;
	victim->position	= POS_STANDING;

	if ( (!IS_NPC(ch) || ch->speaking) )
	{
		int speakswell = UMIN(knows_language(victim, ch->speaking, ch),
					knows_language(ch, ch->speaking, victim));

		if ( speakswell < 85 )
			act( COL_TELL, "$n$0 mówi ci '$t'", ch, translate(speakswell,
				argument, ch->speaking->name), victim, TO_VICT );
		else
			act( COL_TELL, "$n$0 mówi ci '$t'", ch, argument, victim, TO_VICT );
	}
	else
		act( COL_TELL, "$n$0 mówi ci '$t'", ch, argument, victim, TO_VICT );


	if( !IS_NPC( ch ) && !IS_NPC( victim ) )
		add_to_last_tell( victim, ch, argument );

	victim->position	= position;
	victim->reply		= ch;
	ch->retell			= victim;

	if ( !IS_NPC (victim) && ( IS_SET (victim->act, PLR_AFK ) ) )
	{
		ch_printf( ch, "On%s jest w tej chwili AWAY." NL,
			SEX_SUFFIX__AO( victim ) );
		if( victim->pcdata && victim->pcdata->afk_reason[0] != '\0' )
			ch_printf( ch, FG_BLUE "(" FB_BLUE "%s" FG_BLUE ")" EOL, victim->pcdata->afk_reason );
		return;
	}

	if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
	{
		sprintf( buf, "%s (tell to) %s: %s",
		IS_NPC( ch     ) ? ch->przypadki[0] :     ch->name,
		IS_NPC( victim ) ? victim->przypadki[0] : victim->name,
		argument );
		append_to_file( LOG_FILE, buf );
	}

	mprog_tell_trigger( ch, argument, victim );
	return;
}

DEF_DO_FUN( tell )
{
	char			arg	[MAX_INPUT_LENGTH];
	CHAR_DATA		* victim;

	argument = one_argument( argument, arg );

	if ( ( victim = get_char_world( ch, arg ) ) == NULL
	|| ( IS_NPC(victim) && victim->in_room != ch->in_room )  )
	{
		send_to_char( "Nie ma tu nikogo takiego." NL, ch );
		return;
	}

	ch_tell( ch, victim, argument );
	return;
}

DEF_DO_FUN( reply )
{
	if( !ch->reply )
	{
		act( PLAIN, "Przecie¿ nikt nic do ciebie nie mówi³ ostatnio.", ch, NULL, NULL,
			TO_CHAR );
		return;
	}

	ch_tell( ch, ch->reply, argument );
	return;
}

// Added by Thanos: For Jot ;)
DEF_DO_FUN( retell )
{
	if( !ch->retell )
	{
		act( PLAIN, "Nie mówi³$a¶ ostatnio do nikogo przecie¿.", ch, NULL, NULL,
			TO_CHAR );
		return;
	}

	ch_tell( ch, ch->retell, argument );
	return;
}
//done

/* Thanos */
DEF_DO_FUN( ignore )
{
	char	arg	[MIL];

	if ( IS_NPC( ch ) )
	{
		send_to_char( "Jak jestes mobem to siedz cicho :P" NL, ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		if( ch->pcdata->ignorelist[0] != '\0' )
			ch_printf( ch, "Lista ignorowanych przez ciebie osób:" NL "%s" EOL,
				ch->pcdata->ignorelist );
		else
			ch_printf( ch, "Nie ignorujesz nikogo." NL );

		return;
	}

	one_argument( argument, arg );
	arg[0]	= UPPER( arg[0] );

	if ( is_name( arg, ch->pcdata->ignorelist ) )
	{
		char	buf	[MSL];

		strcpy( buf, ch->pcdata->ignorelist );
		STRDUP( ch->pcdata->ignorelist, cut_from_string( buf, arg ) );

		ch_printf( ch, "Imiê '%s" PLAIN "' usuniête z listy ignorowanych." NL, arg );
	}
	else
	{
		char	buf	[MSL];

		buf[0] = '\0';
		if( ch->pcdata->ignorelist )
		{
			strcat( buf, ch->pcdata->ignorelist );
			strcat( buf, " " );
		}
		strcat( buf, arg );
		STRDUP( ch->pcdata->ignorelist, buf );
		ch_printf( ch, "Imiê '%s" PLAIN "' dodane do listy ignorowanych." NL, arg );
	}
	return;
}

DEF_DO_FUN( emote )
{
	char buf[MAX_STRING_LENGTH];
	char *plast;
	int actflags;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Jak± emocjê chcesz okazaæ?" NL, ch );
		return;
	}

	actflags = ch->act;
	if ( IS_NPC( ch ) ) REMOVE_BIT( ch->act, ACT_SECRETIVE );
	for ( plast = argument; *plast != '\0'; plast++ );

	strcpy( buf, argument );
	if ( isalpha(plast[-1]) )
		strcat( buf, FG_CYAN "." );

	MOBtrigger = false;
	act( FG_CYAN, "$n $T", ch, NULL, buf, TO_ROOM );
	MOBtrigger = false;
	act( FG_CYAN, "$n $T", ch, NULL, buf, TO_CHAR );
	ch->act = actflags;
	if ( IS_SET( ch->in_room->room_flags, ROOM_LOGSPEECH ) )
	{
		sprintf( buf, "%s (emote): %s ",
			IS_NPC( ch ) ? ch->przypadki[0] : ch->name,
			argument );
		append_to_file( LOG_FILE, buf );
	}
	return;
}

DEF_DO_FUN( rent )
{
	huh( ch );
	return;
}

DEF_DO_FUN( qui )
{
	send_to_char( FB_RED "Je¶li chcesz wyj¶æ, napisz QUIT w ca³o¶ci." EOL, ch );
	return;
}

DEF_DO_FUN( quit )
{
	int		x, y;
	int		level;

	if ( IS_NPC(ch) )
		return;

	if ( ch->position == POS_FIGHTING )
	{
		send_to_char( "Nie ma szans! Przecie¿ walczysz." NL, ch );
		return;
	}

	if ( ch->position  < POS_STUNNED  )
	{
		send_to_char( "Jeszcze ¯YJESZ." NL, ch );
		return;
	}
	if ( !IS_IMMORTAL(ch) && !NOT_AUTHED(ch) && ch->in_room
		&& !IS_SET( ch->in_room->room_flags , ROOM_HOTEL ))
	{
		send_to_char("Nie mo¿esz wyj¶æ z gry tutaj." NL, ch);
		send_to_char("Musisz znale¼æ bezpieczne miejsce dla twojej postaci takie jak np. hotel..." NL, ch);
		send_to_char("Mo¿e wezwij sobie taksówkê (wpisz help HAIL)" NL, ch);
		return;
	}
	if ( IS_SET( ch->in_room->room_flags , ROOM_EMPTY_HOME ) )
	{
		send_to_char("Nie mo¿esz wyj¶æ gry w miejscu, które mo¿e staæ siê czyim¶ mieszkaniem." NL, ch);
		return;
	}
	if ( IS_SET( ch->in_room->room_flags , ROOM_PLR_HOME ) &&
		 ch->plr_home != ch->in_room )
	{
		send_to_char("Nie mo¿esz wyj¶æ z gry w cudzym mieszkaniu!" NL, ch);
		return;
	}

	uncrew( ship_from_room(ch->in_room) ,ch);
	send_to_char( FB_WHITE
	"Otoczenie zmienia siê mieni±c milionami barw i odcieni otaczaj±c twoje cia³o..." NL
	"Kiedy siê budzisz, wiesz, ¿e nic nie bêdzie ju¿ takie samo." EOL, ch );

	act( COL_TELL,
	"Tajemniczy g³os mówi ci 'Oczekujemy twojego powrotu $n...'", ch, NULL, NULL, TO_CHAR );
	act( PLAIN, "$n opu¶ci³$o grê.", ch, NULL, NULL, TO_ROOM );

	sprintf( log_buf, "%s has quit.", ch->name );
	quitting_char = ch;
	save_char_obj( ch );

	save_home(ch);
	saving_char = NULL;
	level = get_trust(ch);
	/*
	* After extract_char the ch is no longer valid!
	*/
	extract_char( ch, true );
	for ( x = 0; x < MAX_WEAR; x++ )
		for ( y = 0; y < MAX_LAYERS; y++ )
			save_equipment[x][y] = NULL;

	/* don't show who's logging off to leaving player */
/*
	to_channel( log_buf, CHANNEL_MONITOR, "Monitor", level );
*/
	log_string_plus( log_buf, LOG_COMM, level );
	return;
}

DEF_DO_FUN( ansi )
{
	if( IS_NPC(ch) )
		return;

	if( !IS_SET( ch->act, PLR_ANSI ) )
	{
		SET_BIT(ch->act,PLR_ANSI);
		send_to_char( "Kolory "
		FB_YELLOW "A" FB_GREEN "N" FB_PINK "S" FB_BLUE "I "
		MOD_BLINK FB_GREEN "W£¡CZONE" RESET FG_BLUE "!" FB_WHITE "!" FB_RED "!" EOL, ch);
		send_to_char( "Ok." NL, ch);
		return;
	}
	else
	{
		send_to_char( PLAIN, ch);
			REMOVE_BIT(ch->act,PLR_ANSI);
		send_to_char( "Ok... kolory ANSI wy³±czone." NL, ch );
		return;
	}
}

DEF_DO_FUN( sound )
{
	if( IS_NPC(ch) )
		return;

	if( !IS_SET( ch->act, PLR_SOUND ) )
	{
		SET_BIT(ch->act,PLR_SOUND);
		send_to_char( FB_WHITE "D¬WIÊK W£¡CZONY, S£YSZYSZ MNIE?" EOL, ch);
		send_to_char( "!!SOUND(hopeknow)", ch);
		return;
	}
	else
	{
		REMOVE_BIT(ch->act,PLR_SOUND);
		send_to_char( "Ok... d¼wiêk wy³±czony (Ale cisza...)" NL, ch );
		return;
	}
}

DEF_DO_FUN( save )
{
	if ( IS_NPC(ch) )
	{
		send_to_char("Spoko. Ju¿ zapisane." NL, ch);
		return;
	}

	if ( NOT_AUTHED(ch) )
	{
		send_to_char("Nie mo¿esz zapisaæ postaci dopóki nie ukoñczysz szkolenia w akademii." NL, ch);
		return;
	}

	if ( !IS_SET( ch->affected_by, ch->race->affected ) )
		SET_BIT( ch->affected_by, ch->race->affected );
	if ( !IS_SET( ch->resistant, ch->race->resistant ) )
		SET_BIT( ch->resistant, ch->race->resistant );
	if ( !IS_SET( ch->susceptible, ch->race->susceptible ) )
		SET_BIT( ch->susceptible, ch->race->susceptible );

	save_char_obj( ch );
	save_home( ch );
	saving_char = NULL;

	send_to_char(IS_HERO(ch) ? "Ok. Zapamiêtywanie twojej wspania³ej egzystencji zakoñczone." NL
			: "Ok. Gromadzenie danych na temat twojej ¿a³osnej egzystencji zakoñczone." NL, ch);
	return;
}

/*
* Something from original DikuMUD that Merc yanked out.
* Used to prevent following loops, which can cause problems if people
* follow in a loop through an exit leading back into the same room
* (Which exists in many maze areas)			-Thoric
*/
bool circle_follow( CHAR_DATA *ch, CHAR_DATA *victim )
{
	CHAR_DATA *tmp;

	for ( tmp = victim; tmp; tmp = tmp->master )
		if ( tmp == ch )
			return true;
	return false;
}


DEF_DO_FUN( follow )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "I¶c za kim?" NL, ch );
		return;
	}

	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego." NL, ch );
		return;
	}

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master )
	{
		act( PLAIN, "Przecie¿ wolisz chodziæ za $N$4!", ch, NULL, ch->master, TO_CHAR );
		return;
	}

	if ( victim == ch )
	{
		if ( !ch->master )
		{
			send_to_char( "Ju¿ za sob± chodzisz." NL, ch );
			return;
		}
		stop_follower( ch );
		return;
	}

	if ( circle_follow( ch, victim ) )
	{
		send_to_char( "Jak ty sobie wyobra¿asz takie ³a¿enie za sob± ???" NL, ch );
		return;
	}

	if ( ch->master )
		stop_follower( ch );

	add_follower( ch, victim );
	return;
}

void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{
	IF_BUG( ch->master != NULL, "" )
		return;

	ch->master        = master;
	ch->leader        = NULL;

	if ( can_see( master, ch ) )
		act( COL_ACTION, "$n bêdzie teraz chodzi³$o za tob±.", ch, NULL, master, TO_VICT );

	act( COL_ACTION, "Bêdziesz teraz chodzi³$o za $N$4.",  ch, NULL, master, TO_CHAR );
	return;
}

void stop_follower( CHAR_DATA *ch )
{
	IF_BUG( ch->master==NULL, "" )
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) )
	{
		REMOVE_BIT( ch->affected_by, AFF_CHARM );
		affect_strip( ch, gsn_charm_person );
	}

	if ( can_see( ch->master, ch ) )
		act( COL_ACTION, "$n przesta³$o chodziæ za tob±.",     ch, NULL, ch->master, TO_VICT    );
	act( COL_ACTION, "Przesta³$a¶ chodziæ za $N$4.",      ch, NULL, ch->master, TO_CHAR    );

	ch->master = NULL;
	ch->leader = NULL;
	return;
}

void die_follower( CHAR_DATA *ch )
{
	CHAR_DATA *fch;

	if ( ch->master )
		stop_follower( ch );

	ch->leader = NULL;

	for ( fch = first_char; fch; fch = fch->next )
	{
		if ( fch->master == ch )
			stop_follower( fch );
		if ( fch->leader == ch )
			fch->leader = fch;
	}
	return;
}

DEF_DO_FUN( order )
{
	char		arg[MAX_INPUT_LENGTH];
	CHAR_DATA	* victim;
	CHAR_DATA	* och;
	CHAR_DATA	* och_next;
	bool		found;
	bool		fAll;

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' || argument[0] == '\0' )
	{
		send_to_char( "Rozkazaæ komu i co?" NL, ch );
		return;
	}

	if ( IS_AFFECTED( ch, AFF_CHARM ) || IS_AFFECTED( ch, AFF_DOMINATED ) )
	{
		send_to_char( "Wygl±da na to, ¿e ty s³uchasz, a nie wydajesz rozkazy." NL, ch );
		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		fAll   = true;
		victim = NULL;
	}
	else
	{
		fAll   = false;
		if ( ( victim = get_char_room( ch, arg ) ) == NULL )
		{
			send_to_char( "Nie ma tu nikogo takiego." NL, ch );
			return;
		}

		if ( victim == ch )
		{
			send_to_char( "Tak jest! W tej chwili!" NL, ch );
			return;
		}

		if ( (!IS_AFFECTED(victim, AFF_CHARM)
		&& !IS_AFFECTED(victim, AFF_DOMINATED) )
		|| victim->master != ch )
		{
			send_to_char( "Ty to zrób!" NL, ch );
			return;
		}
	}

	found = false;
	for ( och = ch->in_room->first_person; och; och = och_next )
	{
		och_next = och->next_in_room;

		if ( (IS_AFFECTED(och, AFF_CHARM) || IS_AFFECTED(och, AFF_DOMINATED) )
		&&   och->master == ch
		&& ( fAll || och == victim ) )
		{
			found = true;
			act( COL_ACTION, "$n rozkazuje ci '$t'.", ch, argument, och, TO_VICT );
			interpret( och, argument );
		}
	}

	if ( found )
	{
		send_to_char( "Ok." NL, ch );
		WAIT_STATE( ch, 12 );
	}
	else
		send_to_char( "Nie ma tu nikogo, kto s³ucha³by twoich rozkazów." NL, ch );
	return;
}

DEF_DO_FUN( group )
{
	char arg[MAX_INPUT_LENGTH];

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		CHAR_DATA *gch;
		CHAR_DATA *leader;

		leader = ch->leader ? ch->leader : ch;
		ch_printf( ch, FB_WHITE "Grupka %s:" EOL, PERS(leader, ch, 1) );

		/* Changed so that no info revealed on possess */
		for ( gch = first_char; gch; gch = gch->next )
		{
			if ( is_same_group( gch, ch ) )
			{
				if (IS_AFFECTED(gch, AFF_POSSESS))
				ch_printf( ch,
					"[%3d %-10s" PLAIN "] %-16s %4s/%4s hp %4s/%4s frc %4s/%4s mv" NL,
					gch->top_level,
					IS_NPC(gch) ? "Mob" : CH_RACE( gch, 0 ),	//Trog: przypadkiF
					capitalize( PERS(gch, ch, 0) ),
					"????", "????", "????", "????", "????", "????" );
				else
				{
					if( gch->skill_level[FORCE_ABILITY]>1 )
					ch_printf( ch,
						"[%3d %-10s" PLAIN "] %-16s %4d/%4d hp %4d/%4d frc %4d/%4d mv" NL,
						gch->top_level,
						IS_NPC(gch) ? "Mob" : CH_RACE( gch, 0), // Pixel: przypadkiF
						capitalize( PERS(gch, ch, 0) ),
						gch->hit,      gch->max_hit,
						gch->mana,     gch->max_mana,
						gch->move,     gch->max_move   );
					else
					ch_printf( ch,
						"[%3d %-10s" PLAIN "] %-16s %4d/%4d hp %4d/%4d mv" NL,
						gch->top_level,
						IS_NPC(gch) ? "Mob" : CH_RACE( gch, 0), // Pixel: przypadkiF
						capitalize( PERS(gch, ch, 0) ),
						gch->hit,      gch->max_hit,
						gch->move,     gch->max_move   );
				}
			}
		}
		return;
	}

	if ( !str_cmp( arg, "disband" ))
	{
		CHAR_DATA	* gch;
		int			count = 0;

		if ( ch->leader || ch->master )
		{
			send_to_char( "Nie mo¿esz rozdzieliæ grupy nie bêd±c jej liderem." NL, ch );
			return;
		}

		for ( gch = first_char; gch; gch = gch->next )
		{
			if ( is_same_group( ch, gch )
			&& ( ch != gch ) )
			{
				gch->leader = NULL;
				gch->master = NULL;
				count++;
				send_to_char( "Twoja grupa zosta³a rozdzielona." NL, gch );
			}
		}

		if ( count == 0 )
			send_to_char( "Nie masz grupki. Nici z rozdzielania." NL, ch );
		else
			send_to_char( "Twoja grupa zosta³a rozdzielona." NL, ch );

		return;
	}

	if ( !str_cmp( arg, "all" ) )
	{
		CHAR_DATA *rch;
		int count = 0;

		for ( rch = ch->in_room->first_person; rch; rch = rch->next_in_room )
		{
			if ( ch != rch
			&&   !IS_NPC( rch )
			&&   rch->master == ch
			&&   !ch->master
			&&   !ch->leader
			&&   !is_same_group( rch, ch ) )
			{
				rch->leader = ch;
				count++;
			}
		}

		if ( count == 0 )
			send_to_char( "Nie widzisz ¿adnych potencjalnych towarzyszy." NL, ch );
		else
		{
			act( COL_ACTION, "$n zak³ada grupkê.", ch, NULL, NULL, TO_ROOM );
			send_to_char( "Grupujesz swoich towarzyszy." NL, ch );
		}
		return;
	}

	CHAR_DATA *victim;
	if ( ( victim = get_char_room( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie ma tu nikogo takiego." NL, ch );
		return;
	}

	if ( ch->master || ( ch->leader && ch->leader != ch ) )
	{
		send_to_char( "Przecie¿ chodzisz za kim¶ innym!" NL, ch );
		return;
	}

	if ( victim->master != ch && ch != victim )
	{
		act( PLAIN, "$N nie chodzi za tob±.", ch, NULL, victim, TO_CHAR );
		return;
	}

	if ( is_same_group( victim, ch ) && ch != victim )
	{
		victim->leader = NULL;
		act( COL_ACTION, "$n usuwa $N$3 ze swojej grupki.",   ch, NULL, victim, TO_NOTVICT );
		act( COL_ACTION, "$n usuwa ciê ze swojej grupki.",  ch, NULL, victim, TO_VICT    );
		act( COL_ACTION, "Usuwasz $N$3 z grupki.", ch, NULL, victim, TO_CHAR    );
		return;
	}

	victim->leader = ch;
	act( COL_ACTION, "$n przyjmuje $N$3 do swojej grupki.", ch, NULL, victim, TO_NOTVICT );
	act( COL_ACTION, "$n przyjmuje ciê do swojej grupki.", ch, NULL, victim, TO_VICT    );
	act( COL_ACTION, "$N do³±cza do twojej grupki.", ch, NULL, victim, TO_CHAR    );
	return;
}

/*
* 'Split' originally by Gnort, God of Chaos.
*/
DEF_DO_FUN( split )
{
	char		buf	[MAX_STRING_LENGTH];
	char		arg	[MAX_INPUT_LENGTH];
	CHAR_DATA	* gch;
	int			members;
	int			amount;
	int			share;
	int			extra;

	one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Ile kredytek podzieliæ?" NL, ch );
		return;
	}

	amount = atoi( arg );

	if ( amount < 0 )
	{
		send_to_char( "Twojej grupie by siê to nie spodoba³o." NL, ch );
		return;
	}

	if ( amount == 0 )
	{
		send_to_char( "Rozdzielasz 0 kredytek. Jako¶ nikt tego nie zauwa¿y³. Mo¿e jeszcze raz?" NL, ch );
		return;
	}

	if ( ch->gold < amount )
	{
		send_to_char( "Tyle kasy to ty nie masz." NL, ch );
		return;
	}

	members = 0;
	for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
		if ( is_same_group( gch, ch ) )
			members++;


	if (( IS_SET(ch->act, PLR_AUTOGOLD)) && (members < 2))
		return;

	if ( members < 2 )
	{
		send_to_char( "Zatrzymaj sobie wszystko." NL, ch );
		return;
	}

	share = amount / members;
	extra = amount % members;

	if ( share == 0 )
	{
		send_to_char( "Nie k³opocz siê sk±pcu." NL, ch );
		return;
	}

	ch->gold -= amount;
	ch->gold += share + extra;

	ch_printf( ch,
		"Rozdzielasz %d kredytek. twój udzia³ wynosi %d." NL,
		amount, share + extra );

	sprintf( buf, "$n rozdziela %d kredytek. Twój udzia³ wynosi %d.",
		amount, share );

	for ( gch = ch->in_room->first_person; gch; gch = gch->next_in_room )
	{
		if ( gch != ch && is_same_group( gch, ch ) )
		{
			act( FG_YELLOW, buf, ch, NULL, gch, TO_VICT );
			gch->gold += share;
		}
	}
	return;
}

DEF_DO_FUN( gtell )
{
	CHAR_DATA	* gch;

	if( !get_comlink( ch ) )
	{
		send_to_char( "Nie masz urz±dzenia komunikacyjnego." NL, ch );
		return;
	}

	if ( argument[0] == '\0' )
	{
		send_to_char( "Co chcesz powiedzieæ do grupki?" NL, ch );
		return;
	}

	argument = drunk_speech( argument, ch );

	/*
	* Note use of send_to_char, so gtell works on sleepers.
	*/
/*    sprintf( buf, "%s tells the group '%s'.\n\r", ch->name, argument );*/
	for ( gch = first_char; gch; gch = gch->next )
	{
		if ( is_same_group( gch, ch ) )
		{
			/* Groups unscrambled regardless of clan language.  Other languages
			still garble though. -- Altrag */
	#ifndef SCRAMBLE
			if ( (!IS_NPC(ch) || ch->speaking) )
			{
				int speakswell = UMIN(knows_language(gch, ch->speaking, ch),
							knows_language(ch, ch->speaking, gch));

				if ( speakswell < 85 )
					ch_printf( gch, FB_CYAN "%s mówi grupie '%s'" RESET
						FB_CYAN "." EOL, ch->name, translate(speakswell,
						argument, ch->speaking->name) );
				else
					ch_printf( gch, FB_CYAN "%s mówi grupie '%s" RESET
 						FB_CYAN "'." EOL, ch->name, argument );
			}
			else
				ch_printf( gch, "%s mówi grupie '%s'." NL, ch->name, argument );
	#else
			if ( knows_language( gch, ch->speaking, gch )
			||  (IS_NPC(ch) && !ch->speaking) )
				ch_printf( gch, FB_CYAN "%s mówi grupie '%s'" RESET
					FB_CYAN "." EOL, ch->name, argument );
			else
			ch_printf( gch, FB_CYAN "%s mówi grupie '%s'" RESET
					FB_CYAN "." EOL, ch->name, scramble(argument, ch->speaking) );
	#endif
		}
	}

	return;
}

/*
* It is very important that this be an equivalence relation:
* (1) A ~ A
* (2) if A ~ B then B ~ A
* (3) if A ~ B  and B ~ C, then A ~ C
*/
bool is_same_group( CHAR_DATA *ach, CHAR_DATA *bch )
{
	if( ach->leader )
		ach = ach->leader;

	if( bch->leader )
		bch = bch->leader;

	return ach == bch;
}

/*
* this function sends raw argument over the AUCTION: channel
* I am not too sure if this method is right..
*/
void talk_auction (char *argument)
{
	DESCRIPTOR_DATA	* d;
	char			buf[MAX_STRING_LENGTH];
	CHAR_DATA		* original;

	sprintf (buf,"Aukcja: %s", argument); /* last %s to reset color */

	for (d = first_descriptor; d; d = d->next)
	{
		original = d->original ? d->original : d->character; /* if switched */

		if ((d->connected >= CON_PLAYING)
			&& !IS_SET(original->deaf,CHANNEL_AUCTION)
			&& !IS_SET(original->in_room->room_flags, ROOM_SILENCE)
			&& !NOT_AUTHED(original))
				act( COL_ACTION, buf, original, NULL, NULL, TO_CHAR );
	}
}

/*
* Language support functions. -- Altrag
* 07/01/96
*
* Modified to return how well the language is known 04/04/98 - Thoric
* Currently returns 100% for known languages... but should really return
* a number based on player's wisdom (maybe 50+((25-wisdom)*2) ?)
*/
int knows_language( CHAR_DATA *ch, LANG_DATA *language, CHAR_DATA *cch )
{
	if ( !IS_NPC(ch) && IS_IMMORTAL(ch) )
		return 100;

	if ( IS_NPC(ch) && !ch->first_klang ) /* No langs = knows all for npcs */
		return 100;

	/* everyone knows Basic */
	if ( language == lang_base )
		return 100;

	if ( !IS_NPC( ch ) )
	{
		KNOWN_LANG	*klang;

		/* Racial languages for PCs */
		if ( ch->race->language == language )
			return 100;

		if( (klang = find_klang( ch, language )) )
			return klang->learned;
	}

	return 0;
}

bool can_learn_lang( CHAR_DATA *ch, LANG_DATA *language )
{
	if ( language == find_lang( "clan" ) )
		return false;

	if ( IS_NPC(ch) || IS_IMMORTAL(ch) )
		return false;

	if ( ch->race->language == language )
		return false;

	if ( ch->speaking == language )
	{
		KNOWN_LANG	*klang = find_klang( ch, language );

		if( !klang || klang->learned >= 99 )
			return false;

		return true;
	}

	return false;
}


int countlangs( KNOWN_LANG *first_klang )
{
	KNOWN_LANG	*klang;
	int			numlangs = 0;

	FOREACH( klang, first_klang )
		numlangs++;

	return numlangs;
}

const char * const lang_names[] =
{
	"common",		"wookiee",		"twilek",
	"rodian",		"hutt",			"mon calamari",
	"noghri",		"ewok",			"ithorian",
	"gotal",		"devaronian",	"droid",
	"spiritual",	"magical",		"gamorrean",
	"god",			"ancient",		"jawa",
	"clan",			"adarian", 		"verpin",
	"defel",		"trandoshan",	"chadra-fan",
	"quarren",		"duinuogwuin",	"yuuzhan vong",
	""
};

//added by Thanos
const char * const lang_names_pl[] =
{
	"wspólny", 		"wookieech", 		"twileków",
	"rodiañski", 	"huttów",		 "mon calamariañski",
	"noghrich", 	"ewoków", 		"ithorian",
	"gotalski", 	"devaroñski", 		"droidów",
	"spiritual",	"magical", 		"gamorreañski",
	"god", 		"ancient",		 "jawów",
	"klanu", 		"adariañski", 		"verpiñski",
	"defeli", 		"trandoshañski", 	"chadra-fanów",
	"quarreñski", 	"duinuogwuiñski", 	"yuuzhan vongów",
	""
};

/* Trog */
bool can_speak( CHAR_DATA *ch, LANG_DATA *lang )
{
	/* nie-Twi'lek nie moze mowic w Lekku */
	if( str_cmp( ch->race->name, "Twi'lek" )
	&& !str_cmp( lang->name, "Lekku" ) )
		return false;

	/* Wookiee nie moze mowic w nie-Shyriiwook */
	if( !str_cmp( ch->race->name, "Wookiee" )
	&& str_cmp( lang->name, "Shyriiwook" ) )
		return false;

	/* nie-Wookiee nie moze mowic w Shyriiwook */
	if( str_cmp( ch->race->name, "Wookiee" )
	&& !str_cmp( lang->name, "Shyriiwook" ) )
		return false;

	/* nie-Verpine nie moze mowic w Verpidio */
	if( str_cmp( ch->race->name, "Verpine" )
	&& !str_cmp( lang->name, "Verpidio" ) )
		return false;

	/* nie-Droid nie moze mowic w Binarnym */
	if( !IS_SET( ch->act, ACT_DROID )
	&& !str_cmp( lang->name, "Binarny" ) )
		return false;

	return true;
}

DEF_DO_FUN( speak )
{
	LANG_DATA	*lang;
	char		arg[MIL];

	if ( !*argument )
	{
		do_languages( ch, (char*)"" );
		send_to_char( NL "Jakim jêzykiem chcesz siê od teraz pos³ugiwaæ?" NL, ch );
		return;
	}

	argument = one_argument(argument, arg );

	if( !(lang = find_lang( arg )) )
	{
		send_to_char( "Nikt nie s³ysza³ o takim jêzyku." NL, ch );
		return;
	}

	if ( !can_speak( ch, lang ) )
	{
		if( *lang->deny_text )
			ch_printf( ch, "%s" NL, lang->deny_text );
		else
			send_to_char( "Nie mo¿esz mówiæ w tym jêzyku. Mo¿e nie masz odpowiednich czê¶ci cia³a." NL, ch );
		return;
	}

	if( knows_language( ch, lang, ch ) )
	{
		ch->speaking = lang;
		ch_printf( ch,
			"Jêzyk " FB_YELLOW "%s" PLAIN " to mowa, któr± siê od teraz pos³ugujesz." NL,
			lang->name );
		return;
	}

	send_to_char( "Nie znasz tego jêzyka." NL, ch );
}

DEF_DO_FUN( languages )
{
	char	arg[MIL];
	LANG_DATA	*lang;
	KNOWN_LANG	*klang;

	if( IS_NPC( ch ) )
		return;

	argument = one_argument( argument, arg );
	if( *arg && !str_prefix( arg, "learn" )
	&& !IS_IMMORTAL(ch) && !IS_NPC(ch) )
	{
		CHAR_DATA	*sch;
		char		arg2[MIL];
		int			prct;

		argument = one_argument( argument, arg2 );
		if ( !*arg2 )
		{
			send_to_char( "Którego jêzyka chcesz siê nauczyæ?" NL, ch );
			return;
		}

		if( !(lang = find_lang( arg2 )) )
		{
			send_to_char( "Nikt nie s³ysza³ o takim jêzyku." NL, ch );
			return;
		}

		if( ((klang = find_klang( ch, lang )) && klang->learned >= 99)
		|| ch->race->language == lang )
		{
			act( PLAIN, "Przecie¿ ju¿ p³ynnie pos³ugujesz siê jêzykiem $t.", ch,
				lang->name, NULL, TO_CHAR );
			return;
		}

		for ( sch = ch->in_room->first_person; sch; sch = sch->next )
			if( IS_NPC(sch) && IS_SET(sch->act, ACT_SCHOLAR)
			&&  knows_language( sch, ch->speaking, ch )
			&&  knows_language( sch, lang, sch )
			&&	(!sch->speaking || knows_language( ch, sch->speaking, sch )) )
				break;

		if ( !sch
		/* Trog: ale te ponizsze blokady trzeba na flagach zrobic */
		|| !str_cmp( lang->name, "Lekku" )
		|| !str_cmp( lang->name, "Verpidio" )
		|| !str_cmp( lang->name, "Binarny" )
		|| !str_cmp( lang->name, "Mando'a" )
		|| !str_cmp( lang->name, "Olys Corellisi" ) )
		{
			send_to_char( "Nie ma tu nikogo, kto móg³by nauczyæ ciê tego jêzyka." NL, ch );
			return;
		}

		if ( ch->gold < 25 )
		{
			send_to_char( "Nauka jêzyków kosztuje 25 kredytek... nie masz tyle." NL, ch );
			return;
		}

		ch->gold -= 25;
		/* Max 12% (5 + 4 + 3) at 24+ int and 21+ wis. -- Altrag */
		prct = 5 + (get_curr_int(ch) / 6) + (get_curr_wis(ch) / 7);
		if( !(klang = find_klang( ch, lang )) )
		{
			klang = new_known_lang();
			LINK( klang, ch->first_klang, ch->last_klang, next, prev );
			klang->language = lang;

		}
		klang->learned += prct;
		klang->learned = URANGE( 0, klang->learned, 99 );
		ch->speaking = lang;

		if ( klang->learned == prct )
			act( PLAIN, "Zaczynasz poznawaæ jak piêkny jest jêzyk $t.", ch,
				lang->name, NULL, TO_CHAR );
		else if ( klang->learned < 60 )
			act( PLAIN, "Kontynuujesz naukê, a twój jêzyk $t jest coraz lepszy.",
				ch, lang->name, NULL, TO_CHAR );
		else if ( klang->learned < 60 + prct )
			act( PLAIN, "Czujesz, ¿e nie sprawia ci problemów taki jêzyk jak $t.",
				ch, lang->name, NULL, TO_CHAR );
		else if ( klang->learned < 99 )
			act( PLAIN, "Twoja znajomo¶æ mowy w jêzyku $t staje siê jeszcze wiêksza.",
				ch, lang->name, NULL, TO_CHAR );
		else
			act( PLAIN, "Jêzyk $t nie ma ju¿ przed tob± ¿adnych tajemnic!",
				ch, lang->name, NULL, TO_CHAR );
		return;
	}

	ch_printf( ch, "%s(100)" PLAIN FG_GREEN " %s" EOL,
		( ch->speaking == lang_base ) ? FB_WHITE : FG_GREEN,
		lang_base->name );

	FOREACH( lang, first_lang )
	{
		if( !(klang = find_klang( ch, lang )) )
			ch_printf( ch, "%s(  0)" PLAIN FG_GREEN " %s" EOL,
				( ch->speaking == lang ) ? FB_WHITE : FG_GREEN,
				lang->name );
		else
			ch_printf( ch , "%s(%3d)" PLAIN FG_GREEN " %s" EOL,
				( ch->speaking == lang )? FB_WHITE: FG_GREEN,
				klang->learned,	lang->name );
	}
}

DEF_DO_FUN( wartalk )
{
	if (NOT_AUTHED(ch))
	{
		huh( ch );
		return;
	}

	talk_channel( ch, argument, CHANNEL_WARTALK, "mówi", "powiedzieæ do klanu" );
	return;
}





