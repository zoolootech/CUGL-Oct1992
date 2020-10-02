/* Copyright (c) James L. Pinson 1990,1991  */

/**********************   L_GETKEY.C  ***************************/

#include "mydef.h"


/*****************************************************************

 Usage: void get_key(char *ch, char *ext);


  char *ch=  character to read from keyboard.

  char *ext= extended character (if any).

  Reads character and extended character from keyboard.

*****************************************************************/

void get_key(char *ch, char *ext)
{
  *ch=getch();      /* get the character */

    if(!*ch){       /* if the character is zero (a special key) */
      *ext=getch(); /* get the extension */
    }
}
