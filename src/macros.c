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
#include <time.h>
#include "ack.h"
#include "tables.h"
#include <math.h>

sh_int get_psuedo_level( CHAR_DATA *ch )
{
    return 80;
}

bool    ok_to_use( CHAR_DATA *ch, int value )
{
    if ( value == NO_USE
        &&   get_trust(ch) < 85 )
    {
        send_to_char( "Only Creators may use this value.\n\r", ch );
        return FALSE;
    }
    return TRUE;
}

bool    check_level_use( CHAR_DATA *ch, int level )
{
    char buf[MAX_STRING_LENGTH];
    char out[MAX_STRING_LENGTH];

    if ( get_trust(ch) >= level )
        return TRUE;

    sprintf( out, "This option limited to " );

    switch ( level )
    {
        case 85:
            safe_strcat( MSL, out, "Creators only.\n\r" );
            break;
        case 84:
            safe_strcat( MSL, out, "Supremes or higher.\n\r" );
            break;
        case 83:
            safe_strcat( MSL, out, "Dieties or higher.\n\r" );
            break;
        case 82:
            safe_strcat( MSL, out, "Immortals or higher.\n\r" );
            break;
        case 81:
            safe_strcat( MSL, out, "Heroes or higher.\n\r" );
            break;
        default:
            sprintf( buf, "level %d players and higher.\n\r", level );
            safe_strcat( MSL, out, buf );
    }
    send_to_char( out, ch );
    return FALSE;
}

void create_blueprint( BUILDING_DATA *bld )
{
    OBJ_DATA *obj;
    char buf[MSL];

    if ( CIVILIAN(bld) )
        return;
    obj = create_object( get_obj_index( OBJ_VNUM_BLUEPRINTS ), 0 );
    obj->level = URANGE(2, bld->level + 1, 5 );
    obj->value[0] = bld->type;
    sprintf( buf, "Blueprints for %d%s level %s", obj->level, ( obj->level == 2 ) ? "nd" : ( obj->level == 3 ) ? "rd" : "th", capitalize(bld->name) );
    free_string( obj->short_descr );
    free_string( obj->description );
    free_string( obj->name );
    obj->short_descr = str_dup(buf);
    obj->description = str_dup(buf);
    sprintf( buf, "Blueprint %s %d%s", bld->name, obj->level, ( obj->level == 2 ) ? "nd" : ( obj->level == 3 ) ? "rd" : "th" );
    obj->name = str_dup(buf);
    obj->x = bld->x;
    obj->y = bld->y;
    obj->z = bld->z;
    obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
    return;
}

OBJ_DATA * create_teleporter( BUILDING_DATA *bld, int range )
{
    OBJ_DATA *obj;
    char buf[MSL];

    obj = create_object( get_obj_index( OBJ_VNUM_TELEPORTER ), 0 );
    obj->level = range;
    obj->value[0] = range;
    obj->value[1] = bld->type;
    sprintf( buf, "A Series %d, %s-Teleporter", range, capitalize(bld->name) );
    free_string( obj->short_descr );
    free_string( obj->description );
    free_string( obj->name );
    obj->short_descr = str_dup(buf);
    obj->description = str_dup(buf);
    sprintf( buf, "Teleporter %s", bld->name );
    obj->name = str_dup(buf);
    obj->x = bld->x;
    obj->y = bld->y;
    obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
    return obj;
}

OBJ_DATA * create_locator( int range )
{
    OBJ_DATA *obj;
    char buf[MSL];

    obj = create_object( get_obj_index( OBJ_VNUM_LOCATOR ), 0 );
    obj->level = range;
    obj->value[0] = range;
    sprintf( buf, "A Series %d item locator", range );
    free_string( obj->short_descr );
    free_string( obj->description );
    obj->short_descr = str_dup(buf);
    obj->description = str_dup(buf);
    obj->x = 1;
    obj->y = 1;
    obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
    return obj;
}

