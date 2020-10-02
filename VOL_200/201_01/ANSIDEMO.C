/** ANSIDEMO.c
*
*   ANSIDEMO.C
*   (C) Copyright 1985 Don F. Ridgway
*   All Rights Reserved.
*   This program may be copied for
*   personal, non-profit use only.
*
*   Don F. Ridgway
*   Owner & Chief Programmer/Analyst
*   A-1 IBM Programming & Training Service
*   Custom Business Programs
*   119 Plantation Court, Suite D
*   Temple Terrace, FL  33617-3731
*   Ph: (813) 985-3342  (10:00am - 2:00pm EST)
*
*   Written, compiled and tested in Microsoft C,
*   ver. 2.03, and Lattice C, ver. 2.15, under
*   PC-DOS 2.1 on a Compaq w/640Kb RAM & 8087
*   using the PC-DOS 3.0 LINK and the TURBO
*   Pascal 3.0 screen editor.
*
*   (470 lines of code.)
*
*   This program demonstrates the features and
*   capabilities of my C programming language
*   header/module file named "ANSISYS.c", which
*   activates and implements the MS/PC-DOS
*   "ANSI.SYS" device driver for extended screen
*   and keyboard functions and control sequences.
*
*   NOTICE:  To run this program you MUST have booted
*   up with the DOS "ANSI.SYS" file on your boot disk
*   with a "CONFIG.SYS" file containing the statement
*   "device = ansi.sys" on your boot disk.  (Other-
*   wise you'll get meaningless numbers and symbols
*   across your screen.  If so, hit F10 or '0' to exit,
*   then boot up properly.  See the introduction to
*   ANSISYS.C for instructions on how to make the
*   CONFIG.SYS file.)
*
*   The "ANSISYS.c" file is to be #included in
*   your C programs to give them "smart" cursor
*   control and eye-catching "turtlegraphics"-type
*   screen/graphics display capability.
*
**/

#include <stdio.h>
#include "ansisys.c"                   /* this is the file to #include in all   */
                                       /* your C programs from now on to enable */
main()                                 /* all of the following fabulous screen, */
   {                                   /* cursor and keyboard features          */
   int dd,key1;
   dd=0;
   while (key1 != 196 && key1 != 48)   /* while key1 not equal to F10 or '0' */
      {                                /*            show main menu          */
      if (dd==0)
         mainmenu();
      XKPROMPT(20,29,key1);
      dd=0;
      switch(key1)
         {
         case 187:                     /* F1 key -- Set Screen & Graphics    */
         case 49:                      /* '1' key -- just in case some jelly */
                                       /*       spilled on the Function keys */
                  shoscreen();
                  break;
         case 188:                     /* F2 key -- Set Display & Color */
         case 50:                      /* '2' key */
                  shodisplay();
                  break;
         case 189:                     /* F3 key -- Extended Keyboard demo */
         case 51:                      /* '3' key */
                  xkeyboard();
                  break;
         case 190:                     /* F4 key -- Arrow Keys demo */
         case 52:                      /* '4' key */
                  cursarrow();
                  break;
         case 191:                     /* F5 key -- DRAW function demo */
         case 53:                      /* '5' key */
                  showdraw();
                  break;
         case 192:                     /* F6 key -- FILL function demo */
         case 54:                      /* '6' key */
                  showfill();
                  break;
         case 193:                     /* F7 key -- WINDOW function demo */
         case 55:                      /* '7' key */
                  showindow();
                  break;
         case 196:                     /* F10 key -- to exit program */
         case 48:                      /* '0' key */
                  break;
         default:
                  dd=1;                /* any other key loops back around */
                  break;
         }                             /* end switch */
      }                                /* end while */

   XYPRINTF(23,1,"Goom\nbye!");
   BEEP;
   exit(0);
   }                                   /* end main ANSIDEMO.c */

/*
* ------------------------------------------------------------------------
*/

