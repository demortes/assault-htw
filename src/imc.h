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

#define IMC FALSE

#ifndef __IMC2_H__
#define __IMC2_H__

/* Number of entries to keep in the channel histories */
#define MAX_IMCHISTORY 20
#define MAX_IMCTELLHISTORY 20

/* This is the protocol version */
#define IMC_VERSION 2
#define IMC_VERSION_ID "IMC2 AntiFreeze CL-4 "

/* Remcon: Ask and ye shall receive. */
#define IMC_DIR          "../imc/"

#define IMC_CHANNEL_FILE IMC_DIR "imc.channels"
#define IMC_CONFIG_FILE  IMC_DIR "imc.config"
#define IMC_BAN_FILE     IMC_DIR "imc.ignores"
#define IMC_UCACHE_FILE  IMC_DIR "imc.ucache"
#define IMC_COLOR_FILE   IMC_DIR "imc.color"
#define IMC_HELP_FILE    IMC_DIR "imc.help"
#define IMC_CMD_FILE     IMC_DIR "imc.commands"

/* Make sure you set the macros in the imccfg.h file properly or things get ugly from here. */
#include "imccfg.h"

typedef enum
{
    IMCPERM_NOTSET, IMCPERM_NONE, IMCPERM_MORT, IMCPERM_IMM, IMCPERM_ADMIN, IMCPERM_IMP
} imc_permissions;

/* Flag macros */
#define IMCIS_SET(flag, bit)    ((flag) & (bit))
#define IMCSET_BIT(var, bit)    ((var) |= (bit))
#define IMCREMOVE_BIT(var, bit)((var) &= ~(bit))

/* Channel flags, only one so far, but you never know when more might be useful */
#define IMCCHAN_LOG      (1 <<  0)

/* Player flags */
#define IMC_TELL         (1 <<  0)
#define IMC_DENYTELL     (1 <<  1)
#define IMC_BEEP         (1 <<  2)
#define IMC_DENYBEEP     (1 <<  3)
#define IMC_INVIS        (1 <<  4)
#define IMC_PRIVACY      (1 <<  5)
#define IMC_DENYFINGER   (1 <<  6)
#define IMC_AFK          (1 <<  7)
#define IMC_COLORFLAG    (1 <<  8)
#define IMC_PERMOVERRIDE (1 <<  9)
#define IMC_NOTIFY       (1 << 10)

#define IMCPERM(ch)           (CH_IMCDATA((ch))->imcperm)
#define IMCFLAG(ch)           (CH_IMCDATA((ch))->imcflag)
#define FIRST_IMCIGNORE(ch)   (CH_IMCDATA((ch))->imcfirst_ignore)
#define LAST_IMCIGNORE(ch)    (CH_IMCDATA((ch))->imclast_ignore)
#define IMC_LISTEN(ch)        (CH_IMCDATA((ch))->imc_listen)
#define IMC_DENY(ch)          (CH_IMCDATA((ch))->imc_denied)
#define IMC_RREPLY(ch)        (CH_IMCDATA((ch))->rreply)
#define IMC_RREPLY_NAME(ch)   (CH_IMCDATA((ch))->rreply_name)
#define IMC_EMAIL(ch)         (CH_IMCDATA((ch))->email)
#define IMC_HOMEPAGE(ch)      (CH_IMCDATA((ch))->homepage)
#define IMC_AIM(ch)           (CH_IMCDATA((ch))->aim)
#define IMC_ICQ(ch)           (CH_IMCDATA((ch))->icq)
#define IMC_YAHOO(ch)         (CH_IMCDATA((ch))->yahoo)
#define IMC_MSN(ch)           (CH_IMCDATA((ch))->msn)
#define IMC_COMMENT(ch)       (CH_IMCDATA((ch))->comment)
#define IMCTELLHISTORY(ch,x)  (CH_IMCDATA((ch))->imc_tellhistory[(x)])
#define IMCISINVIS(ch)        ( IMCIS_SET( IMCFLAG((ch)), IMC_INVIS ) || hidden(ch) )
#define IMCAFK(ch)            ( IMCIS_SET( IMCFLAG((ch)), IMC_AFK ) || IS_SET(ch->pcdata->pflags,PFLAG_AFK) )

/* Should not need to edit anything below this point */

#define LGST 4096                                           /* Large String */
#define SMST 1024                                           /* Small String */

/* Macro taken from DOTD codebase. Fcloses a file, then nulls its pointer for safety. */
#define IMCFCLOSE(fp)  fclose((fp)); (fp)=NULL;

