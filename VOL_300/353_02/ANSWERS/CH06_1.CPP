                              // Chapter 6 - Programming exercise 1
#include <iostream.h>
#include <string.h>

class box {
   int length;
   int width;
   char line_of_text[25];
public:
   box(char *input_line);             //Constructor
   void set(int new_length, int new_width);
   int get_area(void);
   void change(char *new_text);
};


box::box(char *input_line)        //Constructor implementation
{
   length = 8;
   width = 8;
   strcpy(line_of_text, input_line);
}


// This method will set a box size to the two input parameters
void box::set(int new_length, int new_width)
{
   length = new_length;
   width = new_width;
}


// This method will calculate and return the area of a box instance
int box::get_area(void)
{
   cout << line_of_text << "= ";
   return (length * width);
}


void box::change(char *new_text)
{
   strcpy(line_of_text, new_text);
}


main()
{
box *small, *medium, *large;

   small  = new box("small box ");
   medium = new box("medium box ");
   large  = new box("large box ");

   small->set(5, 7);
   large->set(15, 20);

   cout << "The area of the " << small->get_area() << "\n";
   cout << "The area of the " << medium->get_area() << "\n";
   cout << "The area of the " << large->get_area() << "\n";

   small->change("first box ");
   medium->change("second box ");
   large->change("third box ");

   cout << "The area of the " << small->get_area() << "\n";
   cout << "The area of the " << medium->get_area() << "\n";
   cout << "The area of the " << large->get_area() << "\n";

}




// Result of execution
//
// The area of the small box = 35
// The area is the medium box = 64
// The area is the large box = 300
// The area of the first box = 35
// The area is the second box = 64
// The area is the third box = 300
