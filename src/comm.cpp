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
 *			 Low-level communication module			   *
 ****************************************************************************/

#if defined( WIN32 )
#include <winsock.h>
#endif

#include <errno.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include <iostream>

#include "mud.h"
#include "classes/SWException.h"
#include "classes/SWThread.h"

/*
 * Socket and TCP/IP stuff.
 */
#if !defined( WIN32 )
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <netdb.h>
#endif

/* Trog: cpu_usage() stuff. */
#if defined(__FreeBSD__) && __FreeBSD__ == 5
#include <kvm.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <paths.h>
#include <sys/user.h>
#endif

extern bool noEcho;

#if defined( WIN32 )
const   char go_ahead_str	[] = { '\0' };
void    gettimeofday    	args( ( struct timeval *tp, void *tzp ) );
#else
const char go_ahead_str[] =
{ (char) IAC, (char) GA, '\0' };
#endif

#define MAX_NEST	100

//static	OBJ_DATA *	rgObjNest	[MAX_NEST];

static unsigned char itoa64[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; //byTrog

static list<SWString> denynames;

#if defined(__FreeBSD__) && __FreeBSD__ == 5
double	get_cpu_usage	args( ( struct kinfo_proc * ) );
#endif

#if !defined( WIN32 )
void send_auth args( ( struct descriptor_data *d ) );
void read_auth args( ( struct descriptor_data *d ) );
void start_auth args( ( struct descriptor_data *d ) );
#endif
void to64 args( ( char *, long, int ) ); //byTrog
bool check_password args( ( char *_new, char *old ) );
void handle_connection args( ( DESCRIPTOR_DATA *d, char *cmdline ) );

/*  from act_info?  */
void show_condition(CHAR_DATA *ch, CHAR_DATA *victim);

//Added by Thanos: Ratm status bar
extern void init_status_bar args( ( CHAR_DATA *ch ) );
extern void update_status_bar args( ( CHAR_DATA *ch ) );

/*
 * Global variables.
 */
std::list<DESCRIPTOR_DATA*> descriptor_list;
/* d_next removed - using std::list now */
pid_t pid;
int num_descriptors;
FILE *fpReserve = NULL; /* Reserved file handle		*/
FILE *fpPid; /* Reserved pidfile handle		*/
bool mud_down; /* Shutdown			*/
bool wizlock; /* Game is wizlocked		*/
time_t boot_time;
HOUR_MIN_SEC set_boot_time_struct;
HOUR_MIN_SEC *set_boot_time;
struct tm *new_boot_time;
struct tm new_boot_struct;
char lastplayercmd[MAX_INPUT_LENGTH * 2];
time_t current_time; /* Time of this pulse		*/
int control; /* Controlling descriptor	*/
int control6; /* Controlling descriptor IPv6	*/
int conclient; /* MUDClient controlling desc	*/
int conjava; /* JavaMUD controlling desc	*/
int newdesc; /* New descriptor		*/
fd_set in_set; /* Set of desc's for reading	*/
fd_set out_set; /* Set of desc's for writing	*/
fd_set exc_set; /* Set of desc's with errors	*/
int maxdesc;
u_short port; /* Port number: Copyover -Thanos*/
bool fCopyOver;

/*
 * OS-dependent local functions.
 */
void game_loop args( ( ) );
int init_socket args( ( int port ) );
int init_socket6 args( ( uint16_t port ) );
void new_descriptor args( ( int new_desc ) );
bool read_from_descriptor args( ( DESCRIPTOR_DATA *d ) );

/*
 * Other local functions (OS-independent).
 */
bool flush_buffer args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void read_from_buffer args( ( DESCRIPTOR_DATA *d ) );
void stop_idling args( ( CHAR_DATA *ch ) );
void free_desc args( ( DESCRIPTOR_DATA *d ) );
void display_prompt args( ( DESCRIPTOR_DATA *d ) );
int make_color_sequence args( ( const char *col, char *buf,
				DESCRIPTOR_DATA *d ) );
void set_pager_input args( ( DESCRIPTOR_DATA *d,
				char *argument ) );
bool pager_output args( ( DESCRIPTOR_DATA *d ) );

/*
 * dzieki f-cji signal mozemy powiedziec 'ostatnie' slowo przed padem
 * 'Komunikacja proces�w w Unixie'
 *
 * Oto dwie mo�liwo�ci:
 * 	- Mud pad� nie znaj�c ostatniej komendy
 *            * Musi zapami�ta�, �e pad�
 *            * Musi w��czy� lastcmd_log
 *            * Musi zrobi� copyover
 * 	- Mud pad� znaj�c komend�
 *	      * Musi zapami�ta�, �e pad�
 *            * Musi WY��CZY� lastcmd_log
 *            * Musi da� si� zabi�
 *            * Musi przesta� reagowa� na pady (je�li napisa� ju� notk�)
 *              To pozwoli na zrzucenie core (crashlog = OFF)
 * Thanos
 */
void deal_with_crash()
{
	char buf[MSL];
	char bbuf[MSL];
	int num = 0;
	FILE *fp;
	bool rsn = false;

	if (sysdata.lastcmd_log) /* je�li logowa� komend� */
	{
		rsn = true;
		sprintf(bbuf, "Bardzo pan�w Admin�w przepraszam, �e pad�em, ale" NL
		"nie mia�em innego wyj�cia :( " NL
		"Pad�o mi si� gdy ostatni� wykonywan� komend� by�o:" NL);

		/* sprawdzamy jaka komenda byla ostatnio uzywana */
		if ((fp = fopen( LASTCMD_FILE, "r")) != NULL)
		{
			int ch;
			while ((ch = fgetc(fp)) != EOF && num < MSL - 1)
				buf[num++] = ch;
			fclose(fp);
		}
		buf[num] = '\0';
		snprintf(bbuf + strlen(bbuf), MSL - strlen(bbuf), "%s", buf);
		strcat(bbuf, "Ustawiam 'cset lastcmd' na OFF." NL);

		sysdata.lastcmd_log = false; /* zalogowane 	*/
		sysdata.crashlog = false; /* zapami�tane  */
	}
	else /* je�li nie logowa� komendy */
	{
		sprintf(bbuf, "Panowie. Sprawa jest taka, �e pad�em... :(" NL
		"A najgorsze jest to, �e nie wiem na czym, bo" NL
		"nie ustawili�cie mi 'cset lastcmd', wi�c sami widzicie." NL
		"Przy nast�pnym boocie sam w��cz� logowanie (obiecuj�)." NL);
		/*
		 * niech zacznie wreszcie logowac !!! ;)
		 */
		sysdata.lastcmd_log = true; /* nie zalogowane */
		sysdata.crashlog = true; /* nie zapami�tane*/
	}

	/* i napiszmy o tym notke */
	note("SW-MUD", "Admins Coders", "Crash", bbuf);

	/* wywalmy graczy  -- razem z save, �eby nie narzekali */
	if (sysdata.downtype == DOWN_CRASHED)
	{
		auto desc_snap = descriptor_list;
		for (auto* d : desc_snap)
		{
			save_char_obj(CH(d));

			write_to_descriptor(d->descriptor, "Przykro mi, ale pojawil sie nieoczekiwany problem." NL
			"Wroc do nas za momencik." NL, 0);

			close_socket(d, true);
		}

		sysdata.crashlog = false;
		save_sysdata(sysdata);
		close(control);
		log_string("Game terminated because of crash. (note sent)");

		if (!rsn)
			sprintf(bbuf, "%s MudCrash. Note Send.&", SMS_SCRIPT);
		else
			swsnprintf(bbuf, MSL, "%s MudCrash, LastCmd: %s&",
			SMS_SCRIPT, buf);

		if (system(bbuf) == -1)
			log_string("start of sms crash script failed");

		abort();
		exit(1);
	}
	else
	{
		sysdata.downtype = DOWN_CRASHED;
		save_sysdata(sysdata);
//	log_string( "Game Crashed. Trying to recover by copyover." );
		//abort();
		if (!rsn)
			sprintf(bbuf, "%s MudCrash. Copying Over.&", SMS_SCRIPT);
		else
			swsnprintf(bbuf, MSL, "%s MudCrash, Copying Over. LastCmd: %s&",
			SMS_SCRIPT, buf);
		if (system(bbuf) == -1)
			log_string("start of sms crash (copyover fallback) script failed");
		copyover();
	}
}

/* Thanos: tutaj teraz mo�emy wstawi� reakcje na np. reboot servera
 itd.
 */
void swmud_killed(int sig)
{
	sysdata.downtype = DOWN_KILLED;
	save_sysdata(sysdata);
	log_string("Game killed. Exiting.");
	exit(1);
}

void swmud_cleanup()
{
	unlink( PID_FILE);
#ifdef DMALLOC
    dmalloc_shutdown();
#endif
}

/* Trog: swmud'owa wersja terminate */
void sw_terminate()
{
	if (SWException::lastException)
	{
		cerr << "exception thrown: " << SWException::lastException->what() << endl;
	}
	else
	{
		cerr << "unknown exception thrown" << endl;
	}
}

/* Trog: swmud'owa wersja unexpected */
void sw_unexpected()
{
	if (SWException::lastException)
	{
		cerr << "unexpected exception thrown: " << SWException::lastException->what() << endl;
	}
	else
	{
		cerr << "unexpected unknown exception thrown" << endl;
	}
	::terminate();
}

/*By Ganis*/
int init_socket6(uint16_t port)
{
	struct sockaddr_in6 sa;
	int fd;
	int x = 1;

	fd = socket(AF_INET6, SOCK_STREAM, 0);
	if (fd < 0)
	{
		perror("Init_socket6: socket");
		exit(1);
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*) &x, sizeof(x)) < 0)
	{
		perror("Init_socket6: SO_REUSEADDR");
		close(fd);
		exit(1);
	}

	if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (void*) &x, sizeof(x)) < 0)
	{
		perror("Init_socket6: IPV6_V6ONLY");
		close(fd);
		exit(1);
	}

#if defined(SO_DONTLINGER) && !defined(SYSV)
	{
		struct	linger	ld;

		ld.l_onoff  = 1;
		ld.l_linger = 1000;

		if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
				(void *) &ld, sizeof(ld) ) < 0 )
		{
			perror( "Init_socket6: SO_DONTLINGER" );
			close( fd );
			exit( 1 );
		}
	}
#endif

	sa.sin6_family = AF_INET6;
	sa.sin6_port = htons(port);
	sa.sin6_flowinfo = 0;
	sa.sin6_addr = in6addr_any;
	sa.sin6_scope_id = 0;

	if (::bind(fd, (struct sockaddr*) &sa, sizeof(sa)) == -1)
	{
		perror("Init_socket6: bind");
		close(fd);
		exit(1);
	}

	if (listen(fd, 50) < 0)
	{
		perror("Init_socket6: listen");
		close(fd);
		exit(1);
	}

	return fd;
}

int init_socket(int port)
{
	char hostname[64];
	struct sockaddr_in sa;
	int x = 1;
	int fd;

	gethostname(hostname, sizeof(hostname));

#if !defined( WIN32 )
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Init_socket: socket");
		exit(1);
	}
#else
    WORD    wVersionRequested = MAKEWORD( 1, 1 );
    WSADATA wsaData;
    int err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 )
    {
	perror( "No useable WINSOCK.DLL" );
	exit( 1 );
    }

    if ( ( fd = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "Init_socket: socket" );
	exit( 1 );
    }
#endif

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*) &x, sizeof(x)) < 0)
	{
		perror("Init_socket: SO_REUSEADDR");
#if !defined( WIN32 )
		close(fd);
#else
	closesocket( fd );
#endif
		exit(1);
	}

#if defined(SO_DONTLINGER) && !defined(SYSV)
    {
	struct	linger	ld;

	ld.l_onoff  = 1;
	ld.l_linger = 1000;

	if ( setsockopt( fd, SOL_SOCKET, SO_DONTLINGER,
			(void *) &ld, sizeof(ld) ) < 0 )
	{
	    perror( "Init_socket: SO_DONTLINGER" );
#if !defined( WIN32 )
	    close( fd );
#else
	    closesocket( fd );
#endif
	    exit( 1 );
	}
    }
#endif

	memset(&sa, '\0', sizeof(sa));
#if !defined( WIN32 )
	sa.sin_family = AF_INET; /* hp->h_addrtype; */
#else
    sa.sin_family   = PF_INET;
#endif
	sa.sin_port = htons(port);

	if (::bind(fd, (struct sockaddr*) &sa, sizeof(sa)) == -1)
	{
		perror("Init_socket: bind");
#if !defined( WIN32 )
		close(fd);
#else
	closesocket( fd );
#endif
		exit(1);
	}

	if (listen(fd, 50) < 0)
	{
		perror("Init_socket: listen");
#if !defined( WIN32 )
		close(fd);
#else
	closesocket( fd );
#endif
		exit(1);
	}

	return fd;
}

/*
 * LAG alarm!							-Thoric
 */
