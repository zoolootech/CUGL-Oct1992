
/*-----------------------------------------------------------------*/
/*       STANDARD HEADER FOR BDS C PROGRAMMES
         ------------------------------------

         Programme:     CLEANUP.C
         ---------
         Purpose:       This programme will delete all inactive
         -------        entries from the directory and then re-write
                        the directory in (almost) alphabetical order.

         Written:       05/11/85
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
#define NAME    "CLEANUP\0"
/*-----------------------------------------------------------------*/

main(argc, argv)	/* For Command Line processing             */
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	int i, j, k, l, FD;
	char c;
	int CPM;	/* To check the CP/M Version number        */
	char DRIVE;	/* The active drive                        */
	char OLD_DRIVE;	/* The active drive at start of programme  */
	char OLD_USER;	/* The User No. at start of programme      */
	char USER;	/* The User No. for this programme         */
	char my_dma[128];	/* The dma buffer to use           */
	char **skew_table;
	int  PHYS_SEC;	/* The physical sector to read/write       */
	unsigned STORAGE;	/* The size of the big buffer      */
	struct dpb *THIS;	/* Disk parameter information      */
	int  DIRECTORY;	/* The directory track                     */
	int  SECTORS;	/* Sectors per track                       */
	int  ENTRIES;	/* The active entries in directory         */
	char *BIG_BUF;
/*-----------------------------------------------------------------*/
	pec_clear();	/* Universal routine                       */
	printf("%s - Version %s\n",NAME, VERSION);
	printf("Copyright 1986 - Cogar Computer Services Pty.Ltd.\n");
	printf("----------------------------------------------------------\n");
	printf("This programme is designed to 'clean up' the directory\n");
	printf("tracks by removing any inactive (0xe5) entries.\n");
	printf("At the same time as it does this it sorts the active\n");
	printf("entries into (approximate) alphabetical order.\n");
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
/*  Get the starting drive in case this is changed later.          */
/*-----------------------------------------------------------------*/
	OLD_DRIVE = get_default() + 0x41;
/*-----------------------------------------------------------------*/
/*  The CP/M Version is OK, so save the starting User No. in case
    this is changed later in the programme.                        */
/*-----------------------------------------------------------------*/

	OLD_USER = user_id(0xff);

/*-----------------------------------------------------------------*/
/*  Now check the Command Line to see if a Drive Code was entered.
    Other checks can also be used, as required but then it will be
    necessary to change this coding.                               */
/*-----------------------------------------------------------------*/
	if(argc < 2)
	{
		printf("You have to enter the DRIVE you wish to");
		printf(" check...A, B, C, D - \n");
		DRIVE = toupper(getchar());
	}
	else if(argc > 2)
	{
		printf("Too many arguments entered.   We only need the");
		printf(" DRIVE you wish to check.\n");
		printf("Please enter the DRIVE...A, B, C, D - \n");
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
    erase the existing directory.   After this we write the active
    entries back to the directory track in (almost) alphabetical
    order.   This completes the programme.                         */
/*-----------------------------------------------------------------*/
	printf("Scrubbing old directory.\n");
	for(i = 0; i < 1000; i++)
		;		/* Empty loop                      */
	setmem(&my_dma[0], 128, 0xe5);
	set_trk(DIRECTORY);
	for(i = 0; i < SECTORS; i++)
	{
		set_sec(i);
		write_sec();
	}
	printf("Writing re-formatted directory.\n");
	set_trk(DIRECTORY);
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

	printf("Directory now re-written.");

/*-----------------------------------------------------------------*/
/*  Before closing, make sure the User No. returns to original, and
    the original drive is restored.                                */
/*-----------------------------------------------------------------*/

	user_id(OLD_USER);
	if(select_dsk(OLD_DRIVE) != 0)
		exit();

	if((FD = open("UTIL.COM", 0)) != -1)
	{
		close(FD);
		exec("UTIL");
	}

}
/*-----------------------------------------------------------------*/to DMA buffer */

		for(k = 0; k < 4; k++)
		{
			if(my_dma[k*32] != 0xe5)
			{
				