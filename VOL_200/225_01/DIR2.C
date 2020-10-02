
/*-----------------------------------------------------------------*/
/*       STANDARD HEADER FOR BDS C PROGRAMMES
         ------------------------------------

         Programme:     DIR2.C
         ---------
         Purpose:       This programme will delete all inactive
         -------        entries from the directory and then re-write
                        the directory in (almost) alphabetical order
                        onto the last available track of the nominated
                        disk.   It also creates the file "DIRECT.ORY"
                        under user area 15.

         Written:       02/01/86
         -------
         Amended:       08/11/86
         -------
         Version:       1.2
         -------

         Copyright 1986 - Cogar Computer Services Pty. Ltd.        */
/*-----------------------------------------------------------------*/
#include <bdscio.h>
#include <pec.h>
/*-----------------------------------------------------------------*/
#define	VERSION "1.2\0"
#define NAME    "DIR2\0"
/*-----------------------------------------------------------------*/

main(argc, argv)	/* For Command Line processing             */
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	int i, j, k, l, BLOCK1, FD;
	char c, DATA_MAP;
	int CPM;	/* To check the CP/M Version number        */
	char DRIVE;	/* The active drive                        */
	char OLD_DRIVE;	/* The starting drive No.                  */
	char OLD_USER;	/* The User No. at start of programme      */
	char USER;	/* The User No. for this programme         */
	char my_dma[128];	/* The dma buffer to use           */
	char my_fcb[36];	/* The file control block          */
	char **skew_table;
	int  PHYS_SEC;	/* The physical sector to read/write       */
	unsigned STORAGE;	/* The size of the big buffer      */
	struct dpb *THIS;	/* Disk parameter information      */
	int  DIRECTORY;	/* The directory track                     */
	int  SECTORS;	/* Sectors per track                       */
	unsigned RECORDS;	/* Total available records         */
	int LAST_TRACK;	/* The highest track No. on the disk       */
	int BLK_SIZE;	/* Allocation block size in records        */
	int DIR_SIZE;	/* No. of allocation blocks in directory   */
	int FIRST_BLOCK;	/* First block No. in directory    */
	int  ENTRIES;	/* The active entries in directory         */
	char *BIG_BUF;
/*-----------------------------------------------------------------*/
	pec_clear();	/* Universal routine                       */
	printf("%s - Version %s\n",NAME, VERSION);
	printf("Copyright 1986 - Cogar Computer Services Pty.Ltd.\n");
	printf("----------------------------------------------------------\n");
	printf("This programme will first 'clean up' the directory\n");
	printf("tracks by removing any inactive (0xe5) entries.\n");
	printf("Then it sorts the active entries into (approximate)\n");
	printf("alphabetical order and writes them....as a copy of the\n");
	printf("directory onto the last track of the disk, as DIR2.\n");
	printf("Finally it creates the file 'DIRECT.ORY' in User Area\n");
	printf("15 to protect the second directory against being over\n");
	printf("written by other files.\n");
	printf("----------------------------------------------------------\n");
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
    starting drive in case these are changed later.                */
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
		printf("Enter the DRIVE you wish to check...A, B, C...\n");
		DRIVE = toupper(getchar());
	}
	else if(argc == 2)
		DRIVE = toupper(argv[1][0]);
	lines(2);
/*-----------------------------------------------------------------*/
/*  Check that the selected drive is available/on-line.   If not
    then terminate the programme.   You may need to add a message
    about what is going on if your version of CP/M doesn't do
    this automatically, as mine does.                              */
/*-----------------------------------------------------------------*/
	if(!(skew_table = seldsk(DRIVE)))
		exit();		/* Must be something wrong         */
	if(select_dsk(DRIVE) != 0)
		exit();
/*-----------------------------------------------------------------*/
/*  Calculate the values we need to scan the directory.            */
/*-----------------------------------------------------------------*/

	THIS      = dpb_adr();
	DIRECTORY = THIS->OFF;
	SECTORS   = THIS->SPT;

	RECORDS   = THIS->DSM + 1;	/* Starting value          */

	for(i = 0; i < THIS->BSH; i++)
		RECORDS = RECORDS + RECORDS;

	if(RECORDS%SECTORS != 0)
		LAST_TRACK = RECORDS/SECTORS + DIRECTORY;
	else LAST_TRACK = RECORDS/SECTORS + DIRECTORY - 1;
	printf("The highest track No.  =  %d\n", LAST_TRACK);

	BLK_SIZE = (128 << THIS->BSH)/128;  /* In records          */
	DIR_SIZE = SECTORS/BLK_SIZE;        /* In blocks           */

	FIRST_BLOCK = (THIS->DSM + 1) - DIR_SIZE;

	if((THIS->DSM >> 8) == 0)
		DATA_MAP = 16;
	else DATA_MAP    =  8;

/*-----------------------------------------------------------------*/
/*  Now create a file control block for the file "DIRECT.ORY"
    under User Area 15                                             */
/*-----------------------------------------------------------------*/
	setmem(&my_fcb[0], 36, 0);
	my_fcb[0] = 15;		/* The User Number                 */
	strcpy(&my_fcb[1], "DIRECT  ORY\0"); /* The file name      */
	my_fcb[12] = my_fcb[13] = my_fcb[14] = 0;
	my_fcb[15] = BLK_SIZE*DIR_SIZE;	/* No. of records          */

	BLOCK1 = FIRST_BLOCK;	/* Starting value                  */

	if(DATA_MAP == 16)
	{
		for(i = 0; i < DIR_SIZE; i++)
		{
			my_fcb[16 + i] = BLOCK1;
			BLOCK1++;
		}
	}
	else if(DATA_MAP == 8)
	{
		for(i = 0; i < DIR_SIZE*2; i = i + 2)
		{
			my_fcb[16 + i] = BLOCK1 & 0xff;
			my_fcb[17 + i] = (BLOCK1 >> 8) & 0xff;
			BLOCK1++;
		}
	}
