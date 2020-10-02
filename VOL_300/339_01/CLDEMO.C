/* CLDEMO.C - Demo program for  THE CTRL+C LIBRARY functions */

/*
** CTRLCLIB - The CTRL+C Library(Tm)
** Copyright 1990 Trendtech Corporation, Inc.
** All Rights Reserved Worldwide
*/

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

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
                     /*      calling a CTRLCLIB function. */

/* some neat boolean logic conditions */        
#define FALSE   0
#define TRUE    ~FALSE
#define NO      FALSE
#define YES     ~NO
#define OFF     FALSE
#define ON      ~OFF


#define HELL_IS_BURNING 1
#define QUIT  (((char)c=='q')||((scn==0x10)&&((char)c=='Q')))


char *cr[]={"CTRLCLIB - The Ctrl+C Library(Tm)",
            "Copyright 1990 Trendtech Corporation, Inc.",
            "All Rights Reserved Worldwide" };


/*
===================================================================
= Function Prototypes
===================================================================
*/

int TEST1(void);
int TEST2(void);
int TEST3(void);
int TEST4(void);
int TEST5(void);
int TEST6(void);
int TEST7(void);
int TEST8(void);
int TEST9(void);
int TEST10(void);
int TEST11(void);
int TEST12(void);
int TEST13(void);
int TEST14(void);
int TEST15(void);
int TEST16(void);
int TEST17(void);
int TEST18(void);
int TEST19(void);
int TEST20(void);
int TEST21(void);

/*
===================================================================
= Function Prototypes
===================================================================
*/

static void header(char *);
static void footer(void);

#ifndef __TURBOC__
static unsigned DoTimeDelay(int);
#endif

static unsigned int ReadChar( char * );
static char *spc(void);

void IntroScreen(void);
void FinalScreen(void);

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

   while(HELL_IS_BURNING)
      {
      if (TEST1()  == 'Q') break;
      if (TEST2()  == 'Q') break;  
      if (TEST3()  == 'Q') break;  
      if (TEST4()  == 'Q') break;
      if (TEST5()  == 'Q') break;
      if (TEST6()  == 'Q') break;
      if (TEST7()  == 'Q') break;
      if (TEST8()  == 'Q') break;
      if (TEST9()  == 'Q') break;
      if (TEST10() == 'Q') break;
      if (TEST11() == 'Q') break;
      if (TEST12() == 'Q') break;
      if (TEST13() == 'Q') break;
      if (TEST14() == 'Q') break;
      if (TEST15() == 'Q') break;
      if (TEST16() == 'Q') break;
      if (TEST17() == 'Q') break;
      if (TEST18() == 'Q') break;
      if (TEST19() == 'Q') break;
      if (TEST20() == 'Q') break;
      if (TEST21() == 'Q') break;
      break;
      }

   CLEAR_SCREEN;
   FinalScreen();
   exit(0);
   }


/*********************************************************************/

int TEST1(void)
   {
   static char far *txt[]={
"This test illustrates COMPLETE trapping of the infamous ^C break.\n",
"Using functions from CTRLCLIB, you can trap the <CTRL>+<C> and\n",
"<CTRL>+<BREAK> keystrokes, as well as eliminate the ^C characters from\n",
"echoing to STDOUT when either of these keys are pressed by the user.\n\n",
"          PRESS  q  to quit this test, or  Q  to quit the demo...\n",
""};
   
   c = i = 0;
   CLEAR_SCREEN;
   header("TEST 1 - Demonstrate Full CTRL+C and CTRL+BREAK trapping\n\n");
   do { printf("%s",txt[i]); } while (*txt[++i]);

   printf("\nPress any key, or try <CTRL>+<C> -or- <CTRL>+<BREAK>...\n\n");
   
   FullCtrlC(DISABLE);          /*  <<--*** CTRLCLIB Function *** */
   
   while(1)
      {
      if ( (i = QueryCtrlC()) != 0 ) /*  <<--*** CTRLCLIB Function *** */
         {
         switch (i)
            {
            case CTRLC: 
               printf(" You pressed the <CTRL>+<C> keys\n");
               break;
            case CTRLBREAK:
               printf(" You pressed the <CTRL>+<BREAK> keys\n");
               break;
            case CTRL2:
               printf(" You pressed the <CTRL>+<2> keys\n");
               break;
            case ALT3:
               printf(" You pressed the <ALT>+<kp-3> keys\n");
               break;
            default:
               printf(" Whoops, this should never happen!!\n");
               break;
            }
         printf("     Press any key...\n");
         continue;
         }
         else
            if ( !kbhit() ) continue;

      if ( (c = ReadChar(s)) == 0)       /*  read and print the char typed */
         continue;                       /*  (not a printable char) */
      printf(s);
      if (QUIT) break;
      }

   FullCtrlC(ENABLE);           /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      printf("\nCTRL+C and CTRL+BREAK have been ENABLED;  NOW press either set of keys to\n");
      printf("'break' out of this demo program if you wish; -or- press any key to\n");
      printf("continue the next test...");
      getch();
      }

   return(c);
   }


