/*	
** cypher.c		File Cypher Program	by F.A.Scacchitti  9/11/85
**
**		Written in Small-C Version 2.10 or later
**
**		Copies from original file to encrypted file
**		using cypher key(s) passed to  encode or decode.
*/

#include <stdio.h>

#define BUFSIZE 16384

int fdin, fdout;	/* file  i/o channel pointers */
int n, count;
char *inbuf, *key;

main(argc,argv) int argc, argv[]; {

   inbuf = malloc(BUFSIZE);

/*
** Open file streams
*/
   if(argc < 4) {
      printf("\ncypher usage: cypher <source file> <new file> <key1> <key2> . . . <keyN> <CR>\n");
      exit();
   }
   if((fdin = fopen(argv[1],"r")) == NULL) {
      printf("\nUnable to open %s\n", argv[1]);
      exit();
   }
   if((fdout = fopen(argv[2],"w")) == NULL) {
      printf("\nUnable to create %s\n", argv[2]);
      exit();
   }

/*
** Read file - encode it - write new file 
*/
   do {
      printf("-reading file\n");

      count = read(fdin,inbuf,BUFSIZE);

      n=3;

      while(n++ <argc){
         key = argv[n-1];
         cypher(inbuf,count,key);
      }
      printf("-writing %d byte file\n\n", count);

      write(fdout,inbuf,count);

   } while(count == BUFSIZE);

   /* close up shop */

      fclose(fdin);
      fclose(fdout);
}
