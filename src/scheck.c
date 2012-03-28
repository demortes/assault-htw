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

#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "ack.h"
#include "tables.h"

#include "ssm.h"

/*  String checker, Spectrum 11/96
 *
 *  Basic idea is to walk through all the strings we know about, and mark them
 *  as referenced. Then we check for strings that have a reference count thats
 *  different from ptr->usage and log them
 */

/*
 * Things which are walked (anything else must be touched from these):
 *
 * o char_list
 * o descriptor_list
 * o object_list
 * o obj_index hash table
 * o socials table
 * o helps
 * o areas
 * o notes/ideas/etc
 */

extern char * quote_table[MAX_QUOTE];

/* From ban.c: */

extern BAN_DATA *first_ban;

extern OBJ_INDEX_DATA *        obj_index_hash          [MAX_KEY_HASH];

/* Main code */

static void touch(char *str)
{
    BufEntry *p;

    if (!str)
        return;

    if (str < string_space || str > top_string)
        return;                                             /* not in string space */

    p = (BufEntry *) (str - HEADER_SIZE);
    p->ref++;
}

static void clear(void)
{
    /* Set all reference counts to 0 */

    BufEntry *p;

    for (p = ssm_buf_head; p; p = p->next)
        p->ref = 0;
}

static BufEntry *dump_ptr[2];

static long dump(void)
{
    /* Dump strings that have ref!=usage */

    FILE *dumpf;
    BufEntry *p;
    long count = 0;

    fclose(fpReserve);
    dumpf = fopen("../reports/leaks.dmp", "w");

    for (p = ssm_buf_head; p; p = p->next)
    {
        if (p->usage > 0 && p->usage != p->ref)
        {
            /* things to ignore:
             * the common <%hhp %mm %vmv> prompt string
             * a '$' (from socials)
             */

            if (!str_cmp(p->buf, "<%hhp %mm %vmv> ") ||
                !str_cmp(p->buf, "$"))
                continue;

            fprintf(dumpf, "usage %2d/%2d, caller %s, string %s\n",
                p->ref,
                p->usage,
                p->caller,
                p->buf);
            count += abs(p->usage - p->ref);
        }
        dump_ptr[0]=dump_ptr[1];
        dump_ptr[1]=p;
    }

    fclose(dumpf);
    fpReserve = fopen(NULL_FILE, "r");

    return count;
}

static void walk_web ()
{
    touch(history1);
    touch(history2);
    touch(history3);
    touch(history4);
    touch(history5);
    touch(history6);
    touch(history7);
    touch(history8);
    touch(history9);
    touch(history10);
    touch(web_data.last_killed_in_pit);
    touch(web_data.last_kills_in_pit);
    touch(web_data.highest_ranking_player);
    touch(web_data.newest_player);
    return;
}

static void walk_pager_data(PAGER_DATA *p)
{
    if (!p)
        return;

    touch(p->from);
    touch(p->msg);
    touch(p->time);
    walk_pager_data(p->next);
}

static void walk_queue_data(QUEUE_DATA * q)
{
    if (!q)
        return;

    touch(q->command);
    walk_queue_data(q->next);
}

static void walk_pcdata(PC_DATA * p)
{
    int i;

    if (!p)
        return;

    touch(p->pwd);
    touch(p->bamfin);
    touch(p->bamfout);
    touch(p->title);
    touch(p->room_enter);
    touch(p->room_exit);
    touch(p->host);
    touch(p->who_name);
    touch(p->header);
    touch(p->message );
    touch(p->lastlogin);
    touch(p->load_msg);

    for ( i = 0; i < MAX_IGNORES; i++ )
        touch( p->ignore_list[i]);

    for ( i = 0; i < 5 ; i++ )
    {
        touch( p->pedit_string[i] );
    }
    touch( p->pedit_state );
    touch( p->email_address );
    walk_queue_data(p->queue);
    walk_pager_data(p->pager);
}

static void walk_brand_data(BRAND_DATA * brand)
{
    if (!brand)
        return;

    touch(brand->branded);
    touch(brand->branded_by);
    touch(brand->dt_stamp);
    touch(brand->message);
    touch(brand->priority);
}

static void walk_brands( void )
{
    BRAND_DATA *  this_brand;
    DL_LIST * brands;
    for (brands = first_brand ; brands; brands = brands->next)
    {
        this_brand = brands->this_one;
        walk_brand_data(this_brand);
    }
}

static void walk_char_data(CHAR_DATA * ch)
{
    int i;

    if (!ch)
        return;

    walk_pcdata(ch->pcdata);

    touch(ch->name);
    touch(ch->prompt);
    touch(ch->old_prompt);
    touch(ch->last_tell);
    for ( i = 0;i < 5;i++ )
    {
        touch( ch->alias[i]);
        touch( ch->alias_command[i]);
    }

}

static void walk_obj_index_data(OBJ_INDEX_DATA * o)
{
    if (!o)
        return;

    touch(o->name);
    touch(o->short_descr);
    touch(o->description);
    touch(o->owner);
    touch(o->image);
}

static void walk_obj_data(OBJ_DATA * o)
{
    if (!o)
        return;

    touch(o->owner);
    touch(o->name);
    touch(o->short_descr);
    touch(o->description);
}

static void walk_area_data(AREA_DATA * ad)
{

    if (!ad)
        return;

    touch(ad->filename);
    touch(ad->name);
    touch(ad->owner);
    touch(ad->can_read);
    touch(ad->can_write);
    touch(ad->keyword);                                     /* spec- missed strings */
}

