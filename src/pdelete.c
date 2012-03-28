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
#include <math.h>
#include <ctype.h>

void do_sdelete( CHAR_DATA *ch, char *argument )
{
    DESCRIPTOR_DATA *d;
    char strsave[MAX_INPUT_LENGTH];
    char arg1[MAX_INPUT_LENGTH];
    char *pArg;
    char cEnd;
    char buf[MAX_INPUT_LENGTH];
    int min,i;

    if ( ch->fighttimer > 0 )
    {
        send_to_char( "Not while you're fighting!\n\r", ch );
        return;
    }
    strcpy(buf,ch->name);
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( buf ), "/", capitalize( buf ) );

    pArg = arg1;
    while ( isspace(*argument) )
        argument++;

    cEnd = ' ';
    if ( *argument =='\'' || *argument == '"' )
        cEnd = *argument++;

    while ( *argument != '\0' )
    {
        if ( *argument == cEnd )
        {
            argument++;
            break;
        }
        *pArg++ = *argument++;
    }
    *pArg = '\0';

    if (  ( ch->pcdata->pwd != '\0' )
        && ( arg1[0] == '\0' )  )
    {
        send_to_char( "Syntax: pdelete (password)\n\r", ch );
        return;
    }
    if (  ( ch->pcdata->pwd != '\0' )
        && ( strcmp( crypt( arg1, ch->pcdata->pwd ), ch->pcdata->pwd ) )  )
    {
        WAIT_STATE( ch, 40 );
        send_to_char( "Wrong password.  Wait 10 seconds.\n\r", ch );
        sprintf( buf, "%s attempted to pdelete, but used the wrong password!", ch->name );
        log_f( buf );
        return;
    }
    if ( ch->pcdata->alliance != -1 )
    {
        if ( !str_cmp(ch->name,alliance_table[ch->pcdata->alliance].leader) && alliance_table[ch->pcdata->alliance].members > 1 )
        {
            send_to_char( "You can't pdelete while you're the leader of an alliance. Uset Setowner to give it to someone else.\n\r", ch );
            return;
        }
        alliance_table[ch->pcdata->alliance].members--;
        save_alliances();
    }
    ch->trust = 0;
    min = 0;
    for ( i=0;i<100;i++ )
    {
        if ( min == -1 )
            if ( ( score_table[i].kills < ch->pcdata->pkills || ( score_table[i].kills == ch->pcdata->pkills &&score_table[min].buildings < ch->pcdata->bkills ) || ( score_table[i].kills == ch->pcdata->pkills &&score_table[min].buildings == ch->pcdata->bkills  && score_table[i].time < my_get_hours(ch,FALSE) ) ) )
                min = i;
        if ( ( score_table[i].kills < score_table[min].kills || ( score_table[i].kills == score_table[min].kills &&score_table[min].buildings < score_table[i].buildings ) || ( score_table[i].kills == score_table[i].kills &&score_table[min].buildings == score_table[i].buildings  && score_table[i].time < score_table[min].time ) ) )
            min = i;
        if ( score_table[i].name == NULL )
        {
            min = i;
            break;
        }
    }
    if ( min >= 0 && min <= 99 )
    {
        if ( score_table[min].name != NULL )
            free_string(score_table[min].name);
        if ( score_table[min].killedby != NULL )
            free_string(score_table[min].killedby);

        score_table[min].name = str_dup(ch->name);
        score_table[min].killedby = str_dup("Deleted");
        score_table[min].kills = ch->pcdata->pkills;
        score_table[min].buildings = ch->pcdata->bkills;
        score_table[min].time = my_get_hours(ch,FALSE);
        save_scores();
    }
    {
        BUILDING_DATA *bld = first_building;
        BUILDING_DATA *bld_next = bld;
        OBJ_DATA *obj = first_obj;
        OBJ_DATA *obj_next = obj;
        CHAR_DATA *wch;
        for ( bld = first_building;bld;bld = bld_next )
        {
            bld_next = bld->next;
            if ( !str_cmp(bld->owned,ch->name) && !is_neutral(bld->type) )
                extract_building(bld,TRUE);
        }
        save_buildings();
        for ( obj = first_obj;obj;obj = obj_next )
        {
            obj_next = obj->next;
            if ( !str_cmp(obj->owner,ch->name) || (obj->carried_by && obj->carried_by == ch) )
                extract_obj(obj);
        }
        if ( ch == map_ch[ch->x][ch->y][ch->z] )
            map_ch[ch->x][ch->y][ch->z] = ch->next_in_room;
        else
            for ( wch = map_ch[ch->x][ch->y][ch->z];wch;wch = wch->next_in_room )
                if ( wch->next_in_room == ch )
                    wch->next_in_room = ch->next_in_room;
    }
    update_ranks(ch);
    ch->played = 0;
    if ( ch->played_tot > 2 * 3600 )
        ch->played_tot = 2 * 3600;
    ch->position = POS_STANDING;
    ch->max_hit = STARTING_HP;
    ch->hit = STARTING_HP;
    ch->implants = 0;
    ch->quest_points = 0;
    ch->medals = 0;
    ch->disease = 0;
    ch->effect = 0;
    ch->pcdata->pkills = 0;
    ch->pcdata->tpkills = 0;
    ch->pcdata->bkills = 0;
    ch->pcdata->tbkills = 0;
    ch->pcdata->deaths = 0;
    ch->pcdata->blost = 0;
    ch->pcdata->pbhits = 0;
    ch->pcdata->pbdeaths = 0;
    ch->pcdata->nukemwins = 0;
    ch->pcdata->dead = FALSE;
    for ( i=0;i<MAX_SKILL;i++ )
        ch->pcdata->skill[i] = 0;
    ch->pcdata->alliance = -1;
    ch->pcdata->prof_points = 0;
    ch->pcdata->prof_ttl = 0;
    ch->pcdata->experience = 0;
    ch->pcdata->deleted = TRUE;
    if ( !IS_SET(ch->pcdata->pflags,PLR_PDELETER) && my_get_hours(ch,TRUE) > 6 )
        SET_BIT(ch->pcdata->pflags,PLR_PDELETER);
    save_char_obj(ch);
    // unlink(strsave);
    send_to_char("Character reset.\n\rIf you wish to recreate, relog under the same username (@@eAlts are NOT allowed!@@N).\n\r",ch);
    sprintf( buf, "%s has pdeleted!", ch->name );
    log_f( buf );
    sprintf( buf, "@@d%s has @@RDELETED!@@N\r\n", ch->name );
    info(buf,81);
    sprintf( buf, "%s has left " mudnamenocolor "!", ch->name );
    info( buf, 1 );

    d = ch->desc;
    extract_char( ch, TRUE );
    if ( d != NULL )
        close_socket( d );
    return;
}

