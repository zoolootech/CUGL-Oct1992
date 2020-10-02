/*-----------------------------------------------------------------*/
/*  PROGRAMME  ==>  SECREAD

    This will read a nominated sector on a disk and display the
    information in either ASCII or Hex, depending on which is
    appropriate.

    Written:  21/01/86
    -------
    Updated:  15/11/86
    -------
    Version:  1.1
    -------
    Copyright 1986 - Cogar Computer Services Pty. Ltd.             */
/*-----------------------------------------------------------------*/
#include <bdscio.h>	/* Note: Programme written for BDS C       */
#include <pec.h>

main(argc, argv)
int argc;
char *argv[];
{
/*******************************************************************/
/*  Space reserved for variables used in programme                 */
/*******************************************************************/

	int i, j, k, FD;
	char c;
	int TRACK, SECTOR, PHYS_SEC;
	int LAST_TRACK, LAST_SECTOR;
	char dma_buf[128];
	char DRIVE;		/* The active drive                */
	char OLD_DRIVE;		/* The starting drive No.          */
	struct fcb *FCB1;	/* The active File Control Block   */
	struct dpb *THIS;	/* The Disk Parameter Block        */
	char **skew_table;	/* Pointer to a pointer decl.      */
/*******************************************************************/
/*  Start of programme - check for the active drive                */
/*******************************************************************/

	pec_clear();
	header();
	printf("-----------------------------------------------------\n");
	printf("This programme will read any Track/Sector of a disk in\n");
	printf("both hex and ASCII format.   To use it just follow the\n");
	printf("prompt messages which follow.\n");
	printf("-----------------------------------------------------\n");
	line();

	if(argc != 2)
	{
		printf("Enter the DRIVE you wish to check A, B, C....");
		DRIVE = toupper(getchar());
	}
	else if(argc == 2)
		DRIVE = toupper(argv[1][0]);
	lines(2);
/*-----------------------------------------------------------------*/
/*  Save the starting Drive No.                                    */
/*-----------------------------------------------------------------*/
	OLD_DRIVE = get_default() + 0x41;

/*-----------------------------------------------------------------*/
/*  Select the nominated drive for all disk I/O.                   */
/*-----------------------------------------------------------------*/
	if(!(skew_table = seldsk(DRIVE)) )
		exit();		/* Invalid disk, so quit           */
	if(select_dsk(DRIVE) != 0)
		exit();
/*-----------------------------------------------------------------*/
/*  Get the disk parameters needed in programme.                   */
/*-----------------------------------------------------------------*/
	THIS = dpb_adr();	/* Point to disk parameter block   */
	LAST_SECTOR = THIS->DSM;	/* Starting value          */
	for(i = 0; i < THIS->BSH; i++)
	{
		LAST_SECTOR = LAST_SECTOR + LAST_SECTOR;
	}
	LAST_TRACK = (LAST_SECTOR/THIS->SPT) + THIS->OFF;

	printf("Now enter the TRACK to check.\n");
	printf("The value must lie in the range 0 to %d\n", LAST_TRACK);
MARK1:	scanf("%d", &TRACK);
	lines(2);
	if(TRACK < 0 || TRACK > LAST_TRACK)
	{
		printf("This TRACK is out of range...try again.\n");
		goto MARK1;
	}
	printf("Now enter the SECTOR to read.\n");
	printf("The value must lie in the range 0 to %d\n", THIS->SPT -1);
MARK2:	scanf("%d", &SECTOR);
        lines(2);
        if(SECTOR < 0 || SECTOR > THIS->SPT - 1)
        {
        	printf("This SECTOR is out of range...try again.\n");
		goto MARK2;
        }

MARK3:	set_dma(&dma_buf[0]);		/* The DMA buffer in use   */

	set_trk(TRACK);	                 /* The track to read      */
/*-----------------------------------------------------------------*/
/*  Now get the physical sector to read.                           */
/*-----------------------------------------------------------------*/
	PHYS_SEC = biosh(16, SECTOR, *skew_table);
	set_sec(PHYS_SEC);		/* The sector to read      */
	printf("For Drive No. - %c\n", DRIVE);

	if(read_sec() == 1)	/* Read sector into DMA buffer     */
	{
	printf("Unspecified read error, perhaps track damaged.\n");
printf("Reading Track No. %d Sector - %d Physical Sec - %d\n", TRACK, SECTOR, PHYS_SEC);
	line();
	showsec(&dma_buf[0]);
	lines(2);
	}
else if(read_sec() == 0)
{
printf("Reading Track No. %d Sector - %d Physical Sec - %d\n", TRACK, SECTOR, PHYS_SEC);
	lines(2);
	showsec(&dma_buf[0]);
	lines(2);
}
/*-----------------------------------------------------------------*/
/*  Now ask if there are any more sectors to display.              */
/*-----------------------------------------------------------------*/
	printf("If you want to see either the next sector or the previous\n");
	printf("sector, enter either '+' (for next) or '-' (for last).\n");
	printf("Else just enter <RETURN>\n\n");
	if((c = getchar()) == '+')
	{
		SECTOR++;
		if(SECTOR > THIS->SPT -1)
		{
			TRACK++;
			SECTOR = 0;
			if(TRACK > LAST_TRACK)
				TRACK = 0;
		}
		pec_clear();
		header();
		goto MARK3;
	}
	else if(c == '-')
	{
		SECTOR--;
		if(SECTOR < 0)
		{
			TRACK--;
			SECTOR = THIS->SPT - 1;
			if(TRACK < 0)
				TRACK = LAST_TRACK;
		}
		pec_clear();
		header();
		goto MARK3;
	}
	else if(c == CR)
		putchar('\n');
	else if(c == LF)
		putchar('\n');
	else printf("\nUnknown response.\n\n");
	printf("Do you want to change track/sector - 'Y/N' :- ");
	if(toupper(getchar()) == 'Y')
		main(argc, argv);
/*-----------------------------------------------------------------*/
/*  Restore the original Drive No.                                 */
/*-----------------------------------------------------------------*/
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
else if(c == 'N' || c == CR)
printf("\n\nReturning to CP/M.");
}
/*******************************************************************/
/*  The subroutines used in programme.                             */
/*******************************************************************/

