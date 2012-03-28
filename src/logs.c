/*
 * Log module added to Assault by Grave
 * Includes 'changes', 'bug', 'typo', and 'idea'
 */

#if defined(macintosh)
#include <types.h>
#else
#include <sys/types.h>
#include <sys/time.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include "ack.h"

/*
 * Local Functions
 */
int      totChanges;
int      maxChanges;
int      totLogs;
int      maxLogs;
#define  NULLSTR( str )  ( str == NULL || str[0] == '\0' )

CHANGE_DATA * changes_table;
LOG_DATA * log_table;

char * current_date( )
{
    static char buf [ 128 ];
    struct tm * datetime;

    datetime = localtime( &current_time );
    strftime( buf, sizeof( buf ), "%b %d, %Y", datetime );
    return buf;
}

void load_changes(void)
{
    FILE *fp;
    int i;

    totChanges = 0;
    if ( !(fp = fopen( CHANGES_FILE, "r")) )
    {
        bug( "Could not open Changes File for reading.", 0 );
        return;
    }

    fscanf( fp, "%d\n", &maxChanges );

    /* Use malloc so we can realloc later on */
    changes_table = malloc( sizeof( CHANGE_DATA) * (maxChanges+1) );

    for( i = 0; i < maxChanges; i++ )
    {
        totChanges++;
        changes_table[i].change = fread_string( fp );
        changes_table[i].coder = fread_string( fp );
        changes_table[i].date = fread_string( fp );
        changes_table[i].mudtime = fread_number( fp );
    }
    changes_table[maxChanges].coder = str_dup("");
    fclose(fp);
    return;                                                 /* just return */
}

void save_changes(void)
{
    FILE *fp;
    int i;

    if ( !(fp = fopen( CHANGES_FILE,"w")) )
    {
        perror( CHANGES_FILE );
        return;
    }

    fprintf( fp, "%d\n", maxChanges );
    for( i = 0; i < maxChanges; i++ )
    {
        fprintf (fp, "%s~\n", changes_table[i].change);
        fprintf (fp, "%s~\n", changes_table[i].coder);
        fprintf (fp, "%s~\n", changes_table[i].date);
        fprintf (fp, "%ld\n",  changes_table[i].mudtime );
        fprintf( fp, "\n" );
    }

    fclose(fp);
    return;
}

void delete_change(int iChange)
{
    int i,j;
    CHANGE_DATA * new_table;

    new_table = malloc( sizeof( CHANGE_DATA ) * maxChanges );

    if( !new_table )
    {
        return;
    }

    for ( i= 0, j = 0; i < maxChanges+1; i++)
    {
        if( i != iChange )
        {
            new_table[j] = changes_table[i];
            j++;
        }
    }

    free( changes_table );
    changes_table = new_table;

    maxChanges--;

    return;
}

void do_addchange(CHAR_DATA *ch, char *argument )
{
    CHANGE_DATA * new_table;

    if ( IS_NPC( ch ) )
        return;

    if ( argument[0] == '\0' )
    {
        send_to_char( "Syntax: addchange [change string]\n\r", ch );
        send_to_char( "Type 'changes' to view the list.\n\r", ch );
        return;
    }

    maxChanges++;
    new_table = realloc( changes_table, sizeof( CHANGE_DATA ) *(maxChanges+1) );

    if (!new_table)                                         /* realloc failed */
    {
        send_to_char ("Memory allocation failed. Brace for impact.\n\r",ch);
        return;
    }

    changes_table = new_table;

    changes_table[maxChanges-1].change  = str_dup( argument );
    changes_table[maxChanges-1].coder   = str_dup( ch->name );
    changes_table[maxChanges-1].date    = str_dup(current_date());
    changes_table[maxChanges-1].mudtime = current_time;

    send_to_char("Changes Created.\n\r",ch);
    send_to_char("Type '@@echanges@@w' to see the changes.\n\r",ch);
    info("New change added to the MUD, type '@@echanges@@g' to see it.", 0);
    save_changes();
    load_changes();
    return;
}

