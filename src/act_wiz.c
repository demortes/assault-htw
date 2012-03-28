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
/* For forks etc. */
#include <unistd.h>
#include <fcntl.h>

#include "ack.h"
#include "tables.h"

#ifndef DEC_EMAIL_H
#include "email.h"
#endif

void  purge_room   args( ( CHAR_DATA *, ROOM_INDEX_DATA * ) );
RELEVEL_DATA * HasRelevel( char * strName )
{
   RELEVEL_DATA * pRelevel = NULL;

   /* Scan through the linked list */
   for( pRelevel = rlvldata.pRelevelList; pRelevel != NULL; pRelevel = pRelevel->pNext ) {
      if( str_cmp( pRelevel->strName, strName ) ) continue;

      /* Return the data */
      return pRelevel;
   }

   /* We didn't find what we was looking for, so return NULL (nothing) */
   return NULL;
}

void do_saverelevel( void )
{
   RELEVEL_DATA * pRelevel = NULL;
   FILE * fp = NULL;

   /* Open the file for writing */
   if( ( fp = fopen( RELEVEL_FILE, "w" ) ) == NULL ) {
      //logf( "do_saverelevel: Failed to open file for writing!" );
      return;
   }
   else {
      /* Write the linked list to the file */
      for( pRelevel = rlvldata.pRelevelList; pRelevel != NULL; pRelevel = pRelevel->pNext ) {
         fprintf( fp, "%s", "#RELEVEL\n" );
         fprintf( fp, "Name         %s~\n", pRelevel->strName  );
         fprintf( fp, "Level        %d\n",  pRelevel->iLevel   );
         fprintf( fp, "%s", "End\n\n" );
      }
      fprintf( fp, "%s", "#END\n" );

      /* Close the file */
      fclose( fp );
   }
   return;
}

void do_readrelevel( FILE * fp, RELEVEL_DATA * pRelevel )
{
   const char * word = NULL;
   bool fMatch;

   for( ; ; ) {
      word = feof( fp ) ? "End" : fread_word( fp );
      fMatch = FALSE;

      switch( UPPER( word[0] ) ) {
         case 'E':
            if( !str_cmp( word, "End" ) ) {
               return;
            }
            break;
         case 'L':
            KEY( "Level",     pRelevel->iLevel,       fread_number( fp ) );
            break;
         case 'N':
            KEY( "Name",      pRelevel->strName,      fread_string( fp ) );
            break;
      }
      if( !fMatch ) {
         //logf( "do_readrelevel: no match: %s", word );
      }
   }
}

void do_loadrelevel( void )
{
   RELEVEL_DATA * pRelevel = NULL;
   FILE * fp = NULL;

   /* Open the file for reading */
   if( ( fp = fopen( RELEVEL_FILE, "r" ) ) == NULL ) {
      return;
   }

   /* Enter a loop to read all its contents */
   for( ; ; ) {
      char * strWord = NULL;

      strWord = fread_word( fp );
      if( !str_cmp( strWord, "#RELEVEL" ) ) {

         /* Allocate memory, I use calloc as it clears the memory allocated */
         if( ( pRelevel = (RELEVEL_DATA *)calloc( 1, sizeof( RELEVEL_DATA ) ) ) == NULL ) {
            //logf( "do_loadrelevel: unable to allocate memory, aborting program!" );
            abort();
         }

         /* Read in the data */
         do_readrelevel( fp, pRelevel );

         /* Add the relevel data into the list */
         pRelevel->pNext = rlvldata.pRelevelList;
         rlvldata.pRelevelList = pRelevel;
         continue;
      }
      else if( !str_cmp( strWord, "#END" ) ) {
         break;
      }
      else {
         //logf( "do_loadrelevel: bad section: '%s'.", strWord );
         continue;
      }
   }
   fclose( fp );
   return;
}

void do_setrelevel( CHAR_DATA * dch, char * arg )
{
   char strBuffer[MAX_STRING_LENGTH] = { '\0' };
   char strArg1[MAX_INPUT_LENGTH] = { '\0' };
   char strArg2[MAX_INPUT_LENGTH] = { '\0' };
   char strArg3[MAX_INPUT_LENGTH] = { '\0' };
   RELEVEL_DATA * pRelevel = NULL;
   CHAR_DATA * xch = NULL;

   /* Store the Arguments */
   arg = one_argument( arg, strArg1 );
   arg = one_argument( arg, strArg2 );
   arg = one_argument( arg, strArg3 );

   if( strArg1[0] == '\0' || strArg2[0] == '\0' ) {
      send_to_char( "Syntax: SetRelevel <Grant/Revoke> <Character> (Level)\n\n\r", dch );
      send_to_char( "Level is only required when granting access!\n\r", dch );
      return;
   }

   if( !str_cmp( strArg1, "grant" ) || !str_cmp( strArg1, "give" ) ) {
      /* Make sure they have the level parameter */
      if( strArg3[0] == '\0' ) {
         sprintf( strBuffer, "You must supply a level! (1-%d)\n\r", MAX_LEVEL );
         send_to_char( strBuffer, dch );
         return;
      }
      /* Level Check */
      if( atoi( strArg3 ) < 1 || atoi( strArg3 ) > MAX_LEVEL ) {
         sprintf( strBuffer, "The level must be between 1 and %d!\n\r", MAX_LEVEL );
         send_to_char( strBuffer, dch );
         return;
      }

      /* Find the Character to grant relevel access to */
      if( ( xch = get_char_world( dch, strArg2 ) ) == NULL ) {
         send_to_char( "They are not online.\n\r", dch );
         return;
      }

      /* Don't allow NPC's */
      if( IS_NPC( xch ) ) {
         send_to_char( "You can not add NPCs to relevel.\n\r", dch );
         return;
      }

      /* Does xch already have access to relevel? */
      if( ( pRelevel = HasRelevel( xch->name ) ) != NULL ) {
         send_to_char( "They already have access to relevel!\n\r", dch );
         return;
      }

      /* Allocate memory */
      if( ( pRelevel = (RELEVEL_DATA *)calloc( 1, sizeof( RELEVEL_DATA ) ) ) == NULL ) {
         send_to_char( "Unable to allocate memory, aborting!\n\r", dch );
         //logf( "do_loadrelevel: unable to allocate memory!" );
         return;
      }

      /* Add the relevel data into the list */
      pRelevel->pNext = rlvldata.pRelevelList;
      rlvldata.pRelevelList = pRelevel;

      /* Set the data */
      pRelevel->strName = strdup( xch->name );
      pRelevel->iLevel = atoi( strArg3 );

      /* Inform the other character that they now have access to relevel */
      send_to_char( "You now have access to relevel.\n\r", xch );

      /* Inform the admin that it was succesful */
      sprintf( strBuffer, "%s was successfully added to relevel.\n\r", xch->name );
      send_to_char( strBuffer, dch );

      /* Save the modified relevel list */
      do_saverelevel();
   }
   else if( !str_cmp( strArg1, "revoke" ) || !str_cmp( strArg1, "take" ) ) {
      if( ( pRelevel = HasRelevel( strArg2 ) ) == NULL ) {
         send_to_char( "They do not have access to relevel!\n\r", dch );
         return;
      }

      /* Remove the data from the linked list */
      if( pRelevel == rlvldata.pRelevelList ) {
         rlvldata.pRelevelList = pRelevel->pNext;
      }
      else {
         RELEVEL_DATA * pPrev = NULL;

         for( pPrev = rlvldata.pRelevelList; pPrev != NULL; pPrev = pPrev->pNext ) {
            if( pPrev->pNext == pRelevel ) {
               pPrev->pNext = pRelevel->pNext;
               break;
            }
         }
         if( pPrev == NULL ) {
            //logf( "do_setrelevel: data not found." );
            return;
         }
      }
      /* Clear the pointer */
      pRelevel->pNext = NULL;

      /* Inform the admin of the Success */
      sprintf( strBuffer, "You have revoked relevel access from %s!\n\r", pRelevel->strName );
      send_to_char( strBuffer, dch );

      /* Free the used memory */
      free( pRelevel->strName );
      pRelevel->iLevel = 0;

      /* Save the modified relevel list */
      do_saverelevel();
   }
   else {
      do_setrelevel( dch, "" );
   }
   return;
}

void do_relevel( CHAR_DATA * dch, char * arg )
{
   RELEVEL_DATA * pRelevel = NULL;

   /* Make sure they have relevel access */
   if( ( pRelevel = HasRelevel( dch->name ) ) == NULL ) {
      send_to_char( "Access Denied!\n\r", dch );
      return;
   }

   /* Send them a Message and set their level */
   send_to_char( "Relevel Complete!\n\r", dch );
   dch->level = pRelevel->iLevel;
   dch->trust = pRelevel->iLevel;
   return;
}

void do_wizhelp( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    int cmd, imm_level;
    int col = 0;
    bool creturn = TRUE;

    buf1[0] = '\0';

    for ( imm_level = LEVEL_HERO; imm_level <= get_trust( ch ); imm_level++ )
    {
        if (!creturn)
        {
            strcat( buf1, "\n\r" );
        }
        sprintf( buf, "\n\r@@aLevel @@y%d @@aCommands@@d:@@N\n\r", imm_level);
        strcat( buf1, buf);
        for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
        {
            if (cmd_table[cmd].level != imm_level)
                continue;
            if ( cmd_table[cmd].show == C_SHOW_NEVER )
                continue;

            creturn = FALSE;
            sprintf( buf, "%-12s", cmd_table[cmd].name);
            strcat( buf1, buf );
            if ( ++col % 6 == 0 )
            {
                strcat( buf1, "\n\r" );
                creturn = TRUE;
                col = 0;
            }
        }                                                   /* For command */
        col = 0;
    }                                                       /* For imm_level */

    safe_strcat( MSL, buf1, "\n\r" );
    send_to_char( buf1, ch );
    return;
}

void do_bamfin( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
        smash_tilde( argument );
        free_string( ch->pcdata->bamfin );
        ch->pcdata->bamfin = str_dup( argument );
        send_to_char( "Ok.\n\r", ch );
    }
    return;
}

void do_bamfout( CHAR_DATA *ch, char *argument )
{
    if ( !IS_NPC(ch) )
    {
        smash_tilde( argument );
        free_string( ch->pcdata->bamfout );
        ch->pcdata->bamfout = str_dup( argument );
        send_to_char( "Ok.\n\r", ch );
    }
    return;
}

void do_deny( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char buf[MSL];
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Deny whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        {
            char    buf[MSL];
            sprintf( buf, "%s is not online.\n\r", capitalize( arg ) );
            send_to_char( buf, ch );
            return;
        }

    }

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }
    if ( IS_SET( victim->act, PLR_DENY ) )
    {
        REMOVE_BIT( victim->act, PLR_DENY );
    }
    else
    {
        SET_BIT(victim->act, PLR_DENY);
        send_to_char( "You are denied access!\n\r", victim );
    }
    send_to_char( "OK.\n\r", ch );
    sprintf( buf, "%ld", current_time );
    do_quit( victim, buf );

    return;
}

void do_disconnect( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Disconnect whom?\n\r", ch );
        return;
    }

    /* Added for disconnection of descriptor numbers - Wyn */
    if (is_number(arg))
    {
        int desc;

        if (get_trust(ch) < 84)
        {
            return;
        }

        desc = atoi(arg);
        for ( d = first_desc; d != NULL; d = d->next )
        {
            if ( d->descriptor == desc )
            {
                close_socket( d );
                send_to_char( "Ok.\n\r", ch );
                return;
            }
        }
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL
        || (get_trust(victim) > get_trust(ch) && get_trust(victim) == MAX_LEVEL))
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim->desc == NULL )
    {
        act( "$N doesn't have a descriptor.", ch, NULL, victim, TO_CHAR );
        return;
    }

    for ( d = first_desc; d != NULL; d = d->next )
    {
        if ( d == victim->desc )
        {
            close_socket( d );
            send_to_char( "Ok.\n\r", ch );
            return;
        }
    }

    bug( "Do_disconnect: desc not found.", 0 );
    send_to_char( "Descriptor not found!\n\r", ch );
    return;
}

void do_echo( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Echo what?\n\r", ch );
        return;
    }

    for ( d = first_desc; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING )
        {
            send_to_char( argument, d->character );
            send_to_char( "@@g\n\r",   d->character );
        }
    }

    return;
}

void do_recho( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Recho what?\n\r", ch );
        return;
    }

    for ( d = first_desc; d; d = d->next )
    {
        if ( d->connected == CON_PLAYING
            &&   d->character->in_room == ch->in_room && !NOT_IN_ROOM(ch,d->character) )
        {
            send_to_char( argument, d->character );
            send_to_char( "@@g\n\r",   d->character );
        }
    }

    return;
}

ROOM_INDEX_DATA *find_location( CHAR_DATA *ch, char *arg )
{
    CHAR_DATA *victim;
    OBJ_DATA *obj;
    VEHICLE_DATA *vhc;

    if ( is_number(arg) )
        return get_room_index( atoi( arg ) );

    if ( ( victim = get_char_world( ch, arg ) ) != NULL )
        return victim->in_room;

    if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
        return obj->in_room;

    if ( ( vhc = get_vehicle_world( arg ) ) != NULL )
        return get_room_index(ROOM_VNUM_WMAP);
    return NULL;
}

void do_transfer( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    int x,y;
    int z = 1;
    ROOM_INDEX_DATA *location;
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Transfer whom (and where)?\n\r", ch );
        return;
    }

    if ( !str_cmp( arg1, "all" ) )
    {
        for ( d = first_desc; d != NULL; d = d->next )
        {
            if ( d->connected == CON_PLAYING
                && !IS_IMMORTAL( d->character )
                &&   d->character != ch
                &&   d->character->in_room != NULL
                &&   can_see( ch, d->character ) )
            {
                char buf[MAX_STRING_LENGTH];
                sprintf( buf, "%s %s", d->character->name, arg2 );
                do_transfer( ch, buf );
            }
        }
        return;
    }

    /*
     * Thanks to Grodyn for the optional location parameter.
     */
    location = ch->in_room;
    if ( arg2[0] == '\0' )
    {
        x = ch->x;
        y = ch->y;
        z = ch->z;
    }
    else
    {
        x = atoi(arg2);
        y = atoi(argument);
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {

        {
            char    buf[MSL];
            sprintf( buf, "%s is not online.\n\r", capitalize( arg1 ) );
            send_to_char( buf, ch );
            return;
        }

    }
    if ( victim->in_room == NULL )
    {
        send_to_char( "They are in limbo.\n\r", ch );
        return;
    }

    act( "$n is snatched by the Gods!", victim, NULL, NULL, TO_ROOM );
    char_from_room( victim );
    char_to_room( victim, location );
    move ( victim, x, y, z );
    victim->c_sn = -1;
    act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim )
        act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    send_to_char( "Ok.\n\r", ch );
}

void do_at( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *wch;
    OBJ_DATA *obj;
    int x = ch->x;
    int y = ch->y;
    int z = ch->z;
    VEHICLE_DATA *vhc;
    arg2[0] = '\0';

    argument = one_argument( argument, arg );
    if ( is_number(arg) )
        argument = one_argument( argument, arg2 );

    if ( arg[0] == '\0' || ( is_number(arg2) && arg2[0] == '\0' ) || argument[0] == '\0' )
    {
        send_to_char( "At where what?\n\r", ch );
        return;
    }
    if ( get_trust(ch) <= 83 )
        SET_BIT(ch->pcdata->pflags,PFLAG_HELPING);

    if ( ( wch = get_char_world( ch, arg ) ) != NULL && ( !is_number(arg2) || arg2[0] == '\0' ))
    {
        move( ch, wch->x, wch->y, wch->z );
    }
    else if ( ( obj = get_obj_world( ch, arg ) ) != NULL && ( !is_number(arg2) || arg2[0] == '\0' ) )
    {
        move( ch, obj->x, obj->y, obj->z );
    }
    else if ( ( vhc = get_vehicle_world( arg ) ) != NULL && ( !is_number(arg2) || arg2[0] == '\0' ) )
    {
        move( ch, vhc->x, vhc->y, vhc->z );
    }
    else if ( is_number(arg) && is_number(arg2) )
    {
        if ( atoi(arg) <= 0 || atoi(arg) >= MAX_MAPS || atoi(arg2) <= 0 || atoi(arg2) >= MAX_MAPS )
        {
            send_to_char( "Invalid coordinates.\n\r", ch );
            return;
        }
        move ( ch, atoi(arg), atoi(arg2), ch->z );
    }
    else
    {
        send_to_char( "No such location.\n\r", ch);
        if ( get_trust(ch) <= 83 )
            REMOVE_BIT(ch->pcdata->pflags,PFLAG_HELPING);
        return;
    }
    interpret( ch, argument );

    /*
     * See if 'ch' still exists before continuing!
     * Handles 'at XXXX quit' case.
     */
    for ( wch = first_char; wch != NULL; wch = wch->next )
    {
        if ( wch == ch )
        {
            move ( ch, x, y, z );
            break;
        }
    }

    if ( get_trust(ch) <= 83 )
        REMOVE_BIT(ch->pcdata->pflags,PFLAG_HELPING);
    return;
}

