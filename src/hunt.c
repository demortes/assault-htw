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

extern char * const compass_name[];

void do_hunt( CHAR_DATA *ch, char *argument )
{
    int dir;
    int x,y;
    char arg[MSL];
    char buf[MSL];

    if ( ch->in_vehicle )
    {
        send_to_char( "Not while in a vehicle!\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: Hunt look <dir>\n\r        Hunt fire/shoot <dir>\n\r", ch );
        return;
    }
    if ( ch->z != 1 )
    {
        send_to_char( "You can only hunt on the surface.\n\r", ch );
        return;
    }
    argument = one_argument(argument,arg);

    x = ch->x;
    y = ch->y;
    if ( argument[0] == '\0' )
    {
        send_to_char( "You must specify a direction.\n\r", ch );
        return;
    }
    else if ( !str_prefix(argument,"north") )
    {
        x -= 1;
        dir = DIR_NORTH;
    }
    else if ( !str_prefix(argument,"south") )
    {
        x += 1;
        dir = DIR_SOUTH;
    }
    else if ( !str_prefix(argument,"east") )
    {
        y += 1;
        dir = DIR_EAST;
    }
    else if ( !str_prefix(argument,"west") )
    {
        y -= 1;
        dir = DIR_WEST;
    }
    else
    {
        send_to_char( "That is not a valid direction.\n\r", ch );
        return;
    }
    if ( ch->in_building && ch->in_building->exit[dir] == FALSE )
    {
        send_to_char( "You can only see a wall there.\n\r", ch );
        return;
    }
    if ( get_building(x,y) != NULL )
    {
        send_to_char( "That direction is blocked by a building.\n\r", ch );
        return;
    }
    if ( !str_prefix(arg,"look") )
    {
        if ( map_table2[x].animal[y] == -1 )
        {
            send_to_char("You can't find anything worth hunting over there.\n\r", ch );
            return;
        }
        sprintf( buf, "You can see %s over to the %s!\n\r", animal_table[map_table2[x].animal[y]].name, compass_name[dir] );
        send_to_char( buf, ch );
        return;
    }
    else if ( !str_prefix(arg,"fire") || !str_prefix(arg,"shoot") )
    {
        OBJ_DATA *weapon;
        int animal;
        int range;
        if (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL )
        {
            send_to_char( "You aren't holding a weapon!\n\r", ch );
            return;
        }
        if ( weapon->value[3] != 6 || weapon->item_type != ITEM_WEAPON )
        {
            send_to_char( "This isn't a gun you're holding!\n\r", ch );
            return;
        }
        if ( clip_table[weapon->value[2]].type != DAMAGE_BULLETS )
        {
            send_to_char( "You can only hunt with weapons that fire bullets.\n\r", ch );
            return;
        }
        if ( ( animal = map_table2[x].animal[y]) == -1 )
        {
            send_to_char("You can't find anything worth hunting over there.\n\r", ch );
            return;
        }
        if ( weapon->value[0] <= 0 )
        {
            if ( IS_SET(ch->config, CONFIG_SOUND))
                send_to_char( "!!SOUND(emptychamber V=60 L=1 P=25 T=combat U=http://fredrik.homelinux.org/Amnon/MSP/emptychamber.wav)\n\r", ch);
            send_to_char( "You have no more ammo in that thing!\n\r", ch );
            return;
        }
        range = weapon->value[4] + 1;
        if ( range < 5 && IS_SET(ch->config, CONFIG_SOUND))
            send_to_char( "!!SOUND(shot1 V=40 L=1 P=25 T=combat U=http://fredrik.homelinux.org/Amnon/MSP/shot1.wav)\n\r", ch);
        else if ( range >= 5 && IS_SET(ch->config, CONFIG_SOUND))
            send_to_char( "!!SOUND(sniper V=60 L=1 P=25 T=combat U=http://fredrik.homelinux.org/Amnon/MSP/sniper.wav)\n\r", ch);
        sprintf( buf, "You fire %s to the %s!\n\r", weapon->short_descr, compass_name[dir] );
        send_to_char( buf, ch );
        sprintf( buf, "$n fires %s to the %s!", weapon->short_descr, compass_name[dir] );
        act( buf, ch, NULL, NULL, TO_ROOM );
        send_to_loc( "You hear a gunshot!\n\r", x,y,1);
        weapon->value[0]--;
        WAIT_STATE(ch,clip_table[weapon->value[2]].speed);
                                                            //Hit
        if ( number_percent() < clip_table[weapon->value[2]].miss )
        {
            char buf2[MSL];

            sprintf( buf, "You hit %s!\n\r", animal_table[animal].name );
            sprintf( buf2, "%s is hit!\n\r", animal_table[animal].name );
            ch->anim_hit++;
                                                            //Kill
            if ( number_percent() < clip_table[weapon->value[2]].miss )
            {
                ch->anim_kills++;
                sprintf( buf+strlen(buf), "%s falls down, dead.\n\r", animal_table[animal].name );
                sprintf( buf2+strlen(buf2), "%s falls down, dead.\n\r", animal_table[animal].name );
                create_animal_corpse(animal,x,y);
            }
            else                                            //Wound
            {
                sprintf( buf+strlen(buf), "%s is wounded, and speeds away!\n\r", animal_table[animal].name );
                sprintf( buf2+strlen(buf2), "%s is wounded, and speeds away!\n\r", animal_table[animal].name );
            }
            send_to_char( buf, ch );
            send_to_loc( buf2, x,y,1 );
            map_table2[x].animal[y] = -1;
            return;
        }
        else                                                //Miss
        {
            sprintf( buf, "The bullet misses %s!\n\r", animal_table[animal].name );
            ch->anim_miss++;
            if ( number_percent() < 50 )                    // Animal runs away
            {
                sprintf( buf+strlen(buf), "%s is startled by the shot, and speeds away!\n\r", animal_table[animal].name );
                map_table2[x].animal[y] = -1;
            }
            send_to_char ( buf, ch );
            send_to_loc( buf, x, y, 1 );
            return;
        }
    }
    return;
}
