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
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ack.h"
#include <signal.h>

/*
 * Local functions.
 */
bool    fast_healing    args( (CHAR_DATA *ch) );
int     hit_gain        args( ( CHAR_DATA *ch ) );
void    weather_update  args( ( void ) );
void    init_weather    args( ( void ) );
void    char_update     args( ( void ) );
void    gain_update     args( ( void ) );
void    obj_update      args( ( void ) );
void    bomb_update     args( ( void ) );
void    vehicle_update  args( ( void ) );
void    aggr_update     args( ( void ) );
void    rooms_update    args( ( void ) );
void    quest_update    args( ( void ) );
void    maze_update     args( ( void ) );
void    update_last_obj args( ( void ) );
void    mode_update     args( ( void ) );
void    spec_update args( ( void ) );
void    spec_up     args( ( CHAR_DATA *ch ) );

int abort_threshold = BOOT_DB_ABORT_THRESHOLD;
bool    disable_timer_abort = FALSE;
int last_checkpoint;
extern char *  const   helper          [MAX_HELPER];

int get_user_seconds ()
{
    struct rusage rus;
    getrusage (RUSAGE_SELF, &rus);
    return rus.ru_utime.tv_sec;
}

/* Update the checkpoint */
void alarm_update ()
{
    extern int ssm_dup_count;
    extern int ssm_loops;

    ssm_dup_count=0;
    ssm_loops=0;

    last_checkpoint = get_user_seconds();
    if (abort_threshold == BOOT_DB_ABORT_THRESHOLD)
    {
        abort_threshold = RUNNING_ABORT_THRESHOLD;
        fprintf (stderr, "Used %d user CPU seconds.\n", last_checkpoint);
    }
}

#ifndef WIN32
/* Set the virtual (CPU time) timer to the standard setting, ALARM_FREQUENCY */

void reset_itimer ()
{
    struct itimerval itimer;
    itimer.it_interval.tv_usec = 0;                         /* miliseconds */
    itimer.it_interval.tv_sec  = ALARM_FREQUENCY;
    itimer.it_value.tv_usec = 0;
    itimer.it_value.tv_sec = ALARM_FREQUENCY;

    /* start the timer - in that many CPU seconds, alarm_handler will be called */
    if (setitimer (ITIMER_VIRTUAL, &itimer, NULL) < 0)
    {
        //		perror ("reset_itimer:setitimer");
        //		exit (1);
    }
}
#endif

const char * szFrozenMessage = "Alarm_handler: Not checkpointed recently, aborting!\n";

/* Signal handler for alarm - suggested for use in MUDs by Fusion */
void alarm_handler (int signo)
{
    int usage_now = get_user_seconds();

    /* Has there gone abort_threshold CPU seconds without alarm_update? */
    if (!disable_timer_abort && (usage_now - last_checkpoint > abort_threshold ))
    {
        /* For the log file */
        char buf[MAX_STRING_LENGTH];
        extern int ssm_dup_count;
        extern int ssm_loops;
        extern int ssm_recent_loops;

        /* spec: log usage values */
        log_f("current usage: %d, last checkpoint: %d",
            usage_now, last_checkpoint);
        log_f("SSM dups: %d, loops: %d, recent: %d",
            ssm_dup_count, ssm_loops, ssm_recent_loops);

        sprintf(buf,  "%s\n\r", szFrozenMessage );
        bug(buf,0);
        raise(SIGABRT);                                     /* kill ourselves on return */
        //	do_hotreboot(NULL,"crash"); //Lets see if this works...
    }

    /* The timer resets to the values specified in it_interval
     * automatically.
     *
     * Spec: additionally, SIGABRT is blocked in this handler, and will
     * only be delivered on return. This should ensure a good core.
     */
}

#ifndef WIN32
/* Install signal alarm handler */
void init_alarm_handler()
{
    struct sigaction sa;

    sa.sa_handler = alarm_handler;
    sa.sa_flags = SA_RESTART;                               /* Restart interrupted system calls */
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGABRT);
    /* block abort() in the handler
     * so we can get a good coredump */

    if (sigaction(SIGVTALRM, &sa, NULL) < 0)                /* setup handler for virtual timer */
    {
        perror ("init_alarm_handler:sigaction");
        exit (1);
    }
    last_checkpoint = get_user_seconds();
    reset_itimer();                                         /* start timer */
}
#endif

/*
 * Regeneration stuff.
 */
int hit_gain( CHAR_DATA *ch )
{
    int gain;
    BUILDING_DATA *bld;

    gain = 3;
    if ( ( bld = ch->in_building ) != NULL )
    {
        if ( bld->type == BUILDING_ORGANIC_CHAMBER && bld->owner && bld->owner == ch )
            gain *= 17;
        if ( bld->value[9] > 0 )
        {
            if ( ch->in_vehicle )
                send_to_char( "Your vehicle keeps you safe from the fallout.\n\r", ch);
            else
            {
                send_to_char( "@@eYou are hurt by the nuclear fallout!@@N\n\r", ch);
                gain = -40;
            }
        }
    }

    return UMIN(gain, ch->max_hit - ch->hit);
}

void spec_update( void )
{
    CHAR_DATA *ch;
    for ( ch = first_char;ch;ch = ch->next )
    {
        if ( !IS_SET(ch->config,CONFIG_BLIND) )
            continue;
        if ( --ch->pcdata->spec_timer > 0 )
            continue;
        spec_up(ch);
        ch->pcdata->spec_timer = ch->pcdata->spec_init;
    }
    return;
}

void spec_up( CHAR_DATA *ch )
{
    CHAR_DATA *wch;
    int x,y,maxx,range,z=ch->z;
    OBJ_DATA *obj;
    char buf[MSL];

    if ( IS_SET(ch->config,CONFIG_LARGEMAP) )
        maxx = ch->map;
    else
        maxx = ch->map / 2;
    range = 0;
    if ( ( obj = get_eq_char(ch,WEAR_HOLD_HAND_L) ) != NULL )
        if ( obj->item_type == ITEM_WEAPON )
            range = obj->value[4] + 1;
    for ( x = ch->x - maxx;x < ch->x + maxx;x++ )
        for ( y = ch->y - maxx;y < ch->y + maxx;y++ )
    {
        if ( x < 1 || y < 1 || x >= MAX_MAPS || y >= MAX_MAPS )
            continue;
        for ( wch = map_ch[x][y][z];wch;wch = wch->next_in_room )
        {
            if ( wch == NULL || wch == ch || wch->z != ch->z )
                break;
            sprintf( buf, "%s%s at %d/%d (%s%s)\n\r", (IS_BETWEEN(x,ch->x-range,ch->x+range) && IS_BETWEEN(y,ch->y-range,ch->y+range)) ? "(*) " : "", wch->name, x, y, (ch->y < y) ? "North" : (ch->y == y ) ? "" : "South", (ch->x > x) ? "West" :(ch->x == x) ? "" : "East" );
            send_to_char( buf, ch );
        }
    }
}

