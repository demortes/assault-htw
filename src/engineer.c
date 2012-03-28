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

/* This file should include all the engineering stuff */

#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2

DECLARE_DO_FUN( do_eng_exit     );
DECLARE_DO_FUN( do_eng_connect      );
DECLARE_DO_FUN( do_ecommands        );
DECLARE_DO_FUN( do_wires        );
DECLARE_DO_FUN( do_eng_check        );
const   struct  cmd_type       eng_cmd_table   [] =
{
    { "north",          do_north,       POS_ENGINEERING,    0,  LOG_NORMAL },
    { "east",           do_east,        POS_ENGINEERING,    0,  LOG_NORMAL },
    { "south",          do_south,       POS_ENGINEERING,    0,  LOG_NORMAL },
    { "west",           do_west,        POS_ENGINEERING,    0,  LOG_NORMAL },
    { "look",           do_look,        POS_ENGINEERING,    0,  LOG_NORMAL },
    { "score",          do_score,       POS_ENGINEERING,    0,  LOG_NORMAL },
    { "tell",           do_tell,        POS_ENGINEERING,    0,  LOG_NORMAL },

    { "help",       do_ecommands,   POS_ENGINEERING,    0,  LOG_NORMAL },
    { "commands",   do_ecommands,   POS_ENGINEERING,    0,  LOG_NORMAL },
    { "who",        do_who,     POS_ENGINEERING,    0,  LOG_NORMAL },
    { "exit",       do_eng_exit,    POS_ENGINEERING,    0,  LOG_NORMAL },
    { "stop",       do_stop,    POS_ENGINEERING,    0,  LOG_NORMAL },
    { "wires",      do_wires,   POS_ENGINEERING,    0,  LOG_NORMAL },
    { "connect",    do_eng_connect, POS_ENGINEERING,    0,  LOG_NORMAL },
    { "check",      do_eng_check,   POS_ENGINEERING,    0,  LOG_NORMAL },

    { "",               0,              POS_DEAD,        0,  LOG_NORMAL }
};

void engineering_interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    bool found;
    OBJ_DATA *obj;

    if ( !ch->in_vehicle )
    {
        ch->position = POS_STANDING;
        return;
    }
    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_TOOLKIT )
    {
        send_to_char( "You have lost your toolkit!\n\r", ch );
        ch->position = POS_STANDING;
        return;
    }

    /* 
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
        argument++;
    if ( argument[0] == '\0' )
        return;

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
    for ( cmd = 0; eng_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == eng_cmd_table[cmd].name[0]
            &&   !str_prefix( command, eng_cmd_table[cmd].name )
            &&   get_trust(ch) >= eng_cmd_table[cmd].level )
        {
            found = TRUE;
            break;
        }
    }

    if ( IS_BUSY(ch) && str_cmp(eng_cmd_table[cmd].name,"stop") )
    {
        send_to_char( "Finish or 'stop' your other process, first.\n\r", ch );
        return;
    }

    /*
     * Log and snoop.
     */
    if ( eng_cmd_table[cmd].log == LOG_NEVER )
        strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( IS_SET(ch->act, PLR_LOG) )
        ||   fLogAll
        ||  eng_cmd_table[cmd].log == LOG_ALWAYS )
    {
        sprintf( log_buf, "Log %s: %s", ch->name, logline );
        log_string( log_buf );
    }
    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
        write_to_buffer( ch->desc->snoop_by, "% ",    2 );
        write_to_buffer( ch->desc->snoop_by, logline, 0 );
        write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }

    if ( !found )
    {
        send_to_char( "Invalid command.\n\r", ch );
        return;
    }

    (*eng_cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}

void do_eng_exit( CHAR_DATA *ch, char *argument )
{
    act( "You stop your work.", ch, NULL, NULL, TO_CHAR );
    ch->position = POS_STANDING;
    return;
}