void do_chedit( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MSL];

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( IS_NPC(ch) )
        return;

    if (!ch->desc || NULLSTR(arg1) )
    {
        send_to_char("Syntax: chedit load/save\n\r",ch);
        send_to_char("Syntax: chedit delete (change number)\n\r",ch);
        return;
    }

    if ( !str_cmp(arg1,"load") )
    {
        load_changes( );
        save_changes();
        send_to_char("Changes Loaded.\n\r",ch);
        return;
    }

    if ( !str_cmp(arg1,"save") )
    {
        save_changes( );
        send_to_char("Changes Saved.\n\r",ch);
        return;
    }

    if ( !str_cmp(arg1, "delete"))
    {
        int num;

        if ( NULLSTR(arg2) || !is_number( arg2 ) )
        {
            send_to_char("For chedit delete, you must provide a change number.\n\r",ch);
            send_to_char("Syntax: chedit delete (change number)\n\r",ch);
            return;
        }

        num = atoi( arg2 );
        if ( num < 0 || num > maxChanges )
        {
            sprintf( buf, "Valid changes are from 0 to %d.\n\r", maxChanges );
            send_to_char( buf, ch );
            return;
        }
        delete_change( num );
        send_to_char("Change deleted.\n\r",ch);
        save_changes();
        load_changes();
        return;
    }

    return;
}

static void AddSpaces( char **ppszText, int iNumber )
{
    int iLoop;

    for ( iLoop = 0; iLoop < iNumber; iLoop++ )
    {
        *(*ppszText)++ = ' ';
    }
}

char *change_justify( char *pszText, int iAlignment )
{
    static char s_szResult[4096];
    char *      pszResult = &s_szResult[0];
    char        szStore[4096];
    int         iMax;
    int         iLength = iAlignment-1;
    int         iLoop = 0;

    if ( strlen( pszText ) < 10 )
    {
        strcpy( s_szResult, "BUG: Justified string cannot be less than 10 characters long." );
        return( &s_szResult[0] );
    }

    while ( *pszText == ' ' ) pszText++;

    szStore[iLoop++] = *pszText++;

    if ( szStore[iLoop-1] >= 'a' && szStore[iLoop-1] <= 'z' )
        szStore[iLoop-1] = UPPER( szStore[iLoop] );

    /* The first loop goes through the string, copying it into szStore. The
     * string is formatted to remove all newlines, capitalise new sentences,
     * remove excess white spaces and ensure that full stops, commas and
     * exclaimation marks are all followed by two white spaces.
     */
    while ( *pszText )
    {
        switch ( *pszText )
        {
            default:
                szStore[iLoop++] = *pszText++;
                break;
            case ' ':
                if ( *(pszText+1) != ' ' )
                {
                    /* Store the character */
                    szStore[iLoop++] = *pszText;
                }
                pszText++;
                break;
            case '.': case '?': case '!':
                szStore[iLoop++] = *pszText++;
                switch ( *pszText )
                {
                    default:
                        szStore[iLoop++] = ' ';
                        szStore[iLoop++] = ' ';
                        /* Discard all leading spaces */
                        while ( *pszText == ' ' ) pszText++;
                        /* Store the character */
                        szStore[iLoop++] = *pszText++;
                        if ( szStore[iLoop-1] >= 'a' && szStore[iLoop-1] <= 'z' )
                            szStore[iLoop-1] &= ~32;
                        break;
                    case '.': case '?': case '!':
                        break;
                }
                break;
            case ',':
                /* Store the character */
                szStore[iLoop++] = *pszText++;
                /* Discard all leading spaces */
                while ( *pszText == ' ' ) pszText++;
                /* Commas shall be followed by one space */
                szStore[iLoop++] = ' ';
                break;
            case '$':
                szStore[iLoop++] = *pszText++;
                while ( *pszText == ' ' ) pszText++;
                break;
            case '\n': case '\r':
                pszText++;
                break;
        }
    }

    szStore[iLoop] = '\0';

    /* Initialise iMax to the size of szStore */
    iMax = strlen( szStore );

    /* The second loop goes through the string, inserting newlines at every
     * appropriate point.
     */
    while ( iLength < iMax )
    {
        /* Go backwards through the current line searching for a space */
        while ( szStore[iLength] != ' ' && iLength > 1 )
            iLength--;

        if ( szStore[iLength] == ' ' )
        {
            szStore[iLength] = '\n';

            iLength += iAlignment;
        }
        else
            break;
    }

    /* Reset the counter */
    iLoop = 0;

    /* The third and final loop goes through the string, making sure that there
     * is a \r (return to beginning of line) following every newline, with no
     * white spaces at the beginning of a particular line of text.
     */
    while ( iLoop < iMax )
    {
        /* Store the character */
        *pszResult++ = szStore[iLoop];
        switch ( szStore[iLoop] )
        {
            default:
                break;
            case '\n':
                *pszResult++ = '\r';
                while ( szStore[iLoop+1] == ' ' ) iLoop++;
                /* Add spaces to the front of the line as appropriate */
                AddSpaces( &pszResult, 25 );
                break;
        }
        iLoop++;
    }

    *pszResult++ = '\0';

    return( &s_szResult[0] );
}

