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
#include "ack.h"

/*
 * Local functions.
 */
#define CD CHAR_DATA
void    wear_obj        args( ( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace ) );
CD *    find_keeper     args( ( CHAR_DATA *ch ) );
int     get_cost        args( ( CHAR_DATA *keeper, OBJ_DATA *obj, bool fBuy ) );
void    check_guards    args( ( CHAR_DATA *ch ) );
#undef  CD

void get_obj( CHAR_DATA *ch, OBJ_DATA *obj, OBJ_DATA *container )
{

    if ( NOT_IN_ROOM(ch,obj) )
        return;

    if ( !CAN_WEAR(obj, ITEM_TAKE)  )
    {
        send_to_char( "You can't take that.\n\r", ch );
        return;
    }
    if ( obj->item_type == ITEM_BOMB && obj->value[1] > 0 )
    {
        send_to_char( "You can't pick up an armed bomb! Try @@edisarm@@Ning it.\n\r", ch );
        return;
    }

    {
        BUILDING_DATA *bld;
        CHAR_DATA *vch;

        bld = get_char_building(ch);
        if ( bld && obj->item_type != ITEM_TOKEN && strcmp(bld->owned, "nobody") )
        {
            vch = get_ch(bld->owned);
            if ( ( (ch->played + (int) (current_time - ch->logon )) / 3600 ) <= 0 && vch != ch && !IS_IMMORTAL(vch) && !allied(ch, vch) )
            {
                send_to_char( "You can't steal from other people until you have played for at least 1 hour.\n\r", ch );
                return;
            }
            if ( ( ( vch = get_ch(bld->owned)) == NULL || get_rank(vch) < 2) && !IS_IMMORTAL(ch) && ch != vch && ch->pcdata->alliance!=vch->pcdata->alliance)
            {
                //			if ( vch && (vch->pcdata->alliance != -1 && vch->pcdata->alliance == ch->pcdata->alliance ))
                {
                    if ( sysdata.kill_groups && vch->kill_group == ch->kill_group )
                    {
                    }
                    else
                    {
                        send_to_char( "You can't steal stuff from rank 1 people.\n\r", ch );
                        return;
                    }
                }
            }
        }
    }

    if ( ch->carry_number + get_obj_number( obj ) > can_carry_n( ch ) )
    {
        act( "$d: you can't carry that many items.",
            ch, NULL, obj->name, TO_CHAR );
        return;
    }

    if ( ch->carry_weight + get_obj_weight( obj ) > can_carry_w( ch ) )
    {
        act( "$d: you can't carry that much weight.",
            ch, NULL, obj->name, TO_CHAR );
        return;
    }

    act( "You get $p.", ch, obj, container, TO_CHAR );
    act( "$n gets $p.", ch, obj, container, TO_ROOM );
    obj_from_room( obj );

    obj_to_char( obj, ch );

    if ( obj->item_type == ITEM_TOKEN && obj->value[0] == 1 )
    {
        char buf[MSL];
        ch->quest_points += obj->value[1];
        sprintf( buf, "@@rYou have been rewarded @@W%d @@rquest points!@@N\n\r", obj->value[1] );
        send_to_char( buf, ch );
        if ( ch->quest_points > 5000 )
        {
            send_to_char( "You have reached the cap 5000 QPs.\n\r", ch );
            ch->quest_points = 5000;
        }
        extract_obj(obj);
        save_char_obj(ch);
    }
    return;
}

void do_get( CHAR_DATA *ch, char *argument )
{

    char container_name[MSL];
    char object_list[MSL];
    char one_object[MSL];
    char object_number[MSL];
    char victim_name[MSL];
    char actbuf[MSL];

    ROOM_INDEX_DATA *room;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found = FALSE;
    bool get_ok = FALSE;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
    victim_name[0] = '\0';

    if ( ch->in_vehicle && ch->in_vehicle->type != VEHICLE_MECH )
    {
        send_to_char( "You can't reach while driving.\n\r", ch );
        return;
    }

    pre_parse( argument, victim_name, container_name, object_list );

    /* Get type. */
    if (   object_list[0] == '\0' )
    {
        send_to_char( "Get <object> or Get all\n\r", ch );
        return;
    }

    {
        if ( !str_prefix( "1 all.", object_list ) )
        {                                                   /* 'get all.obj' */
            char * parse;

            parse =  &object_list[6];

            found = FALSE;

            room = ch->in_room;

            parse = one_argument(parse, one_object);
            for ( obj = map_obj[ch->x][ch->y]; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_in_room;
                if ( is_name( one_object, obj->name )
                    && can_see_obj( ch, obj ) && !NOT_IN_ROOM(ch,obj) && obj->carried_by == NULL )
                {
                    found = TRUE;
                    get_obj( ch, obj, NULL );
                }
            }
            if ( !found )
            {
                send_to_char( "I see nothing like that here.\n\r", ch);
            }
            do_save( ch, "");
            return;
        }
        else if ( !str_prefix( "1 all", object_list ) )
        {                                                   /* 'get all' */
            found = FALSE;

            room = ch->in_room;

            if ( get_ok )
                found = TRUE;

            for ( obj = map_obj[ch->x][ch->y]; obj != NULL; obj = obj_next )
            {
                obj_next = obj->next_in_room;
                if ( can_see_obj( ch, obj ) && !NOT_IN_ROOM(ch,obj) && obj->carried_by == NULL )
                {
                    found = TRUE;
                    get_obj( ch, obj, NULL );
                }
            }
            if ( found )
                do_save( ch, "" );

            if ( !found && !get_ok )
            {
                send_to_char( "I see nothing here.\n\r", ch );
                if ( map_table.resource[ch->x][ch->y][ch->z] > -1 )
                    send_to_char( "Outside resources cannot be picked with the 'get all' command. You must get them by name,\n\r", ch );
            }
            return;
        }
        else
        {
            bool found_one_obj = FALSE;
            char * parse;
            parse =  object_list;

            for ( ; ; )
            {
                sh_int looper;
                sprintf( bug_buf, "In get thing room, parse is %s.", parse );
                monitor_chan( NULL, bug_buf, MONITOR_DEBUG );
                parse = one_argument( parse, object_number );
                if ( object_number[0] == '\0' )
                    break;
                parse = one_argument( parse, one_object );
                if ( one_object[0] == '\0' )
                    break;
                sprintf( bug_buf, "Looking for %s %s.", object_number, one_object );
                monitor_chan( NULL, bug_buf, MONITOR_DEBUG );
                for ( looper = 0; looper < atoi( object_number ); looper++ )
                {
                    obj = get_obj_room( ch, one_object, map_obj[ch->x][ch->y] );
                    if ( obj == NULL )
                    {
                        if ( found_one_obj )
                        {
                            sprintf( actbuf, "There isn't another %s here.", one_object );
                            act( actbuf,
                                ch, NULL, NULL, TO_CHAR );
                            break;
                        }
                    }

                    else
                    {
                        room = ch->in_room;

                        get_obj( ch, obj, NULL );
                        found_one_obj = TRUE;
                    }
                }
            }
            if (  ( found  == FALSE )
                && ( found_one_obj == FALSE )
                && ( get_ok == FALSE ) )
            {
                send_to_char( "I don't see what your looking for in this room.\n\r", ch );
                return;
            }
            do_save( ch, "" );

            return;
        }
    }
    do_save( ch, "" );

    return;
}

