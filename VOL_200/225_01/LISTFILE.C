/*-----------------------------------------------------------------*/
/*  FILE NAME:      LISTFILE.C
    ---------
    VERSION:        1.4
    -------
    WRITTEN:        6th July, 1986
    -------
    MODIFIED:       13th July, 1986
    --------        14th July, 1986
                    1st August, 1986  -  Specify start page
                    28/11/86  for UTIL programme                   */

/*  COPYRIGHT:      Cogar Computer Services Pty. Ltd.              */
/*  ---------                                                      */
/*                                                                 */
/*  NOTE:           This programme is specifically written for     */
/*  ----            BDS 'C' running under CP/M 2.2 and may need    */
/*                  to be modified for other C's.                  */
/*                                                                 */
/*  PURPOSE:        Will list a text file on the line printer      */
/*  -------         with page numbers and headings (the file name) */
/*                  with a standard page length of 60 printed      */
/*                  lines.   Note the equates may need to be       */
/*                  changed if you are not using 66-line paper.    */
/*                                                                 */
/*  FUNCTIONS:      LISTC         Found in DEFF3                   */
/*  ---------       ENTAB                                          */
/*                                                                 */
/*-----------------------------------------------------------------*/
#include <bdscio.h>	/* See note, above                         */
#include <pec.h>	/* Required for this file                  */
/*-----------------------------------------------------------------*/
#define VERSION "1.4"	/* The current Version No.                 */
/*-----------------------------------------------------------------*/
/*  SPECIFIC CONSTANTS                                             */
/*-----------------------------------------------------------------*/
#define SECTORS 64	/* Sectors in an 8K printer buffer         */
#define PAGE_LENGTH 66	/* For US sized paper                      */
#define PAGE_TOP 5	/* No. of lines to leave after header      */
#define PAGE_BOTTOM 10	/* No. of lines to skip after last line    */
#define	LINES PAGE_LENGTH-(PAGE_TOP+PAGE_BOTTOM) /* lines per page */
#define EOF 26		/* CP/M end of file (?)  CONTROL-Z         */
/*-----------------------------------------------------------------*/
int start_page;		/* The page from which to print    */
/*-----------------------------------------------------------------*/
main(argc, argv)
char *argv[];
int argc;
{
	int i, j, k, n, page_no;	/* counters                */
	int TAB_WIDTH;
	TAB_WIDTH = 8;		/* Normal tab size                 */
	int c;				/* for listing             */
	char dma[128];	/* The DMA buffer for BDOS functions       */
	char printer_buf[SECTORS*128]; /* The printer buffer       */
	char *list_char;	/* Pointer to listing character    */
	char file_name[15];
	char type[4];		/* To see what is the file type.   */
	int FD; 	/* The file descriptor                     */
	int CPM;	/* To check the CP/M Version number        */
	char DRIVE;	/* The active drive                        */
	char OLD_DRIVE;	/* The drive at start of programme         */
	char OLD_USER;	/* The User No. at start of programme      */
	char USER;	/* The User No. for this programme         */
/*-----------------------------------------------------------------*/
/*	The programme starts here                                  */
/*-----------------------------------------------------------------*/
	pec_clear();	/* clear the screen                        */
	header();
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
/*  Now check the command line to see if a file name was entered.  */
/*-----------------------------------------------------------------*/
	if(argc != 2) {
	printf("We need to know the name of the file you want to\
 list.\n");
	printf("Give this in the form - \n\n");
	printf("          [d:]filename\n\n");
	printf("where 'd:' is an optional drive name and filename is\
 specific.\n\n");
	gets(file_name);
	}

	else strcpy(file_name, argv[1]);
	up_str(file_name);	/* Make sure is upper case         */

	if(file_name[1] == ':')    /* Drive name given             */
		DRIVE = file_name[0];
	else DRIVE = OLD_DRIVE;
/*-----------------------------------------------------------------*/
/*  Check that the selected drive is available/on-line.   If not
    then terminate the programme.   You may need to add a message
    about what is going on if your version of CP/M doesn't do
    this automatically, as mine does.                              */
/*-----------------------------------------------------------------*/
	if(select_dsk(DRIVE) != 0)
		exit();
/*-----------------------------------------------------------------*/
/*  A file name has been obtained but, as this programme is only   */
/*  ASCII files, we should at least check for things like "COM"    */
/*  and "CRL" and "HEX" files.   Terminate the programme if any    */
/*  of these is found to have been specified.                      */
/*-----------------------------------------------------------------*/
	i = j = 0;
	while((c = file_name[i]) != '.' && i < 15)
		i++;
	i++;		/* Skip over the period                    */
	while(c != '\0' && i < 15)
	{
		type[j] = file_name[i];
		i++;
		j++;
	}

	if(!strcmp("COM",type))
		message4(type);
	else if(!strcmp("CRL",type))
		message4(type);
	else if(!strcmp("HEX",type))
		message4(type);
	else if(!strcmp("INT", type))
		message4(type);
	else if(!strcmp("OBJ", type))
		message4(type);
	else printf("\nWhat page will I start from - \n");
/*-----------------------------------------------------------------*/
/*  Get the starting page No.                                      */
/*-----------------------------------------------------------------*/
	scanf("%d", &start_page);
	line();
	printf("Now listing -  %s", file_name);
/*-----------------------------------------------------------------*/
/*  Now open the nominated file                                    */
/*-----------------------------------------------------------------*/
	if((FD = open(file_name, 0)) == -1)
	{
		printf("\nUnable to open  %s  file.", file_name);
		user_id(OLD_USER);
		if(select_dsk(OLD_DRIVE) != 0)
			printf("\nUnable to return to starting drive.");
		exit();
	}

	else list_char = printer_buf;	/* point to buffer */
/*-----------------------------------------------------------------*/
/*  File has been opened, so print the page top                    */
/*-----------------------------------------------------------------*/
	page_no = 1;		/* Starting value                  */
	if(page_no >= start_page)
		put_top(file_name,page_no);
	n = 0;	/* Zero the line counter                           */
	k = 0;	/* Zero line-character counter                     */
	list_char = printer_buf;	/* Point to data to list   */

	while((i = read(FD,printer_buf,SECTORS)) != 0)
	{
		j = 0;	/* Zero the character counter              */

		while((c = *list_char) != EOF && j < i*128)
		{
			k++;	/* Increment character counter     */
			if(page_no < start_page)
				putchar(c);
			else if( c == HT)
				k = entab(k, TAB_WIDTH);
			else listc(c);

			if(c == '\n')
			{
				n++;
				k = 0;
			}
/*-----------------------------------------------------------------*/
/*  Check for the end of page and go to next, if so                */
/*-----------------------------------------------------------------*/
			if(n == LINES)
			{
			if(page_no >= start_page)
				put_bottom();

			n = 0;	/* Re-set the counters     */
			k = 0;
			page_no = page_no + 1;
			if(page_no >= start_page)
				put_top(file_name,page_no);
			}
			list_char++;
			j++;
		if( k == 80)
			k = 0;	/* Re-set the counter             */
		}
	list_char = printer_buf;	/* Re-set the pointer      */
	}
	close(FD);
	if(n < LINES)
		last_page(n);
	else printf("\nListing finished.\n\n");
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
if((FD = open("UTIL.COM", 0)) != 0 && (c = toupper(getchar())) == 'Y')
{
	close(FD);
	exec("UTIL");
}
}
/*----------------------END OF MAIN ROUTINE------------------------*/
/*                                                                 */
/*  SUBROUTINES USED IN THIS PROGRAMME                             */
/*-----------------------------------------------------------------*/
header()
{
	printf("File listing programme - LISTFILE Version  %s\n\
by Cogar Computer Services Pty. Ltd.\n", VERSION);
 	printf("Copyright 1986 - all rights reserved.\n\n");
}
/*-----------------------------------------------------------------*/
message4(str)
char str[4];
{
	printf("\nThis programme is unable to handle  %s  files.", str);
	exit();
}
/*-----------------------------------------------------------------*/
put_top(name,count)
char *name;
int count;
{
	int i;		/* Name counter                    */
	int c;		/* To print out name                       */

	for(i = 0; c != '\0'; i++)
		listc(c = name[i]);	/* Print file name         */
	for(i = 0; i < (80 - strlen(name)) - 20; i++)
		listc(SPACE);
	lists("Page: \0");
	listd(count);
	for(i = 0; i < 5; i++)
	{
		listc(CR);	/* Space down 5 lines              */
		listc(LF);
	}
}
/*-----------------------------------------------------------------*/
put_bottom()
{
	int i;
	for(i = 0; i < PAGE_BOTTOM; i++)
	{
		listc(CR);	/* Space down five lines           */
		listc(LF);
	}
}
/*-----------------------------------------------------------------*/
last_page(line_count)
int line_count;
{
	int FD, z;
	char c;
	listc(CR);

	for(z = 0; z < (LINES - line_count + PAGE_BOTTOM); z++)
	{
		listc(LF);	/* Space down to bottom of page    */
	}
	listc(LF);

	printf("\n\nListing finished.\n\n");
	printf("\nDo you want to run another utility - Y/N.");
if((FD = open("UTIL.COM", 0)) != 0 && (c = toupper(getchar())) == 'Y')
{
	close(FD);
	exec("UTIL");
}
	exit();
}
/*----------------------END OF SUBROUTINES-------------------------*/
 = open("UTIL.COM", 0)) != 0 && (c = toupper(getchar())) == 'Y')
{
	