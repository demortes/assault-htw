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

#define DEC_ACK_H 1

#ifndef DEC_TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef DEC_CONFIG_H
#include "config.h"
#endif

#ifndef DEC_UTILS_H
#include "utils.h"
#endif

/* Various linked lists head/tail pointer declarations. -- Altrag */
#ifndef DEC_LISTS_H
#include "lists.h"
#endif

#ifndef DEC_STRFUNS_H
#include "strfuns.h"
#endif

#include "act_skills.h"
#include "mxp.h"
#include "web.h"
// MCCP
/* mccp: support bits */

#include <zlib.h>

#define TELOPT_COMPRESS 85
#define TELOPT_COMPRESS2 86

#define COMPRESS_BUF_SIZE 16384
// End MCCP
#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )    if ( !str_cmp( word, literal ) ){field  = value;fMatch = TRUE;break;}

struct pager_data
{
    bool is_free;
    PAGER_DATA *next;
    PAGER_DATA *prev;
    char        * from;
    char        * msg;
    char        * time;
};
struct multiplay_type
{
    char * name;
    char * host;
};
struct ability_type
{
    char *name;
    sh_int engineering;
    sh_int building;
    sh_int combat;
    char *desc;
};
struct bomb_data
{
    bool is_free;
    BOMB_DATA *next;
    BOMB_DATA *prev;
    OBJ_DATA *obj;
};
struct queue_data
{
    bool is_free;
    QUEUE_DATA *next;
    char *command;
};
struct s_res_type
{
    char *name;
    int bit;
    sh_int type;
    int req;
    sh_int cost;
    int value;
    int value2;
    int value3;
    int level;
};

struct planet_type
{
    char *name;
    sh_int system;
    double gravity;
    sh_int  z;
    sh_int terrain;
    char * note;
};
struct load_type
{
    int     vnum;
    int     rarity;
    int     building;
};

struct alliance_type
{
    char    * name;
    int members;
    char    * leader;
    char    * history;
    int kills;
};

struct wildmap_type
{
    char * color;
    char * mark;
    char * cmark;
    char * name;
    sh_int    heat;
    sh_int    speed;
    sh_int    fuel;
};
struct formula_type
{
    int solid;
    int liquid;
    int gas;
    int vnum;
    int rank;
    char    * desc;
};
struct bonus_type
{
    char    * name;
    char    * desc;
    int item;
};
struct vehicle_data
{
    bool            is_free;
    VEHICLE_DATA    *next;
    VEHICLE_DATA    *prev;
    VEHICLE_DATA    *next_in_room;
    sh_int      type;
    char        *name;
    char        *desc;
    CHAR_DATA   *driving;
    VEHICLE_DATA    *vehicle_in;
    VEHICLE_DATA    *in_vehicle;
    BUILDING_DATA   *in_building;
    int     hit;
    int     max_hit;
    int     ammo_type;
    int     ammo;
    int     max_ammo;
    int     fuel;
    int     max_fuel;
    sh_int      speed;
    int     x;
    int     y;
    int     z;
    int     flags;
    int     timer;
    sh_int      range;
    sh_int      scanner;
    sh_int      state;
    int     power[POWER_MAX];
    int     value[1];                                       //V1 - Superweapon charge
};

struct skill_type
{
    char    * name;
    int gsn;
    bool    prof;
    char    * desc;
};
struct score_type
{
    char    * name;
    char    * killedby;
    int kills;
    int buildings;
    int time;
};
struct rank_type
{
    char    * name;
    int rank;
};
struct clip_type
{
    char    *name;
    int dam;
    int builddam;
    sh_int  miss;
    bool    explode;
    sh_int  speed;
    sh_int  type;
};
struct building_data
{
    bool        is_free;
    BUILDING_DATA   *next;
    BUILDING_DATA   *prev;
    BUILDING_DATA   *next_active;
    BUILDING_DATA   *prev_active;
    BUILDING_DATA   *next_owned;
    BUILDING_DATA   *prev_owned;
    int type;
    char *name;
    bool exit[4];
    int maxhp;
    int hp;
    int maxshield;
    int shield;
    int value[11];                                          //V0 Generate/Super. V1 Installation. V2 Saves HP (Practice). V3 Virus. V4 Safetimer.
    //V5 Space Weapon. V6 Space Armor. V7 Space Shiptype.  V8 Hacking tool. V9 Nuclear Fallout.
    //V10 Space Gold.
    int resources[8];
    char *owned;
    CHAR_DATA *owner;
    int x;
    int y;
    int z;
    sh_int level;
    bool visible;
    char    *attacker;
    bool active;
    sh_int protection;
    sh_int directories;
    sh_int real_dir;
    int password;
    int timer;
};

struct build_type
{
    int type;
    char *name;
    int hp;
    int shield;
    int resources[8];
    int requirements;
    int requirements_l;
    char *desc;
    char *symbol;
    sh_int buildon[MAX_BUILDON];
    bool militairy;
    int rank;
    sh_int act;
    sh_int max;
    bool disabled;
};
struct build_help_type
{
    char *help;
};

