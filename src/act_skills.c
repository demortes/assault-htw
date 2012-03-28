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
 ~  In order to use any part of this Merc Diku Mud, you must comply with   ~
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

void check_skill( CHAR_DATA *ch, int gsn, int chance, char *name );

const struct skill_type         skill_table[]   =
{
    { "Blueprint Research", gsn_research,   FALSE, "Upgrade a Blueprint to the next level." },
    { "Armor Repair",   gsn_repair,     FALSE, "Fixing Damaged Armor." },
    { "Sneak",      gsn_sneak,  FALSE, "Invisibility on the map/from turrets." },
    { "Dig",        gsn_dig,    FALSE, "Dig the ground for chemical elements." },
    { "Chemistry",      gsn_chemistry,  FALSE, "Creating chemistry-based items." },
    { "Target",         gsn_target,     FALSE, "Locking on to an enemy before firing." },
    { "Computers",      gsn_computer,   FALSE, "Hacking enemy bases." },
    { "Explosive Research", gsn_exresearch, FALSE, "Upgrading bombs." },
    { "Dodge",      gsn_dodge,  FALSE, "Dodging Player vs. Player attacks." },
    { "Engineering",    gsn_engineering, TRUE, "Messing with electronics and vehicles." },
    { "Building",       gsn_building,   TRUE,  "Building structures." },
    { "Combat",         gsn_combat,     TRUE,  "Armor usage, armor piercing, head shots." },
    { "Bombs",      gsn_arm,    FALSE,  "Quick handling of explosives." },
    { "Blind Combat",   gsn_blind_combat, FALSE,  "Seeing while blind." },
    { "Grenades",       gsn_grenades,   FALSE, "Accurate Grenade throwing." },
    { "None", -1, FALSE, "" },
};
const struct ability_type ability_table[] =
{
    // Name               	Eng     Bld     Cmb     Desc
    { "Fix",                  2,      2,      0,      "Repair a building's HP." },
    { "Exploding Ammo",       4,      0,      4,      "15% Chance of instant-kill in weapon-combat." },
    { "Nuclear Containment",  0,  5,  2,  "Reduced nuclear fallout time in your own buildings." },
    { "Peacecraft",       5,  5,  0,  "10% Reduction to damage done to you and by you." },
    { "Fast Healing",     0,  0,  4,  "Recover from bad effects faster." },
    { "Mechanic v1",      3,  0,  0,  "5% Increase in vehicle HP." },
    { "Mechanic v2",      6,  0,  0,  "15% Increase in vehicle HP and Fuel." },
    { "Radioactive",      0,  0,  9,  "Spread nuclear fallout to buildings you are in." },
    { "Electrician",      3,  0,  0,  "Laptop batteries are better conserved." },
    { NULL,                   -1,     -1,     -1,     NULL },
};

char * const wear_name[MAX_WEAR] =
{
    "Head",
    "Eyes",
    "Face",
    "Ear",
    "Ear",
    "Neck",
    "Neck",
    "Shoulders",
    "Arms",
    "Wrist",
    "Wrist",
    "Hands",
    "Finger",
    "Finger",
    "Left Hand",
    "Right Hand",
    "Back",
    "Waist",
    "Body",
    "Legs",
    "Feet",
};
void check_skill( CHAR_DATA *ch, int gsn, int chance, char *name )
{
    char buf[MSL];
    if ( gsn >= MAX_SKILL )
        return;
    if ( number_percent() < chance && ch->pcdata->skill[gsn] < 100 && ch->pcdata->lastskill <= 0 )
    {
        sprintf( buf, "You have become better at %s!\n\r", name );
        send_to_char(buf,ch);
        ch->pcdata->skill[gsn]++;
        ch->pcdata->lastskill = 75;
    }
    return;
}

