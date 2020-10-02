
/*-----------------------------------------------------------------*/
/*
         Programme:     ARC2.C
         ---------
         Purpose:       Will "archive" files to the nominated disk
         -------        and then set bit 7 of byte "t3" of the
                        original file to show that it has been
                        copied.

         Compiler:      BDS C V 1.50
         --------
         Written:       10/07/85
         -------
         Amended:       20/11/86  (for UTIL)
         -------        28/11/86 to allow for the circumstance when
                        the archive disk becomes full before the run
                        is finished.

         Version:       2.0
         -------

         Copyright 1986 - Cogar Computer Services Pty. Ltd.        */
/*-----------------------------------------------------------------*/
#include <bdscio.h>
#include <pec.h>
/*-----------------------------------------------------------------*/
#define	VERSION "2.0\0"
#define NAME    "ARCHIVE\0"
/*-----------------------------------------------------------------*/
main(argc, argv)	/* For Command Line processing             */
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	char DRIVE;	/* The active drive                        */
	char USER;	/* The User No. for this programme         */
	int i, j, n, file_count, done_count; /* Counters           */
	char c, d2;
	int CPM;	/* To check the CP/M Version number        */
	char OLD_DRIVE;	/* The drive at start of programme         */
	char OLD_USER;	/* The User No. at start of programme      */
	int  FD1, FD2;	/* For the open files                      */
	char MY_DMA[128];	/* For data transfer               */
	char NAME_BUF[1600];    /* For 50 file names               */
	char BIG_BUF[16384];   /* For file transfer - 128 records  */
	struct dpb *THIS;  /* For disk information                 */
	int  DIRECTORY, SECTORS, offset;
	char **skew_table; /* For sector translation               */
	int  PHYS_SEC;
	char in_file[15], out_file[15];
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
/*  Now check the Command Line to see if any command was entered.
    Other checks can also be used, as required but then it will be
    necessary to change this coding.                               */
/*-----------------------------------------------------------------*/
	if(argc != 3)
	{
printf("This programme will copy files from the nominated drive\n");
printf("to the ARCHIVE disk but will only accept those files which\n");
printf("have not previously been copied.   It does this by checking\n");
printf("the archive byte....byte 't3' of the file name.\n\n");
printf("When it has copied the file across it will set bit 7 of t3\n");
printf("in the particular file name to show that this file has now\n");
printf("been archived.\n\n");

		printf("\tEnter the DRIVE to take files FROM - ");
		DRIVE = toupper(getchar());
		lines(2);
		printf("\tEnter the DRIVE to copy files TO   - ");
		d2 = toupper(getchar());
	}
	else if(argc == 3)
	{
		DRIVE = toupper(argv[1][0]);
		d2 = toupper(argv[2][0]);
	}
	lines(2);
	if(DRIVE == d2)
	{
		printf("Copying TO and FROM same disk is not allowed.");
		exit();
	}
/*-----------------------------------------------------------------*/
/*  Save the starting drive in case it is changed later.           */
/*-----------------------------------------------------------------*/
	OLD_DRIVE = get_default() + 0x41;
/*-----------------------------------------------------------------*/
/*  Save the starting User No. in case it is changed later.        */
/*-----------------------------------------------------------------*/
	OLD_USER = user_id(0xff);
/*-----------------------------------------------------------------*/
/*  Now check that the selected drives are available.              */
/*-----------------------------------------------------------------*/
	printf("You have nominated to copy FROM -  %c\n", DRIVE);
	if(select_dsk(DRIVE) != 0)
		exit();
	printf("                           TO   -  %c\n", d2);
	if(select_dsk(d2) != 0)
		exit();
	else printf("Both drives are acceptable.");
	lines(2);
/*-----------------------------------------------------------------*/
/*  Get the pointer to the skew table for sector translation when
    reading the Directory, and go back to the Copy drive.          */
/*-----------------------------------------------------------------*/
	if(!(skew_table = seldsk(DRIVE)))
		exit();
	if(select_dsk(DRIVE) != 0)
		exit();