/*********************************************************************/

int TEST2(void)
   {
   static char far *txt[]={
"This test illustrates how DOS normally echos a ^C to the screen when\n",
"either <CTRL>+<C> or <CTRL>+<BREAK> is pressed.  Control-break is NOT\n",
"trapped in this test, so it too acts like control-c.  \n\n",
"       PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};
   
   c = i = 0;
   CLEAR_SCREEN;
   header("TEST 2- Disable CTRL+C and illustrate ^C using DOS print functions");
   do { printf("%s",txt[i]); } while (*txt[++i]);

   _settextposition(15,1);
   printf("Now press any key, or try <CTRL>+<C> and <CTRL>+<BREAK>...\n\n");
   

   CtrlC(DISABLE);              /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      i = QueryCtrlC();         /*  <<--*** CTRLCLIB Function *** */
      if (i == CTRLC)  /*  this is TRUE when either CTRL+C -or- CTRL+BREAK is hit */
         {
         printf(" You pressed the <CTRL>+<C> keys\n");
         printf("     Press any key...\n");
         continue;
         }
       else if (i == CTRLBREAK) /*  this will NEVER be true (in this demo section) */
           {
           printf(" You pressed the <CTRL>+<BREAK> keys\n");
           printf("     Press any key...\n");
           continue;
           }
         else
             if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)           /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      printf(s);
      if (QUIT) break;
      }

   CtrlC(ENABLE);               /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      printf("\n\n\n\nCTRL+C has been ENABLED; press any key ");
      printf("to continue to the next test...\n\n");
      getch();
      }

   return(c);
   }



/*********************************************************************/

int TEST3(void)
   {
   static char far *txt[]={
"This test shows how the installed interrupt handlers set a flag in the \n",
"CTRLCLIB communication area to indicate which set of keys were pressed.\n",
"In the case of pressing <CTRL>+<C>, we will still get the ^C characters \n",
"echoed to the screen.  But in the case of pressing <CTRL>+<BREAK>, we will\n",
"NOT get a ^C echoed.  The installed interrupt handlers are aware of the\n",
"tests installed and will set the flag accordingly.  ALSO note that the ^C\n",
"characters are printed on the screen by DOS when you use DOS functions.\n",
"But for <CTRL>+<BREAK> traps, no ^C will be echoed to the screen because\n",
"this test installs a control-break handler in addition to control-c.\n\n",
"          PRESS  q  to quit this test, or  Q  to quit the demo...\n",
""};
   
   c = i = 0;
   CLEAR_SCREEN;
   header("TEST 3 - Example of disabling CTRL+BREAK *AND* CTRL+C (with ^C echo)\n\n");
   do { printf("%s",txt[i]); } while (*txt[++i]);

   printf("\nNow press any key, or try <CTRL>+<C> -or- <CTRL>+<BREAK>...\n");
   

   CtrlBreak(DISABLE);          /*  <<--*** CTRLCLIB Function *** */
   CtrlC(DISABLE);              /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      i = QueryCtrlC();         /*  <<--*** CTRLCLIB Function *** */
      if (i == CTRLC) 
         {
         printf(" You pressed the <CTRL>+<C> keys\n");
         printf("     Press any key...\n");
         continue;
         }
       else if (i == CTRLBREAK)
           {
           printf(" You pressed the <CTRL>+<BREAK> keys\n");
           printf("     Press any key...\n");
           continue;
           }
         else
             if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)           /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      printf(s);
      if (QUIT) break;
      }

   CtrlBreak(ENABLE);           /*  <<--*** CTRLCLIB Function *** */
   CtrlC(ENABLE);               /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      printf("\n\n\nCTRL+C and CTRL+BREAK have been ENABLED again;  now\n");
      printf(" press any key to continue with the next text...\n\n");
      getch();
      }

   return(c);
   }



/*********************************************************************/

int TEST4(void)
   {
   static char far *txt[]={
"Try to 'break' out of this program by first pressing and holding down\n",
"the CONTROL key and then pressing the BREAK key.\n\n",
"**NOTE: DO NOT PRESS <CTRL>+<C> for this test because the handler\n",
"for this key combination has NOT been installed.  You'll leave\n",
"the <CTRL>+<BREAK> handler still active in memory and you'll have to reboot!\n\n",
"         PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 4 - Example of disabling CONTROL+BREAK only");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try <CTRL>+<BREAK>...\n\n");
   

   CtrlBreak(DISABLE);          /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      i = QueryCtrlC();         /*  <<--*** CTRLCLIB Function *** */
      if (i == CTRLBREAK)
         {
         _outtext(" You pressed the <CTRL>+<BREAK> keys\n");
         _outtext("     Press any key...\n");
         continue;
         }
       else
         if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)           /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   CtrlBreak(ENABLE);           /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\n\n\n\nCTRL+BREAK has been ENABLED; press any key ");
      _outtext("to continue to the next test...\n\n");
      getch();
      }
      
   return(c);
   }

