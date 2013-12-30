/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Ack 2.2 improvements copyright (C) 1994 by Stephen Dooley              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *       _/          _/_/_/     _/    _/     _/    ACK! MUD is modified    *
 *      _/_/        _/          _/  _/       _/    Merc2.0/2.1/2.2 code    *
 *     _/  _/      _/           _/_/         _/    (c)Stephen Zepp 1998    *
 *    _/_/_/_/      _/          _/  _/             Version #: 4.3          *
 *   _/      _/      _/_/_/     _/    _/     _/                            *
 *                                                                         *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/
#define DEC_GLOBALS_H 1

#ifndef DEC_TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef DEC_CONFIG_H
#include "config.h"
#endif

#ifndef DEC_ACK_H
#include "ack.h"
#endif

/*
 * These are skill_lookup return values for common skills and spells.
 */

#define descriptor_list first_desc

extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

extern CHAR_DATA * char_list;                               //for pload
extern BUILDING_DATA * building_list;
extern VEHICLE_DATA  * vehicle_list;
extern      int         MAX_BUILDING;

extern  bool            booting_up;
extern  bool      area_resetting_global;
extern char *  const   dir_name        [];
extern char *  const   rev_name        [];
extern int     const   order           [];
extern char *  const   helper          [MAX_HELPER];
extern int     const   prof_time       [];
extern char *  const   vehicle_name    [MAX_VEHICLE];
extern char *  const   vehicle_desc    [MAX_VEHICLE];

/*
 * Global constants.
 */
extern const struct color_type color_table[MAX_color];
extern const struct ansi_type ansi_table [MAX_ANSI];
extern const struct class_type class_table [MAX_CLASS];
extern struct map_type map_table;
extern struct alliance_type alliance_table[MAX_ALLIANCE];
extern struct score_type score_table [100];
extern struct rank_type rank_table [30];
extern const struct ranking_type ranking_table[];
extern const struct cmd_type cmd_table[];
extern const struct wildmap_type wildmap_table[SECT_MAX];
extern struct social_type *social_table;
extern const struct clip_type clip_table[MAX_AMMO];
extern const struct skill_type skill_table[];
extern const struct  bonus_type bonus_table[];
extern const struct formula_type formula_table[];
extern struct load_type load_list[MAX_BUILDING_LEVEL+1][50];
extern const struct s_res_type s_res_table[];
extern const struct planet_type planet_table[];
extern const struct ability_type ability_table[];
extern struct multiplay_type multiplay_table[30];

extern struct build_type        build_table[MAX_POSSIBLE_BUILDING];
extern struct build_help_type   build_help_table[MAX_POSSIBLE_BUILDING];
//extern const    struct  build_type		build_table[MAX_BUILDING];
//extern const    struct  build_help_type		build_help_table[MAX_BUILDING];

/* spec: log all calls to getmem/dispose when set */
extern bool mem_log;

/*
 * Global variables.
 */
extern          char                    bug_buf         [];
extern          time_t                  current_time;
extern          bool                    fLogAll;
extern          FILE *                  fpReserve;
extern          KILL_DATA               kill_table      [];
extern          char                    log_buf         [];
extern          char                    testerbuf       [];
extern          TIME_INFO_DATA          time_info;
//extern          WEATHER_DATA            weather_info[SECT_MAX];
extern          DESCRIPTOR_DATA   *     descriptor_list;
extern          CHAR_DATA         *     char_list;
extern          ROOM_INDEX_DATA   *     room_index_hash [ MAX_KEY_HASH ];
extern          OBJ_INDEX_DATA    *     obj_index_hash [ MAX_KEY_HASH ];
extern          SYS_DATA_TYPE            sysdata;
extern          BUILDING_DATA         *     building_list;
extern          VEHICLE_DATA          *     vehicle_list;
extern      CHAR_DATA *         map_ch[MAX_MAPS][MAX_MAPS][Z_MAX];
extern      BUILDING_DATA   *   map_bld[MAX_MAPS][MAX_MAPS][Z_MAX];
extern      VEHICLE_DATA    *   map_vhc[MAX_MAPS][MAX_MAPS][Z_MAX];
extern      OBJ_DATA    *   vehicle_weapon;
extern const    int         kill_groups[];
extern      int             quest_objs;
extern      OBJ_DATA    *   quest_obj[MAX_QUEST_ITEMS];
extern      long            building_count;

