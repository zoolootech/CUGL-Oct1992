/*	cypher2.c	Cypher module		by F.A.Scacchitti
**							10/11/85
**
**	Complex cypher module - generates a key of some prime length
**                              between 1024 and 2028 bytes then
**                              encrypts the buffer with this key
**
*/

#include <stdio.h>

#define NEWBUF 2000
#define NUMPRIMES 50

static int i, n, index, length, sum, keylength;
static char *newkey;
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

cypher1(buffer, num, code) char *buffer, *code; int num;{

/*
** allocate a buffer for the generated key 
*/
   newkey = malloc(NEWBUF);

/*
** get keylength and sumcheck for each key
*/

   keylength = sum = 0;
   while((n = code[keylength]) != NULL){
      sum += n;
      keylength++;
   }

/*
** Select a prime and generate a new key that length
*/

   length = prime[sum % NUMPRIMES];

   printf("-generating a %d byte key\n",length);

   for(i=0; i<length; i++){
      index = i % keylength;
      sum = code[index] + sum & 255;
      newkey[i] = code[index] ^ sum;
   }

/*
** encrypt the file with the generated key
*/

   printf("-encoding/decoding buffer\n");

   for(i=0; i<=num; i++)
      buffer[i] = buffer[i] ^ newkey[i % length];

/*
** get rid of the buffer
*/

   cfree(newkey);

}