struct resource_color_type
{
    char *color;
};

struct map_type
{
    unsigned char  type[MAX_MAPS][MAX_MAPS][Z_MAX];
    signed char    resource[MAX_MAPS][MAX_MAPS][Z_MAX];
};

/*
 * Structure for extended bitvectors.
 */

struct bitvector_data
{
    long        bits[XBI];
};

struct str_array
{
    char *this_string;
};

struct sysdata_type
{
    bool w_lock;
    bool test_open;
    char *playtesters;
    STR_ARRAY imms[1];
    bool shownumbers;
    bool pikamod;
    sh_int  showhidden;
    bool killfest;
    bool p_lock;
    bool kill_groups;
    bool silent_reboot;
    int freemap;
    int qpmode;
    int xpmode;
};

struct ranking_type
{
    char            *   name;
    int                 rankfrom;
    int                 rankto;
};

struct changes_data
{
    char            *   coder;
    char            *   date;
    char            *   change;
    time_t              mudtime;
};

struct log_data
{
    char            *   reporter;
    char            *   type;
    char            *   date;
    char            *   log;
    bool                is_finished;
};

struct board_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    BOARD_DATA   *  next;
    BOARD_DATA   *  prev;
    int             vnum;
    MESSAGE_DATA *  first_message;
    MESSAGE_DATA *  last_message;
    int             min_read_lev;
    int             min_write_lev;
    int             expiry_time;
    int             clan;
};

struct message_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    MESSAGE_DATA *  next;
    MESSAGE_DATA *  prev;                                   /* Only used in save_board */
    BOARD_DATA   *  board;
    time_t          datetime;
    char         *  author;
    char         *  title;
    char         *  message;

};

struct charic_type
{
    char *   name;
};

/*
 * color look-up table structure thingy.
 */

struct color_type
{
    char *      name;                                       /* eg, gossip, say, look */
    int        index;                                       /* unique index */
};

struct ansi_type
{
    char *   name;
    char *   value;                                         /* escape sequence, or whatever */
    int      index;
    char         letter;
    int          stlen;
};

#define color_NORMAL "\033[0m"

#define NO_MATERIAL 10                                      /* Number of materials */

struct dl_list
{
    bool      is_free;
    DL_LIST * next;
    DL_LIST * prev;
    void *    this_one;
};

/*
 * Site ban structure.
 */
struct  ban_data
{
    bool    is_free;                                        /* Ramias:for run-time checks of LINK/UNLINK */
    BAN_DATA *  next;
    BAN_DATA *  prev;
    char *      name;
    char *  banned_by;
    char *  note;
    bool    newbie;
};

struct brand_data
{
    bool      is_free;
    BRAND_DATA *  next;
    BRAND_DATA *  prev;
    char *    branded;
    char *    branded_by;
    char *    dt_stamp;
    char *    message;
    char *    priority;
};

struct  time_info_data
{
    int         hour;
    int         day;
    int         month;
    int         year;
};

/*
 * Descriptor (channel) structure.
 */
struct  descriptor_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    DESCRIPTOR_DATA *   next;
    DESCRIPTOR_DATA *   prev;
    DESCRIPTOR_DATA *   snoop_by;
    CHAR_DATA *         character;
    CHAR_DATA *         original;
    char *              host;
    sh_int              descriptor;
    sh_int              connected;
    bool                fcommand;
    char                inbuf           [4 * MAX_INPUT_LENGTH];
    char                incomm          [MAX_INPUT_LENGTH];
    char                inlast          [MAX_INPUT_LENGTH];
    int                 repeat;
    char *              showstr_head;
    char *              showstr_point;
    char *              outbuf;
    int                 outsize;
    int                 outtop;
    unsigned int    remote_port;                            /* 'Pair Port' ? -S- */
    int         check;                                      /* For new players*/
    int                 flags;
    int                 childpid;                           /* Child process id */
    time_t      timeout;
    // MCCP
    unsigned char   compressing;
    z_stream *          out_compress;
    unsigned char *     out_compress_buf;
    // End MCCP
    bool            mxp;                                    /* player using MXP flag */
};

#define DESC_FLAG_PASSTHROUGH 1                             /* Used when data is being passed to */
/*	Another prog.                     */
/*
 * Help table types.
 */
struct  help_data
{
    bool    is_free;                                        /* Ramias:for run-time checks of LINK/UNLINK */
    HELP_DATA * next;
    HELP_DATA * prev;
    sh_int      level;
    char *      keyword;
    char *      text;
};

/*
 * Per-class stuff.
 */
struct  class_type
{
    char        who_name        [4];                        /* Three-letter name for 'who'  */
    char     *  name;                                       /* Full name                    */
    char     *  desc;                                       /* Class Description		*/
    bool    rec;                                            /* Recommend class for newbies? */
    bool    rank;                                           /* Rank limit 			*/
};

/*
 * A kill structure (indexed by level).
 */
struct  kill_data
{
    sh_int              number;
    sh_int              killed;
};

/*
 * One character (PC or NPC).
 */
