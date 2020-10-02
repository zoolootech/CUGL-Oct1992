/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   PD.C   ***************************/

#include "mydef.h"
#include <stdio.h>


int pull_down (struct pd_str m_menu[], struct window_colors color)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int i,j;              /* general index variables */
int cur_x,cur_y;      /* screen coordinates */
int cur_opt=0;        /* the current option (highlighted) */
int found=FALSE;      /* flag to indicate if option found */
int expert=TRUE;      /* flag to indicate expert/novice mode */
char ch=' ', ext=' '; /* character and extension from keyboard */
int return_code;      /* return code */
int main_win;         /* integer handle for main options window */

/* find out if a frame is requested and make window */

if (PD_MAIN_FRAME[0]=='\0')  /* no frame requested */
    main_win=win_make(1,1,scr.columns,1,PD_MAIN_FRAME,""\
      ,color.main_frame,color.main_interior);
   else       /* frame requested */
     main_win=win_make(2,2,scr.columns-2,1,PD_MAIN_FRAME,"",\
      color.main_frame,color.main_interior);

  cursor(NO_CURSOR);   /* turn off cursor */

  for(;;){
       /* if a true letter is pressed */
       if(ch!= ' '&& ch!=RETURN){  
        for(i=0;i<MAIN_OPT;i++){   /* for each main option */
          j=0;
          while(m_menu[i].main[j]!= '\0'){   /* scan each letter */
            /* if quick key found */
            if (m_menu[i].main[j++] ==ch && ch != ' '){  
             found= TRUE;        /* mark found true */
             cur_opt = i;        /* mark it as current option */
             break;
            }
          }
        }
       }
          if (ch==RETURN){     /* if Enter (return) is pressed */
           found = TRUE;       /* accecpt current option */
           expert = FALSE;     /* action indicates non-expert */
          }

         /* reset variables */
         ch=' ';
         cur_x=1;cur_y=1;

       /* turn on bold caps if necessary */
           if (found || !expert )scr.bold_caps=FALSE;
             else scr.bold_caps=TRUE;

           for(i=0;i< MAIN_OPT;i++){  /* print options */
               if(i == cur_opt) scr.current= color.main_inverse;
                else scr.current= color.main_interior;
                 print(cur_x,cur_y,m_menu[i].main);
             /* move to next screen location*/
             cur_x= cur_x+strlen(m_menu[i].main)+3;  
           }

           if (!expert) found = TRUE; /* if not expert then     */
                                      /* force found=TRUE so    */
                                      /* sub-menu always called */

           if (found){                /* enter here if found==TRUE */

             /* redraw  main menu with bold_caps  off */
             scr.bold_caps=FALSE;

             /* call function to create pull-down */
             return_code =(pull_down_sub(m_menu,color,cur_opt,
                           &ext, &expert));
             win_pop_top(main_win);
             if(return_code!=0) break;

             /* reactivate expert mode */
             if (ext == ESCAPE) expert = TRUE; 

             /* if cursor keys used then not expert */
             if (ext == RIGHT || ext == LEFT) expert = FALSE;

             /* if cursor keys then adjust counter */
             if (ext==RIGHT) cur_opt++;
             if (ext ==LEFT) cur_opt--;
             ch= ' ';
             ext= ' ';

           } /* end if found */
            else
           {    /* if selection not made then get key */
            ch=' ';
            get_key(&ch,&ext);
            ch=toupper(ch);
           } /* end not found */

           if (ch==ESCAPE) break;   /* exit if "Escape" pressed */


           /* if cursor keys then not expert */
           if (ext ==RIGHT || ext == LEFT) expert = FALSE;
           /* adjust counter if cursor key used */
           if (ext == RIGHT)  cur_opt++;
           if (ext == LEFT)  cur_opt--;
           if (cur_opt >= MAIN_OPT) cur_opt =0;
           if (cur_opt < 0) cur_opt = MAIN_OPT-1;
           ext=' ';
           found=0;

  } /* end for(;;) */
  /* close window and return return-code */
  win_delete(main_win);
  return(return_code);
}


