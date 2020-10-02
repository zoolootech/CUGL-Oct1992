/*  Program to generate SUB files */
/*  Joe S. Vogler 08/22/83
    3622 Highland Lakes Drive
    Kingwood, Texas  77339
    (713) 358-7586 (evenings)
*/


#define CR          0x0d       /* Carriage return */
#include "bdscio.h"
/* begin externals */
int isopen;
int outbuf;

main(argc,argv)
char **argv;
{
   int c;
   int i;
   wildexp(&argc,&argv);	/* first statement in program  */
   printf("\nCATLBR.COM - Version 1.0 - August 22, 1983.");
   printf("\nSubmit file generator for cataloging of LBR files.");
   printf("\nJoe Vogler (713) 358-7586 (evenings)\n");
   if( argc < 2 ) {
      printf("\nUsage:\n\n  A> CATLBR  [{!}<afn>]... [{!}<afn>] <CR>");
      printf("\n\nThe ! is optional and directs program to exclude files");
      printf("\nSee CATLBR.DOC for additional info.\n");
      exit();
    }

   if(( outbuf=sbrk(BUFSIZ)) == ERROR ) {
      printf("\nCannot create output buffer for COMMANDS.SUB");
      exit();
    }

   if( fcreat("commands.sub",outbuf) == ERROR ) {
      printf("\nCannot open file COMMANDS.SUB.");
      exit();
    }

   for ( i = 1; i < argc; i++ )  {
       printf("Writing line number %d%c",i,CR);
       fprintf( outbuf,"$1 $3 %s      \n$2      \n",argv[i]);
    }

   fprintf(outbuf,"%c",CPMEOF);
   fflush(outbuf);
   fclose(outbuf);
   printf("Output file COMMANDS.SUB created on default disk");
   printf("\n ...for %d matches.",i-1);
   exit();
}
