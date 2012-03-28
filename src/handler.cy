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
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "ack.h"

struct obj_ref_type *obj_ref_list;
int const  prof_times [] =
{ 0,3,10,50,100,150,200,250,300,400,600,-1 };


void obj_reference(struct obj_ref_type *ref)
{
  if (ref->inuse)
  {
    bugf("Reused obj_reference!");
    abort();
  }

  ref->inuse=TRUE;
  ref->next=obj_ref_list;
  obj_ref_list=ref;
}

void obj_unreference(OBJ_DATA **var)
{
  struct obj_ref_type *p, *last;

  for (p=obj_ref_list, last=NULL;
       p && p->var!=var;
       last=p,p=p->next)
    ;

  if (!p)
  {
    bugf("obj_unreference: var not found");
    return;
  }

  p->inuse=FALSE;

  if (!last)
    obj_ref_list=obj_ref_list->next;
  else
    last->next=p->next;
}

struct char_ref_type *char_ref_list;

void char_reference(struct char_ref_type *ref)
{
  if (ref->inuse)
  {
    bugf("Reused char_reference!");
    abort();
  }

  ref->inuse=TRUE;
  ref->next=char_ref_list;
  char_ref_list=ref;
}

void char_unreference(CHAR_DATA **var)
{
  struct char_ref_type *p, *last;

  for (p=char_ref_list, last=NULL;
       p && p->var!=var;
       last = p, p=p->next)
    ;

  if (!p)
  {
    bugf("char_unreference: var not found");
    return;
  }

  p->inuse=FALSE;

  if (!last)
    char_ref_list=char_ref_list->next;
  else
    last->next=p->next;
}



/*
 * Retrieve a character's trusted level for permission checking.
 */
int get_trust( CHAR_DATA *ch )
{
    return ch->trust;
}


/* 
 * Replacement for retrieving a character's age
 * Each tick = 1 mud hr.  (spaced at 1 minute rl)
 * 24 mud hrs = 1 mud day
 * 20 mud days = 1 mud month
 * 8 mud months = 1 mud year
 * Therefore, 24*20*8 = 3840 ticks/mins.
 * Returns a string with age info in for use by score, mst, etc
 */

void my_get_age( CHAR_DATA *ch, char *buf )
{
    int days, years, months;
    int base, ticks;
    
    /* Base = time in seconds ch has been playing... */
    base = ch->played + (int) (current_time - ch->logon );
    
    ticks = base/60;	/* 1 tick = 60 seconds */
    
    days   =      ( ticks /   HOURS_PER_DAY ) % DAYS_PER_MONTH;
    months =      ( ticks /  (HOURS_PER_DAY * DAYS_PER_MONTH)) % MONTHS_PER_YEAR;
    years  = 17 + ( ticks /  (HOURS_PER_DAY * DAYS_PER_MONTH * MONTHS_PER_YEAR )); 
    
    sprintf( buf+strlen(buf), "%d years, %d months and %d days", 
       years, months, days );
    return;
}

/* Simple function to return number of hours a character has played */
int my_get_hours(CHAR_DATA *ch, bool total )
{
   int secs;
   int hrs;
   
   if ( total == FALSE )
   	secs = ch->played + (int) (current_time - ch->logon );
   else
   	secs = ch->played_tot + (int) (current_time - ch->logon );
   hrs  = ( secs / 3600 );
   
   return hrs;
}
int my_get_minutes(CHAR_DATA *ch, bool total )
{
	int mins;
	if ( total == FALSE )
		mins = ( ch->played + (int) (current_time - ch->logon ) ) / 60;
	else
		mins = ( ch->played_tot + (int) (current_time - ch->logon ) ) / 60;
	return mins;
}
    


/*
 * Retrieve a character's age.
 */
int get_age( CHAR_DATA *ch )
{
    return 17 + ( ch->played + (int) (current_time - ch->logon) ) / 14400;

    /* 12240 assumes 30 second hours, 24 hours a day, 20 day - Kahn */
}

/*
 * Retrieve a character's carry capacity.
 */
int can_carry_n( CHAR_DATA *ch )
{
    int n = 50;
    if ( ch->level >= LEVEL_IMMORTAL )
	return 500;

    if ( ch->in_vehicle && ch->in_vehicle->type == VEHICLE_MECH )
	n += 50;
    if ( IS_IMPLANTED(ch,IMPLANT_METAL_ARM_3) )
	n += 10;
    if ( IS_IMPLANTED(ch,IMPLANT_METAL_ARM_2) )
	n += 5;
    if ( IS_IMPLANTED(ch,IMPLANT_METAL_ARM_1) )
	n += 5;
    if ( ch->class == CLASS_GENIUS )
	n -= 20;

    return n;
}



/*
 * Retrieve a character's carry capacity.
 */
int can_carry_w( CHAR_DATA *ch )
{
    int weight;

    if ( ch->level >= LEVEL_IMMORTAL )
	return 9999999;

    if ( ch->in_vehicle && ch->in_vehicle->type == VEHICLE_MECH )
	return 500;

    weight = 350;
    
    if ( ch->disease > 20 && ch->disease <= 30 )
	weight -= 250;
    if ( ch->class == CLASS_ROBOTIC )
	weight += 100;
    else if ( ch->class == CLASS_GENIUS )
	weight -= 100;

    if ( IS_IMPLANTED(ch,IMPLANT_METAL_ABS_1) )
	weight += 30;
    if ( IS_IMPLANTED(ch,IMPLANT_METAL_ABS_2) )
	weight += 30;;
    if ( IS_IMPLANTED(ch,IMPLANT_METAL_ABS_3) )
	weight += 30;

    return weight;
}

