                                        /* Chapter 6 - Program 2 */
#include "stdio.h"

#define WRONG(A) A*A*A          /* Wrong macro for cube     */
#define CUBE(A) (A)*(A)*(A)     /* Right macro for cube     */
#define SQUR(A) (A)*(A)         /* Right macro for square   */
#define ADD_WRONG(A) (A)+(A)    /* Wrong macro for addition */
#define ADD_RIGHT(A) ((A)+(A))  /* Right macro for addition */
#define START 1
#define STOP  7

void main()
{
int i,offset;

   offset = 5;
   for (i = START;i <= STOP;i++) {
      printf("The square of %3d is %4d, and its cube is %6d\n",
              i+offset,SQUR(i+offset),CUBE(i+offset));
      printf("The wrong of  %3d is %6d\n",i+offset,WRONG(i+offset));
   }

   printf("\nNow try the addition macro's\n");
   for (i = START;i <= STOP;i++) {
      printf("Wrong addition macro = %6d, and right = %6d\n"
                               ,5*ADD_WRONG(i),5*ADD_RIGHT(i));
   }
}



/* Result of execution

The square of   6 is   36, and its cube is    216
The wrong of    6 is     16
The square of   7 is   49, and its cube is    343
The wrong of    7 is     27
The square of   8 is   64, and its cube is    512
The wrong of    8 is     38
The square of   9 is   81, and its cube is    729
The wrong of    9 is     49
The square of  10 is  100, and its cube is   1000
The wrong of   10 is     60
The square of  11 is  121, and its cube is   1331
The wrong of   11 is     71
The square of  12 is  144, and its cube is   1728
The wrong of   12 is     82

Now try the addition macro's
Wrong addition macro =      6, and right =     10
Wrong addition macro =     12, and right =     20
Wrong addition macro =     18, and right =     30
Wrong addition macro =     24, and right =     40
Wrong addition macro =     30, and right =     50
Wrong addition macro =     36, and right =     60
Wrong addition macro =     42, and right =     70

*/
