                             // Chapter 4 - Programming exercise 4
#include <iostream.h>

// overload do_stuff;
int do_stuff1(const int);       // This squares an integer
int do_stuff2(float);           // This triples a float & returns int
float do_stuff3(const float, float); // This averages two floats

main()
{
int index = 12;
float length = 14.33;
float height = 34.33;

   cout << "12 squared is "    << do_stuff1(index)         << "\n";
   cout << "24 squared is "    << do_stuff1(2 * index)     << "\n";
   cout << "Three lengths is " << do_stuff2(length)        << "\n";
   cout << "Three heights is " << do_stuff2(height)        << "\n";
   cout << "The average is "   << do_stuff3(length,height) << "\n";
}

int do_stuff1(const int in_value)      // This squares an integer
{
   return in_value * in_value;
}

int do_stuff2(float in_value)    // Triples a float & return int
{
   return (int)(3.0 * in_value);
}

                                      // This averages two floats
float do_stuff3(const float in1, float in2)
{
   return (in1 + in2)/2.0;
}




// Result of execution
//
// 12 squared is 144
// 24 squared is 576
// Three lengths is 42
// Three heights is 102
// The average is 24.330002
