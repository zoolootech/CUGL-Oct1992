/*
HEADER:        ;
TITLE:         QUIP;
VERSION:       2.0;
DESCRIPTION:   "Part of a fortune cookie system
             
               This program takes the quip file 'quip.dat' and creates an new
               index file 'quip.key' from it.";
KEYWORDS:      quip, fortune, utility, startup;
SYSTEM:        MS-DOS, UNIX;
FILENAME:      quipupdt.c;
WARNINGS:      "";
SEE-ALSO:      QUIP.C, QUIP.H, QUPUPDT.C QUIPADD.C, QUIP.DAT, QUIP.KEY;
AUTHORS:       ??;
MODIFIED BY:   David Bryant;
COMPILERS:     Microsoft Quick C;
*/

#include <stdio.h>

FILE  *seekfp,    /* the address file */
      *quipfp;    /* the actual fortune file */

#include    "quip.h"

openfiles()
{
   seekfp = fopen(seekname, "wb");
   if (seekfp == 0) {
      puts("Cannot open address file.");
      exit(0);
   }
   quipfp = fopen(quipname, "r");         /* Open as TEXT file */
   if (quipfp == 0) {
      puts("Cannot open quip data file.");
      exit(0);
   }
}

putaddr(offset)
long  offset;
{
   if (fwrite((char *)&offset, sizeof(long), 1, seekfp) != 1)
      printf("write error on address file\n");
}

int findquips()
{
   #define MAX_LINE  128                  /* Max line length */
   int   count = 0;
   char  line[MAX_LINE];

   putaddr(0L);
   for (;;)
      {
      if (fgets(line, MAX_LINE, quipfp) == 0)
         return count;
      if (line[0] == SEPERATOR)
         {
         putaddr((long)ftell(quipfp));
         count++;
         }
      }
}

closefiles()
{
   fclose(seekfp);
   fclose(quipfp);
}

main()
{
   openfiles();
   printf("Files open...\n");
   printf("There were %d quips found.",findquips());
   closefiles();
}

