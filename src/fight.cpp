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
 *			    Battle & death module			   *
 ****************************************************************************/

#include <sys/types.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include "mud.h"

extern char lastplayercmd[MAX_INPUT_LENGTH];
/* gch_prev removed - using snapshot iteration instead */

/* From Skills.c */
int ris_save(CHAR_DATA *ch, int chance, int ris);

/*
 * Local functions.
 */
void dam_message args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
				int dt ) );
void death_cry args( ( CHAR_DATA *ch ) );
void group_gain args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
int xp_compute args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
int align_compute args( ( CHAR_DATA *gch, CHAR_DATA *victim ) );
ch_ret one_hit args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int obj_hitroll args( ( OBJ_DATA *obj ) );
bool get_cover(CHAR_DATA *ch);
bool dual_flip = false;

/*
 * Check to see if weapon is poisoned.
 */
bool is_wielding_poisoned(CHAR_DATA *ch)
{
	OBJ_DATA *obj;

	if ((obj = get_eq_char(ch, WEAR_WIELD))
			&& (IS_SET(obj->extra_flags, ITEM_POISONED)))
		return true;

	return false;

}

/*
 * hunting, hating and fearing code				-Thoric
 */
bool is_hunting(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (!ch->hunting || ch->hunting->who != victim)
		return false;

	return true;
}

bool is_hating(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (!ch->hating || ch->hating->who != victim)
		return false;

	return true;
}

bool is_fearing(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (!ch->fearing || ch->fearing->who != victim)
		return false;

	return true;
}

void stop_hunting(CHAR_DATA *ch)
{
	if (ch->hunting)
	{
		free_hhf(ch->hunting);
		ch->hunting = NULL;
	}
	return;
}

void stop_hating(CHAR_DATA *ch)
{
	if (ch->hating)
	{
		free_hhf(ch->hating);
		ch->hating = NULL;
	}
	return;
}

void stop_fearing(CHAR_DATA *ch)
{
	if (ch->fearing)
	{
		free_hhf(ch->fearing);
		ch->fearing = NULL;
	}
	return;
}

void start_hunting(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (ch->hunting)
		stop_hunting(ch);

	CREATE(ch->hunting, HHF_DATA, 1);
	STRDUP(ch->hunting->name, victim->name);
	ch->hunting->who = victim;
	return;
}

void start_hating(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (ch->hating)
		stop_hating(ch);

	CREATE(ch->hating, HHF_DATA, 1);
	STRDUP(ch->hating->name, victim->name);
	ch->hating->who = victim;
	return;
}

void start_fearing(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (ch->fearing)
		stop_fearing(ch);

	CREATE(ch->fearing, HHF_DATA, 1);
	STRDUP(ch->fearing->name, victim->name);
	ch->fearing->who = victim;
	return;
}

int max_fight(CHAR_DATA *ch)
{
	return 8;
}

/*
 * Control the fights going on.
 * Called periodically by update_handler.
 * Many hours spent fixing bugs in here by Thoric, as noted by residual
 * debugging checks.  If you never get any of these error messages again
 * in your logs... then you can comment out some of the checks without
 * worry.
 */
void violence_update(void)
{
	char buf[MAX_STRING_LENGTH];
	ch_ret retcode;
	CHAR_DATA *ch;
	CHAR_DATA *victim;
	SKILLTYPE *skill;

	auto violence_snapshot = char_list;
	for (auto vit = violence_snapshot.rbegin(); vit != violence_snapshot.rend(); ++vit)
	{
		ch = *vit;
		set_cur_char(ch);

		/*
		 * See if we got a pointer to someone who recently died...
		 * if so, either the pointer is bad... or it's a player who
		 * "died", and is back at the healer...
		 * Since he/she's in the char_list, it's likely to be the later...
		 * and should not already be in another fight already
		 */
		if (char_died(ch))
			continue;

		/*
		 * See if we got a pointer to some bad looking data...
		 */
		if (!ch->in_room || !ch->name)
		{
			log_string("violence_update: bad ch record!  (Shortcutting.)");

			sprintf(buf, "ch: %p  ch->in_room: %p",
					ch, ch->in_room);
			log_string(buf);

			log_string(lastplayercmd);
			continue;
		}

#if !defined( ARMAGEDDON )
		/*
		 * Experience gained during battle deceases as battle drags on
		 */
		if (ch->fighting)
			if ((++ch->fighting->duration % 24) == 0)
				ch->fighting->xp = ((ch->fighting->xp * 19) / 20);
#endif
		/*
		 #warning Wywalic to do comm.c! (sam to musze zrobic)  - Thanos
		 */
		auto timer_snapshot = ch->timers;
		for (auto* timer : timer_snapshot)
		{
			if (--timer->count <= 0)
			{
				if (timer->type == TIMER_DO_FUN)
				{
					int tempsub;

					tempsub = ch->substate;
					ch->substate = timer->value;
					// zmieniam "" na ch->dest_buf
					(timer->do_fun)(ch, ch->dest_buf);
					//		    (timer->do_fun)(ch, "" );
					if (char_died(ch))
						break;
					ch->substate = tempsub;
				}
				extract_timer(ch, timer);
			}
		}

		if (char_died(ch))
			continue;

		/*
		 * We need spells that have shorter durations than an hour.
		 * So a melee round sounds good to me... -Thoric
		 */
		auto affect_snapshot = ch->affects;
		for (auto afit = affect_snapshot.begin(); afit != affect_snapshot.end(); ++afit)
		{
			auto* paf = *afit;
			auto nxt = std::next(afit);
			AFFECT_DATA *paf_next = (nxt != affect_snapshot.end()) ? *nxt : NULL;
			if (paf->duration > 0)
				paf->duration--;
			else if (paf->duration == 0)
			{
				if (!paf_next || paf_next->type != paf->type
						|| paf_next->duration > 0)
				{
					skill = get_skilltype(paf->type);
					if (paf->type > 0 && skill && skill->msg_off)
					{
						send_to_char(COL_WEAROFF, ch);
						send_to_char(skill->msg_off, ch);
						send_to_char(EOL, ch);
					}
				}
				if (paf->type == gsn_possess)
				{
					ch->desc->character = ch->desc->original;
					ch->desc->original = NULL;
					ch->desc->character->desc = ch->desc;
					ch->desc->character->switched = NULL;
					ch->desc = NULL;
				}
				if (paf->type == gsn_dominate)
					stop_follower(ch);
				affect_remove(ch, paf);
				if (paf->type == gsn_stun)
					update_pos(ch);
			}
		}

		if ((victim = who_fighting(ch))
				== NULL|| IS_AFFECTED( ch, AFF_PARALYSIS ))
			continue;

		retcode = rNONE;

		if (IS_SET(ch->in_room->room_flags, ROOM_SAFE))
		{
			sprintf(buf, "violence_update: %s fighting %s in a SAFE room.",
					ch->name, victim->name);
			log_string(buf);
			stop_fighting(ch, true);
		}
		else if (IS_NPC( ch ) && !IS_NPC(victim) && victim->hit < 0)
		{
			stop_fighting(ch, true);
		}
		else if (IS_AWAKE(ch) && ch->in_room == victim->in_room)
			retcode = multi_hit(ch, victim, TYPE_UNDEFINED);
		else
			stop_fighting(ch, false);

		if (char_died(ch))
			continue;

		if (retcode == rCHAR_DIED || (victim = who_fighting(ch)) == NULL)
			continue;

		/*
		 *  Mob triggers
		 */
		rprog_rfight_trigger(ch);
		if (char_died(ch))
			continue;
		mprog_hitprcnt_trigger(ch, victim);
		if (char_died(ch))
			continue;
		mprog_fight_trigger(ch, victim);
		if (char_died(ch))
			continue;

		/*
		 * Fun for the whole family!
		 */
		auto room_people_snapshot = ch->in_room->people;
		for (auto* rch : room_people_snapshot)
		{
			if (IS_AWAKE(rch) && !rch->fighting)
			{
				/*
				 * PC's auto-assist others in their group.
				 */
				if (!IS_NPC(ch) || IS_AFFECTED(ch, AFF_CHARM))
				{
					if ((!IS_NPC(rch) || IS_AFFECTED(rch, AFF_CHARM))
							&& is_same_group(ch, rch))
						multi_hit(rch, victim, TYPE_UNDEFINED);
					continue;
				}

				/*
				 * NPC's assist NPC's of same type or 12.5% chance regardless.
				 */
				if (IS_NPC(
						rch) && !IS_AFFECTED(rch, AFF_CHARM) && !IS_SET(rch->act, ACT_NOASSIST))
				{
					if (char_died(ch))
						break;
					if (rch->pIndexData == ch->pIndexData
							|| number_bits(3) == 0)
					{
						CHAR_DATA *target;
						int number;

						target = NULL;
						number = 0;
						for (auto* vch : ch->in_room->people)
						{
							if (can_see(rch, vch) && is_same_group(vch, victim)
									&& number_range(0, number) == 0)
							{
								target = vch;
								number++;
							}
						}

						if (target)
							multi_hit(rch, target, TYPE_UNDEFINED);
					}
				}
			}
		}
	}

	return;
}

/*
 * Do one group of attacks.
 */
ch_ret multi_hit(CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
	int chance;
	int dual_bonus;
	ch_ret retcode;

	/* add timer if player is attacking another player */
	if (!IS_NPC(ch) && !IS_NPC(victim))
		add_timer(ch, TIMER_RECENTFIGHT, 20, NULL, 0);

	if (!IS_NPC(ch) && ch->pcdata && ch->pcdata->hotel_safe_time)
		ch->pcdata->hotel_safe_time = 0;

	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_NICE) && !IS_NPC(victim))
		return rNONE;

	if ((retcode = one_hit(ch, victim, dt)) != rNONE)
		return retcode;

	if (who_fighting(ch) != victim || dt == gsn_backstab || dt == gsn_circle)
		return rNONE;

	/* Very high chance of hitting compared to chance of going berserk */
	/* 40% or higher is always hit.. don't learn anything here though. */
	/* -- Altrag */
	chance = IS_NPC(ch) ? 100 : (ch->pcdata->learned[gsn_berserk] * 5 / 2);
	if ( IS_AFFECTED(ch, AFF_BERSERK) && number_percent() < chance)
		if ((retcode = one_hit(ch, victim, dt)) != rNONE
				|| who_fighting(ch) != victim)
			return retcode;

	if (get_eq_char(ch, WEAR_DUAL_WIELD))
	{
		dual_bonus =
		IS_NPC(ch) ?
				(ch->skill_level[COMBAT_ABILITY] / 10) :
				(ch->pcdata->learned[gsn_dual_wield] / 10);
		chance =
		IS_NPC(ch) ? ch->top_level : ch->pcdata->learned[gsn_dual_wield];
		if (number_percent() < chance)
		{
			learn_from_success(ch, gsn_dual_wield);
			retcode = one_hit(ch, victim, dt);
			if (retcode != rNONE || who_fighting(ch) != victim)
				return retcode;
		}
		else
			learn_from_failure(ch, gsn_dual_wield);
	}
	else
		dual_bonus = 0;

	if (ch->move < 10)
		dual_bonus = -20;

	/*
	 * NPC predetermined number of attacks			-Thoric
	 */
	if ( IS_NPC(ch) && ch->numattacks > 0)
	{
		for (chance = 0; chance <= ch->numattacks; chance++)
		{
			retcode = one_hit(ch, victim, dt);
			if (retcode != rNONE || who_fighting(ch) != victim)
				return retcode;
		}
		return retcode;
	}

	chance =
	IS_NPC(ch) ?
			ch->top_level :
			(int) ((ch->pcdata->learned[gsn_second_attack] + dual_bonus) / 1.5);
	if (number_percent() < chance)
	{
		learn_from_success(ch, gsn_second_attack);
		retcode = one_hit(ch, victim, dt);
		if (retcode != rNONE || who_fighting(ch) != victim)
			return retcode;
	}
	else
		learn_from_failure(ch, gsn_second_attack);

	chance =
	IS_NPC(ch) ?
			ch->top_level :
			(int) ((ch->pcdata->learned[gsn_third_attack] + (dual_bonus * 1.5))
					/ 2);
	if (number_percent() < chance)
	{
		learn_from_success(ch, gsn_third_attack);
		retcode = one_hit(ch, victim, dt);
		if (retcode != rNONE || who_fighting(ch) != victim)
			return retcode;
	}
	else
		learn_from_failure(ch, gsn_third_attack);

	retcode = rNONE;

#if defined( ARMAGEDDON )
    chance = IS_NPC(ch) ? (int) (ch->top_level / 2) : 0;
#else
	chance = IS_NPC(ch) ? (int) (ch->top_level / 4) : 0;
#endif

	if (number_percent() < chance)
		retcode = one_hit(ch, victim, dt);

	if (retcode == rNONE)
	{
		int move;

		if (!IS_AFFECTED(ch, AFF_FLYING) && !IS_AFFECTED(ch, AFF_FLOATING))
			move =
					encumbrance(ch,
							movement_loss[UMIN(SECT_MAX - 1,
									ch->in_room->sector_type)]);
		else
			move = encumbrance(ch, 1);
		if (ch->move)
			ch->move = UMAX(0, ch->move - move);
	}

	return retcode;
}

/*
 * Weapon types, haus
 */
