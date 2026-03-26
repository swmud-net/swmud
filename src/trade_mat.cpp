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
*			Materials for Trade				   *
****************************************************************************/
#include <algorithm>
#include "olc.h"

#define KEY( literal, field, value )		\
		if ( !str_cmp( word, literal) )		\
		{									\
				field = value;				\
				fMatch = true;				\
				break;						\
		}

int64 flag_lookup( const char *name, const struct flag_type *flag_table );


MATERIAL_DATA * get_material( char * name, material_type typ)
{
	for (auto* pMat : material_list)
		if ( !str_cmp( name, pMat->name) && pMat->type == typ )
			return pMat;

	return NULL;
}

void material_sort()
{
	material_list.sort([](const MATERIAL_DATA* a, const MATERIAL_DATA* b) {
		return strcmp(a->name, b->name) < 0;
	});
}

DEF_DO_FUN( matlist )
{
	int					number = 0;

	ch_printf( ch, NL FG_CYAN "Lista nierafinowanych rud:" NL
	"----------------------------------------"
	"----------------------------------------" NL);

	for (auto* pMat : material_list)
	{
		if (pMat->type != U_ORE_FORM)
			continue;
		number++;
		ch_printf( ch, PLAIN "%-15s%s", pMat->name,
			(number %4) ? "   " : NL);
	}
	if (!number)
		ch_printf( ch, PLAIN "          Nie ma �adnych nierafinowanych rud" NL);
	else ch_printf( ch, EOL );

	number = 0;
	ch_printf( ch, NL FG_CYAN "Lista rafinowanych rud:" NL
	"----------------------------------------"
	"----------------------------------------" NL);

	for (auto* pMat : material_list)
	{
		if (pMat->type != R_ORE_FORM)
			continue;
		number++;
		ch_printf( ch, PLAIN "%-15s%s", pMat->name,
			(number %4) ? "   " : NL);
	}
	if (!number)
		ch_printf( ch, PLAIN "Nie ma �adnych rafinowanych rud" NL);
	else ch_printf( ch, EOL );

	ch_printf( ch, NL FG_CYAN "Lista stop�w:" NL
	"----------------------------------------"
	"----------------------------------------" NL);
	number = 0;

	for (auto* pMat : material_list)
	{
		if (pMat->type != ALLOY_FORM)
			continue;
		number++;
		ch_printf( ch, PLAIN "%-15s%s", pMat->name,
			(number %4) ? "   " : NL);
	}
	if (!number)
		ch_printf( ch, PLAIN "Nie ma �adnych stop�w" NL);
	send_to_char( NL, ch );
}

DEF_DO_FUN( matstat )
{
	MATERIAL_DATA	* pMat = nullptr;

	for (auto* m : material_list)
		if (!strcmp(m->name,argument))
		{
			pMat = m;
			break;
		}
	if (!pMat)
	{
		ch_printf( ch, "Nie ma materia�u o takiej nazwie." NL);
		return;
	}

	ch_printf( ch, FG_CYAN "Name        : " PLAIN "%s" NL,
		pMat->name);
	ch_printf( ch, FG_CYAN "Type        : " PLAIN "%s" NL,
		bit_name( material_form_list, pMat->type));
	ch_printf( ch, FG_CYAN "Availablity : " PLAIN "%s" NL,
		bit_name( material_rarity_list, pMat->rarity));
	ch_printf( ch, FG_CYAN "Base pricea : " PLAIN "%lld" NL,
		pMat->price);
	ch_printf( ch, FG_CYAN "Min buy price  : " PLAIN "%lld " FG_CYAN
			"(" PLAIN "%lld %" FG_CYAN ")" NL,
			 pMat->min_price, pMat->price ?
			(int)(pMat->price - pMat->min_price) / pMat->price : 0);
	ch_printf( ch, FG_CYAN "Max sell price : " PLAIN "%lld " FG_CYAN
			"(" PLAIN "%lld %" FG_CYAN ")" NL,
			pMat->max_price, pMat->price ?
			(int)( pMat->max_price -   pMat->price) / pMat->price : 0);
	ch_printf( ch,	FG_CYAN "Can be mined : " PLAIN "%s" EOL, pMat->can_mine ?
	"Yes":"No");
	ch_printf( ch,	FG_CYAN "Desc : " PLAIN "%s" NL, pMat->desc);
}