void do_drop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char container_name[MSL];
    char victim_name[MSL];
    char object_list[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    bool found = FALSE;
    char object_number[MSL];
    char one_object[MSL];
    char * parse;
    char buf[MSL];
    int objs = 0;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    if ( ch->in_vehicle && ch->in_vehicle->type != VEHICLE_MECH && !IS_IMMORTAL(ch) )
    {
        send_to_char( "You can't do that while driving.\n\r", ch );
        return;
    }
    if ( paintball(ch) )
        return;
    victim_name[0] = '\0';
    pre_parse( argument, victim_name, container_name, object_list );

    if ( object_list[0] == '\0' )
    {
        send_to_char( "Drop what?\n\r", ch );
        return;
    }

    if ( ch->in_building )
        for ( obj=map_obj[ch->x][ch->y];obj;obj = obj->next_in_room )
            if ( !NOT_IN_ROOM(ch,obj) && obj->carried_by == NULL && COUNTS_TOWARDS_OBJECT_LIMIT(obj))
                objs++;

    if ( objs > get_item_limit(ch->in_building) )
    {
        send_to_char( "You cannot place any more items in this building.\n\r", ch );
        return;
    }

    if ( str_prefix( "1 all", object_list ) && str_prefix( "1 all.", object_list ) )
    {
        parse =  object_list;
        for ( ; ; )
        {
            sh_int looper;
            sprintf( bug_buf, "In drop thing room, parse is %s.", parse );
            monitor_chan( NULL, bug_buf, MONITOR_DEBUG );
            parse = one_argument( parse, object_number );
            if ( object_number[0] == '\0' )
                break;
            parse = one_argument( parse, one_object );
            if ( one_object[0] == '\0' )
                break;
            /*        sprintf( bug_buf, "Looking for %s %s.", object_number, one_object );
                    monitor_chan( NULL, bug_buf, MONITOR_DEBUG );*/
            for ( looper = 0; looper < atoi( object_number ); looper++ )
            {
                obj = get_obj_carry( ch, one_object );
                if ( obj == NULL )
                {
                    break;
                }
                if ( !can_drop_obj( ch, obj ) )
                {
                    send_to_char( "You can't let go of it.\n\r", ch );
                    if ( looper < 10 )
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }

                if ( get_char_building(ch) == NULL && (obj->item_type != ITEM_BOMB || obj->value[1] <= 0 ) && obj->item_type != ITEM_LIGHT && obj->item_type != ITEM_SCAFFOLD && obj->item_type != ITEM_DRONE && !IS_IMMORTAL(ch) )
                {
                    send_to_char( "You can only drop in buildings.\n\r", ch );
                    return;
                }
                obj->x = ch->x;
                obj->y = ch->y;
                obj->z = ch->z;
                obj_from_char( obj );
                obj_to_room( obj, ch->in_room );
                if ( obj->item_type == ITEM_SCAFFOLD )
                    if ( open_scaffold(ch,obj) )
                        return;
                if ( obj->item_type == ITEM_DRONE && obj->value[0] == 2 && obj->z == Z_UNDERGROUND )
                {
                    obj->value[12] = number_range(0,3);
                    send_to_loc("The drone begins moving about.\n\r", obj->x,obj->y,obj->z);
                }

                if ( obj->pIndexData->vnum == OBJ_VNUM_FLAG )
                {
                    if ( !ch->in_building || !complete(ch->in_building) ||ch->in_building->type != BUILDING_HUNTING_LODGE || str_cmp(ch->in_building->owned,ch->name))
                    {
                        send_to_char( "You can only drop flags in hunting lodges.\n\r", ch );
                        obj_from_room(obj);
                        obj_to_char( obj, ch );
                        return;
                    }
                    obj->in_building = ch->in_building;
                    free_string(obj->owner);
                    obj->owner = str_dup(ch->in_building->owned);
                    obj->description = str_dup(buf);
                    if ( IS_SET(obj->wear_flags,BIT_24) )
                        REMOVE_BIT(obj->wear_flags,BIT_24); //Remove WEAR TAKE
                    act( "You place $p on the floor as a trophy!", ch, obj, NULL, TO_CHAR );
                    act( "$n places $p on the floor as a trophy!", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    act( "$n drops $p.", ch, obj, NULL, TO_ROOM);
                    act( "You drop $p.", ch, obj, NULL, TO_CHAR );
                    move_obj(obj,ch->x,ch->y,ch->z);
                }

                if ( IS_IMMORTAL(ch) && ch->trust < 90 )
                {
                    char buf[MSL];

                    sprintf( buf, "%s dropped %s", ch->name, obj->short_descr );
                    log_string(buf);
                }
            }
        }
        do_save( ch, "" );
        return;
    }

    /* 'drop all' or 'drop all.obj' */
    found = FALSE;
    if ( !str_prefix( "1 all.", object_list ) )
    {
        int i =0;
        if ( get_char_building(ch) == NULL )
            return;
        parse = &object_list[6];
        parse = one_argument(parse, one_object);
        sprintf( object_list, "1 all.%s", one_object );
        sprintf( bug_buf, "In drop allthing, object_list is %s.", object_list );
        monitor_chan( NULL, bug_buf, MONITOR_DEBUG );
        for ( obj = ch->first_carry ; obj != NULL ; obj = obj_next )
        {

            obj_next = obj->next_in_carry_list;
            i++;
            if ( i > 50 )
                break;

            if ( is_name( one_object, obj->name )
                && obj->wear_loc == WEAR_NONE
                && can_see_obj( ch, obj )
                && can_drop_obj( ch, obj ) )
            {
                if ( get_char_building(ch) == NULL && (obj->item_type != ITEM_BOMB || obj->value[1] <= 0 ) && obj->item_type != ITEM_DRONE && obj->item_type != ITEM_SCAFFOLD && !IS_IMMORTAL(ch) )
                {
                    send_to_char( "You can only drop in buildings.\n\r", ch );
                    continue;
                }
                if ( objs > get_item_limit(ch->in_building) )
                {
                    send_to_char( "You cannot place any more items in this building.\n\r", ch );
                    return;
                }

                /* found something to put in container... */
                found = TRUE;
                obj_from_char( obj );
                obj_to_room( obj, ch->in_room );
                if ( obj->item_type == ITEM_SCAFFOLD )
                    if ( open_scaffold(ch,obj) )
                        continue;
                if ( obj->item_type == ITEM_DRONE && obj->value[0] == 2 && obj->z == Z_UNDERGROUND )
                {
                    obj->value[12] = number_range(0,3);
                    send_to_loc("The drone begins moving about.\n\r", obj->x,obj->y,obj->z);
                }

                if ( obj->pIndexData->vnum == OBJ_VNUM_FLAG )
                {
                    if ( !ch->in_building || !complete(ch->in_building) || ch->in_building->type != BUILDING_HUNTING_LODGE || str_cmp(ch->in_building->owned,ch->name))
                    {
                        send_to_char( "You can only drop flags in hunting lodges.\n\r", ch );
                        obj_from_room(obj);
                        obj_to_char( obj, ch );
                        continue;
                    }
                    obj->in_building = ch->in_building;
                    obj->description = str_dup(buf);
                    free_string(obj->owner);
                    obj->owner = str_dup(ch->in_building->owned);
                    if ( IS_SET(obj->wear_flags,BIT_24) )
                        REMOVE_BIT(obj->wear_flags,BIT_24); //Remove WEAR TAKE
                    act( "You place $p on the floor as a trophy!", ch, obj, NULL, TO_CHAR );
                    act( "$n places $p on the floor as a trophy!", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    act( "$n drops $p.", ch, obj, NULL, TO_ROOM);
                    act( "You drop $p.", ch, obj, NULL, TO_CHAR );
                    move_obj(obj,ch->x,ch->y,ch->z);
                }
                objs++;
                if ( IS_IMMORTAL(ch) && ch->trust < 90 )
                {
                    char buf[MSL];

                    sprintf( buf, "%s dropped %s", ch->name, obj->short_descr );
                    log_string(buf);
                }
            }
        }

    }
    else
    {
        for ( obj = ch->first_carry; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_in_carry_list;

            if ( can_see_obj( ch, obj )
                &&   obj->wear_loc == WEAR_NONE
                &&   can_drop_obj( ch, obj ) )
            {
                if ( get_char_building(ch) == NULL && (obj->item_type != ITEM_BOMB || obj->value[1] <= 0 ) && obj->item_type != ITEM_DRONE && obj->item_type != ITEM_SCAFFOLD && IS_IMMORTAL(ch) )
                {
                    send_to_char( "You can only drop in buildings.\n\r", ch );
                    continue;
                }
                if ( objs > get_item_limit(ch->in_building) )
                {
                    send_to_char( "You cannot place any more items in this building.\n\r", ch );
                    return;
                }
                found = TRUE;
                obj_from_char( obj );
                obj_to_room( obj, ch->in_room );
                if ( obj->item_type == ITEM_SCAFFOLD )
                    if ( open_scaffold(ch,obj) )
                        continue;
                if ( obj->item_type == ITEM_DRONE && obj->value[0] == 2 && obj->z == Z_UNDERGROUND )
                {
                    obj->value[12] = number_range(0,3);
                    send_to_loc("The drone begins moving about.\n\r", obj->x,obj->y,obj->z);
                }

                if ( obj->pIndexData->vnum == OBJ_VNUM_FLAG )
                {
                    if ( !ch->in_building || !complete(ch->in_building) || ch->in_building->type != BUILDING_HUNTING_LODGE || str_cmp(ch->in_building->owned,ch->name))
                    {
                        send_to_char( "You can only drop flags in hunting lodges.\n\r", ch );
                        obj_from_room(obj);
                        obj_to_char( obj, ch );
                        continue;
                    }
                    free_string(obj->owner);
                    obj->owner = str_dup(ch->in_building->owned);
                    if ( IS_SET(obj->wear_flags,BIT_24) )
                        REMOVE_BIT(obj->wear_flags,BIT_24); //Remove WEAR TAKE
                    act( "You place $p on the floor as a trophy!", ch, obj, NULL, TO_CHAR );
                    act( "$n places $p on the floor as a trophy!", ch, obj, NULL, TO_ROOM );
                }
                else
                {
                    act( "$n drops $p.", ch, obj, NULL, TO_ROOM);
                    act( "You drop $p.", ch, obj, NULL, TO_CHAR );
                    move_obj(obj,ch->x,ch->y,ch->z);
                }
                objs++;
            }
        }
    }

    if ( !found )
    {
        act( "You are not carrying anything.",
            ch, NULL, arg, TO_CHAR );
    }

    if ( found )
        do_save( ch, "" );
    return;
}

void do_give( CHAR_DATA *ch, char *argument )
{
    char victim_name[MSL];
    char container_name[MSL];
    char object_list[MSL];
    char one_object[MSL];
    char object_number[MSL];
    int tax;
    CHAR_DATA *victim;
    OBJ_DATA  *obj;

    if ( ch->in_vehicle && ch->in_vehicle->type != VEHICLE_MECH )
    {
        send_to_char( "Not while driving.\n\r", ch );
        return;
    }
    if ( paintball(ch) )
    {
        send_to_char( "You can't trade in paintball - Trade in the real world, where there is real danger.\n\r", ch );
        return;
    }
    tax = 0;

    container_name[0] = '\0';
    pre_parse( argument, victim_name, container_name, object_list );

    if ( object_list[0] == '\0' || victim_name[0] == '\0' )
    {
        send_to_char( "Give what TO whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, victim_name ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( str_prefix( "1 all", object_list ) && str_prefix( "all.", object_list ) )
    {
        char * parse;
        parse =  object_list;
        for ( ; ; )
        {
            sh_int looper;
            sprintf( bug_buf, "In give thing, parse is %s.", parse );
            monitor_chan( NULL, bug_buf, MONITOR_DEBUG );
            parse = one_argument( parse, object_number );
            if ( object_number[0] == '\0' )
                break;
            parse = one_argument( parse, one_object );
            if ( one_object[0] == '\0' )
                break;
            sprintf( bug_buf, "Looking for %s %s.", object_number, one_object );
            monitor_chan( NULL, bug_buf, MONITOR_DEBUG );
            for ( looper = 0; looper < atoi( object_number ); looper++ )
            {
                obj = get_obj_carry( ch, one_object );
                if ( obj == NULL )
                {
                    break;
                }
                if ( obj->wear_loc != WEAR_NONE )
                {
                    send_to_char( "You must remove it first.\n\r", ch );
                    continue;
                }
                if ( obj->item_type == ITEM_BOMB && obj->value[1] != 0 )
                {
                    send_to_char( "Are you nuts?! Giving away an ARMED bomb?!\n\r", ch );
                    return;
                }
                if ( !can_drop_obj( ch, obj ) )
                {
                    send_to_char( "You can't let go of it.\n\r", ch );
                    if ( looper < 10 )
                    {
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
                {
                    act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
                    continue;
                }

                if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
                {
                    act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
                    continue;
                }

                if ( !can_see_obj( victim, obj ) )
                {
                    act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
                    continue;
                }
                obj_from_char( obj );
                obj_to_char( obj, victim );
                act( "$n gives $p to $N.", ch, obj, victim, TO_NOTVICT);
                act( "You give $p to $N.", ch, obj, victim, TO_CHAR );
                act( "$n gives you $p.", ch, obj, victim, TO_VICT );
                if ( IS_IMMORTAL(ch) && ch->trust < 90 )
                {
                    char buf[MSL];

                    sprintf( buf, "%s gave %s to %s", ch->name, obj->short_descr, victim->name );
                    log_string(buf);
                }
            }
        }
    }
    do_save( victim, "" );

    return;

}

/*
 * Remove an object.
 */
bool remove_obj( CHAR_DATA *ch, int iWear, bool fReplace )
{
    OBJ_DATA *obj;
    if ( ( obj = get_eq_char( ch, iWear ) ) == NULL )
        return TRUE;

    if ( !fReplace )
        return FALSE;

    if ( IS_SET(obj->extra_flags, ITEM_NOREMOVE) )
    {
        act( "You can't remove $p.", ch, obj, NULL, TO_CHAR );
        return FALSE;
    }

    unequip_char( ch, obj );
    act( "$n stops using $p.", ch, obj, NULL, TO_ROOM );
    act( "You stop using $p.", ch, obj, NULL, TO_CHAR );
    return TRUE;
}

/* MAG
   can_wear_at(ch,obj,location) returns true if the char can wear the object
   at the location.
   */

bool can_wear_at(CHAR_DATA * ch, OBJ_DATA * obj, int location)
{
    int loc_flag;

    switch(location)
    {
        case WEAR_NONE:       loc_flag=0;         break;
        case WEAR_FINGER_L:
        case WEAR_FINGER_R:   loc_flag=ITEM_WEAR_FINGER;      break;
        case WEAR_NECK_1:
        case WEAR_NECK_2: loc_flag=ITEM_WEAR_NECK;    break;
        case WEAR_BODY:   loc_flag=ITEM_WEAR_BODY;    break;
        case WEAR_HEAD:   loc_flag=ITEM_WEAR_HEAD;    break;
        case WEAR_LEGS:   loc_flag=ITEM_WEAR_LEGS;    break;
        case WEAR_FEET:   loc_flag=ITEM_WEAR_FEET;    break;
        case WEAR_HANDS:  loc_flag=ITEM_WEAR_HANDS;   break;
        case WEAR_ARMS:   loc_flag=ITEM_WEAR_ARMS;    break;
        case WEAR_ABOUT:  loc_flag=ITEM_WEAR_ABOUT;   break;
        case WEAR_WAIST:  loc_flag=ITEM_WEAR_WAIST;   break;
        case WEAR_WRIST_L:
        case WEAR_WRIST_R:    loc_flag=ITEM_WEAR_WRIST;   break;
        case WEAR_FACE:   loc_flag=ITEM_WEAR_FACE;    break;
        case WEAR_EAR_L:
        case WEAR_EAR_R:  loc_flag=ITEM_WEAR_EAR;         break;
        case WEAR_SHOULDERS:  loc_flag=ITEM_WEAR_SHOULDERS;   break;
        case WEAR_HOLD_HAND_L:
        case WEAR_HOLD_HAND_R: loc_flag=ITEM_WEAR_HOLD_HAND;        break;
        case WEAR_EYES:   loc_flag=ITEM_WEAR_EYES;    break;

        default:      loc_flag=0;         break;
    }

    if (  (  (obj->wear_flags & loc_flag ) == 0 )
        && ( obj->wear_loc == -1 )  )
    {
        send_to_char( "That item can't be worn there!\n\r", ch );
        return FALSE;
    }
    if ( obj->pIndexData->vnum == 505 && get_eq_char(ch,location) )
    {
        //	send_to_char( "Searching...\n\r", ch );
        return FALSE;
    }

    return TRUE;
}

/*
 * Wear one object.
 * Optional replacement of existing objects.
 * Big repetitive code, ick.
 */
void wear_obj( CHAR_DATA *ch, OBJ_DATA *obj, bool fReplace )
{
    if (  ( CAN_WEAR( obj, ITEM_WEAR_FINGER ) )
        && ( can_wear_at( ch, obj, WEAR_FINGER_L ) || can_wear_at( ch, obj, WEAR_FINGER_R ) )  )
    {
        if ( get_eq_char( ch, WEAR_FINGER_L ) != NULL
            &&   get_eq_char( ch, WEAR_FINGER_R ) != NULL
            &&   !remove_obj( ch, WEAR_FINGER_L, fReplace )
            &&   !remove_obj( ch, WEAR_FINGER_R, fReplace ) )
            return;

        if ( get_eq_char( ch, WEAR_FINGER_L ) == NULL )
        {
            act( "$n slips $p onto $s left finger.",    ch, obj, NULL, TO_ROOM );
            act( "You slip $p onto your left finger.",  ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_FINGER_L );
            return;
        }

        if ( get_eq_char( ch, WEAR_FINGER_R ) == NULL )
        {
            act( "$n slips $p onto $s right finger.",   ch, obj, NULL, TO_ROOM );
            act( "You slip $p onto your right finger.", ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_FINGER_R );
            return;
        }

        send_to_char( "You already wear two rings.\n\r", ch );
        return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_NECK ) )
        && ( can_wear_at( ch, obj, WEAR_NECK_1 ) || can_wear_at( ch, obj, WEAR_NECK_2 ) )  )
    {
        if ( get_eq_char( ch, WEAR_NECK_1 ) != NULL
            &&   get_eq_char( ch, WEAR_NECK_2 ) != NULL
            &&   !remove_obj( ch, WEAR_NECK_1, fReplace )
            &&   !remove_obj( ch, WEAR_NECK_2, fReplace ) )
            return;

        if ( get_eq_char( ch, WEAR_NECK_1 ) == NULL )
        {
            act( "$n fastens $p around $s neck.",   ch, obj, NULL, TO_ROOM );
            act( "You fasten $p around your neck.", ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_NECK_1 );
            return;
        }

        if ( get_eq_char( ch, WEAR_NECK_2 ) == NULL )
        {
            act( "$n fastens $p around $s neck.",   ch, obj, NULL, TO_ROOM );
            act( "You fasten $p around your neck.", ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_NECK_2 );
            return;
        }

        bug( "Wear_obj: no free neck.", 0 );
        send_to_char( "You already wear two neck items.\n\r", ch );
        return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_BODY ) )
        && ( can_wear_at( ch, obj, WEAR_BODY ) )  )
    {
        if ( !remove_obj( ch, WEAR_BODY, fReplace ) )
            return;
        act( "$n pulls $p down over $s body.",   ch, obj, NULL, TO_ROOM );
        act( "You pull $p down over your body.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_BODY );
        return;
    }

    if ( ( CAN_WEAR( obj, ITEM_WEAR_HEAD ) )
        && ( can_wear_at( ch, obj, WEAR_HEAD ) )  )
    {
        if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
            return;
        act( "$n places $p onto $s head.",   ch, obj, NULL, TO_ROOM );
        act( "You place $p onto your head.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_HEAD );
        return;
    }
    /*    else if ( CAN_WEAR( obj, ITEM_WEAR_HEAD )
                    && can_wear_at( ch, obj, WEAR_HEAD ) )
        {
        if ( !remove_obj( ch, WEAR_HEAD, fReplace ) )
            return;
        act( "$n places $p onto $s head.",   ch, obj, NULL, TO_ROOM );
        act( "You place $p onto your head.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_HEAD );
        return;
        }
        if ( CAN_WEAR( obj, ITEM_WEAR_HANDS ) )
        {
              if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
                return;
          act( "$n places $p over $s hands.",   ch, obj, NULL, TO_ROOM );
          act( "You place $p over your hands.", ch, obj, NULL, TO_CHAR );
          equip_char( ch, obj, WEAR_HANDS );
          return;
        } else */
    if ( CAN_WEAR( obj, ITEM_WEAR_HANDS )                   
        && can_wear_at( ch, obj, WEAR_HANDS ) )
    {
        if ( !remove_obj( ch, WEAR_HANDS, fReplace ) )
            return;
        act( "$n pulls $p over $s hands.",   ch, obj, NULL, TO_ROOM );
        act( "You pull $p over your hands.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_HANDS );
        return;
    }

    /*    if ( CAN_WEAR( obj, ITEM_WEAR_FEET ) )
        {
              if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
                return;
          act( "$n slides $s feet into $p.",   ch, obj, NULL, TO_ROOM );
          act( "You slide your feet into $p.", ch, obj, NULL, TO_CHAR );
          equip_char( ch, obj, WEAR_FEET );
          return;
        } else */
    if ( CAN_WEAR( obj, ITEM_WEAR_FEET )                    
        && can_wear_at( ch, obj, WEAR_FEET )  )
    {
        if ( !remove_obj( ch, WEAR_FEET, fReplace ) )
            return;
        act( "$n slides $s feet into $p.",   ch, obj, NULL, TO_ROOM );
        act( "You slide your feet into $p.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_FEET );
        return;
    }

    /*    if ( CAN_WEAR( obj, ITEM_WEAR_FACE ) )
        {
              if ( !remove_obj( ch, WEAR_FACE, fReplace ) )
                return;
          act( "$n places $p on $s face.",   ch, obj, NULL, TO_ROOM );
          act( "You place $p on your face.", ch, obj, NULL, TO_CHAR );
          equip_char( ch, obj, WEAR_FACE );
          return;
        } else*/
    if ( CAN_WEAR( obj, ITEM_WEAR_FACE )                    
        && can_wear_at( ch, obj, WEAR_FACE ) )
    {
        if ( !remove_obj( ch, WEAR_FACE, fReplace ) )
            return;
        act( "$n wears $p on $s face.",   ch, obj, NULL, TO_ROOM );
        act( "You wear $p on your face.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_FACE );
        return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_EYES ) )
        && ( can_wear_at( ch, obj, WEAR_EYES ) )  )
    {
        if ( !remove_obj( ch, WEAR_EYES, fReplace ) )
            return;
        act( "$n wears $p over $s eyes.",   ch, obj, NULL, TO_ROOM );
        act( "You wear $p over your eyes.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_EYES );
        return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_LEGS ) )
        && ( can_wear_at( ch, obj, WEAR_LEGS ) )  )
    {
        if ( !remove_obj( ch, WEAR_LEGS, fReplace ) )
            return;
        act( "$n pushes $s legs into $p.",   ch, obj, NULL, TO_ROOM );
        act( "You push your legs into $p.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_LEGS );
        return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_ARMS ) )
        && ( can_wear_at( ch, obj, WEAR_ARMS ) )  )
    {
        if ( !remove_obj( ch, WEAR_ARMS, fReplace ) )
            return;
        act( "$n pushes $p onto $s arms.",   ch, obj, NULL, TO_ROOM );
        act( "You push $p onto your arms.",  ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_ARMS );
        return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_ABOUT ) )
        && ( can_wear_at( ch, obj, WEAR_ABOUT ) )  )
    {
        if ( !remove_obj( ch, WEAR_ABOUT, fReplace ) )
            return;
        act( "$n fixes $p around $s body.",   ch, obj, NULL, TO_ROOM );
        act( "You fix $p around your body.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_ABOUT );
        return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_WAIST ) )
        && ( can_wear_at( ch, obj, WEAR_WAIST ) )  )
    {
        if ( !remove_obj( ch, WEAR_WAIST, fReplace ) )
            return;
        act( "$n secures $p about $s waist.",   ch, obj, NULL, TO_ROOM );
        act( "You secure $p about your waist.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_WAIST );
        return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_WRIST ) )
        && ( can_wear_at( ch, obj, WEAR_WRIST_L ) || can_wear_at( ch, obj, WEAR_WRIST_R) )  )
    {
        if ( get_eq_char( ch, WEAR_WRIST_L ) != NULL
            &&   get_eq_char( ch, WEAR_WRIST_R ) != NULL
            &&   !remove_obj( ch, WEAR_WRIST_L, fReplace )
            &&   !remove_obj( ch, WEAR_WRIST_R, fReplace ) )
            return;

        if ( get_eq_char( ch, WEAR_WRIST_L ) == NULL )
        {
            act( "$n slides $p onto $s left wrist.", ch, obj, NULL, TO_ROOM );
            act( "You slide $p onto your left wrist.", ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_WRIST_L );
            return;
        }

        if ( get_eq_char( ch, WEAR_WRIST_R ) == NULL )
        {
            act( "$n slides $p onto $s right wrist.",
                ch, obj, NULL, TO_ROOM );
            act( "You slide $p onto your right wrist.",
                ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_WRIST_R );
            return;
        }

        bug( "Wear_obj: no free wrist.", 0 );
        send_to_char( "You already wear two wrist items.\n\r", ch );
        return;
    }

    #if 0
    if ( CAN_WEAR( obj, ITEM_WEAR_SHIELD ) )
    {
        if ( get_eq_char( ch, WEAR_WIELD_2 ) != NULL )
        {
            send_to_char( "Cannot use a shield when dual wielding.\n\r", ch );
            return;
        }

        if ( !remove_obj( ch, WEAR_SHIELD, fReplace ) )
            return;
        act( "$n starts using $p as a shield.", ch, obj, NULL, TO_ROOM );
        act( "You start using $p as a shield.", ch, obj, NULL, TO_CHAR );
        equip_char( ch, obj, WEAR_SHIELD );
        return;
    }

    if ( CAN_WEAR( obj, ITEM_WIELD ) )
    {
        /*
         * Need to handle dual wielding here....
         * To DW, player must have no shield or held items
         * as well as the weapon meeting weight restrictions
         */

        /* First, work out if this is a DW */
        if (    !fReplace
                                                            /* first weapon slot used */
            && */ if ( ( get_eq_char( ch, WEAR_WIELD ) != NULL )
            && ( IS_NPC( ch )
            ? IS_SET( ch->skills, MOB_DUALWIELD ) )
            )
        {
            if ( get_eq_char( ch, WEAR_SHIELD ) != NULL )
            {
                send_to_char( "To dual wield a weapon, you must remove any item used as a shield.\n\r", ch );
                    return;
            }
            if ( get_eq_char( ch, WEAR_HOLD ) != NULL )
            {
                send_to_char( "To dual wield a weapon, you must remove any items you are holding.\n\r", ch );
                    return;
            }
            if ( get_obj_weight( obj ) + 0 > str_app[curr_attr(ch, ATTR_STR)].wield )
                /* To set a weight restrict /^\ change this value */
            {
                send_to_char( "It is too heavy to dual wield.\n\r", ch );
                    return;
            }

            if ( !remove_obj( ch, WEAR_WIELD_2, fReplace ) )
                return;

                act( "$n wields $p as a second weapon.", ch, obj, NULL, TO_ROOM );
                act( "You wield $p as a second weapon.", ch, obj, NULL, TO_CHAR );
                equip_char( ch, obj, WEAR_WIELD_2 );
                return;
        }

        if ( !remove_obj( ch, WEAR_WIELD, fReplace ) )
            return;

            if ( get_obj_weight( obj ) > str_app[curr_attr(ch, ATTR_STR)].wield )
        {
            send_to_char( "It is too heavy for you to wield.\n\r", ch );
                return;
        }

        act( "$n wields $p.", ch, obj, NULL, TO_ROOM );
            act( "You wield $p.", ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_WIELD );
            return;
    }

    if ( CAN_WEAR( obj, ITEM_HOLD ) )
    {
        if ( get_eq_char( ch, WEAR_WIELD_2 ) != NULL )
        {
            send_to_char( "Cannot hold objects when dual wielding.\n\r", ch );
                return;
        }

        if ( !remove_obj( ch, WEAR_HOLD, fReplace ) )
            return;
            act( "$n holds $p in $s hands.",   ch, obj, NULL, TO_ROOM );
            act( "You hold $p in your hands.", ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_HOLD );
            return;
    }
    #endif
    if (  ( CAN_WEAR( obj, ITEM_WEAR_EAR ) )
        && ( can_wear_at( ch, obj, WEAR_EAR_L ) || can_wear_at( ch, obj, WEAR_EAR_R ) )  )
    {
        if ( get_eq_char( ch, WEAR_EAR_L ) != NULL
            &&   get_eq_char( ch, WEAR_EAR_R ) != NULL
            &&   !remove_obj( ch, WEAR_EAR_L, fReplace )
            &&   !remove_obj( ch, WEAR_EAR_R, fReplace ) )
            return;

            if ( get_eq_char( ch, WEAR_EAR_L ) == NULL )
        {
            act( "$n clips $p on $s left ear.",   ch, obj, NULL, TO_ROOM );
                act( "You clip $p on your left ear.", ch, obj, NULL, TO_CHAR );
                equip_char( ch, obj, WEAR_EAR_L );
                return;
        }

        if ( get_eq_char( ch, WEAR_EAR_R ) == NULL )
        {
            act( "$n clips $p on $s right ear.",   ch, obj, NULL, TO_ROOM );
                act( "You clip $p on your right ear.", ch, obj, NULL, TO_CHAR );
                equip_char( ch, obj, WEAR_EAR_R );
                return;
        }

        bug( "Wear_obj: no free finger.", 0 );
            send_to_char( "You already wear two rings.\n\r", ch );
            return;
    }

    if (  ( CAN_WEAR( obj, ITEM_WEAR_SHOULDERS ) )
        && ( can_wear_at( ch, obj, WEAR_SHOULDERS ) )  )
    {
        if ( !remove_obj( ch, WEAR_SHOULDERS, fReplace ) )
            return;
            act( "$n slides $p onto $s shoulders.",   ch, obj, NULL, TO_ROOM );
            act( "You slide $p onto your shoulders.", ch, obj, NULL, TO_CHAR );
            equip_char( ch, obj, WEAR_SHOULDERS );
            return;
    }
    if (  ( CAN_WEAR( obj, ITEM_WEAR_HOLD_HAND ) )
        && ( can_wear_at( ch, obj, WEAR_HOLD_HAND_L ) )  )
    {
        if ( get_eq_char( ch, WEAR_HOLD_HAND_L ) != NULL
            &&   get_eq_char( ch, WEAR_HOLD_HAND_R ) != NULL
            &&   !remove_obj( ch, WEAR_HOLD_HAND_L, fReplace )
            &&   !remove_obj( ch, WEAR_HOLD_HAND_R, fReplace ) )
            return;

            if ( get_eq_char( ch, WEAR_HOLD_HAND_L ) == NULL )
        {
            act( "$n holds $p in $s left hand.", ch, obj, NULL, TO_ROOM );
                act( "You hold $p in your left hand.", ch, obj, NULL, TO_CHAR );
                equip_char( ch, obj, WEAR_HOLD_HAND_L );
                return;
        }

        if ( get_eq_char( ch, WEAR_HOLD_HAND_R ) == NULL )
        {
            act( "$n holds $p in $s right hand.",
                ch, obj, NULL, TO_ROOM );
                act( "You hold $p in your right hand.",
                ch, obj, NULL, TO_CHAR );
                equip_char( ch, obj, WEAR_HOLD_HAND_R );
                return;
        }

        bug( "Wear_obj: no free hand.", 0 );
            send_to_char( "You already hold two items.\n\r", ch );
            return;
    }

    #if 0
    if ( fReplace )
        send_to_char( "Well, that didn't work!\n\r", ch );
        #endif

        send_to_char( "You can't wear it.\n\r", ch );
        return;
}

