/* DECLARE VARIABLES */
char infile1[30];
char infile2[30];
int file1, file2;
char a,b;
  
/* --- EXECUTABLE CODE STARTS HERE --- */
main()
{
 char errflg,r;
 int i, n = 0;
 unsigned int lineno;
 
/* display header */
 lineno = 1;
 errflg = 0;
 printf("File compare routine.  Version 1.0 by Joe Kilar \n");
 printf("Any byte counts and values are shown in hex.\n \n");

/* ignore spacing? */
 printf("Do you wish to ignore spacing? (Y/N) N is default:");
 r = getchar();
 if ((r=='y') || (r=='Y'))
   r = 1;
 else
   r = 0;
 putchar('\n');

/* prompt for and open files to be compared */
 puts("Enter name of first file: ");
 gets(infile1);
 puts("Enter name of second file: ");
 gets(infile2);
 file1 = fopen(infile1,"r");
 if (!file1)
  {
   printf("Unable to open file: %s \n",infile1);
   exit();
  }
 file2 = fopen(infile2,"r");
 if (!file2)
  {
   printf("Unable to open file: %s \n",infile2);
   exit();
  }

/* loop while compare continues */
 while (errflg == 0)
  {
getnew1:
   if ((i = fgetc(file1)) != -1)     /* if not end of file 1 */
    {
     a = i;
     if (a == 13)       /* if carriage return, inc line no. count */
       lineno++;
     n++;               /* increment byte count */
     if (r && isspace(a))
       goto getnew1;
getnew2:
     if ((i = fgetc(file2)) != -1)     /* if not end of file 2 */
      {
       b = i;
       if (r && isspace(b))
         goto getnew2;
       if (a != b)       /* if bytes don't compare */
        {
         printf("Files fail compare at byte number: %x \n",n);
         printf("First file's byte is: %x  \n",a);
         printf("Second file's byte is: %x \n",b); 
         printf("If ASCII files, failure is at line %d (decimal).\n",lineno);
         exit();
        }
      }
     else    /* end of file 2, so first is longer */
      {
       printf("First file is longer. \n");
       exit();
      }
    }
   else
    {
     if ((b = fgetc(file2)) != 0xff)
      {
       printf("Second file is longer. \n");
       exit();
      }
     else
      {
       printf("Files compare OK. \n");
       printf("%x number of bytes compared OK. \n"); 
       exit();
      }
    }
  }
}
 
 
 