void do_ecommands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char out[MAX_STRING_LENGTH];
    int cmd;
    int col = 0;

    sprintf( out, "Engineering Commands Available to You:\n\r" );

    for ( cmd = 0; eng_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( eng_cmd_table[cmd].level > get_trust( ch ) )
            continue;
        sprintf( buf, "%-20s    ", eng_cmd_table[cmd].name );
        safe_strcat( MSL, out, buf );
        if ( ++col % 3 == 0 )
            safe_strcat( MSL, out, "\n\r" );
    }
    safe_strcat( MSL, out, "\n\r" );
    send_to_char( out, ch );
    return;
}

void do_eng_connect( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int wire1,wire2;
    char arg[MSL];
    char buf[MSL];

    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_TOOLKIT )
    {
        send_to_char( "You have lost your toolkit!\n\r", ch );
        return;
    }
    argument = one_argument(argument,arg);
    wire1 = 0;
    wire2 = 0;
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char( "Syntax: connect <wire> <wire>\n\rWires can be one of the following: Green, Blue, Red, Yellow.\n\rExample: connect red yellow\n\r", ch );
        return;
    }
    if ( !str_prefix(arg,"green") )
        wire1 = 0;
    else if ( !str_prefix(arg,"blue") )
        wire1 = 1;
    else if ( !str_prefix(arg,"red") )
        wire1 = 2;
    else if ( !str_prefix(arg,"yellow") )
        wire1 = 3;
    else
    {
        send_to_char( "Wire colors are: Green, Blue, Red, Yellow.\n\r", ch );
        return;
    }
    if ( !str_prefix(argument,"green") )
        wire2 = 4;
    else if ( !str_prefix(argument,"blue") )
        wire2 = 5;
    else if ( !str_prefix(argument,"red") )
        wire2 = 6;
    else if ( !str_prefix(argument,"yellow") )
        wire2 = 7;
    else
    {
        send_to_char( "Wire colors are: Green, Blue, Red, Yellow.\n\r", ch );
        return;
    }

    if ( obj->value[wire1] == 0 && obj->value[wire2] == 0 )
    {
        send_to_char( "Those wires have already been connected.\n\r", ch );
        return;
    }
    if ( obj->value[wire1] == obj->value[wire2] )
    {
        send_to_char( "Success! The wires connect.\n\r", ch );
        obj->value[wire1] *= -1;
        obj->value[wire2] *= -1;
    }
    else
    {
        ch->position = POS_STANDING;
        send_to_char( "The wires flickered, and you got ZAPPED!!\n\r", ch );
        damage(ch,ch,250,-1);
        return;
    }
    if ( obj->value[0] <= 0 && obj->value[1] <= 0 && obj->value[2] <= 0 && obj->value[3] <= 0 )
    {
        int i;
        send_to_char( "Success!\n\r", ch );
        if ( ch->section == SECTION_ENGINE_ROOM )
        {
            ch->in_vehicle->speed++;
            sprintf(buf,"Your speed has been increased to %d!\n\r", ch->in_vehicle->speed);
            send_to_char(buf,ch);
        }
        else if ( ch->section == SECTION_CONTROL_ROOM )
        {
            ch->in_vehicle->range++;
            sprintf(buf,"Your combat range has been increased to %d!\n\r", ch->in_vehicle->range);
            send_to_char(buf,ch);
        }
        else if ( ch->section == SECTION_SENSOR_ROOM )
        {
            ch->in_vehicle->scanner++;
            sprintf(buf,"Your scanner range has been increased to %d!\n\r", ch->in_vehicle->scanner);
            send_to_char(buf,ch);
        }
        else if ( ch->section == SECTION_SHIELD_ROOM )
        {
            int x = obj->level;
            ch->in_vehicle->max_hit += x;
            ch->in_vehicle->hit += x;
            sprintf(buf,"Your shields have been increased to %d!\n\r", ch->in_vehicle->max_hit);
            send_to_char(buf,ch);
        }

        for ( i=0;i<8;i++ )
            obj->value[i] = 0;
    }
    return;
}

