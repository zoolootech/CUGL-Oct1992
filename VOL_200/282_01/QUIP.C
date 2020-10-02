/*
HEADER:        ;
TITLE:         QUIP;
VERSION:       2.0;
DESCRIPTION:   "Part of a fortune cookie system

               This program displays the actual fortune.  An optional
               command line paramater detirmines the number of quips
               displayed";
KEYWORDS:      quip, fortune, utility, startup;
SYSTEM:        MS-DOS, UNIX;
FILENAME:      quip.c;
WARNINGS:      "";
SEE-ALSO:      QUIP.C, QUIP.H, QUPUPDT.C QUIPADD.C, QUIP.DAT, QUIP.KEY;
AUTHORS:       ??;
MODIFIED BY:   David Bryant;
COMPILERS:     Microsoft Quick C;
*/

#include <stdio.h>

FILE  *seekfp,    /* the address file */
      *quipfp;    /* the actual fortune file */

#include "quip.h"

unsigned bmap[2048];

bittst(bitno)
int   bitno;
{
   return (bmap[bitno / 16] & (1 << (bitno % 16)));
}

bitset(bitno)
int   bitno;
{
   bmap[bitno / 16] |= (1 << (bitno % 16));
}

openfiles()
{
   seekfp = fopen(seekname, "rb");
   if (seekfp == 0) {
      puts("Cannot open address file.");
      exit(0);
   }
   quipfp = fopen(quipname, "rb");        /* Open as BINARY file */
   if (quipfp == 0) {
      puts("Cannot open fortunes file.");
      exit(0);
   }
}

long pickquip()
{
   unsigned size,
            count = 0;
   int      whseek;
   long     whquip;

   if (fseek(seekfp, 0L, 2))
      printf("fseek of address file failed\n");
   size = ftell(seekfp) / sizeof(long);
   do {
      whseek = (rand() % (size - 1)) + 1;
      if (count++ > 32767L)
         exit(2);
   } while (bittst(whseek));
   bitset(whseek);
   fseek(seekfp, (long)whseek * (long)sizeof(long), 0);
   if (fread((char *)&whquip, sizeof(long), 1, seekfp) < 1)
      puts("fread error in address file.");
   return ((long)whquip);
}

showquip(addr)
long  addr;
{
   int   c;

   fseek(quipfp, addr, 0);
   putchar('\n');
   for (;;) {
      c = getc(quipfp);
      if (c == SEPERATOR || c == EOF) {
         return;
      }
      /* filter out control characters */
      if (c < ' ' && c != '\n' && c != '\007' && c != '\t' && c != 0xD)
         continue;
      putchar(c);
   }
}

closefiles()
{
   fclose(seekfp);
   fclose(quipfp);
}

main(argc, argv)
int   argc;
char  *argv[];
{
   int   times = 1;

   srand((unsigned)time(NULL));
   if (argc > 1)
      times = atoi(argv[1]);
   openfiles();
   while (times--) {
      showquip(pickquip());
      if (times)
         printf("-------\n");
   }
   closefiles();
}

