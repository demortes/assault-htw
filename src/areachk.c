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

/*
 *  This program goes through each area. It checks all vnums to see if
 *  they are in the allowed range. If not it moves them, and updates all
 *  references.
 */
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
#include "hash.h"

void swap_global_hash(char Tp, void * Ptr, int old_vnum, int new_vnum)
{
    ROOM_INDEX_DATA * pRoomIndex,* prevRoomIndex;
    OBJ_INDEX_DATA  * pObjIndex ,* prevObjIndex;

    extern ROOM_INDEX_DATA * room_index_hash[];
    extern OBJ_INDEX_DATA  * obj_index_hash[];

    int iHash;

    switch (Tp)
    {
        case 'R':
            /* Delete old hash table entry */

            iHash   = old_vnum % MAX_KEY_HASH;
            prevRoomIndex = NULL;
            for (pRoomIndex = room_index_hash[iHash]; pRoomIndex != NULL; pRoomIndex=pRoomIndex->next)
            {
                if (pRoomIndex == (ROOM_INDEX_DATA *) Ptr)
                    break;
                prevRoomIndex = pRoomIndex;
            }

            if (pRoomIndex != NULL)
            {
                if (prevRoomIndex == NULL)
                    room_index_hash[iHash]=pRoomIndex->next;
                else
                    prevRoomIndex->next=pRoomIndex->next;
            }

            /* Add another */

            iHash                   = new_vnum % MAX_KEY_HASH;
            pRoomIndex->next        = room_index_hash[iHash];
            room_index_hash[iHash]  = pRoomIndex;

            break;

        case 'O':
            /* Delete old hash table entry */

            iHash   = old_vnum % MAX_KEY_HASH;
            prevObjIndex = NULL;
            for (pObjIndex = obj_index_hash[iHash]; pObjIndex != NULL; pObjIndex=pObjIndex->next)
            {
                if (pObjIndex == (OBJ_INDEX_DATA *) Ptr)
                    break;
                prevObjIndex = pObjIndex;
            }

            if (pObjIndex != NULL)
            {
                if (prevObjIndex == NULL)
                    obj_index_hash[iHash]=pObjIndex->next;
                else
                    prevObjIndex->next=pObjIndex->next;
            }

            /* Add another */

            iHash                   = new_vnum % MAX_KEY_HASH;
            pObjIndex->next        = obj_index_hash[iHash];
            obj_index_hash[iHash]  = pObjIndex;

            break;

    }
    return;

}

void do_check_areas(CHAR_DATA * ch, char * argument)
{
    hash_table * room_hash;
    hash_table * obj_hash;
    hash_table * mob_hash;
    int          min_vnum, max_vnum;
    int          new_vnum,old_vnum;
    char          buffer[MAX_INPUT_LENGTH];
    FILE       *  out_file;

    AREA_DATA * CurArea;
    BUILD_DATA_LIST * pList;
    ROOM_INDEX_DATA * pRoomIndex;
    OBJ_INDEX_DATA  * pObjIndex;

    /* Create hash tables for rooms, mobiles, objects */

    room_hash=create_hash_table(MAX_KEY_HASH);
    obj_hash=create_hash_table(MAX_KEY_HASH);
    mob_hash=create_hash_table(MAX_KEY_HASH);

    out_file=fopen("area_changes.txt","a");

    for (CurArea=first_area; CurArea != NULL; CurArea=CurArea->next)
    {
        min_vnum=CurArea->min_vnum;
        max_vnum=CurArea->max_vnum;
        fprintf(out_file,"%s:\n",CurArea->name);

        /* Go through rooms */
        for (pList=CurArea->first_area_room; pList != NULL; pList=pList->next)
        {
            pRoomIndex=pList->data;
            if (pRoomIndex->vnum < min_vnum || pRoomIndex->vnum > max_vnum)
            {
                old_vnum=pRoomIndex->vnum;
                /* Find a free slot */
                for (new_vnum=min_vnum; new_vnum <= max_vnum; new_vnum++)
                    if (get_room_index(new_vnum)==NULL)
                        break;

                if (new_vnum > max_vnum)
                {
                    sprintf(buffer,"Not enough vnums in area %s\n\r",CurArea->name);
                    send_to_char(buffer,ch);
                }
                else
                {
                    fprintf(out_file,"Room: [%5i] -> [%5i]\n",old_vnum,new_vnum);
                    /* Delete from room hashing table, and put new vnum in. */
                    add_hash_entry(room_hash,old_vnum,(void *) new_vnum);
                    swap_global_hash('R',pRoomIndex,old_vnum,new_vnum);
                    pRoomIndex->vnum=new_vnum;
                    area_modified(CurArea);

                }
            }
        }

        /* Go through objs */
        for (pList=CurArea->first_area_object; pList != NULL; pList=pList->next)
        {
            pObjIndex=pList->data;
            if (pObjIndex->vnum < min_vnum || pObjIndex->vnum > max_vnum)
            {
                old_vnum=pObjIndex->vnum;
                /* Find a free slot */
                for (new_vnum=min_vnum; new_vnum <= max_vnum; new_vnum++)
                    if (get_obj_index(new_vnum)==NULL)
                        break;

                if (new_vnum > max_vnum)
                {
                    sprintf(buffer,"Not enough vnums in area %s\n\r",CurArea->name);
                    send_to_char(buffer,ch);
                }
                else
                {
                    fprintf(out_file,"Obj: [%5i] -> [%5i] %s\n",old_vnum,new_vnum,pObjIndex->short_descr);
                    /* Delete from obj hashing table, and put new vnum in. */
                    add_hash_entry(obj_hash,old_vnum,(void *) new_vnum);
                    swap_global_hash('O',pObjIndex,old_vnum,new_vnum);
                    pObjIndex->vnum=new_vnum;
                    area_modified(CurArea);

                }
            }
        }
    }

    fclose(out_file);
    return;
}

void do_check_area(CHAR_DATA * ch, char * argument)
{
    send_to_char("You must say check_areas in full.\n\r",ch);
    return;
}