int num_changes( void )
{
    char *test;
    int   today;
    int   i;

    i = 0;
    test = current_date();
    today = 0;

    for ( i = 0; i < maxChanges; i++)
        if (!str_cmp(test,changes_table[i].date))
            today++;

    return today;
}

void do_changes(CHAR_DATA *ch, char *argument)
{
    char  arg[MAX_INPUT_LENGTH];
    char  buf[MSL];
    char *test;
    int   today;
    int   i = 0;
    int totalpages = 0;

    one_argument( argument, arg );
    
    if (IS_NPC(ch))
        return;
	
    test = current_date();
    today = 0;

    for ( i = 0; i < maxChanges; i++)
        if (!str_cmp(test,changes_table[i].date))
            today++;


    send_to_char("@@e------------------------------------------------------------------------------@@w\n\r", ch );
    send_to_char("@@WNo.  Coder        Date        Change\n\r",ch );
    send_to_char("@@e------------------------------------------------------------------------------@@w\n\r", ch );

    totalpages = 1 + ( totChanges / 10 );

    if ( !is_number( arg ) && strcmp(arg, "") )
    {
        sprintf(buf,"@@wType changes @@d[@@y1@@d]@@w through @@d[@@y%d@@d]@@w to view the changes.@@w\n\r"
            "@@e------------------------------------------------------------------------------@@w\n\r", totalpages);
        send_to_char(buf, ch);
        return;
    }

    if ( totalpages < 1 )
        totalpages = 1;

    if ( is_number( arg ) || !strcmp(arg, "") )
    {
        int page;
        int number;

        if (!strcmp(arg, ""))
            page = totalpages;

        else
            page = atoi(arg);
            number = page * 10;
            
            if ( page < 0 || page > totalpages )
            {
                sprintf( buf, "@@ePage must be between 1 and %d!@@N\n\r", totalpages );
                send_to_char(buf, ch);
                return;
            }
            
        for (i = 0; i < totChanges; i++)
        {
            if ( ( i > (number - 11) ) && ( i < number ) && ( i < totChanges ) )
            {
                sprintf(buf,"@@d[@@y%2d@@d]@@N @@r%-9s @@a*%-6s  @@W%-55s@@N\n\r", ( i + 1 ), changes_table[i].coder, changes_table[i].date, changes_table[i].change );
                send_to_char(buf, ch);
            }
        }
    }
    send_to_char("@@e------------------------------------------------------------------------------@@w\n\r", ch );
    sprintf(buf, "@@wThere are a total of @@d[@@y %3d @@d]@@w changes in the database.@@w\n\r", maxChanges);
    send_to_char( buf, ch );
    send_to_char("@@e------------------------------------------------------------------------------@@w\n\r", ch );
    sprintf(buf, "@@wThere are a total of @@d[@@y %3d @@d]@@w new changes that have been added today.@@w\n\r", today);
    send_to_char( buf, ch );
    send_to_char("@@e------------------------------------------------------------------------------@@w\n\r", ch );
    sprintf(buf, "@@wThere are a total of @@d[@@y %3d @@d]@@w pages of changes to view with '@@Gchanges #@@N'.@@w\n\r",
        totalpages);
    send_to_char( buf, ch );
    send_to_char("@@e------------------------------------------------------------------------------@@w\n\r", ch );
    return;
}

void load_logs(void)
{
    FILE *fp;
    int i;

    totLogs = 0;
    if ( !(fp = fopen( LOG_FILE, "r")) )
    {
        bug( "Could not open Log File for reading.", 0 );
        return;
    }

    fscanf( fp, "%d\n", &maxLogs );

    /* Use malloc so we can realloc later on */
    log_table = malloc( sizeof( LOG_DATA) * (maxLogs+1) );

    for( i = 0; i < maxLogs; i++ )
    {
        totLogs++;
        log_table[i].reporter = fread_string( fp );
        log_table[i].type = fread_string( fp );
        log_table[i].date = fread_string( fp );
        log_table[i].log = fread_string( fp );
    }
    log_table[maxLogs].reporter = str_dup("");
    fclose(fp);
    return;                                                 /* just return */
}

void save_logs(void)
{
    FILE *fp;
    int i;

    if ( !(fp = fopen( LOG_FILE,"w")) )
    {
        perror( LOG_FILE );
        return;
    }

    fprintf( fp, "%d\n", maxLogs );
    for( i = 0; i < maxLogs; i++ )
    {
        fprintf (fp, "%s~\n", log_table[i].reporter);
        fprintf (fp, "%s~\n", log_table[i].type);
        fprintf (fp, "%s~\n", log_table[i].date);
        fprintf (fp, "%s~\n", log_table[i].log );
        fprintf( fp, "\n" );
    }

    fclose(fp);
    return;
}

