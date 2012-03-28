/*
 * IMC2 AntiFreeze Client - Developed by Mud Domain.
 * This code is certified as 100% LGPL compatible.
 * All aspects have been verified against the core components
 * contained in Oliver's relicensed code. No components licensed
 * under the GPL remain.
 *
 * All additions Copyright (C)2002-2004 Roger Libiez ( Samson )
 * Contributions by Xorith, Copyright (C)2004
 * Contributions by Orion Elder, Copyright (C)2003
 * Comments and suggestions welcome: imc@imc2.org
 * Registered with the United States Copyright Office: TX 5-962-561
 *
 * We would like to thank the following people for their hard work on
 * versions 1.00 Gold through 3.00 Mud-Net even though
 * their contributions no longer remain:
 *
 * Scion Altera, Shogar, Kratas, Tagith, Noplex,
 * Senir, Trax and Ntanel StormBlade.
 *
 * This work is derived from:
 * IMC2 version 0.10 - an inter-mud communications protocol
 * Copyright (C) 1996 & 1997 Oliver Jowett <oliver@randomly.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <fnmatch.h>
#if defined(__OpenBSD__) || defined(__FreeBSD__)
#include <sys/types.h>
#endif
#include "md5.h"

#if defined(IMCCIRCLE)
#include "conf.h"
#include "sysdep.h"
#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "interpreter.h"
#include "imc.h"
time_t current_time;
#endif

#if defined(IMCSMAUG) || defined(IMCCHRONICLES)
#include "mud.h"
#ifdef IMCCHRONICLES
#include "factions.h"
#include "files.h"
#endif
#endif
#if defined(IMCROM) || defined(IMCMERC) || defined(IMCUENVY) || defined(IMC1STMUD)
#include "merc.h"
#ifdef IMCROM
#include "tables.h"
#endif
#endif
#ifdef IMCACK
#include "ack.h"
#endif

#define IMCKEY( literal, field, value ) \
    if( !strcasecmp( word, literal ) ) \
    { \
        field = value; \
        fMatch = TRUE; \
        break; \
    }

    static char *wr_to;
static char wr_buf[IMC_DATA_LENGTH];
static int wr_sequence;
int imc_active;                                             /* Connection state */
time_t imc_now;                                             /* current time */
time_t imc_boot;                                            /* start time */
int imcwait;                                                /* Reconnect timer */
int imcconnect_attempts;                                    /* How many times have we tried to reconnect? */
unsigned long imc_sequencenumber;                           /* sequence# for outgoing packets */
bool imcpacketdebug = FALSE;
time_t imcucache_clock;                                     /* prune ucache stuff regularly */
static char lastping[IMC_MNAME_LENGTH];
static char pinger[100];
char *IMC_VERSIONID;

SITEINFO *this_imcmud;
IMC_CHANNEL *first_imc_channel;
IMC_CHANNEL *last_imc_channel;
REMOTEINFO *first_rinfo;
REMOTEINFO *last_rinfo;
IMC_BAN *first_imc_ban;
IMC_BAN *last_imc_ban;
IMCUCACHE_DATA *first_imcucache;
IMCUCACHE_DATA *last_imcucache;
IMC_COLOR *first_imc_color;
IMC_COLOR *last_imc_color;
IMC_CMD_DATA *first_imc_command;
IMC_CMD_DATA *last_imc_command;
IMC_HELP_DATA *first_imc_help;
IMC_HELP_DATA *last_imc_help;
IMC_PHANDLER *first_phandler;
IMC_PHANDLER *last_phandler;

imc_statistics imc_stats;

_imc_memory imc_memory[IMC_MEMORY];                         /* sequence memory */
static int memory_head;                                     /* next entry in memory table to use, wrapping */

void imclog( const char *format, ... ) __attribute__ ( ( format( printf, 1, 2 ) ) );
void imcbug( const char *format, ... ) __attribute__ ( ( format( printf, 1, 2 ) ) );
void imc_printf( CHAR_DATA *ch, const char *fmt, ... ) __attribute__ ( ( format( printf, 2, 3 ) ) );
void imcpager_printf( CHAR_DATA *ch, const char *fmt, ... ) __attribute__ ( ( format( printf, 2, 3 ) ) );
char *imc_funcname( IMC_FUN *func );
IMC_FUN *imc_function( const char *func );
char *imc_send_social( CHAR_DATA *ch, char *argument, int telloption );
void imc_save_config( void );
void imc_save_channels( void );

char *const imcperm_names[] =
{
    "Notset", "None", "Mort", "Imm", "Admin", "Imp"
};

static struct
{
    int number;
    const char *name;
    const char *chatstr;
    const char *emotestr;
    int perm;
    char *to;
} imc_channels[]=
{
    {
        2,
        "imcinfo",
        /* why didn't I think of this? :) */
        "~p[~GIMCinfo~p] %s announces '%s'\n\r",
        "~p[~GIMCinfo~p] %s %s\n\r",
        IMCPERM_IMM,
        "*",
    },

    {
        15,
        "imc",
        "~p[~GIMC~p] %s %s\n\r",
        "~p[~GIMC~p] %s %s\n\r",
        IMCPERM_IMM,
        "*"
    }
};

#define numchannels ( sizeof( imc_channels ) / sizeof( imc_channels[0] ) )
/*******************************************
 * String buffering and logging functions. *
 ******************************************/

/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 *
 * Renamed so it can play itself system independent.
 * Samson 10-12-03
 */
size_t imcstrlcpy( char *dst, const char *src, size_t siz )
{
    register char *d = dst;
    register const char *s = src;
    register size_t n = siz;

    /* Copy as many bytes as will fit */
    if( n != 0 && --n != 0 )
    {
        do
        {
            if( ( *d++ = *s++ ) == 0 )
                break;
        }
        while( --n != 0 );
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if( n == 0 )
    {
        if( siz != 0 )
            *d = '\0';                                      /* NUL-terminate dst */
        while( *s++ )
            ;
    }
    return( s - src - 1 );                                  /* count does not include NUL */
}

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(initial dst) + strlen(src); if retval >= siz,
 * truncation occurred.
 *
 * Renamed so it can play itself system independent.
 * Samson 10-12-03
 */
size_t imcstrlcat( char *dst, const char *src, size_t siz )
{
    register char *d = dst;
    register const char *s = src;
    register size_t n = siz;
    size_t dlen;

    /* Find the end of dst and adjust bytes left but don't go past end */
    while( n-- != 0 && *d != '\0' )
        d++;
    dlen = d - dst;
    n = siz - dlen;

    if( n == 0 )
        return( dlen + strlen(s) );
    while( *s != '\0' )
    {
        if( n != 1 )
        {
            *d++ = *s;
            n--;
        }
        s++;
    }
    *d = '\0';
    return( dlen + ( s - src ) );                           /* count does not include NUL */
}

char *imc_crypt( const char *pwd )
{
    md5_state_t state;
    md5_byte_t digest[16];
    static char passwd[16];
    unsigned int x;

    md5_init( &state );
    md5_append( &state, (const md5_byte_t *)pwd, strlen( pwd ) );
    md5_finish( &state, digest );

    imcstrlcpy( passwd, (const char *)digest, 16 );

    /* The listed exceptions below will fubar the MD5 authentication packets, so change them */
    for( x = 0; x < strlen( passwd ); x++ )
    {
        if( passwd[x] == '\n' )
            passwd[x] = 'n';
        if( passwd[x] == '\r' )
            passwd[x] = 'r';
        if( passwd[x] == '\t' )
            passwd[x] = 't';
        if( passwd[x] == ' ' )
            passwd[x] = 's';
        if( (int)passwd[x] == 11 )
            passwd[x] = 'x';
        if( (int)passwd[x] == 12 )
            passwd[x] = 'X';
    }
    return( passwd );
}

/* Generic log function which will route the log messages to the appropriate system logging function */
void imclog( const char *format, ... )
{
    char buf[LGST], buf2[LGST];
    va_list ap;

    va_start( ap, format );
    vsnprintf( buf, LGST, format, ap );
    va_end( ap );

    snprintf( buf2, LGST, "IMC: %s", buf );

    #ifndef IMCCIRCLE
    log_string( buf2 );
    #else
    basic_mud_log( "%s", buf2 );
    #endif
    return;
}

/* Generic bug logging function which will route the message to the appropriate function that handles bug logs */
void imcbug( const char *format, ... )
{
    char buf[LGST], buf2[LGST];
    va_list ap;

    va_start( ap, format );
    vsnprintf( buf, LGST, format, ap );
    va_end( ap );

    snprintf( buf2, LGST, "IMC: %s", buf );

    #if defined(IMCSMAUG) || defined(IMCCHRONICLES)
    bug( "%s", buf2 );
    #elif defined(IMCCIRCLE)
    basic_mud_log( "%s", buf2 );
    #else
    bug( buf2, 0 );
    #endif
    return;
}

/*
   Original Code from SW:FotE 1.1
   Reworked strrep function.
   Fixed a few glaring errors. It also will not overrun the bounds of a string.
   -- Xorith
*/
char *imcstrrep( const char *src, const char *sch, const char *rep )
{
    int lensrc = strlen( src ), lensch = strlen( sch ), lenrep = strlen( rep ), x, y, in_p;
    static char newsrc[IMC_DATA_LENGTH];
    bool searching = FALSE;

    newsrc[0] = '\0';
    for( x = 0, in_p = 0; x < lensrc; x++, in_p++ )
    {
        if( src[x] == sch[0] )
        {
            searching = TRUE;
            for( y = 0; y < lensch; y++ )
                if( src[x+y] != sch[y] )
                    searching = FALSE;

            if( searching )
            {
                for( y = 0; y < lenrep; y++, in_p++ )
                {
                    if( in_p == ( IMC_DATA_LENGTH - 1 ) )
                    {
                        newsrc[in_p] = '\0';
                        return newsrc;
                    }
                    if( src[x-1] == sch[0] )
                    {
                        if( rep[0] == '\033' )
                        {
                            if( y < lensch )
                            {
                                if( y == 0 )
                                    newsrc[in_p-1] = sch[y];
                                else
                                    newsrc[in_p] = sch[y];
                            }
                            else
                                y = lenrep;
                        }
                        else
                        {
                            if( y == 0 )
                                newsrc[in_p-1] = rep[y];
                            newsrc[in_p] = rep[y];
                        }
                    }
                    else
                        newsrc[in_p] = rep[y];
                }
                x += lensch - 1;
                in_p--;
                searching = FALSE;
                continue;
            }
        }
        if( in_p == ( IMC_DATA_LENGTH - 1 ) )
        {
            newsrc[in_p] = '\0';
            return newsrc;
        }
        newsrc[in_p] = src[x];
    }
    newsrc[in_p] = '\0';
    return newsrc;
}

char *imcone_argument( char *argument, char *arg_first )
{
    char cEnd;
    int count;

    count = 0;

    while( isspace(*argument) )
        argument++;

    cEnd = ' ';
    if( *argument == '\'' || *argument == '"' )
        cEnd = *argument++;

    while( *argument != '\0' || ++count >= 255 )
    {
        if( *argument == cEnd )
        {
            argument++;
            break;
        }
        *arg_first = (*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while( isspace(*argument) )
        argument++;

    return argument;
}

/********************************
 * User level output functions. *
 *******************************/

char *imc_strip_colors( const char *txt )
{
    IMC_COLOR *color;
    static char tbuf[IMC_DATA_LENGTH];

    imcstrlcpy( tbuf, txt, IMC_DATA_LENGTH );
    for( color = first_imc_color; color; color = color->next )
        imcstrlcpy( tbuf, imcstrrep( tbuf, color->imctag, "" ), IMC_DATA_LENGTH );

    for( color = first_imc_color; color; color = color->next )
        imcstrlcpy( tbuf, imcstrrep( tbuf, color->mudtag, "" ), IMC_DATA_LENGTH );
    return tbuf;
}

/* Now tell me this isn't cleaner than the mess that was here before. -- Xorith */
/* Yes, Xorith it is. Now, how about this update? Much less hassle with no hardcoded table! -- Samson */
/* convert from imc color -> mud color */
char *color_itom( const char *txt, CHAR_DATA *ch )
{
    IMC_COLOR *color;
    static char tbuf[IMC_DATA_LENGTH];

    if( !txt || *txt == '\0' )
        return "";

    if( IMCIS_SET( IMCFLAG(ch), IMC_COLORFLAG ) )
    {
        imcstrlcpy( tbuf, txt, IMC_DATA_LENGTH );
        for( color = first_imc_color; color; color = color->next )
            imcstrlcpy( tbuf, imcstrrep( tbuf, color->imctag, color->mudtag ), IMC_DATA_LENGTH );
    }
    else
        imcstrlcpy( tbuf, imc_strip_colors( txt ), IMC_DATA_LENGTH );

    return tbuf;
}

/* convert from mud color -> imc color */
char *color_mtoi( const char *txt )
{
    IMC_COLOR *color;
    static char tbuf[IMC_DATA_LENGTH];

    if( !txt || *txt == '\0' )
        return "";

    imcstrlcpy( tbuf, txt, IMC_DATA_LENGTH );
    for( color = first_imc_color; color; color = color->next )
        imcstrlcpy( tbuf, imcstrrep( tbuf, color->mudtag, color->imctag ), IMC_DATA_LENGTH );

    return tbuf;
}

/* Generic send_to_char type function to send to the proper code for each codebase */
void imc_to_char( const char *txt, CHAR_DATA *ch )
{
    char buf[LGST*2];

    snprintf( buf, LGST*2, "%s\033[0m", color_itom( txt, ch ) );
    #if defined(IMCSMAUG)
    send_to_char_color( buf, ch );
    #elif defined(IMCCIRCLE)
    #if _CIRCLEMUD < CIRCLEMUD_VERSION(3,0,21)
    send_to_char( buf, ch );
    #else
    send_to_char( ch, "%s", buf );
    #endif
    #else
    send_to_char( buf, ch );
    #endif
    return;
}

/* Modified version of Smaug's ch_printf_color function */
void imc_printf( CHAR_DATA *ch, const char *fmt, ... )
{
    char buf[LGST];
    va_list args;

    va_start( args, fmt );
    vsnprintf( buf, LGST, fmt, args );
    va_end( args );

    imc_to_char( buf, ch );
}

/* Generic send_to_pager type function to send to the proper code for each codebase */
void imc_to_pager( const char *txt, CHAR_DATA *ch )
{
    char buf[LGST*2];

    snprintf( buf, LGST*2, "%s\033[0m", color_itom( txt, ch ) );
    #if defined(IMCSMAUG) || defined(IMCCHRONICLES)
    send_to_pager_color( buf, ch );
    #elif defined(IMCROM)
    page_to_char( buf, ch );
    #elif defined(IMC1STMUD)
    sendpage( ch, buf2 );
    #else
    imc_to_char( buf, ch );
    #endif
    return;
}

/* Generic pager_printf type function */
void imcpager_printf( CHAR_DATA *ch, const char *fmt, ... )
{
    char buf[LGST];
    va_list args;

    va_start( args, fmt );
    vsnprintf( buf, LGST, fmt, args );
    va_end( args );

    imc_to_pager( buf, ch );
    return;
}

/********************************
 * Low level utility functions. *
 ********************************/

bool imcstr_prefix( const char *astr, const char *bstr )
{
    if( !astr )
    {
        imcbug( "Strn_cmp: null astr." );
        return TRUE;
    }

    if( !bstr )
    {
        imcbug( "Strn_cmp: null bstr." );
        return TRUE;
    }

    for( ; *astr; astr++, bstr++ )
    {
        if( LOWER(*astr) != LOWER(*bstr) )
            return TRUE;
    }
    return FALSE;
}

/*
 * Returns an initial-capped string.
 */
char *imccapitalize( const char *str )
{
    static char strcap[LGST];
    int i;

    for( i = 0; str[i] != '\0'; i++ )
        strcap[i] = tolower( str[i] );
    strcap[i] = '\0';
    strcap[0] = toupper( strcap[0] );
    return strcap;
}

/*  imc_getarg: extract a single argument (with given max length) from
 *  argument to arg; if arg==NULL, just skip an arg, don't copy it out
 */
char *imc_getarg( char *argument, char *arg, int length )
{
    int len = 0;

    if( !argument || argument[0] == '\0' )
    {
        if( arg )
            arg[0] = '\0';

        return argument;
    }

    while( *argument && isspace( *argument ) )
        argument++;

    if( arg )
        while( *argument && !isspace( *argument ) && len < length-1 )
            *arg++ = *argument++, len++;
    else
        while( *argument && !isspace( *argument ) )
            argument++;

    while( *argument && !isspace( *argument ) )
        argument++;

    while( *argument && isspace( *argument ) )
        argument++;

    if( arg )
        *arg = '\0';

    return argument;
}

/* Check for a name in a list */
bool imc_hasname( char *list, char *name )
{
    char *p;
    char arg[IMC_NAME_LENGTH];

    if( !list )
        return FALSE;

    p = imc_getarg( list, arg, IMC_NAME_LENGTH );
    while( arg[0] )
    {
        if( !strcasecmp( name, arg ) )
            return TRUE;
        p = imc_getarg( p, arg, IMC_NAME_LENGTH );
    }
    return FALSE;
}

/* Add a name to a list */
void imc_addname( char **list, char *name )
{
    char buf[IMC_DATA_LENGTH];

    if( imc_hasname( *list, name ) )
        return;

    if( (*list) && (*list)[0] != '\0' )
        snprintf( buf, IMC_DATA_LENGTH, "%s %s", *list, name );
    else
        imcstrlcpy( buf, name, IMC_DATA_LENGTH );

    IMCSTRFREE( *list );
    *list = IMCSTRALLOC( buf );
}

/* Remove a name from a list */
void imc_removename( char **list, char *name )
{
    char buf[1000];
    char arg[IMC_NAME_LENGTH];
    char *p;

    buf[0] = '\0';
    p = imc_getarg( *list, arg, IMC_NAME_LENGTH );
    while( arg[0] )
    {
        if( strcasecmp( arg, name ) )
        {
            if( buf[0] )
                imcstrlcat( buf, " ", 1000 );
            imcstrlcat( buf, arg, 1000 );
        }
        p = imc_getarg( p, arg, IMC_NAME_LENGTH );
    }
    IMCSTRFREE( *list );
    *list = IMCSTRALLOC( buf );
}

/* escape2: escape " -> \", \ -> \\, CR -> \r, LF -> \n */
char *escape2( char *data )
{
    static char buf[IMC_DATA_LENGTH];
    char *p;

    for( p = buf; *data && ( p-buf < IMC_DATA_LENGTH-1 ); data++, p++ )
    {
        if( *data == '\n' )
        {
            *p++ = '\\';
            *p = 'n';
        }
        else if( *data == '\r' )
        {
            *p++ = '\\';
            *p = 'r';
        }
        else if( *data == '\\' )
        {
            *p++ = '\\';
            *p = '\\';
        }
        else if( *data == '"' )
        {
            *p++ = '\\';
            *p = '"';
        }
        else
            *p = *data;
    }
    *p = 0;

    return buf;
}

/* free all the keys in "p" */
void imc_freedata( PACKET *p )
{
    int i;

    for( i = 0; i < IMC_MAX_KEYS; i++ )
    {
        IMCSTRFREE( p->key[i] );
        IMCSTRFREE( p->value[i] );
    }
    return;
}

/* get the value of "key" from "p"; if it isn't present, return "def" */
char *imc_getkey( PACKET *p, char *key, char *def )
{
    int i;

    for( i = 0; i < IMC_MAX_KEYS; i++ )
        if( p->key[i] && !strcasecmp( p->key[i], key ) )
            return p->value[i];

    return def;
}

/* identical to imc_getkey, except get the integer value of the key */
int imc_getkeyi( PACKET *p, char *key, int def )
{
    int i;

    for( i = 0; i < IMC_MAX_KEYS; i++ )
        if( p->key[i] && !strcasecmp( p->key[i], key ) )
            return atoi( p->value[i] );

    return def;
}

/* add "key=value" to "p" */
void imc_addkey( PACKET *p, char *key, char *value )
{
    int i;

    for( i = 0; i < IMC_MAX_KEYS; i++ )
    {
        if( p->key[i] && !strcasecmp( key, p->key[i] ) )
        {
            IMCSTRFREE( p->key[i] );
            IMCSTRFREE( p->value[i] );
            break;
        }
    }
    if( !value )
        return;

    for( i = 0; i < IMC_MAX_KEYS; i++ )
    {
        if( !p->key[i] )
        {
            p->key[i]   = IMCSTRALLOC( key );
            p->value[i] = IMCSTRALLOC( value );
            return;
        }
    }
}

/* add "key=value" for an integer value */
void imc_addkeyi( PACKET *p, char *key, int value )
{
    char temp[20];
    snprintf( temp, 20, "%d", value );
    imc_addkey( p, key, temp );
}

/* clone packet data */
void imc_clonedata( PACKET *p, PACKET *n )
{
    int i;

    for( i = 0; i < IMC_MAX_KEYS; i++ )
    {
        if( p->key[i] )
            n->key[i] = IMCSTRALLOC( p->key[i] );
        else
            n->key[i] = NULL;

        if( p->value[i] )
            n->value[i] = IMCSTRALLOC( p->value[i] );
        else
            n->value[i] = NULL;
    }
}

/* clear all keys in "p" */
void imc_initdata( PACKET *p )
{
    int i;

    for( i = 0; i < IMC_MAX_KEYS; i++ )
    {
        p->key[i]   = NULL;
        p->value[i] = NULL;
    }
}

/* printkeys: print key-value pairs, escaping values */
char *printkeys( PACKET *data )
{
    static char buf[IMC_DATA_LENGTH];
    char temp[IMC_DATA_LENGTH];
    int len = 0;
    int i;

    buf[0] = '\0';

    for( i = 0; i < IMC_MAX_KEYS; i++ )
    {
        if( !data->key[i] )
            continue;
        imcstrlcpy( buf + len, data->key[i], IMC_DATA_LENGTH-len-1 );
        imcstrlcat( buf, "=", IMC_DATA_LENGTH );
        len = strlen( buf );

        if( !strchr( data->value[i], ' ' ) )
            imcstrlcpy( temp, escape2( data->value[i] ), IMC_DATA_LENGTH-1 );
        else
        {
            temp[0] = '"';
            imcstrlcpy( temp+1, escape2( data->value[i] ), IMC_DATA_LENGTH-3 );
            imcstrlcat( temp, "\"", IMC_DATA_LENGTH );
        }
        imcstrlcat( temp, " ", IMC_DATA_LENGTH );
        imcstrlcpy( buf + len, temp, IMC_DATA_LENGTH-len );
        len = strlen( buf );
    }
    return buf;
}

/* parsekeys: extract keys from string */
void parsekeys( const char *string, PACKET *data )
{
    const char *p1;
    char *p2;
    char k[IMC_DATA_LENGTH], v[IMC_DATA_LENGTH];
    int quote;

    p1 = string;

    while( *p1 )
    {
        while( *p1 && isspace( *p1 ) )
            p1++;

        p2 = k;
        while( *p1 && *p1 != '=' && p2-k < IMC_DATA_LENGTH-1 )
            *p2++ = *p1++;
        *p2=0;

        if( !k[0] || !*p1 )                                 /* no more keys? */
            break;

        p1++;                                               /* skip the '=' */

        if( *p1 == '"' )
        {
            p1++;
            quote = 1;
        }
        else
            quote = 0;

        p2 = v;
        while( *p1 && ( !quote || *p1 != '"' ) && ( quote || *p1 != ' ' ) && p2-v < IMC_DATA_LENGTH+1 )
        {
            if( *p1 == '\\' )
            {
                switch( *(++p1) )
                {
                    case '\\':
                        *p2++ = '\\';
                        break;
                    case 'n':
                        *p2++ = '\n';
                        break;
                    case 'r':
                        *p2++ = '\r';
                        break;
                    case '"':
                        *p2++ = '"';
                        break;
                    default:
                        *p2++ = *p1;
                        break;
                }
                if( *p1 )
                    p1++;
            }
            else
                *p2++ = *p1++;
        }
        *p2 = 0;

        if( !v[0] )
            continue;

        imc_addkey( data, k, v );

        if( quote && *p1 )
            p1++;
    }
}

char *generate2( PACKET *p )
{
    static char temp[IMC_PACKET_LENGTH];
    char newpath[IMC_PATH_LENGTH];

    if( !p->type[0] || !p->i.from[0] || !p->i.to[0] )
    {
        imcbug( "%s", "generate2: bad packet!" );
        imcbug( "Type: %s From: %s To: %s", p->type, p->i.from, p->i.to );
        imcbug( "Path: %s Data: %s", p->i.path, printkeys( p ) );
        return NULL;                                        /* catch bad packets here */
    }

    if( !p->i.path[0] )
        imcstrlcpy( newpath, this_imcmud->localname, IMC_PATH_LENGTH );
    else
        snprintf( newpath, IMC_PATH_LENGTH, "%s!%s", p->i.path, this_imcmud->localname );

    snprintf( temp, IMC_PACKET_LENGTH, "%s %lu %s %s %s %s", p->i.from, p->i.sequence, newpath, p->type, p->i.to, printkeys( p ) );
    return temp;
}

PACKET *interpret2( char *argument )
{
    char seq[20];
    static PACKET out;

    imc_initdata( &out );
    argument = imc_getarg( argument, out.i.from, IMC_NAME_LENGTH );
    argument = imc_getarg( argument, seq, 20 );
    argument = imc_getarg( argument, out.i.path, IMC_PATH_LENGTH );
    argument = imc_getarg( argument, out.type, IMC_TYPE_LENGTH );
    argument = imc_getarg( argument, out.i.to, IMC_NAME_LENGTH );

    if( !out.i.from[0] || !seq[0] || !out.i.path[0] || !out.type[0] || !out.i.to[0] )
    {
        imcbug( "%s", "interpret2: bad packet received, discarding" );
        imcbug( "Type: %s From: %s To: %s", out.type, out.i.from, out.i.to );
        imcbug( "Path: %s Seq: %s", out.i.path, seq );
        return NULL;
    }

    parsekeys( argument, &out );

    out.i.sequence = strtoul( seq, NULL, 10 );
    return &out;
}

_imc_vinfo imc_vinfo[] =
{
    { 0, NULL, NULL },
    { 1, NULL, NULL },
    { 2, generate2, interpret2 }
};

/* checkrepeat: check for repeats in the memory table */
int checkrepeat( char *mud, unsigned long seq )
{
    int i;

    for( i = 0; i < IMC_MEMORY; i++ )
        if( imc_memory[i].from && !strcasecmp( mud, imc_memory[i].from ) && seq == imc_memory[i].sequence )
            return 1;

    /* not a repeat, so log it */
    IMCSTRFREE( imc_memory[memory_head].from );

    imc_memory[memory_head].from     = IMCSTRALLOC( mud );
    imc_memory[memory_head].sequence = seq;

    memory_head++;
    if( memory_head == IMC_MEMORY )
        memory_head = 0;

    return 0;
}

/* return 1 if 'name' is a part of 'path'  (internal) */
int inpath( char *path, char *name )
{
    char buf[IMC_MNAME_LENGTH+3];
    char tempn[IMC_MNAME_LENGTH], tempp[IMC_PATH_LENGTH];

    imcstrlcpy( tempn, name, IMC_MNAME_LENGTH );
    imcstrlcpy( tempp, path, IMC_PATH_LENGTH );

    if( !strcasecmp( tempp, tempn ) )
        return 1;

    snprintf( buf, IMC_MNAME_LENGTH+3, "%s!", tempn );
    if( !strncmp( tempp, buf, strlen( buf ) ) )
        return 1;

    snprintf( buf, IMC_MNAME_LENGTH+3, "!%s", tempn );
    if( strlen(buf) < strlen(tempp) && !strcasecmp( tempp + strlen(tempp) - strlen(buf), buf ) )
        return 1;

    snprintf( buf, IMC_MNAME_LENGTH+3, "!%s!", tempn );
    if( strstr( tempp, buf ) )
        return 1;

    return 0;
}

/* return 'e' from 'a!b!c!d!e' */
char *imc_lastinpath( char *path )
{
    const char *where;
    static char buf[IMC_NAME_LENGTH];

    where = path + strlen(path)-1;
    while( *where != '!' && where >= path )
        where--;

    imcstrlcpy( buf, where+1, IMC_NAME_LENGTH );
    return buf;
}

/* return 'a' from 'a!b!c!d!e' */
char *imc_firstinpath( char *path )
{
    static char buf[IMC_NAME_LENGTH];
    char *p;

    for( p = buf; *path && *path != '!'; *p++ = *path++ )
        ;

    *p = 0;
    return buf;
}

/* convert from the char data in 'p' to an internal representation in 'd' */
void getdata( PACKET *p, imc_char_data *d )
{
    imcstrlcpy( d->name, p->from, IMC_NAME_LENGTH );
    d->level = imc_getkeyi( p, "level", 0 );
}

/* convert back from 'd' to 'p' */
void setdata( PACKET *p, imc_char_data *d )
{
    imc_initdata( p );

    if( !d )
    {
        imcstrlcpy( p->from, "*", IMC_NAME_LENGTH );
        imc_addkeyi( p, "level", -1 );
        return;
    }

    imcstrlcpy( p->from, d->name, IMC_NAME_LENGTH );
    imc_addkeyi( p, "level", d->level );
}

imc_char_data *imc_getdata( CHAR_DATA *ch )
{
    imc_char_data *d;

    IMCCREATE( d, imc_char_data, 1 );
    if( !ch )                                               /* fake system character */
    {
        d->level = IMCPERM_NONE;
        imcstrlcpy( d->name, "*", IMC_NAME_LENGTH );
        return d;
    }

    d->level = IMCPERM(ch);
    imcstrlcpy( d->name, CH_IMCNAME(ch), IMC_NAME_LENGTH );

    return d;
}

/*
 * Returns a CHAR_DATA structure which matches the string
 */
CHAR_DATA *imc_find_user( char *name )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *vch = NULL;

    for ( d = first_descriptor; d; d = d->next )
    {
        if( ( vch = d->character ? d->character : d->original ) != NULL && !strcasecmp( CH_IMCNAME(vch), name )
            && d->connected == CON_PLAYING )
            return vch;
    }
    return NULL;
}

/* return 'player' from 'player@mud' */
char *imc_nameof( char *fullname )
{
    static char buf[IMC_PNAME_LENGTH];
    char *where = buf;
    int count = 0;

    while( *fullname && *fullname != '@' && count < IMC_PNAME_LENGTH-1 )
        *where++ = *fullname++, count++;

    *where = 0;
    return buf;
}

/* return 'mud' from 'player@mud' */
char *imc_mudof( char *fullname )
{
    static char buf[IMC_MNAME_LENGTH];
    char *where;

    where = strchr( fullname, '@' );
    if( !where )
        imcstrlcpy( buf, fullname, IMC_MNAME_LENGTH );
    else
        imcstrlcpy( buf, where+1, IMC_MNAME_LENGTH );

    return buf;
}

char *channel_mudof( char *fullname )
{
    static char buf[IMC_PNAME_LENGTH];
    char *where = buf;
    int count=0;

    while( *fullname && *fullname != ':' && count < IMC_PNAME_LENGTH-1 )
        *where++ = *fullname++, count++;

    *where = 0;
    return buf;
}

/* return 'player@mud' from 'player' and 'mud' */
char *imc_makename( char *player, char *mud, bool social )
{
    static char buf[IMC_NAME_LENGTH];

    imcstrlcpy( buf, player, IMC_PNAME_LENGTH );

    /* The social code gets wierd if "mud" isn't specified, so bypass it if it's not */
    if( mud && mud[0] != '\0' && !social )
    {
        imcstrlcat( buf, "@", IMC_NAME_LENGTH );
        imcstrlcpy( buf + strlen(buf), mud, IMC_MNAME_LENGTH );
    }
    return buf;
}

char *getname( CHAR_DATA *ch, imc_char_data *vict )
{
    static char buf[IMC_NAME_LENGTH];
    char *mud, *name;
    imc_char_data *chdata = imc_getdata( ch );

    mud = imc_mudof( vict->name );
    name = imc_nameof( vict->name );

    if( !strcasecmp( mud, this_imcmud->localname ) )
        snprintf( buf, IMC_NAME_LENGTH, "%s", imc_nameof( name ) );
    else
        imcstrlcpy( buf, vict->name, IMC_NAME_LENGTH );
    IMCDISPOSE( chdata );
    return buf;
}

/* check if a packet from a given source should be ignored */
bool imc_isbanned( char *who )
{
    IMC_BAN *mud;

    for( mud = first_imc_ban; mud; mud = mud->next )
    {
        if( !strcasecmp( mud->name, imc_mudof( who ) ) )
            return TRUE;
    }
    return FALSE;
}

/* Beefed up to include wildcard ignores. */
bool imc_isignoring( CHAR_DATA *ch, const char *ignore )
{
    IMC_IGNORE *temp;

    /* Wildcard support thanks to Xorith */
    for( temp = FIRST_IMCIGNORE(ch); temp; temp = temp->next )
    {
        if( !fnmatch( temp->name, ignore, 0 ) )
            return TRUE;
    }
    return FALSE;
}

/* need exactly 2 %s's, and no other format specifiers */
bool verify_format( const char *fmt, short sneed )
{
    const char *c;
    int i = 0;

    c = fmt;
    while( ( c = strchr( c, '%' ) ) != NULL )
    {
        if( *( c + 1 ) == '%' )                             /* %% */
        {
            c += 2;
            continue;
        }

        if( *( c + 1 ) != 's' )                             /* not %s */
            return FALSE;

        c++;
        i++;
    }
    if( i != sneed )
        return FALSE;

    return TRUE;
}

/* There should only one of these..... */
void imc_delete_info( void )
{
    int i;

    for( i = 0; i < IMC_MEMORY; i++ )
        IMCSTRFREE( imc_memory[i].from );

    IMCSTRFREE( this_imcmud->routername );
    IMCSTRFREE( this_imcmud->rhost );
    IMCSTRFREE( this_imcmud->network );
    IMCSTRFREE( this_imcmud->clientpw );
    IMCSTRFREE( this_imcmud->serverpw );
    IMCDISPOSE( this_imcmud->outbuf );
    IMCDISPOSE( this_imcmud->inbuf );
    IMCSTRFREE( this_imcmud->localname );
    IMCSTRFREE( this_imcmud->fullname );
    IMCSTRFREE( this_imcmud->ihost );
    IMCSTRFREE( this_imcmud->email );
    IMCSTRFREE( this_imcmud->www );
    IMCSTRFREE( this_imcmud->details );
    IMCSTRFREE( this_imcmud->base );
    IMCDISPOSE( this_imcmud );
    IMCSTRFREE( IMC_VERSIONID );
}

/* delete the info entry "p" */
void imc_delete_reminfo( REMOTEINFO *p )
{
    IMCUNLINK( p, first_rinfo, last_rinfo, next, prev );
    IMCSTRFREE( p->name );
    IMCSTRFREE( p->version );
    IMCSTRFREE( p->network );
    IMCSTRFREE( p->path );
    IMCSTRFREE( p->url );
    IMCDISPOSE( p );
}

/* create a new info entry, insert into list */
REMOTEINFO *imc_new_reminfo( char *mud )
{
    REMOTEINFO *p, *mud_prev;

    IMCCREATE( p, REMOTEINFO, 1 );

    p->name    = IMCSTRALLOC( mud );
    p->url     = IMCSTRALLOC( "Unknown" );
    p->version = NULL;
    p->path   = NULL;
    p->top_sequence = 0;
    p->expired = FALSE;

    for( mud_prev = first_rinfo; mud_prev; mud_prev = mud_prev->next )
        if( strcasecmp( mud_prev->name, mud ) >= 0 )
            break;

    if( !mud_prev )
        IMCLINK( p, first_rinfo, last_rinfo, next, prev );
    else
        IMCINSERT( p, mud_prev, first_rinfo, next, prev );

    return p;
}

/* find an info entry for "name" */
REMOTEINFO *imc_find_reminfo( char *name )
{
    REMOTEINFO *p;

    for( p = first_rinfo; p; p = p->next )
    {
        if( !strcasecmp( name, p->name ) )
            return p;
    }
    return NULL;
}

bool check_mud( CHAR_DATA *ch, char *mud )
{
    REMOTEINFO *r = imc_find_reminfo( mud );

    if( !r )
    {
        imc_printf( ch, "~W%s ~cis not a valid mud name.\n\r", mud );
        return FALSE;
    }

    if( r->expired )
    {
        imc_printf( ch, "~W%s ~cis not connected right now.\n\r", r->name );
        return FALSE;
    }
    return TRUE;
}

bool check_mudof( CHAR_DATA *ch, char *mud )
{
    return check_mud( ch, imc_mudof( mud ) );
}

int get_imcpermvalue( const char *flag )
{
    unsigned int x;

    for( x = 0; x < (sizeof(imcperm_names) / sizeof(imcperm_names[0])); x++ )
        if( !strcasecmp(flag, imcperm_names[x]) )
            return x;
    return -1;
}

bool imccheck_permissions( CHAR_DATA *ch, int checkvalue, int targetvalue, bool enforceequal )
{
    if( checkvalue < 0 || checkvalue > IMCPERM_IMP )
    {
        imc_to_char( "Invalid permission setting.\n\r", ch );
        return FALSE;
    }

    if( checkvalue > IMCPERM(ch) )
    {
        imc_to_char( "You cannot set permissions higher than your own.\n\r", ch );
        return FALSE;
    }

    if( checkvalue == IMCPERM(ch) && IMCPERM(ch) != IMCPERM_IMP && enforceequal )
    {
        imc_to_char( "You cannot set permissions equal to your own. Someone higher up must do this.\n\r", ch );
        return FALSE;
    }

    if( IMCPERM(ch) < targetvalue )
    {
        imc_to_char( "You cannot alter the permissions of someone or something above your own.\n\r", ch );
        return FALSE;
    }
    return TRUE;
}

IMC_BAN *imc_newban( void )
{
    IMC_BAN *ban;

    IMCCREATE( ban, IMC_BAN, 1 );
    ban->name = NULL;
    IMCLINK( ban, first_imc_ban, last_imc_ban, next, prev );
    return ban;
}

void imc_addban( char *what )
{
    IMC_BAN *ban;

    ban = imc_newban();
    ban->name = IMCSTRALLOC( what );
}

void imc_freeban( IMC_BAN *ban )
{
    IMCSTRFREE( ban->name );
    IMCUNLINK( ban, first_imc_ban, last_imc_ban, next, prev );
    IMCDISPOSE( ban );
}

bool imc_delban( char *what )
{
    IMC_BAN *ban, *ban_next;

    for( ban = first_imc_ban; ban; ban = ban_next )
    {
        ban_next = ban->next;
        if( !strcasecmp( what, ban->name ) )
        {
            imc_freeban( ban );
            return TRUE;
        }
    }
    return FALSE;
}

IMC_CHANNEL *imc_findchannel( char *name )
{
    IMC_CHANNEL *c;

    for( c = first_imc_channel; c; c = c->next )
        if( !strcasecmp( c->name, name ) )
            return c;
    return NULL;
}

IMC_CHANNEL *imc_findlchannel( char *name )
{
    IMC_CHANNEL *c;

    for( c = first_imc_channel; c; c = c->next )
        if( c->local_name && !strcasecmp( c->local_name, name ) )
            return c;
    return NULL;
}

void imc_freechan( IMC_CHANNEL *c )
{
    int x;

    if( !c )                                                /* How? */
    {
        imcbug( "%s", "imc_freechan: Freeing NULL channel!" );
        return;
    }
    IMCUNLINK( c, first_imc_channel, last_imc_channel, next, prev );
    IMCSTRFREE( c->name );
    IMCSTRFREE( c->owner );
    IMCSTRFREE( c->operators );
    IMCSTRFREE( c->invited );
    IMCSTRFREE( c->excluded );
    IMCSTRFREE( c->local_name );
    IMCSTRFREE( c->regformat );
    IMCSTRFREE( c->emoteformat );
    IMCSTRFREE( c->socformat );
    IMCSTRFREE( c->dlname );

    for( x = 0; x < MAX_IMCHISTORY; x++ )
        IMCSTRFREE( c->history[x] );
    IMCDISPOSE( c );
    return;
}

void imc_shutdownchannels( void )
{
    IMC_CHANNEL *ic, *icnext;

    for( ic = first_imc_channel; ic; ic = icnext )
    {
        icnext = ic->next;

        imc_freechan( ic );
    }
}

/*
 * Read a number from a file. [Taken from Smaug's fread_number]
 */
int imcfread_number( FILE *fp )
{
    int number;
    bool sign;
    char c;

    do
    {
        if ( feof(fp) )
        {
            imclog( "%s", "imcfread_number: EOF encountered on read." );
            return 0;
        }
        c = getc( fp );
    }
    while ( isspace(c) );

    number = 0;

    sign   = FALSE;
    if ( c == '+' )
    {
        c = getc( fp );
    }
    else if ( c == '-' )
    {
        sign = TRUE;
        c = getc( fp );
    }

    if ( !isdigit(c) )
    {
        imclog( "imcfread_number: bad format. (%c)", c );
        return 0;
    }

    while ( isdigit(c) )
    {
        if ( feof(fp) )
        {
            imclog( "%s", "imcfread_number: EOF encountered on read." );
            return number;
        }
        number = number * 10 + c - '0';
        c      = getc( fp );
    }

    if ( sign )
        number = 0 - number;

    if ( c == '|' )
        number += imcfread_number( fp );
    else if ( c != ' ' )
        ungetc( c, fp );

    return number;
}

/*
 * Read to end of line into static buffer [Taken from Smaug's fread_line]
 */
char *imcfread_line( FILE *fp )
{
    char fline[LGST];
    char *pline;
    char c;
    int ln;

    pline = fline;
    fline[0] = '\0';
    ln = 0;

    /*
     * Skip blanks.
     * Read first char.
     */
    do
    {
        if( feof(fp) )
        {
            imcbug( "%s", "imcfread_line: EOF encountered on read." );
            imcstrlcpy( fline, "", LGST );
            return IMCSTRALLOC( fline );
        }
        c = getc( fp );
    }
    while( isspace(c) );

    ungetc( c, fp );

    do
    {
        if( feof(fp) )
        {
            imcbug( "%s", "imcfread_line: EOF encountered on read." );
            *pline = '\0';
            return IMCSTRALLOC( fline );
        }
        c = getc( fp );
        *pline++ = c; ln++;
        if( ln >= (LGST - 1) )
        {
            imcbug( "%s", "imcfread_line: line too long" );
            break;
        }
    }
    while( c != '\n' && c != '\r' );

    do
    {
        c = getc( fp );
    }
    while( c == '\n' || c == '\r' );

    ungetc( c, fp );
    pline--;
    *pline = '\0';

    /* Since tildes generally aren't found at the end of lines, this seems workable. Will enable reading old configs. */
    if( fline[strlen(fline)-1] == '~' )
        fline[strlen(fline)-1] = '\0';
    return IMCSTRALLOC( fline );
}

/*
 * Read one word (into static buffer). [Taken from Smaug's fread_word]
 */
char *imcfread_word( FILE *fp )
{
    static char word[SMST];
    char *pword;
    char cEnd;

    do
    {
        if( feof(fp) )
        {
            imclog( "%s", "imcfread_word: EOF encountered on read." );
            word[0] = '\0';
            return word;
        }
        cEnd = getc( fp );
    }
    while( isspace( cEnd ) );

    if( cEnd == '\'' || cEnd == '"' )
    {
        pword   = word;
    }
    else
    {
        word[0] = cEnd;
        pword   = word+1;
        cEnd    = ' ';
    }

    for( ; pword < word + SMST; pword++ )
    {
        if( feof(fp) )
        {
            imclog( "%s", "imcfread_word: EOF encountered on read." );
            *pword = '\0';
            return word;
        }
        *pword = getc( fp );
        if( cEnd == ' ' ? isspace(*pword) : *pword == cEnd )
        {
            if( cEnd == ' ' )
                ungetc( *pword, fp );
            *pword = '\0';
            return word;
        }
    }
    imclog( "%s", "imcfread_word: word too long" );
    return NULL;
}

/*
 * Read to end of line (for comments). [Taken from Smaug's fread_to_eol]
 */
void imcfread_to_eol( FILE *fp )
{
    char c;

    do
    {
        if( feof(fp) )
        {
            imclog( "%s", "imcfread_to_eol: EOF encountered on read." );
            return;
        }
        c = getc( fp );
    }
    while( c != '\n' && c != '\r' );

    do
    {
        c = getc( fp );
    }
    while( c == '\n' || c == '\r' );

    ungetc( c, fp );
    return;
}

/*
 * Read a letter from a file. [Taken from Smaug's fread_letter]
 */
char imcfread_letter( FILE *fp )
{
    char c;

    do
    {
        if( feof(fp) )
        {
            imclog( "%s", "imcfread_letter: EOF encountered on read." );
            return '\0';
        }
        c = getc( fp );
    }
    while( isspace(c) );

    return c;
}

/******************************************
 * Packet handling and routing functions. *
 ******************************************/

void imc_register_packet_handler( char *name, void (*func)( imc_char_data *from, PACKET *p ) )
{
    IMC_PHANDLER *ph;

    for( ph = first_phandler; ph; ph = ph->next )
    {
        if( !strcasecmp( ph->name, name ) )
        {
            imclog( "Unable to register packet type %s. Another module has already registered it.", name );
            return;
        }
    }

    IMCCREATE( ph, IMC_PHANDLER, 1 );

    ph->name = IMCSTRALLOC(name);
    ph->func = func;

    IMCLINK( ph, first_phandler, last_phandler, next, prev );
    return;
}

/* put a line onto descriptors output buffer */
void do_imcsend( char *fline )
{
    int len;
    char *newbuf;
    int newsize = this_imcmud->outsize;

    if( this_imcmud->state == IMC_CLOSED )
        return;

    if( !this_imcmud->outbuf[0] )
        this_imcmud->newoutput = 1;

    len = strlen( this_imcmud->outbuf ) + strlen( fline ) + 3;

    if( len > this_imcmud->outsize )
    {
        while( newsize < len )
            newsize *= 2;

        IMCCREATE( newbuf, char, newsize );
        imcstrlcpy( newbuf, this_imcmud->outbuf, newsize );
        IMCDISPOSE( this_imcmud->outbuf );
        this_imcmud->outbuf = newbuf;
        this_imcmud->outsize = newsize;
    }
    if( len < this_imcmud->outsize/2 && len >= IMC_MINBUF )
    {
        newsize = this_imcmud->outsize/2;

        IMCCREATE( newbuf, char, newsize );
        imcstrlcpy( newbuf, this_imcmud->outbuf, newsize );
        IMCDISPOSE( this_imcmud->outbuf );
        this_imcmud->outbuf = newbuf;
        this_imcmud->outsize = newsize;
    }
    imcstrlcat( this_imcmud->outbuf, fline, this_imcmud->outsize );
    imcstrlcat( this_imcmud->outbuf, "\n\r", this_imcmud->outsize );
}

/* send a packet to a mud using the right version */
void do_send_packet( PACKET *p )
{
    char *output;

    output = ( *imc_vinfo[IMC_VERSION].generate )( p );

    if( output )
    {
        imc_stats.tx_pkts++;
        if( strlen( output ) > (unsigned int)imc_stats.max_pkt )
            imc_stats.max_pkt = strlen( output );
        do_imcsend( output );
    }
}

void imc_send( PACKET *p )
{
    if( imc_active < IA_UP )
    {
        imcbug( "%s", "imc_send when not active!" );
        return;
    }

    /* initialize packet fields that the caller shouldn't/doesn't set */
    p->i.path[0] = '\0';

    p->i.sequence = imc_sequencenumber++;
    if( !imc_sequencenumber )
        imc_sequencenumber++;

    if( !strcasecmp( p->type, "remote-admin" ) && this_imcmud->md5pass == TRUE )
    {
        char pwd[IMC_DATA_LENGTH];
        char *cryptpwd;

        snprintf( pwd, IMC_DATA_LENGTH, "%ld%s", p->i.sequence, imc_getkey( p, "hash", "none" ) );
        cryptpwd = imc_crypt( pwd );
        imc_addkey( p, "hash", cryptpwd );
    }

    imcstrlcpy( p->i.to, p->to, IMC_NAME_LENGTH );
    snprintf( p->i.from, IMC_NAME_LENGTH, "%s@%s", p->from, this_imcmud->localname );
    do_send_packet( p );
}

/* update our routing table based on a packet received with path "path" */
void updateroutes( char *path )
{
    REMOTEINFO *p;
    char *sender, *temp;

    /* loop through each item in the path, and update routes to there */
    temp = path;
    while( temp && temp[0] != '\0' )
    {
        sender = imc_firstinpath( temp );

        if( strcasecmp( sender, this_imcmud->localname ) )
        {
            /* not from us */
            /* check if its in the list already */
            p = imc_find_reminfo( sender );
            if( !p )                                        /* not in list yet, create a new entry */
            {
                p = imc_new_reminfo( sender );

                p->version = IMCSTRALLOC( "unknown" );
                p->network = IMCSTRALLOC( "unknown" );
            }
            /* already in list, update the entry */
            else
                p->expired = FALSE;
        }
        /* get the next item in the path */
        temp = strchr( temp, '!' );
        if( temp )
            temp++;                                         /* skip to just after the next '!' */
    }
}

void do_imcchannel( imc_char_data *from, int number, char *argument, int emote )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    char str[LGST], buf[LGST];
    unsigned int chan;

    for( chan = 0; chan < numchannels; chan++ )
        if( imc_channels[chan].number == number )
            break;

    if( chan == numchannels )
        return;

    imcstrlcpy( str, emote ? imc_channels[chan].emotestr : imc_channels[chan].chatstr, LGST );

    for( d = first_descriptor; d; d = d->next )
    {
        if( d->connected == CON_PLAYING && ( victim = d->original ? d->original : d->character ) != NULL
            && IMCPERM(victim) >= imc_channels[chan].perm )
        {
            snprintf( buf, LGST, str, getname( victim, from ), argument );
            imc_to_char( buf, victim );
        }
    }
}

/* send a tell to a remote player */
void imc_send_tell( imc_char_data *from, char *to, char *argument, int isreply )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    if( !strcasecmp( imc_mudof( to ), "*" ) )
        return;                                             /* don't let them do this */

    setdata( &out, from );

    imcstrlcpy( out.to, to, IMC_NAME_LENGTH );
    imcstrlcpy( out.type, "tell", IMC_TYPE_LENGTH );
    imc_addkey( &out, "text", argument );
    if( isreply )
        imc_addkeyi( &out, "isreply", isreply );

    imc_send( &out );
    imc_freedata( &out );
}

