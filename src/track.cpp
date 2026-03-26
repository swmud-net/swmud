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
 *			 Tracking/hunting module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <time.h>
#include "mud.h"

#define BFS_ERROR	   -1
#define BFS_ALREADY_THERE  -2
#define BFS_NO_PATH	   -3
#define BFS_MARK         BV01

#define TRACK_THROUGH_DOORS

extern int top_room;

bool mob_snipe args( ( CHAR_DATA *ch , CHAR_DATA *victim) );
ch_ret one_hit args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
ROOM_INDEX_DATA *generate_exit( ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit );

/* You can define or not define TRACK_THOUGH_DOORS, above, depending on
 whether or not you want track to find paths which lead through closed
 or hidden doors.
 */

struct bfs_queue_struct
{
	ROOM_INDEX_DATA *room;
	char dir;
	struct bfs_queue_struct *next;
};

static struct bfs_queue_struct *queue_head = NULL,
*queue_tail = NULL, *room_queue = NULL;

/* Utility macros */
#define MARK(room)	(SET_BIT(	(room)->room_flags, BFS_MARK) )
#define UNMARK(room)	(REMOVE_BIT(	(room)->room_flags, BFS_MARK) )
#define IS_MARKED(room)	(IS_SET(	(room)->room_flags, BFS_MARK) )

ROOM_INDEX_DATA* toroom(ROOM_INDEX_DATA *room, int door)
{
	return (get_exit(room, door)->to_room);
}

bool valid_edge(ROOM_INDEX_DATA *room, int door)
{
	EXIT_DATA *pexit;
	ROOM_INDEX_DATA *to_room;

	pexit = get_exit(room, door);
	if (pexit && (to_room = pexit->to_room) != NULL
#ifndef TRACK_THROUGH_DOORS
    &&  !IS_SET( pexit->flags, EX_CLOSED )
#endif
			&& !IS_MARKED(to_room))
		return true;
	else
		return false;
}

void bfs_enqueue(ROOM_INDEX_DATA *room, char dir)
{
	struct bfs_queue_struct *curr;

	CREATE(curr, struct bfs_queue_struct, 1);
	curr->room = room;
	curr->dir = dir;
	curr->next = NULL;

	if (queue_tail)
	{
		queue_tail->next = curr;
		queue_tail = curr;
	}
	else
		queue_head = queue_tail = curr;
}

void bfs_dequeue(void)
{
	struct bfs_queue_struct *curr;

	curr = queue_head;

	if (!(queue_head = queue_head->next))
		queue_tail = NULL;
	DISPOSE(curr);
}

void bfs_clear_queue(void)
{
	while (queue_head)
		bfs_dequeue();
}

void room_enqueue(ROOM_INDEX_DATA *room)
{
	struct bfs_queue_struct *curr;

	CREATE(curr, struct bfs_queue_struct, 1);
	curr->room = room;
	curr->next = room_queue;

	room_queue = curr;
}

void clean_room_queue(void)
{
	struct bfs_queue_struct *curr, *curr_next;

	for (curr = room_queue; curr; curr = curr_next)
	{
		UNMARK(curr->room);
		curr_next = curr->next;
		DISPOSE(curr);
	}
	room_queue = NULL;
}