void do_wires( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    OBJ_DATA *obj;
    int i,w;
    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_TOOLKIT )
    {
        send_to_char( "You must be holding a toolkit in your left hand.\n\r", ch );
        return;
    }
    send_to_char( "\n\r@@xThe wires are connected as follows:\n\r", ch );
    w = -1;
    sprintf( buf, "@@rGreen -----> " );
    if ( obj->value[0] > 0 )
    {
        sprintf(buf+strlen(buf),"Nothing\n\r" );
    }
    else
    {
        for ( i=4;i<8;i++ )
        {
            if ( obj->value[i] == obj->value[0] )
            {
                sprintf( buf+strlen(buf), "%s\n\r", (i==4)?"Green":(i==5)?"Blue":(i==6)?"Red":"Yellow" );
                break;
            }
        }
    }
    w = -1;
    send_to_char(buf,ch);
    sprintf( buf, "@@aBlue ------> " );
    if ( obj->value[1] > 0 )
    {
        sprintf(buf+strlen(buf),"Nothing\n\r" );
    }
    else
    {
        for ( i=4;i<8;i++ )
        {
            if ( obj->value[i] == obj->value[1] )
            {
                sprintf( buf+strlen(buf), "%s\n\r", (i==4)?"Green":(i==5)?"Blue":(i==6)?"Red":"Yellow" );
                break;
            }
        }
    }
    w = -1;
    send_to_char(buf,ch);
    sprintf( buf, "@@eRed -------> " );
    if ( obj->value[2] > 0 )
    {
        sprintf(buf+strlen(buf),"Nothing\n\r" );
    }
    else
    {
        for ( i=4;i<8;i++ )
        {
            if ( obj->value[i] == obj->value[2] )
            {
                sprintf( buf+strlen(buf), "%s\n\r", (i==4)?"Green":(i==5)?"Blue":(i==6)?"Red":"Yellow" );
                break;
            }
        }
    }
    w = -1;
    send_to_char(buf,ch);
    sprintf( buf, "@@yYellow ----> " );
    if ( obj->value[3] > 0 )
    {
        sprintf(buf+strlen(buf),"Nothing\n\r" );
    }
    else
    {
        for ( i=4;i<8;i++ )
        {
            if ( obj->value[i] == obj->value[3] )
            {
                sprintf( buf+strlen(buf), "%s\n\r", (i==4)?"Green":(i==5)?"Blue":(i==6)?"Red":"Yellow" );
                break;
            }
        }
    }
    sprintf(buf+strlen(buf),"@@N\n\r");
    send_to_char(buf,ch);
    return;
}

