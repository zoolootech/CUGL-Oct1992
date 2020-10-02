/*-----------------------------------------------------------------*/
/*
         Programme:     RESTORE.C
         ---------
         Purpose:       Will restore a file which has been erased
         -------        if that file can be found it the second
                        directory (on last track) and the allocation
                        blocks it controls are not already in use by
                        another file.

         Compiler:      BDS C V 1.50
         --------
         Written:       10/11/86
         -------
         Amended:       xx/xx/86
         -------
         Version:       1.0
         -------

         Copyright 1986 - Cogar Computer Services Pty. Ltd.        */
/*-----------------------------------------------------------------*/
#include <bdscio.h>
#include <pec.h>
/*-----------------------------------------------------------------*/
#define	VERSION "1.0\0"
#define NAME    "RESTORE\0"
/*-----------------------------------------------------------------*/

main(argc, argv)	/* For Command Line processing             */
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	int i, j, k, l, FD, BIT_NO, EXTENT, BLOCKS;
	unsigned BLOCK_NO, PHYS_SEC, BYTE_NO;
	unsigned FILE_LEN;	/* Length of DIRECT.ORY            */
	char *ALV;		/* Pointer to ALV table            */
	char c;
	int CPM;	/* To check the CP/M Version number        */
	char DRIVE;	/* The active drive                        */
	char OLD_DRIVE;	/* The drive at start of programme         */
	char OLD_USER;	/* The User No. at start of programme      */
	char USER;	/* The User No. for this programme         */
	char MY_FILE[13], *PTR;	/* The file name to be restored    */
	char filename[12];	/* The parsed file name            */
	char **skew_table;
	char *BIG_BUF;
	struct dpb *THIS;	/* Pointer to DPB                  */
	char DMA_BUF[128];	/* Transfer buffer                 */
	char MY_FCB[32];	/* The file control block          */
	char FLAG;	/* The allocation block flag               */
	char CHECK;	/* The allocation block flag               */
	char MATCH;
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
/*  The CP/M Version is OK, so save the starting User No. and the
    starting Drive No. in case either is changed later.            */
/*-----------------------------------------------------------------*/
	OLD_USER = user_id(0xff);
	OLD_DRIVE = get_default() + 0x41;

/*-----------------------------------------------------------------*/
	printf("This programme can be used to restore a file which\n");
	printf("was previously erased so long as CP/M hasn't already\n");
	printf("re-allocated the disk space to another file.   If this\n");
	printf("has happened then you can't recover the data from the\n");
	printf("erased file.\n\n");
	if(argc != 2)
	{
	printf("Enter the name of the file you wish to restore as -\n\n\t");
	printf("[d:]filename\n\n");
	printf("where 'd' is the optional drive code.   If you don't\n");
	printf("specify the drive then the default drive will be used.\n");
	PTR = gets(MY_FILE);
	}
	else PTR = argv[1];
	up_str(PTR);
	line();

/*-----------------------------------------------------------------*/
/*  Now check  to see if a Drive Code was entered.
    Other checks can also be used, as required but then it will be
    necessary to change this coding.                               */
/*-----------------------------------------------------------------*/
	if(PTR[1] == ':')
		DRIVE = PTR[0];
	else DRIVE = OLD_DRIVE;
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
/*  Next put the file name in the form which it will appear in
    the directory.                                                 */
/*-----------------------------------------------------------------*/
	setmem(&filename[0], 11, 0x20);
	if(PTR[1] != ':')	/* No drive code given             */
		j = 0;
	else j = 2;		/* Drive code given                */

	for(i = 0; i < 11; i++)
	{
		if(PTR[j] != '.' && i < 8)
		{
			filename[i] = PTR[j];
			j++;
		}
		else if(PTR[j] == '.' && i < 8)
			filename[i] = SPACE;
		else if(i > 7)
		{
			j++;
		if(isalnum(PTR[j]))
			filename[i] = PTR[j];
		else filename[i] = 0x20;
		}
	}
	filename[i] = '\0';
	printf("The parsed file name is ==> %s\n\n", filename);

/*-----------------------------------------------------------------*/
/*  First check to see whether the file exists on the disk in
    any user area.   If so, no need to restore it.                 */
