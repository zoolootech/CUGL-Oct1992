                                         /* Chapter 7 - Program 2 */
#include "stdio.h"
#include "string.h"

void main()
{
char name1[12],name2[12],mixed[25];
char title[20];

   strcpy(name1,"Rosalinda");
   strcpy(name2,"Zeke");
   strcpy(title,"This is the title.");

   printf("     %s\n\n",title);
   printf("Name 1 is %s\n",name1);
   printf("Name 2 is %s\n",name2);

   if(strcmp(name1,name2)>0)  /* returns 1 if name1 > name2 */
      strcpy(mixed,name1);
   else
      strcpy(mixed,name2);

   printf("The biggest name alpabetically is %s\n",mixed);

   strcpy(mixed,name1);
   strcat(mixed,"  ");
   strcat(mixed,name2);
   printf("Both names are %s\n",mixed);
}



/* Result of execution

     This is the title.

Name1 is Rosalinda
Name2 is Zeke
The biggest name alphabetically is Zeke
Both names are Rosalinda Zeke

*/
