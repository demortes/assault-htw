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
 *                        http://ackmud.nuc.net/                           *
 *                        zenithar@ackmud.nuc.net                          *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * Gsn's
 */
#define gsn_research    0
#define gsn_repair  1
#define gsn_sneak   2
#define gsn_dig     3
#define gsn_chemistry   4
#define gsn_target  5
#define gsn_computer    6
#define gsn_exresearch  7
#define gsn_dodge   8
#define gsn_engineering 9
#define gsn_building    10
#define gsn_combat  11
#define gsn_arm     12
#define gsn_blind_combat 13
#define gsn_grenades    14

#define gsn_move    912
#define gsn_tunnel  913
#define gsn_paradrop    914
#define gsn_crack   915
#define gsn_hack    916
#define gsn_format  917
#define gsn_scan    918
#define gsn_spoof   919
#define gsn_build   920
#define gsn_sresearch   921
#define gsn_warp    922
#define gsn_mine    923
#define gsn_mark    924
#define gsn_fix     925
#define gsn_eng_check   926
#define gsn_oreresearch 927
#define gsn_practice    928
#define gsn_stun    998
#define gsn_dead    999
/*
 * Act functions.
 */

bool blind_combat_check args( (CHAR_DATA *ch) );
DECLARE_ACT_FUN( act_build );
DECLARE_ACT_FUN( act_research );
DECLARE_ACT_FUN( act_sneak );
DECLARE_ACT_FUN( act_repair );
DECLARE_ACT_FUN( act_dig );
DECLARE_ACT_FUN( act_chemistry );
DECLARE_ACT_FUN( act_target );
DECLARE_ACT_FUN( act_tunnel );
DECLARE_ACT_FUN( act_computer );
DECLARE_ACT_FUN( act_exresearch );
DECLARE_ACT_FUN( act_paradrop );
DECLARE_ACT_FUN( act_crack );
DECLARE_ACT_FUN( act_hack );
DECLARE_ACT_FUN( act_format );
DECLARE_ACT_FUN( act_scandir );
DECLARE_ACT_FUN( act_spoof );
DECLARE_ACT_FUN( act_sresearch );
DECLARE_ACT_FUN( act_warp );
DECLARE_ACT_FUN( act_mine );
DECLARE_ACT_FUN( act_mark );
DECLARE_ACT_FUN( act_fix );
DECLARE_ACT_FUN( act_eng_check );
DECLARE_ACT_FUN( act_oresearch );
DECLARE_ACT_FUN( act_practice );
DECLARE_ACT_FUN( act_arm );