bool complete( BUILDING_DATA *bld )
{
    int i;
    for ( i=0;i<8;i++ )
        if ( bld->resources[i] > 0 )
            return FALSE;
    return TRUE;
}

OBJ_DATA *create_material( int type )
{
    OBJ_DATA *obj;

    obj = create_object( get_obj_index( OBJ_VNUM_MATERIAL ), 0 );
    obj->value[0] = type;
    free_string( obj->name );
    free_string( obj->short_descr );
    free_string( obj->description );
    if ( type == ITEM_STICK )
    {
        obj->name = str_dup( "resource Stick" );
        obj->short_descr = str_dup( "@@gA broken @@bstick@@N" );
        obj->description = str_dup( "@@gA broken @@bstick@@N" );
    }
    else if ( type == ITEM_IRON )
    {
        obj->name = str_dup("resource Iron");
        obj->short_descr =  str_dup("@@gA small piece of @@dIron@@N");
        obj->description = str_dup("@@gA small piece of @@dIron@@N");
    }
    else if ( type == ITEM_ROCK )
    {
        obj->name = str_dup("resource rock");
        obj->short_descr =  str_dup("@@gA chipped @@drock@@N");
        obj->description = str_dup("@@gA chipped @@drock@@N");
    }
    else if ( type == ITEM_LOG )
    {
        obj->name = str_dup("resource tree log");
        obj->short_descr = str_dup("@@gA fallen @@rlog@@N");
        obj->description = str_dup("@@gA fallen @@rlog@@N");
    }
    else if ( type == ITEM_SKIN )
    {
        obj->name = str_dup("resource skin");
        obj->short_descr = str_dup("@@gSome animal @@bskin@@N");
        obj->description = str_dup("@@gSome animal @@bskin@@N");
    }
    else if ( obj->value[0] == ITEM_COPPER )
    {
        obj->name = str_dup("resource copper");
        obj->short_descr = str_dup("@@gA chip of @@bco@@Rpp@@ber@@N");
        obj->description = str_dup("@@gA chip of @@bco@@Rpp@@ber@@N");
    }
    else if ( obj->value[0] == ITEM_GOLD )
    {
        obj->name = str_dup("resource gold");
        obj->short_descr = str_dup("@@gA @@yg@@bo@@yl@@bd @@gpebble@@N");
        obj->description = str_dup("@@gA @@yg@@bo@@yl@@bd @@gpebble@@N");
    }
    else if ( obj->value[0] == ITEM_SILVER )
    {
        obj->name = str_dup("resource silver");
        obj->short_descr = str_dup("@@gA flake of @@WS@@gi@@dlv@@ge@@Wr@@N");
        obj->description = str_dup("@@gA flake of @@WS@@gi@@dlv@@ge@@Wr@@N");
    }
    else if ( obj->value[0] == -1 )
    {
        obj->name = str_dup("resource alien metal");
        obj->short_descr = str_dup("@@GSome weird @@ralien metal@@N");
        obj->description = str_dup("@@GSome weird @@ralien metal@@N");
    }
    else
    {
        obj->value[0] = 0;
        obj->name = str_dup("resource Iron");
        obj->short_descr =  str_dup("@@gA small piece of @@dIron@@N");
        obj->description = str_dup("@@gA small piece of @@dIron@@N");
    }
    return obj;
}

VEHICLE_DATA *get_vehicle_from_vehicle( VEHICLE_DATA *vhc )
{
    //	VEHICLE_DATA *vhc2;
    if ( !vhc || vhc == NULL || !TRANSPORT_VEHICLE(vhc->type) )
        return NULL;
    /*	for ( vhc2 = first_vehicle;vhc2;vhc2 = vhc2->next )
            if ( vhc2->in_vehicle && vhc2->in_vehicle == vhc )
            {
                vhc2->x = vhc->x;
                vhc2->y = vhc->y;
                vhc2->z = vhc->z;
                return vhc2;
            }*/
    return vhc->vehicle_in;
}

