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
#include "ack.h"
#include "config.h"

#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2

DECLARE_DO_FUN(do_gcommands);
DECLARE_DO_FUN(do_nuke_start);
DECLARE_DO_FUN(do_nuke_pass);
const   struct  cmd_type       game_cmd_table   [] =
{
    { "commands",   do_gcommands,   0,    0,  LOG_NORMAL },
    { "help",       do_gcommands,   0,    0,  LOG_NORMAL },
    { "exit",       do_nukem,       POS_NUKEM,    0,  LOG_NORMAL },
    { "look",       do_look,    POS_NUKEM,    0,  LOG_NORMAL },
    { "nukem",      do_nukem,       POS_NUKEM,    0,  LOG_NORMAL },
    { "ooc",        do_ooc,     0,    0,  LOG_NORMAL },
    { "pass",       do_nuke_pass,   POS_NUKEM,    0,  LOG_NORMAL },
    { "say",        do_say,     0,    0,  LOG_NORMAL },
    { "start",      do_nuke_start,  POS_NUKEM,    0,  LOG_NORMAL },
    { "tell",       do_tell,    0,    0,  LOG_NORMAL },
    { "who",        do_who,     0,    0,  LOG_NORMAL },

    { "",               0,              POS_DEAD,        0,  LOG_NORMAL }
};

void do_darts( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *board;
    BUILDING_DATA *bld;
    bool bboard = FALSE;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
    char arg[MSL];

    argument = one_argument(argument,arg);

    if ( ( bld = ch->in_building ) == NULL || bld->type != BUILDING_HUNTING_LODGE || !complete(bld) )
    {
        send_to_char( "You must be in a hunting lodge to play.\n\r", ch );
        return;
    }
    for ( board = map_obj[ch->x][ch->y];board != NULL; board = board->next_in_room )
    {
        if ( board->z == bld->z && board->item_type == ITEM_DART_BOARD )
        {
            bboard = TRUE;
            break;
        }
    }
    if ( arg[0] == '\0' )
        send_to_char( "Syntax: Darts begin - Begins a new dart game\n\r        Darts throw <1-100> - Throw a dart.\n\r", ch );
    else if ( !str_prefix(arg,"begin") )
    {
        if ( bboard )
        {
            send_to_char( "There is already an active board here.\n\r", ch );
            return;
        }
        board = create_object(get_obj_index(OBJ_VNUM_DART_BOARD),0);
        board->level = number_range(1,100);
        board->x = ch->x;
        board->y = ch->y;
        board->z = ch->z;
        obj_to_room(board,get_room_index(ROOM_VNUM_WMAP));
        board->in_building = bld;
        board->value[0] = 0;
        board->value[1] = 0;
        board->value[2] = 0;
        send_to_loc( "A new game of Darts has started!\n\r", ch->x, ch->y, ch->z );
        return;
    }
    else if ( !str_prefix(arg,"throw") )
    {
        int hit;
        int score;
        char buf[MSL];

        if ( !bboard )
        {
            send_to_char( "You didn't start a game of darts!\n\r", ch );
            return;
        }
        if ( !is_number(argument) || (hit=atoi(argument)) <= 0 || hit > 100 )
        {
            send_to_char( "Syntax: darts throw <1-100>\n\r", ch );
            return;
        }
        score = (board->level>hit) ? board->level - hit : hit - board->level;
        if ( board->value[0] == 0 )
            board->value[0] = hit;
        else if ( board->value[1] == 0 )
            board->value[1] = hit;
        else if ( board->value[2] == 0 )
            board->value[2] = hit;

        board->value[3] += score;
        if ( score != 0 )
        {
            score = (score / 10) + 1;
            if ( score > 4 )
            {
                act( "You throw a dart, and completely miss the board!", ch, NULL, NULL, TO_CHAR );
                act( "$n throwd a dart, and completely misses the board!", ch, NULL, NULL, TO_ROOM );
            }
            else
            {
                sprintf( buf, "You throw a dart, hitting %d ring%s away from the center!", score, score == 1 ? "" : "s" );
                act( buf, ch, NULL, NULL, TO_CHAR );
                sprintf( buf, "$n throws a dart, hitting %d ring%s away from the center!", score, score == 1 ? "" : "s" );
                act( buf, ch, NULL, NULL, TO_ROOM );
            }
        }
        else
        {
            act( "You throw a dart, hitting the bullseye!", ch, NULL, NULL, TO_CHAR );
            act( "$n throws a dart, hitting the bullseye!", ch, NULL, NULL, TO_ROOM );
        }
        if ( board->value[2] != 0 )
        {
            sprintf( buf, "Game over! The Results:\n\r\n\rRound 1: %d\n\rRound 2: %d\n\rRound 3: %d\n\rScore: %d\n\r\n\rThe target was: %d\n\r", board->value[0], board->value[1], board->value[2], board->value[3], board->level );
            send_to_loc( buf, ch->x, ch->y, ch->z );
            extract_obj(board);
        }
    }
    return;
}

