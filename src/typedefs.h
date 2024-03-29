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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
/* For forks etc. */
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define DEC_TYPEDEFS_H      1

#if 0
#ifndef DEC_ACK_H
#include "ack.h"
#endif
#endif

/*
 * Accommodate old non-Ansi compilers.
 */
#if defined(TRADITIONAL)
#define const
#define args( list )                    ( )
#define DECLARE_DO_FUN( fun )           void fun( )
#define DECLARE_SPEC_FUN( fun )         bool fun( )
#define DECLARE_OBJ_FUN( fun )      void fun( )
#define DECLARE_ACT_FUN( fun )      void fun( )
#else
#define args( list )                    list
#define DECLARE_DO_FUN( fun )           DO_FUN    fun
#define DECLARE_SPEC_FUN( fun )         SPEC_FUN  fun
#define DECLARE_OBJ_FUN( fun )          OBJ_FUN   fun
#define DECLARE_ACT_FUN( fun )          ACT_FUN   fun
#endif

/*
 * Short scalar types.
 * Diavolo reports AIX compiler has bugs with short types.
 */
#if     !defined(NOWHERE)
#define NOWHERE -1
#endif

#if     !defined(FALSE)
#define FALSE    0
#endif

#if     !defined(TRUE)
#define TRUE     1
#endif

#if     defined(_AIX)
#if     !defined(const)
#define const
#endif
typedef int                             sh_int;
typedef int                             bool;
#define unix
#else
#if !defined(sh_int)
typedef short    int                    sh_int;
#endif
#if !defined(bool)
typedef unsigned char                   bool;
#endif
#endif
typedef int                             long_int;
typedef unsigned long int       bitset;

/*
 * OS-dependent declarations.
 * These are all very standard library functions,
 *   but some systems have incomplete or non-ansi header files.
 */
#if     defined(_AIX)
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined(apollo)
int     atoi            args( ( const char *string ) );
void *  calloc          args( ( unsigned nelem, size_t size ) );
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined(hpux)
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined(linux)
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined(macintosh)
#define NOCRYPT
#if     defined(unix)
#undef  unix
#endif
#endif

#if     defined(MIPS_OS)
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined(MSDOS)
#define NOCRYPT
#if     defined(unix)
#undef  unix
#endif
#endif

#if     defined(NeXT)
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

#if     defined(sequent)
char *  crypt           args( ( const char *key, const char *salt ) );
int     fclose          args( ( FILE *stream ) );
int     fprintf         args( ( FILE *stream, const char *format, ... ) );
int     fread           args( ( void *ptr, int size, int n, FILE *stream ) );
int     fseek           args( ( FILE *stream, long offset, int ptrname ) );
void    perror          args( ( const char *s ) );
int     ungetc          args( ( int c, FILE *stream ) );
#endif

#if     defined(sun)
char *  crypt           args( ( const char *key, const char *salt ) );
int     fclose          args( ( FILE *stream ) );
int     fprintf         args( ( FILE *stream, const char *format, ... ) );
#if defined(SYSV)
size_t  fread       args( ( void *ptr, size_t size, size_t n, FILE *stream ) );
#else
int     fread           args( ( void *ptr, int size, int n, FILE *stream ) );
#endif
int     fseek           args( ( FILE *stream, long offset, int ptrname ) );
void    perror          args( ( const char *s ) );
int     ungetc          args( ( int c, FILE *stream ) );
#endif

#if     defined(ultrix)
char *  crypt           args( ( const char *key, const char *salt ) );
#endif

/*
 * The crypt(3) function is not available on some operating systems.
 * In particular, the U.S. Government prohibits its export from the
 *   United States to foreign countries.
 * Turn on NOCRYPT to keep passwords in plain text.
 */
#if     defined(NOCRYPT)
#define crypt(s1, s2)   (s1)
#endif

typedef struct  area_data               AREA_DATA;
typedef struct  ban_data                BAN_DATA;
typedef struct  bomb_data    	       	BOMB_DATA;
typedef struct  brand_data              BRAND_DATA;
typedef struct  board_data              BOARD_DATA;
typedef struct  buf_data_struct         BUF_DATA_STRUCT;
typedef struct  build_data_list         BUILD_DATA_LIST;    /* Online Building */
typedef struct  building_data           BUILDING_DATA;
typedef struct  changes_data            CHANGE_DATA;
typedef struct  char_data               CHAR_DATA;
typedef struct  control_data            CONTROL_DATA;
typedef struct  control_list            CONTROL_LIST;
typedef struct  descriptor_data         DESCRIPTOR_DATA;
typedef struct  disabled_data           DISABLED_DATA;      /* Disabling of commands - Wyn */
typedef struct  dl_list                 DL_LIST;
typedef struct  extra_descr_data        EXTRA_DESCR_DATA;
typedef struct  hash_entry_tp           HASH_ENTRY;
typedef struct  help_data               HELP_DATA;
typedef struct  influence_data          INFLUENCE_DATA;
typedef struct  influence_list          INFLUENCE_LIST;
typedef struct  interact_data           INTERACT_DATA;
typedef struct  kill_data               KILL_DATA;
typedef struct  load_data   	    	LOAD_DATA;
typedef struct  log_data                LOG_DATA;
typedef struct  message_data            MESSAGE_DATA;
typedef struct  mob_index_data          MOB_INDEX_DATA;
typedef struct  npc_data                NPC_DATA;
typedef struct  obj_data                OBJ_DATA;
typedef struct  obj_index_data          OBJ_INDEX_DATA;
typedef struct  pager_data              PAGER_DATA;
typedef struct  pc_data                 PC_DATA;
typedef struct  queue_data              QUEUE_DATA;
typedef struct  queued_interact_list    QUEUED_INTERACT_LIST;
typedef struct  relevel_data            RELEVEL_DATA;
typedef struct  room_index_data         ROOM_INDEX_DATA;
typedef struct  str_array               STR_ARRAY;
typedef struct  sysdata_type            SYS_DATA_TYPE;
typedef struct  time_info_data          TIME_INFO_DATA;
typedef struct  trigger_data           	TRIGGER_DATA;
typedef struct  vehicle_data            VEHICLE_DATA;
typedef struct  weather_data            WEATHER_DATA;

/*
 * Function types.
 */
typedef void DO_FUN     args( ( CHAR_DATA *ch, char *argument ) );
typedef bool SPEC_FUN   args( ( CHAR_DATA *ch ) );
typedef void OBJ_FUN    args( ( OBJ_DATA *obj, CHAR_DATA *keeper ) );
typedef void ACT_FUN    args( ( CHAR_DATA *ch, int level ) );

/*
 * Extended bitvector type
 */
typedef struct  bitvector_data      XBV;
