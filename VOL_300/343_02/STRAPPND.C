
/*
 * NAME: append_string (string1, string2)
 *
 * FUNCTION: Appends the contents of string1 to the contents
 *           of string2.
 *
 * EXAMPLE:  string1    string2 before   string2 after
 *           " there!"      "Hello"      "Hello there!"
 *
 *           append_string (string1, string2);
 *
 * VARIABLES USED:  string1: string to append.
 *                  string2: string being appended to.
 *                  index1:  index into string1.
 *                  index2:  index into string2.
 *
 * PSEUDO CODE:  initialize index2 to 0
 *
 *               while (string2[index2] is not NULL)
 *                  increment index2  -- to find the end of string2
 *
 *               initialize index1 to 0
 *
 *               while (string2[index2] is assigned the letter in
 *                      string1[index1] and the letter is not NULL)
 *                  increment index1 and index2
 *
 */

#include "d:\cips\cips.h"

 append_string (string1, string2)
   char string1[];		  /* string to append */
   char string2[];                 /* string appended to */
  {
   int index1; 	  /* current location in string1 */
   int index2;    /* current location in string2 */

   /* find the end of string2 */

   for (index2 = 0; string2[index2] != '\0'; index2++)
     ;

   /* append string1 to string2 */

   for (index1 = 0;(string2[index2] = string1[index1])!= '\0'; ++index1)
       ++index2;   /* also increment string2 */
  }
