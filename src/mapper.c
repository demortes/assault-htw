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


#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "mapper.h"

extern char * compass_name[];
extern void do_space_look(CHAR_DATA *ch);

int door_marks[4][2] ={ {-1, 0},{ 0, 1},{ 1, 0},{ 0,-1} };
int offsets[4][2] ={ {-2, 0},{ 0, 2},{ 2, 0},{ 0,-2} };

#define SECT_HERE	SECT_MAX
#define SECT_UNSEEN	( SECT_MAX + 1 )
#define SECT_BLOCKED	( SECT_UNSEEN + 1 )
#define SECT_TOP	( SECT_BLOCKED + 1 )


void do_mapper( CHAR_DATA *ch, char *argument )
{
  int size = 0;
  if ( ch->z == Z_SPACE )
  {
	  ShowSpace(ch);
    return;
  }

  if ( NUKEM(ch) )
	return;
  if ( argument[0] != '\0' )
  	size = atoi(argument);
  if ( IS_SET( ch->config, CONFIG_BLIND ) )
  {
	ShowBMap( ch, FALSE );
	return;
  }
  if ( IS_SET( ch->config, CONFIG_CLIENT) )
  {
	ShowCMap( ch );
	return;
  }
  ShowWMap( ch, IS_SET( ch->config, CONFIG_SMALLMAP)?2:IS_SET(ch->config,CONFIG_TINYMAP)?1:4, size );
  return;

}
void ShowCMap( CHAR_DATA *ch )
{
	int x,y,z=ch->z,sect,xx,yy,y1,y2,x1,x2;
	char buf[MSL];
	CHAR_DATA *wch;
	BUILDING_DATA *bld;
	y1 = ch->y - 8;
	x1 = ch->x - 8;
	y2 = ch->y + 8;
	x2 = ch->x + 8;
	send_to_char( "\2", ch );
	for ( x=x1;x<x2;x++ )
	{
		for ( y=y1;y<y2;y++ )
		{
			xx = x; yy = y;
			real_coords(&xx,&yy);
			sect = map_table.type[xx][yy][z];
			if ((bld=map_bld[xx][yy][ch->z]) != NULL)
				sprintf(buf,"%d",(bld->type == BUILDING_DUMMY)?bld->value[0]+100:100+bld->type);
			else
				sprintf	( buf, "0%s%d", (sect < 10)?"0":"",sect );
			if ( (xx != ch->x || yy != ch->y) )
			{
				for ( wch = map_ch[xx][yy][ch->z];wch;wch = wch->next_in_room )
				{
					if ( !can_see(ch,wch) ) continue;
					sprintf(buf+strlen(buf), "P0%d",allied(ch,wch)?1:IS_IMMORTAL(wch)?2:0);
					break;
				}
				if ( map_vhc[xx][yy][ch->z] )
				{
					sprintf(buf+strlen(buf), "V%2d",map_vhc[xx][yy][ch->z]->type);
				}
			}
			send_to_char(buf,ch);
		}
	}
	send_to_char("\1", ch );
	return;
}