/*
 * Memory allocation macros.
 */
#define IMCCREATE(result, type, number) \
    do \
    { \
        if (!((result) = (type *) calloc ((number), sizeof(type)))) \
        { \
            imclog( "Malloc failure @ %s:%d\n", __FILE__, __LINE__ ); \
            abort(); \
        } \
    } while(0)

#define IMCDISPOSE(point) \
    do \
    { \
        if((point)) \
        { \
            free((point)); \
            (point) = NULL; \
        } \
    } while(0)

#define IMCSTRALLOC strdup
#define IMCSTRFREE IMCDISPOSE

    /* double-linked list handling macros -Thoric ( From the Smaug codebase ) */
        /* Updated by Scion 8/6/1999 */
#define IMCLINK(link, first, last, next, prev) \
    do \
    { \
        if ( !(first) ) \
        { \
            (first) = (link); \
            (last) = (link); \
        } \
        else \
        (last)->next = (link); \
        (link)->next = NULL; \
        if ((first) == (link)) \
        (link)->prev = NULL; \
        else \
        (link)->prev = (last); \
        (last) = (link); \
    } while(0)

#define IMCINSERT(link, insert, first, next, prev) \
    do \
    { \
        (link)->prev = (insert)->prev; \
        if ( !(insert)->prev ) \
        (first) = (link); \
        else \
        (insert)->prev->next = (link); \
        (insert)->prev = (link); \
        (link)->next = (insert); \
    } while(0)

#define IMCUNLINK(link, first, last, next, prev) \
    do \
    { \
        if ( !(link)->prev ) \
        { \
            (first) = (link)->next; \
            if((first)) \
            (first)->prev = NULL; \
        } \
        else \
        { \
            (link)->prev->next = (link)->next; \
        } \
        if( !(link)->next ) \
        { \
            (last) = (link)->prev; \
            if((last)) \
            (last)->next = NULL; \
        } \
        else \
        { \
            (link)->next->prev = (link)->prev; \
        } \
    } while(0)

                        /* activation states */
#define IA_NONE        0
#define IA_CONFIG1     1
#define IA_CONFIG2     2
#define IA_UP          3

                                /* connection states */
#define IMC_CLOSED     0                                    /* No active connection */
#define IMC_CONNECTING 1                                    /* Contacting router */
#define IMC_WAIT1      2                                    /* Waiting for router verification */
#define IMC_CONNECTED  3                                    /* Fully connected */

                                                /* Less tweakable parameters - only change these if you know what they do */

                                                        /* number of packets to remember at a time */
#define IMC_MEMORY 256

                                                        /* start dropping really old packets based on this figure */
#define IMC_PACKET_LIFETIME 60

                                                        /* min input/output buffer size */
#define IMC_MINBUF        256

                                                        /* max input/output buffer size */
#define IMC_MAXBUF        16384

                                                        /* Changing these impacts the protocol itself - other muds may drop your
                                                         * packets if you get this wrong
                                                         */
                                                        /* max length of any packet */
#define IMC_PACKET_LENGTH 16300

                                                        /* max length of any mud name */
#define IMC_MNAME_LENGTH  20

                                                        /* max length of any player name */
#define IMC_PNAME_LENGTH  40

                                                        /* max length of any player@mud name */
#define IMC_NAME_LENGTH   (IMC_MNAME_LENGTH+IMC_PNAME_LENGTH+1)

                                                        /* max length of a path */
#define IMC_PATH_LENGTH   200

                                                        /* max length of a packet type */
#define IMC_TYPE_LENGTH   20

                                                        /* max length of a password */
#define IMC_PW_LENGTH     20

                                                        /* max length of a data type (estimate) */
#define IMC_DATA_LENGTH   (IMC_PACKET_LENGTH-2*IMC_NAME_LENGTH-IMC_PATH_LENGTH-IMC_TYPE_LENGTH-20)

                                                        /* max number of data keys in a packet */
#define IMC_MAX_KEYS      20

                                                        /* No real functional difference in alot of this, but double linked lists DO seem to handle better,
                                                         * and they look alot neater too. Yes, readability IS important! - Samson
                                                         */
                                                            /* Channels, both local and non-local */
                                                                                typedef struct imc_channel IMC_CHANNEL;
