/*
HEADER:                 CUG152.02;
TITLE:                  Lump File Transfer Utility;
DATE:                   09/04/85;
DESCRIPTION:
  "Transfers files in a group or "lump" such as
   to or from a BBS."
KEYWORDS:               lump, file, transfer;
FILENAME:               LUMP.C;
WARNINGS:
  "The source code contains the comment that input files must
      contain only printable ASCII characters.
      (Note: The example in LUMP.DOC includes NON-ASCII files.
   If the output file exists, the program aborts.
   If no parameters are given the program tells about itself.
   The authors claim copyrights and authorize non-commercial
      use only.
   The typist of this heading does not have equipment to run
      this program and prepared this header from comments
      within the source code or companion files."
AUTHORS:                David N. Smith;
COMPILERS:              CI/C86;
REFERENCES:
    AUTHERS:            " ";
    TITLE:              " ";
    CITATION:           " ";
ENDREF
*/
/* lump together a group of files. (Use UNLUMP to break apart */

#include "stdio.h"

FILE *infile, *outfile;

main(argc,argv)
int argc;
char *argv[];
{
   int c, i;

   if(argc<3)  { usage(); exit(0); }

   argc--;
   if( exists(argv[1]) )    { usage(); exit(1);  }
   if( (outfile=fopen(argv[1], "w")) == NULL )    { usage();  exit(1); }
   for( i=2; i<=argc; i++ ) {
      if( (infile=fopen(argv[i],"r")) == NULL )  { inerr(argv[i]); exit(2); }
      fprintf(outfile, "*FILE: %s\n", argv[i]);
      printf("*FILE: %s\n", argv[i]);
      while ( (c=getc(infile)) >= 0)
         putc(c,outfile);
      fclose(infile);
   }

}


usage()
{
   printf("Usage:  lump outfile infile1 ... \n");
   printf("   Lump all input files into the output file,\n");
   printf("   each being preceeded with a line identifying itself:\n");
   printf("      *FILE: filename.ext\n\n");
}


inerr(s)
char *s;
{
   printf("\nError opening file %s\n", s);
}


exists(s)
char *s;
{
   FILE *f;
   if( (f=fopen(s,"r")) == NULL )  return 0;
   fclose(f);
   return 1;
}
