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
#include "mapper.h"

int leads_to args( ( BUILDING_DATA *bld, int dir, int xy ) );
void spell_imprint( int sn, int level, CHAR_DATA *ch, void *vo );
CHAR_DATA *search_dir_name( CHAR_DATA *ch, char *argument, int direction, int range );
void game_in_play(CHAR_DATA *ch, char *name);
void warp( CHAR_DATA *ch, int range);
void sell_item( CHAR_DATA *ch, OBJ_DATA *obj );

extern char * const dir_name[];
extern char * const dir_name_2[];
extern char * const compass_name[];
extern int const order[];

const struct resource_color_type resource_color_table[8] =
{
    { "@@d" },
    { "@@b" },
    { "@@R" },
    { "@@y" },
    { "@@W" },
    { "@@g" },
    { "@@b" },
    { "@@r" },
};

bool can_build( int type, int sect, int planet )
{
    int i,sec;

    for ( i=0;i<MAX_BUILDON;i++ )
    {
        sec = build_table[type].buildon[i];
        if ( sec == sect )
            return TRUE;
    }

    return FALSE;
}

int leads_to( BUILDING_DATA *bld, int dir, int xy )
{
    int x = bld->x;
    int y = bld->y;
    if ( dir == DIR_NORTH )
        y = bld->y + 1;
    else if ( dir == DIR_SOUTH )
        y = bld->y - 1;
    else if ( dir == DIR_EAST )
        x = bld->x + 1;
    else if ( dir == DIR_WEST )
        x = bld->x - 1;
    if ( xy == 0 )
        return x;
    else
        return y;
}

int get_loc( char *loc )
{
    if ( !str_prefix(loc,"Head") )
    {
        if ( number_percent() < 70 )
            return WEAR_HEAD;
        else if ( number_percent() < 70 )
            return WEAR_FACE;
        else
            return WEAR_EYES;
    }
    else if ( !str_prefix(loc,"neck") )
    {
        if ( number_percent() < 50 )
            return WEAR_NECK_2;
        else
            return WEAR_NECK_1;
    }
    else if ( !str_prefix(loc,"torso") )
    {
        if ( number_percent() < 50 )
            return WEAR_BODY;
        else if ( number_percent() < 50 )
            return WEAR_ABOUT;
        else
            return WEAR_SHOULDERS;
    }
    else if ( !str_prefix(loc,"arms") )
    {
        if ( number_percent() < 50 )
            return WEAR_ARMS;
        else if ( number_percent() < 50 )
            return WEAR_HOLD_HAND_L;
        else
            return WEAR_HOLD_HAND_R;
    }
    else if ( !str_prefix(loc,"legs") )
    {
        if ( number_percent() < 50 )
            return WEAR_LEGS;
        else if ( number_percent() < 50 )
            return WEAR_FEET;
        else
            return WEAR_ABOUT;
    }
    return -1;
}

int parse_direction( CHAR_DATA *ch, char *arg )
{
    int door = -1;

    if ( !str_prefix( arg, "north" ) ) door = DIR_NORTH;
    else if ( !str_prefix( arg, "east"  ) ) door = DIR_EAST;
    else if ( !str_prefix( arg, "south" ) ) door = DIR_SOUTH;
    else if ( !str_prefix( arg, "west"  ) ) door = DIR_WEST;
    //    else if ( !str_cmp( arg, "u" ) || !str_cmp( arg, "up"    ) ) door = DIR_UP;
    //    else if ( !str_cmp( arg, "d" ) || !str_cmp( arg, "down"  ) ) door = DIR_DOWN;

    return door;
}

char *building_title[MAX_BUILDING_TYPES] =
{"Core","Superweapons","Defenses","Offensive","Labs","Resources","Other"};
void do_a_build( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    char buf1[MSL];
    char buf2[MSL];
    int i,x,y;
    int rank = get_rank(ch);
    bool found = FALSE;
    BUILDING_DATA *bld;
    BUILDING_DATA *bld2;
    unsigned char built[MAX_BUILDING];
    bool build[MAX_BUILDING];
    int count[MAX_BUILDING];
    int buildings = 0;
    int lab = 0;
    int mines = 0;
    sh_int ndist[MAX_BUILDING_TYPES];

    int bact[MAX_BUILDING_TYPES];
    bool tact[MAX_BUILDING_TYPES];
    int cact1 = 0;
    int cact2 = 2;
    int cact3 = 4;
    int j;
    int k;
    int m;
    bool ok;
    bool here = FALSE;
    extern int buildings_lists[MAX_BUILDING_TYPES][MAX_POSSIBLE_BUILDING];

    buf2[0] = '\0';

    for ( i = 0;i < MAX_BUILDING_TYPES;i++ )
    {
        bact[i] = 0;
        tact[i] = TRUE;
        ndist[i] = -1;
    }
    for ( i = 0;i<MAX_BUILDING;i++ )
    {
        if ( sysdata.killfest )
            built[i] = 5;
        else
            built[i] = 0;
        build[i] = FALSE;
        count[i] = 0;
    }
    if ( sysdata.killfest )
        for ( bld = ch->first_building;bld;bld = bld->next_owned )
            build[bld->type] = TRUE;

    for ( bld = ch->first_building;bld;bld = bld->next_owned )
    {
        if ( sysdata.killfest )
        {
            buildings++;
            continue;
        }
        if ( is_neutral(bld->type) )
            continue;
        {
            x = 0;
            y = 0;
            count[bld->type]++;

            if ( build_table[bld->type].requirements != 0 )
            {
                y = build_table[bld->type].requirements;
                if ( build_table[build_table[bld->type].requirements].requirements != 0 )
                    x = build_table[build_table[bld->type].requirements].requirements;
            }
            if ( complete(bld) )
            {
                if ( bld->type == BUILDING_ACIDBLASTER || bld->type == BUILDING_ROCKETCANNON || bld->type == BUILDING_SNOW_DIGGER )
                    build[BUILDING_FLAMESPITTER] = bld->level;
                if ( bld->type == BUILDING_IMPROVED_MINE )
                    if ( bld->level > built[BUILDING_MINE] )
                        built[BUILDING_MINE] = bld->level;
                if ( bld->type == BUILDING_SECURE_WAREHOUSE )
                    if ( bld->level > built[BUILDING_WAREHOUSE] )
                        built[BUILDING_WAREHOUSE] = bld->level;
                if ( bld->type == BUILDING_L_TURRET )
                    if ( bld->level > built[BUILDING_TURRET] )
                        built[BUILDING_TURRET] = bld->level;

                if ( bld->level > built[bld->type] )
                    built[bld->type] = bld->level;
            }
            if ( x > 0 )
                build[x] = TRUE;
            if ( y > 0 )
                build[y] = TRUE;
            if ( bld->type == BUILDING_IMPROVED_MINE )
                build[BUILDING_MINE] = TRUE;
            build[bld->type] = TRUE;
            if ( build_table[bld->type].act == BUILDING_LAB )
            {
                sprintf( buf2+strlen(buf2), "%s (at %d/%d)\n\r", bld->name, bld->x, bld->y );
                lab = bld->type;
            }
            if ( build_table[x].act == BUILDING_LAB )
            {
                sprintf( buf2+strlen(buf2), "%s (at %d/%d)\n\r", bld->name, bld->x, bld->y );
                lab = x;
            }
            if ( build_table[y].act == BUILDING_LAB )
            {
                sprintf( buf2+strlen(buf2), "%s (at %d/%d)\n\r", bld->name, bld->x, bld->y );
                lab = y;
            }
            buildings++;
        }
    }
    if ( !built[BUILDING_HQ]>0 )
        for ( i = 0;i<MAX_BUILDING;i++ )
    {
        built[i] = 0;
        build[i] = FALSE;
    }

    built[0] = 1;

    if ( !str_cmp(argument,"here") )
        here = TRUE;
    //	else if ( !str_cmp(argument,"here all") )
    //	{	here = TRUE; all = TRUE; }

    if ( argument[0] == '\0' || here )
    {
        char s_buf[MSL];
        int sect = map_table.type[ch->x][ch->y][ch->z];

        sprintf(s_buf,"\n\r");
        if ( ( bld = get_building(ch->x,ch->y,ch->z) ) != NULL )
        {
            for ( i=0;i<8;i++ )
            {
                if ( bld->resources[i] > 0 )
                {
                    ch->c_sn = gsn_build;
                    ch->c_time = 8;
                    ch->c_level = 0;
                    act( "You continue building the structure.", ch, NULL, NULL, TO_CHAR );
                    act( "$n continues building the structure.", ch, NULL, NULL, TO_ROOM );
                    return;
                }
            }
        }

        if ( ch->pcdata->pagelen > MAX_BUILDING / 3 )
        {
            ch->pcdata->o_pagelen = ch->pcdata->pagelen;
            ch->pcdata->pagelen = (MAX_BUILDING / 3)-5;
        }

        buf[0] = '\0';
        send_to_char("\n\r@@WYou have met the requirements for the following structures:\n\r\n\r", ch );
        if ( IS_SET(ch->config,CONFIG_BLIND) )
        {
            char buf0[MSL];
            char buf1[MSL];
            char buf2[MSL];
            char buf3[MSL];
            char buf4[MSL];
            char buf5[MSL];
            char buf6[MSL];
            char buf_x[MSL];
            bool disp[7];
            sh_int p[7];

            for ( i=0;i<7;i++ )
            {
                disp[i] = FALSE;
                p[i] = 0;
            }

            sprintf( buf0, "@@N\n\r*%s:\n\r\n\r", building_title[0] );
            sprintf( buf1, "\n\r*%s:\n\r\n\r", building_title[1] );
            sprintf( buf2, "\n\r*%s:\n\r\n\r", building_title[2] );
            sprintf( buf3, "\n\r*%s:\n\r\n\r", building_title[3] );
            sprintf( buf4, "\n\r*%s:\n\r\n\r", building_title[4] );
            sprintf( buf5, "\n\r*%s:\n\r\n\r", building_title[5] );
            sprintf( buf6, "\n\r*%s:\n\r\n\r", building_title[6] );

            for ( i=1;i<MAX_BUILDING;i++ )
            {
                if ( is_neutral(i) )
                    continue;
                if ( build_table[i].disabled )
                    continue;
                if ( is_upgrade(i) )
                    continue;
                if ( build_table[i].rank > rank )
                    continue;
                if ( build[BUILDING_MINING_LAB] && (i == BUILDING_FLAMESPITTER || i == BUILDING_ACIDBLASTER || i == BUILDING_ROCKETCANNON || i == BUILDING_SNOW_DIGGER ))
                    continue;
                if (( build[BUILDING_FLAMESPITTER] || build[BUILDING_ATOM_BOMBER]) && ( i == BUILDING_MINING_LAB || i == BUILDING_MARKETPLACE ))
                    continue;
                if ( built[build_table[i].requirements]<build_table[i].requirements_l )
                    continue;
                p[build_table[i].act]++;

                sprintf(buf_x, "%s%s", build_table[i].name,(p[build_table[i].act]==3)?"\n\r":", " );
                if ( p[build_table[i].act] == 3 )
                    p[build_table[i].act] = 0;

                if ( build_table[i].act == 0 )
                    safe_strcat(MSL,buf0,buf_x);
                if ( build_table[i].act == 1 )
                    safe_strcat(MSL,buf1,buf_x);
                if ( build_table[i].act == 2 )
                    safe_strcat(MSL,buf2,buf_x);
                if ( build_table[i].act == 3 )
                    safe_strcat(MSL,buf3,buf_x);
                if ( build_table[i].act == 4 )
                    safe_strcat(MSL,buf4,buf_x);
                if ( build_table[i].act == 5 )
                    safe_strcat(MSL,buf5,buf_x);
                if ( build_table[i].act == 6 )
                    safe_strcat(MSL,buf6,buf_x);
                if ( !disp[build_table[i].act] )
                    disp[build_table[i].act] = TRUE;
            }

            if ( disp[0] )
                send_to_char(buf0,ch);
            if ( disp[1] )
                send_to_char(buf1,ch);
            if ( disp[2] )
                send_to_char(buf2,ch);
            if ( disp[3] )
                send_to_char(buf3,ch);
            if ( disp[4] )
                send_to_char(buf4,ch);
            if ( disp[5] )
                send_to_char(buf5,ch);
            if ( disp[6] )
                send_to_char(buf6,ch);
            return;
        }
        if ( ch->first_building == NULL )
        {
            send_to_char("@@cHeadquarters@@N\n\r", ch );
            goto comments;
        }

        for ( i=0;i<MAX_BUILDING;i++ )
        {
            ok = FALSE;
            while ( (j=buildings_lists[cact1][bact[cact1]]) > 0 && !ok && !tact[cact1] )
            {
                bact[cact1]++;
                if ( is_neutral(j) )
                    continue;
                if ( build_table[j].disabled )
                    continue;
                if ( is_upgrade(j) )
                    continue;
                if ( build_table[j].rank > rank )
                    continue;
                if ( build[BUILDING_MINING_LAB] && j == BUILDING_FLAMESPITTER )
                    continue;
                if (( build[BUILDING_FLAMESPITTER] || build[BUILDING_ATOM_BOMBER]) && j == BUILDING_MINING_LAB )
                    continue;
                if ( built[build_table[j].requirements]<build_table[j].requirements_l )
                    continue;
                if ( here && !can_build(j,sect,ch->z) )
                    continue;

                ok = TRUE;
                break;
            }
            ok = FALSE;
            while ( (k=buildings_lists[cact2][bact[cact2]]) > 0 && !ok && !tact[cact2] )
            {
                bact[cact2]++;
                if ( is_neutral(k) )
                    continue;
                if ( build_table[k].disabled )
                    continue;
                if ( is_upgrade(k) )
                    continue;
                if ( build_table[k].rank > rank )
                    continue;
                if ( build[BUILDING_MINING_LAB] && k == BUILDING_FLAMESPITTER )
                    continue;
                if (( build[BUILDING_FLAMESPITTER] || build[BUILDING_ATOM_BOMBER]) && k == BUILDING_MINING_LAB )
                    continue;
                if ( built[build_table[k].requirements]<build_table[k].requirements_l )
                    continue;
                if ( here && !can_build(k,sect,ch->z) )
                    continue;
                ok = TRUE;
                break;
            }
            ok = FALSE;
            while ( (m=buildings_lists[cact3][bact[cact3]]) > 0 && !ok && !tact[cact3] )
            {
                bact[cact3]++;
                if ( is_neutral(m) )
                    continue;
                if ( build_table[m].disabled )
                    continue;
                if ( is_upgrade(m) )
                    continue;
                if ( build_table[m].rank > rank )
                    continue;
                if ( build[BUILDING_MINING_LAB] && m == BUILDING_FLAMESPITTER )
                    continue;
                if (( build[BUILDING_FLAMESPITTER] || build[BUILDING_ATOM_BOMBER]) && m == BUILDING_MINING_LAB )
                    continue;
                if ( built[build_table[m].requirements]<build_table[m].requirements_l )
                    continue;
                if ( !str_infix("lab",build_table[m].name) && lab != 0 && lab != m )
                    continue;
                if ( here && !can_build(m,sect,ch->z) )
                    continue;
                ok = TRUE;
                break;
            }

            sprintf(s_buf, "%s%-24s%s       %s%-24s%s       %s%-24s%s\n\r",
                tact[cact1] ? "@@a" : "", tact[cact1] ? building_title[cact1] : (j>0) ? build_table[j].name : "", tact[cact1] ? "@@c" : "",
                tact[cact2] ? "@@a" : "", tact[cact2] ? building_title[cact2] : (k>0) ? build_table[k].name : "", tact[cact2] ? "@@c" : "",
                tact[cact3] ? "@@a" : "", tact[cact3] ? building_title[cact3] : (m>0) ? build_table[m].name : "", tact[cact3] ? "@@c" : "" );

            if ( !(m <= 0 && j <= 0 && k <= 0 ) || tact[BUILDING_OTHER] )
                safe_strcat(MSL,buf,s_buf);

            if ( tact[cact1] )
                tact[cact1] = FALSE;
            if ( tact[cact2] )
                tact[cact2] = FALSE;
            if ( tact[cact3] )
                tact[cact3] = FALSE;

            if ( j < 0 && cact1 < 1 )
                cact1++;
            if ( k < 0 && cact2 < 3 )
                cact2++;
            if ( m < 0 && cact3 < MAX_BUILDING_TYPES -1 )
                cact3++;
            if ( m <= 0 && j <= 0 && k <= 0 && cact1 == 2 && cact2 == 3 && cact3 == MAX_BUILDING_TYPES )
                break;
        }
        send_to_char(buf,ch);
        comments:
        //		if ( my_get_minutes(ch,FALSE) < 20 )
        //			send_to_char( "\n\r@@gNote that the @@abright@@g-colored text are not actual buildings, merely general building catagories that will fill up when you get more technology.\n\rThe first building you can make is the @@cHeadquarters@@g.\n\r\n\r", ch );
        sprintf( buf, "\n\r@@cThe current building limit is @@a%d@@a.@@N\n\r", BUILDING_LIMIT );
        send_to_char( buf, ch );
        return;
    }
    else if ( !str_cmp(argument, "list") )
    {
/*        char u_buf[MSL];
        char s_buf[MSL];

        u_buf[0] = '\0';
        s_buf[0] = '\0';
        buf[0] = '\0';
        if ( ch->pcdata->pagelen > MAX_BUILDING / 3 )
        {
            ch->pcdata->o_pagelen = ch->pcdata->pagelen;
            ch->pcdata->pagelen = (MAX_BUILDING / 3)-5;
        }

        send_to_char( "\n\rName                    IR    SK    CO    GO    SI    RO    ST    LO\n\r" , ch );
        send_to_char( "--------------------------------------------------------------------\n\r", ch );
        for ( i = 1;i<MAX_BUILDING;i++ )
        {
            int j;

            if ( is_neutral(order[i]) )
                continue;
            if ( build_table[order[i]].disabled )
                continue;
            if ( build_table[order[i]].rank > 9999 )
                continue;
            if ( is_upgrade(order[i]) )
            {
                sprintf( u_buf+strlen(u_buf), "@@e%-24s\n\r", build_table[order[i]].name );
            }
            else if ( build_table[order[i]].rank > 0 )
            {
                if ( build_table[order[i]].rank == 9999 )
                {
                    sprintf( s_buf+strlen(s_buf), "@@c%-24s                @@lUnknown\n\r", build_table[order[i]].name );
                }
                else
                {
                    sprintf( s_buf+strlen(s_buf), "@@c%-24s                @@lRank %d\n\r", build_table[order[i]].name, build_table[order[i]].rank );
                }
            }
            else
            {
                sprintf( buf+strlen(buf), "@@a%-24s", build_table[order[i]].name );
                for ( j = 0;j<8;j++ )
                    sprintf( buf+strlen(buf), "%s%-6d", resource_color_table[j].color, build_table[order[i]].resources[j] );
                sprintf( buf+strlen(buf), "\n\r" );
            }
            if ( i == MAX_BUILDING / 2 )
            {
                send_to_char( buf, ch );
                buf[0] = '\0';
            }
        }
        sprintf( buf, "%s\n\r%s\n\r%s", buf, s_buf, u_buf );
        send_to_char( buf, ch );
        sprintf( buf, "\n\r@@cThe current building limit is @@a%d@@a.@@N\n\r", BUILDING_LIMIT );
        send_to_char( buf, ch );
        return;*/
    }
    else if ( !str_cmp(argument, "report") )
    {
        char ibuf[MSL];
        bool s1=FALSE,s2=FALSE;

        sprintf( buf, "@@aCompleted Buildings:\n\r" );
        sprintf( ibuf, "@@cIncomplete Buildings:\n\r" );
        i = 0;
        for (bld = ch->first_building;bld;bld = bld->next_owned )
        {
            if ( is_neutral(bld->type) || IS_SET(bld->value[1],INST_SPOOF) )
                continue;
            i++;
            if ( !complete(bld) )
            {
                if ( bld->z != Z_UNDERGROUND )
                    sprintf( buf1, "@@c%3d.@@d %-20s [%d] %3d/%-3d [%s]%s", i, bld->name, bld->level, bld->x, bld->y, planet_table[bld->z].name, (!s1)?"        ":"\n\r" );
                else
                    sprintf( buf1, "@@c%3d.@@d %-20s [%d] ?/? [%s]%s", i, bld->name, bld->level, planet_table[bld->z].name, (!s1)?"        ":"\n\r" );
                safe_strcat(MSL,ibuf,buf1);
                s1 = !s1;
            }
            else
            {
                if ( bld->z != Z_UNDERGROUND )
                    sprintf( buf1, "@@a%3d.@@W %-20s [%d] %3d/%-3d [%s] %3s%3s%s", i, bld->name, bld->level, bld->x, bld->y, planet_table[bld->z].name, (bld->value[8] != 0) ? "@@e(H)@@N" : (bld->password==0)?"@@y(H)@@N" : "", (bld->value[3] != 0)? "@@p(V)@@N" : "", (!s2)?" ":"\n\r"  );
                else
                    sprintf( buf1, "@@a%3d.@@W %-20s [%d] ?/? [%s] %3s%3s%s", i, bld->name, bld->level, planet_table[bld->z].name, (bld->value[8] != 0) ? "@@e(H)@@N" : (bld->password==0)?"@@y(H)@@N" : "", (bld->value[3] != 0)? "@@p(V)@@N" : "", (!s2)?" ":"\n\r"  );
                safe_strcat(MSL,buf,buf1);
                s2 = !s2;
            }
        }
        send_to_char(buf,ch);
        send_to_char("\n\r",ch);
        if ( s2 )
            send_to_char("\n\r",ch);
        send_to_char(ibuf,ch);
        if ( s1 )
            send_to_char("\n\r",ch);
        sprintf( buf, "\n\r@@gTotal: %d\n\r", buildings );
        send_to_char( buf, ch );
        return;
    }
    else if ( !str_cmp(argument, "report2") )
    {
        char ibuf[MSL];
        sprintf( buf, "Completed Buildings:\n\r" );
        sprintf( ibuf, "Incomplete Buildings:\n\r" );
        i = 0;
        for (bld = ch->first_building;bld;bld = bld->next_owned )
        {
            if ( is_neutral(bld->type) || IS_SET(bld->value[1],INST_SPOOF) )
                continue;
            i++;
            if ( !complete(bld) )
                sprintf( ibuf+strlen(ibuf), "%d. %s Level %d at %d/%d [%s] %s%s\n\r", i, bld->name, bld->level, bld->x, bld->y, planet_table[bld->z].name, ( IS_SET(bld->value[1], INST_ANTIVIRUS ) ) ? "A" : "", ( IS_SET(bld->value[1], INST_FIREWALL ) ) ? "F" : "" );
            else
                sprintf( buf+strlen(buf), "%d. %s Level %d at %d/%d [%s]%s%s %s%s\n\r", i, bld->name, bld->level, bld->x, bld->y, planet_table[bld->z].name, (bld->value[8] != 0) ? " @@e(HACKER)@@N" : (bld->password==0)?" @@y(HACKED)@@N" : "", (bld->value[3] != 0)? " @@l(VIRUS)@@N" : "" , ( IS_SET(bld->value[1], INST_ANTIVIRUS ) ) ? "A" : "", (       IS_SET(bld->value[1], INST_FIREWALL ) ) ? "F" : "" );
        }
        send_to_char(buf,ch);
        send_to_char("\n\r",ch);
        send_to_char(ibuf,ch);
        sprintf( buf, "\n\rTotal: %d\n\r", buildings );
        send_to_char( buf, ch );
        return;

    }
    if ( map_bld[ch->x][ch->y][ch->z] )
    {
        send_to_char( "There is already a building here.\n\r", ch );
        if ( IS_NEWBIE(ch))
            send_to_char( "@@WTIP:@@N You can only build one building per room. Try leaving this room by using the @@eN@@North, @@eS@@Nouth, @@eE@@Nast and @@eW@@Nest commands.\n\r", ch );
        return;
    }
    if ( ch->z == Z_PAINTBALL || ch->z == Z_SPACE )
    {
        send_to_char( "You cannot build on this surface.\n\r", ch );
        return;
    }
    if ( ch->in_vehicle )
    {
        send_to_char( "You must exit the vehicle first.\n\r", ch );
        return;
    }
    if ( buildings >= BUILDING_LIMIT )
    {
        send_to_char( "You have hit the building limit.\n\r", ch );
        return;
    }

    if ( !str_cmp(argument,"hq") )
        sprintf(argument,"headquarters");
    else if ( !str_cmp(argument,"greenhouse") )
        sprintf(argument,"lumberyard");

    for ( i=0;i<MAX_BUILDING;i++ )
    {
        if ( !str_prefix(argument, build_table[i].name) )
        {
            if ( is_upgrade(i) )
                continue;
            if ( build_table[i].disabled )
                continue;
            if ( build_table[i].rank > rank )
                continue;
            if (( i == BUILDING_AIRFIELD || i == BUILDING_PARADROP || i == BUILDING_TUNNEL ) && ch->z == Z_NEWBIE )
            {
                send_to_char( "You can't build that on this grid.\n\r", ch );
                return;
            }
            if ( i == BUILDING_DOOMSDAY_DEVICE && built[BUILDING_SCUD_LAUNCHER] < built[BUILDING_ATOM_BOMBER] )
            {
                built[BUILDING_SCUD_LAUNCHER] = built[BUILDING_ATOM_BOMBER];
            }
            if ( build_table[i].act == BUILDING_OFFENSE && my_get_minutes(ch,FALSE) < 10 )
            {
                send_to_char( "You can't build offensive buildings for 10 minutes after death.\n\r", ch );
                return;
            }
            if ( !str_infix("lab",build_table[i].name) && lab != 0 && lab != i )
            {
                send_to_char( "You can only have one lab.\n\r", ch );
                send_to_char( "Buildings you need to demolish:\n\r", ch );
                send_to_char(buf2,ch);
                return;
            }
            if ( count[i] >= build_table[i].max )
            {
                sprintf( buf, "You may only have %d of that building type.\n\r", build_table[i].max );
                send_to_char(buf,ch);
                return;
            }
            if ( (i == BUILDING_SCUD_LAUNCHER && (build[BUILDING_SCUD_LAUNCHER] || build[BUILDING_NUKE_LAUNCHER] || build[BUILDING_ATOM_BOMBER] ) )
                || (i == BUILDING_NUKE_LAUNCHER && (build[BUILDING_NUKE_LAUNCHER] || build[BUILDING_SCUD_LAUNCHER] || build[BUILDING_ATOM_BOMBER] ) )
                )
            {
                send_to_char( "You can only have one superweapon.\n\r", ch );
                return;
            }
            if ( ( i == BUILDING_STATUE_SPELGURU || i == BUILDING_STATUE_CYLIS || i == BUILDING_STATUE_DEMISE || i == BUILDING_STATUE_WULFSTON ) &&
                ( build[BUILDING_STATUE_CYLIS] || build[BUILDING_STATUE_SPELGURU] || build[BUILDING_STATUE_DEMISE] || build[BUILDING_STATUE_WULFSTON] ) )
            {
                send_to_char( "You can only have one statue.\n\r", ch );
                return;
            }
            if ( build[BUILDING_MINING_LAB] && (i == BUILDING_FLAMESPITTER || i == BUILDING_ACIDBLASTER || i == BUILDING_ROCKETCANNON || i == BUILDING_SNOW_DIGGER ))
            {
                send_to_char( "Get rid of your mining lab's stuff first.\n\r", ch );
                return;
            }
            if ( (i == BUILDING_MINING_LAB || i == BUILDING_MARKETPLACE) && (build[BUILDING_FLAMESPITTER] || build[BUILDING_ATOM_BOMBER]) )
            {
                send_to_char( "Get rid of your flamespitters and/or Atom Bombers first.\n\r", ch );
                return;
            }

            if ( built[build_table[i].requirements] < build_table[i].requirements_l )
            {
                if ( build[BUILDING_HQ] == 0 )
                {
                    send_to_char( "You must have a Headquarters in order to build anything.\n\r", ch );
                    return;
                }
                send_to_char( "You have not met the requirements to construct that building.\n\r", ch );
                sprintf( buf, "Building requires: Level %d %s\n\r", build_table[i].requirements_l, build_table[build_table[i].requirements].name );
                send_to_char(buf,ch);
                return;
            }
            if ( is_neutral(i) )
            {
                send_to_char ("You can't build neutral buildings -_-\n\r", ch );
                return;
            }
            found = TRUE;
            break;
        }
    }
    if ( !found )
    {
        send_to_char( "No such structure.\n\r", ch );

        for ( i=0;i<MAX_BUILDING_TYPES;i++ )
            if ( !str_cmp(argument,building_title[i]) )
                send_to_char( "You've typed the name of a building @@ycatagory@@N, not a building @@ename@@N. Please select an actual building to make.\n\r", ch );

        if ( IS_NEWBIE(ch) )
            send_to_char( "\n\rType 'build', with no argument, to see what you can make.\n\r", ch );
        return;
    }
    if ( build_table[i].name == NULL )
        return;

    {
        int x,y,x1,y1,xx,yy;
        int dist,type,offline=-1,newbie=-1;
        bool lava = FALSE;

        x1 = ch->x - 10;
        y1 = ch->y - 10;
        if ( x1 < BORDER_SIZE )
            x1 = BORDER_SIZE;
        if ( y1 < BORDER_SIZE )
            y1 = BORDER_SIZE;
        xx = ch->x + 10;
        yy = ch->y + 10;
        if ( xx > MAX_MAPS - BORDER_SIZE )
            xx = MAX_MAPS - BORDER_SIZE;
        if ( yy > MAX_MAPS - BORDER_SIZE )
            yy = MAX_MAPS - BORDER_SIZE;

        for ( x = x1;x<xx;x++ )
        {
            for ( y=y1;y<yy;y++ )
            {
                if ( map_table.type[x][y][ch->z] == SECT_LAVA && i == BUILDING_TUNNEL )
                    lava = TRUE;
                if ( ( bld = map_bld[x][y][ch->z] ) == NULL )
                    continue;
                dist = (abs(bld->x - ch->x) < abs(bld->y - ch->y)) ? abs(bld->y - ch->y) : abs(bld->x - ch->x);
                type = build_table[bld->type].act;
                if ( bld->owner )
                {
                    if ( allied(bld->owner,ch) || ch == bld->owner )
                        continue;
                    if ( IS_NEWBIE(bld->owner) && (newbie == -1 || newbie > dist) )
                        newbie = dist;
                    if ( ndist[type] == -1 || ndist[type] > dist )
                        ndist[type] = dist;
                }
                else
                {
                    if ( dist < offline || offline == -1 )
                        offline = dist;
                }
            }
        }

        if ( offline <= 3 && offline != -1 )
        {
            sprintf(buf, "You can't build next to an offline building within a 3-room radius. There is one within %d rooms of you.\n\r", offline );
            send_to_char(buf,ch);
            return;
        }
        if ( newbie <= 5 && newbie != -1 )
        {
            sprintf(buf, "You can't build next to a new player's building within a 5-room radius. There is one within %d rooms of you.\n\r", newbie );
            send_to_char(buf,ch);
            return;
        }
        if ( i == BUILDING_GUNNER && ndist[BUILDING_CORE] <= 20 && ndist[BUILDING_CORE] != -1 )
        {
            send_to_char( "You cannot build gunners next to an enemy's core buildings within 20 rooms.\n\r", ch );
            return;
        }
        if ( build_table[i].act == BUILDING_OFFENSE && offline <= 10 && offline != -1 )
        {
            sprintf(buf, "You can't build offensive buildings within 10 rooms of an offline building, there is one %d rooms away..\n\r", offline );
            send_to_char(buf,ch);
            return;
        }
        if ( build_table[i].act == BUILDING_DEFENSE && ndist[BUILDING_CORE] <= 10 && ndist[BUILDING_CORE] != -1 )
        {
            sprintf(buf, "You can't build defenses next to a core building within a 10-room radius. There is one within %d rooms of you.\n\r", ndist[BUILDING_CORE] );
            send_to_char(buf,ch);
            return;
        }
        if ( build_table[i].act == BUILDING_CORE && ndist[BUILDING_DEFENSE] <= 10 && ndist[BUILDING_DEFENSE] != -1 )
        {
            sprintf(buf, "You can't build core buildings next to a defense building within a 10-room radius. There is one within %d rooms of you.\n\r", ndist[BUILDING_DEFENSE] );
            send_to_char(buf,ch);
            return;
        }
        if ( lava )
            send_to_char( "@@WWarning: @@gBuilding a tunnel near a volcano may be hazardous to your health. Magma rivers that flow underground might burn you to death when you tunnel down.\n\r\n\rYou have been warned.\n\r@@N", ch );
    }

    bld = create_building(i);
    if ( bld == NULL )
    {
        char buff[MSL];
        sprintf( buff, "ERROR! %d", i );
        send_to_char( buff, ch );
        return;
    }
    bld->x = ch->x;
    bld->y = ch->y;
    bld->z = ch->z;
    activate_building(bld,TRUE);
    map_bld[bld->x][bld->y][bld->z] = bld;
    bld->type = i;
    bld->maxhp = build_table[i].hp;
    bld->maxshield = build_table[i].shield;
    bld->hp = 1;
    bld->shield = 0;
    x = bld->x;
    y = bld->y;
    if ( bld->owned != NULL )
        free_string(bld->owned);
    if ( bld->name != NULL )
        free_string(bld->name);
    bld->owned = str_dup(ch->name);

    bld->owner = ch;
    if ( bld->owner->first_building )
        bld->owner->first_building->prev_owned = bld;
    bld->next_owned = bld->owner->first_building;
    bld->owner->first_building = bld;

    bld->name = str_dup(build_table[i].name);
    if ( ch->class == CLASS_ENGINEER && number_percent() < 66 && bld->type != BUILDING_SPACE_CENTER)
    {
        if ( !CIVILIAN(bld) )
            bld->level++;
        bld->maxshield *= 1.3;
        bld->maxhp *= 1.3;
        bld->hp *= 1.3;
        bld->shield *= 1.3;
    }
    for ( i=0;i<ch->pcdata->skill[gsn_building];i++ )
    {
        if ( bld->level >= MAX_BUILDING_LEVEL || bld->type == BUILDING_SPACE_CENTER )
            break;
        if ( number_percent() < 40 )
        {
            if ( !CIVILIAN(bld) )
                bld->level++;
            bld->maxshield *= 1.3;
            bld->maxhp *= 1.3;
            bld->hp *= 1.3;
            bld->shield *= 1.3;
        }
        bld->maxshield *= 1.1;
        bld->maxhp *= 1.1;
        bld->shield *= 1.1;
        bld->hp *= 1.1;
    }

    if ( !can_build(bld->type,map_table.type[bld->x][bld->y][bld->z],bld->z) )
    {
        bool ter[SECT_MAX];
        int j,k;

        for ( j=0;j<SECT_MAX;j++ )
            ter[j] = FALSE;
        buf[0] = '\0';
        for ( j=0;j<MAX_BUILDON;j++ )
        {
            k = build_table[bld->type].buildon[j];
            if ( k < 0 || k >= SECT_MAX )
                continue;
            if ( ter[k] != TRUE )
                sprintf(buf+strlen(buf), "%s%s   ", wildmap_table[k].color, wildmap_table[k].name );

            ter[k] = TRUE;
        }
        k = map_table.type[ch->x][ch->y][ch->z];
        sprintf( buf+strlen(buf),"\n\r\n\r@@NYou are on %s%s @@Nsector.\n\r", wildmap_table[k].color, wildmap_table[k].name );
        send_to_char("This building cannot be built in this location.\n\rPossible Building locations are:\n\r", ch );
        send_to_char(buf,ch);
        extract_building(bld,FALSE);
        return;
    }
    if ( ch->pcdata->set_exit == DIR_NORTH || ch->pcdata->set_exit == -1 || map_bld[bld->x][bld->y+1][bld->z] )
    {
        bld->exit[DIR_NORTH] = TRUE;
        y = bld->y + 1; x = bld->x;
        if ( ( bld2 = map_bld[x][y][bld->z] ) != NULL )
            bld2->exit[DIR_SOUTH] = TRUE;
    }
    if ( ch->pcdata->set_exit == DIR_EAST || ch->pcdata->set_exit == -1 || map_bld[bld->x+1][bld->y][bld->z] )
    {
        bld->exit[DIR_EAST] = TRUE;
        x = bld->x + 1; y = bld->y;
        if ( ( bld2 = map_bld[x][y][bld->z] ) != NULL )
            bld2->exit[DIR_WEST] = TRUE;
    }
    if ( ch->pcdata->set_exit == DIR_SOUTH || ch->pcdata->set_exit == -1 || map_bld[bld->x][bld->y-1][bld->z] )
    {
        bld->exit[DIR_SOUTH] = TRUE;
        y = bld->y - 1; x = bld->x;
        if ( ( bld2 = map_bld[x][y][bld->z] ) != NULL )
            bld2->exit[DIR_NORTH] = TRUE;
    }
    if ( ch->pcdata->set_exit == DIR_WEST || ch->pcdata->set_exit == -1 || map_bld[bld->x-1][bld->y][bld->z] )
    {
        bld->exit[DIR_WEST] = TRUE;
        x = bld->x - 1; y = bld->y;
        if ( ( bld2 = map_bld[x][y][bld->z] ) != NULL )
            bld2->exit[DIR_EAST] = TRUE;
    }

    if ( buildings + 8 > BUILDING_LIMIT )
        send_to_char( "@@eYou are nearing the building limit. See BUILD REPORT to see your building list!@@N\n\r", ch );
    send_to_char( "You have begun the construction of ", ch );
    send_to_char( build_table[bld->type].name, ch );
    send_to_char( "\n\r", ch );
    act( "$n starts building a structure.", ch, NULL, NULL, TO_ROOM );
    if ( sysdata.killfest )
        for ( i=0;i<8;i++ )
            bld->resources[i] = 0;
    if ( ch->pcdata->skill[gsn_building] > 0 )
    {
        for ( i=0;i<8;i++ )
            if ( bld->resources[i] > 0 )
                bld->resources[i] -= (bld->resources[i] * ch->pcdata->skill[gsn_building]) /100;
    }
    ch->c_sn = gsn_build;
    ch->c_time = 8;
    ch->c_level = 0;
    if ( mines == 7 )
        if ( IS_NEWBIE(ch) )
            send_to_char( "@@ePlease note that the recommended amount of mines is 8. Perhaps you should start constructing other buildings?\n\r\n\r@@yThe recommended building amounts are: @@b8 Mines, 3 Lumberyards, 2 Quarries, 1 Tannery@@N\n\r\n\r", ch );
    return;
}

