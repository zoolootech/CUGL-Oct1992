/***************************************************************************
 *                                                                         *
 *                           JJBSHOW2.C                                    *
 *                                                                         *
 *   Copyright (c) 1988, 1989, JJB. All rights reserved.                   *
 *                                                                         *
 *   This example shows you how to assign functions to options.            *
 *                                                                         *
 *                                                                         *
 *                   'funct(functionname)'                                 *
 *                                                                         *
 *                                                                         *
 *   Just glance and it and move on to example JJBSHOW3.C                  *
 *                                                                         *
 *   Turbo C is a trademark of Borland International, Inc.                 *
 *   Quick C is a trademark of Microsoft Corp.                             *
 *   JJB, 9236 church Rd suite 1082, Dallas, Tx 75231 (214) 341-1635       *
 ***************************************************************************/


/***************************************************************************
 * For Turbo C programmers only:                                           *
 *   To run this program from DOS:                                         *
 *                                                                         *
 *         Enter 'TC JJBSHOW2'    ( to load this file with Turbo C)        *
 *         Press 'CTRL F9'        ( to compile and begin executing)        *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * for Quick C programmers only:                                           *
 *   To run this program, from DOS:                                        *
 *                                                                         *
 *         Enter 'JJBQCS2'    ( to load this file with JJB.QLB)            *
 *         Press 'F5'         ( to compile and begin executing)            *
 *                                                                         *
 *   To make the .exe file JJBSHOW2.EXE, from DOS enter:                   *
 *         QCL  /c  /AM  JJBSHOW2.C                                        *
 *         LINK  JJBSHOW2.OBJ + JJBQC.OBJ,,, C:LIB\,                       *
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
  jjb_initalize();        /* initalizes JJB   */
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
     vfsdo("This example shows you how to",SAME);
     vfsdo("assign a function to an option.",SAME);
     vfsdo(" ",SAME);
     vfsdo("Now move on to next show example.",SAME);
     vfsdo(" ",SAME);
     bright();
     vfsc("Press any key "); getch();
     normal();
     vrest_screen();
  }

/* The function below uses some of the functions in the JJB library.*/

video_message() {
  vclrw();         /* clear the window */
  vloc(10,20);     /* position the video fast cursor */
  vfsc("This message will appear on the screen.");
  get_ch();
}



/****************************************************************************
 *                                                                          *
 *                       jjb_setup()                                        *
 *                                                                          *
 *   jjb_setup() is where you should define how your pull down menus will   *
 *   for each group and option of the group.                                *
 *                                                                          *
 *   This example shows you how to assign function names to options.        *
 *                                                                          *
 ****************************************************************************/


jjb_setup() {

  group("Update");
     option("Video a message on the screen");
        funct(video_message);
     option("Update Customer file",DRAWLINE);
     option("Miscellaneous");

  group("File");
      option("Any option you want");
      option("can be placed here.");
      option("Any group can have");
      option("any number of options.");
  group("More");
      option("Call");
      option("Create a new file");
      option("These option descriptions");
      option("  's'how you that you can");
      option("  'n'ame each option.");
  help(my_help);
  }




