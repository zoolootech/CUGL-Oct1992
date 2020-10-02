/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   MAKEHELP.C   ***************************/

/*
link with :
hlp_menu.c
hlp_io.c
*/

#include "mydef.h"
#include "help.h"
#include <stdio.h>

struct hlp help;


int start(void)      /* start is the entry point */
{
extern struct hlp help;
extern struct screen_structure scr;
extern struct window_structure w[];

cls();
 get_name(help.filename);       /* select a filename */
 if(help.filename[0]=='\0'){    /* if none selected */
  win_delete_top();
  cls();
  print(1,1,"A file must be selected.");
  exit();
 }

  /* make the three windows */
 help.edit=  win_make(2,3,help.width,help.height,STD_FRAME,
                     "Edit: Then 'Esc' ",scr.normal,scr.normal);
 help.menu=  win_make(1,1,80,1,NO_FRAME,"",scr.inverse,scr.inverse);
   cursor(NO_CURSOR);
 help.status= win_make(1,25,80,1,NO_FRAME,"",scr.inverse,
                       scr.inverse);
   cursor(NO_CURSOR);
  main_menu();    /* call the first menu */

/* clean up to exit */
  win_delete (help.edit);
  win_delete (help.menu);
  win_delete (help.status);

return (0);
}

/* this function allows editing of the help window */

void edit(int x, int y)
{
extern struct screen_structure scr;
extern struct window_structure w[];

char string[2];
char ch,ext;

 scr.current=win_what_attr(help.edit);
 ch= 0;
    while(ch != ESCAPE) {       /* read while key not 'Esc' */
     ch=0;ext=0;
     get_key(&ch,&ext);

      /* break while{} loop if PgUp or PgDn */
      if(ext==PGUP || ext==PGDN) break;  

       if(ch  > 31){                /* if character */
        string[0]=ch;string[1]='\0';/* build a string */
        print_here(string);         /* put on screen */
       x++;                         /* increment cursor location */
      }
      else{
        switch (ext){              /* if cursor key */
         case UP:y-- ;break;       /* act accordingly*/
         case DOWN:y++ ;break;
         case LEFT:x-- ;break;
         case RIGHT:x++ ;break;
         case HOME:x=1 ;break;
         case END:x=scr.right-scr.left+1;break;
        }
     } /* end else */
       if (ch==BACKSPACE)x--;
       if(ch==RETURN){
         x=1;
         y++;
       }

       /* the following code adjusts the cursor within the window */

       if(scr.left+x-1<scr.left){  /* too far left */
          x=scr.right-scr.left+1;
          y--;
        }

       if(scr.left+x-1>scr.right){x=1;y++;};    /* goto next line */
       if(scr.top+y-1<scr.top)y=scr.bottom-scr.top+1;
       if(scr.top+y-1>scr.bottom)y=1;

         if (ch=='\b') print(x,y," ");
         gotoxy(x,y);

   } /* end while ch!=27 */

   x=1;y=1;
   gotoxy(x,y);
}

/* this function creates an input screen for entry
   of new help screen sizes */

int get_size()
{
char width[3]="";
char height[3]="";
int in_window;
int return_code;

struct in_struc in_scrn[3]= {

/*
X   Y  Label name          Ptr   Length  Label-color  Field color */

1,  3, "Width  (10-78): ", NULL, 2,      WHITE,BLACK, BLACK,WHITE,
1,  4, "Height (5-23):  ", NULL, 2,      WHITE,BLACK, BLACK,WHITE,
0   /* terminator */
};
       in_scrn[0].ptr =width;
       in_scrn[1].ptr =height;

  cls();
  in_window= win_make(1,1,78,5,STD_FRAME,"",scr.normal,scr.normal);

  print(1,1,"PLEASE ENTER HELP SCREEN SIZE:");
  return_code = input(in_scrn);
  if(return_code==ESCAPE)
     return(return_code);
  else{
       help.width=atoi(width);       /* convert to integer */
       help.height=atoi(height);
      }
  win_delete(in_window);
  return(0);
}


/* val_field() is used by input routine to check validity of data */

int val_field( char *string,int length,int field_number)
{
int value;

 /* in this demo "length" is not used */

 int age;

 switch (field_number){
  case 0: value=atoi(string);         /* convert string to integer */
          if (value <10 || value >78) return(REDO);
           break;
  case 1: value=atoi(string);         /* convert string to integer */
          if (value <5 || value >23) return(REDO);
 }
return (OK);
}
