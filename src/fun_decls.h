/*
 * fun_decls.h
 *
 *  Created on: 2009-04-04
 *      Author: Trog
 */

#ifndef FUN_DECLS_H_
#define FUN_DECLS_H_
#include <stdio.h>
#include "typedefs.h"
#include "defines.h"
#include "classes/SWBounty.h"
#include "classes/SWPazaakCard.h"

/*
* Command functions.
* Defined in act_*.c (mostly).
*/
DECLARE_DO_FUN( do_setplanet			);
DECLARE_DO_FUN( do_makeplanet			);
DECLARE_DO_FUN( do_planets				);
DECLARE_DO_FUN( do_teach				);
DECLARE_DO_FUN( do_gather_intelligence	);
DECLARE_DO_FUN( do_add_patrol			);
DECLARE_DO_FUN( do_special_forces		);
/*DECLARE_DO_FUN( do_jail				);*/
//DECLARE_DO_FUN( do_findship			); /*by Dremor*/
DECLARE_DO_FUN( do_elite_guard			);
DECLARE_DO_FUN( do_smalltalk			);
DECLARE_DO_FUN( do_propeganda			);
DECLARE_DO_FUN( do_bargain				);
DECLARE_DO_FUN( do_bribe				);
DECLARE_DO_FUN( do_seduce				);
DECLARE_DO_FUN( do_mass_propeganda		);
DECLARE_DO_FUN( do_copyover				);
DECLARE_DO_FUN( do_copyship				);
DECLARE_DO_FUN( do_copystats			);// Pixel
DECLARE_DO_FUN( do_sound				);
DECLARE_DO_FUN( do_camuflage			);/*by Aldegard*/
DECLARE_DO_FUN( do_interdict			);/*by Aldegard*/
DECLARE_DO_FUN( do_load_cargo			);
DECLARE_DO_FUN( do_unload_cargo			);
DECLARE_DO_FUN( do_astro				);/*by Aldegard*/
DECLARE_DO_FUN( do_addengineer			);/*by Aldegard*/
DECLARE_DO_FUN( do_remengineer			);/*by Aldegard*/
DECLARE_DO_FUN( do_dock					);/*by Aldegard*/
DECLARE_DO_FUN( do_undock				);/*by Aldegard*/
DECLARE_DO_FUN( do_imports				);
DECLARE_DO_FUN( do_autopilot			);
DECLARE_DO_FUN( do_allspeeders			);
DECLARE_DO_FUN( do_speeders				);
DECLARE_DO_FUN( do_suicide				);
DECLARE_DO_FUN( do_gain					);
DECLARE_DO_FUN( do_train				);
DECLARE_DO_FUN( do_beg					);
DECLARE_DO_FUN( do_bail					);	/* Thanos */
DECLARE_DO_FUN( do_bank					);
DECLARE_DO_FUN( do_hijack				);
DECLARE_DO_FUN( do_hackshiplock			);
DECLARE_DO_FUN( do_pickshiplock			);
DECLARE_DO_FUN( do_shiptalk				);
DECLARE_DO_FUN( do_clone				);
DECLARE_DO_FUN( do_systemtalk			);
DECLARE_DO_FUN( do_spacetalk			);
DECLARE_DO_FUN( do_hail					);
DECLARE_DO_FUN( do_allships				);
DECLARE_DO_FUN( do_appoint				);
DECLARE_DO_FUN( do_demote				);
DECLARE_DO_FUN( do_empower				);
DECLARE_DO_FUN( do_capture				);
DECLARE_DO_FUN( do_arm					);
DECLARE_DO_FUN( do_chaff				);
DECLARE_DO_FUN( do_clan_donate			);
DECLARE_DO_FUN( do_clan_withdraw		);
DECLARE_DO_FUN( do_fly					);
DECLARE_DO_FUN( do_drive				);
DECLARE_DO_FUN( do_setblaster			);
DECLARE_DO_FUN( do_ammo					);
DECLARE_DO_FUN( do_takedrug				);
DECLARE_DO_FUN( do_use					);
DECLARE_DO_FUN( do_resign				);
DECLARE_DO_FUN( do_pluogus				);
DECLARE_DO_FUN( do_tractorbeam			);
DECLARE_DO_FUN( do_makearmor			);
DECLARE_DO_FUN( do_makejewelry			);
DECLARE_DO_FUN( do_makegrenade			);
DECLARE_DO_FUN( do_makelandmine			);
DECLARE_DO_FUN( do_makelight			);
DECLARE_DO_FUN( do_makecomlink			);
DECLARE_DO_FUN( do_makeshield			);
DECLARE_DO_FUN( do_makecontainer		);
DECLARE_DO_FUN( do_makemissile			);
DECLARE_DO_FUN( do_gemcutting			);
DECLARE_DO_FUN( do_reinforcements		);
DECLARE_DO_FUN( do_postguard			);
DECLARE_DO_FUN( do_torture				);
DECLARE_DO_FUN( do_snipe				);
DECLARE_DO_FUN( do_throw				);
DECLARE_DO_FUN( do_disguise				);
DECLARE_DO_FUN( do_mine					);
DECLARE_DO_FUN( do_first_aid			);
DECLARE_DO_FUN( do_makeblade			);
DECLARE_DO_FUN( do_makeblaster			);
DECLARE_DO_FUN( do_makehackdev			);//Trog
DECLARE_DO_FUN( do_makelightsaber		);
DECLARE_DO_FUN( do_makespice			);
DECLARE_DO_FUN( do_closebay				);
DECLARE_DO_FUN( do_openbay				);
DECLARE_DO_FUN( do_autotrack			);
DECLARE_DO_FUN( do_tractorbeam			);
DECLARE_DO_FUN( do_radar				);
DECLARE_DO_FUN( do_buyship				);
DECLARE_DO_FUN( do_buyhome				);
DECLARE_DO_FUN( do_clanbuyship			);
DECLARE_DO_FUN( do_sellship				);
DECLARE_DO_FUN( do_autorecharge			);
DECLARE_DO_FUN( do_openhatch			);
DECLARE_DO_FUN( do_closehatch			);
DECLARE_DO_FUN( do_openshipdock			);
DECLARE_DO_FUN( do_closeshipdock		);
DECLARE_DO_FUN( do_status				);
DECLARE_DO_FUN( do_info					);
DECLARE_DO_FUN( do_hyperspace			);
DECLARE_DO_FUN( do_target				);
DECLARE_DO_FUN( do_fire					);
DECLARE_DO_FUN( do_calculate			);
DECLARE_DO_FUN( do_recharge				);
DECLARE_DO_FUN( do_crew					);
DECLARE_DO_FUN( do_crack_transponder	);
DECLARE_DO_FUN( do_transponders			);
DECLARE_DO_FUN( do_uninstal_module		);
DECLARE_DO_FUN( do_modules				);
DECLARE_DO_FUN( do_repairship			);
DECLARE_DO_FUN( do_refuel				);
DECLARE_DO_FUN( do_distres				);//Aldegard -> energia w kosmosie
DECLARE_DO_FUN( do_addpilot				);
DECLARE_DO_FUN( do_rempilot				);
DECLARE_DO_FUN( do_trajectory			);
DECLARE_DO_FUN( do_accelerate			);
DECLARE_DO_FUN( do_launch				);
DECLARE_DO_FUN( do_land					);
DECLARE_DO_FUN( do_leaveship			);
DECLARE_DO_FUN( do_setstarsystem		);
DECLARE_DO_FUN( do_makestarsystem		);
DECLARE_DO_FUN( do_starsystems			);
DECLARE_DO_FUN( do_showstarsystem		);
DECLARE_DO_FUN( skill_notfound			);
DECLARE_DO_FUN( do_addbounty			);
DECLARE_DO_FUN( do_advance				);
DECLARE_DO_FUN( do_affected				);
DECLARE_DO_FUN( do_afk					);
DECLARE_DO_FUN( do_aid					);
DECLARE_DO_FUN( do_allow				);
DECLARE_DO_FUN( do_ansi					);
DECLARE_DO_FUN( do_answer				);
DECLARE_DO_FUN( do_apply				);
DECLARE_DO_FUN( do_appraise				);
DECLARE_DO_FUN( do_areas				);
DECLARE_DO_FUN( do_ask					);
DECLARE_DO_FUN( do_astat				);
DECLARE_DO_FUN( do_alist				);
DECLARE_DO_FUN( do_at					);
DECLARE_DO_FUN( do_authorize			);
DECLARE_DO_FUN( do_backstab				);
DECLARE_DO_FUN( do_balzhur				);
DECLARE_DO_FUN( do_bamfin				);
DECLARE_DO_FUN( do_bamfout				);
DECLARE_DO_FUN( do_ban					);
DECLARE_DO_FUN( do_bash					);
DECLARE_DO_FUN( do_bashdoor				);
DECLARE_DO_FUN( do_beep					);
DECLARE_DO_FUN( do_berserk				);
DECLARE_DO_FUN( do_bestow				);
DECLARE_DO_FUN( do_bio					);
DECLARE_DO_FUN( do_bite					);
DECLARE_DO_FUN( do_board				);
DECLARE_DO_FUN( do_boards				);
DECLARE_DO_FUN( do_bodybag				);
DECLARE_DO_FUN( do_bounties				);
DECLARE_DO_FUN( do_allbounties			);
DECLARE_DO_FUN( do_bset					);
DECLARE_DO_FUN( do_bstat				);
DECLARE_DO_FUN( do_bug					);
DECLARE_DO_FUN( do_bury					);
DECLARE_DO_FUN( do_buy					);
DECLARE_DO_FUN( do_cast					);
DECLARE_DO_FUN( do_channels				);
DECLARE_DO_FUN( do_chat					);
DECLARE_DO_FUN( do_flame				);
DECLARE_DO_FUN( do_ooc					);
DECLARE_DO_FUN( do_check_vnums			);
DECLARE_DO_FUN( do_circle				);
DECLARE_DO_FUN( do_clan					);//byTrog
DECLARE_DO_FUN( do_clans				);
DECLARE_DO_FUN( do_claninfo				); //by Trog
DECLARE_DO_FUN( do_relations			); //by Trog
DECLARE_DO_FUN( do_crank				); //by Trog
DECLARE_DO_FUN( do_clear_modules		); //Qrwa ma� !!!
DECLARE_DO_FUN( do_ships				);
DECLARE_DO_FUN( do_add_ship_title		);
DECLARE_DO_FUN( do_clantalk				);
DECLARE_DO_FUN( do_claw					);
DECLARE_DO_FUN( do_climb				);
DECLARE_DO_FUN( do_close				);
DECLARE_DO_FUN( do_cmdtable				);
DECLARE_DO_FUN( do_commands				);
DECLARE_DO_FUN( do_compare				);
DECLARE_DO_FUN( do_config				);
DECLARE_DO_FUN( do_connect				);//Trog
DECLARE_DO_FUN( do_consider				);
DECLARE_DO_FUN( do_corrupt				);//Ganis
DECLARE_DO_FUN( do_credits				);
DECLARE_DO_FUN( do_cset					);
DECLARE_DO_FUN( do_deities				);
DECLARE_DO_FUN( do_delbounty			);
DECLARE_DO_FUN( do_deny					);
DECLARE_DO_FUN( do_description			);
DECLARE_DO_FUN( do_destro				);
DECLARE_DO_FUN( do_destroy				);
DECLARE_DO_FUN( do_detrap				);
DECLARE_DO_FUN( do_devote				);
DECLARE_DO_FUN( do_dig					);
DECLARE_DO_FUN( do_disarm				);
DECLARE_DO_FUN( do_disconnect			);
DECLARE_DO_FUN( do_dismount				);
DECLARE_DO_FUN( do_dominate				);//byTrog
DECLARE_DO_FUN( do_down					);
DECLARE_DO_FUN( do_drag					);
DECLARE_DO_FUN( do_drink				);
DECLARE_DO_FUN( do_drop					);
DECLARE_DO_FUN( do_diagnose				);
DECLARE_DO_FUN( do_dump					);//Thanos
DECLARE_DO_FUN( do_east					);
DECLARE_DO_FUN( do_eat					);
DECLARE_DO_FUN( do_echo					);
DECLARE_DO_FUN( do_echoat				);//by Trog
DECLARE_DO_FUN( do_editinfo				);//Thanos
DECLARE_DO_FUN( do_emote				);
DECLARE_DO_FUN( do_empty				);
DECLARE_DO_FUN( do_enter				);
DECLARE_DO_FUN( do_equipment			);
DECLARE_DO_FUN( do_examine				);
DECLARE_DO_FUN( do_exits				);
DECLARE_DO_FUN( do_feed					);
DECLARE_DO_FUN( do_fill					);
DECLARE_DO_FUN( do_findresets			);	/* Ganis */
DECLARE_DO_FUN( do_fix					);	/* Thanos */
DECLARE_DO_FUN( do_fixchar				);
DECLARE_DO_FUN( do_flee					);
DECLARE_DO_FUN( do_foldarea				);
DECLARE_DO_FUN( do_follow				);
DECLARE_DO_FUN( do_for					);
DECLARE_DO_FUN( do_force				);
DECLARE_DO_FUN( do_forceclose			);
DECLARE_DO_FUN( do_fquit				);     /* Gorog */
DECLARE_DO_FUN( do_freeze				);
DECLARE_DO_FUN( do_fshow				);     /* Thanos */
DECLARE_DO_FUN( do_get					);
DECLARE_DO_FUN( do_give					);
DECLARE_DO_FUN( do_glance				);
DECLARE_DO_FUN( do_gold					);
DECLARE_DO_FUN( do_goto					);
DECLARE_DO_FUN( do_gouge				);
DECLARE_DO_FUN( do_group				);
DECLARE_DO_FUN( do_gtell				);
DECLARE_DO_FUN( do_guilds				);
DECLARE_DO_FUN( do_guildtalk			);
//DECLARE_DO_FUN( do_hedit				);
DECLARE_DO_FUN( do_isolate				);
DECLARE_DO_FUN( do_help					);
DECLARE_DO_FUN( do_hide					);
DECLARE_DO_FUN( do_hitall				);
DECLARE_DO_FUN( do_hlist				);
DECLARE_DO_FUN( do_holylight			);
DECLARE_DO_FUN( do_homepage				);
DECLARE_DO_FUN( do_hstat				);
DECLARE_DO_FUN( do_ide					);
DECLARE_DO_FUN( do_idea					);
DECLARE_DO_FUN( do_immadvance			);
DECLARE_DO_FUN( do_immtalk				);
DECLARE_DO_FUN( do_admintalk			); /* Trog */
DECLARE_DO_FUN( do_codertalk			); /* Trog */
DECLARE_DO_FUN( do_ignore				);//Thanos
DECLARE_DO_FUN( do_induct				);
DECLARE_DO_FUN( do_inform				); //Thanos
DECLARE_DO_FUN( do_inventory			);
DECLARE_DO_FUN( do_invis				);
DECLARE_DO_FUN( do_kick					);
DECLARE_DO_FUN( do_kill					);
DECLARE_DO_FUN( do_languages			);
DECLARE_DO_FUN( do_last					);//changed by Thanos: do_last zmienione
DECLARE_DO_FUN( do_whowas				);//na do_whowas i dodane nowe do_last :)
DECLARE_DO_FUN( do_leave				);
DECLARE_DO_FUN( do_experience			);
DECLARE_DO_FUN( do_light				);
DECLARE_DO_FUN( do_list					);
DECLARE_DO_FUN( do_litterbug			);
DECLARE_DO_FUN( do_loadup				);
DECLARE_DO_FUN( do_lock					);
DECLARE_DO_FUN( do_log					);
DECLARE_DO_FUN( do_look					);
DECLARE_DO_FUN( do_mailroom				);
DECLARE_DO_FUN( do_make					);
DECLARE_DO_FUN( do_makeboard			);
DECLARE_DO_FUN( do_makeclan				);
DECLARE_DO_FUN( do_makeship				);
DECLARE_DO_FUN( do_makeguild			);
//DECLARE_DO_FUN( do_makewizlist			);
DECLARE_DO_FUN( do_memory				);
DECLARE_DO_FUN( do_mdelete				);
DECLARE_DO_FUN( do_mfind				);
DECLARE_DO_FUN( do_minvoke				);
DECLARE_DO_FUN( do_mlist				);
DECLARE_DO_FUN( do_mont					);/* Thanos: test */
DECLARE_DO_FUN( do_mount				);
DECLARE_DO_FUN( do_mset					);
DECLARE_DO_FUN( do_mstat				);
DECLARE_DO_FUN( do_murde				);
DECLARE_DO_FUN( do_murder				);
DECLARE_DO_FUN( do_music				);
DECLARE_DO_FUN( do_mwhere				);
DECLARE_DO_FUN( do_newbiechat			);
DECLARE_DO_FUN( do_newbieset			);
DECLARE_DO_FUN( do_noemote				);
DECLARE_DO_FUN( do_noresolve			);
DECLARE_DO_FUN( do_north				);
DECLARE_DO_FUN( do_northeast			);
DECLARE_DO_FUN( do_northwest			);
DECLARE_DO_FUN( do_notell				);
DECLARE_DO_FUN( do_notitle				);
DECLARE_DO_FUN( do_noteroom				);
DECLARE_DO_FUN( do_odelete				);
DECLARE_DO_FUN( do_ofind				);
DECLARE_DO_FUN( do_oinvoke				);
DECLARE_DO_FUN( do_oldscore				);
DECLARE_DO_FUN( do_olist				);
DECLARE_DO_FUN( do_open					);
DECLARE_DO_FUN( do_opentourney			);
DECLARE_DO_FUN( do_olctalk				);
DECLARE_DO_FUN( do_order				);
DECLARE_DO_FUN( do_orders				);
DECLARE_DO_FUN( do_ordertalk			);
DECLARE_DO_FUN( do_oset					);
DECLARE_DO_FUN( do_ostat				);
DECLARE_DO_FUN( do_ot					);
DECLARE_DO_FUN( do_outcast				);
DECLARE_DO_FUN( do_owhere				);
DECLARE_DO_FUN( do_pager				);
DECLARE_DO_FUN( do_pardon				);
DECLARE_DO_FUN( do_password				);
DECLARE_DO_FUN( do_pazaak				);
DECLARE_DO_FUN( do_peace				);
DECLARE_DO_FUN( do_pick					);
DECLARE_DO_FUN( do_poison_weapon		);
DECLARE_DO_FUN( do_pose					);
DECLARE_DO_FUN( do_practice				);
DECLARE_DO_FUN( do_prompt				);
DECLARE_DO_FUN( do_pull					);
DECLARE_DO_FUN( do_punch				);
DECLARE_DO_FUN( do_purge				);
DECLARE_DO_FUN( do_push					);
DECLARE_DO_FUN( do_put					);
DECLARE_DO_FUN( do_qpset				);
DECLARE_DO_FUN( do_quaff				);
DECLARE_DO_FUN( do_quest				);
DECLARE_DO_FUN( do_qstat				);//Thanos
DECLARE_DO_FUN( do_qui					);
DECLARE_DO_FUN( do_quit					);
DECLARE_DO_FUN( do_rank					);
DECLARE_DO_FUN( do_rat					);
DECLARE_DO_FUN( do_rdelete				);
DECLARE_DO_FUN( do_reboo				);
DECLARE_DO_FUN( do_reboot				);
DECLARE_DO_FUN( do_recall				);
DECLARE_DO_FUN( do_recho				);
DECLARE_DO_FUN( do_regoto				);
DECLARE_DO_FUN( do_remove				);
DECLARE_DO_FUN( do_rename				);
DECLARE_DO_FUN( do_rent					);
DECLARE_DO_FUN( do_repair				);
DECLARE_DO_FUN( do_repairshops			);
DECLARE_DO_FUN( do_reply				);
DECLARE_DO_FUN( do_retell				);//added by Thanos
DECLARE_DO_FUN( do_report				);
DECLARE_DO_FUN( do_rescue				);
DECLARE_DO_FUN( do_rest					);
DECLARE_DO_FUN( do_reset				);
DECLARE_DO_FUN( do_resetship			);
DECLARE_DO_FUN( do_restore				);
DECLARE_DO_FUN( do_restoretime			);
DECLARE_DO_FUN( do_restrict				);
DECLARE_DO_FUN( do_retire				);
DECLARE_DO_FUN( do_retran				);
DECLARE_DO_FUN( do_return				);
DECLARE_DO_FUN( do_revert				);
DECLARE_DO_FUN( do_rlist				);
DECLARE_DO_FUN( do_rset					);
DECLARE_DO_FUN( do_rstat				);
DECLARE_DO_FUN( do_sacrifice			);
DECLARE_DO_FUN( do_save					);
DECLARE_DO_FUN( do_say					);
DECLARE_DO_FUN( do_scan					);
DECLARE_DO_FUN( do_clonescore			);
DECLARE_DO_FUN( do_score				);
DECLARE_DO_FUN( do_scribe				);
DECLARE_DO_FUN( do_search				);
DECLARE_DO_FUN( do_sedit				);
DECLARE_DO_FUN( do_sell					);
DECLARE_DO_FUN( do_set_boot_time		);
DECLARE_DO_FUN( do_set_password			);//Trog
DECLARE_DO_FUN( do_setclan				);
DECLARE_DO_FUN( do_saveall				);
DECLARE_DO_FUN( do_setship				);
DECLARE_DO_FUN( do_describe_ship		);//byTrog 4 Arian
DECLARE_DO_FUN( do_setcourse			);//added by Thanos
DECLARE_DO_FUN( do_shgoto				);//added by Thanos
DECLARE_DO_FUN( do_shlist				);//added by Thanos
DECLARE_DO_FUN( do_shload				);//added by Thanos
DECLARE_DO_FUN( do_shops				);
DECLARE_DO_FUN( do_shout				);
DECLARE_DO_FUN( do_shove				);
DECLARE_DO_FUN( do_showclan				);
DECLARE_DO_FUN( do_showship				);
DECLARE_DO_FUN( do_showplanet			);
DECLARE_DO_FUN( do_shsave				);//added by Thanos
DECLARE_DO_FUN( do_shstat				);//added by Thanos
DECLARE_DO_FUN( do_shutdow				);
DECLARE_DO_FUN( do_shutdown				);
DECLARE_DO_FUN( do_silence				);
DECLARE_DO_FUN( do_sit					);
DECLARE_DO_FUN( do_sla					);
DECLARE_DO_FUN( do_slay					);
DECLARE_DO_FUN( do_sleep				);
DECLARE_DO_FUN( do_slice				);
DECLARE_DO_FUN( do_slist				);
DECLARE_DO_FUN( do_smoke				);
DECLARE_DO_FUN( do_sneak				);
DECLARE_DO_FUN( do_snoop				);
DECLARE_DO_FUN( do_sober				);
DECLARE_DO_FUN( do_socials				);
DECLARE_DO_FUN( do_south				);
DECLARE_DO_FUN( do_southeast			);
DECLARE_DO_FUN( do_southwest			);
DECLARE_DO_FUN( do_speak				);
DECLARE_DO_FUN( do_split				);
DECLARE_DO_FUN( do_sset					);
DECLARE_DO_FUN( do_stand				);
DECLARE_DO_FUN( do_starttourney			);
DECLARE_DO_FUN( do_start_speak			);
DECLARE_DO_FUN( do_steal				);
DECLARE_DO_FUN( do_sting				);
DECLARE_DO_FUN( do_stun					);
DECLARE_DO_FUN( do_supplicate			);
DECLARE_DO_FUN( do_switch				);
DECLARE_DO_FUN( do_tail					);
DECLARE_DO_FUN( do_tamp					);
DECLARE_DO_FUN( do_tell					);
DECLARE_DO_FUN( do_time					);
DECLARE_DO_FUN( do_timecmd				);
DECLARE_DO_FUN( do_title				);
//DECLARE_DO_FUN( do_thanosscore				); Pixel: moze kiedys
DECLARE_DO_FUN( do_track				);
DECLARE_DO_FUN( do_transfer				);
DECLARE_DO_FUN( do_transship			);	//Thanos
DECLARE_DO_FUN( do_trust				);
DECLARE_DO_FUN( do_typo					);
DECLARE_DO_FUN( do_unfoldarea			);
DECLARE_DO_FUN( do_unisolate			);
DECLARE_DO_FUN( do_unlock				);
DECLARE_DO_FUN( do_unsilence			);
DECLARE_DO_FUN( do_up					);
DECLARE_DO_FUN( do_users				);
DECLARE_DO_FUN( do_value				);
DECLARE_DO_FUN( do_visible				);
DECLARE_DO_FUN( do_vsearch				);
DECLARE_DO_FUN( do_wake					);
DECLARE_DO_FUN( do_wartalk				);
DECLARE_DO_FUN( do_wear					);
DECLARE_DO_FUN( do_weather				);
DECLARE_DO_FUN( do_west					);
DECLARE_DO_FUN( do_where				);
DECLARE_DO_FUN( do_who					);
DECLARE_DO_FUN( do_whois				);
DECLARE_DO_FUN( do_wimpy				);
DECLARE_DO_FUN( do_wizlist				);
DECLARE_DO_FUN( do_wizlock				);
DECLARE_DO_FUN( do_yell					);
DECLARE_DO_FUN( do_prostat				);
DECLARE_DO_FUN( do_eject				);
DECLARE_DO_FUN( do_invite				);/*Ganis*/
DECLARE_DO_FUN( do_uninvite				);/*Ganis*/
DECLARE_DO_FUN( do_invites				);/*Ganis*/
DECLARE_DO_FUN( do_cls					);/* Trog */


