/*~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-
 ~  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        ~
 ~  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   ~
 ~                                                                         ~
 ~  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          ~
 ~  Chastain, Michael Quan, and Mitchell Tse.                              ~
 ~                                                                         ~
 ~  Ack 2.2 improvements copyright (C) 1994 by Stephen Dooley              ~
 ~  ACK!MUD is modified Merc2.0/2.1/2.2 code (c)Stephen Zepp 1998 Ver: 4.3 ~
 ~                                                                         ~
 ~  In order to use any part of this  PA  Diku Mud, you must comply with   ~
 ~  both the original Diku license in 'license.doc' as well the Merc       ~
 ~  license in 'license.txt', and the Ack!Mud license in 'ack_license.txt'.~
 ~  In particular, you may not remove any of these copyright notices.      ~
 ~                                                                         ~
 ~           _______      _____                                            ~
 ~          /  __  /\    / ___ \       222222        PA_MUD by Amnon Kruvi ~
 ~         /______/ /   / /___\ \            2       PA_MUD is modified    ~
 ~        / _______/   / _______ \           2       Ack!Mud, v4.3         ~
 ~       /_/          /_/       \_\        2                               ~
 ~                                      2                                  ~
 ~                                     2222222                             ~
 ~                                                                         ~
 ~                                                                         ~
 ~   Years of work have been invested to create DIKU, Merc, Ack and PA.    ~
 ~   Please show your respect by following the licenses, and issuing       ~
 ~   credits where due.                                                    ~
 ~                                                                         ~
 ~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-*/

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>                                         /* unlink() */
#include "ack.h"

bool    check_social    args( ( CHAR_DATA *ch, char *command,
char *argument ) );

/* Disabled stuff - Wyn */
bool    check_disabled  args ( ( CHAR_DATA *ch, const struct cmd_type *command ) );

DISABLED_DATA *disabled_first;

#define END_MARKER  "END"                                   /* for load_disabled() and save_disabled() */

/*
 * Command logging types.
 */
#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2

/*
 * Log-all switch.
 */
bool                            fLogAll         = FALSE;

/*
 * Command table.
 */
