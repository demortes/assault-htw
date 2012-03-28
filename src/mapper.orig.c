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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "mapper.h"

extern char * compass_name[];

int door_marks[4][2] =
{
    {
        -1, 0
    }
    ,
    {
        0, 1
    }
    ,
    {
        1, 0
    }
    ,
    {
        0,-1
    }
};
int offsets[4][2] =
{
    {
        -2, 0
    }
    ,
    {
        0, 2
    }
    ,
    {
        2, 0
    }
    ,
    {
        0,-2
    }
};

#define SECT_HERE   SECT_MAX
#define SECT_UNSEEN ( SECT_MAX + 1 )
#define SECT_BLOCKED    ( SECT_UNSEEN + 1 )
#define SECT_TOP    ( SECT_BLOCKED + 1 )

void do_mapper( CHAR_DATA *ch, char *argument )
{
    int size = 0;
    if ( ch->z == Z_PAINTBALL && ch->x == 2 && ch->y == 2 )
        return;
    if ( argument[0] != '\0' )
        size = atoi(argument);
    if ( IS_SET( ch->config, CONFIG_BLIND ) )
    {
        ShowBMap( ch, FALSE );
        return;
    }
    if ( ch->z == Z_SPACE )
    {
        ShowSMap( ch, IS_SET(ch->config,CONFIG_SMALLMAP) );
        return;
    }
    if ( IS_SET( ch->config, CONFIG_CLIENT) )
    {
        ShowCMap( ch );
        return;
    }
    ShowWMap( ch, IS_SET( ch->config, CONFIG_SMALLMAP)?2:IS_SET(ch->config,CONFIG_TINYMAP)?1:4, size );
    return;

}

void ShowCMap( CHAR_DATA *ch )
{
    int x,y,z=ch->z,sect;
    int x1,x2,y1,y2;
    char buf[MSL];
    y1 = ch->y - 10;
    x1 = ch->x - 10;
    y2 = ch->y + 10;
    x2 = ch->x + 10;
    send_to_char( "\e[9z", ch );
    for ( x=x1;x<=x2;x++ )
    {
        for ( y=y1;y<=y2;y++ )
        {
            sect = map_table.type[x][y][z];
            sprintf ( buf, "%s%d", (sect < 10)?"0":"",sect );
            send_to_char(buf,ch);
        }
    }
    send_to_char( "\e[9z", ch );
    return;
}