static void walk_social_type(struct social_type *s)
{

    if (!s)
        return;
    touch( s->name );
    touch(s->char_no_arg);
    touch(s->others_no_arg);
    touch(s->char_found);
    touch(s->others_found);
    touch(s->vict_found);
    touch(s->char_auto);
    touch(s->others_auto);
}

static void walk_help_data(HELP_DATA * h)
{
    if (!h)
        return;

    touch(h->keyword);
    touch(h->text);
}

static void walk_descriptor_data(DESCRIPTOR_DATA * d)
{
    if (!d)
        return;

    touch(d->host);
    if ( d->connected != CON_PLAYING )
        walk_char_data (d->character);
}

static void walk_ban_data(BAN_DATA * b)
{
    touch(b->name);
    touch( b->banned_by);
    touch( b->note);
}

static void walk_socials(void)
{
    extern int maxSocial;
    int i;

    for (i = 0; i < maxSocial; i++)
        walk_social_type(&social_table[i]);
}

static void walk_helps(void)
{
    HELP_DATA *h;

    for (h = first_help; h; h = h->next)
        walk_help_data(h);
}

static void walk_chars(void)
{
    CHAR_DATA *ch;

    for (ch = first_char; ch; ch = ch->next)
        walk_char_data(ch);
}

static void walk_descriptors(void)
{
    DESCRIPTOR_DATA *d;

    for (d = first_desc; d; d = d->next)
        walk_descriptor_data(d);
}

static void walk_objects(void)
{
    OBJ_DATA *o;

    for (o = first_obj; o; o = o->next)
        walk_obj_data(o);
    walk_obj_data(vehicle_weapon);
}

static void walk_areas(void)
{
    AREA_DATA *ad;

    for (ad = first_area; ad; ad = ad->next)
        walk_area_data(ad);
}

static void walk_obj_indexes(void)
{
    OBJ_INDEX_DATA *o;
    int i;

    for (i = 0; i < MAX_KEY_HASH; i++)
        for (o = obj_index_hash[i]; o; o = o->next)
            walk_obj_index_data(o);
}

static void walk_bans(void)
{
    BAN_DATA *b;

    for (b = first_ban; b; b = b->next)
        walk_ban_data(b);
}

static void walk_message_data(MESSAGE_DATA * m)
{
    if (!m)
        return;

    touch(m->message);
    touch(m->author);
    touch(m->title );
}

void walk_messages( MESSAGE_DATA * m )
{
    for( ; m ; m = m->next )
        walk_message_data( m );
}

void walk_boards( void )
{
    BOARD_DATA    *board;
    extern BOARD_DATA *first_board;
    for (board = first_board; board; board = board->next)
        walk_messages(board->first_message);
}

void walk_sysdata( void )
{
    sh_int looper;
    touch( sysdata.playtesters );
    for ( looper = 0; looper < 5; looper++ )
        touch( sysdata.imms[looper].this_string);
}

void walk_quotes( void )
{
    sh_int looper;
    for ( looper = 0; looper < MAX_QUOTE; looper++ )
        touch( quote_table[looper] );
}

static void walk_disable_data(DISABLED_DATA * disable)
{
    if (!disable)
        return;

    touch(disable->disabled_by);
}

void walk_disablelist(void)
{
    DISABLED_DATA *disable;

    for (disable = disabled_first; disable; disable = disable->next)
    {
        walk_disable_data(disable);
    }
}

void walk_alliances(void)
{
    int i;
    for ( i=0;i<MAX_ALLIANCE;i++ )
    {
        if ( alliance_table[i].name == NULL )
            break;
        touch( alliance_table[i].name );
        touch( alliance_table[i].leader );
        touch( alliance_table[i].history );
    }
}

void walk_scores(void)
{
    int i;
    for ( i=0;i<100;i++ )
    {
        touch( score_table[i].name );
        touch( score_table[i].killedby );
    }
    for ( i=0;i<30;i++ )
        touch( rank_table[i].name );
}

void walk_multiplay(void)
{
    int i;
    for ( i=0;i<30;i++ )
    {
        touch(multiplay_table[i].name);
        touch(multiplay_table[i].host);
    }
}

void walk_build_table(void)
{
    int i;
    for ( i=0;i<MAX_BUILDING;i++ )
    {
        touch(build_table[i].name);
        touch(build_table[i].desc);
        touch(build_table[i].symbol);
        touch(build_help_table[i].help);
    }
}

void walk_buildings(void)
{
    BUILDING_DATA *bld;
    for ( bld = first_building;bld;bld = bld->next )
    {
        touch( bld->owned );
        touch( bld->name );
        touch( bld->attacker );
    }
}

void walk_vehicles(void)
{
    VEHICLE_DATA *vhc;
    for ( vhc = first_vehicle;vhc;vhc = vhc->next )
    {
        touch( vhc->name );
        touch( vhc->desc );
    }
}

void do_scheck(CHAR_DATA * ch, char *argument)
{
    char buf[MAX_STRING_LENGTH];
    extern bool disable_timer_abort;
    disable_timer_abort = TRUE;
    clear();

    walk_socials();
    walk_helps();
    walk_chars();
    walk_descriptors();
    walk_objects();
    walk_areas();
    walk_bans();

    walk_obj_indexes();
    walk_boards();
    walk_brands( );
    walk_sysdata( );
    walk_quotes( );
    walk_disablelist( );
    walk_buildings( );
    walk_vehicles( );
    walk_scores( );
    walk_multiplay( );
    walk_build_table( );
    walk_alliances( );
    walk_web ();

    sprintf(buf, "%ld leaks dumped to leaks.dmp\n\r",
        dump());
    send_to_char(buf, ch);
    disable_timer_abort = FALSE;
}
