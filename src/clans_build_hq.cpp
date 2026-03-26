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
*       		Space OLC Module Added by Thanos		   *
****************************************************************************/

#include <math.h>
#include "olc.h"
enum
{
	CLAN_HQ_ENTRANCE = 0,
	CLAN_HQ_CORRIDOR
};

const   int   revdir      []      =
{
   2, 3, 0, 1, 5, 4, 9, 8, 7, 6, 10
};

extern char * make_stock_state_info(char * state_name);
extern void terminal_button(CHAR_DATA * ch, char * text, int spaces, char * color);



int get_dir( char *txt );
void hq_build_area(CHAR_DATA *ch);


int hq_build_highest_vnum( CHAR_DATA * ch )
{
	RID *		pRoom=NULL;
	CLAN_DATA * pClan=NULL;
	HQ_DATA *	pHq=NULL;
	int			vnum=0;

	pClan = ch->pcdata->clan;
	if ( pClan == NULL)
	{
//		log_string();
		return -1;
	}
	for (auto* pHq : pClan->hqs)
		for(auto* pRoom : pHq->rooms)
		{
			if (pRoom->vnum > vnum)
				vnum = pRoom->vnum;
		}
	return vnum+1;
}


/**
* @brief Return clan room descriptions
*
* @param ch person which wants to build
* @param type type of building permision
*
* @return this person can or can't perform this building operation
*/
HQ_ROOM_DESC * hq_build_get_room_type(CLAN_DATA * ch, int room_type)
{
	HQ_ROOM_DESC * pHqRoom = NULL;
	for (auto* pHqRoom : hq_room_desc_list)
	{
		if (room_type == pHqRoom->type)
			break;
	}
	return pHqRoom;
}

/**
* @brief Check HQ building permision
*
* @param ch person which wants to build
* @param type type of building permision
*
* @return this person can or can't perform this building operation
*/
bool	hq_build_check_permision( CHAR_DATA * ch, int type)
{
	CLAN_DATA * pClan = NULL;
	if ( IS_NPC( ch ) )
		return false;
	pClan = ch->pcdata->clan;
	if ( pClan == NULL )
		return false;
	return true;
};

EXIT_DATA * hq_build_exit(CHAR_DATA *ch, ROOM_INDEX_DATA * pRoom, int door)
{
	EXIT_DATA *	pExit = NULL;
	RID *		pBackRoom = NULL;

	pBackRoom = ch->in_room;
	pExit = get_exit(pRoom,revdir[door]);
	if (pExit)
	{
		ch_printf(ch, "Z pomieszczenia docelowego prowadzi ju� wyj�cie w kierunku Twojego pomieszczenia");
		return NULL;
	}
	pExit = make_exit(pBackRoom, pRoom, door);
	make_exit(pRoom, pBackRoom, revdir[door]);
	return pExit;
};

void hq_build_remove_room(CHAR_DATA *ch, EXIT_DATA * pExit)
{
//	HQ_ROOM_DESC * pHqRoom = NULL;
	HQ_DATA *	pHq = NULL;
	EXIT_DATA *	pDestExit = NULL;
	RID *		pRoom = NULL;
	RID *		pDestRoom = NULL;

	pHq = (HQ_DATA*)ch->desc->olc_editing;

	pRoom = ch->in_room;
	pDestRoom = pExit->to_room;

	//musimy sprawdzi� ka�de pomieszczenie s�siaduj�ce z usuwanym
	//je�li kt�rekolwiek z pomieszczen ma wi�cej ni� jedno z wyj��
	//przerwij proces usuwania
	if (!pDestRoom)
	{
		bug("Exit pointer isn't pointing to NULL room");
		return;
	}
	if (pDestRoom->exits.size() <= 1 )
	{
		//mo�emy usuwa� pomieszczenie
		pHq->rooms.remove(pDestRoom);
		unlink_room(pDestRoom);
		free_room(pDestRoom);
		if (pExit)
		{
			pRoom->exits.remove(pExit);
			free_exit(pExit);
		}
	}
	else
	{
		ch_printf(ch,"Nie mo�esz usun�� pomieszczenie, do kt�rego" NL
				"prowadzi wi�cej ni� jedno przej�cie" NL NL);
		return;
	}
}

