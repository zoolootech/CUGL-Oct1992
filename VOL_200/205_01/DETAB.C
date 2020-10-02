/*
HEADER:                 CUG205.00;
TITLE:                  DETAB for Microsoft;
DATE:                   09/24/86;
DESCRIPTION:
  "Replaces TABs with the equivalent number of spaces.";
KEYWORDS:               Software tools, Text filters;
SYSTEM:                 MS-DOS;
FILENAME:               DETAB.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <stdio.h>

main(argc,argv)
int argc;                       /* Number of command line words   */
char *argv[];                   /* Pointers to command line words */
{
  FILE *in;                     /* File used for input            */
  int c, column;

  if (argc != 2) {              /* Check if enough arguments      */
    fprintf(stderr,"TAB removal utility\n");
    fprintf(stderr,"Usage:      detab filename\n");
    exit(1);
  }

  if ((in = fopen(argv[1],"r")) == NULL) {
    fprintf(stderr,"Can't open source file:     %s\n",argv[1]); 
    exit(1);
  }

  column = 1;

  while ((c = getc(in)) != EOF) {
    switch(c) {
    case '\t' : 
      do {
        putchar(' '); 
        column++;
      } 
      while ((column % 8) != 1);
      break;
    case '\r':
    case '\n': 
      putchar(c); 
      column = 1;
      break;
    default     :
      putchar(c); 
      column++;
      break;
    }
  }
  fclose(in);
}
