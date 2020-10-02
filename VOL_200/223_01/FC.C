/*
** fc.c		File Copy Program	by F.A.Scacchitti  3/5/86
**
**		Written in Small-C  Version 2.5 or later
**
**		Copies file from file to file
**		Byte modifications may be made during transfer
**
**	printf may be substituted for prntf  (fas)
**
*/

#include <stdio.h>


main(argc,argv) int argc; char *argv[]; {

FILE fdin, fdout;	/* file  i/o channel pointers */
char c;

   if (argc != 3) 	{
      prntf("\nfc usage: fc <source file> <new file> <CR>\n");
      exit();
   }
   if((fdin = fopen(argv[1],"r")) == NULL) {
      prntf("\nUnable to open file %s\n",argv[1]);
      exit();
   }
   if((fdout = fopen(argv[2],"w")) == NULL) {
      prntf("\nUnable to create file %s.\n",argv[2]);
      exit();
   }

   while((c = fgetc(fdin)) != EOF)

      fputc(c,fdout);

   fclose(fdin);
   fclose(fdout);
}

