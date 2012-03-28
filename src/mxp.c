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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ack.h"
#include "config.h"

extern unsigned char will_mxp_str  [];
extern unsigned char start_mxp_str [];
extern unsigned char do_mxp_str    [];
extern unsigned char dont_mxp_str  [];

/*
 * Count number of mxp tags need converting
 *    ie. < becomes &lt;
 *        > becomes &gt;
 *        & becomes &amp;
 */

int count_mxp_tags (const int bMXP, const char *txt, int length)
{
    char c;
    const char * p;
    int count;
    int bInTag = FALSE;
    int bInEntity = FALSE;

    for (p = txt, count = 0;
        length > 0;
        p++, length--)
    {
        c = *p;

        if (bInTag)                                         /* in a tag, eg. <send> */
        {
            if (!bMXP)
                count--;                                    /* not output if not MXP */
            if (c == MXP_ENDc)
                bInTag = FALSE;
        }                                                   /* end of being inside a tag */
        else if (bInEntity)                                 /* in a tag, eg. <send> */
        {
            if (!bMXP)
                count--;                                    /* not output if not MXP */
            if (c == ';')
                bInEntity = FALSE;
        }                                                   /* end of being inside a tag */
        else switch (c)
        {

            case MXP_BEGc:
                bInTag = TRUE;
                if (!bMXP)
                    count--;                                /* not output if not MXP */
                break;

            case MXP_ENDc:                                  /* shouldn't get this case */
                if (!bMXP)
                    count--;                                /* not output if not MXP */
                break;

            case MXP_AMPc:
                bInEntity = TRUE;
                if (!bMXP)
                    count--;                                /* not output if not MXP */
                break;

            default:
                if (bMXP)
                {
                    switch (c)
                    {
                        case '<':                           /* < becomes &lt; */
                        case '>':                           /* > becomes &gt; */
                            count += 3;
                            break;

                        case '&':
                            count += 4;                     /* & becomes &amp; */
                            break;

                        case '"':                           /* " becomes &quot; */
                            count += 5;
                            break;

                    }                                       /* end of inner switch */
                }                                           /* end of MXP enabled */
        }                                                   /* end of switch on character */

    }                                                       /* end of counting special characters */

    return count;
}                                                           /* end of count_mxp_tags */

void convert_mxp_tags (const int bMXP, char * dest, const char *src, int length)
{
    char c;
    const char * ps;
    char * pd;
    int bInTag = FALSE;
    int bInEntity = FALSE;

    for (ps = src, pd = dest;
        length > 0;
        ps++, length--)
    {
        c = *ps;
        if (bInTag)                                         /* in a tag, eg. <send> */
        {
            if (c == MXP_ENDc)
            {
                bInTag = FALSE;
                if (bMXP)
                    *pd++ = '>';
            }
            else if (bMXP)
                *pd++ = c;                                  /* copy tag only in MXP mode */
        }                                                   /* end of being inside a tag */
        else if (bInEntity)                                 /* in a tag, eg. <send> */
        {
            if (bMXP)
                *pd++ = c;                                  /* copy tag only in MXP mode */
            if (c == ';')
                bInEntity = FALSE;
        }                                                   /* end of being inside a tag */
        else switch (c)
        {
            case MXP_BEGc:
                bInTag = TRUE;
                if (bMXP)
                    *pd++ = '<';
                break;

            case MXP_ENDc:                                  /* shouldn't get this case */
                if (bMXP)
                    *pd++ = '>';
                break;

            case MXP_AMPc:
                bInEntity = TRUE;
                if (bMXP)
                    *pd++ = '&';
                break;

            default:
                if (bMXP)
                {
                    switch (c)
                    {
                        case '<':
                            memcpy (pd, "&lt;", 4);
                            pd += 4;
                            break;

                        case '>':
                            memcpy (pd, "&gt;", 4);
                            pd += 4;
                            break;

                        case '&':
                            memcpy (pd, "&amp;", 5);
                            pd += 5;
                            break;

                        case '"':
                            memcpy (pd, "&quot;", 6);
                            pd += 6;
                            break;

                        default:
                            *pd++ = c;
                            break;                          /* end of default */

                    }                                       /* end of inner switch */
                }
                else
                    *pd++ = c;                              /* not MXP - just copy character */
                break;

        }                                                   /* end of switch on character */

    }                                                       /* end of converting special characters */
}                                                           /* end of convert_mxp_tags */

void turn_on_mxp (DESCRIPTOR_DATA *d)
{
    d->mxp = TRUE;                                          /* turn it on now */
    if ( d->character && !IS_SET(d->character->config,CONFIG_MXP) )
        SET_BIT(d->character->config,CONFIG_MXP);

    write_to_buffer( d, start_mxp_str, 0 );
    write_to_buffer( d, MXPMODE (1), 0 );                   /* permanent secure mode */
    write_to_buffer( d, MXPTAG(d,"!-- Set up MXP elements --"), 0);
    /* Exit tag */
    write_to_buffer( d, MXPTAG(d,"!ELEMENT Ex '<send>'"), 0);
    /* Building tag */

    write_to_buffer( d, MXPTAG
    /*      (d,"!ELEMENT Bl \"<send href='blast &x; &y;' "
           "hint='&name;   Owner: &owner;    &x; &y;' prompt>\" "
           "ATT='x y owner name'"),
    */
        (d,"!ELEMENT Bl \"<send 'Blast &x; &y;|computer connect &x; &y;|help &name;|Info &x; &y;' hint='&name;   Owner: &owner;   &x;   &y;|Blast|Connect|Help|Info: &name;  Owner: &owner;  Loc: &x; &y;'>\" ATT='x y owner name' "),

        0);
    /* Player tag (for who lists, tells etc.) */
    write_to_buffer( d, MXPTAG
        (d,"!ELEMENT Player \"<send href='tell &#39;&name;&#39; ' "
        "hint='Send a message to &name;' prompt>\" "
        "ATT='name'"),
        0);
}                                                           /* end of turn_on_mxp */
