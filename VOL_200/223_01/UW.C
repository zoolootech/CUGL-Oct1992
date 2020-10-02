/*	
** uw.c		File Copy & UW(Bin) Program	by F.A.Scacchitti  7/17/84
**
**		Written in Small-C Version 2.09 or later
**
**		Copies file from file to file
**		Un-wordstars as it goes (resets bit 7 of all bytes)
*/

#include <stdio.h>

#define BUFSIZE 0X4000  /* 16K */

FILE fdin, fdout;	/* file  i/o channel pointers */
int i, j, count;
char *inbuf;

main(argc,argv) int argc; char *argv[]; {

   i = 0;
   inbuf = malloc(BUFSIZE);

   if(argc != 3) {
	puts("\nuw usage: uw <source file> <new file> <CR>\n");
	exit();
   }
   if((fdin = fopen(argv[1],"r")) == NULL) {
      puts("\nUnable to open input file.\n");
      exit();
   }
   if((fdout = fopen(argv[2],"w")) == NULL) {
      puts("\nUnable to create output file.\n");
      exit();
   }

   while((count = read(fdin,inbuf,BUFSIZE)) == BUFSIZE) {
      for(j = 0; j < BUFSIZE; j++) inbuf[j] = inbuf[j] & 127;
      write(fdout,inbuf,count);
      i += count;
   }
   i += count;
      for(j = 0; j < count % BUFSIZE; j++) inbuf[j] = inbuf[j] & 127;
   write(fdout,inbuf,count);

   fclose(fdin);
   fclose(fdout);
}

