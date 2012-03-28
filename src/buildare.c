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

/* This file deals with adding/deleting and manipulating areas
   as a whole, also checking on permissions and deals with area bank. */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ack.h"

/* Variables declared in db.c, which we need */

extern OBJ_INDEX_DATA *        obj_index_hash          [MAX_KEY_HASH];
extern ROOM_INDEX_DATA *       room_index_hash         [MAX_KEY_HASH];
extern char *                  string_hash             [MAX_KEY_HASH];

extern char *                  string_space;
extern char *                  top_string;
extern char                    str_empty       [1];

extern AREA_DATA *             area_used[MAX_AREAS];

extern int                     top_ed;
extern int                     top_help;
extern int                     top_obj_index;
extern int                     top_room;

#define                 MAX_STRING      1048576
#define                 MAX_PERM_BLOCK  131072
extern int                     nAllocString;
extern int                     sAllocString;
extern int                     nAllocPerm;
extern int                     sAllocPerm;
extern int                     fBootDb;

/* Some build.c functions : */
void  build_strdup(char * * dest,char * src,bool freesrc,CHAR_DATA * ch);
char * build_simpstrdup(char *);
ROOM_INDEX_DATA *  new_room( AREA_DATA * pArea, sh_int vnum, sh_int sector );

int build_canread(AREA_DATA * Area,CHAR_DATA * ch,int showerror)
{
    if (get_trust(ch) >= MAX_LEVEL)
        return 1;

    if (Area->can_read != NULL)
        if (  is_name("all", Area->can_read)
        || is_name(ch->name, Area->can_read)
        || (is_name("gods", Area->can_read) && IS_IMMORTAL(ch))
        )
            return 1;

    if (showerror==AREA_SHOWERROR)
        send_to_char("You are not allowed to use this area.\n\r",ch);

    return 0;
}

int build_canwrite(AREA_DATA * Area,CHAR_DATA * ch,int showerror)
{
    if (get_trust(ch) >= MAX_LEVEL)
        return 1;

    if (Area->can_write != NULL)
        if (  is_name("all", Area->can_write)
        || is_name(ch->name, Area->can_write)
        || (is_name("gods", Area->can_write) && IS_IMMORTAL(ch))
        )
            return 1;

    if (showerror==AREA_SHOWERROR)
        send_to_char("You are not allowed to edit this area.\n\r",ch);

    return 0;
}

void build_save_area_list(void)
{
    AREA_DATA * pArea;
    FILE * fpArea;

    fpArea=fopen("area.lst.new","w");

    if (fpArea==NULL)
    {
        bug("Could not open area.lst.new for saving.",0);
        return;
    }

    for (pArea=first_area; pArea != NULL; pArea = pArea->next)
    {
        fprintf(fpArea,"%s\n\r",pArea->filename);
    }

    fprintf(fpArea,"$\n\r");

    fclose(fpArea);

    /* Save backup */
    rename("area.lst","area.lst.old");
    rename("area.lst.new","area.lst");
}

void build_save_area_gold(void)
{
    AREA_DATA * pArea;
    FILE * fpArea;

    fpArea=fopen("area.gld.new","w");

    if (fpArea==NULL)
    {
        bug("Could not open area.gld.new for saving.",0);
        return;
    }

    for (pArea=first_area; pArea != NULL; pArea = pArea->next)
    {
        fprintf(fpArea,"%i\n\r",pArea->area_num);
    }

    fprintf(fpArea,"-1\n\r");

    fclose(fpArea);

    /* Save backup */
    rename("area.gld","area.gld.old");
    rename("area.gld.new","area.gld");
}

void build_setarea( CHAR_DATA *ch, char * argument)
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char name [MAX_INPUT_LENGTH];
    char buffer[MAX_INPUT_LENGTH];
    char buf2 [MAX_STRING_LENGTH];
    char * argn,* oldperm;
    AREA_DATA * pArea;
    int num;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );

    if (  ( str_cmp(arg1,"title") ) )
        argument = one_argument( argument, arg2 );
    else
        strcpy(arg2,argument);

    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char( "Syntax: setarea <arguments> \n\r", ch );
        send_to_char( "\n\r",                             ch );
        send_to_char( "Arguments being one of:\n\r",      ch );
        send_to_char( "      owner       <name>  \n\r",      ch );
        send_to_char( "      read        [-]<name>  \n\r",      ch );
        send_to_char( "      write       [-]<name>  \n\r",      ch );
        send_to_char( "      title       <string>\n\r",      ch );
        send_to_char( "	     keyword     <keyword> \n\r",    ch);
        send_to_char( "      offset  <vnum>\n\r",     ch );
        return;
    }

    pArea=ch->in_room->area;

    area_modified(pArea);

    /*
     * Set something.
     */
    if ( !str_cmp( arg1, "read" ) )
    {
        num=1;
        argn=arg2;
        if ( argn[0]=='-' )
        {
            num=0;
            argn++;
        }

        if (num==1)
        {
            if ( pArea->can_read==NULL)
            {
                pArea->can_read=str_dup(argn);
                return;
            }
            if (! is_name(argn, pArea->can_read))
            {
                sprintf(buffer,"%s %s",pArea->can_read,argn);
                free_string(pArea->can_read);
                pArea->can_read=str_dup(buffer);
            }
        }
        else
        {

            if (is_name(argn, pArea->can_read))
            {
                buffer[0]='\0';

                strcpy(buf2,pArea->can_read);
                oldperm=buf2;

                oldperm=one_argument(oldperm,name);
                while (name[0]!='\0')
                {
                    if ( str_cmp(name,argn) )               /* i.e. not the same */
                    {
                        safe_strcat( MSL, buffer,name);
                        safe_strcat( MSL, buffer," ");
                    }
                    oldperm=one_argument(oldperm,name);
                }
                free_string(pArea->can_read);
                pArea->can_read=str_dup(buffer);
            }
        }
        return;
    }

    if ( !str_cmp( arg1, "write" ) )
    {
        num=1;
        argn=arg2;
        if ( argn[0]=='-' )
        {
            num=0;
            argn++;
        }

        if (num==1)
        {
            if ( pArea->can_write==NULL)
            {
                pArea->can_write=str_dup(argn);
                return;
            }
            if (! is_name(argn, pArea->can_write))
            {
                sprintf(buffer,"%s %s",pArea->can_write,argn);
                free_string(pArea->can_write);
                pArea->can_write=str_dup(buffer);
            }
        }
        else
        if (is_name(argn, pArea->can_write))
        {
            buffer[0]='\0';

            strcpy(buf2,pArea->can_write);
            oldperm=buf2;

            oldperm=one_argument(oldperm,name);
            while (name[0]!='\0')
            {
                if ( str_cmp(name,argn) )                   /* i.e. not the same */
                {
                    safe_strcat( MSL, buffer,name);
                    safe_strcat( MSL, buffer," ");
                }
                oldperm=one_argument(oldperm,name);
            }
            free_string(pArea->can_write);
            pArea->can_write=str_dup(buffer);
        }
        return;
    }

    if ( !str_cmp( arg1, "owner" ) )
    {
        build_strdup(&pArea->owner, arg2, TRUE, ch);
        pArea->can_read     = str_dup(arg2);
        pArea->can_write     = str_dup(arg2);
        return;
    }

    if ( !str_cmp( arg1, "title"))
    {
        build_strdup(&pArea->name,argument,TRUE,ch);
        return;
    }
    if ( !str_cmp( arg1, "keyword"))
    {
        build_strdup(&pArea->keyword,arg2,TRUE,ch);
        return;
    }

    if ( !str_cmp( arg1, "offset"))
    {
        if ( is_number(arg2))
        {
            pArea->offset=atoi(arg2);
            return;
        }
        return;
    }

    /*
     * Generate usage message.
     */
    build_setarea( ch, "" );
    return;
}

