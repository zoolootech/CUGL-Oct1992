                               // Chapter 5 - Programming exercise 5

// This is the header file for a name class that can store a name in
//  three parts and provide a string in any of four different formats
//  which can be used in any database requiring named of persons.
// It is not immediately obvious, but it can also be used to store
//  the name of a city, county, state combination also and provide a
//  string in the correct format to be used in a location program.

#ifndef CH05_5_H
#define CH05_5_H

class name {
protected:

   char first_name[12];
   char middle_name[12];
   char last_name[20];
   int  format;
   static char full_name[35];  // A place to store the full name

public:

         // Constructors, also set the format to 3
   name(void);                          // Set all three to NULL
   name(char *fn, char *mn, char *ln);  // Set to input fields

         // Copy a string into the storage area
   void set_first(char *first_in);
   void set_middle(char *middle_in);
   void set_last(char *last_in);

         // Return a pointer to a partial string
   char *get_first(void)  { return first_name; };
   char *get_middle(void) { return middle_name; };
   char *get_last(void)   { return last_name; };

         // Return a pointer to a string in the selected format
         //  format = 1 --> John Paul Doe
         //  format = 2 --> J. P. Doe
         //  format = 3 --> Doe, John Paul (default)
         //  format = 4 --> Doe, J. P.
   char *get_full_name(void);

};

#endif
