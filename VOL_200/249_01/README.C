/*
**  R E A D M E . C
**
**  A program to allow quick and easy viewing of a README.DAT text file
**  and to act as a sample program for the C Spot Run C Add-On Library.
**
**  Copyright 1987 Bob Pritchett.   All Rights Reserved.
**
**  History:
**
**  07/09/87 RDP Created for the release of Version 2.1 of C Spot Run.
**
*/

/*
**  Notes:  Since we only save the cursor once, we know that save_cursor()
**          saves the cursor in pointer 0, thus we needn't save the return
**          value of save_cursor(), and can restore it with restore_cursor(0).
**
**          The variable wmax holds the number of rows in the window, so
**          if the window size is changed, no 'hard wired' integers need to
**          be changed elsewhere in the program.
**
*/

#include <stdio.h>
#include <ctype.h>
#include <csrmisc.h>
#include <color.h>
#include <skey.h>

#define	MAX_LINES	500		/* Maximum Lines in README.DAT */

FILE *data;				/* File Pointer to README.DAT */

int c;					/* All Purpose Input Character */
int x;					/* All Purpose Integer */
int w;					/* Window for Text */

int wmax;				/* Maximum Row in Window */

char line[MAX_LINES][80];		/* 500 80 Character Lines */

int maxline;				/* Highest Line Number */

int curline;				/* Current Line Number */

int ol;					/* Old Line Number */

main()					/* Main Program Entry */
 {
  if ( ! ( data = fopen("README.DAT","r") ) )	/* If Unable to Open File */
   {
    printf("Unable to open README.DAT for reading.\n");	/* Print Message */
    exit(1);				/* Exit with Errorlevel One */
   }
  save_screen();			/* Save the User's Screen */
  save_cursor();			/* Save Cursor Location */
  cursor_off();				/* Turn Off Cursor */
  cls();				/* Clear the Screen */
  wcolor(LGRY_F,WHT_F);			/* Set Colors For Text and Window */
  w = wopen(0,0,20,79,3);		/* Open the Text Window */
  wmax = wrow(w) + 1;			/* See Notes */
  wtitle(w,"µ README.DAT Æ",1);		/* Title Window */
  maxline = 0;				/* Initialize Maximum Line Count */
  while ( fgets(line[maxline],79,data) )/* While We Can Read Data */
   {
    x = strlen(line[maxline]);		/* Get String Length */
    if ( x )				/* If Positive Length */
    line[maxline][x-1] = 0;		/* Put Null Over NewLine */
    if ( ++maxline == MAX_LINES )	/* If At Maximum Line Number */
       break;				/* Break Out of Loop */
   }
  fclose(data);				/* Close Data File */
  ccenter(22,"Use the Arrow keys, Home, End, PgUp and PgDn.",WHT_F);
  ccenter(24,"Copyright 1987 Bob Pritchett, New Dimension Software.",WHT_F);
  process();				/* Process Commands */
  wclose(w);				/* Good Habit to Close Windows */
  cursor_on();				/* Turn On Cursor */
  restore_cursor(0);			/* We Know Cursor Pointer 0 was Used */
  restore_screen();			/* Restore User's Screen */
 }

display()				/* Display Text in Window */
 {
  wcls(w);				/* Clear Window */
  for ( x = curline; x < maxline && ( x - curline ) < wmax; ++x )
      wputat(w,x - curline,0,line[x]);	/* And Display Text */
 }

process()				/* Process Input and Display */
 {
  display();				/* Display First Screen of Text */
  while ( 1 )				/* Go Into Perpetual Loop */
   {
    ol = curline;			/* Set Old Line */
    c = getch();			/* Get a Character */
    if ( c == 27 )			/* If it is an ESCape */
       break;				/* Break Out of the Loop */
    else if ( c == 13 )			/* If it is a Carriage Return */
       curline = min(maxline - wmax,curline + wmax);	/* Find New Place */
    else if ( c == 00 )			/* If it is a Null */
     {
      c = getch();			/* Get the Extended Key */
      if ( c == PGUP )			/* If PageUp */
         curline = max(0,curline - wmax);	/* Find New Place */
      else if ( c == PGDN )		/* If PageDown */
         curline = min(maxline - wmax,curline + wmax);	/* Find New Place */
      else if ( c == HOME )		/* If Home */
         curline = 0;			/* Set to Start */
      else if ( c == END )		/* If End */
         curline = max(0,maxline - wmax);	/* Go to Last Page */
      else if ( c == UARROW || c == DARROW )	/* If Single Line Movement */
       {
        if ( c == UARROW )		/* If Up Arrow */
         {
          if ( curline > 0 )		/* If More Text */
           {
            winsert(w,0,1);		/* Insert a Line */
            wputat(w,0,0,line[--curline]);	/* Put Text */
            continue;			/* Continue Loop */
           }
         }
        else if ( c == DARROW )		/* If Down Arrow */
         {
          if ( ( curline + wmax ) < maxline )	/* If More Text */
           {
            ++curline;			/* Increment Current Line */
            wdelete(w,0,1);		/* Delete First Line */
            wputat(w,wmax - 1,0,line[curline + wmax - 1]);  /* Put Text */
            continue;			/* Continue Loop */
           }
         }
       }
      else				/* If Not a Valid Key */
       {
        beep();				/* Beep */
        continue;			/* Continue Loop */
       }
     }
    else				/* If Not a Valid Key */
     {
      beep();				/* Beep */
      continue;				/* Continue Loop */
     }
    if ( ol != curline )		/* If Curline Changed */
       display();			/* Update Display */
   }
 }