/*********************************************************************/

int TEST5(void)
   {
   static char far *txt[]={
"This test illustrates the blocking of the system reset <CTRL>+<ALT>+<DEL> keys.\n\n",
"**NOTE: DO NOT PRESS <CTRL>+<C> OR <CTRL>+<BREAK> here because those interrupt\n",
"handlers have been ENABLED again from the prior tests.\n\n",
"         PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 5 - Example of disabling system reset - <CTRL>+<ALT>+<DEL>");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try <CTRL>+<ALT>+<DEL>...\n\n");
   
   CtrlAltDel(DISABLE); /* <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if (cl.BootFlag)  
          {
          _outtext("WHOOPS! You pressed the <CTRL>+<ALT>+<DEL> keys\n");
          _outtext("Press any key...\n");
          cl.BootFlag = 0;
          continue;
          }
      if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)         /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   CtrlAltDel(ENABLE); /* <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\n\nCTRL+ALT+DEL has been ENABLED;  You can press <CTRL>+<ALT>+<DEL> to\n");
      _outtext("REBOOT your system now; -or- just press any key to continue the next test..");
      getch();
      }
      
   return(c);
   }



/*********************************************************************/

int TEST6(void)
   {
   static char far *txt[]={
"This test illustrates the blocking of the system PAUSE keys.\n\n",
"On systems equipped with the Enhanced keyboard, the key labeled\n",
"PAUSE will pause the system.  On systems equipped with other keyboards\n",
"the system pause keys are the <CTRL>+<NUMLOCK> keys.\n\n",
"         PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 6 - Example of disabling PAUSE  -or- <CTRL>+<NUMLOCK>");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);

   _outtext("Now press any key to start, then try <PAUSE> or <CTRL>+<NUMLOCK>\n\n");
   
   CtrlcLibSetup( FCTRLC | PAUSEKEY ); /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if (cl.PauseFlag)
          {
          _outtext("WHOA! You tried to PAUSE the system!! \n");
          _outtext("          Press any key...\n");
          cl.PauseFlag = 0;
          continue;
          }
                        /*  if accidently hit any <CTRL>+<BREAK> keys */
      if ( QueryCtrlC() )       /*  <<--*** CTRLCLIB Function ** */
           {
           _outtext(" You're NOT supposed to press the BREAK keys; just the PAUSE key...\n");
           _outtext("     Press any key...\n");
           continue;
           }
      if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)         /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   CtrlcLibCleanup();              /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\n\nPAUSE keys have been ENABLED; press any key to continue\n");
      _outtext("with the next test...");
      getch();
      }
      
   return(c);
   }


/*********************************************************************/

int TEST7(void)
   {
   static char far *txt[]={
"This test illustrates blocking of the DOS screen output pause <CTRL>+<S> keys.\n",
"Pressing <CTRL>+<S> on all MS-DOS systems will pause screen output ONLY.  It\n",
"will not pause the system as <PAUSE> or <CTRL>+<NUMLOCK> will do.\n\n",
"         PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 7 - Example of disabling the MS-DOS screen output pause <CTRL>+<S>");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);

   _outtext("\nNow press any key to start, then try <CTRL>+<S>\n");
   ReadChar(NULL);

   CtrlS(DISABLE);             /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      DoTimeDelay(10);
      sprintf(s,"%sWhile I'm scrolling, press any key, or try to pause with <CTRL>+<S>\n",spc());
      _outtext(s);
      if (cl.PauseFlag == CTRLS)
          {
          _outtext("\n\n      WHOA! You tried to PAUSE MS-DOS screen output!!\n");
          DoTimeDelay(50);
          _outtext("\n\n");
          cl.PauseFlag = 0;
          continue;
          }
      if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)         /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   CtrlS(ENABLE);              /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\n\n<CTRL>+<S> keys have been ENABLED again; press any key to continue\n");
      _outtext("with the next test...");
      getch();
      }
      
   return(c);
   }




/*********************************************************************/

int TEST8(void)
   {
   static char far *txt[]={
"This test illustrates blocking of all pause keys; <CTRL>+<S> and <PAUSE>\n",
" -or- <CTRL>+<NUMLOCK> keys\n\n",
"          PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 8 - Example of disabling ALL pause keys...");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);

   _outtext("\nNow press any key to start, then try <CTRL>+<S> or <PAUSE>\n");
   ReadChar(NULL);

   CtrlcLibSetup( CTRLBREAK | PAUSEKEY | CTRLS );/* <<--*** CTRLCLIB Function *** */

   while(1)
      {
      DoTimeDelay(10);
      sprintf(s,"%sPress any key, or try <CTRL>+<S> -or- <PAUSE> -or- <CTRL>+<NUMLOCK>\n",spc());
      _outtext(s);
      if (cl.PauseFlag == PAUSEKEY)
          {
          _outtext("\n\n      WHOA! You tried to PAUSE the system with the PAUSE key!! \n");
          DoTimeDelay(50);
          _outtext("\n\n");
          cl.PauseFlag = 0;
          continue;
          }
         else if (cl.PauseFlag == CTRLS)
            {
            _outtext("\n\n      WHOA! You tried to PAUSE MS-DOS screen output!! \n");
            DoTimeDelay(50);
            _outtext("\n\n");
            cl.PauseFlag = 0;
            continue;
            }
      i = QueryCtrlC();       /* <<--*** CTRLCLIB Function *** */
      if (i == CTRLBREAK)
           {
           _outtext("\n\n You're NOT supposed to press the <CTRL>+<BREAK> keys; just the pause keys...\n");
           _outtext("     Press any key to continue...\n\n");
           c = getch();
           continue;
           }
         
      if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)         /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   CtrlcLibCleanup();              /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\n\nThe pause keys have been ENABLED again; press any key to continue\n");
      _outtext("with the next test...");
      getch();
      }
      
   return(c);
   }