void gain_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;

    for ( ch = first_char; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;
        if ( ch->is_free != FALSE || ch == NULL || ch->in_room == NULL )
            continue;

        if ( ch->pcdata->lastskill > 0 )
            ch->pcdata->lastskill--;
        if ( ch->dead || ch->c_sn == gsn_dead || ch->position == POS_DEAD )
        {
            pdie(ch);
            continue;
        }
        if ( ch->position == POS_DEAD )
            ch->position = POS_STANDING;
        if ( ch->position >= POS_STUNNED )
        {
            ch->hit = URANGE( -100, ch->hit + hit_gain(ch), ch->max_hit );

            if ( ch->hit < 0 )
            {
                if ( check_dead(ch,ch) )
                    continue;
            }
            if ( ch->carry_weight > can_carry_w( ch ) )
                send_to_char( "You are carrying so much weight that you are @@eEXHAUSTED@@N!!\n\r", ch );
        }

        if ( IS_SET(ch->effect,EFFECT_TRACER) && (number_percent() < 3 || fast_healing(ch)))
            REMOVE_BIT(ch->effect,EFFECT_TRACER);
        if ( IS_SET(ch->effect,EFFECT_BLIND) && (number_percent() < 10 || fast_healing(ch)))
        {
            send_to_char( "You can see again!\n\r", ch );
            act( "$n can see again!", ch, NULL, NULL, TO_ROOM );
            REMOVE_BIT(ch->effect,EFFECT_BLIND);
        }
        if ( IS_SET(ch->effect,EFFECT_VISION) && (number_percent() < 10 || fast_healing(ch)))
            REMOVE_BIT(ch->effect,EFFECT_VISION);

        if ( IS_SET(ch->effect,EFFECT_ENCRYPTION) && number_percent() < 5 )
        {
            send_to_char( "Your encryption barrier fails!\n\r", ch );
            REMOVE_BIT(ch->effect,EFFECT_ENCRYPTION);
        }
        if ( IS_SET(ch->effect,EFFECT_CONFUSE) && (number_percent() < 10 || fast_healing(ch)))
        {
            send_to_char( "You are no longer confused!\n\r", ch );
            REMOVE_BIT(ch->effect,EFFECT_CONFUSE);
        }
        if ( IS_SET(ch->effect,EFFECT_DRUNK) && (number_percent() < 10 || fast_healing(ch)))
        {
            send_to_char( "You are no longer light headed!\n\r", ch );
            REMOVE_BIT(ch->effect,EFFECT_DRUNK);
        }
        if ( IS_SET(ch->effect,EFFECT_SLOW) && (number_percent() < 5 || fast_healing(ch) ))
        {
            send_to_char( "You are no longer sluggish!\n\r", ch );
            REMOVE_BIT(ch->effect,EFFECT_SLOW);
        }
        if ( IS_SET(ch->effect,EFFECT_RESOURCEFUL) && number_percent() < 5 )
        {
            send_to_char( "You feel less resourceful!\n\r", ch );
            REMOVE_BIT(ch->effect,EFFECT_RESOURCEFUL);
        }
        if ( IS_SET(ch->effect,EFFECT_BOMBER) && number_percent() < 5 )
        {
            send_to_char( "You feel calmer.\n\r", ch );
            REMOVE_BIT(ch->effect,EFFECT_BOMBER);
        }
        if ( IS_SET(ch->effect,EFFECT_POSTAL) && number_percent() < 5 )
        {
            send_to_char( "You feel calmer.\n\r", ch );
            REMOVE_BIT(ch->effect,EFFECT_POSTAL);
        }
        if ( IS_SET(ch->effect,EFFECT_WULFSKIN) && number_percent() < 5 )
        {
            send_to_char( "You shed off your wulfskin.\n\r", ch );
            REMOVE_BIT(ch->effect,EFFECT_WULFSKIN);
        }

        if ( ch->position == POS_DEAD )
            update_pos( ch );

        if ( ch->in_vehicle )
            continue;
        {
            BUILDING_DATA *bld;
            CHAR_DATA *bch;
            bool complete = TRUE;
            int i;

            if ( (bld = ch->in_building) != NULL )
            {
                if ( ( bch = bld->owner ) == NULL || !bch )
                    continue;
                if ( !allied(ch,bch) && ch != bch && has_ability(ch,7) )
                {
                    if ( bld->value[9] == 0 )
                        bld->value[9] = 20;
                }
                if ( bld->owner == ch || IS_IMMORTAL(ch) )
                    continue;
                if ( sneak(ch) )
                    continue;
                if ( !CIVILIAN(bld) && !is_neutral(bld->type) && !is_evil(bld) && bld->value[3] >= 0 )
                {
                    if ( ( bch = get_ch(bld->owned) ) != NULL )
                    {
                        if ( bch->security == FALSE )
                            continue;
                        if ( allied(bch,ch) && !practicing(ch) )
                            continue;

                        for ( i=0;i<8;i++ )
                            if ( bld->resources[i] > 0 )
                                complete = FALSE;
                        if ( complete )
                        {
                            if ( IS_SET(ch->config, CONFIG_SOUND))
                                sendsound(ch,"machinegun", 100,1,50,"combat","machinegun.wav");
                            send_to_char( "Small turrets fire at you from the ceiling!\n\r", ch );
                            if ( number_percent() < 85 )
                            {
                                int dam = number_fuzzy(80);
                                if ( IS_SET(bld->value[1], INST_INTERN_DEF) )
                                    dam *= 2;
                                send_to_char( "You are hit by the security system!\n\r", ch );
                                if ( IS_SET(bld->value[1], INST_STUN_GUN) && number_percent() < 50 )
                                {
                                    send_to_char( "@@WAn electrical shock @@aSTUNS@@W you!@@N\n\r", ch );
                                    WAIT_STATE(ch,16);
                                }
                                damage( bch, ch, dam, -1 );
                            }
                            else
                                send_to_char( "You manage to dodge the bullets... for now...\n\r", ch );

                            send_to_char( "@@yYour security system has discovered an intruder!\n\r@@N", bch );
                        }
                    }
                }
            }
            else if ( bld == NULL )
            {
                int heat=0;
                heat = ch->heat + wildmap_table[map_table.type[ch->x][ch->y][ch->z]].heat;
                if ( heat > 15 && !ch->in_vehicle )
                {
                    if ( my_get_minutes(ch,TRUE) <= 5 )
                    {
                        //					send_to_char( "You are protected from heat and cold damage for the first 5 minutes of gameplay. Move to another place quickly, or find some cooler armor to wear.\n\r", ch );
                        return;
                    }
                    send_to_char( "@@eIt's soooo hot!!@@N\n\r", ch );
                    damage(ch,ch,number_fuzzy(heat - 15),DAMAGE_ENVIRO);
                    return;
                }
                else if ( heat < -15 && !ch->in_vehicle )
                {
                    if ( my_get_minutes(ch,TRUE) <= 5 )
                    {
                        //					send_to_char( "You are protected from heat and cold damage for the first 5 minutes of gameplay. Move to another place quickly, or find some warmer armor to wear.\n\r", ch );
                        return;
                    }
                    send_to_char( "@@aIt's soooo cold!!@@N\n\r", ch );
                    damage(ch,ch,number_fuzzy((-1*heat) - 15),DAMAGE_ENVIRO);
                    return;
                }

            }
        }

    }
    return;
}

/*
 * Update all chars.
 * This function is performance sensitive.
 */
void char_update( void )
{
    CHAR_DATA *ch;
    CHAR_DATA *ch_next;
    CHAR_DATA *ch_save;
    CHAR_DATA *ch_quit;
    time_t save_time;
    extern int guess_game;
    CHAR_DATA *guess_ch=NULL;
    int guesses = 0,p=0;

    save_time   = current_time;
    ch_save     = NULL;
    ch_quit     = NULL;

    CREF( ch_next, CHAR_NEXT );
    for ( ch = first_char; ch != NULL; ch = ch_next )
    {
        ch_next = ch->next;
        if ( ch->is_free != FALSE )
            continue;

        /*
         * Find dude with oldest save time.
         */
        if ( !IS_NPC(ch)
            && ( ch->desc == NULL || ch->desc->connected == CON_PLAYING )
            &&   ch->level >= 2
            &&   ch->save_time < save_time )
        {
            ch_save     = ch;
            save_time   = ch->save_time;
        }

        if ( ch->desc != NULL && ch->desc->connected == CON_PLAYING )
            save_char_obj(ch);

        if ( ch->medaltimer > 0 )
            ch->medaltimer--;
        p++;
        if ( guess_game && ch->pcdata->guess > 0 )
        {
            guesses++;
            if ( !guess_ch )
                guess_ch = ch;
            else
            {
                if ( abs(guess_ch->pcdata->guess-guess_game) > abs(ch->pcdata->guess-guess_game))
                {
                    guess_ch->pcdata->guess = 0;
                    guess_ch = ch;
                }
                else
                    ch->pcdata->guess = 0;
            }
        }
        if ( str_cmp(ch->pcdata->load_msg,""))
        {
            send_to_char( "You have mail from an administrator: \n\r", ch );
            send_to_char( ch->pcdata->load_msg, ch);
            free_string(ch->pcdata->load_msg);
            ch->pcdata->load_msg = str_dup( "" );
        }
        /*	if ( my_get_minutes(ch,TRUE) <= 30 && ch->pcdata->deaths == 0 && IS_SET(ch->config,CONFIG_HELPER) )
            {
                char buf[MSL];
                int i = number_range(0,MAX_HELPER*2);

                if ( i < MAX_HELPER )
                {
                    sprintf( buf, "@@mHELPER #%d@@p] @@N%s\n\r", i, helper[i] );
                    send_to_char( buf, ch );
                }
            }*/
        else if ( number_percent() == 1 && number_percent() < 50 )
            send_to_char( "Please help us by using Assault's new voting link:\r\nhttp://www.mudconnect.com/mud-bin/vote_rank.cgi?mud=Assault:+High+Tech+War\n\r\n\rThanks.\n\r", ch );

        if ( ch->disease > 0 && ch->disease <= 20 )         //SARS
        {
            send_to_char( "You feel feverish.\n\r", ch );
            ch->max_hit = URANGE(1,ch->max_hit-(ch->max_hit/ch->disease),ch->max_hit);
            if ( ch->hit > ch->max_hit ) ch->hit = ch->max_hit;
            ch->disease--;
            ch->disease--;
            if ( ch->disease <= 0 )
            {
                if ( number_percent() < 70 )
                {
                    send_to_char( "You have been killed by the virus!\n\r", ch );
                    damage(ch,ch,ch->hit + 1,-1);
                    continue;
                }
                else
                    send_to_char( "You feel the effects of the virus wearing off.\n\r", ch );
            }
        }
        else if ( ch->disease > 0 && ch->disease > 20 )     //Bio Grenade Weakening
        {
            send_to_char( "You feel weak.\n\r", ch );
            ch->disease--;
            if ( ch->disease == 20 )
            {
                send_to_char( "You're starting to feel better.\n\r", ch );
                ch->disease = 0;
            }
            return;
        }

        if ( ch->trust < 81 && !ch->fake )
        {
            if ( ++ch->timer >= 12 )
            {
                if ( ch->in_room->vnum != ROOM_VNUM_LIMBO )
                {
                    act( "$n disappears into the void.", ch, NULL, NULL, TO_ROOM );
                    send_to_char( "You disappear into the void.\n\r", ch );
                    save_char_obj( ch );
                    char_from_room( ch );
                    char_to_room( ch, get_room_index( ROOM_VNUM_LIMBO ) );
                }
            }

            if ( ch->timer > 30 )
                ch_quit = ch;
        }

        if ( ch->position == POS_DEAD )
        {
            damage( ch, ch, number_range(5, 10), TYPE_UNDEFINED );
        }
        else if ( ch->hit < -10 )
        {
            damage( ch, ch, number_range(5, 10), TYPE_UNDEFINED );
        }

    }
    CUREF( ch_next );

    /*
     * Autosave and autoquit.
     * Check that these chars still exist.
     */
    if ( ch_save != NULL || ch_quit != NULL )
    {
        CREF( ch_next, CHAR_NEXT );

        for ( ch = first_char; ch != NULL; ch = ch_next )
        {
            ch_next = ch->next;
            if ( ch == ch_save )
                save_char_obj( ch );
        }
        CUREF( ch_next );

    }

    if ( guess_game )
    {
        if ( !guess_ch || guess_ch == NULL )
        {
            info ( "There were no guesses for the guess game. Nobody wins.", 0 );
        }
        else if ( guesses < 3 )
        {
            info ( "There were not enough guesses. Nobody wins.", 0 );
        }
        else
        {
            int win=0,wtype=0;
            char gbuf[MSL];
            info("Number Game Results:",0);
            sprintf(gbuf, "Number Chosen: %d",guess_game );
            info(gbuf,0);
            sprintf(gbuf, "Closest Guess: %d (%s)", guess_ch->pcdata->guess,guess_ch->name);
            info(gbuf,0);
            win = (100-abs(guess_ch->pcdata->guess-guess_game))/2;
            if ( win < 0 )
            {
                info("The guess was too far off. There is no reward.",0 );
                win = 0;
            }
            if ( guess_ch->pcdata->guess==guess_game )
            {
                win = 100;
                wtype = 1;
            }
            else if ( guess_ch->pcdata->guess<guess_game )
                wtype = 1;
            if ( win > 0 )
            {
                sprintf( gbuf, "Amount Won: %d %s", win, (wtype==0)?"QPs":"XPs");
                info(gbuf,0);
            }
            guess_ch->pcdata->guess = 0;
            if ( wtype==0 )
                guess_ch->quest_points += win;
            else
                guess_ch->pcdata->experience += win;
        }
        guess_game = 0;
    }
    else if (number_percent()<3 && p > 3 )
    {
        guess_game = number_range(1,1000);
        info("The game has picked a number between 1 and 1000. Guess which on the GAME channel!", 0);
    }
    return;
}