void do_research (CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    char buf[MSL];

    if ( ch->z == Z_SPACE )
    {
        do_oresearch(ch,argument);
        return;
    }
    if ( ( bld = get_char_building(ch) ) == NULL || !bld )
    {
        send_to_char( "You must be in the building you wish to research.\n\r", ch );
        return;
    }
    if ( bld->type == BUILDING_SPACE_CENTER )
    {
        do_sresearch(ch,argument);
        return;
    }
    /*	if ( argument[0] == '\0' )
        {
            sprintf( buf, "Your blueprint research skill is at %d%%.\n\rSyntax: Research (blueprint in inventory)\n\r", ch->pcdata->skill[gsn_research]);
            send_to_char( buf, ch );
            return;
        }*/
    if ( bld->owner != ch || !complete(bld) )
    {
        send_to_char( "You must be in one of your completed buildings.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
        {
            if ( obj->item_type == ITEM_BLUEPRINT && obj->value[0] == bld->type )
                break;
        }
        if ( (!obj || obj == NULL ) || (obj->item_type != ITEM_BLUEPRINT || obj->value[0] != bld->type ))
        {
            send_to_char( "You do not have the right blueprints for this building in your inventory.\n\r", ch );
            return;
        }

    }
    else
    {
        if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
        {
            send_to_char( "You must specify blueprint in your inventory to research.\n\r", ch );
            return;
        }
    }
    if ( obj->item_type != ITEM_BLUEPRINT || obj->value[0] != bld->type )
    {
        sprintf(buf, "You can't research %s here.\n\r", obj->short_descr );
        send_to_char(buf,ch);
        return;
    }
    if ( obj->level >= 5 )
    {
        send_to_char( "It is already level 5!\n\r", ch );
        return;
    }
    if ( obj->value[1] != 0 )
    {
        send_to_char( "You can't research this blueprint.\n\r", ch );
        return;
    }
    if ( ( get_eq_char(ch,WEAR_HOLD_HAND_L) ) != NULL )
        unequip_char(ch,get_eq_char(ch,WEAR_HOLD_HAND_L));
    equip_char(ch,obj,WEAR_HOLD_HAND_L);
    sprintf( buf, "You start researching %s! (Type @@estop@@N at any time to stop)\n\r", bld->name );
    send_to_char( buf, ch );
    act( "$n starts researching the building.\n\r", ch, NULL, NULL, TO_ROOM );
    ch->c_sn = gsn_research;
    ch->c_time = 40;
    ch->c_level = ch->pcdata->skill[gsn_research];
    if ( ch->class == CLASS_ENGINEER )
        ch->c_level += ch->c_level / 100 * 20;
    return;
}

void act_research( CHAR_DATA *ch, int level )
{
    OBJ_DATA *obj;
    char buf[MSL];

    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_BLUEPRINT )
    {
        send_to_char( "For some reason, you are no longer holding the blueprint you were researching!\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( ch->in_building == NULL || ch->in_building->type != obj->value[0] )
    {
        send_to_char( "For some reason, you are no longer in a building.\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( obj->value[7] >= 10 )
    {
        send_to_char( "You have completed your research!\n\r", ch );
        obj->level++;
        obj->value[7] = 0;
        sprintf( buf, "Blueprints for %d%s level %s", obj->level, ( obj->level == 2 ) ? "nd" : ( obj->level == 3 ) ? "rd" : "th", capitalize(ch->in_building->name) );
        free_string( obj->short_descr );
        free_string( obj->description );
        obj->short_descr = str_dup(buf);
        obj->description = str_dup(buf);
        ch->c_sn = -1;
        return;
    }
    send_to_char( "You continue researching.\n\r", ch );
    check_skill(ch,gsn_research,5,"Blueprint Research");
    if ( number_percent() < ((100-level)/5) + obj->level && number_percent() > (ch->pcdata->skill[gsn_research]/3)*2 )
    {
        send_to_char( "You press some wrong buttons, and mess up!\n\r", ch );
        act( "$n glitches a little.", ch, NULL, NULL, TO_ROOM );
        if ( number_percent() < 25 )
        {
            check_building_destroyed(ch->in_building);
            extract_building(ch->in_building,TRUE);
            obj->value[7]--;
            ch->c_sn = -1;
            return;
        }
        else
        {
            ch->in_building->shield = URANGE(0,ch->in_building->shield-number_percent(),ch->in_building->maxshield);
            obj->value[7]--;
        }
    }
    act( "$n continues researching.\n\r", ch, NULL, NULL, TO_ROOM );
    obj->value[7]++;
    ch->c_time = 40;
    return;
}

void  do_repair( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    BUILDING_DATA *bld;
    char buf[MSL];

    if ( argument[0] == '\0' )
    {
        sprintf( buf, "Your armor repair skill is at %d%%.\n\r", ch->pcdata->skill[gsn_repair]);
        send_to_char( buf, ch );
        return;
    }
    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must be in an armory.\n\r", ch );
        return;
    }
    if ( str_cmp(bld->owned, ch->name) || !complete(bld) )
    {
        send_to_char( "You must be in one of your completed armories.\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You must specify a piece of armor to repair.\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_ARMOR )
    {
        send_to_char( "It is not armor.\n\r", ch );
        return;
    }
    if ( obj->value[1] == 0 )
    {
        send_to_char( "There is no need to repair it.\n\r", ch );
        return;
    }
    if ( ( get_eq_char(ch,WEAR_HOLD_HAND_L) ) != NULL )
        unequip_char(ch,get_eq_char(ch,WEAR_HOLD_HAND_L));
    equip_char(ch,obj,WEAR_HOLD_HAND_L);
    act( "You begin repairing $p. Type ""@@eStop@@N"" at any time to stop.", ch, obj, NULL, TO_CHAR );
    act( "$n begins repairing $p.", ch, obj, NULL, TO_ROOM );
    ch->c_time = 30;
    ch->c_sn = gsn_repair;
    ch->c_level = ch->pcdata->skill[gsn_research];
    return;
}

void act_repair( CHAR_DATA *ch, int level )
{
    OBJ_DATA *obj;

    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_ARMOR )
    {
        send_to_char( "For some reason, you are no longer holding the armor you were repairing!\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( ch->in_building == NULL || ch->in_building->type != BUILDING_ARMORY )
    {
        send_to_char( "For some reason, you are no longer in an armory.\n\r", ch );
        ch->c_sn = -1;
        return;
    }

    if ( obj->value[1] <= 0 )
    {
        obj->value[1] = 0;
        send_to_char( "You finish repairing.\n\r", ch );
        act( "$n finishes repairing.\n\r", ch, NULL, NULL, TO_ROOM );
        ch->c_sn = -1;
        return;
    }
    check_skill(ch,gsn_repair,5,"Armor Repair");
    if ( number_percent() < (100-level)/5 )
    {
        send_to_char( "You mess up!\n\r", ch );
        act( "$n glitches a little.", ch, NULL, NULL, TO_ROOM );
        if ( number_percent() < 10 )
        {
            extract_obj(obj);
            ch->c_sn = -1;
            send_to_char( "You accidently ruin what you were working on.\n\r", ch );
            return;
        }
        else
            obj->value[1]++;
    }
    act( "$n continues repairing.\n\r", ch, NULL, NULL, TO_ROOM );
    obj->value[1]--;
    obj->value[1] -= number_range(0,level/2);
    ch->c_time = 30;
    return;
}

void do_sneak( CHAR_DATA *ch, char *argument )
{
    int dir;

    if ( argument[0] == '\0' )
    {
        char buf[MSL];
        sprintf( buf, "Your sneak skill is at %d%%.\n\r", ch->pcdata->skill[gsn_sneak] );
        send_to_char( buf, ch );
        if ( ch->position == POS_SNEAKING )
        {
            ch->position = POS_STANDING;
            send_to_char( "You stop sneaking.\n\r", ch );
        }
        return;
    }
    if ( ch->in_vehicle )
    {
        send_to_char( "Not while in a vehicle.\n\r", ch );
        return;
    }
    else if ( !str_prefix(argument,"north") )
        dir = DIR_NORTH;
    else if ( !str_prefix(argument,"east" ) )
        dir = DIR_EAST;
    else if ( !str_prefix(argument,"west") )
        dir = DIR_WEST;
    else if ( !str_prefix(argument,"south") )
        dir = DIR_SOUTH;
    else
    {
        send_to_char( "Invalid direction.\n\r", ch );
        return;
    }
    act( "You start sneaking.", ch, NULL, NULL, TO_CHAR );
    if ( number_percent() > ch->pcdata->skill[gsn_sneak] )
        act( "$n starts sneaking.", ch, NULL, NULL, TO_ROOM );
    ch->c_time = 20-(ch->pcdata->skill[gsn_sneak]/6.6);
    ch->c_sn = gsn_sneak;
    ch->c_level = dir;
    return;
}

void act_sneak( CHAR_DATA *ch, int level )
{
    check_skill(ch,gsn_sneak,10,"Sneak");
    move_char(ch,level);
    ch->position = POS_SNEAKING;
    return;
}

void do_dig( CHAR_DATA *ch, char *argument )
{
    if ( planet_table[ch->z].system == 0 )
        return;
    if ( ch->in_building )
    {
        if ( ch->in_building->type == BUILDING_TUNNEL )
        {
            do_tunnel(ch,argument);
            return;
        }
        send_to_char( "You must be outside.\n\r", ch );
        return;
    }

    act( "You start digging for minerals. (Type 'Stop' to stop)\n\r", ch, NULL, NULL, TO_CHAR );
    act( "$n starts digging in the ground.\n\r", ch, NULL, NULL, TO_ROOM );
    ch->c_sn = gsn_dig;
    ch->c_time = 20-(ch->pcdata->skill[gsn_dig]/20);
    ch->c_level = 1;
    return;
}

void act_dig( CHAR_DATA *ch, int level )
{
    OBJ_DATA *obj;
    int terrain,type;

    check_skill(ch,gsn_dig,5,"Digging");
    if ( ( number_percent() / 2 ) < level + ch->pcdata->skill[gsn_dig] )
    {
        type = -1;
        terrain = map_table.type[ch->x][ch->y][ch->z];

        if ( terrain == SECT_WATER )
        {
            send_to_char( "The only thing you get is wet.\n\r", ch );
            ch->c_sn = -1;
            return;
        }
        if ( terrain == SECT_LAVA )
        {
            send_to_char( "The only thing you get is burned.\n\r", ch );
            ch->c_sn = -1;
            return;
        }
        if ( ch->z == Z_GROUND && number_percent() < 2 && terrain == SECT_MOUNTAIN )
        {
            if ( (obj = create_object(get_obj_index(506),0)) == NULL )
            {
                send_to_char( "There was a system error. Please contact an administrator.\n\r", ch );
                return;
            }
            send_to_char( "You pull up a strange rock...\n\r", ch );
            act( "$n pulls up a strange rock...", ch, NULL, NULL, TO_ROOM );
            obj->x = ch->x;
            obj->y = ch->y;
            obj->z = ch->z;
            obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP) );
            free_string(obj->owner);
            obj->owner = str_dup(ch->name);
            ch->c_sn = -1;
            ch->c_time = 0;
            return;
        }
        if ( ch->z == planet_table[ch->z].terrain == TERRAIN_FOREST )
        {
            if ( (obj = create_material(ITEM_ROCK)) == NULL )
            {
                send_to_char( "There was a system error. Please contact an administrator.\n\r", ch );
                return;
            }
            send_to_char( "You pull up a stone.\n\r", ch );
            act( "$n pulls up a stone.", ch, NULL, NULL, TO_ROOM );
            obj->x = ch->x;
            obj->y = ch->y;
            obj->z = ch->z;
            obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP) );
            free_string(obj->owner);
            obj->owner = str_dup(ch->name);
            ch->c_sn = -1;
            ch->c_time = 0;
            return;
        }
        if ( terrain == SECT_MOUNTAIN )
        {
            if ( number_percent() < 50 )
                type = ELEMENT_CINNABAR_ORE;
        }
        else if ( terrain == SECT_FIELD )
        {
            if ( number_percent() < 50 )
                type = ELEMENT_GRASS;
        }
        else
        if ( number_percent() < 50 )
            type = ELEMENT_SOIL;
        if ( type == -1 )
        {
            send_to_char( "You come up with nothing.\n\r", ch );
            act( "$n comes up with nothing.", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            if ( (obj = create_element(type)) == NULL )
            {
                send_to_char( "There was a system error. Please contact an administrator.\n\r", ch );
                return;
            }
            send_to_char( "You find something in there!\n\r", ch );
            act( "$n finds something, and pulls it up.", ch, NULL, NULL, TO_ROOM );
            obj->x = ch->x;
            obj->y = ch->y;
            obj->z = ch->z;
            obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP) );
            free_string(obj->owner);
            obj->owner = str_dup(ch->name);
        }
        ch->c_sn = -1;
        ch->c_time = 0;
        return;
    }
    act( "You continue digging.", ch, NULL, NULL, TO_CHAR );
    act( "$n continues digging.", ch, NULL, NULL, TO_ROOM );
    ch->c_time = 20-(ch->pcdata->skill[gsn_dig]/20);
    ch->c_level++;
    return;
}

void do_chemistry( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    char arg[MSL];
    char buf[MSL];

    argument = one_argument(argument,arg);

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: Chemistry Process <obj>\n\rChemistry State <obj> <solid/liquid/gas>\n\rChemistry Add <obj> <container>\n\rChemistry Mix <container>\n\r", ch );
        return;
    }
    else if ( !str_prefix(arg,"process") )
    {
        int newtype = -1;

        if ( argument[0] == '\0' )
        {
            send_to_char( "Which mineral would you like to process?\n\r", ch );
            return;
        }
        if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
        {
            send_to_char( "You do not carry that.\n\r", ch );
            return;
        }
        if ( obj->item_type != ITEM_ELEMENT )
        {
            send_to_char( "You can't process that.\n\r", ch );
            return;
        }
        if ( obj->value[0] == ELEMENT_CINNABAR_ORE )
            newtype = ELEMENT_MERCURY;
        else if ( obj->value[0] == ELEMENT_SOIL )
        {
            if ( number_percent() < 40 )
                newtype = ELEMENT_LEAD;
            else
                newtype = ELEMENT_SALT;
        }
        else if ( obj->value[0] == ELEMENT_SALT )
            newtype = ELEMENT_SODIUM;

        if ( newtype == -1 )
        {
            send_to_char( "You can't process that.\n\r", ch );
            return;
        }
        obj2 = create_element(newtype);
        sprintf( buf, "You process %s, and get %s.\n\r", obj->short_descr, obj2->short_descr );
        send_to_char( buf, ch );
        sprintf( buf, "$n processes %s, and gets %s.", obj->short_descr, obj2->short_descr );
        act( buf, ch, NULL, NULL, TO_ROOM );
        extract_obj(obj);
        obj_to_char(obj2,ch);
        return;
    }
    else if ( !str_prefix(arg,"state") )
    {
        char arg2[MSL];
        char desc[MSL];
        char name[MSL];
        int state = -1;

        argument = one_argument(argument,arg2);
        if ( ( obj = get_obj_carry(ch,arg2) ) == NULL )
        {
            send_to_char( "You do not carry that object.\n\r", ch );
            return;
        }
        if ( obj->item_type != ITEM_ELEMENT )
        {
            send_to_char( "It is not an element you can process.\n\r", ch );
            return;
        }

        if ( !str_prefix(argument,"solid") )
            state = STATE_SOLID;
        else if ( !str_prefix(argument,"liquid") )
            state = STATE_LIQUID;
        else if ( !str_prefix(argument,"gas") )
            state = STATE_GAS;
        else
        {
            send_to_char( "Valid states are: Solid, Liquid, Gas.\n\r", ch );
            return;
        }

        if ( obj->value[1] == state )
        {
            send_to_char( "That won't change much.\n\r", ch );
            return;
        }

        if ( obj->value[0] == ELEMENT_MERCURY )
        {
            free_string(obj->name);
            free_string(obj->short_descr);
            free_string(obj->description);
            if ( state == STATE_SOLID )
            {
                sprintf( desc, "@@aSome @@WFrozen @@aMercury@@N" );
                sprintf( name, "element Frozen Mercury" );
            }
            else if ( state == STATE_LIQUID )
            {
                sprintf( desc, "@@dA Vial of @@gm@@We@@dr@@gc@@Wu@@dr@@Wy@@N" );
                sprintf( name, "Element vial mercury" );
            }
            else if ( state == STATE_GAS )
            {
                sprintf( desc, "@@dA small container with mercury gas@@N" );
                sprintf( name, "element container mercury gas" );
            }
        }
        else if ( obj->value[0] == ELEMENT_LEAD )
        {

            free_string(obj->name);
            free_string(obj->short_descr);
            free_string(obj->description);
            if ( state == STATE_SOLID )
            {
                sprintf( desc, "@@bA bit of @@RLead@@N" );
                sprintf( name, "element lead" );
            }
            else if ( state == STATE_LIQUID )
            {
                sprintf( desc, "@@bSome melted @@RLead@@N" );
                sprintf( name, "Element melted lead" );
            }
            else if ( state == STATE_GAS )
            {
                sprintf( desc, "@@bA small container with @@Rlead@@b gas@@N" );
                sprintf( name, "element container lead gas" );
            }
        }
        else if ( obj->value[0] == ELEMENT_SODIUM )
        {

            free_string(obj->name);
            free_string(obj->short_descr);
            free_string(obj->description);
            if ( state == STATE_SOLID )
            {
                sprintf( desc, "@@dA piece of @@gSodium@@N" );
                sprintf( name, "element piece sodium" );
            }
            else if ( state == STATE_LIQUID )
            {
                sprintf( desc, "@@dSome melted @@gSodium@@N" );
                sprintf( name, "Element melted sodium" );
            }
            else if ( state == STATE_GAS )
            {
                sprintf( desc, "@@dA small container with @@gSodium@@b gas@@N" );
                sprintf( name, "element container sodium gas" );
            }
        }
        else if ( obj->value[0] == ELEMENT_THORIUM )
        {

            free_string(obj->name);
            free_string(obj->short_descr);
            free_string(obj->description);
            if ( state == STATE_SOLID )
            {
                sprintf( desc, "@@dA @@yTheorite@@d rock@@N" );
                sprintf( name, "element theorite rock thorium" );
            }
            else if ( state == STATE_LIQUID )
            {
                sprintf( desc, "@@dA vial of @@cliquid @@yThorium@@N" );
                sprintf( name, "Element vial liquid thorium" );
            }
            else if ( state == STATE_GAS )
            {
                sprintf( desc, "@@dA small vial containing @@yThorium@@b gas@@N" );
                sprintf( name, "element vial thorium gas" );
            }
        }
        else
        {
            send_to_char( "You can't change this element's state.\n\r", ch );
            return;
        }
        obj->value[1] = state;
        obj->short_descr = str_dup(desc);
        obj->description = str_dup(desc);
        obj->name = str_dup(name);
        send_to_char( "Ok.\n\r", ch );
        return;
    }
    else if ( !str_prefix(arg,"add") )
    {
        char arg2[MSL];
        OBJ_DATA *container;
        int i = 1;
        int x;

        argument = one_argument(argument,arg2);

        if ( ( obj = get_obj_carry(ch,arg2) ) == NULL )
        {
            send_to_char( "You do not carry that element.\n\r", ch );
            return;
        }
        if ( obj->item_type != ITEM_ELEMENT )
        {
            send_to_char( "This is not an element.\n\r", ch );
            return;
        }
        if ( ( container = get_obj_carry(ch,argument) ) == NULL )
        {
            send_to_char( "You do not carry that container.\n\r", ch );
            return;
        }
        if ( container->item_type != ITEM_CONTAINER )
        {
            send_to_char( "This is not a container.\n\r", ch );
            return;
        }
        if ( obj->value[0] == 0 )
        {
            send_to_char( "You can't add that mineral.\n\r", ch );
            return;
        }

        for ( x = obj->value[0]-1;x > 0;x-- )
            i = i * 10;

        x = ( container->value[obj->value[1]] / i ) % 10;
        if ( x == 9 )
        {
            send_to_char( "You can't add any more of that.\n\r", ch );
            return;
        }

        container->value[obj->value[1]] += i;
        sprintf( buf, "You add %s to %s.\n\r", obj->short_descr, container->short_descr );
        send_to_char( buf, ch );
        sprintf( buf, "$n adds %s to %s.\n\r", obj->short_descr, container->short_descr );
        act( buf, ch, NULL, NULL, TO_ROOM );
        extract_obj(obj);
        return;
    }
    else if ( !str_prefix(arg,"mix") )
    {
        OBJ_DATA *obj;

        if ( !ch->in_building || ch->in_building->type != BUILDING_PROCESSING_PLANT )
        {
            send_to_char( "You must be at your processing plant.\n\r", ch );
            return;
        }
        if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
        {
            send_to_char( "You don't carry that container.\n\r", ch );
            return;
        }
        if ( obj->item_type != ITEM_CONTAINER )
        {
            send_to_char( "This is not a container.\n\r", ch );
            return;
        }
        if ( obj->value[0] == 0 && obj->value[1] == 0 && obj->value[2] == 0 )
        {
            send_to_char( "It's empty.\n\r", ch );
            return;
        }
        if ( ( get_eq_char(ch,WEAR_HOLD_HAND_L) ) != NULL )
            unequip_char(ch,get_eq_char(ch,WEAR_HOLD_HAND_L));
        equip_char(ch,obj,WEAR_HOLD_HAND_L);
        act( "You begin mixing the contents of $p.", ch, obj, NULL, TO_CHAR );
        act( "$n begins mixing the contents of $p.", ch, obj, NULL, TO_ROOM );

        ch->c_sn = gsn_chemistry;
        ch->c_time = 30 - (ch->pcdata->skill[gsn_chemistry]/20);
        ch->c_level = 1;
        return;
    }
    else
    {
        send_to_char( "Syntax: Chemistry Process <obj>\n\rChemistry State <obj> <solid/liquid/gas>\n\rChemistry Add <obj> <container>\n\rChemistry Mix <container>\n\r", ch );
        return;
    }

    return;
}