/*********************************************************************/

int TEST9(void)
   {
   static char far *txt[]={
"This test illustrates how to block the PRINT SCREEN function.  This happens\n"
"when the user presses the key or keys that cause the entire screen to be \n",
"echoed to the printer.  These keys are either the specific 'Print Scrn' key\n",
"on the 101 Key Enhanced Keyboard, or the combination keys <SHIFT>+<PRTSCRN>\n",
"keys on other keyboards.\n\n",
"        PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 9 - Example of disabling <PRTSCRN> screen printout");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try <PRTSCRN>...\n\n");
   

   PrintScreen(DISABLE);      /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if (cl.PrtScrFlag)
         {
         _outtext(" You pressed the <PRTSCRN> screen print keys\n");
         _outtext("     Press any key...\n");
         cl.PrtScrFlag = 0;
         continue;
         }
       else
         if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)         /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   PrintScreen(ENABLE);       /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\n\nScreen print has been ENABLED again;  NOW press <PRTSCN> to\n");
      _outtext("print this screen; or press any key for the next test...\n");
      getch();
      }
      
   return(c);
   }


/*********************************************************************/

int TEST10(void)
   {
   static char far *txt[]={
"This test illustrates how to block the print echo function.  Under MS-DOS,\n",
"pressing certain keys will cause each line appearing on the screen to be\n",
"echoed  to the printer.  These keys are either the <CTRL>+<PRTSCRN> keys\n",
"or the <CTRL>+<P> keys.\n\n",
"       PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 10 - Example of disabling <CTRL>+<PRTSCRN> and <CTRL>+<P> keys");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try <CTRL>+<PRTSCRN> -or- <CTRL>+<P>...\n\n");
   

   CtrlP(DISABLE);            /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if (cl.PrtScrFlag == CTRLPRTSCR)
            {
            _outtext(" You pressed the <CTRL>+<PRTSCRN> screen echo print keys\n");
            _outtext("     Press any key...\n");
            cl.PrtScrFlag = 0;
            continue;
            }
         else if (cl.PrtScrFlag == CTRLP)
               {
               _outtext(" You pressed the <CTRL>+<P> screen echo print keys\n");
               _outtext("     Press any key...\n");
               cl.PrtScrFlag = 0;
               continue;
               }
            else
               if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)         /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   CtrlP(ENABLE);             /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\nScreen echo has been ENABLED again;  press any key to continue\n");
      _outtext("to the next test...");
      getch();
      }
      
   return(c);
   }



/*********************************************************************/

int TEST11(void)
   {
   static char far *txt[]={
"This test illustrates how to block ALL the print screen keys.  These are the\n"
"keys that cause the entire screen to print - <PRTSCRN> -or- <SHIFT>+<PRTSCRN>,\n",
"and also causes each line appearing on the screen to be echoed to the printer.\n",
"The echo keys are either the <CTRL>+<PRTSCRN> keys, or the <CTRL>+<P> keys.\n\n",
"          PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 11 - Example of disabling ALL of the PRINT SCREEN keys.");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try <PRTSCRN> -or- <CTRL>+<PRTSCRN> -or- <CTRL>+<P>...\n\n");


   CtrlcLibSetup( CTRLBREAK | PRINTSCREEN | CTRLP );/*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      i = QueryPrtScr();      /*  <<--*** CTRLCLIB Function *** */
      if (i == PRINTSCREEN)
          {
          _outtext("WHOOPS! You tried to PRINTSCREEN the screen \n");
          _outtext("Press any key...\n");
          continue;
          }
         else if (i == CTRLP)
            {
            _outtext("WHOA! You tried to ECHO to the printer with CTRL+P!!\n");
            _outtext("Press any key...\n");
            continue;
            }
            else if (i == CTRLPRTSCR)
               {
               _outtext("WHOA! You tried to ECHO to the printer with CTRL+PRTSCR!!\n");
               _outtext("Press any key...\n");
               continue;
               }
            else if ( (i = QueryCtrlC()) == CTRLBREAK)
                  {
                  _outtext(" You're NOT supposed to press the <CTRL>+<BREAK> keys; just the print keys...\n");
                  _outtext("     Press any key to continue...\n");
                  c = getch();
                  continue;
                  }
               else
                  if ( ! kbhit()) continue;
         
      if ( (c = ReadChar(s)) == 0)         /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   CtrlcLibCleanup();              /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\n\nAll Screen print functions have been ENABLED again;\n");
      _outtext("press any key to continue to the next test...");
      getch();
      }
      
   return(c);
   }

