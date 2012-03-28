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
#include <string.h>
#include <time.h>
#include "ack.h"
#include <math.h>
#include <stdlib.h>

void load_sysdata( void )
{
    FILE *sysfp;
    char sys_file_name[MAX_STRING_LENGTH];
    extern bool wizlock;
    extern bool paintlock;
    sprintf( sys_file_name, "%s", SYSDAT_FILE );

    if ( ( sysfp = fopen( sys_file_name, "r" ) ) == NULL )
    {
        bug( "Load Sys Table: fopen", 0 );
        log_f( "failed open of system.dat in load_sysdata" );
    }
    else
    {
        sh_int    looper;
        sysdata.playtesters   = fread_string( sysfp );
        for ( looper = 0; looper < 1; looper++ )
            sysdata.imms[looper].this_string    = fread_string( sysfp );
        sysdata.w_lock          = fread_number( sysfp );
        sysdata.shownumbers = ( fread_number( sysfp ) == 1 ? TRUE : FALSE );
        sysdata.showhidden          = fread_number( sysfp );
        sysdata.killfest          = fread_number( sysfp );
        sysdata.p_lock          = fread_number( sysfp );
        sysdata.kill_groups      = fread_number( sysfp );
        sysdata.silent_reboot = fread_number( sysfp );
        sysdata.qpmode = fread_number(sysfp);
        sysdata.xpmode = fread_number(sysfp);
        sysdata.pikamod = FALSE;
        sysdata.freemap = 0;
        if ( sysdata.w_lock == 1 )
        {
            wizlock = TRUE;
        }
        if ( sysdata.p_lock == 1 )
        {
            paintlock = TRUE;
        }
    }
    fclose( sysfp );
}

void save_sysdata( void )
{

    FILE * fp;
    char sys_file_name[MAX_STRING_LENGTH];
    extern bool wizlock;
    extern bool paintlock;

    fclose( fpReserve );
    sprintf( sys_file_name, "%s", SYSDAT_FILE );

    if ( ( fp = fopen( sys_file_name, "w" ) ) == NULL )
    {
        bug( "Save Sysdata: fopen", 0 );
        log_f( "failed open of system.dat in save_sysdata" );
    }
    else
    {
        sh_int  looper;
        fprintf( fp, "%s~\n", sysdata.playtesters );
        for ( looper = 0; looper < 1; looper++ )
            fprintf( fp, "%s~\n", sysdata.imms[looper].this_string );
        fprintf( fp, "%d\n", ( wizlock ? 1 : 0 ) );
        fprintf( fp, "%d\n", ( sysdata.shownumbers ? 1 : 0 ) );
        fprintf( fp, "%d\n", sysdata.showhidden );
        fprintf( fp, "%d\n", sysdata.killfest );
        fprintf( fp, "%d\n", ( paintlock ? 1 : 0 ) );
        fprintf( fp, "%d\n", ( sysdata.kill_groups ? 1 : 0 ) );
        fprintf( fp, "%d\n", ( sysdata.silent_reboot ? 1 : 0 ) );
        fprintf( fp, "%d\n", sysdata.qpmode );
        fprintf( fp, "%d\n", sysdata.xpmode );
        fflush( fp );
        fclose( fp );
    }
    fpReserve = fopen( NULL_FILE, "r" );
    return;

}

