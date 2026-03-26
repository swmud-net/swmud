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
*                         XML saving/loading module                        *
****************************************************************************/

#include "swxml.h"

/* Trog: zapis/odczyt krain w formacie XML */

std::list<AREA_DATA*> uarea_list;

void save_area_list()
{
	xmlDocPtr	doc;
	xmlNodePtr	root;
	char		buf[MAX_INPUT_LENGTH];

	swXmlInitIO();
	doc = xmlNewDoc( BC"1.0" );
	root = xmlNewNode( NULL, BC"list" );
	xmlNewNs( root, BC"http://swmud.pl/ns/swmud/1.0/list", NULL );
	xmlNewNs( root, BC"http://www.w3.org/2001/XMLSchema-instance", BC"xsi" );
	swNewProp( root, "xsi:schemaLocation", "http://swmud.pl/ns/swmud/1.0/list list.xsd" );
	xmlDocSetRootElement( doc, root );
	for (auto* area : area_list)
		swNewChildText( root, NULL, "entry", area->filename );

	sprintf( buf, "Saving lists/%s...", basename( (char *)AREA_LISTXML ) );
	log_string_plus( buf, LOG_NORMAL, LEVEL_GREATER );

	RESERVE_CLOSE;
	xmlSaveFormatFileEnc( AREA_LISTXML, doc, "ISO-8859-2", 1 );
	RESERVE_OPEN;

	swXmlCleanIO();
	xmlFreeDoc( doc );
}