void fread_material( FILE	* fp )
{
	MATERIAL_DATA	* pMat;
	const char		* word;
	bool			fMatch;

	pMat = new_material();
	material_list.push_back(pMat);
	for(;;)
	{
		word = feof( fp ) ? "End" : fread_word( fp );
		fMatch = false;

		switch( UPPER( word[0] ) )
		{
			case 'D':
				SKEY( "Desc",	pMat->desc,		fread_string( fp ) );
				break;
			case 'E':
				if ( !str_cmp( word, "End" ) )
					return;
				break;
			case 'M':
				KEY( "Max",		pMat->max_price,	fread_number64( fp ) );
				KEY( "Min",		pMat->min_price,	fread_number64( fp ) );
				KEY( "Mined",	pMat->can_mine,		fread_number( fp ) );
				break;
			case 'N':
				SKEY( "Name",	pMat->name,			fread_string( fp ) );
				break;
			case 'P':
				KEY( "Price",	pMat->price,		fread_number64( fp ) );
				break;
			case 'R':
				KEY( "Rarity",	pMat->rarity,		(material_rarity)fread_number( fp ) );
				break;
			case 'T':
				KEY( "Type",	pMat->type,			(material_type)fread_number( fp ) );
				break;
		}
		if ( !fMatch )
			bug( "no match : %s", word);
	}
}

void load_materials()
{
	FILE			* fp;
	char 			* word;
	char			letter;
	RESERVE_CLOSE;

	if ( ( fp = fopen ( MATERIAL_FILE, "r" ) ) == NULL )
	{
		bug("fopen");
		perror(MATERIAL_FILE);
		RESERVE_OPEN;
		return;
	}


	for(;;)
	{
		letter = fread_letter( fp );
		if ( letter != '#' )
		{
			bug("No # in #MATERIAL");
			break;
		}
		word = fread_word( fp );
		if ( !str_cmp( word, "MATERIAL" ) )
			fread_material( fp );
		else if ( !str_cmp( word, "END" ) )
			break;
		else
			bug( "bad section : %s ",word);
	}
	fclose( fp );

	RESERVE_OPEN;
	material_sort();
}

void save_materials()
{
	FILE			* fp;

	RESERVE_CLOSE;

	if ( ( fp = fopen ( MATERIAL_FILE, "w" ) ) == NULL )
	{
		bug("fopen");
		perror(MATERIAL_FILE);
		RESERVE_OPEN;
		return;
	}

	for (auto* pMat : material_list)
	{
		fprintf( fp, "#MATERIAL\n");
		fprintf( fp, "Name      %s~\n",pMat->name);
		fprintf( fp, "Desc		%s~\n",pMat->desc);
		fprintf( fp, "Type		%d\n",pMat->type);
		fprintf( fp, "Rarity	%d\n",pMat->rarity);
		fprintf( fp, "Price		%lld\n",pMat->price);
		fprintf( fp, "Min		%lld\n",pMat->min_price);
		fprintf( fp, "Max		%lld\n",pMat->max_price);
		fprintf( fp, "Mined		%d\n",pMat->can_mine?1:0);
		fprintf( fp, "End\n\n");
	}

	fprintf( fp,"#END\n");
	fclose( fp );

	RESERVE_OPEN;
}

bool material_create(CHAR_DATA *ch, char *argument)
{
	char			arg1[MSL];
	MATERIAL_DATA	* pMat;

	argument = one_argument(argument,arg1);

	if (arg1[0]!='\0')
	{
		pMat = new_material();
		material_list.push_back(pMat);
		STRDUP( pMat->name, arg1 );
	}
	else return false;
	ch->pcdata->line_nr     = 0;
	ch->desc->connected     = CON_MATEDITOR;
	ch->desc->olc_editing   = (void *) pMat;
	return true;
}