static void caught_alarm()
{
	char buf[MAX_STRING_LENGTH];
	bug("ALARM CLOCK!");
	strcpy(buf, FB_WHITE
	"Och nie! Wielki wr�g znany tylko jako LAG powraca !!!" EOL);
	echo_to_all(buf, ECHOTAR_ALL);
	if (newdesc)
	{
		FD_CLR(newdesc, &in_set);
		FD_CLR(newdesc, &out_set);
		log_string("clearing newdesc");
	}
	game_loop();

#if !defined( WIN32 )
	close(control);
#else
    closesocket( control );
#endif

	log_string("Normal termination of game.");
	exit(0);
}

bool check_bad_desc(int desc)
{
	if (FD_ISSET(desc, &exc_set))
	{
		FD_CLR(desc, &in_set);
		FD_CLR(desc, &out_set);
		log_string("Bad FD caught and disposed.");
		return true;
	}
	return false;
}

void accept_new(int ctrl, int ctrl2)
{
	static struct timeval null_time;
	/* int maxdesc; Moved up for use with id.c as extern */

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO(&in_set);
	FD_ZERO(&out_set);
	FD_ZERO(&exc_set);
	FD_SET(ctrl, &in_set);
	FD_SET(ctrl2, &in_set);
	maxdesc = UMAX(ctrl, ctrl2);
	newdesc = 0;
	for (auto* d : descriptor_list)
	{
		maxdesc = UMAX(maxdesc, d->descriptor);
		FD_SET(d->descriptor, &in_set);
		FD_SET(d->descriptor, &out_set);
		FD_SET(d->descriptor, &exc_set);
		if (d->auth_fd != -1)
		{
			maxdesc = UMAX(maxdesc, d->auth_fd);
			FD_SET(d->auth_fd, &in_set);
			if (IS_SET(d->auth_state, FLAG_WRAUTH))
				FD_SET(d->auth_fd, &out_set);
		}
	}

	if (select(maxdesc + 1, &in_set, &out_set, &exc_set, &null_time) < 0 && errno != EINTR)
	{
		perror("accept_new: select: poll");
		exit(1);
	}

	if (FD_ISSET(ctrl, &exc_set))
	{
		bug("Exception raise on controlling descriptor %d", ctrl);
		FD_CLR(ctrl, &in_set);
		FD_CLR(ctrl, &out_set);
	}
	else if (FD_ISSET(ctrl, &in_set))
	{
		newdesc = ctrl;
		new_descriptor(newdesc);
	}

	if (FD_ISSET(ctrl2, &exc_set))
	{
		bug("Exception raise on controlling descriptor %d (IPv6)", ctrl2);
		FD_CLR(ctrl2, &in_set);
		FD_CLR(ctrl2, &out_set);
	}
	else if (FD_ISSET(ctrl2, &in_set))
	{
		newdesc = ctrl2;
		new_descriptor(newdesc);
	}
}

void process_input()
{
	char cmdline[MAX_INPUT_LENGTH];

	/*
	 * Kick out descriptors with raised exceptions
	 * or have been idle, then check for input.
	 */
	auto desc_snap2 = descriptor_list;
	for (auto* d : desc_snap2)
	{
		d->idle++; /* make it so a descriptor can idle out */
		if (FD_ISSET(d->descriptor, &exc_set))
		{
			FD_CLR(d->descriptor, &in_set);
			FD_CLR(d->descriptor, &out_set);

			if (d->character && d->connected >= CON_PLAYING)
				save_char_obj(d->character);

			d->outtop = 0;
			close_socket(d, true);
			continue;
		}
		else if ((!d->character && d->idle > 360) /* 2 mins */
		|| (((d->connected < CON_PLAYING && d->idle > 1200) /* 5 mins */
		|| d->idle > 28800) /* 2 hrs  */
		&& !IS_IMMORTAL(d->character)))
		{
			write_to_descriptor(d->descriptor, "C�... Mo�e kiedy� zechce ci si� gra�." NL, 0);
			d->outtop = 0;
			close_socket(d, true);
			continue;
		}
		else
		{
			d->fcommand = false;

			if (FD_ISSET(d->descriptor, &in_set))
			{
				d->idle = 0;

				if (d->character)
					d->character->timer = 0;

				if (!read_from_descriptor(d))
				{
					FD_CLR(d->descriptor, &out_set);

					if (d->character && (d->connected >= CON_PLAYING))
						save_char_obj(d->character);

					d->outtop = 0;
					close_socket(d, false);
					continue;
				}
			}

			/* IDENT authentication */
#if !defined( WIN32 )
			if ((d->auth_fd == -1) && (d->atimes < 20) && !str_cmp(d->user, UNKNOWN_USER))
				start_auth(d);
#endif

			if (d->auth_fd != -1)
			{
#if !defined( WIN32 )
				if (FD_ISSET(d->auth_fd, &in_set))
				{
					read_auth(d);

					/* if ( !d->auth_state )
					 check_ban( d );*/
				}
				else if (FD_ISSET(d->auth_fd, &out_set) && IS_SET(d->auth_state, FLAG_WRAUTH))
				{
					send_auth(d);
					/* if ( !d->auth_state )
					 check_ban( d );*/
				}
#endif
			}

			if (d->character && d->character->wait > 0)
			{
				--d->character->wait;
				continue;
			}

			read_from_buffer(d);

			if (d->incomm[0] != '\0')
			{
				d->fcommand = true;
				stop_idling(d->character);

				strcpy(cmdline, d->incomm);
				d->incomm[0] = '\0';

				if (d->character)
					set_cur_char(d->character);

				if (d->str_editing && d->str_editing[0])
					string_add(d->character, cmdline);
				else if (d->pagepoint)
					set_pager_input(d, cmdline);
				else
					handle_connection(d, cmdline);
			}
		}
	}
}

void process_output()
{
	auto desc_snap3 = descriptor_list;
	for (auto* d : desc_snap3)
	{

		if ((d->fcommand || d->outtop > 0) && FD_ISSET(d->descriptor, &out_set))
		{
			if (d->pagepoint && d->pagebuf)
			{
				if (!pager_output(d))
				{
					if (d->character && (d->connected >= CON_PLAYING))
						save_char_obj(d->character);

					d->outtop = 0;
					close_socket(d, false);
				}
			}
			else if (!flush_buffer(d, true))
			{
				if (d->character && (d->connected >= CON_PLAYING))
					save_char_obj(d->character);

				d->outtop = 0;
				close_socket(d, false);
			}
		}
	}
}

void game_loop()
{
	struct timeval last_time;

#if !defined( WIN32 )
	signal(SIGALRM, (void (*)(int)) caught_alarm);
#endif
	gettimeofday(&last_time, NULL);
	current_time = (time_t) last_time.tv_sec;

	/* Main loop */
	while (!mud_down)
	{
		accept_new(control, control6);

		/*
		 * Input.
		 */
		process_input();

		/*
		 * Autonomous game motion.
		 */
		update_handler();

		/*
		 * Check REQUESTS pipe
		 */
		check_requests();

		/*
		 * Output.
		 */
		process_output();

		/*
		 * Synchronize to a clock.
		 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
		 * Careful here of signed versus unsigned arithmetic.
		 */
#if !defined( WIN32 )
		{
			struct timeval now_time;
			long secDelta;
			long usecDelta;

			gettimeofday(&now_time, NULL);
			usecDelta = ((int) last_time.tv_usec) - ((int) now_time.tv_usec) + 1000000 / PULSE_PER_SECOND;
			secDelta = ((int) last_time.tv_sec) - ((int) now_time.tv_sec);
			while (usecDelta < 0)
			{
				usecDelta += 1000000;
				secDelta -= 1;
			}

			while (usecDelta >= 1000000)
			{
				usecDelta -= 1000000;
				secDelta += 1;
			}

			if (secDelta > 0 || (secDelta == 0 && usecDelta > 0))
			{
				struct timeval stall_time;

				stall_time.tv_usec = usecDelta;
				stall_time.tv_sec = secDelta;
				if (select(0, NULL, NULL, NULL, &stall_time) < 0)
				{
					perror("game_loop: select: stall");
					exit(1);
				}
			}
		}
#else
	{
	    int times_up;
	    int nappy_time;
	    struct _timeb start_time;
	    struct _timeb end_time;
	    _ftime( &start_time );
	    times_up = 0;

	    while( times_up == 0 )
	    {
		_ftime( &end_time );
		if ( ( nappy_time =
		      (int) ( 1000 *
			     (double) ( ( end_time.time - start_time.time ) +
				       ( (double) ( end_time.millitm -
						   start_time.millitm ) /
					1000.0 ) ) ) ) >=
		    (double)( 1000 / PULSE_PER_SECOND ) )
		  times_up = 1;
		else
		{
		    Sleep( (int) ( (double) ( 1000 / PULSE_PER_SECOND ) -
				  (double) nappy_time ) );
		    times_up = 1;
		}
	    }
	}
#endif

		gettimeofday(&last_time, NULL);
		current_time = (time_t) last_time.tv_sec;

		/* CHECK_LINKS removed - using std::list now */

		SWThread::updatePool();

	} /* while */

	fflush (stderr); /* To powinno zalatwic sprawe
	 gubienia logow przy copyover  -- Thanos */

	return;
}

void new_descriptor(int new_desc)
{
	char buf[MSL] =
	{ 0 };
	DESCRIPTOR_DATA *dnew;
	char hostname[MSL];
	union
	{
		struct sockaddr_in sock;
		struct sockaddr_in6 sock6;
	} sock;
	int desc;
	socklen_t size;
	bool ipv6_mode;
	int result;

	hostname[0] = '\0';
	set_alarm(20);

	ipv6_mode = new_desc == control6;

	if (ipv6_mode)
		size = sizeof(sockaddr_in6);
	else
		size = sizeof(sockaddr_in);

	if (check_bad_desc(new_desc))
	{
		set_alarm(0);
		return;
	}
	set_alarm(20);

	if ((desc = accept(new_desc, (struct sockaddr*) &sock, &size)) < 0)
	{
		perror("New_descriptor: accept");
		/*	sprintf(bugbuf, "[*****] BUG: New_descriptor: accept");
		 log_string_plus( bugbuf, LOG_COMM, sysdata.log_level ); */
		set_alarm(0);
		return;
	}

	if (check_bad_desc(new_desc))
	{
		set_alarm(0);
		return;
	}
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif

#if !defined( WIN32 )
	set_alarm(20);
	if (fcntl(desc, F_SETFL, FNDELAY) == -1)
	{
		perror("New_descriptor: fcntl: FNDELAY");
		set_alarm(0);
		return;
	}
#endif

	if (check_bad_desc(new_desc))
		return;

	CREATE(dnew, DESCRIPTOR_DATA, 1);
	init_descriptor(dnew, desc);
	dnew->ipv6 = ipv6_mode;

	if (ipv6_mode)
	{
		dnew->port = (unsigned int) ntohs(sock.sock6.sin6_port);
		inet_ntop(AF_INET6, &sock.sock6.sin6_addr, buf, sizeof(buf));
	}
	else
	{
		dnew->port = (unsigned int) ntohs(sock.sock.sin_port);
		inet_ntop(AF_INET, &sock.sock.sin_addr, buf, sizeof(buf));
	}
	STRDUP(dnew->user, UNKNOWN_USER);

	result = getnameinfo((struct sockaddr*) &sock, size, hostname, sizeof(hostname), 0, 0, 0);

	if (result)
	{
		sprintf(log_buf, "getnameinfo failed: %s", gai_strerror(result));
		log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
	}

	sprintf(log_buf, "[%d] sock.sinaddr: %s, port %hd.", dnew->descriptor, buf, dnew->port);
	log_string_plus(log_buf, LOG_COMM, sysdata.log_level);

	STRDUP(dnew->host, buf);

	for (auto* pban : ban_list)
	{
		if ((!str_prefix(pban->name, dnew->host) || !str_suffix(pban->name, hostname)) && pban->level >= LEVEL_SUPREME)
		{
			write_to_descriptor(desc, "Tw�j adres jest zbanowany." NL, 0);
			swsnprintf(buf, MSL, "Rejected connection from: %s due to ban: %s", *hostname ? hostname : dnew->host, pban->name);
			log_string_plus(buf, LOG_COMM, sysdata.log_level);
			close(desc);
			free_desc(dnew);
			set_alarm(0);
			return;
		}
	}

	if (!sysdata.NO_NAME_RESOLVING)
		STRDUP(dnew->host, (char* )(hostname[0] ? hostname : buf));
	/*
	 * Init descriptor data.
	 */

	descriptor_list.push_back(dnew);

	/*
	 * Send the greeting.
	 */
	{
		extern char *help_greeting;

		if (help_greeting[0] == '.')
			write_to_buffer(dnew, help_greeting + 1, 0);
		else
			write_to_buffer(dnew, help_greeting, 0);

		write_to_buffer(dnew, GREET_QUESTION, 0);
	}

#if !defined( WIN32 )
	start_auth(dnew); /* Start username authorization */
#endif

	if (++num_descriptors > sysdata.maxplayers)
		sysdata.maxplayers = num_descriptors;
	if (sysdata.maxplayers > sysdata.alltimemax)
	{
		sprintf(buf, "%24.24s", ctime(&current_time));
		STRDUP(sysdata.time_of_max, buf);
		sysdata.alltimemax = sysdata.maxplayers;
		sprintf(log_buf, "Broke all-time maximum player record: %d", sysdata.alltimemax);
		log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
		to_channel(log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL);
		save_sysdata(sysdata);
	}

	set_alarm(0);
	return;
}

