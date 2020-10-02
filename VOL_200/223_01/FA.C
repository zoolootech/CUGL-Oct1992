
/*	
** fa.c		File Append Program	by F.A.Scacchitti  7/17/86
**
**		Written in Small-C  Version 2.7 or later
**
**		Appends one files on to another
**
**	printf may be substituted for prntf (fas)
*/

#include <stdio.h>

int fdin, fdin2;

main(argc,argv) int argc; char *argv[]; {

FILE fdin, fdin2;
char c;

   if (argc != 3) {
      prntf("\nfa usage: fa <original file> <file to add> <CR>\n");
      exit();
   }
   if((fdin = fopen(argv[1],"a")) == NULL) {
      prntf("\nUnable to open file %s\n",argv[1]);
      exit();
   }
   if((fdin2 = fopen(argv[2],"r")) == NULL) {
      prntf("\nUnable to create file %s.\n",argv[2]);
      exit();
   }

   while((c = fgetc(fdin2)) != EOF)
      fputc(c,fdin);

   fclose(fdin);
   fclose(fdin2);
}

