/*-----------------------------------------------------------------*/
/*       STANDARD HEADER FOR BDS C PROGRAMMES
         ------------------------------------

         Programme:     NOBAD.C
         ---------
         Purpose:       Finds the bad sectors on a disk and then
         -------        puts these into the file  COLLECT.BAD
                        which is created in User Area 15.

                        If the bad sector(s) is in the Reserved
                        Tracks, or the Directory, the disk is not
                        able to be used with safety.   A message
                        to this effect is printed and the programme
                        terminates without checking the data tracks.

         Written:       10/09/86
         -------
         Amended:       14/11/86
         -------
         Version:       1.2
         -------

         Copyright 1986 - Cogar Computer Services Pty. Ltd.        */
/*-----------------------------------------------------------------*/
#include <bdscio.h>
#include <pec.h>
/*-----------------------------------------------------------------*/
#define	VERSION "1.2\0"
#define NAME    "NOBAD\0"
/*-----------------------------------------------------------------*/

main(argc, argv)	/* For Command Line processing             */
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	int i, j,RECORDS, COUNT, FD;
	char c;
	char MST_SIG, LST_SIG;	/* Bytes in an integer             */
	int CPM;	/* To check the CP/M Version number        */
	char DRIVE;	/* The active drive                        */
	char OLD_DRIVE;	/* The starting drive No.                  */
	char OLD_USER;	/* The User No. at start of programme      */
	char USER;	/* The User No. for this programme         */
	struct dpb *THIS;
	int DIRECTORY;	/* The directory track No.                 */
	int TRACK, SECTOR, PHYS_SEC;
	int BAD_BLK[16];	/* Maximum allowed                 */
	int BLK_SIZE;		/* In records                      */
	int BAD_ONE;
	int LAST_TRACK, LAST_SECTOR;
	char dma_buf[128];
	char MY_FCB[36];	/* The file control block          */
	char **skew_table;	/* Pointer to a pointer decl.      */
/*-----------------------------------------------------------------*/
	pec_clear();	/* Universal routine                       */
	printf("%s - Version %s\n",NAME, VERSION);
	printf("Copyright 1986 - Cogar Computer Services Pty.Ltd.\n\n");
	printf("-----------------------------------------------------\n");
	printf("This programme checks the integrity of the disk one\n");
	printf("sector at a time....including both the Reserved tracks\n");
	printf("and the Directory track.    If it finds that either the\n");
	printf("Reserved tracks or the Directory has a faulty sector,\n");
	printf("then it reports this to the User and terminates the\n");
	printf("programme as this disk shouldn't be used.\n");
	printf("If it finds any other bad sectors then it collects\n");
	printf("these into the file ==> COLLECT.BAD which is written\n");
	printf("to the Directory in user area 15.\n");
	printf("-----------------------------------------------------\n");
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
    Drive No. in case either is changed later.                     */
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
	if(!(skew_table = seldsk(DRIVE)))
		exit();
	if(select_dsk(DRIVE) != 0)
		exit();
/*-----------------------------------------------------------------*/
/*  Get the disk parameters needed in programme.                   */
/*-----------------------------------------------------------------*/
	THIS = dpb_adr();	/* Point to disk parameter block   */

	DIRECTORY = THIS->OFF;

	LAST_SECTOR = THIS->DSM;	/* Starting value          */
	for(i = 0; i < THIS->BSH; i++)
	{
		LAST_SECTOR = LAST_SECTOR + LAST_SECTOR;
	}
	LAST_TRACK = (LAST_SECTOR/THIS->SPT) + THIS->OFF;
	BLK_SIZE = 1;
	for(i = 0; i < THIS->BSH; i++)
	{
		BLK_SIZE = BLK_SIZE*2;
	}
/*-----------------------------------------------------------------*/
/*  See if disk has been checked before, if so erase old file.     */
/*-----------------------------------------------------------------*/
	user_id(15);
	if(open("COLLECT.BAD", 0) != -1)
	{
		unlink("COLLECT.BAD");
		printf("Erasing old COLLECT.BAD\n\n");
	}
	user_id(OLD_USER);
/*-----------------------------------------------------------------*/
/*  First check the Reserved Tracks.                               */
/*-----------------------------------------------------------------*/
	set_dma(&dma_buf[0]);
	printf("\tTesting Reserved Tracks.\n");
	printf("\t-----------------------\n\n");

	for(i = 0; i < DIRECTORY; i++)
	{
		set_trk(i);
		if(i == 0)
			RECORDS = 26;
		else RECORDS = THIS->SPT;
		for(j = 0; j < RECORDS; j++)
		{
			set_sec(j);
			if(read_sec() != 0)
			{
				if(write_sec() != 0)
				{
	printf("Bad sector in Reserved Track - %d\n", i);
	printf("This makes the disc VERY suspect.");
	exit();
				}
			}
		}
	}
	printf("The Reserved Tracks are OK.\n\n");