bool is_upgrade( int type )
{
    int i;

    for ( i = 0;i<8;i++ )
        if ( build_table[type].resources[i] != 0 )
            return FALSE;
    return TRUE;
}

bool is_neutral( int type )
{
    int i;

    return FALSE;
    if ( type < 0 || type > MAX_BUILDING )
        return FALSE;
    for ( i = 0;i<8;i++ )
        if ( build_table[type].resources[i] != -1 )
            return FALSE;
    return TRUE;
}

bool is_evil( BUILDING_DATA *bld )
{
    if ( (!is_neutral(bld->type) && !str_cmp(bld->owned,"nobody")) || bld->timer > 0 )
        return TRUE;
    return FALSE;
}

int get_char_cost( CHAR_DATA *ch )
{
    BUILDING_DATA *bld;
    int cost = 0;

    for ( bld = ch->first_building;bld;bld = bld->next_owned )
    {
        cost += bld->level;
    }
    return cost;
}

bool IS_BETWEEN(int x,int x1, int x2)
{
    if ( x1 > x2 )
        if ( x >= x2 && x <= x1 )
            return TRUE;
    if ( x1 < x2 )
        if ( x <= x2 && x >= x1 )
            return TRUE;
    if ( x1 == x2 )
        if ( x == x1 )
            return TRUE;
    return FALSE;
}

bool building_can_shoot( BUILDING_DATA *bld, CHAR_DATA *ch, int range )
{
    BUILDING_DATA *bld2;

    if ( ch == NULL )
        return FALSE;
    if ( IS_IMMORTAL(ch) || (ch->x == MAX_MAPS-1 && ch->y == MAX_MAPS-1) )
        return FALSE;
    if ( (bld->level < 5 && sneak(ch) ) || sneak(ch) )
        return FALSE;
    if ( bld->owner == ch && !practicing(ch) )
        return FALSE;
    if ( ( bld2 = ch->in_building ) != NULL && complete(bld2)  && !open_bld(bld2) )
        return FALSE;
    if ( bld2 && bld2 == bld )
        return FALSE;

    return TRUE;
}

bool open_bld( BUILDING_DATA *bld )
{
    if ( build_table[bld->type].act == BUILDING_OFFENSE
        )
        return TRUE;
    return FALSE;
}

int get_rank( CHAR_DATA *ch )
{
    int rank;

    if ( IS_IMMORTAL(ch) )
        return 9999;
    if ( IS_SET(ch->pcdata->pflags,PLR_BASIC) )
        return 1;

    rank = ((ch->pcdata->tbkills/6) + (ch->pcdata->tpkills/2)) - ( ch->pcdata->deaths/3);
    rank += 1;
    if ( rank < 1 )
        rank = 1;
    else if ( rank > 9998 )
        rank = 9998;
    return rank;
}

int get_bit_value( int bit )
{
    switch (bit)
    {
        case 1: return BIT_1;
        case 2: return BIT_2;
        case 3: return BIT_3;
        case 4: return BIT_4;
        case 5: return BIT_5;
        case 6: return BIT_6;
        case 7: return BIT_7;
        case 8: return BIT_8;
        case 9: return BIT_9;
        case 10: return BIT_10;
        case 11: return BIT_11;
        case 12: return BIT_12;
        case 13: return BIT_13;
        case 14: return BIT_14;
        case 15: return BIT_15;
        case 16: return BIT_16;
        case 17: return BIT_17;
        case 18: return BIT_18;
        case 19: return BIT_19;
        case 20: return BIT_20;
        case 21: return BIT_21;
        case 22: return BIT_22;
        case 23: return BIT_23;
        case 24: return BIT_24;
        case 25: return BIT_25;
        case 26: return BIT_26;
        case 27: return BIT_27;
        case 28: return BIT_28;
        case 29: return BIT_29;
        case 30: return BIT_30;
        case 31: return BIT_31;
        case 32: return BIT_32;
        default: return 0;
    }
    return 0;
}

