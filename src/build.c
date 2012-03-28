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
#include "tables.h"

/* This program provides the interpreting of building commands */

/* The tables are now in buildtab.c: (This file was getting a bit big.)
     Object item type    :    tab_item_types      : number
     Object extra flags  :    tab_obj_flags       : bit_vector
     Object wear flags   :    tab_wear_flags      : bit_vector
     Wear locations      :    tab_wear_loc        : number

*/
#define                 MAX_STRING      2097152

/*
 * Directions.
 */
const char * sDirs[]={ "North", "East ", "South", "West ", "Up   ", "Down " };
const char * cDirs="NESWUD";
int          RevDirs[]={ 2, 3, 0, 1, 5, 4 };

bool    fClanModified = FALSE;                              // For save_clan_table

/*
 * Sex.
 * Used in #MOBILES.
 */
#define NEUTRAL                   0
#define MALE                      1
#define FEMALE                    2

/* Interp. vars et al.
 *
 */

/*
 * Command logging types.
 */
#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2

/*
 * God Levels
 */
#define L_GOD           MAX_LEVEL
#define L_SUP           L_GOD - 1
#define L_DEI           L_SUP - 1
#define L_ANG           L_DEI - 1
#define L_HER           L_ANG - 1

/*
 * Log-all switch.
 */
extern bool                            fLogAll;

DECLARE_DO_FUN( build_set_bedit         );
DECLARE_DO_FUN( build_set_oedit         );
DECLARE_DO_FUN( build_set_nedit         );

DECLARE_DO_FUN( build_setvnum           );
DECLARE_DO_FUN( build_list              );
DECLARE_DO_FUN( build_set               );
DECLARE_DO_FUN( build_listvalues        );
DECLARE_DO_FUN( build_listbuildings     );
DECLARE_DO_FUN( build_uobjs     );
DECLARE_DO_FUN( build_findhelp      );
DECLARE_DO_FUN( build_commands      );
DECLARE_DO_FUN( build_clone     );

/* build_functions */
DECLARE_DO_FUN ( build_showobj      );
DECLARE_DO_FUN ( build_findobject   );
DECLARE_DO_FUN ( build_help         );
DECLARE_DO_FUN ( build_helpedit     );
DECLARE_DO_FUN ( build_setbuilding  );
DECLARE_DO_FUN ( build_setobject    );
DECLARE_DO_FUN ( build_stop         );
DECLARE_DO_FUN ( build_addobject    );
DECLARE_DO_FUN ( build_delwarn      );
DECLARE_DO_FUN ( build_delobject    );
DECLARE_DO_FUN ( build_delhelp      );
DECLARE_DO_FUN ( build_addhelp      );
DECLARE_DO_FUN ( build_addbuilding  );

/* Functions in buildare.c: */
DECLARE_DO_FUN ( build_showarea     );
DECLARE_DO_FUN ( build_findarea     );
DECLARE_DO_FUN ( build_addarea      );
DECLARE_DO_FUN ( build_setarea      );
DECLARE_DO_FUN ( build_makearea         );

/* Commands */
const   struct  cmd_type        build_cmd_table   [] =
{
    /*
     * Common movement commands.
     */

    { "north",          do_north,       POS_STANDING,    0,  LOG_NORMAL },
    { "east",           do_east,        POS_STANDING,    0,  LOG_NORMAL },
    { "south",          do_south,       POS_STANDING,    0,  LOG_NORMAL },
    { "west",           do_west,        POS_STANDING,    0,  LOG_NORMAL },
    { "goto",           do_goto,        POS_STANDING,    0,  LOG_NORMAL },
    { "purge",          do_purge,       POS_STANDING,    0,  LOG_NORMAL },
    { "who",            do_who,         POS_STANDING,    0,  LOG_NORMAL },

    /*
     * Building commands.
     */

    { "look",           do_look,        POS_STANDING,   0,  LOG_NORMAL },
    { "list",           build_list,     POS_STANDING,   0,  LOG_NORMAL },
    { "x",              build_list,     POS_STANDING,   0,  LOG_NORMAL },
    { "commands",   build_commands, POS_STANDING,   0,  LOG_NORMAL },
    { "showarea",       build_showarea, POS_STANDING,   0,  LOG_NORMAL },
    { "showobject",     build_showobj,  POS_STANDING,   0,  LOG_NORMAL },
    { "findarea",       build_findarea, POS_STANDING,   0,  LOG_NORMAL },
    { "findobject",     build_findobject,POS_STANDING,  0,  LOG_NORMAL },
    { "help",           build_help,     POS_STANDING,   0,  LOG_NORMAL },
    { "helpedit",   build_helpedit, POS_STANDING,   0,  LOG_NORMAL },
    { "set",            build_set,      POS_STANDING,   0,  LOG_NORMAL },
    { "setarea",        build_setarea,  POS_STANDING,MAX_LEVEL,LOG_NORMAL },
    { "stop",           build_stop,     POS_STANDING,   0,  LOG_ALWAYS },
    { "addobject",      build_addobject,POS_STANDING,   0,  LOG_NORMAL },
    { "delobjec",       build_delwarn,  POS_STANDING,   0,  LOG_NORMAL },
    { "delobject",      build_delobject,POS_STANDING,   0,  LOG_NORMAL },
    { "delhelp",    build_delhelp,  POS_STANDING,   84, LOG_ALWAYS },
    { "findhelp",   build_findhelp, POS_STANDING,   0,  LOG_NORMAL },
    { "addhelp",    build_addhelp,   POS_STANDING,  0,  LOG_NORMAL },
    { "oedit",          build_set_oedit, POS_STANDING,  0,  LOG_NORMAL },
    { "bedit",          build_set_bedit, POS_STANDING,  0,  LOG_NORMAL },
    { "nedit",      build_set_nedit, POS_STANDING, 0, LOG_NORMAL },
    { "setvnum",        build_setvnum,   POS_STANDING,  0,  LOG_NORMAL },
    { "vset",       build_setvnum,   POS_STANDING,  0,  LOG_NORMAL },
    { "uobjs",      build_uobjs,     POS_STANDING,  0,  LOG_NORMAL },
    { "values",         build_listvalues,POS_STANDING,  0,  LOG_NORMAL },
    { "buildings",      build_listbuildings,POS_STANDING, 0,  LOG_NORMAL },
    { "addbuilding",    build_addbuilding,   POS_STANDING,  0,  LOG_NORMAL },
    { "check_area",     do_check_area,   POS_STANDING,MAX_LEVEL+1,LOG_NORMAL},
    { "check_areas",    do_check_areas,  POS_STANDING,MAX_LEVEL+1,LOG_ALWAYS},
    { "clone",      build_clone,    POS_STANDING,   0,  LOG_NORMAL },
    { "say",        do_say,     POS_STANDING,   0,  LOG_NORMAL },
    { "areasave",   do_areasave,    POS_STANDING,   0,  LOG_NORMAL },
    /*
     * End of list.
     */
    { "",               0,              POS_DEAD,        0,  LOG_NORMAL }

};

/* Building memory management
 * Using the linked list approach for various sized bits.
 * Store array of sizes, pointing to linked list.
 */

#define MAX_MEM_SIZES 20

int     build_freesize[MAX_MEM_SIZES];
void *  build_freepointer[MAX_MEM_SIZES];
int     build_numsizes=0;

/* String function */
/* moved build_strdup to merc.h - Stephen */
char * build_simpstrdup(char *);
void build_editstr(char * * dest, char * src, CHAR_DATA * ch);
void build_finishedstr( char * orig, char * * dest,CHAR_DATA * ch,bool saved);

/* Variables declared in db.c, which we need */

extern OBJ_INDEX_DATA *        obj_index_hash          [MAX_KEY_HASH];
extern ROOM_INDEX_DATA *       room_index_hash         [MAX_KEY_HASH];
extern char *                  string_hash             [MAX_KEY_HASH];

extern char *                  string_space;
extern char *                  top_string;
extern char                    str_empty       [1];

extern int                     top_ed;
extern int                     top_help;
extern int                     top_obj_index;
extern int                     top_room;

extern HELP_DATA *      help_last;
extern AREA_DATA *      area_last;
extern AREA_DATA *      area_first;
#define                 MAX_PERM_BLOCK  131072
extern int                     nAllocString;
extern int                     sAllocString;
extern int                     nAllocPerm;
extern int                     sAllocPerm;
/* extern int                     fBootDb;   */

/* local functions */
char * build_docount(int *);
char * reset_to_text(BUILD_DATA_LIST **,int *);

ROOM_INDEX_DATA *  new_room( AREA_DATA * pArea, sh_int vnum )
{
    ROOM_INDEX_DATA * pRoomIndex;

    GET_FREE(pRoomIndex, rid_free);
    pRoomIndex->area                = pArea;
    pRoomIndex->vnum                = vnum;
    return pRoomIndex;
}

