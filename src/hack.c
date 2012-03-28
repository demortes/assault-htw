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
#include "ack.h"
#include "tables.h"

/* This file should include all the advanced hacking stuff */

#define HACKING_DIR_NAME "Files"
#define LOG_NORMAL      0
#define LOG_ALWAYS      1
#define LOG_NEVER       2

bool check_connection( CHAR_DATA *ch );

DECLARE_DO_FUN( do_hcommands 	);
DECLARE_DO_FUN( do_logon	);
DECLARE_DO_FUN( do_dir		);
DECLARE_DO_FUN( do_crack	);
DECLARE_DO_FUN( do_spoof	);
DECLARE_DO_FUN( do_cd		);
DECLARE_DO_FUN( do_upload	);
DECLARE_DO_FUN( do_download	);
DECLARE_DO_FUN( do_hack		);
DECLARE_DO_FUN( do_scandir	);
DECLARE_DO_FUN( do_format	);
DECLARE_DO_FUN( do_mark		);
const   struct  cmd_type       hack_cmd_table   [] =
{ 
    { "bye",            do_bye,         POS_HACKING,    0,  LOG_NORMAL },
    { "cd",            	do_cd,         	POS_HACKING,    0,  LOG_NORMAL },
    { "commands",       do_hcommands,   POS_HACKING,    0,  LOG_NORMAL },
    { "crack",         	do_crack,      	POS_HACKING,    0,  LOG_NORMAL },
    { "dir",            do_dir,         POS_HACKING,    0,  LOG_NORMAL },
    { "disconnect",     do_bye,         POS_HACKING,    0,  LOG_NORMAL },
    { "download",     	do_download,   	POS_HACKING,    0,  LOG_NORMAL },
    { "exit",           do_bye,         POS_HACKING,    0,  LOG_NORMAL },
    { "format",     	do_format,    	POS_HACKING,    0,  LOG_NORMAL },
    { "hack",     	do_hack,	POS_HACKING,    0,  LOG_NORMAL },
    { "help",           do_hcommands,   POS_HACKING,    0,  LOG_NORMAL },
    { "logon",     	do_logon,	POS_HACKING,    0,  LOG_NORMAL },
    { "ls",             do_dir,         POS_HACKING,    0,  LOG_NORMAL },
    { "mark",     	do_mark,	POS_HACKING,    0,  LOG_NORMAL },
    { "scan",     	do_scandir,	POS_HACKING,    0,  LOG_NORMAL },
    { "spoof",     	do_spoof,	POS_HACKING,    0,  LOG_NORMAL },
    { "upload",     	do_upload,	POS_HACKING,    0,  LOG_NORMAL },
    { "user",     	do_logon,	POS_HACKING,    0,  LOG_NORMAL },
    { "who",            do_who,         POS_HACKING,    0,  LOG_NORMAL },
    { "stop",           do_stop,        POS_HACKING,    0,  LOG_NORMAL },

    { "",               0,              POS_DEAD,        0,  LOG_NORMAL }
};