void ShowWMap( CHAR_DATA *ch, sh_int small, int size )
{
    int x,y,z=ch->z, looper, maxx,i=0;
    bool warcannon = FALSE;
    bool xray = FALSE;
    char scan[MSL];
    char color[MSL];
    char outbuf[MSL];
    char catbuf[MSL];
    char borderbuf[MSL];
    bool has_structure[MAX_MAPS][MAX_MAPS];
    bool in_border = TRUE;
    bool base = FALSE;
    bool charr = FALSE;
    bool def = FALSE;
    bool pit = FALSE;
    BUILDING_DATA *bld = first_building;
    bool enemy = FALSE;
    outbuf[0] = '\0';
    color[0] = '\0';
    scan[0] = '\0';
    borderbuf[0] = '\0';

    if ( medal(ch) )
    {
        send_to_char( "You must find the spot where A Medal Lies Here , pick it up, and leave the medal arena.\n\r", ch );
    }
    if ( IN_PIT(ch) )
    {
        pit = TRUE;
    }
    maxx = ch->map / 2;
    if ( IS_SET(ch->config,CONFIG_LARGEMAP) && size != 998 && size != 997 )
        maxx = ch->map;

    if ( size == 996 )
        base = TRUE;
    if ( size == 999 )
        warcannon = TRUE;

    if ( map_bld[ch->x][ch->y][ch->z] && warcannon == FALSE && size != 997 && !base && size != 998 )
    {
        char_to_building(ch,map_bld[ch->x][ch->y][ch->z]);
        show_building(ch,small,size);
        return;
    }
    if ( ch->in_vehicle && ch->in_vehicle->type == VEHICLE_XRAY && z == Z_GROUND )
        xray = TRUE;

    if ( size != 998 )                                      /* For quest calls */
    {
        char sbuf[MSL];
        int s;
        borderbuf[0] = '\0';
        sbuf[0] = '\0';
        for ( s = 0;s < ch->map;s++ )
        {
            sprintf( sbuf+strlen(sbuf), makesmall("    ",small) );
        }
        if ( ch->z != Z_UNDERGROUND && !IS_SET(ch->pcdata->pflags,PFLAG_HELPING))
            sprintf( borderbuf, "\n\r%s@@l(@@W%d@@g,@@W%d@@l) @@R[@@e%s@@R]@@N", sbuf, ch->x, ch->y, planet_table[ch->z].name );
        else
            sprintf( borderbuf, "\n\r%s@@l(@@W??@@g,@@W??@@l) @@R[@@e%s@@R]@@N", sbuf, planet_table[ch->z].name );
        sprintf( borderbuf+strlen(borderbuf), "\n\r%s%s%s\n\r@@N", sbuf, wildmap_table[map_table.type[ch->x][ch->y][ch->z]].color,  wildmap_table[map_table.type[ch->x][ch->y][ch->z]].name );
    }

    sprintf( outbuf, "%s", "\n\r" );
    sprintf( borderbuf+strlen(borderbuf), "%s", "@@a*@@c-" );

    sprintf( catbuf, "%s", makesmall("----",small) );

    for ( looper = 0; looper <= ch->map*2; looper++ )
    {
        safe_strcat( MSL, borderbuf, catbuf );
    }
    safe_strcat( MSL, borderbuf, "-@@a*@@N" );
    send_to_char( "\n\r", ch );
    /* this is the top line of the map itself, currently not part of the mapstring */
    if ( !warcannon )
        send_to_char( borderbuf, ch );

    for (y = ch->y + maxx; y >= ch->y - maxx; --y)
    {                                                       /* every row */
        safe_strcat( MSL, outbuf, "@@c| " );
        sprintf( color, "@@c" );
        for (x = ch->x - ch->map; x <= ch->x + ch->map; ++x)
        {                                                   /* every column */

            if ( x >= BORDER_SIZE && y >= BORDER_SIZE && x <= MAX_MAPS-BORDER_SIZE && y <= MAX_MAPS-BORDER_SIZE )
                in_border = TRUE;
            else
                in_border = FALSE;

            if (in_border)
            {
                if ( size == 998 )
                    has_structure[x][y] = FALSE;
                else if ( xray )
                    has_structure[x][y] = map_bld[x][y][Z_UNDERGROUND]?TRUE:FALSE;
                else if ( map_bld[x][y][z] == NULL )
                    has_structure[x][y] = FALSE;
                else
                    has_structure[x][y] = TRUE;
            }
            else
                has_structure[x][y] = FALSE;

            if ( pit && ( x < PIT_BORDER_X || y < PIT_BORDER_Y ) )
                in_border = FALSE;
            if ( !pit && ( x > PIT_BORDER_X && y > PIT_BORDER_Y ) && ch->z == Z_PAINTBALL )
                in_border = FALSE;
            if ( medal(ch) && ( x > MEDAL_BORDER_X || y > MEDAL_BORDER_Y ) )
                in_border = FALSE;

            if ( has_structure[x][y] )
            {
                if ( xray )
                    bld = map_bld[x][y][Z_UNDERGROUND];
                else
                    bld = map_bld[x][y][z];
            }

            if ( ( ch->x == x && ch->y == y && !xray && !has_structure[x][y] && size != 997 && !base && size != 998 ) )
                char_to_building(ch,NULL);

            if ( ch->x == x && ch->y == y )
            {
                sprintf( catbuf, "@@y%s", makesmall(" ** ",small) );
                if ( x < BORDER_SIZE || y < BORDER_SIZE )
                {
                    strcat(catbuf,"@@d");
                    sprintf( color, "@@d" );
                }
                else
                    sprintf( color, "@@y" );
            }
            else if ( in_border && has_structure[x][y] && bld && (bld->visible || ch == bld->owner || ch->trust >= 85 ) )
            {
                CHAR_DATA *vch;
                char ocolor[MSL];

                if ( bld->active && !is_neutral(bld->type) )
                {
                    if ( bld->owner )
                    {
                        vch = bld->owner;
                    }
                    else
                    {
                        if ( str_cmp(bld->owned,ch->name) && !is_evil(bld) )
                            vch = get_ch(bld->owned);
                        else
                            vch = ch;
                    }
                    if ( vch == NULL )
                        activate_building(bld,FALSE);
                }
                else
                    vch = NULL;

                if (ch->in_vehicle != NULL && blind_spot(ch,x,y) )
                {
                    sprintf(catbuf, makesmall("    ",small));
                }
                else
                {
                    char symbol[MSL];
                    bool mxp = TRUE;
                    def = (!ch->security && bld->owner == ch && defense_building(bld));
                    charr = map_ch[x][y][z] ? TRUE : FALSE;
                    sprintf( ocolor, "%s", charr ? "@@J" : (bld->type ==BUILDING_ZAP)?"@@b":def?"@@W":( is_neutral(bld->type) ) ? "@@o" : ( vch == NULL ) ? "@@d" : ( bld->value[9] > 0 ) ? "@@b" : ( IS_LINKDEAD(vch) ) ? "@@o" : ( IS_NEWBIE(vch) ) ? "@@c" : (bld->value[3] != 0) ? "@@m" : ( bld->visible == FALSE ) ? "@@W" : ( bld->hp < bld->maxhp && bld->hp > bld->maxhp / 2 ) ? "@@b" : ( bld->hp < bld->maxhp / 2 ) ? "@@Q" : "@@G" );
                    if ( ch == bld->owner || bld->protection > 0 || ( bld->type == BUILDING_DUMMY && ((IS_BETWEEN(bld->x,ch->x-1,ch->x+1) && IS_BETWEEN(bld->y,ch->y-1,ch->y+1)) || bld->value[5] > 0 ) ) )
                        sprintf( ocolor+strlen(ocolor), "@@x");
                    //			else
                    //				sprintf( ocolor+strlen(ocolor), "%s", ocolor);

                    if ( charr && (map_ch[x][y][z]->pcdata->alliance != ch->pcdata->alliance || map_ch[x][y][z]->pcdata->alliance == -1) )
                        enemy = TRUE;

                    catbuf[0] = '\0';
                    if ( str_cmp(color,ocolor) )
                    {
                        sprintf( color, "%s", ocolor );
                        sprintf( catbuf, "%s", color );
                    }
                    if ( vch && vch->kill_group > ch->kill_group && sysdata.kill_groups )
                    {
                        sprintf( symbol, "%s", makesmall( "????",small) );
                        strcat( catbuf, ocolor);
                        sprintf( ocolor, "%s", symbol );
                        strcat( catbuf, ocolor );
                    }
                    else
                    {
                        if ( bld->z == Z_UNDERGROUND )
                            mxp = FALSE;
                        if ( bld->type != BUILDING_DUMMY )
                            sprintf( symbol, "%s", makesmall(build_table[bld->type].symbol,small));
                        else
                            sprintf( symbol, "%s", makesmall(build_table[bld->value[0]].symbol,small));

                        if ( mxp )
                        {
                            if ( ch->desc->mxp )
                                strcat( catbuf, "\e[1z" );
                            if ( bld->type == BUILDING_DUMMY )
                            {
                                if ( bld->value[0] < 1 || bld->value[0] >= MAX_BUILDING )
                                    bld->value[0] = BUILDING_TURRET;
                                sprintf( ocolor, MXPTAG(ch->desc,"Bl x=%d y=%d owner=%s name='%s'"), bld->x, bld->y, bld->owned, build_table[bld->value[0]].name );
                            }
                            else
                                sprintf( ocolor, MXPTAG(ch->desc,"Bl x=%d y=%d owner=%s name='%s'"), bld->x, bld->y, bld->owned, bld->name );
                        }
                        strcat( catbuf, ocolor);
                        sprintf( ocolor, "%s", symbol );
                        strcat( catbuf, ocolor );
                        if ( mxp )
                            strcat( catbuf, MXPTAG(ch->desc,"/Bl"));
                    }
                }
            }
            else if ( x > 200 && y > 200 && x < 300 && y < 300 && map_bld[x][y][1] && map_bld[x][y][1]->type == BUILDING_HQ && paintball(ch) )
            {
                catbuf[0] = '\0';
                if ( str_cmp(color,"@@r") )
                {
                    sprintf( color, "@@r" );
                    strcat( catbuf, "@@r" );
                }
                strcat( catbuf, makesmall("{{}}",small) );
            }
            else if ( in_border && ( map_ch[x][y][z] != NULL || IS_SET(ch->effect,EFFECT_VISION) ) && size != 998 && size != 997 )
            {
                CHAR_DATA *wch;
                int ppl = 0;
                bool vehicle = FALSE;
                bool allied = FALSE;
                bool newbie = FALSE;
                bool imm = FALSE;
                char ppl_c[MSL];

                if (ch->in_vehicle != NULL && blind_spot(ch,x,y) )
                {
                    sprintf(catbuf, makesmall("    ",small));
                }
                else
                {
                    for ( wch = map_ch[x][y][z];wch;wch = wch->next_in_room )
                    {
                        if ( !can_see(ch,wch) )
                            continue;
                        ppl++;
                        if ( wch->in_vehicle )
                            vehicle = TRUE;
                        if ( ch->pcdata->alliance != -1 && ch->pcdata->alliance == wch->pcdata->alliance )
                            allied = TRUE;
                        else if ( IS_NEWBIE(wch) )
                            newbie = TRUE;
                        else if ( IS_IMMORTAL(wch) )
                            imm = TRUE;
                        else if ( enemy == FALSE )
                            enemy = TRUE;
                        if ( ppl == 1 && wch->class == CLASS_PROJECTOR && !wch->next_in_room )
                            ppl += 2;
                    }
                    if ( ppl > 9 )
                        ppl = 9;

                    if ( IS_SET(ch->effect,EFFECT_VISION) && ppl == 0 && number_percent() < 5 )
                        ppl++;

                    if ( INVALID_COORDS(x,y) )
                        ppl = 0;

                    sprintf( ppl_c, "%d", ppl );
                    if ( ppl <= 0 )
                    {
                        sprintf( color, "%s", ( !in_border ) ? "@@d" : ( !str_cmp(wildmap_table[map_table.type[x][y][ch->z]].color, color) ) ? "" : wildmap_table[map_table.type[x][y][ch->z]].color );
                        sprintf( catbuf, "%s%s", color,  ( ch->in_vehicle != NULL && blind_spot(ch,x,y)) ? makesmall("    ",small) : (!in_border) ? makesmall("++++",small) : makesmall(wildmap_table[map_table.type[x][y][ch->z]].mark,small) );
                        while ( x + 1 <= ch->x + ch->map && in_border && map_table.type[x][y][ch->z] != SECT_NULL )
                        {
                            if ( !map_ch[x+1][y][z] && !map_vhc[x+1][y][z] && !map_bld[x][y][z] && map_table.type[x+1][y][ch->z] == map_table.type[x][y][ch->z] )
                            {
                                sprintf( catbuf+strlen(catbuf), "%s",  ( ch->in_vehicle != NULL && blind_spot(ch,x,y)) ? makesmall("    ",small) : (!in_border) ? makesmall("++++",small) : makesmall(wildmap_table[map_table.type[x][y][ch->z]].mark,small) );
                                x = x + 1;
                            }
                            else
                            {
                                //						sprintf( color, "%s", ( !in_border ) ? "@@d" : ( !str_cmp(wildmap_table[map_table.type[x][y][ch->z]].color, color) ) ? "" : wildmap_table[map_table.type[x][y][ch->z]].color );
                                break;
                            }
                        }
                        sprintf( color, "%s", ( !in_border ) ? "@@d" : ( !str_cmp(wildmap_table[map_table.type[x][y][ch->z]].color, color) ) ? "" : wildmap_table[map_table.type[x][y][ch->z]].color );
                    }
                    else
                    {
                        if ( vehicle )
                        {
                            if ( small == 1 )
                                sprintf( catbuf, "%s%s@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e", ppl_c );
                            if ( small == 2 )
                                sprintf( catbuf, "%s[%s@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e",(ppl==1) ? "]" : ppl_c );
                            if ( small == 4 )
                                sprintf( catbuf, "%s<@@y[%s@@e>@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e",(ppl==1) ? "]" : ppl_c );
                        }
                        else
                        {
                            if ( small == 1 )
                                sprintf( catbuf, "%s*@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e" );
                            if ( small == 2 )
                                sprintf( catbuf, "%s*%s@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e",(ppl==1) ? "*" : ppl_c );
                            if ( small == 4 )
                                sprintf( catbuf, "%s<@@y*%s%s>@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e",(ppl==1) ? "*" : ppl_c,imm?"@@y":allied?"@@r":"@@e" );

                        }
                        sprintf( color, "@@N" );
                    }
                }
            }
            else if ( in_border && ( map_vhc[x][y][z] != NULL && size != 998 && size != 997 ) )
            {
                char mxpbuf[MSL];
                catbuf[0] = '\0';

                if ( ch->desc->mxp && z != Z_UNDERGROUND )
                {
                    strcat( catbuf, "\e[1z" );
                    sprintf( mxpbuf, MXPTAG(ch->desc,"Bl x=%d y=%d owner='Empty' name='%s'"), x,y,map_vhc[x][y][z]->desc );
                    strcat( catbuf, mxpbuf );
                }
                if ( str_cmp(color,"@@d") )
                {
                    strcat( catbuf, "@@d" );
                    sprintf( color, "@@d" );
                }
                if ( small == 1 )
                    sprintf( catbuf+strlen(catbuf), "[" );
                if ( small == 2 )
                    sprintf( catbuf+strlen(catbuf), "[]" );
                if ( small == 4 )
                    sprintf( catbuf+strlen(catbuf), "[[]]" );
                if ( ch->desc->mxp && z != Z_UNDERGROUND )
                    strcat( catbuf, MXPTAG(ch->desc,"/Bl"));
                strcat(catbuf,color);
            }
            else
            {
                char ocolor[MSL];
                sprintf( ocolor, "%s", ( !in_border ) ? "@@d" : wildmap_table[map_table.type[x][y][ch->z]].color );
                catbuf[0] = '\0';
                if ( str_cmp(color,ocolor) )
                {
                    sprintf( color, "%s", ocolor );
                    sprintf( catbuf, "%s", color );
                }
                sprintf( catbuf+strlen(catbuf), "%s", ( ch->in_vehicle != NULL && ch->in_vehicle->type != VEHICLE_MECH && blind_spot(ch,x,y)) ? makesmall("    ",small) : (!in_border) ? makesmall("++++" ,small): makesmall(wildmap_table[map_table.type[x][y][ch->z]].mark,small) );
            }
            if ( IS_SET(ch->pcdata->pflags,PLR_ASS) )
            {
                sprintf( catbuf, makesmall("    ",small));
            }

            if ( !in_border )
                sprintf( color, "@@d" );
            safe_strcat( MSL, outbuf, catbuf  );

            i++;
            if ( i >= 10 )
            {
                i = 0;
                send_to_char( outbuf, ch );
                outbuf[0] = '\0';
            }
        }
        safe_strcat( MSL, outbuf, " @@c|\n\r" );
    }
    /* this is the contents of the map */
    send_to_char( outbuf, ch );
    /* this is the bottom line of the map */
    sprintf( borderbuf, "%s", "@@a*@@c-" );
    sprintf( catbuf, "%s", makesmall("----",small) );
    for ( looper = 0; looper <= ch->map*2; looper++ )
        safe_strcat( MSL, borderbuf, catbuf );

    safe_strcat( MSL, borderbuf, "-@@a*@@N" );
    send_to_char( borderbuf, ch );
    send_to_char( "\n\r", ch );
    if ( warcannon )
    {
        //	sprintf( borderbuf, "\nSCAN RESULTS:\n\r%s\n\r", ( scan[0] == '\0' ) ? "Nothing Found.\n\r" : scan );
        send_to_char( borderbuf, ch);
    }
    if ( enemy && ch->fighttimer < 480 && !IS_IMMORTAL(ch) && ch->z != Z_PAINTBALL )
        ch->fighttimer = 480;
    return;
}

