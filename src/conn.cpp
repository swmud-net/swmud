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
 *		 Constate-dependent interpretting module	by Thanos  *
 ****************************************************************************/

#if !defined( WIN32 )
#include <arpa/telnet.h>
#else
	#include <winsock.h>
	#include <sys/timeb.h> /*for _ftime(), uses _timeb struct*/
#endif
#include <ctype.h>
#include <time.h>

#include "mud.h"
#include <SWDate.h>

#if	defined( WIN32 )
// troszke lamerskie, ale moze zadziala :) (Thanos)
	const	char	echo_off_str	[] = { 0xFF, 0xFB, 0x01, '\0' };
	const	char	echo_on_str		[] = { 0xFF, 0xFC, 0x01, '\0' };
#else
const char echo_off_str[] =
{ (char) IAC, (char) WILL, TELOPT_ECHO, '\0' };
const char echo_on_str[] =
{ (char) IAC, (char) WONT, TELOPT_ECHO, '\0' };
#endif

bool check_multi args( ( DESCRIPTOR_DATA *d, char *name ) );
bool check_password args( ( char *_new, char *old ) );
bool check_parse_name args( ( char *name ) );
int check_playing args( ( DESCRIPTOR_DATA *d, char *name, bool kick ) );
int check_reconnect args( ( DESCRIPTOR_DATA *d, char *name, bool fConn ) );
void show_motd args( ( DESCRIPTOR_DATA *d ) );
void init_status_bar args( ( CHAR_DATA *ch ) );
void update_status_bar args( ( CHAR_DATA *ch ) );
void mail_count args( ( CHAR_DATA *ch ) );
void roll_stats args( ( CHAR_DATA *ch ) );

void rename_char args( ( CHAR_DATA *old, CHAR_DATA *tmp ) );

struct con_type
{
	int constate;
	CON_FUN *code;
};
int look_trait_group;
const SWString format_char_attribute1(CHAR_DATA *ch, int przypadek);

const struct con_type con_table[] =
{
// niech to zawsze bedzie pierwsze
		{ CON_PLAYING, parse_command },

		// olc
		{ CON_QEDITOR, qedit },
		{ CON_MEDITOR, medit },
		{ CON_OEDITOR, oedit },
		{ CON_REDITOR, redit },
		{ CON_MPEDITOR, mpedit },
		{ CON_OPEDITOR, opedit },
		{ CON_RPEDITOR, rpedit },
		{ CON_AEDITOR, aedit },
		{ CON_HEDITOR, hedit },
		{ CON_PEDITOR, pedit },
		{ CON_SSEDITOR, ssedit },
		{ CON_SHEDITOR, shedit },
		{ CON_SKEDITOR, skedit },
		{ CON_CEDITOR, cedit },
		{ CON_PROEDITOR, proedit },
		{ CON_MATEDITOR, matedit },
		{ CON_RAEDITOR, raedit },
		{ CON_DIALOGEDIT, dialedit },
		{ CON_TCEDITOR, tcedit },
		{ CON_SPEAKING, mob_speak },
		{ CON_BUILD_HQ, build_hq },
		{ CON_STOCK_MARKET, exchange },

		{ CON_HOLONET, holonet },

		{ CON_CONFIRM_SUICIDE, nanny_confirm_suicide },
		{ CON_OLD_PASS_TO_CHANGE, nanny_old_pass_to_change },
		{ CON_NEW_PASS_TO_CHANGE, nanny_new_pass_to_change },
		{ CON_CONFIRM_NEW_PASS_TO_CHANGE, nanny_confirm_new_pass_to_change },

		{ CON_RENAMING, process_rename },

		// nanny
		{ CON_GET_NAME, nanny_get_name },
		{ CON_GET_OLD_PASSWORD, nanny_get_old_password },
		{ CON_CONFIRM_NEW_NAME, nanny_confirm_new_name },
		{ CON_GET_NEW_COLOUR, nanny_get_new_colour },
		{ CON_GET_CHAR_SET, nanny_get_char_set },
		{ CON_CHECK_CHAR_SET, nanny_check_char_set },
		{ CON_GET_MSP, nanny_get_msp },
		{ CON_WPROWADZENIE, nanny_wprowadzenie },
		{ CON_GET_NEW_PASSWORD, nanny_get_new_password },
		{ CON_CONFIRM_NEW_PASSWORD, nanny_confirm_new_password },
		{ CON_GET_NEW_SEX, nanny_get_new_sex },
		{ CON_GET_NEW_RACE, nanny_get_new_race },
		{ CON_CONFIRM_NEW_RACE, nanny_confirm_new_race },
		{ CON_GET_NEW_CLASS, nanny_get_new_class },
		{ CON_CONFIRM_CLASS, nanny_confirm_class },
		{ CON_GET_DOPELNIACZ, nanny_get_dopelniacz },
		{ CON_GET_CELOWNIK, nanny_get_celownik },
		{ CON_GET_BIERNIK, nanny_get_biernik },
		{ CON_GET_NARZEDNIK, nanny_get_narzednik },
		{ CON_GET_MIEJSCOWNIK, nanny_get_miejscownik },
		{ CON_CHECK_ODMIANE, nanny_check_odmiane },
		{ CON_STATS_OK, nanny_stats_ok },
		{ CON_GET_MIEJSCOWNIK, nanny_get_miejscownik },
		{ CON_GET_MIEJSCOWNIK, nanny_get_miejscownik },
		{ CON_GET_MIEJSCOWNIK, nanny_get_miejscownik },
		{ CON_READ_MOTD, nanny_read_motd },
		{ CON_SHOW_LOOK_TRAIT_GROUP, nanny_show_look_trait_group },
		{ CON_SHOW_LOOK_TRAIT, nanny_show_look_trait },
		{ CON_CONFIRM_LOOK_TRAIT_CHOICE, nanny_confirm_choice },
		{ CON_CONFIRM_ALL_LOOK_TRAIT_CHOICE, nanny_final_confirm },
		{ -1000000, con_notfound } };

CON_FUN* con_function(int constate)
{
	int fn;

	for (fn = 0; con_table[fn].code != con_notfound; fn++)
		if (con_table[fn].constate == constate)
			return con_table[fn].code;
	return con_notfound;
}

const char* inf_name(int i)
{
	switch (i)
	{
	default:
		return "Mianownik (kto/co):" PLAIN "          ";
	case 1:
		return "Dope�niacz (kogo/czego):" PLAIN "     ";
	case 2:
		return "Celownik (komu/czemu):" PLAIN "       ";
	case 3:
		return "Biernik (kogo/co):" PLAIN "           ";
	case 4:
		return "Narz�dnik (z kim/z czym):" PLAIN "    ";
	case 5:
		return "Miejscownik (o kim/o czym):" PLAIN "  ";
	}
}

void handle_connection(DESCRIPTOR_DATA *d, char *cmdline)
{
	CON_FUN *cf;
	cf = con_function(d->connected);
	(*cf)(d, cmdline);
}

void parse_command(DESCRIPTOR_DATA *d, char *argument)
{
	char one_cmd[MAX_INPUT_LENGTH];
	char cmd[MAX_STRING_LENGTH];
	int i = 0;
	int j = 0;

	one_cmd[0] = '\0';
	strcpy(cmd, substitute_alias(d, argument));
	while (cmd[j] != '\0')
	{
		if (cmd[j] == '|' && cmd[j + 1] == '|')
		{
			one_cmd[i] = '|';
			i++;
			j += 2;
		}
		else if (cmd[j] == '|')
		{
			strcat(d->incomm, cmd + i + 1);
			break;
		}
		else
		{
			one_cmd[i] = cmd[j];
			i++;
			j++;
		}
	}
	one_cmd[i] = '\0';
	if (!*one_cmd)
		strcpy(one_cmd, NL);
	interpret(d->character, one_cmd);
}

void con_notfound(DESCRIPTOR_DATA *d, char *argument)
{
	bug("bad d->connected %d.", d->connected);
	close_socket(d, true);
	return;
}

void nanny_confirm_suicide(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	if (!check_password(argument, ch->pcdata->pwd))
	{
		write_to_buffer(d, NL "Z�e has�o." NL "Poczekaj chwil�." NL, 0);
		write_to_buffer(d, echo_on_str, 0);
		ch->wait = 30;
		d->connected = CON_PLAYING;

		sprintf(log_buf, "%s!%s@%s suicide: Wrong Password.", ch->name, d->user, d->host);
		log_string(log_buf);
		return;
	}

	write_to_buffer(d, NL, 0);
	act( FG_RED, "Z ci�k� determinacj� i ogromnym smutkiem podcinasz sobie �y�y...", ch, NULL, NULL, TO_CHAR);
	act( FG_RED, "Zimny dreszcz przechodzi twoje plecy widz�c, jak $n podcina sobie �y�y!", ch, NULL, NULL, TO_ROOM);

	sprintf(log_buf, "%s!%s@%s: Suicide. Deleting Character.", ch->name, d->user, d->host);
	log_string(log_buf);

	set_cur_char(ch);
	raw_kill(ch, ch, 1);
}

