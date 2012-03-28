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
#include <time.h>
#include "ack.h"

/*
 * All of the global linked lists, in one clump.  Variables here,
 * declarations in lists.h
 * -- Altrag
 */
AREA_DATA *     first_area = NULL;
AREA_DATA *     last_area = NULL;
BAN_DATA *      first_ban = NULL;
BAN_DATA *      last_ban = NULL;
CHAR_DATA *     first_char = NULL;
CHAR_DATA *     last_char = NULL;
BUILDING_DATA *     first_building = NULL;
BUILDING_DATA *     last_building = NULL;
BUILDING_DATA *     first_active_building = NULL;
BUILDING_DATA *     last_active_building = NULL;
VEHICLE_DATA *      first_vehicle = NULL;
VEHICLE_DATA *      last_vehicle = NULL;
DESCRIPTOR_DATA *   first_desc = NULL;
DESCRIPTOR_DATA *   last_desc = NULL;
HELP_DATA *     first_help = NULL;
HELP_DATA *     last_help = NULL;
OBJ_DATA *      first_obj = NULL;
BOMB_DATA *     first_bomb = NULL;
BOMB_DATA *     last_bomb = NULL;
OBJ_DATA *      last_obj = NULL;
CONTROL_LIST    *   first_control_list = NULL;
CONTROL_LIST    *   last_control_list = NULL;
QUEUED_INTERACT_LIST *  first_queued_interact = NULL;
QUEUED_INTERACT_LIST *  last_queued_interact = NULL;
INFLUENCE_LIST  *   first_influence_list = NULL;
INFLUENCE_LIST  *   last_influence_list = NULL;
DL_LIST     *   first_brand = NULL;
DL_LIST     *   last_brand = NULL;
BOARD_DATA   *  first_board = NULL;
BOARD_DATA   *  last_board = NULL;
BUF_DATA_STRUCT * first_buf=NULL;
BUF_DATA_STRUCT * last_buf = NULL;

AREA_DATA *     area_free = NULL;
BAN_DATA *      ban_free = NULL;
CHAR_DATA *     char_free = NULL;
DESCRIPTOR_DATA *   desc_free = NULL;
HELP_DATA *     help_free = NULL;
OBJ_DATA *      obj_free = NULL;
OBJ_INDEX_DATA *    oid_free = NULL;
ROOM_INDEX_DATA *   rid_free = NULL;
PC_DATA *       pcd_free = NULL;
BUILD_DATA_LIST *   build_free = NULL;
INTERACT_DATA *     interact_free = NULL;
INFLUENCE_DATA  *   influence_free= NULL;
CONTROL_DATA    *   control_data_free = NULL;
CONTROL_LIST    *   control_list_free = NULL;
QUEUED_INTERACT_LIST *  queued_interact_free = NULL;
INFLUENCE_LIST  *   influence_list_free = NULL;
DL_LIST     *   dl_list_free = NULL;
BRAND_DATA  *   brand_data_free = NULL;
BOARD_DATA   *  board_free = NULL;
MESSAGE_DATA *  message_free = NULL;
BUF_DATA_STRUCT * buf_free = NULL;
HASH_ENTRY * hash_free = NULL;
BUILDING_DATA *     building_free = NULL;
VEHICLE_DATA *      vehicle_free = NULL;
QUEUE_DATA *        queue_free = NULL;
BOMB_DATA *         bomb_free = NULL;
PAGER_DATA *        pager_free = NULL;