struct  char_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    bool        is_quitting;
    CHAR_DATA *         next;
    CHAR_DATA *     prev;
    CHAR_DATA *     next_in_room;
    CHAR_DATA *         leader;
    CHAR_DATA *         reply;
    BUILDING_DATA * first_building;
    ROOM_INDEX_DATA *   in_room;
    int         deaf;
    int                 poly_level;
    DESCRIPTOR_DATA *   desc;
    OBJ_DATA *          first_carry;
    OBJ_DATA *      last_carry;
    PC_DATA *           pcdata;
    char *              name;
    char *              prompt;
    char *              old_prompt;                         /* used to hold prompt when writing */
    char *      last_tell;
    sh_int              sex;
    sh_int      login_sex;
    sh_int              class;
    int         position;
    sh_int      level;
    sh_int      invis;                                      /* For wizinvis imms - lvl invis to */
    sh_int      incog;
    sh_int              trust;
    bool                wizbit;
    int                 played;
    int                 played_tot;
    time_t              logon;
    time_t              save_time;
    time_t              last_note;
    sh_int              timer;
    sh_int              wait;
    sh_int              hit;
    sh_int              max_hit;
    int                 act;
    int                 config;
    int                 act_build;                          /* for setting what ya editing */
    int                 build_vnum;                         /* the current vnum for w-y-e  */
    float              carry_weight;
    sh_int              carry_number;
    int         quest_points;                               /*As special rewards	 */
    BRAND_DATA  *   current_brand;
    BUILDING_DATA   * bvictim;
    sh_int       c_time;
    sh_int       c_sn;
    int          c_level;
    OBJ_DATA *   c_obj;
    CHAR_DATA *  victim;
    int      x;
    int      y;
    int      z;
    BUILDING_DATA * in_building;
    VEHICLE_DATA * in_vehicle;
    sh_int       fighttimer;
    sh_int       questtimer;
    sh_int       spectimer;
    sh_int       killtimer;
    int      medaltimer;
    sh_int map;
    bool security;
    char    * alias[5];
    char    * alias_command[5];
    int   implants;
    int   disease;
    int   effect;
    int   refund[8];
    bool      suicide;
    float     heat;
    bool    dead;
    sh_int  medals;
    bool    fake;
    sh_int  section;
    sh_int  kill_group;
    sh_int  c_count;
    int     game_points;                                    //For mannaroth's games. (We mean cheesly's)
};

/*
 * Data which only PC's have.
 */

struct  pc_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    PC_DATA *       next;
    PC_DATA *           prev;
    int         color[MAX_color];
    int         dimcol;
    int         hicol;
    char *              pwd;
    char *              bamfin;
    char *      room_enter;
    char *      room_exit;
    char *              bamfout;
    char *              title;
    char *              ranking;
    char *      host;                                       /* Used to tell PC last login site */
    sh_int      failures;                                   /* Failed logins */
    sh_int              pagelen;
    sh_int              o_pagelen;
    char        *       header;                             /* header used for message */
    char        *       message;                            /* message for board in progress */
    char    *   who_name;                                   /* To show on who name */
    int         pkills;
    int         bkills;
    int         tpkills;
    int         tbkills;
    int         deaths;
    int         blost;
    int         pbhits;
    int         pbdeaths;
    int         nukemwins;
    int                 pflags;
    char    *   lastlogin;
    int         monitor;                                    /* monitor channel for imms */
    char        *       ignore_list[MAX_IGNORES];           /* Ignore this person */
    int     hp_from_gain;                                   /* same for hitpoints */
    char *  pedit_state;
    char *  pedit_string[5];
    sh_int term_rows;
    sh_int term_columns;
    char * email_address;
    char *    load_msg;
    bool valid_email;
    bool    dead;
    bool    deleted;
    sh_int  skill[MAX_SKILL];
    sh_int  lastskill;
    int     alliance;
    QUEUE_DATA  * queue;
    QUEUE_DATA  * last_queue;
    int     reimb;
    int     prof_points;
    int     prof_ttl;
    int     spec_timer;
    int     spec_init;
    sh_int  set_exit;
    int     experience;
    //    bool	built[MAX_POSSIBLE_BUILDING];
    int     guess;
    PAGER_DATA  * pager;
};

struct  obj_index_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    OBJ_INDEX_DATA *    next;
    char *              owner;
    char *              name;
    sh_int              level;
    char *              short_descr;
    char *              description;
    sh_int              vnum;
    sh_int              item_type;
    int                 extra_flags;
    int                 wear_flags;
    int                 weight;
    int         building;
    int                 value   [MAX_OBJECT_VALUES];
    sh_int      heat;
    char *      image;
};

/*
 * One object.
 */