void bomb_update( void )
{
    BOMB_DATA *bomb;
    BOMB_DATA *bomb_next;
    OBJ_DATA *obj;
    CHAR_DATA *ch = NULL;
    for ( bomb = first_bomb;bomb;bomb = bomb_next )
    {
        bomb_next = bomb->next;
        obj = bomb->obj;
        if ( !obj || obj == NULL)
            continue;

        if ( obj->value[1] == 0 )
            continue;
        if ( obj->carried_by )
            ch = obj->carried_by;
        if ( ch == NULL || (str_cmp(ch->name,obj->owner) ) )
            if ( ( ch = get_ch(obj->owner) ) == NULL )
                continue;
        if ( obj->value[4] != 1 && obj->pIndexData->vnum != OBJ_VNUM_CANNONBALL && obj->pIndexData->vnum != OBJ_VNUM_SCUD)
            send_to_loc("*TICK*", obj->x, obj->y, obj->z );
        if ( obj->carried_by == NULL && obj->in_room && obj->in_room->vnum == ROOM_VNUM_LIMBO && obj->value[0] <= 1 )
        {
            char buf[MSL];
            bool ex = FALSE;
            obj_from_room(obj);
            obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
            ex = check_missile_defense(obj);
            if ( ex )
            {
                extract_obj(obj);
                continue;
            }
            sprintf( buf, "%s flies from the sky and lands in the room!\n\r", obj->short_descr );
            send_to_loc( buf, obj->x, obj->y, obj->z );
        }
        if ( obj->value[1] != 0 )
            if ( --obj->value[0] <= 0 )
        {
            explode(obj);
            continue;
        }
    }
    return;
}

/*
 * Update all objs.
 * This function is performance sensitive.
 */
