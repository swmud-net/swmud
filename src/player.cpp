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
 * 		Commands for personal player settings/statictics	   *
 ****************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include "mud.h"
#include <SWTimeStamp.h>

/*
 *  Locals
 */
const char* tiny_affect_loc_name(int location);

DEF_DO_FUN( gold )
{
	CHAR_DATA *vch;

	if (*argument && !IS_NPC(ch) && IS_ADMIN(ch->name))
	{
		if ((vch = get_char_world(ch, argument)) == NULL)
		{
			send_to_char("Nie ma nikogo takiego" NL, ch);
			return;
		}
	}
	else
		vch = ch;

	ch_printf(ch, PLAIN FG_RED
	"Masz przy sobie " FB_RED "%d" FG_RED " kredyt%s." EOL, vch->gold, NUMBER_SUFF(vch->gold, "k�", "ki", "ek"));

	if (vch->pcdata && vch->pcdata->bank)
		ch_printf(ch, PLAIN FG_RED
		"Na twoim koncie jest obecnie " FB_RED "%d" FG_RED " kredyt%s." EOL, vch->pcdata->bank,
				NUMBER_SUFF(vch->pcdata->bank, "k�", "ki", "ek"));

	return;
}

char* ac_info(CHAR_DATA *ch)
{
	static char b[MSL];

	GET_AC(ch) > 50 ? sprintf(b, "GORZEJ ni� nag%s!", SEX_SUFFIX_IAE(ch)) :
	GET_AC(ch) >= 40 ? sprintf(b, "Nag%s.", SEX_SUFFIX_IAE(ch)) :
	GET_AC(ch) >= 30 ? sprintf(b, "Ubran%s.", SEX_SUFFIX_YAE(ch)) :
	GET_AC(ch) >= 20 ? sprintf(b, "Nieco uzbrojon%s.", SEX_SUFFIX_YAE(ch)) :
	GET_AC(ch) >= 10 ? sprintf(b, "Troch� uzbrojon%s.", SEX_SUFFIX_YAE(ch)) :
	GET_AC(ch) >= 0 ? sprintf(b, "Uzbrojon%s.", SEX_SUFFIX_YAE(ch)) :
	GET_AC(ch) >= -40 ? sprintf(b, "Dobrze uzbrojon%s.", SEX_SUFFIX_YAE(ch)) :
	GET_AC(ch) >= -80 ? sprintf(b, "Mocno uzbrojon%s.", SEX_SUFFIX_YAE(ch)) :
	GET_AC(ch) >= -120 ? sprintf(b, "Ci�ko uzbrojon%s.", SEX_SUFFIX_YAE(ch)) :
	GET_AC(ch) >= -160 ? sprintf(b, "Niesamowicie uzbrojon%s.", SEX_SUFFIX_YAE(ch)) :
	GET_AC(ch) >= -200 ? sprintf(b, "Nie do zdarcia!") : sprintf(b, "NiEzNiSzCzAlN%s!", SEX_SUFFIX_YAE(ch));

	return b;
}

char* align_info(CHAR_DATA *ch)
{
	/*
	 static char	b	[MSL];
	 sprintf( b,
	 ch->alignment >  900 ? "MISTRZA YODY!"		:
	 ch->alignment >  700 ? "Dzieci Brzasku."		:
	 ch->alignment >  350 ? "Dobry." 			:
	 ch->alignment >  100 ? "Mi�y." 			:
	 ch->alignment > -100 ? "Neutralny."			:
	 ch->alignment > -350 ? "Handlarza niewolnik�w." 	:
	 ch->alignment > -700 ? "Z�y." 			:
	 ch->alignment > -900 ? "Imperatora." 		:
	 "SAMEGO EXAR KHUNA!" );
	 return b;
	 */
	return (char*) (ch->alignment > 900 ? "Mistrza Jedi!" : ch->alignment > 700 ? "Rycerza Jedi." :
					ch->alignment > 350 ? "Senatora Republiki." : ch->alignment > 100 ? "Rebelianta." :
					ch->alignment > -100 ? "Neutralny." : ch->alignment > -350 ? "Szturmowca Imperium." :
					ch->alignment > -700 ? "Moffa Imperium." : ch->alignment > -900 ? "Mrocznego Jedi." : "Lorda Sith!");
}

char* mental_info(CHAR_DATA *ch)
{
	static char b[MSL];

	switch (ch->mental_state / 10)
	{
	default:
		sprintf(b, "Kompletnie zmieszan%s.", SEX_SUFFIX_YAE(ch));
		break;
	case -10:
		sprintf(b, "Ledwie przytomn%s.", SEX_SUFFIX_YAE(ch));
		break;
	case -9:
		sprintf(b, "Ledwie otwierasz oczy.");
		break;
	case -8:
		sprintf(b, "Bardzo senn%s.", SEX_SUFFIX_YAE(ch));
		break;
	case -7:
		sprintf(b, "Zrezygnowan%s.", SEX_SUFFIX_YAE(ch));
		break;
	case -6:
		sprintf(b, "Wyciszon%s.", SEX_SUFFIX_YAE(ch));
		break;
	case -5:
		sprintf(b, "�pi�c%s.", SEX_SUFFIX_YAE(ch));
		break;
	case -4:
		sprintf(b, "Zm�czon%s.", SEX_SUFFIX_YAE(ch));
		break;
	case -3:
		sprintf(b, "Przyda�aby si� drzemka.");
		break;
	case -2:
		sprintf(b, "Lekko przygn�bion%s.", SEX_SUFFIX_YAE(ch));
		break;
	case -1:
		sprintf(b, "W porz�dku.");
		break;
	case 0:
		sprintf(b, "Wspania�y.");
		break;
	case 1:
		sprintf(b, "Podekscytowan%s.", SEX_SUFFIX_YAE(ch));
		break;
	case 2:
		sprintf(b, "Lekko si� podgrza�.");
		break;
	case 3:
		sprintf(b, "Nie mo�esz zebra� prostych my�li.");
		break;
	case 4:
		sprintf(b, "Umys� p�dz�cy 100 mil na godzin�.");
		break;
	case 5:
		sprintf(b, "Latasz w przestworzach.");
		break;
	case 6:
		sprintf(b, "Tw�j umys� opuszcza twoje cia�o.");
		break;
	case 7:
		sprintf(b, "Rzeczywisto�� przestaje by� wa�na.");
		break;
	case 8:
		sprintf(b, "Nie masz poj�cia co jest prawdziwe, a co nie.");
		break;
	case 9:
		sprintf(b, "Czujesz si� jak m�ody B�g.");
		break;
	case 10:
		sprintf(b, "Jeste� CeNtRuM WsZeCh�wIaTa!.");
		break;
	}

	return b;
}