void header()
{
	printf("SECREAD - Version 1.1 of November, 1986\n");
	printf("Copyright 1986 - Cogar Computer Services Pty. Ltd.\n\n");
}
/*-----------------------------------------------------------------*/

showsec( record)
char *record;
{
	int i, j;

	for(j = 0; j < 4; j++)
	{
/*-----------------------------------------------------------------*/
/*  Do the first 16 characters in Hex and ASCII.                   */
/*-----------------------------------------------------------------*/
	for(i = j*32; i < j*32 + 16; i++)
	{
		prt_hex( record[i]);
		if((i + 1)%8 == 0)
			putchar(SPACE);	/* Every 8 characters      */
	}
	printf("  ");		/* Two spaces                      */
	for(i = j*32; i < j*32 + 16; i++)
	{
		if(record[i] > 31 && record[i] < 127)
			putchar(record[i]);
		else putchar('.');
	}
/*-----------------------------------------------------------------*/
/*  Now do the next 16 characters in the 32-character sequence.    */
/*-----------------------------------------------------------------*/
	line();
	for(i = j*32 + 16; i < j*32 + 32; i++)
	{
		prt_hex( record[i]);
		if((i + 1)%8 == 0)
			putchar(SPACE);	/* Every 8 characters      */
	}
	printf("  ");		/* Two spaces                      */
	for(i = j*32 + 16; i < j*32 + 32; i++)
	{
		if(record[i] > 31 && record[i] < 127)
			putchar(record[i]);
		else putchar('.');
	}
	line();
	}
}
/*-----------------------------------------------------------------*/---------*/
	if(select_dsk(OLD_DRIVE) != 0)
	{
		printf("\nUnable to return to starting drive."