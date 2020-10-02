
/* KeyDefs.C
** December 7, 1986, David C. Oshel
**
** Displays a popout window with the ten Fn keys drawn down the middle
** The function names are listed in 21 character fields to left and right
** Sample arrays of menu strings are shown below, all 10 must be defined
** even if the string is null.
** Waits for response from the user, if ESC is typed, returns F1
*/

#include "ciao.h"
#include "keys.h"
#define keycatcher() flush_keys()

/* strings may be up to 21 chars, preferably no more than 18
*/
                   /* ....v....1....v....2. */

/* note: example arrays show how to duplicate strings in the menus! 
** "phi" is a glyph meaning "undefined function" in one application
**

static char nyi[] = "í",
            sco[] = "Screen Colors",
            klk[] = "Key Click";

char *MainMenu[] = { "Quit Program",             "Help", 
                     "Chart of Accounts",        "General Journal", 
                     "User Information",         "Trial Balance",
                      sco,                       "Close the Books",
                      klk,                       "Financial Reports"
                   };

char *JnlMenu[] =  { "Quit Program",             "List Fn Keys",
                     "List Chart of Accts",      "Review Journal Docs",
                      nyi,                        nyi,
                      sco,                        nyi,
                      klk,                        nyi
                   };
**
*/

int KeyDefs( name, menutitle, msg ) 
char **name, *menutitle, *msg;
{
     char far *p; union REGS x;  
     static int i,n;
     char *blks,*dots;

     blks = "                     ",
     dots = ".....................";

     p = savescreen( &x );  /* save caller's original screen */
     windowbox ( 13, 3, 53 + 13, 20 + 3 );

     gotoxy( ((53 - strlen( menutitle )) / 2), 1 ); 
     wprintf("%s\n\n", menutitle );

     /* draw key boxes */ 
     for (n = 1, i = 0; i < 5; n += 2, i++)
     {
        wink(' '); wputs(blks); wputs("ÉÍÍÍ»ÉÍÍÍ»\n"); 
        wink(' '); wputs(dots); wprintf("ºF%-2dººF%-2dº", n, n + 1); 
                   wputs(dots); wink('\n');
        wink(' '); wputs(blks); wputs("ÈÍÍÍ¼ÈÍÍÍ¼\n");
     }

     /* fill in key names */
     for ( n = 4, i = 0; i < 5; n += 3, i++)
     {
          gotoxy( 1, n );                   wputs( *name++ ); /* left  */
          gotoxy((53 - strlen(*name)), n ); wputs( *name++ ); /* right */
     }

     gotoxy( ((53 - strlen( msg )) / 2), 19 ); wputs( msg );
     defcursor();
     keycatcher();
     n = getkey();
     if (n == ESC) n = F1;
     restorescreen( p, &x );
     return (n);
}


