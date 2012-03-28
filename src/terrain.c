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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "ack.h"
#include "tables.h"

// All the map-generating code goes here

void create_full_map args( ( CHAR_DATA *ch ) );
void create_map(CHAR_DATA *ch,int type)
{
    if ( type == TERRAIN_BALANCED )
        create_full_map(ch);
    return;
}

void create_full_map(CHAR_DATA *ch)
{
    int x,y,i,j,k,m,n,z=ch->z;
    char buf[MSL];

    for ( x = 0;x < MAX_MAPS;x++ )
        for ( y = 0; y < MAX_MAPS;y++ )
    {
        map_table.type[x][y][z] = SECT_FIELD;
        map_table.resource[x][y][z] = -1;
    }
    j = number_range(1,MAX_MAPS/20);                        /* Generate Mountain Ridges */
    sprintf( buf, "Map Properties:\n\r%d Mountain Ridges 1\n\r", j );
    for ( i = 1;i < j;i++ )
    {
        x = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        m = number_range(2,4);
        for ( y = 3;y<k;y++ )
        {
            if ( x - 4 < 0 || x + m + 4 >= MAX_MAPS )
                break;
            for ( n = 1;n<m;n++ )
                map_table.type[x+n][y][z] = SECT_MOUNTAIN;
            map_table.type[x+n][y][z] = SECT_HILLS;
            map_table.type[x][y][z] = SECT_HILLS;
            x = number_range(x-1,x+1);
        }
    }
    j = number_range(1,MAX_MAPS/20);
    sprintf( buf+strlen(buf), "%d Mountain Ridges 2\n\r", j );
    for ( i = 1;i < j;i++ )
    {
        y = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        m = number_range(2,4);
        for ( x = 3;x<k;x++ )
        {
            if ( y - 4 < 0 || y + m + 4 >= MAX_MAPS )
                break;
            for ( n = 1;n<m;n++ )
                map_table.type[x][y+n][z] = SECT_MOUNTAIN;
            map_table.type[x][y+n][z] = SECT_HILLS;
            map_table.type[x][y][z] = SECT_HILLS;
            y = number_range(y-1,y+1);
        }

    }
    j = number_range(1,MAX_MAPS/20);
    sprintf( buf+strlen(buf), "%d Mountain Ridges 3\n\r", j );
    for ( i = 1;i < j;i++ )
    {
        x = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        m = number_range(2,4);
        for ( y = 3;y<k;y++ )
        {
            if ( x - 4 < 0 || x + m + 4 >= MAX_MAPS )
                break;
            for ( n = 1;n<m;n++ )
                map_table.type[x+n][MAX_MAPS - y][z] = SECT_MOUNTAIN;
            map_table.type[x+n][MAX_MAPS - y][z] = SECT_HILLS;
            map_table.type[x][MAX_MAPS - y][z] = SECT_HILLS;
            x = number_range(x-1,x+1);
        }
    }
    j = number_range(1,MAX_MAPS/20);
    sprintf( buf+strlen(buf), "%d Mountain Ridges 4\n\r", j );
    for ( i = 1;i < j;i++ )
    {
        y = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        m = number_range(2,4);
        for ( x = 3;x<k;x++ )
        {
            if ( y - 4 < 0 || y + m + 4 >= MAX_MAPS )
                break;
            for ( n = 1;n<m;n++ )
                map_table.type[MAX_MAPS - x][y+n][z] = SECT_MOUNTAIN;
            map_table.type[MAX_MAPS - x][y + n][z] = SECT_HILLS;
            map_table.type[MAX_MAPS - x][y][z] = SECT_HILLS;
            y = number_range(y-1,y+1);
        }
    }
    j = MAX_MAPS / 1;                                       /* Generate Volcanos */
    sprintf( buf+strlen(buf), "%d Volcanos\n\r", j );
    for ( i = 0;i<j;i++ )
    {
        m = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        n = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(3,6);
        for ( x = m;x<m+k;x++ )
        {
            for ( y = n;y<n+k;y++ )
            {
                if ( x < 1 || y < 1 || x >= MAX_MAPS || y >= MAX_MAPS )
                    continue;
                map_table.type[x][y][z] = SECT_LAVA;
            }
        }
        for ( x = m;x<m+k;x++ )
        {
            if ( x < 1 || x >= MAX_MAPS )
                continue;
            if ( map_table.type[x][n-1][z] != SECT_LAVA )
                map_table.type[x][n][z] = SECT_MOUNTAIN;
            if ( map_table.type[x][n+k+1][z] != SECT_LAVA )
                map_table.type[x][n+k][z] = SECT_MOUNTAIN;
        }
        for ( x = n;x<n+k;x++ )
        {
            if ( x < 1 || x >= MAX_MAPS )
                continue;

            if ( map_table.type[m-1][x][z] != SECT_LAVA )
                map_table.type[m][x][z] = SECT_MOUNTAIN;
            if ( map_table.type[m+k+1][x][z] != SECT_LAVA )
                map_table.type[m+k][x][z] = SECT_MOUNTAIN;
        }
    }
    j = number_range(1,MAX_MAPS/20);                        /* Generate Lakes */
    sprintf( buf+strlen(buf), "%d Lakes 1\n\r", j );
    for ( i = 1;i < j;i++ )
    {
        x = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        m = number_range(1,3);
        for ( y = 3;y<k;y++ )
        {
            if ( x - BORDER_SIZE < 0 || x + m + BORDER_SIZE >= MAX_MAPS )
                break;

            for ( n = 1;n<m;n++ )
            {
                if ( map_table.type[x+n][y][z] == SECT_LAVA )
                    map_table.type[x+n][y][z] = SECT_ASH;
                else
                    map_table.type[x+n][y][z] = SECT_WATER;
            }
            x = number_range(x-1,x+1);
        }
    }
    j = number_range(1,MAX_MAPS/20);
    sprintf( buf+strlen(buf), "%d Lakes 2\n\r", j );
    for ( i = 1;i < j;i++ )
    {
        y = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        m = number_range(1,3);
        for ( x = 3;x<k;x++ )
        {
            if ( y - BORDER_SIZE < 0 || y + m + BORDER_SIZE >= MAX_MAPS )
                break;

            for ( n = 1;n<m;n++ )
            {
                if ( map_table.type[x+n][y][z] == SECT_LAVA )
                    map_table.type[x+n][y][z] = SECT_ASH;
                else
                    map_table.type[x+n][y][z] = SECT_WATER;
            }
            y = number_range(y-1,y+1);
        }

    }
    j = number_range(1,MAX_MAPS/20);
    sprintf( buf+strlen(buf), "%d Lakes 3\n\r", j );
    for ( i = 1;i < j;i++ )
    {
        x = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        m = number_range(1,3);
        for ( y = 3;y<k;y++ )
        {
            if ( x - BORDER_SIZE < 0 || x + m + BORDER_SIZE >= MAX_MAPS )
                break;

            for ( n = 1;n<m;n++ )
            {
                if ( map_table.type[x+n][y][z] == SECT_LAVA )
                    map_table.type[x+n][y][z] = SECT_ASH;
                else
                    map_table.type[x+n][y][z] = SECT_WATER;
            }
            x = number_range(x-1,x+1);
        }
    }
    j = number_range(1,MAX_MAPS/20);
    sprintf( buf+strlen(buf), "%d Lakes 4\n\r", j );
    for ( i = 1;i < j;i++ )
    {
        y = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        m = number_range(1,3);
        for ( x = 3;x<k;x++ )
        {
            if ( y - BORDER_SIZE < 0 || y + m + BORDER_SIZE >= MAX_MAPS )
                break;

            for ( n = 1;n<m;n++ )
            {
                if ( map_table.type[x+n][y][z] == SECT_LAVA )
                    map_table.type[x+n][y][z] = SECT_ASH;
                else
                    map_table.type[x+n][y][z] = SECT_WATER;
            }
            y = number_range(y-1,y+1);
        }
    }
    j = number_range(MAX_MAPS / 40,MAX_MAPS / 20);          /* Generate Snow */
    sprintf( buf+strlen(buf), "%d Snow\n\r", j );
    for ( i = 0;i<j;i++ )
    {
        m = number_range(1,MAX_MAPS/10);
        n = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        for ( x = n-m;x<n;x++ )
        {
            if ( (x == n || x == n-1) && number_percent() < 50 )
                continue;
            //				if ( x == number_range((n-m)-1,(n-m)+10) )
            if ( ( x == n-m || x == (n-m)-1 ) && number_percent() < 50 )
                continue;
            for ( y = k-m;y < k;y++ )
            {
                if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
                    continue;
                if ( y == number_range(k-1,k+1) )
                    continue;
                if ( y == number_range((k-m)-1,(k-m)+1) )
                    continue;
                if ( map_table.type[x][y][z] == SECT_FIELD || map_table.type[x][y][z] == SECT_FOREST )
                    map_table.type[x][y][z] = SECT_SNOW;
                else if ( map_table.type[x][y][z] == SECT_WATER )
                    map_table.type[x][y][z] = SECT_ICE;
            }
        }
    }
    j = number_range(MAX_MAPS / 10,MAX_MAPS / 5);           /* Generate Rock Fields */
    sprintf( buf+strlen(buf), "%d Rock Fields\n\r", j );
    for ( i = 0;i<j;i++ )
    {
        m = number_range(1,MAX_MAPS/20);
        n = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        for ( x = n-m;x<n;x++ )
        {
            if ( x == number_fuzzy(n) )
                continue;
            if ( x == number_fuzzy(n-m) )
                continue;
            for ( y = k-m;y < k;y++ )
            {
                if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
                    continue;
                if ( y == number_fuzzy(k) )
                    continue;
                if ( y == number_fuzzy(k-m) )
                    continue;
                if ( map_table.type[x][y][z] == SECT_FIELD )
                    map_table.type[x][y][z] = SECT_ROCK;
            }
        }
    }
    j = number_range(MAX_MAPS / 20,MAX_MAPS / 14);          /* Generate Forests */
    sprintf( buf+strlen(buf), "%d Forests\n\r", j );
    for ( i = 0;i<j;i++ )
    {
        m = number_range(1,MAX_MAPS/15);
        n = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        for ( x = n-m;x<n;x++ )
        {
            if ( x == number_fuzzy(n) )
                continue;
            if ( x == number_fuzzy(n-m) )
                continue;
            for ( y = k-m;y < k;y++ )
            {
                if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
                    continue;
                if ( y == number_fuzzy(k) )
                    continue;
                if ( y == number_fuzzy(k-m) )
                    continue;
                if ( map_table.type[x][y][z] == SECT_FIELD )
                    map_table.type[x][y][z] = SECT_FOREST;
            }
        }
    }
    j = number_range(MAX_MAPS / 40,MAX_MAPS / 20);          /* Generate Deserts */
    sprintf( buf+strlen(buf), "%d Deserts\n\r", j );
    for ( i = 0;i<j;i++ )
    {
        m = number_range(1,MAX_MAPS/10);
        n = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        k = number_range(BORDER_SIZE,MAX_MAPS-BORDER_SIZE);
        for ( x = n-m;x<n;x++ )
        {
            if ( x == number_fuzzy(n) )
                continue;
            if ( x == number_fuzzy(n-m) )
                continue;
            for ( y = k-m;y < k;y++ )
            {
                if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
                    continue;
                if ( y == number_fuzzy(k) )
                    continue;
                if ( y == number_fuzzy(k-m) )
                    continue;
                if ( map_table.type[x][y][z] == SECT_FIELD )
                    map_table.type[x][y][z] = SECT_SAND;
            }
        }
    }

    /* Generate Borders */
    for ( x=0;x<BORDER_SIZE;x++ )
        for( y=0;y<MAX_MAPS;y++ )
    {
        map_table.type[x][y][z] = SECT_NULL;
        map_table.type[y][x][z] = SECT_NULL;
    }
    for ( x=MAX_MAPS - BORDER_SIZE;x<MAX_MAPS;x++ )
        for( y=0;y<MAX_MAPS;y++ )
    {
        map_table.type[x][y][z] = SECT_NULL;
        map_table.type[y][x][z] = SECT_NULL;
    }
    send_to_char( buf, ch );
    return;
}

