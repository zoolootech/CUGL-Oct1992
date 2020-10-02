
/*-----------------------------------------------------------------*/
/*       STANDARD HEADER FOR BDS C PROGRAMMES
         ------------------------------------

         Programme:     USER2.C
         ---------
         Purpose:       This programme will delete all inactive
         -------        entries from the directory and then re-write
                        the directory in (almost) alphabetical order.

         Written:       11/05/86
         -------
         Amended:       06/11/86
         -------
         Version:       2.0
         -------

         Copyright 1986 - Cogar Computer Services Pty. Ltd.        */
/*-----------------------------------------------------------------*/
#include <bdscio.h>
#include <pec.h>
/*-----------------------------------------------------------------*/
#define	VERSION "2.0\0"
#define NAME    "USER2\0"
/*-----------------------------------------------------------------*/

main(argc, argv)	/* For Command Line processing             */
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	int i, j, k, l;
	char c;
	int CPM;	/* To check the CP/M Version number        */
	char DRIVE;	/* The active drive                        */
	char OLD_DRIVE;	/* The starting drive No.                  */
	char OLD_USER;	/* The User No. at start of programme      */
	char FROM_USER;	/* User area to take file from             */
	char TO_USER;	/* User area to put file to                */
	char my_dma[128];	/* The dma buffer to use           */
	char my_file[15];
	char filename[12];
	char READING, FOUND, FLAG;
	int  FD;	/* The file descriptor                     */
	char out_buf[128];
	char **skew_table;
	int  PHYS_SEC;	/* The physical sector to read/write       */
	unsigned STORAGE;	/* The size of the big buffer      */
	struct dpb *THIS;	/* Disk parameter information      */
	int  DIRECTORY;	/* The directory track                     */
	int  SECTORS;	/* Sectors per track                       */
	int  ENTRIES;	/* The active entries in directory         */
	char *BIG_BUF, *infile;
/*-----------------------------------------------------------------*/
	pec_clear();	/* Universal routine                       */
	printf("%s - Version %s\n",NAME, VERSION);
	printf("Copyright 1986 - Cogar Computer Services Pty.Ltd.\n");
	printf("----------------------------------------------------------\n");
	printf("This programme will copy a nominated programme to another\n");
	printf("User Area, from the present User Area.   At the same time\n");
	printf("as it does this it will also 'clean up' the directory\n");
	printf("tracks by removing any inactive (0xe5) entries, and will\n");
	printf("then sort the active entries into (approximate) alphabetical\n");
	printf("order.\n");
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
    starting Drive No. in case either is changed later.            */
/*-----------------------------------------------------------------*/

	OLD_USER = user_id(0xff);
	OLD_DRIVE = get_default() + 0x41;

/*-----------------------------------------------------------------*/
/*  Now check the Command Line to see if anything was entered.
    We are looking for the file name to transfer to another user
    area and the drive code if the current drive isn't being used  */
/*-----------------------------------------------------------------*/
	if(argc != 2)
	{
		printf("You have to enter the FILE you wish to transfer.\n");
		printf("The required style is -\n\n");
		printf("\t\t[dr:]filename.type\n\n");
		printf("where 'dr:' is the optional drive code.   If you ");
		printf("don't\n");
		printf("specify a drive then the current drive is used\n");
		infile = gets(my_file);
	}
	else if(argc == 2)
		infile = argv[1];
	up_str(infile);		/* Make name upper case letters    */
	lines(2);

/*-----------------------------------------------------------------*/
/*  Now check for the drive to be used.                            */
/*-----------------------------------------------------------------*/
	if(infile[1] == ':')	/* Then we have a drive code       */
		DRIVE = toupper(infile[0]);
	else DRIVE = GET_DEFAULT() + 65;
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
	printf("          For Drive - %c\n", DRIVE);
	printf("          -------------\n\n");
/*-----------------------------------------------------------------*/
/*  The next job is to put the file name in the form which it will
    appear in the directory.                                       */
/*-----------------------------------------------------------------*/
	setmem(&filename[0], 11, 0x20);
	if(infile[1] != ':')	/* No drive code given             */
		j = 0;
	else j = 2;		/* Drive code given                */

	for(i = 0; i < 11; i++)
	{
		if(infile[j] != '.' && i < 8)
		{
			filename[i] = infile[j];
			j++;
		}
		else if(infile[j] == '.' && i < 8)
			filename[i] = SPACE;
		else if(i > 7)
		{
			j++;
		if(isalnum(infile[j]))
			filename[i] = infile[j];
		else filename[i] = 0x20;
		}
	}
	filename[i] = '\0';
	printf("The parsed file name is ==> %s\n", filename);

