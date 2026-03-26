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
 *		     Character saving and loading module		   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#if defined (__CYGWIN32__) || defined(__FreeBSD__)
#include <dirent.h>
#define direct dirent
#else
#include <sys/dir.h>
#endif
#include "mud.h"
#include <SWTimeStamp.h>

/*
 * Increment with every major format change.
 */
#define SAVEVERSION	3

/*
 * Array to keep track of equipment temporarily.		-Thoric
 */
OBJ_DATA *save_equipment[MAX_WEAR][8];
CHAR_DATA *quitting_char, *loading_char, *saving_char;

int file_ver = 4;

/*
 * Array of containers read for proper re-nesting of objects.
 */
static OBJ_DATA *rgObjNest[MAX_NEST];

/*
 * Local functions.
 */
void fread_char args( ( CHAR_DATA *ch, FILE *fp ) );
void write_corpses args( ( CHAR_DATA *ch, char *name ) );

void save_room_storage(ROOM_INDEX_DATA *room)
{
	if (!room)
		return;
	if (!(room->room_flags & ROOM_STORAGE))
		return;

	FILE *fp;
	char filename[256];

	sprintf(filename, "%s%d", STORAGES_DIR, room->vnum);
	fp = fopen(filename, "w");
	if (fp)
	{
		fprintf(fp, "#STORAGEDATA\n");
		fprintf(fp, "End\n\n#END\n\n");
		for (auto* cobj : room->contents)
		{
			//First argument isn't used anyway
			fwrite_obj(NULL, cobj, fp, 0, OS_CARRY);
		}
		fprintf(fp, "#END\n");
		fclose(fp);
	}
	else
	{
		bug("Couldn't write storage data file for room: %d", room->vnum);
	}
}

void load_room_storage(ROOM_INDEX_DATA *room)
{
	if (!room)
		return;
	if (!(room->room_flags & ROOM_STORAGE))
		return;

	char filename[256];
	FILE *fph;
	ROOM_INDEX_DATA *storeroom = room;

	{
		auto snapshot = storeroom->contents;
		for (auto* obj : snapshot)
			extract_obj(obj);
	}

	sprintf(filename, "%s%d", STORAGES_DIR, room->vnum);
	fph = fopen(filename, "r");

	if (fph)
	{
		int iNest;

		rset_supermob(storeroom);
		for (iNest = 0; iNest < MAX_NEST; iNest++)
			rgObjNest[iNest] = NULL;

		for (;;)
		{
			char letter;
			char *word;

			letter = fread_letter(fph);
			if (letter == '*')
			{
				fread_to_eol(fph);
				continue;
			}

			if (letter != '#')
			{
				bug("# not found.(storage:%d)", storeroom->vnum);
				break;
			}

			word = fread_word(fph);
			if (!str_cmp(word, "STORAGEDATA"))
				while (str_cmp(word, "#END"))
					word = fread_word(fph);
			else if (!str_cmp(word, "OBJECT")) /* Objects	*/
				fread_obj(supermob, fph, OS_CARRY);
			else if (!str_cmp(word, "END")) /* Done		*/
				break;
			else
			{
				bug("bad section. (ch:%d)", storeroom->vnum);
				break;
			}
		}

		fclose(fph);

		{
			auto snapshot = supermob->carrying;
			for (auto* tobj : snapshot)
			{
				obj_from_char(tobj);
				obj_to_room(tobj, storeroom);
			}
		}
		release_supermob();
	}
	else
	{
		bug("Couldn't read storage data for room: %d", room->vnum);
	}
}

void save_home(CHAR_DATA *ch)
{
	if (ch->plr_home)
	{
		FILE *fp;
		char filename[256];
		int templvl;

		/* NOWE zapisywanie dom�w */
		sprintf(filename, "%s%s", PHOME_DIR, capitalize(ch->name));
		if ((fp = fopen(filename, "w")))
		{
			fprintf(fp, "#HOMEDATA\n");
			fprintf(fp, "Name         %s~\n", ch->plr_home->name);
			fprintf(fp, "Vnum         %d\n", ch->plr_home->vnum);
			fprintf(fp, "End\n\n#END\n\n");
			templvl = ch->top_level;
			ch->top_level = LEVEL_HERO; /* make sure EQ doesn't get lost */
			for (auto* cobj : ch->plr_home->contents)
				fwrite_obj(ch, cobj, fp, 0, OS_CARRY);
			fprintf(fp, "#END\n");
			ch->top_level = templvl;
			fclose(fp);
		}
	}
}

void load_home(CHAR_DATA *ch)
{
	if (ch->plr_home != NULL)
	{
		char filename[256];
		FILE *fph;
		ROOM_INDEX_DATA *storeroom = ch->plr_home;

		{
			auto snapshot = storeroom->contents;
			for (auto* obj : snapshot)
				extract_obj(obj);
		}

		sprintf(filename, "%s%s", PHOME_DIR, capitalize(ch->name));
		if ((fph = fopen(filename, "r")) != NULL)
		{
			int iNest;

			rset_supermob(storeroom);
			for (iNest = 0; iNest < MAX_NEST; iNest++)
				rgObjNest[iNest] = NULL;

			for (;;)
			{
				char letter;
				char *word;

				letter = fread_letter(fph);
				if (letter == '*')
				{
					fread_to_eol(fph);
					continue;
				}

				if (letter != '#')
				{
					bug("# not found.(ch:%s)", ch->name);
					break;
				}

				word = fread_word(fph);
				if (!str_cmp(word, "HOMEDATA"))
					while (str_cmp(word, "#END"))
						word = fread_word(fph);
				else if (!str_cmp(word, "OBJECT")) /* Objects	*/
					fread_obj(supermob, fph, OS_CARRY);
				else if (!str_cmp(word, "END")) /* Done		*/
					break;
				else
				{
					bug("bad section. (ch:%s)", ch->name);
					break;
				}
			}

			fclose(fph);

			{
				auto snapshot = supermob->carrying;
				for (auto* tobj : snapshot)
				{
					obj_from_char(tobj);
					obj_to_room(tobj, storeroom);
				}
			}
			release_supermob();
		}
	}
	return;
}

void load_storages()
{
	DIR *dir;
	struct dirent *entry;
	char buf[MSL];
	char filename[256];
	ROOM_INDEX_DATA *room;
	int vnum;

	if (!(dir = opendir( STORAGES_DIR)))
	{
		perror( STORAGES_DIR);
		return;
	}

	while ((entry = readdir(dir)))
	{
		if (entry->d_name[0] == '.' || !is_number(entry->d_name))
			continue;

		vnum = strtol(entry->d_name, NULL, 10);

		if (vnum)
		{
			room = get_room_index(vnum);
			if (room && IS_SET(room->room_flags, ROOM_STORAGE))
			{
				load_room_storage(room);
			}
			else
			{
				sprintf(filename, "%s%d", STORAGES_DIR, room->vnum);
				sprintf(buf,
						"Removing file %s because room %d is not a storage.",
						filename, vnum);
				log_string(buf);
				unlink(filename);
			}
		}
		else
		{
			sprintf(filename, "%s%d", STORAGES_DIR, vnum);
			bug("I don't know what to do with file %s.", filename);
		}
	}
	closedir(dir);
}

void load_home_flags()
{
#if !defined( WIN32 )
	DIR *dir;
	struct dirent *entry;
	FILE *fp;
	const char *word;
	char *homename = 0;
	char buf[MSL];
	ROOM_INDEX_DATA *room;
	int vnum;
	bool fileok, home;

	if (!(dir = opendir( PHOME_DIR)))
	{
		perror( PHOME_DIR);
		return;
	}

	while ((entry = readdir(dir)))
	{
		if (entry->d_name[0] == '.')
			continue;

		sprintf(buf, "%s%s", PHOME_DIR, entry->d_name);

		if (!(fp = fopen(buf, "r")))
		{
			perror(buf);
			continue;
		}

		fileok = home = false;
		for (;;)
		{
			word = feof(fp) ? "End" : fread_word(fp);
			if (!str_cmp(word, "End") || !str_cmp(word, "#END"))
				break;
			else if (word[0] == '*')
				fread_to_eol(fp);
			else if (!str_cmp(word, "#HOMEDATA"))
				fileok = true;
			else if (!str_cmp(word, "Name"))
			{
				homename = st_fread_string(fp);
				home = true;
			}
			else if (!str_cmp(word, "Vnum"))
			{
				vnum = fread_number(fp);
				if ((room = get_room_index(vnum)) && fileok)
				{
					REMOVE_BIT(room->room_flags, ROOM_EMPTY_HOME);
					SET_BIT(room->room_flags, ROOM_PLR_HOME);
					if (home)
						STRDUP(room->homename, homename);
				}
			}
		}
		fclose(fp);
	}
	closedir(dir);
	return;
#endif
}

/*
 * Un-equip character before saving to ensure proper	-Thoric
 * stats are saved in case of changes to or removal of EQ
 */
void de_equip_char(CHAR_DATA *ch)
{
	OBJ_DATA *obj;
	int x, y;

	for (x = 0; x < MAX_WEAR; x++)
		for (y = 0; y < MAX_LAYERS; y++)
			save_equipment[x][y] = NULL;
	for (auto* obj : ch->carrying)
		if (obj->wear_loc > -1 && obj->wear_loc < MAX_WEAR)
		{

			for (x = 0; x < MAX_LAYERS; x++)
				if (!save_equipment[obj->wear_loc][x])
				{
					save_equipment[obj->wear_loc][x] = obj;
					break;
				}
			if (x == MAX_LAYERS)
			{
				bug(
						"%s had on more than %d layers of clothing in one location (%d): %s",
						ch->name, MAX_LAYERS, obj->wear_loc, obj->name);
			}

			unequip_char(ch, obj);
		}
}

/*
 * Re-equip character					-Thoric
 */
void re_equip_char(CHAR_DATA *ch)
{
	int x, y;

	for (x = 0; x < MAX_WEAR; x++)
		for (y = 0; y < MAX_LAYERS; y++)
			if (save_equipment[x][y] != NULL)
			{
				if (quitting_char != ch)
					equip_char(ch, save_equipment[x][y], x);
				save_equipment[x][y] = NULL;
			}
			else
				break;
}

