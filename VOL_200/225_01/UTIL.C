/*-----------------------------------------------------------------*/
/*
         Programme:     UTIL.C
         ---------
         Purpose:       A Menu for the various C utility programmes
         -------        written by Phil E. Cogar to operate under
                        CP/M 2.x or CP/M 3.0.
                        Note that you need to have the 'COM' files
                        on the same disk as this file to be able to
                        run it.   The programme checks whether you
                        have done this and tells you if all is not
                        well.
                        The COM files required are -

                        CHKDSK         CLEANUP        CNTFIL
                        DIR2           NOBAD          RESTORE
                        SECREAD        SHOWDIR        USER2

         Compiler:      BDS C V 1.50
         --------
         Written:       14/05/86
         -------
         Amended:       12/11/86
         -------
         Version:       2.0
         -------

         Copyright 1986 - Cogar Computer Services Pty. Ltd.        */
/*-----------------------------------------------------------------*/
#include <bdscio.h>
#include <pec.h>
/*-----------------------------------------------------------------*/
#define	VERSION "2.0\0"
#define NAME    "UTIL\0"
/*-----------------------------------------------------------------*/

main(argc, argv)	/* For Command Line processing             */
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	int i, j, FD, CH;
	char c;
	int CPM;	/* To check the CP/M Version number        */
	char DRIVE;	/* The active drive                        */
/*-----------------------------------------------------------------*/
/*  Put Heading.                                                   */
/*-----------------------------------------------------------------*/
	pec_clear();	/* Universal routine                       */
	printf("%s - Version %s\n",NAME, VERSION);
	printf("Copyright 1986 - Cogar Computer Services Pty.Ltd.\n\n");
/*-----------------------------------------------------------------*/
/*  First check the CP/M Version in use.   If it is less than
    Version 2.0 then inform the user and terminate programme.      */
/*-----------------------------------------------------------------*/

	CPM = get_cpm();	/* Obtain the CP/M version and No. */

	i = (CPM & 0xff) - 0x20; /* Mask off the MP/M bit          */

	if(i < 0)		/* Must be less than V 2.0         */
	{
	printf("This programme requires at least V 2.x of CP/M.\n");
		printf("Sorry but it won't run for you.\n");
		exit();
	}
/*-----------------------------------------------------------------*/
/*  Put up the menu.                                               */
/*-----------------------------------------------------------------*/
printf("This programme allows you to access a number of utility programmes\n");
printf("for checking disks,cleaning up the directory, repairing bad sectors\n");
printf("of disks, and so on.   Select what you want to do from the Menu - ");
lines(2);
printf("ENTRY      FUNCTION\n");
printf("-----      --------\n\n");
printf(" 1         Print DISK STATISTICS and Data Map.\n");
printf(" 2         CLEAN UP Directory Entries.\n");
printf(" 3         Display COUNT of FILES in all User Areas.\n");
printf(" 4         COPY Disk DIRECTORY onto last track of disk.\n");
printf(" 5         FIND BAD SECTORS and lock them out of use.\n");
printf(" 6         READ any SECTOR of a disk in Hex and ASCII.\n");
printf(" 7         ARCHIVE files not previously copied.\n");
printf(" 8         TRANSFER FILES between User Areas.\n");
printf(" 9         RESTORE erased files...sometimes.\n");
printf(" L         Make a HARD COPY of a file on LIST device.\n");
printf(" X         Return to CP/M.\n\n");
printf("Enter CODE for required function  - ");

/*-----------------------------------------------------------------*/
/*  Now execute the nominated function.                            */
/*-----------------------------------------------------------------*/
	if((c = getchar()) == '1')
	{
	if((FD = open("CHKDSK.COM",0)) == -1)
	{
		printf("\nYou need  CHKDSK.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("CHKDSK");
	}
/*-----------------------------------------------------------------*/
	else if(c == '2')
	{
	if((FD = open("CLEANUP.COM", 0)) == -1)
	{
		printf("\nYou need  CLEANUP.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("CLEANUP");
	}
/*-----------------------------------------------------------------*/
	else if(c == '3')
	{
	if((FD = open("CNTFIL.COM", 0)) == -1)
	{
		printf("\nYou need  CNTFIL.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("CNTFIL");
	}
/*-----------------------------------------------------------------*/
	else if(c == '4')
	{
	if((FD = open("DIR2.COM", 0)) == -1)
	{
		printf("\nYou need  DIR2.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("DIR2");
	}
/*-----------------------------------------------------------------*/
	else if(c == '5')
	{
	if((FD = open("NOBAD.COM", 0)) == -1)
	{
		printf("\nYou need  NOBAD.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("NOBAD");
	}
/*-----------------------------------------------------------------*/
	else if(c == '6')
	{
	if((FD = open("SECREAD.COM", 0)) == -1)
	{
		printf("\nYou need  SECREAD.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("SECREAD");
	}
/*-----------------------------------------------------------------*/
	else if(c == '7')
	{
	if((FD = open("ARCHIVE.COM", 0)) == -1)
	{
		printf("\nYou need  ARCHIVE.COM on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("ARCHIVE");
	}
/*-----------------------------------------------------------------*/
	else if(c == '8')
	{
	if((FD = open("USER2.COM", 0)) == -1)
	{
		printf("\nYou need  USER2.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("USER2");
	}
/*-----------------------------------------------------------------*/
	else if(c == '9')
	{
	if((FD = open("RESTORE.COM", 0)) == -1)
	{
		printf("\nYou need  RESTORE.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("RESTORE");
	}
/*-----------------------------------------------------------------*/
	else if(toupper(c) == 'L')
	{
	if((FD = open("LISTFILE.COM", 0)) == -1)
	{
		printf("\nYou need  LISTFILE.COM  on this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	exec("LISTFILE");
	}
/*-----------------------------------------------------------------*/


	else if(toupper(c) != 'X')
	{
		lines(2);
		printf("\nUnknown response....return to system.");
		exit();
	}
}
/*-----------------------------------------------------------------*/ this disk to run ");
		printf("the programme.");
		exit();
	}
	close(FD);
	ex