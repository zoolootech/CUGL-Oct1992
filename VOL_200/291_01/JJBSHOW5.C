/**************************************************************************
 *                                                                        *
 *                           JJBSHOW5.C                                   *
 *                                                                        *
 *   Copyright (c) 1988, 1989, JJB. All rights reserved.                  *
 *                                                                        *
 *   This example shows you how to use the 'enter' functions so you can   *
 *   input entire screens of data.                                        *
 *                                                                        *
 *   Turbo C is a trademark of Borland International, Inc.                *
 *   Quick C is a trademark of Microsoft Corp.                            *
 *   JJB, 9236 church Rd suite 1082, Dallas, Tx 75231 (214) 341-1635      *
 **************************************************************************/


/***************************************************************************
 * For Turbo C programmers only:                                           *
 *   To run this program from DOS:                                         *
 *                                                                         *
 *         Enter 'TC JJBSHOW5'    ( to load this file with Turbo C)        *
 *         Press 'CTRL F9'        ( to compile and begin executing)        *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * for Quick C programmers only:                                           *
 *   To run this program, from DOS:                                        *
 *                                                                         *
 *         Enter 'JJBQCS5'    ( to load this file with JJB.QLB)            *
 *         Press 'F5'         ( to compile and begin executing)            *
 *                                                                         *
 *   To make the .exe file JJBSHOW5.EXE, from DOS enter:                   *
 *         QCL  /c  /AM  JJBSHOW5.C                                        *
 *         LINK  JJBSHOW5 + JJBQC + JJBQCINP + JJBQCDOL,,, C:LIB\,         *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 * for both TURBO C and QUICK C programmers:                               *
 *                                                                         *
 * Not all options have been assigned functions. The options unassigned    *
 * execute the JJB copyright function.                                     *
 *                                                                         *
 * As JJB executes (runs) a program, the following keys can be pressed:    *
 *                                                                         *
 *    ALT X     to exit.                                                   *
 *    F1        to display a help screen.                                  *
 *    F2-F9     to instantly select another option.                        *
 *    ALT       to select a group of options.                              *
 *    ALT       and a letter to select a specific group.                   *
 *    ESC       means nevermind.                                           *
 *       ->        to select the group to the right.                       *
 *       <-        to select the group to the left.                        *
 *       UPARROW   to highlight the option above.                          *
 *       DOWNARROW to highlight the option below.                          *
 *       RETURN    to select the option highlighted.                       *
 *       A LETTER  to select a specific option.                            *
 *       ALT and a letter to select a different group.                     *
 *       ESC       return exactly where you were in the function           *
 *                      currently being executed.                          *
 *                                                                         *
 * If another option is selected, JJB will execute:                        *
 *                                                                         *
 *         the leave function for the current option.                      *
 *         the leave function for the current group.                       *
 *         the initalization function for the selected group.              *
 *         the initalization function for the selected option.             *
 *         and option function.                                            *
 *                                                                         *
 ***************************************************************************/

/* see JJB-READ.DOC for an additional explanation of the functions below.  */

#include <jjbkbd.h>
#include <jjbset.h>

main() {

  jjb_initalize();        /* initalize the jjb functions             */
  jjb_setup();            /* set up your options and assign functions*/
  jjb_start();            /* start executing the default option      */
    }


/****************************************************************************
 *                                                                          *
 *                            SAMPLE HELP FUNCTION                          *
 *                                                                          *
 ****************************************************************************/

int scr1_arry[2000];

sample_help() {
     vsave_scr(&scr1_arry);  /* save the screen   */
     set_color(RED ONBLUE);
     vfw(6,11,64,14);        /* 'vf' a window     */

     vloc(8,14);      /* position for video fast functions */

    /* 'vfsdo(' videos fast a string and then does a carriage ret.
       '2,14' sets up all subsequent to come down two and over 14. */

     vfsdo("F1 is reserved for help.",2,14);

     vfsdo("You can set up a function and call it by using the F1 key.",SAME);
     vfsdo("JJB will use whatever function you assign in jjb_setup().",SAME);
     vfsdo("In this example, jjb_setup assigns sample_help() as follows:",SAME);
     vfsdo("      'help(sample_help);'",3,14);
     vfsc( pak() );     /* video fast 'Press any key'and put cursor after  */
     getch();           /* getch() is ok here.                             */
     normal();
     vrest_scr(&scr1_arry);    /* restore the screen                              */
     }
/****************************************************************************
 *                                                                          *
 *                            SAMPLE FUNCTION                               *
 *                                                                          *
 ****************************************************************************/


