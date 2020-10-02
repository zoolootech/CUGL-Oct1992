/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_POPUP.C   ***************************/

#include "mydef.h"
#include <stdio.h>


/*****************************************************************

 Usage: int pop_up(struct pop_struc pop_menu[],int x,int y,
                   char normal, char inverse)


  struct pop_struc pop_menu=  data structure containing menu
                              options.

  int x,y= upper left corner of pop_up window.

  char normal,inverse= text attributes used for regular and
                       high-lighted options.

  Creates and manages a pop-up menu at the location specified.

*****************************************************************/

int pop_up(struct pop_struc pop_menu[],int x,int y, char normal,
           char inverse)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int col;              /* screen column */
int i=0,j;            /* general purpose index variables */
int width;            /* width of window */
int nu_opt;           /* number of options */
int cur_opt;          /* current option (highlighted */
char ch,ext;          /* character and extension */
int found = FALSE;    /* flag to indicate option found(selected) */
int return_code;      /* return code */
int pop_window;
int old_caps=scr.bold_caps;  /* the original value of bold caps */
/* set on bold caps to highlight menu quick keys */
scr.bold_caps=ON;  

/* figure how many options there are */

nu_opt=0;

/* loop until empty string found */
while (pop_menu[i++].name[0]!='\0'); 

nu_opt=i-1;   /* set nu_opt to the number of options found */

/* figure size of box */

  width=0;      /* figure max length of window */

  for (i=0;i< nu_opt;i++){                   /* for each option */
   /* find largest option length */
   if (strlen(pop_menu[i].name) > width){
    width= strlen(pop_menu[i].name);   
   }
  }

/* make a window based on x,y parameters
   and calculated width and height */

pop_window= win_make(x,y,width,nu_opt,STD_FRAME,"",normal,normal);

cursor(NO_CURSOR);    /* hide cursor */

scr.current = scr.normal;

cur_opt = 0;   /* first option */

  /* infinite loop */
   for(;;){

   scr.bold_caps=!found; /* turn off scr.bold_caps if true */

       /* print menu options, highlight current option */
         col=1;                     /* start at first column */
         for(i=0;i< nu_opt;i++){    /* print each option */
           /* highlight current option */
           if(i == cur_opt) scr.current= inverse;  
            else scr.current=normal;          /* else normal */
            print(1,col++,pop_menu[i].name);
         };

          if(found ) {          /* selection found */

            /* return specified code if NULL */
            if(*pop_menu[cur_opt].fun==NULL){   
             return_code=pop_menu[cur_opt].select_id;
             break;
            }
            else
              /* a function was specified by pointer,
                 call the function and get code */

             return_code=(*pop_menu[cur_opt].fun)() ;

             win_pop_top(pop_window);
             /* a non-zero value is signal to exit */
             if(return_code!=0) break;  
             found = FALSE;
             /* make sure keyboard buffer is clear */
             if (kbhit()) getch();  

          } /* end if(found) */

          else{  /* not found */
           /* read keys until selection is made */
           get_key(&ch,&ext); /* get a character */
           ch=toupper(ch);    /* make it upper case */
          }
           if (ext == DOWN)  cur_opt++;  /* move down */
           if (ext == UP)    cur_opt--;  /* move up */
           /* wrap if boundaries exceeded */
           if (cur_opt >= nu_opt) cur_opt =0;
           if (cur_opt < 0) cur_opt = nu_opt-1;

          if (ch== 13) found = TRUE;

          if(ch!='\0'){  /* do we have a letter? */
            for(i=0;i<nu_opt;i++){   /* scan each option? */
             j=0;

             /* check each letter within option */
             while(pop_menu[i].name[j]!=0){  
               /* ignore spaces in string */
               if (pop_menu[i].name[j++]==ch && ch != ' '){
                cur_opt = i;
                found = TRUE;
               }
             } /* end while */
            }
           } /* end ch!='\0' */


           if (ch==ESCAPE){  /* EXIT IF ESCAPE KEY */
            return_code=0;
            break;
           }
           ext=' ';ch=' ';
   } /* end for(;;)*/

   scr.bold_caps=old_caps;   /* restore bold caps */
   win_delete_top();         /* remove top window */

   return (return_code);
}