void do_goto( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *rch;
    OBJ_DATA *obj;
    VEHICLE_DATA *vhc;
    int x,y;
    bool guide = IS_GUIDE(ch);

    if ( get_trust(ch) < LEVEL_GUIDE )
    {
        send_to_char( "You are not an immortal.\n\r", ch );
        return;
    }
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Goto where?\n\r", ch );
        return;
    }

    if ( guide )
    {
        if ( ch->fighttimer > 0 )
        {
            send_to_char( "Finish combat, first.\n\r", ch );
            return;
        }
        if ( IS_SET(ch->pcdata->pflags,PFLAG_HELPING) )
        {
            do_home(ch,ch->name);
            REMOVE_BIT(ch->pcdata->pflags,PFLAG_HELPING);
            return;
        }
        if ( ( rch = get_char_world( ch, arg ) ) != NULL )
        {
            if ( get_rank(rch) > 5 )
            {
                send_to_char( "You may only goto players of rank 5 and below.\n\r", ch );
                return;
            }
            if ( rch->z == Z_SPACE || rch->z == Z_PAINTBALL || IN_PIT(rch) )
            {
                send_to_char( "You can't go to that player's location right now.\n\r", ch );
                return;
            }
            act( "$L$n $T.", ch, NULL,
                (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
            move ( ch, rch->x, rch->y, rch->z );
            SET_BIT(ch->pcdata->pflags,PFLAG_HELPING);
        }
    }

    if ( is_number(argument))
    {
        if ( is_number(argument) && is_number(arg) )
        {
            x = atoi(arg);
            y = atoi(argument);
            if ( x < 0 || x >= MAX_MAPS  || y < 0 || y >= MAX_MAPS )
            {
                send_to_char( "Invalid coordinates.\n\r", ch );
                return;
            }
            {
                act( "$L$n $T.", ch, NULL,
                    (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                    ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
            }
            move(ch,x,y,ch->z);
            {
                act( "$L$n $T.", ch, NULL,
                    (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                    ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );
            }

            do_look( ch, "auto" );
            return;
        }
        send_to_char( "No such location.\n\r", ch );
        return;
    }
    else if ( ( rch = get_char_world( ch, arg ) ) != NULL )
    {
        act( "$L$n $T.", ch, NULL,
            (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
            ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
        move ( ch, rch->x, rch->y, rch->z );
    }
    else if ( ( vhc = get_vehicle_world(arg) ) != NULL )
    {
        act( "$L$n $T.", ch, NULL,
            (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
            ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
        move ( ch, vhc->x, vhc->y, vhc->z );
    }
    else
    {
        int i;
        for ( i=0;planet_table[i].name != NULL;i++ )
        {
            if ( !str_cmp(planet_table[i].name,arg) )
            {
                act( "$L$n $T.", ch, NULL,
                    (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                    ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
                move ( ch, ch->x, ch->y, planet_table[i].z );
                act( "$L$n $T.", ch, NULL,
                    (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                    ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );

                do_look( ch, "auto" );
                return;
            }
        }
        if ( ( obj = get_obj_world(ch,arg) ) != NULL )
        {
            act( "$L$n $T.", ch, NULL,
                (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0')
                ? ch->pcdata->bamfout : "leaves in a swirling mist",  TO_ROOM );
            move ( ch, obj->x, obj->y, obj->z );
            act( "$L$n $T.", ch, NULL,
                (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
                ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );
            do_look(ch,"auto");
            return;
        }
        send_to_char( "No such location.\n\r", ch );
        return;
    }

    act( "$L$n $T.", ch, NULL,
        (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
        ? ch->pcdata->bamfin : "appears in a swirling mist", TO_ROOM );

    do_look( ch, "auto" );
    return;
}

void do_ostat( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int cnt;
    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Ostat what?\n\r", ch );
        return;
    }

    buf1[0] = '\0';

    if ( ( obj = get_obj_carry( ch, arg ) ) == NULL )
        if ( ( obj = get_obj_world( ch, arg ) ) == NULL )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
        return;
    }

    sprintf( buf, "Name: %s.\n\r",
        obj->name );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "Vnum: %d.  Type: %s.\n\r",
        obj->pIndexData->vnum, item_type_name( obj ) );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "Short description: %s.\n\rLong description: %s\n\r",
        obj->short_descr, obj->description );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "Wear bits: %s.\n\rExtra bits: %s.\n\r",
        bit_table_lookup( tab_wear_flags, obj->wear_flags ),
        extra_bit_name( obj->extra_flags ) );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "Number: %d/%d.  Weight: %d/%d.\n\r",
        1,           get_obj_number( obj ),
        obj->weight, get_obj_weight( obj ) );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "Heat: %d.\n\r", obj->heat );
    sprintf( buf+strlen(buf), "Building: %s.\n\r", (obj->pIndexData->building<0||obj->pIndexData->building>=MAX_BUILDING)?"Unknown":build_table[obj->pIndexData->building].name );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf, "Level: %d.\n\r",
        obj->level );
    safe_strcat( MSL, buf1, buf );

    sprintf( buf,
        "In room: %d.  Carried by: %s.  Wear_loc: %d  Owner: %s.\n\r",
        obj->in_room    == NULL    ?        0 : obj->in_room->vnum,
        obj->carried_by == NULL    ? "(none)" : obj->carried_by->name,
        obj->wear_loc, obj->owner );
    safe_strcat( MSL, buf1, buf );

    safe_strcat( MSL, buf1, "Item Values:\n\r" );
    for ( cnt = 0; cnt < MAX_OBJECT_VALUES; cnt++ )
    {
        sprintf( buf, "@@W[Value%d : @@y%6d@@W] %s",
            cnt, obj->value[cnt],
            rev_table_lookup( tab_value_meanings,  (obj->item_type *MAX_OBJECT_VALUES ) + cnt  ) );
        safe_strcat( MSL, buf1, buf );
        sprintf( buf, "@@g\n\r" );
        safe_strcat( MSL, buf1, buf );
    }

    if ( obj->pIndexData->image )
        sprintf( buf1+strlen(buf1), "Image: %s\n\r", obj->pIndexData->image );
    send_to_char( buf1, ch );
    return;
}

void do_mstat( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Mstat whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    display_details(ch,victim);
    return;
}

void do_olmsg( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    DESCRIPTOR_DATA d;
    bool found;
    char buf[MSL];
    char arg[MSL];

    argument = one_argument(argument,arg);
    found = load_char_obj( &d, arg, FALSE );

    if (!found)
    {
        sprintf( buf, "No pFile found for '%s'.\n\r", capitalize( argument ) );
        send_to_char( buf, ch );
        free_char( d.character );                           /* Added - Wyn */
        return;
    }

    victim = d.character;
    d.character = NULL;
    victim->desc = NULL;

    smash_tilde( argument );
    free_string( victim->pcdata->load_msg );
    victim->pcdata->load_msg = str_dup( argument );
    save_char_obj(victim);
    send_to_char( "Ok.\n\r", ch );
    free_char(victim);
    victim = NULL;
    return;
}

void do_ofindlev( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MSL];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;
    int level;
    int level_top;
    int objlev;
    bool mailme = FALSE;
    if ( is_name( "mailme", argument ) )
        mailme = TRUE;
    argument = one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Ofindlev what lev.?\n\r", ch );
        return;
    }
    level       = is_number(arg) ? atoi(arg) : 0;

    argument = one_argument( argument, arg2 );
    if (  ( arg2[0] == '\0' )
        || ( !is_number( arg2 ) )  )
    {
        level_top = level;
    }
    else
    {
        level_top = atoi( arg2 );
    }
    buf1[0] = '\0';
    fAll        = !str_cmp( arg, "all" );
    found       = FALSE;
    nMatch      = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_mob_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
            nMatch++;
            if (  ( fAll )
                || (  ( pObjIndex->level >= level  )
                && ( pObjIndex->level <= level_top )  )  )

            {
                found = TRUE;
                objlev=pObjIndex->level;

                {
                    sprintf( buf, "\n\r(@@aMORTAL@@N) [%3d] [%5d] %s", pObjIndex->level,
                        pObjIndex->vnum, capitalize( pObjIndex->short_descr ) );
                    if ( mailme )
                        safe_strcat(MAX_STRING_LENGTH, buf1, buf);
                    else
                        send_to_char( buf, ch );
                }
            }
        }
    }
    safe_strcat( MSL, buf1, "\n\r" );
    if ( !found )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
        return;
    }
    sprintf( buf, "Ofindlev report for level range %d to %d", level, level_top );
    send_rep_out( ch, buf1, mailme, buf  );
    return;
}

void do_ofind( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;
    bool mailme = FALSE;
    if ( is_name( "mailme", argument ) )
        mailme = TRUE;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Ofind what?\n\r", ch );
        return;
    }

    sprintf( buf1, "%s", " Vnum  Lvl  Flag    Item\n\r" );
    fAll        = !str_cmp( arg, "all" );
    found       = FALSE;
    nMatch      = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
            nMatch++;
            if ( fAll || is_name( arg, pObjIndex->name ) )
            {
                found = TRUE;
                sprintf( buf, "[%5d] [%3d] %s\n\r",
                    pObjIndex->vnum, pObjIndex->level,
                    capitalize( pObjIndex->short_descr ) );
                safe_strcat(MAX_STRING_LENGTH, buf1, buf);
            }
        }
    }

    if ( !found )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
        return;
    }

    sprintf( buf, "Ofind report for %s", arg );
    send_rep_out( ch, buf1, mailme, buf  );
    return;
}

void do_reboo( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to REBOOT, spell it out.\n\r", ch );
    return;
}

void do_reboot( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    extern int saving_area;

    build_save_flush();

    if (saving_area)
    {
        send_to_char("Please wait until area saving complete.\n",ch);
        return;
    }

    sprintf( buf, "Reboot by %s.", ch->name );
    do_echo( ch, buf );
    merc_down = TRUE;
    return;
}

void do_shutdow( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to SHUTDOWN, spell it out.\n\r", ch );
    return;
}

void do_shutdown( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool merc_down;
    extern int  saving_area;

    build_save_flush();

    if (saving_area)
    {
        send_to_char("Please wait until area saving complete.\n",ch);
        return;
    }

    sprintf( buf, "Shutdown by %s.", ch->name );
    append_file( ch, SHUTDOWN_FILE, buf );
    safe_strcat( MSL, buf, "\n\r" );
    do_echo( ch, buf );
    merc_down = TRUE;
    return;
}

void do_snoop( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    DESCRIPTOR_DATA *d;
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Snoop whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim->desc == NULL )
    {
        send_to_char( "No descriptor to snoop.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Cancelling all snoops.\n\r", ch );
        for ( d = first_desc; d != NULL; d = d->next )
        {
            if ( d->snoop_by == ch->desc )
                d->snoop_by = NULL;
        }
        return;
    }

    if ( victim->desc->snoop_by != NULL )
    {
        send_to_char( "Busy already.\n\r", ch );
        return;
    }

    if ( get_trust(ch) < 85 && (!IS_IMMORTAL(victim) && !IS_NEWBIE(victim) ) )
    {
        send_to_char( "You are only able to snoop newbies or other immortals.\n\r", ch );
        return;
    }
    if ( get_trust( victim ) > get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if ( ch->desc != NULL )
    {
        for ( d = ch->desc->snoop_by; d != NULL; d = d->snoop_by )
        {
            if ( d->character == victim || d->original == victim )
            {
                send_to_char( "No snoop loops.\n\r", ch );
                return;
            }
        }
    }

    victim->desc->snoop_by = ch->desc;
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_switch( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Switch into whom?\n\r", ch );
        return;
    }

    if ( ch->desc == NULL )
        return;

    if ( ch->desc->original != NULL )
    {
        send_to_char( "You are already switched.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Ok.\n\r", ch );
        return;
    }

    /*
     * Pointed out by Da Pub (What Mud)
     */
    if ( !IS_NPC(victim) )
    {
        send_to_char( "You cannot switch into a player!\n\r", ch );
        return;
    }

    if ( victim->desc != NULL )
    {
        send_to_char( "Character in use.\n\r", ch );
        return;
    }

    ch->desc->character = victim;
    ch->desc->original  = ch;
    victim->desc        = ch->desc;
    ch->desc            = NULL;
    send_to_char( "Ok.\n\r", victim );
    return;
}

void do_return( CHAR_DATA *ch, char *argument )
{
    if ( ch->desc == NULL )
        return;

    if ( ch->desc->original == NULL )
    {
        send_to_char( "You aren't switched.\n\r", ch );
        return;
    }

    send_to_char( "You return to your original body.\n\r", ch );
    ch->desc->character       = ch->desc->original;
    ch->desc->original        = NULL;
    ch->desc->character->desc = ch->desc;
    ch->desc                  = NULL;
    return;
}

void do_oload( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex = NULL;
    OBJ_DATA *obj;
    bool search = FALSE;

    argument = one_argument( argument, arg1 );

    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: oload <vnum> <level>.\n\r", ch );
        return;
    }
    if ( !is_number(arg1) )
        search = TRUE;

    if ( !search )
    {
        if ( ( pObjIndex = get_obj_index( atoi( arg1 ) ) ) == NULL )
        {
            send_to_char( "No object has that vnum.\n\r", ch );
            return;
        }
    }
    else
    {
        extern int top_obj_index;
        int vnum,nMatch=0;
        bool found = FALSE;
        for ( vnum = 0; nMatch < top_obj_index; vnum++ )
        {
            if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
            {
                nMatch++;
                if ( is_name( arg1, pObjIndex->name ) )
                {
                    found = TRUE;
                    break;
                }
            }
        }
        if ( !found )
        {
            send_to_char( "Object not found.\n\r", ch );
            return;
        }
    }
    if ( pObjIndex == NULL )
    {
        send_to_char( "* Object not found.\n\r", ch );
        return;
    }

    obj = create_object( pObjIndex, 0 );
    if ( CAN_WEAR(obj, ITEM_TAKE) )
    {
        act( "$n @@mgestures majestically, and@@N $p @@mappears with a crash of @@WTHUNDER!!@@N", ch, obj, NULL, TO_ROOM );
        obj_to_char( obj, ch );
    }
    else
    {
        obj_to_room( obj, ch->in_room );
        act( "$n @@mgestures, and a @@N$p@@M appears with a thunderous crash@@N!!!", ch, obj, NULL, TO_ROOM );
    }
    act( "You create $p!", ch, obj, NULL, TO_CHAR );
    move_obj(obj,ch->x,ch->y,ch->z);
    return;
}

void do_apurge( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *room;
    int roomnum,max_vnum;

    roomnum = ch->in_room->area->min_vnum;

    max_vnum = ch->in_room->area->max_vnum;

    // Let us start at the first room, then continue throughout the area
    for(;roomnum < max_vnum;roomnum++)
    {
        room = get_room_index(roomnum);
        if (room != NULL)
            purge_room( ch, room );
        room = room->next;
    }

    send_to_char("You purge the entire area!\n\r",ch);

    return;
}

void do_wpurge( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *room;
    int roomnum,max_vnum;

    roomnum = first_area->min_vnum;

    max_vnum = last_area->max_vnum;

    // Let us start at the first room, then continue throughout the area
    for(;roomnum < max_vnum;roomnum++)
    {
        room = get_room_index(roomnum);
        if (room != NULL)
            purge_room( ch, room );
        room = room->next;
    }

    send_to_char("You purge the entire realm!\n\r",ch);

    return;
}

void purge_room( CHAR_DATA *ch, ROOM_INDEX_DATA *room )
{
    OBJ_DATA *obj;
    OBJ_DATA  *obj_next;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    for ( obj = map_obj[ch->x][ch->y]; obj != NULL; obj = obj_next )
    {
        obj_next = obj->next_in_room;
        if ( !NOT_IN_ROOM(obj,ch) && obj->carried_by == NULL )
            extract_obj( obj );
    }

    return;
}

void do_purge( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    VEHICLE_DATA *vhc;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        /* 'purge' */
        OBJ_DATA  *obj_next;
        VEHICLE_DATA *vhc_next;

        for ( obj = map_obj[ch->x][ch->y]; obj != NULL; obj = obj_next )
        {
            obj_next = obj->next_in_room;
            if ( NOT_IN_ROOM(obj,ch)  || obj->carried_by != NULL )
                continue;
            extract_obj( obj );
        }
        for ( vhc = map_vhc[ch->x][ch->y][ch->z];vhc;vhc = vhc_next )
        {
            vhc_next = vhc->next_in_room;
            if ( vhc->driving )
            {
                vhc->driving->in_vehicle = NULL;
                vhc->driving = NULL;
            }
            extract_vehicle(vhc,FALSE);
        }

        act( "$n cleanses the room with Holy fire!", ch, NULL, NULL, TO_ROOM);
        send_to_char( "Your burst of Holy fire cleanses the room!\n\r", ch );
        return;
    }

    else if ( ( obj = get_obj_world( ch, arg ) ) != NULL )
    {
        act( "$n obliterates $p with Holy fire!", ch, obj, NULL, TO_ROOM );
        act( "You obliterate $p with Holy fire!", ch, obj, NULL, TO_CHAR );
        extract_obj( obj );
    }
    else if ( ( vhc = get_vehicle_world( arg ) ) != NULL )
    {
        extract_vehicle( vhc, TRUE );
    }
    else
    {
        send_to_char( "You can't find it.\n\r", ch );
    }
    save_objects(0);
    return;
}

void do_trust( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int level;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number( arg2 ) )
    {
        send_to_char( "Syntax: trust <char> <level>.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg1 ) ) == NULL )
    {
        send_to_char( "That player is not here.\n\r", ch);
        return;
    }

    if ( victim == ch )
    {
        send_to_char( "Not on yourself!\n\r", ch );
        return;
    }

    if ( ( level = atoi( arg2 ) ) < 0 || level > MAX_LEVEL )
    {
        sprintf( buf, "Level must be 0 (reset) or 1 to %d.\n\r", MAX_LEVEL );
        send_to_char( buf, ch );
        return;
    }

    if ( level > get_trust( ch ) )
    {
        send_to_char( "Limited to your trust.\n\r", ch );
        return;
    }

    victim->trust = level;
    return;
}

void do_restore( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "Restore whom?\n\r", ch );
        return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        /* then loop through all players and restore them */
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        for ( vch = first_char; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;
            if ( !IS_NPC( vch ) )
            {
                if ( IS_IMMORTAL( vch ) && ( vch != ch ) )
                {
                    act( "Everyone has been restored by $n." ,ch,NULL,vch,TO_VICT);
                }
                else
                {
                    vch->hit  = vch->max_hit;
                    update_pos( vch );
                    act( "$n kindly restores you.", ch, NULL, vch, TO_VICT );
                }
            }
        }

        send_to_char ( "Everyone has been restored.\n\r", ch);
        return;
    }

    if ( (victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here!\n\r", ch );
        return;
    }

    victim->hit  = victim->max_hit;
    update_pos( victim );
    act( "$n kindly restores you.", ch, NULL, victim, TO_VICT );
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_freeze( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Freeze whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET(victim->act, PLR_FREEZE) )
    {
        REMOVE_BIT(victim->act, PLR_FREEZE);
        send_to_char( "You can play again.\n\r", victim );
        send_to_char( "FREEZE removed.\n\r", ch );
    }
    else
    {
        SET_BIT(victim->act, PLR_FREEZE);
        send_to_char( "You can't do ANYthing!\n\r", victim );
        sprintf( buf, "You have been FROZEN by %s!!\n\r", ch->name );
        send_to_char( buf, victim );
        send_to_char( "Freeze set.\n\r", ch );

        sprintf( buf, "%s has been FROZEN by %s.\n\r",
            victim->name, ch->name );
        info( buf, get_trust(ch) + 1 );
    }

    save_char_obj( victim );

    return;
}


void do_noemote( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Noemote whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET(victim->act, PLR_NO_EMOTE) )
    {
        REMOVE_BIT(victim->act, PLR_NO_EMOTE);
        send_to_char( "You can emote again.\n\r", victim );
        send_to_char( "NO_EMOTE removed.\n\r", ch );
    }
    else
    {
        SET_BIT(victim->act, PLR_NO_EMOTE);
        send_to_char( "Your ability to emote has been removed!\n\r", victim );
        send_to_char( "NO_EMOTE set.\n\r", ch );
    }

    return;
}

void do_notell( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Notell whom?", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET(victim->act, PLR_NO_TELL) )
    {
        REMOVE_BIT(victim->act, PLR_NO_TELL);
        send_to_char( "You can tell again.\n\r", victim );
        send_to_char( "NO_TELL removed.\n\r", ch );
    }
    else
    {
        SET_BIT(victim->act, PLR_NO_TELL);
        send_to_char( "You now can not use the tell command!\n\r", victim );
        send_to_char( "NO_TELL set.\n\r", ch );
    }

    return;
}

void do_silence( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Silence whom?", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET(victim->act, PLR_SILENCE) )
    {
        REMOVE_BIT(victim->act, PLR_SILENCE);
        send_to_char( "You can use channels again.\n\r", victim );
        send_to_char( "SILENCE removed.\n\r", ch );
    }
    else
    {
        SET_BIT(victim->act, PLR_SILENCE);
        send_to_char( "You can't use channels!\n\r", victim );
        send_to_char( "SILENCE set.\n\r", ch );
    }

    return;
}

void do_ban( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    char arg2[MSL];
    char buf2[MSL];

    BAN_DATA *pban;
    buf[0] = '\0';
    buf2[0] = '\0';

    if ( IS_NPC(ch) )
        return;

    argument = one_argument( argument, arg );
    one_argument( argument, arg2 );

    if ( arg[0] == '\0' )
    {
        strcpy( buf, "Banned sites:\n\r" );
        for ( pban = first_ban; pban != NULL; pban = pban->next )
        {
            safe_strcat( MSL, buf, pban->name );
            sprintf( buf2, ( pban->newbie ? " Newbies" : " All" ) );
            safe_strcat( MSL, buf, buf2 );
            sprintf( buf2, "  Banned by: %s (%s)", pban->banned_by, pban->note );
            safe_strcat( MSL, buf, buf2 );
            safe_strcat( MSL, buf, "\n\r" );
        }
        send_to_char( buf, ch );
        return;
    }

    for ( pban = first_ban; pban != NULL; pban = pban->next )
    {
        if ( !str_cmp( arg, pban->name ) )
        {
            send_to_char( "That site is already banned!\n\r", ch );
            return;
        }
    }

    GET_FREE(pban, ban_free);
    if ( !str_cmp( arg2, "newbie" ) )
        pban->newbie = TRUE;
    else
        pban->newbie = FALSE;

    pban->name  = str_dup( arg );
    pban->banned_by = str_dup( ch->name );
    pban->note = str_dup(argument);
    LINK(pban, first_ban, last_ban, next, prev);
    save_bans( );
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_allow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    BAN_DATA *curr;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Remove which site from the ban list?\n\r", ch );
        return;
    }

    for ( curr = first_ban; curr != NULL; curr = curr->next )
    {
        if ( !str_cmp( arg, curr->name ) )
        {
            UNLINK(curr, first_ban, last_ban, next, prev);

            PUT_FREE(curr, ban_free);
            send_to_char( "Ok.\n\r", ch );
            save_bans( );
            return;
        }
    }

    send_to_char( "Site is not banned.\n\r", ch );
    return;
}