void build_interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    char buffer[MAX_INPUT_LENGTH];
    int cmd;
    bool found;

    /*
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
        argument++;
    if ( argument[0] == '\0' )
        return;

    /*
     * Grab the command word.
     * Special parsing so ' can be a command,
     *   also no spaces needed after punctuation.
     */
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) )
    {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;
        while ( isspace(*argument) )
            argument++;
    }
    else
    {
        argument = one_argument( argument, command );
    }

    /*
     * Look for command in command table.
     */
    found = FALSE;
    for ( cmd = 0; build_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == build_cmd_table[cmd].name[0]
            &&   !str_prefix( command, build_cmd_table[cmd].name )
            &&   get_trust(ch) >= build_cmd_table[cmd].level )
        {
            found = TRUE;
            break;
        }
    }

    /*
     * Log and snoop.
     */
    if ( build_cmd_table[cmd].log == LOG_NEVER )
        strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
        ||   fLogAll
        ||   build_cmd_table[cmd].log == LOG_ALWAYS )
    {
        sprintf( log_buf, "Log %s: %s", ch->name, logline );
        log_string( log_buf );
        monitor_chan( ch, log_buf, MONITOR_BUILD );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
        write_to_buffer( ch->desc->snoop_by, "% ",    2 );
        write_to_buffer( ch->desc->snoop_by, logline, 0 );
        write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
        sprintf(buffer,"%s %s",command,argument);
        build_set(ch,buffer);
        return;
    }

    /*
     * Dispatch the command.
     */
    (*build_cmd_table[cmd].do_fun) ( ch, argument );

    /* make sure that if in Redit, vnum is at new room. */
    if ( ch->act_build == ACT_BUILD_REDIT && ch->build_vnum != ch->in_room->vnum )
    {
        ch->build_vnum = ch->in_room->vnum;
        send_to_char( "Redit: Vnum changed to new room.\n\r", ch );
    }

    tail_chain( );
    return;
}

/* -S- Addition: */
void build_commands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char out[MAX_STRING_LENGTH];
    int cmd;
    int col = 0;

    sprintf( out, "Building / Editing Commands Available to You:\n\r" );

    for ( cmd = 0; build_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( build_cmd_table[cmd].level > get_trust( ch ) )
            continue;
        sprintf( buf, "%-20s    ", build_cmd_table[cmd].name );
        safe_strcat( MSL, out, buf );
        if ( ++col % 3 == 0 )
            safe_strcat( MSL, out, "\n\r" );
    }
    safe_strcat( MSL, out, "\n\r" );
    send_to_char( out, ch );
    return;
}

void build_showobj( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *obj;
    int cnt;
    char *foo = NULL;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Show what object?\n\r", ch );
        return;
    }

    if ( !is_number(arg) )
    {
        send_to_char( "must be a vnum.\n\r", ch );
        return;
    }

    buf1[0] = '\0';

    if ( ( obj = get_obj_index( atoi(arg) ) ) == NULL )
    {
        send_to_char( "Object vnum not found.\n\r", ch );
        return;
    }

    sprintf( buf, "\n\r@@WName: @@y%s  @@WLevel: @@y%d.\n\r",      obj->name, obj->level );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "@@WVnum: @@y%d.  @@WType: @@y%s.\n\r",
        obj->vnum, tab_item_types[ (obj->item_type)-1 ].text );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "@@WShort description: @@y%s.\n\r@@WLong description: @@y%s\n\r",
        obj->short_descr, obj->description );
    safe_strcat( MSL, buf1, buf );

    /*
    sprintf( buf, "@@WItem type: @@y%s.\n\r",rev_table_lookup(tab_item_types,obj->item_type));
    safe_strcat( MSL, buf1, buf);
    */

    sprintf( buf, "@@WWear bits: @@y%s\n\r",
        bit_table_lookup(tab_wear_flags,obj->wear_flags) );

    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "@@WExtra bits: @@y%s\n\r",
        bit_table_lookup(tab_obj_flags,obj->extra_flags) );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "@@WWeight: @@y%d.\n\r", obj->weight );
    sprintf( buf+strlen(buf), "@@WHeat: @@y%d.\n\r", obj->heat );
    sprintf( buf+strlen(buf), "@@WBuilding: @@y%s.\n\r", (obj->building<0||obj->building>=MAX_BUILDING)?"Unknown":build_table[obj->building].name );
    sprintf( buf+strlen(buf), "@@WImage: @@y%s.\n\r", obj->image );
    safe_strcat( MSL, buf1, buf );

    if ( obj->image && ch->desc->mxp && IS_SET(ch->config,CONFIG_IMAGE) )
    {
        sprintf( buf, "\e[1z" );
        sprintf( buf+strlen(buf), MXPTAG(ch->desc,"IMAGE %s ALIGN=Bottom"), obj->image );
        sprintf( buf+strlen(buf), "  " );
        safe_strcat(MAX_STRING_LENGTH, buf1, buf );
    }

    safe_strcat( MSL, buf1, "@@WObject Values:\n\r" );

    for ( cnt = 0; cnt < MAX_OBJECT_VALUES; cnt++ )
    {
        sprintf( buf, "@@W[Value%-2d: @@y%6d@@W] %s",
            cnt, obj->value[cnt],
            rev_table_lookup( tab_value_meanings,  (obj->item_type * MAX_OBJECT_VALUES ) + cnt  ) );
        safe_strcat( MSL, buf1, buf );
        if ( is_name( "ArmorType", rev_table_lookup( tab_value_meanings, ( obj->item_type * MAX_OBJECT_VALUES ) + cnt ) ) )
        {
            foo = ( obj->value[cnt] == -2 ) ? "All" : ( obj->value[cnt] == 1 ) ? "Bulletproof" : (obj->value[cnt] == 2) ? "Blastproof" : (obj->value[cnt] == 3 ) ? "Acidproof" : ( obj->value[cnt] == 4 ) ? "Flameproof" : (obj->value[cnt]==5) ? "Laserproof" : (obj->value[cnt] == -1) ? "General" : "INVALID!";
            if ( foo[0] == '\0' )
                sprintf( buf, "                  @@R(INVALID!)@@g\n\r" );
            else
                sprintf( buf, "                  @@y(%s)@@g\n\r", foo );
        }
        if ( !str_cmp( "Main Type", rev_table_lookup( tab_value_meanings, ( obj->item_type * MAX_OBJECT_VALUES ) + cnt ) ) )
        {
            foo = ( obj->value[cnt] == -2 ) ? "All" : ( obj->value[cnt] == 1 ) ? "Bulletproof" : (obj->value[cnt] == 2) ? "Blastproof" : (obj->value[cnt] == 3 ) ? "Acidproof" : ( obj->value[cnt] == 4 ) ? "Flameproof" : (obj->value[cnt]==5) ? "Laserproof" : (obj->value[cnt] == -1) ? "General" : "INVALID!";
            if ( foo[0] == '\0' )
                sprintf( buf, "                  @@R(INVALID!)@@g\n\r" );
            else
                sprintf( buf, "                  @@y(%s)@@g\n\r", foo );
        }
        else if ( is_name( "Building", rev_table_lookup( tab_value_meanings, ( obj->item_type * MAX_OBJECT_VALUES ) + cnt ) ) )
        {
            sprintf( buf, "                  @@y(%s)@@g\n\r", (obj->value[cnt]<MAX_BUILDING && obj->value[cnt]>0) ? build_table[obj->value[cnt]].name : "Invalid" );
        }
        else if ( is_name( "AmmoType", rev_table_lookup( tab_value_meanings, ( obj->item_type * MAX_OBJECT_VALUES ) + cnt ) ) )
        {
            sprintf( buf, "                  @@y(%s)@@g\n\r", (obj->value[cnt]>= 0 && obj->value[cnt]<MAX_AMMO) ? clip_table[obj->value[cnt]].name : "Invalid" );
        }
        else if ( is_name( "InstType", rev_table_lookup( tab_value_meanings, ( obj->item_type * MAX_OBJECT_VALUES ) + cnt ) ) )
        {
            foo = ( IS_SET(obj->value[cnt],INST_GPS) ) ? "GPS" : (IS_SET(obj->value[cnt],INST_REFLECTOR)) ? " Reflector" : (IS_SET(obj->value[cnt],INST_INTERN_DEF)) ? "Internal Defenses" : (IS_SET(obj->value[cnt],INST_SATELLITE_UPLINK)) ? "Satellite Uplink" : (IS_SET(obj->value[cnt],INST_LASER_AIMS)) ? "Laser Aims" : (IS_SET(obj->value[cnt],INST_SAFEHOUSE)) ? "Safehouse" : (IS_SET(obj->value[cnt],INST_PULSE_NEUTRALIZER)) ? "Pulse Neutralizer" : (IS_SET(obj->value[cnt],INST_DEPLEATED_URANIUM)) ? "Depleated Uranium" : (IS_SET(obj->value[cnt],INST_RESOURCE_PURIFIER)) ? "Resource Purifier" : (IS_SET(obj->value[cnt],INST_ANTIVIRUS))? "Antivirus" : (IS_SET(obj->value[cnt],INST_FIREWALL)) ? "Firewall" : (IS_SET(obj->value[cnt],INST_PROCESSOR_UPGRADE)) ? "Processor Upgrade" :(IS_SET(obj->value[cnt],INST_ORGANIC_CORE))?"Organic Core":(IS_SET(obj->value[cnt],INST_VIRAL_ENHANCER) ) ? "Viral Enhancer" : (IS_SET(obj->value[cnt],INST_ALIEN_TECHNOLOGY))?"Alien Technology":"INVALID";
            if ( foo[0] == '\0' )
                sprintf( buf, "                  @@R(INVALID!)@@g\n\r" );
            else
                sprintf( buf, "                  @@y(%s)@@g\n\r", foo );
        }
        else if ( is_name( "Suit", rev_table_lookup( tab_value_meanings, ( obj->item_type * MAX_OBJECT_VALUES ) + cnt ) ) )
        {
            foo = ( obj->value[cnt] == 1 ) ? "Warp" : (obj->value[cnt] == 2) ? "Ally-Jump" : "INVALID!";
            if ( foo[0] == '\0' )
                sprintf( buf, "                  @@R(INVALID!)@@g\n\r" );
            else
                sprintf( buf, "                  @@y(%s)@@g\n\r", foo );
        }
        else if ( is_name( "Vehicle", rev_table_lookup( tab_value_meanings, ( obj->item_type * MAX_OBJECT_VALUES ) + cnt ) ) )
        {
            if ( obj->value[cnt] > 0 && obj->value[cnt] < MAX_VEHICLE )
                foo = vehicle_desc[obj->value[cnt]];
            else
                foo = "INVALID!";
            if ( foo[0] == '\0' )
                sprintf( buf, "                  @@R(INVALID!)@@g\n\r" );
            else
                sprintf( buf, "                  @@y(%s)@@g\n\r", foo );
        }
        else
            sprintf( buf, "@@g\n\r" );
        safe_strcat( MSL, buf1, buf );
    }

    send_to_char( buf1, ch );
    if (obj->item_type == ITEM_WEAPON && (obj->value[cnt]>= 0 && obj->value[cnt]<MAX_AMMO))
    {
        sprintf( buf, "\n\r@@W%-10s  PDam: %d   BDam: %d   Speed: %d\n\r            Hit %%: %d   Explodes: %d@@g\n\r",
            clip_table[obj->value[2]].name,
            clip_table[obj->value[2]].dam+obj->value[7],
            clip_table[obj->value[2]].builddam+obj->value[8],
            clip_table[obj->value[2]].speed+obj->value[9],
            clip_table[obj->value[2]].miss+obj->value[10],
            clip_table[obj->value[2]].explode
            );
        send_to_char(buf,ch);
    }

    return;
}