/*********************************************************************/

int TEST12(void)
   {

   static char far *txt[]={
"This test illustrates how you can set the LOCK keys to an initial\n",
"desired setting.  The user can subsequently change the setting by\n",
"simply pressing the key.  This test sets the CAPS LOCK to ON and the\n",
"NUM LOCK to ON.  Before you manually change the setting, press some alpha keys\n",
"and some keypad numeric keys to verify the settings.  If your keyboard\n",
"has mode lights, they should indicate an ON condition.\n",
""};

   c = 0;
   header("TEST 12 - Example of Initializing LOCK keys");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Press keys to verify settings, then UNLOCK keys and verify again\n\n");

   CapsLock(L_ON);      /*  <<--*** CTRLCLIB Function *** */
   NumLock(L_ON);       /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if ( ! kbhit()) continue;
      if ( (c = ReadChar(s)) == 0)
         continue;                
      _outtext(s);
      if (QUIT) break;
      }

   CapsLock(L_OFF);      /*  <<--*** CTRLCLIB Function *** */
   NumLock(L_OFF);       /*  <<--*** CTRLCLIB Function *** */

   _outtext("     Press any key to continue with the next test...");
   return(getch());
   }


/*********************************************************************/

int TEST13(void)
   {
   static char far *txt[]={
"Notice that as you type letters, they will appear CAPITALIZED.  This\n",
"is because this function will cause the CAPS LOCK key to be in an\n",
"ALWAYS ON state.  No amount of pressing of pounding will turn it off.\n\n",
"Keep pressing the CAPS lock key repeatedly to try to turn it off!\n\n",
"          PRESS  q  anytime to quit this test...\n",
""};

   c = i = 0;
   header("TEST 13 - Keeping the CAPS Lock key *ALWAYS ON* ");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try to reset the CAPS LOCK from ON to off...\n\n");
   
   CapsLock(L_ALWAYSON);      /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if ( ! kbhit()) continue;
      if ( (c = ReadChar(s)) == 0)
         continue;                
      _outtext(s);
      if (QUIT) break;
      }

   CapsLock(L_OFF);           /*  <<--*** CTRLCLIB Function *** */

   _outtext("\n\nThe CAPS LOCK has now been turned OFF;\n");
   _outtext("If you have a keyboard equipped with the mode lights,\n");
   _outtext("notice that you can now TOGGLE the CAPS lock key on and off.\n");
   _outtext("Press any key to continue to the next test...");
   return(getch());
   }

/*********************************************************************/

int TEST14(void)
   {
   static char far *txt[]={
"This test will lock the NUM key to ALWAYS ON.  Notice that as you hit the\n",
"keys on the number pad,  NUMBERS will be echoed back.  It is because\n",
"this function will cause the NUM LOCK key to be in an ALWAYS ON state.\n\n",
"Keep pressing the NUM lock key repeatedly to try to turn it off!\n\n",
"       PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 14 - Keeping the NUM Lock key *ALWAYS ON*");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try to reset the NUM LOCK from ON to off...\n\n");
   
   NumLock(L_ALWAYSON);       /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if ( ! kbhit()) continue;
      if ( (c = ReadChar(s)) == 0)
         continue;                
      _outtext(s);
      if (QUIT) break;
      }

   NumLock(L_OFF);            /*  <<--*** CTRLCLIB Function *** */

   _outtext("\n\nThe NUM LOCK key has now been turned OFF;\n");
   _outtext("If you have a keyboard equipped with the mode lights,\n");
   _outtext("notice that you can now TOGGLE the NUM lock key on and off.\n");
   _outtext("Press any key to continue to the next test...");
   return(getch());
   }


/*********************************************************************/

int TEST15(void)
   {
   static char far *txt[]={
"        Try to reset the NUM LOCK from ON to off...!!!\n\n",
"        Try to set the CAPS LOCK from off to ON...!!!\n\n",
"If your keyboard is not equipped with mode lights, type a letter key or a\n",
"keypad number key after each keypress of either the CAPS lock key or the\n",
"NUM lock key.\n\n"
"       PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 15 - NUM Lock *ALWAYS ON* and CAPS LOCK *ALWAYS OFF* test");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try to turn NUM LOCK OFF, and CAPS LOCK ON\n\n");
   

   CapsLock(L_ALWAYSOFF);     /*  <<--*** CTRLCLIB Function *** */
   NumLock(L_ALWAYSON);       /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      
      if ( ! kbhit()) continue;
      if ( (c = ReadChar(s)) == 0)
         continue;                
      _outtext(s);
      if (QUIT) break;
      }

   NumLock(L_OFF);            /*  <<--*** CTRLCLIB Function *** */
   CapsLock(L_OFF);           /*  <<--*** CTRLCLIB Function *** */

   _outtext("\nThe NUM LOCK key and CAPS LOCK key are now released;\n");
   _outtext("If you have a keyboard equipped with the mode lights,\n");
   _outtext("notice that you can now TOGGLE both the NUM lock key and\n");
   _outtext("CAPS LOCK key on and off.\n");
   _outtext("Press any key to continue to the next test...");
   return(getch());
   }

