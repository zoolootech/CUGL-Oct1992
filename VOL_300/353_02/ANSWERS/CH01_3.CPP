                              // Chapter 1 - Programming exercise 3
#include <iostream.h>

void main(void)
{
int month, day, year;

   cout << "Enter your birth month ---> ";
   cin >> month;
   cout << "Enter your birth day -----> ";
   cin >> day;
   cout << "Enter your birth year ----> ";
   cin >> year;

   cout << "Birthday(octal)       = " << oct <<
                       month << "/" << day << "/" << year << "\n";
   cout << "Birthday(decimal)     = " << dec <<
                       month << "/" << day << "/" << year << "\n";
   cout << "Birthday(hexadecimal) = " << hex <<
                       month << "/" << day << "/" << year << "\n";
}
