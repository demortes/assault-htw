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
#include <ctype.h>
/* For forks etc. */
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#ifndef  DEC_ACK_H
#include "ack.h"
#include "tables.h"
#endif

#ifndef DEC_STRFUNS_H
#include "strfuns.h"
#endif

void pre_parse( char * list, char * victimname, char * containername, char * things )
{
    char arg1[MSL];
    char container_name[MSL];
    char one_object[MSL];
    char holdbuf[MSL];
    char victim_name[MSL];
    char object_list[MSL];
    char * argument = list;
    container_name[0] = '\0';
    victim_name[0] = '\0';
    object_list[0] = '\0';
    for ( ; ; )
    {
        argument = one_argument( argument, arg1 );
        if ( arg1[0] == '\0' )
            break;
        if (  ( !str_cmp( "from", arg1 ) )
            || ( !str_cmp( "in", arg1 ) )  )
        {
            argument = one_argument( argument, container_name );
        }
        else if ( !str_cmp( "to", arg1 ) )
        {
            argument = one_argument( argument, victim_name );
        }
        else if ( object_list[0] != '\0' )
        {
            safe_strcat(MSL,victim_name,arg1);
        }
        else
        {
            if ( is_number( arg1 ) )
            {
                argument = one_argument( argument, one_object );
                sprintf( holdbuf, "%s %s ", arg1, one_object );
                safe_strcat( MSL, object_list, holdbuf );
            }
            else
            {
                sprintf( holdbuf, "1 %s ", arg1 );
                safe_strcat( MSL, object_list, holdbuf );
            }
        }
    }
    strcpy( victimname , victim_name );
    strcpy( things,object_list);
    strcpy( containername , container_name);
    return;
}

/*
 * Return true if an argument is completely numeric.
 */
bool is_number( char *arg )
{
    if ( *arg == '\0' )
        return FALSE;

    if ( *arg == '+' || *arg == '-' )
        arg++;

    for ( ; *arg != '\0'; arg++ )
    {
        if ( !isdigit(*arg) )
            return FALSE;
    }

    return TRUE;
}

bool is_name( const char *str, char *namelist )
{
    char name[MAX_INPUT_LENGTH];

    for ( ; ; )
    {
        namelist = one_argument( namelist, name );
        if ( name[0] == '\0' )
            return FALSE;
        if ( !str_prefix( str, name ) )
            return TRUE;
    }
}

void safe_strcat(int max_len,char * dest,char * source)
{
    int a;
    char c;
    char * orig_dest;
    char * orig_source;

    if (dest == NULL && source == NULL)
        return;

    if (dest == NULL)
    {
        fprintf(stderr,"safe_strcat: Null dest string for source : %s\n",source);
        return;
    }

    if (source == NULL)
    {
        fprintf(stderr,"safe_strcat: NULL source for dest : %s\n",dest);
        return;
    }

    orig_dest=dest;
    orig_source=source;

    while (*dest !='\0')                                    /* Check to see if dest is already over limit. */
        dest++;

    a=dest-orig_dest;
    if ( a > max_len )
    {
        fprintf(stderr,"WARNING: dest string already too long:\nsource: %s\ndest: %s\n",source,orig_dest);
        return;
    }

    if ( a == max_len )
    {
        fprintf(stderr,"safe_strcat: string too long, source is : %s\n",orig_source);
        return;
    }

    while ( (c = *(source++)) != 0 )
    {
        *(dest++)=c;
        a++;
        if ( a == max_len )
        {
            *(--dest)='\0';
            fprintf(stderr,"safe_strcat: string too long, source is : %s\n",orig_source);
            return;
        }
    }

    *dest='\0';
    return;
}

char * space_pad( const char * str, sh_int final_size )
{
    sh_int space_pad = my_strlen( str );
    static char padbuf[MSL];

    sprintf( padbuf, "%s", str );
    for ( ; space_pad != final_size ; space_pad++ )
        safe_strcat( MSL, padbuf, " " );
    return padbuf;
}

