
        /*****************************************************
        *
        *       file c:\lsu\rstring.c
        *
        *       Functions: This file contains
        *            read_string
        *            clear_buffer
        *            long_clear_buffer
        *
        *       Purpose: This function reads a string of input
        *            from the keyboard.
        *
        *******************************************************/

#include "d:\lsu\vision3.h"


read_string(string)
        char *string;
{
        int     eof,
                letter,
                no_error;

        eof = -1;
        no_error = 0;

        while((letter = getchar()) != '\n' &&
               letter !=  eof)
           *string++ = letter;

        *string = '\0';

        return((letter == eof) ? eof : no_error);

}       /* ends read_string */



clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<MAX_NAME_LENGTH; i++)
      string[i] = ' ';
}



long_clear_buffer(string)
   char string[];
{
   int i;
   for(i=0; i<300; i++)
      string[i] = ' ';
}