struct  obj_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    OBJ_DATA *          next;
    OBJ_DATA *      prev;
    OBJ_DATA *      next_in_carry_list;                     /* carry list is the list on a char, or in a container */
    OBJ_DATA *      prev_in_carry_list;
    OBJ_DATA *      first_in_carry_list;
    OBJ_DATA *      next_in_room;
    BOMB_DATA *     bomb_data;
    CHAR_DATA *         carried_by;
    CHAR_DATA *         attacker;
    OBJ_INDEX_DATA *    pIndexData;
    ROOM_INDEX_DATA *   in_room;
    char *              owner;
    char *              name;
    char *              short_descr;
    char *              description;
    int                 item_type;
    int                 extra_flags;
    int                 wear_flags;
    int                 wear_loc;
    int                 weight;
    sh_int              level;
    int                 value   [MAX_OBJECT_VALUES];
    int         x;
    int         y;
    int         z;
    BUILDING_DATA * in_building;
    sh_int      heat;
    int         quest_timer;
    sh_int      quest_map;
};

struct  area_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    AREA_DATA *         next;
    AREA_DATA *     prev;
    char *              name;
    int         offset;
    int                 modified;
    int                 min_vnum;
    int                 max_vnum;
    int                 area_num;
    char *              owner;
    char *              can_read;
    char *              can_write;
    char *              filename;
    int                 flags;
    int         aggro_list;
    BUILD_DATA_LIST *   first_area_room;
    BUILD_DATA_LIST *   last_area_room;
    BUILD_DATA_LIST *   first_area_help_text;
    BUILD_DATA_LIST *   last_area_help_text;
    BUILD_DATA_LIST *   first_area_object;
    BUILD_DATA_LIST *   last_area_object;
    BUILD_DATA_LIST *   first_area_objfunc;
    BUILD_DATA_LIST *   last_area_objfunc;
    char *      keyword;
};

struct  room_index_data
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    ROOM_INDEX_DATA *   next;
    AREA_DATA *         area;
    sh_int              vnum;
};

struct build_data_list                                      /* Used for storing area file data. */
{
    bool        is_free;                                    /* Ramias:for run-time checks of LINK/UNLINK */
    BUILD_DATA_LIST *    next;
    BUILD_DATA_LIST *    prev;
    void *               data;
};

struct lookup_type
{
    char *          text;
    bitset         value;
    int             cost;                                   /* if == NO_USE, only creators can set. */
};

struct  cmd_type
{
    char * const        name;
    DO_FUN *            do_fun;
    sh_int              position;
    sh_int              level;
    sh_int              log;
    sh_int              type;                               /*added by Aeria for do_commands*/
    sh_int              show;                               /*added by Aeria for do_commands*/
};

/*
 * Structure for a social in the socials table.
 */
struct  social_type
{
    char *         name;
    char *         char_no_arg;
    char *         others_no_arg;
    char *         char_found;
    char *         others_found;
    char *         vict_found;
    char *         char_auto;
    char *         others_auto;
};

/*
 * Disable struct - Wyn
 */
struct disabled_data
{
    DISABLED_DATA       *next;                              /* pointer to the next one */
    struct cmd_type const   *command;                       /* pointer to the command struct */
    char            *disabled_by;                           /* name of disabler */
    sh_int           dislevel;                              /* level of disabler */
    sh_int           uptolevel;                             /* level of execution allowed */
};

/* proto's for relevel crap */
#define RELEVEL_FILE    "../data/relevel.dat"
void do_saverelevel( void );
void do_loadrelevel( void );
void do_readrelevel( FILE * fp, RELEVEL_DATA * pRelevel );

/* Relevel DATA */
struct system_data {
    RELEVEL_DATA   * pRelevelList;
} rlvldata;

struct relevel_data {
    RELEVEL_DATA   * pNext;
    char           * strName;
    int              iLevel;
};

/* prototypes from db.c */
BOMB_DATA * make_bomb   args( ( OBJ_DATA *obj ) );
void    load_disabled   args( ( void ) );
void    save_disabled   args( ( void ) );
void  load_buildings_b  args( ( int mode ) );
void  load_vehicles     args( ( int mode ) );
void    load_building_t args( ( void ) );
void    reward_votes args( ( void ) );
/*
 * Extended bitvector utility functions, in handler.c.
 */
bool    xbv_is_empty    args( ( XBV *bits ) );
bool    xbv_same_bits   args( ( XBV *dest, const XBV *src ) );
void    xbv_clear_bits  args( ( XBV *bits ) );
void    xbv_set_bits    args( ( XBV *dest, const XBV *src ) );
void    xbv_remove_bits args( ( XBV *dest, const XBV *src ) );

/*
 * Our function prototypes.
 * One big lump ... this is every function in Merc.
 */
#define CD      CHAR_DATA
#define OD      OBJ_DATA
#define OID     OBJ_INDEX_DATA
#define RID     ROOM_INDEX_DATA
#define SF      SPEC_FUN
//#define OF	OBJ_FUN

/* act_clan.c */
void    load_clan_table args( ( void ) );
void    save_clan_table args( ( void ) );
void    load_map_data args(  ( void )  );

/* act_comm.c */
bool    can_multiplay   args( ( CHAR_DATA *ch ) );
void    add_follower    args( ( CHAR_DATA *ch, CHAR_DATA *master ) );
void    stop_follower   args( ( CHAR_DATA *ch ) );
void    die_follower    args( ( CHAR_DATA *ch ) );
void    send_to_loc        args( ( char *message, int x, int y, int z ) );
void    list_who_to_output   args(  ( void )   );