/* Trog */
bool check_clone(CHAR_DATA *ch)
{
	struct stat fst;
	char file[256];

	if (IS_NPC(ch))
		return false;
	sprintf(file, "%s%s", CLONE_DIR, capitalize(ch->name));
	if (stat(file, &fst) == -1)
		return false;
	return true;
}

void do_clonescore(CHAR_DATA *ch, char *argument)	//Trog
{
	struct stat fst;
	char buf[MSL];
	int defectprcnt = 80;

	sprintf(buf, "%s%s", CLONE_DIR, capitalize(ch->name));
	if (stat(buf, &fst) == -1)
	{
		send_to_char("Informacja o klonie niedost�pna." NL, ch);
		return;
	}

	/* Trog: tymczasowo */
	if (fst.st_mtime < (time_t) 1194181529)
		defectprcnt = 100;

	if (IS_IMMORTAL(ch))
		defectprcnt = 100;
	else
		defectprcnt -= (fst.st_mtime % 10);

	sprintf(buf, "Kamino.%c%c-%d", ch->name[0], ch->name[strlen(ch->name) - 1], (int) fst.st_mtime);
	ch_printf(ch, COL_SCORE "*-----Informacja-o-klonie-----*" NL
	"| Id: %23s |" NL
	"| Modyfikacje genetyczne:  0%% |" NL
	"| Strata w�a�ciwo�ci:      0%% |" NL
	"| Zgodno�� z orygina�em: %3d%% |" NL
	"| Data: %22s|" NL
	"*-----------------------------*" NL RESET, buf, defectprcnt, static_cast<SWString>(SWTimeStamp(fst.st_mtime)).c_str());
	return;
}

