/* Copyright (c) James L. Pinson 1990,1991  */

/**********************  POPDEMO.C  ***************************/

/* demonstrates the use of pop-up menus */

#include "mydef.h"
#include <stdio.h>

/* function prototypes */

int fake(void);
int prtdemo(void);
int printer(void);
int lpt1(void);
int lpt2(void);
int text(void);


int start()
{
extern struct screen_structure scr;
extern struct window_structure w[];

int i;
char string[MAX_STRING];

 /* set up menu structure */

 struct pop_struc pop_menu [6]={
/*  *name   (*fun)()  select_id  */
    " Sort   " ,fake,   0,
    " Print  " ,prtdemo,0,
    " Delete " ,fake,   0,
    " Copy   " ,fake,   0,
    " Quit   " ,NULL,   1,  /* returns a 1 when selected */
    "\0"      /* mark the end of the options list */
 };


cls();

/* fill the screen with dots '.'*/
/* make up a string of '.' wide enough to fill each row */

  for(i=0;i<scr.columns;i++) string[i]='.';  /* build the string */
  string[i]= '\0';                           /* terminate it */

/* now fill each row of the screen with the string of '.' */

  for (i=1;i<=scr.rows;i++) print(1,i,string);

/* create the pop-up menu */
return (pop_up (pop_menu,33,10,scr.normal,scr.inverse));
        /* pop-up appears at column,row 33,10 */

}  /* end of start(); */


int prtdemo()
{
extern struct screen_structure scr;
extern struct window_structure w[];

int return_code;

 /* set up menu structure */

 struct pop_struc pop_menu [3]={

/*  *name        (*fun)()  select_id  */
 " Printer   "  ,printer,  0,
 " Text-file  " ,text,     0,
 "\0"      /* mark the end of the options list */
 };

/* call routine to handle menu */
return_code= (pop_up (pop_menu,41,13,scr.normal,scr.inverse));   
             /* pop-up appears at column-row */
             /* 41,13 */

/* we could examine return_code here if necessary */

return(0); /* return a zero so the parent menu does not close */
}


int printer()
{
extern struct screen_structure scr;
extern struct window_structure w[];

 /* set up menu structure */

 struct pop_struc pop_menu [3]={

/*  *name        (*fun)()  select_id  */
  " Lpt1:   ",   lpt1,     0,
  " lPt2:  " ,   lpt2,     0,
  "\0"      /* mark the end of the options list */
 };

/* call routine to handle menu */
return (pop_up (pop_menu,53,15,scr.normal,scr.inverse));   
        /* pop-up appears at column-row 53,15 */
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
return(0);  /* return a zero so the menu does not close */
}