/* act_info.c */
void    set_title       args( ( CHAR_DATA *ch, char *title ) );
char *  color_string   args( ( CHAR_DATA *CH, char *argument ) );
void    display_details args( ( CHAR_DATA * viewer, CHAR_DATA *ch ) );
void    display_details_old args( ( CHAR_DATA * viewer, CHAR_DATA *ch ) );
void    show_building_info args( (CHAR_DATA *ch, int i) );

/* act_move.c */
void    move_char       args( ( CHAR_DATA *ch, int door ) );
void    crash       args( ( CHAR_DATA *ch, CHAR_DATA *attacker ) );
void    move        args( ( CHAR_DATA *ch, int x, int y, int z ) );
void    move_vehicle    args( ( VEHICLE_DATA *vhc, int x, int y, int z ) );
void    move_obj    args( ( OBJ_DATA *obj, int x, int y, int z ) );

/* act_obj.c */
void    get_obj     args( ( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container ) );
bool can_wear_at(CHAR_DATA * ch, OBJ_DATA * obj, int location);
void    wear_obj        args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );

/* board.c */
BOARD_DATA * load_board(OBJ_DATA * obj);
void    do_show_contents        args( ( CHAR_DATA *ch, OBJ_DATA * obj ) );
void    do_show_message         args( ( CHAR_DATA *ch, int mess_num, OBJ_DATA * obj ) );
void    do_edit_message     args( ( CHAR_DATA *ch, int mess_num, OBJ_DATA * obj ) );
void    do_add_to_message       args( ( CHAR_DATA *ch, char *argument ) );
void    do_start_a_message      args( ( CHAR_DATA *ch, char *argument ) );
void    save_message_data       args( ( void ) );
void    load_messages           args( ( void ) );

/* comm.c */
void    close_socket    args( ( DESCRIPTOR_DATA *dclose ) );
void    show_menu_to    args( ( DESCRIPTOR_DATA *d ) );     /* Main */
void    show_smenu_to   args( ( DESCRIPTOR_DATA *d ) );     /* Sex */
void    show_cmenu_to   args( ( DESCRIPTOR_DATA *d ) );     /* Class */
void    show_bmenu_to   args( ( DESCRIPTOR_DATA *d ) );     /* Bonus */
void   show_pmenu_to    args( ( DESCRIPTOR_DATA *d ) );     /* Planet */
void    write_to_buffer args( ( DESCRIPTOR_DATA *d, const char *txt,
                                int length ) );
void    send_to_char    args( ( const char *txt, CHAR_DATA *ch ) );
void    show_string     args( ( DESCRIPTOR_DATA *d, char *input ) );
void    act             args( ( const char *format, CHAR_DATA *ch,
                                const void *arg1, const void *arg2, int type ) );
void    hang            args( ( const char *str ) );

/* db.c */
void    load_sobjects   args( ( int mode ) );
void   perm_update args( ( void ) );
void    boot_db         args( ( bool fCopyOver ) );
void    area_update     args( ( void ) );
OD *    create_object   args( ( OBJ_INDEX_DATA *pObjIndex, int level ) );
BUILDING_DATA *    create_building   args( ( int type ) );
VEHICLE_DATA *     create_vehicle   args( ( int type ) );
void    clear_char      args( ( CHAR_DATA *ch ) );
void    free_char       args( ( CHAR_DATA *ch ) );
OID *   get_obj_index   args( ( int vnum ) );
RID *   get_room_index  args( ( int vnum ) );
char    fread_letter    args( ( FILE *fp ) );
int     fread_number    args( ( FILE *fp ) );
long_int fread_long_number args( ( FILE *fp ) );
char *  fread_string    args( ( FILE *fp ) );
void    fread_to_eol    args( ( FILE *fp ) );
char *  fsave_to_eol    args( ( FILE *fp ) );
char *  fread_word      args( ( FILE *fp ) );

/* void *  alloc_mem       args( ( int sMem ) );
void    check_freed     args( ( unsigned int first, unsigned int last) );
void    check_free_mem  args( ( void ) );
void *  alloc_perm      args( ( int sMem ) );
void    free_mem        args( ( void *pMem, int sMem ) );*/

/* spec: renamed getmem -> _getmem, nuked unused alloc_perm */
/* void *  alloc_perm      args( ( int sMem ) ); */
void *  _getmem     args( ( int size, const char *caller, int log ) );
void    dispose     args( ( void *mem, int size ) );
char *  str_dup         args( ( const char *str ) );
void    free_string     args( ( char *pstr ) );
int     number_fuzzy    args( ( int number ) );
int     number_range    args( ( int from, int to ) );
int     number_percent  args( ( void ) );
int     number_door     args( ( void ) );
int     number_bits     args( ( int width ) );
int     number_mm       args( ( void ) );
int     dice            args( ( int number, int size ) );
int     interpolate     args( ( int level, int value_00, int value_32 ) );
void    append_file     args( ( CHAR_DATA *ch, char *file, char *str ) );
void    bug             args( ( const char *str, int param ) );
void    log_string      args( ( const char *str ) );
void    tail_chain      args( ( void ) );
void    safe_strcat     args( ( int max_len, char * dest,char * source ) );
void    send_to_descrips args( ( const char *message ) );
void    bug_string      args( ( const char *str, const char *str2) );
/* Added stuff -Flar */
void    bugf (char * fmt, ...) __attribute__ ((format(printf,1,2)));
void    log_f (char * fmt, ...) __attribute__ ((format(printf,1,2)));

