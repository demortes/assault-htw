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
#include <stdlib.h>                                         /* For div_t, div() */
#include <string.h>
#include <time.h>
#include "ack.h"
#include "tables.h"
#include <math.h>

/*
 * Local functions.
 */
void    one_hit         args( ( CHAR_DATA *ch, CHAR_DATA *victim, int dt ) );
void    raw_kill        args( ( CHAR_DATA *victim, char *argument ) );
void    disarm          args( ( CHAR_DATA *ch, CHAR_DATA *victim, OBJ_DATA * obj ) );
void    air_bomb    args( ( CHAR_DATA *ch ) );
bool    check_group args( ( CHAR_DATA *ch, CHAR_DATA *vch ) );
bool    no_lag      args( ( CHAR_DATA *ch, CHAR_DATA *vch ) );

/*
 * Inflict damage from a hit.
 */
void damage( CHAR_DATA *ch, CHAR_DATA *victim, int dam, int dt )
{
    int loc = 0;
    if ( victim->is_free == TRUE )
    {
        bug( "Freed victim in one_hit", 0 );
        return;
    }

    if ( !victim )
        return;
    if ( !ch )
        ch = victim;

    if ( victim->dead )
        return;

    if ( victim->z == Z_NEWBIE || ch->z == Z_NEWBIE )
        return;
    if ( victim->z == Z_PAINTBALL && victim->x == 2 && victim->y == 2 )
    {
        nuke_blow(victim);
        return;
    }

    if ( practicing(ch) && ch != victim )
        do_practice(ch,"");
    if ( practicing(victim) && ch != victim )
        do_practice(victim,"");
    if ( ch == victim && ch->c_sn == gsn_practice )
        return;

    if ( victim != ch && medal(victim) )
        return;

    if ( ( ( IN_PIT(victim) && !IN_PIT(ch) ) || ( !IN_PIT(victim) && IN_PIT(ch) ) ) || ( paintball(victim) && !paintball(ch) ) )
    {
        send_to_char( "You cannot harm people who are in the pit while you're not!\n\r", ch);
        return;
    }

    if ( paintball(ch) && paintball(victim) )
    {
        char buf[MSL];
        int x;

        if ( dt != DAMAGE_PAINT )
            return;

        victim->fighttimer = 0;
        act( "You've been hit! You're OUT OF THE GAME!", ch, NULL, victim, TO_VICT );
        act( "$N has been hit! $E's OUT OF THE GAME!", ch, NULL, victim, TO_NOTVICT );
        if ( victim->x != ch->x || victim->y != ch->y )
            act( "$N has been hit! $E's OUT OF THE GAME!", ch, NULL, victim, TO_CHAR );

        x = number_range(1,5);
        if ( x == 1 )
            sprintf( buf, "@@a%s @@Wis seeing @@yc@@eo@@al@@po@@rr@@ls@@W, thanks to @@a%s@@W!@@N", victim->name, ch->name );
        else if ( x == 2 )
            sprintf( buf, "@@a%s @@Wpainted @@a%s @@ppink @@Win the paintball arena!@@N", ch->name, victim->name );
        else if ( x == 3 )
            sprintf( buf, "@@a%s @@Wshoves @@lblue paint@@W down @@a%s@@W's throat. No Matrix for you!@@N", ch->name, victim->name );
        else if ( x == 4 )
            sprintf( buf, "@@a%s@@W's paint caused @@a%s @@Wto DYE!@@N", ch->name, victim->name );
        else if ( x == 5 )
            sprintf( buf, "@@a%s@@W \"accidently\" stuffed a @@pCrayon@@W into @@a%s@@W's eye socket!@@N", ch->name, victim->name );
        info( buf, 0);
        if ( ch->victim == victim )
            ch->victim = ch;
        ch->pcdata->pbhits++;
        victim->pcdata->pbdeaths++;
        save_char_obj(ch);
        save_char_obj(victim);
        move( victim,number_range(200,300),number_range(200,300),Z_PAINTBALL);
        if ( ch->c_count > 0 )
            ch->c_count = 0;
        if ( ++victim->c_count >= 10 )
            do_paintball(victim,"idler");
        return;
    }
    /*    if ( check_group(ch,victim) )
        {
        send_to_char( "You cannot attack people in different kill groups.\n\r", ch );
        return;
        } */

    if ( dam < 0 )
        dam = 0;

    if ( has_ability(ch,3) )                                //Peacecraft
        dam -= dam / 10;
    if ( has_ability(victim,3) )
        dam -= dam / 10;
    if ( IS_SET(ch->effect,EFFECT_POSTAL) && dt == DAMAGE_BULLETS )
        dam *= 1.5;

    if ( ch->in_vehicle && SPACE_VESSAL(ch->in_vehicle) )
    {
        dam *= ch->in_vehicle->power[POWER_WEAPONS] / 100;
        if ( ch->in_vehicle->state == VEHICLE_STATE_DEFENSE )
            dam *= 0.8;
        else if ( ch->in_vehicle->state == VEHICLE_STATE_CHARGE )
            dam *= 0.5;
        else if ( ch->in_vehicle->state == VEHICLE_STATE_OFFENSE )
            dam *= 1.2;
    }

    if ( victim->in_vehicle && dt != DAMAGE_PSYCHIC )
    {
        int vtype = victim->in_vehicle->type;
        damage_vehicle(ch, victim->in_vehicle, dam, dt);
        if ( dt == DAMAGE_FLAME && victim->in_vehicle && vtype != VEHICLE_MECH )
        {
            send_to_char ("You feel the heat piercing the vehicle's armor!\n\r", victim );
            dam /= 2;
            if ( victim->in_vehicle )
                if ( IS_SET(victim->in_vehicle->flags,VEHICLE_FIRE_RESISTANT) )
                    dam /= 2;
        }
        else
            return;
    }

    loc = number_range(0,MAX_WEAR);                         // Choose a location to hit the victim (For armor check)
    if ( number_percent() <= 20 || (loc == WEAR_HEAD && number_percent() < 20 ) )
        loc = WEAR_BODY;
    if ( dt == DAMAGE_SOUND || ( number_percent() < ch->pcdata->skill[gsn_combat] * 5 ) )
        loc = WEAR_HEAD;
    if ( ch->position == POS_SNEAKING && number_percent() < 33 )
        loc = WEAR_HEAD;

    if ( loc == WEAR_HEAD && dt != DAMAGE_ENVIRO && dt != DAMAGE_PSYCHIC && dt != DAMAGE_BLAST )
    {
        send_to_char( "@@eHEAD SHOT!@@N\n\r", ch );
        send_to_char( "@@eHEAD SHOT!@@N\n\r", victim );
    }
    if ( dt != DAMAGE_PSYCHIC )
    {
        OBJ_DATA *eq;
        bool wulfskin;                                      // = (IS_SET(victim->effect,EFFECT_WULFSKIN));
        if ( IS_SET(victim->effect,EFFECT_WULFSKIN) )
            wulfskin = TRUE;
        else
            wulfskin = FALSE;
        if ( ( eq = get_eq_char( victim, loc ) ) != NULL )
        {
            if ( eq->item_type == ITEM_ARMOR )
            {
                int chance;
                int armorval = get_armor_value(dt);
                if ( armorval > -1 )
                    chance = eq->value[armorval];
                else
                    chance = 0;

                if ( eq->value[armorval] < eq->level && wulfskin )
                    eq->value[armorval]++;

                chance += (victim->pcdata->skill[gsn_combat] * 10) - (ch->pcdata->skill[gsn_combat]);
                if ( number_percent() < chance )
                {
                    char buf[MSL];
                    if ( number_percent() < chance )
                    {
                        sprintf( buf, "The damage is absorbed by %s! (%d)\n\r", eq->short_descr,dam );
                        send_to_loc(buf,victim->x,victim->y,victim->z);
                        eq->value[1] += (wulfskin)?dam/2:dam;
                        dam = 0;
                    }
                    else
                    {
                        int absorb;
                        absorb= number_range(1,dam - eq->value[1]);
                        sprintf( buf, "The damage is partially absorbed by %s! (%d)\n\r", eq->short_descr,absorb );
                        send_to_loc(buf,victim->x,victim->y,victim->z);
                        eq->value[1] += (wulfskin)?absorb/2:absorb;
                        dam -= absorb;
                    }
                    check_armor(eq);
                }
                else if ( chance < 0 )
                {
                    dam += (dam / 100) * chance;
                }
            }
        }
        else if ( eq == NULL && loc == WEAR_HEAD )
            dam *= 1.5;
    }

    /*
     * Hurt the victim.
     * Inform the victim of his new state.
     */

    victim->hit -= dam;
    update_pos( victim );

    {
        switch( victim->position )
        {
            case POS_MORTAL:
                act( "$n is mortally wounded, and will die soon, if not aided.",
                    victim, NULL, NULL, TO_ROOM );
                send_to_char(
                    "You are mortally wounded, and will die soon, if not aided.\n\r",
                    victim );
                break;

            case POS_INCAP:
                act( "$n is incapacitated and will slowly die, if not aided.",
                    victim, NULL, NULL, TO_ROOM );
                send_to_char(
                    "You are incapacitated and will slowly die, if not aided.\n\r",
                    victim );
                break;

            case POS_DEAD:

                act( "$n @@dis @@2@@aDEAD@@N@@d!!@@N", victim, 0, 0, TO_ROOM );
                send_to_char( "@@dYou have been @@2@@aKILLED@@N@@d!!@@N\n\r\n\r", victim );
                sendsound(ch,"manscream",40,1,25,"combat","manscream.wav");
                break;

            default:
                if ( dam > victim->max_hit / 4 )
                    send_to_char( "That really did HURT!\n\r", victim );
                if ( victim->hit < victim->max_hit / 4 )
                    send_to_char( "You sure are BLEEDING!\n\r", victim );
                break;
        }
    }                                                       /* end of if statement */

    if ( check_dead(ch,victim) )
        return;

    if ( dt != DAMAGE_ENVIRO )
        set_fighting(ch, victim);
    else if ( dam > 50 )
        victim->fighttimer = 240;
    if ( victim == ch )
        return;

    if( victim->c_sn != -1 && victim->c_sn != gsn_move && (number_percent() < 25) )
    {
        send_to_char( "The pain distracts you from what you're doing.\n\r", victim );
        act( "$n stops what $e's doing.", victim, NULL, NULL, TO_ROOM );
        victim->c_sn = -1;
    }

    tail_chain( );
    return;

}

/*
 * Set position of a victim.
 */
void update_pos( CHAR_DATA *victim )
{
    //    char buf[MAX_STRING_LENGTH];
    if ( victim->position == POS_HACKING )
    {
        if ( victim->bvictim )
        {
            victim->bvictim->value[8] = 0;
            victim->bvictim = NULL;
            victim->c_sn = -1;
            send_to_char( "Connection Terminated...\n\r", victim );
            act( "$n pulls $s computer back to $s inventory.", victim, NULL, NULL, TO_ROOM );
        }
    }

    if ( victim->hit <= 0 ) victim->position = POS_DEAD;
    return;
}