int weapon_prof_bonus_check(CHAR_DATA *ch, OBJ_DATA *wield, int *gsn_ptr)
{
	int bonus;

	bonus = 0;
	*gsn_ptr = -1;
	if (!IS_NPC(ch) && wield)
	{
		switch (wield->value[3])
		{
		default:
			*gsn_ptr = -1;
			break;
		case 3:
			*gsn_ptr = gsn_lightsabers;
			break;
		case 2:
			*gsn_ptr = gsn_vibro_blades;
			break;
		case 4:
			*gsn_ptr = gsn_flexible_arms;
			break;
		case 5:
			*gsn_ptr = gsn_talonous_arms;
			break;
		case 6:
			*gsn_ptr = gsn_blasters;
			break;
		case 8:
			*gsn_ptr = gsn_bludgeons;
			break;
		case 9:
			*gsn_ptr = gsn_bowcasters;
			break;
		case 11:
			*gsn_ptr = gsn_force_pikes;
			break;

		}
		if (*gsn_ptr != -1)
			bonus = (int) (ch->pcdata->learned[*gsn_ptr]);
	}
	if ( IS_NPC(ch) && wield)
		bonus = get_trust(ch);
	return bonus;
}

/*
 * Calculate the tohit bonus on the object and return RIS values.
 * -- Altrag
 */
int obj_hitroll(OBJ_DATA *obj)
{
	int tohit = 0;

	for (auto* paf : obj->pIndexData->affects)
		if (paf->location == APPLY_HITROLL)
			tohit += paf->modifier;
	for (auto* paf : obj->affects)
		if (paf->location == APPLY_HITROLL)
			tohit += paf->modifier;
	return tohit;
}

/*
 * Offensive shield level modifier
 */
int off_shld_lvl(CHAR_DATA *ch, CHAR_DATA *victim)
{
	int lvl;

	if (!IS_NPC(ch)) /* players get much less effect */
	{
		lvl = UMAX(1, (ch->skill_level[FORCE_ABILITY]));
		if (number_percent() + (victim->skill_level[COMBAT_ABILITY] - lvl) < 35)
			return lvl;
		else
			return 0;
	}
	else
	{
		lvl = ch->top_level;
		if (number_percent() + (victim->skill_level[COMBAT_ABILITY] - lvl) < 70)
			return lvl;
		else
			return 0;
	}
}

#if defined(ARMAGEDDON)
bool check_weapon_state( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *wield, int dt, int *dam )
{
    bool	fail;
    int		chance;

    if( dt == (TYPE_HIT + WEAPON_BLASTER )
    && wield && wield->item_type == ITEM_WEAPON )
    {
	if ( wield->value[4] < 1  )
     	{
    	    act( PLAIN, "$n mierzy w ciebie swoim blasterem, ale nic si� nie dzieje.",  ch, NULL, victim, TO_VICT    );
            act( PLAIN, "*CLICK* ... Tw�j blaster potrzebuje amunicji!", ch, NULL, victim, TO_CHAR    );
            if ( IS_NPC(ch) )
     	        do_remove( ch, wield->name );
     	    return false;
     	}
	else
	if ( wield->blaster_setting == BLASTER_FULL && wield->value[4] >=5 )
        {
     	    *dam *=  1.5;
     	    wield->value[4] -= 5;
     	}
     	else
	if ( wield->blaster_setting == BLASTER_HIGH && wield->value[4] >=4 )
        {
     	    *dam *=  1.25;
     	    wield->value[4] -= 4;
     	}
     	else
	if ( wield->blaster_setting == BLASTER_NORMAL && wield->value[4] >=3 )
     	{
     	    wield->value[4] -= 3;
     	}
     	else
	if ( wield->blaster_setting == BLASTER_STUN && wield->value[4] >=5 )
        {
     	    *dam /= 10;
     	    wield->value[4] -= 3;
     	    fail = false;
            chance = ris_save( victim, ch->skill_level[COMBAT_ABILITY], RIS_PARALYSIS );
            if ( chance == 1000 )
               fail = true;
            else
               fail = saves_para_petri( chance, victim );

            if ( victim->was_stunned > 0 )
            {
               fail = true;
               victim->was_stunned--;
            }
     	    chance = 100 - get_curr_con(victim) - victim->skill_level[COMBAT_ABILITY]/2;

    	    /* harder for player to stun another player */
    	    if ( !IS_NPC(ch) && !IS_NPC(victim) )
              	chance -= sysdata.stun_plr_vs_plr;
    	    else
      		chance -= sysdata.stun_regular;

            chance = URANGE( 5, chance, 95 );
            if ( !fail && number_percent( ) < chance )
    	    {
		WAIT_STATE( victim, PULSE_VIOLENCE );
		act( FB_BLUE, "Niebieskie pier�cienie z blastera $N$1 powalaj� ci� na ziemi� i og�uszaj�!", victim, NULL, ch, TO_CHAR );
		act( FB_BLUE, "Niebieskie pier�cienie z twojego blastera powalaj� $N$3 na ziemi� i og�uszaj�!", ch, NULL, victim, TO_CHAR );
		act( FB_BLUE, "Niebieskie pier�cienie z blastera $n$1 powalaj� $N$3 na ziemi� i og�uszaj�!!", ch, NULL, victim, TO_NOTVICT );
		stop_fighting( victim, true );
		if ( !IS_AFFECTED( victim, AFF_PARALYSIS ) )
		{
		    AFFECT_DATA 	af;

	  	    af.type      = gsn_stun;
	  	    af.location  = APPLY_AC;
	  	    af.modifier  = 20;
	  	    af.duration  = 7;
	  	    af.bitvector = AFF_PARALYSIS;
	  	    affect_to_char( victim, &af );
	  	    update_pos( victim );
	  	    if ( IS_NPC(victim) )
	  	    {
	  		start_hating( victim, ch );
	  	        start_hunting( victim, ch );
	  	        victim->was_stunned = 10;
	  	    }
		}
    	    }
    	    else
            {
		act( FB_BLUE, "Niebieskie pier�cienie z blastera $N$1 uderzaj� ci�, ale z marnym skutkiem.", victim, NULL, ch, TO_CHAR );
		act( FB_BLUE, "Niebieskie pier�cienie z twojego blastera uderzaj� $N$3, ale z marnym skutkiem.", ch, NULL, victim, TO_CHAR );
		act( FB_BLUE, "Niebieskie pier�cienie z blastera $n$1 uderzaj� $N$3, ale z marnym skutkiem.", ch, NULL, victim, TO_NOTVICT );
            }
     	}
        else
	if ( wield->blaster_setting == BLASTER_HALF && wield->value[4] >=2 )
        {
     	    *dam *=  0.75;
     	    wield->value[4] -= 2;
     	}
        else
        {
     	    *dam *= 0.5;
     	    wield->value[4] -= 1;
     	}
    }
    else
    if ( dt == (TYPE_HIT + WEAPON_VIBRO_BLADE )
    && wield && wield->item_type == ITEM_WEAPON )
    {
     	if ( wield->value[4] < 1  )
     	{
            act( FG_YELLOW, "Twoje wibro-ostrze wymaga na�adowania...", ch, NULL, victim, TO_CHAR    );
            *dam /= 3;
     	}
    }
    else
    if ( dt == (TYPE_HIT + WEAPON_FORCE_PIKE )
    && wield && wield->item_type == ITEM_WEAPON )
    {
	if ( wield->value[4] < 1  )
     	{
            act( FG_YELLOW, "Twoja pika wymaga na�adowania...", ch, NULL, victim, TO_CHAR    );
            *dam /= 2;
     	}
     	else
     	    wield->value[4]--;
    }
    else
    if ( dt == (TYPE_HIT + WEAPON_LIGHTSABER )
    && wield && wield->item_type == ITEM_WEAPON )
    {
     	if ( wield->value[4] < 1  )
     	{
            act( FG_YELLOW, "$n macha r�koj�ci� swojego miecza.",  ch, NULL, victim, TO_VICT    );
            act( FG_YELLOW, "Musisz na�adowa� sw�j miecz. Wygl�da na to, �e jego ostrze znikn�o.", ch, NULL, victim, TO_CHAR    );
     	    if ( IS_NPC(ch) )
     	        do_remove( ch, wield->name );
     	    return false;
     	}
     }
     else if ( dt == (TYPE_HIT + WEAPON_BOWCASTER ) && wield && wield->item_type == ITEM_WEAPON )
     {
     	if ( wield->value[4] < 1  )
     	{
            act( FG_YELLOW, "$n wymierza swoj� kusz�, klika na spu�cie, ale nic si� nie dzieje.",  ch, NULL, victim, TO_VICT    );
            act( FG_YELLOW, "*CLICK* ... Twoja kusza potrzebuje pocisk�w!", ch, NULL, victim, TO_CHAR    );
            if ( IS_NPC(ch) )
     	        do_remove( ch, wield->name );
            return false;
     	}
     	else
          wield->value[4]--;
    }

    return true;
}

int check_attacktype( CHAR_DATA *ch, OBJ_DATA *wield, int dt, int *retcode )
{
    int 	attacktype=0, cnt;
    int		x;

    if ( ch->fighting		/* make sure fight is already started */
    &&   dt == TYPE_UNDEFINED
    &&   IS_NPC(ch)
    &&   ch->attacks != 0 )
    {
	cnt = 0;
	for ( ;; )
	{
	    x = number_range( 0, 6 );

	    attacktype = 1 << x;

	    if ( IS_SET( ch->attacks, attacktype ) )
		break;

	    if ( cnt++ > 16 )
	    {
		attacktype = 0;
	        break;
	    }
	}

	if ( attacktype == ATCK_BACKSTAB )
	    attacktype = 0;

	if ( wield && number_percent( ) > 25 )
	    attacktype = 0;

	switch ( attacktype )
	{
	    default:
		break;
	    case ATCK_BITE:
		do_bite( ch, "" );
		*retcode = global_retcode;
		break;
	    case ATCK_CLAWS:
		do_claw( ch, "" );
		*retcode = global_retcode;
		break;
	    case ATCK_TAIL:
		do_tail( ch, "" );
		*retcode = global_retcode;
		break;
	    case ATCK_STING:
		do_sting( ch, "" );
		*retcode = global_retcode;
		break;
	    case ATCK_PUNCH:
		do_punch( ch, "" );
		*retcode = global_retcode;
		break;
	    case ATCK_KICK:
		do_kick( ch, "" );
		*retcode = global_retcode;
		break;
	    case ATCK_TRIP:
		attacktype = 0;
		break;
	}
    }
    return attacktype;
}


/*
 * Hit one guy once.
 */
ch_ret one_hit( CHAR_DATA *ch, CHAR_DATA *victim, int dt )
{
	OBJ_DATA *wield;
	int victim_ac;
	int thac0;
	int thac0_00;
	int thac0_100;
	int dam;
	int diceroll;
	ch_ret retcode;

    /*
	* Can't beat a dead char!
	* Guard against weird room-leavings.
	*/
	if ( victim->position == POS_DEAD || ch->in_room != victim->in_room )
		return rVICT_DIED;

    /*
	* Figure out the type of damage message.
	*/
	wield = get_eq_char( ch, WEAR_WIELD );
	if ( dt == TYPE_UNDEFINED )
	{
		dt = TYPE_HIT;
		if ( wield != NULL && wield->item_type == ITEM_WEAPON )
			dt += wield->value[3];
	}

    /*
	* Calculate to-hit-armor-class-0 versus armor.
	*/
	if ( IS_NPC(ch) )
	{
		thac0_00 = 20;
		thac0_100 =  0;
	}
	else
	{
		thac0_00 = class_table[ch->main_ability].thac0_00;
		thac0_100 = class_table[ch->main_ability].thac0_100;
	}
	thac0     = interpolate( fight_lev(ch), thac0_00, thac0_100 ) - GET_HITROLL(ch);
	victim_ac = UMAX( -15, GET_AC(victim) / 10 );
	if ( !can_see( ch, victim ) )
		victim_ac -= 4;

    /*
	* The moment of excitement!
	*/
	while ( ( diceroll = number_bits( 5 ) ) >= 20 )
		;

	if ( diceroll == 0
			|| ( diceroll != 19 && diceroll < thac0 - victim_ac ) )
	{
		/* Miss. */
		damage( ch, victim, 0, dt );
		tail_chain( );
		return rNONE;
	}

    /*
	* Hit.
	* Calc damage.
	*/
	if ( IS_NPC(ch) )
	{
		dam = number_range( fight_lev(ch) / 2, fight_lev(ch) * 3 / 2 );
		if ( wield != NULL )
			dam += dam / 2;
	}
	else
	{
		if ( wield != NULL )
			dam = number_range( wield->value[1], wield->value[2] );
		else
			dam = number_range( 1, 4 );
	}

    /*
	* Bonuses.
	*/
	dam += GET_DAMROLL(ch);
	if ( !IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0 )
		dam += dam * ch->pcdata->learned[gsn_enhanced_damage] / 150;
	if ( !IS_AWAKE(victim) )
		dam *= 2;
	if ( dt == gsn_backstab )
		dam *= 2 + fight_lev(ch) / 8;

	if ( dam <= 0 )
		dam = 1;

	retcode = damage( ch, victim, dam, dt );
	tail_chain();
	return retcode;
}

#else