void (*area_free_destructor )            ( AREA_DATA * adat ) = NULL;
void (*desc_free_destructor )            ( DESCRIPTOR_DATA * ddat ) = NULL;
void (*help_free_destructor )            ( HELP_DATA * hdat ) = NULL;
void (*build_free_destructor )           ( BUILD_DATA_LIST * bddat ) = NULL;
void (*interact_free_destructor )        ( INTERACT_DATA * idat ) = NULL;
void (*influence_free_destructor)        ( INFLUENCE_DATA * idat ) = NULL;
void (*control_data_free_destructor )    ( CONTROL_DATA * cdat ) = NULL;
void (*control_list_free_destructor )    ( CONTROL_LIST * cldat ) = NULL;
void (*queued_interact_free_destructor ) ( QUEUED_INTERACT_LIST * qildat ) = NULL;
void (*influence_list_free_destructor )  ( INFLUENCE_LIST * ildat ) = NULL;
void (*dl_list_free_destructor )         ( DL_LIST * dldat ) = NULL;
void (*board_free_destructor )           ( BOARD_DATA * bdat ) = NULL;
void (*buf_free_destructor )             ( BUF_DATA_STRUCT * bdat ) = NULL;
void (*hash_free_destructor )            ( HASH_ENTRY * hdat )= NULL;

void pager_free_destructor ( PAGER_DATA * pdat )
{
    free_string(pdat->from);
    free_string(pdat->time);
    free_string(pdat->msg);
}

void queue_free_destructor ( QUEUE_DATA * qdat )
{
    free_string(qdat->command);
}

void message_free_destructor ( MESSAGE_DATA * mdat )
{
    free_string(mdat->author);
    free_string(mdat->title);
    free_string(mdat->message);
}

void ban_free_destructor( BAN_DATA * bdat )
{
    free_string( bdat->name );
    free_string( bdat->banned_by );
    free_string( bdat->note );
}

void oid_free_destructor( OBJ_INDEX_DATA * oidat )
{
    /* Free strings */
    free_string(oidat->name);
    free_string(oidat->short_descr);
    free_string(oidat->description);
    free_string(oidat->image);
}

void brand_data_free_destructor( BRAND_DATA * bdat )
{
    free_string( bdat->branded);
    free_string( bdat->branded_by);
    free_string( bdat->dt_stamp);
    free_string( bdat->message);
    free_string( bdat->priority);
}

void pcd_free_destructor( PC_DATA * pcdat )
{
    sh_int cnt;

    free_string( pcdat->pwd            );
    free_string( pcdat->bamfin         );
    free_string( pcdat->room_enter        );
    free_string( pcdat->room_exit     );
    free_string( pcdat->bamfout        );
    free_string( pcdat->title          );
    free_string( pcdat->ranking        );
    free_string( pcdat->host      );
    free_string( pcdat->header );
    free_string( pcdat->message );
    free_string( pcdat->who_name );
    free_string( pcdat->lastlogin );
    for ( cnt = 0; cnt < MAX_IGNORES ; cnt++ )
    {
        free_string( pcdat->ignore_list[cnt] );
    }
    free_string( pcdat->load_msg );
    free_string( pcdat->pedit_state );
    for ( cnt = 0; cnt < 5 ; cnt++ )
    {
        free_string( pcdat->pedit_string[cnt] );
    }
    free_string( pcdat->email_address );
}

void char_free_destructor( CHAR_DATA * cdat )
{
    free_string( cdat->name               );
    free_string( cdat->prompt );
    free_string( cdat->old_prompt );
    free_string( cdat->last_tell );
    {
        int i;
        for ( i=0;i<5;i++ )
        {
            free_string( cdat->alias[i] );
            free_string( cdat->alias_command[i] );
        }
    }
}

void obj_free_destructor( OBJ_DATA * odat )
{
    free_string( odat->owner   );
    free_string( odat->name        );
    free_string( odat->description );
    free_string( odat->short_descr );
}

void building_free_destructor( BUILDING_DATA * bdat )
{
    free_string( bdat->owned );
    free_string( bdat->name );
    free_string( bdat->attacker );
}

void vehicle_free_destructor( VEHICLE_DATA * vdat )
{
    free_string( vdat->name );
    free_string( vdat->desc );
}

void bomb_free_destructor( BOMB_DATA *bdat )
{
}
