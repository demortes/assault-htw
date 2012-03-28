/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  Ack 2.2 improvements copyright (C) 1994 by Stephen Dooley              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *       _/          _/_/_/     _/    _/     _/    ACK! MUD is modified    *
 *      _/_/        _/          _/  _/       _/    Merc2.0/2.1/2.2 code    *
 *     _/  _/      _/           _/_/         _/    (c)Stephen Zepp 1998    *
 *    _/_/_/_/      _/          _/  _/             Version #: 4.3          *
 *   _/      _/      _/_/_/     _/    _/     _/                            *
 *                                                                         *
 *                        http://ackmud.nuc.net/                           *
 *                        zenithar@ackmud.nuc.net                          *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 *  money.h -- handles multi unit money transactions.
 */

#ifndef DEC_TYPEDEFS_H
#include "typedefs.h"
#endif

#ifndef DEC_ACK_H
#include "ack.h"
#endif

#define DEC_MONEY_H     1

#define MONEY_WEIGHT .001

long_int new_money_lookup( char * money_name );
long_int money_to_value( char * money_str );

bool can_carry_gold( CHAR_DATA * ch, long_int amount );
char * new_money_string( long_int val );

void deposit_money( CHAR_DATA * ch, char * argument );
bool withdraw_money( CHAR_DATA * ch, char * argument );
void drop_money( CHAR_DATA * ch, char * argument );
bool get_money_room( CHAR_DATA * ch, char * argument );
bool get_money_obj( CHAR_DATA * ch, char * argument, OBJ_DATA * obj );
void put_money_obj( CHAR_DATA * ch, char * argument, OBJ_DATA * obj );
bool give_money( CHAR_DATA * ch, CHAR_DATA * victim, char * argument );