void obj_update( void )
{
    extern int obj_count;
    OBJ_DATA *obj_next;
    OBJ_DATA *obj;
    OBJ_DATA *obj2;
    CHAR_DATA *ch;
    char buf[MSL];

    obj_count = 0;
    quest_objs = 0;
    for ( obj = first_obj;obj;obj = obj_next )
    {
        obj_next = obj->next;

        if ( (obj->x == 0 || obj->y == 0) && obj->carried_by == NULL )
            /* New object erase code:
            Objects used to crash sometimes when extracted,
            so I made them extract up updates instead of
            instantly in case an object is extracted twice. */
        {
            extract_obj(obj);
            continue;
        }

        if ( obj->item_type == ITEM_BOMB )
        {
            if ( !map_bld[obj->x][obj->y][obj->z] && obj->carried_by == NULL && obj->value[1] == 0 && !medal(obj) )
                extract_obj(obj);
            continue;
        }

        if ( obj->quest_timer > 0 )                         /* Quest Objects - See if the quest is over. */
        {
            int i;
            quest_objs++;
            if ( obj->carried_by != NULL )
            {
                obj->quest_timer = 0;
                obj->quest_map = 0;
                for ( i=0;i<MAX_QUEST_ITEMS;i++ )
                {
                    if ( quest_obj[i] == obj )
                    {
                        quest_obj[i] = NULL;
                        //remove me
                        //					free(quest_obj[i]);
                        break;
                    }
                }
                continue;
            }
            obj->quest_timer -= PULSE_OBJECTS / PULSE_PER_SECOND;
            if ( obj->quest_timer <= 0 )
            {
                BUILDING_DATA *bld;
                quest_objs--;
                for ( i=0;i<MAX_QUEST_ITEMS;i++ )
                {
                    if ( quest_obj[i] == obj )
                    {
                        quest_obj[i] = NULL;
                        //					free(quest_obj[i]);
                        //remove me
                        break;
                    }
                }
                bld = obj->in_building;
                extract_obj(obj);
                if ( bld && bld->timer == 0 )
                    extract_building(bld,FALSE);
                continue;
            }
        }

        if ( IS_SET(obj->extra_flags,ITEM_NUCLEAR) )
        {
            if ( number_percent() < 5 )
            {
                REMOVE_BIT(obj->extra_flags,ITEM_NUCLEAR);
                obj->attacker = NULL;
            }
            if ( (ch=obj->carried_by) )
            {
                act( "$p burns your skin!", ch, obj, NULL, TO_CHAR );
                damage((obj->attacker!=NULL)?obj->attacker:ch,ch,40,-1);
            }
            continue;
        }

        if ( obj->item_type == ITEM_MATERIAL )
        {
            if ( !map_bld[obj->x][obj->y][obj->z] && obj->carried_by == NULL )
            {
                map_table.resource[obj->x][obj->y][obj->z] = obj->value[0];
                extract_obj(obj);
                continue;
            }
            else if ( obj->in_building && obj->in_building->type == BUILDING_SPACE_CENTER && obj->value[0] == ITEM_GOLD && obj->carried_by == NULL )
            {
                obj->in_building->value[10] += obj->value[1];
                if ( obj->in_building->value[10] >= 50000 )
                    obj->in_building->value[10] = 50000;
                extract_obj(obj);
                continue;
            }
            else if ( obj->in_building && obj->in_building->type == BUILDING_TELEPORTER && obj->value[0] == -1 && obj->carried_by == NULL )
            {
                obj->in_building->value[10] += obj->value[1];
                if ( obj->in_building->value[10] >= 50000 )
                    obj->in_building->value[10] = 50000;
                extract_obj(obj);
                continue;
            }
        }
        obj_count++;

        if ( obj->z == Z_SPACE && number_percent() < 3 && !obj->carried_by )
        {
            int x,y;
            x = number_fuzzy(obj->x);
            y = number_fuzzy(obj->y);
            if (x < 0)
                x = SPACE_SIZE + 1 + x;
            else if (x > SPACE_SIZE)
                x = x - SPACE_SIZE;
            if (y<0)
                y = SPACE_SIZE + 1 + y;
            else if (y > SPACE_SIZE)
                y = y - SPACE_SIZE;
            sprintf( buf, "%s floats away.\n\r", obj->short_descr );
            send_to_loc(buf,obj->x,obj->y,Z_SPACE);
            move_obj(obj,x,y,Z_SPACE);
        }
        if ( obj->item_type == ITEM_BIOTUNNEL )
        {
            OBJ_DATA *obj2;
            OBJ_DATA *obj2_next;
            int i=0,l;
            bool ok = FALSE;
            if ( obj->value[2] < BORDER_SIZE || obj->value[3] < BORDER_SIZE || obj->value[2] > MAX_MAPS - BORDER_SIZE || obj->value[3] > MAX_MAPS - BORDER_SIZE || obj->carried_by )
                continue;
            if ( obj->value[0] != 1 || obj->value[2] == 0 || obj->value[3] == 0 )
                continue;
            if ( obj->in_building && obj->in_building->type == BUILDING_SECURE_WAREHOUSE )
                continue;
            l = (map_bld[obj->value[2]][obj->value[3]][obj->z]) ? get_item_limit(map_bld[obj->value[2]][obj->value[3]][obj->z]) : -1;
            for ( obj2 = map_obj[obj->value[2]][obj->value[3]];obj2;obj2 = obj2->next_in_room )
            {
                if ( obj2->item_type == ITEM_BIOTUNNEL && obj2->value[0] == 0 && obj2->z == obj->z && !obj->carried_by)
                {
                    ok = TRUE;
                }
                if ( COUNTS_TOWARDS_OBJECT_LIMIT(obj2) )
                    i++;
            }
            if ( l == -1 )
                ok = FALSE;
            if ( !ok )
            {
                obj->value[2] = 0;
                obj->value[3] = 0;
                continue;
            }
            if ( i >= l )
                continue;
            for ( obj2 = map_obj[obj->x][obj->y];obj2;obj2 = obj2_next )
            {
                obj2_next = obj2->next_in_room;
                if ( obj2 == obj || obj2->z != obj->z || obj2->carried_by || !CAN_WEAR(obj2, ITEM_TAKE) )
                    continue;
                move_obj(obj2,obj->value[2],obj->value[3],obj->z);
            }
        }
        if ( obj->item_type == ITEM_DRONE )
        {
            if ( obj->value[0] == 1 && obj->carried_by == NULL )
            {
                BUILDING_DATA *bld;
                char buf[MSL];

                if ( ( bld = get_obj_building(obj) ) == NULL )
                    continue;
                if ( bld->hp >= bld->maxhp || !complete(bld) )
                    continue;

                bld->hp = URANGE( 0, bld->hp + (number_range(obj->value[1]/2,obj->value[1]*1.5)*2), bld->maxhp );
                sprintf( buf, "%s scans, and repairs the building.", obj->short_descr );
                if ( number_percent() < 10 )
                {
                    obj->value[1]--;
                    if ( obj->value[1] <= 0 )
                    {
                        sprintf( buf+strlen(buf), "%s gets too worn out, and falls apart.", obj->short_descr );
                        extract_obj(obj);
                        obj_count--;
                    }
                }
                send_to_loc( buf, bld->x, bld->y, bld->z );
            }
            else if ( obj->value[0] == 2 && !obj->carried_by )
            {
                int nx=obj->x,ny=obj->y,ex=0;
                if ( obj->z != Z_UNDERGROUND )
                    continue;
                if ( obj->value[2] == DIR_NORTH )
                    ny++;
                else if ( obj->value[2] == DIR_SOUTH )
                    ny--;
                else if ( obj->value[2] == DIR_EAST )
                    nx++;
                else
                    nx--;

                if ( nx >= MAX_MAPS-3 || nx <= BORDER_SIZE || ny >= MAX_MAPS - BORDER_SIZE || ny <= BORDER_SIZE )
                {
                    extract_obj(obj);
                    obj_count--;
                    continue;
                }
                move_obj(obj,nx,ny,obj->z);
                for (nx=obj->x-10;nx<obj->x+10;nx++)
                {
                    if ( ex ) break;
                    for (ny=obj->y-10;ny<obj->y+10;ny++)
                    {
                        if ( ex ) break;
                        if ( map_bld[nx][ny][obj->z] && map_bld[nx][ny][obj->z]->active )
                        {
                            sprintf( buf, "@@yOne of your drones has detected: @@W%s@@y at @@W%d/%d@@y underground!@@N\n\r", map_bld[nx][ny][obj->z]->name, nx, ny );
                            if ( ( ch = get_ch(obj->owner ) ) )
                                send_to_char(buf,ch);
                            ex = 1;
                            extract_obj(obj);
                            obj_count--;
                            break;
                        }
                    }
                }
            }
        }
        else if ( obj->item_type == ITEM_ASTEROID )
        {
            if ( number_percent() < 20 )
                obj->value[1]++;
            if ( obj->value[1] > 200 )
                obj->value[1] = 200;
            if ( number_percent() <= 2 )
                obj->value[0] = number_range(-1,0);
        }
        else if ( obj->item_type == ITEM_TRASH )
        {
            if ( obj->pIndexData && obj->pIndexData->vnum == 800 && obj->z == Z_SPACE )
            {
                if ( number_percent() < 20 )
                {
                    for ( ch = map_ch[obj->x][obj->y][Z_SPACE];ch;ch = ch->next_in_room )
                    {
                        if ( number_percent() < 50 && ch->in_vehicle && ch->in_vehicle->z == Z_SPACE )
                        {
                            act( "$p crashes into your ship!", ch, obj, NULL, TO_CHAR );
                            act( "$p crashes into $n's ship!", ch, obj, NULL, TO_ROOM );
                            damage(ch,ch,obj->weight,-1);
                            if ( number_percent() < 50 )
                                continue;
                            if ( (obj2 = create_object(get_obj_index(933),0)) == NULL )
                                break;
                            send_to_char( "You discover some debrees all over your ship, and pull them in.\n\r", ch );
                            obj_to_char(obj2,ch);
                        }
                    }
                }
            }
        }
        else if ( obj->item_type == ITEM_BATTERY )
        {
            if ( obj->in_building && obj->in_building->type == BUILDING_BATTERY && obj->value[1] < obj->value[0] && obj->in_building->shield > 0 )
            {
                obj->value[1]++;
                obj->in_building->shield--;
            }
        }
        else if ( obj->item_type == ITEM_WEAPON )
        {
            if ( obj->value[11] > 0 && obj->value[0] < obj->value[1] )
            {
                if ( --obj->value[11] <= 0 )
                {
                    obj->value[11] = 5;
                    obj->value[0]++;
                }
            }
        }
        else if ( obj->item_type == ITEM_COMPUTER )
        {
            if ( obj->carried_by && obj->carried_by->position == POS_HACKING && obj->carried_by->c_obj == obj )
            {
                if ( !has_ability(obj->carried_by,8) || number_percent() < 75 )
                {
                    obj->value[1]--;
                    if ( obj->value[1] <= 0 )
                    {
                        send_to_char( "The computer blinks out. Battery is dead.\n\r", obj->carried_by );
                        do_bye(obj->carried_by,"");
                    }
                }
            }
        }
        else if ( obj->item_type == ITEM_MATERIAL )
        {
            if ( obj->carried_by != NULL )
                continue;
            if ( obj->in_building == NULL && map_bld[obj->x][obj->y][obj->z] == NULL )
            {
                extract_obj(obj);
                continue;
            }
            for ( obj2 = obj->next_in_room;obj2;obj2 = obj2->next_in_room )
            {
                if ( obj2 == NULL )
                    break;
                if ( NOT_IN_ROOM(obj,obj2) )
                    continue;
                if ( obj2->item_type != ITEM_MATERIAL || obj2->value[0] != obj->value[0] || obj2->value[1] + obj->value[1] > 10000 || obj->z != obj2->z || obj2->carried_by != NULL )
                    continue;
                obj2->value[1] += obj->value[1];
                obj2->weight += obj->weight;
                if ( obj2->weight > MAX_CHUNK_WEIGHT )
                    obj2->weight = MAX_CHUNK_WEIGHT;
                extract_obj(obj);
                obj_count--;
                break;
            }
        }
        else if ( obj->item_type == ITEM_SKILL_UP )
        {
            if ( obj->carried_by == NULL )
                continue;
            if ( obj->value[0] < 0 || obj->value[0] > MAX_SKILL )
            {
                send_to_char( "You had a skill upgrade with invalid values. Please contact an administrator.\n\r", obj->carried_by );
                extract_obj(obj);
                continue;
            }
            if ( obj->carried_by->pcdata->skill[obj->value[0]] < 100 )
                obj->carried_by->pcdata->skill[obj->value[0]] = URANGE(0,obj->carried_by->pcdata->skill[obj->value[0]] + obj->value[1],100);
            extract_obj(obj);
            continue;
        }
        else if ( obj->item_type == ITEM_TOKEN )
        {
            char buf[MSL];
            if ( obj->carried_by == NULL || obj->value[0] != 1 )
                continue;
            obj->carried_by->quest_points += obj->value[1];
            sprintf( buf, "@@rYou have been rewarded @@W%d @@rquest points!@@N\n\r", obj->value[1] );
            send_to_char( buf, obj->carried_by );
            if ( obj->carried_by->quest_points > 5000 )
            {
                send_to_char( "You have reached the cap 5000 QPs.\n\r", obj->carried_by );
                obj->carried_by->quest_points = 5000;
            }
            extract_obj(obj);
            continue;
        }
        else if ( obj->pIndexData && obj->pIndexData->vnum == 32679 && obj->carried_by )
        {
            if ( obj->carried_by->z != Z_PAINTBALL )
                extract_obj(obj);
        }
    }
    return;
}

void update_last_obj()
{
    for (last_obj=last_obj;last_obj->next;last_obj = last_obj->next)
        if ( !last_obj->next )
            break;
    return;
}

