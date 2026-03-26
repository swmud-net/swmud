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

#include "mud.h"

xmlDocPtr				ilist_doc = NULL;
xmlNodePtr				ilist_node = NULL;
bool					ilist_ok = false;
char					*ilist_buf = NULL;
static char				*conv_buf = NULL;

/*
static void xsdError( void *ctx, const char *msg, ... )
{
}

static void xsdWarning( void *ctx, const char *msg, ... )
{
}
*/

/*
void swXmlInit()
{
	xmlSchemaSetValidErrors();
	xmlSchemaSetParserErrors();
}
*/
void swXmlInitIO()
{
	CREATE( conv_buf, char, MSL );
}

void swXmlCleanIO()
{
	DISPOSE( conv_buf );
}

void swCleanupParser()
{
	if( !ilist_ok )
		xmlCleanupParser();
}

/*! konwertuje napis w UTF-8 na napis w ISO-8859-2 */
char *i2u( const char *in )
{
	iconv_t	cd;
	int		ret;
	size_t	inlen = strlen( in );
	size_t	outlen = MSL - 1;


	DISPOSE( conv_buf );
	CREATE( conv_buf, char, MSL );
	cd = iconv_open( "UTF-8", "ISO-8859-2" );
	ret = iconv( cd, (char **)&in, &inlen, &conv_buf, &outlen );
	iconv_close( cd );
	conv_buf -= MSL - 1 - outlen;
	if( ret < 0 )
	{
		bug( "i2u: iconv error" );
		return (char *)in;
	}

	return conv_buf;
}

/*! konwertuje napis w ISO-8859-2 na napis w UTF-8 */
char *u2i( const char *in )
{
	iconv_t	cd;
	int		ret;
	size_t	inlen = strlen( in );
	size_t	outlen = MSL - 1;


	QDISPOSE( conv_buf );
	CREATE( conv_buf, char, MSL );
	cd = iconv_open( "ISO-8859-2", "UTF-8" );
	ret = iconv( cd, (char **)&in, &inlen, &conv_buf, &outlen );
	iconv_close( cd );
	conv_buf -= MSL - 1 - outlen;
	if( ret < 0 )
	{
		bug( "u2i: iconv error" );
		return (char *)in;
	}

	return conv_buf;
}

xmlNodePtr swNewChildText( xmlNodePtr node, xmlNsPtr ns, const char *name,
	const char *content )
{
	return xmlNewTextChild( node, ns, BC name, BC i2u( content ) );
}

xmlNodePtr swNewChildTextNE( xmlNodePtr node, xmlNsPtr ns, const char *name,
	const char *content )
{
	return *content ? xmlNewTextChild( node, ns, BC name, BC i2u( content ) ) : NULL;
}

xmlNodePtr swNewChildInt( xmlNodePtr node, xmlNsPtr ns, const char *name,
	int content )
{
	char	buf[MSL];


	sprintf( buf, "%d", content );
	return xmlNewTextChild( node, ns, BC name, BC i2u( buf ) );
}

xmlNodePtr swNewChildIntNE( xmlNodePtr node, xmlNsPtr ns, const char *name,
	int content )
{
	char	buf[MSL];


	if( !content )
		return NULL;
	sprintf( buf, "%d", content );
	return xmlNewTextChild( node, ns, BC name, BC i2u( buf ) );
}

xmlNodePtr swNewChildLong( xmlNodePtr node, xmlNsPtr ns, const char *name,
	long content )
{
	char	buf[MSL];


	sprintf( buf, "%ld", content );
	return xmlNewTextChild( node, ns, BC name, BC i2u( buf ) );
}

xmlNodePtr swNewChildLongNE( xmlNodePtr node, xmlNsPtr ns, const char *name,
	long content )
{
	char	buf[MSL];


	if( !content )
		return NULL;
	sprintf( buf, "%ld", content );
	return xmlNewTextChild( node, ns, BC name, BC i2u( buf ) );
}

xmlNodePtr swNewChildInt64( xmlNodePtr node, xmlNsPtr ns, const char *name,
	int64 content )
{
	char	buf[MSL];


	sprintf( buf, "%lld", content );
	return xmlNewTextChild( node, ns, BC name, BC i2u( buf ) );
}