/*********************************************************************/

int TEST16(void)
   {
   static char far *txt[]={
"        Try to reset the CAPS LOCK from ON to off...!!!\n",
"        Try to reset the NUM LOCK from ON to off...!!!\n",
"        Try to reset the SCROLL LOCK from ON to off...!!!\n\n",
"If your keyboard is not equipped with mode lights, type a letter key or a\n",
"keypad number key after each keypress of either the CAPS lock key or the\n",
"NUM lock key.\n\n"
"       PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 16 - CAPS Lock, NUM Lock, and SCROLL Lock keys *ALWAYS ON* test");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try to turn CAPS, NUM, and SCROLL Lock keys OFF!\n\n");

   CapsLock(L_ALWAYSON);      /*  <<--*** CTRLCLIB Function *** */
   NumLock(L_ALWAYSON);       /*  <<--*** CTRLCLIB Function *** */
   ScrollLock(L_ALWAYSON);    /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if ( ! kbhit()) continue;
      if ( (c = ReadChar(s)) == 0)
         continue;                
      _outtext(s);
      if (QUIT) break;
      }

   NumLock(L_OFF);            /*  <<--*** CTRLCLIB Function *** */
   CapsLock(L_OFF);           /*  <<--*** CTRLCLIB Function *** */
   ScrollLock(L_OFF);         /*  <<--*** CTRLCLIB Function *** */

   _outtext("\nThe CAPS Lock, NUM Lock and SCROLL LOCK keys are now released;\n");
   _outtext("If you have a keyboard equipped with the mode lights,\n");
   _outtext("notice that you can now TOGGLE them on and off\n");
   _outtext("Press any key to continue to the next test...");
   return(getch());
   }

/*********************************************************************/

int TEST17(void)
   {
   static char far *txt[]={
"This function is called in a delay loop to *toggle* the CAPS Lock key\n",
"on and off.  \n\n",
"You don't have to press any keys - just watch the mode light for the\n",
"CAPS Lock key.  If your keyboard doesn't have mode lights, just press \n",
"q  to quit this test.\n\n",
""};

   c = i = 0;
   header("TEST 17 - Toggle the CAPS Lock key ON and OFF" );
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Press the letter  q  anytime to quit this test\n");

   CapsLock(L_OFF);           /*  <<--*** CTRLCLIB Function *** */

   i = 0;
   while(1)
      {

      CapsLock(L_ON);         /*  <<--*** CTRLCLIB Function *** */
      DoTimeDelay(20);
      if (kbhit()) c = getch();
      if (QUIT) break;

      CapsLock(L_OFF);        /*  <<--*** CTRLCLIB Function *** */
      DoTimeDelay(20);
      if (kbhit()) c = getch();
      if (QUIT) break;

      }

   CapsLock(L_OFF);           /*  <<--*** CTRLCLIB Function *** */

   _outtext("\nThe CAPS Lock key has been released;\n");
   _outtext("Press any key to continue to the next test...");
   return(getch());
   }


/*********************************************************************/

int TEST18(void)
   {
   static char far *txt[]={
"These functions are called in a delay loop to *toggle* the CAPS Lock,\n",
"NUM Lock, and SCROLL Lock keys simultaneously causing the mode lights\n",
"to blink on and off.  This is simply a cute test to illustrate the power\n",
"and control your program can have by using the CTRLCLIB library.\n\n",
"You don't have to press any keys - just watch the mode lights for these\n",
"keys.  If your keyboard doesn't have mode lights, you can press q to\n",
"quit this test.\n\n",
""};

   c = i = 0;
   header("TEST 18 - Toggle the CAPS, NUM, SCROLL keys ON and OFF");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("    Press the letter  q  anytime to quit this test\n\n");

   CapsLock(L_OFF);           /*  <<--*** CTRLCLIB Function *** */
   NumLock(L_OFF);            /*  <<--*** CTRLCLIB Function *** */
   ScrollLock(L_OFF);         /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {

      CapsLock(L_ON);         /*  <<--*** CTRLCLIB Function *** */
      NumLock(L_ON);          /*  <<--*** CTRLCLIB Function *** */
      ScrollLock(L_ON);       /*  <<--*** CTRLCLIB Function *** */
      DoTimeDelay(20);
      if (kbhit()) c = getch();
      if (QUIT) break;

      CapsLock(L_OFF);        /*  <<--*** CTRLCLIB Function *** */
      NumLock(L_OFF);         /*  <<--*** CTRLCLIB Function *** */
      ScrollLock(L_OFF);      /*  <<--*** CTRLCLIB Function *** */
      DoTimeDelay(20);
      if (kbhit()) c = getch();
      if (QUIT) break;
      }

   CapsLock(L_OFF);           /*  <<--*** CTRLCLIB Function *** */
   NumLock(L_OFF);            /*  <<--*** CTRLCLIB Function *** */
   ScrollLock(L_OFF);         /*  <<--*** CTRLCLIB Function *** */

   _outtext("\nAll LOCK keys have been released;\n");
   _outtext("Press any key to continue to the next test...");
   return(getch());
   }


