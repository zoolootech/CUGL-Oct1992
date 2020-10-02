/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   IN-DEMO.C   ***************************/

#include "mydef.h"
#include <stdio.h>
#include <ctype.h>

#define NUMBER_FIELDS 12  /* the maximum number of fields */

/* for memory allocation */
#if defined QUICKC

#include <malloc.h>
#include <memory.h>

#endif

#if defined TURBOC

#include <alloc.h>
#include <mem.h>
#include <string.h>
#include <stdlib.h>
#include <dir.h>

#endif

/* function prototypes */

void help();
int val_field( char *string,int length,int field_number);

int start(void)
{
extern struct screen_structure scr;
extern struct window_structure w[];

int i;
int helpwin, mainwin;  /* window handles */

/* make a help screen */

helpwin= win_make(1,20,78,5,STD_FRAME,"",scr.normal,scr.normal);

help();  /* display help */

/* make the main window */
mainwin=win_make (1,1,78,17,TOP_FRAME,"",scr.normal,scr.normal);

demo();  /* do input demo */

/* remove windows */

win_delete(helpwin);
win_delete(mainwin);

return(0);
}


demo()
{
extern struct screen_structure scr;
extern struct window_structure w[];

int field_attr;
int ret_code;
char ch,ext;
     /* make in_scrn large enough to include zero terminator */
struct in_struc in_scrn[NUMBER_FIELDS+1]= {
/*
X   Y   Label name      Ptr   Length   Label-color   Field color */

1,  1,  "First Name:"     ,NULL  ,10,  YELLOW,BLACK, BLACK,WHITE,
1,  2,  "Last Name :"     ,NULL  ,15,  YELLOW,BLACK, BLACK,WHITE,
40, 2,  "Middle init:"    ,NULL  ,1,   YELLOW,BLACK, BLACK,WHITE,
1,  4,  "Age:"            ,NULL  ,3,   CYAN,BLACK,   BLACK,CYAN,
12, 4,  "Sex:"            ,NULL  ,1,   CYAN,BLACK,   BLACK,CYAN,
25, 4,  "Date of Birth:"  ,NULL  ,10,  CYAN,BLACK,   BLACK,WHITE,
1,  7,  "Street address:" ,NULL  ,30,  GREEN,BLACK,  BLACK,GREEN,
1,  8,  "City :"          ,NULL  ,20,  GREEN,BLACK,  BLACK,GREEN,
1,  9,  "State:"          ,NULL  ,2,   GREEN,BLACK,  BLACK,GREEN,
12, 9,  "Zip:"            ,NULL  ,11,  GREEN,BLACK,  BLACK,GREEN,
1,  11, "Current Title:"  ,NULL  ,40,  WHITE,BLACK,  WHITE,RED,
1,  13, "Comments:"       ,NULL  ,160, WHITE,BLACK,  WHITE,RED,
0   /* terminator */
};
int i;

/* We must allocate enough memory for each field, allowing for the
   '\0' terminator. */

   for (i= 0 ;i<NUMBER_FIELDS;i++){
    in_scrn[i].ptr= malloc((in_scrn[i].length+1)*sizeof(char));
    if(in_scrn[i].ptr==NULL) exit(1);
    in_scrn[i].ptr[0]='\0';
   }

/* call input function, pass it the input structure */

ret_code=input(in_scrn);

cls();

/* show that we really got data */
if (ret_code==ESCAPE) print(1,1,"Escape pressed, no data entered");
 else {
  print (1,1, "The following data were returned from input():");
  gotoxy(1,3);
    for(i=0;i<NUMBER_FIELDS;i++) {
      print(1,i+2,in_scrn[i].prompt);
      print_here(in_scrn[i].ptr);
    }
   print (1,i+4, "Touch any key to quit");
 }

    for(i=0;i<NUMBER_FIELDS;i++)
     /* free memory if it has been allocated*/
     if (in_scrn[i].ptr!=NULL) 
         free(in_scrn[i].ptr); 

get_key(&ch,&ext);
return(0);
}


void help()
{
int x=1, y=1;

print(x,y++,"\x19             Next Field          \x18\
             Previous field");
print(x,y++,"\x1b             Cursor left         \x1a\
             Cursor right (text)");
print(x,y++,"<Home>        beginning of field  <End>\
         End of field (text)");
print(x,y++,"<insert>      Toggle Insert       <Backspace>\
   Backspace(destructive)");
print(x,y++,"<Enter>       Next field          <Escape>   \
   Exit (Abandon record)");

}

/* This function evaluates the data input and does the following
   for each field.

   Field:         Action

  "First Name"   Trims leading spaces,capitalizes first letter.
  "Last Name"    "     "       "      "   "           "     "
  "Middle Init"  Capitalizes
  "Age"          If age is <1 or >110 a re-edit is requested
  "Sex"          If sex equals 'm' or 'f' then it is capitalized
                 if it is any other letter, then re-edit.   */


int val_field( char *string,int length,int field_number)

{
 /* Note: In this demo "length" is not used. */
 int age;

 switch (field_number){
  /* trim leading spaces off field zero */
  case 0: trim_left(string);
          /* make first letter uppercase */
          string[0]=toupper(string[0]); 
          return(REDRAW);
          /* trim leading spaces off field one */
  case 1: trim_left(string);             
          /* make first letter uppercase */
          string[0]=toupper(string[0]);  
          return (REDRAW);
  case 2: string[0]=toupper(string[0]);
          return(REDRAW);

  case 3: age=atoi(string);    /* convert string to integer */
          if (age<0 || age>110) return(REDO);
          break;
  case 4: if (string[0]=='m'){string[0]='M';return(REDRAW);}
          if (string[0]=='f'){ string[0]='F'; return(REDRAW);}
          if(string[0]!='M' && string[0]!='F')return (REDO);
          break;
 }
return (OK);
}
