/*	cypher1.c	Cypher module		by F.A.Scacchitti
**							10/10/85
**
**	Simple cypher module - encodes directly with user keys
**
*/

#include <stdio.h>

static int i, n, keylength;

cypher1(buffer, num, code) char *buffer, *code; int num;{

/*
** get keylength for each key
*/

   keylength = 0;
   while(code[keylength++] != NULL);
   keylength--;

/*
** encrypt the file with each key
*/

   printf("-encoding/decoding buffer\n");

   for(i=0; i<=num; i++)
      buffer[i] = buffer[i] ^ code[i % keylength];
}

