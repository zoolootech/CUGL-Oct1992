
/* macros.c -- 11/27/88, d.c.oshel */

#include <stdarg.h>
#include "burlap.h"

extern FORM_RULES burlap[];

static char *token;
static char *slop;
static char *moreslop;


jmp_buf osiris;


static int crash_macro( char *msg,... )
{
    va_list arg_ptr;
    char buffer[128];

    va_start( arg_ptr, msg );
    vsprintf( buffer, msg, arg_ptr );
    va_end( arg_ptr );

    boxmsg( buffer );
	longjmp( osiris,-1 );
}


static int valid_number( char *p )
{
	static char test[] = "0123456789-.,$+";

	for ( ; *p; p++ )
	{
		if ( strchr( test, *p ) == NULL )
			return (0);
	}
	return (1);
}


static int numeric_args( char * a1, char * a2 )
{
	return ( valid_number( a1 ) && valid_number( a2 ) );
}


static long number( char *p )
{
	long n, sign;

	for ( n = 0L, sign = 1L; *p; p++ )
	{
		if ( *p == '-' )
			sign = -1L;
		else if ( isdigit(*p) )
		{
			n *= 10L;
			n += (long) (*p - '0');
		}
	}
	n *= sign;
	return ( n );	
}


/* load_and_compare() --

   this function determines N from the first field in q,
   then moves the contents of field N to token,
   then move the string argument which immediately follows in q to moreslop,
   then returns the result of case-insensitive comparison between the two
   */

static int load_and_compare( char *q, char *op )
{
	int k, j, n, len;
    char *p;

	memset(token,0,MACROSIZE);
	memset(moreslop,0,MACROSIZE);

    /* numeric argument refers to a burlap field */
	if (*q)
	{
		for( ; *q && !(isdigit(*q) || *q == '-'); q++ )
		;
		for ( n = 0; *q && (isdigit(*q) || *q == '-'); q++ )
		{
			if (*q == '-')
				;
			else
			{
				n *= 10;
				n += (*q - '0');
			}
		}

        /* index array on base 0, and do range check */
        n -= 1; 
		if ( n < 0 || n > ISAM_field_range )  /* 0 <= n <= NUMBER_OF_REAL_FIELDS */
            n = ISAM_field_range;

        len = burlap[n].len;
        if (len < 0)
            len = MAXVFLDLEN;
        p = *(burlap[n].fptr);
        memcpy( token, p, len );
        strip_blanks( token );
        strlwr(token);
	}

    /* string argument might be quoted */
	if (*q)
	{
		for( ++q; *q && *q == ' '; q++ )
		;
		if (*q)
		{
            if (*q == APOSTROPHE || *q == QUOTE)
			    k = *q++; /* delimiter */
            else
                k = EOL;

			for ( j = 0; *q && *q != k; q++, j++ )
				*(moreslop + j) = *q;
            strip_blanks( moreslop );
            strlwr( moreslop );
		}
	}
    
    return (strcmp( token, moreslop ));
}


/*======================  Recursive Macro Expansions  ======================*/

