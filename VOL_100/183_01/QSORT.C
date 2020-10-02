#include <search.h>
#include <stdio.h>

int compare ();  /* declare compare function */

/* define and initial array */
int array[10] = { 2, 3, 2, 6, 8, 1, 9, 8 ,5, 4 };
 

main()  /* qsort the 10  pre given ramdom integers */
{
	int num   = 10 ;          /* 10 integers */
	int     i =  0 ;          /* array element starter*/
     

	/* print the array before we sort it. */
     printf ("before QSORT : \n");
 	while ( num-- > 0 )
		printf ("%3d \n", array[i++]);


	num = 10;  /* reset the total element count */

	/* call the library routine "QSORT" */
     qsort(array, num, sizeof(int), compare);



	/* print the array after we sort it. */
	i = 0;  /* reset the array element count */
     printf ("after QSORT : \n");
	while ( num-- > 0 )
		printf ("%3d \n", array[i++]);
 
}



int compare (num1, num2)   /* little function for QSORT */

int *num1, *num2;  /* initial the POINTERs of the parameter  */

{
	/*  return value :
		negative   when   num1 < num2
             0          when   num1 = num2
             positive   when   num1 > num2   */


	return( *num1 - *num2 );
}