/* Zapisuje dane niezale�ne od postaci gracza ani od jej klonu. -- Thanos */
/* Trog */
void save_pdata2(CHAR_DATA *ch)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr node;
	xmlNodePtr child;
	char buf[MSL];

	swXmlInitIO();
	doc = xmlNewDoc( BC"1.0" );
	root = xmlNewNode( NULL, BC"pdata" );
	xmlNewNs( root, BC"http://swmud.pl/ns/swmud/1.0/pdata", NULL );
	xmlNewNs( root, BC"http://www.w3.org/2001/XMLSchema-instance", BC"xsi" );
	swNewProp(root, "xsi:schemaLocation",
			"http://swmud.pl/ns/swmud/1.0/pdata pdata.xsd");
	xmlDocSetRootElement(doc, root);

	swNewChildText(root, NULL, "name", ch->name);
	node = xmlNewChild( root, NULL, BC"lastplayed", NULL );
	swNewChildInt(node, NULL, "current", (int) current_time);
	swNewChildInt(node, NULL, "minsplaying",
			(int) (current_time - ch->logon) / 60);
	node = xmlNewChild( root, NULL, BC"site", NULL );
	swNewChildText(node, NULL, "host",
			ch->pcdata->host ? ch->pcdata->host : "(Link-Dead)");
	swNewChildText(node, NULL, "ident",
			ch->pcdata->user ? ch->pcdata->user : "UNKNOWN");
	swNewChildInt(root, NULL, "played",
			ch->played + (int) (current_time - ch->logon));
	swNewChildInt(root, NULL, "wizinvis", ch->pcdata->wizinvis);
	if (*ch->pcdata->email)
		swNewChildText(root, NULL, "email", ch->pcdata->email);
	if (ch->trust)
		swNewChildInt(root, NULL, "trust", ch->trust);
	if (ch->plr_home)
		swNewChildInt(root, NULL, "home", ch->plr_home->vnum);
	swNewChildLong(root, NULL, "bank", ch->pcdata->bank);
	swNewChildText(root, NULL, "title", ch->pcdata->title);
	if (ch->pcdata->security)
		swNewChildInt(root, NULL, "security", ch->pcdata->security);
	swNewChildInt64(root, NULL, "act", ch->act & ~PLR_AFK);
	node = xmlNewChild( root, NULL, BC"stbar", NULL );
	swNewChildInt(node, NULL, "width", ch->pcdata->status_width);
	swNewChildInt(node, NULL, "height", ch->pcdata->status_height);
	swNewChildInt(node, NULL, "type", ch->pcdata->status_type);
	swNewChildInt(root, NULL, "deaf", ch->deaf);
	if (ch->pcdata->outcast_time)
		swNewChildLong(root, NULL, "outcasttime", ch->pcdata->outcast_time);
	if (ch->pcdata->restore_time)
		swNewChildLong(root, NULL, "restoretime", ch->pcdata->restore_time);
	if (*ch->pcdata->bamfin)
		swNewChildText(root, NULL, "bamfin", ch->pcdata->bamfin);
	if (*ch->pcdata->bamfout)
		swNewChildText(root, NULL, "bamfout", ch->pcdata->bamfout);
	if (*ch->pcdata->rank)
		swNewChildText(root, NULL, "rank", ch->pcdata->rank);
	if (*ch->pcdata->forbidden_cmd)
		swNewChildText(root, NULL, "forbdncmd", ch->pcdata->forbidden_cmd);
	if (*ch->pcdata->bestowments)
		swNewChildText(root, NULL, "bestowments", ch->pcdata->bestowments);
	swNewChildText(root, NULL, "authedby", ch->pcdata->authed_by);
	swNewChildLong(root, NULL, "lastnote", ch->pcdata->last_note);
	if (*ch->pcdata->editinfo)
		swNewChildText(root, NULL, "editinfo", ch->pcdata->editinfo);
	if (*ch->pcdata->invis_except)
		swNewChildText(root, NULL, "invisexcept", ch->pcdata->invis_except);
	if (*ch->pcdata->ignorelist)
		swNewChildText(root, NULL, "ignorelist", ch->pcdata->ignorelist);
	if (ch->pcdata->min_snoop)
		swNewChildInt(root, NULL, "minsnoop", ch->pcdata->min_snoop);
	if (*ch->pcdata->prompt)
		swNewChildText(root, NULL, "prompt", ch->pcdata->prompt);
	if (*ch->pcdata->editinfo)
		swNewChildText(root, NULL, "editinfo", ch->pcdata->editinfo);
	swNewChildInt(root, NULL, "charset", ch->charset);
	if (ch->pcdata->pagerlen != 24)
		swNewChildInt(root, NULL, "pagerlen", ch->pcdata->pagerlen);
	swNewChildInt64(root, NULL, "flags", ch->pcdata->flags);
	if (ch->pcdata->release_date > current_time)
	{
		node = xmlNewChild( root, NULL, BC"isolated", NULL );
		swNewChildInt(node, NULL, "until", ch->pcdata->release_date);
		swNewChildText(node, NULL, "by", ch->pcdata->isolated_by);
	}
	if (ch->pcdata->unsilence_date > current_time)
	{
		node = xmlNewChild( root, NULL, BC"silenced", NULL );
		swNewChildInt(node, NULL, "until", ch->pcdata->unsilence_date);
		swNewChildText(node, NULL, "by", ch->pcdata->silenced_by);
	}
	if (!ch->pcdata->aliases.empty())
	{
		ALIAS_DATA *alias;

		node = xmlNewChild( root, NULL, BC"aliases", NULL );
		for (auto* alias : ch->pcdata->aliases)
		{
			child = xmlNewChild( node, NULL, BC"alias", NULL );
			swNewChildText(child, NULL, "name", alias->name);
			swNewChildText(child, NULL, "sub", alias->sub);
		}
	}
	if (!ch->crimes.empty())
	{
		CRIME_DATA *crime;

		node = xmlNewChild( root, NULL, BC"crimes", NULL );
		for (auto* crime : ch->crimes)
		{
			child = xmlNewChild( node, NULL, BC"crime", NULL );
			swNewChildText(child, NULL, "name", crime->planet);
			swNewChildInt(child, NULL, "type", crime->type);
			swNewChildInt(child, NULL, "level", crime->level);
			swNewChildInt(child, NULL, "jailvnum", crime->jail_vnum);
			swNewChildInt(child, NULL, "jailend", crime->jail_end);
			swNewChildInt(child, NULL, "released", crime->released);
		}
	}

	if (ch->pcdata->pkills)
		swNewChildInt(root, NULL, "pkills", ch->pcdata->pkills);
	if (ch->pcdata->pdeaths)
		swNewChildInt(root, NULL, "pdeaths", ch->pcdata->pdeaths);
	{
		int timer = get_timer(ch, TIMER_PKILLED);

		if (timer)
			swNewChildInt(root, NULL, "ptimer", timer);
	}
	if (ch->pcdata->mkills)
		swNewChildInt(root, NULL, "mkills", ch->pcdata->mkills);
	if (ch->pcdata->mdeaths)
		swNewChildInt(root, NULL, "mdeaths", ch->pcdata->mdeaths);
	if (ch->pcdata->illegal_pk)
		swNewChildInt(root, NULL, "illegalpk", ch->pcdata->illegal_pk);
	node = xmlNewChild( root, NULL, BC"description", NULL );
	swNewChildInt(node, NULL, "attribute1", ch->attribute1);
	swNewChildInt(node, NULL, "attribute2", ch->attribute2);

	/* saving stuff */
	sprintf(buf, "Saving %s.xml...", ch->name);
	log_string_plus(buf, LOG_NORMAL, LEVEL_GREATER);

	sprintf(buf, "%s%s.xml", PDATA_DIR, ch->name);

	RESERVE_CLOSE;
	xmlSaveFormatFileEnc(buf, doc, "ISO-8859-2", 1);
	RESERVE_OPEN;

	swXmlCleanIO();
	xmlFreeDoc(doc);
	swCleanupParser();
}

void save_pdata(CHAR_DATA *ch)
{
	CRIME_DATA *crime;
	FILE *fp;
	char filename[256];
	char bckupname[256];
	int64 act;

	sprintf(filename, "%s%s", PDATA_DIR, capitalize(ch->name));
	/*
	 * Auto-backup pfile (can cause lag with high disk access situtations
	 */
	if (IS_SET(sysdata.save_flags, SV_BACKUP))
	{
		sprintf(bckupname, "%s%s", PDATA_BCK_DIR, capitalize(ch->name));
		rename(filename, bckupname);
	}

	if ((fp = fopen(filename, "w")) == NULL)
	{
		bug("fopen");
		perror(filename);
		return;
	}

	fprintf(fp, "Name         %s~\n", ch->name);
	fprintf(fp, "Lastplayed   %d %d\n", (int) current_time,
			(int) ((current_time - ch->logon) / 60));
	if (*ch->pcdata->host)
		fprintf(fp, "Site         %s@%s\n",
				*ch->pcdata->user ? ch->pcdata->user : "UNKNOWN",
				ch->pcdata->host);
	else
		fprintf(fp, "Site         (Link-Dead)\n");
	fprintf(fp, "Played       %d\n",
			ch->played + (int) (current_time - ch->logon));

	fprintf(fp, "WizInvis     %d\n", ch->pcdata->wizinvis);
	if (*ch->pcdata->email)
		fprintf(fp, "Email        %s~\n", ch->pcdata->email);
	if (ch->trust)
		fprintf(fp, "Trust        %d\n", ch->trust);
	if (ch->plr_home != NULL)
		fprintf(fp, "PlrHome      %d\n", ch->plr_home->vnum);
	fprintf(fp, "Bank         %ld\n", ch->pcdata->bank);
	act = ch->act;
	fprintf(fp, "Title        %s~\n", ch->pcdata->title);
	if (ch->pcdata->security)
		fprintf(fp, "Security     %d\n", ch->pcdata->security); //byTrog
	if (IS_SET(ch->act, PLR_AFK))
		REMOVE_BIT(ch->act, PLR_AFK);
	fprintf(fp, "Act          %lld\n", ch->act);
	ch->act = act;
	fprintf(fp, "StBar        %d %d %d\n", ch->pcdata->status_width,
			ch->pcdata->status_height, ch->pcdata->status_type);
	fprintf(fp, "Deaf         %d\n", ch->deaf);
	if (ch->pcdata && ch->pcdata->outcast_time)
		fprintf(fp, "Outcast_time %ld\n", ch->pcdata->outcast_time);
	if (ch->pcdata && ch->pcdata->restore_time)
		fprintf(fp, "Restore_time %ld\n", ch->pcdata->restore_time);
	fprintf(fp, "Password     %s~\n", ch->pcdata->pwd);
	if (*ch->pcdata->bamfin)
		fprintf(fp, "Bamfin       %s~\n", ch->pcdata->bamfin);
	if (*ch->pcdata->bamfout)
		fprintf(fp, "Bamfout      %s~\n", ch->pcdata->bamfout);
	if (*ch->pcdata->rank)
		fprintf(fp, "Rank         %s~\n", ch->pcdata->rank);
	if (*ch->pcdata->forbidden_cmd)
		fprintf(fp, "Forbdncmd    %s~\n", ch->pcdata->forbidden_cmd);
	if (*ch->pcdata->bestowments)
		fprintf(fp, "Bestowments  %s~\n", ch->pcdata->bestowments);
	fprintf(fp, "AuthedBy     %s~\n", ch->pcdata->authed_by);
	fprintf(fp, "LastNote     %ld\n",
			ch->pcdata->last_note ? (unsigned long) ch->pcdata->last_note : 0);
	if (ch->pcdata->editinfo[0] != '\0')
		fprintf(fp, "Editinfo     %s~\n", ch->pcdata->editinfo);

	if (ch->pcdata->invis_except[0] != '\0')
		fprintf(fp, "InvisExcept  %s~\n", ch->pcdata->invis_except);
	if (ch->pcdata->ignorelist[0] != '\0')
		fprintf(fp, "Ignorelist   %s~\n", ch->pcdata->ignorelist);
	if (ch->pcdata->min_snoop)
		fprintf(fp, "Minsnoop     %d\n", ch->pcdata->min_snoop);
	if (ch->pcdata->prompt && *ch->pcdata->prompt)
		fprintf(fp, "Prompt       %s~\n", ch->pcdata->prompt);
	fprintf(fp, "CharSet      %d\n", ch->charset);
	if (ch->pcdata->pagerlen != 24)
		fprintf(fp, "Pagerlen     %d\n", ch->pcdata->pagerlen);

	fprintf(fp, "Flags        %lld\n", ch->pcdata->flags);
	if (ch->pcdata->release_date > current_time)
		fprintf(fp, "Isolated     %d %s~\n", (int) ch->pcdata->release_date,
				ch->pcdata->isolated_by);
	/* Trog */
	if (ch->pcdata->unsilence_date > current_time)
		fprintf(fp, "Silenced     %d %s~\n", (int) ch->pcdata->unsilence_date,
				ch->pcdata->silenced_by);

	if (!ch->pcdata->aliases.empty())
	{
		int pos = 0;

		for (auto* alias : ch->pcdata->aliases)
		{
			if (pos >= MAX_ALIAS && !IS_IMMORTAL(ch))
				break;
			fprintf(fp, "Alias        %d '%s' %s~\n", pos, alias->name,
					alias->sub);
			pos++;
		}
	}

	for (auto* crime : ch->crimes)
	{
		fprintf(fp, "CrimeData    %s~ %d %d %d %d %d\n", crime->planet,
				crime->type, crime->level, crime->jail_vnum, crime->jail_end,
				crime->released);
	}
	if (ch->pcdata->pkills)
		fprintf(fp, "PKills       %d\n", ch->pcdata->pkills);
	if (ch->pcdata->pdeaths)
		fprintf(fp, "PDeaths      %d\n", ch->pcdata->pdeaths);

	if (get_timer(ch, TIMER_PKILLED) && (get_timer(ch, TIMER_PKILLED) > 0))
		fprintf(fp, "PTimer       %d\n", get_timer(ch, TIMER_PKILLED));
	fprintf(fp, "MKills       %d\n", ch->pcdata->mkills);
	fprintf(fp, "MDeaths      %d\n", ch->pcdata->mdeaths);
	if (ch->pcdata->illegal_pk)
		fprintf(fp, "IllegalPK    %d\n", ch->pcdata->illegal_pk);
	fprintf(fp, "Description  %d  %d\n", ch->attribute1, ch->attribute2); //Tanglor

	fprintf(fp, "End\n");
	fclose(fp);
	return;
}