void do_wizlock( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    extern bool wizlock;
    wizlock = !wizlock;
    sysdata.w_lock = wizlock;
    save_sysdata( );

    if ( wizlock )
    {
        send_to_char( "Game wizlocked.\n\r", ch );
        sprintf( buf, "%s wizlocks ACK! Mud.\n\r", ch->name );
    }
    else
    {
        send_to_char( "Game un-wizlocked.\n\r", ch );
        sprintf( buf, "%s un-wizlocks ACK! Mud.\n\r", ch->name );
    }
    info( buf, get_trust( ch ) );
    return;
}

void do_paintlock( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_INPUT_LENGTH];
    extern bool paintlock;
    paintlock = !paintlock;
    sysdata.p_lock = paintlock;
    save_sysdata( );

    if ( paintlock )
    {
        send_to_char( "Paintball Arena Locked.\n\r", ch );
        sprintf( buf, "%s has locked the Paintball Arena.\n\r", ch->name );
    }
    else
    {
        send_to_char( "Paintball Arena Un-Locked.\n\r", ch );
        sprintf( buf, "%s has un-locked the Paintball Arena.\n\r", ch->name );
    }
    info( buf, get_trust( ch ) );
    return;
}

void do_mset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char arg4 [MAX_INPUT_LENGTH];
    char buf  [MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int value;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    if ( str_cmp( arg1, "title" ) )
    {
        argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );
    }
    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char( "\n\r", ch );
        send_to_char( "        @@gMset (@@aTarget@@g) (@@aField@@g) (@@aValue@@g)@@N\n\r",     ch );
        send_to_char( "\n\r", ch );
        send_to_char( "@@c+--------------- @@dNormal Fields@@c ---------------+@@N\n\r",     ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c| @@gSex, Class, HP, Flags, Location, Disease    @@c|@@N\n\r",     ch );
        send_to_char( "@@c| @@gAlliance, Kills, Deaths, Bkills             @@c|@@N\n\r",     ch );
        send_to_char( "@@c| @@gDisease, medaltimer                         @@c|@@N\n\r",     ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c+-------------- @@dString Fields@@c ----------------+@@N\n\r",  ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c| @@gTitle                                       @@c|@@N\n\r",  ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c+---------------------------------------------+@@N\n\r",     ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = is_number( arg3 ) ? atoi( arg3 ) : -1;

    if ( !str_cmp( arg2, "sex" ) )
    {
        if ( value < 1 || value > 2 )
        {
            send_to_char( "Sex range is 1 to 2.\n\r", ch );
            return;
        }
        victim->sex = value;
        victim->login_sex = value;
        return;
    }
    if ( !str_cmp( arg2, "time" ) )
    {
        if ( value < 0 )
            return;
        victim->played_tot = value*3600;
        return;
    }
    if ( !str_cmp( arg2, "kills" ) )
    {
        if ( value < 0 )
        {
            send_to_char( "At least 0.\n\r", ch );
            return;
        }
        //	victim->pcdata->pkills = value;
        victim->pcdata->tpkills = value;
        return;
    }
    if ( !str_cmp(arg2,"normal") )
    {
        if ( IS_SET(victim->pcdata->pflags,PLR_BASIC) )
            REMOVE_BIT(victim->pcdata->pflags,PLR_BASIC) ;
    }
    if ( !str_cmp( arg2, "bkills" ) )
    {
        if ( value < 0 )
        {
            send_to_char( "At least 0.\n\r", ch );
            return;
        }
        //	victim->pcdata->bkills = value;
        victim->pcdata->tbkills = value;
        return;
    }
    if ( !str_cmp( arg2, "deaths" ) )
    {
        if ( value < 0 )
        {
            send_to_char( "At least 0.\n\r", ch );
            return;
        }
        victim->pcdata->deaths = value;
        return;
    }
    if ( !str_cmp( arg2, "disease" ) )
    {
        if ( value < 0 )
        {
            send_to_char( "At least 0.\n\r", ch );
            return;
        }
        victim->disease = value;
        return;
    }

    if ( !str_cmp( arg2, "class" ) )
    {
        if ( value < 0 || value >= MAX_CLASS )
        {
            char buf[MAX_STRING_LENGTH];

            sprintf( buf, "Class range is 0 to %d.\n", MAX_CLASS-1 );
            send_to_char( buf, ch );
            return;
        }
        victim->class = value;
        return;
    }

    if ( !str_cmp( arg2, "hp" ) )
    {
        if ( value < -10 || value > 30000 )
        {
            send_to_char( "Hp range is -10 to 30,000 hit points.\n\r", ch );
            return;
        }
        victim->max_hit = value;
        victim->pcdata->hp_from_gain = value;
        return;
    }

    if ( !str_cmp(arg2, "medaltimer") )
    {
	if ( value < 0 || value > 1440 )
	{
	    send_to_char( "Medal timer must be between 0 and 1440 minutes.\n\r", ch );
	    return;
	}
    victim->medaltimer = value;
    return;
    }

    if ( !str_cmp(arg2, "flags" ) )
    {
        int neg=0;
        char * lookupstr=arg3;

        if (get_trust(ch) < MAX_LEVEL-1)
        {
            send_to_char("Only supreme or creator level immortals may use this.\n\r",ch);
            return;
        }

        if (lookupstr[0]=='-')
        {
            neg=1; lookupstr++;
        }
        if (lookupstr[0]=='+')
            lookupstr++;

        value=table_lookup(tab_player_flags,lookupstr);
        if (value<1)
        {
            sprintf(buf,"Valid player flags are :\n\r");
            table_printout(tab_player_flags,buf+strlen(buf));
            send_to_char(buf,ch);
            return;
        }

        if (neg)
            REMOVE_BIT(victim->pcdata->pflags,value);
        else
            SET_BIT(victim->pcdata->pflags,value);
        return;
    }
    if ( !str_cmp( arg2, "title" ) )
    {
        if ( get_trust(ch) < 85 )
        {
            send_to_char( "This option only available to Creators.\n\r", ch );
            return;
        }

        set_title( victim, argument );
        return;
    }

    if ( !str_cmp( arg2, "location") )
    {
        move( victim, is_number( arg3 ) ? atoi( arg3 ) : 0, is_number( arg4 ) ? atoi( arg4 ) : 0, victim->z );
        return;
    }
    if ( !str_cmp( arg2, "alliance") )
    {
        value = is_number(arg3)?atoi(arg3):-1;
        if ( alliance_table[value].name == NULL && value != -1 )
        {
            send_to_char( "Invalid alliance.\n\r", ch );
            return;
        }
        if ( victim->pcdata->alliance != -1 )
            alliance_table[victim->pcdata->alliance].members--;
        victim->pcdata->alliance = value;
        if ( victim->pcdata->alliance != -1 )
            alliance_table[victim->pcdata->alliance].members++;
        return;
    }
    if ( !str_cmp( arg2, "z") )
    {
        if (( atoi(arg3) < 0 || atoi(arg3) >= Z_MAX ) )
        {
            send_to_char( "Invalid Value.\n\r", ch );
            return;
        }

        victim->z = is_number(arg3) ? atoi(arg3) : 1;
        if ( victim->in_vehicle )
            victim->in_vehicle->z = victim->z;
        return;
    }
    /*
     * Generate usage message.
     */
    do_mset( ch, "" );
    return;
}

void do_oset( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf [MAX_STRING_LENGTH];
    OBJ_DATA *obj;
    int value;
    int num;
    char *argn;

    smash_tilde( argument );
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    strcpy( arg3, argument );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg3[0] == '\0' )
    {
        send_to_char( "Syntax: oset <object> <field>  <value>\n\r",     ch );
        send_to_char( "or:     oset <object> <string> <value>\n\r",     ch );
        send_to_char( "\n\r",                                           ch );
        send_to_char( "Field being one of:\n\r",                        ch );
        send_to_char( "  value0 value1 value2 value3 [v0,v1,v2,v3]\n\r",ch );
        send_to_char( "  extra wear level weight cost timer condition\n\r", ch );
        send_to_char( "\n\r",                                           ch );
        send_to_char( "String being one of:\n\r",                       ch );
        send_to_char( "  name short long   \n\r",                       ch );
        send_to_char( "To delete objfuns, use - as value\n\r",      ch );
        return;
    }

    if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
        if ( ( obj = get_obj_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
        return;
    }

    /*
     * Snarf the value (which need not be numeric).
     */
    value = atoi( arg3 );

    /*
     * Set something.
     */
    if ( !str_prefix( "v", arg2 ) )
    {
        int indexer = -1;
        char numbuf[10];
        sprintf( numbuf, "%c", arg2[1] );
        if ( is_number( numbuf ) )
        {
            indexer = atoi( numbuf );
        }
        else
        {
            sprintf( numbuf, "%c", arg1[5] );
            if ( is_number( numbuf ) )
            {
                indexer = atoi( numbuf );
            }
        }
        if (  ( indexer <0 ) || ( indexer > 9 )  )
        {
            send_to_char( "Value numbers are 0 - 9.\n\r", ch );
            return;
        }
        obj->value[indexer] = value;
        return;
    }

    if ( !str_cmp( arg2, "extra" ) )
    {
        num=1;
        argn=arg3;
        if (argn[0]=='+')
        {
            num=1;
            argn++;
        }
        if (argn[0]=='-')
        {
            num=0;
            argn++;
        }
        value=table_lookup(tab_obj_flags,argn);
        if (value==0)
        {
            sprintf(buf,"Values for extra flags are +/- :\n\r");
            wide_table_printout(tab_obj_flags,buf+strlen(buf));
            send_to_char(buf,ch);
            return;
        }
        if ( !ok_to_use( ch, value ) )
            return;

        if (num==1)
            SET_BIT(obj->extra_flags,value);
        else
            REMOVE_BIT(obj->extra_flags,value);
        return;
    }

    if ( !str_cmp( arg2, "wear" ) )
    {
        num=1;
        argn=arg3;
        if (argn[0]=='+')
        {
            num=1;
            argn++;
        }
        if (argn[0]=='-')
        {
            num=0;
            argn++;
        }
        value=table_lookup(tab_wear_flags,argn);
        if (value==0)
        {
            sprintf(buf,"Values for wear flags are +/- :\n\r");
            wide_table_printout(tab_wear_flags,buf+strlen(buf));
            send_to_char(buf,ch);
            return;
        }
        if ( !ok_to_use( ch, value ) )
            return;

        if (num==1)
            SET_BIT(obj->wear_flags,value);
        else
            REMOVE_BIT(obj->wear_flags,value);
        return;
    }

    if ( !str_cmp( arg2, "level" ) )
    {
        if ( get_trust(ch) < 85 )
            return;

        obj->level = value;
        return;
    }

    if ( !str_cmp( arg2, "weight" ) )
    {
        obj->weight = value;
        return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
        free_string( obj->name );
        obj->name = str_dup( arg3 );
        return;
    }

    if ( !str_cmp( arg2, "short" ) )
    {
        free_string( obj->short_descr );
        obj->short_descr = str_dup( arg3 );
        return;
    }

    if ( !str_cmp( arg2, "long" ) )
    {
        free_string( obj->description );
        obj->description = str_dup( arg3 );
        return;
    }

    /*
     * Generate usage message.
     */
    do_oset( ch, "" );
    return;
}

void do_users( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char buf3[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int count;

    count       = 0;
    buf[0]      = '\0';
    buf2[0]     = '\0';

    send_to_char( "\n\r Desc.  Connection State.   Player Name.     Login Site.                   Size Of", ch );
    if ( get_trust( ch ) >= 85 )
        send_to_char("                 Port.\n\r", ch );
    else
        send_to_char( "\n\r", ch );

    for ( d = first_desc; d != NULL; d = d->next )
    {
        if ( d->character != NULL )
        {
            count++;
            switch ( d->connected )
            {
                case CON_PLAYING:
                    sprintf( buf3, "%s", "Playing         "); break;
                case CON_GET_NAME:
                    sprintf( buf3, "%s", "Get Name        "); break;
                case CON_GET_OLD_PASSWORD:
                    sprintf( buf3, "%s", "Get Old Passwd  "); break;
                case CON_CONFIRM_NEW_NAME:
                    sprintf( buf3, "%s", "Cnrm New Name   "); break;
                case CON_GET_NEW_PASSWORD:
                    sprintf( buf3, "%s", "Get New Passwd  "); break;
                case CON_CONFIRM_NEW_PASSWORD:
                    sprintf( buf3, "%s", "Cnfm New Passwd "); break;
                case CON_READ_MOTD:
                    sprintf( buf3, "%s", "Reading MOTD    "); break;
                case CON_READ_RULES:
                    sprintf( buf3, "%s", "Reading Rules   "); break;
                case CON_FINISHED:
                    sprintf( buf3, "%s", "Finished        "); break;
                case CON_MENU:
                    sprintf( buf3, "%s", "Menu            "); break;
                case CON_COPYOVER_RECOVER:
                    sprintf( buf3, "%s", "Copyover Recover"); break;
                case CON_QUITTING:
                    sprintf( buf3, "%s", "Quitting        "); break;
                case CON_RECONNECTING:
                    sprintf( buf3, "%s", "Reconnecting    "); break;
                case CON_SETTING_STATS:
                    sprintf( buf3, "%s", "Setting Stats   "); break;
                case CON_GET_NEW_CLASS:
                    sprintf( buf3," %s", "Setting Class   "); break;
                case CON_GET_ANSI:
                    sprintf( buf3," %s", "Setting Color   "); break;
                case CON_GET_SEX:
                    sprintf( buf3," %s", "Setting Sex     "); break;
                case CON_GET_RECREATION:
                    sprintf( buf3," %s", "Recreating      "); break;
                case CON_GET_BONUS:
                    sprintf( buf3," %s", "Selecting Bonus "); break;
                case CON_GET_NEW_PLANET:
                    sprintf( buf3," %s", "Selecting Grid  "); break;
                case CON_GET_NEW_MODE:
                    sprintf( buf3," %s", "Selecting Mode  "); break;
                default:
                    sprintf( buf3, "%s", "Unknown...      "); break;
            }

            sprintf( buf + strlen(buf), "@@G[@@r%3d %-21s@@G] @@y%-12s @@r%-30s@@N",
                d->descriptor,
                buf3,
                d->original  ? d->original->name  :
            d->character ? d->character->name : "(none)",
                d->host
                );

            if ( get_trust( ch ) >= 85 )
                sprintf( buf + strlen(buf), "  %5d\n\r", d->remote_port );
            else
                sprintf( buf + strlen(buf), "\n\r" );
        }
        else
        {
            sprintf( buf + strlen(buf), "@@G[@@r%3d Connecting           @@G] @@y(none)       @@r%-30s@@N",
                d->descriptor, d->host );

            if ( get_trust( ch ) >= 85 )
                sprintf( buf + strlen(buf), "  %5d\n\r", d->remote_port );
            else
                sprintf( buf + strlen(buf), "\n\r" );
        }
    }

    sprintf( buf2, "%d user%s\n\r", count, count == 1 ? "" : "s" );
    safe_strcat( MSL, buf, buf2 );
    sprintf( buf2, "%s%s%s", color_string( ch, "stats" ), buf,
        color_string( ch, "normal" ) );
    send_to_char( buf2, ch );
    return;
}

/*
 * Thanks to Grodyn for pointing out bugs in this function.
 */
void do_force( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int trust;
    int cmd;

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char( "Force whom to do what?\n\r", ch );
        return;
    }

    /*
     * Look for command in command table.
     */
    trust = get_trust( ch );
    for ( cmd = 0; cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( argument[0] == cmd_table[cmd].name[0]
            &&   !str_prefix( argument, cmd_table[cmd].name )
            &&   ( cmd_table[cmd].level > trust
            && cmd_table[cmd].level != 41 ) )
        {
            send_to_char( "You can't even do that yourself!\n\r", ch );
            return;
        }
    }

    /*
     * Allow force to be used on ALL mobs....
     * Only highest level players to use this... it can cause trouble!!!
     * Good for mob "invasions"
     * This could get interesting ;)
     * -- Stephen
     */

    if ( !str_cmp( arg, "everymob" ) )
    {

        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        if ( ch->level < MAX_LEVEL )
        {
            send_to_char( "This option is only available to true Gods.\n\r", ch );
            return;
        }

        for ( vch = first_char; vch != NULL; vch = vch=vch_next )
        {

            vch_next = vch->next;

            if ( IS_NPC( vch ) )
            {
                interpret( vch, argument );
            }
        }
        return;
    }

    /* Like above but for mobs in same area as ch */

    if ( !str_cmp( arg, "localmobs" ) )

    {
        CHAR_DATA *vim;
        CHAR_DATA *vim_next;

        for ( vim = first_char; vim != NULL; vim = vim=vim_next )
        {

            vim_next = vim->next;

            if ( IS_NPC( vim )
                && ( vim->in_room->area == ch->in_room->area ) )
            {
                interpret( vim, argument );
            }
        }
        return;
    }

    if ( !str_cmp( arg, "all" ) )
    {
        CHAR_DATA *vch;
        CHAR_DATA *vch_next;

        for ( vch = first_char; vch != NULL; vch = vch_next )
        {
            vch_next = vch->next;

            if ( !IS_NPC(vch) && !IS_IMMORTAL( vch ) )
            {
                act( "$n forces you to '$t'.", ch, argument, vch, TO_VICT );
                interpret( vch, argument );
            }
        }
    }
    else
    {
        CHAR_DATA *victim;

        if ( ( victim = get_char_world( ch, arg ) ) == NULL )
        {
            send_to_char( "They aren't here.\n\r", ch );
            return;
        }

        if ( victim == ch )
        {
            send_to_char( "Aye aye, right away!\n\r", ch );
            return;
        }

        if ( get_trust( victim ) >= get_trust( ch ) )
        {
            send_to_char( "Do it yourself!\n\r", ch );
            return;
        }

        act( "$n forces you to '$t'.", ch, argument, victim, TO_VICT );
        interpret( victim, argument );
    }

    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_imminfo( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: imminfo [string]\r\n", ch);
        return;
    }

    info(argument, 0);
}

/*
 * New routines by Dionysos.
 */
void do_invis( CHAR_DATA *ch, char *argument )
{

    sh_int level;
    char buf[MAX_STRING_LENGTH];

    level = -1;

    if ( argument[0] != '\0' )
        /* Then we have a level argument */
    {
        if ( !is_number( argument ) )
        {
            level = get_trust( ch );
        }
        level = UMAX( 1, atoi( argument ) );
        level = UMIN( get_trust(ch), level );

        if (IS_SET(ch->act, PLR_WIZINVIS) )
        {
            ch->invis = level;
            sprintf( buf, "Wizinvis changed to level: %d\n\r", level );
            send_to_char( buf, ch );
            return;
        }

    }

    if ( level == -1 )
        level = get_trust( ch );

    ch->invis = level;

    if ( IS_NPC(ch) )
        return;

    if ( IS_SET(ch->act, PLR_INCOG) )
    {
        do_incog(ch,"");
    }

    if ( IS_SET(ch->act, PLR_WIZINVIS) )
    {
        REMOVE_BIT(ch->act, PLR_WIZINVIS);
        act( "Small, dazzling spots of light focus into the shape of $n!", ch, NULL, NULL, TO_ROOM );

        send_to_char( "Your body becomes solid again.\n\r", ch );
    }
    else
    {
        SET_BIT(ch->act, PLR_WIZINVIS);
        if ( get_trust(ch) < 90 )
            act( "$n dissolves into a storm of dazzling points of light!", ch, NULL, NULL, TO_ROOM );
        send_to_char( "You slowly vanish into thin air.\n\r", ch );
        sprintf( buf, "Setting Wizinvis to level: %d.\n\r", level );
        send_to_char( buf, ch );
    }

    return;
}

void do_incog( CHAR_DATA *ch, char *argument )
{

    sh_int level;
    char buf[MAX_STRING_LENGTH];

    level = -1;

    if ( argument[0] != '\0' )
        /* Then we have a level argument */
    {
        if ( !is_number( argument ) )
        {
            level = get_trust( ch );
        }
        level = UMAX( 1, atoi( argument ) );
        level = UMIN( ch->level, level );

        if (IS_SET(ch->act, PLR_INCOG) )
        {
            ch->incog = level;
            sprintf( buf, "Incog changed to level: %d\n\r", level );
            send_to_char( buf, ch );
            return;
        }

    }

    if ( level == -1 )
        level = get_trust( ch );

    ch->incog = level;

    if ( IS_NPC(ch) )
        return;

    if ( IS_SET(ch->act, PLR_WIZINVIS))
    {
        do_invis(ch,"");
    }

    if ( IS_SET(ch->act, PLR_INCOG) )
    {
        REMOVE_BIT(ch->act, PLR_INCOG);
        act( "Small, dazzling spots of light focus into the shape of $n!", ch, NULL, NULL, TO_ROOM );
        send_to_char( "Your body becomes solid to the world.\n\r", ch );
    }
    else
    {
        SET_BIT(ch->act, PLR_INCOG);
        act( "$n dissolves from the world!", ch, NULL, NULL, TO_ROOM );
        send_to_char( "You slowly vanish from the world.\n\r", ch );
        sprintf( buf, "Setting Incog to level: %d.\n\r", level );
        send_to_char( buf, ch );
    }

    return;
}

void do_holylight( CHAR_DATA *ch, char *argument )
{
    if ( IS_NPC(ch) )
        return;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
    {
        REMOVE_BIT(ch->act, PLR_HOLYLIGHT);
        send_to_char( "Holy light mode off.\n\r", ch );
    }
    else
    {
        SET_BIT(ch->act, PLR_HOLYLIGHT);
        send_to_char( "Holy light mode on.\n\r", ch );
    }

    return;
}

/* Wizify and Wizbit sent in by M. B. King */

void do_wizify( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1  );
    if ( arg1[0] == '\0' )
    {
        send_to_char( "Syntax: wizify <name>\n\r" , ch );
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r" , ch );
        return;
    }
    if ( IS_NPC( victim ) )
    {
        send_to_char( "Not on mobs.\n\r", ch );
        return;
    }
    victim->wizbit = !victim->wizbit;
    if ( victim->wizbit )
    {
        act( "$N wizified.\n\r", ch, NULL, victim, TO_CHAR );
        act( "$n has wizified you!\n\r", ch, NULL, victim, TO_VICT );
    }
    else
    {
        act( "$N dewizzed.\n\r", ch, NULL, victim, TO_CHAR );
        act( "$n has dewizzed you!\n\r", ch, NULL, victim, TO_VICT );
    }

    do_save( victim, "");
    return;
}

