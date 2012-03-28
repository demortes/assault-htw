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

/* This file should include all the space combat stuff */

#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2

const char * room_names [] =
{ "Control Room", "Engine Room", "Sensors Room", "Shield Generator", NULL};

DECLARE_DO_FUN( do_go       );
DECLARE_DO_FUN( do_space_exit   );
DECLARE_DO_FUN( do_space_fire   );
DECLARE_DO_FUN( do_scommands    );
DECLARE_DO_FUN( do_power    );
DECLARE_DO_FUN( do_space_blast  );
const   struct  cmd_type       space_cmd_table   [] =
{
    { "north",          do_north,       POS_SPACE_COM,    0,  LOG_NORMAL },
    { "east",           do_east,        POS_SPACE_COM,    0,  LOG_NORMAL },
    { "south",          do_south,       POS_SPACE_COM,    0,  LOG_NORMAL },
    { "west",           do_west,        POS_SPACE_COM,    0,  LOG_NORMAL },
    { "look",           do_look,        POS_SPACE_COM,    0,  LOG_NORMAL },
    { "score",          do_score,       POS_SPACE_COM,    0,  LOG_NORMAL },
    { "tell",           do_tell,        POS_SPACE_COM,    0,  LOG_NORMAL },
    { "target",         do_target,      POS_SPACE_COM,    0,  LOG_NORMAL },

    { "battlestations", do_space_exit,  POS_SPACE_COM,    0,  LOG_NORMAL },
    { "fire",       do_space_fire,  POS_SPACE_COM,    0,  LOG_NORMAL },
    { "blast",      do_space_blast, POS_SPACE_COM,    0,  LOG_NORMAL },
    { "eject",      do_exit,    POS_SPACE_COM,    0,  LOG_NORMAL },
    { "exit",       do_exit,    POS_SPACE_COM,    0,  LOG_NORMAL },
    { "go",     do_go,      POS_SPACE_COM,    0,  LOG_NORMAL },
    { "help",       do_scommands,   POS_SPACE_COM,    0,  LOG_NORMAL },
    { "hijack",     do_space_hijack,POS_SPACE_COM,    0,  LOG_NORMAL },
    { "commands",   do_scommands,   POS_SPACE_COM,    0,  LOG_NORMAL },
    { "power",      do_power,   POS_SPACE_COM,    0,  LOG_NORMAL },
    { "shoot",      do_space_fire,  POS_SPACE_COM,    0,  LOG_NORMAL },
    { "who",        do_who,     POS_SPACE_COM,    0,  LOG_NORMAL },
    { "stop",       do_stop,    POS_SPACE_COM,    0,  LOG_NORMAL },
    { "move",       do_space_move,  POS_SPACE_COM,    0,  LOG_NORMAL },

    { "",               0,              POS_DEAD,        0,  LOG_NORMAL }
};

void space_interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    bool found;

    if ( !ch->in_vehicle )
    {
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
    for ( cmd = 0; space_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == space_cmd_table[cmd].name[0]
            &&   !str_prefix( command, space_cmd_table[cmd].name )
            &&   get_trust(ch) >= space_cmd_table[cmd].level )
        {
            found = TRUE;
            break;
        }
    }

    if ( IS_BUSY(ch) && str_cmp(space_cmd_table[cmd].name,"stop") )
    {
        send_to_char( "Finish or 'stop' your other process, first.\n\r", ch );
        return;
    }

    /*
     * Log and snoop.
     */
    if ( space_cmd_table[cmd].log == LOG_NEVER )
        strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( IS_SET(ch->act, PLR_LOG) )
        ||   fLogAll
        ||   space_cmd_table[cmd].log == LOG_ALWAYS )
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

    (*space_cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}

void do_space_blast( CHAR_DATA *ch, char *argument )
{
    if ( ch->in_vehicle->state == VEHICLE_STATE_CHARGE )
    {
        send_to_char("Powering down...\n\r",ch);
        ch->in_vehicle->value[0] = 0;
        ch->in_vehicle->state = VEHICLE_STATE_NORMAL;
        return;
    }
    ch->in_vehicle->state = VEHICLE_STATE_CHARGE;
    send_to_char( "You begin charging up your ion blaster!\n\r", ch );
    act( "$n begins charging up $s ion blaster!", ch, NULL, NULL, TO_ROOM );
    return;
}