DEF_DO_FUN( oldscore )
{
	AFFECT_DATA *paf;
	KNOWN_LANG *klang;
	int drug;

	if (IS_NPC(ch))
	{
		do_score(ch, argument);
		return;
	}

	ch_printf(ch, "\n\r&bKarta ID: %s %s.&w\n\r" COL_SCORE, ch->przypadki[0], ch->pcdata->title);
	if (ch->attribute1 >= 0 || ch->attribute2 >= 0)
	{
		const SWString &attrib = format_char_attribute(ch, 0);
		ch_printf(ch, "\n\rJeste� postrzegan%s jako : %s" EOL, ch->sex == SEX_MALE ? "y" : "a", attrib.c_str());
	}

	if (get_trust(ch) != ch->top_level)
		ch_printf(ch, "Jeste� zaufany na poziom %d.\n\r", get_trust(ch));

	send_to_char("----------------------------------------------------------------------------\n\r", ch);
	ch_printf(ch, "Zapisywa�e�:  %s" NL, ch->save_time ? static_cast<SWString>(SWTimeStamp(ch->save_time)).c_str() : "nie." NL);

	ch_printf(ch, "Godzina:   %s" NL, static_cast<SWString>(SWTimeStamp(current_time)).c_str());
	send_to_char("----------------------------------------------------------------------------\n\r", ch);

	{
		int ability;

		for (ability = 0; ability < MAX_ABILITY; ability++)
			if (ability != FORCE_ABILITY || ch->skill_level[FORCE_ABILITY] > 1)
				ch_printf(ch, "%-15s  Poziom: %-3d   Max: %-3d Exp: %-10ld Nast�pny: %-10ld\n\r", class_table[ability].przypadki[0],
						ch->skill_level[ability], max_level(ch, ability), ch->experience[ability], exp_level(ch->skill_level[ability] + 1));
			else
				ch_printf(ch, "%-15s Poziom: %-3d   Max: ???   Exp: ???  Nast�pny: ???\n\r", class_table[ability].przypadki[0],
						ch->skill_level[ability], ch->experience[ability]);
	}

	send_to_char("----------------------------------------------------------------------------\n\r", ch);

	ch_printf(ch, "Kredytki: %-10d  Konto: %-10d    Pkills: %-5.5d   Mkills: %-5.5d\n\r", ch->gold, ch->pcdata->bank, ch->pcdata->pkills,
			ch->pcdata->mkills);

	ch_printf(ch, "Ekwipunek: %5.5d (max %7.7d)   Przedmiot�w: %5.5d (max %5.5d)\n\r", ch->carry_weight, can_carry_w(ch), ch->carry_number,
			can_carry_n(ch));

	if (!IS_NPC(ch) && ch->pcdata->condition[COND_DRUNK] > 10)
		send_to_char("Jeste� pijany.\n\r", ch);
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_THIRST] == 0)
		send_to_char("Grozi ci odwodnienie.\n\r", ch);
	if (!IS_NPC(ch) && ch->pcdata->condition[COND_FULL] == 0)
		send_to_char("Umierasz z g�odu.\n\r", ch);
	if (ch->position != POS_SLEEPING)
		switch (ch->mental_state / 10)
		{
		default:
			send_to_char("You're completely messed up!\n\r", ch);
			break;
		case -10:
			send_to_char("You're barely conscious.\n\r", ch);
			break;
		case -9:
			send_to_char("You can barely keep your eyes open.\n\r", ch);
			break;
		case -8:
			send_to_char("You're extremely drowsy.\n\r", ch);
			break;
		case -7:
			send_to_char("You feel very unmotivated.\n\r", ch);
			break;
		case -6:
			send_to_char("You feel sedated.\n\r", ch);
			break;
		case -5:
			send_to_char("You feel sleepy.\n\r", ch);
			break;
		case -4:
			send_to_char("You feel tired.\n\r", ch);
			break;
		case -3:
			send_to_char("You could use a rest.\n\r", ch);
			break;
		case -2:
			send_to_char("You feel a little under the weather.\n\r", ch);
			break;
		case -1:
			send_to_char("You feel fine.\n\r", ch);
			break;
		case 0:
			send_to_char("You feel great.\n\r", ch);
			break;
		case 1:
			send_to_char("You feel energetic.\n\r", ch);
			break;
		case 2:
			send_to_char("Your mind is racing.\n\r", ch);
			break;
		case 3:
			send_to_char("You can't think straight.\n\r", ch);
			break;
		case 4:
			send_to_char("Your mind is going 100 miles an hour.\n\r", ch);
			break;
		case 5:
			send_to_char("You're high as a kite.\n\r", ch);
			break;
		case 6:
			send_to_char("Your mind and body are slipping apart.\n\r", ch);
			break;
		case 7:
			send_to_char("Reality is slipping away.\n\r", ch);
			break;
		case 8:
			send_to_char("You have no idea what is real, and what is not.\n\r", ch);
			break;
		case 9:
			send_to_char("You feel immortal.\n\r", ch);
			break;
		case 10:
			send_to_char("You are a Supreme Entity.\n\r", ch);
			break;
		}
	else if (ch->mental_state > 45)
		send_to_char("Your sleep is filled with strange and vivid dreams.\n\r", ch);
	else if (ch->mental_state > 25)
		send_to_char("Your sleep is uneasy.\n\r", ch);
	else if (ch->mental_state < -35)
		send_to_char("You are deep in a much needed sleep.\n\r", ch);
	else if (ch->mental_state < -25)
		send_to_char("You are in deep slumber.\n\r", ch);
	send_to_char("SPICE Level/Addiction: ", ch);
	for (drug = 0; drug <= 9; drug++)
		if (ch->pcdata->drug_level[drug] > 0 || ch->pcdata->drug_level[drug] > 0)
		{
			ch_printf(ch, "%s(%d/%d) ", spice_types_list[drug], ch->pcdata->drug_level[drug], ch->pcdata->addiction[drug]);
		}
	send_to_char(NL "Znasz j�zyki:", ch);
	for (auto* klang : ch->klangs)
	{
		if (klang->language == ch->speaking)
		{
			send_to_char( FB_RED, ch);
			ch_printf(ch, " %s", klang->language->name);
			send_to_char( FG_CYAN, ch);
		}
		else
			ch_printf(ch, " %s", klang->language->name);
	}

	send_to_char( NL, ch);

	if (ch->pcdata->bestowments && ch->pcdata->bestowments[0] != '\0')
		ch_printf(ch, "Jeste� obdarzony komendami: %s.\n\r", ch->pcdata->bestowments);

	if (ch->pcdata->clan)
	{
		send_to_char("----------------------------------------------------------------------------\n\r", ch);
		ch_printf(ch, "ORGANIZACJA: %-35s Pkills/Deaths: %3.3d/%3.3d", ch->pcdata->clan->name, ch->pcdata->clan->pkills,
				ch->pcdata->clan->pdeaths);
		send_to_char("\n\r", ch);
	}
	if (IS_IMMORTAL(ch))
	{
		send_to_char("----------------------------------------------------------------------------\n\r", ch);

		ch_printf(ch, "IMMORTAL DATA:  Wizinvis [%s]  Wizlevel (%d)\n\r",
		IS_SET(ch->act, PLR_WIZINVIS) ? "X" : " ", ch->pcdata->wizinvis);

		ch_printf(ch, "Bamfin:  %s\n\r", (ch->pcdata->bamfin[0] != '\0') ? ch->pcdata->bamfin : "%s appears in a swirling mist.", ch->name);
		ch_printf(ch, "Bamfout: %s\n\r", (ch->pcdata->bamfout[0] != '\0') ? ch->pcdata->bamfout : "%s leaves in a swirling mist.",
				ch->name);
	}
	if (!ch->affects.empty())
	{
		int i;
		SKILLTYPE *sktmp;

		i = 0;
		send_to_char("----------------------------------------------------------------------------\n\r", ch);
		send_to_char("AFFECT DATA:                            ", ch);
		for (auto* paf : ch->affects)
		{
			if ((sktmp = get_skilltype(paf->type)) == NULL)
				continue;
			if (ch->top_level < 20)
			{
				ch_printf(ch, "[%-34.34s]    ", sktmp->name);
				if (i == 0)
					i = 2;
				if ((++i % 3) == 0)
					send_to_char("\n\r", ch);
			}
			else
			{
				if (paf->modifier == 0)
					ch_printf(ch, "[%-24.24s;%5d rds]    ", sktmp->name, paf->duration);
				else if (paf->modifier > 999)
					ch_printf(ch, "[%-15.15s; %7.7s;%5d rds]    ", sktmp->name, tiny_affect_loc_name(paf->location), paf->duration);
				else
					ch_printf(ch, "[%-11.11s;%+-3.3d %7.7s;%5d rds]    ", sktmp->name, paf->modifier, tiny_affect_loc_name(paf->location),
							paf->duration);
				if (i == 0)
					i = 1;
				if ((++i % 2) == 0)
					send_to_char("\n\r", ch);
			}
		}
	}
	send_to_char("\n\r", ch);
	return;
}

/*
 * Return ascii name of an affect location.
 */