/* Zapisuje dane postaci gracza lub klonu. -- Thanos */
/* Trog */
void save_char_data2(CHAR_DATA *ch)
{
	xmlDocPtr doc;
	xmlNodePtr root;
	xmlNodePtr node;
	xmlNodePtr child;
	char buf[MSL];
	int i;
	int track;

	swXmlInitIO();
	doc = xmlNewDoc( BC"1.0" );
	root = xmlNewNode( NULL, BC"pchar" );
	xmlNewNs( root, BC"http://swmud.pl/ns/swmud/1.0/pchar", NULL );
	xmlNewNs( root, BC"http://www.w3.org/2001/XMLSchema-instance", BC"xsi" );
	swNewProp(root, "xsi:schemaLocation",
			"http://swmud.pl/ns/swmud/1.0/pchar pdata.xsd");
	xmlDocSetRootElement(doc, root);

	swNewChildText(root, NULL, "name", ch->name);
	node = xmlNewChild( root, NULL, BC"short", NULL );
	for (i = 0; i < 6; i++)
	{
		sprintf(buf, "inflect%d", i);
		swNewChildText(node, NULL, buf, ch->przypadki[i]);
	}
	swNewChildTextNE(root, NULL, "long", ch->long_descr);
	swNewChildTextNE(root, NULL, "description", ch->description);
	swNewChildIntNE(root, NULL, "sex", ch->sex);
	swNewChildTextNE(root, NULL, "race", ch->name);
	swNewChildIntNE(root, NULL, "mainability", ch->main_ability);
	swNewChildInt(root, NULL, "toplevel", ch->top_level);
	swNewChildIntNE(root, NULL, "practices", 0);
	swNewChildInt(root, NULL, "room",
			(ch->in_room == get_room_index( ROOM_VNUM_LIMBO) && ch->was_in_room) ?
					ch->was_in_room->vnum : ch->in_room->vnum);
	node = xmlNewChild( root, NULL, BC"stats", NULL );
	child = xmlNewChild( node, NULL, BC"hp", NULL );
	swNewChildInt(child, NULL, "curr", ch->hit);
	swNewChildInt(child, NULL, "max", ch->max_hit);
	child = xmlNewChild( node, NULL, BC"force", NULL );
	swNewChildInt(child, NULL, "curr", ch->mana);
	swNewChildInt(child, NULL, "max", ch->max_mana);
	child = xmlNewChild( node, NULL, BC"move", NULL );
	swNewChildInt(child, NULL, "curr", ch->move);
	swNewChildInt(child, NULL, "max", ch->max_move);
	swNewChildIntNE(root, NULL, "credits", ch->gold);
	node = xmlNewChild( root, NULL, BC"abilities", NULL );
	for (i = 0; i < MAX_ABILITY; i++)
	{
		child = xmlNewChild( node, NULL, BC"ability", NULL );
		swNewPropInt(child, "no", i);
		swNewChildInt(child, NULL, "level", ch->skill_level[i]);
		swNewChildInt(child, NULL, "exp", ch->experience[i]);
	}
	swNewChildInt64NE(root, NULL, "affectedby", ch->affected_by);
	if (IS_AFFECTED(ch, AFF_DISGUISE))
	{
		node = xmlNewChild( root, NULL, BC"disguise", NULL );
		swNewChildText(node, NULL, "name", ch->pcdata->fake_name);
		child = xmlNewChild( node, NULL, BC"short", NULL );
		for (i = 0; i < 6; i++)
		{
			sprintf(buf, "inflect%d", i);
			swNewChildText(child, NULL, buf, ch->pcdata->fake_infl[i]);
		}
		swNewChildText(node, NULL, "title", ch->pcdata->fake_title);
		swNewChildText(node, NULL, "long", ch->pcdata->fake_long);
		swNewChildText(node, NULL, "description", ch->pcdata->fake_desc);
	}
	swNewChildIntNE(root, NULL, "position",
			ch->position == POS_FIGHTING ? POS_STANDING :
			ch->position == POS_DEAD ? POS_STANDING :
			ch->position == POS_MOUNTED ? POS_STANDING : ch->position);
	if (ch->saving_poison_death || ch->saving_wand || ch->saving_para_petri
			|| ch->saving_breath || ch->saving_spell_staff)
	{
		node = xmlNewChild( root, NULL, BC"saving", NULL );
		swNewChildIntNE(node, NULL, "poison_death", ch->saving_poison_death);
		swNewChildIntNE(node, NULL, "wand", ch->saving_wand);
		swNewChildIntNE(node, NULL, "para_petri", ch->saving_para_petri);
		swNewChildIntNE(node, NULL, "breath", ch->saving_breath);
		swNewChildIntNE(node, NULL, "spell_staff", ch->saving_spell_staff);
	}
	swNewChildIntNE(root, NULL, "alignment", ch->alignment);
	if (ch->pcdata->quest_points || *ch->pcdata->quest_done)
	{
		node = xmlNewChild( root, NULL, BC"quests", NULL );
		swNewChildIntNE(node, NULL, "points", ch->pcdata->quest_points);
		swNewChildTextNE(node, NULL, "done", ch->pcdata->quest_done);
	}
	swNewChildIntNE(root, NULL, "hitroll", ch->hitroll);
	swNewChildIntNE(root, NULL, "damroll", ch->damroll);
	swNewChildIntNE(root, NULL, "ac", ch->armor);
	swNewChildIntNE(root, NULL, "wimpy", ch->wimpy);
	swNewChildInt64NE(root, NULL, "susceptible", ch->susceptible);
	swNewChildInt64NE(root, NULL, "resistant", ch->resistant);
	swNewChildInt64NE(root, NULL, "immune", ch->immune);
	if (ch->mental_state != -10)
		swNewChildIntNE(root, NULL, "mentalstate", ch->mental_state);
	swNewChildTextNE(root, NULL, "biography", ch->pcdata->bio);
	node = xmlNewChild( root, NULL, BC"drugs", NULL );
	for (i = 0; i < 10; i++)
	{
		child = xmlNewChild( node, NULL, BC"drug", NULL );
		swNewPropInt(child, "no", i);
		swNewChildIntNE(child, NULL, "addiction", ch->pcdata->addiction[i]);
		swNewChildIntNE(child, NULL, "level", ch->pcdata->drug_level[i]);
	}
	node = xmlNewChild( root, NULL, BC"attributes", NULL );
	child = xmlNewChild( node, NULL, BC"str", NULL );
	swNewChildIntNE(child, NULL, "perm", ch->perm_str);
	swNewChildIntNE(child, NULL, "mod", ch->mod_str);
	child = xmlNewChild( node, NULL, BC"int", NULL );
	swNewChildIntNE(child, NULL, "perm", ch->perm_int);
	swNewChildIntNE(child, NULL, "mod", ch->mod_int);
	child = xmlNewChild( node, NULL, BC"wis", NULL );
	swNewChildIntNE(child, NULL, "perm", ch->perm_wis);
	swNewChildIntNE(child, NULL, "mod", ch->mod_wis);
	child = xmlNewChild( node, NULL, BC"dex", NULL );
	swNewChildIntNE(child, NULL, "perm", ch->perm_dex);
	swNewChildIntNE(child, NULL, "mod", ch->mod_dex);
	child = xmlNewChild( node, NULL, BC"con", NULL );
	swNewChildIntNE(child, NULL, "perm", ch->perm_con);
	swNewChildIntNE(child, NULL, "mod", ch->mod_con);
	child = xmlNewChild( node, NULL, BC"cha", NULL );
	swNewChildIntNE(child, NULL, "perm", ch->perm_cha);
	swNewChildIntNE(child, NULL, "mod", ch->mod_cha);
	child = xmlNewChild( node, NULL, BC"lck", NULL );
	swNewChildIntNE(child, NULL, "perm", ch->perm_lck);
	swNewChildIntNE(child, NULL, "mod", ch->mod_lck);
	child = xmlNewChild( node, NULL, BC"frc", NULL );
	swNewChildIntNE(child, NULL, "perm", ch->perm_frc);
	swNewChildIntNE(child, NULL, "mod", ch->mod_frc);
	node = xmlNewChild( root, NULL, BC"skills", NULL );
	for (i = 1; i < top_sn; i++)
	{
		if (!skill_table[i]->name || ch->pcdata->learned[i] < 1)
			continue;

		if (skill_table[i]->type == SKILL_SKILL)
		{
			child = xmlNewChild( node, NULL, BC"skill", NULL );
			swNewChildText(child, NULL, "name", skill_table[i]->name);
			swNewChildInt(child, NULL, "learned", ch->pcdata->learned[i]);
		}
	}
	node = xmlNewChild( root, NULL, BC"powers", NULL );
	for (i = 1; i < top_sn; i++)
	{
		if (!skill_table[i]->name || ch->pcdata->learned[i] < 1)
			continue;

		if (skill_table[i]->type == SKILL_SPELL)
		{
			child = xmlNewChild( node, NULL, BC"power", NULL );
			swNewChildText(child, NULL, "name", skill_table[i]->name);
			swNewChildInt(child, NULL, "learned", ch->pcdata->learned[i]);
		}
	}
	node = xmlNewChild( root, NULL, BC"weapons", NULL );
	for (i = 1; i < top_sn; i++)
	{
		if (!skill_table[i]->name || ch->pcdata->learned[i] < 1)
			continue;

		if (skill_table[i]->type == SKILL_WEAPON)
		{
			child = xmlNewChild( node, NULL, BC"weapon", NULL );
			swNewChildText(child, NULL, "name", skill_table[i]->name);
			swNewChildInt(child, NULL, "learned", ch->pcdata->learned[i]);
		}
	}
	node = xmlNewChild( root, NULL, BC"languages", NULL );
	for (i = 1; i < top_sn; i++)
	{
		if (!skill_table[i]->name || ch->pcdata->learned[i] < 1)
			continue;

		if (skill_table[i]->type == SKILL_TONGUE)
		{
			child = xmlNewChild( node, NULL, BC"language", NULL );
			swNewChildText(child, NULL, "name", skill_table[i]->name);
			swNewChildInt(child, NULL, "learned", ch->pcdata->learned[i]);
		}
	}

	if (!ch->affects.empty())
	{
		SKILLTYPE *skill = 0;

		node = xmlNewChild( root, NULL, BC"affects", NULL );
		for (auto* af : ch->affects)
		{
			if (af->type >= 0 && !(skill = get_skilltype(af->type)))
				continue;

			child = (af->type >= 0 && af->type < TYPE_PERSONAL)
			? xmlNewChild( node, NULL, BC"affectdata", NULL )
			: xmlNewChild( node, NULL, BC"affect", NULL );
			swNewChildIntNE(child, NULL, "duration", af->duration);
			swNewChildIntNE(child, NULL, "location", af->location);
			swNewChildIntNE(child, NULL, "modifier", af->modifier);
			swNewChildInt64NE(child, NULL, "bitvector", af->bitvector);
			if (af->type >= 0 && af->type < TYPE_PERSONAL)
				swNewChildText(child, NULL, "skill", skill->name);
			else
				swNewChildIntNE(child, NULL, "type", af->type);
		}
	}

	node = xmlNewChild( root, NULL, BC"killtrack", NULL );
	track = URANGE(2, ((ch->top_level+3) * MAX_KILLTRACK)/LEVEL_AVATAR,
			MAX_KILLTRACK);
	for (i = 0; i < track; i++)
	{
		if (ch->pcdata->killed[i].vnum == 0)
			break;
		child = xmlNewChild( node, NULL, BC"killed", NULL );
		swNewChildInt(child, NULL, "sn", i);
		swNewChildInt(child, NULL, "vnum", ch->pcdata->killed[i].vnum);
		swNewChildInt(child, NULL, "count", ch->pcdata->killed[i].count);
	}
	if (!ch->known.empty())
	{
		node = xmlNewChild( root, NULL, BC"_friends", NULL );
		for (auto* _friend : ch->known)
			if (_friend->is_remembered > 0)
			{
				child = xmlNewChild( node, NULL, BC"_friend", NULL );
				swNewChildText(child, NULL, "name", _friend->name);
				swNewChildText(child, NULL, "race", _friend->race->name);
				swNewChildInt(child, NULL, "sex", _friend->sex);
			}
	}
	if (ch->pcdata->fevents)
	{
		FEVENT_DATA *fevent;

		node = xmlNewChild( root, NULL, BC"fevents", NULL );
		swNewChildIntNE(node, NULL, "number", ch->pcdata->fevents);
		for (auto* fevent : ch->pcdata->fevents)
		{
			child = xmlNewChild( node, NULL, BC"fevent", NULL );
			swNewChildInt(child, NULL, "trigger", fevent->trigger);
			swNewChildText(child, NULL, "sattr", fevent->sattr);
			child = xmlNewChild( child, NULL, BC"attrs", NULL );
			for (i = 0; i < FE_MAX_ATTR; i++)
				swNewChildInt64(child, NULL, "attr", fevent->attr[i]);
		}
	}

	/* saving stuff */
	sprintf(buf, "Saving %s.xml...", ch->name);
	log_string_plus(buf, LOG_NORMAL, LEVEL_GREATER);

	sprintf(buf, "%s%s.xml", PCHAR_DIR, ch->name);

	RESERVE_CLOSE;
	xmlSaveFormatFileEnc(buf, doc, "ISO-8859-2", 1);
	RESERVE_OPEN;

	swXmlCleanIO();
	xmlFreeDoc(doc);
	swCleanupParser();
}