ch_ret one_hit(CHAR_DATA *ch, CHAR_DATA *victim, int dt)
{
	OBJ_DATA *wield;
	int victim_ac;
	int thac0;
	int thac0_00;
	int thac0_32;
	int plusris;
	int dam, x;
	int diceroll;
	int attacktype, cnt;
	int prof_bonus;
	int prof_gsn;
	ch_ret retcode = 0;
	int chance;
	bool fail;
	AFFECT_DATA af;
	OBJ_DATA *lightsaber; //byTrog

	/*
	 * Can't beat a dead char!
	 * Guard against weird room-leavings.
	 */
	if (victim->position == POS_DEAD || ch->in_room != victim->in_room)
		return rVICT_DIED;

	/*
	 * Figure out the weapon doing the damage			-Thoric
	 */
	if ((wield = get_eq_char(ch, WEAR_DUAL_WIELD)) != NULL)
	{
		if (dual_flip == false)
		{
			dual_flip = true;
			wield = get_eq_char(ch, WEAR_WIELD);
		}
		else
			dual_flip = false;
	}
	else
		wield = get_eq_char(ch, WEAR_WIELD);

	prof_bonus = weapon_prof_bonus_check(ch, wield, &prof_gsn);

	if (ch->fighting /* make sure fight is already started */
	&& dt == TYPE_UNDEFINED && IS_NPC(ch) && ch->attacks != 0)
	{
		cnt = 0;
		for (;;)
		{
			x = number_range(0, 6);
			attacktype = 1 << x;
			if (IS_SET(ch->attacks, attacktype))
				break;
			if (cnt++ > 16)
			{
				attacktype = 0;
				break;
			}
		}
		if (attacktype == ATCK_BACKSTAB)
			attacktype = 0;
		if (wield && number_percent() > 25)
			attacktype = 0;
		switch (attacktype)
		{
		default:
			break;
		case ATCK_BITE:
			do_bite(ch, (char*) "");
			retcode = global_retcode;
			break;
		case ATCK_CLAWS:
			do_claw(ch, (char*) "");
			retcode = global_retcode;
			break;
		case ATCK_TAIL:
			do_tail(ch, (char*) "");
			retcode = global_retcode;
			break;
		case ATCK_STING:
			do_sting(ch, (char*) "");
			retcode = global_retcode;
			break;
		case ATCK_PUNCH:
			do_punch(ch, (char*) "");
			retcode = global_retcode;
			break;
		case ATCK_KICK:
			do_kick(ch, (char*) "");
			retcode = global_retcode;
			break;
		case ATCK_TRIP:
			attacktype = 0;
			break;
		}
		if (attacktype)
			return retcode;
	}

	if (dt == TYPE_UNDEFINED)
	{
		dt = TYPE_HIT;
		if (wield && wield->item_type == ITEM_WEAPON)
			dt += wield->value[3];
	}

	/*
	 * Calculate to-hit-armor-class-0 versus armor.
	 */
	thac0_00 = 20;
#if defined(ARMAGEDDON)
    thac0_32 = -4;
#else
	thac0_32 = 10;
#endif
	thac0 = interpolate(ch->skill_level[COMBAT_ABILITY], thac0_00, thac0_32);

	if (thac0 < 0)
		thac0 = thac0 / 2;
	if (thac0 < -5)
		thac0 = -5 + (thac0 + 5) / 2;

	thac0 -= GET_HITROLL(ch) * prof_bonus / 100;
	thac0 += 5 * (100 - prof_bonus) / 100;

	victim_ac = (int) (GET_AC(victim) / 10);

	if (victim_ac < -15)
		victim_ac = (victim_ac + 15) / 5 - 15;

	/* if you can't see what's coming... */
	if (wield && !can_see_obj(victim, wield))
		victim_ac += 1;

	if (!can_see(ch, victim))
		victim_ac -= 4;

	if (isDefelInvisible(ch))
		victim_ac += 2;

	if (!IS_AWAKE(victim))
		victim_ac += 5;

	/* Weapon proficiency bonus */
	victim_ac += prof_bonus / 20;

	diceroll = number_range(0, 30);
	if (diceroll == 0 || (diceroll < 19 && diceroll < thac0 - victim_ac))
	{
		/* Miss. */
		if (prof_gsn != -1)
			learn_from_failure(ch, prof_gsn);
		if (wield && IS_SET(wield->wear_flags, ITEM_DUAL_WIELD))
			learn_from_failure(ch, gsn_twohanded_wield);
		damage(ch, victim, 0, dt);
		tail_chain();
		return rNONE;
	}

	/*
	 * Hit.
	 * Calc damage.
	 */
#if defined(ARMAGEDDON)
    if ( IS_NPC(ch) )
    {
        dam = number_range( fight_lev(ch) / 2, fight_lev(ch) * 3 / 2 );
    	    if ( wield != NULL )
        	dam += dam / 2;
    }
    else
    {
	if (prof_gsn != -1)
	    learn_from_success(ch,prof_gsn);

	if ( wield != NULL )
	{
	    dam = number_range( wield->value[1] * prof_bonus/100,
				wield->value[2] * prof_bonus/100);
	}
	else
	    dam = number_range( fight_lev(ch) / 2, fight_lev(ch) * 3 / 2 );
    }

/*    if ( IS_NPC( ch ) )
    {
	dam = number_range( fight_lev(ch)/ 2, fight_lev(ch) * 2 )*2;

	if ( wield )
	    dam += dam / 2;
    }
    else
    {*/
#else
	if (!wield)       // dice formula fixed by Thoric
	{
		dam = number_range(ch->barenumdie, ch->baresizedie * ch->barenumdie);
	}
	else
		dam = number_range(wield->value[1] * 2 / 3, wield->value[2] * 2 / 3);
#endif
//    }
	/*
	 * Bonuses.
	 */

	dam += GET_DAMROLL(ch);

//    if( IS_NPC( ch ) )
//	dam *= 1.5;

	if (prof_bonus)
		dam *= (1 + prof_bonus / 100);

	if (!IS_NPC(ch) && ch->pcdata->learned[gsn_enhanced_damage] > 0)
	{
		dam += (int) (dam * ch->pcdata->learned[gsn_enhanced_damage] / 120);
		learn_from_success(ch, gsn_enhanced_damage);
	}

	if (!IS_AWAKE(victim))
		dam *= 2;
	if (dt == gsn_backstab)
		dam *=
				(2
						+ URANGE(2,
								ch->skill_level[HUNTING_ABILITY] - (victim->skill_level[COMBAT_ABILITY]/4),
								30) / 8);

	if (dt == gsn_circle)
		dam *=
				(2
						+ URANGE(2,
								ch->skill_level[HUNTING_ABILITY] - (victim->skill_level[COMBAT_ABILITY]/4),
								30) / 16);

	plusris = 0;

	if (wield)
	{
		if (IS_SET(wield->extra_flags, ITEM_FORCE))
			dam = ris_damage(victim, dam, RIS_FORCE);
		else
			dam = ris_damage(victim, dam, RIS_NONFORCE);

		/*
		 * Handle PLUS1 - PLUS6 ris bits vs. weapon hitroll	-Thoric
		 */
		plusris = obj_hitroll(wield);
	}
	else
		dam = ris_damage(victim, dam, RIS_NONFORCE);

	/* check for RIS_PLUSx 					-Thoric */
	if (dam)
	{
		int x, res, imm, sus, mod;

		if (plusris)
			plusris = RIS_PLUS1 << UMIN(plusris, 7);

		/* initialize values to handle a zero plusris */
		imm = res = -1;
		sus = 1;

		/* find high ris */
		for (x = RIS_PLUS1; x <= RIS_PLUS6; x <<= 1)
		{
			if (IS_SET(victim->immune, x))
				imm = x;
			if (IS_SET(victim->resistant, x))
				res = x;
			if (IS_SET(victim->susceptible, x))
				sus = x;
		}
		mod = 10;
		if (imm >= plusris)
			mod -= 10;
		if (res >= plusris)
			mod -= 2;
		if (sus <= plusris)
			mod += 2;

		/* check if immune */
		if (mod <= 0)
			dam = -1;
		if (mod != 10)
			dam = (dam * mod) / 10;
	}

	/* race modifier */

	if (IS_RACE(victim, "DUINUOGWUIN"))
		dam /= 5;

	/*
	 * check to see if weapon is charged
	 */

	if (dt == (TYPE_HIT + WEAPON_BLASTER) && wield
			&& wield->item_type == ITEM_WEAPON)
	{
		if (wield->value[4] < 1)
		{
			act( PLAIN,
					"$n mierzy w ciebie swoim blasterem, ale nic si� nie dzieje.",
					ch, NULL, victim, TO_VICT);
			act( PLAIN, "*CLICK* ... Tw�j blaster potrzebuje amunicji!", ch,
					NULL, victim, TO_CHAR);
			if (IS_NPC(ch))
			{
				do_remove(ch, wield->name);
			}
			return rNONE;
		}
		else if (wield->blaster_setting == BLASTER_FULL && wield->value[4] >= 5)
		{
			dam *= 1.5;
			wield->value[4] -= 5;
		}
		else if (wield->blaster_setting == BLASTER_HIGH && wield->value[4] >= 4)
		{
			dam *= 1.25;
			wield->value[4] -= 4;
		}
		else if (wield->blaster_setting == BLASTER_NORMAL
				&& wield->value[4] >= 3)
		{
			wield->value[4] -= 3;
		}
		else if (wield->blaster_setting == BLASTER_STUN && wield->value[4] >= 5)
		{
			dam /= 10;
			wield->value[4] -= 3;
			fail = false;
			chance = ris_save(victim, ch->skill_level[COMBAT_ABILITY],
			RIS_PARALYSIS);
			if (chance == 1000)
				fail = true;
			else
				fail = saves_para_petri(chance, victim);
			if (victim->was_stunned > 0)
			{
				fail = true;
				victim->was_stunned--;
			}
			chance = 100 - get_curr_con(victim)
					- victim->skill_level[COMBAT_ABILITY] / 2;
			/* harder for player to stun another player */
			if (!IS_NPC(ch) && !IS_NPC(victim))
				chance -= sysdata.stun_plr_vs_plr;
			else
				chance -= sysdata.stun_regular;
			chance = URANGE(5, chance, 95);
			if (!fail && number_percent() < chance)
			{
				WAIT_STATE(victim, PULSE_VIOLENCE);
				act( FB_BLUE,
						"Niebieskie pier�cienie z blastera $N$1 powalaj� ci� na ziemi� i og�uszaj�!",
						victim, NULL, ch, TO_CHAR);
				act( FB_BLUE,
						"Niebieskie pier�cienie z twojego blastera powalaj� $N$3 na ziemi� i og�uszaj�!",
						ch, NULL, victim, TO_CHAR);
				act( FB_BLUE,
						"Niebieskie pier�cienie z blastera $n$1 powalaj� $N$3 na ziemi� i og�uszaj�!!",
						ch, NULL, victim, TO_NOTVICT);
				stop_fighting(victim, true);
				if (!IS_AFFECTED(victim, AFF_PARALYSIS))
				{
					af.type = gsn_stun;
					af.location = APPLY_AC;
					af.modifier = 20;
					af.duration = 7;
					af.bitvector = AFF_PARALYSIS;
					affect_to_char(victim, &af);
					update_pos(victim);
					if (IS_NPC(victim))
					{
						start_hating(victim, ch);
						start_hunting(victim, ch);
						victim->was_stunned = 10;
					}
				}
			}
			else
			{
				act( FB_BLUE,
						"Niebieskie pier�cienie energii z blastera $N$1 trafiaj� ci� ale z mizernym efektem.",
						victim, NULL, ch, TO_CHAR);
				act( FB_BLUE,
						"Niebieskie pier�cienie energii z wtojego blastera trafiaj� $N$3, jednak nic si� nie dzieje!",
						ch, NULL, victim, TO_CHAR);
				act( FB_BLUE,
						"Niebieskie pier�cienie energii z blastera $N$1 trafiaj� $N$3, jednak nic si� nie dzieje!",
						ch, NULL, victim, TO_NOTVICT);

			}
		}
		else if (wield->blaster_setting == BLASTER_HALF && wield->value[4] >= 2)
		{
			dam *= 0.75;
			wield->value[4] -= 2;
		}
		else
		{
			dam *= 0.5;
			wield->value[4] -= 1;
		}

	}
	else if (dt == (TYPE_HIT + WEAPON_VIBRO_BLADE) && wield
			&& wield->item_type == ITEM_WEAPON)
	{
		if (wield->value[4] < 1)
		{
			act( FG_YELLOW, "Twoje wibro-ostrze wymaga na�adowania...", ch,
					NULL, victim, TO_CHAR);
			dam /= 3;
		}
	}
	else if (dt == (TYPE_HIT + WEAPON_FORCE_PIKE) && wield
			&& wield->item_type == ITEM_WEAPON)
	{
		if (wield->value[4] < 1)
		{
			act( FG_YELLOW, "Twoja force-pike wymaga na�adowania...", ch, NULL,
					victim, TO_CHAR);
			dam /= 2;
		}
		else
			wield->value[4]--;
	}
	else if (dt == (TYPE_HIT + WEAPON_LIGHTSABER) && wield
			&& wield->item_type == ITEM_WEAPON)
	{
		if (wield->value[4] < 1)
		{
			act( FG_YELLOW, "$n wykonuje r�k� jakie� dziwne wymachy.", ch, NULL,
					victim, TO_VICT);
			act( FG_YELLOW,
					"Musisz na�adowa� sw�j miecz �wietlny... brakuje mu ostrza.",
					ch, NULL, victim, TO_CHAR);
			if (IS_NPC(ch))
			{
				do_remove(ch, wield->name);
			}
			return rNONE;
		}
	}
	else if (dt == (TYPE_HIT + WEAPON_BOWCASTER) && wield
			&& wield->item_type == ITEM_WEAPON)
	{
		if (wield->value[4] < 1)
		{
			act( FG_YELLOW,
					"$n celuje swoj� kusz� w ciebie, ale nic si� nie dzieje.",
					ch, NULL, victim, TO_VICT);
			act( FG_YELLOW, "*CLICK* ... Twoja kusza nie ma amunicji!", ch,
					NULL, victim, TO_CHAR);
			if (IS_NPC(ch))
			{
				do_remove(ch, wield->name);
			}
			return rNONE;
		}
		else
			wield->value[4]--;
	}

	/* Added by Thanos - to nieznacznie przed�u�a walk� i czyni j� bardziej
	 emocjonuj�c� - dotyczy mob�w i graczy wi�c jest sprawiedliwe        */
//        dam/=2; /* Trog nie koniecznie :P */
	if (dam <= 0)
		dam = 1;

	if (prof_gsn != -1)
	{
		if (dam > 0)
			learn_from_success(ch, prof_gsn);
		else
			learn_from_failure(ch, prof_gsn);
	}

	if (wield && IS_SET(wield->wear_flags, ITEM_DUAL_WIELD))
		learn_from_success(ch, gsn_twohanded_wield);

	/* immune to damage */
	if (dam == -1)
	{
		if (dt >= 0 && dt < top_sn)
		{
			SKILLTYPE *skill = skill_table[dt];
			bool found = false;

			if (skill->imm_char && skill->imm_char[0] != '\0')
			{
				act( COL_HIT, skill->imm_char, ch, NULL, victim, TO_CHAR);
				found = true;
			}
			if (skill->imm_vict && skill->imm_vict[0] != '\0')
			{
				act( COL_HITME, skill->imm_vict, ch, NULL, victim, TO_VICT);
				found = true;
			}
			if (skill->imm_room && skill->imm_room[0] != '\0')
			{
				act( COL_ACTION, skill->imm_room, ch, NULL, victim, TO_NOTVICT);
				found = true;
			}
			if (found)
				return rNONE;
		}
		dam = 0;
	}
//byTrog
	if (((lightsaber = get_eq_char(victim, WEAR_WIELD)) != NULL)
			&& lightsaber->value[3] == WEAPON_LIGHTSABER
			&& !IS_NPC(
					victim) && ((lightsaber = get_eq_char( ch, WEAR_WIELD )) != NULL)
					&& lightsaber->value[3] == WEAPON_BLASTER)
	{
		if ((int) victim->pcdata->learned[gsn_lightsabers] == 100)
		{
			act( PLAIN, "$n odbija mieczem strza� z blastera.", victim, NULL,
					ch, TO_ROOM);
			act( PLAIN, "Odbijasz mieczem strza� z blastera.", ch, NULL, victim,
			TO_VICT);
			act( PLAIN, "Odbity strza� z blastera trafia ci�.", victim, NULL,
					ch, TO_VICT);
			act( PLAIN, "Odbity strza� z blastera trafia $n$1.", ch, NULL,
					victim, TO_ROOM);
			if ((retcode = damage(victim, ch, dam, dt)) != rNONE)
				return retcode;
		}
		else if ((int) victim->pcdata->learned[gsn_lightsabers] > 90)
		{
			act( PLAIN, "$n odbija mieczem strza� z blastera.", victim, NULL,
					ch, TO_ROOM);
			act( PLAIN, "Odbijasz mieczem strza� z blastera.", ch, NULL, victim,
			TO_VICT);
			if (!ch->fighting)
				set_fighting(ch, victim);
		}
		else
		{
			if ((retcode = damage(ch, victim, dam, dt)) != rNONE)
				return retcode;
		}
	}
	else
	{
		if ((retcode = damage(ch, victim, dam, dt)) != rNONE)
			return retcode;
	}
	/* to bylo wczesniej
	 if ( (retcode = damage( ch, victim, dam, dt )) != rNONE )
	 return retcode;
	 koniec */
	if (char_died(ch))
		return rCHAR_DIED;
	if (char_died(victim))
		return rVICT_DIED;

	retcode = rNONE;
	if (dam == 0)
		return retcode;

	/* weapon spells	-Thoric */
	if (wield
			&& !IS_SET(victim->immune,
					RIS_FORCE) && !IS_SET(victim->in_room->room_flags, ROOM_NO_FORCE))
	{
		for (auto* aff : wield->pIndexData->affects)
			if (aff->location == APPLY_WEAPONSPELL && IS_VALID_SN(aff->modifier)
					&& skill_table[aff->modifier]->spell_fun)
				retcode = (*skill_table[aff->modifier]->spell_fun)(
						aff->modifier, (wield->level + 3) / 3, ch, victim);
		if (retcode != rNONE || char_died(ch) || char_died(victim))
			return retcode;
		for (auto* aff : wield->affects)
			if (aff->location == APPLY_WEAPONSPELL && IS_VALID_SN(aff->modifier)
					&& skill_table[aff->modifier]->spell_fun)
				retcode = (*skill_table[aff->modifier]->spell_fun)(
						aff->modifier, (wield->level + 3) / 3, ch, victim);
		if (retcode != rNONE || char_died(ch) || char_died(victim))
			return retcode;
	}

	/*
	 * force shields that retaliate				-Thoric
	 */
	if (retcode != rNONE || char_died(ch) || char_died(victim))
		return retcode;

	if (retcode != rNONE || char_died(ch) || char_died(victim))
		return retcode;

	if ( IS_AFFECTED(victim,
			AFF_SHOCKSHIELD) && !IS_AFFECTED(ch, AFF_SHOCKSHIELD))
		retcode = spell_lightning_bolt(gsn_lightning_bolt,
				off_shld_lvl(victim, ch), victim, ch);
	if (retcode != rNONE || char_died(ch) || char_died(victim))
		return retcode;

	/*
	 *   folks with blasters move and snipe instead of getting neatin up in one spot.
	 */
	if (IS_NPC(victim))
	{
		OBJ_DATA *wield;

		wield = get_eq_char(victim, WEAR_WIELD);
		if (wield != NULL && wield->value[3] == WEAPON_BLASTER
				&& get_cover(victim) == true)
		{
			start_hating(victim, ch);
			start_hunting(victim, ch);
		}
	}

	tail_chain();
	return retcode;
}
#endif
/*
 * Calculate damage based on resistances, immunities and suceptibilities
 *					-Thoric
 */
