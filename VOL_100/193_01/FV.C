/*	
** fv.c		File View/Compare Program	by F.A.Scacchitti  9/11/85
**
**		Written in Small-C Version 2.10 or later
**
**		Dumps contents of single file to screen
**				or
**		Dumps contents of 2 files and xored difference
** 
**		Displays in hex and ascii form
*/

#include <stdio.h>

#define BUFSIZE 1024

int fdin1, fdin2;	/* file  i/o channel pointers */
int i, j, k,  val, count, total, offset, numdisp;
char *inbuf1, *inbuf2, *difbuf, c;

main(argc,argv) int argc, argv[]; {

   switch (argc) {
   case 2:
   case 3:
      inbuf1 = malloc(BUFSIZE);
      if((fdin1 = fopen(argv[1],"r")) == NULL) {
         printf("\nUnable to open %s\n", argv[1]);
         exit();
      }
      numdisp = 1;
      if(argc == 2) break;
   case 3:
      inbuf2 = malloc(BUFSIZE);
      difbuf = malloc(BUFSIZE);
      if((fdin2 = fopen(argv[2],"r")) == NULL) {
         printf("\nUnable to open %s\n", argv[2]);
         exit();
      }
      numdisp = 3;
      break;
   default:
      printf("\nfv usage: fv <file1>          -  dump file\n");
      printf("          fv <file1> <file2>  -  compare 2 files\n");
      exit();
   }
   total = offset = 0;

   printf("\n");

   do {
      count = read(fdin1,inbuf1,BUFSIZE);
      if(argc >= 3){
         read(fdin2,inbuf2,BUFSIZE);

         for(i=0; i< count; i++)
            difbuf[i] = inbuf1[i] ^ inbuf2[i];
      }

      for(i=0; i< count; i+=16){

         for(k=1; k <= numdisp; k++){

            switch (k) {
            case 2:
               offset = BUFSIZE;
               break;
            case 3:
               offset = 2 * BUFSIZE;
               break;
            default:
               offset = 0;
               break;
            }
            if(k < 3)
               printf("f-%d", k);
            else
               printf("dif");

            printf(" %04x  ",i+total);

            for(j=0; j<=15; j++){
               val = inbuf1[i + j + offset];
               printf("%02x ",val < 0 ? val - 65280 : val);

               if((c = bdos(6,255)) == 19){     /* hold on ^S */
                  if((c = getchx()) == 3)
                     exit();                    /* exit on ^C */

               }                                /* continue on ^Q */

            }
            printf("  ");

            for(j=0; j<=15; j++){
               c = inbuf1[i + j + offset];
               if(c > 31)
                  putchar(c);
               else
                  if(c==0)
                     putchar(61);
                  else
                     putchar(94);
            }
            printf("\n");
            if(k == 3) printf("\n");
         }
      }
      total += count;

   } while(count == BUFSIZE);

      /* close up shop */

   fclose(fdin1);
   if(argc == 3)
      fclose(fdin2);
}