void vehicle_update( void )
{
    VEHICLE_DATA *vhc;
    VEHICLE_DATA *vhc_next;
    extern int vehicle_count;
    //	char buf[MSL];
    CHAR_DATA *wch;

    vehicle_count = 0;
    for ( vhc = first_vehicle;vhc;vhc = vhc_next )
    {
        vehicle_count++;
        vhc_next = vhc->next;
        if ( vhc->x == 0 && vhc->y == 0 )
        {
            extract_vehicle(vhc,FALSE);
            continue;
        }
        if ( vhc->driving && !vhc->driving->desc )
        {
            continue;
        }
        vhc->in_building = map_bld[vhc->x][vhc->y][vhc->z];
        if ( vhc->in_building != NULL && vhc->in_building->type == BUILDING_GARAGE && !AIR_VEHICLE(vhc->type) )
        {
            vhc->fuel = URANGE(0,vhc->fuel+number_range(1,3*vhc->in_building->level),vhc->max_fuel);
            vhc->hit = URANGE(0,vhc->hit+number_range(1,3*vhc->in_building->level),vhc->max_hit);
            vhc->ammo = URANGE(0,vhc->ammo+number_range(1,3*vhc->in_building->level),vhc->max_ammo);
        }
        else if ( vhc->in_building != NULL && vhc->in_building->type == BUILDING_AIRFIELD && AIR_VEHICLE(vhc->type))
        {
            vhc->fuel = URANGE(0,vhc->fuel+number_range(1,3*vhc->in_building->level),vhc->max_fuel);
            vhc->hit = URANGE(0,vhc->hit+number_range(1,3*vhc->in_building->level),vhc->max_hit);
            vhc->ammo = URANGE(0,vhc->ammo+number_range(1,3*vhc->in_building->level),vhc->max_ammo);
        }
        else if ( vhc->in_building != NULL && vhc->in_building->type == BUILDING_SPACE_CENTER && SPACE_VESSAL(vhc))
        {
            vhc->fuel = URANGE(0,vhc->fuel+number_range(1,3*vhc->in_building->level),vhc->max_fuel);
            vhc->hit = URANGE(0,vhc->hit+number_range(1,3*vhc->in_building->level),vhc->max_hit);
            vhc->ammo = URANGE(0,vhc->ammo+number_range(1,3*vhc->in_building->level),vhc->max_ammo);
            if ( IS_SET(vhc->flags,VEHICLE_CORROSIVE_A) )
                REMOVE_BIT(vhc->flags,VEHICLE_CORROSIVE_A);
        }
        if (( vhc->driving != NULL && vhc->driving->class == CLASS_DRIVER ) || IS_SET(vhc->flags,VEHICLE_REGEN) )
        {
            int c=13;
            if ( vhc->driving && vhc->driving->class == CLASS_DRIVER )
                c = 13;
            else if (IS_SET(vhc->flags,VEHICLE_REGEN) )
                c = 6;
            if ( number_percent() < c )
                vhc->fuel = URANGE(0,vhc->fuel+1,vhc->max_fuel);
            if ( number_percent() < c )
                vhc->hit = URANGE(0,vhc->hit+1,vhc->max_hit);
            if ( number_percent() < c )
                vhc->ammo = URANGE(0,vhc->ammo+1,vhc->max_ammo);
        }
        if ( SPACE_VESSAL(vhc) )
        {
            if ( vhc->power[POWER_REPAIR] > 0 )
            {
                int x;
                x = vhc->max_hit / 100;
                x = (x * vhc->power[POWER_REPAIR]) / 300;
                vhc->hit = URANGE(0,vhc->hit+x,vhc->max_hit);
            }
            if ( vhc->state == VEHICLE_STATE_CHARGE && vhc->driving && vhc->z == Z_SPACE )
            {
                if ( vhc->value[0] >= 100 )
                {
                    int old = vhc->ammo_type;
                    vhc->ammo++;
                    vhc->ammo_type = 35;
                    do_shoot(vhc->driving,"");
                    vhc->ammo_type = old;
                    vhc->value[0] = -1;
                    vhc->state = VEHICLE_STATE_NORMAL;
                }
                vhc->value[0]+=number_range(1,1);
                /*				if ( vhc->value[0] == 10 || vhc->value[0] == 20 || vhc->value[0] == 40 || vhc->value[0] == 60 || vhc->value[0] == 90 || vhc->value[0] >= 95 )
                                {
                                    sprintf(buf,"Power: %d%%\n\r", vhc->value[0] );
                                    send_to_char(buf,vhc->driving);
                                }*/
            }
        }
        if ( !vhc->driving )
        {
            vhc->timer++;
            if ( vhc->timer >= 100000 )
                extract_vehicle(vhc,FALSE);
        }

        if ( continual_flight(vhc) && ( wch = vhc->driving ) != NULL )
        {
            if ( wch->in_vehicle == vhc )
            {
                if ( wch->c_sn == -1 && wch->c_level > -1 )
                    move_char(wch,wch->c_level);
            }
            else
            {
                vhc->driving = NULL;
            }
        }

        if ( IS_SET(vhc->flags,VEHICLE_CORROSIVE_A) )
        {
            vhc->hit -= 1;
            if ( vhc->hit <= 0 )
            {
                CHAR_DATA *ch = vhc->driving;
                if ( ch )
                    send_to_char( "The acid eats through your armor!\n\r", vhc->driving );
                extract_vehicle(vhc,TRUE);
                if ( ch && ch->z == Z_SPACE )
                {
                    send_to_char( "Your head swells, and BLOWS INTO A MILLION PIECES!\n\r", ch );
                    ch->hit = 0;
                    damage(ch,ch,ch->max_hit + 1, -1);
                }
                continue;
            }
        }

    }
    return;
}

void aggr_update( void )
{
    CHAR_DATA *wch;
    CHAR_DATA *wch_next;

    CREF( wch_next, CHAR_NEXT );

    for ( wch = first_char; wch != NULL; wch = wch_next )
    {
        wch_next = wch->next;
        if ( wch->is_free != FALSE )
            continue;

        if ( wch->fighttimer > 0 )
        {
            wch->fighttimer--;
            if ( wch->fighttimer == 0 )
            {
                if( IS_SET(wch->config,CONFIG_SOUND) )
                    send_to_char( "\n\r!!MUSIC(Off)", wch );
                if ( IS_SET(wch->pcdata->pflags,PLR_BASIC) )
                    respawn_buildings(wch);
            }
        }
        if ( wch->questtimer > 0 )
            wch->questtimer--;
        if ( wch->spectimer > 0 )
            wch->spectimer--;
        if ( wch->killtimer > 0 )
            wch->killtimer--;

        if ( IS_BUSY( wch ) )
        {
            if ( wch->c_sn == gsn_move )
            {
                if ( wch->class == CLASS_SPRINTER && wch->c_time % 2 == 0 )
                    wch->c_time--;
            }
            else if ( wch->c_sn == gsn_research || wch->c_sn == gsn_repair || wch->c_sn == gsn_chemistry || wch->c_sn == gsn_exresearch || wch->c_sn == gsn_computer)
            {
                if ( wch->class == CLASS_GENIUS )
                    wch->c_time--;
            }
            if ( --wch->c_time <= 0 )
            {
                if ( wch->c_sn == gsn_build )
                    act_build( wch, wch->c_level );
                else if ( wch->c_sn == gsn_move )
                    move_char( wch, wch->c_level );
                else if ( wch->c_sn == gsn_research )
                    act_research( wch, wch->c_level );
                else if ( wch->c_sn == gsn_repair )
                    act_repair( wch, wch->c_level );
                else if ( wch->c_sn == gsn_sneak )
                    act_sneak( wch, wch->c_level );
                else if ( wch->c_sn == gsn_dig )
                    act_dig( wch, wch->c_level );
                else if ( wch->c_sn == gsn_chemistry )
                    act_chemistry( wch, wch->c_level );
                else if ( wch->c_sn == gsn_target )
                    act_target( wch, wch->c_level );
                else if ( wch->c_sn == gsn_tunnel )
                    act_tunnel( wch, wch->c_level );
                else if ( wch->c_sn == gsn_computer )
                    act_computer( wch, wch->c_level );
                else if ( wch->c_sn == gsn_exresearch )
                    act_exresearch( wch, wch->c_level );
                else if ( wch->c_sn == gsn_paradrop )
                    act_paradrop( wch, wch->c_level );
                else if ( wch->c_sn == gsn_crack )
                    act_crack( wch, wch->c_level );
                else if ( wch->c_sn == gsn_hack )
                    act_hack( wch, wch->c_level );
                else if ( wch->c_sn == gsn_format )
                    act_format( wch, wch->c_level );
                else if ( wch->c_sn == gsn_scan )
                    act_scandir( wch, wch->c_level );
                else if ( wch->c_sn == gsn_spoof )
                    act_spoof( wch, wch->c_level );
                else if ( wch->c_sn == gsn_sresearch )
                    act_sresearch( wch, wch->c_level );
                else if ( wch->c_sn == gsn_mine )
                    act_mine( wch, wch->c_level );
/*
                else if ( wch->c_sn == gsn_mark )
                    act_mark( wch, wch->c_level );
*/
                else if ( wch->c_sn == gsn_fix )
                    act_fix( wch, wch->c_level );
                else if ( wch->c_sn == gsn_eng_check )
                    act_eng_check( wch, wch->c_level );
                else if ( wch->c_sn == gsn_oreresearch )
                    act_oresearch( wch, wch->c_level );
                else if ( wch->c_sn == gsn_practice )
                    act_practice( wch, wch->c_level );
                else if ( wch->c_sn == gsn_arm )
                    act_arm( wch, wch->c_level );
                else if ( wch->c_sn == gsn_dead )
                {
                    pdie(wch);
                    continue;
                }
                else
                {
                    send_to_char( "Unknown action. Please report what you were just doing to an imm.\n\r", wch );
                    wch->c_sn = -1;
                }

                if ( wch->c_sn == gsn_sneak || wch->c_sn == gsn_target || wch->c_sn == gsn_tunnel || wch->c_sn == gsn_practice || wch->c_sn == gsn_arm )
                    wch->c_sn = -1;
                if ( wch->c_sn == -1 )
                    check_queue(wch);
            }
        }                                                   // end if is_busy
    }
    CUREF( wch_next );
    return;
}

