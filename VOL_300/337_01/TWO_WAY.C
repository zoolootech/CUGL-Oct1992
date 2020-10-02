/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   TWO-WAY.C   ***************************/

/* demonstrates partial/full menus */

#include "mydef.h"
#include <stdio.h>

/* defines */

#define FULL    1
#define PARTIAL 2

/* function prototypes */

int fake(void);


int start()
{
extern struct screen_structure scr;
extern struct window_structure w[];

struct pop_struc *menu;
int return_code;
int i;
char string[MAX_STRING];


 /* set up menu structure */

 struct pop_struc partial[7]={       /* partial menu */
 /*  *name   (*fun)()  select_id  */
    " Load    " ,fake,   0,
    " Save    " ,fake,   0,
    " Compile " ,fake,   0,
    " Run     " ,fake,   0,
    " Full    " ,NULL,   FULL,       /* return "FULL" */
    " Quit    " ,NULL,   0,
    "\0"      /* mark the end of the options list */
 };

 struct pop_struc full[11]={         /* full menu */
/*  *name   (*fun)()  select_id  */
    " Load    " ,fake,   0,
    " Save    " ,fake,   0,
    " Compile " ,fake,   0,
    " Edit    " ,fake,   0,
    " Run     " ,fake,   0,
    " Debug   " ,fake,   0,
    " Options " ,fake,   0,
    " proJect " ,fake,   0,
    " Partial " ,NULL,   PARTIAL,       /* return "PARTIAL" */
    " Quit    " ,NULL,   0,
    "\0"      /* mark the end of the options list */
 };

menu= partial;       /* set *menu to point to the partial menu */


   /* loop until user presses "Escape" or selects "Quit" */
   for(;;){

   /* call pop_up() giving it the partial menu */
    return_code= (pop_up(menu,33,10,scr.normal,scr.inverse));

      switch (return_code){
       case FULL:    menu=full;break;     /* select full menu */
       case PARTIAL: menu=partial;break;  /* select partial menu */
      }
     if (return_code==0) return(0);       /* Quit or Escape */
   }
}


int fake(void)
{
return(0);  /* return a zero so the menu does not close */
}
