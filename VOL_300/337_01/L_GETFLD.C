/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_GETFLD.C   ***************************/

#include "mydef.h"
#include "stdio.h"


/*****************************************************************

 Usage: char getfield( char *string, int inlength, int start,
                      char attribute);

  char *string=   string to edit (if empty "" then accepts new text.
  int inlength=   maximum string length allowed.
  int start=      position within string to start editing.
  char attribute= text attribute to use to highlight field.

  Allows input of text.  If *string contains text, then *string
  may be edited.

  Editing keys (home,end etc) allowed.

*****************************************************************/

char getfield(char *string,int inlength,int start, char attribute)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

int width;
char left[255], right[255], chip[255];
char ch=' ',ext2=' ';
int i,x,y,orig_x,orig_y,temp_x,temp_y;
int insert=FALSE;
int letter=0,len_left=0, len_right=0;
int return_code;
int old_attribute=scr.current;

scr.current=attribute;     /* use specified attribute */
wherexy(&orig_x,&orig_y);  /* save original cursor position */
x=orig_x;y=orig_y;         /* set x,y to current cursor location */

hilight_field(x,y,inlength,attribute);  /* highlight input area */

width=scr.right-scr.left +1;  /* get the width of the window */

chip[0]= '\0'; left[0]= '\0';right[0]= '\0';

  strcpy(right,string);
  i=strlen(right);
  if(start>i)       /* break up string into left and right
                       components, depending on the requested
                       start position */
     chip_left(left,right,i);
   else
    chip_left(left,right,start);

 print(x,y,left);     /* print left string */
 wherexy(&x,&y);      /* store cursor edit position */
 print_here(right);   /* print right string */

 gotoxy(x,y);         /* move cursor back to edit location */