/*
 * Move a char out of a room.
 */
void char_from_room( CHAR_DATA *ch )
{
    CHAR_DATA *wch;
    int i = 0;

    if ( map_ch[ch->x][ch->y][ch->z] == ch->list )
    	map_ch[ch->x][ch->y][ch->z] = ch->next_in_room;
    else
    for ( wch = get_char_from_list(map_ch[ch->x][ch->y][ch->z]); wch != NULL; wch = get_char_from_list(wch->next_in_room) )
    {
	if ( i > 50 )
		break;
	if ( wch->next_in_room == ch->list )
	    wch->next_in_room = ch->next_in_room;
	i++;
    }

    ch->in_room      = NULL;
    return;
}



/*
 * Move a char into a room.
 */
void char_to_room( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoomIndex )
{
    ch->in_room         = pRoomIndex;
    return;
}



/*
 * Give an obj to a char.
 */
void obj_to_char( OBJ_DATA *obj, CHAR_DATA *ch )
{
    obj->next_in_carry_list = NULL;
    obj->prev_in_carry_list = NULL;

    LINK(obj, ch->first_carry, ch->last_carry, next_in_carry_list, prev_in_carry_list);
    obj->carried_by      = ch;
    obj->in_room         = NULL;
    obj->in_building	 = NULL;
    ch->carry_number    += get_obj_number( obj );
    ch->carry_weight    += get_obj_weight( obj );
    obj->x = ch->x;
    obj->y = ch->y;
    obj->z = ch->z;
    if ( obj->item_type != ITEM_BOMB || obj->value[1] <= 0 )
    {
    	free_string(obj->owner);
    	obj->owner = str_dup(ch->name);
    }
    if ( obj->wear_loc != WEAR_NONE && ch )
	ch->heat += obj->heat;
}


/*
 * Take an obj from its character.
 */
void obj_from_char( OBJ_DATA *obj )
{
    CHAR_DATA *ch;

    if ( ( ch = obj->carried_by ) == NULL )
    {
	char buf[MAX_STRING_LENGTH];
	sprintf( buf, "obj_from_char: NULL ch to remove %s from.",
	   obj->short_descr );
	monitor_chan( NULL, buf, MONITOR_OBJ );
	
	bug( "Obj_from_char: null ch.", 0 );
	return;
    }

    if ( obj->wear_loc != WEAR_NONE )
	unequip_char( ch, obj );

    UNLINK(obj, ch->first_carry, ch->last_carry,
           next_in_carry_list, prev_in_carry_list);

    free_string(obj->owner);
    obj->owner = str_dup(obj->carried_by->name);
    obj->carried_by      = NULL;
    obj->next_in_carry_list    = NULL;
    obj->prev_in_carry_list	 = NULL;
    obj->in_room = NULL;
    obj->x = ch->x;
    obj->y = ch->y;
    obj->z = ch->z;
    obj->in_building = ch->in_building;

    ch->carry_number    -= get_obj_number( obj );
    ch->carry_weight    -= get_obj_weight( obj );
    return;
}



/*
 * Find the ac value of an obj, including position effect.
 */
int apply_ac( OBJ_DATA *obj, int iWear )
{
    if ( obj->item_type != ITEM_ARMOR )
	return 0;

    switch ( iWear )
    {
    case WEAR_BODY:     return 3 * obj->value[0];
    case WEAR_HEAD:     return 2 * obj->value[0];
    case WEAR_LEGS:     return 2 * obj->value[0];
    case WEAR_FEET:     return     obj->value[0];
    case WEAR_HANDS:    return     obj->value[0];
    case WEAR_ARMS:     return     obj->value[0];
    case WEAR_HOLD_HAND_R:   return     obj->value[0];
    case WEAR_FINGER_L: return     obj->value[0];
    case WEAR_FINGER_R: return     obj->value[0];
    case WEAR_NECK_1:   return     obj->value[0];
    case WEAR_NECK_2:   return     obj->value[0];
    case WEAR_ABOUT:    return 2 * obj->value[0];
    case WEAR_WAIST:    return     obj->value[0];
    case WEAR_WRIST_L:  return     obj->value[0];
    case WEAR_WRIST_R:  return     obj->value[0];
    case WEAR_HOLD_HAND_L:     return     obj->value[0];
    }

    return 0;
}



/*
 * Find a piece of eq on a character.
 */
OBJ_DATA *get_eq_char( CHAR_DATA *ch, int iWear )
{
    OBJ_DATA *obj;

    for ( obj = ch->first_carry; obj != NULL; obj = obj->next_in_carry_list )
    {
	if ( obj->wear_loc == iWear )
	    return obj;
    }

    return NULL;
}



/*
 * Equip a char with an obj.
 */
void equip_char( CHAR_DATA *ch, OBJ_DATA *obj, int iWear )
{
    char log[MAX_STRING_LENGTH];

    if ( paintball(ch) )
    {
	send_to_char( "You cannot wear items in paintball mode!\n\r", ch );
	return;
    }
    if (  ( ch->desc && ch->desc->connected != CON_SETTING_STATS )
       && ( get_eq_char( ch, iWear ) != NULL )  )
    {
	sprintf( log, "equip_char: %s (room %d) cannot be equiped with %s, as wear slot (%d) not empty.",
	   NAME(ch), ch->in_room->vnum, obj->short_descr, iWear );
	monitor_chan( ch, log, MONITOR_OBJ );
		
	bug( log, 0 );
	return;
    }

    obj->wear_loc        = iWear;
    ch->heat += obj->heat;
    return;
}



/*
 * Unequip a char with an obj.
 */