int find_first_step(ROOM_INDEX_DATA *src, ROOM_INDEX_DATA *target, int maxdist)
{
	int curr_dir, count;

	if (!src || !target)
	{
		bug("Illegal value passed to find_first_step (track.c)", 0);
		return BFS_ERROR;
	}

	if (src == target)
		return BFS_ALREADY_THERE;

	if (src->area != target->area)
		return BFS_NO_PATH;

	room_enqueue(src);
	MARK(src);

	/* first, enqueue the first steps, saving which direction we're going. */
	for (curr_dir = 0; curr_dir < 10; curr_dir++)
		if (valid_edge(src, curr_dir))
		{
			MARK(toroom(src, curr_dir));
			room_enqueue(toroom(src, curr_dir));
			bfs_enqueue(toroom(src, curr_dir), curr_dir);
		}

	count = 0;
	while (queue_head)
	{
		if (++count > maxdist)
		{
			bfs_clear_queue();
			clean_room_queue();
			return BFS_NO_PATH;
		}
		if (queue_head->room == target)
		{
			curr_dir = queue_head->dir;
			bfs_clear_queue();
			clean_room_queue();
			return curr_dir;
		}
		else
		{
			for (curr_dir = 0; curr_dir < 10; curr_dir++)
				if (valid_edge(queue_head->room, curr_dir))
				{
					MARK(toroom(queue_head->room, curr_dir));
					room_enqueue(toroom(queue_head->room, curr_dir));
					bfs_enqueue(toroom(queue_head->room, curr_dir),
							queue_head->dir);
				}
			bfs_dequeue();
		}
	}
	clean_room_queue();

	return BFS_NO_PATH;
}

DEF_DO_FUN( track )
{
	CHAR_DATA *vict;
	char arg[MAX_INPUT_LENGTH];
	char buf[MAX_STRING_LENGTH];
	int dir, maxdist;

	if (!IS_NPC(ch) && !ch->pcdata->learned[gsn_track])
	{
		send_to_char("Nie wiesz jak to si� robi." NL, ch);
		return;
	}

	one_argument(argument, arg);
	if (arg[0] == '\0')
	{
		send_to_char("Kogo chcesz tropi�?" NL, ch);
		return;
	}

	WAIT_STATE(ch, skill_table[gsn_track]->beats);

	if (!(vict = get_char_world(ch, arg)))
	{
		send_to_char("Nie widzisz tu �lad�w nikogo takiego." NL, ch);
		return;
	}

	maxdist = 100 + ch->top_level * 30;

	if (!IS_NPC(ch))
		maxdist = (maxdist * ch->pcdata->learned[gsn_track]) / 100;

	dir = find_first_step(ch->in_room, vict->in_room, maxdist);
	switch (dir)
	{
	case BFS_ERROR:
		send_to_char("Hmm... co� jest nie tak." NL, ch);
		break;
	case BFS_ALREADY_THERE:
		if (!can_see(ch, vict))
			send_to_char("Tutaj trop si� urywa." NL, ch);
		else
			ch_printf(ch, "Przecie� jeste� w tym samym pomieszczeniu co %s!" NL,
					PERS(vict, ch, 0));
		break;
	case BFS_NO_PATH:
		sprintf(buf, "Nie mo�esz znale�� tropu st�d." NL);
		send_to_char(buf, ch);
		learn_from_failure(ch, gsn_track);
		break;
	default:
		ch_printf(ch, "Znajdujesz trop na %s st�d..." NL, dir_where_name[dir]);
		learn_from_success(ch, gsn_track);
		break;
	}
}

