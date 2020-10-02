/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_CHIP.C   ***************************/

#include "mydef.h"


/*****************************************************************

 Usage: void chip_left(char *chip,char *block,int number);

  char *chip  = string to receive characters.
  char *block = string to lose characters.
  int  number = number of characters to move.

  Takes a "chip of the old block", removing characters from the
  left side of "block" and placing them in "chip".

  Example: block= "this is a test"
           chip= ""
           chip_left(&chip,&block,4);

           results: chip=  "this"
           block= " is a test"
*****************************************************************/

void chip_left(char *chip,char *block,int number)
{
char temp[MAX_STRING];

strcpy(chip,"\0");
 if (number <1 ) return;  /* nothing to chip */
 /* are we trying to chip too much off block? */
 if (number > strlen(block)) number=strlen(block);  
 strcpy (temp,block);
 copy (block,chip,0,number);
 copy(block,temp,number,strlen(block)-number);
 strcpy (block,temp);
 return;
}


/*****************************************************************

 Usage: void chip_right(char *chip,char *block,int number);

  char *chip  = string to receive characters.
  char *block = string to lose characters.
  int  number = number of characters to move.

  Works just like chip_left() but moves characters from the
  right side.

*****************************************************************/

void chip_right(char *chip,char *block, int number)
{
char temp[MAX_STRING];
strcpy(chip,"\n");
if (number <1 || number >strlen(block))return;
copy(block,chip,strlen(block)-number,number);
copy (block,block,0,strlen(block)-number);
return;
}