void unequip_char( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( paintball(ch) )
    {
	send_to_char( "You cannot remove items in paintball mode.\n\r", ch );
	return;
    }
    if ( obj->wear_loc == WEAR_NONE )
    {
	char buf[MAX_STRING_LENGTH];
	sprintf( buf, "unequip_char: %s is not wearing %s.",
	   NAME(ch), obj->short_descr );
	monitor_chan( ch, buf, MONITOR_OBJ );
	
	bug( "Unequip_char: already unequipped.", 0 );
	return;
    }
    if ( obj->item_type == ITEM_WEAPON && ch->victim != ch )
    {
	send_to_char( "You have lost your target.\n\r", ch );
	ch->victim = ch;
    }

    obj->wear_loc        = -1;
    ch->heat 		-= obj->heat;

    if ( ch->hit > ch->max_hit )
      ch->hit = ch->max_hit;

    return;
}



/*
 * Count occurrences of an obj in a list.
 */
int count_obj_list( OBJ_INDEX_DATA *pObjIndex, OBJ_DATA *list )
{
    OBJ_DATA *obj;
    int nMatch;

    nMatch = 0;
    for ( obj = list; obj != NULL; obj = obj->next_in_carry_list )
    {
	if ( obj->pIndexData == pObjIndex )
	    nMatch++;
    }

    return nMatch;
}

/*
 * Move an obj out of a room.
 */
void obj_from_room( OBJ_DATA *obj )
{
    ROOM_INDEX_DATA *in_room;
    BUILDING_DATA *bld;

    if ( ( in_room = obj->in_room ) == NULL )
    {
	char buf[MAX_STRING_LENGTH];
	sprintf( buf, "obj_from_room: %s in NULL room.", obj->short_descr );
	monitor_chan( NULL, buf, MONITOR_OBJ );
	
	bug( "obj_from_room: NULL.", 0 );
/* attempt to recover by moving obj to another room */
        if ( obj->carried_by != NULL )
         obj_from_char( obj );

	obj_to_room( obj, get_room_index( ROOM_VNUM_LIMBO ) );
        if ( ( in_room = obj->in_room ) == NULL )
        {
          sprintf( buf, "obj_from_room, %s really screwed up, failed attempts to move to Limbo.", obj->short_descr );
          monitor_chan( NULL, buf, MONITOR_OBJ );
          return;
        }
        /* code to save everyone here  Zen */
    }

    obj->in_room      = NULL;
    obj->next_in_carry_list = NULL;
    obj->prev_in_carry_list = NULL;
    obj->carried_by = NULL;
    if ( ( bld = get_building(obj->x,obj->y,obj->z) ) != NULL && is_evil(bld) && bld->z != Z_MEDAL && bld->timer <= 0 )
	extract_building(bld,TRUE);
    {
	extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
	OBJ_DATA *obj2;

	if ( obj == map_obj[obj->x][obj->y] )
		map_obj[obj->x][obj->y] = obj->next_in_room;
	else
		for ( obj2 = map_obj[obj->x][obj->y];obj2;obj2 = obj2->next_in_room )
			if ( obj2->next_in_room && obj2->next_in_room == obj )
				obj2->next_in_room = obj->next_in_room;
    }

    return;
}



/*
 * Move an obj into a room.
 */
void obj_to_room( OBJ_DATA *obj, ROOM_INDEX_DATA *pRoomIndex )
{
  obj->in_room = pRoomIndex;
  obj->carried_by = NULL;
  obj->next_in_carry_list = NULL;
  obj->prev_in_carry_list = NULL;
  move_obj(obj,obj->x,obj->y,obj->z);
  return;
}

/*
 * Extract an obj from the world.
 */
void extract_obj( OBJ_DATA *obj )
{
  struct obj_ref_type *ref;
   if ( obj == NULL || !obj )
	return;
   if (!( obj->is_free == FALSE && ( !obj->next || obj->next->prev == obj ) && ( !obj->prev || obj->prev->next == obj ) ))
   {
	char buf[MSL];
	sprintf(buf,"%s - Bad extract: %d/%d/%d, carried by %s.", obj->short_descr,obj->x,obj->y,obj->z,(obj->carried_by)?obj->carried_by->name:"nobody");
	log_f(buf);
	move_obj(obj,0,1,Z_GROUND);
	return;
   }
   if ( obj->quest_timer > 0 )
   {
	int i;
	for ( i=0;i<MAX_QUEST_ITEMS;i++ )
		if ( quest_obj[i] == obj )
			quest_obj[i] = NULL;
   }
   if ( obj->x != 0 || obj->y != 0 || obj->z != 1 )
   {
	if ( obj->carried_by )
	{
		obj_from_char(obj);
		obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
	}
/*	if ( map_bld[obj->x][obj->y][obj->z] && map_bld[obj->x][obj->y][obj->z]->type == BUILDING_WAREHOUSE )
	{
		char buf[MSL];
		sprintf(buf,"%s extracted from %s's warehouse.\n\r", obj->short_descr, map_bld[obj->x][obj->y][obj->z]->owned );
		log_f(buf);
	}*/
	move_obj(obj,0,0,1);
	return;
   }
   for (ref=obj_ref_list; ref; ref=ref->next)
    if (*ref->var==obj)
      switch (ref->type)
      {
      case OBJ_NEXT:
        *ref->var=obj->next;
        break;
      case OBJ_NEXTCONTENT:
        *ref->var=obj->next_in_carry_list;
        break;
      case OBJ_NULL:
        *ref->var=NULL;
        break;
      default:
        bugf("Bad obj_ref_list type %d", ref->type);
        break;
      }
   
    
    if  ( obj->carried_by != NULL )
	obj_from_char( obj );
    else if
        ( obj->in_room != NULL )
	obj_from_room( obj );

    {
	extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];
	OBJ_DATA *obj2;

	if ( obj == map_obj[obj->x][obj->y] )
		map_obj[obj->x][obj->y] = obj->next_in_room;
	else
		for ( obj2 = map_obj[obj->x][obj->y];obj2;obj2 = obj2->next_in_room )
			if ( obj2->next_in_room && obj2->next_in_room == obj )
				obj2->next_in_room = obj->next_in_room;
    }

    if ( obj->bomb_data )
    {
	BOMB_DATA *bomb = obj->bomb_data;
	if ( ( bomb->next && bomb->next->prev != bomb ) || ( bomb->prev && bomb->prev->next != bomb ) )
	{
		char buf[MSL];
		sprintf( buf, "Bad bomb extract (%s)", obj->short_descr);
		log_f(buf);
	}
	else
		UNLINK(bomb,first_bomb,last_bomb,next,prev);
	PUT_FREE(bomb,bomb_free);
    }
    if ( obj->is_free == FALSE && ( !obj->next || obj->next->prev == obj ) && ( !obj->prev || obj->prev->next == obj ) )
    {
    	UNLINK(obj, first_obj, last_obj, next, prev);

    	PUT_FREE(obj, obj_free);
    }
    return;
}