int ris_damage(CHAR_DATA *ch, int dam, int ris)
{
	int modifier;

	modifier = 10;
	if (IS_SET(ch->immune, ris))
		modifier -= 10;
	if (IS_SET(ch->resistant, ris))
		modifier -= 5;
	if (IS_SET(ch->susceptible, ris))
		modifier += 5;
	if (modifier <= 0)
		return -1;
	if (modifier == 10)
		return dam;
	return (dam * modifier) / 10;
}

/*
 * Inflict damage from a hit.
 */
ch_ret damage(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt)
{
	char buf1[MAX_STRING_LENGTH];
	int dameq;
	bool npcvict;
	bool loot;
	int xp_gain;
	OBJ_DATA *damobj;
	ch_ret retcode;
	int dampmod;

	int init_gold, new_gold, gold_diff;

	retcode = rNONE;

	IF_BUG(ch == NULL, "")
		return rERROR;

	IF_BUG(victim == NULL, "")
		return rVICT_DIED;

//Trog: chwilowo?
	IF_BUG(victim->position == POS_DEAD, "Trog: chwilowo?")
		return rVICT_DIED;

	npcvict = IS_NPC(victim);

	/*
	 * Check damage types for RIS				-Thoric
	 */
	if (dam && dt != TYPE_UNDEFINED)
	{
		if (IS_FIRE(dt))
			dam = ris_damage(victim, dam, RIS_FIRE);
		else if (IS_COLD(dt))
			dam = ris_damage(victim, dam, RIS_COLD);
		else if (IS_ACID(dt))
			dam = ris_damage(victim, dam, RIS_ACID);
		else if (IS_ELECTRICITY(dt))
			dam = ris_damage(victim, dam, RIS_ELECTRICITY);
		else if (IS_ENERGY(dt))
			dam = ris_damage(victim, dam, RIS_ENERGY);
		else if (IS_DRAIN(dt))
			dam = ris_damage(victim, dam, RIS_DRAIN);
		else if (dt == gsn_poison || IS_POISON(dt))
			dam = ris_damage(victim, dam, RIS_POISON);
		else if (dt == (TYPE_HIT + 7) || dt == (TYPE_HIT + 8))
			dam = ris_damage(victim, dam, RIS_BLUNT);
		else if (dt == (TYPE_HIT + 2) || dt == (TYPE_HIT + 11)
				|| dt == (TYPE_HIT + 10))
			dam = ris_damage(victim, dam, RIS_PIERCE);
		else if (dt == (TYPE_HIT + 1) || dt == (TYPE_HIT + 3)
				|| dt == (TYPE_HIT + 4) || dt == (TYPE_HIT + 5))
			dam = ris_damage(victim, dam, RIS_SLASH);

		if (dam == -1)
		{
			if (dt >= 0 && dt < top_sn)
			{
				bool found = false;
				SKILLTYPE *skill = skill_table[dt];

				if (skill->imm_char && skill->imm_char[0] != '\0')
				{
					act( COL_HIT, skill->imm_char, ch, NULL, victim, TO_CHAR);
					found = true;
				}
				if (skill->imm_vict && skill->imm_vict[0] != '\0')
				{
					act( COL_HITME, skill->imm_vict, ch, NULL, victim, TO_VICT);
					found = true;
				}
				if (skill->imm_room && skill->imm_room[0] != '\0')
				{
					act( COL_ACTION, skill->imm_room, ch, NULL, victim,
					TO_NOTVICT);
					found = true;
				}
				if (found)
					return rNONE;
			}
			dam = 0;
		}
	}

	if (dam && npcvict && ch != victim)
	{
		if (!IS_SET(victim->act, ACT_SENTINEL))
		{
			if (victim->hunting)
			{
				if (victim->hunting->who != ch)
				{
					STRDUP(victim->hunting->name, ch->name);
					victim->hunting->who = ch;
				}
			}
			else
				start_hunting(victim, ch);
		}

		if (victim->hating)
		{
			if (victim->hating->who != ch)
			{
				STRDUP(victim->hating->name, ch->name);
				victim->hating->who = ch;
			}
		}
		else
			start_hating(victim, ch);
	}

	if (victim != ch)
	{
		/*
		 * Certain attacks are forbidden.
		 * Most other attacks are returned.
		 */
		if (is_safe(ch, victim))
			return rNONE;

		/* Tylko na rozpocz�ciu walki: */
		if (!ch->fighting || who_fighting(ch) != victim)
		{
			add_inform(victim, ch);
		}

		if (victim->position > POS_STUNNED)
		{
			if (!victim->fighting)
				set_fighting(victim, ch);
			if (victim->fighting)
				victim->position = POS_FIGHTING;
			fevent_trigger(ch, FE_POSITION);
		}

		if (victim->position > POS_STUNNED)
		{
			if (!ch->fighting)
				set_fighting(ch, victim);

			/*
			 * If victim is charmed, ch might attack victim's master.
			 */
			if ( IS_NPC(ch) && npcvict && IS_AFFECTED(victim, AFF_CHARM)
					&& victim->master && victim->master->in_room == ch->in_room
					&& number_bits(3) == 0)
			{
				stop_fighting(ch, false);
				retcode = multi_hit(ch, victim->master, TYPE_UNDEFINED);
				return retcode;
			}
		}

		/*
		 * More charm stuff.
		 */
		if (victim->master == ch)
			stop_follower(victim);

		/*
		 * Inviso attacks ... not.
		 */
		if ( IS_AFFECTED(ch, AFF_INVISIBLE) || IS_AFFECTED(ch, AFF_FORCEINVIS))
		{
			if (!isDefelInvisible(ch))
			{
				affect_strip(ch, gsn_invis);
				REMOVE_BIT(ch->affected_by, AFF_INVISIBLE);
			}
			affect_strip(ch, gsn_forceinvis);
			REMOVE_BIT(ch->affected_by, AFF_FORCEINVIS);
			act( PLAIN, "$n pojawia si� jakby znik�d!", ch, NULL, NULL,
			TO_ROOM);
		}

		/* Take away Hide */
		if (IS_AFFECTED(ch, AFF_HIDE))
			REMOVE_BIT(ch->affected_by, AFF_HIDE);
		/*
		 * Damage modifiers.
		 */
		if (IS_AFFECTED(victim, AFF_SANCTUARY))
			dam /= 2;

		if ( IS_AFFECTED(victim, AFF_PROTECT) && IS_EVIL(ch))
			dam -= (int) (dam / 5);

		if (dam < 0)
			dam = 0;

		/*
		 * Check for disarm, trip, parry, and dodge.
		 */
		if (dt >= TYPE_HIT && ch->in_room == victim->in_room)
		{
			int leveldiff = fight_lev(ch) - fight_lev(victim);

			if ( IS_NPC(ch)
					&& ( IS_SET(ch->attacks, DFND_DISARM)
							|| number_percent() < 30)
					&& number_percent()
							< (leveldiff < -5 ?
									fight_lev(ch) / 2 : UMAX(10, leveldiff))
					&& dam == 0)
				/*    &&   number_percent( ) < ch->skill_level[COMBAT_ABILITY] / 2 )*/
				disarm(ch, victim);

			if ((IS_NPC(ch)
					&& number_percent()
							< (leveldiff < -5 ?
									fight_lev(ch) / 2 : UMAX(20, leveldiff))
					&& ( IS_SET(ch->attacks, ATCK_TRIP) || number_percent() < 30)
					&& dam == 0) && (dt != ( TYPE_HIT + WEAPON_BLASTER)))
				/*	    &&   number_percent( ) < ch->skill_level[COMBAT_ABILITY] )*/
				trip(ch, victim);

			if (check_parry(ch, victim))
				return rNONE;
			if (check_dodge(ch, victim))
				return rNONE;
		}

		/*
		 * Check control panel settings and modify damage
		 */
		if (IS_NPC(ch))
		{
			if (npcvict)
				dampmod = sysdata.dam_mob_vs_mob;
			else
				dampmod = sysdata.dam_mob_vs_plr;
		}
		else
		{
			if (npcvict)
				dampmod = sysdata.dam_plr_vs_mob;
			else
				dampmod = sysdata.dam_plr_vs_plr;
		}
		if (dampmod > 0)
			dam = (dam * dampmod) / 100;

		/*dam_message( ch, victim, dam, dt );*/

	}

	/* przenios�em to tutaj, �eby gracze mogli widzie�
	 gdy atakuj� sami siebie 					-- Thanos */
	if (dt != TYPE_UNDEFINED)
		dam_message(ch, victim, dam, dt);

	/*
	 * Code to handle equipment getting damaged, and also support  -Thoric
	 * bonuses/penalties for having or not having equipment where hit
	 */

	/*
	 if( !str_cmp( victim->name, "Yarn" ) )
	 ch_printf( victim, FB_YELLOW "On: %d" EOL, dam );

	 if( !str_cmp( ch->name, "Yarn" ) )
	 ch_printf( ch, FB_BLUE "Ty: %d" EOL, dam );
	 */
	if (dam > 10 && dt != TYPE_UNDEFINED)
	{

		/* get a random body eq part */
		dameq = number_range(WEAR_LIGHT, WEAR_EYES);
		damobj = get_eq_char(victim, dameq);
		if (damobj)
		{
			if (dam > get_obj_resistance(damobj))
			{
				set_cur_obj(damobj);
				damage_obj(damobj);
			}
			dam -= dam / 5; /* add a bonus for having something to block the blow */
		}
		else
			dam += dam / 5; /* add penalty for bare skin! */
	}

	/*
	 * Hurt the victim.
	 * Inform the victim of his new state.
	 */

	victim->hit -= dam;

	/*
	 * Get experience based on % of damage done			-Thoric
	 */
	if (dam && ch != victim && !IS_NPC(ch) && ch->fighting && ch->fighting->xp)
	{
		xp_gain = (int) (xp_compute(ch, victim) * 0.1 * dam) / victim->max_hit;
		gain_exp(ch, xp_gain, COMBAT_ABILITY);
	}

	if (!IS_NPC(victim) && victim->top_level >= LEVEL_IMMORTAL
			&& victim->hit < 1)
		victim->hit = 1;

	/* Make sure newbies dont die */

	if (!IS_NPC(victim) && NOT_AUTHED(victim) && victim->hit < 1)
		victim->hit = 1;

	if (dam > 0 && dt > TYPE_HIT && !IS_AFFECTED(victim, AFF_POISON)
			&& is_wielding_poisoned(ch) && !IS_SET(victim->immune, RIS_POISON)
			&& !saves_poison_death(ch->skill_level[COMBAT_ABILITY], victim))
	{
		AFFECT_DATA af;

		af.type = gsn_poison;
		af.duration = 20;
		af.location = APPLY_STR;
		af.modifier = -2;
		af.bitvector = AFF_POISON;
		affect_join(victim, &af);
		ch->mental_state = URANGE(20, ch->mental_state + 2, 100);
	}

	if (get_trust(victim) >= LEVEL_IMMORTAL && !npcvict
			&& get_trust(ch) >= LEVEL_IMMORTAL && victim->hit < 1)
		victim->hit = 1;

	update_pos(victim);

	if (victim->hit < 1 && IS_SET(victim->in_room->room_flags, ROOM_SAFEARENA)) //Trog
	{
		stop_fighting(victim, true);
		victim->hit = 1;
		update_pos(victim);

		act( FG_GREEN, "Tw�j pojedynek z $N$4 dobieg� ko�ca. Jeste� zwyci�zc�.",
				ch, NULL, victim, TO_CHAR);
		act( FG_RED, "Tw�j pojedynek z $n$4 dobieg� ko�ca. Jeste� przegranym.",
				ch, NULL, victim, TO_VICT);
		act( FG_YELLOW,
				"Pojedynek $n$1 z $N$4 zosta� zako�czony. $n jest zwyci�zc�.",
				ch, NULL, victim, TO_NOTVICT);
		return rNONE;
	}

	switch (victim->position)
	{
	case POS_MORTAL:
		act( COL_DYING,
				"$n jest �miertelnie rann$y i szybko umrze, je�li nikt nie udzieli $m pomocy.",
				victim, NULL, NULL, TO_ROOM);
		ch_printf(victim,
				FB_RED
				"Jeste� �miertelnie rann%s i szybko umrzesz, je�li nikt ci nie pomo�e." EOL,
				SEX_SUFFIX_YAE(victim));
		break;

	case POS_INCAP:
		act( COL_DYING, "$n jest nieprzytomn$y. Umrze bez pomocy.", victim,
				NULL, NULL, TO_ROOM);
		ch_printf(victim, FB_RED
		"Jeste� nieprzytomn%s. Umrzesz, je�li nikt ci nie pomo�e." EOL,
				SEX_SUFFIX_YAE(victim));
		break;

	case POS_STUNNED:
		if (!IS_AFFECTED(victim, AFF_PARALYSIS))
		{
			act( COL_ACTION,
					"$n jest og�uszon$y, ale prawdopodobnie z tego wyjdzie.",
					victim, NULL, NULL, TO_ROOM);
			ch_printf(victim, FB_RED
			"Jeste� og�uszon%s, ale chyba z tego wyjdziesz." EOL,
					SEX_SUFFIX_YAE(victim));
		}
		break;

	case POS_DEAD:
		if (dt >= 0 && dt < top_sn)
		{
			SKILLTYPE *skill = skill_table[dt];

			if (skill->die_char && skill->die_char[0] != '\0')
				act( COL_DEAD, skill->die_char, ch, NULL, victim, TO_CHAR);
			if (skill->die_vict && skill->die_vict[0] != '\0')
				act( COL_DEAD, skill->die_vict, ch, NULL, victim, TO_VICT);
			if (skill->die_room && skill->die_room[0] != '\0')
				act( COL_DEAD, skill->die_room, ch, NULL, victim, TO_NOTVICT);
		}
		if ( IS_NPC(victim) && IS_SET(victim->act, ACT_NOKILL))
			act( PLAIN, "$n ucieka w pop�ochu cudem unikaj�c �mierci!", victim,
					0, 0, TO_ROOM);
		else if ( IS_NPC(victim) && IS_SET(victim->act, ACT_DROID))
			act( COL_DEAD, "$n EXPLODUJE na ma�e kawa�eczki!", victim, 0, 0,
			TO_ROOM);
		else
			act( COL_DEAD, "$n UMIERA!!!", victim, 0, 0, TO_ROOM);
		ch_printf(victim, NL FB_WHITE "W�A�NIE ZGIN%s� !!!" EOL NL,
		MALE( victim ) ? "��E" : FEMALE(victim) ? "ʣA" : "ʣO");
		break;

	default:
		if (dam > victim->max_hit / 4)
		{
			act( COL_HURT, "Ough!!! To naprawd� BOLA�O !!!", victim, 0, 0,
			TO_CHAR);
			if (number_bits(3) == 0)
				worsen_mental_state(ch, 1);
		}

		if (victim->hit < victim->max_hit / 4)
		{
			act( COL_DANGER, "KRWAWISZ POTʯNIE !!!", victim, 0, 0, TO_CHAR);
			if (number_bits(2) == 0)
				worsen_mental_state(ch, 1);
		}
		break;
	}

	/*
	 * Sleep spells and extremely wounded folks.
	 */
	if (!IS_AWAKE(victim) /* lets make NPC's not slaughter PC's */
	&& !IS_AFFECTED(victim, AFF_PARALYSIS))
	{
		if (victim->fighting && victim->fighting->who->hunting
				&& victim->fighting->who->hunting->who == victim)
			stop_hunting(victim->fighting->who);

		if (victim->fighting && victim->fighting->who->hating
				&& victim->fighting->who->hating->who == victim)
			stop_hating(victim->fighting->who);

		stop_fighting(victim, true);
	}

	if (victim->hit <= 0 && !IS_NPC(victim))
	{
		OBJ_DATA *obj;

		REMOVE_BIT(victim->act, PLR_ATTACKER);

		if (victim->fighting && victim->fighting->who->hunting
				&& victim->fighting->who->hunting->who == victim)
			stop_hunting(victim->fighting->who);

		if (victim->fighting && victim->fighting->who->hating
				&& victim->fighting->who->hating->who == victim)
			stop_hating(victim->fighting->who);

		stop_fighting(victim, true);

		if ((obj = get_eq_char(victim, WEAR_DUAL_WIELD)) != NULL)
			unequip_char(victim, obj);
		if ((obj = get_eq_char(victim, WEAR_WIELD)) != NULL)
			unequip_char(victim, obj);
		if ((obj = get_eq_char(victim, WEAR_HOLD)) != NULL)
			unequip_char(victim, obj);
		if ((obj = get_eq_char(victim, WEAR_MISSILE_WIELD)) != NULL)
			unequip_char(victim, obj);
		if ((obj = get_eq_char(victim, WEAR_LIGHT)) != NULL)
			unequip_char(victim, obj);

		auto carrying_snapshot = victim->carrying;
		for (auto* obj : carrying_snapshot)
		{
			if (obj->wear_loc == WEAR_NONE)
			{
				if ((obj->pIndexData->progtypes & DROP_PROG) && obj->count > 1)
				{
					separate_obj(obj);
					obj_from_char(obj);
				}
				else
				{
					obj_from_char(obj);
				}
				act( COL_ACTION, "$n odrzuca $p$3.", victim, obj, NULL,
				TO_ROOM);
				act( COL_ACTION, "Odrzucasz $p$3.", victim, obj, NULL, TO_CHAR);
				obj = obj_to_room(obj, victim->in_room);
			}
		}

		if ( IS_NPC( ch ) && !IS_NPC(victim))
		{
			long lose_exp;
			lose_exp =
					UMAX(
							( victim->experience[COMBAT_ABILITY] - exp_level( victim->skill_level[COMBAT_ABILITY] ) ),
							0);
			ch_printf(victim, "Tracisz %ld expa." NL, lose_exp);
			victim->experience[COMBAT_ABILITY] -= lose_exp;
		}

		add_timer(victim, TIMER_RECENTFIGHT, 100, NULL, 0);
	}

	/*
	 * Payoff for killing things.
	 */
	if (victim->position == POS_DEAD)
	{
		group_gain(ch, victim);

		if (!npcvict)
		{
			sprintf(log_buf, "%s killed by %s at %d", victim->name,
					(IS_NPC(ch) ? ch->przypadki[0] : ch->name),
					victim->in_room->vnum);
			log_string(log_buf);
			to_channel(log_buf, CHANNEL_MONITOR, "Monitor", LEVEL_IMMORTAL);
		}
		else if (!IS_NPC(ch)) /* keep track of mob vnum killed */
			add_kill(ch, victim);

		check_killer(ch, victim);

		if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_NOKILL))
			loot = legal_loot(ch, victim);
		else
			loot = false;

		set_cur_char(victim);
		raw_kill(ch, victim, 0);
		victim = NULL;

		if (!IS_NPC(ch) && loot)
		{
			/* Autogold by Scryn 8/12 */
			if (IS_SET(ch->act, PLR_AUTOGOLD))
			{
				init_gold = ch->gold;
				do_get(ch, (char*) "kredytki cia�o");
				new_gold = ch->gold;
				gold_diff = (new_gold - init_gold);
				if (gold_diff > 0)
				{
					sprintf(buf1, "%d", gold_diff);
					do_split(ch, buf1);
				}
			}

			if (IS_SET(ch->act, PLR_AUTOLOOT))
				do_get(ch, (char*) "all cia�o");
			else
				do_look(ch, (char*) "in cia�o");
		}

		if (IS_SET(sysdata.save_flags, SV_KILL))
			save_char_obj(ch);

		return rVICT_DIED;
	}

	if (victim == ch)
		return rNONE;

	/*
	 * Take care of link dead people.
	 */
	if (!npcvict && !victim->desc && !victim->switched)
	{
		if (number_range(0, victim->wait) == 0)
		{
			for (int i = 0; i < 5; ++i)
			{
				do_flee(victim, (char*) "");
			}
			do_hail(victim, (char*) "");
			do_quit(victim, (char*) "");
			return rNONE;
		}
	}

	/*
	 * Wimp out?
	 */
	if (npcvict && dam > 0)
	{
		if (( IS_SET(victim->act, ACT_WIMPY) && number_bits(1) == 0
				&& victim->hit < victim->max_hit / 2)
				|| ( IS_AFFECTED(victim, AFF_CHARM) && victim->master
						&& victim->master->in_room != victim->in_room))
		{
			start_fearing(victim, ch);
			stop_hunting(victim);
			do_flee(victim, (char*) "");
		}
	}

	if (!npcvict && victim->hit > 0 && victim->hit <= victim->wimpy
			&& victim->wait == 0)
		do_flee(victim, (char*) "");
	else if (!npcvict && IS_SET(victim->act, PLR_FLEE))
		do_flee(victim, (char*) "");

	tail_chain();
	return rNONE;
}