void found_prey(CHAR_DATA *ch, CHAR_DATA *victim)
{
	char buf[MSL] = {0};
	char victname[MSL] = {0};

	IF_BUG(victim == NULL, "")
		return;

	IF_BUG(ch == NULL, "")
		return;

	IF_BUG(victim->in_room == NULL, "")
		return;

	/* by Ratm. Sprawdzamy w jakiej pozycji znajduje si� posta� */
	if (ch->position == POS_RESTING || ch->position == POS_SITTING)
		do_stand(ch, (char*) ""); //je�li odpoczywa to wstaje
	else if (ch->position != POS_STANDING && ch->position != POS_MOUNTED)
		return;	// gdy nie stoi lub nie jedzie to olewamy dalsze sprawdzanie

	strcpy(victname, PERS(victim, ch, 0));

	if (!can_see(ch, victim))
	{
		if (number_percent() < 90)
			return;
		switch (number_bits(2))
		{
		case 0:
			swsnprintf(buf, MSL, "Nie chcij bym ci� znalaz�%s %s!", SEX_SUFFIX__AO(ch),
					victname);
			do_say(ch, buf);
			break;
		case 1:
			act( COL_ACTION, "$n w�szy po doko�a szukaj�c $N$1.", ch, NULL,
					victim, TO_NOTVICT);
			act( COL_ACTION, "W�szysz doko�a szukaj�c $N$1.", ch, NULL, victim,
					TO_CHAR);
			act( COL_ACTION, "$n w�szy doko�a szukaj�c ciebie.", ch, NULL,
					victim, TO_VICT);
			sprintf(buf, "Czuj� ju� zapach twojej krwi!!!");
			do_say(ch, buf);
			break;
		case 2:
			swsnprintf(buf, MSL, "Rozszarpi� ci� na kawa�ki %s!", victname);
			do_yell(ch, buf);
			break;
		case 3:
			do_say(ch, (char*) "Czekaj! Niech tylko ci� znajd�...");
			break;
		}
		return;
	}

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
	{
		if (number_percent() < 90)
			return;
		switch (number_bits(2))
		{
		case 0:
			do_say(ch, (char*) "Wy�a� tch�rzu!");
			swsnprintf(buf, MSL, "%s to zakichany tch�rz !!!", victname);
			do_yell(ch, buf);
			break;
		case 1:
			swsnprintf(buf, MSL, "Za�atwmy to na zewn�trz %s", victname);
			do_say(ch, buf);
			break;
		case 2:
			swsnprintf(buf, MSL, "%s to wymoczek i tch�rz!", victname);
			do_yell(ch, buf);
			break;
		case 3:
			act( COL_ACTION, "$n popycha $N$3 prowokuj�co.", ch, NULL, victim,
					TO_NOTVICT);
			act( COL_ACTION, "Popychasz $N$3 prowokuj�co.", ch, NULL, victim,
					TO_CHAR);
			act( COL_ACTION, "$n popycha ci� prowokuj�co.", ch, NULL, victim,
					TO_VICT);
			break;
		}
		return;
	}

	switch (number_bits(2))
	{
	case 0:
		swsnprintf(buf, MSL, "Twoja krew jest ju� moja %s!", victname);
		do_yell(ch, buf);
		break;
	case 1:
		swsnprintf(buf, MSL, "Ha, wi�c zn�w si� spotykamy %s!", victname);
		do_say(ch, buf);
		break;
	case 2:
		swsnprintf(buf, MSL, "Jaki chcesz mie� nagrobek, %s?", victname);
		do_say(ch, buf);
		break;
	case 3:
		act( COL_ACTION, "$n wyskakuje na $N$3 dos�ownie znik�d!", ch, NULL,
				victim, TO_NOTVICT);
		act( COL_ACTION, "Wyskakujesz na $N$3 zaskakuj�c $I go zupe�nie.", ch,
				NULL, victim, TO_CHAR);
		act( COL_ACTION, "$n wyskakuje na ciebie dos�ownie znik�d!", ch, NULL,
				victim, TO_VICT);
	}
	stop_hunting(ch);
	set_fighting(ch, victim);
	multi_hit(ch, victim, TYPE_UNDEFINED);
	return;
}