void nanny_get_name(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch;
	BAN_DATA *pban;
	char buf[MSL];
	bool fOld;
	int chk;

	if (argument[0] == '\0')
	{
		close_socket(d, false);
		return;
	}

	argument[0] = UPPER(argument[0]);
	if (!check_parse_name(argument))
	{
		write_to_buffer(d, "Nieprawidlowe imie, sprobuj podac inne." NL "Imie:  ", 0);
		return;
	}

	if (!str_cmp(argument, "New") || !str_cmp(argument, "Nowy") || !str_cmp(argument, "Nowe") || !str_cmp(argument, "Nowa"))
	{
		if (d->newstate == 0)
		{
			/* New player */
			/* Don't allow new players if DENY_NEW_PLAYERS is true */
			if (sysdata.DENY_NEW_PLAYERS == true)
			{
				sprintf(buf, "Przykro mi, ale mud wlasnie sie rebootuje." NL);
				write_to_buffer(d, buf, 0);
				sprintf(buf, "Sprobuj za pare minut." NL);
				write_to_buffer(d, buf, 0);
				close_socket(d, false);
			}

			sprintf(buf,
			NL
			"                                  Witaj." NL
			"Jesli chcesz zalozyc sobie postac, musisz wymyslic imie dla swojej postaci." NL
			"Wyb�r imienia jest bardzo wazny. Postaraj sie sprawic, by w miare mozliwosci" NL
			"nie nalezalo do istniejacych juz bohaterow Gwiezdnych Wojen. Upewnij sie, ze" NL
			"wybierasz imie pasujace do postaci, ktora zamierzasz odgrywac i postaraj sie" NL
			"dopasowac ja do swiata naszej gry. Jesli imie, ktore wybierzesz nie bedzie" NL
			"zbyt ciekawe lub z jakichkolwiek innych powodow bedzie nieakceptowalne mozemy" NL
			"poprosic Cie o wymyslenie innego imienia, wiec dobrze sie postaraj." NL NL
			"Wybierz teraz imie dla swojej postaci: ");
			write_to_buffer(d, buf, 0);
			d->newstate++;
			d->connected = CON_GET_NAME;
			return;
		}
		else
		{
			write_to_buffer(d, "Nieprawidlowe imie, sprobuj podac inne." NL "Imie: ", 0);
			return;
		}
	}

	if (check_playing(d, argument, false) == BERR)
	{
		write_to_buffer(d, "Imie: ", 0);
		return;
	}

	fOld = load_char_obj(d, capitalize(argument), true);
	if (!d->character)
	{
		sprintf(log_buf, "Bad player file %s@%s.", argument, d->host);
		log_string(log_buf);
		write_to_buffer(d, "Twoj plik zostal uszkodzony..." NL
		"Wyslij maila na adres mud@swmud.pl by to zglosic." NL, 0);
		close_socket(d, false);
		free_char(d->character);
		return;
	}
	ch = d->character;

	for (auto* pban : ban_list)
	{
		if (!reg_cmp(pban->name, d->host) && pban->level >= get_trust(ch))
		{
			write_to_buffer(d, "Ten adres zostal zbanowany. Nie masz dostepu." NL, 0);
			close_socket(d, false);
			free_char(d->character);
			sprintf(log_buf, "Rejected banned user from: %s, related ban: %s", d->host, pban->name);
			log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
			return;
		}
	}

	if (IS_SET(ch->act, PLR_DENY))
	{
		sprintf(log_buf, "Denying access to %s@%s.", argument, d->host);
		log_string_plus(log_buf, LOG_COMM, sysdata.log_level);

		if (d->newstate != 0)
		{
			write_to_buffer(d, "Taki gracz juz u nas istnieje." NL
			"Wybierz inne imie: ", 0);
			d->connected = CON_GET_NAME;
			d->character->desc = NULL;
			free_char(d->character); /* Big Memory Leak before --Shaddai */
			d->character = NULL;
			return;
		}
		write_to_buffer(d, "Masz zablokowany dostep do muda." NL, 0);
		close_socket(d, false);
		free_char(d->character);
		return;
	}

	chk = check_reconnect(d, argument, false);
	if (chk == BERR)
		return;

	if (chk)
		fOld = true;
	else if (wizlock && !IS_IMMORTAL(ch))
	{
		write_to_buffer(d, "Gra jest zamknieta dla graczy. Moga laczyc sie tylko administratorzy." NL, 0);
		write_to_buffer(d, "Sprobuj za jakis czas." NL, 0);
		close_socket(d, false);
		free_char(d->character);
		return;
	}

	if (fOld)
	{
		if (d->newstate != 0)
		{
			write_to_buffer(d, "Taki gracz juz niestety istnieje." NL "Wybierz inne imie: ", 0);
			d->connected = CON_GET_NAME;
			d->character->desc = NULL;
			free_char(d->character); /* Big Memory Leak before --Shaddai */
			d->character = NULL;
			return;
		}

		/* Old player */
		write_to_buffer(d, "Haslo: ", 0);
		write_to_buffer(d, echo_off_str, 0);
		d->connected = CON_GET_OLD_PASSWORD;
		return;
	}
	else
	{
		if (d->newstate != 0)
		{
			write_to_buffer(d, NL "Nowa postac." NL NL, 0);
			sprintf(buf, "Czy dobrze zrozumialem, %s? (T/N): ", argument);
			write_to_buffer(d, buf, 0);
			d->connected = CON_CONFIRM_NEW_NAME;
			return;
		}
		else
		{
			write_to_buffer(d, "Taki gracz nie istnieje na tym mudzie." NL
			"Wpisz 'nowy' jesli chcesz zalozyc nowa postac: ", 0);
			d->connected = CON_GET_NAME;
			d->character->desc = NULL;
			free_char(d->character); /* Big Memory Leak before --Shaddai */
			d->character = NULL;
			return;
		}
	}
}

void nanny_get_old_password(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];
	int chk;

	write_to_buffer(d, NL, 2);

	if (!check_password(argument, ch->pcdata->pwd))
	{
		//Added by Thanos ( 3 proby podania hasla)
		write_to_buffer(d, "Bledne haslo." NL, 0);
		if (++d->pass_try > MAX_PASS_TRY)
		{
			write_to_descriptor(d->descriptor, "Wroc moze jak poznasz to haslo." NL, 0);
			/* clear descriptor pointer to get rid of bug message in log */
			d->character->desc = NULL;
			close_socket(d, true);
			return;
		}
		else
			write_to_buffer(d, "Sprobuj jeszcze raz." NL "Haslo: ", 0);
		return;
	}

	/* Trog: wazny kod - moze sie kiedys przydac
	 if (SWDate().getTimeT() > 1304869621 && SWString(ch->name).toLower() == "shinobi")
	 {
	 write_to_descriptor( d->descriptor, NL "Nie rozwiesiles plakatow. Wroc jak to zrobisz." NL, 0 );
	 //clear descriptor pointer to get rid of bug message in log
	 d->character->desc = NULL;
	 close_socket( d, true );
	 return;
	 }
	 */

	write_to_buffer(d, echo_on_str, 0);

	if (check_playing(d, ch->name, true))
		return;

	chk = check_reconnect(d, ch->name, true);

	if (chk == BERR)
	{
		if (d->character && d->character->desc)
			d->character->desc = NULL;
		free_char(d->character);
		close_socket(d, false);
		return;
	}

	if (chk)
		return;

	if (check_multi(d, ch->name))
	{
		free_char(d->character);
		close_socket(d, false);
		return;
	}

	strcpy(buf, ch->name);
	d->character->desc = NULL;
	free_char(d->character);

	load_char_obj(d, buf, false);

	ch = d->character;
	STRDUP(ch->pcdata->host, d->host);
	STRDUP(ch->pcdata->user, d->user);

	show_motd(d);
}

void nanny_confirm_new_name(DESCRIPTOR_DATA *d, char *argument)
{
	char buf[MSL];

	switch (*argument)
	{
	case 't':
	case 'T':
		sprintf(buf, NL
		"Kilka technicznych pytan." NL
		"Czy twoj terminal obsluguje kolory zgodne z ANSI? (T/N): ");
		write_to_buffer(d, buf, 0);
		d->connected = CON_GET_NEW_COLOUR;
		break;

	case 'n':
	case 'N':
		write_to_buffer(d, "Ok, wiec jak brzmi to imie? ", 0);
		/* clear descriptor pointer to get rid of bug message in log */
		d->character->desc = NULL;
		free_char(d->character);
		d->character = NULL;
		d->connected = CON_GET_NAME;
		break;

	default:
		write_to_buffer(d, "Napisz [T]ak lub [N]ie. ", 0);
		break;
	}
}

void nanny_get_new_colour(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	switch (*argument)
	{
	case 't':
	case 'T':
	case 'y':
	case 'Y':
		SET_BIT(ch->act, PLR_ANSI);
		break;
	case 'n':
	case 'N':
		break;
	default:
		write_to_buffer(d, "To jest opcja." NL
		"Czy chcesz uzywac kolorow (terminal zgodny z ANSI)? (T/N): ", 0);
		return;
	}

	send_to_char(
	NL "Oto lista dostepnych standardow kodowania polskich znakow:" NL NL, d->character);
	write_to_buffer(d, "[I]SO       - polskie znaki w standardzie ISO-8859-2. (UNIX)" NL, 0);
	write_to_buffer(d, "[W]IN       - polskie znaki w standardzie CP-1250. (Windows)" NL, 0);
	write_to_buffer(d, "[N]OPOL     - nie masz polskich znakow." NL, 0);
	send_to_char( NL "Wybierz typ kodowania polskich znakow: ", ch);

	d->connected = CON_GET_CHAR_SET;
}

void nanny_get_char_set(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	switch (argument[0])
	{
	case 'n':
	case 'N':
		ch->charset = CHARSET_NOPOL;
		break;
	case 'i':
	case 'I':
		ch->charset = CHARSET_ISO;
		break;
	case 'w':
	case 'W':
		ch->charset = CHARSET_WIN;
		break;
	default:
		send_to_char(
		NL "Oto lista dostepnych standardow kodowania polskich znakow:" NL NL, d->character);
		write_to_buffer(d, "[I]SO       - polskie znaki w standardzie ISO-8859-2. (UNIX)" NL, 0);
		write_to_buffer(d, "[W]IN       - polskie znaki w standardzie CP-1250. (Windows)" NL, 0);
		write_to_buffer(d, "[N]OPOL     - nie masz polskich znakow." NL, 0);
		send_to_char( NL "Wybierz typ kodowania polskich znakow: ", d->character);
		return;
	}
	write_to_buffer(d, NL "Test polskich znak�w:" NL, 0);
	write_to_buffer(d, NL "[-�-�-�-�-�-�-�-�-�-�-�-�-�-�-�-�-�-�-]" NL, 0);
	write_to_buffer(d, NL "Czy polskie znaki wy�wietlane s� poprawnie? (T/N): ", 0);
	d->connected = CON_CHECK_CHAR_SET;
}

