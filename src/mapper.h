/*~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-~-
 ~  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        ~
 ~  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   ~
 ~                                                                         ~
 ~  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          ~
 ~  Chastain, Michael Quan, and Mitchell Tse.                              ~
 ~                                                                         ~
 ~  Ack62 2.2 improvements copyright (C) 1994 by Stephen Dooley            ~
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

#ifdef ACK_43
#include "cursor.h"
#ifndef DEC_ACK_H
#include "ack.h"
#endif
#else
#ifndef DEC_MERC_H
#include "merc.h"
#endif
#endif

#define MAX_MAP 40
#define MAX_MAP_DIR 4

#define DOOR_LOCKED -1
#define DOOR_CLOSED -2
#define DOOR_OPEN -3
#define DOOR_NS -4
#define DOOR_EW -5
#define DOOR_NULL -6

#define LOS_INITIAL -5
#define MAP_Y       9
struct room_content_type
{
    char string[10];
};

struct room_content_type contents[MAX_MAP][MAX_MAP];

int map[MAX_MAP][MAX_MAP];

extern sh_int rev_dir[];

struct map_info_type
{
    int sector_type;
    char * display_color;
    char * display_code;
    char * invert_color;
    char * desc;
};

void MapArea
(ROOM_INDEX_DATA *room, CHAR_DATA *ch, int x, int y, int min, int max, int line_of_sight);

void ShowMap( CHAR_DATA *ch, int min, int max, int size, int center );
void ShowHalfMap( CHAR_DATA *ch, int min, int max);
void do_printmap(CHAR_DATA *ch, char *argument);
void ShowRoom( CHAR_DATA *ch, int min, int max, int size, int center, bool full);
void ShowWMap( CHAR_DATA *ch, sh_int small, int size );
void ShowCMap( CHAR_DATA *ch );
void do_map( CHAR_DATA *ch, char *argument );
void do_smallmap( CHAR_DATA *ch, char *argument );
char *makesmall (char *arg,int size);
void ShowBMap( CHAR_DATA *ch, bool quest );
void show_building( CHAR_DATA *ch, sh_int small, int size );
void ShowSpace( CHAR_DATA *ch );
void ShowSMap( CHAR_DATA *ch, bool small );
