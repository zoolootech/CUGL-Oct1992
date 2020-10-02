                              // Chapter 5 - Programming exercise 2
#include <iostream.h>

class one_datum {
   int data_store;
public:
   one_datum(void);
   void set(int in_value);
   int get_value(void);
   int square(void);
};

one_datum::one_datum(void)
{
   data_store = 10;
}

void one_datum::set(int in_value)
{
   data_store = in_value;
}

int one_datum::get_value(void)
{
   return data_store;
}

int one_datum::square(void)
{
   return data_store * data_store;
}

main()
{
one_datum dog1, dog2, dog3;
int piggy;

   cout << "dog1 = " << dog1.get_value() << "\n";
   cout << "dog2 = " << dog2.get_value() << "\n";
   cout << "dog3 = " << dog3.get_value() << "\n";

   dog1.set(12);
   dog2.set(17);
   dog3.set(-13);
   piggy = 123;

// dog1.data_store = 115;      This is illegal in C++
// dog2.data_store = 211;      This is illegal in C++

   cout << "The value of dog1 is " << dog1.get_value() << "\n";
   cout << "The value of dog2 is " << dog2.get_value() << "\n";
   cout << "The value of dog3 is " << dog3.get_value() << "\n";
   cout << "The value of piggy is " << piggy << "\n";

   cout << "The value of dog1 squared is " << dog1.square() << "\n";
   cout << "The value of dog2 squared is " << dog2.square() << "\n";
}




// Result of execution
//
// dog1 = 10
// dog2 = 10
// dog3 = 10
// The value of dog1 is 12
// The value of dog2 is 17
// The value of dog3 is -13
// The value of piggy is 123
// The value of dog1 squared is 144
// The value of dog2 squared is 289