/*
 * Extract a char from the world.
 */
void extract_char( CHAR_DATA *ch, bool fPull )
{
    CHAR_DATA *wch;
    OBJ_DATA * this_object;
    BUILDING_DATA *bld;
    BUILDING_DATA *bld_next;
    struct char_ref_type *ref;

    if ( ch == NULL )
    {
      char buf[MAX_STRING_LENGTH];
	sprintf( buf, "extract_char: %s in NULL room., Moved to room 2", NAME(ch) );
	monitor_chan( NULL, buf, MONITOR_BAD );
	bug( "Extract_char: NULL.", 0 ); 
	 return;  
    }

    if ( practicing(ch) )
	do_practice(ch,"");
    if ( ch->bvictim && ch->bvictim->value[8] != 0 )
	ch->bvictim->value[8] = 0;

    {
	QUEUE_DATA * q;
	QUEUE_DATA * q_next;
	for ( q = ch->pcdata->queue;q;q = q_next )
	{
		q_next = q->next;
		extract_queue(q);
	}
    }
  ch->is_free = FALSE;
  for (ref=char_ref_list; ref; ref=ref->next)
    if (*ref->var==ch)
      switch (ref->type)
      {
      case CHAR_NEXT:
        *ref->var=ch->next;
        break;
      case CHAR_NULL:
        *ref->var=NULL;
        break;
      default:
        bugf("Bad char_ref_list type %d", ref->type);
        break;
      }

    if ( fPull )
	die_follower( ch );

    for ( bld = ch->first_building;bld;bld = bld_next )
    {
	bld_next = bld->next_owned;
	if ( is_neutral(bld->type) )
		continue;
	if ( bld->timer > 0 )
		continue;
	activate_building(bld,FALSE);
    }

    if ( ch->in_vehicle != NULL )
    {
	VEHICLE_DATA *vhc = ch->in_vehicle;
	ch->in_vehicle->driving = NULL;
	ch->in_vehicle = NULL;
	extract_vehicle(vhc,FALSE);
    }
    ch->is_quitting = TRUE;
    while ( ( this_object = ch->last_carry ) != NULL )
      extract_obj( this_object );

    char_from_room( ch );
    del_char_from_list(ch);

    if ( ch->desc != NULL && ch->desc->original != NULL )
	do_return( ch, "" );

    if ( map_ch[ch->x][ch->y][ch->z] == ch->list )
    {
	if ( ch->next_in_room == ch->list )
		map_ch[ch->x][ch->y][ch->z] = -1;
	else
	    	map_ch[ch->x][ch->y][ch->z] = ch->next_in_room;
    }
    for ( wch = first_char; wch != NULL; wch = wch->next )
    {
	if ( wch->next_in_room == ch->list )
	{
	    	wch->next_in_room = ch->next_in_room;
	}
	if ( wch->reply == ch )
	    wch->reply = NULL;
	if ( wch->victim == ch )
	    wch->victim = wch;
	if ( wch->bvictim && !str_cmp(wch->bvictim->owned,ch->name) )
	{
	    send_to_char( "Mainframe shut down. Connection Terminated...\n\r", ch );
	    wch->bvictim->value[8] = 0;
	    wch->position =POS_STANDING;
	    wch->bvictim = NULL;
	    wch->c_sn = -1;
	}
    }

    UNLINK(ch, first_char, last_char, next, prev);

    if ( ch->desc )
	ch->desc->character = NULL;
    free_char( ch );
    return;
}


/*
 * Find a char in the room.
 */
CHAR_DATA *get_char_room( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *rch;
    int number;
    int count;
    char names[MSL];

    number = number_argument( argument, arg );
    count  = 0;
    if ( arg[0] == '\0' )
      return NULL;

    if ( !str_cmp( arg, "self" ) )
	return ch;

    for ( rch = get_char_from_list(map_ch[ch->x][ch->y][ch->z]); rch != NULL; rch = get_char_from_list(rch->next_in_room) )
    {
	sprintf( names, "%s", rch->name );
	if ( NOT_IN_ROOM( ch, rch ) )
		continue;

	if ( !( is_name( arg, names ) ) )
		continue;

	if ( !can_see( ch, rch ) )
	    continue;

	if ( ++count == number )
	    return rch;
    }

    return NULL;
}




/*
 * Find a char in the world.
 */