void ShowBMap( CHAR_DATA *ch, bool quest )
{
	DESCRIPTOR_DATA *d;
	BUILDING_DATA *bld;
	char b_north[MSL];
	char b_east[MSL];
	char b_west[MSL];
	char b_south[MSL];
	char p_buf[MSL];
	char b_buf[MSL];
	char e_buf[MSL];
	char w_buf[MSL];
	char g_buf[MSL];
	int x,y,last,maxx,xx,yy;
	int offline,allied,enemy,yours,total;

	offline=0;
	allied=0;
	enemy=0;
	yours=0;
	total=0;

	p_buf[0] = '\0';
	b_buf[0] = '\0';
	b_north[0] = '\0';
	b_south[0] = '\0';
	b_east[0] = '\0';
	b_west[0] = '\0';

	maxx = ch->map / 2;

	if ( IS_SET(ch->config,CONFIG_LARGEMAP) )
		maxx = ch->map;

		char_to_building(ch,NULL);

                for ( d = first_desc; d != NULL; d = d->next )
                {
                        if ( d->character == NULL )
                                continue;
			if ( d->connected != CON_PLAYING )
				continue;
            if ( !can_see(ch,d->character) )
                continue;
			if ( paintball(ch) && map_bld[d->character->x][d->character->y][1] != NULL && map_bld[d->character->x][d->character->y][1]->type == BUILDING_HQ )
				continue;
			if ( sneak(d->character) )
					continue;
			if ( ch->z != d->character->z )
				continue;
                        if ( d->character != ch
                        &&  d->character->x > ch->x - maxx
                        &&  d->character->x < ch->x + maxx
                        &&  d->character->y > ch->y - maxx
                        &&  d->character->y < ch->y + maxx )
                                sprintf( p_buf+strlen(p_buf), "%s%s: %s at: %d/%d\n\r", (ch->y < d->character->y) ? "North" : (ch->y == d->character->y ) ? (ch->x == d->character->x)?"Here":"" : "South", (ch->x > d->character->x) ? "West" : (ch->x == d->character->x) ? "" : "East" , d->character->name, d->character->x, d->character->y );
                }
//		if ( ch->z != Z_AIR )
		{

  			for (xx = ch->x - maxx; xx <= ch->x + maxx; ++xx)
  			{
    				for (yy = ch->y - ch->map; yy <= ch->y + ch->map; ++yy)
				{
					x = xx;y = yy; real_coords(&x,&y);
					bld = map_bld[x][y][ch->z];

					if ( !bld || bld == NULL )
						continue;

					if ( bld->visible || bld->owner == ch )
					{
						if ( bld->x == ch->x && bld->y == ch->y && !quest )
						{
							char_to_building(ch,bld);
							continue;
						}
						if ( bld->x + 1 == ch->x && bld->y == ch->y )
							sprintf( b_west, "%s", bld->name );
						if ( bld->x - 1 == ch->x && bld->y == ch->y )
							sprintf( b_east, "%s", bld->name );
						if ( bld->x == ch->x && bld->y + 1 == ch->y )
							sprintf( b_south, "%s", bld->name );
						if ( bld->x == ch->x && bld->y - 1 == ch->y )
							sprintf( b_north, "%s", bld->name );
//						sprintf( b_buf+strlen(b_buf), "Building: %s, owned by %s, at %d/%d (%s%s)\n\r", bld->name, bld->owned, bld->x, bld->y, (ch->y < bld->y) ? "North" : (ch->y == bld->y) ? "" : "South", (ch->x > bld->x) ? "West" : (ch->x == bld->x) ? "" : "East" );


						if ( !bld->owner )
							offline++;
						else if ( bld->owner == ch )
							yours++;
						else if ( bld->owner->pcdata->alliance != -1 && bld->owner->pcdata->alliance == ch->pcdata->alliance )
							allied++;
						else
							enemy++;
						total++;
					}
				}
			}
		}
		if ( total > 0 )
		{
			sprintf( b_buf, "Buildings:\n\r" );
			if ( yours > 0 )
				sprintf( b_buf+strlen(b_buf), "%d Yours\n\r", yours );
			if ( allied > 0 )
				sprintf( b_buf+strlen(b_buf), "%d Allied\n\r", allied );
			if ( enemy > 0 )
				sprintf( b_buf+strlen(b_buf), "%d Enemy\n\r", enemy );
			if ( offline > 0 )
				sprintf( b_buf+strlen(b_buf), "%d Offline\n\r", offline );
			sprintf( b_buf+strlen(b_buf), "%d Total\n\r", total );

		}
		sprintf( e_buf, "North: %s\n\rEast: %s\n\rSouth: %s\n\rWest: %s\n\r",
		(b_north[0] != '\0') ? b_north : wildmap_table[leads_to(ch->x,ch->y,ch->z,DIR_NORTH)].name,
		(b_east[0] != '\0')  ? b_east  : wildmap_table[leads_to(ch->x,ch->y,ch->z,DIR_EAST)].name,
		(b_south[0] != '\0') ? b_south : wildmap_table[leads_to(ch->x,ch->y,ch->z,DIR_SOUTH)].name,
		(b_west[0] != '\0')  ? b_west  : wildmap_table[leads_to(ch->x,ch->y,ch->z,DIR_WEST)].name );

		x = ch->x;
		y = ch->y;
		last = map_table.type[x][y][ch->z];
		for ( y = ch->y;y < MAX_MAPS;y++ )
		{
			if ( map_table.type[x][y][ch->z] != last )
			{
				sprintf( w_buf, "Far north (%d): %s\n\r", y-ch->y, wildmap_table[map_table.type[x][y][ch->z]].name );
				break;
			}
		}
		y = ch->y;
		for ( x = ch->x;x < MAX_MAPS;x++ )
		{
			if ( map_table.type[x][y][ch->z] != last )
			{
				sprintf( w_buf+strlen(w_buf), "Far East (%d): %s\n\r", x - ch->x, wildmap_table[map_table.type[x][y][ch->z]].name );
				break;
			}
		}
		x = ch->x;
		for ( y = ch->y;y>0;y-- )
		{
			if ( map_table.type[x][y][ch->z] != last )
			{
				sprintf( w_buf+strlen(w_buf), "Far South (%d): %s\n\r", ch->y-y, wildmap_table[map_table.type[x][y][ch->z]].name );
				break;
			}
		}
		y = ch->y;
		for ( x = ch->x;x > 0;x-- )
		{
			if ( map_table.type[x][y][ch->z] != last )
			{
				sprintf( w_buf+strlen(w_buf), "Far West (%d): %s\n\r", ch->x-x, wildmap_table[map_table.type[x][y][ch->z]].name );
				break;
			}
		}


	if ( ch->in_building && !quest )
	{
		show_building(ch,IS_SET( ch->config, CONFIG_SMALLMAP)?2:IS_SET(ch->config,CONFIG_TINYMAP)?1:4,ch->map);
		return;
	}

	sprintf( g_buf, "Your location: %d/%d (%s)\n\r\n\r", ch->x, ch->y, wildmap_table[map_table.type[ch->x][ch->y][ch->z]].name );

/*	if ( quest )
	{
		for ( x = ch->x - ch->map/2;x < ch->x + ch->map/2;x++ )
			for ( y = ch->y - ch->map/2;y < ch->y + ch->map/2;y++ )
				terrain[map_table.type[x][y][1]]++;

		sprintf( b_buf, "\n\rBasic map description:\n\r\n\r" );
		for ( x = 0;x < SECT_MAX;x++ )
			if ( terrain[x] > 0 )
				sprintf( b_buf+strlen(b_buf), "%d %s sectors.\n\r", terrain[x], wildmap_table[x].name );
	}*/
	if ( ch->z == Z_AIR )
	{
		sprintf( g_buf, "%d/%d\n\r", ch->x,ch->y);
		send_to_char( g_buf, ch );
	}
//	else if ( !quest )
	{
		send_to_char( g_buf, ch );
		send_to_char( w_buf, ch );
		send_to_char( e_buf, ch );
		send_to_char( b_buf, ch );
	}
//	if ( !quest )
		send_to_char( p_buf, ch );
	return;
}
void show_building( CHAR_DATA *ch, sh_int small, int size )
{
	BUILDING_DATA *bld = ch->in_building;
	char borderbuf[MSL];
	char outbuf[MSL];
	char exbuf[MSL];
	int i,j;
	bool warcannon = FALSE;
	bool msg = FALSE;

	if ( size == 999 )
		warcannon = TRUE;

	if ( IS_SET(ch->config,CONFIG_BLIND) )
	{
		if ( IS_SET(ch->pcdata->pflags,PFLAG_HELPING) )
			sprintf( borderbuf, "\n\rUnknown, %s", planet_table[ch->z].name );
		else
			sprintf( borderbuf, "\n\r%d,%d, %s", ch->x, ch->y, planet_table[ch->z].name );
		sprintf( borderbuf+strlen(borderbuf), "\n\r%s%s          Level %d\n\r\n\r", wildmap_table[map_table.type[ch->x][ch->y][ch->z]].color, bld->name, bld->level);
	}
	else
	{
		if ( IS_SET(ch->pcdata->pflags,PFLAG_HELPING) )
			sprintf( borderbuf, "\n\r        @@l(@@W??@@g,@@W??@@l) @@R[@@e%s@@R]@@N", planet_table[ch->z].name );
		else
			sprintf( borderbuf, "\n\r        @@l(@@W%d@@g,@@W%d@@l) @@R[@@e%s@@R]@@N", ch->x, ch->y, planet_table[ch->z].name );
		sprintf( borderbuf+strlen(borderbuf), "\n\r%s%s          Level %d\n\r\n\r", wildmap_table[map_table.type[ch->x][ch->y][ch->z]].color, bld->name, bld->level);
	}

	sprintf(exbuf,"%sExits:@@d ",IS_SET(ch->config,CONFIG_BLIND)?"":"@@r[@@G ");
	if ( ch->desc->mxp )
		strcat( exbuf, "\e[1z" );
	for ( i=0;i<4;i++ )
	{
		if ( bld->exit[i] )
		{
			char tempbuf[MSL];
			strcat (exbuf, MXPTAG(ch->desc,"Ex"));
			sprintf( tempbuf, "%s", ( i == 0 ) ? "North" : ( i == 1 ) ? "East" : ( i == 2 ) ? "South" : "West" );
			strcat (exbuf, tempbuf );
			strcat (exbuf, MXPTAG(ch->desc,"/Ex"));
			strcat (exbuf," ");
		}
	}
	if ( !IS_SET(ch->config,CONFIG_BLIND) )
		strcat (exbuf, "@@r]@@N");
	sprintf( borderbuf+strlen(borderbuf), "%s\n\r",exbuf );
	if (  open_bld(bld) && warcannon == FALSE )
	{
		int mapsize;
		mapsize = ch->map;
		ch->map = 10;
		send_to_char( borderbuf, ch );
		if ( !IS_SET(ch->config,CONFIG_BLIND) )
			ShowWMap(ch,small,999);
		ch->map = mapsize;
	}
	else
	{
		if ( !IS_SET(ch->config,CONFIG_BRIEF) )
			sprintf( outbuf, "%s\n\r\n\r", build_table[bld->type].desc );
		else
			outbuf[0] = '\0';
		send_to_char(borderbuf,ch);
		send_to_char(outbuf,ch);
	}
	sprintf(borderbuf, "\n\r ");
	if ( !IS_SET(ch->config,CONFIG_BLIND) && bld->maxhp > 0 )
	{
		j = ((10000 / bld->maxhp) * bld->hp) / 1000;
		for ( i=0;i<j;i++ )
			sprintf( borderbuf+strlen(borderbuf), "%s>", ( i < 3 ) ? "@@e" : ( i < 6 ) ? "@@y" : "@@r" );
	}
	sprintf( borderbuf+strlen(borderbuf), " %d HP\n\r", bld->hp );
	send_to_char( borderbuf, ch );
	sprintf(borderbuf, " ");
	if ( !IS_SET(ch->config,CONFIG_BLIND) )
	{
		if ( bld->shield > 0 )
			j = ((10000 / bld->maxshield) * bld->shield) / 1000;
		else
			j = 0;
		for ( i=0;i<j;i++ )
			sprintf( borderbuf+strlen(borderbuf), "%s>", ( i < 3 ) ? "@@e" : ( i < 6 ) ? "@@y" : "@@r" );
	}
	sprintf( borderbuf+strlen(borderbuf), " %d SHIELD@@N\n\r", bld->shield );
	send_to_char(borderbuf,ch);
	borderbuf[0] = '\0';
	outbuf[0] = '\0';
    for ( i=0; i<8; i++ )
    {
        if ( bld->resources[i] > 0 )
        {
            if ( !msg )
            {
                send_to_char( "Needed for completion:\n\r", ch );
                msg = TRUE;
            }
            sprintf( outbuf+strlen(outbuf), "%d %s\n\r", bld->resources[i], ( i == 0 ) ? "Iron" : ( i == 1 ) ? "Skins" : ( i == 2 ) ? "Copper" : ( i == 3 ) ? "Gold" : ( i == 4 ) ? "Silver" : ( i == 5 ) ? "Rocks" : ( i == 6 ) ? "Sticks" : "Logs" );
            //			send_to_char( outbuf, ch );
        }
    }
	if ( bld->type == BUILDING_SCUD_LAUNCHER || bld->type == BUILDING_NUKE_LAUNCHER )
		sprintf( borderbuf+strlen(borderbuf), "Missile Ready In: %d minutes.\n\r", bld->value[0] / 6);
	else if ( bld->type == BUILDING_BAR && bld->value[0] > 0 )
		sprintf(borderbuf+strlen(borderbuf), "Bar opens in: %d minutes.\n\r", bld->value[0] / 6 );
	else if ( bld->type == BUILDING_BANK )
		sprintf(borderbuf+strlen(borderbuf), "Next Interest gain in: %d minutes.\n\r", bld->value[0] / 6 );
	else if ( bld->type == BUILDING_HACKPORT )
		sprintf( borderbuf+strlen(borderbuf), "Backdoor Ready In: %d minutes (%d ticks).\n\r", bld->value[0] / 6, bld->value[0] );
	else if ( bld->type == BUILDING_SPY_SATELLITE || bld->type == BUILDING_PSYCHIC_TORMENTOR || bld->type == BUILDING_SHOCKWAVE )
		sprintf( borderbuf+strlen(borderbuf), "Transmission Ready In: %d minutes.\n\r", bld->value[0] / 6 );
	else if ( bld->type == BUILDING_SPY_QUARTERS )
		sprintf( borderbuf+strlen(borderbuf), "Spy Mission Ready In: %d minutes.\n\r", bld->value[0] / 6 );
	send_to_char( borderbuf, ch );
	send_to_char( outbuf,ch);

	if ( IS_SET(ch->config, CONFIG_EXITS )  )
	{
		send_to_char( exbuf, ch );
	}
	sprintf( outbuf, "\n\rOwned by %s.\n\r", bld->owned );
	send_to_char( outbuf, ch );
	return;
}

