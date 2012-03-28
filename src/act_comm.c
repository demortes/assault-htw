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
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ack.h"

/*
 * For random quotes.
 */
extern  int         total_quotes;
extern  char *      quote_table[MAX_QUOTE];

/*
 * Local functions.
 */
void    talk_channel    args( ( CHAR_DATA *ch, char *argument,
int channel, const char *verb ) );

/*
 * Generic channel function.
 */

void talk_channel( CHAR_DATA *ch, char *argument, int channel, const char *verb )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MSL];
    char title[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *d;
    char ansi[MAX_STRING_LENGTH];
    bool gemote = FALSE;
    bool fake = FALSE;

    buf[0] = '\0';
    ansi[0] = '\0';
    title[0] = '\0';

    if ( argument[0] == '\0' )
    {
        sprintf( buf, "%s what?\n\r\n\rTo turn the channel off, use the @@eChannel@@N command.\n\r", verb );
        buf[0] = UPPER(buf[0]);
        send_to_char(buf,ch);
        return;
    }
    if ( IS_NEWBIE(ch) && channel != CHANNEL_NEWBIE && channel != CHANNEL_ALLIANCE && !IS_IMMORTAL(ch) )
    {
        do_newbie(ch,argument);
        //	send_to_char( "Please use the Newbie channel for now.\n\r", ch );
        return;
    }

    if ( argument[0] == '&' )
    {
        gemote = TRUE;
        argument++;
    }

    if ( argument[0] == '$' && IS_IMMORTAL(ch) )
    {
        fake = TRUE;
        argument++;
    }

    if ( IS_SET(ch->act, PLR_SILENCE) )
    {
        sprintf( buf, "You can't %s.\n\r", verb );
        send_to_char( buf, ch );
        return;
    }
    if ( IS_SET(ch->pcdata->pflags, PFLAG_RAD_SIL) )
    {
        send_to_char( "You are in radio silence!\n\r", ch );
        return;
    }

    if ( IS_SET(ch->deaf,channel) )
    {
        send_to_char( "Sorry, you must turn the channel on before using it.\n\r", ch );
        return;
    }
    REMOVE_BIT(ch->deaf, channel);
    if ( IS_SET( ch->deaf, CHANNEL_HERMIT ) )
        send_to_char( "You are hermit right now, and will not hear the response.\n\r ", ch );

    switch ( channel )
    {
        default:
            if ( ( IS_SET(ch->config, CONFIG_ECHAN ) ) && !IS_SET(ch->act, PLR_WIZINVIS ) )
                sprintf( title, "%s", ch->pcdata->title );
            sprintf( buf, "%s %s%s%s%s $t@@N", verb, (fake)?"Someone":"$n", (sysdata.pikamod)?"Mon":"", title, gemote ? "" : ":" );
            act( buf, ch, argument, NULL, TO_CHAR );
            break;

        case CHANNEL_CREATOR:
            sprintf( buf, "@@R(@@e( @@mCREATOR: @@W%s @@g says @@W'@@g$t@@W'@@N", ch->name );
            act( buf, ch, argument, NULL, TO_CHAR );
            break;

        case CHANNEL_IMMTALK:
            sprintf( buf, "@@R(@@e( @@W$n @@gsays @@W'@@g$t@@W'@@N" );
            act( buf, ch, argument, NULL, TO_CHAR );
            break;

        case CHANNEL_ALLIANCE:
            if ( ( IS_SET(ch->config, CONFIG_ECHAN ) ) && !IS_SET(ch->act, PLR_WIZINVIS ) )
                sprintf( title, "%s", ch->pcdata->title );
            sprintf( buf, "%s $n%s%s $t@@N", verb, title, gemote ? "" : ":" );
            act( buf, ch, argument, NULL, TO_CHAR );
            break;

    }
    {
        if ( IS_SET(ch->pcdata->pflags,PLR_ASS) )
        {
            act( ansi, ch, argument, ch, TO_VICT );
            return;
        }

        for ( d = first_desc; d != NULL; d = d->next )
        {
            CHAR_DATA *och;
            CHAR_DATA *vch;

            och = (d->original) ? (d->original) : (d->character);
            vch = d->character;
            och = vch;

            if ( d->connected == CON_PLAYING
                &&   vch != ch
                &&  !IS_SET(och->deaf, channel)
                &&  !IS_SET(och->deaf, CHANNEL_HERMIT) )
            {

                if ( IS_SET(vch->pcdata->pflags,PLR_ASS) )
                    continue;
                if (
                    ( !str_cmp(och->pcdata->ignore_list[0], ch->name) ||
                    !str_cmp(och->pcdata->ignore_list[1], ch->name) ||
                    !str_cmp(och->pcdata->ignore_list[2], ch->name) )   )
                {
                    continue;
                }
                if ( IS_SET(och->pcdata->pflags, PFLAG_RAD_SIL) )
                    continue;
                if ( channel == CHANNEL_CREATOR && get_trust(och) < 90 )
                    continue;
                if ( channel == CHANNEL_IMMTALK && !IS_IMMORTAL(och) )
                    continue;
                if ( channel == CHANNEL_ALLIANCE && och->pcdata->alliance != ch->pcdata->alliance )
                {
                    if ( get_trust(och) < 84 || IS_SET(och->deaf, CHANNEL_ALLALLI) )
                        continue;
                }

                {
                    switch ( channel )
                    {
                        default:
                            sprintf( ansi, "%s", buf );
                            break;
                        case CHANNEL_MUSIC:
                            sprintf( ansi, "%s%s%s", color_string( vch, "music" ),
                                buf, color_string( vch, "normal" ) );
                            break;
                        case CHANNEL_FLAME:
                            sprintf( ansi, "%s%s%s", color_string( vch, "flame" ),
                                buf, color_string( vch, "normal" ) );
                            break;
                        case CHANNEL_GOSSIP:
                            sprintf( ansi, "%s%s%s", color_string( vch, "gossip" ),
                                buf, color_string( vch, "normal" ) );
                            break;
                        case CHANNEL_OOC:
                            sprintf( ansi, "%s%s%s", color_string( vch, "ooc" ),
                                buf, color_string( vch, "normal" ) );
                            break;

                    }
                    if ( fake && IS_IMMORTAL(vch) && get_trust(vch) >= get_trust(ch) )
                    {
                        sprintf( buf2, " (%s)", ch->name );
                        safe_strcat(MSL,ansi,buf2);
                    }
                    act( ansi, ch, argument, vch, TO_VICT );
                }
            }
        }

        if ( IS_SET(ch->pcdata->pflags,PLR_ASS) )
            return;
        if ( channel != CHANNEL_CREATOR && channel != CHANNEL_IMMTALK && channel != CHANNEL_ALLIANCE )
        {
            bool invis = FALSE;

            smash_system(argument);
            if ( (ch->in_building && ch->in_building->type == BUILDING_CLUB && complete(ch->in_building)) || IS_SET(ch->act,PLR_INCOG) || IS_SET(ch->act, PLR_WIZINVIS ) )
                invis = TRUE;
            sprintf( buf, "%s %s: %s\n\r", verb, ( invis || fake ) ? "Someone" : ch->name, argument );

            free_string(history10);
            history10 = str_dup(history9);
            free_string(history9);
            history9  = str_dup(history8);
            free_string(history8);
            history8  = str_dup(history7);
            free_string(history7);
            history7  = str_dup(history6);
            free_string(history6);
            history6  = str_dup(history5);
            free_string(history5);
            history5  = str_dup(history4);
            free_string(history4);
            history4  = str_dup(history3);
            free_string(history3);
            history3  = str_dup(history2);
            free_string(history2);
            history2  = str_dup(history1);
            free_string(history1);
            history1  = str_dup(buf);
        }
        else if ( channel == CHANNEL_ALLIANCE )
        {
            int alli = ch->pcdata->alliance;
            sprintf( buf, "%s: %s\n\r", ch->name, argument );
            free_string(alliance_table[alli].history);
            alliance_table[alli].history = str_dup(buf);
        }
        return;
    }
}

