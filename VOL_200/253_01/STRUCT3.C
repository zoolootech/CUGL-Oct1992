                                         /* Chapter 11 - Program 3 */
#include "stdio.h"

void main()
{
struct {
   char initial;
   int age;
   int grade;
   } kids[12],*point,extra;

int index;

   for (index = 0;index < 12;index++) {
      point = kids + index;
      point->initial = 'A' + index;
      point->age = 16;
      point->grade = 84;
   }

   kids[3].age = kids[5].age = 17;
   kids[2].grade = kids[6].grade = 92;
   kids[4].grade = 57;

   for (index = 0;index < 12;index++) {
      point = kids + index;
      printf("%c is %d years old and got a grade of %d\n",
             (*point).initial, kids[index].age,
             point->grade);
   }
   extra = kids[2];               /* Structure assignment */
   extra = *point;                /* Structure assignment */
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
K is 16 years old and got a grade of 84
L is 16 years old and got a grade of 84

*/
