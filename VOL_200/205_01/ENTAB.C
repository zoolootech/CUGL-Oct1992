/*
HEADER:                 CUG205.00;
TITLE:                  ENTAB for Microsoft C;
DATE:                   09/24/86;
DESCRIPTION:
  "Places TABs at the appropriate locations in text files.";
KEYWORDS:               Software tools, Text filters;
SYSTEM:                 MS-DOS;
FILENAME:               ENTAB.C;
WARNINGS:
  "The author claims copyrights and authorizes non-commercial use only.";
AUTHORS:                 Michael M. Yokoyama;
COMPILERS:              Microsoft;
*/

#include <stdio.h>
#include <stdlib.h>

#define MAXLINE    100          /* Maximum line size            */
#define TABINC       8          /* Default TAB increment size   */
#define YES          1
#define NO           0

FILE *in;                       /* File used for input            */

main(argc,argv)
int argc;                       /* Number of command line words   */
char *argv[];                   /* Pointers to command line words */
{
  int tab[MAXLINE+1];

  if (argc != 2) {
    fprintf(stderr,"TAB placement utility\n");
    fprintf(stderr,"Usage:      entab filename\n");
    exit(1);
  }

  if ((in = fopen(argv[1],"r")) == NULL ) {
    fprintf(stderr,"entab:  can't open %s\n",argv[1]);  
    exit(1);
  }

  settab(argc,TABINC,tab);
  entab(tab);
  fclose(in);
}

/* Initialize TAB stops
*/
settab(argc, argv, tab)
int argc;
char *argv[];
int tab[];
{
  int i;

  for (i = 1; i <= MAXLINE ; i++)
    if (i %     TABINC == 1)
      tab[i] = YES;
  else
    tab[i] = NO;
}

/* Insert TABs
*/
entab(tab)
int tab[];
{
  int c, pos;
  int nb = 0;                   /* Number of blanks needed      */
  int nt = 0;                   /* Number of TABs needed        */

  for (pos = 1; (c = getc(in)) != EOF; pos++)
    if (c == ' ')
      if (tabpos(pos+1, tab) == NO)
        ++nb;                   /* Insert blank         */
      else {
        nb = 0;                 /* Insert TAB           */
        ++nt;
      }
    else {
      while (nt) {              /* Output               */
        putchar('\t');
        --nt;
      }
      if (c!=   '\t')
        while (nb > 0) {
          putchar(' ');
          --nb;
        }
      else
        nb = 0;
      putchar(c);
      if (c == '\n')
        pos = 0;
      else if   (c == '\t')
        while (tabpos(pos+1,tab) != YES)
          ++pos;
    }
}

/* Determine if position is a TAB stop
*/
tabpos(pos, tab)
int pos;
int tab[];
{
  if (pos       > MAXLINE)
    return(YES);
  else
    return(tab[pos]);
}