//system znajomosci
DECLARE_DO_FUN( do_remember				);	// Tanglor
DECLARE_DO_FUN( do_introduce			);
DECLARE_DO_FUN( do_changeattribute		);
DECLARE_DO_FUN( do_kin					);
DECLARE_DO_FUN( do_exchange				);	//Tanglor
DECLARE_DO_FUN( do_forget				);  // Pixel

//added by Thanos
DECLARE_DO_FUN( do_stbar				);
DECLARE_DO_FUN( do_wizinfo				);
DECLARE_DO_FUN( do_iso					);
DECLARE_DO_FUN( do_win					);
DECLARE_DO_FUN( do_nopol				);
DECLARE_DO_FUN( do_forbid				);
DECLARE_DO_FUN( do_alia					);
DECLARE_DO_FUN( do_unalia				);
DECLARE_DO_FUN( do_alias				);//tu nie by�o alias�w
DECLARE_DO_FUN( do_unalias				);//ani unalias�w ;)
DECLARE_DO_FUN( do_wanted				);
DECLARE_DO_FUN( do_identify				);//engineer.c
DECLARE_DO_FUN( do_detonate				);//engineer.c
DECLARE_DO_FUN( do_questchat			);
DECLARE_DO_FUN( do_holonet				);//engineer.c
DECLARE_DO_FUN( do_garota				);
DECLARE_DO_FUN( do_prefi				);
DECLARE_DO_FUN( do_prefix				);
//done