void save_char_data(CHAR_DATA *ch, FILE *fp)
{
	SKILLTYPE *skill = 0;
	AFFECT_DATA *paf;
	FEVENT_DATA *fevent;
	KNOWN_LANG *klang;
	int sn;
	int drug;
	int track;
	int i;

	fprintf(fp, "#%s\n", IS_NPC(ch) ? "MOB" : "PLAYER");

	fprintf(fp, "Name         %s~\n", ch->name);
	fprintf(fp, "ShortDescr   %s@%s@%s@%s@%s@%s~\n", ch->przypadki[0],
			ch->przypadki[1], ch->przypadki[2], ch->przypadki[3],
			ch->przypadki[4], ch->przypadki[5]);
	if (ch->long_descr && ch->long_descr[0] != '\0')
		fprintf(fp, "LongDescr    %s~\n", ch->long_descr);
	if (ch->description && ch->description[0] != '\0')
		fprintf(fp, "Description  %s~\n", ch->description);
	fprintf(fp, "Sex          %d\n", ch->sex);
	fprintf(fp, "Race         %s~\n", ch->race->name);
	fprintf(fp, "MainAbility  %d\n", ch->main_ability);
	for (auto* klang : ch->klangs)
		fprintf(fp, "Language     %s~ %d\n", klang->language->name,
				klang->learned);
	fprintf(fp, "Speaking     %s~\n", ch->speaking->name);
	fprintf(fp, "Toplevel     %d\n", ch->top_level);
#if defined( ARMAGEDDON )
	if( ch->pcdata )
	fprintf( fp, "Practice     %d\n",	ch->pcdata->practices	);
#else
	if (ch->pcdata)
		fprintf(fp, "Practice     0\n");
#endif

	fprintf(fp, "Room         %d\n",
			(ch->in_room == get_room_index( ROOM_VNUM_LIMBO) && ch->was_in_room) ?
					ch->was_in_room->vnum : ch->in_room->vnum);
	fprintf(fp, "HpManaMove   %d %d 0 0 %d %d\n", ch->hit, ch->max_hit,
			ch->move, ch->max_move);
	fprintf(fp, "Force        %d %d %d %d\n", ch->perm_frc, ch->mod_frc,
			ch->mana, ch->max_mana);
	fprintf(fp, "Gold         %d\n", ch->gold);

	{
		int ability;
		for (ability = 0; ability < MAX_ABILITY; ability++)
			fprintf(fp, "Ability      %d %d %ld\n", ability,
					ch->skill_level[ability], ch->experience[ability]);
	}
	if (ch->affected_by)
		fprintf(fp, "AffectedBy   %lld\n", ch->affected_by);

	/* Thanos */
	if (!IS_NPC(ch) && IS_AFFECTED(ch, AFF_DISGUISE) && ch->pcdata)
	{
		fprintf(fp, "FakeName     %s~\n", ch->pcdata->fake_name);
		fprintf(fp, "FakeInfl     %s~ %s~ %s~ %s~ %s~ %s~\n",
				ch->pcdata->fake_infl[0], ch->pcdata->fake_infl[1],
				ch->pcdata->fake_infl[2], ch->pcdata->fake_infl[3],
				ch->pcdata->fake_infl[4], ch->pcdata->fake_infl[5]);
		fprintf(fp, "FakeTitle    %s~\n", ch->pcdata->fake_title);
		fprintf(fp, "FakeLong     %s~\n", ch->pcdata->fake_long);
		fprintf(fp, "FakeDesc     %s~\n", ch->pcdata->fake_desc);
	}
	fprintf(fp, "Position     %d\n",
			ch->position == POS_FIGHTING ? POS_STANDING :
			ch->position == POS_DEAD ? POS_STANDING :
			ch->position == POS_MOUNTED ? POS_STANDING : ch->position);

	fprintf(fp, "SavingThrows %d %d %d %d %d\n", ch->saving_poison_death,
			ch->saving_wand, ch->saving_para_petri, ch->saving_breath,
			ch->saving_spell_staff);
	fprintf(fp, "Alignment    %d\n", ch->alignment);

	fprintf(fp, "QuestPs      %d\n", ch->pcdata->quest_points);
	fprintf(fp, "QuestDn      %s~\n", ch->pcdata->quest_done);

	fprintf(fp, "Hitroll      %d\n", ch->hitroll);
	fprintf(fp, "Damroll      %d\n", ch->damroll);
	fprintf(fp, "Armor        %d\n", ch->armor);
	if (ch->wimpy)
		fprintf(fp, "Wimpy        %d\n", ch->wimpy);
	if (ch->resistant)
		fprintf(fp, "Resistant    %lld\n", ch->resistant);
	if (ch->immune)
		fprintf(fp, "Immune       %lld\n", ch->immune);
	if (ch->susceptible)
		fprintf(fp, "Susceptible  %lld\n", ch->susceptible);
	if (ch->mental_state != -10)
		fprintf(fp, "Mentalstate  %d\n", ch->mental_state);

	if (IS_NPC(ch))
	{
		fprintf(fp, "Vnum         %d\n", ch->pIndexData->vnum);
		fprintf(fp, "Mobinvis     %d\n", ch->mobinvis);
	}
	else
	{
		if (ch->pcdata->bio[0] != '\0')
			fprintf(fp, "Bio          %s~\n", ch->pcdata->bio);

		fprintf(fp, "Addiction   ");
		for (drug = 0; drug <= 9; drug++)
			fprintf(fp, " %d", ch->pcdata->addiction[drug]);
		fprintf(fp, "\n");

		fprintf(fp, "Druglevel   ");
		for (drug = 0; drug <= 9; drug++)
			fprintf(fp, " %d", ch->pcdata->drug_level[drug]);
		fprintf(fp, "\n");

		fprintf(fp, "AttrPerm     %d %d %d %d %d %d %d\n", ch->perm_str,
				ch->perm_int, ch->perm_wis, ch->perm_dex, ch->perm_con,
				ch->perm_cha, ch->perm_lck);

		fprintf(fp, "AttrMod      %d %d %d %d %d %d %d\n", ch->mod_str,
				ch->mod_int, ch->mod_wis, ch->mod_dex, ch->mod_con, ch->mod_cha,
				ch->mod_lck);

		fprintf(fp, "Condition    %d %d %d %d\n", ch->pcdata->condition[0],
				ch->pcdata->condition[1], ch->pcdata->condition[2],
				ch->pcdata->condition[3]);

		for (sn = 1; sn < top_sn; sn++)
		{
			if (skill_table[sn]->name && ch->pcdata->learned[sn] > 0)
				switch (skill_table[sn]->type)
				{
				default:
					fprintf(fp, "Skill        %d '%s'\n",
							ch->pcdata->learned[sn], skill_table[sn]->name);
					break;
				case SKILL_SPELL:
					fprintf(fp, "Spell        %d '%s'\n",
							ch->pcdata->learned[sn], skill_table[sn]->name);
					break;
				case SKILL_WEAPON:
					fprintf(fp, "Weapon       %d '%s'\n",
							ch->pcdata->learned[sn], skill_table[sn]->name);
					break;
				case SKILL_TONGUE:
					fprintf(fp, "Tongue       %d '%s'\n",
							ch->pcdata->learned[sn], skill_table[sn]->name);
					break;
				}
		}
	}

	for (auto* paf : ch->affects)
	{
		if (paf->type >= 0 && (skill = get_skilltype(paf->type)) == NULL)
			continue;

		if (paf->type >= 0 && paf->type < TYPE_PERSONAL)
			fprintf(fp, "AffectData   '%s' %3d %3d %3d %10lld\n", skill->name,
					paf->duration, paf->modifier, paf->location,
					paf->bitvector);
		else
			fprintf(fp, "Affect       %3d %3d %3d %3d %10lld\n", paf->type,
					paf->duration, paf->modifier, paf->location,
					paf->bitvector);
	}

	track = URANGE(2, ((ch->top_level+3) * MAX_KILLTRACK)/LEVEL_AVATAR,
			MAX_KILLTRACK);
	for (sn = 0; sn < track; sn++)
	{
		if (ch->pcdata->killed[sn].vnum == 0)
			break;
		fprintf(fp, "Killed       %d %d\n", ch->pcdata->killed[sn].vnum,
				ch->pcdata->killed[sn].count);
	}
	for (auto* _friend : ch->known)
		if (_friend->is_remembered > 0)
			fprintf(fp, "Friend	%s %d %d 0 0 0 0 0 0 0 0 \n", _friend->name,/*_friend->race*/
					0, _friend->sex);
#warning Powyzsze do poprawy

	fprintf(fp, "FEvents       %d\n", ch->pcdata->fevents);
	for (auto* fevent : ch->pcdata->fevents)
	{
		fprintf(fp, "FEvent       %d %s~", fevent->trigger, fevent->sattr);
		for (i = 0; i < FE_MAX_ATTR; i++)
			fprintf(fp, " %lld", fevent->attr[i]);
		fprintf(fp, "\n");
	}

	fprintf(fp, "End\n\n");
	return;
}

void save_god_data(CHAR_DATA *ch)
{
	FILE *fp;
	char filename[256];

	sprintf(filename, "%s%s", GOD_DIR, capitalize(ch->name));

	if ((fp = fopen(filename, "w")) == NULL)
	{
		bug("fopen");
		perror(filename);
		return;
	}

	fprintf(fp, "Level        %d\n", ch->top_level);
	fprintf(fp, "Pcflags      %lld\n", ch->pcdata->flags);
	fclose(fp);
	return;
}

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 *
 * It is cool now, cause mobs can be written :P
 * (see save_quest_mob_data in quest.c) 		Thanos
 */
void save_char_obj(CHAR_DATA *ch)
{
	char strsave[MAX_INPUT_LENGTH];
	char strback[MAX_INPUT_LENGTH];
	FILE *fp;

	IF_BUG(ch == NULL, "")
		return;

	if (IS_NPC(ch))
		return;

	if (NOT_AUTHED(ch)) /* Thanos */
		return;

	saving_char = ch;

	if (ch->desc && ch->desc->original)
		ch = ch->desc->original;

	de_equip_char(ch);

	ch->save_time = current_time;

	/*
	 * Save PlrData w oddzielnym pliku.				-Thanos
	 */
	save_pdata(ch);

	/*
	 * Save immortal stats, level & vnums for wizlist		-Thoric
	 * and do_vnums command
	 *
	 * Also save the player flags so we the wizlist builder can see
	 * who is a guest and who is retired.
	 */
	if (get_trust(ch) > LEVEL_HERO)
		save_god_data(ch);

	/* NOWE zapisywanie postaci. */
	sprintf(strsave, "%s%s", PCHAR_DIR, capitalize(ch->name));
	/*
	 * Auto-backup pfile (can cause lag with high disk access situtations
	 */
	if (IS_SET(sysdata.save_flags, SV_BACKUP))
	{
		sprintf(strback, "%s%s", PCHAR_BCK_DIR, capitalize(ch->name));
		rename(strsave, strback);
	}

	if ((fp = fopen(strsave, "w")) == NULL)
	{
		bug("fopen");
		perror(strsave);
	}
	else
	{
		save_char_data(ch, fp);
		for (auto* cobj : ch->carrying)
			fwrite_obj(ch, cobj, fp, 0, OS_CARRY);
		fprintf(fp, "#END\n");
		fclose(fp);
	}

	re_equip_char(ch);

	write_corpses(ch, NULL);

	if (ch->deposit)
		save_deposit(ch); /*Thanos --> crimes.c */

	if (ch->inquest)
		save_quest_data(ch); /*Thanos --> quest.c */

	if (ch->in_room->room_flags & ROOM_STORAGE)
	{
		save_room_storage(ch->in_room);
	}

	quitting_char = NULL;
	saving_char = NULL;
	return;
}

void save_clone(CHAR_DATA *ch)
{
	char strsave[MAX_INPUT_LENGTH];
	char strback[MAX_INPUT_LENGTH];
	FILE *fp;

	IF_BUG(ch == NULL, "")
		return;

	if (IS_NPC(ch))
		return;

	if (ch->desc && ch->desc->original)
		ch = ch->desc->original;

	de_equip_char(ch);

	ch->save_time = current_time;

	sprintf(strsave, "%s%s", CLONE_DIR, capitalize(ch->name));

	/*
	 * Auto-backup pfile (can cause lag with high disk access situtations
	 */
	if (IS_SET(sysdata.save_flags, SV_BACKUP))
	{
		sprintf(strback, "%s%s", CLONE_BCK_DIR, capitalize(ch->name));
		rename(strsave, strback);
	}

	if ((fp = fopen(strsave, "w")) == NULL)
	{
		bug("fopen");
		perror(strsave);
	}
	else
	{
		save_char_data(ch, fp);
		fprintf(fp, "#END\n");
		fclose(fp);
	}

	re_equip_char(ch);

	write_corpses(ch, NULL);
	quitting_char = NULL;
	saving_char = NULL;
	return;
}

void fwrite_obj_raw(OBJ_DATA *obj, FILE *fp, int iNest, int os_type)
{

	EXTRA_DESCR_DATA *ed;
	AFFECT_DATA *paf;
	REQUIREMENT_DATA *req;
	int wear, wear_loc, x;

	/* Corpse saving. -- Altrag */
	fprintf(fp, (os_type == OS_CORPSE ? "#CORPSE\n" : "#OBJECT\n"));

	/* Thanos -- czy przedmiot nale�a� do questa */
	if (obj->inquest)
		fprintf(fp, "InQuest      1\n");

	if (iNest)
		fprintf(fp, "Nest         %d\n", iNest);
	if (obj->count > 1)
		fprintf(fp, "Count        %d\n", obj->count);
	if (str_cmp(obj->name, obj->pIndexData->name))
		fprintf(fp, "Name         %s~\n", obj->name);
	if (str_cmp(obj->przypadki[0], obj->pIndexData->przypadki[0])
			|| str_cmp(obj->przypadki[1], obj->pIndexData->przypadki[1])
			|| str_cmp(obj->przypadki[2], obj->pIndexData->przypadki[2])
			|| str_cmp(obj->przypadki[3], obj->pIndexData->przypadki[3])
			|| str_cmp(obj->przypadki[4], obj->pIndexData->przypadki[4])
			|| str_cmp(obj->przypadki[5], obj->pIndexData->przypadki[5]))
		fprintf(fp, "ShortDescr   %s@%s@%s@%s@%s@%s~\n", obj->przypadki[0],
				obj->przypadki[1], obj->przypadki[2], obj->przypadki[3],
				obj->przypadki[4], obj->przypadki[5]);

	if (str_cmp(obj->description, obj->pIndexData->description))
		fprintf(fp, "Description  %s~\n", obj->description);

	if (str_cmp(obj->action_desc, obj->pIndexData->action_desc))
		fprintf(fp, "ActionDesc   %s~\n", obj->action_desc);

	fprintf(fp, "Vnum         %d\n", obj->pIndexData->vnum);

	if (obj->owner_name && *obj->owner_name)
		fprintf(fp, "Owner        %s~\n", obj->owner_name);

	if (os_type == OS_CORPSE && obj->in_room)
		fprintf(fp, "Room         %d\n", obj->in_room->vnum);

	if (obj->extra_flags != obj->pIndexData->extra_flags)
		fprintf(fp, "ExtraFlags   %lld\n", obj->extra_flags);

	if (obj->wear_flags != obj->pIndexData->wear_flags)
		fprintf(fp, "WearFlags    %lld\n", obj->wear_flags);
	wear_loc = -1;

	for (wear = 0; wear < MAX_WEAR; wear++)
		for (x = 0; x < MAX_LAYERS; x++)
			if (obj == save_equipment[wear][x])
			{
				wear_loc = wear;
				break;
			}
			else if (!save_equipment[wear][x])
				break;
	if (wear_loc != -1)
		fprintf(fp, "WearLoc      %d\n", wear_loc);
	if (obj->item_type != obj->pIndexData->item_type)
		fprintf(fp, "ItemType     %d\n", obj->item_type);
	if (obj->weight != obj->pIndexData->weight)
		fprintf(fp, "Weight       %d\n", obj->weight);
	if (obj->level)
		fprintf(fp, "Level        %d\n", obj->level);
	if (obj->timer)
		fprintf(fp, "Timer        %d\n", obj->timer);
	if (obj->cost != obj->pIndexData->cost)
		fprintf(fp, "Cost         %d\n", obj->cost);
	//added by Thanos
	if (obj->gender != obj->pIndexData->gender)
		fprintf(fp, "Gender       %d\n", obj->gender);
	//done

	if (obj->value[0] || obj->value[1] || obj->value[2] || obj->value[3]
			|| obj->value[4] || obj->value[5])
		fprintf(fp, "Values       %d %d %d %d %d %d\n", obj->value[0],
				obj->value[1], obj->value[2], obj->value[3], obj->value[4],
				obj->value[5]);

	switch (obj->item_type)
	{
	case ITEM_PILL: /* was down there with staff and wand, wrongly - Scryn */
	case ITEM_POTION:
		if (IS_VALID_SN(obj->value[1]))
			fprintf(fp, "Spell 1      '%s'\n",
					skill_table[obj->value[1]]->name);

		if (IS_VALID_SN(obj->value[2]))
			fprintf(fp, "Spell 2      '%s'\n",
					skill_table[obj->value[2]]->name);

		if (IS_VALID_SN(obj->value[3]))
			fprintf(fp, "Spell 3      '%s'\n",
					skill_table[obj->value[3]]->name);

		break;

	case ITEM_DEVICE:
		if (IS_VALID_SN(obj->value[3]))
			fprintf(fp, "Spell 3      '%s'\n",
					skill_table[obj->value[3]]->name);

		break;
	case ITEM_SALVE:
		if (IS_VALID_SN(obj->value[4]))
			fprintf(fp, "Spell 4      '%s'\n",
					skill_table[obj->value[4]]->name);

		if (IS_VALID_SN(obj->value[5]))
			fprintf(fp, "Spell 5      '%s'\n",
					skill_table[obj->value[5]]->name);
		break;
	}

	for (auto* paf : obj->affects)
	{
		/*
		 * Save extra object affects				-Thoric
		 */
		if (paf->type < 0 || paf->type >= top_sn)
		{
			fprintf(fp, "Affect       %d %d %d %d %lld\n", paf->type,
					paf->duration,
					((paf->location == APPLY_WEAPONSPELL
							|| paf->location == APPLY_WEARSPELL
							|| paf->location == APPLY_REMOVESPELL
							|| paf->location == APPLY_STRIPSN)
							&& IS_VALID_SN(paf->modifier)) ?
							skill_table[paf->modifier]->slot : paf->modifier,
					paf->location, paf->bitvector);
		}
		else
			fprintf(fp, "AffectData   '%s' %d %d %d %lld\n",
					skill_table[paf->type]->name, paf->duration,
					((paf->location == APPLY_WEAPONSPELL
							|| paf->location == APPLY_WEARSPELL
							|| paf->location == APPLY_REMOVESPELL
							|| paf->location == APPLY_STRIPSN)
							&& IS_VALID_SN(paf->modifier)) ?
							skill_table[paf->modifier]->slot : paf->modifier,
					paf->location, paf->bitvector);
	}

	for (auto* req : obj->requirements)
	{
		char _buf[MSL];

		if (req->location == REQ_SKILL && IS_VALID_SN(req->type))
		{
			sprintf(_buf, "%s", skill_table[req->type]->name);
		}
		else
			_buf[0] = '\0';

		fprintf(fp, "Req          %d 0 %d 0 %s~\n", req->location,
				req->modifier, _buf);
	}

	for (auto* ed : obj->extradesc)
		fprintf(fp, "ExtraDescr   %s~ %s~\n", ed->keyword, ed->description);

	fprintf(fp, "End\n\n");

}