void matedit( DESCRIPTOR_DATA *d, char *argument )
{
 	CHAR_DATA			* ch = d->character;
 	MATERIAL_DATA		* pMat;
 	char				arg[MSL];
 	char				arg1[MSL];
 	char				arg2[MSL];
 	char				buf[MSL];
	int					value;

	strcpy( arg, argument );
	smash_tilde( argument );
	argument = one_argument( argument, arg1 );
	strcpy( arg2, argument );

	if ( !(pMat = (MATERIAL_DATA *) ch->desc->olc_editing ) )
	{
		bug("matedit: null Material");
		edit_done(ch, (char *)"");
	}

	if ( !str_prefix( arg1, "show" ) )
	{
		sprintf( buf, "%s", pMat->name);
		do_matstat( ch, buf);
		return;
	}
	GET_DONE();
	GET_HELP( arg2 );
	GET_CREATE(  material_create( ch, arg2 ) );

	if ( !str_prefix( arg1, "fold" ) )
	{
		save_materials();
		ch_printf( ch, "Material saved." NL);
		return;
	}
	if ( !str_prefix( arg1, "next" ) || !str_cmp( arg1, ">" ) )
	{
		auto it = std::find(material_list.begin(), material_list.end(), pMat);
		if ( it != material_list.end() && ++it != material_list.end() )
		{
			pMat = *it;
			ch->desc->olc_editing = (void *) pMat;
			sprintf( buf, "%s", pMat->name);
			do_matstat( ch, buf);
		}
		else
			ch_printf( ch, "Nie ma ju� �adnych wi�cej materia��w na li�cie");
		return;
	}
	if ( !str_prefix( arg1, "prev" ) || !str_cmp( arg1, "<") )
	{
		auto it = std::find(material_list.begin(), material_list.end(), pMat);
		if ( it != material_list.end() && it != material_list.begin() )
		{
			pMat = *--it;
			ch->desc->olc_editing = (void *) pMat;
			sprintf( buf, "%s", pMat->name);
			do_matstat( ch, buf);
		}
		else
			ch_printf( ch, "To jest pierwszy materia� na li�cie");
		return;
	}
	SET_STR( "name", pMat->name, arg2 );

	if ( !str_prefix( arg1, "desc" ) )
	{
		string_append( ch, &pMat->desc );
		return;
	}
	SET_INT("price",	pMat->price,		UMAX(0,atoi(arg2)));
	SET_INT("max",		pMat->max_price,	UMAX(0,atoi(arg2)));
	SET_INT("min",		pMat->min_price,	UMAX(0,atoi(arg2)));
	SET_INT("mined",	pMat->can_mine,		atoi(arg2) ? 1:0);
	SET_STR("name",		pMat->name,			arg2);

	if ( !str_prefix( arg1, "type" ) )
	{
		if ( !arg2[0] )
		{
			value = 0;
			while( str_cmp( bit_name( material_form_list, value ),"nieznany" ) )
			{
				ch_printf( ch, "%-20s",bit_name( material_form_list,
				value ) );
				value++;
			}
			send_to_char( NL EOL, ch );
			return;
		}
		else if ( flag_lookup( arg2, material_form_list) > NO_FLAG )
		{
			pMat->type = (material_type)flag_lookup( arg2, material_form_list);
			ch_printf( ch, "Material type set to : %s" NL, bit_name(
			material_form_list, pMat->type) ) ;
		}
		else
			ch_printf( ch, "There's no type which you selected");

		return;
	}
	if ( !str_prefix( arg1, "rarity" ) || !str_prefix( arg1,
			"availability" ) )
	{

		if ( !arg2[0] )
		{
			value = 0;
			while( str_cmp( bit_name( material_rarity_list, value ),"nieznany" ) )
			{
				ch_printf( ch, "%-20s",bit_name( material_rarity_list,
				value ) );
				value++;
			}
			send_to_char( NL EOL, ch );
			return;
		}
		else if ( flag_lookup( arg2, material_rarity_list) > NO_FLAG )
		{
			pMat->rarity = (material_rarity)flag_lookup( arg2, material_rarity_list);
			ch_printf( ch, "Material availability set to : %s" NL, bit_name(
			material_rarity_list, pMat->rarity) ) ;
		}
		else
			ch_printf( ch, "There's no type which you selected");
		return;
	}
	if ( !str_prefix( arg1, "remove" ) || !str_prefix( arg1, "delete") )
	{
		material_list.remove(pMat);
		free_material( pMat	);
		edit_done(ch, (char *)"" );
		return;
	}

	interpret( ch, arg );
}

DEF_DO_FUN( matedit )
{
	char			arg1[MSL];
	MATERIAL_DATA	* pMat;

	if (!*argument)
	{
		ch_printf( ch,	"Syntax:   matedit <nazwa>" NL
						"Syntax:   matedit create <nazwa>" NL);
		return;
	}

	argument = one_argument(argument,arg1);

	if ( !str_cmp (arg1,"create"))
	{
		if ( material_create( ch, argument))
			ch_printf( ch, "Editing material." NL);
		return;
	}
	else
	{
		pMat = nullptr;
		for( auto* m : material_list )
			if ( !str_prefix( arg1, m->name ) )
			{
				pMat = m;
				ch_printf( ch, "Editing material." NL);
				break;
			}
		if (!pMat)
		{
			ch_printf( ch, "No material with that name." NL);
			return;
		}
	}
	ch->pcdata->line_nr     = 0;
	ch->desc->connected     = CON_MATEDITOR;
	ch->desc->olc_editing   = (void *) pMat;
}
