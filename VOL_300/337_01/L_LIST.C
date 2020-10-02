/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_LIST.C   ***************************/

#include "mydef.h"   /* always include this */
#include "stddef.h"  /* we need the definition of NULL from here */
#include "dos.h"     /* directory related header files */

#include "stdio.h"
#include "string.h"

/* load memory allocation header files for specific compiler */

#if defined QUICKC

#include "malloc.h"
#include "memory.h"

#endif

#if defined TURBOC

#include "alloc.h"
#include "mem.h"
#include "stdlib.h"
#include "dir.h"

#endif


/*****************************************************************

 Usage: int list_select (char *ptr[]);


        char *ptr[]= Array of pointers, each of which points to
                     to a option. The pointer following the last
                     option must be NULL.

 This function allows selection from a list of options.  The user
 may point to the option using the Up/Down cursor keys, and select
 by pressing Enter.   Pressing Escape exits without making a
 selection.

 The user may also Speed Search by typing in the option. As the
 user types in the text, the option most closely matching the typed
 text is highlighted.  Actual selection must still be made by
 pressing Enter.

 The number corresponding to the selection is returned.  A -1
 indicates the user pressed Escape without making a selection.

*****************************************************************/

int list_select (char *ptr[])
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int current_opt=0;    /* highlighted current option */
int top_opt=0;        /* first option in window */
int offset=0;         /* distance between top_opt and current_opt */
int last_opt;         /* the last option which was highlighted */
int height;           /* height of the window */
int last=0;           /* last element in pointer array */
int i=0,y,j=0;        /* general purpose */
char ch,ext;          /* character and extension*/
int sub_search=0;     /* index for location within search string */
int sub_option=0 ;    /* index for location within list options */
char search[MAX_STRING];  /* holds the speedsearch characters */
int redraw=TRUE;          /* flag which forces a redraw of selection
                             window */
int start,end;            /* pre-existing start end scan lines */
height = scr.bottom-scr.top +1;   /* height of selection window */

/* find out how many options are in the NULL terminated
   array of pointers */

for(last=0;ptr[last++]!=NULL;);
last-=2;     /* decrease by one so "last"
                now indicates the last option */

scr.current=scr.normal;
what_cursor(&start,&end);

/* draw initial window */

cursor(NO_CURSOR); /* turn off cursor */
y=1;

