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

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/telnet.h>
#include "ack.h"
#include "globals.h"

char    compress_start  [] = { IAC, SB, TELOPT_COMPRESS, WILL, SE, '\0' };
char    compress2_start  [] = { IAC, SB, TELOPT_COMPRESS2, WILL, SE, '\0' };
char    compress_on_str   [] = { IAC, WILL, TELOPT_COMPRESS, '\0' };
char    compress2_on_str   [] = { IAC, WILL, TELOPT_COMPRESS2, '\0' };

bool processCompressed(DESCRIPTOR_DATA *desc);
bool    write_to_descriptor     args( ( DESCRIPTOR_DATA *d, char *txt, int length ) );

void *zlib_alloc(void *opaque, unsigned int items, unsigned int size)
{
    return calloc(items, size);
}

void zlib_free(void *opaque, void *address)
{
    free(address);
}

bool process_compressed(DESCRIPTOR_DATA *d)
{
    int iStart = 0, nBlock, nWrite, len;

    if (!d->out_compress)
        return TRUE;

    // Try to write out some data..
    len = d->out_compress->next_out - d->out_compress_buf;

    if (len > 0)
    {
        // we have some data to write
        for (iStart = 0; iStart < len; iStart += nWrite)
        {
            nBlock = UMIN (len - iStart, 4096);
            if ((nWrite = write(d->descriptor, d->out_compress_buf + iStart, nBlock)) < 0)
            {
                if (errno == EAGAIN ||
                        errno == ENOSR)
                    break;

                return FALSE;
            }

            if (!nWrite)
                break;
        }

        if (iStart)
        {
            // We wrote "iStart" bytes
            if (iStart < len)
                memmove(d->out_compress_buf, d->out_compress_buf+iStart, len - iStart);

            d->out_compress->next_out = d->out_compress_buf + len - iStart;
        }
    }

    return TRUE;
}

bool write_compressed( DESCRIPTOR_DATA * d, char *txt, int length )
{
    int     iStart = 0;
    int     nWrite = 0;
    int     nBlock;
    int     len;

    if (length <= 0)
        length = strlen(txt);

    if (d && d->out_compress)
    {
        d->out_compress->next_in = (unsigned char *)txt;
        d->out_compress->avail_in = length;

        while (d->out_compress->avail_in)
        {
            d->out_compress->avail_out = COMPRESS_BUF_SIZE - (d->out_compress->next_out - d->out_compress_buf);

            if (d->out_compress->avail_out)
            {
                int status = deflate(d->out_compress, Z_SYNC_FLUSH);

                if (status != Z_OK)
                    return FALSE;
            }

            len = d->out_compress->next_out - d->out_compress_buf;
            if (len > 0)
            {
                for (iStart = 0; iStart < len; iStart += nWrite)
                {
                    nBlock = UMIN (len - iStart, 4096);
                    if ((nWrite = write(d->descriptor, d->out_compress_buf + iStart, nBlock)) < 0)
                    {
                        perror( "Write_to_descriptor: compressed" );
                        return FALSE;
                    }

                    if (!nWrite)
                        break;
                }

                if (!iStart)
                    break;

                if (iStart < len)
                    memmove(d->out_compress_buf, d->out_compress_buf+iStart, len - iStart);

                d->out_compress->next_out = d->out_compress_buf + len - iStart;
            }
        }
        return TRUE;
    }

    for (iStart = 0; iStart < length; iStart += nWrite)
    {
        nBlock = UMIN (length - iStart, 4096);
        if ((nWrite = write(d->descriptor, txt + iStart, nBlock)) < 0)
        {
            perror( "Write_to_descriptor" );
            return FALSE;
        }
    }

    return TRUE;
}

char enable_compress[] =
{
    IAC, SB, TELOPT_COMPRESS, WILL, SE, 0
};
char enable_compress2[] =
{
    IAC, SB, TELOPT_COMPRESS2, IAC, SE, 0
};

bool compressStart(DESCRIPTOR_DATA *d, unsigned char telopt)
{
    z_stream *s;

    if (d->out_compress)
        return TRUE;

    //    bug("Starting compression for descriptor %d", d->descriptor);

    s = (z_stream *)malloc(sizeof(*s));
    d->out_compress_buf = (unsigned char *)malloc(COMPRESS_BUF_SIZE);

    s->next_in = NULL;
    s->avail_in = 0;

    s->next_out = d->out_compress_buf;
    s->avail_out = COMPRESS_BUF_SIZE;

    s->zalloc = zlib_alloc;
    s->zfree  = zlib_free;
    s->opaque = NULL;

    if (deflateInit(s, 9) != Z_OK)
    {
        free(d->out_compress_buf);
        free(s);
        return FALSE;
    }

    if (telopt == TELOPT_COMPRESS)
        write_to_descriptor(d, enable_compress, 0);
    else if (telopt == TELOPT_COMPRESS2)
        write_to_descriptor(d, enable_compress2, 0);
    //    else
    //        bug("compressStart: bad TELOPT passed");

    d->compressing = telopt;
    d->out_compress = s;

    /*    if ( d->character )
        {
        if ( telopt == 85 && !IS_SET(d->character->config,CONFIG_COMPRESS))
                SET_BIT(d->character->config,CONFIG_COMPRESS);
        if ( telopt == 86 && !IS_SET(d->character->config,CONFIG_COMPRESS2))
                SET_BIT(d->character->config,CONFIG_COMPRESS2);
        }*/
    return TRUE;
}

bool compressEnd(DESCRIPTOR_DATA *d,unsigned char type)
{
    unsigned char dummy[1];
    unsigned char telopt;

    if (!d->out_compress)
        return TRUE;
    if (d->compressing != type)
        return FALSE;

    //    bug("Stopping compression for descriptor %d", d->descriptor);

    d->out_compress->avail_in = 0;
    d->out_compress->next_in = dummy;
    telopt = d->compressing;

    if (deflate(d->out_compress, Z_FINISH) != Z_STREAM_END)
        return FALSE;

    if ( !d )
        return FALSE;

    if (!process_compressed(d))
        return FALSE;

    deflateEnd(d->out_compress);
    free(d->out_compress_buf);
    free(d->out_compress);
    d->compressing = 0;
    d->out_compress_buf = NULL;
    d->out_compress = NULL;

    /*    if ( d->character )
        {
        if ( telopt == 85 && IS_SET(d->character->config,CONFIG_COMPRESS))
                REMOVE_BIT(d->character->config,CONFIG_COMPRESS);
        if ( telopt == 86 && IS_SET(d->character->config,CONFIG_COMPRESS2))
                REMOVE_BIT(d->character->config,CONFIG_COMPRESS2);
        }*/
    return TRUE;
}

void do_compress( CHAR_DATA *ch, char *argument )
{
    if (!ch->desc)
    {
        send_to_char("What descriptor?!\n", ch);
        return;
    }

    if (!ch->desc->out_compress)
    {

        if ( str_cmp(argument,"2") )
            write_to_buffer( ch->desc, compress_on_str, 0 );
        if ( str_cmp(argument,"1") )
            write_to_buffer( ch->desc, compress2_on_str, 0 );
        send_to_char("Ok, compression enabled.\n", ch);
    }
    else
    {
        if (!compressEnd(ch->desc,ch->desc->compressing))
        {
            send_to_char("Failed.\n", ch);
            return;
        }

        send_to_char("Ok, compression disabled.\n", ch);
    }

    return;
}
