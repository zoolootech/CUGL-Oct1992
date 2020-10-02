/* BOOTDEMO.C - Demo program for CTRLCLIB boot functions */

/*
** CTRLCLIB - The CTRL-C Library(Tm)
** Copyright 1990 Trendtech Corporation, Inc.
** All Rights Reserved Worldwide
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>

#ifdef __TURBOC__
#define CLEAR_SCREEN     clrscr()
#define _outtext(x)         printf("%s",(x))
#define _settextposition(r,c)         gotoxy(c,r) /* yes, Microsoft has it backwards */
#define _settextwindow(r1,c1,r2,c2)   window(r1,c1,r2,c2)
#define getch            getche
#define DoTimeDelay(x)   delay(x + 50);
#else
#include <graph.h>
#define CLEAR_SCREEN     _clearscreen(_GCLEARSCREEN)
#endif

#include <dos.h>
#include <bios.h>
#include <time.h>
#include <string.h>

#include "ctrlc.h"   /*  <-- this header must be in every source module */
                     /*      calling a CTRL-C LIBRARY function. */

#define HELL_IS_BURNING 1


char *cr[]={"CTRLCLIB - The CTRL-C Library(Tm)",
            "Copyright 1990 Trendtech Corporation, Inc.",
            "All Rights Reserved Worldwide" };


/*
===================================================================
= Function Prototypes
===================================================================
*/

int BOOTTEST(void);
void IntroScreen(void);
static void IntroBoot(void);

/*
===================================================================
= Function Prototypes
===================================================================
*/

static void footer(void);

#ifndef __TURBOC__
static unsigned DoTimeDelay(int);
#endif



/*
===================================================================
= Variables
===================================================================
*/

#ifndef __TURBOC__
struct dostime_t t, *pt;
time_t tm;
#endif

char s[80], *sp;
unsigned int c, i, scn;

/*
===================================================================
=           M A I N  
===================================================================
*/

void main(void)
   {

#ifdef __TURBOC__
   delay(0);
#else
   time(&tm);
   srand((unsigned)tm);
#endif

   IntroScreen();
   IntroBoot();

   BOOTTEST();

   CLEAR_SCREEN;
   exit(0);
   }


/*********************************************************************/

int BOOTTEST(void)
   {

   static char far *txt[]={
"\n",
"                  Test of function:  SystemReset(xxxx)\n",
"\n",
"Run this test twice, once for cold and once for warm.  Be sure to type\n",
"the characters as shown; in either UPPER or lower case.\n",
"\n",
"There will be a short pause just before the system resets.....\n",
"\0"  }; 

   i = c = 0;
   CLEAR_SCREEN;
   _settextwindow(1,1,14,80);
   _outtext("CTRLCLIB - The CTRL-C Library(Tm)");
   _outtext(" - Copyright 1990 - Trendtech Corporation\n");
   _settextposition(5,1);
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   while(HELL_IS_BURNING)
      {
      _settextwindow(15,1, 24,80);
      _outtext("Please answer this question......\n\n\n\n");
      _outtext ("       Do you want a COLD (w/diagnostics),\n");
      _outtext ("                     WARM (ctrl+alt+del),\n");
      _outtext ("                  or NO system reset?\n\n");
      _outtext ("                                          (C/W/n/q): ");
      c = getche();
      c = toupper(c);
      if ( c == 'C' )
         { i = 1; break; }
      if ( c == 'W' )
         { i = 2; break; }
      if ( c == 'N' || c == 'Q' )
         { i = 0; break; }
      }

   _settextwindow(15,1, 24,80);
   _outtext("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"); /* clear window */

   if (i == 1)
      {
      _outtext("\nThe following CTRLCLIB function will COLDBOOT your system...\n\n\n");
      _outtext("                   SystemReset(COLDBOOT);\n\n\n\n");
      DoTimeDelay(400);
      SystemReset(COLDBOOT);   /*  <<------****** CTRL-C LIBRARY Function ****** */
      }
   if (i == 2)
      {
      _outtext("\nThe following CTRLCLIB function will WARMBOOT your system...\n\n\n");
      _outtext("                   SystemReset(WARMBOOT);\n\n\n\n");
      DoTimeDelay(300);
      SystemReset(WARMBOOT);   /*  <<------****** CTRL-C LIBRARY Function ****** */
      }

   _outtext("\n\n\n   You elected NOT to reboot your system at this time.\n\n");
   _outtext("Press any key...\n\n\n");
   return(getch());
   }


/*********************************************************************/

static void IntroBoot(void)
   {
   int i;
   static char *boottext[] = {
"                      ***   SYSTEM RESET TEST   ***",
" ",
"This test illustrates the system reset function available with CTRLCLIB.",
"With the SystemReset() function, you can force either a WARM BOOT or a",
"COLD BOOT.  A warm boot is equivalent to the user pressing the",
"CTRL+ALT+DEL keys.  A cold boot performs complete system diagnostics",
"before booting; the same as if they turned on the power switch.",
" ",
"To fully test CTRLCLIB's SYSTEM RESET function, you should run this test",
"at lease twice,  once for the COLD boot and once for the WARM boot.",
" ",
"Simply answer the question on the next screen to see this function in action.",
"Fear not; there's also an option to quit.\n ",
"                      Press any key to continue...",
"\0"  }; 

   CLEAR_SCREEN;
   _settextwindow(1,1, 25,80);
   _settextposition(1,1);
   _outtext("CTRLCLIB - The CTRL-C Library(Tm)");
   _outtext(" - Copyright 1990 - Trendtech Corporation\n");
   _settextposition(4,1);

   i = 0;
   while ( boottext[i][0] != '\0')
      printf(" %s\n",boottext[i++]);

   getch();

   return;
  }


/*********************************************************************/

static void footer(void)
   {
   static char st[]=
"==========================================================================";
   _settextposition(14,1);
   _outtext(st);
#ifdef __TURBOC__
   printf("\n");
#endif
   return;
   }


/*********************************************************************/

#ifndef __TURBOC__
static unsigned DoTimeDelay(int timedelay)
   {
   unsigned accumhun, oldhun;

   accumhun = 0;
   _dos_gettime(&t);
   pt = &t;

   while(HELL_IS_BURNING)
      {
      oldhun = pt->hsecond;
      _dos_gettime(&t);
      pt = &t;
      accumhun = accumhun + (((100 - oldhun) + pt->hsecond) % 100);
      if (accumhun >= (unsigned)timedelay)
         break;                 /* exit when time elapsed */
      }

   return(accumhun);
   }
#endif




/* --------------------------- End of Source ----------------------- */





