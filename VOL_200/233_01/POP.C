/* pop.c -- changes to last directory stored in file cdstack.dat (LIFO) */

#include <string.h>
#include <direct.h>
#include <stdio.h>

main()
{
   FILE *fi;
   char c[67];
   int result;
   long length, pos;

   pos = -(sizeof(c));   /*set to size of 67 chars */
   fi = fopen("c:\\cdstack.dat", "r+"); /*open for reading and writing */
      /* position filepointer 67 chars from end of file  */
   result = fseek(fi,pos,SEEK_END);
   fscanf(fi,"%s",c);   /* read the stored path innto "c" */
   chdir(c);            /* change to last directory stored with PUSH */
   length = filelength(fileno(fi)); /* get size of file in bytes */
        /* truncate file by cutting 67 bytes from end */
   result = chsize(fileno(fi), (length - (sizeof(c))));
   length = filelength(fileno(fi));  /*get new length of cdstack.dat */
   fclose (fi);
   if (length < 67)    /* if  cdstack.dat is now less than 67 bytes */
      result = remove("c:\\cdstack.dat");     /* erase it*/
}