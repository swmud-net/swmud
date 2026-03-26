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
*                          XML manipulating module                         *
****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <libgen.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <iconv.h>
#include "mud.h"

/* Trog */

xmlDocPtr	xsd_validate	( const char *what, const char *xsd, const char *dir,
	const char *filename );

#define	BC	BAD_CAST

#define	EONLY( node )	if( (node)->type != XML_ELEMENT_NODE ) continue

#define	ILIST( path )\
ilist_ok = TRUE;\
if( (ilist_doc = xsd_validate( "list", SCHEMA_LIST, "", path )) )\
	for( ilist_node = xmlDocGetRootElement( ilist_doc )->children; ilist_node; ilist_node = ilist_node->next )\
		if( ilist_node->type == XML_ELEMENT_NODE\
		&& !swXmlStrcmp( ilist_node->name, "entry" )\
		&& swGetContent( &ilist_buf, ilist_node ) )\


#define	ICLEAN	\
xmlFreeDoc( ilist_doc );\
xmlCleanupParser();\
STRFREE( ilist_buf );\
ilist_doc = NULL;\
ilist_buf = NULL;\
ilist_ok = FALSE

#define	ILD_CREATE( type, name_list, field, ild_list )\
do\
{\
	ILD		*ild;\
\
	for( auto* ptr : name_list )\
	{\
		ild = new_ild();\
		STRDUP( ild->data, ptr->field );\
		(ild_list).push_back( ild );\
	}\
\
} while(0)

#define	ILD_FREE( ild_list )\
do\
{\
	for( auto* ild : (ild_list) )\
		free_ild( ild );\
	(ild_list).clear();\
} while(0)

extern xmlDocPtr				ilist_doc;
extern xmlNodePtr				ilist_node;
extern bool						ilist_ok;
extern char						*ilist_buf;

void swXmlInitIO();
void swXmlCleanIO();
void swCleanupParser();

xmlNodePtr swNewChildText( xmlNodePtr node, xmlNsPtr ns, const char *name,
	const char *content );
/**
 * NE = Not Empty, dziecko zosanie dodane tylko w przypadku gdy jego zawartosc jest niepusta
 */
xmlNodePtr swNewChildTextNE( xmlNodePtr node, xmlNsPtr ns, const char *name,
	const char *content );
xmlNodePtr swNewChildInt( xmlNodePtr node, xmlNsPtr ns, const char *name,
	int content );
xmlNodePtr swNewChildIntNE( xmlNodePtr node, xmlNsPtr ns, const char *name,
	int content );
xmlNodePtr swNewChildLong( xmlNodePtr node, xmlNsPtr ns, const char *name,
	long content );
xmlNodePtr swNewChildLongNE( xmlNodePtr node, xmlNsPtr ns, const char *name,
	long content );
xmlNodePtr swNewChildInt64( xmlNodePtr node, xmlNsPtr ns, const char *name,
	int64 content );
xmlNodePtr swNewChildInt64NE( xmlNodePtr node, xmlNsPtr ns, const char *name,
	int64 content );
xmlNodePtr swNewComment( const char *content );
xmlAttrPtr swNewProp( xmlNodePtr node, const char *name, const char *content );
xmlAttrPtr swNewPropInt( xmlNodePtr node, const char *name, int content );
xmlAttrPtr swNewPropInt64( xmlNodePtr node, const char *name, int64 content );

bool swGetContent( char **dst, xmlNodePtr node );
void swGetContentInt( int *dst, xmlNodePtr node );
void swGetContentLong( long *dst, xmlNodePtr node );
void swGetContentInt64( int64 *dst, xmlNodePtr node );
void swGetPropInt( int *dst, xmlNodePtr node, const char *name );
void swGetPropInt64( int64 *dst, xmlNodePtr node, const char *name );

void save_list( const char *path, std::list<ILD*>& ild_list );

int swXmlStrcmp( const xmlChar *str1, const char *str2 );