void nanny_check_char_set(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];

	switch (argument[0])
	{
	case 't':
	case 'T':
		break;
	default:
		send_to_char(
		NL "Oto lista dostepnych standardow kodowania polskich znakow:" NL NL, d->character);
		write_to_buffer(d, "[I]SO       - polskie znaki w standardzie ISO-8859-2. (UNIX)" NL, 0);
		write_to_buffer(d, "[W]IN       - polskie znaki w standardzie CP-1250. (Windows)" NL, 0);
		write_to_buffer(d, "[N]OPOL     - nie masz polskich znakow." NL, 0);
		send_to_char( NL "Wybierz typ kodowania polskich znakow: ", d->character);
		d->connected = CON_GET_CHAR_SET;
		return;
	}

	REMOVE_BIT(ch->act, PLR_SOUND); // domyslnie bez dzwieku, Pixel

	// from nanny_get_msp, Pixel
	send_to_char( CLRSCR, ch); /*"\033[2J"*/

	sprintf(buf,
			NL
			FG_CYAN "Witaj na SW-Mudzie " FB_WHITE "%s" FG_CYAN "!" EOL
			FG_CYAN "Mamy nadziej�, �e czas sp�dzony u nas, b�dzie dla Ciebie mi�y." EOL
			FG_CYAN "Zanim jednak przejdziesz do gry, musisz wykona� kilka niezb�dnych do tego" EOL
			FG_CYAN "czynno�ci, a mianowicie  " FB_WHITE "STWORZY� SWOJ� POSTA�" FG_CYAN ". Nasz " FB_WHITE "MUD" FG_CYAN " jest gr� " FB_WHITE "RPG" FG_CYAN "." EOL
			FG_CYAN "Pami�taj wi�c, �e od tego, czy starannie przeczytasz to WPROWADZENIE i jak" EOL
			FG_CYAN "sumiennie przy�o�ysz si� do tej czynno�ci, zale�e� mo�e twoje powodzenie w" EOL
			FG_CYAN "przysz�ej " FB_WHITE "GRZE" FG_CYAN ". Przejd�my wi�c od s��w do czyn�w i " FB_WHITE "Zaczynajmy" FG_CYAN "!" EOL NL
			NL FG_CYAN "Wci�nij [Enter]" PLAIN " ", ch->name);

	send_to_char(buf, d->character);
	d->connected = CON_WPROWADZENIE;

	//Pixel tu byl i nie denerwowac mnie
	//send_to_char( NL "Czy twoj terminal obsluguje protokol dzwieku MSP? (T/N): ", ch);
	//d->connected = CON_GET_MSP;
}

void nanny_get_msp(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];

	if (argument[0] == 't' || argument[0] == 'T')
		SET_BIT(ch->act, PLR_SOUND);
	else if (argument[0] == 'n' || argument[0] == 'N')
		REMOVE_BIT(ch->act, PLR_SOUND);
	else
	{
		ch_printf(ch, "Wybierz [T]ak albo [N]ie: ");
		return;
	}

	send_to_char( CLRSCR, ch); /*"\033[2J"*/

	sprintf(buf,
			NL
			FG_CYAN "Witaj na SW-Mudzie " FB_WHITE "%s" FG_CYAN "!" EOL
			FG_CYAN "Mamy nadziej�, �e czas sp�dzony u nas, b�dzie dla Ciebie mi�y." EOL
			FG_CYAN "Zanim jednak przejdziesz do gry, musisz wykona� kilka niezb�dnych do tego" EOL
			FG_CYAN "czynno�ci, a mianowicie  " FB_WHITE "STWORZY� SWOJ� POSTA�" FG_CYAN ". Nasz " FB_WHITE "MUD" FG_CYAN " jest gr� " FB_WHITE "RPG" FG_CYAN "." EOL
			FG_CYAN "Pami�taj wi�c, �e od tego, czy starannie przeczytasz to WPROWADZENIE i jak" EOL
			FG_CYAN "sumiennie przy�o�ysz si� do tej czynno�ci, zale�e� mo�e twoje powodzenie w" EOL
			FG_CYAN "przysz�ej " FB_WHITE "GRZE" FG_CYAN ". Przejd�my wi�c od s��w do czyn�w i " FB_WHITE "Zaczynajmy" FG_CYAN "!" EOL NL
			NL FG_CYAN "Wci�nij [Enter]" PLAIN " ", ch->name);

	send_to_char(buf, d->character);

	d->connected = CON_WPROWADZENIE;
}

void nanny_wprowadzenie(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	RACE_DATA *race;
	int i = 1;
	char buf[MSL];

	send_to_char(
	CLRSCR NL
	FG_GREEN "*" PLAIN " " FB_GREEN "WYB�R RASY" PLAIN " " FG_GREEN "*" EOL NL
	FG_GREEN "�wiat Star-Wars, a w takim �wiecie dzieje si� akcja naszej gry, sk�ada" EOL
	FG_GREEN "si� z " FB_WHITE "PLANET" FG_GREEN ", na kt�rych �yj� przedstawiciele r�nych " FB_WHITE "RAS" FG_GREEN ". Ka�da z nich" EOL
	FG_GREEN "posiada unikalne dla niej " FB_WHITE "CECHY" FG_GREEN " i " FB_WHITE "ATRYBUTY" FG_GREEN ". Twoja posta� mo�e sta� si�" EOL
	FG_GREEN "przedstawicielem jednej z nich staj�c si� w ten spos�b cz�ci� naszego �WIATA." EOL, d->character);

	send_to_char( EOL FG_GREEN "Wybierz zatem ras� swojej postaci:" EOL NL, d->character);

	send_to_char("[ " FB_WHITE "1" PLAIN "] " FB_BLUE "Cz�owiek            " PLAIN " ", ch);
	for (auto* race : race_list)
	{
		if (!IS_SET(race->flags, FRACE_PC_AVAIL))
			continue;

		i++;
		sprintf(buf, "[" FB_WHITE "%2d" PLAIN "] " FB_BLUE "%-20s" PLAIN "%s", i, race->przypadki[0], i % 3 == 0 ? NL : " ");
		send_to_char(buf, ch);
	}

	send_to_char( EOL NL FG_BLUE "Jaki jest tw�j wyb�r?" PLAIN " ", d->character);
	d->connected = CON_GET_NEW_RACE;
}

void nanny_old_pass_to_change(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	if (!check_password(argument, ch->pcdata->pwd))
	{
		WAIT_STATE(ch, 40);
		send_to_char("Z�e has�o. Poczekaj 10 sekund." NL, ch);
		d->connected = CON_PLAYING;
		write_to_buffer(d, echo_on_str, 0);
		sprintf(log_buf, "%s!%s@%s bad password to change.", ch->name, d->user, d->host);
		log_string(log_buf);
	}
	else
	{
		write_to_buffer(d, NL "Ok." NL "Podaj teraz nowe has�o: ", 0);
		d->connected = CON_NEW_PASS_TO_CHANGE;
	}
}

void nanny_new_pass_to_change(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];
	char *pwdnew;
	char *p;

	write_to_buffer(d, NL, 2);

	if (strlen(argument) < MIN_PASS_LEN)
	{
		sprintf(buf, "Has�o musi mie� przynajmniej %d znak�w." NL "Has�o: ",
		MIN_PASS_LEN);
		write_to_buffer(d, buf, 0);
		return;
	}

	pwdnew = mkpasswd(argument);
	for (p = pwdnew; *p != '\0'; p++)
	{
		if (*p == '~')
		{
			write_to_buffer(d, "Przykro mi, ale nowe has�o nie mo�e zosta� zaakceptowane, podaj inne." NL
			"Has�o: ", 0);
			return;
		}
	}

	STRDUP(ch->pcdata->pwd, pwdnew);
	write_to_buffer(d, "Prosz� powt�rzy� has�o: ", 0);
	d->connected = CON_CONFIRM_NEW_PASS_TO_CHANGE;
}

void nanny_get_new_password(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];
	char *pwdnew;
	char *p;

	write_to_buffer(d, NL, 2);

	if (strlen(argument) < MIN_PASS_LEN)
	{
		sprintf(buf, "Has�o musi mie� przynajmniej %d znak�w." NL "Has�o: ",
		MIN_PASS_LEN);
		write_to_buffer(d, buf, 0);
		return;
	}

	pwdnew = mkpasswd(argument);
	for (p = pwdnew; *p != '\0'; p++)
	{
		if (*p == '~')
		{
			write_to_buffer(d, "Przykro mi, ale nowe has�o nie mo�e zosta� zaakceptowane, podaj inne." NL
			"Has�o: ", 0);
			return;
		}
	}

	STRDUP(ch->pcdata->pwd, pwdnew);
	write_to_buffer(d, "Prosz� powt�rzy� has�o: ", 0);
	d->connected = CON_CONFIRM_NEW_PASSWORD;
}

void nanny_confirm_new_pass_to_change(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	write_to_buffer(d, NL, 2);

	if (!check_password(argument, ch->pcdata->pwd))
	{
		write_to_buffer(d, "Has�a nie zgadzaj� si�." NL
		"Prosz� poda� has�o: ", 0);
		d->connected = CON_GET_NEW_PASSWORD;
		return;
	}

	send_to_char( PLAIN, ch);
	write_to_buffer(d, echo_on_str, 0);

	d->connected = CON_PLAYING;
	write_to_buffer(d, "Ok." NL "Has�o zosta�o zmienione." NL, 0);
	sprintf(log_buf, "%s!%s@%s changing password.", ch->name, d->user, d->host);
	log_string(log_buf);
	save_char_obj(ch);
}