/* Idea from Talen of Vego's do_where command */

void do_owhere( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char catbuf[MSL];
    char arg[MAX_INPUT_LENGTH];
    bool found = FALSE;
    OBJ_DATA *obj;
    int obj_counter = 1;
    bool mailme = FALSE;
    if ( is_name( "mailme", argument ) )
        mailme = TRUE;
    one_argument( argument, arg );
    sprintf( buf, "Output for Owhere %s", arg );
    if( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  owhere <object>.\n\r", ch );
        return;
    }
    else
    {
        for ( obj = first_obj; obj != NULL; obj = obj->next )
        {
            if ( !is_name( arg, obj->name ) )
                continue;
            found = TRUE;

            if ( obj->carried_by != NULL )
            {
                sprintf( catbuf, "[%2d] %s carried by %s [At:%d/%d].\n\r",
                    obj_counter,
                    obj->short_descr,
                    PERS( obj->carried_by, ch ),
                    obj->carried_by->x, obj->carried_by->y );
            }
            else
            {
                sprintf( catbuf, "[%2d] %s%s at %d/%d.\n\r",
                    obj_counter,
                    obj->quest_timer > 0 ? "@@R(@@eQ@@R)@@N " : "",
                    obj->short_descr,
                    obj->x, obj->y );
            }

            obj_counter++;
            buf[0] = UPPER( buf[0] );
            safe_strcat( MSL, buf, catbuf );
        }
    }

    if ( !found )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r" , ch );
        return;
    }

    sprintf( catbuf, "Owhere report for %s [%d items found]", arg, obj_counter );
    send_rep_out( ch, buf, mailme, catbuf  );
    return;
}

void do_owhereflag( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char catbuf[MSL];
    char arg[MAX_INPUT_LENGTH];
    bool found = FALSE;
    OBJ_DATA *obj;
    int obj_counter = 1;
    bool mailme = FALSE;
    int flag = 0;

    one_argument( argument, arg );
    flag = multi_table_lookup(tab_obj_flags,arg);
    if ( flag == 0 )
    {
        sprintf(buf,"Values for extra flags are +/- :\n\r");
        table_printout(tab_obj_flags,buf+strlen(buf));
        send_to_char(buf,ch);
        return;
    }
    sprintf( buf, "Output for Owhereflag %s", arg );
    if( arg[0] == '\0' )
    {
        send_to_char( "Syntax:  owhereflag <flag>.\n\r", ch );
        return;
    }
    else
    {
        for ( obj = first_obj; obj != NULL; obj = obj->next )
        {
            //        if ( !str_infix ( arg, bit_table_lookup( tab_obj_flags, obj->extra_flags ) ) )
            if ( !IS_SET(obj->extra_flags,flag) )
                continue;
            found = TRUE;

            if ( obj->carried_by != NULL )
            {
                sprintf( catbuf, "[%2d] %s carried by %s [At:%d/%d].\n\r",
                    obj_counter,
                    obj->short_descr,
                    PERS( obj->carried_by, ch ),
                    obj->carried_by->x, obj->carried_by->y );
            }
            else
            {
                sprintf( catbuf, "[%2d] %s%s at %d/%d.\n\r",
                    obj_counter,
                    obj->quest_timer > 0 ? "@@R(@@eQ@@R)@@N " : "",
                    obj->short_descr,
                    obj->x, obj->y );
            }

            obj_counter++;
            buf[0] = UPPER( buf[0] );
            safe_strcat( MSL, buf, catbuf );
        }
    }

    if ( !found )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r" , ch );
        return;
    }

    sprintf( catbuf, "Owhereflag report for %s [%d items found]", arg, obj_counter );
    send_rep_out( ch, buf, mailme, catbuf  );
    return;
}

void do_resetpassword( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char *pwdnew;

    if ( IS_NPC(ch) )
        return;

    argument=one_argument( argument, arg1 );
    argument=one_argument( argument, arg2 );

    victim = get_char_world(ch, arg1);

    if( victim == '\0' )
    {
        send_to_char( "This character is not playing at this time\n\r", ch);
        return;
    }
    if ( get_trust(ch) <= get_trust(victim))
    {
        send_to_char( "You cannot change their password!\n\r",ch);
        return;
    }
    if ( IS_NPC( victim ) )
    {
        send_to_char( "You cannot change the password of NPCs!\n\r",ch);
        return;
    }

    if (  ( ch->pcdata->pwd != '\0' )
        && ( arg1[0] == '\0' || arg2[0] == '\0')  )
    {
        send_to_char( "Syntax: password <char> <new>.\n\r", ch );
        return;
    }

    if ( strlen(arg2) < 5 )
    {
        send_to_char(
            "New password must be at least five characters long.\n\r", ch );
        return;
    }

    pwdnew = crypt( arg2, victim->name );

    free_string( victim->pcdata->pwd );
    victim->pcdata->pwd = str_dup( pwdnew );
    save_char_obj( victim );
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_iscore( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    extern bool wizlock;

    sprintf( buf, "(wiz) Invis: %s   Holylight: %s   Incog: %s\n\r",
        IS_SET( ch->act, PLR_WIZINVIS  ) ? "YES" : "NO ",
        IS_SET( ch->act, PLR_HOLYLIGHT ) ? "YES" : "NO ",
        IS_SET( ch->act, PLR_INCOG     ) ? "YES" : "NO" );
    send_to_char( buf, ch );

    if ( IS_SET( ch->act, PLR_WIZINVIS ) )
    {
        sprintf( buf, "You are wizinvis at level %d.\n\r", ch->invis );
        send_to_char( buf, ch );
    }
    if ( IS_SET( ch->act, PLR_INCOG ) )
    {
        sprintf( buf, "You are incog at level %d.\n\r", ch->incog );
        send_to_char( buf, ch );
    }

    sprintf( buf, "Bamfin:  %s\n\r",
        (ch->pcdata != NULL && ch->pcdata->bamfin[0] != '\0')
        ? ch->pcdata->bamfin : "Not changed/Switched." );
    send_to_char( buf, ch );

    sprintf( buf, "Bamfout: %s\n\r",
        (ch->pcdata != NULL && ch->pcdata->bamfout[0] != '\0' )
        ? ch->pcdata->bamfout : "Not changed/Switched." );
    send_to_char( buf, ch );

    sprintf( buf, "Mud Info:\n\rWizlock: %s\n\r",
        wizlock ? "YES" : "NO " );
    send_to_char( buf, ch );

    return;
}

void do_iwhere( CHAR_DATA *ch, char *argument )
{
    /* Like WHERE, except is global, and shows area & room.
     * --Stephen
     */

    CHAR_DATA *vch;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    int count = 0;
    buf2[0] = '\0';

    send_to_char( "Name          Location\n\r", ch );
    send_to_char( "----          --------\n\r", ch );

    for ( vch = first_char; vch != NULL; vch = vch->next )
    {
        if ( !IS_NPC( vch ) && can_see( ch, vch ) )
        {

            count++;
            sprintf( buf, "%-12s %3d/%-3d\n\r",
                vch->name, vch->x, vch->y );
            safe_strcat( MSL, buf2, buf );
        }
    }

    if ( count == 0 )
        safe_strcat( MSL, buf2, "No Players found!\n\r" );
    else
    {
        sprintf( buf, "%d Player%s found.\n\r", count,
            ( count > 1 ) ? "s" : "" );
        safe_strcat( MSL, buf2, buf );
    }

    safe_strcat( MSL, buf2, "\n\r" );
    send_to_char( buf2, ch );
    return;
}

void do_isnoop( CHAR_DATA *ch, char *argument )
{
    /* Creator-only command.  Lists who (if anyone) is being snooped.
     * -S- */

    DESCRIPTOR_DATA *d;
    char buf[MAX_STRING_LENGTH];
    int count = 0;

    send_to_char( "Snoop List:\n\r-=-=-=-=-=-\n\r", ch );

    for ( d = first_desc; d != NULL; d = d->next )
    {
        if ( d->snoop_by != NULL )
        {
            count++;
            sprintf( buf, "%s by %s.\n\r", d->character->name,
                d->snoop_by->character->name );
            send_to_char( buf, ch );
        }
    }

    if ( count != 0 )
        sprintf( buf, "%d snoops found.\n\r", count );
    else
        sprintf( buf, "No snoops found.\n\r" );

    send_to_char( buf, ch );
    return;
}

void do_togbuild( CHAR_DATA *ch, char *argument )
{
    /* Toggles PC's ch->act PLR_BUILDER value
     * -S-
     */

    CHAR_DATA *victim;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Toggle who as a builder??\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC( victim ) )
    {
        send_to_char( "Not on NPCs!\n\r", ch );
        return;
    }

    if ( !IS_SET( victim->act, PLR_BUILDER ) )
    {
        SET_BIT( victim->act, PLR_BUILDER );
        send_to_char( "Bit set to ALLOW building.\n\r", ch );
        send_to_char( "You have been authorized to use the builder.\n\r", victim );
    }
    else
    {
        REMOVE_BIT( victim->act, PLR_BUILDER );
        send_to_char( "Bit set to DISALLOW building.\n\r", ch );
        send_to_char( "You authorization to build has been revoked.\n\r", victim );
    }

    return;
}

void do_whoname( CHAR_DATA *ch, char *argument )
{
    /* Set victim's who name -
     * what appears on who list in place of their levels
     * --Stephen
     */

    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char( "Usage: whoname <victim> <string>\n\r\n\r", ch );
        send_to_char( "Where string is no more than 14 letters long.\n\r", ch );
        send_to_char( "Use 'off' as name to use default who name.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "Couldn't find target.\n\r", ch );
        return;
    }

    if ( !IS_IMMORTAL( ch ) )
    {
        send_to_char( "You must be an immortal to change your whoname!\n\r", ch );
        return;
    }

    if ( ( get_trust( ch ) < ( 82 ) ) && ch != victim )
    {
        send_to_char( "You do not have access to change other people's whonames.\n\r", ch );
        return;
    }

    if ( !str_cmp( argument, "off" ) )
    {
        free_string( victim->pcdata->who_name );
        victim->pcdata->who_name = str_dup( "off" );
        send_to_char( "Who name set to default value.\n\r", ch );
        return;
    }

    smash_tilde( argument );

    if ( nocol_strlen( argument ) > 9 )
    {
        send_to_char( "Name too long.\n\r", ch );
        do_whoname( ch, "" );                               /* Usage message */
        return;
    }
    else if ( nocol_strlen( argument ) < 9 )
    {
        int i,l;
        l = 9-nocol_strlen(argument);
        for (i=0;i<l;i++)
            sprintf(argument,"%s ",argument);
    }

    free_string( victim->pcdata->who_name );
    victim->pcdata->who_name = str_dup( argument );
    send_to_char( "Ok, done.\n\r", ch );
    return;
}

void do_ranking( CHAR_DATA *ch, char *argument )
{

    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );

    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char( "Usage: ranking <victim> <string>\n\r\n\r", ch );
        send_to_char( "Use 'off' as name to use default rankings.\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "Couldn't find target.\n\r", ch );
        return;
    }

    if ( !IS_IMMORTAL( ch ) )
    {
        send_to_char( "You must be an immortal to change your whoname!\n\r", ch );
        return;
    }

    if ( my_strlen( argument ) > 10 && !IS_IMMORTAL( victim ))
    {
        send_to_char( "That ranking is too long. Maximum length is 10 characters for mortals.", ch );
        return;
    }


    if ( !str_cmp( argument, "off" ) )
    {
        free_string( victim->pcdata->ranking );
        victim->pcdata->ranking = str_dup( "off" );
        send_to_char( "Ranking set to default value.\n\r", ch );
        return;
    }

    smash_tilde( argument );

    free_string( victim->pcdata->ranking );
    victim->pcdata->ranking = str_dup( argument );
    send_to_char( "Ok, done.\n\r", ch );
    return;
}

struct monitor_type
{
    char *name;
    int  channel;
    int min_level;
    char *col;
    char *id;
    char *on_name;
    char *off_name;
};

struct monitor_type monitor_table[] =
{
    {
        "connection", MONITOR_CONNECT, 83, "@@l", "CON",
        "[ CONNECTION   ] Shows details of players connecting to the mud.\n\r",
        "[ connection   ] Not showing details of players connecting.\n\r"
    },

    {
        "area_update", MONITOR_AREA_UPDATE, 82, "@@p", "A_UPD",
        "[ AREA_UPDATE  ] Informs you of ALL area updates.\n\r",
        "[ area_update  ] You are not informed of area updates.\n\r"
    },

    {
        "area_bugs", MONITOR_AREA_BUGS, 82, "@@p", "A_BUG",
        "[ AREA_BUGS    ] Notifies you of any errors within areas.\n\r",
        "[ area_bugs    ] You are not told of errors within areas.\n\r"
    },

    {
        "area_save", MONITOR_AREA_SAVING, 83, "@@p", "A_SAVE",
        "[ AREA_SAVE    ] You get told of all area saving.\n\r",
        "[ area_save    ] You don't get told of all area saves.\n\r"
    },

    {
        "objects",   MONITOR_OBJ, 83, "@@r", "OBJ",
        "[ OBJECTS      ] You are told of problems relating to objects.\n\r",
        "[ objects      ] You are not told of object-related problems.\n\r"
    },
    {
        "imm_general", MONITOR_GEN_IMM, 85, "@@y", "IMM_GEN",
        "[ IMM_GENERAL  ] You are notified of use of logged immortal commands.\n\r",
        "[ imm_general  ] You are not told of the use of logged immortal commands.\n\r"
    },

    {
        "mort_general", MONITOR_GEN_MORT, 84, "@@y", "MORT_GEN",
        "[ MORT_GENERAL ] You are notified of use of logged mortal commands.\n\r",
        "[ mort_general ] You are not told of the use of logged mortal commands.\n\r"
    },

    {
        "combat", MONITOR_COMBAT, 82, "@@R", "COMBAT",
        "[ COMBAT       ] You are monitoring problems in combat.\n\r",
        "[ combat       ] Not monitoring any combat problems.\n\r"
    },

    {
        "build", MONITOR_BUILD, 85, "@@y", "BUILD",
        "[ BUILD        ] You receive logged building commands.\n\r",
        "[ build        ] You don't monitor logged building commands.\n\r"
    },

    {
        "bad", MONITOR_BAD, 85, "@@W", "BAD",
        "[ BAD          ] You are told of 'bad' things players (try to) do!\n\r",
        "[ bad          ] Not told of 'bad' things players do.\n\r"
    },
    {
        "debug", MONITOR_DEBUG, 85, "@@W", "DEBUG",
        "[ DEBUG        ] You are watching code debugging info!\n\r",
        "[ debug        ] Not watching code debugging info.\n\r"
    },
    {
        "system", MONITOR_SYSTEM, 84, "@@W", "SYSTEM",
        "[ SYSTEM       ] You are told of system messages.\n\r",
        "[ system       ] Not told of system messages.\n\r"
    },
    {
        "ldebug", MONITOR_LDEBUG, 85, "@@W", "LDEBUG",
        "[ LDEBUG       ] You are monitoring Local Debug information.\r\n",
        "[ ldebug       ] You are not monitoring Local Debug Information.\r\n"
    },
    {
        "fake", MONITOR_FAKE, 82, "@@W", "FAKE",
        "[ FAKE         ] You are monitoring fake characters.\r\n",
        "[ fake         ] You are not monitoring fake characters.\r\n"
    },
    { NULL, 0, 0, NULL, NULL }
};


void do_monitor( CHAR_DATA *ch, char *argument )
{
    int a;
    bool found = FALSE;
    char buf[MAX_STRING_LENGTH];
    buf[0] = '\0';

    if ( argument[0] == '\0' )
    {
        send_to_char( "@@yMonitor Channel Details:@@g\n\r\n\r", ch );
        for ( a = 0; monitor_table[a].min_level != 0; a++ )
        {
            char colbuf[10];
            colbuf[0] = '\0';

            if ( monitor_table[a].min_level > get_trust( ch ) )
                continue;

            if ( IS_SET( ch->pcdata->monitor, monitor_table[a].channel ) )
            {
                if ( !IS_NPC( ch ) )
                {
                    sprintf( colbuf, "@@%c", ch->pcdata->hicol );
                    safe_strcat( MSL, buf, colbuf );
                }
                safe_strcat( MAX_STRING_LENGTH, buf, monitor_table[a].on_name );
            }

            else
            {
                if ( !IS_NPC( ch ) )
                {
                    sprintf( colbuf, "@@%c", ch->pcdata->dimcol );
                    safe_strcat( MSL, buf, colbuf );
                }
                safe_strcat( MAX_STRING_LENGTH, buf, monitor_table[a].off_name);
            }

        }

        send_to_char( buf, ch );

        send_to_char( "\n\r@@yMONITOR <name> toggles the monitor channels.@@g\n\r", ch );
        return;
    }
    /* Search for monitor channel to turn on/off */
    for ( a = 0; monitor_table[a].min_level != 0; a++ )
    {
        if ( !strcmp( argument, monitor_table[a].name ) )
        {
            found = TRUE;
            if ( IS_SET( ch->pcdata->monitor, monitor_table[a].channel ) )
                REMOVE_BIT( ch->pcdata->monitor, monitor_table[a].channel );
            else
                SET_BIT( ch->pcdata->monitor, monitor_table[a].channel );
            break;
        }
    }
    if ( !found )
    {
        do_monitor( ch, "" );
        return;
    }
    send_to_char( "Ok, monitor channel toggled.\n\r", ch );
    return;
}