void do_nuke( CHAR_DATA *ch, char *argument)
{
    DESCRIPTOR_DATA *d;
    bool found = FALSE;
    char strsave[MAX_INPUT_LENGTH];
    CHAR_DATA *victim;
    char arg[MAX_INPUT_LENGTH];
    char buf[MAX_STRING_LENGTH];

    one_argument( argument, arg );
    if ( arg[0] == '\0' )
    {
        send_to_char( "@@dSyntax:nuke  <character name>@@N \n\r", ch );
        return;
    }

    if ( ( victim = get_ch (arg) ) == NULL )
    {
        DESCRIPTOR_DATA d2;
        found = load_char_obj( &d2, arg, FALSE );

        if (!found)
        {
            sprintf( buf, "No pFile found for '%s'.\n\r", capitalize( arg ) );
            send_to_char( buf, ch );
            free_char( d2.character );
            return;
        }
        else
        {
            victim = d2.character;
            d2.character = NULL;
            victim->desc = NULL;
        }
    }

    if ( (victim == ch) || (get_trust(victim) > get_trust(ch)) )
    {
        send_to_char( "Yah, right.\n\r", ch );
        return;
    }

    strcpy(buf,victim->name);
    sprintf( strsave, "%s%s%s%s", PLAYER_DIR, initial( buf ), "/", capitalize( buf ) );
    unlink(strsave);

    if ( victim->pcdata->alliance != -1 )
    {
        alliance_table[victim->pcdata->alliance].members--;
        save_alliances();
    }
    {
        BUILDING_DATA *bld = first_building;
        BUILDING_DATA *bld_next = bld;
        OBJ_DATA *obj = first_obj;
        OBJ_DATA *obj_next = obj;
        CHAR_DATA *wch;
        for ( bld = first_building;bld;bld = bld_next )
        {
            bld_next = bld->next;
            if ( !str_cmp(bld->owned,victim->name) && !is_neutral(bld->type) )
                extract_building(bld,TRUE);
        }
        save_buildings();
        for ( obj = first_obj;obj;obj = obj_next )
        {
            obj_next = obj->next;
            if ( !str_cmp(obj->owner,victim->name) )
                extract_obj(obj);
        }
        if ( victim == map_ch[victim->x][victim->y][victim->z] )
            map_ch[victim->x][victim->y][victim->z] = victim->next_in_room;
        else
            for ( wch = map_ch[victim->x][victim->y][victim->z];wch;wch = wch->next_in_room )
                if ( wch->next_in_room == victim )
                    wch->next_in_room = victim->next_in_room;
    }

    d = victim->desc;
    if ( d != NULL )
    {
        close_socket( d );
        extract_char( victim, TRUE );
    }
    else
    {
        free_char(victim);
        victim = NULL;
    }

    sprintf( buf,"%d", web_data.tot_players-1);
    update_web_data(WEB_DATA_TOT_PLAYERS,buf);
    return;
}