void nanny_confirm_new_password(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	write_to_buffer(d, NL, 2);

	if (!check_password(argument, ch->pcdata->pwd))
	{
		write_to_buffer(d, "Has�a nie zgadzaj� si�." NL
		"Prosz� poda� has�o: ", 0);
		d->connected = CON_GET_NEW_PASSWORD;
		return;
	}

	send_to_char( PLAIN, ch);
	write_to_buffer(d, echo_on_str, 0);

	show_motd(d);
}

void nanny_get_new_sex(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];
	int iClass;

	switch (argument[0])
	{
	case 'm':
	case 'M':
	case '1':
		ch->sex = SEX_MALE;
		break;
	case 'k':
	case 'K':
	case '2':
		ch->sex = SEX_FEMALE;
		break;
	default:
		send_to_char(
		FG_BLUE
		"To nie jest p�e�. Jak� chcesz mie� " FG_GREEN "P�E�" FG_BLUE "?" PLAIN " ", d->character);
		return;
	}

//    send_to_char( "\033[2J", ch );
	send_to_char( CLRSCR, ch);

	send_to_char(
	FB_GREEN "* WYB�R ZDOLNO�CI *" EOL EOL
	FG_GREEN "S�aw� i Chwa�� mo�na osi�gn�� na wiele sposob�w, jednak tylko nielicznym" EOL
	FG_GREEN "udaje si� dost�pi� tego zaszczytu. Aby bramy " FB_WHITE "MOCY" FG_GREEN " otworzy�y si� przed" EOL
	FG_GREEN "Tob�, przydatnym wyda� si� mo�e wyb�r odpowiedniej " FB_WHITE "ZDOLNO�CI PODSTAWOWEJ" EOL
	FG_GREEN "Mimo, �e na naszym mudzie obowi�zuje " FB_WHITE "MULTIKLASOWO��" FG_GREEN " i twoja posta� b�dzie" EOL
	FG_GREEN "posiada�a zdolno�ci WSZYSTKICH klas, to od zdolno�ci podstawowej zale�y, jaki" EOL
	FG_GREEN "kierunek obierzesz w tej d�ugiej i kr�tej DRODZE." EOL
	FG_GREEN "Plotki g�osz�, �e niekt�rzy rodz� si� posiadaj�c jeszcze inn� zdolno��..." EOL, d->character);

	send_to_char( EOL FG_GREEN "Wybierz teraz zdolno��:" EOL, d->character);
	strcpy(buf, EOL "[" FB_WHITE " 1" PLAIN "] " FB_BLUE);

	for (iClass = 0; iClass < MAX_ABILITY; iClass++)
	{
		/* Thanos --> No More Moc  ;) */
		if (iClass == FORCE_ABILITY)
			continue;

		if (iClass > 0)
			sprintf(buf, EOL "[" FB_WHITE "%2d" PLAIN "] " FB_BLUE, (iClass + 1));
		strcat(buf, class_table[iClass].przypadki[0]);
		send_to_char(buf, ch);
	}

	send_to_char( EOL NL FG_BLUE "Jaki jest tw�j wyb�r?" PLAIN " ", d->character);
	d->connected = CON_GET_NEW_CLASS;
}

void nanny_get_new_race(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	RACE_DATA *race;
	char arg[MSL];
	int i = 1;
	bool found = false;

	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "help"))
	{
		do_help(ch, argument);
		send_to_char( EOL FG_BLUE "Jaki jest tw�j wyb�r?" PLAIN " ", d->character);
		return;
	}

	if ((atoi(arg) == 1) || (toupper(arg[0]) == toupper(base_race->przypadki[0][0]) && !str_prefix(arg, base_race->przypadki[0])))
	{
		ch->race = base_race;
		found = true;
	}
	else
		for (auto* race : race_list)
		{
			if (!IS_SET(race->flags, FRACE_PC_AVAIL))
				continue;

			i++;
			if ((atoi(arg) == i) || (toupper(arg[0]) == toupper(race->przypadki[0][0]) && !str_prefix(arg, race->przypadki[0])))
			{
				ch->race = race;
				found = true;
				break;
			}
		}

	if (!found)
	{
		send_to_char(
		EOL FG_BLUE
		"To nie jest rasa. Jak� " FG_GREEN "RAS�" FG_BLUE " chcesz wybra�?" PLAIN " ", d->character);
		return;
	}

	send_to_char(ch->race->description, ch);
//	do_help( ch, (char *)(ch->race->name) );

	ch_printf(ch,
	EOL FG_BLUE "Czy na pewno chcesz by� " FB_WHITE "%s" FG_BLUE "? (" FB_BLUE "T" FG_BLUE "/" FB_BLUE "N" FG_BLUE "):" PLAIN " ",
	IS_RACE(ch, "HUMAN") ? "Cz�owiekiem" : ch->race->przypadki[4]);
	d->connected = CON_CONFIRM_NEW_RACE;
}

void nanny_confirm_new_race(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	RACE_DATA *race;
	char buf[MSL];
	int i = 1;

	switch (argument[0])
	{
	case 'y':
	case 'Y':
	case 't':
	case 'T':
		break;
	default:
		send_to_char( EOL FG_GREEN "Wybierz ras�:" EOL NL, d->character);

		send_to_char("[ " FB_WHITE "1" PLAIN "] " FB_BLUE "Cz�owiek            " PLAIN " ", ch);
		for (auto* race : race_list)
		{
			if (!IS_SET(race->flags, FRACE_PC_AVAIL))
				continue;

			i++;
			sprintf(buf, "[" FB_WHITE "%2d" PLAIN "] " FB_BLUE "%-20s" PLAIN "%s", i, race->przypadki[0], i % 3 == 0 ? NL : " ");
			send_to_char(buf, ch);
		}

		send_to_char( EOL NL FG_BLUE "Jaki jest tw�j wyb�r?" PLAIN " ", d->character);

		d->connected = CON_GET_NEW_RACE;
		return;
	}

	send_to_char( CLRSCR, ch);

	send_to_char(" " FB_GREEN "*" PLAIN " " FG_GREEN "WYB�R P�CI" PLAIN " " FB_GREEN "*" EOL NL
	FG_GREEN "Teraz mo�esz wybra� " FB_WHITE "P�E�" FG_GREEN " twojej postaci. Mimo, �e wyb�r ten" EOL
	FG_GREEN "nie ma bezpo�redniego wp�ywu na przebieg gry. Dobrze jest wybra�" EOL
	FG_GREEN "tak�, jaka najbardziej pasuje do postaci, kt�r� chcesz odgrywa�." EOL
	FG_GREEN "Pami�taj jednak, �e w p�niejszej grze mog� zdarzy� si� sytuacje, �e" EOL
	FG_GREEN "p�e� twojej postaci mo�e si� " FB_WHITE "ZMIENI�" FG_GREEN ". Nie martw si� jednak," EOL
	FG_GREEN "bo w �wiecie " FB_WHITE "STAR-WARS" FG_GREEN " wszystko jest mo�liwe." EOL, d->character);

	send_to_char(
	EOL FG_GREEN "Wybierz p�e�:" EOL NL
	"[" FB_WHITE "1" PLAIN "] "
	FB_BLUE "M�czyzna" FG_BLUE "     - Silny, odwa�ny i honorowy." EOL
	"[" FB_WHITE "2" PLAIN "] "
	FB_BLUE "Kobieta  " FG_BLUE "     - Nieco s�absza fizycznie, ale ten urok..." EOL, d->character);
	send_to_char( EOL FG_BLUE "Jaki jest tw�j wyb�r?" PLAIN " ", d->character);

	d->connected = CON_GET_NEW_SEX;
}

void nanny_get_new_class(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	int iClass;
	char arg[MSL];

	argument = one_argument(argument, arg);
	if (!str_cmp(arg, "help"))
	{
		do_help(ch, argument);
		send_to_char( EOL FG_BLUE "Jaki jest tw�j wyb�r?" PLAIN " ", d->character);
		return;
	}

	for (iClass = 0; iClass < MAX_ABILITY; iClass++)
	{
		/* Thanos --> No More Moc  ;) */
		if (iClass == FORCE_ABILITY)
			continue;

		if ((atoi(arg) == iClass + 1)
				|| (toupper(arg[0]) == toupper(class_table[iClass].przypadki[0][0]) || !str_prefix(arg, class_table[iClass].przypadki[0])))
		{
			ch->main_ability = iClass;
			break;
		}
	}

	if (iClass == MAX_ABILITY || !class_table[iClass].przypadki[0] || class_table[iClass].przypadki[0][0] == '\0')
	{
		send_to_char( FG_BLUE
		"To nie jest zdolno��. Jak� " FG_GREEN "ZDOLNO��" FG_BLUE
		" chcesz wybra�?" PLAIN " ", d->character);
		return;
	}

	ch_printf(ch,
	NL FG_BLUE "Czy napewno chcesz by to by�a twoja zdolno�� podstawowa? (&BT&b/&BN&b):&w ", 0);
	d->connected = CON_CONFIRM_CLASS;
}