const char* tiny_affect_loc_name(int location)
{
	switch (location)
	{
	case APPLY_NONE:
		return "NIL";
	case APPLY_STR:
		return " STR  ";
	case APPLY_DEX:
		return " DEX  ";
	case APPLY_INT:
		return " INT  ";
	case APPLY_WIS:
		return " WIS  ";
	case APPLY_CON:
		return " CON  ";
	case APPLY_CHA:
		return " CHA  ";
	case APPLY_LCK:
		return " LCK  ";
	case APPLY_SEX:
		return " SEX  ";
	case APPLY_LEVEL:
		return " LVL  ";
	case APPLY_MANA:
		return " MANA ";
	case APPLY_HIT:
		return " HV   ";
	case APPLY_MOVE:
		return " MOVE ";
	case APPLY_AC:
		return " AC   ";
	case APPLY_HITROLL:
		return " HITRL";
	case APPLY_DAMROLL:
		return " DAMRL";
	case APPLY_SAVING_POISON:
		return "SV POI";
	case APPLY_SAVING_ROD:
		return "SV ROD";
	case APPLY_SAVING_PARA:
		return "SV PARA";
	case APPLY_SAVING_BREATH:
		return "SV BRTH";
	case APPLY_SAVING_SPELL:
		return "SV SPLL";
	case APPLY_HEIGHT:
		return "HEIGHT";
	case APPLY_WEIGHT:
		return "WEIGHT";
	case APPLY_AFFECT:
		return "AFF BY";
	case APPLY_RESISTANT:
		return "RESIST";
	case APPLY_IMMUNE:
		return "IMMUNE";
	case APPLY_SUSCEPTIBLE:
		return "SUSCEPT";
	case APPLY_WEAPONSPELL:
		return " WEAPON";
	case APPLY_BACKSTAB:
		return "BACKSTB";
	case APPLY_PICK:
		return " PICK  ";
	case APPLY_TRACK:
		return " TRACK ";
	case APPLY_STEAL:
		return " STEAL ";
	case APPLY_SNEAK:
		return " SNEAK ";
	case APPLY_HIDE:
		return " HIDE  ";
	case APPLY_SNIPE:
		return " SNIPE ";
	case APPLY_DETRAP:
		return " DETRAP";
	case APPLY_DODGE:
		return " DODGE ";
	case APPLY_PEEK:
		return " PEEK  ";
	case APPLY_SCAN:
		return " SCAN  ";
	case APPLY_GOUGE:
		return " GOUGE ";
	case APPLY_SEARCH:
		return " SEARCH";
	case APPLY_MOUNT:
		return " MOUNT ";
	case APPLY_DISARM:
		return " DISARM";
	case APPLY_KICK:
		return " KICK  ";
	case APPLY_PARRY:
		return " PARRY ";
	case APPLY_BASH:
		return " BASH  ";
	case APPLY_STUN:
		return " STUN  ";
	case APPLY_PUNCH:
		return " PUNCH ";
	case APPLY_CLIMB:
		return " CLIMB ";
	case APPLY_GRIP:
		return " GRIP  ";
	case APPLY_SCRIBE:
		return " SCRIBE";
	case APPLY_BREW:
		return " BREW  ";
	case APPLY_WEARSPELL:
		return " WEAR  ";
	case APPLY_REMOVESPELL:
		return " REMOVE";
	case APPLY_EMOTION:
		return "EMOTION";
	case APPLY_MENTALSTATE:
		return " MENTAL";
	case APPLY_STRIPSN:
		return " DISPEL";
	case APPLY_REMOVE:
		return " REMOVE";
	case APPLY_DIG:
		return " DIG   ";
	case APPLY_FULL:
		return " HUNGER";
	case APPLY_THIRST:
		return " THIRST";
	case APPLY_DRUNK:
		return " DRUNK ";
	case APPLY_BLOOD:
		return " BLOOD ";
	}

	bug("unknown location %d.", location);
	return "(?.?)";
}

