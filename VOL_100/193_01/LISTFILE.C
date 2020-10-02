/*-----------------------------------------------------------------*/
/*  FILE NAME:      listfile                                       */
/*  VERSION:        1.2                                            */
/*  WRITTEN:        6th July, 1986                                 */
/*  MODIFIED:       13th July, 1986                                */
/*  --------        14th July, 1986                                */
/*  COPYRIGHT:      Cogar Computer Services Pty. Ltd.              */
/*                                                                 */
/*  NOTE:           This programme is specifically written for     */
/*  ----            BDS 'C' running under CP/M 2.2 and may need    */
/*                  to be modified for other C's.                  */
/*                                                                 */
/*  PURPOSE:        Will list a text file on the line printer      */
/*  -------         with page numbers and headings (the file name) */
/*                  with a standard page length of 60 printed      */
/*                  lines.   Note the equates may need to be       */
/*                  changed if you are using 66-line paper instead */
/*                  of A4, which allows 70 lines per page.         */
/*                                                                 */
/*  FUNCTIONS:      LISTC         Found in DEFF3                   */
/*  ---------       ENTAB                                          */
/*                                                                 */
/*-----------------------------------------------------------------*/
#include <bdscio.h>	/* See note, above                         */
#include <pec.h>	/* Required for this file                  */
/*-----------------------------------------------------------------*/
#define VERSION "1.2"	/* The current Version No.                 */
/*-----------------------------------------------------------------*/
/*  SPECIFIC CONSTANTS                                             */
/*-----------------------------------------------------------------*/
#define SECTORS 64	/* Sectors in an 8K printer buffer         */
#define PAGE_LENGTH 70	/* For A4 paper                            */
#define PAGE_TOP 5	/* No. of lines to leave after header      */
#define PAGE_BOTTOM 10	/* No. of lines to skip after last line    */
#define	LINES PAGE_LENGTH-(PAGE_TOP+PAGE_BOTTOM) /* lines per page */
#define EOF 26		/* CP/M end of file (?)  CONTROL-Z         */
/*-----------------------------------------------------------------*/


main(argc, argv)
char *argv[];
short argc;
{
	short i, j, k, n, page_no;	/* counters                */
	short TAB_WIDTH;
	TAB_WIDTH = 8;		/* Normal tab size                 */
	int c;				/* for listing             */
	char dma[128];	/* The DMA buffer for BDOS functions       */
	char printer_buf[SECTORS*128]; /* The printer buffer       */
	char *list_char;	/* Pointer to listing character    */
	char file_name[15];
	char type[4];		/* To see what is the file type.   */
	short file;	/* The file descriptor                     */
/*-----------------------------------------------------------------*/
/*	The programme starts here                                  */
/*	First check to see whether anything was entered from the   */
/*      console as we need a specific file name of the form -      */
/*                                                                 */
/*		[d:]filename                                       */
/*                                                                 */
/*	where "d:" is the optional drive name.                     */
/*-----------------------------------------------------------------*/
	pec_clear();	/* clear the screen                        */
	header();
	
	if(argc < 2) {
	printf("We need to know the name of the file you want to\
 list.\n");
	printf("Give this in the form - \n\n");
	printf("          [d:]filename\n\n");
	printf("where 'd:' is an optional drive name and filename is\
 specific.\n\n");
	gets(up_str(file_name));
	}
	
	else if(argc > 2) {
	printf("Too many parameters given.   Only the filename is\
 required.\n");
	printf("Give this in the form - \n\n");
	printf("          [d:]filename\n\n");
	printf("where 'd:' is an optional drive name and filename is\
 specific.\n");
	exit();
	}
	
	else strcpy(file_name, argv[1]);

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
	else printf("\nNow listing -  %s", file_name);

/*  Now have a file name nominated so we should open this file     */

	if((file = open(file_name, 0)) != -1)
		list_char = printer_buf;	/* point to buffer */
	else message3();

/*  File has been opened, so print the page top                    */

	page_no = 1;		/* Starting value                  */
	put_top(file_name,page_no);
	n = 0;	/* Zero the line counter                           */
	k = 0;	/* Zero line-character counter                     */
	
	while((i = read(file,printer_buf,SECTORS)) != 0)
	{
		j = 0;	/* Zero the character counter              */

		while((c = *list_char) != EOF && j < i*128)
		{
			k++;	/* Increment character counter     */
			if( c == HT)
				k = entab(k, TAB_WIDTH);
			else listc(c);
			if(c == '\n')
			{
				n++;
				k = 0;
			}
/*  Check for the end of page and go to next, if so                */

			if(n == LINES)
			{
				put_bottom();
				n = 0;	/* Re-set the counters     */
				k = 0;
				page_no = page_no + 1;
				put_top(file_name,page_no);
			}
			list_char++;
			j++;
		if( k == 80)
			k = 0;	/* Re-set the counter             */
		}
	list_char = printer_buf;	/* Re-set the pointer      */
	}
	close(file);
	if(n < LINES)
		last_page(n);
	else printf("\nListing finished, return to CP/M.");
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
message3()
{
	printf("\nUnable to open the file.");
	exit();
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
short count;
{
	short i;		/* Name counter                    */
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
	short i;
	for(i = 0; i < PAGE_BOTTOM; i++)
	{
		listc(CR);	/* Space down five lines           */
		listc(LF);
	}
}
/*-----------------------------------------------------------------*/
last_page(line_count)
short line_count;
{
	short z;
	listc(CR);

	for(z = 0; z < (LINES - line_count + PAGE_BOTTOM); z++)
	{
		listc(LF);	/* Space down to bottom of page    */
	}
	listc(LF);
	printf("\n\nListing finished.  Return to CP/M.");
	exit();
}
/*----------------------END OF SUBROUTINES-------------------------*/
ter      */
	}
	close(file);
	if(n < LINES)
		last_page(n);
	else printf("\nListing 