void monitor_chan( CHAR_DATA *ch, const char *message, int channel )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    int a;
    int level = 85;

    for ( a = 0; monitor_table[a].min_level != 0; a++ )
        if ( monitor_table[a].channel == channel )
    {
        level = monitor_table[a].min_level;
        break;
    }

    sprintf( buf, "%s[%7s]@@N %s@@N\n\r",
        monitor_table[a].col, monitor_table[a].id, strip_out( message, "\n\r" ) );

    for ( d = first_desc; d; d = d->next )
    {
        if (
            d->connected == CON_PLAYING
            && !IS_NPC( d->character )
            && IS_SET( d->character->pcdata->monitor, channel )
            && level <= get_trust( d->character )
            && (ch ? can_see( d->character, ch ) : TRUE ) )
        {
            send_to_char( buf, d->character );
        }

    }
    return;
}

void do_immlog( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    argument = one_argument( argument, arg1 );
    
    if ( get_trust(ch) > 80 )
    {
        if ( arg1[0] == '\0' )
        {
            send_to_char( "ImmLog whom?\n\r", ch );
            return;
        }

            if ( !str_cmp( arg1, "all" ) )
        {
            if ( fLogAll )
            {
                fLogAll = FALSE;
                send_to_char( "ImmLog ALL off.\n\r", ch );
            }
            else
            {
                fLogAll = TRUE;
                send_to_char( "ImmLog ALL on.\n\r", ch );
            }
            return;
        }

        if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
        {
            {
                char buf[MSL];
                sprintf( buf, "%s is not online.\n\r", capitalize( arg1 ) );
                send_to_char( buf, ch );
                return;
            }

        }

        if ( IS_NPC(victim) )
        {
            send_to_char( "Not on NPC's.\n\r", ch );
            return;
        }

        /*
         * No level check, gods can log anyone.
         */
        if ( IS_SET(victim->act, PLR_LOG) )
        {
            REMOVE_BIT(victim->act, PLR_LOG);
            send_to_char( "IMMLOG removed.\n\r", ch );
        }
        else
        {
            SET_BIT(victim->act, PLR_LOG);
            send_to_char( "IMMLOG set.\n\r", ch );
        }
        return;
    }
}

void do_reward( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int value;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: reward <victim> <value>\n\r",      ch );
        send_to_char( "Value being pos to give points, or neg to take points.\n\r",            ch );
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg2 ) )
    {
        send_to_char( "Value must be numeric.\n\r", ch );
        return;
    }
    value = atoi( arg2 );

    if ( !str_cmp(arg1,"all") )
    {

        for ( victim = first_char;victim;victim = victim->next )
        {
            if ( IS_IMMORTAL(victim) || !victim->desc || victim->desc->connected != CON_PLAYING )
                continue;
            sprintf( buf, "@@NYou have been rewarded @@y%3d @@aQuest Points@@N by @@m %s @@N!!!\n\r", value, ch->name );
            send_to_char( buf, victim );
            sprintf( buf, "@@NYou have rewarded @@r%s  @@y%3d @@aQuest Points@@N!!!\n\r", victim->name, value );
            send_to_char( buf, ch );

            victim->quest_points += value;

        }
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    sprintf( buf, "@@NYou have been rewarded @@y%3d @@aQuest Points@@N by @@m %s @@N!!!\n\r", value, ch->name );
    send_to_char( buf, victim );
    sprintf( buf, "@@NYou have rewarded @@r%s  @@y%3d @@aQuest Points@@N!!!\n\r", victim->name, value );
    send_to_char( buf, ch );

    victim->quest_points += value;
    return;
}

void do_xpreward( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int value;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: xpreward <victim> <value>\n\r",      ch );
        send_to_char( "Value being pos to give points, or neg to take points.\n\r",            ch );
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg2 ) )
    {
        send_to_char( "Value must be numeric.\n\r", ch );
        return;
    }
    value = atoi( arg2 );

    if ( !str_cmp(arg1,"all") )
    {

        for ( victim = first_char;victim;victim = victim->next )
        {
            if ( IS_IMMORTAL(victim) || !victim->desc || victim->desc->connected != CON_PLAYING )
                continue;
            sprintf( buf, "@@NYou have been rewarded @@y%3d @@aExperience Points@@N by @@m %s @@N!!!\n\r", value, ch->name );
            send_to_char( buf, victim );
            sprintf( buf, "@@NYou have rewarded @@r%s  @@y%3d @@aExperience Points@@N!!!\n\r", victim->name, value );
            send_to_char( buf, ch );

            victim->pcdata->experience += value;
        }
        return;
    }
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    sprintf( buf, "@@NYou have been rewarded @@y%3d @@aExperience Points@@N by @@m %s @@N!!!\n\r", value, ch->name );
    send_to_char( buf, victim );
    sprintf( buf, "@@NYou have rewarded @@r%s  @@y%3d @@aExperience Points@@N!!!\n\r", victim->name, value );
    send_to_char( buf, ch );

    victim->pcdata->experience += value;
    return;
}

void do_gpreward( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    int value;

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: gpreward <victim> <value>\n\r",      ch );
        send_to_char( "Value being pos to give points, or neg to take points.\n\r",            ch );
        return;
    }

    /*
     * Snarf the value.
     */
    if ( !is_number( arg2 ) )
    {
        send_to_char( "Value must be numeric.\n\r", ch );
        return;
    }
    value = atoi( arg2 );
    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    sprintf( buf, "@@NYou have been rewarded @@y%3d @@aGame Points@@N by @@m %s @@N!!!\n\r", value, ch->name );
    send_to_char( buf, victim );
    sprintf( buf, "@@NYou have rewarded @@r%s  @@y%3d @@aGame Points@@N!!!\n\r", victim->name, value );
    send_to_char( buf, ch );

    victim->game_points += value;
    return;
}

/* Expand the name of a character into a string that identifies THAT
   character within a room. E.g. the second 'guard' -> 2. guard */
const char * name_expand (CHAR_DATA *ch)
{
    int count = 1;
    CHAR_DATA *rch;
    char name[MAX_INPUT_LENGTH];                            /*  HOPEFULLY no mob has a name longer than THAT */

    static char outbuf[MAX_INPUT_LENGTH];

    if (!IS_NPC(ch))
        return ch->name;

    one_argument (ch->name, name);                          /* copy the first word into name */

    if (!name[0])                                           /* weird mob .. no keywords */
    {
        strcpy (outbuf, "");                                /* Do not return NULL, just an empty buffer */
        return outbuf;
    }

    for (rch = first_char; rch && (rch != ch);rch = rch->next)
        if (is_name (name, rch->name))
            count++;

    sprintf (outbuf, "%d.%s", count, name);
    return outbuf;
}

/*
 * For by Erwin S. Andreasen (4u2@aabc.dk)
 */
void do_for (CHAR_DATA *ch, char *argument)
{
    char range[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];
    bool fGods = FALSE, fMortals = FALSE, fMobs = FALSE, fEverywhere = FALSE, found;
    ROOM_INDEX_DATA *room, *old_room;
    CHAR_DATA *p, *p_next;
    int i;

    extern bool disable_timer_abort;

    disable_timer_abort = TRUE;

    argument = one_argument (argument, range);

    if (!range[0] || !argument[0])                          /* invalid usage? */
    {
        do_help (ch, "for");
        disable_timer_abort = FALSE;
        return;
    }

    if (!str_prefix("quit", argument))
    {
        send_to_char ("Are you trying to crash the MUD or something?\n\r",ch);
        disable_timer_abort = FALSE;
        return;
    }

    if (!str_cmp (range, "all"))
    {
        fMortals = TRUE;
        fGods = TRUE;
    }
    else if (!str_cmp (range, "gods"))
        fGods = TRUE;
    else if (!str_cmp (range, "mortals"))
        fMortals = TRUE;
    else if (!str_cmp (range, "everywhere"))
        fEverywhere = TRUE;
    else
        do_help (ch, "for");                                /* show syntax */

    /* do not allow # to make it easier */
    if (fEverywhere && strchr (argument, '#'))
    {
        send_to_char ("Cannot use FOR EVERYWHERE with the # thingie.\n\r",ch);
        disable_timer_abort = FALSE;
        return;
    }

    if (fMobs && strchr (argument, '#'))
    {
        send_to_char ("Cannot use FOR MOBS with the # thingie.\n\r",ch);
        disable_timer_abort = FALSE;
        return;
    }

    if (strchr (argument, '#'))                             /* replace # ? */
    {
        for (p = first_char; p ; p = p_next)
        {
            p_next = p->next;                               /* In case someone DOES try to AT MOBS SLAY # */
            found = FALSE;

            if (!(p->in_room) || (p == ch))
                continue;

            if (IS_NPC(p) && fMobs)
                found = TRUE;
            else if (!IS_NPC(p) && p->level >= LEVEL_IMMORTAL && fGods)
                found = TRUE;
            else if (!IS_NPC(p) && p->level < LEVEL_IMMORTAL && fMortals)
                found = TRUE;

            /* It looks ugly to me.. but it works :) */
            if (found)                                      /* p is 'appropriate' */
            {
                char *pSource = argument;                   /* head of buffer to be parsed */
                char *pDest = buf;                          /* parse into this */

                while (*pSource)
                {
                    if (*pSource == '#')                    /* Replace # with name of target */
                    {
                        const char *namebuf = name_expand (p);

                        if (namebuf)                        /* in case there is no mob name ?? */
                            while (*namebuf)                /* copy name over */
                                *(pDest++) = *(namebuf++);

                        pSource++;
                    }
                    else
                        *(pDest++) = *(pSource++);
                }                                           /* while */
                *pDest = '\0';                              /* Terminate */

                /* Execute */
                old_room = ch->in_room;
                char_from_room (ch);
                char_to_room (ch,p->in_room);
                interpret (ch, buf);
                char_from_room (ch);
                char_to_room (ch,old_room);

            }                                               /* if found */
        }                                                   /* for every char */
    }
    else                                                    /* just for every room with the appropriate people in it */
    {
        for (i = 0; i < MAX_KEY_HASH; i++)                  /* run through all the buckets */
            for (room = room_index_hash[i] ; room ; room = room->next)
        {
            found = FALSE;

            /* Anyone in here at all? */
            if (fEverywhere)                                /* Everywhere executes always */
                found = TRUE;
            else if (!first_char)                           /* Skip it if room is empty */
                continue;

            /* Check if there is anyone here of the requried type */
            /* Stop as soon as a match is found or there are no more ppl in room */
            for (p = first_char; p && !found; p = p->next)
            {

                if (p == ch)                                /* do not execute on oneself */
                    continue;

                if (IS_NPC(p) && fMobs)
                    found = TRUE;
                else if (!IS_NPC(p) && (p->level >= LEVEL_IMMORTAL) && fGods)
                    found = TRUE;
                else if (!IS_NPC(p) && (p->level <= LEVEL_IMMORTAL) && fMortals)
                    found = TRUE;
            }                                               /* for everyone inside the room */

            if (found)
            {
                /* This may be ineffective. Consider moving character out of old_room
                   once at beginning of command then moving back at the end.
                   This however, is more safe?
                */

                old_room = ch->in_room;
                char_from_room (ch);
                char_to_room (ch, room);
                interpret (ch, argument);
                char_from_room (ch);
                char_to_room (ch, old_room);
            }                                               /* if found */
        }                                                   /* for every room in a bucket */
    }                                                       /* if strchr */
    disable_timer_abort = FALSE;
}                                                           /* do_for */

void do_otype( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;
    bool mailme = FALSE;
    if ( is_name( "mailme", argument ) )
        mailme = TRUE;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
        if ( arg[0] == '\0' )
    {
        sprintf(buf,"Values for object types:\n\r");
        wide_table_printout(tab_item_types,buf+strlen(buf));
        send_to_char(buf,ch);
        return;
    }

    buf1[0] = '\0';
    fAll        = !str_cmp( arg, "all" );
    found       = FALSE;
    nMatch      = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
            nMatch++;
            if ( fAll || is_name( arg, tab_item_types[(pObjIndex->item_type)-1 ].text ) )
            {
                found = TRUE;
                sprintf( buf, "<%2d> [%5d] %s\n\r", pObjIndex->level,
                    pObjIndex->vnum, pObjIndex->short_descr );
                if ( mailme )
                    safe_strcat(MAX_STRING_LENGTH, buf1, buf);
                else
                    send_to_char( buf, ch );
            }
        }
    }

    if ( !found )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
        return;
    }

    sprintf( buf, "Otype report for %s", arg );
    send_rep_out( ch, buf1, mailme, buf  );
    return;
}

void do_owear( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;
    bool mailme = FALSE;
    if ( is_name( "mailme", argument ) )
        mailme = TRUE;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        sprintf(buf,"Values for wear slots:\n\r");
        wide_table_printout(tab_wear_flags,buf+strlen(buf));
        send_to_char(buf,ch);
        return;
    }
    buf1[0] = '\0';
    fAll        = !str_cmp( arg, "all" );
    found       = FALSE;
    nMatch      = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
            nMatch++;
            if ( fAll || !str_infix ( arg,
                bit_table_lookup(tab_wear_flags, pObjIndex->wear_flags) ) )
            {
                found = TRUE;
                sprintf( buf, "<%s> [%5d] [%3d] %s\n\r",
                    bit_table_lookup(tab_wear_flags, pObjIndex->wear_flags),
                    pObjIndex->vnum,
                    pObjIndex->level,
                    pObjIndex->short_descr );
                if ( mailme )
                    safe_strcat(MAX_STRING_LENGTH, buf1, buf);
                else
                    send_to_char( buf, ch );
            }
        }
    }

    if ( !found )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r",
            ch );
        return;
    }

    sprintf( buf, "Owear report for %s", arg );
    send_rep_out( ch, buf1, mailme, buf  );
    return;
}

void do_osearch( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int level;
    int nMatch;
    bool found;
    bool mailme = FALSE;
    if ( is_name( "mailme", argument ) )
        mailme = TRUE;
    argument = one_argument( argument, arg1 );
    one_argument( argument, arg2 );
    if ( arg1[0] == '\0' || arg2[0] == '\0' || !is_number(arg2) )
    {
        sprintf(buf,"Values for wear slots:\n\r");
        wide_table_printout(tab_wear_flags,buf+strlen(buf));
        send_to_char(buf,ch);
        send_to_char("Syntax: osearch <wear> <level>\n\r",ch);
        return;
    }
    buf1[0] = '\0';
    found       = FALSE;
    nMatch      = 0;
    level       = atoi( arg2 );

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
            nMatch++;
            if ( pObjIndex->level == level &&
                !str_infix( arg1, bit_table_lookup(tab_wear_flags, pObjIndex->wear_flags) ) )
            {
                found = TRUE;
                sprintf( buf, "<%s> [%5d] [%3d] %s\n\r",
                    bit_table_lookup(tab_wear_flags, pObjIndex->wear_flags),
                    pObjIndex->vnum,
                    pObjIndex->level,
                    pObjIndex->short_descr );
                safe_strcat(MAX_STRING_LENGTH, buf1, buf);
            }
        }
    }

    if ( !found )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
        return;
    }

    sprintf( buf, "Osearch report for %s, level %d", arg1, level );
    send_rep_out( ch, buf1, mailme, buf  );
    return;
}

void do_areasave( CHAR_DATA *ch, char *argument )
{
    AREA_DATA *pArea;

    for ( pArea = first_area; pArea != NULL; pArea = pArea->next )
        area_modified( pArea );
    send_to_char( "Done.\n\r", ch );
    build_save_flush();

    return;
}

void do_smite (CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    int number;

    argument = one_argument (argument, arg1);
    argument = one_argument (argument, arg2);
    argument = one_argument (argument, arg3);
    number = atoi(arg2);
    if (arg1[0] == '\0' || arg2[0] == '\0')
    {
        send_to_char ("Syntax:  smite <person> <amount of damage> <send notice? (yes or no)>\n\r", ch);
        return;
    }
    if ((victim = get_char_world (ch, arg1)) == NULL)
    {
        sprintf (buf, "%s is not connected.\n\r", capitalize (arg1));
        send_to_char (buf, ch);
        return;
    }
    if ( get_trust(victim) >= get_trust(ch) )
    {
        sprintf (buf, "%s would not appreciate that.\n\r", capitalize (arg1));
        send_to_char (buf, ch);
        return;
    }
    if (number == 0)
    {
        send_to_char("At least do 1 point of damage, shesh!", ch);
        return;
    }
    if (number < -1)
    {
        sprintf (buf, "Your doing %d points of damage would HEAL the target, moron!", number);
        send_to_char(buf, ch);
        return;
    }
    if (ch == victim)
    {
        send_to_char("Why would you wanna smite yourself?!\n\r", ch);
        return;
    }
    victim->hit -= number;
    sprintf( buf, "@@WA burst of astroids sent by @@c%s @@Wcrashes down on you! @@c(@@a%d@@c)@@N\n\r", ch->name, number );
    send_to_char( buf, victim );
    sprintf( buf, "@@WA burst of astroids sent by %s crashes down on %s@@W!@@N\n\r", ch->name, victim->name );
    act( buf, victim, NULL, NULL, TO_ROOM );
    send_to_char( "@@eDirect hit!\n\r@@N", ch );
    if(victim->hit <=0)
    {
        act( "$N @@dis @@2@@aDEAD@@N@@d!!@@N", ch, NULL, victim, TO_NOTVICT );
        act( "@@dYou have been @@2@@aKILLED@@N@@d!!@@N\n\r", ch, NULL, victim, TO_VICT );
        raw_kill(victim, ch->name);
        victim->dead = TRUE;
    }
    if ( arg3[0] != '\0' )
    {
        sprintf( buf, "@@a%s@@W has been smitten by @@e%s@@N", 
victim->name, 
ch->name );
        info( buf, 0);
    }
    return;
}