typedef struct imc_siteinfo SITEINFO;                       /* The given mud :) */
typedef struct imc_packet PACKET;                           /* It's a packet! */
typedef struct imc_remoteinfo REMOTEINFO;                   /* Information on a mud connected to IMC */
typedef struct imc_ban_data IMC_BAN;                        /* Mud level bans */
typedef struct imcchar_data IMC_CHARDATA;                   /* Player flags */
typedef struct imc_ignore IMC_IGNORE;                       /* Player level ignores */
typedef struct imcucache_data IMCUCACHE_DATA;               /* User cache data for gender targetting socials */
typedef struct imc_color_table IMC_COLOR;                   /* The Color config */
typedef struct imc_command_table IMC_CMD_DATA;              /* Command table */
typedef struct imc_help_table IMC_HELP_DATA;                /* Help table */
typedef struct imc_cmd_alias IMC_ALIAS;                     /* Big, bad, bloated command alias thing */
typedef struct imc_packet_handler IMC_PHANDLER;             /* custom packet handlers added dynamically */

typedef void IMC_FUN( CHAR_DATA *ch, char *argument );
#define IMC_CMD( name ) void (name)( CHAR_DATA *ch, char *argument )

extern REMOTEINFO *first_rinfo;
extern REMOTEINFO *last_rinfo;
extern SITEINFO *this_imcmud;

/* Oh yeah, baby, that rauncy looking Merc structure just got the facelift of the century.
 * Thanks to Thoric and friends for the slick idea.
 */
struct imc_cmd_alias
{
    IMC_ALIAS *next;
    IMC_ALIAS *prev;
    char *name;
};

struct imc_command_table
{
    IMC_CMD_DATA *next;
    IMC_CMD_DATA *prev;
    IMC_ALIAS *first_alias;
    IMC_ALIAS *last_alias;
    IMC_FUN *function;
    char *name;
    int level;
    bool connected;
};

struct imc_help_table
{
    IMC_HELP_DATA *next;
    IMC_HELP_DATA *prev;
    char *name;
    char *text;
    int level;
};

struct imc_color_table
{
    IMC_COLOR *next;
    IMC_COLOR *prev;
    char *name;                                             /* the name of the color */
    char *mudtag;                                           /* What the mud uses for the raw tag */
    char *imctag;                                           /* The imc tilde code that represents the mudtag to the network */
};

struct imc_ignore
{
    IMC_IGNORE *next;
    IMC_IGNORE *prev;
    char *name;
};

struct imcucache_data
{
    IMCUCACHE_DATA *next;
    IMCUCACHE_DATA *prev;
    char *name;
    int gender;
    time_t time;
};

struct imcchar_data
{
    IMC_IGNORE   *imcfirst_ignore;                          /* List of ignored people */
    IMC_IGNORE   *imclast_ignore;
    char *rreply;                                           /* IMC reply-to */
    char *rreply_name;                                      /* IMC reply-to shown to char */
    char *imc_listen;                                       /* Channels the player is listening to */
    char *imc_denied;                                       /* Channels the player has been denied use of */
    char *imc_tellhistory[MAX_IMCTELLHISTORY];              /* History of received imctells - Samson 1-21-04 */
    char *email;                                            /* Person's email address - for imcfinger - Samson 3-21-04 */
    char *homepage;                                         /* Person's homepage - Samson 3-21-04 */
    char *aim;                                              /* Person's AOL Instant Messenger screenname - Samson 3-21-04 */
    char *yahoo;                                            /* Person's Y! screenname - Samson 3-21-04 */
    char *msn;                                              /* Person's MSN Messenger screenname - Samson 3-21-04 */
    char *comment;                                          /* Person's personal comment - Samson 3-21-04 */
    int icq;                                                /* Person's ICQ UIN Number - Samson 3-21-04 */
    int imcperm;                                            /* Permission level for the player */
    int imcflag;                                            /* Flags set on the player */
};

/* a player on IMC */
typedef struct
{
    char name[IMC_NAME_LENGTH];                             /* name of character */
    int level;                                              /* permission level */
} imc_char_data;

struct imc_channel
{
    IMC_CHANNEL *next;
    IMC_CHANNEL *prev;
    char *name;                                             /* name of channel */
    char *owner;                                            /* owner (singular) of channel */
    char *operators;                                        /* current operators of channel */
    char *invited;
    char *excluded;
    char *local_name;                                       /* Operational localname */
    char *dlname;                                           /* Default localname - sent by router */
    char *regformat;
    char *emoteformat;
    char *socformat;
    char *history[MAX_IMCHISTORY];
    bool open;
    bool refreshed;
    short level;
    long flags;
};