bool is_safe(CHAR_DATA *ch, CHAR_DATA *victim)
{
	if (!victim)
		return false;

	/* Thx Josh! */
	if (who_fighting(ch) == ch)
		return false;

	if (IS_SET(victim->in_room->room_flags, ROOM_SAFE))
	{
		send_to_char("Nie mo�esz zrobi� tego tutaj." EOL, ch);
		return true;
	}

	if (!IS_NPC(victim) && victim->pcdata
			&& victim->pcdata->hotel_safe_time > 0)
	{
		send_to_char("Jeszcze nie mo�esz tego zrobi�." NL, ch);
		return true;
	}

	//Added by Thanos (moby questowe)
	if (!IS_NPC(ch))
	{
		if ( IS_NPC(victim) && victim->inquest
		&& victim->inquest != ch->inquest
		&& get_trust( ch ) < LEVEL_QUESTSEE) /* no tam jeden wyj�tek ;) */
		{
			send_to_char("Nie mo�esz tego zrobi�." EOL, ch);
			return true;
		}
	}

	if (get_trust(ch) > LEVEL_HERO)
		return false;

	if ( IS_NPC(ch) || IS_NPC(victim))
		return false;

	return false;

}

/* checks is_safe but without the output
 cuts out imms and safe rooms as well
 for info only */

bool is_safe_nm(CHAR_DATA *ch, CHAR_DATA *victim)
{
	return false;
}

/*
 * just verify that a corpse looting is legal
 */