mainmenu()                             /* draw Main Menu */
   {
   CLS;                                /* clear screen */
   DRAW(3,19,23,61,213);               /* draw distinctive one/two line border */
   HLON;                               /* turn high-intensity display on */
   DRAW(4,21,22,59,178);               /* draw artistic inside border to offset */
   XYPRINTF(2,31,"A N S I D E M O . c");
   XYPRINTF(24,29,"(c) 1985 Don F. Ridgway");
   HLOFF;                              /* turn high-intensity off */
   XYPRINTF(6,28,"F1)  Set Screen/Graphics");
   XYPRINTF(8,28,"F2)  Set Display/Color");
   XYPRINTF(10,28,"F3)  Extended Keyboard Keys");
   XYPRINTF(12,28,"F4)  Cursor Arrow Keys");
   XYPRINTF(14,28,"F5)  DRAW Border,Line,Point");
   XYPRINTF(16,28,"F6)  FILL macro/function");
   XYPRINTF(18,28,"F7)  WINDOW macro/function");
   XYPRINTF(20,28,"   <---- (F10 to Exit)");
   return(0);
   }                                   /* return to main program */

/*
* ------------------------------------------------------------------------
*/

shoscreen()                                      /* Set Screen Graphics demo */
   {
   int c;

   CLS;                                          /* clear screen */
   while (c!=9)                                  /* while not number 9 */
      {
      DRAW(1,1,1,80,178);
      XCTRPRINTF(5,"This is Set Screen - Set Graphics Demo");
      XCTRPRINTF(7,"0=40x25 monochrome,1=40x25 color, 2=80x25 mono,  3=80x25 color,    ");
      XCTRPRINTF(9,"4=320x200 color,   5=320x200 mono,6=640x200 mono,7=enable word-wrap");
      c=' ';
      XYPRINTF(12,5,"Enter # of Graphics Mode desired  ( 9 => Exit) : ");
      scanf("%d",&c);
      if (c==9) break;                           /* if 9 break out of loop */
      SETSCREEN(c);
      XYPRINTF(15,1,"ABCDEFGHIJKLabcdefghijkl");
      XCTRPRINTF(18,"1234567890");
      }

   return(0);
   }                                             /* return to main menu */

/*
* ------------------------------------------------------------------------
*/

shodisplay()                           /* Set Display/Color attributes demo */
   {
   int c,d,e;

   CLS;
   while (c!=9)                                  /* while not number 9 */
      {
      DRAW(1,1,1,80,178);
      XCTRPRINTF(3,"This is Set Display/Color Attributes Demo");
      XCTRPRINTF(5,"Set screen display attributes and colors:");
XYPRINTF(7,1," 0 = default,           1 = high-intensity, 4 = underline,");
XYPRINTF(8,1," 5 = blink,             7 = inverse,        8 = invisible (black-on-black),");
XYPRINTF(10,1,"30 = FOREGROUND black, 31 = fore red,      32 = fore green, 33 = fore yellow,");
XYPRINTF(11,1,"34 = fore blue,        35 = fore magenta,  36 = fore cyan,  37 = fore white, ");
XYPRINTF(13,1,"40 = BACKGROUND black, 41 = back red,      42 = back green, 43 = back yellow,");
XYPRINTF(14,1,"44 = back blue,        45 = back magenta,  46 = back white.");
      c=d=e=' ';
      XYPRINTF(16,1,"Enter three numbers, seperated by SPACES of Display/Color desired");
      XYPRINTF(17,1,"putting numbers in right-hand columns first, e.g., 0 0 5 is BLINK");
      XYPRINTF(18,1,"(A '9'in any column will Exit)  '0 0 0' resets to normal  ");
      XY(18,58);
      scanf("%d %d %d",&c,&d,&e);                /* careful! no error-trapping here! */
      if (c==9||d==9||e==9) break;               /* if any number 9, break out */
      SETDISPLAY(c,d,e);
      DRAW(18,58,18,80,255);
      XCTRPRINTF(20,"Is this what you wanted?");
      }

   return(0);
   }                                             /* return to main menu */

/*
* ------------------------------------------------------------------------
*/

