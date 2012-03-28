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
#include <string.h>
#include <time.h>
#include "ack.h"
#include "hash.h"

#if !defined(macintosh)
extern  int     _filbuf         args( (FILE *) );
#endif

#define SAVE_REVISION 52

char *cap_nocol( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
        strcap[i] = LOWER(str[i]);
    strcap[i] = '\0';
    strcap[0] = UPPER(strcap[0]);
    return strcap;
}

/*
 * Array of containers read for proper re-nesting of objects.
 */
#define MAX_NEST        100
static  OBJ_DATA *      rgObjNest       [MAX_NEST];

/*
 * Local functions.
 */
void    fwrite_building args( ( BUILDING_DATA *bld,  FILE *fp ) );
void    fwrite_vehicle  args( ( VEHICLE_DATA *vhc,  FILE *fp ) );
void    fwrite_char     args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fwrite_obj      args( ( CHAR_DATA *ch,  OBJ_DATA  *obj,
FILE *fp, int iNest ) );
void    fwrite_vhc      args( ( VEHICLE_DATA *vhc,  FILE *fp ) );
void    fread_char      args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_obj       args( ( CHAR_DATA *ch,  FILE *fp ) );
void    fread_object    args( ( FILE *fp ) );
VEHICLE_DATA * fread_vhc args( ( CHAR_DATA *ch, FILE *fp ) );

void abort_wrapper(void);

/* Courtesy of Yaz of 4th Realm */
char *initial( const char *str )
{
    static char strint[ MAX_STRING_LENGTH ];

    strint[0] = LOWER( str[ 0 ] );
    return strint;

}

/*
 * Save a character and inventory.
 * Would be cool to save NPC's too for quest purposes,
 *   some of the infrastructure is provided.
 */
int loop_counter;
void save_char_obj( CHAR_DATA *ch )
{
    char strsave[MAX_INPUT_LENGTH];
    char tempstrsave[MAX_INPUT_LENGTH];                     /* Hold temp filename here.. */
    char buf[MAX_INPUT_LENGTH];                             /* hold misc stuff here..*/
    extern int  loop_counter;
    FILE *fp;
    char * nmptr,*bufptr;

    //    if ( sysdata.killfest || my_get_minutes(ch,TRUE) <= 0 )
    if ( my_get_minutes(ch,TRUE) <= 0 )
    {
        //	send_to_char( "Your character cannot save until you have played for at least 5 minutes or if Killfest mode is active.\n\r", ch );
        return;
    }
    if ( ch->desc && ch->desc->connected != CON_PLAYING )
        return;

    if (ch->desc != NULL && ch->desc->original != NULL )
        ch = ch->desc->original;

    ch->save_time = current_time;
    fclose( fpReserve );

    if ( ch->pcdata->o_pagelen > 0 )
    {
        ch->pcdata->pagelen = ch->pcdata->o_pagelen;
        ch->pcdata->o_pagelen = 0;
    }
    /* player files parsed directories by Yaz 4th Realm */
    #if !defined(machintosh) && !defined(MSDOS)
    if (IS_NPC(ch))                                         /* convert spaces to . */
    {
        for (nmptr=ch->name,bufptr=buf;*nmptr != 0; nmptr++)
        {
            if (*nmptr == ' ')
                *(bufptr++)='.';
            else
                *(bufptr++)=*nmptr;
        }
        *(bufptr)=*nmptr;
    }
    else
        strcpy(buf,ch->name);
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR,
        initial( buf ), "/", cap_nocol( buf ) );
    #else
    /* Convert npc names to dos compat name.... yuk */
    if (IS_NPC(ch))
    {
        for (nmptr=ch->name,bufptr=buf;*nmptr != 0; nmptr++)
        {
            if (*nmptr != ' ' && *nmptr != '.')
                *(bufptr++)=*nmptr;
            if ( bufptr-buf == 8)
                break;
        }
        *(bufptr)=0;
    }
    else
        strcpy(buf,ch->name);

    sprintf( strsave, "%s%s", PLAYER_DIR,
        cap_nocol( buf ) );
    #endif
    /* Tack on a .temp to strsave, use as tempstrsave */

    sprintf( tempstrsave, "%s.temp", strsave );

    if ( ( fp = fopen( tempstrsave, "w" ) ) == NULL )
    {
        monitor_chan( NULL, "Save_char_obj: fopen", MONITOR_BAD );
        perror( strsave );
    }
    else
    {
        fwrite_char( ch, fp );
        loop_counter = 0;
        if ( ch->first_carry != NULL )
            fwrite_obj( ch, ch->first_carry, fp, 0 );
        if ( ch->in_vehicle )
            fwrite_vhc( ch->in_vehicle, fp );
        fprintf( fp, "#END\n" );
    }
    fflush( fp );
    fclose( fp );

    /* Now make temp file the actual pfile... */

    rename( tempstrsave, strsave );
    /* THAT easy?? */

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

/*
 * Write the char.
 */
