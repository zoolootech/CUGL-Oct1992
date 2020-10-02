/* DECLARE VARIABLES */
char fat[1050];
char directry[4096];
unsigned int offset, cluster, nxtclust, direntry, noncontig=0;
unsigned int i,j,cl,numin,tot,sector, bytedata, *pint, nsects, svcluster;
char *cfrom, drive, resp, chgflag;
char file[11], cmpflag, evenflag, col, foundflag;
char ds=0;
char vol[35];
char volget[20];


/* EXECUTABLE CODE STARTS HERE */
  
main()
{
 char c, s[10]; 
 int i,j,sectindx;
 
/* print heading and prompt for drive */
 printf("Modify DS/DD diskette to give it a volume name - by Joe Kilar.\n");
 printf("Enter letter of drive to be accessed/used: ");
 scanf("%c",&drive);

/* convert drive from ASCII to binary */
 drive = drive - 0x41;
 if (drive > 26)
   drive = drive - 0x20;

/* prompt for and get volume name */
 printf("Enter volume name desired (11 chars max): ");
 gets(volget);

 printf("\nGetting directory from diskette ..... \n");
 for (i = 0; i < 7; i++)
   getsect(&directry[(i*512)],drive,(i+5));

 volume();
}

 
/* --- volume - do the actual diskette operation to create volume name --- */

volume()
{
 int i;
 char *p;

 nsects = chgflag = 0;
 


/* SET UP VOLUME ENTRY */
 for (i = 0; i < 11; i++)     /* initialize name to spaces */
   vol[i] = ' ';

 for (i = 11; i < 32; i++)     /* place zeroes in remainder */
   vol[i] = 0;

 vol[11] = 8;                  /* set up codes DOS uses for volumes */
 vol[22] = 0x65;
 vol[24] = 0x21;

/* TRANSFER VOLUME NAME TO VOLUME ENTRY */
 i = 0;
 p = volget;
 while (*p)
   vol[i++] = *p++;


/* SEARCH FOR EMPTY DIRECTORY ENTRY TO MOVE FIRST ONE TO */
 i = cmpflag = 0;
 while (!cmpflag)
  {
   direntry = i * 32;
   if (directry[direntry] == 0)
     cmpflag = 1;
   else if (i > 112)
    {
     printf("No directory space left, terminating to DOS \n");
     return;
    }
   else
     i++;
  }

/* MOVE FIRST DIRECTORY ENTRY TO THIS BLANK ONE */
 for (i = 0; i < 32; i++)
   directry[(direntry + i)] = directry[i];

/* MOVE VOLUME NAME TO FIRST ENTRY */
 for (i = 0; i < 32; i++)
   directry[i] = vol[i];

 printf("\n Writing revised directory with volume name to diskette ..... \n");

 for (i = 0; i < 7; i++)
   putsect(&directry[(i*512)],drive,(i+5));
}


