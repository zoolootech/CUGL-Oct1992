
        /*****************************************************
        *
        *       file d:\cips\rstring.c
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

#include "d:\cips\cips.h"


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


           /******************************************************
           *
           *       file d:\cips\mof.c
           *
           *       Functions: This file contains
           *          my_open
           *
           *       Purpose: This function opens a file. Borland's
           *          Turbo C opens files a little different from
           *          the standard UNIX C. Instead of using this
           *          different method in a number of various files,
           *          the method is placed in this one file. If the
           *          programs are moved to another system, all changes
           *          will be located in this one place.
           *
           *       External Calls:
           *          none
           *
           *       Modifications:
           *          18 June 1987 - created
           *
           ****************************************************/



my_open(file_name)
   char file_name[];
{
   int file_descriptor;

   file_descriptor = open(file_name, O_RDWR | O_CREAT | O_BINARY, 
                          S_IWRITE);

   return(file_descriptor);


}  /* ends my_open  */

       /*****************************************************
       *
       *       file c:\lsu\mrw.c
       *
       *       Functions: This file contains
       *           my_read
       *           my_write
       *
       *       Purpose: These two functions call the Turbo C
       *           functions _read and _write. All software
       *           will use my_read and my_write so that if
       *           the software is ported to another system
       *           that uses read and write changing the
       *           two functions in this file will take care
       *           of the move.
       *
       *       External Call:
       *           read
       *           _write
       *
       *       Modifications:
       *           10 June 1987 - created
       *
       *
       ************************************************************/


my_read(file_descriptor, buffer, number_of_bytes)
   int  file_descriptor, number_of_bytes;
   char *buffer;
{
   int bytes_read;
   int read();
   bytes_read = read(file_descriptor, buffer, number_of_bytes);
   return(bytes_read);
}


my_write(file_descriptor, buffer, number_of_bytes)
   int  file_descriptor, number_of_bytes;
   char *buffer;
{
   int bytes_written;
   int write();
   bytes_written = write(file_descriptor, buffer, number_of_bytes);
   return(bytes_written);
}

        /******************************************************
        *
        *        file d:\cips\gpcips.c
        *
        *       Functions: This file contains
        *           get_parameters
        *           show_parameters
        *
        *       Purpose - These functions get image parameters.
        *
        *       External Calls:
        *           rstring.c - read_string
        *           intcvrt.c - get_integer
        *
        *       Modifications:
        *           19 February 1987 - These functions were taken out
        *                of the file ip.c.
        *           28 June 1990 - changed to gpcips and the channel
        *                parameter was removed.
        *
        *******************************************************/






get_parameters(il, ie, ll, le)
        int *il, *ie, *le, *ll;
{
        int choice, not_finished;

        not_finished = 1;
        while(not_finished){
           show_parameters(il, ie, ll, le);
           printf("\n\nEnter choice to change (enter 0 for no changes) __\b\b");
           get_integer(&choice);

           switch (choice){
              case 0:
                 not_finished = 0;
                 break;

              case 1:
                 break;

              case 2:
                 printf("\nEnter initial line\n___\b\b\b");
                 get_integer(il);
                 break;

              case 3:
                 printf("\nEnter initial element\n___\b\b\b");
                 get_integer(ie);
                 break;

              case 4:
                 printf("\nEnter last line\n___\b\b\b");
                 get_integer(ll);
                 break;

              case 5:
                 printf("\nEnter last element\n___\b\b\b");
                 get_integer(le);
                 break;

           }  /* ends switch choice       */
        }     /* ends while not_finished  */
}             /* ends get_parameters      */


show_parameters(il, ie, ll, le)
        int *il, *ie, *le, *ll;
{
   printf("\n\nThe image parameters are:");
   printf("\n\t2.      il = %4d", *il);
   printf("\n\t3.      ie = %4d", *ie);
   printf("\n\t4.      ll = %4d", *ll);
   printf("\n\t5.      le = %4d", *le);
}  /* ends show_parameters  */

       /******************************************************
       *
       *       file d:\cips\intcvrt.c
       *
       *       Functions: This file contains
       *           get_integer
       *           int_convert
       *
       *       Purpose: These functions convert a string of
       *                characters to their number value.
       *
       *       Modifications:
       *            Taken from Jamsa's software package
       *            and altered to fit into the computer
       *            vision programming 22 August 1986.
       *
       *******************************************************/