void fwrite_char( CHAR_DATA *ch, FILE *fp )
{
    int foo;

    fprintf( fp, "#PLAYER\n\r"         );

    fprintf( fp, "Revision     %d\n",     SAVE_REVISION           );
    fprintf( fp, "Name         %s~\n",  ch->name                );
    fprintf( fp, "Prompt       %s~\n",  ch->prompt              );
    fprintf( fp, "Dead	       %d\n",   ch->pcdata->dead );
    fprintf( fp, "Deleted      %d\n",   ch->pcdata->deleted );
    fprintf( fp, "Sex          %d\n",   ch->sex                 );
    fprintf( fp, "LoginSex     %d\n",   ch->login_sex       );
    fprintf( fp, "Class        %d\n",   ch->class               );
    fprintf( fp, "Level        %d\n",   ch->level               );
    fprintf( fp, "Invis        %d\n",   ch->invis           );
    fprintf( fp, "Incog		 %d\n",   ch->incog              );
    fprintf( fp, "Trust        %d\n",   ch->trust               );
    fprintf( fp, "Wizbit       %d\n",   ch->wizbit              );
    fprintf( fp, "Played       %d\n",
        ch->played + (int) (current_time - ch->logon)           );
    fprintf( fp, "PlayedTot    %d\n",
        ch->played_tot + (int) (current_time - ch->logon)        );
    fprintf( fp, "Note         %ld\n",   ch->last_note           );
    fprintf( fp, "Room         3\n" );
    fprintf( fp, "Location         %d %d %d\n", ch->x, ch->y, ch->z );
    fprintf( fp, "Hp   		%d %d\n",
        ch->hit, ch->max_hit);
    fprintf( fp, "Act          %d\n",   ch->act                 );
    fprintf( fp, "Config       %d\n",   ch->config              );
    fprintf( fp, "Position     %d\n",
        ch->position == POS_SNEAKING ? POS_STANDING : ch->position );

    fprintf( fp, "Questpoints  %d\n",   ch->quest_points    );
    fprintf( fp, "Pkills	   %d\n",  ch->pcdata->pkills  );
    fprintf( fp, "Bkills	   %d\n",  ch->pcdata->bkills  );
    fprintf( fp, "TPkills	   %d\n", ch->pcdata->tpkills );
    fprintf( fp, "TBkills	   %d\n", ch->pcdata->tbkills );
    fprintf( fp, "Deaths	   %d\n",  ch->pcdata->deaths  );
    fprintf( fp, "BLost	   %d\n",   ch->pcdata->blost   );
    fprintf( fp, "PBHits	   %d\n",  ch->pcdata->pbhits  );
    fprintf( fp, "PBDeaths	   %d\n",    ch->pcdata->pbdeaths    );
    fprintf( fp, "NukemWins	   %d\n",   ch->pcdata->nukemwins   );
    fprintf( fp, "Medals	   %d\n",  ch->medals      );
    fprintf( fp, "GPoints	   %d\n", ch->game_points     );
    fprintf( fp, "Implants     %d\n",       ch->implants            );
    fprintf( fp, "Password     %s~\n",      ch->pcdata->pwd         );
    fprintf( fp, "Bamfin       %s~\n",      ch->pcdata->bamfin      );
    fprintf( fp, "Bamfout      %s~\n",      ch->pcdata->bamfout     );
    fprintf( fp, "Roomenter    %s~\n",    ch->pcdata->room_enter      );
    fprintf( fp, "Roomexit     %s~\n",    ch->pcdata->room_exit       );
    fprintf( fp, "Title        %s~\n",      ch->pcdata->title       );
    fprintf( fp, "Ranking      %s~\n", ch->pcdata->ranking    );
    fprintf( fp, "Whoname      W%s~\n", ch->pcdata->who_name    );
    fprintf( fp, "Monitor	   %d\n", ch->pcdata->monitor );
    fprintf( fp, "Host         %s~\n",  ch->pcdata->host    );
    fprintf( fp, "Failures     %d\n",   ch->pcdata->failures    );
    fprintf( fp, "LastLogin    %s~\n",      (char *) ctime( &current_time ) );
    fprintf( fp, "Loadmsg	   %s~\n",    ch->pcdata->load_msg    );
    fprintf( fp, "HiCol	   %c~\n",  ch->pcdata->hicol   );
    fprintf( fp, "DimCol	   %c~\n", ch->pcdata->dimcol  );
    fprintf( fp, "TermRows    %i\n", ch->pcdata->term_rows );
    fprintf( fp, "TermColumns    %i\n", ch->pcdata->term_columns );
    fprintf( fp, "Email   %s~\n", ch->pcdata->email_address );
    fprintf( fp, "EmailValid    %i\n", ch->pcdata->valid_email );
    fprintf( fp, "colors\n" );
    for ( foo = 0; foo < MAX_color; foo++ )
        fprintf( fp, "%d\n", ch->pcdata->color[foo] );

    fprintf( fp, "GainHp        %d\n", ch->pcdata->hp_from_gain   );

    fprintf( fp, "Pagelen      %d\n",   ch->pcdata->pagelen     );
    fprintf( fp, "Pflags       %d\n",   ch->pcdata->pflags      );
    fprintf( fp, "Map	   %d\n",   ch->map               );
    fprintf( fp, "Disease	   %d\n",   ch->disease       );
    fprintf( fp, "Effect	   %d\n",   ch->effect         );
    fprintf( fp, "Alliance     %d\n",   ch->pcdata->alliance    );
    fprintf( fp, "Suicided	   %d\n",   ch->suicide          );
    fprintf( fp, "MedalTimer   %d\n",  ch->medaltimer );
    fprintf( fp, "FightTimer   %d\n",  ch->fighttimer );
    fprintf( fp, "Reimb        %d\n",  ch->pcdata->reimb );
    fprintf( fp, "ProfPt       %d %d\n",  ch->pcdata->prof_points, ch->pcdata->prof_ttl );
    fprintf( fp, "Spec         %d\n",  ch->pcdata->spec_init );
    fprintf( fp, "Setexit      %d\n",  ch->pcdata->set_exit     );
    fprintf( fp, "Experience   %d\n",  ch->pcdata->experience );
    //	fprintf( fp, "Built" );
    //	for ( foo = 0;foo < MAX_POSSIBLE_BUILDING;foo++ )
    //		fprintf( fp, "   %d", ch->pcdata->built[foo] );
    //	fprintf( fp, " -1\nRefund" );
    fprintf( fp, " \nRefund" );
    for ( foo = 0;foo < 8;foo++ )
        fprintf( fp, "   %d", ch->refund[foo] );
    fprintf( fp, "\nSkill" );
    for ( foo = 0;foo < MAX_SKILL;foo++ )
        fprintf( fp, "   %d", ch->pcdata->skill[foo] );
    fprintf( fp, "   -1\n" );
    for ( foo = 0;foo < 5;foo++ )
        fprintf( fp, "Alias    %s~  %s~\n", ch->alias[foo], ch->alias_command[foo] );
    if ( ch->pcdata )
    {
        PAGER_DATA *p;
        for ( p=ch->pcdata->pager;p;p=p->next )
            fprintf(fp, "Pager   %s~ %s~ %s~\n", p->from,p->time,p->msg );
    }
    //if (IMC)
    //    imc_savechar( ch, fp );
    fprintf( fp, "End\n\n" );
    return;
}

/*
 * Write an object and its contents.
 */
void fwrite_obj( CHAR_DATA *ch, OBJ_DATA *obj, FILE *fp, int iNest )
{
    loop_counter++;
    int i;
    if ( loop_counter > 650 )
    {
        return;
        abort_wrapper();
    }
    if ( obj->next_in_carry_list != NULL && iNest != -1 )
        fwrite_obj( ch, obj->next_in_carry_list, fp, iNest );

    if ( obj->pIndexData == NULL )
        return;
    /*
     * Castrate storage characters.
     */

    fprintf( fp, "#OBJECT\n" );
    fprintf( fp, "Nest         %d\n",   iNest                        );
    fprintf( fp, "Name         %s~\n",  obj->name                    );
    fprintf( fp, "ShortDescr   %s~\n",  obj->short_descr             );
    fprintf( fp, "Description  %s~\n",  obj->description             );
    fprintf( fp, "Vnum         %d\n",   obj->pIndexData->vnum        );
    fprintf( fp, "ExtraFlags   %d\n",   obj->extra_flags             );
    fprintf( fp, "WearFlags    %d\n",   obj->wear_flags              );
    fprintf( fp, "WearLoc      %d\n",   obj->wear_loc                );
    fprintf( fp, "ItemType     %d\n",   obj->item_type               );
    fprintf( fp, "Weight       %d\n",   obj->weight                  );
    fprintf( fp, "Heat         %d\n",   obj->heat                    );
    fprintf( fp, "Level        %d\n",   obj->level                   );
    fprintf( fp, "Values      " );
    for ( i=0;i<MAX_OBJECT_VALUES;i++ )
        fprintf( fp, " %d", obj->value[i] );
    fprintf( fp, "\n" );
    fprintf( fp, "Quest       %d %d\n", obj->quest_timer, obj->quest_map );

    fprintf( fp, "End\n\n" );

    if ( obj->first_in_carry_list != NULL && iNest != -1 )
        fwrite_obj( ch, obj->first_in_carry_list, fp, iNest + 1 );

    return;
}

/* so the stack doesn't get hosed */
void abort_wrapper(void) { abort(); };

hash_table * hash_changed_vnums=NULL;

/* Nasty hack for db.c to get back address of ch */
CHAR_DATA * loaded_mob_addr;

/*
 * Load a char and inventory into a new ch structure.
 */

int cur_revision = 0;