/*-----------------------------------------------------------------*/
/*  Everything appears to be OK, so now ask the user to nominate
    the user area he/she wishes the file to be put into.           */
/*-----------------------------------------------------------------*/
	printf("Which User Area is the file now in - ");
	scanf("%d", &FROM_USER);
	lines(2);
	printf("Which USER No. do you want this file to go to -\n");
	THIS = dpb_adr();
	scanf("%d", &TO_USER);
	SECTORS = THIS->SPT;
	TO_USER = TO_USER & 0xff;
	if(TO_USER == FROM_USER)
	{
		printf("You can't copy to same User Area.");
		exit();
	}
	lines(2);
/*-----------------------------------------------------------------*/
/*  Now do some checking to see -

         A.   Whether the file exists in the first user area; and

         B.   Whether or not it already exists in the nominated
              user area.                                           */
/*-----------------------------------------------------------------*/
	user_id(FROM_USER);	/* Go to first User Area           */
	if((FD = open(infile,0)) == -1)
	{
		printf("Unable to find - %s\n", infile);
		user_id(OLD_USER);
		exit();
	}
	else printf("OK, have found - %s\n", infile);
	close(FD);
	user_id(TO_USER);	/* Go to nominated user area for check     */
	if((FD = open(infile,0)) != -1)
	{
printf("The file - %s  already exists in User Area - %d\n", infile, TO_USER);
		user_id(OLD_USER);
		exit();
	}
else printf("and it doesn't yet exist in User Area - %d\n\n",  TO_USER);
	close(FD);
	user_id(FROM_USER);	/* Return to first user area       */
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
	printf("Present active entries (all extents) = %d\n", ENTRIES);
/*-----------------------------------------------------------------*/
/*  Now secure enough storage to hold all the active entries.      */
/*-----------------------------------------------------------------*/
	STORAGE = (((ENTRIES/4)*4) + 8)*32;
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
    erase the existing directory.   After this we find the file
    to transfer to the nominated user area, add this to the end
    of the big buffer then write the active entries back to the
    directory track in (almost) alphabetical order.   This
    completes the programme.                                       */
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
/*-----------------------------------------------------------------*/
/*  Old directory erased.                                          */
/*-----------------------------------------------------------------*/
	FOUND   = FALSE;
	setmem(&out_buf[0], 128, 0xe5);
	l = 0;

	for(i = 0; i < ENTRIES*32; i = i + 32)
	{
		if(BIG_BUF[i] != 0xe5)
		{
if(compare(&filename[0], &BIG_BUF[i + 1], 11) == 1 && BIG_BUF[i] == FROM_USER)
		{
		movmem(&BIG_BUF[i], &out_buf[l], 32);
		out_buf[l] = TO_USER;	/* Put into chosen area    */
		l = l + 32;
		FOUND = TRUE;
		}
		}
	}
	if(FOUND == FALSE)
	{
		printf("The file - %s  was not matched.\n", filename);
		user_id(OLD_USER);
	}
/*-----------------------------------------------------------------*/
/*  File located, so add to big buffer.                            */
/*-----------------------------------------------------------------*/
	if(FOUND == TRUE)
		movmem(&out_buf[0], &BIG_BUF[ENTRIES*32], 128);

/*-----------------------------------------------------------------*/
/*  Then write back the re-formatted directory entries.            */
/*-----------------------------------------------------------------*/
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
/*  Before closing, make sure the User No. and Drive No. return
    to original settings.                                          */
/*-----------------------------------------------------------------*/
	user_id(OLD_USER);
	if(select_dsk(OLD_DRIVE) != 0)
	{
		printf("\nUnable to return to starting drive.");
		exit();
	}
	printf("\n\nDo you want to run another utility - Y/N.");
if((FD = open("UTIL.COM", 0)) != -1 && (c = toupper(getchar())) == 'Y')
{
	close(FD);
	exec("UTIL");
}
else printf("\n\nReturning to CP/M.");

}
/*-----------------------------------------------------------------*/
char compare(s1, s2, n)
char *s1, *s2;
int n;
{
	int i;

	for(i = 0; i < n; i++)
	{
		if(toupper(s1[i]) != (toupper(s2[i]) & 0x7f))
			return(0);
	}
	return(1);
}
/*-----------------------------------------------------------------*/                      */
/*------------------------------