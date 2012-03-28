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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "ack.h"

const char wizutil_id [] = "";

/*
===========================================================================
This snippet was written by Erwin S. Andreasen, 4u2@aabc.dk. You may use
this code freely, as long as you retain my name in all of the files. You
also have to mail me telling that you are using it. I am giving this,
hopefully useful, piece of source code to you for free, and all I require
from you is some feedback.

Please mail me if you find any bugs or have any new ideas or just comments.

All my snippets are publically available at:

http://pip.dknet.dk/~pip1773/

If you do not have WWW access, try ftp'ing to pip.dknet.dk and examine
the /pub/pip1773 directory.
===========================================================================

  Various administrative utility commands.
  Version: 3 - Last update: January 1996.

  To use these 2 commands you will have to add a filename field to AREA_DATA.
  This value can be found easily in load_area while booting - the filename
  of the current area boot_db is reading from is in the strArea global.

  Since version 2 following was added:

  A rename command which renames a player. Search for do_rename to see
  more info on it.

  A FOR command which executes a command at/on every player/mob/location.

  Fixes since last release: None.

*/

/*
 * do_rename renames a player to another name.
 * PCs only. Previous file is deleted, if it exists.
 * Char is then saved to new file.
 * New name is checked against std. checks, existing offline players and
 * online players.
 * .gz files are checked for too, just in case.
 */

bool check_parse_name (char* name);                         /* comm.c */
char *initial( const char *str );                           /* comm.c */

void do_rename (CHAR_DATA* ch, char* argument)
{
    char old_name[MAX_INPUT_LENGTH],
        new_name[MAX_INPUT_LENGTH],
        strsave [MAX_INPUT_LENGTH];

    CHAR_DATA* victim;
    FILE* file;

    argument = one_argument(argument, old_name);            /* find new/old name */
    one_argument (argument, new_name);

    /* Trivial checks */
    if (!old_name[0])
    {
        send_to_char ("Rename who?\n\r",ch);
        return;
    }

    victim = get_char_world (ch, old_name);

    if (!victim)
    {
        send_to_char ("There is no such a person online.\n\r",ch);
        return;
    }

    if (IS_NPC(victim))
    {
        send_to_char ("You cannot use Rename on NPCs.\n\r",ch);
        return;
    }

    /* allow rename self new_name,but otherwise only lower level */
    if ( (victim != ch) && (get_trust (victim) >= get_trust (ch)) )
    {
        send_to_char ("You failed.\n\r",ch);
        return;
    }

    if (!victim->desc || (victim->desc->connected != CON_PLAYING) )
    {
        send_to_char ("This player has lost his link or is inside a pager or the like.\n\r",ch);
        return;
    }

    if (!new_name[0])
    {
        send_to_char ("Rename to what new name?\n\r",ch);
        return;
    }

    if (!check_parse_name(new_name))
    {
        send_to_char ("The new name is illegal.\n\r",ch);
        return;
    }

    /* First, check if there is a player named that off-line */
    #if !defined(machintosh) && !defined(MSDOS)
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( new_name ),
        "/", capitalize( new_name ) );
    #else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( new_name ) );
    #endif

    fclose (fpReserve);                                     /* close the reserve file */
    file = fopen (strsave, "r");                            /* attempt to to open pfile */
    if (file)
    {
        send_to_char ("A player with that name already exists!\n\r",ch);
        fclose (file);
        fpReserve = fopen( NULL_FILE, "r" );                /* is this really necessary these days? */
        return;
    }
    fpReserve = fopen( NULL_FILE, "r" );                    /* reopen the extra file */

    /* Check .gz file ! */
    #if !defined(machintosh) && !defined(MSDOS)
    sprintf( strsave, "%s%s%s%s.gz", PLAYER_DIR, initial( new_name ),
        "/", capitalize( new_name ) );
    #else
    sprintf( strsave, "%s%s.gz", PLAYER_DIR, capitalize( new_name ) );
    #endif

    fclose (fpReserve);                                     /* close the reserve file */
    file = fopen (strsave, "r");                            /* attempt to to open pfile */
    if (file)
    {
        send_to_char ("A player with that name already exists in a compressed file!\n\r",ch);
        fclose (file);
        fpReserve = fopen( NULL_FILE, "r" );
        return;
    }
    fpReserve = fopen( NULL_FILE, "r" );                    /* reopen the extra file */

                                                            /* check for playing level-1 non-saved */
    if ( get_ch(new_name) && !str_cmp(get_ch(new_name)->name,new_name))
    {
        send_to_char ("A player with the name you specified already exists!\n\r",ch);
        return;
    }

    /* Save the filename of the old name */

    #if !defined(machintosh) && !defined(MSDOS)
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( victim->name ),
        "/", capitalize( victim->name ) );
    #else
    sprintf( strsave, "%s%s", PLAYER_DIR, capitalize( victim->name )
        );
    #endif

    {
        BUILDING_DATA *bld;
        OBJ_DATA *obj;
        for (bld = first_building;bld;bld = bld->next)
        {
            if ( !str_cmp(bld->owned,victim->name) )
            {
                free_string(bld->owned);
                bld->owned = str_dup(capitalize(new_name));
                bld->owner = victim;
            }
        }
        for(obj = first_obj;obj;obj = obj->next)
        {
            if ( !str_cmp(obj->owner,victim->name) )
            {
                free_string(obj->owner);
                obj->owner = capitalize(new_name);
            }
        }
    }
    free_string (victim->name);
    victim->name = str_dup (capitalize(new_name));

    save_char_obj (victim);

    /* unlink the old file */
    unlink (strsave);                                       /* unlink does return a value.. but we do not care */

    /* That's it! */

    send_to_char ("Character renamed.\n\r",ch);

    victim->position = POS_STANDING;                        /* I am laaazy */
    act ("$n has renamed you to $N!",ch,NULL,victim,TO_VICT);

}                                                           /* do_rename */

#if 0
const char * name_expand (CHAR_DATA *ch)
{
    int count = 1;
    CHAR_DATA *rch;
    char name[MAX_INPUT_LENGTH];                            /*  HOPEFULLY no mob has a name longer than THAT */

    static char outbuf[MAX_INPUT_LENGTH];

    if (!IS_NPC(ch))
        return ch->name;

    one_argument (ch->name, name);                          /* copy the first word into name */

    if (!name[0])                                           /* weird mob .. no keywords */
    {
        strcpy (outbuf, "");                                /* Do not return NULL, just an empty buffer */
        return outbuf;
    }

    for (rch = first_char; rch && (rch != ch);rch = rch->next)
        if (is_name (name, rch->name))
            count++;

    sprintf (outbuf, "%d.%s", count, name);
    return outbuf;
}
#endif
