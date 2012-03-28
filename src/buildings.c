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

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#endif
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ack.h"
#include <signal.h>

//If it's related to buildings, it's here.
void check_alien_hide args((OBJ_DATA *obj));
const int order []  =
{ 0, BUILDING_HQ, BUILDING_MINE, BUILDING_QUARRY, BUILDING_LUMBERYARD, BUILDING_TANNERY,BUILDING_FORGE, BUILDING_WAREHOUSE, BUILDING_SECURE_WAREHOUSE, BUILDING_GATHERER, BUILDING_ARMORY, BUILDING_ARMORER, BUILDING_STORAGE, BUILDING_GUNNER, BUILDING_S_TURRET, BUILDING_TURRET, BUILDING_ROCK_TOWER, BUILDING_WATCHTOWER, BUILDING_LASER_BATTERY, BUILDING_WAVE_GENERATOR, BUILDING_SURFACE_JOLTER, BUILDING_UNDERGROUND_TURRET, BUILDING_ZAP, BUILDING_DIRT_TURRET, BUILDING_WATER_PUMP, BUILDING_HYDRO_PUMP, BUILDING_LAVA_THROWER, BUILDING_FIRE_TURRET, BUILDING_FLAMESPITTER, BUILDING_ACIDBLASTER, BUILDING_ROCKETCANNON, BUILDING_SNOW_DIGGER, BUILDING_WEATHER_MACHINE, BUILDING_TRAP, BUILDING_VIRUS_ANALYZER, BUILDING_SNIPER_TOWER, BUILDING_AIRFIELD, BUILDING_PARADROP, BUILDING_TELEPORTER, BUILDING_TUNNEL,BUILDING_PROCESSING_PLANT, BUILDING_WEAPONS_LAB, BUILDING_WAR_CANNON, BUILDING_L_TURRET, BUILDING_GARAGE, BUILDING_EXPLOSIVES_SUPPLIER, BUILDING_BOOM, BUILDING_BIO_LAB, BUILDING_CHEMICAL_FACTORY, BUILDING_MEDICAL_CENTER, BUILDING_IMPLANT_RESEARCH, BUILDING_CLONING_FACILITY, BUILDING_CREEPER_COLONY, BUILDING_ACID_TURRET, BUILDING_BOT_FACTORY, BUILDING_COOKIE_FACTORY, BUILDING_SCUD_LAUNCHER, BUILDING_DOOMSDAY_DEVICE, BUILDING_TECH_LAB, BUILDING_RADAR, BUILDING_JAMMER, BUILDING_BLACKOUT, BUILDING_WARP, BUILDING_WARP_TOWER, BUILDING_PARTICLE_EMITTER,BUILDING_LASER_WORKSHOP, BUILDING_LASER_TOWER, BUILDING_SOLAR_FACILITY, BUILDING_NUKE_LAUNCHER, BUILDING_ATOM_BOMBER, BUILDING_MINING_LAB, BUILDING_REFINERY, BUILDING_MARKETPLACE, BUILDING_IMPROVED_MINE, BUILDING_SHIELD_GENERATOR, BUILDING_TRAFFIC_JAMMER, BUILDING_MAGNET_TOWER, BUILDING_COMPUTER_LAB, BUILDING_CHIP_FACTORY, BUILDING_WEB_RESEARCH, BUILDING_BATTERY, BUILDING_ONLINE_MARKET, BUILDING_TRANSMISSION_TOWER, BUILDING_HACKERS_HIDEOUT, BUILDING_PROGRAMMER_SHACK, BUILDING_HACKPORT, BUILDING_DEFENSE_LAB, BUILDING_T_TURRET, BUILDING_STUNGUN,BUILDING_EARTHQUAKER, BUILDING_POISON_TURRET, BUILDING_FLASH_TOWER, BUILDING_ARMOR_FACTORY, BUILDING_ENCRYPTION_POD,BUILDING_MISSILE_DEFENSE, BUILDING_COMM_LAB, BUILDING_SPY_TRAINING, BUILDING_SPY_QUARTERS, BUILDING_SPY_SATELLITE, BUILDING_SONIC_BLASTER, BUILDING_EMP_RESEARCH, BUILDING_PROJECTOR, BUILDING_TRANSMITTER, BUILDING_SHOCKWAVE, BUILDING_PSYCHIC_LAB, BUILDING_PSYCHOSTER, BUILDING_MIND_TOWER, BUILDING_PSYCHIC_EYES, BUILDING_PSYCHIC_RADAR, BUILDING_PSYCHIC_SHIELD, BUILDING_COOLER, BUILDING_PSYCHIC_AMPLIFIER,BUILDING_PSYCHIC_TORMENTOR, BUILDING_ALIEN_LAB, BUILDING_TRACTOR_BEAM, BUILDING_MOTHERSHIP_COMM, BUILDING_MOTHERSHIP_RESEARCH, BUILDING_INFRARED_TOWER, BUILDING_SPECIES_RESEARCH, BUILDING_ORGANIC_CHAMBER, BUILDING_ALIEN_PROBE, BUILDING_INTERGALACTIC_PUB, BUILDING_GOVERNMENT_HALL, BUILDING_BAR, BUILDING_CLUB, BUILDING_BANK, BUILDING_HUNTING_LODGE, BUILDING_ENGINEER_HOME, BUILDING_SPACE_CENTER, BUILDING_DUMMY, BUILDING_STATUE_SPELGURU, BUILDING_STATUE_CYLIS, BUILDING_STATUE_DEMISE, BUILDING_STATUE_WULFSTON, BUILDING_STATUE_SERYX, BUILDING_PORTAL,136 };