void act_chemistry( CHAR_DATA *ch, int level )
{
    OBJ_DATA *obj;
    OBJ_DATA *new;
    int item = -1;

    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_CONTAINER )
    {
        send_to_char( "For some reason, you are no longer holding the container you were using!\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( !ch->in_building || ch->in_building->type != BUILDING_PROCESSING_PLANT )
    {
        send_to_char( "For some reason, you are no longer in a processing plant.\n\r", ch );
        ch->c_sn = -1;
        return;
    }

    act( "You mix the contents of $p.", ch, obj, NULL, TO_CHAR );
    act( "$n mixes the contents of $p.", ch, obj, NULL, TO_ROOM );
    check_skill(ch,gsn_chemistry,5,"Chemistry");

    if ( number_percent() / 2 < ch->pcdata->skill[gsn_chemistry] + level )
    {
        int i;
        //Formulas

        for ( i=0;formula_table[i].vnum != -1;i++ )
        {
            if ( ( obj->value[0] == formula_table[i].solid
                && obj->value[1] == formula_table[i].liquid
                && obj->value[2] == formula_table[i].gas )
                || ( ch->class == CLASS_SCIENTIST && number_percent() < 10
                && ( ( obj->value[0] > 0 && formula_table[i].solid > 0 ) || ( obj->value[0] == 0 && formula_table[i].solid == 0 ) )
                && ( ( obj->value[1] > 0 && formula_table[i].liquid > 0 ) || ( obj->value[0] == 0 && formula_table[i].solid == 0 ) )
                && ( ( obj->value[2] > 0 && formula_table[i].gas > 0 ) || ( obj->value[0] == 0 && formula_table[i].solid == 0 ) ) ) )
            {
                item = formula_table[i].vnum;
                break;
            }
        }
        if ( item == -1 )
        {
            int x;

            x = ( ( obj->value[0] / 100000 ) % 10 ) + ( ( obj->value[1] / 100000 ) % 10 ) + ( ( obj->value[2] / 100000 ) % 10 );
            if ( number_percent() < x && ch->class != CLASS_SCIENTIST )
            {
                int dam;
                dam = number_range(x/2,x*2);
                act( "$p explodes!!", ch, obj, NULL, TO_CHAR );
                act( "$p explodes!!", ch, obj, NULL, TO_ROOM );
                damage(ch,ch,dam,-1);
                extract_obj(obj);
                return;
            }
            if ( number_percent() < 30 )
                item = OBJ_VNUM_CHINESE_TEA;
            else
                item = OBJ_VNUM_POISON_TEA;
        }

        if ( ( new = create_object(get_obj_index(item),0) ) == NULL )
        {
            send_to_char( "There was a system error. Please contact an administrator.\n\r", ch );
            return;
        }

        act( "You finish mixing, and get $p!", ch, new, NULL, TO_CHAR );
        act( "$n finishes mixing, and gets $p!", ch, new, NULL, TO_ROOM );
        ch->c_sn = -1;
        obj_to_char(new,ch);
        extract_obj(obj);
        return;
    }

    ch->c_level++;
    ch->c_time = 20 - (ch->pcdata->skill[gsn_chemistry]/20);
    return;
}

void do_target( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim=NULL;
    OBJ_DATA *weapon;
    int range;
    int time;
    BUILDING_DATA *bld = NULL;
    bool list = FALSE;
int btime;
    if ( IN_PIT(ch) )
        mreturn( "Not in the pit.\n\r", ch );

    if ( argument[0] == '\0' )
    {
        //		send_to_char( "Target whom?\n\r", ch );
        //		return;
        list = TRUE;
    }

    else
    {
        if ( ( victim = get_char_world(ch,argument) ) == NULL )
        {
            send_to_char( "You can't find that target.\n\r", ch );
            return;
        }

        if ( victim == ch )
        {
            act( "You stop targetting $N.", ch, NULL, ch->victim, TO_CHAR );
            ch->victim = ch;
            return;
        }
    }
    if ( ch->in_building && GUNNER(ch->in_building) && (ch->in_building->owner && ch->in_building->owner == ch) )
        bld = ch->in_building; 

	if (list != TRUE && (ch->x != victim->x || ch->y != victim->y || ch->z != victim->z) && bld == NULL)
	{	
		if (ch->in_building && !open_bld(ch->in_building))
		{
			send_to_char( "You can't target from inside the building.\n\r", ch );
			return;
		}
		if (victim->in_building && !open_bld(victim->in_building) )
		{
			send_to_char( "You can't target through walls.\n\r", ch );
			return;
		}
	}
    if ( (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL || weapon->item_type != ITEM_WEAPON ) && ch->in_vehicle == NULL && bld == NULL )
    {
        if (  ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) == NULL )
        {
            send_to_char( "You aren't holding a weapon!\n\r", ch );
            return;
        }
    }
    if ( ch->in_vehicle == NULL && bld == NULL )
    {
        if (  weapon->item_type != ITEM_WEAPON )
        {
            send_to_char( "This isn't a gun you're holding!\n\r", ch );
            return;
        }
        range = 1 + weapon->value[4];
        if ( ch->class == CLASS_SNIPER )
            range += 1;
        if ( range < 1 )
            range = 1;
        if ( ch->in_building && ch->in_building->type == BUILDING_SNIPER_TOWER && complete(ch->in_building) )
            range += 5;
    }
    else if ( bld != NULL )
    {
        range = 6;
    }
    else
    {
        if ( SPACE_VESSAL(ch->in_vehicle) )
        {
            if ( ch->z != Z_SPACE )
            {
                send_to_char( "Your ship isn't operational until you enter space!\n\r", ch );
                return;
            }
            range = get_ship_weapon_range(ch->in_vehicle);
        }
        else if ( AIR_VEHICLE(ch->in_vehicle->type) )
        {
            if ( ch->z != Z_AIR )
            {
                send_to_char( "The craft isn't operational until you lift!\n\r", ch );
                return;
            }
            range = 3;
        }
        else
            range = 1;
    }

    if ( list )
    {
        char buf[MSL];
        int x,y;

        sprintf( buf, "You are able to target the following people:\n\r\n\r" );
        for ( x=ch->x-range;x<=ch->x+range;x++ )
            for ( y=ch->y-range;y<=ch->y+range;y++ )
        {
            if ( x < BORDER_SIZE || y < BORDER_SIZE || x > MAX_MAPS - BORDER_SIZE || y > MAX_MAPS - BORDER_SIZE )
                continue;
            for ( victim = map_ch[x][y][ch->z];victim;victim = victim->next_in_room )
            {
                sprintf( buf+strlen(buf), "@@g%s @@c(@@a%d@@c/@@a%d@@c)\n\r", victim->name, x,y);
            }
        }
        send_to_char(buf,ch);
        return;
    }

    if ( ch->z != victim->z )
    {
        if ( (ch->z == Z_GROUND || ch->z == Z_AIR) && (victim->z == Z_GROUND || victim->z == Z_AIR) )
        {
            if ( ch->in_vehicle != NULL || (weapon && !IS_SET(weapon->value[3], WEAPON_HITS_AIR)) )
            {
                send_to_char( "You can't lock on to air targets using this weapon.\n\r", ch );
                return;
            }
        }
        else
        {
            send_to_char( "Your target is not in range.\n\r", ch );
            return;
        }
    }
    if ( ch->victim && ch->victim == victim )
    {
        send_to_char( "They are already targetted.\n\r", ch );
        return;
    }
    /*	if ( !NOT_IN_ROOM(ch,victim) )
        {
            send_to_char( "He's right here!\n\r", ch );
            return;
        }*/

    if ( ch->in_vehicle || bld != NULL )
        time =  (40 - ((ch->pcdata->skill[gsn_target] / 20) - (victim->pcdata->skill[gsn_target] / 20))) * 2;
    else
        time =  (40 - ((ch->pcdata->skill[gsn_target] / 20) - (victim->pcdata->skill[gsn_target] / 20))) - weapon->value[6];

    if ( time < 5 )
        time = 5;

if (IS_SET(ch->config,CONFIG_BLIND))
{
                                    if (!ch->in_building)
                                    time=5;
}
                                  
    if ( victim->z == Z_AIR && ch->z == Z_GROUND )
        time = 1;

    if ( victim->in_room->vnum == ROOM_VNUM_WMAP && (IS_BETWEEN(victim->x,ch->x-range,ch->x+range)) && (IS_BETWEEN(victim->y,ch->y-range,ch->y+range)))
    {
        if (( IN_PIT(ch) && !IN_PIT(victim) ) || ( !IN_PIT(ch) && IN_PIT(victim) ) )
            return;
        act( "You begin targetting $N!", ch, NULL, victim, TO_CHAR );
        act( "$n begins targetting $N!", ch, NULL, victim, TO_ROOM );
        ch->victim = victim;
        ch->c_sn = gsn_target;
        ch->c_time = time;
        ch->c_level = 1;
        if ( ch->victim->in_vehicle && SPACE_VESSAL(ch->victim->in_vehicle) && ch->in_vehicle && SPACE_VESSAL(ch->in_vehicle) )
            send_to_char( "@@eALERT! Missile Lock-On Detected!!@@N\n\r", ch->victim );
        return;
    }
    else
    {
        send_to_char( "Your target is not in range.\n\r", ch );
        return;
    }
    return;
}