bool sneak( CHAR_DATA *ch )
{
    int chance,sect;
    if ( !ch )
        return TRUE;

    if ( ch->z == Z_SPACE || ch->z == Z_PAINTBALL )
        return FALSE;
    if ( ch->class == CLASS_DARKOP && ch->in_vehicle == NULL && number_percent() < 22 )
        return TRUE;
    if ( ch->position != POS_SNEAKING )
        return FALSE;
    sect = map_table.type[ch->x][ch->y][ch->z];
    chance = ch->pcdata->skill[gsn_sneak];
    if ( ch->class == CLASS_DARKOP )
        chance += 10;
    if ( ch->class == CLASS_ROBOTIC )
        chance -= 5;
    if ( sect == SECT_ROCK )
        chance -= 7;
    if ( sect == SECT_FIELD )
        chance -= 5;
    if ( sect == SECT_FOREST )
        chance += 10;

    if ( number_percent() < chance )
        return TRUE;
    return FALSE;
}

void check_building_destroyed( BUILDING_DATA *bld )
{
    CHAR_DATA *bch;

    if ( bld == NULL )
        return;
    bch = get_ch(bld->owned);
    if ( bch == NULL )
        return;
    if ( bld->type == BUILDING_IMPLANT_RESEARCH && bch != NULL && bch->implants != 0 )
    {
        send_to_char( "Your implants have lost their link to the research facility!\n\r", bch );
        act( "$n suddenly appears weaker.", bch, NULL, NULL, TO_ROOM );
        bch->max_hit = STARTING_HP;
        bch->implants = 0;
        if ( bch->hit > bch->max_hit )
            bch->hit = bch->max_hit;
    }
    if ( ( bld->type == BUILDING_MARKETPLACE ) && bch->quest_points > 10)
    {
        send_to_char( "@@aYour marketplace has gone to heaven... So have your QPs...@@N\n\r", bch );
        bch->quest_points = 10;
    }
    if ( bld->type == BUILDING_TECH_LAB || bld->type == BUILDING_BIO_LAB )
    {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        for ( obj = bch->first_carry;obj;obj = obj_next )
        {
            obj_next = obj->next_in_carry_list;
            if ( obj->item_type != ITEM_BLUEPRINT )
                continue;
            if ( build_table[obj->value[1]].requirements == bld->type )
                extract_obj(obj);
        }
    }
    if ( GUNNER(bld) )
    {
        if ( bch->in_building == bld && bch->victim != bch )
            bch->victim = bch;
    }
    return;
}

int get_item_limit( BUILDING_DATA *bld )
{
    if ( bld == NULL )
        return 0;
    if ( bld->type == BUILDING_WAREHOUSE || bld->type == BUILDING_GATHERER )
    {
        return bld->level * 20;
    }
    else if ( bld->type == BUILDING_SECURE_WAREHOUSE )
        return bld->level * 25;
    return 20;
}