/*
 * Removes the tildes from a string.
 * Used for player-entered strings that go into disk files.
 */
void smash_tilde( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
        if ( *str == '~' )
            *str = '-';
    }

    return;
}

void smash_system( char *str )
{
    for ( ; *str != '\0'; str++ )
    {
        if ( *str == '%' )
            *str = '*';
    }

    return;
}

char *smash_swear( char *str )
{
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
    {
        if (  ( str[i]   == 'f' || str[i]   == 'F'  )
            && ( str[i+1] == 'u' || str[i+1] == 'U'  )
            && ( str[i+2] == 'c' || str[i+2] == 'C'  )
            && ( str[i+3] == 'k' || str[i+3] == 'K'  )  )
        {
            str[i] = '*';
            str[i +1] = '*';
            str[i +2] = '*';
            str[i +3] = '*';
            i += 3;
        }
        /* if (  ( str[i]   == 's' || str[i]   == 'S'  )
            && ( str[i+1] == 'h' || str[i+1] == 'H'  )
            && ( str[i+2] == 'i' || str[i+2] == 'I'  )
            && ( str[i+3] == 't' || str[i+3] == 'T'  )  )
        {
            str[i] = '*';
            str[i +1] = '*';
            str[i +2] = '*';
            str[i +3] = '*';
            i += 3;
        } */
        if (  ( str[i]   == 'c' || str[i]   == 'C'  )
            && ( str[i+1] == 'u' || str[i+1] == 'U'  )
            && ( str[i+2] == 'n' || str[i+2] == 'N'  )
            && ( str[i+3] == 't' || str[i+3] == 'T'  )  )
        {
            str[i] = '*';
            str[i +1] = '*';
            str[i +2] = '*';
            str[i +3] = '*';
            i += 3;
        }
        if (  ( str[i]   == 'p' || str[i]   == 'P'  )
            && ( str[i+1] == 'e' || str[i+1] == 'E'  )
            && ( str[i+2] == 'n' || str[i+2] == 'N'  )
            && ( str[i+3] == 'i' || str[i+3] == 'I'  )
            && ( str[i+4] == 's' || str[i+4] == 'S'  )  )
        {
            str[i] = '*';
            str[i +1] = '*';
            str[i +2] = '*';
            str[i +3] = '*';
            str[i +4] = '*';
            i += 4;
        }
        /* if (  ( str[i]   == 'd' || str[i]   == 'D'  )
            && ( str[i+1] == 'i' || str[i+1] == 'I'  )
            && ( str[i+2] == 'c' || str[i+2] == 'C'  )
            && ( str[i+3] == 'k' || str[i+3] == 'K'  )  )
        {
            str[i] = '*';
            str[i +1] = '*';
            str[i +2] = '*';
            str[i +3] = '*';
            i += 3;
        } */
        if (  ( str[i]   == 'c' || str[i]   == 'C'  )
            && ( str[i+1] == 'o' || str[i+1] == 'O'  )
            && ( str[i+2] == 'c' || str[i+2] == 'C'  )
            && ( str[i+3] == 'k' || str[i+3] == 'K'  )  )
        {
            str[i] = '*';
            str[i +1] = '*';
            str[i +2] = '*';
            str[i +3] = '*';
            i += 3;
        }
        if (  ( str[i]   == 'f' || str[i]   == 'F'  )
            && ( str[i+1] == 'a' || str[i+1] == 'A'  )
            && ( str[i+2] == 'g' || str[i+2] == 'G'  ) )
        {
            str[i] = '*';
            str[i +1] = '*';
            str[i +2] = '*';
            i += 2;
        }
    }
    return str;
}