void imc_update_tellhistory( CHAR_DATA *ch, const char *msg )
{
    char new_msg[LGST];
    time_t t = time( NULL );
    struct tm *local = localtime(&t);
    int x;

    snprintf( new_msg, LGST, "~R[%-2.2d:%-2.2d] %s", local->tm_hour, local->tm_min, msg );

    for( x = 0; x < MAX_IMCTELLHISTORY; x++ )
    {
        if( IMCTELLHISTORY(ch,x) == '\0' )
        {
            IMCTELLHISTORY(ch,x) = IMCSTRALLOC( new_msg );
            break;
        }

        if( x == MAX_IMCTELLHISTORY-1 )
        {
            int i;

            for( i = 1; i < MAX_IMCTELLHISTORY; i++ )
            {
                IMCSTRFREE( IMCTELLHISTORY(ch,i-1) );
                IMCTELLHISTORY(ch,i-1) = IMCSTRALLOC( IMCTELLHISTORY(ch,i) );
            }
            IMCSTRFREE( IMCTELLHISTORY(ch,x) );
            IMCTELLHISTORY(ch,x) = IMCSTRALLOC( new_msg );
        }
    }
    return;
}

void imc_recv_tell( imc_char_data *from, char *to, char *argument, int isreply )
{
    CHAR_DATA *victim = NULL;
    char buf[IMC_DATA_LENGTH];

    if( !strcasecmp( to, "*" ) )                            /* ignore messages to system */
        return;

    victim = imc_find_user( to );

    if( victim )
    {
        if( IMCPERM(victim) < IMCPERM_MORT )
            return;

        if( strcasecmp( imc_nameof( from->name ), "ICE" ) )
        {
            if( IMCISINVIS(victim) )
            {
                if( strcasecmp( imc_nameof( from->name ), "*" ) )
                {
                    snprintf( buf, IMC_DATA_LENGTH, "%s is not receiving tells.", to );
                    imc_send_tell( NULL, from->name, buf, 1 );
                }
                return;
            }

            if( imc_isignoring( victim, from->name ) )
            {
                if( strcasecmp( imc_nameof( from->name ), "*" ) )
                {
                    snprintf( buf, IMC_DATA_LENGTH, "%s is not receiving tells.", to );
                    imc_send_tell( NULL, from->name, buf, 1 );
                }
                return;
            }

            if( IMCIS_SET( IMCFLAG(victim), IMC_TELL ) || IMCIS_SET( IMCFLAG(victim), IMC_DENYTELL ) )
            {
                if( strcasecmp( imc_nameof( from->name ), "*" ) )
                {
                    snprintf( buf, IMC_DATA_LENGTH, "%s is not receiving tells.", to );
                    imc_send_tell( NULL, from->name, buf, 1 );
                }
                return;
            }

            if( IMCAFK(victim) )
            {
                if( strcasecmp( imc_nameof( from->name ), "*" ) )
                {
                    snprintf( buf, IMC_DATA_LENGTH, "%s is currently AFK. Try back later.", to );
                    imc_send_tell( NULL, from->name, buf, 1 );
                }
                return;
            }

                                                            /* not a system message */
            if( strcasecmp( imc_nameof( from->name ), "*" ) )
            {
                IMCSTRFREE( IMC_RREPLY(victim) );
                IMCSTRFREE( IMC_RREPLY_NAME(victim) );
                IMC_RREPLY(victim) = IMCSTRALLOC( from->name );
                IMC_RREPLY_NAME(victim) = IMCSTRALLOC( getname( victim, from ) );
            }
        }
        /* Tell social */
        if( isreply == 2 )
            snprintf( buf, IMC_DATA_LENGTH, "~WImctell: ~c%s\n\r", argument );
        else
            snprintf( buf, IMC_DATA_LENGTH, "~C%s ~cimctells you ~c'~W%s~c'~!\n\r", getname( victim, from ), argument );
        imc_to_char( buf, victim );
        imc_update_tellhistory( victim, buf );
    }
    else
    {
        snprintf( buf, IMC_DATA_LENGTH, "No player named %s exists here.", to );
        imc_send_tell( NULL, from->name, buf, 1 );
    }
}

void imc_recv_whoreply( char *to, char *text, int sequence )
{
    CHAR_DATA *victim;

    if( !( victim = imc_find_user( to ) ) )
        return;

    imc_to_pager( text, victim );
    return;
}

void update_imchistory( IMC_CHANNEL *channel, char *message )
{
    char msg[LGST], buf[LGST];
    struct tm *local;
    time_t t;
    int x;

    if( !channel )
    {
        imcbug( "%s", "update_imchistory: NULL channel received!" );
        return;
    }

    if( !message || message[0] == '\0' )
    {
        imcbug( "%s", "update_imchistory: NULL message received!" );
        return;
    }

    imcstrlcpy( msg, message, LGST );
    for( x = 0; x < MAX_IMCHISTORY; x++ )
    {
        if( channel->history[x] == NULL )
        {
            t = time( NULL );
            local = localtime( &t );
            snprintf( buf, LGST, "~R[%-2.2d/%-2.2d %-2.2d:%-2.2d] ~G%s",
                local->tm_mon+1, local->tm_mday, local->tm_hour, local->tm_min, msg );
            channel->history[x] = IMCSTRALLOC( buf );

            if( IMCIS_SET( channel->flags, IMCCHAN_LOG ) )
            {
                FILE *fp;
                snprintf( buf, LGST, "%s%s.log", IMC_DIR, channel->local_name );
                if( !( fp = fopen( buf, "a" ) ) )
                {
                    perror( buf );
                    imcbug( "Could not open file %s!", buf );
                }
                else
                {
                    fprintf( fp, "%s\n", imc_strip_colors( channel->history[x] ) );
                    IMCFCLOSE( fp );
                }
            }
            break;
        }

        if( x == MAX_IMCHISTORY - 1 )
        {
            int y;

            for( y = 1; y < MAX_IMCHISTORY; y++ )
            {
                int z = y-1;

                if( channel->history[z] != NULL )
                {
                    IMCSTRFREE( channel->history[z] );
                    channel->history[z] = IMCSTRALLOC( channel->history[y] );
                }
            }

            t = time( NULL );
            local = localtime( &t );
            snprintf( buf, LGST, "~R[%-2.2d/%-2.2d %-2.2d:%-2.2d] ~G%s",
                local->tm_mon+1, local->tm_mday, local->tm_hour, local->tm_min, msg );
            IMCSTRFREE( channel->history[x] );
            channel->history[x] = IMCSTRALLOC( buf );

            if( IMCIS_SET( channel->flags, IMCCHAN_LOG ) )
            {
                FILE *fp;
                snprintf( buf, LGST, "%s%s.log", IMC_DIR, channel->local_name );
                if( !( fp = fopen( buf, "a" ) ) )
                {
                    perror( buf );
                    imcbug( "Could not open file %s!", buf );
                }
                else
                {
                    fprintf( fp, "%s\n", imc_strip_colors( channel->history[x] ) );
                    IMCFCLOSE( fp );
                }
            }
        }
    }
    return;
}

void imc_showchannel( IMC_CHANNEL *c, char *from, char *txt, int emote )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    char buf[LGST], name[SMST];

    if( !c->local_name || c->local_name[0] == '\0' || !c->refreshed )
        return;

    if( emote < 2 )
        snprintf( buf, LGST, emote ? c->emoteformat : c->regformat, from, txt );
    else
        snprintf( buf, LGST, c->socformat, txt );

    for( d = first_descriptor; d; d = d->next )
    {
        ch = d->original ? d->original : d->character;

        if( !ch || d->connected != CON_PLAYING )
            continue;

        /* Freaking stupid PC_DATA crap! */
        if( IS_NPC(ch) )
            continue;

        if( IMCPERM(ch) < c->level || !imc_hasname( IMC_LISTEN(ch), c->local_name ) )
            continue;

        if( !c->open )
        {
            snprintf( name, SMST, "%s@%s", CH_IMCNAME(ch), this_imcmud->localname );
            if( !imc_hasname( c->invited, name ) && strcasecmp( c->owner, name ) )
                continue;
        }
        imc_printf( ch, "%s\n\r", buf );
    }
    update_imchistory( c, buf );
}

void imc_sendmessage( IMC_CHANNEL *c, char *name, char *text, int emote )
{
    PACKET out;

    imcstrlcpy( out.from, name, IMC_NAME_LENGTH );
    imc_initdata( &out );

    imc_addkey( &out, "channel", c->name );
    imc_addkey( &out, "text", text );
    imc_addkeyi( &out, "emote", emote );
    imc_addkeyi( &out, "echo", 1 );

    /* send a message out on a channel */
    if( !c->open )
    {
        imcstrlcpy( out.type, "ice-msg-p", IMC_TYPE_LENGTH );
        snprintf( out.to, IMC_NAME_LENGTH, "IMC@%s", channel_mudof( c->name ) );
    }
    else
    {
        /* broadcast */
        imcstrlcpy( out.type, "ice-msg-b", IMC_TYPE_LENGTH );
        imcstrlcpy( out.to, "*@*", IMC_NAME_LENGTH );
    }
    imc_send( &out );
    imc_freedata( &out );
}

/* respond to a who request with the given data */
void imc_send_whoreply( char *to, char *data, int sequence )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    if( !strcasecmp( imc_mudof( to ), "*" ) )
        return;                                             /* don't let them do this */

    imc_initdata( &out );

    imcstrlcpy( out.to, to, IMC_NAME_LENGTH );

    imcstrlcpy( out.type, "who-reply", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );

    if( sequence != -1 )
        imc_addkeyi( &out, "sequence", sequence );

    imc_addkey( &out, "text", data );

    imc_send( &out );
    imc_freedata( &out );
}

void imc_whoreply_start( char *to )
{
    wr_sequence = 0;
    wr_to = IMCSTRALLOC( to );
    wr_buf[0] = '\0';
}

void imc_whoreply_add( char *text )
{
    /* give a bit of a margin for error here */
    if( strlen( wr_to ) + strlen( text ) >= IMC_DATA_LENGTH-500 )
    {
        imc_send_whoreply( wr_to, wr_buf, wr_sequence );
        wr_sequence++;
        imcstrlcpy( wr_buf, text, IMC_DATA_LENGTH );
        return;
    }
    imcstrlcat( wr_buf, text, IMC_DATA_LENGTH );
}

void imc_whoreply_end( void )
{
    imc_send_whoreply( wr_to, wr_buf, -( wr_sequence + 1 ) );
    IMCSTRFREE( wr_to );
    wr_buf[0] = '\0';
}

char *imccenterline( const char *string, int length )
{
    char stripped[300];
    static char outbuf[400];
    int amount;

    imcstrlcpy( stripped, imc_strip_colors( string ), 300 );
    amount = length - strlen( stripped );                   /* Determine amount to put in front of line */

    if( amount < 1 )
        amount = 1;

    /* Justice, you are the String God! */
    snprintf( outbuf, 400, "%*s%s%*s", ( amount / 2 ), "", string,
        ( ( amount / 2 ) * 2 ) == amount ? ( amount / 2 ) : ( ( amount / 2 ) + 1 ), "" );

    return outbuf;
}

char *imcrankbuffer( CHAR_DATA *ch )
{
    static char rbuf[SMST];

    if( IMCPERM(ch) >= IMCPERM_IMM )
    {
        imcstrlcpy( rbuf, "~YStaff", SMST );

        if( CH_IMCRANK(ch) && CH_IMCRANK(ch)[0] != '\0' )
            snprintf( rbuf, SMST, "~Y%s", color_mtoi( CH_IMCRANK(ch) ) );
    }
    else
    {
        imcstrlcpy( rbuf, "~BPlayer", SMST );

        if( CH_IMCRANK(ch) && CH_IMCRANK(ch)[0] != '\0' )
            snprintf( rbuf, SMST, "~B%s", color_mtoi( CH_IMCRANK(ch) ) );
    }
    return rbuf;
}

/* expanded for minimal mud-specific code. I really don't want to replicate
 * stock in-game who displays here, since it's one of the most commonly
 * changed pieces of code. shrug.
 */
void process_rwho( imc_char_data *from, char *argument )
{
    DESCRIPTOR_DATA *d;
    CHAR_DATA *person;
    char buf[LGST], whobuf[LGST], personbuf[LGST], tailbuf[LGST], rank[LGST], rankout[LGST];
    char stats[SMST];
    int pcount = 0, xx, yy;

    imc_whoreply_start( from->name );
    imc_whoreply_add( "\n\r" );

    snprintf( whobuf, LGST, "~R-=[ ~WPlayers on %s ~R]=-", this_imcmud->fullname );
    imcstrlcpy( buf, imccenterline( whobuf, 78 ), LGST );
    imcstrlcat( buf, "\n\r", LGST );
    imc_whoreply_add( buf );

    if( this_imcmud->iport > 0 )
        snprintf( whobuf, LGST, "~Y-=[ ~Wtelnet://%s:%d ~Y]=-", this_imcmud->ihost, this_imcmud->iport );
    else
        snprintf( whobuf, LGST, "~Y-=[ telnet://%s ]=-", this_imcmud->ihost );

    imcstrlcpy( buf, imccenterline( whobuf, 78 ), LGST );
    imcstrlcat( buf, "\n\r\n\r", LGST );
    imc_whoreply_add( buf );

    xx = 0;
    for( d = first_descriptor; d; d = d->next )
    {
        person = d->original ? d->original : d->character;
        if( person && d->connected == CON_PLAYING )
        {
            if( IMCPERM(person) <= IMCPERM_NONE || IMCPERM(person) >= IMCPERM_IMM )
                continue;

            if( IMCISINVIS( person ) )
                continue;

            pcount++;

            if( xx == 0 )
                imc_whoreply_add( "~B--------------------------------=[ ~WPlayers ~B]=---------------------------------\n\r\n\r" );

            imcstrlcpy( rank, imcrankbuffer( person ), LGST );
            imcstrlcpy( rankout, imccenterline( rank, 20 ), LGST );

            imcstrlcpy( stats, "~z[", SMST );
            if( IMCAFK( person ) )
                imcstrlcat( stats, "AFK", SMST );
            else
                imcstrlcat( stats, "---", SMST );
            imcstrlcat( stats, "]~G", SMST );
            snprintf( personbuf, LGST, "%s %s %s%s\n\r", rankout, stats, CH_IMCNAME(person), CH_IMCTITLE(person) );
            imc_whoreply_add( color_mtoi( personbuf ) );
            xx++;
        }
    }

    yy = 0;
    for( d = first_descriptor; d; d = d->next )
    {
        person = d->original ? d->original : d->character;
        if( person && d->connected == CON_PLAYING )
        {
            if( IMCPERM(person) <= IMCPERM_NONE || IMCPERM(person) < IMCPERM_IMM )
                continue;

            if( IMCISINVIS( person ) )
                continue;

            pcount++;

            if( yy == 0 )
                imc_whoreply_add( "\n\r~R-------------------------------=[ ~WImmortals ~R]=--------------------------------\n\r\n\r" );

            imcstrlcpy( rank, imcrankbuffer( person ), LGST );
            imcstrlcpy( rankout, imccenterline( rank, 20 ), LGST );

            imcstrlcpy( stats, "~z[", SMST );
            if( IMCAFK( person ) )
                imcstrlcat( stats, "AFK", SMST );
            else
                imcstrlcat( stats, "---", SMST );
            imcstrlcat( stats, "]~G", SMST );

            snprintf( personbuf, LGST, "%s %s %s%s\n\r", rankout, stats, CH_IMCNAME(person), CH_IMCTITLE(person) );
            imc_whoreply_add( color_mtoi( personbuf ) );
            yy++;
        }
    }

    snprintf( tailbuf, LGST, "\n\r~Y[~W%d Player%s~Y] ", pcount, pcount == 1 ? "" : "s" );
    imc_whoreply_add( tailbuf );

    snprintf( tailbuf, LGST, "~Y[~WHomepage: %s~Y] [~W%3d Player%s Online~Y]\n\r",
        this_imcmud->www, xx+yy, ( xx+yy == 1 ) ? "" : "s" );
    imc_whoreply_add( tailbuf );

    imc_whoreply_end();
}