void do_go( CHAR_DATA *ch, char *argument )
{
    if ( !str_cmp(argument,"evasive") )
    {
        ch->in_vehicle->state = VEHICLE_STATE_EVADE;
        act( "Engaging evasive maneuvers.", ch, NULL, NULL, TO_CHAR );
        act( "$n's $t engages evasive maneuvers.", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
        return;
    }
    else if ( !str_cmp(argument,"defensive") )
    {
        ch->in_vehicle->state = VEHICLE_STATE_DEFENSE;
        act( "Rerouting power to defenses.", ch, NULL, NULL, TO_CHAR );
        act( "$n's $t reroutes power to defenses.", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
        return;
    }
    else if ( !str_cmp(argument,"offensive") )
    {
        ch->in_vehicle->state = VEHICLE_STATE_OFFENSE;
        act( "Rerouting power to weapons.", ch, NULL, NULL, TO_CHAR );
        act( "$n's $t reroutes power to its weapons.", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
        return;
    }
    else if ( !str_cmp(argument,"normal") )
    {
        ch->in_vehicle->state = VEHICLE_STATE_NORMAL;
        act( "All systems in normal status.", ch, NULL, NULL, TO_CHAR );
        return;
    }
    else
        send_to_char( "Valid choices: evasive, defensive, offensive, normal.\n\r", ch );

    return;
}

void do_space_exit( CHAR_DATA *ch, char *argument )
{
    act( "The alarms stop.", ch, NULL, NULL, TO_CHAR );
    ch->position = POS_STANDING;
    return;
}

void do_space_fire( CHAR_DATA *ch, char *argument )
{
    //	int x = ch->in_vehicle->ammo_type;
    //	ch->in_vehicle->ammo_type = x;
    do_shoot(ch,argument);
    if ( ch->position != POS_DEAD )
        ch->position = POS_SPACE_COM;
    return;
}

void do_scommands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char out[MAX_STRING_LENGTH];
    int cmd;
    int col = 0;

    sprintf( out, "Space Commands Available to You:\n\r" );

    for ( cmd = 0; space_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( space_cmd_table[cmd].level > get_trust( ch ) )
            continue;
        sprintf( buf, "%-20s    ", space_cmd_table[cmd].name );
        safe_strcat( MSL, out, buf );
        if ( ++col % 3 == 0 )
            safe_strcat( MSL, out, "\n\r" );
    }
    safe_strcat( MSL, out, "\n\r" );
    send_to_char( out, ch );
    return;
}

void do_power( CHAR_DATA *ch, char *argument )
{
    int i,type,amount;
    char arg[MSL];
    char buf[MSL];
    VEHICLE_DATA *vhc = ch->in_vehicle;

    if ( argument[0] == '\0' )
    {
        sprintf( buf, "Engines: %d%%\n\rDefenses: %d%%\n\rWeapons: %d%%\n\rRepair Drones: %d%%\n\r\n\rReserved Power: %d\n\r", vhc->power[POWER_ENGINE], vhc->power[POWER_ARMOR], vhc->power[POWER_WEAPONS], vhc->power[POWER_REPAIR], vhc->power[POWER_SOURCE] );
        send_to_char( buf, ch );
        return;
    }
    argument = one_argument(argument,arg);
    if ( arg[0] == '\0' || !is_number(argument) )
    {
        send_to_char( "Syntax: power <engines/defenses/weapons/repair> <amount>\n\r", ch );
        return;
    }
    amount = atoi(argument);
    if ( !str_prefix(arg,"engines") )
        type = POWER_ENGINE;
    else if ( !str_prefix(arg,"defenses") )
        type = POWER_ARMOR;
    else if ( !str_prefix(arg,"weapons") )
        type = POWER_WEAPONS;
    else if ( !str_prefix(arg,"repair") )
        type = POWER_REPAIR;
    else
    {
        send_to_char( "Invalid system to power.\n\r", ch );
        return;
    }
    i = vhc->power[type] + amount;
    if ( i < 0 || i > 200 )
    {
        send_to_char( "New power value must be in the bounderies of 0 and 200%.\n\r", ch );
        return;
    }
    if ( vhc->power[POWER_SOURCE] < amount )
    {
        send_to_char( "You do not have enough power in the reserves. Decrease another system's power, first.\n\r", ch );
        return;
    }
    vhc->power[type] = i;
    vhc->power[0] -= amount;
    sprintf( buf, "Power set to %d%%.\n\r", i );
    send_to_char(buf,ch);
    return;
}

void do_space_move( CHAR_DATA *ch, char *argument )
{
    int i;
    char buf[MSL];

    if ( !ch->in_vehicle || !SPACE_VESSAL(ch->in_vehicle ) )
    {
        if ( IS_NEWBIE(ch) )
        {
            do_help(ch,"move");
            return;
        }
        send_to_char( "You must be in a space ship.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        sprintf( buf, "\n\r@@gYou are at: @@W%s.\n\r\n\r@@gYou can go to: @@W\n\r", room_names[ch->section] );
        send_to_char(buf,ch);
        buf[0] = '\0';
        for ( i=0;room_names[i] != NULL;i++ )
            sprintf( buf+strlen(buf), "%s\n\r", room_names[i] );
        send_to_char(buf,ch);
        send_to_char("@@N\n\r", ch );
        return;
    }
    for ( i=0;room_names[i] != NULL;i++ )
    {
        if ( room_names[i] == NULL )
            break;
        if ( !str_prefix(argument,room_names[i]) )
        {
            sprintf( buf, "You head towards the %s.\n\r", room_names[i] );
            send_to_char(buf,ch);
            ch->section = i;
            return;
        }
    }
    send_to_char( "There is no such room aboard your ship.\n\r", ch );
    return;
}

void do_space_hijack(CHAR_DATA *ch, char *argument )
{
    VEHICLE_DATA *vhc;

    if ( ch->z != Z_SPACE )
    {
        send_to_char( "You must be in space to hijack ships.\n\r", ch );
        return;
    }
    if ( !ch->in_vehicle )
        return;
    if ( ( vhc = get_vehicle_char(ch,argument) ) == NULL )
    {
        send_to_char( "You can't find that ship here.\n\r", ch );
        return;
    }
    if ( vhc->driving )
    {
        if ( vhc->driving == ch )
            send_to_char( "This is your ship.\n\r", ch );
        else
            send_to_char( "You can't hijack a ship when someone is inside!\n\r", ch );
        return;
    }
    ch->in_vehicle->driving = NULL;
    ch->in_vehicle = vhc;
    vhc->driving = ch;
    vhc->timer = 0;
    ch->section = 0;
    send_to_char( "You board the ship and hijack it!\n\r", ch );
    act( "$n boards $t and hijacks it!", ch, vhc->desc, NULL, TO_ROOM );
    return;
}