void do_buildings( CHAR_DATA *ch, char *argument )
{
	BUILDING_DATA *bld;
	int x,y,maxx,xx,yy,i=0;
	char buf[MSL];
	bool all=FALSE;

	if ( !IS_SET(ch->config,CONFIG_BLIND) )
	{
		send_to_char("Huh?\n\r", ch );
		return;
	}
	maxx = ch->map / 2;

	if ( argument[0] == '\0' )
		all = TRUE;
	buf[0] = '\0';

 	for (xx = ch->x - maxx; xx <= ch->x + maxx;xx++)
 	{
		for (yy = ch->y - ch->map; yy <= ch->y + ch->map; yy++)
		{
			x = xx;y = yy; real_coords(&x,&y);
			bld = map_bld[x][y][ch->z];
			if ( !bld || bld == NULL )
				continue;

			if ( !all && (str_prefix(argument,bld->owned) && str_cmp(argument,bld->name) ) )
				continue;
			if ( bld->visible == FALSE && (!bld->owner || (!allied(ch,bld->owner) && bld->owner != ch) ) )
				continue;

			sprintf(buf+strlen(buf), "%s: ", bld->owned );
			sprintf( buf+strlen(buf), "%s, at %d/%d (%s%s)\n\r", (bld->type == BUILDING_DUMMY && bld->value[0] > 0 && bld->value[0] < MAX_BUILDING)? build_table[bld->value[0]].name:bld->name, bld->x, bld->y, (ch->y < bld->y) ? "North" : (ch->y == bld->y) ? "" : "South", (ch->x > bld->x) ? "West" : (ch->x == bld->x) ? "" : "East" );
			if ( ++i > 100 )
			{
				sprintf(buf+strlen(buf),"There are over 100 buildings here, can't display them all.\n\r" );
				break;
			}
		}
		if ( i > 100 ) break;
	}
	send_to_char(buf,ch);
	return;
}
void do_scanmap( CHAR_DATA *ch, char *argument )
{
	int dir,sect,lsect,count=0;
	int x,y;
	char buf[MSL];

	if ( !IS_SET(ch->config,CONFIG_BLIND) )
	{
		send_to_char( "Huh?\n\r", ch );
		return;
	}
	if ( ( dir = parse_direction(ch,argument) ) == -1 || argument[0] == '\0' )
	{
		send_to_char( "Valid directions are North, East, South and West\n\r", ch );
		return;
	}

	x = ch->x;
	y = ch->y;
	lsect = -1;
	sect = -1;
	buf[0] = '\0';
	while ( TRUE )
	{
		lsect = sect;
		if ( dir == DIR_NORTH )
		{
			y++;
			if ( y > ch->y + ch->map )
				break;
		}
		else if ( dir == DIR_SOUTH )
		{
			y--;
			if ( y < ch->y - ch->map )
				break;
		}
		else if ( dir == DIR_EAST )
		{
			x++;
			if ( x > ch->x + ch->map )
				break;
		}
		else if ( dir == DIR_WEST )
		{
			x--;
			if ( x < ch->x - ch->map )
				break;
		}
		if ( INVALID_COORDS(x,y) )
			break;

		sect = map_table.type[x][y][ch->z];
		if ( lsect != sect && lsect != -1 )
		{
			sprintf( buf+strlen(buf), "%d %s terrain, ", count, wildmap_table[lsect].name );
			count = 0;
		}
		count++;
	}
	sprintf( buf+strlen(buf), "%d %s terrain.\n\r", count, wildmap_table[lsect].name );
	send_to_char(buf,ch);
	return;
}