void building_update( void )
{
    int i,range,x,y,hpinc,z;
    OBJ_DATA *obj = NULL;
    OBJ_DATA *obj_next;
    BUILDING_DATA *bld;
    BUILDING_DATA *bld_next;
    CHAR_DATA *bch = NULL;
    CHAR_DATA *ch = NULL;
    bool qb = FALSE;
    char buf[MSL];
    extern int active_building_count;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    if ( first_building == NULL )
        return;

    //	building_count = 0;
    active_building_count = 0;
    //	for ( bld = first_building;bld;bld = bld_next )
    for ( bld = first_active_building;bld;bld = bld_next )
    {
        bld_next = bld->next_active;
        //		bld_next = bld->next;
        qb = FALSE;
        //		building_count++;
        active_building_count++;
        if ( bld->z == Z_PAINTBALL && bld->type == BUILDING_SNIPER_TOWER )
            continue;
        if ( number_percent() < 25 )
            bld->visible = TRUE;
        if ( bld->protection > 0 )
            bld->protection--;
        if ( bld->timer > 0 )
        {
            bld->timer--;
            if ( bld->timer == 0 )
            {
                if ( is_evil(bld) )
                {
                    extract_building(bld,TRUE);
                    continue;
                }
                activate_building(bld,FALSE);
            }
            continue;
        }
        if ( !bld->active )
            continue;
        if ( bld->x < 0 && bld->y < 0 )
            map_table.resource[bld->x* -1][bld->y * -1][bld->z] = -1;
        else
            map_table.resource[bld->x][bld->y][bld->z] = -1;
        if ( bld->value[4] > 0 )
            bld->value[4]--;
        if ( bld->value[9] > 0 )
        {
            bld->value[9]--;
            //			if ( bld->value[9] > 0 && bld->owner && has_ability(bld->owner,5) ) //nuclear containment
            //				bld->value[9]--;

        }
        if ( is_evil(bld) )
            qb = TRUE;
        z = bld->z;
        if ( !qb )
        {
            if ( bld->owner )
            {
                bch = bld->owner;
            }
            else
            {
                bch = get_ch(bld->owned);
                bld->owner = bch;
            }
            ch = bch;
            if ( bch == NULL )
            {
                if ( !is_neutral(bld->type) )
                    activate_building(bld,FALSE);
                continue;
            }
            if ( bch->in_building == bld && bch->c_sn == gsn_research )
                continue;

            if ( complete(bld) )
            {
                if ( bld->shield < bld->maxshield )
                    bld->shield+=bld->level;
                if ( bld->hp < bld->maxhp / 2 && IS_SET(bld->value[1],INST_ORGANIC_CORE))
                {
                    if ( number_percent() < 20 )
                        bld->hp++;
                }
            }

            if ( bld->x < 0 && !practicing(bch) )
                bld->x *= -1;
            else if ( bld->x < 0 && practicing(bch) )
                continue;

            if ( bld->y < 0 && !practicing(bch) )
                bld->y *= -1;
            else if ( bld->y < 0 && practicing(bch) )
                continue;

            if ( bld->value[9] > 0 )
            {
                if ( has_ability(ch,2) )                    //Nuclear containment
                {
                    bld->value[9] -= number_range(1,3);
                    if ( bld->value[9] < 0 )
                        bld->value[9] = 0;
                }
            }
            if ( bld->value[3] > 0 && ( !IS_SET(bld->value[1],INST_FIREWALL) || number_percent() < 55 ) )
            {
                bld->value[3]--;
                if ( bld->value[3] == 0 )
                {
                    bld->value[3] = -1;
                    send_to_char( "@@yOne of your buildings has been taken over by a virus!@@N\n\rDon't know what to do? See: Help Virus\n\r", bch );
                }
                if ( is_neutral(bld->type) )
                    bld->value[3] = 0;
            }
            if ( bld->value[3] < 0 )
            {
                if ( bld->type == BUILDING_VIRUS_ANALYZER && bld->value[0] == 0 && bld->attacker )
                {
                    //					char buf[MSL];
                    int chance=bld->level * 20;

                    sprintf( buf,"@@yOne of your Virus Analyzers has come up with the following report:\n\r\n\r@@cThe virus was originated by @@a%s@@c.@@N\n\r", bld->attacker );
                    if ( number_percent() < chance )
                    {
                        if ( ( ch = get_ch(bld->attacker) ) != NULL )
                        {
                            if ( ch->first_building )
                                sprintf( buf+strlen(buf), "@@cIt has been determined that they have a building in the vicinity of@@a %d/%d@@c.@@N\n\r", ch->first_building->x,ch->first_building->y);
                        }
                    }
                    else
                        sprintf( buf+strlen(buf), "@@cIt couldn't trace the location of the attacker.@@N\n\r");
                    bld->value[0] = 1;
                    send_to_char(buf,bch);
                }
                if ( IS_SET(bld->value[1],INST_ANTIVIRUS) && number_percent() < 30 )
                {
                    bld->value[3] = 0;
                    bld->value[4] = 12;
                    if ( number_percent() < 33 )
                        REMOVE_BIT(bld->value[1],INST_ANTIVIRUS);
                }
                else if ( number_percent() < 30 )
                {
                    BUILDING_DATA *bld2;
                    bool got = FALSE;
                    range = bld->level * 2;

                    y = bld->y;
                    for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    {
                        if ( INVALID_COORDS(x,y) )
                            continue;
                        bld2 = map_bld[x][y][z];
                        if ( bld2 == NULL )
                            continue;
                        if ( number_percent() < 10 - bld->value[3] && bld2->value[4] == 0 )
                        {
                            if ( bld2 && bld2->value[3] == 0 && !str_cmp(bld->owned,bld2->owned) )
                            {
                                bld2->value[3] = bld->value[3];
                                if ( bld->attacker )
                                {
                                    if ( bld2->attacker != NULL && bld->attacker )
                                        free_string(bld2->attacker);
                                    bld2->attacker = str_dup(bld->attacker);
                                }
                                send_to_char( "@@eReports indicate the virus is spreading through your network!@@N\n\r", bch );
                                got = TRUE;
                                break;
                            }
                        }
                    }
                    x = bld->x;
                    if ( !got )
                    {
                        for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                        {
                            if ( INVALID_COORDS(x,y) )
                                continue;
                            bld2 = map_bld[x][y][z];
                            if ( bld2 == NULL )
                                continue;
                            if ( number_percent() < 10 - bld->value[3] && bld2->value[4] == 0 )
                            {
                                if ( bld2 && bld2->value[3] == 0 && !str_cmp(bld->owned,bld2->owned) )
                                {
                                    bld2->value[3] = bld->value[3];
                                    if ( bld->attacker )
                                    {
                                        if ( bld2->attacker != NULL )
                                            free_string(bld2->attacker);
                                        bld2->attacker = str_dup(bld->attacker);
                                    }
                                    send_to_char( "@@eReports indicate the virus is spreading through your network!@@N\n\r", bch );
                                    got = TRUE;
                                    break;
                                }
                            }
                        }
                    }
                    if ( bld->value[3] < -1 && str_cmp(bld->attacker,"none") )
                    {
                        ch = get_ch(bld->attacker);
                        if ( ch && bld->value[3] <= -1 )
                            damage_building(ch,bld,bld->value[3]*-1);
                    }
                    continue;
                }
                else
                    continue;
            }
            if ( !complete(bld) )
                continue;

            if ( IS_SET( bch->pcdata->pflags, PFLAG_AFK ) && number_percent() < 75 )
                continue;

            if ( ( bch->security == FALSE || bld->protection > 0 ) && defense_building(bld) )
                continue;
        }
        i = 0;
        for ( obj = map_obj[bld->x][bld->y];obj;obj = obj_next )
        {
            obj_next = obj->next_in_room;
            if ( obj->z != bld->z )
                continue;
            if ( COUNTS_TOWARDS_OBJECT_LIMIT(obj) )
                i++;
            if ( obj->item_type == ITEM_MATERIAL && upgradable(bld) )
            {
                hpinc = 0;
                if ( obj->value[0] == ITEM_IRON )
                    hpinc += obj->value[1] / 3;
                else if ( obj->value[0] == ITEM_SKIN )
                    hpinc += obj->value[1] / 3;
                else if ( obj->value[0] == ITEM_COPPER )
                    hpinc += obj->value[1] / 2;
                else if ( obj->value[0] == ITEM_SILVER )
                    hpinc += obj->value[1] / 1.5;
                else if ( obj->value[0] == ITEM_GOLD )
                    hpinc += obj->value[1];
                else if ( obj->value[0] == ITEM_ROCK )
                    hpinc += obj->value[1] / 3;
                else if ( obj->value[0] == ITEM_LOG )
                    hpinc += obj->value[1] / 2;
                else if ( obj->value[0] == ITEM_STICK )
                    hpinc += obj->value[1] / 5;
                else
                    hpinc += obj->value[1] * 1.5;
                if ( bld->maxhp + hpinc <= build_table[bld->type].hp * 1.5 )
                {
                    bld->maxhp += hpinc;
                    bld->hp += hpinc;
                    extract_obj(obj);
                }
            }
        }

        obj = NULL;

        if ( !complete(bld) )
            continue;

        switch( UPPER(bld->name[0]) )
        {
            /**/        case 'A':
            if ( bld->type == BUILDING_ARMOR_FACTORY )
            {
                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj->next_in_room )
                {
                    if ( obj->z == bld->z && obj->item_type == ITEM_ARMOR && obj->level < 99 )
                    {
                        int j,am;
                        am = (obj->level < 95 ) ? 5 : 99 - obj->level;
                        for ( j=2;j<9;j++ )
                            obj->value[j] += am;
                        obj->level += am;
                        break;
                    }
                }
            }
            else if ( bld->type == BUILDING_ALIEN_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 3 )
                    continue;
                if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                }
                else if ( bld->level > 3 && number_percent() < 3 )
                {
                    if ( number_percent() < 70 )
                    {
                        if ( ( obj = create_object( get_obj_index(32605), 0 ) ) == NULL )
                            continue;
                    }
                }
                if ( obj )
                {
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
            }
            else if ( bld->type == BUILDING_ALIEN_PROBE )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour Alien Probe is ready!@@N\n\r", ch);
                    continue;
                }
                if ( bld->value[0] == 0 && number_percent() <= 5 )
                    send_to_char( "@@eA reminder-> One of your facilities has a ready alien probe.@@N\n\r", ch );
            }
            else if ( bld->type == BUILDING_ARMORER )
            {
                int vnum;
                int lev = bld->level;
                OBJ_INDEX_DATA *pObj;

                if ( i >= 20 )
                    continue;
                if ( bld->value[0] != 0 && number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(bld->value[0]), 0 ) ) == NULL )
                        continue;

                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    if ( IS_SET(bld->value[1],INST_ALIEN_HIDES) )
                        check_alien_hide(obj);
                    continue;
                }

                for ( i=1;;i++ )
                {
                    vnum = load_list[lev][i].vnum;
                    if ( vnum == -1 || vnum == 0 )
                    {
                        if ( lev > 1 )
                        {
                            i=1;
                            lev--;
                        }
                        else
                            break;
                    }

                    if ( load_list[lev][i].building != bld->type || number_percent() < 40 )
                        continue;
                    if ( number_percent() < load_list[lev][i].rarity )
                        continue;
                    if ( ( pObj = get_obj_index(vnum) ) == NULL )
                        continue;
                    if ( ( obj = create_object( get_obj_index(vnum), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    if ( IS_SET(bld->value[1],INST_ALIEN_HIDES) )
                        check_alien_hide(obj);
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    break;
                }

            }
            else if ( bld->type == BUILDING_AIRFIELD )
            {
                int j;
                if ( map_vhc[bld->x][bld->y][bld->z] )
                    map_vhc[bld->x][bld->y][bld->z]->timer = 0;
                if ( i >= 20 )
                    continue;
                i = number_range(0,9);
                j = number_range(0,3)*10;
                x = 900 + j + i;
                if ( get_obj_index(x) == NULL )
                    continue;
                if ( ( obj = create_object(get_obj_index(x),0) ) == NULL )
                    continue;
                if ( obj->value[9] == 1 || obj->value[9] == 3 )
                    continue;
                if ( number_percent() < obj->level - ( bld->level * 5 ) )
                    continue;
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_ATOM_BOMBER )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour Atom Bomber has produced a bomb!@@N\n\r", ch);
                    continue;
                }
                if ( bld->value[0] == 0 && number_percent() <= 5 )
                    send_to_char( "@@eA reminder-> One of your launchers has an atom bomb ready for use.@@N\n\r", ch );
            }
            else if ( bld->type == BUILDING_ACIDBLASTER )
            {
                range = 7;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "@@eA burst of acid fires from the acidblaster!\n\r", ch );
                    if ( number_percent() < ( 65 + (bld->level * 5) ) )
                    {
                        send_to_char( "@@eYou are hit by the burst of acid!\n\r", ch );
                        damage( bch, ch, number_fuzzy(40*(bld->level)), DAMAGE_ACID );
                    }
                    else
                        send_to_char( "@@eYou manage to dodge the acid... for now...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_ACID_TURRET )
            {
                range = 3;
                if ( IS_SET(bld->value[1],INST_LASER_AIMS) )
                    range++;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "@@eA burst of acid fires from a turret!@@n\n\r", ch );
                    if ( number_percent() < ( 55 + (bld->level * 5) ) )
                    {
                        send_to_char( "@@eYou are hit by the burst of acid!\n\r", ch );
                        damage( bch, ch, number_fuzzy(15*(bld->level*2)), DAMAGE_ACID );
                    }
                    else
                        send_to_char( "@@eYou manage to dodge the acid... for now...@@n\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_ARMORY )
            {
                int vnum;
                int lev = bld->level;
                OBJ_INDEX_DATA *pObj;

                if ( i >= 20 )
                    continue;
                if ( bld->value[0] != 0 && number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(bld->value[0]), 0 ) ) == NULL )
                        continue;

                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    continue;
                }

                for ( i=1;;i++ )
                {
                    vnum = load_list[lev][i].vnum;
                    if ( vnum == -1 || vnum == 0 )
                    {
                        if ( lev > 1 )
                        {
                            i=1;
                            lev--;
                        }
                        else
                            break;
                    }

                    if ( load_list[lev][i].building != bld->type || number_percent() < 40 )
                        continue;

                    if ( number_percent() < load_list[lev][i].rarity )
                        continue;
                    if ( ( pObj = get_obj_index(vnum) ) == NULL )
                        continue;
                    if ( pObj->item_type == ITEM_WEAPON && pObj->value[5] != 0 && pObj->value[5] != map_table.type[bld->x][bld->y][bld->z] )
                        continue;
                    if ( ( obj = create_object( get_obj_index(vnum), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                   
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    break;
                }
            }

            break;
            /**/        case 'B':
            if ( bld->type == BUILDING_BOT_FACTORY )
            {
                int vnum;
                int lev = bld->level;
                OBJ_INDEX_DATA *pObj;

                if ( i >= bld->level )
                    continue;
                if ( bld->value[0] != 0 && number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(bld->value[0]), 0 ) ) == NULL )
                        continue;

                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    continue;
                }

                for ( i=1;;i++ )
                {
                    vnum = load_list[lev][i].vnum;
                    if ( vnum == -1 || vnum == 0 )
                    {
                        if ( lev > 1 )
                        {
                            i=1;
                            lev--;
                        }
                        else
                            break;
                    }

                    if ( load_list[lev][i].building != bld->type || number_percent() < 40 )
                        continue;
                    if ( number_percent() < load_list[lev][i].rarity )
                        continue;
                    if ( ( pObj = get_obj_index(vnum) ) == NULL )
                        continue;
                    if ( ( obj = create_object( get_obj_index(vnum), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    break;
                }
            }
            else if ( bld->type == BUILDING_BLACKOUT )
            {
                range = 4;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch != ch && ( bch->pcdata->alliance == -1 || bch->pcdata->alliance != ch->pcdata->alliance || practicing(ch) ) )
                        continue;
                    if ( !IS_SET(ch->effect,EFFECT_BARIN))
                    {
                        SET_BIT(ch->effect,EFFECT_BARIN);
                    }
                    break;
                }
            }
            else if ( bld->type == BUILDING_BIO_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 4 )
                    continue;
                if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    if ( number_percent() != 1 )
                        continue;
                    if ( number_percent() < 70 )
                    {
                        if ( ( obj = create_object( get_obj_index(OBJ_VNUM_ACID_TURRET_U), 0 ) ) == NULL )
                            continue;
                    }
                    else
                    {
                        if ( ( obj = create_object( get_obj_index(OBJ_VNUM_RESOURCE_PURE), 0 ) ) == NULL )
                            continue;
                    }
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_BATTERY )
            {
                if ( number_percent() > 30 || i >= 5 )
                    continue;
                if ( ( obj = create_object( get_obj_index(32657), 0 ) ) == NULL )
                    continue;
                obj->value[0] = number_range(bld->level * 30,bld->level * 50);
                obj->value[1] = obj->value[0];
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_BAR )
            {
                CHAR_DATA *wch;
                CHAR_DATA *wch_next;
                CHAR_DATA *tar = NULL;
                int gold = 0,silv=0,p;

                ch = bch;
                if ( ch->in_building != bld || first_char->next == NULL )
                    continue;
                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj_next )
                {
                    obj_next = obj->next_in_room;
                    if ( obj->item_type != ITEM_MATERIAL || obj->x != bld->x || obj->y != bld->y || obj->z != bld->z || obj->carried_by != NULL )
                        continue;

                    if ( obj->value[0] == 3 )
                    {
                        gold += obj->value[1];
                        extract_obj(obj);
                    }
                    else if ( obj->value[0] == 4 && !IS_SET(ch->effect,EFFECT_BARIN))
                    {
                        silv += obj->value[1];
                        extract_obj(obj);
                    }
                }
                if ( silv > 0 && !IS_SET(ch->effect,EFFECT_BARIN))
                {
                    p = get_char_cost(bch);
                    if ( silv >= p )
                    {
                        SET_BIT(ch->effect,EFFECT_BARIN);
                        send_to_char( "The barkeep tells you, 'Alright, as far as the barkeepers are concerned, you no longer exist.'\n\r", bch );
                        continue;
                    }
                    else
                    {
                        if ( silv - p > 500 )
                            send_to_char( "The barkeep tells you, 'Wow, you were way off. I won't even look at you for this much silver!'\n\r", ch );
                        else if ( silv - p > 100 )
                            send_to_char( "The barkeep tells you, 'I have a wife and seventeen hitmen after you, you think this is enough?'\n\r", ch );
                        else if ( silv - p > 50 )
                            send_to_char( "The barkeep tells you, 'You could be a little more charitable next time you try to buy me off.'\n\r", ch );
                        else if ( silv - p > 0 )
                            send_to_char( "The barkeep tells you, 'Small change can really make a difference sometimes, especially in cases like this.'\n\r", ch );
                    }
                }
                if ( gold == 0 )
                    continue;
                if ( web_data.num_players <= 1 )
                    continue;
                p = number_range(1,web_data.num_players);
                wch_next = first_char;
                for ( wch = first_char;wch;wch = wch_next )
                {
                    p--;
                    if ( wch == last_char )
                        wch_next = first_char;
                    else
                        wch_next = wch->next;
                    if ( p < (-1)*web_data.num_players )
                        break;
                    if ( wch == ch || IS_IMMORTAL(wch) || !can_see(ch,wch) || IS_NEWBIE(wch) )
                        continue;
                    if ( wch->pcdata->alliance != -1 && wch->pcdata->alliance == ch->pcdata->alliance )
                        continue;
                    if ( wch->z == Z_PAINTBALL || wch->z == Z_NEWBIE || wch->fake )
                        continue;
                    if ( wch->in_building && IS_SET(wch->in_building->value[1], INST_SAFEHOUSE) )
                        continue;
                    if ( p <= 0 )
                    {
                        tar = wch;
                        break;
                    }
                }
                wch = tar;
                if ( wch == NULL || wch == ch )
                {
                    send_to_char( "The barkeep whispers to you, 'You -do- know it's illegal for me to give you the locations of other people, right? I could get arrested... And as for your gold, lets just call it a payment for not calling the cops.'\n\r", ch );
                    continue;
                }
                if ( ch->z == Z_NEWBIE )
                {
                    send_to_char( "This is where you should get your coords from the barkeep... buuuut... Since you're on the newbie planet, it's just not fair, right?\n\r", ch );
                    continue;
                }
                if ( IS_SET(wch->effect,EFFECT_BARIN) )     //|| wch->fake )
                {
                    if ( number_percent() < 25 )
                    {
                        if ( IS_SET(wch->effect,EFFECT_BARIN) )
                            REMOVE_BIT(wch->effect,EFFECT_BARIN);
                        send_to_char( "Your contact on the street tells you, 'I can't protect you from those bars anymore.', and runs away.\n\r", wch );
                    }
                    sprintf( buf, "The barkeep whispers to you, 'I just know I've had something in here about %s, but I think someone stole those papers!\n\r", wch->name );
                    send_to_char( buf, ch );
                    continue;
                }
                if ( gold < get_char_cost(wch) )
                {
                    sprintf( buf, "The barkeep whispers to you, 'You know, I had some very interesting stuff about %s, but you seem to be cheap today... Oh, you want your gold back? What gold?'\n\r", wch->name );
                    send_to_char( buf, ch );
                    continue;
                }
                if ( wch->z == Z_SPACE )
                {
                    sprintf( buf, "The barkeep whispers to you, 'Pssst, I got some info for ya. Word on the street is that %s is up there, floating in space.", wch->name );
                }
                else if ( wch->z == Z_UNDERGROUND )
                {
                    sprintf( buf, "The barkeep whispers to you, 'Pssst, I got some info for ya. Word on the street is that %s is hiding somewhere underground.", wch->name );
                }
                else
                {
                    sprintf( buf, "The barkeep whispers to you, 'Pssst, I got some info for ya. Word on the street is that %s is hanging %s on %s, near %d, %d. But you didn't hear it from me.'\n\r", wch->name, (wch->z == ch->z) ? "around here" : "over there", planet_table[wch->z].name, (wch->fake)?number_range(100,400):wch->x, (wch->fake)?number_range(100,400):wch->y );
                }
                send_to_char( buf, ch );
                if ( gold / 10 > number_percent() && number_percent() < 25 )
                {
                    send_to_char( "He adds, 'You know, I'd hate to see the same thing happen to you out there. Here, take this.', and slips you an item when nobody is watching.\n\r", ch );
                    obj = create_object(get_obj_index(OBJ_VNUM_SAFEHOUSE_INST),0);
                    obj_to_char(obj,ch);
                }
            }
            break;
            /**/        case 'C':
            if ( bld->type == BUILDING_COOLER )
            {
                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj->next_in_room )
                {
                    if ( obj->z == bld->z && obj->heat > 0)
                    {
                        obj->heat = 0;
                        break;
                    }
                }
            }
            else if ( bld->type == BUILDING_COOKIE_FACTORY )
            {
                if ( i >= 20 )
                    continue;

                if ( number_percent() < ( 10 - bld->level * 2) )
                    continue;
                if ( number_percent() < 25 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_COOKIE_LAUNCH), 0 ) ) == NULL )
                        continue;
                    obj->value[0] = number_range(1,bld->level * 7);
                }
                else
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_COOKIE_AMMO), 0 ) ) == NULL )
                        continue;
                    obj->value[1] = number_range(1,bld->level * 7);
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_CREEPER_COLONY )
            {
                if ( i >= 20 )
                    continue;

                if ( number_percent() < ( 80 - bld->level * 2) )
                    continue;
                if ( number_percent() < 25 )
                {
                    if ( ( obj = create_object( get_obj_index(32598), 0 ) ) == NULL )
                        continue;
                    obj->value[1] = number_range(1,bld->level * 5);
                }
                else
                {
                    if ( ( obj = create_object( get_obj_index(32597), 0 ) ) == NULL )
                        continue;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_COMPUTER_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 5 )
                    continue;
                if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
            }
            else if ( bld->type == BUILDING_COMM_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 3 )
                    continue;
                if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
                else if ( bld->level == 5 && number_percent() < 5 )
                {
                    if ( ( obj = create_object( get_obj_index(32666), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
                else if ( bld->level >= 4 && number_percent() < 5 )
                {
                    if ( ( obj = create_object( get_obj_index(32588), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
                else if ( number_percent() < 20 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_PROCESSOR_UP), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
            }
            else if ( bld->type == BUILDING_CHIP_FACTORY )
            {
                if ( i >= 1 )
                    continue;

                if ( number_percent() < 75 )
                    continue;
                if ( ( obj = create_object( get_obj_index(OBJ_VNUM_COMPUTER), 0 ) ) == NULL )
                    continue;

                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
                obj->value[0] = number_range(bld->level/2,bld->level*2);
                if ( bld->level == 5 )
                    obj->value[2] = 1;
                if ( IS_SET(bld->value[1],INST_PROCESSOR_UPGRADE) )
                    obj->value[4] = number_range((bld->level*5)/2,(bld->level*10)/2-10);
                obj->value[1] = 75; obj->value[8] = obj->value[1];
            }
            else if ( bld->type == BUILDING_CHEMICAL_FACTORY )
            {
                if ( i >= 20 )
                    continue;

                if ( number_percent() < ( 10 - bld->level * 2) )
                    continue;
                if ( bld->level >= 4 && number_percent() < 40 )
                {
                    if ( ( obj = create_object( get_obj_index(32675), 0 ) ) == NULL )
                        continue;
                }
                else if ( bld->level >= 2 && number_percent() < 40 )
                {
                    if ( ( obj = create_object( get_obj_index(32596), 0 ) ) == NULL )
                        continue;
                }
                else if ( bld->level >= 3 && number_percent() < 3 )
                {
                    if ( ( obj = create_object( get_obj_index(32592), 0 ) ) == NULL )
                        continue;
                }
                else if ( bld->level >= 3 && number_percent() < 10 )
                {
                    if ( ( obj = create_object( get_obj_index(927), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    i = (number_percent()<50)?OBJ_VNUM_ACID_SPRAY:1121;
                    if ( ( obj = create_object( get_obj_index(i), 0 ) ) == NULL )
                        continue;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                if ( obj->item_type == ITEM_WEAPON )
                    obj->value[0] = obj->value[1];
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
            }
            break;
            /**/        case 'D':
            if ( bld->type == BUILDING_DOOMSDAY_DEVICE )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour doomsday device is ready!@@N\n\r", ch);
                }
            }
            else if ( bld->type == BUILDING_DIRT_TURRET )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range;x++ )
                    for ( y = bld->y - range;y < bld->y + range;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;

                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) || practicing(ch) )
                        continue;
                    send_to_char( "A turret suddenly starts blasting sand and dirt around you!\n\r", ch );
                    if ( number_percent() < ( 65 + (bld->level * 5) ) )
                    {
                        if ( ch->x > BORDER_SIZE + 1 && number_percent() < 25 && !map_bld[ch->x-1][ch->y][Z_UNDERGROUND] )
                            map_table.type[ch->x-1][ch->y][Z_UNDERGROUND] = SECT_NULL;
                        if ( ch->x < (MAX_MAPS - BORDER_SIZE) - 1 && number_percent() < 25 && !map_bld[ch->x+1][ch->y][Z_UNDERGROUND] )
                            map_table.type[ch->x+1][ch->y][Z_UNDERGROUND] = SECT_NULL;
                        if ( ch->y > BORDER_SIZE + 1 && number_percent() < 25 && !map_bld[ch->x][ch->y-1][Z_UNDERGROUND] )
                            map_table.type[ch->x][ch->y-1][Z_UNDERGROUND] = SECT_NULL;
                        if ( ch->y < (MAX_MAPS - BORDER_SIZE) - 1 && number_percent() < 25 && !map_bld[ch->x][ch->y+1][Z_UNDERGROUND] )
                            map_table.type[ch->x][ch->y+1][Z_UNDERGROUND] = SECT_NULL;
                    }

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_DEFENSE_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 4 )
                    continue;
                if ( bld->level == 4 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(32671), 0 ) ) == NULL )
                        continue;
                }
                else if ( number_percent() < 3 )
                {
                    if ( ( obj = create_object( get_obj_index(32589), 0 ) ) == NULL )
                        continue;
                }
                else if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                }
                if ( !obj )
                    continue;
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            break;
            /**/        case 'E':
            if ( bld->type == BUILDING_EXPLOSIVES_SUPPLIER )
            {

                if ( i >= 20 )
                    continue;

                if ( number_percent() < ( 10 - bld->level * 2) )
                    continue;
                if ( bld->level > 3 && number_percent() < 5 )
                {
                    if ( ( obj = create_object( get_obj_index(32591), 0 ) ) == NULL )
                        continue;
                }
                else if ( number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_GRANADE), 0 ) ) == NULL )
                        continue;
                    obj->value[2] += bld->level;
                }
                else
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FLASH_GRENADE), 0 ) ) == NULL )
                        continue;
                    obj->value[2] += bld->level;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_ENGINEER_HOME )
            {
                if ( i >= 1 )
                    continue;

                if ( number_percent() < 75 )
                    continue;
                if ( ( obj = create_object( get_obj_index(OBJ_VNUM_TOOLKIT), 0 ) ) == NULL )
                    continue;

                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
                obj->level = number_range(bld->level*3,bld->level*6);
            }
            else if ( bld->type == BUILDING_ENCRYPTION_POD )
            {
                if ( bch->in_building == bld && bch->x == bld->x && bch->y == bld->y && bch->z == bld->z )
                {
                    if ( !IS_SET(ch->effect,EFFECT_ENCRYPTION) )
                    {
                        SET_BIT(ch->effect,EFFECT_ENCRYPTION);
                        send_to_char( "You are protected by an encryption barrier.\n\r", ch );
                    }
                }
            }
            else if ( bld->type == BUILDING_EARTHQUAKER )
            {
                int q;
                range = 5;
                if ( bld->z != Z_GROUND )
                    continue;

                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,0) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,5) )
                        continue;
                    send_to_char( "Dirt and rocks fall over your head as the ground shakes!\n\r", ch );
                    q = number_range(0,3);
                    if ( number_percent() < bld->level * 15 )
                        move_char(ch,q);
                    if ( number_percent() < bld->level * 15 )
                        move_char(ch,q);
                    if ( number_percent() < bld->level * 15 )
                        move_char(ch,q);
                    damage(bch,ch,bld->level * 10,-1);
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_EMP_RESEARCH )
            {
                int vnum;
                int lev = bld->level;
                OBJ_INDEX_DATA *pObj;

                if ( i >= 20 )
                    continue;
                if ( bld->value[0] != 0 && number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(bld->value[0]), 0 ) ) == NULL )
                        continue;

                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    continue;
                }

                for ( i=1;;i++ )
                {
                    vnum = load_list[lev][i].vnum;
                    if ( vnum == -1 || vnum == 0 )
                    {
                        if ( lev > 1 )
                        {
                            i=1;
                            lev--;
                        }
                        else
                            break;
                    }

                    if ( load_list[lev][i].building != bld->type || number_percent() < 40 )
                        continue;
                    if ( number_percent() < load_list[lev][i].rarity )
                        continue;
                    if ( ( pObj = get_obj_index(vnum) ) == NULL )
                        continue;
                    if ( ( obj = create_object( get_obj_index(vnum), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    break;
                }
            }
            break;
            /**/        case 'F':
            if ( bld->type == BUILDING_FIRE_TURRET )
            {
                range = 3;
                if ( IS_SET(bld->value[1],INST_LASER_AIMS) )
                    range++;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "@@eA burst of flames fires from a turret!@@n\n\r", ch );
                    if ( number_percent() < ( 55 + (bld->level * 5) ) )
                    {
                        send_to_char( "@@eYou are hit by the burst of flame!@@n\n\r", ch );
                        damage( bch, ch, number_fuzzy(20*(bld->level*2)), DAMAGE_FLAME );
                    }
                    else
                        send_to_char( "You manage to dodge the fire... for now...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_FLASH_TOWER )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "A Flash Tower begins flashing some bright red light at you!\n\r", ch );
                    if ( number_percent() < ( 55 + (bld->level * 5) ) )
                    {
                        if ( !IS_SET(ch->effect,EFFECT_BLIND) )
                        {
                            send_to_char( "You suddenly can't see!\n\r", ch );
                            SET_BIT(ch->effect,EFFECT_BLIND);
                        }
                    }
                    else
                        send_to_char( "It didn't bother you much...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_FLAMESPITTER )
            {
                range = 7;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "@@eA burst of flames fires from the flamespitter!@@n\n\r", ch );
                    if ( number_percent() < ( 65 + (bld->level * 5) ) )
                    {
                        send_to_char( "@@eYou are hit by the burst of flame!@@n\n\r", ch );
                        damage( bch, ch, number_fuzzy(40*(bld->level)), DAMAGE_FLAME );
                    }
                    else
                        send_to_char( "You manage to dodge the fire... for now...@@n\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            break;
            /**/        case 'G':
            if ( bld->type == BUILDING_GOVERNMENT_HALL && bch->pcdata->alliance != -1 )
            {
                i = 0;
                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj->next_in_room )
                    if ( obj->x == bld->x && obj->y == bld->y && obj->z == bld->z && obj->item_type == ITEM_BOARD
                    )
                {
                    i++;
                    break;
                }
                if ( i <= 0 )
                {
                    obj = create_object( get_obj_index(OBJ_VNUM_ALLI_BOARD), 0 );
                    if ( obj )
                    {
                        obj->x = bld->x;
                        obj->y = bld->y;
                        obj->z = bld->z;
                        obj->value[4] = bch->pcdata->alliance;
                        obj->value[3] = OBJ_VNUM_ALLI_BOARD + bch->pcdata->alliance;
                        obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    }
                }
            }
            else if ( bld->type == BUILDING_GUNNER )
            {
                if ( bld->value[0] > 0 )
                    bld->value[0]--;

            }
            else if ( bld->type == BUILDING_GATHERER )
            {
                BUILDING_DATA *jam;
                range = 4+bld->level;
                int sx,ex,sy,ey;
                bool steal = FALSE;

                if ( i >= bld->level * 20 )
                    continue;
                sx = bld->x - range;
                ex = bld->x + range + 1;
                sy = bld->y - range;
                ey = bld->y + range + 1;
                if ( sx < BORDER_SIZE )
                    sx = BORDER_SIZE;
                if ( sy < BORDER_SIZE )
                    sy = BORDER_SIZE;
                if ( ex > MAX_MAPS - BORDER_SIZE )
                    ex = MAX_MAPS - BORDER_SIZE;
                if ( ey > MAX_MAPS - BORDER_SIZE )
                    ey = MAX_MAPS - BORDER_SIZE;

                for ( x = sx;x < ex;x++ )
                    for ( y = sy;y < ey;y++ )
                {
                    if ( x <= 0 || y <= 0 || y >= MAX_MAPS || y >= MAX_MAPS )
                        continue;
                    jam = map_bld[x][y][bld->z];
                    if ( jam == NULL || jam == bld || !jam->active )
                        continue;
                    if ( jam->type == BUILDING_SECURE_WAREHOUSE )
                        continue;
                    steal = FALSE;
                    for ( obj = map_obj[x][y];obj;obj = obj_next )
                    {
                        obj_next = obj->next_in_room;
                        if ( obj->item_type == ITEM_MATERIAL && obj->z == bld->z )
                            move_obj(obj,bld->x,bld->y,bld->z);
                        steal = TRUE;
                    }
                    if ( steal && jam->type == BUILDING_WAREHOUSE )
                    {
                        if ( jam->owner != ch )
                        {
                            sprintf( buf, "%s's gatherer stealing from %s (%d/%d)", bld->owned, jam->owned, jam->x, jam->y );
                            log_f(buf);
                        }
                        else
                            send_to_char( "@@yWarning: One of your gatherers is stealing from your warehouse. You must move it out of the gatherer's range.@@N\n\r", ch );
                    }

                }
            }
            else if ( bld->type == BUILDING_GARAGE )
            {
                int j;
                if ( map_vhc[bld->x][bld->y][bld->z] )
                    map_vhc[bld->x][bld->y][bld->z]->timer = 0;
                if ( i >= 20 )
                    continue;
                i = number_range(0,9);
                j = number_range(0,3)*10;
                x = 900 + j + i;
                if ( get_obj_index(x) == NULL || x == 933 )
                    continue;
                if ( ( obj = create_object(get_obj_index(x),0) ) == NULL )
                    continue;
                if ( obj->value[9] == 2 || obj->value[9] == 3 )
                    continue;
                if ( number_percent() < obj->level - (bld->level * 5) )
                    continue;
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            break;
            /**/        case 'H':
            if ( bld->type == BUILDING_HQ )
            {
                i = 0;
                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj->next_in_room )
                    if ( obj->x == bld->x && obj->y == bld->y && obj->z == bld->z && obj->item_type == ITEM_BOARD )
                {
                    i++;
                    break;
                }
                if ( i <= 0 )
                {
                    obj = create_object( get_obj_index(OBJ_VNUM_MAIN_BOARD), 0 );
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                }
            }
            else if ( bld->type == BUILDING_HYDRO_PUMP )
            {
                range = 4;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "@@eA burst of water flies towards you!@@n\n\r", ch );
                    if ( number_percent() < ( 55 + (bld->level * 5) ) )
                    {
                        send_to_char( "@@eYou are hit by the burst of water!@@n\n\r", ch );
                        //					WAIT_STATE(ch,bld->level * 8);
                        set_stun(ch,bld->level * 8);
                        send_to_char( "The stream drops you to the ground!\n\r", ch );
                        damage( bch, ch, number_fuzzy(30+(bld->level*2)), -1 );
                    }
                    else
                        send_to_char( "By the time the stream reaches you, it has become too weak...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_HACKPORT )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour hackport is ready!@@N\n\r", ch);
                }
            }

            break;
            /**/        case 'I':
            if ( bld->type == BUILDING_IMPROVED_MINE )
            {
                int type;
                if ( !bch || bch->class != CLASS_MINER )
                {
                    if ( i >= 20 || number_percent() < (100 - (bld->level*25) ) )
                        continue;
                }
                else
                {
                    if ( i >= 20 || number_percent() < (75 - (bld->level*25) ) )
                        continue;
                }
                type = (number_percent() < 15) ? 2 : (number_percent() < 15) ? 3 : (number_percent() < 15) ? 4 : 0;
                if ( bld->value[0] != -1 && number_percent() < 50 )
                    type = bld->value[0];

                obj = create_material(type);
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj->value[1] = 2;
                obj->value[1] += number_range(0,bld->level*2);
                if ( bch->class == CLASS_MINER )
                    obj->value[1]+=number_range(0,1);
                if ( IS_SET(bch->effect,EFFECT_RESOURCEFUL) )
                    obj->value[1]+=number_range(0,obj->value[1]);
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
            }
            if ( bld->type == BUILDING_INTERGALACTIC_PUB )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour assassins from the Intergalactic Pub are ready!@@N\n\r", ch);
                    continue;
                }
            }
            else if ( bld->type == BUILDING_INFRARED_TOWER )
            {
                BUILDING_DATA *jam;
                range = bld->level * 2;
                int sx,ex,sy,ey;

                sx = bld->x - range;
                ex = bld->x + range + 1;
                sy = bld->y - range;
                ey = bld->y + range + 1;
                if ( sx < BORDER_SIZE )
                    sx = BORDER_SIZE;
                if ( sy < BORDER_SIZE )
                    sy = BORDER_SIZE;
                if ( ex > MAX_MAPS - BORDER_SIZE )
                    ex = MAX_MAPS - BORDER_SIZE;
                if ( ey > MAX_MAPS - BORDER_SIZE )
                    ey = MAX_MAPS - BORDER_SIZE;

                for ( x = sx;x < ex;x++ )
                    for ( y = sy;y < ey;y++ )
                {
                    if ( x < 0 || y < 0 || y >= MAX_MAPS || y >= MAX_MAPS )
                        continue;
                    jam = map_bld[x][y][z];
                    if ( jam == NULL )
                        continue;
                    if ( jam->visible == TRUE )
                        continue;
                    if ( jam->owner )
                    {
                        if ( jam->owner->pcdata->alliance != -1 && jam->owner->pcdata->alliance == bch->pcdata->alliance )
                            continue;
                        if ( jam->owner == bch )
                            continue;
                    }
                    jam->visible = TRUE;
                    damage_building(bch,jam,bld->level * 20);
                }
            }
            else if ( bld->type == BUILDING_IMPLANT_RESEARCH )
            {
                int vnum;
                int lev = bld->level;
                OBJ_INDEX_DATA *pObj;

                if ( i >= 1 )
                    continue;
                if ( bld->value[0] != 0 && number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(bld->value[0]), 0 ) ) == NULL )
                        continue;

                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    continue;
                }

                for ( i=1;;i++ )
                {
                    vnum = load_list[lev][i].vnum;
                    if ( vnum == -1 || vnum == 0 )
                    {
                        if ( lev > 1 )
                        {
                            i=1;
                            lev--;
                        }
                        else
                            break;
                    }

                    if ( load_list[lev][i].building != bld->type || number_percent() < 40 )
                        continue;
                    if ( number_percent() < load_list[lev][i].rarity )
                        continue;
                    if ( ( pObj = get_obj_index(vnum) ) == NULL )
                        continue;
                    if ( ( obj = create_object( get_obj_index(vnum), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    break;
                }
            }
            break;
            /**/        case 'J':
            if ( bld->type == BUILDING_JAMMER )
            {
                BUILDING_DATA *jam;
                range = bld->level * 2;
                int sx,ex,sy,ey;

                if ( number_percent() < 75 )
                    bld->visible = FALSE;

                sx = bld->x - range;
                ex = bld->x + range + 1;
                sy = bld->y - range;
                ey = bld->y + range + 1;
                if ( sx < BORDER_SIZE )
                    sx = BORDER_SIZE;
                if ( sy < BORDER_SIZE )
                    sy = BORDER_SIZE;
                if ( ex > MAX_MAPS - BORDER_SIZE )
                    ex = MAX_MAPS - BORDER_SIZE;
                if ( ey > MAX_MAPS - BORDER_SIZE )
                    ey = MAX_MAPS - BORDER_SIZE;

                for ( x = sx;x < ex;x++ )
                    for ( y = sy;y < ey;y++ )
                {
                    if ( x < 0 || y < 0 || y >= MAX_MAPS || y >= MAX_MAPS )
                        continue;
                    jam = map_bld[x][y][z];
                    if ( jam == NULL )
                        continue;
                    if ( GUNNER(jam) )
                        continue;
                    if ( str_cmp(jam->owned,bld->owned) || jam->x + (bld->level * 2) < bld->x || jam->y + (bld->level * 2) < bld->y || jam->x - (bld->level * 2) > bld->x || jam->y - (bld->level * 2) > bld->y )
                        continue;
                    if ( number_percent() < bld->level * 15 )
                        continue;
                    jam->visible = FALSE;
                }
            }
            break;
            /**/        case 'K':
            break;
            /**/        case 'L':
            if ( bld->type == BUILDING_LAVA_THROWER )
            {
                range = 6;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,6) )
                        continue;
                    send_to_char( "@@eA burst of burning lava fires from the lava thrower!@@n\n\r", ch );
                    if ( number_percent() < ( 65 + (bld->level * 5) ) )
                    {
                        send_to_char( "@@eYou are hit by the lava!@@n\n\r", ch );
                        damage( bch, ch, number_fuzzy(40*(bld->level)), DAMAGE_FLAME );
                    }
                    else
                        send_to_char( "You manage to dodge the lava... for now...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_LASER_BATTERY )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,Z_AIR) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "You have been marked by a laser-guided missile!\n\r", ch );
                    if ( number_percent() < ( 65 + (bld->level * 5) ) )
                    {
                        send_to_char( "You are hit by the missile!\n\r", ch );
                        damage( bch, ch, number_fuzzy(100*(bld->level)), DAMAGE_LASER );
                    }
                    else
                        send_to_char( "You manage to evade the missile...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_LUMBERYARD )
            {
                int type;
                if ( i >= 20 || number_percent() < (100 - (bld->level*25) ) )
                    continue;

                type = (number_percent() < 30 ) ? ITEM_STICK : ITEM_LOG;
                if ( (bld->value[0] == 7 || bld->value[0] == 6) && number_percent() < 50 )
                    type = bld->value[0];
                obj = create_material(type);
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                if ( map_table.type[bld->x][bld->y][bld->z] == SECT_FOREST && type == ITEM_LOG )
                    obj->value[1] *= 2;
                if ( IS_NEWBIE(bch) ) obj->value[1]++;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
            }
            else if ( bld->type == BUILDING_LASER_WORKSHOP )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 3 )
                    continue;
                else
                {
                    if ( number_percent() > 2 )
                        continue;
                    if ( number_percent() < 50 && bld->level > 3 )
                    {
                        if ( ( obj = create_object( get_obj_index(32674), 0 ) ) == NULL )
                            continue;
                    }
                    else
                    {
                        if ( ( obj = create_object( get_obj_index(OBJ_VNUM_LASER_TOWER_U), 0 ) ) == NULL )
                            continue;
                    }
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_LASER_TOWER )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_AIR) ) == NULL )
                            continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,5) )
                        continue;
                    send_to_char( "A barrage of lasers starts flashing towards you!\n\r", ch );
                    if ( number_percent() < ( 50 + (bld->level * 2) ) )
                    {
                        int dam = number_fuzzy(60+bld->level*2);
                        send_to_char( "You are hit by the lasers!\n\r", ch );
                        if ( number_percent() < ( 80 + (bld->level * 2) ) )
                        {
                            dam += number_fuzzy(30+bld->level*2);
                            send_to_char( "You are hit by the lasers!\n\r", ch );
                        }
                        damage( bch, ch, dam, DAMAGE_LASER );
                    }
                    else
                        send_to_char( "You manage to dodge the lasers... for now...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_L_TURRET )
            {
                range = 4;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_AIR) ) == NULL )
                            continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    if ( map_table.type[ch->x][ch->y][ch->z] == SECT_FOREST && !IS_SET(bld->value[1],INST_GPS) )
                        continue;
                    send_to_char( "A turret fires at you!\n\r", ch );
                    if ( number_percent() < ( 65 + (bld->level * 5) ) )
                    {
                        int dam = number_fuzzy(80);
                        send_to_char( "You are hit by the turret!\n\r", ch );
                        if ( number_percent() < ( 55 - (bld->level * 5) ) )
                        {
                            dam += number_fuzzy(60);
                            send_to_char( "You are hit by the turret!\n\r", ch );
                        }
                        damage( bch, ch, dam, DAMAGE_BLAST );
                        if ( IS_SET(bld->value[1],INST_ACID_DEFENSE) && ch && ch->position != POS_DEAD )
                        {
                            send_to_char( "The bullets were covered in acid!\n\r", ch );
                            damage(bch,ch,bld->level*10,DAMAGE_ACID);
                        }
                    }
                    else
                        send_to_char( "You manage to dodge the bullets... for now...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            break;
            /**/        case 'M':
            if ( bld->type == BUILDING_MINING_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 4 )
                    continue;
                if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(32672), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    if ( number_percent() != 1 )
                        continue;
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_MOTHERSHIP_RESEARCH )
            {

                if ( i >= 5 )
                    continue;

                if ( number_percent() < ( 50 - bld->level * 2) )
                    continue;
                if ( number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(32586), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    if ( ( obj = create_object( get_obj_index(32587), 0 ) ) == NULL )
                        continue;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_MIND_TOWER )
            {
                int q;
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_AIR) ) == NULL )
                            continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,5) )
                        continue;
                    q = number_range(0,3);
                    if ( number_percent() < bld->level * 15 )
                        move_char(ch,q);
                    if ( number_percent() < bld->level * 15 )
                        move_char(ch,q);
                    if ( number_percent() < bld->level * 15 )
                        move_char(ch,q);
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_MINE )
            {
                int type;

                if ( !bch || bch->class != CLASS_MINER )
                {
                    if ( i >= 20 || number_percent() < (100 - (bld->level*25) ) )
                        continue;
                }
                else
                {
                    if ( i >= 20 || number_percent() < (75 - (bld->level*25) ) )
                        continue;
                }
                if ( bld->value[0] < 0 || bld->value[0] > 10 )
                    bld->value[0] = 0;

                type = (number_percent() < 15) ? 2 : (number_percent() < 15) ? 3 : (number_percent() < 25) ? 4 : 0;
                if ( bld->value[0] != -1 && number_percent() < 50 )
                    type = bld->value[0];

                obj = create_material(type);
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                if ( bch->class == CLASS_MINER )
                    obj->value[1]+=number_range(0,1);
                if ( IS_SET(bch->effect,EFFECT_RESOURCEFUL) )
                    obj->value[1]+=number_range(0,obj->value[1]);
                if ( IS_NEWBIE(bch) ) obj->value[1]++;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
            }
            else if ( bld->type == BUILDING_MEDICAL_CENTER )
            {
                int vnum;
                int lev = bld->level;
                OBJ_INDEX_DATA *pObj;

                if ( i >= 5 )
                    continue;
                if ( bld->value[0] != 0 && number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(bld->value[0]), 0 ) ) == NULL )
                        continue;

                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    continue;
                }

                for ( i=1;;i++ )
                {
                    vnum = load_list[lev][i].vnum;
                    if ( vnum == -1 || vnum == 0 )
                    {
                        if ( lev > 1 )
                        {
                            i=1;
                            lev--;
                        }
                        else
                            break;
                    }

                    if ( load_list[lev][i].building != bld->type || number_percent() < 40 )
                        continue;
                    if ( number_percent() < load_list[lev][i].rarity )
                        continue;
                    if ( ( pObj = get_obj_index(vnum) ) == NULL )
                        continue;
                    if ( ( obj = create_object( get_obj_index(vnum), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    break;
                }
            }
            else if ( bld->type == BUILDING_MAGNET_TOWER )
            {
                range = 7;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,9) )
                        continue;
                    send_to_char( "A strange magnetic wave enfoulges you!\n\r", ch );
                    if ( number_percent() < ( 20 * (bld->level) ) )
                    {
                        OBJ_DATA *obj;

                        if ( ( obj = ch->first_carry ) == NULL )
                        {
                            break;
                        }
                        while ( obj && ( IS_SET(obj->extra_flags,ITEM_NODROP) || IS_SET(obj->extra_flags,ITEM_STICKY) ) )
                            obj = obj->next_in_carry_list;

                        if ( !obj )
                            break;
                        set_fighting(bch,ch);
                        act( "You discover $p floating towards the magnet tower!", ch, obj, NULL, TO_CHAR );
                        act( "$n discovers $p floating towards the magnet tower!", ch, obj, NULL, TO_ROOM );
                        obj_from_char(obj);
                        obj->x = bld->x;
                        obj->y = bld->y;
                        obj->z = bld->z;
                        free_string(obj->owner);
                        obj->owner = str_dup(bld->owned);
                        obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
                    }
                    else
                        send_to_char( "Nothing seems to have happened... yet...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            break;
            /**/        case 'N':
            if ( bld->type == BUILDING_NUKE_LAUNCHER )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour Nuke Launcher has produced a Nuke!@@N\n\r", ch);
                    continue;
                }
                if ( bld->value[0] == 0 && number_percent() <= 5 )
                    send_to_char( "@@eA reminder-> One of your launchers has a Nuke ready for use.@@N\n\r", ch );
            }
            break;
            /**/        case 'O':
            break;
            /**/        case 'P':
            if ( bld->type == BUILDING_PROCESSING_PLANT )
            {

                if ( i >= 5 )
                    continue;

                if ( number_percent() < ( 10 - bld->level * 2) )
                    continue;
                if ( ( obj = create_object( get_obj_index(OBJ_VNUM_CONTAINER), 0 ) ) == NULL )
                    continue;
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_PSYCHIC_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 5 )
                    continue;
                if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
                else if ( bld->level >= 3 && number_percent() < 3 )
                {
                    if ( ( obj = create_object( get_obj_index(32590), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
                else
                {
                    if ( bld->level >= 3 && number_percent() < 10 )
                    {
                        if ( ( obj = create_object( get_obj_index(OBJ_VNUM_SUIT_JUMP), 0 ) ) == NULL )
                            continue;
                        obj->x = bld->x;
                        obj->y = bld->y;
                        obj->z = bld->z;
                        obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                        free_string(obj->owner);
                        obj->owner = str_dup(bld->owned);
                    }
                }

            }
            else if ( bld->type == BUILDING_PSYCHIC_SHIELD )
            {
                if ( i >= 1 )
                    continue;

                if ( number_percent() == 1 )
                {
                    if ( ( obj = create_object( get_obj_index(32659), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    if ( bld->level < 4 || number_percent() != 1 )
                        continue;
                    if ( ( obj = create_object( get_obj_index(32670), 0 ) ) == NULL )
                        continue;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);

            }
            else if ( bld->type == BUILDING_PSYCHIC_TORMENTOR )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour Psychic Tormentor is ready!@@N\n\r", ch);
                    continue;
                }
            }
            else if ( bld->type == BUILDING_PROGRAMMER_SHACK )
            {
                if ( i >= 5 )
                    continue;

                if ( number_percent() < 50 )
                    continue;
                if ( number_percent() < 20 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_DISK_V), 0 ) ) == NULL )
                        continue;
                }
                else if ( number_percent() < 25 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_DISK_C), 0 ) ) == NULL )
                        continue;
                }
                else if ( number_percent() < 33 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_DISK_S), 0 ) ) == NULL )
                        continue;
                }
                else if ( number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_DISK_P), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_DISK_F), 0 ) ) == NULL )
                        continue;
                }

                obj->value[1] = number_range((bld->level-1)*10,bld->level * 10);
                if ( obj->value[1] == 0 )
                    obj->value[1] = 1;
                sprintf( buf, obj->short_descr, obj->value[1]/10,obj->value[1]%10 );
                free_string(obj->short_descr);
                obj->short_descr = str_dup(buf);
                sprintf( buf, obj->description, obj->value[1]/10,obj->value[1]%10 );
                free_string(obj->description);
                obj->description = str_dup(buf);
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_PSYCHOSTER )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "Waves of horrible images flow through your head!\n\r", ch );
                    damage( bch, ch, number_fuzzy(30+(bld->level*2)), DAMAGE_PSYCHIC );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_PSYCHIC_RADAR )
            {
                range = 30;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL || ch == bch || IS_IMMORTAL(ch) )
                        if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_AIR) ) == NULL || ch == bch || IS_IMMORTAL(ch) )
                            if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_UNDERGROUND) ) == NULL || ch == bch || IS_IMMORTAL(ch) )
                                continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    sprintf( buf, "%s has been detected nearby at %d/%d!\n\r", ch->name, ch->x, ch->y );
                    send_to_char(buf,bch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_PARTICLE_EMITTER )
            {
                range = 4;
                OBJ_DATA *obj2;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( x <= 1 || x >=498 || y <= 1 || y >= 498 )
                        continue;
                    for ( obj2 = map_obj[x][y];obj2;obj2 = obj2->next_in_room )
                    {
                        if ( !obj2 )
                            continue;
                        if ( obj2->z != bld->z )
                            continue;
                        if ( obj2->item_type != ITEM_BOMB || obj2->value[1] <= 0 )
                            continue;
                        obj2->value[1] = 0;
                    }
                }
            }
            else if ( bld->type == BUILDING_POISON_TURRET )
            {
                range = 6;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,6) )
                        continue;
                    send_to_char( "A turret fires at you!\n\r", ch );
                    if ( number_percent() < ( 75 + (bld->level * 5) ) )
                    {
                        send_to_char( "You are hit by some strange poisoned bullet!\n\r", ch );
                        damage( bch, ch, number_fuzzy(8*(bld->level*2)), DAMAGE_ACID );
                        if ( !IS_SET(ch->effect,EFFECT_SLOW) )
                        {
                            SET_BIT(ch->effect,EFFECT_SLOW);
                            send_to_char("You feel sluggish!\n\r", ch );
                        }
                    }
                    else
                        send_to_char( "You manage to dodge the bullets... for now...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_PROJECTOR )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,5) )
                        continue;
                    if ( number_percent() < ( 75 + (bld->level * 5) ) )
                        if ( !IS_SET(ch->effect,EFFECT_VISION) )
                            SET_BIT(ch->effect,EFFECT_VISION);

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_PSYCHIC_AMPLIFIER )
            {
                int vnum;
                int lev = bld->level;
                OBJ_INDEX_DATA *pObj;

                if ( i >= 5 )
                    continue;
                if  ( lev < 4 )
                    continue;

                for ( i=1;;i++ )
                {
                    vnum = load_list[lev][i].vnum;
                    if ( vnum == -1 || vnum == 0 )
                    {
                        if ( lev > 4 )
                        {
                            i=1;
                            lev--;
                        }
                        else
                            break;
                    }

                    if ( load_list[lev][i].building != bld->type || number_percent() < 40 )
                        continue;
                    if ( number_percent() < load_list[lev][i].rarity )
                        continue;
                    if ( ( pObj = get_obj_index(vnum) ) == NULL )
                        continue;
                    if ( ( obj = create_object( get_obj_index(vnum), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    break;
                }
            }

            break;
            /**/        case 'Q':
            if ( bld->type == BUILDING_QUARRY )
            {
                if ( !bch || bch->class != CLASS_MINER )
                {
                    if ( i >= 20 || number_percent() < (100 - (bld->level*25) ) )
                        continue;
                }
                else
                {
                    if ( i >= 20 || number_percent() < (75 - (bld->level*25) ) )
                        continue;
                }

                obj = create_object( get_obj_index( OBJ_VNUM_MATERIAL ), 0 );
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj->value[0] = 5;
                free_string( obj->name );
                obj->name = str_dup("Resource Rock");
                free_string( obj->short_descr );
                free_string( obj->description );
                obj->short_descr = str_dup("@@gA large @@drock@@N");
                obj->description = str_dup("@@gA large @@drock@@N");
                if ( bch->class == CLASS_MINER )
                    obj->value[1]+=number_range(0,1);
                if ( IS_SET(bch->effect,EFFECT_RESOURCEFUL) )
                    obj->value[1]+=number_range(0,obj->value[1]);
                if ( map_table.type[bld->x][bld->y][bld->z] == SECT_ROCK )
                    obj->value[1] *= 1.5;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
            }
            break;
            /**/        case 'R':
                                                            if (bld->type==136) //rec hall
                                                            {
                                                                                if (i>=20) continue;
                 if (number_percent()<20)
                 {
                                       if (number_percent()<80)
                                       {
 obj = create_object( get_obj_index(1500),0); //beer
send_to_loc("Another beer rolls out of a processing machine and bounces over the floor.\n\r",bld->x,bld->y,bld->z);
}
 else
 {
 obj = create_object( get_obj_index(1501),0); //damn cubans
 send_to_loc("A cuban cigar, fresh from the nation hated by so many, plops from an open box and lands on the floor.\n\r",bld->x,bld->y,bld->z);
}
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    }
}
else             if ( bld->type == BUILDING_ROCK_TOWER )
            {
                range = 6;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,6) )
                        continue;
                    if ( number_percent() < ( 40 + (bld->level * 2) ) )
                    {
                        obj = create_object( get_obj_index( OBJ_VNUM_MATERIAL ), 0 );
                        obj->x = bld->x;
                        obj->y = bld->y;
                        obj->value[0] = 5;
                        obj->weight = 20 * bld->level;
                        free_string( obj->name );
                        obj->name = str_dup("Resource Rock");
                        free_string( obj->short_descr );
                        free_string( obj->description );
                        obj->short_descr = str_dup("@@gA giant, heavy @@drock@@N");
                        obj->description = str_dup("@@gA giant, heavy @@drock@@N");
                        obj_to_char(obj,ch);
                        act( "$p is thrown at you! You catch it!", ch, obj, NULL, TO_CHAR );
                        act( "$p is thrown at $n! $e catches it!", ch, obj, NULL, TO_ROOM );
                        damage(bch,ch,number_range(obj->weight/2,obj->weight),-1);
                    }
                    else
                    {
                        send_to_char( "Giant rocks roll around the room!\r\n", ch );
                        send_warning(bch,bld,ch);
                    }
                }
                continue;
            }
            else if ( bld->type == BUILDING_ROCKETCANNON )
            {
                int rockets = number_range(1,bld->level),j,hit=0,dam=0;
                range = 7;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    if ( rockets==1)
                        sprintf(buf,"You spot a rocket heading your way!\n\r");
                    else
                        sprintf( buf, "You spot %d rockets heading your way!\n\r", rockets );
                    send_to_char( buf, ch );
                    for ( j=0;j<rockets;j++ )
                        if ( number_percent() < 70 )
                    {
                        hit++; dam += number_range(20,30);
                    }
                    if ( hit == 0 )
                        send_to_char( "You didn't get hit!\n\r", ch );
                    else if ( hit == 1 )
                        sprintf( buf, "You are hit by one of the rockets! " );
                    else
                        sprintf( buf, "You are hit by %d rockets! ", hit );

                    if ( dam > 0 )
                        damage(bch,ch,dam,DAMAGE_BLAST);
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            break;
            /**/        case 'S':
            if ( bld->type == BUILDING_SPY_TRAINING )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 4 || number_percent() > 2 )
                    continue;
                if ( ( obj = create_object( get_obj_index(32673), 0 ) ) == NULL )
                    continue;
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);

            }
            else if ( bld->type == BUILDING_SPECIES_RESEARCH )
            {
                if ( i >= 1 )
                    continue;

                if ( number_percent() < ( 50 - bld->level * 2) )
                    continue;
                if ( number_percent() < 60 )
                {
                    if ( ( obj = create_object( get_obj_index(32585), 0 ) ) == NULL )
                        continue;
                }
                else if ( bld->level >= 3 )
                {
                    if ( ( obj = create_object( get_obj_index(32584), 0 ) ) == NULL )
                        continue;
                }

                if ( obj )
                {
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                }
            }
            else if ( bld->type == BUILDING_STATUE_SPELGURU || bld->type == BUILDING_STATUE_CYLIS || bld->type == BUILDING_STATUE_DEMISE || bld->type == BUILDING_STATUE_WULFSTON )
            {
                range = 4;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch != ch && ( bch->pcdata->alliance == -1 || bch->pcdata->alliance != ch->pcdata->alliance ) )
                        continue;
                    if ( bld->type == BUILDING_STATUE_SPELGURU && !IS_SET(ch->effect,EFFECT_RESOURCEFUL))
                    {
                        send_to_char( "You feel more resourceful.\n\r", ch );
                        SET_BIT(ch->effect,EFFECT_RESOURCEFUL);
                    }
                    else if ( bld->type == BUILDING_STATUE_CYLIS && !IS_SET(ch->effect,EFFECT_BOMBER))
                    {
                        send_to_char( "You feel angry, explosive!\n\r", ch );
                        SET_BIT(ch->effect,EFFECT_BOMBER);
                    }
                    else if ( bld->type == BUILDING_STATUE_DEMISE && !IS_SET(ch->effect,EFFECT_POSTAL))
                    {
                        send_to_char( "You go POSTAL!\n\r", ch );
                        SET_BIT(ch->effect,EFFECT_POSTAL);
                    }
                    else if ( bld->type == BUILDING_STATUE_WULFSTON && !IS_SET(ch->effect,EFFECT_WULFSKIN))
                    {
                        send_to_char( "You howl with rage as your skin changes!\n\r", ch );
                        SET_BIT(ch->effect,EFFECT_WULFSKIN);
                    }
                    break;
                }
            }
            else if ( bld->type == BUILDING_STATUE_SERYX )
            {
                range = 4;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( x < BORDER_SIZE || y < BORDER_SIZE || x >= MAX_MAPS - BORDER_SIZE || y >= MAX_MAPS - BORDER_SIZE )
                        continue;
                    if ( map_table.type[x][y][bld->z] == SECT_MAGMA || map_table.type[x][y][bld->z] == SECT_NULL )
                        map_table.type[x][y][bld->z] = SECT_UNDERGROUND;
                    if ( map_table.type[x][y][bld->z] == SECT_SNOW_BLIZZARD )
                        map_table.type[x][y][bld->z] = SECT_SNOW;
                }
            }
            else if ( bld->type == BUILDING_SOLAR_FACILITY )
            {
                if ( i >= 20 )
                    continue;
                if ( number_percent() < 50 )
                    continue;
                if ( bld->level < 3 )
                {
                    if ( ( obj = create_object( get_obj_index(1042), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    if ( ( obj = create_object( get_obj_index(1043), 0 ) ) == NULL )
                        continue;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_SPACE_CENTER )
            {
                int gold = 0;
                if ( map_vhc[bld->x][bld->y][bld->z] )
                    map_vhc[bld->x][bld->y][bld->z]->timer = 0;

                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj_next )
                {
                    obj_next = obj->next_in_room;
                    if ( obj->item_type != ITEM_MATERIAL || obj->x != bld->x || obj->y != bld->y || obj->z != bld->z || obj->carried_by != NULL || obj->value[0] != 3 )
                        continue;

                    gold += obj->value[1];
                    extract_obj(obj);
                }
                if ( gold > 0 )
                    bld->value[10] += gold;
            }
            else if ( bld->type == BUILDING_SHIELD_GENERATOR )
            {
                BUILDING_DATA *jam;
                int s,sx,sy,ex,ey;

                if ( bld->shield <= 0 )
                    continue;

                range = bld->level * 2;
                sx = bld->x - range;
                ex = bld->x + range;
                sy = bld->y - range;
                ey = bld->y + range;
                if ( sx < BORDER_SIZE )
                    sx = BORDER_SIZE;
                if ( sy < BORDER_SIZE )
                    sy = BORDER_SIZE;
                if ( ex > MAX_MAPS - BORDER_SIZE )
                    ex = MAX_MAPS - BORDER_SIZE;
                if ( ey > MAX_MAPS - BORDER_SIZE )
                    ey = MAX_MAPS - BORDER_SIZE;

                for ( x = sx;x <= ex;x++ )
                    for ( y = sy;y <= ey;y++ )
                {
                    if ( x <= 2 || y <= 2 || y >= MAX_MAPS-2 || y >= MAX_MAPS-2 )
                        continue;
                    jam = map_bld[x][y][z];

                    if ( jam == NULL || !map_bld[x][y][z] )
                        continue;
                    if ( jam == bld )
                        continue;
                    if ( bch )
                    {
                        if ( jam->owner )
                        {
                            if ( jam->owner != bch )
                                continue;
                        }
                    }
                    else
                    {
                        if ( str_cmp(jam->owned,bld->owned) )
                            continue;
                    }
                    if ( !complete(jam) )
                        continue;
                    if ( jam->shield >= jam->maxshield )
                        continue;
                    if ( jam->value[9] > 0 )
                        continue;
                    s = bld->shield;
                    if ( jam->maxshield - jam->shield < s )
                        s = jam->maxshield - jam->shield;
                    bld->shield -= s;
                    jam->shield += s;
                }
            }
            else if ( bld->type == BUILDING_SPY_SATELLITE || bld->type == BUILDING_SPY_QUARTERS || bld->type == BUILDING_SHOCKWAVE )
            {
                if ( bch && bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                    bld->value[0]--;
            }
            else if ( bld->type == BUILDING_SCUD_LAUNCHER )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour SCUD Launcher has produced a SCUD!@@N\n\r", ch);
                    continue;
                }
                if ( bld->value[0] == 0 && number_percent() <= 5 )
                    send_to_char( "@@eA reminder-> One of your launchers has a SCUD ready for use.@@N\n\r", ch );
            }
            else if ( bld->type == BUILDING_STUNGUN )
            {
                range = 6;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "A Stungun fires at you!\n\r", ch );
                    if ( number_percent() < ( 75 + (bld->level * 5) ) )
                    {
                        send_to_char( "You are SHOCKED by the stungun!\n\r", ch );
                        set_stun(ch,13);
                        damage(bch, ch, (number_range(1, 5)), DAMAGE_ENVIRO);
                    }
                    else
                        send_to_char( "You ignore the shock.\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_S_TURRET )
            {
                range = 3;
                if ( IS_SET(bld->value[1],INST_LASER_AIMS) )
                    range++;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    if ( map_table.type[ch->x][ch->y][bld->z] == SECT_FOREST && !IS_SET(bld->value[1],INST_GPS) )
                        continue;
                    send_to_char( "A turret fires at you!\n\r", ch );
                    if ( number_percent() < ( 75 + (bld->level * 5) ) )
                    {
                        send_to_char( "You are hit by the turret!\n\r", ch );
                        damage( bch, ch, number_fuzzy(20+(bld->level*2)), DAMAGE_BULLETS );
                        if ( IS_SET(bld->value[1],INST_ACID_DEFENSE) && ch && ch->position != POS_DEAD )
                        {
                            send_to_char( "The bullets were covered in acid!\n\r", ch );
                            damage(bch,ch,bld->level*10,DAMAGE_ACID);
                        }
                    }
                    else
                        send_to_char( "You manage to dodge the bullets... for now...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_SURFACE_JOLTER )
            {
                range = 5;
                if ( bld->z != Z_GROUND )
                    continue;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,0) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,5) )
                        continue;
                    send_to_char( "A wave of electricity flows through the tunnels!\n\r", ch );
                    send_to_char( "You are zapped!\n\r", ch );
                    damage( bch, ch, number_fuzzy(20+(bld->level*20)), DAMAGE_LASER );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_SNOW_DIGGER )
            {
                range = 7;
                if ( bld->z != Z_GROUND )
                    continue;

                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,1) ) == NULL )
                        if ( ( ch = get_rand_char(x,y,0) ) == NULL )
                            continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    if ( ch->z == 1 )
                        send_to_char( "The ground begins to crack!\n\r", ch );
                    else if ( ch->z == 0 )
                        send_to_char( "The gound above you begins to crack!\n\r", ch );
                    if ( number_percent() < ( 55 + (bld->level*7) ) && ( !ch->in_vehicle || !AIR_VEHICLE(ch->in_vehicle->type)) && ch->z == 1 )
                    {
                        int x,y;
                        set_fighting(bch,ch);
                        act( "The ground breaks, and you fall downwards!", ch, NULL, NULL, TO_CHAR );
                        act( "The ground breaks, and $n falls downwards!", ch, NULL, NULL, TO_ROOM );
                        x = URANGE(5,number_range(ch->x-(bld->level*3),ch->x+(bld->level*3)),MAX_MAPS-5);
                        y = URANGE(5,number_range(ch->y-(bld->level*3),ch->y+(bld->level*3)),MAX_MAPS-5);
                        if ( x <= 4 )
                            x = 4;
                        if ( y <= 4 )
                            y = 4;
                        if ( x >= MAX_MAPS-BORDER_SIZE )
                            x = MAX_MAPS-BORDER_SIZE;
                        if ( y >= MAX_MAPS-BORDER_SIZE )
                            y = MAX_MAPS-BORDER_SIZE;
                        while (map_table.type[x][y][Z_UNDERGROUND] == SECT_MAGMA)
                        {
                            if ( x >= MAX_MAPS-BORDER_SIZE && y >= MAX_MAPS-BORDER_SIZE )
                                break;
                            if ( (number_percent() < 50 || y >= MAX_MAPS-BORDER_SIZE) && x < MAX_MAPS-BORDER_SIZE )
                            {
                                x++;
                            }
                            else
                            {
                                if ( y < MAX_MAPS-BORDER_SIZE )
                                    y++;
                            }
                        }
                        move(ch,x,y,Z_UNDERGROUND);
                        act( "$n falls from above!", ch, NULL, NULL, TO_ROOM );
                        do_look(ch,"");
                    }
                    else if ( ch->z == Z_UNDERGROUND && number_percent() < 55 + (bld->level*7) )
                    {
                        send_to_char( "The ground from above collapses over your head!\n\r", ch );
                        damage(bch,ch,bld->level * 20,-1);
                    }
                    else
                        send_to_char( "Nothing seems to have happened... yet...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_STORAGE )
            {
                int vnum;
                int lev = bld->level;
                OBJ_INDEX_DATA *pObj;

                if ( i >= 20 )
                    continue;
                if ( bld->value[0] != 0 && number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(bld->value[0]), 0 ) ) == NULL )
                        continue;

                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    continue;
                }

                for ( i=1;;i++ )
                {
                    vnum = load_list[lev][i].vnum;
                    if ( vnum == -1 || vnum == 0 )
                    {
                        if ( lev > 1 )
                        {
                            i=1;
                            lev--;
                        }
                        else
                            break;
                    }

                    if ( load_list[lev][i].building != bld->type || number_percent() < 40 )
                        continue;
                    if ( number_percent() < load_list[lev][i].rarity )
                        continue;
                    if ( ( pObj = get_obj_index(vnum) ) == NULL )
                        continue;
                    if ( ( obj = create_object( get_obj_index(vnum), 0 ) ) == NULL )
                        continue;
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                    break;
                }

            }

            break;
            /**/        case 'T':
            if ( bld->type == BUILDING_TANNERY )
            {
                if ( i >= 20 || number_percent() < (100 - (bld->level*25) ) )
                    continue;

                obj = create_material(ITEM_SKIN);
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj->value[1] = 1 + number_range(0,bld->level - 1);
                obj->weight = 1;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
            }
            else if ( bld->type == BUILDING_TRANSMITTER )
            {
                if ( bch->timer > 5 )
                    continue;
                if ( bld->value[0] > 0 )
                {
                    bld->value[0]--;
                    if ( bld->value[0] == 0 )
                        send_to_char( "@@eYour Transmitter is ready!@@N\n\r", ch);
                    continue;
                }
            }
            else if ( bld->type == BUILDING_TECH_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 3 )
                    continue;
                if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                }
                else if ( bld->level > 3 && number_percent() < 3 )
                {
                    if ( number_percent() < 70 )
                    {
                        if ( ( obj = create_object( get_obj_index(OBJ_VNUM_REFLECTOR), 0 ) ) == NULL )
                            continue;
                    }
                }
                else if ( number_percent() < 10 )
                {
                    if ( ( obj = create_object( get_obj_index(32607), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    if ( number_percent() < 10 )
                        if ( ( obj = create_object( get_obj_index(32593), 0 ) ) == NULL )
                            continue;
                    if ( !obj && number_percent() < 20 )
                        if ( ( obj = create_object( get_obj_index(OBJ_VNUM_STUN_GUN), 0 ) ) == NULL )
                            continue;
                }
                if ( obj )
                {
                    obj->x = bld->x;
                    obj->y = bld->y;
                    obj->z = bld->z;
                    obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
            }
            else if ( bld->type == BUILDING_TRANSMISSION_TOWER )
            {
                BUILDING_DATA *jam;
                range = bld->level * 2;
                int sx,ex,sy,ey;

                sx = bld->x - range;
                ex = bld->x + range + 1;
                sy = bld->y - range;
                ey = bld->y + range + 1;
                if ( sx < BORDER_SIZE )
                    sx = BORDER_SIZE;
                if ( sy < BORDER_SIZE )
                    sy = BORDER_SIZE;
                if ( ex > MAX_MAPS - BORDER_SIZE )
                    ex = MAX_MAPS - BORDER_SIZE;
                if ( ey > MAX_MAPS - BORDER_SIZE )
                    ey = MAX_MAPS - BORDER_SIZE;

                for ( x = sx;x < ex;x++ )
                    for ( y = sy;y < ey;y++ )
                {
                    if ( x < 0 || y < 0 || y >= MAX_MAPS || y >= MAX_MAPS )
                        continue;
                    jam = map_bld[x][y][z];
                    if ( jam == NULL )
                        continue;
                    if ( jam->value[3] != 0 || is_neutral(jam->type) )
                        continue;
                    if ( !jam->owner || !bld->owner )
                        continue;
                    if ( jam->owner == bld->owner )
                        continue;
                    if ( ch->pcdata->alliance != -1 && ch->pcdata->alliance == jam->owner->pcdata->alliance )
                        continue;
                    if ( number_percent() < bld->level * 10 )
                        continue;
                    if ( IS_SET(bld->value[1],INST_FIREWALL) && number_percent() < bld->level * 20 )
                        continue;
                    jam->value[3] = 2;
                    free_string(jam->attacker);
                    jam->attacker = str_dup(bch->name);
                    send_to_char( "@@yThere has been a security breach in one of your systems!@@N\n\r", jam->owner );
                    if ( jam->z != Z_PAINTBALL )
                        jam->owner->fighttimer = 480;
                    if ( bld->owner->z != Z_PAINTBALL )
                        bld->owner->fighttimer = 120 * 8;
                    break;
                }
            }
            else if ( bld->type == BUILDING_TRAFFIC_JAMMER )
            {
                range = 7;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( !ch->in_vehicle || ch->in_vehicle == NULL || ch->in_vehicle->fuel <= 0 )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,7) )
                        continue;
                    send_to_char( "Your vehicle starts shaking!\n\r", ch );
                    if ( number_percent() < ( 20 * (bld->level) ) )
                    {
                        ch->in_vehicle->fuel -= number_range(1,bld->level * 40);
                        if ( ch->in_vehicle->fuel < 0 )
                            ch->in_vehicle->fuel = 0;
                        set_fighting(bch,ch);
                    }

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_TRACTOR_BEAM )
            {
                range = 4;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( !ch->in_vehicle || ch->in_vehicle == NULL || ch->in_vehicle->fuel <= 0 )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "Your vehicle starts shaking!\n\r", ch );
                    if ( number_percent() < ( 20 * (bld->level) ) )
                    {
                        ch->in_vehicle->driving = NULL;
                        move_vehicle(ch->in_vehicle,bld->x,bld->y,bld->z);
                        ch->in_vehicle = NULL;
                        set_fighting(bch,ch);
                        send_to_char( "Your vehicle shimmers, and disappears!\n\r", ch );
                    }

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_TURRET )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_AIR) ) == NULL )
                            continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    if ( map_table.type[ch->x][ch->y][ch->z] == SECT_FOREST && !IS_SET(bld->value[1],INST_GPS) )
                        continue;
                    send_to_char( "A turret fires at you!\n\r", ch );
                    if ( number_percent() < ( 65 + (bld->level * 5) ) )
                    {
                        send_to_char( "You are hit by the turret!\n\r", ch );
                        damage( bch, ch, number_fuzzy(40+(bld->level*2)), DAMAGE_BULLETS );
                        if ( IS_SET(bld->value[1],INST_ACID_DEFENSE) && ch && ch->position != POS_DEAD )
                        {
                            send_to_char( "The bullets were covered in acid!\n\r", ch );
                            damage(bch,ch,bld->level*10,DAMAGE_ACID);
                        }
                    }
                    else
                        send_to_char( "You manage to dodge the bullets... for now...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_T_TURRET )
            {
                range = 8;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,8) )
                        continue;
                    send_to_char( "A turret fires at you!\n\r", ch );
                    if ( number_percent() < ( 75 + (bld->level * 5) ) )
                    {
                        send_to_char( "You are hit by the turret!\n\r", ch );
                        damage( bch, ch, number_fuzzy(1+(bld->level*2)), DAMAGE_BULLETS );
                    }
                    else
                        send_to_char( "You manage to dodge the bullets... for now...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }

            break;
            /**/        case 'U':
            if ( bld->type == BUILDING_UNDERGROUND_TURRET )
            {
                range = 4;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;

                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "A turret fires at you!\n\r", ch );
                    if ( number_percent() < ( 65 + (bld->level * 5) ) )
                    {
                        send_to_char( "You are hit by the turret!\n\r", ch );
                        damage( bch, ch, number_fuzzy((bld->level*20)), DAMAGE_BULLETS );
                        if ( IS_SET(bld->value[1],INST_ACID_DEFENSE) && ch && ch->position != POS_DEAD )
                        {
                            send_to_char( "The bullets were covered in acid!\n\r", ch );
                            damage(bch,ch,bld->level*10,DAMAGE_ACID);
                        }
                    }
                    else
                        send_to_char( "You manage to dodge the bullets... for now...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            break;
            /**/        case 'V':
            if ( bld->type == BUILDING_VIRUS_ANALYZER )
            {
                bld->value[0] = 0;
            }
            break;
            /**/        case 'W':
            if ( bld->type == BUILDING_WARP )
            {
                if ( i >= 1 )
                    continue;

                if ( number_percent() < ( 10 - bld->level * 2) )
                    continue;
                if ( ( obj = create_object( get_obj_index(OBJ_VNUM_SUIT_WARP), 0 ) ) == NULL )
                    continue;
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
                obj->value[1] = number_range(1,bld->level * 7);
            }
            else if ( bld->type == BUILDING_WATCHTOWER )
            {
                range = 4;

                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_AIR) ) == NULL )
                            continue;

                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    send_to_char( "@@eA burst of bullets is fired at you!@@n\n\r", ch );
                    if ( number_percent() < 50 )
                    {
                        int dam = (bld->level * 10);
                        if ( ch->z == Z_AIR )
                            dam *= 1.5;
                        set_fighting(bch,ch);
                        send_to_char( "@@eThe bullets strike you!@@n\n\r", ch );
                        damage(bch,ch,dam,DAMAGE_BULLETS);
                    }
                    else
                        send_to_char( "You stormed through the bullets unharmed...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_WEATHER_MACHINE )
            {
                range = 7;

                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_AIR) ) == NULL )
                            continue;
                    if ( is_evil(bld) )
                        bch = ch;
                    else
                    {
                        if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                            continue;
                    }
                    if ( !building_can_shoot(bld,ch,range) )
                        continue;
                    if ( ch->hit == 1 )
                        continue;
                    send_to_char( "A gust of wind blows at you, followed by a giant ice storm!\n\r", ch );
                    if ( number_percent() < ( 55 + (bld->level*7) ) && ( !ch->in_vehicle || AIR_VEHICLE(ch->in_vehicle->type)) )
                    {
                        int dam = 50 + (bld->level * 20);
                        if ( ch->z == Z_AIR )
                        {
                            dam *= 1.5;
                            i=number_range(0,3);
                            ch->c_level = i;
                        }
                        if ( dam >= ch->hit )
                            dam = ch->hit - 1;
                        set_fighting(bch,ch);
                        send_to_char( "The blizzard strikes you!\n\r", ch );
                        damage(bch, ch, dam, DAMAGE_ENVIRO);
                    }
                    else
                        send_to_char( "You stormed through the blizzard unharmed...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_WEB_RESEARCH )
            {
                if ( i >= 1 )
                    continue;

                if ( number_percent() < 50 )
                    continue;
                if ( bld->level > 3 && number_percent() < 50 )
                {
                    if ( ( obj = create_object( get_obj_index(990), 0 ) ) == NULL )
                        continue;
                }
                else
                {
                    if ( ( obj = create_object( get_obj_index(991), 0 ) ) == NULL )
                        continue;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_WEAPONS_LAB )
            {
                if ( i >= 1 )
                    continue;

                if ( bld->level < 3 )
                    continue;
                if ( bld->level == 5 && number_percent() < 2 )
                {
                    if ( ( obj = create_object( get_obj_index(OBJ_VNUM_FIRE_TURRET_U), 0 ) ) == NULL )
                        continue;
                }
                else if ( bld->level >= 4 && number_percent() < 50)
                {
                    if ( number_percent() != 1 )
                        continue;
                    if ( number_percent() < 70 )
                    {
                        if ( ( obj = create_object( get_obj_index(OBJ_VNUM_IDUP), 0 ) ) == NULL )
                            continue;
                    }
                    else
                    {
                        if ( ( obj = create_object( get_obj_index(OBJ_VNUM_DEPLEATED_URA), 0 ) ) == NULL )
                            continue;
                    }
                }
                else
                {
                    if ( number_percent() <= 70 )
                        continue;
                    if ( ( obj = create_object( get_obj_index(988), 0 ) ) == NULL )
                        continue;
                }
                obj->x = bld->x;
                obj->y = bld->y;
                obj->z = bld->z;
                obj_to_room( obj, get_room_index(ROOM_VNUM_WMAP) );
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            else if ( bld->type == BUILDING_WAVE_GENERATOR )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        if ( bld->z != Z_GROUND || ( ch = get_rand_char(x,y,Z_AIR) ) == NULL )
                            continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,5) )
                        continue;
                    send_to_char( "@@eA burst of supersonic waves hurt your ears!@@n\n\r", ch );
                    damage( bch, ch, number_fuzzy(10*(bld->level*2)), DAMAGE_SOUND );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_WARP_TOWER )
            {
                range = 5;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,5) )
                        continue;
                    send_to_char( "A bright ray of light surrounds you!\n\r", ch );
                    if ( number_percent() < ( 20 * (bld->level) ) )
                    {
                        int x,y;
                        set_fighting(bch,ch);
                        act( "You feel yourself transported elsewhere!", ch, NULL, NULL, TO_CHAR );
                        act( "$n dissolves from the world!", ch, NULL, NULL, TO_ROOM );
                        x = URANGE(5,number_range(ch->x-(bld->level*10),ch->x+(bld->level*10)),MAX_MAPS-5);
                        y = URANGE(5,number_range(ch->y-(bld->level*10),ch->y+(bld->level*10)),MAX_MAPS-5);
                        if ( x <= 4 )
                            x = 4;
                        if ( y <= 4 )
                            y = 4;
                        if ( y >= MAX_MAPS-4 )
                            y =MAX_MAPS-4;
                        if ( x >= MAX_MAPS-4 )
                            x = MAX_MAPS-4;
                        move(ch,x,y,ch->z);
                        if ( ch->in_vehicle )
                        {
                            if ( number_percent() < 20 )
                            {
                                ch->in_vehicle->driving = NULL;
                                move_vehicle(ch->in_vehicle,number_fuzzy(ch->x),number_fuzzy(ch->y),ch->z);
                                ch->in_vehicle = NULL;
                                act( "You have been seperated from your vehicle!", ch, NULL, NULL, TO_CHAR );
                            }
                        }
                        act( "$n dissolves into the world!", ch, NULL, NULL, TO_ROOM );
                        do_look(ch,"");
                    }
                    else
                        send_to_char( "Nothing seems to have happened... yet...\n\r", ch );

                    send_warning(bch,bld,ch);
                    break;
                }
            }
            else if ( bld->type == BUILDING_WATER_PUMP )
            {
                range = 4;
                for ( x = bld->x - range;x < bld->x + range + 1;x++ )
                    for ( y = bld->y - range;y < bld->y + range + 1;y++ )
                {
                    if ( ( ch = get_rand_char(x,y,bld->z) ) == NULL )
                        continue;
                    if ( bch->pcdata->alliance != -1 && bch->pcdata->alliance == ch->pcdata->alliance && !practicing(ch) )
                        continue;
                    if ( !building_can_shoot(bld,ch,4) )
                        continue;
                    send_to_char( "@@eA burst of water flies towards you!@@n\n\r", ch );
                    if ( number_percent() < ( 75 + (bld->level * 5) ) )
                    {
                        send_to_char( "@@eYou are hit by the burst of water!@@n\n\r", ch );
                        damage( bch, ch, number_fuzzy(40+(bld->level*2)), -1 );
                    }
                    else
                        send_to_char( "By the time the stream reaches you, it has become too weak...\n\r", ch );
                    send_warning(bch,bld,ch);
                    break;
                }
            }
            break;
            /**/        case 'X':
            break;
            /**/        case 'Y':
            break;
            /**/        case 'Z':
            break;
        }

    }
    return;
}

