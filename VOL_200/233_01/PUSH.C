/* push.c -- saves current working directory */

#include <string.h>
#include <direct.h>
#include <stdio.h>

main()
{
   FILE *fi;
   char curdir[67];
   int i, ch;

   /* "cdstack.dat is the name chosen to store the CWD -- you can use any name */

   fi = fopen("c:\\cdstack.dat","a"); /* Mine is stored in root directory */
                                      /* but you can use any directory*/

   getcwd(curdir,67);   /* get current directory and store in "curdir" */

      /* write current directory to file */
   for ( i = 0; (i <= strlen(curdir)) && ((ch = fputc(curdir[i], fi)) != EOF); i++)
       ;

   /*pad file with ASCII null characters '\0' to total 67 */
   for (i = (strlen(curdir) + 1); (i < 67) && ((ch = fputc('\0', fi)) != EOF); i++)
       ;
    fclose (fi);
 }
