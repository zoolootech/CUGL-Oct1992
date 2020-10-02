                                         /* Chapter 5 - Program 3 */
float z;   /* This is a global variable */

main()
{
int index;
float x,y,sqr(),glsqr();

   for (index = 0;index <= 7;index++){
      x = index;  /* convert int to float */
      y = sqr(x); /* square x to a floating point variable */
      printf("The square of %d is %10.4f\n",index,y);
   }

   for (index = 0; index <= 7;index++) {
      z = index;
      y = glsqr();
      printf("The square of %d is %10.4f\n",index,y);
   }
}

float sqr(inval)  /* square a float, return a float */
float inval;
{
float square;

   square = inval * inval;
   return(square);
}

float glsqr()    /* square a float, return a float */
{
   return(z*z);
}



/* Result of execution

The square of 0 is      0.0000
The square of 1 is      1.0000
The square of 2 is      4.0000
The square of 3 is      9.0000
The square of 4 is     16.0000
The square of 5 is     25.0000
The square of 6 is     36.0000
The square of 7 is     49.0000
The square of 0 is      0.0000
The square of 1 is      1.0000
The square of 2 is      4.0000
The square of 3 is      9.0000
The square of 4 is     16.0000
The square of 5 is     25.0000
The square of 6 is     36.0000
The square of 7 is     49.0000

*/