void do_where ( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    bool world = FALSE;
    char buf[MSL];
    char buf2[MSL];
    int range = 10;

    buf2[0] = '\0';
    if ( !paintball(ch) && !sysdata.killfest )
    {
        //		send_to_char( "You can only use the command during a paintball game.\n\r", ch );
        //		return;
        world = TRUE;
    }
    if ( ch->z == Z_SPACE && ch->in_vehicle && IS_SET(ch->in_vehicle->flags,VEHICLE_SPACE_SCANNER) )
        world = FALSE;
    for ( wch = first_char;wch;wch = wch->next )
    {
        if ( !world && !paintball(wch) && !sysdata.killfest )
            continue;
        if ( world && IS_IMMORTAL(wch) )
            continue;
        if ( wch->z != ch->z && !sysdata.killfest )
            continue;
        else if ( world && (wch->x + range < ch->x || wch->x - range > ch->x || wch->y + range < ch->y || wch->y - range > ch->y ) )
        {
            if ( IS_SET(wch->effect,EFFECT_TRACER) && wch != ch )
                sprintf( buf2, "@@W%s @@gat @@a%d@@g/@@a%d@@N\n\r", wch->name, wch->x, wch->y );
            continue;
        }
        if ( ch->z == Z_UNDERGROUND )
            sprintf( buf, "@@W%s @@gat @@a?@@g/@@a?@@N\n\r", wch->name );
        else
            sprintf( buf, "@@W%s @@gat @@a%d@@g/@@a%d@@N\n\r", wch->name, wch->x, wch->y );
        send_to_char( buf, ch );
    }

    if ( buf2[0] != '\0' )
    {
        send_to_char( "@@eYou could also trace the following:\n\r", ch );
        send_to_char(buf2,ch);
    }
    return;
}