void do_fslay( CHAR_DATA *ch, char *argument )
{
    {
        char buf[MAX_STRING_LENGTH];
        CHAR_DATA *victim;
        char arg[MAX_INPUT_LENGTH];
        char arg2[MAX_INPUT_LENGTH];

        argument = one_argument( argument, arg );
        one_argument( argument, arg2 );
        if ( arg[0] == '\0' )
        {
            send_to_char( "Syntax: [Char] [Type]\n\r", ch );
            send_to_char( "Types: Skin, Slit, Immolate, Demon, Shatter, Deheart, Pounce, Fslay.\n\r", ch);
            return;
        }

        if ( ( victim = get_char_room( ch, arg ) ) == NULL )
        {
            send_to_char( "They aren't here.\n\r", ch );
            return;
        }

        if ( ch == victim )
        {
            send_to_char( "Suicide is a mortal sin.\n\r", ch );
            return;
        }

        if ( !IS_NPC(victim) && get_trust( victim ) >= get_trust( ch ) )
        {
            send_to_char( "You failed.\n\r", ch );
            return;
        }

        if ( !str_cmp( arg2, "skin" ) )
        {
            act( "You rip the flesh from $N and send his soul to the fiery depths of hell.", ch, NULL, victim, TO_CHAR );
            act( "Your flesh has been torn from your bones and your bodyless soul now watches your bones incenerate in the fires ofhell.", ch, NULL, victim,TO_VICT );
            act( "$n rips the flesh off of $N, releasing his soul into the fiery depths of hell.", ch, NULL, victim, TO_NOTVICT );
        }

        else if ( !str_cmp( arg2, "deheart" ) )
        {
            act( "You rip through $N's chest and pull out $M beating heart in your hand.", ch, NULL, victim, TO_CHAR );
            act( "You feel a sharp pain as $n rips into your chest and pulls our your beating heart in $M hand.", ch, NULL,victim, TO_VICT );
            act( "Specks of blood hit your face as $n rips through $N's chest pulling out $M's beating heart.", ch, NULL, victim,TO_NOTVICT );
        }

        else if ( !str_cmp( arg2, "immolate" ) )
        {
            act( "Your fireball turns $N into a blazing inferno.",  ch, NULL, victim, TO_CHAR    );
            act( "$n releases a searing fireball in your direction.", ch, NULL, victim, TO_VICT    );
            act( "$n points at $N, who bursts into a flaming inferno.",  ch, NULL, victim, TO_NOTVICT );
        }

        else if ( !str_cmp( arg2, "shatter" ) )
        {
            act( "You freeze $N with a glance and shatter the frozen corpse into tiny shards.",  ch, NULL, victim, TO_CHAR    );
            act( "$n freezes you with a glance and shatters your frozen body into tiny shards.", ch, NULL, victim, TO_VICT    );
            act( "$n freezes $N with a glance and shatters the frozen body into tiny shards.",  ch, NULL, victim, TO_NOTVICT );
        }

        else if ( !str_cmp( arg2, "demon" ) )
        {
            act( "You gesture, and a slavering demon appears.  With a horrible grin, the foul creature turns on $N, who screams inpanic before being eaten alive.",  ch, NULL, victim, TO_CHAR );
            act( "$n gestures, and a slavering demon appears.  The foul creature turns on you with a horrible grin.   You scream inpanic before being eaten alive.",  ch, NULL, victim, TO_VICT );
            act( "$n gestures, and a slavering demon appears.  With a horrible grin, the foul creature turns on $N, who screams inpanic before being eaten alive.",  ch, NULL, victim, TO_NOTVICT );
        }

        else if ( !str_cmp( arg2, "pounce" ) )
        {
            act( "Leaping upon $N with bared fangs, you tear open $S throat and toss the corpse to the ground...",  ch, NULL,victim, TO_CHAR );
            act( "In a heartbeat, $n rips $s fangs through your throat!  Your blood sprays and pours to the ground as your lifeends...", ch, NULL, victim, TO_VICT );
            act( "Leaping suddenly, $n sinks $s fangs into $N's throat.  As blood sprays and gushes to the ground, $n tosses $N'sdying body away.",  ch, NULL, victim, TO_NOTVICT );
        }
        else if ( !str_cmp( arg2, "slit" ))
        {
            act( "You calmly slit $N's throat.", ch, NULL, victim, TO_CHAR );
            act( "$n reaches out with a clawed finger and calmly slits your throat.", ch, NULL, victim, TO_VICT );
            act( "A claw extends from $n's hand as $M calmly slits $N's throat.", ch, NULL, victim, TO_NOTVICT );
        }

        else if ( !str_cmp( arg2, "fslay" ))
        {
            act( "You point at $N and fall down laughing.", ch, NULL, victim, TO_CHAR );
            act( "$n points at you and falls down laughing. How embaressing!.", ch, NULL, victim, TO_VICT );
            act( "$n points at $N and falls down laughing.", ch, NULL, victim, TO_NOTVICT );
            return;
        }

        else
        {
            act( "You slay $N in cold blood!",  ch, NULL, victim, TO_CHAR    );
            act( "$n slays you in cold blood!", ch, NULL, victim, TO_VICT    );
            act( "$n slays $N in cold blood!",  ch, NULL, victim, TO_NOTVICT );
        }

        victim->hit    = victim->max_hit - victim->max_hit + 1;
        sprintf(buf, "%s 3001", victim->name);
        do_transfer(victim, buf);
        update_pos( victim);
        return;
    }
}

void do_oflags( CHAR_DATA *ch, char *argument )
{
    extern int top_obj_index;
    char buf[MAX_STRING_LENGTH];
    char buf1[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    OBJ_INDEX_DATA *pObjIndex;
    int vnum;
    int nMatch;
    bool fAll;
    bool found;
    bool mailme = FALSE;
    if ( is_name( "mailme", argument ) )
        mailme = TRUE;
    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        sprintf(buf,"Values for obj flags:\n\r");
        wide_table_printout(tab_obj_flags,buf+strlen(buf));
        send_to_char(buf,ch);
        return;
    }
    buf1[0] = '\0';
    fAll        = !str_cmp( arg, "all" );
    found       = FALSE;
    nMatch      = 0;

    /*
     * Yeah, so iterating over all vnum's takes 10,000 loops.
     * Get_obj_index is fast, and I don't feel like threading another link.
     * Do you?
     * -- Furey
     */
    for ( vnum = 0; nMatch < top_obj_index; vnum++ )
    {
        if ( ( pObjIndex = get_obj_index( vnum ) ) != NULL )
        {
            nMatch++;
            if ( fAll || !str_infix ( arg, bit_table_lookup( tab_obj_flags, pObjIndex->extra_flags ) ) )
            {
                found = TRUE;
                sprintf( buf, "[%5d] [%3d] %s\n\r",
                    pObjIndex->vnum,
                    pObjIndex->level,
                    pObjIndex->short_descr );
                safe_strcat(MAX_STRING_LENGTH, buf1, buf);
            }
        }
    }

    if ( !found )
    {
        send_to_char( "Nothing like that in hell, earth, or heaven.\n\r", ch );
        return;
    }

    sprintf( buf, "Oflags report for %s", arg );
    send_rep_out( ch, buf1, mailme, buf  );
    return;
}

void do_olist( CHAR_DATA *ch, char *argument )
{
    int vnum;
    OBJ_DATA *obj;
    char buf[MSL];

    for ( vnum = ch->in_room->area->min_vnum;vnum < ch->in_room->area->max_vnum;vnum++ )
    {
        if ( get_obj_index(vnum) == NULL )
            continue;

        if ( ( obj = create_object( get_obj_index(vnum), 1 ) ) == NULL )
            continue;
        sprintf( buf, "[%5d] [%3d] %s\n", vnum, obj->level, obj->short_descr );
        send_to_char( buf, ch );
        extract_obj( obj );
    }
    return;
}

void do_setwcode( CHAR_DATA *ch, char *argument )
{
    int value;

    if ( IS_NPC(ch) )
        return;

    if ( ch->in_room->vnum != ROOM_VNUM_WMAP )
    {
        send_to_char( "You should be in the wilderness to do that.\n\r", ch );
        return;
    }
    if ( is_number(argument) )
        value = atoi(argument);
    else
    {
        send_to_char( "Invalid sector value.\n\r", ch );
        return;
    }
    if ( value == 9997 )
    {
        int j,m,n,k,x,y,i;
        j = number_range(MAX_MAPS / 40,MAX_MAPS / 24);      /* Generate Forests */

        for ( i = 0;i<j;i++ )
        {
            m = number_range(1,MAX_MAPS/15);
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
                    if ( x < 0 || x > MAX_MAPS || y < 0 || y > MAX_MAPS )
                        continue;
                    if ( y == number_fuzzy(k) )
                        continue;
                    if ( y == number_fuzzy(k-m) )
                        continue;
                    if ( map_table.type[x][y][1] == SECT_FIELD || map_table.type[x][y][1] == SECT_SAND )
                        map_table.type[x][y][1] = SECT_FOREST;
                }
            }
        }

    }
    else if ( value == 9996 )
    {
        int j,m,n,k,x,y,i;
        j = number_range(MAX_MAPS / 40,MAX_MAPS / 24);      /* Generate Snow */

        for ( i = 0;i<j;i++ )
        {
            m = number_range(1,MAX_MAPS/15);
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
                    if ( x < 0 || x > MAX_MAPS || y < 0 || y > MAX_MAPS )
                        continue;
                    if ( y == number_fuzzy(k) )
                        continue;
                    if ( y == number_fuzzy(k-m) )
                        continue;
                    if ( map_table.type[x][y][1] == SECT_FIELD )
                        map_table.type[x][y][1] = SECT_SNOW;
                }
            }
        }

    }
    else if ( value == 9998 )
    {
        if ( planet_table[ch->z].terrain != TERRAIN_NONE )
            create_map(ch,planet_table[ch->z].terrain);
        //		save_map();
        return;
    }
    else if ( value >= SECT_MAX )
    {
        send_to_char( "Invalid sector type.\n\r", ch );
        return;
    }
    if ( ch->z != Z_SPACE )
        map_table.type[ch->x][ch->y][ch->z] = value;
    return;
}

void do_listbuildings( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    char buf[MSL];
    bool imm = IS_IMMORTAL(ch);
    int i = 0,x=0;

    for ( bld = first_building;bld;bld = bld->next )
    {
        i++;
        if ( ( imm && ( ( argument[0] == '\0' && str_cmp(bld->owned,"nobody") ) || !str_cmp(bld->owned,argument) || !str_cmp(bld->name,argument) )  )
            || ( !imm && !str_cmp(bld->owned,argument) ) )
        {
            x++;
            if ( imm )
                sprintf( buf, "@@r%d@@d.@@G %-20s at (%-3d,%-3d,%-3d) [%d]  -> %s\n", i, bld->name, bld->x, bld->y, bld->z, bld->level, bld->owned );
            else
                sprintf( buf, "@@r%d@@d.@@G %-20s at (%-3d,%-3d) [%d]\n", x, bld->name, bld->x, bld->y, bld->level );

            send_to_char( buf, ch );
        }
    }
    sprintf( buf, "\n\r%d/%d buildings found.\n\r", x,i );
    send_to_char(buf, ch );
    return;
}

void do_killbuildin( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to KILL someone's BUILDINGS, type the whole thing out.\n\r", ch);
    return;
}

void do_killbuilding( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    BUILDING_DATA *bld_next;
    int num = 1;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Kill who's buildings? Or what number?\n\r", ch );
        return;
    }
    if ( !is_number(argument) )
    {
        for ( bld = first_building;bld;bld = bld_next )
        {
            bld_next = bld->next;
            if ( !str_cmp(bld->owned,argument) )
            {
                if ( is_neutral(bld->type) && str_cmp(argument,"Nobody"))
                {
                    free_string(bld->owned);
                    bld->owned = str_dup("Nobody");
                    bld->owner = NULL;
                    continue;
                }
                check_building_destroyed(bld);
                extract_building(bld,TRUE);
            }
        }
        save_buildings();
        send_to_char( "Buildings destroyed.\n\r", ch );
        return;
    }
    else
    {
        num = atoi(argument);
        if ( num < 1 )
            return;
        for ( bld = first_building;bld;bld = bld->next )
        {
            num--;
            if ( num == 0 )
            {
                check_building_destroyed(bld);
                extract_building(bld,TRUE);
                send_to_char( "Building destroyed.\n\r", ch );
                save_buildings();
                return;
            }
        }
    }
    send_to_char( "No building found.\n\r", ch );
    return;
}

void do_bset( CHAR_DATA *ch, char *argument )
{
    char arg[MSL];
    char arg2[MSL];
    BUILDING_DATA *bld;
    BUILDING_DATA *bld_next;
    int i;
    bool active = FALSE;

    if ( argument[0] == '\0' )
    {
        send_to_char( "\n\r", ch );
        send_to_char( "        @@gBset <@@aBuilding ID/Owner@@g> <@@aField@@g> <@@aValue@@g>@@N\n\r",     ch );
        send_to_char( "\n\r", ch );
        send_to_char( "@@c+--------------- @@dNormal Fields@@c ---------------+@@N\n\r",     ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c| @@gLevel                                       @@c|@@N\n\r",     ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c+------------- @@dNo-Value Fields@@c ---------------+@@N\n\r",  ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c| @@gComplete                                    @@c|@@N\n\r",     ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c+-------------- @@dString Fields@@c ----------------+@@N\n\r",  ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c| @@gOwner                                       @@c|@@N\n\r",  ch );
        send_to_char( "@@c|                                             @@c|@@N\n\r",     ch );
        send_to_char( "@@c+---------------------------------------------+@@N\n\r",     ch );
        return;
    }
    argument = one_argument(argument,arg);
    argument = one_argument(argument, arg2);
    if ( !str_cmp(arg2,"owner") )
        if ( get_ch(argument) )
            active = TRUE;

    if ( is_number(arg) )
    {
        i = atoi(arg);
        for ( bld = first_building;bld;bld = bld_next )
        {
            bld_next = bld->next;
            i--;
            if ( i == 0 )
            {
                if ( is_neutral(bld->type) )
                    return;
                if ( !str_cmp(arg2, "complete") )
                {
                    for ( i=0;i<8;i++ )
                        bld->resources[i] = 0;
                    send_to_char( "Building completed!\n\r", ch );
                    bld->hp = bld->maxhp;
                    bld->shield = bld->maxshield;
                }
                else if ( !str_cmp(arg2, "owner") )
                {
                    free_string(bld->owned);
                    bld->owned = str_dup(argument);
                    bld->owner = NULL;
                    activate_building(bld,active);
                }
                else if ( !str_cmp(arg2, "level") )
                {
                    int level;
                    if ( ( level = atoi(argument) ) < 1 || level > 5 )
                    {
                        send_to_char( "Level must be from 1 to 5.\n\r", ch );
                        return;
                    }
                    bld->level = level;
                }
                else if ( !str_cmp(arg2, "v0") )
                    bld->value[0] = atoi(argument);
                else if ( !str_cmp(arg2, "v1") )
                    bld->value[1] = atoi(argument);
                else if ( !str_cmp(arg2, "v2") )
                    bld->value[2] = atoi(argument);
                else if ( !str_cmp(arg2, "v3") )
                    bld->value[3] = atoi(argument);
                else if ( !str_cmp(arg2, "v4") )
                    bld->value[4] = atoi(argument);
                else if ( !str_cmp(arg2, "v5") )
                    bld->value[5] = atoi(argument);
                else if ( !str_cmp(arg2, "v6") )
                    bld->value[6] = atoi(argument);
                else if ( !str_cmp(arg2, "v7") )
                    bld->value[7] = atoi(argument);
                else if ( !str_cmp(arg2, "v8") )
                    bld->value[8] = atoi(argument);
                else if ( !str_cmp(arg2, "v9") )
                    bld->value[9] = atoi(argument);
                else if ( !str_cmp(arg2, "v10") )
                    bld->value[10] = atoi(argument);
                save_buildings();
                return;
            }
        }
    }
    else
    {
        bool all = !str_cmp(arg,"all");
        bool here = !str_cmp(arg,"here");
        for ( bld = first_building;bld;bld = bld_next )
        {
            bld_next = bld->next;
            if ( is_neutral(bld->type) )
                continue;
            if (  !str_cmp(arg,bld->owned) || all || ( here && bld == ch->in_building ) )
            {
                if ( !str_cmp(arg2, "complete") )
                {
                    for ( i=0;i<8;i++ )
                        bld->resources[i] = 0;
                    send_to_char( "Building completed!\n\r", ch );
                    bld->hp = bld->maxhp;
                    bld->shield = bld->maxshield;
                }
                else if ( !str_cmp(arg2, "mhp") )
                {
                    bld->maxhp *= 1.3;
                }
                else if ( !str_cmp(arg2, "owner") )
                {
                    free_string(bld->owned);
                    bld->owned = str_dup(argument);
                    bld->owner = NULL;
                    activate_building(bld,active);
                }
                else if ( !str_cmp(arg2, "level") )
                {
                    int level;
                    if ( CIVILIAN(bld) )
                        continue;
                    if ( ( level = atoi(argument) ) < 1 || level > 5 )
                    {
                        send_to_char( "Level must be from 1 to 5.\n\r", ch );
                        return;
                    }
                    bld->level = level;
                }
                else if ( !str_cmp(arg2, "v0") )
                    bld->value[0] = atoi(argument);
                else if ( !str_cmp(arg2, "v1") )
                    bld->value[1] = atoi(argument);
                else if ( !str_cmp(arg2, "v2") )
                    bld->value[2] = atoi(argument);
                else if ( !str_cmp(arg2, "v3") )
                    bld->value[3] = atoi(argument);
                else if ( !str_cmp(arg2, "v4") )
                    bld->value[4] = atoi(argument);
                else if ( !str_cmp(arg2, "v5") )
                    bld->value[5] = atoi(argument);
                else if ( !str_cmp(arg2, "v6") )
                    bld->value[6] = atoi(argument);
                else if ( !str_cmp(arg2, "v7") )
                    bld->value[7] = atoi(argument);
                else if ( !str_cmp(arg2, "v8") )
                    bld->value[8] = atoi(argument);
                else if ( !str_cmp(arg2, "v9") )
                    bld->value[9] = atoi(argument);
                else if ( !str_cmp(arg2, "v10") )
                    bld->value[10] = atoi(argument);
            }
        }
        save_buildings();
        return;
    }
    send_to_char( "No building found.\n\r", ch );
    return;
}

void do_vload( CHAR_DATA *ch, char *argument )
{
    VEHICLE_DATA *vhc;
    int i;

    if ( argument[0] == '\0' || !is_number(argument) )
    {
        char buf[MSL];
        for ( i=0;i<MAX_VEHICLE;i++ )
        {
            sprintf( buf, "%d. %s\n\r", i, vehicle_desc[i] );
            send_to_char(buf,ch);
        }
        return;
    }
    if ( (i=atoi(argument) ) < 0 || i >= MAX_VEHICLE )
    {
        send_to_char( "Invalid vehicle number.\n\r", ch );
        return;
    }
    vhc = create_vehicle(i);
    vhc->scanner = 10;
    vhc->range = 5;
    move_vehicle(vhc,ch->x,ch->y,ch->z);
    vhc->fuel = 10000;
    vhc->max_fuel = 10000;
    vhc->ammo = 10000;
    vhc->max_ammo = 10000;
    vhc->ammo_type = 5;
    vhc->hit = 10000;
    vhc->max_hit = 10000;
    vhc->speed = 4;
    send_to_loc( "A Vehicle appears in the area.\n\r", ch->x, ch->y,ch->z );
    return;
}

void do_repop( CHAR_DATA *ch, char *argument )
{
    rooms_update();
    building_update();
    return;
}

void do_mmake( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    int type;

    if ( !is_number(argument) )
    {
        send_to_char( "Value must be numeric.\n\r", ch );
        return;
    }
    if ( ( type = atoi(argument) ) < -1 || type > 7 )
    {
        send_to_char( "This value is invalid.\n\r", ch );
        return;
    }

    obj = create_material(type);
    obj_to_char(obj,ch);
    send_to_char( "Done.\n\r", ch );
    return;
}

void do_backup ( CHAR_DATA *ch, char *argument )
{
    if ( !str_cmp(argument,"save") )
    {
        save_buildings_b( 0 );
        save_objects(3);
        save_vehicles(1);
        send_to_char( "Buildings, vehicles and items backed up.\n\r", ch );
    }
    /*	else if ( !str_cmp(argument,"load") )
        {
            BUILDING_DATA *bld;
            BUILDING_DATA *bld_next;
            OBJ_DATA *obj;
            OBJ_DATA *obj_next;

            while ( first_vehicle )
                extract_vehicle(first_vehicle,TRUE);
            load_vehicles(1);
            for ( bld = first_building;bld;bld = bld_next )
            {
                bld_next = bld->next;
                extract_building(bld,TRUE);
            }
            load_buildings_b( 0 );
            for ( obj = first_obj;obj;obj = obj_next )
            {
                obj_next = obj->next;
                move_obj(obj,0,0,1);
                extract_obj(obj);
            }
            load_sobjects(3);
            if ( ch != NULL )
                send_to_char( "Buildings, vehicles and items loaded from backup.\n\r", ch );
            else
                info( "The buildings, vehicles and items have been reloaded from the backup file!",0);
        } */
    else if ( !str_cmp(argument,"savefest") )
    {
        save_buildings_b( 1 );
        save_vehicles( 2 );
        save_objects( 4 );
        send_to_char( "Buildings, objects and items backed up.\n\r", ch );
    }
    else if ( !str_cmp(argument,"loadfest") )
    {
        BUILDING_DATA *bld;
        BUILDING_DATA *bld_next;
        for ( bld = first_building;bld;bld = bld_next )
        {
            bld_next = bld->next;
            extract_building(bld,TRUE);
        }
        load_buildings_b( 1 );
        while ( first_vehicle )
            extract_vehicle(first_vehicle,TRUE);
        load_vehicles(2);
        while ( first_obj )
            extract_obj(first_obj);
        load_sobjects(4);
        send_to_char( "Buildings, items and vehicles loaded from backup.\n\r", ch );
    }
    else
        send_to_char( "Options are: Save, Load, Savefest, Loadfest.\n\r", ch );
    return;
}

void do_bmake( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;

    if ( ( bld = get_char_building(ch) ) == NULL )
    {
        send_to_char( "You must be in a building to generate blueprints for it.\n\r", ch );
        return;
    }
    create_blueprint(bld);
    send_to_char( "Done.\n\r", ch );
    return;
}

void do_objclear( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    BUILDING_DATA *bld;
    CHAR_DATA *wch;
    int x,y;
    bool ok[MAX_MAPS][MAX_MAPS];

    if ( str_cmp(argument,"all") )
    {
        send_to_char( "Syntax: clearobjects all\n\r", ch );
        return;
    }

    for ( x = 0;x < MAX_MAPS;x++ )
        for ( y = 0;y < MAX_MAPS; y++ )
            ok[x][y] = FALSE;

    for ( bld = first_building;bld;bld = bld->next )
    {
        if ( bld->x < 0 || bld->y < 0 )
            continue;
        if ( ( wch = get_ch(bld->owned) ) != NULL || bld->type == BUILDING_WAREHOUSE )
            ok[bld->x][bld->y] = TRUE;
    }
    for ( obj = first_obj;obj;obj = obj_next )
    {
        obj_next = obj->next;
        if ( !ok[obj->x][obj->y] )
            extract_obj(obj);
        else if ( obj->in_building == NULL && obj->carried_by == NULL && obj->quest_timer == 0 )
            extract_obj(obj);
    }
    send_to_char( "Old objects cleared.\n\r", ch );
    return;
}

void do_savemap( CHAR_DATA *ch, char *argument )
{
    save_map();
    send_to_char( "Done\n\r", ch );
    return;
}

void do_test( CHAR_DATA *ch, char *argument )
{
    /*	extern int guess_game;
        guess_game = number_range(1,1000);
        info("The game has picked a number between 1 and 1000. Guess which on the GAME channel!", 0);
    */
    BUILDING_DATA *bld;                                     
    for ( bld = first_building;bld;bld = bld->next )
    {
        /*		if ( bld->type != BUILDING_WAREHOUSE && bld->type != BUILDING_SECURE_WAREHOUSE )
                    continue;
                obj = create_object(get_obj_index(500),0);
                obj->x = bld->x; obj->y = bld->y; obj->z = bld->z; obj->value[1] = 10000;
                obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));*/
        if ( bld->z != Z_PAINTBALL )
            continue;
        bld->exit[0] = TRUE;
        bld->exit[1] = TRUE;
        bld->exit[2] = TRUE;
        bld->exit[3] = TRUE;
    }
    return;
}

void do_spacepop( CHAR_DATA *ch, char *argument )
{
    int i,s;
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    int warp=5;
    for ( obj = first_obj;obj;obj = obj_next )
    {
        obj_next = obj->next;
        if ( obj->z == Z_SPACE )
            extract_obj(obj);
    }
    for ( s=1;s<4;s++ )
    {
        warp = 5;
        for ( i=0;i<SPACE_SIZE/2;i++ )
        {
            obj = create_object(get_obj_index(800),0);
            obj->x = number_range(1,SPACE_SIZE-1);
            obj->y = number_range(1,SPACE_SIZE-1);
            obj->z = Z_SPACE;
            if ( warp > 0 )
            {
                obj->value[2] = 3;
                warp--;
            }
            obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
        }
        for ( i=0;i<SPACE_SIZE;i++ )
        {
            obj = create_object(get_obj_index(801),0);
            obj->x = number_range(1,SPACE_SIZE-1);
            obj->y = number_range(1,SPACE_SIZE-1);
            obj->z = Z_SPACE;
            obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
        }
    }
    return;
}

void do_deletefromscores( CHAR_DATA *ch, char *argument )
{
    int i;
    bool found = FALSE;
    for ( i = 0;i<100;i++ )
    {
        if (!str_cmp(score_table[i].name,argument))
        {
            score_table[i].kills = 0;
            score_table[i].buildings = 0;
            score_table[i].time = 0;
            found = TRUE;
        }
    }
    save_scores();
    if (found)
        send_to_char( "Player deleted from high scores.\n\r", ch );
    else
        send_to_char( "No such player found.\n\r", ch );
    return;
}

void do_buildingreimburse( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char name[MAX_STRING_LENGTH];
    bool found = FALSE;
    BUILDING_DATA *bld;
    BUILDING_DATA *bld_next;
    BUILDING_DATA *bld2;
    BUILDING_DATA *bld2_next;
    DESCRIPTOR_DATA  d;
    OBJ_DATA *obj;

    if ( ch->max_hit != 1001 )                              //Just added protection... Gotta have exactly 1001 HP to use
        return;

    for ( bld = first_building;bld;bld = bld_next )
    {
        bld_next = bld->next;
        sprintf( name, "%s", bld->owned );
        found = load_char_obj( &d, name, FALSE );

        if (!found)
        {
            free_char( d.character );
            bld_next = bld->next;
            extract_building(bld,FALSE);
            continue;
        }

        victim = d.character;
        d.character = NULL;
        victim->desc = NULL;

        for ( bld2 = bld;bld2;bld2 = bld2_next )
        {
            bld2_next = bld2->next;
            if ( str_cmp(bld2->owned,victim->name) )
                continue;

            if ( build_table[bld2->type].resources[0] > 0 )
            {
                obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL),0);
                free_string(obj->short_descr);
                obj->short_descr = "Reimbursement: Iron";
                obj->value[0] = ITEM_IRON;
                obj->value[1] = build_table[bld2->type].resources[0];
                obj_to_char(obj,victim);
            }
            if ( build_table[bld2->type].resources[1] > 0 )
            {
                obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL),0);
                free_string(obj->short_descr);
                obj->short_descr = "Reimbursement: Skin";
                obj->value[0] = ITEM_SKIN;
                obj->value[1] = build_table[bld2->type].resources[1];
                obj_to_char(obj,victim);
            }
            if ( build_table[bld2->type].resources[2] > 0 )
            {
                obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL),0);
                free_string(obj->short_descr);
                obj->short_descr = "Reimbursement: Copper";
                obj->value[0] = ITEM_COPPER;
                obj->value[1] = build_table[bld2->type].resources[2];
                obj_to_char(obj,victim);
            }
            if ( build_table[bld2->type].resources[3] > 0 )
            {
                obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL),0);
                free_string(obj->short_descr);
                obj->short_descr = "Reimbursement: Gold";
                obj->value[0] = ITEM_GOLD;
                obj->value[1] = build_table[bld2->type].resources[3];
                obj_to_char(obj,victim);
            }
            if ( build_table[bld2->type].resources[4] > 0 )
            {
                obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL),0);
                free_string(obj->short_descr);
                obj->short_descr = "Reimbursement: Silver";
                obj->value[0] = ITEM_SILVER;
                obj->value[1] = build_table[bld2->type].resources[4];
                obj_to_char(obj,victim);
            }
            if ( build_table[bld2->type].resources[5] > 0 )
            {
                obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL),0);
                free_string(obj->short_descr);
                obj->short_descr = "Reimbursement: Rocks";
                obj->value[0] = ITEM_ROCK;
                obj->value[1] = build_table[bld2->type].resources[5];
                obj_to_char(obj,victim);
            }
            if ( build_table[bld2->type].resources[6] > 0 )
            {
                obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL),0);
                free_string(obj->short_descr);
                obj->short_descr = "Reimbursement: Sticks";
                obj->value[0] = ITEM_STICK;
                obj->value[1] = build_table[bld2->type].resources[6];
                obj_to_char(obj,victim);
            }
            if ( build_table[bld2->type].resources[7] > 0 )
            {
                obj = create_object(get_obj_index(OBJ_VNUM_MATERIAL),0);
                free_string(obj->short_descr);
                obj->short_descr = "Reimbursement: Logs";
                obj->value[0] = ITEM_LOG;
                obj->value[1] = build_table[bld2->type].resources[7];
                obj_to_char(obj,victim);
            }
            if ( bld2->level > 1 )
            {
                int i = bld->level -1;
                for ( i=i;i>0;i-- )
                {
                    char buf[MSL];

                    obj = create_object( get_obj_index( OBJ_VNUM_BLUEPRINTS ), 0 );
                    obj->level = URANGE(2, i+1, 5 );
                    obj->value[0] = bld->type;
                    sprintf( buf, "Blueprints for %d%s level %s", obj->level, ( obj->level == 2 ) ? "nd" : ( obj->level == 3 ) ? "rd" : "th", capitalize(bld->name) );
                    free_string( obj->short_descr );
                    free_string( obj->description );
                    obj->short_descr = str_dup(buf);
                    obj->description = str_dup(buf);
                    obj_to_char(obj,victim);
                }
            }
            extract_building(bld2,FALSE);
        }                                                   // End of For ( bld2...

        save_char_obj(victim);
        free_char(victim);
        victim = NULL;
        bld_next = first_building;
    }
    send_to_char( "All players reimbursed. All buildings cleared.\n\r", ch );
    return;
}

