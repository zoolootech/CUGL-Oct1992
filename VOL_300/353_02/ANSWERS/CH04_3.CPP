                              // Chapter 4 - Programming exercise 3
#include <iostream.h>
#include <stdio.h>

int get_volume(int length, int width = 2, int height);

main()
{
int x = 10, y = 12, z = 15;

   cout << "Some box data is " << get_volume(x, y, z) << "\n";
   cout << "Some box data is " << get_volume(x, y) << "\n";
   cout << "Some box data is " << get_volume(x) << "\n";
   cout << "Some box data is " << get_volume(x, 7) << "\n";
   cout << "Some box data is " << get_volume(5, 5, 5) << "\n";
   
}

int get_volume(int length, int width, int height)
{
   printf("%4d %4d %4d   ", length, width, height);
   return length * width * height;
}




// Result of execution
//
// (No result - errors)