void process_rfinger( imc_char_data *from, char *to )
{
    CHAR_DATA *victim;
    char buf[IMC_DATA_LENGTH];

    if( !to || to[0] == '\0' )
        return;

    if( !( victim = imc_find_user( to ) ) )
    {
        imc_send_whoreply( from->name, "No such player is online.\n\r", -1 );
        return;
    }

    if( IMCISINVIS(victim) || IMCPERM(victim) < IMCPERM_MORT )
    {
        imc_send_whoreply( from->name, "No such player is online.\n\r", -1 );
        return;
    }

    snprintf( buf, IMC_DATA_LENGTH,
        "\n\r~cPlayer Profile for ~W%s~c:\n\r"
        "~W-------------------------------\n\r"
        "~cStatus: ~W%s\n\r"
        "~cPermission level: ~W%s\n\r"
        "~cListening to channels [Names may not match your mud]: ~W%s\n\r",
        CH_IMCNAME(victim), ( IMCAFK(victim) ? "AFK" : "Lurking about" ),
        imcperm_names[IMCPERM(victim)],
        ( IMC_LISTEN(victim) && IMC_LISTEN(victim)[0] != '\0' ) ? IMC_LISTEN(victim) : "None" );

    if( !IMCIS_SET( IMCFLAG(victim), IMC_PRIVACY ) )
        snprintf( buf+strlen(buf), IMC_DATA_LENGTH-strlen(buf),
            "~cEmail   : ~W%s\n\r"
            "~cHomepage: ~W%s\n\r"
            "~cICQ     : ~W%d\n\r"
            "~cAIM     : ~W%s\n\r"
            "~cYahoo   : ~W%s\n\r"
            "~cMSN     : ~W%s\n\r",
            ( IMC_EMAIL(victim) && IMC_EMAIL(victim)[0] != '\0' ) ? IMC_EMAIL(victim) : "None",
        ( IMC_HOMEPAGE(victim) && IMC_HOMEPAGE(victim)[0] != '\0' ) ? IMC_HOMEPAGE(victim) : "None",
        IMC_ICQ(victim),
        ( IMC_AIM(victim) && IMC_AIM(victim)[0] != '\0' ) ? IMC_AIM(victim) : "None",
        ( IMC_YAHOO(victim) && IMC_YAHOO(victim)[0] != '\0' ) ? IMC_YAHOO(victim) : "None",
        ( IMC_MSN(victim) && IMC_MSN(victim)[0] != '\0' ) ? IMC_MSN(victim) : "None" );

    snprintf( buf+strlen(buf), IMC_DATA_LENGTH-strlen(buf), "~W%s\n\r",
        ( IMC_COMMENT(victim) && IMC_COMMENT(victim)[0] != '\0' ) ? IMC_COMMENT(victim) : "" );

    imc_send_whoreply( from->name, buf, -1 );
}

void imc_recv_who( imc_char_data *from, char *type )
{
    char arg[SMST];
    char output[LGST];

    type = imc_getarg( type, arg, SMST );

    if( !strcasecmp( arg, "who" ) )
    {
        process_rwho( from, type );
        return;
    }
    else if( !strcasecmp( arg, "finger" ) )
    {
        process_rfinger( from, type );
        return;
    }
    else if( !strcasecmp( arg, "info" ) )
        snprintf( output, LGST,
                "~!Site Information --\n\r"
                "~cSite Name      ~W: ~!%s\n\r"
                "~cSite Host      ~W: ~!%s\n\r"
                "~cAdmin Email    ~W: ~!%s\n\r"
                "~cAdmin MUDMail  ~W: ~!Disabled\n\r"
                "~cWeb Site       ~W: ~!%s\n\r"
                "IMC Information --\n\r"
                "~cIMC Version    ~W: ~!%s\n\r"
                "~cIMC Details    ~W: ~!%s\n\r",
                this_imcmud->fullname, this_imcmud->ihost, this_imcmud->email, this_imcmud->www, IMC_VERSIONID, this_imcmud->details );
    else
        snprintf( output, LGST, "%s is not a valid option. Options are: who, finger, or info.\n\r", type );

    imc_send_whoreply( from->name, output, -1 );
}

/* respond with a whois-reply */
void imc_send_whoisreply( char *to, char *data )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    if( !strcasecmp( imc_mudof( to ), "*" ) )
        return;                                             /* don't let them do this */

    imc_initdata( &out );

    imcstrlcpy( out.to, to, IMC_NAME_LENGTH );
    imcstrlcpy( out.type, "whois-reply", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    imc_addkey( &out, "text", data );

    imc_send( &out );
    imc_freedata( &out );
}

void imc_recv_whois( imc_char_data *from, char *to )
{
    CHAR_DATA *victim;
    char buf[LGST];

    if( ( victim = imc_find_user( to ) ) != NULL && !IMCISINVIS(victim) )
    {
        snprintf( buf, LGST, "~RIMC Locate: ~Y%s@%s: ~cOnline.\n\r", CH_IMCNAME(victim), this_imcmud->localname );
        imc_send_whoisreply( from->name, buf );
    }
    return;
}

void imc_recv_whoisreply( char *to, char *text )
{
    CHAR_DATA *victim;

    if( ( victim = imc_find_user( to ) ) != NULL )
        imc_to_char( text, victim );
    return;
}

void imc_recv_beep( imc_char_data *from, char *to )
{
    CHAR_DATA *victim = NULL;
    char buf[IMC_DATA_LENGTH];

    if( !strcasecmp( to, "*" ) )                            /* ignore messages to system */
        return;

    victim = imc_find_user( to );

    if( victim )
    {
        if( IMCPERM(victim) < IMCPERM_MORT )
            return;

        if( IMCISINVIS(victim) )
        {
            if( strcasecmp( imc_nameof( from->name ), "*" ) )
            {
                snprintf( buf, IMC_DATA_LENGTH, "%s is not receiving beeps.", to );
                imc_send_tell( NULL, from->name, buf, 1 );
            }
            return;
        }

        if( imc_isignoring( victim, from->name ) )
        {
            if( strcasecmp( imc_nameof( from->name ), "*" ) )
            {
                snprintf( buf, IMC_DATA_LENGTH, "%s is not receiving beeps.", to );
                imc_send_tell( NULL, from->name, buf, 1 );
            }
            return;
        }

        if( IMCIS_SET( IMCFLAG(victim), IMC_BEEP ) || IMCIS_SET( IMCFLAG(victim), IMC_DENYBEEP ) )
        {
            if( strcasecmp( imc_nameof( from->name ), "*" ) )
            {
                snprintf( buf, IMC_DATA_LENGTH, "%s is not receiving beeps.", to );
                imc_send_tell( NULL, from->name, buf, 1 );
            }
            return;
        }

        if( IMCAFK(victim) )
        {
            if( strcasecmp( imc_nameof( from->name ), "*" ) )
            {
                snprintf( buf, IMC_DATA_LENGTH, "%s is currently AFK. Try back later.", to );
                imc_send_tell( NULL, from->name, buf, 1 );
            }
            return;
        }

        /* always display the true name here */
        snprintf( buf, IMC_DATA_LENGTH, "~c\a%s imcbeeps you.~!\n\r", from->name );
        imc_to_char( buf, victim );
    }
    else
    {
        snprintf( buf, IMC_DATA_LENGTH, "No player named %s exists here.", to );
        imc_send_tell( NULL, from->name, buf, 1 );
    }
}

/* send a ping with a given timestamp */
void imc_send_ping( char *to )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    imc_initdata( &out );
    imcstrlcpy( out.type, "ping", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    imcstrlcpy( out.to, "*@", IMC_NAME_LENGTH );
    imcstrlcpy( out.to+2, to, IMC_MNAME_LENGTH-2 );

    imc_send( &out );
    imc_freedata( &out );
}

/* send a who-request to a remote mud */
void imc_send_who( imc_char_data *from, char *to, char *type )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    if( !strcasecmp( imc_mudof( to ), "*" ) )
        return;                                             /* don't let them do this */

    setdata( &out, from );

    snprintf( out.to, IMC_NAME_LENGTH, "*@%s", to );
    imcstrlcpy( out.type, "who", IMC_TYPE_LENGTH );

    imc_addkey( &out, "type", type );

    imc_send( &out );
    imc_freedata( &out );
}

/* send a locate request to the network */
void imc_send_locate( imc_char_data *from, char *to )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    if( strchr( to, '@' ) )
        return;

    setdata( &out, from );

    snprintf( out.to, IMC_NAME_LENGTH, "%s@*", to );
    imcstrlcpy( out.type, "whois", IMC_TYPE_LENGTH );

    imc_send( &out );
    imc_freedata( &out );
}

/* beep a remote player */
void imc_send_beep( imc_char_data *from, char *to )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    if( !strcasecmp( imc_mudof( to ), "*" ) )
        return;                                             /* don't let them do this */

    setdata( &out, from );
    imcstrlcpy( out.type, "beep", IMC_TYPE_LENGTH );
    imcstrlcpy( out.to, to, IMC_NAME_LENGTH );

    imc_send( &out );
    imc_freedata( &out );
}

/* called when a keepalive has been received */
void imc_recv_keepalive( char *from, char *version, char *flags, char *network, char *url )
{
    REMOTEINFO *p;

    if( !strcasecmp( from, this_imcmud->localname ) )
        return;

    /*  this should never fail, imc.c should create an entry if one doesn't exist (in the path update code) */
    if( !( p = imc_find_reminfo( from ) ) )                 /* boggle */
        return;

    if( strcasecmp( version, p->version ) )                 /* remote version has changed? */
    {
        IMCSTRFREE( p->version );                           /* if so, update it */
        p->version = IMCSTRALLOC( version );
    }
    /* Not sure how or why this might change, but just in case it does */
    if( network && network[0] != '\0' && strcasecmp( network, p->network ) )
    {
        IMCSTRFREE( p->network );
        p->network = IMCSTRALLOC( network );
    }

    if( strcasecmp( url, p->url ) )
    {
        IMCSTRFREE( p->url );
        p->url = IMCSTRALLOC( url );
    }
}

void imc_send_keepalive( const char *to )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    imc_initdata( &out );
    imcstrlcpy( out.type, "is-alive", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    imcstrlcpy( out.to, to, IMC_NAME_LENGTH );
    imc_addkey( &out, "versionid", IMC_VERSIONID );
    imc_addkey( &out, "url", this_imcmud->www );

    imc_send( &out );
    imc_freedata( &out );
}

void imc_request_keepalive( void )
{
    PACKET out;

    imc_initdata( &out );
    imcstrlcpy( out.type, "keepalive-request", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    imcstrlcpy( out.to, "*@*", IMC_NAME_LENGTH );

    imc_send( &out );
    imc_freedata( &out );

    imc_send_keepalive( "*@*" );
}

void imc_firstrefresh( void )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    imc_initdata( &out );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    imcstrlcpy( out.to, "IMC@$", IMC_NAME_LENGTH );
    imcstrlcpy( out.type, "ice-refresh", IMC_TYPE_LENGTH );

    imc_send( &out );
    imc_freedata( &out );
}