void create_special_map()
{
    int x,y,z=Z_PAINTBALL;

    for ( x = BORDER_SIZE;x < MEDAL_BORDER_X;x++ )
        for ( y = BORDER_SIZE; y < MEDAL_BORDER_Y;y++ )
    {
        map_table.type[x][y][z] = SECT_BURNED;
        map_table.resource[x][y][z] = -1;
    }

    for ( x = BORDER_SIZE; x < MEDAL_BORDER_X;x++ )
    {
        map_table.type[x][MEDAL_BORDER_Y+1][z] = SECT_NULL;
        map_table.type[x][BORDER_SIZE-1][z] = SECT_NULL;
    }
    for ( y = BORDER_SIZE; y < MEDAL_BORDER_Y;y++ )
    {
        map_table.type[MEDAL_BORDER_X+1][y][z] = SECT_NULL;
        map_table.type[BORDER_SIZE-1][y][z] = SECT_NULL;
    }
    return;
}

void make_lava_river(int x, int y, int z)
{
    int k,type;

    type = number_range(0,3);
    k = number_range(1,10);
    while ( k > 0 )
    {
        if ( x - BORDER_SIZE < 0 || x + BORDER_SIZE >= MAX_MAPS || y - BORDER_SIZE < 0 || y + BORDER_SIZE >= MAX_MAPS )
            break;
        if ( !(map_ch[x][y][z] || map_bld[x][y][z]) )
            map_table.type[x][y][z] = SECT_MAGMA;
        //		if ( map_bld[x][y][z] )
        //			extract_building(map_bld[x][y][z],TRUE);
        if ( type == DIR_NORTH )
        {
            y++;
            x = number_range(x-1,x+1);
        }
        else if ( type == DIR_SOUTH )
        {
            y--;
            x = number_range(x-1,x+1);
        }
        else if ( type == DIR_EAST )
        {
            x++;
            y = number_range(y-1,y+1);
        }
        else if ( type == DIR_WEST )
        {
            x--;
            y = number_range(y-1,y+1);
        }
        k--;
    }
    return;
}