update_cust()  {  int x;  unsigned long lfrom, lto;
  char buffer[256];    /*  customer I/O buffer    */

  /*   define the define pointers   */
  char *name, *street, *city, *phone1, *phone2, *phone3,
       *comment, *mo, *day, *year, *check , *amt,
       *pak = "  ";  /* for press any key   */

  /*   define the data lengths.        */
  /*   if you change any data length, the buffer pointers below
       will automatically be adjusted for the new address        */

  int lname    = 44,
      lstreet  = 30,
      lcity    = 30,
      lphone1  =  3,
      lphone2  =  3,
      lphone3  =  4,
      lcomment =  30,
      lmo      = 2,
      lday     = 2,
      lyear    = 2,
      lcheck   = 6,
      lamt     = 14;
      x=1;

  /* store buffer address in the character pointers     */
      name    = &buffer[x];   x += lname    +1;
      street  = &buffer[x];   x += lstreet  +1;
      city    = &buffer[x];   x += lcity    +1;
      phone1  = &buffer[x];   x += lphone1  +1;
      phone2  = &buffer[x];   x += lphone2  +1;
      phone3  = &buffer[x];   x += lphone3  +1;
      comment = &buffer[x];   x += lcomment +1;
      mo      = &buffer[x];   x += lmo      +1;
      day     = &buffer[x];   x += lday     +1;
      year    = &buffer[x];   x += lyear    +1;
      check   = &buffer[x];   x += lcheck   +1;
      amt     = &buffer[x];   x += lamt     +1;


  /*a  zero data buffer  */
  for (x=0; x<256; ++x) buffer[x] = '\0';

/* Normally you would read the data file, but since we don't have a data
   file in this example, we need to fill the data buffer as follows:  */

  name    = "John Thomas Wellington";
  street  = "1527 South Meadow Road";
  city    = "Dallas, TX    75247";
  phone1  = "214";  phone2 = "357";  phone3 = "8934";
  comment = "Place any comment here";
  mo      = "11";
  day     = "24";
  year    = "88";
  check   = "34512";

  amt     = "";
  vloc(15,22); vfs("(   )    -");    /*  for phone number  */
  vloc(19,22); vfs("  -  -  ");      /*  for date  */


   vloc(4,3); vfs("Note: ");
vfsdo("At any time while entering, you can press: ALT to change options.",1,9);
   /* Notice 'SAME' on next line does 1 down 9 over.                    */
   vfsdo("UP-DOWN ARROWS to backup, '*' to remove data.",SAME);
   vfsdo("Plus any assigned function keys F1-F9, or ALT X to exit.",SAME);
   vfsdo("SPACE BAR is same as RETURN if first character entered.",SAME);

/*  BEGIN....AGAIN  is a continuous while loop                          */
/*  example: eptr(4) will be true only if the entry pointer equals 4    */
/*  Each enter( increments the entry pointer by 1                       */
/*  In addition to entering, JJB also displays the data from the buffer */
/*  Since the buffer is updated directly, the only thing left for the   */
/*    programmer to do is to read & write the record to the disk file.  */
/*  The first time thru the enter( will just display the screen.        */
/*  until it starts with (x)                                            */

  eptr_reset();    /* set eptr (enter pointer) to 1        */


  lfrom = 100;            /* see enter_lnum(  below       */
  lto = 999998;

BEGIN

  if (eptr(1))  enter(9,9 ,   "Enter name: ",name,   lname,ATEND);
  if (eptr(2))  enter(11,12,     " street: ",street, lstreet,ATEND);
  if (eptr(3))  enter(13,12,     "   city: ",city,   lcity,ATEND);
  if (eptr(4))  enter_num(15,12, "  phone: ",phone1, lphone1,100,999,15,23);
  if (eptr(5))  enter_num(15,12, "  phone: ",phone2, lphone2,100,999,15,28);
  if (eptr(6))  enter_num(15,12, "  phone: ",phone3, lphone3,1000,9999,15,32);
  if (eptr(7))  enter (17,12,    "comment: ",comment, lcomment,17,22);
  if (eptr(8))  enter_num (19,12,"   date: ",mo,  lmo,  1,12,19,22);
  if (eptr(9))  enter_num (19,12,"   date: ",day, lday, 1,31,19,25);
  if (eptr(10)) enter_num (19,12,"   date: ",year,lyear,0,99,19,28);
  if (eptr(11)) enter_lnum (21,12," check#: ",check,lcheck,lfrom,lto,21,23);
  if (eptr(12))  enter_dollar(23,12," amount:",amt, 23,22);
  if (eptr(13)) {enter(23,50 ,"Press any key ",pak, 1,ATEND);
       vloc(23,50); vspaces(22);}

  eptr_start_with(1);   /* turn off displaying and start entering     */

AGAIN

}


jjb_setup() {
  group("Update");
      option("Update customer file");
          funct(update_cust);
      option("Type the");
      option("Description to");
      option("Your options here.",DRAWLINE);
      option("Each group can");
      option("have as many ");
      option("Options as you need.");

  group("Other");
      option("These options have no");
      option("functions assigned to them.");

       help(sample_help);

   }




