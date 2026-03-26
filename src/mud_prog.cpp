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
 *  The MUDprograms are heavily based on the original MOBprogram code that  *
 *  was written by N'Atas-ha.						   *
 *  Much has been added, including the capability to put a "program" on     *
 *  rooms and objects, not to mention many more triggers and ifchecks, as   *
 *  well as "script" support.						   *
 *  Error reporting has been changed to specify whether the offending       *
 *  program is on a mob, a room or and object, along with the vnum.         *
 *  Mudprog parsing has been rewritten (in mprog_driver). Mprog_process_if  *
 *  and mprog_process_cmnd have been removed, mprog_do_command is new.      *
 *  Full support for nested ifs is in.                                      *
 * ------------------------------------------------------------------------ *
 *                     ____________   __     ______                         *
 *   Aldegard    Jot  (   ___   ___| /  \   |  __  )   Thanos      Trog     *
 *        ______________\  \|   |  /  ^   \ |     <_______________          *
 *        \________________/|___|/___/"\___\|__|\________________/          *
 *                  \   \/ \/   //  \   |  __  )(  ___/`                    *
 *                    \       //  ^   \ |     <__)  \                       *
 *                      \_!_//___/"\___\|__|\______/TM                      *
 * (c) 2001, 2002            M       U        D                Ver 1.1      *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include "mud.h"

/* Defines by Narn for new mudprog parsing, used as
 return values from mprog_do_command. */
#define COMMANDOK    1
#define IFtrue       2
#define IFfalse      3
#define ORtrue       4
#define ORfalse      5
#define FOUNDELSE    6
#define FOUNDENDIF   7
#define IFIGNORED    8
#define ORIGNORED    9

/* Ifstate defines, used to create and access ifstate array
 in mprog_driver. */
#define MAX_IFS     10		/* should always be generous */
#define IN_IF        0
#define IN_ELSE      1
#define DO_IF        2
#define DO_ELSE      3

#define MAX_PROG_NEST   20

int mprog_do_command args( ( char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor,
				OBJ_DATA *obj, void *vo, CHAR_DATA *rndm,
				bool ignore, bool ignore_ors ) );
void runscript args( ( char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor,
				OBJ_DATA * obj, void *vo, CHAR_DATA * rndm ) );
void stopscript args( ( SCRIPT_DATA *script ) );
void update_script args( ( SCRIPT_DATA *script ) );

/*
 *  Mudprogram additions
 */
CHAR_DATA *supermob;
SCRIPT_DATA *curr_script;
std::list<act_prog_data*> room_act_list;
std::list<act_prog_data*> obj_act_list;
std::list<act_prog_data*> mob_act_list;
/*
 * Local function prototypes
 */

char* mprog_next_command args( ( char* clist ) );
bool mprog_seval args( ( char* lhs, char* opr, char* rhs,
				CHAR_DATA *mob ) );
bool mprog_veval args( ( int lhs, char* opr, int rhs,
				CHAR_DATA *mob ) );
int mprog_do_ifcheck args( ( char* ifcheck, CHAR_DATA* mob,
				CHAR_DATA* actor, OBJ_DATA* obj,
				void* vo, CHAR_DATA* rndm ) );
int mprog_translate args( ( char* var, char* t, CHAR_DATA* mob,
				CHAR_DATA* actor, OBJ_DATA* obj,
				void* vo, CHAR_DATA* rndm ) );
void mprog_driver args( ( char* com_list, CHAR_DATA* mob,
				CHAR_DATA* actor, OBJ_DATA* obj,
				void* vo, bool single_step ) );

bool mprog_keyword_check args( ( const char *argu, const char *argl ) );
void obj_countvnum args( ( CHAR_DATA *ch, OBJ_DATA *obj, int vnum, int *ammount ) );
void obj_countvname args( ( CHAR_DATA *ch, OBJ_DATA *obj, char *name, int *ammount ) );

void oprog_wordlist_check(char *arg, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type, OBJ_DATA *iobj);
void set_supermob(OBJ_DATA *obj);
bool oprog_percent_check(CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type);
void rprog_percent_check(CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type);
void rprog_wordlist_check(char *arg, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type, ROOM_INDEX_DATA *room);
/* Ratm - zmienne */
char* get_var_value(char *var_name, CHAR_DATA *mob);
int is_check_fun(char *point, CHAR_DATA **chkchar, OBJ_DATA **chkobj, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo,
		CHAR_DATA *rndm);

/***************************************************************************
 * Local function code and brief comments.
 */

/* if you dont have these functions, you damn well should... */

#ifdef DUNNO_STRSTR
char * strstr(s1,s2) const char *s1; const char *s2;
{
  char *cp;
  int i,j=strlen(s1)-strlen(s2),k=strlen(s2);
  if(j<0)
    return NULL;
  for(i=0; i<=j && strncmp(s1++,s2, k)!=0; i++);
  return (i>j) ? NULL : (s1-1);
}
#endif

void init_supermob()
{
	CREATE(supermob, CHAR_DATA, 1);
	clear_char(supermob);
	supermob->sex = SEX_MALE;
	supermob->position = POS_STANDING;
	supermob->act = ACT_IS_NPC | ACT_SECRETIVE;
	STRDUP(supermob->name, "supermob");
	STRDUP(supermob->description, "");
	STRDUP(supermob->przypadki[0], "");
	STRDUP(supermob->przypadki[1], "");
	STRDUP(supermob->przypadki[2], "");
	STRDUP(supermob->przypadki[3], "");
	STRDUP(supermob->przypadki[4], "");
	STRDUP(supermob->przypadki[5], "");
	STRDUP(supermob->long_descr, "");
	CREATE(supermob->pIndexData, MOB_INDEX_DATA, 1);
}

/* Used to get sequential lines of a multi line string (separated by "\n\r")
 * Thus its like one_argument(), but a trifle different. It is destructive
 * to the multi line string argument, and thus clist must not be shared.
 */
char* mprog_next_command(char *clist)
{

	char *pointer = clist;

	while (*pointer != '\n' && *pointer != '\0')
		pointer++;
	if (*pointer == '\n')
		*pointer++ = '\0';
	if (*pointer == '\r')
		*pointer++ = '\0';

	return (pointer);

}

/* These two functions do the basic evaluation of ifcheck operators.
 *  It is important to note that the string operations are not what
 *  you probably expect.  Equality is exact and division is substring.
 *  remember that lhs has been stripped of leading space, but can
 *  still have trailing spaces so be careful when editing since:
 *  "guard" and "guard " are not equal.
 */
bool mprog_seval(char *lhs, char *opr, char *rhs, CHAR_DATA *mob)
{

	if (!str_cmp(opr, "=="))
		return (bool) (!str_cmp(lhs, rhs));
	if (!str_cmp(opr, "!="))
		return (bool) (str_cmp(lhs, rhs));
	if (!str_cmp(opr, "/"))
		return (bool) (!str_infix(rhs, lhs));
	if (!str_cmp(opr, "!/"))
		return (bool) (!str_infix(rhs, lhs));

	sprintf(log_buf, "Improper MOBprog operator '%s'", opr);
	progbug(log_buf, mob);
	return 0;

}

bool mprog_veval(int lhs, char *opr, int rhs, CHAR_DATA *mob)
{

	if (!str_cmp(opr, "=="))
		return (lhs == rhs);
	if (!str_cmp(opr, "!="))
		return (lhs != rhs);
	if (!str_cmp(opr, ">"))
		return (lhs > rhs);
	if (!str_cmp(opr, "<"))
		return (lhs < rhs);
	if (!str_cmp(opr, "<="))
		return (lhs <= rhs);
	if (!str_cmp(opr, ">="))
		return (lhs >= rhs);
	if (!str_cmp(opr, "&"))
		return (lhs & rhs);
	if (!str_cmp(opr, "|"))
		return (lhs | rhs);

	sprintf(log_buf, "Improper MOBprog operator '%s'", opr);
	progbug(log_buf, mob);

	return 0;

}

/*
 * Trog
 * Dzieki tym funkcjom mozna policzyc nawet ilosc wlosow na jajach.
 */
void obj_countname(CHAR_DATA *ch, const std::list<OBJ_DATA*>& objlist, char *name, int *ammount)
{
	for (auto* obj : objlist)
	{
		if (obj->item_type == ITEM_CONTAINER)
			obj_countname(ch, obj->contents, name, ammount);
		if (can_see_obj(ch, obj) && !str_cmp(obj->name, name))
			*ammount += obj->count;
	}
}

/* Jakby te typy  byly gdzies indziej jeszcze potrzebne to wywalic do mud.h */
typedef enum
{
	COUNT_VNUM, COUNT_NAME, COUNT_TYPE
} count_by;
typedef enum
{
	CNT_NONE, CNT_INV, CNT_EQ
} count_locations;

void obj_count(CHAR_DATA *ch, const std::list<OBJ_DATA*>& objlist, count_locations loc, count_by type, void *data, int *ammount)
{
	for (auto* obj : objlist)
	{
		if (obj->item_type == ITEM_CONTAINER)
			obj_count(ch, obj->contents, loc, type, data, ammount);
		if (can_see_obj(ch, obj))
			switch (type)
			{
			case COUNT_VNUM:
				if (obj->pIndexData->vnum == (intptr_t) data
						&& ((loc == CNT_INV && obj->wear_loc == WEAR_NONE) || (loc == CNT_EQ && obj->wear_loc != WEAR_NONE)
								|| loc == CNT_NONE))
					*ammount += obj->count;
				break;
			case COUNT_TYPE:
				if (obj->item_type == (intptr_t) data
						&& ((loc == CNT_INV && obj->wear_loc == WEAR_NONE) || (loc == CNT_EQ && obj->wear_loc != WEAR_NONE)
								|| loc == CNT_NONE))
					*ammount += obj->count;
				break;
			case COUNT_NAME:
				if (!str_cmp(obj->name, (char*) data)
						&& ((loc == CNT_INV && obj->wear_loc == WEAR_NONE) || (loc == CNT_EQ && obj->wear_loc != WEAR_NONE)
								|| loc == CNT_NONE))
					*ammount += obj->count;
				break;
			}
	}
}

void obj_countall(CHAR_DATA *ch, count_locations loc, count_by type, void *data, int *ammount)
{
	for (auto* pCh : ch->in_room->people)
		obj_count(pCh, pCh->carrying, loc, type, data, ammount);
	obj_count(ch, ch->in_room->contents, loc, type, data, ammount);
}

/* This function performs the evaluation of the if checks.  It is
 * here that you can add any ifchecks which you so desire. Hopefully
 * it is clear from what follows how one would go about adding your
 * own. The syntax for an if check is: ifcheck ( arg ) [opr val]
 * where the parenthesis are required and the opr and val fields are
 * optional but if one is there then both must be. The spaces are all
 * optional. The evaluation of the opr expressions is farmed out
 * to reduce the redundancy of the mammoth if statement list.
 * If there are errors, then return BERR otherwise return boolean 1,0
 * Redone by Altrag.. kill all that big copy-code that performs the
 * same action on each variable..
 */