void do_sysdata( CHAR_DATA * ch, char * argument )
{
    char  outbuf[MSL];
    char  catbuf[MSL];
    char  arg1[MSL];
    char  arg2[MSL];
    extern bool wizlock;
    if ( get_trust( ch ) < MAX_LEVEL )
    {
        send_to_char( "Nice try.\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg1 );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax for sysedit:\n\r", ch );
        send_to_char( "  sysedit config <value> | <string> <+/-> <new_word>\n\r", ch );
        send_to_char( "  strings:  testers 81 82 83 84 85\n\r", ch );
        send_to_char( "  config values: shownumbers, showhidden, pikamod, killfest, killgroups\n\r", ch );
        return;
    }
    if ( !str_prefix( arg1, "show" ) )
    {
        sh_int  looper;
        sprintf( outbuf,"%s", "System data for " mudnamecolor ":\n\r" );
        sprintf( catbuf,"Playtesters: %s\n\r", sysdata.playtesters );
        safe_strcat( MSL, outbuf, catbuf );
        for ( looper = 0; looper < 1; looper++ )
        {
            sprintf( catbuf,"Level %d Immortals: %s\n\r",
                81+looper,
                sysdata.imms[looper].this_string );
            safe_strcat( MSL, outbuf, catbuf );
        }
        sprintf( catbuf, "Rooms freed: %d\n\r", sysdata.freemap );
        safe_strcat( MSL, outbuf, catbuf );
        sprintf( catbuf, "Wizlocked: %s\n\r",
            ( wizlock ? "Yes" : "No" )  );
        safe_strcat( MSL, outbuf, catbuf );
        sprintf( catbuf, "Show Players Damage numbers in combat: %s\n\r",
            ( sysdata.shownumbers ? "Yes" : "No" ) );
        safe_strcat( MSL, outbuf, catbuf );
        sprintf( catbuf, "Pika mod? %s\n\r",
            ( sysdata.pikamod ? "Yes" : "No" ) );
        safe_strcat( MSL, outbuf, catbuf );
        sprintf( catbuf, "Show Hidden Players in WHO list: %d\n\r",
            sysdata.showhidden);
        safe_strcat( MSL, outbuf, catbuf );
        sprintf( catbuf, "Killfest? %s\n\r",
            sysdata.killfest ? "Yes" : "No" );
        safe_strcat( MSL, outbuf, catbuf );
        sprintf( catbuf, "Kill Groups? %s\n\r",
            sysdata.kill_groups ? "Yes" : "No" );
        safe_strcat( MSL, outbuf, catbuf );
        send_to_char( outbuf, ch );
        return;
    }
    if ( !str_cmp( arg1, "config" ) )
    {
        argument = one_argument( argument, arg2 );
        if ( !str_cmp( arg2, "shownumbers" ) )
        {
            sysdata.shownumbers = ( sysdata.shownumbers ? FALSE : TRUE );
            save_sysdata( );
            do_sysdata( ch, "show" );
            return;
        }
        else if ( !str_cmp( arg2, "Pikamod" ) )
        {
            sysdata.pikamod = ( sysdata.pikamod ? FALSE : TRUE );
            save_sysdata( );
            do_sysdata( ch, "show" );
            return;
        }
        else if ( !str_cmp( arg2, "Killgroups" ) )
        {
            sysdata.kill_groups = ( sysdata.kill_groups ? FALSE : TRUE );
            save_sysdata( );
            do_sysdata( ch, "show" );
            return;
        }
        else if ( !str_cmp( arg2, "Killfest" ) )
        {
            sysdata.killfest = ( sysdata.killfest ? FALSE : TRUE );
            save_sysdata( );
            do_sysdata( ch, "show" );
            if ( sysdata.killfest )
            {
                BUILDING_DATA *bld;
                BUILDING_DATA *bld_next;
                do_backup(ch,"savefest");
                info("@@2@@aThe KILLING FEST has begun!@@N",0);
                for ( bld = first_building;bld;bld = bld_next )
                {
                    bld_next = bld->next;
                    extract_building(bld,TRUE);
                }
                while ( first_vehicle )
                    extract_vehicle(first_vehicle,TRUE);
                while ( first_obj )
                    extract_obj(first_obj);
                do_pipe(NULL,"cp -rf ../player ../player_fst");
                do_hotreboot(NULL,"silent");
            }
            else
            {
                do_backup(ch,"loadfest");
                info("@@2@@aThe KILLING FEST has ended!@@N",0);
                save_buildings();
                save_objects(0);
                save_vehicles(0);
                do_pipe(NULL,"mv -f ../player_fst player");
                do_hotreboot(NULL,"nosave");
            }
            return;
        }
        else if ( !str_prefix( arg2, "Showhidden" ) )
        {
            if ( !is_number(argument) || atoi(argument) < -1 )
                return;
            sysdata.showhidden = atoi(argument);
            save_sysdata( );
            do_sysdata( ch, "show" );
            return;
        }
        else
        {
            send_to_char( "Option not implemented.\n\r", ch );
            return;
        }
    }

    if ( !is_name( arg1, "testers 81 82 83 84 85" ) )
    {
        do_sysdata( ch, "" );
        return;
    }
    for ( ; ; )
    {
        sh_int imm_level = -1;
        argument = one_argument( argument, arg2 );
        if ( is_number( arg2 ) )
            if ( ( imm_level = atoi( arg2 ) - 81 ) < 0 || imm_level >= 5 )
        {
            send_to_char( "Imm levels must be 81 to 85\n\r", ch );
            return;
        }

        if ( str_cmp( arg2, "" ) )
        {
            if ( !str_prefix( arg1, "testers" ) )
            {
                sysdata.playtesters = str_mod( sysdata.playtesters, arg2 );
            }
            else if ( is_name( arg1, "81 82 83 84 85" ) )
            {
                char arg3[MSL];
                strcpy( arg3, argument );
                if (  ( imm_level < 0 )
                    || ( imm_level >= 5 )  )
                {
                    send_to_char( "Illegal Immortal level selected.\n\r", ch );
                    return;
                }
                sysdata.imms[imm_level].this_string = str_mod( sysdata.imms[imm_level].this_string, arg2 );
            }
            else
            {
                send_to_char( "Unknown system string.\n\r", ch );
                do_sysdata( ch, "" );
                return;
            }
        }
        else
            break;
    }
    save_sysdata( );
    do_sysdata( ch, "show" );
    return;
}
