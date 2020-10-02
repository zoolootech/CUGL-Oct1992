/*
**  C S R D E M O . C
**
**  This is a program to demonstrate the use and power of the C Spot Run
**  C Add-On Library.  C Spot Run (CSR) is a large library of routines and
**  programming aids for C programmers who work in the IBM-PC environment.
**  Windowing, screen control, directory access, printer control, BIOS and
**  DOS interfaces, and sophisticated data input are just a few of the many
**  features of this package.
**  C Spot Run is a user-supported package, distributed as "Shareware."  
**  Users may register to receive source code and updates for $50, and a
**  license to use CSR commercially is available for $75, which includes all
**  the benefits of normal registration.  A $15 donation is requested, in
**  exchange for update notifications, from those who use CSR but don't need
**  source code.
**
**  This program is designed to work with Version 2.1 of CSR, available on
**  the BBS listed below and from various BBS's around the United States and
**  Canada.
**
**  Copyright 1987 Bob Pritchett.  All Rights Reserved.
**
**  07/12/87  RDP  Some more work on options, including the windows demo.
**
**  07/09/87  RDP  Working on more options.
**
**  07/05/87  RDP  More work, with sound and new menu routines.
**
**  06/18/87  RDP  Coding begun.  Total replacement of CSRPROMO.C.
**
**    New Dimension Software
**      23 Pawtucket Dr.
**      Cherry Hill, NJ  08003
**      Voice: (609) 424-2595
**       Data: (609) 354-9259 (300/1200/2400B)
**
*/

/* --------------- Include Files ------------------- */

#include <dos.h>			/* For int86() */
#include <color.h>			/* Color Code Definitions */
#include <csrsound.h>			/* Note Values */
#include <csrmisc.h>			/* Miscellaneous CSR Definitions */
#include <skey.h>			/* Special Key Values */

/* --------------- Defined Values ----------------- */

#define NULL	00			/* Define NULL for Readability */
#define MENOPTS	10			/* Number of Options on Menu */

/* --------------- Menu Definition ---------------- */

static char *mmenu[MENOPTS] =
 {
  "General Information",
  "-Specifics Follow",			/* Line With Comment */
  "Windows",
  "Sound",
  "Menuing",
  "Directory Access",
  "Input",
  "Other Features",
  "-More Info",
  "New Dimension Software"
 };

/* --------------- Global Variables --------------- */

static int c;				/* Character Input */
static int sw;				/* Status Window */
static int hw;				/* Help Window */
static int x;				/* Do it all Variable */
static int sc;				/* Saved Cursor */
static int mm;				/* Main Menu Pointer */
static int dtw;				/* Date and Time Window */

static int cbflag = 0;			/* Control-Break Detect Flag */
static int dt = 0;			/* Type for DateTime Update */

static char name[40];			/* Name for Mailing List */
static char title[40];			/* Title for Mailing List */
static char company[40];		/* Company for Mailing List */
static char address[40];		/* Address for Mailing List */
static char city[40];			/* City for Mailing List */
static char state[40];			/* State for Mailing List */
static char zip[40];			/* Zip for Mailing List */
static char phone[40];			/* Phone for Mailing List */
static char comment[40];		/* Comment for Mailing List */

/* --------------- Program Code ------------------- */

wait_cr()				/* Wait for a Carriage Return */
 {
  while ( 1 )				/* Forever */
   {
    if ( kbhit() )			/* If Keyboard Hit */
       if ( getch() == 13 )		/* If A Carriage Return */
          break;			/* Break Out of Loop */
    if ( cbflag )			/* If Control Break Hit */
       handcb();			/* Handle It */
   }
 }

updt_cr()				/* Update Until Carriage Return */
 {
  while ( 1 )				/* Forever */
   {
    dtupdate();				/* Update Time and Date */
    if ( getch() == 13 )		/* If A Carriage Return */
       break;				/* Break Out of Loop */
   }
 }

dtupdate()				/* Update Date and Time */
 {
  int d,mn,y;
  int h,m,s,hs;
  char tmp[40];
  while ( ! kbhit() )
   {
    get_date(&d,&mn,&y);
    get_time(&h,&m,&s,&hs);
    sprintf(tmp," Date: %02d/%02d/%d",mn,d,(y-1900));
    gotoxy(2,62);		/*   We don't want to activate the window */
    ccputs(tmp,BLK_F+GRN_B);	/* for this update, because of the constant */
    sprintf(tmp," Time: %02d:%02d:%02d",h,m,s);	/* change from the menu to */
    gotoxy(3,62);		/* here.  (We want to eliminate unnecessary */
    ccputs(tmp,BLK_F+GRN_B);	/* window manipulation in this situation.) */
    if ( cbflag )			/* If Control-Break Hit */
       handcb();			/* Handle It */
    if ( dt )				/* If Type 1 */
       break;				/* Don't Wait for Key */
   }
 }

