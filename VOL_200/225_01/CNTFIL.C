/*-----------------------------------------------------------------*/
/*
         Programme:     CNTFIL.C
         ---------
         Purpose:       Counts all the active files in all the User
         -------        areas and displays a table of the count.

         Compiler:      BDS C V 1.50
         --------
         Written:       12/12/85
         -------
         Amended:       15/11/86
         -------
         Version:       1.1
         -------

         Copyright 1986 - Cogar Computer Services Pty. Ltd.        */
/*-----------------------------------------------------------------*/
#include <bdscio.h>
#include <pec.h>
/*-----------------------------------------------------------------*/
#define	VERSION "1.1\0"
#define NAME    "CNTFIL\0"
/*-----------------------------------------------------------------*/

main(argc, argv)	/* For Command Line processing             */
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	int i, j, FD;
	char c;
	int CPM;	/* To check the CP/M Version number        */
	char DRIVE;	/* The active drive                        */
	char OLD_DRIVE;	/* The drive at start of programme         */
	char OLD_USER;	/* The User No. at start of programme      */
	char USER;	/* The User No. for this programme         */
	int ALL, HID;
	int DIRECTORY, sector, DIR, SYS;
	struct dpb *THIS;	/* The disk parameter block        */
	char DMA_BUF[128];	/* For information flow            */
	int RECORDS;		/* In the Directory                */
	char USERS[32];		/* To store the counts             */
/*-----------------------------------------------------------------*/
	pec_clear();	/* Universal routine                       */
	printf("%s - Version %s\n",NAME, VERSION);
	printf("Copyright 1986 - Cogar Computer Services Pty.Ltd.\n\n");
/*-----------------------------------------------------------------*/
	printf("This programme will count ALL files in ALL user areas\n");
	printf("and display a table of the count split up into user\n");
	printf("areas and into visible (DIR) and hidden (SYS) files.\n");
	line();
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
/*  The CP/M Version is OK, so save the starting User No. and the
    starting Drive No. in case either is changed later.            */
/*-----------------------------------------------------------------*/
	OLD_USER = user_id(0xff);
	OLD_DRIVE = get_default() + 0x41;

/*-----------------------------------------------------------------*/
/*  Now check the Command Line to see if a Drive Code was entered.
    Other checks can also be used, as required but then it will be
    necessary to change this coding.                               */
/*-----------------------------------------------------------------*/
	if(argc != 2)
	{
		printf("Please enter the DRIVE to check...A, B, C...\n");
		DRIVE = toupper(getchar());
	}
	else DRIVE = toupper(argv[1][0]);
	lines(2);
/*-----------------------------------------------------------------*/
/*  Check that the selected drive is available/on-line.   If not
    then terminate the programme.   You may need to add a message
    about what is going on if your version of CP/M doesn't do
    this automatically, as mine does.                              */
/*-----------------------------------------------------------------*/
	if(select_dsk(DRIVE) != 0)
		exit();
/*-----------------------------------------------------------------*/
	for(i = 0; i < 32; i++)
		USERS[i] = 0;	/* Initialise the counters         */
/*-----------------------------------------------------------------*/
/*  Get the values needed in the programme.                        */
/*-----------------------------------------------------------------*/

	THIS = dpb_adr();
/*	RECORDS = (THIS->DRM + 1) >> 2;                            */
	RECORDS = THIS->SPT;
	DIRECTORY = THIS->OFF;		/* First Directory track   */

/*-----------------------------------------------------------------*/
/*  Now read the directory sectors into the DMA buffer.            */
/*  And count the active entries.                                  */
/*-----------------------------------------------------------------*/
	pec_clear();
	header();
	printf("Summary of files in directory of Drive - %c.\n", DRIVE);

	set_trk(DIRECTORY);		/* Starting values         */
	sector = ALL = HID = 0;
	setdma(DMA_BUF);

for(i = 0; i < RECORDS; i++)
{
	if(i == THIS->SPT)
	{
		DIRECTORY++;
		set_trk(DIRECTORY);
		sector = 0;
	}
	set_sec(sector);
	if(read_sec() == 1)
	{
		printf("Error reading directory");
		exit();
	}

	for(j = 0; j < 4; j++)
	{
		USER = DMA_BUF[j*32];

	if(USER != 0xe5 && DMA_BUF[j*32 + 12] == 0	&& DMA_BUF[j*32 + 10] < 128)
	{
			USERS[USER*2]++;
			ALL++;
	}
	else if(USER != 0xe5 && DMA_BUF[j*32 + 12] == 0
		&& DMA_BUF[j*32 + 10] > 128)
	{
				USERS[USER*2 + 1]++;
				HID++;
	}
	}
	sector++;
}
	printf("User No.\tDir. Files\tHidden Files\tTotals\n");
	for(i = 0; i < 16; i++)
	{
printf("%2d\t\t%3d\t\t%3d\t\t%3d\n", i,USERS[i*2],USERS[i*2 + 1], USERS[i*2] + USERS[i*2 + 1]);
	}
	lines(2);
	printf("TOTALS\t\t %d\t\t  %d\t\t %d\n", ALL, HID, ALL + HID);
/*-----------------------------------------------------------------*/
/*  Before finishing return to the original User Area and Drive No.*/
/*-----------------------------------------------------------------*/
	user_id(OLD_USER);
	if(select_dsk(OLD_DRIVE) != 0)
	{
		printf("\nUnable to return to starting drive.");
		exit();
	}
	printf("\nDo you want to run another utility - Y/N.");
if((FD = open("UTIL.COM", 0)) != 0 && (c = toupper(getchar())) == 'Y')
{
	close(FD);
	exec("UTIL");
}

}
/*-----------------------------------------------------------------*/
header()
{
	printf("COUNTFILE - Version 2.0\n");
	printf("Copyright 1986 - Cogar Computer Services Pty. Ltd.\n\n");
}
/*-----------------------------------------------------------------*/
---------