void ShowSMap( CHAR_DATA *ch, bool small )
{
    int x,y,looper, maxx,i=0,xx,yy,xmaxx,ymaxx;
    char scan[MSL];
    char color[MSL];
    char outbuf[MSL];
    char catbuf[MSL];
    char borderbuf[MSL];
    OBJ_DATA *obj;
    VEHICLE_DATA *vhc = ch->in_vehicle;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
    int col;
    outbuf[0] = '\0';
    color[0] = '\0';
    scan[0] = '\0';
    borderbuf[0] = '\0';

    if ( !vhc )
        return;

    maxx = get_ship_range(vhc);
    ShowSpace(ch);
    sprintf( outbuf, "\n\r" );
    sprintf( borderbuf, "@@d+@@g-" );

    if ( small )
        sprintf( catbuf, "%s", "--" );
    else
        sprintf( catbuf, "%s", "----" );

    for ( looper = 0; looper <= maxx*2; looper++ )
        safe_strcat( MSL, borderbuf, catbuf );

    safe_strcat( MSL, borderbuf, "-@@d+@@N" );
    send_to_char( "\n\r", ch );
    send_to_char( borderbuf, ch );

    xmaxx = maxx;
    ymaxx = maxx;

    for (yy = ch->y + ymaxx; yy >= ch->y - ymaxx; --yy)
        //  for (xx = ch->x - xmaxx; xx <= ch->x + xmaxx; ++xx)
    {                                                       /* every row */
        if ( yy < 0 )
            y = SPACE_SIZE + 1 + yy;
        else if ( yy >= SPACE_SIZE )
            y = yy - SPACE_SIZE;
        else
            y = yy;

        safe_strcat( MSL, outbuf, "@@g| " );
        sprintf( color, "@@g" );
        for (xx = ch->x - xmaxx; xx <= ch->x + xmaxx; ++xx)
            //    for (yy = ch->y - ymaxx; yy <= ch->y + ymaxx; ++yy)
        {                                                   /* every column */

            if ( xx < 0 )
                x = SPACE_SIZE + 1 + xx;
            else if ( xx >= SPACE_SIZE )
                x = xx - SPACE_SIZE;
            else
                x = xx;

            if ( ( ( ( xx - (ch->x - xmaxx)) + (yy - (ch->y - ymaxx)) ) <= 3 )
                ||   ( ( ( (ch->x + xmaxx) - xx) + ((ch->y + ymaxx))-yy ) <= 3 )
                ||   ( ( ( (ch->x + xmaxx) - xx) + (yy - (ch->y - ymaxx)) ) <= 3 )
                ||   ( ( ( xx - (ch->x - xmaxx)) + ((ch->y + ymaxx))-yy ) <= 3 ) )
            {
                if ( small )
                {
                    safe_strcat( MSL, outbuf, "++" );
                }
                else
                {
                    safe_strcat( MSL, outbuf, "++++" );
                }
                continue;
            }

            if ( x == ch->x && y == ch->y )
            {
                safe_strcat( MSL, outbuf, "@@y**@@g" );
                if ( x < BORDER_SIZE || y < BORDER_SIZE )
                    strcat(outbuf,"@@d");
                continue;
            }

            col = -1;
            if ( number_percent() < 2 )
                col = number_range(1,4);
            if ( small )
                sprintf( catbuf, "%s ", (col==1)?"@@y*@@g":(col==2)?"@@cx@@g":(col==3)?"@@a+@@g":" " );
            else
                sprintf( catbuf, " %s  ", (col==1)?"@@y*@@g":(col==2)?"@@cx@@g":(col==3)?"@@a+@@g":" " );

            if ( map_obj[x][y] )
            {
                int ppl = 0;
                int type = -1,ttype = -1;

                for ( obj = map_obj[x][y];obj;obj = obj->next_in_room )
                {
                    ppl++;
                    type = obj->pIndexData->vnum-799;
                    if ( ttype != type && ttype != 0 )
                    {
                        if ( ttype == -1 )
                            ttype = type;
                        else
                            ttype = 0;
                    }
                }

                if ( ppl > 0 )
                {
                    if ( IS_SET(vhc->flags,VEHICLE_OBJ_SENSORS) )
                    {
                        sprintf(catbuf,"@@%s",(ttype==1)?"d":(ttype==2)?"p":(ttype==0)?"W":"d");
                    }
                    else
                    {
                        sprintf(catbuf,"@@m");
                    }
                    if ( small )
                        sprintf( catbuf+strlen(catbuf), "()@@g" );
                    else
                        sprintf( catbuf+strlen(catbuf), "(())@@g" );
                    sprintf( color, "@@g" );
                }
            }
            if ( ( map_vhc[x][y] || IS_SET(ch->effect,EFFECT_VISION) ) )
            {
                VEHICLE_DATA *whc;
                CHAR_DATA *wch;
                int ppl = 0;
                bool vehicle = FALSE;
                bool allied = FALSE;
                bool imm = FALSE;
                bool range= FALSE;
                char ppl_c[MSL];

                {
                    for ( whc = map_vhc[x][y][Z_SPACE];whc;whc = whc->next_in_room )
                    {
                        ppl++;
                        if ( ( wch = whc->driving ) == NULL )
                            continue;
                        if ( in_range(ch,wch,get_ship_weapon_range(vhc)) )
                            range = TRUE;
                        sprintf( scan+strlen(scan), "Found: %s - %s.\n\r", wch->name,vhc->desc );
                        if ( wch->in_vehicle )
                            vehicle = TRUE;
                        if ( ch->pcdata->alliance != -1 && ch->pcdata->alliance == wch->pcdata->alliance )
                            allied = TRUE;
                        if ( IS_IMMORTAL(wch) )
                            imm = TRUE;
                        if ( ppl == 1 && wch->class == CLASS_PROJECTOR && !wch->next_in_room )
                            ppl += 2;
                    }
                    if ( ppl > 9 )
                        ppl = 9;

                    if ( IS_SET(ch->effect,EFFECT_VISION) && ppl == 0 && number_percent() < 5 )
                        ppl++;

                    if ( INVALID_COORDS(x,y) )
                        ppl = 0;

                    sprintf( ppl_c, "%d", ppl );
                    if ( ppl > 0 )
                    {
                        if ( small )
                            sprintf( catbuf, "%s%s%s@@g",imm?"@@y":allied?"@@r":"@@e", (range) ? "<" : "[", (ppl==1) ? "]" : ppl_c );
                        else
                            sprintf( catbuf, "%s<@@y%s%s@@e>@@g", imm?"@@y":allied?"@@r":"@@e", (range) ? "<" : "[", (ppl==1) ? "]" : ppl_c );
                        sprintf( color, "@@g" );
                    }
                }
            }
            if ( IS_SET(ch->pcdata->pflags,PLR_ASS) )
            {
                if ( small )
                    sprintf( catbuf, "  ");
                else
                    sprintf( catbuf, "    ");
            }
            safe_strcat( MSL, outbuf, catbuf  );
        }
        safe_strcat( MSL, outbuf, " @@g|\n\r" );
        i++;
        if ( i >= 5 )
        {
            i = 0;
            send_to_char( outbuf, ch );
            outbuf[0] = '\0';
        }
    }
    send_to_char( outbuf, ch );
    sprintf( borderbuf, "%s", "@@d+@@g-" );
    if ( small )
        sprintf( catbuf, "%s", "--" );
    else
        sprintf( catbuf, "%s", "----" );
    for ( looper = 0; looper <= maxx*2; looper++ )
        safe_strcat( MSL, borderbuf, catbuf );
    safe_strcat( MSL, borderbuf, "-@@d+@@N" );
    send_to_char( borderbuf, ch );
    send_to_char( "\n\r", ch );
    catbuf[0] = '\0';
    xx = ch->x;
    yy = ch->y;
    if ( IS_SET(vhc->flags,VEHICLE_PSI_SCANNER) )
        send_to_char(scan,ch);
    for ( obj = map_obj[xx][yy];obj;obj = obj->next_in_room )
    {
        if ( obj->z != Z_SPACE )
            continue;
        sprintf(catbuf+strlen(catbuf),"  %s @@c \n\r",obj->short_descr );
    }
    send_to_char(catbuf,ch);
    return;
}