OBJ_DATA *create_element( int type )
{
    OBJ_DATA *obj;

    obj = create_object( get_obj_index( OBJ_VNUM_ELEMENT ), 0 );
    obj->value[0] = type;
    free_string( obj->name );
    free_string( obj->short_descr );
    free_string( obj->description );
    if ( type == ELEMENT_CINNABAR_ORE )
    {
        obj->value[1] = STATE_SOLID;
        obj->name = str_dup("batch element cinnabar ore");
        obj->short_descr = str_dup("@@dA batch of @@eci@@Rn@@ena@@Rb@@ear ore@@N");
        obj->description = str_dup("@@dA batch of @@eci@@Rn@@ena@@Rb@@ear ore@@N");
    }
    else if ( type == ELEMENT_MERCURY )
    {
        obj->value[1] = STATE_LIQUID;
        obj->name = str_dup("element vial mercury");
        obj->short_descr = str_dup("@@dA Vial of @@gm@@We@@dr@@gc@@Wu@@dr@@Wy@@N");
        obj->description = str_dup("@@dA Vial of @@gm@@We@@dr@@gc@@Wu@@dr@@Wy@@N");
    }
    else if ( type == ELEMENT_GRASS )
    {
        obj->value[1] = STATE_SOLID;
        obj->name = str_dup("element grass");
        obj->short_descr = str_dup("@@rSome grass@@N");
        obj->description = str_dup("@@rSome grass@@N");
    }
    else if ( type == ELEMENT_SOIL )
    {
        obj->value[1] = STATE_SOLID;
        obj->name = str_dup("element soil hard");
        obj->short_descr = str_dup("@@bSome hard soil@@N");
        obj->description = str_dup("@@bSome hard soil@@N");
    }
    else if ( type == ELEMENT_LEAD )
    {
        obj->value[1] = STATE_SOLID;
        obj->name = str_dup("element lead");
        obj->short_descr = str_dup("@@bA bit of @@RLead@@N");
        obj->description = str_dup("@@bA bit of @@RLead@@N");
    }
    else if ( type == ELEMENT_SALT )
    {
        obj->value[1] = STATE_SOLID;
        obj->name = str_dup("element salt");
        obj->short_descr = str_dup("@@gA tiny amount of @@WSalt@@N");
        obj->description = str_dup("@@gA tiny amount of @@WSalt@@N");
    }
    else if ( type == ELEMENT_SODIUM )
    {
        obj->value[1] = STATE_SOLID;
        obj->name = str_dup("element sodium piece");
        obj->short_descr = str_dup("@@dA piece of @@gSodium@@N");
        obj->description = str_dup("@@dA piece of @@gSodium@@N");
    }
    else
    {
        obj->value[1] = STATE_SOLID;
        obj->name = str_dup("element salt");
        obj->short_descr = str_dup("@@gA tiny amount of @@WSalt@@N");
        obj->description = str_dup("@@gA tiny amount of @@WSalt@@N");
    }

    return obj;
}

void send_warning( CHAR_DATA *ch, BUILDING_DATA *bld, CHAR_DATA *victim )
{
    char buf[MSL];
    if ( ch == victim )
        return;
    if ( victim->in_vehicle && victim->in_vehicle->type == VEHICLE_BOMBER )
        return;

    sprintf( buf, "@@rYour %s at %d/%d@@G@@r fires at %s!@@n\n\r", bld->name, bld->x, bld->y, victim->name );
    send_to_char( buf, ch );
    return;
}

void update_ranks( CHAR_DATA *ch )
{
    int i,min,rank,max=0;
    bool stop = FALSE;

    if ( !ch )
        return;

    min = 0;
    rank = get_rank(ch);
    for ( i = 0;i<30;i++ )
    {
        if ( !stop && (rank_table[i].name == NULL || !str_cmp(rank_table[i].name,ch->name) ) )
        {
            min = i;
            stop = TRUE;
            //			break;
        }
        if ( !stop && rank_table[i].rank < rank_table[min].rank )
            min = i;
        if ( rank_table[i].rank > max )
            max = rank_table[i].rank;
    }
    if ( rank_table[min].name != NULL )
        free_string(rank_table[min].name);
    rank_table[min].name = str_dup(ch->name);
    rank_table[min].rank = rank;
    save_ranks();
    
    if ( rank > max )
    {
        web_data.highest_rank = rank;
        update_web_data(WEB_DATA_HIGHEST_RANK,ch->name);
    }
    return;
}

bool defense_building( BUILDING_DATA *bld )
{
    if ( build_table[bld->type].act == BUILDING_DEFENSE )
        return TRUE;
    return FALSE;
}

void sendsound( CHAR_DATA *ch, char *file, int V, int I, int P, char *T, char *filename )
{
    char buf[MSL];
    if ( !IS_SET(ch->config,CONFIG_SOUND) )
        return;
    sprintf( buf, "\n\r!!SOUND(%s V=%d L=%d P=%d T=%s U=%s/MSP/%s)", file, V,I,P,T,WEBSITE,filename );
    send_to_char(buf,ch);
    return;
}

