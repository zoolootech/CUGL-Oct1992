                               // Chapter 2 - Programming exercise 2
#include <iostream.h>

class animal {
public:
   int weight;
   int feet;
   float height;
};

main()
{
animal dog1, dog2, chicken;
animal cat1;
class animal cat2;

   dog1.weight = 15;
   dog2.weight = 37;
   chicken.weight = 3;

   dog1.feet = 4;
   dog2.feet = 4;
   chicken.feet = 2;

   dog1.height = 17.4;
   dog2.height = 2.221;
   chicken.height = 7.123;

   cout << "The weight of dog1 is " << dog1.weight << "\n";
   cout << "The weight of dog2 is " << dog2.weight << "\n";
   cout << "The weight of chicken is " << chicken.weight << "\n";

   cout << "Dog1 height is " << dog1.height << " inches.\n";
   cout << "The chicken's height is " << chicken.height
                                                 << " inches.\n";
}




// Result of execution
//
// The weight of dog1 is 15
// The weight of dog2 is 37
// The weight of chicken is 3
// Dog1 height is 17.4 inches.
// The chicken's height is 7.123 inches.
