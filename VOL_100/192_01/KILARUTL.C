
/* -- DECLARE VARIABLES -- */
char fat[1050];
char directry[4096];
unsigned int offset, cluster, nxtclust, direntry, noncontig=0;
unsigned int i,j,cl,numin,tot,sector, bytedata, *pint, nsects, svcluster;
char *cfrom, drive, resp, chgflag;
char file[11], cmpflag, evenflag, col, foundflag;
char ds=0;

/* -- GLOBAL VARIABLES FOR SCREEN CONTROL -- */
char scr_cols;
char scr_rows;
char scr_mode;
char scr_page;
char scr_attr;

/* ---- EXECUTABLE CODE BEGINS HERE --- */

/* -- MAIN ROUTINE -- */
main()
{
 char c, s[10]; 
 int i,j,sectindx;

/* SET SCREEN TO WHITE CHARS ON BLUE */
 scr_setup();                   
 scr_setmode(3);
 scr_mode = 3; 
 scr_attr = 0x17;
 scr_clr();
 scr_rowcol(1,1);

/* PRINT WELCOME */
 printf("WELCOME to the Kilar Utilities.  These programs give you \n");
 printf("various operations on DS/DD (360K) diskettes with no subdirs.\n");

/* GET DRIVE DESIGNATION */
 printf("Enter letter of drive to be accessed/used: ");
 scanf("%c",&drive);

/* CONVERT DRIVE FROM ASCII TO BINARY */
 drive = drive - 0x41;          
 if (drive > 26)
   drive = drive - 0x20;
 printf(" \n");

/* GET FAT AND DIRECTORY FROM DISKETTE */
 printf("\nGetting FAT and directory from diskette ..... \n");
 getsect(fat,drive,1);
 getsect(&fat[512],drive,2);
 for (i = 0; i < 7; i++)
   getsect(&directry[(i*512)],drive,(i+5));

/* DETERMINE AMOUNT OF FRAGMENTATION,
     NON-CONTIGUOUS CLUSTERS IN FILES */ 
 sectindx = sectriz();          
 printf("\n\nFragmentation index for diskette is: %d\n",sectindx);
 printf("\n");
 if (sectindx == 0)
   printf("No file fragmentation on diskette.\n");
 else if (sectindx < 8)
   printf("Only some file fragmentation on diskette.\n");
 else if (sectindx < 15)
   printf("Moderate file fragmentation on diskette.\n");
 else if (sectindx < 22)
   printf("File fragmentation high, COPY *.* to new disk recommended.\n");
 else
   printf("Excessive file fragmentation, COPY *.* to new disk urged.\n");

 printf("\nHit any key to continue\n");
 getchar();

/* MAIN MENU LOOP */
again:
 scr_clr();
 scr_rowcol(1,1);
 printf("WELCOME to the Kilar Utilities.  These programs give you \n");
 printf("various operations on double sided, double density diskettes.\n");
 scr_rowcol(4,10);
 printf("Please make your selection:\n");
 scr_rowcol(6,13);
 printf("1 - Display/Modify data in specific diskette sector\n");
 scr_rowcol(8,13);
 printf("2 - Display use (file belongs to or free) of sector\n");
 scr_rowcol(10,13);
 printf("3 - Display map showing use of all clusters on disk\n");
 scr_rowcol(12,13);
 printf("4 - Display all sectors used by a specific file\n");
 scr_rowcol(14,13);
 printf("5 - Save a sector (and associated cluster) as a new file\n");
 scr_rowcol(16,13);
 printf("6 - Return to DOS\n");
 scr_rowcol(18,10);
 printf("Strike appropriate digit followed by ENTER key: \n"); 
 gets(s); 
 
 c = s[0]; 
 
 switch(c)
  {
   case '1': sect();        /* DISPLAY/MODIFY SECTOR */
             break;
   case '2': sectuse();     /* USE OF SPECIFIED SECTOR */
             break;
   case '3': clustmap();    /* CLUSTER MAP */
             break;
   case '4': fileallo();    /* SECTORS ALLOCATED TO SPECIFIED FILE */
             break;
   case '5': filesect();    /* SAVE SECTOR AS A FILE */
             break;
   case '6': exit();        /* RETURN TO DOS */
             break;
   default:  printf("Illegal option - try again \n");
  }
 printf("\n Type any key to continue \n");
 c = getchar();
 goto again;                /* WHEN DONE, RETURN TO MAIN MENU */
}
 