void act_build( CHAR_DATA *ch, int level )
{
    int i,j;
    bool found = FALSE;
    int more = -1;
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;

    if ( ( bld = get_char_building(ch) ) != NULL )
        found = TRUE;
    if ( !found )
    {
        ch->c_sn = -1;
        return;
    }

    for ( i = 0;i<8;i++ )
    {
        if ( bld->resources[i] > 0 )
        {
            for ( obj = ch->first_carry;obj;obj = obj_next )
            {
                if ( ch->pcdata->reimb <= 0 )
                {
                    obj_next = obj->next_in_carry_list;
                    if ( obj->item_type != ITEM_MATERIAL || (obj->value[0] != i && obj->value[0] != -1))
                        continue;
                    j = obj->value[1];
                    obj->value[1] -= bld->resources[i];
                    bld->resources[i] -= j;
                    if ( !IS_SET(ch->config,CONFIG_BRIEF ) )
                        act( "You place $p in your $T.", ch, obj, bld->name, TO_CHAR );
                    act( "$n places $p in $s $T.", ch, obj, bld->name, TO_ROOM );
                    ch->c_time = 8;
                    if ( obj->value[1] <= 0 )
                        extract_obj(obj);
                    if ( bld->resources[i] > 0 )
                        more = i;
                }
                if ( complete(bld) || ch->pcdata->reimb > 0 )
                {
                    if ( ch->pcdata->reimb > 0 )
                        ch->pcdata->reimb--;
                    bld->resources[0] = 0;
                    bld->resources[1] = 0;
                    bld->resources[2] = 0;
                    bld->resources[3] = 0;
                    bld->resources[4] = 0;
                    bld->resources[5] = 0;
                    bld->resources[6] = 0;
                    bld->resources[7] = 0;
                    if ( ch->pcdata->reimb >= 40 && !CIVILIAN(bld) )
                    {
                        bld->level = 5;
                        bld->maxshield *= 1.3;
                        bld->maxhp *= 1.3;
                        bld->maxshield *= 1.3;
                        bld->maxhp *= 1.3;
                        bld->maxshield *= 1.3;
                        bld->maxhp *= 1.3;
                        bld->maxshield *= 1.3;
                        bld->maxhp *= 1.3;
                    }
                    else if ( ch->pcdata->reimb >= 1 && !CIVILIAN(bld) )
                    {
                        bld->level = 3;
                        bld->maxshield *= 1.3;
                        bld->maxhp *= 1.3;
                        bld->maxshield *= 1.3;
                        bld->maxhp *= 1.3;
                    }
                    send_to_char( "You finish the construction!\n\r", ch );
                    bld->hp = bld->maxhp;
                    bld->shield = bld->maxshield;
                    reset_special_building(bld);
                    act( "$n stops building.", ch, NULL, NULL, TO_ROOM );
                    ch->c_sn = -1;
                    //					if ( ch->pcdata->built[bld->type] == FALSE )
                    if ( FALSE )
                    {
                        gain_exp(ch,30);
                        //						ch->pcdata->built[bld->type] = TRUE;
                        save_char_obj(ch);
                    }
                    bld = NULL;
                }
                bld = NULL;
                return;
            }
        }
        if ( bld->resources[i] > 0 && more <0 )
            more = i;

    }
    if ( more >= 0 )
    {
        char buf[MSL];
        i = more;
        sprintf( buf, "You need more %s to complete the construction.\n\r",  ( i == 0 ) ? "Iron" : ( i == 1 ) ? "Skins" : ( i == 2 ) ? "Copper" : ( i == 3 ) ? "Gold" : ( i == 4 ) ? "Silver" : ( i == 5 ) ? "Rocks" : ( i == 6 ) ? "Sticks" : "Logs" );
        send_to_char(buf,ch);
        //		send_to_char( "You need more materials to complete the construction.\n\r", ch );
        if ( IS_NEWBIE(ch) )
        {
            if ( bld->type == BUILDING_MINE )
                send_to_char( "TIP: If you have no more logs, build some Lumberyards (usually on hill or forest sectors). Once you have the resources, go back here and type BUILD.\n\r", ch );
            else if ( bld->type == BUILDING_ARMORY )
                send_to_char( "TIP: If you haven't already done so, building mines will get you the resources you need for an Armory.\n\r", ch );
        }
        else if ( bld->type == BUILDING_HQ && bld == ch->first_building )
            send_to_char( "@@WTIP:@@g So you've lost your base, eh? Don't worry. You have two options now:\n\r\n\r1) Get a friend to help you with the resources (especially gold)\n\r2) Do a couple QP quests. Once you have 200 QPs you can use the @@eQPSPEND COMPLETE@@g option to complete your HQ.\n\r\n\rGood luck.", ch );
    }
    else
    {
        send_to_char( "You finish the construction!\n\r", ch );
        bld->hp = bld->maxhp;
        bld->shield = bld->maxshield;
        //		if ( ch->pcdata->built[bld->type] == FALSE )
        if ( FALSE )
        {
            gain_exp(ch,30);
            //			ch->pcdata->built[bld->type] = TRUE;
            save_char_obj(ch);
        }
    }
    if ( bld->type == BUILDING_MINE || bld->type == BUILDING_IMPROVED_MINE || bld->type == BUILDING_LUMBERYARD || bld->type == BUILDING_SPACE_CENTER )
        bld->value[0] = -1;
    else if ( bld->type == BUILDING_DUMMY )
        bld->value[0] = BUILDING_TURRET;
    act( "$n stops building.", ch, NULL, NULL, TO_ROOM );
    reset_special_building(bld);
    ch->c_sn = -1;
    bld = NULL;
    return;
}

void do_upgrade( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    OBJ_DATA *temp = NULL;
    char buf[MSL];
    bool found = FALSE;

    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You must be standing inside a building to upgrade it.\n\r", ch );
        return;
    }
    if ( !complete(bld) )
    {
        send_to_char( "Finish building it first.\n\r", ch );
        return;
    }
    for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
    {
        if ( obj->item_type != ITEM_BLUEPRINT || obj->level <= bld->level )
            continue;
        if ( obj->value[0] != bld->type && obj->value[2] != bld->type && obj->value[6] != 2 )
            continue;

        if ( temp != NULL && temp->level < obj->level )
            continue;

        temp = obj;
        found = TRUE;
    }
    if ( !found )
    {
        send_to_char( "You do not have the right blueprints for this type of building.\n\r", ch );
        return;
    }
    obj = temp;
    if ( obj->value[1] != 0 )
    {
        int i,ave=0;
        sprintf( buf, "The building has been upgraded into %s!\n\r", build_table[obj->value[1]].name );
        send_to_loc( buf, bld->x, bld->y, bld->z );
        bld->type = obj->value[1];
        extract_obj(obj);
        bld->level = 1;
        bld->hp = build_table[bld->type].hp;
        bld->maxhp = bld->hp;
        bld->shield = build_table[bld->type].shield;
        bld->maxshield = bld->shield;
        free_string(bld->name);
        bld->name = str_dup(build_table[bld->type].name);

        ave = (ch->pcdata->skill[gsn_building] + ch->pcdata->skill[gsn_engineering]);
        if ( ave >= 2 )
            ave /= 2;
        for ( i=0;i<ave;i++ )
        {
            if ( bld->level >= MAX_BUILDING_LEVEL )
                break;
            if ( number_percent() < 40 )
            {
                if ( !CIVILIAN(bld) )
                    bld->level++;
                bld->maxshield *= 1.3;
                bld->maxhp *= 1.3;
                bld->hp *= 1.3;
                bld->shield *= 1.3;
            }
            bld->maxshield *= 1.1;
            bld->maxhp *= 1.1;
            bld->shield *= 1.1;
            bld->hp *= 1.1;
        }
        return;
    }
    bld->level++;
    bld->maxshield *= 1.3;
    bld->maxhp *= 1.3;
    bld->hp *= 1.3;
    bld->shield *= 1.3;
    if ( obj->value[6] == 0 )
        extract_obj(obj);
    sprintf( buf, "You have upgraded %s to level %d!", bld->name, bld->level );
    act( buf, ch, NULL, NULL, TO_CHAR );
    sprintf( buf, "$n has upgraded %s to level %d!", bld->name, bld->level );
    act( buf, ch, NULL, NULL, TO_ROOM );
    return;
}

void do_makeexit( CHAR_DATA *ch, char *argument )
{
    int dir;
    BUILDING_DATA *bld;
    bool all = FALSE;
    if ( ch->fighttimer > 0 )
    {
        send_to_char( "Not during combat.\n\r", ch );
        return;
    }
    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You must be in a building.\n\r", ch );
        return;
    }
    if ( str_cmp(bld->owned,ch->name) )
    {
        send_to_char( "In your OWN building, please.\n\r", ch);
        return;
    }
    if ( ( dir = parse_direction(ch,argument) ) < 0 || dir > 3 )
    {
        if ( str_cmp(argument,"all") )
        {
            send_to_char( "Invalid direction.\n\r", ch );
            return;
        }
        else
            all = TRUE;
    }
    if ( all )
    {
        for ( dir=0;dir<4;dir++ )
            bld->exit[dir] = TRUE;
    }
    else
        bld->exit[dir] = TRUE;
    send_to_char( "Exit formed.\n\r", ch );
    return;
}