void build_findarea( CHAR_DATA *ch, char * argument)
{
    /*    extern int top_room_index; Unused Var */
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    AREA_DATA * pArea;
    ROOM_INDEX_DATA * pRoomIndex = NULL;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Find what area?\n\r", ch );
        return;
    }

    buf1[0] = '\0';
    fAll        = !str_cmp( arg, "all" );
    found       = FALSE;
    nMatch      = 0;

    for ( pArea=first_area; pArea != NULL; pArea=pArea->next )
    {
        nMatch++;
        if (  ( fAll || is_name( arg, pArea->name ) )
            && build_canread(pArea,ch,0) )
        {
            found = TRUE;
            if (pArea->first_area_room != NULL)
                pRoomIndex=pArea->first_area_room->data;
            sprintf( buf, "[%5d] %s\n\r",
                pArea->first_area_room !=NULL ? pRoomIndex->vnum : 0,
                pArea->name );
            safe_strcat( MSL, buf1, buf );
        }
    }

    if ( !found )
    {
        send_to_char( "No area like that.\n\r", ch );
        return;
    }

    send_to_char( buf1, ch );
    return;
}

void build_showarea( CHAR_DATA *ch, char * argument)
{
    AREA_DATA * pArea;
    char buf[MAX_STRING_LENGTH];
    char buffer[MAX_INPUT_LENGTH];

    pArea=ch->in_room->area;

    if (! build_canread(pArea,ch,1))
        return;

    buf[0]='\0';

    sprintf(buffer,"\n\rTitle: %s\n\r",pArea->name);
    safe_strcat( MSL, buf,buffer);
    sprintf(buffer,"Keyword: %s\n\r",pArea->keyword);
    safe_strcat( MSL, buf,buffer);

    if (get_trust(ch) >= MAX_LEVEL-1)
    {
        sprintf(buffer,"filename: %s\n\r",pArea->filename);
        safe_strcat( MSL, buf,buffer);
    }

    sprintf( buffer, "OFFSET: %d\n\r", pArea->offset );
    safe_strcat( MSL, buf, buffer );

    sprintf(buffer,"Owner: %s\n\rCan Read: %s\n\rCan Write: %s\n\r",
        pArea->owner,
        pArea->can_read,
        pArea->can_write);
    safe_strcat( MSL, buf,buffer);

    sprintf(buffer,"Min Vnum: %5d    Max Vnum: %5d\n\r",
        pArea->min_vnum, pArea->max_vnum );
    safe_strcat( MSL, buf,buffer);

    send_to_char(buf,ch);
    return;
}

void build_arealist( CHAR_DATA *ch, char *argument )
{
    /* -S- : Show list of areas, vnum range and owners. */
    char buf[MAX_STRING_LENGTH];
    char msg[MAX_STRING_LENGTH];
    AREA_DATA *pArea;
    sh_int   stop_counter = 0;

    buf[0] = '\0';
    sprintf( msg, "%s", "Areas of " mudnamecolor ":\n\r" );
    for ( pArea = first_area; pArea != NULL; pArea = pArea->next )
    {
        sprintf( buf, "%12s [%5d to %5d] %s [%s]\n\r", capitalize( pArea->owner ),
            pArea->min_vnum, pArea->max_vnum, pArea->name, pArea->filename );
        stop_counter++;
        if (stop_counter > 40 )
        {
            safe_strcat( MSL, msg, buf );
            stop_counter = 0;
            send_to_char( msg, ch );
            msg[0] = '\0';
        }
        else
        {
            safe_strcat( MAX_STRING_LENGTH, msg, buf );
        }
    }

    send_to_char( msg, ch );
    return;
}
