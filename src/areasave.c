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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ack.h"

/* Way this works:
    Mud reads in area files, stores details in data lists.
    Edit rooms, objects, resets.
    type savearea.
    Sets bool saving_area to true.
    Incrementally saves an area, using data lists.
*/

#define SAVEQUEUESIZE 50
#define NOT_SAVING 0
#define START_SAVING 1
#define AM_SAVING 2
#define BUILD_OK -1
#define BUILD_CANTSAVE 1
#define BUILD_TOOMANY  2

#define BUILD_SEC_AREA     1
#define BUILD_SEC_HELP     2
#define BUILD_SEC_ROOMS    3
#define BUILD_SEC_OBJECTS  4
#define BUILD_SEC_END      5
#define AREA_VERSION  20

struct save_queue_type
{
    AREA_DATA * area;
    CHAR_DATA * ch;
    int         loops;
} SaveQ[SAVEQUEUESIZE];

/* Semi-local vars. */
int saving_area=0;

/* local */
int         offset;
int             ToBeSaved=0;
int             CurrentSaving=-1;
AREA_DATA     * CurSaveArea=NULL;
CHAR_DATA     * CurSaveChar=NULL;
int             CurLoops=1;
int             Section;
BUILD_DATA_LIST * Pointer;
FILE          * SaveFile;
FILE          * Envy;
int             AreasModified=0;

/* Local functions */
/* void build_save(); proto in merc.h */
void build_save_area(void);
void build_save_help(void);
void build_save_mobs(void);
void build_save_mobprogs(void);
void build_save_objects(void);
void build_save_rooms(void);
void build_save_resets(void);
void build_save_objfuns(void);
void build_save_end(void);
char * mprog_type_to_name(int);
void vuild_save_flush(void);
/*  int convert(int lev); */
/* Convert levels from ack -> envy! */

/* int  convert(int lev)	*/
/*  { 		*/
/*   return( lev - ( lev/5 ) ); 	*/
/* }   */

void do_savearea( CHAR_DATA *ch, char *argument )
{
    AREA_DATA * SaveArea;
    int loops;
    /*     char first_arg[MAX_INPUT_LENGTH]; unused? */

    if (ch==NULL)
    {
        SaveArea=(AREA_DATA *) argument;
        loops=10;
    }
    else
    {
        if (ch->in_room==NULL)
        {
            send_to_char("Do not know what room you are in!!, cannot save.\n",ch);
            return;
        }

        SaveArea=(ch->in_room)->area;
        if (SaveArea==NULL)
        {
            send_to_char("Do not know what area you are in!!, cannot save.\n",ch);
            return;
        }

        if (*argument != '\0')
        {
            loops=atoi(argument);
            if (loops<1)
                loops=1;
        }
        else
            loops=1;
    }

    if (ToBeSaved==CurrentSaving)
    {
        send_to_char("Too many areas in queue, please try later.\n",ch);
        return;
    }

    SaveQ[ToBeSaved].area=SaveArea;
    SaveQ[ToBeSaved].ch=ch;
    SaveQ[ToBeSaved].loops=loops;
    ToBeSaved=(ToBeSaved + 1) % SAVEQUEUESIZE;

    if (saving_area==NOT_SAVING)
        saving_area=START_SAVING;
    else
        send_to_char("Save is queued, please wait. \n",ch);

    build_save();
    return;
}

void build_save()
{
    int a;
    char filename[255];
    char buf[MAX_STRING_LENGTH];

    for (a=0;a<CurLoops && saving_area>0;a++)
    {

        if ( saving_area== START_SAVING)
        {
            CurrentSaving=(CurrentSaving+1) % SAVEQUEUESIZE;
            CurSaveArea=SaveQ[CurrentSaving].area;
            CurSaveChar=SaveQ[CurrentSaving].ch;
            CurLoops=SaveQ[CurrentSaving].loops;
            send_to_char("Starting Save.\n",CurSaveChar);

            sprintf(filename,"%s.new",CurSaveArea->filename);
            SaveFile=fopen(filename,"w");
            if (SaveFile==NULL)
            {
                if (CurrentSaving==ToBeSaved)
                    saving_area=NOT_SAVING;
                send_to_char("Can not open file for saving.\n",CurSaveChar);
                return;
            }
            /* Open second file for saving in envy format */

            sprintf( buf, "Starting to save %s", CurSaveArea->filename );
            monitor_chan( NULL, buf, MONITOR_AREA_SAVING );

            Section=1;
            offset=CurSaveArea->offset;
            saving_area=AM_SAVING;
            Pointer=NULL;
        }

        switch (Section)
        {
            case BUILD_SEC_AREA:     build_save_area();     break;
            case BUILD_SEC_HELP:     build_save_help();     break;
            case BUILD_SEC_ROOMS:    build_save_rooms();    break;
            case BUILD_SEC_OBJECTS:  build_save_objects();  break;
            case BUILD_SEC_END:      build_save_end();      break;
        }
    }
    return;
}

void build_save_area()
{
    fprintf(SaveFile,"#AREA\n");
    fprintf(SaveFile,"%s~\n",CurSaveArea->name);
    fprintf( SaveFile,"Q %i\n", AREA_VERSION );
    fprintf(SaveFile,"K %s~\n",CurSaveArea->keyword);
    fprintf(SaveFile,"N %i\n",CurSaveArea->area_num);
    fprintf(SaveFile,"V %i %i\n",CurSaveArea->min_vnum,CurSaveArea->max_vnum);
    fprintf(SaveFile,"X %i\n", CurSaveArea->offset );
    if (CurSaveArea->owner != NULL)
        fprintf(SaveFile,"O %s~\n",CurSaveArea->owner);
    if (CurSaveArea->can_read != NULL)
        fprintf(SaveFile,"R %s~\n",CurSaveArea->can_read);
    if (CurSaveArea->can_write != NULL)
        fprintf(SaveFile,"W %s~\n",CurSaveArea->can_write);

    /*     fprintf( Envy, "#AREA\n" );                      remove save bug */
    /*     fprintf( Envy, "%s~\n", CurSaveArea->name );                     */

    Section++;
}