char *makesmall( char *arg, int size )
{
    static char small[MSL];
    if ( size == 4 )
        return arg;

    small[0] = arg[1];
    if ( size == 2 )
    {
        small[1] = arg[2];
        small[2] = '\0';
    }
    if ( size == 1 )
    {
        small[1] = '\0';
    }
    return (small);
}

void ShowBMap( CHAR_DATA *ch, bool quest )
{
    DESCRIPTOR_DATA *d;
    BUILDING_DATA *bld;
    char b_north[MSL];
    char b_east[MSL];
    char b_west[MSL];
    char b_south[MSL];
    char p_buf[MSL];
    char b_buf[MSL];
    char e_buf[MSL];
    char w_buf[MSL];
    char g_buf[MSL];
    int x,y,last,maxx;
    int terrain[SECT_MAX];
    int offline,allied,enemy,yours,total;

    offline=0;
    allied=0;
    enemy=0;
    yours=0;
    total=0;

    p_buf[0] = '\0';
    b_buf[0] = '\0';
    b_north[0] = '\0';
    b_south[0] = '\0';
    b_east[0] = '\0';
    b_west[0] = '\0';

    maxx = ch->map / 2;

    if ( quest )
        for ( x = 0;x < SECT_MAX;x++ )
            terrain[x] = 0;

    if ( IS_SET(ch->config,CONFIG_LARGEMAP) )
        maxx = ch->map;

    char_to_building(ch,NULL);

    for ( d = first_desc; d != NULL; d = d->next )
    {
        if ( d->character == NULL )
            continue;
        if ( d->connected != CON_PLAYING )
            continue;
        if ( !can_see(ch,d->character) )
            continue;
        if ( paintball(ch) && map_bld[d->character->x][d->character->y][1] != NULL && map_bld[d->character->x][d->character->y][1]->type == BUILDING_HQ )
            continue;
        if ( sneak(d->character) )
            continue;
        if ( ch->z != d->character->z )
            continue;
        if ( d->character != ch
            &&  d->character->x > ch->x - maxx
            &&  d->character->x < ch->x + maxx
            &&  d->character->y > ch->y - maxx
            &&  d->character->y < ch->y + maxx )
            sprintf( p_buf+strlen(p_buf), "Player at: %d/%d (%s%s)\n\r", d->character->x, d->character->y, (ch->y < d->character->y) ? "North" : (ch->y == d->character->y ) ? "" : "South", (ch->x > d->character->x) ? "West" : (ch->x == d->character->x) ? "" : "East" );
    }
    if ( ch->z != Z_AIR )
    {

        for (x = ch->x - maxx; x <= ch->x + maxx; ++x)
        {
            for (y = ch->y - ch->map; y <= ch->y + ch->map; ++y)
            {
                if ( INVALID_COORDS(x,y) )
                    continue;
                bld = map_bld[x][y][ch->z];
                if ( !bld || bld == NULL )
                    continue;

                if ( bld->visible || bld->owner == ch )
                {
                    if ( bld->x == ch->x && bld->y == ch->y )
                    {
                        char_to_building(ch,bld);
                        continue;
                    }
                    if ( bld->x + 1 == ch->x && bld->y == ch->y )
                        sprintf( b_west, "%s", bld->name );
                    if ( bld->x - 1 == ch->x && bld->y == ch->y )
                        sprintf( b_east, "%s", bld->name );
                    if ( bld->x == ch->x && bld->y + 1 == ch->y )
                        sprintf( b_south, "%s", bld->name );
                    if ( bld->x == ch->x && bld->y - 1 == ch->y )
                        sprintf( b_north, "%s", bld->name );
                    //						sprintf( b_buf+strlen(b_buf), "Building: %s, owned by %s, at %d/%d (%s%s)\n\r", bld->name, bld->owned, bld->x, bld->y, (ch->y < bld->y) ? "North" : (ch->y == bld->y) ? "" : "South", (ch->x > bld->x) ? "West" : (ch->x == bld->x) ? "" : "East" );

                    if ( !bld->owner )
                        offline++;
                    else if ( bld->owner == ch )
                        yours++;
                    else if ( bld->owner->pcdata->alliance != -1 && bld->owner->pcdata->alliance == ch->pcdata->alliance )
                        allied++;
                    else
                        enemy++;
                    total++;
                }
            }
        }
    }
    if ( total > 0 )
    {
        sprintf( b_buf, "Buildings:\n\r" );
        if ( yours > 0 )
            sprintf( b_buf+strlen(b_buf), "%d Yours\n\r", yours );
        if ( allied > 0 )
            sprintf( b_buf+strlen(b_buf), "%d Allied\n\r", allied );
        if ( enemy > 0 )
            sprintf( b_buf+strlen(b_buf), "%d Enemy\n\r", enemy );
        if ( offline > 0 )
            sprintf( b_buf+strlen(b_buf), "%d Offline\n\r", offline );
        sprintf( b_buf+strlen(b_buf), "%d Total\n\r", total );

    }
    sprintf( e_buf, "North: %s\n\rEast: %s\n\rSouth: %s\n\rWest: %s\n\r",
        (b_north[0] != '\0') ? b_north : wildmap_table[map_table.type[ch->x][ch->y+1][ch->z]].name,
        (b_east[0] != '\0')  ? b_east  : wildmap_table[map_table.type[ch->x+1][ch->y][ch->z]].name,
        (b_south[0] != '\0') ? b_south : wildmap_table[map_table.type[ch->x][ch->y-1][ch->z]].name,
        (b_west[0] != '\0')  ? b_west  : wildmap_table[map_table.type[ch->x-1][ch->y][ch->z]].name );

    x = ch->x;
    y = ch->y;
    last = map_table.type[x][y][ch->z];
    for ( y = ch->y;y < MAX_MAPS -2;y++ )
    {
        if ( map_table.type[x][y][ch->z] != last )
        {
            sprintf( w_buf, "Far north (%d): %s\n\r", y-ch->y, wildmap_table[map_table.type[x][y][ch->z]].name );
            break;
        }
    }
    y = ch->y;
    for ( x = ch->x;x < MAX_MAPS-2;x++ )
    {
        if ( map_table.type[x][y][ch->z] != last )
        {
            sprintf( w_buf+strlen(w_buf), "Far East (%d): %s\n\r", x - ch->x, wildmap_table[map_table.type[x][y][ch->z]].name );
            break;
        }
    }
    x = ch->x;
    for ( y = ch->y;y > 2;y-- )
    {
        if ( map_table.type[x][y][ch->z] != last )
        {
            sprintf( w_buf+strlen(w_buf), "Far South (%d): %s\n\r", ch->y-y, wildmap_table[map_table.type[x][y][ch->z]].name );
            break;
        }
    }
    y = ch->y;
    for ( x = ch->x;x > 2;x-- )
    {
        if ( map_table.type[x][y][ch->z] != last )
        {
            sprintf( w_buf+strlen(w_buf), "Far West (%d): %s\n\r", ch->x-x, wildmap_table[map_table.type[x][y][ch->z]].name );
            break;
        }
    }

    if ( ch->in_building )
    {
        show_building(ch,IS_SET( ch->config, CONFIG_SMALLMAP)?2:IS_SET(ch->config,CONFIG_TINYMAP)?1:4,ch->map);
        return;
    }

    sprintf( g_buf, "Your location: %d/%d (%s)\n\r\n\r", ch->x, ch->y, wildmap_table[map_table.type[ch->x][ch->y][ch->z]].name );

    if ( quest )
    {
        for ( x = ch->x - ch->map/2;x < ch->x + ch->map/2;x++ )
            for ( y = ch->y - ch->map/2;y < ch->y + ch->map/2;y++ )
                terrain[map_table.type[x][y][1]]++;

        sprintf( b_buf, "\n\rBasic map description:\n\r\n\r" );
        for ( x = 0;x < SECT_MAX;x++ )
            if ( terrain[x] > 0 )
                sprintf( b_buf+strlen(b_buf), "%d %s sectors.\n\r", terrain[x], wildmap_table[x].name );
    }
    if ( ch->z == Z_AIR )
    {
        sprintf( g_buf, "%d/%d\n\r", ch->x,ch->y);
        send_to_char( g_buf, ch );
    }
    else if ( !quest )
    {
        send_to_char( g_buf, ch );
        send_to_char( e_buf, ch );
        send_to_char( w_buf, ch );
        send_to_char( b_buf, ch );
    }
    if ( !quest )
        send_to_char( p_buf, ch );
    return;
}