void maze_update( void )
{
    int x,y,c;
    bool more = TRUE;
    int dir = 1;
    CHAR_DATA *wch;
    static bool under;

    under = !under;
    for ( x = BORDER_SIZE;x < MAX_MAPS - BORDER_SIZE;x++ )
        for ( y = BORDER_SIZE;y < MAX_MAPS - BORDER_SIZE;y++ )
    {
        if ( map_table.type[x][y][Z_UNDERGROUND] != SECT_MAGMA || under )
            map_table.type[x][y][Z_UNDERGROUND] = SECT_UNDERGROUND;
    }
    if ( under )
        init_fields();

    for ( c = 0;c < MAX_MAPS / 10; c++ )
    {
        more = TRUE;
        x = number_range(4,MAX_MAPS-4);
        y = number_range(4,MAX_MAPS-4);
        dir = number_range(1,2);
        while ( more )
        {
            if ( map_bld[x][y][Z_UNDERGROUND] == NULL )
                if ( map_table.type[x][y][Z_UNDERGROUND] != SECT_MAGMA )
                    map_table.type[x][y][Z_UNDERGROUND] = SECT_NULL;
            if ( number_percent() < 50 )
            {
                if ( dir == 1 )
                    x += 1;
                else
                    x -= 1;
            }
            else
            {
                if ( dir == 1 )
                    y += 1;
                else
                    y -= 1;
            }
            if ( y < BORDER_SIZE || y > MAX_MAPS - BORDER_SIZE || x < BORDER_SIZE || x > MAX_MAPS - BORDER_SIZE || ( number_percent() == 1 && number_percent() < 4 ))
                more = FALSE;
        }
    }
    for ( wch = first_char;wch; wch = wch->next )
        if ( wch->z == Z_UNDERGROUND )
            map_table.type[x][y][Z_UNDERGROUND] = SECT_BURNED;

    return;
}

void rooms_update( void )
{
    int x,y,sect,z;

    for ( z = 0;z < Z_MAX;z++ )
    {

        for ( x = BORDER_SIZE;x < MAX_MAPS - BORDER_SIZE;x++ )
        {
            for ( y=BORDER_SIZE;y<MAX_MAPS-BORDER_SIZE;y++ )
            {
                if ( planet_table[z].system == 0 )
                    continue;
                sect = map_table.type[x][y][z];
                if ( sect == SECT_SNOW && number_percent() < 25 && z != Z_NEWBIE )
                    map_table.type[x][y][z] = SECT_SNOW_BLIZZARD;
                else if ( sect == SECT_SNOW_BLIZZARD && number_percent() < 25 )
                    map_table.type[x][y][z] = SECT_SNOW;

                /*Resources*/   if ( number_percent() < 10 )
                {
                }
                else if ( map_table.resource[x][y][z] != -1 )
                {
                }
                else if ( map_table.type[x][y][z] == SECT_MOUNTAIN )
                {
                    map_table.resource[x][y][z] = ITEM_IRON;
                }
                else if ( map_table.type[x][y][z] == SECT_ROCK )
                {
                    map_table.resource[x][y][z] = ITEM_ROCK;
                }
                else if ( map_table.type[x][y][z] == SECT_SNOW && number_percent() < 50 )
                {
                    map_table.resource[x][y][z] = ITEM_SILVER;
                }
                else if ( map_table.type[x][y][z] == SECT_SNOW )
                {
                    map_table.resource[x][y][z] = ITEM_ROCK;
                }
                else if ( map_table.type[x][y][z] == SECT_FOREST && number_percent() < 50 )
                {
                    map_table.resource[x][y][z] = ITEM_LOG;
                }
                else if ( ( map_table.type[x][y][z] == SECT_FIELD || map_table.type[x][y][z] == SECT_FOREST ) )
                {
                    map_table.resource[x][y][z] = ITEM_STICK;
                }
            }
        }
    }
    return;
}

extern void build_save_flush(void);

/*
 * Handle all kinds of updates.
 * Called once per pulse from game loop.
 * Random times to defeat tick-timing clients and players.
 */
void update_handler( void )
{
    static  int     pulse_objects;
    static  int     pulse_area;
    static  int     pulse_rooms;
    static  int     pulse_violence;
    static  int     pulse_point;
    static  int     pulse_gain;
    static  int     pulse_quest;
    static  int     pulse_time;
    static  int     pulse_bomb;
    static  int     pulse_backup;
    static  int     pulse_spec;
    //    static  int	    pulse_remap;
    extern  int     saving_area;

    if ( saving_area )
        build_save();                                       /* For incremental area saving */

    if ( --pulse_spec    <= 0 )
    {
        spec_update();
        pulse_spec = PULSE_SPEC;
    }

    if ( --pulse_gain    <= 0 )
    {
        gain_update();
        pulse_gain = PULSE_PER_SECOND * number_range(5,8);
    }

    if ( --pulse_area     <= 0 )
    {
        pulse_area      = number_range( PULSE_AREA / 2, 3 * PULSE_AREA / 2 );
        build_save_flush();
    }

    if ( --pulse_violence <= 0 )
    {
        alarm_update();
        pulse_violence  = PULSE_VIOLENCE;
    }

    if ( --pulse_rooms   <= 0 )
    {
        pulse_rooms  = number_range(PULSE_ROOMS-3,PULSE_ROOMS+3);
        //       generate_webpage();
        building_update();
    }

    if ( --pulse_point    <= 0 )
    {
        pulse_point     = PULSE_TICK;
        char_update     ( );
        rooms_update();
        save_objects(0);
        save_buildings();
        save_alliances();
    }
    if ( --pulse_backup    <= 0 )
    {
        pulse_backup = PULSE_BACKUP;

        if ( building_count > 2500 && !sysdata.killfest )
        {
            save_buildings_b(0);
            save_objects(3);
            save_vehicles(0);
        }
        else if ( building_count < 1000 && !sysdata.killfest )
        {
            do_backup(NULL,"load");
            load_sobjects(3);
        }
    }

    if ( --pulse_time    <= 0 )
    {
        pulse_time     = PULSE_TIME;
        maze_update();
    }
    /*    if ( --pulse_remap   <= 0 )
        {
        pulse_remap = PULSE_REMAP;
        init_fields();
        }*/
    if ( --pulse_objects <= 0 )
    {
        pulse_objects = PULSE_OBJECTS;
        obj_update();
    }
    if ( --pulse_bomb <= 0 )
    {
        pulse_bomb = PULSE_BOMB;
        bomb_update      ( );
        vehicle_update  ( );
    }
    if ( --pulse_quest <= 0 )
    {
        pulse_quest = PULSE_QUEST;
        quest_update    ( );
        mode_update();
    }
    aggr_update( );
    tail_chain( );
    return;
}

void quest_update( void )
{
    OBJ_DATA *obj = NULL;
    BUILDING_DATA *bld = first_building;
    int x=0,y=0,z=get_random_planet();
    int i = 0;
    char buf[MSL];
    int type,sect;
    bool protect = TRUE;

    if ( sysdata.showhidden > 0 && number_percent() < 10 )
    {
        if ( sysdata.showhidden == 1 )
            sysdata.showhidden++;
        else if ( sysdata.showhidden >= 5 )
            sysdata.showhidden--;
        else
        {
            if ( number_percent() < 50 )
                sysdata.showhidden++;
            else
                sysdata.showhidden--;

        }
    }
    if ( number_percent() < 15 )
        type = 1;
    else if ( number_percent() < 40 )
        type = 2;
    else if ( number_percent() < 30 )
        type = 4;
    //	else if ( number_percent() < 20 )
    //		type = 3;
    else
        type = 3;

    if ( quest_objs >= MAX_QUEST_ITEMS )
        return;

    if ( type == 1 || type == 3 )
    {
        for ( bld = last_building;bld;bld = bld->prev )
        {
            if ( !bld->owner )
                continue;
            i++;
            if ( number_percent() <= 2 && type == 1 && !CIVILIAN(bld) && bld->type != BUILDING_SPACE_CENTER )
            {
                obj = create_object( get_obj_index( OBJ_VNUM_BLUEPRINTS ), 0 );
                obj->level = URANGE(2, bld->level + 1, 4 );
                if ( obj->level < 4 && number_percent() < 3 )
                    obj->level++;
                obj->value[0] = bld->type;
                sprintf( buf, "Blueprints for %d%s level %s", obj->level, ( obj->level == 2 ) ? "nd" : ( obj->level == 3 ) ? "rd" : "th", capitalize(bld->name) );
                free_string( obj->short_descr );
                free_string( obj->description );
                free_string( obj->name        );
                obj->short_descr = str_dup(buf);
                obj->description = str_dup(buf);
                sprintf( buf, "Blueprint %s", bld->name );
                obj->name = str_dup(buf);
                obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
                break;
            }
            else if ( number_percent() < 10 && type == 3 )
            {
                obj = create_teleporter(bld,number_range(50,200));
                break;
            }
        }
    }
    else if ( type == 2 )
    {
        i = number_range(MIN_QUEST_OBJ,MAX_QUEST_OBJ);
        obj = create_object( get_obj_index(i), 0 );
        if ( i == 9 )                                       // Scope Upgrade
        {
            int x = number_range(1,25);
            obj->value[1] = x;
            sprintf( buf, "@@dA @@a%d@@cX@@d-Zoom Scope Installation@@N", x );
            free_string(obj->short_descr);
            free_string(obj->description);
            obj->short_descr = str_dup(buf);
            obj->description = str_dup(buf);
        }
        obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
    }
    else if ( type == 4 )
    {
        obj = create_object( get_obj_index(OBJ_VNUM_QP_TOKEN), 0 );
        obj->value[0] = 1;
        obj->value[1] = number_range(10,20) * 10;
        free_string(obj->short_descr);
        free_string(obj->description);
        sprintf( buf, "A %d-QP Token", obj->value[1] );
        obj->short_descr = str_dup(buf);
        obj->description = str_dup(buf);
        obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
    }
    else if ( type == 5 )
    {
        obj = create_locator(number_range(10,40));
    }
    else if ( type == 6 )
    {
        obj = make_quest_base(BUILDING_PORTAL,7,z);
        if ( obj == NULL )
            return;
        obj->quest_timer = 600;
        obj->quest_map = 10;
        for ( i=0;i<MAX_QUEST_ITEMS;i++ )
        {
            if ( quest_obj[i] == NULL )
            {
                quest_obj[i] = obj;
                break;
            }
        }
        return;
    }
    if ( obj == NULL )
        return;

    obj->z = z;
    x = -1;
    y = -1;
    while ( x <= BORDER_SIZE || y <= BORDER_SIZE || x >= MAX_MAPS - BORDER_SIZE || y >= MAX_MAPS - BORDER_SIZE || bld != NULL )
    {
        x = number_range(BORDER_SIZE+1,(MAX_MAPS-BORDER_SIZE)-1);
        y = number_range(BORDER_SIZE+1,(MAX_MAPS-BORDER_SIZE)-1);
        bld = get_building(x,y,obj->z);
    }
    move_obj(obj,x,y,obj->z);
    if ( type == 1 )
    {
        obj->quest_timer = 1000;
        obj->quest_map = 10 - obj->level;
    }
    else if ( type == 2 )
    {
        obj->quest_timer = 600;
        obj->quest_map = 10;
    }
    else if ( type == 3 )
    {
        obj->quest_timer = 300;
        obj->quest_map = 7;
    }
    else if ( type == 4 || type == 5 )
    {
        obj->quest_timer = 800;
        obj->quest_map = 9 - (obj->value[1] / 40);
    }
    obj->quest_timer = number_range(obj->quest_timer/2,obj->quest_timer * 2);

    sect = map_table.type[x][y][z];
    if ( ( sect == SECT_FIELD || sect == SECT_FOREST || sect == SECT_SNOW || sect == SECT_SAND || sect == SECT_ROCK )
        && number_percent() < 70 )
    {
        for ( x = obj->x - 7;x < obj->x + 7;x++ )
            for ( y = obj->y - 7;y < obj->y + 7;y++ )
                if ( get_building(x,y,z) || get_char_loc(x,y,z) )
                {
                    protect = FALSE;
                    break;
                }
        if ( protect )
        {
            BUILDING_DATA *bld;
            type = sect == SECT_FOREST ? BUILDING_WATCHTOWER : sect == SECT_SNOW ? BUILDING_SNOW_DIGGER : BUILDING_TURRET;

            bld = create_building(type);
            bld->x = obj->x;
            bld->y = obj->y;
            bld->z = obj->z;
            map_bld[bld->x][bld->y][bld->z] = bld;
            reset_building(bld,type);
        }
    }
    for ( i=0;i<MAX_QUEST_ITEMS;i++ )
    {
        if ( quest_obj[i] == NULL )
        {
            quest_obj[i] = obj;
            break;
        }
    }
    return;
}