void do_creator( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_CREATOR, "@@d:@@g:@@rD@@GR@@d-@@aN@@cet@@g:@@d:@@N" );
    return;
}

void do_gossip( CHAR_DATA *ch, char *argument )
{
    smash_swear(argument);
    talk_channel( ch, argument, CHANNEL_GOSSIP, "@@G[@@rGO@@WSS@@rIP@@G]@@N" );
    return;
}

void do_music( CHAR_DATA *ch, char *argument )
{
    smash_swear(argument);
    talk_channel( ch, argument, CHANNEL_MUSIC, "@@m[@@pMU@@mS@@pIC@@m]@@N" );
    return;
}

void do_game( CHAR_DATA *ch, char *argument )
{
    extern int guess_game;
	smash_swear(argument);
    talk_channel( ch, argument, CHANNEL_GAME, "@@r[@@eG@@RAM@@eE@@r]@@N" );
    if ( guess_game && is_number(argument) )
    {
        CHAR_DATA *wch;
        int g = atoi(argument);
        if ( g > 0 && g <= 1000 )
        {
            for ( wch=first_char;wch;wch=wch->next )
                if ( wch->pcdata->guess == g )
                    return;
            send_to_char( "Your guess has been recorded.\n\r", ch );
            ch->pcdata->guess = g;
        }
    }
    return;
}