void do_construct( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    VEHICLE_DATA *vhc;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int fuel,ammo,hit,speed,type,i,flags,at;
    bool parts[4];
    bool mech = FALSE;
    bool drill = FALSE;
    i = 0;

    bld = ch->in_building;
    if ( bld == NULL || ( bld->type != BUILDING_GARAGE && bld->type != BUILDING_AIRFIELD ) || !complete(bld) )
    {
        if ( bld && bld->type == BUILDING_SPACE_CENTER )
        {
            construct_space_vessal(ch,argument);
            return;
        }
        else if ( bld && bld->type == BUILDING_MOTHERSHIP_COMM )
        {
            construct_alien_vessal(ch,argument);
            return;
        }
        send_to_char( "You must be inside a garage or airfield to do that.\n\r", ch );
        return;
    }

    for ( i =0;i<4;i++ )
        parts[i] = FALSE;

    hit = 0;
    fuel = 0;
    ammo = 0;
    speed = 0;
    flags = 0;
    type = -1;
    for ( obj = ch->first_carry;obj;obj = obj_next )
    {
        obj_next = obj->next_in_carry_list;
        if ( obj->item_type != ITEM_PART )
            continue;
        if ( bld->type == BUILDING_AIRFIELD && (obj->value[0] == 0 || obj->value[9] == 1) )
            continue;
        if ( bld->type == BUILDING_GARAGE && obj->value[9] == 2 )
            continue;
        if ( parts[obj->value[0]] == FALSE || (obj->value[0] == 3 && ch->class == CLASS_MECHANIC) )
        {
            if ( obj->value[0] == 1 && obj->value[5] == 20 )
            {
                if ( parts[0] == TRUE )
                    continue;
                mech = TRUE;
                hit += obj->value[1];
                speed += obj->value[2];
                fuel += obj->value[3];
                ammo += obj->value[4];
                if ( type == -1 )
                    type = obj->value[5];
                parts[1] = TRUE;
                if ( obj->value[6] != 0 && !IS_SET(flags,get_bit_value(obj->value[6])))
                    SET_BIT(flags,get_bit_value(obj->value[6]));
                extract_obj(obj);
                continue;
            }
            if ( !mech && obj->pIndexData && obj->pIndexData->vnum == 933 )
            {
                drill = TRUE;
                send_to_char( "The dark metal was used to construct a drill on the vehicle.\n\r", ch );
            }
            if ( mech && obj->value[0] == 0 )
                continue;
            hit += obj->value[1];
            speed += obj->value[2];
            fuel += obj->value[3];
            ammo += obj->value[4];
            if (obj->value[0] == 2)
                type = obj->value[5];
            if ( obj->value[6] != 0 && !IS_SET(flags,get_bit_value(obj->value[6])))
                SET_BIT(flags,get_bit_value(obj->value[6]));
            parts[obj->value[0]] = TRUE;
            extract_obj(obj);
        }

    }
    if ( hit <= 0 || fuel <= 0 || speed <= 0 || type <= -1 )
    {
        send_to_char("Production Failed.\n\r", ch );
        if ( hit <= 0 )
            send_to_char( "Vehicle created with no HP!\n\r", ch );
        if ( fuel <= 0 )
            send_to_char( "Vehicle created with no fuel!\n\r", ch );
        if ( speed <= 0 )
            send_to_char( "Vehicle created with no mobility!\n\r", ch );
        if ( type <= -1 )
            send_to_char( "You didn't use all the parts required.\n\r", ch );
        return;
    }

    at = type;
    if ( bld->type == BUILDING_GARAGE )
    {
        if ( type == 0 )
            type = VEHICLE_TRUCK;
        else if ( type == 5 )
            type = VEHICLE_TANK;
        else if ( type == 6 )
            type = VEHICLE_JEEP;
        else if ( type == 11 )
            type = VEHICLE_BBQ;
        else if ( type == 17 )
            type = VEHICLE_LASER;
        else if ( type == 40 )
            type = VEHICLE_XRAY;
        if ( mech )
            type = VEHICLE_MECH;
    }
    else
    {

        if ( type == 0 )
            type = VEHICLE_CHINOOK;
        else if ( type == 10 )
            type = VEHICLE_BOMBER;
        else if ( type == 41 )
            type = VEHICLE_BIO_FLOATER;
        else
            type = VEHICLE_AIRCRAFT;
    }

    if ( (vhc = create_vehicle(type)) == NULL )
        return;
    move_vehicle(vhc,bld->x,bld->y,bld->z);
    if ( IS_SET(bld->value[1],INST_RESOURCE_PURIFIER) )
    {
        hit *= 1.1;
        ammo *= 1.1;
        fuel *= 1.1;
    }
    hit += (hit/100)* number_range(0,ch->pcdata->skill[gsn_engineering]*2);
    ammo += (ammo/100)* number_range(0,ch->pcdata->skill[gsn_engineering]*2);
    fuel += (fuel/100)* number_range(0,ch->pcdata->skill[gsn_engineering]*2);

    if ( has_ability(ch,6) )                                //Mechanic v2
    {
        hit *= 1.15;
        fuel *= 1.15;
    }
    else if ( has_ability(ch,5) )                           //Mechanic v1
    {
        hit *= 1.05;
    }
    vhc->max_fuel = fuel;
    vhc->fuel = fuel;
    vhc->max_hit = hit;
    vhc->hit = hit;
    vhc->max_ammo = ammo;
    vhc->ammo = ammo;
    vhc->ammo_type = at;
    vhc->speed = speed;
    vhc->flags = flags;
    vhc->in_building = bld;
    if ( vhc->max_fuel >= 5000 )
    {
        vhc->fuel = 5000;
        vhc->max_fuel = 5000;
    }
    if ( vhc->max_ammo >= 5000 )
    {
        vhc->ammo = 5000;
        vhc->max_ammo = 5000;
    }
    if ( vhc->speed >= 10 )
        vhc->speed = 10;

    if ( vhc->type == VEHICLE_BOMBER)
    {
        vhc->max_ammo = 3;
        vhc->ammo = 3;
    }
    else if ( vhc->type == VEHICLE_AIRCRAFT )
    {
        vhc->ammo_type = 17;
    }
    else if ( vhc->type == VEHICLE_CHINOOK )
    {
        vhc->max_ammo = 0;
        vhc->ammo = 0;
    }
    if ( drill )
        SET_BIT(vhc->flags,VEHICLE_DRILL);
    send_to_char( "You produce the vehicle.\n\r", ch );
    return;
}