/* mob prog stuff */
DECLARE_DO_FUN( do_mp_close_passage		);
DECLARE_DO_FUN( do_mp_damage			);
DECLARE_DO_FUN( do_mp_restore			);
DECLARE_DO_FUN( do_mp_open_passage		);
DECLARE_DO_FUN( do_mp_practice			);
DECLARE_DO_FUN( do_mp_slay				);
DECLARE_DO_FUN( do_mpaddtolist			);
DECLARE_DO_FUN( do_mpremfromlist		);
DECLARE_DO_FUN( do_mpaddtowanted		); /* Thanos */
DECLARE_DO_FUN( do_mpremfromwanted		); /* Thanos */
DECLARE_DO_FUN( do_mpclrnameslist		);
DECLARE_DO_FUN( do_mpadvance			);
DECLARE_DO_FUN( do_mpasound				);
DECLARE_DO_FUN( do_mpat					);
DECLARE_DO_FUN( do_mpdelay				); /*Thanos*/
DECLARE_DO_FUN( do_mpdream				);
DECLARE_DO_FUN( do_mp_deposit			);
DECLARE_DO_FUN( do_mp_withdraw			);
DECLARE_DO_FUN( do_mpecho				);
DECLARE_DO_FUN( do_mpechoaround			);
DECLARE_DO_FUN( do_mpechoat				);
DECLARE_DO_FUN( do_mpforce				);
DECLARE_DO_FUN( do_mpinduct				); //byTrog
DECLARE_DO_FUN( do_mpoutcast			); //byTrog
DECLARE_DO_FUN( do_mpinvis				);
DECLARE_DO_FUN( do_mpgoto				);
DECLARE_DO_FUN( do_mpjunk				);
DECLARE_DO_FUN( do_mpkill				);
DECLARE_DO_FUN( do_mpmload				);
DECLARE_DO_FUN( do_mpmset				);
DECLARE_DO_FUN( do_mpnoecho				);// Thanos
DECLARE_DO_FUN( do_mpnothing			);
DECLARE_DO_FUN( do_mpoload				);
DECLARE_DO_FUN( do_mposet				);
DECLARE_DO_FUN( do_mppurge				);
DECLARE_DO_FUN( do_mpstat				);
DECLARE_DO_FUN( do_mpsuggest			);
DECLARE_DO_FUN( do_mpresetroom			);// Thanos
DECLARE_DO_FUN( do_mpstun				);// Thanos
DECLARE_DO_FUN( do_opstat				);
DECLARE_DO_FUN( do_rpstat				);
DECLARE_DO_FUN( do_mptransfer			);
DECLARE_DO_FUN( do_mptire				);
DECLARE_DO_FUN( do_mpapply				);
DECLARE_DO_FUN( do_mpapplyb				);
DECLARE_DO_FUN( do_mpgain				);
DECLARE_DO_FUN( do_mpaddfrc				);
DECLARE_DO_FUN( do_mpqmload				);// Thanos
DECLARE_DO_FUN( do_mpqoload				);// Thanos
DECLARE_DO_FUN( do_mpauth				);// Trog

/*
*Thanos:
*OLC z Ciapka
*/
DECLARE_DO_FUN( do_olcinfo				);
DECLARE_DO_FUN( do_pedit				);
DECLARE_DO_FUN( do_aedit				);
DECLARE_DO_FUN( do_qedit				);
DECLARE_DO_FUN( do_medit				);
DECLARE_DO_FUN( do_oedit				);
DECLARE_DO_FUN( do_redit				);
DECLARE_DO_FUN( do_mpedit				);
DECLARE_DO_FUN( do_opedit				);
DECLARE_DO_FUN( do_rpedit				);
DECLARE_DO_FUN( do_hedit				);
DECLARE_DO_FUN( do_shedit				);
DECLARE_DO_FUN( do_skedit				);
DECLARE_DO_FUN( do_cedit				);
DECLARE_DO_FUN( do_proedit				);
DECLARE_DO_FUN( do_ssedit				);
DECLARE_DO_FUN( do_raedit				);
DECLARE_DO_FUN( do_matedit				);
DECLARE_DO_FUN( do_matlist				);
DECLARE_DO_FUN( do_dialedit				);
DECLARE_DO_FUN( do_diallist				);
DECLARE_DO_FUN( do_build_hq				);
DECLARE_DO_FUN( do_tcedit				);


/*done*/


/*
* Spell functions.
* Defined in force.c.
*/
DECLARE_SPELL_FUN(	spell_null			);
DECLARE_SPELL_FUN(	spell_notfound		);
DECLARE_SPELL_FUN(	spell_animate_dead	);
DECLARE_SPELL_FUN(	spell_blindness		);
DECLARE_SPELL_FUN(	spell_burning_hands	);
DECLARE_SPELL_FUN(	spell_cause_critical);
DECLARE_SPELL_FUN(	spell_cause_light	);
DECLARE_SPELL_FUN(	spell_cause_serious	);
DECLARE_SPELL_FUN(	spell_charm_person	);
DECLARE_SPELL_FUN(	spell_control_weather	);
DECLARE_SPELL_FUN(	spell_cure_blindness	);
DECLARE_SPELL_FUN(	spell_cure_poison	);
DECLARE_SPELL_FUN(	spell_curse			);
DECLARE_SPELL_FUN(	spell_detect_poison	);
DECLARE_SPELL_FUN(	spell_dispel_evil	);
DECLARE_SPELL_FUN(	spell_dispel_force	);
DECLARE_SPELL_FUN(	spell_dream			);
DECLARE_SPELL_FUN(	spell_earthquake	);
DECLARE_SPELL_FUN(	spell_enchant_weapon	);
DECLARE_SPELL_FUN(	spell_energy_drain	);
DECLARE_SPELL_FUN(	spell_faerie_fire	);
DECLARE_SPELL_FUN(	spell_faerie_fog	);
DECLARE_SPELL_FUN(	spell_farsight		);
DECLARE_SPELL_FUN(	spell_knock			);
DECLARE_SPELL_FUN(	spell_harm			);
DECLARE_SPELL_FUN(	spell_identify		);
DECLARE_SPELL_FUN(	spell_invis			);
DECLARE_SPELL_FUN(	spell_know_alignment);
DECLARE_SPELL_FUN(	spell_lightning_bolt);
DECLARE_SPELL_FUN(	spell_poison		);
DECLARE_SPELL_FUN(	spell_possess		);
DECLARE_SPELL_FUN(	spell_remove_curse	);
DECLARE_SPELL_FUN(	spell_remove_invis	);
DECLARE_SPELL_FUN(	spell_remove_trap	);
DECLARE_SPELL_FUN(	spell_shocking_grasp);
DECLARE_SPELL_FUN(	spell_sleep			);
DECLARE_SPELL_FUN(	spell_smaug			);
DECLARE_SPELL_FUN(	spell_ventriloquate	);
DECLARE_SPELL_FUN(	spell_weaken		);
DECLARE_SPELL_FUN(	spell_acid_breath	);
DECLARE_SPELL_FUN(	spell_fire_breath	);
DECLARE_SPELL_FUN(	spell_frost_breath	);
DECLARE_SPELL_FUN(	spell_gas_breath	);
DECLARE_SPELL_FUN(	spell_lightning_breath	);
DECLARE_SPELL_FUN(	spell_scorching_surge	);

DECLARE_SPELL_FUN(	spell_ethereal_fist	);
DECLARE_SPELL_FUN(	spell_hand_of_darkness	);
DECLARE_SPELL_FUN(	spell_disruption	);
DECLARE_SPELL_FUN(	spell_sonic_resonance	);
DECLARE_SPELL_FUN(	spell_mind_wrack	);
DECLARE_SPELL_FUN(	spell_mind_wrench	);
DECLARE_SPELL_FUN(	spell_revive		);
DECLARE_SPELL_FUN(	spell_sulfurous_spray	);
DECLARE_SPELL_FUN(	spell_caustic_fount	);
DECLARE_SPELL_FUN(	spell_acetum_primus	);
DECLARE_SPELL_FUN(	spell_galvanic_whip	);
DECLARE_SPELL_FUN(	spell_magnetic_thrust	);
DECLARE_SPELL_FUN(	spell_quantum_spike	);
DECLARE_SPELL_FUN(	spell_black_lightning	);
DECLARE_SPELL_FUN(	spell_midas_touch	);

DECLARE_SPELL_FUN(	spell_cure_addiction);


DECLARE_CON_FUN(	con_notfound		);
DECLARE_CON_FUN(	parse_command		);
// olc
DECLARE_CON_FUN( pedit					);
DECLARE_CON_FUN( aedit					);
DECLARE_CON_FUN( qedit					);
DECLARE_CON_FUN( medit					);
DECLARE_CON_FUN( oedit					);
DECLARE_CON_FUN( redit					);
DECLARE_CON_FUN( mpedit					);
DECLARE_CON_FUN( opedit					);
DECLARE_CON_FUN( rpedit					);
DECLARE_CON_FUN( hedit					);
DECLARE_CON_FUN( skedit					);
DECLARE_CON_FUN( shedit					);
DECLARE_CON_FUN( cedit					);
DECLARE_CON_FUN( proedit				);
DECLARE_CON_FUN( ssedit					);
DECLARE_CON_FUN( raedit					);
DECLARE_CON_FUN( matedit				);
DECLARE_CON_FUN( dialedit				);
DECLARE_CON_FUN( exchange				);
DECLARE_CON_FUN( mob_speak				);
DECLARE_CON_FUN( build_hq				);
DECLARE_CON_FUN( tcedit					);