void save_area( AREA_DATA *area )
{
	xmlDocPtr	doc;
	xmlNodePtr	root;
	xmlNodePtr	snode;
	xmlNodePtr	node;
	xmlNodePtr	child;
	MID			*pMobIndex;
	OID			*pObjIndex;
	RID			*pRoomIndex;
	MPROG_DATA	*mprog;
	EDD			*ed;
	REQ			*req;
	AFF			*paf;
	ED			*xit;
	RD			*pReset;
	SHD			*pShop;
	RHD			*pRepair;
	char		buf[MSL];
	int			vnum;
	int			i;


	swXmlInitIO();
	doc = xmlNewDoc( BC"1.0" );
	root = xmlNewNode( NULL, BC"area" );
	xmlNewNs( root, BC"http://swmud.pl/ns/swmud/1.0/area", NULL );
	xmlNewNs( root, BC"http://www.w3.org/2001/XMLSchema-instance", BC"xsi" );
	swNewProp( root, "xsi:schemaLocation", "http://swmud.pl/ns/swmud/1.0/area area.xsd" );
	xmlDocSetRootElement( doc, root );

	/* head */
	node = xmlNewChild( root, NULL, BC"head", NULL );
	swNewChildText( node, NULL, "name", area->name );
	swNewChildText( node, NULL, "authors", area->author );
	swNewChildText( node, NULL, "builders", area->builder );
	swNewChildInt( node, NULL, "security", area->security );
	child = xmlNewChild( node, NULL, BC"vnums", NULL );
	swNewChildInt( child, NULL, "lvnum", area->lvnum );
	swNewChildInt( child, NULL, "uvnum", area->uvnum );
	swNewChildInt64( node, NULL, "flags", area->flags );
	child = xmlNewChild( node, NULL, BC"economy", NULL );
	swNewChildInt( child, NULL, "low", area->low_economy );
	swNewChildInt( child, NULL, "high", area->high_economy );
	child = xmlNewChild( node, NULL, BC"reset", NULL );
	swNewChildInt( child, NULL, "frequency", area->reset_frequency );
	swNewChildText( child, NULL, "message", area->resetmsg );
	child = xmlNewChild( node, NULL, BC"ranges", NULL );
	swNewChildInt( child, NULL, "low", area->low_range );
	swNewChildInt( child, NULL, "high", area->high_range );

	/* mobiles */
	snode = xmlNewChild( root, NULL, BC"mobiles", NULL );
	for( vnum = area->lvnum; vnum <= area->uvnum; vnum++ )
	{
		if( !(pMobIndex = get_mob_index( vnum )) )
			continue;

		node = xmlNewChild( snode, NULL, BC"mobile", NULL );
		swNewChildInt( node, NULL, "vnum", vnum );
		swNewChildText( node, NULL, "name", pMobIndex->player_name );

		child = xmlNewChild( node, NULL, BC"short", NULL );
		for( i = 0; i < 6; i++ )
		{
			sprintf( buf, "inflect%d", i );
			swNewChildText( child, NULL, buf, pMobIndex->przypadki[i] );
		}
		swNewChildText( node, NULL, "long", pMobIndex->long_descr );
		swNewChildText( node, NULL, "description", pMobIndex->description );
		swNewChildText( node, NULL, "race", pMobIndex->race->name );
		swNewChildInt( node, NULL, "level", pMobIndex->level );
		swNewChildInt64( node, NULL, "act", pMobIndex->act );
		swNewChildInt64( node, NULL, "affected", pMobIndex->affected_by );
		swNewChildInt( node, NULL, "alignment", pMobIndex->alignment );
		swNewChildInt( node, NULL, "sex", pMobIndex->sex );
		swNewChildInt( node, NULL, "credits", pMobIndex->gold );
		swNewChildInt( node, NULL, "position", pMobIndex->position );
		child = xmlNewChild( node, NULL, BC"sectiona", NULL );
		swNewChildInt( child, NULL, "str", pMobIndex->perm_str );
		swNewChildInt( child, NULL, "int", pMobIndex->perm_int );
		swNewChildInt( child, NULL, "wis", pMobIndex->perm_wis );
		swNewChildInt( child, NULL, "dex", pMobIndex->perm_dex );
		swNewChildInt( child, NULL, "con", pMobIndex->perm_con );
		swNewChildInt( child, NULL, "cha", pMobIndex->perm_cha );
		swNewChildInt( child, NULL, "lck", pMobIndex->perm_lck );
		
		child = xmlNewChild( node, NULL, BC"sections", NULL );
		swNewChildInt( child, NULL, "saving_poison_death", pMobIndex->saving_poison_death );
		swNewChildInt( child, NULL, "saving_wand", pMobIndex->saving_wand );
		swNewChildInt( child, NULL, "saving_para_petri", pMobIndex->saving_para_petri );
		swNewChildInt( child, NULL, "saving_breath", pMobIndex->saving_breath );
		swNewChildInt( child, NULL, "saving_spell_staff", pMobIndex->saving_spell_staff );

		child = xmlNewChild( node, NULL, BC"sectionr", NULL );
		swNewChildInt( child, NULL, "height", pMobIndex->height );
		swNewChildInt( child, NULL, "weight", pMobIndex->weight );
		swNewChildInt64( child, NULL, "speaks", 0/*pMobIndex->speaks*/ );
		swNewChildText( child, NULL, "speaking", pMobIndex->speaking->name );
		swNewChildInt( child, NULL, "numattacks", pMobIndex->numattacks );

		child = xmlNewChild( node, NULL, BC"sectionx", NULL );
		swNewChildInt( child, NULL, "hitroll", pMobIndex->hitroll );
		swNewChildInt( child, NULL, "damroll", pMobIndex->damroll );
		swNewChildInt64( child, NULL, "xflags", pMobIndex->xflags );
		swNewChildInt64( child, NULL, "resistant", pMobIndex->resistant );
		swNewChildInt64( child, NULL, "immune", pMobIndex->immune );
		swNewChildInt64( child, NULL, "susceptible", pMobIndex->susceptible );
		swNewChildInt64( child, NULL, "attacks", pMobIndex->attacks );
		swNewChildInt64( child, NULL, "defenses", pMobIndex->defenses );

		child = xmlNewChild( node, NULL, BC"sectiont", NULL );
		swNewChildInt( child, NULL, "thac0", pMobIndex->mobthac0 );
		swNewChildInt( child, NULL, "ac", pMobIndex->ac );
		swNewChildInt( child, NULL, "hitnodice", pMobIndex->hitnodice );
		swNewChildInt( child, NULL, "hitsizedice", pMobIndex->hitsizedice );
		swNewChildInt( child, NULL, "hitplus", pMobIndex->hitplus );
		swNewChildInt( child, NULL, "damnodice", pMobIndex->damnodice );
		swNewChildInt( child, NULL, "damsizedice", pMobIndex->damsizedice );
		swNewChildInt( child, NULL, "damplus", pMobIndex->damplus );

		child = xmlNewChild( node, NULL, BC"sectionv", NULL );
		swNewChildText( child, NULL, "vipflags", pMobIndex->s_vip_flags );

		swNewChildText( node, NULL, "dialog", pMobIndex->dialog_name );
		
		node = xmlNewChild( node, NULL, BC"programs", NULL );
		for (auto* mprog : pMobIndex->mudprogs)
		{
			child = xmlNewChild( node, NULL, BC"program", NULL );
			swNewChildText( child, NULL, "type", mprog_type_to_name( mprog->type ) );
			swNewChildText( child, NULL, "args", mprog->arglist );
			swNewChildText( child, NULL, "comlist", mprog->comlist );
		}
	}

	/* objects */
	snode = xmlNewChild( root, NULL, BC"objects", NULL );
	for( vnum = area->lvnum; vnum <= area->uvnum; vnum++ )
	{
		if( !(pObjIndex = get_obj_index( vnum )) )
			continue;

		node = xmlNewChild( snode, NULL, BC"object", NULL );
		swNewChildInt( node, NULL, "vnum", vnum );
		swNewChildText( node, NULL, "name", pObjIndex->name );

		child = xmlNewChild( node, NULL, BC"short", NULL );
		for( i = 0; i < 6; i++ )
		{
			sprintf( buf, "inflect%d", i );
			swNewChildText( child, NULL, buf, pObjIndex->przypadki[i] );
		}
		swNewChildText( node, NULL, "description", pObjIndex->description );
		swNewChildText( node, NULL, "actiondesc", pObjIndex->action_desc );

		swNewChildInt( node, NULL, "type", pObjIndex->item_type );
		swNewChildInt64( node, NULL, "extraflags", pObjIndex->extra_flags );
		swNewChildInt64( node, NULL, "wearflags", pObjIndex->wear_flags );
		swNewChildInt( node, NULL, "layers", pObjIndex->layers );

		child = xmlNewChild( node, NULL, BC"values", NULL );
		switch( pObjIndex->item_type )
		{
			case ITEM_PILL:
			case ITEM_POTION:
			case ITEM_DEVICE:
				swNewChildInt( child, NULL, "value0", pObjIndex->value[0] );
				swNewChildInt( child, NULL, "value1", pObjIndex->value[1] );
				swNewChildInt( child, NULL, "value2", pObjIndex->value[2] );
				swNewChildText( child, NULL, "value3", IS_VALID_SN(pObjIndex->value[3])
					? skill_table[pObjIndex->value[3]]->name : " " );
				swNewChildInt( child, NULL, "value4", pObjIndex->value[4] );
				swNewChildInt( child, NULL, "value5", pObjIndex->value[5] );
				break;

			case ITEM_SALVE:
				swNewChildInt( child, NULL, "value0", pObjIndex->value[0] );
				swNewChildInt( child, NULL, "value1", pObjIndex->value[1] );
				swNewChildInt( child, NULL, "value2", pObjIndex->value[2] );
				swNewChildInt( child, NULL, "value3", pObjIndex->value[3] );
				swNewChildText( child, NULL, "value4", IS_VALID_SN(pObjIndex->value[4])
					? skill_table[pObjIndex->value[4]]->name : " " );
				swNewChildText( child, NULL, "value5", IS_VALID_SN(pObjIndex->value[5])
					? skill_table[pObjIndex->value[5]]->name : " " );
				break;

			default:
				for( i = 0; i < 6; i++ )
				{
					sprintf( buf, "value%d", i );
					swNewChildInt( child, NULL, buf, pObjIndex->value[i] );
				}
		}

		swNewChildInt( node, NULL, "weight", pObjIndex->weight );
		swNewChildInt( node, NULL, "cost", pObjIndex->cost );
		swNewChildInt( node, NULL, "gender", pObjIndex->gender );
		swNewChildInt( node, NULL, "level", pObjIndex->level );
		child = xmlNewChild( node, NULL, BC"extradescs", NULL );
		for (auto* ed : pObjIndex->extradesc)
		{
			xmlNodePtr	gchild = xmlNewChild( child, NULL, BC"extradesc", NULL );
			
			swNewChildText( gchild, NULL, "keyword", ed->keyword );
			swNewChildText( gchild, NULL, "description", ed->description );
		}
		child = xmlNewChild( node, NULL, BC"requirements", NULL );
		for (auto* req : pObjIndex->requirements)
		{
			xmlNodePtr	gchild = xmlNewChild( child, NULL, BC"requirement", NULL );
			
			swNewChildInt( gchild, NULL, "location", req->location );
			swNewChildInt( gchild, NULL, "modifier", req->modifier );
			swNewChildText( gchild, NULL, "type",
				(req->location == REQ_SKILL && IS_VALID_SN( req->type ))
				? skill_table[req->type]->name : " " );
		}
		child = xmlNewChild( node, NULL, BC"affects", NULL );
		for (auto* paf : pObjIndex->affects)
		{
			xmlNodePtr	gchild = xmlNewChild( child, NULL, BC"affect", NULL );
			
			swNewChildInt( gchild, NULL, "location", paf->location );
			swNewChildInt( gchild, NULL, "modifier",
				((paf->location == APPLY_WEAPONSPELL
				|| paf->location == APPLY_WEARSPELL
				|| paf->location == APPLY_REMOVESPELL
				|| paf->location == APPLY_STRIPSN)
				&& IS_VALID_SN(paf->modifier))
				? skill_table[paf->modifier]->slot : paf->modifier );
		}
		node = xmlNewChild( node, NULL, BC"programs", NULL );
		for (auto* mprog : pObjIndex->mudprogs)
		{
			child = xmlNewChild( node, NULL, BC"program", NULL );
			swNewChildText( child, NULL, "type", mprog_type_to_name( mprog->type ) );
			swNewChildText( child, NULL, "args", mprog->arglist );
			swNewChildText( child, NULL, "comlist", mprog->comlist );
		}
	}

	/* rooms */
	snode = xmlNewChild( root, NULL, BC"rooms", NULL );
	for( vnum = area->lvnum; vnum <= area->uvnum; vnum++ )
	{
		if( !(pRoomIndex = get_room_index( vnum )) )
			continue;

		node = xmlNewChild( snode, NULL, BC"room", NULL );
		swNewChildInt( node, NULL, "vnum", vnum );
		swNewChildText( node, NULL, "name", pRoomIndex->name );

		swNewChildText( node, NULL, "description", pRoomIndex->description );
		if( IS_SET( area->flags, AFLAG_NIGHTDESCS ) )
			swNewChildText( node, NULL, "nightdesc", pRoomIndex->nightdesc );
		swNewChildInt( node, NULL, "light", pRoomIndex->light );
		swNewChildInt64( node, NULL, "flags", pRoomIndex->room_flags );
		swNewChildInt( node, NULL, "sector", pRoomIndex->sector_type );
		swNewChildInt( node, NULL, "teledelay", pRoomIndex->tele_delay );
		swNewChildInt( node, NULL, "televnum", pRoomIndex->tele_vnum );
		swNewChildInt( node, NULL, "tunnel", pRoomIndex->tunnel );

		child = xmlNewChild( node, NULL, BC"exits", NULL );
		for (auto* xit : pRoomIndex->exits)
		{
			xmlNodePtr	gchild;

			if( IS_SET( xit->orig_flags, EX_PORTAL ) )
				continue;

			gchild = xmlNewChild( child, NULL, BC"exit", NULL );
			swNewChildInt( gchild, NULL, "direction", xit->vdir );
			swNewChildText( gchild, NULL, "description", xit->description );
			swNewChildText( gchild, NULL, "keyword", xit->keyword );
			swNewChildInt64( gchild, NULL, "flags", xit->orig_flags & ~EX_BASHED );
			swNewChildInt( gchild, NULL, "key", xit->key );
			swNewChildInt( gchild, NULL, "vnum", xit->vnum );
			swNewChildInt( gchild, NULL, "distance", xit->distance );
		}

		child = xmlNewChild( node, NULL, BC"extradescs", NULL );
		for (auto* ed : pRoomIndex->extradesc)
		{
			xmlNodePtr	gchild = xmlNewChild( child, NULL, BC"extradesc", NULL );
			
			swNewChildText( gchild, NULL, "keyword", ed->keyword );
			swNewChildText( gchild, NULL, "description", ed->description );
		}
		node = xmlNewChild( node, NULL, BC"programs", NULL );
		for (auto* mprog : pRoomIndex->mudprogs)
		{
			child = xmlNewChild( node, NULL, BC"program", NULL );
			swNewChildText( child, NULL, "type", mprog_type_to_name( mprog->type ) );
			swNewChildText( child, NULL, "args", mprog->arglist );
			swNewChildText( child, NULL, "comlist", mprog->comlist );
		}
	}

	/* resets */
	node = xmlNewChild( root, NULL, BC"resets", NULL );
	for( vnum = area->lvnum; vnum <= area->uvnum; vnum++ )
	{
		if( !(pRoomIndex = get_room_index( vnum )) )
			continue;

		for (auto* pReset : pRoomIndex->resets)
		{
			child = xmlNewChild( node, NULL, BC"reset", NULL );

			switch( pReset->command )
			{
				case 'm': case 'M':
				case 'o': case 'O':
				case 'p': case 'P':
				case 'e': case 'E':
				case 'd': case 'D':
				case 't': case 'T':
				case 'c': case 'C':
					sprintf( buf, "%c", UPPER( pReset->command ) );
					swNewChildText( child, NULL, "command", buf );
					swNewChildInt( child, NULL, "extra", pReset->extra );
					swNewChildInt64( child, NULL, "arg1", pReset->arg1 );
					swNewChildInt64( child, NULL, "arg2", pReset->arg2 );
					swNewChildInt64( child, NULL, "arg3", pReset->arg3 );
					swNewChildInt64( child, NULL, "arg4", pReset->arg4 );
					break;
				case 'g': case 'G':
				case 'r': case 'R':
					sprintf( buf, "%c", UPPER( pReset->command ) );
					swNewChildText( child, NULL, "command", buf );
					swNewChildInt( child, NULL, "extra", pReset->extra );
					swNewChildInt64( child, NULL, "arg1", pReset->arg1 );
					swNewChildInt64( child, NULL, "arg2", pReset->arg2 );
					swNewChildInt64( child, NULL, "arg3", pReset->arg3 );
					swNewChildInt64( child, NULL, "arg4", pReset->arg4 );
					break;

				default:
					bug( "no such reset: %c", pReset->command );
					xmlUnlinkNode( child );
					xmlFreeNode( child );
			}
		}
	}

	/* shops */
	snode = xmlNewChild( root, NULL, BC"shops", NULL );
	for( vnum = area->lvnum; vnum <= area->uvnum; vnum++ )
	{
		xmlNodePtr	comment;
		
		if( !(pMobIndex = get_mob_index( vnum ))
		|| !(pShop = pMobIndex->pShop) )
			continue;

		node = xmlNewChild( snode, NULL, BC"shop", NULL );
		comment = swNewComment( pMobIndex->przypadki[0] );
		xmlAddChild( node, comment );
		swNewChildInt( node, NULL, "keeper", pShop->keeper );
		child = xmlNewChild( node, NULL, BC"types", NULL );
		for( i = 0; i < MAX_TRADE; i++ )
		{
			sprintf( buf, "type%d", i );
			/* Bylo pShop->buy_type[0], przez co sie wszyscy
			 * wkurzali bo spredawca po reboocie kupowal tylko
			 * jeden typ itemow zamias pieciu  - Ganis */
			swNewChildInt( child, NULL, buf, pShop->buy_type[i] );
		}
		swNewChildInt( node, NULL, "profitbuy", pShop->profit_buy );
		swNewChildInt( node, NULL, "profitsell", pShop->profit_sell );
		swNewChildInt( node, NULL, "open", pShop->open_hour );
		swNewChildInt( node, NULL, "close", pShop->close_hour );
		swNewChildInt64( node, NULL, "flags", pShop->flags );
	}
		
	/* repair shops */
	snode = xmlNewChild( root, NULL, BC"repairs", NULL );
	for( vnum = area->lvnum; vnum <= area->uvnum; vnum++ )
	{
		xmlNodePtr	comment;
		
		if( !(pMobIndex = get_mob_index( vnum ))
		|| !(pRepair = pMobIndex->rShop) )
			continue;

		node = xmlNewChild( snode, NULL, BC"repair", NULL );
		comment = swNewComment( pMobIndex->przypadki[0] );
		xmlAddChild( node, comment );
		swNewChildInt( node, NULL, "keeper", pRepair->keeper );
		child = xmlNewChild( node, NULL, BC"types", NULL );
		for( i = 0; i < MAX_FIX; i++ )
		{
			sprintf( buf, "type%d", i );
			/* bylo pRepair->fix_type[0], przez co sie wszyscy
			 * wkurzali, bo mud zapominal o innych fixach 
			 * Ganis */
			swNewChildInt( child, NULL, buf, pRepair->fix_type[i] );
		}
		swNewChildInt( node, NULL, "profitfix", pRepair->profit_fix );
		swNewChildInt( node, NULL, "shoptype", pRepair->shop_type );
		swNewChildInt( node, NULL, "open", pRepair->open_hour );
		swNewChildInt( node, NULL, "close", pRepair->close_hour );
	}

	/* specials */
	node = xmlNewChild( root, NULL, BC"specials", NULL );
	for( vnum = area->lvnum; vnum <= area->uvnum; vnum++ )
	{
		if( !(pMobIndex = get_mob_index( vnum ))
		|| !pMobIndex->spec_fun )
			continue;

		child = xmlNewChild( node, NULL, BC"special", NULL );
		swNewChildInt( child, NULL, "vnum", vnum );
		swNewChildText( child, NULL, "function", lookup_spec( pMobIndex->spec_fun ) );
		swNewChildText( child, NULL, "function2", lookup_spec( pMobIndex->spec_2 ) );
	}

	/* saving stuff */
	sprintf( buf, "Saving %s...", area->filename );
	log_string_plus( buf, LOG_NORMAL, LEVEL_GREATER );

	sprintf( buf, "%s%s", AREA_DIR, area->filename );
	{
		char	buf2[MSL];
		
		sprintf( buf2, "%s%s", AREA_BAK_DIR, area->filename );
		rename( buf, buf2 );
	}

	
	RESERVE_CLOSE;
	xmlSaveFormatFileEnc( buf, doc, "ISO-8859-2", 1 );
	RESERVE_OPEN;

	swXmlCleanIO();
	xmlFreeDoc( doc );
	swCleanupParser();
}

