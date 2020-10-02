/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   BARDEMO.C   ***************************/

/* Demonstration of a un-stacked horizontal moving light bar menu.*/

#include "mydef.h"
#include "stdio.h"

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

 struct bar_struc main_menu [6]={

 /* *name      *info                        (*fun)()   select_id */

   "Sort",     "Sort the data file",        fake,      0,
   "Print",    "Printer, Text-file" ,       prtdemo,   0,
   "Delete",   "Delete current data file",  fake,      0,
   "Copy",     "Copy data file to backup",  fake,      0,
   "Quit",     "Quit and return to DOS",    NULL,      1,
   "\0"      /* mark the end of the options list */
  };

 cls();

 /* make a window for the bar menu */
 win1=win_make(2,2,scr.columns-2,2,STD_FRAME,"",
              scr.current,scr.current);

 /* call the bar menu*/

 return_code= bar_menu(main_menu,scr.normal,scr.inverse);

 win_delete(win1);  /* delete window */
 return(return_code);  /* pass along the return code */
}


int prtdemo(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int win1;
int return_code;

 /* set up new menu */
 struct bar_struc main_menu [3]={

  /*
  *name        *info                         (*fun)()  select_id */
  "Printer",   "Lpt1:, Lpt2:",               printer,  0,
  "Text-file", "Send output to a text file", text,     0,
  "\0"      /* mark the end of the options list */
 };

 cls();  /* clear the existing menu window */
 return_code= bar_menu(main_menu,scr.normal,scr.inverse);

 return(0); /* return a zero instead of "return_code"
               so previous menu remains open
               so we don't close the main menu */
}


int printer(void)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int win1;
int return_code;


 struct bar_struc main_menu [3]={

 /*
  *name       *info                                  (*fun)()  id */

  "Lpt1:",   "Route output to printer on port lpt1:", lpt1,     0,
  "lPt2:",   "Route output to printer on port lpt2:", lpt2,     0,
  "\0"       /* mark the end of the options list */
 };

cls();  /* clear the existing menu window */

return(bar_menu(main_menu,scr.normal,scr.inverse));

}


int lpt1(void)
{
return(1);  /* return a non-zero number so that menu closes down */
}

int lpt2(void)
{
return(1);  /* send back a non-zero number to close the menu */
}

int text(void)
{
return(1);  /* send back a non-zero number to close the menu */
}

int fake(void)
{
return(0);  /* send back a zero and keep the menu open */
}
