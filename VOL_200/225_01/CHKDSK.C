/*-----------------------------------------------------------------*/
/* FILE:           CHKDSK.C
   ----

   This programme checks the disk parameters using the Disk
   Parameter Block information in the BIOS.   It will only work
   with CP/M 2.0 or higher.

   NOTE:      Written for BDS C.
   ----       It also requires DEFF4.CRL to compile the "COM"
              programme.

   Written:        August 23, 1986
   -------
   Updated:        November 8, 1986
   -------
   Version:        1.1
   -------

   Copyright 1986 - Cogar Computer Services Pty. Ltd.              */
/*-----------------------------------------------------------------*/
#include <bdscio.h>
#include <pec.h>
/*-----------------------------------------------------------------*/
#define	NAME	"CHKDSK"
#define	VERSION	"1.1"

main(argc, argv)
int argc;
char *argv[];
{
/*-----------------------------------------------------------------*/
/*  Space reserved for variables used in programme                 */
/*-----------------------------------------------------------------*/
	struct dpb *THIS;	/* Pointer to disk parameter block */
	char DRIVE;		/* For the drive code              */
	int i, NO_TRKS, LAST_TRACK, LAST_BLOCK;
	int BLOCK, BLK_TRK, FD;
	char c;
	int  BLOCKS;		/* The number of Allocation Blocks */
	int  BYTE_COUNT;	/* The bytes holding Al. Blocks    */
	char *MAP_ADR; 		/* The bit map address             */
	int CPM;	/* To check the CP/M Version number        */
	char OLD_DRIVE;	/* The drive at start of programme         */
	char OLD_USER;	/* The User No. at start of programme      */
	char USER;	/* The User No. for this programme         */
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
    starting Drive No.in case either is changed later.             */
/*-----------------------------------------------------------------*/

	OLD_USER = user_id(0xff);
	OLD_DRIVE = get_default() + 0x41;
/*-----------------------------------------------------------------*/
/*  The actual programme starts here.                              */
/*-----------------------------------------------------------------*/

	pec_clear();
	printf("%s  -  Version %s\n", NAME, VERSION);
	printf("Copyright 1986 - Cogar Computer Services Pty. Ltd.\n\n");
	printf("-----------------------------------------------------\n");
	printf("This programme checks the Disk Parameter Block of the\n");
	printf("nominated disk and prints out the information it gets\n");
	printf("from the DPB plus a copy of the Data Map.\n");
	printf("This shows 1 [one] for each Allocation Block in use\n");
	printf("and 0 [zero] for an unused Allocation Block.\n");
	printf("-----------------------------------------------------\n");
	line();
/*-----------------------------------------------------------------*/
/*  Check which drive to examine.                                  */
/*-----------------------------------------------------------------*/

	if(argc != 2)		/* See if any drive was given      */
	{
	printf("Nominate the drive to be checked  A, B, C.... ");
	DRIVE = toupper(getchar());
	}
	else DRIVE = toupper(argv[1][0]);

	reset_dsk();		/* Load dpb's into memory          */
	if(select_dsk(DRIVE) != 0)
		exit();             /* Select the drive to be done */
/*-----------------------------------------------------------------*/
/*  Now get the disk parameters for this drive.                    */
/*-----------------------------------------------------------------*/

	THIS = dpb_adr();
	BLOCK = 128 << THIS->BSH;  /* Allocation Block in bytes    */
	BLK_TRK = (THIS->SPT)*128/BLOCK;  /* Blocks per track      */
	BLOCKS = THIS->DSM + 1;
/*-----------------------------------------------------------------*/
/*  And print the information to screen.                           */
/*-----------------------------------------------------------------*/
	pec_clear();
	printf("%s  -  Version %s\n", NAME, VERSION);
	printf("Copyright 1986 - Cogar Computer Services Pty. Ltd.\n\n");
/*-----------------------------------------------------------------*/

	printf("The disk parameter block for Drive  %c  is - \n\n", DRIVE);
	printf("Allocation vector AL0       - %d \t",THIS->AL0);
	printf("Allocation vector AL1       - %d\n",THIS->AL1);
	printf("Allocation Block (bytes)    - %d \t", BLOCK);
	printf("Block mask                  - %d\n",THIS->BLM);
	printf("Block shift factor          - %d \t",THIS->BSH);
	printf("Blocks per track            - %d\n", BLK_TRK);
	printf("Check area size             - %d \t",THIS->CKS);
	printf("Drive capacity (Blocks)     - %d\n",BLOCKS);
	printf("Directory size (Entries)    - %d \t",THIS->DRM + 1);
	printf("Extent mask                 - %d\n",THIS->EXM);
	printf("Records per track           - %d \t",THIS->SPT);
	printf("Records per block           - %d\n", BLOCK/128);
	printf("Reserved tracks             - %d \t",THIS->OFF);

	if((LAST_BLOCK = (THIS->DSM + 1) << THIS->BSH)%THIS->SPT != 0)
		NO_TRKS = LAST_BLOCK/THIS->SPT + 1;
	else NO_TRKS = LAST_BLOCK/THIS->SPT;
	NO_TRKS = NO_TRKS + THIS->OFF; /* Add the offset     */
	LAST_TRACK = NO_TRKS - 1; /* Numbered from zero         */
	printf("Last Track No.              - %d\n",LAST_TRACK);
/*-----------------------------------------------------------------*/
/* Now print out the bit map for the active drive.                 */
/*-----------------------------------------------------------------*/
	if(BLOCKS%8 == 0)
		BYTE_COUNT = BLOCKS/8;
	else BYTE_COUNT = BLOCKS/8 + 1;

	MAP_ADR = get_map();
	for(i = 0; i < 75; i++)
		printf("-");
	printf("\n");
	printf("The Allocation Block Table is -\n\n");
	for(i = 0; i < BYTE_COUNT; i++)
	{
		print_byte(MAP_ADR[i]);
		putchar(SPACE);
		if((i + 1)%8 == 0)
			putchar('\n');
	}
	line();
/*-----------------------------------------------------------------*/
/*  Before closing, make sure the User No. and the Drive No.
    return to original settings.                                   */
/*-----------------------------------------------------------------*/

	user_id(OLD_USER);
	if(select_dsk(OLD_DRIVE) != 0)
	{
		printf("\nUnable to return to original Drive.");
		exit();
	}
/*-----------------------------------------------------------------*/
/*  Ask if any further utilities are to be run.                    */
/*-----------------------------------------------------------------*/
	printf("Do you want to run any more utilities  -  Y/N");
if((FD = open("UTIL.COM", 0)) != -1 && (c = toupper(getchar()) == 'Y'))
	{
		close(FD);
		exec("UTIL");
	}
	else printf("\nReturning to CP/M.");

}
/*-----------------------------------------------------------------*/
/*  Sub-routines used in this programme.                           */
/*-----------------------------------------------------------------*/

unsigned get_map()
{
	return(bdos(27, 0));
}
/*-----------------------------------------------------------------*/

print_byte(c)
char c;
{
	int i;
	char bin_display[9];
	bin_display[8] = '\0';	/* Null terminated                 */

	for(i = 0; i < 8; i++)
	{
		if(c & 0x01)
			bin_display[7 - i] = '1';
		else bin_display[7 - i] = '0';

		c = c >> 1;
	}
	printf("%s", bin_display);
}
/*-----------------------------------------------------------------*/

------------------------*/
/* Now 