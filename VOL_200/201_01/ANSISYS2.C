/** ansisys.c
*
*   ANSISYS.C
*   (C) Copyright 1985 Don F. Ridgway
*   All rights reserved.
*   This program may be copied for
*   personal, non-profit use only.
*
*   This is an original and unique C programming
*   language header/function file to #include with
*   your C programs to give them "smart" cursor
*   control and eye-catching "turtlegraphics"-
*   type screen and graphics display qualities.
*
*   Programmed by:
*   Don F. Ridgway
*   Owner & Chief Programmer/Analyst
*   A-1 IBM Programming & Training Service
*   CUSTOM BUSINESS PROGRAMS
*   119 Plantation Ct., Suite D
*   Temple Terrace, FL  33617-3731
*   Ph: (813) 985-3342  (10:00 am - 2:00 pm EST)
*
*   Written, compiled & tested in Microsoft C,
*   ver. 2.03, and Lattice C, vers. 2.15 & 3.00, under
*   PC-DOS 2.1 & 3.1 on a Compaq w/640Kb RAM & 8087, using
*   the TURBO Pascal 3.0 screen editor.  (260+ lines.)
*
*   NOTE:  To utilize these macros you must have: (1) The
*   ANSI.SYS file that came with your PC-DOS or MS-DOS 2.xx
*   operating system on your boot disk and, (2) you must boot
*   up with a CONFIG.SYS file on that boot disk which
*   contains the statement: DEVICE = ANSI.SYS.  This loads
*   the ANSI.SYS device driver into DOS at bootup time.  The
*   operating system searches (for) CONFIG.SYS before it looks
*   for an AUTOEXEC.BAT file.  Please refer to your DOS
*   Reference Guide under "ANSI.SYS" and "COPY" for details.
*
*   (Simply, at A> prompt on a boot diskette, type:
*                 COPY CON:CONFIG.SYS<cr>
*                 DEVICE=ANSI.SYS<cr>
*                 <F6><cr>
*   and then reboot and you're ready to go!  Small bother
*   for the brilliant performance gained in your C programs--
*   just have these two files, ANSI.SYS and CONFIG.SYS, on
*   your boot disk whenever you boot up.)
*
*   (The diskette these programs are sent to you on is a
*   PC-DOS 2.1 boot disk so you may boot up with it and run
*   ANSIDEMO.EXE.  Note the ANSI.SYS and CONFIG.SYS files.)
*
*   This custom C module/header file connects the C programming
*   language to the MS-DOS/PC-DOS "ANSI.SYS" device driver
*   used to implement extended screen and keyboard functions.
*   Like any C program, each of the following macros can itself
*   become a building block for a still larger one.  Note the
*   evolution of WINDOW(row1,col1,row2,col2,fill,border) from
*   DRAW(row1,col1,row2,col2,icon) and FILL(row1,col1,row2,col2,fill).
*
*   Please refer to the MS-DOS/PC-DOS Reference Manual and the
*   ANSI.SYS Device Driver commands for the "original" commands
*   and control sequences that are here made into C macros.
*
*   Refer to the IBM Technical Reference Manual or to the
*   appendix of the BASIC Version 2 Reference for the ASCII
*   Character Codes and the Extended Keyboard Function codes.
*
*   Run the ANSIDEMO.EXE for a superb demonstration of all these
*   powerful macros and C programming tools in action.  The
*   actual source code is included in ANSIDEMO.C, an excellent
*   demonstration/introduction to the C programming language.
*
*   Simply #include "ansisys.c" this file in your programs
*   to enable the following "smart" screen and cursor commands
*   to really supercharge your C programs with professional
*   features that are easier, safer and more portable than
*   tacked-on assembly languge routines.
*
*   Remember that C is "case sensitive" so be sure and
*   reference the following macros with CAPITAL LETTERS.
*
**/

#define BEEP                           printf("\007")
        /* 800 Mz tone for 1/4 second -- same as PRINT CHR$(7) */
#define CLEARSCREEN                    printf("\033[2J")
#define CLS                            CLEARSCREEN
        /* clears the screen and positions cursor at top left corner */
        /* "\033" is Octal for "Escape" or ASCII Decimal 27  (CHR$(27)) */
        /* "Escape-[" is the lead-in for the ANSI.SYS code routines */
#define CURSPOS(x,y)                   printf("\033[%u;%uH",(x),(y))
#define XY(x,y)                        CURSPOS(x,y)
        /* positions cursor at x = row, y = column */
#define EOL                            printf("\033[K")
        /* erases to end of line, including cursor position */
        /* NOTE:  error in DOS documentation has 'K' lower case */
#define XYEOL(x,y)                     printf("\033[%u;%uH\033[K",(x),(y))
        /* positions cursor at x,y then erases to end of line */