/*-----------------------------------------------------------------*/

	set_dma(&my_dma[0]);	/* Create DMA buffer               */
	set_trk(DIRECTORY);

/*-----------------------------------------------------------------*/
/*  Read the directory track and count the number of active
    entries.   This information is required for the size of
    the big buffer.                                                */
/*-----------------------------------------------------------------*/
	printf("Counting the active directory entries.\n");
	ENTRIES = 0;		/* Starting conditions             */
	for(i = 0; i < SECTORS; i++)
	{
		PHYS_SEC = biosh(16, i, *skew_table);
		set_sec(PHYS_SEC);
		setmem(&my_dma[0], 128, 0xe5);
		read_sec();	/* Put information into DMA buffer */

	for(j = 0; j < 4; j++)
	{
		if(my_dma[j*32] != 0xe5)
			ENTRIES++;
	}
	}
	printf("Active entries (all extents) = %d\n", ENTRIES);
/*-----------------------------------------------------------------*/
/*  Now secure enough storage to hold all the active entries.      */
/*-----------------------------------------------------------------*/
	STORAGE = (((ENTRIES/4)*4) + 4)*32;
	BIG_BUF = alloc(STORAGE);
	setmem(BIG_BUF, STORAGE, 0xe5);	/* Clear the buffer        */
/*-----------------------------------------------------------------*/
/*  Then transfer the active entries to the big buffer.            */
/*-----------------------------------------------------------------*/
	set_trk(DIRECTORY);
	j = 0;
	for(i = 0; i < SECTORS; i++)
	{
		PHYS_SEC = biosh(16, i, *skew_table);
		set_sec(PHYS_SEC);
		setmem(&my_dma[0], 128, 0xe5);
		read_sec();	/* Put information into DMA buffer */

		for(k = 0; k < 4; k++)
		{
			if(my_dma[k*32] != 0xe5)
			{
				movmem(&my_dma[k*32], &BIG_BUF[j], 32);
				j = j + 32;
			}
		}
	}
/*-----------------------------------------------------------------*/
/*  All the active entries are now in the big buffer so we can
    condition the last track (all 0xe5) and then write the contents
    of the big buffer to this track in (almost) alphabetical
    order.                                                         */
/*-----------------------------------------------------------------*/
	printf("Re-formatting last track of Disk - %c.\n", DRIVE);
	setmem(&my_dma[0], 128, 0xe5);
	set_trk(LAST_TRACK);

	for(i = 0; i < SECTORS; i++)
	{
		PHYS_SEC = biosh(16, i, *skew_table);
		set_sec(PHYS_SEC);
		write_sec();
	}
	printf("Writing directory to the last track.\n");
	set_trk(LAST_TRACK);
	k = 0;		/* Count of active files in DMA buffer     */
	l = 0;		/* The directory sector to write           */

	for(c = 0x41; c < 0x60; c++)
	{
	for(i = 0; i < STORAGE; i = i + 32)
	{
	if(BIG_BUF[i + 1] == c)
	{
		movmem(&BIG_BUF[i], &my_dma[k*32], 32);
		k++;
		if(k == 4)
		{
			PHYS_SEC = biosh(16, l, *skew_table);
			set_sec(PHYS_SEC);
			write_sec();	/* Write to directory      */
			l++;	/* Advance sector count            */
			k = 0;
			setmem(&my_dma[0], 128, 0xe5);
		}
	}
	}
	}
	if(k > 0 && k < 4)
	{
		PHYS_SEC = biosh(16, l, *skew_table);
		set_sec(PHYS_SEC);
		write_sec();
	}

	printf("Directory now re-written onto last track.\n");

/*-----------------------------------------------------------------*/
/*  As the final job, now write the entry under DIRECT.ORY to the
    disk's directory tracks.   Put the entry into the first
    available slot.                                                */
/*-----------------------------------------------------------------*/
	user_id(15);		/* Check to see whether the file   */
	if((FD = open("DIRECT.ORY", 0)) != -1)
	{
		close(FD);
		unlink("DIRECT.ORY");	/* already exists          */
		printf("Deleting existing DIRECT.ORY file.\n");
	}
	user_id(OLD_USER);

	set_trk(DIRECTORY);
	set_dma(&my_dma[0]);

	for(i = 0; i < SECTORS; i++)
	{
		PHYS_SEC = biosh(16, l, *skew_table);
		set_sec(PHYS_SEC);
		read_sec();
	for(j = 0; j < 128; j = j + 32)
	{
		if(my_dma[j] == 0xe5)	/* Found a vacant slot     */
		{
		movmem(&my_fcb[0], &my_dma[j], 32);
		write_sec();
		j = 128;	/* Get out of counting loop        */
		i = SECTORS;
		}
	}
	}
	printf("File ==> DIRECT.ORY written to main directory.");
/*-----------------------------------------------------------------*/
/*  Before closing, make sure the User No. and the Drive return
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
else printf("\nCan't find - UTIL.COM.   Returning to CP/M.");

}
/*-----------------------------------------------------------------*/
			PHYS_SEC = biosh(16, l, *skew_table);
			set_sec(PHYS_SEC);
			write_sec();	/* Write to directory      */
			l