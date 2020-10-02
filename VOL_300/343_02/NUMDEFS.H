

       /**************************************************
       *
       *       file d:\cips\numdefs.h
       *
       *       This file contains some macro definitions
       *       which are used by the get_integer, float,
       *       short, and long functions.
       *
       ***************************************************/

#define is_digit(x) ((x >= '0' && x <= '9') ? 1 : 0)

#define is_blank(x) ((x == ' ') ? 1 : 0)

#define to_decimal(x) (x - '0')

#define NO_ERROR  0
#define IO_ERROR -1
#define NULL2   '\0'
