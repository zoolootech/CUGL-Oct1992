/*	
** fm.c		File Modification Program	by F.A.Scacchitti  9/23/84
**
**		Written in Small-C  Version 2.7 or later
**
**	Copies from source file to new file modifying on the fly.
**	Mod. description is in modify routine.
**
**	This program may be used as a shell for any ascii file
**		modification utilities.
*/

#include <stdio.h>

FILE fdin, fdout;	/* file  i/o channel pointers */
char c;			/* the byte we're modifying */

main(argc,argv) int argc; char *argv[]; {

   if (argc != 3) {
      puts("\nfm usage: fm <source file> <new file> <CR>\n");
      exit();
   }
   if((fdin = fopen(argv[1],"r")) == NULL) {
      puts("\nUnable to open input file\n");
      exit();
   }
   if((fdout = fopen(argv[2],"w")) == NULL) {
      puts("\nUnable to create output file\n");
      exit();
   }

   while((c = fgetc(fdin)) != EOF)
/*
**	Here's where we modify the file
*/
      fputc((modify(c)),fdout);

   fclose(fdin);
   fclose(fdout);
	
}

/* this routine double spaces an ascii file or adds missing line feeds */

modify(c) char c; {

   if( c == CR) {
      fputc(CR,fdout);
      return(LF);
   }else
      return(c);
}