void swGetContentPrograms( std::list<MPROG_DATA*>& progs, int64 *progtypes, xmlNodePtr node )
{
	xmlNodePtr	child;
	xmlNodePtr	gchild;
	MPROG_DATA	*prog;
	char		*buf = NULL;
	bool		invalid;


	STRDUP( buf, "" );
	progs.clear();
	for (child = node->children; child; child = child->next) /* program */
	{
		EONLY( child );

		invalid = false;
		prog = new_mprog();
		for (gchild = child->children; gchild; gchild = gchild->next)
		{
			EONLY( child );

			if( !swXmlStrcmp( gchild->name, "type" ) )
			{
				swGetContent( &buf, gchild );
				if( (prog->type = mprog_name_to_type( buf )) == ERROR_PROG )
				{
					bug( "no such prog type: %s", buf );
					invalid = true;
					break;
				}

				/* TODO: obsluga in_file_prog */
			}
			else
			if( !swXmlStrcmp( gchild->name, "args" ) )
				swGetContent( &prog->arglist, gchild );
			else
			if( !swXmlStrcmp( gchild->name, "comlist" ) )
				swGetContent( &prog->comlist, gchild );
		}
		if( invalid )
			free_mprog( prog );
		else
		{
			progs.push_back( prog );
			*progtypes |= prog->type;
		}
	}
	STRFREE( buf );
}