void do_nukem( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    int p=0;
    char buf[MSL];
    bool in = FALSE;
    bool start = FALSE;

    for ( wch = map_ch[2][2][Z_PAINTBALL];wch;wch = wch->next_in_room )
    {
        if ( wch->z != Z_PAINTBALL )
            continue;
        p++;
        if ( wch == ch )
            in = TRUE;
        if ( wch->first_carry )
            start = TRUE;
    }
    if ( in )
    {
        BUILDING_DATA * bld;
        if ( ch->first_carry )
        {
            send_to_char( "Pass the bomb first!\n\r", ch );
            return;
        }
        if ( p == 2 )
        {
            send_to_char( "Come on, finish this last round!\n\r", ch );
            return;
        }
        sprintf( buf, "%s has left the Nukem Arena.", ch->name );
        info(buf,0);
        for ( bld = first_building;bld;bld = bld->next )
        {
            if ( bld->type != BUILDING_HQ )
                continue;
            if ( str_cmp(bld->owned,ch->name) )
                continue;
            move(ch,bld->x,bld->y,bld->z);
            do_look(ch,"");
            return;
        }
        move(ch,number_range(100,400),number_range(100,400),Z_GROUND);
        do_look(ch,"");
        return;
    }
    else
    {
        if ( start )
        {
            send_to_char( "Sorry, the game has already started.\n\r", ch );
            return;
        }
        if ( p >= 10 )
        {
            send_to_char( "There can only be 10 people inside.\n\r", ch );
            return;
        }
        if ( ch->z == Z_SPACE || ch->z == Z_PAINTBALL || IN_PIT(ch) )
        {
            send_to_char( "Not from here.\n\r", ch );
            return;
        }
        if ( !ch->in_building || ch->in_building->type != BUILDING_HQ )
        {
            send_to_char( "You can only go there from your HQ.\n\r", ch );
            return;
        }
        if ( ch->in_vehicle )
        {
            send_to_char( "Exit your vehicle first.\n\r", ch );
            return;
        }
        if ( ch->first_carry )
        {
            send_to_char( "You may not carry any items into the game arena.\n\r", ch );
            return;
        }
        move(ch,2,2,Z_PAINTBALL);
        sprintf( buf, "%s has entered the Nukem Arena. There are %d people inside.", ch->name, p+1 );
        info(buf,0);
        ch->position = POS_NUKEM;
        return;
    }
    return;
}

void game_interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    bool found;

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
    for ( cmd = 0; game_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == game_cmd_table[cmd].name[0]
            &&   !str_prefix( command, game_cmd_table[cmd].name )
            &&   (game_cmd_table[cmd].position == ch->position || game_cmd_table[cmd].position == 0)
            &&   get_trust(ch) >= game_cmd_table[cmd].level )
        {
            found = TRUE;
            break;
        }
    }

    if ( IS_BUSY(ch) && str_cmp(game_cmd_table[cmd].name,"stop") )
    {
        send_to_char( "Finish or 'stop' your other process, first.\n\r", ch );
        return;
    }

    /*
     * Log and snoop.
     */
    if ( game_cmd_table[cmd].log == LOG_NEVER )
        strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );

    if ( ( IS_SET(ch->act, PLR_LOG) )
        ||   fLogAll
        ||  game_cmd_table[cmd].log == LOG_ALWAYS )
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

    (*game_cmd_table[cmd].do_fun) ( ch, argument );

    tail_chain( );
    return;
}

void do_gcommands( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char out[MAX_STRING_LENGTH];
    int cmd;
    int col = 0;

    sprintf( out, "Game Commands Available to You:\n\r" );

    for ( cmd = 0; game_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( game_cmd_table[cmd].level > get_trust( ch ) )
            continue;
        sprintf( buf, "%-20s    ", game_cmd_table[cmd].name );
        safe_strcat( MSL, out, buf );
        if ( ++col % 3 == 0 )
            safe_strcat( MSL, out, "\n\r" );
    }
    safe_strcat( MSL, out, "\n\r" );
    send_to_char( out, ch );
    return;
}

void do_nuke_start( CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
    CHAR_DATA *wch;
    int p = 0;
    bool in = FALSE;
    bool start = FALSE;
    char buf[MSL];

    for ( wch = map_ch[2][2][Z_PAINTBALL];wch;wch = wch->next_in_room )
    {
        if ( wch->z != Z_PAINTBALL )
            continue;
        p++;
        if ( wch == ch )
            in = TRUE;
        if ( wch->first_carry )
            start = TRUE;
    }
    if ( !in )
    {
        send_to_char( "You're not in the Nukem arena!\n\r", ch );
        move(ch,number_range(100,400),number_range(100,400),Z_GROUND);
        return;
    }
    if ( p == 1 )
    {
        send_to_char( "Playing with yourself? Bah...\n\r", ch );
        return;
    }
    if ( start )
    {
        send_to_char( "The game has already started!\n\r", ch );
        return;
    }
    obj = create_object(get_obj_index(1028),0);
    obj->value[0] = number_range(10,30);
    obj->value[1] = 1;
    obj->x = 2;
    obj->y = 2;
    obj->z = Z_PAINTBALL;
    obj_to_char(obj,ch);
    sprintf( buf, "@@WThe game has started, and @@a%s@@W has the nuke!@@N\n\r", ch->name );
    send_to_loc(buf,2,2,Z_PAINTBALL);
    send_to_char( "@@WYou have the nuke, you must @@aPASS@@W it on to another player!@@N\n\r", ch );
    return;
}

