/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   BARDEMO2.C   ***************************/

/* Demonstration of a stacked (multi-level) horizontal
   moving light bar menu. */

#include "mydef.h"
#include <stdio.h>

/* function prototypes */

int fake(void);
int prtdemo(void);
int printer(void);
int lpt1(void);
int lpt2(void);
int text(void);


int start(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int win1;
int return_code;

 /* prepare menu */
 struct bar_struc main_menu [6]={
            "Sort",     "Sort the data file",        fake,    0,
            "Print",    "Printer, Text-file" ,       prtdemo, 0,
            "Delete",   "Delete current data file",  fake,    0,
            "Copy",     "Copy data file to backup",  fake,    0,
            "Quit",     "Quit and return to DOS",    NULL,    1,
            "\0"      /* mark the end of the options list */
 };

   cls();
   /* make a window for the menu */
   win1=win_make(2,2,scr.columns-2,2,STD_FRAME,"",
                 scr.current,scr.current);

    /* create the first menu */
    return_code= bar_menu(main_menu,scr.normal,scr.inverse);

    win_delete(win1);      /* remove window */
    return(return_code);  /* pass along the return code */
}


int prtdemo(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int win2;
int return_code;

 struct bar_struc main_menu [3]={
            "Printer",   "Lpt1:, Lpt2:",               printer, 0,
            "Text-file", "Send output to a text file", text,    0,
            "\0"      /* mark the end of the options list */
 };

   win2=win_make(2,4,scr.columns-2,2,STD_FRAME,"",
                 scr.current,scr.current);

return_code= bar_menu(main_menu,scr.normal,scr.inverse);

  win_delete(win2);

  return(0); /* we don't want to close the parent menu
                so we return a zero value instead of
                "return_code" */
}


int printer(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int win3;
int return_code;

 struct bar_struc main_menu [3]={
  "Lpt1:",   "Route output to printer on port lpt1:", lpt1, 0,
  "lPt2:",   "Rout output to printer on port lpt2:",  lpt2, 0,
  "\0"      /* mark the end of the options list */
 };

   win3=win_make(2,6,scr.columns-2,2,STD_FRAME,"",
                 scr.current,scr.current);

return_code= bar_menu(main_menu,scr.normal,scr.inverse);

 win_delete(win3);
 return(return_code);
}


int lpt1(void)
{
return(1);  /* return a non-zero number to close menu */
}


int lpt2(void)
{
return(1);  /* return a non-zero number to close menu */
}


int text(void)
{
return(1);  /* return a non-zero number to close menu */
}

int fake(void)
{
return(0);  /* return a zero to close menu */
}