int expand_macros( char *buffer )
{
	char *p, *q;		/* RECURSIVE */
	char /* **r, *str, */ *tkn;

	static int i, arg1, arg2, sign;

	/* RECURSIVE, but macros expand by inserting in place, so the
	   receiving buffer is not adversely affected by nested macros, 
	   and there is NO undue stress on the stack!  There is no
	   error checking, but the working buffers are huge, so there is
	   no problem with almost any practical expansion.
	   
	   In the burlap version, all of the defined macros "expand" to 
       shorter, not longer, strings.
	   */


	if ( (p = strstr( buffer, "(" )) != NULL )
	{
		if ( strstr( p + 1, "(" ) != NULL )
		{
			expand_macros( p + 1 ); /* RECURSIVE */
		}

		q = p;	/* mark opening paren */

		i = 0;
		while (*p)
		{
			if (*p == '(')  /* skip over "quoted macros" embedded in arg */
				++i;
			else if (*p == ')')
				--i;

			if ( !i )
				break;
			else
				p++;
		}

		if ( *p != ')' )
			crash_macro("macro needs ')': %s", q );

		*q = '\0';  /* wipes out opening paren */
		*p = '\0';  /* wipes out closing paren and delimits macro */
		++p;        /* advance beyond macro to next portion of buffer */

		/* ===== Arithmetic and Logical Operators ===== */

		if ((i = *( q + 1 )) == '&' || 
				  i == '|' ||
				  i == '^'
				)
		{
			tkn = q + 1;
			for( q += 2; *q && !(isdigit(*q) || *q == '-'); q++ )
			;
			for ( sign = 1, arg1 = 0; *q && (isdigit(*q) || *q == '-'); q++ )
			{
				if (*q == '-')
					sign = -1;
				else
				{
					arg1 *= 10;
					arg1 += (*q - '0');
				}
			}
			arg1 *= sign;
			for( ++q; *q && !(isdigit(*q) || *q == '-'); q++ )
			;
			for ( sign = 1, arg2 = 0; *q && (isdigit(*q) || *q == '-'); q++ )
			{
				if (*q == '-')
					sign = -1;
				else
				{
					arg2 *= 10;
					arg2 += (*q - '0');
				}
			}
			arg2 *= sign;
			switch (i)
			{
			case '&': arg1 = (arg1 != 0)? 1: 0;
			          arg2 = (arg2 != 0)? 1: 0;
					  arg1 &= arg2;
					  break;
			case '|': arg1 = (arg1 != 0)? 1: 0;
			          arg2 = (arg2 != 0)? 1: 0;
					  arg1 |= arg2;
					  break;
			case '^': arg1 = (arg1 != 0)? 1: 0;
			          arg1 ^= 1;
					  break;
			}
			sprintf( slop, "%d", arg1 );
		}

		/* ===== Conditionals ===== */

		else if ( strncmp( q + 1, "<=", 2 ) == 0 )
		{
			i = load_and_compare(q+1,"<=");
			if ( numeric_args( token, moreslop ) )
			{
				arg1 = number(token);
				arg2 = number(moreslop);
				i = ( arg1 < arg2 || arg1 == arg2 )? 1: 0;
			}
			else
				i = ( i <= 0 )? 1: 0;
			sprintf( slop, "%d", i );
		}
		else if ( strncmp( q + 1, ">=", 2 ) == 0 )
		{
			i = load_and_compare(q+1,">=");
			if ( numeric_args( token, moreslop ) )
			{
				arg1 = number(token);
				arg2 = number(moreslop);
				i = ( arg1 > arg2 || arg1 == arg2 )? 1: 0;
			}
			else
				i = ( i >= 0 )? 1: 0;
			sprintf( slop, "%d", i );
		}
		else if ( strncmp( q + 1, "<", 1 ) == 0 )
		{
			i = load_and_compare(q+1,"<");
			if ( numeric_args( token, moreslop ) )
			{
				arg1 = number(token);
				arg2 = number(moreslop);
				i = ( arg1 < arg2 )? 1: 0;
			}
			else
				i = ( i < 0 )? 1: 0;
			sprintf( slop, "%d", i );
		}
		else if ( strncmp( q + 1, ">", 1 ) == 0 )
		{
			i = load_and_compare(q+1,">");
			if ( numeric_args( token, moreslop ) )
			{
				arg1 = number(token);
				arg2 = number(moreslop);
				i = ( arg1 > arg2 )? 1: 0;
			}
			else
				i = ( i > 0 )? 1: 0;
			sprintf( slop, "%d", i );
		}
		else if ( strncmp( q + 1, "=", 1 ) == 0 )
		{
			i = load_and_compare(q+1,"=");
			if ( numeric_args( token, moreslop ) )
			{
				arg1 = number(token);
				arg2 = number(moreslop);
				i = ( arg1 == arg2 )? 1: 0;
			}
			else
				i = ( i == 0 )? 1: 0;
			sprintf( slop, "%d", i );
		}
		else if ( strncmp( q + 1, "?", 1 ) == 0 )
		{
			load_and_compare(q+1,"?");  /* sets token and moreslop */
            if ( strlen(token) && strlen(moreslop) )
			    tkn = strstr( token, moreslop );
            else
                tkn = NULL;
			i = ( tkn != NULL )? 1 : 0;
			sprintf( slop, "%d", i );
		}
        else if ( stricmp( q+1, "TRUE" ) == 0 )
        {
        	strcpy( slop,"1");
        }
        else if ( stricmp( q+1, "FALSE" ) == 0 )
        {
        	strcpy( slop,"0");
        }
        else if ( stricmp( q+1, "T" ) == 0 )
        {
        	strcpy( slop,"1");
        }
        else if ( stricmp( q+1, "F" ) == 0 )
        {
        	strcpy( slop,"0");
        }
        else
        {
        	strcpy(slop,"1");  /* any (undefined) is 1 */
        }

		/* insert macro expansion into receiving buffer */

		q = "macro buffer overflow";  /* insertion error message */
		strcpy( moreslop, buffer );
		if (((strlen(moreslop) + strlen(slop))) >= (MACROSIZE - 1))
			crash_macro(q);
		else
			strcat( moreslop, slop );
		if (((strlen(moreslop) + strlen(p))) >= (MACROSIZE - 1))
			crash_macro(q);
		else
			strcat( moreslop, p );
		strcpy( buffer, moreslop );

		/* clear working buffers */

		memset( moreslop, 0, MACROSIZE );
		memset( slop, 0, MACROSIZE );
	}
}


/* macro evaluation returns 1 or 0, or -1 if syntax error */
int evaluate_macro( char *p )
{
    int result;

    token = malloc( MACROSIZE );
    slop = malloc( MACROSIZE );
    moreslop = malloc( MACROSIZE );

    if ( token == (char *)NULL || slop == (char *)NULL || moreslop == (char *)NULL )
        bomb("out of memory");

    if ( setjmp( osiris ) )
        result = -1;
    else
    {
	    expand_macros(p);
        strip_blanks( p );
        result = (*p == '1')? 1 : 0;
    }

    free( moreslop );
    free( slop );
    free( token );

    return( result );
}