bool legal_loot(CHAR_DATA *ch, CHAR_DATA *victim)
{
	/* pc's can now loot .. why not .. death is pretty final */
	if (!IS_NPC(ch))
		return true;
	/* non-charmed mobs can loot anything */
	if ( IS_NPC(ch) && !ch->master)
		return true;

	return false;
}

/*
 see if an attack justifies a KILLER flag --- edited so that none do but can't
 murder a no pk person. --- edited again for planetary wanted flags -- well will be soon :p
 */

void check_killer(CHAR_DATA *ch, CHAR_DATA *victim)
{
	/*
	 * Charm-o-rama.
	 */
	if (IS_SET(ch->affected_by, AFF_CHARM))
	{
		if (!ch->master)
		{
			bug("%s bad AFF_CHARM",
			IS_NPC(ch) ? ch->przypadki[0] : ch->name);
			affect_strip(ch, gsn_charm_person);
			REMOVE_BIT(ch->affected_by, AFF_CHARM);
			return;
		}

		/* stop_follower( ch ); */
		if (ch->master)
			check_killer(ch->master, victim);
	}

	if ( IS_NPC(victim) && !IS_NPC(ch))
	{
		/*   changed by Thanos	 */
		if (IS_VIP1(victim))
		{
				CRIME_DATA *crime;

			for (auto* planet : planet_list)
			{
				if (IS_VIP2(victim, planet))
				{
					if ((crime = find_crime(ch, planet)) != NULL)
						ch_printf(ch,
								FG_YELLOW
								"Pope�niaj�c przest�pstwa na %s pogarszasz swoj� sytuacj�." EOL,
								planet->name);
					else
						ch_printf(ch, FG_YELLOW
						"Jeste� teraz poszukiwan%s na %s." EOL,
								SEX_SUFFIX_YAE(ch), planet->name);

					crime_to_char(ch, planet->name, CRIME_MURDER);
				}
			}
		}
		/* done by Thanos */

		if (ch->pcdata->clan)
			ch->pcdata->clan->mkills++;
		ch->pcdata->mkills++;
		ch->in_room->area->mkills++;
		return;
	}

	if (!IS_NPC(ch) && !IS_NPC(victim))
	{
		if (ch->pcdata->clan)
			ch->pcdata->clan->pkills++;
		ch->pcdata->pkills++;

		update_pos(victim);

		if (victim->pcdata->clan)
			victim->pcdata->clan->pdeaths++;
	}

	if (IS_NPC(ch))
		if (!IS_NPC(victim))
			victim->in_room->area->mdeaths++;

	return;
}

/*
 * Set position of a victim.
 */
void update_pos(CHAR_DATA *victim)
{
	IF_BUG(victim == NULL, "")
		return;

	if (victim->hit > 0)
	{
		if (victim->position <= POS_STUNNED)
			victim->position = POS_STANDING;
		if (IS_AFFECTED(victim, AFF_PARALYSIS))
			victim->position = POS_STUNNED;
		return;
	}

	if ( IS_NPC(victim) || victim->hit <= -500)
	{
		if (victim->mount)
		{
			act( COL_ACTION, "$n spada z $N$1.", victim, NULL, victim->mount,
			TO_ROOM);
			REMOVE_BIT(victim->mount->act, ACT_MOUNTED);
			victim->mount = NULL;
		}

		victim->position = POS_DEAD;
		return;
	}

	if (victim->hit <= -400)
		victim->position = POS_MORTAL;
	else if (victim->hit <= -200)
		victim->position = POS_INCAP;
	else
		victim->position = POS_STUNNED;

	if (victim->position > POS_STUNNED && IS_AFFECTED(victim, AFF_PARALYSIS))
		victim->position = POS_STUNNED;

	if (victim->mount)
	{
		act( COL_ACTION, "$n spada bezw�adnie z $N$1.", victim, NULL,
				victim->mount, TO_ROOM);
		REMOVE_BIT(victim->mount->act, ACT_MOUNTED);
		victim->mount = NULL;
	}
	fevent_trigger(victim, FE_POSITION);
	return;
}

/*
 * Start fights.
 */
void set_fighting(CHAR_DATA *ch, CHAR_DATA *victim)
{
	FIGHT_DATA *fight;

	IF_BUG(ch->fighting, "%s -> %s (already fighting %s)", ch->name,
			victim->name, ch->fighting->who->name)
		return;

	if (IS_AFFECTED(ch, AFF_SLEEP))
		affect_strip(ch, gsn_sleep);

	/* Limit attackers -Thoric */
	if (victim->num_fighting > max_fight(victim))
	{
		send_to_char("Jatka jest tu zbyt du�a. Lepiej si� nie mieszaj." NL, ch);
		return;
	}

	CREATE(fight, FIGHT_DATA, 1);
	fight->who = victim;
	fight->xp = (int) xp_compute(ch, victim);
	fight->align = align_compute(ch, victim);

	if (!IS_NPC(ch) && IS_NPC(victim))
		fight->timeskilled = times_killed(ch, victim);

	ch->num_fighting = 1;
	ch->fighting = fight;
	ch->position = POS_FIGHTING;
	victim->num_fighting++;

	if (victim->switched && IS_AFFECTED(victim->switched, AFF_POSSESS))
	{
		send_to_char("Kto� ci przerwa�!" NL, victim->switched);
		do_return(victim->switched, (char*) "");
	}
	return;
}

CHAR_DATA* who_fighting(CHAR_DATA *ch)
{
	IF_BUG(ch == NULL, "")
		return NULL;

	if (!ch->fighting)
		return NULL;
	return ch->fighting->who;
}

void free_fight(CHAR_DATA *ch)
{
	IF_BUG(ch == NULL, "")
		return;

	if (ch->fighting)
	{
		if (!char_died(ch->fighting->who))
			--ch->fighting->who->num_fighting;
		DISPOSE(ch->fighting);
	}

	ch->fighting = NULL;
	if (ch->mount)
		ch->position = POS_MOUNTED;
	else
		ch->position = POS_STANDING;

	/* Berserk wears off after combat. -- Altrag */
	if (IS_AFFECTED(ch, AFF_BERSERK))
	{
		affect_strip(ch, gsn_berserk);
		send_to_char(COL_WEAROFF, ch);
		send_to_char(skill_table[gsn_berserk]->msg_off, ch);
		send_to_char( EOL, ch);
	}
	fevent_trigger(ch, FE_POSITION);
	return;
}

/*
 * Stop fights.
 */
void stop_fighting(CHAR_DATA *ch, bool fBoth)
{
	free_fight(ch);
	update_pos(ch);

	if (!fBoth) /* major short cut here by Thoric */
		return;

	for (auto* fch : char_list)
	{
		if (who_fighting(fch) == ch)
		{
			free_fight(fch);
			update_pos(fch);
		}
	}
	return;
}

void death_cry(CHAR_DATA *ch)
{
#if 0
    ROOM_INDEX_DATA *	was_in_room;
    EXIT_DATA *		pexit;
    char            *	msg;

    msg   = "S�yszysz �miertelny krzyk $n$1.";

    switch ( number_bits( 4 ) )
    {
    default: msg  = "S�yszysz �miertelny krzyk $n$1.";			break;
    case  0: msg  = "$n pada na ziemi� ... jest TRUPEM.";		break;
    case  1: msg  = "Krew $n$1 plami twoj� zbroj�.";			break;
    case  2: msg  = "Czujesz zapach �mierci wydobywaj�cy si� z cia�a $n$1.";break;
    case  3: msg = "Flaki $n$1 rozpryskuj� si� po ca�ej pod�odze.";	break;
    case  4: msg  = "Odci�ta g�owa $n$1 upada na ziemi�.";		break;
    case  5: msg  = "Serce $n$1 wydobywa si� z $s piersi.";		break;
    case  6: msg  = "R�ka $n$1 odci�ta od $s martwego cia�a upada na ziemi�.";
									break;
    case  7: msg  = "Noga $n$1 odci�ta od $s martwego cia�a upada na ziemi�.";
									break;
    case  8: msg = "Roztrzaskana g�owa $n$1 toczy si� po pod�odze.";	break;
    }

    act( FB_RED, msg, ch, NULL, NULL, TO_ROOM );

    if ( IS_NPC( ch ) )
	msg = "S�yszysz czyj� �miertelny krzyk.";
    else
	msg = "S�yszysz �miertelny krzyk jakiego� gracza.";
    was_in_room = ch->in_room;

    for ( auto* pexit : was_in_room->exits )
    {

	if ( pexit->to_room
	&&   pexit->to_room != was_in_room )
	{
	    ch->in_room = pexit->to_room;
	    act( PLAIN, msg, ch, NULL, NULL, TO_ROOM );
	}
    }
    ch->in_room = was_in_room;

#endif
	return;
}

void raw_kill(CHAR_DATA *ch, CHAR_DATA *victim, int suicide)
{
	char arg[MIL];
	bool clone;

	if (IS_NPC(victim))
	{
		fevent_trigger(ch, FE_KILL_MOB, victim->pIndexData->vnum);
		fevent_trigger(ch, FE_MKILLS);
	}
	else
	{
		fevent_trigger(ch, FE_KILL_PLAYER, victim->name);
		fevent_trigger(ch, FE_PKILLS);
	}

	IF_BUG(victim == NULL, "")
		return;

	strcpy(arg, victim->name);

	stop_fighting(victim, true);

	//added by Thanos
	send_to_char( COL_DIEMSG, victim);
	do_help(victim, (char*) "_DIEMSG_");
	send_to_char( PLAIN, victim);

	if (ch && !IS_NPC(ch) && !IS_NPC(victim))
	{
		//TODO: check for bounty completion here!
	}

	if (victim->in_room && IS_NPC(victim) && victim->s_vip_flags
			&& victim->s_vip_flags[0] != '\0' && victim->in_room->area
			&& victim->in_room->area->planet)
	{
		victim->in_room->area->planet->population--;
		victim->in_room->area->planet->population = UMAX(
				victim->in_room->area->planet->population, 0);
		victim->in_room->area->planet->pop_support -= (float) (1
				+ 1 / (victim->in_room->area->planet->population + 1));
		if (victim->in_room->area->planet->pop_support < -100)
			victim->in_room->area->planet->pop_support = -100;
	}

	if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_NOKILL))
		mprog_death_trigger(ch, victim);

	if (char_died(victim))
		return;

	if (!IS_NPC(victim) || !IS_SET(victim->act, ACT_NOKILL))
		rprog_death_trigger(ch, victim);

	if (char_died(victim))
		return;

	if (!IS_NPC(victim)
			|| (!IS_SET(victim->act, ACT_NOKILL)
					&& !IS_SET(victim->act, ACT_NOCORPSE)))
		make_corpse(victim, ch, suicide);
	else
	{
		while (!victim->carrying.empty())
		{
			auto* obj = victim->carrying.back();
			obj_from_char(obj);
			extract_obj(obj);
		}
	}

	if (IS_NPC(victim))
	{
		/*Thanos: questy*/
		quest_trigger(ch, EVENT_KILL_MOB, NULL, victim);

		victim->pIndexData->killed++;
		extract_char(victim, true);
		victim = NULL;
		return;
	}

	for (auto* clan : clan_list)
		remove_member(clan, victim->name);

	/********************** swreality changes begin here ********************/

	for (auto* ship : ship_list)
	{
		if (!str_cmp(ship->owner, victim->name))
		{
			STRDUP(ship->owner, "");
			STRDUP(ship->pilot, "");
			STRDUP(ship->copilot, "");

			save_ship(ship);
		}
	}

	// czy ofiara ma klona?
	clone = check_clone(victim);

	if (victim->plr_home && !clone)
	{
		STRDUP(victim->plr_home->homename, "");

		REMOVE_BIT(victim->plr_home->room_flags, ROOM_PLR_HOME);
		SET_BIT(victim->plr_home->room_flags, ROOM_EMPTY_HOME);
	}

	// jak kto� zabije gracza bez klona...
	if (!IS_NPC(ch) && !IS_NPC(victim) && !clone)
	{
		INFORM_DATA *inf;

		// ale tylko jesli walke zaczal ten drugi...
		if ((inf = get_inform(ch->name, victim->name)) != NULL)
		{
			// automatyczne zg�oszenie PKilla i flaga
			inf->reported = true;
			save_informs();
		}
	}

	if (!victim)
	{
		/* Make sure they aren't halfway logged in. */
		DESCRIPTOR_DATA *found_d = NULL;
		for (auto* d : descriptor_list)
		{
			if ((victim = d->character) && !IS_NPC(victim))
			{
				found_d = d;
				break;
			}
		}
		if (found_d)
			close_socket(found_d, true);
	}
	else
	{
		int x, y;

		quitting_char = victim;
		save_char_obj(victim);
		saving_char = NULL;
		extract_char(victim, true);
		for (x = 0; x < MAX_WEAR; x++)
			for (y = 0; y < MAX_LAYERS; y++)
				save_equipment[x][y] = NULL;
	}

	destroy_char(arg);
	return;
}