void show_building( CHAR_DATA *ch, sh_int small, int size )
{
    BUILDING_DATA *bld = ch->in_building;
    char borderbuf[MSL];
    char outbuf[MSL];
    int i,j;
    bool warcannon = FALSE;
    bool msg = FALSE;

    if ( size == 999 )
        warcannon = TRUE;

    if ( IS_SET(ch->pcdata->pflags,PFLAG_HELPING) || bld->z == Z_UNDERGROUND )
        sprintf( borderbuf, "\n\r        @@l(@@W??@@g,@@W??@@l) @@R[@@e%s@@R]@@N", planet_table[ch->z].name );
    else
        sprintf( borderbuf, "\n\r        @@l(@@W%d@@g,@@W%d@@l) @@R[@@e%s@@R]@@N", ch->x, ch->y, planet_table[ch->z].name );
    sprintf( borderbuf+strlen(borderbuf), "\n\r%s%s%s          Level %d\n\r\n\r@@r[@@GExits:@@d ", wildmap_table[map_table.type[ch->x][ch->y][ch->z]].color, bld->name, (bld->maxhp <= build_table[bld->type].hp * 1.5 && upgradable(bld) )?" @@b(@@yU@@b)@@N" : "", bld->level);
    if ( ch->desc->mxp )
        strcat( borderbuf, "\e[1z" );
    for ( i=0;i<4;i++ )
    {
        if ( bld->exit[i] )
        {
            char tempbuf[MSL];
            strcat (borderbuf, MXPTAG(ch->desc,"Ex"));
            sprintf( tempbuf, "%s", ( i == 0 ) ? "North" : ( i == 1 ) ? "East" : ( i == 2 ) ? "South" : "West" );
            strcat (borderbuf, tempbuf );
            strcat (borderbuf, MXPTAG(ch->desc,"/Ex"));
            strcat (borderbuf, " ");
        }
    }
    sprintf( borderbuf+strlen(borderbuf), " @@r]@@N\n\r" );
    if ( (bld->type == BUILDING_WAR_CANNON || bld->type == BUILDING_SNIPER_TOWER || GUNNER(bld) ) && warcannon == FALSE )
    {
        int mapsize;
        mapsize = ch->map;
        ch->map = 10;
        send_to_char( borderbuf, ch );
        send_to_char( "@@a*@@c--------------------------------------------@@a*", ch );
        ShowWMap( ch, small, 999 );
        ch->map = mapsize;
    }
    else
    {
        if ( !IS_SET(ch->config,CONFIG_BRIEF) )
            sprintf( outbuf, "%s\n\r\n\r", build_table[bld->type].desc );
        else
            outbuf[0] = '\0';
        send_to_char(borderbuf,ch);
        send_to_char(outbuf,ch);
    }
    sprintf(borderbuf, "\n\r ");
    if ( !IS_SET(ch->config,CONFIG_BLIND) )
    {
        j = ((10000 / bld->maxhp) * bld->hp) / 1000;
        for ( i=0;i<j;i++ )
            sprintf( borderbuf+strlen(borderbuf), "%s>", ( i < 3 ) ? "@@e" : ( i < 6 ) ? "@@y" : "@@r" );
    }
    sprintf( borderbuf+strlen(borderbuf), " %d HP\n\r", bld->hp );
    send_to_char( borderbuf, ch );
    sprintf(borderbuf, " ");
    if ( !IS_SET(ch->config,CONFIG_BLIND) )
    {
        if ( bld->shield > 0 )
            j = ((10000 / bld->maxshield) * bld->shield) / 1000;
        else
            j = 0;
        for ( i=0;i<j;i++ )
            sprintf( borderbuf+strlen(borderbuf), "%s>", ( i < 3 ) ? "@@e" : ( i < 6 ) ? "@@y" : "@@r" );
    }
    sprintf( borderbuf+strlen(borderbuf), " %d SHIELD@@N\n\r", bld->shield );
    send_to_char(borderbuf,ch);
    borderbuf[0] = '\0';
    outbuf[0] = '\0';
    for ( i=0;i<8;i++ )
    {
        if ( bld->resources[i] > 0 )
        {
            if ( !msg )
            {
                send_to_char( "Needed for completion:\n\r", ch );
                msg = TRUE;
            }
            sprintf( outbuf+strlen(outbuf), "%d %s\n\r", bld->resources[i], ( i == 0 ) ? "Iron" : ( i == 1 ) ? "Skins" : ( i == 2 ) ? "Copper" : ( i == 3 ) ? "Gold" : ( i == 4 ) ? "Silver" : ( i == 5 ) ? "Rocks" : ( i == 6 ) ? "Sticks" : "Logs" );
            //			send_to_char( outbuf, ch );
        }
    }

    if ( bld->type == BUILDING_SCUD_LAUNCHER || bld->type == BUILDING_NUKE_LAUNCHER || bld->type == BUILDING_ATOM_BOMBER )
        sprintf( borderbuf+strlen(borderbuf), "Missile Ready In: %d minutes.\n\r", bld->value[0] / 6);
    else if ( bld->type == BUILDING_DOOMSDAY_DEVICE )
        sprintf( borderbuf+strlen(borderbuf), "SARS Ready In: %d minutes.\n\r", bld->value[0] / 6 );
    else if ( bld->type == BUILDING_HACKPORT )
        sprintf( borderbuf+strlen(borderbuf), "Backdoor Ready In: %d minutes.\n\r", bld->value[0] / 6 );
    else if ( bld->type == BUILDING_ALIEN_PROBE || bld->type == BUILDING_SPY_SATELLITE || bld->type == BUILDING_PSYCHIC_TORMENTOR || bld->type == BUILDING_TRANSMITTER || bld->type == BUILDING_SHOCKWAVE )
        sprintf( borderbuf+strlen(borderbuf), "Transmission Ready In: %d minutes.\n\r", bld->value[0] / 6 );
    else if ( bld->type == BUILDING_SPY_QUARTERS || bld->type == BUILDING_INTERGALACTIC_PUB )
        sprintf( borderbuf+strlen(borderbuf), "Spy Mission Ready In: %d minutes.\n\r", bld->value[0] / 6 );
    send_to_char( borderbuf, ch );
    send_to_char( outbuf,ch);

    if ( IS_SET(ch->config, CONFIG_EXITS ) )
    {
        sprintf( borderbuf, "\n@@r[@@GExits:@@d @@N" );
        if ( ch->desc->mxp )
            strcat( borderbuf, "\e[1z" );
        for ( i=0;i<4;i++ )
        {
            if ( bld->exit[i] )
            {
                char tempbuf[MSL];
                strcat (borderbuf, MXPTAG(ch->desc,"Ex"));
                sprintf( tempbuf, "%s", ( i == 0 ) ? "North" : ( i == 1 ) ? "East" : ( i == 2 ) ? "South" : "West" );
                strcat (borderbuf, tempbuf );
                strcat (borderbuf, MXPTAG(ch->desc,"/Ex"));
                strcat (borderbuf, " ");
            }
        }
        strcat(borderbuf,"]");
        send_to_char( borderbuf, ch );
    }
    sprintf( outbuf, "\nOwned by %s.\n\r", bld->owned );
    send_to_char( outbuf, ch );
    return;
}