void swGetContentExtraDescs( std::list<EDD*>& edlist, xmlNodePtr node )
{
	xmlNodePtr	child;
	xmlNodePtr	gchild;
	EDD			*ed;


	for (child = node->children; child; child = child->next) /* extradesc */
	{
		EONLY( child );

		ed = new_ed();
		edlist.push_back( ed );
		for (gchild = child->children; gchild; gchild = gchild->next)
		{
			EONLY( gchild );

			if( !swXmlStrcmp( gchild->name, "keyword" ) )
				swGetContent( &ed->keyword, gchild );
			else
			if( !swXmlStrcmp( gchild->name, "description" ) )
				swGetContent( &ed->description, gchild );
		}
	}
}

MID *get_mob_tmp( int vnum, AREA_DATA *area )
{
	MID		*pMobIndex;

	for (auto* pMobIndex : area->area_tmp->mobs)
		if( pMobIndex->vnum == vnum )
			return pMobIndex;

	return get_mob_index( vnum );
}

OID *get_obj_tmp( int vnum, AREA_DATA *area )
{
	OID		*pObjIndex;

	for (auto* pObjIndex : area->area_tmp->objs)
		if( pObjIndex->vnum == vnum )
			return pObjIndex;

	return get_obj_index( vnum );
}

RID *get_room_tmp( int vnum, AREA_DATA *area )
{
	RID		*pRoomIndex;

	for (auto* pRoomIndex : area->area_tmp->rooms)
		if( pRoomIndex->vnum == vnum )
			return pRoomIndex;

	return get_room_index( vnum );
}

xmlDocPtr xsd_validate( const char *what, const char *xsd, const char *dir,
	const char *filename )
{
	xmlSchemaParserCtxtPtr	ctxt;
	xmlSchemaPtr			schema;
	xmlSchemaValidCtxtPtr	vctxt;
	xmlDocPtr				doc;
	char					buf[MSL];


	RESERVE_CLOSE;
	if( !(ctxt = xmlSchemaNewParserCtxt( xsd )) )
	{
		bug( "cannot create schema parser context for: %s", xsd );
		RESERVE_OPEN;
		return NULL;
	}
	
	if( !(schema = xmlSchemaParse( ctxt )) )
	{
		bug( "cannot parse schema file: %s", xsd );
		xmlSchemaFreeParserCtxt( ctxt );
		RESERVE_OPEN;
		return NULL;
	}

	if( !(vctxt = xmlSchemaNewValidCtxt( schema )) )
	{
		bug( "cannot create validation context for: %s", xsd );
		xmlSchemaFreeParserCtxt( ctxt );
		xmlSchemaFree( schema );
		RESERVE_OPEN;
		return NULL;
	}

	sprintf( buf, "%s%s", dir, filename );
	if( !(doc = xmlReadFile( buf, NULL, 0 )) )
	{
		bug( "cannot read %s file: %s", what, filename );
		xmlSchemaFreeParserCtxt( ctxt );
		xmlSchemaFree( schema );
		xmlSchemaFreeValidCtxt( vctxt );
		RESERVE_OPEN;
		return NULL;
	}

	if( xmlSchemaValidateDoc( vctxt, doc ) )
	{
		bug( "%s (filename: %s) is not valid", what, filename );
		xmlSchemaFreeParserCtxt( ctxt );
		xmlSchemaFree( schema );
		xmlSchemaFreeValidCtxt( vctxt );
		xmlFreeDoc( doc );
		RESERVE_OPEN;
		return NULL;
	}
	RESERVE_OPEN;

	xmlSchemaFreeParserCtxt( ctxt );
	xmlSchemaFree( schema );
	xmlSchemaFreeValidCtxt( vctxt );

	return doc;
}