void hack_interpret( CHAR_DATA *ch, char *argument )
{
    char command[MAX_INPUT_LENGTH];
    char logline[MAX_INPUT_LENGTH];
    int cmd;
    bool found;
      

   if ( ch->position == POS_HACKING )
   {
	if ( check_connection(ch) )
		return;
   }

    /*  
     * Strip leading spaces.
     */
    while ( isspace(*argument) )
        argument++;
    if ( argument[0] == '\0' )
        return;
     
    strcpy( logline, argument );
    if ( !isalpha(argument[0]) && !isdigit(argument[0]) ) 
    {
        command[0] = argument[0];
        command[1] = '\0';
        argument++;
        while ( isspace(*argument) )
            argument++;
    } 
    else
    {  
        argument = one_argument( argument, command );
    }
     
    
    /*
     * Look for command in command table.
     */
    found = FALSE;
    for ( cmd = 0; hack_cmd_table[cmd].name[0] != '\0'; cmd++ )
    {
        if ( command[0] == hack_cmd_table[cmd].name[0]
        &&   !str_prefix( command, hack_cmd_table[cmd].name )
        &&   get_trust(ch) >= hack_cmd_table[cmd].level )
        {
            found = TRUE;
            break;
        }
    }

   if ( IS_BUSY(ch) && str_cmp(hack_cmd_table[cmd].name,"stop") )
   {
	send_to_char( "Finish or 'stop' your other process, first.\n\r", ch );
	return;
   }
            
    /*
     * Log and snoop.
     */
    if ( hack_cmd_table[cmd].log == LOG_NEVER )
        strcpy( logline, "XXXXXXXX XXXXXXXX XXXXXXXX" );
     
    if ( ( IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   hack_cmd_table[cmd].log == LOG_ALWAYS )
    {
        sprintf( log_buf, "Log %s: %s", ch->name, logline );
        log_string( log_buf );
    }
    if ( ch->desc != NULL && ch->desc->snoop_by != NULL ) 
    {
        write_to_buffer( ch->desc->snoop_by, "% ",    2 );
        write_to_buffer( ch->desc->snoop_by, logline, 0 );
        write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }   
     
    if ( !found )
    {
	send_to_char( "Invalid command.\n\r", ch );
        return;
    }
    
    
    (*hack_cmd_table[cmd].do_fun) ( ch, argument );
        
    tail_chain( );
    return;
}

void do_bye( CHAR_DATA *ch, char *argument )
{
	if ( !ch->bvictim )
	{
		ch->position = POS_STANDING;
		return;
	}
	if ( !str_cmp(argument,"reset") )
	{
		send_to_char("Password reset.\n\r", ch );
		ch->bvictim->password = number_range(10000,99999);
	}
	if ( ch )
	{
		send_to_char( "Connection Terminated...\n\r", ch );
		act( "$n pulls $s computer back to $s inventory.", ch, NULL, NULL, TO_ROOM );
		ch->bvictim->value[8] = 0;
		ch->bvictim = NULL;
		ch->c_obj = NULL;
		if ( ch->position != POS_DEAD )
			ch->position = POS_STANDING;
	}
	return;
}

void do_hcommands( CHAR_DATA *ch, char *argument )
{
   char buf[MAX_STRING_LENGTH];
   char out[MAX_STRING_LENGTH];
   int cmd;
   int col = 0;
    
   sprintf( out, "Hacking Commands Available to You:\n\r" );   
    
   for ( cmd = 0; hack_cmd_table[cmd].name[0] != '\0'; cmd++ )
   {
      if ( hack_cmd_table[cmd].level > get_trust( ch ) )
         continue;
      sprintf( buf, "%-20s    ", hack_cmd_table[cmd].name );
      safe_strcat( MSL, out, buf );
      if ( ++col % 3 == 0 )
         safe_strcat( MSL, out, "\n\r" );
   }
   safe_strcat( MSL, out, "\n\r" );
   send_to_char( out, ch );
   return;
}

void do_dir( CHAR_DATA *ch, char *argument )
{
	int i;
	int col = 0;
	char buf[MSL];
	char out[MSL];
	if ( ch->bvictim->password != 0 && ch->bvictim->level > 1 )
	{
		send_to_char( "Not Authorized to view directory tree. Please log on.\n\r", ch );
		return;
	}
	sprintf( out, "Directories:\n\r" );
	for ( i=1;i<=ch->bvictim->directories;i++)
	{
		sprintf( buf, "%s %-14d", HACKING_DIR_NAME, i );
		safe_strcat( MSL, out, buf );
		if ( ++col % 3 == 0 )
			safe_strcat( MSL, out, "\n\r" );
	}
	safe_strcat( MSL, out, "\n\r" );
	send_to_char( out, ch );
	return;
}

void do_logon( CHAR_DATA *ch, char *argument )
{
	int pass;

	if ( argument[0] == '\0' )
	{
		send_to_char( "Syntax: logon <password>\n\r", ch );
		return;
	}
	if ( !is_number(argument) )
	{
		send_to_char( "Invalid Password. Password must be numeric.\n\r", ch );
		return;
	}
	if ( ch->bvictim->password == 0 )
	{
		send_to_char( "You are already logged as ADMIN.\n\r", ch );
		return;
	}
	pass = atoi(argument);
	if ( pass != ch->bvictim->password )
	{
		CHAR_DATA *vch;
		send_to_char( "Password Incorrect. Goodbye.\n\r\n\r", ch );
		vch = get_ch(ch->bvictim->owned);
		if ( vch )
		{
			send_to_char( "@@yThere has been an attempt to breach one of your systems' security!@@N\n\r", vch );
			set_fighting(ch,vch);
		}
		do_bye(ch,"");
		return;
	}
	send_to_char( "Access Granted. Authorizing ADMIN commands.\n\r", ch );
	ch->bvictim->password = 0;
	return;
}

void do_crack( CHAR_DATA *ch, char *argument )
{
	bool found = FALSE;
	OBJ_DATA *obj;
	int version = -1;

	if ( ch->bvictim->password == 0 )
	{
		send_to_char( "You are already logged on as ADMIN.\n\r", ch );
		return;
	}
	for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
	{
		if ( obj->item_type != ITEM_DISK || obj->value[0] != 1 || obj->value[1] < version )
			continue;
		found = TRUE;
		version = obj->value[1];
	}
	if ( !found )
	{
		send_to_char( "You must be carrying a disk with a password-cracking software.\n\r", ch );
		return;
	}
	ch->c_sn = gsn_crack;
	ch->c_time = 50 - version;
	ch->c_level = ch->bvictim->password;
	if ( ch->bvictim->value[3] < 0 )
		ch->c_time /= 1.2;
	if ( IS_SET(ch->bvictim->value[1],INST_FIREWALL) )
	{
		send_to_char( "\n\r@@eWARNING: FIREWALL DETECTED.@@N\n\r\n\r", ch );
		ch->c_time *= 2;
		if ( ch->bvictim->value[3] < 0 || ch->class == CLASS_HACKER )
		{
			send_to_char("\n\r@@aFIREWALL BYPASSED.@@N\n\r\n\r", ch );
			ch->c_time /= 2;
		}
	}
	if ( number_percent() < version )
	{
		ch->c_level -= number_range(0,3) * 10000;
		ch->c_level -= number_range(0,3) * 1000;
		ch->c_level -= number_range(0,3) * 100;
		ch->c_level -= number_range(0,3) * 10;
		ch->c_level -= number_range(0,3);
	}
	else
		ch->c_level = 10000;
	return;
}
void act_crack( CHAR_DATA *ch, int level )
{
	char password[MSL];
	int pdig[5];
	int dig[5];
	int t = 0;
	CHAR_DATA *vch;
	char buf[MSL];


	if ( check_connection(ch) )
		return;

	if ( ch->bvictim->owner == NULL || !ch->bvictim->owner )
		return;
	if ( IS_SET(ch->bvictim->value[1],INST_FIREWALL) && number_percent() < 15 && ch->bvictim->value[3] == 0 && ch->class != CLASS_HACKER )
	{
		send_to_char( "@@eWARNING! Illegal Program Terminated by Firewall!@@N\n\r", ch );
		if ( ( vch = ch->bvictim->owner ) != NULL )
		{
			send_to_char( "@@yAn intruder has been discovered attempting to hack one of your mainframes!@@N\n\r", vch );
			sprintf( buf, "@@yDetails:\n\rName: %s   Location: %d/%d@@N\n\r", ch->name,ch->x,ch->y);
			send_to_char( buf, vch );
			ch->bvictim->value[8] = 0;
			set_fighting(ch,vch);
			if ( number_percent() < 50 )
			{
				send_to_char( "Your computer starts flashing wildly, then @@yZAPS@@N you!\n\r", ch );
				ch->c_sn = -1;
//				ch->bvictim = NULL;
				do_bye(ch,"reset");
//				damage(vch,ch,100,DAMAGE_PSYCHIC);
				return;
			}
		}
		if ( ch )
			do_bye(ch,"reset");
		return;
	}


	pdig[0] = ch->bvictim->password / 10000;
	pdig[1] = (ch->bvictim->password / 1000)%10;
	pdig[2] = (ch->bvictim->password / 100)%10;
	pdig[3] = (ch->bvictim->password / 10)%10;
	pdig[4] = (ch->bvictim->password)%10;
	dig[0] = ch->c_level / 10000;
	dig[1] = (ch->c_level / 1000)%10;
	dig[2] = (ch->c_level / 100)%10;
	dig[3] = (ch->c_level / 10)%10;
	dig[4] = (ch->c_level)%10;

	sprintf( password, "%d %d %d %d %d\n\r", dig[0],dig[1],dig[2],dig[3],dig[4]);
	send_to_char(password,ch);

	if ( ch->c_level == ch->bvictim->password )
	{
		if ( ( vch = get_ch(ch->bvictim->owned) ) != NULL )
		{
			send_to_char( "@@yAn intruder has been discovered attempting to hack one of your mainframes!@@N\n\r", vch );
		}

		send_to_char( "\n\rBingo!\n\r\n\r", ch );
		ch->c_sn = -1;
		ch->c_level = 0;
		return;
	}


	if ( pdig[0] != dig[0] )
	{
		dig[0] = number_range(1,9);
		t += 8;
	}
	if ( pdig[1] != dig[1] )
	{
		dig[1] = number_range(0,9);
		t += 8;
	}
	if ( pdig[2] != dig[2] )
	{
		dig[2] = number_range(0,9);
		t += 8;
	}
	if ( pdig[3] != dig[3] )
	{
		dig[3] = number_range(0,9);
		t += 8;
	}
	if ( pdig[4] != dig[4] )
	{
		dig[4] = number_range(0,9);
		t += 8;
	}
	ch->c_level = (dig[0] * 10000)+(dig[1] * 1000)+(dig[2] * 100)+(dig[3] * 10)+dig[4];
	ch->c_time = t - ch->bvictim->value[8];
	if ( IS_SET(ch->bvictim->value[1],INST_FIREWALL) && ch->bvictim->value[3] == 0 && ch->class != CLASS_HACKER)
		ch->c_time *= 2;
	if ( ch->bvictim->value[3] < 0 )
		ch->c_time /= 1.2;

	return;
}

void do_cd( CHAR_DATA *ch, char *argument )
{
	int dir;
	char arg[MSL];
	char buf[MSL];

	argument = one_argument(argument,arg);
	if ( ch->bvictim->password != 0 )
	{
		send_to_char( "Unable to process command. Must be logged as ADMIN.\n\r", ch );
		return;
	}
	if ( str_cmp(HACKING_DIR_NAME,arg) || !is_number(argument) )
	{
		send_to_char( "Directory does not exist.\n\r", ch );
		return;
	}
	dir = atoi(argument);
	if ( dir < 1 || dir > ch->bvictim->directories )
	{
		send_to_char( "Directory does not exist.\n\r", ch );
		return;
	}

	if ( dir != ch->bvictim->real_dir )
	{
		CHAR_DATA *vch;
		sprintf( buf, "Displaying Contents of Directory: %s\n\r\n\r   No Files Found.\n\r\n\r@@eINTRUDER ALERT!@@N\n\r\n\r\n\r", argument );
		send_to_char( buf, ch );
		if ( ( vch = get_ch(ch->bvictim->owned) ) != NULL )
			send_to_char( "@@yAn intruder has been discovered attempting to hack one of your mainframes!@@N\n\r", vch );
		if ( IS_SET(ch->bvictim->value[1],INST_FIREWALL) )
			do_bye(ch,"reset");
		else
			do_bye(ch,"");
		return;
	}
	sprintf( buf, "Displaying Contents of Directory: %s\n\r\n\rBase_data.dat    %dKB\n\r", argument, ch->bvictim->value[8]+number_range(0,100) );
	send_to_char( buf, ch );
	return;	
}

void do_upload( CHAR_DATA *ch, char *argument )
{
	char arg1[MSL];
	char arg2[MSL];
	int dir, vir,x;
	OBJ_DATA *obj;
	bool found = FALSE;
	char buf[MSL];

	argument = one_argument(argument,arg1);
	argument = one_argument(argument,arg2);

	if ( strlen(arg1) != 3 )
	{
		send_to_char( "Virus version must be in the format: x.x  (1.3, 2.0, etc)\n\r", ch );
		return;
	}

	x = number_argument(arg1,arg1);
	sprintf( arg1, "%d", x * 10 + atoi(arg1));
	if ( !is_number(arg1) || is_number(arg2) || !is_number(argument) )
	{
		send_to_char( "Syntax: upload <virus version> <directory>\n\rE.g. upload 2.0 Files 1\n\r", ch );
		return;
	}
	if ( ch->bvictim->password != 0 )
	{
		send_to_char( "Unable to process command. Must be logged as ADMIN.\n\r", ch );
		return;
	}
	vir = atoi(arg1);
	buf[0] = '\0';
	for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
	{
		if ( obj->item_type != ITEM_DISK || obj->value[0] != 0 )
			continue;

		sprintf( buf+strlen(buf), "Version %d.%d\n\r", obj->value[1] / 10, obj->value[1] % 10 );
		if ( obj->value[1] != vir )
			continue;
		found = TRUE;
		break;
	}
	if ( !found )
	{
		send_to_char( "You must be carrying a disk with the selected virus version.\n\r\n\rYou carry the following disks:\n\r", ch );
		send_to_char(buf,ch);
		return;
	}
	dir = atoi(argument);
	if ( str_cmp(arg2,HACKING_DIR_NAME) || dir < 1 || dir > ch->bvictim->directories )
	{
		send_to_char( "Directory does not exist.\n\r", ch );
		return;
	}
	send_to_char( "Upload initiated...\n\r", ch );
	if ( dir != ch->bvictim->real_dir || number_percent() < vir)
	{
		CHAR_DATA *vch;
		send_to_char( "@@eINTRUDER ALERT!@@N\n\r\n\r", ch );
		if ( ( vch = get_ch(ch->bvictim->owned) ) != NULL )
			send_to_char( "@@yAn intruder has been discovered attempting to hack one of your mainframes!@@N\n\r", vch );
		if ( IS_SET(ch->bvictim->value[1],INST_FIREWALL) )
			do_bye(ch,"reset");
		else
			do_bye(ch,"");
		return;
	}
	send_to_char( "Virus upload successful.\n\r", ch );
	ch->bvictim->value[3] = vir*(-1);
	if ( number_percent() < 20 )
	{
		send_to_char( "Your virus disk has fallen apart!\n\r", ch );
		extract_obj(obj);
	}
	return;
}

void do_download( CHAR_DATA *ch, char *argument )
{
	char buf[MSL];
	char arg2[MSL];
	int dir;

	argument = one_argument(argument,arg2);

	if ( is_number(arg2) || !is_number(argument) )
	{
		send_to_char( "Syntax: download <directory>\n\rE.g. download Files 1\n\r", ch );
		return;
	}
	if ( ch->bvictim->password != 0 )
	{
		send_to_char( "Unable to process command. Must be logged as ADMIN.\n\r", ch );
		return;
	}
	dir = atoi(argument);
	if ( str_cmp(arg2,HACKING_DIR_NAME) || dir < 1 || dir > ch->bvictim->directories )
	{
		send_to_char( "Directory does not exist.\n\r", ch );
		return;
	}
	send_to_char( "Download initiated...\n\r", ch );
	if ( dir != ch->bvictim->real_dir )
	{
		CHAR_DATA *vch;
		send_to_char( "@@eINTRUDER ALERT!@@N\n\r\n\r", ch );
		if ( ( vch = get_ch(ch->bvictim->owned) ) != NULL )
			send_to_char( "@@yAn intruder has been discovered attempting to hack one of your mainframes!@@N\n\r", vch );
		if ( IS_SET(ch->bvictim->value[1],INST_FIREWALL) )
			do_bye(ch,"reset");
		else
			do_bye(ch,"");
		return;
	}
	sprintf( buf, "You have downloaded the contents of %s %d.\n\r", arg2,atoi(argument) );
	send_to_char(buf,ch);
	if ( ch->bvictim->type == BUILDING_GOVERNMENT_HALL )
	{
		CHAR_DATA *vch;
		if ( ( vch = get_ch(ch->bvictim->owned) ) != NULL && vch->pcdata->alliance != -1 )
		{
			send_to_char( "The file contains the following information:\n\r\n\r", ch );
        		sprintf(buf,"fgrep -lx 'Alliance     %d' %s*/*", vch->pcdata->alliance, PLAYER_DIR);
        		do_pipe(ch, buf);		
		}
		else
			send_to_char( "The file contained nothing of importance.\n\r", ch );
	}
	if ( ch->bvictim->type == BUILDING_HQ )
	{
		do_listbuildings(ch,ch->bvictim->owned);
		return;
	}
	if ( ch->bvictim->type == BUILDING_MINE || ch->bvictim->type == BUILDING_IMPROVED_MINE )
	{
		sprintf( buf, "The mine is currently mining for: %s.\n\r", ch->bvictim->value[0] == 0 ? "Iron" : ch->bvictim->value[0] == 2 ? "Copper" : ch->bvictim->value[0] == 3 ? "Gold" : ch->bvictim->value[0] == 4 ? "Silver" : "Nothing in particular" );
		send_to_char( buf, ch );
		return;
	}
	if ( ch->bvictim->type == BUILDING_ARMORY )
	{
		sprintf( buf, "The armory is currently producing: %s.\n\r", ch->bvictim->value[0] != 0 ? get_obj_index(ch->bvictim->value[0])->short_descr : "Nothing in particular" );
		send_to_char( buf, ch );
		return;
	}
	return;
}

void do_hack( CHAR_DATA *ch, char *argument )
{
	char buf[MSL];
	CHAR_DATA *vch;

	if ( ( vch = get_ch(ch->bvictim->owned)) == NULL )
	{
		send_to_char( "Mainframe off line.\n\r", ch );
		do_bye(ch,"");
		return;
	}
	if ( ch->bvictim->password != 0 )
	{
		send_to_char( "Unable to process command. Must be logged as ADMIN.\n\r", ch );
		return;
	}
	if ( ch->bvictim->type == BUILDING_GOVERNMENT_HALL )
	{
		if ( vch->pcdata->alliance == -1 )
		{
			send_to_char( "That player is not a member of an alliance.", ch );
			return;
		}
		sprintf( buf,"You begin inserting your data into the %s database!\n\r", alliance_table[vch->pcdata->alliance].name );
		send_to_char(buf,ch);
		ch->c_sn = gsn_hack;
		ch->c_time = 40 - ch->bvictim->value[8];
		ch->c_level = 1;
		send_to_char( "1%\n\r", ch );
		return;
	}
	if ( ch->bvictim->type == BUILDING_MINE || ch->bvictim->type == BUILDING_IMPROVED_MINE )
	{
		send_to_char( "You begin messing with the mine's definitions.\n\r", ch );
		ch->c_sn = gsn_hack;
		ch->c_time = 40 - ch->bvictim->value[8];
		ch->c_level = 1;
		send_to_char( "1%\n\r", ch );
		return;
	}
	if ( ch->bvictim->type == BUILDING_ARMORY )
	{
		send_to_char( "You begin messing with the armory's definitions.\n\r", ch );
		ch->c_sn = gsn_hack;
		ch->c_time = 40 - ch->bvictim->value[8];
		ch->c_level = 1;
		send_to_char( "1%\n\r", ch );
		return;
	}
	else
		send_to_char( "There is nothing worthwhile to hack into here.\n\r", ch );
	return;
}

void act_hack( CHAR_DATA *ch, int level )
{
	char buf[MSL];
	CHAR_DATA *vch;

	if ( check_connection(ch) )
		return;

	if ( ( vch = get_ch(ch->bvictim->owned)) == NULL )
	{
		send_to_char( "Mainframe off line.\n\r", ch );
		do_bye(ch,"");
		return;
	}
	ch->c_level += number_range(1,5);
	if ( ch->c_level >= 100 )
	{
		if ( ch->bvictim->type == BUILDING_GOVERNMENT_HALL )
		{
			if ( vch->pcdata->alliance == -1 )
			{
				send_to_char( "All of your target's alliance data has been erased!\n\r", ch );
				ch->c_sn = -1;
				return;
			}
			sprintf( buf, "\n\rDone!\n\rYou are now a member of %s@@N!", alliance_table[vch->pcdata->alliance].name );
			send_to_char( buf, ch );
			if ( ch->pcdata->alliance != -1 )
				alliance_table[ch->pcdata->alliance].members--;
			alliance_table[vch->pcdata->alliance].members++; 
			ch->pcdata->alliance = vch->pcdata->alliance;
		}
		if ( ch->bvictim->type == BUILDING_MINE || ch->bvictim->type == BUILDING_IMPROVED_MINE )
		{
			ch->bvictim->value[0] = 0;
			send_to_char( "\n\rDone!\n\rMine is set to search for: Iron.\n\r", ch );
		}
		if ( ch->bvictim->type == BUILDING_MARKETPLACE)
		{
			CHAR_DATA *victim = get_ch(ch->bvictim->owned);
			if ( victim )
			{
				victim->quest_points -= 1000;
				if ( victim->quest_points < 0 )
					victim->quest_points = 0;
				send_to_char( "Your quest points have been drained from the marketplace!\n\r", victim );
				send_to_char( "You've drained the quest points from the marketplace!\n\r", ch );
			}
		}
		if ( ch->bvictim->type == BUILDING_ARMORY)
		{
			ch->bvictim->value[0] = 1000;
			send_to_char( "\n\rDone!\n\rArmory is set to produce: Pistols.\n\r", ch );
		}
		if ( ch->bvictim->type == BUILDING_HQ )
		{
/*			BUILDING_DATA *bld2;
			int i;
			for ( bld2=first_building;bld2;bld2 = bld2->next )
				if ( is_neutral(bld2->type) && !str_cmp(bld2->owned,ch->bvictim->owned) )
				{
					free_string(bld2->owned);
					bld2->owned = str_dup("Nobody");
					bld2->owner = NULL;
					for ( i=0;i<10;i++ )
						bld2->value[i] = 0;
				}
			send_to_char( "\n\rDone!\n\rControl over neutral buildings has been lifted.\n\r", ch );*/
			ch->security = FALSE;
			send_to_char( "Switched off Security!\n\r", ch );
			if ( ch->bvictim->owner && number_percent() < 50 )
				send_to_char( "@@eYour headquarters reports your security has been turned off!@@N\n\r", ch->bvictim->owner );
		}
		if ( ch->bvictim->type == BUILDING_DUMMY )
		{
			ch->bvictim->value[5] = 1;
			send_to_char( "Dummy disabled!\n\r", ch );
		}
		ch->c_sn = -1;
		return;
	}
	sprintf( buf, "%d%%", ch->c_level );
	send_to_char( buf, ch );
	ch->c_time = 40 - ch->bvictim->value[8];
	return;
}


bool check_connection( CHAR_DATA *ch )
{
//	if ( ch->position == POS_HACKING )
	{
        	if ( ch->bvictim == NULL )
        	{
			send_to_char( "You have lost link with your target.\n\r", ch );
        	        ch->position = POS_STANDING; 
			ch->c_sn = -1;
        	        return TRUE;
        	}
        	if ( ch->bvictim->active == FALSE )
        	{
        	        send_to_char( "Mainframe shut down.\n\r", ch );
			do_bye(ch,"");
			ch->c_sn = -1;
       	         	return TRUE;
       	 	}
	}
	return FALSE;
}

void do_format( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	int version = -1;
	bool found = FALSE;

	if ( ch->bvictim->password != 0 )
	{
		send_to_char( "Unable to process command. Must be logged as ADMIN.\n\r", ch );
		return;
	}
	for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
	{
		if ( obj->item_type != ITEM_DISK || obj->value[0] != 2 || obj->value[1] < version )
			continue;
		found = TRUE;
		version = obj->value[1];
	}
	if ( !found )
	{
		send_to_char( "You must be carrying a disk with a formatting software.\n\r", ch );
		return;
	}
	ch->c_sn=gsn_format;
	ch->c_time = 40 - ch->bvictim->value[8] - version;
	ch->c_level = number_range(version/2,version);
//	send_to_char( "1%",ch );
	return;
}
void act_format( CHAR_DATA *ch, int level )
{
	char buf[MSL];
	CHAR_DATA *vch;

	if ( check_connection(ch) )
		return;

	if ( ( vch = get_ch(ch->bvictim->owned)) == NULL )
	{
		send_to_char( "Mainframe off line.\n\r", ch );
		do_bye(ch,"");
		return;
	}
	ch->c_level += number_range(1,5);
	if ( ch->c_level >= 100 )
	{
		ch->bvictim->level = 1;
		ch->bvictim->value[1] = 0;
		ch->bvictim->value[3] = 0;
		ch->bvictim->hp = build_table[ch->bvictim->type].hp;
		ch->bvictim->maxhp = build_table[ch->bvictim->type].hp;
		ch->bvictim->shield = build_table[ch->bvictim->type].shield;
		ch->bvictim->maxshield = build_table[ch->bvictim->type].shield;

		send_to_char("100%\n\r\n\rFormatting Complete.\n\r", ch );
		if ( ch->bvictim->type == BUILDING_WAREHOUSE )
		{
			OBJ_DATA *obj;
			OBJ_DATA *obj_next;
			int i=0;
			for ( obj = map_obj[ch->bvictim->x][ch->bvictim->y];obj;obj = obj_next )
			{
				obj_next = obj->next_in_room;
				if ( obj->z != ch->bvictim->z )
					continue;
				i++;
				if ( i <= 20 )
					continue;
				extract_obj(obj);
			}
		}
		ch->c_sn = -1;
		return;
	}
	sprintf( buf, "%d%%", ch->c_level );
	send_to_char( buf, ch );
	ch->c_time = 40 - ch->bvictim->value[8];
	return;	
}

void do_scandir( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	int version = -1;
	bool found = FALSE;

	if ( ch->bvictim->password != 0 )
	{
		send_to_char( "Unable to process command. Must be logged as ADMIN.\n\r", ch );
		return;
	}
	for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
	{
		if ( obj->item_type != ITEM_DISK || obj->value[0] != 3 || obj->value[1] < version )
			continue;
		found = TRUE;
		version = obj->value[1];
	}
	if ( !found )
	{
		send_to_char( "You must be carrying a disk with a scanning software.\n\r", ch );
		return;
	}
	ch->c_sn=gsn_scan;
	ch->c_time = 40 - ch->bvictim->value[8] - version;
	ch->c_level = number_range(version/2,version);
	return;
}
void act_scandir( CHAR_DATA *ch, int level )
{
	char buf[MSL];
	CHAR_DATA *vch;

	if ( check_connection(ch) )
		return;

	if ( ( vch = get_ch(ch->bvictim->owned)) == NULL )
	{
		send_to_char( "Mainframe off line.\n\r", ch );
		do_bye(ch,"");
		return;
	}
	ch->c_level += number_range(1,5);
	if ( ch->c_level >= 100 )
	{
		int i;
		sprintf( buf, "\n\rScan complete.\n\r\n\r" );
		for ( i=1;i<=ch->bvictim->directories;i++ )
			sprintf( buf+strlen(buf), "%s %d - %s\n\r", HACKING_DIR_NAME, i, (i==ch->bvictim->real_dir)?"1 File":"Empty" );
		send_to_char(buf,ch);
		ch->c_sn = -1;
		return;
	}
	sprintf( buf, "%d%%", ch->c_level );
	send_to_char( buf, ch );
	ch->c_time = 40 - ch->bvictim->value[8];
	return;	
}

void do_spoof( CHAR_DATA *ch, char *argument )
{
	OBJ_DATA *obj;
	int version = -1;
	bool found = FALSE;

	if ( ch->bvictim->password != 0 )
	{
		send_to_char( "Unable to process command. Must be logged as ADMIN.\n\r", ch );
		return;
	}
	for ( obj = ch->first_carry;obj;obj = obj->next_in_carry_list )
	{
		if ( obj->item_type != ITEM_DISK || obj->value[0] != 4 || obj->value[1] < version )
			continue;
		found = TRUE;
		version = obj->value[1];
	}
	if ( !found )
	{
		send_to_char( "You must be carrying a disk with a spoofing software.\n\r", ch );
		return;
	}
	if ( IS_SET(ch->bvictim->value[1],INST_SPOOF) )
	{
		send_to_char( "A spoofing program is already running here!\n\r", ch );
		return;
	}
	ch->c_sn=gsn_spoof;
	ch->c_time = 40 - ch->bvictim->value[8] - version;
	ch->c_level = number_range(version/2,version);
	return;
}
void act_spoof( CHAR_DATA *ch, int level )
{
	char buf[MSL];
	CHAR_DATA *vch;

	if ( check_connection(ch) )
		return;

	if ( ( vch = get_ch(ch->bvictim->owned)) == NULL )
	{
		send_to_char( "Mainframe off line.\n\r", ch );
		do_bye(ch,"");
		return;
	}
	ch->c_level += number_range(1,5);
	if ( ch->c_level >= 100 )
	{
		if ( !IS_SET(ch->bvictim->value[1],INST_SPOOF) )
			SET_BIT(ch->bvictim->value[1],INST_SPOOF);
		send_to_char("100%\n\r\n\rSpoofing Complete.\n\r", ch );
		ch->c_sn = -1;
		return;
	}
	sprintf( buf, "%d%%", ch->c_level );
	send_to_char( buf, ch );
	ch->c_time = 40 - ch->bvictim->value[8];
	return;	
}

void do_mark( CHAR_DATA *ch, char *argument )
{
	CHAR_DATA *bch = get_ch(ch->bvictim->owned);

	if ( ch->bvictim->z == Z_UNDERGROUND )
	{
		send_to_char( "You can't call an airstrike against underground targets.\n\r", ch );
		return;
	}
	ch->c_sn = gsn_mark;
	ch->c_time = 8;
	ch->c_level = 30;
	if ( ch->bvictim->password != 0 )
		ch->c_level *= 1.5;
	if ( ch->bvictim->value[3] < 0 )
		ch->c_level /= 1.2;
	send_to_char( "You mark the building for an air strike!\n\r", ch );
	if ( bch )
	{
		char buf[MSL];
		sprintf( buf, "@@eYour %s at @@a%d@@c/@@a%d@@e has been targetted for an airstrike!@@N\n\r", ch->bvictim->name, ch->bvictim->x,ch->bvictim->y );
		if ( number_percent() < 50 )
			sprintf( buf+strlen(buf), "@@eIt has originated from @@a%d@@c/@@a%d@@e, by @@a%s@@e.@@N\n\r", ch->x, ch->y, ch->name );
		send_to_char( buf,bch );
	}
	return;
}
void act_mark( CHAR_DATA *ch, int level )
{
	char buf[MSL];
	if ( !ch->bvictim )
	{
		ch->c_sn = -1;
		return;
	}
	if ( level == 0 )
	{
		CHAR_DATA *bch = get_ch(ch->bvictim->owned);
		CHAR_DATA *wch;
		int x,y,z;
		OBJ_DATA *obj;
		OBJ_DATA *obj2;
		BUILDING_DATA *bld;

                x = ch->bvictim->x - 5;
                y = ch->bvictim->y - 5;
                if ( x < 0 )
                        x = 0;
                if ( y < 0 )
                        y = 0;
                for ( x = x;x <= ch->bvictim->x + 5;x++ )
                {
                        for ( y = y;y <= ch->bvictim->y + 5;y++ )
                        {
                                if ( INVALID_COORDS(x,y) )
                                        continue;
                                if ( ( bld = map_bld[x][y][ch->bvictim->z] ) == NULL || bld->type != BUILDING_MISSILE_DEFENSE || !complete(bld) )
                                        continue;
                                if ( ( wch = get_ch(bld->owned) ) == NULL ) 
                                        return;
                                send_to_char( "@@eYour missile defense system has intercepted a bomb!@@N\n\r", wch );
				ch->bvictim->value[8] = 0;
				ch->bvictim = NULL;
				ch->c_sn = -1;
				ch->position = POS_STANDING;
				return;
                        }
                }
		x = ch->bvictim->x;
		y = ch->bvictim->y;
		z = ch->bvictim->z;
		send_to_char( "Two stealth planes arrive, dropping missiles at the target.\n\r", ch );
		ch->bvictim->value[8] = 0;
		ch->bvictim = NULL;
		ch->position = POS_STANDING;
		if ( bch )
			send_to_char( "The planes have arrived at your base.\n\r", bch );
		obj = create_object(get_obj_index(OBJ_VNUM_CANNONBALL),0);
		obj->weight = 50;
		obj->value[2] = 50;
		free_string(obj->short_descr);
		obj->short_descr = str_dup("A Missile");
		move_obj(obj,x,y,z);
		free_string(obj->owner);
		obj->owner = str_dup(ch->name);
		obj2 = create_object(get_obj_index(OBJ_VNUM_CANNONBALL),0);
		obj2->weight = 50;
		obj2->value[2] = 50;
		free_string(obj2->short_descr);
		obj2->short_descr = str_dup("A Missile");
		move_obj(obj2,x,y,z);
		free_string(obj2->owner);
		obj2->owner = str_dup(ch->name);
		explode(obj);
		explode(obj2);
		ch->c_sn = -1;
		return;
	}
	sprintf( buf, "Air strike arriving in: %d Second(s)!\n\r", level );
	send_to_char(buf,ch);
	ch->c_sn = gsn_mark;
	ch->c_time = 8;
	ch->c_level = level - 1;
	return;
}