/* --- clustmap routine - displays a cluster map of diskette */
clustmap()
{

 nsects = chgflag = 0;
 printf("Cluster map routine. \n");
 printf("Displays use of all data clusters on diskette.\n");


 printf("Cluster map: \n");

 col = 1;

/* For each of the data cluster */
 for (cluster = 2; cluster < 356; cluster++) 
  { 
   printf(" %3d=",cluster);
   col += 5;
/* Check if cluster is part of file */
   if (next(cluster) == 0) 
    {
      printf("free         ");
      col += 12;
      checkcol();
      continue;
    }
 
/* Check directory if cluster is beginning of a file */
     foundflag = 0;
     for (i = 0; i < 112; i++)     /* check each directory entry */
     {
      direntry = 32 * i;
      pint = &directry[direntry + 26];   /* check first cluster value */
      if (cluster == *pint)
       {
        directry[direntry+11] = 0;       /* if checks, get file name */
        for (j = 0; j < 8; j++)
         putchar(directry[direntry+j]);
        putchar('.');
        for (j = 8; j < 11; j++)
          putchar(directry[direntry+j]);
        putchar(' ');
        col += 12;                        /* 12 chars displayed */
        checkcol();                       /* check for end of line */
        foundflag = 1;
        break;
       }
      if (foundflag)
        continue; 
  }
 if (foundflag)
   continue;


/* Check FAT for reference to cluster */
  svcluster = cluster;
  while (cl = prev(svcluster))
    svcluster = cl;

/* Check directory for reference to cluster */
  for (i = 0; i < 112; i++)
   {
     direntry = 32 * i;
     pint = &directry[direntry + 26]; 
     if (svcluster == *pint)
      {
       directry[direntry+11] = 0;
       for (j = 0; j < 8; j++)
         putchar(directry[direntry+j]);
       putchar('.');
       for (j = 8; j < 11; j++)
         putchar(directry[direntry+j]);
       putchar(' ');
       col += 12;
       checkcol();
       break;
      }
   }
 }
}


/* -- checkcol routine - checks for near end of line -- */
checkcol()    
{
 if (col > 66)
  {
   putchar('\n');
   col = 1;
  }
 return;
}

/* -- next - use FAT to get no. of the next cluster in file -- */
next(clust) 
unsigned int clust;
{
 unsigned int nxtclust, *pint, offset;
 char evenflag;

/* see PC-DOS Technical Reference Manual for explanation */
 if ((clust & 0xfffe) == clust)
   evenflag = 1;
 else
   evenflag = 0;
 nxtclust = clust + clust/2;
 pint = fat + nxtclust;
 offset = *pint;
 if (evenflag)
   offset = offset & 0x0fff;
 else
   offset = offset >> 4;
 return(offset);
}

/* -- prev - Check thru FAT to find entry pointing to current
       cluster.  That is the previous cluster. --- */
prev(clust)        
unsigned int clust;
{
 unsigned int nxtclust, *pint, offset;
 char evenflag;

for (i = 2; i < 356; i++)      /* check each FAT entry */
 {
  if ((i & 0xfffe) == i)
    evenflag = 1;
  else
    evenflag = 0;              /* determine next cluster */
  nxtclust = i + i/2;
  pint = fat + nxtclust;
  offset = *pint;
  if (evenflag)
    offset = offset & 0x0fff;
  else
    offset = offset >> 4;
  if (offset == clust)         /* if equal to current, have the previous */
    return(i);
  }
 return(0);
}
 

/* --- fileallo routine - displays sectors belonging to specified file */
 