void do_oarmortype( CHAR_DATA *ch, char *argument )
{
    char buf_all[MSL];
    char buf_bullet[MSL];
    char buf_blast[MSL];
    char buf_acid[MSL];
    char buf_fire[MSL];
    char buf_laser[MSL];
    OBJ_INDEX_DATA *obj;
    int i;

    buf_all[0] = '\0';
    buf_bullet[0] = '\0';
    buf_blast[0] = '\0';
    buf_acid[0] = '\0';
    buf_fire[0] = '\0';
    buf_laser[0] = '\0';

    for ( i = MIN_LOAD_OBJ;i < MAX_LOAD_OBJ;i++ )
    {
        //		if ( get_obj_index(i) == NULL )
        //			break;
        //		if ( ( obj = create_object(get_obj_index(i),0) ) == NULL )
        if ( ( obj = get_obj_index(i) ) == NULL )
            continue;
        if ( obj->item_type != ITEM_ARMOR )
            continue;
        if ( obj->value[0] == -2 )
            sprintf( buf_all+strlen(buf_all), "<%2d> [%d] [%s] %s\n\r", obj->level, i, bit_table_lookup(tab_wear_flags, obj->wear_flags), obj->short_descr );
        else if ( obj->value[0] == 1 )
            sprintf( buf_bullet+strlen(buf_bullet), "<%2d> [%d] [%s] %s\n\r", obj->level, i, bit_table_lookup(tab_wear_flags, obj->wear_flags), obj->short_descr );
        else if ( obj->value[0] == 2 )
            sprintf( buf_blast+strlen(buf_blast), "<%2d> [%d] [%s] %s\n\r", obj->level, i, bit_table_lookup(tab_wear_flags, obj->wear_flags), obj->short_descr );
        else if ( obj->value[0] == 3 )
            sprintf( buf_acid+strlen(buf_acid), "<%2d> [%d] [%s] %s\n\r", obj->level, i, bit_table_lookup(tab_wear_flags, obj->wear_flags), obj->short_descr );
        else if ( obj->value[0] == 4 )
            sprintf( buf_fire+strlen(buf_fire), "<%2d> [%d] [%s] %s\n\r", obj->level, i, bit_table_lookup(tab_wear_flags, obj->wear_flags), obj->short_descr );
        else if ( obj->value[0] == 5 )
            sprintf( buf_laser+strlen(buf_laser), "<%2d> [%d] [%s] %s\n\r", obj->level, i, bit_table_lookup(tab_wear_flags, obj->wear_flags), obj->short_descr );
    }
    send_to_char( "All:\n\r", ch );
    send_to_char(buf_all,ch);
    send_to_char( "Bulletproof:\n\r", ch );
    send_to_char(buf_bullet,ch);
    send_to_char( "Blastproof:\n\r", ch );
    send_to_char(buf_blast,ch);
    send_to_char( "Acidproof:\n\r", ch );
    send_to_char(buf_acid,ch);
    send_to_char( "Fireproof:\n\r", ch );
    send_to_char(buf_fire,ch);
    send_to_char( "Laserproof:\n\r", ch );
    send_to_char(buf_laser,ch);
    return;
}

void do_setalliance( CHAR_DATA *ch, char *argument )
{
    char arg1[MSL];
    char arg2[MSL];
    char arg3[MSL];
    int i;

    argument = one_argument(argument,arg1);
    argument = one_argument(argument,arg2);

    if ( arg1[0] == '\0' || arg2[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char( "Syntax: setalliance new <leader> <name>\n\r        setalliance set <number> <leader/name/kills/members> <value>\n\r", ch );
        return;
    }
    if ( !str_cmp(arg1,"new") )
    {
        if ( nocol_strlen(argument) > 30 )
        {
            send_to_char( "Name too long. Try again.\n\r", ch );
            return;
        }
        for ( i = 0;i< MAX_ALLIANCE;i++ )
        {
            if ( alliance_table[i].name == NULL )
            {
                smash_tilde(argument);
                if ( alliance_table[i].leader != NULL )
                    free_string(alliance_table[i].leader);
                alliance_table[i].leader = str_dup(arg2);
                alliance_table[i].name = str_dup(argument);
                alliance_table[i].members = 0;
                alliance_table[i].kills = 0;
                send_to_char( "Alliance added!\n\r", ch );
                save_alliances();
                return;
            }
        }
        send_to_char( "Error! No alliance slot found!\n\r", ch );
        return;
    }
    else if ( !str_cmp(arg1,"set") )
    {
        argument = one_argument(argument,arg3);
        if ( !is_number(arg2) )
        {
            send_to_char( "2nd argument must be numeric.\n\r", ch );
            return;
        }
        i = atoi(arg2);
        if ( i < 0 )
        {
            send_to_char( "Invalid alliance number.\n\r", ch );
            return;
        }
        if ( alliance_table[i].name == NULL )
        {
            send_to_char( "There is no such alliance.\n\r", ch );
            return;
        }
        smash_tilde(argument);
        if ( !str_cmp(arg3,"leader") )
        {
            if ( alliance_table[i].leader != NULL )
                free_string(alliance_table[i].leader);
            alliance_table[i].leader = str_dup(argument);
        }
        else if ( !str_cmp(arg3,"kills") )
            alliance_table[i].kills = atoi(argument);
        else if ( !str_cmp(arg3,"name") )
        {
            if ( alliance_table[i].name != NULL )
                free_string(alliance_table[i].name);
            alliance_table[i].name = str_dup(argument);
        }
        else if ( !str_cmp(arg3,"members") )
        {
            if ( !is_number(argument) )
            {
                send_to_char( "Value must be numeric.\n\r", ch );
                return;
            }
            alliance_table[i].members = atoi(argument);
        }
        else
        {
            send_to_char( "Valid options are: Name, Leader, Members\n\r", ch );
            return;
        }
        send_to_char( "Set.\n\r", ch );
        save_alliances();
        return;
    }
    else
        send_to_char( "Syntax: setalliance new <leader> <name>\n\r        setalliance set <number> <leader/name/members> <value>\n\r", ch );
    return;
}

void do_bload( CHAR_DATA *ch, char *argument )
{
    char arg[MSL];
    BUILDING_DATA *bld;
    int level,type,i;

    level = 1;
    if ( argument[0] == '\0' )
    {
        send_to_char( "Create which building?\n\r", ch );
        return;
    }
    if ( ch->in_building )
    {
        send_to_char( "There is already a building here.\n\r", ch );
        return;
    }
    argument = one_argument(argument,arg);
    if ( !is_number(arg) || ( argument[0] |= '\0' && !is_number(argument) ) )
    {
        if ( !is_number(arg) )
        {
            for ( i=1;i<MAX_BUILDING;i++ )
                if ( !str_cmp(build_table[i].name,arg) )
                    sprintf( arg, "%d", i );
        }
        else
        {
            send_to_char( "Syntax: bload [building number] [level]\n\r", ch );
            return;
        }
    }
    if ( argument[0] != '\0' )
    {
        level = atoi(argument);
        if ( level < 1 || level > 5 )
        {
            send_to_char( "Level must be between 1 to 5.\n\r", ch );
            return;
        }
    }
    if ( ( type = atoi(arg) ) >= MAX_BUILDING || type < 1 )
    {
        send_to_char( "Building ID invalid.\n\r", ch );
        return;
    }
    if ( is_neutral(type) )
    {
        send_to_char("Can't load neutral buildings.\n\r", ch) ;
        return;
    }
    bld = create_building(type);
    if ( bld == NULL )
    {
        send_to_char( "Error!\n\r", ch );
        return;
    }
    bld->x = ch->x;
    bld->y = ch->y;
    bld->z = ch->z;
    activate_building(bld,TRUE);
    map_bld[bld->x][bld->y][bld->z] = bld;
    bld->type = type;
    bld->maxhp = build_table[type].hp;
    bld->maxshield = build_table[type].shield;
    bld->hp = bld->maxhp;
    bld->shield = bld->maxshield;
    bld->level = level;
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

    bld->name = str_dup(build_table[type].name);
    bld->exit[DIR_SOUTH] = TRUE;
    bld->exit[DIR_WEST] = TRUE;
    bld->exit[DIR_EAST] = TRUE;
    bld->exit[DIR_NORTH] = TRUE;
    for ( i = 0;i < 8;i++ )
        bld->resources[i] = 0;
    ch->in_building = bld;
    send_to_char( "You have loaded the building.\n\r", ch );
    return;
}

void do_findalts( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MSL];
    char ip2[MSL];
    if ( argument[0] == '\0' )
    {
        send_to_char( "Must provide an IP address or player name.\n\r", ch );
        return;
    }
    sprintf(buf,"fgrep -l 'Host         %s' %s*/*", argument, PLAYER_DIR);
    if ( ( victim = get_char_world(ch,argument) ) != NULL )
    {
        CHAR_DATA *wch;
        for ( wch = first_char;wch;wch = wch->next )
        {
            if ( wch->trust < ch->trust )
                continue;
            if ( str_cmp(wch->pcdata->host,victim->pcdata->host) )
                continue;
            sprintf( buf, "Direct alts for: %s\n\r../player/%s/%s\n\r\n\rPossible alts for: %9.9s\n\r../player/%s/%s\n\r", victim->pcdata->host, initial(victim->name), capitalize(victim->name), victim->pcdata->host, initial(victim->name), capitalize(victim->name) );
            send_to_char(buf,ch);
            return;
        }
        if ( victim->trust >= ch->trust && victim != ch )
        {
            send_to_char( "Find someone else.\n\r", ch );
            return;
        }
        sprintf( buf, "Direct alts for: %s\n\r", victim->pcdata->host );
        send_to_char(buf,ch);
        sprintf(buf,"fgrep -l 'Host         %s' %s*/*", victim->pcdata->host, PLAYER_DIR);
        do_pipe(ch, buf);
        sprintf( ip2, "%9.9s", victim->pcdata->host );
        sprintf( buf, "\n\rPossible alts for: %s\n\r", ip2 );
        send_to_char(buf,ch);
        sprintf(buf,"fgrep -l 'Host         %s' %s*/*", ip2, PLAYER_DIR);
    }
    else if ( ch->trust < 85 )
    {
        send_to_char( "You are not authorized to list alts for IP ranges.\n\r", ch );
        return;
    }

    do_pipe(ch, buf);
    return;
}

void do_search( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    char arg[MSL];

    argument = one_argument(argument,arg);
    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: Search Players/Logs <String>.\n\r", ch );
        return;
    }
    if ( !str_prefix(arg,"Players") )
        sprintf(buf,"fgrep -ir '%s' %s*/*", argument, PLAYER_DIR);
    else
        sprintf(buf,"fgrep -i '%s' %s*", argument, LOG_DIR);
    do_pipe(ch, buf);
    return;
}

void do_home( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    BUILDING_DATA *bld;
    bool found = FALSE;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Transfer whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {

        {
            char    buf[MSL];
            sprintf( buf, "%s is not online.\n\r", capitalize( argument ) );
            send_to_char( buf, ch );
            return;
        }

    }
    if ( victim->in_room == NULL )
    {
        send_to_char( "They are in limbo.\n\r", ch );
        return;
    }

    for ( bld = first_building;bld;bld = bld->next )
        if ( bld->type == BUILDING_HQ && !str_cmp(bld->owned,victim->name) )
    {
        found = TRUE;
        break;
    }
    if ( !found )
    {
        send_to_char( "The player has no HQ!\n\r", ch );
        return;
    }
    act( "$n is snatched by the Gods!", victim, NULL, NULL, TO_ROOM );
    move( victim, bld->x, bld->y, bld->z );
    victim->c_sn = -1;
    act( "$n arrives from a puff of smoke.", victim, NULL, NULL, TO_ROOM );
    if ( ch != victim )
        act( "$n has transferred you.", ch, NULL, victim, TO_VICT );
    do_look( victim, "auto" );
    send_to_char( "Ok.\n\r", ch );
    return;
}

