/*
HEADER:                 CUG205.00;
TITLE:                  Text Capitalization Filter;
DATE:                   09/24/86; 
DESCRIPTION:
                        "Capitalize text files.";
KEYWORDS:               Software tools, Text filters, toupper, case,
                        capitalization;
SYSTEM:                 MS-DOS;
FILENAME:               CAPITALS.C;
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
    putchar(toupper(c));
  }
}