/**
* @brief Builds room in given direction and enters in this room
*
* @param ch - player
* @param door - direction in which we want to create room
* @param enter - move to newly created room ?
*/
void hq_build_room(CHAR_DATA * ch, int door,bool enter, int room_type )
{
	HQ_ROOM_DESC * pHqRoom = NULL;
	HQ_DATA *	pHq = NULL;
	EXIT_DATA *	pExit = NULL;
	RID *		pRoom = NULL;
	char		buf[100];
	int			vnum=0;

	if (IS_NPC(ch)) return;

	pHq = (HQ_DATA*)ch->desc->olc_editing;

	//pobierz szczegolowy opis pomieszczenia
	pHqRoom = hq_build_get_room_type(ch->pcdata->clan, room_type);
	if (pHqRoom == NULL )
	{
		bug("This HQ type room doesn't exits");
		return;
	}

	sprintf(buf, "%s", pHqRoom->room_name);
/* Trog: zakomentowalem, bo nie ma w kodzie produkcyjnym a generuja warninga i mnie wqrwia
	if (pHqRoom->type == CLAN_HQ_ENTRANCE)
		strcat(buf, ch->pcdata->clan->name);
*/
	vnum = hq_build_highest_vnum(ch);

	pRoom = make_room(vnum,0);
	STRDUP(pRoom->name,buf);
	STRDUP(pRoom->description,pHqRoom->room_desc);

	pRoom->area = ch->pcdata->area;
	pHq->rooms.push_back(pRoom);
	pExit = hq_build_exit(ch, pRoom, door);
	//skoro chcemy wejsc do lokacji - to smialo
	if (pExit && enter)
		move_char(ch, pExit, 0);
}

/**
* @brief Builds entrance to base from current location
*
* @param ch
* @param argument
*/
void hq_build_entrance(CHAR_DATA * ch, char * argument)
{
	HQ_DATA *	pHq = NULL;
	CLAN_DATA * pClan=NULL;
	int			door;

	if (IS_NPC(ch))
		return;
	pClan = ch->pcdata->clan;

	hq_build_area(ch);

	door = get_dir( argument );
	if (door == 10 ) return;

	pHq = new_hq_data();
	pClan->hqs.push_back(pHq);
	ch->desc->olc_editing	= (void*)pHq;

	hq_build_room( ch, door, true, CLAN_HQ_ENTRANCE );
}

bool hq_build_dir_free(CHAR_DATA * ch, char * arg)
{
	EXIT_DATA		* pexit = NULL;
	int				door;

	if (arg == NULL || !str_cmp(arg,""))
		return false;

	door = get_dir( arg );
	if ( door == 10 )
	{
		huh(ch);
		return false;
	}
	if ( (pexit = get_exit( ch->in_room, door )) == NULL )
	{
		return true;
	}
	return false;
}

bool hq_build_check_cash(CHAR_DATA * ch)
{
	return true;
}
bool hq_build_planet_free(CHAR_DATA * ch)
{

	return true;
}

void hq_build_show_rooms_list( CHAR_DATA * ch)
{
	HQ_DATA *	pHq = NULL;
//	EXIT_DATA *	pExit = NULL;
	RID *		pRoom = NULL;

	if (IS_NPC(ch)) return;

	pHq = (HQ_DATA*)ch->desc->olc_editing;
	ch_printf(ch, "  [Vnum][%-40s]" NL,"Nazwa");
	ch_printf(ch, "  [-----------------------------------------------]" NL);
	for(auto* pRoom : pHq->rooms)
	{
		ch_printf(ch, "  [%4d][%-40s]" NL,pRoom->vnum,pRoom->name);
	}
	ch_printf(ch, NL NL NL);
}