void group_gain(CHAR_DATA *ch, CHAR_DATA *victim)
{
	char buf[MAX_STRING_LENGTH];
	CHAR_DATA *lch;
	int xp;
	int members;

	/*
	 * Monsters don't get kill xp's or alignment changes.
	 * Dying of mortal wounds or poison doesn't give xp to anyone!
	 */
	if ( IS_NPC(ch) || victim == ch)
		return;

	members = 0;

	for (auto* gch : ch->in_room->people)
	{
		if (is_same_group(gch, ch))
			members++;
	}

	IF_BUG(members == 0, "")
		members = 1;

	lch = ch->leader ? ch->leader : ch;

	for (auto* gch : ch->in_room->people)
	{
		if (!is_same_group(gch, ch))
			continue;

		xp = (int) (xp_compute(gch, victim) / members);

		gch->alignment = align_compute(gch, victim);

		if (!IS_NPC(gch) && IS_NPC(victim) && gch->pcdata && gch->pcdata->clan
				&& !str_cmp(gch->pcdata->clan->name, victim->mob_clan))
		{
			xp = 0;
			sprintf(buf,
					"Nie otrzymujesz �adnego do�wiadczenia za zabicie przyjaci� swojej organizacji." NL);
			send_to_char(buf, gch);
		}
		else
		{
			sprintf(buf, "Zdobywasz %d punkt%s do�wiadczenia w walce." NL, xp,
					NUMBER_SUFF(xp, "", "y", "�w"));
			send_to_char(buf, gch);
		}

		gain_exp(gch, xp, COMBAT_ABILITY);

		if (lch == gch && members > 1)
		{
			xp =
					URANGE(members, xp * members,
							(exp_level( gch->skill_level[LEADERSHIP_ABILITY]+1) - exp_level(gch->skill_level[LEADERSHIP_ABILITY] )/30));
			sprintf(buf,
					"Zdobywasz %d punkt�w do�wiadczenia przyw�dczego za poprowadzenie grupy do zwyci�stwa." NL,
					xp);
			send_to_char(buf, gch);
			gain_exp(gch, xp, LEADERSHIP_ABILITY);
		}

		for (auto* obj : ch->carrying)
		{
			if (obj->wear_loc == WEAR_NONE)
				continue;

			if (( IS_OBJ_STAT(obj, ITEM_ANTI_EVIL) && IS_EVIL(ch))
					|| ( IS_OBJ_STAT(obj, ITEM_ANTI_GOOD) && IS_GOOD(ch))
					|| ( IS_OBJ_STAT(obj, ITEM_ANTI_NEUTRAL) && IS_NEUTRAL(ch)))
			{
				act( COL_FORCE, "Zosta�$a� oparzon$y przez $p$3.", ch, obj,
						NULL, TO_CHAR);
				act( COL_FORCE, "$n zosta�$o oparzon$y przez $p$3.", ch, obj,
						NULL, TO_ROOM);

				obj_from_char(obj);
				obj = obj_to_room(obj, ch->in_room);
				oprog_zap_trigger(ch, obj); /* mudprogs */
				if (char_died(ch))
					return;
			}
		}
	}

	return;
}

int align_compute(CHAR_DATA *gch, CHAR_DATA *victim)
{

	/* never cared much for this system

	 int align, newalign;

	 align = gch->alignment - victim->alignment;

	 if ( align >  500 )
	 newalign  = UMIN( gch->alignment + (align-500)/4,  1000 );
	 else
	 if ( align < -500 )
	 newalign  = UMAX( gch->alignment + (align+500)/4, -1000 );
	 else
	 newalign  = gch->alignment - (int) (gch->alignment / 4);

	 return newalign;

	 make it simple instead */

	return URANGE(-1000, (int ) (gch->alignment - victim->alignment / 5), 1000);

}

/*
 * Calculate how much XP gch should gain for killing victim
 * Lots of redesigning for new exp system by Thoric
 */
int xp_compute(CHAR_DATA *gch, CHAR_DATA *victim)
{
	int align;
	int xp, exp;

	xp =
			(get_exp_worth(victim)
					* URANGE(1,
							(victim->skill_level[COMBAT_ABILITY] - gch->skill_level[COMBAT_ABILITY]) + 10,
							20)) / 10;
	align = gch->alignment - victim->alignment;

	/* bonus for attacking opposite alignment */
	if (align > 990 || align < -990)
		xp = (xp * 5) >> 2;
	else
	/* penalty for good attacking same alignment */
	if (gch->alignment > 300 && align < 250)
		xp = xp >> 2;

	xp = number_range((xp * 3) >> 2, (xp * 5) >> 2);

	/* reduce exp for killing the same mob repeatedly		-Thoric */
	if (!IS_NPC(gch) && IS_NPC(victim))
	{
		int times = times_killed(gch, victim);

		if (times >= 5)
			xp = 0;
		else if (times)
			xp = (xp * (5 - times)) / 3;
	}

	/* new xp cap for swreality */

	exp =
			URANGE(1, xp,
					( exp_level( gch->skill_level[COMBAT_ABILITY]+1 ) - exp_level( gch->skill_level[COMBAT_ABILITY]) ));
#if defined (ARMAGEDDON)
    return exp/5;
#else
	return exp / 2;
#endif
}

void dam_message(CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt)
{
	char vp[MSL];
	char vs[MSL];
	char buf1[MSL];
	char buf2[MSL];
	char buf3[MSL];
	const char *attack;
	const char *v;
	char punct;
	int dampc;
	struct skill_type *skill = NULL;
	bool gcflag = false;
	bool gvflag = false;

	dampc = dam * 100;

	if (victim->hit > 0)
		dampc /= victim->hit;

	if (dam < 0)
	{
		v = "leczy%s";
	}
	else if (dam == 0)
	{
		v = "chybia%s";
	}
	else if (dampc <= 1)
	{
		v = "muska%s";
	}
//    else if ( dampc <=    2 )  { v = "ledwie drapie%s";		}
	else if (dampc <= 2)
	{
		v = "drapie%s";
	}
	else if (dampc <= 4)
	{
		v = "trafia%s";
	}
	else if (dampc <= 8)
	{
		v = "lekko rani%s";
	}
	else if (dampc <= 15)
	{
		v = "kaleczy%s";
	}
	else if (dampc <= 30)
	{
		v = "rani%s";
	}
	else if (dampc <= 50)
	{
		v = "dotkliwie rani%s";
	}
	else if (dampc <= 70)
	{
		v = "powa�nie rani%s";
	}
	else if (dampc <= 80)
	{
		v = "�miertelnie rani%s";
	}
	else
	{
		v = "ZABIJA%s";
	}

	sprintf(vs, v, dampc <= 80 ? "sz" : "SZ");
	sprintf(vp, v, "");

	punct = (dampc <= 30) ? '.' : '!';

	if (dam == 0 && (!IS_NPC(ch) && (IS_SET(ch->pcdata->flags, PCFLAG_GAG))))
	{
		gcflag = true;
	}

	if (dam == 0
			&& (!IS_NPC(victim) && (IS_SET(victim->pcdata->flags, PCFLAG_GAG))))
	{
		gvflag = true;
	}

	if (dt >= 0 && dt < top_sn)
	{
		skill = skill_table[dt];
	}

	if (dt == (TYPE_HIT + WEAPON_BLASTER))
	{
		char sound[MAX_STRING_LENGTH];
		int vol = number_range(20, 80);

		sprintf(sound, "!!SOUND(blaster V=%d)", vol);
		sound_to_room(ch->in_room, sound);
	}

	if (dt == TYPE_HIT)
	{
		swsnprintf(buf1, MSL, "$n %s $N$3%c", vp, punct);
		swsnprintf(buf2, MSL, "%s $N$3%c", vs, punct);
		swsnprintf(buf3, MSL, "$n %s ci�%c", vp, punct);
	}
	else       //Added by Onyx
	if (dam == 0)
	{
		int chance;
		chance = number_range(1, 14);
		switch (chance)
		{
		default:
			sprintf(buf1, "$n chybia $N$3.");
			sprintf(buf2, "Chybiasz $N$3.");
#if !defined (ARMAGEDDON)
			sprintf(buf3, "$n chybia ci�.");
#endif
			break;
		case 2:
			sprintf(buf1, "$n chybia $N$3.");
			sprintf(buf2, "Co za niefart! Chybiasz $N$3.");
#if !defined (ARMAGEDDON)
			sprintf(buf3, "Co za niefart! $n chybia ci�.");
#endif
			break;
		case 3:
			sprintf(buf1,
					"$n o ma�o si� nie przewr�ci�$o pr�buj�c trafi� $N$3.");
			sprintf(buf2, "O ma�o si� nie przewr�ci�$a� pr�buj�c trafi� $N$3.");
			sprintf(buf3, "$n prawie si� przewr�ci�$o pr�buj�c ci� trafi�.");
			break;
		case 4:
			sprintf(buf1, "Cios $n$1 o ma�y w�os mija g�ow� $N$1.");
			sprintf(buf2, "Ach! O ma�y w�os chybiasz $N$3.");
			sprintf(buf3, "Ach! Ma�o brakowa�o by $n ci� trafi�$o.");
			break;
		case 5:
			sprintf(buf1, "Pot�ny cios $n$1 chybia $N$3.");
			sprintf(buf2, "Tw�j pot�ny cios chybia $N$3.");
			sprintf(buf3, "Uff... Mia�$a� szcz�cie, �e $n ci� nie trafi�$o.");
			break;
		case 6:
			sprintf(buf1, "$n potyka si� i nie trafia $N$1.");
			sprintf(buf2, "Potykasz si� i nie trafiasz $N$3.");
			sprintf(buf3, "Jaka ulga... $n nie trafia ci� potykaj�c si�.");
			break;
		case 7:
			sprintf(buf1, "$n nie trafia $N$3.");
			sprintf(buf2, "Nie trafi�$a� $N$3.");
			sprintf(buf3, "$n nie trafia ci�.");
			break;
		}
	}       //done by Onyx
	else if (dt > TYPE_HIT && is_wielding_poisoned(ch))
	{
		if (dt
				< (int) (TYPE_HIT
						+ sizeof(attack_table) / sizeof(attack_table[0])))
			attack = attack_table[dt - TYPE_HIT];
		else
		{
			bug("bad dt %d (%s - %s).", dt, ch ? ch->przypadki[0] : "Null Ch",
					victim ? victim->przypadki[0] : "Null Victim");

			dt = TYPE_HIT;
			attack = attack_table[0];
		}

		swsnprintf(buf1, MSL, "Zatrute %s $n$1 %s $N$3%c", attack, vp, punct);
		swsnprintf(buf2, MSL, "Twoje zatrute %s %s $N$3%c", attack, vp, punct);
		swsnprintf(buf3, MSL, "Zatrute %s $n$1 %s ci�%c", attack, vp, punct);
	}
	else
	{
		if (skill)
		{
			attack = skill->noun_damage;
			if (dam == 0)
			{
				bool found = false;

				if (skill->miss_char && skill->miss_char[0] != '\0')
				{
					act( COL_HIT, skill->miss_char, ch, NULL, victim, TO_CHAR);
					found = true;
				}
				if (skill->miss_vict && skill->miss_vict[0] != '\0')
				{
					act( COL_HITME, skill->miss_vict, ch, NULL, victim,
					TO_VICT);
					found = true;
				}
				if (skill->miss_room && skill->miss_room[0] != '\0')
				{
					act( COL_ACTION, skill->miss_room, ch, NULL, victim,
					TO_NOTVICT);
					found = true;
				}
				if (found) /* miss message already sent */
					return;
			}
			else
			{
				if (skill->hit_char && skill->hit_char[0] != '\0')
					act( COL_HIT, skill->hit_char, ch, NULL, victim, TO_CHAR);
				if (skill->hit_vict && skill->hit_vict[0] != '\0')
					act( COL_HITME, skill->hit_vict, ch, NULL, victim, TO_VICT);
				if (skill->hit_room && skill->hit_room[0] != '\0')
					act( COL_ACTION, skill->hit_room, ch, NULL, victim,
					TO_NOTVICT);
			}
		}
		else if (dt >= TYPE_HIT
				&& dt
						< (int) (TYPE_HIT
								+ sizeof(attack_table) / sizeof(attack_table[0])))
			attack = attack_table[dt - TYPE_HIT];
		else
		{
			bug("bad dt %d (%s - %s).", dt, ch ? ch->przypadki[0] : "Null Ch",
					victim ? victim->przypadki[0] : "Null Victim");
			dt = TYPE_HIT;
			attack = attack_table[0];
		}

		if (dt > TYPE_HIT)
		{
			if (ch == victim)
			{
				swsnprintf(buf1, MSL, "Lamerskie %s $n$1 %s $i%c", attack, vp, punct);
				swsnprintf(buf2, MSL, "Twoje lamerskie %s %s Ciebie%c", attack, vp,
						punct);
			}
			else
			{
				swsnprintf(buf1, MSL, "%s $n$1 %s $N$3%c", attack, vp, punct);
				swsnprintf(buf2, MSL, "Twoje %s %s $N$3%c", attack, vp, punct);
				swsnprintf(buf3, MSL, "%s $n$1 %s Ciebie%c", attack, vp, punct);
			}
		}
		else
		{
			if (ch == victim)
			{
				swsnprintf(buf1, MSL, "$n %s sam%s siebie %s%c", vp, SEX_SUFFIX__AO(ch),
						attack, punct);
				swsnprintf(buf2, MSL, "%s sam%s siebie %s%c", vs, SEX_SUFFIX__AO(ch),
						attack, punct);
			}
			else
			{
				swsnprintf(buf1, MSL, "$n %s $N$3 %s%c", vp, attack, punct);
				swsnprintf(buf2, MSL, "%s $N$3 %s%c", vs, attack, punct);
				swsnprintf(buf3, MSL, "$n %s ci� %s%c", vp, attack, punct);
			}
		}
	}

	if (!IS_NPC(ch) && IS_SET(ch->act, PLR_HOLYLIGHT))
	{
		char buf4[MSL] =
		{ 0 };
		strncpy(buf4, buf2, MSL);
		swsnprintf(buf2, MSL, "%s Zadajesz cios o sile %d punkt�w.", buf4, dam);
	}

	act( COL_ACTION, buf1, ch, NULL, victim, TO_NOTVICT);
	if (!gcflag && *buf2)
		act( COL_ACTION, buf2, ch, NULL, victim, TO_CHAR);
	if (!gvflag && *buf3 && ch != victim)
		act( COL_ACTION, buf3, ch, NULL, victim, TO_VICT);

	return;
}

