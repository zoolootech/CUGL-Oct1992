
/* oldedits.c, david c. oshel, 11/22/86
** 4/23/88, d.c.o, removed signal code, doesn't belong in service routines
** 8/29/88, d.c.o. formerly MONEY.C, rearranged old editing routines to
**                 avoid linking them when not called for
*/

#include <stdarg.h>
#include "ciao.h"
#include "keys.h"

#define MINUS '-'

/* edit interrupt flags
*/

int ESCflag = 0, MINUSflag = 0;

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
     static char *p; 
     static int ch;
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
              case CR: { /* accept entry */
                       while (*p) wink( *p++ );
                       continue;
                       break;
                       }
              case BS:
              case LF:{ /* left arrow */
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
              case RT:{ /* right arrow */
                       wink( *p++ );
                       continue;
                       break;
                       }
              case F1:
              case F2:
              case F3:
              case F4:
              case F5:  /* was input a Fn key? */
              case F6:
              case F7:
              case F8:
              case F9:
              case F10:{
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
     static char *p; 
     static int ch;
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
             case CR:{
                      /* CR, accept input & exit */
                      while (*p) wink( *p++ );
                      break;
                      }
             case BS:{                        /* backspace */
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
             case F1:
             case F2:
             case F3:
             case F4:
             case F5:  /* was input a function key? */
             case F6:
             case F7:
             case F8:
             case F9:
             case F10:{
                      (* fnkey)( ch );
                      break;
                      }
             case LF:{                         /* left arrow */
                      if (p > buffptr)
                      {
                         --p;
                         wink( '\b' );
                      }
                      break;
                      }
             case RT:{                         /* right arrow */
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



/*========================================*/
/*  getms(p, maxinput, wait, esc)         */
/*  Simple getline function with special  */
/*  input handling,  1 <= len <= maxinput */
/*========================================*/ 

int getms( p, maxinput, wait, esc ) 
char *p; 
int maxinput; 
void (* wait)();
void (* esc)();
{
   static int len;
   static int ch;

   len = -1;
   if (maxinput < 1 || maxinput > 79) maxinput = 79;

   while (1)
   {
        ch = keyin( wait );  

        if (len < 0) len = 0;       /* don't destroy prompt by backing up */

        if (ch == '\n' || ch == '\r') 
        {                           /* end of line?  don't store newline */
                *p = '\0';          /* mark it with a B for baby and me  */
                break;              /* and break out of while loop       */
        }

        else if ((ch == '\b') || (ch == 127)) 
        {                                /* backspace? rubout?  */
                if (len-- > 0) 
                {
                        wink(127);       /* destructive bs      */
                        p--;             /* delete from string  */
                }
        }

        /*--------------------------------*/
        /* SPECIAL ROUTINE FOR ESCAPE KEY */
        /*--------------------------------*/

        else if (ch == '\033') 
        {                                /* do ESC function */
                (* esc)();               /* routinely contains a longjmp */
        }

        else if (ch == '\025' || ch == '\030') 
        {                                /* Ctl-U, Ctl-X */
                while (len--) 
                {
                        p--;
                        wink(127);
                }
        }
        else if (len == maxinput) 
        {                                /* test specials before testing len */
                bell();
        }
        else if (ch > 31 && ch < 127) 
        {                                /* printable char? */
                wink(ch);                /* yes, echo it */
                *p++ = ch;               /* collect it */
                len++;                   /* keep track of it */
        }
        else 
        {                                /* control chars? */
                bell();
        }                
   }
   return( len );

} /* end: getms */