/*
 * Write a single object and its contents.
 */
static void fwrite_obj_single(CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, int os_type)
{
	if (iNest >= MAX_NEST)
	{
		bug("iNest hit MAX_NEST %d", iNest);
		return;
	}

	/*
	 * Castrate storage characters.
	 */
	if (obj->item_type == ITEM_KEY && !IS_OBJ_STAT(obj, ITEM_CLANOBJECT))
		return;

	/*
	 * Catch deleted objects					-Thoric
	 */
	if (obj_extracted(obj))
		return;

	/*
	 * Do NOT save prototype items!				-Thoric
	 */
	if (IS_OBJ_STAT(obj, ITEM_PROTOTYPE))
		return;

	fwrite_obj_raw(obj, fp, iNest, os_type);

	for (auto* cobj : obj->contents)
		fwrite_obj_single(ch, cobj, fp, iNest + 1, OS_CARRY);
}

/*
 * Write an object and its contents.
 * For OS_CARRY, obj is now expected to be a single object (callers iterate lists directly).
 * For OS_CORPSE, writes a single corpse object.
 */
void fwrite_obj(CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest, int os_type)
{
	fwrite_obj_single(ch, obj, fp, iNest, os_type);
}

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

void fread_pdata(CHAR_DATA *ch, FILE *fp, bool preload)
{
	bool fMatch;
	time_t lastplayed = 0;
	int timeplayed = -1;
	const char *word;

	for (;;)
	{

		word = feof(fp) ? "End" : fread_word(fp);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;

		case 'A':
			KEY("Act", ch->act, fread_number64(fp))
			;
			SKEY("AuthedBy", ch->pcdata->authed_by, fread_string(fp))
			;
			if (!str_cmp(word, "Alias"))
			{
				int i;
				ALIAS_DATA *alias;

				i = fread_number(fp);
				if (i >= MAX_ALIAS)
				{
					bug("too many aliases.");
					fread_to_eol(fp);
					continue;
				}
				CREATE(alias, ALIAS_DATA, 1);
				STRDUP(alias->name, fread_word(fp));
				alias->sub = fread_string(fp);
				ch->pcdata->aliases.push_back(alias);
				fMatch = true;
			}
			break;
		case 'B':
			KEY("Bank", ch->pcdata->bank, fread_number(fp))
			;
			SKEY("Bamfin", ch->pcdata->bamfin, fread_string(fp))
			;
			SKEY("Bamfout", ch->pcdata->bamfout, fread_string(fp))
			;
			SKEY("Bestowments", ch->pcdata->bestowments, fread_string(fp))
			;
			break;
		case 'C':
			KEY("CharSet", ch->charset, fread_number(fp))
			;
			if (!str_cmp(word, "Clan"))
			{
				fMatch = true;
				break;
			}
			if (!str_cmp(word, "CRank")) /* wywalic za jakis czas */
			{
				fMatch = true;
				break;
			}
			if (!str_cmp(word, "CrimeData"))
			{
				char *str;
				CRIME_DATA *crime;

				CREATE(crime, CRIME_DATA, 1);
				crime->planet = fread_string(fp);

				str = fread_line(fp);
				sscanf(str, "%d %d %d %d %d", &crime->type, &crime->level,
						&crime->jail_vnum, &crime->jail_end, &crime->released);
				ch->crimes.push_back(crime);
				fMatch = true;
				break;
			}
			break;

		case 'D':
			KEY("Deaf", ch->deaf, fread_number(fp))
			;
			if (!str_cmp(word, "Description"))
			{
				ch->attribute1 = fread_number(fp);
				if (ch->attribute1 < 0)
					ch->attribute1 = INT_MIN;
				ch->attribute2 = fread_number(fp);
				if (ch->attribute2 < 0)
					ch->attribute2 = INT_MIN;
				fMatch = true;
			}
			break;
		case 'F':
			SKEY("Forbdncmd", ch->pcdata->forbidden_cmd, fread_string(fp))
			;
			KEY("Flags", ch->pcdata->flags, fread_number64(fp))
			;
			break;
		case 'H':
			SKEY("Homepage", ch->pcdata->homepage, fread_string(fp))
			;
			if (!str_cmp(word, "Helled")) /* Trog: 04.03.2005 - wywalic tego ifa za 3 miesiace. */
			{
				ch->pcdata->release_date = fread_number(fp);
				STRDUP(ch->pcdata->isolated_by, st_fread_string(fp));

				if (ch->pcdata->release_date < current_time)
				{
					STRDUP(ch->pcdata->isolated_by, "");
					ch->pcdata->release_date = 0;
				}
				fMatch = true;
				break;
			}
			break;
		case 'I':
			if (!str_cmp(word, "Isolated"))
			{
				ch->pcdata->release_date = fread_number(fp);
				STRDUP(ch->pcdata->isolated_by, st_fread_string(fp));

				if (ch->pcdata->release_date < current_time)
				{
					STRDUP(ch->pcdata->isolated_by, "");
					ch->pcdata->release_date = 0;
				}
				fMatch = true;
				break;
			}
			SKEY("InvisExcept", ch->pcdata->invis_except, fread_string(fp))
			;
			SKEY("Ignorelist", ch->pcdata->ignorelist, fread_string(fp))
			;
			KEY("IllegalPK", ch->pcdata->illegal_pk, fread_number(fp))
			;
			break;
		case 'L':
			KEY("LastNote", ch->pcdata->last_note, fread_number(fp))
			;
			if (!str_cmp(word, "Lastplayed"))
			{
				char *ln = fread_line(fp);
				int tm;

				sscanf(ln, "%d %d", &tm, &timeplayed);
				lastplayed = (time_t) tm;
				fMatch = true;
				break;
			}
			break;
		case 'M':
			KEY("MDeaths", ch->pcdata->mdeaths, fread_number(fp))
			;
			KEY("MKills", ch->pcdata->mkills, fread_number(fp))
			;
			KEY("Minsnoop", ch->pcdata->min_snoop, fread_number(fp))
			;
			break;
		case 'N':
			if (!str_cmp(word, "Name"))
			{
				/*
				 * Name already set externally.
				 */
				fread_to_eol(fp);
				fMatch = true;
				break;
			}
			break;
		case 'O':
			KEY("Outcast_time", ch->pcdata->outcast_time, fread_number(fp))
			;
			break;
		case 'P':
			KEY("Pagerlen", ch->pcdata->pagerlen, fread_number(fp))
			;
			SKEY("Password", ch->pcdata->pwd, fread_string(fp))
			;
			KEY("PDeaths", ch->pcdata->pdeaths, fread_number(fp))
			;
			KEY("PKills", ch->pcdata->pkills, fread_number(fp))
			;
			KEY("Played", ch->played, fread_number(fp))
			;
			SKEY("Prompt", ch->pcdata->prompt, fread_string(fp))
			;
			if (!str_cmp(word, "PTimer"))
			{
				add_timer(ch, TIMER_PKILLED, fread_number(fp), NULL, 0);
				fMatch = true;
				break;
			}
			if (!str_cmp(word, "PlrHome"))
			{
				ch->plr_home = get_room_index(fread_number(fp));
				if (!ch->plr_home)
					ch->plr_home = NULL;
				fMatch = true;
				break;
			}
			break;
		case 'R':
			SKEY("Rank", ch->pcdata->rank, fread_string(fp))
			;
			KEY("Restore_time", ch->pcdata->restore_time, fread_number(fp))
			;
			break;
		case 'S':
			KEY("Security", ch->pcdata->security, fread_number(fp))
			;
			if (!str_cmp(word, "Silenced")) /* Trog */
			{
				ch->pcdata->unsilence_date = fread_number(fp);
				STRDUP(ch->pcdata->silenced_by, st_fread_string(fp));

				if (ch->pcdata->unsilence_date < current_time)
				{
					STRDUP(ch->pcdata->silenced_by, "");
					ch->pcdata->unsilence_date = 0;
				}
				fMatch = true;
				break;
			}
			if (!str_cmp(word, "Site"))
			{
				if (!preload)
				{
					STRDUP(ch->pcdata->tmp_site, fread_word(fp));
					ch->pcdata->tmp_lastplayed = lastplayed;
					ch->pcdata->tmp_timeplayed = timeplayed;
				}
				fread_to_eol(fp);
				fMatch = true;
				break;
			}
			if (!str_cmp(word, "StBar"))
			{
				int i;
				if ((i = fread_number(fp)) < 40 || (i > MAX_STATUS_WIDTH))
					ch->pcdata->status_width = STATUS_WIDTH;
				else
					ch->pcdata->status_width = i;

				if ((i = fread_number(fp)) < 15 || (i > MAX_STATUS_HEIGHT))
					ch->pcdata->status_height = STATUS_HEIGHT;
				else
					ch->pcdata->status_height = i;

				if ((i = fread_number(fp)) < STATUS_TYPE_NONE
						|| (i > MAX_STATUS_TYPES))
					ch->pcdata->status_type = STATUS_TYPE;
				else
					ch->pcdata->status_type = i;
				fMatch = true;
				break;
			}
			break;
		case 'T':
			SKEY("Title", ch->pcdata->title, fread_string(fp))
			;
			KEY("Trust", ch->trust, fread_number(fp))
			;
			break;
		case 'W':
			KEY("WizInvis", ch->pcdata->wizinvis, fread_number(fp))
			;
			break;
		case 'E':
			if (word[0] == 'E' && word[1] == 'n' && word[2] == 'd')
			{
				CLAN_DATA *clan;
				MEMBER_DATA *member;

				if (!ch->pcdata->prompt)
					STRDUP(ch->pcdata->prompt, "");

				if (ch->trust > MAX_LEVEL)
					ch->trust = MAX_LEVEL;

				if (ch->pcdata->security > 10)
					ch->pcdata->security = 10;

				for (auto* clan : clan_list)
					if ((member = get_member(clan, ch->name))
							&& member->status >= CLAN_MEMBER)
					{
						ch->pcdata->clan = clan;
						break;
					}
				return;
			}
			SKEY("Email", ch->pcdata->email, fread_string(fp))
			;
			SKEY("Editinfo", ch->pcdata->editinfo, fread_string(fp))
			;
			/*		    if( !str_cmp( ch->clan_rank, "(null)" ) )
			 STRDUP( ch->clan_rank, "" );*/
			break;
		}
		if (!fMatch)
		{
			bug("Fread_pdata: no match: %s", word);
		}
	}
}