/* send a pingreply with the given timestamp */
void imc_send_pingreply( char *to, char *path )
{
    PACKET out;

    if( imc_active < IA_UP )
        return;

    imc_initdata( &out );
    imcstrlcpy( out.type, "ping-reply", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    imcstrlcpy( out.to, "*@", IMC_NAME_LENGTH );
    imcstrlcpy( out.to+2, to, IMC_MNAME_LENGTH-2 );
    imc_addkey( &out, "path", path );

    imc_send( &out );
    imc_freedata( &out );
}

/* called when a ping request is received */
void imc_recv_ping( char *from, char *path )
{
    /* ping 'em back */
    imc_send_pingreply( from, path );
}

void imc_traceroute( char *pathto, char *pathfrom )
{
    if( !strcasecmp( imc_firstinpath( pathfrom ), lastping ) )
    {
        CHAR_DATA *ch = NULL;

        if( ( ch = imc_find_user( pinger ) ) == NULL )
            return;

        imc_printf( ch, "%s: Return path: %s\n\rSend path:   %s\n\r",
            imc_firstinpath( pathfrom ), pathfrom, pathto ? pathto : "unknown" );
    }
}

/* called when a ping reply is received */
void imc_recv_pingreply( char *from, char *pathto, char *pathfrom )
{
    REMOTEINFO *p;

    p = imc_find_reminfo( from );                           /* should always exist */
    if( !p )                                                /* boggle */
        return;

    /* check for pending traceroutes */
    imc_traceroute( pathto, pathfrom );
}

int imctodikugender( int gender )
{
    int sex = 0;

    if( gender == 0 )
        sex = SEX_MALE;

    if( gender == 1 )
        sex = SEX_FEMALE;

    return sex;
}

int dikutoimcgender( int gender )
{
    int sex = 0;

    if( gender > 2 || gender < 0 )
        sex = 2;

    if( gender == SEX_MALE )
        sex = 0;

    if( gender == SEX_FEMALE )
        sex = 1;

    return sex;
}

int imc_get_ucache_gender( const char *name )
{
    IMCUCACHE_DATA *user;

    for( user = first_imcucache; user; user = user->next )
    {
        if( !strcasecmp( user->name, name ) )
            return user->gender;
    }

    /* -1 means you aren't in the list and need to be put there. */
    return -1;
}

/* Saves the ucache info to disk because it would just be spamcity otherwise */
void imc_save_ucache( void )
{
    FILE *fp;
    IMCUCACHE_DATA *user;

    if( ( fp = fopen( IMC_UCACHE_FILE, "w" ) ) == NULL )
    {
        imclog( "%s", "Couldn't write to IMC2 ucache file." );
        return;
    }

    for( user = first_imcucache; user; user = user->next )
    {
        fprintf( fp, "%s", "#UCACHE\n" );
        fprintf( fp, "Name %s\n", user->name );
        fprintf( fp, "Sex  %d\n", user->gender );
        fprintf( fp, "Time %ld\n", (long int)user->time );
        fprintf( fp, "%s", "End\n\n" );
    }
    fprintf( fp, "%s", "#END\n" );
    IMCFCLOSE( fp );
    return;
}

void imc_prune_ucache( void )
{
    IMCUCACHE_DATA *ucache, *next_ucache;

    for( ucache = first_imcucache; ucache; ucache = next_ucache )
    {
        next_ucache = ucache->next;

        /* Info older than 30 days is removed since this person likely hasn't logged in at all */
        if( current_time - ucache->time >= 2592000 )
        {
            IMCSTRFREE( ucache->name );
            IMCUNLINK( ucache, first_imcucache, last_imcucache, next, prev );
            IMCDISPOSE( ucache );
        }
    }
    imc_save_ucache( );
    return;
}

/* Updates user info if they exist, adds them if they don't. */
void imc_ucache_update( const char *name, int gender )
{
    IMCUCACHE_DATA *user;

    for( user = first_imcucache; user; user = user->next )
    {
        if( !strcasecmp( user->name, name ) )
        {
            user->gender = gender;
            user->time = current_time;
            return;
        }
    }
    IMCCREATE( user, IMCUCACHE_DATA, 1 );
    user->name = IMCSTRALLOC( name );
    user->gender = gender;
    user->time = current_time;
    IMCLINK( user, first_imcucache, last_imcucache, next, prev );

    imc_save_ucache( );
    return;
}

void imc_send_ucache_update( const char *visname, int gender )
{
    PACKET out;

    imc_initdata( &out );
    imcstrlcpy( out.type, "user-cache", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, visname, IMC_NAME_LENGTH );
    imcstrlcpy( out.to, "*@*", IMC_NAME_LENGTH );
    imc_addkeyi( &out, "gender", gender );

    imc_send( &out );
    imc_freedata( &out );
    return;
}

void imc_recv_usercache( const char *from, int gender )
{
    int sex;

    sex = imc_get_ucache_gender( from );

    if( sex == gender )
        return;

    imc_ucache_update( from, gender );
    return;
}

void imc_send_ucache_request( char *targetuser )
{
    PACKET out;

    imc_initdata( &out );
    imcstrlcpy( out.type, "user-cache-request", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    snprintf( out.to, IMC_NAME_LENGTH, "*@%s", imc_mudof( targetuser ) );
    imc_addkey( &out, "user", targetuser );

    imc_send( &out );
    imc_freedata( &out );
    return;
}

void imc_process_ucache_request( char *from, char *user )
{
    PACKET out;
    int gender;

    gender = imc_get_ucache_gender( user );

    /* Gender of -1 means they aren't in the mud's ucache table. Don't waste the reply packet. */
    if( gender == -1 )
        return;

    imc_initdata( &out );
    imcstrlcpy( out.type, "user-cache-reply", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    snprintf( out.to, IMC_NAME_LENGTH, "*@%s", imc_mudof( from ) );
    imc_addkey( &out, "user", user );
    imc_addkeyi( &out, "gender", gender );

    imc_send( &out );
    imc_freedata( &out );
    return;
}

void imc_process_ucache_reply( const char *from, const char *user, int gender )
{
    int sex;

    sex = imc_get_ucache_gender( user );

    if( sex == gender )
        return;

    imc_ucache_update( user, gender );
    return;
}

void imc_recv_msg_r( char *from, char *realfrom, char *chan, char *txt, int emote )
{
    IMC_CHANNEL *c;
    char *mud;

    mud = imc_mudof( from );

    /* forged? */
    if( !strchr( chan, ':' ) || strcasecmp( mud, channel_mudof( chan ) ) )
        return;

    c = imc_findchannel( chan );
    if( !c )
        return;

    if( !c->local_name || ( c->open ) )
        return;

    /* We assume that anything redirected is automatically audible - since we trust the router...
     * What's all this *WE* business? *I* don't trust it :)
     */
    imc_showchannel( c, realfrom, txt, emote );
}

void imc_recv_msg_b( char *from, char *chan, char *txt, int emote, char *sender )
{
    IMC_CHANNEL *c;

    c = imc_findchannel( chan );
    if( !c )
        return;

    if( !c->local_name || !c->open )
        return;

    if( sender && sender[0] != '\0' )
        imc_showchannel( c, sender, txt, emote );
    else
        imc_showchannel( c, from, txt, emote );
}

void imc_recv_update( char *from, char *chan, char *owner, char *localname, char *operators, char *policy, char *invited, char *excluded, char *level )
{
    IMC_CHANNEL *c;
    char *mud;

    mud = imc_mudof( from );

    /* forged? */
    if( !strchr( chan, ':' ) || strcasecmp( mud, channel_mudof( chan ) ) )
        return;

    c = imc_findchannel( chan );

    if( !c )
    {
        int value = get_imcpermvalue( level );
        if( value < 0 || value > IMCPERM_IMP )
            value = IMCPERM_IMP;

        IMCCREATE( c, IMC_CHANNEL, 1 );
        c->name = IMCSTRALLOC( chan );
        c->owner = IMCSTRALLOC( owner );
        c->operators = IMCSTRALLOC( operators );
        c->invited = IMCSTRALLOC( invited );
        c->excluded = IMCSTRALLOC( excluded );
        c->local_name = NULL;

        if( localname && localname[0] != '\0' )
            c->dlname = IMCSTRALLOC( localname );
        else
            c->dlname = NULL;

        c->level = value;
        c->refreshed = TRUE;
        IMCLINK( c, first_imc_channel, last_imc_channel, next, prev );
    }
    else
    {
        IMCSTRFREE( c->name );
        IMCSTRFREE( c->owner );
        IMCSTRFREE( c->operators );
        IMCSTRFREE( c->invited );
        IMCSTRFREE( c->excluded );
        IMCSTRFREE( c->dlname );
        c->name = IMCSTRALLOC( chan );
        c->owner = IMCSTRALLOC( owner );
        c->operators = IMCSTRALLOC( operators );
        c->invited = IMCSTRALLOC( invited );
        c->excluded = IMCSTRALLOC( excluded );

        if( localname && localname[0] != '\0' )
            c->dlname = IMCSTRALLOC( localname );
        else
            c->dlname = NULL;

        c->refreshed = TRUE;
    }

    /* 3.20+ only supports open and private. Anything not one of these becomes private. */
    if( !strcasecmp( policy, "open" ) )
        c->open = TRUE;
    else
        c->open = FALSE;
}

void imc_recv_destroy( char *from, char *channel )
{
    IMC_CHANNEL *c;
    char *mud;

    mud = imc_mudof( from );

    if( !strchr( channel, ':' ) || strcasecmp( mud, channel_mudof( channel ) ) )
        return;

    c = imc_findchannel( channel );
    if( !c )
        return;

    imc_freechan( c );
    imc_save_channels();
}

void imc_recv_channel_whoreply( char *to, char *channel, char *list )
{
    IMC_CHANNEL *c;
    CHAR_DATA *vic;

    if( !strcasecmp( to, "*" ) )                            /* ignore messages to system */
        return;

    if( !strchr( channel, ':' ) )
        return;

    c = imc_findchannel( channel );
    if( !c )
        return;

    if( !( vic = imc_find_user( to ) ) )
        return;

    imc_printf( vic, "~G%s", list );
    return;
}

void imc_recv_channel_who( char *from, char *to, char *channel, char *lname, int level )
{
    IMC_CHANNEL *c;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *person;
    PACKET out;
    char buf[IMC_DATA_LENGTH];

    if( !strchr( channel, ':' ) )
        return;

    c = imc_findchannel( channel );
    if( !c )
        return;

    if( !c->local_name )
        snprintf( buf, IMC_DATA_LENGTH, "Channel %s is not locally configured on %s\n\r", lname, this_imcmud->localname );
    else if( c->level > level )
        snprintf( buf, IMC_DATA_LENGTH, "Channel %s is above your permission level on %s\n\r", lname, this_imcmud->localname );
    else
    {
        int count = 0;

        snprintf( buf, IMC_DATA_LENGTH, "The following people are listening to %s on %s:\n\r\n\r", lname, this_imcmud->localname );
        for( d = first_descriptor; d; d = d->next )
        {
            person = d->original ? d->original : d->character;

            if( !person )
                continue;

            if( IMCISINVIS(person) )
                continue;

            if( !imc_hasname( IMC_LISTEN(person), c->local_name ) )
                continue;

            snprintf( buf+strlen(buf), IMC_DATA_LENGTH-strlen(buf), "%s\n\r", CH_IMCNAME(person) );
            count++;
        }
        /* Send no response to a broadcast request if nobody is listening. */
        if( count == 0 && !strcasecmp( to, "*" ) )
            return;
        else
            imcstrlcat( buf, "Nobody", IMC_DATA_LENGTH );
    }

    imc_initdata( &out );
    imcstrlcpy( out.type, "ice-chan-whoreply", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, "*", IMC_NAME_LENGTH );
    imcstrlcpy( out.to, from, IMC_NAME_LENGTH );
    imc_addkey( &out, "channel", c->name );
    imc_addkey( &out, "list", buf );

    imc_send( &out );
    imc_freedata( &out );
    return;
}

void imc_sendnotify( CHAR_DATA *ch, char *chan, bool chon )
{
    PACKET out;
    IMC_CHANNEL *channel;

    if( !IMCIS_SET( IMCFLAG(ch), IMC_NOTIFY ) )
        return;

    if( !( channel = imc_findlchannel( chan ) ) )
        return;

    imc_initdata( &out );
    imcstrlcpy( out.type, "channel-notify", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, CH_IMCNAME(ch), IMC_NAME_LENGTH );
    imcstrlcpy( out.to, "*@*", IMC_NAME_LENGTH );
    imc_addkey( &out, "channel", channel->name );
    imc_addkeyi( &out, "status", chon );

    imc_send( &out );
    imc_freedata( &out );
    return;
}

void imc_recv_channel_notify( const char *from, char *chan, bool chon )
{
    IMC_CHANNEL *c;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *ch;
    char buf[LGST];

    if( !( c = imc_findchannel( chan ) ) )
        return;

    if( !c->local_name || c->local_name[0] == '\0' )
        return;

    if( chon == TRUE )
        snprintf( buf, LGST, c->emoteformat, from, "has joined the channel." );
    else
        snprintf( buf, LGST, c->emoteformat, from, "has left the channel." );

    for( d = first_descriptor; d; d = d->next )
    {
        ch = d->original ? d->original : d->character;

        if( !ch || d->connected != CON_PLAYING )
            continue;

        /* Freaking stupid PC_DATA crap! */
        if( IS_NPC(ch) )
            continue;

        if( IMCPERM(ch) < c->level || !imc_hasname( IMC_LISTEN(ch), c->local_name ) )
            continue;

        if( !IMCIS_SET( IMCFLAG(ch), IMC_NOTIFY ) )
            continue;

        imc_printf( ch, "%s\n\r", buf );
    }
    return;
}

/* Handles all incoming packets destined for this mud */
void imc_recv( PACKET *p )
{
    imc_char_data d;
    REMOTEINFO *i;
    IMC_PHANDLER *ph;

    getdata( p, &d );

    i = imc_find_reminfo( imc_mudof( p->from ) );
    if( i )
    {
        IMCSTRFREE( i->path );
        i->path = IMCSTRALLOC( p->i.path );
    }

    /* user-cache: Genger targetting socials. Keep a local table of known users */
    if( !strcasecmp( p->type, "user-cache" ) && !imc_isbanned( p->from ) )
        imc_recv_usercache( p->from, imc_getkeyi( p, "gender", -1 ) );

    else if( !strcasecmp( p->type, "user-cache-request" ) && !imc_isbanned( p->from ) )
        imc_process_ucache_request( p->from, imc_getkey( p, "user", "" ) );

    else if( !strcasecmp( p->type, "user-cache-reply" ) && !imc_isbanned( p->from ) )
        imc_process_ucache_reply( p->from, imc_getkey( p, "user", "" ), imc_getkeyi( p, "gender", -1 ) );

    /* chat: message to a channel (broadcast) */
    else if( !strcasecmp( p->type, "chat" ) && !imc_isbanned( p->from ) )
        do_imcchannel( &d, imc_getkeyi( p, "channel", 15 ), imc_getkey( p, "text", "" ), 0 );

    /* emote: emote to a channel (broadcast) */
    else if( !strcasecmp( p->type, "emote" ) && !imc_isbanned( p->from ) )
        do_imcchannel( &d, imc_getkeyi( p, "channel", 15 ), imc_getkey( p, "text", "" ), 1 );

    /* tell: tell a player here something */
    else if( !strcasecmp( p->type, "tell" ) )
    {
        if( !imc_isbanned( p->from ) )
            imc_recv_tell( &d, p->to, imc_getkey( p, "text", "" ), imc_getkeyi( p, "isreply", 0 ) );
    }

    /* who-reply: receive a who response */
    else if( !strcasecmp( p->type, "who-reply" ) || !strcasecmp( p->type, "wHo-reply" ) )
        imc_recv_whoreply( p->to, imc_getkey( p, "text", "" ), imc_getkeyi( p, "sequence", -1 ) );

    /* who: receive a who request */
    else if( !strcasecmp( p->type, "who" ) || !strcasecmp( p->type, "wHo" ) )
    {
        if( !imc_isbanned( p->from ) )
            imc_recv_who( &d, imc_getkey( p, "type", "who" ) );
    }

    /* whois-reply: receive a whois response */
    else if( !strcasecmp( p->type, "whois-reply" ) )
        imc_recv_whoisreply( p->to, imc_getkey( p, "text", "" ) );

    /* whois: receive a whois request */
    else if( !strcasecmp( p->type, "whois" ) )
    {
        if( !imc_isbanned( p->from ) )
            imc_recv_whois( &d, p->to );
    }

    /* beep: beep a player */
    else if( !strcasecmp( p->type, "beep" ) )
    {
        if( !imc_isbanned( p->from ) )
            imc_recv_beep( &d, p->to );
    }

    /* is-alive: receive a keepalive (broadcast) */
    else if( !strcasecmp( p->type, "is-alive" ) )
        imc_recv_keepalive( imc_mudof( p->from ), imc_getkey( p, "versionid", "unknown" ),
                imc_getkey( p, "flags", "" ), imc_getkey( p, "networkname", "" ),
                imc_getkey( p, "url", "??" ) );

    /* ping: receive a ping request */
    else if( !strcasecmp( p->type, "ping" ) && !imc_isbanned( p->from ) )
        imc_recv_ping( imc_mudof( p->from ), p->i.path );

    /* ping-reply: receive a ping reply */
    else if( !strcasecmp( p->type, "ping-reply" ) )
        imc_recv_pingreply( imc_mudof( p->from ), imc_getkey( p, "path", NULL ), p->i.path );

    else if( !strcasecmp( p->type, "keepalive-request" ) )
        imc_send_keepalive( p->from );

    else if( !strcasecmp( p->type, "close-notify" ) )
    {
        REMOTEINFO *r;

        if( ( r = imc_find_reminfo( imc_getkey( p, "host", "" ) ) ) != NULL )
            r->expired = TRUE;
    }

    /* ICE channel packets, used to be in their own function. */
    else if( !strcasecmp( p->type, "ice-msg-r" ) && !imc_isbanned( p->from ) )
    {
        imc_recv_msg_r( p->from, imc_getkey( p, "realfrom", "" ), imc_getkey( p, "channel", "" ),
            imc_getkey( p, "text", "" ), imc_getkeyi( p, "emote", 0 ) );
    }
    else if( !strcasecmp( p->type, "ice-msg-b" ) && !imc_isbanned( p->from ) )
    {
        imc_recv_msg_b( p->from, imc_getkey( p, "channel", "" ), imc_getkey( p, "text", "" ),
            imc_getkeyi( p, "emote", 0 ), imc_getkey( p, "sender", "" ) );
    }
    else if( !strcasecmp( p->type, "ice-update" ) )
    {
        imc_recv_update( p->from, imc_getkey( p, "channel", "" ), imc_getkey( p, "owner", "" ),
            imc_getkey( p, "localname", "" ), imc_getkey( p, "operators", "" ),
            imc_getkey( p, "policy", "" ), imc_getkey( p, "invited", "" ),
            imc_getkey( p, "excluded", "" ), imc_getkey( p, "level", "" ) );
    }
    else if( !strcasecmp( p->type, "ice-destroy" ) )
        imc_recv_destroy( p->from, imc_getkey( p, "channel", "" ) );
    else if( !strcasecmp( p->type, "ice-chan-who" ) && !imc_isbanned( p->from ) )
    {
        imc_recv_channel_who( p->from, p->to, imc_getkey( p, "channel", "" ),
            imc_getkey( p, "lname", "..." ), imc_getkeyi( p, "level", 0 ) );
    }
    else if( !strcasecmp( p->type, "ice-chan-whoreply" ) && !imc_isbanned( p->from ) )
        imc_recv_channel_whoreply( p->to, imc_getkey( p, "channel", "" ), imc_getkey( p, "list", "" ) );
    else if( !strcasecmp( p->type, "channel-notify" ) && !imc_isbanned( p->from ) )
        imc_recv_channel_notify( p->from, imc_getkey( p, "channel", "" ), imc_getkeyi( p, "status", FALSE ) );
    else
        for( ph = first_phandler; ph; ph = ph->next )
            if( !strcasecmp( ph->name, p->type ) )
                (*ph->func)( &d, p );
    return;
}

/* read waiting data from descriptor.
 * read to a temp buffer to avoid repeated allocations
 */
void do_imcread( void )
{
    int size;
    int r;
    char temp[IMC_MAXBUF];
    char *newbuf;
    int newsize;

    r = read( this_imcmud->desc, temp, IMC_MAXBUF-1 );
    if( !r || ( r < 0 && errno != EAGAIN && errno != EWOULDBLOCK ) )
    {
        if( r < 0 )                                         /* read error */
            imclog( "Read error on connection to %s", this_imcmud->routername ? this_imcmud->routername : "router" );
        else                                                /* socket was closed */
            imclog( "EOF encountered on connection to %s", this_imcmud->routername ? this_imcmud->routername : "router" );

        imc_shutdown( TRUE );
        return;
    }

    if( r < 0 )                                             /* EAGAIN error */
        return;

    temp[r] = '\0';

    size = strlen( this_imcmud->inbuf ) + r + 1;

    if( size >= this_imcmud->insize )
    {
        newsize = this_imcmud->insize;

        while( newsize < size )
            newsize *= 2;

        IMCCREATE( newbuf, char, newsize );
        imcstrlcpy( newbuf, this_imcmud->inbuf, newsize );
        IMCDISPOSE( this_imcmud->inbuf );
        this_imcmud->inbuf = newbuf;
        this_imcmud->insize = newsize;
    }

    if( size < this_imcmud->insize/2 && size >= IMC_MINBUF )
    {
        newsize = this_imcmud->insize;
        newsize /= 2;

        IMCCREATE( newbuf, char, newsize );
        imcstrlcpy( newbuf, this_imcmud->inbuf, newsize );
        IMCDISPOSE( this_imcmud->inbuf );
        this_imcmud->inbuf = newbuf;
        this_imcmud->insize = newsize;
    }

    imcstrlcat( this_imcmud->inbuf, temp, this_imcmud->insize );

    imc_stats.rx_bytes += r;
}

/* write to descriptor */
void do_imcwrite( void )
{
    int size, w;

    if( this_imcmud->state == IMC_CONNECTING )
    {
        /* Wait for server password */
        this_imcmud->state = IMC_WAIT1;
        return;
    }

    size = strlen( this_imcmud->outbuf );
    if( !size )                                             /* nothing to write */
        return;

    w = write( this_imcmud->desc, this_imcmud->outbuf, size );
    if( !w || ( w < 0 && errno != EAGAIN && errno != EWOULDBLOCK ) )
    {
        if( w < 0 )                                         /* write error */
            imclog( "Write error on connection to %s", this_imcmud->routername ? this_imcmud->routername : "router" );
        else                                                /* socket was closed */
            imclog( "EOF encountered on connection to %s", this_imcmud->routername ? this_imcmud->routername : "router" );

        imc_shutdown( TRUE );
        return;
    }

    if( w < 0 )                                             /* EAGAIN */
        return;

    if( imcpacketdebug )
        imclog( "Packet sent: %s", this_imcmud->outbuf );

    /* throw away data we wrote */
    imcstrlcpy( this_imcmud->outbuf, this_imcmud->outbuf + w, this_imcmud->outsize );

    imc_stats.tx_bytes += w;
}

/*  try to read a line from the input buffer, NULL if none ready
 *  all lines are \n\r terminated in theory, but take other combinations
 */
char *imcgetline( char *buffer )
{
    int i;
    static char buf[IMC_PACKET_LENGTH];

    /* copy until \n, \r, end of buffer, or out of space */
    for( i = 0; buffer[i] && buffer[i] != '\n' && buffer[i] != '\r' && i+1 < IMC_PACKET_LENGTH; i++ )
        buf[i] = buffer[i];

    /* end of buffer and we haven't hit the maximum line length */
    if( !buffer[i] && i+1 < IMC_PACKET_LENGTH )
    {
        buf[0] = '\0';
        return NULL;                                        /* so no line available */
    }

    /* terminate return string */
    buf[i] = '\0';

    /* strip off extra control codes */
    while( buffer[i] && ( buffer[i] == '\n' || buffer[i] == '\r' ) )
        i++;

    /* remove the line from the input buffer */
    imcstrlcpy( buffer, buffer + i, this_imcmud->insize );

    return buf;
}

void imc_finalize_connection( char *name, char *netname, char *version )
{
    int rversion;

    this_imcmud->state = IMC_CONNECTED;

    if( version && version[0] != '\0' )
    {
        /* check for a version string (assume version 2 if not present) */
        if( sscanf( version, "version=%d", &rversion ) != 1 )
            rversion = 2;

        /* check for generator/interpreter */
        if( !imc_vinfo[rversion].generate || !imc_vinfo[rversion].interpret )
        {
            imclog( "%s: Unsupported version %d", name, rversion );
            imc_shutdown( FALSE );
            return;
        }
    }

    if( netname && netname[0] != '\0' )
    {
        IMCSTRFREE( this_imcmud->network );
        this_imcmud->network = IMCSTRALLOC( netname );
    }

    IMCSTRFREE( this_imcmud->routername );
    this_imcmud->routername = IMCSTRALLOC( name );

    imclog( "Connected to %s. Network ID: %s", name, ( netname && netname[0] != '\0' ) ? netname : "Unknown" );

    imcconnect_attempts = 0;
    imc_request_keepalive();
    imc_firstrefresh();
    return;
}

/* Handle an autosetup response from a supporting router - Samson 8-12-03 */
void imc_handle_autosetup( char *source, char *routername, char *cmd, char *txt, char *md5 )
{
    if( !strcasecmp( cmd, "reject" ) )
    {
        if( !strcasecmp( txt, "connected" ) )
        {
            imclog( "There is already a mud named %s connected to the network.", this_imcmud->localname );
            imc_shutdown( FALSE );
            return;
        }
        if( !strcasecmp( txt, "private" ) )
        {
            imclog( "%s is a private router. Autosetup denied.", routername );
            imc_shutdown( FALSE );
            return;
        }
        if( !strcasecmp( txt, "full" ) )
        {
            imclog( "%s has reached its connection limit. Autosetup denied.", routername );
            imc_shutdown( FALSE );
            return;
        }
        if( !strcasecmp( txt, "ban" ) )
        {
            imclog( "%s has banned your connection. Autosetup denied.", routername );
            imc_shutdown( FALSE );
            return;
        }
        imclog( "%s: Invalid 'reject' response. Autosetup failed.", routername );
        imclog( "Data received: %s %s %s %s %s", source, routername, cmd, txt, md5 );
        imc_shutdown( FALSE );
        return;
    }

    if( !strcasecmp( cmd, "accept" ) )
    {
        imclog( "Autosetup completed successfully." );
        if( md5 && md5[0] != '\0' && !strcasecmp( md5, "MD5-SET" ) )
        {
            imclog( "MD5 Authentication has been enabled." );
            this_imcmud->md5pass = TRUE;
            imc_save_config();
        }
        imc_finalize_connection( routername, txt, "" );
        return;
    }

    imclog( "%s: Invalid autosetup response.", routername );
    imclog( "Data received: %s %s %s %s %s", source, routername, cmd, txt, md5 );
    imc_shutdown( FALSE );
    return;
}

/* handle a password response from a router - for connection negotiation. DON'T REMOVE THIS */
/* Updated for new MD5 supporting routers - Samson 2-18-04 */
void serverpassword( char *argument )
{
    char arg1[IMC_PW_LENGTH], name[IMC_MNAME_LENGTH], pw[IMC_PW_LENGTH];
    char version[IMC_PW_LENGTH], netname[IMC_MNAME_LENGTH], md5[IMC_PW_LENGTH], response[IMC_PACKET_LENGTH];

    argument = imc_getarg( argument, arg1, IMC_PW_LENGTH );
    argument = imc_getarg( argument, name, IMC_MNAME_LENGTH );
    argument = imc_getarg( argument, pw, IMC_PW_LENGTH );
    argument = imc_getarg( argument, version, IMC_PW_LENGTH );
    argument = imc_getarg( argument, netname, IMC_MNAME_LENGTH );
    argument = imc_getarg( argument, md5, IMC_PW_LENGTH );

    if( !name || name[0] == '\0' )
    {
        imclog( "%s", "Incomplete authentication packet. Unable to connect." );
        imc_shutdown( FALSE );
        return;
    }

    if( !strcasecmp( arg1, "MD5-AUTH-INIT" ) )
    {
        char pwd[IMC_DATA_LENGTH];
        char *cryptpwd;
        long auth_value = 0;

        if( !pw || pw[0] == '\0' )
        {
            imclog( "MD5 Authentication failure: No auth_value was returned by %s.", name );
            imc_shutdown( FALSE );
            return;
        }

        /* Lets encrypt this bastard now! */
        auth_value = atol( pw );
        snprintf( pwd, IMC_DATA_LENGTH, "%ld%s%s", auth_value, this_imcmud->clientpw, this_imcmud->serverpw );
        cryptpwd = imc_crypt( pwd );

        snprintf( response, IMC_PACKET_LENGTH, "MD5-AUTH-RESP %s %s version=%d",
            this_imcmud->localname, cryptpwd, IMC_VERSION );
        do_imcsend( response );
        return;
    }

    /* MD5 response is pretty simple. If you blew the authentication, it happened on the router anyway.
     * name=Routername pw=Networkname
     */
    if( !strcasecmp( arg1, "MD5-AUTH-APPR" ) )
    {
        imclog( "%s", "MD5 Authentication completed." );
        imc_finalize_connection( name, pw, version );
        return;
    }

    /* The old way. Nice and icky, but still very much required for compatibility. */
    if( !strcasecmp( arg1, "PW" ) )
    {
        if( strcasecmp( this_imcmud->serverpw, pw ) )
        {
            imclog( "%s sent an improper serverpassword.", name );
            imc_shutdown( FALSE );
            return;
        }

        imclog( "%s", "Standard Authentication completed." );
        if( md5 && md5[0] != '\0' && !strcasecmp( md5, "MD5-SET" ) )
        {
            imclog( "MD5 Authentication has been enabled." );
            this_imcmud->md5pass = TRUE;
            imc_save_config();
        }
        imc_finalize_connection( name, netname, version );
        return;
    }

    /* Should only be received from routers supporting this obviously
     * arg1=autosetup name=routername pw=command version=response netname=MD5-SET
     */
    if( !strcasecmp( arg1, "autosetup" ) )
    {
        imc_handle_autosetup( arg1, name, pw, version, netname );
        return;
    }

    imclog( "Invalid authentication response received from %s!!", name );
    imclog( "Data received: %s %s %s %s %s", arg1, name, pw, version, netname );
    imc_shutdown( FALSE );
    return;
}

/* interpret an incoming packet using the right version */
PACKET *do_interpret_packet( char *fline )
{
    int v;
    PACKET *p;

    if( !fline[0] )
        return NULL;

    v = IMC_VERSION;

    p = (*imc_vinfo[v].interpret)(fline);
    return p;
}

int imc_fill_fdsets( int maxfd, fd_set *iread, fd_set *iwrite, fd_set *exc )
{
    if( imc_active < IA_UP )
        return maxfd;

    /* set up fd_sets for select */

    if( maxfd < this_imcmud->desc )
        maxfd = this_imcmud->desc;

    switch( this_imcmud->state )
    {
        case IMC_CONNECTING:                                /* connected/error when writable */
            FD_SET( this_imcmud->desc, iwrite );
            break;
        case IMC_CONNECTED:
        case IMC_WAIT1:
            FD_SET( this_imcmud->desc, iread );
            if( this_imcmud->outbuf && this_imcmud->outbuf[0] != '\0' )
                FD_SET( this_imcmud->desc, iwrite );
            break;
    }
    return maxfd;
}

/* low-level idle function: read/write buffers as needed, etc */
void imc_idle_select( fd_set *iread, fd_set *iwrite, fd_set *exc, time_t now )
{
    char *command;
    PACKET *p;

    if( this_imcmud->desc < 1 )
        return;

    if( imc_active < IA_CONFIG1 )
        return;

    if( imc_sequencenumber < (unsigned long)imc_now )
        imc_sequencenumber = (unsigned long)imc_now;

    if( imc_active < IA_UP )
        return;

    /* handle results of the select */
    if( this_imcmud->state != IMC_CLOSED && FD_ISSET( this_imcmud->desc, exc ) )
    {
        imc_shutdown( TRUE );
        return;
    }

    if( this_imcmud->state != IMC_CLOSED && FD_ISSET( this_imcmud->desc, iread ) )
        do_imcread( );

    while( this_imcmud->state != IMC_CLOSED && ( command = imcgetline( this_imcmud->inbuf ) ) != NULL )
    {
        if( strlen( command ) > (unsigned int)imc_stats.max_pkt )
            imc_stats.max_pkt = strlen( command );

        if( imcpacketdebug )
            imclog( "Packet received: %s", command );

        switch( this_imcmud->state )
        {
            case IMC_CLOSED:
                break;
            case IMC_WAIT1:
                serverpassword( command );
                break;
            case IMC_CONNECTED:
                p = do_interpret_packet( command );
                if( p )
                {
                    imc_stats.rx_pkts++;

                    /* check for duplication, and register the packet in the sequence memory */
                    if( p->i.sequence && checkrepeat( imc_mudof( p->i.from ), p->i.sequence ) )
                        break;

                    /* update our routing info */
                    updateroutes( p->i.path );

                    /* Receive it if it's for us, otherwise it gets silently dropped */
                    if( !strcasecmp( imc_mudof( p->i.to ), "*" ) || !strcasecmp( imc_mudof( p->i.to ), this_imcmud->localname ) )
                    {
                                                            /* strip the name from the 'to' */
                        imcstrlcpy( p->to, imc_nameof( p->i.to ), IMC_NAME_LENGTH );
                        imcstrlcpy( p->from, p->i.from, IMC_NAME_LENGTH );

                        imc_recv( p );
                    }
                    imc_freedata( p );
                }
                break;
        }
    }

    if( this_imcmud->desc > 0 )                             /* Something could have caused shutdown during reading */
    {
        if( this_imcmud->state != IMC_CLOSED && ( FD_ISSET( this_imcmud->desc, iwrite ) || this_imcmud->newoutput ) )
        {
            do_imcwrite( );
            this_imcmud->newoutput = this_imcmud->outbuf[0];
            if( this_imcmud->disconnect == TRUE )
                imc_shutdown( FALSE );
        }
    }
}

void imc_loop( void )
{
    fd_set in_set, out_set, exc_set;
    static struct timeval null_time;
    int maxdesc = 0;

    #ifdef IMCCIRCLE
    current_time = time(NULL);
    #endif

    if( imcwait > 0 )
        imcwait--;

    /* Condition reached only if network shutdown after startup */
    if( imcwait == 1 )
    {
        imcconnect_attempts++;
        if( imcconnect_attempts > 5 )
        {
            imcwait = -2;
            imclog( "Unable to reestablish connection to %s. Abandoning reconnect.", this_imcmud->routername );
            return;
        }
        imc_startup( TRUE );
        return;
    }

    if( imc_active == IA_NONE || this_imcmud->desc == -1 )
        return;

    /* Will prune the cache once every 24hrs after bootup time */
    if( imcucache_clock <= current_time )
    {
        imcucache_clock = current_time + 86400;
        imc_prune_ucache( );
    }

    FD_ZERO( &in_set  );
    FD_ZERO( &out_set );
    FD_ZERO( &exc_set );

    maxdesc = imc_fill_fdsets( maxdesc, &in_set, &out_set, &exc_set );
    if( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
    {
        perror( "imc_loop: select: poll" );
        imc_shutdown( TRUE );
        return;
    }
    imc_idle_select( &in_set, &out_set, &exc_set, current_time );
    return;
}

/************************************
 * User login and logout functions. *
 ************************************/

void imc_adjust_perms( CHAR_DATA *ch )
{
    if( !this_imcmud )
        return;

    /* Ugly hack to let the permission system adapt freely, but retains the ability to override that adaptation
     * in the event you need to restrict someone to a lower level, or grant someone a higher level. This of
     * course comes at the cost of forgetting you may have done so and caused the override flag to be set, but hey.
     * This isn't a perfect system and never will be. Samson 2-8-04.
     */
    if( !IMCIS_SET( IMCFLAG(ch), IMC_PERMOVERRIDE ) )
    {
        if( CH_IMCLEVEL(ch) < this_imcmud->minlevel )
            IMCPERM(ch) = IMCPERM_NONE;
        else if( CH_IMCLEVEL(ch) >= this_imcmud->minlevel && CH_IMCLEVEL(ch) < this_imcmud->immlevel )
            IMCPERM(ch) = IMCPERM_MORT;
        else if( CH_IMCLEVEL(ch) >= this_imcmud->immlevel && CH_IMCLEVEL(ch) < this_imcmud->adminlevel )
            IMCPERM(ch) = IMCPERM_IMM;
        else if( CH_IMCLEVEL(ch) >= this_imcmud->adminlevel && CH_IMCLEVEL(ch) < this_imcmud->implevel )
            IMCPERM(ch) = IMCPERM_ADMIN;
        else if( CH_IMCLEVEL(ch) >= this_imcmud->implevel )
            IMCPERM(ch) = IMCPERM_IMP;
    }
    return;
}

void imc_char_login( CHAR_DATA *ch )
{
    char buf[IMC_NAME_LENGTH];
    int gender, sex;

    if( !this_imcmud )
        return;

    imc_adjust_perms( ch );

    if( imc_active != IA_UP )
    {
        if( IMCPERM(ch) >= IMCPERM_IMM && imcwait == -2 )
            imc_to_char( "~RThe IMC2 connection is down. Attempts to reconnect were abandoned due to excessive failures.\n\r", ch );
        return;
    }

    if( IMCPERM(ch) < IMCPERM_MORT )
        return;

    snprintf( buf, IMC_NAME_LENGTH, "%s@%s", CH_IMCNAME(ch), this_imcmud->localname );
    gender = imc_get_ucache_gender( buf );
    sex = dikutoimcgender( CH_IMCSEX(ch) );

    if( gender == sex )
        return;

    imc_ucache_update( buf, sex );
    if( !IMCIS_SET( IMCFLAG(ch), IMC_INVIS ) )
        imc_send_ucache_update( CH_IMCNAME(ch), sex );

    return;
}

bool imc_loadchar( CHAR_DATA *ch, FILE *fp, const char *word )
{
    bool fMatch = FALSE;

    if( IS_NPC(ch) )
        return FALSE;

    if( IMCPERM(ch) == IMCPERM_NOTSET )
        imc_adjust_perms( ch );

    switch( word[0] )
    {
        case 'I':
            IMCKEY( "IMCPerm",         IMCPERM(ch),      imcfread_number( fp ) );
            IMCKEY( "IMCEmail",        IMC_EMAIL(ch),    imcfread_line( fp ) );
            IMCKEY( "IMCAIM",          IMC_AIM(ch),      imcfread_line( fp ) );
            IMCKEY( "IMCICQ",          IMC_ICQ(ch),      imcfread_number( fp ) );
            IMCKEY( "IMCYahoo",        IMC_YAHOO(ch),    imcfread_line( fp ) );
            IMCKEY( "IMCMSN",          IMC_MSN(ch),      imcfread_line( fp ) );
            IMCKEY( "IMCHomepage",     IMC_HOMEPAGE(ch), imcfread_line( fp ) );
            IMCKEY( "IMCComment",      IMC_COMMENT(ch),  imcfread_line( fp ) );
            if( !strcasecmp( word, "IMCFlags" ) )
            {
                IMCFLAG(ch) = imcfread_number( fp );
                imc_char_login( ch );
                fMatch = TRUE;
                break;
            }

            if( !strcasecmp( word, "IMClisten" ) )
            {
                IMC_LISTEN(ch) = imcfread_line( fp );
                if( IMC_LISTEN(ch) != NULL && imc_active == IA_UP )
                {
                    IMC_CHANNEL *channel = NULL;
                    char *channels = IMC_LISTEN(ch);
                    char arg[SMST];

                    while( 1 )
                    {
                        if( channels[0] == '\0' )
                            break;
                        channels = imcone_argument( channels, arg );

                        if( !( channel = imc_findlchannel( arg ) ) )
                            imc_removename( &IMC_LISTEN(ch), arg );
                        if( channel && IMCPERM(ch) < channel->level )
                            imc_removename( &IMC_LISTEN(ch), arg );
                        if( imc_hasname( IMC_LISTEN(ch), arg ) )
                            imc_sendnotify( ch, arg, TRUE );
                    }
                }
                fMatch = TRUE;
                break;
            }

            if( !strcasecmp( word, "IMCdeny" ) )
            {
                IMC_DENY(ch) = imcfread_line( fp );
                if( IMC_DENY(ch) != NULL && imc_active == IA_UP )
                {
                    IMC_CHANNEL *channel = NULL;
                    char *channels = IMC_DENY(ch);
                    char arg[SMST];

                    while( 1 )
                    {
                        if( channels[0] == '\0' )
                            break;
                        channels = imcone_argument( channels, arg );

                        if( !( channel = imc_findlchannel( arg ) ) )
                            imc_removename( &IMC_DENY( ch ), arg );
                        if( channel && IMCPERM( ch ) < channel->level )
                            imc_removename( &IMC_DENY( ch ), arg );
                    }
                }
                fMatch = TRUE;
                break;
            }

            if( !strcasecmp( word, "IMCignore" ) )
            {
                IMC_IGNORE *temp;

                IMCCREATE( temp, IMC_IGNORE, 1 );
                temp->name = imcfread_line( fp );
                IMCLINK( temp, FIRST_IMCIGNORE(ch), LAST_IMCIGNORE(ch), next, prev );
                fMatch = TRUE;
                break;
            }
            break;
    }
    return fMatch;
}

void imc_savechar( CHAR_DATA *ch, FILE *fp )
{
    IMC_IGNORE *temp;

    if( IS_NPC(ch) )
        return;

    fprintf( fp, "IMCPerm      %d\n", IMCPERM(ch) );
    fprintf( fp, "IMCFlags     %d\n", IMCFLAG(ch) );
    if( IMC_LISTEN(ch) && IMC_LISTEN(ch)[0] != '\0' )
        fprintf( fp, "IMCListen %s\n", IMC_LISTEN(ch) );
    if( IMC_DENY(ch) && IMC_DENY(ch)[0] != '\0' )
        fprintf( fp, "IMCDeny   %s\n", IMC_DENY(ch) );
    if( IMC_EMAIL(ch) && IMC_EMAIL(ch)[0] != '\0' )
        fprintf( fp, "IMCEmail     %s\n", IMC_EMAIL(ch) );
    if( IMC_HOMEPAGE(ch) && IMC_HOMEPAGE(ch)[0] != '\0' )
        fprintf( fp, "IMCHomepage  %s\n", IMC_HOMEPAGE(ch) );
    if( IMC_ICQ(ch) )
        fprintf( fp, "IMCICQ       %d\n", IMC_ICQ(ch) );
    if( IMC_AIM(ch) && IMC_AIM(ch)[0] != '\0' )
        fprintf( fp, "IMCAIM       %s\n", IMC_AIM(ch) );
    if( IMC_YAHOO(ch) && IMC_YAHOO(ch)[0] != '\0' )
        fprintf( fp, "IMCYahoo     %s\n", IMC_YAHOO(ch) );
    if( IMC_MSN(ch) && IMC_MSN(ch)[0] != '\0' )
        fprintf( fp, "IMCMSN       %s\n", IMC_MSN(ch) );
    if( IMC_COMMENT(ch) && IMC_COMMENT(ch)[0] != '\0' )
        fprintf( fp, "IMCComment   %s\n", IMC_COMMENT(ch) );
    for( temp = FIRST_IMCIGNORE(ch); temp; temp = temp->next )
        fprintf( fp, "IMCignore	%s\n", temp->name );
    return;
}

#ifdef _DISKIO_H_
/* This is used only by CircleMUDs which have the ASCII Pfile code installed */
void imc_load_pfile( CHAR_DATA *ch, char *tag, int num, char *line )
{
    if( !strcmp( tag, "IMCPrm" ) )
        IMCPERM(ch) = num;
    if( !strcmp( tag, "IMCEml" ) )
        IMC_EMAIL(ch) = IMCSTRALLOC( line );
    if( !strcmp( tag, "IMCAIM" ) )
        IMC_AIM(ch) = IMCSTRALLOC( line );
    if( !strcmp( tag, "IMCICQ" ) )
        IMC_ICQ(ch) = num;
    if( !strcmp( tag, "IMCYah" ) )
        IMC_YAHOO(ch) = IMCSTRALLOC( line );
    if( !strcmp( tag, "IMCMSN" ) )
        IMC_MSN(ch) = IMCSTRALLOC( line );
    if( !strcmp( tag, "IMCURL" ) )
        IMC_HOMEPAGE(ch) = IMCSTRALLOC( line );
    if( !strcmp( tag, "IMCCMT" ) )
        IMC_COMMENT(ch) = IMCSTRALLOC( line );
    if( !strcmp( tag, "IMCFLG" ) )
        IMCFLAG(ch) = num;
    if( !strcmp( tag, "IMCLSN" ) )
    {
        IMC_LISTEN(ch) = IMCSTRALLOC( line );
        if( IMC_LISTEN(ch) != NULL && imc_active == IA_UP )
        {
            IMC_CHANNEL *channel = NULL;
            char *channels = IMC_LISTEN(ch);
            char arg[SMST];

            while( 1 )
            {
                if( channels[0] == '\0' )
                    break;
                channels = imcone_argument( channels, arg );

                if( !( channel = imc_findlchannel( arg ) ) )
                    imc_removename( &IMC_LISTEN(ch), arg );
                if( channel && IMCPERM(ch) < channel->level )
                    imc_removename( &IMC_LISTEN(ch), arg );
                if( imc_hasname( IMC_LISTEN(ch), arg ) )
                    imc_sendnotify( ch, arg, TRUE );
            }
        }
    }

    if( !strcmp( tag, "IMCDNY" ) )
    {
        IMC_DENY(ch) = IMCSTRALLOC( line );
        if( IMC_DENY(ch) != NULL && imc_active == IA_UP )
        {
            IMC_CHANNEL *channel = NULL;
            char *channels = IMC_DENY(ch);
            char arg[SMST];

            while( 1 )
            {
                if( channels[0] == '\0' )
                    break;
                channels = imcone_argument( channels, arg );

                if( !( channel = imc_findlchannel( arg ) ) )
                    imc_removename( &IMC_DENY( ch ), arg );
                if( channel && IMCPERM( ch ) < channel->level )
                    imc_removename( &IMC_DENY( ch ), arg );
            }
        }
    }

    if( !strcmp( tag, "IMCIGN" ) )
    {
        IMC_IGNORE *temp;

        IMCCREATE( temp, IMC_IGNORE, 1 );
        temp->name = IMCSTRALLOC( line );
        IMCLINK( temp, FIRST_IMCIGNORE(ch), LAST_IMCIGNORE(ch), next, prev );
    }
}

/* This is used only by CircleMUDs which have the ASCII Pfile code installed */
void imc_save_pfile( struct char_data *ch, FBFILE *fp )
{
    IMC_IGNORE *temp;

    if( IS_NPC(ch) )
        return;

    fbprintf( fp, "IMCPrm      %d\n", IMCPERM(ch) );
    fbprintf( fp, "IMCFLG     %d\n", IMCFLAG(ch) );
    if( IMC_LISTEN(ch) && IMC_LISTEN(ch)[0] != '\0' )
        fbprintf( fp, "IMCLSN %s\n", IMC_LISTEN(ch) );
    if( IMC_DENY(ch) && IMC_DENY(ch)[0] != '\0' )
        fbprintf( fp, "IMCDNY   %s\n", IMC_DENY(ch) );
    if( IMC_EMAIL(ch) && IMC_EMAIL(ch)[0] != '\0' )
        fbprintf( fp, "IMCEml   %s\n", IMC_EMAIL(ch) );
    if( IMC_HOMEPAGE(ch) && IMC_HOMEPAGE(ch)[0] != '\0' )
        fbprintf( fp, "IMCURL   %s\n", IMC_HOMEPAGE(ch) );
    if( IMC_ICQ(ch) )
        fbprintf( fp, "IMCICQ   %d\n", IMC_ICQ(ch) );
    if( IMC_AIM(ch) && IMC_AIM(ch)[0] != '\0' )
        fbprintf( fp, "IMCAIM   %s\n", IMC_AIM(ch) );
    if( IMC_YAHOO(ch) && IMC_YAHOO(ch)[0] != '\0' )
        fbprintf( fp, "IMCYah   %s\n", IMC_YAHOO(ch) );
    if( IMC_MSN(ch) && IMC_MSN(ch)[0] != '\0' )
        fbprintf( fp, "IMCMSN   %s\n", IMC_MSN(ch) );
    if( IMC_COMMENT(ch) && IMC_COMMENT(ch)[0] != '\0' )
        fbprintf( fp, "IMCCMT   %s\n", IMC_COMMENT(ch) );
    for( temp = FIRST_IMCIGNORE(ch); temp; temp = temp->next )
        fbprintf( fp, "IMCIGN   %s\n", temp->name );
    return;
}
#endif

void imc_freechardata( CHAR_DATA *ch )
{
    IMC_IGNORE *ign, *ign_next;
    int x;

    if( IS_NPC(ch) )
        return;

    if( CH_IMCDATA(ch) == NULL )
        return;

    for( ign = FIRST_IMCIGNORE(ch); ign; ign = ign_next )
    {
        ign_next = ign->next;
        IMCSTRFREE( ign->name );
        IMCUNLINK( ign, FIRST_IMCIGNORE(ch), LAST_IMCIGNORE(ch), next, prev );
        IMCDISPOSE( ign );
    }
    for( x = 0; x < MAX_IMCTELLHISTORY; x++ )
        IMCDISPOSE( IMCTELLHISTORY(ch,x) );
    IMCSTRFREE( IMC_LISTEN(ch) );
    IMCSTRFREE( IMC_RREPLY(ch) );
    IMCSTRFREE( IMC_RREPLY_NAME(ch) );
    IMCSTRFREE( IMC_EMAIL(ch) );
    IMCSTRFREE( IMC_HOMEPAGE(ch) );
    IMCSTRFREE( IMC_AIM(ch) );
    IMCSTRFREE( IMC_YAHOO(ch) );
    IMCSTRFREE( IMC_MSN(ch) );
    IMCSTRFREE( IMC_COMMENT(ch) );
    IMCDISPOSE( CH_IMCDATA(ch) );
    return;
}

void imc_initchar( CHAR_DATA *ch )
{
    if( IS_NPC(ch) )
        return;

    IMCCREATE( CH_IMCDATA(ch), IMC_CHARDATA, 1 );
    IMC_LISTEN(ch)   = NULL;
    IMC_DENY(ch)   = NULL;
    IMC_RREPLY(ch)   = NULL;
    IMC_RREPLY_NAME(ch) = NULL;
    IMC_EMAIL(ch)       = NULL;
    IMC_HOMEPAGE(ch)    = NULL;
    IMC_AIM(ch)         = NULL;
    IMC_YAHOO(ch)       = NULL;
    IMC_MSN(ch)         = NULL;
    IMC_COMMENT(ch)     = NULL;
    IMCFLAG(ch)         = 0;
    IMCSET_BIT( IMCFLAG(ch), IMC_COLORFLAG );
    FIRST_IMCIGNORE(ch) = NULL;
    LAST_IMCIGNORE(ch)  = NULL;
    IMCPERM(ch)         = IMCPERM_NOTSET;

    return;
}

/*******************************************
 * Network Startup and Shutdown functions. *
 *******************************************/

void imc_loadhistory( void )
{
    char filename[256];
    FILE *tempfile;
    IMC_CHANNEL *tempchan = NULL;
    int x;

    for( tempchan = first_imc_channel; tempchan; tempchan = tempchan->next )
    {
        if( !tempchan->local_name )
            continue;

        snprintf( filename, 256, "%s%s.hist", IMC_DIR, tempchan->local_name );

        if( !( tempfile = fopen( filename, "r" ) ) )
            continue;

        for( x = 0; x < MAX_IMCHISTORY; x++ )
        {
            if( feof( tempfile ) )
                tempchan->history[x] = NULL;
            else
                tempchan->history[x] = imcfread_line( tempfile );
        }
        IMCFCLOSE( tempfile );
        unlink( filename );
    }
}

void imc_savehistory( void )
{
    char filename[256];
    FILE *tempfile;
    IMC_CHANNEL *tempchan = NULL;
    int x;

    for( tempchan = first_imc_channel; tempchan; tempchan = tempchan->next )
    {
        if( !tempchan->local_name )
            continue;

        if( !tempchan->history[0] )
            continue;

        snprintf( filename, 256, "%s%s.hist", IMC_DIR, tempchan->local_name );

        if( !( tempfile = fopen( filename, "w" ) ) )
            continue;

        for( x = 0; x < MAX_IMCHISTORY; x++ )
        {
            if( tempchan->history[x] != NULL )
                fprintf( tempfile, "%s\n", tempchan->history[x] );
        }
        IMCFCLOSE( tempfile );
    }
}

void imc_save_channels( void )
{
    IMC_CHANNEL *c;
    FILE *fp;

    if( !( fp = fopen( IMC_CHANNEL_FILE, "w" ) ) )
    {
        imcbug( "Can't write to %s", IMC_CHANNEL_FILE );
        return;
    }

    for( c = first_imc_channel; c; c = c->next )
    {
        if( !c->local_name || c->local_name[0] == '\0' )
            continue;

        fprintf( fp, "%s", "#IMCCHAN\n" );
        fprintf( fp, "ChanName   %s\n", c->name );
        fprintf( fp, "ChanLocal  %s\n", c->local_name );
        fprintf( fp, "ChanRegF   %s\n", c->regformat );
        fprintf( fp, "ChanEmoF   %s\n", c->emoteformat );
        fprintf( fp, "ChanSocF   %s\n", c->socformat );
        fprintf( fp, "ChanLevel  %d\n", c->level );
        fprintf( fp, "%s", "End\n\n" );
    }
    fprintf( fp, "%s", "#END\n" );
    IMCFCLOSE( fp );
}

void imc_readchannel( IMC_CHANNEL *channel, FILE *fp )
{
    const char *word;
    bool fMatch;

    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : imcfread_word( fp );
        fMatch = FALSE;

        switch( word[0] )
        {
            case '*':
                fMatch = TRUE;
                imcfread_to_eol( fp );
                break;

            case 'C':
                IMCKEY( "ChanName",     channel->name,      imcfread_line( fp ) );
                IMCKEY( "ChanLocal",        channel->local_name,    imcfread_line( fp ) );
                IMCKEY( "ChanRegF",     channel->regformat, imcfread_line( fp ) );
                IMCKEY( "ChanEmoF",     channel->emoteformat,   imcfread_line( fp ) );
                IMCKEY( "ChanSocF",     channel->socformat, imcfread_line( fp ) );
                IMCKEY( "ChanLevel",        channel->level,     imcfread_number( fp ) );
                break;

            case 'E':
                if ( !strcasecmp( word, "End" ) )
                {
                    /* Legacy support to convert channel permissions */
                    if( channel->level > IMCPERM_IMP )
                    {
                        /* The IMCPERM_NONE condition should realistically never happen.... */
                        if( channel->level < this_imcmud->minlevel )
                            channel->level = IMCPERM_NONE;
                        else if( channel->level >= this_imcmud->minlevel && channel->level < this_imcmud->immlevel )
                            channel->level = IMCPERM_MORT;
                        else if( channel->level >= this_imcmud->immlevel && channel->level < this_imcmud->adminlevel )
                            channel->level = IMCPERM_IMM;
                        else if( channel->level >= this_imcmud->adminlevel && channel->level < this_imcmud->implevel )
                            channel->level = IMCPERM_ADMIN;
                        else if( channel->level >= this_imcmud->implevel )
                            channel->level = IMCPERM_IMP;
                    }
                }
                return;
                break;
        }

        if( !fMatch )
            imcbug( "imc_readchannel: no match: %s", word );
    }
}

void imc_loadchannels( void )
{
    FILE *fp;
    IMC_CHANNEL *channel;

    first_imc_channel = NULL;
    last_imc_channel = NULL;

    imclog( "%s", "Loading channels..." );

    fp = fopen( IMC_CHANNEL_FILE, "r" );
    if( !fp )
    {
        imcbug( "%s", "Can't open imc channel file" );
        return;
    }

    for ( ; ; )
    {
        char letter;
        char *word;

        letter = imcfread_letter( fp );
        if( letter == '*' )
        {
            imcfread_to_eol( fp );
            continue;
        }

        if( letter != '#' )
        {
            imcbug( "%s", "imc_loadchannels: # not found." );
            break;
        }

        word = imcfread_word( fp );
        if( !strcasecmp( word, "IMCCHAN" ) )
        {
            int x;

            IMCCREATE( channel, IMC_CHANNEL, 1 );
            imc_readchannel( channel, fp );

            for( x = 0; x < MAX_IMCHISTORY; x++ )
                channel->history[x] = NULL;

            channel->refreshed = FALSE;                     /* Prevents crash trying to use a bogus channel */
            IMCLINK( channel, first_imc_channel, last_imc_channel, next, prev );
            imclog( "configured %s as %s", channel->name, channel->local_name );
            continue;
        }
        else if( !strcasecmp( word, "END" ) )
            break;
        else
        {
            imcbug( "imc_loadchannels: bad section: %s.", word );
            continue;
        }
    }
    IMCFCLOSE( fp );
    return;
}

/* Save current mud-level ban list. Short, simple. */
void imc_savebans( void )
{
    FILE *out;
    IMC_BAN *ban;

    if( !( out = fopen( IMC_BAN_FILE, "w" ) ) )
    {
        imcbug( "%s", "imc_savebans: error opening ban file for write" );
        return;
    }

    fprintf( out, "%s", "#IGNORES\n" );

    for( ban = first_imc_ban; ban; ban = ban->next )
        fprintf( out, "%s\n", ban->name );

    fprintf( out, "%s", "#END\n" );

    IMCFCLOSE( out );
    return;
}

void imc_readbans( void )
{
    FILE *inf;
    char *word;
    char temp[IMC_NAME_LENGTH];

    imclog( "%s", "Loading ban list..." );

    if( !( inf = fopen( IMC_BAN_FILE, "r" ) ) )
    {
        imcbug( "%s", "imc_readbans: couldn't open ban file" );
        return;
    }

    word = imcfread_word( inf );
    if( strcasecmp( word, "#IGNORES" ) )
    {
        imcbug( "%s", "imc_readbans: Corrupt file" );
        IMCFCLOSE( inf );
        return;
    }

    while( !feof( inf ) && !ferror( inf ) )
    {
        imcstrlcpy( temp, imcfread_word( inf ), IMC_NAME_LENGTH );
        if( !strcasecmp( temp, "#END" ) )
        {
            IMCFCLOSE( inf );
            return;
        }
        imc_addban( temp );
    }

    if( ferror( inf ) )
    {
        perror( "imc_readbans" );
        IMCFCLOSE( inf );
        return;
    }

    IMCFCLOSE( inf );
    return;
}

void imc_savecolor( void )
{
    FILE *fp;
    IMC_COLOR *color;

    if( ( fp = fopen( IMC_COLOR_FILE, "w" ) ) == NULL )
    {
        imclog( "%s", "Couldn't write to IMC2 color file." );
        return;
    }

    for( color = first_imc_color; color; color = color->next )
    {
        fprintf( fp, "%s", "#COLOR\n" );
        fprintf( fp, "Name   %s\n", color->name );
        fprintf( fp, "Mudtag %s\n", color->mudtag );
        fprintf( fp, "IMCtag %s\n", color->imctag );
        fprintf( fp, "%s", "End\n\n" );
    }
    fprintf( fp, "%s", "#END\n" );
    IMCFCLOSE( fp );
    return;
}

void imc_readcolor( IMC_COLOR *color, FILE *fp )
{
    const char *word;
    bool fMatch;

    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : imcfread_word( fp );
        fMatch = FALSE;

        switch( word[0] )
        {
            case '*':
                fMatch = TRUE;
                imcfread_to_eol( fp );
                break;

            case 'E':
                if( !strcasecmp( word, "End" ) )
                    return;
                break;

            case 'I':
                IMCKEY( "IMCtag",       color->imctag,  imcfread_line( fp ) );
                break;

            case 'M':
                IMCKEY( "Mudtag",       color->mudtag,  imcfread_line( fp ) );
                break;

            case 'N':
                IMCKEY( "Name",     color->name,    imcfread_line( fp ) );
                break;
        }
        if( !fMatch )
            imcbug( "imc_readcolor: no match: %s", word );
    }
}

void imc_load_color_table( void )
{
    FILE *fp;
    IMC_COLOR *color;

    first_imc_color = last_imc_color = NULL;

    imclog( "%s", "Loading IMC2 color table..." );

    if( !( fp = fopen( IMC_COLOR_FILE, "r" ) ) )
    {
        imclog( "%s", "No color table found." );
        return;
    }

    for ( ; ; )
    {
        char letter;
        char *word;

        letter = imcfread_letter( fp );
        if ( letter == '*' )
        {
            imcfread_to_eol( fp );
            continue;
        }

        if ( letter != '#' )
        {
            imcbug( "%s", "imc_load_color_table: # not found." );
            break;
        }

        word = imcfread_word( fp );
        if ( !strcasecmp( word, "COLOR" ) )
        {
            IMCCREATE( color, IMC_COLOR, 1 );
            imc_readcolor( color, fp );
            IMCLINK( color, first_imc_color, last_imc_color, next, prev );
            continue;
        }
        else if( !strcasecmp( word, "END" ) )
            break;
        else
        {
            imcbug( "imc_load_color_table: bad section: %s.", word );
            continue;
        }
    }
    IMCFCLOSE( fp );
    return;
}

void imc_savehelps( void )
{
    FILE *fp;
    IMC_HELP_DATA *help;

    if( ( fp = fopen( IMC_HELP_FILE, "w" ) ) == NULL )
    {
        imclog( "%s", "Couldn't write to IMC2 help file." );
        return;
    }

    for( help = first_imc_help; help; help = help->next )
    {
        fprintf( fp, "%s", "#HELP\n" );
        fprintf( fp, "Name %s\n", help->name );
        fprintf( fp, "Perm %s\n", imcperm_names[help->level] );
        fprintf( fp, "Text %s¢\n", help->text );
        fprintf( fp, "%s", "End\n\n" );
    }
    fprintf( fp, "%s", "#END\n" );
    IMCFCLOSE( fp );
    return;
}

void imc_readhelp( IMC_HELP_DATA *help, FILE *fp )
{
    const char *word;
    char hbuf[LGST];
    int permvalue;
    bool fMatch;

    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : imcfread_word( fp );
        fMatch = FALSE;

        switch( word[0] )
        {
            case '*':
                fMatch = TRUE;
                imcfread_to_eol( fp );
                break;

            case 'E':
                if( !strcasecmp( word, "End" ) )
                    return;
                break;

            case 'N':
                IMCKEY( "Name",     help->name,       imcfread_line( fp ) );
                break;

            case 'P':
                if( !strcasecmp( word, "Perm" ) )
                {
                    word = imcfread_word( fp );
                    permvalue = get_imcpermvalue( word );

                    if( permvalue < 0 || permvalue > IMCPERM_IMP )
                    {
                        imcbug( "imc_readhelp: Command %s loaded with invalid permission. Set to Imp.", help->name );
                        help->level = IMCPERM_IMP;
                    }
                    else
                        help->level = permvalue;
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'T':
                if( !strcasecmp( word, "Text" ) )
                {
                    int num = 0;

                    while( ( hbuf[num] = fgetc( fp ) ) != EOF && hbuf[num] != '¢' && num < (LGST-2) )
                        num++;
                    hbuf[num] = '\0';
                    help->text = IMCSTRALLOC( hbuf );
                    fMatch = TRUE;
                    break;
                }
                IMCKEY( "Text",         help->text,       imcfread_line( fp ) );
                break;
        }
        if( !fMatch )
            imcbug( "imc_readhelp: no match: %s", word );
    }
}

void imc_load_helps( void )
{
    FILE *fp;
    IMC_HELP_DATA *help;

    first_imc_help = last_imc_help = NULL;

    imclog( "%s", "Loading IMC2 help file..." );

    if( !( fp = fopen( IMC_HELP_FILE, "r" ) ) )
    {
        imclog( "%s", "No help file found." );
        return;
    }

    for ( ; ; )
    {
        char letter;
        char *word;

        letter = imcfread_letter( fp );
        if ( letter == '*' )
        {
            imcfread_to_eol( fp );
            continue;
        }

        if ( letter != '#' )
        {
            imcbug( "%s", "imc_load_helps: # not found." );
            break;
        }

        word = imcfread_word( fp );
        if( !strcasecmp( word, "HELP" ) )
        {
            IMCCREATE( help, IMC_HELP_DATA, 1 );
            imc_readhelp( help, fp );
            IMCLINK( help, first_imc_help, last_imc_help, next, prev );
            continue;
        }
        else if( !strcasecmp( word, "END" ) )
            break;
        else
        {
            imcbug( "imc_load_helps: bad section: %s.", word );
            continue;
        }
    }
    IMCFCLOSE( fp );
    return;
}

void imc_savecommands( void )
{
    FILE *fp;
    IMC_CMD_DATA *cmd;
    IMC_ALIAS *alias;

    if( !( fp = fopen( IMC_CMD_FILE, "w" ) ) )
    {
        imclog( "%s", "Couldn't write to IMC2 command file." );
        return;
    }

    for( cmd = first_imc_command; cmd; cmd = cmd->next )
    {
        fprintf( fp, "%s", "#COMMAND\n" );
        fprintf( fp, "Name      %s\n", cmd->name );
        if( cmd->function != NULL )
            fprintf( fp, "Code      %s\n", imc_funcname( cmd->function ) );
        else
            fprintf( fp, "%s", "Code      NULL\n" );
        fprintf( fp, "Perm      %s\n", imcperm_names[cmd->level] );
        fprintf( fp, "Connected %d\n", cmd->connected );
        for( alias = cmd->first_alias; alias; alias = alias->next )
            fprintf( fp, "Alias     %s\n", alias->name );
        fprintf( fp, "%s", "End\n\n" );
    }
    fprintf( fp, "%s", "#END\n" );
    IMCFCLOSE( fp );
    return;
}

void imc_readcommand( IMC_CMD_DATA *cmd, FILE *fp )
{
    IMC_ALIAS *alias;
    const char *word;
    int permvalue;
    bool fMatch;

    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : imcfread_word( fp );
        fMatch = FALSE;

        switch( word[0] )
        {
            case '*':
                fMatch = TRUE;
                imcfread_to_eol( fp );
                break;

            case 'E':
                if( !strcasecmp( word, "End" ) )
                    return;
                break;

            case 'A':
                if( !strcasecmp( word, "Alias" ) )
                {
                    IMCCREATE( alias, IMC_ALIAS, 1 );
                    alias->name = imcfread_line( fp );
                    IMCLINK( alias, cmd->first_alias, cmd->last_alias, next, prev );
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'C':
                IMCKEY( "Connected",    cmd->connected,   imcfread_number( fp ) );
                if( !strcasecmp( word, "Code" ) )
                {
                    word = imcfread_word( fp );
                    cmd->function = imc_function( word );
                    if( cmd->function == NULL )
                        imcbug( "imc_readcommand: Command %s loaded with invalid function. Set to NULL.", cmd->name );
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'N':
                IMCKEY( "Name",     cmd->name,      imcfread_line( fp ) );
                break;

            case 'P':
                if( !strcasecmp( word, "Perm" ) )
                {
                    word = imcfread_word( fp );
                    permvalue = get_imcpermvalue( word );

                    if( permvalue < 0 || permvalue > IMCPERM_IMP )
                    {
                        imcbug( "imc_readcommand: Command %s loaded with invalid permission. Set to Imp.", cmd->name );
                        cmd->level = IMCPERM_IMP;
                    }
                    else
                        cmd->level = permvalue;
                    fMatch = TRUE;
                    break;
                }
                break;
        }
        if( !fMatch )
            imcbug( "imc_readcommand: no match: %s", word );
    }
}

bool imc_load_commands( void )
{
    FILE *fp;
    IMC_CMD_DATA *cmd;

    first_imc_command = last_imc_command = NULL;

    imclog( "%s", "Loading IMC2 command table..." );

    if( !( fp = fopen( IMC_CMD_FILE, "r" ) ) )
    {
        imclog( "%s", "No command table found." );
        return FALSE;
    }

    for ( ; ; )
    {
        char letter;
        char *word;

        letter = imcfread_letter( fp );
        if ( letter == '*' )
        {
            imcfread_to_eol( fp );
            continue;
        }

        if ( letter != '#' )
        {
            imcbug( "%s", "imc_load_commands: # not found." );
            break;
        }

        word = imcfread_word( fp );
        if ( !strcasecmp( word, "COMMAND" ) )
        {
            IMCCREATE( cmd, IMC_CMD_DATA, 1 );
            imc_readcommand( cmd, fp );
            IMCLINK( cmd, first_imc_command, last_imc_command, next, prev );
            continue;
        }
        else if( !strcasecmp( word, "END" ) )
            break;
        else
        {
            imcbug( "imc_load_commands: bad section: %s.", word );
            continue;
        }
    }
    IMCFCLOSE( fp );
    return TRUE;
}

void imc_readucache( IMCUCACHE_DATA *user, FILE *fp )
{
    const char *word;
    bool fMatch;

    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : imcfread_word( fp );
        fMatch = FALSE;

        switch( word[0] )
        {
            case '*':
                fMatch = TRUE;
                imcfread_to_eol( fp );
                break;

            case 'N':
                IMCKEY( "Name",     user->name,     imcfread_line( fp ) );
                break;

            case 'S':
                IMCKEY( "Sex",      user->gender,   imcfread_number( fp ) );
                break;

            case 'T':
                IMCKEY( "Time",     user->time,     imcfread_number( fp ) );
                break;

            case 'E':
                if( !strcasecmp( word, "End" ) )
                    return;
                break;
        }
        if( !fMatch )
            imcbug( "imc_readucache: no match: %s", word );
    }
}

void imc_load_ucache( void )
{
    FILE *fp;
    IMCUCACHE_DATA *user;

    imclog( "%s", "Loading ucache data..." );

    if( !( fp = fopen( IMC_UCACHE_FILE, "r" ) ) )
    {
        imclog( "%s", "No ucache data found." );
        return;
    }

    for ( ; ; )
    {
        char letter;
        char *word;

        letter = imcfread_letter( fp );
        if ( letter == '*' )
        {
            imcfread_to_eol( fp );
            continue;
        }

        if ( letter != '#' )
        {
            imcbug( "%s", "imc_load_ucahe: # not found." );
            break;
        }

        word = imcfread_word( fp );
        if ( !strcasecmp( word, "UCACHE" ) )
        {
            IMCCREATE( user, IMCUCACHE_DATA, 1 );
            imc_readucache( user, fp );
            IMCLINK( user, first_imcucache, last_imcucache, next, prev );
            continue;
        }
        else if( !strcasecmp( word, "END" ) )
            break;
        else
        {
            imcbug( "imc_load_ucache: bad section: %s.", word );
            continue;
        }
    }
    IMCFCLOSE( fp );
    imc_prune_ucache();
    imcucache_clock = current_time + 86400;
    return;
}

void imc_save_config( void )
{
    FILE *fp;

    if( ( fp = fopen( IMC_CONFIG_FILE, "w" ) ) == NULL )
    {
        imclog( "%s", "Couldn't write to config file." );
        return;
    }

    fprintf( fp, "%s", "$IMCCONFIG\n\n" );
    fprintf( fp, "# %sconfig file.\n", IMC_VERSION_ID );
    fprintf( fp, "%s", "# This file can now support the use of tildes in your strings.\n" );
    fprintf( fp, "%s", "# This information can be edited online using the 'imcconfig' command.\n" );
    fprintf( fp, "LocalName      %s\n", this_imcmud->localname );
    fprintf( fp, "Autoconnect    %d\n", this_imcmud->autoconnect );
    fprintf( fp, "MinPlayerLevel %d\n", this_imcmud->minlevel );
    fprintf( fp, "MinImmLevel    %d\n", this_imcmud->immlevel );
    fprintf( fp, "AdminLevel     %d\n", this_imcmud->adminlevel );
    fprintf( fp, "Implevel       %d\n", this_imcmud->implevel );
    fprintf( fp, "InfoName       %s\n", this_imcmud->fullname );
    fprintf( fp, "InfoHost       %s\n", this_imcmud->ihost );
    fprintf( fp, "InfoPort       %d\n", this_imcmud->iport );
    fprintf( fp, "InfoEmail      %s\n", this_imcmud->email );
    fprintf( fp, "InfoBase       %s\n", this_imcmud->base );
    fprintf( fp, "InfoWWW        %s\n", this_imcmud->www );
    fprintf( fp, "InfoDetails    %s\n\n", this_imcmud->details );
    fprintf( fp, "%s", "# Your router connection information goes here.\n" );
    fprintf( fp, "%s", "# This information should be available from the network you plan to join.\n" );
    fprintf( fp, "RouterAddr     %s\n", this_imcmud->rhost );
    fprintf( fp, "RouterPort     %d\n", this_imcmud->rport );
    fprintf( fp, "ClientPwd      %s\n", this_imcmud->clientpw );
    fprintf( fp, "ServerPwd      %s\n", this_imcmud->serverpw );
    fprintf( fp, "#MD5 auth: 0 = disabled, 1 = enabled\n" );
    fprintf( fp, "MD5            %d\n", this_imcmud->md5 );
    if( this_imcmud->md5pass )
    {
        fprintf( fp, "%s", "#Your router is expecting MD5 authentication now. Do not remove this line unless told to do so.\n" );
        fprintf( fp, "MD5Pwd         %d\n", this_imcmud->md5pass );
    }
    fprintf( fp, "%s", "End\n\n" );
    fprintf( fp, "%s", "$END\n" );
    IMCFCLOSE( fp );
    return;
}

void imcfread_config_file( FILE *fin )
{
    const char *word;
    bool fMatch;

    for( ;; )
    {
        word   = feof( fin ) ? "end" : imcfread_word( fin );
        fMatch = FALSE;

        switch( word[0] )
        {
            case '#':
                fMatch = TRUE;
                imcfread_to_eol( fin );
                break;

            case 'A':
                IMCKEY( "Autoconnect",  this_imcmud->autoconnect,   imcfread_number( fin ) );
                IMCKEY( "AdminLevel",   this_imcmud->adminlevel,    imcfread_number( fin ) );
                break;

            case 'C':
                IMCKEY( "ClientPwd",    this_imcmud->clientpw,      imcfread_line( fin ) );
                break;

            case 'E':
                if( !strcasecmp( word, "End" ) )
                {
                #ifdef IMCCHRONICLES
                    char lbuf1[LGST], lbuf2[LGST];

                    snprintf( lbuf1, LGST, "%s %s.%s", CODEBASE_VERSION_TITLE, CODEBASE_VERSION_MAJOR, CODEBASE_VERSION_MINOR );
                    if( this_imcmud->base )
                        IMCSTRFREE( this_imcmud->base );
                    this_imcmud->base = IMCSTRALLOC( lbuf1 );

                    snprintf( lbuf2, LGST, "%s%s", IMC_VERSION_ID, this_imcmud->base );
                    IMC_VERSIONID = IMCSTRALLOC( lbuf2 );
                #endif
                    return;
                }
                break;

            case 'I':
                IMCKEY( "Implevel",     this_imcmud->implevel,  imcfread_number( fin ) );
                IMCKEY( "InfoName", this_imcmud->fullname,  imcfread_line( fin ) );
                IMCKEY( "InfoHost", this_imcmud->ihost, imcfread_line( fin ) );
                IMCKEY( "InfoPort", this_imcmud->iport, imcfread_number( fin ) );
                IMCKEY( "InfoEmail",    this_imcmud->email, imcfread_line( fin ) );
                IMCKEY( "InfoWWW",  this_imcmud->www,       imcfread_line( fin ) );
                IMCKEY( "InfoDetails",  this_imcmud->details,   imcfread_line( fin ) );
                IMCKEY( "InfoBase", this_imcmud->base,  imcfread_line( fin ) );
                break;

            case 'L':
                IMCKEY( "LocalName",       this_imcmud->localname,    imcfread_line( fin ) );
                break;

            case 'M':
                IMCKEY( "MD5",             this_imcmud->md5,          imcfread_number( fin ) );
                IMCKEY( "MD5Pwd",          this_imcmud->md5pass,      imcfread_number( fin ) );
                IMCKEY( "MinImmLevel",     this_imcmud->immlevel,     imcfread_number( fin ) );
                IMCKEY( "MinPlayerLevel",  this_imcmud->minlevel,     imcfread_number( fin ) );
                break;

            case 'R':
                IMCKEY( "RouterAddr",   this_imcmud->rhost,           imcfread_line( fin ) );
                IMCKEY( "RouterPort",   this_imcmud->rport,           imcfread_number( fin ) );
                break;

            case 'S':
                IMCKEY( "ServerPwd",    this_imcmud->serverpw,  imcfread_line( fin ) );
                break;
        }
        if( !fMatch )
            imcbug( "imcfread_config_file: Bad keyword: %s\n\r", word );
    }
}

bool imc_read_config( void )
{
    FILE *fin;
    char cbase[SMST];

    if( this_imcmud != NULL )
        imc_delete_info();
    this_imcmud = NULL;

    IMC_VERSIONID = NULL;

    imclog( "%s", "Loading IMC2 network data..." );

    if( !( fin = fopen( IMC_CONFIG_FILE, "r" ) ) )
    {
        imclog( "%s", "Can't open configuration file" );
        imclog( "%s", "Network configuration aborted." );
        return FALSE;
    }

    for( ; ; )
    {
        char letter;
        char *word;

        letter = imcfread_letter( fin );

        if( letter == '#' )
        {
            imcfread_to_eol( fin );
            continue;
        }

        if( letter != '$' )
        {
            imcbug( "%s", "imc_read_config: $ not found" );
            break;
        }

        word = imcfread_word( fin );
        if( !strcasecmp( word, "IMCCONFIG" ) && this_imcmud == NULL )
        {
            IMCCREATE( this_imcmud, SITEINFO, 1 );

            /* If someone can think of better default values, I'm all ears. Until then, keep your bitching to yourselves. */
            this_imcmud->minlevel = 10;
            this_imcmud->immlevel = 101;
            this_imcmud->adminlevel = 113;
            this_imcmud->implevel = 115;
            this_imcmud->network = IMCSTRALLOC( "Unknown" );
            this_imcmud->md5     = TRUE;
            this_imcmud->md5pass = FALSE;

            imcfread_config_file( fin );
            continue;
        }
        else if( !strcasecmp( word, "END" ) )
            break;
        else
        {
            imcbug( "imc_read_config: Bad section in config file: %s", word );
            continue;
        }
    }
    IMCFCLOSE( fin );

    if( !this_imcmud )
    {
        imcbug( "%s", "imc_read_config: No router connection information!!" );
        imcbug( "%s", "Network configuration aborted." );
        return FALSE;
    }

    if( !this_imcmud->rhost || !this_imcmud->clientpw || !this_imcmud->serverpw )
    {
        imcbug( "%s", "imc_read_config: Missing required configuration info." );
        imcbug( "%s", "Network configuration aborted." );
        return FALSE;
    }

    if( !this_imcmud->localname || this_imcmud->localname[0] == '\0' )
    {
        imcbug( "%s", "imc_read_config: Mud name not loaded in configuration file." );
        imcbug( "%s", "Network configuration aborted." );
        return FALSE;
    }

    if( !this_imcmud->fullname || this_imcmud->fullname[0] == '\0' )
    {
        imcbug( "%s", "imc_read_config: Missing InfoName parameter in configuration file." );
        imcbug( "%s", "Network configuration aborted." );
        return FALSE;
    }

    if( !this_imcmud->ihost || this_imcmud->ihost[0] == '\0' )
    {
        imcbug( "%s", "imc_read_config: Missing InfoHost parameter in configuration file." );
        imcbug( "%s", "Network configuration aborted." );
        return FALSE;
    }

    if( !this_imcmud->email || this_imcmud->email[0] == '\0' )
    {
        imcbug( "%s", "imc_read_config: Missing InfoEmail parameter in configuration file." );
        imcbug( "%s", "Network configuration aborted." );
        return FALSE;
    }

    if( !this_imcmud->base || this_imcmud->base[0] == '\0' )
        this_imcmud->base = IMCSTRALLOC( "Unknown Codebase" );

    if( !this_imcmud->www || this_imcmud->www[0] == '\0' )
        this_imcmud->www = IMCSTRALLOC( "Not specified" );

    if( !this_imcmud->details || this_imcmud->details[0] == '\0' )
        this_imcmud->details = IMCSTRALLOC( "No details provided." );

    if( !IMC_VERSIONID )
    {
        snprintf( cbase, SMST, "%s%s", IMC_VERSION_ID, this_imcmud->base );
        IMC_VERSIONID = IMCSTRALLOC( cbase );
    }
    this_imcmud->disconnect = FALSE;
    return TRUE;
}

/* connect to router */
bool imc_connect_to( void )
{
    struct sockaddr_in sa;
    struct hostent *hostp;
    char buf[IMC_DATA_LENGTH];
    int r, desc;

    if( imc_active == IA_NONE )
    {
        imcbug( "%s", "IMC is not active" );
        return FALSE;
    }

    if( !this_imcmud )
    {
        imcbug( "%s", "No connection data loaded" );
        return FALSE;
    }

    if( this_imcmud->desc > 0 )
    {
        imcbug( "%s", "Already connected" );
        return FALSE;
    }

    imclog( "%s", "Connecting to router." );

    memset( &sa, 0, sizeof( sa ) );
    sa.sin_family = AF_INET;

    /* warning: this blocks. It would be better to farm the query out to
     * another process, but that is difficult to do without lots of changes
     * to the core mud code. You may want to change this code if you have an
     * existing resolver process running.
     */
    if( !inet_aton( this_imcmud->rhost, &sa.sin_addr ) )
    {
        hostp = gethostbyname( this_imcmud->rhost );
        if( !hostp )
        {
            imclog( "%s", "imc_connect_to: Cannot resolve router hostname." );
            imc_shutdown( FALSE );
            return FALSE;
        }
        memcpy( &sa.sin_addr, hostp->h_addr, hostp->h_length );
    }

    sa.sin_port = htons( this_imcmud->rport );

    desc = socket( AF_INET, SOCK_STREAM, 0 );
    if( desc < 0 )
    {
        perror( "socket" );
        return FALSE;
    }

    r = fcntl( desc, F_GETFL, 0 );
    if( r < 0 || fcntl( desc, F_SETFL, O_NONBLOCK | r ) < 0 )
    {
        perror( "imc_connect: fcntl" );
        close( desc );
        return FALSE;
    }

    if( connect( desc, (struct sockaddr *)&sa, sizeof(sa) ) < 0 )
    {
        if( errno != EINPROGRESS )
        {
            perror( "connect" );
            close( desc );
            return FALSE;
        }
    }

    this_imcmud->state    = IMC_CONNECTING;
    this_imcmud->desc     = desc;
    this_imcmud->insize   = IMC_MINBUF;
    IMCCREATE( this_imcmud->inbuf, char, this_imcmud->insize );
    this_imcmud->outsize  = IMC_MINBUF;
    IMCCREATE( this_imcmud->outbuf, char, this_imcmud->outsize );
    this_imcmud->inbuf[0] = this_imcmud->outbuf[0] = '\0';
    this_imcmud->newoutput = 0;

    /* The MUD is electing to enable MD5 - this is the default setting */
    if( this_imcmud->md5 )
    {
        /* No MD5 setup enabled.
         * Situations where this might happen:
         *
         * 1. You are connecting for the first time. This is expected.
         * 2. You are connecting to an older router which does not support it, so you will continue connecting this way.
         * 3. You got stupid and deleted the MD5 line in your config file after it got there. Ooops.
         * 4. The router lost your data. In which case you'll need to do #3 because authentication will fail.
         * 5. You let your connection lapse, and #4 happened because of it.
         * 6. Gremlins. When in doubt, blame them.
         */
        if( !this_imcmud->md5pass )
        {
            snprintf( buf, IMC_DATA_LENGTH, "PW %s %s version=%d autosetup %s MD5",
                this_imcmud->localname, this_imcmud->clientpw, IMC_VERSION, this_imcmud->serverpw );
        }
        /* You have MD5 working. Excellent. Lets send the new packet for it.
         * Situations where this will fail:
         *
         * 1. You're a new connection, and for whatever dumb reason, the MD5 line is in your config already.
         * 2. You have MD5 enabled and you're switching to a new router. This is generally not going to work well.
         * 3. Something happened and the hashing failed. Resulting in authentication failure. Ooops.
         * 4. The router lost your connection data.
         * 5. You let your connection lapse, and #4 happened because of it.
         * 6. Gremlins. When in doubt, blame them.
         */
        else
            snprintf( buf, IMC_PACKET_LENGTH, "MD5-AUTH-REQ %s", this_imcmud->localname );
    }
    /* The MUD is electing not to use MD5 for whatever reason - this must be specifically set */
    else
        snprintf( buf, IMC_DATA_LENGTH, "PW %s %s version=%d autosetup %s",
            this_imcmud->localname, this_imcmud->clientpw, IMC_VERSION, this_imcmud->serverpw );

    do_imcsend( buf );
    return TRUE;
}

void free_imcdata( bool complete )
{
    REMOTEINFO *p, *pnext;
    IMC_BAN *ban, *ban_next;
    IMCUCACHE_DATA *ucache, *next_ucache;
    IMC_CMD_DATA *cmd, *cmd_next;
    IMC_ALIAS *alias, *alias_next;
    IMC_HELP_DATA *help, *help_next;
    IMC_COLOR *color, *color_next;
    IMC_PHANDLER *ph, *ph_next;

    for( p = first_rinfo; p; p = pnext )
    {
        pnext = p->next;
        imc_delete_reminfo( p );
    }

    for( ban = first_imc_ban; ban; ban = ban_next )
    {
        ban_next = ban->next;
        imc_freeban( ban );
    }

    for( ucache = first_imcucache; ucache; ucache = next_ucache )
    {
        next_ucache = ucache->next;
        IMCSTRFREE( ucache->name );
        IMCUNLINK( ucache, first_imcucache, last_imcucache, next, prev );
        IMCDISPOSE( ucache );
    }

    /* This stuff is only killed off if the mud itself shuts down. For those of you Valgrinders out there. */
    if( complete )
    {
        for( cmd = first_imc_command; cmd; cmd = cmd_next )
        {
            cmd_next = cmd->next;

            for( alias = cmd->first_alias; alias; alias = alias_next )
            {
                alias_next = alias->next;

                IMCSTRFREE( alias->name );
                IMCUNLINK( alias, cmd->first_alias, cmd->last_alias, next, prev );
                IMCDISPOSE( alias );
            }
            IMCSTRFREE( cmd->name );
            IMCUNLINK( cmd, first_imc_command, last_imc_command, next, prev );
            IMCDISPOSE( cmd );
        }

        for( help = first_imc_help; help; help = help_next )
        {
            help_next = help->next;
            IMCSTRFREE( help->name );
            IMCSTRFREE( help->text );
            IMCUNLINK( help, first_imc_help, last_imc_help, next, prev );
            IMCDISPOSE( help );
        }

        for( color = first_imc_color; color; color = color_next )
        {
            color_next = color->next;
            IMCSTRFREE( color->name );
            IMCSTRFREE( color->mudtag );
            IMCSTRFREE( color->imctag );
            IMCUNLINK( color, first_imc_color, last_imc_color, next, prev );
            IMCDISPOSE( color );
        }

        for( ph = first_phandler; ph; ph = ph_next )
        {
            ph_next = ph->next;

            IMCSTRFREE( ph->name );
            IMCUNLINK( ph, first_phandler, last_phandler, next, prev );
            IMCDISPOSE( ph );
        }
    }
    return;
}

void imc_shutdown_network( void )
{
    if( imc_active < IA_UP )
        return;

    imclog( "%s", "Shutting down network" );
    imclog( "rx %ld packets, %ld bytes", imc_stats.rx_pkts, imc_stats.rx_bytes );
    imclog( "tx %ld packets, %ld bytes", imc_stats.tx_pkts, imc_stats.tx_bytes );
    imclog( "largest packet %d bytes", imc_stats.max_pkt );
    imclog( "dropped %d packets by sequence number", imc_stats.sequence_drops );

    if( this_imcmud->desc > 0 )
        close( this_imcmud->desc );
    this_imcmud->desc = -1;

    imc_savehistory();
    imc_shutdownchannels();
    imc_active = IA_CONFIG2;
}

/* Shutdown IMC2 */
void imc_shutdown( bool reconnect )
{
    if( imc_active == IA_NONE )
        return;

    if( imc_active >= IA_UP )
        imc_shutdown_network( );

    imc_active = IA_NONE;
    free_imcdata( FALSE );

    if( reconnect )
    {
        imcwait = 100;                                      /* About 20 seconds or so */
        imclog( "%s", "Connection to router was lost. Reconnecting in approximately 20 seconds." );
    }
}

/* Startup IMC2 */
bool imc_startup_network( void )
{
    if( imc_active != IA_CONFIG2 )
    {
        imcbug( "imc_startup_network: called with imc_active == %d", imc_active );
        return FALSE;
    }

    imclog( "%s", "IMC2 Network Initializing..." );
    this_imcmud->desc = -1;

    imc_active = IA_UP;

    imc_stats.rx_pkts  = 0;
    imc_stats.tx_pkts  = 0;
    imc_stats.rx_bytes = 0;
    imc_stats.tx_bytes = 0;
    imc_stats.sequence_drops = 0;

    /* Connect to router */
    if( !imc_connect_to() )
        return FALSE;

    return TRUE;
}

void imc_startup( bool force )
{
    if( imc_active != IA_NONE )
    {
        imclog( "imc_startup: called with imc_active = %d", imc_active );
        return;
    }

    imc_now = time( NULL );                                 /* start our clock */
    imc_boot = imc_now;

    imc_sequencenumber = imc_now;

    /* The Command table is required for operation. Like.... duh? */
    if( first_imc_command == NULL )
    {
        if( !imc_load_commands() )
        {
            imcbug( "%s", "imc_startup: Unable to load command table!" );
            imc_active = IA_NONE;
            return;
        }
    }

    /* Configuration required for network operation. */
    if( !imc_read_config() )
    {
        imc_active = IA_NONE;
        return;
    }

    /* Help information should persist even when the network is not connected... */
    if( first_imc_help == NULL )
        imc_load_helps();

    /* ... as should the color table. */
    if( first_imc_color == NULL )
        imc_load_color_table();

    if( !this_imcmud->autoconnect && !force )
    {
        imclog( "%s", "IMC2 data loaded. Autoconnect not set. IMC2 will need to be connected manually." );
        return;
    }

    imc_active = this_imcmud->localname ? IA_CONFIG2 : IA_CONFIG1;

    if( imc_active == IA_CONFIG2 && ( this_imcmud->autoconnect || force ) )
    {
        if( imc_startup_network() )
        {
            imc_loadchannels();
            imc_loadhistory();
            imc_readbans();
            imc_load_ucache();
            return;
        }
        imc_active = IA_NONE;
    }
    return;
}

/*****************************************
 * User level commands and social hooks. *
 *****************************************/

IMC_CMD( imccommand )
{
    char cmd[IMC_NAME_LENGTH];
    char chan[IMC_NAME_LENGTH];
    char data[IMC_DATA_LENGTH];
    char *p;
    PACKET out;
    IMC_CHANNEL *c;

    p = imc_getarg( argument, cmd, IMC_NAME_LENGTH );
    p = imc_getarg( p, chan, IMC_NAME_LENGTH );
    imcstrlcpy( data, p, IMC_DATA_LENGTH );

    if( !cmd[0] || !chan[0] )
    {
        imc_to_char( "Syntax: imccommand <command> <router:channel> [<data..>]\n\r", ch );
        imc_to_char( "Command access will depend on your privledges and what each individual router allows.\n\r", ch );
        return;
    }

    p = strchr( chan, ':' );
    if( !p )
    {
        c = imc_findlchannel( chan );
        if( c )
            imcstrlcpy( chan, c->name, IMC_NAME_LENGTH );
    }

    imc_initdata( &out );
    snprintf( out.to, IMC_NAME_LENGTH, "IMC@%s", channel_mudof( chan ) );
    imcstrlcpy( out.type, "ice-cmd", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, CH_IMCNAME(ch), IMC_NAME_LENGTH );
    imc_addkey( &out, "channel", chan );
    imc_addkey( &out, "command", cmd );
    imc_addkey( &out, "data", data );

    imc_send( &out );
    imc_freedata( &out );

    imc_to_char( "Command sent.\n\r", ch );
    return;
}

void imcformat_channel( CHAR_DATA *ch, IMC_CHANNEL *d, int format, bool all )
{
    IMC_CHANNEL *c = NULL;
    char buf[LGST];

    if( all )
    {
        for( c = first_imc_channel; c; c = c->next )
        {
            if( !c->local_name || c->local_name[0] == '\0' )
                continue;

            if( format == 1 || format == 4 )
            {
                snprintf( buf, LGST, "~R[~Y%s~R] ~C%%s: ~c%%s", c->local_name );
                IMCSTRFREE( c->regformat );
                c->regformat = IMCSTRALLOC( buf );
            }
            if( format == 2 || format == 4 )
            {
                snprintf( buf, LGST, "~R[~Y%s~R] ~c%%s %%s", c->local_name );
                IMCSTRFREE( c->emoteformat );
                c->emoteformat = IMCSTRALLOC( buf );
            }
            if( format == 3 || format == 4 )
            {
                snprintf( buf, LGST, "~R[~Y%s~R] ~c%%s", c->local_name );
                IMCSTRFREE( c->socformat );
                c->socformat = IMCSTRALLOC( buf );
            }
        }
    }
    else
    {
        if( !d->local_name || d->local_name[0] == '\0' )
        {
            imc_to_char( "This channel is not yet locally configured.\n\r", ch );
            return;
        }

        if( format == 1 || format == 4 )
        {
            snprintf( buf, LGST, "~R[~Y%s~R] ~C%%s: ~c%%s", d->local_name );
            IMCSTRFREE( d->regformat );
            d->regformat = IMCSTRALLOC( buf );
        }
        if( format == 2 || format == 4 )
        {
            snprintf( buf, LGST, "~R[~Y%s~R] ~c%%s %%s", d->local_name );
            IMCSTRFREE( d->emoteformat );
            d->emoteformat = IMCSTRALLOC( buf );
        }
        if( format == 3 || format == 4 )
        {
            snprintf( buf, LGST, "~R[~Y%s~R] ~c%%s", d->local_name );
            IMCSTRFREE( d->socformat );
            d->socformat = IMCSTRALLOC( buf );
        }
    }
    imc_save_channels();
    return;
}

IMC_CMD( imcsetup )
{
    char imccmd[IMC_NAME_LENGTH];
    char chan[IMC_NAME_LENGTH];
    char arg1[IMC_DATA_LENGTH];
    char buf[LGST];
    IMC_CHANNEL *c = NULL;
    char *a, *a1;
    int x;
    bool all = FALSE;

    a = imc_getarg( argument, imccmd, IMC_NAME_LENGTH );
    a = a1 = imc_getarg( a, chan, IMC_NAME_LENGTH );
    a = imc_getarg( a, arg1, IMC_DATA_LENGTH );

    if( !imccmd || imccmd[0] == '\0' || !chan || chan[0] == '\0' )
    {
        imc_to_char( "Syntax: imcsetup <command> <channel> [<data..>]\n\r", ch );
        imc_to_char( "Where 'command' is one of the following:\n\r", ch );
        imc_to_char( "delete setlocal rename perm regformat emoteformat socformat\n\r\n\r", ch );
        imc_to_char( "Where 'channel' is one of the following:\n\r", ch );
        for( c = first_imc_channel; c; c = c->next )
        {
            if( c->local_name && c->local_name[0] != '\0' )
                imc_printf( ch, "%s ", c->local_name );
            else
                imc_printf( ch, "%s ", c->name );
        }
        imc_to_char( "\n\r", ch );
        return;
    }

    if( !strcasecmp( chan, "all" ) )
        all = TRUE;
    else
    {
        c = imc_findchannel( chan );
        if( !c )
        {
            c = imc_findlchannel( chan );

            if( !c )
            {
                imc_to_char( "Unknown channel.\n\r", ch );
                return;
            }
        }
    }

    /* Permission check -- Xorith */
    if( c && c->level > IMCPERM(ch) )
    {
        imc_to_char( "You cannot modify that channel.", ch );
        return;
    }

    /* Support for "add" argument put back for legacy isetup users */
    if( !strcasecmp( imccmd, "setlocal" ) || !strcasecmp( imccmd, "add" ) )
    {
        if( !all )
        {
            if( c->local_name && c->local_name[0] != '\0' )
            {
                imc_printf( ch, "Channel %s is already locally configured as %s\n\r", c->name, c->local_name );
                return;
            }

            if( c->dlname && c->dlname[0] != '\0' && !arg1 && arg1[0] == '\0' )
                c->local_name = IMCSTRALLOC( c->dlname );

            else
            {
                if( !arg1 || arg1[0] == '\0' )
                {
                    imc_to_char( "You must specify a local channel name to complete this setup.\n\r", ch );
                    return;
                }
                else
                    c->local_name = IMCSTRALLOC( arg1 );
            }
            imcformat_channel( ch, c, 4, FALSE );
            imc_printf( ch, "Channel %s is now locally configured as %s\n\r", c->name, c->local_name );
        }
        else
        {
            for( c = first_imc_channel; c; c = c->next )
            {
                if( c->local_name && c->local_name[0] != '\0' )
                    continue;
                if( !c->dlname || c->dlname[0] == '\0' )
                    continue;
                c->local_name = IMCSTRALLOC( c->dlname );
            }
            imcformat_channel( ch, NULL, 4, TRUE );
            imc_to_char( "All known network channels have been configured locally.\n\r", ch );
        }
        imc_save_channels();
        return;
    }

    if( !strcasecmp( imccmd, "delete" ) )
    {
        if( all )
        {
            imc_to_char( "You cannot perform a delete all on channels.\n\r", ch );
            return;
        }
        IMCSTRFREE( c->local_name );
        IMCSTRFREE( c->regformat );
        IMCSTRFREE( c->emoteformat );
        IMCSTRFREE( c->socformat );

        for( x = 0; x < MAX_IMCHISTORY; x++ )
            IMCSTRFREE( c->history[x] );

        imc_to_char( "Channel is no longer locally configured.\n\r", ch );

        if( !c->refreshed )
            imc_freechan( c );
        imc_save_channels();
        return;
    }

    if( !strcasecmp( imccmd, "rename" ) )
    {
        if( all )
        {
            imc_to_char( "You cannot perform a rename all on channels.\n\r", ch );
            return;
        }

        if( !arg1 || arg1[0] == '\0' )
        {
                                                            /* Lets be more kind! -- X */
            imc_to_char( "Missing 'newname' argument for 'imcsetup rename'\n\r", ch );
                                                            /* Fixed syntax message -- X */
            imc_to_char( "Syntax: imcsetup rename <local channel> <newname>\n\r", ch );
            return;
        }

        if( imc_findlchannel( arg1 ) )
        {
            imc_to_char( "New channel name already exists.\n\r", ch );
            return;
        }

        /* Small change here to give better feedback to the ch -- Xorith */
        snprintf( buf, LGST, "Renamed channel '%s' to '%s'.\n\r", c->local_name, arg1 );
        IMCSTRFREE( c->local_name );
        c->local_name = IMCSTRALLOC( arg1 );
        imc_to_char( buf, ch );

        /* Reset the format with the new local name */
        imcformat_channel( ch, c, 4, FALSE );
        imc_save_channels();
        return;
    }

    if( !strcasecmp( imccmd, "resetformats" ) )
    {
        if( all )
        {
            imcformat_channel( ch, NULL, 4, TRUE );
            imc_to_char( "All channel formats have been reset to default.\n\r", ch );
        }
        else
        {
            imcformat_channel( ch, c, 4, FALSE );
            imc_to_char( "The formats for this channel have been reset to default.\n\r", ch );
        }
        return;
    }

    if( !strcasecmp( imccmd, "regformat" ) )
    {
        if( !a1[0] )
        {
                                                            /* Syntax Fix -- Xorith */
            imc_to_char( "Syntax: imcsetup regformat <localchannel|all> <string>\n\r", ch );
            return;
        }

        if( !verify_format( a1, 2 ) )
        {
            imc_to_char( "Bad format - must contain exactly 2 %s's.\n\r", ch );
            return;
        }

        if( all )
        {
            for( c = first_imc_channel; c; c = c->next )
            {
                IMCSTRFREE( c->regformat );
                c->regformat = IMCSTRALLOC( a1 );
            }
            imc_to_char( "All channel regular formats have been updated.\n\r", ch );
        }
        else
        {
            IMCSTRFREE( c->regformat );
            c->regformat = IMCSTRALLOC( a1 );
            imc_to_char( "The regular format for this channel has been changed successfully.\n\r", ch );
        }
        imc_save_channels();
        return;
    }

    if( !strcasecmp( imccmd, "emoteformat" ) )
    {
        if( !a1[0] )
        {
                                                            /* Syntax Fix -- Xorith */
            imc_to_char( "Syntax: imcsetup emoteformat <localchannel|all> <string>\n\r", ch );
            return;
        }

        if( !verify_format( a1, 2 ) )
        {
            imc_to_char( "Bad format - must contain exactly 2 %s's.\n\r", ch );
            return;
        }

        if( all )
        {
            for( c = first_imc_channel; c; c = c->next )
            {
                IMCSTRFREE( c->emoteformat );
                c->emoteformat = IMCSTRALLOC( a1 );
            }
            imc_to_char( "All channel emote formats have been updated.\n\r", ch );
        }
        else
        {
            IMCSTRFREE( c->emoteformat );
            c->emoteformat = IMCSTRALLOC( a1 );
            imc_to_char( "The emote format for this channel has been changed successfully.\n\r", ch );
        }
        imc_save_channels();
        return;
    }

    if( !strcasecmp( imccmd, "socformat" ) )
    {
        if( !a1[0] )
        {
                                                            /* Xorith */
            imc_to_char( "Syntax: imcsetup socformat <localchannel|all> <string>\n\r", ch );
            return;
        }

        if( !verify_format( a1, 1 ) )
        {
            imc_to_char( "Bad format - must contain exactly 1 %s.\n\r", ch );
            return;
        }

        if( all )
        {
            for( c = first_imc_channel; c; c = c->next )
            {
                IMCSTRFREE( c->socformat );
                c->socformat = IMCSTRALLOC( a1 );
            }
            imc_to_char( "All channel social formats have been updated.\n\r", ch );
        }
        else
        {
            IMCSTRFREE( c->socformat );
            c->socformat = IMCSTRALLOC( a1 );
            imc_to_char( "The social format for this channel has been changed successfully.\n\r", ch );
        }
        imc_save_channels();
        return;
    }

    if( !strcasecmp( imccmd, "perm" ) || !strcasecmp( imccmd, "permission" ) || !strcasecmp( imccmd, "level" ) )
    {
        int permvalue = -1;

        if( all )
        {
            imc_to_char( "You cannot do a permissions all for channels.\n\r", ch );
            return;
        }

        if( !arg1[0] )
        {
            imc_to_char( "Syntax: imcsetup perm <localchannel> <permission>\n\r", ch );
            return;
        }

        permvalue = get_imcpermvalue( arg1 );
        if( permvalue < 0 || permvalue > IMCPERM_IMP )
        {
            imc_to_char( "Unacceptable permission setting.\n\r", ch );
            return;
        }

        /* Added permission checking here -- Xorith */
        if( permvalue > IMCPERM(ch) )
        {
            imc_to_char( "You cannot set a permission higher than your own.\n\r", ch );
            return;
        }

        c->level = permvalue;

        imc_to_char( "Channel permissions changed.\n\r", ch );
        imc_save_channels();
        return;
    }
    imcsetup( ch, "" );
    return;
}

IMC_CMD( imcchanlist )
{
    char buf[LGST];
    IMC_CHANNEL *c;
    int count = 0;                                          /* Count -- Xorith */
    char col = 'C';                                         /* Listening Color -- Xorith */

    if( argument && argument[0] != '\0' )
    {
        c = imc_findlchannel( argument );
        if( !c )
            c = imc_findchannel( argument );

        if( !c )
        {
            imc_to_char( "No such channel.\n\r", ch );
            return;
        }

        if( IMCPERM(ch) < c->level )
        {
            imc_to_char( "No such channel.\n\r", ch );
            return;
        }

        if( !c->local_name && IMCPERM(ch) < c->level )
        {
            imc_to_char( "No such channel.\n\r", ch );
            return;
        }

        if( !c->refreshed )
        {
            imc_printf( ch, "%s has not been refreshed yet.\n\r", c->name );
            return;
        }

        snprintf( buf, LGST,
            "~WChannel %s:\n\r"
            "~c Local name: ~w%s\n\r"
            "~c Regformat : ~w%s\n\r"
            "~c Emformat  : ~w%s\n\r"
            "~c Socformat : ~w%s\n\r"
            "~c Perms     : ~w%s\n\r"
            "\n\r"
            "~c Policy    : ~w%s\n\r"
            "~c Owner     : ~w%s\n\r"
            "~c Operators : ~w%s\n\r"
            "~c Invited   : ~w%s\n\r"
            "~c Excluded  : ~w%s\n\r",

            c->name, c->local_name ? c->local_name : "", c->regformat ? c->regformat : "",
            c->emoteformat ? c->emoteformat : "", c->socformat ? c->socformat : "",
            imcperm_names[c->level], c->open ? "~gOpen" : "~yPrivate", c->owner, c->operators, c->invited, c->excluded );
        imc_to_char( buf, ch );
        return;
    }

    snprintf( buf, LGST, "~c%-15s ~C%-15s ~B%-15s ~b%-7s ~!%s\n\r", "Name",
        "Local name", "Owner", "Perm", "Policy" );

    for( c = first_imc_channel; c; c = c->next )
    {
        if( IMCPERM(ch) < c->level )
            continue;

        /* If it's locally configured and we're not listening, then color it red -- Xorith */
        if( c->local_name )
        {
            if( !imc_hasname( IMC_LISTEN(ch), c->local_name ) )
                col = 'R';
            else
                col = 'C';                                  /* Otherwise, keep it Cyan -- X */
        }

        snprintf( buf+strlen(buf), LGST,
            "~c%-15.15s ~%c%-*.*s ~B%-15.15s ~b%-7s %s\n\r", c->name, col,
            c->local_name ? 15 : 17, c->local_name ? 15 : 17,
            c->local_name ? c->local_name : "~Y(not local)  ", c->owner,
            imcperm_names[c->level], c->refreshed ? ( c->open ? "~gOpen" : "~yPrivate" ) : "~Runknown" );
        count++;                                            /* Keep a count -- Xorith */
    }
    /* Show the count and a bit of text explaining the red color -- Xorith */
    snprintf( buf+strlen(buf), LGST, "\n\r~W%d ~cchannels found." , count );
    imcstrlcat( buf+strlen(buf), "\n\r~RRed ~clocal name indicates a channel not being listened to.\n\r", LGST );
    imc_to_pager( buf, ch );
}

IMC_CMD( imclisten )
{
    IMC_CHANNEL *c;

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "~cCurrently tuned into:\n\r", ch );
        if( IMC_LISTEN(ch) && IMC_LISTEN(ch)[0] != '\0' )
            imc_printf( ch, "~W%s", IMC_LISTEN(ch) );
        else
            imc_to_char( "~WNone", ch );
        imc_to_char( "\n\r", ch );
        return;
    }

    if( !strcasecmp( argument, "all" ) )
    {
        for( c = first_imc_channel; c; c = c->next )
        {
            if( !c->local_name )
                continue;

            if( IMCPERM(ch) >= c->level && !imc_hasname( IMC_LISTEN(ch), c->local_name ) )
                imc_addname( &IMC_LISTEN(ch), c->local_name );
            imc_sendnotify( ch, c->local_name, TRUE );
        }
        imc_to_char( "~YYou are now listening to all available IMC2 channels.\n\r", ch );
        return;
    }

    if( !strcasecmp( argument, "none" ) )
    {
        for( c = first_imc_channel; c; c = c->next )
        {
            if( !c->local_name )
                continue;

            if( imc_hasname( IMC_LISTEN(ch), c->local_name ) )
                imc_removename( &IMC_LISTEN(ch), c->local_name );
            imc_sendnotify( ch, c->local_name, FALSE );
        }
        imc_to_char( "~YYou no longer listen to any available IMC2 channels.\n\r", ch );
        return;
    }

    if( !( c = imc_findlchannel( argument ) ) )
    {
        imc_to_char( "No such channel configured locally.\n\r", ch );
        return;
    }

    if( IMCPERM(ch) < c->level )
    {
        imc_to_char( "No such channel configured locally.\n\r", ch );
        return;
    }

    if( imc_hasname( IMC_LISTEN(ch), c->local_name ) )
    {
        imc_removename( &IMC_LISTEN(ch), c->local_name );
        imc_to_char( "Channel off.\n\r", ch );
        imc_sendnotify( ch, c->local_name, FALSE );
    }
    else
    {
        imc_addname( &IMC_LISTEN(ch), c->local_name );
        imc_to_char( "Channel on.\n\r", ch );
        imc_sendnotify( ch, c->local_name, TRUE );
    }
}