char *  format_obj_to_char      args( ( OBJ_DATA *obj, CHAR_DATA *ch,
bool fShort ) );

void do_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
        OBJ_DATA *obj;
        sh_int  num_unique = 0;

    {
        OBJ_DATA *obj_next;

            for ( obj = ch->first_carry; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_in_carry_list;
                if  ( obj->wear_loc != WEAR_NONE && IS_SET( obj->extra_flags, ITEM_UNIQUE ) )
                num_unique++;
        }
    }

    one_argument( argument, arg );

        if ( arg[0] == '\0' )
    {
        sh_int  location;
            char      outbuf[MSL];
            char      catbuf[MSL];
            char    colbuf[MSL],eqbuf[MSL];
            OBJ_DATA * worn;
            extern char * const where_name [];

            sprintf( outbuf, "%s", "Wear slots:\n\r" );

            for ( location = 0; location < MAX_WEAR ; location++ )
        {
            if ( (worn = get_eq_char( ch, location ) ) != NULL )
            {
                sprintf( colbuf, "%s", "@@!" );
                    sprintf( eqbuf, "%s", format_obj_to_char( worn, ch, TRUE ) );
            }
            else
            {
                sprintf( colbuf, "%s", "@@." );
                    sprintf( eqbuf, "%s", "@@dNothing@@N" );
            }
            sprintf( catbuf, "%s%25s@@N %-*s\n\r", colbuf, where_name[location],
                ccode_len( eqbuf, 40 ), eqbuf );
                safe_strcat( MSL, outbuf, catbuf );

        }
        send_to_char(  outbuf, ch );
            return;
    }
    else if ( !str_cmp(arg,"status") )
    {
        sh_int  location;
            char      outbuf[MSL];
            char      catbuf[MSL];
            char    colbuf[MSL],eqbuf[MSL];
            OBJ_DATA * worn;
            extern char * const where_name [];
            int heat;
            int general=0,bullet=0,blast=0,fire=0,laser=0,acid=0,sound=0;

            heat = ch->heat + wildmap_table[map_table.type[ch->x][ch->y][ch->z]].heat;

            sprintf( outbuf, "%s", "Wear slots:\n\r" );

            for ( location = 0; location < MAX_WEAR ; location++ )
        {
            if ( (worn = get_eq_char( ch, location ) ) != NULL )
            {
                sprintf( colbuf, "%s", "@@d" );
                    if ( worn->item_type == ITEM_ARMOR )
                {
                    sprintf( eqbuf, "%d%% %s", worn->level, ( worn->value[0] == -2 ) ? "All" : ( worn->value[0] == 1 ) ? "Bulletproof" : (worn->value[0] == 2) ? "Blastproof" : (worn->value[0] == 3 ) ? "Acidproof" : ( worn->value[0] == 4 ) ? "Flameproof" : (worn->value[0]==5) ? "Laserproof" : (worn->value[0] == -1) ? "General" : "INVALID!" );
                        if ( worn->value[1] > 0 )
                        sprintf( eqbuf+strlen(eqbuf), " (%d Damage)", worn->value[1] );
                        general += worn->value[2];
                        bullet += worn->value[3];
                        blast += worn->value[4];
                        acid += worn->value[5];
                        fire += worn->value[6];
                        laser += worn->value[7];
                        sound += worn->value[8];

                }
                else if ( worn->item_type == ITEM_WEAPON )
                {
                    sprintf( eqbuf, "%d/%d Ammo, %d Range", worn->value[0], worn->value[1], worn->value[4]+1);
                }
                else
                {
                    sprintf( colbuf, "%s", "@@." );
                        sprintf( eqbuf, "%s", "@@dNothing@@N" );
                }
            }
            else
            {
                sprintf( colbuf, "%s", "@@." );
                    sprintf( eqbuf, "%s", "@@dNothing@@N" );
            }
            sprintf( catbuf, "%s%25s@@N %-*s\n\r", colbuf, where_name[location],
                ccode_len( eqbuf, 40 ), eqbuf );
                safe_strcat( MSL, outbuf, catbuf );
        }
        send_to_char(  outbuf, ch );
            sprintf( outbuf, "\n\r@@RHeat: @@e%d@@N\n\r", heat) ;
            send_to_char(  outbuf, ch );
            sprintf( outbuf, "\n\rProtection values:\n\r" );
            sprintf( outbuf+strlen(outbuf), "General: %d%% (%d%% Average)\n\r", general, general / MAX_WEAR );
            sprintf( outbuf+strlen(outbuf), "Bullets: %d%% (%d%% Average)\n\r", bullet, bullet / MAX_WEAR );
            sprintf( outbuf+strlen(outbuf), "Blast  : %d%% (%d%% Average)\n\r", blast, blast / MAX_WEAR );
            sprintf( outbuf+strlen(outbuf), "Acid   : %d%% (%d%% Average)\n\r", acid, acid / MAX_WEAR );
            sprintf( outbuf+strlen(outbuf), "Fire   : %d%% (%d%% Average)\n\r", fire, fire / MAX_WEAR );
            sprintf( outbuf+strlen(outbuf), "Laser  : %d%% (%d%% Average)\n\r", laser, laser / MAX_WEAR );
            sprintf( outbuf+strlen(outbuf), "Sound  : %d%% (%d%% Average)\n\r", sound, sound / MAX_WEAR );
            send_to_char( outbuf,ch);

            return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        OBJ_DATA *obj_next;

            for ( obj = ch->first_carry; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_in_carry_list;
                if ( obj->wear_loc == WEAR_NONE && can_see_obj( ch, obj ) )
            {
                if ( ( num_unique > 4 ) && ( IS_SET( obj->extra_flags, ITEM_UNIQUE ) ) )
                    send_to_char( "You may only wear 5 unique items at one time.\n\r", ch );
                    else
                    wear_obj( ch, obj, FALSE );
            }
            if ( IS_SET( obj->extra_flags, ITEM_UNIQUE ) )
                num_unique++;
        }

        return;
    }
    else
    {
        if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
        {
            send_to_char( "You do not have that item.\n\r", ch );
                return;
        }
        if (  ( num_unique > 4 )
            && ( IS_SET( obj->extra_flags, ITEM_UNIQUE ) )  )
        {
            send_to_char(" You may only wear 5 unique items at one time.\n\r", ch );
        }
        else
            wear_obj( ch, obj, TRUE );
    }

    return;
}

