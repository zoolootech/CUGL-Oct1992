/*	cypher3.c	Cypher module		by F.A.Scacchitti
**							11/09/85
**
**	Complex cypher module - generates a key of some prime length
**                              between 1024 and 2028 bytes then
**                              encrypts the buffer with this key
**						or
**				if key starts with a '-' (dash)
**				calculate a transposition block size
**				and invert (transpose) the file in
**				this size blocks 
**
*/

#include <stdio.h>

#define DASH 45
#define NEWBUF 2000
#define NUMPRIMES 50

static int i, j, n, index, length, sum, keylength;
static char *tbuff, c;
static int prime[] = {1009, 1999, 1013, 1997, 1019,
                      1993, 1021, 1987, 1031, 1979,
                      1033, 1973, 1039, 1951, 1049,
                      1949, 1051, 1933, 1061, 1931,
                      1063, 1913, 1069, 1907, 1087,
                      1901, 1091, 1889, 1093, 1879,
                      1097, 1877, 1103, 1873, 1109,
                      1871, 1117, 1867, 1123, 1861,
                      1129, 1847, 1151, 1831, 1153,
                      1823, 1163, 1813, 1171, 1803};

cypher(buffer, num, code) char *buffer, *code; int num;{

/*
** allocate a buffer for the new key or transposition
*/
   tbuff = malloc(NEWBUF);

/*
** get keylength and sumcheck for each key
*/

   keylength = sum = 0;
   while((n = code[keylength]) != NULL){
      sum += n;
      keylength++;
   }

/*
** do we transpose or encode ?
*/

   if((c = *code) == DASH)
      transpose(buffer, num, code);
   else
      encode(buffer, num, code);

/*
** get rid of the buffer
*/

   cfree(tbuff);

}

/*
** Here's where we transpose
*/

transpose(buffer, num, code) char *buffer, *code; int num;{

   length = (((sum + keylength) % 16) & 15) + 2;

   printf("-transposing file by %d\n",length);

   index = 0;

   do{
      for(i = 0; i < length; i++){
         j = length - i - 1;
         tbuff[j] = buffer[index + i];
      }
      for(i = 0; i < length; i++){
         buffer[index + i] = tbuff[i];
      }
      index += length;

   }while(index < count);

}

/*
** Here's where we encode
*/

encode(buffer, num, code) char *buffer, *code; int num;{

/*
** Select a prime and generate a new key that length
*/

   length = prime[sum % NUMPRIMES];

   printf("-generating a %d byte key\n",length);

   for(i=0; i<length; i++){
      index = i % keylength;
      sum = code[index] + sum & 255;
      tbuff[i] = code[index] ^ sum;
   }

/*
** encrypt the file with the generated key
*/

   printf("-encoding/decoding buffer\n");

   for(i=0; i<=num; i++)
      buffer[i] = buffer[i] ^ tbuff[i % length];


}

