/*
HEADER:        ;
TITLE:         QUIP;
VERSION:       2.0;
DESCRIPTION:   "Part of a fortune cookie system

               This program appends new quips to the end of the quip file 'quip.dat',
               and updates the index file 'quip.key'.";
KEYWORDS:      quip, fortune, utility, startup;
SYSTEM:        MS-DOS, UNIX;
FILENAME:      quipadd.c;
WARNINGS:      "";
SEE-ALSO:      QUIP.C, QUIP.H, QUPUPDT.C QUIPADD.C, QUIP.DAT, QUIP.KEY;
AUTHORS:       ?;
MODIFIED BY:   David Bryant;
COMPILERS:     Microsoft Quick C;
*/


#include <stdio.h>

FILE  *seekfp,    /* the address file */
      *quipfp;    /* the actual fortune file */

#include "quip.h"

char  buffer[MAX_SIZE];

openfiles()
{
   seekfp = fopen(seekname, "r+b");
   if (seekfp == 0) {
      puts("Cannot open quip address file.");
      exit(0);
   }
   quipfp = fopen(quipname, "r+");       /* Open as TEXT file */
   if (quipfp == 0) {
      puts("Cannot open quip data file.");
      exit(0);
   }
}

putaddr(addr)
long  addr;
{
   if (fwrite(&addr, sizeof(long), 1, seekfp) < 1)
      printf("\nWrite error on address file!\n");
}

addquip()
{
   int   i = 0,
         c;
   if (fseek(seekfp, 0L, 2) / 2 > 32767L) {
      puts("Sorry, the quip file has too many entries.");
      return;
   }
   fseek(quipfp, 0L, 2);
   printf("Add your quip and end with a %c.\n", SEPERATOR);
   printf("Each quip can be a maximum of %d characters long.\n\n", MAX_SIZE);
   while (((c = getchar()) != SEPERATOR) && i <= MAX_SIZE)
      buffer[i++] = c;
   fprintf(quipfp, "%c\n", SEPERATOR);
   putaddr(ftell(quipfp));
   if (fwrite(buffer, i, 1, quipfp) != 1) {
      printf("Write of quip failed!\n");
      exit(2);
   }
}

closefiles()
{
   fclose(seekfp);
   fclose(quipfp);
}

main()
{
#ifdef UNIX
   if (!strcmp(getlogin(), "demo") || !strcmp(getlogin(), "intro"))
      {
      printf("Sorry, but demo cannot add quips.\n");
      exit(0);
      }
#endif
   openfiles();
   addquip();
   closefiles();
}