DEF_DO_FUN( kill )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Zabi� kogo?" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	if (!IS_NPC(victim))
	{
		send_to_char("Gracza trzeba ZAMORDOWA�." NL, ch);
		return;
	}
	// Przywracam to, bo to teraz wazne ze wzgledu na ewentualnego
	// informa -- Thanos
	else
	{
		if ( IS_AFFECTED(victim, AFF_CHARM) && victim->master != NULL)
		{
			send_to_char("Ubezw�asnowolnionego moba trzeba ZAMORDOWA�." NL, ch);
			return;
		}
	}

	if (victim == ch)
	{
		send_to_char("Kopiesz si� w plecy. Au�!" NL, ch);
		multi_hit(ch, ch, TYPE_UNDEFINED);
		return;
	}

	if (is_safe(ch, victim))
		return;

	if ( IS_AFFECTED(ch, AFF_CHARM) && ch->master == victim)
	{
		act( PLAIN, "Przecie� $N to tw�j uwielbiany mistrz.", ch, NULL, victim,
		TO_CHAR);
		return;
	}

	if (ch->position == POS_FIGHTING)
	{
		send_to_char("Starasz si� jak mo�esz!" NL, ch);
		return;
	}

	if (victim->s_vip_flags && victim->s_vip_flags[0] != '\0')
		ch->alignment -= 10;

	WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
	multi_hit(ch, victim, TYPE_UNDEFINED);
	return;
}

DEF_DO_FUN( murde )
{
	send_to_char("Je�li chcesz ZAMORDOWA�, napisz to w ca�o�ci." NL, ch);
	return;
}

DEF_DO_FUN( murder )
{
	char arg[MAX_INPUT_LENGTH];
	CHAR_DATA *victim;

	one_argument(argument, arg);

	if (arg[0] == '\0')
	{
		send_to_char("Zamordowa� kogo?" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	if (victim == ch)
	{
		send_to_char("Samob�jstwo?" NL, ch);
		return;
	}

	if (is_safe(ch, victim))
		return;

	if (IS_AFFECTED(ch, AFF_CHARM))
	{
		if (ch->master == victim)
		{
			act( PLAIN, "Przecie� $N to tw�j uwielbiany mistrz.", ch, NULL,
					victim, TO_CHAR);
			return;
		}
	}

	if (ch->position == POS_FIGHTING)
	{
		send_to_char("Starasz si� jak mo�esz!" NL, ch);
		return;
	}

	if (!IS_NPC(victim) && IS_SET(ch->act, PLR_NICE))
	{
		send_to_char("Zbyt dobrze si� czujesz by robi� takie rzeczy!" NL, ch);
		return;
	}

	ch->alignment -= 10;

	WAIT_STATE(ch, 1 * PULSE_VIOLENCE);
	multi_hit(ch, victim, TYPE_UNDEFINED);
	return;
}

/* Trog: funkcja fajna - dzialanie debilne.
 *       Ci ze smauga dziwne pomysly maja.
 */
bool in_arena(CHAR_DATA *ch)
{

	if (!str_cmp(ch->in_room->area->filename, "arena.are"))
		return true;

	if (ch->in_room->vnum < 29 || ch->in_room->vnum > 43)
		return false;

	return true;
}

DEF_DO_FUN( flee )
{
	ROOM_INDEX_DATA *was_in;
	ROOM_INDEX_DATA *now_in;
	char buf[MAX_STRING_LENGTH];
	int attempt;
	int door;
	EXIT_DATA *pexit;
#if defined (ARMAGEDDON)
    int	los;
#endif
	if (!who_fighting(ch))
	{
		if (ch->position == POS_FIGHTING)
		{
			if (ch->mount)
				ch->position = POS_MOUNTED;
			else
				ch->position = POS_STANDING;
		}
		send_to_char("Nie walczysz z nikim." NL, ch);
		return;
	}

	if (ch->move <= 0)
	{
		send_to_char("Brak ci si� by uciec!" NL, ch);
		return;
	}

	/* No fleeing while stunned. - Narn */
	if (ch->position < POS_FIGHTING)
		return;

	WAIT_STATE(ch, 4);

	was_in = ch->in_room;
	for (attempt = 0; attempt < 8; attempt++)
	{

		door = number_door();
		if ((pexit = get_exit(was_in, door)) == NULL || !pexit->to_room
				|| ( IS_SET(pexit->flags, EX_CLOSED)
						&& !IS_AFFECTED(ch, AFF_PASS_DOOR))
				|| ( IS_NPC(ch)
						&& IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)))
			continue;

		affect_strip(ch, gsn_sneak);
		REMOVE_BIT(ch->affected_by, AFF_SNEAK);
		if (ch->mount && ch->mount->fighting)
			stop_fighting(ch->mount, true);
		move_char(ch, pexit, 0);
		if ((now_in = ch->in_room) == was_in)
			continue;

		ch->in_room = was_in;
		act( COL_ACTION, "$n ucieka w pop�ochu!", ch, NULL, NULL, TO_ROOM);
		ch->in_room = now_in;
		act( COL_ACTION, "$n rozgl�da si� szukaj�c kryj�wki.", ch, NULL, NULL,
		TO_ROOM);
		sprintf(buf, "Uciekasz w pop�ochu!" NL);
		send_to_char(buf, ch);
#if defined (ARMAGEDDON)
	if( ch->main_ability == COMBAT_ABILITY )
	los = (	exp_level( ch->skill_level[COMBAT_ABILITY] + 1)
	    - 	exp_level( ch->skill_level[COMBAT_ABILITY]) ) * 0.01;
	gain_exp( ch, 0 - los, COMBAT_ABILITY );
	ch_printf( ch, "Tracisz nieco punkt�w do�wiadczenia w walce." NL );
#endif

		stop_fighting(ch, true);
		return;
	}

	sprintf(buf, "Starasz si� uciec, ale nic z tego!" NL);
	send_to_char(buf, ch);
	fevent_trigger(ch, FE_POSITION);
	return;
}

bool get_cover(CHAR_DATA *ch)
{
	ROOM_INDEX_DATA *was_in;
	ROOM_INDEX_DATA *now_in;
	int attempt;
	int door;
	EXIT_DATA *pexit;

	if (!who_fighting(ch))
		return false;

	if (ch->position < POS_FIGHTING)
		return false;

	was_in = ch->in_room;
	for (attempt = 0; attempt < 10; attempt++)
	{

		door = number_door();
		if ((pexit = get_exit(was_in, door)) == NULL || !pexit->to_room
				|| ( IS_SET(pexit->flags, EX_CLOSED)
						&& !IS_AFFECTED(ch, AFF_PASS_DOOR))
				|| ( IS_NPC(ch)
						&& IS_SET(pexit->to_room->room_flags, ROOM_NO_MOB)))
			continue;

		affect_strip(ch, gsn_sneak);
		REMOVE_BIT(ch->affected_by, AFF_SNEAK);
		if (ch->mount && ch->mount->fighting)
			stop_fighting(ch->mount, true);
		move_char(ch, pexit, 0);
		if ((now_in = ch->in_room) == was_in)
			continue;

		ch->in_room = was_in;
		act( PLAIN, "$n ucieka w pop�ochu!", ch, NULL, NULL, TO_ROOM);
		ch->in_room = now_in;
		act( PLAIN, "$n rozgl�da si� szukaj�c kryj�wki.", ch, NULL, NULL,
		TO_ROOM);

		stop_fighting(ch, true);

		return true;
	}

	return false;
}

DEF_DO_FUN( sla )
{
	send_to_char("If you want to SLAY, spell it out." NL, ch);
	return;
}

DEF_DO_FUN( slay )
{
	CHAR_DATA *victim;
	ROOM_INDEX_DATA *room;
	char arg[MAX_INPUT_LENGTH];
	char arg2[MAX_INPUT_LENGTH];

	argument = one_argument(argument, arg);
	one_argument(argument, arg2);
	if (arg[0] == '\0')
	{
		send_to_char("Kogo zaSLAYowa�??" NL, ch);
		return;
	}

	if ((victim = get_char_room(ch, arg)) == NULL)
	{
		send_to_char("Nie ma tu nikogo takiego." NL, ch);
		return;
	}

	if (ch == victim)
	{
		send_to_char("Samob�jstwo to grzech �miertelnik�w." NL, ch);
		return;
	}

	if (!IS_NPC(victim) && (get_trust(victim) == 103 || get_trust(ch) < 103))
	{
		send_to_char("Nie uda�o Ci si�." NL, ch);
		return;
	}

	if (!str_cmp(arg2, "immolate"))
	{
		act( FG_YELLOW,
				"Tworzysz niewyobra�alnie du�� kul� ognia i ciskasz ni� w $N$3!",
				ch, NULL, victim, TO_CHAR);
		act( FG_YELLOW, "$n ciska w ciebie kul� ognia!", ch, NULL, victim,
		TO_VICT);
		act( FG_YELLOW, "$n ciska w stron� $N$3 kul� ognia i spala go �ywcem.",
				ch, NULL, victim, TO_NOTVICT);
	}

	else if (!str_cmp(arg2, "shatter"))
	{
		act( FB_CYAN,
				"Zamra�asz wzrokiem $N$3 po czym jednym szybkim ciosem rozpierdalasz go na kawa�eczki!",
				ch, NULL, victim, TO_CHAR);
		act( FB_CYAN,
				"$n zamra�a Ci� wzorkiem, po czym jednym szybkim ciosem rozpierdala Ci� na kawa�eczki!",
				ch, NULL, victim, TO_VICT);
		act( FB_CYAN,
				"$n zamra�a wzrokiem $N$3 po czym jednym szybkim ciosem rozpierdala go na kawa�eczki!!",
				ch, NULL, victim, TO_NOTVICT);
	}

	else if (!str_cmp(arg2, "demon"))
	{
		act( FB_WHITE,
				"You gesture, and a slavering demon appears.  With a horrible grin, the",
				ch, NULL, victim, TO_CHAR);
		act( FB_WHITE,
				"foul creature turns on $N, who screams in panic before being eaten alive.",
				ch, NULL, victim, TO_CHAR);
		act( FB_WHITE,
				"$n gestures, and a slavering demon appears.  The foul creature turns on",
				ch, NULL, victim, TO_VICT);
		act( FB_WHITE,
				"you with a horrible grin.   You scream in panic before being eaten alive.",
				ch, NULL, victim, TO_VICT);
		act( FB_WHITE,
				"$n gestures, and a slavering demon appears.  With a horrible grin, the",
				ch, NULL, victim, TO_NOTVICT);
		act( FB_WHITE,
				"foul creature turns on $N, who screams in panic before being eaten alive.",
				ch, NULL, victim, TO_NOTVICT);
	}

	else if (!str_cmp(arg2, "pounce") && get_trust(ch) >= LEVEL_ASCENDANT)
	{
		act( FG_RED,
				"Leaping upon $N with bared fangs, you tear open $S throat and toss the corpse to the ground...",
				ch, NULL, victim, TO_CHAR);
		act( FG_RED,
				"In a heartbeat, $n rips $s fangs through your throat!  Your blood sprays and pours to the ground as your life ends...",
				ch, NULL, victim, TO_VICT);
		act( FG_RED,
				"Leaping suddenly, $n sinks $s fangs into $N's throat.  As blood sprays and gushes to the ground, $n tosses $N's dying body away.",
				ch, NULL, victim, TO_NOTVICT);
	}

	else if (!str_cmp(arg2, "slit") && get_trust(ch) >= LEVEL_ASCENDANT)
	{
		act( FG_RED, "You calmly slit $N's throat.", ch, NULL, victim, TO_CHAR);
		act( FG_RED,
				"$n reaches out with a clawed finger and calmly slits your throat.",
				ch, NULL, victim, TO_VICT);
		act( FG_RED, "$n calmly slits $N's throat.", ch, NULL, victim,
		TO_NOTVICT);
	}

	else if (!str_cmp(arg2, "decapit"))
	{
		act( FG_RED,
				"Z przyjemno�ci� ogl�dasz, jak r�ce $N$3 odrywaj� si� od jego cia�a i �ami� $N$2 kark jak zapa�k�.",
				ch, NULL, victim, TO_CHAR);
		act( FG_RED,
				"Twoje w�asne r�ce odrywaj� si� od cia�a i �ami� Ci kark jak zapa�k�. Silny by�e�...",
				ch, NULL, victim, TO_VICT);
		act( FG_RED,
				"Od cia�a $N$3 nagle odrywaj� si� r�ce a nast�pnie �ami� $N$2 kark jak zapa�k�...",
				ch, NULL, victim, TO_NOTVICT);
	}

	else
	{
		act( FB_WHITE, "Za�ynasz $N$3 z zimn� krwi�!", ch, NULL, victim,
		TO_CHAR);
		act( FB_WHITE, "$n za�yna ci� z zimn� krwi�!", ch, NULL, victim,
		TO_VICT);
		act( FB_WHITE,
				"$n b�yskawicznie dobywa swego miecza swietlnego i rozcina $N$3 na dwie cz�sci!",
				ch, NULL, victim, TO_NOTVICT);
	}

	/* Thanos - autoclone przed smiercia */
	if (!IS_NPC(victim))
	{
		room = victim->in_room;
		char_from_room(victim);
		char_to_room(victim, cloning_list.front()->cylinder);
		save_clone(victim);
		char_from_room(victim);
		char_to_room(victim, room);
	}

	set_cur_char(victim);
	raw_kill(ch, victim, 0);
	return;
}

/* Thanos */
int fight_lev(CHAR_DATA *ch)
{
	int lev;

	/* Level = 60%COMBAT + 40%MAIN_ABILITY */
	lev = (ch->skill_level[COMBAT_ABILITY] * 60
			+ ch->skill_level[ch->main_ability] * 40) / 100;

	return UMAX(1, lev);
}