bool upgradable ( BUILDING_DATA *bld )
{
    if ( is_neutral(bld->type) || bld->type < 1 || bld->type >= MAX_BUILDING )
        return FALSE;
    if ( build_table[bld->type].act == BUILDING_RESOURCES )
        return FALSE;
    if ( bld->type != BUILDING_BAR && bld->type != BUILDING_GATHERER && bld->type != BUILDING_WAREHOUSE && bld->type != BUILDING_SECURE_WAREHOUSE && bld->type != BUILDING_SPACE_CENTER && bld->type != BUILDING_REFINERY )
        return TRUE;
    return FALSE;
}

void do_mimic( CHAR_DATA *ch, char *argument )
{
    int i;
    BUILDING_DATA *bld;
    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must be inside a dummy building.\n\r", ch );
        return;
    }
    if ( bld->type != BUILDING_DUMMY || str_cmp(bld->owned,ch->name) || !complete(bld) )
    {
        send_to_char( "You can't do that here.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "Please provide a building to mimic.\n\r", ch );
        return;
    }
    for ( i = 1;i<MAX_BUILDING;i++ )
    {
        if ( !str_cmp(build_table[i].name,argument) )
        {
            bld->value[0] = i;
            send_to_char( "Ok.", ch );
            return;
        }
    }
    send_to_char( "No such building.\n\r", ch );
    return;
}

