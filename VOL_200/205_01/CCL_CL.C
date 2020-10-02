/*
HEADER:                 CUG205.00;
TITLE:                  Multiple Carriage Return Filter;
DATE:                   09/24/86;
DESCRIPTION:
  "Filter to eliminate multiple carriage returns.  Changes [CR][CR][LF] 
   sequences to [CR][LF] sequences.";
KEYWORDS:               Software tools, Text filters, carriage return,
                        single space, new line filter;
SYSTEM:                 MS-DOS;
FILENAME:               CCL-CL.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <stdio.h>

main()
{
  int ch;

  while ((ch = getchar()) != EOF) {
    switch(ch) {
      case ('\15') :
      break;
      case ('\12') :
      putchar('\n');
      break;
    default :
      putchar(ch);
      break;
    }
  }
}