void do_closeexit( CHAR_DATA *ch, char *argument )
{
    int dir;
    int exits = 0;
    BUILDING_DATA *bld;
    if ( ch->fighttimer > 0 )
    {
        send_to_char( "Not during combat.\n\r", ch );
        return;
    }
    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You must be in a building.\n\r", ch );
        return;
    }
    if ( str_cmp(bld->owned,ch->name) )
    {
        send_to_char( "In your OWN building, please.\n\r", ch);
        return;
    }
    for ( dir = 0;dir < 4;dir++ )
        if ( bld->exit[dir] == TRUE )
            exits++;
    if ( ( dir = parse_direction(ch,argument) ) < 0 || dir > 3 )
    {
        send_to_char( "Invalid direction.\n\r", ch );
        return;
    }
    if ( exits <= 1 )
    {
        send_to_char( "You can't lock yourself in!\n\r", ch);
        return;
    }
    bld->exit[dir] =FALSE;
    send_to_char( "Exit closed.\n\r", ch );
    return;
}

void do_warp( CHAR_DATA *ch, char *argument )
{
    int x,y;
    BUILDING_DATA *bld;

    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You aren't even in a building!\n\r", ch );
        return;
    }
    if ( bld->type != BUILDING_WARP )
    {
        send_to_char( "You must be inside a warp to do that.\n\r", ch );
        return;
    }
    if ( !complete(bld) )
    {
        send_to_char( "You haven't finished the construction.\n\r", ch );
        return;
    }

    x = number_range(ch->x - (bld->level * 100), ch->x + (bld->level * 100) );
    y = number_range(ch->y - (bld->level * 100), ch->y + (bld->level * 100) );
    if ( x < 5 )
        x = 5;
    if ( y < 5 )
        y = 5;
    if ( x > MAX_MAPS-5 )
        x = MAX_MAPS - 5;
    if ( y > MAX_MAPS-5 )
        y = MAX_MAPS - 5;

    if ( get_building(x,y,ch->z) != NULL || map_table.type[x][y][ch->z] == SECT_NULL )
    {
        send_to_char( "The warp failed!\n\r", ch );
        return;
    }

    act( "You feel a tingle, and quickly fade away.", ch, NULL, NULL, TO_CHAR );
    act( "$n quickly fades!", ch, NULL, NULL, TO_ROOM );
    move ( ch, x, y, ch->z );
    act( "$n fades into the room!", ch, NULL, NULL, TO_ROOM );
    do_look(ch,"");
    WAIT_STATE(ch,20);
    return;
}

void do_throw( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    char arg[MSL];
    int dir=-1,rev=0;
    char buf[MSL];
    CHAR_DATA *victim=NULL;
    int x,y;

    argument = one_argument(argument,arg);
    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
    {
        send_to_char( "You do not carry that object.\n\r", ch );
        return;
    }
    if ( obj->value[4] != 1 )
    {
        send_to_char( "This is not a grenade!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "You must specify a direction or player to throw at.\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world(ch,argument) ) == NULL )
    {
        if ( ( dir = parse_direction(ch,argument) ) == -1 )
        {
            send_to_char( "Where do you want to throw the grenade to?\n\r", ch );
            return;
        }
    }
    obj_from_char(obj);
    free_string(obj->owner);
    obj->owner = str_dup(ch->name);
    if ( victim != NULL )
    {
        int range = 2;
        if ( ch->in_building && ch->in_building->type == BUILDING_SNIPER_TOWER )
            range += 3;
        if ( victim->x == ch->x && victim->y == ch->y )
        {
            send_to_char( "You are too scared to do that!\n\r", ch );
            return;
        }
        if ( abs(ch->x-victim->x) > range || abs(ch->y-victim->y) > range )
        {
            send_to_char( "They are too far.\n\r", ch );
            return;
        }
        if ( victim->y < 3 || victim->y >= MAX_MAPS - BORDER_SIZE || victim->x < 3 || victim->x >= MAX_MAPS - BORDER_SIZE || victim->z != ch->z )
        {
            send_to_char( "They are too far.\n\r", ch );
            return;
        }
        rev = -1;
        x = victim->x; y = victim->y;
        if ( number_percent() > ch->pcdata->skill[gsn_grenades] )
            x = number_range(x-1,x+1);
        if ( number_percent() > ch->pcdata->skill[gsn_grenades] )
            y = number_range(y-1,y+1);
        obj->x = x; obj->y = y;
        sprintf( buf, "You throw %s as hard as you can towards %s!", obj->short_descr, victim->name);
        act( buf, ch, NULL, NULL, TO_CHAR );
        sprintf( buf, "$n throws %s towards %s!", obj->short_descr, victim->name );
        act( buf, ch, NULL, NULL, TO_ROOM );
    }
    else
    {
        sprintf( buf, "You throw %s as hard as you can towards the %s!", obj->short_descr, compass_name[dir] );
        act( buf, ch, NULL, NULL, TO_CHAR );
        sprintf( buf, "$n throws %s towards the %s!", obj->short_descr, compass_name[dir] );
        act( buf, ch, NULL, NULL, TO_ROOM );
        if ( dir == DIR_NORTH )
        {
            obj->y++;
            rev = DIR_SOUTH;
        }
        else if ( dir == DIR_SOUTH )
        {
            obj->y--;
            rev = DIR_NORTH;
        }
        else if ( dir == DIR_EAST )
        {
            obj->x++;
            rev = DIR_WEST;
        }
        else
        {
            obj->x--;
            rev = DIR_EAST;
        }
    }
    obj_to_room(obj,ch->in_room);
    if ( rev <= -1 )
        rev = number_range(0,3);
    if ( ( bld = get_obj_building(obj) ) != NULL )
    {
        if ( (bld->exit[rev] && number_percent() < 10) || (!bld->exit[rev] && number_percent() < 30))
        {
            act( "$p hits the wall, and rolls back!", ch, obj, NULL, TO_CHAR );
            act( "$p hits the wall, and rolls back!", ch, obj, NULL, TO_ROOM );
            move_obj(obj,ch->x,ch->y,ch->z);
        }
        else
            send_to_loc("A grenade rolls into the room!\n\r", obj->x,obj->y,obj->z );
    }
    else
        send_to_loc("A grenade rolls into the room!\n\r", obj->x,obj->y,obj->z );
    obj->value[1] = 1;
    obj->value[0] = 3;
    WAIT_STATE(ch,5);
    return;
}

void do_activate( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *suit;

    if ( ( suit = get_eq_char( ch, WEAR_BODY ) ) == NULL )
    {
        send_to_char( "You are not wearing a suit!\n\r", ch );
        return;
    }
    if ( suit->item_type != ITEM_SUIT )
    {
        send_to_char( "This isn't a special suit.\n\r", ch );
        return;
    }
    if ( suit->value[1] <= 0 )
    {
        send_to_char( "This suit doesn't even work anymore!\n\r", ch );
        return;
    }
    if ( suit->value[0] == SUIT_WARP )
    {
        act( "You press a few buttons on your suit...", ch, NULL, NULL, TO_CHAR );
        act( "$n presses a few buttons on a suit...", ch, NULL, NULL, TO_ROOM );
        warp(ch,suit->value[1]*10);
    }
    else if ( suit->value[0] == SUIT_JUMP )
    {
        CHAR_DATA *vch;
        CHAR_DATA *victim = ch;
        bool found = FALSE;
        if ( ch->pcdata->alliance == -1 )
        {
            send_to_char( "It's hard to jump to another alliance member when you're not a member of one.\n\r", ch );
            return;
        }
        for ( vch = first_char;vch;vch = vch->next )
            if ( vch != ch && vch->pcdata->alliance == ch->pcdata->alliance && (vch->z == Z_GROUND || vch->z == Z_UNDERGROUND) && (!found || number_percent() < 50) )
        {
            victim = vch;
            found = TRUE;
        }
        if ( !found )
        {
            send_to_char( "No reachable alliance members found.\n\r", ch );
            return;
        }
        if ( ch->in_vehicle != NULL )
            act( "$t shimmers away!", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
        else
            act( "$n shimmers away!", ch, NULL, NULL, TO_ROOM );
        move(ch,victim->x,victim->y,victim->z);
        do_look(ch,"");
        if ( ch->in_vehicle != NULL )
            act( "$t shimmers into the room!", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
        else
            act( "$n shimmers into the room!", ch, NULL, NULL, TO_ROOM );
    }
    else
    {
        send_to_char( "This suit does nothing.\n\r", ch );
        return;
    }
    if ( number_percent() < 10 )
    {
        send_to_char( "Your suit appears a little damaged after its use.\n\r", ch );
        suit->value[1]--;
    }
    WAIT_STATE(ch,15);
    return;
}

void do_heal( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int heal;

    if ( ch->class == CLASS_MEDIC && argument[0] == '\0' )
    {
        CHAR_DATA *victim;
        char buf[MSL];
        if ( argument[0] == '\0' )
        {
            victim = ch;
        }
        else
        {
            if ( ( victim = get_char_room(ch,argument) ) == NULL )
            {
                send_to_char( "You can't find them here.\n\r", ch );
                return;
            }
        }
        if ( victim->hit == victim->max_hit )
        {
            send_to_char( "There's no need to heal.\n\r", ch );
            return;
        }
        heal = ch->max_hit / 10;
        if ( victim != ch )
        {
            if ( ch->hit <= heal )
            {
                send_to_char( "You do not have enough HP to heal that person.\n\r", ch );
                return;
            }
            ch->hit -= heal;
            sprintf(buf,"@@gYou heal $N! @@W(@@a%d@@W)@@N", heal );
            act( buf, ch, NULL, victim, TO_CHAR );
            sprintf(buf,"@@g%s heals you! @@W(@@a%d@@W)@@N\n\r", ch->name,heal );
            send_to_char(buf,victim);
            sprintf(buf,"@@g%s heals %s!", ch->name,victim->name );
            act( buf, ch, NULL, victim, TO_NOTVICT );
        }
        else
        {
            sprintf(buf,"@@gYou heal yourself! @@W(@@a%d@@W)@@N", heal );
            send_to_char(buf,ch);
            act( "$n heals $mself.", ch, NULL, NULL, TO_ROOM );
        }
        victim->hit += heal;
        if ( victim->hit > victim->max_hit )
            victim->hit = victim->max_hit;
        sendsound(ch,"energy1",40,1,25,"misc","energy1.wav");
        WAIT_STATE(ch,16);
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: heal <medpack to use>\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry(ch,  argument) ) == NULL )
         	{
        send_to_char( "You are not carrying that!\n\r", ch );
        return;
        }
    if ( obj->item_type != ITEM_MEDPACK )
{
                   
                 if (obj->pIndexData->vnum==1500) //beer
{
                                   extract_obj(obj);
                                   act("$n has another beer; what an alcoholic.",ch,NULL,NULL,TO_ROOM);
                                   if (!IS_SET(ch->effect,EFFECT_DRUNK))
                                   {
SET_BIT(ch->effect,EFFECT_DRUNK);                                                                        
                                                                        mreturn("You can't help chugging the damned thing. Woooooooo!\n\rYou feel tipsy.\n\r",ch);
}
else if (IS_SET(ch->effect,EFFECT_DRUNK) && !IS_SET(ch->effect,EFFECT_CONFUSE))
{
     send_to_char("What the hell; you slam another one!\n\r",ch);
     if (number_percent()<40)
     {
     SET_BIT(ch->effect,EFFECT_CONFUSE);
     act("$n looks a little unsteady.",ch,NULL,NULL,TO_ROOM);
send_to_char("You feel drunker somehow... Woooooo!\n\r",ch);
}
return;
}
else if (IS_SET(ch->effect,EFFECT_DRUNK) && IS_SET(ch->effect,EFFECT_CONFUSE))
{
switch (number_range(1,10))
{
case 1:
     send_to_char("You're hick-upping too damn bad right now, but you manage another one.\n\r",ch);
     break;
     case 2:
          send_to_char("If you have another one, you'll surely puke; but why not?\n\r",ch);
          break;
          case 3:
               do_drop(ch,"all");
               send_to_char("Woops... Hehhehheheh...\n\r",ch);
               break;
               case 4:
                    send_to_char("Yes ma'am.\n\r",ch);
                    break;
                    case 5:
                         send_to_char("I amaze myself; don't you?\n\r",ch);
                         break;       
case 6:
     send_to_char("Zhure...\n\r",ch);
     break;
     case 7:
          send_to_char("Iv u wanna... Yeah...\n\r",ch);
          break;
          case 8:
               send_to_char("I'm so zhrunk... Wooooo!\n\r",ch);
               break;
               case 9:
                    send_to_char("I's love... Beer... Zhyeah... Have another one...\n\r",ch);
                    break;
                    case 10:
                         send_to_char("Ozh kayze...\n\r",ch);
                         break;
                         }
                         return;
                         }
}
else
{
                 send_to_char( "This isn't a Medpack!\n\r", ch );
        return;
    }
}
    if ( obj->value[1] == 1 )
    {
        if ( ch->disease == 0 )
        {
            send_to_char( "You are not infected by anything.\n\r", ch );
            return;
        }
        send_to_char( "You inject yourself with the antidote, and feel much better.\n\r", ch );
        ch->disease = 0;
        sendsound(ch,"energy1",40,1,25,"misc","energy1.wav");
        return;
    }
    else if ( obj->value[1] == 2 )
    {
        bool ex = TRUE;
        act( "You use $p to heal your injuries.", ch, obj, NULL, TO_CHAR );
        act( "$n uses $p to heal some injuries.", ch, obj, NULL, TO_ROOM );
        send_to_char( "It was poisonous!!\n\r", ch );
        heal = number_range(obj->level*obj->value[0]/2, obj->level*obj->value[0]);
        if ( heal > ch->hit )
            ex = FALSE;
        damage(ch,ch,heal,DAMAGE_PSYCHIC);
        if ( ex )
            extract_obj(obj);
        sendsound(ch,"energy1",40,1,25,"misc","energy1.wav");
        return;
    }
    heal = number_range(obj->level*obj->value[0]/2, obj->level*obj->value[0]);
    ch->hit = URANGE(0,ch->hit + heal, ch->max_hit);
    act( "You use $p to heal your injuries.", ch, obj, NULL, TO_CHAR );
    act( "$n uses $p to heal some injuries.", ch, obj, NULL, TO_ROOM );
    WAIT_STATE(ch,20);
    extract_obj(obj);
    sendsound(ch,"energy1",40,1,25,"misc","energy1.wav");
    return;
}

void do_demolis( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to DEMOLISH your building, type the full word.\n\r", ch);
    return;
}

void do_demolish( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    BUILDING_DATA *bld2;
    bool check = TRUE;
    char arg[MSL];
    argument = one_argument(argument,arg);

    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must be in a building.\n\r", ch );
        return;
    }
    if ( ch->fighttimer > 0)
    {
        send_to_char( "Not during combat.\r\n", ch );
        return;
    }
    if ( str_cmp(bld->owned,ch->name) )
    {
        send_to_char( "This isn't your building!\n\r", ch );
        return;
    }
    if ( is_neutral(bld->type) )
    {
        if ( IS_IMMORTAL(ch) && !str_cmp(argument,"neutral") )
        {
            send_to_char( "Overriding neutral building...\n\r", ch );
        }
        else
        {
            send_to_char( "You can't demolish neutral buildings.\n\r", ch );
            return;
        }
    }
    if ( !str_cmp(arg,"all") )
    {
        BUILDING_DATA *bld2_next;
        char buf[MSL];
        int i;
        int type=0;

        if ( ch->fighttimer > 0 )
        {
            send_to_char( "Not during combat.\n\r", ch );
            return;
        }

        if ( build_table[bld->type].act == BUILDING_LAB )
        {
            for ( bld2 = ch->first_building;bld2;bld2 = bld2_next )
            {
                bld2_next = bld2->next_owned;
                type = build_table[bld2->type].requirements;
                if ( type > 0 )
                {
                    if ( type == bld->type || build_table[type].requirements == bld->type )
                    {
                        check_building_destroyed(bld2);
                        extract_building(bld2,TRUE);
                        continue;
                    }
                }
            }
            send_to_char( "Your lab buildings have been demolished. You must wait 20 seconds.\n\r", ch );
            WAIT_STATE(ch,20*8);
            return;
        }
        if ( bld->type != BUILDING_HQ )
        {
            send_to_char( "You can only do this from the HQ.\n\r", ch );
            return;
        }
        if ( argument[0] == '\0' )
        {
            type = -1;
            sprintf ( buf, "Demolishing all buildings.\n\r" );
        }
        else
        {
            for ( i=1;i<MAX_BUILDING;i++ )
            {
                if ( !str_prefix(argument,build_table[i].name) )
                {
                    type = i;
                    sprintf( buf, "Demolishing all \"%s\"\n\r", build_table[i].name );
                    break;
                }
            }
        }
        if ( type == 0 )
        {
            send_to_char( "Syntax: Demolish All\n\r        Demolish All <building name>\n\r", ch );
            return;
        }
        send_to_char(buf,ch);
        for ( bld2 = ch->first_building;bld2;bld2 = bld2_next )
        {
            bld2_next = bld2->next_owned;

            if ( is_neutral(bld2->type) )
                continue;
            if ( bld->value[3] != 0 )
                continue;
            if ( bld2 == ch->in_building )
                continue;
            if ( type == -1 || type == bld2->type )
            {
                check_building_destroyed(bld2);
                extract_building(bld2,TRUE);
            }
        }
        send_to_char( "Your base has been demolished.\n\rYou must wait 20 seconds.\n\r", ch );
        WAIT_STATE(ch,20*8);
        return;
    }
    for ( bld2 = ch->first_building;bld2;bld2 = bld2->next_owned )
        if ( bld2 != bld && bld2->type == bld->type && complete(bld2) )
            check = FALSE;

    if ( !complete(bld) )
        check = FALSE;

    if ( bld->value[3] != 0 && bld->type != BUILDING_DUMMY && bld->type != BUILDING_PORTAL )
    {
        CHAR_DATA *vch;
        if ( (vch= get_ch(bld->attacker) ) == NULL || vch == ch )
        {
            damage_building(vch,bld,bld->maxhp*2);
            return;
        }
    }

    act( "You enter a code, and the building collapses!", ch, NULL, NULL, TO_CHAR );
    act( "$n enters a code, and the building collapses!", ch, NULL, NULL, TO_ROOM );
    if ( my_get_hours(ch,TRUE) < 5 && complete(bld) )
    {
        OBJ_DATA *obj;
        int i;
        if ( IS_SET(bld->value[1],INST_QP) )
        {
            send_to_char( "@@aThis building was completed with quest points, you won't get anything back.@@N\n\r", ch );
        }
        else
        {
            for ( i=0;i<8;i++ )
                if ( build_table[bld->type].resources[i] > 0 )
            {
                obj = create_material(i);
                obj->value[1] = build_table[bld->type].resources[i];
                obj_to_char(obj,ch);
            }
            send_to_char( "@@aFor the first 5 hours of gameplay, you are refunded for demolished buildings.@@N\n\r", ch );
        }
    }
    if ( check )
        check_building_destroyed(bld);
    extract_building(bld,TRUE);
    return;
}

void do_securit( CHAR_DATA *ch, char *argument )
{
    send_to_char( "You must type SECURITY to turn it on or off.\n\r", ch );
    return;
}

void do_security( CHAR_DATA *ch, char *argument )
{
    if ( ch->security )
    {
        send_to_char( "Security turned OFF!!!\n\r", ch);
        ch->security = FALSE;
    }
    else
    {
        send_to_char( "Security turned back on.\n\r", ch );
        ch->security = TRUE;
    }
    return;
}

void do_mine( CHAR_DATA *ch, char *argument )
{
    int type = -1;
    BUILDING_DATA *bld;
    bool mine = TRUE;

    if ( ch->z == Z_SPACE )
    {
        space_mine(ch,argument);
        return;
    }
    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must be in a mine.\n\r", ch );
        return;
    }
    if ( bld->type != BUILDING_MINE && bld->type != BUILDING_IMPROVED_MINE && bld->type != BUILDING_LUMBERYARD )
    {
        send_to_char( "You must be in a mine or lumberyard.\n\r", ch );
        return;
    }
    if ( bld->type == BUILDING_LUMBERYARD )
        mine = FALSE;
    if ( bld->level < 3 )
    {
        send_to_char( "You must upgrade the building to level 3 before you can direct its mining priority.\n\r", ch);
        return;
    }

    if ( mine )
    {
        if ( !str_cmp(argument, "iron") )
            type = 0;
        else if (!str_cmp(argument, "copper") )
            type = 2;
        else if (!str_cmp(argument, "silver") )
            type = 4;
        else if (!str_cmp(argument, "gold") )
            type = 3;
        else if ( !str_cmp(argument, "none") )
            type = -1;
        else
        {
            send_to_char( "Invalid Type. Valid types are: Iron, Copper, Silver, Gold, None.\n\r", ch );
            return;
        }
    }
    else
    {
        if (!str_cmp(argument, "logs") || !str_cmp(argument, "trees") )
            type = ITEM_LOG;
        else if (!str_cmp(argument, "sticks") )
            type = ITEM_STICK;
        else if ( !str_cmp(argument, "none") )
            type = -1;
        else
        {
            send_to_char( "Invalid Type. Valid types are: Logs, Sticks, None.\n\r", ch );
            return;
        }

    }

    bld->value[0] = type;
    send_to_char( "Mining priority set.\n\r", ch);
    return;
}

void do_chunk( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    OBJ_DATA *obj2_next;
    bool found = FALSE;
    BUILDING_DATA *bld;
    bool type[8];
    int resource[8];
    int i;

    for ( i = 0;i<8;i++ )
    {
        type[i] = FALSE;
        resource[i] = 0;
    }
    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must be at a forge!\n\r", ch);
        return;
    }
    if ( bld->type != BUILDING_FORGE )
    {
        send_to_char( "You must be in a forge!\n\r", ch);
        return;
    }
    if ( !complete(bld) )
    {
        send_to_char( "This forge is not completed yet.\n\r", ch);
        return;
    }
    if ( bld->value[3] < 0 )
    {
        send_to_char( "It's corrupted by a virus!\n\r", ch );
        return;
    }
    if ( !str_cmp(argument,"all") )
    {
        OBJ_DATA *obj_next;
        int x;
        for ( obj = ch->first_carry;obj;obj = obj_next )
        {
            obj_next = obj->next_in_carry_list;
            if ( obj->item_type != ITEM_MATERIAL || obj->value[0] < 0 || obj->value[0] > 7 )
                continue;
            type[obj->value[0]] = TRUE;
            resource[obj->value[0]] += obj->value[1];
            found = TRUE;
            extract_obj(obj);
        }
        for ( i = 0;i<8;i++ )
        {
            if ( resource[i] <= 0 )
                continue;
            for ( x = resource[i];x>0;x -= 20000)
            {
                obj = create_material(i);
                obj->value[1] = x;
                if ( obj->value[1] > 20000 )
                    obj->value[1] = 20000;
                obj->weight = obj->weight * obj->value[1];
                if ( obj->weight > MAX_CHUNK_WEIGHT - (bld->level * 10) )
                    obj->weight = MAX_CHUNK_WEIGHT  - ( bld->level * 10 ) ;
                obj_to_char(obj,ch);
            }
        }
        if ( !found )
            send_to_char( "You have no more materials to chunk!\n\r", ch);
        else
            send_to_char( "All materials chunked!\n\r", ch);
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You do not carry that object.\n\r", ch);
        return;
    }
    if ( obj->item_type != ITEM_MATERIAL )
    {
        send_to_char( "You can only chunk materials.\n\r", ch);
        return;
    }
    for ( obj2 = ch->first_carry;obj2;obj2 = obj2_next )
    {
        obj2_next = obj2->next_in_carry_list;
        if ( obj2->item_type != ITEM_MATERIAL || obj2->value[0] != obj->value[0] || obj2 == obj )
            continue;
        if ( obj->value[1] + obj2->value[1] > 20000 )
            continue;
        obj->value[1] += obj2->value[1];
        obj->weight = URANGE(0,(obj2->weight + obj->weight),105-(bld->level*5));
        extract_obj(obj2);
        found = TRUE;
    }
    if ( !found )
        send_to_char( "You have no more materials of that type!\n\r", ch);
    else
        send_to_char( "Materials chunked!\n\r", ch);

    ch->carry_weight = 0;
    for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
        ch->carry_weight += obj->weight;
    return;
}

