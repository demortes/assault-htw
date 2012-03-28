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
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "ack.h"
#include "tables.h"

// Local Functions
void generate_basic_page( FILE * fp );
void generate_basic_page_end( FILE * fp );
void add_affiliates ( FILE *fp );
void save_web_data();
char *format_web( char *str );

struct web_data web_data;

void generate_basic_page( FILE * fp )
{
    fprintf( fp, "<HTML><HEAD><LINK HREF=tgh.css TYPE=text/css REL=stylesheet></HEAD><BODY bgcolor=\"#ffffff\">" );
    return;
}

void generate_basic_page_end( FILE * fp )
{
    fprintf(fp,"</body></html>");
    return;
}

void add_affiliates( FILE *fp )
{
    return;
}

void generate_webpage( void )
{
    FILE * fp;
    char file_name[MAX_STRING_LENGTH];
    char buf[MSL];
    fclose( fpReserve );
    sprintf( file_name, "%s", STAT_WEB_FILE );

    if ( ( fp = fopen( file_name, "w" ) ) != NULL )
    {
        sprintf( buf, "<?php\n" );
        sprintf( buf+strlen(buf), "$last_killed_in_pit = \"%s\";\n" , web_data.last_killed_in_pit );
        sprintf( buf+strlen(buf), "$last_kills_in_pit = \"%s\";\n"  , web_data.last_kills_in_pit );
        sprintf( buf+strlen(buf), "$num_players = %d;\n"        , web_data.num_players );
        sprintf( buf+strlen(buf), "$tot_players = %d;\n"        , web_data.tot_players );
        sprintf( buf+strlen(buf), "$highest_rank = \"%s\";\n"       , web_data.highest_ranking_player );
        sprintf( buf+strlen(buf), "$highest_rank2 = %d;\n"      , web_data.highest_rank );
        sprintf( buf+strlen(buf), "$newest_player = \"%s\";\n"      , web_data.newest_player );
        sprintf( buf+strlen(buf), "$num_buildings = %ld;\n"     , building_count );
        sprintf( buf+strlen(buf), "?>" );

        fprintf( fp, buf );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

char *format_web( char * str )
{
    int i;
    for ( i=0;str[i] != '\0';i++ )
    {
        if ( i + 8 > strlen(str) )
            break;
        if ( str[i] == '*'
            && str[i+1] == '*'
            && str[i+2] == '*'
            && str[i+3] == '*'
            && str[i+4] == '*'
            && str[i+5] == '*'
            && str[i+6] == '*'
            && str[i+7] == '*' )
        {
            str[i] = '<';
            str[i+1] = 'b';
            str[i+2] = 'r';
            str[i+3] = '>';
            str[i+4] = '<';
            str[i+5] = 'l';
            str[i+6] = 'i';
            str[i+7] = '>';
        }
        i += 8;
    }
    return str;
}

void update_web_data(int type,char *value)
{
    if ( type == WEB_DATA_KILLED_PIT )
    {
        if ( web_data.last_killed_in_pit != NULL )
            free_string(web_data.last_killed_in_pit);
        web_data.last_killed_in_pit = str_dup(value);
    }
    else if ( type == WEB_DATA_KILLS_PIT )
    {
        if ( web_data.last_kills_in_pit != NULL )
            free_string(web_data.last_kills_in_pit);
        web_data.last_kills_in_pit = str_dup(value);
    }
    else if ( type == WEB_DATA_HIGHEST_RANK )
    {
        if ( web_data.highest_ranking_player != NULL )
            free_string(web_data.highest_ranking_player);
        web_data.highest_ranking_player = str_dup(value);
    }
    else if ( type == WEB_DATA_NEWEST_PLAYER )
    {
        if ( web_data.newest_player != NULL )
            free_string(web_data.newest_player);
        web_data.newest_player = str_dup(value);
    }
    else if ( type == WEB_DATA_NUM_PLAYERS )
    {
        web_data.num_players = atoi(value);
    }
    else if ( type == WEB_DATA_TOT_PLAYERS )
    {
        web_data.tot_players = atoi(value);
    }
    else
        return;
    generate_webpage();
    save_web_data();
    return;
}

void save_web_data( )
{
    FILE * fp;

    fclose( fpReserve );

    if ( ( fp = fopen( WEB_DATA_FILE, "w" ) ) != NULL )
    {
        fprintf( fp, "%s~\n%s~\n%s~\n%s~\n%d\n%d\n", web_data.last_killed_in_pit, web_data.last_kills_in_pit, web_data.highest_ranking_player, web_data.newest_player, web_data.highest_rank, web_data.tot_players );
    }
    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;

}
