/*
HEADER:                 CUG205.00;
TITLE:                  WORDS for Microsoft;
DATE:                   09/24/86;
DESCRIPTION:
  "Places words on individual lines.";
KEYWORDS:               Software tools, Text filters, words, make words;
SYSTEM:                 MS-DOS;
FILENAME:               WORDS.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <stdio.h>
#include <ctype.h>

main()
{
  int ch;
  int col;
  col = 1;

  while ((ch = getchar()) != EOF) 
    if (isalpha(ch)) {
      putchar(ch); 
      col ++;
    }
    else
      if (col != 1) {
        putchar('\n'); 
        col = 1;
      }
}