extern BOARD_DATA   *  first_board;
extern BOARD_DATA   *   last_board;
extern BOARD_DATA   *   board_free;
extern MESSAGE_DATA *   message_free;
extern QUEUE_DATA * queue_free;
extern BOMB_DATA *  bomb_free;
extern PAGER_DATA * pager_free;

extern          DISABLED_DATA     *     disabled_first;     /* interp.c */

extern char *history1;
extern char *history2;
extern char *history3;
extern char *history4;
extern char *history5;
extern char *history6;
extern char *history7;
extern char *history8;
extern char *history9;
extern char *history10;

/*
 * Command functions.
 * Defined in act_*.c (mostly).
 */

// MCCP
DECLARE_DO_FUN( do_compress );                              /* MCCP */
// End MCCP

DECLARE_DO_FUN( do_rename       );
DECLARE_DO_FUN( do_afk      );
DECLARE_DO_FUN( do_alias    );
DECLARE_DO_FUN( build_arealist  );
DECLARE_DO_FUN( do_ask          );
DECLARE_DO_FUN( do_bamfin       );
DECLARE_DO_FUN( do_bamfout      );
DECLARE_DO_FUN( do_beep     );
DECLARE_DO_FUN( do_colist   );
DECLARE_DO_FUN( do_color    );
DECLARE_DO_FUN( do_code     );
DECLARE_DO_FUN( do_creator  );
DECLARE_DO_FUN( do_edit     );
DECLARE_DO_FUN( do_enter    );
DECLARE_DO_FUN( do_finger   );
DECLARE_DO_FUN( do_flame        );
DECLARE_DO_FUN( do_gossip       );
DECLARE_DO_FUN( do_ooc          );
DECLARE_DO_FUN( do_politics     );
DECLARE_DO_FUN( do_game         );
DECLARE_DO_FUN( do_quest    );
DECLARE_DO_FUN( do_heal     );
DECLARE_DO_FUN( do_resetpassword);
DECLARE_DO_FUN( do_iscore       );
DECLARE_DO_FUN( do_isnoop       );
DECLARE_DO_FUN( do_iwhere       );
DECLARE_DO_FUN( do_monitor  );
DECLARE_DO_FUN( do_music        );
DECLARE_DO_FUN( do_newbie       );
DECLARE_DO_FUN( do_atalk        );
DECLARE_DO_FUN( do_nopray       );
DECLARE_DO_FUN( do_pemote       );
DECLARE_DO_FUN( do_pray         );
DECLARE_DO_FUN( do_respond      );
DECLARE_DO_FUN( do_scan         );
DECLARE_DO_FUN( do_status       );
DECLARE_DO_FUN( do_togbuild     );
DECLARE_DO_FUN( do_whisper      );
DECLARE_DO_FUN( do_whoname  );
DECLARE_DO_FUN( do_ranking  );
DECLARE_DO_FUN( do_disable  );
DECLARE_DO_FUN( game_interpret );
DECLARE_DO_FUN( build_interpret );
DECLARE_DO_FUN( hack_interpret  );
DECLARE_DO_FUN( engineering_interpret  );
DECLARE_DO_FUN( space_interpret );
DECLARE_DO_FUN( do_build        );
DECLARE_DO_FUN( do_delete       );
DECLARE_DO_FUN( do_read         );
DECLARE_DO_FUN( do_savearea     );
DECLARE_DO_FUN( do_write        );
DECLARE_DO_FUN( do_check_areas  );
DECLARE_DO_FUN( do_check_area   );
DECLARE_DO_FUN( do_ofindlev     );
DECLARE_DO_FUN( do_olist    );
DECLARE_DO_FUN( do_answer       );
DECLARE_DO_FUN( do_at           );
DECLARE_DO_FUN( do_auto         );
DECLARE_DO_FUN( do_pubmail  );
DECLARE_DO_FUN( do_sound    );
DECLARE_DO_FUN( do_ban          );
DECLARE_DO_FUN( do_blank        );
DECLARE_DO_FUN( do_bug          );
DECLARE_DO_FUN( do_channels     );
DECLARE_DO_FUN( do_combine      );
DECLARE_DO_FUN( do_commands     );
DECLARE_DO_FUN( do_config       );
DECLARE_DO_FUN( do_credits      );
DECLARE_DO_FUN( do_deny         );
DECLARE_DO_FUN( do_disarm       );
DECLARE_DO_FUN( do_disconnect   );
DECLARE_DO_FUN( do_drop         );
DECLARE_DO_FUN( do_east         );
DECLARE_DO_FUN( do_echo         );
DECLARE_DO_FUN( do_emote        );
DECLARE_DO_FUN( do_equipment    );
DECLARE_DO_FUN( do_examine      );
DECLARE_DO_FUN( do_follow       );
DECLARE_DO_FUN( do_force        );
DECLARE_DO_FUN( do_freeze       );
DECLARE_DO_FUN( do_get          );
DECLARE_DO_FUN( do_give         );
DECLARE_DO_FUN( do_goto         );
DECLARE_DO_FUN( do_help         );
DECLARE_DO_FUN( do_helplist     );
DECLARE_DO_FUN( do_holylight    );
DECLARE_DO_FUN( do_idea         );
DECLARE_DO_FUN( do_immtalk      );
DECLARE_DO_FUN( do_incog    );
DECLARE_DO_FUN( do_inventory    );
DECLARE_DO_FUN( do_invis        );
DECLARE_DO_FUN( do_log          );
DECLARE_DO_FUN( do_look         );
DECLARE_DO_FUN( do_memory       );
DECLARE_DO_FUN( do_noemote      );
DECLARE_DO_FUN( do_north        );
DECLARE_DO_FUN( do_note         );
DECLARE_DO_FUN( do_notell       );
DECLARE_DO_FUN( do_ofind        );
DECLARE_DO_FUN( do_oload        );
DECLARE_DO_FUN( do_oset         );
DECLARE_DO_FUN( do_ostat        );
DECLARE_DO_FUN( do_owhere       );
DECLARE_DO_FUN( do_pagelen      );
DECLARE_DO_FUN( do_password     );
DECLARE_DO_FUN( do_prompt       );
DECLARE_DO_FUN( do_purge        );
DECLARE_DO_FUN( do_question     );
DECLARE_DO_FUN( do_qui          );
DECLARE_DO_FUN( do_quote	);
DECLARE_DO_FUN( do_quit         );
DECLARE_DO_FUN( do_reboo        );
DECLARE_DO_FUN( do_reboot       );
DECLARE_DO_FUN( do_remove       );
DECLARE_DO_FUN( do_reply        );
DECLARE_DO_FUN( do_rest         );
DECLARE_DO_FUN( do_restore      );
DECLARE_DO_FUN( do_sacrifice    );
DECLARE_DO_FUN( do_save         );
DECLARE_DO_FUN( do_say          );
DECLARE_DO_FUN( do_osay     );
DECLARE_DO_FUN( do_score        );
DECLARE_DO_FUN( do_shutdow      );
DECLARE_DO_FUN( do_shutdown     );
DECLARE_DO_FUN( do_silence      );
DECLARE_DO_FUN( do_sla          );
DECLARE_DO_FUN( do_slay         );
DECLARE_DO_FUN( do_sleep        );
DECLARE_DO_FUN( do_snoop        );
DECLARE_DO_FUN( do_socials      );
DECLARE_DO_FUN( do_south        );
DECLARE_DO_FUN( do_stand        );
DECLARE_DO_FUN( do_tag      );
DECLARE_DO_FUN( do_tell         );
DECLARE_DO_FUN( do_time         );
DECLARE_DO_FUN( do_title        );
DECLARE_DO_FUN( do_transfer     );
DECLARE_DO_FUN( do_trust        );
DECLARE_DO_FUN( do_typo         );
DECLARE_DO_FUN( do_users        );
DECLARE_DO_FUN( do_wake         );
DECLARE_DO_FUN( do_wear         );
DECLARE_DO_FUN( do_west         );
DECLARE_DO_FUN( do_who          );
DECLARE_DO_FUN( do_wizhelp      );
DECLARE_DO_FUN( do_wizify       );
DECLARE_DO_FUN( do_wizlist      );
DECLARE_DO_FUN( do_wizlock      );
DECLARE_DO_FUN( do_yell         );
DECLARE_DO_FUN( do_otype );
DECLARE_DO_FUN( do_owear );
DECLARE_DO_FUN( do_ignore       );
DECLARE_DO_FUN( do_for      );
DECLARE_DO_FUN( do_hotreboo   );
DECLARE_DO_FUN( do_hotreboot      );
DECLARE_DO_FUN( do_gitpull	);
DECLARE_DO_FUN( do_olmsg    );
DECLARE_DO_FUN( do_scheck   );
DECLARE_DO_FUN( do_immbrand );
DECLARE_DO_FUN( do_sysdata );
DECLARE_DO_FUN( do_areasave );
DECLARE_DO_FUN( do_mapper );
DECLARE_DO_FUN( do_email );
DECLARE_DO_FUN( do_oflags );
DECLARE_DO_FUN( do_mine );
DECLARE_DO_FUN( do_use );
DECLARE_DO_FUN( do_setwcode  );
DECLARE_DO_FUN( do_a_build       );
DECLARE_DO_FUN( do_listbuildings       );
DECLARE_DO_FUN( do_killbuildin       );
DECLARE_DO_FUN( do_killbuilding       );
DECLARE_DO_FUN( do_bset       );
DECLARE_DO_FUN( do_relevel       );
DECLARE_DO_FUN( do_setrelevel       );
DECLARE_DO_FUN( do_load       );
DECLARE_DO_FUN( do_set         );
DECLARE_DO_FUN( do_arm         );
DECLARE_DO_FUN( do_highscores         );
DECLARE_DO_FUN( do_highranks );
DECLARE_DO_FUN( do_upgrade );
DECLARE_DO_FUN( do_makeexit );
DECLARE_DO_FUN( do_blast);
DECLARE_DO_FUN( do_warp );
DECLARE_DO_FUN( do_throw );
DECLARE_DO_FUN( do_activate );
DECLARE_DO_FUN( do_vload );
DECLARE_DO_FUN( do_exit );
DECLARE_DO_FUN( do_demolish );
DECLARE_DO_FUN( do_map );
DECLARE_DO_FUN( do_securit );
DECLARE_DO_FUN( do_security );
DECLARE_DO_FUN( do_demolis );
DECLARE_DO_FUN( do_repop );
DECLARE_DO_FUN( do_closeexit );
DECLARE_DO_FUN( do_history );
DECLARE_DO_FUN( do_chunk );
DECLARE_DO_FUN( do_split );
DECLARE_DO_FUN( do_pit );
DECLARE_DO_FUN( do_install );
DECLARE_DO_FUN( do_winstall );
DECLARE_DO_FUN( do_sdelete );
DECLARE_DO_FUN( do_return );
DECLARE_DO_FUN( do_smite );
DECLARE_DO_FUN( do_reward );
DECLARE_DO_FUN( do_recho );
DECLARE_DO_FUN( do_shoot );
DECLARE_DO_FUN( do_mmake );
DECLARE_DO_FUN( do_identify );
DECLARE_DO_FUN( do_backup );
DECLARE_DO_FUN( do_bmake );
DECLARE_DO_FUN( do_objclear );
DECLARE_DO_FUN( do_swap );
DECLARE_DO_FUN( do_teleport );
DECLARE_DO_FUN( do_talktodesc );
DECLARE_DO_FUN( do_allow );
DECLARE_DO_FUN( do_history );
DECLARE_DO_FUN( do_test );
DECLARE_DO_FUN( do_deletefromscores );
DECLARE_DO_FUN( do_stats );
DECLARE_DO_FUN( do_implant );
DECLARE_DO_FUN( do_mset );
DECLARE_DO_FUN( do_darts );
DECLARE_DO_FUN( do_buildingreimburse );
DECLARE_DO_FUN( do_mstat );
DECLARE_DO_FUN( do_qpspend );
DECLARE_DO_FUN( do_oarmortype );
DECLARE_DO_FUN( do_sedit );
DECLARE_DO_FUN( do_doom );
DECLARE_DO_FUN( do_generate );
DECLARE_DO_FUN( do_research );
DECLARE_DO_FUN( do_sneak );
DECLARE_DO_FUN( do_repair );
DECLARE_DO_FUN( do_destroy );
DECLARE_DO_FUN( do_refine );
DECLARE_DO_FUN( do_sell );
DECLARE_DO_FUN( do_skills );
DECLARE_DO_FUN( do_chemistry );
DECLARE_DO_FUN( do_dig );
DECLARE_DO_FUN( do_target );
DECLARE_DO_FUN( do_accept );
DECLARE_DO_FUN( do_aban );
DECLARE_DO_FUN( do_alliances );
DECLARE_DO_FUN( do_leave );
DECLARE_DO_FUN( do_setalliance );
DECLARE_DO_FUN( do_amem );
DECLARE_DO_FUN( do_practice );
DECLARE_DO_FUN( do_kick );
DECLARE_DO_FUN( do_punch );
DECLARE_DO_FUN( do_bload );
DECLARE_DO_FUN( do_findalts );
DECLARE_DO_FUN( do_tunnel );
DECLARE_DO_FUN( do_home );
DECLARE_DO_FUN( do_move );
DECLARE_DO_FUN( do_connect );
DECLARE_DO_FUN( do_computer );
DECLARE_DO_FUN( do_classes );
DECLARE_DO_FUN( do_status );
DECLARE_DO_FUN( do_trade );
DECLARE_DO_FUN( do_boom );
DECLARE_DO_FUN( do_awhere );
DECLARE_DO_FUN( do_track );
DECLARE_DO_FUN( do_exresearch );
DECLARE_DO_FUN( do_paintball );
DECLARE_DO_FUN( do_where );
DECLARE_DO_FUN( do_createalliance );
DECLARE_DO_FUN( do_setowner );
DECLARE_DO_FUN( do_message );
DECLARE_DO_FUN( do_spy );
DECLARE_DO_FUN( do_radiosilence );
DECLARE_DO_FUN( do_torment );
DECLARE_DO_FUN( do_locate );
DECLARE_DO_FUN( do_update_website );
DECLARE_DO_FUN( do_paradrop );
DECLARE_DO_FUN( do_savemap );
DECLARE_DO_FUN( do_construct );
DECLARE_DO_FUN( do_reset );
DECLARE_DO_FUN( do_asshole );
DECLARE_DO_FUN( do_clean );
DECLARE_DO_FUN( do_sblast );
DECLARE_DO_FUN( do_psy_message );
DECLARE_DO_FUN( do_nuke );
DECLARE_DO_FUN( do_loadlist );
DECLARE_DO_FUN( do_sresearch );
DECLARE_DO_FUN( construct_space_vessal );
DECLARE_DO_FUN( construct_alien_vessal );
DECLARE_DO_FUN( construct_sailing_vessal );
DECLARE_DO_FUN( do_lift );
DECLARE_DO_FUN( do_land );
DECLARE_DO_FUN( space_warp );
DECLARE_DO_FUN( space_mine );
DECLARE_DO_FUN( do_info );
DECLARE_DO_FUN( do_teleport_b );
DECLARE_DO_FUN( do_mimic );
DECLARE_DO_FUN( do_deathray );
DECLARE_DO_FUN( do_backup_building );
DECLARE_DO_FUN( do_mspend );
DECLARE_DO_FUN( do_formulas );
DECLARE_DO_FUN( do_meda );
DECLARE_DO_FUN( do_medal );
DECLARE_DO_FUN( do_search );
DECLARE_DO_FUN( do_loadfake );
DECLARE_DO_FUN( do_vinstall );
DECLARE_DO_FUN( do_battlestations );
DECLARE_DO_FUN( do_stop );
DECLARE_DO_FUN( do_queue );
DECLARE_DO_FUN( do_spacepop );
DECLARE_DO_FUN( do_run );
DECLARE_DO_FUN( do_proficiencies );
DECLARE_DO_FUN( do_fix );
DECLARE_DO_FUN( do_engineer );
DECLARE_DO_FUN( do_space_move );
DECLARE_DO_FUN( do_space_hijack );
DECLARE_DO_FUN( do_paintlock );
DECLARE_DO_FUN( do_nukem );
DECLARE_DO_FUN( do_cloneflag );
DECLARE_DO_FUN( do_shell );
DECLARE_DO_FUN( do_blindupdate );
DECLARE_DO_FUN( do_prize );
DECLARE_DO_FUN( do_donate );
DECLARE_DO_FUN( do_oresearch );
DECLARE_DO_FUN( do_multiplayers );
DECLARE_DO_FUN( do_peek );
DECLARE_DO_FUN( do_use );
DECLARE_DO_FUN( do_setexit );
DECLARE_DO_FUN( do_mute );
DECLARE_DO_FUN( do_coords );
DECLARE_DO_FUN( do_settunnel );
DECLARE_DO_FUN( do_buildings);
DECLARE_DO_FUN( do_ammo );
DECLARE_DO_FUN( do_vehicle_status );
DECLARE_DO_FUN( do_peace );
DECLARE_DO_FUN( do_list );
DECLARE_DO_FUN( do_qpmode );
DECLARE_DO_FUN( do_train );
DECLARE_DO_FUN( do_xpreward );
DECLARE_DO_FUN( do_xpmode );
DECLARE_DO_FUN( do_gunner_shoot );
DECLARE_DO_FUN( do_scanmap );
DECLARE_DO_FUN( do_owhereflag );
DECLARE_DO_FUN( do_gpreward );
DECLARE_DO_FUN( do_bye          );
DECLARE_DO_FUN( do_pager );
DECLARE_DO_FUN( do_bscan );
DECLARE_DO_FUN( do_bthere );
DECLARE_DO_FUN( do_imminfo );
DECLARE_DO_FUN( do_changes );
DECLARE_DO_FUN( do_logs );
DECLARE_DO_FUN( do_addchange            );
DECLARE_DO_FUN( do_chedit               );
DECLARE_DO_FUN( do_immlog );
DECLARE_DO_FUN( do_phase );
DECLARE_DO_FUN( do_survey );
DECLARE_DO_FUN( do_vset );
DECLARE_DO_FUN( do_ammolist );
DECLARE_DO_FUN( do_bomb );
DECLARE_DO_FUN( do_devastate );
DECLARE_DO_FUN( do_rules );
DECLARE_DO_FUN( do_ranks );
DECLARE_DO_FUN( do_rangen );
DECLARE_DO_FUN( do_hurl );