bool check_missile_defense(OBJ_DATA *obj)
{
    bool ex = FALSE;
    int x,y,z,xx,yy,y1;
    BUILDING_DATA *bld;
    CHAR_DATA *bch;

    x = obj->x - 5;
    y1 = obj->y - 5;
    if ( x < 0 )
        x = 0;
    if ( y1 < 0 )
        y1 = 0;
    xx = obj->x + 5;
    yy = obj->y + 5;
    if ( xx >= MAX_MAPS )
        xx = MAX_MAPS-1;
    if ( yy >= MAX_MAPS )
        yy = MAX_MAPS-1;
    z = obj->z;
    for ( ;x <= xx;x++ )
    {
        for ( y=y1;y <= yy;y++ )
        {
            if ( INVALID_COORDS(x,y) )
                continue;

            bld = map_bld[x][y][z];

            if ( !bld || bld->type != BUILDING_MISSILE_DEFENSE || !complete(bld) )
                continue;
            bch = bld->owner;
            if ( !bch )
                if ( ( bch = get_ch(bld->owned) ) == NULL )
                    continue;
            ex = TRUE;
            send_to_char( "@@eYour missile defense system has intercepted a bomb!@@N\n\r", bch );
            break;
        }
        if ( ex )
            break;
    }
    return ex;
}