void nanny_confirm_class(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];
	int iClass;

	switch (argument[0])
	{
	case 'y':
	case 'Y':
	case 't':
	case 'T':
		break;
	default:
		send_to_char( FG_GREEN "Wybierz zdolno��:" EOL, d->character);
		strcpy(buf, EOL "[" FB_WHITE " 1" PLAIN "] " FB_BLUE);
		for (iClass = 0; iClass < MAX_ABILITY; iClass++)
		{
			/* Thanos --> No More Moc  ;) */
			if (iClass == FORCE_ABILITY)
				continue;

			if (iClass > 0)
				sprintf(buf, EOL "[" FB_WHITE "%2d" PLAIN "] " FB_BLUE, (iClass + 1));

			strcat(buf, class_table[iClass].przypadki[0]);
			send_to_char(buf, ch);
		}
		send_to_char( EOL NL FG_BLUE "Jaki jest tw�j wyb�r?" PLAIN " ", d->character);
		d->connected = CON_GET_NEW_CLASS;
		return;
	}

	send_to_char(
	CLRSCR NL
	FB_GREEN "*" FG_GREEN " ODMIANA IMIENIA " FB_GREEN "*" EOL NL
	FG_GREEN "Nasz MUD jest gr� " FB_WHITE "POLSK�" FG_GREEN ". Zosta� stworzony w Polsce i przez Polak�w." EOL
	FG_GREEN "Oznacza to, �e imiona graczy bior�cych udzia� w grze powinny zosta�" EOL
	FB_WHITE "PRAWID�OWO" FG_GREEN " odmienione przez " FB_WHITE "PRZYPADKI" FG_GREEN "." EOL, d->character);

	send_to_char( FG_GREEN "Wprowad� wi�c odmian� imienia swojej postaci:" EOL, ch);

	STRDUP(ch->przypadki[0], ch->name);
	ch_printf(ch, NL FG_BLUE "%s " FB_WHITE "%s" EOL, inf_name(0), ch->name);
	ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(1));
	d->connected = CON_GET_DOPELNIACZ;
}

void nanny_get_dopelniacz(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	send_to_char( PLAIN, ch);
	if ((argument[0] == '\0') || ( UPPER( argument[0] ) != UPPER(ch->name[0])))
	{
		write_to_buffer(d, "Musisz poda� poprawn� odmian�." NL, 0);
		ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(1));
		return;
	}
	STRDUP(ch->przypadki[1], capitalize(argument));
	ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(2));
	d->connected = CON_GET_CELOWNIK;
}

void nanny_get_celownik(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	send_to_char( PLAIN, ch);

	if ((argument[0] == '\0') || ( UPPER( argument[0] ) != UPPER(ch->name[0])))
	{
		write_to_buffer(d, "Musisz poda� poprawn� odmian�." NL, 0);
		ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(2));
		return;
	}

	STRDUP(ch->przypadki[2], capitalize(argument));
	ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(3));
	d->connected = CON_GET_BIERNIK;
}

void nanny_get_biernik(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	send_to_char( PLAIN, ch);
	if ((argument[0] == '\0') || ( UPPER( argument[0] ) != UPPER(ch->name[0])))
	{
		write_to_buffer(d, "Musisz poda� poprawn� odmian�." NL, 0);
		ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(3));
		return;
	}

	STRDUP(ch->przypadki[3], capitalize(argument));
	ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(4));
	d->connected = CON_GET_NARZEDNIK;
}

void nanny_get_narzednik(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	send_to_char( PLAIN, ch);
	if ((argument[0] == '\0') || ( UPPER( argument[0] ) != UPPER(ch->name[0])))
	{
		write_to_buffer(d, "Musisz poda� poprawn� odmian�." NL, 0);
		ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(4));
		return;
	}

	STRDUP(ch->przypadki[4], capitalize(argument));
	ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(5));
	d->connected = CON_GET_MIEJSCOWNIK;
}

void nanny_get_miejscownik(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	send_to_char( PLAIN, ch);
	if ((argument[0] == '\0') || ( UPPER( argument[0] ) != UPPER(ch->name[0])))
	{
		write_to_buffer(d, "Musisz poda� poprawn� odmian�." NL, 0);
		ch_printf(ch, FG_BLUE "%s " FB_WHITE, inf_name(5));
		return;
	}

	argument[0] = UPPER(argument[0]);

	STRDUP(ch->przypadki[5], argument);
	ch_printf(ch, EOL FG_BLUE
	"Czy odmiana zosta�a wprowadzona prawid�owo (bardzo wa�ne)? "
	"(" FB_BLUE "T" FG_BLUE "/" FB_BLUE "N" FG_BLUE "):" PLAIN " ", 0);
	d->connected = CON_CHECK_ODMIANE;
}

void nanny_check_odmiane(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	switch (argument[0])
	{
	char buf[MAX_INPUT_LENGTH];

case 't':
case 'T':
case 'y':
case 'Y':
	send_to_char(
	CLRSCR NL
	NL FB_GREEN "*" FG_GREEN " LOSOWANIE ATRYBUT�W " FB_GREEN "*" EOL NL
	FG_GREEN "Nasz mud wylosuje ci teraz " FB_WHITE "ATRYBUTY" FG_GREEN ". S� one bardzo wa�nym elementem gry, bo" EOL
	FG_GREEN "to od nich zale�e� b�dzie wizerunek twojej postaci oraz stopie� przyswajania" EOL
	FG_GREEN "sobie " FB_WHITE "ZDOLNO�CI" FG_GREEN ". Zastan�w si�, jak� postac chcesz odgrywa� i kt�ry z sze�ciu " EOL
	FG_GREEN "poni�szych ATRYBUT�W b�dzie dla ciebie najwa�niejszy. Mo�esz niezgodzi� si� z" EOL
	FG_GREEN "wynikiem losowania, nasz mud powt�rzy je do skutku. Nie licz jednak na to, �e" EOL
	FG_GREEN "wszystkie z nich b�d� na wysokim poziomie. W ko�cu 'Co� za co�' ;-)" EOL, ch);
	break;

default:
	send_to_char( NL FG_GREEN
	"Wprowad� odmian� imienia postaci:" EOL NL FG_BLUE
	"Mianownik (kto/co):" PLAIN "          ", ch);
	sprintf(buf, FB_WHITE "%s" PLAIN, ch->name);
	send_to_char(buf, ch);
	send_to_char( EOL FG_BLUE
	"Dope�niacz (kogo/czego):" PLAIN "     " FB_WHITE, ch);
	d->connected = CON_GET_DOPELNIACZ;
	return;
	}

#if defined( ARMAGEDDON )
	roll_stats( ch );
#else
	ch->perm_str = number_range(1, 6) + number_range(1, 6) + number_range(1, 6);
	ch->perm_int = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
	ch->perm_wis = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
	ch->perm_dex = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
	ch->perm_con = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
	ch->perm_cha = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);

	ch->perm_str += ch->race->str_plus;
	ch->perm_int += ch->race->int_plus;
	ch->perm_wis += ch->race->wis_plus;
	ch->perm_dex += ch->race->dex_plus;
	ch->perm_con += ch->race->con_plus;
	ch->perm_cha += ch->race->cha_plus;
#endif
	ch_printf(ch, NL NL "      " FB_BLUE
	"Charyzma:    " FB_WHITE "%2d     " FB_BLUE
	"Inteligencja:" FB_WHITE "%2d     " FB_BLUE
	"M�dro��:     " FB_WHITE "%2d" EOL, ch->perm_cha, ch->perm_int, ch->perm_wis);
	ch_printf(ch, "      " FB_BLUE
	"Zr�czno��:   " FB_WHITE "%2d     " FB_BLUE
	"Kondycja:    " FB_WHITE "%2d     " FB_BLUE
	"Si�a:        " FB_WHITE "%2d" EOL NL, ch->perm_dex, ch->perm_con, ch->perm_str);

	ch_printf(ch, NL FG_BLUE "Czy takie warto�ci ci� zadowalaj�? "
	"(" FB_BLUE "T" FG_BLUE "/" FB_BLUE "N" FG_BLUE "):" PLAIN " ");
	d->connected = CON_STATS_OK;
}

void show_look_groups(CHAR_DATA *ch)
{
	char buf[MSL];
	int index, count;
	int splitCounter;
	RACE_DATA *pRace = ch->race;

	if (ch->attribute1 >= 0 || ch->attribute2 >= 0)
	{
		ch_printf(ch, FG_GREEN " Jeste� rozpoznawan%s jako : " FG_YELLOW,
		MALE(ch) ? "y" : "a");
		const SWString &attrib = format_char_attribute1(ch, 0);
		ch_printf(ch, attrib.c_str());
		ch_printf(ch, EOL);
	}
	ch_printf(ch, NL FG_GREEN " Wybierz jedn� z grup:" NL NL " ");

	for (index = 0, splitCounter = 0; index < MAX_DESC_TYPES; index++)
	{
		if (pRace->desc_restrictions[index] == 0)
			continue;

		if (ch->attribute2 != INT_MIN && ch->attribute2 / 100 == FUR && index == FUR)
			continue;

		splitCounter++;
		sprintf(buf, PLAIN "[" FG_YELLOW "%s" PLAIN "]", category_desc[index]);
		count = 22 - strlen(buf);
		if (splitCounter % 4 == 0)
			strcat(buf, NL " ");
		else
			for (; count > 0; count--)
				strcat(buf, " ");
		send_to_char(buf, ch);
	}
	ch_printf(ch, NL);
}