int check_dodge( CHAR_DATA *ch, int chance )
{
    int newchance;
    newchance = chance - ( (chance / 100) * (ch->pcdata->skill[gsn_dodge] /2) );
    if ( ch->pcdata->skill[gsn_dodge] < 100 && number_percent() < 2 )
    {
        send_to_char( "You have become better at dodge!\n\r", ch );
        ch->pcdata->skill[gsn_dodge]++;
        save_char_obj(ch);
    }
    return newchance;
}

bool in_range( CHAR_DATA *ch, CHAR_DATA *victim, int range )
{
    if ( !ch || !victim )
        return FALSE;
    if ( (IS_BETWEEN(victim->x,ch->x-range,ch->x+range)) && (IS_BETWEEN(victim->y,ch->y-range,ch->y+range)))
        return TRUE;
    else
        return FALSE;
}

int get_ship_range( VEHICLE_DATA *vhc )
{
    return vhc->scanner;
}

int get_ship_weapon_range ( VEHICLE_DATA * vhc )
{
    return vhc->range;
}

void make_medal_base( CHAR_DATA *ch )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    int x,y,i,t,h;

    for ( x=BORDER_SIZE;x<100;x++ )
        for ( y=BORDER_SIZE;y<30;y++ )
            if ( map_bld[x][y][Z_PAINTBALL] )
                extract_building(map_bld[x][y][Z_PAINTBALL],FALSE);

    h = URANGE(30,my_get_hours(ch,TRUE),100);
    for ( x=BORDER_SIZE+10;x<MEDAL_BORDER_X;x++ )
        for ( y=BORDER_SIZE;y<MEDAL_BORDER_Y;y++ )
    {
        if ( number_percent() < 50 )
            continue;
        i = number_range(1,5);
        if ( i == 1 )
            t = BUILDING_FLAMESPITTER;
        else if ( i == 2 )
            t = BUILDING_STUNGUN;
        else if ( i == 3 )
            t = BUILDING_L_TURRET;
        else if ( i == 4 )
            t = BUILDING_PSYCHOSTER;
        else
            t = BUILDING_HYDRO_PUMP;
        bld = create_building(t);
        if ( bld->owned != NULL )
            free_string(bld->owned);
        bld->owned = str_dup("nobody");
        bld->owner = NULL;
        bld->hp = (bld->hp * h) / 100;
        bld->x = x;
        bld->y = y;
        bld->exit[0] = TRUE;
        bld->exit[1] = TRUE;
        bld->exit[2] = TRUE;
        bld->exit[3] = TRUE;
        for ( i=0;i<8;i++ )
            bld->resources[i] = 0;
        bld->z = Z_PAINTBALL;
        activate_building(bld,TRUE);
        map_bld[x][y][Z_PAINTBALL] = bld;
    }
    x = number_range(BORDER_SIZE+18,42);
    y = number_range(3,21);
    obj = create_object(get_obj_index(OBJ_VNUM_MEDAL),0);
    obj->x = x;
    obj->y = y;
    obj->z = Z_PAINTBALL;
    obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
    return;
}

void create_obj_atch( CHAR_DATA *ch, int index )
{
    OBJ_DATA *obj;
    obj = create_object(get_obj_index(index),0);
    obj->x = ch->x;
    obj->y = ch->y;
    obj->z = ch->z;
    obj_to_room(obj,ch->in_room);
    return;
}

bool blind_spot( CHAR_DATA *ch, int x, int y )
{
    int range = 4;
    if ( !ch->in_vehicle || ch->in_vehicle == NULL )
        return FALSE;

    if ( ch->in_vehicle->type == VEHICLE_MECH )
        range = 8;
    else if ( AIR_VEHICLE(ch->in_vehicle->type) )
        range = 6;
    else if ( ch->in_vehicle->type == VEHICLE_CREEPER )
        range = 2;

    if ( (x < ch->x - range || x > ch->x + range || y < ch->y - range || y > ch->y + range))
        return TRUE;
    return FALSE;
}