/*-----------------------------------------------------------------*/
/*  Get the information needed to read Directory track.            */
/*-----------------------------------------------------------------*/
	THIS = dpb_adr();
	DIRECTORY = THIS->OFF;	/* The directory track             */
	SECTORS   = THIS->SPT;  /* Records per track               */

	set_dma(&MY_DMA[0]);
	set_trk(DIRECTORY);
	file_count = done_count = offset = 0;
/*-----------------------------------------------------------------*/
/*  We now have to check for files to be archived.   When found
    their names won't be in a suitable form to be used in the "open"
    and "close" file functions available in BDS C.   It will be
    necessary to first convert the names to suitable null-terminated
    strings.                                                       */
/*-----------------------------------------------------------------*/
	printf("               FILES BEING ARCHIVED\n");
	printf("               --------------------\n\n");

	for(i = 0; i < SECTORS; i++)
	{
		PHYS_SEC = biosh(16, i, *skew_table);
		set_sec(PHYS_SEC);
		setmem(&MY_DMA[0], 128, 0xe5);
/*-----------------------------------------------------------------*/
/*  Read one Directory sector into DMA buffer and then check if
    any files are available for archiving.                         */
/*-----------------------------------------------------------------*/
		if(read_sec() != 0) /* Put information into DMA buffer */
		{
			printf("\nBad read of sector - %d", PHYS_SEC);
			user_id(OLD_USER);
			if(select_dsk(OLD_DRIVE) != 0)
		printf("\nUnable to return to starting drive.");
			exit();
		}

		for(j = 0; j < 4; j++)
		{
			USER = MY_DMA[j*32];
			c    = MY_DMA[j*32 + 11];

	if(USER != 0xe5 && USER == OLD_USER && c < 128)
			{
				if(++file_count == 50)
				{
	printf("There is a limit of 50 files which can be copied.\n");
	printf("Your options are to run the programme again after\n");
	printf("this run finishes....and so do the additional files.\n");
	printf("Else you can go to ARCHIVE.C and alter the size of\n");
	printf("the NAME_BUF to accomodate more files.\n\n");
				j = 4;
				i = SECTORS;
				}

	movmem(&MY_DMA[j*32], &NAME_BUF[offset], 32);
				offset = offset + 32;
			}
		}
	}
/*-----------------------------------------------------------------*/
/*  Check to see whether we have anything to archive.   If not
    then tell the user and terminate.                              */
/*-----------------------------------------------------------------*/
	if(file_count == 0)
	{
		user_id(OLD_USER);
		if(select_dsk(OLD_DRIVE) != 0)
		{
			printf("\nUnable to return to starting drive.");
			exit();
		}
	printf("No files altered since last time.\n");
	printf("\nDo you want to run another utility - Y/N.");
if((FD1 = open("UTIL.COM", 0)) != 0 && (c = toupper(getchar())) == 'Y')
{
	close(FD1);
	exec("UTIL");
}
	exit();
	}
/*-----------------------------------------------------------------*/
/*  Now open the files for reading/writing as the case may be and
    copy them across using the available BDS C functions.          */
/*-----------------------------------------------------------------*/
	offset = 0;	/* Go back to start of name buffer         */

	for(i = 0; i < file_count; i++)
	{
		make_name(&NAME_BUF[offset], out_file);
		out_file[0] = DRIVE;
		make_name(&NAME_BUF[offset], in_file);
		in_file[0]  = d2;

printf("Copying - %s\t\tTO - %s\n", &out_file[0], &in_file[0]);

		if((FD1 = open(&out_file[0], 0)) == -1)
		{
			printf("Unable to find - %s", &out_file[0]);
			user_id(OLD_USER);
			if(select_dsk(OLD_DRIVE) != 0)
		printf("\nUnable to return to starting drive.");
			exit();
		}

		if((FD2 = creat(&in_file[0])) == -1)
		{
			printf("Unable to create file - %s\n", &in_file[0]);
			printf("It may mean the disk is full, so try a new ");
			printf("disk and run ARCHIVE again.\n\n");
			close(FD1);
			exit();
		}

/*-----------------------------------------------------------------*/
/*  The files are open for read/write so now transfer to the
    archive disk.                                                  */
/*-----------------------------------------------------------------*/
		while(( n = read(FD1, &BIG_BUF[0], 128)) > 0)
			if((write(FD2, &BIG_BUF[0], n)) != n)
			{
	printf("The ARCHIVE disk is probably full.   Try again with a ");
	printf("fresh disk.\n\n");
			close(FD2);
			unlink(in_file);
			i = file_count;
			GOTO JMP1;
			}
		done_count++;
		close(FD2);
JMP1:		close(FD1);
		offset = offset + 32;
	}