xkeyboard()                                      /* Extended Keyboard demo */
   {
   int c;

   CLS;
   HLON;
   DRAW(1,1,1,80,178);
   HLOFF;
   printf("\nHello there, This is Extended Keyboard Demo ('*'= Exit )\n\n\n");

   while (c!='*')                                /* while not '*' */
      {
      printf("\n ----> Press ANY key on the keyboard: ");
      XKREAD(c);                                 /* no-echo read */
      printf("  The extended-keyboard-read code =  %d",c);
      }

   return(0);
   }                                             /* return to main menu */

/*
* ------------------------------------------------------------------------
*/

cursarrow()                            /* Display use of cursor arrow keys */
   {
   int key;

   CLS;                                /* clear screen */
   HLON;
   DRAW(1,1,1,80,178);
   HLOFF;
   XCTRPRINTF(3,"Move cursor with ARROW keys, HOME and END  ('*'= Exit )");
   XY(12,40);
   SAVCURS;                            /* save cursor position (12,40) for later */

   while (key != '*')                  /* while not '*' */
      {
      XKREAD(key);                     /* read keystroke, no echo */

      switch(key)
         {
         case 199:                     /* HOME key */
                  XY(1,1);
                  break;
         case 200:                     /* UP arrow key */
                  printf("\033[1A\b"); /* --> NOTE: When utilizing these macros     */
               /* CURSUP(1); */        /* from the actual keyboard, as we are doing */
                                       /* in this demo, they need a '\b' backspace  */
                  break;               /* because hitting the key moves it forward  */
         case 203:                     /* LEFT arrow key */
                  CURSBCK(2);          /* NOTE the (2) per above reason (need two  */
                  break;               /* spaces back to overcome the one forward) */
         case 205:                     /* RIGHT arrow */
               /* CURSFWD(1);*/        /* NOTE letting it move forward by itself */
                  break;               /* because of the physical keystroke      */
         case 207:                     /* END (of screen) key */
                  XY(24,79);
                  break;
         case 208:
                  printf("\033[1B\b"); /* DOWN key */
               /* CURSDWN(1);*/        /* see NOTE on UP arrow key */
                  break;
         case 42:                      /* hit '*' to quit program */
                  break;
         default:                      /* Any other key, while not doing any- */
                  CURSBCK(1);          /* thing, nevertheless needs to be moved */
                  break;               /* back where it was.  See NOTE UP arrow */
         }                             /* end switch */
      }                                /* end while */
      RECALLCURS;                      /* recall cursor (to 12,40) */
      puts("Press any key to return to Main Menu");   /* then print message */
      XKREAD(key);
      CLS;
      return(0);
   }                                   /* end cursarrow demo */

/*
* ------------------------------------------------------------------------
*/