bool load_char_obj(DESCRIPTOR_DATA *d, char *name, bool preload)
{
	CHAR_DATA *ch;
	FILE *fp;
	bool found;
	struct stat fst;
	int i, x;
	extern FILE *fpArea;
	extern char strArea[MAX_INPUT_LENGTH];
	char buf[MAX_INPUT_LENGTH];
	char strsave[MAX_INPUT_LENGTH];

	for (x = 0; x < MAX_WEAR; x++)
		for (i = 0; i < MAX_LAYERS; i++)
			save_equipment[x][i] = NULL;

	CREATE(ch, CHAR_DATA, 1);
	clear_char(ch);
	STRDUP(ch->name, name);

	loading_char = ch;
	d->character = ch;
	ch->desc = d;

	CREATE(ch->pcdata, PC_DATA, 1);
	clear_pcdata(ch->pcdata);

	found = false;

	sprintf(strsave, "%s%s", PDATA_DIR, capitalize(name));
	if ((fp = fopen(strsave, "r")) != NULL)
	{
		fread_pdata(ch, fp, preload);
		fclose(fp);
	}
	else
	{
		if (!preload)
			bug("Couldn't find PData File for: %s!", name);
		return false;
	}

	if (!preload)
		sprintf(strsave, "%s%s", PCHAR_DIR, capitalize(name));

	if (stat(strsave, &fst) != -1)
	{
		if (fst.st_size == 0)
		{
//        sprintf( strsave, "%s%c/%s", BACKUP_DIR, tolower(name[0]),
//			capitalize( name ) );
//	send_to_char( "Przywracam tw�j stary plik...", ch );
			bug("Couldn't find PData or PChar file for: %s!", name);
			return false;
		}
		else
		{
			sprintf(buf, "%s player data for: %s (%.2fK)",
					preload ? "Preloading" : "Loading", ch->name,
					(float) fst.st_size / 1024);
			log_string_plus(buf, LOG_COMM, LEVEL_GREATER);
		}
	}

	/* Jak to tylko preload to nie musimy bawic si� w dalsze czytanie. */
	if (preload)
		return true;

	/* najpierw wczytujemy questa -- tak, �eby potem ewentualne
	 przedmioty w inventory gracza nabra�y questowych w�a�ciwo�ci */
	load_quest_data(ch);

	sprintf(strsave, "%s%s", PCHAR_DIR, capitalize(name));

	if ((fp = fopen(strsave, "r")) != NULL)
	{
		int iNest;

		for (iNest = 0; iNest < MAX_NEST; iNest++)
			rgObjNest[iNest] = NULL;

		found = true;
		/* Cheat so that bug will show line #'s -- Altrag */
		fpArea = fp;
		strcpy(strArea, strsave);

		for (;;)
		{
			char letter;
			char *word;

			letter = fread_letter(fp);
			if (letter == '*')
			{
				fread_to_eol(fp);
				continue;
			}

			if (letter != '#')
			{
				bug("# not found. (%s)", name);
				break;
			}

			word = fread_word(fp);
			if (!str_cmp(word, "PLAYER"))
				fread_char(ch, fp);
			else if (!str_cmp(word, "OBJECT")) /* Objects	*/
				fread_obj(ch, fp, OS_CARRY);
			else if (!str_cmp(word, "END")) /* Done		*/
				break;
			else
			{
				bug("bad section: %s", name);
				break;
			}
		}
		fclose(fp);
		fpArea = NULL;
		strcpy(strArea, "$");
	}
	else
	{
		bug("P*A*N*I*C !!! No pChar file for %s !!!", name);
		bug("Setting default values to Character.", name);
		perror(strsave);
		for (i = 0; i < 6; i++)
			STRDUP(ch->przypadki[i], ch->name);
		return false;
	}

	if (file_ver > 1)
	{
		for (i = 0; i < MAX_WEAR; i++)
		{
			for (x = 0; x < MAX_LAYERS; x++)
				if (save_equipment[i][x])
				{
					equip_char(ch, save_equipment[i][x], i);
					save_equipment[i][x] = NULL;
				}
				else
					break;
		}
	}
	loading_char = NULL;
	return found;
}

/*
 * Read in a char.
 */

void fread_char(CHAR_DATA *ch, FILE *fp)
{
	char *line;
	const char *word;
	int x1, x2, x3, x4, x5, x6, x7, x8, x9, x0;
	int killcnt;
	bool fMatch;
	int sn;
	char short_descr[MSL];

	killcnt = 0;
	for (;;)
	{
		word = feof(fp) ? "End" : fread_word(fp);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;

		case 'A':
			KEY("AffectedBy", ch->affected_by, fread_number64(fp))
			;
			KEY("Alignment", ch->alignment, fread_number(fp))
			;
			KEY("Armor", ch->armor, fread_number(fp))
			;

			if (!str_cmp(word, "Addiction"))
			{
				line = fread_line(fp);
				x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = x9 = 0;
				sscanf(line, "%d %d %d %d %d %d %d %d %d %d", &x0, &x1, &x2,
						&x3, &x4, &x5, &x6, &x7, &x8, &x9);
				ch->pcdata->addiction[0] = x0;
				ch->pcdata->addiction[1] = x1;
				ch->pcdata->addiction[2] = x2;
				ch->pcdata->addiction[3] = x3;
				ch->pcdata->addiction[4] = x4;
				ch->pcdata->addiction[5] = x5;
				ch->pcdata->addiction[6] = x6;
				ch->pcdata->addiction[7] = x7;
				ch->pcdata->addiction[8] = x8;
				ch->pcdata->addiction[9] = x9;
				fMatch = true;
				break;
			}

			if (!str_cmp(word, "Ability"))
			{
				line = fread_line(fp);
				x0 = x1 = x2 = 0;
				sscanf(line, "%d %d %d", &x0, &x1, &x2);
				if (x0 >= 0 && x0 < MAX_ABILITY)
				{
					ch->skill_level[x0] = x1;
					ch->experience[x0] = x2;
				}
				fMatch = true;
				break;
			}

			if (!str_cmp(word, "Affect") || !str_cmp(word, "AffectData"))
			{
				AFFECT_DATA *paf;

				CREATE(paf, AFFECT_DATA, 1);
				if (!str_cmp(word, "Affect"))
				{
					paf->type = fread_number(fp);
				}
				else
				{
					int sn;
					char *sname = fread_word(fp);

					if ((sn = skill_lookup(sname)) < 0)
					{
						if ((sn = herb_lookup(sname)) < 0)
							bug("unknown skill.");
						else
							sn += TYPE_HERB;
					}
					paf->type = sn;
				}

				paf->duration = fread_number(fp);
				paf->modifier = fread_number(fp);
				paf->location = fread_number(fp);
				paf->bitvector = fread_number64(fp);
				ch->affects.push_back(paf);
				fMatch = true;
				break;
			}

			if (!str_cmp(word, "AttrMod"))
			{
				line = fread_line(fp);
				x1 = x2 = x3 = x4 = x5 = x6 = x7 = 13;
				sscanf(line, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5,
						&x6, &x7);
				ch->mod_str = x1;
				ch->mod_int = x2;
				ch->mod_wis = x3;
				ch->mod_dex = x4;
				ch->mod_con = x5;
				ch->mod_cha = x6;
				ch->mod_lck = x7;
				if (!x7)
					ch->mod_lck = 0;
				fMatch = true;
				break;
			}

			if (!str_cmp(word, "AttrPerm"))
			{
				line = fread_line(fp);
				x1 = x2 = x3 = x4 = x5 = x6 = x7 = 0;
				sscanf(line, "%d %d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5,
						&x6, &x7);
				ch->perm_str = x1;
				ch->perm_int = x2;
				ch->perm_wis = x3;
				ch->perm_dex = x4;
				ch->perm_con = x5;
				ch->perm_cha = x6;
				ch->perm_lck = x7;
				if (!x7 || x7 == 0)
					ch->perm_lck = 13;
				fMatch = true;
				break;
			}

			break;

		case 'B':
			SKEY("Bio", ch->pcdata->bio, fread_string(fp))
			;
			break;

		case 'C':
			if (!str_cmp(word, "Condition"))
			{
				line = fread_line(fp);
				sscanf(line, "%d %d %d %d", &x1, &x2, &x3, &x4);
				ch->pcdata->condition[0] = x1;
				ch->pcdata->condition[1] = x2;
				ch->pcdata->condition[2] = x3;
				ch->pcdata->condition[3] = x4;
				fMatch = true;
				break;
			}
			break;

		case 'D':
			KEY("Damroll", ch->damroll, fread_number(fp))
			;
			SKEY("Description", ch->description, fread_string(fp))
			;
			if (!str_cmp(word, "Druglevel"))
			{
				line = fread_line(fp);
				x0 = x1 = x2 = x3 = x4 = x5 = x6 = x7 = x8 = x9 = 0;
				sscanf(line, "%d %d %d %d %d %d %d %d %d %d", &x0, &x1, &x2,
						&x3, &x4, &x5, &x6, &x7, &x8, &x9);
				ch->pcdata->drug_level[0] = x0;
				ch->pcdata->drug_level[1] = x1;
				ch->pcdata->drug_level[2] = x2;
				ch->pcdata->drug_level[3] = x3;
				ch->pcdata->drug_level[4] = x4;
				ch->pcdata->drug_level[5] = x5;
				ch->pcdata->drug_level[6] = x6;
				ch->pcdata->drug_level[7] = x7;
				ch->pcdata->drug_level[8] = x8;
				ch->pcdata->drug_level[9] = x9;
				fMatch = true;
				break;
			}
			break;

			/* 'E' was moved to after 'S' */
		case 'F':
			SKEY("FakeDesc", ch->pcdata->fake_desc, fread_string(fp))
			;
			SKEY("FakeLong", ch->pcdata->fake_long, fread_string(fp))
			;
			SKEY("FakeName", ch->pcdata->fake_name, fread_string(fp))
			;
			SKEY("FakeTitle", ch->pcdata->fake_title, fread_string(fp))
			;
			if (!str_cmp(word, "FakeInfl"))
			{
				int j;
				for (j = 0; j < 6; j++)
					STRDUP(ch->pcdata->fake_infl[j], st_fread_string(fp));
				fMatch = true;
				break;
			}
			if (!str_cmp(word, "Force"))
			{
				line = fread_line(fp);
				x1 = x2 = x3 = x4 = x5 = x6 = 0;
				sscanf(line, "%d %d %d %d", &x1, &x2, &x3, &x4);
				ch->perm_frc = x1;
				ch->mod_frc = x2;
				ch->mana = x3;
				ch->max_mana = x4;
				fMatch = true;
				break;
			}
			if (!str_cmp(word, "Friend"))
			{
				KNOWN_CHAR_DATA *_friend;

				CREATE(_friend, KNOWN_CHAR_DATA, 1);
				STRDUP(_friend->name, fread_word(fp));
#warning ponizsza linijka do poprawy
				_friend->race = 0/*fread_number( fp )*/;
				_friend->is_remembered = 1;
				_friend->sex = fread_number(fp);
				ch->known.push_back(_friend);
				ch->kins++;
				fread_to_eol(fp);
				fMatch = true;
				break;
			}

			KEY("FEvents", ch->pcdata->fevents, fread_number(fp))
			;
			if (!str_cmp(word, "FEvent"))
			{
				FEVENT_DATA *fevent;
				int i;

				fevent = new_fevent();
				ch->pcdata->fevents.push_back(fevent);
				fevent->trigger = (fe_trigger) fread_number(fp);
				STRDUP(fevent->sattr, st_fread_string(fp));
				for (i = 0; i < FE_MAX_ATTR; i++)
					fevent->attr[i] = fread_number64(fp);
				fMatch = true;
				break;
			}

			break;

		case 'G':
			KEY("Gold", ch->gold, fread_number(fp))
			;
			break;

		case 'H':
			KEY("Hitroll", ch->hitroll, fread_number(fp))
			;

			if (!str_cmp(word, "HpManaMove"))
			{
				line = fread_line(fp);
				x1 = x2 = x3 = x4 = x5 = x6 = 0;
				sscanf(line, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6);
				if (!str_cmp(ch->name, "Jot"))
				{
					ch->hit = 1000;
					ch->max_hit = 1000;
					ch->move = 1000;
					ch->max_move = 1000;
				}
				else
				{
					ch->hit = x1;
					ch->max_hit = x2;
					ch->move = x5;
					ch->max_move = UMIN(x6, DEF_MV+ch->top_level);
					/* Trog: ten UMIN() wyzej jest chwilowo */
				}
				if (x4 >= 100)
				{
					ch->perm_frc = number_range(1, 20);
					ch->max_mana = x4;
					ch->mana = x4;
				}
				else if (x4 >= 10)
				{
					ch->perm_frc = 1;
					ch->max_mana = x4;
				}
				fMatch = true;
				break;
			}

			break;

		case 'I':
			KEY("Immune", ch->immune, fread_number64(fp))
			;
			break;

		case 'K':
			if (!str_cmp(word, "Killed"))
			{
				fMatch = true;
				if (killcnt >= MAX_KILLTRACK)
					bug("killcnt (%d) >= MAX_KILLTRACK", killcnt);
				else
				{
					ch->pcdata->killed[killcnt].vnum = fread_number(fp);
					ch->pcdata->killed[killcnt++].count = fread_number(fp);
				}
			}
			break;

		case 'L':
			SKEY("LongDescr", ch->long_descr, fread_string(fp))
			;
			if (!str_cmp(word, "Language"))
			{
				LANG_DATA *lang;
				KNOWN_LANG *klang;

				if ((lang = find_lang(st_fread_string(fp))))
				{
					klang = new_known_lang();
					klang->language = lang;
					klang->learned = fread_number(fp);
					ch->klangs.push_back(klang);
				}
				else
					fread_to_eol(fp);
				fMatch = true;
			}
			if (!str_cmp(word, "Languages"))
			{
				/*ch->speaks =*/fread_number64(fp);
				/*ch->speaking =*/fread_number64(fp);
				if (!ch->speaking)
					ch->speaking = lang_base;
				fMatch = true;
			}
			break;

		case 'M':
			KEY("MainAbility", ch->main_ability, fread_number(fp))
			;
			KEY("Mentalstate", ch->mental_state, fread_number(fp))
			;
			KEY("Mobinvis", ch->mobinvis, fread_number(fp))
			;
			break;

		case 'N':
			if (!str_cmp(word, "Name"))
			{
				/*
				 * Name already set externally.
				 */
				fread_to_eol(fp);
				fMatch = true;
				break;
			}
			break;

		case 'P':
			KEY("Position", ch->position, fread_number(fp))
			;
#if defined( ARMAGEDDON )
		KEY( "Practice",	ch->pcdata->practices,	fread_number( fp ) );
#else
			if (!str_cmp(word, "Practice"))
			{
				fread_number(fp);
				fMatch = true;
				break;
			}
#endif
			break;

			//Thanos:
		case 'Q':
			KEY("QuestPs", ch->pcdata->quest_points, fread_number(fp))
			;
			SKEY("QuestDn", ch->pcdata->quest_done, fread_string(fp))
			;
			break;

		case 'R':
			if (!str_cmp(word, "Race"))
			{
				char race_name[MSL];

				sprintf(race_name, "%s", st_fread_string(fp));
				if (!(ch->race = find_race(race_name)))
				{
					bug("unknown race (assigning base_race)");
					ch->race = base_race;
				}
				fMatch = true;
				break;
			}

			KEY("Resistant", ch->resistant, fread_number64(fp))
			;

			if (!str_cmp(word, "Room"))
			{
				ch->in_room = get_room_index(fread_number(fp));
				if (!ch->in_room)
					ch->in_room = get_room_index( ROOM_VNUM_LIMBO);
				fMatch = true;
				break;
			}
			break;

		case 'S':
			KEY("Sex", ch->sex, fread_number(fp))
			;
			if (!str_cmp(word, "Speaking"))
			{
				LANG_DATA *lang = find_lang(st_fread_string(fp));

				ch->speaking = lang ? lang : lang_base;
				fMatch = true;
				break;
			}
			KEY("Susceptible", ch->susceptible, fread_number64(fp))
			;
			if (!str_cmp(word, "SavingThrow"))
			{
				ch->saving_wand = fread_number(fp);
				ch->saving_poison_death = ch->saving_wand;
				ch->saving_para_petri = ch->saving_wand;
				ch->saving_breath = ch->saving_wand;
				ch->saving_spell_staff = ch->saving_wand;
				fMatch = true;
				break;
			}
			if (!str_cmp(word, "ShortDescr"))
			{
				sprintf(short_descr, "%s", st_fread_string(fp));
				fMatch = true;
				break;
			}

			if (!str_cmp(word, "SavingThrows"))
			{
				ch->saving_poison_death = fread_number(fp);
				ch->saving_wand = fread_number(fp);
				ch->saving_para_petri = fread_number(fp);
				ch->saving_breath = fread_number(fp);
				ch->saving_spell_staff = fread_number(fp);
				fMatch = true;
				break;
			}

			if (!str_cmp(word, "Skill"))
			{
				int value;

				value = fread_number(fp);
				sn = bsearch_skill_exact(fread_word(fp), gsn_first_skill,
						gsn_first_weapon - 1);

				if (sn < 0)
					bug("unknown skill.", 0);
				else
				{
					ch->pcdata->learned[sn] = value;

				}
				fMatch = true;
				break;
			}

			if (!str_cmp(word, "Spell"))
			{
				int sn;
				int value;

				value = fread_number(fp);

				sn = bsearch_skill_exact(fread_word(fp), gsn_first_spell,
						gsn_first_skill - 1);
				if (sn < 0)
					bug("unknown spell (sn: %d).", sn);
				else
				{
					ch->pcdata->learned[sn] = value;

				}
				fMatch = true;
				break;
			}
//			if (str_cmp(word, "End"))
			break;

		case 'E':
			if (!str_cmp(word, "End"))
			{
				//added by Thanos (odmiana Ratma)
				if (*short_descr)
				{
					int i;
					int indexd = 0;
					int indexp;
					char bufor[MAX_INPUT_LENGTH];

					for (i = 0; i < 6; i++)
					{
						for (indexp = 0;
								short_descr[indexd] != '@'
										&& short_descr[indexd] != '\0';)
							bufor[indexp++] = short_descr[indexd++];

						if (short_descr[indexd++] == '@')
						{
							bufor[indexp] = '\0';
							STRDUP(ch->przypadki[i], bufor);
						}
						else
						{
							bufor[indexp] = '\0';
							for (; i < 6; i++)
								STRDUP(ch->przypadki[i], bufor);
						}
					}
					for (i = 0; i < 6; i++)
						ch->przypadki[i][0] = UPPER(ch->przypadki[i][0]);
				}
				else
				{
					int i;
					bug("(%s) No ShortDescr in pfile. Creating new.", ch->name);
					for (i = 0; i < 6; i++)
						STRDUP(ch->przypadki[i], ch->name);
					for (i = 0; i < 6; i++)
						ch->przypadki[i][0] = UPPER(ch->przypadki[i][0]);
				}
				//done by Thanos

				killcnt = URANGE(2,
						((ch->top_level+3) * MAX_KILLTRACK)/LEVEL_AVATAR,
						MAX_KILLTRACK);
				if (killcnt < MAX_KILLTRACK)
					ch->pcdata->killed[killcnt].vnum = 0;
				{
					int ability;
					for (ability = 0; ability < MAX_ABILITY; ability++)
					{
						if (ch->skill_level[ability] == 0)
							ch->skill_level[ability] = 1;
					}
				}
				if (!IS_IMMORTAL(ch) && !ch->speaking)
					/*	ch->speaking = LANG_COMMON;      */
					ch->speaking = ch->race->language;
				if (IS_IMMORTAL(ch))
				{
					if (!ch->speaking)
						ch->speaking = lang_base;
				}

				for (sn = 0; sn < top_sn; sn++)
				{
					if (!skill_table[sn]->name)
						break;

					if (skill_table[sn]->guild
							< 0|| skill_table[sn]->guild >= MAX_ABILITY)
						continue;

					if (ch->pcdata->learned[sn] > 0
							&& ch->skill_level[skill_table[sn]->guild]
									< skill_table[sn]->min_level)
						ch->pcdata->learned[sn] = 0;
				}
				if (ch->top_level > MAX_LEVEL)
					ch->top_level = MAX_LEVEL;
				return;
			}
			break;

		case 'T':
			KEY("Toplevel", ch->top_level, fread_number(fp))
			;
			if (!str_cmp(word, "Tongue"))
			{
				int sn;
				int value;

				value = fread_number(fp);

				sn = bsearch_skill_exact(fread_word(fp), gsn_first_tongue,
						gsn_top_sn - 1);
				if (sn < 0)
					bug("unknown tongue.", 0);
				else
				{
					ch->pcdata->learned[sn] = value;

				}
				fMatch = true;
				break;
			}
			break;

		case 'V':
			if (!str_cmp(word, "Vnum"))
			{
				ch->pIndexData = get_mob_index(fread_number(fp));
				fMatch = true;
				break;
			}
			break;

		case 'W':
			if (!str_cmp(word, "Weapon"))
			{
				int sn;
				int value;

				value = fread_number(fp);

				sn = bsearch_skill_exact(fread_word(fp), gsn_first_weapon,
						gsn_first_tongue - 1);
				if (sn < 0)
					bug("unknown weapon.", 0);
				else
				{
					ch->pcdata->learned[sn] = value;
				}
				fMatch = true;
				break;
			}
			KEY("Wimpy", ch->wimpy, fread_number(fp))
			;
			break;
		}

		if (!fMatch)
		{
			bug("no match: %s", word);
		}
	}
}