main()
 {
  sc = save_cursor();			/* Save the Cursor Position */
  save_screen();			/* Save the Screen */
  cursor_off();				/* Send the Cursor Bye-Bye */
  cls();				/* Clear to Black and White */
  cbcapt(&cbflag);			/* Capture Control-Break */

  color(WHT_F+RED_B);			/* Set Output Colors */
  hw = wopen(20,6,24,74,2);		/* Open Help Window */
  helpwin(0);				/* Display Help Set Zero */
  color(BLK_F+GRN_B);			/* Set Output Colors */
  dtw = wopen(1,61,4,78,1);		/* Open Date and Time Window */
  pmfunc(dtupdate);			/* Set Update Function */
  mm = pmopen(6,26,"´ Main Menu Ã",MENOPTS,mmenu,1);	/* Open Menu */
  while ( 1 )				/* Forever */
   {
    x = pmrun(mm);			/* Run Main menu */
    cursor_off();			/* Cursor Turned on by pmrun() */
    if ( x == -1 )			/* If He/She Wants to Exit */
       break;				/* Go Right Ahead and Break Out */
    switch ( x )			/* Process Option */
     {
      case 0:				/* "General Information" */
        gi();				/* Display General Info */
        break;
      case 2:				/* "Windowing" */
        win();				/* Do Windowing Demo */
        break;
      case 3:				/* "Sound" */
        snd();				/* Display Sound Info and Demo */
        break;
      case 4:				/* "Menuing" */
        mnuing();			/* Display Menuing Information */
        break;
      case 5:				/* "Directory Access" */
        diracc();			/* Display Directory and Info */
        break;
      case 6: 				/* "Input" */
        inpinf();			/* Display Input Information */
        break;
      case 7: 				/* "Other Features" */
        ofinf();			/* Display Other Features Info */
        break;
      case 9:				/* "New Dimension Software" */
        nds();				/* Display NDS Info */
        break;
      default:				/* Otherwise */
        beep();				/* Beep */
        continue;			/* Continue While Loop */
     }
    helpwin(0);				/* Display Help Set Zero */
   }
  quit();				/* Quit */
 }

helpwin(hs)				/* Update Help Window */
 int hs;				/* Help Text Set */
 {
  int lv;				/* Local Value */
  static char *hlp[15] = 		/* Help Text */
   {
    "From the main menu you can select the option you'd like",
    "by using the up and down arrow keys, space and backspace,",
    "or by hitting the first letter of the option's name.",
    "",
    "Hit the enter key when you are ready to go on.",
    "",
    "Please provide the requested information in the fields specified",
    "and hit the escape key when you are finished.  You will then be",
    "given an opportunity to print a mailing list application.",
    "",
    "Hit return to print the form, or <ESC> to return to the menu.",
    "",
    "",
    "Hit any key or wait for the program to continue.",
    ""
   };
  wcls(hw);				/* Clear Help Window */
  for ( lv = 0; lv < 3; ++lv )		/* Three Times */
      wcenter(hw,lv,hlp[(hs*3)+lv]);	/*   Display Help Line */
 }

handcb()				/* Handle Control Break */
 {
  int lv;				/* Local Value */
  beep();				/* Beep for Attention */
  color(YEL_F+RED_B);			/* Colors that Stand Out */
  lv = message("Control-Break Detected:  Hit 'Y' to Exit Program",0);
  if ( toupper(lv) == 'Y' )		/* If 'Y' Hit */
     quit();				/* Quit Program */
  cbflag = 0;				/* Reset Flag */
 }

quit()					/* Quit Program */
 {
  cbrest();				/* Restore Control-Break Handler */
  pmclose(mm);				/* Close Main Menu */
  wclose(dtw);				/* Close Date and Time Window */
  wclose(hw);				/* Close Help Window */
  cursor_on();				/* Bring the Cursor Back */
  restore_screen();			/* Restore the User's Screen */
  restore_cursor(sc);			/* Restore the Cursor Position */
  exit(0);				/* Exit With Errorlevel Zero */
 }