void explode( OBJ_DATA *obj )
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;
    BUILDING_DATA *bld;
    VEHICLE_DATA *vhc;
    CHAR_DATA *ch;
    OBJ_DATA *obj2;
    int dam = 0;
    int x,y,z = obj->z;
    bool ex = TRUE;
    OBJ_DATA *obj_next;
    char buf[MSL];
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    ch = get_ch(obj->owner);
    if ( obj->carried_by != NULL )
    {
        ch = obj->carried_by;
        obj->x = ch->x;
        obj->y = ch->y;
        obj->z = ch->z;
        if ( ch->z == Z_PAINTBALL && ch->x == 2 && ch->y == 2 )
        {
            extract_obj(obj);
            damage(ch,ch,1,-1);
            return;
        }
    }
    if ( ch == NULL || obj->x < BORDER_SIZE || obj->y < BORDER_SIZE || number_percent() < obj->value[6] )
    {
        sprintf(buf,"Smoke rises from %s. It didn't explode.\n\r",obj->short_descr);
        send_to_loc(buf, obj->x,obj->y,obj->z);
        extract_obj(obj);
        return;
    }
    if ( obj->z != Z_SPACE )
    {
        for ( obj2 = map_obj[obj->x][obj->y];obj2;obj2 = obj_next )
        {
            obj_next = obj2->next_in_room;
            if ( obj2->item_type == ITEM_BOMB && obj2->z == obj->z && obj2 != obj )
                move_obj(obj2,0,0,1);
        }
    }
    if ( IS_SET(ch->effect,EFFECT_BOMBER) )
    {
        obj->weight += number_range(0,obj->weight);
        obj->value[2] += number_range(0,obj->value[2]);
    }
    if ( obj->pIndexData->vnum == OBJ_VNUM_SHOCK_BOMB || obj->pIndexData->vnum == OBJ_VNUM_DIRTY_BOMB )
    {
        int x,y,maxx,q;
        int type = (obj->pIndexData->vnum == OBJ_VNUM_DIRTY_BOMB)?1:0;
        maxx = 3;
        sprintf( buf, "%s explodes, the ground starts to shake!\n\r", obj->short_descr );

        dam = dice(obj->weight,obj->value[2]);
        if ( obj->carried_by != NULL )
            dam *= 1.5;

        for ( x = obj->x - maxx;x <= obj->x + maxx;x++ )
            for ( y = obj->y - maxx;y <= obj->y + maxx;y++ )
        {
            if ( x < BORDER_SIZE || y < BORDER_SIZE || x > MAX_MAPS - BORDER_SIZE || y > MAX_MAPS -BORDER_SIZE )
                continue;
            if ( type == 0 )
            {
                for ( vch = map_ch[x][y][z];vch;vch = vch_next )
                {
                    vch_next = vch->next_in_room;
                                                            //Add planets here
                    if ( obj->z == Z_GROUND && (vch->z != 1 && vch->z != 0 ))
                        continue;
                    if ( ( obj->z != Z_GROUND && obj->z != Z_UNDERGROUND ) && obj->z != vch->z )
                        continue;
                    if ( obj->z != Z_GROUND && obj->z != Z_UNDERGROUND && obj->z != vch->z )
                        continue;
                    send_to_char( buf, vch );
                    act( "The ground starts shaking!", vch, NULL, NULL, TO_CHAR );
                    q = number_range(0,3);
                    move_char(vch,q);
                    q = number_range(0,3);
                    move_char(vch,q);
                    q = number_range(0,3);
                    move_char(vch,q);
                    damage( ch, vch, dam,-1 );
                }
            }
            else
            {
                bld = map_bld[x][y][obj->z];
                if ( !bld )
                    continue;
                if ( ( vch = get_ch(bld->owned) ) == NULL && !is_evil(bld) )
                    continue;
                bld->value[9] = number_range(obj->level/3*2,obj->level);
                if ( x == obj->x && y == obj->y )
                    damage_building(ch,bld,dam);
                else
                    damage_building(ch,bld,dam/3);
                for ( vch = map_ch[x][y][z];vch;vch = vch_next )
                {
                    vch_next = vch->next_in_room;
                    if ( vch->z != obj->z )
                        continue;
                    send_to_char( "A nuclear mushroom-blast surrounds the area!\n\r", vch );
                    damage(ch,vch,dam,DAMAGE_BLAST);
                }
            }
        }
        extract_obj(obj);
        return;
    }
    if ( obj->pIndexData->vnum == OBJ_VNUM_FLASH_GRENADE )
    {
        sprintf( buf, "%s explodes in a really really bright flash!\n\r", obj->short_descr );
        for ( vch = map_ch[obj->x][obj->y][obj->z];vch;vch = vch_next )
        {
            vch_next = vch->next_in_room;
            if ( NOT_IN_ROOM(vch,obj) )
                continue;
            send_to_char( buf, vch );
            if ( vch->in_vehicle != NULL )
                continue;
            if ( ( obj2 = get_eq_char( vch, WEAR_EYES ) ) != NULL )
            {
                if ( obj2->item_type == ITEM_ARMOR && obj2->value[0] == -1 )
                {
                    act( "$p protect you from the flash.", ch, obj2, NULL, TO_CHAR );
                    continue;
                }
            }
            act( "You are blinded by a bright flash!", vch, obj, NULL, TO_CHAR );
            dam = dice(obj->weight,obj->value[2]);
            if ( number_percent() < 40 && !IS_SET(vch->effect,EFFECT_BLIND) )
            {
                send_to_char( "You are blinded from the flash!\n\r", vch );
                SET_BIT(vch->effect,EFFECT_BLIND);
            }
            damage( ch, vch, dam,-1 );
        }
        extract_obj(obj);
        return;
    }
    if ( obj->pIndexData->vnum == OBJ_VNUM_SMOKE_BOMB )
    {
        sprintf( buf, "%s explodes, releasing toxic smoke!\n\r", obj->short_descr );
        for ( vch = map_ch[obj->x][obj->y][obj->z];vch;vch = vch_next )
        {
            vch_next = vch->next_in_room;
            if ( NOT_IN_ROOM(vch,obj))
                continue;
            send_to_char( buf, vch );
            if ( vch->in_vehicle != NULL || vch == ch )
                continue;
            if ( ( obj2 = get_eq_char( vch, WEAR_FACE ) ) != NULL )
            {
                if ( obj2->item_type == ITEM_ARMOR && obj2->value[0] == -1 )
                {
                    act( "$p protect you from the gas.", ch, obj2, NULL, TO_CHAR );
                    continue;
                }
            }
            act( "You choke on the gas!", vch, obj, NULL, TO_CHAR );
            dam = dice(obj->weight,obj->value[2]) * 2;

            damage( ch, vch, dam,-1 );
            set_stun(vch,24);
        }
        extract_obj(obj);
        return;
    }
    if ( obj->pIndexData->vnum == OBJ_VNUM_BIO_GRENADE )
    {
        sprintf( buf, "%s explodes, releasing a green cloud!\n\r", obj->short_descr );
        for ( vch = map_ch[obj->x][obj->y][obj->y];vch;vch = vch_next )
        {
            vch_next = vch->next_in_room;
            if ( NOT_IN_ROOM(vch,obj))
                continue;
            send_to_char( buf, vch );
            if ( vch->in_vehicle != NULL )
                continue;
            if ( ch->disease <= 20 )
                send_to_char( "It didn't seem to have an effect.\n\r", ch );
            else
            {
                ch->disease = 30;
                act( "You feel weaker!", vch, obj, NULL, TO_CHAR );
                act( "$n looks weaker.", vch, NULL, NULL, TO_ROOM );
            }
        }
        extract_obj(obj);
        return;
    }
    if ( obj->pIndexData->vnum == OBJ_VNUM_BURN_GRENADE )
    {
        sprintf( buf, "%s explodes, releasing a stinging chemical!\n\r", obj->short_descr );
        for ( vch = map_ch[obj->x][obj->y][obj->z];vch;vch = vch_next )
        {
            vch_next = vch->next_in_room;
            if ( NOT_IN_ROOM(vch, obj))
                continue;
            send_to_char( buf, vch );
            if ( vch->in_vehicle != NULL )
                continue;

            dam = dice(obj->weight,obj->value[2]);
            damage( ch, vch, dam,DAMAGE_FLAME );

            if ( IS_SET(ch->effect,EFFECT_BLIND) || number_percent() < 30 )
                send_to_char( "It didn't seem to have any other effect.\n\r", ch );
            else
            {
                SET_BIT(ch->effect,EFFECT_BLIND);
                act( "That stuff got in your eyes, you have been blinded!", vch, obj, NULL, TO_CHAR );
                act( "$n has been blinded.", vch, NULL, NULL, TO_ROOM );
            }
        }
        extract_obj(obj);
        return;
    }
    if ( obj->item_type == ITEM_BOMB )
    {
        if ( ch->in_vehicle != NULL && obj->carried_by == ch )
        {
            if ( !IS_SET(ch->in_vehicle->flags,VEHICLE_EXPLOSIVE) )
            {
                act( "The bomb doesn't do anything from inside the vehicle!", ch, NULL, NULL, TO_CHAR );
                act( "You hear a large BOOM from inside $t!", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
                obj_from_char(obj);
                obj_to_room(obj,ch->in_room);
                ch->in_vehicle->hit /= 2;
                ch->in_vehicle->driving = NULL;
                ch->in_vehicle = NULL;
                damage( ch, ch, 10000, -1 );
                extract_obj(obj);
                return;
            }
            else
            {
                act( "$t goes KABOOM!", ch, ch->in_vehicle->desc, NULL, TO_ROOM );
                act( "$t goes KABOOM!", ch, ch->in_vehicle->desc, NULL, TO_CHAR );
                ch->in_vehicle->driving = NULL;
                vhc = ch->in_vehicle;
                ch->in_vehicle = NULL;
                extract_vehicle(vhc,FALSE);
                obj->weight *= 2;
            }
        }
        vch = get_char_loc(obj->x,obj->y,obj->z);
        if ( obj->carried_by == NULL && obj->value[8] == 0 )
        {
            act( "$p EXPLODES!!!", ( vch ) ? vch : NULL, obj, NULL, TO_ROOM );
            act( "$p EXPLODES!!!", ( vch ) ? vch : NULL, obj, NULL, TO_CHAR );
        }
        else
        {
            act( "$n takes a last breath, and EXPLODES into a million pieces!", obj->carried_by, NULL, NULL, TO_ROOM );
            act( "With your last breath, you gaze as the entire world around you goes up in flames.", obj->carried_by, NULL, NULL, TO_CHAR );
        }
        for ( x=obj->x-1;x<=obj->x+1;x++ )
            for ( y=obj->y-1;y<=obj->y+1;y++ )
                if ( x > 1 && y > 1 && x <= MAX_MAPS-BORDER_SIZE && y <= MAX_MAPS - BORDER_SIZE )
                    for ( vch = map_ch[x][y][obj->z];vch;vch = vch_next )
                        //		for ( vch = first_char;vch;vch = vch_next )
                        {
                            vch_next = vch->next_in_room;
                            if ( obj->value[3] == 3 )
                                break;
                            if ( vch == ch )
                                continue;

                            act( "You are blasted by the force of $p!", vch, obj, NULL, TO_CHAR );
                            act( "$n is blasted by the force of $p!", vch, obj, NULL, TO_ROOM );

                            dam = dice(obj->weight,obj->value[2]);

                            if ( vch->x != obj->x )
                                dam /= 2;
                            if ( vch->y != obj->y )
                                dam /= 2;

                            if ( obj->carried_by != NULL && vch == obj->carried_by )
                                dam = obj->carried_by->hit * 3;
                            if ( obj->carried_by != NULL )
                                dam *= 1.5;
                            else
                            if ( !str_cmp(obj->owner,ch->name) && dam >= ch->hit )
                                ex = FALSE;

                            damage( ch, vch, dam,DAMAGE_BLAST );
                        }
        if ( obj->z != Z_SPACE )
        {
            for ( x=obj->x-1;x<=obj->x+1;x++ )
                for ( y=obj->y-1;y<=obj->y+1;y++ )
                    if ( x > 1 && y > 1 && x <= MAX_MAPS-BORDER_SIZE && y <= MAX_MAPS - BORDER_SIZE )
                    {
                        bld = map_bld[x][y][obj->z];
                        if ( !bld || bld == NULL )
                            continue;
                        if ( !is_evil(bld) && ((vch = bld->owner) == NULL) )
                            continue;

                        dam = number_range((obj->weight*obj->value[2])/2,obj->weight*obj->value[2]);
                        if ( obj->pIndexData->vnum == 1029 )//deadman
                    //					dam /= 10;
                            continue;
                        if ( x != obj->x )
                            dam /= 2;
                        if ( y != obj->y )
                            dam /= 2;

                        if ( obj->value[3] == 1 )
                        {
                            bld->value[9] = number_range(obj->level/3*2,obj->level);
                            send_to_loc( "Nuclear fallout spreads everywhere!", bld->x, bld->y, bld->z );
                        }
                        if ( obj->value[3] == 2 && bld->type == BUILDING_DUMMY )
                        {
                            dam = 99999;
                            bld->type = BUILDING_TURRET;
                        }
                        if ( obj->carried_by != NULL )
                            dam *= 1.5;
                        if ( dam >= bld->hp + bld->shield )
                            ex = FALSE;
                        damage_building(ch,bld,dam);
                    }
        }
        if ( ch->position != POS_DEAD  )
        {
            vch = ch;
            if ( ( abs(ch->x-obj->x) <= 1 && abs(ch->y-obj->y) <= 1 && obj->z == ch->z )
                || obj->carried_by == vch)
            {
                act( "You are blasted by the force of $p!", vch, obj, NULL, TO_CHAR );
                act( "$n is blasted by the force of $p!", vch, obj, NULL, TO_ROOM );
                dam = dice(obj->weight,obj->value[2]);
                if ( obj->carried_by != NULL && vch == obj->carried_by )
                    dam = obj->carried_by->hit * 3;
                if ( vch->x != obj->x || vch->y != obj->y )
                    dam /= 10;
                if ( obj->carried_by == vch && vch->class == CLASS_SUICIDE_BOMBER )
                {
                    obj_from_char(obj);
                    obj_to_room(obj,vch->in_room);
                    damage(vch, vch, 100,DAMAGE_BLAST );
                    set_stun(vch,20);
                }
                else
                {
                    if ( obj->carried_by != NULL && vch == obj->carried_by )
                    {
                        obj_from_char(obj);
                        obj_to_room(obj,vch->in_room);
                    }
                    damage( ch, vch, dam,DAMAGE_BLAST );
                    if ( vch->position == POS_DEAD )
                        return;
                }
            }
        }
    }
    //	if (ex)
    extract_obj(obj);
    return;
}