void raw_kill( CHAR_DATA *victim, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_DATA *bomb;
    BUILDING_DATA *bld;
    BUILDING_DATA *clone = NULL;
    QUEUE_DATA *q;
    QUEUE_DATA *q_next;
    bool cloned = FALSE;
    bool suicide = FALSE;
    bool ml = FALSE;
    bool bone = FALSE;
    int i,min,rank=get_rank(victim);

    victim->is_free = FALSE;

    min = 0;

    for ( q = victim->pcdata->queue;q;q = q_next )
    {
        q_next = q->next;
        extract_queue(q);
    }
    victim->pcdata->queue = NULL;
    victim->pcdata->last_queue = NULL;
    if ( victim->fighttimer > 0 && !str_cmp(victim->name,argument) )
        suicide = TRUE;

    for ( bld = first_active_building;bld;bld = bld->next_active )
    {
        if ( !bld->active )
            continue;

        if ( bld->value[3] != 0 && !suicide )
            if ( ( !str_cmp(bld->attacker,victim->name) ) || (rank <= 3 && bld->owner == victim) )
                bld->value[3] = 0;

        if ( bld->owner != victim )
            continue;

        if ( !complete(bld) )
            continue;

        if ( !suicide )
        {
            bld->hp = bld->maxhp;
            bld->shield = bld->maxshield;
            bld->value[9] = 0;
            if ( bld->type != BUILDING_SNIPER_TOWER && bld->type != BUILDING_WAR_CANNON && bld->type != BUILDING_TRANSMISSION_TOWER && !defense_building(bld) && bld->type != BUILDING_ARMORY && !WAREHOUSE(bld))
                bld->protection = 15;
        }
        else
        {
            bld->shield = 0;
            bld->timer = 12;
        }
        if ( bld->type == BUILDING_MINING_LAB || build_table[bld->type].requirements == BUILDING_MINING_LAB )
            ml = TRUE;
        if ( bld->type == BUILDING_CLONING_FACILITY )
        {
            clone = bld;
            cloned = TRUE;
        }
    }

    if ( ( bomb = get_eq_char( victim, WEAR_BODY ) ) != NULL )
        if ( bomb->item_type == ITEM_BOMB && ( bomb->value[1] != 1 || bomb->value[0] != 0 ))
    {
        int xx = victim->x;
        int yy = victim->y;

        if ( victim->in_vehicle )
        {
            extract_vehicle(victim->in_vehicle,TRUE);
            victim->in_vehicle = NULL;
        }
        obj_from_char(bomb);
        bomb->x = victim->x;
        bomb->y = victim->y;
        bomb->z = victim->z;
        obj_to_room(bomb,get_room_index(ROOM_VNUM_WMAP));
        move( victim, 0,0,victim->z);
        explode(bomb);
        move( victim, xx, yy, victim->z );
    }

    min = -1;
    for ( i=0;i<100;i++ )
    {
        if ( min == -1 )
        {
            if ( ( score_table[i].kills < victim->pcdata->pkills )
                || ( score_table[i].kills == victim->pcdata->pkills && score_table[i].buildings < victim->pcdata->bkills )
                || ( score_table[i].kills == victim->pcdata->pkills && score_table[i].buildings == victim->pcdata->bkills && score_table[i].time < my_get_hours(victim,FALSE) ) )
                min = i;
        }
        else
        {
            if ( ( score_table[i].kills < score_table[min].kills )
                || ( score_table[i].kills == score_table[min].kills && score_table[min].buildings < score_table[i].buildings )
                || ( score_table[i].kills == score_table[min].kills && score_table[min].buildings == score_table[i].buildings  && score_table[i].time < score_table[min].time ) )
                min = i;
        }

        if ( score_table[i].name == NULL )
        {
            min = i;
            break;
        }
    }
    if ( IS_IMMORTAL(victim) )
        min = -1;
    if ( min >= 0 && min <= 99 )
    {
        if ( score_table[min].name != NULL )
            free_string(score_table[min].name);
        if ( score_table[min].killedby != NULL )
            free_string(score_table[min].killedby);

        score_table[min].name = str_dup(victim->name);
        score_table[min].killedby = str_dup(argument);
        score_table[min].kills = victim->pcdata->pkills;
        score_table[min].buildings = victim->pcdata->bkills;
        score_table[min].time = my_get_hours(victim,FALSE);
        save_scores();
    }

    for ( obj = first_obj;obj;obj = obj_next )
    {
        obj_next = obj->next;
        if ( obj->carried_by )
            continue;
        if ( obj->in_building != NULL && WAREHOUSE(obj->in_building) )
            continue;
        if ( str_cmp(obj->owner,victim->name) )
            continue;
        /*	if ( obj->bomb_data )
            {
                if ( obj->carried_by )
                    obj_from_char(obj);
                obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
                continue;
            } */
        extract_obj(obj);
    }
    for ( obj = victim->first_carry;obj;obj = obj_next )
    {
        obj_next = obj->next_in_carry_list;
        if ( obj->wear_loc == WEAR_FINGER_L || obj->wear_loc == WEAR_FINGER_R )
            if ( obj->pIndexData->vnum == 502 )             //Crystal Rings
                bone = TRUE;
        if ( IS_SET(obj->extra_flags,ITEM_STICKY) )
            continue;
        if ( cloned && number_percent() < clone->level * 19 )
        {
            obj_from_char(obj);
            obj->x = clone->x;
            obj->y = clone->y;
            obj->z = clone->z;
            obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
            obj->in_building = clone;
            continue;
        }
        extract_obj(obj);
    }
    for ( i = 0;i<MAX_SKILL;i++ )
    {
        if ( skill_table[i].prof )
            continue;
        if ( victim->pcdata->skill[i] > 1 )
            victim->pcdata->skill[i] -= 1;
        else
            victim->pcdata->skill[i] = 0;
    }
    if ( !suicide )
    {
        for ( i=0;i<8;i++ )
            if ( victim->refund[i] > 0 )
        {
            obj = create_material(i);
            obj->value[1] = victim->refund[i];
            obj->weight = 1;
            obj_to_char(obj,victim);
            victim->refund[i] = 0;
        }
    }
    if ( victim->in_vehicle )
    {
        victim->in_vehicle->driving = NULL;
        victim->in_vehicle = NULL;
    }
    victim->hit         = UMAX( 1, victim->hit  );
    if ( rank > 3 )
        victim->pcdata->deaths++;
    victim->pcdata->dead = TRUE;
    victim->disease = 0;
    victim->effect = 0;
    victim->implants = 0;
    if ( ml )
    {
        victim->quest_points -= 1000;
        if ( victim->quest_points < 0 )
            victim->quest_points = 0;
    }
    if ( cloned )
        move ( victim, clone->x, clone->y, clone->z );
    if ( victim->fighttimer > 480 )
        victim->fighttimer = 480;
    save_char_obj(victim);
    victim->dead = TRUE;
    victim->c_sn = gsn_dead;
    victim->c_time = 16;
    victim->position = POS_DEAD;
    if ( map_table.type[victim->x][victim->y][victim->z] == SECT_MAGMA )
        map_table.type[victim->x][victim->y][victim->z] = SECT_UNDERGROUND;

    if ( get_rank(victim) < rank )
    {
        char buf[MSL];
        sprintf( buf, "%s has lost a rank.", victim->name );
        info(buf,1);
        update_ranks(victim);
    }
    if ( bone )
    {
        obj = create_object(get_obj_index(OBJ_VNUM_BROKEN_BONE),0);
        obj_to_char(obj,victim);
    }
    return;
}

void pdie(CHAR_DATA *ch)
{
    DESCRIPTOR_DATA *d;
    char buf[MSL];
    CHAR_DATA *victim = ch;
    d = victim->desc;
    sprintf(buf,"%s",victim->name);
    victim->c_sn = -1;
    victim->c_time = 0;
    victim->is_free = FALSE;
    victim->is_quitting = TRUE;
    victim->dead = FALSE;
    if ( !ch->fake )
        victim->pcdata->dead = TRUE;
    victim->c_sn = -1;
    victim->position = POS_STANDING;
    save_char_obj(ch);
    extract_char( victim, TRUE );
    if ( d != NULL )
    {
        //        close_socket( d );
        BUILDING_DATA *bld;
        bool build = FALSE,fOld;
        fOld = load_char_obj( d, buf, FALSE );
        char bbuf[MSL];
        for ( bld = first_building;bld;bld = bld->next )
        {
            if ( !str_cmp(bld->owned,ch->name) )
            {
                build = TRUE;
                break;
            }
        }
        write_to_buffer(d, "Your character has died. You will now get to recreate it!\n\r\n\r", 0 );
        d->connected = CON_GET_RECREATION;
        write_to_buffer( d,"Select one of the following options:\n\r\n\r", 0 );
        write_to_buffer( d,"0. Respawn - From where you last died\n\r", 0 );
        write_to_buffer( d,"2. Random - Pick a random location on the map.\n\r", 0 );
        if ( build )
        {
            sprintf( bbuf, "3. Start at your base - (%d/%d, %s), all buildings lose a level and HPs.\n\r", bld->x, bld->y,bld->name);
            write_to_buffer(d,bbuf,0);
        }
        return;
    }
    return;
}

void do_disarm( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *bomb;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    if ( ( bomb = get_obj_room( ch, argument, map_obj[ch->x][ch->y] ) ) == NULL )
    {
        send_to_char( "There is no such object here.\n\r", ch );
        return;
    }
    if ( bomb->item_type != ITEM_BOMB )
    {
        send_to_char( "That is not a bomb.\n\r", ch );
        return;
    }
    if ( bomb->value[1] == 0 )
    {
        send_to_char( "The bomb is not armed.\n\r", ch );
        return;
    }
    if ( number_percent() < (bomb->value[2]/3) + ch->pcdata->skill[gsn_arm] )
    {
        act( "You have successfully disarmed $p!", ch, bomb, NULL, TO_CHAR );
        act( "$n has successfully disarmed $p!", ch, bomb, NULL, TO_ROOM );
        bomb->value[1] = 0;
    }
    else
    {
        send_to_loc( "Oh oh... Somebody cut the wrong wire...\n\r", ch->x, ch->y, ch->z );
        bomb->value[0] = 1;
    }
    return;
}

void do_sla( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SLAY, spell it out.\n\r", ch );
    return;
}

void do_slay( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Slay whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( ch == victim )
    {
        send_to_char( "Suicide is a mortal sin.\n\r", ch );
        return;
    }

    if ( IS_HERO(victim) )
    {
        send_to_char( "Not on other Immortal / Adept players!\n\r", ch );
        return;
    }

    if ( victim->level >= ch->level )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    act( "You suck the life energy out of $M!",  ch, NULL, victim, TO_CHAR    );
    act( "$n sucks out your life energy!", ch, NULL, victim, TO_VICT    );
    act( "$n sucks out $N's life energy!",  ch, NULL, victim, TO_NOTVICT );
    raw_kill( victim, ch->name );
    return;
}

