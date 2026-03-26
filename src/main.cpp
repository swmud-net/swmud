/*
 * main.cpp
 *
 *  Created on: 2010-08-21
 *      Author: Trog
 *
 * Tutaj ma byc definicja jedynie funkcji main. Nic mnie to nie obchodzi, ale
 * nic innego ma tutaj nie byc poza ta funkcja, ew. extern'ami i include'ami!
 *
 */

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

extern void swmud_killed(int);
extern void swmud_cleanup();
extern void sw_terminate();
extern void sw_unexpected();
extern void deal_with_crash();
extern int init_socket(int);
extern int init_socket6(uint16_t);
extern void game_loop();

extern FILE *fpPid;
extern HOUR_MIN_SEC set_boot_time_struct;

int main(int argc, char **argv)
{
	struct timeval now_time;
	int i;
	fCopyOver = false; /*Thanos*/

	/*
	 * Najpierw przejd�my do mud/bin/ i tam osi�d�my.
	 * Dzi�ki temu tam znajdzie si� ewentualny 'core'.
	 *							-- Thanos
	 */
	if (chdir( BIN_DIR) < 0)
	{
		perror("chdir");
		exit(0);
	}

	atexit(swmud_cleanup);

	STRDUP(sysdata.binfile, argv[0]);

	/*
	 * Memory debugging if needed.
	 */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

	set_terminate(&sw_terminate);

	pid = getpid();
	if ((fpPid = fopen( PID_FILE, "w")) == NULL)
	{
		perror( PID_FILE);
		exit(1);
	}
	else
	{
		fprintf(fpPid, "%d", pid);
		fclose(fpPid);
	}
	sprintf(log_buf, "Got pid: %d.", pid);
	log_string(log_buf);

	num_descriptors = 0;
	sysdata.NO_NAME_RESOLVING = true;
	sysdata.WAIT_FOR_AUTH = true;
	sysdata.test_only = false;

	/*
	 * Init time.
	 */
	gettimeofday(&now_time, NULL);
	current_time = (time_t) now_time.tv_sec;
	boot_time = time(0);

	/*
	 * Init boot time.
	 */
	set_boot_time = &set_boot_time_struct;
	set_boot_time->manual = 0;

	do_set_boot_time(NULL, (char*) "after 120 h");

	/*
	 * Reserve two channels for our use.
	 */
	if (!(fpReserve = fopen( NULL_FILE, "r")))
	{
		perror(NULL_FILE);
		exit(1);
	}

	if (!(fpLOG = fopen( NULL_FILE, "r")))
	{
		perror(NULL_FILE);
		exit(1);
	}

	/*
	 * Get the port number.
	 */
	port = 4000;
	fCopyOver = false;
	for (i = 1; i < argc; i++)
	{
		if (argv[i] && argv[i][0])
		{
			if (!str_cmp(argv[i], "copyover"))
			{
				fCopyOver = true;

				if (!argv[i + 1] || !argv[i + 1][0])
				{
					fprintf(stderr, "Control not specified." NL);
					exit(1);
				}
				if (!argv[i + 2] || !argv[i + 2][0])
				{
					fprintf(stderr, "Control IPv6 not specified." NL);
					exit(1);
				}
				control = atoi(argv[i + 1]);
				control6 = atoi(argv[i + 2]);
			}
			else if (!str_cmp(argv[i], "test"))	// tylko testujemy krainy
			{
				sysdata.test_only = true;
				break;
			}
			else if ( is_number(argv[i]))
			{
				if ((port = atoi(argv[1])) <= 1024)
				{
					fprintf(stderr, "Port number must be above 1024." NL);
					exit(1);
				}
			}
			else
			{
				fprintf(stderr, "Usage: %s [test] [#port] [copyover <control> <control6>]" NL, argv[0]);
				exit(1);
			}
		}
	}

	/*
	 * Run the game.
	 */
	if (!fCopyOver && !sysdata.test_only)
	/*
	 Je�li startujemy z CopyOver nie otwieramy socketu (bo ju� go mamy)
	 Thanos.
	 */
	{
		log_string("Initializing socket");
		control = init_socket(port);
		control6 = init_socket6(port);
	}

	log_string("Booting Database");
	boot_db(fCopyOver);

#if defined( ARMAGEDDON )
    sprintf( log_buf, "StarWars (ARMAGEDDON Version) MUD ready on port: %d.", port );
#else
	sprintf(log_buf, "StarWars MUD ready on port: %d.", port);
#endif
	log_string(log_buf);

	/*
	 * Thanos -- f-cja bada na czym pada mud
	 * Jednak aktywujemy j� DOPIERO PO STARCIE MUDA (boot_db) !!!
	 * Poniewa� 'deal_with_crash' b�dzie si� stara� postawi� muda na nowo
	 * zmniejszamy niebezpiecze�stwo 'wstawania' w niesko�czono��.
	 *
	 * thx 2 mr John Shapley Gray
	 */
	if (sysdata.crashlog)
		signal(SIGSEGV, (void (*)(int)) deal_with_crash);

#if !defined( WIN32 )
	signal(SIGPIPE, SIG_IGN);

	signal(SIGKILL, swmud_killed);
	signal(SIGTERM, swmud_killed);
	signal(SIGHUP, swmud_killed);
	signal(SIGQUIT, swmud_killed);
	signal(SIGSTOP, swmud_killed);
	signal(SIGINT, swmud_killed);
#endif

	game_loop();

	/*
	 * That's all, folks.
	 */
#if !defined( WIN32 )
	close(control);
	close(control6); //Nie ma IPv6 dla windowsa
#else
    closesocket( control );
    WSACleanup();
#endif
	log_string("Normal termination of game.");
	return 0;
}