fileallo()
{

 noncontig = 0;
 nsects = chgflag = 0;
 printf("File allocation routine.\n");
 printf("Displays the logical numbers of sectors the file occupies \n");
 printf("for double sided/double density diskette. \n \n");

 printf("Enter 8 letter basename of file in CAPS, spaces after end: ");
 for (i = 0; i < 8; i++)
   file[i] = getchar();
 printf("\nEnter 3 letter suffix of file in CAPS, spaces after end: ");
 for (i = 0; i < 3; i++)
   file[(i+8)] = getchar();


/* Loop to find file name in directory */
 i = cmpflag = 0;
 while (!cmpflag)
  {
   direntry = i * 32;           /* Find file entry in directory */
   if (!strncmp(&directry[direntry],file,11))
     cmpflag = 1;
   else if (i > 112)           /* If not found, return to DOS */
    {
     printf("File not found, terminating to DOS \n");
     return;
    }
   else
     i++;
  }

 pint = &directry[(direntry+26)];     /* Get first no. of first cluster */
 cluster = *pint;
 col = 1;
 printf("\n \nFile is on sectors: \n");
 prtsect(cluster);

next1:                                 /* Get next cluster number */
 if ((cluster & 0xfffe) == cluster)
   evenflag = 1;
 else
   evenflag = 0;
 nxtclust = cluster + cluster/2;
 pint = fat + nxtclust;
 offset = *pint;
 if (evenflag)
   offset = offset & 0x0fff;
 else
   offset = offset >> 4;
 if (offset > 0xff7)                /* If last sector, display totals */
  {
   printf("\nTotal of %d sectors. \n",nsects);
   printf("%d noncontiguous cluster breaks. \n",noncontig);
   return;
  }
 else
  {
   if (offset != (cluster+1))
     noncontig++;
   cluster = offset;
   prtsect(cluster);
  }
 goto next1;
}





/* --- prtsect - prints sectors for current cluster --- */

prtsect(clust)        /* Print sector numbers for current cluster */
int clust;
{
 int sect;

 sect = (clust - 2)*2 + 12;
 printf("%4d%4d",sect,(sect+1));
 col += 8;
 nsects += 2;
 if (col > 78)
  {
   printf(" \n");
   col = 1;
  }
 return;
}

char buffer[4096];
char leadflag;


/* --- sect - displays contents of specified sector --- */
sect()            
{

/* display heading an diskette information */
 chgflag = 0;
 printf("Sector display program. \n");
 printf(" \n");
 printf("For double sided/double density diskette: \n");
 printf("   Sector 0            = boot record. \n");
 printf("   Sectors 1 thru 4    = File Allocation Tables (FAT) \n");
 printf("   Sectors 5 thru 11   = Directory area \n");
 printf("   Sectors 12 thru 719 = File data \n \n");
 printf("Enter number of sector to be displayed: ");
 scanf("%d",&sector);
 getsect(buffer,drive,sector);

done:
 cfrom=0;
 while (cfrom < 512)
  {
   /* print the offset in hex */
   leadflag = 1;
   dtoh(cfrom);
   putchar(' ');

  /* print 16 bytes in hex */

   for (i=0; i < 16; i++)
    {
     putchar(' ');
     btoh(buffer[cfrom++]);
    }
   cfrom-=16;
   puts("   ");

   /* print the bytes in ascii */

   for (i=0; i < 16; i++)
    {
     putchar((buffer[cfrom] >= ' ' && buffer[cfrom] < 0x7f) 
       ? buffer[cfrom]: '.');
     cfrom++;
    }
   puts(" \n");
   if (cfrom == 256)
    {
     printf(" ** Hit any key to continue ** \n");
     getchar();
     printf(" \n");
    }
  }

 printf("\n \n Do you wish to change byte(s) of data (Y/N)? ");
 resp = getchar();
 if ((resp == 'Y') || (resp == 'y'))
  {
   chgflag = 1;
   printf("\n \n Enter number of (starting) byte in hex to be modified: ");
   scanf("%x",&cfrom);
   printf("\nEnter a hex number between 100 and FFFF to terminate entries\n"); 
change:
   printf("\n Enter new data in hex: ");
   scanf("%x",&bytedata);
   if (bytedata > 0xff)
     goto done;
   buffer[cfrom++] = bytedata;
   goto change;
  }

 if (chgflag)
  {
   printf(" \n Do you wish to update sector on disk with new data (Y/N)? ");
   resp = getchar();
   if ((resp == 'Y') || (resp == 'y'))
    {
     putsect(buffer,drive,sector);
     printf("\n Sector written back to disk. \n");
    }
  }
}    

/* display double byte in hex */

dtoh(db)
unsigned int db; 
{
 btoh(db>>8);
 btoh(db);
}

/* display byte in hex */

btoh(b)
char b; 
{
 ntoh(b>>4);
 ntoh(b);
}

/* display nibble in hex */

ntoh(n)
char n; 
{

 n &= 0x0f;
 if (n >= 10)
   putchar(n-10+'A');
 else
   putchar(n+'0');
 return;
}


/* --- sectuse - display use of specified sector --- */

