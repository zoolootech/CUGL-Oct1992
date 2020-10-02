                              // Chapter 4 - Programming exercise 2
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

void do_stuff(float wings, float feet, char eyes)
{
   cout << "There are " << wings << " wings." << "\n";
   cout << "There are " << feet << " feet." << "\n";
   cout << "There are " << eyes << " eyes." << "\n\n";
}




// Result of execution
//
// There are 3 wings.
// There are 12 feet.
// There are 4 eyes.
//
// There are 2 wings.
// There are 1000 feet.
// There are 2 eyes.

// There is no change in the result because int and float are
//  compatible types and the system will change from one to 
//  the other automatically.  Note the funny output of the
//  char type however.