/*********************************************************************/

int TEST19(void)
   {
   static char far *txt[]={
"This test illustrates capturing the SYSTEM REQUEST key.  This is the\n"
"key that doesn't appear to be used for anything useful. If your\n",
"application needs to capture the <ALT>+<SYSRQ> key, this function\n",
"will do it for you.\n\n",
"If your keyboard is not equipped with a SysRq key, press q.\n\n"
"         PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 19 - Example of capturing <ALT>+<SYSRQ> keys");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try <ALT>+<SYSRQ>...\n\n");
   
   PrintScreen(DISABLE);      /*  <<--*** CTRLCLIB Function *** */
   SysReq(INSTALL);           /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      i = QuerySysReq();      /*  <<--*** CTRLCLIB Function *** */
      if (i)
          {
          _outtext("   You pressed the <ALT>+<SysRq> keys! \n");
          _outtext("           Press any key...\n");
          continue;
          }
      if ( ! kbhit()) continue;
      if ( (c = ReadChar(s)) == 0)
         continue;                
      _outtext(s);
      if (QUIT) break;
      }

   SysReq(REMOVE);            /*  <<--*** CTRLCLIB Function *** */
   PrintScreen(ENABLE);       /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("\nThe SYS REQ key handler has been removed; press any key to continue\n");
      _outtext("with the next test...");
      getch();
      }
      
   return(c);
   }

/*********************************************************************/