void do_log( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg1 );
        LOG_DATA * new_table;

        if ( IS_NPC( ch ) )
            return;

        if ( arg1[0] == '\0' || argument[0] == '\0' )
        {
            send_to_char( "Syntax: log [bug/typo/idea] [string]\n\r", ch );
            send_to_char( "Type 'logs' to view the list.\n\r", ch );
            return;
        }
        
        if ( !str_cmp( arg1, "bug" ) || !str_cmp( arg1, "typo" ) || !str_cmp( arg1, "idea" ) )
        {
            maxLogs++;
            new_table = realloc( log_table, sizeof( LOG_DATA ) *(maxLogs+1) );

            if (!new_table)                                         /* realloc failed */
            {
                send_to_char ("Memory allocation failed. Brace for impact.\n\r",ch);
                return;
            }

            log_table = new_table;
        
            log_table[maxLogs-1].reporter  = str_dup( ch->name );
            log_table[maxLogs-1].type      = str_dup( arg1 );
            log_table[maxLogs-1].date      = str_dup( current_date());
            log_table[maxLogs-1].log       = str_dup( argument );

            save_logs();
            load_logs();
        
            send_to_char("Thank you for posting this log, we'll look into it as soon as possible.\r\n", ch );
            return;
        }

        else
        {
            send_to_char( "Syntax: log [bug/typo/idea] [string]\n\r", ch );
            send_to_char( "Type 'logs' to view the list.\n\r", ch );
            return;
        }
}

void do_logs( CHAR_DATA *ch, char *argument )
{
    char bug_buf[MSL], idea_buf[MSL], typo_buf[MSL], bugs_buf[MSL];
    char *this_list;
    int i = 0;
    bool validcmd=FALSE;
    /* Empty the three buffers */
    *bug_buf = '\0';
    *idea_buf = '\0';
    *typo_buf = '\0';

    /* Populate the 3 lists */
    for (i = 0; i < totLogs; i++)
    {
        if ( !str_cmp(log_table[i].type, "bug") )  
            this_list = bug_buf;
        else if ( !str_cmp(log_table[i].type, "idea") ) 
            this_list = idea_buf;
        else if ( !str_cmp(log_table[i].type, "typo") ) 
            this_list = typo_buf;
        else
        {
            sprintf(bugs_buf, "Log type not recognised (%s) at log line %d", log_table[i].type, i);
            monitor_chan( NULL, bugs_buf, MONITOR_DEBUG);
            return;
        }

        if( IS_IMMORTAL(ch))
        {
            sprintf(this_list, "%s@@d[@@y%2d@@d]@@w @@r%-9s @@e%c%-4s @@a*%-6s  @@W%-55s@@N\r\n", this_list, ( i + 1 ), log_table[i].reporter, UPPER(*log_table[i].type), log_table[i].type + 1, log_table[i].date, log_table[i].log );
        }
        else
        {
            sprintf(this_list, "%s@@d[@@y%2d@@d]@@w @@e%c%-4s @@a*%-6s  @@W%-55s@@N\r\n", this_list, ( i + 1 ), UPPER(*log_table[i].type), log_table[i].type + 1, log_table[i].date, log_table[i].log );    
        }
    }

    if ( !str_cmp(argument, "bug") || !str_cmp(argument, "all") )
    {
        validcmd = TRUE;
        
        if (*bug_buf)
        {
            send_to_char("\r\n@@e------------------------@@d[@@WBug List@@d]@@N\r\n", ch);
            send_to_char(bug_buf, ch);
        }
        
        else
        send_to_char("\r\nNo bugs logged.\r\n", ch);
    }
    
    if ( !str_cmp(argument, "idea") || !str_cmp(argument, "all") )
    {
        validcmd = TRUE;

        if (*idea_buf)
        {
            send_to_char("\r\n@@e-----------------------@@d[@@WIdea List@@d]@@N\r\n", ch);
            send_to_char(idea_buf, ch);
        }
        
        else
            send_to_char("\r\nNo ideas logged.\r\n", ch);
    }
    
    if ( !str_cmp(argument, "typo") || !str_cmp(argument, "all") )
    {
        validcmd = TRUE;
        
        if (*typo_buf)
        {
            send_to_char("\r\n@@e-----------------------@@d[@@WTypo List@@d]@@N\r\n", ch);
            send_to_char(typo_buf, ch);
        }
        
        else
            send_to_char("\r\nNo typos logged.\r\n", ch);
    }
    
    if (!validcmd)
    {
        send_to_char( "Syntax: logs [bug/typo/idea/all]\r\nType 'log' if you want to log a bug, typo or idea.\r\n", ch);
        return;
    }
}
