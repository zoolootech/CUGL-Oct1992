/*
HEADER:                 CUG205.00;
TITLE:                  TEE for Microsoft;
DATE:                   09/24/86;
DESCRIPTION:
  "Copies stdin to the specified file without changing stdout.";
KEYWORDS:               Software tools, Text filters,tee, pipes;
SYSTEM:                 MS-DOS;
FILENAME:               TEE.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <stdio.h>
#include <def.h>

#define LINT_ARGS 1
#define TRUE  1
#define FALSE 0

main(argc, argv)
int argc;
char *argv[];
{
  int cin;
  char correct, fname[MAXLINE];
  FILE *fout;

  correct = FALSE;
  if (argc == 2) {
    if ((fout = fopen(argv[1], "w")) == NULL) {
      fprintf(stderr,"tee:  cannot create %s\n", fname);
      exit(1);
    }
    correct = TRUE;
  }

  while ((cin = getchar()) != EOF) {
    putchar(cin);
    if (correct) 
      putc(cin, fout);
  }
  fclose(fout);
}