#define DISPLAY_BRIEFDOORS  1
#define DISPLAY_RESETS      2
#define DISPLAY_FULLDOORS   4
#define DISPLAY_DESC        8

char * build_docount(int * pcount)
{
    static char buf[20];

    buf[0] = '\0';
    if (pcount != NULL)
        sprintf(buf,"%d) ",++*pcount);
    else
        buf[0]='\0';
    return buf;
}

void build_findobject( CHAR_DATA *ch, char * argument)
{
    /*    extern int top_obj_index; Unused Var */
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    BUILD_DATA_LIST *pList;
    AREA_DATA * Area;
    int nMatch;
    bool fAll;
    bool found;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Find what object?\n\r", ch );
        return;
    }

    if ( !(Area=ch->in_room->area) )
    {
        send_to_char( "Don't know what area you're in.\n\r", ch);
        return;
    }

    if (!build_canread(Area,ch,1))
        return;

    if ( !(pList=Area->first_area_object) )
    {
        send_to_char( "No objects in this area.\n\r",ch);
        return;
    }

    buf1[0] = '\0';
    fAll        = !str_cmp( arg, "all" );
    found       = FALSE;
    nMatch      = 0;

    for ( ; pList != NULL; pList=pList->next )
    {
        pObjIndex = pList->data;
        nMatch++;
        if ( fAll || is_name( arg, pObjIndex->name ) )
        {
            found = TRUE;
            sprintf( buf, "[%5d] %s\n\r",
                pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
            safe_strcat( MSL, buf1, buf );
        }
    }

    if ( !found )
    {
        send_to_char( "Nothing like that in the area.\n\r", ch );
        return;
    }

    send_to_char( buf1, ch );
    return;
}