/*-----------------------------------------------------------------*/
/*  Now check the Directory Track.                                 */
/*-----------------------------------------------------------------*/
	printf("\tChecking Directory Track.\n");
	printf("\t------------------------\n\n");
	set_trk(DIRECTORY);
	for(j = 0; j < RECORDS; j++)
	{
		PHYS_SEC = biosh(16, j, *skew_table);
		set_sec(PHYS_SEC);
		if(read_sec() != 0)
		{
			if(write_sec() != 0)
			{
	printf("Bad sector in Directory Track - %d\n", i);
	printf("This makes the disc VERY suspect.");
	exit();
			}
		}
	}
	printf("The Directory Track is OK.\n\n");
/*-----------------------------------------------------------------*/
/*  And finally, check the data tracks.                            */
/*-----------------------------------------------------------------*/
	COUNT = 0;
	RECORDS = THIS->SPT;
	printf("\tChecking Data Tracks.\n");
	printf("\t--------------------\n\n");

	for(i = DIRECTORY + 1; i < LAST_TRACK; i++)
	{
		set_trk(i);
		for(j = 0; j < RECORDS; j++)
		{
			PHYS_SEC = biosh(16, j, *skew_table);
			set_sec(PHYS_SEC);

			if(read_sec() != 0)
			{
				if(write_sec() != 0)
				{
	BAD_ONE = ((i - DIRECTORY)*RECORDS + j)/BLK_SIZE;
	BAD_BLK[COUNT] = BAD_ONE;
printf("\tBad sector in Track - %d  Sector - %d  Block - %d\n", i, j, BAD_ONE);
	if(COUNT != 0 && BAD_BLK[COUNT] != BAD_BLK[COUNT - 1])
		COUNT++;
	if(COUNT == 0)
		COUNT++;
				}
			}
		}
	if(i%10 == 0)
		printf("Finished reading Track - %d\n", i);
	}
	if(COUNT != 0)
		printf("%d  bad blocks found.\n", COUNT);
	else printf("No bad blocks found...disk is OK.");
/*-----------------------------------------------------------------*/
/*  If any bad blocks found then write them to file  COLLECT.BAD   */
/*-----------------------------------------------------------------*/
	if(COUNT > 0)
	{
		setmem(&MY_FCB[0], 36, 0); /* Zero the buffers     */
		setmem(&dma_buf[0], 128, 0);

		MY_FCB[0] = 15;	/* The User No.                    */
		strcpy(&MY_FCB[1],"COLLECT BAD");
		MY_FCB[15] = COUNT*BLK_SIZE;	/* Records         */

		j = 16;		/* Starting value                  */
		if((THIS->DSM >> 8) == 0)
		{
		if(COUNT > 16)
		{
		printf("Too many bad blocks to make disk safe to use.");
		user_id(OLD_USER);
		exit();
		}
		for(i = 0; i < COUNT; i++)
		{
			MY_FCB[j] = (LST_SIG = BAD_BLK[i] & 0xff);
			j++;
		}
		}
		else if((THIS->DSM >> 8) != 0)
		{
		if(COUNT > 8)
		{
		printf("Too many bad blocks to make disk safe to use.");
		user_id(OLD_USER);
		exit();
		}
		for(i = 0; i < COUNT; i++)
		{
			MY_FCB[j] = (LST_SIG = BAD_BLK[i] & 0xff);
			MY_FCB[j + 1] = (MST_SIG = (BAD_BLK[i] >> 8) & 0xff);
			j = j + 2;
		}
		}
/*-----------------------------------------------------------------*/
/*  We now have a formulated FCB entry for the file COLLECT.BAD so
    all that has to be done is to write this entry into the disk
    directory.                                                     */
/*-----------------------------------------------------------------*/

	set_trk(DIRECTORY);
	set_dma(&dma_buf[0]);

	for(i = 0; i < RECORDS; i++)
	{
		PHYS_SEC = biosh(16, i, *skew_table);
		set_sec(PHYS_SEC);
		read_sec();
	for(j = 0; j < 128; j = j + 32)
	{
		if(dma_buf[j] == 0xe5)	/* Found a vacant slot     */
		{
/*-----------------------------------------------------------------*/
/*  Copy COLLECT.BAD into the directory entry slot.                */
/*-----------------------------------------------------------------*/
		movmem(&MY_FCB[0], &dma_buf[j], 32);
		write_sec();
		user_id(OLD_USER);
		printf("Have created file  COLLECT.BAD  to hold ");
		printf("the bad blocks.");
		exit();
		}
	}
	}
	}
/*-----------------------------------------------------------------*/
/*  Before closing, make sure the User No. and Drive No. return
    to original settings.                                          */
/*-----------------------------------------------------------------*/

	user_id(OLD_USER);
	if(select_dsk(OLD_DRIVE) != 0)
	{
		printf("\nUnable to return to original drive.");
		exit();
	}
	if((FD = open("UTIL.COM", 0)) != -1)
	{
		close(FD);
		exec("UTIL");
	}
	else printf("\nUnable to find  UTIL.COM.   Returning to CP/M.");

}
/*-----------------------------------------------------------------*/< COUNT; i++)
		{
			MY_FCB[j] = (LST_SIG = BAD_BLK[i] & 0xff);
			j++;
		}
		}
		else if((THIS->DSM >> 8) != 0)
		{
	