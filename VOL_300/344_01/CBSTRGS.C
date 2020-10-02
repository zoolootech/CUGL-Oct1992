/*
HEADER:		;
TITLE:		BASIC-like string functions;
VERSION:	1.0;

DESCRIPTION:    Functions in C to implement the BASIC string functions MID$
		and RIGHT$;

KEYWORDS:	String utilities;
SYSTEM:		MSDOS;
FILENAME:	CBstrgs;
WARNINGS:	None;
SEE ALSO:	Wgets;

AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C;
*/
#include <stdio.h>
#include <stdlib.h>

/***************************************************************************
 *                             Function: Mid                               *
 *          Mid selects 'nofchar' characters beginning at 'start'          *
 ***************************************************************************/

char *Mid(char *str, int start, int nofchar)
{
     int strgleng = strlen(str), stop;
     char *newstr, *newstr2;
     newstr = calloc(strgleng+1,sizeof(char));
     newstr2 = newstr;
     if(start<1 || nofchar<1 || start>strgleng)
       return(newstr=NULL);
     --start;
     if((nofchar+start)>strgleng)
       nofchar = strgleng - start;
     stop = start + nofchar;
     while(start<stop)
     {
     *newstr++ = *(str + start);
     ++start;
     }
     *newstr = NULL;
     newstr = newstr2;
     return(newstr);
}
/***************************************************************************
 *				Function: Right				   *
 *                 Right selects 'nofchar' rightmost characters            *
 ***************************************************************************/

char *Right(char *str, int nofchar)

{
     int start;
     char *newstr;
     start = strlen(str) - nofchar +1;
     newstr = Mid(str,start,nofchar);
     return(newstr);
}
/***************************************************************************
 *	          		Function: Wgets                            *
 * Gets a string of characters from 'stdin' and echoes to current viewport *
 ***************************************************************************/
char *Wgets(char *s)
{
     char *s2;
     int keys;
     s2 = s;
     while((keys=getche())!=13)
     {
       *s++ = (char) keys;
       if(keys==8)
       {
	 putch(32);
	 putch(keys);
	 s-=2;
       }
     }
     *s = '\0';
     return(s2);
}