static int pull_down_sub(struct pd_str m_menu[],
                         struct window_colors color,int option,
                         char *ext, int *expert)
{

char ch=' ';

int i,j;              /* general index variables */
int y;                /* y screen coordinate (row) */
int start,width;      /* info for pull-down window */
int nu_opt;           /* the number of options in pull-down */
int cur_opt=0;        /* the current (highlighted) option */
int found= FALSE;     /* flag to indicate selection made (found)*/
int pd_win;           /* handle of pull-down window */
int return_code=0;    /* return code */

nu_opt=PD_SUB;   /* set nu_opt to maximum value */

scr.bold_caps=TRUE;   /* turn on bold_caps */

/* find out how many options are in pull-down menu */

for(i=0;i<nu_opt;i++){
  /* scan until empty string found */
  if (m_menu[option].sub[i][0] == '\0'){  
   nu_opt = i;
   break;
  }
}

start=3;  /* Figure where to draw pull-down box.
             The column must begin at least on the 3rd column.
             We can calculate the column at which to place
             the pull-down window by adding up the lengths
             of the main menu options appearing before
             the current option. */

  for(i=0; i< option; i++)  start= start+strlen(m_menu[i].main)+3;

  width=0;      /* figure max length of window, assume 0 to start */
  for (i=0;i< nu_opt;i++){
   if (strlen(m_menu[option].sub[i]) > width){
    /* set width to largest strlen */
    width= strlen(m_menu[option].sub[i]);    
   }
  }

 /* move box to left if it will spill off right side */
 if(start+width+1>scr.columns) start = scr.columns-width-2;

 /* create pull-down window based on calculated values */
 pd_win= win_make(start++,PD_SUB_ROW,width,nu_opt,PD_SUB_FRAME,"",\
          color.pd_frame,color.pd_interior); /*make a window */
 cursor(NO_CURSOR); /* turn off cursor for this window */

 y=1;  /* reposition in pull-down window for writing */

  for(;;){     /* begin endless loop while we process input */
         y=1;
        /* if a selection is made, turn off bold_caps */
        if (found )scr.bold_caps=FALSE;
         else scr.bold_caps=TRUE;

   /* print options in pull-down menu highlighing current option */
         for(i=0;i< nu_opt;i++){
           if(i == cur_opt) scr.current= color.pd_inverse;
            else scr.current= color.pd_interior;
            print(1,y++,m_menu[option].sub[i]);
         };

          if(found ) {  /* an option is selected */

            scr.bold_caps=FALSE;  /* turn off bold caps */
             /* if function pointer =NULL return specified code */
             /* else call function */
             if (m_menu[option].fun[cur_opt]==NULL)
              return_code=m_menu[option].select_id[cur_opt];
              else
              return_code=(*m_menu[option].fun[cur_opt])() ;
               win_pop_top(pd_win);

            /* exit pull-down if expert and return_code >0 */
            if(*expert==TRUE && return_code >=0)break;

            if(return_code>0) break;
            /* reset for next pass */
            if(return_code <0) return_code=0;
            found = FALSE;
            scr.bold_caps=TRUE;
            /* make sure keyboard buffer is clear */
            if (kbhit()) getch();  
          } /* end found */
          else {         /* begin not found */

            /* get a character */
            get_key(&ch,ext); ch=toupper(ch);  

            /* adjust cur_opt if up or down cursor key pressed */
            if (*ext == DOWN )  cur_opt = cur_opt +1;
            if (*ext == UP)  cur_opt = cur_opt -1;

            /* test for boundary and wrap if necessary */
            if (cur_opt >= nu_opt) cur_opt =0;
            if (cur_opt < 0) cur_opt = nu_opt-1;

           /* has a regular letter key been pressed? */
           if(ch=='\0') *expert=FALSE; 

           if (ch== RETURN){  /* user pressed "Enter" */
             found = TRUE;
             *expert=FALSE;
           }

           if(ch!='\0'){    /* check for quick keys */
            for(i=0;i<nu_opt;i++){   /* do for each option */
             j=0;

            /* check each letter in option to
               see if it matches character */

              while( m_menu[option].sub[i][j]!='\0'){
               if ( m_menu[option].sub[i][j++]==ch && ch != ' '){
                cur_opt = i;
                found = TRUE;
                break;
               }
              }
             } /* end for(); */
           }   /* end if(ch) */
          }    /* end else */

           /* check for options which would exit pull-down */
           if (*ext==LEFT || *ext==RIGHT) break;
           if (ch==ESCAPE){          /* EXIT IF ESCAPE KEY */
            *ext = ch;
            break;
           }
           *ext=' ';ch=' ';
  } /* end for(;;)*/

  /* exit pull-down */
  win_delete(pd_win);
  return (return_code);
}