DECLARE_CON_FUN( holonet				);

DECLARE_CON_FUN( process_rename			);

//nanny
DECLARE_CON_FUN( nanny_new_pass_to_change);
DECLARE_CON_FUN( nanny_confirm_new_pass_to_change);
DECLARE_CON_FUN( nanny_confirm_suicide	);
DECLARE_CON_FUN( nanny_get_name		);
DECLARE_CON_FUN( nanny_get_old_password );
DECLARE_CON_FUN( nanny_confirm_new_name );
DECLARE_CON_FUN( nanny_get_new_colour	);
DECLARE_CON_FUN( nanny_get_char_set	);
DECLARE_CON_FUN( nanny_check_char_set	);
DECLARE_CON_FUN( nanny_get_msp		);
DECLARE_CON_FUN( nanny_wprowadzenie	);
DECLARE_CON_FUN( nanny_old_pass_to_change);
DECLARE_CON_FUN( nanny_get_new_password );
DECLARE_CON_FUN( nanny_confirm_new_password);
DECLARE_CON_FUN( nanny_get_new_sex	);
DECLARE_CON_FUN( nanny_get_new_race	);
DECLARE_CON_FUN( nanny_confirm_new_race );
DECLARE_CON_FUN( nanny_get_new_class	);
DECLARE_CON_FUN( nanny_confirm_class	);
DECLARE_CON_FUN( nanny_get_dopelniacz	);
DECLARE_CON_FUN( nanny_get_celownik	);
DECLARE_CON_FUN( nanny_get_biernik	);
DECLARE_CON_FUN( nanny_get_narzednik	);
DECLARE_CON_FUN( nanny_get_miejscownik	);
DECLARE_CON_FUN( nanny_check_odmiane	);
DECLARE_CON_FUN( nanny_stats_ok		);
DECLARE_CON_FUN( nanny_read_motd	);

DECLARE_CON_FUN( nanny_show_look_trait_group );
DECLARE_CON_FUN( nanny_show_look_trait );
DECLARE_CON_FUN( nanny_confirm_choice );
DECLARE_CON_FUN( nanny_final_confirm  );

/* engineer.c -- Skill inzyniera	    Thanos */
void	save_projects		args( ( void ) );

/* Trog: rasy */
void	save_race			args( ( RACE_DATA *pRace ) );
void	save_races_list		args( ( void ) );

/* Trog: turbocar'y */
void	save_turbocar			args( ( TURBOCAR *pTurbocar ) );
void	save_turbocars_list		args( ( void ) );
/*
* OS-dependent declarations.
* These are all very standard library functions,
*   but some systems have incomplete or non-ansi header files.
*/
#if	defined(_AIX)
char *	crypt				args( ( const char *key, const char *salt ) );
#endif

/*
#if	defined(apollo)
void *	calloc				args( ( unsigned nelem, size_t size ) );
char *	crypt				args( ( const char *key, const char *salt ) );
#endif
*/

#if	defined(hpux)
char *	crypt				args( ( const char *key, const char *salt ) );
#endif

#if	defined(interactive)
#endif

#if	defined(linux)
#ifdef __cplusplus
extern "C" {
#endif
//char *	crypt				args( ( const char *key, const char *salt ) );
#ifdef __cplusplus
}
#endif
#endif

#if	defined(MIPS_OS)
char *	crypt				args( ( const char *key, const char *salt ) );
#endif

#if	defined(NeXT)
char *	crypt				args( ( const char *key, const char *salt ) );
#endif

#if	defined(sequent)
char *	crypt				args( ( const char *key, const char *salt ) );
int		fclose				args( ( FILE *stream ) );
int		fprintf				args( ( FILE *stream, const char *format, ... ) );
int		fread				args( ( void *ptr, int size, int n, FILE *stream ) );
int		fseek				args( ( FILE *stream, long offset, int ptrname ) );
void	perror				args( ( const char *s ) );
int		ungetc				args( ( int c, FILE *stream ) );
#endif

#if	defined(sun)
char *	crypt				args( ( const char *key, const char *salt ) );
int		fclose				args( ( FILE *stream ) );
int		fprintf				args( ( FILE *stream, const char *format, ... ) );
/*
#if	defined(SYSV)
size_t	fread				args( ( void *ptr, size_t size, size_t n,
							FILE *stream ) );
#else
int	fread		args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
*/
int		fseek				args( ( FILE *stream, long offset, int ptrname ) );
void	perror				args( ( const char *s ) );
int		ungetc				args( ( int c, FILE *stream ) );
#endif

#if	defined(ultrix)
char *	crypt				args( ( const char *key, const char *salt ) );
#endif

/*
* The crypt(3) function is not available on some operating systems.
* In particular, the U.S. Government prohibits its export from the
*   United States to foreign countries.
* Turn on NOCRYPT to keep passwords in plain text.
*/
#if defined( WIN32 )
	#define NOCRYPT
#endif

#if	defined(NOCRYPT)
	#define crypt(s1, s2)	(s1)
#endif

// nie wiem czy to dobra deklaracja, ale przynajmniej kompilator sie nie
// czepia, ze implict ;)	-- Thanos
// Trog: akurat to nie zwraca inta tylko char *
// Thanos: a wiec adres, moglem sie pomylic (zwlaszcza, ze w Linuxie
// nie ma manuala do tego..). Btw: strstr nie jest case sensitive?
#if !defined( WIN32 )
//char *	strcasestr			args( ( const char *, const char * ) );
#endif

/* act_comm.c */
void	ch_tell				args( ( CHAR_DATA *ch, CHAR_DATA *victim, const char *argument ) );
void	ch_tell				args( ( CHAR_DATA *ch, CHAR_DATA *victim, char *argument ) );
void	sound_to_room		args( ( ROOM_INDEX_DATA *room , const char *argument ) );
bool	circle_follow		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	add_follower		args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void	stop_follower		args( ( CHAR_DATA *ch ) );
void	die_follower		args( ( CHAR_DATA *ch ) );
bool	is_same_group		args( ( CHAR_DATA *ach, CHAR_DATA *bch ) );
void	to_channel			args( ( const char *argument, int64 channel,
									const char *verb, int level ) );
void	talk_auction		args( ( char *argument ) );
LANG_DATA * find_lang		args( ( const char *name ) );
KNOWN_LANG * find_klang		args( ( CHAR_DATA *ch, LANG_DATA *lang ) );
bool	can_speak			args( ( CHAR_DATA *ch, LANG_DATA *lang ) );
int		knows_language		args( ( CHAR_DATA *ch, LANG_DATA *language,
									CHAR_DATA *cch ) );
bool	can_learn_lang		args( ( CHAR_DATA *ch, LANG_DATA *language ) );
int	countlangs		args( ( KNOWN_LANG *first_klang ) );
char *	translate		args( ( int percent, const char *in, const char *name ) );
char *	obj_short			args( ( OBJ_DATA *obj ) );

/* act_info.c */
int		get_door			args( ( char *arg ) );
char *	format_obj_to_char	args( ( OBJ_DATA *obj, CHAR_DATA *ch,
									bool fShort ) );
void	show_list_to_char	args( ( OBJ_DATA *list, CHAR_DATA *ch,
									bool fShort, bool fShowNothing ) );

/* act_move.c */
void	clear_vrooms		args( ( void ) );
ED *	find_door			args( ( CHAR_DATA *ch, char *arg, bool quiet ) );
ED *	get_exit			args( ( ROOM_INDEX_DATA *room, int dir ) );
ED *	get_exit_to			args( ( ROOM_INDEX_DATA *room, int dir, int vnum ) );
ED *	get_exit_num		args( ( ROOM_INDEX_DATA *room, int count ) );
ch_ret	move_char			args( ( CHAR_DATA *ch, EXIT_DATA *pexit, int fall ) );
void	teleport			args( ( CHAR_DATA *ch, int room, int64 flags ) );
int		encumbrance			args( ( CHAR_DATA *ch, int move ) );
bool	will_fall			args( ( CHAR_DATA *ch, int fall ) );
int		wherehome			args( ( CHAR_DATA *ch ) );

/* act_obj.c */
void	show_obj_stats		args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
obj_ret damage_obj			args( ( OBJ_DATA *obj ) );
int		get_obj_resistance	args( ( OBJ_DATA *obj ) );
void	save_clan_storeroom args( ( CHAR_DATA *ch, CLAN_DATA *clan ) );
void	obj_fall			args( ( OBJ_DATA *obj, bool through ) );
bool	isDefelInvisible	(CHAR_DATA *ch);
bool	isDefelVisible		(OBJ_DATA *obj);

/* act_wiz.c */
void	close_area			args( ( AREA_DATA *pArea ) );
RID *	find_location		args( ( CHAR_DATA *ch, char *arg ) );
void	echo_to_room		args( ( ROOM_INDEX_DATA *room, const char *argument ) );
void	echo_to_area		args( ( AREA_DATA *room, char *argument ) );
void	echo_to_all			args( ( const char *argument, int tar ) );
void	get_reboot_string	args( ( void ) );
struct	tm *update_time		args( ( struct tm *old_time ) );
void	add_social			args( ( SOCIALTYPE *social ) );
void	unlink_command		args( ( CMDTYPE *command ) );
void	add_command			args( ( CMDTYPE *command ) );

/* boards.c */
void	note_count		args( ( CHAR_DATA *ch ) );//added by Thanos
void	load_boards			args( ( void ) );
BD *	get_board			args( ( OBJ_DATA *obj ) );
void	note				args( ( const char *name, const char *to, const char *subject,
									const char *text ) );
/* build.c */
int		get_dir				args( ( char *txt  ) );
char *	strip_cr			args( ( char *str  ) );
int		get_vip_flag		args( ( char *flag ) );
int		get_wanted_flag		args( ( char *flag ) );

/* clans.c */
CL		*get_clan			args( ( char *name ) );
void	load_clans			args( ( void ) );
void	save_clan			args( ( CLAN_DATA *clan ) );
void	save_clan2			args( ( CLAN_DATA *clan ) );
CL		*load_clan2			args( ( const char *filename ) );
long	get_taxes			args( ( PLANET_DATA *planet ) );
MD		*get_member			args( ( CLAN_DATA *clan, char *member ) );
bool	in_clan				args( ( CHAR_DATA *ch, CLAN_DATA *clan ) );
void	add_member			args( ( CLAN_DATA *clan, char *member, int status ) );
void	remove_member		args( ( CLAN_DATA *clan, char *member ) );
void	rename_member		args( ( const char *name, const char *newname ) );
void	clan_members		args( ( CHAR_DATA *ch ) );
void	induct_member		args( ( CLAN_DATA *clan, CHAR_DATA *ch ) );
void	outcast_member		args( ( CLAN_DATA *clan, CHAR_DATA *ch ) );
void	adjust_clan			args( ( CLAN_DATA *clan ) );
bool	clan_bestow			args( ( CHAR_DATA *ch, const char *cmd ) );

/* planets.c */
PD		*get_planet			args( ( const char *name ) );
void	load_planets		args( ( void ) );
void	save_planet			args( ( PLANET_DATA *planet ) );

/* bounty.c */
void	load_bounties		args( ( void ) );
void	save_bounties		args( ( void ) );
void	load_informs		args( ( void ) );
ID *	add_inform			args( ( CHAR_DATA *victim, CHAR_DATA *attacker ) );
ID *	get_inform			args( ( char *attacker, char *victim ) );
void	save_informs		args( ( void ) );
SWBounty *is_bounty_victim_killed(CHAR_DATA *);
SWBounty *is_bounty_completed(OBJ_DATA *,CHAR_DATA *);
SWBounty *is_bounty_object(OBJ_DATA *);
void delete_bounty(SWBounty *);
void finalize_bounty(SWBounty *,CHAR_DATA *,CHAR_DATA *,OBJ_DATA *);
int get_bounty_xp(CHAR_DATA *, CHAR_DATA *);

