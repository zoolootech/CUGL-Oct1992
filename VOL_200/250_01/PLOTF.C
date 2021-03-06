#include <stdio.h>

#define void     VOID
#define READFAIL (-1)

/* ------------------------------------------------------------------------- */

main(argc, argv)
int   argc;
char  *argv[];
{
   FILE   *fopenb(),
          *infile;

   register int   c;



   if (argc != 2)
   {
      fprintf(stderr, "%s", "usage: plotf <picture file>");
      exit(1);
   }

   if ( !(infile = fopenb(argv[1], "r")) )
   {
      fprintf(stderr, "%s%s", "unable to open ", argv[1]);
      exit(1);
   }

   while ( (c = getc(infile)) != READFAIL )
      pputc(c);

   fclose(infile);
}

/* ------------------------------------------------------------------------- */
                                                                              