bool load_char_obj( DESCRIPTOR_DATA *d, char *name, bool system_call )
{
    static PC_DATA pcdata_zero;
    char strsave[MAX_INPUT_LENGTH];
    char tempstrsave[MAX_INPUT_LENGTH];
    char * bufptr,*nmptr;
    CHAR_DATA *ch;
    char buf[MAX_STRING_LENGTH];
    FILE *fp;
    bool found;
    bool is_npc;
    int oldvnum,newvnum;
    int foo;

    if (hash_changed_vnums == NULL)
    {
        /* Initialise hash table for changed vnums, and read in file. */
        hash_changed_vnums=create_hash_table(1024);

        if ((fp=fopen("area_changes.txt","r")) != NULL)     /* -- Alty */
        {
            while (!feof(fp))
            {
                if (   str_cmp( fread_word(fp), "Obj:" )
                    || fread_letter(fp) != '['
                    || (oldvnum = fread_number(fp)) == 0
                    || fread_letter(fp) != ']'
                    || str_cmp( fread_word(fp) , "->")
                    || fread_letter(fp) != '['
                    || (newvnum = fread_number(fp)) == 0
                    || fread_letter(fp) != ']' )
                    fread_to_eol(fp);
                else
                {
                    fread_to_eol(fp);
                    add_hash_entry(hash_changed_vnums,oldvnum,(void *) newvnum);
                }
            }
            fclose(fp);
        }
    }

    if (  ( d == NULL )                                     /* load npc */
        && ( !system_call )  )
        is_npc=TRUE;
    else
        is_npc=FALSE;

    GET_FREE(ch, char_free);
    clear_char( ch );

    if (!is_npc)
    {
        GET_FREE(ch->pcdata, pcd_free);
        *ch->pcdata                         = pcdata_zero;

        d->character                        = ch;

        ch->in_room = get_room_index(ROOM_VNUM_WMAP);
        //	char_to_room(ch,get_room_index(ROOM_VNUM_WMAP));
        ch->pcdata->host            = str_dup( "Unknown!" );
        ch->pcdata->lastlogin       = str_dup( "Unknown!" );
        ch->pcdata->who_name        = str_dup( "off" );
        ch->pcdata->pwd                     = str_dup( "" );
        ch->pcdata->bamfin                  = str_dup( "" );
        ch->pcdata->bamfout                 = str_dup( "" );
        ch->pcdata->room_enter         = str_dup( "" );
        ch->pcdata->room_exit          = str_dup( "" );
        ch->pcdata->title                   = str_dup( "" );
        ch->pcdata->ranking         = str_dup( "off" );
        ch->pcdata->load_msg        = str_dup( "" );
        ch->pcdata->pagelen                 = 50;
        ch->pcdata->o_pagelen      = 0;
        ch->pcdata->pbhits         = 0;
        ch->pcdata->pbdeaths       = 0;
        ch->pcdata->nukemwins      = 0;
        ch->pcdata->pkills         = 0;
        ch->pcdata->bkills         = 0;
        ch->pcdata->tpkills        = 0;
        ch->pcdata->tbkills        = 0;
        ch->pcdata->deaths         = 0;
        ch->pcdata->blost          = 0;
        ch->pcdata->pflags                 = 0;
        ch->pcdata->hp_from_gain = -1;
        ch->pcdata->hicol = 'y';
        ch->pcdata->dimcol = 'b';
        for ( foo = 0; foo < 5; foo++ )
            ch->pcdata->pedit_string[foo] = str_dup( "none" );
        ch->pcdata->pedit_state = str_dup( "none" );
        ch->pcdata->term_rows = 25;
        ch->pcdata->term_columns = 80;
        ch->pcdata->valid_email = FALSE;
        ch->pcdata->email_address = str_dup( "not set" );
        ch->quest_points           = 0;
        ch->disease = 0;
        ch->effect = 0;
        ch->spectimer = 0;
        ch->questtimer = 0;
        ch->fighttimer = 0;
        ch->killtimer = 0;
        ch->medaltimer = 0;
        ch->game_points = 0;
        ch->section = 0;
        ch->dead = FALSE;
        ch->pcdata->reimb = 0;
        ch->pcdata->prof_points = 0;
        ch->pcdata->prof_ttl = 0;
        ch->pcdata->spec_init = 2;
        ch->pcdata->set_exit = DIR_NORTH;
        ch->pcdata->queue = NULL;
        ch->pcdata->last_queue = NULL;
        ch->pcdata->pager = NULL;
        ch->pcdata->experience = 0;
        ch->pcdata->guess = 0;
        for ( foo = 0;foo < MAX_SKILL; foo++ )
            ch->pcdata->skill[foo] = 0;
        for ( foo = 0;foo < 8; foo++ )
            ch->refund[foo] = 0;
        //     for ( foo = 0;foo < MAX_POSSIBLE_BUILDING; foo++ )
        //	ch->pcdata->built[foo] = FALSE;
        ch->pcdata->dead = FALSE;
        ch->pcdata->lastskill = 10;
        ch->pcdata->deleted = FALSE;
        for ( foo = 0; foo < 5; foo++ )
        {
            ch->alias[foo] = str_dup("");
            ch->alias_command[foo] = str_dup("");
        }
    }
    ch->deaf                = CHANNEL_FLAME;
    ch->desc                            = d;
    if ( ch->name != NULL )
        free_string( ch->name );
    ch->name                            = str_dup( name );
    //    ch->prompt = str_dup("");
    ch->old_prompt = str_dup("");
    ch->last_tell = str_dup("");
    ch->prompt                          = str_dup( "@@W(@@c%C-%s@@W) @@W[@@gExits:%E@@W] @@e%h/%H@@Rhp @@N> " );
    ch->last_note                       = 0;
    ch->config                            =
        CONFIG_COMBINE
        | CONFIG_QUESTS
        | CONFIG_EXITS
        | CONFIG_HELPER
        | CONFIG_SMALLMAP
        | CONFIG_PROMPT;
    ch->implants = 0;
    ch->sex = SEX_MALE;
    ch->login_sex = -1;
    ch->class = 0;
    ch->current_brand = NULL;
    ch->carry_weight = 0.0;
    ch->carry_number = 0;
    ch->heat = 0.0;
    ch->security = TRUE;
    ch->map = 16;
    ch->c_sn = -1;
    ch->c_level = -1;
    ch->c_obj = NULL;
    ch->victim = ch;
    ch->bvictim = NULL;
    ch->c_time = 0;
    ch->pcdata->alliance = -1;
    ch->x = number_range(BORDER_SIZE+1,(MAX_MAPS-BORDER_SIZE)-1);
    ch->y = number_range(BORDER_SIZE+1,(MAX_MAPS-BORDER_SIZE)-1);
    ch->z = Z_GROUND;
    ch->medals = 0;
    ch->game_points = 0;
    ch->suicide = FALSE;
    ch->kill_group = 1;
    ch->c_count = 0;
    ch->first_building = NULL;
    ch->next_in_room = NULL;
    //if (IMC)
    //    imc_initchar( ch );
    found = FALSE;
    fclose( fpReserve );

    #if !defined(machintosh) && !defined(MSDOS)
    if (is_npc)                                             /* convert spaces to . */
    {
        for (nmptr=name,bufptr=buf;*nmptr != 0; nmptr++)
        {
            if (*nmptr == ' ')
                *(bufptr++)='.';
            else
                *(bufptr++)=*nmptr;
        }
        *(bufptr)=*nmptr;
    }
    else
        strcpy(buf,name);
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR,
        initial( buf ), "/", cap_nocol( buf ) );
    #else
    if (is_npc)
    {
        for (nmptr=ch->name,bufptr=buf;*nmptr != 0; nmptr++)
        {
            if (*nmptr != ' ' && *nmptr != '.')
                *(bufptr++)=*nmptr;
            if ( bufptr-buf == 8)
                break;
        }
        *(bufptr)=0;
    }
    else
        strcpy(buf,name);

    sprintf( strsave, "%s%s", PLAYER_DIR,
        cap_nocol( buf ) );
    #endif

    #if !defined(macintosh) && !defined(MSDOS)
    sprintf( tempstrsave, "%s%s", strsave, ".gz" );
    if ( ( fp = fopen( tempstrsave, "r" ) ) != NULL )
    {
        char buf[MAX_STRING_LENGTH];
        fclose( fp );
        sprintf( buf, "gzip -dfq %s", tempstrsave );
        system( buf );
    }
    #endif

    if ( ( fp = fopen( strsave, "r" ) ) != NULL )
    {
        int iNest;

        for ( iNest = 0; iNest < MAX_NEST; iNest++ )
            rgObjNest[iNest] = NULL;

        found = TRUE;
        for ( ; ; )
        {
            char letter;
            char *word;

            letter = fread_letter( fp );
            if ( letter == '*' )
            {
                fread_to_eol( fp );
                continue;
            }

            if ( letter != '#' )
            {
                monitor_chan( NULL, "Load_char_obj: # not found.",MONITOR_BAD );
                break;
            }

            word = fread_word( fp );
            if      ( !str_cmp( word, "PLAYER" ) )
                fread_char ( ch, fp );
            else if ( !str_cmp( word, "OBJECT" ) ) fread_obj  ( ch, fp );
            else if ( !str_cmp( word, "VEHICLE" ) )
            {
                ch->in_vehicle = fread_vhc  ( ch, fp );
                ch->in_vehicle->driving = ch;
            }
            else if ( !str_cmp( word, "END"    ) ) break;
            else
            {
                monitor_chan( NULL, "Load_char_obj: bad section.", MONITOR_BAD );
                break;
            }
        }
        fclose( fp );
    }

    if (!found && is_npc)
    {
        /* return memory for char back to system. */
        free_char(ch);
    }
    fpReserve = fopen( NULL_FILE, "r" );

    return found;
}