void build_setobject( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    char * argn;
    OBJ_INDEX_DATA * pObj;
    int value,num;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: [set] <field>  <value>\n\r",         ch );
        send_to_char( "or:     [set] <string> <value>\n\r",         ch );
        send_to_char( "\n\r",                                           ch );
        send_to_char( "Field being one of:\n\r",                        ch );
        send_to_char( "  value0 value1 value2 value3\n\r",              ch );
        send_to_char( "  level extra wear weight heat type\n\r",        ch );
        send_to_char( "\n\r",                                           ch );
        send_to_char( "String being one of:\n\r",                       ch );
        send_to_char( "  name short long ed objfun\n\r",                ch );
        send_to_char( "Use [set] objfun - to clear objfun.\n\r",    ch );
        return;
    }

    if ( ( pObj = get_obj_index( atoi(arg1) ) ) == NULL )
    {
        send_to_char( "Vnum not found.\n\r", ch );
        return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atol( arg3 );

    /*
     * Set something.
     */
    if ( !str_prefix("value",arg2))
    {
        //	num=arg2[5]-'0';
        num = atoi(arg2+5);
        if (num<0 || num>=MAX_OBJECT_VALUES)
        {
            sprintf( buf, "Value%d: ", num );
            send_to_char(buf,ch);
            send_to_char("Value0-14.\n\r",ch);
            return;
        }
        pObj->value[num]=value;
        return;
    }
    if ( !str_cmp( arg2, "level" ) )
    {
        if ( value < 1 || value > 120 )
        {
            send_to_char( "item level is 1 to 120.\n\r", ch );
            return;
        }
        pObj->level = value;
        return;
    }

    if ( UPPER(arg2[0])=='V' && arg2[2]=='\0')
    {
        //	num=arg2[1]-'0';
        num = atoi(arg2+1);
        if ( num<0 || num>=MAX_OBJECT_VALUES)
        {
            send_to_char("v0-v14.\n\r",ch);
            return;
        }
        pObj->value[num]=value;
        return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {

        num=1;
        argn=arg3;
        /*
        if (argn[0]=='+')
        {
         num=1;
         argn++;
        }
        if (argn[0]=='-')
        {
         num=0;
         argn++;
        }
        */

        value= multi_table_lookup(tab_obj_flags,argn);
        if (value==0)
        {
            sprintf(buf,"Values for extra flags are +/- :\n\r");
            table_printout(tab_obj_flags,buf+strlen(buf));
            send_to_char(buf,ch);
            return;
        }
        pObj->extra_flags ^= value;
        sprintf( buf, "New values for @@WExtra bits: @@y%s. \n\r",
            bit_table_lookup( tab_obj_flags, pObj->extra_flags ) );
        send_to_char( buf, ch );
        /*
        if (num==1)
         SET_BIT(pObj->extra_flags,value);
        else
         REMOVE_BIT(pObj->extra_flags,value);
        */
        return;
    }

    if ( !str_cmp( arg2, "wear" ) )
    {

        num=1;
        argn=arg3;
        /*
        if (argn[0]=='+')
        {
         num=1;
         argn++;
        }
        if (argn[0]=='-')
        {
         num=0;
         argn++;
        }
        */

        value = multi_table_lookup(tab_wear_flags,argn);

        if (value==0)
        {
            sprintf(buf,"Values for wear flags are +/- :\n\r");
            table_printout(tab_wear_flags,buf+strlen(buf));
            send_to_char(buf,ch);
            return;
        }
        pObj->wear_flags ^= value;
        sprintf( buf, "New values for @@WWear bits: @@y%s. @@N\n\r",
            bit_table_lookup( tab_wear_flags, pObj->wear_flags ) );
        send_to_char( buf, ch );
        /*
        if (num==1)
        SET_BIT(pObj->wear_flags,value);
        else
        REMOVE_BIT(pObj->wear_flags,value);
        */
        return;

    }

    if ( !str_cmp( arg2, "type" ) )
    {
        value=table_lookup(tab_item_types,arg3);
        if (value==0)
        {
            sprintf(buf,"Values for item types are :\n\r");
            table_printout(tab_item_types,buf+strlen(buf));
            send_to_char(buf,ch);
            return;
        }

        pObj->item_type=value;
        return;
    }

    if ( !str_cmp( arg2, "weight" ) )
    {
        pObj->weight = value;
        return;
    }
    if ( !str_cmp( arg2, "heat" ) )
    {
        pObj->heat = value;
        return;
    }
    if ( !str_cmp( arg2, "building" ) )
    {
        if ( !is_number(arg3) )
        {
            int i;
            for ( i=0;i<MAX_BUILDING;i++ )
            {
                if ( !str_cmp(build_table[i].name,arg3) )
                {
                    value=i;
                }
            }
        }
        if ( value < 0 || value >= MAX_BUILDING )
        {
            send_to_char( "No such building.\n\r", ch );
            return;
        }
        pObj->building = value;
        return;
    }

    if ( !str_cmp( arg2, "image" ) )
    {
        if ( !str_cmp(arg3,"remove") )
        {
            free_string(pObj->image);
            pObj->image = NULL;
            return;
        }
        build_strdup(&pObj->image,arg3,TRUE,ch);
        return;
    }
    if ( !str_cmp( arg2, "name" ) )
    {
        build_strdup(&pObj->name,arg3,TRUE,ch);
        return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
        build_strdup(&pObj->short_descr,arg3,TRUE,ch);
        return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
        build_strdup(&pObj->description,arg3,TRUE,ch);
        return;
    }

    /*
     * Generate usage message.
     */
    build_setobject( ch, "" );
    return;
}

void build_stop( CHAR_DATA *ch, char *argument )
{
    ch->position=POS_STANDING;
    send_to_char( "Building stopped.\n\r", ch );
}

void do_build( CHAR_DATA *ch, char *argument )
{
    if ( ch->trust <= LEVEL_IMMORTAL )
    {
        if ( IS_SET(ch->act, PLR_BUILDER) )
            REMOVE_BIT(ch->act, PLR_BUILDER);
    }
    if ( !IS_SET( ch->act, PLR_BUILDER ) )
    {
        send_to_char( "You aren't allowed to build!\n\r", ch );
        return;
    }
    /* Test for BUILD MONITOR LOG */
    sprintf( log_buf, "Log: %s has entered the Builder.", ch->name);
    monitor_chan( ch, log_buf, MONITOR_BUILD );

    ch->position=POS_BUILDING;
    do_help( ch, "build_bmotd" );                           /* motd for builders -S- */
    send_to_char("Building commands are now operative. Type stop to stop building.\n\r",ch);
}

void build_addobject( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buffer[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA * pObjIndex;
    AREA_DATA * pArea;
    BUILD_DATA_LIST *pList;
    int vnum;
    int iHash;
    int looper;
    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char( "Syntax: addobject <vnum> <name>\n\r",     ch );
        return;
    }

    vnum=is_number(arg1) ? atoi(arg1) : -1;

    if (vnum<0 || vnum > 32767 )
    {
        send_to_char( "Vnum must be between 0 and 32767.\n\r",ch);
        return;
    }

    /*    if ( get_trust(ch) <= 81 && (vnum<1101 || vnum>1199) )
        {
        send_to_char( "Your vnums are 1101 to 1199.\n\r", ch );
        return;
        }
    */
    if (get_obj_index(vnum) != NULL)
    {
        send_to_char( "There is already an object with that vnum.\n\r",ch);
        return;
    }

    pArea=ch->in_room->area;

    if (!build_canwrite(pArea,ch,1))
        return;

    if (vnum < pArea->min_vnum || vnum > pArea->max_vnum)
    {
        sprintf(buffer,"Vnum must be between %i and %i.\n\r",pArea->min_vnum, pArea->max_vnum);
        send_to_char(buffer,ch);
        return;
    }

    ch->build_vnum = vnum;
    ch->act_build = ACT_BUILD_OEDIT;

    GET_FREE(pObjIndex, oid_free);
    pObjIndex->vnum                 = vnum;
    pObjIndex->name                 = str_dup(arg2);
    pObjIndex->short_descr          = &str_empty[0];
    pObjIndex->description          = &str_empty[0];
    pObjIndex->owner                = &str_empty[0];
    pObjIndex->image                = &str_empty[0];
    pObjIndex->level                = 1;
    pObjIndex->item_type            = 1;
    pObjIndex->extra_flags          = 0;
    pObjIndex->wear_flags           = 0;
    for ( looper = 0; looper < 10; looper++ )
        pObjIndex->value[looper]             = 0;
    pObjIndex->weight               = 1;
    pObjIndex->heat               = 1;
    pObjIndex->building        = -1;

    iHash                   = vnum % MAX_KEY_HASH;
    SING_TOPLINK(pObjIndex, obj_index_hash[iHash], next);

    GET_FREE(pList, build_free);
    pList->data     = pObjIndex;
    LINK(pList, pArea->first_area_object, pArea->last_area_object,
        next, prev);

    top_obj_index++;

    return;
}

int old_ovnum=0;

void build_delwarn( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You must spell it out completely.\n\r", ch);
    return;
}

void build_delobject( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    AREA_DATA * pArea;
    CHAR_DATA *vch;

    OBJ_INDEX_DATA * pObjIndex;
    BUILD_DATA_LIST * pList;
    int vnum;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    strcpy( arg2, argument );

    if ( arg1[0] == '\0')
    {
        send_to_char( "Syntax: delobject <vnum>\n\r",     ch );
        return;
    }

    if (is_number(arg1))
    {
        vnum=atoi(arg1);
        if (! (pObjIndex = get_obj_index(vnum)))
        {
            send_to_char( "Vnum not found.\n\r", ch);
            return;
        }

        old_ovnum=vnum;

        sprintf(buf, "Are you sure you want to delete object: [%d] %s?\n\r",vnum,pObjIndex->name);
        strcat(buf,"Type delobject ok if you are sure.\n\r");
        send_to_char(buf,ch);
        return;
    }

    if ( str_cmp(arg1,"ok"))                                /* arg1 is NOT ok. */
    {
        /* Usage message */
        build_delobject( ch, "");
        return;
    }

    if ( old_ovnum == 0)
    {
        send_to_char( "First specify a object number.\n\r",ch);
        return;
    }

    /* make sure that NO one else has build_vnum set to this room!! */
    for ( vch = first_char; vch != NULL; vch = vch->next )
        if ( vch->build_vnum == old_ovnum )
            vch->build_vnum = -1;

    /* Now do deletion *gulp* */
    vnum=old_ovnum;
    old_ovnum=0;
    pObjIndex = get_obj_index(vnum);
    pArea = ch->in_room->area;

    for ( pList = pArea->first_area_object; pList; pList = pList->next )
        if ( pList->data == pObjIndex )
            break;
            {
                UNLINK(pList, pArea->first_area_object, pArea->last_area_object,
                    next, prev);
                PUT_FREE(pList, build_free);
            }

    /* Remove object from vnum hash table */
    {
        int iHash;

        iHash                   = vnum % MAX_KEY_HASH;
        SING_UNLINK(pObjIndex, obj_index_hash[iHash], next, OBJ_INDEX_DATA);
    }

    /* Now delete structure */
    PUT_FREE(pObjIndex, oid_free);

    top_obj_index--;

    send_to_char("Done.\n\r",ch);
    return;
}

void build_help(CHAR_DATA * ch,char * argument)
{
    char buf[MAX_STRING_LENGTH];
    HELP_DATA *pHelp;

    if ( argument[0] != '\0' )                              /* If an argument supplied... */
    {
        sprintf( buf, "BUILD_%s", argument );               /* Format text to send */
        for ( pHelp = first_help; pHelp != NULL; pHelp = pHelp->next )
        {
            if ( pHelp->level > get_trust( ch ) )
                continue;

            if ( is_name( buf, pHelp->keyword ) )
            {
                do_help( ch, buf );                         /* Try and find BUILD_<helpname> in helps */
                return;
            }
        }
        do_help( ch, argument );
        return;
    }

    do_help( ch, "build_summary" );
    return;
}

char * build_simpstrdup( char * buf)
{
    char * rvalue;

    build_strdup(&rvalue,buf,FALSE,NULL);
    return rvalue;
}

#define STRING_FILE_DIR "temp/"

/* spec- rewritten to work correctly with SSM */

void build_strdup( char * * dest, char * src, bool freesrc, CHAR_DATA * ch)
{
    /* Does the same as fread_string plus more, if there is enough memory. */
    FILE *infile;
    char *filechar;
    char filename[255];
    char *old_destp;
    char *out;
    char buf[MSL];

    if (src[0]=='$')                                        /* Special functions */
    {
        src++;
        if (src[0]!='$')
        {
            /* Check for edit, new, clear */
            if (is_name(src,"edit new clear") && ch != NULL)
            {
                if (! str_cmp(src,"clear") )
                {
                    if (freesrc && (*dest)!=NULL)
                        free_string(*dest);
                    *dest=&str_empty[0];
                    return;
                }

                if ( (!str_cmp(src,"edit")) && freesrc )
                {
                    old_destp=*dest;
                    build_editstr(dest,*dest,ch);
                    if ( (old_destp) != NULL)
                        free_string(old_destp);
                    return;
                }

                /* If clear, or freesrc is FALSE, start with a blank sheet. */
                build_editstr(dest,"",ch);
                return;
            }

            if (freesrc && (*dest)!=NULL)
                free_string(*dest);

            /* Read in a file */
            fclose( fpReserve );
            filename[0]='\0';
            safe_strcat( MSL, filename,STRING_FILE_DIR);
            safe_strcat( MSL, filename,src);
            infile=fopen(filename,"r");
            if (!infile)
                filechar=str_dup("Could not open file.\n\r");
            else
            {
                /*       fBootDb=1; */
                /* spec- we can't do the fBootDb thing, since SSM has already
                 * freed the hash table
                 */
                filechar=fread_string(infile);
                /*       fBootDb=0; */
            }
            fpReserve=fopen( NULL_FILE, "r");
            *dest=filechar;
            return;
        }
    }

    if (freesrc && (*dest)!=NULL)
        free_string(*dest);

    /* spec- call str_dup to do most of the work */

    /* SSM still uses a single string block, so we're relatively safe here */
    if (src >= string_space && src < top_string)
    {
        *dest=str_dup(src);
        return;
    }

    /* spec- rewrite of the ugly fread_string close, to use str_dup */

    /* do literal \n -> LF etc. conversions to a buffer, then str_dup it */
    /* we assume here that the src string is <MSL (should be safe to do so) */

    out=buf;

    while (*src)
    {
        switch (*src)
        {
            default:
                *out++=*src++;
                break;
            case '\n':
                *out++='\n';
                *out++='\r';
                src++;
                break;
            case '\r':
                src++;
                break;
            case '\\':
                switch (*++src)
                {
                    case 'n':
                        *out++='\n';
                        break;
                    case 'r':
                        *out++='\r';
                        break;
                    default:
                        *out++='\\';
                        *out++=*src;
                        break;
                }
                if (*src)                                   /* don't overrun here.. */
                    src++;
        }
    }

    *out=0;
    *dest=str_dup(buf);
}

void build_editstr(char * * dest, char * src, CHAR_DATA * ch)
{
    /* Starts a character editing. write_start sets *dest to the buffer*/
    char * orig;

    orig=str_dup( src );

    send_to_char("Editing string. Type .help for help.\n\r",ch);
    write_start(dest,build_finishedstr,orig,ch);

    if (*dest != &str_empty[0] )
    {
        send_to_char(src,ch);
        safe_strcat( MSL, *dest,src);                       /* Add src string into the buffer */
    }
    else
    {
        free_string(orig);
    }
    return;
}

void build_finishedstr( char * orig, char * * dest, CHAR_DATA * ch, bool saved)
{
    if (!saved)
    {
        *dest=str_dup(orig);
    }

    free_string(orig);
    return;
}

void build_set_oedit( CHAR_DATA *ch, char *argument )
{
    OBJ_INDEX_DATA *obj;

    ch->act_build = ACT_BUILD_OEDIT;
    if ( is_number( argument ) )
    {
        ch->build_vnum = atoi( argument );
        if ( ( obj = get_obj_index( ch->build_vnum) ) == NULL )
            ch->build_vnum = -1;
    }
    else
    {
        int i;
        char arg[MSL];
        bool fAll = FALSE;
        OBJ_INDEX_DATA *pObjIndex;
        argument = one_argument(argument,arg);

        if ( argument[0] == '\0' )
            fAll = TRUE;

        if ( !str_cmp(arg,"next") )
        {
            for (i=ch->build_vnum+1;i<32766;i++ )
            {
                if ( (pObjIndex = get_obj_index(i)) == NULL )
                    continue;
                if ( fAll || is_name( argument, tab_item_types[(pObjIndex->item_type)-1 ].text ) )
                {
                    ch->build_vnum = i;
                    break;
                }
            }
        }
        else
            ch->build_vnum = -1;
    }

    send_to_char( ch->build_vnum == -1 ? "No vnum set.  Use setvnum.\n\r"
        : "Vnum now set. " , ch );
    return;
}

void build_set_redit( CHAR_DATA *ch, char *argument )
{
    ch->build_vnum = ch->in_room->vnum;

    ch->act_build = ACT_BUILD_REDIT;

    send_to_char( "Now in Redit mode.  Vnum set to current room.\n\r", ch );
    return;
}

void build_set_bedit( CHAR_DATA *ch, char *argument )
{
    int i;

    if ( get_trust(ch) < 83 )
    {
        send_to_char( "You may not use the building-edit option.\n\r", ch );
        return;
    }
    ch->act_build = ACT_BUILD_BEDIT;
    ch->build_vnum = -1;
    if ( is_number( argument ) )
    {
        ch->build_vnum = atoi( argument );
        if ( ch->build_vnum > MAX_BUILDING || ch->build_vnum < 1 )
            ch->build_vnum = -1;
    }
    else
    {
        for ( i=0;i<MAX_BUILDING;i++ )
            if ( !str_prefix(argument,build_table[i].name) )
        {
            ch->build_vnum = i;
            break;
        }
    }

    send_to_char( ch->build_vnum == -1 ? "No vnum set.  Use setvnum.\n\r"
        : "Vnum now set. " , ch );
    return;
}

void build_set_nedit( CHAR_DATA *ch, char *argument )
{
    ch->act_build = ACT_BUILD_NOWT;
    ch->build_vnum = -1;

    send_to_char( "You are no longer in ANY editing mode.\n\r", ch );
    return;
}

void build_setvnum( CHAR_DATA *ch, char *argument )
{
    char             buf[MAX_STRING_LENGTH];
    char             buf2[MAX_STRING_LENGTH];
    int              vnum;
    OBJ_INDEX_DATA  *obj;
    /*   ROOM_INDEX_DATA *room; unused */
    bool             found;
    sh_int       inc= 0;

    if ( argument[0] == '\0' )
    {
        send_to_char( "USAGE: setvnum <vnum>, or v n(dec)/m(inc)\n\r", ch );
        return;
    }

    if ( !is_number( argument ) )
    {
        if ( !str_cmp( argument, "m" ) )
            inc = 1;
        else if ( !str_cmp( argument, "n" ) )
            inc = -1;
        else
        {
            send_to_char( "Argument must be numeric. [vnum]\n\r", ch );
            return;
        }
    }
    if ( is_number( argument ) )
        vnum = atoi( argument );
    else
    {

        vnum = ch->build_vnum + inc;
    }

    sprintf( buf, "Current vnum now set to: %d.\n\r", vnum );
    found = TRUE;

    switch ( ch->act_build )
    {
        case ACT_BUILD_OEDIT:
            if ( ( obj = get_obj_index( vnum ) ) == NULL )
            {
                sprintf( buf2, "No object with that vnum exists.  Use addobject first.\n\r" );
                found = FALSE;
            }
            else
                sprintf( buf2, "Object exists: %s\n\r", obj->short_descr );
            break;

        case ACT_BUILD_BEDIT:
            if ( vnum < 0 || vnum >= MAX_BUILDING )
            {
                sprintf( buf2, "No such building.\n\r" );
                found = FALSE;
            }
            else
                sprintf( buf2, "Building exists: %s\n\r", build_table[vnum].name );

            break;

        default:
            sprintf( buf2, "Please set your editing mode first!!\n\r" );

    }

    send_to_char( buf, ch );
    send_to_char( buf2, ch );

    if ( found )
    {
        ch->build_vnum = vnum;
        send_to_char( "New vnum IS set.\n\r", ch );
        build_list( ch, "" );
    }
    else
    {
        send_to_char( "New vnum NOT set - still at old value.\n\r", ch );
    }

    return;
}

void build_list( CHAR_DATA *ch, char *argument )
{
    /* do show obj|mob|room according to ch->act_build -S- */

    char buf[MAX_STRING_LENGTH];
    bool found;

    found = FALSE;

    if ( argument[0] == '\0'|| ( ch->act_build == ACT_BUILD_REDIT
        && is_name( argument, "brief doors resets desc all" ) ) )
    {
        switch ( ch->act_build )
        {
            case ACT_BUILD_NOWT:
                send_to_char( "Not in any editing mode.  Nothing to show!\n\r", ch );
                break;
            case ACT_BUILD_OEDIT:
                if ( ch->build_vnum == -1 )
                    send_to_char( "No vnum has been selected!\n\r", ch );
                else
                {
                    sprintf( buf, "%d", ch->build_vnum );
                    build_showobj( ch, buf );
                }
                break;
            case ACT_BUILD_BEDIT:
                if ( ch->build_vnum == -1 )
                    send_to_char( "No vnum has been selected!\n\r", ch );
                else
                {
                    sprintf( buf, "%d", ch->build_vnum );
                    show_building_info(ch,ch->build_vnum);
                }
                break;
        }
        return;
    }

    if ( ch->act_build == ACT_BUILD_NOWT )
    {
        send_to_char( "You must be in an editing mode first!\n\r", ch );
        return;
    }

    /* Ok, now arg is valid.  See if it applic. to edit mode */

    if ( !strcmp( argument, "flags" ) )
    {
        found = TRUE;
        switch ( ch->act_build )
        {
            case ACT_BUILD_OEDIT:
                sprintf(buf,"Valid object flags are :\n\r");
                wide_table_printout(tab_obj_flags,buf+strlen(buf));
                send_to_char(buf,ch);
                break;
        }
    }

    if ( !strcmp( argument, "types" ) )
    {
        if ( ch->act_build != ACT_BUILD_OEDIT )
        {
            send_to_char( "Only valid when in Oedit mode.\n\r", ch );
            return;
        }
        found = TRUE;
        sprintf(buf,"Valid object types are :\n\r");
        wide_table_printout(tab_item_types,buf+strlen(buf));
        send_to_char(buf,ch);
    }

    if ( !strcmp( argument, "wear" ) )
    {
        if ( ch->act_build != ACT_BUILD_OEDIT )
        {
            send_to_char( "Only valid when in Oedit mode.\n\r", ch );
            return;
        }
        found = TRUE;
        sprintf(buf,"Valid object wear flags are :\n\r");
        wide_table_printout(tab_wear_flags,buf+strlen(buf));
        send_to_char(buf,ch);
    }

    if ( !strcmp( argument, "loc" ) )
    {
        if ( ch->act_build != ACT_BUILD_OEDIT )
        {
            send_to_char( "Only valid when in Oedit mode.\n\r", ch );
            return;
        }
        found = TRUE;
        sprintf(buf,"Valid object wear locations are :\n\r");
        wide_table_printout(tab_wear_loc,buf+strlen(buf));
        send_to_char(buf,ch);
    }

    if ( !found )
    {
        send_to_char( "You may display the following values:\n\r\n\r", ch );
        send_to_char( "Edit Mode:      Values:\n\r----------      -------\n\r", ch );
        send_to_char( "  Oedit         TYPES - object types.\n\r", ch );
        send_to_char( "                FLAGS - object flags.\n\r", ch );
        send_to_char( "                WEAR  - object wear flags.\n\r", ch );
        send_to_char( "                LOC   - object wear locations.\n\r", ch );
        send_to_char( "                AFF   - object affected_by values.\n\r", ch );
        return;
    }
    return;
}

void build_set( CHAR_DATA *ch, char *argument )
{
    /* Call setroom/mob/obj with argument, and vnum, etc. */
    char buf[MAX_STRING_LENGTH];

    if ( ch->build_vnum == -1 )
    {
        send_to_char( "No vnum is set!!\n\r", ch );
        return;
    }
    switch( ch->act_build )
    {
        case ACT_BUILD_OEDIT:
            sprintf( buf, "%d %s", ch->build_vnum, argument );
            build_setobject( ch, buf );
            break;
        case ACT_BUILD_BEDIT:
            sprintf( buf, "%d %s", ch->build_vnum, argument );
            build_setbuilding( ch, buf );
            break;
    }
    return;
}

void build_listvalues( CHAR_DATA *ch, char *argument )
{
    /* Lookup what the 4 values mean for the given object type -S- */
    int value;
    int foo;
    char buf[MAX_STRING_LENGTH];

    if ( argument[0] == '\0' )
    {
        send_to_char( "USAGE: values <item-type-name>\n\r", ch );
        send_to_char( "Eg: values armor\n\r", ch );
        return;
    }

    value=table_lookup(tab_item_types,argument);
    if (value==0)
    {
        sprintf(buf,"Valid object types are :\n\r");
        wide_table_printout(tab_item_types,buf+strlen(buf));
        send_to_char(buf,ch);
        return;
    }

    send_to_char( "Details for value0,...,value3 are:\n\r", ch );
    value*= 10;
    for ( foo = 0; foo < 10; foo++ )
    {
        if ( !str_cmp(rev_table_lookup(tab_value_meanings, value + foo),"Unused" ) )
            continue;
        sprintf( buf, "@@W[Value@@y%d@@W] : @@y%s@@g\n\r",
            foo, rev_table_lookup(tab_value_meanings, value + foo) );
        send_to_char( buf, ch );
    }
    return;
}

void build_listbuildings( CHAR_DATA *ch, char *argument )
{
    int i;
    char buf[MSL];

    for ( i = 1;i<MAX_BUILDING;i++ )
    {
        sprintf( buf, "%s%d. %s\n\r", build_table[i].disabled?"@@e":"@@W",i, build_table[i].name );
        send_to_char( buf, ch );
    }
    return;
}

int get_dir(char dir)
{
    char * temp;
    if (   dir=='\0'
        || (temp=strchr(cDirs,dir))==NULL)
        return -1;

    return temp-cDirs;
}

void    build_uobjs( CHAR_DATA *ch, char *argument )
{
    /* List vnum usage for area... */
    int curvnum;
    char buf[MAX_STRING_LENGTH];
    char free[MAX_STRING_LENGTH];
    char used[MAX_STRING_LENGTH];
    AREA_DATA *area;
    int last = 0;                                           /* 0 = start, 1 = used, 2 = free */
    int foo = 0;                                            /* holds start of free/used vnums, so no 3001-3001 */

    area=ch->in_room->area;
    /* Rooms */
    sprintf( free, "(Free) " );
    sprintf( used, "(Used) " );
    for ( curvnum = area->min_vnum; curvnum < area->max_vnum; curvnum ++ )
    {
        if ( get_obj_index( curvnum ) != NULL )
        {
            switch( last )
            {
                case 0:
                    sprintf( buf, "%d", curvnum );
                    safe_strcat( MSL, used, buf );
                    foo = curvnum;
                    last = 1;
                    break;
                case 1:
                    break;
                case 2:
                    if ( foo != curvnum-1 )
                    {
                        sprintf( buf, "-%d", curvnum-1 );
                        safe_strcat( MSL, free, buf );
                    }
                    sprintf( buf, " %d", curvnum );
                    safe_strcat( MSL, used, buf );
                    foo = curvnum;
                    last = 1;
            }
        }
        else
        {
            switch( last )
            {
                case 0:
                    sprintf( buf, "%d", curvnum );
                    safe_strcat( MSL, free, buf );
                    foo = curvnum;
                    last = 2;
                    break;
                case 1:
                    if ( foo != curvnum -1 )
                    {
                        sprintf( buf, "-%d", curvnum-1 );
                        safe_strcat( MSL, used, buf );
                    }
                    sprintf( buf, " %d", curvnum );
                    safe_strcat( MSL, free, buf );
                    last =2;
                    foo = curvnum;
                    break;
                case 2:
                    break;
            }
        }
    }

    curvnum = area->max_vnum;
    if ( get_obj_index( curvnum ) != NULL )
    {
        switch( last )
        {
            case 1:
                if ( foo != ( curvnum-1 ) )
                    sprintf( buf, "-%d.", curvnum );
                else
                    sprintf( buf, " %d.", curvnum );
                safe_strcat( MSL, used, buf );
                break;
            case 2:
                if ( foo != curvnum -1 )
                {
                    sprintf( buf, "-%d.", curvnum-1 );
                    safe_strcat( MSL, used, buf );
                }
                sprintf( buf, " %d.", curvnum );
                safe_strcat( MSL, free, buf );
                break;
        }
    }
    else
    {
        switch( last )
        {
            case 1:
                if ( foo != curvnum -1 )
                {
                    sprintf( buf, "-%d.", curvnum-1 );
                    safe_strcat( MSL, used, buf );
                }
                sprintf( buf, " %d.", curvnum );
                safe_strcat( MSL, free, buf );
                break;
            case 2:
                if ( foo != curvnum -1 )
                    sprintf( buf, "-%d.", curvnum );
                else
                    sprintf( buf, " %d.", curvnum );
                safe_strcat( MSL, free, buf );
                break;
        }
    }

    sprintf( buf, "Object vnum usage summary:\n\r\n\r%s\n\r\n\r%s\n\r", used, free );
    send_to_char( buf, ch );
    return;
}

/** Help Editor
    We want to be able to edit ANY help, so 3.bank etc should work,
    in case we have helps with the same keyword(s).
 **/
void build_findhelp( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_STRING_LENGTH];
    int cnt = 0;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Usage: Findhelp <keyword>\n\r", ch );
        return;
    }

    for ( pHelp = first_help; pHelp != NULL; pHelp = pHelp->next )
    {
        if ( is_name( arg, pHelp->keyword ) )
        {
            cnt++;
            sprintf( buf, "[%2d] <%s> \n\r%1.100s\n\r", cnt, pHelp->keyword, pHelp->text );
            send_to_char( buf, ch );
        }
    }
    if ( cnt == 0 )
        send_to_char( "Couldn't find that keyword.\n\r", ch );
    return;
}