/* space.c */
SH *	get_ship			args( ( char *name ) );
void	load_ships			args( ( void ) );
//added by Thanos (�aduje statki publiczne)
void	load_pub_ships			args( ( void ) );
void	save_ship				args( ( SHIP_DATA *ship ) );
void	load_space				args( ( void ) );
void	save_starsystem			args( ( SPACE_DATA *starsystem ) );
SD *	starsystem_from_name	args( ( char *name ) );
SD *	starsystem_from_vnum	args( ( int vnum ) );
SH *	ship_from_obj			args( ( int vnum ) );
SH *	ship_from_entrance		args( ( RID *room ) );
SH *	ship_from_hangar		args( ( RID *room ) );
SH *	ship_from_cockpit		args( ( RID *room ) );
SH *	ship_from_navseat		args( ( RID *room ) );
SH *	ship_from_coseat		args( ( RID *room ) );
SH *	ship_from_pilotseat		args( ( RID *room ) );
SH *	ship_from_gunseat		args( ( RID *room ) );
SH *	ship_from_turret		args( ( RID *room ) );
SH *	ship_from_engine		args( ( RID *room ) );
SH *	ship_from_room			args( ( RID *room ) );
SH *	ship_from_pilot			args( ( char *name ) );
SH *	get_ship_here			args( ( char *name , SPACE_DATA *starsystem ) );
void	showstarsystem			args( ( CHAR_DATA *ch , SPACE_DATA *starsystem ) );
void	update_space			args( ( void ) );
void	recharge_ships			args( ( void ) );
void	move_ships				args( ( void ) );
void	update_bus				args( ( void ) );
void	update_traffic			args( ( void ) );
bool	check_pilot				args( ( CHAR_DATA *ch , SHIP_DATA *ship ) );
bool	is_rental				args( ( CHAR_DATA *ch , SHIP_DATA *ship ) );
void	echo_to_ship			args( ( SHIP_DATA *ship , const char *argument ) );
void	echo_to_ship_nospam		args( ( SHIP_DATA *ship , const char *argument ) );
void	echo_to_cockpit			args( ( SHIP_DATA *ship , const char *argument ) );
void	echo_to_system			args( ( SHIP_DATA *ship , const char *argument ,
										SHIP_DATA *ignore ) );
bool	extract_ship			args( ( SHIP_DATA *ship ) );
bool	ship_to_room			args( ( SHIP_DATA *ship , RID *room ) );
long	get_ship_value			args( ( SHIP_DATA *ship ) );
bool	rent_ship				args( ( CHAR_DATA *ch , SHIP_DATA *ship ) );
void	damage_ship				args( ( SHIP_DATA *ship , int min , int max ) );
void	damage_ship_ch			args( ( SHIP_DATA *ship , int min , int max ,
										CHAR_DATA *ch ) );
void	destroy_ship			args( ( SHIP_DATA *ship , CHAR_DATA *ch ) );
void	ship_to_starsystem		args( ( SHIP_DATA *ship , SPACE_DATA *starsystem ) );
void	ship_from_starsystem	args( ( SHIP_DATA *ship , SPACE_DATA *starsystem ) );
void	new_missile				args( ( SHIP_DATA *ship , SHIP_DATA *target,
										CHAR_DATA *ch , int missiletype ) );
void	extract_missile			args( ( MISSILE_DATA *missile ) );
SH *	ship_in_room			args( ( ROOM_INDEX_DATA *room, char *name ) );
float	srange					args( ( float x , float y , float z ) );
/*aldegard zmniejszylem ciut wielkosc pliku no i zaokraglilem system*/
int		get_sorted_systems		args( ( SD ***sorted_systems ) ); /* Trog */
void	write_starsystem_list	args( ( void ) );
void	extract_starsystem	args( ( SPACE_DATA *starsystem ) );
SHIP_INDEX_DATA *get_ship_index args( ( char * ship_index ) );
void	delete_space_reset	args( ( SPACE_DATA *pStarsystem, RESET_DATA *pReset ) );
RESET_DATA *get_space_reset	args( ( SPACE_DATA *pStarsystem, int num ) );
void	reset_starsystem	args( ( SPACE_DATA *starsystem ) );
RESET_DATA *add_space_reset	args( ( SPACE_DATA *starsystem, char letter, int extra, int64 arg1, int64 arg2, int64 arg3, int64 arg4 ) );
void	display_space_resets	args( ( SPACE_DATA *pStarsystem, CHAR_DATA *ch ) );
MATERIAL_DATA * get_material( char * name, material_type typ );
DEPOSIT_DATA * get_deposit( PLANET_DATA * planet, int nr);
//float	abs( float );



/* space2.c all by Aldegard*/
void	clear_temp_systems			args( ( void ) );
void	ship_to_repository			args( ( SHIP_DATA *ship ) );
void	astro_to_starsystem			args( ( ASTRO_DATA *astro,
											SPACE_DATA *starsystem ) );
void	astro_from_starsystem		args( ( ASTRO_DATA *astro,
											SPACE_DATA *starsystem ) );
void	load_astros				args( ( void ) );
void	save_astro			args( ( ASTRO_DATA *astro ) );
AS *	get_astro					args( ( char *name ) );
AS *	get_astro_here				args( ( char *name , SPACE_DATA *starsystem ) );
void	extract_astro				args( ( ASTRO_DATA *astro ) );
void	update_astro			args( ( void ) );
void	echo_to_system_astro		args( ( ASTRO_DATA *astro , char *argument,
											SHIP_DATA *ignore ) );
void	new_missile_astro		args( ( ASTRO_DATA *astro , SHIP_DATA *target,
											int missiletype ) );
bool	check_crew					args( ( SHIP_DATA *ship, CHAR_DATA *ch,
											const char *argument ) );
void	echo_to_crew				args( ( SHIP_DATA *ship , char *argument ) );
TURRET_DATA * get_turret			args( ( SHIP_DATA *ship , int nr ) );
TURRET_DATA * get_turret_here		args( ( SHIP_DATA *ship , RID *room ) );
bool	is_vnum_ship_system			args( ( SHIP_DATA *ship , int vnum ,
											const char *argument ) );
MOON_DATA * get_moon				args( ( SPACE_DATA *starsystem , int nr ) );
MOON_DATA * moon_from_name			args( ( SPACE_DATA *starsystem ,
											char *argument ) );
DOCK_DATA * get_dock				args( ( PLANET_DATA *planet , int nr ) );
DOCK_DATA * dock_from_name			args( ( SPACE_DATA *starsystem ,
											char *argument ) );
HANGAR_DATA * get_hangar			args( ( SHIP_DATA *ship , int nr ) );
HANGAR_DATA * hangar_from_room		args( ( SHIP_DATA *ship , int vnum ) );
CARGO_DATA * gat_cargo				args( ( SHIP_DATA *ship , int nr ) );
PLANET_DATA * planet_from_dock		args( ( DOCK_DATA *dock, CHAR_DATA *ch	) );
float check_capacity				args( ( int vnum ) );
bool has_hangar						args( ( SHIP_DATA *ship ) );
STAR_DATA *get_star					args( ( SPACE_DATA *system, int nr ) );
bool	is_dock_area				args( ( PLANET_DATA *planet , int vnum ) );
void	generate_transponder		args( ( SHIP_DATA *ship ) );
void	clear_transponders			args( ( SHIP_DATA *ship ) );
bool	know_trans					args( ( SHIP_DATA *ship , SHIP_DATA *target) );
TRANSPONDER_DATA * get_transponder	args( ( SHIP_DATA *ship , int nr ) );
MODULE_DATA *get_module				args( ( SHIP_DATA *ship , int nr ) );
bool	is_ship_fight				args( ( SHIP_DATA *ship ) );
bool	is_fighter					args( (SHIP_DATA *ship) );
bool	is_midship					args( (SHIP_DATA *ship) );
bool	is_capital					args( (SHIP_DATA *ship) );
bool	is_platform					args( (SHIP_DATA *ship) );
bool	is_ship						args( (SHIP_DATA *ship) );
int	count_modules_all				args( ( SHIP_DATA *ship ) );
int	count_modules					args( ( SHIP_DATA *ship , int nr ) );
MODULE_DATA *get_module_ready		args( ( SHIP_DATA *ship , int type ) );
void	show_module_to_char			args( ( SHIP_DATA *ship , CHAR_DATA *ch , int nvnum ) );
void	show_module_to_char0		args( ( SHIP_DATA *ship , CHAR_DATA *ch , int nvnum ) );
MODULE_DATA *get_module_ch			args( ( SHIP_DATA *ship , int nvnum , int nr ) );
void	generate_sslook				args( ( SHIP_DATA *ship ) );
SHIPDOCK_DATA * get_shipdock		args( ( SHIP_DATA *ship , int nr ) );
SHIPDOCK_DATA * shipdock_from_room	args( ( SHIP_DATA *ship, int vnum ) );
bool	has_dock					args( ( SHIP_DATA *ship ) );
SH *	ship_from_dock			    args( ( int vnum ) );
void	leave_ship_by_dock			args( ( SHIP_DATA *ship, CHAR_DATA *ch ) );
bool	is_linked					args( ( SHIP_DATA *ship , SHIP_DATA *target ) );
SHIPDOCK_DATA * shipdock_from_slave	args( ( SHIP_DATA *ship ) );
void	uncrew						args( ( SHIP_DATA *ship , CHAR_DATA *ch ) );
bool	are_crew_onboard			args( ( SHIP_DATA *ship ) );
void	star_update			args( ( void ) );
void	setallships					args( ( CHAR_DATA *ch , char *argument ,
											char *argument2 ) );
bool	is_linked_with				args( ( SHIP_DATA *ship , SHIP_DATA *target ) );
bool	is_scout					args( (SHIP_DATA *ship) );
bool	is_freighter				args( (SHIP_DATA *ship) );
bool	is_huge						args( (SHIP_DATA *ship) );
int		get_space_chance			args( (SHIP_DATA *ship , CHAR_DATA *ch ) );
void	learn_from_failure_space	args( (SHIP_DATA *ship , CHAR_DATA *ch ) );
void	learn_from_success_space	args( (SHIP_DATA *ship , CHAR_DATA *ch ) );
bool	known_biotech				args( ( SHIP_DATA *ship , CHAR_DATA *ch ) );
void	yuuzhan_info				args( ( SHIP_DATA *ship , CHAR_DATA *ch ) );

/* shpdb.c */
int		search_free_vnums		args( ( AREA_DATA *area, int ammount ) );

/* space_reset.c */
void	place_ship_in_room		args( ( SHIP_DATA *ship, ROOM_INDEX_DATA *pRoomIndex ) );
void	place_ship_in_space		args( ( SHIP_DATA *ship,  SPACE_DATA *starsystem, int arg1, int arg2, int arg3 ) );

/* shpsave.c */
void	save_ship_state			args( ( SHIP_DATA *ship ) );

/* comm.c */
void	close_socket			args( ( DESCRIPTOR_DATA *dclose, bool force ) );
int		write_to_buffer			args( ( DESCRIPTOR_DATA *d, const char *txt,
										int length ) );
void	write_to_pager			args( ( DESCRIPTOR_DATA *d, const char *txt,
										int length ) );
void	send_to_char			args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_char_color		args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_pager			args( ( const char *txt, CHAR_DATA *ch ) );
void	send_to_pager_color		args( ( const char *txt, CHAR_DATA *ch ) );
void	ch_printf				args( ( CHAR_DATA *ch, const char *fmt, ... ) );
void	pager_printf			args( ( CHAR_DATA *ch, const char *fmt, ...) );
void	act						args( ( const char *color, const char *format,
										CHAR_DATA *ch, const void *arg1,
										const void *arg2, int type ) );
char	*mkpasswd				args( ( char *pass ) );
#if defined(__FreeBSD__) && __FreeBSD__ == 5
void	cpu_usage				args( ( void ) );
#endif
/* uglobalnione dla potrzeb CopyOver		--Thanos */
bool	write_to_descriptor		args( ( int desc, const char *txt, int length ) );
void	save_sysdata			args( ( SYSTEM_DATA sys ) );

/* reset.c */
RD  *	make_reset				args( ( char letter, int extra, int arg1,
										int arg2, int arg3 ) );
RD  *	add_reset				args( ( RID *room, char letter, int extra,
										int arg1, int arg2, int arg3 ) );
RD  *	place_reset				args( ( AREA_DATA *tarea, char letter,
										int extra, int arg1, int arg2, int arg3 ) );
