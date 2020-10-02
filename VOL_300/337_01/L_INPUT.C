/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_INPUT.C   ***************************/

#include "mydef.h"
#include "stdio.h"

/*****************************************************************

 Usage: int  input(struct in_struc in_scrn[]);

  in_scrn[] = a array of struct in_struc.

  Creates a data input screen based on the data structure in_scrn[].
  Allows full editing of input screen until the user presses "Enter"
  on the last field, or presses "PgUp", "PgDn" or "Esc".

*****************************************************************/

int  input(struct in_struc in_scrn[])
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int number=0;
int i,x,y,done=FALSE;
char old_current=scr.current; /* save current attribute */
char temp_attr;
char ext;
int val_code;

/* search for terminating zero in data structure */
for(;;){
 if (in_scrn[number++].x==0) break;
 if (number==1000) return(1);  /* count too high, must not
                                  have terminator */
}
number--;  /* reset to correct value */

  /* write field prompts and highlight input area */
   for(i=0;i<number;i++){
    x= in_scrn[i].x; y= in_scrn[i].y;   /* calc. x-y coordinates */

    if (scr.mode==COLOR_80) /*if color then set label x_attribute */
        scr.current= set_color(in_scrn[i].label_f,
                               in_scrn[i].label_b);
      else
       scr.current=scr.normal;

    print(x,y,in_scrn[i].prompt);   /* print input label */

    if (scr.mode==COLOR_80)
     scr.current = set_color(in_scrn[i].input_f,in_scrn[i].input_b);
    else
       scr.current = scr.inverse;

    x=x+strlen(in_scrn[i].prompt)+1; /* move past input label */
        hilight_field(x,y,in_scrn[i].length,scr.current);

    print(x,y,in_scrn[i].ptr); /* show contents */
   }

   /* At this point we have set up the screen,
      we now input the data. */

   i=0;  /* use first field */

   /* this is the main loop in which editing occurs */
   while(!done){
    x= in_scrn[i].x; y=in_scrn[i].y;       /* get x,y coordinates */
    x=x+strlen(in_scrn[i].prompt )+1;      /* move past label */

/* set up field attribute */
    if (scr.mode==COLOR_80)
     scr.current = set_color(in_scrn[i].input_f,in_scrn[i].input_b);
      else
      scr.current=scr.inverse;

 for(;;){   /* loop until a valid field is read */
    gotoxy(x,y);
    /* get the string */
    ext=getfield(in_scrn[i].ptr,in_scrn[i].length,0,scr.current);

   if (ext==ESCAPE) break; /* don't validate if Escape pressed */

   /* validate the field */

   temp_attr=scr.current;  /* save our attribute */

   /* validate data entered */
   val_code=val_field(in_scrn[i].ptr,in_scrn[i].length,i);
   scr.current=temp_attr;

       if (val_code==REDO) putchar(BELL);
       if (val_code==OK) break;

   if (val_code==REDRAW) {         /* redraw field is requested */
      /* highlight input field to erase current contents */
      hilight_field(x,y,in_scrn[i].length,scr.current);
      print (x,y,in_scrn[i].ptr); /* print new contents */
      break;
   }
 } /* end for(;;) */

    if(ext== UP) i--;
    if(ext== DOWN|| ext==RETURN) i++;
    if(ext== ESCAPE || ext== PGUP || ext==PGDN) break;

    if (i==number && ext==RETURN ){     /* last field? */
      ext=RETURN;
      break;
    }
    if (i<0) i=(number-1);    /* check boundary conditions */
    if (i> (number-1)) i=0;

    }   /*end while (!done)*/

   scr.current=old_current;
   return(ext);
}