#include "d:\cips\numdefs.h"


get_integer(n)
   int *n;
{
   char string[80];

   read_string(string);
   int_convert(string, n);
}





 int_convert (ascii_val, result)
    char *ascii_val;
    int *result;
  {
    int sign = 1;  /* -1 if negative */

    *result = 0;   /* value returned to the calling routine */

    /* read passed blanks */

    while (is_blank(*ascii_val))
       ascii_val++;              /* get next letter */

    /* check for sign */

    if (*ascii_val == '-' || *ascii_val == '+')
       sign = (*ascii_val++ == '-') ? -1 : 1;  /* find sign */

   /*
    * convert the ASCII representation to the actual
    * decimal value by subtracting '0' from each character.
    *
    * for example, the ASCII '9' is equivalent to 57 in decimal.
    * by subtracting '0' (or 48 in decimal) we get the desired
    * value.
    *
    * if we have already converted '9' to 9 and the next character
    * is '3', we must first multiply 9 by 10 and then convert '3'
    * to decimal and add it to the previous total yielding 93.
    *
    */

    while (*ascii_val)
     if (is_digit(*ascii_val))
       *result = *result * 10 + to_decimal(*ascii_val++);

     else
       return (IO_ERROR);


    *result = *result * sign;

    return (NO_ERROR);
  }





         /************************************************
         *
         *         Functions: This file contains
         *           get_short
         *             short_convert
         *
         *         Purpose: These functions convert a string of
         *                characters to their number value.
         *
         *         Modifications:
         *            Taken from Jamsa's software package
         *            and altered to fit into the computer
         *            vision programming 22 August 1986.
         *
         *************************************************/


get_short(n)
   short *n;
{
   char string[80];

   read_string(string);
   short_convert(string, n);
}





 short_convert (ascii_val, result)
    char *ascii_val;
    short *result;
  {
    int sign = 1;  /* -1 if negative */

    *result = 0;   /* value returned to the calling routine */

    /* read passed blanks */

    while (is_blank(*ascii_val))
       ascii_val++;              /* get next letter */

    /* check for sign */

    if (*ascii_val == '-' || *ascii_val == '+')
       sign = (*ascii_val++ == '-') ? -1 : 1;  /* find sign */

   /*
    * convert the ASCII representation to the actual
    * decimal value by subtracting '0' from each character.
    *
    * for example, the ASCII '9' is equivalent to 57 in decimal.
    * by subtracting '0' (or 48 in decimal) we get the desired
    * value.
    *
    * if we have already converted '9' to 9 and the next character
    * is '3', we must first multiply 9 by 10 and then convert '3'
    * to decimal and add it to the previous total yielding 93.
    *
    */

    while (*ascii_val){
     if (is_digit(*ascii_val)){
       *result = *result * 10 + to_decimal(*ascii_val++);
       if( (sign == -1) && (*result > 0)) *result = *result * -1;
     }
     else
       return (IO_ERROR);
    }  /* ends while ascii_val  */

    return (NO_ERROR);
  }





         /***********************************************
         *
         *         file d:\cips\locvrt.c
         *
         *         Functions: This file contains
         *           get_long
         *             long_convert
         *
         *         Purpose: These functions convert a string of
         *                characters to their number value.
         *
         *         Modifications:
         *            Taken from Jamsa's software package
         *            and altered to fit into the computer
         *            vision programming 22 August 1986.
         *
         ************************************************/