/*DEF_DO_FUN( thanosscore )
 {
 char        titbuf[MSL];
 char        langbuf[MSL];
 int		lang;
 char	b	[MAX_ABILITY][MIL];
 int		i;


 if ( IS_AFFECTED(ch, AFF_POSSESS) )
 {
 send_to_char("W twoim stanie umys�u to niemo�liwe niestety." NL, ch);
 return;
 }

 sprintf( titbuf, "%s %s", ch->name, !IS_NPC( ch ) ? ch->pcdata->title : "Cudowny mob."  );

 for ( lang = 0; lang_array[lang] != LANG_UNKNOWN; lang++ )
 {
 if ( knows_language( ch, lang_array[lang], ch )
 ||  (IS_NPC(ch) && ch->speaks == 0) )
 {
 if ( lang_array[lang] & ch->speaking
 ||  (IS_NPC(ch) && !ch->speaking) )
 {
 sprintf(langbuf, "%s", lang_names_pl[lang] );
 break;
 }
 }
 }

 ch_printf(ch, COL_SCORE
 "         _____________________________________________________          "	EOL COL_SCORE
 "        |   Obywatel%s: %s"  COL_SCORE                      " \\        "	EOL COL_SCORE
 "  .----:|   Status:   %3d letni %-15s "         "(%4d godz. gry)|:----. "	EOL COL_SCORE
 " |      |   J�zyk:    %-40s "                                 " |      |"	EOL,
 FEMALE( ch ) ? "ka" : "",
 IS_NPC(ch) ?  (char *)strip_colors( ch->przypadki[0], 40 ) :
 (char *)strip_colors( titbuf, 40 ),
 get_age(ch),
 ( IS_NPC( ch ) && IS_SET( ch->act, ACT_DROID ) ) ? "Droid" :
 CH_RACE( ch, 0),                  // Pixel: przypadkiF
 ( (int)SEC_PLAYED( ch )/3600 ),
 langbuf );

 b[1][0] = '\0';
 b[2][0] = '\0';
 if ( get_trust( ch ) != ch->top_level )
 sprintf( b[1], "Zaufanie na poziom: %3d", get_trust(ch) );

 if ( IS_IMMORTAL( ch ) || IS_OLCMAN( ch ) )
 sprintf( b[2], "Security:  %2d", ch->pcdata ? ch->pcdata->security : 0 );

 if( b[1][0] != '\0' || b[2][0] != '\0' )
 ch_printf(ch, COL_SCORE
 " |      |   %23s            %15s  |      |" EOL,
 b[1][0] != '\0' ? b[1] : " ", b[2][0] != '\0' ? b[2] : " "  );

 ch_printf(ch, COL_SCORE
 " |       \"-----------------------------------------------------\"       |" EOL );


 if ( ch->pcdata && ch->pcdata->clan )
 {
 ch_printf(ch, COL_SCORE
 " |  Organizacja:     %-20s Funkcja: %-15s     |" EOL,
 ch->pcdata->clan->pl_name,
 IS_LEADER( ch )   ? "Lider" 		:
 IS_DIPLOMAT( ch ) ? "Dyplomata"		:
 IS_FIRST( ch )	  ? "Pierwszy Mo�ny"	:
 IS_SECOND( ch )	  ? "Drugi Mo�ny"	: "Cz�onek" );
 }


 ch_printf(ch, COL_SCORE
 " |  Hp: %6d/%-6d "   " Mv: %6d/%-6d "      " Frc: %6d/%-6d           |"	EOL COL_SCORE
 " |  Ucieczka z walki:  %-6d hp.                        ""              |"	EOL COL_SCORE
 " |  Praktyki:          %-6d Punkty questowe: %4d   " "                 |"	EOL,
 ch->hit, ch->max_hit, ch->move, ch->max_move, ch->mana, ch->max_mana, ch->wimpy,
 IS_NPC( ch ) ? 0 : ch->pcdata->practices, IS_NPC( ch ) ? 0 : ch->pcdata->quest_points );


 for( i=0; i<MAX_ABILITY; i++ )
 sprintf( b[i], show_exp_stars( ch, i ));
 ch_printf(ch, COL_SCORE
 " |  Si�a:           %3d/%-3d   %-15s  %3d/%-3d   %s  |"	EOL COL_SCORE
 " |  M�dro��:        %3d/%-3d   %-15s  %3d/%-3d   %s  |"	EOL COL_SCORE
 " |  Inteligencja:   %3d/%-3d   %-15s  %3d/%-3d   %s  |"	EOL COL_SCORE
 " |  Kondycja:       %3d/%-3d   %-15s  %3d/%-3d   %s  |"	EOL COL_SCORE
 " |  Zr�czno��:      %3d/%-3d   %-15s  %3d/%-3d   %s  |"	EOL COL_SCORE
 " |  Charyzma:       %3d/%-3d   %-15s  %3d/%-3d   %s  |"	EOL COL_SCORE
 " |  Scz�cie:       %3d/%-3d   %-15s  %3d/%-3d   %s  |"	EOL,
 get_curr_str(ch), get_max_str(ch), class_table[0].przypadki[0], ch->skill_level[0],max_level( ch, 0 ),b[0],
 get_curr_wis(ch), get_max_wis(ch), class_table[1].przypadki[0],	ch->skill_level[1],max_level( ch, 1 ),b[1],
 get_curr_int(ch), get_max_int(ch), class_table[2].przypadki[0],	ch->skill_level[2],max_level( ch, 2 ),b[2],
 get_curr_con(ch), get_max_con(ch), class_table[3].przypadki[0],	ch->skill_level[3],max_level( ch, 3 ),b[3],
 get_curr_dex(ch), get_max_dex(ch), class_table[4].przypadki[0],	ch->skill_level[4],max_level( ch, 4 ),b[4],
 get_curr_cha(ch), get_max_cha(ch), class_table[5].przypadki[0],	ch->skill_level[5],max_level( ch, 5 ),b[5],
 get_curr_lck(ch), get_max_lck(ch), class_table[6].przypadki[0],	ch->skill_level[6],max_level( ch, 6 ),b[6] );

 if ( ch->top_level >= 15 )
 ch_printf( ch, COL_SCORE
 " |  Hitroll:           %-4d   Damroll:         %-4d                    |"	EOL,
 GET_HITROLL(ch), GET_DAMROLL(ch) );

 sprintf( b[1], "(%5d)", ch->alignment );
 ch_printf( ch, COL_SCORE
 " |  Charakter:       %-22s         %7s            |"	EOL,
 align_info(ch),  ch->top_level >= 10 ? b[1] : " " 	);

 sprintf( b[1], "(%5d)", GET_AC(ch) );
 ch_printf( ch, COL_SCORE
 " |  Uzbrojenie:      %-23s        %7s            |"	EOL,
 ac_info(ch), ch->top_level >= 25 ? b[1] : " "	);

 ch_printf(ch, COL_SCORE
 " |  Stan umys�u:     %-45s     |" EOL, mental_info(ch) );

 ch_printf( ch, COL_SCORE
 " |  Ekwipunek:       %7d/%-7d  (%8d/%-8dkg)            |"	EOL,
 ch->carry_number, can_carry_n( ch ),
 ch->carry_weight, can_carry_w( ch ) );

 ch_printf( ch, COL_SCORE
 " |_____________________________________________________________________|"	EOL );

 }*/