void act_target (CHAR_DATA *ch, int level )
{
    OBJ_DATA *weapon;
    int range;
    BUILDING_DATA *gunner = NULL;

    if ( !ch->victim || ch->victim == NULL )
    {
        send_to_char( "For some reason, your victim isn't online anymore.\n\r", ch );
        ch->victim = ch;
        return;
    }
    if ( ch->victim == ch )
    {
        send_to_char( "For some reason, you were targetting yourself!\n\r", ch );
        ch->victim = ch;
        return;
    }
    if ( ch->in_building && GUNNER(ch->in_building) )
        gunner = ch->in_building;
    if ( ( ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_L ) ) == NULL || weapon->item_type != ITEM_WEAPON ) && ch->in_vehicle == NULL && gunner == NULL )
    {
        if ( ( ( weapon = get_eq_char( ch, WEAR_HOLD_HAND_R ) ) == NULL || weapon->item_type != ITEM_WEAPON ) && ch->in_vehicle == NULL )
        {
            send_to_char( "For some reason, you aren't holding a weapon anymore!\n\r", ch );
            ch->victim = ch;
            return;
        }
    }

    if ( (ch->pcdata->alliance == -1 || ch->pcdata->alliance != ch->victim->pcdata->alliance ) )
        check_skill(ch,gsn_target,15,"Target");
    if ( ch->in_vehicle == NULL && gunner == NULL )
    {
        if ( weapon->item_type != ITEM_WEAPON )
        {
            send_to_char( "This isn't a gun you're holding!\n\r", ch );
            return;
        }
        range = 1 + weapon->value[4];
        if ( ch->class == CLASS_SNIPER )
            range += 1;
        if ( range < 1 )
            range = 1;
        if ( ch->in_building && ch->in_building->type == BUILDING_SNIPER_TOWER )
            range += 5;
    }
    else if ( gunner != NULL )
    {
        range = 6;
    }
    else
    {
        if ( SPACE_VESSAL(ch->in_vehicle) )
            range = get_ship_weapon_range(ch->in_vehicle);
        else
            range = 1;
    }

    if ( ch->victim->in_room->vnum != ROOM_VNUM_WMAP || (!IS_BETWEEN(ch->victim->x,ch->x-range,ch->x+range)) || (!IS_BETWEEN(ch->victim->y,ch->y-range,ch->y+range)) )
    {
        send_to_char( "Your victim got away!\n\r", ch );
        ch->victim = ch;
        return;
    }
    if ( ch->z != ch->victim->z )
    {
        if ( ch->in_vehicle != NULL || (weapon && !IS_SET(weapon->value[3],WEAPON_HITS_AIR) ) )
        {
            send_to_char( "Your victim got away!\n\r", ch );
            ch->victim = ch;
            return;
        }
    }

    if ( ch->victim->in_vehicle && ch->victim->in_vehicle->state == VEHICLE_STATE_EVADE )
    {
        if ( number_percent() < 33 )
        {
            ch->victim = ch;
            send_to_char( "You couldn't get a lock.\n\r", ch );
            return;
        }
    }
    act( "You are locked on $N!", ch, NULL, ch->victim, TO_CHAR );
    act( "$n is locked on!", ch, NULL, NULL, TO_ROOM );
    if ( ch->victim->in_vehicle && SPACE_VESSAL(ch->victim->in_vehicle) && ch->in_vehicle && SPACE_VESSAL(ch->in_vehicle) )
    {
        send_to_char( "@@lALERT!! You have been locked on!!@@N\n\r", ch->victim );
        if ( ch->victim->c_sn == -1 && ch->victim->victim == ch->victim )
        {
            send_to_char( "Attempting to lock on attacker...\n\r", ch->victim );
            ch->victim->victim = ch;
            ch->victim->c_sn = gsn_target;
            ch->victim->c_time = 4;
        }
    }
    return;
}

