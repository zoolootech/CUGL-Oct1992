                                          /* Chapter 5 - Program 2 */
main()  /* This is the main program */
{
int x,y;

   for(x = 0;x <= 7;x++) {
      y = squ(x);  /* go get the value of x*x */
      printf("The square of %d is %d\n",x,y);
   }

   for (x = 0;x <= 7;++x)
      printf("The value of %d is %d\n",x,squ(x));
}

squ(in)  /* function to get the value of in squared */
int in;
{
int square;

   square = in * in;
   return(square);  /* This sets squ() = square */
}



/* Result of execution

The square of 0 is 0
The square of 1 is 1
The square of 2 is 4
The square of 3 is 9
The square of 4 is 16
The square of 5 is 25
The square of 6 is 36
The square of 7 is 49
The value of 0 is 0
The value of 1 is 1
The value of 2 is 4
The value of 3 is 9
The value of 4 is 16
The value of 5 is 25
The value of 6 is 36
The value of 7 is 49

*/
