/*	
** sp.c		Search Pattern Program		by F.A.Scacchitti 10/15/85
**
**		Written in Small-C Version 2.10 or later
**
**		Searches file for repetitive pattern.
**
**
*/

#include <stdio.h>

#define BUFSIZE 16384


int  fdin;		/* file  i/o channel pointers */
int i, j, n, block, start, depth, count, limit;
char *c, *inbuf; 

main(argc,argv) int argc, argv[]; {

/*
** Set defaults
*/
   block = 128;
   depth = 4;
   start = 0;

/*
** Allocate memory for buffer
*/
   inbuf = malloc(BUFSIZE);

/*
** Check arguments passed and open file stream
*/
   if(argc < 2) {
      printf("\nsp usage: sp <source file> [nnnn] [dddd] [ssss] [x]\n");
      printf("          nnnn = block size to search   default = 128\n");
      printf("          dddd = minimum depth of comparison  default = 4\n");
      printf("          ssss = starting point in buffer default = 0\n");
      printf("          x - any char. gen's difference buffer ((n+1)-n)\n");
      exit();
   }
   if((fdin = fopen(argv[1],"r")) == NULL) {
      printf("\nUnable to open %s\n", argv[1]);
      exit();
   }

/*
** Convert optional inputs to integer and implement
*/
   if(argc > 2)
      if((n = atoi(argv[2])) != NULL) 
         block = n;

   if(argc > 3)
      if((n = atoi(argv[3])) != NULL) 
         depth = n;

   if(argc > 4)
      if((n = atoi(argv[4])) != NULL) 
         start = n;

/*
** Fill the buffer with as much as possible
*/
   count = read(fdin,inbuf,BUFSIZE);

   limit = count - depth;

/*
** If there's a sixth argument, convert the file to numerical sequence
*/

   if(argc > 5){
      for(i = 0; i < count - 1; i++)
         inbuf[i] = inbuf[i + 1] - inbuf[i];
   }

   for(i = start; i < block; i++){
      printf("%c", (i % 10 == 0) ? '|' : '.');
      chkkbd();
      for(j = i + 1; j <= limit; j++)
         if(inbuf[i] == inbuf[j]){
            if((n = chkdepth(i, j, 0)) >= depth)
               printf("\nmatch at %d (%x hex) and %d (%x hex) - %d deep\n", i, i, j, j, n);
            if(n >= block) exit();
         }
   }
/*
** Close up shop
*/
   printf("\n");	/* Flush print buffer */

   fclose(fdin);
}

chkdepth(pointer, offset, k)int pointer, offset, k;{


   while(inbuf[pointer] == inbuf[offset] && k < count){
      pointer++;
      offset++;
      k++;
   }
   return(k);
}

chkkbd(){

char c;

   if((c = bdos(6,255)) == 19){     /* hold on ^S */
      if((c = getchx()) == 3)
         exit();                    /* exit on ^C */

   }                                /* continue   */
}