AREA_DATA *load_area2( const char *filename )
{
	xmlDocPtr				doc;
	xmlNodePtr				root;
	xmlNodePtr				snode;
	xmlNodePtr				node;
	xmlNodePtr				child;
	xmlNodePtr				gchild;
	xmlNodePtr				ggchild;
	AREA_DATA				*area;
	MID						*pMobIndex;
	OID						*pObjIndex;
	RID						*pRoomIndex;
	REQ						*req;
	AFF						*paf;
	ED						*xit;
	RD						*pReset;
	SHD						*pShop;
	RHD						*pRepair;
//	char					buf[MSL];
	int						vnum;


	swXmlInitIO();

	if( !(doc = xsd_validate( "area", SCHEMA_AREA, AREA_DIR, filename )) )
		return NULL;
	/*! tutaj jestesmy juz PEWNI, ze plik krainki jest poprawny */

	area = new_area();
	area->area_tmp = new_area_tmp();
	STRDUP( area->filename, filename );

	root = xmlDocGetRootElement( doc );

	for (snode = root->children; snode; snode = snode->next)
	{
		EONLY( snode );

		if( !swXmlStrcmp( snode->name, "head" ) )
		{
			for (node = snode->children; node; node = node->next)
			{
				EONLY( node );

				if( !swXmlStrcmp( node->name, "name" ) )
					swGetContent( &area->name, node );
				else
				if( !swXmlStrcmp( node->name, "authors" ) )
					swGetContent( &area->author, node );
				else
				if( !swXmlStrcmp( node->name, "builders" ) )
					swGetContent( &area->builder, node );
				else
				if( !swXmlStrcmp( node->name, "security" ) )
					swGetContentInt( &area->security, node );
				else
				if( !swXmlStrcmp( node->name, "vnums" ) )
				{
					for (child = node->children; child; child = child->next)
					{
						EONLY( child );

						if( !swXmlStrcmp( child->name, "lvnum" ) )
							swGetContentInt( &area->lvnum, child );
						else
						if( !swXmlStrcmp( child->name, "uvnum" ) )
							swGetContentInt( &area->uvnum, child );
					}
				}
				else
				if( !swXmlStrcmp( node->name, "flags" ) )
					swGetContentInt64( &area->flags, node );
				else
				if( !swXmlStrcmp( node->name, "economy" ) )
				{
					for (child = node->children; child; child = child->next)
					{
						EONLY( child );

						if( !swXmlStrcmp( child->name, "low" ) )
							swGetContentInt( &area->low_economy, child );
						else
						if( !swXmlStrcmp( child->name, "high" ) )
							swGetContentInt( &area->high_economy, child );
					}
				}
				else
				if( !swXmlStrcmp( node->name, "reset" ) )
				{
					for (child = node->children; child; child = child->next)
					{
						EONLY( child );

						if( !swXmlStrcmp( child->name, "message" ) )
							swGetContent( &area->resetmsg, child );
						else
						if( !swXmlStrcmp( child->name, "frequency" ) )
							swGetContentInt( &area->reset_frequency, child );
					}
				}
				else
				if( !swXmlStrcmp( node->name, "ranges" ) )
				{
					for (child = node->children; child; child = child->next)
					{
						EONLY( child );

						if( !swXmlStrcmp( child->name, "low" ) )
							swGetContentInt( &area->low_range, child );
						else
						if( !swXmlStrcmp( child->name, "high" ) )
							swGetContentInt( &area->high_range, child );
					}
				}
			}
		}
		else
		if( !swXmlStrcmp( snode->name, "mobiles" ) )
		{
			for (node = snode->children; node; node = node->next)
			{
				EONLY( node );

				pMobIndex = new_mob();
				for (child = node->children; child; child = child->next) /* mobile */
				{
					EONLY( child );

					if( !swXmlStrcmp( child->name, "vnum" ) ) {
						swGetContentInt( &pMobIndex->vnum, child );
						fread_nameslist( pMobIndex );
					}
					else
					if( !swXmlStrcmp( child->name, "name" ) )
						swGetContent( &pMobIndex->player_name, child );
					else
					if( !swXmlStrcmp( child->name, "short" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "inflect0" ) )
								swGetContent( &pMobIndex->przypadki[0], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect1" ) )
								swGetContent( &pMobIndex->przypadki[1], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect2" ) )
								swGetContent( &pMobIndex->przypadki[2], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect3" ) )
								swGetContent( &pMobIndex->przypadki[3], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect4" ) )
								swGetContent( &pMobIndex->przypadki[4], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect5" ) )
								swGetContent( &pMobIndex->przypadki[5], gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "long" ) )
						swGetContent( &pMobIndex->long_descr, child );
					else
					if( !swXmlStrcmp( child->name, "description" ) )
						swGetContent( &pMobIndex->description, child );
					else
					if( !swXmlStrcmp( child->name, "race" ) )
					{
						RACE_DATA	*race;
						char		*buf2 = NULL;
						
						STRDUP( buf2, "" );
						swGetContent( &buf2, child );
						if( !(race = find_race( buf2 )) )
							race = base_race;
						pMobIndex->race = race;
						STRFREE( buf2 );
					}
					else
					if( !swXmlStrcmp( child->name, "level" ) )
						swGetContentInt( &pMobIndex->level, child );
					else
					if( !swXmlStrcmp( child->name, "act" ) )
						swGetContentInt64( &pMobIndex->act, child );
					else
					if( !swXmlStrcmp( child->name, "affected" ) )
						swGetContentInt64( &pMobIndex->affected_by, child );
					else
					if( !swXmlStrcmp( child->name, "alignment" ) )
						swGetContentInt( &pMobIndex->alignment, child );
					else
					if( !swXmlStrcmp( child->name, "sex" ) )
						swGetContentInt( &pMobIndex->sex, child );
					else
					if( !swXmlStrcmp( child->name, "credits" ) )
						swGetContentInt( &pMobIndex->gold, child );
					else
					if( !swXmlStrcmp( child->name, "position" ) )
						swGetContentInt( &pMobIndex->position, child );
					else
					if( !swXmlStrcmp( child->name, "sectiona" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "str" ) )
								swGetContentInt( &pMobIndex->perm_str, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "int" ) )
								swGetContentInt( &pMobIndex->perm_int, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "wis" ) )
								swGetContentInt( &pMobIndex->perm_wis, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "dex" ) )
								swGetContentInt( &pMobIndex->perm_dex, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "con" ) )
								swGetContentInt( &pMobIndex->perm_con, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "cha" ) )
								swGetContentInt( &pMobIndex->perm_cha, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "lck" ) )
								swGetContentInt( &pMobIndex->perm_lck, gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "sections" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "saving_poison_death" ) )
								swGetContentInt( &pMobIndex->saving_poison_death, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "saving_wand" ) )
								swGetContentInt( &pMobIndex->saving_wand, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "saving_para_petri" ) )
								swGetContentInt( &pMobIndex->saving_para_petri, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "saving_breath" ) )
								swGetContentInt( &pMobIndex->saving_breath, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "saving_spell_staff" ) )
								swGetContentInt( &pMobIndex->saving_spell_staff, gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "sectionr" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "height" ) )
								swGetContentInt( &pMobIndex->height, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "weight" ) )
								swGetContentInt( &pMobIndex->weight, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "speaks" ) )
								;
								//swGetContentInt64( &pMobIndex->speaks, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "speaking" ) )
							{
								LANG_DATA	*lang;
								char		*buf = NULL;

								STRDUP( buf, "" );
								swGetContent( &buf, gchild );
								if( (lang = find_lang( buf )) )
									pMobIndex->speaking = lang;
								else
									pMobIndex->speaking = lang_base;
								STRFREE( buf );
							}
							else
							if( !swXmlStrcmp( gchild->name, "numattacks" ) )
								swGetContentInt( &pMobIndex->numattacks, gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "sectionx" ) )
					{

						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "hitroll" ) )
								swGetContentInt( &pMobIndex->hitroll, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "damroll" ) )
								swGetContentInt( &pMobIndex->damroll, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "xflags" ) )
								swGetContentInt64( &pMobIndex->xflags, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "resistant" ) )
								swGetContentInt64( &pMobIndex->resistant, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "immune" ) )
								swGetContentInt64( &pMobIndex->immune, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "susceptible" ) )
								swGetContentInt64( &pMobIndex->susceptible, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "attacks" ) )
								swGetContentInt64( &pMobIndex->attacks, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "defenses" ) )
								swGetContentInt64( &pMobIndex->defenses, gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "sectiont" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "thac0" ) )
								swGetContentInt( &pMobIndex->mobthac0, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "ac" ) )
								swGetContentInt( &pMobIndex->ac, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "hitnodice" ) )
								swGetContentInt( &pMobIndex->hitnodice, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "hitsizedice" ) )
								swGetContentInt( &pMobIndex->hitsizedice, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "hitplus" ) )
								swGetContentInt( &pMobIndex->hitplus, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "damnodice" ) )
								swGetContentInt( &pMobIndex->damnodice, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "damsizedice" ) )
								swGetContentInt( &pMobIndex->damsizedice, gchild );
							else
							if( !swXmlStrcmp( gchild->name, "damplus" ) )
								swGetContentInt( &pMobIndex->damplus, gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "sectionv" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "vipflags" ) )
								swGetContent( &pMobIndex->s_vip_flags, gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "dialog" ) )
						swGetContent( &pMobIndex->dialog_name, child );
					else
					if( !swXmlStrcmp( child->name, "programs" ) )
						swGetContentPrograms( pMobIndex->mudprogs, &pMobIndex->progtypes, child );
				}
				area->area_tmp->mobs.push_back(pMobIndex);
			}
		}
		else
		if( !swXmlStrcmp( snode->name, "objects" ) )
		{
			for (node = snode->children; node; node = node->next)
			{
				EONLY( node );

				pObjIndex = new_obj_index();
				for (child = node->children; child; child = child->next) /* object */
				{
					EONLY( child );

					if( !swXmlStrcmp( child->name, "vnum" ) )
						swGetContentInt( &pObjIndex->vnum, child );
					else
					if( !swXmlStrcmp( child->name, "name" ) )
						swGetContent( &pObjIndex->name, child );
					else
					if( !swXmlStrcmp( child->name, "short" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "inflect0" ) )
								swGetContent( &pObjIndex->przypadki[0], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect1" ) )
								swGetContent( &pObjIndex->przypadki[1], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect2" ) )
								swGetContent( &pObjIndex->przypadki[2], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect3" ) )
								swGetContent( &pObjIndex->przypadki[3], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect4" ) )
								swGetContent( &pObjIndex->przypadki[4], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "inflect5" ) )
								swGetContent( &pObjIndex->przypadki[5], gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "description" ) )
						swGetContent( &pObjIndex->description, child );
					else
					if( !swXmlStrcmp( child->name, "actiondesc" ) )
						swGetContent( &pObjIndex->action_desc, child );
					else
					if( !swXmlStrcmp( child->name, "type" ) )
						swGetContentInt( &pObjIndex->item_type, child );
					else
					if( !swXmlStrcmp( child->name, "extraflags" ) )
						swGetContentInt64( &pObjIndex->extra_flags, child );
					else
					if( !swXmlStrcmp( child->name, "wearflags" ) )
						swGetContentInt64( &pObjIndex->wear_flags, child );
					else
					if( !swXmlStrcmp( child->name, "layers" ) )
						swGetContentInt( &pObjIndex->layers, child );
					else
					if( !swXmlStrcmp( child->name, "values" ) )
					{
						char	*buf2 = NULL;

						STRDUP( buf2, "" );
						for (gchild = child->children; gchild; gchild = gchild->next)
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "value0" ) )
								swGetContentInt( &pObjIndex->value[0], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "value1" ) )
								swGetContentInt( &pObjIndex->value[1], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "value2" ) )
								swGetContentInt( &pObjIndex->value[2], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "value3" ) )
							{
								if( pObjIndex->item_type == ITEM_PILL
								|| pObjIndex->item_type == ITEM_POTION
								|| pObjIndex->item_type == ITEM_DEVICE )
								{
									swGetContent( &buf2, gchild );
									pObjIndex->value[3] = skill_lookup( buf2 );
								}
								else
									swGetContentInt( &pObjIndex->value[3], gchild );
							}
							else
							if( !swXmlStrcmp( gchild->name, "value4" ) )
							{
								if( pObjIndex->item_type == ITEM_SALVE )
								{
									swGetContent( &buf2, gchild );
									pObjIndex->value[4] = skill_lookup( buf2 );
								}
								else
									swGetContentInt( &pObjIndex->value[4], gchild );
							}
							else
							if( !swXmlStrcmp( gchild->name, "value5" ) )
							{
								if( pObjIndex->item_type == ITEM_SALVE )
								{
									swGetContent( &buf2, gchild );
									pObjIndex->value[5] = skill_lookup( buf2 );
								}
								else
									swGetContentInt( &pObjIndex->value[5], gchild );
							}
						}
						STRFREE( buf2 );
					}
					else
					if( !swXmlStrcmp( child->name, "weight" ) )
						swGetContentInt( &pObjIndex->weight, child );
					else
					if( !swXmlStrcmp( child->name, "cost" ) )
						swGetContentInt( &pObjIndex->cost, child );
					else
					if( !swXmlStrcmp( child->name, "gender" ) )
						swGetContentInt( &pObjIndex->gender, child );
					else
					if( !swXmlStrcmp( child->name, "level" ) )
						swGetContentInt( &pObjIndex->level, child );
					else
					if( !swXmlStrcmp( child->name, "extradescs" ) )
						swGetContentExtraDescs( pObjIndex->extradesc, child );
					else
					if( !swXmlStrcmp( child->name, "requirements" ) )
					{
						char	*buf2 = NULL;

						STRDUP( buf2, "" );						
						for (gchild = child->children; gchild; gchild = gchild->next) /* requirement */
						{
							EONLY( gchild );

							req = new_requirement();
							pObjIndex->requirements.push_back(req);
							for (ggchild = gchild->children; ggchild; ggchild = ggchild->next)
							{
								EONLY( ggchild );

								if( !swXmlStrcmp( ggchild->name, "location" ) )
									swGetContentInt( &req->location, ggchild );
								else
								if( !swXmlStrcmp( ggchild->name, "modifier" ) )
									swGetContentInt( &req->modifier, ggchild );
								else
								if( !swXmlStrcmp( ggchild->name, "type" ) )
								{
									swGetContent( &buf2, ggchild );
									req->type = skill_lookup( buf2 );
								}
							}
						}
						STRFREE( buf2 );
					}
					else
					if( !swXmlStrcmp( child->name, "affects" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next) /* affect */
						{
							EONLY( gchild );

							paf = new_affect();
							pObjIndex->affects.push_back(paf);
							for (ggchild = gchild->children; ggchild; ggchild = ggchild->next)
							{
								EONLY( ggchild );

								if( !swXmlStrcmp( ggchild->name, "location" ) )
									swGetContentInt( &paf->location, ggchild );
								else
								if( !swXmlStrcmp( ggchild->name, "modifier" ) )
									swGetContentInt( &paf->modifier, ggchild );
							}
						}
					}
					else
					if( !swXmlStrcmp( child->name, "programs" ) )
						swGetContentPrograms( pObjIndex->mudprogs, &pObjIndex->progtypes, child );
				}
				area->area_tmp->objs.push_back(pObjIndex);
			}
		}
		else
		if( !swXmlStrcmp( snode->name, "rooms" ) )
		{
			for (node = snode->children; node; node = node->next)
			{
				EONLY( node );

				pRoomIndex = new_room();
				for (child = node->children; child; child = child->next) /* room */
				{
					EONLY( child );

					if( !swXmlStrcmp( child->name, "vnum" ) )
						swGetContentInt( &pRoomIndex->vnum, child );
					else
					if( !swXmlStrcmp( child->name, "name" ) )
						swGetContent( &pRoomIndex->name, child );
					else
					if( !swXmlStrcmp( child->name, "description" ) )
						swGetContent( &pRoomIndex->description, child );
					else
					if( !swXmlStrcmp( child->name, "nightdesc" ) )
						swGetContent( &pRoomIndex->nightdesc, child );
					else
					if( !swXmlStrcmp( child->name, "light" ) )
						swGetContentInt( &pRoomIndex->light, child );
					else
					if( !swXmlStrcmp( child->name, "flags" ) )
						swGetContentInt64( &pRoomIndex->room_flags, child );
					else
					if( !swXmlStrcmp( child->name, "sector" ) )
						swGetContentInt( &pRoomIndex->sector_type, child );
					else
					if( !swXmlStrcmp( child->name, "teledelay" ) )
						swGetContentInt( &pRoomIndex->tele_delay, child );
					else
					if( !swXmlStrcmp( child->name, "televnum" ) )
						swGetContentInt( &pRoomIndex->tele_vnum, child );
					else
					if( !swXmlStrcmp( child->name, "tunnel" ) )
						swGetContentInt( &pRoomIndex->tunnel, child );
					else
					if( !swXmlStrcmp( child->name, "exits" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next) /* exit */
						{
							EONLY( gchild );

							xit = new_exit();
							pRoomIndex->exits.push_back(xit);
							for (ggchild = gchild->children; ggchild; ggchild = ggchild->next)
							{
								EONLY( gchild );

								if( !swXmlStrcmp( ggchild->name, "direction" ) )
									swGetContentInt( &xit->vdir, ggchild );
								else
								if( !swXmlStrcmp( ggchild->name, "description" ) )
									swGetContent( &xit->description, ggchild );
								else
								if( !swXmlStrcmp( ggchild->name, "keyword" ) )
									swGetContent( &xit->keyword, ggchild );
								else
								if( !swXmlStrcmp( ggchild->name, "flags" ) )
								{
									swGetContentInt64( &xit->orig_flags, ggchild );
									xit->flags = xit->orig_flags;
								}
								else
								if( !swXmlStrcmp( ggchild->name, "key" ) )
									swGetContentInt( &xit->key, ggchild );
								else
								if( !swXmlStrcmp( ggchild->name, "vnum" ) )
									swGetContentInt( &xit->vnum, ggchild );
								else
								if( !swXmlStrcmp( ggchild->name, "distance" ) )
									swGetContentInt( &xit->distance, ggchild );
							}
						}
					}
					else
					if( !swXmlStrcmp( child->name, "extradescs" ) )
						swGetContentExtraDescs( pRoomIndex->extradesc, child );
					else
					if( !swXmlStrcmp( child->name, "programs" ) )
						swGetContentPrograms( pRoomIndex->mudprogs, &pRoomIndex->progtypes, child );
				}
				pRoomIndex->area = area;
				area->area_tmp->rooms.push_back(pRoomIndex);
			}
		}
		else
		if( !swXmlStrcmp( snode->name, "resets" ) )
		{
			char	*buf2 = NULL;
			int64	lastroom = 0;
			int64	lastobj = 0;

			STRDUP( buf2, "" );
			for (node = snode->children; node; node = node->next) /* reset */
			{
				EONLY( node );

				pReset = new_reset();
				for (child = node->children; child; child = child->next)
				{
					EONLY( child );

					if( !swXmlStrcmp( child->name, "command" ) )
					{
						swGetContent( &buf2, child );
						pReset->command = *buf2;
					}
					else
					if( !swXmlStrcmp( child->name, "extra" ) )
						swGetContentInt( &pReset->extra, child );
					else
					if( !swXmlStrcmp( child->name, "arg1" ) )
						swGetContentInt64( &pReset->arg1, child );
					else
					if( !swXmlStrcmp( child->name, "arg2" ) )
						swGetContentInt64( &pReset->arg2, child );
					else
					if( !swXmlStrcmp( child->name, "arg3" ) )
						swGetContentInt64( &pReset->arg3, child );
					else
					if( !swXmlStrcmp( child->name, "arg4" ) )
						swGetContentInt64( &pReset->arg4, child );
				}
				if( !(pRoomIndex = validate_reset( area, pReset, &lastroom, &lastobj )) )
					free_reset( pReset );
				else
					pRoomIndex->resets.push_back(pReset);
			}
			STRFREE( buf2 );
		}
		else
		if( !swXmlStrcmp( snode->name, "shops" ) )
		{
			for (node = snode->children; node; node = node->next) /* shop */
			{
				EONLY( node );

				pShop = new_shop();
				for (child = node->children; child; child = child->next)
				{
					EONLY( child );

					if( !swXmlStrcmp( child->name, "keeper" ) )
						swGetContentInt( &pShop->keeper, child );
					else
					if( !swXmlStrcmp( child->name, "types" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next) /* type */
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "type0" ) )
								swGetContentInt( &pShop->buy_type[0], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "type1" ) )
								swGetContentInt( &pShop->buy_type[1], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "type2" ) )
								swGetContentInt( &pShop->buy_type[2], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "type3" ) )
								swGetContentInt( &pShop->buy_type[3], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "type4" ) )
								swGetContentInt( &pShop->buy_type[4], gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "profitbuy" ) )
						swGetContentInt( &pShop->profit_buy, child );
					else
					if( !swXmlStrcmp( child->name, "profitsell" ) )
						swGetContentInt( &pShop->profit_sell, child );
					else
					if( !swXmlStrcmp( child->name, "open" ) )
						swGetContentInt( &pShop->open_hour, child );
					else
					if( !swXmlStrcmp( child->name, "close" ) )
						swGetContentInt( &pShop->close_hour, child );
					else
					if( !swXmlStrcmp( child->name, "flags" ) )
						swGetContentInt64( &pShop->flags, child );
				}
				if( !(pMobIndex = get_mob_tmp( pShop->keeper, area )) )
				{
					bug( "no such shop keeper: %d", pShop->keeper );
					free_shop( pShop );
				}
				else
				{
					area->area_tmp->shops.push_back(pShop);
					pMobIndex->pShop = pShop;
					pShop->profit_sell = URANGE( 0, pShop->profit_sell, pShop->profit_buy-5 );
					pShop->profit_buy = URANGE( pShop->profit_sell+5, pShop->profit_buy, 1000 );
				}
			}
		}
		else
		if( !swXmlStrcmp( snode->name, "repairs" ) )
		{
			for (node = snode->children; node; node = node->next) /* repair */
			{
				EONLY( node );

				pRepair = new_repair();
				for (child = node->children; child; child = child->next)
				{
					EONLY( child );

					if( !swXmlStrcmp( child->name, "keeper" ) )
						swGetContentInt( &pRepair->keeper, child );
					else
					if( !swXmlStrcmp( child->name, "types" ) )
					{
						for (gchild = child->children; gchild; gchild = gchild->next) /* type */
						{
							EONLY( gchild );

							if( !swXmlStrcmp( gchild->name, "type0" ) )
								swGetContentInt( &pRepair->fix_type[0], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "type1" ) )
								swGetContentInt( &pRepair->fix_type[1], gchild );
							else
							if( !swXmlStrcmp( gchild->name, "type2" ) )
								swGetContentInt( &pRepair->fix_type[2], gchild );
						}
					}
					else
					if( !swXmlStrcmp( child->name, "profitfix" ) )
						swGetContentInt( &pRepair->profit_fix, child );
					else
					if( !swXmlStrcmp( child->name, "shoptype" ) )
						swGetContentInt( &pRepair->shop_type, child );
					else
					if( !swXmlStrcmp( child->name, "open" ) )
						swGetContentInt( &pRepair->open_hour, child );
					else
					if( !swXmlStrcmp( child->name, "close" ) )
						swGetContentInt( &pRepair->close_hour, child );
				}
				if( !(pMobIndex = get_mob_tmp( pRepair->keeper, area )) )
				{
					bug( "no such repair keeper: %d", pRepair->keeper );
					free_repair( pRepair );
				}
				else
				{
					area->area_tmp->repairs.push_back(pRepair);
					pMobIndex->rShop = pRepair;
				}
			}
		}
		else
		if( !swXmlStrcmp( snode->name, "specials" ) )
		{
			char	*buf2 = NULL;
			char	*buf3 = NULL;

			STRDUP( buf2, "" );
			STRDUP( buf3, "" );
			for (node = snode->children; node; node = node->next) /* special */
			{
				EONLY( node );

				for (child = node->children; child; child = child->next)
				{
					EONLY( child );

					if( !swXmlStrcmp( child->name, "vnum" ) )
						swGetContentInt( &vnum, child );
					else
					if( !swXmlStrcmp( child->name, "function" ) )
						swGetContent( &buf2, child );
					else
					if( !swXmlStrcmp( child->name, "function2" ) )
						swGetContent( &buf3, child );
				}
				if( !(pMobIndex = get_mob_tmp( vnum, area )) )
					bug( "no such mob: %d for special", vnum );
				else
				{
					pMobIndex->spec_fun = spec_lookup( buf2 );
					if( *buf3 )
						pMobIndex->spec_2 = spec_lookup( buf3 );
				}
			}
			STRFREE( buf2 );
			STRFREE( buf3 );
		}
	}

	uarea_list.push_back(area);

	swXmlCleanIO();
	xmlFreeDoc( doc );
	swCleanupParser();
	return area;
}