const   struct  cmd_type        cmd_table       [] =
{

    {
        "north",          do_north,       POS_SNEAKING,    0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "east",           do_east,        POS_SNEAKING,    0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "south",          do_south,       POS_SNEAKING,    0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "west",           do_west,        POS_SNEAKING,    0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "look",           do_look,        POS_RESTING,     0,  LOG_NORMAL,        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "changes",        do_changes,     POS_DEAD,        0,  LOG_NORMAL,        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "inventory",      do_inventory,   POS_DEAD,        0,  LOG_NORMAL,        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "score",          do_score,       POS_DEAD,        0,  LOG_NORMAL,        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "tell",           do_tell,        POS_RESTING,     0,  LOG_NORMAL,        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "stop",           do_stop,        POS_DEAD,        0,  LOG_NORMAL,        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "help",           do_help,        POS_DEAD,        0,  LOG_NORMAL,        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        ")",              do_creator, 	  POS_DEAD,    	  85,  LOG_NORMAL,        C_TYPE_COMM, C_SHOW_ALWAYS
    },

//    { ".",              do_gossip,      POS_RESTING,     0,  LOG_NORMAL,/      C_TYPE_COMM, C_SHOW_ALWAYS },

    {
        ",",              do_emote,       POS_RESTING,     0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "'",              do_say,         POS_RESTING,     0,  LOG_NORMAL,        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "accept",         do_accept,      POS_STANDING,    0,  LOG_NORMAL,        C_TYPE_ALLI, C_SHOW_ALWAYS
    },
    {
        "activate",       do_activate,    POS_STANDING,    0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "afk",            do_afk,         POS_DEAD,        0,  LOG_NORMAL,        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "aim",            do_target,      POS_SNEAKING,    0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_NEVER
    },
    {
        "alias",          do_alias,       POS_DEAD,        0,  LOG_NORMAL,        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "alliances",      do_alliances,   POS_STANDING,    0,  LOG_NORMAL,        C_TYPE_ALLI, C_SHOW_ALWAYS
    },
    {
        "altalk",         do_atalk,       POS_RESTING,     0,  LOG_NORMAL,        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "amem",           do_amem,        POS_STANDING,    0,  LOG_NORMAL,        C_TYPE_ALLI, C_SHOW_ALWAYS
    },
    {
        "ammo",           do_ammo,        POS_STANDING,    0,  LOG_NORMAL,        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "arm",            do_arm,         POS_SNEAKING,    0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "auto",           do_auto,        POS_DEAD,        0,  LOG_NORMAL,        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "awhere",         do_awhere,      POS_STANDING,    0,  LOG_NORMAL,        C_TYPE_ALLI, C_SHOW_ALWAYS
    },
    {
        "ban",            do_aban,        POS_STANDING,    0,  LOG_NORMAL,        C_TYPE_ALLI, C_SHOW_ALWAYS
    },
    {
        "battlestations", do_battlestations,POS_STANDING,  0,  LOG_NORMAL,        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "beep",           do_beep,        POS_RESTING,     0,  LOG_NORMAL,        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "blast",      do_blast,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "blindupdate",    do_blindupdate, POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_NEVER
    },
    {
        "boom",       do_boom,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "bscan",           do_bscan,        POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "build",          do_a_build,       POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "buildings",      do_buildings,       POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_NEVER
    },

    {
        "channels",       do_channels,    POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "chat",       do_ooc,   POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "chemistry",        do_chemistry,     POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_SKILL
    },
    {
        "chunk",      do_chunk,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "classes",        do_classes,   POS_DEAD,    0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "clean",      do_clean,     POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "closeexit",    do_closeexit,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "cloneflag",    do_cloneflag,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "code",       do_code,    POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "coords",     do_coords,      POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "colist",   do_colist,  POS_DEAD,   0,  LOG_NORMAL, C_TYPE_CONFIG,
        C_SHOW_ALWAYS
    },
    {
        "color",         do_color,      POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "combine",        do_combine,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "commands",       do_commands,    POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "computer",   do_computer,    POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "compress",   do_compress,    POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "complete",       do_a_build,     POS_STANDING,  0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_NEVER
    },
    {
        "config",         do_config,      POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "connect",        do_connect,     POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "construct",      do_construct,   POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "createalliance",  do_createalliance, POS_DEAD,    0,  LOG_NORMAL,
        C_TYPE_ALLI, C_SHOW_ALWAYS
    },
    {
        "creator",  do_creator, POS_DEAD,    85,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "credits",        do_credits,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },

    {
        "darts",            do_darts,       POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "delete",         do_delete,      POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "demolis",    do_demolis,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_NEVER
    },
    {
        "demolish",    do_demolish,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "destroy",         do_destroy,     POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "dig",              do_dig,           POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_SKILL
    },
    {
        "disarm",         do_disarm,      POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "donate",     do_donate,  POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "doom",       do_doom,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "drink",          do_heal,         POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_NEVER
    },
    {
        "drop",           do_drop,        POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },

    {
        "edit",           do_edit,        POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "eject",          do_exit,       POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_NEVER
    },
    {
        "email",   do_email,  POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "emote",          do_emote,       POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "enter",            do_enter,       POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "engineer",         do_engineer,    POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "equipment",      do_equipment,   POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "examine",        do_examine,     POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "exit",            do_exit,       POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "exresearch", do_exresearch,  POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },

    {
        "finger",   do_finger,  POS_DEAD,  0, LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "fire",       do_shoot,   POS_SNEAKING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "fix",        do_fix,     POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "flame",          do_flame,       POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "follow",         do_follow,      POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "formulas",       do_formulas,    POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },

    {
        "get",            do_get,         POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "game",       do_game,   POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "generate",   do_generate,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "give",           do_give,        POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "gossip",     do_gossip,  POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },

    {
        "heal",           do_heal,         POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "helplist",       do_helplist,        POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "highranks",   do_highranks,  POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "highscores",   do_highscores,  POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "hijack",     do_space_hijack,  POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "history",        do_history,     POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "hold",           do_wear,        POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },

    {
        "ignore",         do_ignore,      POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "info",           do_info,        POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "install",        do_install,     POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "identify",       do_identify,    POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "implant",        do_implant,     POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },

    {
        "junk",      do_sacrifice,   POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },

    //    { "kick",             do_kick,          POS_STANDING,    0,  LOG_NORMAL,
    //      C_TYPE_ACTION, C_SHOW_SKILL },

    {
        "land",       do_land,    POS_STANDING,  0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "leave",      do_leave,   POS_STANDING,  0,  LOG_NORMAL,
        C_TYPE_ALLI, C_SHOW_ALWAYS
    },
    {
        "lift",       do_lift,    POS_STANDING,  0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "list",       do_list,    POS_STANDING,  0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "load",       do_load,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "log",         do_log,     POS_DEAD,        0,     LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    
    { 
        "logs",          do_logs,     POS_DEAD,        0,     LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "makeexit",    do_makeexit,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "map",       do_map,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "meda",       do_meda,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION,   C_SHOW_NEVER
    },
    {
        "medal",      do_medal,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION,   C_SHOW_ALWAYS
    },
    {
        "message",    do_psy_message,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "mimic",      do_mimic,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "mine",       do_mine,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "move",           do_space_move,  POS_STANDING,    0,   LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "mspend",     do_mspend,  POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "music",          do_music,       POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "mute",           do_mute,        POS_DEAD,     0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_NEVER
    },

    {
        "newbie",         do_newbie,      POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "nukem",          do_nukem,       POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },

    {
        "ooc",    do_ooc,   POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "osay",       do_osay,    POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },

    {
        "pagelength",     do_pagelen,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "pager",      do_pager,       POS_STANDING,   90,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "paintball",      do_paintball,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "paradrop",       do_paradrop,    POS_STANDING,    0,  LOG_NORMAL ,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "password",       do_password,    POS_DEAD,        0,  LOG_NEVER ,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "pemote",         do_pemote,      POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "pit",            do_pit,         POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "politics",       do_politics,   POS_RESTING,  0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "practice",     do_practice,  POS_STANDING,  0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "proficiencies",  do_proficiencies, POS_STANDING,  0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "prompt",         do_prompt,      POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "pubmail",        do_pubmail,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO,    C_SHOW_ALWAYS
    },
    //    { "punch",		do_punch,	POS_STANDING,	 0,  LOG_NORMAL,
    //     C_TYPE_ACTION,    C_SHOW_ALWAYS},

    {
        "quest",          do_quest,       POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_NEVER
    },
    {
        "qui",            do_qui,         POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_NEVER
    },

    { 
	"quote",          do_quote,       POS_DEAD,        0,  LOG_NORMAL,
	C_TYPE_MISC, C_SHOW_ALWAYS 
    },
    
    {
	"quotes",	  do_quote,	  POS_DEAD,	   0,  LOG_NORMAL,
	C_TYPE_MISC, C_SHOW_ALWAYS
    },

    {
        "quit",           do_quit,        POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },

    {
        "qpspend",        do_qpspend,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },

    {
        "reply",          do_reply,       POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "radiosilence",   do_radiosilence,POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "read",           do_read,        POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "refine",         do_refine,      POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "reload",     do_load,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "relevel",     do_relevel,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_NEVER
    },
    {
        "remove",         do_remove,      POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "repair",           do_repair,        POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_SKILL
    },
    {
        "reset",          do_reset,       POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "research",         do_research,      POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_SKILL
    },
        {
        "rules",         do_rules,     POS_DEAD,        0,     LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "run",            do_run,         POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },

    {
        "say",            do_say,         POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "save",           do_save,        POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_ALWAYS
    },
    {
        "scan",       do_scan,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "tscan",  do_scanmap, POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_NEVER
    },
    {
        "securit",        do_securit,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_NEVER
    },
    {
        "security",       do_security,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "sell",           do_sell,        POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "set",        do_set,     POS_SNEAKING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "setrelevel",        do_setrelevel,     POS_SNEAKING,    90,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_NEVER
    },
    {
        "setexit",    do_setexit, POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "setowner",   do_setowner,    POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ALLI, C_SHOW_ALWAYS
    },
    {
        "settunnel",  do_settunnel,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "shoot",      do_shoot,   POS_SNEAKING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "skills",         do_skills,      POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "sneak",            do_sneak,         POS_SNEAKING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_SKILL
    },
    {
        "socials",        do_socials,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "sound",      do_sound,   POS_DEAD,    0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "spy",            do_spy,         POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "stats",      do_stats,       POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "status",         do_status,      POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "swap",           do_swap,        POS_SNEAKING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },

    {
        "take",           do_get,         POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "target",           do_target,        POS_SNEAKING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_SKILL
    },
    {
        "teleport",    do_teleport,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "throw",          do_throw,       POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "time",           do_time,        POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "title",          do_title,       POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_CONFIG, C_SHOW_ALWAYS
    },
    {
        "ranks",          do_ranks,       POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO , C_SHOW_ALWAYS
    },
    {
        "torment",        do_torment,     POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "train",          do_train,       POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "trace",          do_track,       POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "track",          do_track,       POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "trade",          do_trade,       POS_STANDING,        0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "tunnel",     do_tunnel,  POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "upgrade",    do_upgrade,   POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "use",        do_use,       POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },

    {
        "vinstall",       do_vinstall,    POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "vr",         do_vehicle_status,    POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_MISC, C_SHOW_NEVER
    },

    {
        "warp",           do_warp,        POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "wear",           do_wear,        POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "where",          do_where,       POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "whisper",        do_whisper,     POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_COMM, C_SHOW_ALWAYS
    },
    {
        "who",            do_who,         POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "whois",          do_finger,       POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "wield",          do_wear,        POS_RESTING,     0,  LOG_NORMAL,
        C_TYPE_OBJECT, C_SHOW_ALWAYS
    },
    {
        "winstall",       do_winstall,    POS_STANDING,     0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "wizhelp",        do_wizhelp,     POS_DEAD,        81,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "wizlist",        do_wizlist,     POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_INFO, C_SHOW_ALWAYS
    },
    {
        "write",          do_write,       POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },

    /*
     * Immortal commands.
     */

    {
        ":",              do_immtalk,     POS_DEAD,    80,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "abuild",    do_build,       POS_STANDING,    80,       LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "alist",    build_arealist, POS_DEAD,        83,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    { "addchange",      do_addchange,   POS_DEAD,        85,      LOG_NORMAL,      C_TYPE_IMM, C_SHOW_ALWAYS},
    {
        "allow",            do_allow,         POS_DEAD,   85,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "areasave",        do_areasave,     POS_DEAD,    80,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "asshole",         do_asshole,      POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "at",             do_at,          POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    //    { "backup",         do_backup,     	POS_DEAD,    85,  LOG_NORMAL,
    //      C_TYPE_IMM, C_SHOW_ALWAYS},
    {
        "backup",         do_backup_building,     POS_STANDING,    0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_ALWAYS
    },
    {
        "bamfin",         do_bamfin,      POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "bamfout",        do_bamfout,     POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "blist",      do_listbuildings,  POS_DEAD,       83,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "bload",          do_bload,       POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "bmake",          do_bmake,   POS_STANDING, 85, LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "bmove",           do_move,        POS_DEAD,    85,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    
    {
        "bomb",    do_bomb,       POS_STANDING,    90,       LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },

    {
        "bset",           do_bset,        POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    { "chedit",         do_chedit,      POS_DEAD,        85,      LOG_NORMAL,      C_TYPE_IMM, C_SHOW_ALWAYS},
        {
        "devastate",    do_devastate,       POS_STANDING,    90,       LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "delscores",  do_deletefromscores,POS_DEAD,   87,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "deny",           do_deny,        POS_DEAD,    85,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "disable", do_disable, POS_STANDING, 85, LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "disconnect",     do_disconnect,  POS_DEAD,    83,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "echo",           do_echo,        POS_DEAD,    81,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "findalts",       do_findalts,    POS_DEAD,    85,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "for",            do_for,         POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "force",          do_force,       POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "freeze",         do_freeze,      POS_DEAD,    85,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "goto",           do_goto,        POS_DEAD,    79,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "gpreward",       do_gpreward,    POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "holylight",      do_holylight,   POS_DEAD,    80,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "home",           do_home,        POS_DEAD,    85,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "hotreboo",       do_hotreboo,    POS_DEAD,    84,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_NEVER
    },
    {
        "hotreboot",  do_hotreboot,    POS_DEAD,       87,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "copyover",  do_hotreboot,    POS_DEAD,       87,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "iban",           do_ban,         POS_DEAD,   83,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "immtalk",        do_immtalk,     POS_DEAD,    80,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "imminfo",        do_imminfo,     POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "incog",          do_incog,       POS_DEAD,    80,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "invis",          do_invis,       POS_DEAD,    80,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "killbuildin",      do_killbuildin,  POS_DEAD,       85,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "killbuilding",      do_killbuilding,  POS_DEAD,       85,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "immbrand",       do_immbrand,    POS_DEAD,    83,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "iscore",         do_iscore,      POS_DEAD,    87,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "isnoop",         do_isnoop,      POS_DEAD,    87,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "iwhere",         do_iwhere,      POS_DEAD,    81,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "listbuildings",      do_listbuildings,  POS_DEAD,       84,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "loadlist",       do_loadlist,    POS_DEAD,    87,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "loadfake",       do_loadfake,    POS_DEAD,    85,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "memory",         do_memory,      POS_DEAD,    87,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "monitor",        do_monitor,     POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "mmake",          do_mmake,   POS_STANDING, 85, LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "mset",           do_mset,        POS_DEAD,    83,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "mstat",          do_mstat,       POS_DEAD,    83,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "multiplayers",   do_multiplayers, POS_DEAD,   90,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "noemote",        do_noemote,     POS_DEAD,    80,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "notell",         do_notell,      POS_DEAD,    80,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "nuk",            do_look,        POS_DEAD,    89,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_NEVER
    },
    {
        "nukep",          do_nuke,        POS_DEAD,    89,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "oarmortype",     do_oarmortype,  POS_DEAD,    80,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "objclear",       do_objclear,    POS_DEAD,    90,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "ofind",          do_ofind,       POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "oflags",     do_oflags,  POS_DEAD,    83,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "olist",          do_olist,       POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "ostat",          do_ostat,       POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "otype",          do_otype,       POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "owear",          do_owear,      POS_DEAD,       81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "peace",          do_peace,       POS_DEAD,       83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "peek",           do_peek,        POS_DEAD,       83,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
        {
        "phase",    do_phase,       POS_STANDING,    80,       LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },

    {
        "prize",          do_prize,      POS_DEAD,       81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "whoname",        do_whoname,     POS_DEAD,       83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "ranking",     do_ranking,     POS_DEAD,       83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "immlog",     do_immlog,  POS_DEAD,    85,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "rangen",          do_rangen,       POS_DEAD,    81,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "reboo",          do_reboo,       POS_DEAD,    87,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_NEVER
    },
    {
        "reboot",         do_reboot,      POS_DEAD,    87,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "repop",         do_repop,      POS_DEAD,    83,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "resetpasswd",  do_resetpassword,POS_DEAD,   85,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "reward",         do_reward,      POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "savemap",        do_savemap,     POS_DEAD,    85,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "search",         do_search,      POS_DEAD,    85,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_NEVER
    },
    {
        "setwcode",       do_setwcode,    POS_DEAD,    85,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "shell",          do_shell,       POS_DEAD,    90,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_NEVER
    },
    {
        "shutdow",        do_shutdow,     POS_DEAD,    87,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_NEVER
    },
    {
        "shutdown",       do_shutdown,    POS_DEAD,    87,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
        {
        "smite",          do_smite,      POS_DEAD,       83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
        {
        "survey",    do_survey,       POS_STANDING,    80,       LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },

    {
        "sysdata",        do_sysdata,     POS_DEAD,       85,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "talktodesc",     do_talktodesc,  POS_DEAD,    85,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_NEVER
    },
    {
        "togbuild",       do_togbuild,    POS_DEAD,    87,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "trust",          do_trust,       POS_DEAD,    90,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "users",          do_users,       POS_DEAD,    83,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
        {
        "vset",    do_vset,       POS_STANDING,    83,       LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },

    {
        "wizify",         do_wizify,      POS_DEAD,    90,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "wizlock",        do_wizlock,     POS_DEAD,    87,   LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "ofindlev",       do_ofindlev,    POS_DEAD,    83,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "olmsg",          do_olmsg,       POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "oload",          do_oload,       POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "oset",           do_oset,        POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "owhere",         do_owhere,      POS_DEAD,    83,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "owhereflag",     do_owhereflag,  POS_DEAD,    83,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "paintlock",      do_paintlock,   POS_DEAD,    81,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "pdelete",        do_sdelete,     POS_DEAD,    0,   LOG_NEVER,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "prename",  do_rename,            POS_DEAD,    85,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "purge",          do_purge,       POS_DEAD,    81,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "recho",          do_recho,       POS_DEAD,    81,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "restore",        do_restore,     POS_DEAD,    81,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "scheck",   do_scheck,  POS_DEAD,    85,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "sedit",          do_sedit,       POS_DEAD,    85,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "setalliance",    do_setalliance,   POS_DEAD,    85,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "silence",        do_silence,     POS_DEAD,    80,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "sla",            do_sla,         POS_DEAD,       85,  LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "slay",           do_slay,        POS_DEAD,    85,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "snoop",          do_snoop,       POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "spacepop",       do_spacepop,    POS_DEAD,    85,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "test",           do_test,        POS_DEAD,    90,   LOG_NORMAL,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "transfer",       do_transfer,    POS_DEAD,    81,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "vload",          do_vload,       POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "qpmode",         do_qpmode,      POS_DEAD,    85,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "queue",          do_queue,       POS_DEAD,    90,  LOG_NEVER,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "xpmode",         do_xpmode,      POS_DEAD,    85,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },
    {
        "xpreward",       do_xpreward,    POS_DEAD,    83,  LOG_ALWAYS,
        C_TYPE_IMM, C_SHOW_ALWAYS
    },

    //    { "buildingreimburse",           do_buildingreimburse,        POS_DEAD,    85,   LOG_NORMAL,
    //      C_TYPE_IMM, C_SHOW_ALWAYS},

    /*
     * End of list.
     */
    {
        "",               0,              POS_DEAD,        0,  LOG_NORMAL,
        C_TYPE_ACTION, C_SHOW_NEVER
    }

};

/* Log all commands.. rewrite every 5 mins.. */
void comlog( CHAR_DATA *ch, int cmd, char *args )
{
    static FILE *fplog;
    static time_t ltime;

    if ( !fplog || ltime+(5*60) <= current_time )
    {
        if ( fplog )
            fclose(fplog);
        if ( !(fplog = fopen("../log/comlog.txt", "w")) )
            return;
        ltime = current_time;
    }
    fprintf(fplog, "%.24s :: %12.12s: %s %s\n", ctime(&current_time),
        ch->name, cmd_table[cmd].name,
        (cmd_table[cmd].log == LOG_NEVER ? "XXX" : args));
    fflush(fplog);
}

/*
 * The main entry point for executing commands.
 * Can be recursively called from 'at', 'order', 'force'.
 */
void interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    int trust;
    bool found;

    if ( ch->position == POS_WRITING )
        /* if player is writing, pass argument straight to write_interpret */
    {
        write_interpret( ch, argument );
        return;
    }

    if ( ch->position == POS_BUILDING )
    {
        if (argument[0]==':')
            argument++;
        else
        {
            build_interpret(ch,argument);
            return;
        }
    }
    else if ( ch->position == POS_HACKING )
    {
        hack_interpret(ch,argument);
        return;
    }
    else if ( ch->position == POS_SPACE_COM )
    {
        if (argument[0]==':')
            argument++;
        else
        {
            space_interpret(ch,argument);
            return;
        }
    }
    else if ( ch->position == POS_ENGINEERING )
    {
        if (argument[0]==':')
            argument++;
        else
        {
            engineering_interpret(ch,argument);
            return;
        }
    }
    else if ( ch->position == POS_NUKEM || NUKEM(ch) )
    {
        {
            if ( ch->position != POS_NUKEM )
                ch->position = POS_NUKEM;
            game_interpret(ch,argument);
            return;
        }
    }

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
        argument++;
    if ( argument[0] == '\0' )
        return;

    /*
     * Implement freeze command.
     */
    if ( IS_SET(ch->act, PLR_FREEZE) )
    {
        send_to_char( "@@a@@fYou're totally frozen!@@N\n\r", ch );
        return;
    }

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;
        while ( isspace(*argument) )
            argument++;
    }
    else
    {
        argument = one_argument( argument, command );
    }

    {
        int i;
        char command2[MSL];
        command2[0] = '\0';
        for ( i = 0;i<5;i++ )
        {
            if ( ch->alias[i] == "" )
                continue;
            if ( !str_cmp(ch->alias[i],command) )
            {
                if ( IS_SET( ch->pcdata->pflags, PFLAG_ALIAS ) )
                {
                    send_to_char( "Possible alias looping detected. Cancelling.\n\r", ch );
                    REMOVE_BIT(ch->pcdata->pflags,PFLAG_ALIAS);
                    return;
                }
                sprintf( command2, "%s%s%s", ch->alias_command[i], (argument[0] == '\0') ? "" : " ", argument );
                SET_BIT(ch->pcdata->pflags, PFLAG_ALIAS);
                interpret(ch,command2);
                return;
            }
        }
    }
    if ( IS_SET( ch->pcdata->pflags, PFLAG_ALIAS ) )
        REMOVE_BIT(ch->pcdata->pflags,PFLAG_ALIAS);

    /*
     * Look for command in command table.
     */
    found = FALSE;
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( cmd_table[cmd].level == MAX_LEVEL
            &&  ( trust < MAX_LEVEL )   )
            continue;

        if ( cmd_table[cmd].level == MAX_LEVEL
            &&  ( trust < MAX_LEVEL )   )
            continue;

        if ( command[0] == cmd_table[cmd].name[0]
            &&   !str_prefix( command, cmd_table[cmd].name )
            &&   ( cmd_table[cmd].level <= trust ) )
        {

            found = TRUE;
            break;
        }

    }

    if ( cmd_table[cmd].type != C_TYPE_INFO && cmd_table[cmd].type != C_TYPE_COMM && IS_BUSY(ch) && (cmd_table[cmd].do_fun != do_inventory) )
    {
        char buf[MSL];
        if ( ch->c_sn == gsn_dead || ch->c_sn == gsn_warp || ch->c_sn == gsn_paradrop || ch->dead || ch->position == POS_DEAD
            )
        {
            send_to_char( "Not now.\n\r", ch );
            return;
        }
        sprintf( buf, "%s %s", cmd_table[cmd].name, argument );
        add_to_queue(ch,buf);
        return;
    }
    if ( planet_table[ch->z].system == 4 && cmd_table[cmd].type == C_TYPE_ALLI )
    {
        send_to_char( "You may not use alliance commands in the newbie planet.\n\r", ch );
        return;
    }

    /* Log and snoop.
     */
    if ( cmd_table[cmd].log == LOG_NEVER )
        strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX@@N");

    if ( ( IS_SET(ch->act, PLR_LOG) )
        ||   fLogAll
        ||   cmd_table[cmd].log == LOG_ALWAYS )
    {
        sprintf( log_buf, "Log %s: %s", ch->name, logline );
        log_string( log_buf );
        if ( IS_SET( ch->act, PLR_LOG ) )
            monitor_chan( ch, log_buf, MONITOR_BAD );
        else
        if ( cmd_table[cmd].level >= LEVEL_HERO )
        {
            if ( ch->trust < 90 )
                monitor_chan( ch, log_buf, MONITOR_GEN_IMM );
        }
        else
        {
            monitor_chan( ch, log_buf, MONITOR_GEN_MORT );
        }

    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )   /* -S- Mod */
    {
        char snp[MAX_STRING_LENGTH];
        sprintf( snp, "[Snoop:%s] %s\n\r", ch->name, logline );
        write_to_buffer( ch->desc->snoop_by, snp, 0 );
    }

    if ( !found )
    {
        /*
         * Look for command in socials table.
         */
        if ( !check_social( ch, command, argument )

            //	&&   ( !IMC || !imc_command_hook( ch, command, argument ) )
            )
        {
            /*	    if ( *command == '[' )
                    {
                    interpret(ch,argument);
                    return;
                    }*/
            send_to_char( "Huh?\n\r", ch );
            if ( ch->trust >= 90 )
            {
                send_to_char(command,ch);
                send_to_char(argument,ch);
            }

            if ( my_get_minutes(ch,TRUE)<10 )
                send_to_char( "@@aAre you trying to communicate with others? Try the following commands:@@c\n\rooc blahblah @@g - Send a message to everyone (Replace Blahblah with your message).\n\r@@ctell player blahblah @@g - Send a message to a specific player (Replace Blahblah with your message).\n\r@@clook @@g - See the map.\n\r@@N", ch );
        }
        return;
    }
    /* Disabled command check - Wyn */
    else if ( check_disabled ( ch, &cmd_table[cmd] ) )      /* a normal valid command.. check if it is disabled */
    {
        send_to_char ("This command has been temporarily disabled.\n\r",ch);
        return;
    }

    /*
     * Character not in position for command?
     */
    if ( ch->position < cmd_table[cmd].position )
    {
        switch( ch->position )
        {
            case POS_DEAD:
                send_to_char( "Lie still; you are @@dDEAD@@N.\n\r", ch );
                break;

            case POS_STUNNED:
                ch->position = POS_STANDING;
                break;

            case POS_MORTAL:
            case POS_INCAP:
                send_to_char( "You are @@Rhurt@@N far too bad for that.\n\r", ch );
                break;

            case POS_SLEEPING:
                send_to_char( "Oh, go back to @@Wsleep!@@N\n\r", ch );
                break;

            case POS_RESTING:
                send_to_char( "Naaaaaah... You feel too @@brelaxed@@N...\n\r", ch);
                break;

            case POS_SNEAKING:
                send_to_char( "Not while you're sneaking!\n\r", ch);
                break;

        }
        return;
    }

    /*
     * Dispatch the command.
     */

    comlog(ch, cmd, argument);
    (*cmd_table[cmd].do_fun) ( ch, argument );

    if ( IS_SET( ch->pcdata->pflags,PFLAG_AFK ) && str_cmp(cmd_table[cmd].name,"afk") && !IS_IMMORTAL(ch) )
    {
        REMOVE_BIT( ch->pcdata->pflags,PFLAG_AFK );
        send_to_char( "AFK flag turned off.\n\r", ch );
    }
    tail_chain( );
    return;
}

bool check_social( CHAR_DATA *ch, char *command, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int cmd;
    bool found;

    found  = FALSE;
    for ( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == social_table[cmd].name[0]
            &&   !str_prefix( command, social_table[cmd].name ) )
        {
            found = TRUE;
            break;
        }
    }

    if ( !found )
        return FALSE;

    if ( !IS_NPC(ch) && IS_SET(ch->act, PLR_NO_EMOTE) )
    {
        send_to_char( "You are anti-social!\n\r", ch );
        return TRUE;
    }

    one_argument( argument, arg );
    victim = NULL;
    if ( arg[0] == '\0' )
    {
        act( social_table[cmd].others_no_arg, ch, NULL, victim, TO_ROOM    );
        act( social_table[cmd].char_no_arg,   ch, NULL, victim, TO_CHAR    );
    }
    else if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
    }
    else if ( victim == ch )
    {
        act( social_table[cmd].others_auto,   ch, NULL, victim, TO_ROOM    );
        act( social_table[cmd].char_auto,     ch, NULL, victim, TO_CHAR    );
    }

    /* Remote socials - Wyn */
    else if ( !get_char_room( ch, arg ) && can_see( ch, victim ) )
    {
        if ( !IS_NPC( victim ) )
        {
            char            *ldbase                      = "From a distance, ";
            char             ldmsg [ MAX_STRING_LENGTH ];

            if (   ( !str_cmp(victim->pcdata->ignore_list[0], ch->name) ||
                !str_cmp(victim->pcdata->ignore_list[1], ch->name) ||
                !str_cmp(victim->pcdata->ignore_list[2], ch->name) )   )
            {
                send_to_char( "They are ignoring you.\n\r", ch );
                return TRUE;
            }

            strcpy( ldmsg, ldbase );
            safe_strcat( MSL, ldmsg, social_table[cmd].char_found );
            act( ldmsg,                       ch, NULL, victim, TO_CHAR    );

            strcpy( ldmsg, ldbase );
            safe_strcat( MSL, ldmsg, social_table[cmd].vict_found );
            act( ldmsg,                       ch, NULL, victim, TO_VICT    );
        }
        else
        {
            send_to_char( "They aren't here.\n\r",      ch );
        }
    }
    else
    {
        act( social_table[cmd].others_found,  ch, NULL, victim, TO_NOTVICT );
        act( social_table[cmd].char_found,    ch, NULL, victim, TO_CHAR    );
        act( social_table[cmd].vict_found,    ch, NULL, victim, TO_VICT    );
    }

    return TRUE;
}

void do_disable (CHAR_DATA *ch, char *argument)
{
    int     i;
    DISABLED_DATA   *p,*q;
    char        buf[100];
    char        arg1 [ MAX_INPUT_LENGTH ];
    char        arg2 [ MAX_INPUT_LENGTH ];

    if (IS_NPC(ch))
    {
        send_to_char ("RETURN first.\n\r",ch);
        return;
    }

    if (!argument[0])                                       /* Nothing specified. Show disabled commands. */
    {
        if (!disabled_first)                                /* Any disabled at all ? */
        {
            send_to_char ("There are no commands disabled.\n\r",ch);
            return;
        }

        send_to_char ("Disabled commands:\n\r"
            "Command      To Level    By Level   Disabled by\n\r",ch);

        for (p = disabled_first; p; p = p->next)
        {
            sprintf (buf, "%-12s %5d       %5d %-12s\n\r",p->command->name, p->uptolevel, p->dislevel,p->disabled_by);
            send_to_char (buf,ch);
        }
        return;
    }

    /* command given */

    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );

    /* First check if it is one of the disabled commands */
    for (p = disabled_first; p ; p = p->next)
        if (!str_cmp(arg1, p->command->name))
            break;

    if (p)                                                  /* this command is disabled */
    {
        /* Optional: The level of the imm to enable the command must equal or exceed level
           of the one that disabled it */

        if (get_trust(ch) < p->dislevel)
        {
            send_to_char ("This command was disabled by a higher power.\n\r",ch);
            return;
        }

        /* Remove */

        if (disabled_first == p)                            /* node to be removed == head ? */
            disabled_first = p->next;
        else                                                /* Find the node before this one */
        {
                                                            /* empty for */
            for (q = disabled_first; q->next != p; q = q->next);
            q->next = p->next;
        }

        free_string (p->disabled_by);                       /* free name of disabler */
        dispose (p,sizeof(DISABLED_DATA));                  /* free node */
        save_disabled();                                    /* save to disk */
        send_to_char ("Command enabled.\n\r",ch);
    }
    /*
     * Disable all to re-enable all disabled commands by Canth (canth@xs4all.nl)
     */
    else if ( !str_cmp( arg1, "all" ) )                     /* re-enable all commands */
    {
        p = disabled_first;
        while( p )
        {
            disabled_first = p->next;
            free_string( p->disabled_by );
            dispose( p, sizeof( DISABLED_DATA ) );
            p = disabled_first;
        }
        save_disabled();
        send_to_char( "All commands re-enabled", ch );
    }
    else                                                    /* not a disabled command, check if that command exists */
    {
        /* IQ test */
        if (!str_cmp(arg1,"disable"))
        {
            send_to_char ("You cannot disable the disable command.\n\r",ch);
            return;
        }

        /* Search for the command */
        for (i = 0; cmd_table[i].name[0] != '\0'; i++)
            if (!str_cmp(cmd_table[i].name, arg1))
                break;

        /* Found? */
        if (cmd_table[i].name[0] == '\0')
        {
            send_to_char ("No such command.\n\r",ch);
            return;
        }

        /* Can the imm use this command at all ? */
        if (cmd_table[i].level > get_trust(ch))
        {
            send_to_char ("You dot have access to that command; you cannot disable it.\n\r",ch);
            return;
        }

        /* Disable the command */

        p = getmem (sizeof(DISABLED_DATA));
        p->command = &cmd_table[i];
        p->disabled_by = str_dup (ch->name);                /* save name of disabler */
        p->dislevel = get_trust(ch);                        /* save trust */
        if ( is_number( arg2 ) )                            /* unusable upto & including level arg2 */
            p->uptolevel = atoi( arg2 );                    /* (no check for out of range) */
        else
            p->uptolevel = get_trust( ch );
        p->next = disabled_first;
        disabled_first = p;                                 /* add before the current first element */

        send_to_char ("Command disabled.\n\r",ch);
        save_disabled();                                    /* save to disk */
    }
}

/* Check if that command is disabled
   Note that we check for equivalence of the do_fun pointers; this means
   that disabling 'chat' will also disable the '.' command
*/
bool check_disabled ( CHAR_DATA *ch, const struct cmd_type *command )
{
    DISABLED_DATA *p;

    for (p = disabled_first; p ; p = p->next)
        if (p->command->do_fun == command->do_fun && ch->level <= p->uptolevel)
            return TRUE;

    return FALSE;
}

/* Load disabled commands */
void load_disabled()
{
    FILE *fp;
    DISABLED_DATA *p;
    char *name;
    int i;

    disabled_first = NULL;

    fp = fopen (DISABLED_FILE, "r");

    if (!fp)                                                /* No disabled file.. no disabled commands : */
        return;

    name = fread_word (fp);

    while (str_cmp(name, END_MARKER))                       /* as long as name is NOT END_MARKER :) */
    {
        /* Find the command in the table */
        for (i = 0; cmd_table[i].name[0] ; i++)
            if (!str_cmp(cmd_table[i].name, name))
                break;

        if (!cmd_table[i].name[0])                          /* command does not exist? */
        {
            bug ("Skipping uknown command in " DISABLED_FILE " file.",0);
            fread_number(fp);                               /* uptolevel */
            fread_number(fp);                               /* dislevel */
            fread_word(fp);                                 /* disabled_by */
        }
        else                                                /* add new disabled command */
        {
            p = getmem(sizeof(DISABLED_DATA));
            p->command = &cmd_table[i];
            p->uptolevel = fread_number(fp);
            p->dislevel = fread_number(fp);
            p->disabled_by = str_dup(fread_word(fp));
            p->next = disabled_first;

            disabled_first = p;

        }

        name = fread_word(fp);
    }

    fclose (fp);
}

/* Save disabled commands */
void save_disabled()
{
    FILE *fp;
    DISABLED_DATA *p;

    if (!disabled_first)                                    /* delete file if no commands are disabled */
    {
        unlink (DISABLED_FILE);
        return;
    }

    fp = fopen (DISABLED_FILE, "w");

    if (!fp)
    {
        bug ("Could not open " DISABLED_FILE " for writing",0);
        return;
    }

    for (p = disabled_first; p ; p = p->next)
        fprintf (fp, "%s %d %d %s\n", p->command->name, p->uptolevel, p->dislevel, p->disabled_by);

    fprintf (fp, "%s\n",END_MARKER);

    fclose (fp);
}

void add_to_queue( CHAR_DATA *ch, char *argument )
{
    QUEUE_DATA *q;

    GET_FREE(q,queue_free);
    if ( q->command != NULL )
        free_string(q->command);
    q->command = str_dup(argument);
    q->next = NULL;
    q->is_free = FALSE;
    if ( !ch->pcdata->queue )
    {
        ch->pcdata->queue = q;
    }
    else
    {
        if ( ch->pcdata->last_queue )
        {
            ch->pcdata->last_queue->next = q;
        }
        else
        {
            PUT_FREE(q,queue_free);
            return;
        }
    }
    ch->pcdata->last_queue = q;
    return;
}

void check_queue ( CHAR_DATA * ch )
{
    QUEUE_DATA *q;
    char cmd[MSL];;
    if ( !ch->pcdata->queue )
    {
        if (IS_SET(ch->effect,EFFECT_RUNNING) )
        {
            REMOVE_BIT(ch->effect,EFFECT_RUNNING);
            do_look(ch,"");
        }
        return;
    }
    if ( IS_BUSY(ch) || ch->wait > 0 )
        return;
    if ( ch->dead || ch->position == POS_DEAD || ch->c_sn == gsn_dead )
        return;
    q = ch->pcdata->queue;
    ch->pcdata->queue = q->next;
    if ( q == ch->pcdata->last_queue )
        ch->pcdata->last_queue = NULL;
    sprintf( cmd, "%s", q->command );
    extract_queue(q);
    interpret(ch,cmd);
    if ( !IS_BUSY(ch) )
        check_queue(ch);
    return;
}
