			      // Chapter 3 - Programming exercise 2
#include <iostream.h>
#include <stdio.h>

void print_stuff(float data_to_ignore);
void print_message(float list_this_data);
void print_float(float data_to_print);
void (*function_pointer)(float);
void print_int(int data);

main()
{
float pi = 3.14159;
float two_pi = 2.0 * pi;

   print_stuff(pi);
   function_pointer = print_stuff;
   function_pointer(pi);
   function_pointer = print_message;
   function_pointer(two_pi);
   function_pointer(13.0);
   function_pointer = print_float;
   function_pointer(pi);
   print_float(pi);

   function_pointer = print_int;
   function_pointer(37);
}


void print_stuff(float data_to_ignore)
{
   printf("This is the print stuff function.\n");
}


void print_message(float list_this_data)
{
   printf("The data to be listed is %f\n", list_this_data);
}


void print_float(float data_to_print)
{
   printf("The data to be printed is %f\n", data_to_print);
}


void print_int(int data)
{
   printf("This is an integer %d\n", data);
}


// Result of execution
//
// This is the print stuff function.
// This is the print stuff function.
// The data to be listed is 6.283180
// The data to be listed is 13.000000
// The data to be printed is 3.141590
// The data to be printed is 3.141590
// This is an integer 0