IMC_CMD( imctell )
{
    char buf[LGST], buf1[LGST];
    imc_char_data *chdata;

    if( IMCIS_SET( IMCFLAG(ch), IMC_DENYTELL ) )
    {
        imc_to_char( "You are not authorized to use imctell.\n\r", ch );
        return;
    }

    argument = imcone_argument( argument, buf );

    if( !argument || argument[0] == '\0' )
    {
        int x;

        imc_to_char( "Usage: imctell user@mud <message>\n\r", ch );
        imc_to_char( "Usage: imctell [on]/[off]\n\r\n\r", ch );
        imc_printf( ch, "&cThe last %d things you were told:\n\r", MAX_IMCTELLHISTORY );

        for( x = 0 ; x < MAX_IMCTELLHISTORY; x++ )
        {
            if( IMCTELLHISTORY(ch,x) == NULL )
                break;
            imc_to_char( IMCTELLHISTORY(ch,x), ch );
        }
        return;
    }

    if( !strcasecmp( argument, "on" ) )
    {
        IMCREMOVE_BIT( IMCFLAG(ch), IMC_TELL );
        imc_to_char( "You now send and receive imctells.\n\r", ch );
        return;
    }

    if( !strcasecmp( argument, "off" ) )
    {
        IMCSET_BIT( IMCFLAG(ch), IMC_TELL );
        imc_to_char( "You no longer send and receive imctells.\n\r", ch );
        return;
    }

    if( IMCIS_SET( IMCFLAG(ch), IMC_TELL ) )
    {
        imc_to_char( "You have imctells turned off.\n\r", ch );
        return;
    }

    if( IMCISINVIS(ch) )
    {
        imc_to_char( "You are invisible.\n\r", ch );
        return;
    }

    if( !check_mudof( ch, buf ) )
        return;

    chdata = imc_getdata( ch );
    /* Tell socials. Suggested by Darien@Sandstorm */
    if( argument[0] == '@' )
    {
        char *p, *p2;
        char buf2[SMST];

        argument++;
        while( isspace( *argument ) ) argument++;
        imcstrlcpy( buf2, argument, SMST );
        p = imc_send_social( ch, argument, 1 );
        if( !p || p[0] == '\0' )
        {
            IMCDISPOSE( chdata );
            return;
        }
        imc_send_tell( chdata, buf, p, 2 );
        p2 = imc_send_social( ch, buf2, 2 );
        if( !p2 || p2[0] == '\0' )
        {
            IMCDISPOSE( chdata );
            return;
        }
        snprintf( buf1, LGST, "~WImctell ~C%s: ~c%s\n\r", buf, p2 );
    }
    else
    {
        imc_send_tell( chdata, buf, color_mtoi(argument), 0 );
        snprintf( buf1, LGST, "~cYou imctell ~C%s ~c'~W%s~c'\n\r", buf, argument );
    }
    imc_to_char( buf1, ch );
    imc_update_tellhistory( ch, buf1 );
    IMCDISPOSE( chdata );
}