void do_shoot( CHAR_DATA *ch, char *argument )
{
    char *direc;
    char buf[MSL];
    int x,y,range,accuracy,sect,xx,yy,d,d_next,z=ch->z;
    int dam = 0,dir;
    bool air = FALSE;
    bool xaxis = FALSE;
    BUILDING_DATA *bld;
    OBJ_DATA *weapon;
    CHAR_DATA *victim = NULL;
    CHAR_DATA *vch;

    if ( !ch->victim || ch->victim == NULL )
        ch->victim = ch;
    if ( ch->in_building && GUNNER(ch->in_building) )
    {
        do_gunner_shoot(ch,argument);
        return;
    }
    if ( ch->in_vehicle )
    {
        weapon = vehicle_weapon;
        if ( AIR_VEHICLE(ch->in_vehicle->type) )
        {
            weapon->value[4] = 3;
            if ( ch->z == Z_AIR )
                if ( ch->in_vehicle->type == VEHICLE_BOMBER || ch->in_vehicle->type == VEHICLE_BIO_FLOATER )
                    air = TRUE;
        }
        if ( SPACE_VESSAL(ch->in_vehicle) )
            weapon->value[4] = get_ship_weapon_range(ch->in_vehicle);

        if ( ch->in_vehicle->ammo <= 0 && (argument[0] != '0' || !air) )
        {
            send_to_char( "You are out of ammo!\n\r", ch );
            return;
        }
        sprintf( buf, "%s's weapon", ch->in_vehicle->desc );
        free_string(weapon->short_descr);
        weapon->short_descr = str_dup(buf);
        weapon->value[0] = ch->in_vehicle->ammo;
        weapon->value[2] = ch->in_vehicle->ammo_type;
    }
    else
    {
        if (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL || weapon->item_type != ITEM_WEAPON )
        {
            if (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) == NULL )
            {
                send_to_char( "You aren't holding a weapon!\n\r", ch );
                return;
            }
        }
        if ( weapon->item_type != ITEM_WEAPON )
        {
            send_to_char( "This isn't a gun you're holding!\n\r", ch );
            return;
        }
        if ( weapon->value[0] <= 0 )
        {
            if ( IS_SET(ch->config, CONFIG_SOUND))
                sendsound( ch, "emptychamber", 60, 1, 25, "combat", "emptychamber.wav" );
            send_to_char( "You have no more ammo in that thing!\n\r", ch );
            return;
        }
    }
    x = ch->x;
    y = ch->y;
    range = 1 + weapon->value[4];
    if ( ch->class == CLASS_SNIPER )
        range += 1;
    if ( range < 1 )
        range = 1;
    bld = ch->in_building;
    if ( bld && bld->type == BUILDING_SNIPER_TOWER )
        range += 5;
    if ( ch->victim != ch )
    {
        victim = ch->victim;
        act( "You fire towards $N!", ch, NULL, ch->victim, TO_CHAR );
        act( "$n fires towards $s target!", ch, NULL, NULL, TO_ROOM );
        x = ch->victim->x;
        y = ch->victim->y;
    }
    else
    {
        dir = parse_direction(ch,argument);
        if ( argument[0] == '\0' )
        {
            if ( air )
            {
                air_bomb(ch);
                return;
            }
            direc = "people in the surroundings";
        }
        else if ( dir == DIR_NORTH )
        {
            if ( bld && !bld->exit[DIR_NORTH] )
            {
                send_to_char( "You cannot shoot in that direction.\n\r", ch );
                return;
            }
            direc = "north";
            y+=range;
        }
        else if ( dir == DIR_EAST )
        {
            if ( bld && !bld->exit[DIR_EAST] )
            {
                send_to_char( "You cannot shoot in that direction.\n\r", ch );
                return;
            }
            direc = "east";
            x+=range;
            xaxis = TRUE;
        }
        else if ( dir == DIR_SOUTH )
        {
            if ( bld && !bld->exit[DIR_SOUTH] )
            {
                send_to_char( "You cannot shoot in that direction.\n\r", ch );
                return;
            }
            direc = "south";
            y-=range;
        }
        else if ( dir == DIR_WEST )
        {
            if ( bld && !bld->exit[DIR_WEST] )
            {
                send_to_char( "You cannot shoot in that direction.\n\r", ch );
                return;
            }
            direc = "west";
            x-=range;
            xaxis = TRUE;
        }
        else
        {
            send_to_char( "Invalid Direction.\n\r", ch );
            return;
        }
        sprintf( buf, "You fire %s towards the %s!", weapon->short_descr, direc );
        act( buf, ch, NULL, NULL, TO_CHAR );
        sprintf( buf, "$n fires %s towards the %s!", weapon->short_descr, direc );
        act( buf, ch, NULL, NULL, TO_ROOM );
    }
    xx = ch->x;
    yy = ch->y;
    d = -999;
    if ( xaxis )
        d_next = ch->x;
    else
        d_next = ch->y;
    for (;;)
    {
        if ( (xaxis && d == x) || (!xaxis && d == y ))
            break;
        d = d_next;
        if ( ch->victim != ch || victim != NULL )
            break;

        if ( xaxis )
        {
            if ( d < x )
                d_next = d + 1;
            else
                d_next = d - 1;
            xx = d;
        }
        else
        {
            if ( d < y )
                d_next = d + 1;
            else
                d_next = d - 1;
            yy = d;
        }

        if ( get_building(xx,yy,z) )
            continue;
        if (yy == ch->y && xx == ch->x && argument[0] != '\0' )
            continue;
        if ( INVALID_COORDS(xx,yy) )
            break;

        for ( vch = map_ch[xx][yy][z];vch;vch = vch->next_in_room )
        {
            if ( vch == NULL )
                break;
            if ( sysdata.pikamod )
            {
                send_to_char( "Flying... Pikachus? Fly everywhere!\n\r", vch );
            }
            else
            {
                if ( clip_table[weapon->value[2]].type == DAMAGE_BULLETS )
                    send_to_char( "Bullets start flying everywhere!\n\r", vch );
                else if ( clip_table[weapon->value[2]].type == DAMAGE_BLAST )
                    send_to_char( "Rockets start flying everywhere!\n\r", vch );
                else if ( clip_table[weapon->value[2]].type == DAMAGE_ACID )
                    send_to_char( "Acid starts flying everywhere!\n\r", vch );
                else if ( clip_table[weapon->value[2]].type == DAMAGE_FLAME )
                    send_to_char( "Fire starts flying everywhere!\n\r", vch );
                else if ( clip_table[weapon->value[2]].type == DAMAGE_LASER )
                    send_to_char( "Lasers start flying everywhere!\n\r", vch );
                else if ( clip_table[weapon->value[2]].type == DAMAGE_PSYCHIC )
                    send_to_char( "Psychic ripples fill the room!\n\r", vch );
                else if ( clip_table[weapon->value[2]].type == DAMAGE_PAINT )
                    send_to_char( "Paintballs start flying everywhere!\n\r", vch );
                else
                    send_to_char( "Bullets start flying everywhere!", vch );
            }

            accuracy = clip_table[weapon->value[2]].miss+weapon->value[10];
            if ( vch->z == Z_SPACE )
                sect = SECT_NULL;
            else
                sect = map_table.type[x][y][vch->z];
            if ( x != ch->x && y != ch->y )
                accuracy /= 1.1;
            if ( sect == SECT_FOREST )
                accuracy /= 1.2;
            else if ( sect == SECT_FIELD || sect == SECT_ROCK || sect == SECT_SAND )
                accuracy *= 1.2;
            if ( sect != SECT_MOUNTAIN && map_table.type[ch->x][ch->y][ch->z] == SECT_MOUNTAIN )
                accuracy *= 1.2;
            if ( ch != vch )
                accuracy = 75;

            if ( number_percent() < accuracy && vch != ch )
            {
                victim = vch;
                xx = x;
                yy = y;
                break;
            }
        }
    }
    if ( ch->victim != ch )
        victim = ch->victim;
    if ( IS_SET(ch->config,CONFIG_SOUND) )
    {
        if ( clip_table[weapon->value[2]].type == DAMAGE_BULLETS && range < 5 )
            sendsound(ch,"shot1",40,1,25,"combat","shot1.wav");
        else if ( clip_table[weapon->value[2]].type == DAMAGE_BULLETS && range >= 5 )
            sendsound(ch,"sniper",40,1,25,"combat","sniper.wav");
        else if ( clip_table[weapon->value[2]].type == DAMAGE_BLAST )
            sendsound(ch,"missile",40,1,25,"combat","missile.wav");
        else if ( clip_table[weapon->value[2]].type == DAMAGE_LASER && dam > 1000 )
            sendsound(ch,"ioncannon",40,1,25,"combat","ioncannon.wav");
        else if ( clip_table[weapon->value[2]].type == DAMAGE_LASER )
            sendsound(ch,"laser",40,1,25,"combat","laser.wav");
    }
    if ( ( bld = get_building_range(ch->x, ch->y,x,y,z) ) != NULL  && ( x != ch->x || y != ch->y ) && ch->victim == ch )
    {
        if ( !get_ch(bld->owned) && !is_evil(bld) )
        {
            send_to_char( "You are not allowed to fire at a player's buildings while they are not online.\n\r", ch );
            return;
        }
        dam = number_range(clip_table[weapon->value[2]].builddam / 2, clip_table[weapon->value[2]].builddam)+weapon->value[8];
        if ( clip_table[weapon->value[2]].explode )
        {
            int i;
            if ( number_percent() < 50 )
                dam *= 2;

            for ( i=0;i<4;i++ )
                if ( number_percent() < 25 && bld->exit[i] == FALSE )
            {
                bld->exit[i] = TRUE;
                sprintf( buf, "The building's %s wall collapses from the blast.\n\r", i==DIR_NORTH?"Northern":i==DIR_SOUTH?"Southern":i==DIR_EAST?"Eastern":"Western" );
                send_to_char(buf,ch);
            }
        }
        if ( clip_table[weapon->value[2]].type == DAMAGE_EMP )
        {
            bld->shield = 0;
            if ( bld->type == BUILDING_PORTAL )
                bld->value[0] = 1;
        }
        damage_building(ch,bld,dam);
        if ( ch->in_vehicle && !air )
            ch->in_vehicle->ammo--;
        if ( weapon->value[0] != 999 )
            weapon->value[0]--;
        if ( weapon->value[0] <= 0 )
        {
            weapon->value[0] = 0;
            send_to_char( "You are out of ammo!\n\r", ch );
        }
        WAIT_STATE(ch,clip_table[weapon->value[2]].speed+weapon->value[9]);
        return;
    }
    if ( victim != NULL && victim != ch )
    {
        dam = number_range(clip_table[weapon->value[2]].dam / 2, clip_table[weapon->value[2]].dam) + weapon->value[7];
        sprintf( buf, "@@eYou are hit by %s! @@c(@@a%d@@c)@@N\r\n", clip_table[weapon->value[2]].name, dam );
        send_to_char(buf, victim);
        //		act( buf, victim, NULL, NULL, TO_CHAR );
        sprintf(buf, "@@r%s is hit! @@c(@@a%d@@c)@@N\r\n", victim->name, dam);
        send_to_char(buf, ch);
        if ( NOT_IN_ROOM(ch,victim) )
        {
            sprintf( buf, "@@eYour %s hit %s! @@c(@@a%d@@c)@@N\n\r", clip_table[weapon->value[2]].name, victim->name, dam );
            send_to_char(buf,ch);
            sprintf( buf, "@@e$n's %s hit %s! @@c(@@a%d@@c)@@N\r\n", clip_table[weapon->value[2]].name, victim->name, dam );
            act(buf,ch, NULL, NULL, TO_ROOM );
        }
        if ( IS_SET(weapon->value[3], WEAPON_BLINDING) && !IS_SET(victim->effect,EFFECT_BLIND) && number_percent() < 15 )
        {
            SET_BIT(victim->effect,EFFECT_BLIND);
            send_to_char( "Your eyes start bleeding, you can't see!\n\r", victim );
            act( "$n's eyes start bleeding!", victim, NULL, NULL, TO_ROOM );
        }
        if ( IS_SET(weapon->value[3], WEAPON_POISON) && victim->disease == 0  && number_percent() < 15 )
        {
            victim->disease = 20;
            send_to_char( "You become very sick!\n\r", victim );
            act( "$n's becomes sick!", victim, NULL, NULL, TO_ROOM );
        }
        if ( IS_SET(weapon->value[3], WEAPON_CONFUSING) && !IS_SET(victim->effect,EFFECT_CONFUSE) && number_percent() < 15 )
        {
            SET_BIT(victim->effect,EFFECT_CONFUSE);
            send_to_char( "You become very confused!\n\r", victim );
            act( "$n's becomes confused!", victim, NULL, NULL, TO_ROOM );
        }
        if ( IS_SET(weapon->value[3], WEAPON_ALCOHOL) && !IS_SET(victim->effect,EFFECT_DRUNK) )
        {
            SET_BIT(victim->effect,EFFECT_DRUNK);
            send_to_char( "You become very light headed!\n\r", victim );
            act( "$n's becomes light headed!", victim, NULL, NULL, TO_ROOM );
        }
        if ( ch->z != Z_SPACE && ((clip_table[weapon->value[2]].explode && number_percent() < 10) || (has_ability(ch,1) && number_percent() < 15) ))
        {                                                   //Exploding ammo or explosive weapons

            act( "The explosion tears you to pieces!", victim, NULL, NULL, TO_CHAR );
            act( "The explosion tears $n to pieces!", victim, NULL, NULL, TO_ROOM );
            dam = victim->max_hit;
        }
        damage(ch,victim,dam,clip_table[weapon->value[2]].type);
        if ( ch->victim != ch )
        {
            if ( ch->victim->in_room->vnum != ROOM_VNUM_WMAP ||
                (clip_table[weapon->value[2]].speed+weapon->value[9]>=30) ||
                (ch->z != victim->z)                 ||
                (!IS_BETWEEN(victim->x,ch->x-range,ch->x+range)) ||
                (!IS_BETWEEN(victim->y,ch->y-range,ch->y+range)) ||
				((ch->in_building && !open_bld(ch->in_building)) && (ch->victim->x != ch->x || ch->victim->y != ch->y || ch->victim->z != ch->z)) ||
				((ch->victim->in_building && !open_bld(ch->victim->in_building) && (ch->victim->x != ch->x || ch->victim->y != ch->y || ch->victim->z != ch->z))))
            {
                ch->victim = ch;
                send_to_char( "You have lost track of your target.\n\r", ch );
            }
            if ( ch->victim->dead || ch->victim->position == POS_DEAD || ch->victim->c_sn == gsn_dead )
                ch->victim = ch;
        }
    }
    if ( ch->in_vehicle && !air )
        ch->in_vehicle->ammo--;
    else
        weapon->value[0]--;
    if ( weapon->value[0] <= 0 )
    {
        weapon->value[0] = 0;
        send_to_char( "You are out of ammo!\n\r", ch );
    }
    WAIT_STATE(ch,clip_table[weapon->value[2]].speed+weapon->value[9]);
    return;
}