bool fast_healing(CHAR_DATA *ch)
{
    if ( has_ability(ch,4) && number_percent() < 7 )
        return TRUE;
    return FALSE;
}

void mode_update()
{
    char buf[MSL];

    if ( sysdata.qpmode > 0 )
    {
        buf[0] = '\0';
        if ( --sysdata.qpmode % 20 == 0 )
        {
            if ( sysdata.qpmode / 4 == 0 )
                sprintf( buf, "QPMode is almost OVER.@@N\n\r" );
            else
                sprintf( buf, "QPMode is ENABLED for %d more minutes!@@N\n\r", sysdata.qpmode / 4 );
        }
        if ( buf[0] != '\0' )
            info(buf,0);
    }
    if ( sysdata.xpmode > 0 )
    {
        buf[0] = '\0';
        if ( --sysdata.xpmode % 20 == 0 )
        {
            if ( sysdata.xpmode / 4 == 0 )
                sprintf( buf, "XPMode is almost OVER.@@N\n\r" );
            else
                sprintf( buf, "XPMode is ENABLED for %d more minutes!@@N\n\r", sysdata.xpmode / 4 );
        }

        if ( buf[0] != '\0' )
            info(buf,0);
    }

    if ( number_percent() == 1 && number_percent() <= 50 )
    {
        if ( number_percent() < 50 )
        {
            if ( sysdata.qpmode <= 0 )
                do_qpmode(NULL,"10");
        }
        else
        {
            if ( sysdata.xpmode <= 0 )
                do_xpmode(NULL,"10");
        }
    }
    return;
}