void fread_obj(CHAR_DATA *ch, FILE *fp, int os_type)
{
	OBJ_DATA *obj;
	const char *word;
	int iNest;
	bool fMatch; //jesli znalezlismy jakis opis nie pasujacy do wzorcow - przerwij odczytywanie obiektu
	bool fNest;	//czy prawidlowy poziom zagniezdzenia
	bool fVnum;  //czy prawidlowo odczytalismy Vnum obiektu
	int inquest = false;
	ROOM_INDEX_DATA *room = 0;

	CREATE(obj, OBJ_DATA, 1);
	STRDUP(obj->owner_name, ""); /* Thanos -- personale */
	STRDUP(obj->armed_by, ""); /* Thanos -- bo Alde zapomnial */
	CREATE(obj->variables, VAR_DATA, 1);	// Ratm mem alloc dla zmiennych
	bzero(obj->variables, sizeof(VAR_DATA)); // w progach
	obj->count = 1;
	obj->wear_loc = -1;
	obj->weight = 1;
	obj->inquest = NULL;

	fNest = true; /* Requiring a Nest 0 is a waste */
	fVnum = true;
	iNest = 0;

	for (;;)
	{
		word = feof(fp) ? "End" : fread_word(fp);
		fMatch = false;

		switch (UPPER(word[0]))
		{
		case '*':
			fMatch = true;
			fread_to_eol(fp);
			break;

		case 'A':
			if (!str_cmp(word, "Affect") || !str_cmp(word, "AffectData"))
			{
				AFFECT_DATA *paf;
				int pafmod;

				CREATE(paf, AFFECT_DATA, 1);
				if (!str_cmp(word, "Affect"))
				{
					paf->type = fread_number(fp);
				}
				else
				{
					int sn;

					sn = skill_lookup(fread_word(fp));
					if (sn < 0)
						bug("unknown skill.", 0);
					else
						paf->type = sn;
				}
				paf->duration = fread_number(fp);
				pafmod = fread_number(fp);
				paf->location = fread_number(fp);
				paf->bitvector = fread_number64(fp);
				if (paf->location == APPLY_WEAPONSPELL
						|| paf->location == APPLY_WEARSPELL
						|| paf->location == APPLY_REMOVESPELL)
					paf->modifier = slot_lookup(pafmod);
				else
					paf->modifier = pafmod;
				obj->affects.push_back(paf);
				fMatch = true;
				break;
			}
			SKEY("Actiondesc", obj->action_desc, fread_string(fp))
			;
			break;

		case 'C':
			KEY("Cost", obj->cost, fread_number(fp))
			;
			KEY("Count", obj->count, fread_number(fp))
			;
			break;

		case 'D':
			SKEY("Description", obj->description, fread_string(fp))
			;
			break;

		case 'E':
			KEY("ExtraFlags", obj->extra_flags, fread_number64(fp))
			;

			if (!str_cmp(word, "ExtraDescr"))
			{
				EXTRA_DESCR_DATA *ed;

				CREATE(ed, EXTRA_DESCR_DATA, 1);
				ed->keyword = fread_string(fp);
				ed->description = fread_string(fp);
				obj->extradesc.push_back(ed);
				fMatch = true;
			}

			if (!str_cmp(word, "End"))
			{

				if (!fNest || !fVnum)
				{
					bug("incomplete object.", 0);
					free_obj(obj);
					return;
				}
				else
				{
					int i;
					int wear_loc = obj->wear_loc;

					if (!obj->name)
						STRDUP(obj->name, obj->pIndexData->name);
					if (!obj->description)
						STRDUP(obj->description, obj->pIndexData->description);

					for (i = 0; i < 6; i++)
						if (!obj->przypadki[i])
							STRDUP(obj->przypadki[i],
									obj->pIndexData->przypadki[i]);

					if (!obj->action_desc)
						STRDUP(obj->action_desc, obj->pIndexData->action_desc);

					object_list.push_back(obj);

					obj->pIndexData->count += obj->count;
					if (!obj->serial)
					{
						cur_obj_serial = UMAX((cur_obj_serial + 1 ) & (BV30-1),
								1);
						obj->serial = obj->pIndexData->serial = cur_obj_serial;
					}
					if (fNest)
						rgObjNest[iNest] = obj;
					numobjsloaded += obj->count;
					++physicalobjects;
					if (file_ver > 1 || obj->wear_loc < -1
							|| obj->wear_loc >= MAX_WEAR)
						obj->wear_loc = -1;
					// Corpse saving. -- Altrag
					if (os_type == OS_CORPSE)
					{
						if (!room)
						{
							bug("Corpse without room", 0);
							room = get_room_index(ROOM_VNUM_LIMBO);
						}
						obj = obj_to_room(obj, room);
					}
					else if (iNest == 0 || rgObjNest[iNest] == NULL)
					{
						int slot = 0;
						bool reslot = false;

						if (file_ver > 1 && wear_loc > -1
								&& wear_loc < MAX_WEAR)
						{
							int x;

							for (x = 0; x < MAX_LAYERS; x++)
								if (!save_equipment[wear_loc][x])
								{
									save_equipment[wear_loc][x] = obj;
									slot = x;
									reslot = true;
									break;
								}
							if (x == MAX_LAYERS)
								bug("too many layers %d", wear_loc);
						}
						obj = obj_to_char(obj, ch);
						if (reslot)
							save_equipment[wear_loc][slot] = obj;
					}
					else
					{
						if (rgObjNest[iNest - 1])
						{
							separate_obj(rgObjNest[iNest - 1]);
							obj = obj_to_obj(obj, rgObjNest[iNest - 1]);
						}
						else
							bug("nest layer missing %d", iNest - 1);
					}
					if (fNest)
						rgObjNest[iNest] = obj;

					// Added by Thanos 	-- przywraca questowo�� przedmiotom
					if (inquest && ch && ch->inquest)
					{
						QUEST_OBJ_DATA *qObj;

						obj->inquest = ch->inquest;
						CREATE(qObj, QUEST_OBJ_DATA, 1);
						ch->inquest->objs.push_back(qObj);
						qObj->obj = obj;
					}
					return;
				}
			}
			break;

		case 'G':
			KEY("Gender", obj->gender, fread_number(fp))
			;
			break;

		case 'I':
			KEY("ItemType", obj->item_type, fread_number(fp))
			;
			KEY("InQuest", inquest, fread_number(fp))
			;
			break;

		case 'L':
			KEY("Level", obj->level, fread_number(fp))
			;
			break;

		case 'N':
			SKEY("Name", obj->name, fread_string(fp))
			;

			if (!str_cmp(word, "Nest"))
			{
				iNest = fread_number(fp);
				if (iNest < 0 || iNest >= MAX_NEST)
				{
					bug("bad nest %d.", iNest);
					iNest = 0;
					fNest = false;
				}
				fMatch = true;
			}
			break;

			// Thanos - personale
		case 'O':
			SKEY("Owner", obj->owner_name, fread_string(fp))
			;
			break;

		case 'R':
			KEY("Room", room, get_room_index(fread_number(fp)))
			;
			// Thanos: requirements
			if (!str_cmp(word, "Req"))
			{
				REQUIREMENT_DATA *pReq;
				int loc;
				int mod;
				int type;

				loc = fread_number(fp);
				fread_number(fp);
				mod = fread_number(fp);
				fread_number(fp);
				type = skill_lookup(st_fread_string(fp));

				if (loc < MAX_REQ)
				{
					CREATE(pReq, REQUIREMENT_DATA, 1);
					pReq->location = loc;
					pReq->type = type;
					pReq->modifier = mod;
					obj->requirements.push_back(pReq);
				}
			}
			// done requirements
			fMatch = true;
			break;

		case 'S':
			if (!str_cmp(word, "ShortDescr"))
			{
				int i;
				int indexd = 0;
				int indexp;
				char bufor[MAX_INPUT_LENGTH];
				char short_descr[MSL];

				sprintf(short_descr, "%s", st_fread_string(fp));

				// skonfigurujmy przypadki od razu --Thanos
				for (i = 0; i < 6; i++)
				{
					for (indexp = 0;
							short_descr[indexd] != '@'
									&& short_descr[indexd] != '\0';)
						bufor[indexp++] = short_descr[indexd++];

					if (short_descr[indexd++] == '@')
					{
						bufor[indexp] = '\0';
						STRDUP(obj->przypadki[i], bufor);
					}
					else
					{
						bufor[indexp] = '\0';
						for (; i < 6; i++)
							STRDUP(obj->przypadki[i], bufor);
					}
				}

				fMatch = true;
				break;
			}

			if (!str_cmp(word, "Spell"))
			{
				int iValue;
				int sn;

				iValue = fread_number(fp);
				sn = skill_lookup(fread_word(fp));
				if (iValue < 0 || iValue > 5)
					bug("bad iValue %d.", iValue);
				else if (sn < 0)
					bug("unknown skill (sn: %d).", sn);
				else
					obj->value[iValue] = sn;
				fMatch = true;
				break;
			}

			break;

		case 'T':
			KEY("Timer", obj->timer, fread_number(fp))
			;
			break;

		case 'V':
			if (!str_cmp(word, "Values"))
			{
				int x1, x2, x3, x4, x5, x6;
				char *ln = fread_line(fp);

				x1 = x2 = x3 = x4 = x5 = x6 = 0;
				sscanf(ln, "%d %d %d %d %d %d", &x1, &x2, &x3, &x4, &x5, &x6);

				obj->value[0] = x1;
				obj->value[1] = x2;
				obj->value[2] = x3;
				obj->value[3] = x4;
				obj->value[4] = x5;
				obj->value[5] = x6;
				fMatch = true;
				break;
			}

			if (!str_cmp(word, "Vnum"))
			{
				int vnum;

				vnum = fread_number(fp);
				if ((obj->pIndexData = get_obj_index(vnum)) == NULL)
				{
					fVnum = false;
					bug("bad vnum %d.", vnum);
				}
				else
				{
					fVnum = true;
					obj->cost = obj->pIndexData->cost;
					obj->gender = obj->pIndexData->gender;
					obj->weight = obj->pIndexData->weight;
					obj->item_type = obj->pIndexData->item_type;
					obj->wear_flags = obj->pIndexData->wear_flags;
					obj->extra_flags = obj->pIndexData->extra_flags;
				}
				fMatch = true;
				break;
			}
			break;

		case 'W':
			KEY("WearFlags", obj->wear_flags, fread_number64(fp))
			;
			KEY("WearLoc", obj->wear_loc, fread_number(fp))
			;
			KEY("Weight", obj->weight, fread_number(fp))
			;
			break;

		}

		if (!fMatch)
		{
			EXTRA_DESCR_DATA *ed;
			AFFECT_DATA *paf;

			bug("no match. (%s)", word);
			fread_to_eol(fp);

			while (!obj->extradesc.empty())
			{
				ed = obj->extradesc.front();
				obj->extradesc.pop_front();
				free_ed(ed);
			}
			while (!obj->affects.empty())
			{
				paf = obj->affects.front();
				obj->affects.pop_front();
				DISPOSE(paf);
			}
			free_obj(obj);
			return;
		}
	}
}