IMC_CMD( imcreply )
{
    char buf1[LGST];
    imc_char_data *chdata;

    /* just check for deny */
    if( IMCIS_SET( IMCFLAG(ch), IMC_DENYTELL ) )
    {
        imc_to_char( "You are not authorized to use imcreply.\n\r", ch );
        return;
    }

    if( IMCIS_SET( IMCFLAG(ch), IMC_TELL ) )
    {
        imc_to_char( "You have imctells turned off.\n\r", ch );
        return;
    }

    if( IMCISINVIS(ch) )
    {
        imc_to_char( "You are invisible.\n\r", ch );
        return;
    }

    if( !IMC_RREPLY(ch) )
    {
        imc_to_char( "You haven't received an imctell yet.\n\r", ch );
        return;
    }

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "imcreply what?\n\r", ch );
        return;
    }

    if( !check_mudof( ch, IMC_RREPLY(ch) ) )
        return;

    chdata = imc_getdata( ch );
    /* Tell socials. Suggested by Darien@Sandstorm */
    if( argument[0] == '@' )
    {
        char *p, *p2;
        char buf2[SMST];

        argument++;
        while( isspace( *argument ) ) argument++;
        imcstrlcpy( buf2, argument, SMST );
        p = imc_send_social( ch, argument, 1 );
        if( !p || p[0] == '\0' )
        {
            IMCDISPOSE( chdata );
            return;
        }
        imc_send_tell( chdata, IMC_RREPLY(ch), p, 2 );
        p2 = imc_send_social( ch, buf2, 2 );
        if( !p2 || p2[0] == '\0' )
        {
            IMCDISPOSE( chdata );
            return;
        }
        snprintf( buf1, LGST, "~WImctell ~C%s: ~c%s\n\r", IMC_RREPLY(ch), p2 );
    }
    else
    {
        imc_send_tell( chdata, IMC_RREPLY(ch), color_mtoi(argument), 0 );
        snprintf( buf1, LGST, "~cYou imctell ~C%s ~c'~W%s~c'\n\r", IMC_RREPLY(ch), argument );
    }
    imc_to_char( buf1, ch );
    imc_update_tellhistory( ch, buf1 );
    IMCDISPOSE( chdata );
}

IMC_CMD( imcwho )
{
    imc_char_data *chdata;

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "imcwho which mud? See imclist for a list of connected muds.\n\r", ch );
        return;
    }

    if( !check_mud( ch, argument ) )
        return;

    chdata = imc_getdata( ch );
    imc_send_who( chdata, argument, "who" );
    IMCDISPOSE( chdata );
}

IMC_CMD( imclocate )
{
    imc_char_data *chdata;

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "imclocate who?\n\r", ch );
        return;
    }

    chdata = imc_getdata( ch );
    imc_send_locate( chdata, argument );
    IMCDISPOSE( chdata );
}

IMC_CMD( imcfinger )
{
    char name[LGST], arg[SMST];
    imc_char_data *chdata;

    if( IMCIS_SET( IMCFLAG(ch), IMC_DENYFINGER ) )
    {
        imc_to_char( "You are not authorized to use imcfinger.\n\r", ch );
        return;
    }

    argument = imcone_argument( argument, arg );

    if( !arg || arg[0] == '\0' )
    {
        imc_to_char( "~wUsage: imcfinger person@mud\n\r", ch );
        imc_to_char( "~wUsage: imcfinger <field> <value>\n\r", ch );
        imc_to_char( "~wWhere field is one of:\n\r\n\r", ch );
        imc_to_char( "~wdisplay email homepage icq aim yahoo msn privacy comment\n\r", ch );
        return;
    }

    if( !strcasecmp( arg, "display" ) )
    {
        imc_to_char( "~GYour current information:\n\r\n\r", ch );
        imc_printf( ch, "~GEmail   : ~g%s\n\r",
            ( IMC_EMAIL(ch) && IMC_EMAIL(ch)[0] != '\0' ) ? IMC_EMAIL(ch) : "None" );
        imc_printf( ch, "~GHomepage: ~g%s\n\r",
            ( IMC_HOMEPAGE(ch) && IMC_HOMEPAGE(ch)[0] != '\0' ) ? IMC_HOMEPAGE(ch) : "None" );
        imc_printf( ch, "~GICQ     : ~g%d\n\r", IMC_ICQ(ch) );
        imc_printf( ch, "~GAIM     : ~g%s\n\r",
            ( IMC_AIM(ch) && IMC_AIM(ch)[0] != '\0' ) ? IMC_AIM(ch) : "None" );
        imc_printf( ch, "~GYahoo   : ~g%s\n\r",
            ( IMC_YAHOO(ch) && IMC_YAHOO(ch)[0] != '\0' ) ? IMC_YAHOO(ch) : "None" );
        imc_printf( ch, "~GMSN     : ~g%s\n\r",
            ( IMC_MSN(ch) && IMC_MSN(ch)[0] != '\0' ) ? IMC_MSN(ch) : "None" );
        imc_printf( ch, "~GComment : ~g%s\n\r",
            ( IMC_COMMENT(ch) && IMC_COMMENT(ch)[0] != '\0' ) ? IMC_COMMENT(ch) : "None" );
        imc_printf( ch, "~GPrivacy : ~g%s\n\r",
            IMCIS_SET( IMCFLAG(ch), IMC_PRIVACY ) ? "Enabled" : "Disabled" );
        return;
    }

    if( !strcasecmp( arg, "privacy" ) )
    {
        if( IMCIS_SET( IMCFLAG(ch), IMC_PRIVACY ) )
        {
            IMCREMOVE_BIT( IMCFLAG(ch), IMC_PRIVACY );
            imc_to_char( "Privacy flag removed. Your information will now be visible on imcfinger.\n\r", ch );
        }
        else
        {
            IMCSET_BIT( IMCFLAG(ch), IMC_PRIVACY );
            imc_to_char( "Privacy flag enabled. Your information will no longer be visible on imcfinger.\n\r", ch );
        }
        return;
    }

    if( !argument || argument[0] == '\0' )
    {
        if( imc_active != IA_UP )
        {
            imc_to_char( "The mud is not currently connected to IMC2.\n\r", ch );
            return;
        }

        if( !check_mudof( ch, arg ) )
            return;

        chdata = imc_getdata( ch );
        snprintf( name, LGST, "finger %s", imc_nameof( arg ) );
        imc_send_who( chdata, imc_mudof( arg ), name );
        IMCDISPOSE( chdata );
        return;
    }

    if( !strcasecmp( arg, "email" ) )
    {
        IMCSTRFREE( IMC_EMAIL(ch) );
        IMC_EMAIL(ch) = IMCSTRALLOC( argument );
        imc_printf( ch, "Your email address has changed to: %s\n\r", IMC_EMAIL(ch) );
        return;
    }

    if( !strcasecmp( arg, "homepage" ) )
    {
        IMCSTRFREE( IMC_HOMEPAGE(ch) );
        IMC_HOMEPAGE(ch) = IMCSTRALLOC( argument );
        imc_printf( ch, "Your homepage has changed to: %s\n\r", IMC_HOMEPAGE(ch) );
        return;
    }

    if( !strcasecmp( arg, "icq" ) )
    {
        IMC_ICQ(ch) = atoi( argument );
        imc_printf( ch, "Your ICQ Number has changed to: %d\n\r", IMC_ICQ(ch) );
        return;
    }

    if( !strcasecmp( arg, "aim" ) )
    {
        IMCSTRFREE( IMC_AIM(ch) );
        IMC_AIM(ch) = IMCSTRALLOC( argument );
        imc_printf( ch, "Your AIM Screenname has changed to: %s\n\r", IMC_AIM(ch) );
        return;
    }

    if( !strcasecmp( arg, "yahoo" ) )
    {
        IMCSTRFREE( IMC_YAHOO(ch) );
        IMC_YAHOO(ch) = IMCSTRALLOC( argument );
        imc_printf( ch, "Your Yahoo Screenname has changed to: %s\n\r", IMC_YAHOO(ch) );
        return;
    }

    if( !strcasecmp( arg, "msn" ) )
    {
        IMCSTRFREE( IMC_MSN(ch) );
        IMC_MSN(ch) = IMCSTRALLOC( argument );
        imc_printf( ch, "Your MSN Screenname has changed to: %s\n\r", IMC_MSN(ch) );
        return;
    }

    if( !strcasecmp( arg, "comment" ) )
    {
        if( strlen( argument ) > 78 )
        {
            imc_to_char( "You must limit the comment line to 78 characters or less.\n\r", ch );
            return;
        }
        IMCSTRFREE( IMC_COMMENT(ch) );
        IMC_COMMENT(ch) = IMCSTRALLOC( argument );
        imc_printf( ch, "Your comment line has changed to: %s\n\r", IMC_COMMENT(ch) );
        return;
    }
    imcfinger( ch, "" );
    return;
}

/* Removed imcquery and put in imcinfo. -- Xorith */
IMC_CMD( imcinfo )
{
    imc_char_data *chdata;

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "Syntax: imcinfo <mud>\n\r", ch );
        return;
    }

    if( !check_mud( ch, argument ) )
        return;

    chdata = imc_getdata( ch );
    imc_send_who( chdata, argument, "info" );
    IMCDISPOSE( chdata );
}