DEF_DO_FUN( score )
{
	int ability;
	char buf[MAX_STRING_LENGTH];
	char titbuf[MSL];

	if (IS_AFFECTED(ch, AFF_POSSESS))
	{
		send_to_char("W twoim stanie umys�u to niemo�liwe niestety." NL, ch);
		return;
	}
	sprintf(titbuf, "%s %s", ch->name, !IS_NPC(ch) ? ch->pcdata->title : "Cudowny mob.");

	ch_printf(ch, COL_SCORE
	" _____________________________________________________" NL
	"| Imi�:  %s" COL_SCORE " )___________ " NL
	"| Status: %3d letn%s %-15s (%4d godz. gry)/         " "   |" NL,
	IS_NPC(ch) ? (char*) strip_colors(ch->przypadki[0], 44) : (char*) strip_colors(titbuf, 44), get_age(ch),
			ch->sex == SEX_MALE ? "i " : "ia", ( IS_NPC( ch ) && IS_SET(ch->act, ACT_DROID)) ? "Droid" : CH_RACE(ch, 0), // Pixel: przypadkiF
			((int) SEC_PLAYED(ch) / 3600));
	ch_printf(ch, "| U�ywany j�zyk: %-30s     /             |" NL, ch->speaking->name);

	ch_printf(ch, "`--------------------------------------------------'              |" NL);
	ch_printf(ch, " |   Hp:          %6d/%-6d    Ucieczka z walki: %-6d       |" NL, ch->hit, ch->max_hit, ch->wimpy);
	if (ch->skill_level[FORCE_ABILITY] > 1 || IS_IMMORTAL(ch))
		ch_printf(ch, " |   Mv:          %6d/%-6d    Frc:       %6d/%-6d       |" NL, ch->move, ch->max_move, ch->mana, ch->max_mana);
	else
		ch_printf(ch, " |   Mv:          %6d/%-6d                                   |" NL, ch->move, ch->max_move);

	/*    ch_printf(ch, " |                                 Praktyki:         %-6d       |" NL,
	 ch->practice );*/
	ch_printf(ch, " |   Punkty questowe:  %-6d      Charyzma:         %-6d       |" NL, !IS_NPC(ch) ? ch->pcdata->quest_points : 0,
			get_curr_cha(ch));
	for (ability = 0; ability < MAX_ABILITY; ability++)
	{
		if (ability != FORCE_ABILITY || ch->skill_level[FORCE_ABILITY] > 1)
			ch_printf(ch, " |   %-15s %3d/%-3d ", class_table[ability].przypadki[0], ch->skill_level[ability], max_level(ch, ability));
		else
			ch_printf(ch, " |                           ");

		switch (ability)
		{
		default:
			ch_printf(ch, "      Charakter %s|" NL, (char*) strip_colors(align_info(ch), 21));
			break;
		case 0:
			ch_printf(ch, "      Si�a:             %-2d           |" NL, get_curr_str(ch));
			break;
		case 1:
			ch_printf(ch, "      M�dro��:          %-2d           |" NL, get_curr_wis(ch));
			break;
		case 2:
			ch_printf(ch, "      Inteligencja:     %-2d           |" NL, get_curr_int(ch));
			break;
		case 3:
			ch_printf(ch, "      Zr�czno��:        %-2d           |" NL, get_curr_dex(ch));
			break;
		case 4:
			ch_printf(ch, "      Kondycja:         %-2d           |" NL, get_curr_con(ch));
			break;
		case 5:
			IS_IMMORTAL( ch ) ?
					ch_printf(ch, "      Szcz�cie:        %-2d           |" NL, get_curr_lck(ch)) :
					ch_printf(ch, "      Szcz�cie:        ??           |" NL);
			break;
		case 6:
			(IS_IMMORTAL( ch ) || IS_NPC(ch)) ?
					ch_printf(ch, "      Moc:              %-2d           |" NL, get_curr_frc(ch)) :
					ch_printf(ch, "      Moc:              ??           |" NL);
			break;
		}

	}

	if (ch->top_level >= 15)
		ch_printf(ch, " |   Hitroll:            %-6d    Damroll:           %-6d      |" NL, GET_HITROLL(ch), GET_DAMROLL(ch));

	ch_printf(ch, " |   Stan umys�u:   %-47s\\" NL, mental_info(ch));

	ch_printf(ch, " |   Pozycja: ");
	switch (ch->position)
	{
	case POS_DEAD:
		ch_printf(ch, "MARTW%s !!!  ", capitalize(SEX_SUFFIX_YAE(ch)));
		break;
	case POS_MORTAL:
		ch_printf(ch, "Umieraj�c%s  ", SEX_SUFFIX_YAE(ch));
		break;
	case POS_INCAP:
		ch_printf(ch, "Nieprzytomn%s", SEX_SUFFIX_YAE(ch));
		break;
	case POS_STUNNED:
		ch_printf(ch, "Og�uszon%s   ", SEX_SUFFIX_YAE(ch));
		break;
	case POS_SLEEPING:
		ch_printf(ch, "Sen           ");
		break;
	case POS_RESTING:
		ch_printf(ch, "Odpoczynek    ");
		break;
	case POS_STANDING:
		ch_printf(ch, "Stoj�ca       ");
		break;
	case POS_SITTING:
		ch_printf(ch, "Siedz�ca      ");
		break;
	case POS_FIGHTING:
		ch_printf(ch, "Walcz�ca      ");
		break;
	case POS_MOUNTED:
		ch_printf(ch, "Dosiadan%s   ", SEX_SUFFIX_YAE(ch));
		break;
	case POS_SHOVE:
		ch_printf(ch, "Pchan%s      ", SEX_SUFFIX_YAE(ch));
		break;
	case POS_DRAG:
		ch_printf(ch, "Ci�gni�t%s   ", SEX_SUFFIX_YAE(ch));
		break;
	}
	if (IS_IMMORTAL(ch))
		ch_printf(ch, " Ekwipunek: %10d  %10.1f kg   |" NL, ch->carry_number, ((float) ch->carry_weight) / 10);
	else
		ch_printf(ch, " Ekwipunek: %5d/%-5d  %5.1f/%-5.1f kg |" NL, ch->carry_number, can_carry_n(ch), ((float) ch->carry_weight) / 10,
				((float) can_carry_w(ch)) / 10);

	sprintf(buf, "%-4d", GET_AC(ch));
	ch_printf(ch, " |   Uzbrojenie:    %s           ", (ch->top_level >= 25) ? buf : "    ");
	ch_printf(ch, "%-30s", ac_info(ch));
	ch_printf(ch, "   |" NL);
	ch_printf(ch, " |                                                                  |" NL);
	if (ch->pcdata && ch->pcdata->clan)
	{
		bool iscrank = false; /* Trog: na razie nie ma! */
		sprintf(buf, "%s:", capitalize(CLANTYPE(ch->pcdata->clan, 0)));
		ch_printf(ch, " |   %-12s %-30s", buf, ch->pcdata->clan->name);
		ch_printf(ch, "                    |" NL);
		if (iscrank)
			ch_printf(ch, " |   %-12s %-15s                                   |" NL, "Stopie�:", buf);
		ch_printf(ch, " |                                                                  |" NL);
	}
	ch_printf(ch, " `------------------------------------------------------------------'" NL);
	return;
}

/*								-Thoric
 * Display your current exp, level, and surrounding level exp requirements
 */
DEF_DO_FUN( experience )
{
	int ability;
	send_to_char("Oto jak przedstawia si� twoja sytuacja:" NL, ch);
	for (ability = 0; ability < MAX_ABILITY; ability++)
		if (ability != FORCE_ABILITY || ch->skill_level[FORCE_ABILITY] > 1)
			ch_printf(ch, "%-15s   Poziom: %3d/%-3d       Post�p: %s" NL, capitalize(class_table[ability].przypadki[0]),
					ch->skill_level[ability], max_level(ch, ability), show_exp_stars(ch, ability));
		else
			ch_printf(ch, "%-15s   Poziom: %3d/???       Post�p: [&c    ??    &w]\n\r", capitalize(class_table[ability].przypadki[0]),
					ch->skill_level[ability], ch->experience[ability]);

	return;
}