void init_fields()
{
    int i,j,m,n,k,x,y;

    for ( x=BORDER_SIZE;x<=MAX_MAPS-BORDER_SIZE;x++ )
    {
        for ( y=BORDER_SIZE;y<=MAX_MAPS-BORDER_SIZE;y++ )
        {
            if ( x <= MEDAL_BORDER_X && y <= MEDAL_BORDER_Y )
                continue;
            if ( (y == 199 || y == 301 || x == 199 || x == 301 ) && x >= 199 && x <= 301 && y >= 199 && y <= 301 )
                map_table.type[x][y][Z_PAINTBALL] = SECT_NULL;
            else
                map_table.type[x][y][Z_PAINTBALL] = SECT_FOREST;

            if ( map_table.type[x][y][Z_GROUND] == SECT_LAVA )
                make_lava_river(x,y,Z_UNDERGROUND);
        }
    }
    {
        j = number_range(MAX_MAPS / 8,MAX_MAPS / 4);        /* Generate Fields */
        for ( i = 0;i<j;i++ )
        {
            m = number_range(1,MAX_MAPS/20);
            n = number_range(3,MAX_MAPS-4);
            k = number_range(3,MAX_MAPS-4);
            for ( x = n-m;x<n;x++ )
            {
                if ( x == number_fuzzy(n) )
                    continue;
                if ( x == number_fuzzy(n-m) )
                    continue;
                for ( y = k-m;y < k;y++ )
                {
                    if ( x < 4 || x > MAX_MAPS-5 || y < 4 || y > MAX_MAPS-5 )
                        continue;
                    if ( x <= MEDAL_BORDER_X && y <= MEDAL_BORDER_Y )
                        continue;
                    if ( y == number_fuzzy(k) )
                        continue;
                    if ( y == number_fuzzy(k-m) )
                        continue;
                    if ( map_table.type[x][y][Z_PAINTBALL] != SECT_NULL )
                        map_table.type[x][y][Z_PAINTBALL] = SECT_FIELD;
                }
            }
        }
        j = number_range(MAX_MAPS / 8,MAX_MAPS / 4);        /* Generate Snow */
        for ( i = 0;i<j;i++ )
        {
            m = number_range(1,MAX_MAPS/20);
            n = number_range(3,MAX_MAPS-3);
            k = number_range(3,MAX_MAPS-3);
            for ( x = n-m;x<n;x++ )
            {
                if ( x == number_fuzzy(n) )
                    continue;
                if ( x == number_fuzzy(n-m) )
                    continue;
                for ( y = k-m;y < k;y++ )
                {
                    if ( x < 4 || x > MAX_MAPS-5 || y < 4 || y > MAX_MAPS-5 )
                        continue;
                    if ( x <= MEDAL_BORDER_X && y <= MEDAL_BORDER_Y )
                        continue;
                    if ( y == number_fuzzy(k) )
                        continue;
                    if ( y == number_fuzzy(k-m) )
                        continue;
                    if ( map_table.type[x][y][Z_PAINTBALL] != SECT_NULL )
                        map_table.type[x][y][Z_PAINTBALL] = SECT_SNOW;
                }
            }
        }
    }
    for ( x=PIT_BORDER_X;x<=MAX_MAPS;x++ )
    {
        for ( y=PIT_BORDER_Y;y<=MAX_MAPS;y++ )
        {
            if ( x == PIT_BORDER_X || y == PIT_BORDER_Y )
                map_table.type[x][y][Z_PAINTBALL] = SECT_NULL;
            else
                map_table.type[x][y][Z_PAINTBALL] = SECT_BURNED;
        }
    }
    return;
}