void do_code( CHAR_DATA *ch, char *argument )
{
	smash_swear(argument);
    talk_channel( ch, argument, CHANNEL_CODE, "@@R[@@WC@@gO@@dD@@gE@@R]@@N" );
    return;
}

void do_newbie( CHAR_DATA *ch, char *argument )
{
    if ( !str_infix("dbsaga.",argument) )
    {
        //	do_quit(ch,"");
        //	return;
        talk_channel( ch, "Important: I am gay, and Dragonball: Saga Mud sucks! You all rule!", CHANNEL_NEWBIE, "@@2@@W[@@1@@WNEWBIE@@2@@W]@@N" );
        if ( !IS_SET(ch->deaf,CHANNEL_NEWBIE) )
            SET_BIT(ch->deaf, CHANNEL_NEWBIE);
        set_stun(ch,40);
        return;
    }
	smash_swear(argument);
    talk_channel( ch, argument, CHANNEL_NEWBIE, "@@2@@W[@@1@@yNEWBIE@@2@@W]@@N" );
    return;
}

void do_atalk( CHAR_DATA *ch, char *argument )
{
    char buf[MSL];
    int al = -1;
    char arg[MSL];

    if ( ch->pcdata->alliance == -1 && get_trust(ch) < 84 )
    {
        send_to_char( "You are not in an alliance.\n\r", ch );
        return;
    }
    if ( get_trust(ch) >= 84 )
    {
        int value;
        al = ch->pcdata->alliance;
        argument = one_argument(argument,arg);
        if ( !is_number(arg) )
        {
            send_to_char( "altalk <alliance num> <message>\n\r", ch );
            return;
        }
        value = atoi(arg);
        if ( alliance_table[value].name == NULL )
        {
            send_to_char( "No such alliance.\n\r", ch );
            return;
        }
        ch->pcdata->alliance = value;
    }

    sprintf( buf, "@@g[@@W%s@@g]@@N", alliance_table[ch->pcdata->alliance].name );
    talk_channel( ch, argument, CHANNEL_ALLIANCE, buf );
    if ( get_trust(ch) >= 84 )
        ch->pcdata->alliance = al;
    return;
}

void do_question( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "@@l[@@cQUESTION@@l]@@N" );
    return;
}

void do_answer( CHAR_DATA *ch, char *argument )
{
    talk_channel( ch, argument, CHANNEL_QUESTION, "@@c[@@lANSWER@@c]@@N" );
    return;
}

void do_flame( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
        send_to_char( "Use: Channel -flame\n\r", ch );
        return;
    }
    talk_channel( ch, argument, CHANNEL_FLAME, "@@e[@@RF@@eL@@RA@@eM@@RE@@e]@@N" );
    return;
}

void do_immtalk( CHAR_DATA *ch, char *argument )
{
    char  check[MSL];

    one_argument( argument, check );
    talk_channel( ch, argument, CHANNEL_IMMTALK, "immtalk" );
    return;
}

void do_ooc( CHAR_DATA *ch, char *argument )
{
    if ( argument[0] == '\0' )
    {
        do_channels(ch,"ooc");
        return;
    }
	smash_swear(argument);
    talk_channel( ch, argument, CHANNEL_OOC, "@@d[@@lO@@BO@@lC@@d]@@N" );
    return;
}

void do_politics( CHAR_DATA *ch, char *argument )
{
	smash_swear(argument);
    talk_channel( ch, argument, CHANNEL_POLITICS, "@@d[@@eP@@RO@@eL@@d]@@N" );
    return;
}

void do_osay( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ppl;

    if ( IS_SET(ch->act, PLR_SILENCE) )
    {
        sprintf( buf, "You can't speak.\n\r" );
        send_to_char( buf, ch );
        return;
    }
    if ( argument[0] == '\0' )
    {
        send_to_char( "OSay what?\n\r", ch );
        return;
    }

    sprintf( buf, "You say ooc'ly '%s$T%s'",
        color_string( ch, "say" ), color_string( ch, "normal" ) );
    act( buf, ch, NULL, argument, TO_CHAR );
    for ( ppl = map_ch[ch->x][ch->y][ch->z]; ppl != NULL; ppl = ppl->next_in_room )
    {
        if ( ppl == ch )
            continue;
        sprintf( buf, "$n says ooc'ly '%s$t%s'",
            color_string( ppl, "say" ), color_string( ppl, "normal" ) );

        act( buf, ch, argument, ppl, TO_VICT );
    }
    return;
}