void do_engineer( CHAR_DATA *ch, char *argument )
{
    VEHICLE_DATA *vhc;
    OBJ_DATA *obj;
    int val=-1;
    int i,wire;
    if ( ( vhc = ch->in_vehicle ) == NULL || !SPACE_VESSAL(vhc) )
    {
        send_to_char( "You must be inside a spaceship.\n\r", ch );
        return;
    }
    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_TOOLKIT )
    {
        send_to_char( "You must be holding a toolkit in your left hand.\n\r", ch );
        return;
    }
    if ( ch->section == SECTION_ENGINE_ROOM )
        val = vhc->speed;
    else if ( ch->section == SECTION_CONTROL_ROOM )
        val = ch->in_vehicle->range;
    else if ( ch->section == SECTION_SENSOR_ROOM )
        val = ch->in_vehicle->scanner;
    else if ( ch->section == SECTION_SHIELD_ROOM )
        val = ch->in_vehicle->max_hit / 3333;

    if ( val == -1 )
    {
        send_to_char( "There is nothing you can upgrade here... yet...\n\r", ch );
        return;
    }
    if ( obj->level > ch->pcdata->skill[gsn_engineering] * 3 )
    {
        char buf[MSL];
        sprintf( buf, "At your proficiency level, you can only use level %d toolkits or worse.\n\r", ch->pcdata->skill[gsn_engineering]*3 );
        send_to_char(buf,ch);
        return;
    }
    if ( obj->level < val )
    {
        send_to_char( "You must use a better toolkit for these parts.\n\r", ch );
        return;
    }
    for ( i=0;i<8;i++ )
        obj->value[i] = 0;
    obj->value[number_range(0,3)] = 1;
    obj->value[number_range(4,7)] = 1;
    wire = -1;
    for ( i=0;i<4;i++ )
    {
        if ( obj->value[i] != 0 )
            continue;
        if ( wire == -1 || number_percent() < 30 )
            wire = i;
    }
    obj->value[wire] = 2;
    wire = -1;
    for ( i=4;i<8;i++ )
    {
        if ( obj->value[i] != 0 )
            continue;
        if ( wire == -1 || number_percent() < 30 )
            wire = i;
    }
    obj->value[wire] = 2;
    wire = -1;
    for ( i=0;i<4;i++ )
    {
        if ( obj->value[i] != 0 )
            continue;
        if ( wire == -1 || number_percent() < 30 )
            wire = i;
    }
    obj->value[wire] = 3;
    wire = -1;
    for ( i=4;i<8;i++ )
    {
        if ( obj->value[i] != 0 )
            continue;
        if ( wire == -1 || number_percent() < 30 )
            wire = i;
    }
    obj->value[wire] = 3;
    wire = -1;
    for ( i=0;i<4;i++ )
    {
        if ( obj->value[i] != 0 )
            continue;
        if ( wire == -1 || number_percent() < 30 )
            wire = i;
    }
    obj->value[wire] = 4;
    wire = -1;
    for ( i=4;i<8;i++ )
    {
        if ( obj->value[i] != 0 )
            continue;
        if ( wire == -1 || number_percent() < 30 )
            wire = i;
    }
    obj->value[wire] = 4;
    ch->position = POS_ENGINEERING;
    return;
}

void do_eng_check( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int wire;
    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_TOOLKIT )
    {
        send_to_char( "You have lost your toolkit!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "Check which wire?\n\r", ch );
        return;
    }
    if ( !str_prefix(argument,"green") )
        wire = 0;
    else if ( !str_prefix(argument,"blue") )
        wire = 1;
    else if ( !str_prefix(argument,"red") )
        wire = 2;
    else if ( !str_prefix(argument,"yellow") )
        wire = 3;
    else
    {
        send_to_char( "Wire can be one of the following: Green, Blue, Red, Yellow.\n\r", ch );
        return;
    }
    wire = obj->value[wire];
    ch->c_sn = gsn_eng_check;
    ch->c_time = 8 + (10-ch->pcdata->skill[gsn_engineering]);
    ch->c_level = wire;
    act( "You begin checking the wires.", ch, NULL, NULL, TO_CHAR );
    act( "$n begins checking the wires.", ch, NULL, NULL, TO_ROOM );
    return;
}

void act_eng_check( CHAR_DATA *ch, int level )
{
    OBJ_DATA *obj;
    int i,chance,wire=7;
    char buf[MSL];
    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_TOOLKIT )
    {
        send_to_char( "You have lost your toolkit!\n\r", ch );
        return;
    }
    if ( number_range(1,21) < 20 - ch->pcdata->skill[gsn_engineering] )
    {
        send_to_char( "You go through the wires...\n\r", ch );
        ch->c_time = 8 + (10-ch->pcdata->skill[gsn_engineering]);
        return;
    }
    if ( ch->pcdata->skill[gsn_engineering] >= 7 )
        chance = 0;
    else
        chance = ( 7 - ch->pcdata->skill[gsn_engineering] ) * 17;
    wire = number_range(4,7);
    for ( i=4;i<8;i++ )
    {
        if ( obj->value[i] <= 0 )
            continue;
        if ( ( obj->value[i] == level && number_percent() > chance ) || ( obj->value[i] != level && number_percent() < chance ) )
            wire = i;
    }
    chance = 100 - chance;
    sprintf( buf, "You are %d%% sure that it connects to the %s wire!\n\r", chance,(wire==4)?"Green":(wire==5)?"Blue":(wire==6)?"Red":"Yellow" );
    send_to_char(buf,ch);
    ch->c_sn = -1;
    return;
}