for (;;){    /* begin input cycle */
  ext2= 0; ch= 0;letter=0;

  len_left=strlen(left);     /* get string lengths */
  len_right = strlen(right); 

  get_key(&ch,&ext2);

  if (ch >=32) letter =TRUE;

  if (ext2 == INSERT){     /* toggle inset/overtype mode */
    insert = !insert;
    if(insert) cursor(2);  /* make big cursor to show insert */
    else cursor(1);
  }

   if(len_right>0) trim_right(right); /* Trim trailing blanks
                                         so we avoid problems
                                         with the insert
                                         feature. */
  len_left=strlen(left); len_right = strlen(right);

  if (!letter){         /* not regular letter */

   if (ext2 == LEFT && len_left >0){  /* if it is a left arrow  */

    chip_right(chip,left,1);  /* remove end of left and  */
    strcat(chip,right);       /* add to right            */
    strcpy(right,chip);
    x--; len_left--;
   }

   if (ext2 == HOME && len_left >0){ /* home */
    strcat(left,right);     /* concat stings */
    strcpy(right,left);     /* copy combined strings to right */
    len_left = 0;           /* set left length variable to zero */
    len_right = len_left+len_right;  /* set new right length */
    left[0] ='\0';                   /* set left string to "" */
    x=orig_x; y=orig_y;   /* move cursor to beginning of field */
   }

   if (ext2 == END){          /* end */
    print (x,y,right);        /* print right part of string */
    wherexy(&x,&y);           /* get cursor location */
    strcat(left,right);       /* combine left and right in left */
     len_right = 0;           /* new length to be zero */
     len_left = len_left+len_right;   /* new right length */
    right[0] ='\0';                   /* right ="" */
   }

   if (ext2 == RIGHT && len_right> 0){  /* if it is right arrow */
    chip_left(chip,right,1);      /* chip left end off right sting */
    strcat (left,chip);           /* add it to left string */
    x++;                 /* update cursor location */
    len_right++;         /* adjust length */
    gotoxy(x,y);        /* move cursor */
   }
   if (ext2 == DELETE && len_right >0){  /* delete */
    chip_left(chip,right,1); /* chip left end of right string */
    len_right--;            /* update length */
   }

  } /* end  if not letter */

 if (ch == BACKSPACE && len_left >0) {   /* if it is backspace */
  chip_right(chip,left,1);    /* chip right end of left string */
  x--;                        /* up-date cursor */
  len_left--;                 /* up-date length */
 }

/* enter here if it is letter   */

  if (letter){

   /* insert mode on and no room for char */
   if ((insert && (len_left +len_right >= inlength))){  
    putchar(BELL);  letter = 0;  /* no room for char */
   }
    else{
     if(insert){        /* insert mode on */
      x++;              /* update cursor location */
      /* add char to string */
      left[len_left++] = ch; left[len_left]= '\0'; 
     }
      if(!insert){      /* in overtype mode */
         if( len_left ==inlength){    /* no room for character */
          putchar(BELL);              /* "beep" the user */
          letter =0;
         }
         else
        {
         /* add new character, overtype old character */
         chip_left(chip,right,1);   /* chip off left end of
                                       right string */
         left[len_left++] = ch ;/* add new character */
         left[len_left]= '\0'; 
         len_right--;      /* record new right length */
         x++;
        }
      }  /* not insert */

    }
  }            /* end if (letter) */

        /* check for boundry conditions */

      if (x<1 && y == orig_y) x=1; else

      if (x<1){ y--; x=width;}
      if (x>width){   /* have we reached the end of the window ? */
        y++;x=1;
      }

      if (ext2 == LEFT || ext2 == RIGHT|| ext2 == HOME) gotoxy(x,y);

      /* if backspace or delete then redraw the line */
      if( ch == BACKSPACE || ext2 == DELETE){   
         gotoxy(x,y);                            
         temp_x=x; temp_y=y;
          /* print right string (don't move cursor) */
          dma_print(&temp_x,&temp_y,right); 
          /* print  blanks to cover old trailing text */
          dma_print(&temp_x,&temp_y," ");   
      }
          if (letter ){
            chip[0]=ch;chip[1]='\0'; /* turn character into string */
            print_here(chip);  /* print string at current location */
            wherexy(&x,&y);    /* get new location */

            if (insert){
               temp_x=x; temp_y=y;
               dma_print(&temp_x,&temp_y,right);
             }
          }

   if (ch==ESCAPE || ext2 == UP || ext2 == DOWN) break;
   if (ext2 == PGUP || ext2 == PGDN) break;
   if (ch==RETURN /*|| ext2 == UP || ext2 == DOWN*/) break;

}  /* end of for loop */

 switch(ext2){
  case UP :return_code=UP;break;
  case DOWN:return_code=DOWN;break;
  case PGUP:return_code=PGUP;break;
  case PGDN:return_code=PGDN;break;
 }

if (ch==RETURN) return_code = RETURN;
if (ch==ESCAPE) return_code = ESCAPE;
 strcat(left,right);
 strcpy(string,left);
cursor(NORMAL_CURSOR); /* set normal cursor */
scr.current=old_attribute;
return(return_code);

}


/*****************************************************************

 Usage: static hilight_field (int x, int y, int length,
                              char attribute);


  int x,y =        column,row to place highlighted field.
  int length=      length of highlighted area.
  char *attribute= text attribute to use for highlighted area.


  Creates a highlighted area on the screen.

*****************************************************************/

void hilight_field (int x, int y, int length, char attribute)
{
extern struct  screen_structure scr;
extern struct window_structure w[];

char hilight[MAX_STRING];
int i;
char old_attribute=scr.current;  /* save current attribute */
scr.current=attribute;  /* set current attribute */

    for(i=0;i<length;i++){  /* build a string of blanks */
      hilight[i]=' ';
    }
    hilight[i]='\0'; /* terminate string */

    dma_print(&x,&y,hilight); /* use dma_print which does
                                 not move the cursor */

   scr.current=old_attribute; /* restore current attribute */
}