/*
 * Compare strings, case insensitive.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp( const char *astr, const char *bstr )
{
    int count = 0;
    bool notrunc = FALSE;
    if ( astr == NULL )
    {
        /* bug( "Str_cmp: null astr.", 0 );   */
        return TRUE;
    }

    if ( bstr == NULL )
    {
        /*	bug( "Str_cmp: null bstr.", 0 );  */
        return TRUE;
    }
    if ( *astr == '^' )
    {
        notrunc = TRUE;
        astr++;
    }
    if ( *bstr == '^' )
    {
        notrunc = TRUE;
        bstr++;
    }
    if ( astr == NULL )
    {
        /* bug( "Str_cmp: null astr.", 0 );   */
        return TRUE;
    }

    if ( bstr == NULL )
    {
        /*	bug( "Str_cmp: null bstr.", 0 );  */
        return TRUE;
    }

    for ( ; *astr || *bstr; astr++, bstr++ )
    {
        if ( !notrunc )
            if ( *astr == '*' && count )
                return FALSE;

        if ( LOWER(*astr) != LOWER(*bstr) )
            return TRUE;
        count++;
    }

    return FALSE;
}

/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix( const char *astr, const char *bstr )
{
    if ( astr == NULL )
    {
        bug( "Strn_cmp: null astr.", 0 );
        return TRUE;
    }

    if ( bstr == NULL )
    {
        bug( "Strn_cmp: null bstr.", 0 );
        return TRUE;
    }

    for ( ; *astr; astr++, bstr++ )
    {
        if ( LOWER(*astr) != LOWER(*bstr) )
            return TRUE;
    }

    return FALSE;
}

/*
 * Compare strings, case insensitive, for match anywhere.
 * Returns TRUE is astr not part of bstr.
 *   (compatibility with historical functions).
 */
bool str_infix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;
    int ichar;
    char c0;

    if ( ( c0 = LOWER(astr[0]) ) == '\0' )
        return FALSE;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);

    for ( ichar = 0; ichar <= sstr2 - sstr1; ichar++ )
    {
        if ( c0 == LOWER(bstr[ichar]) && !str_prefix( astr, bstr + ichar ) )
            return FALSE;
    }

    return TRUE;
}

/*
 * Compare strings, case insensitive, for suffix matching.
 * Return TRUE if astr not a suffix of bstr
 *   (compatibility with historical functions).
 */
bool str_suffix( const char *astr, const char *bstr )
{
    int sstr1;
    int sstr2;

    sstr1 = strlen(astr);
    sstr2 = strlen(bstr);
    if ( sstr1 <= sstr2 && !str_cmp( astr, bstr + sstr2 - sstr1 ) )
        return FALSE;
    else
        return TRUE;
}

/*
 * Returns an initial-capped string.
 */
#if 0
char *capitalize( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];
    int i;

    for ( i = 0; str[i] != '\0'; i++ )
    {
        if (  ( str[i]   == '@'   )
            && ( str[i+1] == '@'   )
            && ( str[i+2] != '\0'  )  )
        {
            strcap[i] = str[i];
            strcap[i +1] = str[i+1];
            strcap[i +2] = str[i+2];
            i += 2;
        }
        else
            strcap[i] = LOWER(str[i]);
    }

    strcap[i] = '\0';
    for ( i = 0; strcap[i] != '\0' && !isalpha( strcap[i] ); i++ );
    if (  ( i > 0 )
        && ( strcap[i] != '\0' )  )
        i++;
    if ( strcap[i] != '\0' )
        strcap[i] = UPPER(strcap[i]);
    return strcap;
}
#endif
/* Capitalize function by KaVir: 3th December 1997.
 *
 * Pass in a pointer to the string to capitalize.  The function will return
 * a pointer to the corrected string, however for safety purposes the original
 * pointer will also be set to point to the new string.  Thus if you do:
 *    char *normal_string = "a piece of text";
 *    char *cap_string = capitalize( normal_string );
 * Then both cap_string and normal_string will contain the capitalized string.
 *
 * The precise rules of capitalization are as follows:
 *    1) The first non-colorcode alphabetic character will be uppercase.
 *    2) Any alphabetic character following two or more @'s will be unchanged.
 *    3) Any other alphabetic character will be set to lowercase.
 *    4) Any other non-alphabetic characters will be ignored.
 */