void	reset_area				args( ( AREA_DATA * pArea ) );
RID		*validate_reset			args( ( AREA_DATA *area, RESET_DATA *reset, int64 *iLastRoom, int64 *iLastObj ) );

/* dialogs.c */
int		free_dialog_ID			args( ( ) );

/* db.c */
void	show_file				args( ( CHAR_DATA *ch, const char *filename ) );
char *	str_dup					args( ( char const *str ) );
void	boot_db					args( ( bool fCopyOver ) );
int64	mprog_name_to_type		args( ( char* name ) );

/* CopyOver Stuff	--Thanos */
void	copyover				args( ( void ) );
void	init_descriptor			args( ( DESCRIPTOR_DATA *dnew, int desc) );

void	area_update				args( ( void ) );
void	add_char				args( ( CHAR_DATA *ch ) );
CD *	create_mobile			args( ( MOB_INDEX_DATA *pMobIndex ) );
OD *	create_object			args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
void	clear_char				args( ( CHAR_DATA *ch ) );
void	clear_pcdata			args( ( PC_DATA	  *pcdata ) );
char *	get_extra_descr			args( ( const char *name, EXTRA_DESCR_DATA *ed ) );
MID *	get_mob_index			args( ( int vnum ) );
OID *	get_obj_index			args( ( int vnum ) );
RID *	get_room_index			args( ( int vnum ) );
char	fread_letter			args( ( FILE *fp ) );
int		fread_number			args( ( FILE *fp ) );
int		fread_number64			args( ( FILE *fp ) );
char *	st_fread_string			args( ( FILE *fp ) );
char *	fread_string			args( ( FILE *fp ) );
void	fread_to_eol			args( ( FILE *fp ) );
char *	fread_word				args( ( FILE *fp ) );
char *	fread_line				args( ( FILE *fp ) );
int		number_fuzzy			args( ( int number ) );
int		number_range			args( ( int from, int to ) );
int		number_percent			args( ( void ) );
int		number_door				args( ( void ) );
int		number_bits		args( ( int width ) );
int		number_mm		args( ( void ) );
int		dice			args( ( int number, int size ) );
int		interpolate		args( ( int level, int value_00, int value_32 ) );
void	smash_tilde		args( ( char *str ) );
void	hide_tilde		args( ( char *str ) );
char *	show_tilde		args( ( char *str ) );
bool	sw_strcmp		args( ( const char *astr, const char *bstr ) );
bool	str_cmp			args( ( const char *astr, const char *bstr ) );
bool	str_prefix		args( ( const char *astr, const char *bstr ) );
bool	str_infix		args( ( const char *astr, const char *bstr ) );
char *	str_str			args( ( const char *astr, char *bstr ) );
char *	str_repl		args( ( char * orig, const char * old, const char * _new, bool rAll ) );
bool	str_suffix		args( ( const char *astr, const char *bstr ) );
bool	reg_cmp			args( ( const char *regexp, const char *str ) );
char *	capitalize		args( ( const char *str ) );
char *	fcapitalize		args( ( const char *str ) );
char *	strlower		args( ( const char *str ) );
char *	strupper		args( ( const char *str ) );
void	append_file		args( ( CHAR_DATA *ch, const char *file, char *str ) );
void	append_to_file		args( ( const char *file, char *str ) );
void	_bug			args( ( const char *str, ... ) );
void	_perror			args( ( const char *fmt, ... ) );
void	log_string_plus		args( ( const char *str, int log_type, int level ) );
RID *	make_room		args( ( int vnum, int svnum ) );
OID *	make_object		args( ( int vnum, int cvnum, char *name ) );
MID *	make_mobile		args( ( int vnum, int cvnum, char *name ) );
ED  *	make_exit		args( ( ROOM_INDEX_DATA *pRoomIndex, ROOM_INDEX_DATA *to_room, int door ) );
void	add_help		args( ( HELPS_FILE *fHelp, HELP_DATA *pHelp, bool moveto ) );
void	fix_area_exits		args( ( AREA_DATA *tarea ) );
void	load_area_file		args( ( AREA_DATA *tarea, char *filename ) );
void	randomize_exits		args( ( ROOM_INDEX_DATA *room, int maxdir ) );
//void	make_wizlist		args( ( void ) );
void	tail_chain		args( ( void ) );
bool	delete_room		args( ( ROOM_INDEX_DATA *room ) );
bool	delete_obj		args( ( OBJ_INDEX_DATA *obj ) );
bool	delete_mob		args( ( MOB_INDEX_DATA *mob ) );
/* Functions to add to sorting lists. -- Altrag */
/*void	mob_sort		args( ( MOB_INDEX_DATA *pMob ) );
void	obj_sort		args( ( OBJ_INDEX_DATA *pObj ) );
void	room_sort		args( ( ROOM_INDEX_DATA *pRoom ) );*/
void	sort_area		args( ( AREA_DATA *pArea, bool proto ) );
void	unlink_room		(RID *);
bool	is_room_unlinked		(RID *);

/* build.c */
bool	can_rmodify		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *room ) );
bool	can_omodify		args( ( CHAR_DATA *ch, OBJ_DATA *obj  ) );
bool	can_mmodify		args( ( CHAR_DATA *ch, CHAR_DATA *mob ) );
bool	can_medit		args( ( CHAR_DATA *ch, MOB_INDEX_DATA *mob ) );
EDD *	SetRExtra		args( ( ROOM_INDEX_DATA *room, char *keywords ) );
bool	DelRExtra		args( ( ROOM_INDEX_DATA *room, char *keywords ) );
EDD *	SetOExtra		args( ( OBJ_DATA *obj, const char *keywords ) );
EDD *	SetOExtra		args( ( OBJ_DATA *obj, char *keywords ) );
bool	DelOExtra		args( ( OBJ_DATA *obj, char *keywords ) );
EDD *	SetOExtraProto		args( ( OBJ_INDEX_DATA *obj, char *keywords ) );
bool	DelOExtraProto		args( ( OBJ_INDEX_DATA *obj, char *keywords ) );
void	fold_area		args( ( AREA_DATA *tarea, char *filename, bool install ) );

/* buildxml.c */
void	save_area_list	args( ( ) );
void	save_area		args( ( AREA_DATA *area ) );
AREA_DATA	*load_area2	args( ( const char *filename ) );
bool	install_area	args( ( AREA_DATA *area ) );
MID		*get_mob_tmp	( int vnum, AREA_DATA *area );
OID	*get_obj_tmp	( int vnum, AREA_DATA *area );
RID	*get_room_tmp	( int vnum, AREA_DATA *area );

//added by Thanos
/* ciapek.c */
char *	nocolor				args( ( const char *src ) );
char *	my_one_argument		args( ( char *argument, char *arg_first ) );
const char *	strip_colors		args( ( const char *str, int length ) );
void	toggle_string		args( ( char **string, char *flag ) );
char *	cut_from_string		args( ( char *s_flag, const char *name ) );
void	add_to_last_buf		args( ( CHAR_DATA *ch, char *tekst, const char *verb, int64 channel ) );
void	add_to_last_tell	args( ( CHAR_DATA *ch, CHAR_DATA *sender, const char *tekst ) );
void	add_to_last_ctalk	args( ( CLAN_DATA *clan, CHAR_DATA *sender, const char *tekst ) );
char *	show_exp_stars		args( ( CHAR_DATA *ch, int ability) );
void	show_tick		args( ( void ) );
void	update_status_bar	args( ( CHAR_DATA *ch ) );
char *	all_capitalize		args( ( const char *str ) );
char *	shrtime			args( ( char *data, int type ) );
char *	pltime			args( ( char *data ) );
char	shiftpl			args( ( bool shift, char letter ) );
bool	ispolchar		args( ( const char letter ) );

/* complains.c -- Thanos */
void	load_complains		args( ( ) );
void	save_complains		args( ( bool fixed ) );
void	show_complains		args( ( CHAR_DATA *ch, int type ) );

/* crimes.c -- Thanos*/
void	destroy_deposit		args( ( CHAR_DATA *ch ) );
CD   *	deposit_owner		args( ( OBJ_DATA *obj ) );
bool	is_deposit		args( ( OBJ_DATA *obj ) );
void	save_deposit		args( ( CHAR_DATA *ch ) );
void	load_deposit		args( ( CHAR_DATA *ch ) );
OD   *	find_jail_closet	args( ( CHAR_DATA *ch, PLANET_DATA *planet ) );
CRD  *	find_crime		args( ( CHAR_DATA *ch, PLANET_DATA *planet ) );
void	crime_to_char		args( ( CHAR_DATA *ch, char *planet, int type ) );
void	crime_remove		args( ( CHAR_DATA *ch, char *planet, int type ) );
void	free_crime		args( ( CHAR_DATA *ch, CRIME_DATA *crime ) );
int		calculate_wanted_price	args( ( const CHAR_DATA *ch ) ); /* Ganis */

/* snippets.c */
void	load_tongues		args( ( void ) );

/* mem.c */
#include "mem.h"
MATERIAL_DATA * get_material ( char * name, material_type typ);


/* olc_misc.c (only global = not only for OLC) */
RACE_DATA	*find_race	args( ( char *name ) );

void	clear_helps_file	args( ( char *filename ) );
/* string.c */
void	string_edit		args( ( CHAR_DATA *ch, char **pString ) );
void	string_append		args( ( CHAR_DATA *ch, char **pString ) );
char *	string_replace		args( ( char * orig, char * old, char * _new, bool sensitive ) );
void	string_add		args( ( CHAR_DATA *ch, char *argument ) );
char *	format_string		args( ( char *oldstring, int len ) );
char *	first_arg		args( ( char *argument, char *arg_first, bool fCase ) );
char *	string_unpad		args( ( char * argument ) );
char *	string_proper		args( ( char * argument ) );
int     swsnprintf      args( (char *str, size_t size, const char *format, ...) );
char *  swstrncpy       args( (char *dest, const char *src, size_t n) );


/* flags.c */
int64	flag_value		args( ( const struct flag_type * flag_table,
					const char *argument ) );
int64	flag_value		args( ( const struct flag_type * flag_table,
					char *argument ) );
char *	flag_string		args( ( const struct flag_type * flag_table,
						int64 bits ) );
char *	bit_name		args( ( const struct flag_type * flag_table,
						int64 bit ) );
int	wear_loc		args( ( int bits, int count ) );
int	wear_bit		args( ( int loc ) );
int	adv_atoi		args( ( char *s ) );
int64	atoi64		args( ( char *s ) );
bool	adv_is_number		args( ( char *arg ) );
int	find_q_arg		args( ( int cmdtype, int cmd, char * txt ) );
const char *	show_q_arg		args( ( int cmdtype, int cmd, int argnr ) );

/* fight.c */
int	fight_lev		args( ( CHAR_DATA *ch ) );
int	max_fight		args( ( CHAR_DATA *ch ) );
void	violence_update		args( ( void ) );
ch_ret	multi_hit		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
int	ris_damage		args( ( CHAR_DATA *ch, int dam, int ris ) );
ch_ret	damage			args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
					int dt ) );
void	update_pos		args( ( CHAR_DATA *victim ) );
void	set_fighting		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	stop_fighting		args( ( CHAR_DATA *ch, bool fBoth ) );
void	free_fight		args( ( CHAR_DATA *ch ) );
CD *	who_fighting		args( ( CHAR_DATA *ch ) );
void	check_killer		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	check_attacker		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	death_cry		args( ( CHAR_DATA *ch ) );
void	stop_hunting		args( ( CHAR_DATA *ch ) );
void	stop_hating		args( ( CHAR_DATA *ch ) );
void	stop_fearing		args( ( CHAR_DATA *ch ) );
void	start_hunting		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	start_hating		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	start_fearing		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_hunting		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_hating		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_fearing		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe			args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	is_safe_nm		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	legal_loot		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_illegal_pk	args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	raw_kill		args( ( CHAR_DATA *ch, CHAR_DATA *victim, int suicide ) );
bool	in_arena		args( ( CHAR_DATA *ch ) );

/* makeobjs.c */
void	make_corpse		args( ( CHAR_DATA *ch, CHAR_DATA *killer, int suicide ) );
void	make_scraps		args( ( OBJ_DATA *obj ) );
OD *	make_trap		args( ( int v0, int v1, int v2, int v3 ) );
OD *	create_money		args( ( int amount ) );