/* fight.c */
void    gain_exp    args( ( CHAR_DATA *ch, int value ) );
void    damage          args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dam,
                                int dt ) );
void    damage_building args( ( CHAR_DATA *ch, BUILDING_DATA *bld, int dam ) );
void    damage_vehicle  args( ( CHAR_DATA *ch, VEHICLE_DATA *vhc, int dam, int dt ) );
void    update_pos      args( ( CHAR_DATA *victim ) );
void    raw_kill        args( ( CHAR_DATA *victim, char *argument ) );
void    set_fighting    args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    check_armor     args( ( OBJ_DATA *obj ) );
bool    check_dead  args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
void    pdie        args( ( CHAR_DATA *ch ) );
void    set_stun    args( ( CHAR_DATA *ch, int time ) );
bool    same_planet     args( (CHAR_DATA *ch, CHAR_DATA *vch ) );

/* handler.c */
void    activate_building   args( ( BUILDING_DATA *bld, bool on ) );
void    check_prof  args( ( CHAR_DATA *ch ) );
int count_users args( (OBJ_DATA *obj) );
bool    remove_obj  args( ( CHAR_DATA *ch, int iWear, bool fReplace ) );
int     get_trust       args( ( CHAR_DATA *ch               ) );
void    my_get_age  args( ( CHAR_DATA *ch, char * buf       ) );
int     my_get_hours    args( ( CHAR_DATA *ch, bool total ) );
int     my_get_minutes  args( ( CHAR_DATA *ch, bool total ) );
int     get_age         args( ( CHAR_DATA *ch ) );
int     can_carry_n     args( ( CHAR_DATA *ch ) );
int     can_carry_w     args( ( CHAR_DATA *ch ) );
void    char_from_room  args( ( CHAR_DATA *ch ) );
void    char_to_room    args( ( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex ) );
void    char_to_building args( ( CHAR_DATA *ch, BUILDING_DATA *bld ) );
void    obj_to_char     args( ( OBJ_DATA *obj, CHAR_DATA *ch ) );
void    obj_from_char   args( ( OBJ_DATA *obj ) );
OD *    get_eq_char     args( ( CHAR_DATA *ch, int iWear ) );
void    equip_char      args( ( CHAR_DATA *ch, OBJ_DATA *obj, int iWear ) );
void    unequip_char    args( ( CHAR_DATA *ch, OBJ_DATA *obj ) );
int     count_obj_list  args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
int     count_obj_room  args( ( OBJ_INDEX_DATA *obj, OBJ_DATA *list ) );
void    obj_from_room   args( ( OBJ_DATA *obj ) );
void    obj_to_room     args( ( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex ) );
void    obj_to_obj      args( ( OBJ_DATA *obj, OBJ_DATA *obj_to ) );
void    obj_from_obj    args( ( OBJ_DATA *obj ) );
void    extract_obj     args( ( OBJ_DATA *obj ) );
void    extract_building args( ( BUILDING_DATA *bld, bool msg ) );
void    extract_vehicle args( ( VEHICLE_DATA *vhc, bool msg ) );
void    extract_char    args( ( CHAR_DATA *ch, bool fPull ) );
void    extract_queue   args( ( QUEUE_DATA *q ) );
void	empty_queue	args( (CHAR_DATA *ch) );
void    extract_pager   args( ( PAGER_DATA *p ) );
BUILDING_DATA *    get_char_building   args( ( CHAR_DATA *ch ) );
BUILDING_DATA *    get_obj_building    args( ( OBJ_DATA *obj ) );
BUILDING_DATA *    get_building      args( ( int x, int y, int z ) );
BUILDING_DATA *    get_building_range  args( ( int x, int y, int x2, int y2, int z ) );
VEHICLE_DATA * get_vehicle_char     args( ( CHAR_DATA *ch, char *argument ) );
VEHICLE_DATA *get_vehicle       args( ( char *argument, int x, int y, int z ) );
VEHICLE_DATA *get_vehicle_world     args( ( char *argument ) );
CD *    get_ch   args( ( char *argument ) );
CD *    get_char_room   args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_world  args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_area   args( ( CHAR_DATA *ch, char *argument ) );
CD *    get_char_loc      args( ( int x, int y, int z ) );
OD *    get_obj_loc       args( ( CHAR_DATA *ch, char *argument, int x, int y ) );
OD *    get_obj_type    args( ( OBJ_INDEX_DATA *pObjIndexData ) );
OD *    get_obj_list    args( ( CHAR_DATA *ch, char *argument,
                                OBJ_DATA *list ) );
OD *    get_obj_room    args( ( CHAR_DATA *ch, char *argument,
                                OBJ_DATA *list ) );