void ShowSpace( CHAR_DATA *ch )
{
    char ss[MSL];
    char aa[MSL];
    char ff[MSL];
    char buf[MSL];
    char pref[MSL];
    VEHICLE_DATA *vhc = ch->in_vehicle;
    int i,j=get_ship_range(vhc);
    pref[0] = '\0';
    for ( i=0;i<((j*4)-34);i++ )
        sprintf( pref+strlen(pref)," ");

    sprintf(buf, "\n\r" );
    sprintf(buf+strlen(buf),"%s@@c     ------------------------ \n\r", pref );
    sprintf(buf+strlen(buf),"%s  --/  SCN: %-3d    WPN: %-3d  \\-- \n\r", pref, j, vhc->range );
    sprintf(buf+strlen(buf),"%s / ______________      @@RSS@@dAA@@aFF@@c_  \\ \n\r", pref  );

    if ( vhc->hit > ( vhc->max_hit / 100 ) * 80 )
        sprintf( ss, "@@r||" );
    else
        sprintf( ss, "  " );
    if ( vhc->ammo > ( vhc->max_ammo / 100 ) * 80 )
        sprintf( aa, "@@r||" );
    else
        sprintf( aa, "  " );
    if ( vhc->fuel > ( vhc->max_fuel / 100 ) * 80 )
        sprintf( ff, "@@r||" );
    else
        sprintf( ff, "  " );

    sprintf(buf+strlen(buf),"%s| |@@aSYS: @@y%9s@@c\\    |%s%s%s @@c\\  | \n\r", pref, "ACTIVE", ss, aa, ff );

    if ( vhc->hit > ( vhc->max_hit / 100 ) * 60 )
        sprintf( ss, "@@y||" );
    else
        sprintf( ss, "  " );
    if ( vhc->ammo > ( vhc->max_ammo / 100 ) * 60 )
        sprintf( aa, "@@y||" );
    else
        sprintf( aa, "  " );
    if ( vhc->fuel > ( vhc->max_fuel / 100 ) * 60 )
        sprintf( ff, "@@y||" );
    else
        sprintf( ff, "  " );

    sprintf(buf+strlen(buf),"%s|  ______________     |%s%s%s  @@c| | \n\r",pref, ss, aa, ff );

    if ( vhc->hit > ( vhc->max_hit / 100 ) * 40 )
        sprintf( ss, "@@e||" );
    else
        sprintf( ss, "  " );
    if ( vhc->ammo > ( vhc->max_ammo / 100 ) * 40 )
        sprintf( aa, "@@e||" );
    else
        sprintf( aa, "  " );
    if ( vhc->fuel > ( vhc->max_fuel / 100 ) * 40 )
        sprintf( ff, "@@e||" );
    else
        sprintf( ff, "  " );
    {
        char msg[MSL];
        msg[0] = '\0';
        if ( IS_SET(vhc->flags,VEHICLE_CORROSIVE_A) )
            sprintf( msg, "@@rACID@@c" );
        sprintf(buf+strlen(buf),"%s| |@@aMSG:@@c %9s\\    |%s%s%s  @@c| | \n\r", pref,msg, ss, aa, ff );
    }

    if ( vhc->hit > ( vhc->max_hit / 100 ) * 20 )
        sprintf( ss, "@@R||" );
    else
        sprintf( ss, "  " );
    if ( vhc->ammo > ( vhc->max_ammo / 100 ) * 20 )
        sprintf( aa, "@@R||" );
    else
        sprintf( aa, "  " );
    if ( vhc->fuel > ( vhc->max_fuel / 100 ) * 20 )
        sprintf( ff, "@@R||" );
    else
        sprintf( ff, "  " );

    sprintf(buf+strlen(buf),"%s|                     |%s%s%s    @@c| \n\r", pref, ss, aa, ff );

    sprintf(buf+strlen(buf),"%s @@c\\_                            _/ \n\r", pref );
    sprintf(buf+strlen(buf),"%s   \\_________________________/ \n\r", pref );
    if ( IS_IMMORTAL(ch) )
        sprintf( buf+strlen(buf), "%s           %d/%d\n\r", pref, ch->x,ch->y);

    send_to_char(buf,ch);
}