void show_look_traits(CHAR_DATA *ch, int index)
{
	char buf[MSL];
	int descItem, descGroup = index, count;
	int splitCounter;
	RACE_DATA *pRace = ch->race;

	if (ch->attribute1 >= 0 || ch->attribute2 >= 0)
	{
		ch_printf(ch, NL FG_GREEN " Jeste� rozpoznawan%s jako : " FG_YELLOW,
		MALE(ch) ? "y" : "a");
		const SWString &attrib = format_char_attribute1(ch, 0);
		ch_printf(ch, attrib.c_str());
		ch_printf(ch, EOL NL);
	}
	ch_printf(ch,
	FG_GREEN " Cechy wygl�du zebrane w kategorii " FG_YELLOW "%s." EOL NL, category_desc[descGroup]);

	for (descItem = 0, splitCounter = 0; descItem < 61; descItem++)
	{
		if (pRace->desc_restrictions[descGroup] < (1 << (descItem + 1)))
			break;
		if (!IS_SET(pRace->desc_restrictions[descGroup], (1 << (descItem + 1))))
			continue;

		splitCounter++;
		const SWString &attrib = char_attribute((desc_type) descGroup, descItem, ch->sex, 0);
		sprintf(buf, FG_YELLOW "%s" PLAIN, attrib.c_str());
		if (splitCounter == 1)
			sprintf(buf, FG_YELLOW " %s" PLAIN, attrib.c_str());
		count = 26 - strlen(buf);
		if (splitCounter == 1)
			count++;
		if (splitCounter % 3 != 0)
			for (; count > 0; count--)
				strcat(buf, " ");
		else
			strcat(buf, NL " ");
		send_to_char(buf, ch);
	}
	strcat(buf, NL);
	ch_printf(ch, NL NL PLAIN " [" FG_GREEN "Powr�t" PLAIN "]" EOL); // wywalilem jedno EOL
}
void nanny_stats_ok(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);

	switch (argument[0])
	{
	case 't':
	case 'T':
		break;
	case 'n':
	case 'N':
#if defined( ARMAGEDDON )
	roll_stats( ch );
#else
		ch->perm_str = number_range(1, 6) + number_range(1, 6) + number_range(1, 6);
		ch->perm_int = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
		ch->perm_wis = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
		ch->perm_dex = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
		ch->perm_con = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);
		ch->perm_cha = number_range(3, 6) + number_range(1, 6) + number_range(1, 6);

		ch->perm_str += ch->race->str_plus;
		ch->perm_int += ch->race->int_plus;
		ch->perm_wis += ch->race->wis_plus;
		ch->perm_dex += ch->race->dex_plus;
		ch->perm_con += ch->race->con_plus;
		ch->perm_cha += ch->race->cha_plus;