/* void do_repair( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    BUILDING_DATA *bld;

    if ( ( obj = get_obj_carry(ch, argument) ) == NULL )
    {
        send_to_char( "You do not carry that object.\n\r", ch);
        return;
    }
    if ( obj->item_type != ITEM_ARMOR )
    {
        send_to_char( "You can only repair armor.\n\r", ch);
        return;
    }
    if ( obj->value[1] <= 0 )
    {
        send_to_char( "That item doesn't need to be repaired.\n\r", ch);
        return;
    }
    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must be in an armory to repair.\n\r", ch);
        return;
    }
    if ( bld->type != BUILDING_ARMORY )
    {
        send_to_char( "You must be in an armory to repair.\n\r", ch);
        return;
    }
    if ( !complete(bld) )
    {
        send_to_char( "This armory isn't completed.\n\r", ch);
        return;
    }
        if ( bld->value[3] < 0 )
        {
                send_to_char( "It's corrupted by a virus!\n\r", ch );
                return;
        }
    if ( str_cmp(bld->owned,ch->name) )
    {
        send_to_char( "This isn't your armory!\n\r", ch);
        return;
    }
    obj->value[1] = 0;
    act( "You repair $p.", ch, obj, NULL, TO_CHAR );
    act( "$n repairs $p.", ch, obj, NULL, TO_ROOM );
    return;
} */

void do_install( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    char buf[MSL];

    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must install something in a building.\n\r", ch);
        return;
    }

    if ( bld->type == BUILDING_SPACE_CENTER )
    {
        send_to_char( "Nothing can be installed here.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "This building has been installed with:\n\r\n\r", ch );
        if ( IS_SET(bld->value[1], INST_GPS ) )         send_to_char( "GPS Locating System\n\r", ch );
        if ( IS_SET(bld->value[1], INST_REFLECTOR ) )       send_to_char( "Bomb Reflector System\n\r", ch );
        if ( IS_SET(bld->value[1], INST_SATELLITE_UPLINK ) )    send_to_char( "Satellite Uplink\n\r", ch );
        if ( IS_SET(bld->value[1], INST_INTERN_DEF ) )      send_to_char( "Internal Defense Upgrade\n\r", ch );
        if ( IS_SET(bld->value[1], INST_LASER_AIMS ) )      send_to_char( "Laser Aims\n\r", ch );
        if ( IS_SET(bld->value[1], INST_SAFEHOUSE ) )       send_to_char( "Safehouse Upgrade\n\r", ch );
        if ( IS_SET(bld->value[1], INST_PULSE_NEUTRALIZER ) )   send_to_char( "Pulse Neutralizer\n\r", ch );
        if ( IS_SET(bld->value[1], INST_DEPLEATED_URANIUM ) )   send_to_char( "Depleated Uranium\n\r", ch );
        if ( IS_SET(bld->value[1], INST_RESOURCE_PURIFIER ) )   send_to_char( "Resource Purifier\n\r", ch );
        if ( IS_SET(bld->value[1], INST_ANTIVIRUS ) )       send_to_char( "Antivirus\n\r", ch );
        if ( IS_SET(bld->value[1], INST_FIREWALL ) )        send_to_char( "Firewall\n\r", ch );
        if ( IS_SET(bld->value[1], INST_STUN_GUN ) )        send_to_char( "Stun Gun\n\r", ch );
        if ( IS_SET(bld->value[1], INST_PROCESSOR_UPGRADE ) )   send_to_char( "Processor Upgrade\n\r", ch );
        if ( IS_SET(bld->value[1], INST_ORGANIC_CORE ) )    send_to_char( "Organic Core\n\r", ch );
        if ( IS_SET(bld->value[1], INST_VIRAL_ENHANCER ) )  send_to_char( "Viral Enhancer\n\r", ch );
        if ( IS_SET(bld->value[1], INST_ALIEN_TECHNOLOGY ) )    send_to_char( "Alien Technology\n\r", ch );
        if ( IS_SET(bld->value[1], INST_ACID_DEFENSE ) )    send_to_char( "Acid Defenses\n\r", ch );
        if ( IS_SET(bld->value[1], INST_ALIEN_HIDES ) )     send_to_char( "Alien Hides\n\r", ch );
        if ( GUNNER(bld) )
        {
            if ( IS_SET(bld->value[5], GUNNER_NUCLEAR ) )       send_to_char( "Gunner: Nuclear\n\r", ch );
            if ( IS_SET(bld->value[5], GUNNER_POISON ) )        send_to_char( "Gunner: Poison\n\r", ch );
            if ( IS_SET(bld->value[5], GUNNER_ROCKETS ) )       send_to_char( "Gunner: Rockets\n\r", ch );
            if ( IS_SET(bld->value[5], GUNNER_PSYCHIC ) )       send_to_char( "Gunner: Psychic\n\r", ch );
            if ( IS_SET(bld->value[5], GUNNER_CHAOS ) )         send_to_char( "Gunner: Chaos\n\r", ch );
            if ( IS_SET(bld->value[5], GUNNER_TRACER ) )        send_to_char( "Gunner: Tracer\n\r", ch );
        }
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You do not have that installation part.\n\r", ch );
        return;
    }

    if ( obj->item_type != ITEM_INSTALLATION )
    {
        send_to_char( "You can't install this.\n\r", ch );
        return;
    }
    if ( obj->value[1] != bld->type
        && obj->value[2] != bld->type
        && obj->value[3] != bld->type
        && obj->value[4] != bld->type
        && obj->value[1] != -1 )
    {
        send_to_char( "This upgrade cannot be installed here.\n\r", ch );
        return;
    }

    if ( obj->value[0] == -1 )                              //Gunner upgrades
    {
        int bit = get_bit_value(obj->value[5]);
        if ( !GUNNER(bld) )
        {
            send_to_char( "You can't install that here.\n\r", ch );
            return;
        }
        if (IS_SET(bld->value[5],bit ))
        {
            send_to_char( "You have already installed this here.\n\r", ch );
            return;
        }
        SET_BIT(bld->value[5],bit);
        sprintf( buf, "You install %s into %s's system!\n\r", obj->short_descr, bld->name );
        send_to_char( buf, ch );
        sprintf( buf, "$n installs %s into %s's system!\n\r", obj->short_descr, bld->name );
        act( buf, ch, NULL, NULL, TO_ROOM );
        extract_obj(obj);
        return;
    }

    if ( obj->pIndexData->vnum == 11 )                      // Dr Norton
    {
        sprintf(buf,"%s", bld->owned );
        for ( bld = first_building;bld;bld = bld->next )
            if ( !str_cmp(bld->owned,buf) )
                bld->value[3] = 0;
        send_to_char( "Dr. Norton fixed your base right up!\n\r", ch );
        extract_obj(obj);
        return;
    }
    if ( IS_SET(bld->value[1], obj->value[0]) )
    {
        send_to_char( "You have already installed this here.\n\r", ch );
        return;
    }

    sprintf( buf, "You install %s into %s's system!\n\r", obj->short_descr, bld->name );
    send_to_char( buf, ch );
    sprintf( buf, "$n installs %s into %s's system!\n\r", obj->short_descr, bld->name );
    act( buf, ch, NULL, NULL, TO_ROOM );
    SET_BIT( bld->value[1], obj->value[0] );
    extract_obj(obj);
    return;
}

void do_implant( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    BUILDING_DATA *bld;
    int bit_value;

    if ( ( bld = ch->in_building ) == NULL || !complete(bld) || str_cmp(bld->owned,ch->name) || bld->type != BUILDING_IMPLANT_RESEARCH )
    {
        send_to_char( "You must be in one of your completed implant research facilities.\n\r", ch );
        return;
    }
    if ( bld->value[3] < 0 )
    {
        send_to_char( "It's corrupted by a virus!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        char buf[MSL];
        //		send_to_char( "Implant what?\n\r", ch );
        send_to_char( "Implants currently in you:\n\r", ch );
        buf[0] = '\0';
        if ( IS_SET(ch->implants,IMPLANT_METAL_ABS_3) )
            safe_strcat(MSL,buf,"Abs lvl 3");
        else if ( IS_SET(ch->implants,IMPLANT_METAL_ABS_2) )
            safe_strcat(MSL,buf,"Abs lvl 2");
        else if ( IS_SET(ch->implants,IMPLANT_METAL_ABS_1) )
            safe_strcat(MSL,buf,"Abs lvl 1");
        if ( IS_SET(ch->implants,IMPLANT_METAL_ARM_3) )
            safe_strcat(MSL,buf,"Arm lvl 3");
        else if ( IS_SET(ch->implants,IMPLANT_METAL_ARM_2) )
            safe_strcat(MSL,buf,"Arm lvl 2");
        else if ( IS_SET(ch->implants,IMPLANT_METAL_ARM_1) )
            safe_strcat(MSL,buf,"Arm lvl 1");
        if ( IS_SET(ch->implants,IMPLANT_METAL_CHEST_3) )
            safe_strcat(MSL,buf,"Chest lvl 3");
        else if ( IS_SET(ch->implants,IMPLANT_METAL_CHEST_2) )
            safe_strcat(MSL,buf,"Chest lvl 2");
        else if ( IS_SET(ch->implants,IMPLANT_METAL_CHEST_1) )
            safe_strcat(MSL,buf,"Chest lvl 1");

        send_to_char(buf,ch);
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You do not carry that implant.\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_IMPLANT )
    {
        send_to_char( "This is not an implant.\n\r", ch );
        return;
    }

    bit_value = get_bit_value(obj->value[3]);
    if ( IS_SET(ch->implants,bit_value) )
    {
        send_to_char( "You have already installed this implant.\n\r", ch );
        return;
    }

    if ( obj->value[0] == 0 )                               //HP Implant
    {
        if ( ch->max_hit > obj->value[2] )
        {
            send_to_char( "That implant will have no effect.\n\r", ch );
            return;
        }
        if ( ch->max_hit < obj->value[1] && ch->class != CLASS_ROBOTIC )
        {
            send_to_char( "You are not qualified to use that implant yet.\n\r", ch );
            return;
        }
        ch->max_hit = obj->value[2];
    }
    else if ( obj->value[0] == 1 )                          // Carry weight implant
    {
        if ( can_carry_w(ch) > obj->value[2] )
        {
            send_to_char( "That implant will have no effect.\n\r", ch );
            return;
        }
        if ( can_carry_w(ch) < obj->value[1] && ch->class != CLASS_ROBOTIC )
        {
            send_to_char( "You are not qualified to use that implant yet.\n\r", ch );
            return;
        }
    }
    else if ( obj->value[0] == 2 )                          // Carry number implant
    {
        if ( can_carry_n(ch) > obj->value[2] )
        {
            send_to_char( "That implant will have no effect.\n\r", ch );
            return;
        }
        if ( can_carry_n(ch) < obj->value[1] && ch->class != CLASS_ROBOTIC )
        {
            send_to_char( "You are not qualified to use that implant yet.\n\r", ch );
            return;
        }
    }
    extract_obj(obj);
    act( "You implant $p in your body!", ch, obj, NULL, TO_CHAR );
    act( "$n implants $p in $s body!", ch, obj, NULL, TO_ROOM );
    if ( !IS_SET(ch->implants,bit_value));
    SET_BIT(ch->implants,bit_value);
    return;
}

void do_qpspend( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    OBJ_INDEX_DATA *pObj;
    int x,y,i,cost;
    BUILDING_DATA *bld;
    CHAR_DATA *vch;
    char arg[MSL];
    char arg2[MSL];
    char buf[MSL];

    argument = one_argument(argument,arg);
    argument = one_argument(argument,arg2);
    if ( medal(ch) )
    {
        send_to_char( "Not here.\n\r", ch );
        return;
    }
if (!str_cmp(arg,"norton"))
{
                        if (ch->quest_points<500)
                        mreturn("You need 500 quest points to buy a Norton.\n\r",ch);
obj = create_object(get_obj_index(11),0);
obj_to_char(obj,ch);
ch->quest_points-=500;
mreturn("You buy yourself a Norton installation for 500 quest points!\n\r",ch);
}

    if ( !str_cmp(arg,"buy") )
    {
        if ( !str_cmp(arg2,"list") || arg2[0] == '\0' )
        {
            int length = ch->pcdata->pagelen;
            buf[0] = '\0';

            if ( length > 100 )
                ch->pcdata->pagelen = 100;
            for ( i=MIN_LOAD_OBJ;i<MAX_LOAD_OBJ;i++ )
            {
                if ( i == 1043 || i == OBJ_VNUM_ACID_SPRAY || i == 1062)
                    continue;
                if ( (pObj = get_obj_index(i) ) == NULL )
                    continue;
                if ( pObj->item_type != ITEM_WEAPON && pObj->item_type != ITEM_BOMB && pObj->item_type != ITEM_ARMOR && pObj->item_type != ITEM_MEDPACK )
                    continue;
                if ( pObj->item_type == ITEM_ARMOR && pObj->value[0] == -2 )
                    continue;
                if ( pObj->item_type == ITEM_WEAPON && pObj->value[2] == 21 )
                    continue;
                if ( pObj->item_type == ITEM_BOMB && pObj->value[4] == 1 )
                    continue;
                if ( IS_SET(pObj->extra_flags,ITEM_NOQP) )
                    continue;
                sprintf( buf+strlen(buf), "@@g[@@W%4d@@g] [@@a%4d@@g] @@N%-25s\n\r", i, pObj->level, pObj->short_descr );
            }
            for ( i=MIN_QUEST_OBJ;i<=MAX_QUEST_OBJ;i++ )
            {
                if ( ( pObj = get_obj_index(i)) == NULL || i == 9 || i == 12 || i == 11 )
                    continue;
                sprintf( buf+strlen(buf), "@@g[@@W%4d@@g] [@@a%4d@@g] @@N%-25s\n\r", i, QUEST_ITEM_COST, pObj->short_descr );
            }
            send_to_char( buf, ch );
            ch->pcdata->pagelen = length;
            return;
        }
        else if ( !str_prefix(arg2,"listweapons") )
        {

            int length = ch->pcdata->pagelen;
            buf[0] = '\0';

            if ( length > 100 )
                ch->pcdata->pagelen = 100;
            for ( i=MIN_LOAD_OBJ;i<MAX_LOAD_OBJ;i++ )
            {
                if ( i == 1043 || i == OBJ_VNUM_ACID_SPRAY )
                    continue;
                if ( (pObj = get_obj_index(i) ) == NULL )
                    continue;
                if ( pObj->item_type != ITEM_WEAPON || pObj->value[2] == 21 )
                    if ( pObj->item_type != ITEM_BOMB )
                        continue;
                if ( pObj->item_type == ITEM_BOMB && pObj->value[4] == 1 )
                    continue;
                if ( IS_SET(pObj->extra_flags,ITEM_NOQP) )
                    continue;
                sprintf( buf+strlen(buf), "@@g[@@W%4d@@g] [@@a%4d@@g] @@N%-25s\n\r", i, pObj->level, pObj->short_descr );
            }
            for ( i=MIN_QUEST_OBJ;i<=MAX_QUEST_OBJ;i++ )
            {
                if ( ( pObj = get_obj_index(i)) == NULL || i == 9 || i == 12 || i == 11)
                    continue;
                if ( pObj->item_type != ITEM_WEAPON )
                    continue;
                if ( IS_SET(pObj->extra_flags,ITEM_NOQP) )
                    continue;
                sprintf( buf+strlen(buf), "@@g[@@W%4d@@g] [@@a%4d@@g] @@N%-25s\n\r", i, QUEST_ITEM_COST, pObj->short_descr );
            }
            send_to_char( buf, ch );
            ch->pcdata->pagelen = length;
            return;
        }
        else if ( !str_prefix(arg2, "listarmor") )
        {
            do_oarmortype(ch,"");
            return;
        }
        else if ( is_number(arg2) )
        {
            i = atoi(arg2);
            int time = my_get_minutes(ch,FALSE);
            if ( COMBAT_LAG(ch) )
            {
                send_to_char( "Not during combat.\n\r", ch );
                return;
            }
            if ( time <= 9 && !sysdata.killfest )
            {
                sprintf( buf, "You must wait another %d minutes before you can buy again.\n\rGo to your base to restock.\n\r", 9-time );
                send_to_char( buf,ch);
                return;
            }
            if ( ( ( i < MIN_LOAD_OBJ || i > MAX_LOAD_OBJ ) && ( i < MIN_QUEST_OBJ || i > MAX_QUEST_OBJ ) ) || i == 9 || i == 12 || i == 1062 || i == 11 )
            {
                sprintf( buf, "Valid object numbers are %d to %d, and %d to %d.\n\r", MIN_LOAD_OBJ, MAX_LOAD_OBJ, MIN_QUEST_OBJ, MAX_QUEST_OBJ );
                send_to_char( buf, ch );
                return;
            }
            if ( get_obj_index(i) == NULL )
            {
                send_to_char( "Object doesn't exist.\n\r", ch );
                return;
            }
            if ( ( obj = create_object(get_obj_index(i),0) ) == NULL )
            {
                send_to_char( "Object doesn't exist.\n\r", ch );
                return;
            }
            if ( obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_BOMB && obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_MEDPACK )
            {
                if ( i < MIN_QUEST_OBJ || i > MAX_QUEST_OBJ )
                {
                    send_to_char( "You can only buy weapons, bombs and armor... For now.\n\r", ch );
                    return;
                }
            }
            if ( obj->item_type == ITEM_BOMB && obj->value[4] == 1 )
            {
                send_to_char( "You can't buy this.\n\r", ch );
                return;
            }
            if ( ( obj->item_type == ITEM_ARMOR && obj->value[0] == -2 ) || i == OBJ_VNUM_ACID_SPRAY || i == 1043 || ( obj->item_type == ITEM_WEAPON && obj->value[2] == 21 ))
            {
                send_to_char( "You can't buy this.\n\r", ch );
                return;
            }
            if ( IS_SET(obj->extra_flags,ITEM_NOQP) )
            {
                send_to_char( "You can't buy this.\n\r", ch );
                return;
            }
            if ( i == 1062 )
                obj->level = 200;
            if ( ( i >= MIN_QUEST_OBJ && i <= MAX_QUEST_OBJ && ch->quest_points < QUEST_ITEM_COST ) || ch->quest_points < obj->level )
            {
                sprintf( buf, "Object cost is %d. You only have %d questpoints.\n\r", obj->level, ch->quest_points );
                if ( i >= MIN_QUEST_OBJ && i < MAX_QUEST_OBJ && ch->quest_points < QUEST_ITEM_COST )
                    sprintf( buf, "Object cost is %d. You only have %d questpoints.\n\r", QUEST_ITEM_COST, ch->quest_points );
                send_to_char( buf, ch );
                return;
            }
            obj_to_char( obj, ch );
            if ( i >= MIN_QUEST_OBJ && i <= MAX_QUEST_OBJ)
                ch->quest_points -= QUEST_ITEM_COST;
            else
                ch->quest_points -= obj->level;

            sprintf( buf, "%s bought for %d QPs!\n\r", obj->short_descr, ( i >= MIN_QUEST_OBJ && i <= MAX_QUEST_OBJ ) ? QUEST_ITEM_COST : obj->level );
            send_to_char( buf, ch );
            return;
        }
        else
        {
            send_to_char( "Syntax:\n\r\n\rQPSpend buy list\n\rQPSpend buy listarmor\n\rQPSpend buy listweapons\n\rQPSpend buy <obj #>\n\rQPSpend transport <x> <y>\n\rQPSpend move <x> <y>\n\rQPSpend repair <object> <amount>\n\rQPSpend complete\n\rQPSpend blueprint\n\rQPSpend clip <weapon> <clip size to add>\n\rQPSpend atom\n\rQPSpend norton\r\n", ch );
            return;
        }
    }
    else if ( !str_cmp(arg,"transport") || !str_cmp(arg,"move") )
    {
        bool pit = FALSE;
        bool trap = FALSE;
        bool mmove = FALSE;

        if ( ch->z == Z_UNDERGROUND || ch->z == Z_AIR || ch->z == Z_SPACE || ch->z == Z_PAINTBALL )
        {
            send_to_char( "You can't do that here.\n\r", ch );
            return;
        }
        if ( my_get_minutes(ch,FALSE) <= 4 && !sysdata.killfest )
        {
            send_to_char( "And mass-attack the poor guy? Walk there yourself.\n\r", ch );
            return;
        }
        if ( !str_cmp(arg,"move") )
            mmove = TRUE;
        else
        if ( ch->fighttimer > 0 )
        {
            send_to_char( "Not during combat.\n\r", ch );
            return;
        }

        if ( ch->in_vehicle && SPACE_VESSAL(ch->in_vehicle) )
        {
            send_to_char( "You can't do that while inside a spaceship.\n\r", ch );
            return;
        }
        if ( IN_PIT(ch) )
            pit = TRUE;
        if ( paintball(ch) )
            return;
        if ( ( bld = ch->in_building ) == NULL && mmove )
        {
            send_to_char( "You must be inside the building you wish to move.\n\r", ch );
            return;
        }
        if ( !is_number(arg2) || !is_number(argument) )
        {
            send_to_char( "Coordinates must be numerical.\n\r", ch );
            return;
        }
        x = atoi(arg2);
        y = atoi(argument);
        if ( ch->z == Z_PAINTBALL )
            if ( ( !pit && x >= PIT_BORDER_X && y >= PIT_BORDER_Y )
            || ( pit  && x <= PIT_BORDER_X && y <= PIT_BORDER_Y ) )
        {
            send_to_char( "You can't transport in and out of the pit.\n\r", ch );
            return;
        }
        if ( ch->x == x && ch->y == y )
        {
            send_to_char( "That won't do much.\n\r", ch );
            return;
        }
        if ( x < BORDER_SIZE || x > MAX_MAPS-BORDER_SIZE || y < BORDER_SIZE || y > MAX_MAPS-BORDER_SIZE )
        {
            send_to_char( "Invalid Coordinates.\n\r", ch );
            return;
        }
        if ( map_table.type[x][y][ch->z] == SECT_NULL )
        {
            send_to_char( "Invalid Coordinates.\n\r", ch );
            return;
        }
        if ( ch->in_vehicle && !AIR_VEHICLE(ch->in_vehicle->type) && map_table.type[x][y][1] == SECT_WATER && !mmove)
        {
            send_to_char( "You can't teleport tanks into water.\n\r", ch );
            return;
        }
        if ( !mmove )
            cost = ( IS_BETWEEN(x,ch->x-100,ch->x+100) && IS_BETWEEN(y,ch->y-100,ch->y+100) ) ? 30 : ( IS_BETWEEN(x,ch->x-300,ch->x+300) && IS_BETWEEN(y,ch->y-300,ch->y+300) ) ? 50 : 70;
        else
        {
            if ( str_cmp(bld->owned,ch->name) || is_neutral(bld->type) )
            {
                send_to_char( "You can only move your own buildings.\n\r", ch );
                return;
            }
            cost = bld->level * 25;
        }
        if ( ch->quest_points < cost )
        {
            sprintf( buf, "Cost is %d. You only have %d quest points.\n\r", cost, ch->quest_points );
            send_to_char( buf, ch );
            return;
        }
        if ( mmove )
        {
            BUILDING_DATA *bld2;
            int xx,yy;
            extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
            if ( is_neutral(bld->type) )
            {
                send_to_char( "You can't move neutral buildings.\n\r", ch );
                return;
            }
            if ( GUNNER(bld) || build_table[bld->type].rank > 1 )
            {
                send_to_char( "This building is not transferrable.\n\r", ch );
                return;
            }
            if ( !can_build(bld->type,map_table.type[x][y][bld->z], bld->z) )
            {
                send_to_char( "Invalid sector type.\n\r", ch );
                return;
            }
            for ( xx = x-10;xx<x+10;xx++ )
                for ( yy = y-10;yy<y+10;yy++ )
            {
                bld2 = get_building(xx,yy,ch->z);
                if ( bld2 == NULL )
                    continue;
                if ( str_cmp(bld2->owned,ch->name) )
                {
                    send_to_char( "You can't move buildings near enemy bases.\n\r", ch );
                    return;
                }
            }
            if ( ( bld2 = get_building(x,y,ch->z) ) != NULL )
            {
                send_to_char( "There is already a building there!\n\r", ch );
                return;
            }
            for ( obj = map_obj[bld->x][bld->y];obj;obj = obj_next )
            {
                obj_next = obj->next_in_room;
                if ( obj->z != bld->z )
                    continue;
                move_obj(obj,x,y,bld->z);
            }
            map_bld[bld->x][bld->y][bld->z] = NULL;
            bld->x = x;
            bld->y = y;
            free(map_bld[bld->x][bld->y][bld->z]);
            map_bld[bld->x][bld->y][bld->z] = bld;
            ch->in_building = NULL;
            ch->quest_points -= cost;
            send_to_char( "Moved.\n\r", ch );
            return;
        }
        if ( ( bld = get_building(x,y,ch->z) ) != NULL )
        {
            if ( str_cmp(bld->owned,ch->name) )
            {
                send_to_char( "There is a building there!\n\r", ch );
                return;
            }
        }
        send_to_char( "You dissolve from the room.\n\r", ch );
        for ( vch = first_char;vch;vch = vch->next )
        {
            if ( vch->x == ch->x && vch->y == ch->y && vch->z == ch->z && vch != ch )
                act( "$N dissolves from the room!", vch, NULL, ch, TO_CHAR );
            if ( vch->x == x && vch->y == y && vch->z == ch->z )
                act( "$N shimmers into the room!", vch, NULL, ch, TO_CHAR );
        }

        {
            int xx,yy;
            for ( xx = x - 3;xx <= x + 3;xx++ )
                for ( yy = y - 3;yy <= y + 3;yy++ )
                    if ( xx > 0 && yy > 0 && xx < MAX_MAPS && yy < MAX_MAPS )
                        if ( ( bld = get_building(xx,yy,ch->z)  ) != NULL )
                            if ( bld->type == BUILDING_TRAP && str_cmp(ch->name,bld->owned))
                                if ( vch != NULL || ( vch = get_ch(bld->owned) ) != NULL )
                                    if ( vch->pcdata->alliance == -1 || vch->pcdata->alliance != ch->pcdata->alliance )
                                        if ( vch->security )
                                            trap = TRUE;
        }
        ch->quest_points -= cost;
        move( ch, x, y, ch->z );
        WAIT_STATE(ch,40);
        do_look(ch,"");
        if ( trap )
        {
            send_to_char( "@@ySomething catches your teleportation, and ZAPS you!@@N\n\r", ch );
            damage(vch,ch,50,-1);
            WAIT_STATE(ch,70);
        }
        return;
    }
    else if ( !str_cmp(arg,"repair") )
    {
        if ( !is_number(argument) )
        {
            send_to_char( "Repair how much?\n\r", ch );
            return;
        }
        cost = atoi(argument);
        if ( ( obj = get_obj_carry(ch,arg2) ) == NULL )
        {
            send_to_char( "You do not carry that object.\n\r", ch );
            return;
        }
        if ( obj->item_type != ITEM_ARMOR )
        {
            send_to_char( "You can only repair armor.\n\r", ch );
            return;
        }
        if ( obj->value[1] == 0 )
        {
            send_to_char( "That item doesn't need repair.\n\r", ch );
            return;
        }
        if ( cost > obj->value[1] )
            cost = obj->value[1];
        if ( ch->quest_points < cost )
        {
            send_to_char( "You do not have enough quest points.\n\r", ch );
            return;
        }
        obj->value[1] -= cost;
        ch->quest_points -= cost;
        sprintf( buf, "%s repaired for %d QPs. Current Damage: %d.\n\r", obj->short_descr, cost, obj->value[1] );
        send_to_char( buf, ch );
        return;
    }
    else if ( !str_cmp(arg,"complete") )
    {
        cost = 200;
        if ( ( bld = get_char_building(ch) ) == NULL )
        {
            send_to_char( "You are not in a building.\n\r", ch );
            return;
        }
        if ( str_cmp(ch->name,bld->owned) )
        {
            send_to_char( "This isn't your building.\n\r", ch );
            return;
        }
        if ( complete(bld) )
        {
            send_to_char( "The building is already complete.\n\r", ch );
            return;
        }
        if ( build_table[bld->type].rank > 0 )
        {
            send_to_char( "This is one building you can't use qpspend to finish.\n\r", ch );
            return;
        }
        if ( ch->quest_points < cost )
        {
            sprintf( buf, "Cost is %d. You only have %d QPs.\n\r", cost, ch->quest_points );
            send_to_char( buf, ch );
            return;
        }
        for ( i = 0;i < 8;i++ )
            bld->resources[i] = 0;
        bld->hp = bld->maxhp;
        bld->shield = bld->maxshield;
        if ( !IS_SET(bld->value[1],INST_QP) )
            SET_BIT(bld->value[1],INST_QP);
        reset_special_building(bld);
        send_to_char( "Building completed.\n\r", ch );
        ch->quest_points -= cost;
    }
    else if ( !str_cmp(arg,"blueprint") )
    {
        int levelor = -1;
        if ( ( bld = get_char_building(ch) ) == NULL )
        {
            send_to_char( "You must be in a building.\n\r", ch );
            return;
        }
        if ( str_cmp(bld->owned,ch->name) )
        {
            send_to_char( "This isn't your building.\n\r", ch );
            return;
        }
        if ( !complete(bld) )
        {
            send_to_char( "The building isn't finished yet.\n\r", ch );
            return;
        }
        if ( bld->type == BUILDING_SPACE_CENTER )
        {
            send_to_char( "You can't purchase upgrades for space centers, you must find them by mining in space.\n\r", ch );
            return;
        }
        if ( bld->type == BUILDING_HQ )
        {
            send_to_char( "You may not purchase HQ upgrades with quest points.\n\r", ch );
            return;
        }
        if ( build_table[bld->type].rank > 0 )
        {
            send_to_char( "You cannot buy blueprints for this building.\n\r", ch );
            return;
        }
        if ( CIVILIAN(bld) || bld->type == BUILDING_ENGINEER_HOME )
        {
            send_to_char( "This building can't be upgraded.\n\r", ch );
            return;
        }
        if ( bld->level > 3 )
        {
            send_to_char( "You can't buy L5 blueprints. You must use the Blueprint Research skill.\n\r", ch );
            levelor = bld->level;
            bld->level = 3;
        }
        cost = (bld->level == 1) ? 40 : (bld->level == 2) ? 60 : (bld->level >= 3 ) ? 115 : 999;
        if ( bld->type == BUILDING_SCUD_LAUNCHER || bld->type == BUILDING_NUKE_LAUNCHER )
            cost *= 1.5;
        if ( bld->type == BUILDING_ATOM_BOMBER || bld->type == BUILDING_ALIEN_PROBE || bld->type == BUILDING_INTERGALACTIC_PUB  )
            cost *= 2;
        if ( bld->type == BUILDING_DOOMSDAY_DEVICE || bld->type == BUILDING_FLAMESPITTER )
            cost *= 2.5;
        if ( ch->quest_points < cost )
        {
            char buf[MSL];
            sprintf( buf, "This costs %d QPs. You only have %d.\n\r", cost, ch->quest_points );
            send_to_char( buf, ch );
            if ( levelor != -1 )
                bld->level = levelor;
            return;
        }
        create_blueprint(bld);
        if ( levelor != -1 )
            bld->level = levelor;
        send_to_char( "Blueprint created.\n\r", ch );
        ch->quest_points -= cost;
        return;
    }
    else if ( !str_cmp(arg,"atom") )
    {
        if ( ch->quest_points < 200 )
        {
            send_to_char( "You do not have 200 quest points.\n\r", ch );
            return;
        }
        if ( ( obj = create_object(get_obj_index(32700),0) ) == NULL )
        {
            send_to_char( "System error! Atom bomb upgrade item not found.\n\r", ch );
            return;
        }
        obj_to_char( obj, ch );
        ch->quest_points -= 200;
        send_to_char( "Item purchased.\n\r", ch );
        return;
    }
    else if ( !str_cmp(arg,"clip") )
    {
        int size;
        if ( ( obj = get_obj_carry(ch,arg2)) == NULL )
        {
            send_to_char( "You do not have a weapon by that name.\n\r", ch );
            return;
        }
        if ( obj->item_type != ITEM_WEAPON )
        {
            send_to_char( "This is not a weapon.\n\r", ch );
            return;
        }
        if ( !is_number(argument) )
        {
            send_to_char( "Clip size must be numeric.\n\r", ch );
            return;
        }
        size = atoi(argument);
        if ( size < 1 )
        {
            send_to_char( "Increase size must be at least 1.\n\r", ch );
            return;
        }
        if ( obj->pIndexData->value[1] * 3 < obj->value[1] + size )
        {
            send_to_char( "Adding this much will go over the maximum clip size.\n\r", ch );
            return;
        }
        cost = size * 3;
        if ( ch->quest_points < cost )
        {
            send_to_char( "You can't afford that.\n\r", ch );
            return;
        }
        obj->value[1] += size;
        obj->value[0] += size;
        ch->quest_points -= cost;
        send_to_char( "Clip size increased.\n\r", ch );
        return;
    }
    else
    {
        send_to_char( "Syntax:\n\r\n\rQPSpend buy list\n\rQPSpend buy listarmor\n\rQPSpend buy listweapons\n\rQPSpend buy <obj #>\n\rQPSpend transport <x> <y>\n\rQPSpend move <x> <y>\n\rQPSpend repair <object> <amount>\n\rQPSpend complete\n\rQPSpend blueprint\n\rQPSpend clip <weapon> <clip size to add>\n\rQPSpend atom\n\rQPSpend norton\r\n", ch );
        return;
    }
    return;
}

void do_doom( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    CHAR_DATA *vch = NULL;
    char buf[MSL];
    int i = 0;

    if ( !ch->in_building )
        return;
    if ( ch->z == Z_NEWBIE )
    {
        send_to_char( "Not from the newbie planet.\n\r",ch);
        return;
    }
    if ( ( bld = get_char_building(ch) ) == NULL || str_cmp(ch->name,bld->owned) || !complete(bld) || (bld->type != BUILDING_DOOMSDAY_DEVICE && bld->type != BUILDING_HACKPORT && bld->type != BUILDING_TRANSMITTER ) )
    {
        send_to_char( "You must be in your doomsday device, transmitter or hackport!\n\r", ch );
        return;
    }
    if ( bld->value[0] != 0 )
    {
        send_to_char( "It's not ready yet!\n\r", ch );
        return;
    }
    if ( bld->type == BUILDING_DOOMSDAY_DEVICE )
    {
        int chance = bld->level * 25;
        int dvalue = 20;

        if ( IS_SET(bld->value[1],INST_VIRAL_ENHANCER) )
        {
            chance *= 2;
            dvalue = 1;
        }
        act( "You press a button, and a thick, green cloud covers the world!", ch, NULL, NULL, TO_CHAR );
        for ( vch = first_char;vch;vch = vch->next )
        {
            if ( vch == ch || vch->z != 1 )
                continue;
            if ( vch->in_building == bld )
            {
                act( "$n presses a button, and a thick, green cloud covers the world!", ch, NULL, vch, TO_VICT );
                continue;
            }

            act( "A thick, green cloud covers the world!", vch, NULL, NULL, TO_CHAR );
            if ( ch->disease > 0 )
                continue;
            if ( number_percent() < chance && !IS_NEWBIE(vch) && !IS_IMMORTAL(vch) )
            {
                send_to_char( "You breath in the green, and start feeling sick.\n\r", ch );
                vch->disease = dvalue;
                i++;
            }
            else
                send_to_char( "You hold your breath... nothing seems to have happened.\n\r", ch );
        }
    }
    else if ( bld->type == BUILDING_HACKPORT )
    {
        OBJ_DATA *obj;
        for ( obj = first_obj;obj;obj = obj->next )
            if ( obj->item_type == ITEM_COMPUTER )
        {
            if ( obj->in_building && obj->in_building->active == FALSE )
                continue;
            if ( obj->in_building && (obj->owner == NULL || str_cmp(obj->owner,obj->in_building->owned) ) )
            {
                free_string(obj->owner);
                obj->owner = str_dup(bld->owned);
            }
            if ( vch == NULL || str_cmp(vch->name,obj->owner) )
                vch = get_ch(obj->owner);
            if ( !vch || vch == NULL || IS_IMMORTAL(vch) )
                continue;
            if ( vch == ch || ( ch->pcdata->alliance != -1 && ch->pcdata->alliance == vch->pcdata->alliance ) )
                continue;
            obj->value[3] = 1;
            i++;
        }
        if ( i == 0 )
        {
            send_to_char( "Here's a tip... wait until some players with computers log on.\n\r", ch );
            return;
        }
        act( "You press a button, and begin transmitting the virus!", ch, NULL, NULL, TO_CHAR );
    }
    else if ( bld->type == BUILDING_TRANSMITTER )
    {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;
        act( "You press a button, and begin transmitting the signal!", ch, NULL, NULL, TO_CHAR );
        for ( obj = first_obj;obj;obj = obj_next )
        {
            obj_next = obj->next;
            if ( obj->item_type == ITEM_COMPUTER )
            {
                if ( obj->in_building && obj->in_building->active == FALSE )
                    continue;
                if ( obj->in_building && str_cmp(obj->owner,obj->in_building->owned) )
                {
                    free_string(obj->owner);
                    obj->owner = str_dup(bld->owned);
                }
                if ( vch == NULL || str_cmp(vch->name,obj->owner) )
                    vch = get_ch(obj->owner);
                if ( vch && (vch == ch || ( ch->pcdata->alliance != -1 && ch->pcdata->alliance == vch->pcdata->alliance ) || IS_IMMORTAL(vch) || IS_NEWBIE(vch) ) )
                    continue;
                obj->item_type = ITEM_BOMB;
                obj->bomb_data = make_bomb(obj);
                if ( obj->carried_by )
                {
                    if ( obj->carried_by->position == POS_HACKING )
                    {
                        obj->carried_by->bvictim->value[8] = 0;
                        obj->carried_by->bvictim= NULL;
                        send_to_char( "Connection Terminated...\n\r", obj->carried_by );
                        obj->carried_by->position = POS_STANDING;
                    }
                }
                free_string(obj->owner);
                obj->owner = str_dup(ch->name);
                explode(obj);
                i++;
            }
        }
    }
    sprintf( buf, "Reports indicate of %d infections!\n\r", i );
    send_to_char( buf, ch );
    bld->value[0] = 1800;
    return;
}

void do_generate( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    char buf[MSL];

    if ( argument[0] == '\0' )
    {
        send_to_char( "Produce more of which one of your objects (Or ""None"")?\n\r", ch );
        return;
    }
    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must be in a building.\n\r", ch );
        return;
    }
    if ( str_cmp(ch->name,bld->owned) || !complete(bld ) )
    {
        send_to_char( "Not here.\n\r", ch );
        return;
    }
    if ( !(bld->type != BUILDING_MINE && bld->type != BUILDING_IMPROVED_MINE && bld->type != BUILDING_LUMBERYARD ) )
    {
        do_mine(ch,argument);
        return;
    }
    if ( !str_cmp(argument,"none") )
    {
        send_to_char( "Building set to generate normally.\n\r", ch );
        bld->value[0] = 0;
        return;
    }
    if ( bld->level < 3 )
    {
        send_to_char( "Building must be at least L3.\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You must carry the object you want the structure to generate.\n\r", ch );
        return;
    }
    if ( obj->pIndexData->building != bld->type )
    {
        send_to_char( "This building can't generate that.\n\r", ch );
        return;
    }
    bld->value[0] = obj->pIndexData->vnum;
    sprintf( buf, "You insert %s into the research box.\n\r", obj->short_descr );
    send_to_char( buf, ch );
    extract_obj(obj);
    return;
}