void reset_building(BUILDING_DATA *bld,int type)
{
    int i;
    activate_building(bld,TRUE);
    bld->type = type;
    bld->maxhp = build_table[type].hp;
    bld->maxshield = build_table[type].shield;
    bld->hp = bld->maxhp;
    bld->shield = bld->maxshield;
    bld->level = 1;
    if ( bld->owned != NULL )
        free_string(bld->owned);
    if ( bld->name != NULL )
        free_string(bld->name);
    bld->owned = str_dup("Nobody");
    bld->owner = NULL;
    bld->name = str_dup(build_table[type].name);
    for ( i = 0;i < 8;i++ )
        bld->resources[i] = 0;
}

OBJ_DATA * make_quest_base( int type, int size, int z )
{
    bool bad = TRUE;
    bool skip = FALSE;
    BUILDING_DATA *bld;
    OBJ_DATA *obj = NULL;
    int i,x,y,xx,yy;
    int ox,oy;

    i=0;
    ox = 0;
    oy = 0;
    size /= 1.5;
    while ( bad )
    {
        i++;
        if ( i > 5 )
            return NULL;
        bad = FALSE;
        x = number_range(size+4,MAX_MAPS-size-5);
        y = number_range(size+4,MAX_MAPS-size-5);
        if ( map_table.type[x][y][z] == SECT_NULL )
        {
            bad = TRUE;
            continue;
        }
        for ( xx = x-size-4;xx<x+size+4;xx++ )
            for ( yy = y-size-4;yy<y+size+4;yy++ )
                if ( map_bld[xx][yy][z] )
                    bad = TRUE;

    }
    for ( xx = x-size;xx<x+size;xx++ )
    {
        for ( yy = y-size;yy<y+size;yy++ )
        {
            skip = !skip;
            if ( number_percent() < 15 )
                skip = !skip;
            if ( skip )
                continue;
            if ( ox == 0 || number_percent() < 5 )
            {
                ox = xx;
                oy = yy;
            }
            bld = create_building(type);
            bld->x = xx;
            bld->y = yy;
            bld->z = z;
            map_bld[bld->x][bld->y][bld->z] = bld;
            reset_building(bld,type);
            bld->timer = 60;
            for ( i=0;i<4;i++ )
                bld->exit[i] = TRUE;
        }
    }
    if ( ox > 0 && oy > 0 )
    {
        char buf[MSL];
        obj = create_object(get_obj_index(OBJ_VNUM_SCAFFOLD),0);
        obj->level = 1;
        obj->value[0] = type;
        sprintf(buf,"@@cA@@a %s @@cScaffold@@N",build_table[type].name);
        free_string(obj->short_descr);
        free_string(obj->description);
        obj->short_descr = str_dup(buf);
        obj->description = str_dup(buf);
        sprintf(buf,"%s Scaffold",build_table[type].name);
        free_string(obj->name);
        obj->name = str_dup(buf);
        obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
        move_obj(obj,ox,oy,z);
    }
    return obj;
}

bool open_scaffold(CHAR_DATA *ch, OBJ_DATA *obj)
{
    BUILDING_DATA *bld;
    int i,buildings=0,same=0;
    char buf[MSL];
    if ( !obj || obj->item_type != ITEM_SCAFFOLD || get_building(obj->x,obj->y,obj->z))
        return FALSE;

    if ( !sysdata.killfest )
    {
        for ( bld = first_building;bld;bld = bld->next )
        {
            if ( is_neutral(bld->type) )
                continue;
            if ( str_cmp(ch->name, bld->owned) )
                continue;
            if ( bld->type == obj->value[0] )
                same++;
            buildings++;
        }
    }
    if ( buildings >= BUILDING_LIMIT )
        return FALSE;
    if ( same >= obj->value[1] )
        return FALSE;
    bld = create_building(obj->value[0]);
    if ( bld == NULL )
        return FALSE;
    reset_building(bld,obj->value[0]);
    bld->x = obj->x;
    bld->y = obj->y;
    bld->z = obj->z;
    map_bld[bld->x][bld->y][bld->z] = bld;
    free_string(bld->owned);
    bld->owned = str_dup(ch->name);
    bld->owner = ch;
    sprintf(buf,"%s @@copens up into the @@a%s@@c!@@N\n\r",obj->short_descr,bld->name);
    send_to_loc(buf,obj->x,obj->y,obj->z);
    for ( i=0;i<8;i++ )
        bld->resources[i] = 500;
    for ( i=0;i<4;i++ )
        bld->exit[i] = TRUE;
    bld->hp = 1;
    bld->shield = 1;
    extract_obj(obj);
    reset_special_building(bld);
    return TRUE;
}