#define XYWHERE        printf("\033[6n");scanf("%*1c%2d%*1c%2d%*2c",&row,&col)
        /* requests cursor position, device driver answers row,col--declare int */
#define CURSUP(x)                      printf("\033[%uA",(x))
#define CURSDWN(x)                     printf("\033[%uB",(x))
        /* cursor up or down x-many lines */
#define CURSFWD(y)                     printf("\033[%uC",(y))
#define CURSBCK(y)                     printf("\033[%uD",(y))
        /* cursor forward (right) or backward (left) y-many spaces */
#define SAVCURS                        printf("\033[s")
#define RECALLCURS                     printf("\033[u")
        /* cursor position is saved for later recall via RECALLCURS */
#define CPR(x,y,z)                     printf("\033[%u;%uH%c",(x),(y),(z))
#define XYCHAR(x,y,z)                  CPR(x,y,z)
        /* position cursor at x,y and print char z (using ASCII code) */
#define XCTRPRINTF(x,str)   printf("\033[%u;%uH%s",(x),((80-(strlen(str)-1))/2),str)
        /* on row x, center (and printf) the string str (in double quotes) */
#define CURSPOSPRTF(x,y,str)           printf("\033[%u;%uH%s",(x),(y),str)
#define XYPRINTF(x,y,str)              CURSPOSPRTF(x,y,str)
        /* at position x,y printf the string str (in double quotes) */
#define XKREAD(x)                      x=0;x=bdos(1);if (bdos(11)) x=bdos(8)+128
        /* extended code keyboard read, reads function keys, arrow keys, etc. */
     /* NOTE:  Lattice C 2.15/3.00 bdos() works like this--others don't: caution */
#define XKREADE(x)                     x=0;x=bdos(1);if (bdos(11)) x=bdos(1)+128
        /* same as XKREAD(), except this one echoes the input on the screen */
#define CHKBRK                          if (key==196) break
        /* if F10 key was pressed, break out of loop */
#define SETSCREEN(a)                   printf("\033[=%uh",a)
     /* set screen graphics mode */
     /* 0=40x25 monochrome,1=40x25 color,2=80x25 mono,3=80x25 color,        */
     /* 4=320x200 color,5=320x200 mono,6=640x200 mono,7=enable word-wrap.   */
#define RESETSCREEN(a)                 printf("\033[=%ul",a)
     /* reset screen graphics mode */
     /* the attributes are same as SETSCREEN(a) except 7=disables word-wrap */
#define SETDISPLAY(a,b,c)              printf("\033[%u;%u;%um",a,b,c)
     /* set screen display attributes and colors = (a,b,c) any order:       */
     /* 0 = default, 1 = high intensity, 4 = underline,                     */
/* 5=blinking,7=inverse,8=invisible (black-on-black),30=foreground black,   */
/* 31=fore red,32=fore green,33=fore yellow,34=fore blue,35=fore magenta,   */
/* 36=fore cyan,37=fore white,40=background black,41=back red,42=back green,*/
/* 43=back yellow,44=back blue,45=back magenta,46=back cyan,47=back white.  */
#define HLON                           SETDISPLAY(0,0,1)
        /* set high light (high intensity) on */
#define BLON                           SETDISPLAY(0,0,5)
        /* set blinking on */
#define HLOFF                          SETDISPLAY(0,0,0)
#define BLOFF                          HLOFF
        /* set high intensity, blink (and all other display attributes) to off */
#define PROMPT(x,y,cc)            SETDISPLAY(0,0,7);printf("\033[%u;%uH",(x),(y));\
                                       cc=getchar();SETDISPLAY(0,0,0)
        /* at position x,y read inverse prompt for input cc */
#define XKPROMPT(x,y,z)             HLON;XY((x),(y));printf(" \b");XKREAD(z);HLOFF
        /* at position x,y read highlighted prompt for input z */
#define WINDOW(a,b,c,d,e,f)            DRAW(a,b,c,d,f);FILL(a+1,b+2,c-1,d-2,e)
        /* a rectangle determined by upper left-hand corner coordinates, */
        /* row1 = a, col1 = b, and lower right-hand corner coordinates,  */
        /* row2 = c, col2 = d, is filled with extended graphics character */
        /* ASCII decimal code e, and the border is ASCII decimal code f  */
#define WINDOW2(a,b,c,d,e,f)        DRAW(a,b,c,d,f);DRAW(a+1,b+1,c-1,d-1,255);\
                                       FILL(a+1,b+2,c-1,d-2,e)
        /* same as WINDOW(a,b,c,d,e,f) except use this one to overwrite other */
        /* drawings because this one fills empty spaces with blanks */