void do_destroy( CHAR_DATA *ch, char *argument )
{
    VEHICLE_DATA *vhc;
    char buf[MSL];

    if ( ( vhc = ch->in_vehicle ) == NULL )
    {
        send_to_char( "You must be in the vehicle you'd like to destroy.\n\r", ch );
        return;
    }
    if ( ch->z == Z_SPACE || ch->z == Z_AIR )
    {
        send_to_char( "Umm, that might not be such a good idea... You can try ejecting with the Exit command, though!\n\r", ch );
        return;
    }
    ch->in_vehicle = NULL;
    /*	if ( ( vhc2 = get_vehicle_from_vehicle(vhc) ) != NULL )
            extract_vehicle(vhc2,FALSE);*/

    sprintf( buf, "%s bursts into flames!\n\r", vhc->desc );
    send_to_loc(buf,vhc->x,vhc->y,vhc->z);
    extract_vehicle(vhc,FALSE);
    return;
}

void do_refine( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    int i,x,iron,gold,copper,silver;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Refine what?\n\r", ch );
        return;
    }
    if ( ( bld = ch->in_building ) == NULL || bld->type != BUILDING_REFINERY || !complete(bld) || str_cmp(bld->owned,ch->name))
    {
        send_to_char( "You must be in one of your completed refineries.\n\r", ch );
        return;
    }
    if ( bld->value[3] < 0 )
    {
        send_to_char( "It's corrupted by a virus!\n\r", ch );
        return;
    }
    if ( !str_cmp(argument,"all") )
    {
        char name[MSL];
        OBJ_DATA *obj_next;
        for ( obj = ch->first_carry;obj;obj = obj_next )
        {
            obj_next = obj->next_in_carry_list;
            if ( obj->wear_loc != WEAR_NONE || IS_SET(obj->extra_flags,ITEM_STICKY) )
                continue;
            one_argument(obj->name,name);
            do_refine(ch,name);
        }
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You are not carrying that object.\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR )
    {
        send_to_char( "You can only refine weapons and armor.\n\r", ch );
        return;
    }
    if ( IS_SET(obj->extra_flags,ITEM_STICKY) )
    {
        send_to_char( "You can't refine sticky items.\n\r", ch );
        return;
    }
    iron = 0;
    copper = 0;
    gold = 0;
    silver = 0;
    i = obj->level;
    act( "You refine $p.", ch, obj, NULL, TO_CHAR );
    act( "$n refines $p.", ch, obj, NULL, TO_ROOM );
    extract_obj(obj);
    for ( ; i > 0;i-- )
    {
        x = (number_percent() < 5) ? 2 : (number_percent() < 5) ? 3 : (number_percent() < 15) ? 4 : 0;
        if ( x == ITEM_IRON )
            iron++;
        if ( x == ITEM_COPPER )
            copper++;
        if ( x == ITEM_SILVER )
            silver++;
        if ( x == ITEM_GOLD )
            gold++;
    }
    if ( iron > 0 )
    {
        obj = create_material(ITEM_IRON);
        obj->value[1] = iron;
        obj->x = bld->x;
        obj->y = bld->y;
        obj->z = bld->z;
        obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
        obj->in_building = bld;
    }
    if ( copper > 0 )
    {
        obj = create_material(ITEM_COPPER);
        obj->value[1] = copper;
        obj->x = bld->x;
        obj->y = bld->y;
        obj->z = bld->z;
        obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
        obj->in_building = bld;
    }
    if ( silver > 0 )
    {
        obj = create_material(ITEM_SILVER);
        obj->value[1] = silver;
        obj->x = bld->x;
        obj->z = bld->z;
        obj->y = bld->y;
        obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
        obj->in_building = bld;
    }
    if ( gold > 0 )
    {
        obj = create_material(ITEM_GOLD);
        obj->value[1] = gold;
        obj->x = bld->x;
        obj->y = bld->y;
        obj->z = bld->z;
        obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
        obj->in_building = bld;
    }
    return;
}

void do_sell( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Sell what?\n\r", ch );
        return;
    }
    if ( ( bld = ch->in_building ) == NULL || bld->type != BUILDING_MARKETPLACE || !complete(bld) || str_cmp(bld->owned,ch->name))
    {
        send_to_char( "You must be in one of your completed marketplaces.\n\r", ch );
        return;
    }
    if ( bld->value[3] < 0 )
    {
        send_to_char( "It's corrupted by a virus!\n\r", ch );
        return;
    }
    if ( !str_cmp(argument,"all") )
    {
        OBJ_DATA *obj_next;
        for ( obj = ch->first_carry;obj;obj = obj_next )
        {
            obj_next = obj->next_in_carry_list;
            if ( obj->wear_loc != WEAR_NONE || ( obj->item_type != ITEM_ARMOR && obj->item_type != ITEM_WEAPON ) )
                continue;
            sell_item(ch,obj);
        }
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You are not carrying that object.\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_WEAPON && obj->item_type != ITEM_ARMOR )
    {
        send_to_char( "You can only sell weapons and armor.\n\r", ch );
        return;
    }
    sell_item(ch,obj);
    return;
}

void sell_item(CHAR_DATA *ch, OBJ_DATA *obj)
{
    int x;
    x = number_range(0,number_range(1,obj->level));

    if ( IS_SET(obj->extra_flags,ITEM_NODROP) || IS_SET(obj->extra_flags,ITEM_STICKY) )
    {
        char buf[MSL];
        sprintf( buf, "%s cannot be sold.\n\r", obj->short_descr );
        send_to_char(buf,ch);
        return;
    }
    if ( x == 0 )
        send_to_char( "You tried to sell it, but someone stole it from you!\n\r", ch );
    else
    {
        char buf[MSL];
        sprintf( buf, "You sell %s for %d QPs.\n\r", obj->short_descr, x );
        send_to_char( buf, ch );
        sprintf( buf, "$n sells %s for %d QPs.\n\r", obj->short_descr, x );
        act( buf, ch, NULL, NULL, TO_ROOM );
    }
    extract_obj(obj);
    ch->quest_points += x;
    if ( ch->quest_points > 5000 )
    {
        send_to_char( "You have reached the cap of 5000 QPs.\n\r", ch );
        ch->quest_points = 5000;
    }
    return;
}