bool hq_build_is_clan_room(CHAR_DATA * ch, bool only_this_hq)
{
	RID *		pRoom=NULL;
	RID *		pRoom1=NULL;
	CLAN_DATA * pClan=NULL;
	HQ_DATA *	pHq=NULL;

	pClan = ch->pcdata->clan;
	if ( pClan == NULL)
	{
//		log_string();
		return -1;
	}
	pRoom1 = ch->in_room;
	if (only_this_hq)
	{
		pHq = (HQ_DATA*)ch->desc->olc_editing;
		for(auto* pRoom : pHq->rooms)
		{
			if (pRoom == pRoom1)
			{
				ch->desc->olc_editing	= (void*)pHq;
				return true;
			}
		}
	}
	else
	{
	for (auto* pHq : pClan->hqs)
		for(auto* pRoom : pHq->rooms)
		{
			if (pRoom == pRoom1)
			{
				ch->desc->olc_editing	= (void*)pHq;
				return true;
			}
		}
	}
	return false;
}

void build_hq(DESCRIPTOR_DATA * d, char * argument)
{
	CHAR_DATA		* ch = d->character;
	char			arg[MSL];
	char			arg1[MIL];
	char			arg2[MIL];
	HQ_DATA *	pHq = NULL;
	int				door,vnum  = 0;
	EXIT_DATA *		pExit = NULL;
	EXIT_DATA *		pDestExit = NULL;
	RID *			pRoom = NULL;

	pHq = (HQ_DATA*)ch->desc->olc_editing;

	strcpy( arg, argument );
	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );

	if (IS_NPC(ch))
		return;

	//teraz robimy interpretacje dost�pnych polecen buduj�cych

	if ( arg1[0]=='\0' )
	{
		ch_printf(ch, "Dost�pne polecenia : " NL);
		ch_printf(ch, "      croom new <dir>" NL
					  "  lub croom delete <dir>" NL);
		ch_printf(ch, "      cexit new <dir> <vnum>" NL
					  "  lub cexit delete <dir>" NL);
		ch_printf(ch, "      clist" NL);
		ch_printf(ch, "      cfold" NL);
		return;
	}
	else if ( !str_prefix( arg1, "clist" ) )
	{
		hq_build_show_rooms_list(ch);
		return;
	}
	else if ( !str_prefix( arg1, "cfold" ) )
	{
		ch_printf(ch,"Baza zapisana" NL);
		return;
	}
	//tworzymy nowy pokoj
    if ( !str_prefix( arg1, "croom" ) )
    {
		if ( arg2[0]!='\0' )
		{
			if ( !str_prefix( arg2, "new" ) )
			{
				//sprawdz kierunek
				if (hq_build_dir_free(ch, argument) == false)
				{
					ch_printf(ch, "W tym kierunku istnieje juz przej�cie. Musisz wybra� inny" NL);
					return;
				}
				else
				{
					door = get_dir( argument );
					hq_build_room( ch, door, true, CLAN_HQ_CORRIDOR );
					return;
				}

			}
			else if ( !str_prefix( arg2, "delete" ) )
			{
				if (hq_build_dir_free(ch, argument) == false)
				{
					door = get_dir( argument );
					pExit = get_exit( ch->in_room, door );
					hq_build_remove_room(ch, pExit);
					return;
				}
				else
				{
					ch_printf(ch, "W tym kierunku nie ma �adnego przej�cia. Musisz wybra� inny" NL);
					return;
				}
			}
		}
		ch_printf(ch, "Sk�adnia to croom new <dir>" NL
					  "        lub croom delete <dir>" NL);
        return;
    }
	//tworzymy przejscie
	else if ( !str_prefix( arg1, "cexit" ) )
    {
		if ( arg2[0]!='\0' )
		{
			if ( !str_prefix( arg2, "new" ) )
			{
				//sprawdz kierunek
				if (hq_build_dir_free(ch, argument) == false)
				{
					ch_printf(ch, "W tym kierunku istnieje juz przej�cie. Musisz wybra� inny" NL);
					return;
				}
				else
				{
					door = get_dir( argument );
					argument = one_argument( argument, arg2 );
					argument = one_argument( argument, arg2 );
					if (!is_number(arg2))
					{
						ch_printf(ch, "Zabrak�o numeru Vnum do pomieszczenia docelowego" NL);
						return;
					}
					vnum = atoi(arg2);
					for(auto* pRoom : pHq->rooms)
					{
						if (pRoom->vnum == vnum)
							break;
					}
					if (!pRoom)
					{
						ch_printf(ch, "W tej bazie nie istnieje pomieszczenie o takim numerze porz�dkowym" NL);
						return;
					}
					if (hq_build_exit( ch, pRoom, door))
						ch_printf(ch, "Przej�cie zosta�o utworzone" NL);
					return;
				}

			}
			else if ( !str_prefix( arg2, "delete" ) )
			{
				if (hq_build_dir_free(ch, argument) == false)
				{
					door = get_dir( argument );
					pExit = get_exit( ch->in_room, door );
					pDestExit = get_exit( pExit->to_room, revdir[door]);
					if (pDestExit)
					{
						extract_exit(pExit->to_room,pDestExit);
						extract_exit(ch->in_room, pExit);
						ch_printf(ch,"Przej�cie zosta�o usuni�te" NL);
					}
					else
					{
						ch_printf(ch,"Niestety dosz�o do b��du." NL
								"Przej�cie z lokacji docelowej wskazuje w z�� stron�" NL
								"Budowniczy tego kompleksu doprowadzi� do karygodnych b��d�w" NL);
						bug("Clan %s, HeadQuater");
					}
					return;
				}
				else
				{
					ch_printf(ch, "W tym kierunku nie ma �adnego przej�cia. Musisz wybra� inny" NL);
					return;
				}
			}
		}
		ch_printf(ch, "Sk�adnia to cexit new <dir> <vnum>" NL
					  "        lub cexit delete <dir>" NL);
        return;
    }

	GET_DONE();
	interpret( ch, arg );
	//trzeba dodac test sprawdzajacy czy gracz opuscil obszar bazy
	if ( hq_build_is_clan_room(ch,true) == false )
	{
		ch_printf( ch,NL "Opu�ci�e� teren bazy" NL);
		ch->desc->olc_editing	= NULL;
		ch->desc->connected		= CON_PLAYING;
		return;
	}
}

