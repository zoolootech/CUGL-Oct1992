

        /********************************************************
        *
        *       file c:\lsu\fwrite.c
        *
        *       Functions: This file contains
        *           my_fwrite
        *           my_fwriteln
        *           tab_string
        *
        *       Purpose: These functions write a string to a
        *                file.
        *
        *       Modifications:
        *           3 September 1986 - created from the routines
        *               in Jamsa's book on pp.  95-96.
        *           12 June 1987 - ported to Borland Turbo C and
        *               changed the names of functions from
        *               fwrite and fwriteln to my_fwrite and
        *               my_fwriteln. This is because Borland
        *               defines their own functions with the
        *               same names.
        *
        *********************************************************/

#include "d:\cips\cips.h"

/*
 * NAME: my_fwrite (file_pointer, string)
 *
 * FUNCTION: Writes the string to the file specified by
 *           file_pointer insuring that a carriage
 *           return is not written.
 *
 * EXAMPLES fwrite (fp, last_name);
 *
 * VARIABLES USED: string: pointer to the string to be written.
 *                 file_pointer: pointer to the output file.
 *
 * PSEUDO CODE: while (the letter referenced by *string is not an
 *                     End of Line (EOL) or equal to NULL)
 *
 *                 write the letter to the file
 *                 increment string to point to the next letter
 *
 *              return without printing a carriage return
 *
 */

 my_fwrite (file_pointer, string)
  FILE *file_pointer;  /* requires stdio.h to be #included */
  char *string;
 {

  while (*string != '\n' && *string != '\0')
    putc (*string++, file_pointer);

 }


/*
 * NAME: my_fwriteln (string)
 *
 * FUNCTION: Writes the string to the file referenced
 *           by file_pointer insuring that only one
 *           carriage is written.   If a line is read
 *           from a file, we have no way of knowing whether
 *           or not it contains a newline character.  If we
 *           assume it does, and none are present the output
 *           will consist of one long line.  If we assume it
 *           doesn't and it does, each line is double spaced.
 *
 * EXAMPLES fwriteln (file_pointer, address);
 *
 * VARIABLES USED: string: pointer to the string to be written.
 *                 file_pointer: pointer to the output file.
 *
 * PSEUDO CODE: while (letter referenced by *string is not an
 *                     End of Line (EOL) or  equal to NULL)
 *                 write the letter to the file
 *                 increment string to point to the next letter
 *
 *              write a newline character to the file
 *
 */

 my_fwriteln (file_pointer, string)
  char *string;
  FILE *file_pointer;  /* requires stdio.h be #included */
 {

  while (*string != '\n' && *string != '\0'){
    putc (*string++, file_pointer);
  }

  putc ('\n', file_pointer);

 }




 tab_string(string)
    char string[];
{
   char temp_string[80];
   int  i;

   clear_buffer(temp_string);
   for(i=0; i<8; i++)
      temp_string[i] = ' ';
   for(i=8; i<MAX_NAME_LENGTH; i++)
      temp_string[i] = string[i-8];
   for(i=0; i<MAX_NAME_LENGTH; i++)
      string[i] = temp_string[i];
}  /* ends tab_string  */