/*-----------------------------------------------------------------*/
	THIS = dpb_adr();	/* Get dpb address for later       */
	BLOCKS = THIS->DSM + 1;	/* Total blocks in table           */

	if((FLAG = THIS->DSM >> 8) == 0)
		FLAG = FALSE;   /* 8-bit addresses                 */
	else FLAG = TRUE;       /* 16-bit addresses                */
	if(THIS->AL0 < 240)
		c = 2;
	else c = 4;
	ALV = get_alv();	/* Pointer to ALV table            */

	printf("Checking all User Areas for file - %s\n\n", PTR);
	for(i = 0; i < 16; i++)
	{
		user_id(i);	/* Go to User Area                 */
		if((FD = open(PTR, 0)) != -1)
		{
	printf("The file  %s can be found in User Area - %d\n", PTR, i);
	printf("so you can get it with USER2.\n");
		close(FD);
		user_id(OLD_USER);
		exit();
		}
	}
	printf("It wasn't found so now check file  DIRECT.ORY\n\n");
	user_id(15);

	if((FD = open("DIRECT.ORY", 0)) == -1)
	{
		printf("The second directory doesn't exist so we can't\n");
		printf("go any further....sorry.\n");
		user_id(OLD_USER);
		exit();
	}
	printf("OK...we have the second directory.   Now check for\n");
	printf("the file  %s\n\n", PTR);

	FILE_LEN = cfsize(FD);
	BIG_BUF = alloc(FILE_LEN*128);
	read(FD, BIG_BUF, FILE_LEN*128);	/* Get information         */
	close(FD);

	MATCH = FALSE;
	EXTENT = 0;

	for(i = 0; i < FILE_LEN*128; i = i + 32)
	{
	if(compare(&filename[0], &BIG_BUF[i + 1], 11))
	{
		printf("Have matched name.\n");
		MATCH = TRUE;
/*-----------------------------------------------------------------*/
/*  The file name has been found so now check the blocks to see
    whether any other file is using them.                          */
/*-----------------------------------------------------------------*/
		movmem(&BIG_BUF[i], &MY_FCB[0], 32);
		for(j = 0; j < 16; j++)
		{
		if(!FLAG)
			BLOCK_NO = MY_FCB[16 + j] & 0xff;
		else if(FLAG)
		{
			BLOCK_NO = (MY_FCB[17 + j] << 8) & 0xff00;
		BLOCK_NO = BLOCK_NO + (MY_FCB[16 + j] & 0xff);
		j++;
		}
/*-----------------------------------------------------------------*/
/*  Now have the block No. so we need to convert this to a number
    in the ALV table.   Note the Table holds 8 block numbers per
    byte, starting from Block No. 0.                               */
/*-----------------------------------------------------------------*/
	if(BLOCK_NO > 0)
	{
	printf("\tChecking Block No. = %d\n", BLOCK_NO);

		BYTE_NO = BLOCK_NO/8;	/* In ALV Table            */
		BIT_NO  = BLOCK_NO%8;	/* I hope !                */
		BIT_NO  = ABS(BIT_NO - 7);
		if(b_test(ALV[BYTE_NO], BIT_NO))
		{
		printf("Unable to restore file.   Allocation block ");
		printf("%d  already in use.", BLOCK_NO);
		user_id(OLD_USER);
		exit();
		}
		}
	}
/*-----------------------------------------------------------------*/
/*  All allocation blocks test OK.   Now re-write the FCB into
    the disk directory.                                            */
/*-----------------------------------------------------------------*/
	printf("Writing Extent - %d  to disk directory.\n", EXTENT);
	EXTENT++;

	set_trk(THIS->OFF);
	set_dma(&DMA_BUF[0]);

	for(k = 0; k < THIS->SPT; k++)
	{
		PHYS_SEC = biosh(16, k, *skew_table);
		set_sec(PHYS_SEC);
		read_sec();
	for(l = 0; l < 128; l = l + 32)
	{
		if(DMA_BUF[l] == 0xe5)	/* Found a vacant slot     */
		{
		movmem(&MY_FCB[0], &DMA_BUF[l], 32);
		write_sec();
		l = 128;	/* Get out of counting loop        */
		k = THIS->SPT;
		}
	}
	}
/*-----------------------------------------------------------------*/
/*  Have transferred one extent to the disk directory.             */
/*-----------------------------------------------------------------*/
	}
	}
	if(!MATCH)
		printf("Unable to match file name.   Not in DIR2.\n");
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
	line();
	printf("\nDo you want to run another utility - Y/N.");
if((FD = open("UTIL.COM", 0)) != 0 && (c = toupper(getchar())) == 'Y')
{
	close(FD);
	exec("UTIL");
}
}
/*-----------------------------------------------------------------*/
/*  Subroutines used in this programme.                            */
/*-----------------------------------------------------------------*/
char compare(s1, s2, n)
char *s1, *s2;
int n;
{
	int i;

	for(i = 0; i < n; i++)
	{
		if(toupper(s1[i]) != (toupper(s2[i]) & 0x7f))
			return(0);	/* If they don't match     */
	}
	return(1);	/* If the two strings match                */
}
/*-----------------------------------------------------------------*/
e-write the FCB into
    the disk directory.                                            */
/