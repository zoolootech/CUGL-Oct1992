/*	
** fb.c		File Copy (Bin) Program		by F.A.Scacchitti  7/17/84
**
**		Written in Small-C Version 2.7 or later
**
**		Copies file from file to file
**		Byte modifications may be made during transfer
*/

#include <stdio.h>

#define BUFSIZE 0x4000  /* 16K */

FILE fdin, fdout;	/* file  i/o channel pointers */
int i, count;
char *inbuf;

main(argc,argv) int argc; char *argv[]; {

   i = 0;
   inbuf = malloc(BUFSIZE);

   if(argc != 3) {
	puts("\nfb usage: fb <source file> <new file> <CR>\n");
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
      write(fdout,inbuf,count);
      i += count;
   }
   i += count;
   write(fdout,inbuf,count);

   fclose(fdin);
   fclose(fdout);
}