void do_trade( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    char arg[MSL];
    char buf[MSL];
    OBJ_DATA *obj;
    OBJ_DATA *obj_new;
    int type;

    argument = one_argument(argument,arg);
    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char( "Trade what for what?\n\r(Eg. 'trade iron gold' will turn your iron to gold)\n\r", ch );
        return;
    }

    if ( ( bld = ch->in_building ) == NULL || bld->type != BUILDING_ONLINE_MARKET || !complete(bld) || str_cmp(bld->owned,ch->name))
    {
        send_to_char( "You must be in one of your completed online markets.\n\r", ch );
        return;
    }
    if ( bld->value[3] < 0 )
    {
        send_to_char( "It's corrupted by a virus!\n\r", ch );
        return;
    }
    if ( !str_prefix(argument,"iron") )
        type = ITEM_IRON;
    else if ( !str_prefix(argument,"skin") )
        type = ITEM_SKIN;
    else if ( !str_prefix(argument,"gold") )
        type = ITEM_GOLD;
    else if ( !str_prefix(argument,"copper") )
        type = ITEM_COPPER;
    else if ( !str_prefix(argument,"silver") )
        type = ITEM_SILVER;
    else if ( !str_prefix(argument,"rocks") )
        type = ITEM_ROCK;
    else if ( !str_prefix(argument,"sticks") )
        type = ITEM_STICK;
    else if ( !str_prefix(argument,"logs") )
        type = ITEM_LOG;
    else
    {
        send_to_char( "Valid types are: Iron, Skin, Gold, Silver, Copper, Rocks, Sticks, Logs\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry(ch,arg) ) == NULL )
    {
        send_to_char( "You are not carrying that object.\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_MATERIAL )
    {
        send_to_char( "You can only trade resources.\n\r", ch );
        return;
    }
    if ( obj->value[0] == type )
    {
        send_to_char( "You can't convert it to what it already is!\n\r", ch );
        return;
    }
    if ( obj->value[1] < 3 )
    {
        send_to_char( "You don't have enough of it. Must have a chunk of at least 3.\n\r", ch );
        return;
    }
    obj_new = create_material(type);
    obj_new->value[1] = obj->value[1] / 3;
    obj_new->weight = obj->weight / 3;
    sprintf( buf, "You trade %s to %s.\n\r", obj->short_descr, obj_new->short_descr );
    send_to_char( buf, ch );
    sprintf( buf, "$n trades %s to %s.\n\r", obj->short_descr, obj_new->short_descr );
    act( buf, ch, NULL, NULL, TO_ROOM );
    extract_obj(obj);
    obj_to_char(obj_new,ch);
    obj->value[0] = type;
    return;
}

void do_winstall( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *weapon;
    char buf[MSL];

    if ( paintball(ch) )
        return;
    if ( ( weapon = get_eq_char(ch, WEAR_HOLD_HAND_L) ) == NULL )
    {
        send_to_char( "You must be holding the weapon in your left hand.\n\r", ch );
        return;
    }
    if ( weapon->item_type != ITEM_WEAPON )
    {
        send_to_char( "You are not holding a weapon in your left hand.\n\r", ch );
        return;
    }
    if ( weapon->pIndexData->vnum == 1152 )
    {
        send_to_char( "You can't install stuff on that.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' || ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "Weapon-Install what?\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_WEAPON_UP )
    {
        send_to_char( "This is not a weapon upgrade.\n\r", ch );
        return;
    }
//    if (IS_SET(weapon->extra_flags,ITEM_STICKY))
//    mreturn("No mods on a sticky weapon, please.\n\r",ch);
    if ( obj->value[0] == 0 )
    {
        if ( weapon->value[4] - 2 >= weapon->pIndexData->value[4] )
        {
            send_to_char( "This won't do anything.\n\r", ch );
            return;
        }
        weapon->value[4] += obj->value[1];
    }
    else if ( obj->value[0] == 1 )
    {
        if ( weapon->value[6] > obj->value[1] )
        {
            send_to_char( "You already have a better one installed.\n\r", ch );
            return;
        }
        weapon->value[6] = obj->value[1];
    }
    else if ( obj->value[0] == 2 )
    {
        if ( obj->value[2] == 0 )
        {
            if ( weapon->value[2] != 3 )
            {
                send_to_char( "This weapon does not fire rockets!\n\r", ch );
                return;
            }
            weapon->value[2] = 18;
            weapon->value[0] = 5;
            weapon->value[1] = 5;
            weapon->value[4] += 3;
            free_string(weapon->short_descr);
            free_string(weapon->description);
            free_string(weapon->name);
            weapon->short_descr = str_dup("@@WA @@RF@@ei@@Rr@@ee@@g-@@WRocket Launcher@@N");
            weapon->description = str_dup("@@WA @@RF@@ei@@Rr@@ee@@g-@@WRocket Launcher@@N");
            weapon->name = str_dup("fire rocket launcher");
        }
        else
        {
            if ( weapon->value[2] != 8 )
            {
                send_to_char( "You must install it on a short-ranged cookie launcher.\n\r", ch );
                return;
            }
            weapon->value[1] = 25;
            weapon->value[0] = 25;
            weapon->value[4] = 4;
            free_string(weapon->short_descr);
            free_string(weapon->description);
            free_string(weapon->name);
            weapon->short_descr = str_dup( "@@bA @@WG@@bi@@yng@@We@@yrb@@br@@We@@bad @@yB@@Wom@@bbe@@yr@@N" );
            weapon->description = str_dup( "@@bA @@WG@@bi@@yng@@We@@yrb@@br@@We@@bad @@yB@@Wom@@bbe@@yr@@N" );
            weapon->name = str_dup("gingerbread bomber ginger bread");
        }
    }
    else if ( obj->value[0] == 3 )
    {
        int bit = get_bit_value(obj->value[1]);
        if ( IS_SET(weapon->value[3],bit) )
        {
            send_to_char( "The weapon already has this effect.\n\r", ch );
            return;
        }
        SET_BIT(weapon->value[3],bit);
    }
    else if ( obj->value[0] == 4 )
    {
        if ( IS_SET(weapon->extra_flags,ITEM_STICKY) )
        {
            send_to_char( "The weapon already has that effect.\n\r", ch );
            return;
        }
/*        if (weapon->value[4]>weapon->pIndexData->value[4]
|| weapon->value[6] > weapon->pIndexData->value[6]
|| weapon->value[7] > weapon->pIndexData->value[7]
|| weapon->value[3]!=weapon->pIndexData->value[3])
mreturn("No enhancements; no sticky for you!\n\r",ch); */
        SET_BIT(weapon->extra_flags,ITEM_STICKY);
        if ( !IS_SET(weapon->extra_flags,ITEM_NODROP) )
            SET_BIT(weapon->extra_flags,ITEM_NODROP);
    }
    else if ( obj->value[0] == 5 )
    {
        if ( weapon->value[7]+clip_table[weapon->value[2]].dam
            >= (clip_table[weapon->value[2]].dam+weapon->pIndexData->value[7]) * 1.1 )
        {
            send_to_char( "This won't do anything.\n\r", ch );
            return;
        }
        weapon->value[7] += (weapon->pIndexData->value[7]+clip_table[weapon->value[2]].dam) * 0.05;
    }
    else
    {
        send_to_char( "Unknown installation value. Please contact an administrator.\n\r", ch );
        return;
    }

    sprintf( buf, "You install %s in %s.\n\r", obj->short_descr, weapon->short_descr );
    send_to_char( buf, ch );
    sprintf( buf, "$n installs %s in %s.", obj->short_descr, weapon->short_descr );
    act( buf, ch, NULL, NULL, TO_ROOM );
    extract_obj(obj);
    return;
}

void do_vinstall( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    VEHICLE_DATA *vhc;
    char buf[MSL];

    if ( ( vhc = ch->in_vehicle ) == NULL )
    {
        send_to_char( "You must be inside the vehicle you want to improve.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' || ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        sprintf( buf, "Installations:\n\rScanner: %d    Weapons: %d\n\r", vhc->scanner, vhc->range );
        if ( IS_SET(vhc->flags,VEHICLE_CORROSIVE) )
            sprintf( buf+strlen(buf), "Corrosive Weapons\n\r" );
        if ( IS_SET(vhc->flags,VEHICLE_GUARD_LASERS) )
            sprintf( buf+strlen(buf), "Guard Lasers\n\r" );
        if ( IS_SET(vhc->flags,VEHICLE_MINING_BEAM) )
            sprintf( buf+strlen(buf), "Mining Beam\n\r" );
        if ( IS_SET(vhc->flags,VEHICLE_PSI_SCANNER) )
            sprintf( buf+strlen(buf), "Psi Scanner\n\r" );
        if ( IS_SET(vhc->flags,VEHICLE_OBJ_SENSORS) )
            sprintf( buf+strlen(buf), "Object Sensors\n\r" );
        if ( IS_SET(vhc->flags,VEHICLE_DRILL) )
            sprintf( buf+strlen(buf), "Drill\n\r" );
        if ( IS_SET(vhc->flags,VEHICLE_ALIEN_MAGNET) )
            sprintf( buf+strlen(buf), "Alien Magnet\n\r" );
        if ( IS_SET(vhc->flags,VEHICLE_TREASURE_BEAM) )
            sprintf( buf+strlen(buf), "Treasure Beam\n\r" );
        if ( IS_SET(vhc->flags,VEHICLE_SPACE_SCANNER) )
            sprintf( buf+strlen(buf), "Life-Form Scanner\n\r" );
        send_to_char(buf,ch);
        return;
    }
    if ( obj->item_type != ITEM_VEHICLE_UP )
    {
        send_to_char( "This is not a vehicle addon.\n\r", ch );
        return;
    }
    if ( obj->level > ch->pcdata->skill[gsn_engineering] && obj->level <= 10 )
    {
        sprintf(buf,"You must have an engineering proficiency of %d to use this.\n\r", obj->level );
        send_to_char(buf,ch);
        return;
    }
    obj->value[1] += ch->pcdata->skill[gsn_engineering];
    if ( obj->value[0] == 0 )
    {
        if ( vhc->scanner >= obj->value[1] )
        {
            sprintf( buf, "Your ship already has a scanning range of %d. This upgrade won't change anything.\n\r", vhc->scanner );
            send_to_char(buf,ch);
            return;
        }
        vhc->scanner = obj->value[1];
    }
    else if ( obj->value[0] == 1 )
    {
        if ( vhc->range >= obj->value[1] )
        {
            sprintf( buf, "Your ship already has a combat range of %d. This upgrade won't change anything.\n\r", vhc->range );
            send_to_char(buf,ch);
            return;
        }
        vhc->range = obj->value[1];
    }
    else if ( obj->value[0] == 2 )
    {
        int val = get_bit_value(obj->value[9]);
        if ( IS_SET(vhc->flags,val) )
        {
            send_to_char( "The craft is already implanted with the upgrade.\n\r", ch );
            return;
        }
        SET_BIT(vhc->flags,val);
    }
    else
    {
        send_to_char( "Unknown installation value. Please contact an administrator.\n\r", ch );
        return;
    }

    sprintf( buf, "You install %s in %s.\n\r", obj->short_descr, vhc->desc );
    send_to_char( buf, ch );
    sprintf( buf, "$n installs %s in %s.", obj->short_descr, vhc->desc );
    act( buf, ch, NULL, NULL, TO_ROOM );
    extract_obj(obj);
    return;
}

void do_practice( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;

    if ( IS_NEWBIE(ch) )
        send_to_char( "@@gIf you are looking for information on skills, type @@WSKILLS@@g.@@N\n\r", ch );
    if ( IS_SET(ch->pcdata->pflags,PLR_BASIC) )
    {
        send_to_char( "You cannot go into practice mode while in Basic gameplay.\n\r", ch );
        return;
    }
    if (  IS_SET( ch->pcdata->pflags, PFLAG_PRACTICE ) )
    {
        extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
        send_to_char( "You have left practicing mode!\n\r", ch );
        ch->fighttimer = 1;
        ch->c_sn = gsn_practice;
        ch->c_time = 80;
        for ( bld = ch->first_building;bld;bld = bld->next_owned )
        {
            {
                if ( bld->x < 0 )
                    bld->x *= -1;
                if ( bld->y < 0 )
                    bld->y *= -1;
                map_bld[bld->x][bld->y][bld->z] = bld;
                bld->hp = bld->value[2];
                if ( bld->type != BUILDING_SPACE_CENTER )
                    bld->shield = bld->value[5];
                else
                    bld->shield = bld->maxshield;
                for ( obj = map_obj[bld->x][bld->y];obj;obj = obj->next )
                    if ( obj->z == bld->z && obj->value[1] != 0 && obj->item_type == ITEM_BOMB )
                        if ( !str_cmp(obj->owner,ch->name) )
                            obj->value[1] = 0;
            }
        }
        send_to_char( "You must now wait 10 seconds.\n\r", ch );
        save_char_obj(ch);
        return;
    }
    else
    {
        if ( ch->fighttimer > 0 )
        {
            send_to_char( "Not while you're fighting!\n\r", ch );
            return;
        }
        if ( ch->z == Z_PAINTBALL )
            return;
        for ( bld = ch->first_building;bld;bld = bld->next_owned )
        {
            bld->value[2] = bld->hp;
            if ( bld->type != BUILDING_SPACE_CENTER )
                bld->value[5] = bld->shield;
            if ( bld->x < 0 )
                bld->x *= -1;
            if ( bld->y < 0 )
                bld->y *= -1;
            map_bld[bld->x][bld->y][bld->z] = bld;
        }
        SET_BIT(ch->pcdata->pflags, PFLAG_PRACTICE);
        send_to_char( "You have entered practicing mode!\n\r", ch );
        save_char_obj(ch);
        return;
    }
    return;
}

void act_practice( CHAR_DATA *ch, int level )
{
    if (  IS_SET( ch->pcdata->pflags, PFLAG_PRACTICE ) )
        REMOVE_BIT(ch->pcdata->pflags, PFLAG_PRACTICE);
    send_to_char ("You are out of practice mode.\n\r", ch );
}

void do_connect( CHAR_DATA *ch, char *argument )
{
    char arg[MSL];
    char buf[MSL];
    OBJ_DATA *obj1;
    OBJ_DATA *obj2;
    OBJ_DATA *obj3;
    int item = -1;
    bool ok = FALSE;

    argument = one_argument(argument,arg);
    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char( "Syntax: connect <item1> <item2>\n\r", ch );
        return;
    }
    if ( ( obj1 = get_obj_carry(ch,arg) ) == NULL )
    {
        sprintf( buf, "You are not carrying %s.\n\r", arg );
        send_to_char( buf, ch );
        return;
    }
    if ( ( obj2 = get_obj_carry(ch,argument) ) == NULL )
    {
        sprintf( buf, "You are not carrying %s.\n\r", argument );
        send_to_char( buf, ch );
        return;
    }
    if ( obj1->item_type != ITEM_PIECE || obj2->item_type != ITEM_PIECE )
    {
        send_to_char( "Both items must be pieces of another.\n\r", ch );
        return;
    }
    if ( obj1->value[0] == obj2->pIndexData->vnum )
    {
        ok = TRUE;
        item = obj1->value[1];
    }
    else if ( obj1->value[2] == obj2->pIndexData->vnum )
    {
        ok = TRUE;
        item = obj1->value[3];
    }
    if ( !ok )
    {
        send_to_char( "These pieces won't connect in that order.\n\r", ch );
        return;
    }
    if ( ( obj3 = create_object(get_obj_index(item),0) ) == NULL )
    {
        send_to_char( "System error! Target object not found!\n\r", ch );
        return;
    }
    sprintf( buf, "You connect %s and %s - They become %s!\n\r", obj1->short_descr, obj2->short_descr, obj3->short_descr );
    send_to_char( buf, ch );
    obj_to_char(obj3,ch);
    extract_obj(obj1);
    extract_obj(obj2);
    return;
}

void do_track( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    CHAR_DATA *wch;
    char buf[MSL];

    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You must be at a hackers hideout!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        bool found = FALSE;
        //		send_to_char( "Track whom?\n\r", ch );
        sprintf( buf, "Tracable Targets:\n\r\n\r" );
        for ( wch = first_char;wch;wch = wch->next )
        {
            for ( obj = wch->first_carry;obj;obj = obj->next_in_carry_list )
            {
                if ( obj->item_type == ITEM_COMPUTER && obj->value[3] != 0 )
                {
                    found = TRUE;
                    sprintf( buf+strlen(buf), "%s\n\r", wch->name );
                    break;
                }
            }
        }
        send_to_char( buf, ch );
        if ( !found )
            send_to_char( "No tracable targets detected.\n\r", ch );

        return;
    }
    if ( ( wch = get_char_world(ch,argument) ) == NULL )
    {
        send_to_char( "You can't find that target.\n\r", ch );
        return;
    }
    for ( obj = wch->first_carry;obj;obj = obj->next_in_carry_list )
    {
        if ( obj->item_type == ITEM_COMPUTER && obj->value[3] != 0 )
        {
            char pbuf[MSL];
            if ( IS_SET(wch->effect,EFFECT_ENCRYPTION) )
            {
                send_to_char( "You just can't seem to lock on that target...\n\r", ch );
                return;
            }
            sprintf(pbuf,", on %s", planet_table[wch->z].name);
            sprintf( buf, "\n\r@@cYou detect your target at @@a%d@@c,@@a%d@@c%s!@@N\n\r", wch->x, wch->y, (ch->z==wch->z)?"":pbuf );
            send_to_char( buf, ch );
            return;
        }
    }
    send_to_char( "Your target was not carrying an infected computer.\n\r", ch );
    return;
}

void do_spy( CHAR_DATA *ch, char *argument )
{
    char arg[MSL];
    BUILDING_DATA *bld;

    if ( ch->z == Z_NEWBIE )
    {
        send_to_char( "Not from the newbie planet.\n\r",ch);
        return;
    }
    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You must be at a ready spy quarters or satellite.\n\r", ch );
        return;
    }
    if ( (bld->type != BUILDING_SPY_QUARTERS && bld->type != BUILDING_SPY_SATELLITE && bld->type != BUILDING_ALIEN_PROBE && bld->type != BUILDING_INTERGALACTIC_PUB && bld->type != BUILDING_SHOCKWAVE ) || bld->value[0] > 0 || !complete(bld) )
    {
        send_to_char( "You must be at a ready spy quarters or satellite, alien probe, intergalactic pub or at a shockwave.\n\r", ch );
        return;
    }
    if ( str_cmp(bld->owned,ch->name) )
    {
        send_to_char( "This isn't your building!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' && bld->type != BUILDING_INTERGALACTIC_PUB )
    {
        send_to_char( "Spy after whom, or where?\n\r", ch );
        return;
    }
    if ( bld->type == BUILDING_INTERGALACTIC_PUB )
    {
        CHAR_DATA *wch;
        OBJ_DATA *bomb;

        for ( wch = first_char;wch;wch = wch->next )
        {
            if ( allied(ch,wch) || wch->z == Z_NEWBIE || IS_IMMORTAL(wch) || IS_NEWBIE(wch) )
                continue;
            if ( wch->in_building && wch->in_building->type == BUILDING_CLUB )
            {
                bomb = create_object(get_obj_index(999),0);
                bomb->weight = 1;
                bomb->value[2] = bld->level * 750;
                bomb->x = wch->x; bomb->y = wch->y; bomb->z= wch->z;
                bomb->value[0] = 5;
                bomb->value[1] = 1;
                free_string(bomb->owner);
                bomb->owner = str_dup(ch->name);
                obj_to_room(bomb,wch->in_room);
            }
        }
        send_to_char( "You send your assassins to plant the bombs.\n\r", ch );
        bld->value[0] = 360;
        return;
    }
    else if ( bld->type == BUILDING_ALIEN_PROBE )
    {
        CHAR_DATA *wch;
        int map,x,y,z;
        map = ch->map;
        x = ch->x; y = ch->y; z = ch->z;
        if ( ( wch = get_char_world(ch,argument) ) == NULL )
        {
            send_to_char( "You can't find that target.\n\r", ch );
            return;
        }
        if ( IS_NEWBIE(wch) || IS_IMMORTAL(wch) || wch->z == Z_SPACE || wch->z == Z_PAINTBALL )
        {
            send_to_char( "You can't target that person.\n\r", ch );
            return;
        }
        ch->map = 10;
        move(ch,wch->x,wch->y,wch->z);
        do_look(ch,"outside");

        ch->map = map;
        move(ch,x,y,z);
    }
    else if ( bld->type == BUILDING_SPY_QUARTERS )
    {
        CHAR_DATA *wch;
        int x=0;
        int b=0;
        int i;
        BUILDING_DATA *bld2;
        char buf[MSL];
        int buildings[MAX_BUILDING];
        OBJ_DATA *obj;

        if ( ( wch = get_char_world(ch,argument) ) == NULL || wch->z == Z_NEWBIE || IS_NEWBIE(wch) )
        {
            send_to_char( "You can't find that target.\n\r", ch );
            return;
        }
        for ( i = 0;i<MAX_BUILDING;i++ )
            buildings[i] = 0;
        act( "You send your spies on a mission.\n\r", ch, NULL, NULL, TO_CHAR );
        for ( bld2=first_building;bld2;bld2 = bld2->next )
            if ( !str_cmp(bld2->owned,wch->name) )
        {
            buildings[bld2->type]++;
            if ( defense_building(bld2) )
                x += bld2->level;
            if ( bld->level >= 4 && bld2->type == BUILDING_HQ )
            {
                obj = create_object(get_obj_index(1028),0);
                obj->x = bld2->x;
                obj->y = bld2->y;
                obj->z = bld2->z;
                obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
                obj->value[1] = 1;
                obj->value[0] = 120;
                free_string(obj->owner);
                obj->owner = str_dup(ch->name);
                b++;
            }
        }
        x = number_range(x/1.1,x*1.1);
        sprintf( buf, "Your spies report: We estimate %s's defense rating is at %d.\n\r", wch->name, x );
        if ( bld->level >= 4 )
            sprintf( buf+strlen(buf), "%d bombs have been planted in headquarters.\n\r", b );
        act( buf, ch, NULL, NULL, TO_CHAR );
        if ( bld->level >= 2 && wch->z != Z_PAINTBALL && !wch->fake )
        {
            sprintf( buf, "They have also discovered %s residing at %d/%d, on %s.\n\r", wch->name,wch->x,wch->y, planet_table[wch->z].name );
            send_to_char(buf,ch);
        }

        if ( bld->level >= 5 )
        {
            send_to_char( "\n\rOur spies have delivered a more detailed report of the enemy's base:\n\r", ch );
            for ( i = 0;i<MAX_BUILDING;i++ )
            {
                if ( buildings[i] == 0 )
                    continue;
                if ( is_neutral(i) )
                    continue;
                sprintf( buf, "@@G%d @@r%s\n\r", buildings[i], build_table[i].name );
                send_to_char(buf,ch);
            }
        }
    }
    else if ( bld->type == BUILDING_SHOCKWAVE )
    {
        CHAR_DATA *wch;
        BUILDING_DATA *bld2;
        char buf[MSL];
        int x = 0;

        if ( ( wch = get_char_world(ch,argument) ) == NULL || wch->z == Z_NEWBIE || IS_NEWBIE(wch) )
        {
            send_to_char( "You can't find that target.\n\r", ch );
            return;
        }
        act( "You transmit a shockwave at your target location!\n\r", ch, NULL, NULL, TO_CHAR );
        for ( bld2=wch->first_building;bld2;bld2 = bld2->next_owned )
        {
            if ( bld2->type != BUILDING_DUMMY )
                continue;
            if ( !str_cmp(bld2->owned,wch->name) )
            {
                bld2->value[5] = 1;
                x++;
            }
        }
        sprintf( buf, "@@gReports indicate that @@W%d@@g dummies were disabled.\n\r", x );
        send_to_char(buf,ch);
        bld->value[0] = 540;
        return;
    }
    else if ( bld->type == BUILDING_SPY_SATELLITE )
    {
        int x,y,zz,xx,yy,map;
        argument = one_argument(argument,arg);
        if ( !is_number(arg) || !is_number(argument) || INVALID_COORDS(atoi(arg),atoi(argument)) )
        {
            send_to_char( "Invalid coords.\n\r", ch );
            return;
        }
        xx = ch->x;
        yy = ch->y;
        zz = ch->z;
        map = ch->map;
        x = atoi(arg);
        y = atoi(argument);
        move(ch,x,y,zz);
        ch->map = 5;
        if ( IS_SET(ch->config,CONFIG_SMALLMAP) )
            ShowWMap( ch, TRUE, 997 );
        else
            ShowWMap( ch, FALSE, 997 );
        move(ch,xx,yy,zz);
        ch->map = map;
    }
    bld->value[0] = 360;
    if ( ch->fighttimer < 40 ) ch->fighttimer = 40;
    return;
}

void do_torment( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    BUILDING_DATA *bld;
    char arg[MSL];
    CHAR_DATA *victim;
    int action;

    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You must be at a psychic tormentor!\n\r", ch );
        return;
    }
    if ( !complete(bld) || str_cmp(bld->owned,ch->name) || bld->value[0] != 0 || bld->type != BUILDING_PSYCHIC_TORMENTOR )
    {
        send_to_char( "You can't use this building.\n\r", ch );
        return;
    }
    argument = one_argument(argument,arg);
    if ( argument[0] == '\0' || arg[0] == '\0')
    {
        send_to_char( "Torment <player> <demolish/arm/fire>\n\r", ch );
        return;
    }
    action = (!str_cmp(argument,"demolish") ) ? 1 : (!str_cmp(argument,"arm") ) ? 2 : (!str_cmp(argument,"fire") ) ? 3 : -1;
    if ( action == -1 )
    {
        send_to_char( "Invalid action (demolish/arm/fire).\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world(ch,arg) ) == NULL )
    {
        send_to_char( "You can't find that target.\n\r", ch );
        return;
    }
    if ( IS_NEWBIE(victim) || IS_IMMORTAL(victim) )
    {
        send_to_char( "You can't target that player.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        send_to_char( "Ummm... can't you just type the command yourself?\n\r", ch );
        return;
    }
    bld->value[0] = 360;
    if ( ( bld = victim->in_building) != NULL && bld->type == BUILDING_PSYCHIC_SHIELD )
    {
        send_to_char( "The psychic shield begins to glow!\n\r", victim );
        victim = ch;
    }
    if ( (obj=get_eq_char(victim,WEAR_HEAD))!= NULL)
    {
        if ( obj->pIndexData->vnum == 32659 )               //Psicap
        {
            char buf[MSL];
            sprintf( buf, "%s begins to glow!\n\r", obj->short_descr );
            send_to_loc(buf,victim->x,victim->y,victim->z);
            victim = ch;
        }
    }
    send_to_char( "@@eYou suddenly feel compelled to do something... You just can't control it!@@N\n\r", victim );
    if ( action == 1 )
    {
        do_demolish(victim,"");
    }
    else if ( action == 2 )
    {
        do_set(victim,"bomb 3");
        do_arm(victim,"bomb");
    }
    else
    {
        do_shoot(victim,"");
    }
    act( "You force $N to do your bidding!", ch, NULL, victim, TO_CHAR );
    return;
}

void do_locate( CHAR_DATA *ch, char *argument )
{
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
    int x,y,i,m;
    OBJ_DATA *obj;
    OBJ_DATA *scanner = NULL;
    char buf[MSL];
    bool ex = FALSE;

    if ( ch->z == Z_PAINTBALL )
        return;
    m = 1;
    x = ch->x;
    y = ch->y;
    if ( ch->class == CLASS_SCANNER )
        m = 50;
    for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
    {
        if ( obj->item_type == ITEM_LOCATOR && obj->value[0] > m )
        {
            m = obj->value[0];
            scanner = obj;
        }
    }
    for ( i=0;i < m;i++ )
    {
        y = ch->y + i;
        for ( x = ch->x-i;x < ch->x+i;x++ )
        {
            if (INVALID_COORDS(x,y) )
                continue;
            for ( obj = map_obj[x][y];obj;obj = obj->next_in_room )
            {
                if ( obj->z == ch->z )
                {
                    ex = TRUE;
                    break;
                }
            }
        }
        if ( ex )
            break;
        y = ch->y - i;
        for ( x = ch->x-i;x < ch->x+i;x++ )
        {
            if (INVALID_COORDS(x,y) )
                continue;
            for ( obj = map_obj[x][y];obj;obj = obj->next_in_room )
            {
                if ( obj->z == ch->z )
                {
                    ex = TRUE;
                    break;
                }
            }
        }
        if ( ex )
            break;
        x = ch->x + i;
        for ( y = ch->y-i;y < ch->y+i;y++ )
        {
            if (INVALID_COORDS(x,y) )
                continue;
            for ( obj = map_obj[x][y];obj;obj = obj->next_in_room )
            {
                if ( obj->z == ch->z )
                {
                    ex = TRUE;
                    break;
                }
            }
        }
        if ( ex )
            break;
        x = ch->x - i;
        for ( y = ch->y-i;y < ch->y+i;y++ )
        {
            if (INVALID_COORDS(x,y) )
                continue;
            for ( obj = map_obj[x][y];obj;obj = obj->next_in_room )
            {
                if ( obj->z == ch->z )
                {
                    ex = TRUE;
                    break;
                }
            }
        }
        if ( ex )
            break;
    }
    if ( map_obj[x][y] == NULL || !map_obj[x][y] )
    {
        send_to_char( "No objects located nearby!\n\r", ch );
        if ( scanner == NULL || !scanner )
            send_to_char( "You didn't even have an item locator.\n\r", ch );
        return;
    }
    if ( scanner )
    {
        scanner->value[1]--;
        if ( scanner->value[1] <= 0 )
        {
            send_to_char( "Your item locator's battery has run out!\n\r", ch );
            extract_obj(scanner);
        }
    }
    if ( map_obj[x][y]->z == Z_UNDERGROUND )
    {
        sprintf( buf, "%s is located to the %s%s!\n\r",
            obj->short_descr,
            (obj->y == ch->y ) ? "" : (obj->y > ch->y)?"North":"South",
            (obj->x == ch->x ) ? "" : (obj->x > ch->x)?"East":"West" );
    }
    else
        sprintf( buf, "%s located at %d/%d\n\r", (map_obj[x][y]->z != ch->z) ? "Unidentified object" : map_obj[x][y]->short_descr,x,y );
    send_to_char(buf,ch);
    return;
}

void do_paradrop( CHAR_DATA *ch, char *argument )
{
    int x,y,z,xx,yy;
    char arg[MSL];
    BUILDING_DATA *bld;

    if ( ( bld = ch->in_building ) == NULL || !complete(bld) || bld->type != BUILDING_PARADROP || str_cmp(bld->owned,ch->name) )
    {
        send_to_char( "You can only do that in a completed paradrop building.\n\r", ch );
        return;
    }
    if ( ch->in_vehicle )
    {
        send_to_char( "Not while inside a vehicle!\n\r", ch );
        return;
    }
    argument = one_argument(argument,arg);
    if ( !is_number(arg) || !is_number(argument) )
    {
        send_to_char( "Syntax: paradrop <x> <y>\n\r", ch );
        return;
    }
    x = atoi(arg);
    y = atoi(argument);
    z = ch->z;
    if ( INVALID_COORDS(x,y) )
    {
        send_to_char( "Invalid coords.\n\r", ch );
        return;
    }
    if ( map_bld[x][y][ch->z] != NULL || map_table.type[x][y][ch->z] == SECT_NULL || map_table.type[x][y][ch->z] == SECT_WATER || map_table.type[x][y][ch->z] == SECT_NULL )
    {
        send_to_char( "You can't go to that location.\n\r", ch );
        return;
    }
    for ( xx=x-3;xx<=x+3;xx++ )
        for ( yy=y-3;yy<=y+3;yy++ )
            if ( !INVALID_COORDS(x,y) && map_bld[xx][yy][ch->z] != NULL && map_bld[xx][yy][ch->z]->active == FALSE )
            {
                send_to_char( "You can't go to that location.\n\r", ch );
                return;
            }
    act( "You jump on board a transport plane and begin the flight.", ch, NULL, NULL, TO_CHAR );
    move(ch,490,490,Z_SPACE);
    ch->x = x;
    ch->y = y;
    ch->z = z;
    ch->in_building = NULL;
    ch->c_sn = gsn_paradrop;
    ch->c_time = 8;
    ch->c_level = 0;
    return;
}

void act_paradrop( CHAR_DATA *ch, int level )
{
    int x,y;
    if ( level < 20 )
    {
        if ( !blind_player(ch) )
            send_to_char( "You continue the flight.\n\r", ch );
    }
    else if ( level == 20 )
    {
        x = ch->x;
        y = ch->y;
        ch->x = 1;
        ch->y = 1;
        move(ch,x,y,ch->z);
        if ( !blind_player(ch) )
            do_look(ch,"");
        send_to_char( "You reach the destination, and jump off the plane!\n\r", ch );
    }
    else if ( level > 20 && level < 40 )
    {
        if ( !blind_player(ch) )
            send_to_char( "You glide down quietly.\n\r", ch );
    }
    else
    {
        BUILDING_DATA *bld;
        bld = map_bld[ch->x][ch->y][ch->z];
        if ( bld && !ch->in_vehicle )
        {
            send_to_char( "You CRASH into a building, falling through the glass windows!\n\r", ch );
            act( "$n CRASHES into the building, falling through the glass windows!\n\r", ch, NULL, NULL, TO_ROOM );
        }
        else
        {
            send_to_char( "You hit the ground!\n\r", ch );
            act( "$n lands on the ground.", ch, NULL, NULL, TO_ROOM );
        }
        ch->c_sn = -1;
        ch->c_level = 0;
        ch->c_time = 0;
        if ( ch->in_vehicle && (SPACE_VESSAL(ch->in_vehicle) || !str_cmp(ch->in_vehicle->name, "Escape Pod") ))
        {
            if ( bld != NULL && bld->type != BUILDING_SPACE_CENTER )
            {
                VEHICLE_DATA *vhc = ch->in_vehicle;
                act( "You CRASH into the $t!!", ch, bld->name, bld->name, TO_CHAR );
                act( "$n CRASHES into the $t!!", ch, bld->name, bld->name, TO_ROOM );
                ch->in_vehicle->driving = NULL;
                ch->in_vehicle = NULL;
                damage(ch,ch,9999,-1);
                extract_vehicle(vhc,TRUE);
                return;
            }
            else if (bld && bld->type == BUILDING_SPACE_CENTER )
            {
                ch->in_building = bld;
                ch->in_vehicle->in_building = bld;
            }
            act( "You land safely.", ch, NULL, NULL, TO_CHAR );
            act( "$n lands $s spaceship safely.", ch, NULL, NULL, TO_ROOM );
            do_exit(ch,"");
        }
        return;
    }
    ch->c_time = 8;
    ch->c_level++;
    return;
}

void do_reset( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    if ( ch->in_building == NULL || str_cmp(ch->name,ch->in_building->owned))
        send_to_char( "You must be in a building to reset its password.\n\r", ch );
    else
    {
        if ( IS_SET(ch->in_building->value[1],INST_SPOOF) )
            REMOVE_BIT(ch->in_building->value[1],INST_SPOOF);
        ch->in_building->password = number_range(10000,99999);
        ch->in_building->real_dir = number_range(1,ch->in_building->directories);
        sprintf( buf, "Your new building password is %d\n\r", ch->in_building->password );
        send_to_char(buf,ch);
    }
    return;
}

void warp(CHAR_DATA *ch, int range)
{
    int x,y;
    if ( ch->z == Z_SPACE || ch->z == Z_PAINTBALL )
    {
        send_to_char( "The warp failed.\n\r", ch );
        return;
    }
    x = number_range(ch->x - (range), ch->x + (range) );
    y = number_range(ch->y - (range), ch->y + (range) );
    if ( x < 5 )
        x = 5;
    if ( y < 5 )
        y = 5;
    if ( x > MAX_MAPS-5 )
        x = MAX_MAPS - 5;
    if ( y > MAX_MAPS-5 )
        y = MAX_MAPS - 5;

    if ( get_building(x,y,ch->z) != NULL )
    {
        send_to_char( "The warp failed!\n\r", ch );
        act( "Nothing happens!", ch, NULL, NULL, TO_ROOM );
        return;
    }
    act( "You shimmer away!", ch, NULL, NULL, TO_CHAR );
    if ( ch->in_vehicle != NULL )
        act( "$t shimmers away!", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
    else
        act( "$n shimmers away!", ch, NULL, NULL, TO_ROOM );
    move ( ch, x, y, ch->z );
    do_look(ch,"");
    if ( ch->in_vehicle != NULL )
        act( "$t shimmers into the room!", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
    else
        act( "$n shimmers into the room!", ch, NULL, NULL, TO_ROOM );
    return;
}

void do_sblast(CHAR_DATA *ch, char *argument)
{
    int range = 7;
    BUILDING_DATA *bld;
    int i = 0;
    char buf[MSL];
    CHAR_DATA *wch;
    int x,y;

    if ( ( bld = ch->in_building ) == NULL || str_cmp(bld->owned,ch->name) || bld->type != BUILDING_SONIC_BLASTER )
    {
        send_to_char( "You must be inside a Sonic Blaster!\n\r", ch );
        return;
    }
    if ( bld->shield < 30 )
    {
        send_to_char( "It doesn't have enough charge!\n\r", ch );
        return;
    }
    for ( x = bld->x - range;x < bld->x + range + 1;x++ )
        for ( y = bld->y - range;y < bld->y + range + 1;y++ )
    {
        if ( ( wch = get_rand_char(x,y,bld->z) ) == NULL )
            continue;
        if ( wch == ch )
            continue;
        if ( IS_IMMORTAL(wch) )
            continue;
        if ( ch->pcdata->alliance != -1 && wch->pcdata->alliance == ch->pcdata->alliance )
            continue;
        send_to_char( "An electrical ring flows throughout the area, zapping you!\n\r", wch );
        damage( ch, wch, number_fuzzy(30+(bld->level*2)), DAMAGE_PSYCHIC );
        bld->shield -= 30;
        i++;
        if ( bld->shield <= 0 )
        {
            bld->shield = 0;
            break;
        }
    }
    sprintf( buf, "You hit %d people!\n\r", i );
    send_to_char(buf,ch);
    WAIT_STATE(ch,14);
    return;
}

void do_psy_message( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld = ch->in_building;
    CHAR_DATA *wch;
    int range,i = 0;
    char arg[MSL];
    char buf[MSL];

    if ( bld == NULL || bld->type != BUILDING_PSYCHIC_AMPLIFIER )
    {
        send_to_char( "Error!\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "You must specify an order!\n\r", ch );
        return;
    }
    one_argument(argument,arg);
    if ( str_cmp(arg,"demolish")
        && str_prefix(arg,"east")
        && str_prefix(arg,"west")
        && str_prefix(arg,"south")
        && str_prefix(arg,"north")
        && str_cmp(arg,"fire")
        && str_cmp(arg,"shoot")
        && str_cmp(arg,"set")
        && str_cmp(arg,"arm")
        && str_cmp(arg,"drop")
        && str_cmp(arg,"throw")
        && str_cmp(arg,"target")
        && str_cmp(arg,"swap")
        && str_cmp(arg,"security")
        && str_cmp(arg,"exit") )
    {
        send_to_char( "You can't issue this command. Must be:\n\rDemolish, East, West, North, South, Fire, Shoot, Set, Arm, Drop, Throw, Target, Swap, Security, Exit.\n\r", ch );
        return;
    }
    range = 3 + bld->level;
    for ( wch = first_char;wch;wch = wch->next )
    {
        if ( ch->pcdata->alliance != -1 && ch->pcdata->alliance == wch->pcdata->alliance )
            continue;
        if ( IS_IMMORTAL(wch) || IS_NEWBIE(wch) || ch == wch || !same_planet(ch,wch) )
            continue;
        if ( wch->in_room->vnum == ROOM_VNUM_WMAP && (IS_BETWEEN(wch->x,ch->x-range,ch->x+range)) && (IS_BETWEEN(wch->y,ch->y-range,ch->y+range)))
        {
            i++;
            send_to_char( "You suddenly feel a strange urge to do something...\n\r", wch );
            interpret( wch, argument );
        }
    }
    sprintf( buf, "You sense your message got through to %d people.\n\r", i );
    send_to_char(buf,ch);
    WAIT_STATE(ch,24);
    return;
}

void construct_space_vessal( CHAR_DATA *ch, char *argument )
{
    char arg1[MSL];
    char arg2[MSL];
    char buf[MSL];
    VEHICLE_DATA *vhc;
    BUILDING_DATA *bld = ch->in_building;
    int i,t,a,w,cost,type=-1;

    cost = 0;
    argument = one_argument(argument,arg1);
    argument = one_argument(argument,arg2);
    one_argument(argument,argument);

    t = -1;
    a = -1;
    w = -1;
    if ( argument[0] == '\0' || arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: construct SHIPTYPE 'ARMOR' 'WEAPON'\n\rExample: construct frigate 'steel armor 1' 'laser cannons 2'\n\r", ch );
        return;
    }

    if ( bld == NULL || bld->type != BUILDING_SPACE_CENTER || !complete(bld) || str_cmp(bld->owned,ch->name) )
    {
        send_to_char( "You must be in one of your completed space centers.\n\r", ch );
        return;
    }

    for (i=0;s_res_table[i].name != NULL;i++ )
    {
        if ( !IS_SET(bld->value[s_res_table[i].type],s_res_table[i].bit ) )
            continue;
        if ( s_res_table[i].level > bld->level )
            continue;
        if ( !str_cmp(arg1,s_res_table[i].name) )
        {
            if ( s_res_table[i].type != RES_SHIP )
            {
                sprintf( buf, "%s is not a ship type.\n\r", arg1 );
                send_to_char(buf,ch);
                return;
            }
            cost = cost + s_res_table[i].cost*100;
            t = i;
        }
        else if ( !str_cmp(arg2,s_res_table[i].name) )
        {
            if ( s_res_table[i].type != RES_ARMOR )
            {
                sprintf( buf, "%s is not an armor type.\n\r", arg2 );
                send_to_char(buf,ch);
                return;
            }
            cost = cost + s_res_table[i].cost*100;
            a = i;
        }
        else if ( !str_cmp(argument,s_res_table[i].name) )
        {
            if ( s_res_table[i].type != RES_WEAPON )
            {
                sprintf( buf, "%s is not a weapon type.\n\r", argument );
                send_to_char(buf,ch);
                return;
            }
            cost = cost + s_res_table[i].cost*100;
            w = i;
        }
    }
    if ( t == -1 )
    {
        sprintf( buf, "%s - You do not know of such ship type.\n\r", arg1 );
        send_to_char( buf, ch );
        return;
    }
    else if ( w == -1 )
    {
        sprintf( buf, "%s - You do not know of such weapon type.\n\r", argument );
        send_to_char( buf, ch );
        return;
    }
    else if ( a == -1 )
    {
        sprintf( buf, "%s - You do not know of such armor type.\n\r", arg2 );
        send_to_char( buf, ch );
        return;
    }
    if ( bld->value[10] < cost )
    {
        sprintf( buf, "The cost to create this is %d. You only have %d deposited.\n\r", cost, bld->value[10] );
        send_to_char(buf,ch);
        return;
    }

    if ( s_res_table[t].bit == RESEARCH_S_SCOUT )
    {
        type = VEHICLE_SCOUT;
    }
    else if ( s_res_table[t].bit == RESEARCH_S_FIGHTER )
        type = VEHICLE_FIGHTER;
    else if ( s_res_table[t].bit == RESEARCH_S_FRIGATE )
        type = VEHICLE_FRIGATE;
    else if ( s_res_table[t].bit == RESEARCH_S_BATTLECRUISER )
        type = VEHICLE_BATTLECRUISER;
    else if ( s_res_table[t].bit == RESEARCH_S_DESTROYER )
        type = VEHICLE_DESTROYER;
    else if ( s_res_table[t].bit == RESEARCH_S_STARBASE )
        type = VEHICLE_STARBASE;

    if ( s_res_table[a].value3 > type )
    {
        sprintf( buf, "You cannot fit %s onto a %s.\n\r", s_res_table[a].name, s_res_table[t].name );
        send_to_char(buf,ch);
        return;
    }
    if ( s_res_table[w].value3 > type )
    {
        sprintf( buf, "You cannot fit %s onto a %s.\n\r", s_res_table[w].name, s_res_table[t].name );
        send_to_char(buf,ch);
        return;
    }

    vhc = create_vehicle(type);
    if ( s_res_table[t].bit == RESEARCH_S_SCOUT )
    {
        vhc->max_hit = s_res_table[a].value / 10;
        vhc->hit = vhc->max_hit;
    }
    //	sprintf( buf, "Vehicle Space %s", s_res_table[t].name );
    //	free_string(vhc->name);
    //	vhc->name = str_dup(buf);
    //	sprintf( buf, "A %s Spaceship", s_res_table[t].name );
    //	free_string(vhc->desc);
    //	vhc->desc = str_dup(buf);
    move_vehicle(vhc,bld->x,bld->y,bld->z);
    vhc->max_fuel = 1000;
    vhc->fuel = 1000;
    vhc->max_hit = s_res_table[a].value;
    vhc->hit = vhc->max_hit;
    vhc->max_ammo = 9999;
    vhc->ammo = vhc->max_ammo;
    vhc->ammo_type = s_res_table[w].value;
    vhc->speed = s_res_table[t].value;
    vhc->scanner = s_res_table[t].value2;
    vhc->range = s_res_table[w].value2 + s_res_table[t].value3;
    bld->value[10] -= cost;

    vhc->max_ammo += (vhc->max_ammo/100)* number_range(0,ch->pcdata->skill[gsn_engineering]*2);
    vhc->max_fuel += (vhc->max_fuel/100)* number_range(0,ch->pcdata->skill[gsn_engineering]*2);
    vhc->max_hit += (vhc->max_hit/100)* number_range(0,ch->pcdata->skill[gsn_engineering]*2);
    vhc->hit = vhc->max_hit;
    vhc->fuel = vhc->max_fuel;
    vhc->ammo = vhc->max_ammo;
    vhc->range += number_range(0,ch->pcdata->skill[gsn_engineering]);
    act( "You construct the $t!", ch, vhc->desc, vhc->desc, TO_CHAR );
    act( "$n constructs a $t!", ch, vhc->desc, vhc->desc, TO_ROOM );
    return;
}

void space_mine( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj = NULL;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    if ( ch->carry_number > 100 )
    {
        send_to_char( "You can't carry any more items.\n\r", ch );
        return;
    }
    for ( obj = map_obj[ch->x][ch->y];obj;obj = obj->next_in_room )
        if ( obj->item_type == ITEM_ASTEROID )
            break;

    if ( obj == NULL || obj->item_type != ITEM_ASTEROID )
    {
        send_to_char( "There is no asteroid here!\n\r", ch );
        return;
    }
    send_to_char( "You begin mining... Type '@@estop@@N' to stop.\n\r", ch );
    ch->c_sn = gsn_mine;
    ch->c_time = 80;
    if ( ch->in_vehicle && IS_SET(ch->in_vehicle->flags,VEHICLE_MINING_BEAM) )
        ch->c_time /= 2;
    return;
}

void act_mine( CHAR_DATA *ch, int level )
{
    OBJ_DATA *obj = NULL;
    OBJ_DATA *obj2;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
    char buf[MSL];
    int i,chance;

    for ( obj = map_obj[ch->x][ch->y];obj;obj = obj->next_in_room )
        if ( obj->item_type == ITEM_ASTEROID )
            break;

    if ( obj == NULL || obj->item_type != ITEM_ASTEROID )
    {
        send_to_char( "The asteroid has left your range.\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( obj->value[1] <= 0 )
    {
        send_to_char( "This asteroid has been depleated.\n\r", ch );
        ch->c_sn = -1;
        return;
    }
    if ( IS_SET(ch->in_vehicle->flags,VEHICLE_TREASURE_BEAM) )
        chance = 15;
    else
        chance = 1;
    if ( number_percent() <= chance && number_percent() <= 50 )
    {
        if ( number_percent() < 25 )
            obj2 = create_object(get_obj_index(OBJ_VNUM_BLACK_POWDER),0);
        else if ( number_percent() < 33 )
            obj2 = create_object(get_obj_index(32668),0);
        else if ( number_percent() < 50 )
            obj2 = create_object(get_obj_index(799),0);
        else if ( number_percent() < 25 )
            obj2 = create_object(get_obj_index(501),0);
        else
            obj2 = create_object(get_obj_index(940),0);
    }
    else
    {
        i = number_range(1,5);
        if ( IS_SET(ch->in_vehicle->flags,VEHICLE_ALIEN_MAGNET) )
            obj2 = create_material(-1);
        else
            obj2 = create_material(obj->value[0]);
        obj2->value[1] = i;
        obj->value[0] -= i;
    }
    obj_to_char(obj2,ch);
    sprintf( buf, "You extract %s from the asteroid.\n\r", obj2->short_descr );
    act( "$n pulls some resources from the asteroid.", ch, NULL, NULL, TO_ROOM );
    send_to_char(buf,ch);
    ch->c_time = 80;
    if ( ch->in_vehicle && IS_SET(ch->in_vehicle->flags,VEHICLE_MINING_BEAM) )
        ch->c_time /= 2;
    return;
}

void do_backup_building( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int i,f;
    char buf[MSL];
    char arg[MSL];
    BUILDING_DATA *bld;

    argument = one_argument(argument,arg);
    if ( !str_cmp(arg,"save") )
    {
        f = 1;
    }
    else if ( !str_cmp(arg,"load") )
    {
        f = 2;
    }
    else
    {
        send_to_char( "Syntax: backup save/load <disk>\n\r", ch );
        return;
    }
    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You must be in a building.\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL || obj->item_type != ITEM_BACKUP_DISK )
    {
        send_to_char( "You must specify a backup disk to save or load the data on.\n\r", ch );
        return;
    }
    if ( f == 1 )
        for ( i=0;i<10;i++ )
            obj->value[i] = bld->value[i];
    if ( f == 2 )
    {
        if ( bld->type != obj->value[2] )
        {
            send_to_char( "This disk doesn't contain data for this building.\n\r", ch );
            return;
        }
        for ( i=0;i<10;i++ )
            if ( i != 2 )
                bld->value[i] = obj->value[i];
        send_to_char( "Data Loaded.\n\r", ch );
        return;
    }
    obj->level = bld->level;
    obj->value[2] = bld->type;
    sprintf( buf, "A L%d %s Backup Disk\n\r", obj->level, build_table[obj->value[2]].name );
    free_string(obj->short_descr);
    free_string(obj->description);
    obj->short_descr = str_dup(buf);
    obj->description = str_dup(buf);
    send_to_char( "Backup Made.\n\r", ch );
    return;
}

void do_mspend( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    char arg1[MSL];
    char arg2[MSL];
    int cost=0,item=-1;

    sprintf( buf, "Syntax: mspend buy <item>\n\r        mspend profreset\n\r" );
    if ( argument[0] == '\0' )
    {
        send_to_char( buf,ch );
        return;
    }
    argument = one_argument(argument,arg1);
    argument = one_argument(argument,arg2);
    if ( !str_prefix(arg1,"buy") )
    {
        OBJ_DATA *obj;
        if ( arg2[0] == '\0' || !str_cmp(arg2,"list") )
        {
            send_to_char( "\n\r@@WItem Name         Cost   Description\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W\\\n\r", ch );
            send_to_char( "@@bViral Enhancer   @@g|  @@y1  @@g| @@yTurns a doomsday device into a REAL doomsday.@@g|\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W<\n\r", ch );
            send_to_char( "@@bQuest Points     @@g|  @@y1  @@g| @@yTrade for 100 QPs.                           @@g|\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W<\n\r", ch );
            send_to_char( "@@bSpace Center BPs @@g|  @@y2  @@g| @@yIncrease space center level.                 @@g|\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W<\n\r", ch );
            send_to_char( "@@b100 Alien Metal  @@g|  @@y2  @@g| @@yA free-for-all resource.                     @@g|\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W<\n\r", ch );
            send_to_char( "@@b30X-Zoom Scope   @@g|  @@y3  @@g| @@yDramatically decreases targetting time.      @@g|\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W<\n\r", ch );
            send_to_char( "@@bLvl 2 Master BPs @@g|  @@y3  @@g| @@yMaster L2 Blueprints that last.              @@g|\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W<\n\r", ch );
            send_to_char( "@@bLvl 3 Master BPs @@g|  @@y6  @@g| @@yMaster L3 Blueprints that last.              @@g|\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W<\n\r", ch );
            send_to_char( "@@bSticky Weapon    @@g|  @@y6  @@g| @@ySave-through-death Weapon Install.           @@g|\n\r", ch );
            send_to_char( "@@g----------------------------------------------------------------------@@W/\n\r", ch );
            return;
        }
        else if ( !str_prefix(arg2,"viral enhancer") )
        {
            item = 32665;
            cost = 1;
        }
        else if ( !str_prefix(arg2,"space center bps") )
        {
            item = 799;
            cost = 2;
        }
        else if ( !str_prefix(arg2,"100 alien metal") )
        {
            item = 500;
            cost = 2;
        }
        else if ( !str_prefix(arg2,"30X-Zoom Scope") )
        {
            item = 32663;
            cost = 3;
        }
        else if ( !str_prefix(arg2,"Quest Points") )
        {
            item = 32662;
            cost = 1;
        }
        else if ( !str_prefix(arg2,"lvl 2 master bps") )
        {
            item = 32661;
            cost = 3;
        }
        else if ( !str_prefix(arg2,"lvl 3 master bps") )
        {
            item = 32660;
            cost = 6;
        }
        else if ( !str_prefix(arg2,"sticky weapon") )
        {
            item = 32595;
            cost = 6;
        }
        if ( item == -1 )
        {
            send_to_char( "That is not a valid item.\n\r", ch );
            return;
        }
        if ( ch->medals < cost )
        {
            send_to_char( "Riiight, try again when you have enough medals.\n\r", ch );
            return;
        }
        obj = create_object(get_obj_index(item),0);
        obj_to_char(obj,ch);
        ch->medals -= cost;
        act( "You bought $p.", ch, obj, NULL, TO_CHAR );
        act( "$n bought $p.", ch, obj, NULL, TO_ROOM );
        return;
    }
    else if ( !str_cmp(arg1,"profreset") )
    {
        if ( !str_cmp(arg2,"Sure") )
        {
            int i;
            if ( ch->medals < 15 )
            {
                send_to_char( "You need 15 medals to reset the proficiencies.\n\r", ch );
                return;
            }
            i = ch->pcdata->skill[gsn_engineering] + ch->pcdata->skill[gsn_building] + ch->pcdata->skill[gsn_combat];
            ch->pcdata->skill[gsn_engineering] = 0;
            ch->pcdata->skill[gsn_building] = 0;
            ch->pcdata->skill[gsn_combat] = 0;
            ch->pcdata->prof_points = i;
            ch->medals -= 15;
            return;
        }
        send_to_char( "Type: mspend profreset sure\n\rTo approve.\n\r", ch );
        return;
    }
    send_to_char( buf, ch );
    return;
}

void do_run( CHAR_DATA * ch, char *argument )
{
    char arg[MSL];
    int dir, loop;
    argument = one_argument(argument,arg);
    if ( ch->fighttimer > 0 )
    {
        send_to_char( "Not during combat.\n\r", ch );
        return;
    }
    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char( "Syntax: run x dir  (run 10 west)\n\r", ch );
        return;
    }
    if ( ch->z != Z_GROUND && ch->z != Z_NEWBIE && ch->z != Z_UNDERGROUND && ch->z != Z_PAINTBALL )
    {
        send_to_char( "You can't walk here, how do you expect to run?\n\r", ch );
        return;
    }
    if ( IS_BUSY(ch) || ch->pcdata->queue )
    {
        send_to_char( "You must not be busy, and have an empty command queue to use this.\n\r", ch );
        return;
    }
    if ( medal(ch) )
    {
        send_to_char("Not in here.\n\r", ch );
        return;
    }
    dir = parse_direction(ch,argument);
    loop = atoi(arg);
    if ( loop <= 1 || loop > 50 )
    {
        send_to_char( "Running amount must be between 2 and 50.\n\r", ch );
        return;
    }
    if ( dir == -1 )
    {
        send_to_char( "Valid directions are: North, South, East, West.\n\r", ch );
        return;
    }
    send_to_char( "You begin running...\n\r", ch );
    WAIT_STATE(ch,loop);
    if ( !IS_SET(ch->effect,EFFECT_RUNNING) )
        SET_BIT(ch->effect,EFFECT_RUNNING);
    for ( ;loop > 0;loop-- )
        add_to_queue(ch,argument);
    check_queue(ch);
    return;
}

void do_cloneflag( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    if ( !str_cmp(argument,"off") )
    {
        free_string(ch->pcdata->who_name);
        ch->pcdata->who_name = str_dup("off");
        send_to_char("Done.\n\r", ch );
        return;
    }
    if ( ch->pcdata->alliance == -1 )
    {
        send_to_char( "You can only copy flags from your alliance members.\n\r", ch );
        return;
    }
    if ( ( victim = get_char_world(ch,argument) ) == NULL )
    {
        send_to_char( "You can't find that person.\n\r", ch );
        return;
    }
    if ( victim->pcdata->alliance != ch->pcdata->alliance )
    {
        send_to_char( "Target must be in the same alliance as you are.\n\r", ch );
        return;
    }
    free_string(ch->pcdata->who_name);
    ch->pcdata->who_name = str_dup(victim->pcdata->who_name);
    send_to_char("Done.\n\r", ch );
    return;
}

void do_blindupdate( CHAR_DATA *ch, char *argument )
{
    int i;
    if ( !IS_SET(ch->config,CONFIG_BLIND) )
    {
        send_to_char( "This option is meant for people who are blind in RL.\n\r", ch );
        return;
    }
    i = atoi(argument);
    if ( !is_number(argument) || i <= 0 || i > 100 )
    {
        send_to_char( "Syntax: blindupdate <interval>\n\rWhere the second interval between updates is <interval> X 3 seconds.\n\r", ch );
        return;
    }
    ch->pcdata->spec_init = i;
    send_to_char( "Blind update interval set.\n\r", ch );
    return;
}

void do_oresearch( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    char buf[MSL];

    if ( ch->z != Z_SPACE )
    {
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: research <ore>\n\rYou must specify an ore item to research!\n\r", ch );
        return;
    }
    if ( ( obj = get_obj_carry(ch,argument) ) == NULL )
    {
        send_to_char( "You do not carry that ore.\n\r", ch );
        return;
    }
    if ( obj->item_type != ITEM_ORE )
    {
        sprintf(buf, "%s is not a researchable ore.\n\r", obj->short_descr );
        send_to_char(buf,ch);
        return;
    }
    ch->c_level = obj->value[0];
    ch->c_sn = gsn_oreresearch;
    ch->c_time = 40;
    ch->c_obj = obj;
    act( "You begin studying $p.", ch, obj, NULL, TO_CHAR );
    act( "$n begins studying $p.", ch, obj, NULL, TO_ROOM );
    return;
}

void act_oresearch(CHAR_DATA *ch, int level)
{
    OBJ_DATA *obj = ch->c_obj;
    char buf[MSL];
    int i,m=5;

    if ( !obj || obj->item_type != ITEM_ORE )
    {
        send_to_char( "For some reason, you have lost the ore you were researching!\n\r", ch );
        ch->c_obj = NULL;
        ch->c_level = -1;
        ch->c_sn = -1;
        return;
    }
    if ( ch->z != Z_SPACE )
    {
        send_to_char( "You are only able to continue researching in space.\n\r", ch );
        ch->c_obj = NULL;
        ch->c_level = -1;
        ch->c_sn = -1;
        return;
    }
    if ( ch->class == CLASS_GENIUS && number_percent() < 33 )
        m=8;
    ch->c_level -= m;
    ch->c_obj->value[0] -= m;
    if ( ch->c_level <= 0 )
    {
        OBJ_DATA *n_obj;
        bool stop = FALSE;

        act( "You have completed your research!", ch, NULL, NULL, TO_CHAR );
        act( "$n finishes researching the ore.", ch, NULL, NULL, TO_ROOM );
        i=0;
        while ( !stop )
        {
            i++;
            if ( number_percent() < 80 || obj->value[i+1] <= 0 )
                //				stop = TRUE;
                break;
        }
        if ( ( n_obj = create_object(get_obj_index(obj->value[i]),0) ) == NULL )
        {
            send_to_char( "There was an error with your new item, please contact an imm to check your ore's values.\n\r", ch );
            return;
        }
        obj_to_char(n_obj,ch);
        act( "You managed to extract $p!", ch, n_obj, NULL, TO_CHAR );
        act( "$n managed to extract $p!", ch, n_obj, NULL, TO_ROOM );
        ch->c_sn = -1;
        ch->c_obj = NULL;
        ch->c_level = -1;
        extract_obj(obj);
        return;
    }
    act( "You continue researching $p.", ch, obj, NULL, TO_CHAR );
    act( "$n continues researching the ore.", ch, NULL, NULL, TO_ROOM );
    if ( number_percent() < 25 )
    {
        sprintf( buf, "You estimate it would last another %d seconds.\n\r", ch->c_level );
        send_to_char(buf,ch);
    }
    ch->c_time = 40;
    return;
}

void do_use( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    char arg[MSL];
    char *arg2;
    char cmd[MSL];
    char buf[MSL];
    cmd[0] = '\0';

    arg2 = one_argument(argument,arg);
    if ( ( obj = get_obj_carry(ch,arg) ) != NULL )
    {
        if ( obj->item_type == ITEM_AMMO )
            sprintf(cmd,"load %s",argument);
        else if ( obj->item_type == ITEM_BOMB )
            sprintf(cmd," %s",argument);
        else if ( obj->item_type == ITEM_BLUEPRINT )
            sprintf(cmd,"upgrade %s",argument);
        else if ( obj->item_type == ITEM_INSTALLATION )
            sprintf(cmd,"install %s",argument);
        else if ( obj->item_type == ITEM_WEAPON )
            sprintf(cmd,"wear %s",argument);
        else if ( obj->item_type == ITEM_ARMOR )
            sprintf(cmd,"wear %s",argument);
        else if ( obj->item_type == ITEM_SUIT )
            sprintf(cmd,"wear %s",argument);
        else if ( obj->item_type == ITEM_MEDPACK )
            sprintf(cmd,"heal %s",arg);
        else if ( obj->item_type == ITEM_TELEPORTER )
            sprintf(cmd,"teleport %s",arg);
        else if ( obj->item_type == ITEM_IMPLANT )
            sprintf(cmd,"implant %s",arg);
        else if ( obj->item_type == ITEM_LOCATOR )
            sprintf(cmd,"locate");
        else if ( obj->item_type == ITEM_ORE )
            sprintf(cmd,"research %s",arg);
        else if ( obj->item_type == ITEM_PIECE )
            sprintf(cmd,"connect %s",argument);
        else if ( obj->item_type == ITEM_VEHICLE_UP )
            sprintf(cmd,"vinstall %s",arg);
        else if ( obj->item_type == ITEM_WEAPON_UP )
            sprintf(cmd,"winstall %s",arg);
        else if ( obj->item_type == ITEM_BIOTUNNEL )
            sprintf(cmd,"settunnel %s",argument);
        else
        {
            send_to_char( "There is nothing you can do with it using this command. Try using the IDENTIFY command.\n\r", ch );
            return;
        }
    }
    else if ( ( bld = ch->in_building ) != NULL )
    {
        if ( bld->type == BUILDING_ARMORY )
            sprintf(cmd,"generate %s",argument);
        else if ( bld->type == BUILDING_MINE || bld->type == BUILDING_IMPROVED_MINE || bld->type == BUILDING_LUMBERYARD )
            sprintf(cmd,"mine %s",argument);
        else if ( bld->type == BUILDING_FORGE )
            sprintf(cmd,"chunk %s",argument);
        else if ( bld->type == BUILDING_ARMORER )
            sprintf(cmd,"generate %s",argument);
        else if ( bld->type == BUILDING_STORAGE )
            sprintf(cmd,"generate %s",argument);
        else if ( bld->type == BUILDING_SNIPER_TOWER )
            sprintf(cmd,"target %s",argument);
        else if ( bld->type == BUILDING_AIRFIELD )
            sprintf(cmd,"construct %s",argument);
        else if ( bld->type == BUILDING_GARAGE )
            sprintf(cmd,"construct %s",argument);
        else if ( bld->type == BUILDING_PARADROP )
            sprintf(cmd,"paradrop %s",argument);
        else if ( bld->type == BUILDING_PROCESSING_PLANT )
            sprintf(cmd,"help chemistry");
        else if ( bld->type == BUILDING_WAR_CANNON )
            sprintf(cmd,"blast %s",argument);
        else if ( bld->type == BUILDING_BOOM )
            sprintf(cmd,"boom");
        else if ( bld->type == BUILDING_IMPLANT_RESEARCH )
            sprintf(cmd,"implant %s",argument);
        else if ( bld->type == BUILDING_SCUD_LAUNCHER )
            sprintf(cmd,"blast %s",argument);
        else if ( bld->type == BUILDING_NUKE_LAUNCHER )
            sprintf(cmd,"blast %s",argument);
        else if ( bld->type == BUILDING_GUNNER )
            sprintf(cmd,"blast %s",argument);
        else if ( bld->type == BUILDING_DOOMSDAY_DEVICE )
            sprintf(cmd,"doom %s",argument);
        else if ( bld->type == BUILDING_RADAR )
            sprintf(cmd,"scan %s",argument);
        else if ( bld->type == BUILDING_ALIEN_PROBE )
            sprintf(cmd,"spy %s",argument);
        else if ( bld->type == BUILDING_WARP )
            sprintf(cmd,"warp %s",argument);
        else if ( bld->type == BUILDING_REFINERY )
            sprintf(cmd,"refine %s",argument);
        else if ( bld->type == BUILDING_MARKETPLACE )
            sprintf(cmd,"sell %s",argument);
        else if ( bld->type == BUILDING_ONLINE_MARKET )
            sprintf(cmd,"trade %s",argument);
        else if ( bld->type == BUILDING_HACKERS_HIDEOUT )
            sprintf(cmd,"trace %s",argument);
        else if ( bld->type == BUILDING_HACKPORT )
            sprintf(cmd,"doom %s",argument);
        else if ( bld->type == BUILDING_SPY_QUARTERS || bld->type == BUILDING_SPY_SATELLITE || bld->type == BUILDING_SHOCKWAVE )
            sprintf(cmd,"spy %s",argument);
        else if ( bld->type == BUILDING_SONIC_BLASTER )
            sprintf(cmd,"blast %s",argument);
        else if ( bld->type == BUILDING_PSYCHIC_AMPLIFIER )
            sprintf(cmd,"message %s",argument);
        else if ( bld->type == BUILDING_PSYCHIC_TORMENTOR )
            sprintf(cmd,"torment %s",argument);
        else if ( bld->type == BUILDING_SPACE_CENTER )
            sprintf(cmd,"research %s",argument);
        else if ( bld->type == BUILDING_ATOM_BOMBER )
            sprintf(cmd,"blast %s",argument);
        else
        {
            send_to_char( "There is nothing you can do here.\n\r", ch );
            return;
        }
    }
    else
    {
        send_to_char( "You must either be in a building, or specify an object to use.\n\r", ch );
        return;
    }
    sprintf(buf, "Using command: %s\n\r", cmd );
    send_to_char(buf,ch);
    interpret(ch,cmd);
    return;
}

void do_setexit( CHAR_DATA *ch, char *argument )
{
    int dir;
    if ( !str_cmp(argument,"all") )
    {
        dir = -1;
    }
    else
    {
        if ( ( dir = parse_direction(ch,argument) ) == -1 )
        {
            send_to_char( "Valid default building exits are: North, West, East, South, All.\n\r", ch );
            return;
        }
    }
    ch->pcdata->set_exit = dir;
    send_to_char( "Done.\n\r", ch );
    do_save(ch,"");
    return;
}

void do_mute( CHAR_DATA *ch, char *argument )
{
    send_to_char("!!MUSIC(off)\n\r", ch );
    return;
}

void do_settunnel( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    int x,y;
    char arg[MSL];
    char arg2[MSL];
    char buf[MSL];
    bool found = FALSE;

    argument = one_argument(argument,arg);
    argument = one_argument(argument,arg2);

    if ( arg[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char( "Syntax: set <item> <x> <y>\n\r", ch );
        return;
    }

    if ( ( obj = get_obj_here(ch,arg) ) == NULL )
    {
        sprintf( buf,"You can't find a %s here.\n\r", arg );
        send_to_char( buf,ch);
        return;
    }
    if ( obj->item_type != ITEM_BIOTUNNEL )
    {
        send_to_char( "That is not a bio tunnel.\n\r", ch );
        return;
    }

    if ( obj->value[0] != 1 )
    {
        send_to_char( "That is not a transmitting biotunnel.\n\r", ch );
        return;
    }
    x = atoi(arg2);
    y = atoi(argument);

    if ( x < BORDER_SIZE || y < BORDER_SIZE || x > MAX_MAPS - BORDER_SIZE || y > MAX_MAPS - BORDER_SIZE )
    {
        send_to_char( "Invalid coords.\n\r", ch );
        return;
    }
    if ( abs(obj->x - x) > obj->value[1] || abs(obj->y - y) > obj->value[1] )
    {
        send_to_char( "That is out of range of the tunnel.\n\r", ch );
        return;
    }
    for ( obj2 = map_obj[x][y];obj2;obj2 = obj2->next_in_room )
    {
        if ( obj2->z != ch->z )
            continue;
        if ( obj2->item_type != ITEM_BIOTUNNEL || obj2->value[0] != 0 )
            continue;

        found = TRUE;
        break;
    }
    if ( !found )
    {
        send_to_char( "There is no receiving bio tunnel there.\n\r", ch );
        return;
    }
    obj->value[2] = x;
    obj->value[3] = y;
    sprintf( buf, "@@gYou set %s @@gto @@y%d@@g/@@y%d@@g.@@N\n\r", obj->short_descr, x, y );
    send_to_char( buf,ch);
    return;
}

void respawn_buildings(CHAR_DATA *ch)
{
    BUILDING_DATA *bld;
    OBJ_DATA *obj;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    for ( bld = ch->first_building;bld;bld = bld->next_owned )
    {
        if ( bld->x < 0 )
            bld->x *= -1;
        if ( bld->y < 0 )
            bld->y *= -1;
        map_bld[bld->x][bld->y][bld->z] = bld;
        bld->hp = bld->maxhp;
        bld->shield = bld->maxshield;
        bld->value[9] = 0; bld->value[3] = 0;
        for ( obj = map_obj[bld->x][bld->y];obj;obj = obj->next )
            if ( obj->z == bld->z && obj->value[1] != 0 && obj->item_type == ITEM_BOMB )
                if ( !str_cmp(obj->owner,ch->name) )
                    obj->value[1] = 0;
    }
    save_char_obj(ch);
    return;
}

//pager
void send_page(char *from, char *target, char *msg)
{
    PAGER_DATA *p;
    CHAR_DATA *ch = get_ch(from);
    CHAR_DATA *victim;
    bool found;
    DESCRIPTOR_DATA d;

    if ( target[0] == '\0' || from[0] == '\0' || msg[0] == '\0' )
        return;

    if ( ( victim = get_ch(target) ) == NULL )
    {
        found = load_char_obj(&d,target,FALSE);
        if ( found )
        {
            if ( ch )
                send_to_char( "There is no such player.\n\r", ch );
            return;
        }
    }
    GET_FREE(p,pager_free);
    if ( p->from != NULL )
        free_string(p->from);
    if ( p->msg != NULL )
        free_string(p->msg);
    if ( p->time != NULL )
        free_string(p->time);
    p->is_free = FALSE;
    p->prev = NULL;
    p->next = victim->pcdata->pager;
    p->time = str_dup((char *) ctime( &current_time ));
    p->from = str_dup(from);
    p->msg = str_dup(msg);
    victim->pcdata->pager = p;
    save_char_obj(victim);
    if ( ch )
        send_to_char("Page sent\n\r.", ch );
    return;
}

void show_pager(CHAR_DATA *ch)
{
    PAGER_DATA *p;
    char buf[MSL];
    int i=0;

    send_to_char("@@dl=--------------------=------------------=\n\r", ch );
    send_to_char("!@@GX________________________________________@@d\\\n\r", ch );
    send_to_char("!@@G|                                        @@d|\n\r", ch );
    for(p=ch->pcdata->pager;p;p=p->next)
    {
        i++;
        sprintf(buf,"@@d|@@G| @@e(%2d)@@r %-8s@@g %24s@@d |\n\r", i,p->from,p->time);
        send_to_char( buf,ch );
    }
    send_to_char("!@@G|                                        @@d|\n\r", ch );
    send_to_char("!@@G|                                        @@d|\n\r", ch );
    send_to_char("!@@G| (@@rW@@G)rite   (@@rD@@G)elete   (@@rQ@@G)uit           @@d |\n\r", ch );
    send_to_char("L=========================================/\n\r", ch );
    return;
}

void do_pager(CHAR_DATA *ch,char *argument)
{
    show_pager(ch);
    return;
}