void build_helpedit( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    AREA_DATA *area;
    BUILD_DATA_LIST *plist;
    char arg[MAX_STRING_LENGTH];
    int number;
    int count;

    number = number_argument( argument, arg );
    count =0;

    if ( arg[0] == '\0' )
    {
        send_to_char( "Usage: HELPEDIT <keyword>\n\r", ch );
        return;
    }

    /** Now try and find the keyword **/

    for ( pHelp = first_help; pHelp != NULL; pHelp = pHelp->next )
        if ( is_name( arg, pHelp->keyword )
        && ( ++count == number ) )
            break;

    if ( pHelp == NULL )
    {
        send_to_char( "Couldn't find that keyword.\n\r", ch );
        return;
    }

    build_strdup( &pHelp->text, "$edit", TRUE, ch );
    /* Mark the help's area as modified so the help saves... */
    for ( area = first_area; area != NULL; area = area->next )
    {
        if ( area->first_area_help_text != NULL )
        {
            for ( plist = area->first_area_help_text; plist != NULL; plist = plist->next )
            {
                if ( plist->data == pHelp )
                {
                    area_modified( area );
                    break;
                }
            }
        }
    }
    return;
}

void build_addhelp( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    BUILD_DATA_LIST *pList;
    char arg[MAX_STRING_LENGTH];
    int level;
    AREA_DATA *area;
    argument = one_argument( argument, arg );

    if ( !is_number( arg ) || argument[0] == '\0' )
    {
        send_to_char( "Usage: ADDHELP <level> <keyword(s)>.\n\r", ch );
        return;
    }

    level = atoi( arg );

    if ( level < -1 || level > 85 )
    {
        send_to_char( "Level must be between -1 and 85.\n\r", ch );
        return;
    }

    GET_FREE(pHelp, help_free);
    pHelp->level    = level;
    pHelp->keyword  = str_dup( argument );
    pHelp->text     = str_dup( "NEW HELP.  DELETE THIS LINE FIRST!" );

    LINK(pHelp, first_help, last_help, next, prev);
    /* MAG Mod */
    GET_FREE(pList, build_free);
    pList->data     = pHelp;
    /* find helps area, or use system if not set */

    for ( area = first_area; area; area = area->next )
        if ( !str_cmp( area->keyword, "helps" ) )
            break;
    if ( area == NULL )
        area = first_area;
    LINK(pList, area->first_area_help_text,
        area->last_area_help_text, next, prev);

    top_help++;
    send_to_char( "Help added.  Use HELPEDIT <keyword> to edit it.\n\r", ch );
    return;
}