void remove_all ( CHAR_DATA *ch )
{
    int      counter = 0;

        while ( counter < MAX_WEAR )
    {
        remove_obj ( ch, counter, TRUE );
            counter = counter + 1;
    }

    return;
}

void do_remove( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
        OBJ_DATA *obj;

        one_argument( argument, arg );

        if ( arg[0] == '\0' )
    {
        send_to_char( "Remove what?\n\r", ch );
            return;
    }

    if ( !str_cmp ( arg, "all" ) )
    {
        remove_all ( ch );
            return;
    }

    if ( ( obj = get_obj_wear( ch, arg ) ) == NULL )
    {
        send_to_char( "You do not have that item.\n\r", ch );
            return;
    }

    remove_obj( ch, obj->wear_loc, TRUE );
        return;
}

void do_sacrifice( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
        char arg[MSL];
        extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

        if ( argument[0] == '\0' )
    {
        send_to_char( "Junk what?\n\r", ch );
            return;
    }
    argument = one_argument(argument,arg);
        if ( !str_cmp(arg,"all") )
    {
        OBJ_DATA *obj_next;
            bool found = FALSE;

            if ( argument[0] == '\0' )
        {
            send_to_char( "Junk all what?\n\r", ch );
                return;
        }
        for ( obj = ch->first_carry;obj;obj = obj_next )
        {
            obj_next = obj->next_in_carry_list;
                if ( is_name(argument, obj->name) )
            {
                do_sacrifice(ch,argument);
                    found = TRUE;
            }
        }
        if ( !found )
            send_to_char( "You don't have any of those.\n\r", ch );
            return;
    }
    if ( ( obj = get_obj_carry(ch,arg) ) == NULL )
    {
        if ( ( obj = get_obj_room( ch, arg, map_obj[ch->x][ch->y] ) ) == NULL )
        {
            send_to_char( "You are not carrying that item.\n\r", ch );
                return;
        }
    }
    if ( obj->carried_by == NULL && str_cmp(obj->owner,ch->name) )
    {
        send_to_char( "It's not yours to junk!\n\r", ch );
            return;
    }
    if ( obj->z == Z_SPACE && obj->carried_by == NULL )
    {
        send_to_char( "You can't junk items in space.\n\r", ch );
            return;
    }
    if ( obj->item_type == ITEM_BOMB && obj->value[1] != 0 )
    {
        send_to_char( "Junk an armed bomb? Heh, you wish!\n\r", ch );
            return;
    }
    if ( IS_SET(obj->extra_flags,ITEM_STICKY) )
    {
        send_to_char( "You can't junk that.\n\r", ch );
            return;
    }
    act( "You junk $p.", ch, obj, NULL, TO_CHAR );
        act( "$n junks $p.", ch, obj, NULL, TO_ROOM );
        if ( IS_SET(obj->extra_flags,ITEM_STICKY) )
    {
        char buf[MSL];
            sprintf( buf, "%s junked by %s (Sticky)", obj->short_descr, ch->name );
            log_f(buf);
            return;
    }
    extract_obj( obj );
        return;
}

