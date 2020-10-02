/*
HEADER:		;
TITLE:		Prompted delete with wildcards;
VERSION:	1.0;

DESCRIPTION:	Undo allows for multiple file names to be specified on the
		same command line along with wildcard specifiers if desired.
		Undo will display the directory and the file name and wait
		for the user to respond before deleting the file.  Undo will
		continue until all matching files have been prompted for.
		Also, the file(s) entered on the command line do not have to
		be in the same directory or even on the same drive. Undo will
		find any matches as long as the complete path is specified.

KEYWORDS:	Dos utilities;
SYSTEM:		MSDOS;
FILENAME:	UNDO;
WARNINGS:	None;

SEE-ALSO:	FBYTE;
AUTHORS:	Dr. Ronald J. Terry;
COMPILERS:	Turbo C;
*/
/***************************************************************************/
/*  Most of the code is self explanatory.  This function will examine      */
/*  multiple inputs from the command line including wildcards and prompt   */
/*  the user for a yes (y) or no (n) response before deleting the file(s). */
/***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <dir.h>
#define BKSLASH 92
int getdrv_num(char dltr);

void main(int argc, char *argv[])  /* Get command line arguments */
{
   struct ffblk  ffblk;
   char buf[80],cmd_arg[80],driveno[80];
   char response,drvltr,errmsg[80];
   int i,len,len1,buflen,done,drive,cdir;
   buflen = 80;       /* Maximum number of characters on command line */
   getcwd(buf,buflen);  /* save current directory */
   i = 1;
   while(i<argc)
   {
   cdir = 0;
   strncpy(driveno,buf,2);
   driveno[2] = NULL;
   strcpy(cmd_arg,argv[i]);
   len = strlen(argv[i]);
   cmd_arg[len] = NULL;
     if(cmd_arg[1]==':')
     {
     cmd_arg[0] = toupper((int)(cmd_arg[0]));   /* drive specification */
     strncpy(driveno,cmd_arg,2);
     driveno[2] = NULL;
     }
     else
     {
     cdir = 1;  /* true if file(s) in current directory otherwise false */
     }
     if(cmd_arg[2]!=BKSLASH)
     {
       if(cmd_arg[0]==BKSLASH)        /* get path */
       {
       strcpy(cmd_arg,cmd_arg+1);
       }
     driveno[2] = NULL;
     strcat(driveno,"\\");
     driveno[3] = NULL;
      if(!cdir)
      {
      strcpy(cmd_arg,cmd_arg+2);
      }
     strcpy(cmd_arg,(strcat(driveno,cmd_arg)));
     }
   strcpy(driveno,strrchr(cmd_arg+3,BKSLASH));
   len1=strlen(driveno);
   if(cdir&&!len1)
   {
   strcpy(cmd_arg,buf);
   }
   else
   {
     if(!len1)
     {
       if(cmd_arg[0]==buf[0])
       {
       strcpy(cmd_arg,buf);
       }
     else
     {
     cmd_arg[3] = NULL;
     }}
   len = strlen(cmd_arg);
   len = len - len1;
   cmd_arg[len] = NULL;
   }
   done = findfirst(argv[i],&ffblk,0);
   putchar(10);                  /* find first file and any other matches */
   drvltr = cmd_arg[0];
   drive = getdrv_num(drvltr);
   setdisk(drive);
   chdir(cmd_arg);
   printf("  directory of %s\n",cmd_arg);
     if(done)
     {
     strcpy(errmsg,argv[i]);
     putchar(7);
     printf("  ");
     perror(errmsg);        /* indicate if no matches were found */
     puts("  By courtesy of RT: <Press any key>");
       for(;;)
       {
	 if(getch())
	 {
	 break;
	 }
       }
     }
   while(!done)
   {
   printf("  %s",ffblk.ff_name);
     for(;;)
     {
     response = tolower(getch());
       if(response=='y' || response=='n')
       {
	 if(response=='y')
	 {
	 remove(ffblk.ff_name);   /* delete file */
	 printf("  deleted");
	 }
       putchar(10);
       break;
       }
     }
   done = findnext(&ffblk);
   }
   i++;
   drvltr = buf[0];
   drive  = getdrv_num(drvltr);
   setdisk(drive);
   chdir(buf);
   }
}
/********* function GETDRV_NUM is used to convert the drive letter into an
		       integer number for use in SETDISK. ****************/

int getdrv_num(char drvltr)
{
    int drive;
    switch(drvltr)
    {
    case 'A': drive = 0;
    break;
    case 'B': drive = 1;
    break;
    default : drive = 2;
    }
    return(drive);
}