void build_delhelp( CHAR_DATA *ch, char *argument )
{
    HELP_DATA *pHelp;
    AREA_DATA *area;
    BUILD_DATA_LIST *plist;
    char arg[MAX_STRING_LENGTH];
    int number;
    int count;

    number = number_argument( argument, arg );
    count =0;

    if ( arg[0] == '\0' )
    {
        send_to_char( "Usage: DELHELP <keyword>\n\r", ch );
        return;
    }

    /** Now try and find the keyword **/

    for ( pHelp = first_help; pHelp != NULL; pHelp = pHelp->next )
        if ( is_name( arg, pHelp->keyword )
        && ( ++count == number ) )
            break;

    if ( pHelp == NULL )
    {
        send_to_char( "Couldn't find that keyword.\n\r", ch );
        return;
    }

    /* Mark the help's area as modified so the help saves... */
    for ( area = first_area; area != NULL; area = area->next )
    {
        if ( area->first_area_help_text != NULL )
        {
            for ( plist = area->first_area_help_text; plist != NULL; plist = plist->next )
            {
                if ( plist->data == pHelp )
                {
                    send_to_char( "GONE!\n\r", ch );
                    UNLINK(plist, area->first_area_help_text, area->last_area_help_text, next, prev);
                    PUT_FREE(plist, build_free);
                    top_help--;
                    UNLINK( pHelp, first_help, last_help, next, prev);
                    PUT_FREE( pHelp, help_free );
                    area_modified( area );
                    break;
                }
            }
        }
    }
    return;
}

