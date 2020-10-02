/*******************************************************
*                                                      *
*   Full test case for addition of multi-dimensional   *
*   arrays to Small C V2.0 using the modified          *
*   CUG disk #163, and PC Club of Toronto #152.        *
*   Release 1.01                                       *
*                                 -  Don Lang  1/91    *
*******************************************************/

#define LASTROW   1
#define CLOSELAST 2
/*
  Declaration required for c.lib
				*/
extern int printf();
/*
    Test for global declaration of a 2-D array and for
    proper initialization.  This will exercise the modified
    Small C functions, "declglb", "initials", and "addsym."
    Size of 1st dimension is determined by the number of
    initialized elements as per the C language definition.
							  */
char garray[][2] = { 1, 2, 3, 4 };

int garray2 [3] [3];

/* Test to see that 1-D is unaffected. */

char gar1D[] = { 1,2 };

/*
   Try an external 2-D array of unknown size.
					      */
	/*   (in file: ext.c)    */

extern char ext2_D [] [CLOSELAST + 2];

main()
{
	/*
	   Declare local 2-D array and thus test the
	   function "declloc" for 2-D.
							*/
	int i, k, count, larray[3][4];
	count = 0;
	for (i = 0; i < 3; i++)
	       for (k=0; k < 4; k++) {
		      ext2_D[i][k] = larray[i][k] = count;
		      count++;
	       }
/*
       The following strange code has been written to test
   the compiler's ability to correctly generate code for
   multi-dimensional array expressions using both variable
   and constant expressions as array indices.  This, and the
   above nested "for" loop, will test the modifications to
   the Small-C expression analyzer; specifically the
   function "heir14."
							  */
	for (i=0; i<3; i++) {
	       printf("\n");
	       printf(" %d ", larray[i][0]);
	       printf(" %d ", larray[i][LASTROW]);
	       printf(" %d ", larray[i][2]);
	       printf(" %d ", larray[i][3]);
	}
	printf("\n"); 
	printf("\n");
	for (i=0; i<4; i++) printf(" %d ",
			  larray[CLOSELAST-2][i]);
	printf("\n");
	for (i=0; i<4; i++) printf(" %d ",
			  larray[CLOSELAST/2][i]);
	printf("\n");
/*
    Try printing initialized global character array and the
    processed external.
					      */
	ga_print(2,2, garray);
	for (i=0; i<3; i+=1) {
		printf("\n");
		for (k=0; k<4; k++) printf(" %d ",
					   ext2_D[i][k]);
	}
/*
   Print out 1-D array to see that it hasn't been changed by
   the modifications for "n" dimensions.
							  */
	printf("\n");
	ga2_print(2, gar1D);
}
/*
    This global print function will test passing of array
    arguments, and thus the modified Small C function
    "doargs."
							  */
ga_print (a,b, array) int a,b; char array[][2];
{
	int i,k;
	printf("\n");
	i=0;
	while (i < a) {
		for(k=0; k<b; k++) printf(" %d ",
					   array[i][k]);
		printf("\n");
		i++;
	}
}
ga2_print (a, array) int a; char array[];
{
	int i;
	printf("\n");
	for (i = 0; i < a; i++) printf(" %d ", array[i]);
}