CHAR_DATA *get_char_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    CHAR_DATA *wch;
    int number;
    int count;

    if ( argument[0] == '\0' )
	return NULL;

    if ( !str_cmp(argument,"self") )
	return ch;
    number = number_argument( argument, arg );
    count  = 0;
    for ( wch = first_char; wch != NULL ; wch = wch->next )
    {
	if ( !is_name( arg, wch->name ) || !can_see(ch,wch))
		continue;
	if ( ++count == number )
	    return wch;
    }

    return NULL;
}

CHAR_DATA *get_char( CHAR_DATA *ch )
{
    if ( !ch->pcdata )
        return ch->desc->original; 
    else
        return ch;
}


/*
 * Find some object with a given index data.
 * Used by area-reset 'P' command.
 */
OBJ_DATA *get_obj_type( OBJ_INDEX_DATA *pObjIndex )
{
    OBJ_DATA *obj;

    for ( obj = first_obj; obj != NULL; obj = obj->next )
    {
	if ( obj->pIndexData == pObjIndex )
	    return obj;
    }

    return NULL;
}


/*
 * Find an obj in a room.
 */
OBJ_DATA *get_obj_room( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    if ( argument[0] == '\0' )
	return NULL;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj != NULL; obj = obj->next_in_room )
    {
	if ( is_name( arg, obj->name ) && !NOT_IN_ROOM(obj,ch) && obj->carried_by == NULL )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    if ( map_table.resource[ch->x][ch->y][ch->z] != -1 && ch->in_building == NULL )
    {
	int type = map_table.resource[ch->x][ch->y][ch->z];
	obj = create_material(type);
	if ( is_name(arg,obj->name) || !str_cmp(arg,"all") )
	{
		obj->x = ch->x;
		obj->y = ch->y;
		obj->z = ch->z;
		obj->in_building = NULL;
		obj->carried_by = NULL;
		map_table.resource[ch->x][ch->y][ch->z] = -1;
		obj_to_room(obj,get_room_index(ROOM_VNUM_WMAP));
		return obj;
	}
    }
    return NULL;
}

/*
 * Find an obj in a room.
 */