void draw_space( CHAR_DATA *ch )
{
    char buf[MSL];
    int i;
    int j;
    sprintf( buf, "\n\r" );
    for( i=1;i<6;i++ )
    {
        j = number_range(1,6);
        if ( j == 1 )
            sprintf(buf+strlen(buf), " @@y.  @@b+ @@e`     @@y+     @@e`    @@y.  @@R*\n\r" );
        else if ( j == 2 )
            sprintf(buf+strlen(buf), " @@e` @@y.      @@e.     @@B'     @@y+    @@e.   @@e'\n\r" );
        else if ( j == 3 )
            sprintf(buf+strlen(buf), " @@B.  @@b+ @@e`     @@y+     @@R+    @@y.\n\r" );
        else if ( j == 4 )
            sprintf(buf+strlen(buf), " @@b* @@y.      @@R*     @@b'     @@B+    @@e.       @@y+\n\r" );
        else if ( j == 5 )
            sprintf(buf+strlen(buf), " @@e.  @@b+ @@y`     @@B+     @@e`    @@y.\n\r" );
        else if ( j == 6 )
            sprintf(buf+strlen(buf), " @@e` @@R*      @@e.     @@E'     @@e+    @@y.    @@e*\n\r" );
    }
    send_to_char( buf, ch );
    return;
}