void close_socket(DESCRIPTOR_DATA *dclose, bool force)
{
	CHAR_DATA *ch;

	/* flush outbuf */
	if (!force && dclose->outtop > 0)
		flush_buffer(dclose, false);

	/* say bye to whoever's snooping this descriptor */
	if (dclose->snoop_by)
		write_to_buffer(dclose->snoop_by, "Twoja ofiara wysz�a z gry." NL, 0);

	/* stop snooping everyone else */
	for (auto* d : descriptor_list)
		if (d->snoop_by == dclose)
			d->snoop_by = NULL;

	/* Check for switched people who go link-dead. -- Altrag */
	if (dclose->original)
	{
		if ((ch = dclose->character) != NULL)
			do_return(ch, (char*) "");
		else
		{
			bug("dclose->original without character %s", (dclose->original->name ? dclose->original->name : "unknown"));
			dclose->character = dclose->original;
			dclose->original = NULL;
		}
	}

	ch = dclose->character;

	if (dclose->character)
	{
		sprintf(log_buf, "Closing link to %s.", ch->name);
		log_string_plus(log_buf, LOG_COMM, UMAX(sysdata.log_level, ch->top_level));

		if (dclose->connected >= CON_PLAYING)
		{
			act( COL_ACTION, "$n straci�$o link do muda.", ch, NULL, NULL,
			TO_ROOM);
			ch->desc = NULL;
		}
		else
		{
			/* clear descriptor pointer to get rid of bug message in log */
			dclose->character->desc = NULL;
			free_char(dclose->character);
		}
	}

	descriptor_list.remove(dclose);

	if (dclose->descriptor == maxdesc)
		--maxdesc;

#if !defined( WIN32 )
	if (dclose->auth_fd != -1)
		close(dclose->auth_fd);
#endif

	free_desc(dclose);
	return;
}

bool read_from_descriptor(DESCRIPTOR_DATA *d)
{
	int iStart;

	/* Hold horses if pending command already. */
	if (d->incomm[0] != '\0')
		return true;

	/* Check for overflow. */
	iStart = strlen(d->inbuf);
	if (iStart >= (int) sizeof(d->inbuf) - 10)
	{
		sprintf(log_buf, "%s input overflow!", d->host);
		log_string(log_buf);
		write_to_descriptor(d->descriptor,
		NL "* * * MO�E CEG�� NA KLAWISZE PO�O�YSZ ?!? * * *" NL NL, 0);
		return false;
	}

	for (;;)
	{
		int nRead;

#if !defined( WIN32 )
		nRead = read(d->descriptor, d->inbuf + iStart, sizeof(d->inbuf) - 10 - iStart);
#else
	nRead = recv( d->descriptor, d->inbuf + iStart,
		     sizeof( d->inbuf ) - 10 - iStart, 0 );
#endif
		if (nRead > 0)
		{
			iStart += nRead;
			if (d->inbuf[iStart - 1] == '\n' || d->inbuf[iStart - 1] == '\r')
				break;
		}
		else if (nRead == 0)
		{
			sprintf(log_buf, "EOF encountered on read (%s@%s).", *d->user ? d->user : "UNKNOWN", d->host);
			log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
			return false;
		}
#if !defined( WIN32 )
		else if (errno == EWOULDBLOCK)
			break;
#else
        else if ( WSAGetLastError() == WSAEWOULDBLOCK || errno == EAGAIN )
	    break;
#endif
		else
		{
			perror("Read_from_descriptor");
			return false;
		}
	}

	d->inbuf[iStart] = '\0';
	return true;
}

char const iso_table[] =
{ (char) 0x00, (char) 0x01, (char) 0x02, (char) 0x03, (char) 0x04, (char) 0x05, (char) 0x06, (char) 0x07, (char) 0x08, (char) 0x09,
		(char) 0x0a, (char) 0x0b, (char) 0x0c, (char) 0x0d, (char) 0x0e, (char) 0x0f, (char) 0x10, (char) 0x11, (char) 0x12, (char) 0x13,
		(char) 0x14, (char) 0x15, (char) 0x16, (char) 0x17, (char) 0x18, (char) 0x19, (char) 0x1a, (char) 0x1b, (char) 0x1c, (char) 0x1d,
		(char) 0x1e, (char) 0x1f, (char) 0x20, (char) 0x21, (char) 0x22, (char) 0x23, (char) 0x24, (char) 0x25, (char) 0x26, (char) 0x27,
		(char) 0x28, (char) 0x29, (char) 0x2a, (char) 0x2b, (char) 0x2c, (char) 0x2d, (char) 0x2e, (char) 0x2f, (char) 0x30, (char) 0x31,
		(char) 0x32, (char) 0x33, (char) 0x34, (char) 0x35, (char) 0x36, (char) 0x37, (char) 0x38, (char) 0x39, (char) 0x3a, (char) 0x3b,
		(char) 0x3c, (char) 0x3d, (char) 0x3e, (char) 0x3f, (char) 0x40, (char) 0x41, (char) 0x42, (char) 0x43, (char) 0x44, (char) 0x45,
		(char) 0x46, (char) 0x47, (char) 0x48, (char) 0x49, (char) 0x4a, (char) 0x4b, (char) 0x4c, (char) 0x4d, (char) 0x4e, (char) 0x4f,
		(char) 0x50, (char) 0x51, (char) 0x52, (char) 0x53, (char) 0x54, (char) 0x55, (char) 0x56, (char) 0x57, (char) 0x58, (char) 0x59,
		(char) 0x5a, (char) 0x5b, (char) 0x5c, (char) 0x5d, (char) 0x5e, (char) 0x5f, (char) 0x60, (char) 0x61, (char) 0x62, (char) 0x63,
		(char) 0x64, (char) 0x65, (char) 0x66, (char) 0x67, (char) 0x68, (char) 0x69, (char) 0x6a, (char) 0x6b, (char) 0x6c, (char) 0x6d,
		(char) 0x6e, (char) 0x6f, (char) 0x70, (char) 0x71, (char) 0x72, (char) 0x73, (char) 0x74, (char) 0x75, (char) 0x76, (char) 0x77,
		(char) 0x78, (char) 0x79, (char) 0x7a, (char) 0x7b, (char) 0x7c, (char) 0x7d, (char) 0x7e, (char) 0x7f, (char) 0x80, (char) 0x81,
		(char) 0x82, (char) 0x83, (char) 0x84, (char) 0x85, (char) 0x86, (char) 0x87, (char) 0x88, (char) 0x89, (char) 0x8a, (char) 0x8b,
		(char) 0xa6, (char) 0x8d, (char) 0x8e, (char) 0xac, (char) 0x90, (char) 0x91, (char) 0x92, (char) 0x93, (char) 0x94, (char) 0x95,
		(char) 0x96, (char) 0x97, (char) 0x98, (char) 0x99, (char) 0x9a, (char) 0x9b, (char) 0xb6, (char) 0x9d, (char) 0x93, (char) 0xbc,
		(char) 0xa0, (char) 0xa1, (char) 0xa2, (char) 0xa3, (char) 0xa4, (char) 0xa1, (char) 0xa6, (char) 0xa7, (char) 0xa8, (char) 0xa9,
		(char) 0xaa, (char) 0xab, (char) 0xac, (char) 0xad, (char) 0xae, (char) 0xaf, (char) 0xb0, (char) 0xb1, (char) 0xb2, (char) 0xb3,
		(char) 0xb4, (char) 0xb5, (char) 0xb6, (char) 0xb7, (char) 0xb8, (char) 0xb1, (char) 0xba, (char) 0xbb, (char) 0xbc, (char) 0xbd,
		(char) 0xbe, (char) 0xbf, (char) 0xc0, (char) 0xc1, (char) 0xc2, (char) 0xc3, (char) 0xc4, (char) 0xc5, (char) 0xc6, (char) 0xc7,
		(char) 0xc8, (char) 0xc9, (char) 0xca, (char) 0xcb, (char) 0xcc, (char) 0xcd, (char) 0xce, (char) 0xcf, (char) 0xd0, (char) 0xd1,
		(char) 0xd2, (char) 0xd3, (char) 0xd4, (char) 0xd5, (char) 0xd6, (char) 0xd7, (char) 0xd8, (char) 0xd9, (char) 0xda, (char) 0xdb,
		(char) 0xdc, (char) 0xdd, (char) 0xde, (char) 0xdf, (char) 0xe0, (char) 0xe1, (char) 0xe2, (char) 0xe3, (char) 0xe4, (char) 0xe5,
		(char) 0xe6, (char) 0xe7, (char) 0x68, (char) 0xe9, (char) 0xea, (char) 0xeb, (char) 0xec, (char) 0xed, (char) 0xee, (char) 0xef,
		(char) 0xf0, (char) 0xf1, (char) 0xf2, (char) 0xf3, (char) 0xf4, (char) 0xf5, (char) 0xf6, (char) 0xf7, (char) 0xf8, (char) 0xf9,
		(char) 0xfa, (char) 0xfb, (char) 0xfc, (char) 0xfd, (char) 0xfe, (char) 0xff };