OBJ_DATA *get_obj_list( CHAR_DATA *ch, char *argument, OBJ_DATA *list )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = list; obj != NULL; obj = obj->next_in_carry_list )
    {
	if ( can_see_obj( ch, obj ) && is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}




/*
 * Find an obj in player's inventory.
 */
OBJ_DATA *get_obj_carry( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    if ( argument[0] == '\0' )
	return NULL;
    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->first_carry; obj != NULL; obj = obj->next_in_carry_list )
    {
	if ( obj->wear_loc == WEAR_NONE
	&&   is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in player's equipment.
 */
OBJ_DATA *get_obj_wear( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = ch->first_carry; obj != NULL; obj = obj->next_in_carry_list )
    {
	if ( obj->wear_loc != WEAR_NONE
	&&   is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}



/*
 * Find an obj in the room or in inventory.
 */
OBJ_DATA *get_obj_here( CHAR_DATA *ch, char *argument )
{
    OBJ_DATA *obj;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];

    obj = get_obj_room( ch, argument, map_obj[ch->x][ch->y] );
    if ( obj != NULL )
	return obj;

    if ( ( obj = get_obj_carry( ch, argument ) ) != NULL )
	return obj;

    if ( ( obj = get_obj_wear( ch, argument ) ) != NULL )
	return obj;

    return NULL;
}



/*
 * Find an obj in the world.
 */
OBJ_DATA *get_obj_world( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    OBJ_DATA *obj;
    int number;
    int count;

    if ( ( obj = get_obj_here( ch, argument ) ) != NULL )
	return obj;

    number = number_argument( argument, arg );
    count  = 0;
    for ( obj = first_obj; obj != NULL; obj = obj->next )
    {
	if ( is_name( arg, obj->name ) )
	{
	    if ( ++count == number )
		return obj;
	}
    }

    return NULL;
}

/*
 * Return # of objects which an object counts as.
 * Thanks to Tony Chamberlain for the correct recursive code here.
 */
int get_obj_number( OBJ_DATA *obj )
{
    int number;
 /*   OBJ_DATA *vobj;  */

    number = 1;  /*set to one since bag will count as 1 item*/
/*    if ( obj->item_type == ITEM_CONTAINER )
    {
       for ( vobj = obj->first_in_carry_list; vobj != NULL; vobj = vobj->next_in_carry_list )
       { 
          number = number - 1;
       }
    }

*/
/* containers should count as one item!  
    if ( obj->item_type == ITEM_CONTAINER )
      for ( obj = obj->contains; obj != NULL; obj = obj->next_content )
	number += get_obj_number( obj );
    else
	number = 1;
Zen */
    return number;
}



/*
 * Return weight of an object, including weight of contents.
 */
int get_obj_weight( OBJ_DATA *obj )
{
    int weight;

    weight = obj->weight;
    if ( obj->item_type == ITEM_MATERIAL && weight >= MAX_CHUNK_WEIGHT )
	weight = MAX_CHUNK_WEIGHT;
//    for ( obj = obj->first_in_carry_list; obj != NULL; obj = obj->next_in_carry_list )
//	weight += get_obj_weight( obj );

    return weight;
}

/*
 * True if char can see victim.
 */
bool can_see( CHAR_DATA *ch, CHAR_DATA *victim )
{
    if ( ch == victim )
	return TRUE;

    if ( IS_SET(ch->pcdata->pflags,PLR_ASS) )
	return FALSE; 
   if ( IS_SET(victim->act, PLR_WIZINVIS)
    &&   get_trust( ch ) < victim->invis )
	return FALSE;

    if ( IS_SET(victim->act, PLR_INCOG)
    &&   get_trust( ch ) < victim->incog 
    &&   NOT_IN_ROOM(ch,victim) )
        return FALSE;

    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

    if ( IS_SET(victim->pcdata->pflags,PLR_ASS) )
	return FALSE;

    if ( IS_SET(ch->effect,EFFECT_BLIND) )
	return FALSE;

    if ( victim->in_building && victim->in_building->type == BUILDING_CLUB && complete(victim->in_building) && NOT_IN_ROOM(ch,victim) && ( !ch->in_building || ch->in_building->type != BUILDING_PSYCHIC_EYES || !complete(ch->in_building) ) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can see obj.
 */
bool can_see_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( IS_SET(ch->act, PLR_HOLYLIGHT) )
	return TRUE;

    if ( IS_SET(ch->effect,EFFECT_BLIND) )
	return FALSE;

    return TRUE;
}



/*
 * True if char can drop obj.
 */
bool can_drop_obj( CHAR_DATA *ch, OBJ_DATA *obj )
{
    if ( !IS_SET(obj->extra_flags, ITEM_NODROP) )
	return TRUE;

    if ( !IS_NPC(ch) && ch->level >= LEVEL_IMMORTAL )
	return TRUE;

    return FALSE;
}


bool can_use( CHAR_DATA *ch, OBJ_DATA *obj )
{
   return( TRUE );
}


/*
 * Return names of classes which can use an object
 * -- Stephen
 */

char *who_can_use( OBJ_DATA *obj )
{
   return( " all classes." );
}


void info( char * message, int lv )
{
 /* This function sends <message>
  * to all players of level (lv) and above
  * Used mainly to send level gain, death info, etc to mortals.
  * - Stephen
  */
  DESCRIPTOR_DATA *d;
  char   buf[MAX_STRING_LENGTH];

  if ( lv == 1 )
	lv = 0;
  for ( d = first_desc; d; d = d->next )
    if ( ( d->connected == CON_PLAYING ) 
       && d->character->level >= lv
       && !IS_NPC(d->character) 
       && !IS_SET( d->character->deaf, CHANNEL_INFO ) )
    {
      sprintf( buf, "@@i@@B@@7[INFO]@@N@@b: %s%s%s\n\r", 
               color_string( d->character, "info" ), message,
               color_string( d->character, "normal" ) );
      send_to_char( buf, d->character );    
    }
  return;
}


void log_chan( const char *message, int lv )
{
   /* Used to send messages to Immortals.  
    * Level is used to determine WHO gets the message... */
   DESCRIPTOR_DATA *d;
   char buf[MAX_STRING_LENGTH];
   
   sprintf( buf, "[LOG]: %s\n\r", message );
   for ( d = first_desc; d; d = d->next )
      if ( ( d->connected == CON_PLAYING )
          && ( get_trust( d->character ) == MAX_LEVEL )
          && ( !IS_NPC( d->character ) )
          && ( d->character->level >= lv )
          && ( !IS_SET( d->character->deaf, CHANNEL_LOG ) ) )
        send_to_char( buf, d->character );
   return;
}

/*
 * Extended bitvector utility functions.
 */
bool xbv_is_empty( XBV *bits )
{
    register int i;

    for ( i = 0; i < XBI; i++ )
	if ( bits->bits[i] != 0 )	return FALSE;

    return TRUE;
}

bool xbv_same_bits( XBV *dest, const XBV *src )
{
    register int i;

    for ( i = 0; i < XBI; i++ )
	if ( dest->bits[i] != src->bits[i] )	return FALSE;

    return TRUE;
}

void xbv_clear_bits( XBV *bits )
{
    register int i;

    for ( i = 0; i < XBI; i++ )
	bits->bits[i] = 0;
}

void xbv_set_bits( XBV *dest, const XBV *src )
{
    register int i;

    for ( i = 0; i < XBI; i++ )
	SET_BIT( dest->bits[i], src->bits[i] );
}

void xbv_remove_bits( XBV *dest, const XBV *src )
{
    register int i;

    for ( i = 0; i < XBI; i++ )
	REMOVE_BIT( dest->bits[i], src->bits[i] );
}

XBV new_xbv (int bit, ...)
{
  static XBV	 bits;
  	 va_list param;
  	 int	 b;
  
  xCLEAR_BITS (bits);
  xSET_BIT (bits, bit);

  va_start (param, bit);

  while ((b=va_arg (param, int)) != -1)
    xSET_BIT( bits, b);
  va_end (param);

  return bits;
}


void extract_building( BUILDING_DATA *bld, bool msg )
{
    OBJ_DATA *obj;
    OBJ_DATA *obj_next;
    CHAR_DATA *ch;
    extern OBJ_DATA *map_obj[MAX_MAPS][MAX_MAPS];


    if ( bld->owner && bld->owner->first_building == bld )
	bld->owner->first_building = bld->next_owned;
    if ( bld->next_owned )
	bld->next_owned->prev_owned = bld->prev_owned;
    if ( bld->prev_owned )
	bld->prev_owned->next_owned = bld->next_owned;

    activate_building(bld,FALSE);
    if ( msg && !is_evil(bld) )
    {
    	for ( obj = map_obj[bld->x][bld->y];obj;obj = obj_next )
    	{
		obj_next = obj->next_in_room;
		if ( obj->z != bld->z )
			continue;
		obj->in_building = NULL;
		if ( obj->quest_timer > 0 )
			continue;
		if ( (obj->item_type == ITEM_BOMB && obj->value[1] != 0) || obj->item_type == ITEM_BLUEPRINT || obj->carried_by != NULL || obj->item_type == ITEM_TOKEN )
			continue;
		if ( obj->x == bld->x && obj->y == bld->y && obj->z == bld->z )
			extract_obj(obj);
    	}
    }
    else
    {
    	for ( obj = map_obj[bld->x][bld->y];obj;obj = obj_next )
    	{
		obj_next = obj->next_in_room;
		if ( obj->z != bld->z )
			continue;
		obj->in_building = NULL;
	}
    }
    for ( ch = first_char;ch;ch = ch->next )
    {
	if ( ch->x == bld->x && ch->y == bld->y && ch->z == bld->z && msg )
	{
		send_to_char( "The building you are in has collapsed!\n\r", ch );    
		ch->in_building = NULL;
	}
	if ( ch->bvictim == bld )
		ch->bvictim = NULL;
    }

    map_bld[bld->x][bld->y][bld->z] = NULL;
    bld->is_free = FALSE;
    building_count--;
    UNLINK(bld, first_building, last_building, next, prev);
    PUT_FREE(bld, building_free);
    return;
}

BUILDING_DATA *get_char_building( CHAR_DATA *ch )
{
	ch->in_building = map_bld[ch->x][ch->y][ch->z];
	return ch->in_building;
}

BUILDING_DATA *get_obj_building( OBJ_DATA *obj )
{
	if ( obj->carried_by != NULL )
		return obj->carried_by->in_building;

	obj->in_building = map_bld[obj->x][obj->y][obj->z];
	return obj->in_building;
}
CHAR_DATA *get_ch( char *argument )
{
	CHAR_DATA *rch;

	if ( !str_cmp(argument,"nobody") )
		return NULL;
	for ( rch = first_char;rch;rch = rch->next )
		if ( !str_cmp(rch->name, argument) )
			return rch;

	return NULL;
}
BUILDING_DATA *get_building( int x, int y, int z )
{
	if ( z < 0 || z >= Z_MAX )
		return NULL;
	if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
		return NULL;
	return map_bld[x][y][z];
}
BUILDING_DATA *get_building_range( int x, int y, int x2, int y2, int z )
{
	int xx,yy,d,d_next;
	bool xaxis = FALSE;

	d = -999;
	if ( x != x2 )
	{
		xaxis = TRUE;
		d_next = x;
	}
	else
		d_next = y;

	xx = x;
	yy = y;
	for ( ;; )
	{
		if ( ( xaxis && d == x2 ) || ( !xaxis && d == y2 ) )
			break;
                d = d_next;
 
                if ( xaxis )
                {
                        if ( d < x2 )
                                d_next = d + 1;
                        else
                                d_next = d - 1;
                        xx = d;
                }
                else
                {
                        if ( d < y2 )
                                d_next = d + 1;
                        else
                                d_next = d - 1;
                        yy = d;
                }
		if ( map_bld[xx][yy][z] )
			return map_bld[xx][yy][z];
	}
	return NULL;
}
CHAR_DATA *get_char_loc( int x, int y, int z )
{
	CHAR_DATA *ch;
	for ( ch = first_char;ch;ch = ch->next )
		if ( ch->x == x && ch->y == y && ch->z == z && ch->in_room->vnum == ROOM_VNUM_WMAP )
			return ch;
	return NULL;
}

void extract_vehicle( VEHICLE_DATA *vhc, bool msg )
{
    char buf[MSL];
    if ( vhc == NULL )
	return;
    if ( vhc->vehicle_in )
	extract_vehicle(vhc->vehicle_in,FALSE);

    if ( vhc->x > 0 && vhc->y > 0 )
    {
	if ( vhc->driving )
	{
		CHAR_DATA *ch = vhc->driving;
		act( "You eject!!", ch, NULL, NULL, TO_CHAR );
		act( "$n ejects!!", ch, NULL, NULL, TO_ROOM );
		if ( vhc->z == Z_AIR )
		{
			ch->c_sn = gsn_paradrop;
			ch->c_level = 20;
			ch->c_time = 0;
		}
		do_look(ch,"");
		vhc->driving->in_vehicle = NULL;
		vhc->driving = NULL;
        }
    	if ( msg )
    	{
		sprintf( buf, "%s has exploded!\n\r", vhc->desc );
		send_to_loc( buf, vhc->x, vhc->y, vhc->z );
	}
	    {
		VEHICLE_DATA *vhc2;
	
		if ( vhc == map_vhc[vhc->x][vhc->y][vhc->z] )
		{
			map_vhc[vhc->x][vhc->y][vhc->z] = vhc->next_in_room;
		}
		else
		{
			for ( vhc2 = map_vhc[vhc->x][vhc->y][vhc->z];vhc2;vhc2 = vhc2->next_in_room )
				if ( vhc2->next_in_room && vhc2->next_in_room == vhc )
					vhc2->next_in_room = vhc->next_in_room;
		}
	    }
	move_vehicle(vhc,0,0,vhc->z);
    }
    else
    {
    	UNLINK(vhc, first_vehicle, last_vehicle, next, prev);
    	PUT_FREE(vhc, vehicle_free);
    }
    return;
}

VEHICLE_DATA *get_vehicle_char( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    VEHICLE_DATA *vhc;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( vhc = map_vhc[ch->x][ch->y][ch->z]; vhc; vhc = vhc->next_in_room )
    {
	if ( vhc->driving == ch )
		continue;
	if ( is_name( arg, vhc->name ) && vhc->in_vehicle == NULL )
	{
	    if ( ++count == number )
		return vhc;
	}
    }

    return NULL;
}
VEHICLE_DATA *get_vehicle( char *argument, int x, int y, int z )
{
    char arg[MAX_INPUT_LENGTH];
    VEHICLE_DATA *vhc;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( vhc = map_vhc[x][y][z]; vhc; vhc = vhc->next_in_room )
    {
	if ( is_name( arg, vhc->name ) && vhc->in_vehicle == NULL )
	{
	    if ( ++count == number )
		return vhc;
	}
    }

    return NULL;
}
VEHICLE_DATA *get_vehicle_world( char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    VEHICLE_DATA *vhc;
    int number;
    int count;

    number = number_argument( argument, arg );
    count  = 0;
    for ( vhc = first_vehicle; vhc; vhc = vhc->next )
    {
	if ( is_name( arg, vhc->name ) && vhc->in_vehicle == NULL )
	{
	    if ( ++count == number )
		return vhc;
	}
    }

    return NULL;
}

void char_to_building( CHAR_DATA *ch, BUILDING_DATA *bld )
{
	OBJ_DATA *obj;
	VEHICLE_DATA *vhc;

	ch->in_building = bld;
	if ( ch->in_vehicle )
	{
		ch->in_vehicle->in_building = bld;;
		if ( TRANSPORT_VEHICLE(ch->in_vehicle->type) )
		{
			vhc = get_vehicle_from_vehicle(ch->in_vehicle);
			if ( vhc != NULL )
				vhc->in_building = ch->in_building;
		}
	}
	for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
		obj->in_building = ch->in_building;
	if ( bld && is_neutral(bld->type) && str_cmp(bld->owned,ch->name) )
	{
		BUILDING_DATA *bld2;
		bool hq = FALSE;
		for ( bld2 = ch->first_building;bld2;bld2 = bld2->next_owned )
		{
			if ( bld2->type == BUILDING_HQ && complete(bld) && IS_BETWEEN(bld2->x,bld->x-50,bld->x+50) && IS_BETWEEN(bld2->y,bld->y-50,bld->y+50) && bld2->z == bld->z )
			{
				bld->value[0] = bld2->x;
				bld->value[1] = bld2->y;
				free_string(bld->owned);
				bld->owned = str_dup(ch->name);
				bld->owner = NULL;
				send_to_char( "@@aYou have taken control of the building!@@N\n\r", ch );
				if ( bld->type == BUILDING_CELLULAR_COMPANY )
					bld->value[0] = 180;
				hq = TRUE;
				activate_building(bld,TRUE);
				break;
			}
		}
		if ( !hq )
			send_to_char( "You have no HQ nearby - Can't take over the neutral bulding.\n\r", ch );
	}
}

CHAR_DATA *get_rand_char( int x, int y, int z )
{
	CHAR_DATA *ch;
	CHAR_DATA *wch = NULL;

	if ( x < 0 || x >= MAX_MAPS || y < 0 || y >= MAX_MAPS )
		return NULL;

	ch = get_char_from_list(map_ch[x][y][z]);
	if ( ch == NULL )
		return NULL;
	if ( ch->x != x || ch->y != y )
		map_ch[x][y][z] = ch->next_in_room;
	for ( ;ch; ch = get_char_from_list(ch->next_in_room) )
	{
		if ( ch->dead || (ch->c_sn == gsn_paradrop && ch->c_level < 20 ) )
			continue;
		if ( IN_PIT(ch) )
			continue;
		if ( number_percent() < 50 || wch == NULL )
			wch = ch;
	}
	return wch;
}

int get_random_planet()
{
	return Z_GROUND;
}

void extract_queue(QUEUE_DATA *q)
{
//	free_string(q->command);
	if ( !q->is_free )
		PUT_FREE(q,queue_free);
	return;
}

void check_prof(CHAR_DATA *ch)
{
	int time,ttl,x=0,i;
	char buf[MSL];
	if ( !ch )
		return;
	time = my_get_hours(ch,TRUE);
	ttl = ch->pcdata->prof_ttl;
	for ( i=0;prof_times[i] > -1;i++ )
	{
		if ( prof_times[i] >= time || prof_times[i+1] == -1 )
		{
			x = i - ttl;
			break;
		}
	}
	if ( x == 0 )
		return;
	ch->pcdata->prof_ttl = ch->pcdata->prof_ttl + x;
	ch->pcdata->prof_points = ch->pcdata->prof_points + x;
	sprintf( buf, "\n\r@@WYou have earned @@a%d@@W Proficiency Points in your last login session!\n\rType '@@eprof@@W' to use them.@@N\n\r", x );
	send_to_char(buf,ch);
	return;
}
void activate_building(BUILDING_DATA *bld, bool on)
{
	if ( !bld )
		return;
	if ( bld->active == on )
		return;
	bld->active = on;
	if ( on )
		LINK(bld,first_active_building,last_active_building,next_active,prev_active);
	else
	{
		bld->owner = NULL;
		UNLINK(bld,first_active_building,last_active_building,next_active,prev_active);
	}
	return;
}


void add_char_to_list(CHAR_DATA *ch)
{
	int i;
	extern CHAR_DATA *ch_list[MAX_POSSIBLE_PLAYERS];

	for (i=0;i<MAX_POSSIBLE_PLAYERS;i++ )
	{
		if ( !ch_list[i] )
		{
			ch->list = i;
			ch_list[i] = ch;
			return;
		}
	}
	log_f("No room to add character!");
	return;
}
void del_char_from_list(CHAR_DATA *ch)
{
	extern CHAR_DATA *ch_list[MAX_POSSIBLE_PLAYERS];

	ch_list[ch->list] = NULL;
	ch->list = -1;

	return;
}
CHAR_DATA * get_char_from_list(unsigned sh_int i)
{
	extern CHAR_DATA *ch_list[MAX_POSSIBLE_PLAYERS];

	if ( i <= -1 )
		return NULL;
	return ch_list[i];
}