nds()					/* Display NDS Info */
 {
  int lw;				/* Local Window Pointer */
  int lv;				/* Local Value */
  static char *ndsinfo[15] =		/* Text of NDS Information */
   {
    "New Dimension Software is a small company dedicated to producing",
    "quality products for various markets, with a special interest in",
    "both \"Shareware\" software and programming tools.",
    "",
    "At NDS we believe that quality is achieved by hard work and",
    "maintained by support and growth.  We support our products, constantly",
    "making changes and improvements, and we assure future growth by",
    "staying on top of a changing industry.  (Preliminary development",
    "is already underway in the Microsoft Windows environment, providing",
    "a bridge to the coming OS/2.)",
    "",
    "If you would like to talk to us about any of our products, present",
    "or future, please feel free to call.  If you would like to be placed",
    "on our mailing list please fill in the on-screen form you will see",
    "in just a moment and follow the instructions given."
   };
  helpwin(1);				/* Display Help Set One */
  color(WHT_F+BLU_B);			/* Set Colors */
  lw = wopen(0,1,18,78,3);		/* Open the Window */
  wtitle(lw,"µ About New Dimension Software Æ",1);	/* Put Window Title */
  for ( lv = 0; lv < 15; ++lv )		/* Loop Through Text */
      wcenter(lw,lv+1,ndsinfo[lv]);	/* Display The Text */
  cursor_off();				/* Make Sure Cursor is Off */
  wait_cr();				/* Wait for a Carriage Return */
  helpwin(2);				/* Display Help Set Two */
  cursor_on();				/* Turn Cursor On for Input */
  mailapp();				/* Allow User to Input Information */
  cursor_off();				/* Turn Cursor Off After Input */
  helpwin(3);				/* Display Help Set Three */
  while ( 1 )				/* Forever */
   {
    if ( kbhit() )			/* If Keyboard Hit */
     {
      if ( ( c = getch() ) == 27 )	/* If Escape Key */
         break;				/* Break Out of Loop */
      else if ( c == 13 )		/* If Carriage Return */
       {
        printform();			/* Print Form */
        break;				/* Break Out of Loop */
       }
     }
    if ( cbflag )			/* If Control Break Hit */
       handcb();			/* Handle It */
   }
  wclose(lw);				/* Close Local Window */
 }