void do_swap( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj1;
        OBJ_DATA *obj2;
        char buf[MSL];

        if ( paintball(ch) )
    {
        send_to_char( "Not here.\n\r", ch );
            return;
    }
    if ( ( obj1 = get_eq_char(ch, WEAR_HOLD_HAND_L) ) == NULL || ( obj2 = get_eq_char(ch, WEAR_HOLD_HAND_R) ) == NULL )
    {
        send_to_char( "You must be holding weapons in both hands in order to use SWAP.\n\r", ch );
            return;
    }
    /*	if ( obj1->item_type != ITEM_WEAPON || obj2->item_type != ITEM_WEAPON )
        {
            sprintf( buf, "%s is not a weapon!\n\r", ( obj1->item_type != ITEM_WEAPON ) ? obj1->short_descr : obj2->short_descr );
            send_to_char( buf, ch );
            return;
        }*/

    remove_obj( ch, obj1->wear_loc, FALSE );
        remove_obj( ch, obj2->wear_loc, FALSE );
        obj1->wear_loc = WEAR_HOLD_HAND_R;
        obj2->wear_loc = WEAR_HOLD_HAND_L;
        sprintf( buf, "You swap %s and %s!", obj1->short_descr, obj2->short_descr );
        if ( obj2->item_type == ITEM_WEAPON )
        sprintf(buf+strlen(buf), " (New weapon has %d ammo)\n\r",obj2->value[0]);
        if ( ch->victim != ch )
        ch->victim = ch;
        send_to_char( buf, ch );
        act( "$n swaps weapons!", ch, NULL, NULL, TO_ROOM );
        return;
}