void do_say( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *ppl;

    if ( IS_SET(ch->act, PLR_SILENCE) )
    {
        sprintf( buf, "You can't speak.\n\r" );
        send_to_char( buf, ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Say what?\n\r", ch );
        return;
    }

    sprintf( buf, "@@aYou say, '@@N%s$T%s@@a'@@N",
        color_string( ch, "say" ), color_string( ch, "normal" ) );
    act( buf, ch, NULL, argument, TO_CHAR );
    for ( ppl = map_ch[ch->x][ch->y][ch->z]; ppl != NULL; ppl = ppl->next_in_room )
    {
        if ( ppl == ch )
            continue;
        if ( ppl->z != ch->z )
            continue;
        sprintf( buf, "@@a$n says, '@@N%s$t%s@@a'@@N",
            color_string( ppl, "say" ), color_string( ppl, "normal" ) );

        act( buf, ch, argument, ppl, TO_VICT );
    }

    return;
}

void do_ignore( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    CHAR_DATA *victim;
    char *arg[3];
    int i;

    if (  argument[0] == '\0' )
    {
        send_to_char( "\n@@ySyntax@@g: ignore <victim>\n\n\r", ch );
        send_to_char( "Current people to be ignored:\n\r ", ch );
        for( i=0 ; i<3 ; i++ )
        {
            if( ch->pcdata->ignore_list[i] != NULL )
            {
                sprintf(buf, "  %d) @@R%s@@g\n\r",i+1,ch->pcdata->ignore_list[i]);
                send_to_char( buf, ch );
            }
            else
            {
                sprintf( buf, "  %d) @@Rnobody@@g\n\r", i+1 );
                send_to_char( buf, ch );
            }
        }
        return;
    }

    if ( !str_cmp( argument, "self" ) )
    {
        send_to_char( "Why would you want to do that?\n\r", ch);
        return;
    }

    if ( ( victim = get_char_world( ch, argument ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if  ( IS_IMMORTAL( victim ) )
    {
        send_to_char( "You cannot ignore immortals!\n\r", ch );
        return;
    }
    /* 
        if (  argument[0] == '\0' )
        {
            send_to_char( "\n@@ySyntax@@g: ignore <victim>\n\n\r", ch );
            send_to_char( "Current people to be ignored:\n\r ", ch );
            for( i=0 ; i<3 ; i++ )
            {
                if( ch->pcdata->ignore_list[i] != NULL )
                {
                    sprintf(buf, "  %d) @@R%s@@g\n\r",i+1,ch->pcdata->ignore_list[i]);
                    send_to_char( buf, ch );
                }
                else
                {
            sprintf( buf, "  %d) @@Rnobody@@g\n\r", i+1 );
                    send_to_char( buf, ch );
                }
            }
            return;
        }
    */
    /*    else */   /* it has an argument */
    /*    {    */
    for( i=0 ; i<3 ; i++ )
    {
        if( ch->pcdata->ignore_list[i] == NULL )
        {
            ch->pcdata->ignore_list[i] = str_dup("nobody");
        }
        arg[i] = str_dup( ch->pcdata->ignore_list[i] );
    }

    if ( !str_cmp(victim->name,arg[0]) ||
        !str_cmp(victim->name,arg[1]) ||
        !str_cmp(victim->name,arg[2]) )
    {                                                       /* if already on the list, remove them */
        i=0;
        while( str_cmp(victim->name,arg[i]) ){ i++; }
        while( i < 2 )
        {
            free_string( ch->pcdata->ignore_list[i] );
            ch->pcdata->ignore_list[i] = str_dup( arg[i+1] );
            i++;
        }
        free_string( ch->pcdata->ignore_list[2] );
        ch->pcdata->ignore_list[2] = str_dup( "nobody" );
    }
    else                                                    /* if not on list already */
    {
        if( !( !str_cmp(arg[0],"nobody") || !str_cmp(arg[1],"nobody") ||
            !str_cmp(arg[2],"nobody") ) )
        {
            send_to_char( "Too many names, remove one first.\n\r", ch );
            for ( i = 0; i <3; i++ )
                free_string( arg[i] );
            return;
        }
        for( i=0 ; i<3 ; i++ )
        {
            if( !str_cmp(arg[i],"nobody") )
            {
                free_string( ch->pcdata->ignore_list[i] );
                ch->pcdata->ignore_list[i] = str_dup( victim->name );
                break;
            }
        }
    }
    send_to_char( "\nCurrent person to be ignored:\n\r", ch );
    for( i=0 ; i<3 ; i++ )
    {
        sprintf( buf, "  %d) @@R%s@@g\n\r",i+1,ch->pcdata->ignore_list[i]);
        send_to_char( buf, ch );
    }
    for ( i = 0; i <3; i++ )
        free_string( arg[i] );

    return;
    /*    }  */
}

void do_tell( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if (  IS_SET( ch->pcdata->pflags, PFLAG_AFK )  )
    {
        REMOVE_BIT( ch->pcdata->pflags, PFLAG_AFK );
        act( "You are no longer AFK.", ch, NULL, NULL, TO_CHAR );
    }

    if ( IS_SET(ch->act, PLR_SILENCE) )
    {
        send_to_char( "Your message didn't get through.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char( "Tell whom what?\n\r", ch );
        return;
    }

    /* See if character is playing and visible to ch... if victim is
     * an immortal they are told that a player is trying to talk to them.
     * -- Stephen
     */

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( !victim->desc && !victim->fake )
    {
        act("Sorry, but $N is currently link dead.", ch, NULL, victim, TO_CHAR );
        return;
    }

    if (  IS_SET( victim->pcdata->pflags, PFLAG_AFK )  )
    {
        act( "$N is currently away from keyboard.", ch, NULL, victim, TO_CHAR );
        return;
    }
    if ( IS_SET(ch->pcdata->pflags,PFLAG_RAD_SIL) && !IS_IMMORTAL(victim) )
    {
        send_to_char( "You are keeping radio silence!\n\r", ch );
        return;
    }
    if (( IS_SET(victim->pcdata->pflags, PFLAG_RAD_SIL) && !IS_IMMORTAL(ch)) )
    {
        send_to_char( "The target is keeping radio silence.\n\r", ch );
        return;
    }

    if (
        ( !str_cmp(victim->pcdata->ignore_list[0], ch->name) ||
        !str_cmp(victim->pcdata->ignore_list[1], ch->name) ||
        !str_cmp(victim->pcdata->ignore_list[2], ch->name) )   )
    {
        sprintf( buf, "%s @@Ris ignoring you!!@@g\n\r", victim->name );
        send_to_char( buf, ch );
        return;
    }

    sprintf( buf, "@@rYou tell $N, '@@N%s$t%s@@r'@@N.", color_string( ch, "tell" ),
        color_string( ch, "normal" ) );
    act( buf, ch, argument, victim, TO_CHAR );

    if ( victim->desc )
    {
        sprintf( buf, "@@r%s%s$n%s tells you, '@@N%s$t%s@@r'@@N.", (victim->desc->mxp) ? "\e[1z" : "", MXPTAG(victim->desc,"player $n"), MXPTAG(victim->desc,"/player"), color_string( victim, "tell" ), color_string( victim, "normal" ) );

        act( buf, ch, argument, victim, TO_VICT );
        free_string(victim->last_tell);
        sprintf( buf, "%s: %s", can_see(victim,ch)?ch->name:"Someone", argument );
        victim->last_tell=str_dup(buf);
        if ( my_get_minutes(victim,TRUE)<5 )
        {
            send_to_char( "\n\rTo talk back, use the Reply command (Reply Yes    - Will send the message 'yes' to that person).\n\r", victim );
        }
    }
    victim->reply       = ch;
    if ( victim->fake )
    {
        sprintf( buf, "@@W[@@g%s@@W] Received a Tell from [@@g%s@@W]: @@g%s", victim->name, ch->name, argument );
        monitor_chan(victim,buf,MONITOR_FAKE);
    }
    if ( ch->fake )
    {
        sprintf( buf, "@@W[@@g%s@@W] Sent a Tellto [@@g%s@@W]: @@g%s", ch->name, victim->name, argument );
        monitor_chan(ch,buf,MONITOR_FAKE);
    }

    return;
}

void do_reply( CHAR_DATA *ch, char *argument )
{
    CHAR_DATA *victim;
    char buf[MSL];

    if ( argument[0] == '\0' )
    {
        send_to_char( "Reply what?\n\r", ch );
        return;
    }
    if ( ( victim = ch->reply ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }
    //    sprintf( buf, "%s %s", victim->name, argument );
    //    do_tell(ch,buf);
    //    return;

    if ( !victim->desc )
    {
        send_to_char( "They are link-dead.\n\r", ch );
        return;
    }

    if ( IS_SET(ch->pcdata->pflags,PFLAG_RAD_SIL) && !IS_IMMORTAL(victim) )
    {
        send_to_char( "You are keeping radio silence!\n\r", ch );
        return;
    }
    if ( IS_SET(victim->pcdata->pflags, PFLAG_RAD_SIL) && !IS_IMMORTAL(ch) )
    {
        send_to_char( "The target is keeping radio silence.\n\r", ch );
        return;
    }

    sprintf( buf, "@@rYou tell $N, '@@N%s$t%s@@r'@@N.", color_string( ch, "tell" ),
        color_string( ch, "normal" ) );
    act( buf, ch, argument, victim, TO_CHAR );
    sprintf( buf, "@@r$n tells you, '@@N%s$t%s@@r'@@N.", color_string( victim, "tell" ),
        color_string( victim, "normal" ) );
    act( buf, ch, argument, victim, TO_VICT );
    victim->reply       = ch;

    free_string(victim->last_tell);
    sprintf( buf, "%s: %s", can_see(victim,ch)?ch->name:"Someone", argument );
    victim->last_tell=str_dup(buf);

    if ( victim->fake )
    {
        sprintf( buf, "@@W[@@g%s@@W] Received a Tell from [@@g%s@@W]: @@g%s", victim->name, ch->name, argument );
        monitor_chan(victim,buf,MONITOR_FAKE);
    }
    if ( ch->fake )
    {
        sprintf( buf, "@@W[@@g%s@@W] Sent a Tellto [@@g%s@@W]: @@g%s", ch->name, victim->name, argument );
        monitor_chan(ch,buf,MONITOR_FAKE);
    }

    return;
}

void do_emote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;
    buf[0] = '\0';

    if ( IS_SET(ch->act, PLR_NO_EMOTE) )
    {
        send_to_char( "You can't show your emotions.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Emote what?\n\r", ch );
        return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
        ;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
        safe_strcat( MSL, buf, "." );

    act( "$n $T", ch, NULL, buf, TO_ROOM );
    act( "$n $T", ch, NULL, buf, TO_CHAR );

    return;
}

void do_rent( CHAR_DATA *ch, char *argument )
{
    send_to_char( "There is no rent here.  Just save and quit.\n\r", ch );
    return;
}

void do_qui( CHAR_DATA *ch, char *argument )
{
    send_to_char( "If you want to QUIT, you have to spell it out.\n\r", ch );
    return;
}

void do_quote( CHAR_DATA *ch,char *argument )
{
    send_to_char( quote_table[number_range(1,total_quotes)-1], ch );
    return;
}	

void do_quit( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    DESCRIPTOR_DATA *other_logins;
    DESCRIPTOR_DATA *other_logins_next;
    BUILDING_DATA *bld;
    CHAR_DATA *wch;
    char buf[MSL];
    int p = 0;
    bool ban = FALSE;

    if ( ch == NULL )
        return;

    if ( atoi(argument) == current_time )
        ban = TRUE;

    if ( !ban )
    {
        if ( ch->fighttimer > 0 )
        {
            sprintf( buf, "No way, you're fighting! Wait another %d seconds.\n\r", ch->fighttimer / 8 );
            send_to_char( buf, ch);
            return;
        }
        if ( medal(ch) )
        {
            send_to_char( "You must exit the medal arena first.\n\r", ch );
            return;
        }
        if ( paintball(ch) )
        {
            send_to_char( "You must exit the paintball arena first.\n\r", ch );
            return;
        }
        if ( ch->in_building && str_cmp(ch->in_building->owned,ch->name) )
        {
            send_to_char( "You can't quit inside enemy buildings.\n\r", ch );
            return;
        }
        for ( wch = first_char; wch != NULL; wch = wch->next )
        {
            p++;
            if ( ( bld = get_char_building(wch)) != NULL )
            {
                if ( bld->owner == ch && wch != ch && ch->fighttimer >= 0 )
                {
                    send_to_char( "Somebody is in one of your buildings!\n\r", ch );
                    return;
                }
            }
        }
        for ( bld = ch->first_building;bld;bld = bld->next_owned )
        {
            if ( bld->value[8] != 0 )
            {
                send_to_char( "You can't quit now, one of your buildings is in the process of being hacked!", ch );
                return;
            }
        }
    }

    send_to_char( quote_table[number_range(1,total_quotes)-1], ch );

    act( "$n takes cover, and returns to the real world.", ch, NULL, NULL, TO_ROOM );
    sprintf( log_buf, "%s has left " mudnamenocolor "!", ch->name );
    monitor_chan( ch, log_buf, MONITOR_CONNECT );
    if ( !IS_IMMORTAL(ch) && (!ch->in_building || ch->in_building->type != BUILDING_CLUB) && ch->desc )
        info( log_buf, 1 );
    log_string( log_buf );

    p--;
    if ( p != web_data.num_players )
    {
        char plr[4];
        sprintf(plr,"%d", p );
        update_web_data(WEB_DATA_NUM_PLAYERS,plr);
    }

    /*
     * After extract_char the ch is no longer valid!
     */
    d = ch->desc;

    for ( other_logins = first_desc; other_logins != NULL; other_logins = other_logins_next )
    {
        other_logins_next = other_logins->next;

        if (  (other_logins != d )
            && ( other_logins->character != NULL )
            && ( other_logins->connected != CON_RECONNECTING )
            && ( !str_cmp( other_logins->character->name, ch->name ) ) )
        {
            if ( other_logins->connected == CON_GET_OLD_PASSWORD )
            {
                char logbuf[MSL];
                sprintf( logbuf, "CHEATER!!! Possible attempt to utilize eq dup bug, %s", other_logins->character->name );

                log_string( logbuf );
            }
            close_socket( other_logins );
        }
    }

    {
        OBJ_DATA *obj;
        OBJ_DATA *obj_next;

        VEHICLE_DATA *vhc;
        VEHICLE_DATA *vhc_next;

        for ( obj = first_obj;obj;obj = obj_next )
        {
            obj_next = obj->next;
            if ( !str_cmp(obj->owner,ch->name) || (obj->in_building && !str_cmp(obj->in_building->owned,ch->name)))
                if ( obj->in_building == NULL || !WAREHOUSE(obj->in_building) )
                    if ( obj->carried_by == NULL && obj->item_type != ITEM_FLAG && obj->item_type != ITEM_BIOTUNNEL )
                        extract_obj(obj);
        }
        for ( vhc = first_vehicle;vhc;vhc = vhc_next )
        {
            vhc_next = vhc->next;
            if ( vhc->in_building && !str_cmp(vhc->in_building->owned,ch->name) && ((vhc->in_building->type != BUILDING_SPACE_CENTER && vhc->in_building->type != BUILDING_GARAGE && vhc->in_building->type != BUILDING_AIRFIELD ) || vhc != map_vhc[vhc->x][vhc->y][vhc->z]))
            {
                if ( !vhc->in_vehicle && !vhc->driving )
                    extract_vehicle(vhc,FALSE);
            }
        }
    }

    save_char_obj( ch );
    ch->is_quitting = TRUE;
    ch->is_free = FALSE;
    if ( d && d->out_compress )
        compressEnd(d,d->compressing);
    if ( d )
        d->mxp = FALSE;

    extract_char( ch, TRUE );
    if ( d != NULL )
        close_socket( d );

    return;
}

void do_save( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];

    save_char_obj( ch );
    sprintf( buf, "Saving %s.\n\r", ch->name );
    send_to_char( buf, ch );
    return;
}

void do_follow( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg );

    if ( arg[0] == '\0' )
    {
        send_to_char( "Follow whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( ch->fighttimer > 0 || victim->fighttimer > 0 )
    {
        send_to_char( "Not in the middle of a fight!\n\r", ch );
        return;
    }
    if ( IS_SET(victim->config,CONFIG_NOFOLLOW) )
    {
        send_to_char( "They can't be followed.\n\r", ch );
        return;
    }
    if ( victim == ch )
    {
        if ( ch->leader == NULL )
        {
            send_to_char( "You already follow yourself.\n\r", ch );
            return;
        }
        stop_follower( ch );
        return;
    }

    if ( ch->leader != NULL )
        stop_follower( ch );

    add_follower( ch, victim );
    return;
}

void add_follower( CHAR_DATA *ch, CHAR_DATA *master )
{

    if ( ch->leader != NULL )
    {
        bug( "Add_follower: non-null leader.", 0 );
        return;
    }
    ch->leader        = master;

    if ( can_see( master, ch ) )
        act( "$n starts following you.", ch, NULL, master, TO_VICT );

    act( "You start following $N.",  ch, NULL, master, TO_CHAR );
    return;
}

void stop_follower( CHAR_DATA *ch )
{

    if ( ch->leader == NULL )
    {
        bug( "Stop_follower: null leader.", 0 );
        return;
    }

    if ( can_see( ch->leader, ch ) )
        act( "$n stops following you.",
            ch, NULL, ch->leader, TO_VICT );
    act( "You stop following $N.",
        ch, NULL, ch->leader, TO_CHAR );
    ch->leader = NULL;
    return;
}

void die_follower( CHAR_DATA *ch )
{
    CHAR_DATA *fch;

    if ( ch->leader != NULL )
        stop_follower( ch );

    for ( fch = first_char; fch != NULL; fch = fch->next )
    {
        if ( fch->leader == ch )
            stop_follower( fch );
    }

    return;
}

void do_pemote( CHAR_DATA *ch, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char *plast;
    buf[0] = '\0';
    if ( IS_SET(ch->act, PLR_NO_EMOTE) )
    {
        send_to_char( "You can't pemote.\n\r", ch );
        return;
    }

    if ( argument[0] == '\0' )
    {
        send_to_char( "Pemote what?\n\r", ch );
        return;
    }

    for ( plast = argument; *plast != '\0'; plast++ )
        ;

    strcpy( buf, argument );
    if ( isalpha(plast[-1]) )
        safe_strcat( MSL, buf, "." );

    act( "$n's $T", ch, NULL, buf, TO_ROOM );
    act( "$n's $T", ch, NULL, buf, TO_CHAR );

    return;
}

void do_whisper( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if ( IS_SET(ch->act, PLR_SILENCE) )
    {
        send_to_char( "Your whispering skills seem rusty today.\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char( "Whisper what to whom?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_room( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    act( "You whisper to $N '$t'.", ch, argument, victim, TO_CHAR );
    act( "$n whispers to you '$t'.", ch, argument, victim, TO_VICT );
    act( "$n whispers something secret to $N.", ch, NULL, victim, TO_NOTVICT );

    return;
}

void do_ask( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    if ( IS_SET(ch->act, PLR_SILENCE) )
    {
        send_to_char( "You seem to have problems speaking!\n\r", ch );
        return;
    }

    argument = one_argument( argument, arg );

    if ( arg[0] == '\0' || argument[0] == '\0' )
    {
        send_to_char( "Ask whom what?\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    if ( ch->in_room != victim->in_room )
    {
        send_to_char( "They aren't here.\n\r", ch );
        return;
    }

    act( "You ask $N '$t'.", ch, argument, victim, TO_CHAR );
    act( "$n asks you '$t'.", ch, argument, victim, TO_VICT );
    act( "$n asks $N a question.", ch, NULL, victim, TO_NOTVICT );

    return;
}

void send_to_loc( char *message, int x, int y, int z )
{
    CHAR_DATA *vch;

    for ( vch = map_ch[x][y][z]; vch != NULL; vch = vch->next_in_room )
    {
        if ( !vch->desc || vch->desc->connected != CON_PLAYING )
            continue;
        send_to_char( message, vch );
        send_to_char( "\n\r",  vch );
    }
    return;
}

void do_beep( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;

    one_argument( argument, arg1 );

    if ( arg1[0] =='\0' )
    {
        send_to_char( "Usage: BEEP <victim>\n\r", ch );
        return;
    }

    if ( ( victim = get_char_world( ch, arg1 ) ) == NULL )
    {
        send_to_char( "Couldn't find the victim.\n\r", ch );
        return;
    }

    if ( ch == victim )
    {
        send_to_char( "Beep yourself?  Sure, ok... beep! beep! beep!\n\r", ch );
        return;
    }

    if ( IS_SET( victim->deaf, CHANNEL_BEEP ) )
    {
        send_to_char( "Your victim is ignoring beeps.  Sorry!\n\r", ch );
        return;
    }

    act( "You beep $N...",         ch, NULL, victim, TO_CHAR );
    act( "\a$n is beeping you...", ch, NULL, victim, TO_VICT );

    return;
}

void do_history( CHAR_DATA *ch, char *argument )
{
    if ( IS_SET(ch->pcdata->pflags,PLR_ASS) )
        return;

    send_to_char( history10, ch );
    send_to_char( history9, ch );
    send_to_char( history8, ch );
    send_to_char( history7, ch );
    send_to_char( history6, ch );
    send_to_char( history5, ch );
    send_to_char( history4, ch );
    send_to_char( history3, ch );
    send_to_char( history2, ch );
    send_to_char( history1, ch );
    send_to_char( "\n\r\n\rLast Tell: \n\r", ch );
    send_to_char(ch->last_tell,ch);
    if ( ch->pcdata->alliance > -1 )
    {
        send_to_char( "\n\r\n\rLast Alliance Message: \n\r", ch );
        send_to_char(alliance_table[ch->pcdata->alliance].history,ch);
    }
    send_to_char( "\n\r", ch );
    return;
}

void do_radiosilence(CHAR_DATA *ch, char *argument)
{
    if ( IS_SET(ch->pcdata->pflags,PFLAG_RAD_SIL) )
    {
        REMOVE_BIT(ch->pcdata->pflags,PFLAG_RAD_SIL);
        send_to_char( "You no longer keep radio silence.\n\r", ch );
    }
    else
    {
        SET_BIT(ch->pcdata->pflags,PFLAG_RAD_SIL);
        send_to_char( "You now keep radio silence.\n\r", ch );
    }
    return;
}