void ShowWMap( CHAR_DATA *ch, sh_int small, int size )
{
  	int x,y,z=ch->z,i=0,xx,yy,looper;
  	int maxx = ( IS_SET(ch->config,CONFIG_LARGEMAP) && size != 995 && size != 994 && size != 998 && size != 997 )?ch->map:ch->map/2;
  	bool warcannon = (size==999);
  	bool base = (size==996 || size ==995 || size==994);
	sh_int security = (size==995)?1:(size==994)?2:0;
  	bool inverse = IS_SET(ch->config,CONFIG_INVERSE)?TRUE:FALSE;
  	bool pit = IN_PIT(ch);
  	bool inMedal = medal(ch);
	char linebuf[MSL];


  	char color[MSL];
  	char outbuf[MSL];
  	char catbuf[MSL];
  	char borderbuf[MSL];
  	bool charr = FALSE;
  	bool def = FALSE;
  	BUILDING_DATA *bld = first_building;
  	bool enemy = FALSE;
	bool ft=FALSE;

  	bool terrain[SECT_MAX]={FALSE};

	small = 2;
  	outbuf[0] = '\0';
  	color[0] = '\0';
  	borderbuf[0] = '\0';

	if ( map_bld[ch->x][ch->y][ch->z] && warcannon == FALSE && size != 997 && !base && size != 998 )
	{
		char_to_building(ch,map_bld[ch->x][ch->y][ch->z]);
		show_building(ch,small,size);
		return;
	}

	if ( !pit && !inMedal && size < 998 ) /* For quest calls */
	{
		char sbuf[MSL];
		int c = (ch->map * 2)-16;
		sbuf[0] = '\0';
		sprintf(borderbuf,"\n\r");
		if ( !IS_SET(ch->pcdata->pflags,PFLAG_HELPING))
			sprintf( sbuf, "  @@cLocation: @@a%3d@@g/@@a%-*d @@g| @@cTerrain: %s%s", ch->x, c,ch->y,wildmap_table[map_table.type[ch->x][ch->y][ch->z]].color, wildmap_table[map_table.type[ch->x][ch->y][ch->z]].name );
		else
			sprintf( sbuf, "@@l(@@g??@@g,@@g??@@l) @@R[@@e%s@@R]@@N",  planet_table[ch->z].name );
		strcat(borderbuf,sbuf);
		strcat(borderbuf,"\n\r");
	}

  	sprintf( linebuf, "%s", "@@W+@@o" );
  	sprintf( catbuf, "--" );

	if (pit)
	  	for ( looper = 0; looper < MAX_MAPS-PIT_BORDER_Y-1; looper++ )
    			safe_strcat( MSL, linebuf, catbuf );
	else if (inMedal)
		for ( looper = 0; looper < MEDAL_BORDER_Y; looper++ )
		    	safe_strcat( MSL, linebuf, catbuf );
	else
	  	for ( looper = 0; looper < ch->map*2; looper++ )
    			safe_strcat( MSL, linebuf, catbuf );

  	safe_strcat( MSL, linebuf, "@@W+@@N" );
  	send_to_char( "\n\r", ch );
	if ( !warcannon )
	send_to_char( linebuf,ch );
  	send_to_char( borderbuf, ch );
	send_to_char( linebuf, ch );
  	sprintf( outbuf, "%s", "\n\r" );
	i=0;
	catbuf[0]='\0';

  	for (yy = ch->y + maxx; yy >= ch->y - maxx; --yy)
  	{ /* every row */
		if ( pit && (yy < PIT_BORDER_Y || yy >= MAX_MAPS))
			continue;
//    		safe_strcat( MSL, outbuf, "" );
		strcpy(color,"");
//    		sprintf( color, "" );
    		for (xx = ch->x - ch->map; xx <= ch->x + ch->map; ++xx)
    		{ /* every column */

			x = xx; y = yy;
			real_coords(&x,&y);

			bld = map_bld[x][y][z];
			if ( ch->x == x && ch->y == y && security == 0 )
			{
				sprintf( catbuf, "@@y**");
				sprintf( color, "@@y" );
				if (inverse)
				{
					strcat(catbuf,"@@i");
					strcat(color,"@@i");
				}
			}
			else if ((yy < 1 || yy > MEDAL_BORDER_Y || xx < 1 || xx > MEDAL_BORDER_X) && ch->z == Z_PAINTBALL)
			{
				sprintf(catbuf, "@@k++@@a");
			}
			else if ( z == Z_AIR && ((map_bld[x][y][Z_GROUND] && map_bld[x][y][Z_GROUND]->type == BUILDING_AIRFIELD && map_bld[x][y][Z_GROUND]->active)))
			{
				sprintf(catbuf,"@@W##@@a");
			}
                        else if ( z == Z_AIR && map_table.type[x][y][Z_GROUND] == SECT_MOUNTAIN)
			{
				sprintf(catbuf, "@@b~/@@a");
			}
			else if ( z == Z_SPACE && map_bld[x][y][Z_GROUND] && map_bld[x][y][Z_GROUND]->type == BUILDING_SPACE_CENTER)
			{
				sprintf(catbuf, "@@dSP");
			}
			else if ( z == Z_UNDERGROUND &&  map_bld[x][y][Z_GROUND] && map_bld[x][y][Z_GROUND]->type == BUILDING_TUNNEL)
			{
				sprintf(catbuf, "@@bTN");
			}
			else if ( pit && (xx<PIT_BORDER_X || xx >= MAX_MAPS ) )
			{
				continue;
			}
			else if ( bld && (bld->visible || (bld->owner && allied(ch,bld->owner)) || ch == bld->owner || ch->trust >= 85 ) )
			{
				CHAR_DATA *vch;
				char ocolor[MSL];

				if ( bld->active )
				{
					if ( (vch = bld->owner) == NULL )
					{
						if ( str_cmp(bld->owned,ch->name) && !is_evil(bld) )
							vch = get_ch(bld->owned);
						else
							vch = ch;
					}
					if ( vch == NULL )
						activate_building(bld,FALSE);
				}
				else
					vch = NULL;

 				if (blind_spot(ch,xx,yy) )
				{
					sprintf(catbuf, "  ");
				}
				else
				{
					char symbol[MSL];
					bool mxp = TRUE;
					def = ((!ch->security && bld->owner == ch && defense_building(bld)));
					charr = map_ch[x][y][z] ? TRUE : FALSE;
					if ( security == 0 )
					{
						sprintf( ocolor, "%s",  ( vch == NULL ) ? "@@d":charr ? "@@J" : def?"@@W" : ( bld->value[9] > 0 ) ? "@@b" : ( IS_LINKDEAD(vch) ) ? "@@o" : ( IS_NEWBIE(vch) ) ? "@@c" : (bld->value[3] != 0) ? "@@m" : ( bld->visible == FALSE ) ? "@@W" : ( bld->hp < bld->maxhp && bld->hp > bld->maxhp / 2 ) ? "@@b" : ( bld->hp < bld->maxhp / 2 ) ? "@@Q" : "@@G" );
						if ( ch == bld->owner || bld->protection > 0 || ( bld->type == BUILDING_DUMMY && ((IS_BETWEEN(bld->x,ch->x-1,ch->x+1) && IS_BETWEEN(bld->y,ch->y-1,ch->y+1)) || bld->value[5] > 0 ) ) )
							sprintf( ocolor+strlen(ocolor), "@@x");
						if ( charr && (map_ch[x][y][z]->pcdata->alliance != ch->pcdata->alliance || map_ch[x][y][z]->pcdata->alliance == -1) )
							enemy = TRUE;
					}
					else if ( security == 2 )
						sprintf(ocolor,"%s",(build_table[bld->type].act != BUILDING_DEFENSE)?"@@d":bld->value[6]<=3?"@@g":bld->value[6]<=6?"@@b":bld->value[6] <=9?"@@R":"@@e");

					if ( inverse )
						strcat(ocolor,"@@i");

					catbuf[0] = '\0';
					if ( str_cmp(color,ocolor) )
					{
						sprintf( color, "%s", ocolor );
						sprintf( catbuf, "%s", color );
					}

					if ( security == 2 && build_table[bld->type].act == BUILDING_DEFENSE && bld->owner && bld->owner == ch )
					{
						sprintf( symbol,"%*d",small,bld->value[6]);
					}
					else
					{
						if ( bld->type != BUILDING_DUMMY )
							sprintf( symbol, "%s", build_table[bld->type].symbol);
						else
							sprintf( symbol, "%s", build_table[bld->value[0]].symbol);
					}
					if ( mxp )
					{
						if ( ch->desc->mxp )
							strcat( catbuf, "\e[1z" );
						if ( bld->type == BUILDING_DUMMY )
						{
							if ( bld->value[0] < 1 || bld->value[0] >= MAX_BUILDING )
								bld->value[0] = BUILDING_TURRET;
							sprintf( ocolor, MXPTAG(ch->desc,"Bl x=%d y=%d owner=%s name='%s'"), bld->x, bld->y, bld->owned, build_table[bld->value[0]].name );
						}
						else
							sprintf( ocolor, MXPTAG(ch->desc,"Bl x=%d y=%d owner=%s name='%s'"), bld->x, bld->y, bld->owned, bld->name );
					}
					strcat( catbuf, ocolor);
					sprintf( ocolor, "%s", symbol );
					strcat( catbuf, ocolor );
					if ( mxp )
						strcat( catbuf, MXPTAG(ch->desc,"/Bl"));
				}
			}
			else if ( ( map_ch[x][y][z] != NULL || IS_SET(ch->effect,EFFECT_VISION) ) && size != 998 && size != 997 )
			{
				CHAR_DATA *wch;
				bool vehicle = FALSE;
				bool allied = FALSE;
				bool newbie = FALSE;
				bool imm = FALSE;
				int ppl = 0;
				char ppl_c[MSL];

 				if (blind_spot(ch,xx,yy) )
				{
					sprintf(catbuf,"  ");
				}
				else
				{
					for ( wch = map_ch[x][y][z];wch;wch = wch->next_in_room )
					{
						if ( !can_see(ch,wch) )
							continue;
						ppl++;
						if ( wch->in_vehicle )
							vehicle = TRUE;
						if ( ch->pcdata->alliance != -1 && ch->pcdata->alliance == wch->pcdata->alliance )
							allied = TRUE;
						else if ( IS_NEWBIE(wch) )
							newbie = TRUE;
						else if ( IS_IMMORTAL(wch) )
							imm = TRUE;
						else if ( enemy == FALSE && wch != ch)
						{
							enemy = TRUE;
						}
					}
					if ( ppl > 9 )
						ppl = 9;
					if ( IS_SET(ch->effect,EFFECT_VISION) && ppl == 0 && number_percent() < 5 )
						ppl++;

					sprintf( ppl_c, "%d", ppl );
					if ( ppl <= 0 )
					{
						sprintf( color, "%s", ( !str_cmp(wildmap_table[map_table.type[x][y][ch->z]].color, color) ) ? "" : wildmap_table[map_table.type[x][y][ch->z]].color );
						if (inverse) strcat(color,"@@i");
						sprintf(catbuf,"%s%s",color,wildmap_table[map_table.type[xx][yy][ch->z]].mark);
					}
					else
					{
						if ( vehicle )
						{
							if ( small == 1 )
								sprintf( catbuf, "%s%s@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e", ppl_c );
							if ( small == 2 )
								sprintf( catbuf, "%s[%s@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e",(ppl==1) ? "]" : ppl_c );
							if ( small == 4 )
								sprintf( catbuf, "%s<@@y[%s@@e>@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e",(ppl==1) ? "]" : ppl_c );
						}
						else
						{
							if ( small == 1 )
								sprintf( catbuf, "%s*@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e" );
							if ( small == 2 )
								sprintf( catbuf, "%s*%s@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e",(ppl==1) ? "*" : ppl_c );
							if ( small == 4 )
								sprintf( catbuf, "%s<@@y*%s%s>@@N", imm?"@@y":newbie?"@@a":allied?"@@r":"@@e",(ppl==1) ? "*" : ppl_c,imm?"@@y":allied?"@@r":"@@e" );
						}
						sprintf( color, "@@N" );
					}
					if (inverse) strcat(color,"@@i");
				}
			}
			else if ( ( map_vhc[x][y][z] != NULL && size != 998 && size != 997 ) )
			{
				char mxpbuf[MSL];
				catbuf[0] = '\0';

				if ( ch->desc->mxp )
				{
					strcat( catbuf, "\e[1z" );
					sprintf( mxpbuf, MXPTAG(ch->desc,"Bl x=%d y=%d owner='Empty' name='%s'"), x,y,map_vhc[x][y][z]->desc );
					strcat( catbuf, mxpbuf );
				}
				if ( str_cmp(color,"@@d") )
				{
					strcat( catbuf, "@@d" );
					sprintf( color, "@@d" );
				}
				if (inverse) strcat(color,"@@i");
				sprintf( catbuf+strlen(catbuf), "[]" );
				if ( ch->desc->mxp )
					strcat( catbuf, MXPTAG(ch->desc,"/Bl"));
				strcat(catbuf,color);
			}
			else
			{
				char ocolor[MSL];
				sprintf( ocolor, "%s", wildmap_table[map_table.type[x][y][ch->z]].color );
				catbuf[0] = '\0';
				if ( str_cmp(color,ocolor) )
				{
					sprintf( color, "%s", ocolor );
					if (inverse) strcat(color,"@@i");
					sprintf( catbuf, "%s", color );
				}
		        	sprintf( catbuf+strlen(catbuf), "%s", ( blind_spot(ch,xx,yy)) ? "  " : wildmap_table[map_table.type[x][y][ch->z]].mark );
				if ( terrain[map_table.type[x][y][ch->z]] == FALSE ) terrain[map_table.type[x][y][ch->z]] = TRUE;

			}
			if ( IS_SET(ch->pcdata->pflags,PLR_ASS) )
			{
				sprintf( catbuf,"  ");
			}

		        safe_strcat( MSL, outbuf, catbuf  );
		    	i++;
		    	if ( i >= 10 )
		    	{
				i = 0;
		    		send_to_char( outbuf, ch );
		    		outbuf[0] = '\0';
		    	}
		}
    		safe_strcat( MSL, outbuf, "\n\r" );
  	}


  /* this is the contents of the map */
  send_to_char( outbuf, ch );
  /* this is the bottom line of the map */
  send_to_char( linebuf, ch );
  send_to_char( "\n\r", ch );
  if ( my_get_hours(ch,TRUE) < 2 && !IS_SET(ch->config,CONFIG_NOLEGEND))
  {
	char tbuf[MSL];
	int j,l=0;
	if ( terrain[SECT_SNOW_BLIZZARD] )
	{
		terrain[SECT_SNOW_BLIZZARD] = FALSE;
		terrain[SECT_SNOW] = TRUE;
	}
	sprintf( tbuf, "@@WLegend:  " );
	for ( j=0;j<SECT_MAX;j++ )
	{
		if ( !terrain[j] )
			continue;
		sprintf( tbuf+strlen(tbuf), "%s%s %-11s ", wildmap_table[j].color, wildmap_table[j].mark, wildmap_table[j].name );
		l++;
		if ( l%4 == 0 )
			sprintf(tbuf+strlen(tbuf), "\n\r         " );
	}
	sprintf(tbuf+strlen(tbuf), "\n\r\n\r@@WItems:@@N\n\r" );
	send_to_char(tbuf,ch);
  }
  x = 480;
  if ( enemy && ft ) x *= 1.3;
  if ( enemy && ch->fighttimer < x && !IS_IMMORTAL(ch) && ch->z != Z_PAINTBALL )
	ch->fighttimer = x;
  return;
}
void ShowSpace( CHAR_DATA *ch )
{
    char ss[MSL] = "\0";
    char aa[MSL] = "\0";
    char ff[MSL] = "\0";
    char buf[MSL] = "\0";
    char pref[MSL] = "\0";
    VEHICLE_DATA *vhc = ch->in_vehicle;
    int i,j= 0;
    pref[0] = '\0';
    if(!vhc)
    {
    	return;
    }
    for ( i=0; i<((j*4)-34); i++ )
        sprintf( pref+strlen(pref)," ");
    j = vhc?get_ship_range(vhc):0;
    sprintf(buf, "\n\r" );
    sprintf(buf+strlen(buf),"%s@@c     ------------------------ \n\r", pref );
    sprintf(buf+strlen(buf),"%s  --/  SCN: %-3d    WPN: %-3d  \\-- \n\r", pref, j, vhc?vhc->range:0 );
    sprintf(buf+strlen(buf),"%s / ______________      @@RSS@@dAA@@aFF@@c_  \\ \n\r", pref  );

    if ( vhc->hit > ( vhc->max_hit / 100 ) * 80 )
        sprintf( ss, "@@r||" );
    else
        sprintf( ss, "  " );
    if ( vhc->ammo > ( vhc->max_ammo / 100 ) * 80 )
        sprintf( aa, "@@r||" );
    else
        sprintf( aa, "  " );
    if ( vhc->fuel > ( vhc->max_fuel / 100 ) * 80 )
        sprintf( ff, "@@r||" );
    else
        sprintf( ff, "  " );

    sprintf(buf+strlen(buf),"%s| |@@aSYS: @@y%9s@@c\\    |%s%s%s @@c\\  | \n\r", pref, "ACTIVE", ss, aa, ff );

    if ( vhc->hit > ( vhc->max_hit / 100 ) * 60 )
        sprintf( ss, "@@y||" );
    else
        sprintf( ss, "  " );
    if ( vhc->ammo > ( vhc->max_ammo / 100 ) * 60 )
        sprintf( aa, "@@y||" );
    else
        sprintf( aa, "  " );
    if ( vhc->fuel > ( vhc->max_fuel / 100 ) * 60 )
        sprintf( ff, "@@y||" );
    else
        sprintf( ff, "  " );

    sprintf(buf+strlen(buf),"%s|  ______________     |%s%s%s  @@c| | \n\r",pref, ss, aa, ff );

    if ( vhc->hit > ( vhc->max_hit / 100 ) * 40 )
        sprintf( ss, "@@e||" );
    else
        sprintf( ss, "  " );
    if ( vhc->ammo > ( vhc->max_ammo / 100 ) * 40 )
        sprintf( aa, "@@e||" );
    else
        sprintf( aa, "  " );
    if ( vhc->fuel > ( vhc->max_fuel / 100 ) * 40 )
        sprintf( ff, "@@e||" );
    else
        sprintf( ff, "  " );
    {
        char msg[MSL] = "\0";
        msg[0] = '\0';
        if ( IS_SET(vhc->flags,VEHICLE_CORROSIVE_A) )
            sprintf( msg, "@@rACID@@c" );
        sprintf(buf+strlen(buf),"%s| |@@aMSG:@@c %9s\\    |%s%s%s  @@c| | \n\r", pref,msg, ss, aa, ff );
    }

    if ( vhc->hit > ( vhc->max_hit / 100 ) * 20 )
        sprintf( ss, "@@R||" );
    else
        sprintf( ss, "  " );
    if ( vhc->ammo > ( vhc->max_ammo / 100 ) * 20 )
        sprintf( aa, "@@R||" );
    else
        sprintf( aa, "  " );
    if ( vhc->fuel > ( vhc->max_fuel / 100 ) * 20 )
        sprintf( ff, "@@R||" );
    else
        sprintf( ff, "  " );

    sprintf(buf+strlen(buf),"%s|                     |%s%s%s    @@c| \n\r", pref, ss, aa, ff );

    sprintf(buf+strlen(buf),"%s @@c\\_                            _/ \n\r", pref );
    sprintf(buf+strlen(buf),"%s   \\_________________________/ \n\r", pref );
    if ( IS_IMMORTAL(ch) )
        sprintf( buf+strlen(buf), "%s           %d/%d\n\r", pref, ch->x,ch->y);

    send_to_char(buf,ch);
}