DEF_DO_FUN( build_hq )
{

	if ( IS_NPC( ch ) )
	{
		huh(ch);
		return;
	}

	if ( hq_build_check_permision(ch,CLAN_HQ_ENTRANCE) == false)
	{
		ch_printf(ch, "Nie masz uprawnie� do budowania" NL);
		return;
	}

	//jesli jestesmy juz w pomieszczeniu klanowym
	if (hq_build_is_clan_room(ch,false) ==false)
	{
		if (hq_build_dir_free(ch, argument) == false)
		{
			ch_printf(ch, "W tym kierunku istnieje juz przej�cie. Musisz wybra� inny" NL);
			return;
		}
		if (hq_build_check_cash(ch) == false)
		{
			ch_printf(ch, "Nie masz do�� got�wki, by rozpocz�� budow�." NL);
			return;
		}
		if (hq_build_planet_free(ch) == false)
		{
			ch_printf(ch, "Na tej planecie jest ju� zbudowana kwatera g��wna innego klanu." NL);
			return;
		}
		hq_build_entrance(ch, argument);
	}
	else
	{

	}
	ch->pcdata->line_nr		= 0;
	ch->desc->connected		= CON_BUILD_HQ;
}

/**
	* @brief Check if exist area for this clan HQ
	*
	* @param ch
 */
void hq_build_area(CHAR_DATA *ch)
{
	char buf[MSL];
	AREA_DATA * pArea = NULL;

	if (!ch->pcdata->clan)
		return;
	sprintf(buf,"Kwatera %s",ch->pcdata->clan->name);

	pArea = nullptr;
	for (auto* a : area_list)
	{
		if (!str_cmp( buf, a->name ) )
		{
			pArea = a;
			break;
		}
	}

	if (!pArea)
	{
		//trzeba zamieni� spacje w pliku
		pArea = new_area();
		pArea->lvnum = 1;
		STRDUP( pArea->name,buf);
		STRDUP(pArea->filename,ch->pcdata->clan->filename);
		SET_BIT(pArea->flags,AFLAG_HIDDEN);
		SET_BIT(pArea->flags,AFLAG_DONTSHOWPLANET);
		SET_BIT(pArea->flags,AFLAG_NOHAIL);
		area_list.push_back(pArea);
//		fold_area(pArea,pArea->filename,false);
		save_area(pArea);
		write_area_list();
	}
	ch->pcdata->area = pArea;
}
