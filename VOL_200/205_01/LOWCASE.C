/*
HEADER:                 CUG205.00;
TITLE:                  Text File Lowercase Filter;
DATE:                   09/24/86;
DESCRIPTION:
  "Filter for converting an entire text file in lower case.";
KEYWORDS:               Software tools, Text filters,case, lower case, to
                        lower;
SYSTEM:                 MS-DOS;
FILENAME:               LOWCASE.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <stdio.h>
#include <ctype.h>

main()
{
  int c;
  while((c = getchar()) != EOF) {
    putchar(tolower(c));
  }
}