void draw_space( CHAR_DATA *ch )
{
    char buf[MSL] = "\0";
    int i;
    int j;
    sprintf( buf, "\n\r" );
    for( i=1; i<6; i++ )
    {
        j = number_range(1,6);
        if ( j == 1 )
            sprintf(buf+strlen(buf), " @@y.  @@b+ @@e`     @@y+     @@e`    @@y.  @@R*\n\r" );
        else if ( j == 2 )
            sprintf(buf+strlen(buf), " @@e` @@y.      @@e.     @@B'     @@y+    @@e.   @@e'\n\r" );
        else if ( j == 3 )
            sprintf(buf+strlen(buf), " @@B.  @@b+ @@e`     @@y+     @@R+    @@y.\n\r" );
        else if ( j == 4 )
            sprintf(buf+strlen(buf), " @@b* @@y.      @@R*     @@b'     @@B+    @@e.       @@y+\n\r" );
        else if ( j == 5 )
            sprintf(buf+strlen(buf), " @@e.  @@b+ @@y`     @@B+     @@e`    @@y.\n\r" );
        else if ( j == 6 )
            sprintf(buf+strlen(buf), " @@e` @@R*      @@e.     @@E'     @@e+    @@y.    @@e*\n\r" );
    }
    send_to_char( buf, ch );
    return;
}