void hunt_victim(CHAR_DATA *ch)
{
	bool found;
	CHAR_DATA *tmp;
	int ret;

	if (!ch || !ch->hunting || !ch->hunting->who)
		return;

	/* make sure the char still exists */
	found = false;
	for (auto* tmp : char_list)
		if (ch->hunting->who == tmp)
		{
			found = true;
			break;
		}

	if (!found)
	{
		do_say(ch, (char*) "Do licha! Moja ofiara znikn�a !!!");
		stop_hunting(ch);
		return;
	}

	if (ch->in_room == ch->hunting->who->in_room)
	{
		if (ch->fighting)
			return;
		found_prey(ch, ch->hunting->who);
		return;
	}

	/* hunting with snipe */
	{
		OBJ_DATA *wield;

		wield = get_eq_char(ch, WEAR_WIELD);
		if (wield != NULL && wield->value[3] == WEAPON_BLASTER)
		{
			if (mob_snipe(ch, ch->hunting->who) == true)
				return;
		}
		else if (!IS_SET(ch->act, ACT_DROID))
			do_hide(ch, (char*) "");
	}

	ret = find_first_step(ch->in_room, ch->hunting->who->in_room, 5000);
	if (ret == BFS_NO_PATH)
	{
		EXIT_DATA *pexit;
		int attempt;

		for (attempt = 0; attempt < 25; attempt++)
		{
			ret = number_door();
			if ((pexit = get_exit(ch->in_room, ret)) == NULL || !pexit->to_room
			|| IS_SET(pexit->flags, EX_CLOSED)
			|| IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB))
				continue;
		}
	}
	if (ret < 0)
	{
		do_say(ch, (char*) "Do licha! Straci�em moj� ofiar�!");
		stop_hunting(ch);
		return;
	}
	else
	{
		move_char(ch, get_exit(ch->in_room, ret), false);
		if (char_died(ch))
			return;
		if (!ch->hunting)
		{
			IF_BUG(ch->in_room == NULL,
					"Mob #%d, name: %s. Placing mob in limbo.",
					ch->pIndexData->vnum, ch->name)
			{
				char_to_room(ch, get_room_index( ROOM_VNUM_LIMBO));
				return;
			}
			do_say(ch, (char*) "Do licha! Nie ma go!");
			return;
		}
		if (ch->in_room == ch->hunting->who->in_room)
			found_prey(ch, ch->hunting->who);
		return;
	}
}

bool mob_snipe(CHAR_DATA *ch, CHAR_DATA *victim)
{
	int dir, dist;
	int max_dist = 3;
	EXIT_DATA *pexit;
	ROOM_INDEX_DATA *was_in_room;
	ROOM_INDEX_DATA *to_room;
	char buf[MAX_STRING_LENGTH];
	bool pfound = false;

	if (!ch->in_room || !victim->in_room)
		return false;

	if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
		return false;

	for (dir = 0; dir <= 10; dir++)
	{
		if ((pexit = get_exit(ch->in_room, dir)) == NULL)
			continue;

		if (IS_SET(pexit->flags, EX_CLOSED))
			continue;

		was_in_room = ch->in_room;

		for (dist = 0; dist <= max_dist; dist++)
		{
			if (IS_SET(pexit->flags, EX_CLOSED))
				break;

			if (!pexit->to_room)
				break;

			to_room = NULL;
			if (pexit->distance > 1)
				to_room = generate_exit(ch->in_room, &pexit);

			if (to_room == NULL)
				to_room = pexit->to_room;

			char_from_room(ch);
			char_to_room(ch, to_room);

			if (ch->in_room == victim->in_room)
			{
				pfound = true;
				break;
			}

			if ((pexit = get_exit(ch->in_room, dir)) == NULL)
				break;

		}

		char_from_room(ch);
		char_to_room(ch, was_in_room);

		if (!pfound)
		{
			char_from_room(ch);
			char_to_room(ch, was_in_room);
			continue;
		}

		if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
			return false;

		if (is_safe(ch, victim))
			return false;

		if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
			return false;

		if (ch->position == POS_FIGHTING)
			return false;

		char_from_room(ch);
		char_to_room(ch, victim->in_room);

		sprintf(buf, "Strza� z blastera dosi�ga ci� %s.", dir_rev_name[dir]);
		act( COL_ACTION, buf, victim, NULL, ch, TO_CHAR);
		act( COL_ACTION, "Strzelasz w $N$3.", ch, NULL, victim, TO_CHAR);
		sprintf(buf, "Strza� z blastera dosi�ga $N$3 %s.", dir_rev_name[dir]);
		act( COL_ACTION, buf, ch, NULL, victim, TO_NOTVICT);

		one_hit(ch, victim, TYPE_UNDEFINED);

		if (char_died(ch))
			return true;

		stop_fighting(ch, true);

		if (victim && !char_died(victim) && victim->hit < 0)
		{
			stop_hunting(ch);
			stop_hating(ch);
		}

		char_from_room(ch);
		char_to_room(ch, was_in_room);

		return true;
	}

	return false;
}