xmlNodePtr swNewChildInt64NE( xmlNodePtr node, xmlNsPtr ns, const char *name,
	int64 content )
{
	char	buf[MSL];


	if( !content )
		return NULL;
	sprintf( buf, "%lld", content );
	return xmlNewTextChild( node, ns, BC name, BC i2u( buf ) );
}

xmlNodePtr swNewComment( const char *content )
{
	return xmlNewComment( BC i2u( content ) );
}

xmlAttrPtr swNewProp( xmlNodePtr node, const char *name, const char *content )
{
	return xmlNewProp( node, BC name, BC i2u( content ) );
}

xmlAttrPtr swNewPropInt( xmlNodePtr node, const char *name, int content )
{
	char	buf[MSL];


	sprintf( buf, "%d", content );
	return xmlNewProp( node, BC name, BC i2u( buf ) );
}

xmlAttrPtr swNewPropInt64( xmlNodePtr node, const char *name, int64 content )
{
	char	buf[MSL];


	sprintf( buf, "%lld", content );
	return xmlNewProp( node, BC name, BC i2u( buf ) );
}

/* in all swGetContent* functions the buffer returned by xmlNodeGetContent must be freed */

bool swGetContent( char **dst, xmlNodePtr node )
{
	if( node->children && node->children->content )
	{
		char *buf = (char *)xmlNodeGetContent( node->children );
		STRDUP( *dst, u2i( buf ) );
		xmlFree(buf);
	}

	return true;
}

void swGetContentInt( int *dst, xmlNodePtr node )
{
	if( node->children && node->children->content )
	{
		char *buf = (char *)xmlNodeGetContent( node->children );
		*dst = atoi( u2i( buf ) );
		xmlFree(buf);
	}
}

void swGetContentLong( long *dst, xmlNodePtr node )
{
	if( node->children && node->children->content )
	{
		char *buf = (char *)xmlNodeGetContent( node->children );
		*dst = atol( u2i( buf ) );
		xmlFree(buf);
	}
}

void swGetContentInt64( int64 *dst, xmlNodePtr node )
{
	if( node->children && node->children->content )
	{
		char *buf = (char *)xmlNodeGetContent( node->children );
		*dst = atoi64( u2i( buf ) );
		xmlFree(buf);
	}
}

void swGetPropInt( int *dst, xmlNodePtr node, const char *name )
{
	char	*buf;

	buf = (char *)xmlGetProp( node, BC name );
	*dst = atoi( u2i( buf ) );
	xmlFree( buf );
}

void swGetPropInt64( int64 *dst, xmlNodePtr node, const char *name )
{
	char	*buf;

	buf = (char *)xmlGetProp( node, BC name );
	*dst = atoi64( u2i( buf ) );
	xmlFree( buf );
}

void save_list( const char *path, std::list<ILD*>& ild_list )
{
	xmlDocPtr	doc;
	xmlNodePtr	root;
	char		buf[MIL];


	swXmlInitIO();
	doc = xmlNewDoc( BC"1.0" );
	root = xmlNewNode( NULL, BC"list" );
	xmlNewNs( root, BC"http://swmud.pl/ns/swmud/1.0/list", NULL );
	xmlNewNs( root, BC"http://www.w3.org/2001/XMLSchema-instance", BC"xsi" );
	swNewProp( root, "xsi:schemaLocation", "http://swmud.pl/ns/swmud/1.0/list list.xsd" );
	xmlDocSetRootElement( doc, root );
	for (auto* ild : ild_list)
		swNewChildText( root, NULL, "entry", ild->data );

	/* saving stuff */
	sprintf( buf, "Saving lists/%s...", basename( (char *)path ) );
	log_string_plus( buf, LOG_NORMAL, LEVEL_GREATER );

	RESERVE_CLOSE;
	xmlSaveFormatFileEnc( path, doc, "ISO-8859-2", 1 );
	RESERVE_OPEN;

	swXmlCleanIO();
	xmlFreeDoc( doc );
	swCleanupParser();
}

int swXmlStrcmp( const xmlChar *str1, const char *str2 )
{
	return xmlStrcmp( str1, BC str2 );
}