void do_nuke_pass(CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    if ( !ch->first_carry )
    {
        send_to_char( "You don't carry the nuke!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "Pass it to whom?\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room(ch,argument) ) == NULL )
    {
        send_to_char( "They are not playing here!\n\r", ch );
        return;
    }
    obj = ch->first_carry;
    obj_from_char(obj);
    obj_to_char(obj,victim);
    act( "You pass the nuke to $N!", ch, NULL, victim, TO_CHAR );
    act( "$n passes the nuke to $N!", ch, NULL, victim, TO_NOTVICT );
    act( "$n passes the nuke to you!", ch, NULL, victim, TO_VICT );
    send_to_char( "@@WYou have the nuke, you must @@aPASS@@W it on to another player!@@N\n\r", victim );
    WAIT_STATE(victim,16);
    return;
}

void nuke_blow(CHAR_DATA *ch)
{
    OBJ_DATA *obj;
    CHAR_DATA *wch;
    CHAR_DATA *last = ch;
    int p = 0;
    BUILDING_DATA * bld;
    char buf[MSL];
    bool found = FALSE;

    sprintf(buf,"@@a%s @@dgot @@eNUKEM'd@@d!", ch->name );
    info(buf,0);
    for ( bld = first_building;bld;bld = bld->next )
    {
        if ( bld->type != BUILDING_HQ )
            continue;
        if ( str_cmp(bld->owned,ch->name) )
            continue;
        found = TRUE;
        move(ch,bld->x,bld->y,bld->z);
        do_look(ch,"");
    }
    if ( !found )
    {
        move(ch,number_range(100,400),number_range(100,400),Z_GROUND);
        do_look(ch,"");
    }

    for ( wch = map_ch[2][2][Z_PAINTBALL];wch;wch = wch->next_in_room )
    {
        if ( wch->z != Z_PAINTBALL )
            continue;
        last = wch;
        p++;
    }
    if ( p == 1 )
    {
        send_to_char("@@WYou are the winner!@@N\n\r", last);
        sprintf(buf,"@@a%s@@d got everyone @@eNUKEM'd@@d, and is the winner!", last->name );
        last->pcdata->nukemwins++;
        found = FALSE;
        for ( bld = first_building;bld;bld = bld->next )
        {
            if ( bld->type != BUILDING_HQ )
                continue;
            if ( str_cmp(bld->owned,last->name) )
                continue;
            found = TRUE;
            move(last,bld->x,bld->y,bld->z);
            do_look(last,"");
        }
        if ( !found )
        {
            move(last,number_range(100,400),number_range(100,400),Z_GROUND);
            do_look(last,"");
        }
        save_char_obj(last);
        info(buf,0);
        return;
    }
    obj = create_object(get_obj_index(1028),0);
    obj->value[0] = number_range(10,30);
    obj->value[1] = 1;
    obj_to_char(obj,last);
    sprintf( buf, "@@WThe game continues, and @@a%s@@W has the nuke!@@N\n\r", last->name );
    send_to_loc(buf,2,2,Z_PAINTBALL);
    send_to_char( "@@WYou have the nuke, you must @@aPASS@@W it on to another player!@@N\n\r", last );
    return;
}
