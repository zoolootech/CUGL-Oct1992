/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   WINDEMO.C  ***************************/

#include "mydef.h"   /* always include this */
#include <stddef.h>  /* we need the definition of NULL from here */

/* function prototype */
void fill_win(void);  

int start(void)      /* start is the entry point */
{
extern struct screen_structure scr;
extern struct window_structure w[];

char string[255];
int i;
char frame_attr, window_attr;
int w1,w2,w3,message;     /* variables to hold window handles */

/* clear the screen */
       cls();
/* fill the screen with dots '.'*/
/* make up a string of '.' wide enough to fill each row */

  /* build the string */
  for(i=0;i<scr.columns;i++) string[i]='.';
  string[i]= '\0';         /* terminate it*/

/* turn on alternate (virtual) screen so the printing is not seen */

  alt_screen(ON);

/* now fill each row of the screen with the string of '.' */

  for (i=1;i<=scr.rows;i++) print(1,i,string);

/* make the newly drawn screen visible */
  alt_screen(OFF);

/* make window 1, and print to it */

  w1=win_make (1,1,35,6,STD_FRAME,"Window1",scr.normal,scr.normal);

  print(1,1,"This window is framed.");
  print(1,2,"The frame attribute is normal.");
  print(1,3,"The interior attribute is normal.");
  print(1,4,"The cursor is of normal size.");
  print(1,6,"TOUCH ANY KEY TO CONTINUE:");

  getch();

/* make window 2 */
  w2= win_make (4,4,35,6,NO_FRAME,"",scr.normal,scr.inverse);

  cursor(BIG_CURSOR);  /* set the cursor to large */

  print(1,1,"This window is unframed.");
  print(1,3,"The interior attribute is inverse.");
  print(1,4,"The cursor is of large size.");
  print(1,6,"TOUCH ANY KEY TO CONTINUE:");

  getch();

/* make window 3 */
  w3=win_make (8,8,40,9,STD_FRAME,"Window3",scr.normal,scr.normal);

  cursor(NO_CURSOR);  /* hide the cursor */

/* demonstrate the use of attributes */

  print(1,1," Here are the predefined text attributes:");
  scr.current=scr.normal;
   print(1,2,"NORMAL");
  scr.current=set_intense(scr.current);
   print(1,3,"intense");
  scr.current=scr.inverse;
   print(1,4,"INVERSE");
  scr.current=scr.normal;

/* demonstrate bold caps */

  scr.bold_caps=TRUE;  /* set bold caps flag */
   print(1,5,"Demo of Bold Caps.");
  scr.bold_caps=FALSE; /* turn off flag */

  print(1,7,"The cursor is hidden.");
  print(1,9,"TOUCH ANY KEY TO CONTINUE:");

    getch();


/* make a message window */
/* if color mode, set color attributes */

  if(scr.mode==COLOR_80){
    window_attr=set_color(WHITE,BLUE);
    frame_attr=set_color(YELLOW,BLACK);
  }
   else {    /* if not color then use predefined attributes */
    frame_attr= scr.normal;
    window_attr=scr.normal;
   }

  message=win_make (10,20,47,5,STD_FRAME," message window",
                    frame_attr,window_attr);

  print(1,1,"Touch a key several times ");
  print(1,2,"to pop windows 1-3 to the top.");
  print(1,3,"The correct cursor appears for each window");
  print(1,5,"TOUCH ANY KEY TO CONTINUE:");

  getch();

/* now pop the windows to the top, one by one */

  win_pop_top(w1);   /* pop window one to the top */
   getch();
  win_pop_top(w2);   /* pop window two */
   getch();
  win_pop_top(w3);   /* pop window three */

/* pop the message window */
  win_pop_top(message);

  /* get the window attribute */
  scr.current=win_what_attr(message);
   cls();
  print(1,1,"Touch any key to see window movement");
   getch();

/* now move window 2 around the screen */

  for(i=0;i<4;i++) win_up(w2,1);    /* move it up */
  win_insert(w2,3);                 /* insert at level 3 */
  for(i=0;i<17;i++) win_down(w2,1); /* move it down */
     win_insert(w2,2);              /* move it to level 2 */
  for(i=0;i<14;i++) win_up(w2,1);   /* move it up */


/* give a demonstration of clearing and writing to overlapped
windows */

  cls();
  print(1,1,"Touch any key to see the windows cleared");
  print(1,2,"and new text written");
   getch();

/* do window 1 */
/* we will not do a win_redraw_all() until all the window are done */

  win_cls(w1);                    /* clear window 1 */
  scr.current=win_what_attr(w1);  /* get the attribute */
  win_print(w1,1,1,"Window 1");   /* print with the correct attribute
                                     for window 1 */

/* do window 2 */

  win_cls(w2);                    /* clear window 2*/
  scr.current=win_what_attr(w2);  /* get the attribute */
  win_print(w2,1,1,"Window 2");   /* print */


/* do window 3 */

  win_cls(w3);                    /* clear window 3 */
  scr.current=win_what_attr(w3);  /* get the attribute */
  win_print(w3,1,1,"Window 3");   /* print */


/* now that all the virtual windows have been updated,
   we will display the changes */

  win_redraw_all();

/* note: the active window is still the message window */

  cls();
  /* get attribute for window */
  scr.current =win_what_attr(message);

  print(1,1,"Touch any key to delete the windows");
   getch();
  for(i=0;i<4;i++) win_delete_top(); /* delete the window */

return(0);
}