/*
void do_all_help_save()
{

  FILE * fp;
  char help_file_name[MAX_STRING_LENGTH];

  if ( ( fp = fopen( help_file, "w" ) ) == NULL )
  {
    bug( "Save Help Table: fopen", 0 );
    perror( "failed open of helpfile.dat in do_help_save" );
  }
  else
  {

  HELP_DATA *pHelp;
  BUILD_SATA_LIST *Pointer;

  for (pointer = first_help; pointer != NULL, pointer = pointer_next )
  {
    pHelp=Pointer->data;
    fprintf( fp,"%i %s~\n",pHelp->level,pHelp->keyword);

    if (isspace(pHelp->text[0]))
       fprintf( fp,".%s~\n",pHelp->text);
    else
      fprintf(fp,"%s~\n",pHelp->text);

  }
  fflush( fp );
  fclose( fp );
  return;

}

void do_all_help_load()
{

    HELP_DATA *pHelp;
    BUILD_DATA_LIST *pointer;
    FILE * fp;
    char help_file[MAX_STRING_LENGTH];

  if ( ( fp = fopen( help_file, "r" ) ) == NULL )
  {
    bug( "Help Table: fopen", 0 );
    perror( "failed open of helpfile.dat in do_help_load" );
  }

  for ( pointer = first_help; pointer != NULL; pointer = pointer_next )
  {
    GET_FREE(pHelp, help_free);
    pHelp->level    = fread_number( fp );
    pHelp->keyword  = fread_string( fp );
    if ( pHelp->keyword[0] == '$' )
      break;
    pHelp->text     = fread_string( fp );

    LINK(pHelp, first_help, last_help, next, prev);
    top_help++;
  }
  fclose ( fp );
  return;

}
  */
/* NOTE--NEED TO MAKE SURE WE GET MOTD, TOO--I THINK IT WIL BE OKAY ZEN */

