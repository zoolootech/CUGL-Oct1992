/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   HLP_MENU.C   ***************************/

/*
link with:
makehelp.c
hlp_io.c
*/

#include "mydef.h"

#include "help.h"
#include <stdio.h>

#if defined QUICKC

#include <malloc.h>
#include <memory.h>

#endif

#if defined TURBOC

#include <alloc.h>
#include <mem.h>
#include <string.h>
#include <stdlib.h>

#endif

/* these functions create the menus used by makehelp.c */

void main_menu(void)   /* the main menu */
{
extern struct screen_structure scr;
extern struct window_structure w[];

struct bar_struc main_menu [5]={
    "Browse" ,"",browse,0,
    "Add" ,"",add,0,
    "New-file" ,"",file,0,
    "Quit","",NULL,1,
    "\0"
  };

  status(0);

 bar_menu(main_menu,scr.inverse,scr.normal);
 };


int browse(void)     /* page browsing menu */
{
extern struct hlp help;
extern struct screen_structure scr;
extern struct window_structure w[];

int current_page=0;
int done=FALSE;
int last_load;
int ret_code;

  struct bar_struc browse_menu [5]={
    "Next " ,     "", NULL,1,
    " Previous ", "", NULL,2,
    " Edit " ,    "", NULL,3,
    "Quit ",      "", NULL,4,
    "\0"
  };

if (help.number_pages==0) return(0);

  while(!done){
    alt_screen(ON);
    win_pop_top(help.edit);
    scr.current=win_what_attr(help.edit);
    load_page(current_page);
     status(current_page+1);
     win_pop_top(2);
     alt_screen(OFF);

     ret_code=bar_menu(browse_menu,scr.inverse,scr.normal);

     switch(ret_code){
       case 0: break;
       case 1: current_page++;break;
       case 2: current_page--;break;
       case 3: cls(); win_pop_top(help.edit);
         for(;;){

             /* get edit window attribute */
          scr.current=win_what_attr(help.edit);
           edit(1,1);  /* edit window */

           /* pop up the menu and status windows*/
           win_pop_top(help.status);
           win_pop_top(help.menu);
           ret_code=verify_save();
             if (ret_code==0)break;

              if(ret_code==1){
                save_page(current_page);
                break;
              }

              if(ret_code==2)win_pop_top(help.edit);
              if(ret_code==3) break;
            }/* end for(;;) */
             break;

       case 4: done=TRUE;

     }  /* end switch */

  if (current_page==help.number_pages)
      current_page=help.number_pages-1;
  if (current_page<0)current_page=0;

  } /* end while(!done)  */

 win_cls(help.edit);win_redraw_all();
 status(0);  /* update status window */
 return(0);
}


int file(void)      /* get the name of a new help file */
{
extern struct hlp help;
extern struct screen_structure scr;
extern struct window_structure w[];

 char old_name[80];
 strcpy(old_name,help.filename);
 get_name(help.filename);
 if((strcmp(old_name,help.filename))!=0){
   /* delete old edit window, create new one */
   win_delete(help.edit);
   help.edit= win_make(2,3,help.width,help.height,STD_FRAME,
                      "Edit: Then 'Esc' ",scr.normal,scr.normal);
     win_cls(help.status); /* clear status window */
     status(0);            /* create new one */
 }
 return(0);
}


int verify_save(void)   /* verify the user wants to save the page */
{
extern struct hlp help;
extern struct screen_structure scr;
extern struct window_structure w[];

int return_code;
 /* no function pointers, we want return codes only */
  struct bar_struc main_menu [4]={
    "Save",  "", NULL,1,
    "Edit",  "", NULL,2,
    "Quit" , "", NULL,3,

    "\0"
  };

 return(bar_menu(main_menu,scr.inverse,scr.normal));
}


int add(void)   /* add a new help page */
{
extern struct hlp help;
extern struct screen_structure scr;
extern struct window_structure w[];

int done=FALSE;
char *ptr=NULL;
char *temp;
char far *scrn_ptr;
char ch;
int i,j;
int return_code;

cls();

win_pop_top(help.edit);
scr.current=win_what_attr(help.edit);

/* allocate space, allow for \0 terminator */
ptr=(char *)malloc ((help.width*help.height)*sizeof(char));
status(help.number_pages+1);
win_pop_top(help.edit);
edit(1,1);

while(!done){
  status(help.number_pages+1);
  win_pop_top(help.menu);

  return_code=verify_save();   /* save file?*/

   if ( return_code==1){   /* save help page to file */
     win_pop_top(help.edit);
     /* scan the help window for characters */

      temp=ptr;    /* set temp pointer = pointer */

      for(i=0;i<help.height;i++){  /* get each row */

       scrn_ptr=(char far *)(scr.buffer+(scr.top+i-1)*
                (scr.columns*2)+2*(scr.left-1));

        for(j=0;j<help.width;j++){
         *temp=*scrn_ptr;
         temp++;scrn_ptr+=2;
        }
 
       }
       /* save image to file */
      append(help.filename,ptr,(help.width*help.height*
             sizeof(char)));
      help.number_pages++;
      ch=' ';

     status(help.number_pages+1);
     win_pop_top(help.menu);

     ceol(1,1);
     print(1,1, "Add another page ");
     scr.bold_caps=TRUE;
     print_here("Y/N? ");
     scr.bold_caps=FALSE;
     while(ch != 'Y' && ch !='N'){
      ch=toupper(getch());
     }

     if(ch=='N') done=TRUE;

     else{               /* edit a new page */
        cls();
        win_pop_top(help.edit);
        cls();
        edit(1,1);
     }
   } /* end return code==1 */

    if(return_code==2) {
     win_pop_top(help.edit);
     edit(1,1);
    }
    if(return_code==3){
     done=TRUE;

    }

} /* end while !done */

if(ptr!=NULL)free(ptr);
win_cls(help.menu);
win_cls(help.edit);
win_redraw_all(); status(0); return(0);

}

void status(page)        /* display the status line */
{
extern struct hlp help;
extern struct  screen_structure scr;
extern struct window_structure w[];

 char string[20];
 win_pop_top(help.status);
 ceol(1,1);
 scr.current=win_what_attr(help.status);
 print(1,1,help.filename);
 sprintf(string,"Page %3i of %d",page,help.number_pages);

 if(page>help.number_pages)
  strcpy(string,"NEW-PAGE");
 print(60,1,string);
 win_pop_top(help.menu);
}