OD *    get_obj_carry   args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_wear    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_here    args( ( CHAR_DATA *ch, char *argument ) );
OD *    get_obj_world   args( ( CHAR_DATA *ch, char *argument ) );
int     get_obj_number  args( ( OBJ_DATA *obj ) );
int     get_obj_weight  args( ( OBJ_DATA *obj           ) );
bool    can_see         args( ( CHAR_DATA *ch, CHAR_DATA *victim ) );
bool    can_see_obj     args( ( CHAR_DATA *ch, OBJ_DATA *obj    ) );
bool    can_drop_obj    args( ( CHAR_DATA *ch, OBJ_DATA *obj    ) );
bool    can_use         args( ( CHAR_DATA *ch, OBJ_DATA *obj    ) );
char *  who_can_use     args( ( OBJ_DATA *obj           ) );
void    info            args( ( char * message, int lv      ) );
void    log_chan    args( ( const char * message, int lv    ) );
CD   *  switch_char args( ( CHAR_DATA *victim, int mvnum, int poly_level ) );
CD   *  unswitch_char   args( ( CHAR_DATA *ch           ) );
void    monitor_chan    args( ( CHAR_DATA *ch, const char *message, int channel ) );
CD   *  get_char        args( ( CHAR_DATA *ch ) );
void char_reference args( (struct char_ref_type *ref) );
void char_unreference   args( (CHAR_DATA **var) );
void obj_reference  args( (struct obj_ref_type *ref) );
void obj_unreference    args( (OBJ_DATA **var) );
CHAR_DATA *get_rand_char args( ( int x, int y, int z ) );
int get_random_planet   args( ( ) );

/* interp.c */
void    interpret       args( ( CHAR_DATA *ch, char *argument )   );
bool    is_number       args( ( char *arg )                       );
bool    check_social    args( ( CHAR_DATA *ch, char *command, char *argument ) );
void    add_to_queue    args( ( CHAR_DATA *ch, char *argument ) );
void    check_queue args( ( CHAR_DATA *ch ) );

// logs.c
void        load_changes        args( ( void ) );
void        save_changes        args( ( void ) );
void        delete_change       args( ( int num ) );
char    *   current_date        args( ( void ) );
int         num_changes         args( ( void ) );
void        load_logs           args( ( void ) );
void        save_logs           args( ( void ) );

/* macros.c */
void clear_basic    args( ( CHAR_DATA *ch ) );
int  count_buildings    args( ( CHAR_DATA *victim ) );
bool hidden     args( ( CHAR_DATA *victim ) );
bool open_bld       args( ( BUILDING_DATA *bld ) );
bool has_ability    args( ( CHAR_DATA *ch, int abil ) );
bool open_scaffold  args( (CHAR_DATA *ch, OBJ_DATA *obj) );
OBJ_DATA * make_quest_base args( ( int type, int size, int z ) );
void reset_building args( ( BUILDING_DATA *bld, int type ) );
void reset_special_building args( (BUILDING_DATA *bld) );
bool    ok_to_use   args( ( CHAR_DATA *ch, int value    ) );
void create_blueprint   args( ( BUILDING_DATA *bld ) );
bool complete       args( ( BUILDING_DATA *bld ) );
bool is_upgrade     args( ( int type ) );
bool is_neutral     args( ( int type ) );
bool is_evil        args( ( BUILDING_DATA *bld ) );
int get_char_cost   args( ( CHAR_DATA *ch ) );
bool IS_BETWEEN     args( ( int x, int x1, int x2 ) );
bool building_can_shoot args( ( BUILDING_DATA *bld, CHAR_DATA *ch, int range ) );
int  get_rank       args( ( CHAR_DATA *ch ) );
int  get_bit_value  args( ( int bit ) );
OBJ_DATA *create_material( int type );
VEHICLE_DATA *get_vehicle_from_vehicle( VEHICLE_DATA *vhc );
OBJ_DATA *create_teleporter( BUILDING_DATA *bld, int range );
OBJ_DATA *create_locator( int range );
bool sneak      args( ( CHAR_DATA *ch ) );
void check_building_destroyed args( (BUILDING_DATA *bld) );
int get_item_limit  args( ( BUILDING_DATA *bld ) );
OBJ_DATA *create_element( int type );
void send_warning   args( ( CHAR_DATA *ch, BUILDING_DATA *bld, CHAR_DATA *victim ) );
void update_ranks   args( ( CHAR_DATA *ch ) );
bool defense_building   args( ( BUILDING_DATA *bld ) );
void sendsound      args( ( CHAR_DATA *ch, char *file, int V, int I, int P, char *T, char *filename ) );
int check_dodge     args( ( CHAR_DATA *ch, int chance ) );
bool in_range       args(( CHAR_DATA *ch, CHAR_DATA *victim, int range ));
int get_ship_range  args( ( VEHICLE_DATA *vhc ) );
int get_ship_weapon_range   args( ( VEHICLE_DATA *vhc ) );
bool ok_moon        args( ( int sec ) );
void make_medal_base    args( ( CHAR_DATA *ch ) );
void create_obj_atch    args( ( CHAR_DATA *ch, int index ) );
bool blind_spot     args( ( CHAR_DATA *ch, int x, int y ) );
int get_armor_value args( ( int dt ) );
void real_coords(int *x,int *y);