bool install_area( AREA_DATA *area )
{
	AREA_DATA	*pArea;
	MID			*pMobIndex;
	OID			*pObjIndex;
	RID			*pRoomIndex;
	MID			*pMobIndex_next;
	OID			*pObjIndex_next;
	RID			*pRoomIndex_next;
	SHD			*pShop;
	RHD			*pRepair;
	int			i;


	if( !area->area_tmp )
	{
		bug( "area (filename: %s) already installed", area->filename );
		return false;
	}

	for (auto* pArea : area_list)
		if( IVR( pArea->lvnum, area ) || IVR( pArea->uvnum, area ) )
		{
			bug( "area (filename: %s) vnums <%d;%d> collide with"
				" installed area (filename: %s) vnums <%d;%d>",
				area->filename, area->lvnum, area->uvnum,
				pArea->filename, pArea->lvnum, pArea->uvnum	);
			return false;
		}

	for (auto* pMobIndex : area->area_tmp->mobs)
	{
		if( !IVR( pMobIndex->vnum, area ) )
		{
			bug( "mob %d vnum is not an area vnum", pMobIndex->vnum );
			return false;
		}
	}

	for (auto* pObjIndex : area->area_tmp->objs)
	{
		if( !IVR( pObjIndex->vnum, area ) )
		{
			bug( "obj %d vnum is not an area vnum", pObjIndex->vnum );
			return false;
		}
	}

	for (auto* pRoomIndex : area->area_tmp->rooms)
	{
		if( !IVR( pRoomIndex->vnum, area ) )
		{
			bug( "room %d vnum is not an area vnum", pRoomIndex->vnum );
			return false;
		}
	}

	for (auto* pShop : area->area_tmp->shops)
	{
		if( !IVR( pShop->keeper, area ) )
		{
			bug( "shop keeper %d vnum is not an area vnum", pShop->keeper );
			return false;
		}
	}

	for (auto* pRepair : area->area_tmp->repairs)
	{
		if( !IVR( pRepair->keeper, area ) )
		{
			bug( "repair keeper %d vnum is not an area vnum", pRepair->keeper );
			return false;
		}
	}
	
	/* wszystko ok, instalujemy krainke */

	uarea_list.remove(area);
	sort_area( area, false );
	area_list.push_back(area);

	for (auto* pMobIndex : area->area_tmp->mobs)
	{
		i = pMobIndex->vnum % MAX_KEY_HASH;
		mob_index_hash[i].push_front(pMobIndex);
		top_mob_index++;
	}

	for (auto* pObjIndex : area->area_tmp->objs)
	{
		i = pObjIndex->vnum % MAX_KEY_HASH;
		obj_index_hash[i].push_front(pObjIndex);
		top_obj_index++;
	}

	for (auto* pRoomIndex : area->area_tmp->rooms)
	{
		i = pRoomIndex->vnum % MAX_KEY_HASH;
		room_index_hash[i].push_front(pRoomIndex);
	}

	shop_list.splice(shop_list.end(), area->area_tmp->shops);
	repair_list.splice(repair_list.end(), area->area_tmp->repairs);

	free_area_tmp( area->area_tmp );

	return true;
}

