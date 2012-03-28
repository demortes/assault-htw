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
#include <ctype.h>
/* For forks etc. */
#include <unistd.h>
#include <fcntl.h>

#include "ack.h"
#include "tables.h"

void save_brands( )
{

    FILE * fp;
    char brand_file_name[MAX_STRING_LENGTH];
    DL_LIST   * brand;
    BRAND_DATA    * this_brand;

    fclose( fpReserve );
    sprintf( brand_file_name, "%s", BRANDS_FILE );

    if ( ( fp = fopen( brand_file_name, "w" ) ) == NULL )
    {
        bug( "Save brands list: fopen", 0 );
        perror( "failed open of brands.lst in save_brands" );
    }
    else
    {
        for ( brand = first_brand; brand != NULL; brand = brand->next )
        {
            this_brand = brand->this_one;
            fprintf( fp, "#BRAND~\n" );
            fprintf( fp, "%s~\n", this_brand->branded );
            fprintf( fp, "%s~\n", this_brand->branded_by );
            fprintf( fp, "%s~\n", this_brand->dt_stamp );
            fprintf( fp, "%s~\n", this_brand->message );
            fprintf( fp, "%s~\n", this_brand->priority );

        }
        fprintf( fp, "#END~\n\n" );
    }

    fflush( fp );
    fclose( fp );

    fpReserve = fopen( NULL_FILE, "r" );
    return;

}

void load_brands( void )
{

    FILE *brandsfp;
    char brands_file_name[MAX_STRING_LENGTH];
    char buf[MAX_STRING_LENGTH];
    BRAND_DATA    * this_brand;
    DL_LIST * brand_member;

    sprintf( brands_file_name, "%s", BRANDS_FILE );

    sprintf( buf, "Loading %s\n\r", brands_file_name);
    monitor_chan( NULL, buf, MONITOR_SYSTEM );

    if ( ( brandsfp = fopen( brands_file_name, "r" ) ) == NULL )
    {
        bug( "Load brands Table: fopen", 0 );
        perror( "failed open of brands_table.dat in load_brands_table" );
    }
    else
    {
        for ( ; ; )
        {

            char  *word;

            word = fread_string( brandsfp );
            if ( !str_cmp( word, "#BRAND" ) )
            {
                GET_FREE( this_brand, brand_data_free );
                GET_FREE( brand_member, dl_list_free );
                this_brand->branded = fread_string( brandsfp );
                this_brand->branded_by = fread_string( brandsfp );
                this_brand->dt_stamp = fread_string( brandsfp );
                this_brand->message = fread_string( brandsfp );
                this_brand->priority = fread_string( brandsfp );

                free_string( word );

                brand_member->this_one = this_brand;
                brand_member->next = NULL;
                brand_member->prev = NULL;
                LINK( brand_member, first_brand, last_brand, next, prev );

            }
            else if ( !str_cmp( word, "#END"    ) )
            {
                free_string( word );
                break;
            }
            else
            {
                free_string( word );
                monitor_chan( NULL, "Load_brands: bad section.", MONITOR_BAD );
                break;
            }
        }

        fclose( brandsfp );

        sprintf( buf, "Done Loading %s\n\r", brands_file_name);
        monitor_chan( NULL, buf, MONITOR_SYSTEM );

    }
}

