                                        /* Chapter 11 - Program 2 */
#include "stdio.h"

void main()
{
struct {
   char initial;
   int age;
   int grade;
   } kids[12];

int index;

   for (index = 0;index < 12;index++) {
      kids[index].initial = 'A' + index;
      kids[index].age = 16;
      kids[index].grade = 84;
   }

   kids[3].age = kids[5].age = 17;
   kids[2].grade = kids[6].grade = 92;
   kids[4].grade = 57;

   kids[10] = kids[4];               /* Structure assignment  */

   for (index = 0;index < 12;index++)
      printf("%c is %d years old and got a grade of %d\n",
             kids[index].initial, kids[index].age,
             kids[index].grade);
}



/* Result of execution

A is 16 years old and got a grade of 84
B is 16 years old and got a grade of 84
C is 16 years old and got a grade of 92
D is 17 years old and got a grade of 84
E is 16 years old and got a grade of 57
F is 17 years old and got a grade of 84
G is 16 years old and got a grade of 92
H is 16 years old and got a grade of 84
I is 16 years old and got a grade of 84
J is 16 years old and got a grade of 84
E is 16 years old and got a grade of 57
L is 16 years old and got a grade of 84

*/