/* ------------------------------------------------------------- */
/** DRAW(row1,col1,row2,col2,icon)                               */
/*                                                               */
/*  can be rectangle, vertical line, horizontal line or point!   */
/*                                                               */
/* row1,col1=Upper Left-hand corner of border                    */
/* row2,col2=Lower Right-hand corner                             */
/* icon=ASCII Decimal number of Character want border made of    */
/*                                                               */
/*   (Note:  Error-trapping is up to you in calling program,     */
/*           e.g., [0<=row<=24], [0<=col<=80], graphics mode,    */
/*           etc.)                                               */
/*                                                               */
/* Dbl Lines=205;Sngl Line=196;Dark=176;Medium=177;Light=178     */
/* White=219;Blank=255;Sunshine=15;Music notes=14;Asterisks=42   */
/* Happy Face=1,2;Hearts=3;Diamonds=4;Clubs=5;Spades=6;Beeps=7   */
/* ------------------------------------------------------------- */
/**/

DRAW(row1,col1,row2,col2,icon)
int row1,col1,row2,col2,icon;
{
   int hlen,hlen2,hlen3,vlen,r,c,hzl,vtl,ulc,llc,urc,lrc;

   hlen=hlen2=hlen3=col2-col1;
   vlen=row2-row1;
   if (hlen<0 || vlen<0) BEEP;         /* audibly alert possible input error */

   if (hlen<=0 && vlen<=0)             /* then it's a point or a corner */
      {
      CPR(row1,col1,icon);
      return(0);
      }

   if (vlen<=0)                        /* then it's a horizontal line */
      {
      CURSPOS(row1,col1);
      while (hlen--)
         printf("%c",icon);
         return(0);
      }

   switch (icon)
      {
      case 196:                        /* for Single line border */
      case 218:
                hzl=196;vtl=179;ulc=218;llc=192;urc=191;lrc=217;
                break;
      case 201:                        /* for Double line border */
      case 205:
                hzl=205;vtl=186;ulc=201;llc=200;urc=187;lrc=188;
                break;
      case 213:                        /* for Double top, single side */
                hzl=205;vtl=179;ulc=213;llc=212;urc=184;lrc=190;
                break;
      default:
                hzl=vtl=ulc=llc=urc=lrc=icon;  /* for same char all around */
      }

   if (hlen<=0)              /* it's a vertical line -- use vtl from above */
      {
      CURSPOS(row1,col1);
      for (r=row1;r<=row2;r++)
         CPR(r,col1,vtl);
         return(0);
      }
                             /* if it's fallen through this far it's a rectangle */
   CURSPOS(row1,col1);
   while (hlen--)            /* print horizintal icon top row, left to right */
      printf("%c",hzl);
   CPR(row1,col2,urc);       /* print upper right-hand corner */
   for (r=row1+1;r<row2;r++) /* print vertical right-hand column, top to bottom */
     CPR(r,col2,vtl);
   CPR(row2,col2,lrc);       /* print lower right-hand corner */
   CURSPOS(row2,col2-1);
   while (hlen2--)           /* print horizontal bottom row, right to left */
      printf("%c\b\b",hzl);  /* one forward, two back (NOTE: this is slow) */
   CPR(row2,col1,llc);       /* print lower left-hand corner */
   for (r=row2-1;r>row1;r--) /* print vertical left-hand column, bottom to top */
     CPR(r,col1,vtl);
   CPR(row1,col1,ulc);       /* print upper left-hand corner to complete object */
   return(0);
}                            /* end DRAW() function */

/* ------------------------------------------------------------- */
/** FILL(row1,col1,row2,col2,icon)                               */
/*                                                               */
/*  can be "window," vertical line, horizontal line or point!    */
/*                                                               */
/* row1,col1=Upper Left-hand corner of area-to-be-filled         */
/* row2,col2=Lower Right-hand corner                             */
/* icon=ASCII Decimal number of Character want area filled with  */
/*                                                               */
/*   (Note:  Error-trapping is up to you in calling program,     */
/*           e.g., [0<=row<=24], [0<=col<=80], graphics mode,    */
/*           etc.                                                */
/*                                                               */
/* Dbl Lines=205;Sngl Line=196;Dark=176;Medium=177;Light=178     */
/* White=219;Blank=255;Sunshine=15;Music notes=14;Asterisks=42   */
/* Happy Face=1,2;Hearts=3;Diamonds=4;Clubs=5;Spades=6;Beeps=7   */
/* ------------------------------------------------------------- */
/**/

FILL(row1,col1,row2,col2,icon)
int row1,col1,row2,col2,icon;
{
   int hlen,hlen1,vlen,r,c;

   hlen=hlen1=col2-col1;
   vlen=row2-row1;
   if (hlen<0 || vlen<0) BEEP;  /* audibly alert possible input error */

   for (r=row1;r<=row2;r++)
     {
     hlen=hlen1+1;
     CURSPOS(r,col1);
       {
       while (hlen--)
         printf("%c",icon);
       }
     }
   return(0);
}                            /* end FILL() function */
