/*	
** makef.c	File Generator Program		by F.A.Scacchitti 10/7/85
**
**		Written in Small-C Version 2.10 or later
**
**		Creates a seqential file of characters from
**		0 to 255 in blocks of 255 bytes.
**		The sequential characters can be replaced
**		with any single character desired.
*/

#include <stdio.h>

#define BUFSIZE 256


int fdout;		/* file  i/o channel pointers */
int i, n, num;
char *outbuf, value; 

main(argc,argv) int argc, argv[]; {

/*
** Allocate memory for buffer
*/

   outbuf = malloc(BUFSIZE);

/*
** Check arguments passed and open file stream
*/
   if(argc < 3) {
      printf("\nmakef usage: makef <new file> <nnnn> [ddd]\n");
      printf("             nnnn = file size in 256 byte blocks\n");
      printf("             ddd  = optional alternate value in decimal\n");
      exit();
   }
   if((fdout = fopen(argv[1],"w")) == NULL) {
      printf("\nUnable to create %s\n", argv[1]);
      exit();
   }
/*
** Convert file size argument to integer
*/
   if((n = atoi(argv[2])) == NULL) exit();

/*
** Fill the buffer with 0 to 255 sequence
*/
   for(i = 0; i <=255; i++)
      outbuf[i] = i;

/*
** Refill the buffer with a single character if directed by argument
*/
   if(argc == 4)
      if((value = atoi(argv[3])) < 256)
         for(i = 0; i <=255; i++)
            outbuf[i] = value;

/*
** Write blocks to file
*/
   for(i=1; i <= n; i++)
      if((num = write(fdout,outbuf,BUFSIZE)) < BUFSIZE) exit();

/*
** Close up shop
*/

   fclose(fdout);
}

