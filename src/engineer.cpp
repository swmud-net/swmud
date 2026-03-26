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
*  		           Engineer skills module    			   *
****************************************************************************/


#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "mud.h"

std::list<PROJECT_DATA *>	project_list;

#define IF_ABORT				\
    if( ch->substate == SUB_TIMER_DO_ABORT )

#define PREWAIT( __time  )			\
    if( ch->substate != 1 )			\
{						\
	add_timer( ch, TIMER_DO_FUN, __time, skill_function((char *)__FUNCTION__), 1 );\
	STRDUP( ch->dest_buf,argument);		\
}						\
    if( ch->substate == 1 )			\
{						\
        if ( *ch->dest_buf )			\
    	    STRDUP( ch->dest_buf, "" );		\
    	ch->substate = SUB_NONE;		\
}						\
    else

DEF_DO_FUN( identify )
{
	char 		arg[MAX_INPUT_LENGTH];
	OBJ_DATA 		*obj;
	int			chance;
	int			credits;

	IF_ABORT
	{
		break_skill( FB_RED "Przerywasz identyfikacj�." EOL, ch );
		return;
	}

	if( !check_knowledge( ch, gsn_identify ) )
		return;

	if ( !get_comlink( ch ) )
	{
		send_to_char(
				"Potrzebujesz urz�dzenia komunikacyjnego by to zrobi�!" NL, ch);
		return;
	}

	PREWAIT( 5 )
	{
		send_to_char( FB_GREEN "Wysy�asz przez komlink zapytanie..." EOL, ch);
		act( PLAIN, "$n wciska jakie� przyciski na komlinku.", ch, NULL, NULL , TO_ROOM );
		return;
	}

	argument = one_argument( argument, arg );

	if ( arg[0] == '\0' )
	{
		send_to_char( "Co chcesz zidentyfikowa�?" NL, ch );
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
	{
		send_to_char( "Nie masz takiego przedmiotu." NL, ch );
		return;
	}

	credits =  30 + 3*ch->top_level + obj->level/5;
	if( ch->gold < credits )
	{
		ch_printf( ch,
				   "Nie sta� ci� na zidentyfikowanie %s. Potrzebujesz %d kredytek." NL,
				   obj->przypadki[1], credits);
		return;
	}
	ch->gold -= UMIN( credits, ch->gold );
	ch_printf( ch,
			   "P�acisz %d kredyt%s za otrzymanie informacji o %s." NL,
			   credits,
			   NUMBER_SUFF( credits, "k�", "ki", "ek" ),
			   obj->przypadki[5] );

	chance = (int) (ch->pcdata->learned[gsn_identify]);

	if ( number_percent( ) >= chance )
	{
		send_to_char( FB_RED
				"Nie mo�esz nic wywnioskowa� z danych, jakie wy�wietla tw�j komlink." EOL,ch);
		learn_from_failure( ch, gsn_identify );
		return;
	}

	learn_from_success( ch, gsn_identify );
	show_obj_stats( obj, ch );
	return;
}

/*
DEF_DO_FUN( identify )
{
    char 		arg[MAX_INPUT_LENGTH];
    OBJ_DATA 		*obj;
    int			chance;
    int			credits;

    if( !check_knowledge( ch, gsn_identify ) )
	return;

    if ( !get_comlink( ch ) )
{
	send_to_char(
	    "Potrzebujesz urz�dzenia komunikacyjnego by to zrobi�!" NL, ch);
	return;
}

    PREWAIT( 5 )
{
	send_to_char( FB_GREEN "Wysy�asz przez komlink zapytanie..." EOL, ch);
	act( PLAIN, "$n wciska jakie� przyciski na komlinku.", ch, NULL, NULL , TO_ROOM );
	return;
}

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' )
{
	send_to_char( "Co chcesz zidentyfikowa�?" NL, ch );
	return;
}

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
{
	send_to_char( "Nie masz takiego przedmiotu." NL, ch );
	return;
}

    credits =  30 + 3*ch->top_level + obj->level/5;
    if( ch->gold < credits )
{
	ch_printf( ch,
	"Nie sta� ci� na zidentyfikowanie %s. Potrzebujesz %d kredytek." NL,
	    obj->przypadki[1], credits);
	return;
}
    ch->gold -= UMIN( credits, ch->gold );
    ch_printf( ch,
    "P�acisz %d kredyt%s za otrzymanie informacji o %s." NL,
	credits,
	NUMBER_SUFF( credits, "k�", "ki", "ek" ),
	obj->przypadki[5] );

    chance = (int) (ch->pcdata->learned[gsn_identify]);

    if ( number_percent( ) >= chance )
{
        send_to_char( FB_RED
	"Nie mo�esz nic wywnioskowa� z danych, jakie wy�wietla tw�j komlink." EOL,ch);
        learn_from_failure( ch, gsn_identify );
   	return;
}

    learn_from_success( ch, gsn_identify );
    show_obj_stats( obj, ch );
    return;
}
*/

DEF_DO_FUN( hack )
{
	char 		arg	[MAX_INPUT_LENGTH];
	int			chance;
	CHAR_DATA *		victim;

	if( !ch->desc || ch->desc->connected != CON_HOLONET )
	{
		huh( ch );
		return;
	}
	if( !check_knowledge( ch, gsn_hacking ) )
		return;

	one_argument( argument, arg );

	switch( ch->substate )
	{
		default:
			if ( arg[0] == '\0' )
			{
				send_to_char( "--> Holonet: Niesprecyzowany obiekt." NL, ch );
				return;
			}
			send_to_char( "--> Holonet: Pobieranie danych... Cierpliwo�ci." NL, ch);
			act( PLAIN, "$n wciska klawisze komputera mrucz�c co� pod nosem.", ch,
				 NULL, NULL , TO_ROOM );
			add_timer ( ch , TIMER_DO_FUN , 3, do_hack , 1 );
			STRDUP( ch->dest_buf,arg);
			return;

		case 1:
			if ( !*ch->dest_buf )
				return;
			strcpy(arg, ch->dest_buf);
			STRDUP( ch->dest_buf, "" );
			break;

	}

	ch->substate = SUB_NONE;

	if ( ( victim = get_char_world( ch, arg ) ) == NULL )
	{
		ch_printf( ch, "--> Holonet: Obiekt nie figuruje w bazie: %s." NL, arg );
		return;
	}

	if( victim->crimes.empty() || IS_NPC( victim ) )
	{
		ch_printf( ch,
				   "--> Holonet: Brak wskaza� obiektu '%s' na li�cie poszukiwanych." NL,
				   victim->przypadki[0] );
		return;
	}

	/* Najpierw sprawd�my, czy gracz w og�le b�dzie umia� si� w�ama� */
	chance = ch->pcdata->learned[gsn_hacking];
	if ( number_percent( ) >= chance )
	{
		if( number_percent( ) < 5 )
		{
			send_to_char( FB_RED
					"--> Holonet Alert!" EOL
					"--> Holonet: Intruz wykryty." 			 NL,ch);
			/* no c� */
			crime_to_char( ch, planet_list.front()->name, CRIME_HACKING );
		}
		else
			send_to_char( FB_RED
					"--> Holonet Alert!" EOL
					"--> Holonet: Kod dost�pu nieprawid�owy!" 		 NL,ch);

		learn_from_failure( ch, gsn_hacking );
		return;
	}

	/* Je�li offiara ma wy�szy od niego poziom, jest trudniej */
	if ( victim->top_level > ch->top_level + number_range( -10, 10 )
			&&  ch != victim)
	{
		if( number_percent( ) < 5 )
		{
			send_to_char( FB_RED
					"--> Holonet Alert!" EOL
					"--> Holonet: Niepoprawna identyfikacja u�ytkownika."  EOL,ch);
			/* no c� */
			crime_to_char( ch, planet_list.front()->name, CRIME_HACKING );
		}
		else
			send_to_char(
					"--> Holonet: Za niskie uprawnienia do edycji rejestru u�ytkownika." EOL, ch);

		learn_from_failure( ch, gsn_hacking );
		return;
	}

	/* Ok - uda�o si�, ale... */
	if( number_percent( ) < victim->crimes.front()->type )
	{
		send_to_char( FB_RED
				"--> Holonet Alert!" NL
				"--> Holonet: Wiadomo�� od Administratora: WYLOGUJ SI� NATYCHMIAST!" EOL,ch);
		/* no c� */
		crime_to_char( ch, planet_list.front()->name, CRIME_HACKING );
		return;
	}

	send_to_char( "--> Holonet: Wpis w rejestrze zaktualizowany pomy�lnie." NL, ch );
	crime_remove( victim,
				  victim->crimes.front()->planet,
				  victim->crimes.front()->type );

	learn_from_success( ch, gsn_hacking );
	return;
}




void holonet_done( CHAR_DATA *ch, char *argument )
{
	ch->desc->connected   = CON_PLAYING;
	send_to_char( "--> Holonet: po��czenie zerwane ze strony zdalnego hosta." NL, ch );
	act( PLAIN, "$n od��cza komputer z sieci.", ch, NULL, NULL, TO_ROOM );
	return;
}

void holonet_show( CHAR_DATA *ch )
{
	ch_printf( ch,
			   "o-----------------------------------------------------._____________ " NL
					   "|  H O L O N E T                            Galaktyczna Baza Danych |" NL
					   "|  Nieautoryzowane po��czenia ZABRONIONE.                           |" NL
					   "`-------------------------------------------------------------------'" NL );
	return;
}

void holonet( DESCRIPTOR_DATA *d, char *argument )
{
	CHAR_DATA *	ch	= d->character;
	char 	arg	[ MAX_STRING_LENGTH ];


    /*
	* Nie ma abort�w w holonecie
	* (chocia� �atwo je wstawi� wystarczy copy & paste from interp.c)
	*/
	if( get_timerptr( ch, TIMER_DO_FUN ) )
	{
		send_to_char( "--> Holonet: Konsola wykonuje ju� polecenie." NL, ch );
		return;
	}

	argument = one_argument( argument, arg );

	if ( !str_prefix( arg, "show" ) )
	{
		holonet_show( ch );
		return;
	}

	if ( !str_cmp( arg, "hack" ) )
	{
		do_hack( ch, argument );
		return;
	}

	if ( !str_prefix( arg, "exit" ) )
	{
		holonet_done( ch, (char *)"" );
		return;
	}
	send_to_char( "--> Holonet: Nieznane polecenie. U�yj 'exit' by wyj�� z systemu." NL, ch );
	return;
}

DEF_DO_FUN( holonet )
{
	OBJ_DATA *		obj;
	bool		laptop	= false;
	int			credits;
	int			chance;

	if( !check_knowledge( ch, gsn_holonet ) )
		return;

	for ( auto it = ch->carrying.rbegin(); it != ch->carrying.rend(); ++it )
	{
		obj = *it;
		if (obj->item_type == ITEM_COMPUTER)
		{
			laptop = true;
			break;
		}
	}

	switch( ch->substate )
	{
		default:
			if ( !get_comlink( ch ) )
			{
				send_to_char( "Potrzebujesz urz�dzenia komunikacyjnego by to zrobi�!" NL, ch);
				return;
			}

			if( !laptop )
			{
				send_to_char("Niby jak chcesz wpi�� si� do sieci nie maj�c komputera?" NL, ch);
				return;
			}

			if( !get_obj_here( ch, (char *)"terminal" ) )
			{
				send_to_char("Musisz jako� wpi�� si� do sieci. Nie widz� tu terminala" NL, ch);
				return;
			}

			credits =  10 + 2*ch->top_level + ch->top_level/5;

			if( ch->gold < credits )
			{
				ch_printf( ch, "Nie sta� ci� na po��czenie. Potrzebujesz %d kredyt%s." NL,
						   credits, NUMBER_SUFF( credits, "k�", "ki", "ek" ) );
				return;
			}

			ch->gold -= UMIN( credits, ch->gold );

			ch_printf( ch, "P�acisz %d kredyt%s za otrzymanie po��czenia." NL,
					   credits, NUMBER_SUFF( credits, "k�", "ki", "ek" ) );

			send_to_char( FB_GREEN "Nawi�zujesz po��czenie..." EOL, ch);
			act( PLAIN,
				 "$n wklepuje jakie� znaki na komputerze strasznie si� przy tym poc�c.", ch,
				 NULL, NULL, TO_ROOM );
			add_timer ( ch , TIMER_DO_FUN , 5 , do_holonet, 1 );
			return;

		case 1:
			break;

		case SUB_TIMER_DO_ABORT:
			break_skill( FB_RED "Roz��czasz si�." EOL, ch);
			act( PLAIN, "$n mocnym ruchem wyrywa komputer z sieci.", ch,
				 NULL, NULL, TO_ROOM );
			return;
	}
	ch->substate = SUB_NONE;

	chance = ch->pcdata->learned[gsn_holonet];

	ch_printf( ch,
			   "o-----------------------------------------------------._____________ " NL
					   "|                                                                   |" NL
					   "|  Witamy w Galaktycznej Bazie Danych   H O L O N E T               |" NL
					   "|  Nieautoryzowane po��czenia ZABRONIONE.                           |" NL
					   "|                                                                   |" NL
					   "|  Login: sw%d                                                    |" NL
					   "|  Kod wej�cia: *************                                       |" NL,
			   number_range( 1111, 9999 ) );


	if( number_percent( ) > chance )
	{
		ch_printf( ch,
				   "|  Kod wej�cia b��dny.                                              |" NL
						   "|                                                                   |" NL
						   "`-------------------------------------------------------------------|" NL );
		learn_from_failure( ch, gsn_holonet );
		send_to_char( FB_RED NL "Roz��czasz si�." EOL, ch);
		return;
	}

	ch_printf( ch,
			   "|  Kod wej�cia prawdi�owy.                                          |" NL
					   "|  Akceptuj� login.                                                 |" NL
					   "`-------------------------------------------------------------------|" NL );

	ch->desc->connected   = CON_HOLONET;

	learn_from_success( ch, gsn_holonet) ;
	gain_exp(ch, 30*ch->top_level, ENGINEERING_ABILITY);
	return;
}


/* Skill podobny do bashdoor, tylko wymaga �adunku wybuchowego */
DEF_DO_FUN( detonate )
{
	EXIT_DATA *	pexit;
	char       	arg 	[ MAX_INPUT_LENGTH ];

	if( !check_knowledge( ch, gsn_detonate ) )
		return;

	one_argument( argument, arg );

	switch( ch->substate )
	{
		default:

			if ( ch->fighting )
			{
				send_to_char( "Nie mo�esz przerwa� walki." NL, ch );
				return;
			}

			if ( arg[0] == '\0' )
			{
				send_to_char( "Kt�re drzwi chcesz wysadzi�?" NL, ch );
				return;
			}

			if ( ( pexit = find_door( ch, arg, true ) ) != NULL
							&&     !IS_SET( pexit->flags, EX_CLOSED) )
			{
				send_to_char( "Spokojnie. To jest ju� otwarte." NL, ch );
				return;
			}

			send_to_char( FB_GREEN "Ostro�nie zak�adasz �adunek..." EOL, ch);
			act( PLAIN, "$n uzbraja �adunek wybuchowy...", ch,
				 NULL, NULL, TO_ROOM );
			add_timer ( ch , TIMER_DO_FUN , 5 , do_detonate , 1 );
			STRDUP( ch->dest_buf,arg);
			return;

		case SUB_TIMER_DO_ABORT:
			ch_printf( ch, "Jeste� teraz skupion%s na detonowaniu!" NL,
					   SEX_SUFFIX_YAE( ch ) );
			ch->substate = SUB_TIMER_CANT_ABORT;
			return;

		case 1:
			if ( !*ch->dest_buf )
				return;
			strcpy( arg, ch->dest_buf);
			STRDUP( ch->dest_buf, "" );
			break;
	}
	ch->substate = SUB_NONE;

	if ( ( pexit = find_door( ch, arg, true ) ) != NULL )
	{
		ROOM_INDEX_DATA *	to_room;
		EXIT_DATA *		pexit_rev;
		int              	chance;
		char *			keyword;

		if ( IS_SET( pexit->flags, EX_SECRET ) )
			keyword = (char *)"�cian�";
		else
			keyword = pexit->keyword;

		if ( !IS_NPC(ch) )
			chance = ch->pcdata->learned[gsn_bashdoor] / 2;
		else
			chance = 90;

		if ( !IS_SET( pexit->flags, EX_BASHPROOF )
					&&   ch->move >= 15
					&&   number_percent( ) < ( chance + 4 * ( get_curr_wis( ch ) - 19 ) ) )
		{
			REMOVE_BIT( pexit->flags, EX_CLOSED );
			if( IS_SET( pexit->flags, EX_LOCKED ) )
				REMOVE_BIT( pexit->flags, EX_LOCKED );
			SET_BIT(    pexit->flags, EX_BASHED );

			act(COL_ACTION, "BoOm !!!  $d rozwalaj� si� pod wp�ywem explozji!",
				ch, NULL, keyword, TO_CHAR );
			act(COL_ACTION, "BoOm !!!  $d rozwalaj� si� pod wp�ywem explozji!",
				ch, NULL, keyword, TO_ROOM );

			/* :) */
			if( number_percent() < 50 )
				damage( ch, ch, ( ch->max_hit / 20 ), gsn_detonate );

			learn_from_success( ch, gsn_detonate );

			if ( (to_room = pexit->to_room) != NULL
						   &&   (pexit_rev = pexit->rexit) != NULL
						   &&    pexit_rev->to_room	== ch->in_room )
			{
				REMOVE_BIT( pexit_rev->flags, EX_CLOSED );
				if( IS_SET( pexit_rev->flags, EX_LOCKED ) )
					REMOVE_BIT( pexit_rev->flags, EX_LOCKED );
				SET_BIT(    pexit_rev->flags, EX_BASHED );

				for ( auto* rch : to_room->people )
				{
					act(COL_ACTION, "S�yszysz potworny HUK! $d Rozpadaj� si� na kawa�ki!",
						rch, NULL, pexit_rev->keyword, TO_CHAR );
				}
			}
		}
		else
		{
			act(COL_ACTION,
				"BoOm !!!  Eksplozja nie by�a w stanie rozwali� $d. Ale ciebie tak !!!",
				ch, NULL, keyword, TO_CHAR );
			act(COL_ACTION,
				"BoOm !!!  $n detonuje �adunek. $d pozostaj� nietkni�te, ale $e obrywa!",
				ch, NULL, keyword, TO_ROOM );
			damage( ch, ch, ( ch->max_hit / 10 ) + 10, gsn_detonate );
			learn_from_failure(ch, gsn_detonate);
		}
	}
	else
	{
		act(COL_ACTION,
			"BoOm !!! �ciana ani drgnie. Za to ty nie�le obrywasz !!!",
			ch, NULL, NULL, TO_CHAR );
		act(COL_ACTION,
			"BoOm !!!  $n detonuje �adunek. �ciana pozostaje nietkni�ta, ale $e obrywa.",
			ch, NULL, NULL, TO_ROOM );
		damage( ch, ch, ( ch->max_hit / 20 ) + 10, gsn_bashdoor );
		learn_from_failure(ch, gsn_detonate);
	}

	if ( !char_died( ch ) )
		for ( auto* gch : ch->in_room->people )
	{
		if ( IS_AWAKE( gch )
				   && !gch->fighting
				   && ( IS_NPC( gch ) && !IS_AFFECTED( gch, AFF_CHARM ) )
				   && ( ch->top_level - gch->top_level <= 4 )
				   && number_bits( 2 ) == 0 )
			multi_hit( gch, ch, TYPE_UNDEFINED );
	}

	return;
}

/* Tak, jesli dany przedmiot jest uzywany przez projekt */
bool is_project_part( PROJECT_DATA *proj, OBJ_DATA *obj )
{
    // no niestety to jest mozliwe, np. kiedy upgradujemy
    // tarcze, to potrzebujemy np. baterii i przedmiotu
    // o wear_fladze shield.
    // Wtedy powstaje np. tarcza silowa.
    // Ale sek w tym, ze po dodaniu baterii do tarczy silowej
    // powstanie ... tarcza silowa! Tak byc nie moze, dlatego:
    // komponent projektu nie moze byc jednoczesnie jego efektem.
	if( obj->pIndexData->vnum == proj->effect->vnum )
		return false;

	if( obj->pIndexData->vnum == OBJ_VNUM_UNFINISHED_PART )
	{
		for( auto* cont : obj->contents )
			if( !is_project_part( proj, cont ) )
				return false;

		return true;
	}
	else
	{
		for( auto* part : proj->parts )
			for( auto* comp : part->components )
		{
			if( comp->type == COMP_VNUM
						 &&  comp->nr == obj->pIndexData->vnum )
				return true;

			if( comp->type == COMP_TYPE
						 &&  comp->nr == obj->item_type )
				return true;

			if( comp->type == COMP_WEAR
						 &&  IS_SET( obj->wear_flags, comp->nr ) )
				return true;

		}
	}
	return false;
}


/* znajduje wszystkie projekty, kt�rych cz�ciami s� obj1 i obj2
   i nadpisuje eff ilo�ci� znalezionych projekt�w */
PROJECT_DATA *find_project( OBJ_DATA *obj1, OBJ_DATA *obj2, int *eff )
{
	PROJECT_DATA *	OkProj = 0;
	int			ok = 0;

    /*
	Sprawd�my, czy kombinacja dw�ch przedmiot�w jednoznacznie
	uto�samia si� z jednym tylko projektem, czy jest jeszcze
	'anonimowa'
	*/
	for( auto* pProj : project_list )
	{
		if( is_project_part( pProj, obj1 ) &&
				  is_project_part( pProj, obj2 ) )
		{
			OkProj=pProj;
			ok++;
		}
	}

	*eff = ok;

	return OkProj;
}

/* wrzuca sk�adniki do przemiotu wynikowego */
void obj_to_dest( OBJ_DATA *obj, OBJ_DATA *dest )
{
	separate_obj( obj );

	if( obj->carried_by )
		obj_from_char( obj );

	if( obj->in_obj )
		obj_from_obj( obj );

	if( obj->pIndexData->vnum == OBJ_VNUM_UNFINISHED_PART )
		empty_obj( obj, dest, NULL );
	else
		obj = obj_to_obj( obj, dest );

	return;
}

/* znajduje w przedmiocie wszystkie odpowiedniki danej cz�ci */
bool find_part( PART_DATA * part, OBJ_DATA *obj )
{
	int			i=0;

	for( auto* con : obj->contents )
		separate_obj( con );

	for( auto* con : obj->contents )
	{
		for( auto* comp : part->components )
		{
			if( ( comp->type == COMP_VNUM
						   && comp->nr == con->pIndexData->vnum )
						   || ( comp->type == COMP_TYPE
						   && comp->nr == con->item_type )
						   || ( comp->type == COMP_WEAR
						   && IS_SET( con->wear_flags, comp->nr ) ) )
			{
				i++;
				break;
			}
		}
	}

	if( i == part->quantity )
		return true;

	return false;
}

/* rozdziela skombinowane przedmioty w inwentory gracza */
void separate_inv( CHAR_DATA *ch )
{
	for( auto* obj : ch->carrying )
		if( obj->count )
			separate_obj( obj );
}

/* ��czy ze sob� dwa przedmioty (je�li si� dadz� po��czy�)
  i daje wynik graczowi */
void mont_objects( CHAR_DATA *ch, OBJ_DATA *obj1, OBJ_DATA *obj2 )
{
	OBJ_DATA *		tmpObj;
	OBJ_DATA *		dest;
	PROJECT_DATA *	proj;
	PROJECT_DATA *	pProj = nullptr;
	int			ok = 0;
	bool		found = false;

	/* szukamy projekt�w, kt�rych cz�ciami s� te, ��czone przez gracza */
	proj = find_project( obj1, obj2, &ok );
	if( !proj || !ok )
	{
		ch_printf( ch, "Z tego nie wyjdzie chyba nic ciekawego..." NL );
		return;
	}

	/* wszystko do tymczasowego przedmiotu */
	tmpObj = create_object(
			get_obj_index( OBJ_VNUM_UNFINISHED_PART ), ch->top_level );

	if( !tmpObj )
	{
		bug( "OBJ_VNUM_UNFINISHED_PART (#%d) doesn't exist!",
			 OBJ_VNUM_UNFINISHED_PART );
		ch_printf( ch, "Jako� nie udaje ci si� nic zmontowa�..." NL );
		return;
	}

	obj_to_dest( obj1, tmpObj );
	obj_to_dest( obj2, tmpObj );

    /* sprawdzamy, czy projekt jest juz skonczony
	(tzn we wnetrzu tymczasowego przedmiotu znajduja sie wszystkie
	elementy jakiegos projektu) */
	for( auto* p : project_list )
	{
		pProj = p;
		found = true;
		for( auto* part : pProj->parts )
		{
	    /* jesli w przedmiocie brak kt�rejkolwiek
			cz�ci, to znaczy, ze nie skonczylismy */
			if( !find_part( part, tmpObj ) )
			{
				found = false;
				break;
			}

			if( !found )
				break;
		}

		if( found )
			break;
	}

	if( found ) /* jesli skonczylismy */
	{
		dest = create_object( get_obj_index( pProj->effect->vnum ),
							  ch->top_level );

		empty_obj( tmpObj, dest, NULL );
		extract_obj( tmpObj );

		act( COL_ACTION,
			 "Z po��czenia powstaje $p!", ch, dest, NULL, TO_CHAR);
	}
	else
	{
		int 	i, gender;
		char	decl	[MSL];

	/* ok == 1 --> to znaczy, ze jest tylkoi jeden taki projekt,
		kt�ry mo�emy uzyska� montuj�c dany klamot */
		if( ok == 1 )
		{
			const char *	const inf[6][4]	=
			{
				{ "gotowe",  "gotowy",  "gotowa", "gotowe" },
				{ "gotowego","gotowego","gotowej","gotowych" },
				{ "gotowemu","gotowemu","gotowej","gotowym" },
				{ "gotowe",  "gotowy",  "gotow�", "gotowe" },
				{ "gotowym", "gotowym", "gotow�", "gotowymi" },
				{ "gotowym", "gotowym", "gotowej","gotowych" }
			};

			gender = URANGE(
					GENDER_NEUTRAL, proj->effect->gender, GENDER_PLURAL );

			for( i = 0; i<6; i++ )
			{
				sprintf( decl, "prawie %s %s",
						 inf[i][gender], proj->effect->przypadki[0] );
				STRDUP( tmpObj->przypadki[i], decl );
			}

			sprintf( decl, "prawie %s %s",
					 inf[0][gender],	proj->effect->przypadki[0] );
			STRDUP( tmpObj->name, decl );

			tmpObj->gender = gender;

			act( COL_ACTION,
				 "Otrzymujesz $p$3.", ch, tmpObj, NULL, TO_CHAR);
		}
		else
		{
			for( i = 0; i<6; i++ )
			{
				sprintf( decl, "%s z %s",
						 obj1->przypadki[i], obj2->przypadki[4] );
				STRDUP( tmpObj->przypadki[i], decl );
			}

			sprintf( decl, "%s %s",
					 obj1->przypadki[0], obj2->przypadki[0] );
			STRDUP( tmpObj->name, decl );

			tmpObj->gender = obj1->gender;

			act( COL_ACTION,
				 "Gotowe.", ch, NULL, NULL, TO_CHAR);
		}
		dest = tmpObj;
	}

	if( IS_SET( dest->wear_flags, ITEM_TAKE ) )
		obj_to_char( dest, ch );
	else
		obj_to_room( dest, ch->in_room );
	return;
}

char *show_part( PART_DATA *part )
{
	OBJ_INDEX_DATA *	objIndex;
	static char 	buf	[MSL];

	sprintf( buf, " %dszt. ", part->quantity );

	for( auto* comp : part->components )
	{
		switch( comp->type )
		{
			case COMP_VNUM:
				objIndex = get_obj_index( comp->nr );

				strcat( buf, objIndex->przypadki[1] );
				strcat( buf, " (" );
				strcat( buf, itoa( comp->nr ) );
				strcat( buf, ")" );
				break;
			case COMP_WEAR:
				strcat( buf, "dowolnego przedmiotu o wear_fladze: " );
				strcat( buf, flag_string( wear_flags_list, comp->nr ) );
				break;
			case COMP_TYPE:
				strcat( buf, "dowolnego przedmiotu typu: " );
				strcat( buf, bit_name( obj_types_list, comp->nr ) );
				break;
		}

		if( comp != part->components.back() )
			strcat( buf, ", lub " );
	}
	return buf;
}

DEF_DO_FUN( mont )
{
	char 		arg1	[MIL];
	char 		arg2	[MIL];
	OBJ_DATA *		obj1;
	OBJ_DATA *		obj2;

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if( !*arg1 || !*arg2 )
	{
		send_to_char( "Co chcesz po��czy�... I z czym?" NL, ch );

		for( auto* pro : project_list )
		{
			ch_printf( ch,
					   "Aby otrzyma� %s (%d) musisz %s ze sob�:" EOL,
					   pro->effect->przypadki[3],
					   pro->effect->vnum,
					   flag_string( mont_types_list, pro->montage_type ) );
			for( auto* part : pro->parts )
			{
				ch_printf( ch,
						   "%s" EOL,
						   show_part( part ) );
			}
			ch_printf( ch, EOL );
		}
		return;
	}

	if ( ms_find_obj(ch) )
		return;

	separate_inv( ch );
	if ( ( obj1 = get_obj_carry( ch, arg1 ) ) == NULL )
	{
		ch_printf( ch, "Nie masz �adnego %s!" NL, arg1 );
		return;
	}

	if ( ( obj2 = get_obj_carry( ch, arg2 ) ) == NULL )
	{
		ch_printf( ch, "Nie masz �adnego %s!" NL, arg2 );
		return;
	}

	if( obj1 == obj2 )
	{
		ch_printf( ch, "No co� ty? To ten sam przedmiot!" NL );
		return;
	}

	act( COL_ACTION,
		 "��czysz w skupieniu $p$3 z $P$4...", ch, obj1, obj2, TO_CHAR);
	act( COL_ACTION,
		 "$n ��czy w skupieniu $p$3 z $P$4...", ch, obj1, obj2, TO_ROOM);

	mont_objects( ch, obj1, obj2 );
	return;
}

void load_projects()
{
	FILE *	fp;
	char	buf	[MSL];
	PART_DATA *	part;

	if ( ( fp = fopen( PROJECTS_FILE, "r" ) ) == NULL )
	{
		log_string( "No Projects File - No means No :P" );
		return;
	}

	for( ; ; )
	{
		char 	letter;

		letter = feof( fp ) ? '$' : fread_letter( fp );

		if( letter == '*' )
		{
			fread_to_eol( fp );
			continue;
		}
		else
			if( letter == '$' )
				break;
		else
			if( letter == 'A' || letter == 'B' || letter == 'C'
						 ||  letter == 'D' || letter == 'E' || letter == 'F' )
		{
			int nr;
			OBJ_INDEX_DATA *	pObj;
			PROJECT_DATA *	project;

			if( (nr = fread_number( fp )) <= 0 )
			{
				log_string( "Some weak project found -- ignoring" );
				fread_to_eol( fp );
				continue;
			}

			if( (pObj = get_obj_index( nr )) == NULL )
			{
				sprintf( buf, "Load_project: effect_obj does not exist (vnum:%d)", nr );
				log_string( buf );
				fread_to_eol( fp );
				continue;
			}

			CREATE( project, PROJECT_DATA, 1 );
			project->effect 	 	= pObj;
			project->montage_type 	= letter;

			for( ; ; )
			{
				bool 	end = false;
				char	type;

				type = fread_letter( fp );
				switch( type )
				{

					default:
						sprintf( buf,
								 "Load_project: Unknown Character found '%c'", type );
						log_string( buf );
						type = ';';
						/*break;  -- NIE RUSZA� */

						case ';':					/* koniec */
							fread_to_eol( fp );
							end = true;
							break;

							case 'x':		   /* ile sztuk sk�adnika potrzeba */
								if( !project->parts.empty() )
									project->parts.back()->quantity = fread_number( fp );
								break;

					case '=':
						case '+':				/* komponenty i ich */
							case '/':				     /* substytuty  */
							{
								COMPONENT_DATA * comp;
								int		 lett;
								int		 comp_nr;

								lett = fread_letter( fp );
								if( lett != COMP_VNUM
															&&  lett != COMP_WEAR
															&&  lett != COMP_TYPE )
									lett =  COMP_VNUM;

								comp_nr = fread_number( fp );
								if( type != '/' || project->parts.empty() )
								{
									CREATE( part, PART_DATA, 1 );
									part->quantity = 1;
									project->parts.push_back( part );
								}
								part = project->parts.back();
								CREATE( comp, COMPONENT_DATA, 1 );
								comp->nr 	= comp_nr;
								comp->type 	= lett;
								part->components.push_back( comp );
								break;
							}
				}
				if( end )
					break;
			}


			if( !project->parts.empty() )
				project_list.push_back( project );
			else
			{
				DISPOSE( project );
				log_string( "Empty project found - deleting" );
			}
		}
		else
		{
			sprintf( buf,"Load_projects: No match letter: %c", letter );
			log_string( buf );
			fread_to_eol( fp );
		}
	}
	fclose( fp );
	if( !sysdata.silent )
		log_string( " Done projects" );
	return;
}

void save_projects( )
{
	FILE *		fp;

	RESERVE_CLOSE;
	if ( ( fp = fopen( PROJECTS_FILE, "w" ) ) == NULL )
	{
		log_string( "Cannot open project file for writing." );
		perror( PROJECTS_FILE );
		RESERVE_OPEN;
		return;
	}

	fprintf( fp,
			 "*\n"
					 "* Sk�adnia: <monta�><vnum_efektu> = <part> + <part> { + <part> };\n"
					 "* Monta�e:\n"
					 "* A - wkr�canie       B - lutowanie       C - spawanie\n"
					 "* D - sklejanie       E - wtykanie        F - nitowanie\n"
					 "* Sk�adnia parta: <komponent>{ /<komponent> }[ x<quantity> ] \n"
					 "* Sk�adnia komponentu: <litera><warto��>\n"
					 "* Litera:\n"
					 "* v -- wymagany jest odpowiedni vnum\n"
					 "* t -- wymagany jest typ\n"
					 "* w -- wymagana jest wear_flaga\n"
					 "* Warto��:\n"
					 "* Numer flagi, typu lub vnumu w zale�no�ci od Litery\n"
					 "*\n"
					 "* / -- odpowiednik,  x -- quantity (ilo�� sztuk)\n"
					 "*\n\n" );

	for( auto* pro : project_list )
	{
		if( !pro->effect )
			continue;

		if( pro->parts.empty() )
			continue;

		fprintf( fp, "%c%-6d", pro->montage_type, pro->effect->vnum );

		for( auto* part : pro->parts )
		{
			if( part->components.empty() )
				continue;

			for( auto* comp : part->components )
			{
				fprintf( fp, "%s%c%d",
					comp != part->components.front() ? "/" :
					part == pro->parts.front() ?  " = "  : " + ",
					comp->type,
					comp->nr );
			}

			if( part->quantity > 1 )
				fprintf( fp, "x%d", part->quantity );
		}
		fprintf( fp, "; * %s\n", pro->effect->name );
	}
	fprintf( fp, "$\n\n" );
	fclose( fp );
	RESERVE_OPEN;
	return;
}

