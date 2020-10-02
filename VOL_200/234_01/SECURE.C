/*
  HEADER: CUG	 nnn.nn;
  TITLE:	 XDIR - Hard Disk Manager
  VERSION:	 1.0 for IBM-PC
  DATE: 	 Apr 03, 1987
  DESCRIPTION:	 Hard Disk Manager for IBM PC
  KEYWORDS:	 Hard Disk Manager Dump Directory
  SYSTEM:	 IBM-PC and Compatiables
  FILENAME:      secure.c
  WARNINGS:	 None
  CRC:		 N/A
  SEE-ALSO:	 HDIR.DOC and XDIR.DOC
  AUTHOR:	 Mike Blakley 15645 SW 82 Cir Ln #76, Miami, Fl 33193
  COMPILERS:	 ECO-C
  REFERENCES:	 XDIR.DOC
*/


/*
    secure.c
    from DDJ 5/86

*/
#include "stdio.h"
#define BUFSIZE 16384
#define NEWBUF 2000
#define NUMPRIMES 50

/*
   secure
   security encryption module

*/
int secure(ifile,ofile,key,keycount)
char *ifile;      /* file to encrypt */
char *ofile;      /* name of encrypted output file */
char *key;        /* key value for encryption */
int  keycount;    /* number of keys */
{
    int  fdin, fdout;
    int  n, count;
    char *inbuf;
    int  argc;
    char *calloc(int, int);
    static char *argv[3] = {
    NULL,NULL,"                      "};
    static char keyx[20];

    strcpy(argv[2],key);
    strcpy(keyx,key);
    argc = 3 + keycount;
    inbuf = calloc(BUFSIZE,1);
    if  (inbuf == NULL) return(-1);


    if ((fdin = open(ifile,0)) == EOF)
       {
       writestr("\nCan't open input ");
       writestr(ifile);
       getch();
       return(-1);
       }

    if ((fdout = creat(ofile,0)) == EOF)
       {
       writestr("\nCan't open output ");
       writestr(ofile);
       getch();
       return(-1);
       }

    while (1)
       {
       count = read(fdin,inbuf,BUFSIZE);
       n = 3;

       while (n++ <argc)
          {
          /* keyx = argv[n-1]; */
          cypher(inbuf,count,keyx);
          }

       n = write(fdout,inbuf,count);
       if (count < BUFSIZE) break;
       }


       close(fdin);
       close(fdout);
       return (0);

}

/*
   cypher1.c
   DDJ 5/86

*/

void cypher(buffer,num,code)
char *buffer, *code;
int num;
{
static int i, n, index, length;
static int sum, keylength;
static char *newkey;
static int prime[] = {
      1009, 1999, 1013, 1997, 1019,
      1993, 1021, 1987, 1031, 1979,
      1033, 1973, 1039, 1951, 1049,
      1949, 1051, 1933, 1061, 1931,
      1063, 1913, 1069, 1907, 1087,
      1901, 1091, 1889, 1093, 1879,
      1097, 1877, 1103, 1873, 1109,
      1871, 1117, 1867, 1123, 1861,
      1129, 1847, 1151, 1831, 1153,
      1823, 1163, 1813, 1171, 1803
      };


   char *calloc(int, int);
   newkey = calloc(NEWBUF,1);  /* allocate for new key */
   keylength = sum = 0;
   while (n = (int) code[keylength])  /* key length and sum check for each key*/
     {
     sum += n;
     ++keylength;
     }

    length = prime[sum % NUMPRIMES];

    for (i=0;i<length;i++)
    {
    index = i % keylength;
    sum = code[index] + sum & 0xff;
    newkey[i] = code[index] ^ sum;
    }

    /* encrypt the file with the generated key */


    for (i=0;i<=num;i++)
       buffer[i] = buffer[i] ^ newkey[i % length];

    free(newkey);     /* release storage */

}   /* end cypher1.c */