IMC_CMD( imcbeep )
{
    imc_char_data *chdata;

    if( IMCIS_SET( IMCFLAG(ch), IMC_DENYBEEP ) )
    {
        imc_to_char( "You are not authorized to use imcbeep.\n\r", ch );
        return;
    }

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "Usage: imcbeep user@mud\n\r", ch );
        imc_to_char( "Usage: imcbeep [on]/[off]\n\r", ch );
        return;
    }

    if( !strcasecmp( argument, "on" ) )
    {
        IMCREMOVE_BIT( IMCFLAG(ch), IMC_BEEP );
        imc_to_char( "You now send and receive imcbeeps.\n\r", ch );
        return;
    }

    if( !strcasecmp( argument, "off" ) )
    {
        IMCSET_BIT( IMCFLAG(ch), IMC_BEEP );
        imc_to_char( "You no longer send and receive imcbeeps.\n\r", ch );
        return;
    }

    if( IMCIS_SET( IMCFLAG(ch), IMC_BEEP ) )
    {
        imc_to_char( "You have imcbeep turned off.\n\r", ch );
        return;
    }

    if( IMCISINVIS(ch) )
    {
        imc_to_char( "You are invisible.\n\r", ch );
        return;
    }

    if( !check_mudof( ch, argument ) )
        return;

    chdata = imc_getdata( ch );
    imc_send_beep( chdata, argument );
    imc_printf( ch, "~cYou imcbeep ~Y%s~c.\n\r", argument );
    IMCDISPOSE( chdata );
}

IMC_CMD( imclist )
{
    REMOTEINFO *p;
    char routerpath[LGST], netname[SMST];
    char *start, *onpath;
    int count = 1, end;

    /* Silly little thing, but since imcchanlist <channel> works... why not? -- Xorith */
    if( argument && argument[0] != '\0' )
    {
        imcinfo( ch, argument );
        return;
    }

    imcpager_printf( ch, "~WActive muds on %s:~!\n\r", this_imcmud->network );
    imcpager_printf( ch, "~c%-15.15s ~B%-40.40s~! ~g%-15.15s ~G%s", "Name", "IMC2 Version", "Network", "Router" );

    /* Put local mud on the list, why was this not done? It's a mud isn't it? */
    imcpager_printf( ch, "\n\r\n\r~c%-15.15s ~B%-40.40s ~g%-15.15s ~G%s",
        this_imcmud->localname, IMC_VERSIONID, this_imcmud->network, this_imcmud->routername );

    for( p = first_rinfo; p; p = p->next, count++ )
    {
        if( !strcasecmp( p->network, "unknown" ) )
            imcstrlcpy( netname, this_imcmud->network, SMST );
        else
            imcstrlcpy( netname, p->network, SMST );
        /* If there is more then one path use the second path */
        if( p->path && p->path[0] != '\0' )
        {
            if( ( start = strchr( p->path, '!' ) ) != NULL )
            {
                start++;
                onpath = start;
                end = 0;
                for( onpath = start; *onpath != '!' && *onpath != '\0'; onpath++ )
                {
                    routerpath[end] = *onpath;
                    end++;
                }
                routerpath[end] = '\0';
            }
            else
                imcstrlcpy( routerpath, p->path, LGST );
        }
        imcpager_printf( ch, "\n\r~%c%-15.15s ~B%-40.40s ~g%-15.15s ~G%s",
            p->expired ? 'R' : 'c', p->name, p->version, netname, routerpath );
    }
    imcpager_printf( ch, "\n\r~WRed mud names indicate connections that are down." );
    imcpager_printf( ch, "\n\r~W%d muds on %s found.\n\r", count, this_imcmud->network );
}

IMC_CMD( imcconnect )
{
    if( imc_active != IA_NONE )
    {
        imc_to_char( "IMC appears to already be up!\n\r", ch );
        return;
    }
    imcconnect_attempts = 0;
    imcwait = 0;
    imc_startup( TRUE );
    return;
}

IMC_CMD( imcdisconnect )
{
    if( imc_active < IA_UP )
    {
        imc_to_char( "IMC does not appear to be up!\n\r", ch );
        return;
    }
    imc_shutdown( FALSE );
    return;
}

IMC_CMD( imcconfig )
{
    char arg1[IMC_DATA_LENGTH];

    argument = imc_getarg( argument, arg1, IMC_DATA_LENGTH );

    if( !arg1 || arg1[0] == '\0' )
    {
        imc_to_char( "~wSyntax: &Gimc <field> [value]\n\r\n\r", ch );
        imc_to_char( "~wConfiguration info for your mud. Changes save when edited.\n\r", ch );
        imc_to_char( "~wYou may set the following:\n\r\n\r", ch );
        imc_to_char( "~wShow           : ~GDisplays your current configuration.\n\r", ch );
        imc_to_char( "~wLocalname      : ~GThe name IMC2 knows your mud by.\n\r", ch );
        imc_to_char( "~wAutoconnect    : ~GToggles automatic connection on reboots.\n\r", ch );
        imc_to_char( "~wMinPlayerLevel : ~GSets the minimum level IMC2 can see your players at.\n\r", ch );
        imc_to_char( "~wMinImmLevel    : ~GSets the level at which immortal commands become available.\n\r", ch );
        imc_to_char( "~wAdminlevel     : ~GSets the level at which administrative commands become available.\n\r", ch );
        imc_to_char( "~wImplevel       : ~GSets the level at which immplementor commands become available.\n\r", ch );
        imc_to_char( "~wInfoname       : ~GName of your mud, as seen from the imcquery info sheet.\n\r", ch );
        imc_to_char( "~wInfohost       : ~GTelnet address of your mud.\n\r", ch );
        imc_to_char( "~wInfoport       : ~GTelnet port of your mud.\n\r", ch );
        imc_to_char( "~wInfoemail      : ~GEmail address of the mud's IMC administrator.\n\r", ch );
        imc_to_char( "~wInfoWWW        : ~GThe Web address of your mud.\n\r", ch );
        imc_to_char( "~wInfoBase       : ~GThe codebase your mud uses.\n\r", ch );
        imc_to_char( "~wInfoDetails    : ~GSHORT Description of your mud.\n\r", ch );
        imc_to_char( "~wRouterAddr     : ~GDNS or IP address of the router you mud connects to.\n\r", ch );
        imc_to_char( "~wRouterPort     : ~GPort of the router your mud connects to.\n\r", ch );
        imc_to_char( "~wClientPwd      : ~GClient password for your mud.\n\r", ch );
        imc_to_char( "~wServerPwd      : ~GServer password for your mud.\n\r", ch );
        return;
    }

    if( !strcasecmp( arg1, "md5" ) )
    {
        this_imcmud->md5 = !this_imcmud->md5;

        if( this_imcmud->md5 )
            imc_to_char( "MD5 support enabled.\n\r", ch );
        else
            imc_to_char( "MD5 support disabled.\n\r", ch );
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "md5pass" ) )
    {
        this_imcmud->md5pass = !this_imcmud->md5pass;

        if( this_imcmud->md5pass )
            imc_to_char( "MD5 Authentication enabled.\n\r", ch );
        else
            imc_to_char( "MD5 Authentication disabled.\n\r", ch );
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "autoconnect" ) )
    {
        this_imcmud->autoconnect = !this_imcmud->autoconnect;

        if( this_imcmud->autoconnect )
            imc_to_char( "Autoconnect enabled.\n\r", ch );
        else
            imc_to_char( "Autoconnect disabled.\n\r", ch );
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "show" ) )
    {
        imc_printf( ch, "~wLocalname      : ~G%s\n\r", this_imcmud->localname );
        imc_printf( ch, "~wAutoconnect    : ~G%s\n\r", this_imcmud->autoconnect ? "Enabled" : "Disabled" );
        imc_printf( ch, "~wMinPlayerLevel : ~G%d\n\r", this_imcmud->minlevel );
        imc_printf( ch, "~wMinImmLevel    : ~G%d\n\r", this_imcmud->immlevel );
        imc_printf( ch, "~wAdminlevel     : ~G%d\n\r", this_imcmud->adminlevel );
        imc_printf( ch, "~wImplevel       : ~G%d\n\r", this_imcmud->implevel );
        imc_printf( ch, "~wInfoname       : ~G%s\n\r", this_imcmud->fullname );
        imc_printf( ch, "~wInfohost       : ~G%s\n\r", this_imcmud->ihost );
        imc_printf( ch, "~wInfoport       : ~G%d\n\r", this_imcmud->iport );
        imc_printf( ch, "~wInfoemail      : ~G%s\n\r", this_imcmud->email );
        imc_printf( ch, "~wInfoWWW        : ~G%s\n\r", this_imcmud->www );
        imc_printf( ch, "~wInfoBase       : ~G%s\n\r", this_imcmud->base );
        imc_printf( ch, "~wInfoDetails    : ~G%s\n\r\n\r", this_imcmud->details );
        imc_printf( ch, "~wRouterAddr     : ~G%s\n\r", this_imcmud->rhost );
        imc_printf( ch, "~wRouterPort     : ~G%d\n\r", this_imcmud->rport );
        imc_printf( ch, "~wClientPwd      : ~G%s\n\r", this_imcmud->clientpw );
        imc_printf( ch, "~wServerPwd      : ~G%s\n\r", this_imcmud->serverpw );
        if( this_imcmud->md5 )
            imc_to_char( "~RThis mud has enabled MD5 authentication.\n\r", ch );
        else
            imc_to_char( "~RThis mud has disabled MD5 authentication.\n\r", ch );
        if( this_imcmud->md5 && this_imcmud->md5pass )
            imc_to_char( "~RThe mud is using MD5 encryption to authenticate.\n\r", ch );
        else
            imc_to_char( "~RThe mud is using plain text passwords to authenticate.\n\r", ch );
        return;
    }

    if( !argument || argument[0] == '\0' )
    {
        imcconfig( ch, "" );
        return;
    }

    if( !strcasecmp( arg1, "minplayerlevel" ) )
    {
        int value = atoi( argument );

        imc_printf( ch, "Minimum level set to %d\n\r", value );
        this_imcmud->minlevel = value;
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "minimmlevel" ) )
    {
        int value = atoi( argument );

        imc_printf( ch, "Immortal level set to %d\n\r", value );
        this_imcmud->immlevel = value;
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "adminlevel" ) )
    {
        int value = atoi( argument );

        imc_printf( ch, "Admin level set to %d\n\r", value );
        this_imcmud->adminlevel = value;
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "implevel" ) && IMCPERM(ch) == IMCPERM_IMP )
    {
        int value = atoi( argument );

        imc_printf( ch, "Implementor level set to %d\n\r", value );
        this_imcmud->implevel = value;
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "infoname" ) )
    {
        IMCSTRFREE( this_imcmud->fullname );
        this_imcmud->fullname = IMCSTRALLOC( argument );
        imc_save_config();
        imc_printf( ch, "Infoname change to %s\n\r", argument );
        return;
    }

    if( !strcasecmp( arg1, "infohost" ) )
    {
        IMCSTRFREE( this_imcmud->ihost );
        this_imcmud->ihost = IMCSTRALLOC( argument );
        imc_save_config();
        imc_printf( ch, "Infohost changed to %s\n\r", argument );
        return;
    }

    if( !strcasecmp( arg1, "infoport" ) )
    {
        this_imcmud->iport = atoi( argument );
        imc_save_config();
        imc_printf( ch, "Infoport changed to %d\n\r", this_imcmud->iport );
        return;
    }

    if( !strcasecmp( arg1, "infoemail" ) )
    {
        IMCSTRFREE( this_imcmud->email );
        this_imcmud->email = IMCSTRALLOC( argument );
        imc_save_config();
        imc_printf( ch, "Infoemail changed to %s\n\r", argument );
        return;
    }

    if( !strcasecmp( arg1, "infowww" ) )
    {
        IMCSTRFREE( this_imcmud->www );
        this_imcmud->www = IMCSTRALLOC( argument );
        imc_save_config();
        imc_printf( ch, "InfoWWW changed to %s\n\r", argument );
        imc_send_keepalive( "*@*" );
        return;
    }

    if( !strcasecmp( arg1, "infobase" ) )
    {
        char cbase[SMST];

        IMCSTRFREE( this_imcmud->base );
        this_imcmud->base = IMCSTRALLOC( argument );
        imc_save_config();
        imc_printf( ch, "Infobase changed to %s\n\r", argument );

        IMCSTRFREE( IMC_VERSIONID );
        snprintf( cbase, SMST, "%s%s", IMC_VERSION_ID, this_imcmud->base );
        IMC_VERSIONID = IMCSTRALLOC( cbase );
        imc_send_keepalive( "*@*" );
        return;
    }

    if( !strcasecmp( arg1, "infodetails" ) )
    {
        IMCSTRFREE( this_imcmud->details );
        this_imcmud->details = IMCSTRALLOC( argument );
        imc_save_config();
        imc_to_char( "Infodetails updated.\n\r", ch );
        return;
    }

    if( imc_active != IA_NONE )
    {
        imc_printf( ch, "Cannot alter %s while the mud is connected to IMC.\n\r", arg1 );
        return;
    }

    if( !strcasecmp( arg1, "routeraddr" ) )
    {
        IMCSTRFREE( this_imcmud->rhost );
        this_imcmud->rhost = IMCSTRALLOC( argument );
        imc_printf( ch, "Routeraddr changed to %s\n\r", argument );
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "routerport" ) )
    {
        this_imcmud->rport = atoi( argument );
        imc_printf( ch, "Routerport changed to %d\n\r", this_imcmud->rport );
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "clientpwd" ) )
    {
        IMCSTRFREE( this_imcmud->clientpw );
        this_imcmud->clientpw = IMCSTRALLOC( argument );
        imc_printf( ch, "Clientpwd changed to %s\n\r", argument );
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "serverpwd" ) )
    {
        IMCSTRFREE( this_imcmud->serverpw );
        this_imcmud->serverpw = IMCSTRALLOC( argument );
        imc_printf( ch, "Serverpwd changed to %s\n\r", argument );
        imc_save_config();
        return;
    }

    if( !strcasecmp( arg1, "localname" ) )
    {
        IMCSTRFREE( this_imcmud->localname );
        this_imcmud->localname = IMCSTRALLOC( argument );
        imc_save_config();
        imc_printf( ch, "Localname changed to %s\n\r", argument );
        return;
    }

    imcconfig( ch, "" );
    return;
}

/* Modified this command so it's a little more helpful -- Xorith */
IMC_CMD( imcignore )
{
    int count;
    IMC_IGNORE *ign;
    char arg[SMST];

    argument = imcone_argument( argument, arg );

    if( !arg || arg[0] == '\0' )
    {
        imc_to_char( "You currently ignore the following:\n\r", ch );
        for( count = 0, ign = FIRST_IMCIGNORE(ch); ign; ign = ign->next, count++ )
            imc_printf( ch, "%s\n\r", ign->name );

        if( !count )
            imc_to_char( " none\n\r", ch );
        else
            imc_printf( ch, "\n\r[total %d]\n\r", count );
        imc_to_char( "For help on imcignore, type: IMCIGNORE HELP\n\r", ch );
        return;
    }

    if( !strcasecmp( arg, "help" ) )
    {
        imc_to_char( "~wTo see your current ignores  : ~GIMCIGNORE\n\r", ch );
        imc_to_char( "~wTo add an ignore             : ~GIMCIGNORE ADD <argument>\n\r", ch );
        imc_to_char( "~wTo delete an ignore          : ~GIMCIGNORE DELETE <argument>\n\r", ch );
        imc_to_char( "~WSee your MUD's help for more information.\n\r", ch );
        return;
    }

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "Must specify both action and name.\n\r", ch );
        imc_to_char( "Please see IMCIGNORE HELP for details.\n\r", ch );
        return;
    }

    if( !strcasecmp( arg, "delete" ) )
    {
        for( ign = FIRST_IMCIGNORE(ch); ign; ign = ign->next )
        {
            if( !strcasecmp( ign->name, argument ) )
            {
                IMCUNLINK( ign, FIRST_IMCIGNORE(ch), LAST_IMCIGNORE(ch), next, prev );
                IMCSTRFREE( ign->name );
                IMCDISPOSE( ign );
                imc_to_char( "Entry deleted.\n\r", ch );
                return;
            }
        }
        imc_to_char( "Entry not found.\n\rPlease check your ignores by typing IMCIGNORE with no arguments.\n\r", ch );
        return;
    }

    if( !strcasecmp( arg, "add" ) )
    {
        IMCCREATE( ign, IMC_IGNORE, 1 );
        ign->name = IMCSTRALLOC( argument );
        IMCLINK( ign, FIRST_IMCIGNORE(ch), LAST_IMCIGNORE(ch), next, prev );
        imc_printf( ch, "%s will now be ignored.\n\r", argument );
        return;
    }
    imcignore( ch, "help" );
    return;
}

/* Made this command a little more helpful --Xorith */
IMC_CMD( imcban )
{
    int count;
    IMC_BAN *ban;
    char arg[SMST];

    argument = imcone_argument( argument, arg );

    if( !arg || arg[0] == '\0' )
    {
        imc_to_char( "The mud currently bans the following:\n\r", ch );
        for( count = 0, ban = first_imc_ban; ban; ban = ban->next, count++ )
            imc_printf( ch, "%s\n\r", ban->name );

        if( !count )
            imc_to_char( " none\n\r", ch );
        else
            imc_printf( ch, "\n\r[total %d]\n\r", count );
        imc_to_char( "Type: IMCBAN HELP for more information.\n\r", ch );
        return;
    }

    if( !strcasecmp( arg, "help" ) )
    {
        imc_to_char( "~wTo see the current bans             : ~GIMCBAN\n\r", ch );
        imc_to_char( "~wTo add a MUD to the ban list        : ~GIMCBAN ADD <argument>\n\r", ch );
        imc_to_char( "~wTo delete a MUD from the ban list   : ~GIMCBAN DELETE <argument>\n\r", ch );
        imc_to_char( "~WSee your MUD's help for more information.\n\r", ch );
        return;
    }

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "Must specify both action and name.\n\rPlease type IMCBAN HELP for more information\n\r", ch );
        return;
    }

    if( !strcasecmp( arg, "delete" ) )
    {
        if( imc_delban( argument ) )
        {
            imc_savebans( );
            imc_to_char( "Entry deleted.\n\r", ch );
            return;
        }
        imc_to_char( "Entry not found.\n\rPlease type IMCBAN without arguments to see the current ban list.\n\r", ch );
    }

    if( !strcasecmp( arg, "add" ) )
    {
        imc_addban( argument );
        imc_savebans( );
        imc_printf( ch, "Mud %s will now be banned.\n\r", argument );
        return;
    }
    imcban( ch, "" );
    return;
}

IMC_CMD( imcstats )
{
    if( argument && argument[0] != '\0' )
    {
        imc_char_data *chdata;

        if( !check_mud( ch, argument ) )
            return;

        chdata = imc_getdata( ch );
        imc_send_who( chdata, argument, "istats" );
        IMCDISPOSE( chdata );
        return;
    }

    imc_printf( ch, "IMC statistics\n\r\n\r"
        "~cConnected on descriptor: ~C%d\n\r"
        "~cReceived packets   : ~C%ld\n\r"
        "~cReceived bytes     : ~C%ld\n\r", this_imcmud->desc, imc_stats.rx_pkts, imc_stats.rx_bytes );

    imc_printf( ch, "~cTransmitted packets: ~C%ld\n\r"
        "~cTransmitted bytes  : ~C%ld\n\r", imc_stats.tx_pkts, imc_stats.tx_bytes );

    imc_printf( ch, "~cMaximum packet size: ~C%d\n\r"
        "~cSequence drops     : ~C%d\n\r", imc_stats.max_pkt, imc_stats.sequence_drops );

    imc_printf( ch, "~cLast IMC Boot: ~C%s~!\n\r", ctime( &imc_boot ) );
}

IMC_CMD( imc_deny_channel )
{
    char vic_name[SMST];
    CHAR_DATA *victim;
    IMC_CHANNEL *channel;

    argument = imcone_argument( argument, vic_name );

    if( !vic_name || vic_name[0] == '\0' || !argument || argument[0] == '\0' )
    {
        imc_to_char( "Usage: imcdeny <person> <local channel name>\n\r", ch );
        imc_to_char( "Usage: imcdeny <person> [tell/beep/finger]\n\r", ch );
        return;
    }

    if( !( victim = imc_find_user( vic_name ) ) )
    {
        imc_to_char( "No such person is currently online.\n\r", ch );
        return;
    }

    if( IMCPERM(ch) <= IMCPERM(victim) )
    {
        imc_to_char( "You cannot alter their settings.\n\r", ch );
        return;
    }

    if( !strcasecmp( argument, "tell" ) )
    {
        if( !IMCIS_SET( IMCFLAG(victim), IMC_DENYTELL ) )
        {
            IMCSET_BIT( IMCFLAG(victim), IMC_DENYTELL );
            imc_printf( ch, "%s can no longer use imctells.\n\r", CH_IMCNAME(victim) );
            return;
        }
        IMCREMOVE_BIT( IMCFLAG(victim), IMC_DENYTELL );
        imc_printf( ch, "%s can use imctells again.\n\r", CH_IMCNAME(victim) );
        return;
    }

    if( !strcasecmp( argument, "beep" ) )
    {
        if( !IMCIS_SET( IMCFLAG(victim), IMC_DENYBEEP ) )
        {
            IMCSET_BIT( IMCFLAG(victim), IMC_DENYBEEP );
            imc_printf( ch, "%s can no longer use imcbeeps.\n\r", CH_IMCNAME(victim) );
            return;
        }
        IMCREMOVE_BIT( IMCFLAG(victim), IMC_DENYBEEP );
        imc_printf( ch, "%s can use imcbeeps again.\n\r", CH_IMCNAME(victim) );
        return;
    }

    if( !strcasecmp( argument, "finger" ) )
    {
        if( !IMCIS_SET( IMCFLAG(victim), IMC_DENYFINGER ) )
        {
            IMCSET_BIT( IMCFLAG(victim), IMC_DENYFINGER );
            imc_printf( ch, "%s can no longer use imcfingers.\n\r", CH_IMCNAME(victim) );
            return;
        }
        IMCREMOVE_BIT( IMCFLAG(victim), IMC_DENYFINGER );
        imc_printf( ch, "%s can use imcfingers again.\n\r", CH_IMCNAME(victim) );
        return;
    }

    /* Assumed to be denying a channel by this stage. */
    if( ( channel = imc_findlchannel( argument ) ) == NULL )
    {
        imc_to_char( "Unknown or unconfigured local channel. Check your channel name.\n\r", ch );
        return;
    }

    if( imc_hasname( IMC_DENY(ch), channel->local_name ) )
    {
        imc_printf( ch, "%s can now listen to %s\n\r", CH_IMCNAME(victim), channel->local_name );
        imc_removename( &IMC_DENY(ch), channel->local_name );
    }
    else
    {
        imc_printf( ch, "%s can no longer listen to %s\n\r", CH_IMCNAME(victim), channel->local_name );
        imc_addname( &IMC_DENY(ch), channel->local_name );
    }
    return;
}

/*  Traceroute and ping.
 *
 *  Be lazy - only remember the last traceroute
 */
IMC_CMD( imcping )
{
    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "Ping which mud?\n\r", ch );
        return;
    }

    if( !check_mud( ch, argument ) )
        return;

    imcstrlcpy( lastping, argument, IMC_MNAME_LENGTH );
    imcstrlcpy( pinger, CH_IMCNAME(ch), 100 );
    imc_send_ping( argument );
}

IMC_CMD( imcpermstats )
{
    CHAR_DATA *victim;

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "Usage: imcperms <user>\n\r", ch );
        return;
    }

    if( !( victim = imc_find_user( argument ) ) )
    {
        imc_to_char( "No such person is currently online.\n\r", ch );
        return;
    }

    if( IMCPERM(victim) < 0 || IMCPERM(victim) > IMCPERM_IMP )
    {
        imc_printf( ch, "%s has an invalid permission setting!\n\r", CH_IMCNAME(victim) );
        return;
    }

    imc_printf( ch, "~GPermissions for %s: %s\n\r", CH_IMCNAME(victim), imcperm_names[IMCPERM(victim)] );
    imc_printf( ch, "~gThese permissions were obtained %s.\n\r",
        IMCIS_SET( IMCFLAG(victim), IMC_PERMOVERRIDE ) ? "manually via imcpermset" : "automatically by level" );
    return;
}

IMC_CMD( imcpermset )
{
    CHAR_DATA *victim;
    char arg[SMST];
    int permvalue;

    argument = imcone_argument( argument, arg );

    if( !arg || arg[0] == '\0' )
    {
        imc_to_char( "Usage: imcpermset <user> <permission>\n\r", ch );
        imc_to_char( "Permission can be one of: None, Mort, Imm, Admin, Imp\n\r", ch );
        return;
    }

    if( !( victim = imc_find_user( arg ) ) )
    {
        imc_to_char( "No such person is currently online.\n\r", ch );
        return;
    }

    if( !strcasecmp( argument, "override" ) )
        permvalue = -1;
    else
    {
        permvalue = get_imcpermvalue( argument );

        if( !imccheck_permissions( ch, permvalue, IMCPERM(victim), TRUE ) )
            return;
    }

    /* Just something to avoid looping through the channel clean-up --Xorith */
    if( IMCPERM(victim) == permvalue )
    {
        imc_printf( ch, "%s already has a permission level of %s.\n\r", CH_IMCNAME(victim), imcperm_names[permvalue] );
        return;
    }

    if( permvalue == -1 )
    {
        IMCREMOVE_BIT( IMCFLAG(victim), IMC_PERMOVERRIDE );
        imc_printf( ch, "~YPermission flag override has been removed from %s\n\r", CH_IMCNAME(victim) );
        return;
    }

    IMCPERM(victim) = permvalue;
    IMCSET_BIT( IMCFLAG(victim), IMC_PERMOVERRIDE );

    imc_printf( ch, "~YPermission level for %s has been changed to %s\n\r", CH_IMCNAME(victim), imcperm_names[permvalue] );
    /* Channel Clean-Up added by Xorith 9-24-03 */
    /* Note: Let's not clean up IMC_DENY for a player. Never know... */
    if( IMC_LISTEN( victim ) != NULL && imc_active == IA_UP )
    {
        IMC_CHANNEL *channel = NULL;
        char *channels = IMC_LISTEN(victim);

        while(1)
        {
            if( channels[0] == '\0' )
                break;
            channels = imcone_argument( channels, arg );

            if( !( channel = imc_findlchannel( arg ) ) )
                imc_removename( &IMC_LISTEN( victim ), arg );
            if( channel && IMCPERM(victim) < channel->level )
            {
                imc_removename( &IMC_LISTEN(victim), arg );
                imc_printf( ch, "~WRemoving '%s' level channel: '%s', exceeding new permission of '%s'\n\r",
                    imcperm_names[channel->level], channel->local_name, imcperm_names[IMCPERM(victim)] );
            }
        }
    }
    return;
}

IMC_CMD( imcinvis )
{
    if( IMCIS_SET( IMCFLAG(ch), IMC_INVIS ) )
    {
        IMCREMOVE_BIT( IMCFLAG(ch), IMC_INVIS );
        imc_to_char( "You are now imcvisible.\n\r", ch );
    }
    else
    {
        IMCSET_BIT( IMCFLAG(ch), IMC_INVIS );
        imc_to_char( "You are now imcinvisible.\n\r", ch );
    }
    return;
}

IMC_CMD( imcchanwho )
{
    IMC_CHANNEL *c;
    PACKET out;
    char chan[SMST], mud[SMST];
    imc_char_data *chdata;

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "Usage: imcchanwho <channel> [<mud> <mud> <mud> <...>|<all>]\n\r", ch );
        return;
    }

    argument = imcone_argument( argument, chan );

    c = imc_findlchannel( chan );
    if( !c )
        c = imc_findchannel( chan );

    if( !c )
    {
        imc_to_char( "No such channel.\n\r", ch );
        return;
    }

    if( IMCPERM(ch) < c->level )
    {
        imc_to_char( "No such channel.\n\r", ch );
        return;
    }

    if( !c->refreshed )
    {
        imc_printf( ch, "%s has not been refreshed yet.\n\r", c->name );
        return;
    }

    chdata = imc_getdata( ch );

    if( strcasecmp( argument, "all" ) )
    {
        while( argument[0] != '\0' )
        {
            argument = imcone_argument( argument, mud );

            if( !check_mud( ch, mud ) )
                continue;

            imc_initdata( &out );
            setdata( &out, chdata );

            imcstrlcpy( out.type, "ice-chan-who", IMC_TYPE_LENGTH );
            imcstrlcpy( out.to, mud, IMC_NAME_LENGTH );
            imc_addkey( &out, "channel", c->name );
            imc_addkey( &out, "lname", c->local_name ? c->local_name : c->name );

            imc_send( &out );
            imc_freedata( &out );
        }
        IMCDISPOSE( chdata );
        return;
    }
    else
        imcstrlcpy( mud, "*", SMST );

    imc_initdata( &out );
    setdata( &out, chdata );

    imcstrlcpy( out.type, "ice-chan-who", IMC_TYPE_LENGTH );
    imcstrlcpy( out.to, mud, IMC_NAME_LENGTH );
    imc_addkey( &out, "channel", c->name );
    imc_addkey( &out, "lname", c->local_name ? c->local_name : c->name );

    imc_send( &out );
    imc_freedata( &out );
    IMCDISPOSE( chdata );
}

IMC_CMD( imcremoteadmin )
{
    REMOTEINFO *r;
    char router[IMC_NAME_LENGTH];
    char pwd[IMC_DATA_LENGTH];
    char cmd[IMC_NAME_LENGTH];
    char data[IMC_DATA_LENGTH];
    PACKET out;

    argument = imc_getarg( argument, router, IMC_NAME_LENGTH );
    argument = imc_getarg( argument, pwd, IMC_DATA_LENGTH );
    argument = imc_getarg( argument, cmd, IMC_NAME_LENGTH );
    imcstrlcpy( data, argument, IMC_DATA_LENGTH );

    if( !router[0] || !cmd[0] )
    {
        imc_to_char( "Syntax: imcadmin <router> <password> <command> [<data..>]\n\r", ch );
        imc_to_char( "You must be an approved router administrator to use remote commands.\n\r", ch );
        return;
    }

    if( !( r = imc_find_reminfo( router ) ) )
    {
        imc_printf( ch, "~W%s ~cis not a valid mud name.\n\r", router );
        return;
    }

    if( r->expired )
    {
        imc_printf( ch, "~W%s ~cis not connected right now.\n\r", r->name );
        return;
    }

    imc_initdata( &out );
    snprintf( out.to, IMC_NAME_LENGTH, "IMC@%s", r->name );
    imcstrlcpy( out.type, "remote-admin", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, CH_IMCNAME(ch), IMC_NAME_LENGTH );
    imc_addkey( &out, "command", cmd );
    imc_addkey( &out, "data", data );
    if( this_imcmud->md5pass == TRUE )
        imc_addkey( &out, "hash", pwd );

    imc_send( &out );
    imc_freedata( &out );

    imc_to_char( "Remote command sent.\n\r", ch );
    return;
}

IMC_CMD( imchelp )
{
    char buf[LGST];
    IMC_HELP_DATA *help;
    int col, perm;

    if( !argument || argument[0] == '\0' )
    {
        imcstrlcpy( buf, "~gHelp is available for the following commands:\n\r", LGST );
        imcstrlcat( buf, "~G---------------------------------------------\n\r", LGST );
        for( perm = IMCPERM_MORT; perm <= IMCPERM(ch); perm++ )
        {
            col = 0;
            snprintf( buf+strlen(buf), LGST-strlen(buf), "\n\r~g%s helps:~G\n\r", imcperm_names[perm] );
            for( help = first_imc_help; help; help = help->next )
            {
                if( help->level != perm )
                    continue;

                snprintf( buf+strlen(buf), LGST-strlen(buf), "%-15s", help->name );
                if( ++col % 6 == 0 )
                    imcstrlcat( buf, "\n\r", LGST );
            }
            if( col % 6 != 0 )
                imcstrlcat( buf, "\n\r", LGST );
        }
        imc_to_pager( buf, ch );
        return;
    }

    for( help = first_imc_help; help; help = help->next )
    {
        if( !strcasecmp( help->name, argument ) )
        {
            if( !help->text || help->text[0] == '\0' )
                imc_printf( ch, "~gNo inforation available for topic ~W%s~g.\n\r", help->name );
            else
                imc_printf( ch, "~g%s\n\r", help->text );
            return;
        }
    }
    imc_printf( ch, "~gNo help exists for topic ~W%s~g.\n\r", argument );
    return;
}

IMC_CMD( imccolor )
{
    if( IMCIS_SET( IMCFLAG(ch), IMC_COLORFLAG ) )
    {
        IMCREMOVE_BIT( IMCFLAG(ch), IMC_COLORFLAG );
        imc_to_char( "IMC2 color is now off.\n\r", ch );
    }
    else
    {
        IMCSET_BIT( IMCFLAG(ch), IMC_COLORFLAG );
        imc_to_char( "~RIMC2 c~Yo~Gl~Bo~Pr ~Ris now on. Enjoy :)\n\r", ch );
    }
    return;
}

IMC_CMD( imcafk )
{
    if( IMCIS_SET( IMCFLAG(ch), IMC_AFK ) )
    {
        IMCREMOVE_BIT( IMCFLAG(ch), IMC_AFK );
        imc_to_char( "You are no longer AFK to IMC2.\n\r", ch );
    }
    else
    {
        IMCSET_BIT( IMCFLAG(ch), IMC_AFK );
        imc_to_char( "You are now AFK to IMC2.\n\r", ch );
    }
    return;
}