/* misc.c */
bool get_comlink		args( ( CHAR_DATA *ch ) );/*Thanos*/
void actiondesc			args( ( CHAR_DATA *ch, OBJ_DATA *obj, void *vo ) );
void jedi_checks		args( ( CHAR_DATA *ch ) );
void jedi_bonus			args( ( CHAR_DATA *ch ) );
void sith_penalty		args( ( CHAR_DATA *ch ) );

//misc.c
const SWString char_attribute	args( (desc_type desc_type, int index, int gender, int przypadek) );
bool does_knows			args( ( CHAR_DATA *ch, CHAR_DATA *victim ) ); /*Tanglor*/
bool does_knows_name	args( (CHAR_DATA * ch, char * vict_name) );
const SWString format_char_attribute	args( (CHAR_DATA *ch, int przypadek) );
char * format_char_attribute_wrapper	args( (CHAR_DATA *ch, int przypadek) );
const bool is_invited			args( (const CHAR_DATA * ch, const ROOM_INDEX_DATA * home) ); /*Ganis*/
void evacuate_guests	args( (ROOM_INDEX_DATA * home) ); /*Ganis*/

/* mud_comm.c */
const char *	mprog_type_to_name	args( ( int64 type ) );
void	defragment_list		args( ( char **list ) );
bool	isinnameslist		args( ( CHAR_DATA *ch, char *arg ) );
bool	nameslistfull		args( ( CHAR_DATA *ch ) );
bool	nameslistempty		args( ( CHAR_DATA *ch ) );
void	save_nameslist		args( ( CHAR_DATA *ch ) );
void	fread_nameslist		args( ( MOB_INDEX_DATA *pMobIndex ) );

/* mud_prog.c */
#ifdef DUNNO_STRSTR
char *	strstr			args ( (const char *s1, const char *s2 ) );
#endif

void	mprog_wordlist_check	args ( ( char * arg, CHAR_DATA *mob,
							CHAR_DATA* actor, OBJ_DATA* object,
					void* vo, int64 type ) );
void	mprog_percent_check	args ( ( CHAR_DATA *mob, CHAR_DATA* actor,
					OBJ_DATA* object, void* vo,
					int type ) );
void	mprog_act_trigger	args ( ( char* buf, CHAR_DATA* mob,
								CHAR_DATA* ch, OBJ_DATA* obj,
					void* vo ) );
void	mprog_bribe_trigger	args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
								int amount ) );
void	mprog_entry_trigger	args ( ( CHAR_DATA* mob ) );
void	mprog_give_trigger	args ( ( CHAR_DATA* mob, CHAR_DATA* ch,
								OBJ_DATA* obj ) );
void	mprog_greet_trigger	args ( ( CHAR_DATA* mob ) );
void	mprog_all_greet_trigger args ( ( CHAR_DATA* mob ) ); /*Thanos*/
void	mprog_fight_trigger	args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void	mprog_hitprcnt_trigger	args ( ( CHAR_DATA* mob, CHAR_DATA* ch ) );
void	mprog_death_trigger	args ( ( CHAR_DATA *killer, CHAR_DATA* mob ) );
void	mprog_random_trigger	args ( ( CHAR_DATA* mob ) );
void	mprog_tell_trigger	args ( ( CHAR_DATA *ch, char *txt, CHAR_DATA *vict ) );
void	mprog_speech_trigger	args ( ( char* txt, CHAR_DATA* mob ) );
void	mprog_hour_trigger	args ( ( CHAR_DATA *mob ) );
void	mprog_time_trigger	args ( ( CHAR_DATA *mob ) );
void	progbug			args( ( const char *str, CHAR_DATA *mob ) );
void	rset_supermob		args( ( ROOM_INDEX_DATA *room) );
void	release_supermob	args( ( ) );
void	set_supermob		args( ( OBJ_DATA *obj ) );

/* player.c */
void	set_title		args( ( CHAR_DATA *ch, char *title ) );
bool	check_clone		args( ( CHAR_DATA *ch ) );

/* skills.c */
bool	check_skill		args( ( CHAR_DATA *ch, char *command, char *argument ) );
bool	check_knowledge		args( ( CHAR_DATA *ch, int sn ) );
void	break_skill		args( ( const char *txt, CHAR_DATA *ch ) );
void	mount_msg		args( ( CHAR_DATA *ch ) );
void	concentrate_msg		args( ( CHAR_DATA *ch ) );
void	hes_not_here_msg	args( ( CHAR_DATA *ch ) );
void	its_not_here_msg	args( ( CHAR_DATA *ch ) );
void	notfight_msg		args( ( CHAR_DATA *ch ) );
void	learn_from_success	args( ( CHAR_DATA *ch, int sn ) );
void	learn_from_failure	args( ( CHAR_DATA *ch, int sn ) );
bool	check_parry		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_dodge		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	check_grip		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	disarm			args( ( CHAR_DATA *ch, CHAR_DATA *victim) );
void	trip			args( ( CHAR_DATA *ch, CHAR_DATA *victim) );


/* handler.c */
void	explode			args( ( OBJ_DATA *obj ) );
int	get_exp			args( ( CHAR_DATA *ch , int ability ) );
int	get_exp_worth		args( ( CHAR_DATA *ch ) );
int	exp_level		args( ( int level ) );
int	get_trust		args( ( CHAR_DATA *ch ) );
int	get_age			args( ( CHAR_DATA *ch ) );
int	get_curr_str		args( ( CHAR_DATA *ch ) );
int	get_curr_int		args( ( CHAR_DATA *ch ) );
int	get_curr_wis		args( ( CHAR_DATA *ch ) );
int	get_curr_dex		args( ( CHAR_DATA *ch ) );
int	get_curr_con		args( ( CHAR_DATA *ch ) );
int	get_curr_cha		args( ( CHAR_DATA *ch ) );
int get_curr_lck		args( ( CHAR_DATA *ch ) );
int get_curr_frc		args( ( CHAR_DATA *ch ) );
#if defined( ARMAGEDDON )
int	get_max_str		args( ( CHAR_DATA *ch ) );
int	get_max_int		args( ( CHAR_DATA *ch ) );
int	get_max_wis		args( ( CHAR_DATA *ch ) );
int	get_max_dex		args( ( CHAR_DATA *ch ) );
int	get_max_con		args( ( CHAR_DATA *ch ) );
int	get_max_cha		args( ( CHAR_DATA *ch ) );
int get_max_lck		args( ( CHAR_DATA *ch ) );
int get_max_frc		args( ( CHAR_DATA *ch ) );
#endif
bool	can_take_proto		args( ( CHAR_DATA *ch ) );
int	can_carry_n		args( ( CHAR_DATA *ch ) );
int	can_carry_w		args( ( CHAR_DATA *ch ) );
bool	is_name			args( ( const char *str, char *namelist ) );
bool	is_name			args( ( const char *str, const char *namelist ) );
bool	is_name_prefix		args( ( const char *str, const char *namelist ) );
bool	is_name_prefix		args( ( const char *str, char *namelist ) );
bool	nifty_is_name		args( ( char *str, char *namelist ) );
bool	nifty_is_name_prefix	args( ( char *str, char *namelist ) );
void	affect_modify		args( ( CHAR_DATA *ch, AFFECT_DATA *paf, bool fAdd ) );
void	affect_to_char		args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_remove		args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	affect_strip		args( ( CHAR_DATA *ch, int sn ) );
bool	is_affected		args( ( CHAR_DATA *ch, int sn ) );
void	affect_join		args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	char_from_room		args( ( CHAR_DATA *ch ) );
void	char_to_room		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void	obj_personalize		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
OD *	obj_to_char		args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void	obj_from_char		args( ( OBJ_DATA *obj ) );
int	apply_ac		args( ( OBJ_DATA *obj, int iWear ) );
OD *	get_eq_char		args( ( CHAR_DATA *ch, int iWear ) );
void	equip_char		args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void	unequip_char		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	count_obj_list		args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void	obj_from_room		args( ( OBJ_DATA *obj ) );
OD *	obj_to_room		args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
OD *	obj_to_obj		args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void	obj_from_obj		args( ( OBJ_DATA *obj ) );
void	extract_obj		args( ( OBJ_DATA *obj ) );
void	extract_exit		args( ( ROOM_INDEX_DATA *room, EXIT_DATA *pexit ) );
void	extract_room		args( ( ROOM_INDEX_DATA *room ) );
void	clean_room		args( ( ROOM_INDEX_DATA *room ) );
void	clean_obj		args( ( OBJ_INDEX_DATA *obj ) );
void	clean_mob		args( ( MOB_INDEX_DATA *mob ) );
void	clean_resets		args( ( AREA_DATA *tarea ) );
void	extract_char		args( ( CHAR_DATA *ch, bool fPull ) );
CD *	get_char_room		args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_char_world		args( ( CHAR_DATA *ch, char *argument ) );
CD *	get_player_world	args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_type		args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *	get_obj_list		args( ( CHAR_DATA *ch, char *argument,
					const std::list<OBJ_DATA*>& list ) );
OD *	get_obj_list_rev	args( ( CHAR_DATA *ch, char *argument,
					std::list<OBJ_DATA*>& list ) );
OD *	get_obj_carry		args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_wear		args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_here		args( ( CHAR_DATA *ch, char *argument ) );
OD *	get_obj_world		args( ( CHAR_DATA *ch, char *argument ) );
int	get_obj_number		args( ( OBJ_DATA *obj ) );
int	get_obj_weight		args( ( OBJ_DATA *obj ) );
bool	room_is_dark		args( ( ROOM_INDEX_DATA *pRoomIndex ) );
bool	room_is_private		args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
bool	can_see			args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool	can_see_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
bool	can_drop_obj		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
//Thanos:
char *	item_type_name		args( ( OBJ_DATA *obj ) );
char *	weapon_type_pl		args( ( int value ) );
const char *	affect_loc_name_pl	args( ( int location ) );
//done
void	name_stamp_stats	args( ( CHAR_DATA *ch ) );
ch_ret	check_for_trap		args( ( CHAR_DATA *ch, OBJ_DATA *obj, int flag ) );
ch_ret	check_room_for_traps	args( ( CHAR_DATA *ch, int flag ) );
bool	is_trapped		args( ( OBJ_DATA *obj ) );
OD *	get_trap		args( ( OBJ_DATA *obj ) );
ch_ret	spring_trap		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
void	fix_char		args( ( CHAR_DATA *ch ) );
void	showaffect		args( ( CHAR_DATA *ch, AFFECT_DATA *paf ) );
void	set_cur_obj		args( ( OBJ_DATA *obj ) );
bool	obj_extracted		args( ( OBJ_DATA *obj ) );
void	queue_extracted_obj	args( ( OBJ_DATA *obj ) );
void	clean_obj_queue		args( ( void ) );
void	set_cur_char		args( ( CHAR_DATA *ch ) );
bool	char_died		args( ( CHAR_DATA *ch ) );
void	queue_extracted_char	args( ( CHAR_DATA *ch, bool extract ) );
void	clean_char_queue	args( ( void ) );
void	add_timer		args( ( CHAR_DATA *ch, int type, int count, DO_FUN *fun, int value ) );
TIMER * get_timerptr		args( ( CHAR_DATA *ch, int type ) );
int	get_timer		args( ( CHAR_DATA *ch, int type ) );
void	extract_timer		args( ( CHAR_DATA *ch, TIMER *timer ) );
void	remove_timer		args( ( CHAR_DATA *ch, int type ) );
bool	in_level_range		args( ( CHAR_DATA *ch, AREA_DATA *tarea ) );
bool	chance			args( ( CHAR_DATA *ch, int percent ) );
bool	chance_attrib		args( ( CHAR_DATA *ch, int percent, int attrib ) );
OD *	clone_object		args( ( OBJ_DATA *obj ) );
void	split_obj		args( ( OBJ_DATA *obj, int num ) );
void	separate_obj		args( ( OBJ_DATA *obj ) );
bool	empty_obj		args( ( OBJ_DATA *obj, OBJ_DATA *destobj,
					ROOM_INDEX_DATA *destroom ) );
OD *	find_obj		args( ( CHAR_DATA *ch, char *argument,
					bool carryonly ) );