/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer(DESCRIPTOR_DATA *d)
{
	int i, j, k;

	const char winletter[(2 * INNE_LITERY_WIN)] =
	{ (char) 0xB9, '\xb1', (char) 0x9C, '\xb6', (char) 0x9F, '\xbc', (char) 0xA5, '\xa1', (char) 0x8C, '\xa6', (char) 0x8F, '\xac' };

	/*
	 * Hold horses if pending command already.
	 */
	if (d->incomm[0] != '\0')
		return;

	/*
	 * Look for at least one new line.
	 */
	for (i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r' && i < MAX_INBUF_SIZE; i++)
	{
		if (d->inbuf[i] == '\0')
			return;
	}

	/*Added by Thanos - 4 debugging */
	if (d->character && d->connected >= CON_PLAYING && sysdata.lastcmd_log)
	{
		char buf[MSL];

		sprintf(buf, "Con: %d, Pos: %d, Cmd: %s", d->connected,
		CH( d )->position, d->inbuf);

		unlink( LASTCMD_FILE);
		append_file(d->character, LASTCMD_FILE, buf);
	}

	/* Thanos	security option */
	if (d->character && d->connected >= CON_PLAYING && sysdata.log_all)
	{
		char buf[MSL];

		sprintf(buf, "Con: %d, Pos: %d, Cmd: %s", d->connected,
		CH( d )->position, d->inbuf);
		buf[strlen(buf) - 1] = '\0';

		append_file(d->character, LOGALL_FILE, buf);
	}

	/*
	 * Canonical input processing.
	 */
	for (i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++)
	{
		if (k >= 254)
		{
			write_to_descriptor(d->descriptor, "Zbyt d�uga linia." NL, 0);

			/* skip the rest of the line */
			/*
			 for ( ; d->inbuf[i] != '\0' || i>= MAX_INBUF_SIZE ; i++ )
			 {
			 if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
			 break;
			 }
			 */
			d->inbuf[i] = '\n';
			d->inbuf[i + 1] = '\0';
			break;
		}

		d->inbuf[i] = iso_table[(unsigned char) d->inbuf[i]];

		if (d->inbuf[i] == '\b' && k > 0)
			--k;
		else if ((isascii(d->inbuf[i]) && isprint(d->inbuf[i]))
				|| ((d->inbuf[i] == __FGMOD[0] || d->inbuf[i] == __BGMOD[0]) && d->character && IS_ADMIN(d->character->name)))
			d->incomm[k++] = d->inbuf[i];
		else if (ispolchar(d->inbuf[i]))
		{
			for (j = 0; j < INNE_LITERY_WIN; j++)
				if (d->inbuf[i] == winletter[2 * j])
					d->inbuf[i] = winletter[2 * j + 1];

			d->incomm[k++] = d->inbuf[i];
		}
	}

	/*
	 * Finish off the line.
	 */
	if (k == 0)
		d->incomm[k++] = ' ';
	d->incomm[k] = '\0';

	/*
	 * Deal with bozos with #repeat 1000 ...
	 */
	if (k > 1 || d->incomm[0] == '!')
	{
		if (d->incomm[0] != '!' && strcmp(d->incomm, d->inlast))
		{
			d->repeat = 0;
		}
		else
		{
			if (++d->repeat >= 20)
			{
				/*		sprintf( log_buf, "%s input spamming!", d->host );
				 log_string( log_buf );
				 */
				//added by Thanos (w walce mo�na si� powtarza�)
				if (d->character && d->character->position != POS_FIGHTING)
				{
					write_to_descriptor(d->descriptor,
					NL "* * * MO�E CEG�� NA KLAWISZE PO�O�YSZ ?!? * * *" NL NL, 0);
					//poza walk� wyrzucamy z gry tylko nieimmos�w
					if (d->repeat >= 40)
					{
						if (d->character && IS_IMMORTAL(d->character))
							d->repeat = 39; /* to mo�e by� niebezpieczne */
						else
							strcpy(d->incomm, "quit");
					}
				}
				else
					d->repeat = 0;
			}
		}
	}

	/*
	 * Do '!' substitution.
	 */
	if (d->incomm[0] == '!' && !d->str_editing)
		strcpy(d->incomm, d->inlast);
	else
		strcpy(d->inlast, d->incomm);

	/*
	 * Shift the input buffer.
	 */
	while (d->inbuf[i] == '\n' || d->inbuf[i] == '\r')
		i++;
	for (j = 0; (d->inbuf[j] = d->inbuf[i + j]) != '\0'; j++)
		;
	return;
}

//added by Thanos (dwie poni�sze funkcje rysuj� mapk�, b�d�c� pseudopromptem
//u�ywan� przy drugim typie linii statusu. Fajnie dzia�a przy X-ach i Windzie)
//jak kto� nie chce - niech nie u�ywa ;)
char* check_exit(CHAR_DATA *ch, int dir)
{
	static char buf[MAX_STRING_LENGTH];

	*buf = '\0';
	for (auto* pexit : ch->in_room->exits)
	{
		if (pexit->vdir == dir && pexit->to_room && !IS_SET(pexit->flags, EX_HIDDEN))
		{
			if (IS_SET(pexit->flags, EX_CLOSED))
				sprintf(buf, FG_GREEN "][" FG_CYAN);
			else if (IS_SET(pexit->flags, EX_WINDOW))
				sprintf(buf, FG_GREEN "()" FG_CYAN);
			else
				sprintf(buf, FG_GREEN "[]" FG_CYAN);
			return (buf);
		}
	}
	if (!(*buf) || buf[0] == '\0')
		sprintf(buf, FG_CYAN "::");
	return (buf);
}

void display_map(DESCRIPTOR_DATA *d)
{
	char buf[MSL] =
	{ 0 };
	char buf1[MSL] =
	{ 0 };
	char buf2[MSL] =
	{ 0 };
	int i;
	CHAR_DATA *ch;

	ch = d->original ? d->original : d->character;

	sprintf(buf2, "|");
	for (i = 0; i < ch->pcdata->status_width - 66; i++)
		strcat(buf2, "_");
	strcat(buf2, "|" NL);

	sprintf(buf1, " %s", check_exit(ch, 7));
	strcat(buf, buf1);
	sprintf(buf1, " %s", check_exit(ch, 0));
	strcat(buf, buf1);
	sprintf(buf1, " %s", check_exit(ch, 6));
	strcat(buf, buf1);
	swsnprintf(buf1, MSL, "    %s %s", check_exit(ch, 4), buf2);
	strcat(buf, buf1);
	sprintf(buf1, "%s  " FB_WHITE "::" PLAIN, check_exit(ch, 3));
	strcat(buf, buf1);
	sprintf(buf1, "  %s      |" NL, check_exit(ch, 1));
	strcat(buf, buf1);
	sprintf(buf1, " %s", check_exit(ch, 9));
	strcat(buf, buf1);
	sprintf(buf1, " %s", check_exit(ch, 2));
	strcat(buf, buf1);
	sprintf(buf1, " %s", check_exit(ch, 8));
	strcat(buf, buf1);
	sprintf(buf1, "    %s | " PLAIN, check_exit(ch, 5));
	strcat(buf, buf1);
	ch_printf(ch, buf);
	return;
}
//done by Thanos

/*Added by Thanos - edytory OLCa w prompcie*/
char* olc_ed_name(CHAR_DATA *ch)
{
	static char buf[10];

	buf[0] = '\0';

	if (!ch->desc)
		return &buf[0];

	strcpy(buf, bit_name(constate_types_list, ch->desc->connected));

	switch (ch->desc->connected)
	{
	case CON_HEDITOR:
	case CON_REDITOR:
	case CON_PEDITOR:
	case CON_SSEDITOR:
	case CON_AEDITOR:
	case CON_OEDITOR:
	case CON_MEDITOR:
	case CON_RPEDITOR:
	case CON_OPEDITOR:
	case CON_MPEDITOR:
	case CON_QEDITOR:
	case CON_SHEDITOR:
	case CON_SKEDITOR:
	case CON_CEDITOR:
	case CON_PROEDITOR:
	case CON_RAEDITOR:
	case CON_MATEDITOR:
	case CON_DIALOGEDIT:
	case CON_TCEDITOR:
		sprintf(buf, "%sor", bit_name(constate_types_list, ch->desc->connected));
		break;
	default:
		sprintf(buf, " ");
		break;
	}

	return buf;
}

const SWString olc_ed_vnum(CHAR_DATA *ch)
{
	QUEST_INDEX_DATA *pQuest;
	ROOM_INDEX_DATA *pRoom;
	AREA_DATA *pArea;
	PLANET_DATA *pPlanet;
	SPACE_DATA *pStarsystem;
	HELP_DATA *pHelp;
	OBJ_INDEX_DATA *pObj;
	MOB_INDEX_DATA *pMob;
	SHIP_INDEX_DATA *pShrec;
	SKILLTYPE *pSkill;
	CMDTYPE *pCmd;
	PROJECT_DATA *pPro;
	RACE_DATA *pRace;
	MATERIAL_DATA *pMat;
	DIALOG_DATA *pDialogData;
	TURBOCAR *pTurbocar;
	char buf[(MAX_INT32_STR_LEN * 2) + 1] =
	{ 0 };

	if (!ch->desc)
		return SWString();

	switch (ch->desc->connected)
	{
	case CON_REDITOR:
		pRoom = ch->in_room; /*(ROOM_INDEX_DATA *) ch->desc->olc_editing;*/
		sprintf(buf, "%d", pRoom ? pRoom->vnum : 0);
		break;
	case CON_HEDITOR:
		pHelp = (HELP_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%s" PROMPT_COL_N, pHelp ? pHelp->keyword : "???");
		break;
	case CON_PEDITOR:
		pPlanet = (PLANET_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%s", pPlanet ? pPlanet->filename : "???");
		break;
	case CON_SSEDITOR:
		pStarsystem = (SPACE_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%s", pStarsystem ? pStarsystem->name : "???");
		break;
	case CON_AEDITOR:
		pArea = (AREA_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%s", pArea ? pArea->filename : "???");
		break;
	case CON_OEDITOR:
		pObj = (OBJ_INDEX_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%d", pObj ? pObj->vnum : 0);
		break;
	case CON_MEDITOR:
		pMob = (MOB_INDEX_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%d", pMob ? pMob->vnum : 0);
		break;
	case CON_MPEDITOR:
		pMob = (MOB_INDEX_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%d(%d)", pMob ? pMob->vnum : 0, ch->pcdata->mprog_edit + 1);
		break;
	case CON_OPEDITOR:
		pObj = (OBJ_INDEX_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%d(%d)", pObj ? pObj->vnum : 0, ch->pcdata->mprog_edit + 1);
		break;
	case CON_RPEDITOR:
		pRoom = (ROOM_INDEX_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%d(%d)", pRoom ? pRoom->vnum : 0, ch->pcdata->mprog_edit + 1);
		break;
	case CON_QEDITOR:
		pQuest = (QUEST_INDEX_DATA*) ch->desc->olc_editing;
		if (ch->pcdata && ch->pcdata->line_nr <= 0)
			sprintf(buf, "%d", pQuest ? pQuest->quest_id : 0);
		else
			sprintf(buf, "%d(%d)", pQuest ? pQuest->quest_id : 0, ch->pcdata ? ch->pcdata->line_nr : 0);
		break;
	case CON_SHEDITOR:
		pShrec = (SHIP_INDEX_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%d", pShrec ? pShrec->vnum : 0);
		break;
	case CON_SKEDITOR:
		pSkill = (SKILLTYPE*) ch->desc->olc_editing;
		sprintf(buf, "%s", pSkill ? pSkill->name : "???");
		break;
	case CON_CEDITOR:
		pCmd = (CMDTYPE*) ch->desc->olc_editing;
		sprintf(buf, "%s", pCmd ? pCmd->name : "???");
		break;
	case CON_PROEDITOR:
		pPro = (PROJECT_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%d", pPro ? pPro->effect->vnum : 0);
		break;
	case CON_RAEDITOR:
		pRace = (RACE_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%s", pRace ? pRace->name : "???");
		break;
	case CON_MATEDITOR:
		pMat = (MATERIAL_DATA*) ch->desc->olc_editing;
		sprintf(buf, "%s", pMat ? pMat->name : "???");
		break;
	case CON_DIALOGEDIT:
		pDialogData = ((DIALOG_NODE*) ch->desc->olc_editing)->pBase;
		sprintf(buf, "%s", pDialogData ? pDialogData->name : "???");
		break;
	case CON_TCEDITOR:
		pTurbocar = (TURBOCAR*) ch->desc->olc_editing;
		sprintf(buf, "%s", pTurbocar ? pTurbocar->name : "???");
		break;
	default:
		sprintf(buf, " ");
		break;
	}

	return SWString(buf);
}
/*done by Thanos*/

/*
 * Low level output function.
 */
bool flush_buffer(DESCRIPTOR_DATA *d, bool fPrompt)
{
	char buf[MAX_INPUT_LENGTH];
	extern bool mud_down;
	CHAR_DATA *ch;

	ch = CH(d);

	if (ch && ch->fighting && ch->fighting->who)
		show_condition(ch, ch->fighting->who);

	/*
	 * Bust a prompt.
	 */
	if (fPrompt && !mud_down && d->connected >= CON_PLAYING && d->connected < CON_CONFIRM_SUICIDE)
	{
		ch = d->original ? d->original : d->character;

		if (IS_SET(ch->act, PLR_STATUS_BAR))
		{
			write_to_descriptor(d->descriptor, VT_SAVECURSOR, 0);
			sprintf(buf, "\e[%d;1H", ch->pcdata->status_height - 1);
			write_to_descriptor(d->descriptor, buf, 0);

			update_status_bar(ch);

			if ( IS_SET(ch->act, PLR_BLANK) && !d->str_editing)
				write_to_descriptor(d->descriptor, NL, 2);

			if (d->fcommand)
			{
				sprintf(buf, "\e[%d;1H\e[J", ch->pcdata->status_height + 1);
				write_to_buffer(d, buf, 0);

				if (ch->pcdata->status_type == STATUS_TYPE_TWO)
					display_map(d);

				if (IS_SET(ch->act, PLR_PROMPT))
					display_prompt(d);

				if (IS_SET(ch->act, PLR_TELNET_GA))
					write_to_buffer(d, go_ahead_str, 0);
			}
			else
			{
				if ( IS_SET(ch->act,
						PLR_PROMPT) || ch->pcdata->status_type == STATUS_TYPE_TWO)
				{
					sprintf(buf, "\e[%d;1H", ch->pcdata->status_height + 1);
					write_to_buffer(d, buf, 0);

					if (ch->pcdata->status_type == STATUS_TYPE_TWO)
						display_map(d);

					if (IS_SET(ch->act, PLR_PROMPT))
						display_prompt(d);

					if (IS_SET(ch->act, PLR_TELNET_GA))
						write_to_buffer(d, go_ahead_str, 0);
				}
				write_to_buffer(d, VT_RESTORECURSOR, 0);
			}

		}
		else
		{
			if ( IS_SET(ch->act, PLR_BLANK) && !d->str_editing)
				write_to_buffer(d, NL, 2);

			if (IS_SET(ch->act, PLR_PROMPT))
				display_prompt(d);

			if (IS_SET(ch->act, PLR_TELNET_GA))
				write_to_buffer(d, go_ahead_str, 0);
		}
	}

	/*
	 * Short-circuit if nothing to write.
	 */
	if (d->outtop == 0)
		return true;

	/*
	 * Snoop-o-rama.
	 */
	if (d->snoop_by)
	{
		/* without check, 'force mortal quit' while snooped caused crash, -h */
		if (d->character && d->character->name)
		{
			/* Show original snooped names. -- Altrag */
			if (d->original && d->original->name)
				sprintf(buf, "%s (%s)", d->character->name, d->original->name);
			else
				sprintf(buf, "%s", d->character->name);
			write_to_buffer(d->snoop_by, buf, 0);
		}
		write_to_buffer(d->snoop_by, "% ", 2);
		write_to_buffer(d->snoop_by, d->outbuf, d->outtop);
	}

	/*
	 * OS-dependent output.
	 */
	if (!write_to_descriptor(d->descriptor, d->outbuf, d->outtop))
	{
		d->outtop = 0;
		return false;
	}
	else
	{
		d->outtop = 0;
		return true;
	}
}

/*
 * Append onto an output buffer.
 */
int write_to_buffer(DESCRIPTOR_DATA *d, const char *txt, int length)
{
	int i, j;
	const char letter[(2 * INNE_LITERY_WIN)] =
	{ '\xb1', '\xb9', '\xb6', '\x9c', '\xbc', '\x9f', '\xa1', '\xa5', '\xa6', '\x8c', '\xac', '\x8f' };

	const char noplletter[2 * INNE_LITERY_NOPL] =
	{ '\xb1', 'a', '\xe6', 'c', '\xea', 'e', '\xb3', 'l', '\xf1', 'n', '\xf3', 'o', '\xb6', 's', '\xbc', 'z', '\xbf', 'z', '\xa1', 'A', '\xc6', 'C', '\xca', 'E', '\xa3', 'L', '\xd1',
			'N', '\xd3', 'O', '\xa6', 'S', '\xac', 'Z', '\xaf', 'Z' };

//tutaj    char 	*txt1;
	char txt1[MSL * 4];

	if (!d)
	{
		bug("NULL descriptor");
		return -1;
	}

	/*
	 * Normally a bug... but can happen if loadup is used.
	 */
	if (!d->outbuf)
		return -1;

	/*
	 * Find length in case caller didn't.
	 */
	if (length <= 0)
		length = strlen(txt);

	/* Uncomment if debugging or something
	 if ( length != strlen(txt) )
	 {
	 bug( "Write_to_buffer: length(%d) != strlen(txt)!", length );
	 length = strlen(txt);
	 }
	 */

	// added by Thanos (w�a�ciwie Ratm)
	txt1[0] = '\0';
	strcat(txt1, txt);

	if (d->character)
	{
		if (d->character->charset == CHARSET_NOPOL)	// nopl
		{
			for (i = 0; i <= length; i++)
				for (j = 0; j < INNE_LITERY_NOPL; j++)
					if (txt1[i] == noplletter[2 * j])
						txt1[i] = noplletter[2 * j + 1];
		}
		else if (d->character->charset == CHARSET_WIN)
			for (i = 0; i <= length; i++)
				for (j = 0; j < INNE_LITERY_WIN; j++)
					if (txt1[i] == letter[2 * j])
						txt1[i] = letter[2 * j + 1];
	}
	//done

	/*
	 * Initial \n\r if needed.
	 */
	if (d->outtop == 0 && !d->fcommand)
	{
		CHAR_DATA *ch;

		ch = d->original ? d->original : d->character;

		if (ch && !IS_SET(ch->act, PLR_STATUS_BAR))
		{

			d->outbuf[0] = '\n';
			d->outbuf[1] = '\r';
			d->outtop = 2;
		}
	}

	/*
	 * Expand the buffer as needed.
	 */
	while (d->outtop + length >= (int) d->outsize)
	{
		if (d->outsize > 32000)
		{
			/* empty buffer */
			d->outtop = 0;

			SWString charName(d->character ? d->character->name : "???");
			close_socket(d, true);
			bug("Buffer overflow. Closing (%s).", charName.c_str());

			return -1;
		}
		d->outsize *= 2;
		RECREATE(d->outbuf, char, d->outsize);
	}

	/*
	 * Copy.
	 */
	strncpy(d->outbuf + d->outtop, txt1, length);
	d->outtop += length;
	d->outbuf[d->outtop] = '\0';
	return length;
}

/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 *   try lowering the max block size.
 */
bool write_to_descriptor(int desc, const char *txt, int length)
{
	int iStart;
	int nWrite;
	int nBlock;

	if (length <= 0)
		length = strlen(txt);

	for (iStart = 0; iStart < length; iStart += nWrite)
	{
		nBlock = UMIN(length - iStart, 4096);
#if !defined( WIN32 )
		if ((nWrite = write(desc, txt + iStart, nBlock)) < 0)
#else
	if ( ( nWrite = send( desc, txt + iStart, nBlock , 0) ) < 0 )
#endif
		{
			perror("Write_to_descriptor");
			return false;
		}
	}

	return true;
}

void load_denynames()
{
}

/*
 * Parse a name for acceptability.
 */
bool check_parse_name(char *name)
{
	/*
	 * Reserved words.
	 */
	if (is_name(name, "all auto someone immortal self god supreme demigod clans"
			" ass fuck shit piss crap quit ktos kto� pipa dupa builders"
			" kutas immo admin koder admins koders coder coders council"
			" ips blacksun jedi sith kurwa qrwa luke lando imperator"
			" koordynator senator mistrz mastash master chuj huj qtas"
			" pizda ship ships yoda egzar khun mob supermob batman superman"
			" spiderman szturmowiec droid public leia grievous ackbar padme"
			" amidala bail organa wedge bane darth tavion garm ponda depa"
			" billaba jar jarjar jolee bossk joruus joruss chewbacca biggs"
			" dooku count moff durge jango boba fett borsk kit fisto greedo"
			" nute gunray corran ysanne isanne jabba mara jade talon karrde"
			" plo koon rahm exar kun lobot malak galen marek maul mothma gonk"
			" nihilus nien nunb barriss baris carth onasi oola canderous ordo"
			" ozzel palpatine panaka pellaeon thrawn sebulba aayla"
			" anakin jacen jaina han tarkin vader mace windu zam wesell winter"
			" xizor yaddle yoda"))
		return false;

	for (list<SWString>::const_iterator it = denynames.begin(); it != denynames.end(); it++)
	{
		if (*it == name)
			return false;
	}

	if (is_name(name, sysdata.denied_nicks))
		return false;

	/*
	 * Length restrictions.
	 */
	if (strlen(name) < 3)
		return false;

	if (strlen(name) > 12)
		return false;

	/*
	 * Alphanumerics only.
	 * Lock out IllIll twits.
	 */
	{
		char *pc;
		bool fIll;

		fIll = true;
		for (pc = name; *pc != '\0'; pc++)
		{
			if (!isalpha(*pc))
				return false;
			if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l')
				fIll = false;
		}

		if (fIll)
			return false;
	}

	/*
	 * Code that followed here used to prevent players from naming
	 * themselves after mobs... this caused much havoc when new areas
	 * would go in...
	 */

	return true;
}

/*
 * Look for link-dead player to reconnect.
 */
int check_reconnect(DESCRIPTOR_DATA *d, char *name, bool fConn)
{
	for (auto* ch : char_list)
	{
		if (!IS_NPC(ch) && (!fConn || !ch->desc) && ch->name && !str_cmp(name, ch->name))
		{
			if (fConn && ch->switched)
			{
				write_to_buffer(d, "Ta postac jest juz w grze." NL "Imie: ", 0);
				d->connected = CON_GET_NAME;
				if (d->character)
				{
					/* clear descriptor pointer to get rid of bug message in log */
					d->character->desc = NULL;
					free_char(d->character);
					d->character = NULL;
				}
				return BERR;
			}
			if (fConn == false)
				STRDUP(d->character->pcdata->pwd, ch->pcdata->pwd);
			else
			{
				/* clear descriptor pointer to get rid of bug message in log */
				d->character->desc = NULL;
				free_char(d->character);
				d->character = ch;
				ch->desc = d;
				ch->timer = 0;

//Added by Ratm
				if ( IS_SET(ch->act, PLR_STATUS_BAR) && d->connected >= CON_PLAYING)
					init_status_bar(ch);

				send_to_char("Powt�rne po��czenie." NL, ch);
				act( COL_ACTION, "$n po��czy�$o si� na nowo.", ch, NULL, NULL,
				TO_ROOM);
				sprintf(log_buf, "%s!%s@%s reconnected.", ch->name, d->user, d->host);
				log_string_plus(log_buf, LOG_COMM, UMAX(sysdata.log_level, ch->top_level));
				/*
				 if ( ch->top_level < LEVEL_SAVIOR )
				 to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
				 */
				d->connected = CON_PLAYING;
			}
			return 1;
		}
	}

	return 0;
}

/*
 * Check if already playing.
 */

bool check_multi(DESCRIPTOR_DATA *d, char *name)
{
	for (auto* dold : descriptor_list)
	{
		if (dold != d && (dold->character || dold->original) && str_cmp(name, dold->original ? dold->original->name : dold->character->name)
				&& !str_cmp(dold->host, d->host)
				&& (d->user && dold->user
						&& (!str_cmp(dold->user, d->user) && str_cmp(d->user, UNKNOWN_USER) && str_cmp(d->user, "UNKNOWN"))))
		{

			/* Hmmm .... a czyje te ipki?         Thanos */
			const char *ok = "194.234.177";
			const char *ok2 = "209.183.133.229";
			int iloop;

			if (get_trust(d->character) >= LEVEL_SUPREME || get_trust(dold->original ? dold->original : dold->character) >= LEVEL_SUPREME)
				return false;
			for (iloop = 0; iloop < 11; iloop++)
			{
				if (ok[iloop] != d->host[iloop])
					break;
			}
			if (iloop >= 10)
				return false;
			for (iloop = 0; iloop < 11; iloop++)
			{
				if (ok2[iloop] != d->host[iloop])
					break;
			}
			if (iloop >= 10)
				return false;
			write_to_buffer(d, "Przykro mi, ale granie multiplaying jest u nas zakazane..." NL
			"Wyjd� najpierw z gry inn� postaci�." NL, 0);
			sprintf(log_buf, "%s attempting to multiplay with %s.", dold->original ? dold->original->name : dold->character->name,
					d->character->name);
			log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
			d->character = NULL;
			free_char(d->character);
			return true;
		}
	}

	return false;

}

int check_playing(DESCRIPTOR_DATA *d, char *name, bool kick)
{
	CHAR_DATA *ch;
	int cstate;

	for (auto* dold : descriptor_list)
	{
		if (dold != d && (dold->character || dold->original)
				&& !str_cmp(name, dold->original ? dold->original->name : dold->character->name))
		{
			cstate = dold->connected;
			ch = dold->original ? dold->original : dold->character;
			if (!ch->name || (cstate < CON_PLAYING))
			{
				write_to_buffer(d, "Powt�rne po��czenie nie powiod�o si�. Spr�buj jeszcze raz." NL, 0);
				sprintf(log_buf, "%s already connected.", ch->name);
				log_string_plus(log_buf, LOG_COMM, sysdata.log_level);
				return BERR;
			}
			if (!kick)
				return 1;
//Added by Ratm
			if (IS_SET(d->character->act, PLR_STATUS_BAR))
				init_status_bar(d->character);
			write_to_buffer(dold, "�egnaj stary nieu�ywany linku!" NL, 0);
			close_socket(dold, false);
			/* clear descriptor pointer to get rid of bug message in log */
			d->character->desc = NULL;
			free_char(d->character);
			d->character = ch;
			ch->desc = d;
			ch->timer = 0;
			if (ch->switched)
				do_return(ch->switched, (char*) "");
			ch->switched = NULL;
			send_to_char("Powt�rne po��czenie." NL, ch);
			act( COL_ACTION, "$n po��czy�$o si� na nowo.", ch, NULL, NULL,
			TO_ROOM);
			sprintf(log_buf, "%s!%s@%s reconnected, kicking off old link.", ch->name, d->user, d->host);
			log_string_plus(log_buf, LOG_COMM, UMAX(sysdata.log_level, ch->top_level));
			/*
			 if ( ch->top_level < LEVEL_SAVIOR )
			 to_channel( log_buf, CHANNEL_MONITOR, "Monitor", ch->top_level );
			 */
			d->connected = cstate;
			return 1;
		}
	}

	return 0;
}

void stop_idling(CHAR_DATA *ch)
{
	if (!ch || !ch->desc || ch->desc->connected != CON_PLAYING || !ch->was_in_room || ch->in_room != get_room_index( ROOM_VNUM_LIMBO))
		return;

	ch->timer = 0;
	char_from_room(ch);
	char_to_room(ch, ch->was_in_room);
	ch->was_in_room = NULL;
	act( COL_ACTION, "$n powr�ci�$o z letargu.", ch, NULL, NULL, TO_ROOM);
	return;
}

/*
 * Write to one char. Commented out in favour of colour
 *
 void send_to_char( const char *txt, CHAR_DATA *ch )
 {
 if ( !ch )
 {
 bug( "Send_to_char: NULL *ch" );
 return;
 }
 if ( txt && ch->desc )
 write_to_buffer( ch->desc, txt, strlen(txt) );
 return;
 }
 */

/*
 * Same as above, but converts &color codes to ANSI sequences..
 */
void send_to_char_color(const char *txt, CHAR_DATA *ch)
{
	DESCRIPTOR_DATA *d;
	char *colstr;
	const char *prevstr = txt;
	char colbuf[20] =
	{ 0 };
	int ln;

	IF_BUG(ch == NULL, "(%s)", txt)
		return;

	if (!txt || !ch->desc)
		return;

	/* Trog: bugfix, jak goscia rozlaczy wczesniej, to ch jest, a ch->desc nie! */
	if (!(d = ch->desc))
	{
		bug("no ch->desc");
		return;
	}

	/* Clear out old color stuff */
	/*  make_color_sequence(NULL, NULL, NULL);*/
	int written;
	while ((colstr = strpbrk((char*) prevstr, __FGMOD __BGMOD)) != NULL)
	{

		if (colstr > prevstr)
		{
			written = write_to_buffer(d, prevstr, (colstr - prevstr));
			IF_BUG(written == -1, "descriptor no longer valid")
				return;

		}

		ln = make_color_sequence(colstr, colbuf, d);

		if (ln < 0)
		{
			prevstr = colstr + 1;
			break;
		}
		else if (ln > 0)

			write_to_buffer(d, colbuf, ln);

		prevstr = colstr + 2;
	}

	if (*prevstr)
		write_to_buffer(d, prevstr, 0);
	return;
}

void write_to_pager(DESCRIPTOR_DATA *d, const char *txt, int length)
{

	int i, j;
	char letter[(2 * INNE_LITERY_WIN)] =
	{ '\xb1', '\xb9', '\xb6', '\x9c', '\xbc', '\x9f', '\xa1', '\xa5', '\xa6', '\x8c', '\xac', '\x8f' };

	char noplletter[2 * INNE_LITERY_NOPL] =
	{ '\xb1', 'a', '\xe6', 'c', '\xea', 'e', '\xb3', 'l', '\xf1', 'n', '\xf3', 'o', '\xb6', 's', '\xbc', 'z', '\xbf', 'z', '\xa1', 'A', '\xc6', 'C', '\xca', 'E', '\xa3', 'L', '\xd1',
			'N', '\xd3', 'O', '\xa6', 'S', '\xac', 'Z', '\xaf', 'Z' };

	//  char 	*txt1;
	char txt1[MSL * 8];

	if (length <= 0)
		length = strlen(txt);
	if (length == 0)
		return;

	// added by Thanos (w�a�ciwie Ratm)
	txt1[0] = '\0';
	strcat(txt1, txt);

	if (d->character)
	{
		if (d->character->charset == CHARSET_NOPOL)	// nopl
		{
			for (i = 0; i <= length; i++)
				for (j = 0; j < INNE_LITERY_NOPL; j++)
					if (txt1[i] == noplletter[2 * j])
						txt1[i] = noplletter[2 * j + 1];
		}
		else if (d->character->charset == CHARSET_WIN)
			for (i = 0; i <= length; i++)
				for (j = 0; j < INNE_LITERY_WIN; j++)
					if (txt1[i] == letter[2 * j])
						txt1[i] = letter[2 * j + 1];
	}
	//done

	if (!d->pagebuf)
	{
		d->pagesize = MAX_STRING_LENGTH;
		CREATE(d->pagebuf, char, d->pagesize);
	}
	if (!d->pagepoint)
	{
		d->pagepoint = d->pagebuf;
		d->pagetop = 0;
		d->pagecmd = '\0';
	}
	if (d->pagetop == 0 && !d->fcommand)
	{
		d->pagebuf[0] = '\n';
		d->pagebuf[1] = '\r';
		d->pagetop = 2;
	}
	while (d->pagetop + length >= (int) d->pagesize)
	{
		if (d->pagesize > MAX_PAGER)
		{
			bug("Pager overflow. Ignoring.");
			d->pagetop = 0;
			d->pagepoint = NULL;
			DISPOSE(d->pagebuf);
			d->pagesize = MAX_STRING_LENGTH;
			return;
		}
		d->pagesize *= 2;
		RECREATE(d->pagebuf, char, d->pagesize);
	}
	strncpy(d->pagebuf + d->pagetop, txt1, length);
	d->pagetop += length;
	d->pagebuf[d->pagetop] = '\0';
	return;
}

void send_to_pager_color(const char *txt, CHAR_DATA *ch)
{
	DESCRIPTOR_DATA *d;
	char *colstr;
	const char *prevstr = txt;
	char colbuf[20];
	int ln;

	IF_BUG(ch == NULL, "")
		return;

	if (!txt || !ch->desc)
		return;

	d = ch->desc;
	ch = CH(d);

	if ( IS_NPC(ch) || !ch->pcdata || !IS_SET(ch->pcdata->flags, PCFLAG_PAGERON))
	{
		send_to_char_color(txt, d->character);
		return;
	}
	/* Clear out old color stuff */
	/*  make_color_sequence(NULL, NULL, NULL);*/
	while ((colstr = strpbrk((char*) prevstr, __FGMOD __BGMOD)) != NULL)
	{
		if (colstr > prevstr)
			write_to_pager(d, prevstr, (colstr - prevstr));

		ln = make_color_sequence(colstr, colbuf, d);
		if (ln < 0)
		{
			prevstr = colstr + 1;
			break;
		}
		else if (ln > 0)
			write_to_pager(d, colbuf, ln);
		prevstr = colstr + 2;
	}
	if (*prevstr)
		write_to_pager(d, prevstr, 0);
	if (colstr)
		DISPOSE(colstr);	//added by Thanos
	return;
}

/* source: EOD, by John Booth <???> */
void ch_printf(CHAR_DATA *ch, const char *fmt, ...)
{
	char buf[MAX_STRING_LENGTH]; /* better safe than sorry */
	va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);
	send_to_char(buf, ch);
}

void pager_printf(CHAR_DATA *ch, const char *fmt, ...)
{
	char buf[MSL] =
	{ 0 };
	va_list args;
	int written;

	va_start(args, fmt);
	written = vsnprintf(buf, MSL, fmt, args);
	va_end(args);

	IF_BUG(written >= MSL, "characters ommited due to overflow")
		; // @suppress("Suspicious semicolon")

	send_to_pager(buf, ch);
}

char* obj_short(OBJ_DATA *obj)
{
	static char buf[MAX_STRING_LENGTH];

	if (obj->count > 1)
	{
		sprintf(buf, "%s (%d)", obj->przypadki[0], obj->count);
		return (buf);
	}
	return (obj->przypadki[0]);
}

/*
 * The primary output interface for formatted output.
 */
/* Major overhaul. -- Alty */
char* act_string(const char *color, const char *format, CHAR_DATA *to, CHAR_DATA *ch, const void *arg1, const void *arg2)
{
	//added by Thanos (polskie zaimki)
	static const char *const he_she[] =
	{ "to", "on", "ona" };	// E
	static const char *const him_her[] =
	{ "temu", "mu", "jej" };	// M
	static const char *const his_her[] =
	{ "tego", "jego", "jej" };	// S
	static const char *const in_him_her[] =
	{ "tym", "nim", "niej" };	// G
	static const char *const at_him_her[] =
	{ "nie", "niego", "ni�" };	// R
	static const char *const with_him_her[] =
	{ "tym", "nim", "ni�" };	// W
	static const char *const from_him_her[] =
	{ "tego", "niego", "niej" };	// F
	static const char *const i_see_him_her[] =
	{ "to", "go", "j�" };	// I
	static const char *const soc[] =
	{ "o", "", "a" };	// O
	static const char *const socy[] =
	{ "e", "y", "a" };	// Y
	static const char *const socp[] =
	{ "e", "", "a" };	// X
	static const char *const socr[] =
	{ "o", "e", "a" };	// A
	static const char *const socz[] =
	{ "ie", "i", "a" };	// Z
	//done

	static char buf[MAX_STRING_LENGTH];
	char fname[MAX_INPUT_LENGTH];
	char *point = buf;
	const char *str = format;
	const char *i = 0;
	CHAR_DATA *vch = (CHAR_DATA*) arg2;
	OBJ_DATA *obj1 = (OBJ_DATA*) arg1;
	OBJ_DATA *obj2 = (OBJ_DATA*) arg2;
	char *col;

	while (*str != '\0')
	{
		col = (char*) color;
		if (*str != '$')
		{
			*point++ = *str++;
			continue;
		}

		++str;
		if (!arg2 && *str >= 'A' && *str <= 'Z')
		{
			bug("missing arg2 for code %c: [%s]", *str, format);
			i = " <@@@> ";
		}
		else
		{
			switch (*str)
			{
			default:
				bug("bad code %c.", *str);
				i = " <@@@> ";
				break;
			case 't':
				i = (char*) arg1;
				break;
			case 'T':
				i = (char*) arg2;
				break;

			case 'n':
				if (str[1] == '$')
				{
					if (str[2] >= '0' && str[2] <= '5')
					{
						str += 2;
						i = (to ? PERS(ch, to, (*str - 48)) : NAME(ch, (*str - 48)));
					}
					else if (str[2] == 'R')
					{
						str += 2;
						i = (to && can_see(to, ch)) ? ( IS_NPC( ch ) ? ch->przypadki[0] : ch->name) : "Kto�";
					}
					else if (str[2] == 'x')
					{
						str += 2;
						i = does_knows(to, ch) ? ch->przypadki[0] : "Kto�";
					}
				}
				else
					i = (to ? PERS(ch, to, 0) : NAME(ch, 0));
				break;

			case 'N':
				if (str[1] == '$')
				{
					if (str[2] >= '0' && str[2] <= '5')
					{
						str += 2;
						i = (to ? PERS(vch, to, (*str - 48)) : NAME(vch, (*str - 48)));
					}
				}
				else if (str[2] == 'R')
				{
					str += 2;
					i = (to && can_see(to, vch)) ? ( IS_NPC( vch ) ? vch->przypadki[0] : vch->name) : "Kto�";
				}
				else
					i = (to ? PERS(vch, to, 0) : NAME(vch, 0));
				break;
//done
			case 'e':
				if (ch->sex > 2 || ch->sex < 0)
				{
					bug("player %s has sex set at %d!", ch->name, ch->sex);
					i = "to";
				}
				else
					i = he_she[URANGE(0, ch->sex, 2)];
				break;
			case 'E':
				if (vch->sex > 2 || vch->sex < 0)
				{
					bug("player %s has sex set at %d!", vch->name, vch->sex);
					i = "to";
				}
				else
					i = he_she[URANGE(0, vch->sex, 2)];
				break;
			case 'm':
				if (ch->sex > 2 || ch->sex < 0)
				{
					bug("player %s has sex set at %d!", ch->name, ch->sex);
					i = "temu";
				}
				else
					i = him_her[URANGE(0, ch->sex, 2)];
				break;
			case 'M':
				if (vch->sex > 2 || vch->sex < 0)
				{
					bug("player %s has sex set at %d!", vch->name, vch->sex);
					i = "temu";
				}
				else
					i = him_her[URANGE(0, vch->sex, 2)];
				break;
			case 's':
				if (ch->sex > 2 || ch->sex < 0)
				{
					bug("player %s has sex set at %d!", ch->name, ch->sex);
					i = "tego";
				}
				else
					i = his_her[URANGE(0, ch->sex, 2)];
				break;
			case 'S':
				if (vch->sex > 2 || vch->sex < 0)
				{
					bug("player %s has sex set at %d!", vch->name, vch->sex);
					i = "tego";
				}
				else
					i = his_her[URANGE(0, vch->sex, 2)];
				break;
			case 'q':
				i = (to == ch) ? "" : "";
				break;
			case 'Q':
				i = (to == ch) ? "twoje" : his_her[URANGE(0, ch->sex, 2)];
				break;
				//Added by Thanos (zaimki)
			case 'g':
				i = in_him_her[URANGE(0, ch->sex, 2)];
				break;
			case 'G':
				i = in_him_her[URANGE(0, vch->sex, 2)];
				break;
			case 'r':
				i = at_him_her[URANGE(0, ch->sex, 2)];
				break;
			case 'R':
				i = at_him_her[URANGE(0, vch->sex, 2)];
				break;
			case 'w':
				i = with_him_her[URANGE(0, ch->sex, 2)];
				break;
			case 'W':
				i = with_him_her[URANGE(0, vch->sex, 2)];
				break;
			case 'f':
				i = from_him_her[URANGE(0, ch->sex, 2)];
				break;
			case 'F':
				i = from_him_her[URANGE(0, vch->sex, 2)];
				break;
			case 'i':
				i = i_see_him_her[URANGE(0, ch->sex, 2)];
				break;
			case 'I':
				i = i_see_him_her[URANGE(0, vch->sex, 2)];
				break;
			case 'o':
				i = soc[URANGE(0, ch->sex, 2)];
				break;
			case 'O':
				i = soc[URANGE(0, vch->sex, 2)];
				break;
			case 'y':
				i = socy[URANGE(0, ch->sex, 2)];
				break;
			case 'Y':
				i = socy[URANGE(0, vch->sex, 2)];
				break;
			case 'x':
				i = socp[URANGE(0, ch->sex, 2)];
				break;
			case 'X':
				i = socp[URANGE(0, vch->sex, 2)];
				break;
			case 'a':
				i = socr[URANGE(0, ch->sex, 2)];
				break;
			case 'A':
				i = socr[URANGE(0, vch->sex, 2)];
				break;
			case 'z':
				i = socz[URANGE(0, ch->sex, 2)];
				break;
			case 'Z':
				i = socz[URANGE(0, vch->sex, 2)];
				break;

			case 'p':
				if (str[1] == '$')
				{
					if (str[2] >= '0' && str[2] <= '5')
					{
						str += 2;
						i = can_see_obj(to, obj1) ? obj1->przypadki[(*str - 48)] : "co�";
					}
				}
				else
					i = can_see_obj(to, obj1) ? obj1->przypadki[0] : "co�";
				break;
			case 'P':
				if (str[1] == '$')
				{
					if (str[2] >= '0' && str[2] <= '5')
					{
						str += 2;
						i = can_see_obj(to, obj2) ? obj2->przypadki[(*str - 48)] : "co�";
					}
				}
				else
					i = can_see_obj(to, obj2) ? obj2->przypadki[0] : "co�";
				break;

			case 'd':
				if (!arg2 || ((char*) arg2)[0] == '\0')
					i = "drzwi";
				else
				{
					one_argument((char*) arg2, fname);
					i = fname;
				}
				break;
			}
		}
		++str;
		while ((*point = *i) != '\0')
			++point, ++i;

		while ((*point = *col) != '\0')
			++point, ++col;
	}

	*point++ = '\n';
	*point++ = '\r';
	*point = '\0';

	buf[0] = UPPER(buf[0]);

	return buf;
}

void act(const char *color, const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type)
{
	char *txt;
	CHAR_DATA *to;
	CHAR_DATA *vch = (CHAR_DATA*) arg2;

	/*
	 * Discard null and zero-length messages.
	 */
	if (!format || format[0] == '\0')
		return;

	IF_BUG(ch == NULL, "(%s)", format)
		return;

	if (!ch->in_room)
		to = NULL;
	else if (type == TO_CHAR)
		to = ch;
	else
		to = ch->in_room->people.empty() ? nullptr : ch->in_room->people.front();

	/*
	 * ACT_SECRETIVE handling
	 */
	if ( IS_NPC(ch) && IS_SET(ch->act, ACT_SECRETIVE) && type != TO_CHAR)
		return;

	if (type == TO_VICT)
	{
		IF_BUG(vch == NULL, "TO_VICT: %s (%s)", ch->name, format)
			return;
		IF_BUG(vch->in_room == NULL, "TO_VICT: %s -> %s (%s)", ch->name, vch->name, format)
			return;

		to = vch;
		/*	to = vch->in_room->first_person;*/
	}

	/* Thanos: 4 Alde */
	if (type == TO_SHIP)
	{
		SHIP_DATA *ship;
		ROOM_INDEX_DATA *room_orig;

		IF_BUG(ch->in_room == NULL, "TO_SHIP %s (%s)", ch->name, format)
			return;

		ship = ship_from_room(ch->in_room);
		IF_BUG(ship == NULL, "TO_SHIP %s (%s)", ch->name, format)
			return;

		/*
		 * zasada jest prosta: odwo�ujemy sie rekurencyjnie do f-cji
		 * act, tyle ze wywolujemy ja z TO_ROOM, na kolejne pokoje
		 * w statq
		 */
		room_orig = ch->in_room;
		for (auto* room : ship->locations)
		{
			char_from_room(ch);
			char_to_room(ch, room);
			act(color, format, ch, arg1, arg2, TO_ROOM);
		}
		char_from_room(ch);
		char_to_room(ch, room_orig);
		return;
	}

	if (MOBtrigger && type != TO_CHAR && type != TO_VICT && to)
	{
		txt = act_string(color, format, NULL, ch, arg1, arg2);

		if (to->in_room && to->in_room->progtypes && IS_SET(to->in_room->progtypes, ACT_PROG))
			rprog_act_trigger(txt, to->in_room, ch, (OBJ_DATA*) arg1, (void*) arg2);

		if (to->in_room)
		{
			for (auto* to_obj : to->in_room->contents)
			{
				if (to_obj && to_obj->pIndexData->progtypes && IS_SET(to_obj->pIndexData->progtypes, ACT_PROG))
					oprog_act_trigger(txt, to_obj, ch, (OBJ_DATA*) arg1, (void*) arg2);
			}
		}
	}

	/* Anyone feel like telling me the point of looping through the whole
	 room when we're only sending to one char anyways..? -- Alty */
	if (type == TO_CHAR || type == TO_VICT)
	{
		if (to && (to->desc || (IS_NPC(to) && IS_SET(to->pIndexData->progtypes, ACT_PROG))) && IS_AWAKE(to))
		{
			txt = act_string(color, format, to, ch, arg1, arg2);

			if (to->desc)
			{
				if (!noEcho)
					ch_printf(to, "%s%s" PLAIN, color, txt);
			}

			if (MOBtrigger)
				mprog_act_trigger(txt, to, ch, (OBJ_DATA*) arg1, (void*) arg2);
		}
	}
	else if (to && to->in_room)
	{
		for (auto* to : to->in_room->people)
		{
			if ((!to->desc && ( IS_NPC(to) && !IS_SET(to->pIndexData->progtypes, ACT_PROG))) || !IS_AWAKE(to))
				continue;

			if (type == TO_ROOM && to == ch)
				continue;
			if (type == TO_NOTVICT && (to == ch || to == vch))
				continue;

			txt = act_string(color, format, to, ch, arg1, arg2);

			if (to->desc)
			{
				/* Thanos - je�li mob wykonuje mpNoEcho -- act() nic nie robi */
				if (!noEcho)
					ch_printf(to, "%s%s" PLAIN, color, txt);
			}

			if (MOBtrigger)
			{
				/* Note: use original string, not string with ANSI. -- Alty */
				mprog_act_trigger(txt, to, ch, (OBJ_DATA*) arg1, (void*) arg2);
			}
		}
	}

	MOBtrigger = true;
	return;
}

char* default_prompt(CHAR_DATA *ch)
{
	static char buf[MAX_STRING_LENGTH];

	strcpy(buf, "");
	if (ch->skill_level[FORCE_ABILITY] > 1 || get_trust(ch) >= LEVEL_IMMORTAL)
		strcat(buf,
		PROMPT_COL_N "[Hp:" PROMPT_COL_B "%h" PROMPT_COL_N "/%H"
		FG_BLUE " Frc:" FB_BLUE "%m" FG_BLUE "/%M"
		PROMPT_COL_N " Mv:" PROMPT_COL_B "%v" PROMPT_COL_N "/%V]" PLAIN " ");
	else
		strcat(buf,
		PROMPT_COL_N "[Hp:" PROMPT_COL_B "%h" PROMPT_COL_N "/%H"
		PROMPT_COL_N " Mv:" PROMPT_COL_B "%v" PROMPT_COL_N "/%V]" PLAIN " ");

	return buf;
}

int getcolor(char clr)
{
	static const char colors[17] = "xrgObpcwzRGYBPCW";
	int r;

	for (r = 0; r < 17; r++)
		if (clr == colors[r])
			return r;
	return -1;
}

void display_prompt(DESCRIPTOR_DATA *d)
{
	CHAR_DATA *ch;
	const char *prompt;
	char buf[MAX_STRING_LENGTH];
	char *pbuf = buf;
	unsigned int stat;

	ch = d->character;

	IF_BUG(ch == NULL, "")
		return;

	if (!IS_NPC(ch) && ch->pcdata && ch->pcdata->prompt && *ch->pcdata->prompt)
		prompt = ch->pcdata->prompt;
	else
		prompt = default_prompt(ch);

	send_to_char( RESET PLAIN, ch);

	if (d->str_editing)
	{
		send_to_char("  " FB_WHITE ">" PLAIN " ", ch);
		return;
	}

	if (ch->desc->connected == CON_HOLONET)
	{
		ch_printf(ch, "%s@hoLOnet.sw# ", strlower(ch->name));
		return;
	}

	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_AFK))
	{
		if (ch->pcdata->afk_reason && ch->pcdata->afk_reason[0] != '\0')
			ch_printf(ch, PROMPT_COL_N "[Jeste� AWAY (%s)]" PLAIN " ", ch->pcdata->afk_reason);
		else
			ch_printf(ch, PROMPT_COL_N "[Jeste� AWAY]" PLAIN " ");
		return;
	}

	if ( IS_CREW(ch) && IS_SET(ch->pcdata->flags, PCFLAG_SHIPSTAT) && ship_from_room(ch->in_room) != NULL)
	{
		ch_printf(ch, FB_GREEN "Hull: %d/%d Shields: %d/%d Energia: %d" EOL, ship_from_room(ch->in_room)->maxhull,
				ship_from_room(ch->in_room)->hull, ship_from_room(ch->in_room)->shield, ship_from_room(ch->in_room)->maxshield,
				ship_from_room(ch->in_room)->energy);
	}
	for (; *prompt; prompt++)
	{
		/*
		 * '&' = foreground color/intensity bit
		 * '^' = background color/blink bit
		 * '%' = prompt commands
		 * Note: foreground changes will revert background to 0 (black)
		 */
		char c;

		if (*prompt != __FGMOD[0] && *prompt != __BGMOD[0] && *prompt != '%')
		{
			*(pbuf++) = *prompt;
			continue;
		}

		++prompt;

		if (!*prompt)
			break;

		if (*prompt == *(prompt - 1))
		{
			*(pbuf++) = *prompt;
			continue;
		}

		c = (*(prompt - 1));

		if (c == __FGMOD[0] || c == __BGMOD[0])
		{
			stat = make_color_sequence(&prompt[-1], pbuf, d);

			if (stat < 0)
				--prompt;
			else if (stat > 0)
				pbuf += stat;
		}
		else if (c == '%')
		{
			*pbuf = '\0';
			stat = 0x80000000;
			switch (*prompt)
			{
			case '%':
				*pbuf++ = '%';
				*pbuf = '\0';
				break;

			case 'a':
				if (ch->top_level >= 10)
					stat = ch->alignment;
				else if (IS_GOOD(ch))
					strcpy(pbuf, "dobry");
				else if (IS_EVIL(ch))
					strcpy(pbuf, "z�y");
				else
					strcpy(pbuf, "neutralny");
				break;

			case 'h':
				stat = ch->hit;
				break;

			case 'H':
				stat = ch->max_hit;
				break;

			case 'm':
				if ( IS_IMMORTAL(ch) || ch->skill_level[FORCE_ABILITY] > 1)
					stat = ch->mana;
				else
					stat = 0;
				break;

			case 'M':
				if ( IS_IMMORTAL(ch) || ch->skill_level[FORCE_ABILITY] > 1)
					stat = ch->max_mana;
				else
					stat = 0;
				break;

				/*		    case 'u':
				 stat = num_descriptors;			break;
				 */
			case 'U':
				stat = sysdata.maxplayers;
				break;

			case 'v':
				stat = ch->move;
				break;

			case 'V':
				stat = ch->max_move;
				break;

			case 'g':
				stat = ch->gold;
				break;
				/*Thanos - kasa z banku w prompcie */
			case 'b':
				stat = (ch->pcdata ? ch->pcdata->bank : 0);
				break;

			case 'r':
				sprintf(pbuf, "%s" PLAIN PROMPT_COL_N, ch->in_room->name);
				break;
			case 'R':
				if (IS_IMMORTAL(ch))
					sprintf(pbuf, "%d", ch->in_room->vnum);
				else
					strcpy(pbuf, "%R");
				break;
				/*added by Thanos (krainka w prompcie) */
			case 'p':
				sprintf(pbuf, "%s",
						( IS_IMMORTAL( ch ) ? ((ch->in_room->area && ch->in_room->area->name) ? ch->in_room->area->name : "...") : "%p"));
				break;
			case 'P': /*(i planeta w prompcie)*/
				sprintf(pbuf, "%s",
						( IS_IMMORTAL( ch ) ?
								((ch->in_room->area && ch->in_room->area->planet && ch->in_room->area->planet->name) ?
										ch->in_room->area->planet->name : "...") :
								"%P"));
				break;

				/* added by Thanos (pi�trowe prompty) */
			case 'n':
				sprintf(pbuf, EOL PROMPT_COL_N);
				break;

			case 'o': /*(OLC)*/
				sprintf(pbuf, "%s", olc_ed_name(ch));
				break;

			case 'O': /*(OLC)*/
			{
				const SWString &svnum = olc_ed_vnum(ch);
				sprintf(pbuf, "%s", svnum.c_str());
			}
				break;

			case 'i':
				if ((!IS_NPC(ch) && IS_SET(ch->act, PLR_WIZINVIS)) || (IS_NPC(ch) && IS_SET(ch->act, ACT_MOBINVIS)))
					sprintf(pbuf, "Invis %d", (IS_NPC(ch) ? ch->mobinvis : ch->pcdata->wizinvis));
				else if (IS_AFFECTED(ch, AFF_INVISIBLE))
					sprintf(pbuf, "Invis");
				break;

			case 'I':
				stat = (IS_NPC(ch) ?
						(IS_SET(ch->act, ACT_MOBINVIS) ? ch->mobinvis : 0) : (IS_SET(ch->act, PLR_WIZINVIS) ? ch->pcdata->wizinvis : 0));
				break;

			case 'w':
			{
				int value = 0;
				for (auto* timer : ch->timers)
					value += timer->count;

				stat = ch->wait + value;    //added by Thanos
			}
				break;
			}

			if (stat != 0x80000000)
				sprintf(pbuf, "%d", stat);

			pbuf += strlen(pbuf);
		}
		else
			bug("bad command char '%c'.", *(prompt - 1));

	}
	*pbuf = '\0';

	send_to_char(buf, ch);
	send_to_char( PLAIN, ch);
#if defined(ARMAGEDDON)
   if (ch->fighting)
	ch_printf( ch, FB_BLUE "<%d%%>" PLAIN " ",
	    ( 	ch->fighting->who->hit * 100 ) /
		ch->fighting->who->max_hit );
#endif
	if (ch->pcdata && *ch->pcdata->prefix)
		ch_printf(ch, "%s" RESET PLAIN " ", ch->pcdata->prefix);
	return;
}

int make_color_sequence(const char *col, char *buf, DESCRIPTOR_DATA *d)
{
	int ln;
	const char *ctype = col;
	unsigned char cl;
	CHAR_DATA *och;
	bool ansi;

	och = (d->original ? d->original : d->character);
	ansi = (!IS_NPC(och) && IS_SET(och->act, PLR_ANSI));

	col++;
	if (!*col)
		ln = -1;
	else if (*ctype != __FGMOD[0] && *ctype != __BGMOD[0])
	{
		bug("command '%c' not '%c' or '%c'.", *ctype, __FGMOD[0], __BGMOD[0]);
		ln = -1;
	}
	else if (*col == *ctype)
	{
		buf[0] = *col;
		buf[1] = '\0';
		ln = 1;
	}
	else if (!ansi)
		ln = 0;
	else
	{
		char c;
		int newcol;

		cl = d->prevcolor;

		c = *ctype;

		if (c == __FGMOD[0])
		{
			if (*col == '-')
			{
				buf[0] = '~';
				buf[1] = '\0';
				return 1;
			}
			else
			/* Added by Thanos */
			if (*col == 'x')
			{
				buf[0] = 'x';
				buf[1] = '\0';
				return 1;
			}
		}
		/*    else
		 if( c == __BGMOD[0] )
		 {
		 }
		 else
		 {
		 bug( "Make_color_sequence: bad command char '%c'.", *ctype );
		 *buf = '\0';
		 return -1;
		 }
		 */
		if ((newcol = getcolor(*col)) < 0)
		{
			*buf = '\0';
			return 0;
		}
		else if (*ctype == __FGMOD[0])
			cl = (cl & 0xF0) | newcol;
		else
			cl = (cl & 0x0F) | (newcol << 4);

		if (cl == d->prevcolor)
		{
			*buf = '\0';
			return 0;
		}

		strcpy(buf, "\033[");
		if ((cl & 0x88) != (d->prevcolor & 0x88))
		{
			strcat(buf, "m\033[");
			if ((cl & 0x08))
				strcat(buf, "1;");
			if ((cl & 0x80))
				strcat(buf, "5;");

			d->prevcolor = 0x07 | (cl & 0x88);
			ln = strlen(buf);
		}
		else
			ln = 2;

		if ((cl & 0x07) != (d->prevcolor & 0x07))
		{
			sprintf(buf + ln, "3%d;", cl & 0x07);
			ln += 3;
		}

		if ((cl & 0x70) != (d->prevcolor & 0x70))
		{
			sprintf(buf + ln, "4%d;", (cl & 0x70) >> 4);
			ln += 3;
		}
		if (buf[ln - 1] == ';')
			buf[ln - 1] = 'm';
		else
		{
			buf[ln++] = 'm';
			buf[ln] = '\0';
		}
		d->prevcolor = cl;

	}
	if (ln <= 0)
		*buf = '\0';
	return ln;
}

void set_pager_input(DESCRIPTOR_DATA *d, char *argument)
{
	while (isspace(*argument))
		argument++;
	d->pagecmd = *argument;
	return;
}

bool pager_output(DESCRIPTOR_DATA *d)
{
	char *last;
	CHAR_DATA *ch;
	int pclines;
	int lines;
	int total;    //added by Thanos
	int now;    //i to
	char *ptr;    //i jeszcze to
	unsigned char cl;    //no i to jeszcze
	bool ret;
	char buf[MAX_STRING_LENGTH];

	if (!d || !d->pagepoint || d->pagecmd == -1)
		return true;

	ch = CH(d);
	cl = d->prevcolor;    //added by Thanos
	pclines = UMAX(ch->pcdata->pagerlen, 5) - 1;
	switch (LOWER(d->pagecmd))
	{
	case '\0':
	case 'd': /* dalej */
		lines = 0;
		break;
	case 'c': /* cofnij */
		lines = -1 - (pclines * 2);
		break;
	case 'o': /* od�wie� */
		lines = -1 - pclines;
		break;
	default: /* wyjd� */
		d->pagetop = 0;
		d->pagepoint = NULL;
		flush_buffer(d, true);
		DISPOSE(d->pagebuf);
		d->pagesize = MAX_STRING_LENGTH;
		return true;
	}

	//Added by Ratm (status bar)
//    if( IS_SET( ch->act, PLR_STATUS_BAR ) && ch->desc->connected >= CON_PLAYING )
//	update_status_bar( ch );

	if ( IS_SET(ch->act, PLR_STATUS_BAR) && ch->desc->connected >= CON_PLAYING)
	{
		sprintf(buf, "\e[%d;1H", ch->pcdata->status_height - 1);
		write_to_descriptor(d->descriptor, buf, 0);
		update_status_bar(ch);
		sprintf(buf, "\e[%d;1H\e[J", ch->pcdata->status_height + 1);
		write_to_buffer(d, buf, 0);
	}

	while (lines < 0 && d->pagepoint >= d->pagebuf)
		if (*(--d->pagepoint) == '\n')
			++lines;

	if (*d->pagepoint == '\n' && *(++d->pagepoint) == '\r')
		++d->pagepoint;

	if (d->pagepoint < d->pagebuf)
		d->pagepoint = d->pagebuf;

	for (lines = 0, last = d->pagepoint; lines < pclines; ++last)
		if (!*last)
			break;
		else if (*last == '\n')
			++lines;

	total = 0;
	for (ptr = d->pagebuf; *ptr != 0; ptr++)
		if (*ptr == '\n')
			total++;

	now = 0;
	for (ptr = d->pagepoint; *ptr != 0; ptr++)
		if (*ptr == '\n')
			now++;

	if (*last == '\r')
		++last;

	if (last != d->pagepoint)
	{
		if (!write_to_descriptor(d->descriptor, d->pagepoint, (last - d->pagepoint)))
			return false;
		d->pagepoint = last;
	}

	while (isspace(*last))
		++last;

	if (!*last)
	{
		d->pagetop = 0;
		d->pagepoint = NULL;
		flush_buffer(d, true);
		DISPOSE(d->pagebuf);
		d->pagesize = MAX_STRING_LENGTH;
		return true;
	}

	d->pagecmd = -1;

	if (write_to_descriptor(d->descriptor, "\033[7m", 7) == false)
		return false;

	/*
	 added by Thanos (pokazuje nr strony aktualnie wyswietlanego tekstu)
	 wiem, ze to lamerskie takie sprawdzanie charsetu, ale nie chce mi
	 sie pisac funkcji
	 */

	if (d->character->charset == CHARSET_WIN)
		sprintf(buf, " (%3d/%-3d)  Wci�nij: (D)alej, (O)d�wie�, (C)ofnij, (W)yjd�: [D] ", total / lines - now / lines + 1,
				total / lines + 1);
	else if (d->character->charset == CHARSET_ISO)
		sprintf(buf, " (%3d/%-3d)  Wci�nij: (D)alej, (O)d�wie�, (C)ofnij, (W)yjd�: [D] ", total / lines - now / lines + 1,
				total / lines + 1);
	else
		sprintf(buf, " (%3d/%-3d)  Wcisnij: (D)alej, (O)dswiez, (C)ofnij, (W)yjdz: [D] ", total / lines - now / lines + 1,
				total / lines + 1);

	if ((ret = write_to_descriptor(d->descriptor, buf, 0)) == false)
		return false;

	if (IS_SET(ch->act, PLR_ANSI))
	{
		char buf[32];

		if (cl == 7)
			strcpy(buf, "\033[0m");
		else
			sprintf(buf, "\033[0m\033[%d;%s%dm", (cl & 8) == 8, /*bold?*/
			cl > 15 ? "5;" : "", /*blink?*/
			(cl & 7) + 30); /*colour*/
		ret = write_to_descriptor(d->descriptor, buf, 0);
	}
	else
		ret = write_to_descriptor(d->descriptor, "\033[0m", 0);

	return ret;
}

/*
 To sprawdza ile % procka mud uzywa w momencie wywolania tej funkcji.
 Moge tez napisac, zeby sprawdzal ile pamieci uzywa. To dziala tylko
 i wylacznie pod FreeBSD 5.0, pod 4.x nie chce mi sie tego pisac bo
 sie nie przydal. Chcialem tylko zapytac czy dasz rade napisac cos
 takiego pod linuxa, bo uzywasz Ty (Thanos) i Aldegard. Ten bajer moze byc
 bardzo przydatny do testowania czegos, etc... Na razie dalem to w
 auth_update, zeby zbyt czesto sie nie wywolywalo, ale jak Ci sie
 spodoba i nie bedziesz kazal mi tego wyjebac z kodu to mozna to
 umiescic w innych ciekawych miejscach. To napiszesz pod Linuxa to?

 Trog
 */
#if defined(__FreeBSD__) && __FreeBSD__ == 5
void cpu_usage()
{
    struct kinfo_proc   *kp;
    kvm_t               *kd;
    int                 count;
	bool				found = false;
	float				pcpu;
	char				buf[MIL];

	if( !(kd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL)) )
		return;
	if( !(kp = kvm_getprocs(kd, KERN_PROC_RUID, getuid(), &count)) || count < 1 )
	{
		kvm_close( kd );
		return;
	}

	kvm_close( kd );

	while( kp && count > 0 )
	{
		if(!strcmp( kp->ki_comm, "swmud" ))
		{
			found = true;
			break;
		}
		if( --count > 0 )
			kp++;
	}
	if( !found )
		return;

	if( (pcpu = get_cpu_usage( kp )) > 75.0 )
	    sprintf( buf, "EXTREMELY high CPU usage: %.4f%%.", pcpu );
	else
	if( pcpu > 50.0 )
	    sprintf( buf, "Very high CPU usage: %.4f%%.", pcpu );
	else
	if( pcpu > 20.0 )
	    sprintf( buf, "High CPU usage: %.4f%%.", pcpu );
	else
		return;
	bug( "%s", buf );

	return;
}

double get_cpu_usage( struct kinfo_proc *kp )
{
    size_t	oldlen;
	int		ccpu,fscale;

	oldlen = sizeof(ccpu);
	if (sysctlbyname("kern.ccpu", &ccpu, &oldlen, NULL, 0) < 0)
		return(0.0);
	oldlen = sizeof(fscale);
	if (sysctlbyname("kern.fscale", &fscale, &oldlen, NULL, 0) < 0)
		return(0.0);

#define fixpttof(fixpt)   ((double)(fixpt) / fscale)

    if (kp->ki_swtime == 0 || (kp->ki_sflag & PS_INMEM) == 0)
        return (0.0);
    return (100.0 * fixpttof(kp->ki_pctcpu) /
        (1.0 - exp(kp->ki_swtime * log(fixpttof(ccpu)))));
}
#endif

/* Trog: lepsze kodowanie hasel zerzniete z unixa. */

void to64(char *s, long v, int n)
{
	while (--n >= 0)
	{
		*s++ = itoa64[v & 0x3f];
		v >>= 6;
	}
}

char* mkpasswd(char *pass)
{
	char salt[32];
	struct timeval tv;

	gettimeofday(&tv, 0);
	srandom(tv.tv_sec);
#if defined(__FreeBSD__) && __FreeBSD__ > 3
	if( strcmp( crypt_get_format(), "des" ) )
		(void)crypt_set_format("des");
#endif
	to64(&salt[0], random(), 3);
	to64(&salt[3], tv.tv_usec, 3);
	to64(&salt[6], tv.tv_sec, 2);
	to64(&salt[8], random(), 5);
	to64(&salt[13], random(), 5);
	to64(&salt[17], random(), 5);
	to64(&salt[22], random(), 5);
	salt[27] = '\0';

	return (crypt(pass, salt));
}

bool check_password(char *_new, char *old)
{
#if defined(__FreeBSD__) && __FreeBSD__ > 3
	if( strcmp( crypt_get_format(), "des" ) )
		(void)crypt_set_format("des");
#endif
	if (!strcmp(crypt(_new, old), old))
		return true;

	return false;
}

#if defined( WIN32 )
void gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif

