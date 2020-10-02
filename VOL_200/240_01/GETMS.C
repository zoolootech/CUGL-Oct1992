

#include "ciao.h"


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