void do_load( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    char arg[MSL];
    OBJ_DATA *weapon;
    OBJ_DATA *clip;
    int temp;
    bool comp = FALSE;

    if ( argument[0] == '\0' )
    {
        send_to_char( "You must specify a clip to load into your weapon or a battery to load into the laptop.\n\r", ch );
        return;
    }
    argument = one_argument(argument,arg);
    if ( ( clip = get_obj_carry( ch, arg ) ) == NULL )
    {
        send_to_char( "You don't have that item.\n\r", ch );
        return;
    }
    if ( clip->item_type == ITEM_AMMO )
        comp = FALSE;
    else if ( clip->item_type == ITEM_BATTERY )
        comp = TRUE;
    else
    {
        sprintf( buf, "You can't load into anything.\n\r" );
        send_to_char( buf, ch );
        return;
    }
    if ( !comp )
    {
        if (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL || weapon->item_type != ITEM_WEAPON )
        {
            if (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) == NULL )
            {
                send_to_char( "You aren't holding a weapon!\n\r", ch );
                return;
            }
        }
        if ( weapon->item_type != ITEM_WEAPON )
        {
            send_to_char( "This isn't a gun you're holding!\n\r", ch );
            return;
        }
        if ( clip->value[0] != weapon->value[2] || clip->item_type != ITEM_AMMO )
        {
            sprintf( buf, "You can't load %s into %s.\n\r", clip->short_descr, weapon->short_descr );
            send_to_char( buf, ch );
            return;
        }
        if ( clip->value[1] > weapon->value[1] )
        {
            send_to_char( "The clip is too large for this weapon.\n\r", ch );
            return;
        }
        if ( clip_table[weapon->value[2]].explode && weapon->value[0] > 0 )
        {
            send_to_char( "You can't take that out.\n\r", ch );
            return;
        }
        temp = weapon->value[0];
        weapon->value[0] = clip->value[1];
        clip->value[1] = temp;
        if ( IS_SET(ch->config, CONFIG_SOUND))
            sendsound(ch,"loadclip",100,1,50,"misc","loadclip.wav");
        sprintf( buf, "You take out an old %s from %s, and pop %s inside.", ( clip_table[weapon->value[2]].explode ) ? "shell" : "clip", weapon->short_descr, clip->short_descr );
        act( buf, ch, NULL, NULL, TO_CHAR );
        sprintf( buf, "$n takes out an old %s from %s, and pop %s inside.", ( clip_table[weapon->value[2]].explode ) ? "shell" : "clip", weapon->short_descr, clip->short_descr );
        act( buf, ch, NULL, NULL, TO_ROOM );
        if ( clip->value[1] > 0 )
        {
            free_string(clip->short_descr);
            free_string(clip->description);
            free_string(clip->name);
            sprintf(buf,"used clip %s",clip_table[weapon->value[2]].name );
            clip->name = str_dup(buf);
            sprintf( buf, "A used clip containing some %s lies half-broken.", clip_table[weapon->value[2]].name );
            clip->description = str_dup(buf);
            sprintf( buf, "A used %s clip", clip_table[weapon->value[2]].name );
            clip->short_descr = str_dup(buf);
            act( "$p still has some ammo in it, you might be able to use it again.", ch, clip, NULL, TO_CHAR );
        }
        else
        {
            if ( !clip_table[weapon->value[0]].explode )
            {
                act( "The old clip is empty. You throw it away.", ch, clip, NULL, TO_CHAR );
                act( "The old is empty. $n throws it away.", ch, clip, NULL, TO_ROOM );
            }
            extract_obj(clip);
        }
        WAIT_STATE(ch,clip_table[clip->value[0]].speed);
    }
    else
    {
        OBJ_DATA *old;
        if ( argument[0] == '\0' )
        {
            send_to_char( "You must specify a laptop to load the battery into.\n\r", ch );
            return;
        }
        if ( ( weapon = get_obj_carry(ch,argument) ) == NULL )
        {
            send_to_char( "You do not carry that item.\n\r", ch );
            return;
        }
        if ( weapon->item_type != ITEM_COMPUTER )
        {
            send_to_char( "This isn't a laptop.\n\r", ch );
            return;
        }
        if ( weapon->value[8] > 0 )
        {
            old = create_object(get_obj_index(32657),0);
            old->value[0] = weapon->value[8];
            old->value[1] = weapon->value[1];
            obj_to_char(old,ch);
            send_to_char( "You pull the old battery out of the laptop.\n\r", ch );
        }
        weapon->value[8] = clip->value[0];
        weapon->value[1] = clip->value[1];
        act( "You load $p into the laptop.", ch, clip, NULL, TO_CHAR );
        act( "$n loads $p into a laptop.", ch, clip, NULL, TO_ROOM );
        extract_obj(clip);
    }
    return;
}

void do_set( CHAR_DATA *ch, char *argument )
{
    char arg[MSL];
    char buf[MSL];
    int time;
    OBJ_DATA *bomb;

    argument = one_argument(argument,arg);
    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char( "Syntax: Set <bomb/all> <time in seconds>\n\r", ch );
        return;
    }
    if ( !is_number(argument) || atoi(argument) < 1 )
    {
        send_to_char( "You must set the bomb to a valid time in seconds.\n\r", ch );
        return;
    }
    time = atoi(argument);
    if ( time > 120 )
    {
        send_to_char( "That's too long. Max is 120 seconds.\n\r", ch );
        return;
    }
    if ( !str_cmp(arg,"all") )
    {
        for ( bomb = ch->first_carry;bomb;bomb = bomb->next_in_carry_list )
        {
            if ( bomb->item_type != ITEM_BOMB )
                continue;
            if ( bomb->value[1] != 0 || bomb->value[4] == 1 || bomb->pIndexData->vnum == 1029 )
                continue;
            bomb->value[0] = time;
            sprintf( buf, "You set %s's timer to %d seconds.", bomb->short_descr, time );
            act( buf, ch, NULL, NULL, TO_CHAR );
            sprintf( buf, "$n sets %s's timer to %d seconds.", bomb->short_descr, time );
            act( buf, ch, NULL, NULL, TO_ROOM );
        }
        return;
    }
    if ( ( bomb = get_obj_carry(ch,arg) ) == NULL )
    {
        send_to_char( "You do not carry that bomb.\n\r", ch );
        return;
    }
    if ( bomb->item_type != ITEM_BOMB )
    {
        send_to_char( "This isn't a bomb!\n\r", ch );
        return;
    }
                                                            //grenade or deadman
    if ( bomb->value[4] == 1 || bomb->pIndexData->vnum == 1029 )
    {
        send_to_char( "You can't set that.\n\r", ch );
        return;
    }
    if ( bomb->value[1] != 0 )
    {
        send_to_char( "The bomb is already armed!!!\n\r", ch );
        return;
    }
    bomb->value[0] = time;
    sprintf( buf, "You set %s's timer to %d seconds.", bomb->short_descr, time );
    act( buf, ch, NULL, NULL, TO_CHAR );
    sprintf( buf, "$n sets %s's timer to %d seconds.", bomb->short_descr, time );
    act( buf, ch, NULL, NULL, TO_ROOM );
    return;
}