#endif
		ch_printf(ch, NL NL "      " FB_BLUE
		"Charyzma:    " FB_WHITE "%2d     " FB_BLUE
		"Inteligencja:" FB_WHITE "%2d     " FB_BLUE
		"M�dro��:     " FB_WHITE "%2d" EOL, ch->perm_cha, ch->perm_int, ch->perm_wis);
		ch_printf(ch, "      " FB_BLUE
		"Zr�czno��:   " FB_WHITE "%2d     " FB_BLUE
		"Kondycja:    " FB_WHITE "%2d     " FB_BLUE
		"Si�a:        " FB_WHITE "%2d" EOL NL, ch->perm_dex, ch->perm_con, ch->perm_str);

		ch_printf(ch, NL FG_BLUE "Czy takie warto�ci ci� zadowalaj�? "
		"(" FB_BLUE "T" FG_BLUE "/" FB_BLUE "N" FG_BLUE "):" PLAIN " ");
		return;

	default:
		ch_printf(ch, FG_BLUE "Wybierz " PLAIN
		"[" FB_WHITE "T" PLAIN "]" FG_BLUE "ak lub " PLAIN
		"[" FB_WHITE "N" PLAIN "]" FG_BLUE "ie:" PLAIN " ");
		return;
	}

	/* Trog: losujemy Force Events'y */
	drawlots_fevents(ch);

	ch_printf(ch,
	CLRSCR NL FB_GREEN "*" FG_GREEN " OKRE�LENIE WYGL�DU " FB_GREEN "*" EOL NL
	FG_GREEN " Twoj� posta� charakteryzuj� dwie cechy wygl�du. Tak postrzega� b�d� Ci� inni" NL
	" gracze i to one b�d� Ci� spo�r�d nich wyr�nia�. Ka�da z ras posiada specyficzne cechy" NL
	" Cechy te podzielone s� na kilka grup. Wybierz najpierw grup�, potem interesuj�ca Ci� ceche" NL);
	show_look_groups(ch);

	d->connected = CON_SHOW_LOOK_TRAIT;
}
void nanny_show_look_trait_group(DESCRIPTOR_DATA *d, char *argument)
{

}
void nanny_show_look_trait(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];
	char arg[MSL];
	int descGroup;
	RACE_DATA *pRace = ch->race;

	if (ch->attribute1 >= 0 && ch->attribute2 >= 0)
	{
		const SWString &attrib = format_char_attribute1(ch, 0);
		ch_printf(ch,
		CLRSCR NL FB_GREEN "*" FG_GREEN " USTALENIE HAS�A " FB_GREEN "*" EOL NL
		FG_GREEN "Twoja posta� zosta�a " FB_WHITE "STWORZONA" FG_GREEN ". Oto jak si� prezentuje:" EOL NL
		FB_PINK "Nazywasz si� " FB_WHITE "%s" FB_PINK " i jeste� " FB_WHITE "%s" FB_PINK ", a twoj� podstawow� zdolno�ci�" EOL
		FB_PINK "jest " FB_WHITE "%s" FB_PINK ". Jeste� %s postrzegan%s jako " FB_WHITE "%s." EOL, ch->name,
		IS_RACE(ch, "HUMAN") ? "Cz�owiekiem" : CH_RACE(ch, 4), //Pixel: przypadkiF
		class_table[ch->main_ability].przypadki[0],
		MALE( ch ) ? "m�czyzn�" : "kobiet�",
		MALE( ch ) ? "yn" : "�", attrib.c_str());
		send_to_char(
		EOL FG_GREEN "Ostatni� rzecz�, jak� musisz wykona� jest wymy�lenie trudnego do odgadni�cia" EOL
		FB_WHITE "HAS�A" FG_GREEN ". Nasz mud zapyta Ci� o nie przy ka�dej nast�pnej pr�bie zalogowania si�." EOL
		FG_GREEN "Pomy�l wi�c jakie ma ono by� i dobrze je " FB_WHITE "ZAPAMI�TAJ" FG_GREEN "!" EOL NL, ch);

		send_to_char( EOL FG_BLUE, ch);
		sprintf(buf, "Podaj has�o dla postaci %s: %s", ch->name, echo_off_str);
		write_to_buffer(d, buf, 0);

		d->connected = CON_GET_NEW_PASSWORD;
	}

	argument = one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		show_look_groups(ch);
		return;
	}
	for (descGroup = 0; descGroup <= MAX_DESC_TYPES; descGroup++)
	{
		if (!strcmp(category_desc[descGroup], "Empty") || descGroup >= MAX_DESC_TYPES)
		{
			send_to_char( FG_GREEN NL "Nie ma takiej kategorii" NL, ch);
			return;
		}
		if (pRace->desc_restrictions[descGroup] == 0)
			continue;
		if (is_name_prefix(arg, category_desc[descGroup]))
			break;
	}

	ch_printf(ch, FG_GREEN " Wybrana kategoria to " FG_YELLOW "%s." EOL NL, category_desc[descGroup]); // moze NL?
	show_look_traits(ch, descGroup);
	send_to_char(NL, ch);
	look_trait_group = descGroup;
	d->connected = CON_CONFIRM_LOOK_TRAIT_CHOICE;
}
void nanny_confirm_choice(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char arg[MSL];
	int descItem, descGroup;
	RACE_DATA *pRace = ch->race;

	argument = one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		show_look_traits(ch, look_trait_group);
		send_to_char(NL, ch);
		return;
	}
	else if (is_name_prefix(arg, (char*) "powr�t"))
	{
		show_look_groups(ch);
		d->connected = CON_SHOW_LOOK_TRAIT;
		return;
	}
	//ustawianie wybranej cechy charakteru
	else
	{
		descGroup = look_trait_group;
		bool bFound = false;

		for (descItem = 0; descItem < 61; descItem++)
		{
			if (!IS_SET(pRace->desc_restrictions[descGroup], (1 << (descItem + 1))))
				continue;

			const SWString &attrib = char_attribute((desc_type) descGroup, descItem, ch->sex, 0);
			if (is_name_prefix(arg, attrib.c_str()))
			{
				//cecha wygladu znaleziona
				bFound = true;
				break;
			}
		}
		if (bFound == false)
		{
			send_to_char( FG_GREEN NL "Nie ma takiej cechy wygl�du" NL, ch);
			return;
		}
		int markIndex = -descGroup * 100 - descItem - 1;
		int finalIndex = descGroup * 100 + descItem;
		if (ch->attribute1 == finalIndex || ch->attribute2 == finalIndex)
		{
			send_to_char(
			FG_GREEN "Ju� masz tak� cech� - wybierz inn� jako drug�" EOL, ch);
			return;
		}
		if (descGroup == FUR)
		{
			if (ch->attribute2 == markIndex)
			{
				ch->attribute2 = finalIndex;
			}
			else
				ch->attribute2 = markIndex;
		}
		else if (ch->attribute1 == markIndex)
		{
			ch->attribute1 = finalIndex;
		}
		else if (ch->attribute2 == markIndex)
		{
			ch->attribute2 = finalIndex;
		}
		else if (ch->attribute1 >= 0)
			ch->attribute2 = markIndex;
		else
			ch->attribute1 = markIndex;

		if ((ch->attribute1 < 0 && ch->attribute1 != INT_MIN) || (ch->attribute2 < 0 && ch->attribute2 != INT_MIN))
		{
			ch_printf(ch,
			NL FG_GREEN " Czy chcesz by� rozpoznawan%s jako : " FG_YELLOW,
			MALE(ch) ? "y" : "a");
			const SWString &attrib = format_char_attribute1(ch, 0);
			ch_printf(ch, attrib.c_str());
			ch_printf(ch,
			FG_GREEN "? (" FB_GREEN "T" FG_GREEN "/" FB_GREEN "N" FG_GREEN "):");
		}
		d->connected = CON_CONFIRM_ALL_LOOK_TRAIT_CHOICE;
	}
}
void nanny_final_confirm(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];

	switch (argument[0])
	{
	case 't':
	case 'T':
		if (ch->attribute1 < 0 || ch->attribute2 < 0)
		{
			if (ch->attribute1 < 0 && ch->attribute1 != INT_MIN)
				ch->attribute1 = -(ch->attribute1 + 1);
			else if (ch->attribute2 < 0 && ch->attribute2 != INT_MIN)
				ch->attribute2 = -(ch->attribute2 + 1);
		}
		if (ch->attribute1 >= 0 && ch->attribute2 >= 0)
		{
			const SWString &attrib = format_char_attribute1(ch, 0);
			ch_printf(ch,
			CLRSCR NL FB_GREEN "*" FG_GREEN " USTALENIE HAS�A " FB_GREEN "*" EOL NL
			FG_GREEN "Twoja posta� zosta�a " FB_WHITE "STWORZONA" FG_GREEN ". Oto jak si� prezentuje:" EOL NL
			FB_PINK "Nazywasz si� " FB_WHITE "%s" FB_PINK " i jeste� " FB_WHITE "%s" FB_PINK ", a twoj� podstawow� zdolno�ci�" EOL
			FB_PINK "jest " FB_WHITE "%s" FB_PINK ". Jeste� %s postrzegan%s jako " FB_WHITE "%s." EOL, ch->name, CH_RACE(ch, 4), // Pixel: przypadkiF
			class_table[ch->main_ability].przypadki[0],
			MALE( ch ) ? "m�czyzn�" : "kobiet�",
			MALE( ch ) ? "yn" : "�", attrib.c_str());
			send_to_char(
			EOL FG_GREEN "Ostatni� rzecz�, jak� musisz wykona� jest wymy�lenie trudnego do odgadni�cia" EOL
			FB_WHITE "HAS�A" FG_GREEN ". Nasz mud zapyta Ci� o nie przy ka�dej nast�pnej pr�bie zalogowania si�." EOL
			FG_GREEN "Pomy�l wi�c jakie ma ono by� i dobrze je " FB_WHITE "ZAPAMI�TAJ" FG_GREEN "!" EOL NL, ch);

			send_to_char( EOL FG_BLUE, ch);
			sprintf(buf, "Podaj has�o dla postaci %s: %s", ch->name, echo_off_str);
			write_to_buffer(d, buf, 0);

			d->connected = CON_GET_NEW_PASSWORD;
		}
		else
		{
			d->connected = CON_SHOW_LOOK_TRAIT;
			show_look_groups(ch);
		}
		break;
	case 'n':
	case 'N':
		show_look_traits(ch, look_trait_group);
		send_to_char(NL, ch);
		if (ch->attribute1 < 0 && ch->attribute1 != INT_MIN)
			ch->attribute1 = INT_MIN;
		else if (ch->attribute2 < 0 && ch->attribute2 != INT_MIN)
			ch->attribute2 = INT_MIN;
		d->connected = CON_CONFIRM_LOOK_TRAIT_CHOICE;
		break;
	default:
		if ((ch->attribute1 < 0 && ch->attribute1 != INT_MIN) || (ch->attribute2 < 0 && ch->attribute2 != INT_MIN))
		{
			ch_printf(ch,
			NL FG_GREEN " Czy chcesz by� rozpoznawan%s jako : " FG_YELLOW,
			MALE(ch) ? "y" : "a");
			const SWString &attrib = format_char_attribute1(ch, 0);
			ch_printf(ch, attrib.c_str());
			ch_printf(ch,
			EOL FG_BLUE " Odpowiedz tylko " FB_BLUE "T" FG_BLUE "/" FB_BLUE "N" FG_BLUE "" PLAIN, CH_RACE(ch, 4));  // Pixel: przypadkiF
		}
		d->connected = CON_CONFIRM_ALL_LOOK_TRAIT_CHOICE;
	}
}
void nanny_read_motd(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char buf[MSL];

	add_char(ch);
	d->connected = CON_PLAYING;

	send_to_pager( CLRSCR, ch);

	if (ch->top_level == 0)
	{
		OBJ_DATA *obj;

		ch->pcdata->clan = NULL;

		ch->perm_lck = number_range(6, 18);
		/* Normalny gracz nie ma juz szanys na moc, moc by questy...
		 ch->perm_frc = number_range(-2000, 20); */
		ch->perm_frc = 0;
		ch->affected_by = ch->race->affected;
		ch->perm_lck += ch->race->lck_plus;
		ch->perm_frc += ch->race->frc_plus;

		if ( IS_RACE( ch, "DUINUOGWUIN" ) || ch->main_ability == FORCE_ABILITY)
			ch->perm_frc = URANGE(1, ch->perm_frc, 20);
		else
			ch->perm_frc = URANGE(0, ch->perm_frc, 20);

		/* Trog: tutaj ustawiamy kazdemu graczowi
		 * common na 100 i jego "rasowy" jezyk, czyli
		 * tu nie robimy nic, bo jest w knows_language */

		/* ch->resist           += ch->race->resist;    drats */
		/* ch->susceptible     += ch->race->suscept;    drats */

#if defined( ARMAGEDDON )
		name_stamp_stats( ch );
#endif
		{
			int ability;

			for (ability = 0; ability < MAX_ABILITY; ability++)
			{
				ch->skill_level[ability] = 1;
				ch->experience[ability] = 0;
			}
		}
		ch->top_level = 1;
		ch->hit = ch->max_hit;
		ch->hit += ch->race->hp_plus;
		ch->move = ch->max_move;
		if (ch->perm_frc > 0)
			ch->max_mana = 100 + 100 * ch->perm_frc;
		else
			ch->max_mana = 0;
		ch->mana = ch->max_mana;
		sprintf(buf, "%s", CH_RACE(ch, 0)); // PrzypadkiF
		set_title(ch, buf);

		/* Added by Narn.  Start new characters with autoexit and autgold
		 already turned on.  Very few people don't use those. */
		SET_BIT(ch->act, PLR_AUTOGOLD);
		SET_BIT(ch->act, PLR_AUTOEXIT);
		SET_BIT(ch->pcdata->flags, PCFLAG_PAGERON);

		SET_BIT(ch->deaf, CHANNEL_FLAME);

		//added by Thanos (niech sie tym graczom cos da na dziendobry)
#if defined( ARMAGEDDON )
		ch->pcdata->practices = 10;
#endif
		ch->gold = 10; // ;)
		ch->pcdata->quest_points = 0;
		//done

		/* New players don't have to earn some eq */

		/* Trog: wywalilem na prosbe Shinobi'ego
		 obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_BANNER), 0 );
		 obj_to_char( obj, ch );
		 equip_char( ch, obj, WEAR_LIGHT );
		 */

		/* armor they do though
		 obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_VEST), 0 );
		 obj_to_char( obj, ch );
		 equip_char( ch, obj, WEAR_BODY );

		 obj = create_object( get_obj_index(OBJ_VNUM_SCHOOL_SHIELD), 0 );
		 obj_to_char( obj, ch );
		 equip_char( ch, obj, WEAR_SHIELD );
		 */

		obj = create_object(get_obj_index(OBJ_VNUM_SCHOOL_DAGGER), 0);
		obj_to_char(obj, ch);
		equip_char(ch, obj, WEAR_WIELD);

		/* comlink */
		/* Trog: a moze by tu ktos kurwa zrobil #define na tego vnuma do comlink'a */
		{
			OBJ_INDEX_DATA *obj_ind = get_obj_index(10424);
			if (obj_ind != NULL)
			{
				obj = create_object(obj_ind, 0);
				obj_to_char(obj, ch);
			}
		}

		if (!sysdata.WAIT_FOR_AUTH)
		{
			char_to_room(ch, get_room_index( ROOM_VNUM_SCHOOL));
			ch->pcdata->auth_state = 3;
		}
		else
		{
			char_to_room(ch, get_room_index( ROOM_VNUM_SCHOOL));
			ch->pcdata->auth_state = 1;
			SET_BIT(ch->pcdata->flags, PCFLAG_UNAUTHED);
		}
		/* Display_prompt interprets blank as default */
		STRDUP(ch->pcdata->prompt, "");
	}
	else if (!IS_IMMORTAL(ch) && ch->pcdata->release_date > current_time)
		char_to_room(ch, get_room_index(6));
	else if (ch->in_room && !IS_IMMORTAL(ch) && !IS_SET(ch->in_room->room_flags, ROOM_SPACECRAFT) && ch->in_room != get_room_index(6))
		char_to_room(ch, ch->in_room);
	else if (ch->in_room && !IS_IMMORTAL(ch) && IS_SET(ch->in_room->room_flags, ROOM_SPACECRAFT) && ch->in_room != get_room_index(6))
	{
		SHIP_DATA *ship;

//	    for ( ship = first_ship; ship; ship = ship->next )
//	      if ( ch->in_room->vnum >= ship->firstroom && ch->in_room->vnum <= ship->lastroom )
// Thanos -- zmieniam na:
		if ((ship = ch->in_room->ship) != NULL)
			if (is_platform(ship) || ship->starsystem)
				char_to_room(ch, ch->in_room);
	}
	else
	{
		char_to_room(ch, get_room_index(wherehome(ch)));
	}

	/* Thanos - transportujemy gracza do wi�zienia */
	if (!ch->crimes.empty())
	{
		CRIME_DATA *crime;
		ROOM_INDEX_DATA *cell;

		for (auto* crime : ch->crimes)
		{
			if (crime->jail_vnum && crime->jail_end)
			{

				if (ch->in_room)
					char_from_room(ch);

				if ((cell = get_room_index(crime->jail_vnum)) == NULL)
				{
					bug(" No cell found. ch: %s", ch->name);
					char_to_room(ch, get_room_index( ROOM_VNUM_DEFAULT_CELL));
				}
				else
					char_to_room(ch, cell);

				/*
				 * Ok. Mamy w�adowanego pi�knie gracza.
				 * Teraz zajmijmy si� jego depozytem.
				 */
				load_deposit(ch);
				break;
			}
		}
	}
	/* that's it -- Thanos */

	if (get_timer(ch, TIMER_SHOVEDRAG) > 0)
		remove_timer(ch, TIMER_SHOVEDRAG);

	if (get_timer(ch, TIMER_PKILLED) > 0)
		remove_timer(ch, TIMER_PKILLED);

	if (ch->plr_home)
		load_home(ch);

	act( COL_ACTION, "$n $T do gry.", ch, NULL,
	MALE( ch ) ? "wszed�" : FEMALE(ch) ? "wesz�a" : "wesz�o", TO_ROOM);

	sprintf(log_buf, "%s!%s@%s has entered the game.", ch->name, d->user, d->host);
	if (ch->top_level < LEVEL_DEMI)
		log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
	else
		log_string_plus(log_buf, LOG_COMM, ch->top_level);

	//Added by Ratm
	if (IS_SET(ch->act, PLR_STATUS_BAR))
		init_status_bar(ch);

	if (IS_AWAKE(ch))
		do_look(ch, (char*) "auto");

	show_last(ch, ch);

	if (!IS_SET(ch->pcdata->flags, PCFLAG_NOINTRO))
	{
		note_count(ch);
		mail_count(ch);
	}

	if (ch->inquest)
	{
		MOB_INDEX_DATA *questor;

		if ((questor = get_mob_index(ch->inquest->pIndexData->questor)) != NULL)
			ch_printf(ch, NL "                "
			FB_GREEN "*** Pami�taj o zadaniu dla %s ***" EOL, questor->przypadki[1]);
		else
			bug("No questor found for %s's quest", ch->name);
	}

	//Ganis
	if (current_time - ch->pcdata->tmp_lastplayed > HOTEL_SAFE_COOLDOWN)
		ch->pcdata->hotel_safe_time = HOTEL_SAFE_TIME;
}

