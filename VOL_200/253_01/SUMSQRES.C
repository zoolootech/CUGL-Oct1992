                                          /* Chapter 5 - Program 1 */
int sum; /* This is a global variable */

main()
{
int index;

   header();        /* This calls the function named header */
   for (index = 1;index <= 7;index++)
     square(index); /* This calls the square function */
   ending();        /* This calls the ending function */
}

header()        /* This is the function named header */
{
   sum = 0;     /* Initialize the variable "sum" */
   printf("This is the header for the square program\n\n");
}

square(number)   /* This is the square function */
int number;
{
int numsq;

   numsq = number * number;  /* This produces the square */
   sum += numsq;
   printf("The square of %d is %d\n",number,numsq);
}

ending()   /* This is the ending function */
{
   printf("\nThe sum of the squares is %d\n",sum);
}



/* Result of execution

This is the header for the square program

The square of 1 is 1
The square of 2 is 4
The square of 3 is 9
The square of 4 is 16
The square of 5 is 25
The square of 6 is 36
The square of 7 is 49

The sum of the squares is 140

*/