/*------*\ 
) save.c (
\*------*/
void    save_char_obj   args( ( CHAR_DATA *ch ) );
bool    load_char_obj   args( ( DESCRIPTOR_DATA *d, char *name, bool system_call ) );
void    save_objects    args( ( int mode ) );
void    save_map    args( ( void ) );
void    save_buildings  args( ( void ) );
void    save_vehicles   args( ( int mode ) );
void    save_scores args( ( void ) );
void    save_ranks  args( ( void ) );
void    save_multiplay  args( ( void ) );
void    save_planets    args( ( void ) );
void    fread_object    args( ( FILE * fp ) );
void    save_bans   args( ( void ) );
char    *initial    args( ( const char *str ) );
void   save_buildings_b args( ( int mode ) );
void    save_alliances  args( ( void ) );
void    save_building_table args( ( void ) );

/* social-edit.c  */

void load_social_table  args(  ( void )  );

/*---------*\ 
) trigger.c (
\*---------*/

void    trigger_handler args( ( CHAR_DATA *ch, OBJ_DATA *obj, int trigger ) );

/*--------*\ 
) update.c# (
\*--------*/
int get_user_seconds args(());
void    explode     args( ( OBJ_DATA *obj ) );
void    update_handler  args( ( void )                                );
void    rooms_update    args( ( void ) );
void    building_update args( ( void ) );
void    init_alarm_handler args(  ( void ) );
void    alarm_update args( ( void ) );
void    building_update args( ( void ) );

/* write.c */
void    write_start     args( ( char ** dest, void * retfunc, void * retparm, CHAR_DATA * ch ) );
void    write_interpret args( ( CHAR_DATA * ch, char * argument ) );

/* build.c */
void  build_strdup(char ** dest,char * src,bool freesrc,CHAR_DATA * ch);
char * build_simpstrdup( char * buf);                       /* A plug in alternative to str_dup */
void build_save args( ( void ) );
extern const char * cDirs;
int  get_dir(char);
char * show_values( const struct lookup_type * table, int value, bool fBit );

/* buildtab.c  */
/*
int table_lookup	args( (const struct lookup_type * table,char * name) );
char * rev_table_lookup	args( (const struct lookup_type * table,int number) );
char * bit_table_lookup	args( (const struct lookup_type * table,int number) );
*/

/* buildare.c */
/* Area manipulation funcs in buildare.c */
int build_canread(AREA_DATA * Area,CHAR_DATA * ch,int showerror);
int build_canwrite(AREA_DATA * Area,CHAR_DATA * ch,int showerror);
#define AREA_NOERROR   0
#define AREA_SHOWERROR 1

/* areasave.c */
void area_modified(AREA_DATA *);
void build_save_flush(void);

/*    SSM   */
void temp_fread_string  args( (FILE * fp, char *buf) );
void save_brands    args( ( void ) );

/*
 *  sysdat.c
 */
void load_sysdata   args( ( void ) );
void save_sysdata   args( ( void ) );

// MCCP
/*
 * mccp.c
 */
bool compressStart(DESCRIPTOR_DATA *desc, unsigned char telopt);
bool compressEnd(DESCRIPTOR_DATA *desc,unsigned char type);
bool process_compressed(DESCRIPTOR_DATA *desc);
bool write_compressed(DESCRIPTOR_DATA *desc, char *txt, int length);
// End MCCP

/*
 * mxp.c
 */
void convert_mxp_tags (const int bMXP, char * dest, const char *src, int length);
int count_mxp_tags (const int bMXP, const char *txt, int length);
void turn_on_mxp (DESCRIPTOR_DATA *d);

// act_misc.c
void respawn_buildings args( (CHAR_DATA *ch) );
int get_loc args( ( char *loc ) );
bool can_build args( ( int type, int sect, int planet ) );
int parse_direction args( ( CHAR_DATA *ch, char *arg ) );

// act_alliance.c
void    do_pipe                 args( ( CHAR_DATA *ch, char *argument ) );

bool    upgradable      args( ( BUILDING_DATA *bld ) );

void    quest_update    args( ( void ) );
void draw_space( CHAR_DATA *ch );

// terrain.c
void create_map args( ( CHAR_DATA *ch, int type ) );
void create_special_map args( ( void ) );
void make_lava_river    args( (int x, int y, int z) );
void init_fields    args( ( void ) );

// games.c
void nuke_blow args( ( CHAR_DATA *ch ) );

//buildings.c
bool check_missile_defense args( (OBJ_DATA *obj) );

//web.c
void update_web_data    args( ( int type, char *value ) );
void generate_webpage   args( ( void ) );
void load_web_data  args( ( void ) );

#undef  CD
#undef  MID
#undef  OD
#undef  OID
#undef  RID
#undef  SF
#undef  OF

#ifndef DEC_GLOBALS_H
#include "globals.h"
#endif
