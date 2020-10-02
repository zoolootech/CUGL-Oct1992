                              // Chapter 3 - Programming exercise 1
#include <iostream.h>

struct date {
   int month;
   int day;
   int year;
};

main()
{
int index, *point1, *point2;

   point1 = &index;
   *point1 = 77;
   point2 = new int;
   *point2 = 173;
   cout << "The values are " << index << "  " << 
                                  *point1 << "  " << *point2 << "\n";
   point1 = new int;
   point2 = point1;
   *point1 = 999;
   cout << "The values are " << index << "  " << 
                                  *point1 << "  " << *point2 << "\n";
   delete point1;
   cout << "The values are " << index << "  " << 
                                  *point1 << "  " << *point2 << "\n";

float *float_point1, *float_point2 = new float;

   float_point1 = new float;
   *float_point2 = 3.14159;
   *float_point1 = 2.4 * (*float_point2);
   delete float_point2;
   delete float_point1;

date *date_point;

   date_point = new date;
   date_point->month = 10;
   date_point->day = 18;
   date_point->year = 1938;
   cout << date_point->month << "/" << date_point->day << "/" <<
                                            date_point->year << "\n";
   delete date_point;

char *c_point;

   c_point = new char[37];
   delete c_point;
   c_point = new char[sizeof(date) + 133];
   delete c_point;

   cout << "The values are " << index << "  " << 
                                  *point1 << "  " << *point2 << "\n";
}




// Result of execution
//
// The values are 77  77  173
// The values are 77  999  999
// The values are 77  -10260  -10260
// 10/18/1938
// The values are 77  -10260  -10260