/*
 * Read in a char.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )  if ( !str_cmp( word, literal ) ) { field  = value; fMatch = TRUE;  break;}
#define SKEY( literal, field, value )  if ( !str_cmp( word, literal ) ) { if (field!=NULL) free_string(field);field  = value; fMatch = TRUE;  break;}

void fread_char( CHAR_DATA *ch, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    char *word;
    bool fMatch;
    int foo;
    /* Save revision control: */

    foo = 0;

    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : fread_word( fp );
        fMatch = FALSE;

        switch ( UPPER(word[0]) )
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol( fp );
                break;

            case 'A':
                KEY( "Act",         ch->act,                fread_number( fp ) );
                KEY( "Alliance",    ch->pcdata->alliance,   fread_number( fp ) );
                if ( !str_cmp(word, "alias") )
                {
                    ch->alias[foo] = fread_string(fp);
                    ch->alias_command[foo] = fread_string(fp);
                    fMatch = TRUE;
                    foo++;
                }
                break;
            case 'B':
                if ( !str_cmp( word, "Built" ) )
                {
                    /*		sh_int value = 0;
                            for ( foo = 0; foo < MAX_POSSIBLE_BUILDING; foo++ )
                            {
                                if ( value > -1 )
                                    value = fread_number(fp);

                                if ( value == -1 )
                                    ch->pcdata->built[foo] = FALSE;
                                else
                                    ch->pcdata->built[foo] = value;
                            }*/
                    fMatch = TRUE;
                }
                KEY( "Bkills",  ch->pcdata->bkills, fread_number( fp ) );
                KEY( "BLost",   ch->pcdata->blost,  fread_number( fp ) );
                SKEY( "Bamfin",      ch->pcdata->bamfin,     fread_string( fp ) );
                SKEY( "Bamfout",     ch->pcdata->bamfout,    fread_string( fp ) );
                break;
            case 'C':
                KEY( "Class",       ch->class,              fread_number( fp ) );
                KEY( "Config",      ch->config,             fread_number( fp ) );
                if ( !str_cmp( word, "colors" ) && !IS_NPC(ch))
                {
                    int foo;
                    for ( foo = 0; foo < MAX_color; foo++ )
                    {
                        if ( cur_revision == 35 && foo > 8 )
                            continue;

                        ch->pcdata->color[foo] = fread_number( fp );
                    }
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'D':
                KEY( "Disease", ch->disease,                fread_number( fp ) );
                KEY( "Dead", ch->pcdata->dead,          fread_number( fp ) );
                KEY( "Deleted", ch->pcdata->deleted,        fread_number( fp ) );
                KEY( "Deaths", ch->pcdata->deaths,      fread_number( fp ) );

                if ( !str_cmp( word, "DimCol" ) )
                {
                    char * temp;
                    temp =  fread_string( fp );
                    ch->pcdata->dimcol = temp[0];
                    /*              fread_to_eol( fp );   */
                    free_string( temp );
                    fMatch = TRUE;
                    break;
                }

                break;

            case 'E':
                if ( !str_cmp( word, "End" ) )
                {
                    int rank = get_rank(ch);

                    for ( foo=1;kill_groups[foo-1]>-1;foo++ )
                    {
                        if ( kill_groups[foo] > rank || kill_groups[foo] == -1 )
                        {
                            ch->kill_group = foo;
                            break;
                        }
                    }
                    if ( ch->login_sex < 0 )
                        ch->login_sex = ch->sex;
                    return;
                }
                KEY( "EmailValid", ch->pcdata->valid_email, fread_number( fp ) );
                KEY( "Effect", ch->effect,            fread_number( fp ) );
                KEY( "Experience", ch->pcdata->experience,        fread_number( fp ) );
                SKEY( "Email", ch->pcdata->email_address, fread_string( fp ) );

            case 'F':
                KEY( "Failures",   ch->pcdata->failures,   fread_number( fp ) );
                KEY( "Fighttimer", ch->fighttimer, fread_number( fp ) );
                break;

            case 'G':
                KEY( "GainHp",  ch->pcdata->hp_from_gain, fread_number( fp ) );
                KEY( "GPoints", ch->game_points, fread_number( fp ) );
                break;

            case 'H':
                SKEY( "Host",  ch->pcdata->host,   fread_string( fp ) );

                if ( !str_cmp( word, "HiCol" ) )
                {
                    char * temp;
                    temp =  fread_string( fp );
                    ch->pcdata->hicol = temp[0];
                    /*              fread_to_eol( fp );   */
                    free_string( temp );
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp( word, "Hp" ) )
                {
                    ch->hit         = fread_number( fp );
                    ch->max_hit     = fread_number( fp );
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'I':
                KEY( "Incog", ch->incog,        fread_number( fp ) );
                KEY( "Invis",   ch->invis,      fread_number( fp ) );
                KEY( "Implants",    ch->implants,           fread_number( fp ) );
                //if (IMC)
                //           if( ( fMatch = imc_loadchar( ch, fp, word ) ) )
                //                break;
                break;

            case 'L':
                if ( !str_cmp( word, "Location" ) )
                {
                    ch->x   = fread_number( fp );
                    ch->y     = fread_number( fp );
                    ch->z     = fread_number( fp );
                    if ( ch->x > MAX_MAPS - BORDER_SIZE || ch->y > MAX_MAPS - BORDER_SIZE )
                    {
                        ch->x = number_range(BORDER_SIZE+1,MAX_MAPS-BORDER_SIZE-1);
                        ch->y = number_range(BORDER_SIZE+1,MAX_MAPS-BORDER_SIZE-1);
                        ch->z = Z_UNDERGROUND;
                    }

                    fMatch = TRUE;
                    break;
                }
                KEY( "LoginSex",    ch->login_sex,      fread_number( fp ) );
                KEY( "Level",   ch->level,      fread_number( fp ) );
                SKEY( "LastLogin",   ch->pcdata->lastlogin,  fread_string( fp ) );
                SKEY( "Loadmsg",   ch->pcdata->load_msg,  fread_string( fp ) );
                break;

            case 'M':
                KEY( "Map",    ch->map,        fread_number( fp ) );
                KEY( "Medals", ch->medals,     fread_number( fp ) );
                KEY( "MedalTimer", ch->medaltimer, fread_number( fp ) );
                KEY( "Monitor",    ch->pcdata->monitor,    fread_number( fp ) );
                if ( !str_cmp( word, "m/c" ) )
                {
                    switch (cur_revision)
                    {
                        default:
                            break;
                    }
                    fMatch = TRUE;
                }
                break;

            case 'N':
                KEY( "NukemWins",  ch->pcdata->nukemwins,  fread_number( fp ) );
                if ( !str_cmp( word, "Name" ) )
                {
                    /*
                     * Name already set externally.
                     */
                    fread_to_eol( fp );
                    fMatch = TRUE;
                    break;
                }
                KEY( "Note",        ch->last_note,          fread_number( fp ) );
                break;

            case 'P':
                if (!IS_NPC(ch))
                {
                    KEY( "Pagelen",     ch->pcdata->pagelen,    fread_number( fp ) );
                    SKEY( "Password",    ch->pcdata->pwd,        fread_string( fp ) );
                    KEY( "Pkills", ch->pcdata->pkills, fread_number( fp ) );
                    KEY( "Pflags",      ch->pcdata->pflags,     fread_number( fp ) );
                    KEY( "PBHits", ch->pcdata->pbhits, fread_number( fp ) );
                    KEY( "PBDeaths",      ch->pcdata->pbdeaths,     fread_number( fp ) );
                }
                KEY( "Played",      ch->played,             fread_number( fp ) );
                KEY( "PlayedTot",   ch->played_tot,         fread_number( fp ) );
                KEY( "Position",    ch->position,           fread_number( fp ) );
                SKEY( "Prompt",      ch->prompt,             fread_string( fp ) );
                if ( !str_cmp(word, "pager") )
                {
                    PAGER_DATA *p;

                    GET_FREE(p,pager_free);
                    if ( p->from != NULL )
                        free_string(p->from);
                    if ( p->msg != NULL )
                        free_string(p->msg);
                    if ( p->time != NULL )
                        free_string(p->time);
                    p->is_free = FALSE;
                    p->prev = NULL;
                    p->next = ch->pcdata->pager;
                    p->from = fread_string(fp);
                    p->time = fread_string(fp);
                    p->msg = fread_string(fp);
                    ch->pcdata->pager = p;
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp(word,"ProfPt") )
                {
                    ch->pcdata->prof_points = fread_number(fp);
                    ch->pcdata->prof_ttl = fread_number(fp);
                    if ( ch->pcdata->prof_ttl < ch->pcdata->prof_points )
                        ch->pcdata->prof_ttl = ch->pcdata->prof_points;
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'Q':
                KEY( "Questpoints",  ch->quest_points,           fread_number( fp ) );

            case 'R':
                KEY( "Reimb",      ch->pcdata->reimb,     fread_number( fp ) );
                KEY( "Revision",  cur_revision,           fread_number( fp ) );
                SKEY( "Roomenter",    ch->pcdata->room_enter, fread_string( fp ) );
                SKEY( "Roomexit", ch->pcdata->room_exit,  fread_string( fp ) );
                if ( !str_cmp(word,"Refund") )
                {
                    int value, i;
                    value = 0;

                    for ( i=0;i<8;i++ )
                    {
                        if ( value != -1 )
                            value = fread_number(fp);
                        if ( value <= -1 )
                            ch->refund[i] = 0;
                        else
                            ch->refund[i] = value;
                    }
                    fMatch = TRUE;
                    break;
                }

                if ( !str_cmp( word, "Room" ) )
                {
                    ch->in_room = get_room_index( fread_number( fp ) );
                    if ( ch->in_room == NULL )
                        ch->in_room = get_room_index( ROOM_VNUM_LIMBO );
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp( word, "Ranking" ) )
                {
                    if ( ch->pcdata->ranking != NULL )
                        free_string( ch->pcdata->ranking );
                        ch->pcdata->ranking = fread_string( fp );
                    if ( isalpha(ch->pcdata->ranking[0]) || isdigit(ch->pcdata->ranking[0]) )
                    {
                        sprintf( buf, "%s", ch->pcdata->ranking );
                        free_string( ch->pcdata->ranking );
                        ch->pcdata->ranking = str_dup( buf );
                    }
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'S':
                KEY( "Sex",         ch->sex,                fread_number( fp ) );
                KEY( "Setexit",     ch->pcdata->set_exit,   fread_number( fp ) );
                KEY( "Suicided",    ch->suicide,            fread_number( fp ) );
                if ( !str_cmp(word,"Spec") )
                {
                    ch->pcdata->spec_init = fread_number(fp);
                    fMatch = TRUE;
                    break;
                }
                if ( !str_cmp(word,"Skill") )
                {
                    int value, i;
                    value = 0;

                    for ( i=0;i<MAX_SKILL;i++ )
                    {
                        if ( value != -1 )
                            value = fread_number(fp);
                        if ( value == -1 )
                            ch->pcdata->skill[i] = 0;
                        else
                            ch->pcdata->skill[i] = value;
                    }
                    if ( value != -1 )
                        value = fread_number(fp);
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'T':
                KEY( "Trust",       ch->trust,              fread_number( fp ) );
                KEY( "TermRows", ch->pcdata->term_rows,     fread_number( fp ) );
                KEY( "TermColumns", ch->pcdata->term_columns, fread_number( fp ) );
                KEY( "TBkills", ch->pcdata->tbkills,    fread_number( fp ) );
                KEY( "TPkills", ch->pcdata->tpkills,    fread_number( fp ) );
                if ( !str_cmp( word, "Title" ) )
                {
                    if ( ch->pcdata->title != NULL )
                        free_string( ch->pcdata->title );
                    ch->pcdata->title = fread_string( fp );
                    if ( isalpha(ch->pcdata->title[0])
                        ||   isdigit(ch->pcdata->title[0]) )
                    {
                        sprintf( buf, " %s", ch->pcdata->title );
                        free_string( ch->pcdata->title );
                        ch->pcdata->title = str_dup( buf );
                    }
                    fMatch = TRUE;
                    break;
                }

                break;

            case 'W':
                KEY( "Wizbit",      ch->wizbit,             fread_number( fp ) );
                if ( !str_cmp( word, "Whoname" ) )
                {
                    if ( ch->pcdata->who_name != NULL )
                        free_string( ch->pcdata->who_name );
                    ch->pcdata->who_name = fread_string( fp );
                    sprintf( buf, "%s", ch->pcdata->who_name+1 );

                    free_string( ch->pcdata->who_name );
                    ch->pcdata->who_name = str_dup( buf );
                    fMatch = TRUE;
                    break;
                }
                break;
        }

        /* Make sure old chars have this field - Kahn */
        if (!IS_NPC(ch))
        {
            if ( !ch->pcdata->pagelen )
                ch->pcdata->pagelen = 20;
            if ( !ch->prompt || *ch->prompt == '\0' )
                ch->prompt = str_dup("<%h %m %mv> ");
        }

        if ( !fMatch )
        {
            sprintf( log_buf, "Loading in pfile :%s, no match for ( %s ).",
                ch->name, word );
            monitor_chan( ch, log_buf, MONITOR_BAD );
            fread_to_eol( fp );
        }
    }

}

VEHICLE_DATA * fread_vhc( CHAR_DATA *ch, FILE *fp )
{
    VEHICLE_DATA *vhc;
    char *word;
    int i;

    GET_FREE(vhc, vehicle_free);
    vhc->type = fread_number(fp);
    //	free_string(vhc->name);
    vhc->name = fread_string(fp);
    //	free_string(vhc->desc);
    vhc->desc = fread_string(fp);
    vhc->x = fread_number(fp);
    vhc->y = fread_number(fp);
    vhc->z = fread_number(fp);
    vhc->hit = fread_number(fp);
    vhc->max_hit = fread_number(fp);
    vhc->ammo_type = fread_number(fp);
    vhc->ammo = fread_number(fp);
    vhc->max_ammo = fread_number(fp);
    vhc->fuel = fread_number(fp);
    vhc->max_fuel = fread_number(fp);
    vhc->flags = fread_number(fp);
    vhc->speed = fread_number(fp);
    vhc->range = fread_number(fp);
    vhc->scanner = fread_number(fp);
    for ( i=0;i<POWER_MAX;i++ )
        vhc->power[i] = fread_number(fp);
    vhc->x = ch->x;
    vhc->y = ch->y;
    vhc->z = ch->z;
    move_vehicle(vhc,vhc->x,vhc->y,vhc->z);
    word   = feof( fp ) ? "End" : fread_word( fp );
    LINK(vhc, first_vehicle, last_vehicle, next, prev);
    if ( !str_cmp(word,"INSIDE") )
    {
        VEHICLE_DATA *vhc2;
        vhc2 = fread_vhc(ch,fp);
        vhc2->in_vehicle = vhc;
        vhc->vehicle_in = vhc2;
        move_vehicle(vhc2,vhc->x,vhc->y,vhc->z);
    }
    return vhc;
}

#define TEMP_VNUM 3090

extern int top_obj_index;

void fread_obj( CHAR_DATA *ch, FILE *fp )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;
    char *word = "End";
    int iNest;
    bool fMatch = FALSE;
    bool fNest;
    bool fVnum;
    int Temp_Obj=0,OldVnum=0;
    extern int cur_revision;

    GET_FREE(obj, obj_free);
    *obj                = obj_zero;
    obj->name           = str_dup( "" );
    obj->short_descr    = str_dup( "" );
    obj->description    = str_dup( "" );
    fNest               = FALSE;
    fVnum               = TRUE;
    iNest               = 0;

    for ( ; ; )
    {
        word   = feof( fp ) ? "End" : fread_word( fp );
        fMatch = FALSE;

        switch ( UPPER(word[0]) )
        {
            case '*':
                fMatch = TRUE;
                fread_to_eol( fp );
                break;

            case 'D':
                SKEY( "Description", obj->description,       fread_string( fp ) );
                break;

            case 'E':
                KEY( "ExtraFlags",  obj->extra_flags,       fread_number( fp ) );

                if ( !str_cmp( word, "End" ) )
                {
                    if ( !fNest || !fVnum )
                    {
                        monitor_chan( NULL, "Fread_obj: incomplete object.", MONITOR_BAD );
                        PUT_FREE(obj, obj_free);
                        return;
                    }
                    else
                    {
                        LINK(obj, first_obj, last_obj, next, prev);
                        if ( obj->item_type == ITEM_BOMB )
                            obj->bomb_data = make_bomb(obj);
                        else
                            obj->bomb_data = NULL;

                        if (Temp_Obj)
                        {
                            int newvnum;
                            OBJ_INDEX_DATA *pObjIndex;
                            int nMatch=0;
                            int vnum;

                            /* One of three things:
                                 Obj Vnum was deleted
                                 Obj Vnum was moved
                                 Obj Vnum was previously deleted */
                            newvnum=TEMP_VNUM;

                            if (OldVnum != TEMP_VNUM)
                            {
                                /* Check on move table */
                                if ( (newvnum=(int) get_hash_entry(hash_changed_vnums,OldVnum)) != 0)
                                {
                                    obj->pIndexData=get_obj_index(newvnum);
                                    if (obj->pIndexData == NULL)
                                    {
                                        obj->pIndexData=get_obj_index(TEMP_VNUM);
                                        newvnum=TEMP_VNUM;
                                    }
                                }
                            }

                            if (newvnum==TEMP_VNUM)
                            {
                                /* Scan through objects, trying to find a matching description */
                                for ( vnum = 0; nMatch < top_obj_index; vnum++ )
                                {
                                    if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
                                    {
                                        nMatch++;
                                        if ( !str_cmp( obj->short_descr, pObjIndex->short_descr ) )
                                        {
                                            obj->pIndexData=pObjIndex;
                                            break;
                                        }
                                    }
                                }
                            }

                        }

                        if ( obj->pIndexData->vnum == 1152 && obj->value[4] > 0 )
                            obj->value[4] = 0;

                        else if ( iNest == 0 || rgObjNest[iNest] == NULL )
                            obj_to_char( obj, ch );
                        return;
                    }
                }
                break;

            case 'H':
                KEY( "Heat",      obj->heat,            fread_number( fp ) );
                break;

            case 'I':
                KEY( "ItemType",    obj->item_type,         fread_number( fp ) );
                break;

            case 'L':
                KEY( "Level",       obj->level,             fread_number( fp ) );
                break;

            case 'N':
                SKEY( "Name",        obj->name,              fread_string( fp ) );

                if ( !str_cmp( word, "Nest" ) )
                {
                    iNest = fread_number( fp );
                    if ( iNest == -1 )
                    {
                        fNest = TRUE;
                    }
                    else if ( iNest < 0 || iNest >= MAX_NEST )
                    {
                        monitor_chan( NULL, "Fread_obj: bad nest.", MONITOR_BAD );
                    }
                    else
                    {
                        rgObjNest[iNest] = obj;
                        fNest = TRUE;
                    }
                    fMatch = TRUE;
                }
                break;

            case 'S':
                SKEY( "ShortDescr",  obj->short_descr,       fread_string( fp ) );
                break;

            case 'V':
                if ( !str_cmp( word, "Values" ) )
                {
                    int looper;

                    for( looper = 0;
                        looper < MAX_OBJECT_VALUES;
                        obj->value[looper] = fread_number( fp ),looper++ );

                    fMatch          = TRUE;
                    break;
                }

                if ( !str_cmp( word, "Vnum" ) )
                {
                    int vnum;

                    vnum = fread_number( fp );

                    if ( ( obj->pIndexData = get_obj_index( vnum ) ) == NULL
                        || vnum == TEMP_VNUM )
                    {
                        /* Set flag saying that object is temporary */
                        Temp_Obj=1;
                        OldVnum=vnum;
                        vnum = TEMP_VNUM;
                        obj->pIndexData = get_obj_index(vnum);
                    }
                    /* bug( "Fread_obj: bad vnum %d.", vnum ); This killed it. */
                    else
                        fVnum = TRUE;
                    fMatch = TRUE;
                    break;
                }
                break;

            case 'Q':
                if ( !str_cmp( word, "Quest" ) )
                {
                    obj->quest_timer = fread_number(fp);
                    obj->quest_map = fread_number(fp);
                    fMatch = TRUE;
                    break;
                }
                break;
            case 'W':
                if ( !str_cmp( word, "WearLoc" ) )
                {
                    if ( cur_revision < UPGRADE_REVISION )
                    {
                        int temp_loc;
                        temp_loc = fread_number( fp );
                        obj->wear_loc = -1;
                        fMatch = TRUE;
                        break;
                    }
                    else if ( cur_revision < 19 )
                    {
                        int loc_adjust=0;
                        int temp_loc;
                        temp_loc = fread_number( fp );
                        /* Adjust to the new wear slots */
                        if ( temp_loc > 1 && temp_loc < 5 )
                            loc_adjust = 1;
                        else if (temp_loc > 4 && temp_loc < 25)
                            loc_adjust = 2;
                        else if (temp_loc > 24 && temp_loc < 27)
                            loc_adjust = 4;
                        else if (temp_loc > 26)
                            loc_adjust = 6;

                        fMatch = TRUE;
                        obj->wear_loc = temp_loc + loc_adjust;
                        break;
                    }
                    else
                    {
                        KEY( "WearLoc",     obj->wear_loc,          fread_number( fp ) );
                        if ( cur_revision < 19 )
                        {
                            int loc_adjust=0;
                            /* Adjust to the new wear slots */
                            if ( obj->wear_loc > 1 && obj->wear_loc < 5 )
                                loc_adjust = 1;
                            else if (obj->wear_loc > 4 && obj->wear_loc < 25)
                                loc_adjust = 2;
                            else if (obj->wear_loc > 24 && obj->wear_loc < 27)
                                loc_adjust = 4;
                            else if (obj->wear_loc > 26)
                                loc_adjust = 6;
                            obj->wear_loc = obj->wear_loc + loc_adjust;
                        }
                    }
                }
                if ( !str_cmp( word, "WearFlags" ) )
                {
                    if ( cur_revision < UPGRADE_REVISION )
                    {
                        int temp_flags, index, new_flags = 0;
                        extern const int convert_wearflags[];
                        temp_flags = fread_number( fp );
                        for ( index = 0; index < 32; index++ )
                        {
                            if (  IS_SET( temp_flags, ( 1 << index )  ) )
                            {
                                SET_BIT( new_flags, convert_wearflags[ index ]);
                            }
                        }
                        obj->wear_flags = new_flags;
                        fMatch = TRUE;
                        break;
                    }
                    else
                    {
                        KEY( "WearFlags",     obj->wear_flags,          fread_number( fp ) );
                    }
                }
                KEY( "Weight",      obj->weight,            fread_number( fp ) );
                break;

        }

        if ( !fMatch )
        {
            char log_buf[MSL];                              //xxxxx
            sprintf( log_buf, "PFILE: %s BAD!", ch->name );
            log_f(log_buf);
            monitor_chan( NULL, "Fread_obj: no match.", MONITOR_BAD );
            fread_to_eol( fp );
            return;
        }
    }
}

void fwrite_object( OBJ_DATA *obj, FILE *fp )
{
    int i;
    fprintf( fp, "O " );
    fprintf( fp, "%d %s~ %s~ %s~ %d %d %d %d %d %d %d %d %d %d %d",
        obj->pIndexData->vnum, obj->name, obj->short_descr, obj->description, obj->extra_flags,
        obj->wear_flags, obj->item_type, obj->weight, obj->heat, obj->level, obj->x, obj->y, obj->z,
        obj->quest_timer, obj->quest_map
        );

    for ( i=0;i<MAX_OBJECT_VALUES;i++ )
        fprintf( fp, " %d", obj->value[i] );

    fprintf( fp, "\n" );
    return;
}

void save_bans( )
{

    FILE * fp;
    char ban_file_name[MAX_STRING_LENGTH];
    BAN_DATA  *pban;

    fclose( fpReserve );
    sprintf( ban_file_name, "%s", BANS_FILE );

    if ( ( fp = fopen( ban_file_name, "w" ) ) == NULL )
    {
        bug( "Save ban list: fopen", 0 );
        perror( "failed open of bans.lst in save_ban" );
    }
    else
    {
        for ( pban = first_ban; pban != NULL; pban = pban->next )
        {
            fprintf( fp, "#BAN~\n" );
            fprintf( fp, "%d\n", ( pban->newbie ? 1 : 0 ) );
            fprintf( fp, "%s~\n", pban->name );
            fprintf( fp, "%s~\n", pban->banned_by );
            fprintf( fp, "%s~\n", pban->note );
        }
        fprintf( fp, "#END~\n\n" );
    }

    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;

}

void save_objects( int mode )
{

    FILE * fp;
    char objects_file_name[MAX_STRING_LENGTH];
    OBJ_DATA * obj;
    OBJ_DATA * obj_next;
    BUILDING_DATA *bld;

    fclose( fpReserve );
    sprintf( objects_file_name, "%s", OBJECTS_FILE );
    if ( mode == 3 )
        sprintf( objects_file_name, "%s", OBJECTS_BACKUP_FILE );
    else if ( mode == 4 )
        sprintf( objects_file_name, "%s", OBJECTS_FEST_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Objects: fopen", 0 );
        perror( "failed open of objects_file in save_objects" );
    }
    else
    {
        for (obj = first_obj; obj != NULL; obj = obj_next )
        {
            if ( obj == NULL )
                break;
            obj_next = obj->next;
            if ( obj->in_room == NULL || obj->carried_by != NULL )
                continue;
            bld = get_obj_building(obj);
            if ( obj->x == 0 || obj->y == 0 )
                continue;
            if ( mode == 0 && obj->item_type != ITEM_FLAG && obj->z != Z_SPACE && obj->item_type != ITEM_LIGHT && obj->item_type != ITEM_BIOTUNNEL && (!bld || !WAREHOUSE(bld) || !complete(bld) || obj->item_type == ITEM_BOARD || obj->carried_by != NULL || obj->quest_timer > 0 || ( obj->item_type == ITEM_BOMB && obj->value[1] != 0 ) ) )
                continue;

            fwrite_object( obj, fp );
        }
        fprintf( fp, "#DONE\n\n" );

    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void fread_object( FILE *fp )
{
    static OBJ_DATA obj_zero;
    OBJ_DATA *obj;
    OBJ_INDEX_DATA*pObj;
    char buf[MSL];
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
    int vnum;

    vnum = fread_number(fp);
    if ( ( pObj = get_obj_index(vnum) ) == NULL )
    {
        sprintf( buf, "Bad object, vnum %d.", vnum );
        log_f(buf);
        return;
    }

    GET_FREE(obj, obj_free);
    *obj                = obj_zero;

    obj->wear_loc = WEAR_NONE;
    obj->pIndexData = pObj;
    obj->name           = fread_string(fp);
    obj->short_descr    = fread_string(fp);
    obj->description    = fread_string(fp);
    obj->extra_flags    = fread_number( fp );
    obj->wear_flags     = fread_number( fp );
    obj->item_type      = fread_number( fp );
    obj->weight     = fread_number( fp );
    obj->heat       = fread_number( fp );
    obj->level      = fread_number( fp );
    obj->x          = fread_number( fp );
    obj->y          = fread_number( fp );
    obj->z          = fread_number( fp );
    obj->quest_timer    = fread_number( fp );
    obj->quest_map      = fread_number( fp );

    for ( vnum=0;vnum<MAX_OBJECT_VALUES;vnum++ )
        obj->value[vnum]    = fread_number( fp );

    if ( obj->pIndexData->vnum >= 800 && obj->pIndexData->vnum < 900 )
        obj->z = Z_SPACE;
    obj->in_building = map_bld[obj->x][obj->y][obj->z];
    obj->next_in_room = map_obj[obj->x][obj->y];
    map_obj[obj->x][obj->y] = obj;

    if ( obj->quest_timer > 0 )
    {
        int i;
        extern OBJ_DATA *quest_obj[MAX_QUEST_ITEMS];
        for ( i=0;i<MAX_QUEST_ITEMS;i++ )
        {
            if ( quest_obj[i] == NULL )
            {
                quest_obj[i] = obj;
                break;
            }
        }
    }
    if ( obj->owner != NULL )
        free_string(obj->owner);
    if ( obj->in_building )
    {
        obj->owner = str_dup(obj->in_building->owned);
    }
    else
        obj->owner = str_dup("Nobody");

    LINK(obj, first_obj, last_obj, next, prev);

    if ( obj->item_type == ITEM_BOMB )
        obj->bomb_data = make_bomb(obj);
    else
        obj->bomb_data = NULL;
    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
    move_obj(obj,obj->x,obj->y,obj->z);
    return;
}

void save_map( )
{

    FILE * fp;
    char objects_file_name[MAX_STRING_LENGTH];
    int i,j,z;
    char buf[MSL];
    fclose( fpReserve );
    sprintf( objects_file_name, "%s", MAP_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Objects: fopen", 0 );
        perror( "failed open of map_file in save_map" );
    }
    else
    {
        for ( z=1;z<Z_MAX;z++ )
        {
            if ( planet_table[z].system == 0 )
                continue;
            for (i=0;i<MAX_MAPS;i++)
            {
                sprintf( buf, " " );
                for (j=0;j<MAX_MAPS;j++)
                    sprintf( buf+strlen(buf), "%d ", map_table.type[i][j][z] );
                sprintf( buf+strlen(buf), "\n" );
                fprintf( fp, buf );
            }
        }
    }
    fprintf( fp, "-1" );
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;

}

void save_alliances( )
{
    FILE * fp;
    char objects_file_name[MAX_STRING_LENGTH];
    int i;
    char buf[MSL];
    fclose( fpReserve );
    sprintf( objects_file_name, "%s", ALLIANCES_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Objects: fopen", 0 );
        perror( "failed open of alliances_file in save_alliances" );
    }
    else
    {
        for (i=0;i<99999;i++)
        {
            if ( alliance_table[i].name == NULL )
                break;
            sprintf( buf, "A %s~ %s~ %d %d\n", alliance_table[i].name, alliance_table[i].leader, alliance_table[i].members, alliance_table[i].kills );
            fprintf( fp, buf );
        }
        fprintf( fp, "#DONE" );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;

}

void fwrite_building( BUILDING_DATA *bld, FILE *fp )
{
    fprintf( fp, "B   %d %s~ %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s~ %d %d %d %d %d %d %d %d %s~\n",
        bld->type, bld->name, bld->exit[0], bld->exit[1], bld->exit[2], bld->exit[3], bld->maxhp, bld->hp, bld->maxshield, bld->shield,
        bld->value[0], bld->value[1], bld->value[2], bld->value[3], bld->value[4], bld->value[5], bld->value[6], bld->value[7], bld->value[8], bld->value[9], bld->value[10],
        bld->resources[0], bld->resources[1], bld->resources[2], bld->resources[3], bld->resources[4], bld->resources[5], bld->resources[6], bld->resources[7],
        bld->owned, bld->x, bld->y, bld->z, bld->level, bld->visible, bld->directories, bld->real_dir, bld->password, bld->attacker );
    return;
}

void save_buildings( )
{
    BUILDING_DATA *bld;
    FILE * fp;
    char objects_file_name[MAX_STRING_LENGTH];
    fclose( fpReserve );
    sprintf( objects_file_name, "%s", BUILDING_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Buildings: fopen", 0 );
        perror( "failed open of building_file in save_buildings" );
    }
    else
    {
        for ( bld = first_building;bld;bld = bld->next )
        {
            if ( is_evil(bld) && bld->z != Z_PAINTBALL && !medal(bld) && bld->timer == 0 )
                continue;
            fwrite_building(bld, fp);
        }
        fprintf( fp, "#DONE" );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;

}

void save_buildings_b( int mode )
{
    BUILDING_DATA *bld;
    FILE * fp;
    char objects_file_name[MAX_STRING_LENGTH];
    fclose( fpReserve );
    if ( mode == 0 )
        sprintf( objects_file_name, "%s", BUILDING_BACKUP_FILE );
    else
        sprintf( objects_file_name, "%s", BUILDING_FEST_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Buildings: fopen", 0 );
        perror( "failed open of building_backup_file in save_buildings" );
    }
    else
    {
        for ( bld = first_building;bld;bld = bld->next )
        {
            fwrite_building(bld, fp);
        }
        fprintf( fp, "#DONE" );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void save_multiplay( )
{
    int i;
    FILE *fp;
    char objects_file_name[MAX_STRING_LENGTH];
    fclose( fpReserve );
    sprintf( objects_file_name, "%s", MULTIPLAY_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Multiplay: fopen", 0 );
        perror( "failed open of multiplay_file in save_multiplay" );
    }
    else
    {
        for ( i=0;i<30;i++ )
        {
            if ( multiplay_table[i].name == NULL && multiplay_table[i].host == NULL )
                continue;
            fprintf( fp, "M %s~ %s~\n", multiplay_table[i].name, multiplay_table[i].host );
        }
        fprintf( fp, "\n#DONE" );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void save_scores( )
{
    int i;
    FILE *fp;
    char objects_file_name[MAX_STRING_LENGTH];
    fclose( fpReserve );
    sprintf( objects_file_name, "%s", SCORE_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Scores: fopen", 0 );
        perror( "failed open of score_file in save_scores" );
    }
    else
    {
        for ( i=0;i<100;i++ )
        {
            if ( score_table[i].name == NULL )
                break;
            fprintf( fp, "S %s~ %s~ %d %d %d\n", score_table[i].name, score_table[i].killedby, score_table[i].kills, score_table[i].buildings,score_table[i].time );
        }
        fprintf( fp, "\n#DONE" );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void save_ranks( )
{
    int i;
    FILE *fp;
    char objects_file_name[MAX_STRING_LENGTH];
    fclose( fpReserve );
    sprintf( objects_file_name, "%s", RANK_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Scores: fopen", 0 );
        perror( "failed open of rank_file in save_ranks" );
    }
    else
    {
        for ( i=0;i<30;i++ )
        {
            if ( rank_table[i].name == NULL )
                break;
            fprintf( fp, "R %s~ %d\n",rank_table[i].name, rank_table[i].rank );
        }
        fprintf( fp, "\n#DONE" );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void fwrite_vhc( VEHICLE_DATA *vhc, FILE *fp )
{
    int i;
    if ( vhc->in_vehicle )
        fprintf(fp, "INSIDE\n" );
    else
        fprintf( fp, "#VEHICLE\n" );
    fprintf( fp, "%d %s~ %s~ %d %d %d %d %d %d %d %d %d %d %d %d %d %d ",
        vhc->type, vhc->name, vhc->desc, vhc->x, vhc->y, vhc->z, vhc->hit, vhc->max_hit,
        vhc->ammo_type, vhc->ammo, vhc->max_ammo, vhc->fuel, vhc->max_fuel, vhc->flags, vhc->speed,
        vhc->range, vhc->scanner );
    for ( i=0;i<POWER_MAX;i++ )
        fprintf( fp, "%d ", vhc->power[i] );
    if ( vhc->vehicle_in )
        fwrite_vhc(vhc->vehicle_in,fp);
    fprintf( fp, "End\n\n" );
    return;
}

void fwrite_vehicle( VEHICLE_DATA *vhc, FILE *fp )
{
    int i;
    fprintf( fp, "V   %d %s~ %s~ %d %d %d %d %d %d %d %d %d %d %d %d %d %d %s~ ",
        vhc->type, vhc->name, vhc->desc, vhc->x, vhc->y, vhc->z, vhc->hit, vhc->max_hit,
        vhc->ammo_type, vhc->ammo, vhc->max_ammo, vhc->fuel, vhc->max_fuel, vhc->flags, vhc->speed,
        vhc->range, vhc->scanner,
        (vhc->driving)?vhc->driving->name:"Nobody" );
    for ( i=0;i<POWER_MAX;i++ )
        fprintf( fp, "%d ", vhc->power[i] );
    fprintf( fp, "\n" );
    return;
}

void save_vehicles( int mode )
{
    VEHICLE_DATA *vhc;
    FILE * fp;
    char objects_file_name[MAX_STRING_LENGTH];
    fclose( fpReserve );

    if ( mode == 1 )
        sprintf( objects_file_name, "%s", VEHICLE_BACKUP_FILE );
    else if ( mode == 2 )
        sprintf( objects_file_name, "%s", VEHICLE_FEST_FILE );
    else
        sprintf( objects_file_name, "%s", VEHICLE_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Vehicless: fopen", 0 );
        perror( "failed open of vehicle_file in save_vehicles" );
    }
    else
    {
        for ( vhc = first_vehicle;vhc;vhc = vhc->next )
            if ( !vhc->driving && !vhc->in_vehicle )
                fwrite_vehicle(vhc, fp);
        fprintf( fp, "#DONE" );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;

}

void save_building_table( )
{
    FILE * fp;
    int i,j;
    char objects_file_name[MAX_STRING_LENGTH];
    fclose( fpReserve );
    sprintf( objects_file_name, "%s", BUILDING_TABLE_FILE );

    if ( ( fp = fopen( objects_file_name, "w" ) ) == NULL )
    {
        bug( "Save Building_table: fopen", 0 );
        perror( "failed open of vehicle_file in save_building_table" );
    }
    else
    {
        fprintf( fp, "%d\n", BUILDING_REVISION );
        for ( i=0;i<MAX_BUILDING;i++ )
        {
            fprintf( fp, "B %s~ %d %d ", build_table[i].name, build_table[i].hp, build_table[i].shield );
            for ( j=0;j<8;j++ )
                fprintf( fp, "%d ", build_table[i].resources[j] );
            fprintf( fp, "%d %d %s~ %s~ ", build_table[i].requirements, build_table[i].requirements_l, build_table[i].desc, build_table[i].symbol );
            for ( j=0;j<MAX_BUILDON;j++ )
                fprintf( fp, "%d ", build_table[i].buildon[j] );
            fprintf( fp, "%d %d %d %d %d %s~\n", build_table[i].militairy, build_table[i].rank, build_table[i].act, build_table[i].max, build_table[i].disabled, build_help_table[i].help );
        }
        fprintf( fp, "#DONE" );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;

}