showdraw()                             /* DRAW(row1,col1,row2,col2,icon) demo */
   {
   int a,b,c,d,e,key;
   char *greet;
   greet="Hi there -- I'm Fast-draw Demo!";

   CLS;
   XCTRPRINTF(2,"Demo of DRAW(row1,col1,row2,col2,icon)");
   DRAW(5,9,20,71,205);                          /* little demo display */
   DRAW(6,11,19,69,176);
   DRAW(7,12,18,68,177);
   DRAW(8,13,17,67,178);
   DRAW(9,14,16,66,219);
   DRAW(11,20,14,60,196);
   HLON;
   XCTRPRINTF(12,greet);
   DRAW(21,3,21,77,207);
   DRAW(22,3,22,77,178);
   DRAW(8,4,8,4,14);
   DRAW(16,4,16,4,2);
   DRAW(8,76,16,76,219);
   DRAW(8,75,16,75,182);
   HLOFF;
   CURSPOSPRTF(24,46,"Press any key to continue ");
   XKREAD(key);
   key=99;
   CLS;
   while (key!=81&&key!=113)     /* while not Capital Q or lower case q q)uit */
                                 /* NOTE: The possibility of upper or lower case data */
                                 /* entry is handled throughout in this fashion so    */
                                 /* this program could more "stand on its own" and    */
                                 /* not require the islower() or toupper() functions  */
                                 /* to be #included from ctype.h header file          */
      {
      if (key==99||key==67)                      /* if C)learscreen */
         {
         XCTRPRINTF(1,"Demonstration of DRAW(row1,col1,row2,col2,icon)");
         SETDISPLAY(0,0,1);                      /* high intensity */
         DRAW(3,1,3,80,196);                     /* border line    */
         SETDISPLAY(0,0,0);
      CURSPOSPRTF(2,3,"-> Enter row1,col1,row2,col2,icon, SPACES delimiting: ");
 XCTRPRINTF(4,"Try: 10 25 15 55 205, 5 9 20 71 178, 13 1 13 80 219, 9 24 16 56 213");
         };
      key=a=b=c=d=e=0;                           /* initialize         */
      CURSPOSPRTF(2,59,".. .. .. .. ...");       /* little input guide -- don't have  */
      CURSPOS(2,59);                             /* to follow exactly but just space  */
      scanf("%d %d %d %d %d",&a,&b,&c,&d,&e);    /* between entries and hit carriage  */
                                                 /* return at end.  If goof, do ^C    */
                                                 /* to start all over again. [solly]  */
      DRAW(a,b,c,d,e);                           /* Careful! No input error-checking  */
      CURSPOSPRTF(24,46,"A)nother E)raselast C)LS Q)uit?");
      SETDISPLAY(0,0,5);
      CPR(24,46,65);                             /* blink the   */
      CPR(24,55,69);                             /* A,E,C and Q */
      CPR(24,66,67);
      CPR(24,71,81);
      CURSPOS(24,79);
      SETDISPLAY(0,0,0);
      XKREADE(key);                              /* extended keyboard read */
      if (key==97||key==65)                      /* do A)nother */
         DRAW(24,46,24,80,255);
      else if (key==99||key==67)                 /* C)learScreen,restart */
         CLS;
      else if (key==101||key==69)                /* E)rase last figure */
         {
         DRAW(a,b,c,d,255);                      /* redraw with blanks */
         DRAW(24,46,24,80,255);
         }                                       /* Q)uit falls through */
      }                                          /* end while           */
   CLS;
   return(0);
   }                                             /* end of Showdraw Demo */

/*
* ------------------------------------------------------------------------
*/

showfill()                             /* Demo of FILL(row1,col1,row2,col2,fill) */
   {
   int a,b,c,d,e,key;
   CLS;
   XCTRPRINTF(2,"Demo of FILL(row1,col1,row2,col2,fill)");
   FILL(10,25,15,55,219);                        /* little razzledazzle */
   FILL(5,9,20,71,197);
   FILL(4,40,24,40,221);
   FILL(5,41,20,71,255);
   FILL(10,45,15,75,219);
   CURSPOSPRTF(24,46,"Press any key to continue ");
   XKREAD(key);
   key=99;
   CLS;
   while (key!=81&&key!=113)                     /* while not Q)uit */
      {
      if (key==99||key==67)                      /* if C)learscreen */
         {
         XCTRPRINTF(1,"Demonstration of FILL(row1,col1,row2,col2,fill)");
         SETDISPLAY(0,0,1);                      /* high intensity */
         DRAW(3,1,3,80,196);                     /* border line    */
         SETDISPLAY(0,0,0);
      CURSPOSPRTF(2,3,"-> Enter row1,col1,row2,col2,fill, SPACES delimiting: ");
XCTRPRINTF(4,"Try: 10 25 15 55 219, 5 9 20 71 197, 4 40 24 40 221, 9 24 16 56 178");
         };
      key=a=b=c=d=e=0;                           /* initialize         */
      CURSPOSPRTF(2,59,".. .. .. .. ...");       /* little input guide */
      CURSPOS(2,59);
      scanf("%d %d %d %d %d",&a,&b,&c,&d,&e);
      FILL(a,b,c,d,e);                           /* careful! -- no input */
                                                 /* error-checking here! */
      CURSPOSPRTF(24,46,"A)nother E)raselast C)LS Q)uit?");
      SETDISPLAY(0,0,5);
      CPR(24,46,65);                             /* blink the   */
      CPR(24,55,69);                             /* A,E,C and Q */
      CPR(24,66,67);
      CPR(24,71,81);
      CURSPOS(24,79);
      SETDISPLAY(0,0,0);
      XKREADE(key);                              /* extended keyboard read */
      if (key==97||key==65)                      /* do A)nother */
         XYEOL(24,46);
      else if (key==99||key==67)                 /* C)learScreen,restart */
         CLS;
      else if (key==101||key==69)                /* E)rase last figure */
         {
         FILL(a,b,c,d,255);                      /* refill with blanks */
         XYEOL(24,46);
         }                                       /* Q)uit falls through */
      }                                          /* end while           */
   CLS;
   return(0);
   }                                             /* end of Showfill Demo */

