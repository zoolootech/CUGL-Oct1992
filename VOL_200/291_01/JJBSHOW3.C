/***************************************************************************
 *                                                                         *
 *                           JJBSHOW3.C                                    *
 *                                                                         *
 *   Copyright (c) 1988, 1989, JJB. All rights reserved.                   *
 *                                                                         *
 *                                                                         *
 *   This example shows you how to assign functions for initalizing and    *
 *   leaving groups and options.                                           *
 *                                                                         *
 *                  'init(functionname)'                                   *
 *                  'leave(functionname)'                                  *
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
 *         Enter 'TC JJBSHOW3'    ( to load this file with Turbo C)        *
 *         Press 'CTRL F9'        ( to compile and begin executing)        *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * for Quick C programmers only:                                           *
 *   To run this program, from DOS:                                        *
 *                                                                         *
 *         Enter 'JJBQCS3'    ( to load this file with JJB.QLB)            *
 *         Press 'F5'         ( to compile and begin executing)            *
 *                                                                         *
 *   To make the .exe file JJBSHOW3.EXE, from DOS enter:                   *
 *         QCL  /c  /AM  JJBSHOW3.C                                        *
 *         LINK  JJBSHOW3.OBJ + JJBQC.OBJ,,, C:LIB\,                       *
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
     vfwc(38,9);                 /*  'vf' a window centered   */
     vfsdo(" ",SAME);
     vfsdo("This example shows you how to",SAME);
     vfsdo("assign functions for initalizing",SAME);
     vfsdo("and exiting groups and options.",SAME);
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
  vloc(10,20);     /* position the video invisible fast cursor */
  vfsc("This message will appear on the screen.");
  get_ch();
   }

init_update() {
  /* you can place any C functions here to initalize the group. */
   }

leave_update() {
     /* you can place any C functions here to leave the group. */

   }

init_vid_msg() {
  /* you can place any functions here to initalize an option.*/
  vloc(4,55); vfs("initalize message, etc.");
   }

leave_vid_msg() {
  /* you can place any functions here to leave an option.*/
   }


/****************************************************************************
 *                                                                          *
 *                       jjb_setup()                                        *
 *                                                                          *
 *   jjb_setup() is where you should define how your pull down menus will   *
 *   for each group and option.                                             *
 *                                                                          *
 *   This example shows you how to initialize and leave groups and options. *
 *                                                                          *
 *   initalizing and leaving functions for groups and options are not       *
 *   required. They are optional and to be used only if you need them.      *
 *                                                                          *
 ****************************************************************************/


jjb_setup() {
  help(my_help);
  group("Update");
      init(init_update);
      leave(leave_update);

     option("Video a message on the screen");
        funct(video_message);
        init(init_vid_msg);
        leave(leave_vid_msg);

     option("Update Customer file");

  }




