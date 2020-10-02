                              // Chapter 4 - Programming exercise 1
#include <iostream.h>

void do_stuff(float wings, float feet, char eyes);

main()
{
int arm = 2;
float foot = 1000.0;
char lookers = 2;

   do_stuff(3, 12.0, 4);
   do_stuff(arm, foot, lookers);
}

void do_stuff(int wings, float feet, char eyes)
{
   cout << "There are " << wings << " wings." << "\n";
   cout << "There are " << feet << " feet." << "\n";
   cout << "There are " << eyes << " eyes." << "\n\n";
}




// Result of execution
//
// (No result - errors)
