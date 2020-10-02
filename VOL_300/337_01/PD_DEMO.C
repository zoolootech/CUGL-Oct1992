/* Copyright (c) James L. Pinson 1990,1991  */


/**********************   PD-DEMO.C   ***************************/


/* Link with pd.c and library */

#include "mydef.h"
#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include <stdlib.h>

/* the number of main options is defined in mydef.h as MAIN_OPT */

/* the maximum number of suboptions is defined in
   "menu.h" as  PD_SUB  */

/* function prototypes */

int demo();   
void howto(void);
int printer(void);


int start(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

char ch,ext;
int return_code;
int i;
int menu_win;

struct pd_str m_menu [MAIN_OPT]={

/* main menu   sub-menu       *function     list select values  */

"Sort",
              " Ascending  ",
              " Descending ",
              "",
                              demo,
                              demo,
                              NULL,
                                            0,
                                            0,
                                            0,
"Print",
              " Printer   ",
              " Text-file ",
              "",
                              printer,
                              demo,
                              NULL,
                                            0,
                                            0,
                                            0,
"Delete",
              " Data-file    ",
              " program-File ",
              " Programmer   ",

                              demo,
                              demo,
                              demo,
                                            0,
                                            0,
                                            0,
"Copy",
              " Block ",
              " Line  ",
              "",
                              demo,
                              demo,
                              NULL,
                                             0,
                                             0,
                                             0,
"Quit",
              " Exit program ",
              "",
              "",
                              NULL,
                              NULL,
                              NULL,
                                            1,
                                            0,
                                            0,
};

/* colors for main and pull-down windows */
struct window_colors color;   

/*initilize window colors*/

if (scr.mode==COLOR_80){
 color.main_frame=set_color(YELLOW,BLACK);
 color.main_interior=set_color(WHITE,BLUE);
 color.main_inverse=set_color(BLUE,WHITE);

 color.pd_frame=set_color(YELLOW,BLACK);
 color.pd_interior=set_color(WHITE,BLUE);
 color.pd_inverse=set_color(BLUE,WHITE);
 }
 else{

 color.main_frame=scr.normal;
 color.main_interior=scr.normal;
 color.main_inverse=scr.inverse;

 color.pd_frame=scr.normal;
 color.pd_interior=scr.normal;
 color.pd_inverse=scr.inverse;
 }

howto();       /* SHOW INSTRUCTIONS */

   return_code=pull_down(m_menu,color);

 cursor(NORMAL_CURSOR);  /* restore normal cursor */

cls();

return(return_code);
}

void howto()
{
  if (scr.mode==COLOR_80)
   scr.current=  set_color(GREEN,BLACK);
    else
     scr.current = NORMAL;

 cls();
 print (1,4,"INSTRUCTIONS:");
 print (1,6,"EXPERT MODE: Select by touching the\
 key which represents each option");
 print (14,7,"(the capital letter).");

 print (1,10, "ASSIST MODE: Pull down menu by touching\
 Enter or a cursor key.");
 print (14,11,"Select by highlighting with cursor keys -\
 then touch Enter.");
 print (14,13,"Return to Expert mode by touching Escape.");

 print (1,15,"EXIT:        To exit to DOS, touch\
 Escape while in Expert mode.");

}


int demo(void)
{
int demo_win;
char ch,ext;

 demo_win=win_make(24,10,30,5,STD_FRAME," Demo window ",
                   scr.normal,scr.normal);

  print(1,1,"Make believe window");

  scr.bold_caps=TRUE;
    print(1,3,"TOUCH ANY KEY TO EXIT");
  scr.bold_caps=FALSE;
     get_key(&ch,&ext);
  win_delete(demo_win);
 return(0);
}


int printer(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];


 /* set up menu structure */

 struct pop_struc pop_menu [3]={
            " Lpt1:   " ,NULL,   0,
            " lPt2:  "  ,NULL,   0,
            "\0"      /* mark the end of the options list */
 };

/* call routine to handle menu */
return (pop_up (pop_menu,25,5,scr.normal,scr.inverse));
  
}