DEF_DO_FUN( affected )
{
	char arg[MAX_INPUT_LENGTH];
	AFFECT_DATA *paf;
	SKILLTYPE *skill;

	if (IS_NPC(ch))
	{
		send_to_char("Eee tam, a co takiego moba jak ty mo�e to obchodzi�?" NL, ch);
		return;
	}

	argument = one_argument(argument, arg);
#if defined(ARMAGEDDON)
    if ( ch->top_level >= 25 )
    	ch_printf( ch, "AC: " FB_CYAN "%d" PLAIN ", Jeste� " FB_CYAN "%s" EOL,
	    GET_AC( ch ), ac_info( ch ) );
    else
	ch_printf( ch, "Jeste� " FB_CYAN "%s" EOL, ac_info( ch ));
#endif

	if (!str_cmp(arg, "by"))
	{
		ch_printf(ch, FB_BLUE "Jeste� nasycon%s:" EOL, SEX_SUFFIX_YAE(ch));
		ch_printf(ch, COL_SCORE "%s" EOL, flag_string(aff_flags_list, ch->affected_by));
		if (ch->top_level >= 20)
		{
			if (ch->resistant > 0)
			{
				send_to_char( FB_BLUE "Odporno��:" EOL, ch);
				ch_printf(ch, COL_SCORE "%s" EOL, flag_string(ris_flags_list, ch->resistant));
			}
			if (ch->immune > 0)
			{
				send_to_char( FB_BLUE "Nietykalno��:" EOL, ch);
				ch_printf(ch, COL_SCORE "%s" EOL, flag_string(ris_flags_list, ch->immune));
			}
			if (ch->susceptible > 0)
			{
				send_to_char( FB_BLUE "Podatno��:" EOL, ch);
				ch_printf(ch, COL_SCORE "%s" EOL, flag_string(ris_flags_list, ch->susceptible));
			}
		}
		return;
	}

	if (ch->affects.empty())
		send_to_char( COL_SCORE "Nic na ciebie nie wp�ywa." EOL, ch);
	else
	{
		send_to_char( FB_BLUE "Znajdujesz si� pod wp�ywem:" EOL, ch);

		for (auto* paf : ch->affects)
			if ((skill = get_skilltype(paf->type)) != NULL)
			{

				ch_printf(ch, COL_SCORE "%s: " FB_YELLOW "%s" COL_SCORE, skill_tname[skill->type], skill->name);

				if (ch->top_level >= 30)
					ch_printf(ch, " zmienia %s", affect_loc_name_pl(paf->location));

				if (ch->top_level >= 40)
					ch_printf(ch, " o %d", paf->modifier);

				if (ch->top_level >= 20)
					ch_printf(ch, " przez oko�o %d godzi%s", paf->duration / 35, NUMBER_SUFF(paf->duration / 35, "n�", "ny", "n"));

				send_to_char("." EOL, ch);
			}

	}
	return;
}

DEF_DO_FUN( inventory )
{
	CHAR_DATA *vch;

	if (*argument && !IS_NPC(ch) && IS_ADMIN(ch->name))
	{
		if ((vch = get_char_world(ch, argument)) == NULL)
		{
			send_to_char("Nie ma nikogo takiego." NL, ch);
			return;
		}
	}
	else
		vch = ch;

	do_gold(ch, vch->name);

	send_to_char( FB_RED "Nosisz ze sob�:" EOL, ch);
	show_list_to_char(vch->carrying, ch, true, true);
	return;
}

DEF_DO_FUN( equipment )
{
	OBJ_DATA *obj;
	int iWear, dam;
	bool found;
	char buf[MAX_STRING_LENGTH];

	send_to_char( FB_RED "Masz na sobie:" EOL, ch);
	found = false;

	for (iWear = 0; iWear < MAX_WEAR; iWear++)
	{
		for (auto* obj : ch->carrying)
			if (obj->wear_loc == iWear)
			{
				/* by Ratm: Ugly haX00r, ale nie chce mi sie tego robi� poprawnie
				 bo bym musia� zmienia� w pizdu rzeczy i potem godzinami
				 testowa� czy dzia�a, wybaczcie ;) */
				if (iWear == WEAR_WIELD)
				{
					if (IS_SET(obj->wear_flags, ITEM_DUAL_WIELD))
						send_to_char( FG_CYAN "<bro� dwur�czna>         " PLAIN, ch);
					else
						send_to_char(where_name[iWear], ch);
				}
				else
					send_to_char(where_name[iWear], ch);

				if (can_see_obj(ch, obj))
				{
					send_to_char(format_obj_to_char(obj, ch, true), ch);
					strcpy(buf, "");
					switch (obj->item_type)
					{
					default:
						break;

					case ITEM_ARMOR:
						if (obj->value[1] == 0)
							obj->value[1] = obj->value[0];
						if (obj->value[1] == 0)
							obj->value[1] = 1;
						dam = (int) ((obj->value[0] * 10) / obj->value[1]);
						if (dam >= 10)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (nowiutki) " :
									obj->gender == GENDER_FEMALE ? " (nowiutka) " : " (nowiutkie) ");
						else if (dam >= 7)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (dobry) " : obj->gender == GENDER_FEMALE ? " (dobra) " : " (dobre) ");
						else if (dam >= 5)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (u�ywany) " :
									obj->gender == GENDER_FEMALE ? " (u�ywana) " : " (u�ywane) ");
						else if (dam >= 3)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (zu�yty) " : obj->gender == GENDER_FEMALE ? " (zu�yta) " : " (zu�yte) ");
						else if (dam >= 1)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (okropny) " :
									obj->gender == GENDER_FEMALE ? " (okropna) " : " (okropne) ");
						else if (dam == 0)
							strcat(buf, " (nieu�ytek) ");
						send_to_char(buf, ch);
						break;

					case ITEM_WEAPON:
						dam = INIT_WEAPON_CONDITION - obj->value[0];
						if (dam < 2)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (nowiutki) " :
									obj->gender == GENDER_FEMALE ? " (nowiutka) " : " (nowiutkie) ");
						else if (dam < 4)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (dobry) " : obj->gender == GENDER_FEMALE ? " (dobra) " : " (dobry) ");
						else if (dam < 7)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (u�ywany) " :
									obj->gender == GENDER_FEMALE ? " (u�ywana) " : " (u�ywane) ");
						else if (dam < 10)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (zu�yty) " : obj->gender == GENDER_FEMALE ? " (zu�yta) " : " (zu�yte) ");
						else if (dam < 12)
							strcat(buf,
									obj->gender == GENDER_MALE ? " (okropny) " :
									obj->gender == GENDER_FEMALE ? " (okropna) " : " (okropne) ");
						else if (dam == 12)
							strcat(buf, " (nieu�ytek) ");
						send_to_char(buf, ch);
						if (obj->value[3] == WEAPON_BLASTER)
						{
							if (obj->blaster_setting == BLASTER_FULL)
								ch_printf(ch, FG_GREEN
								"[" FB_YELLOW "#####" FG_GREEN "]" PLAIN);
							else if (obj->blaster_setting == BLASTER_HIGH)
								ch_printf(ch, FG_GREEN
								"[" FB_YELLOW "#### " FG_GREEN "]" PLAIN);
							else if (obj->blaster_setting == BLASTER_NORMAL)
								ch_printf(ch, FG_GREEN
								"[" FB_YELLOW "###  " FG_GREEN "]" PLAIN);
							else if (obj->blaster_setting == BLASTER_HALF)
								ch_printf(ch, FG_GREEN
								"[" FB_YELLOW "##   " FG_GREEN "]" PLAIN);
							else if (obj->blaster_setting == BLASTER_LOW)
								ch_printf(ch, FG_GREEN
								"[" FB_YELLOW "#    " FG_GREEN "]" PLAIN);
							else if (obj->blaster_setting == BLASTER_STUN)
								ch_printf(ch, FG_GREEN
								"[" FB_YELLOW "     " FG_GREEN "]" PLAIN);
							ch_printf(ch, " %d", obj->value[4]);
						}
						else if ((obj->value[3] == WEAPON_LIGHTSABER || obj->value[3] == WEAPON_VIBRO_BLADE
								|| obj->value[3] == WEAPON_FORCE_PIKE || obj->value[3] == WEAPON_BOWCASTER))
						{
							ch_printf(ch, "%d", obj->value[4]);
						}
						break;
					}
					send_to_char( EOL, ch);
				}
				else
					send_to_char("co�." EOL, ch);
				found = true;
			}
	}

	if (!found)
		send_to_char("Nic." EOL, ch);

	return;
}

