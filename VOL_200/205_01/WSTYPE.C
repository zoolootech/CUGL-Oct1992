/*
HEADER:                 CUG205.00;
TITLE:                  WordStar TYPE Command;
DATE:                   09/24/86;
DESCRIPTION:
  "TYPE command for WordStar document files on MS-DOS.";
KEYWORDS:               Software tools, Text filters,wordstar type, text
                        formatter;
SYSTEM:                 MS-DOS;
FILENAME:               WSTYPE.C;
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
  int c;
  FILE *in;             /* File used for input            */

  if (argc != 2) {      /* Check if enough arguments      */
    fprintf(stderr,"WordStar TYPE command\n");
    fprintf(stderr,"Usage:      wstype source\n");
    exit(1);
  }

  if ((in = fopen(argv[1],"r")) == NULL) {
    fprintf(stderr,"wstype:  Can't open source file %s\n",argv[1]);     
    exit(2);
  }

  c = getc(in);
  while (c != EOF) {    /* Continue until end of file     */
    c = c &     127;    /* Strip most significant bit     */
    if (((c >= ' ') && (c <= '~')) || (c == '\n')       || 
      (c == '\r') || (c == '\t') || (c == '\f')) {
      putchar(c);
    }
    c = getc(in);
  }
  fclose(in);
}
