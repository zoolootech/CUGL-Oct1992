/*
HEADER:                 CUG205.00;
TITLE:                  Line Frequency Filter;
DATE:                   09/24/86;
DESCRIPTION:
  "Counts occurrences of similar lines in sorted files.";
KEYWORDS:               Software tools, Text filters,frequency, duplicate
                        lines;
SYSTEM:                 MS-DOS;
FILENAME:               FREQ.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#define LINT_ARGS 1
#include <stdio.h>
#include <string.h>

main(argc, argv)
int argc;
char *argv[];
{
  int count = 1;
  char current[512], *result, *temp;
  char *last = { 
    "FiRsT"         };

  if (argc != 1) {
    fprintf(stderr,"freq:  extra characters following command\n");
    exit(0);
  }

  while ((result = gets(current)) != NULL) {
    if (strcmp(current,last) == 0)
      count++;
    else 
      if (strcmp(last,"FiRsT") != 0) {
        printf("%3d: %s\n", count, last);
        count = 1;
      }
    strcpy(last,current);
  }
  printf("%3d: %s\n", count, last);
}