get_long(n)
   long *n;
{
   char string[80];

   read_string(string);
   long_convert(string, n);
}





 long_convert (ascii_val, result)
    char *ascii_val;
    long *result;
  {
    int sign = 1;  /* -1 if negative */

    *result = 0;   /* value returned to the calling routine */

    /* read passed blanks */

    while (is_blank(*ascii_val))
       ascii_val++;              /* get next letter */

    /* check for sign */

    if (*ascii_val == '-' || *ascii_val == '+')
       sign = (*ascii_val++ == '-') ? -1 : 1;  /* find sign */

   /*
    * convert the ASCII representation to the actual
    * decimal value by subtracting '0' from each character.
    *
    * for example, the ASCII '9' is equivalent to 57 in decimal.
    * by subtracting '0' (or 48 in decimal) we get the desired
    * value.
    *
    * if we have already converted '9' to 9 and the next character
    * is '3', we must first multiply 9 by 10 and then convert '3'
    * to decimal and add it to the previous total yielding 93.
    *
    */

    while (*ascii_val)
     if (is_digit(*ascii_val))
       *result = *result * 10 + to_decimal(*ascii_val++);
     else
       return (IO_ERROR);


    *result = *result * sign;

    return (NO_ERROR);
  }






     /*******************************************************
     *
     *   file d:\cips\flocvrt.c
     *
     *   Functions: This file contains
     *       get_float
     *           float_convert
     *       power
     *
     *   Purpose: This function converts a string of
     *           characters to its number value.
     *
     *   Modifications:
     *            This was taken from Jamsa's software
     *            packages and modified to work in the
     *            computer vision programs 22 August 1986.
     *
     *            16 June 1987 - the power function was not working
     *                so Borland's Turbo C function pow10
     *                was substituted for it.
     *
     ****************************************************/


float get_float(f)
   float *f;
{
   char string[80];

   read_string(string);
   float_convert(string, f);
}

 float_convert (ascii_val, result)
    char *ascii_val;
    float *result;
  {
    int count;            /* # of digits to the right of the
                             decimal point. */
    int sign = 1;         /* -1 if negative */

    double pow10();       /* Turbo C function */
    float power();        /* function returning a value raised
                             to the power specified. */

    *result = 0.0;        /* value desired by the calling routine */

    /* read passed blanks */

    while (is_blank(*ascii_val))
       ascii_val++;              /* get the next letter */

    /* check for a sign */

    if (*ascii_val == '-' || *ascii_val == '+')
       sign = (*ascii_val++ == '-') ? -1 : 1;  /* find sign */


   /*
    * first convert the numbers on the left of the decimal point.
    *
    * if the number is 33.141592  this loop will convert 33
    *
    * convert ASCII representation to the  actual decimal
    * value by subtracting '0' from each character.
    *
    * for example, the ASCII '9' is equivalent to 57 in decimal.
    * by subtracting '0' (or 48 in decimal) we get the desired
    * value.
    *
    * if we have already converted '9' to 9 and the next character
    * is '3', we must first multiply 9 by 10 and then convert '3'
    * to decimal and add it to the previous total yielding 93.
    *
    */

    while (*ascii_val)
     if (is_digit(*ascii_val))
       *result = *result * 10 + to_decimal(*ascii_val++);

     else if (*ascii_val == '.')  /* start the fractional part */
       break;

     else
       return (IO_ERROR);


   /*
    * find number to the right of the decimal point.
    *
    * if the number is 33.141592 this portion will return 141592.
    *
    * by converting a character and then dividing it by 10
    * raised to the number of digits to the right of the
    * decimal place the digits are placed in the correct locations.
    *
    *     4 / power (10, 2) ==> 0.04
    *
    */

    if (*ascii_val != NULL2)
     {
        ascii_val++;   /* past decimal point */

        for (count = 1; *ascii_val != NULL2; count++, ascii_val++)

              /*********************************************
              *
              *   The following change was made 16 June 1987.
              *   For some reason the power function below
              *   was not working. Borland's Turbo C pow10
              *   was substituted.
              *
              ***********************************************/
         if (is_digit(*ascii_val)){
           *result = *result + to_decimal(*ascii_val)/power(10.0,count);
        /***********
        *result = *result + to_decimal(*ascii_val)/((float)(pow10(count)));
        ************/
          }

         else
          return (IO_ERROR);
     }
    *result = *result * sign; /* positive or negative value */

    return (NO_ERROR);
  }


float power(value, n)
   float value;
   int n;
{
   int   count;
   float result;

   if(n < 0)
      return(-1.0);

   result = 1;
   for(count=1; count<=n; count++){
      result = result * value;
   }

   return(result);
}

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