bool	ms_find_obj		args( ( CHAR_DATA *ch ) );
void	worsen_mental_state	args( ( CHAR_DATA *ch, int mod ) );
void	better_mental_state	args( ( CHAR_DATA *ch, int mod ) );
void	boost_economy		args( ( AREA_DATA *tarea, int gold ) );
void	lower_economy		args( ( AREA_DATA *tarea, int gold ) );
void	economize_mobgold	args( ( CHAR_DATA *mob ) );
bool	economy_has		args( ( AREA_DATA *tarea, int gold ) );
void	add_kill		args( ( CHAR_DATA *ch, CHAR_DATA *mob ) );
int	times_killed		args( ( CHAR_DATA *ch, CHAR_DATA *mob ) );
char *	get_race		args( ( CHAR_DATA *ch ) );
void	destroy_char		args( ( char *name ) );
/* Thanos -- requirements */
bool	req_check		args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int	req_eq			args( ( REQUIREMENT_DATA *req ) );
void	show_req		args( ( CHAR_DATA *ch, REQUIREMENT_DATA *req ));
void	explain			args( ( CHAR_DATA *ch, REQUIREMENT_DATA *req,
					OBJ_DATA  *obj, bool iden ) );
/* interp.c */
void	huh				args( ( CHAR_DATA *ch ) );
void	donemsg			args( ( CHAR_DATA *ch ) );
bool	check_pos		args( ( CHAR_DATA *ch, int position ) );
char *	substitute_alias	args( ( DESCRIPTOR_DATA *d, char *input ) );
void	interpret		args( ( CHAR_DATA *ch, char *argument ) );
bool	is_number		args( ( char *arg ) );
int	number_argument		args( ( char *argument, char *arg ) );
char *	one_argument		args( ( char *argument, char *arg_first ) );
char *	one_argument2		args( ( char *argument, char *arg_first ) );
ST *	find_social		args( ( const char *command ) );
CMDTYPE *find_command		args( ( const char *command ) );
void	hash_commands		args( ( ) );
void	start_timer		args( ( struct timeval *stime ) );
time_t	end_timer		args( ( struct timeval *stime ) );
void	send_timer		args( ( struct timerset *vtime, CHAR_DATA *ch ) );
void	update_userec		args( ( struct timeval *time_used,
									struct timerset *userec ) );
bool	check_social		args( ( CHAR_DATA *ch,	const char *command,
							const char *argument ) );

/* force.c */
bool	process_spell_components args( ( CHAR_DATA *ch, int sn ) );
int	ch_slookup		args( ( CHAR_DATA *ch, const char *name ) );
int	find_spell		args( ( CHAR_DATA *ch, const char *name, bool know ) );
int	find_skill		args( ( CHAR_DATA *ch, const char *name, bool know ) );
int	find_weapon		args( ( CHAR_DATA *ch, const char *name, bool know ) );
int	find_tongue		args( ( CHAR_DATA *ch, const char *name, bool know ) );
int	skill_lookup		args( ( const char *name ) );
int	herb_lookup		args( ( const char *name ) );
int	personal_lookup		args( ( CHAR_DATA *ch, const char *name ) );
int	slot_lookup		args( ( int slot ) );
int	bsearch_skill		args( ( const char *name, int first, int top ) );
int	bsearch_skill_exact	args( ( const char *name, int first, int top ) );
bool	saves_poison_death	args( ( int level, CHAR_DATA *victim ) );
bool	saves_wand		args( ( int level, CHAR_DATA *victim ) );
bool	saves_para_petri	args( ( int level, CHAR_DATA *victim ) );
bool	saves_breath		args( ( int level, CHAR_DATA *victim ) );
bool	saves_spell_staff	args( ( int level, CHAR_DATA *victim ) );
ch_ret	obj_cast_spell		args( ( int sn, int level, CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA *obj ) );
int	dice_parse		args( (CHAR_DATA *ch, int level, char *exp) );
SK *	get_skilltype		args( ( int sn ) );
void	fevent_trigger		args( ( CHAR_DATA *ch, fe_trigger trigger, ... ) );
void	drawlots_fevents	args( ( CHAR_DATA *ch ) );

/* olc.c ??? */
void	show_obj_values		args( ( CHAR_DATA *ch, OBJ_INDEX_DATA *obj ) );
void	show_chapter		args( ( CHAR_DATA *ch, QUEST_INDEX_DATA *quest,
					int nr ) );
void	adv_show_chapter	args( ( CHAR_DATA *ch, QUEST_INDEX_DATA *quest,
					int nr ) );
bool	is_builder			args( ( CHAR_DATA *ch, AREA_DATA *area ) );
bool	can_edit			args( ( CHAR_DATA *ch, int vnum ) );
AREA_DATA *get_vnum_area	args( ( int vnum ) );
AREA_DATA *get_vnum_area	args( ( int area_id ) );
void	show_help			args( ( CHAR_DATA *ch, char *argument ) );
void	edit_done			args( ( CHAR_DATA *ch, char *argument ) );

/* request.c */
void	init_request_pipe	args( ( void ) );
void	check_requests		args( ( void ) );

/* save.c */
void	save_char_obj	args( ( CHAR_DATA *ch ) );
void	save_clone		args( ( CHAR_DATA *ch ) );
bool	load_char_obj	args( ( DESCRIPTOR_DATA *d, char *name, bool preload ) );
void	set_alarm		args( ( long seconds ) );
void	requip_char		args( ( CHAR_DATA *ch ) );
void	fwrite_obj	args( ( CHAR_DATA *ch,	OBJ_DATA  *obj, FILE *fp,
							int iNest, int os_type ) );

void	fwrite_obj_raw	args( ( OBJ_DATA  *obj, FILE *fp,int iNest,
							int os_type ) );
void	save_pdata		args( ( CHAR_DATA *ch ) );

void	fread_obj		args( ( CHAR_DATA *ch,	FILE *fp, int os_type ) );
void	de_equip_char		args( ( CHAR_DATA *ch ) );
void	re_equip_char		args( ( CHAR_DATA *ch ) );
void	save_home		args( ( CHAR_DATA *ch ) );
void	load_home		args( ( CHAR_DATA *ch ) );
void	load_home_flags args( ( void ) );
void	load_storages	args( ( void ) );
void	save_room_storage args( (ROOM_INDEX_DATA *room) );
void	show_last		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
/* shops.c */

/* special.c */
SF *	spec_lookup		args( ( const char *name ) );
const char *	lookup_spec		args( ( SPEC_FUN *special ) );

/* tables.c */
int	get_skill		args( ( char *skilltype ) );
const char *	spell_name		args( ( SPELL_FUN *spell ) );
const char *	skill_name		args( ( DO_FUN *skill ) );
void	load_skill_table	args( ( void ) );
void	save_skill_table	args( ( void ) );
void	sort_skill_table	args( ( void ) );
void	load_socials		args( ( void ) );
void	save_socials		args( ( void ) );
void	load_commands		args( ( void ) );
void	save_commands		args( ( void ) );
SPELL_FUN *spell_function	args( ( char *name ) );
DO_FUN *skill_function		args( ( char *name ) );
void	load_herb_table		args( ( void ) );
void	save_herb_table		args( ( void ) );

/* track.c */
void	found_prey		args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void	hunt_victim		args( ( CHAR_DATA *ch) );

/* update.c */
#if defined( itoa )
#undef itoa
#endif
#define itoa			__itoa
char *	__itoa			args( ( int num ) );
void	clean_for_loop		args( ( void ) );
void	update_for		args( ( void ) );
void	clean_rat_loop		args( ( void ) );
void	update_rat		args( ( void ) );
void	advance_level		args( ( CHAR_DATA *ch , int ability ) );
void	gain_exp		args( ( CHAR_DATA *ch, int gain , int ability) );
void	gain_condition		args( ( CHAR_DATA *ch, int iCond, int value ) );
void	update_handler		args( ( void ) );
void	reboot_check		args( ( time_t reset ) );
#if 0
void	reboot_check		args( ( char *arg ) );
#endif
void	auction_update		args( ( void ) );
void	remove_portal		args( ( OBJ_DATA *portal ) );
int	max_level		args( ( CHAR_DATA *ch, int ability) );

/* hashstr.c */
char *	str_alloc		args( ( const char *str ) );
char *	quick_link		args( ( char *str ) );
int	str_free		args( ( char *str ) );
void	show_hash		args( ( int count ) );
char *	hash_stats		args( ( void ) );
char *	check_hash		args( ( char *str ) );
void	hash_dump		args( ( int hash ) );
void	show_high_hash		args( ( int top ) );

/* quest.c Thanos*/
void	quest_update		args ( ( void ) );
void	quest_cleanup		args ( ( CHAR_DATA *ch, bool end ) );
void	quest_trigger		args ( ( CHAR_DATA *ch, int cmd, OBJ_DATA *obj, CHAR_DATA *victim ) );
void	save_quest_data		args ( ( CHAR_DATA *ch ) );
void	load_quest_data		args ( ( CHAR_DATA *ch ) );
QUEST_MOB_DATA *qmob_invoke( int pIndex, QUEST_DATA *quest );
QUEST_OBJ_DATA *qobj_invoke( int pIndex, QUEST_DATA *quest, int level );

/* olc_quest.c Thanos*/
void	save_quest		args( ( QUEST_INDEX_DATA * pQuest ) );
void	write_quest_list	args( ( void ) );

/* swxml.c */
#include "swxml.h"

/* pazaak.cpp */
OD *pazaak_create_card_obj(const SWPazaakCard &);

/*
* mudprograms stuff
*/
extern	CHAR_DATA *supermob;

void oprog_speech_trigger		( char *txt, CHAR_DATA *ch );
void oprog_random_trigger		( OBJ_DATA *obj );
void oprog_wear_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
bool oprog_use_trigger			( CHAR_DATA *ch, OBJ_DATA *obj,
					CHAR_DATA *vict, OBJ_DATA *targ,
					void *vo );
void oprog_remove_trigger		( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_sac_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_damage_trigger		( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_repair_trigger		( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_drop_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_zap_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
char *oprog_type_to_name		( int type );

/*
* MUD_PROGS START HERE
* (object stuff)
*/
void oprog_greet_trigger		( CHAR_DATA *ch );
void oprog_speech_trigger		( char *txt, CHAR_DATA *ch );
void oprog_random_trigger		( OBJ_DATA *obj );
void oprog_random_trigger		( OBJ_DATA *obj );
void oprog_remove_trigger		( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_sac_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_get_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_damage_trigger		( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_repair_trigger		( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_drop_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_examine_trigger		( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_zap_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_pull_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );
void oprog_push_trigger			( CHAR_DATA *ch, OBJ_DATA *obj );

void rprog_leave_trigger		( CHAR_DATA *ch );
void rprog_enter_trigger		( CHAR_DATA *ch );
void rprog_sleep_trigger		( CHAR_DATA *ch );
void rprog_rest_trigger			( CHAR_DATA *ch );
void rprog_rfight_trigger		( CHAR_DATA *ch );
void rprog_death_trigger		( CHAR_DATA *killer, CHAR_DATA *ch );
void rprog_speech_trigger		( char *txt, CHAR_DATA *ch );
void rprog_random_trigger		( CHAR_DATA *ch );
void rprog_time_trigger			( CHAR_DATA *ch );
void rprog_hour_trigger			( CHAR_DATA *ch );
char *rprog_type_to_name		( int type );
void rprog_extradesc_trigger	( EXTRA_DESCR_DATA *, CHAR_DATA * );

#define OPROG_ACT_TRIGGER
#ifdef OPROG_ACT_TRIGGER
void oprog_act_trigger			( char *buf, OBJ_DATA *mobj,
					CHAR_DATA *ch, OBJ_DATA *obj, void *vo );
#endif
#define RPROG_ACT_TRIGGER
#ifdef RPROG_ACT_TRIGGER
void rprog_act_trigger			( char *buf, ROOM_INDEX_DATA *room,
					CHAR_DATA *ch, OBJ_DATA *obj, void *vo );
#endif

//int64 abs(int64);
//long int abs(long int);

int atoi(const char *) throw();
int atoi(char *);
int atoi(char *);

#endif /* FUN_DECLS_H_ */