/*-----------------------------------------------------------------*/
/*  Now set the archive bit for each of the files which was
    copied.                                                        */
/*-----------------------------------------------------------------*/
	set_dma(&MY_DMA[0]);
	set_trk(DIRECTORY);
	n = 0;			/* The counter                     */

	for(i = 0; i < SECTORS; i++)
	{
		PHYS_SEC = biosh(16, i, *skew_table);
		set_sec(PHYS_SEC);
		setmem(&MY_DMA[0], 128, 0xe5);
/*-----------------------------------------------------------------*/
/*  Read one Directory sector into DMA buffer and then check the
    files available for archiving.                                 */
/*-----------------------------------------------------------------*/
		if(read_sec() != 0) /* Put information into DMA buffer */
		{
			printf("\nBad read of sector - %d", PHYS_SEC);
			user_id(OLD_USER);
			if(select_dsk(OLD_DRIVE) != 0)
		printf("\nUnable to return to starting drive.");
			exit();
		}

		for(j = 0; j < 4; j++)
		{
			USER = MY_DMA[j*32];
			c    = MY_DMA[j*32 + 11];

	if(USER != 0xe5 && USER == OLD_USER && c < 128 && n < done_count)
			{
				MY_DMA[j*32 + 11] = c | 0x80;
				n++;
			}
		}
		write_sec();
	}
	printf("\n\nTotal No. of files archived  =  %d", done_count);
/*-----------------------------------------------------------------*/
/*  Before closing, make sure the User No. and the Drive No.
    return to original settings.                                   */
/*-----------------------------------------------------------------*/

	user_id(OLD_USER);
	if(select_dsk(OLD_DRIVE) != 0)
	{
		printf("\nUnable to return to starting drive.");
		exit();
	}
	printf("\nDo you want to run another utility - Y/N.");
if((FD1 = open("UTIL.COM", 0)) != 0 && (c = toupper(getchar())) == 'Y')
{
	close(FD1);
	exec("UTIL");
}
}
/*-----------------------------------------------------------------*/
/*            SUBROUTINES                                          */
/*-----------------------------------------------------------------*/
void make_name(buf, file_name)
char *buf, file_name[15];
{
	char c, SW;
	int i, j;
	SW = TRUE;

	for(i = 1, j = 2; i < 12; i++)
	{
		c = buf[i] & 0xff;

		if(c != SPACE && j != 10 && SW)
		{
			file_name[j] = c;
			j++;
		}
		else if(j == 10 && SW)
		{
			file_name[j] = '.';
			j++;
			file_name[j] = c;
			j++;
			SW = FALSE;
		}
		else if(c == SPACE && SW )
		{
			file_name[j] = '.';
			j++;
			SW = FALSE;
		}
		else if(i > 8 && !SW)
		{
			file_name[j] = c;
			j++;
		}
/*-----------------------------------------------------------------*/
/*  Knowledgable programmers will have noticed that this algorithmn
    makes no provision for the circumstance when there is no TYPE.
    e.g. for a file named, say, "PHIL".   Fortunately for us BDS C
    doesn't care whether such a file is entered as -

              PHIL
    or        PHIL.

    because it will convert it to the proper form anyway.   So I have
    taken advantage of this to keep things simple.   Just thought
    you'd be pleased to know.                                      */
/*-----------------------------------------------------------------*/
	}
	file_name[j] = '\0';
	file_name[1] = ':';
}
/*-----------------------------------------------------------------*/
---