IMC_CMD( imcdebug )
{
    imcpacketdebug = !imcpacketdebug;

    if( imcpacketdebug )
        imc_to_char( "Packet debug enabled.\n\r", ch );
    else
        imc_to_char( "Packet debug disabled.\n\r", ch );
    return;
}

/* This is very possibly going to be spammy as hell */
IMC_CMD( imc_show_ucache_contents )
{
    IMCUCACHE_DATA *user;
    int users = 0;

    imc_to_pager( "Cached user information\n\r", ch );
    imc_to_pager( "User                          | Gender ( 0 = Male, 1 = Female, 2 = Other )\n\r", ch );
    imc_to_pager( "--------------------------------------------------------------------------\n\r", ch );
    for( user = first_imcucache; user; user = user->next )
    {
        imcpager_printf( ch, "%-30s %d\n\r", user->name, user->gender );
        users++;
    }
    imcpager_printf( ch, "%d users being cached.\n\r", users );
    return;
}

IMC_CMD( imccedit )
{
    IMC_CMD_DATA *cmd, *tmp;
    IMC_ALIAS *alias, *alias_next;
    char name[SMST], option[SMST];
    bool found = FALSE, aliasfound = FALSE;

    argument = imcone_argument( argument, name );
    argument = imcone_argument( argument, option );

    if( !name || name[0] == '\0' || !option || option[0] == '\0' )
    {
        imc_to_char( "Usage: imccedit <command> <create|delete|alias|rename|code|permission|connected> <field>.\n\r", ch );
        return;
    }

    for( cmd = first_imc_command; cmd; cmd = cmd->next )
    {
        if( !strcasecmp( cmd->name, name ) )
        {
            found = TRUE;
            break;
        }
        for( alias = cmd->first_alias; alias; alias = alias->next )
        {
            if( !strcasecmp( alias->name, name ) )
                aliasfound = TRUE;
        }
    }

    if( !strcasecmp( option, "create" ) )
    {
        if( found )
        {
            imc_printf( ch, "~gA command named ~W%s ~galready exists.\n\r", name );
            return;
        }

        if( aliasfound )
        {
            imc_printf( ch, "~g%s already exists as an alias for another command.\n\r", name );
            return;
        }

        IMCCREATE( cmd, IMC_CMD_DATA, 1 );
        cmd->name = IMCSTRALLOC( name );
        cmd->level = IMCPERM(ch);
        cmd->connected = FALSE;
        imc_printf( ch, "~gCommand ~W%s ~gcreated.\n\r", cmd->name );
        if( argument && argument[0] != '\0' )
        {
            cmd->function = imc_function( argument );
            if( cmd->function == NULL )
                imc_printf( ch, "~gFunction ~W%s ~gdoes not exist - set to NULL.\n\r", argument );
        }
        else
        {
            imc_to_char( "~gFunction set to NULL.\n\r", ch );
            cmd->function = NULL;
        }
        IMCLINK( cmd, first_imc_command, last_imc_command, next, prev );
        imc_savecommands();
        return;
    }

    if( !found )
    {
        imc_printf( ch, "~gNo command named ~W%s ~gexists.\n\r", name );
        return;
    }

    if( !imccheck_permissions( ch, cmd->level, cmd->level, FALSE ) )
        return;

    if( !strcasecmp( option, "delete" ) )
    {
        imc_printf( ch, "~gCommand ~W%s ~ghas been deleted.\n\r", cmd->name );
        for( alias = cmd->first_alias; alias; alias = alias_next )
        {
            alias_next = alias->next;

            IMCUNLINK( alias, cmd->first_alias, cmd->last_alias, next, prev );
            IMCSTRFREE( alias->name );
            IMCDISPOSE( alias );
        }
        IMCUNLINK( cmd, first_imc_command, last_imc_command, next, prev );
        IMCSTRFREE( cmd->name );
        IMCDISPOSE( cmd );
        imc_savecommands();
        return;
    }

    /* MY GOD! What an inefficient mess you've made Samson! */
    if( !strcasecmp( option, "alias" ) )
    {
        for( alias = cmd->first_alias; alias; alias = alias_next )
        {
            alias_next = alias->next;

            if( !strcasecmp( alias->name, argument ) )
            {
                imc_printf( ch, "~W%s ~ghas been removed as an alias for ~W%s\n\r", argument, cmd->name );
                IMCUNLINK( alias, cmd->first_alias, cmd->last_alias, next, prev );
                IMCSTRFREE( alias->name );
                IMCDISPOSE( alias );
                imc_savecommands();
                return;
            }
        }

        for( tmp = first_imc_command; tmp; tmp = tmp->next )
        {
            if( !strcasecmp( tmp->name, argument ) )
            {
                imc_printf( ch, "~W%s &gis already a command name.\n\r", argument );
                return;
            }
            for( alias = tmp->first_alias; alias; alias = alias->next )
            {
                if( !strcasecmp( argument, alias->name ) )
                {
                    imc_printf( ch, "~W%s ~gis already an alias for ~W%s\n\r", argument, tmp->name );
                    return;
                }
            }
        }

        IMCCREATE( alias, IMC_ALIAS, 1 );
        alias->name = IMCSTRALLOC( argument );
        IMCLINK( alias, cmd->first_alias, cmd->last_alias, next, prev );
        imc_printf( ch, "~W%s ~ghas been added as an alias for ~W%s\n\r", alias->name, cmd->name );
        imc_savecommands();
        return;
    }

    if( !strcasecmp( option, "connected" ) )
    {
        cmd->connected = !cmd->connected;

        if( cmd->connected )
            imc_printf( ch, "~gCommand ~W%s ~gwill now require a connection to IMC2 to use.\n\r", cmd->name );
        else
            imc_printf( ch, "~gCommand ~W%s ~gwill no longer require a connection to IMC2 to use.\n\r", cmd->name );
        imc_savecommands();
        return;
    }

    if( !strcasecmp( option, "show" ) )
    {
        char buf[LGST];

        imc_printf( ch, "~gCommand       : ~W%s\n\r", cmd->name );
        imc_printf( ch, "~gPermission    : ~W%s\n\r", imcperm_names[cmd->level] );
        imc_printf( ch, "~gFunction      : ~W%s\n\r", imc_funcname( cmd->function ) );
        imc_printf( ch, "~gConnection Req: ~W%s\n\r", cmd->connected ? "Yes" : "No" );
        if( cmd->first_alias )
        {
            int col = 0;
            imcstrlcpy( buf, "~gAliases       : ~W", LGST );
            for( alias = cmd->first_alias; alias; alias = alias->next )
            {
                snprintf( buf+strlen(buf), LGST-strlen(buf), "%s ", alias->name );
                if( ++col % 10 == 0 )
                    imcstrlcat( buf, "\n\r", LGST );
            }
            if( col % 10 != 0 )
                imcstrlcat( buf, "\n\r", LGST );
            imc_to_char( buf, ch );
        }
        return;
    }

    if( !argument || argument[0] == '\0' )
    {
        imc_to_char( "Required argument missing.\n\r", ch );
        imccedit( ch, "" );
        return;
    }

    if( !strcasecmp( option, "rename" ) )
    {
        imc_printf( ch, "~gCommand ~W%s ~ghas been renamed to ~W%s.\n\r", cmd->name, argument );
        IMCSTRFREE( cmd->name );
        cmd->name = IMCSTRALLOC( argument );
        imc_savecommands();
        return;
    }

    if( !strcasecmp( option, "code" ) )
    {
        cmd->function = imc_function( argument );
        if( cmd->function == NULL )
            imc_printf( ch, "~gFunction ~W%s ~gdoes not exist - set to NULL.\n\r", argument );
        else
            imc_printf( ch, "~gFunction set to ~W%s.\n\r", argument );
        imc_savecommands();
        return;
    }

    if( !strcasecmp( option, "perm" ) || !strcasecmp( option, "permission" ) )
    {
        int permvalue = get_imcpermvalue( argument );

        if( !imccheck_permissions( ch, permvalue, cmd->level, FALSE ) )
            return;

        cmd->level = permvalue;
        imc_printf( ch, "~gCommand ~W%s ~gpermission level has been changed to ~W%s.\n\r", cmd->name, imcperm_names[permvalue] );
        imc_savecommands();
        return;
    }
    imccedit( ch, "" );
    return;
}

IMC_CMD( imchedit )
{
    IMC_HELP_DATA *help;
    char name[SMST], cmd[SMST];
    bool found = FALSE;

    argument = imcone_argument( argument, name );
    argument = imcone_argument( argument, cmd );

    if( !name || name[0] == '\0' || !cmd || cmd[0] == '\0' || !argument || argument[0] == '\0' )
    {
        imc_to_char( "Usage: imchedit <topic> [name|perm] <field>\n\r", ch );
        imc_to_char( "Where <field> can be either name, or permission level.\n\r", ch );
        return;
    }

    for( help = first_imc_help; help; help = help->next )
    {
        if( !strcasecmp( help->name, name ) )
        {
            found = TRUE;
            break;
        }
    }

    if( !found )
    {
        imc_printf( ch, "~gNo help exists for topic ~W%s~g. You will need to add it to the helpfile manually.\n\r", name );
        return;
    }

    if( !strcasecmp( cmd, "name" ) )
    {
        imc_printf( ch, "~W%s ~ghas been renamed to ~W%s.\n\r", help->name, argument );
        IMCSTRFREE( help->name );
        help->name = IMCSTRALLOC( argument );
        imc_savehelps();
        return;
    }

    if( !strcasecmp( cmd, "perm" ) )
    {
        int permvalue = get_imcpermvalue( argument );

        if( !imccheck_permissions( ch, permvalue, help->level, FALSE ) )
            return;

        imc_printf( ch, "~gPermission level for ~W%s ~ghas been changed to ~W%s.\n\r", help->name, imcperm_names[permvalue] );
        help->level = permvalue;
        imc_savehelps();
        return;
    }
    imchedit( ch, "" );
    return;
}

IMC_CMD( imcnotify )
{
    if( IMCIS_SET( IMCFLAG(ch), IMC_NOTIFY ) )
    {
        IMCREMOVE_BIT( IMCFLAG(ch), IMC_NOTIFY );
        imc_to_char( "You no longer see channel notifications.\n\r", ch );
    }
    else
    {
        IMCSET_BIT( IMCFLAG(ch), IMC_NOTIFY );
        imc_to_char( "You now see channel notifications.\n\r", ch );
    }
    return;
}

IMC_CMD( imcrefresh )
{
    REMOTEINFO *r, *rnext;

    for( r = first_rinfo; r; r = rnext )
    {
        rnext = r->next;
        imc_delete_reminfo( r );
    }
    imc_request_keepalive();
    imc_to_char( "Mud list is being refreshed.\n\r", ch );
    return;
}

IMC_CMD( imclast )
{
    PACKET out;

    imc_initdata( &out );
    imcstrlcpy( out.type, "imc-laston", IMC_TYPE_LENGTH );
    imcstrlcpy( out.from, CH_IMCNAME(ch), IMC_NAME_LENGTH );
    imcstrlcpy( out.to, this_imcmud->routername, IMC_NAME_LENGTH );

    imc_addkey( &out, "username", argument );

    imc_send( &out );
    imc_freedata( &out );
    return;
}

IMC_CMD( imc_other )
{
    char buf[LGST];
    IMC_CMD_DATA *cmd;
    int col, perm;

    imcstrlcpy( buf, "~gThe following commands are available:\n\r", LGST );
    imcstrlcat( buf, "~G-------------------------------------\n\r\n\r", LGST );
    for( perm = IMCPERM_MORT; perm <= IMCPERM(ch); perm++ )
    {
        col = 0;
        snprintf( buf+strlen(buf), LGST-strlen(buf), "\n\r~g%s commands:~G\n\r", imcperm_names[perm] );
        for( cmd = first_imc_command; cmd; cmd = cmd->next )
        {
            if( cmd->level != perm )
                continue;

            snprintf( buf+strlen(buf), LGST-strlen(buf), "%-15s", cmd->name );
            if( ++col % 6 == 0 )
                imcstrlcat( buf, "\n\r", LGST );
        }
        if( col % 6 != 0 )
            imcstrlcat( buf, "\n\r", LGST );
    }
    imc_to_pager( buf, ch );
    imc_to_pager( "\n\r~gFor information about a specific command, see ~Wimchelp <command>~g.\n\r", ch );
    return;
}

char *imc_find_social( CHAR_DATA *ch, char *sname, char *person, char *mud, int victim )
{
    static char socname[LGST];
    #ifdef SMAUGSOCIAL
    SOCIAL_DATA *social;
    char *c;
    #else
    int cmd;
    bool found;
    #endif

    socname[0] = '\0';

    #ifdef SMAUGSOCIAL
    for( c = sname; *c; *c = tolower(*c), c++ );

    if( ( social = find_social( sname ) ) == NULL )
    {
        imc_printf( ch, "~YSocial ~W%s~Y does not exist on this mud.\n\r", sname );
        return socname;
    }

    if( person && person[0] != '\0' && mud && mud[0] != '\0' )
    {
        if( person && person[0] != '\0' && !strcasecmp( person, CH_IMCNAME(ch) )
            && mud && mud[0] != '\0' && !strcasecmp( mud, this_imcmud->localname ) )
        {
            if( !social->others_auto )
            {
                imc_printf( ch, "~YSocial ~W%s~Y: Missing others_auto.\n\r", sname );
                return socname;
            }
            imcstrlcpy( socname, social->others_auto, LGST );
        }
        else
        {
            if( victim == 0 )
            {
                if( !social->others_found )
                {
                    imc_printf( ch, "~YSocial ~W%s~Y: Missing others_found.\n\r", sname );
                    return socname;
                }
                imcstrlcpy( socname, social->others_found, LGST );
            }
            else if( victim == 1 )
            {
                if( !social->vict_found )
                {
                    imc_printf( ch, "~YSocial ~W%s~Y: Missing vict_found.\n\r", sname );
                    return socname;
                }
                imcstrlcpy( socname, social->vict_found, LGST );
            }
            else
            {
                if( !social->char_found )
                {
                    imc_printf( ch, "~YSocial ~W%s~Y: Missing char_found.\n\r", sname );
                    return socname;
                }
                imcstrlcpy( socname, social->char_found, LGST );
            }
        }
    }
    else
    {
        if( victim == 0 || victim == 1 )
        {
            if( !social->others_no_arg )
            {
                imc_printf( ch, "~YSocial ~W%s~Y: Missing others_no_arg.\n\r", sname );
                return socname;
            }
            imcstrlcpy( socname, social->others_no_arg, LGST );
        }
        else
        {
            if( !social->char_no_arg )
            {
                imc_printf( ch, "~YSocial ~W%s~Y: Missing char_no_arg.\n\r", sname );
                return socname;
            }
            imcstrlcpy( socname, social->char_no_arg, LGST );
        }
    }
    #else
    found  = FALSE;
    for( cmd = 0; social_table[cmd].name[0] != '\0'; cmd++ )
    {
        if( sname[0] == social_table[cmd].name[0] && !imcstr_prefix( sname, social_table[cmd].name ) )
        {
            found = TRUE;
            break;
        }
    }

    if( !found )
    {
        imc_printf( ch, "~YSocial ~W%s~Y does not exist on this mud.\n\r", sname );
        return socname;
    }

    if( person && person[0] != '\0' && mud && mud[0] != '\0' )
    {
        if( person && person[0] != '\0' && !strcasecmp( person, CH_IMCNAME(ch) )
            && mud && mud[0] != '\0' && !strcasecmp( mud, this_imcmud->localname ) )
        {
            if( !social_table[cmd].others_auto )
            {
                imc_printf( ch, "~YSocial ~W%s~Y: Missing others_auto.\n\r", sname );
                return socname;
            }
            imcstrlcpy( socname, social_table[cmd].others_auto, LGST );
        }
        else
        {
            if( victim == 0 )
            {
                if( !social_table[cmd].others_found )
                {
                    imc_printf( ch, "~YSocial ~W%s~Y: Missing others_found.\n\r", sname );
                    return socname;
                }
                imcstrlcpy( socname, social_table[cmd].others_found, LGST );
            }
            else if( victim == 1 )
            {
                if( !social_table[cmd].vict_found )
                {
                    imc_printf( ch, "~YSocial ~W%s~Y: Missing vict_found.\n\r", sname );
                    return socname;
                }
                imcstrlcpy( socname, social_table[cmd].vict_found, LGST );
            }
            else
            {
                if( !social_table[cmd].char_found )
                {
                    imc_printf( ch, "~YSocial ~W%s~Y: Missing char_found.\n\r", sname );
                    return socname;
                }
                imcstrlcpy( socname, social_table[cmd].char_found, LGST );
            }
        }
    }
    else
    {
        if( victim == 0 || victim == 1 )
        {
            if( !social_table[cmd].others_no_arg )
            {
                imc_printf( ch, "~YSocial ~W%s~Y: Missing others_no_arg.\n\r", sname );
                return socname;
            }
            imcstrlcpy( socname, social_table[cmd].others_no_arg, LGST );
        }
        else
        {
            if( !social_table[cmd].char_no_arg )
            {
                imc_printf( ch, "~YSocial ~W%s~Y: Missing char_no_arg.\n\r", sname );
                return socname;
            }
            imcstrlcpy( socname, social_table[cmd].char_no_arg, LGST );
        }
    }
    #endif
    return socname;
}

/* Revised 10/10/03 by Xorith: Recognize the need to capitalize for a new sentence. */
char *imc_act_string( const char *format, CHAR_DATA *ch, CHAR_DATA *vic )
{
    static char * const he_she  [] = { "it",  "he",  "she" };
    static char * const him_her [] = { "it",  "him", "her" };
    static char * const his_her [] = { "its", "his", "her" };
    static char buf[LGST];
    char tmp_str[LGST];
    const char *i = "";
    char *point;
    bool should_upper = FALSE;

    if( !format || format[0] == '\0' || !ch )
        return NULL;

    point = buf;

    while( *format != '\0' )
    {
        if( *format == '.' || *format == '?' || *format == '!' )
            should_upper = TRUE;
        else if ( should_upper == TRUE && !isspace( *format ) && *format != '$' )
            should_upper = FALSE;

        if( *format != '$' )
        {
            *point++ = *format++;
            continue;
        }
        ++format;

        if( ( !vic ) && ( *format == 'N' || *format == 'E'
            || *format == 'M' || *format == 'S' || *format == 'K' ) )
            i = " !!!!! ";
        else
        {
            switch( *format )
            {
                default:  i = " !!!!! "; break;
                case 'n': i = imc_makename( CH_IMCNAME(ch), this_imcmud->localname, FALSE ); break;
                case 'N': i = CH_IMCNAME(vic); break;

                case 'e': i = should_upper ?
                    imccapitalize( he_she[URANGE(0, CH_IMCSEX(ch), 2)] ) : he_she[URANGE(0, CH_IMCSEX(ch), 2)];
                break;

                case 'E': i = should_upper ?
                    imccapitalize( he_she[URANGE(0, CH_IMCSEX(vic), 2)] ) : he_she[URANGE(0, CH_IMCSEX(vic), 2)];
                break;

                case 'm': i = should_upper ?
                    imccapitalize( him_her[URANGE(0, CH_IMCSEX(ch), 2)] ) : him_her[URANGE(0, CH_IMCSEX(ch), 2)];
                break;

                case 'M': i = should_upper ?
                    imccapitalize( him_her[URANGE(0, CH_IMCSEX(vic), 2)] ) : him_her[URANGE(0, CH_IMCSEX(vic), 2)];
                break;

                case 's': i = should_upper ?
                    imccapitalize(his_her[URANGE(0, CH_IMCSEX(ch), 2)]) : his_her[URANGE(0, CH_IMCSEX(ch), 2)];
                break;

                case 'S': i = should_upper ?
                    imccapitalize(his_her[URANGE(0,CH_IMCSEX(vic), 2)]) : his_her[URANGE(0, CH_IMCSEX(vic), 2)];
                break;

                case 'k': imcone_argument( CH_IMCNAME(ch), tmp_str ); i = (char *)tmp_str; break;
                case 'K': imcone_argument( CH_IMCNAME(vic), tmp_str ); i = (char *)tmp_str; break;
            }
        }
        ++format;
        while( ( *point = *i ) != '\0' )
            ++point, ++i;
    }
    *point = 0;
    point++;
    *point = '\0';

    buf[0] = UPPER( buf[0] );
    return buf;
}

CHAR_DATA *imc_make_skeleton( char *name )
{
    CHAR_DATA *skeleton;

    IMCCREATE( skeleton, CHAR_DATA, 1 );

    #ifdef IMCCIRCLE
    skeleton->player.name = IMCSTRALLOC( name );
    skeleton->player.short_descr = IMCSTRALLOC( name );
    skeleton->in_room = real_room( 1 );
    #else
    skeleton->name = IMCSTRALLOC( name );
    skeleton->in_room = get_room_index( ROOM_VNUM_LIMBO );
    #endif

    return skeleton;
}

void imc_purge_skeleton( CHAR_DATA *skeleton )
{
    if( !skeleton )
        return;

    #ifdef IMCCIRCLE
    IMCSTRFREE( skeleton->player.name );
    IMCSTRFREE( skeleton->player.short_descr );
    #else
    IMCSTRFREE( (char *)skeleton->name );
    #endif
    IMCDISPOSE( skeleton );

    return;
}

/* Socials can now be called anywhere you want them - like for instance, tells.
 * Thanks to Darien@Sandstorm for this suggestion. -- Samson 2-21-04
 */
char *imc_send_social( CHAR_DATA *ch, char *argument, int telloption )
{
    CHAR_DATA *skeleton = NULL;
    char *ps;
    char socbuf[LGST], msg[LGST];
    char arg1[SMST], person[SMST], mud[SMST], buf[LGST];
    unsigned int x;

    person[0] = '\0';
    mud[0] = '\0';

    /* Name of social, remainder of argument is assumed to hold the target */
    argument = imcone_argument( argument, arg1 );

    if( argument && argument[0] != '\0' )
    {
        if( !( ps = strchr( argument, '@' ) ) )
        {
            imc_to_char( "You need to specify a person@mud for a target.\n\r", ch );
            return "";
        }
        else
        {
            for( x = 0; x < strlen( argument ); x++ )
            {
                person[x] = argument[x];
                if( person[x] == '@' )
                    break;
            }
            person[x] = '\0';

            ps[0] = '\0';
            imcstrlcpy( mud, ps+1, SMST );
        }
    }

    if( telloption == 0 )
    {
        snprintf( socbuf, LGST, "%s", imc_find_social( ch, arg1, person, mud, 0 ) );
        if( !socbuf || socbuf[0] == '\0' )
            return "";
    }

    if( telloption == 1 )
    {
        snprintf( socbuf, LGST, "%s", imc_find_social( ch, arg1, person, mud, 1 ) );
        if( !socbuf || socbuf[0] == '\0' )
            return "";
    }

    if( telloption == 2 )
    {
        snprintf( socbuf, LGST, "%s", imc_find_social( ch, arg1, person, mud, 2 ) );
        if( !socbuf || socbuf[0] == '\0' )
            return "";
    }

    if( argument && argument[0] != '\0' )
    {
        int sex;

        snprintf( buf, LGST, "%s@%s", person, mud );
        sex = imc_get_ucache_gender( buf );
        if( sex == -1 )
        {
            imc_send_ucache_request( buf );
            sex = SEX_MALE;
        }
        else
            sex = imctodikugender( sex );

        skeleton = imc_make_skeleton( buf );
        CH_IMCSEX(skeleton) = sex;
    }

    imcstrlcpy( msg, (char *)imc_act_string( socbuf, ch, skeleton ), LGST );
    if( skeleton )
        imc_purge_skeleton( skeleton );
    return( color_mtoi(msg) );
}

char *imc_funcname( IMC_FUN *func )
{
    if( func == imc_other )                return( "imc_other" );
    if( func == imclisten )                return( "imclisten" );
    if( func == imcchanlist )              return( "imcchanlist" );
    if( func == imclist )                  return( "imclist" );
    if( func == imcinvis )                 return( "imcinvis" );
    if( func == imcwho )                   return( "imcwho" );
    if( func == imclocate )                return( "imclocate" );
    if( func == imctell )                  return( "imctell" );
    if( func == imcreply )                 return( "imcreply" );
    if( func == imcbeep )                  return( "imcbeep" );
    if( func == imcignore )                return( "imcignore" );
    if( func == imcfinger )                return( "imcfinger" );
    if( func == imcinfo )                  return( "imcinfo" );
    if( func == imccolor )                 return( "imccolor" );
    if( func == imcafk )                   return( "imcafk" );
    if( func == imcchanwho )               return( "imcchanwho" );
    if( func == imcconnect )               return( "imcconnect" );
    if( func == imcdisconnect )            return( "imcdisconnect" );
    if( func == imcpermstats )             return( "imcpermstats" );
    if( func == imc_deny_channel )         return( "imc_deny_channel" );
    if( func == imcpermset )               return( "imcpermset" );
    if( func == imcsetup )                 return( "imcsetup" );
    if( func == imccommand )               return( "imccommand" );
    if( func == imcban )                   return( "imcban" );
    if( func == imcconfig )                return( "imcconfig" );
    if( func == imcping )                  return( "imcping" );
    if( func == imcstats )                 return( "imcstats" );
    if( func == imc_show_ucache_contents ) return( "imc_show_ucache_contents" );
    if( func == imcremoteadmin )           return( "imcremoteadmin" );
    if( func == imcdebug )                 return( "imcdebug" );
    if( func == imchedit )                 return( "imchedit" );
    if( func == imchelp )                  return( "imchelp" );
    if( func == imccedit )                 return( "imccedit" );
    if( func == imcnotify )                return( "imcnotify" );
    if( func == imcrefresh )               return( "imcrefresh" );
    if( func == imclast )                  return( "imclast" );

    return "";
}

IMC_FUN *imc_function( const char *func )
{
    if( !strcasecmp( func, "imc_other" ) )                   return imc_other;
    if( !strcasecmp( func, "imclisten" ) )                   return imclisten;
    if( !strcasecmp( func, "imcchanlist" ) )                 return imcchanlist;
    if( !strcasecmp( func, "imclist" ) )                     return imclist;
    if( !strcasecmp( func, "imcinvis" ) )                    return imcinvis;
    if( !strcasecmp( func, "imcwho" ) )                      return imcwho;
    if( !strcasecmp( func, "imclocate" ) )                   return imclocate;
    if( !strcasecmp( func, "imctell" ) )                     return imctell;
    if( !strcasecmp( func, "imcreply" ) )                    return imcreply;
    if( !strcasecmp( func, "imcbeep" ) )                     return imcbeep;
    if( !strcasecmp( func, "imcignore" ) )                   return imcignore;
    if( !strcasecmp( func, "imcfinger" ) )                   return imcfinger;
    if( !strcasecmp( func, "imcinfo" ) )                     return imcinfo;
    if( !strcasecmp( func, "imccolor" ) )                    return imccolor;
    if( !strcasecmp( func, "imcafk" ) )                      return imcafk;
    if( !strcasecmp( func, "imcchanwho" ) )                  return imcchanwho;
    if( !strcasecmp( func, "imcconnect" ) )                  return imcconnect;
    if( !strcasecmp( func, "imcdisconnect" ) )               return imcdisconnect;
    if( !strcasecmp( func, "imcpermstats" ) )                return imcpermstats;
    if( !strcasecmp( func, "imc_deny_channel" ) )            return imc_deny_channel;
    if( !strcasecmp( func, "imcpermset" ) )                  return imcpermset;
    if( !strcasecmp( func, "imcsetup" ) )                    return imcsetup;
    if( !strcasecmp( func, "imccommand" ) )                  return imccommand;
    if( !strcasecmp( func, "imcban" ) )                      return imcban;
    if( !strcasecmp( func, "imcconfig" ) )                   return imcconfig;
    if( !strcasecmp( func, "imcping" ) )                     return imcping;
    if( !strcasecmp( func, "imcstats" ) )                    return imcstats;
    if( !strcasecmp( func, "imc_show_ucache_contents" ) )    return imc_show_ucache_contents;
    if( !strcasecmp( func, "imcremoteadmin" ) )              return imcremoteadmin;
    if( !strcasecmp( func, "imcdebug" ) )                    return imcdebug;
    if( !strcasecmp( func, "imchelp" ) )                     return imchelp;
    if( !strcasecmp( func, "imccedit" ) )                    return imccedit;
    if( !strcasecmp( func, "imchedit" ) )                    return imchedit;
    if( !strcasecmp( func, "imcnotify" ) )                   return imcnotify;
    if( !strcasecmp( func, "imcrefresh" ) )                  return imcrefresh;
    if( !strcasecmp( func, "imclast" ) )                     return imclast;

    return NULL;
}

/* Check for IMC channels, return TRUE to stop command processing, FALSE otherwise */
bool imc_command_hook( CHAR_DATA *ch, char *command, char *argument )
{
    IMC_CMD_DATA *cmd;
    IMC_ALIAS *alias;
    IMC_CHANNEL *c;
    char *p;

    if( IS_NPC(ch) )
        return FALSE;

    if( !this_imcmud )
    {
        imcbug( "%s", "Ooops. IMC being called with no configuration!" );
        return FALSE;
    }

    if( !first_imc_command )
    {
        imcbug( "%s", "ACK! There's no damn command data loaded!" );
        return FALSE;
    }

    if( IMCPERM(ch) <= IMCPERM_NONE )
        return FALSE;

    #ifdef IMCCIRCLE
    /* CircleMUD parser leaves leading spaces after splitting one argument */
    skip_spaces( &argument );
    #endif

    /* Simple command interpreter menu. Nothing overly fancy etc, but it beats trying to tie directly into the mud's
     * own internal structures. Especially with the differences in codebases.
     */
    for( cmd = first_imc_command; cmd; cmd = cmd->next )
    {
        if( IMCPERM(ch) < cmd->level )
            continue;

        for( alias = cmd->first_alias; alias; alias = alias->next )
        {
            if( !strcasecmp( command, alias->name ) )
            {
                command = cmd->name;
                break;
            }
        }

        if( !strcasecmp( command, cmd->name ) )
        {
            if( cmd->connected == TRUE && imc_active < IA_UP )
            {
                imc_to_char( "The mud is not currently connected to IMC2.\n\r", ch );
                return TRUE;
            }

            if( cmd->function == NULL )
            {
                imc_to_char( "That command has no code set. Inform the administration.\n\r", ch );
                imcbug( "imc_command_hook: Command %s has no code set!", cmd->name );
                return TRUE;
            }

            ( *cmd->function )( ch, argument );
            return TRUE;
        }
    }

    /* Assumed to be aiming for a channel if you get this far down */
    c = imc_findlchannel( command );

    if( !c || c->level > IMCPERM(ch) )
        return FALSE;

    if( imc_hasname( IMC_DENY(ch), c->local_name ) )
    {
        imc_printf( ch, "You have been denied the use of %s by the administration.\n\r", c->local_name );
        return TRUE;
    }

    if( !c->refreshed )
    {
        imc_printf( ch, "The %s channel has not yet been refreshed by the router.\n\r", c->local_name );
        return TRUE;
    }

    if( !argument || argument[0] == '\0' )
    {
        int y;

        imc_printf( ch, "~cThe last %d %s messages:\n\r", MAX_IMCHISTORY, c->local_name );
        for( y = 0; y < MAX_IMCHISTORY; y++ )
        {
            if( c->history[y] != NULL )
                imc_printf( ch, "%s\n\r", c->history[y] );
            else
                break;
        }
        return TRUE;
    }

    if( IMCPERM(ch) >= IMCPERM_ADMIN && !strcasecmp( argument, "log" ) )
    {
        if( !IMCIS_SET( c->flags, IMCCHAN_LOG ) )
        {
            IMCSET_BIT( c->flags, IMCCHAN_LOG );
            imc_printf( ch, "~RFile logging enabled for %s, PLEASE don't forget to undo this when it isn't needed!\n\r", c->local_name );
        }
        else
        {
            IMCREMOVE_BIT( c->flags, IMCCHAN_LOG );
            imc_printf( ch, "~GFile logging disabled for %s.\n\r", c->local_name );
        }
        imc_save_channels();
        return TRUE;
    }

    if( !imc_hasname( IMC_LISTEN(ch), c->local_name ) )
    {
        imc_printf( ch, "You are not currently listening to %s. Use the imclisten command to listen to this channel.\n\r", c->local_name );
        return TRUE;
    }

    switch( argument[0] )
    {
        case ',':
            /* Strip the , and then extra spaces - Remcon 6-28-03 */
            argument++;
            while( isspace( *argument ) ) argument++;
            imc_sendmessage( c, CH_IMCNAME(ch), color_mtoi(argument), 1 );
            break;
        case '@':
            /* Strip the @ and then extra spaces - Remcon 6-28-03 */
            argument++;
            while( isspace( *argument ) ) argument++;
            p = imc_send_social( ch, argument, 0 );
            if( !p || p[0] == '\0' )
                return TRUE;
            imc_sendmessage( c, CH_IMCNAME(ch), p, 2 );
            break;
        default:
            imc_sendmessage( c, CH_IMCNAME(ch), color_mtoi(argument), 0 );
            break;
    }
    return TRUE;
}
