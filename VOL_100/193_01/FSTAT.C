/*	
** fstat.c	File Statistics Program		by F.A.Scacchitti 10/8/85
**
**		Written in Small-C Version 2.10 or later
**
**		Scans file and displays distribution of 
**		characters.
**		Calculates and displays mean, mode, median 
**		and range of file.
**		Displays histogram of distribution.
*/

#include <stdio.h>

#define BUFSIZE 16384

int fdin;	/* file  pointer */
int i, j, temp, value, count, total, *file, *sorted;
int sum, hisum, meansum, himeansum, mean, eflag, changing;
int median, oddmedian, range, min, max, mode;
int *data, scale;
char c, *inbuf;

main(argc,argv) int argc, argv[]; {

   if(argc < 2) {
      printf("\nfstat usage: fstat <input file>\n");
      exit();
   }
   if((fdin = fopen(argv[1],"r")) == NULL) {
      printf("\nUnable to open file %s\n",argv[1]);
      exit();
   }

   inbuf = calloc(BUFSIZE,1);
   file = calloc(256,2);
   sorted = calloc(256,2);
   data = calloc(17,2);
   eflag = FALSE;
   sum = hisum = meansum = himeansum = mean = mode = j = 0;

   printf("reading the file-");

   do {
      count = read(fdin,inbuf,BUFSIZE);

      for(i=0; i< count; i++){

         value = inbuf[i];

                        
         if(value < 0)
            value = 256 + value;
         file[value]++;
         if(++sum == 10000){
            hisum++;
            sum =0;
         }
         if((meansum += value) >= 10000){
            himeansum++;
            meansum -= 10000;
         }
      }
   } while(count == BUFSIZE);

/*
** Calculate the mean
*/

   printf("calculating mean-");

   do{
      if((meansum -= sum) < 0)
         if(himeansum > 0){
            himeansum--;
            meansum += 10000;
         }else{
            meansum += sum;
            eflag = TRUE;
            mean--;
         }
      if((himeansum -= hisum) < 0){
         himeansum += hisum;
         eflag = TRUE;
      }else{
         mean++;
      }
   }while(eflag == FALSE);

/*
** Calculate range, find mode min and max, fill the sorted array
*/

   printf("calculating range-");

   min = max = file[0];

   for(i = 0; i <= 255; i++){
      sorted[i] = file[i];
      if(file[i] > max){
         max = file[i];
         mode = i;
      }
      if(file[i] < min)
         min = file[i];
   }
   range = max - min + 1;

/*
** Sort the sorted array to calculate median
*/

   printf("sorting the array");

   changing = TRUE;

   while(changing){
      changing = FALSE;
      for(i = 0; i <= 254; i++)
         if(sorted[i] > sorted[i+1]){
            temp = sorted[i];
            sorted[i] = sorted[i+1];
            sorted[i+1] = temp;
            changing = TRUE;
         }
   }

   median = (sorted[128] + sorted[127]) / 2;
   oddmedian = (sorted[128] + sorted[127]) % 2;

/*
** Display the results
*/

   printf("\n    0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F\n");

   for(i = 0; i <= 255; i++) {
      printf("%5d", file[i]);
      chkkbd();
   }

   printf("\n %d%04d characters read from file %s\n", hisum, sum, argv[1]);
   printf("file mean = %d  ",mean);
   if((himeansum || meansum) > 0)
      printf("%d%04d/%d%04d",himeansum,meansum,hisum,sum);
   printf("         mode = %d  ( %x hex)", mode, mode);
   printf("\n");
   printf("file median = %d", median);
   if(oddmedian)
      printf(" 1/2 ");
   else
      printf("     ");
   printf("    file range = %d  [ min = %d    max = %d ]\n", range, min, max,);
   printf("\nDepress spacebar to display histogram ");
   getchar();

/*
** Sum the data in 16 groups of 16 elements and find max. value
*/
   max = 0;
   for(i = 1; i <= 16; i++){
      for(j = 0; j <= 15; j++)
         data[i] += file[(i - 1) * 16 + j];
      if(data[i] > max)
         max = data[i];
   }
/*
** Calculate scaling for plot
*/
   scale = max / 50;
   temp = max % 50;
   if(temp / scale > 7)
      scale++;
   printf("   scale = %4d\n\n", scale);

/*
** Print data and plot of histogram
*/

   for(i = 0; i <= 15; i++){
      printf(" %3d to %3d = %5d ||",i  * 16, (i * 16) + 15, data[i + 1]);
      temp = data[i + 1] / scale;
      if(data[i + 1] % scale > 0)
         temp++;
      while(temp-- > 0)
         printf("*");
      printf("\n");
   }

/*
** close up shop
*/
   fclose(fdin);
}

chkkbd(){ char c;

   if((c = bdos(6,255)) == 19)     /* hold on ^S */
      if((c = getchx()) == 3)
         exit();                    /* exit on ^C */
}                                   /* continue   */