bool has_ability(CHAR_DATA *ch,int abil)
{
    if ( ch->pcdata->skill[gsn_engineering] >= ability_table[abil].engineering )
        if ( ch->pcdata->skill[gsn_building] >= ability_table[abil].building )
            if ( ch->pcdata->skill[gsn_combat] >= ability_table[abil].combat )
                return TRUE;
    return FALSE;
}

void reset_special_building(BUILDING_DATA *bld)
{
    if ( bld->type == BUILDING_MINE || bld->type == BUILDING_IMPROVED_MINE || bld->type == BUILDING_LUMBERYARD || bld->type == BUILDING_SPACE_CENTER )
        bld->value[0] = -1;
    else if ( bld->type == BUILDING_DUMMY )
        bld->value[0] = BUILDING_TURRET;
    else if ( bld->type == BUILDING_SCUD_LAUNCHER || bld->type == BUILDING_NUKE_LAUNCHER || bld->type == BUILDING_PSYCHIC_TORMENTOR || bld->type == BUILDING_ALIEN_PROBE )
        bld->value[0] = 360;
    else if ( bld->type == BUILDING_DOOMSDAY_DEVICE || bld->type == BUILDING_HACKPORT || bld->type == BUILDING_TRANSMITTER )
        bld->value[0] = 1500;
    else if ( bld->type == BUILDING_SPY_QUARTERS || bld->type == BUILDING_SPY_SATELLITE )
        bld->value[0] = 360;
    else if ( bld->type == BUILDING_SHOCKWAVE || bld->type == BUILDING_INTERGALACTIC_PUB )
        bld->value[0] = 540;
    else if ( GUNNER(bld) )
        bld->value[0] = 60;
    else if ( bld->type == BUILDING_SPACE_CENTER )
    {
        bld->value[0] = 0;
        bld->value[1] = 0;
    }
    return;
}

int get_armor_value(int dt)
{
    if ( dt == DAMAGE_GENERAL )
        return 2;
    else if ( dt == DAMAGE_BULLETS )
        return 3;
    else if ( dt == DAMAGE_BLAST )
        return 4;
    else if ( dt == DAMAGE_ACID )
        return 5;
    else if ( dt == DAMAGE_FLAME )
        return 6;
    else if ( dt == DAMAGE_LASER )
        return 7;
    else if ( dt == DAMAGE_SOUND )
        return 8;
    else
        return -1;

}

bool hidden(CHAR_DATA *victim)
{
    if ( IS_SET(victim->act, PLR_WIZINVIS) || IS_SET(victim->act, PLR_INCOG) )
        return TRUE;
    if ( victim->in_building && victim->in_building->type == BUILDING_CLUB && complete(victim->in_building) )
        return TRUE;
    return FALSE;
}

int count_buildings(CHAR_DATA *victim)
{
    int i=0;
    BUILDING_DATA *bld;
    if ( !victim || victim == NULL )
        return 999;
    for ( bld = victim->first_building;bld;bld = bld->next_owned )
        i++;
    return i;
}

void clear_basic(CHAR_DATA *ch)
{
    ch->pcdata->pkills = 0;
    ch->pcdata->tpkills = 0;
    ch->pcdata->bkills = 0;
    ch->pcdata->tbkills = 0;
    ch->quest_points = 0;
    ch->pcdata->deaths = 0;
    return;
}