void build_save_help()
{
    HELP_DATA *pHelp;

    if (Pointer==NULL)                                      /* Start */
    {
        if (CurSaveArea->first_area_help_text==NULL)
        {
            Section++;
            return;
        }
        send_to_char("Saving help section.\n",CurSaveChar);
        fprintf(SaveFile,"#HELPS\n");
        Pointer=CurSaveArea->first_area_help_text;
    }

    pHelp=Pointer->data;
    fprintf(SaveFile,"%i %s~\n",pHelp->level,pHelp->keyword);
    if (isspace(pHelp->text[0]))
        fprintf(SaveFile,".%s~\n",pHelp->text);
    else
        fprintf(SaveFile,"%s~\n",pHelp->text);

    Pointer=Pointer->next;
    if (Pointer==NULL)                                      /* End */
    {
        fprintf(SaveFile,"0 $~\n");
        Section++;
    }
    /* No saving helps for envy format */

    return;
}

void build_save_objects()
{
    OBJ_INDEX_DATA *pObject;
    //     int val0,val1,val2,val3;
    int i;

    if (Pointer==NULL)                                      /* Start */
    {
        if (CurSaveArea->first_area_object==NULL)
        {
            Section++;
            return;
        }
        send_to_char("Saving objects.\n",CurSaveChar);
        fprintf(SaveFile,"#OBJECTS\n");
        Pointer=CurSaveArea->first_area_object;
    }

    pObject=Pointer->data;

    fprintf(SaveFile,"#%i\n",pObject->vnum);
    fprintf(SaveFile,"%s~\n",pObject->name);
    fprintf(SaveFile,"%s~\n",pObject->short_descr);
    fprintf(SaveFile,"%s~\n",pObject->description);
    fprintf(SaveFile,"%i %i %i\n",pObject->item_type,pObject->extra_flags,
        pObject->wear_flags);

    for ( i=0;i<MAX_OBJECT_VALUES;i++ )
        fprintf(SaveFile,"%i ", pObject->value[i]);
    /*     fprintf(SaveFile,"%i %i %i %i %i %i %i %i %i %i\n",val0,val1,val2,val3,
         pObject->value[4], pObject->value[5],pObject->value[6],pObject->value[7],
         pObject->value[8],pObject->value[9]); */
    fprintf(SaveFile,"%i %i %i\n",pObject->weight,pObject->heat,pObject->building);
    fprintf(SaveFile,"%s~\n",pObject->image);

    if ( ( pObject->level > 1 ) && (pObject->level < 130 ) )
    {
        fprintf(SaveFile, "L\n");
        fprintf(SaveFile,"%d\n",pObject->level);
    }
    else
    {
        fprintf(SaveFile, "L\n");
        fprintf(SaveFile,"%d\n", 1);
    }

    /* Now for Envy... taken from my OLC :P */

    Pointer=Pointer->next;
    if (Pointer==NULL)                                      /* End */
    {
        fprintf(SaveFile,"#0\n");
        Section++;
    }

    return;
}

void build_save_rooms()
{
    ROOM_INDEX_DATA *pRoomIndex;

    if (Pointer==NULL)                                      /* Start */
    {
        if (CurSaveArea->first_area_room==NULL)
        {
            Section++;
            return;
        }
        send_to_char("Saving rooms.\n",CurSaveChar);
        fprintf(SaveFile,"#ROOMS\n");
        Pointer=CurSaveArea->first_area_room;
    }

    pRoomIndex=Pointer->data;

    fprintf(SaveFile,"#%i\n",pRoomIndex->vnum);

    /* End of one room */
    fprintf(SaveFile,"S\n");

    Pointer=Pointer->next;
    if (Pointer==NULL)                                      /* End */
    {
        fprintf(SaveFile,"#0\n");
        Section++;
    }
    return;
}

void build_save_end()
{
    char filename[255];
    char buf[MAX_STRING_LENGTH];

    sprintf( buf, "Finished saving %s", CurSaveArea->filename );
    monitor_chan( NULL, buf, MONITOR_AREA_SAVING );

    fprintf(SaveFile,"#$\n");
    send_to_char("Finished saving.\n",CurSaveChar);
    fclose(SaveFile);
    /* Save backup */
    sprintf(filename,"%s.old",CurSaveArea->filename);
    rename(CurSaveArea->filename,filename);
    /* And rename .new to area filename */
    sprintf(filename,"%s.new",CurSaveArea->filename);
    rename(filename,CurSaveArea->filename);

    Section=0;
    if (ToBeSaved==(CurrentSaving+1) % SAVEQUEUESIZE)
        saving_area=NOT_SAVING;
    else
        saving_area=START_SAVING;
}

void build_save_flush()
{
    AREA_DATA * pArea;

    if (AreasModified == 0)
        return;

    for (pArea=first_area; pArea != NULL; pArea=pArea->next)
    {
        if (pArea->modified)
        {
            pArea->modified=0;
            do_savearea(NULL,(char *) pArea);
        }
    }

    AreasModified=0;
}

void area_modified(AREA_DATA * pArea)
{
    pArea->modified=1;
    AreasModified=1;
}