/*
 void nanny_get_new_password( DESCRIPTOR_DATA *d, char *argument )
 {
 CHAR_DATA *	ch	= CH( d );
 }
 */

DEF_DO_FUN( suicide )
{

	if ( IS_NPC(ch) || !ch->pcdata)
	{
		send_to_char("Tak! Na pewno! Poczekaj, mo�e jaki� gracz ci� zabije." NL, ch);
		return;
	}

	if (IS_COUNCIL(ch->name))
	{
		send_to_char("Co?! Jeste� cz�onkiem rady! Nie mo�na by� tak nieodpowiedzialnym!" NL, ch);
		return;
	}

	if (ch->top_level >= LEVEL_IMMORTAL)
	{
		send_to_char("Jeste� nie�miertelny! Podcinaj�c �y�y sprawisz sobie tylko niepotrzebny b�l." NL, ch);
		return;
	}

	if (ch->position == POS_FIGHTING)
	{
		send_to_char("Nic z tego - przecie� walczysz!" NL, ch);
		return;
	}

	if (ch->position < POS_STUNNED)
	{
		send_to_char("Twoje sytuacja jest kiepska, ale jeszcze �yjesz." NL, ch);
		return;
	}

	if (argument[0] != '\0')
	{
		send_to_char("Je�li naprawd� chcesz skasowa� swoj� posta�. Wpisz 'suicide' bez argumentu." NL, ch);
		return;
	}

	ch->desc->connected = CON_CONFIRM_SUICIDE;

	send_to_char(
	EOL NL "         "
	"* * * * * * * * * * * " FB_YELLOW "UWAGA !!!" PLAIN " * * * * * * * * * *" NL "         "
	"* Z tej sytuacji nie b�dzie ju� odwrotu, a twoja  *" NL "         "
	"* posta� zniknie z listy graczy. Czy masz pewno�� *" NL "         "
	"* �e chcesz to zrobi� ?                           *" NL "         "
	"* * * * * * * * * * * * * * * * * * * * * * * * * *" NL NL
	"Podaj swoje has�o je�li na pewno chcesz si� zabi�: ", ch);

	write_to_buffer(ch->desc, echo_off_str, 0);
	return;
}

DEF_DO_FUN( password )
{
	if (IS_NPC(ch))
	{
		send_to_char("Nie martw si�. Moby maj� u nas zawsze bezpieczny dost�p." NL, ch);
		return;
	}

	if (ch->position == POS_FIGHTING)
	{
		send_to_char("Nic z tego - przecie� walczysz!" NL, ch);
		return;
	}

	if (argument[0] != '\0')
	{
		send_to_char("Sk�adnia: password (bez argumentu)" NL, ch);
		return;
	}

	send_to_char( NL "Zmiana has�a." NL "Podaj stare has�o: ", ch);

	write_to_buffer(ch->desc, echo_off_str, 0);

	ch->desc->connected = CON_OLD_PASS_TO_CHANGE;
	return;
}

void show_motd(DESCRIPTOR_DATA *d)
{
	CHAR_DATA *ch = CH(d);

	send_to_char( CLRSCR, ch);
	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_SOUND))
		send_to_char("!!MUSIC(starwars.mid V=100)", ch);

	if (ch->top_level > 0)
		do_help(ch, (char*) "motd");
	if (ch->top_level >= 100)
		do_help(ch, (char*) "amotd");
	if (ch->top_level == 0)
		do_help(ch, (char*) "nmotd");
	if (IS_IMMORTAL(ch))
		do_help(ch, (char*) "imotd");

	send_to_char( EOL "Wci�nij [ENTER] ", ch);
	d->connected = CON_READ_MOTD;
}

void rename_done(DESCRIPTOR_DATA *d, bool accept)
{
	CHAR_DATA *ch = CH(d);

	if (!accept)
		send_to_char("Aborted." NL, CH(d));
	else
	{
		CHAR_DATA *victim;

		if ((victim = get_char_world(ch, d->tmp_ch->name)) == NULL || IS_NPC(victim))
			send_to_char("Your victim is gone..." NL "Couldn't rename." NL, ch);
		else
		{
			rename_char(victim, d->tmp_ch);

			ch_printf(victim, "Twoje imi� zosta�o zmienione na " FB_WHITE "%s" PLAIN "." NL, victim->name);
		}
		send_to_char("Done." NL, ch);
	}
	free_char(d->tmp_ch);
	d->connected = CON_PLAYING;
	return;
}

void process_rename(DESCRIPTOR_DATA *d, char *argument)
{
	CHAR_DATA *ch = CH(d);
	char arg[MIL];
	int i;

	IF_BUG(ch == NULL, "")
		return;

	IF_BUG(d->tmp_ch == NULL, "No char to rename (ch=%s)", ch->name)
		return;

	argument = one_argument(argument, arg);

	if (!str_cmp(arg, "@"))
	{
		rename_done(d, false);
		return;
	}
	else if (!str_cmp(arg, "?"))
	{
		ch_printf(ch, NL);
		for (i = 0; i < 6; i++)
			ch_printf(ch, "%s <%s>" NL, inf_name(i), d->tmp_ch->przypadki[i]);
		ch_printf(ch, NL);
	}
	else if (!str_cmp(arg, "/"))
	{
		if (d->tmp_ch->top_level <= 1)
			send_to_char("Cannot step up any further!" NL, ch);
		else
			d->tmp_ch->top_level--;
	}
	else
	{
		if (*arg)
			STRDUP(d->tmp_ch->przypadki[d->tmp_ch->top_level], capitalize(arg));
		d->tmp_ch->top_level++;
	}

	if (d->tmp_ch->top_level > 5)
	{
		rename_done(d, true);
		return;
	}

	i = d->tmp_ch->top_level;
	sprintf(arg, "<%s>", d->tmp_ch->przypadki[i]);
	ch_printf(ch, "%s " FB_WHITE "%16s" PLAIN " ", inf_name(i), arg);
	return;
}

const SWString format_char_attribute1(CHAR_DATA *ch, int przypadek)
{
	char buf[MSL] =
	{ 0 };
	char buf2[MSL] =
	{ 0 };
	//if ( !IS_DESCRIBED( ch ) )
	//{
	//	sprintf(buf,"%s",ch->race->przypadki[przypadek]);
	//	return buf;
	//}
	int attribute1 = -1, attribute2 = -1;
	if (ch->attribute1 != INT_MIN && ch->attribute1 < 0)
		attribute1 = -(ch->attribute1 + 1);
	else if (ch->attribute1 >= 0)
		attribute1 = ch->attribute1;
	if (ch->attribute2 != INT_MIN && ch->attribute2 < 0)
		attribute2 = -(ch->attribute2 + 1);
	else if (ch->attribute2 >= 0)
		attribute2 = ch->attribute2;

	if (attribute1 >= 0)
	{
		const SWString &attrib = char_attribute((desc_type) (attribute1 == 0 ? 0 : attribute1 / 100), attribute1 % 100, ch->sex, przypadek);
		swstrncpy(buf, attrib.c_str(), MSL);
	}

	if (attribute2 >= 0)
	{
		if (desc_table[attribute2 / 100][attribute2 % 100].type == FUR)
		{
			const SWString &attrib = char_attribute((desc_type) (attribute2 == 0 ? 0 : attribute2 / 100), attribute2 % 100, ch->sex,
					przypadek);
			swsnprintf(buf2, MSL, "%s %s z %s futrem", buf, CH_RACE(ch, przypadek), // Pixel: przypadkiF
			attrib.c_str());
		}
		else
		{
			const SWString &attrib = char_attribute((desc_type) (attribute2 == 0 ? 0 : attribute2 / 100), attribute2 % 100, ch->sex,
					przypadek);
			swsnprintf(buf2, MSL, "%s, %s %s", buf, attrib.c_str(), CH_RACE(ch, przypadek)); // Pixel: przypadkiF
		}
	}
	else
		swsnprintf(buf2, MSL, "%s %s", buf, CH_RACE(ch, przypadek)); // Pixel: przypadkiF
	return SWString(buf2);
}