char *capitalize( const char *str )
{
    static char strcap[MAX_STRING_LENGTH];                  /* Permanent store */
    /*    char *oldstr = str;  */
    int i = 0;                                              /* Position in strcap */
    int count = 0;                                          /* Number of previous '@'s encountered */
    bool first_letter = TRUE;                               /* You will UPPER the first letter you find */
    bool ignore_next  = FALSE;                              /* Ignore the case of the next letter you find */

    /* If this occurs, then you have a bug elsewhere */
    if ( ( strcap[i] = *str ) == '\0' )
        return NULL;

    do                                                      /* Begin looping through the string, checking each character */
    {
        /* Should be more efficient than all those 'if' statements ;) */
        switch ( strcap[i] )
        {
            default:                                        /* Non-alphabetic letters and not '@' */
                ignore_next = FALSE;                        /* Not a color code */
                count = 0;
                break;
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
            case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
            case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
            case 's': case 't': case 'u': case 'v': case 'w': case 'x':
            case 'y': case 'z':                             /* Any lowercase letter */
                if ( ignore_next )
                    ignore_next = FALSE;                    /* We ignore the case of the this letter */
                else if ( first_letter )
                {
                    first_letter = FALSE;                   /* Reset the flag */
                    strcap[i] = UPPER(strcap[i]);           /* We set this letter to uppercase */
                }
                count = 0;
                break;
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
            case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
            case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
            case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
            case 'Y': case 'Z':                             /* Any uppercase letter */
                /* Then its the first letter in the string */
                if ( ignore_next )
                    ignore_next = FALSE;                    /* We ignore the case of the this letter */
                else if ( first_letter )
                    first_letter = FALSE;                   /* We ignore the case of the this letter */
                else
                    strcap[i] = LOWER(strcap[i]);           /* Set this letter to lowercase */
                count = 0;
                break;
            case '@':
                if ( ++count >= 2 )                         /* If there are two or more '@@'s in a row */
                    ignore_next = TRUE;                     /* Set the flag to ignore the next letter */
                break;
        }
    } while ( ( strcap[++i] = *++str ) != '\0' );           /* loop until end of string */

    i = 0;                                                  /* str = oldstr;  Reset variables */
    #if 0
    /* Copy strcap back into the old string */
    while ( ( *str++ = strcap[i++] ) != '\0' )
        ;

    return ( oldstr );                                      /* Return pointer to start of old string */
    #endif
    return ( strcap );
}

/*
 * We need a replacement for strlen() which will take the color
 * codes into account when reporting a string's length.
 * -- Stephen
 */

int my_strlen( const char *text )
{
    char c;
    int i;
    int status;
    int length;
    int strlen_size;

    status = 0;
    length = 0;
    strlen_size = strlen( text );

    for ( i = 0; i < strlen_size; i++ )
    {

        c = text[i];
        length++;

        switch( status )
        {
            case 0:
            case 1:
                if ( c == '@' )
                    status++;
                else
                    status = 0;
                break;
            case 2:
                if ( c == '@' )
                    length -= 2;
                else
                    length -= 3;                            /* Subtract for '@@x' */
                status = 0;
                break;
        }
    }
    return( length );
}

int nocol_strlen( const char *text )
{
    char c;
    int i;
    int status;
    int length;
    int strlen_size;

    status = 0;
    length = 0;
    strlen_size = strlen( text );

    for ( i = 0; i < strlen_size; i++ )
    {

        c = text[i];
        length++;

        switch( status )
        {
            case 0:
            case 1:
                if ( c == '@' )
                    status++;
                else
                    status = 0;
                break;
            case 2:
                length -= 3;                                /* Subtract for '@@x' */
                status=0;
                break;
        }
    }
    return( length );
}

int ccode_len( const char *text, sh_int desired )
{
    char c;
    int i;
    int status;
    int length;
    int strlen_size;

    status = 0;
    length = 0;
    strlen_size = strlen( text );

    for ( i = 0; i < strlen_size; i++ )
    {

        c = text[i];

        switch( status )
        {
            case 0:
            case 1:
                if ( c == '@' )
                    status++;
                else
                    status = 0;
                break;
            case 2:
                if ( c != '@' )
                    length += 3;                            /* Subtract for '@@x' */
                status=0;
                break;
        }
    }
    return( length + desired );
}

