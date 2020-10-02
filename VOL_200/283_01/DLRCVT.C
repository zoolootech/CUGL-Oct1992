/* dlrcvt.c - extracted from MONEY.C to avoid linking old edit routines
*/

#include "ciao.h"

/*
**  dlrcvt -- return a pointer to dollar & cents string with commas
**            input is a long value
**            has trailing minus sign if negative input
**            dollar sign not included
**            leading spaces not included
**            overflow shows ***,***,***.**
*/

char *dlrcvt( long dval )
{
        static char *f, *q;
        static char *p, dollars[16];
        static char buffer[34];
        static int sign;

        /* initialize receiving field */

                      /*  0....v....1....v */
        strcpy( dollars, "   ,   ,  0.00 "); 

        /* fetch digits to be formatted */

        p = ltoa( dval, buffer, 10 );

        sign = (*p == '-');
        if (sign) ++p;


        /* format dollars field, right to left */

        q = strchr(p,'\0');        /* locate from end of source string */
        q--;
        f = dollars + 13;          /* locate to end of destination */
        while (f >= dollars)
        {
             if ( q >= p )   /* is q to the right of first digit? */
             {
                 if (*f == ',' || *f == '.') --f;  /* jump left */
                 *f = *q--;
             }
             else            /* no, no more digits remain in source */
             {
                 if (f < dollars + 10) 
                 {
                     *f = ' ';  /* fill destination left with blanks */
                 }
             }
             --f;
        }

        if (q > buffer)  /* OVERFLOW? does source still have digits? */
        {
                           /*  0....v....1....v */
             strcpy( dollars, "***,***,***.** ");  /* overflow */
        }

        if (sign) dollars[14] = '-';
        f = dollars;
        while (*f == ' ') f++;
        return (f);

} /* dlrcvt() */


/* Same, but minus sign is interpreted as trailing Cr, positive is Dr
*/

char *crdrform( long x )
{
     static char *q, r[20];

     strcpy( r, dlrcvt( x ) );
     q = strchr( r, '\0' );
     --q;
     if ( x == 0L )      strcpy( q, "Dr" ); /* zero is positive */
     else if (*q == '-') strcpy( q, "Cr" ); 
     else                strcpy( q, "Dr" );
     return (r);
}



