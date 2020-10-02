                               // Chapter 7 - Programming exercise 4

#ifndef CH07_4_H
#define CH07_4_H

class new_name : public name {

   char sex;

public;

         // Constructor to set all data to NULL.  This will also
         //  call the name constructor with no parameters.
   new_name(void) { sex = ' '; };

         // Constructor to set sex to blank and pass the name
         //  to the parent class via a member initializer.
   new_name(char *fn, char *mn, char *ln) :
                                 name(fn, mn, ln) { sex = ' '; };

         // This sets the sex to the input value, if it is either
         //  'M' or 'F', otherwise it leaves it blank and returns
         //  an error value of zero.
   int set_sex(char input_sex);

         // This retrieves the value stored
   char get_sex(void) { return sex; };
};

#endif