/* END NOTE */

/* A Function to "center" text, and return a string with
 * the required amount of white space either side of the
 * original text.  --Stephen
 */

char *center_text( char *text, int width )
{
    /* This could do with a LOT of improvement sometime! */
    /* Improvements done.. -- Altrag */
    static char foo[MAX_STRING_LENGTH];
    int len, diff;

    /* Don't bother with strings too long.. */
    if ( (len = my_strlen(text)) >= width )
        return text;

    diff = strlen(text)-len;

    /* Add the spaces and terminate string */
    memset(foo, ' ', width+diff);
    foo[width+diff] = '\0';

    /* Copy old string over */
    memcpy(&foo[(width-len)/2], text, len+diff);

    return foo;
}

char * str_mod( char * mod_string,  char *argument )
{

    char       arg1 [ MAX_INPUT_LENGTH  ];
    char       buf  [ MAX_STRING_LENGTH ];
    char      *buf2;
    char      *buf3 = NULL;
    char      *word;
    char        temp[MSL];
    int        i = 0;
    bool        multiple = FALSE;

    if ( !str_cmp( argument, "" ) )
    {
        sprintf( bug_buf, "Unknown reason for return, argument is -%s-", argument );
        monitor_chan( NULL, bug_buf, MONITOR_DEBUG );
        return mod_string;

    }

    if ( argument[0] == '+' || argument[0] == '-' )
    {
        buf[0] = '\0';
        smash_tilde( argument );
        if ( argument[0] == '+' )
        {
            argument++;

            if ( mod_string )
                safe_strcat( MSL, buf, mod_string );
            while ( isspace( *argument ) )
                argument++;
            if ( !str_infix( argument, mod_string ) )
            {
                return mod_string;
            }
            safe_strcat( MSL, buf, argument );
            safe_strcat( MSL, buf, " " );
        }
        if ( argument[0] == '-' )
        {
            argument++;
            if ( argument[0] == '\'' )
                multiple = TRUE;
            one_argument( argument, arg1 );

            if ( multiple )
            {
                sprintf( temp, "\'%s\'", arg1 );
                sprintf( arg1, "%s", temp );
            }

            if ( arg1 )
            {
                buf2 = str_dup( mod_string );
                buf3 = buf2;
                if ( (word = strstr( buf2, arg1 ) ) == NULL )
                {
                    free_string( buf2 );
                    return mod_string;
                }
                else
                {
                    while ( buf2 != word )
                        buf[i++] = *(buf2++);
                    while (  ( !isspace ( *(buf2++) ) )
                        || (  ( multiple )
                        && (  ( buf2[0] != '\'' )
                        && ( buf2[0] != '\0' ) ) )  );
                    buf[i] = '\0';
                    safe_strcat ( MSL, buf, buf2 );
                }
            }

        }

        free_string( buf3 );
        word = buf2 = buf3 = NULL;
        free_string( mod_string );
        mod_string = str_dup( buf );
    }
    return mod_string;
}

void rand_arg( char *argument, char *output )
{
    char  temp[MSL];
    sh_int        counter = 0;
    argument = one_argument( argument, temp );
    while ( temp[0] )
    {
        if (number_range(0, counter)==0)
            strcpy (output, temp);
        counter++;
        argument = one_argument( argument, temp );
    }
}

