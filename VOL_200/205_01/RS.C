/*
HEADER:                 CUG205.00;
TITLE:                  Whitespace Remover;
DATE:                   09/24/86;
DESCRIPTION:
  "Filter to remove whitespaces (SPACEs and TABs) from the ends of lines.";
KEYWORDS:               Software tools, Text filters,whitespace remover,
                        space, tabs;
SYSTEM:                 MS-DOS;
FILENAME:               RS.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <stdio.h>
#define MAXLINE 256 
#define YES     1
#define NO      0

main(argc,argv)
int argc;                       /* Number of command line words   */
char *argv[];                   /* Pointers to command line words */
{
  char *result, line[MAXLINE];          /* Current input line */

  if (argc != 1) {
    fprintf(stderr,"rs:  remove space filter\n");
    fprintf(stderr,"Error: extra characters after command\n");
    exit(1);
  }

  while ((result = gets(line)) != NULL) {
    remove(line);
    puts(line);
  }
}

/* Remove trailing blanks and tabs */
remove(s)       
char s[];
{
  int i;
  int nl;                               /* Newline character flag        */

  i = 0;
  while(s[i] != '\0')                   /* Find the end of char string s */
    ++i;
  --i;
  if (s[i] == '\n') {
    nl = YES;
    --i;
  }
  else
    nl = NO;
  while ( i >= 0 && (s[i] == ' ' || s[i] == '\t' ) )
    --i;
  if (i >= 0) {
    ++i;
    if (nl == YES) {
      s[i] = '\n';
      ++i;
    }
    s[i] = '\0';
  }
  return(i);
}