void do_kick( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MSL];
    char buf[MSL];
    OBJ_DATA *eq;
    int loc,dam;

    dam = 0;
    argument = one_argument(argument,arg);

    if ( ch->in_vehicle )
    {
        send_to_char( "Sure! You'll just reach out of the vehicle with your super-long leg!...\n\r", ch );
        return;
    }
    if ( paintball(ch) )
    {
        send_to_char( "Not during a paintball game!\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room(ch,arg) ) == NULL )
    {
        send_to_char( "You can't find your target here.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "You're not that flexible.\n\r", ch );
        return;
    }
    if ( victim->in_vehicle )
    {
        send_to_char( "That would be hard to do... Your victim being inside a vehicle and all...\n\r", ch );
        return;
    }

    if ( ( loc = get_loc(argument) ) == -1 )
        loc = number_range(1,MAX_WEAR);

    if ( practicing(ch) && ch != victim )
        do_practice(ch,"");
    if ( practicing(victim) && ch != victim )
        do_practice(victim,"");

    dam = number_range(get_rank(ch)/2,get_rank(ch));
    if ( dam <= 0 )
        dam = 1;
    if ( dam > 99 )
        dam = 99;
    sprintf( buf, "You kick $N in the %s!", wear_name[loc] );
    act( buf, ch, NULL, victim, TO_CHAR );
    sprintf( buf, "$n kicks $N in the %s!", wear_name[loc] );
    act( buf, ch, NULL, victim, TO_NOTVICT );
    sprintf( buf, "$n kicks you in the %s!", wear_name[loc] );
    act( buf, ch, NULL, victim, TO_VICT );
    if ( ( eq = get_eq_char( victim, loc ) ) != NULL )
    {
        if ( eq->item_type == ITEM_ARMOR )
        {
            if ( (eq->value[0] == -1 || eq->value[0] == -2) && number_percent() < eq->level * 3 )
            {
                if ( number_percent() < eq->level )
                {
                    act( "The damage is absorbed by $p!", ch, eq, NULL, TO_CHAR );
                    act( "The damage is absorbed by $p!", ch, eq, NULL, TO_ROOM );
                    eq->value[1] += dam;
                    dam = 0;
                }
                else
                {
                    int absorb;
                    act( "$p absorbs some of the damage.", ch, eq, NULL, TO_CHAR );
                    act( "$p absorbs some of the damage.", ch, eq, NULL, TO_ROOM );
                    absorb= number_range(1,dam - eq->value[1]);
                    eq->value[1] += absorb;
                    dam -= absorb;
                }
                check_armor(eq);
            }
        }
    }

    if ( loc == WEAR_BODY && number_percent() < dam )
    {
        act( "The kick sends you FLYING!", victim, NULL, NULL, TO_CHAR );
        act( "The kick sends $n FLYING!", victim, NULL, NULL, TO_ROOM );
        dam += number_range(1,get_rank(ch));
    }

    victim->hit -= dam;
    update_pos(victim);
    if ( check_dead(ch,victim) )
        return;

    if ( loc == WEAR_HEAD && number_percent() < dam )
    {
        send_to_char( "You feel dizzy...\n\r", victim );
        act( "$n looks dizzy...", victim, NULL, NULL, TO_ROOM );
        WAIT_STATE(victim,number_range(1,20));
    }

    set_fighting(ch, victim);

    tail_chain( );
    WAIT_STATE(ch,16);
    return;
}

void do_punch( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char arg[MSL];
    char buf[MSL];
    OBJ_DATA *eq;
    int loc,dam;

    dam = 0;
    argument = one_argument(argument,arg);

    if ( ch->in_vehicle )
    {
        send_to_char( "Sure! You'll just reach out of the vehicle with your super-long arm!...\n\r", ch );
        return;
    }
    if ( paintball(ch) )
    {
        send_to_char( "Not during a paintball game!\n\r", ch );
        return;
    }
    if ( ( victim = get_char_room(ch,arg) ) == NULL )
    {
        send_to_char( "You can't find your target here.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "You're not that flexible.\n\r", ch );
        return;
    }
    if ( victim->in_vehicle )
    {
        send_to_char( "That would be hard to do... Your victim being inside a vehicle and all...\n\r", ch );
        return;
    }

    loc = WEAR_HEAD;

    if ( practicing(ch) && ch != victim )
        do_practice(ch,"");
    if ( practicing(victim) && ch != victim )
        do_practice(victim,"");

    dam = number_range(get_rank(ch)/2,get_rank(ch));
    if ( dam <= 0 )
        dam = 1;
    if ( dam > 99 )
        dam = 99;
    sprintf( buf, "You kick $N in the %s!", wear_name[loc] );
    act( buf, ch, NULL, victim, TO_CHAR );
    sprintf( buf, "$n kicks $N in the %s!", wear_name[loc] );
    act( buf, ch, NULL, victim, TO_NOTVICT );
    sprintf( buf, "$n kicks you in the %s!", wear_name[loc] );
    act( buf, ch, NULL, victim, TO_VICT );
    if ( ( eq = get_eq_char( victim, loc ) ) != NULL )
    {
        if ( eq->item_type == ITEM_ARMOR )
        {
            if ( (eq->value[0] == -1 || eq->value[0] == -2) && number_percent() < eq->level * 3 )
            {
                if ( number_percent() < eq->level )
                {
                    act( "The damage is absorbed by $p!", ch, eq, NULL, TO_CHAR );
                    act( "The damage is absorbed by $p!", ch, eq, NULL, TO_ROOM );
                    eq->value[1] += dam * 1.5;
                    dam = 0;
                }
                else
                {
                    int absorb;
                    act( "$p absorbs some of the damage.", ch, eq, NULL, TO_CHAR );
                    act( "$p absorbs some of the damage.", ch, eq, NULL, TO_ROOM );
                    absorb= number_range(1,dam - eq->value[1]);
                    eq->value[1] += absorb * 1.5;
                    dam -= absorb;
                }
                check_armor(eq);
            }
        }
    }

    victim->hit -= dam;
    update_pos(victim);
    if ( check_dead(ch,victim) )
        return;

    if ( number_percent() < dam )
    {
        send_to_char( "You feel dizzy...\n\r", victim );
        act( "$n looks dizzy...", victim, NULL, NULL, TO_ROOM );
        WAIT_STATE(victim,number_range(1,20));
    }

    set_fighting(ch, victim);

    tail_chain( );
    WAIT_STATE(ch,16);
    return;
}

void do_computer( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    char arg1[MSL];
    char arg2[MSL];

    argument = one_argument(argument,arg1);
    argument = one_argument(argument,arg2);

    if ( !str_cmp(arg1,"defend"))
    {
        if ( ( bld = ch->in_building ) == NULL )
        {
            send_to_char( "You can't defend the building while you are not inside.\n\r", ch );
            return;
        }
        if ( bld->value[8] == 0 )
        {
            send_to_char( "There are no traces of a hacker here.\n\r", ch );
            return;
        }
        act( "You begin defending against the hacker.", ch, NULL, NULL, TO_CHAR );
        act( "$n goes to the computer terminal and begins defending against the hacker.", ch, NULL, NULL, TO_ROOM );
        ch->c_sn = gsn_computer;
        ch->c_level = 1;
        ch->c_time = 30 - ( ch->pcdata->skill[gsn_computer] / 4 );
        return;
    }
    else if ( !str_cmp(arg1,"repair"))
    {
        if ( ( bld = ch->in_building ) == NULL )
        {
            send_to_char( "You can't repair the building while you are not inside.\n\r", ch );
            return;
        }
        if ( bld->value[3] == 0 )
        {
            send_to_char( "There are no traces of a virus here.\n\r", ch );
            return;
        }
        /*		if ( bld->value[3] > 0 )
                {
                    send_to_char( "The virus has not yet reached your system. You should use the 'Defend' option now.\n\r", ch );
                    return;
                }*/
        act( "You begin defending against the virus.", ch, NULL, NULL, TO_CHAR );
        act( "$n goes to the computer terminal and begins defending against the virus.", ch, NULL, NULL, TO_ROOM );
        ch->c_sn = gsn_computer;
        ch->c_level = 2;
        ch->c_time = 40 - ( ch->pcdata->skill[gsn_computer] / 4 );
        return;
    }
    else if ( !str_cmp(arg1,"scan"))
    {
        if ( ( bld = ch->in_building ) == NULL )
        {
            send_to_char( "You can't repair the building while you are not inside.\n\r", ch );
            return;
        }
        if ( bld->type != BUILDING_HQ )
        {
            send_to_char( "You need to be in a headquarters.\n\r", ch );
            return;
        }
        act( "You begin resetting your system.", ch, NULL, NULL, TO_CHAR );
        act( "$n goes to the computer terminal and begins resetting the systems.", ch, NULL, NULL, TO_ROOM );
        ch->c_sn = gsn_computer;
        ch->c_level = 9;
        ch->c_time = 80 - ( ch->pcdata->skill[gsn_computer] / 4 );
        return;
    }
    if ( !str_cmp(arg1,"protect"))
    {
        OBJ_DATA *obj;
        OBJ_DATA *comp = NULL;
        for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
            if ( obj->item_type == ITEM_COMPUTER )
                if ( comp == NULL || obj->value[0] > comp->value[0] )
                    comp = obj;

        if ( comp == NULL )
        {
            send_to_char( "You must be carrying a computer in your inventory!\n\r", ch );
            return;
        }
        if ( comp->value[0] <= 2 )
        {
            send_to_char( "This computer is no good for setting up dummies.\n\r", ch );
            return;
        }
        if ( ( bld = ch->in_building ) == NULL || bld->type != BUILDING_HQ || !complete(bld) )
        {
            send_to_char( "You must be in a headquarters.\n\r", ch );
            return;
        }
        act( "You begin setting some dummy directories on the mainframe.", ch, NULL, NULL, TO_CHAR );
        act( "$n begins setting up dummy directories on the mainframe.", ch, NULL, NULL, TO_ROOM );
        ch->c_sn = gsn_computer;
        ch->c_level = 8;
        ch->c_time = 60 - ( ch->pcdata->skill[gsn_computer] / 4 );
        return;
    }
    else if ( !str_cmp(arg1,"backdoor"))
    {
        CHAR_DATA *wch;
        OBJ_DATA *obj;
        OBJ_DATA *comp = NULL;
        bool found = FALSE;
        int range;

        if ( arg2[0] == '\0' )
        {
            send_to_char( "Install a back door at whose computer?\n\r", ch );
            return;
        }
        for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
            if ( obj->item_type == ITEM_COMPUTER )
                if ( comp == NULL || obj->value[0] > comp->value[0] )
                    comp = obj;

        if ( comp == NULL )
        {
            send_to_char( "You must be carrying a computer in your inventory!\n\r", ch );
            return;
        }
        if ( ( wch = get_char_world(ch,arg2) ) == NULL )
        {
            send_to_char( "No such player.\n\r", ch );
            return;
        }
        if ( wch == ch )
        {
            send_to_char( "You don't really need to hack yourself... You already know where you are.\n\r", ch );
            return;
        }
        range = comp->value[0];
        if ( !IS_BETWEEN(wch->x,comp->carried_by->x-range,comp->carried_by->x+range) || !IS_BETWEEN(wch->y,comp->carried_by->y+range,comp->carried_by->y-range) || wch->z != ch->z )
        {
            send_to_char( "Your computer doesn't have that range.\n\r", ch );
            return;
        }
        for ( obj = wch->first_carry;obj;obj = obj->next_in_carry_list )
        {
            if ( obj->item_type == ITEM_COMPUTER && obj->value[3] == 0 )
            {
                found = TRUE;
                break;
            }
        }
        if ( !found )
        {
            send_to_char( "Either your target does not carry a computer, or all of their computers are already infected.\n\r", ch );
            return;
        }
        ch->c_sn = gsn_computer;
        ch->c_time = 40 - ( ch->pcdata->skill[gsn_computer] / 4 );
        ch->c_level = 5;
        ch->victim = wch;
        act( "You begin hacking $N's computer!", ch, NULL,wch, TO_CHAR );
        act( "$n pulls up his laptop, and begins hacking someone.", ch, NULL, NULL, TO_ROOM );
        return;
    }
    else if ( !str_cmp(arg1,"connect"))
    {
        int x,y,z,range;
        OBJ_DATA *obj;
        OBJ_DATA *comp = NULL;
        CHAR_DATA *bch;
        CHAR_DATA *bounce = NULL;
        char arg3[MSL];
        bool found = FALSE;

        argument = one_argument(argument,arg3);

        if ( !is_number(arg2) || !is_number(arg3) )
        {
            send_to_char( "You must insert valid coordinates.\n\r", ch );
            return;
        }

        x = atoi(arg2);
        y = atoi(arg3);
        for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
            if ( obj->item_type == ITEM_COMPUTER )
                if ( comp == NULL || obj->value[0] > comp->value[0] )
                {
                    comp = obj;
                    found = TRUE;
                }

        if ( !found )
        {
            send_to_char( "You must be carrying a computer in your inventory!\n\r", ch );
            return;
        }

        found = FALSE;
        if ( argument[0] != '\0' )
        {
            if ( ( bounce = get_ch(argument) ) == NULL )
            {
                send_to_char( "You couldn't find that target to bounce through.\n\r", ch );
                return;
            }

            if ( IS_SET(bounce->effect,EFFECT_ENCRYPTION) )
            {
                send_to_char( "You just can't seem to lock on that target...\n\r", ch );
                return;
            }

            for ( obj = bounce->first_carry;obj;obj = obj->next_in_carry_list )
            {
                if ( obj->item_type == ITEM_COMPUTER && obj->value[3] != 0 )
                {
                    found = TRUE;
                    break;
                }
            }
            if ( !found )
            {
                send_to_char( "That target doesn't have a hackable computer. You must infect one of their laptops with a trojan first.\n\r", ch );
                return;
            }
        }

        if ( comp->value[1] <= 0 )
        {
            send_to_char( "Its battery is dead.\n\r", ch );
            return;
        }
        comp->value[1]--;
        if ( bounce != NULL && found )
        {
            comp = obj;
            send_to_char( "You bounce your connection through a hacked computer.\n\r", ch );
        }
        range = comp->value[0];

        if ( !IS_BETWEEN(x,comp->carried_by->x-range,comp->carried_by->x+range) || !IS_BETWEEN(y,comp->carried_by->y+range,comp->carried_by->y-range) )
        {
            send_to_char( "Your target is outside the computer's range.\n\r", ch );
            return;
        }
        /*		if ( ch->z == 0 && comp->value[2] == 0 )
                {
                    send_to_char( "Your computer can't reach the target from the underground.\n\r", ch );
                    return;
                }*/
        z = (bounce && found)?bounce->z:ch->z;
        if ( ch->z == 0 && !map_bld[x][y][z] && comp->value[2] > 0 )
            z = 1;
        if ( ( bld = get_building(x,y,z) ) == NULL )
        {
            if ( z != Z_UNDERGROUND || (( bld = get_building(x,y,Z_GROUND )) == NULL) )
            {
                send_to_char( "There is no building over there.\n\r", ch );
                return;
            }
        }
        if ( is_neutral(bld->type) || is_evil(bld) || !complete(bld) || !str_cmp(bld->owned,"nobody") )
        {
            send_to_char( "It has no computer network.\n\r", ch );
            return;
        }
        if ( ( bch = get_ch(bld->owned) ) == NULL )
        {
            send_to_char( "Its mainframe is shut down.\n\r", ch );
            return;
        }
        if ( IN_PIT(bch) || paintball(bch) )
        {
            send_to_char( "That's not fair...\n\r", ch );
            return;
        }
        ch->c_sn = gsn_computer;
        ch->c_level = 7;
        ch->c_time = 40;
        ch->bvictim = bld;
        act( "$n pulls out a laptop and begins typing...", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Connecting...\n\r", ch );
        sendsound(ch,"waitcon",40,1,25,"misc","waitcon.wav");
        ch->c_obj = comp;
    }
    else
    {
        send_to_char( "Syntax: Computer Defend - Defends against connected hackers.\n\rComputer Repair - Repairs viruses that have entered the system.\n\rComputer Scan - Scans and repairs evil programs.\n\rComputer Protect - Creates dummy directories in a whole base\n\rComputer Connect <x> <y> <bounce?> - Attempts to establish a connection to a given building. Must carry a laptop. You can also set a player's name to Bounce the connection through.\n\rComputer Backdoor - Uploads a trojan to the target player's laptop, if carrying one, which you can track at a Hackers Hideout.", ch );
        return;
    }
    return;
}

void act_computer( CHAR_DATA *ch, int level )
{
    BUILDING_DATA *bld;
    int chance;

    if ( ( bld = ch->in_building ) == NULL && ( ch->c_level == 1 || ch->c_level == 2 || ch->c_level == 8 || ch->c_level == 9) )
    {
        send_to_char( "For some reason, you are no longer in a building.\n\r", ch );
        ch->c_sn = -1;
        ch->c_level = 0;
        return;
    }
    if ( number_percent() < 5 && ch->pcdata->skill[gsn_computer] < 100)
    {
        send_to_char( "You have become better at Computers!\n\r", ch );
        ch->pcdata->skill[gsn_computer]++;
    }
    if ( level == 1 )
    {
        if ( bld->value[8] == 0 )
        {
            send_to_char( "The hacker has already left your system!!\n\r", ch );
            ch->c_sn = -1;
            ch->c_level = 0;
            return;
        }
        ch->c_time = 30 - ( ch->pcdata->skill[gsn_computer] / 4 );
        act( "You work hard on the computer terminal - Trying to defend against the hacker.", ch, NULL, NULL, TO_CHAR );
        act( "$n works hard on the computer terminal - Trying to defend against the hacker.", ch, NULL, NULL, TO_ROOM );
    }
    if ( level == 2 )
    {
        if ( bld->value[3] == 0 )
        {
            send_to_char( "The virus has been cleared out of your system!!\n\r", ch );
            ch->c_sn = -1;
            ch->c_level = 0;
            return;
        }
        ch->c_time = 40 - ( ch->pcdata->skill[gsn_computer] / 4 );
        act( "You work hard on the computer terminal - Trying to defend against the virus.", ch, NULL, NULL, TO_CHAR );
        act( "$n works hard on the computer terminal - Trying to defend against the virus.", ch, NULL, NULL, TO_ROOM );
    }
    if ( level == 3 || level == 4 )
    {
        char buf[MSL];
        if ( ch->bvictim == NULL )
        {
            send_to_char( "Your target was shut down.\n\r", ch );
            ch->c_sn = -1;
            ch->bvictim = NULL;
            ch->c_level = 0;
            return;
        }
        if ( ch->bvictim->value[3] > 0 )
        {
            sprintf( buf, "Estimated time: Less than %d seconds.\n\r", ch->bvictim->value[3] * 10 );
            act( "$n continues uploading the virus.", ch, NULL, NULL, TO_ROOM );
            send_to_char( buf, ch );
            ch->c_time = 40;
        }
        else
        {
            if ( level == 3 )
                send_to_char( "Your virus was successfully implanted!\n\r", ch );
            else
            {
                send_to_char( "You have upgraded the virus!\n\r", ch );
                ch->bvictim->value[3]--;
            }
            ch->c_sn = -1;
            ch->c_level = -1;
            free_string(ch->bvictim->attacker);
            ch->bvictim->attacker = str_dup(ch->name);
            ch->bvictim = NULL;
            return;
        }
        return;
    }
    if ( level == 5 )
    {
        OBJ_DATA *obj;
        bool found = FALSE;

        if ( ch->victim == ch )
        {
            send_to_char( "Your victim has left!\n\r", ch );
            ch->c_sn = -1;
            return;
        }
        for ( obj = ch->victim->first_carry;obj;obj = obj->next_in_carry_list )
        {
            if ( obj->item_type == ITEM_COMPUTER )
            {
                found = TRUE;
                break;
            }
        }
        if ( !found )
        {
            send_to_char( "For some reason, your target is no longer carrying a computer.\n\r", ch );
            return;
        }
        obj->value[3] = 1;
        send_to_char( "You have uploaded the back-door trojan!\n\r", ch );
        ch->victim = ch;
        ch->c_sn = -1;
        ch->c_level = 0;
        return;
    }
    if ( level == 6 )
    {
        CHAR_DATA *bch;
        if ( ch->bvictim == NULL )
        {
            send_to_char( "Your target was shut down.\n\r", ch );
            ch->c_sn = -1;
            ch->bvictim = NULL;
            ch->c_level = 0;
            return;
        }
        chance = 10;
        if ( ch->class == CLASS_HACKER )
            chance *= 2;
        if ( number_percent() < chance )
        {
            char buf[MSL];
            if ( ( bch = get_ch(ch->bvictim->owned) ) == NULL )
            {
                send_to_char( "The mainframe was shut down.\n\r", ch );
                return;
            }
            if ( bch->pcdata->alliance == -1 )
            {
                send_to_char( "The owner of this structure is not a member of any alliance.\n\r", ch );
                return;
            }
            if ( bch->pcdata->alliance == ch->pcdata->alliance )
            {
                send_to_char( "You're already a member of that alliance.\n\r", ch );
                return;
            }
            if ( ch->pcdata->alliance != -1 )
                alliance_table[ch->pcdata->alliance].members--;
            alliance_table[bch->pcdata->alliance].members++;
            ch->pcdata->alliance = bch->pcdata->alliance;
            sprintf( buf, "@@gYou have inserted yourself into %s @@gdatabase.@@N\n\r", alliance_table[ch->pcdata->alliance].name );
            send_to_char(buf,ch);
        }
        return;
    }
    else if ( level == 7 )
    {
        OBJ_DATA *obj;
        bool found = FALSE;
        for ( obj = ch->victim->first_carry;obj;obj = obj->next_in_carry_list )
        {
            if ( obj->item_type == ITEM_COMPUTER )
            {
                found = TRUE;
                break;
            }
        }
        if ( !found )
        {
            ch->c_sn = -1;
            send_to_char( "For some reason, you are no longer carrying a computer.\n\r", ch );
            return;
        }
        if ( !ch->bvictim || ch->bvictim == NULL )
        {
            ch->c_sn = -1;
            return;
        }

        if ( ch->bvictim->value[8] != 0 )
        {
            send_to_char( "Connection Refused.\n\r", ch );
            act( "$n takes his computer and places it back in $s inventory.", ch, NULL, NULL, TO_ROOM );
            ch->c_sn = -1;
            return;
        }
        send_to_char( "Connection Established.\n\r", ch );
        act( "$n watches $s computer, grinning.", ch, NULL, NULL, TO_ROOM );
        free_string(ch->bvictim->attacker);
        ch->bvictim->attacker = str_dup(ch->name);
        ch->c_sn = -1;
        ch->c_level = 0;
        ch->position=POS_HACKING;
        ch->bvictim->value[8] = (obj->level/10) + obj->value[4];
        return;
    }
    else if ( level == 8 )
    {
        OBJ_DATA *obj;
        OBJ_DATA *comp = NULL;
        BUILDING_DATA *bld2;
        char buf[MSL];
        for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
            if ( obj->item_type == ITEM_COMPUTER )
                if ( comp == NULL || obj->value[0] > comp->value[0] )
                    comp = obj;

        if ( comp == NULL )
        {
            ch->c_sn = -1;
            send_to_char( "For some reason, you are no longer carrying a computer.\n\r", ch );
            return;
        }
        if ( comp->value[0] <= 2 )
        {
            ch->c_sn = -1;
            send_to_char( "This computer is no good for setting up dummies.\n\r", ch );
            return;
        }

        for ( bld2 = first_building;bld2;bld2 = bld2->next )
            if ( !str_cmp(bld2->owned,bld->owned) )
        {
            bld2->directories = comp->value[0];
            bld2->real_dir = number_range(1,bld2->directories);
            bld2->password = number_range(10000,99999);
        }
        sprintf( buf, "You set up %d directories, and only number %d is real!\n\r", comp->value[0], bld->real_dir );
        send_to_char( buf, ch );
        ch->c_sn = -1;
        ch->c_level = 0;
        return;
    }
    if ( level == 9 )
    {
        BUILDING_DATA *bld2;
        if ( bld->type != BUILDING_HQ )
        {
            send_to_char( "For some reason, you are no longer in a HQ!\n\r", ch );
            ch->c_sn = -1;
            return;
        }
        for ( bld2 = first_building;bld2;bld2 = bld2->next )
            if ( !str_cmp(bld2->owned,bld->owned) )
        {
            if ( IS_SET(bld2->value[1],INST_SPOOF) )
                REMOVE_BIT(bld2->value[1],INST_SPOOF);
        }
        send_to_char( "Scan complete.\n\r", ch );
        ch->c_sn = -1;
        return;
    }

    if ( bld->value[3] == 0 && level == 2 )
    {
        send_to_char( "The virus has been cleared.\n\r", ch );
        ch->c_sn = -1;
        ch->c_level = 0;
        return;
    }
    chance = 25;
    if ( ch->class == CLASS_HACKER )
        chance *= 2;
    if ( number_percent() < chance )
    {
        if ( level == 2 )
        {
            send_to_char( "You have defeated the virus!\n\r", ch );
            bld->value[3] = 0;
            bld->value[4] = 24;
            ch->c_sn = -1;
            ch->c_level = 0;
            free_string(bld->attacker);
            bld->attacker = str_dup("None");
            return;
        }
        if ( level == 1 && number_percent() < 25 )
        {
            CHAR_DATA *vch;
            send_to_char( "You got that hacker!\n\r", ch );
            ch->c_sn = -1;
            for ( vch = first_char;vch;vch = vch->next )
            {
                if ( vch->bvictim == ch->in_building )
                {
                    send_to_char( "Connection Terminated by system administrator...\n\r", vch );
                    vch->position = POS_STANDING;
                    vch->bvictim->value[8] = 0;
                    vch->c_sn = -1;
                    vch->bvictim = NULL;
                    return;
                }
            }
        }
    }
    return;
}

void do_exresearch( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Research which explosive?\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You do not carry that explosive.\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_BOMB )
    {
        send_to_char( "That is not an explosive.\n\r", ch );
        return;
    }
    ch->c_sn = gsn_exresearch;
    ch->c_time = 40 -  (ch->pcdata->skill[gsn_engineering]*3);
    ch->c_level = 0;
    if ( ( get_eq_char(ch,WEAR_HOLD_HAND_L) ) != NULL )
        unequip_char(ch,get_eq_char(ch,WEAR_HOLD_HAND_L));
    equip_char(ch,obj,WEAR_HOLD_HAND_L);
    act( "You begin researching $p.", ch, obj, NULL, TO_CHAR );
    act( "$n begins researching $p.", ch, obj, NULL, TO_ROOM );
    return;
}

void act_exresearch( CHAR_DATA *ch, int level )
{
    OBJ_DATA *obj;
    int chance;

    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) == NULL || obj->item_type != ITEM_BOMB )
    {
        send_to_char( "For some reason, you are no longer holding the explosive you were researching!\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( level == 10 )
    {
        send_to_char( "You have completed your research!\n\r", ch );
        obj->value[2] += dice(1,ch->pcdata->skill[gsn_exresearch]/2) + number_range(0, ch->pcdata->skill[gsn_engineering]);
        if ( obj->value[6] > 0 )
            obj->value[6]--;
        ch->c_sn = -1;
        ch->c_level = 0;
        return;
    }
    send_to_char( "You continue researching.\n\r", ch );
    if ( number_percent() < 5 && ch->pcdata->skill[gsn_exresearch] < 100)
    {
        send_to_char( "You have become better at Explosive Research!\n\r", ch );
        ch->pcdata->skill[gsn_exresearch]++;
    }
    chance = (ch->pcdata->skill[gsn_exresearch]/3)*2;
    chance +=  ch->pcdata->skill[gsn_engineering] * 3;
    if ( number_percent() < ((100-level)/5) + obj->level && number_percent() > chance )
    {
        send_to_char( "You mess up!\n\r", ch );
        act( "$n glitches a little.", ch, NULL, NULL, TO_ROOM );
        if ( number_percent() < 25 )
        {
            int dam = dice(obj->weight,obj->value[2]);
            if ( dam >= ch->max_hit )
                dam = ch->max_hit - 1;
            act( "$p EXPLODES!!", ch, obj, NULL, TO_CHAR );
            act( "$p EXPLODES!!", ch, obj, NULL, TO_ROOM );
            extract_obj(obj);
            ch->c_sn = -1;
            damage(ch,ch,dam,DAMAGE_BLAST);
            return;
        }
        else
            level--;
    }
    act( "$n continues researching.\n\r", ch, NULL, NULL, TO_ROOM );
    ch->c_time = 40-( ch->pcdata->skill[gsn_engineering]*3);
    ch->c_level = level + 1;
    return;
}

void do_sresearch (CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    bool found = FALSE;
    bool alien_tech = FALSE;
    int i;
    char buf[MSL];

    if ( ( bld = get_char_building(ch) ) == NULL || bld->type != BUILDING_SPACE_CENTER || str_cmp(bld->owned,ch->name) || !complete(bld) )
    {
        send_to_char( "You must be in one of your completed space centers.", ch );
        return;
    }
    if ( bld->value[1] > 100 )
        bld->value[1] = 0;

    //	if ( IS_SET(bld->value[1],INST_ALIEN_TECHNOLOGY) )
    //		alien_tech = TRUE;

    if ( argument[0] == '\0' )
    {
        char f_buf[MSL];
        char c_buf[MSL];
        sprintf( f_buf, "@@cCompleted Research:@@a\n\r" );
        sprintf( c_buf, "@@cResearchable:@@a\n\r" );
        for ( i = 0;s_res_table[i].name != NULL;i++ )
        {
            if ( s_res_table[i].name == NULL )
                break;
            if ( s_res_table[i].level > bld->level )
                continue;
            if ( IS_SET(bld->value[s_res_table[i].type],s_res_table[i].bit) )
                sprintf( f_buf+strlen(f_buf), "%s\n\r", s_res_table[i].name );
            else if ( IS_SET(bld->value[s_res_table[i].type],s_res_table[i].req) || s_res_table[i].req == 0 )
                sprintf( c_buf+strlen(c_buf), "%s  @@d(Cost: @@g%d@@d)@@a\n\r", s_res_table[i].name, s_res_table[i].cost  );
        }
        send_to_char( f_buf, ch );
        send_to_char( c_buf, ch );
        if ( bld->value[0] != -1 )
        {
            sprintf( buf, "@@cCurrently Researching: @@a%s at @@y%d%%@@N\n\r", s_res_table[bld->value[0]].name, bld->value[1] );
            send_to_char( buf, ch );
        }
        sprintf( buf, "\n\rCurrently Deposited: %d Gold (Drop gold here to deposit more)\n\r", bld->value[10] );
        if ( alien_tech )
            sprintf(buf+strlen(buf),"* The research center is equipped with alien technology upgrade.\n\r" );
        send_to_char(buf,ch);
        return;
    }
    for ( i = 0;s_res_table[i].name != NULL;i++ )
    {
        if ( s_res_table[i].name == NULL )
            break;
        if ( s_res_table[i].level > bld->level )
            continue;
        if ( !str_prefix(argument,s_res_table[i].name) )
        {
            found = TRUE;
            break;
        }
    }
    if ( !found )
    {
        send_to_char( "You do not know of such space technology.\n\r", ch );
        return;
    }
    if ( IS_SET(bld->value[s_res_table[i].type],s_res_table[i].bit) )
    {
        send_to_char( "You have already researched this technology.\n\r", ch );
        return;
    }
    if ( !IS_SET(bld->value[s_res_table[i].type],s_res_table[i].req) && s_res_table[i].req != 0 )
    {
        send_to_char( "You do not know of such space technology.\n\r", ch );
        return;
    }

    if ( bld->value[0] != -1 && bld->value[0] == i )
    {
        sprintf( buf, "You continue researching %s! (Type @@estop@@N at any time to stop)\n\r", s_res_table[i].name );
        send_to_char( buf, ch );
        act( "$n continues researching some space technology.\n\r", ch, NULL, NULL, TO_ROOM );
        ch->c_sn = gsn_sresearch;
        ch->c_time = 20-(bld->level*3);
        if ( alien_tech )
            ch->c_time /= 2;
        return;
    }
    sprintf( buf, "You begin researching %s! (Type @@estop@@N at any time to stop)\n\r", s_res_table[i].name );
    send_to_char( buf, ch );
    act( "$n begins researching some space technology.\n\r", ch, NULL, NULL, TO_ROOM );
    ch->c_sn = gsn_sresearch;
    ch->c_time = 20-(bld->level*3);
    if ( alien_tech )
        ch->c_time /= 2;
    bld->value[0] = i;
    bld->value[1] = 0;
    return;
}

void act_sresearch( CHAR_DATA *ch, int level )
{
    BUILDING_DATA *bld;
    bool alien_tech = FALSE;
    char buf[MSL];

    if ( ( bld = get_char_building(ch) ) == NULL || bld->type != BUILDING_SPACE_CENTER || str_cmp(bld->owned,ch->name) || !complete(bld) )
    {
        send_to_char( "For some reason, you are not in a space center anymore.", ch );
        ch->c_sn = -1;
        return;
    }
    if ( IS_SET(bld->value[1],INST_ALIEN_TECHNOLOGY) )
        alien_tech = TRUE;
    if ( bld->value[10] < s_res_table[bld->value[0]].cost )
    {
        send_to_char( "You must deposit more gold in here to research further.\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( bld->value[1] == 100 )
    {
        send_to_char( "You have completed your research!\n\r", ch );
        SET_BIT(bld->value[s_res_table[bld->value[0]].type], s_res_table[bld->value[0]].bit);
        bld->value[0] = -1;
        bld->value[1] = 0;
        ch->c_sn = -1;
        return;
    }
    sprintf( buf, "You continue researching %s  (%d%%)\n\r", s_res_table[bld->value[0]].name, bld->value[1] );
    send_to_char(buf,ch);
    act( "$n continues researching.\n\r", ch, NULL, NULL, TO_ROOM );
    bld->value[10] -= s_res_table[bld->value[0]].cost;
    bld->value[1]++;
    ch->c_time = 20-(bld->level*3);
    if ( alien_tech )
        ch->c_time /= 2;
    return;
}

void do_proficiencies(CHAR_DATA *ch, char *argument)
{
    char buf[MSL];
    char arg[MSL];
    int i;

    if ( argument[0] == '\0' )
    {
        do_skills(ch,"");
        sprintf( buf, "\n\rYou have %d proficiency points.\n\r", ch->pcdata->prof_points );
        send_to_char(buf, ch );
        send_to_char( "\n\rSyntax: Proficiencies Add <prof>\n\r", ch );
        send_to_char( "        Proficiencies Abilities\n\r", ch );
        return;
    }
    argument = one_argument(argument,arg);
    if ( !str_prefix(arg,"abilities") )
    {
        char buf2[MSL];
        sprintf(buf,"@@d");
        sprintf(buf2,"@@r");
        send_to_char( "\n\rName                Eng Bld Cmb  Desc\n\r----------------------------------------------------------------\n\r", ch );
        for ( i=0;ability_table[i].engineering!=-1;i++ )
        {
            if ( !has_ability(ch,i) )
                sprintf(buf+strlen(buf),"%-20s %d   %d   %d   %s\n\r", ability_table[i].name, ability_table[i].engineering,ability_table[i].building,ability_table[i].combat,ability_table[i].desc );
            else
                sprintf(buf2+strlen(buf2),"%-20s             %s\n\r", ability_table[i].name, ability_table[i].desc );
        }
        send_to_char(buf,ch);
        send_to_char(buf2,ch);
        return;
    }
    else if ( !str_prefix(arg,"add") )
    {
        if ( ch->pcdata->prof_points <= 0 )
        {
            send_to_char( "You don't have any more proficiency points!\n\r", ch );
            return;
        }
        for ( i = 0;skill_table[i].gsn != -1;i++ )
        {
            if ( !skill_table[i].prof )
                continue;
            if ( str_cmp(skill_table[i].name,argument) )
                continue;
            ch->pcdata->skill[skill_table[i].gsn]++;
            ch->pcdata->prof_points--;
            sprintf( buf, "@@WYou have increased your @@a%s@@W proficiency!@@N\n\r", skill_table[i].name );
            send_to_char(buf,ch);
            save_char_obj(ch);
            return;
        }
        send_to_char("No such proficiency.\n\r", ch );
        return;
    }
    else
    {
        send_to_char( "Invalid option. Options are:\n\rProficiencies ADD <prof>   -   Proficiencies add electronics\n\rProficiencies Abilities - List the prof. abilities\n\r", ch );
        return;
    }
    return;
}

void do_fix ( CHAR_DATA *ch, char * argument)
{
    BUILDING_DATA *bld = ch->in_building;
    if ( !bld || !complete(bld) )
    {
        send_to_char( "You can't fix anything here.\n\r", ch );
        return;
    }
    if ( !has_ability(ch,0) )
    {
        send_to_char( "You must have both your engineering and building proficiencies at level 2 to use this skill.\n\r", ch );
        return;
    }
    if ( bld->hp >= bld->maxhp )
    {
        send_to_char( "This building doesn't need fixing.\n\r", ch );
        return;
    }
    ch->c_sn = gsn_fix;
    ch->c_time = 16 - ch->pcdata->skill[gsn_engineering];
    send_to_char( "You begin fixing the building.\n\r", ch );
    act( "$n begins fixing the building.", ch, NULL, NULL, TO_ROOM );
    return;
}

void act_fix( CHAR_DATA *ch, int level )
{
    BUILDING_DATA *bld = ch->in_building;
    int x = ch->pcdata->skill[gsn_engineering]+ch->pcdata->skill[gsn_building];
    if ( !bld || !complete(bld) )
    {
        send_to_char( "You can't fix anything here.\n\r", ch );
        return;
    }
    bld->hp = URANGE(1,bld->hp+x,bld->maxhp);
    if ( bld->hp >= bld->maxhp )
    {
        ch->c_sn = -1;
        send_to_char( "You finish fixing the building.\n\r", ch );
        return;
    }
    send_to_char( "You continue working on the building.\n\r", ch );
    act( "$n continues working on the building.", ch, NULL, NULL, TO_ROOM );
    ch->c_time = 16 - ch->pcdata->skill[gsn_engineering];
    return;
}

void do_train(CHAR_DATA *ch, char *argument)
{
    int skill,cost,i,disc;
    char buf[MSL];

    if ( argument[0] == '\0' )
    {
        send_to_char( "Which skill would you like to train?\n\r", ch );
        if ( IS_NEWBIE(ch) )
            send_to_char( "TIP: Type \"Skills\" to see the list.\n\r", ch );
        return;
    }
    skill = -1;
    for ( i=0;skill_table[i].gsn != -1;i++ )
    {
        if ( skill_table[i].gsn == -1 )
            break;
        if ( !str_prefix(argument,skill_table[i].name) )
        {
            skill = i;
            break;
        }
    }
    if ( skill == -1 )
    {
        send_to_char( "There is no such skill.\n\r", ch );
        return;
    }
    if ( skill_table[skill].prof )
    {
        send_to_char( "You cannot train proficiencies with experience points.\n\r", ch );
        return;
    }
    cost = (ch->pcdata->skill[skill] +1) * 3;
    disc = get_rank(ch)/100;
    if ( disc > cost - 10 )
        disc = cost - 10;
    cost -= (cost * disc)/100;
    if ( ch->pcdata->experience < cost )
    {
        sprintf( buf, "This will require %d experience points, you only have %d.\n\r", cost, ch->pcdata->experience );
        send_to_char(buf,ch);
        return;
    }
    if ( ch->pcdata->skill[skill] >= 100 )
    {
	send_to_char( "That skill is already at 100%...\n\r", ch );
	return;
    }
    ch->pcdata->skill[skill]++;
    ch->pcdata->experience -= cost;
    save_char_obj(ch);
    sprintf( buf, "You train your %s skill.\n\r", skill_table[skill].name );
    send_to_char(buf,ch);
    return;
}

bool blind_combat_check(CHAR_DATA *ch)
{
    if ( number_percent() < ch->pcdata->skill[gsn_blind_combat] )
        return TRUE;
    //	send_to_char( "You feel around your inventory, but couldn't find it.\n\r", ch );
    return FALSE;
}