void construct_alien_vessal(CHAR_DATA *ch, char *argument)
{
    VEHICLE_DATA *vhc;

    if ( map_vhc[ch->x][ch->y][ch->z] )
    {
        send_to_char( "There is already a vehicle in here. You cannot summon another.\n\r", ch );
        return;
    }
    if ( ( vhc = create_vehicle(VEHICLE_ALIEN_SCOUT) ) == NULL )
        return;
    vhc->scanner = 10;
    vhc->range = 5;
    move_vehicle(vhc,ch->x,ch->y,ch->z);
    vhc->fuel = 1000;
    vhc->max_fuel = 1000;
    vhc->ammo = 10;
    vhc->max_ammo = 10;
    vhc->ammo_type = 5;
    vhc->hit = 100;
    vhc->max_hit = 100;
    vhc->speed = 7;
    send_to_char( "You have summoned an alien scout!\n\r", ch );
    return;
}

void check_alien_hide(OBJ_DATA *obj)
{
    int i,x,t;
    x = obj->level;
    t = (obj->value[0]==DAMAGE_BULLETS)?3:(obj->value[0]==DAMAGE_GENERAL)?2:(obj->value[0]==DAMAGE_BLAST)?4:(obj->value[0]==DAMAGE_ACID)?5:(obj->value[0]==DAMAGE_FLAME)?6:(obj->value[0]==DAMAGE_LASER)?7:(obj->value[0]==DAMAGE_SOUND)?8:-1;
    if ( t == -1 )
        return;
    for ( i=2;i<9;i++ )
    {
        if ( i == t )
            continue;
        if ( obj->value[i] < obj->level / 2 )
            obj->value[i] = obj->level / 2;
    }
    return;
}