sectuse()
{

 nsects = chgflag = 0;
 printf("Sector use routine.\n");
 printf("Displays whether a sector belongs to a file and the name of the\n");
 printf("file if it does. \n \n");

 printf("Enter logical number of sector: ");
 scanf("%d",&sector);
 cluster = ((sector - 12)/2) + 2;


/* Check if cluster is part of file */
 if (next(cluster) == 0) 
  {
    printf("Sector is not part of a file, it is free space. \n");
    return;
   }
 
/* Check directory if cluster is beginning of a file */
  for (i = 0; i < 112; i++)
   {
     direntry = 32 * i;
     pint = &directry[direntry + 26]; 
     if (cluster == *pint)
      {
       directry[direntry+11] = 0;
       printf("Sector is part of file ");
       for (j = 0; j < 8; j++)
         putchar(directry[direntry+j]);
       putchar('.');
       for (j = 8; j < 11; j++)
         putchar(directry[direntry+j]);
       putchar('\n');
       return;
      }
   }


/* Check FAT for reference to cluster */
  while (cl = prev(cluster))
    cluster = cl;

  for (i = 0; i < 112; i++)
   {
     direntry = 32 * i;
     pint = &directry[direntry + 26]; 
     if (cluster == *pint)
      {
       directry[direntry+11] = 0;
       printf("Sector is part of file ");
       for (j = 0; j < 8; j++)
         putchar(directry[direntry+j]);
       putchar('.');
       for (j = 8; j < 11; j++)
         putchar(directry[direntry+j]);
       putchar('\n');
       return;
      }
   }

}




/* ---- filesect - save a sector (and associated cluster) as a file */

filesect()
{

 nsects = chgflag = 0;
 printf("Sector save routine.\n");
 printf("Saves the cluster associated with sector as a file. \n");

 printf("Enter sector number to be saved as file: ");
 scanf("%d",&sector);
 printf(" \n");

/* determine number of associated cluster */
 cluster = ((sector - 12)/2) + 2;

/* file name will be SECTOR.FIL */
 strcpy(file,"SECTOR  FIL");

/* find a blank directory entry */
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

/* make sure cluster is not first cluster of a file */
 for (i = 0; i < 112; i++)
  {
   if (directry[32*i] != 0xe5)
    {
     pint = directry + 32*i + 26;
     if (cluster == *pint)
       inuse();
    }
  }

 strcpy(&directry[direntry],file);
 directry[direntry + 11]= 0x20;
 for (i = 12; i < 26; i++)
   directry[direntry + i] = 0;
 
/* point directory's cluster to this one */
 pint = &directry[(direntry+26)];
 *pint++ = cluster;
 *pint++ = 512;
 *pint = 0;
 
/* make sure cluster is not in use by checking its FAT entry,
    place end of file terminator in it */
 if ((cluster & 0xfffe) == cluster)
   evenflag = 1;
 else
   evenflag = 0;
 nxtclust = cluster + cluster/2;
 pint = fat + nxtclust;
 offset = *pint;
 if (evenflag)
  {
   if (offset & 0x0fff)
     inuse();
   offset &= 0xf000;
   offset |= 0x0fff;
  }
 else
  {
   if (offset & 0xfff0)
     inuse();
   offset &= 0x000f;
   offset |= 0xfff0;
  }
 *pint = offset;

 printf("\n \n Writing revised FAT and directory to diskette ..... \n");
 putsect(fat,drive,1);
 putsect(&fat[512],drive,2);
 putsect(fat,drive,3);
 putsect(&fat[512],drive,4);

 for (i = 0; i < 7; i++)
   putsect(&directry[(i*512)],drive,(i+5));
 printf("\n Cluster associated with sector %d saved as SECTOR.FIL.\n",sector);
}


inuse()
{
 printf("\nSector is already in use by a file.  Not made into new file.\n");
 return;
}


/* sectriz - calculate the fragmentation index of the diskette by
    scanning the FAT */
sectriz()
{

 noncontig = nsects = chgflag = 0;
 printf("Calculating fragmentation index.\n");


/* for each cluster */
 for (cluster = 2; cluster < 355; cluster++)
  {
   if ((cluster & 0xfffe) == cluster)
     evenflag = 1;
   else
     evenflag = 0;
   nxtclust = cluster + cluster/2;   /* get entry */
   pint = fat + nxtclust;
   offset = *pint;
   if (evenflag)
     offset = offset & 0x0fff;
   else
     offset = offset >> 4;

/* if entry is not bad cluster, end of file, or next cluster then
      must have non-contiguous segment */
   if ((offset < 0xff7) && (offset != (cluster+1)) && (offset))
     noncontig++;
  }
 return(noncontig);
}