int mprog_do_ifcheck(char *ifcheck, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm)
{
	char cvar[MIL] =
	{ 0 };
	char chck[MIL] =
	{ 0 };
	char opr[MIL] =
	{ 0 };
	char rval[MIL] =
	{ 0 };
	char buf[MIL] =
	{ 0 };
	char *point = ifcheck;
	char *pchck = chck;
	CHAR_DATA *chkchar = NULL;
	OBJ_DATA *chkobj = NULL;
	int lhsvl, rhsvl;
	int neg_value = false;	// Ratm - negacja wartosci ifchecka
	/* Ratm - makro liczace exclusive or uzyte do wprowadzenia operatora negacji
	 w ifcheckach */
#define XOR(nval, rval)( (nval) ? !(rval) : (rval) )

	if (!*point)
	{
		progbug("Null ifcheck", mob);
		return BERR;
	}
	while (*point == ' ')
		point++;
	if (*point == '!')	// Ratm - negacja wartosci ifchecka
	{
		point++;
		neg_value = true;
	}
	while (*point != '(')
		if (*point == '\0')
		{
			progbug("Ifcheck syntax error", mob);
			return BERR;
		}
		else if (*point == ' ')
			point++;
		else
			*pchck++ = *point++;
	*pchck = '\0';
	point++;
	pchck = cvar;
	while (*point != ')')
		if (*point == '\0')
		{
			progbug("Ifcheck syntax error", mob);
			return BERR;
		}
		else if (*point == ' ')
			point++;
		else
			*pchck++ = *point++;
	point++;
	*pchck = '\0';

	while (*point == ' ')
		point++;
	if (!*point)
	{
		opr[0] = '\0';
		rval[0] = '\0';
	}
	else
	{
		pchck = opr;
		while (*point != ' ' && !isalnum(*point))
			if (*point == '\0')
			{
				progbug("Ifcheck operator without value", mob);
				return BERR;
			}
			else
				*pchck++ = *point++;
		*pchck = '\0';

		while (*point == ' ')
			point++;
		pchck = buf;
		while (*point != '\0' && *point != '\0')
			*pchck++ = *point++;
		*pchck = '\0';

		/* Ratm rozwijanie zmiennych po prawej stronie operatora
		 This chunk of code taken from mprog_process_cmnd. */
		pchck = buf;
		point = rval;
		while (*pchck != '\0')
		{
			char tmp[MAX_INPUT_LENGTH];
			char *i;

			if (*pchck != '$')
			{
				*point++ = *pchck++;
				continue;
			}
			pchck++;
			pchck += mprog_translate(pchck, tmp, mob, actor, obj, vo, rndm);
			i = tmp;
			while ((*point = *i) != '\0')
				++point, ++i;
		}
		*point = '\0';
	}

	/* chck contains check, cvar is the variable in the (), opr is the
	 * operator if there is one, and rval is the value if there was an
	 * operator.
	 */
	if (cvar[0] == '$')
	{
		switch (cvar[1])
		{
		case 'i':
			chkchar = mob;
			break;
		case 'n':
			chkchar = actor;
			break;
		case 't':
			chkchar = (CHAR_DATA*) vo;
			break;
		case 'r':
			chkchar = rndm;
			break;
		case 'o':
			chkobj = obj;
			break;
		case 'p':
			chkobj = (OBJ_DATA*) vo;
			break;
		case 'v': /* Ratm - var, do nothing */
			break;
		default:
			swsnprintf(rval, MIL, "Bad argument '$%c' to '%s'", cvar[1], chck);
			progbug(rval, mob);
			return BERR;
		}
		if (!chkchar && !chkobj && (cvar[1] != 'v'))
			return BERR;
	}

	if (!str_cmp(chck, "if"))
		(void) mprog_do_command(ifcheck, mob, actor, obj, vo, rndm, false, false);

	if (!str_cmp(chck, "rand"))
	{
		return XOR(neg_value, number_percent() <= atoi(cvar));
	}

	if (!str_cmp(chck, "economy"))
	{
		int idx = atoi(cvar);
		ROOM_INDEX_DATA *room;

		if (!idx)
		{
			if (!mob->in_room)
			{
				progbug("'economy' ifcheck: mob in NULL room with no room vnum "
						"argument", mob);
				return BERR;
			}
			room = mob->in_room;
		}
		else
			room = get_room_index(idx);
		if (!room)
		{
			progbug("Bad room vnum passed to 'economy'", mob);
			return BERR;
		}
		return XOR(neg_value,
				mprog_veval(((room->area->high_economy > 0) ? 1000000000 : 0) + room->area->low_economy, opr, atoi(rval), mob));
	}

	if (!str_cmp(chck, "mobinroom"))
	{
		int vnum = atoi(cvar);
		int lhsvl;

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("Bad vnum to 'mobinroom'", mob);
			return BERR;
		}
		lhsvl = 0;
		for (auto* oMob : mob->in_room->people)
			if ( IS_NPC(oMob) && oMob->pIndexData->vnum == vnum)
				lhsvl++;
		rhsvl = atoi(rval);
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "timeskilled"))
	{
		MOB_INDEX_DATA *pMob;

		if (chkchar)
		{
			if (IS_NPC(chkchar))
				pMob = chkchar->pIndexData;
			else
			{
				progbug("TimesKilled ifcheck: victim must be mob not player", mob);
				return BERR;
			}
		}
		else if (!(pMob = get_mob_index(atoi(cvar))))
		{
			progbug("TimesKilled ifcheck: bad vnum", mob);
			return BERR;
		}
		return XOR(neg_value, mprog_veval(pMob->killed, opr, atoi(rval), mob));
	}

	if (!str_cmp(chck, "ovnumhere"))
	{
		int vnum = atoi(cvar);

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("OvnumHere: bad vnum", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_countall(mob, CNT_NONE, COUNT_VNUM, reinterpret_cast<void*>(vnum), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "onamehere"))
	{
		if (*cvar == '\0')
		{
			progbug("OnameHere: bad (or no) name", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_countall(mob, CNT_NONE, COUNT_NAME, (void*) cvar, &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "otypehere"))
	{
		int type;

		if ( is_number(cvar))
			type = atoi(cvar);
		else
			type = (int) flag_value(obj_types_list, cvar);
		if (type < 0 || type > MAX_ITEM_TYPE)
		{
			progbug("OtypeHere: bad type", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_countall(mob, CNT_NONE, COUNT_TYPE, reinterpret_cast<void*>(type), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "ovnumroom"))
	{
		int vnum = atoi(cvar);

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("OvnumRoom: bad vnum", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->in_room->contents, CNT_NONE, COUNT_VNUM, reinterpret_cast<void*>(vnum), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "onameroom"))
	{
		if (*cvar == '\0')
		{
			progbug("OnameRoom: bad (or no) name", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->in_room->contents, CNT_NONE, COUNT_NAME, (void*) cvar, &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "otyperoom"))
	{
		int type;

		if ( is_number(cvar))
			type = atoi(cvar);
		else
			type = (int) flag_value(obj_types_list, cvar);
		if (type < 0 || type > MAX_ITEM_TYPE)
		{
			progbug("OtypeRoom: bad type", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->in_room->contents, CNT_NONE, COUNT_TYPE, reinterpret_cast<void*>(type), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "ovnumcarry"))
	{
		int vnum = atoi(cvar);

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("OvnumCarry: bad vnum", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_NONE, COUNT_VNUM, reinterpret_cast<void*>(vnum), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "onamecarry"))
	{

		if (*cvar == '\0')
		{
			progbug("OnameCarry: bad (or no) name", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_NONE, COUNT_NAME, reinterpret_cast<void*>(cvar), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "otypecarry"))
	{
		int type;

		if ( is_number(cvar))
			type = atoi(cvar);
		else
			type = (int) flag_value(obj_types_list, cvar);

		if (type < 0 || type > MAX_ITEM_TYPE)
		{
			progbug("OtypeCarry: bad type", mob);
			bug("%d %s", type, cvar);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_NONE, COUNT_TYPE, reinterpret_cast<void*>(type), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "ovnumwear"))
	{
		int vnum = atoi(cvar);

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("OvnumWear: bad vnum", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_EQ, COUNT_VNUM, reinterpret_cast<void*>(vnum), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "onamewear"))
	{
		if (*cvar == '\0')
		{
			progbug("OnameWear: bad (or no) name", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_EQ, COUNT_NAME, reinterpret_cast<void*>(cvar), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "otypewear"))
	{
		int type;

		if ( is_number(cvar))
			type = atoi(cvar);
		else
			type = (int) flag_value(obj_types_list, cvar);
		if (type < 0 || type > MAX_ITEM_TYPE)
		{
			progbug("OtypeWear: bad type", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_EQ, COUNT_TYPE, reinterpret_cast<void*>(type), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "ovnuminv"))
	{
		int vnum = atoi(cvar);

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("OvnumInv: bad vnum", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_INV, COUNT_VNUM, reinterpret_cast<void*>(vnum), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "onameinv"))
	{

		if (*cvar == '\0')
		{
			progbug("OnameInv: bad (or no) name", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_INV, COUNT_NAME, (void*) cvar, &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "otypeinv"))
	{
		int type;

		if ( is_number(cvar))
			type = atoi(cvar);
		else
			type =
			//get_otype(cvar);
					(int) flag_value(obj_types_list, cvar);
		if (type < 0 || type > MAX_ITEM_TYPE)
		{
			progbug("OtypeInv: bad type", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, mob->carrying, CNT_INV, COUNT_TYPE, reinterpret_cast<void*>(type), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}
	/* Thanos */

	if (!str_cmp(chck, "weapontypeinv"))
	{
		int type;

		if ( is_number(rval))
			type = atoi(rval);
		else
			type = (int) flag_value(weapon_types_list, rval);

		if (type < 0)
		{
			progbug("WeaponTypeInv: bad weapon type", mob);
			return BERR;
		}

		lhsvl = 0;
		for (auto* pObj : chkchar->carrying)
			if (can_see_obj(mob, pObj) && pObj->wear_loc == WEAR_NONE && pObj->item_type == ITEM_WEAPON && pObj->value[3] == type)
				lhsvl = pObj->count;
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}
	if (!str_cmp(chck, "weapontypewear"))
	{
		int type;

		if ( is_number(rval))
			type = atoi(rval);
		else
			type = (int) flag_value(weapon_types_list, rval);

		if (type < 0)
		{
			progbug("WeaponTypeWear: bad weapon type", mob);
			return BERR;
		}

		lhsvl = 0;
		for (auto* pObj : chkchar->carrying)
			if (can_see_obj(mob, pObj) && pObj->wear_loc != WEAR_NONE && pObj->item_type == ITEM_WEAPON && pObj->value[3] == type)
				lhsvl = pObj->count;
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}
	if (!str_cmp(chck, "weapontypecarry"))
	{
		int type;

		if ( is_number(rval))
			type = atoi(rval);
		else
			type = (int) flag_value(weapon_types_list, rval);

		if (type < 0)
		{
			progbug("WeaponTypeCarry: bad weapon type", mob);
			return BERR;
		}

		lhsvl = 0;
		for (auto* pObj : chkchar->carrying)
			if (can_see_obj(mob, pObj) && pObj->item_type == ITEM_WEAPON && pObj->value[3] == type)
				lhsvl = pObj->count;
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}
	/*Trog & Thanos*/
	if (!str_cmp(chck, "questdone"))
	{
		if (!chkchar)
			return XOR(neg_value, false);

		if (IS_NPC(chkchar))
			return XOR(neg_value, false);

		return XOR(neg_value, mprog_veval(true, opr, is_name(rval, chkchar->pcdata->quest_done), mob));
	}
	if (!str_cmp(chck, "cansee"))
	{
		return XOR(neg_value, can_see(mob, chkchar));
	}

	/* Thanos */
	if (!str_cmp(chck, "iswanted"))
	{
		PLANET_DATA *planet = get_planet(rval);
		bool op;

		if (IS_NPC(chkchar))
			return XOR(neg_value, false);

		if (!str_cmp(opr, "=="))
			op = true;
		else if (!str_cmp(opr, "!="))
			op = false;
		else
		{
			bug("Invaild operator");
			return BERR;
		}

		if (planet && find_crime(actor, planet))
			return XOR(neg_value, op);

		return XOR(neg_value, !op);
	}

	if (!str_cmp(chck, "ovnuminvchar"))
	{
		int vnum = atoi(cvar);

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("OvnumInvChar: bad vnum", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_INV, COUNT_VNUM, reinterpret_cast<void*>(vnum), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "onameinvchar"))
	{
		if (*cvar == '\0')
		{
			progbug("OnameInvChar: bad (or no) name", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_INV, COUNT_NAME, (void*) cvar, &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "otypeinvchar"))
	{
		int type;

		if ( is_number(rval))
			type = atoi(rval);
		else
			type = (int) flag_value(obj_types_list, rval);

		if (type < 0)
		{
			progbug("OTypeInvChar: bad type", mob);
			return BERR;
		}

		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_INV, COUNT_TYPE, reinterpret_cast<void*>(type), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));

	}
	if (!str_cmp(chck, "ovnumwearchar"))
	{
		int vnum = atoi(cvar);

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("OvnumWearChar: bad vnum", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_EQ, COUNT_VNUM, reinterpret_cast<void*>(vnum), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "onamewearchar"))
	{

		if (*cvar == '\0')
		{
			progbug("OnameWearChar: bad (or no) name", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_EQ, COUNT_NAME, (void*) cvar, &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "otypewearchar"))
	{
		int type;

		if ( is_number(rval))
			type = atoi(rval);
		else
			type = (int) flag_value(obj_types_list, rval);

		if (type < 0)
		{
			progbug("OTypeWearChar: bad type", mob);
			return BERR;
		}

		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_EQ, COUNT_TYPE, reinterpret_cast<void*>(type), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "otypecarrychar"))
	{
		int type;

		if ( is_number(rval))
			type = atoi(rval);
		else
			type = (int) flag_value(obj_types_list, rval);

		if (type < 0)
		{
			progbug("OTypeCarryChar bad type", mob);
			return BERR;
		}

		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_NONE, COUNT_TYPE, reinterpret_cast<void*>(type), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "ovnumcarrychar"))
	{
		int vnum = atoi(cvar);

		if (vnum < 1 || vnum > MAX_VNUM)
		{
			progbug("OvnumCarryChar bad vnum", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_NONE, COUNT_VNUM, reinterpret_cast<void*>(vnum), &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (!str_cmp(chck, "onamecarrychar"))
	{
		if (*cvar == '\0')
		{
			progbug("OnameCarryChar bad (or no) name", mob);
			return BERR;
		}
		lhsvl = 0;
		obj_count(mob, actor->carrying, CNT_NONE, COUNT_NAME, (void*) cvar, &lhsvl);
		rhsvl = is_number(rval) ? atoi(rval) : -1;
		if (rhsvl < 1)
			rhsvl = 1;
		if (!*opr)
			strcpy(opr, "==");
		return XOR(neg_value, mprog_veval(lhsvl, opr, rhsvl, mob));
	}

	if (chkchar)
	{
		if (!str_cmp(chck, "ismobinvis"))
		{
			return XOR(neg_value, (IS_NPC(chkchar) && IS_SET(chkchar->act, ACT_MOBINVIS)));
		}
		if (!str_cmp(chck, "mobinvislevel"))
		{
			return XOR(neg_value, (IS_NPC(chkchar) ? mprog_veval(chkchar->mobinvis, opr, atoi(rval), mob) : false));
		}
		if (!str_cmp(chck, "ispc"))
		{
			return XOR(neg_value, IS_NPC(chkchar) ? false : true);
		}
		if (!str_cmp(chck, "isstunned"))
		{
			return XOR(neg_value, (chkchar->position == POS_STUNNED) ? true : false);
		}
		if (!str_cmp(chck, "isnpc"))
		{
			return XOR(neg_value, IS_NPC(chkchar) ? true : false);
		}
		if (!str_cmp(chck, "ismounted"))
		{
			return XOR(neg_value, (chkchar->position == POS_MOUNTED));
		}
		if (!str_cmp(chck, "isgood"))
		{
			return XOR(neg_value, IS_GOOD(chkchar) ? true : false);
		}
		if (!str_cmp(chck, "isneutral"))
		{
			return XOR(neg_value, IS_NEUTRAL(chkchar) ? true : false);
		}
		if (!str_cmp(chck, "isevil"))
		{
			return XOR(neg_value, IS_EVIL(chkchar) ? true : false);
		}
		if (!str_cmp(chck, "isfight"))
		{
			return XOR(neg_value, who_fighting(chkchar) ? true : false);
		}
		if (!str_cmp(chck, "isimmort"))
		{
			return XOR(neg_value, (get_trust(chkchar) >= LEVEL_IMMORTAL));
		}
		if (!str_cmp(chck, "ischarmed"))
		{
			return XOR(neg_value, IS_AFFECTED(chkchar, AFF_CHARM) ? true : false);
		}
		if (!str_cmp(chck, "isinnameslist"))
		{
			return XOR(neg_value, isinnameslist(mob, chkchar->name));
		}
		if (!str_cmp(chck, "nameslistfull"))
		{
			return XOR(neg_value, nameslistfull(mob));
		}
		if (!str_cmp(chck, "nameslistempty"))
		{
			return XOR(neg_value, nameslistempty(mob));
		}
		if (!str_cmp(chck, "isfollow"))
		{
			return XOR(neg_value, (chkchar->master != NULL && chkchar->master->in_room == chkchar->in_room));
		}
		if (!str_cmp(chck, "isaffected"))
		{
			int value = (int) flag_value(aff_flags_list, rval);
			if (value < 0)
			{
				progbug("Unknown affect being checked", mob);
				return BERR;
			}
			return XOR(neg_value, IS_AFFECTED(chkchar, value) ? true : false);
		}
		if (!str_cmp(chck, "hitprcnt"))
		{
			return XOR(neg_value, mprog_veval(chkchar->hit / chkchar->max_hit, opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "inroom"))
		{
			if (chkchar->in_room)
				return XOR(neg_value, mprog_veval(chkchar->in_room->vnum, opr, atoi(rval), mob));
			else
			{
				progbug("No chkchar->in_room", mob);
				return false;
			}
		}
		if (!str_cmp(chck, "wasinroom"))
		{
			if (chkchar->was_in_room)
				return XOR(neg_value, mprog_veval(chkchar->was_in_room->vnum, opr, atoi(rval), mob));
			else
			{
				progbug("No chkchar->in_room", mob);
				return false;
			}
		}
		/* Trog: do 2 powyzszych:
		 * w przypadku, gdy moba ladujemy przedmiotem, ktory ma (exa_)prog taki:
		 * mpecho $n Wykonuje dziwne czynnosci przy figurce.
		 * mpoload <vnum_moba>
		 * a mob ma act_prog z imieniem gracza, ktory laduje przedmiot:
		 * if inroom($n)
		 *  say ble
		 * endif
		 * to mud pada i musialem zrobic takie ify w 2 powyzszych ifcheckach
		 * Jest tak, ze chkchar to supermob dlatego, ze to przedmiot (czyli
		 * supermob) wykoduje program moba a gracz wykonuje program obiektu.
		 * Co jest zle to wiem bo zrobilem ifchecki, ale jak to poprawic to
		 * juz nie wiem i mam nadzieje, ze Ty wiesz... Mam takie progi u siebie
		 * na kompie, ale pada odkryl Dungal.
		 */
		if (!str_cmp(chck, "sex"))
		{
			return XOR(neg_value, mprog_veval(chkchar->sex, opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "position"))
		{
			return XOR(neg_value, mprog_veval(chkchar->position, opr, atoi(rval), mob));
		}
		/*Thanos, Trog: lekkie zmiany i dodatki*/
		if (!str_cmp(chck, "questid"))
		{
			return XOR(neg_value,
					(!actor || IS_NPC(actor) || !actor->inquest ) ? false : mprog_veval(chkchar->inquest->pIndexData->quest_id, opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "questp"))
		{
			return XOR(neg_value, (IS_NPC(actor) ? false : actor->inquest ? true : false));
		}
		if (!str_cmp(chck, "questpoints"))
		{
			return XOR(neg_value, (IS_NPC(actor) ? false : mprog_veval(chkchar->pcdata->quest_points, opr, atoi(rval), mob)));
		}
		if (!str_cmp(chck, "isauthed"))
		{
			return XOR(neg_value, NOT_AUTHED(chkchar) ? false : true);
		}
		if (!str_cmp(chck, "ishelled"))
		{
			return XOR(neg_value, IS_NPC(actor) ? false : mprog_veval(chkchar->pcdata->release_date, opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "level"))
		{
			return XOR(neg_value, mprog_veval(get_trust(chkchar), opr, atoi(rval), mob));
		}
		/*Trog: dodalem credits*/
		if (!str_cmp(chck, "goldamt") || !str_cmp(chck, "credits"))
		{
			return XOR(neg_value, mprog_veval(chkchar->gold, opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "race"))
		{
			return XOR(neg_value, mprog_seval((char* )chkchar->race->name, opr, rval, mob));
		}
		if (!str_cmp(chck, "class"))
		{
			return XOR(neg_value, mprog_veval(chkchar->main_ability, opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "clan"))
		{
			if ( IS_NPC(chkchar) || !chkchar->pcdata->clan)
				return XOR(neg_value, false);
			return XOR(neg_value, mprog_seval(chkchar->pcdata->clan->name, opr, rval, mob));
		}
		if (!str_cmp(chck, "iscouncil") /*||  !str_cmp(chck, "senator")*/)
		{
			/* Kiedys to zakoduje to SENATE bo mi sie podoba, tzn skoncze. Trog
			 SENATE_DATA *senator;

			 if ( IS_NPC(chkchar) )
			 return false;
			 if ( IS_IMMORTAL(chkchar) )
			 return true;
			 for ( senator = first_senator; senator; senator = senator->next )
			 if ( !str_cmp( chkchar->name, senator->name ) )
			 return true;
			 */
			if (IS_NPC(chkchar))
				return XOR(neg_value, false);

			if (IS_COUNCIL(chkchar->name))
				return XOR(neg_value, true);

			return XOR(neg_value, false);
		}
		if (!str_cmp(chck, "clantype"))
		{
			if ( IS_NPC(chkchar) || !chkchar->pcdata->clan)
				return XOR(neg_value, false);
			return XOR(neg_value, mprog_veval(chkchar->pcdata->clan->type, opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "str"))
		{
			return XOR(neg_value, mprog_veval(get_curr_str(chkchar), opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "wis"))
		{
			return XOR(neg_value, mprog_veval(get_curr_wis(chkchar), opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "int"))
		{
			return XOR(neg_value, mprog_veval(get_curr_int(chkchar), opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "dex"))
		{
			return XOR(neg_value, mprog_veval(get_curr_dex(chkchar), opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "con"))
		{
			return XOR(neg_value, mprog_veval(get_curr_con(chkchar), opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "cha"))
		{
			return XOR(neg_value, mprog_veval(get_curr_cha(chkchar), opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "frc"))
		{
			return XOR(neg_value, mprog_veval(get_curr_frc(chkchar), opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "lck"))
		{
			return XOR(neg_value, mprog_veval(get_curr_lck(chkchar), opr, atoi(rval), mob));
		}
	}
	if (chkobj)
	{
		if (!str_cmp(chck, "objtype"))
		{
			return XOR(neg_value, mprog_veval(chkobj->item_type, opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "objval0"))
		{
			return XOR(neg_value, mprog_veval(chkobj->value[0], opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "objval1"))
		{
			return XOR(neg_value, mprog_veval(chkobj->value[1], opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "objval2"))
		{
			return XOR(neg_value, mprog_veval(chkobj->value[2], opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "objval3"))
		{
			return XOR(neg_value, mprog_veval(chkobj->value[3], opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "objval4"))
		{
			return XOR(neg_value, mprog_veval(chkobj->value[4], opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "objval5"))
		{
			return XOR(neg_value, mprog_veval(chkobj->value[5], opr, atoi(rval), mob));
		}
		if (!str_cmp(chck, "objiswear"))
		{
			return XOR(neg_value, mprog_veval(chkobj->wear_loc, opr, (int )flag_value(wear_types_list, rval), mob));
		}
	}
	/* The following checks depend on the fact that cval[1] can only contain
	 one character, and that NULL checks were made previously. */
	if (!str_cmp(chck, "number"))
	{
		if (chkchar)
		{
			if (!IS_NPC(chkchar))
				return false;
			lhsvl = (chkchar == mob) ? chkchar->gold : chkchar->pIndexData->vnum;
			return mprog_veval(lhsvl, opr, atoi(rval), mob);
		}
		return mprog_veval(chkobj->pIndexData->vnum, opr, atoi(rval), mob);
	}
	if (!str_cmp(chck, "name"))
	{
		if (chkchar)
			return XOR(neg_value, mprog_seval(chkchar->name, opr, rval, mob));
		return XOR(neg_value, mprog_seval(chkobj->name, opr, rval, mob));
	}

	/****************** Ratm - zmienne ******************/
	if (!str_cmp(chck, "var"))
	{
		char *v_val = get_var_value(cvar, mob);

		if (!v_val)
		{
			swsnprintf(buf, MIL, "var(%s) error: variable %s not initialized", cvar, cvar);
			progbug(buf, mob);
			return false;
		}

		if ( is_number(v_val))
			return XOR(neg_value, mprog_veval(atoi(v_val), opr, atoi(rval), mob));
		else
			return XOR(neg_value, mprog_seval(v_val, opr, rval, mob));
	}

	if (!str_cmp(chck, "isset"))
	{
		char *v_val = get_var_value(cvar, mob);

		if (!v_val)
			return XOR(neg_value, false);
		else
			return XOR(neg_value, true);
	}

	if (!str_cmp(chck, "isnumber"))
	{
		char *v_val = get_var_value(cvar, mob);

		if (!v_val)
		{
			swsnprintf(buf, MIL, "isnumber(%s) error: variable %s not initialized", cvar, cvar);
			progbug(buf, mob);
			return false;
		}
		else
		{
			point = cvar;
			if (strtol(v_val, &point, 10))
				;
			if (*point)
				return XOR(neg_value, false);

			return XOR(neg_value, true);
		}
	}
	/* Ok... all the ifchecks are done, so if we didnt find ours then something
	 * odd happened.  So report the bug and abort the MUDprogram (return error)
	 */
	progbug("Unknown ifcheck", mob);
	return BERR;

	/* Ratm To makro ju� nie b�dzie potrzebne. */
#undef XOR
}

/* Ratm Funkcja wykonuje podstawianie wartosci checka
 do zmiennej (�eby mog�y dzia�a� takie konstrukcje jak: $vl0 = name($n) )
 spora cz�� tej funkcji to copy/paste z mprog_do_ifcheck()
 */
int eval_check_func(char **var, char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm)
{
	char buf[MIL];
	char *point = cmnd;
	CHAR_DATA *chkchar = NULL;
	OBJ_DATA *chkobj = NULL;

	if (!*point)
	{
		progbug("Null eval value", mob);
		return BERR;
	}

	while (*point == ' ')
		point++;

	if (!str_prefix("name", point))
	{
		if (!is_check_fun(point + strlen("name"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		STRDUP(*var, chkchar->name);
		return COMMANDOK;
	}
	else if (!str_prefix("inroom", point))
	{
		if (!is_check_fun(point + strlen("inroom"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		if (chkchar->in_room)
		{
			sprintf(buf, "%i", chkchar->in_room->vnum);
			STRDUP(*var, buf);
		}
		else
			progbug("No chkchar->in_room", mob);

		return COMMANDOK;
	}
	else if (!str_prefix("wasinroom", point))
	{
		if (!is_check_fun(point + strlen("wasinroom"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		if (chkchar->was_in_room)
		{
			sprintf(buf, "%i", chkchar->was_in_room->vnum);
			STRDUP(*var, buf);
		}
		else
			progbug("No chkchar->was_in_room", mob);

		return COMMANDOK;
	}
	else if (!str_prefix("sex", point))
	{
		if (!is_check_fun(point + strlen("sex"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", chkchar->sex);
		STRDUP(*var, buf);
	}
	else if (!str_prefix("position", point))
	{
		if (!is_check_fun(point + strlen("position"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", chkchar->position);
		STRDUP(*var, buf);
	}
	else if (!str_prefix("level", point))
	{
		if (!is_check_fun(point + strlen("level"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_trust(chkchar));
		STRDUP(*var, buf);
	}
	else if (!str_prefix("credits", point))
	{
		if (!is_check_fun(point + strlen("credits"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", chkchar->gold);
		STRDUP(*var, buf);
	}
	else if (!str_prefix("race", point))
	{
		if (!is_check_fun(point + strlen("race"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		STRDUP(*var, (char* )chkchar->race->name);
	}
	else if (!str_prefix("clan", point))
	{
		if (!is_check_fun(point + strlen("clan"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		if ( IS_NPC(chkchar) || !chkchar->pcdata->clan)
			return BERR;

		STRDUP(*var, chkchar->pcdata->clan->name);
	}
	else if (!str_prefix("str", point))
	{
		if (!is_check_fun(point + strlen("str"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_curr_str(chkchar));
		STRDUP(*var, buf);
		return COMMANDOK;
	}
	else if (!str_prefix("wis", point))
	{
		if (!is_check_fun(point + strlen("wis"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_curr_wis(chkchar));
		STRDUP(*var, buf);
		return COMMANDOK;
	}
	else if (!str_prefix("int", point))
	{
		if (!is_check_fun(point + strlen("int"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_curr_int(chkchar));
		STRDUP(*var, buf);
		return COMMANDOK;
	}
	else if (!str_prefix("dex", point))
	{
		if (!is_check_fun(point + strlen("dex"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_curr_dex(chkchar));
		STRDUP(*var, buf);
		return COMMANDOK;
	}
	else if (!str_prefix("con", point))
	{
		if (!is_check_fun(point + strlen("con"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_curr_con(chkchar));
		STRDUP(*var, buf);
		return COMMANDOK;
	}
	else if (!str_prefix("cha", point))
	{
		if (!is_check_fun(point + strlen("cha"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_curr_cha(chkchar));
		STRDUP(*var, buf);
		return COMMANDOK;
	}
	else if (!str_prefix("frc", point))
	{
		if (!is_check_fun(point + strlen("frc"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_curr_frc(chkchar));
		STRDUP(*var, buf);
		return COMMANDOK;
	}
	else if (!str_prefix("lck", point))
	{
		if (!is_check_fun(point + strlen("lck"), &chkchar, &chkobj, mob, actor, obj, vo, rndm))
			return BERR;

		sprintf(buf, "%i", get_curr_lck(chkchar));
		STRDUP(*var, buf);
		return COMMANDOK;
	}
	else if (!str_prefix("rand", point))
	{
		char cvar[MAX_INPUT_LENGTH];
		char *pvar = cvar;
		char *coma = NULL;
		int from = 0, to = 0;

		point += strlen("rand");

		while (*point != '(')
			if (*point != ' ')
			{
				return false;
			}
		point++;
		while (*point != ')')
			if (*point == '\0')
			{
				return false;
			}
			else if (*point == ' ')
				point++;
			else if (*point == ',' || *point == '.')
			{
				point++;
				*pvar++ = '\0';
				coma = pvar;
			}
			else
				*pvar++ = *point++;
		point++;
		*pvar = '\0';
		pvar = cvar;

		if (coma)
		{
			from = atoi(pvar);
			pvar = coma;
		}

		to = atoi(pvar);

		if (from == to)
		{
			return BERR;
		}
		sprintf(buf, "%i", number_range(from, to));
		STRDUP(*var, buf);
		return COMMANDOK;
	}

	return BERR;
}

/* Ratm funkcja sprawdza czy to na co wskazuje point to na pewno check
 ( czy ma konstrukcje: chkname($x) ), a jesli tak to zaleznie od x podstawia
 odpowiednie warto�ci pod chkchar i chkobj.
 Tak�e tu spora cz�� tej funkcji to copy/paste z mprog_do_ifcheck() */
int is_check_fun(char *point, CHAR_DATA **chkchar, OBJ_DATA **chkobj, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo,
		CHAR_DATA *rndm)
{
	char buf[MIL] =
	{ 0 };
	char cvar[MIL] =
	{ 0 };
	char *pvar = cvar;

	while (*point != '(')
		if (*point != ' ')
		{
			return false;
		}
	point++;
	while (*point != ')')
		if (*point == '\0')
		{
			return false;
		}
		else if (*point == ' ')
			point++;
		else
			*pvar++ = *point++;
	point++;
	*pvar = '\0';

	while (*point == ' ')
		point++;
	if (*point)
	{
		return false;
	}

	/* cvar is the variable in the ()  */
	if (cvar[0] == '$')
	{
		switch (cvar[1])
		{
		case 'i':
			*chkchar = mob;
			break;
		case 'n':
			*chkchar = actor;
			break;
		case 't':
			*chkchar = (CHAR_DATA*) vo;
			break;
		case 'r':
			*chkchar = rndm;
			break;
		case 'o':
			*chkobj = obj;
			break;
		case 'p':
			*chkobj = (OBJ_DATA*) vo;
			break;
		default:
			swsnprintf(buf, MIL, "Var assignment error, bad argument '%s'", cvar);
			progbug(buf, mob);
			return false;
		}
		if (!*chkchar && !*chkobj)
			return false;
	}

	return true;
}

/****************** Ratm - zmienne ******************
 Funkcja zwraca warto�� (wskaznik do stringa) zmiennej
 o podanej nazwie
 ****************************************************/
char* get_var_value(char *var_name, CHAR_DATA *mob)
{
	int i = 0;
	char **vtab;
	char buf[MIL];

	if (!str_prefix("$v", var_name))
	{
		switch (var_name[2])
		{
		case 'g':
			vtab = mob->variables->global_vars;
			break;
		case 'l':
			vtab = mob->variables->local_vars;
			break;
		case 'a':
			vtab = mob->variables->prog_args;
			break;
		default:
			sprintf(buf, "Bad variable name: '%s'", var_name);
			progbug(buf, mob);
			return NULL;
		}
	}
	else
	{
		sprintf(buf, "Bad variable name: '%s'", var_name);
		progbug(buf, mob);
		return NULL;
	}

	i = var_name[3] - 0x30;

	if (i < 0 || i > 9)
	{
		sprintf(buf, "Bad variable name: '%s'", var_name);
		progbug(buf, mob);
		return NULL;
	}
	return vtab[i];
}

/* Ratm - zmienne. Funkcja zwalnia pamiec zajmowana przez zmienne lokalne */
void clear_local_variables(CHAR_DATA *mob)
{
	int i;

	if (!mob->variables)
		return;

	for (i = 0; i < 10; i++)
	{
		if (mob->variables->local_vars[i])
		{
			STRFREE(mob->variables->local_vars[i]);
			mob->variables->local_vars[i] = NULL;
		}
	}
}

/* This routine handles the variables for command expansion.
 * If you want to add any go right ahead, it should be fairly
 * clear how it is done and they are quite easy to do, so you
 * can be as creative as you want. The only catch is to check
 * that your variables exist before you use them. At the moment,
 * using $t when the secondary target refers to an object
 * i.e. >prog_act drops~<nl>if ispc($t)<nl>sigh<nl>endif<nl>~<nl>
 * probably makes the mud crash (vice versa as well) The cure
 * would be to change act() so that vo becomes vict & v_obj.
 * but this would require a lot of small changes all over the code.
 */

/*
 *  There's no reason to make the mud crash when a variable's
 *  fubared.  I added some ifs.  I'm willing to trade some
 *  performance for stability. -Haus
 *
 *  Narn's fubar ***ANNIHILATES*** you!  Hmm, could we add that
 *  as a weapon type? -Narn
 *
 *  Added char_died and obj_extracted checks	-Thoric
 */
int mprog_translate(char *var, char *t, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm)
{
	static const char *const he_she[] =
	{ "to", "on", "ona" };
	static const char *const him_her[] =
	{ "temu", "jemu", "jej" };
	static const char *const his_her[] =
	{ "tego", "jego", "jej" };
	CHAR_DATA *vict = (CHAR_DATA*) vo;
	OBJ_DATA *v_obj = (OBJ_DATA*) vo;
	char buf[MIL];

	*t = '\0';
	switch (*var)
	{
	case 'i':
		if (mob && !char_died(mob))
		{
			if (mob->name)
				one_argument(mob->name, t);
		}
		else
			strcpy(t, "kto�");
		break;

	case 'I':
		if (mob && !char_died(mob))
		{
			if (mob->przypadki[0])
			{
				strcpy(t, NAME(mob, 0));
			}
			else
			{
				strcpy(t, "kto�");
			}
		}
		else
			strcpy(t, "kto�");
		break;

	case 'n':
		if (actor && !char_died(actor) && can_see(mob, actor))
		{
			one_argument(actor->name, t);
			if (!IS_NPC(actor))
				*t = UPPER(*t);
		}
		else
			strcpy(t, "Kto�");
		break;

	case 'N':
		if (actor && !char_died(actor))
		{
			if (can_see(mob, actor))
				if (IS_NPC(actor))
					strcpy(t, NAME(actor, 0));
				else
				{
					strcpy(t, actor->name);
					strcpy(t, " ");
					strcat(t, actor->pcdata->title);
					strcat(t, PLAIN);
				}
			else
				strcpy(t, "kto�");
		}
		else
			strcpy(t, "kto�");
		break;

	case 't':
		if (vict && !char_died(vict))
		{
			one_argument(vict->name, t);
			if (!IS_NPC(vict))
				*t = UPPER(*t);
		}
		else
			strcpy(t, "kto�");

		break;

	case 'T':
		if (vict && !char_died(vict))
		{
			if (can_see(mob, vict))
				if (IS_NPC(vict))
					strcpy(t, NAME(vict, 0));
				else
				{
					strcpy(t, vict->name);
					strcat(t, " ");
					strcat(t, vict->pcdata->title);
					strcat(t, PLAIN);
				}
			else
				strcpy(t, "kto�");
		}
		else
			strcpy(t, "kto�");
		break;

	case 'r':
		if (rndm && !char_died(rndm))
		{
			one_argument(rndm->name, t);
			if (!IS_NPC(rndm))
			{
				*t = UPPER(*t);
			}
		}
		else
			strcpy(t, "kto�");
		break;

	case 'R':
		if (rndm && !char_died(rndm))
		{
			if (can_see(mob, rndm))
				if (IS_NPC(rndm))
					strcpy(t, NAME(rndm, 0));
				else
				{
					strcpy(t, NAME(rndm, 0));
					strcat(t, " ");
					if (IS_AFFECTED(rndm, AFF_DISGUISE))
						strcat(t, rndm->pcdata->fake_title);
					else
						strcat(t, rndm->pcdata->title);
					strcat(t, PLAIN);
				}
			else
				strcpy(t, "kto�");
		}
		else
			strcpy(t, "kto�");
		break;

	case 'e':
		if (actor && !char_died(actor))
		{
			can_see(mob, actor) ? strcpy(t, he_she[actor->sex]) : strcpy(t, "kto�");
		}
		else
			strcpy(t, "to");
		break;

	case 'm':
		if (actor && !char_died(actor))
		{
			can_see(mob, actor) ? strcpy(t, him_her[actor->sex]) : strcpy(t, "kto�");
		}
		else
			strcpy(t, "temu");
		break;

	case 's':
		if (actor && !char_died(actor))
		{
			can_see(mob, actor) ? strcpy(t, his_her[actor->sex]) : strcpy(t, "czyj�");
		}
		else
			strcpy(t, "tego");
		break;

	case 'E':
		if (vict && !char_died(vict))
		{
			can_see(mob, vict) ? strcpy(t, he_she[vict->sex]) : strcpy(t, "kto�");
		}
		else
			strcpy(t, "to");
		break;

	case 'M':
		if (vict && !char_died(vict))
		{
			can_see(mob, vict) ? strcpy(t, him_her[vict->sex]) : strcpy(t, "komu�");
		}
		else
			strcpy(t, "temu");
		break;

	case 'S':
		if (vict && !char_died(vict))
		{
			can_see(mob, vict) ? strcpy(t, his_her[vict->sex]) : strcpy(t, "czyj�");
		}
		else
			strcpy(t, "czego�");
		break;

	case 'j':
		if (mob && !char_died(mob))
		{
			strcpy(t, he_she[mob->sex]);
		}
		else
		{
			strcpy(t, "to");
		}
		break;

	case 'k':
		if (mob && !char_died(mob))
		{
			strcpy(t, him_her[mob->sex]);
		}
		else
		{
			strcpy(t, "to");
		}
		break;

	case 'l':
		if (mob && !char_died(mob))
		{
			strcpy(t, his_her[mob->sex]);
		}
		else
		{
			strcpy(t, "to");
		}
		break;

	case 'J':
		if (rndm && !char_died(rndm))
		{
			can_see(mob, rndm) ? strcpy(t, he_she[rndm->sex]) : strcpy(t, "kto�");
		}
		else
			strcpy(t, "co�");
		break;

	case 'K':
		if (rndm && !char_died(rndm))
		{
			can_see(mob, rndm) ? strcpy(t, him_her[rndm->sex]) : strcpy(t, "komu�");
		}
		else
			strcpy(t, "czemu�");
		break;

	case 'L':
		if (rndm && !char_died(rndm))
		{
			can_see(mob, rndm) ? strcpy(t, his_her[rndm->sex]) : strcpy(t, "czyj�");
		}
		else
			strcpy(t, "czego�");
		break;

	case 'o':
		if (obj && !obj_extracted(obj))
		{
			can_see_obj(mob, obj) ? one_argument(obj->name, t) : strcpy(t, "co�");
		}
		else
			strcpy(t, "co�");
		break;

	case 'O':
		if (obj && !obj_extracted(obj))
		{
			can_see_obj(mob, obj) ? strcpy(t, obj->przypadki[0]) : strcpy(t, "co�");
		}
		else
			strcpy(t, "co�");
		break;

	case 'p':
		if (v_obj && !obj_extracted(v_obj))
		{
			can_see_obj(mob, v_obj) ? one_argument(v_obj->name, t) : strcpy(t, "co�");
		}
		else
			strcpy(t, "co�");
		break;

	case 'P':
		if (v_obj && !obj_extracted(v_obj))
		{
			can_see_obj(mob, v_obj) ? strcpy(t, v_obj->przypadki[0]) : strcpy(t, "co�");
		}
		else
			strcpy(t, "co�");
		break;

	case 'a':
		strcpy(t, SEX_SUFFIX_YAE(mob));
		break;

	case 'A':
		strcpy(t, SEX_SUFFIX_YAE(actor));
		break;

		/*
		 Thanos: dla zgodno�ci prog�w z np. Ku�ni� Dusz
		 $1  - Thanos
		 $2  - Thanosa
		 .
		 .
		 .
		 $6  - Thanosie
		 */
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
		strcpy(t, PERS(actor, mob, var[0] - 49));
		break;

		// Ratm - zmienne
	case 'v':
	{
		char *tmpstr = get_var_value(--var, mob);
		if (tmpstr)
			strcpy(t, tmpstr);
		else
		{
			sprintf(buf, "Variable '%s' not initialized", var);
			progbug(buf, mob);
		}
	}
		return 3;
	case '$':
		strcpy(t, "$");
		break;

	default:
		sprintf(buf, "Bad variable name: '$%s'", var);
		progbug(buf, mob);
		break;
	}

	return 1;

}

/*  The main focus of the MOBprograms.  This routine is called
 *  whenever a trigger is successful.  It is responsible for parsing
 *  the command list and figuring out what to do. However, like all
 *  complex procedures, everything is farmed out to the other guys.
 *
 *  This function rewritten by Narn for Realms of Despair, Dec/95.
 *
 */
void mprog_driver(char *com_list, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, bool single_step)
{
	char tmpcmndlst[ MAX_STRING_LENGTH];
	char *command_list;
	char *cmnd;
	CHAR_DATA *rndm = NULL;
	int count = 0;
	int ignorelevel = 0;
	int iflevel, result;
	bool ifstate[MAX_IFS][ DO_ELSE + 1];
	static int prog_nest;

	if (IS_AFFECTED(mob, AFF_CHARM))
		return;

	/* Next couple of checks stop program looping. -- Altrag */
	if (mob == actor)
	{
		progbug("triggering oneself.", mob);
		return;
	}

	if (++prog_nest > MAX_PROG_NEST)
	{
		progbug("max_prog_nest exceeded.", mob);
		--prog_nest;
		return;
	}

	/* Make sure all ifstate bools are set to false */
	for (iflevel = 0; iflevel < MAX_IFS; iflevel++)
	{
		for (count = 0; count < DO_ELSE; count++)
		{
			ifstate[iflevel][count] = false;
		}
	}

	iflevel = 0;

	/*
	 * get a random visible player who is in the room with the mob.
	 *
	 *  If there isn't a random player in the room, rndm stays NULL.
	 *  If you do a $r, $R, $j, or $k with rndm = NULL, you'll crash
	 *  in mprog_translate.
	 *
	 *  Adding appropriate error checking in mprog_translate.
	 *    -Haus
	 *
	 * This used to ignore players MAX_LEVEL - 3 and higher (standard
	 * Merc has 4 immlevels).  Thought about changing it to ignore all
	 * imms, but decided to just take it out.  If the mob can see you,
	 * you may be chosen as the random player. -Narn
	 *
	 */

	count = 0;
	if (mob->in_room) //Trog: bugfix
		for (auto* vch : mob->in_room->people)
			if (!IS_NPC(vch) && can_see(mob, vch))
			{
				if (number_range(0, count) == 0)
					rndm = vch;
				count++;
			}

	strcpy(tmpcmndlst, com_list);
	command_list = tmpcmndlst;
	if (single_step)
	{
		if (curr_script->position > (int) strlen(tmpcmndlst))
		{
			/* BUG - script progi dzialay w niekonczonosc (bleh)	-- Thanos */
			stopscript(curr_script);
			--prog_nest;
		}
		else
			command_list += curr_script->position;

		if (*command_list == '\0')
		{
			command_list = tmpcmndlst;
			/* zebysmy nie wylecieli z proga -- Thanos */
			stopscript(curr_script);
			--prog_nest;
			return;
		}
	}

	/* From here on down, the function is all mine.  The original code
	 did not support nested ifs, so it had to be redone.  The max
	 logiclevel (MAX_IFS) is defined at the beginning of this file,
	 use it to increase/decrease max allowed nesting.  -Narn
	 */

	while (true)
	{
		/* With these two lines, cmnd becomes the current line from the prog,
		 and command_list becomes everything after that line. */
		cmnd = command_list;
		command_list = mprog_next_command(command_list);
		current_prog_line++;

		/* Are we at the end? */
		if (cmnd[0] == '\0')
		{
			if (ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE])
			{
				progbug("Missing endif", mob);
			}
			--prog_nest;
			return;
		}

		/* Evaluate/execute the command, check what happened. */
		result = mprog_do_command(cmnd, mob, actor, obj, vo, rndm,
				(ifstate[iflevel][IN_IF] && !ifstate[iflevel][DO_IF]) || (ifstate[iflevel][IN_ELSE] && !ifstate[iflevel][DO_ELSE]),
				(ignorelevel > 0));

		/* Script prog support  -Thoric */
		if (single_step)
		{
			if (curr_script)
			{
				curr_script->position = command_list - tmpcmndlst;
				curr_script->curr_line = current_prog_line;
			}

			--prog_nest;
			return;
		}

		/* This is the complicated part.  Act on the returned value from
		 mprog_do_command according to the current logic state. */
		switch (result)
		{
		case COMMANDOK:
#ifdef DEBUG
log_string( "COMMANDOK" );
#endif
			/* Ok, this one's a no-brainer. */
			continue;
			break;

		case IFtrue:
#ifdef DEBUG
log_string( "IFtrue" );
#endif
			/* An if was evaluated and found true.  Note that we are in an
			 if section and that we want to execute it. */
			iflevel++;
			if (iflevel > MAX_IFS)
			{
				progbug("Maximum nested ifs exceeded", mob);
				--prog_nest;
				return;
			}

			ifstate[iflevel][IN_IF] = true;
			ifstate[iflevel][DO_IF] = true;
			break;

		case IFfalse:
#ifdef DEBUG
log_string( "IFfalse" );
#endif
			/* An if was evaluated and found false.  Note that we are in an
			 if section and that we don't want to execute it unless we find
			 an or that evaluates to true. */
			iflevel++;
			if (iflevel > MAX_IFS)
			{
				progbug("Maximum nested ifs exceeded", mob);
				--prog_nest;
				return;
			}
			ifstate[iflevel][IN_IF] = true;
			ifstate[iflevel][DO_IF] = false;
			break;

		case ORtrue:
#ifdef DEBUG
log_string( "ORtrue" );
#endif
			/* An or was evaluated and found true.  We should already be in an
			 if section, so note that we want to execute it. */
			if (!ifstate[iflevel][IN_IF])
			{
				progbug("Unmatched or", mob);
				--prog_nest;
				return;
			}
			ifstate[iflevel][DO_IF] = true;
			break;

		case ORfalse:
#ifdef DEBUG
log_string( "ORfalse" );
#endif
			/* An or was evaluated and found false.  We should already be in an
			 if section, and we don't need to do much.  If the if was true or
			 there were/will be other ors that evaluate(d) to true, they'll set
			 do_if to true. */
			if (!ifstate[iflevel][IN_IF])
			{
				progbug("Unmatched or", mob);
				--prog_nest;
				return;
			}
			continue;
			break;

		case FOUNDELSE:
#ifdef DEBUG
log_string( "FOUNDELSE" );
#endif
			/* Found an else.  Make sure we're in an if section, bug out if not.
			 If this else is not one that we wish to ignore, note that we're now
			 in an else section, and look at whether or not we executed the if
			 section to decide whether to execute the else section.  Ca marche
			 bien. */
			if (ignorelevel > 0)
				continue;

			if (ifstate[iflevel][IN_ELSE])
			{
				progbug("Found else in an else section", mob);
				--prog_nest;
				return;
			}
			if (!ifstate[iflevel][IN_IF])
			{
				progbug("Unmatched else", mob);
				--prog_nest;
				return;
			}

			ifstate[iflevel][IN_ELSE] = true;
			ifstate[iflevel][DO_ELSE] = !ifstate[iflevel][DO_IF];
			ifstate[iflevel][IN_IF] = false;
			ifstate[iflevel][DO_IF] = false;

			break;

		case FOUNDENDIF:
#ifdef DEBUG
log_string( "FOUNDENDIF" );
#endif
			/* Hmm, let's see... FOUNDENDIF must mean that we found an endif.
			 So let's make sure we were expecting one, return if not.  If this
			 endif matches the if or else that we're executing, note that we are
			 now no longer executing an if.  If not, keep track of what we're
			 ignoring. */
			if (!(ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE]))
			{
				progbug("Unmatched endif", mob);
				--prog_nest;
				return;
			}

			if (ignorelevel > 0)
			{
				ignorelevel--;
				continue;
			}

			ifstate[iflevel][IN_IF] = false;
			ifstate[iflevel][DO_IF] = false;
			ifstate[iflevel][IN_ELSE] = false;
			ifstate[iflevel][DO_ELSE] = false;

			iflevel--;
			break;

		case IFIGNORED:
#ifdef DEBUG
log_string( "IFIGNORED" );
#endif
			if (!(ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE]))
			{
				progbug("Parse error, ignoring if while not in if or else", mob);
				--prog_nest;
				return;
			}
			ignorelevel++;
			break;

		case ORIGNORED:
#ifdef DEBUG
log_string( "ORIGNORED" );
#endif
			if (!(ifstate[iflevel][IN_IF] || ifstate[iflevel][IN_ELSE]))
			{
				progbug("Unmatched or", mob);
				--prog_nest;
				return;
			}
			if (ignorelevel == 0)
			{
				progbug("Parse error, mistakenly ignoring or", mob);
				--prog_nest;
				return;
			}

			break;

		case BERR:
#ifdef DEBUG
log_string( "BERR" );
#endif
			--prog_nest;
			return;
			break;
		}
	}
	--prog_nest;
	return;
}

/* Thanos 	-- uruchamia skrypt */
void runscript(char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm)
{
	int prog_nr = 0;
	int p_nr = 0;
	char arg[MAX_INPUT_LENGTH];
	ROOM_INDEX_DATA *s_room = NULL;
	OBJ_DATA *s_obj = NULL;

	if (mob == supermob)
	{
		s_room = mob->curr_room;
		s_obj = mob->curr_obj;
	}
	else if (!IS_NPC(mob))
	{
		huh(mob);
		return;
	}

	if (mob->mpscriptrun > MAX_SCRIPTS)
		return;

	prog_nr = atoi(one_argument(cmnd, arg));

	/* je�liby odkomentowa� t� lini� mud b�dzie zabezpiecza�
	 si� przed wywo�aniem skryptu samego z siebie.
	 A teraz nie pozwala na w��czenie skryptu z poziomu skryptu W OG�LE */
	if (curr_script && curr_script->active
	/*    &&  curr_script->trigger == prog_nr  */)
	{
//	bug( "Script_prog Loop detected. No Way!" );
		progbug("Trying to run Script from script.", mob);
		return;
	}

	if (s_obj)
	{
		if (!IS_SET(s_obj->pIndexData->progtypes, SCRIPT_PROG))
		{
			bug("Runscript: Obj: {%d}. No script_progs found.", s_obj->pIndexData->vnum);
			return;
		}
	}
	else if (s_room)
	{
		if (!IS_SET(s_room->progtypes, SCRIPT_PROG))
		{
			bug("Runscript: Room: {%d}. No script_progs found.", s_room->vnum);
			return;
		}
	}
	else
	{
		if (!IS_SET(mob->pIndexData->progtypes, SCRIPT_PROG))
		{
			bug("Runscript: Mob: {%d}. No script_progs found.", mob->pIndexData->vnum);
			return;
		}
	}

	if (!prog_nr)
	{
		if (s_room)
			bug("Runscript: Room: {%d} invalid usage of mprunscript.", s_room->vnum);
		else if (s_obj)
			bug("Runscript: Obj: {%d} invalid usage of mprunscript.", s_obj->pIndexData->vnum);
		else
			bug("Runscript: Mob: {%d} invalid usage of mprunscript.", mob->pIndexData->vnum);
		return;
	}

	auto& script_progs = s_room ? s_room->mudprogs : s_obj ? s_obj->pIndexData->mudprogs : mob->pIndexData->mudprogs;
	for (auto* pMprg : script_progs)
	{
		p_nr++;
		if (pMprg->type == SCRIPT_PROG && prog_nr == atoi(pMprg->arglist))
		{
			SCRIPT_DATA *pScript;

			CREATE(pScript, SCRIPT_DATA, 1);
			STRDUP(pScript->command, pMprg->comlist);
			pScript->victim = actor;
			pScript->random = rndm;
			pScript->object = obj;
			pScript->vo = vo;
			pScript->position = 0;
			pScript->curr_line = 0;
			pScript->trigger = prog_nr;
			pScript->prog_nr = p_nr;
			pScript->active = false;

			pScript->m_owner = NULL;
			pScript->o_owner = NULL;
			pScript->r_owner = NULL;

			if (s_room)
			{
				pScript->r_owner = s_room;
				s_room->mpscriptrun++;
			}
			else if (s_obj)
			{
				pScript->o_owner = s_obj;
				s_obj->mpscriptrun++;
			}
			else
			{
				pScript->m_owner = mob;
				mob->mpscriptrun++;
			}

			script_prog_list.push_back(pScript);

			update_script(pScript);
			return;
		}
	}

	if (s_room)
		bug("Runscript: Room {%d} Wrong script_prog nr.", s_room->vnum);
	else if (s_obj)
		bug("Runscript: Obj {%d} Wrong script_prog nr.", s_obj->pIndexData->vnum);
	else
		bug("Runscript: Mob {%d} Wrong script_prog nr.", mob->pIndexData->vnum);
	return;
}

/* Thanos --> czy�ci i usuwa z listy aktualnych skrypt */
void stopscript(SCRIPT_DATA *script)
{
	if (curr_script == script)
		curr_script = NULL;

	script_prog_list.remove(script);

	if (script->m_owner)
		script->m_owner->mpscriptrun--;

	if (script->r_owner)
		script->r_owner->mpscriptrun--;

	if (script->o_owner)
		script->o_owner->mpscriptrun--;

	free_script(script);
	return;
}

/* This function replaces mprog_process_cmnd.  It is called from
 * mprog_driver, once for each line in a mud prog.  This function
 * checks what the line is, executes if/or checks and calls interpret
 * to perform the the commands.  Written by Narn, Dec 95.
 */
int mprog_do_command(char *cmnd, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, CHAR_DATA *rndm, bool ignore, bool ignore_ors)
{
	char firstword[MAX_INPUT_LENGTH];
	char *ifcheck;
	char buf[MAX_INPUT_LENGTH];
	char buf1[MAX_INPUT_LENGTH];
	char tmp[MAX_INPUT_LENGTH];
	char *point, *str, *i;
	int validif;
	int index = 0;
	char **vtab = 0;

	/* Isolate the first word of the line, it gives us a clue what
	 we want to do. */
	ifcheck = one_argument(cmnd, firstword);

	if (!str_cmp(firstword, "if"))
	{
		/* Ok, we found an if.  According to the boolean 'ignore', either
		 ignore the ifcheck and report that back to mprog_driver or do
		 the ifcheck and report whether it was successful. */
		if (ignore)
			return IFIGNORED;
		else
			validif = mprog_do_ifcheck(ifcheck, mob, actor, obj, vo, rndm);

		if (validif == 1)
			return IFtrue;

		if (validif == 0)
			return IFfalse;

		return BERR;
	}

	if (!str_cmp(firstword, "or"))
	{
		/* Same behavior as with ifs, but use the boolean 'ignore_ors' to
		 decide which way to go. */
		if (ignore_ors)
			return ORIGNORED;
		else
			validif = mprog_do_ifcheck(ifcheck, mob, actor, obj, vo, rndm);

		if (validif == 1)
			return ORtrue;

		if (validif == 0)
			return ORfalse;

		return BERR;
	}

	/* For else and endif, just report back what we found.  Mprog_driver
	 keeps track of logiclevels. */
	if (!str_cmp(firstword, "else"))
	{
		return FOUNDELSE;
	}

	if (!str_cmp(firstword, "endif"))
	{
		return FOUNDENDIF;
	}

	/* Ok, didn't find an if, an or, an else or an endif.
	 If the command is in an if or else section that is not to be
	 performed, the boolean 'ignore' is set to true and we just
	 return.  If not, we try to execute the command. */

	if (ignore)
		return COMMANDOK;

	/* If the command is 'break', that's all folks. */
	if (!str_cmp(firstword, "break"))
		return BERR;

	/* Ratm - Je�li komenda zaczyna sie od $v to znaczy ze jest to
	 przypisanie wartosci zmiennej */
	if (!str_prefix("$v", firstword))
	{
		switch (firstword[2])
		{
		case 'g':
			vtab = mob->variables->global_vars;
			break;
		case 'l':
			vtab = mob->variables->local_vars;
			break;
		default:
			progbug("Bad variable name", mob);
			return BERR;
		}

		index = firstword[3] - 0x30;

		if (index < 0 || index > 9)
		{
			progbug("Bad variable name", mob);
			return BERR;
		}
		if (firstword[4] == '+' && firstword[5] == '+')
		{
			if (!vtab[index])
			{
				progbug("Can't increment variable, var not initialized", mob);
				return BERR;
			}
			sprintf(buf, "%i", 1 + atoi(vtab[index]));
			STRDUP(vtab[index], buf);
			return COMMANDOK;
		}
		else if (firstword[4] == '-' && firstword[5] == '-')
		{
			if (!vtab[index])
			{
				progbug("Can't decrement variable, var not initialized", mob);
				return BERR;
			}
			sprintf(buf, "%i", atoi(vtab[index]) - 1);
			STRDUP(vtab[index], buf);
			return COMMANDOK;
		}
		else
		{
			str = one_argument(ifcheck, firstword);

			if (str_cmp(firstword, "="))
			{
				progbug("Bad variable assignment", mob);
				return BERR;
			}
			if (eval_check_func(&vtab[index], str, mob, actor, obj, vo, rndm) != BERR)
				return COMMANDOK;
		}
	}
	else
		str = cmnd;

	point = buf;

	/* This chunk of code taken from mprog_process_cmnd. */
	while (*str != '\0')
	{
		if (*str != '$')
		{
			*point++ = *str++;
			continue;
		}
		str++;
		str += mprog_translate(str, tmp, mob, actor, obj, vo, rndm);
		i = tmp;
		while ((*point = *i) != '\0')
			++point, ++i;
	}
	*point = '\0';

	/* Ratm - obliczenie i przypisanie zmiennej wyliczonej wartosci */
	if (!str_cmp(firstword, "="))
	{
		str = one_argument(buf, buf1);
		str = one_argument(str, tmp);
		// sprawdzamy czy mamy do czynienia z dzia�aniem matematycznym
		if (*str && (*tmp == '+' || *tmp == '-' || *tmp == '*' || *tmp == '/'))
		{
			int lval, rval;
			one_argument(str, buf);
			lval = atoi(buf1);
			rval = atoi(buf);

			switch (*tmp)
			{
			case '+':
				lval += rval;
				break;
			case '-':
				lval -= rval;
				break;
			case '*':
				lval *= rval;
				break;
			case '/':
				if (rval == 0)
				{
					progbug("Division by zero", mob);
					return BERR;
				}
				lval /= rval;
				break;
			}

			sprintf(buf, "%i", lval);
		}

		STRDUP(vtab[index], buf);
		return COMMANDOK;
	}

	/* Thanos start scriptproga
	 (hardcoding sux, ale tutaj �atwiej zapami�tac actora) */
	one_argument(buf, buf1);
	if (!str_prefix("mprunscript", buf1))
		runscript(buf, mob, actor, obj, vo, rndm);
	else
		interpret(mob, buf);

	/* If the mob is mentally unstable and does things like fireball
	 itself, let's make sure it's still alive. */
	if (char_died(mob))
	{
		return BERR;
	}

	return COMMANDOK;
}

/***************************************************************************
 * Global function code and brief comments.
 */

bool mprog_keyword_check(const char *argu, const char *argl)
{
	char word[MAX_INPUT_LENGTH];
	char arg1[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];
	int i;
	char *arg, *arglist;
	char *start, *end;

	strcpy(arg1, strlower(argu));
	arg = arg1;
	strcpy(arg2, strlower(argl));
	arglist = arg2;

	for (i = 0; i < (int) strlen(arglist); i++)
		arglist[i] = LOWER(arglist[i]);
	for (i = 0; i < (int) strlen(arg); i++)
		arg[i] = LOWER(arg[i]);
	if ((arglist[0] == 'p') && (arglist[1] == ' '))
	{
		arglist += 2;
		while ((start = strstr(arg, arglist)))
			if ((start == arg || *(start - 1) == ' ')
					&& (*(end = start + strlen(arglist)) == ' ' || *end == '\n' || *end == '\r' || *end == '\0'))
				return true;
			else
				arg = start + 1;
	}
	else
	{
		arglist = one_argument(arglist, word);
		for (; word[0] != '\0'; arglist = one_argument(arglist, word))
			while ((start = strstr(arg, word)))
				if ((start == arg || *(start - 1) == ' ')
						&& (*(end = start + strlen(word)) == ' ' || *end == '\n' || *end == '\r' || *end == '\0'))
					return true;
				else
					arg = start + 1;
	}
	/*    bug( "don't match" ); */
	return false;
}

/* The next two routines are the basic trigger types. Either trigger
 *  on a certain percent, or trigger on a keyword or word phrase.
 *  To see how this works, look at the various trigger routines..
 */
void mprog_wordlist_check(char *arg, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type)
{

	char temp1[ MAX_STRING_LENGTH];
	char temp2[ MAX_INPUT_LENGTH];
	char word[ MAX_INPUT_LENGTH];
	char *list;
	char *start;
	char *dupl;
	char *end;
	int i;
	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	for (auto* mprg : mob->pIndexData->mudprogs)
	{
		prog_nr++;
		if (mprg->type & type)
		{
			strcpy(temp1, mprg->arglist);
			list = temp1;
			for (i = 0; i < (int) strlen(list); i++)
				list[i] = LOWER(list[i]);
			strcpy(temp2, arg);
			dupl = temp2;
			for (i = 0; i < (int) strlen(dupl); i++)
				dupl[i] = LOWER(dupl[i]);
			if ((list[0] == 'p') && (list[1] == ' '))
			{
				list += 2;
				while ((start = str_str(list, dupl)))
					if ((start == dupl || *(start - 1) == ' ')
							&& (*(end = start + strlen(list)) == ' ' || *end == '\n' || *end == '\r' || *end == '\0'))
					{
						// Ratm - zmienne. Wszystko ponizej do mprog_driver()
						int j;
						char argbuf[MIL + 10]; // Ratm +10 na wszelki wypadek ;)
						char wordbuf[MIL];
						char *argstr = arg;
						char *abufstr = argbuf;

						clear_local_variables(mob);
						mob->variables->prog_args[0] = arg;

						for (j = 1; j < 10; j++)
						{
							int len;

							argstr = one_argument(argstr, wordbuf);
							if (*wordbuf == '\0')
								break;

							len = strlen(wordbuf);
							if (wordbuf[len - 1] == '\r')
								len--;
							if (wordbuf[len - 1] == '\n')
								len--;

							wordbuf[len] = '\0';
							strcpy(abufstr, wordbuf);
							mob->variables->prog_args[j] = abufstr;
							abufstr += len + 1;
						}

						current_prog_line = 0;
						current_prog_number = prog_nr;
						mprog_driver(mprg->comlist, mob, actor, obj, vo, false);
						//Ratm Po wykonaniu proga czyscimy niepotrzebne zmienne
						clear_local_variables(mob);
						for (j = 0; j < 10; j++)
							mob->variables->prog_args[j] = NULL;

						break;
					}
					else
						dupl = start + 1;
			}
			else
			{
				list = one_argument(list, word);
				for (; word[0] != '\0'; list = one_argument(list, word))
					while ((start = str_str(word, dupl)))
						if ((start == dupl || *(start - 1) == ' ')
								&& (*(end = start + strlen(word)) == ' ' || *end == '\n' || *end == '\r' || *end == '\0'))
						{
							// Ratm - zmienne. Wszystko ponizej do mprog_driver()
							int j;
							char argbuf[MIL + 10]; // Ratm +10 na wszelki wypadek ;)
							char wordbuf[MIL];
							char *argstr = arg;
							char *abufstr = argbuf;

							clear_local_variables(mob);
							mob->variables->prog_args[0] = arg;

							for (j = 1; j < 10; j++)
							{
								int len;

								argstr = one_argument(argstr, wordbuf);
								if (*wordbuf == '\0')
									break;

								len = strlen(wordbuf);
								if (wordbuf[len - 1] == '\r')
									len--;
								if (wordbuf[len - 1] == '\n')
									len--;

								wordbuf[len] = '\0';
								strcpy(abufstr, wordbuf);
								mob->variables->prog_args[j] = abufstr;
								abufstr += len + 1;
							}

							current_prog_line = 0;
							current_prog_number = prog_nr;
							mprog_driver(mprg->comlist, mob, actor, obj, vo, false);

							//Ratm Po wykonaniu proga czyscimy niepotrzebne zmienne
							clear_local_variables(mob);
							for (j = 0; j < 10; j++)
								mob->variables->prog_args[j] = NULL;

							break;
						}
						else
							dupl = start + 1;
			}
		}
	}

	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;

}

void mprog_percent_check(CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int type)
{
	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;
	int value;

	for (auto* mprg : mob->pIndexData->mudprogs)
	{
		prog_nr++;
		if ((mprg->type & type) && (number_percent() <= (value = atoi(mprg->arglist))))
		{
			if (type != RAND_PROG || (type == RAND_PROG && value > 0))
			{
				current_prog_line = 0;
				current_prog_number = prog_nr;
				// Ratm - zmienne. Wszystko ponizej do mprog_driver()
				mob->variables->prog_args[0] = mprg->arglist;
				clear_local_variables(mob);
				mprog_driver(mprg->comlist, mob, actor, obj, vo, false);
				mob->variables->prog_args[0] = NULL; // Ratm zmienne
				if (type != GREET_PROG && type != ALL_GREET_PROG)
					break;
			}
		}
	}

	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
}

void mprog_time_check(CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int type)
{
	bool trigger_time;
	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	for (auto* mprg : mob->pIndexData->mudprogs)
	{
		prog_nr++;

		trigger_time = ((GET_PLANET(mob))->hour == atoi(mprg->arglist));

		if (!trigger_time)
		{
			if (mprg->triggered)
				mprg->triggered = false;
			continue;
		}

		if ((mprg->type & type) && ((!mprg->triggered) || (mprg->type & HOUR_PROG)))
		{
			mprg->triggered = true;
			current_prog_line = 0;
			current_prog_number = prog_nr;
			// Ratm - zmienne. Wszystko ponizej do mprog_driver()
			mob->variables->prog_args[0] = mprg->arglist;
			clear_local_variables(mob);
			mprog_driver(mprg->comlist, mob, actor, obj, vo, false);
			mob->variables->prog_args[0] = NULL; //Ratm zmienne
		}
	}
	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;
}

void mob_act_add(CHAR_DATA *mob)
{
	for (auto* runner : mob_act_list)
		if (runner->vo == mob)
			return;
	struct act_prog_data *runner;
	CREATE(runner, struct act_prog_data, 1);
	runner->vo = mob;
	mob_act_list.push_front(runner);
}

/* The triggers.. These are really basic, and since most appear only
 * once in the code (hmm. i think they all do) it would be more efficient
 * to substitute the code in and make the mprog_xxx_check routines global.
 * However, they are all here in one nice place at the moment to make it
 * easier to see what they look like. If you do substitute them back in,
 * make sure you remember to modify the variable names to the ones in the
 * trigger calls.
 */
void mprog_act_trigger(char *buf, CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj, void *vo)
{
	MPROG_ACT_LIST *tmp_act;
	bool found = false;

	if ( IS_NPC( mob ) && IS_SET(mob->pIndexData->progtypes, ACT_PROG))
	{
		/* Don't let a mob trigger itself, nor one instance of a mob
		 trigger another instance. */
		if ( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData)
			return;

		/* make sure this is a matching trigger */
		for (auto* mprg : mob->pIndexData->mudprogs)
			if (mprg->type & ACT_PROG && mprog_keyword_check(buf, mprg->arglist))
			{
				found = true;
				break;
			}
		if (!found)
			return;

		CREATE(tmp_act, MPROG_ACT_LIST, 1);
		if (mob->mpactnum > 0)
			tmp_act->next = mob->mpact;
		else
			tmp_act->next = NULL;

		mob->mpact = tmp_act;
		STRDUP(mob->mpact->buf, buf);
		mob->mpact->ch = ch;
		mob->mpact->obj = obj;
		mob->mpact->vo = vo;
		mob->mpactnum++;
		mob_act_add(mob);
	}
	return;
}

void mprog_bribe_trigger(CHAR_DATA *mob, CHAR_DATA *ch, int amount)
{
	char buf[ MAX_STRING_LENGTH];
	OBJ_DATA *obj;
	int i;
	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	if ( IS_NPC( mob ) && (mob->pIndexData->progtypes & BRIBE_PROG))
	{
		/* Don't let a mob trigger itself, nor one instance of a mob
		 trigger another instance. */
		if ( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData)
			return;

		obj = create_object(get_obj_index( OBJ_VNUM_MONEY_SOME), 0);
		for (i = 0; i < 6; i++)
		{
			sprintf(buf, obj->przypadki[i], amount);
			STRDUP(obj->przypadki[i], buf);
		}
		obj->value[0] = amount;
		obj = obj_to_char(obj, mob);
		mob->gold -= amount;

		for (auto* mprg : mob->pIndexData->mudprogs)
		{
			prog_nr++;
			if ((mprg->type & BRIBE_PROG) && (amount >= atoi(mprg->arglist)))
			{
				// Ratm - zmienne. Wszystko ponizej do mprog_driver()
				char buf[MIL];

				sprintf(buf, "%i", amount);
				mob->variables->prog_args[0] = buf;
				clear_local_variables(mob);
				current_prog_line = 0;
				current_prog_number = prog_nr;
				mprog_driver(mprg->comlist, mob, ch, obj, NULL, false);
				mob->variables->prog_args[0] = NULL; //Ratm zmienne
				break;
			}
		}
	}
	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;

}

void mprog_death_trigger(CHAR_DATA *killer, CHAR_DATA *mob)
{
	mob->position = POS_STANDING;
	if ( IS_NPC( mob ) && killer != mob && (mob->pIndexData->progtypes & DEATH_PROG))
	{
		mprog_percent_check(mob, killer, NULL, NULL, DEATH_PROG);
	}
	mob->position = POS_DEAD;
	death_cry(mob);
	return;
}

void mprog_entry_trigger(CHAR_DATA *mob)
{
	if ( IS_NPC( mob ) && (mob->pIndexData->progtypes & ENTRY_PROG))
		mprog_percent_check(mob, NULL, NULL, NULL, ENTRY_PROG);

	return;
}

void mprog_fight_trigger(CHAR_DATA *mob, CHAR_DATA *ch)
{

	if ( IS_NPC( mob ) && (mob->pIndexData->progtypes & FIGHT_PROG))
		mprog_percent_check(mob, ch, NULL, NULL, FIGHT_PROG);

	return;

}

void mprog_give_trigger(CHAR_DATA *mob, CHAR_DATA *ch, OBJ_DATA *obj)
{

	char buf[MAX_INPUT_LENGTH];
	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	if ( IS_NPC( mob ) && (mob->pIndexData->progtypes & GIVE_PROG))
	{
		/* Don't let a mob trigger itself, nor one instance of a mob
		 trigger another instance. */
		if ( IS_NPC( ch ) && ch->pIndexData == mob->pIndexData)
			return;

		for (auto* mprg : mob->pIndexData->mudprogs)
		{
			prog_nr++;
			one_argument(mprg->arglist, buf);

			if ((mprg->type & GIVE_PROG) && ((!str_cmp(obj->name, mprg->arglist)) || (!str_cmp("all", buf))))
			{
				clear_local_variables(mob);
				current_prog_line = 0;
				current_prog_number = prog_nr;
				mprog_driver(mprg->comlist, mob, ch, obj, NULL, false);
				break;
			}
		}
	}
	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;
}

void mprog_greet_trigger(CHAR_DATA *ch)
{
#ifdef DEBUG
 char buf[MAX_STRING_LENGTH];
 sprintf( buf, "mprog_greet_trigger -> %s", ch->name );
 log_string( buf );
#endif

	auto people_snapshot = ch->in_room->people;
	for (auto* vmob : people_snapshot)
	{
		if (!IS_NPC(vmob) || vmob->fighting || !IS_AWAKE(vmob) || !can_see(vmob, ch))
			continue;

		/* Don't let a mob trigger itself, nor one instance of a mob
		 trigger another instance. */
		if ( IS_NPC( ch ) && ch->pIndexData == vmob->pIndexData)
			continue;

		if (vmob->pIndexData->progtypes & GREET_PROG)
			mprog_percent_check(vmob, ch, NULL, NULL, GREET_PROG);

	}
	return;

}

void mprog_all_greet_trigger(CHAR_DATA *ch)
{
	/* Imps are safe ;) */
	if (!IS_NPC(ch) && (
	IS_SET( ch->act, PLR_WIZINVIS ) || IS_SET(ch->act, PLR_HOLYLIGHT)))
		return;

#ifdef DEBUG
 char buf[MAX_STRING_LENGTH];
 sprintf( buf, "mprog_all_greet_trigger -> %s", ch->name );
 log_string( buf );
#endif

	auto people_snapshot = ch->in_room->people;
	for (auto* vmob : people_snapshot)
	{
		if (!IS_NPC(vmob))
			continue;

		/* Don't let a mob trigger itself, nor one instance of a mob
		 trigger another instance. */
		if ( IS_NPC( ch ) && ch->pIndexData == vmob->pIndexData)
			continue;

		if (vmob->pIndexData->progtypes & ALL_GREET_PROG)
			mprog_percent_check(vmob, ch, NULL, NULL, ALL_GREET_PROG);
	}
	return;

}

void mprog_hitprcnt_trigger(CHAR_DATA *mob, CHAR_DATA *ch)
{

	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	if ( IS_NPC( mob ) && (mob->pIndexData->progtypes & HITPRCNT_PROG))
		for (auto* mprg : mob->pIndexData->mudprogs)
		{
			prog_nr++;
			if ((mprg->type & HITPRCNT_PROG) && ((100 * mob->hit / mob->max_hit) < atoi(mprg->arglist)))
			{
				clear_local_variables(mob);
				current_prog_line = 0;
				current_prog_number = prog_nr;
				mprog_driver(mprg->comlist, mob, ch, NULL, NULL, false);
				break;
			}
		}
	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;

}

void mprog_random_trigger(CHAR_DATA *mob)
{
	if (mob->pIndexData->progtypes & RAND_PROG)
		mprog_percent_check(mob, NULL, NULL, NULL, RAND_PROG);

	return;
}

void mprog_time_trigger(CHAR_DATA *mob)
{
	if (mob->pIndexData->progtypes & TIME_PROG)
		mprog_time_check(mob, NULL, NULL, NULL, TIME_PROG);
	return;
}

void mprog_hour_trigger(CHAR_DATA *mob)
{
	if (mob->pIndexData->progtypes & HOUR_PROG)
		mprog_time_check(mob, NULL, NULL, NULL, HOUR_PROG);
	return;
}

void mprog_speech_trigger(char *txt, CHAR_DATA *actor)
{

	auto people_snapshot = actor->in_room->people;
	for (auto* vmob : people_snapshot)
	{
		if ( IS_NPC( vmob ) && (vmob->pIndexData->progtypes & SPEECH_PROG))
		{
			if ( IS_NPC( actor ) && actor->pIndexData == vmob->pIndexData)
				continue;
			mprog_wordlist_check(txt, vmob, actor, NULL, NULL, SPEECH_PROG);
		}
	}
	return;

}

/* Added by Thanos */
void mprog_tell_trigger(CHAR_DATA *ch, char *txt, CHAR_DATA *vict)
{
	if (char_died(vict) || char_died(ch))
		return;

	if (!vict || !IS_NPC(vict) || IS_AFFECTED(vict, AFF_CHARM) || vict->fighting)
		return;

	if (!ch || IS_NPC(ch) || ch->fighting)
		return;

	if (IS_SET(vict->pIndexData->progtypes, TELL_PROG))
	{
		mprog_wordlist_check(txt, vict, ch, NULL, NULL, TELL_PROG);
	}
	return;
}

/*
 *  Mudprogram additions begin here
 */
void set_supermob(OBJ_DATA *obj)
{
	ROOM_INDEX_DATA *room;
	OBJ_DATA *in_obj;
	char buf[200];
	int i;

	if (!supermob)
		init_supermob();
	/* nie tam �adne mob_index bo supermob ma byc NIEMATERIALNY !!!
	 supermob = create_mobile(get_mob_index( 3 ));
	 -- Thanos */
	if (!obj)
		return;

	for (in_obj = obj; in_obj->in_obj; in_obj = in_obj->in_obj)
		;

	if (in_obj->carried_by)
	{
		room = in_obj->carried_by->in_room;
	}
	else
	{
		room = obj->in_room;
	}

	if (!room)
		return;

	for (i = 0; i < 6; i++)
		STRDUP(supermob->przypadki[i], obj->przypadki[i]);

	/* Added by Jenny to allow bug messages to show the vnum
	 of the object, and not just supermob's vnum */
	sprintf(buf, "Object #%d", obj->pIndexData->vnum);
	STRDUP(supermob->description, buf);

	supermob->curr_room = NULL;
	supermob->curr_obj = obj;
	supermob->mpscriptrun = obj->mpscriptrun;
	supermob->inquest = obj->inquest;
	supermob->variables = obj->variables;
	if (room != NULL)
	{
		char_to_room(supermob, room);
	}
	return;
}

/*
 * NOSZ KUWA !!!
 * Supermob ma nie stac w pokoju jak nic nie robi !!!
 * (za�mieca tylko pokoje i w og�le - stwarza wiele niebezpiecze�stw)
 * lepiej zeby mial in_room = NULL
 *								-- Thanos
 */
void release_supermob()
{

	char_from_room(supermob);
	/*
	 char_to_room( supermob, get_room_index( 3 ) );
	 */
	supermob->curr_room = NULL;
	supermob->curr_obj = NULL;
	supermob->inquest = NULL;
	supermob->variables = NULL;
	return;
}

bool oprog_percent_check(CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type)
{
	bool executed = false;
	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;
	int value;

	for (auto* mprg : obj->pIndexData->mudprogs)
	{
		prog_nr++;
		if ((mprg->type & type) && (number_percent() <= (value = atoi(mprg->arglist))))
		{
			if (type != RAND_PROG || (type == RAND_PROG && value > 0))
			{
				executed = true;
				clear_local_variables(mob);
				current_prog_line = 0;
				current_prog_number = prog_nr;
				mprog_driver(mprg->comlist, mob, actor, obj, vo, false);
				if (type != GREET_PROG)
					break;
			}
		}
	}

	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;

	return executed;
}

/*
 * Triggers follow
 */

/*
 *  Hold on this
 *
 void oprog_act_trigger( CHAR_DATA *ch, OBJ_DATA *obj )
 {
 set_supermob( obj );
 if ( obj->pIndexData->progtypes & ACT_PROG )
 oprog_percent_check( supermob, ch, obj, NULL, ACT_PROG );

 release_supermob();
 return;
 }
 *
 *
 */

void oprog_greet_trigger(CHAR_DATA *ch)
{
	for (auto* vobj : ch->in_room->contents)
		if (vobj->pIndexData->progtypes & GREET_PROG)
		{
			set_supermob(vobj); /* not very efficient to do here */
			oprog_percent_check(supermob, ch, vobj, NULL, GREET_PROG);
			release_supermob();
		}

	return;
}

void oprog_speech_trigger(char *txt, CHAR_DATA *ch)
{
	/* supermob is set and released in oprog_wordlist_check */
	for (auto* vobj : ch->in_room->contents)
		if (vobj->pIndexData->progtypes & SPEECH_PROG)
		{
			oprog_wordlist_check(txt, supermob, ch, vobj, NULL, SPEECH_PROG, vobj);
		}

	return;
}

/*
 * Called at top of obj_update
 * make sure to put an if(!obj) continue
 * after it
 */
void oprog_random_trigger(OBJ_DATA *obj)
{
	if (!obj || !obj->pIndexData)
		return;

	if (obj->pIndexData->progtypes & RAND_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, NULL, obj, NULL, RAND_PROG);
		release_supermob();
	}
	return;
}

/*
 * in wear_obj, between each successful equip_char
 * the subsequent return
 */
void oprog_wear_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & WEAR_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, WEAR_PROG);
		release_supermob();
	}
	return;
}

bool oprog_use_trigger(CHAR_DATA *ch, OBJ_DATA *obj, CHAR_DATA *vict, OBJ_DATA *targ, void *vo)
{
	bool executed = false;

	if (obj->pIndexData->progtypes & USE_PROG)
	{
		set_supermob(obj);
		executed = oprog_percent_check(supermob, ch, obj, NULL, USE_PROG);
		release_supermob();
	}
	return executed;
}

/*
 * call in remove_obj, right after unequip_char
 * do a if(!ch) return right after, and return true (?)
 * if !ch
 */
void oprog_remove_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & REMOVE_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, REMOVE_PROG);
		release_supermob();
	}
	return;
}

/*
 * call in do_sac, right before extract_obj
 */
void oprog_sac_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & SAC_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, SAC_PROG);
		release_supermob();
	}
	return;
}

/*
 * call in do_get, right before check_for_trap
 * do a if(!ch) return right after
 */
void oprog_get_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & GET_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, GET_PROG);
		release_supermob();
	}
	return;
}

/*
 * called in damage_obj in act_obj.c
 */
void oprog_damage_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & DAMAGE_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, DAMAGE_PROG);
		release_supermob();
	}
	return;
}

/*
 * called in do_repair in shops.c
 */
void oprog_repair_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{

	if (obj->pIndexData->progtypes & REPAIR_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, REPAIR_PROG);
		release_supermob();
	}
	return;
}

/*
 * call twice in do_drop, right after the act( COL_ACTION,...)
 * do a if(!ch) return right after
 */
void oprog_drop_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & DROP_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, DROP_PROG);
		release_supermob();
	}
	return;
}

/*
 * call towards end of do_examine, right before check_for_trap
 */
void oprog_examine_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & EXA_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, EXA_PROG);
		release_supermob();
	}
	return;
}

/*
 * call in fight.c, group_gain, after (?) the obj_to_room
 */
void oprog_zap_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & ZAP_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, ZAP_PROG);
		release_supermob();
	}
	return;
}

/*
 * call in levers.c, towards top of do_push_or_pull
 *  see note there
 */
void oprog_pull_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & PULL_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, PULL_PROG);
		release_supermob();
	}
	return;
}

/*
 * call in levers.c, towards top of do_push_or_pull
 *  see note there
 */
void oprog_push_trigger(CHAR_DATA *ch, OBJ_DATA *obj)
{
	if (obj->pIndexData->progtypes & PUSH_PROG)
	{
		set_supermob(obj);
		oprog_percent_check(supermob, ch, obj, NULL, PUSH_PROG);
		release_supermob();
	}
	return;
}

void obj_act_add(OBJ_DATA *obj);
void oprog_act_trigger(char *buf, OBJ_DATA *mobj, CHAR_DATA *ch, OBJ_DATA *obj, void *vo)
{
	if (mobj->pIndexData->progtypes & ACT_PROG)
	{
		MPROG_ACT_LIST *tmp_act;

		CREATE(tmp_act, MPROG_ACT_LIST, 1);
		if (mobj->mpactnum > 0)
			tmp_act->next = mobj->mpact;
		else
			tmp_act->next = NULL;

		mobj->mpact = tmp_act;
		STRDUP(mobj->mpact->buf, buf);
		mobj->mpact->ch = ch;
		mobj->mpact->obj = obj;
		mobj->mpact->vo = vo;
		mobj->mpactnum++;
		obj_act_add(mobj);
	}
	return;
}

void oprog_wordlist_check(char *arg, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type, OBJ_DATA *iobj)
{

	char temp1[ MAX_STRING_LENGTH];
	char temp2[ MAX_INPUT_LENGTH];
	char word[ MAX_INPUT_LENGTH];
	char *list;
	char *start;
	char *dupl;
	char *end;
	int i;
	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	for (auto* mprg : iobj->pIndexData->mudprogs)
	{
		prog_nr++;
		if (mprg->type & type)
		{
			strcpy(temp1, mprg->arglist);
			list = temp1;
			for (i = 0; i < (int) strlen(list); i++)
				list[i] = LOWER(list[i]);
			strcpy(temp2, arg);
			dupl = temp2;
			for (i = 0; i < (int) strlen(dupl); i++)
				dupl[i] = LOWER(dupl[i]);
			if ((list[0] == 'p') && (list[1] == ' '))
			{
				list += 2;
				while ((start = str_str(list, dupl)))
					if ((start == dupl || *(start - 1) == ' ')
							&& (*(end = start + strlen(list)) == ' ' || *end == '\n' || *end == '\r' || *end == '\0'))
					{
						// Ratm - zmienne. Wszystko ponizej do mprog_driver()
						int j;
						char argbuf[MIL + 10]; // Ratm +10 na wszelki wypadek ;)
						char wordbuf[MIL];
						char *argstr = arg;
						char *abufstr = argbuf;

						set_supermob(iobj);
						clear_local_variables(supermob);
						supermob->variables->prog_args[0] = arg;

						for (j = 1; j < 10; j++)
						{
							int len;

							argstr = one_argument(argstr, wordbuf);
							if (*wordbuf == '\0')
								break;

							len = strlen(wordbuf);
							if (wordbuf[len - 1] == '\r')
								len--;
							if (wordbuf[len - 1] == '\n')
								len--;

							wordbuf[len] = '\0';
							strcpy(abufstr, wordbuf);
							supermob->variables->prog_args[j] = abufstr;
							abufstr += len + 1;
						}

						current_prog_line = 0;
						current_prog_number = prog_nr;
						mprog_driver(mprg->comlist, mob, actor, obj, vo, false);

						//Ratm Po wykonaniu proga czyscimy niepotrzebne zmienne
						clear_local_variables(supermob);
						if (supermob->variables)
							for (j = 0; j < 10; j++)
								supermob->variables->prog_args[j] = NULL;
						release_supermob();
						break;
					}
					else
						dupl = start + 1;
			}
			else
			{
				list = one_argument(list, word);
				for (; word[0] != '\0'; list = one_argument(list, word))
					while ((start = str_str(word, dupl)))
						if ((start == dupl || *(start - 1) == ' ')
								&& (*(end = start + strlen(word)) == ' ' || *end == '\n' || *end == '\r' || *end == '\0'))
						{
							// Ratm - zmienne. Wszystko ponizej do mprog_driver()
							int j;
							char argbuf[MIL + 10]; // Ratm +10 na wszelki wypadek ;)
							char wordbuf[MIL];
							char *argstr = arg;
							char *abufstr = argbuf;

							set_supermob(iobj);
							clear_local_variables(supermob);

							// bezpieczniki by Thanos
							if (supermob->variables)
								supermob->variables->prog_args[0] = arg;

							if (supermob->variables)
								for (j = 1; j < 10; j++)
								{
									int len;

									argstr = one_argument(argstr, wordbuf);
									if (*wordbuf == '\0')
										break;

									len = strlen(wordbuf);
									if (wordbuf[len - 1] == '\r')
										len--;
									if (wordbuf[len - 1] == '\n')
										len--;

									wordbuf[len] = '\0';
									strcpy(abufstr, wordbuf);
									supermob->variables->prog_args[j] = abufstr;
									abufstr += len + 1;
								}

							current_prog_line = 0;
							current_prog_number = prog_nr;
							mprog_driver(mprg->comlist, mob, actor, obj, vo, false);

							//Ratm Po wykonaniu proga czyscimy niepotrzebne zmienne
							clear_local_variables(supermob);
							if (supermob->variables)
								for (j = 0; j < 10; j++)
									supermob->variables->prog_args[j] = NULL;
							release_supermob();
							break;
						}
						else
							dupl = start + 1;
			}
		}
	}

	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;
}

/*
 *  room_prog support starts here
 *
 *
 */

void rset_supermob(ROOM_INDEX_DATA *room)
{
	char buf[200];
	int i;

	if (room)
	{
		for (i = 0; i < 6; i++)
			STRDUP(supermob->przypadki[i], room->name);
		STRDUP(supermob->name, room->name);

		/* Added by Jenny to allow bug messages to show the vnum
		 of the room, and not just supermob's vnum */
		sprintf(buf, "Room #%d", room->vnum);
		STRDUP(supermob->description, buf);
		supermob->position = POS_STANDING;
		supermob->curr_room = room;
		supermob->curr_obj = NULL;
		supermob->mpscriptrun = room->mpscriptrun;
		supermob->variables = room->variables;

		char_to_room(supermob, room);
	}
	return;
}

void rprog_percent_check(CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type)
{
	int prog_nr = 1;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;
	int value;

	if (!mob->in_room)
		return;

	for (auto* mprg : mob->in_room->mudprogs)
		if ((mprg->type & type) && (number_percent() <= (value = atoi(mprg->arglist))))
		{
			if (type != RAND_PROG || (type == RAND_PROG && value > 0))
			{
				current_prog_line = 0;
				current_prog_number = prog_nr;
				mprog_driver(mprg->comlist, mob, actor, obj, vo, false);
				if (type != ENTER_PROG)
					break;
			}
		}
	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
}

/*
 * Triggers follow
 */

/*
 *  Hold on this
 * Unhold. -- Alty
 */
void room_act_add(ROOM_INDEX_DATA *room);
void rprog_act_trigger(char *buf, ROOM_INDEX_DATA *room, CHAR_DATA *ch, OBJ_DATA *obj, void *vo)
{
	if (room->progtypes & ACT_PROG)
	{
		MPROG_ACT_LIST *tmp_act;

		CREATE(tmp_act, MPROG_ACT_LIST, 1);
		if (room->mpactnum > 0)
			tmp_act->next = room->mpact;
		else
			tmp_act->next = NULL;

		room->mpact = tmp_act;
		STRDUP(room->mpact->buf, buf);
		room->mpact->ch = ch;
		room->mpact->obj = obj;
		room->mpact->vo = vo;
		room->mpactnum++;
		room_act_add(room);
	}
}
/*
 *
 */

void rprog_leave_trigger(CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & LEAVE_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_percent_check(supermob, ch, NULL, NULL, LEAVE_PROG);
		release_supermob();
	}
}

void rprog_enter_trigger(CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & ENTER_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_percent_check(supermob, ch, NULL, NULL, ENTER_PROG);
		release_supermob();
	}
}

void rprog_sleep_trigger(CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & SLEEP_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_percent_check(supermob, ch, NULL, NULL, SLEEP_PROG);
		release_supermob();
	}
}

void rprog_rest_trigger(CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & REST_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_percent_check(supermob, ch, NULL, NULL, REST_PROG);
		release_supermob();
	}
}

void rprog_rfight_trigger(CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & RFIGHT_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_percent_check(supermob, ch, NULL, NULL, RFIGHT_PROG);
		release_supermob();
	}
}

void rprog_death_trigger(CHAR_DATA *killer, CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & RDEATH_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_percent_check(supermob, ch, NULL, NULL, RDEATH_PROG);
		release_supermob();
	}
}

void rprog_speech_trigger(char *txt, CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & SPEECH_PROG)
	{
		/* supermob is set and released in rprog_wordlist_check */
		rprog_wordlist_check(txt, supermob, ch, NULL, NULL, SPEECH_PROG, ch->in_room);
	}
}

void rprog_random_trigger(CHAR_DATA *ch)
{

	if (ch->in_room->progtypes & RAND_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_percent_check(supermob, ch, NULL, NULL, RAND_PROG);
		release_supermob();
	}
}

void rprog_extradesc_trigger(EXTRA_DESCR_DATA *ed, CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & EXTRADESC_PROG)
	{
		/* supermob is set and released in rprog_wordlist_check */
		SWString keyword(ed->keyword);
		const char *prefix = "p ";
		if (keyword.find(prefix))
		{
			keyword.prepend(prefix); /* Trog: cale slowo kluczowe musi pasowac, nie puszczamy jak pasuje tylko czesc */
		}
		rprog_wordlist_check((char*) keyword.c_str(), supermob, ch, NULL, NULL, EXTRADESC_PROG, ch->in_room);
	}
}

void rprog_wordlist_check(char *arg, CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int64 type, ROOM_INDEX_DATA *room)
{
	char temp1[MAX_STRING_LENGTH];
	char temp2[MAX_INPUT_LENGTH];
	char word[MAX_INPUT_LENGTH];
	char *list;
	char *start;
	char *dupl;
	char *end;
	int i;
	int prog_nr = 0;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	if (actor && !char_died(actor) && actor->in_room)
		room = actor->in_room;

	for (auto* mprg : room->mudprogs)
	{
		prog_nr++;
		if (mprg->type & type)
		{
			strcpy(temp1, mprg->arglist);
			list = temp1;
			for (i = 0; i < (int) strlen(list); i++)
				list[i] = LOWER(list[i]);
			strcpy(temp2, arg);
			dupl = temp2;
			for (i = 0; i < (int) strlen(dupl); i++)
				dupl[i] = LOWER(dupl[i]);
			if ((list[0] == 'p') && (list[1] == ' '))
			{
				list += 2;
				while ((start = str_str(list, dupl)))
					if ((start == dupl || *(start - 1) == ' ')
							&& (*(end = start + strlen(list)) == ' ' || *end == '\n' || *end == '\r' || *end == '\0'))
					{
						// Ratm - zmienne. Wszystko ponizej do mprog_driver()
						int j;
						char argbuf[MIL + 10]; // Ratm +10 na wszelki wypadek ;)
						char wordbuf[MIL];
						char *argstr = arg;
						char *abufstr = argbuf;

						rset_supermob(room);
						clear_local_variables(supermob);
						supermob->variables->prog_args[0] = arg;

						for (j = 1; j < 10; j++)
						{
							int len;

							argstr = one_argument(argstr, wordbuf);
							if (*wordbuf == '\0')
								break;

							len = strlen(wordbuf);
							if (wordbuf[len - 1] == '\r')
								len--;
							if (wordbuf[len - 1] == '\n')
								len--;

							wordbuf[len] = '\0';
							strcpy(abufstr, wordbuf);
							supermob->variables->prog_args[j] = abufstr;
							abufstr += len + 1;
						}

						current_prog_line = 0;
						current_prog_number = prog_nr;
						mprog_driver(mprg->comlist, mob, actor, obj, vo, false);

						//Ratm Po wykonaniu proga czyscimy niepotrzebne zmienne
						clear_local_variables(supermob);
						if (supermob->variables)
							for (j = 0; j < 10; j++)
								supermob->variables->prog_args[j] = NULL;
						release_supermob();
						break;
					}
					else
						dupl = start + 1;
			}
			else
			{
				list = one_argument(list, word);
				for (; word[0] != '\0'; list = one_argument(list, word))
					while ((start = str_str(word, dupl)))
						if ((start == dupl || *(start - 1) == ' ')
								&& (*(end = start + strlen(word)) == ' ' || *end == '\n' || *end == '\r' || *end == '\0'))
						{
							// Ratm - zmienne. Wszystko ponizej do mprog_driver()
							int j;
							char argbuf[MIL + 10]; // Ratm +10 na wszelki wypadek ;)
							char wordbuf[MIL];
							char *argstr = arg;
							char *abufstr = argbuf;

							rset_supermob(room);
							clear_local_variables(supermob);
							supermob->variables->prog_args[0] = arg;

							for (j = 1; j < 10; j++)
							{
								int len;

								argstr = one_argument(argstr, wordbuf);
								if (*wordbuf == '\0')
									break;

								len = strlen(wordbuf);
								if (wordbuf[len - 1] == '\r')
									len--;
								if (wordbuf[len - 1] == '\n')
									len--;

								wordbuf[len] = '\0';
								strcpy(abufstr, wordbuf);
								supermob->variables->prog_args[j] = abufstr;
								abufstr += len + 1;
							}

							current_prog_line = 0;
							current_prog_number = prog_nr;
							mprog_driver(mprg->comlist, mob, actor, obj, vo, false);

							//Ratm Po wykonaniu proga czyscimy niepotrzebne zmienne
							clear_local_variables(supermob);
							if (supermob->variables)
								for (j = 0; j < 10; j++)
									supermob->variables->prog_args[j] = NULL;

							release_supermob();
							break;
						}
						else
							dupl = start + 1;
			}
		}
	}
	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;
}

void rprog_time_check(CHAR_DATA *mob, CHAR_DATA *actor, OBJ_DATA *obj, void *vo, int type)
{
	ROOM_INDEX_DATA *room = (ROOM_INDEX_DATA*) vo;
	bool trigger_time;
	int prog_nr = 1;
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	for (auto* mprg : room->mudprogs)
	{
		if (room->area && room->area->planet)
			trigger_time = (room->area->planet->hour == atoi(mprg->arglist));
		else
			trigger_time = (!planet_list.empty() && planet_list.front()->hour == atoi(mprg->arglist));

		if (!trigger_time)
		{
			if (mprg->triggered)
				mprg->triggered = false;
			continue;
		}

		if ((mprg->type & type) && ((!mprg->triggered) || (mprg->type & HOUR_PROG)))
		{
			mprg->triggered = true;
			current_prog_line = 0;
			current_prog_number = prog_nr;
			mprog_driver(mprg->comlist, mob, actor, obj, vo, false);
		}
	}
	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;
}

void rprog_time_trigger(CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & TIME_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_time_check(supermob, NULL, NULL, ch->in_room, TIME_PROG);
		release_supermob();
	}
	return;
}

void rprog_hour_trigger(CHAR_DATA *ch)
{
	if (ch->in_room->progtypes & HOUR_PROG)
	{
		rset_supermob(ch->in_room);
		rprog_time_check(supermob, NULL, NULL, ch->in_room, HOUR_PROG);
		release_supermob();
	}
	return;
}

void prog_bug(const char *str, ...)
{
	char buf[MAX_STRING_LENGTH];
	FILE *fp;

	strcpy(buf, "PROGBUG: ");
	{
		va_list param;

		va_start(param, str);
		vsprintf(buf + strlen(buf), str, param);
		va_end(param);
	}

	to_channel(buf, CHANNEL_PROGBUG, "[****] " FB_YELLOW, 103);

	LOG_CLOSE;
	if ((fp = fopen( PROGBUG_FILE, "a")) != NULL)
	{
		fprintf(fp, "%s\n", buf);
		fclose(fp);
	}
	LOG_OPEN;
}

// Ratm do progbug() linia i nr wykonywanego proga
int current_prog_line;
int current_prog_number;

/* Written by Jenny, Nov 29/95
 prog line and number by Ratm */
void progbug(const char *str, CHAR_DATA *mob)
{
	char buf[MAX_STRING_LENGTH];

	/* Check if we're dealing with supermob, which means the bug occurred
	 in a room or obj prog. */
	if (mob == supermob)
	{
		/* It's supermob.  In set_supermob and rset_supermob, the description
		 was set to indicate the object or room, so we just need to show
		 the description in the bug message. */
		sprintf(buf, "%s, %s. Prog #%i, Line #%i.", str, mob->description == NULL ? "(unknown)" : mob->description, current_prog_number,
				current_prog_line);
	}
	else
	{
		sprintf(buf, "%s. Mob #%d, Room #%d, Prog #%i, Line #%i.", str, mob->pIndexData->vnum, mob->in_room->vnum, current_prog_number,
				current_prog_line);
	}

	prog_bug(buf, 0);
	return;
}

/* Room act prog updates.  Use a separate list cuz we dont really wanna go
 thru 5-10000 rooms every pulse.. can we say lag? -- Alty */

void room_act_add(ROOM_INDEX_DATA *room)
{
	for (auto* runner : room_act_list)
		if (runner->vo == room)
			return;
	struct act_prog_data *runner;
	CREATE(runner, struct act_prog_data, 1);
	runner->vo = room;
	room_act_list.push_front(runner);
}

void room_act_update(void)
{
	MPROG_ACT_LIST *mpact;

	while (!room_act_list.empty())
	{
		struct act_prog_data *runner = room_act_list.front();
		ROOM_INDEX_DATA *room = (RID*) runner->vo;

		while ((mpact = room->mpact) != NULL)
		{
			if (mpact->ch->in_room == room)
				rprog_wordlist_check(mpact->buf, supermob, mpact->ch, mpact->obj, mpact->vo, ACT_PROG, room);
			room->mpact = mpact->next;
			free_mpact(mpact);
		}
		room->mpact = NULL;
		room->mpactnum = 0;
		room_act_list.pop_front();
		DISPOSE(runner);
	}
	return;
}

void obj_act_add(OBJ_DATA *obj)
{
	for (auto* runner : obj_act_list)
		if (runner->vo == obj)
			return;
	struct act_prog_data *runner;
	CREATE(runner, struct act_prog_data, 1);
	runner->vo = obj;
	obj_act_list.push_front(runner);
}
void obj_act_update(void)
{
	MPROG_ACT_LIST *mpact;

	while (!obj_act_list.empty())
	{
		struct act_prog_data *runner = obj_act_list.front();
		OBJ_DATA *obj = (OD*) runner->vo;

		while ((mpact = obj->mpact) != NULL)
		{
			oprog_wordlist_check(mpact->buf, supermob, mpact->ch, mpact->obj, mpact->vo, ACT_PROG, obj);
			obj->mpact = mpact->next;
			free_mpact(mpact);
		}
		obj->mpact = NULL;
		obj->mpactnum = 0;
		obj_act_list.pop_front();
		DISPOSE(runner);
	}
	return;
}

void update_script(SCRIPT_DATA *pScript)
{
	int old_prog_line = current_prog_line;
	int old_prog_nr = current_prog_number;

	current_prog_line = pScript->curr_line;
	current_prog_number = pScript->prog_nr;

	curr_script = pScript;
	curr_script->active = true;

	/* Mob */
	if (pScript->m_owner && pScript->m_owner->mpscriptrun)
	{
		mprog_driver(pScript->command, pScript->m_owner, pScript->victim, pScript->object, pScript->vo, true);
	}
	/* Obj */
	else if (pScript->o_owner && pScript->o_owner->mpscriptrun)
	{
		OBJ_DATA *obj = pScript->o_owner;

		set_supermob(obj);
		mprog_driver(pScript->command, supermob, pScript->victim, pScript->object, pScript->vo, true);
		obj->mpscriptrun = supermob->mpscriptrun;
		release_supermob();
	}
	/* Room */
	else if (pScript->r_owner && pScript->r_owner->mpscriptrun)
	{
		ROOM_INDEX_DATA *room = pScript->r_owner;

		rset_supermob(room);
		mprog_driver(pScript->command, supermob, pScript->victim, pScript->object, pScript->vo, true);
		room->mpscriptrun = supermob->mpscriptrun;
		release_supermob();
	}
	else
	{
		bug("Unknown owner of script_prog detected");
		stopscript(pScript);
	}

	if (curr_script)
		curr_script->active = false;

	curr_script = NULL;
	current_prog_line = old_prog_line;
	current_prog_number = old_prog_nr;
	return;
}

void scripts_update(void)
{
	auto scripts_snapshot = script_prog_list;
	for (auto* pScript : scripts_snapshot)
	{
		if (pScript->position > (int) strlen(pScript->command))
			stopscript(pScript);
		else
			update_script(pScript);
	}
	return;
}