void do_arm( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *bomb;
    if ( paintball(ch) )
    {
        send_to_char( "You cannot arm bombs in paintball.\n\r", ch );
        return;
    }
    if ( ch->in_vehicle )
    {
        send_to_char( "You can't do that in a vehicle.\n\r", ch );
        return;
    }
    if ( ( bomb = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You do not carry that bomb.\n\r", ch );
        return;
    }
    if ( bomb->item_type != ITEM_BOMB )
    {
        send_to_char( "This isn't a bomb!\n\r", ch );
        return;
    }
    if ( bomb->value[4] == 1 )
    {
        send_to_char( "You can't arm that.\n\r", ch );
    }
    if ( bomb->value[1] != 0 )
    {
        send_to_char( "The bomb is already armed!!!\n\r", ch );
        return;
    }
    if ( bomb->value[0] <= 0 )
    {
        send_to_char( "The bomb's timer isn't set yet.\n\r", ch );
        return;
    }
    act( "You begin arming $p.", ch, bomb, NULL, TO_CHAR );
    act( "$n begins arming $p.", ch, bomb, NULL, TO_ROOM );
    ch->c_time = 20 - (ch->pcdata->skill[gsn_arm]/10);
    ch->c_sn = gsn_arm;
    ch->c_obj = bomb;
    return;
}

void act_arm( CHAR_DATA *ch, int level )
{
    OBJ_DATA *bomb=ch->c_obj;

    if ( !bomb || bomb == NULL )
    {
        send_to_char( "You have lost your bomb!\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( bomb->carried_by == NULL || bomb->carried_by != ch )
    {
        send_to_char( "You have lost your bomb!\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( bomb->item_type != ITEM_BOMB )
    {
        send_to_char( "This isn't a bomb!\n\r", ch );
        return;
    }

    bomb->value[1] = 1;
    act( "You arm $p!", ch, bomb, NULL, TO_CHAR );
    act( "$n arms $p!", ch, bomb, NULL, TO_ROOM );
    free_string(bomb->owner);
    bomb->owner = str_dup(ch->name);
    SET_BIT(bomb->extra_flags,ITEM_NODROP);
    if ( ch->class != CLASS_SUICIDE_BOMBER )
    {
        obj_from_char(bomb);
        bomb->x = ch->x;
        bomb->y = ch->y;
        bomb->z = ch->z;
        obj_to_room(bomb,ch->in_room);
    }
    return;
}

void damage_building( CHAR_DATA *ch, BUILDING_DATA *bld, int dam )
{
    CHAR_DATA *vch = NULL;
    bool neutral = FALSE;
    bool dummy = FALSE;
    bool dest = FALSE;
    int portal = 0;
    char buf[MSL];
    bool hack = FALSE;

    if ( bld == NULL || ch == NULL )
        return;
    if ( bld->value[8] != 0 )
        hack = TRUE;
    if ( !str_cmp(bld->owned,"nobody") || bld->timer > 0 )
        neutral = TRUE;
    else
    {
        if ( bld->owner && !str_cmp(bld->owner->name,bld->owned) )
        {
            vch = bld->owner;
        }
        else
        {
            if ( ( vch = get_ch(bld->owned) ) == NULL )
                return;
        }
    }
    if ( bld->z == Z_NEWBIE || paintball(ch) || IN_PIT(ch) )
        return;
    if ( is_neutral(bld->type) || paintball(bld) )
        return;
    /*    	if ( check_group(ch,vch) )
            {
            send_to_char( "You cannot attack people in different kill groups.\n\r", ch );
            return;
            }*/

    if ( !neutral && complete(bld) && ch != vch )
    {
        if ( bld->type == BUILDING_DUMMY && bld->value[5] == 0 )
            dummy = TRUE;
        if ( bld->type == BUILDING_PORTAL && bld->value[0] == 0 )
            portal = bld->level;
    }
//    if ( ch != vch && (!neutral && (medal(vch) ) ) )        // || paintball(vch) ) ) )
//        return;

    if ( !neutral )
    {
        if ( practicing(ch) && ch != vch )
            do_practice(ch,"");
        if ( practicing(vch) && ch != vch )
            do_practice(vch,"");

        if ( has_ability(ch,3) )                            //Peacecraft
            dam -= dam / 10;
        if ( has_ability(vch,3) )
            dam -= dam / 10;

        if ( vch->security == FALSE )
        {
            send_to_char( "Security automatically turned on!\n\r", vch );
            vch->security = TRUE;
        }
    }
    if ( bld->protection > 0 )
        return;

    if ( IS_IMMORTAL(ch) && get_trust(ch) < 90 )
        return;

    sprintf( buf, "@@e(%s) at (%d,%d) is being attacked! @@W[@@g%s: @@W%d/%d Shield/HP vs %d Damage]\n\r@@N", bld->name, bld->x, bld->y, ch->name, bld->shield, bld->hp, dam );
    if ( !neutral )
        send_to_char( buf, vch );

    sprintf( buf, "@@rEnemy (%s) at (%d,%d) has been damaged! @@W[@@g%s: @@W%d/%d Shield/HP vs %d Damage]\n\r@@N", bld->name, bld->x, bld->y, ch->name, bld->shield, bld->hp, dam );
    if ( !IS_SET(ch->config,CONFIG_BLIND) )
        send_to_char(buf,ch);

    bld->shield -= dam;
    if ( bld->shield < 0 )
    {
        bld->hp += bld->shield;
        bld->shield = 0;
        if ( bld->hp <= 0 )
        {
            int c_ch = count_buildings(ch), c_vic = count_buildings(vch);
            sprintf( buf, "@@e%s at (%d,%d) has been destroyed!\n\r@@N", bld->name, bld->x, bld->y );
            if ( !neutral )
                send_to_char( buf, vch );
            sprintf( buf, "@@r%s at (%d,%d) has been destroyed by you!\n\r@@N", bld->name, bld->x, bld->y );
            send_to_char(buf,ch);
            if ( ch == vch && practicing(ch) )
            {
                OBJ_DATA *obj;
                extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj->next_in_room )
                    obj->in_building = NULL;
                map_bld[bld->x][bld->y][bld->z] = NULL;
                bld->x *= -1;
                bld->y *= -1;
                if ( !( ( IN_PIT(vch) && !IN_PIT(ch) ) || ( paintball(vch) && !paintball(ch) ) || ( no_lag(ch,vch) ) ) )
                    if ( vch->z != Z_SPACE && !IN_PIT(vch))
                        set_fighting(ch,vch);
                if ( IS_SET(ch->config, CONFIG_SOUND))
                    sendsound(ch,"boom",100,1,50,"combat","boom.wav");
                if ( ch->in_building == bld )
                    ch->in_building = NULL;
                return;
            }
            if ( !neutral && ch != vch && IS_SET(vch->pcdata->pflags,PLR_BASIC) && build_table[bld->type].act != BUILDING_OFFENSE )
            {
                OBJ_DATA *obj;
                extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj->next_in_room )
                    obj->in_building = NULL;
                map_bld[bld->x][bld->y][bld->z] = NULL;
                gain_exp(ch,bld->level);
                if ( bld->value[3] != 0 )
                    bld->value[3] = 0;
                bld->x *= -1;
                bld->y *= -1;
                if ( !( ( IN_PIT(vch) && !IN_PIT(ch) ) || ( paintball(vch) && !paintball(ch) ) || ( no_lag(ch,vch) ) ) )
                    if ( vch->z != Z_SPACE && !IN_PIT(vch))
                        set_fighting(ch,vch);
                if ( IS_SET(ch->config, CONFIG_SOUND))
                    sendsound(ch,"boom",100,1,50,"combat","boom.wav");
                if ( ch->in_building == bld )
                    ch->in_building = NULL;
                return;
            }
            if ( !neutral && ch != vch && complete(bld) )
            {
                int rank = get_rank(ch);
                int i;

                if ( bld->type != BUILDING_SPACE_CENTER && bld->type != BUILDING_ENGINEER_HOME )
                    create_blueprint(bld);
                if ( vch && (ch->pcdata->alliance == -1 || ch->pcdata->alliance != vch->pcdata->alliance  ) )
                {
                    if ( c_vic <= 5 )
                    {
                        send_to_char( "You do not gain points for destroying the last bits of a player's base.\n\r", ch );
                    }
                    else if ( c_ch < MAX_BUILDING / 3 )
                    {
                        char bbuf[MSL];
                        sprintf( buf, "You do not gain points if you attack someone while not having a base of your own.\n\r(You have %d of the required %d buildings)", c_ch, MAX_BUILDING/3 );
                        send_to_char(bbuf,ch);
                    }
                    else if ( !check_group(ch,vch) && !IS_IMMORTAL(vch) &&!vch->dead)
                    {
                        ch->pcdata->bkills++;
                        ch->pcdata->tbkills++;
                        vch->pcdata->blost++;
                        gain_exp(ch,bld->level);
                        if ( sysdata.qpmode > 0 )
                        {
                            ch->quest_points += bld->level * 5;
                            sprintf( buf, "@@WYou have been awarded @@y%d@@W Quest Points!@@N\n\r", bld->level * 5 );
                            send_to_char(buf,ch);
                            if ( ch->quest_points > 5000 )
                                ch->quest_points = 5000;
                        }
                        if (( !vch->first_building || vch->first_building == NULL ) && !vch->fake )
                        {
                            int msg = number_range(1,2);
                            BUILDING_DATA *bld2;
                            char ibuf[MSL];
                            if ( msg == 1 )
                                sprintf( ibuf, "@@a%s@@W turned @@a%s@@W's base into mush.", ch->name, vch->name );
                            else
                                sprintf( ibuf, "@@a%s@@W didn't leave @@a%s@@W a single building.", ch->name, vch->name );
                            info(ibuf,0);
                            for ( bld2 = ch->first_building;bld2;bld2 = bld2->next_owned )
                            {
                                if ( bld2->type == BUILDING_HUNTING_LODGE )
                                {
                                    OBJ_DATA *obj;

                                    obj = create_object(get_obj_index(OBJ_VNUM_FLAG),0);
                                    obj->x = bld2->x; obj->y = bld2->y; obj->z = bld2->z;
                                    obj_to_room(obj,ch->in_room);
                                    sprintf( ibuf, "%s's (rank %d) Flag for destroying %s's (rank %d) HQ.", ch->name, get_rank(ch), vch->name, get_rank(vch) );
                                    free_string(obj->description);
                                    obj->description = str_dup(ibuf);
                                    break;
                                }
                            }
                        }
                    }
                }
                save_char_obj(ch);
                if ( get_rank(ch) > rank )
                {
                    sprintf( buf, "@@W%s has risen in rank to %d!", ch->name, get_rank(ch) );
                    info(buf,0);
                    update_ranks(ch);
                }
                buf[0] = '\0';
                for ( i = 0;i<8;i++ )
                {
                    vch->refund[i] += build_table[bld->type].resources[i] / 1.5;
                    if ( (build_table[bld->type].resources[i] / 1.5) > 0 )
                        sprintf( buf, "@@aUpon death, you will be partially refunded for your buildings.@@N\n\r" );
                }
                if ( buf[0] != '\0' )
                    send_to_char(buf,vch);
            }
            if ( !neutral )
            {
                check_building_destroyed(bld);
                sprintf( buf, "%s's (%d%s)%s %s destroyed by %s", bld->owned, my_get_hours(vch,TRUE), (IS_NEWBIE(vch)) ? "-NEWBIE" : "", (vch && vch->security == FALSE)?"(SE)":"", bld->name, ch->name );
                log_string(buf);
                if ( IS_SET(vch->config, CONFIG_SOUND) && vch != ch )
                    sendsound(vch,"boom",100,1,50,"combat","boom.wav");
            }
            /*        		if ( !neutral && bld->type == BUILDING_HQ && vch && vch != ch )
                            {
                                    OBJ_DATA *obj;

                                    obj = create_object(get_obj_index(OBJ_VNUM_FLAG),0);
                                    move_obj(obj,bld->x,bld->y,bld->z);
                                    sprintf( buf, "%s's (rank %d) Flag for destroying %s's (rank %d) HQ.", ch->name, get_rank(ch), vch->name, get_rank(vch) );
                                    free_string(obj->description);
                                    obj->description = str_dup(buf);
                            }*/
            extract_building( bld, TRUE );
            dest = TRUE;
            if ( IS_SET(ch->config, CONFIG_SOUND))
                sendsound(ch,"boom",100,1,50,"combat","boom.wav");

        }
    }
    if ( !neutral )
    {
        if ( same_planet(ch,vch) && vch->z != Z_SPACE && !hack && vch->z != Z_PAINTBALL )
        {
            if ( dam > 0 )
            {
                set_fighting(vch,ch);
            }
            else
            {
                vch->fighttimer = 480;
            }
        }

        if ( dummy && dest )
        {
            send_to_char( "You hit a dummy building! It explodes in a giant fireball!\n\r", ch );
            damage(vch,ch,250,DAMAGE_PSYCHIC);
        }
    }
    if ( portal > 0 && dest )
    {
        int x,y;
        x = number_range(ch->x-(portal * 2),ch->x+(portal*2));
        y = number_range(ch->y-(portal * 2),ch->y+(portal*2));
        if ( x <= BORDER_SIZE || x >= MAX_MAPS-BORDER_SIZE )
            x = ch->x;
        if ( y <= BORDER_SIZE || y >= MAX_MAPS-BORDER_SIZE )
            y = ch->y;
        move(ch,x,y,ch->z);
        do_look(ch,"");
        if ( neutral )
            vch = ch;
        damage(vch,ch,100,-1);
        send_to_char( "The portal ZAAAPS you!\n\r", ch );
    }
    return;
}

void do_blast( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *ammo = NULL;
    BUILDING_DATA *bld = ch->in_building;
    BUILDING_DATA *bld2;
    int x,y;
    char arg[MSL];
    char buf[MSL];
    bool cannon = FALSE;
    CHAR_DATA *wch;

    argument = one_argument(argument,arg);
    if ( bld == NULL )
    {
        send_to_char( "You're not even in a building!\n\r", ch );
        return;
    }
    if ( bld->value[3] < 0 )
    {
        send_to_char( "It's corrupted by a virus!\n\r", ch );
        return;
    }
    if ( bld->type == BUILDING_SONIC_BLASTER )
    {
        do_sblast(ch,"");
        return;
    }
    if ( bld->type == BUILDING_WAR_CANNON || GUNNER(bld) )
        cannon = TRUE;
    else if ( bld->type == BUILDING_SCUD_LAUNCHER )
        cannon = FALSE;
    else if ( bld->type == BUILDING_NUKE_LAUNCHER )
        cannon = FALSE;
    else if ( bld->type == BUILDING_ATOM_BOMBER )
        cannon = FALSE;
    else
    {
        send_to_char( "You can use this command only in: War cannon, gunner, scud launcher, nuke launcher and atom bomber.\n\r", ch );
        return;
    }
    if ( !complete(bld) )
    {
        send_to_char( "You haven't finished the construction.\n\r", ch );
        return;
    }
    if ( str_cmp(ch->name,bld->owned) )
    {
        send_to_char( "This isn't your building.\n\r", ch);
        return;
    }
    if ( (!cannon || GUNNER(bld)) && bld->value[0] > 0 )
    {
        sprintf( buf, "You have to wait another %d minutes.\n\r", bld->value[0] / 6 );
        send_to_char( buf, ch );
        return;
    }
    if ( !is_number(arg) || !is_number(argument) )
    {
        send_to_char( "Those are invalid coordinates.\n\r", ch );
        return;
    }
    x = atoi(arg);
    y = atoi(argument);
    if ( x < BORDER_SIZE || x > MAX_MAPS - BORDER_SIZE || y < BORDER_SIZE || y > MAX_MAPS - BORDER_SIZE )
    {
        send_to_char( "Those are invalid coordinates.\n\r", ch );
        return;
    }
    if ( x == ch->x && y == ch->y )
    {
        send_to_char( "That wouldn't be very smart, now, would it?\n\r", ch );
        return;
    }
    if ( cannon )
    {
        if ( x - 10 > ch->x || x + 10 < ch->x || y - 10 > ch->y || y + 10 < ch->y )
        {
            send_to_char( "That's too far. You can only shoot in a 10X10 radius.\n\r", ch );
            return;
        }

        ammo = create_object(get_obj_index(OBJ_VNUM_CANNONBALL),0);
        sprintf( buf, "You aim the turret, and fire towards %d, %d!", x,y);
        act( buf, ch, NULL, NULL, TO_CHAR );
        act( "$n aims the turret, and fires!", ch, NULL, NULL, TO_ROOM );
        ammo->value[2] = bld->level * 10;
        if ( GUNNER(bld) )
        {
            ammo->value[3] = 3;
            if ( !IS_SET(bld->value[5],GUNNER_ROCKETS) )
                ammo->value[2] /= 2;
        }
    }
    else
    {
        if ( bld->type == BUILDING_SCUD_LAUNCHER )
        {
            int bcount = 0;
            int pcount = 0;
            char buf[MSL];

            ammo = create_object(get_obj_index(OBJ_VNUM_SCUD),0);
            ammo->value[2] += bld->level * 10;
            sprintf( buf, "You insert the coordinates, and launch a SCUD towards %d, %d!", x,y);
            act( buf, ch, NULL, NULL, TO_CHAR );
            act( "$n inserts some coordinates, and launches a SCUD!", ch, NULL, NULL, TO_ROOM );
            x = number_fuzzy(number_fuzzy(x));              // SCUDs miss
            y = number_fuzzy(number_fuzzy(y));
            if ( get_building(x,y,bld->z) )
                bcount += 1;
            if ( get_building(x,y+1,bld->z) )
                bcount += 1;
            if ( get_building(x,y-1,bld->z) )
                bcount += 1;
            if ( get_building(x+1,y,bld->z) )
                bcount += 1;
            if ( get_building(x-1,y,bld->z) )
                bcount += 1;
            for ( wch = map_ch[x][y][bld->z];wch;wch = wch->next_in_room )
                pcount++;
            sprintf( buf, "@@yReports assume the SCUD to hit %d building(s) and %d player(s), landing at %d/%d@@N\n\r", bcount, pcount, x, y );
            send_to_char( buf, ch );
        }
        else if ( bld->type == BUILDING_NUKE_LAUNCHER )
        {
            sprintf( buf, "You insert the coordinates, and launch a NUKE towards %d, %d!", x,y);
            act( buf, ch, NULL, NULL, TO_CHAR );
            act( "$n inserts some coordinates, and launches a NUKE!", ch, NULL, NULL, TO_ROOM );
            {
                OBJ_DATA *obj;
                extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
                int xx,yy,range;

                if ( IS_SET(bld->value[1],INST_DEPLEATED_URANIUM) )
                    range = 7;
                else
                    range = 5;

                for ( xx = x - range;xx < x + range;xx++ )
                    for ( yy = x - range;yy < x + range;yy++ )
                {
                    if ( INVALID_COORDS(xx,yy) )
                        continue;
                    if ( ( bld2 = map_bld[xx][yy][ch->z] ) != NULL && bld2->active )
                    {
                        bld2->value[9] += bld->level * 100;
                        send_to_loc( "@@yThe building fills with nuclear fallout!!@@N\n\r", bld->x, bld->y, bld->z );
                    }
                    for ( obj = map_obj[xx][yy];obj;obj = obj->next_in_room )
                    {
                        if ( obj->z != ch->z )
                            continue;
                        if ( !IS_SET(obj->extra_flags,ITEM_NUCLEAR) )
                            SET_BIT(obj->extra_flags,ITEM_NUCLEAR);
                        obj->attacker = ch;
                    }
                }
            }
            bld->value[0] = 360;
            WAIT_STATE(ch,15);
            return;
        }
        if ( bld->type == BUILDING_ATOM_BOMBER )
        {
            ammo = create_object(get_obj_index(OBJ_VNUM_ATOM_BOMB),0);
            ammo->value[2] += bld->level * 10;
            sprintf( buf, "You insert the coordinates, and launch an ATOM BOMB towards %d, %d!", x,y);
            act( buf, ch, NULL, NULL, TO_CHAR );
            act( "$n inserts some coordinates, and launches an ATOM BOMB!", ch, NULL, NULL, TO_ROOM );
            {
                OBJ_DATA *obj;
                extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
                int xx,yy,range;

                if ( IS_SET(bld->value[1],INST_DEPLEATED_URANIUM) )
                    range = 7;
                else
                    range = 5;

                for ( xx = x - range;xx < x + range;xx++ )
                    for ( yy = x - range;yy < x + range;yy++ )
                {
                    if ( INVALID_COORDS(xx,yy) )
                        continue;
                    if ( ( bld2 = map_bld[xx][yy][ch->z] ) != NULL && bld2->active )
                    {
                        bld2->value[9] += bld->level * 100;
                        send_to_loc( "@@yThe building fills with nuclear fallout!!@@N\n\r", bld->x, bld->y, bld->z );
                    }
                    for ( obj = map_obj[xx][yy];obj;obj = obj->next_in_room )
                    {
                        if ( obj->z != ch->z )
                            continue;
                        if ( !IS_SET(obj->extra_flags,ITEM_NUCLEAR) )
                            SET_BIT(obj->extra_flags,ITEM_NUCLEAR);
                        obj->attacker = ch;
                    }
                }
            }
            ammo->x = x;
            ammo->y = y;
            obj_to_room(ammo, get_room_index(ROOM_VNUM_WMAP) );
            if ( number_percent() < 33 )
            {
                bool ex;
                ex = check_missile_defense(ammo);

                if ( ex )
                {
                    extract_obj(ammo);
                    WAIT_STATE(ch,25);
                    bld->value[0] = 360;
                    return;
                }
            }
            ammo->weight = bld->level * 20;
            free_string(ammo->owner);
            ammo->owner = str_dup(ch->name);
            WAIT_STATE(ch,25);
            explode(ammo);
            bld->value[0] = 360;
            return;
        }
        bld->value[0] = 360;
    }
    ammo->x = x;
    ammo->y = y;
    ammo->z = ch->z;
    ammo->value[0] *= planet_table[ammo->z].gravity;
    obj_to_room(ammo, get_room_index(ROOM_VNUM_LIMBO) );
    ammo->weight = bld->level * 20;
    free_string(ammo->owner);
    ammo->owner = str_dup(ch->name);
    for ( wch = map_ch[x][y][bld->z];wch;wch = wch->next_in_room )
    {
        send_to_char( "@@eYou detect a missile being fired towards your location!@@N\n\r", wch );
        if ( wch->fighttimer < 60 )
            wch->fighttimer = 60;
    }
    if ( ( bld2 = get_building(x,y,bld->z) ) != NULL )
    {
        if (IS_SET(bld2->value[1],INST_REFLECTOR) && number_percent() < (55-(bld->level*5)+(bld2->level*5)) )
        {
            move_obj(ammo,bld->x,bld->y,bld->z);
            send_to_char( "@@WA @@ppink @@Wlight shimmers around the target for a moment, then fades!@@N\n\r", ch );
        }
    }
    if ( check_missile_defense(ammo) )
        extract_obj(ammo);
    WAIT_STATE(ch,45);
    return;
}

void damage_vehicle( CHAR_DATA *ch, VEHICLE_DATA *vhc, int dam, int dt )
{
    char buf[MSL];
    if ( vhc->in_vehicle )
    {
        damage_vehicle(ch,vhc->in_vehicle,dam,dt );
        return;
    }
    if ( vhc->z == Z_NEWBIE || ch->z == Z_NEWBIE )
        return;
    if ( SPACE_VESSAL(vhc) )
    {
        int dec = 0, arm = vhc->power[POWER_ARMOR];

        if ( arm > 100 )
            dec = ((dam / 2) * (arm-100)) / 100;
        else if ( arm < 100 )
            dec = (dam * (arm-100)) / 100;

        dam -= dec;
        if ( vhc->state == VEHICLE_STATE_DEFENSE )
        {
            dam /= 1.2;
            if ( number_percent() < 20 || ( IS_SET(vhc->flags,VEHICLE_GUARD_LASERS) && number_percent() < 30) )
            {
                CHAR_DATA *vch = vhc->driving;
                if ( vch )
                {
                    send_to_char( "Your defenses deflect the blow!\n\r", vch );
                    act( "$N's defenses deflect the blow.", ch, NULL, vch, TO_CHAR );
                    return;
                }
            }
        }
        else if ( vhc->state == VEHICLE_STATE_EVADE )
        {
            if ( number_percent() < 33 || ( IS_SET(vhc->flags,VEHICLE_GUARD_LASERS) && number_percent() < 30) )
            {
                CHAR_DATA *vch = vhc->driving;
                if ( vch )
                {
                    send_to_char( "Your defenses deflect the blow!\n\r", vch );
                    act( "$N's defenses deflect the blow.", ch, NULL, vch, TO_CHAR );
                    return;
                }
            }
        }
        else if ( vhc->state == VEHICLE_STATE_OFFENSE )
            dam *= 1.2;
        else if ( vhc->state == VEHICLE_STATE_CHARGE )
            dam *= 1.5;

    }
    if ( AIR_VEHICLE(vhc->type) && vhc->z == Z_AIR )
    {
        int evade = 30;
        if ( vhc->type == VEHICLE_BOMBER )
            evade += 20;
        if ( dt == DAMAGE_LASER )
        {
            dam *= 2;
        }
        else if ( number_percent() < evade )
        {
            if ( ( ch = vhc->driving) == NULL )
                return;
            act( "You evade the enemy fire!", ch, NULL, NULL, TO_CHAR );
            act( "$n evades the enemy fire!", ch, NULL, NULL, TO_ROOM );
            return;
        }
        else if ( vhc->z == Z_AIR && dt == DAMAGE_BLAST && number_percent() < 50 && vhc->driving )
        {
            send_to_char( "The explosion damaged all your flight systems!\n\r", vhc->driving );
            dam *= 10000;
        }
    }

    if ( dt == DAMAGE_ACID )
        dam *= 2;
    else if ( dt == DAMAGE_BLAST )
        dam *= 1.5;
    else if ( dt == DAMAGE_LASER )
        dam *= 1.5;

    if ( vhc->type == VEHICLE_TANK && dt == DAMAGE_BULLETS )
        dam /= 2;
    if ( vhc->type == VEHICLE_BBQ && dt == DAMAGE_FLAME )
        dam *= 2;
    if ( vhc->type == VEHICLE_BBQ && dt == DAMAGE_LASER )
        dam *= 1.5;
    if ( vhc->type == VEHICLE_LASER && dt == DAMAGE_FLAME )
        dam *= 2;

    sprintf( buf, "@@eYou feel a shock as your vehicle is hit! @@R(@@W%d@@R)@@N\n\r", dam );
    if ( vhc->driving != NULL )
        send_to_char( buf, vhc->driving );

    if ( vhc->type == VEHICLE_MECH && dt == DAMAGE_BULLETS && number_percent() < dam / 40 )
    {
        dam *= 100;
        send_to_char( "The bullet pierces through the Mech's panels, and hits the uranium core!!\n\r", ch );
        if ( vhc->driving )
            send_to_char( "The bullet pierces through the Mech's panels, and hits the uranium core!!\n\r", vhc->driving );
    }

    vhc->hit -= dam;
    if ( vhc->driving )
        set_fighting(vhc->driving,ch);
    if ( vhc->hit <= 0 )
    {
        if ( AIR_VEHICLE(vhc->type) && vhc->driving && vhc->z == Z_AIR )
        {
            crash(vhc->driving,ch);
            return;
        }
        act( "$T explodes!!", ch, NULL, vhc->desc, TO_ROOM );
        act( "$T explodes!!", ch, NULL, vhc->desc, TO_CHAR );
        if ( vhc->driving != NULL )
        {
            CHAR_DATA *vch = vhc->driving;
            vhc->driving = NULL;
            vch->in_vehicle = NULL;
            damage(ch,vch,vch->hit+1,DAMAGE_BLAST);
        }
        extract_vehicle(vhc,FALSE);
    }
    return;
}

void check_armor( OBJ_DATA *obj )
{
    CHAR_DATA *ch = obj->carried_by;
    if ( obj->item_type != ITEM_ARMOR || obj->value[1] < 100 || ch == NULL )
        return;
    if ( number_range(100,1000) < obj->value[1] )
    {
        act( "$p has taken too much damage, and is destroyed.", ch, obj, NULL, TO_CHAR );
        act( "$n's $p has taken too much damage, and is destroyed.", ch, obj, NULL, TO_ROOM );
        extract_obj(obj);
    }
    return;
}

void set_fighting( CHAR_DATA *ch, CHAR_DATA *victim )
{
    char sound[MSL];
    int type;

    if ( (victim->z == Z_SPACE && ch->z != Z_SPACE) || (ch->z == Z_SPACE && victim->z != Z_SPACE) )
        return;
    if ( ch->fighttimer == 0 )
    {
        type = number_range(1,3);
        sprintf( sound, "FightMusic%d.mid", type );
        if ( IS_SET(ch->config, CONFIG_SOUND) )
            sendsound( ch, sound, 40,0,1,"fightmusic",sound);
        if ( ch != victim && victim->fighttimer == 0 && ( IS_SET(victim->config, CONFIG_SOUND) ))
            sendsound( victim, sound, 40,0,1,"fightmusic",sound);
    }
    if (!paintball(ch))
        ch->fighttimer = 120 * 8;
    if (!paintball(victim))
        victim->fighttimer = 120 * 8;
    if ( victim->position == POS_HACKING || victim->bvictim )
    {
        if ( victim->bvictim )
            victim->bvictim->value[8] = 0;
        victim->bvictim = NULL;
        victim->c_sn = -1;
    }
    if ( ch->position != POS_SPACE_COM )
    {
        //		ch->position = POS_STANDING;
        victim->position = POS_STANDING;
    }
    return;
}

bool check_dead( CHAR_DATA *ch, CHAR_DATA *victim )
{
    bool masskill = FALSE;
    if ( !victim )
        return TRUE;
    if ( victim->dead )
        return TRUE;
    if ( victim->position == POS_DEAD || victim->hit <= 0 )
    {
        if ( victim->in_vehicle )
        {
            victim->in_vehicle->driving = NULL;
            victim->in_vehicle = NULL;
        }
        if ( medal(victim) )
            do_medal(victim,"");
        if ( practicing(victim) || IS_SET(victim->pcdata->pflags,PFLAG_HELPING))
        {
            victim->hit = victim->max_hit;
            victim->position = POS_STANDING;
            victim->hit = 1;
            do_practice(victim,"");
            do_home(victim,victim->name);
            return TRUE;
        }
        if ( ch != victim )
        {
            char buf[MSL];
            int rank = get_rank(ch);
            int msg = number_range(1,19);
            bool alli = FALSE;
            bool killer = FALSE;

            if ( ch->pcdata->alliance != -1 && ch->pcdata->alliance == victim->pcdata->alliance )
                alli = TRUE;
            if ( ch->killtimer > 0 && !IN_PIT(victim) )
                killer = TRUE;
            if ( !killer )
                ch->killtimer = 1440;
            if ( !killer && !alli )
            {
                if ( !check_group(ch,victim) )
                {
                    int vrank = get_rank(victim);
                    ch->pcdata->pkills++;
                    ch->pcdata->tpkills++;
                    if ( ch->pcdata->alliance > -1 && victim->pcdata->alliance > -1 )
                        alliance_table[ch->pcdata->alliance].kills++;
                    gain_exp(ch,vrank);
                    if ( sysdata.killfest || sysdata.qpmode > 0 )
                    {
                        ch->quest_points += vrank * 10;
                        sprintf( buf, "@@WYou have been awarded @@y%d@@W Quest Points!@@N\n\r", vrank * 10 );
                        send_to_char(buf,ch);
                        if ( ch->quest_points > 5000 )
                            ch->quest_points = 5000;
                    }
                }
            }
            else
            {
                masskill = TRUE;
            }
            victim->suicide = FALSE;
            victim->dead = TRUE;

            if( msg == 1 )
                sprintf(buf, "@@a%s @@eexterminates @@a%s @@Wafter a fierce battle.@@N", ch->name, victim->name);
            else if( msg == 2 )
                sprintf(buf, "@@a%s @@Wmade @@a%s @@yholy@@W. Full of bullet holes, that is.@@N", ch->name, victim->name);
            else if( msg == 3 )
                sprintf(buf, "@@WIt's a bird! It's a plane! It's... @@a%s@@W's arm?? @@a%s@@W just sits there, grinning.@@N", victim->name, ch->name );
            else if( msg == 4 )
                sprintf(buf, "@@WIf @@a%s@@W wins, @@a%s@@W dies. If @@a%s @@Wwins, @@a%s@@W lives. Guess who died?@@N", ch->name, victim->name, victim->name, ch->name );
            else if( msg == 5 )
                sprintf(buf, "@@a%s @@Wkicked the bucket... then it hit @@a%s@@W and killed %s@@W!!@@N", ch->name, victim->name, victim->login_sex == SEX_MALE ? "him" : "her" );
            else if( msg == 6 )
                sprintf(buf, "@@WAfter what @@a%s@@W just did to @@a%s@@W, %s ain't gonna be resting in any less than 10 \"peace\"s.@@N", ch->name, victim->name, victim->login_sex == SEX_MALE ? "he" : "she" );
            else if( msg == 7 )
                sprintf(buf, "@@W\"I'll see you in hell @@a%s@@W!!\", @@a%s@@W says. \"You first.\"... BOOM!.@@N", ch->name,victim->name);
            else if( msg == 8 )
                sprintf(buf, "@@a%s @@Whas met %s maker... And they blew @@a%s@@W up together!@@N", ch->name,ch->login_sex == SEX_MALE ? "his" : "her",victim->name);
            else if( msg == 9 )
                sprintf(buf, "@@a%s @@Wforces @@a%s@@W to look at %s own reflection!!...Naturally, %s died.@@N", ch->name,victim->name,victim->login_sex == SEX_MALE ? "his" : "her",victim->login_sex ==SEX_MALE ? "he" : "she" );
            else if( msg == 10 )
                sprintf(buf, "@@a%s@@W made sure that @@a%s @@Wgets %s picture in the dictionary... next to \"@@eOWNED@@W\"!@@N", ch->name,victim->name,victim->login_sex == SEX_MALE ? "his" : "her" );
            else if( msg == 11 )
                sprintf(buf, "@@a%s@@W replaced @@a%s@@W's car tires with @@eFirestone@@W brand. %s crashed minutes later.@@N", ch->name,victim->name,victim->login_sex == SEX_MALE ? "He" : "She" );
            else if( msg == 12 )
                sprintf(buf, "@@a%s@@W convinced president Bush to invade @@a%s@@W. After a few months of war, %s was caught and killed.@@N", ch->name,victim->name,victim->login_sex == SEX_MALE ? "he" : "she" );
            else if( msg == 13 )
                sprintf(buf, "@@WEmail us at: @@a%s@@W@KilledBy@@a%s@@W.com@@N", victim->name, ch->name );
            else if ( msg == 14 )
                sprintf(buf, "@@a%s @@Wturned @@a%s@@W into a @@eVAMPIRE@@W!!... Then threw %s on a rocket to the sun!@@N", ch->name,victim->name,(victim->sex==SEX_MALE)?"him":"her" );
            else if ( msg == 15 )
                sprintf(buf, "@@a%s @@Wsacrifices @@a%s@@W to Amnon's cat.@@N", ch->name,victim->name );
            else if ( msg == 16 )
                sprintf(buf, "@@a%s @@Wbits the dust... then bites @@a%s@@W and kills %s!@@N", ch->name,victim->name, (victim->sex==SEX_MALE)?"him":"her" );
            else if ( msg == 17 )
                sprintf(buf, "@@a%s @@Wturned @@a%s@@W's gut into a new scarf.@@N", ch->name,victim->name );
            else if ( msg == 18 )
                sprintf(buf, "@@a%s @@Wbored @@a%s@@W to death. A kill nevertheless.@@N", ch->name,victim->name );
            else
                sprintf(buf, "@@a%s @@Wlost an arm in battle!.. And a leg... And a head... Thank @@a%s@@W.@@N", victim->name,ch->name);
            info(buf, 1);
            if ( get_rank(ch) > rank )
            {
                sprintf( buf, "@@W%s has risen in rank to %d!", (ch->sex==1)?"He":"She", get_rank(ch) );
                update_ranks(ch);
                info(buf,0);
            }

            sprintf( log_buf, "%s (%d%s%s) killed by %s at %d/%d/%d",
                victim->name,
                my_get_hours(victim,TRUE),
                masskill ? "-MASSKILL" : "",
                IS_NEWBIE(victim) ? "-NEWBIE" : "",
                ch->name,
                victim->x, victim->y, victim->z );
            log_string( log_buf );
            monitor_chan( victim, log_buf, MONITOR_COMBAT );
            if ( IN_PIT(victim) && IN_PIT(ch) )
            {
                free_string(web_data.last_killed_in_pit);
                web_data.last_killed_in_pit = str_dup(victim->name);
                update_web_data(WEB_DATA_KILLS_PIT,ch->name);
            }
        }
        else
        {
            int msg = number_range(1,4);
            char buf[MSL];
            if ( msg == 1 )
                sprintf( buf, "@@a%s@@W got %sself killed.@@N", ch->name, (ch->sex == 1) ? "him" : "her" );
            else if ( msg == 2 )
                sprintf( buf, "@@a%s@@W pressed the little red button. NEVER press the little red button!.@@N", ch->name );
            else if ( msg == 3 )
                sprintf( buf, "@@WMemo to @@a%s@@W: You do not take 7 turns in a row in Russian Roulette.@@N", ch->name );
            else
                sprintf( buf, "@@a%s@@W stepped into one of them new shiny suicide boothes!", ch->name );
            info( buf, 0 );
            ch->suicide = TRUE;
            sprintf( log_buf, "%s suicided.", ch->name );
            monitor_chan( victim, log_buf, MONITOR_COMBAT );
        }

        {
            char name_buf[MAX_STRING_LENGTH];
            victim->is_free = FALSE;
            sprintf( name_buf, "%s", ch->name );
            raw_kill( victim, name_buf );
            if ( IN_PIT(victim) )
                do_home(victim,victim->name);
            return TRUE;
        }
        return TRUE;
    }
    return FALSE;
}

void do_boom( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;
    BUILDING_DATA *bld;
    BUILDING_DATA *bld2;
    int range, dam,x,y,xx,yy,x_n,y2,z=ch->z;
    bool boomer = FALSE;

    if ( ( bld = ch->in_building ) == NULL || !complete(bld) || str_cmp(bld->owned,ch->name) || bld->type != BUILDING_BOOM )
    {
        send_to_char( "You must be in one of your completed BOOM buildings.\n\r", ch );
        return;
    }
    range = 2 + bld->level;
    if ( boomer )
        range *= 2;
    send_to_char( "You press a few buttons, and the building EXPLODES in a giant fireball!\n\r", ch );
    x = bld->x - range;
    xx = bld->x + range;
    y2 = bld->y - range;
    yy = bld->y + range;
    if ( x < 3 )
        x = 3;
    if ( y2 < 3 )
        y2 = 3;

    for ( x=x;x<xx;x = x_n )
    {
        x_n = x + 1;
        for ( y=y2;y<yy;y++ )
        {
            if ( INVALID_COORDS(x,y) || ( x == bld->x && y == bld->y ) )
                continue;
            if ( ( bld2 = get_building(x,y,bld->z) ) != NULL )
            {
                if ( ( wch = get_ch(bld2->owned) ) == NULL )
                    continue;
                if ( wch == ch && !practicing(ch) )
                    continue;
                if ( wch != ch && wch->pcdata->alliance != -1 && wch->pcdata->alliance == ch->pcdata->alliance )
                    continue;
                dam = number_range(20*bld->level,35*bld->level);
                if ( boomer )
                    dam *= 2;
                damage_building(ch,bld2,dam);
            }
            for ( wch = map_ch[x][y][z];wch;wch = wch_next )
            {
                wch_next = wch->next_in_room;
                if ( wch == ch )
                    continue;
                if ( wch->pcdata->alliance != -1 && wch->pcdata->alliance == ch->pcdata->alliance )
                    continue;
                dam = number_range(25*bld->level,50*bld->level);
                send_to_char( "You hear a large BOOM, followed by a fireball!\n\r", wch );
                if ( boomer )
                    dam *= 2;
                damage(ch,wch,dam,DAMAGE_BLAST);
            }
        }
    }
    extract_building(bld,FALSE);
    ch->in_building = NULL;
    WAIT_STATE(ch,24);
    return;
}

void do_battlestations( CHAR_DATA *ch, char *argument )
{
    if ( !ch->in_vehicle || !SPACE_VESSAL(ch->in_vehicle) )
    {
        send_to_char( "You must be in a space ship.\n\r", ch );
        return;
    }
    ch->position = POS_SPACE_COM;
    act( "You call out, \"Battle Stations!\", and the alarms start sounding!", ch, NULL, NULL, TO_CHAR );
    return;
}

void set_stun(CHAR_DATA *ch, int time)
{
    ch->wait += time;
    /*	if ( ch->c_sn != -1 )
            do_stop(ch,"");
        ch->c_sn = gsn_stun;
        ch->c_time += time;*/
    return;
}

bool check_group(CHAR_DATA *ch, CHAR_DATA *vch)
{
    if ( !sysdata.kill_groups )
        return FALSE;
    if ( IN_PIT(ch) )
        return FALSE;
    if ( !ch || !vch )
        return FALSE;
    if ( ch->kill_group != vch->kill_group )
        return TRUE;
    return FALSE;
}

void air_bomb(CHAR_DATA *ch)
{
    VEHICLE_DATA *vhc=ch->in_vehicle;
    OBJ_DATA *bomb;
    int type;

    if ( vhc == NULL || !vhc || !ch || ch == NULL )
        return;
    if ( vhc->type == VEHICLE_BIO_FLOATER )
        type = 2;
    else
        type = 1;

    if ( type == 1 )
    {
        act( "You drop a bomb.", ch, NULL, NULL, TO_CHAR );
        act( "$n drops a bomb.", ch, NULL, NULL, TO_ROOM );
        bomb = create_object(get_obj_index(OBJ_VNUM_AIR2GROUNDBOMB),0);
    }
    else
    {
        act( "You spray the buildings below.", ch, NULL, NULL, TO_CHAR );
        act( "$n sprays the buildings below.", ch, NULL, NULL, TO_ROOM );
        bomb = create_object(get_obj_index(32606),0);
    }
    bomb->x = vhc->x;
    bomb->y = vhc->y;
    bomb->z = Z_GROUND;
    obj_to_room(bomb, get_room_index(ROOM_VNUM_LIMBO) );
    free_string(bomb->owner);
    bomb->owner = str_dup(ch->name);
    vhc->ammo--;
    WAIT_STATE(ch,8);
    if ( type == 2 )
        explode(bomb);
    return;
}

bool no_lag(CHAR_DATA *ch, CHAR_DATA *vch )
{
    bool lag = TRUE;
    if ( ch->z != vch->z )
        lag = FALSE;
    if ( ch->z == Z_UNDERGROUND || ch->z == Z_GROUND )
        if ( vch->z == Z_UNDERGROUND || vch->z == Z_GROUND )
            lag = TRUE;
    return lag;
}

bool same_planet(CHAR_DATA *ch, CHAR_DATA *vch )
{
    if ( ch->z == vch->z )
        return TRUE;
    if ( (ch->z == Z_GROUND || ch->z == Z_UNDERGROUND || ch->z == Z_AIR) && (vch->z == Z_GROUND || vch->z == Z_UNDERGROUND || vch->z == Z_AIR ) )
        return TRUE;
    return FALSE;
}

void gain_exp(CHAR_DATA *ch, int value)
{
    int rank = get_rank(ch);
    char buf[MSL];

    if ( value > 150 )
        value = 150;
    if ( rank > 50 )
        value /= 1.2;
    if ( rank > 100 )
        value /= 1.2;
    if ( rank > 200 )
        value /= 1.2;
    if ( rank > 350 )
        value /= 1.2;
    if ( rank > 500 )
        value /= 1.2;
    if ( rank > 700 )
        value /= 1.2;
    if ( value + ch->pcdata->experience > 5000 )
        value = 5000 - ch->pcdata->experience;
    if ( value <= 0 )
    {
        value = 1;
        //		return;
    }
    if ( sysdata.xpmode > 0 )
        value *= 2;
    ch->pcdata->experience += value;
    sprintf( buf, "@@WYou have received @@y%d@@W experience point(s).\n\r", value );
    send_to_char(buf,ch);
    return;
}

void do_gunner_shoot( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MSL];
    int dam, chance,range=6;
    BUILDING_DATA *bld = ch->in_building;

    if ( !ch->in_building )
        return;

    if ( bld->value[0] > 0 )
    {
        sprintf( buf, "You have to wait another %d minutes.\n\r", bld->value[0] / 6 );
        send_to_char( buf, ch );
        return;
    }
    if ( ( victim = ch->victim ) == NULL || victim == ch )
    {
        send_to_char( "You must @@eTARGET@@N your enemy first!\n\r", ch );
        return;
    }
    if ( abs(ch->x-victim->x)>range || abs(ch->y-victim->y) > range)
    {
        send_to_char( "You have lost your target!\n\r", ch );
        ch->victim = ch;
        return;
    }

    act( "You fire the gunner at $N!", ch, NULL, victim, TO_CHAR );
    act( "$n begins firing the Gunner!", ch, NULL, NULL, TO_ROOM );
    send_to_char( "You are attacked by a barrage of bullets!\n\r", victim );
    chance = bld->level * 15;
    if ( number_percent() > chance )
    {
        send_to_char( "You manage to dodge the bullets.\n\r", victim );
        set_stun(ch,12);
        return;
    }
    dam = 50-(bld->level * 3);
    sprintf( buf, "@@eYou are hit! @@c(@@a%d@@c)@@N\n\r", dam );
    send_to_char( buf,victim );
    sprintf( buf, "@@eYou hit %s! @@c(@@a%d@@c)@@N\n\r", victim->name, dam );
    send_to_char( buf,ch );
    if ( bld->value[5] > 0 )                                //Check for gunner installations
    {
        if ( IS_SET(bld->value[5],GUNNER_NUCLEAR) && number_percent()<20)
        {
            OBJ_DATA *obj;
            for ( obj = victim->first_carry;obj;obj = obj->next_in_carry_list )
            {
                if ( IS_SET(obj->extra_flags,ITEM_NUCLEAR) || obj->wear_loc == WEAR_NONE )
                    continue;
                SET_BIT(obj->extra_flags,ITEM_NUCLEAR);
            }
        }
        if ( IS_SET(bld->value[5],GUNNER_POISON) && number_percent()<20)
        {
            if ( victim->disease == 0 )
            {
                victim->disease = 20;
                send_to_char( "You become very sick!\n\r", victim );
                act( "$n's becomes sick!", victim, NULL, NULL, TO_ROOM );
            }
        }
        if ( IS_SET(bld->value[5],GUNNER_PSYCHIC) && number_percent()<20)
        {
            int dir = number_range(0,3);
            move_char(victim,dir);
        }
        if ( IS_SET(bld->value[5],GUNNER_CHAOS) && number_percent()<20)
        {
            int eff,x = number_range(1,4);
            if ( x == 1 )
                eff = EFFECT_BLIND;
            else if ( x == 2 )
                eff = EFFECT_SLOW;
            else
                eff = EFFECT_CONFUSE;
            if ( !IS_SET(victim->effect,eff) )
            {
                SET_BIT(victim->effect,eff);
                send_to_char( "You feel funny...\n\r", victim );
            }
        }
        if ( IS_SET(bld->value[5],GUNNER_TRACER) && number_percent()<20)
        {
            if ( !IS_SET(victim->effect,EFFECT_TRACER) )
                SET_BIT(victim->effect,EFFECT_TRACER);
        }
    }
    damage(ch,victim,dam,DAMAGE_BULLETS);
    set_stun(ch,12);
    return;
}