int TEST20(void)
   {
   static char far *txt[]={
"The center key on the keypad is difficult to detect using conventional\n",
"C programming functions.  Part of the CTRLCLIB keyboard handler's\n",
"responsibility is to detect the keypad 5 keypress and set a flag in the\n",
"control structure.  This keypress is detected by the handler regardless\n",
"of the NUMLOCK key setting.\n\n",
"        PRESS  q  to quit this test, or  Q  to quit demo...\n",
""};

   c = i = 0;
   header("TEST 20 - Example of capturing the <KEYPAD 5> keypress");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try <KEYPAD 5>...\n\n");
   
   cl.KeyPad5Flag = 0;          /* make sure it's OFF */
   FullCtrlC(DISABLE);          /*  <<--*** CTRLCLIB Function *** */

   while(1)
      {
      if ( QueryKeyPad5() )  /*  <<--*** CTRLCLIB Function *** */
         {
         _outtext(" You pressed the <KEYPAD 5> key\n");
         _outtext("     Press any key...\n");
         continue;
         }
       else
         if ( ! kbhit()) continue;

      if ( (c = ReadChar(s)) == 0)         /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   FullCtrlC(ENABLE);           /*  <<--*** CTRLCLIB Function *** */

   if (c != 'Q')
      {
      _outtext("Press any key for the next test...\n");
      getch();
      }
      
   return(c);
   }


/*********************************************************************/

int TEST21(void)
   {
   static char far *txt[]={
"This test illustrates the simplicity with which you can use CTRLCLIB\n",
"and the power that it gives your programs.  The single library function\n",
"call shown below will trap the keypress sequences that could cause your\n",
"program a lot of grief:\n",
"     CtrlcLibSetup( FCTRLC | CTRLALTDEL | PAUSEKEY | CTRLPRTSCR \\\n",
"                    PRINTSCREEN | SYSREQ | CTRLS | CTRLP );\n", 
"At the end of this test all of the above traps and interrupts will be cleared\n",
"with the following single function call issued just before program\n",
"termination:   CtrlcLibCleanup(void);\n",
""};

   c = i = 0;
   header("TEST 21 - The CTRLCLIB Library Grand Finale - Trap 'em all...");
   do { _outtext(txt[i]); } while (*txt[++i]);
   footer();

   _settextwindow(15,1, 24,80);
   _outtext("Now press any key, or try these:\n");

   _outtext("     <CTRL>+<BREAK>,  <CTRL>+<C>\n");
   _outtext("     <CTRL>+<2>,      <ALT>+<keypad 3>\n");
   _outtext("     <CTRL>+<S>,      <ALT>+<SYSREQ>\n");
   _outtext("     <PRINTSCREEN> key\n");
   _outtext("     <CTRL>+<PRTSCREEN>,  <CTRL>+<P>\n");
   _outtext("     <PAUSE>  -or- <CTRL>+<NUMLOCK>\n");
   _outtext("     <CTRL>+<ALT>+<DEL>\n");

   _outtext("                       Type Q or q to quit demo...\n");
   

                              /* * CTRLCLIB Function ** */
   CtrlcLibSetup( FCTRLC|CTRLALTDEL|PAUSEKEY|CTRLPRTSCR );
   CtrlcLibSetup( PRINTSCREEN | SYSREQ | CTRLS | CTRLP ); 

   while(1)
      {
      if ( (i = QueryCtrlC()) != 0 )    /*  <<--*** CTRLCLIB Function *** */
         {
         switch (i)
            {
            case CTRLC: 
               _outtext("You pressed the <CTRL>+<C> keys\n");
               break;
            case CTRLBREAK:
               _outtext("You pressed the <CTRL>+<BREAK> keys\n");
               break;
            case CTRL2:
               _outtext("You pressed the <CTRL>+<2> keys\n");
               break;
            case ALT3:
               _outtext("You pressed the <ALT>+<kp-3> keys\n");
               break;
            default:
               _outtext(" Whoops, this should never happen!!\n");
               break;
            }
         _outtext("          Press any key...\n");
         continue;
         }
      if (cl.BootFlag)  
         {
         _outtext("WHOOPS! You pressed the <CTRL>+<ALT>+<DEL> keys\n");
         _outtext("          Press any key...\n");
         cl.BootFlag = 0;
         continue;
         }
      if (cl.PauseFlag == PAUSEKEY)
          {
          _outtext("You pressed the system <PAUSE> key\n");
          _outtext("          Press any key...\n");
          cl.PauseFlag = 0;
          continue;
          }
         else if (cl.PauseFlag == CTRLS)
            {
            _outtext("You pressed the <CTRL>+<S> to pause MS-DOS screen output\n");
            _outtext("          Press any key...\n");
            cl.PauseFlag = 0;
            continue;
            }

      i = QueryPrtScr();      /*  <<--*** CTRLCLIB Function *** */
      if (i == PRINTSCREEN)
          {
          _outtext("You pressed the <PRINTSCREEN> key(s)\n");
          _outtext("          Press any key...\n");
          continue;
          }
         else if (i == CTRLP)
            {
            _outtext("You pressed the <CTRL>+<P> screen echo keys\n");
            _outtext("          Press any key...\n");
            continue;
            }
            else if (i == CTRLPRTSCR)
               {
               _outtext("You pressed the <CTRL>+<PRTSCR> screen echo keys\n");
               _outtext("          Press any key...\n");
               continue;
               }

      if( QuerySysReq() )    /*  <<--*** CTRLCLIB Function *** */
         {
         _outtext("You pressed the <ALT>+<SysRq> keys! \n");
         _outtext("          Press any key...\n");
         continue;
         }

      if ( QueryKeyPad5() )  /*  <<--*** CTRLCLIB Function *** */
         {
         _outtext(" You pressed the <KEYPAD 5> key\n");
         _outtext("     Press any key...\n");
         continue;
         }
         
      if ( !kbhit() ) continue;

      if ( (c = ReadChar(s)) == 0)           /*  read and print the char typed */
         continue;                         /*  (not a printable char) */
      _outtext(s);
      if (QUIT) break;
      }

   CtrlcLibCleanup();              /*  <<--*** CTRLCLIB Function *** */
   return(c);
   }


/*********************************************************************/

static void header(char *st)
   {
   CLEAR_SCREEN;
   _settextwindow(1,1,14,80);
   _outtext("CTRLCLIB - The Ctrl+C Library(Tm)");
   _outtext("   -   Copyright 1990 - Trendtech Corporation\n");
   _settextposition(3,1);
   _outtext(st);
   _settextposition(5,1);
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



/*********************************************************************/

static unsigned int ReadChar( char *sp)
   {
   static unsigned chr;
   int i;

#ifdef __TURBOC__
   chr = bioskey(0);
#else
   chr = _bios_keybrd(_KEYBRD_READ);
#endif

   if (sp == NULL) return(chr);
   scn = (chr >> 8);                    /*  get scan code */
   i =  (unsigned char)(chr & 0x00FF);  /*  strip scan code */

   switch (i)
      {
      case 0x20:
         sprintf(sp,"          You pressed the SPACEBAR\n");
         break;
      case 0x0a:
      case 0x0d:
         sprintf(sp,"          You pressed the <cr> key\n");
         break;
      case 0x1a:
         sprintf(sp,"          You pressed the <^Z> key\n");
         break;
      case 0x08:
         sprintf(sp,"          You pressed the <BKSP> key\n");
         break;
      case 0x09:
         sprintf(sp,"          You pressed the <TAB> key\n");
         break;
      case 0x13:
         sprintf(sp,"          You pressed the <^S> key\n");
         break;
      default:
         sprintf(sp,"          You pressed the %c key\n",i);
         break;
      }

   return((unsigned)i);
   }


/*********************************************************************/

static char *spc(void)
   {
	static char spaces[11]=
         {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
	static int i=0;

   if (++i > 10 ) i = 1;
   spaces[i-1] = ' ';
   spaces[i]   = '\0';
	return(&spaces[0]);
	}


/* --------------------------- End of Source ------------------------- */

