/**************************************************************************
 *                                                                        *
 *                           JJBSHOW4.C                                   *
 *                                                                        *
 *   Copyright (c) 1988, 1989, JJB. All rights reserved.                  *
 *                                                                        *
 *   The purpose of JJB is to help you to write programs as quickly       *
 *   as possible. JJB handles things for you and provides you with        *
 *   an environment which allows you to concentrate on the functions      *
 *   of your program.                                                     *
 *                                                                        *
 *   This example shows you how to use the following functions:           *
 *                                                                        *
 *      jjb_initalize()            initalizes JJB.                        *
 *      jjb_setup()                sets up the JJB option arrays.         *
 *      jjb_start()                begins executing the default option.   *
 *                                                                        *
 *      group(group description)   start setting up a group of options &  *
 *                                  assign a description to the group.    *
 *                                                                        *
 *      option(option description) sets up an option.                     *
 *                                                                        *
 *      funct(function name)       assigned a function to the option.     *
 *      default_opt()              assigns default to an option           *
 *      help(functionname)        assigns a function to F1 help.          *
 *                                                                        *
 *    Turbo C is a trademark of Borland International, Inc.               *
 *    Quick C is a trademark of Microsoft Corp.                           *
 *    JJB, 9236 church Rd suite 1082, Dallas, Tx 75231 (214) 341-1635     *
 **************************************************************************/


/***************************************************************************
 * For Turbo C programmers only:                                           *
 *   To run this program from DOS:                                         *
 *                                                                         *
 *         Enter 'TC JJBSHOW4'    ( to load this file with Turbo C)        *
 *         Press 'CTRL F9'        ( to compile and begin executing)        *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
 * for Quick C programmers only:                                           *
 *   To run this program, from DOS:                                        *
 *                                                                         *
 *         Enter 'JJBQCS4'    ( to load this file with JJB.QLB)            *
 *         Press 'F5'         ( to compile and begin executing)            *
 *                                                                         *
 *   To make the .exe file JJBSHOW4.EXE, from DOS enter:                   *
 *         QCL  /c  /AM  JJBSHOW4.C                                        *
 *         LINK  JJBSHOW4.OBJ + JJBQC.OBJ + JJBQCINP.OBJ ,,, C:LIB\,       *
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

  jjb_initalize();      /* initalize JJB arrays and video screen */

  jjb_setup();          /* set up the options */

  jjb_start();          /* start executing the default option   */

  }


/****************************************************************************
 *                                                                          *
 *                        sample_function()                                 *
 *                                                                          *
 *   This is a sample function which is assigned to the options             *
 *   to show you how functions can be executed when you select              *
 *   an option.                                                             *
 *                                                                          *
 *   if you do not assign a function to each option, JJB will initalize     *
 *                                                                          *
 *   each option with a null function which just gets a character.          *
 *                                                                          *
 ****************************************************************************/


/* Below is just a sample of some of the video fast 'vf' functions          */
/*   you will be able to use in the JJB library, see JJBREAD.ME             */

/* 'vfs(string)'         video fast a string direct to video memory.        */
/* 'vfsdo(string,2,14)'  sets up all subsequent 'vfsdo' down 2 over  14     */
/* 'vfsdo(string,SAME)'  does 2 carr. rets and goes over to column 14       */
/* All the 'vf' functions are portable and work for all video controllers   */
/* 'vfsc(string)' places physical cursor after putting out string.          */


/* 'vfwc' puts out a window centered on the screen. It is not used in this
       source. 'vfwc(50,14);' will center a box 50 positions wide, 14 deep.*/

vfwc(width,depth) int width,depth; { int row,col;
    row = (25 - depth) / 2;   /* compute row for making the window  */
    col = (80 - width) / 2;   /* compute col for making the window  */
    vfw(row,col,width,depth); /* now make the window                */
    }


int scr1_arry[2000];

sample_help() {
     vsave_scr(&scr1_arry);  /* save the screen   */
     set_color(RED ONBLUE);
     vfw(6,11,64,14);

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

sample_function()  {  char str[31]; char *sp = "  "; int tab = 8;

 vloc(4,8);        /* prepare for a video fast at row 4, column 8  */
 vfsdo("Look at the file JJBSHOW4.C to see how JJB works.",2,tab);
 vfsdo("JJB may be used for any programming application.",SAME);
 vfs("JJB LA (Large Application)");
 vfsdo(" is to be released in the spring of 1989.",3,tab);
 vfsdo("At any time while typing, you may press ALT and change options.",2,tab);
 vfs("The option selected is displayed in bottom left corner.");

   while (1)
      {
  /* see JJBSHOW5.C for an example of how you can use the enter   */
  /* functions to input entire screens of data.                   */

  enter(16,20,"Enter a string ",&str[0],30,16,36);
  enter(18,20,"Press any key  ",&str[0],1,ATEND);

  vloc(18,20); vspaces(55);   /* clear press any key.    */


    }
}


/****************************************************************************
 *                                                                          *
 *                       jjb_setup()                                        *
 *                                                                          *
 *   jjb_setup() is where you set up all your program option descriptions   *
 *      and assign function names to each option.                           *
 *                                                                          *
 ****************************************************************************/

/*  The groups and options descriptions being set up have no meaning.      */
/*  Any description can be chosen.                                         */
/*  Try changing them and see what happens when you press 'F5'.            */
/*  Some of the options below do not have a function assigned to them.     */
/*  To keep this example simple, more than one function is assigned to     */
/*     the sample function. Normally, you would not assign a function      */
/*     to more than one option.                                            */
/*                                                                         */

jjb_setup() {

  group("Application");          /* this starts a group of options */

      option("Applications Test #1");
          funct(sample_function);
      option("Applications 'V'ideo Test");
          funct(sample_function);

      option("Applications 'D'isk  Test");
          funct(sample_function);
          default_opt();   /* start with this option          */

      option("C'o'mmunicatyion Test");
          funct(sample_function);


  group("Reports");          /* this starts a group of options */

      option("General ledger F7");    /* assign F7 to this option */
      option("Trial Balance");
          funct(sample_function);
      option("Chart of Accounts Report");
          funct(sample_function);
      option("Profit & Loss");
          funct(sample_function);
      option("Balance Sheet",DRAWLINE); /* DRAWLINE separates */
          funct(sample_function);
      option("More Reports");
          funct(sample_function);

  group("More");
      option("Miscellaneous Options");
      option("Month-end Closing");

  help(sample_help);

  }


