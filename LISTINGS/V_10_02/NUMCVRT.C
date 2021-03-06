
       /******************************************************
       *
       *       file c:\lsu\intcvrt.c
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


#include "d:\lsu\numdefs.h"


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





		/******************************************************
		*
		*		Functions: This file contains
		*           get_short
		*		    short_convert
		*
		*		Purpose: These functions convert a string of
		*                characters to their number value.
		*
		*		Modifications:
		*            Taken from Jamsa's software package
		*            and altered to fit into the computer
		*            vision programming 22 August 1986.
		*
		*******************************************************/


get_short(n)
   short *n;
{
   char string[80];

   read_string(string);
   int_convert(string, n);
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





		/******************************************************
		*
		*		file c:\lsu\locvrt.c
		*
		*		Functions: This file contains
		*           get_long
		*		    long_convert
		*
		*		Purpose: These functions convert a string of
		*                characters to their number value.
		*
		*		Modifications:
		*            Taken from Jamsa's software package
		*            and altered to fit into the computer
		*            vision programming 22 August 1986.
		*
		*******************************************************/


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
	*	file c:\lsu\flocvrt.c
	*
	*	Functions: This file contains
	*	    get_float
	*           float_convert
	*	    power
	*
	*	Purpose: This function converts a string of
	*           characters to its number value.
	*
	*	Modifications:
	*            This was taken from Jamsa's software
	*            packages and modified to work in the
	*            computer vision programs 22 August 1986.
        *
        *            16 June 1987 - the power function was not working
        *                so Borland's Turbo C function pow10
        *                was substituted for it.
	*
	*********************************************************/


get_float(f)
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