void set_title(CHAR_DATA *ch, char *title)
{
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
	{
		send_to_char("I niby jaki by mia� by� ten title? 'Mob nad Moby' ???" NL, ch);
		return;
	}

	strcpy(buf, title);

	STRDUP(ch->pcdata->title, buf);
	return;
}

DEF_DO_FUN( title )
{
	if ( IS_NPC(ch) || !ch->pcdata)
		return;

	if (argument[0] == '\0')
	{
		send_to_char("Zmieni� title na jaki?" NL, ch);
		return;
	}

	smash_tilde(argument);
	set_title(ch, argument);

	ch_printf(ch, "Ok." NL "Od teraz jestes znan%s jako '%s %s" PLAIN "'." NL, SEX_SUFFIX_YAE(ch), ch->name, ch->pcdata->title);
}

DEF_DO_FUN( homepage )
{
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
		return;

	if (argument[0] == '\0')
	{
		if (!ch->pcdata->homepage)
			STRDUP(ch->pcdata->homepage, "");
		ch_printf(ch, "Twoja strona domowa: %s\n\r", show_tilde(ch->pcdata->homepage));
		return;
	}

	if (!str_cmp(argument, "clear"))
	{
		STRDUP(ch->pcdata->homepage, "");
		send_to_char("Ok strona domowa wyczyszczona.\n\r", ch);
		return;
	}

	if (strstr(argument, "://"))
		strcpy(buf, argument);
	else
		sprintf(buf, "http://%s", argument);

	if (strlen(buf) > 70)
		buf[70] = '\0';

	hide_tilde(buf);
	STRDUP(ch->pcdata->homepage, buf);
	send_to_char("Strona domowa ustalona.\n\r", ch);
}

/*
 * Set your personal description				-Thoric
 */
DEF_DO_FUN( description )
{
	if (IS_NPC(ch))
	{
		send_to_char("Mob nie mo�e sam sobie wymy�li� opisu (szkoda)." NL, ch);
		return;
	}

	if (!ch->desc)
	{
		bug("no descriptor");
		return;
	}

	string_append(ch, &ch->description);
	return;
}

/* Ripped off do_description for whois bio's -- Scryn*/
DEF_DO_FUN( bio )
{
	if (IS_NPC(ch))
	{
		send_to_char("Mob nie mo�e ustali� bio!" NL, ch);
		return;
	}

	if (!ch->desc)
	{
		bug("no descriptor", 0);
		return;
	}

	string_append(ch, &ch->pcdata->bio);
	return;
}

DEF_DO_FUN( report )
{
	char buf[MAX_INPUT_LENGTH];

	ch_printf(ch, "Podajesz: %d/%d hp %d/%d mv." NL, ch->hit, ch->max_hit, ch->move, ch->max_move);

	sprintf(buf, "$n podaje: %d/%d hp %d/%d.", ch->hit, ch->max_hit, ch->move, ch->max_move);

	act( PLAIN, buf, ch, NULL, NULL, TO_ROOM);

	return;
}

DEF_DO_FUN( prompt )
{
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];

	if (IS_NPC(ch))
	{
		send_to_char("Eeee tam. Ten prompt jest fajny." NL, ch);
		return;
	}

	smash_tilde(argument);
	one_argument(argument, arg);

	if (!*arg)
	{
		send_to_char("Zmieni� prompt na jaki? (spr�buj 'help prompt')" NL, ch);
		return;
	}

	if (strlen(argument) > 128)
		argument[128] = '\0';

	sprintf(buf, PROMPT_COL_N "%s" PLAIN " ", argument);

	/* Can add a list of pre-set prompts here if wanted.. perhaps
	 'prompt 1' brings up a different, pre-set prompt */
	if (!str_cmp(arg, "default"))
		STRDUP(ch->pcdata->prompt, "");
	else if (!str_cmp(arg, "show"))
	/* fragment odpowiada za wyswietlenie aktualnego prompta - Tanglor
	 */
	{
		ch_printf(ch, NL PROMPT_COL_N "Aktualny prompt : ");
		if (!str_cmp(ch->pcdata->prompt, ""))
			ch_printf(ch, "domy�lny");
		else
			write_to_buffer(ch->desc, ch->pcdata->prompt, 0);
		ch_printf(ch, NL);
		return;
	}
	else
		STRDUP(ch->pcdata->prompt, buf);

	send_to_char("Ok." NL, ch);
	return;
}