/*
* ------------------------------------------------------------------------
*/

showindow()                            /* WINDOW(row1,col1,row2,col2,fill,bord) demo */
   {
   int a,b,c,d,e,f,key;
   CLS;                                          /* clearscreen       */
   XCTRPRINTF(2,"Demo of WINDOW(row1,col1,row2,col2,fill,bord)");
   WINDOW(10,25,15,55,219,205);                  /* ----------------- */
   WINDOW(8,23,19,59,178,213);                   /*   if you got it   */
   WINDOW(5,65,10,75,219,196);                   /*   -------------   */
   WINDOW(15,5,20,15,254,205);                   /*     flaunt it!    */
   WINDOW(5,1,7,63,14,219);                      /* ----------------- */
   WINDOW(15,70,15,70,7,2);
   CURSPOSPRTF(24,46,"Press any key to continue ");
   XKREAD(key);
   key=99;
   CLS;
   while (key!=81&&key!=113)                     /* while not Q)uit do  */
      {                                          /* note upper/lowercase */
      if (key==99||key==67)                      /* if screen's cleared */
         {                                       /* redraw header-menu  */
      XCTRPRINTF(1,"Demonstration of WINDOW(row1,col1,row2,col2,fill,bord)");
         SETDISPLAY(0,0,1);                      /* set high intensity  */
         DRAW(3,1,3,80,196);                     /* border line         */
         SETDISPLAY(0,0,0);                      /* set display normal  */
      CURSPOSPRTF(2,1,"-> Enter coordinates & fill & border with SPACE delimit: ");
XCTRPRINTF(4,"Try: 10 25 15 55 219 205, 5 65 10 75 219 196, 9 24 16 56 219 213");
         };
      key=a=b=c=d=e=f=0;                         /* initialize          */
      CURSPOSPRTF(2,59,".. .. .. .. ... ...");   /* little input guide  */
      CURSPOS(2,59);                             /* position cursor     */
      scanf("%d %d %d %d %d %d",&a,&b,&c,&d,&e,&f); /* BE CAREFUL!!! -- no  */
      WINDOW(a,b,c,d,e,f);                          /* error-checking here! */
      CURSPOSPRTF(24,46,"A)nother E)raselast C)LS Q)uit?");
      SETDISPLAY(0,0,5);                         /* set display to blink */
      CPR(24,46,65);                             /* print/blink the 'A' */
      CPR(24,55,69);                             /* blink 'E'           */
      CPR(24,66,67);                             /* blink 'C'           */
      CPR(24,71,81);                             /* blink 'Q'           */
      CURSPOS(24,79);                            /* position cursor     */
      SETDISPLAY(0,0,0);                         /* set display normal  */
      XKREADE(key);                              /* extended keyboard read */
      if (key==97||key==65)                      /* do A)nother         */
         XYEOL(24,46);                           /* erase line 24 menu  */
      else if (key==101||key==69)                /* E)rase last figure  */
         {
         FILL(a,b,c,d,255);                      /* refill with blanks  */
         DRAW(24,46,24,80,255);                  /* erase line 24 menu  */
         }
      else if (key==99||key==67)                 /* C)learscreen,restart */
         CLS;                                    /* Q)uit falls through */
      }                                          /* end while           */
   CLS;                                          /* clear screen so image */
   return(0);                                    /* ends with program   */
   }                                             /* end of WINDOW Demo */

/*
***  the end of ANSIDEMO.c  --  hope you liked it! --  C you again sometime?  ***
*/