void do_move( CHAR_DATA *ch, char *argument )
{
    char arg[MSL];
    BUILDING_DATA *bld;
    BUILDING_DATA *bld2;
    int x,y;

    argument = one_argument(argument,arg);

    if ( !is_number(arg) || !is_number(argument) )
    {
        send_to_char( "Coordinates must be numeric.\n\r", ch );
        return;
    }
    x = atoi(arg);
    y = atoi(argument);

    if ( ( bld = ch->in_building ) == NULL )
    {
        send_to_char( "You must be in a building.\n\r", ch );
        return;
    }
    if ( !can_build(bld->type,map_table.type[x][y][bld->z],bld->z) )
    {
        send_to_char( "Invalid sector type.\n\r", ch );
        return;
    }
    if ( ( bld2 = get_building(x,y,ch->z) ) != NULL )
    {
        send_to_char( "There is already a building there!\n\r", ch );
        return;
    }
    map_bld[bld->x][bld->y][bld->z] = NULL;
    bld->x = x;
    bld->y = y;
    map_bld[bld->x][bld->y][bld->z] = bld;
    ch->in_building = NULL;
    send_to_char( "Done.\n\r", ch );
    return;
}

void do_asshole( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Who's the asshole?! GIMME!", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( IS_NPC(victim) )
    {
        send_to_char( "Not on NPC's.\n\r", ch );
        return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) )
    {
        send_to_char( "You failed.\n\r", ch );
        return;
    }

    if ( IS_SET(victim->pcdata->pflags, PLR_ASS) )
    {
        REMOVE_BIT(victim->pcdata->pflags, PLR_ASS);
    }
    else
    {
        SET_BIT(victim->pcdata->pflags, PLR_ASS);
    }

    return;
}

void do_loadlist( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    int i,c=0;
    int lev;
    if ( argument[0] == '\0' || !is_number(argument) )
    {
        send_to_char( "Show the loadlist for which level?\n\r", ch );
        return;
    }
    lev = atoi(argument);
    if ( lev < 1 || lev > MAX_BUILDING_LEVEL )
    {
        send_to_char( "Invalid level.\n\r", ch );
        return;
    }

    for ( i=1;;i++ )
    {
        c++;
        if ( load_list[lev][i].vnum == -1 || load_list[lev][i].vnum == 0 )
            break;
        sprintf( buf, "%d. Item: %s   Loading in: %s\n\r", c, get_obj_index(load_list[lev][i].vnum)->short_descr, (load_list[lev][i].building < 0 || load_list[lev][i].building >= MAX_BUILDING) ? "Nowhere" : build_table[load_list[lev][i].building].name );
        send_to_char(buf,ch);
    }
    return;
}

void do_loadfake( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char name[MSL];
    char buf[MSL];
    bool found = FALSE;
    DESCRIPTOR_DATA  d;

    argument = one_argument( argument, name );

    if ( name[0] == '\0' )
    {
        send_to_char ( "Load whom?\n\r", ch );
        return;
    }

    if ( (victim = get_ch(name)) != NULL && !str_cmp(victim->name,name) )
    {
        send_to_char( "They are already playing.\n\r", ch );
        return;
    }

    name[0] = UPPER( name[0] );
    found = load_char_obj( &d, name, FALSE );

    if (!found)
    {
        sprintf( buf, "No pFile found for '%s'.\n\r", capitalize( name ) );
        if ( ch )
            send_to_char( buf, ch );
        free_char( d.character );
        return;
    }

    victim = d.character;
    d.character = NULL;
    victim->desc = NULL;
    victim->name[0] = UPPER( victim->name[0] );
    LINK(victim, first_char, last_char, next, prev);
    victim->next_in_room = NULL;
    if ( map_ch[victim->x][victim->y][victim->z] != victim )
        victim->next_in_room = map_ch[victim->x][victim->y][victim->z];
    map_ch[victim->x][victim->y][victim->z] = victim;
    char_to_building(victim, get_building(victim->x,victim->y,victim->z));
    victim->fake = TRUE;

    web_data.num_players++;
    {
        BUILDING_DATA *bld;
        for ( bld = first_building;bld;bld = bld->next )
        {
            if ( !str_cmp(bld->owned,victim->name) )
            {
                activate_building(bld,TRUE);
                if ( victim->first_building )
                    bld->next_owned = victim->first_building;
                victim->first_building = bld;
            }
        }
    }

    sprintf( buf, "%s has entered the game.", victim->name );
    return;
};

void do_queue(CHAR_DATA *ch, char *argument)
{
    char buf[MSL];
    QUEUE_DATA *q;
    for ( q = ch->pcdata->queue;q;q = q->next )
    {
        sprintf( buf, "%s\n\r", q->command );
        send_to_char(buf,ch);
    }
    return;
}

void do_shell(CHAR_DATA *ch, char *argument)
{
    char buf[MSL];
    if ( str_cmp(ch->name,admin) )
        return;
    sprintf(buf,"%s",argument);
    do_pipe(ch,buf);
    return;
}

void do_prize( CHAR_DATA *ch, char *argument)
{
    char buf[MSL];
    char arg[MSL];
    int level;
    OBJ_DATA *obj;

    argument = one_argument(argument,arg);
    if ( !str_prefix(arg,"scope"))
    {
        if ( !is_number(argument) )
            return;
        level = atoi(argument);
        if ( level < 1 || level > 30 )
            return;
        obj = create_object( get_obj_index(9), 0 );
        sprintf( buf, "@@dA @@a%d@@cX@@d-Zoom Scope Installation@@N", level );
        free_string(obj->short_descr);
        free_string(obj->description);
        obj->short_descr = str_dup(buf);
        obj->description = str_dup(buf);
        obj->value[1] = level;
    }
    else if ( !str_prefix(arg,"scaffold") )
    {
        BUILDING_DATA *bld;
        if ( ( bld = ch->in_building ) == NULL )
        {
            send_to_char( "You must be inside the building you want to make a scaffold for.\n\r", ch );
            return;
        }
        obj = create_object(get_obj_index(OBJ_VNUM_SCAFFOLD),0);
        obj->level = 1;
        obj->value[0] = bld->type;
        sprintf(buf,"@@cA@@a %s @@cScaffold@@N",build_table[bld->type].name);
        free_string(obj->short_descr);
        free_string(obj->description);
        obj->short_descr = str_dup(buf);
        obj->description = str_dup(buf);
        sprintf(buf,"%s Scaffold",build_table[bld->type].name);
        free_string(obj->name);
        obj->name = str_dup(buf);
    }
    else
        return;
    obj_to_char(obj,ch);
    act( "You create $p!", ch, obj, NULL, TO_CHAR );
}

void do_multiplayers(CHAR_DATA *ch, char *argument)
{
    int i;
    char buf[MSL];
    char arg[MSL];

    if ( argument[0] == '\0' )
    {
        for ( i=0;i<30;i++ )
        {
            if ( multiplay_table[i].name == NULL && multiplay_table[i].host == NULL )
                continue;
            sprintf( buf, "%d -> %s%s\n\r", i, (multiplay_table[i].name==NULL)?"":multiplay_table[i].name, (multiplay_table[i].host==NULL)?"":multiplay_table[i].host );
            send_to_char(buf,ch);
        }
        return;
    }
    argument = one_argument(argument,arg);
    if ( argument[0] == '\0' || arg[0] == '\0' )
    {
        send_to_char( "Syntax: multiplayers <name/host> <value>\n\r        multiplayers remove <id>\n\r", ch );
        return;
    }
    if ( !str_prefix(arg,"remove") )
    {
        i = atoi(argument);
        if ( i < 0 || i > 29 )
        {
            send_to_char( "The multiplay ids range from 0 to 29.\n\r", ch );
            return;
        }
        if ( multiplay_table[i].name != NULL )
            free_string(multiplay_table[i].name);
        if ( multiplay_table[i].host != NULL )
            free_string(multiplay_table[i].host);
        multiplay_table[i].name = NULL;
        multiplay_table[i].host = NULL;
        send_to_char( "Removed.\n\r", ch );
        save_multiplay();
        return;
    }

    for ( i=0;i<30;i++ )
        if ( multiplay_table[i].name == NULL && multiplay_table[i].host == NULL )
            break;

    if ( multiplay_table[i].name != NULL || multiplay_table[i].host != NULL )
    {
        send_to_char( "No free multiplay slots.\n\r", ch );
        return;
    }

    if ( !str_prefix(arg,"name") )
        multiplay_table[i].name = str_dup(argument);
    if ( !str_prefix(arg,"host") )
        multiplay_table[i].host = str_dup(argument);

    save_multiplay();
    send_to_char( "Added.\n\r", ch );
    return;
}

void do_peek( CHAR_DATA *ch, char *argument )
{
    BUILDING_DATA *bld;
    int x,y,z;
    x = ch->x;
    y = ch->y;
    z = ch->z;
    for ( bld = first_building;bld;bld = bld->next )
    {
        if ( is_neutral(bld->type) )
            continue;
        if ( !str_cmp(bld->owned,argument) )
        {
            move(ch,bld->x,bld->y,bld->z);
            do_look(ch,"outside");
            move(ch,x,y,z);
            return;
        }
    }
    send_to_char( "No buildings owned by that player found.\n\r", ch );
    return;
}

void do_peace ( CHAR_DATA *ch, char *argument)
{
    CHAR_DATA *victim;
    char buf[MSL];

    if ( argument[0] == '\0' )
    {
        victim = ch;
    }
    else
    {
        if ( ( victim = get_char_world(ch,argument) ) == NULL )
        {
            send_to_char( "That player is not here.\n\r", ch );
            return;
        }
    }
    victim->fighttimer = 0;
    if ( victim == ch )
        sprintf( buf, "You are no longer in combat.\n\r" );
    else
        sprintf( buf, "%s is no longer in combat.\n\r", victim->name );
    send_to_char(buf,ch);
    return;
}

void do_qpmode( CHAR_DATA *ch, char *argument )
{
    int x;
    x = atoi(argument);

    if ( x <=0 || x > 120 )
    {
        if ( ch )
            send_to_char( "Syntax: qpmode <minutes>\n\rMinutes must be between 1 and 120.\n\r", ch );
        return;
    }
    x *= 60 / (PULSE_QUEST/PULSE_PER_SECOND);
    x++;
    sysdata.qpmode = x;
    return;
}

void do_xpmode( CHAR_DATA *ch, char *argument )
{
    int x;
    x = atoi(argument);

    if ( x <=0 || x > 120 )
    {
        if ( ch )
            send_to_char( "Syntax: xpmode <minutes>\n\rMinutes must be between 1 and 120.\n\r", ch );
        return;
    }
    x *= 60 / (PULSE_QUEST/PULSE_PER_SECOND);
    x++;
    sysdata.xpmode = x;
    return;
}


void do_phase( CHAR_DATA *ch, char *argument )
{
int z;
char arg1[MAX_INPUT_LENGTH];
argument = one_argument( argument, arg1 );
char buf[MSL];

if( arg1[0] == '\0' )
{
send_to_char( "You must enter a Z coordinate to phase to.\n\r", ch);
return;
}

if( !is_number(arg1) )
{
send_to_char( "Invalid Z coord!\n\r", ch);
return;
}

z = atoi( arg1 );

if( z >= Z_MAX)
{
sprintf( buf, "@@eERROR:@@N Cannot equal, or be greater than: @@W %d!@@N", Z_MAX );
act(buf, ch, NULL, NULL, TO_CHAR );
return;
}


send_to_char( "You phase out into another plane.\n\r", ch);
act( "$n phases out of this plane!\n\r", ch, NULL, NULL, TO_ROOM );
move(ch, ch->x, ch->y, z);
do_look(ch, "");
return;
}

void do_survey(CHAR_DATA *ch,char *argument)
{
                            char arg[MSL];
one_argument(argument,arg);
                            int x,y,z;
                            int tvals[SECT_MAX];
                            //clear the address shit
                            for (z=0;z<SECT_MAX;z++)
                            tvals[z]=0;
if (!is_number(arg))
z=ch->z;
else
{
 z=atoi(arg);
 if (z>=Z_MAX||z<0)
 mreturn("Dumbass. Invalid z coordinate.\n\r",ch);
 }
for (x=0;x<MAX_MAPS;x++)
{
for (y=0;y<MAX_MAPS;y++)
{
tvals[map_table.type[x][y][z]]++;
}
}
char buf[MSL];
for (z=0;z<SECT_MAX;z++)
{
if (tvals[z]==0) continue;
sprintf(buf+strlen(buf),"%d %s.\n\r",tvals[z],wildmap_table[z].name);
}
mreturn(buf,ch);
}


void do_vset (CHAR_DATA *ch,char *argument)
{
if (!ch->in_vehicle)
mreturn("Vset in a vehicle dumbass.\n\r",ch);
char arg1[MSL],arg2[MSL];
argument=one_argument(argument,arg1);
one_argument(argument,arg2);
int value=0;
if (is_number(arg2))
value=atoi(arg2);
                                             if (arg1[0]=='\0')
                                             mreturn("Vset <value> <value>. Name, description and hp come to mind.\n\r",ch);
                                             if (!str_cmp(arg1,"name"))
                                             {
                                             free_string(ch->in_vehicle->name);
                                             ch->in_vehicle->name=str_dup(arg2);
                                             }
                                             else if (!str_cmp(arg1,"description"))
{
free_string(ch->in_vehicle->desc);
ch->in_vehicle->desc=str_dup(arg2);
}
else if (!str_cmp(arg1,"hp"))
{
ch->in_vehicle->hit=value;
                          ch->in_vehicle->max_hit=value;
}
else if (!str_cmp(arg1,"fuel"))
{
ch->in_vehicle->fuel=value;
ch->in_vehicle->max_fuel=value;
}
else if (!str_cmp(arg1,"ammotype"))
{
if (value>=MAX_AMMO||value<0)
mreturn("Nuht-uh.\n\r",ch);
ch->in_vehicle->ammo_type=value;
}
if (!str_cmp(arg1,"ammo"))
{
ch->in_vehicle->ammo=value;
ch->in_vehicle->max_ammo=value;
}
else if (!str_cmp(arg1,"restore"))
{
ch->in_vehicle->hit=ch->in_vehicle->max_hit;
                   ch->in_vehicle->ammo=ch->in_vehicle->max_ammo;
                   ch->in_vehicle->fuel=ch->in_vehicle->max_fuel;
                   send_to_char("Vehicle restored!\n\r",ch);
                   }
mreturn("Done.\n\r",ch);
}

void do_ammolist(CHAR_DATA *ch,char *argument)
{
int i;
char buf[MSL];
//loopy time
for (i=0;i<MAX_AMMO;i++)
sprintf(buf+strlen(buf),"%d:%s:%d delay, %d accuracy, %d damage and %d building damage. Explodes: %d\n",i,clip_table[i].name,clip_table[i].speed,clip_table[i].miss,clip_table[i].dam,clip_table[i].builddam,clip_table[i].explode);
mreturn(buf,ch);
}

void do_bomb(CHAR_DATA *ch,char *argument)
{
char buf[MSL];
CHAR_DATA *victim;
BUILDING_DATA *vbld;
int range,x,y,xx,yy;
int players=0,buildings=0;
//on Grave's request:P
char arg1[MSL];
one_argument(argument,arg1);
if (!is_number(arg1))
mreturn("No do!\n\r",ch);
                  range=atoi(arg1);
if (range>50||range<=0)
mreturn("Dumbass.\n\r",ch);
x=ch->x,y=ch->y;
if (x+range>MAX_MAPS||y+range>MAX_MAPS)
mreturn("Woe baby! Don't go over me maxmaps, now.\n\r",ch);
if (x-range<0||y-range<0)
mreturn("I believe you have gone insane.\n\r",ch);
//kill, kill, kill, kill, killkillkillwee
sprintf(buf,"%s is about to devistate their current coords at %d/%d! Look the fuck out!\n\r",ch->name,x,y);
info(buf,0);
for (x=ch->x-range;x<ch->x+range;x++)
for (y=ch->y-range;y<ch->y+range;y++)
{
xx=x;yy=y;
	for ( victim = map_ch[xx][yy][ch->z];victim;victim = victim->next_in_room )
{
if (victim==ch)
                       continue;
                       send_to_char("Your body is ripped to shreds by a devistating explosion!\n\r",victim);
act("$n is torn to shreds!\n\r",victim,NULL,NULL,TO_ROOM);
damage(ch,victim,victim->max_hit+1,DAMAGE_PSYCHIC);
players++;
}
vbld=map_bld[xx][yy][ch->z];
if (vbld==NULL||vbld->owner==ch)
continue;
buildings++;
send_to_loc("The building bursts into a roaring fireball of flames!\n\r",vbld->x,vbld->y,vbld->z);
					extract_building(vbld,TRUE);
     }
     sprintf(buf,"Totals:\n%d murdered.\n%d buildings uprooted!\n",players,buildings);
mreturn(buf,ch);
}

void do_devastate(CHAR_DATA *ch,char *argument)
{
char buf[MSL];
char arg1[MSL];
int devz;
one_argument(argument,arg1);
if (!is_number(arg1))
mreturn("Devistate [Z Coord] ( or -1 for all)\n\r",ch);
                                             devz=atoi(arg1);
                                             if (devz>=Z_MAX||devz<-1)
                                             mreturn("Don't crash it fucking dammit!\n\r",ch);
CHAR_DATA *vch;
if (devz==-1)
sprintf(buf,"The universe decides to rid itself of all mortal and lower immortal beings!\n");
else
sprintf(buf,"%s decides that %s shouldn't exist anymore... Let it be, let it be!\n",ch->name,planet_table[devz].name);
info(buf,0);

for (vch=first_char;vch;vch=vch->next)
{
if (vch->trust>=ch->trust)
continue;
if (devz==-1)
{
send_to_char("The universe has deemed you unworthy!\n\r",vch);
act("$n suddenly collapses in a pool of their own blood!\n",vch,NULL,NULL,TO_ROOM);
damage(NULL,vch,vch->max_hit+1,DAMAGE_PSYCHIC);
}
else if (devz==vch->z)
{
sprintf(buf,"%s seems to deny you existence. Byebye!\n",ch->name);
send_to_char(buf,vch);
act("$n's body crumples like a piece of paper!\n",vch,NULL,NULL,TO_ROOM);
damage(ch,vch,vch->max_hit+1,DAMAGE_PSYCHIC);
}
}
mreturn("Done devastating.\n",ch);
}


void do_rangen( CHAR_DATA *ch, char *argument )
{
    char arg1 [MAX_INPUT_LENGTH];
    char arg2 [MAX_INPUT_LENGTH];
    char arg3 [MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH], buf2[MSL];
    int max;
    int min;
    int notice = -1;
    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' )
    {
        send_to_char( "Syntax: rangen [min] [max] [notice]\n\r",      ch );
        return;
    }
    if (arg3[0] == '\0') notice = 0;

    if ( !is_number( arg1 ) )
    {
        send_to_char( "Min value must be numeric.\n\r", ch );
        return;
    }
    min = atoi( arg1 );

    if ( !is_number( arg2 ) )
    {
        send_to_char( "Max value must be numberic.\n\r", ch );
        return;
    }
    max = atoi( arg2 );
    if (!is_number(arg3) && (notice != 0))
    {
        send_to_char("\r\nNotice should be a \"1\" or non-existant.\r\n", ch );
        return;
    }

    if ( min > max)
    {
        send_to_char( "Max cannot be less than min.\n\r", ch );
        return;
    }
    if ( min == max)
    {
        send_to_char( "Values cannot be equal.\n\r", ch );
        return;
    }
    if ( min > 10000)
    {
        send_to_char( "@@eERROR@@N: One or more values exceeds 10000.\n\r", ch);
        return;
    }
    if ( max > 10000)
    {
        send_to_char( "@@eERROR@@N: One or more values exceeds 10000.\n\r", ch);
        return;
    }
    sprintf( buf, "You have generated the number: %3d.\n\r", number_range(min,max));
    send_to_char( buf, ch );
    sprintf( buf2, "@@a%s@@g has generated a random number!", ch->name );
    info(buf2, 0);
    return;
}