/* an IMC packet, as seen by the high-level code */
struct imc_packet
{
    char to[IMC_NAME_LENGTH];                               /* destination of packet */
    char from[IMC_NAME_LENGTH];                             /* source of packet      */
    char type[IMC_TYPE_LENGTH];                             /* type of packet        */
    char *key[IMC_MAX_KEYS];
    char *value[IMC_MAX_KEYS];

    /* internal things which only the low-level code needs to know about */
    struct
    {
        char to[IMC_NAME_LENGTH];
        char from[IMC_NAME_LENGTH];
        char path[IMC_PATH_LENGTH];
        unsigned long sequence;
    } i;
};

/* The mud's connection data for the router */
/* Merged the old siteinfo struct here as well */
struct imc_siteinfo
{
    char *routername;                                       /* name of router */
    char *rhost;                                            /* DNS/IP of router */
    char *network;                                          /* Network name of the router, set at keepalive - Samson */
    char *serverpw;                                         /* server password */
    char *clientpw;                                         /* client password */
    char *localname;                                        /* One word localname */
    char *fullname;                                         /* FULL name of mud */
    char *ihost;                                            /* host AND port of mud */
    char *email;                                            /* contact address (email) */
    char *www;                                              /* homepage */
    char *base;                                             /* The mud's codebase name */
    char *details;                                          /* BRIEF description of mud */
    unsigned short rport;                                   /* remote port of router */
    bool md5;                                               /* Client will support MD5 authentication */
    bool md5pass;                                           /* Client is using MD5 authentication */
    bool autoconnect;                                       /* Do we autoconnect on bootup or not? - Samson */
    int iport;                                              /* The port the mud itself is on */
    int minlevel;                                           /* Minimum player level */
    int immlevel;                                           /* Immortal level */
    int adminlevel;                                         /* Admin level */
    int implevel;                                           /* Implementor level */

    /* Conection parameters - These don't save in the config file */
    int desc;                                               /* descriptor */
    unsigned short state;                                   /* IMC_xxxx state */
    unsigned short version;                                 /* version of remote site */
    short newoutput;                                        /* try to write at end of cycle regardless of fd_set state? */
    char *inbuf;                                            /* input buffer */
    int insize;
    char *outbuf;                                           /* output buffer */
    int outsize;
    bool disconnect;                                        /* Will we disconnect at the next imc_loop call? */
};

/* IMC statistics */
typedef struct
{
    long rx_pkts;                                           /* Received packets                      */
    long tx_pkts;                                           /* Transmitted packets                   */
    long rx_bytes;                                          /* Received bytes                        */
    long tx_bytes;                                          /* Transmitted bytes                     */

    int max_pkt;                                            /* Max. size packet processed            */
    int sequence_drops;                                     /* Dropped packets due to age            */
} imc_statistics;

/* info about another mud on IMC */
struct imc_remoteinfo
{
    REMOTEINFO *next;
    REMOTEINFO *prev;
    char *name;
    char *version;
    char *network;
    char *path;
    char *url;
    bool expired;
    unsigned long top_sequence;
};

/* A mudwide ban */
struct imc_ban_data
{
    IMC_BAN *next;
    IMC_BAN *prev;
    char *name;
};

/* for the versioning table */
typedef struct
{
    int version;
    char *(*generate) (PACKET *);
    PACKET *(*interpret) (char *);
} _imc_vinfo;

/* an entry in the memory table */
typedef struct
{
    char *from;
    unsigned long sequence;
} _imc_memory;

struct imc_packet_handler
{
    IMC_PHANDLER *next;
    IMC_PHANDLER *prev;
    char *name;
    void (*func)( imc_char_data *from, PACKET *p );
};

extern _imc_vinfo imc_vinfo[];                              /* Version table */
extern char *IMC_VERSIONID;

/* External functions hooked to the mud */
bool imc_command_hook( CHAR_DATA *ch, char *command, char *argument );
void imc_startup( bool forced );
void imc_shutdown( bool reconnect );
void imc_initchar( CHAR_DATA *ch );
bool imc_loadchar( CHAR_DATA *ch, FILE *fp, const char *word );
void imc_savechar( CHAR_DATA *ch, FILE *fp );
void imc_freechardata( CHAR_DATA *ch );
void imc_loop( void );
void imc_register_packet_handler( char *name, void (*func)( imc_char_data *from, PACKET *p ) );
#ifdef _DISKIO_H_
void imc_load_pfile( CHAR_DATA *ch, char *tag, int num, char *line );
void imc_save_pfile( struct char_data *ch, FBFILE *fp );
#endif
#endif