void build_clone( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    /*
     * Allow builder to clone a room/mob/object -
     * Takes existing r/m/o and makes new copy with relevant details
     * copied across...
     */

    if ( argument[0] == '\0' )                              /* Show help info */
    {
        send_to_char( "Usage: CLONE <type> <vnum>\n\r", ch );
        send_to_char( "Where <type> is one of: obj\n\r", ch );
        send_to_char( "      <vnum> is the vnum of the type you want to clone.\n\r", ch );
        send_to_char( "[The item will be copied onto the exisiting item you are editing]\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg1 );              /* room/mob/obj     */
    argument = one_argument( argument, arg2 );              /* vnum to clone to */

    /* Check arguments */
    if (    arg1[0] == '\0'
        || arg2[0] == '\0'
        || !is_number( arg2 )
        || !is_name( arg1, "room obj mob" ) )
    {
        build_clone( ch, "" );
        return;
    }

    if ( !str_cmp( arg1, "obj" ) )
    {
        OBJ_INDEX_DATA *obj;
        OBJ_INDEX_DATA *this_obj;
        int looper;
        if ( ch->act_build != ACT_BUILD_OEDIT )
        {
            send_to_char( "You must be in OEDIT mode to clone an object.\n\r", ch );
            return;
        }

        if ( ( this_obj = get_obj_index( ch->build_vnum ) ) == NULL )
        {
            send_to_char( "You must select a valid object in OEDIT before you clone.\n\r", ch );
            return;
        }

        if ( ( obj = get_obj_index( atoi(arg2) ) ) == NULL )
        {
            send_to_char( "That object does not exist to be cloned.\n\r", ch );
            return;
        }

        /* Copy details across... */
        if ( this_obj->name != NULL )
            free_string( this_obj->name );
        this_obj->name        = str_dup( obj->name );
        this_obj->level         = obj->level;
        if ( this_obj->short_descr != NULL )
            free_string( this_obj->short_descr );
        this_obj->short_descr = str_dup( obj->short_descr );
        if ( this_obj->description != NULL )
            free_string( this_obj->description );
        this_obj->description = str_dup( obj->description );
        this_obj->item_type   = obj->item_type;
        this_obj->extra_flags = obj->extra_flags;
        this_obj->wear_flags  = obj->wear_flags;
        for ( looper = 0; looper < MAX_OBJECT_VALUES; looper++ )
            this_obj->value[looper] = obj->value[looper];

        this_obj->weight      = obj->weight;
        this_obj->heat        = obj->heat;
        this_obj->building    = obj->building;
        if ( this_obj->image != NULL )
            free_string(this_obj->image);
        this_obj->image       = str_dup(obj->image);

        send_to_char( "Object cloned.\n\r", ch );
        return;
    }
    return;
}

void build_setbuilding( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    int value,num,type,i;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: [set] <field>  <value>\n\r",         ch );
        send_to_char( "\n\r",                                           ch );
        send_to_char( "Numeric fields:\n\r",                        ch );
        send_to_char( "  hp shield rank iron copper gold\n\r",              ch );
        send_to_char( "  silver rocks sticks logs max\n\r",        ch );
        send_to_char( "\n\r",                                           ch );
        send_to_char( "String fields:\n\r",                       ch );
        send_to_char( "  name symbol requires buildon help\n\r",                ch );
        return;
    }

    if ( ( type = atoi(arg1) ) < 1 || type >= MAX_BUILDING )
    {
        send_to_char( "Illegal building number.\n\r", ch );
        return;
    }

    if ( !str_cmp("reload",arg2))
    {
        send_to_char("Reloading...\n\r", ch );
        load_building_t();
        send_to_char( "Reloaded.\n\r", ch );
        return;
    }
    if ( !str_cmp("save",arg2))
    {
        send_to_char("Saving...\n\r",ch);
        save_building_table();
        send_to_char("Saved.\n\r",ch);
        return;
    }
    if ( !str_cmp("disable",arg2))
    {
        build_table[type].disabled = !build_table[type].disabled;
        if ( build_table[type].disabled )
            send_to_char( "Building disabled!!\n\r", ch );
        else
            send_to_char( "Building re-enabled.\n\r", ch );
        return;
    }
    /*
     * Snarf the value (which need not be numeric).
     */
    if ( arg3[0] == '\0' )
    {
        send_to_char( "You need to provide a new value.\n\r", ch );
        return;
    }
    value = atol( arg3 );

    /*
     * Set something.
     */
    if ( !str_prefix("hp",arg2))
    {
        build_table[type].hp = value;
        return;
    }
    else if ( !str_prefix("shield",arg2))
    {
        build_table[type].shield = value;
        return;
    }
    else if ( !str_prefix("max",arg2))
    {
        build_table[type].max = value;
        return;
    }
    else if ( !str_prefix("iron",arg2))
    {
        build_table[type].resources[0] = value;
        return;
    }
    else if ( !str_prefix("skins",arg2))
    {
        build_table[type].resources[1] = value;
        return;
    }
    else if ( !str_prefix("copper",arg2))
    {
        build_table[type].resources[2] = value;
        return;
    }
    else if ( !str_prefix("gold",arg2))
    {
        build_table[type].resources[3] = value;
        return;
    }
    else if ( !str_prefix("silver",arg2))
    {
        build_table[type].resources[4] = value;
        return;
    }
    else if ( !str_prefix("rocks",arg2))
    {
        build_table[type].resources[5] = value;
        return;
    }
    else if ( !str_prefix("sticks",arg2))
    {
        build_table[type].resources[6] = value;
        return;
    }
    else if ( !str_prefix("logs",arg2))
    {
        build_table[type].resources[7] = value;
        return;
    }
    else if ( !str_prefix( arg2, "rank" ) )
    {
        build_table[type].rank = value;
        return;
    }
    else if ( !str_prefix( arg2, "neutral" ) )
    {
        build_table[type].militairy = !build_table[type].militairy;
        send_to_char( "Toggled.\n\r", ch );
        return;
    }
    else if ( !str_prefix( arg2, "type" ) )
    {
        extern char *building_title[MAX_BUILDING_TYPES];
        char buf[MSL];

        if ( arg3[0] == '\0' )
        {
            for ( i=0;i<MAX_BUILDING_TYPES;i++ )
            {
                sprintf( buf, "%s\n\r", building_title[i] );
                send_to_char(buf,ch);
            }
            return;
        }
        for ( i=0;i<=MAX_BUILDING_TYPES;i++ )
        {
            if ( i >= MAX_BUILDING_TYPES )
                continue;
            if ( !str_prefix(arg3,building_title[i]) )
            {
                value = i;
                break;
            }
        }
        if ( value < 0 || value >= MAX_BUILDING_TYPES )
        {
            send_to_char( "Invalid type.\n\r", ch );
            return;
        }
        build_table[type].act = value;
        return;
    }
    else if ( !str_prefix( arg2, "levelrequirements" ) )
    {
        build_table[type].requirements_l = value;
        return;
    }
    else if ( !str_prefix( arg2, "name" ) )
    {
        free_string(build_table[type].name);
        build_table[type].name = str_dup(strip_out(arg3,"~"));
        return;
    }
    else if ( !str_prefix( arg2, "symbol" ) )
    {
        free_string(build_table[type].symbol);
        build_table[type].symbol = str_dup(strip_out(arg3,"~"));
        return;
    }
    else if ( !str_prefix( arg2, "help" ) )
    {
        free_string(build_help_table[type].help);

        if ( !str_cmp(arg3,"$edit") )
            build_strdup( &build_help_table[type].help, "$edit", TRUE, ch );
        else
            build_help_table[type].help = str_dup(strip_out(arg3,"~"));
        return;
    }
    else if ( !str_prefix( arg2, "desc" ) )
    {
        free_string(build_table[type].desc);

        if ( !str_cmp(arg3,"$edit") )
            build_strdup( &build_table[type].desc, "$edit", TRUE, ch );
        else
            build_table[type].desc = str_dup(strip_out(arg3,"~"));
        return;
    }
    else if ( !str_prefix( arg2, "requires" ) )
    {
        for ( i=0;i<MAX_BUILDING;i++ )
        {
            if ( !str_cmp(build_table[i].name,arg3) )
            {
                if ( i == type )
                {
                    send_to_char( "You can't set it to require itself.\n\r", ch );
                    return;
                }
                build_table[type].requirements = i;
                send_to_char( "Set.\n\r", ch );
                return;
            }
        }
        send_to_char( "No such building.\n\r", ch );
        return;
    }
    else if ( !str_prefix( arg2, "buildon" ) )
    {
        bool ok = FALSE;
        bool ter[SECT_MAX];
        int open = -1;

        for ( num=0;num<SECT_MAX;num++ )
            ter[num] = FALSE;

        for ( i=0;i<SECT_MAX;i++ )
        {
            if ( !str_prefix(arg3,wildmap_table[i].name) )
            {
                ok = TRUE;
                break;
            }
        }
        if ( !ok )
        {
            send_to_char( "No such sector type.\n\r", ch );
            return;
        }
        ok = FALSE;
        for ( num=0;num<MAX_BUILDON;num++ )
        {
            if ( (ter[build_table[type].buildon[num]] || build_table[type].buildon[num] < 0 || build_table[type].buildon[num] > SECT_MAX ) && open == -1 )
                open = num;

            if ( build_table[type].buildon[num] == i )
            {
                build_table[type].buildon[num] = -1;
                if ( !ok )
                {
                    send_to_char( "Sector removed.\n\r", ch  );
                    ok = TRUE;
                }
            }
            if ( build_table[type].buildon[num] >= 0 && build_table[type].buildon[num] < SECT_MAX )
                ter[build_table[type].buildon[num]] = TRUE;
        }
        if ( ok )
            return;
        if ( open == -1 )
        {
            send_to_char( "No open slots, remove at least one sector type to add this one.\n\r", ch );
            return;
        }
        build_table[type].buildon[open] = i;
        send_to_char( "Sector added.\n\r", ch );
        return;
    }
    else if ( !str_cmp( arg2, "change_all_sectors" ) )
    {
        char buf[MSL];
        int oldsect=-1,newsect=-1;
        int changes = 0;

        argument = one_argument(argument,arg3);

        if ( argument[0] == '\0' )
        {
            send_to_char( "Change the sector to what new type?\n\r", ch );
            return;
        }
        for ( i=0;i<SECT_MAX;i++ )
        {
            if ( !str_prefix(arg3,wildmap_table[i].name) && oldsect == -1 )
            {
                oldsect = i;
            }
            else if ( !str_prefix(argument,wildmap_table[i].name) && newsect == -1 )
            {
                newsect = i;
            }
        }
        if ( oldsect == -1 || newsect == -1 )
        {
            send_to_char( "One of the sectors you inputted was invalid.\n\r", ch );
            return;
        }
        for ( type = 1;type < MAX_BUILDING;type++ )
        {
            for ( num=0;num<5;num++ )
            {
                if ( build_table[type].buildon[num] == oldsect )
                {
                    build_table[type].buildon[num] = newsect;
                    changes++;
                }
            }
        }
        sprintf ( buf, "%d changes were made.\n\r", changes );
        send_to_char(buf,ch);
        return;
    }
    else if ( !str_cmp( arg2, "calc_all_hp" ) )
    {
        int hp;
        for ( type = 1;type < MAX_BUILDING;type++ )
        {
            hp = 0;
            hp += build_table[type].resources[0] * 5;
            hp += build_table[type].resources[1] * 2;
            hp += build_table[type].resources[2] * 4;
            hp += build_table[type].resources[3] * 6;
            hp += build_table[type].resources[4] * 10;
            hp += build_table[type].resources[5] * 5;
            hp += build_table[type].resources[6] * 2;
            hp += build_table[type].resources[7] * 6;
            if ( hp < 10 )
                hp = 10;
            else
            {
                if ( hp % 10 < 5 )
                    hp -= hp % 10;
                else
                    hp += hp % 10;
            }
            build_table[type].hp = hp;
            build_table[type].shield = hp / 2;
        }
        send_to_char( "Done.\n\r", ch );
        return;
    }

    build_setbuilding(ch,"");
    return;
}

void build_addbuilding( CHAR_DATA *ch, char *argument )
{
    int i=MAX_BUILDING,j;
    if ( MAX_BUILDING >= MAX_POSSIBLE_BUILDING )
    {
        send_to_char( "No more building slots to add.\n\rPlease change MAX_POSSIBLE_BUILDINGS in the code.\n\r", ch );
        return;
    }

    if ( build_table[i].name != NULL)
        free_string(build_table[i].name);
    if ( build_table[i].desc != NULL)
        free_string(build_table[i].desc);
    if ( build_table[i].symbol != NULL)
        free_string(build_table[i].symbol);
    if ( build_help_table[i].help != NULL)
        free_string(build_help_table[i].help);

    build_table[i].type = i;
    build_table[i].name = str_dup(" ");
    build_table[i].symbol = str_dup("[  ]");
    build_table[i].desc = str_dup(" ");
    build_help_table[i].help = str_dup(" ");
    build_table[i].hp = 1;
    build_table[i].shield = 1;
    for ( j=0;j<8;j++ )
        build_table[i].resources[j] = 0;
    for ( j=0;j<MAX_BUILDON;j++ )
        build_table[i].buildon[j] = 1;
    build_table[i].requirements = 1;
    build_table[i].requirements_l = 1;
    build_table[i].militairy = 1;
    build_table[i].rank = 0;
    build_table[i].act = 0;
    build_table[i].max = 100;
    build_table[i].disabled = TRUE;

    ch->build_vnum = i;
    MAX_BUILDING = MAX_BUILDING + 1;
    send_to_char( "Building added.\n\r", ch );
    return;
}