void do_clean(CHAR_DATA *ch, char *argument)
{
    OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
        if ( !ch->in_building || str_cmp(ch->in_building->owned,ch->name) )
    {
        send_to_char ("You can't here.\n\r", ch );
            return;
    }
    for ( obj = map_obj[ch->x][ch->y];obj;obj = obj_next )
    {
        obj_next = obj->next_in_room;
            if ( !str_cmp(obj->owner,ch->name) && obj->z == ch->z )
            extract_obj(obj);
    }
    send_to_char( "Room cleaned.\n\r", ch );
        if ( WAREHOUSE(ch->in_building) )
    {
        char buf[MSL];
            sprintf(buf,"%s cleaned a warehouse.\n\r", ch->name );
            log_f(buf);
    }
    return;
}

void do_donate( CHAR_DATA *ch, char *argument )
{
    char arg[MSL];
        CHAR_DATA *victim;
        OBJ_DATA  *obj;

        if ( ch->in_vehicle && ch->in_vehicle->type != VEHICLE_MECH )
    {
        send_to_char( "Not while driving.\n\r", ch );
            return;
    }
    if ( ch->z == Z_PAINTBALL || ch->z == Z_NEWBIE )
    {
        send_to_char( "You can't trade in here - Trade in the real world, where there is real danger.\n\r", ch );
            return;
    }
    argument = one_argument(argument,arg);

        if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char( "Syntax: Donate <item> <player>\n\r", ch );
            return;
    }
    if ( ( victim = get_char_world(ch,argument) ) == NULL )
    {
        send_to_char( "You cannot find that player.\n\r", ch );
            return;
    }
    if ( get_rank(victim) > 2 || !IS_NEWBIE(victim) )
    {
        send_to_char( "You may only send items to -new- players of Rank 2 and below.\n\r", ch );
            return;
    }

    if ( ( obj = get_obj_carry(ch,arg) ) == NULL )
    {
        send_to_char( "You do not carry that item.\n\r", ch );
            return;
    }
    if ( obj->item_type != ITEM_MATERIAL )
    {
        send_to_char( "You may only donate resources to newbies.\n\r", ch );
            return;
    }

    if ( !can_drop_obj( ch, obj ) )
    {
        send_to_char( "You can't let go of it.\n\r", ch );
            return;
    }
    if ( victim->carry_number + get_obj_number( obj ) > can_carry_n( victim ) )
    {
        act( "$N has $S hands full.", ch, NULL, victim, TO_CHAR );
            return;
    }

    if ( victim->carry_weight + get_obj_weight( obj ) > can_carry_w( victim ) )
    {
        act( "$N can't carry that much weight.", ch, NULL, victim, TO_CHAR );
            return;
    }

    if ( !can_see_obj( victim, obj ) )
    {
        act( "$N can't see it.", ch, NULL, victim, TO_CHAR );
            return;
    }
    obj_from_char( obj );
        obj_to_char( obj, victim );
        act( "You donate $p to $N.", ch, obj, victim, TO_CHAR );
        act( "$n donates $p to you from afar.", ch, obj, victim, TO_VICT );
        save_char_obj(ch);
        save_char_obj(victim);
        return;

}