printform()				/* Print Mailing List Form */
 {
  lprint("\n                     --  NDS Mailing List Application --\n\n");
  lprint("\tPlease place me on New Dimension Software's mailing list.\n\n\n\n");
  lprintf("\t\t\t%s\n",name);
  lprintf("\t\t\t%s\n",title);
  lprintf("\t\t\t%s\n",company);
  lprintf("\t\t\t%s\n",address);
  lprintf("\t\t\t%s, %s  %s\n",city,state,zip);
  lprintf("\t\t\t%s\n",phone);
  lprintf("\t\t\t%s\n\n\n\n",comment);
  lprint("                  -------  Fold Outward on this Line  -------\n\n\n");
  lprint("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  lprint("                  -------  Fold Outward on this Line  -------\n\n\n");
  lprint("\t\t\t\t\t\t\t\t\tPlace\n");
  lprint("\t\t\t\t\t\t\t\t\tStamp\n");
  lprint("\t\t\t\t\t\t\t\t\tHere.\n\n\n\n\n\n");
  lprint("\t\t\t\tNew Dimension Software\n");
  lprint("\t\t\t\tMailing List Application\n");
  lprint("\t\t\t\t23 Pawtucket Dr.\n");
  lprint("\t\t\t\tCherry Hill, NJ  08003\n");
  lputchar('\f');
 }

mailapp()				/* Input Data for Mailing List */
 {
  int lw;
  int x = 0;
  int v;
  wcolor(WHT_F+MAG_B,WHT_F+MAG_B);
  wfcolor(BLK_F+WHT_B);
  lw = wopen(7,12,16,62,1);
  wtitle(lw,"´ Mailing List Application Ã",1);
  wmessage(lw,"´ ESC when Done Ã",0);
  wputat(lw,0,4,"Name:");
  wffill(lw,0,10,30);
  wputat(lw,1,3,"Title:");
  wffill(lw,1,10,25);
  wputat(lw,2,1,"Company:");
  wffill(lw,2,10,34);
  wputat(lw,3,1,"Address:");
  wffill(lw,3,10,30);
  wputat(lw,4,4,"City:");
  wffill(lw,4,10,30);
  wputat(lw,5,3,"State:");
  wffill(lw,5,10,2);
  wputat(lw,5,13,"Zip:");
  wffill(lw,5,18,10);
  wputat(lw,6,3,"Phone:");
  wffill(lw,6,10,14);
  wputat(lw,7,1,"Comment:");
  wffill(lw,7,10,38);
  while ( 1 )
   {
    switch ( x )
     {
      case 0: v = wfinptstred(lw,0,10,30,name,name);		break;
      case 1: v = wfinptstred(lw,1,10,25,title,title);		break;
      case 2: v = wfinptstred(lw,2,10,34,company,company);	break;
      case 3: v = wfinptstred(lw,3,10,30,address,address);	break;
      case 4: v = wfinptstred(lw,4,10,30,city,city);		break;
      case 5: v = wfinptstred(lw,5,10,2,state,state);		break;
      case 6: v = wfinptstred(lw,5,18,10,zip,zip);		break;
      case 7: v = wfinptstred(lw,6,10,14,phone,phone);		break;
      case 8: v = wfinptstred(lw,7,10,38,comment,comment);	break;
     }
    switch ( v )
     {
      case UARROW: x = max(0,x-1);	break;
      case      1: 
      case DARROW: x = min(8,x+1);	break;
      case   PGUP:
      case   HOME: x = 0;		break;
      case   PGDN:
      case    END: x = 8;		break;
      case     27: x = 100;		break;
          default: x = min(8,x+1);	break;
     }
    if ( x == 100 )
       break;
   }
  wclose(lw);
 }

gi()					/* Display General Information */
 {
  int lw;				/* Local Window Pointer */
  int lv;				/* Local Value */
  static char *geninfo[18] =		/* General Information Text */
   {
    "C Spot Run is a library of routines and utilities for C",
    "programmers designed to make coding easier by providing",
    "prewritten tools to take advantage of many of the features",
    "of the IBM-PC family of computers and the C language itself.",
    "",
    "C Spot Run is distributed in object code format as",
    "\"Shareware\" and the source code is available in exchange for",
    "a $50 donation.  (A commercial license for the use of C Spot",
    "Run is $75 and includes the source code.)",

    "C Spot Run is primarily the work of Bob Pritchett and New",
    "Dimension Software although contributions of source code and",
    "tools are and have been accepted and used.",
    "",
    "New Dimension Software",
    "23 Pawtucket Dr.",
    "Cherry Hill, NJ  08003",
    "Voice: (609) 424-2595",
    "Data: (609) 354-9259"
   };
  helpwin(1);				/* Display Help Set One */
  color(YEL_F+BLU_B);			/* Set Colors */
  lw = wopen(6,6,18,74,1);		/* Open Display Window */
  wtitle(lw,"´ General Information Ã",1);	/* Display Window Title */
  for ( lv = 0; lv < 9; ++lv )		/* Loop Through First Screen */
      wcenter(lw,lv+1,geninfo[lv]);	/* Display Text */
  cursor_off();				/* Assure That Cursor is Off */
  updt_cr();				/* Update Until Carriage Return */
  wcls(lw);				/* Clear Current Window */
  for ( lv = 0; lv < 9; ++lv )		/* Loop Through Second Screen */
      wcenter(lw,lv+1,geninfo[lv+9]);	/* Display Text */
  cursor_off();				/* Assure That Cursor is Off */
  updt_cr();				/* Update Until Carriage Return */
  wclose(lw);				/* Close Local Window */
 }

snd()					/* Display Sound Info and Demo */
 {
  int lv;				/* Local Value */
  int lw;				/* Local Window */
  int mp;				/* Music Pointer */
#ifndef MSC
  static char *music[21] =		/* Music */
   {
    "Mlt32o5",
    "L8<dd4.efgab-c+b-ag>e4.<gfc+d<ab->dfgc+<a>c+eL12agfgfe",
    "L8fdfaL12b-agagfL8egb->dl12c<b-ab-agl8afa>c",
    "fgacd<ab->dgab-de<b->c<gab->c<ef<ab->gcegb-l7agfl8a>d<fedc+>efg<a>gfefed",
    "<edc<bag+>b>cd<e>dc<b>c<bab>cl16fel8dc<bagab>l16edl8c<b",
    "agfl16gal8d>c<bag+f+ef+g+ab>cdefedc<ba>g+abag+f+edcl16del8fa<b>l16cdl8eg",
    "<al16b>cl8df<gl16abl8>ce<fl16gal8b>d<el16f+g+l8a>c",
    "<dc+dfb->cd<b-fefb->defd<b-ab->dfl16gal8b-ag+l16bal8g+l16fel8dl16fel8d",
    "<l16bal8g+b>eg+be>dl16c<b>l8c<l16bal8g+al12cdedc<bl8a..b>c+de<gfdfa",
    ">dfg+.<d.Mll16<a>e.>c+.a4.p4p4p4p4p4",

    "l8ee.fedc+<bal16gal8b-agfefgfed",
    ">F<AB-DCGB->e-<a>cfe-d<ab->d<e-gab>cde-cl12agf+gf+e",
    "l8f+l16cdl8e-dcl16<b-a>l8dc<b-l16>cdl8e-g<al16b->cl8df",
    "<gl16ab->l8ce-<fl16gal8b->d<e->l16gfl8e-l16c<b-l8a>l16c<b-l8al16f+e",
    "l8df+a>cl12e-dcf+edl8b-l16agl8f+gl12<b->cdc<b-ag..l8ab->cd<fecegb->",
    "l16efl8g<b-afa>cf<agfcg>cde<b-agfdfa>d<fed<a>eab->c<gfe-#",
    "d<b->dfb-agf>el16fgl8fedc<b->gl16<agfel8f<b->cegab-agf>aefd<b>dfagec+",
    "g<da>c+gfc+d<b-",
    "gb->dfe-c<a>e-<<b->fa>e-d<ab-gegb->dc<af+>c<b-agfe-de-gb-ab->e-",
    "gf+gb-c+<l16abl8>c+e<gl16b-al8gl16fel8fl16agl8fl16edl8<a>e>dc+<d.>d..",
    "p4p4p4p4p4"
   };
#endif
  static char *sinfo[9] =		/* Sound Info */
   {
    "C Spot Run provides a complete set of routines for use in",
    "making music in either fore or background modes.  A play()",
    "function, similar to that of BASIC, is being used to play",
    "the music you are hearing now.",
    "(Background sound is driven by the timer interrupt and can",
    "be completely controlled by the calling program.)",
    "",
#ifndef MSC
    "You are listening to the first movement of Bach's Partita II",
    "Alemanda, transposed by Daniel Pritchett."
#else
    "** Background sound is currently not supported by the **",
    "** Microsoft C version of C Spot Run. **"
#endif
   };
  helpwin(1);				/* Display Help Set One */
  color(YEL_F+BLU_B);			/* Set Colors */
  lw = wopen(6,6,18,74,1);		/* Open Display Window */
  wtitle(lw,"´ Sound Functions Ã",1);	/* Display Window Title */
  for ( lv = 0; lv < 9; ++lv )		/* Loop Through First Screen */
      wcenter(lw,lv+1,sinfo[lv]);	/* Display Text */
  cursor_off();				/* Assure That Cursor is Off */
#ifndef MSC
  sound_init();				/* Initialize Background Sound */
  dt = 1;				/* Set Type Not to Wait for KB */
  mp = 0;				/* Initialize Music Pointer */
  while ( 1 )				/* Forever */
   {
    if ( sound_left() < 90 )		/* If Room in Buffer */
       play(music[mp++]);		/* Play Another String */
    if ( mp == 21 )			/* If At End of Music */
       mp = 0;				/* Start Again */
    dtupdate();				/* Update Once */
    if ( kbhit() )			/* If Keyboard Hit */
       if ( getch() == 13 )		/* If Carriage Return */
        {
         sound_done();			/* Quit the Sound */
         break;				/* And Break Out of Loop */
        }
   }
  dt = 0;				/* Set Type Back to Wait for KB */
#else
  while ( 1 )
   {
    dtupdate();				/* Update Once */
    if ( kbhit() )			/* If Keyboard Hit */
       if ( getch() == 13 )		/* If Carriage Return */
        break;
   }
#endif
  wclose(lw);				/* Close Local Window */
 }

win()					/* Do Windowing Demo */
 {
  int lv;				/* Local Value */
  int w1;				/* First Window */
  int w2;				/* Second Window */
  int w3;				/* Third Window */
  static char *wininf[12] = 		/* Window Info */
   {
    "C Spot Run provides a powerful set of tools for text windows",
    "that can be quickly opened, moved, overlayed, and closed.  As",
    "many as 256 windows can be displayed at one time in a wide",
    "number of color and style variations.",

    "Windows can have have one of five pre-determined borders, a",
    "custom border, or be borderless.  Bordered windows can be as",
    "small as a nine character square, borderless windows as small",
    "as one character.",

    "As is being illustrated, window borders can be updated to both",
    "eliminate titles and change styles.  As the next feature,",
    "bringing background windows to foreground, is demonstrated",
    "watch the borders on these three windows."
   };
  wclose(dtw);				/* Close Date Time Window */
  helpwin(4);				/* Display Help Set Four */
  color(LRED_F+BLK_B);			/* Set Color */
  w1 = wopen(0,0,5,70,'Û');		/* Open First Window */
  wtitle(w1," Active Window ",1);	/* Set Window Title */
  for ( lv = 0; lv < 4; ++lv )		/* Loop Through Text */
      wcenter(w1,lv,wininf[lv]);	/* Center And Display Text */
  cursor_off();				/* Make Sure Cursor is Gone */
  wait_hs(800);				/* Wait 8 Seconds or Keyboard Hit */
  wborder(w1,'°');			/* Set New 'Inactive' Border */
  wcls(w1);				/* Clear the Window */
  color(LBLU_F+BLK_B);			/* Set Color */
  w2 = wopen(4,3,9,73,'Û');		/* Open Second Window */
  wtitle(w2," Active Window ",1);	/* Set Window Title */
  for ( lv = 0; lv < 4; ++lv )		/* Loop Through Text */
      wcenter(w2,lv,wininf[lv+4]);	/* Center And Display Text */
  cursor_off();				/* Make Sure Cursor is Gone */
  wait_hs(800);				/* Wait 8 Seconds or Keyboard Hit */
  wborder(w2,'°');			/* Set New 'Inactive' Border */
  wcls(w2);				/* Clear the Window */
  color(MAG_F+BLK_B);			/* Set Color */
  w3 = wopen(8,2,13,72,'Û');		/* Open Third Window */
  wtitle(w3," Active Window ",1);	/* Set Window Title */
  for ( lv = 0; lv < 4; ++lv )		/* Loop Through Text */
      wcenter(w3,lv,wininf[lv+8]);	/* Center And Display Text */
  cursor_off();				/* Make Sure Cursor is Gone */
  wait_hs(800);				/* Wait 8 Seconds or Keyboard Hit */
  wborder(w3,1);			/* Set New 'Inactive' Border */
  wcls(w3);				/* Clear the Window */
  wactivate(w1);			/* Activate First Window */
  wborder(w1,3);			/* Set 'Active' Border */
  wtitle(w1," Active Window ",1);	/* Set Window Title */
  wait_hs(200);				/* Wait 2 Seconds or Keyboard Hit */
  wborder(w1,1);			/* Set New 'Inactive' Border */
  wactivate(w2);			/* Activate Second Window */
  wscolor(w2,BLU_F+WHT_B,BLU_F+WHT_B);	/* Set Specific Window's Colors */
  wborder(w2,5);			/* Set 'Active' Border */
  wcls(w2);				/* Clear Window With New Colors */
  wtitle(w2,"| Active Window |",1);	/* Set Window Title */
  wcenter(w2,1,"Windows can also be 'jumped' to other positions.");
  wait_hs(300);				/* Wait 3 Seconds */
  wjump(w2,9,9);			/* Jump Six Spaces Down and Right */
  wcenter(w2,1,"Additionally, they can be 'moved' a space at a time.");
  wait_hs(300);				/* Wait 3 Seconds */
  for ( lv = 0; lv < 6; ++lv )		/* Loop Through Six Times */
   {
    wmove(w2,1);			/* Move Up Once */
    wait_hs(20);			/* Wait */
    wmove(w2,4);			/* Move Left Once */
    wait_hs(20);			/* Wait */
   }
  wait_hs(200);				/* Wait 2 Seconds */
  wclose(w1);				/* Close Window One */
  wclose(w2);				/* Close Window Two */
  wclose(w3);				/* Close Window Three */
  color(BLK_F+WHT_B);			/* Set Colors */
  w1 = wopen(1,4,17,75,1);		/* Open Window */
  wtitle(w1,"´ Window Output Ã",1);	/* Title Window */
  wcenter(w1,0,"Assembly and C routines allow us to quickly print");
  wcenter(w1,1,"text into a window.");
  wait_hs(500);				/* Wait 5 Seconds */
  cursor_on();				/* Turn on Cursor */
  timer();				/* Start Timing */
  for ( lv = 0; lv < 50; ++lv )		/* Loop 50 Times */
      wprint(w1,"\nFast output!");	/* Printing Text */
  lv = timer();				/* Save Time */
  wprintf(w1,"\nUsing the CSR timer() routine we calculated that");
  wprintf(w1,"\nthose 50 lines of output took %d timer ticks.",lv);
  wait_hs(300);				/* Wait 3 Seconds */
  wprint(w1,"\n\nFull color output is also possible:");
  wait_hs(200);				/* Wait 2 Seconds */
  for ( lv = BLK_F+WHT_B; lv < WHT_F+WHT_B; ++lv )	/* Loop Through */
      wprintc(w1,"\nWindow text in color!",lv);	/* Print in Color */
  wait_hs(200);				/* Wait 2 Seconds */
  wscolor(w1,BLU_F+WHT_B,BLU_F+WHT_B);	/* Set Window Colors */
  wprint(w1,"\nWe also have a wprintf() statement!");
  wait_hs(200);				/* Wait 2 Seconds */
  for ( lv = 0; lv < 50; ++lv )		/* Loop 50 Times */
      wprintf(w1,"\nCounting: %d",lv);	/* Printing Text */
  wait_hs(200);				/* Wait 2 Seconds */
  wprint(w1,"\nIt might be nice to set the scrolling boundaries....");
  wait_hs(300);				/* Wait 2 Seconds */
  wscolor(w1,WHT_F+BLU_B,WHT_F+BLU_B);	/* Set Window Colors */
  wborder(w1,1);			/* Redraw Border */
  wtitle(w1,"´ Window Output Ã",1);	/* Retitle Window */
  wcls(w1);				/* Clear the Window */
  wcenter(w1,0,"Freeze this line.");	/* Display Frozen Line */
  wcenter(w1,wrow(w1),"Freeze this line.");	/* Display Frozen Line */
  wfreeze(w1,1,wrow(w1)-1);		/* Freeze the Lines */
  whome(w1);				/* Go to First Available Line */
  wait_hs(200);				/* Wait 2 Seconds */
  for ( lv = 0; lv < 50; ++lv )		/* Loop 50 Times */
      wprintf(w1,"\nCounting: %d",lv);	/* Printing Text */
  cursor_off();				/* Turn off Cursor */
  wfreeze(w1,0,wrow(w1));		/* Unfreeze All Lines */
  wait_hs(200);				/* Wait 2 Seconds */
  wcls(w1);				/* Clear the Window */
  wcenter(w1,"You've already seen how text is centered.");
  wputat(w1,2,2,"+ The plus sign is at 2,2 via wputat().");
  wcenter(w1,4,"We also have a wgotoxy() and whome().");
  wcenter(w1,6,"Note that lines can be deleted and inserted:");
  wcenter(w1,7,"Line One");
  wcenter(w1,8,"Line Two");
  wcenter(w1,9,"Line Three");
  wait_hs(300);				/* Wait 3 Seconds */
  wdelete(w1,8,1);			/* Delete One Line at 8 */
  wait_hs(300);				/* Wait 3 Seconds */
  winsert(w1,8,1);			/* Insert One Line at 8 */
  wcenter(w1,8,"This line was inserted!");
  wait_hs(300);				/* Wait 3 Seconds */
  wblank(w1,7);				/* Blank Line */
  wblank(w1,8);				/* Blank Line */
  wblank(w1,9);				/* Blank Line */
  wcenter(w1,8,"We just used wblank() to eliminate those lines.");
  wcenter(w1,10,"Note that messages like titles can be put on windows.");
  wmessage(w1,"´ This is a message Ã",0);	/* Put a Window Message */
  wait_hs(300);				/* Wait 3 Seconds */
  wcls(w1);				/* Clear the Window */
  wcenter(w1,2,"This is just a sampling of the windowing routines,");
  wcenter(w1,3,"consult the documentation for more and better");
  wcenter(w1,4,"descriptions of these and other routines.");
  helpwin(1);				/* Display Help Set One */
  wait_cr();				/* Wait for a Carriage Return */
  wclose(w1);				/* Close the Window */
  color(BLK_F+GRN_B);			/* Set Output Colors */
  dtw = wopen(1,61,4,78,1);		/* Open Date and Time Window */
 }

mnuing()				/* Display Menuing Information */
 {
  int lv;				/* Local Value */
  int lw;				/* Local Window Pointer */
  static char *mnuinf[15] =		/* Menu Information */
   {
    "C Spot Run includes a number of routines for the creation",
    "and use of pop-up menus.  Menu options are selectable by",
    "scrolling a highlight bar or entering the first letter of",
    "an options name.  Text can be non-selectable and non-selectable",
    "lines can be used to separate text or options.",
    "",
    "Menus can be created and used later, created and run, run and",
    "closed, etc.  As many as eight menus of fifteen options each",
    "can be in memory at one time with the pmopen() function, and",
    "colors can be set for the menus, their borders, and the highlight",
    "bar providing you with complete control.  The pmrun() function",
    "even allows you to specify a function to run while the menu",
    "awaits keyboard activity.",
    "",
    "The menuing in this program is done with the pm*() functions."
   };
  helpwin(1);				/* Display Help Set One */
  color(GRN_F+BLK_B);			/* Set Colors */
  lw = wopen(0,1,18,78,254);		/* Open the Window */
  wtitle(lw,"Û Menuing Routines Û",1);	/* Put Window Title */
  for ( lv = 0; lv < 15; ++lv )		/* Loop Through Text */
      wcenter(lw,lv+1,mnuinf[lv]);	/* Display The Text */
  cursor_off();				/* Make Sure Cursor is Off */
  wait_cr();				/* Wait for a Carriage Return */
  wclose(lw);				/* Close Local Window */
 }

diracc()				/* Display Directory and Info */
 {
  int lv;				/* Local Value */
  int lw;				/* Local Window */
  char dir[3];				/* Directory String */
  static char *dirinf[4] =		/* Directory Information */
   {
    "A number of functions for directory access and display are",
    "available.  Complete control of file searching attributes",
    "is available, and the interface of directory routines and",
    "windows provides a powerful DOS interface."
   };
  helpwin(1);				/* Display Help Set One */
  color(WHT_F+MAG_B);			/* Set Colors */
  lw = wopen(0,0,7,79,3);		/* Open Window */
  wtitle(lw,"µ Directory Access Æ",1);	/* Put Window Title */
  for ( lv = 0; lv < 4; ++lv )		/* Loop Through Text */
      wcenter(lw,lv+1,dirinf[lv]);	/* Display The Text */
  cursor_off();				/* Make Sure Cursor is Off */
  wait_cr();				/* Wait for a Carriage Return */
  dir[0] = get_drive() + 'A';		/* Get Letter of Current Drive */
  dir[1] = ':';				/* Add a Colon to String */
  dir[2] = 0;				/* Tack a Null to End */
  color(WHT_F+RED_B);			/* Set Colors */
  dirwin(dir,"*.*");			/* Display Directory of Root */
  wclose(lw);				/* Close Local Window */
 }

inpinf()				/* Display Input Information */
 {
  int lv;				/* Local Value */
  int lw;				/* Local Window */
  static char *inpinfo[11] =		/* Input Information */
   {
    "Input from a simple yes or no query to a fully editable field",
    "for strings or integers is easily implemented with the CSR",
    "input and field input routines.",
    "",
    "Input can be requested in or out of windows, in or out of fields",
    "and can be totally customized by the programmer.",
    "",
    "Some of the most powerful input possible is demonstrated by the",
    "mailing list application under the \"New Dimension Software\"",
    "selection at the main menu.  Please give it a try, and play",
    "with the arrow keys, insert and delete, etc."
   };
  helpwin(1);				/* Display Help Set One */
  color(BLK_F+CYN_B);			/* Set Colors */
  lw = wopen(5,6,19,74,1);		/* Open Display Window */
  wtitle(lw,"´ Input Routines Ã",1);	/* Display Window Title */
  for ( lv = 0; lv < 11; ++lv )		/* Loop Through First Screen */
      wcenter(lw,lv+1,inpinfo[lv]);	/* Display Text */
  cursor_off();				/* Assure That Cursor is Off */
  updt_cr();				/* Update Time and Date till CR */
  wclose(lw);				/* Close Window */
 }

ofinf()					/* Display Other Features Info */
 {
  int lv;				/* Local Value */
  int lw;				/* Local Window */
  static char *ofinfo[9] =		/* Text of Information */
   {
    "This demo program presents only a portion of the features of",
    "C Spot Run.  Dozens of additional functions manipulate windows,",
    "provide an interface to DOS and BIOS, manipulate text and",
    "attributes on the screen, get and set the date and time,",
    "time events and manipulate strings.",
    "",
    "I encourage you to browse through the documentation, the",
    "newsletters, and sample source code for more ideas and",
    "illustrations on use and implementation."
   };
  helpwin(1);				/* Display Help Set One */
  color(RED_F+WHT_B);			/* Set Colors */
  lw = wopen(6,6,18,74,1);		/* Open Display Window */
  wtitle(lw,"´ Other Features Ã",1);	/* Display Window Title */
  for ( lv = 0; lv < 9; ++lv )		/* Loop Through First Screen */
      wcenter(lw,lv+1,ofinfo[lv]);	/* Display Text */
  cursor_off();				/* Assure That Cursor is Off */
  updt_cr();				/* Update Time and Date till CR */
  wclose(lw);				/* Close Local Window */
 }
