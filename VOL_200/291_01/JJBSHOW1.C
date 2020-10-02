/***************************************************************************
 *                                                                         *
 *                           JJBSHOW1.C                                    *
 *                                                                         *
 *   Copyright (c) 1988, 1989, JJB. All rights reserved.                   *
 *                                                                         *
 *   This example shows Turbo C and Quick C programmers the basic concepts.*
 *   Just glance at it and move on to example JJBSHOW2.C.                  *
 *                                                                         *
 *   Turbo C is a trademark of Borland International, Inc.                 *
 *   Quick C is a trademark of Microsoft Corp.                             *
 *   JJB, 9236 church Rd suite 1082, Dallas, Tx 75231 (214) 341-1635       *
 ***************************************************************************/

/***************************************************************************
 * For Turbo C programmers only:                                           *
 *   To run this program from DOS:                                         *
 *                                                                         *
 *         Enter 'TC JJBSHOW1'    ( to load this file with Turbo C)        *
 *         Press 'CTRL F9'        ( to compile and begin executing)        *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * for Quick C programmers only:                                           *
 *   To run this program, from DOS:                                        *
 *                                                                         *
 *         Enter 'JJBQCS1'    ( to load this file with JJB.QLB)            *
 *         Press 'F5'         ( to compile and begin executing)            *
 *                                                                         *
 *   To make the .exe file JJBSHOW1.EXE, from DOS enter:                   *
 *         QCL  /c  /AM  JJBSHOW1.C                                        *
 *         LINK  JJBSHOW1.OBJ + JJBQC.OBJ,,, C:LIB\,                       *
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

#include <jjbset.h>


main()
  {
  jjb_initalize();        /* initalizes all JJB arrays & video    */
  jjb_setup();            /* setup options & assign functions     */
  jjb_start();            /* start executing the default option   */

  }



/***************************************************************************
 *                        SAMPLE HELP FUNCTION                             *
 ***************************************************************************/

/* 'vfwc' puts out a window centered on the screen.                    */
vfwc(width,depth) int width,depth; { int row,col;
    row = (25 - depth) / 2;   /* compute row for making the window  */
    col = (80 - width) / 2;   /* compute col for making the window  */
    vfw(row,col,width,depth); /* now make the window                */
    }

my_help() {
     vsave_screen();
     set_color(RED ONBLUE);
     vfwc(38,7);                 /*  'vf' a window centered   */
     vfsdo(" ",SAME);
     vfsdo("This example shows you how fast",SAME);
     vfsdo("you can set up your options.",SAME);
     vfsdo(" ",SAME);
     vfsdo("Now move on to next show example.",SAME);
     vfsdo(" ",SAME);
     bright();
     vfsc("Press any key "); getch();
     normal();
     vrest_screen();
  }



/****************************************************************************
 *                                                                          *
 *                       jjb_setup()                                        *
 *                                                                          *
 *   jjb_setup() is where you define how your pull down menus will look     *
 *   for for each group and option.                                         *
 *                                                                          *
 *   This example shows you how to begin.                                   *
 *                                                                          *
 *   The string descriptions have no meaning.                               *
 *   You may change them to any description you wish.                       *
 *                                                                          *
 *   Notice how easily you can assign function keys F2 thru F9.             *
 *                                                                          *
 *   You may have up to 10 groups and 50 total options.                     *
 *   Any one group can have any number of options.                          *
 *                                                                          *
 *      'group('  starts a new group of options.                            *
 *      'option(' starts an option.                                         *
 *      ' ' lets you specify any keypress for the options.                  *
 *          Otherwise JJB will use the first, second, or third letter.      *
 *      ',DRAWLINE' can be used to separate an option.                      *
 *      'default_opt()' tells JJB to start with and default to this option  *
 *                                                                          *
 ****************************************************************************/

/* JJB CAN BE USED FOR ANY APPLICATION. BELOW IS JUST AN EXAMPLE.         */
/* WITH OPTION DESCRIPTIONS WHICH HAVE NO MEANING.                        */

jjb_setup() {
  /* use the set( function below to show you the JJB setup    */
  /* set(SHOWSETUP_SW,T);  */
  help(my_help);
  group("File");
      option("Access");
      option("Update file");
      option("Merge");
      option("Copy");
      option("Copy and Mer'g'e   ",DRAWLINE);
      option("Create file",DRAWLINE);
      option("List");
      option("List 'P'artially",DRAWLINE);
      option("Text search");
      option("Replace text");

  group("Enter");
      option("Master records  F4");
      option("Activity",DRAWLINE);
      option("Anything you want for any application");
      option("Special report descriptions   F5");
      option("Special 'K'eys",DRAWLINE);
      option("Setup configuration");
      option("Color selections");

  group("Reports");
      option("Daily reports     F9");
      option("Monthly");
      option("Annual",DRAWLINE);
      option("Special 'F'oken reports");
      option("Activity 'J'ournals   F6",DRAWLINE);
      option("Send reports to asc file    ",DRAWLINE);
      option("Print all reports");

  group("Other");
      option("Press ALT and RETURN now");
         default_opt(); /* JJB will now default to this option */
      option("This is just an option description");
      option("It is stored in an array");
      option("And placed on the screen");
      option("For your user to use in selecting options");
      option("You may assign function names to these descriptions");
      option("which will be executed when you select the option");
      option("JJB has over 30 arrays which it uses to maintain");
      option("control over option selection and execution.");

  }