void do_bscan(CHAR_DATA *ch,char *argument)
{
    BUILDING_DATA *bld;
    int x, xx, y, yy, dir;
    char buf[MSL] = "\0";

    if (!IS_SET(ch->config, CONFIG_BLIND))
        mreturn("Huh?\r\n",ch);
    if ( ( dir = parse_direction(ch, argument) ) == -1 || argument[0] == '\0' )
        mreturn("Bscan <direction>\r\n",ch);

    x = ch->x, y = ch->y;
    while (1)
    {
        if (dir == DIR_NORTH)
            y++;
        else if (dir == DIR_SOUTH)
            y--;
        else if (dir == DIR_EAST)
            x++;
        else if (dir == DIR_WEST)
            x--;
        if (x < 0 || x > MAX_MAPS || y < 0 || y > MAX_MAPS)
            break;
        if (x > ch->x + ch->map || x < ch->x - ch->map)
            break;
        if (y > ch->y + ch->map || y < ch->y - ch->map)
            break;
        xx=x;
        yy=y;
        bld=map_bld[xx][yy][ch->z];
        if (bld!=NULL)
        {
            sprintf(buf,"%s owned by %s: %d.\n\r", bld->type == BUILDING_DUMMY ? build_table[bld->value[0]].name : bld->name,bld->owned, (dir==DIR_NORTH) || (dir==DIR_SOUTH) ? bld->y : bld->x);
            send_to_char(buf,ch);
        }
    }
    return;
}

char *makesmall( char *arg, int size )
{
    static char small[MSL] = "\0";
    if ( size == 4 )
        size = 2;

    small[0] = arg[1];
    if ( size == 2 )
    {
        small[1] = arg[2];
        small[2] = '\0';
    }
    if ( size == 1 )
    {
        small[1] = '\0';
    }
    return (small);
}
