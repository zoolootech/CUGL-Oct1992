/*
HEADER:                 CUG205.00;
TITLE:                  Whitespace Counter;
DATE:                   09/24/86;
DESCRIPTION:
  "Counts the blanks, tabs, and carriage returns in a file.";
KEYWORDS:               Software tools, Text filters, white space counter;
SYSTEM:                 MS-DOS;
FILENAME:               WHITE.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <stdio.h>

main(argc,argv)
int argc;               /* Number of command line words     */
char *argv[];           /* Pointers to command line words   */
{
  FILE *in;             /* File used for input              */
  int c, nb, nt, nl;    /* Number of blanks, tabs, newlines */

  nb = nt = nl = 0;

  if (argc != 2) {
    fprintf(stderr,"Whitespace counting utility\n");
    fprintf(stderr,"Usage:  white source\n");
    exit(1);
  }

  if ((in = fopen(argv[1],"r")) == NULL ) {
    fprintf(stderr,"Can't open source file: %s\n",argv[1]);     
    exit(2);
  }

  while ((c = getc(in)) != EOF) {
    if (c == ' ')
      ++nb;
    else if (c == '\t')
      ++nt;
    else if (c == '\n')
      ++nl;
  }
  fclose(in);
  printf("Spaces    Tabs    Newlines\n");
  printf("%6d  %6d  %6d\n", nb, nt, nl);
}