/* begin loop to process use selection */

 for(;;){

  if(redraw){     /* redraw the window contents */
   alt_screen(ON); /* turn on alternate screen so that
                      redraw appears to be instantaneous */
   cls();
   y=1;    /* y location (row) to print option */
   scr.current=scr.normal;      /* start with normal attribute */

    for(i=top_opt;i<top_opt+height;i++){
    if (ptr[i]==NULL) break;     /* stop if we go too far */
     if (i==current_opt)scr.current=scr.inverse;

      /* highlight current option */
      print (1,y++,ptr[i]);
      scr.current=scr.normal;   /* reset to normal */
    }
   alt_screen(OFF);
   redraw=FALSE;
  } /* end if (redraw) */

  ch='\0';ext='\0';       /* reset character and extension */
  get_key( &ch, &ext);    /* get character and extension */

  /* scan list of options match search key */

  /* make sure we have a valid letter, number or punctuation */
  if (ch>=32 && ch <=126 ) {

   /* here we search to find match for speed search letters */
   last_opt=current_opt;     /* save the highlight position */

   if(sub_search==0){     /* are we looking for the first letter
                             of the word? */
     for(i=0;i<=last;i++){  /* scan each list option */
       sub_option=0;    /* look for the first letter within option */
       while(ptr[i][sub_option]==' ') sub_option++; /* skip spaces */

       if(toupper(ptr[i][sub_option])==toupper(ch)){ /* match*/
            search[sub_search]=ch;  /* add it to the speed search
                                       string */
            offset=current_opt-top_opt;  /* difference between window
                                            top and highlighter */
            current_opt=i;      /* set current_opt to option found */
            sub_search++;       /* reposition indexes within search */
            sub_option++;       /* string and option */

            break;
       }
     }  /* end scan each list option */
   } /* end first letter */
   else{    /* No longer the first letter.
               The next loop examines further letters within the
               current highlighted option to see if they still
               match ch.*/

   if(toupper(ptr[current_opt][sub_option]) != toupper(ch)){
      /* Match with current highlighted option fails.
         Scan list to see if we can match string elsewhere. */

     search[sub_search]=ch;  /* finish building string */
     search[sub_search+1]='\0';
     sub_search=0;    /* reset to begin search at first letter */

      /* see if a match exists anywhere else */

      for(i=0;i<=last;i++){   /* examine each option */
       sub_option=0;          /* start at beginning of
                                 each option */
       while(ptr[i][sub_option]==' ') /* skip space */
           sub_option++;

           j=pos(&ptr[i][sub_option],search); /* is search string
                                                 in option? */
           if (j==0){                     /* search string found */
            offset=current_opt-top_opt;  /* calculate distance
                                            (top to current)*/
            current_opt=i;
            sub_search=strlen(search);
            sub_option=j+strlen(search);
            break;
           }
      } /* end (examine each option) */

   } /* end failed to match within current option */
     else
      {   /* current option still matches search key */
       search[sub_search]=ch;
       sub_search++;
      }
   } /* end of (not first letter) */

/* the following code moves the highlighter or
   requests a redraw as needed */


  /* is new selection beyond window?  */

  if(current_opt >top_opt+ height-1 || current_opt < top_opt){
    top_opt=current_opt-offset;   /* keep the offset the same */
    if (top_opt <0) top_opt=0;    /* correct if off top of window */
    redraw=TRUE;                  /* redraw window */
  } else{
               /* new option is within displayed page */
               /* un-highlight old position and redraw highlight */
     scr.current=scr.normal;  /* over-write highlighter with
                                 normal attribute */
     print(1,last_opt-top_opt+1,ptr[last_opt]);

     scr.current=scr.inverse;  /* print new highlighter */
     print(1,current_opt-top_opt+1,ptr[current_opt]);
    }

  } /* end valid letter */

  else /* not valid letter */
  {
   sub_search=0;  /* reset sub_search to look for first letter */

    if(ch==RETURN || ch==ESCAPE){ /* selection made or Escape key */
      scr.current= scr.normal;
      set_cursor(start,end);
     if (ch==RETURN) return(current_opt); /* item selected */
      else
       return(-1);         /* no selection */
    }
     y=current_opt-top_opt+1;   /* y = distance from top of window
                                   to highlighter */

   /* down arrow and not at bottom of window */
   if (ext==DOWN && y<height && current_opt<last ){
     /* overwrite highlighter with normal attribute */
     scr.current=scr.normal;
     print(1,y,ptr[current_opt]);

     scr.current=scr.inverse;
     current_opt++;                  /* increment list pointer */
     print(1,y+1,ptr[current_opt]);  /* print new highlighter */
   }
    else  /* down arrow and bottom of window */

   if (ext==DOWN && y==height && current_opt<last){

    /* rewrite current line with normal attribute */
    scr.current=scr.normal;
    print(1,y,ptr[current_opt]);
    scroll_up (1);                    /* scroll window up */
    current_opt++;                    /* increment positions */
    top_opt++;
    scr.current=scr.inverse;       /* print a new highlighter */
    print(1,y,ptr[current_opt]);
   }
   if (ext==UP && y>1){   /* up arrow and not at top of window */
     /* rewrite current line with normal attribute */
     scr.current=scr.normal;
     print(1,y,ptr[current_opt]);

     scr.current=scr.inverse;
     current_opt--;               /* increment list pointer */
     print(1,y-1,ptr[current_opt]); /* print new highlighter */
   } else
   if (ext==UP && y==1 && current_opt>0){  /* up arrow and top
                                              of window */
   /* rewrite current line with normal attribute */
    scr.current=scr.normal;
    print(1,y,ptr[current_opt]);
    scroll_down (1);                 /* scroll window down */
    current_opt--;                   /* move current_opt back */
    top_opt--;

      scr.current=scr.inverse;       /* print new highlighter */
     print(1,y-1,ptr[current_opt]);
   }

  /* page down and not at end of selection list */
  if(ext==PGDN && top_opt!= last) {
    offset=current_opt-top_opt; /* how far is the highlighter
                                   from the top of window? */
    top_opt+=height;            /* increase top_opt by the height
                                   of screen */
    current_opt=top_opt+offset; /* increase current_opt for
                                   new location */
     /* check for boundary errors */
    if(top_opt>last) top_opt=last;
    if(current_opt>last) current_opt=last;

    redraw=TRUE;  /* force a redraw of list options */

  }
  if(ext==PGUP && top_opt !=0){  /* PgUp pressed and not at top
                                    of window */
    offset=current_opt-top_opt;  /* move all positions up */
    top_opt-=height;
    current_opt=top_opt+offset;

    if(top_opt<0){     /* out of bounds ?*/
     top_opt=0;        /* reset */
     current_opt=offset;
    }
    redraw=TRUE;

  } /* end ext==PGUP */
  } /* end of not valid letter */

 } /* end of for(;;) loop which reads user selection*/
}