void do_immbrand( CHAR_DATA *ch, char *argument )
{
    DL_LIST * brand_list;
    DL_LIST * this_brand;
    BRAND_DATA *brand;
    char       buf  [ MAX_STRING_LENGTH   ];
    char       buf1 [ MAX_STRING_LENGTH*7 ];
    char       arg  [ MAX_INPUT_LENGTH    ];
    int        vnum = 0;
    int        anum = 0;

    if ( IS_NPC( ch ) )
        return;

    argument = one_argument( argument, arg );
    smash_tilde( argument );

    if ( arg[0] == '\0' )
    {
        do_immbrand( ch, "read" );
        return;
    }

    if ( !str_cmp( arg, "list" ) )
    {
        vnum    = 0;
        buf1[0] = '\0';
        for ( brand_list = first_brand; brand_list; brand_list = brand_list->next )
        {
            brand = brand_list->this_one;
            sprintf( buf, "[%3d] @@r%s@@W: @@GBrander@@W: %s  @@a%s @@ePriority: %s@@N\n\r",
                vnum,
                brand->branded,
                brand->branded_by,
                brand->dt_stamp,
                brand->priority );
            safe_strcat( MSL, buf1, buf );
            vnum++;
            if ( vnum > 100 )
            {
                safe_strcat( MSL, buf1, "---More Follow---\n\r" );
                break;
            }
        }

        if ( vnum == 0 )
            send_to_char( "There are no outstanding brands.\n\r", ch );
        else
        {
            /* act message */
            send_to_char( buf1, ch );
        }
        return;
    }

    if ( !str_cmp( arg, "read" ) )
    {
        if ( is_number( argument ) )
        {
            anum = atoi( argument );
        }
        else
        {
            send_to_char( "Read which brand?\n\r", ch );
            return;
        }

        vnum    = 0;
        buf1[0] = '\0';
        for ( brand_list = first_brand; brand_list; brand_list = brand_list->next )
        {
            if (  vnum++ == anum  )
            {
                brand = brand_list->this_one;
                sprintf( buf, "[%3d] @@r%s@@W: @@GBrander@@W: %s  @@a%s @@ePriority: %s@@N\n\r",
                    anum,
                    brand->branded,
                    brand->branded_by,
                    brand->dt_stamp,
                    brand->priority );
                safe_strcat( MSL, buf1, buf );
                safe_strcat( MSL, buf1, brand->message );
                send_to_char( buf1, ch );
                return;
            }
            else
                continue;
            send_to_char( "No such brand.\n\r", ch );
            return;
        }
    }

    if ( !str_cmp( arg, "write" ) || !str_cmp( arg, "edit" ) )
    {
        if ( ch->current_brand == NULL )
        {
            GET_FREE( ch->current_brand, brand_data_free );
            ch->current_brand->branded = str_dup( "" );
            ch->current_brand->branded_by = str_dup( "" );
            ch->current_brand->message = str_dup( "" );
            ch->current_brand->dt_stamp = str_dup( "" );
            ch->current_brand->priority = str_dup( "" );
        }

        build_strdup( &ch->current_brand->message, "$edit", TRUE, ch );
        return;
    }

    if ( !str_cmp( arg, "player" ) )
    {
        if ( ch->current_brand == NULL )
        {
            GET_FREE( ch->current_brand, brand_data_free );
            ch->current_brand->branded = str_dup( "" );
            ch->current_brand->branded_by = str_dup( "" );
            ch->current_brand->message = str_dup( "" );
            ch->current_brand->dt_stamp = str_dup( "" );
            ch->current_brand->priority = str_dup( "" );
        }

        free_string( ch->current_brand->branded );
        ch->current_brand->branded = str_dup( argument );
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "priority" ) )
    {
        if ( ch->current_brand == NULL )
        {
            GET_FREE( ch->current_brand, brand_data_free );
            ch->current_brand->branded = str_dup( "" );
            ch->current_brand->branded_by = str_dup( "" );
            ch->current_brand->message = str_dup( "" );
            ch->current_brand->dt_stamp = str_dup( "" );
            ch->current_brand->priority = str_dup( "" );
        }
        free_string( ch->current_brand->priority );
        ch->current_brand->priority = str_dup( argument );
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "clear" ) )
    {
        if ( ch->current_brand )
        {
            PUT_FREE(ch->current_brand, brand_data_free);
            ch->current_brand       = NULL;
        }
        save_brands ( );
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "show" ) )
    {
        if ( !ch->current_brand )
        {
            send_to_char( "You have no brand in progress.\n\r", ch );
            return;
        }
        buf1[0] = '\0';
        sprintf( buf, "[%3d] %s: Brander: %s  Date: %s Priority: %s\n\r",
            vnum,
            ch->current_brand->branded,
            ch->current_brand->branded_by,
            ch->current_brand->dt_stamp,
            ch->current_brand->priority );
        safe_strcat( MSL, buf1, buf );
        safe_strcat( MSL, buf1, ch->current_brand->message );
        send_to_char( buf1, ch );
        return;
    }

    if ( !str_cmp( arg, "post" )  )
    {

        char *strtime;

        if ( !ch->current_brand )
        {
            send_to_char( "You have no brand in progress.\n\r", ch );
            return;
        }

        if ( !str_cmp( ch->current_brand->branded, "" ) )
        {
            send_to_char(
                "You need to provide a player name .\n\r", ch );
            return;
        }

        if ( !str_cmp( ch->current_brand->message, "" ) )
        {
            send_to_char( "You need to provide a message.\n\r", ch );
            return;
        }

        strtime             = ctime( &current_time );
        strtime[strlen(strtime)-1]  = '\0';
        free_string( ch->current_brand->dt_stamp );
        ch->current_brand->dt_stamp = str_dup( strtime );
        free_string( ch->current_brand->branded_by );
        ch->current_brand->branded_by = str_dup( ch->name );
        GET_FREE( this_brand, dl_list_free );
        this_brand->next = NULL;
        this_brand->prev = NULL;
        this_brand->this_one = ch->current_brand;
        LINK(this_brand, first_brand, last_brand, next, prev);
        ch->current_brand       = NULL;
        save_brands( );
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "remove" ) )
    {
        if ( !is_number( argument ) )
        {
            send_to_char( "Remove which letter number?\n\r", ch );
            return;
        }

        anum = atoi( argument );
        vnum = 0;
        for ( brand_list = first_brand; brand_list; brand_list = brand_list->next )
        {
            if (  vnum++ == anum  )
            {
                break;
            }
        }
        if ( brand_list != NULL )
        {
            UNLINK( brand_list, first_brand, last_brand, next, prev );
            brand = brand_list->this_one;
            PUT_FREE(brand, brand_data_free);
            brand_list->this_one = NULL;
            PUT_FREE( brand_list, dl_list_free );
            save_brands( );
            return;
        }

        send_to_char( "No such brand.\n\r", ch );
        return;
    }

    send_to_char( "Huh?  Type 'help letter' for usage.\n\r", ch );
    return;
}
