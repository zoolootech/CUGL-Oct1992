
/* money.c, david c. oshel, 11/22/86
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <malloc.h>
#include <stdarg.h>
#include <signal.h>
#include <setjmp.h>
#include "ciao.h"

#define ESC 27
#define MINUS '-'

/* edit interrupt flags
*/

int ESCflag = 0, MINUSflag = 0;


/* system date
*/

char date_mask[] = "00/00/00";   /* initialized from system date */


/*
**  dlrcvt -- return a pointer to dollar & cents string with commas
**            input is a long value
**            has trailing minus sign if negative input
**            dollar sign not included
**            leading spaces not included
**            overflow shows ***,***,***.**
*/

char *dlrcvt( dval )
long dval;
{
        register char *f, *q;
        static char *p, dollars[16];
        auto char buffer[34];
        auto int sign;

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

char *crdrform( x )
long x;
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



/* keyin with key click and Control C trap
** user must do  BREAKOFF()  in order to enable Control C trapping
** the macro is defined in "ciao.h"
** BREAK becomes ESC, but screen is still marred by "^C" message
*/

static jmp_buf l_onj;
static void c_onj()
{
     longjmp( l_onj, -1 );
}

int getkey()
{
      int n;
      signal( SIGINT, c_onj );
      if (setjmp( l_onj ) == 0) n = keyin( noop ); else n = ESC;
      signal( SIGINT, SIG_DFL );
      HIclack();
      return (n);
}


/* edit masked numeric
** may only edit digits in the self-masking string, can't delete
** skips over (forward or backward) any edit character not a digit
** calls Function Key handler if user enters one
** the called Fn function is responsible for screen condition!
** returns 0 if successful 
** returns ESC if user interrupts with ESC and ESCflag is non-zero
*/

int edit_masked_numeric( buffptr, fnkey )
char *buffptr;
void (* fnkey)();
{
     register char *p; 
     register int ch;
     int len;

     /* highlight string to be edited */

     wprintf( "^2%s", buffptr );
     len = strlen( buffptr );
     for (ch = 0; ch < len; ch++ ) wink( '\b' );

     p = buffptr;
     while (*p)
     {
         if ( isdigit( *p ) ) 
         {
              odd:  ch = getkey();
              switch (ch)
              {
              case ESC:{ if (ESCflag != 0) return (ESC); break; }
              case 13: { /* accept entry */
                       while (*p) wink( *p++ );
                       continue;
                       break;
                       }
              case '\b':
              case 203:{ /* left arrow */
                       if (p > buffptr)
                       {
                          do
                          {
                             --p;
                             wink( '\b' );
                          }
                          while ( (!isdigit( *p )) && (p > buffptr) );
                       }
                       continue;
                       break;
                       }
              case 205:{ /* right arrow */
                       wink( *p++ );
                       continue;
                       break;
                       }
              case 187:
              case 188:
              case 189:
              case 190:
              case 191:  /* was input a Fn key? */
              case 192:
              case 193:
              case 194:
              case 195:
              case 196:{
                       (* fnkey)( ch );
                       continue;
                       break;
                       }
              default: if ( isascii( ch ) && isdigit( ch ) )
                       ;
                       else { thurb(); goto odd; }
              }
         }
         else ch = *p;        /* not digit mask, accept entry from buffptr */

         *p = ch;             /* put char in buffptr */
         wink( *p++ );        /* echo it */

     }

     /* display string as edited, normal vid mode */

     for (ch = 0; ch < len; ch++ ) wink( '\b' );
     wprintf( "^0%s", buffptr );
     return ( 0 );
}




/* Edit String.
** calls Function Key handler if user enters one
** the called Fn function is responsible for screen condition!
** returns ESC if user interrupts with ESC and ESCFlag is nonzero
** returns MINUS if user interrupts with MINUS and MINUSFlag is nonzero
** returns 0 otherwise 
*/

int edit_string( buffptr, fnkey )
char *buffptr;
void (* fnkey)();
{
     register char *p; 
     register int ch;
     int len;

     /* highlight string to be edited */

     wprintf( "^2%s", buffptr );
     len = strlen( buffptr );
     for (ch = 0; ch < len; ch++ ) wink( '\b' );

     p = buffptr;
     while (*p)
     {
          ch = getkey();
          if ( isascii( ch ) )
          {
             switch (ch)
             {
             case ESC:{ if (ESCflag != 0) return (ESC); break; }
             case '\r':{
                      /* CR, accept input & exit */
                      while (*p) wink( *p++ );
                      break;
                      }
             case '\b':{                        /* backspace */
                      if (p > buffptr)
                      {
                         --p;
                         wink( '\b' );
                      }
                      break;
                      }
             case '-':{ if (MINUSflag != 0) return (MINUS); /* NO BREAK */ } 
             default: {
                      if ( isgraph( ch ) || ch == 0x20 ) 
                      { 
                          *p = ch;              /* put char in buffptr */
                          wink( *p++ );         /* echo it */
                      }
                      else 
                      {
                          thurb();              /* reject unprintables */
                      }
                      break;
                      }
             }
          }
          else
          {
             switch (ch)
             {
             case 187:
             case 188:
             case 189:
             case 190:
             case 191:  /* was input a function key? */
             case 192:
             case 193:
             case 194:
             case 195:
             case 196:{
                      (* fnkey)( ch );
                      break;
                      }
             case 203:{                         /* left arrow */
                      if (p > buffptr)
                      {
                         --p;
                         wink( '\b' );
                      }
                      break;
                      }
             case 205:{                         /* right arrow */
                      wink( *p++ );
                      break;
                      }
             default: {
                      thurb();
                      break;
                      }
             }
          }
     }

     /* display string as edited, normal vid mode */

     for (ch = 0; ch < len; ch++ ) wink( '\b' );
     wprintf( "^0%s", buffptr );
     return ( 0 );
}



void set_date()
{
       long ltime;
       struct tm *timevals;
       int d,m,y;
       time( &ltime );
       timevals = localtime( &ltime );
       d = timevals->tm_mday;
       m = timevals->tm_mon + 1;
       y = timevals->tm_year;
       sprintf( date_mask, "%02u/%02u/%02u", m, d, y );
}



/* message() has variable args, works like wprintf with gotoxy
*/

void message( x,y,p ) int x,y; char *p;  
{
     va_list arg_ptr;
     char *buff;

     gotoxy( x,y ); wputs("^0"); clreol();  /* NOTE: Clears line! */
     buff = malloc( 128 );
     va_start(arg_ptr, p);
     vsprintf( buff, p, arg_ptr );
     va_end( arg_ptr );
     wputs( buff );
     free( buff );
}