void do_buildings( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    int x,y,maxx;
    char buf[MSL];
    bool all=FALSE;

    if ( !IS_SET(ch->config,CONFIG_BLIND) )
    {
        send_to_char("Huh?\n\r", ch );
        return;
    }
    maxx = ch->map / 2;

    if ( argument[0] == '\0' )
        all = TRUE;
    buf[0] = '\0';

    for (x = ch->x - maxx; x <= ch->x + maxx; ++x)
    {
        for (y = ch->y - ch->map; y <= ch->y + ch->map; ++y)
        {
            if ( INVALID_COORDS(x,y) )
                continue;
            bld = map_bld[x][y][ch->z];
            if ( !bld || bld == NULL )
                continue;

            if ( !all && (str_prefix(argument,bld->owned) && str_cmp(argument,bld->name) ) )
                continue;

            if ( bld->visible || bld->owner == ch )
            {
                //				if ( all )
                sprintf(buf+strlen(buf), "%s: ", bld->owned );
                if ( bld->type == BUILDING_ZAP )
                    continue;
                sprintf( buf+strlen(buf), "%s, at %d/%d (%s%s)\n\r", (bld->type == BUILDING_DUMMY && bld->value[0] > 0 && bld->value[0] < MAX_BUILDING)? build_table[bld->value[0]].name:bld->name, bld->x, bld->y, (ch->y < bld->y) ? "North" : (ch->y == bld->y) ? "" : "South", (ch->x > bld->x) ? "West" : (ch->x == bld->x) ? "" : "East" );
            }
        }
    }
    send_to_char(buf,ch);
    return;
}

void do_scanmap( CHAR_DATA *ch, char *argument )
{
    int dir,sect,lsect,count=0;
    int x,y;
    char buf[MSL];

    if ( !IS_SET(ch->config,CONFIG_BLIND) )
    {
        send_to_char( "Huh?\n\r", ch );
        return;
    }
    if ( ( dir = parse_direction(ch,argument) ) == -1 || argument[0] == '\0' )
    {
        send_to_char( "Valid directions are North, East, South and West\n\r", ch );
        return;
    }

    x = ch->x;
    y = ch->y;
    lsect = -1;
    sect = -1;
    buf[0] = '\0';
    while ( TRUE )
    {
        lsect = sect;
        if ( dir == DIR_NORTH )
        {
            y++;
            if ( y > ch->y + ch->map )
                break;
        }
        else if ( dir == DIR_SOUTH )
        {
            y--;
            if ( y < ch->y - ch->map )
                break;
        }
        else if ( dir == DIR_EAST )
        {
            x++;
            if ( x > ch->x + ch->map )
                break;
        }
        else if ( dir == DIR_WEST )
        {
            x--;
            if ( x < ch->x - ch->map )
                break;
        }
        if ( INVALID_COORDS(x,y) )
            break;

        sect = map_table.type[x][y][ch->z];
        if ( lsect != sect && lsect != -1 )
        {
            sprintf( buf+strlen(buf), "%d %s terrain, ", count, wildmap_table[lsect].name );
            count = 0;
        }
        count++;
    }
    sprintf( buf+strlen(buf), "%d %s terrain.\n\r", count, wildmap_table[lsect].name );
    send_to_char(buf,ch);
    return;
}
