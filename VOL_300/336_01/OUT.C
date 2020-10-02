/*
*			O U T P U T F O R M . C
*
*                       written by Marwan El-Augi 22-oct-1990
*
*           utility for printing number columns for edipal
*
*           copyright (c) 1990, Marwan EL-AUGI, MEASoftlines, France
*           but distribution is not restricted as long as done
*           in a freeware form
*/

#include <stdio.h>
#include <process.h>


#define  MAX    31
#define  OK     0
#define  BAD    1

void xprt(FILE *name);
void bye(int status);
void _Cdecl clrscr(void);  /*  from conio.h  include file */

char *title = "Palette codes chart";

int status;

void main(int argc, char *argv[])
{

   if( (argc < 2))
     {
	 FILE *outfile;
	 char *name = "c:\\tc\\garbage\\printcol.dat";

	  if ( (outfile = fopen(name, "w")) == NULL)
	      {
		  printf("some error has occured...\n");
		  status = BAD;
		  bye(status);
	       }
	   xprt(outfile);
	   fclose(outfile);
	   status = OK;
	   bye(status);
     }
     else
      {

	if( (*argv[1] != 'p') && (*argv[1] != 'P'))
	 {
	     printf("\nusage is : %s  [p|P] (p being printing option)\n",argv[0]);
	     status = BAD;
	     bye(status);
	  }
	  else
	    {
	       xprt(stdprn);
	       status = OK;
	       bye(status);
	     }
       }


}

void xprt(FILE *name)
{
   int i;
   fprintf(name,"\t\t\t\t%s\n", title);
   for(i=0; i<=MAX; i++)
     {
	fprintf(name, "%02d EGA_\t\t\t\t\t%2d EGA_\n\n", i, i + 32);
     }

}

void bye(int status)
{

    if(status == OK)
      {
	clrscr();
	printf("\nDone...");
       }

    printf("\nBye bye from MEASoftlines...\n");
    exit(status);
}