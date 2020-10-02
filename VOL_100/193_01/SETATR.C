/*-----------------------------------------------------------------*/
/*  PROGRAMME  ==>  setatr.c                                       */
/*  VERSION:        1.2      Written: 13th July, 1986              */
/*  -------                  -------  Phil. E. Cogar               */
/*  Will set the nominated byte of the filename to give -          */
/*                                                                 */
/*       R/O  -  read only status                                  */
/*       R/W  -  read/write status                                 */
/*       SYS  -  system file (not shown in directory)              */
/*       DIR  -  directory file                                    */
/*       ARC  -  file has been archived already                    */
/*       CPY  -  file can be copied by archive programme           */
/*                                                                 */
/*  This programme uses the CP/M functions -                       */
/*                                                                 */
/*                 17   search first                               */
/*                 18   search next                                */
/*                 26   set DMA address                            */
/*                 30   set file attributes                        */
/*                                                                 */
/*  to do the required work.                                       */
/*  Note that it requires "DEFF3" for the BDOS functions which     */
/*  form part of the compiled programme.  Also DEFF4.              */
/*  Copyright 1986 - Cogar Computer Services Pty.Ltd.              */
/*-----------------------------------------------------------------*/
#include <bdscio.h>	/* NOTE: This programme is specific to     */
			/* ----  BDS 'C'.   If you use a different */
			/*       C then you must change this line. */
#include <pec.h>	/* Needed for this file                    */
/*-----------------------------------------------------------------*/

main(argc,argv)
short argc;
char *argv[];
{
	short count, file_count, i, j, n;	/* counters                */
	short offset;
	char name[14];	/* The file name buffer                    */
	char fcb[36];	/* The file control block  used throughout */
	char mode[4];	/* The file mode                           */
	short drive;	/* To save the current drive               */
	char name_buf[3200];	/* Use to hold the file name       */
 				/* Note present limit of 100       */
	char dma[128];	/* The DMA buffer for CP/M to use          */
/*-----------------------------------------------------------------*/
/*	Start of the programme                                     */
/*-----------------------------------------------------------------*/
	
	offset = file_count = 0;	/* starting value          */

	pec_clear();	/* clear the screen to start               */

	set_dma(dma);	/* create a DMA buffer                     */
/*-----------------------------------------------------------------*/
/*  Now check to see what was entered from the keyboard.   If the  */
/*  file name and mode aren't specified then ask for them.         */
/*-----------------------------------------------------------------*/
	
	if(argc < 2)	/* check to see if name given              */
		put_header(name);
	else strcpy(name,argv[1]);
/*-----------------------------------------------------------------*/
/*  Save the drive name for later use                              */
/*-----------------------------------------------------------------*/
	if(name[1] == ':')
		drive = toupper(name[0]) - 'A' + 1;
	else drive = 0;
	
	setfcb(fcb,name); /* put name into file control block      */

	if(argc < 3)	/* check to see if a MODE is nominated     */
		get_mode(mode);
	else strcpy(mode,argv[2]);
	mode[3] = '\0';		/* Null-terminated */

/*-----------------------------------------------------------------*/
/*  Now let the user know what we are looking for                  */
/*-----------------------------------------------------------------*/

	printf("\nThe file mask in use is -  ");
	for(n = 1; n < 12; n++)
		putchar(fcb[n]);
	printf(" with mode = %s\n", mode);
	
/*-----------------------------------------------------------------*/
/*  Use the CP/M functions "search first" and, if required,        */
/*  "search next" to seek out matching file names from the disk    */
/*  directory.   Any that are found are then saved into the  name  */
/*   buffer.                                                       */
/*-----------------------------------------------------------------*/

	if((i = search_first(fcb)) != 255)
	{
		count = save_name(i, dma, name_buf, file_count, offset);
		file_count = count;
		offset = offset + 32;
	}
	else do_exit();


	while((i = search_next()) != 255)
	{
		count = save_name(i, dma, name_buf, file_count, offset);
		file_count = count;
		offset = offset + 32;
/*-----------------------------------------------------------------*/
/*  Note the buffer limit of 100 file names.   If this is exceeded */
/*  then the programme fails.                                      */
/*-----------------------------------------------------------------*/
		if(file_count > 99)
		{
			printf("\nWe have a limit of 100 files.\n");
			printf("Try smaller groupings.\n");
			exit();
		}
	}
		
/*-----------------------------------------------------------------*/
/*  Have now loaded all the file names in to the large (name )     */
/*  buffer and can  change the attributes in these in the order    */
/*  they were found.                                               */
/*-----------------------------------------------------------------*/

	offset = 0; /* re-set the offset to start of name buffer   */
	
	for(n = 0; n < file_count; n++)
	{
		fcb_zero(fcb);
		for(j = 0; j < 32; j++)
			fcb[j] = name_buf[offset + j];
		fcb[0] = drive;	/* Make sure proper drive used     */
		set_bit(fcb,mode);
		printf("\nHave now altered - ");
		for(j = 1; j < 12; j++)
			putchar(fcb[j]);
	offset = offset + 32;
	}

	printf("\n\nAll finished.   Return to CP/M.");
}
/*------------END OF MAIN PROGRAMME--------------------------------*/
/*  SUPPORT ROUTINES USED IN MAIN PROGRAMME                        */
/*-----------------------------------------------------------------*/

save_name(identifier, dma, buffer, count, ptr)
short identifier, count, ptr;
char dma[128], *buffer;
{
	short n;
	identifier = identifier*32; /* find the name in DMA buffer */
	for(n = 0; n < 32; n++)
		buffer[ptr + n] = dma[identifier + n];
	count++;
	return(count);
}
/*-----------------------------------------------------------------*/

fcb_zero(fcb)
char fcb[36];
{
	short n;

	for(n = 0; n < 36; n++)
		fcb[n] = '\0';
}
/*-----------------------------------------------------------------*/

get_mode(str)
char str[3];
{

printf("\n\nSay which attribute you wish to set.   The choices are -\n\n");
printf("          R/O - read only\n");
printf("          R/W - read/write\n");
printf("          SYS - system file\n");
printf("          DIR - directory file\n");
printf("          ARC - can't be archived\n");
printf("          CPY - available for archiving\n\n");
printf("Either upper case or lower case is OK.\n\n");
gets(str);

}
/*-----------------------------------------------------------------*/

put_header(name)
char name[14];
{
	printf("Enter the name of the file you wish to alter.\n");
	printf("The entry should be in the form -\n\n");
	printf("          [d:]filename\n\n");
	printf("where 'd' is the optional drive name.\n\n");
	gets(name);
}
/*-----------------------------------------------------------------*/

do_exit()
{
	printf("\nUnable to locate the named file.\n");
	exit();
}
/*------------END OF PROGRAMME-------------------------------------*/or(j = 0; j < 32; j++)
			fcb[j] = name_buf[offset + j];
		fcb[0] = drive;	/* Ma