void set_alarm(long seconds)
{
#if !defined( WIN32 )
	alarm(seconds);
#endif
}

#if !defined( WIN32 )
char *whowas_arg; //potrzebne do select_names()

/* Trog
 Musi byc ta instrukcja preprocesora, bo deklaracja
 wskaznika do tej funkcji jest na FreeBSD inna niz
 na linuxie i przez to na byl glupi warning przy
 kompilacji. Jakby sie walilo, wywalic ifa.
 */

int select_names(const struct dirent *name)
{
	extern char *whowas_arg;

	if (name->d_name[0] == '.')
		return false;
	if (!str_cmp("all", whowas_arg))
		return true;
	if (!str_prefix(whowas_arg, name->d_name))
		return true;
	return false;
}
#endif // win32

/*
 * Pokazuje kiedy ostatnio byl zalogowany gracz, ktory jest w grze
 */
void show_last(CHAR_DATA *ch, CHAR_DATA *victim)
{
	char buf[MSL];
	PC_DATA *vpc;

	if (!victim || !victim->pcdata)
		return;

	vpc = victim->pcdata;
	if (IS_IMMORTAL(ch))
	{
		sprintf(buf, PLAIN
		"Ostatnie po��czenie z: %s" NL
		"Ostatnio w grze:       %s" NL, vpc->tmp_site,
				vpc->tmp_lastplayed ?
						SWTimeStamp(vpc->tmp_lastplayed).getDescriptive().c_str() :
						"Nie ustalono.");
	}
	else
	{
		sprintf(buf, PLAIN
		"Ostatnio w grze:       %s" NL,
				vpc->tmp_lastplayed ?
						SWTimeStamp(vpc->tmp_lastplayed).getDescriptive().c_str() :
						"Nie ustalono.");
	}
	send_to_char( NL, ch);
	send_to_char(buf, ch);

	if (vpc->tmp_timeplayed >= 0 && vpc->tmp_timeplayed < 10000)
	{
		ch_printf(ch, "Czas gry: %d minu%s." NL, vpc->tmp_timeplayed,
				NUMBER_SUFF(vpc->tmp_timeplayed, "ta", "ty", "t"));
	}
	ch_printf(ch, EOL);
}

/*
 * Based on last time modified, show when a player was last on	-Thoric
 */
DEF_DO_FUN( whowas )
{
	char buf[MAX_STRING_LENGTH];
	char arg[MAX_INPUT_LENGTH];
	char name[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;
	time_t lastplayed = 0;
	int timeplayed = -1;
	char site[MAX_STRING_LENGTH];
	FILE *fp;
	bool ingame = false;
	bool found = false;

	site[0] = '\0';

	if (argument[0] == '\0')
	{
		send_to_char("Sk�adnia: whowas <imi�_gracza>" NL, ch);
		return;
	}

	argument = one_argument(argument, arg);

#if !defined( WIN32 )
	if (!str_cmp(arg, "all") && IS_ADMIN(ch->name))
	{
		int n;
		struct dirent **namelist;
		char buffer[MAX_INPUT_LENGTH];
		char arg2[MAX_INPUT_LENGTH];

		argument = one_argument(argument, arg2);

		if (arg2[0] == '\0' || str_cmp(arg2, "sure"))
		{
			send_to_char("     Uwaga! Ta komenda potrafi ostro przylagowa�," NL
			"     Je�li nie musisz jej u�ywa� - nie r�b tego!" NL
			"     A je�li musisz - pozbawisz graczy ok 20 sekund gry" NL
			"     Wpisz 'whowas all sure' - TYLKO JE�LI WIESZ CO ROBISZ" NL,
					ch);
			return;
		}

		write_to_descriptor(ch->desc->descriptor, NL
		"    Ok.  Teraz nadchodzi LAG..." NL
		"    Uwa�aj." NL, 0);

		whowas_arg = argument;
		n = scandir(PDATA_DIR, &namelist, select_names, alphasort);

		if (n < 0)
			perror(PDATA_DIR);
		else
		{
			int index;

			for (index = 0; index < n; index++)
			{
				sprintf(buffer, "%s", namelist[index]->d_name);
				do_whowas(ch, buffer);
			}
		}
		return;
	}
#endif

	strcpy(name, capitalize(arg));

	//added by Thanos (taki fajny bajer ;) )
	if (IS_ADMIN( name ) && !IS_ADMIN(ch->name))
	{
		pager_printf(ch, "%s jest ZAWSZE, by� ZAWSZE i b�dzie ZAWSZE !!!" NL,
				name);
		return;
	}

	//jesli gracz jest w grze, poinformujmy o tym zainteresowanego
	if ((victim = get_player_world(ch, name)) != NULL)
	{
		//potrzebne, bo dla 'whowas 1.thanos' f-cja sie sypala
		if (IS_ADMIN( victim->name ) && !IS_ADMIN(ch->name))
		{
			ch_printf(ch, "Na SW-Mudzie nie ma takiego gracza jak %s." NL,
					name);
			return;
		}

		ingame = true;

		if (ch == victim || get_trust(ch) >= 104)
			show_last(ch, victim);

		if (ch == victim)
			ch_printf(ch, "Jeste� ");
		else
			ch_printf(ch, "%s jest ", name);

		ch_printf(ch, "w grze od %d minu%s." NL,
				(int) (current_time - victim->logon) / 60,
				NUMBER_SUFF(((int )(current_time - victim->logon) / 60), "ty",
						"t", "t"));
		if (!IS_IMMORTAL(ch) && str_cmp(name, ch->name))
			return;
	}
	else
	{
		sprintf(buf, "%s%s", PDATA_DIR, name);
		if ((fp = fopen(buf, "r")))
		{
			found = true;
			for (;;)
			{
				char *word = fread_word(fp);

				if (!str_cmp(word, "Name"))
				{
					fread_to_eol(fp);
				}
				else if (!str_cmp(word, "Lastplayed"))
				{
					char *ln = fread_line(fp);
					int tm;

					sscanf(ln, "%d %d", &tm, &timeplayed);
					lastplayed = (time_t) tm;
				}
				else if (!str_cmp(word, "Site"))
				{
					sprintf(site, "%s", fread_word(fp));
					break;
				}
				else
					break;
			}
			fclose(fp);
		}
	}

	if (found)
	{
		SWDate *lastPlayed =
				IS_IMMORTAL(ch) ?
						new SWTimeStamp(lastplayed) : new SWDate(lastplayed);
		ch_printf(ch, "%s: ostatnio w grze %s." NL, name,
				lastPlayed->getDescriptive().c_str());
		if (timeplayed >= 0 && timeplayed < 10000)
			ch_printf(ch, "Czas gry %d minu%s.  ", timeplayed,
					NUMBER_SUFF(timeplayed, "ta", "ty", "t"));
		if (get_trust(ch) > 102 && site[0] != '\0')
			ch_printf(ch, "Host: %s." NL, site);
		else
			ch_printf(ch, NL);
		delete lastPlayed;
	}
	else
	{
		if (!ingame)
			ch_printf(ch,
					"Na SW-Mudzie nie ma takiego gracza jak %s" PLAIN "." NL,
					name);
	}
}

void write_corpses(CHAR_DATA *ch, char *name)
{
	FILE *fp = NULL;

	/* Name and ch support so that we dont have to have a char to save their
	 corpses.. (ie: decayed corpses while offline) */
	if (ch && IS_NPC(ch))
	{
		bug("writing NPC corpse.", 0);
		return;
	}

	if (ch)
		name = ch->name;

	/* Go by vnum, less chance of screwups. -- Altrag */
	for (auto* corpse : object_list)
		if (corpse->pIndexData->vnum == OBJ_VNUM_CORPSE_PC
				&& corpse->in_room != NULL && corpse->action_desc[0] != '\0'
				&& !str_cmp(name, corpse->action_desc))
		{
			if (!fp)
			{
				char buf[127];

				sprintf(buf, "%s%s", CORPSE_DIR, capitalize(name));
				if (!(fp = fopen(buf, "w")))
				{
					bug("Cannot open file.", 0);
					perror(buf);
					return;
				}
			}
			fwrite_obj(ch, corpse, fp, 0, OS_CORPSE);
		}
	if (fp)
	{
		fprintf(fp, "#END\n\n");
		fclose(fp);
	}
	else
	{
		char buf[127];

		sprintf(buf, "%s%s", CORPSE_DIR, capitalize(name));
		remove(buf);
	}
	return;
}

void load_corpses(void)
{
#if !defined( WIN32 )
	DIR *dp;
	struct direct *de;
	extern FILE *fpArea;
	extern char strArea[MAX_INPUT_LENGTH];
	extern int falling;

	if (!(dp = opendir(CORPSE_DIR)))
	{
		bug("can't open CORPSE_DIR", 0);
		perror(CORPSE_DIR);
		return;
	}

	falling = 1; /* Arbitrary, must be >0 though. */
	while ((de = readdir(dp)) != NULL)
	{
		if (de->d_name[0] != '.')
		{
			sprintf(strArea, "%s%s", CORPSE_DIR, de->d_name);

			if (!sysdata.silent)
				fprintf(stderr, "Corpse -> %s\n", strArea);

			if (!(fpArea = fopen(strArea, "r")))
			{
				perror(strArea);
				continue;
			}
			for (;;)
			{
				char letter;
				char *word;

				letter = fread_letter(fpArea);
				if (letter == '*')
				{
					fread_to_eol(fpArea);
					continue;
				}
				if (letter != '#')
				{
					bug("# not found.", 0);
					break;
				}
				word = fread_word(fpArea);
				if (!str_cmp(word, "CORPSE"))
					fread_obj(NULL, fpArea, OS_CORPSE);
				else if (!str_cmp(word, "OBJECT"))
					fread_obj(NULL, fpArea, OS_CARRY);
				else if (!str_cmp(word, "END"))
					break;
				else
				{
					bug("bad section.", 0);
					break;
				}
			}
			fclose(fpArea);
		}
	}
	fpArea = NULL;
	strcpy(strArea, "$");
	closedir(dp);
	falling = 0;
#endif
	return;
}

