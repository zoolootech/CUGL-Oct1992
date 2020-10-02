/* Copyright (c) James L. Pinson 1990,1991  */

/**********************    L_BAR.C    ***************************/

#include "mydef.h"
#include "stdio.h"


/*****************************************************************

 Usage: int bar_menu(struct bar_struc bar_menu[], char normal,
                     char inverse)

  struct bar_struc bar_menu=  data structure containing menu options.

  char normal,inverse= text attributes used for regular and
                       highlighted options.

  Creates and manages a moving light bar menu at the first line of
  the topmost (active) window.

*****************************************************************/

int bar_menu(struct bar_struc menu[], char normal, char inverse)
{

extern struct  screen_structure scr;
extern struct window_structure w[];

int x=1;       /* x screen location */
int y=1;       /* y screen location */
int i,j;       /* index variables */
int nu_opt;    /* number of options in menu */
int old_caps=scr.bold_caps;  /* original value of bold_caps */

int cur_opt;   /* current menu (highlighted option) */

char ch;       /* char and extension variables */
char ext;

int found = FALSE;    /* selection made (found) flag */
int return_code=0;
int my_win=scr.active;

cls();
cursor(NO_CURSOR); /* turn off cursor */
 /* set number of options to max */

/* figure how many options there are */

for(i=0;i<MAX_BAR;i++){
  if (menu[i].name[0] == '\0'){
   nu_opt = i;
   break;
  }
}

cur_opt = 0;

   for(;;){       /* loop infinite */
         x=1;

          scr.bold_caps=!found;  /* turn off caps when found */

         for(i=0;i< nu_opt;i++){    /* print all options */
           if(i == cur_opt) scr.current= inverse;
            else scr.current= normal;
            print(x,y,menu[i].name);
            x=x+strlen(menu[i].name)+3; /* move option location */

         };  /* end of menu printing loop */

           if(menu[cur_opt].info[0]!='\0'){ /* is there info? */
            scr.current=normal;    /* then print it on next line */
            scr.bold_caps=FALSE; /* don't highlight bold caps */

            ceol(1,y+1);    /* clear to end of line
                               to clear old info */
            /* print new information */
            print(1,y+1,menu[cur_opt].info);    
           }

  if(found ){        /* selection made.
                        return correct return code if function
                        pointer NULL */
   if(menu[cur_opt].fun==NULL) return(menu[cur_opt].select_id);
      else{  /* else run option to get code */
        return_code= (*menu[cur_opt].fun)() ;  
        win_pop_top(my_win);
        ceol(1,1);
      }
        found = FALSE;   /* reset flag, ready for new selection */

      if (kbhit()) getch(); /* make sure keyboard buffer is clear */

  }
   else{       /* selection not made
                  read keys until selection is made */

        get_key(&ch,&ext); ch=toupper(ch);  /* get a character */
        if (ext == RIGHT)  cur_opt = cur_opt +1;  /* move right */
        if (ext == LEFT)  cur_opt = cur_opt -1;   /* move left */
        if (cur_opt >= nu_opt) cur_opt =0;        /* wrap if out of
                                                     bounds */
        if (cur_opt < 0) cur_opt = nu_opt-1;

        if (ch== RETURN) found = TRUE;

     /* if we have a valid character then it may be a quick key */
     if(ch!='\0'){
        for(i=0;i<nu_opt;i++){    /* does it match an option? */
         j=0;                     /* scan each menu option name */

      /* check each letter within option */
      while(menu[i].name[j]!= '\0'){
          /* if match and not space */
          if ( ch==menu[i].name[j++] && ch != ' '){
           cur_opt = i;            /* mark found flag and break */
           found = TRUE;
           break;
          }
       }
       if(found==TRUE) break;   /* break if found */
     }
   }
           if (ch==ESCAPE){          /* EXIT IF ESCAPE KEY */
            return_code = 0;         /* exit but don't close down */
            break;                   /* parent menu */
           }
           ext=ch=' ';

           }  /* end else */
          /* a non-zero return code means exit menu */
          if (return_code!=0) break;

   } /* end for(;;)*/

 scr.bold_caps=old_caps; /* restore old value */
 return (return_code);
}