/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument( char *argument, char *arg )
{
    char *pdot;
    int number;

    for ( pdot = argument; *pdot != '\0'; pdot++ )
    {
        if ( *pdot == '.' )
        {
            *pdot = '\0';
            number = atoi( argument );
            *pdot = '.';
            strcpy( arg, pdot+1 );
            return number;
        }
    }

    strcpy( arg, argument );
    return 1;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_argument( char *argument, char *arg_first )
{
    char cEnd;
    //    char buf[MSL];

    if ( argument == NULL )
        return NULL;
    while ( isspace(*argument) )
        argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
        cEnd = *argument++;

    while ( *argument != '\0' )
    {
        if ( *argument == cEnd )
        {
            argument++;
            break;
        }
        *arg_first = LOWER(*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
        argument++;

    return argument;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
char *one_word( char *argument, char *arg_first )
{
    char cEnd;
    if ( argument == NULL )
        return NULL;
    while ( isspace(*argument) )
        argument++;

    cEnd = ' ';
    if ( *argument == '\'' || *argument == '"' )
        cEnd = *argument++;

    while ( *argument != '\0' )
    {
        if ( *argument == cEnd )
        {
            argument++;
            break;
        }
        *arg_first = (*argument);
        arg_first++;
        argument++;
    }
    *arg_first = '\0';

    while ( isspace(*argument) )
        argument++;

    return argument;
}

char *two_args( char *argument, char *arg_first, char *arg_second )
{
    char *temp_arg = argument;

    temp_arg = one_argument( temp_arg, arg_first );
    temp_arg = one_argument( temp_arg, arg_second );
    return temp_arg;
}

char *strip_out(const char *orig, const char *strip)
{
    static char buf[MAX_STRING_LENGTH];
    char *i, *b = buf;
    int slen;

    if (!orig || !strip)
    {
        /*    log_f("strip_out called with NULL param");  */
        return "!!OUT!!";
    }
    slen = strlen(strip);
    for (i = strstr(orig, strip); i; i = strstr(orig, strip))
    {
        strncpy(b, orig, (i-orig));
        b += i-orig;
        orig = i+slen;
    }
    strcpy(b, orig);
    return buf;
}

/* this code donated by Altrag */
char *strip_color(const char *orig, const char *strip)
{
    static char buf[MAX_STRING_LENGTH];
    char *i, *b = buf;
    int slen;

    if (!orig || !strip)
    {
        /*    log_f("strip_out called with NULL param");  */
        return "!!OUT!!";
    }
    slen = strlen(strip)+1;
    for (i = strstr(orig, strip); i; i = strstr(orig, strip))
    {
        strncpy(b, orig, (i-orig));
        b += i-orig;
        orig = i+slen;
    }
    strcpy(b, orig);
    return buf;
}

bool list_in_list( char * first_list, char * second_list )
{
    char check_word[MAX_INPUT_LENGTH];
    char against_word[MSL];
    char *      checklist;
    char *      againstlist;

    checklist =  first_list ;
    againstlist = second_list;

    for ( ; ; )
    {
        checklist = one_argument( checklist, check_word );
        againstlist = second_list;
        if ( check_word[0] == '\0' )
        {
            return FALSE;
        }
        for ( ; ; )
        {
            againstlist = one_argument( againstlist, against_word );
            if ( against_word[0] == '\0' )
                break;
            if ( !str_cmp( check_word, against_word ) )
            {
                return TRUE;
            }

        }
    }
}

/*
 * Return ascii name of an item type.
 */
char *item_type_name( OBJ_DATA *obj )
{
    char log[MAX_STRING_LENGTH];

    switch ( obj->item_type )
    {
        case ITEM_LIGHT:            return "light";
        case ITEM_AMMO:             return "ammo";
        case ITEM_BOMB:             return "bomb";
        case ITEM_BLUEPRINT:        return "blueprint";
        case ITEM_SUIT:     return "suit";
        case ITEM_MEDPACK:          return "medpack";
        case ITEM_WEAPON:           return "weapon";
        case ITEM_DRONE:            return "drone";
        case ITEM_ARMOR:            return "armor";
        case ITEM_TELEPORTER:       return "teleporter";
        case ITEM_IMPLANT:          return "implant";
        case ITEM_BOARD:            return "bulletin board";
        case ITEM_INSTALLATION: return "installation";
        case ITEM_TOKEN:            return "token";
        case ITEM_MATERIAL:         return "material";
        case ITEM_FLAG:             return "flag";
        case ITEM_DART_BOARD:   return "dart board";
        case ITEM_ELEMENT:      return "element";
        case ITEM_CONTAINER:    return "container";
        case ITEM_WEAPON_UP:    return "weapon upgrade";
        case ITEM_PIECE:        return "piece";
        case ITEM_COMPUTER:     return "computer";
        case ITEM_LOCATOR:      return "item locator";
        case ITEM_SKILL_UP:     return "skill upgrade";
        case ITEM_PART:     return "vehicle part";
        case ITEM_DISK:     return "disk";
        case ITEM_TRASH:        return "trash";
        case ITEM_ASTEROID:     return "asteroid";
        case ITEM_VEHICLE_UP:   return "vehicle addon";
        case ITEM_TOOLKIT:      return "toolkit";
        case ITEM_SCAFFOLD:     return "scaffold";
        case ITEM_ORE:      return "ore";
        case ITEM_BIOTUNNEL:    return "Bio Tunnel";
        case ITEM_BATTERY:      return "battery";
    }

    sprintf( log, "Item_type_name: Object: %d.  Unknown Type: %d",
        obj->pIndexData->vnum, obj->item_type );
    monitor_chan( NULL, log, MONITOR_OBJ );
    bug( log, 0 );
    return "(unknown)";
}

char *item_type_desc( int type )
{
    switch ( type )
    {
        case ITEM_LIGHT:            return "(unknown)";
        case ITEM_AMMO:             return "Ammo can be LOADed into weapons, so they can fire after they run out.";
        case ITEM_BOMB:             return "Bombs are dangerous tools of destruction - Just SET them, ARM them, and RUN AWAY!";
        case ITEM_BLUEPRINT:        return "Blueprints are used to upgrade buildings. They can only upgrade buildings by 1 level at a time.";
        case ITEM_SUIT:     return "Suits are used with the ACTIVATE command, suits can have special functions such as teleportation.";
        case ITEM_MEDPACK:          return "Medpacks are used with the HEAL command, they can cure your wounds when you use them.";
        case ITEM_WEAPON:           return "Weapons are the basic attack items - Wield/wear one, then FIRE or SHOOT at your enemies.";
        case ITEM_DRONE:            return "Just drop them, and they'll do their job. Repair drones, for example, fix damaged buildings.";
        case ITEM_ARMOR:            return "Armor can protect you from hits - Each armor type protects against a different damage type.";
        case ITEM_TELEPORTER:       return "A Teleporter is used to take you back to one of your controlled buildings, in a certain radius.";
        case ITEM_IMPLANT:          return "Implants are given to Bio Lab players, and can be used to increase their stats (HP, carry ability...).";
        case ITEM_BOARD:            return "Boards are found in players' HQs and Government Centers (alliance boards). You can READ and WRITE messages on them!";
        case ITEM_INSTALLATION: return "You can INSTALL these into buildings, each giving a different effect.";
        case ITEM_TOKEN:            return "Tokens give players quest points when they are picked up.";
        case ITEM_MATERIAL:         return "Ranging from sticks to gold, these are the basic elements needed to build a structure.";
        case ITEM_FLAG:             return "Flags you get for destroying a player's base. You can save them at the lodge as trophies!";
        case ITEM_DART_BOARD:   return "You can play Darts in a hunting lodge, and this is what you play with.";
        case ITEM_ELEMENT:      return "Elements are items you can process using the Chemistry command to create new items.";
        case ITEM_CONTAINER:    return "Containers are what you put elements in before you can mix them.";
        case ITEM_WEAPON_UP:    return "Weapon Upgrades can be WINSTALLed into weapons to give them special effects.";
        case ITEM_PIECE:        return "Piece items can be combined together to create new items.";
        case ITEM_COMPUTER:     return "Computers can be used to upload viruses to enemy buildings and track players down.";
        case ITEM_LOCATOR:      return "An item locator will scan the surroundings for items, letting you know whether you're close to an enemy base.";
        case ITEM_SKILL_UP:     return "Skill Upgrades are used to instantly increase your skill at a certain action.";
        case ITEM_PART:     return "A Vehicle Part used to construct vehicles.";
        case ITEM_DISK:     return "A disk containing software such as password crackers or viruses.";
        case ITEM_TRASH:        return "Useless junk.";
        case ITEM_ASTEROID:     return "Asteroids can be mined for metals in space, and might hold some hidden items within.";
        case ITEM_VEHICLE_UP:   return "Vehicle addons add special features to vehicles (increased range, scanner, etc).";
        case ITEM_TOOLKIT:      return "Toolkits are used by engineers to upgrade vehicles.";
        case ITEM_SCAFFOLD:     return "Creates the layout for a new building.";
        case ITEM_ORE:      return "Research these to produce new items!";
        case ITEM_BIOTUNNEL:    return "Transport all the items in the room to a receiving tunnel.";
    }

    return "(unknown)";
}

/*
 * Return ascii name of extra flags vector.
 */
char *extra_bit_name( int extra_flags )
{
    static char buf[512];

    buf[0] = '\0';
    if ( extra_flags & ITEM_INVIS        ) safe_strcat( MAX_STRING_LENGTH,  buf, " invis"             );
    if ( extra_flags & ITEM_NODROP       ) safe_strcat( MAX_STRING_LENGTH,  buf, " nodrop"            );
    if ( extra_flags & ITEM_NOREMOVE     ) safe_strcat( MAX_STRING_LENGTH,  buf, " noremove"          );
    if ( extra_flags & ITEM_NOSAVE       ) safe_strcat( MAX_STRING_LENGTH,  buf, " nosave"            );
    if ( extra_flags & ITEM_RARE         ) safe_strcat( MAX_STRING_LENGTH,  buf, " rare"              );
    if ( extra_flags & ITEM_UNIQUE       ) safe_strcat( MAX_STRING_LENGTH,  buf, " unique"            );

    return ( buf[0] != '\0' ) ? buf+1 : "none";
}

char *material_name( int type )
{
    static char buf[512];
    buf[0] = '\0';

    if ( type == ITEM_IRON )        safe_strcat(MSL, buf, "Iron" );
    else if ( type == ITEM_SKIN )   safe_strcat(MSL, buf, "Skins" );
    else if ( type == ITEM_GOLD )   safe_strcat(MSL, buf, "Gold" );
    else if ( type == ITEM_SILVER ) safe_strcat(MSL, buf, "Silver" );
    else if ( type == ITEM_COPPER ) safe_strcat(MSL, buf, "Copper" );
    else if ( type == ITEM_ROCK )   safe_strcat(MSL, buf, "Rocks" );
    else if ( type == ITEM_STICK )  safe_strcat(MSL, buf, "Sticks" );
    else if ( type == ITEM_LOG )    safe_strcat(MSL, buf, "Logs" );
    else            safe_strcat(MSL, buf, "Unknown!" );

    return buf;
}

/* Thanks to FearItself of AvP Mud */
char *format_to_lines(char *src, int length)
{
    static char buf[MSL];
    char buf2[MSL];

    buf[0] = '\0';
    while (*src)
    {
        //		sprintf( buf2, "%-*.*s", length, length, src );
        sprintf(buf + strlen(buf), "| %-*.*s |\n\r", length, length, src);
        if (strlen(src) < length)
            break;
        src += length;
    }
    return buf;
}

int cstrlen_for_sprintf(const char *str, size_t n)
{
    int             length = n;

    char c, nc;

    if (!str)       return 0;
    if (n <= 0)     return 0;

    while ((c = *str) && (n > 0))
    {
        nc = *(++str);

        if (c == '@' && nc == '@')
        {
            ++str;
            ++length;
            ++length;

            if (*str != '@')
                ++length;                                   //      safety catch to prevent @\0
            else
                --n;
        }
        else
            --n;
    }

    //      Reached the max or c == '\0'
    //      We'll look for more color codes to skip if at the max...
    if (c)
    {
        while ((c = *(str++)))
        {
            nc = *str;

            if (c=='@' && nc=='@' && str[1] != '@')
            {
                ++str;
                length += 3;
            }
            else
                break;
        }
    }

    return length;
}
