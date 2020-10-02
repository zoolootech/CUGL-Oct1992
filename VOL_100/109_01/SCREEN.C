/*
    SCREEN:   Currently setup for TRS-80 Model II operation.

    Screen is a raw I/O routine with direct video drive (flashwriter),
    intended for quickly viewing source files (ASCII character files).
    The F2 key on the TRS-80 Model II pages deeper into file.
    The BREAK key on the TRS-80 Model II provides an immediate and
    orderly return to CP/M.
    
    Usage: screen filename <cr>
    
    Bugs:  If there are exactly n*23 (n > 0) lines in a file
	   the last page will be completely blank. All data,
	   however, will have been displayed correctly.
    
    Environment: CP/M 2.2 (P&T)
	         BDS C compiler Rev. 1.42

    Author: Jack S. Bakeman, Jr.
	    1222 Inverrary Ln.
	    Deerfield, IL 60015
	    (312) 459-0565
*/

#include "bdscio.h"

#define BUFSECTS 192	/* Buffer up to 192 sectors in memory	*/
#define VIDADDR  0xf800	/* Starting address if video memory	*/
#define VIDSIZE  1920	/* Video mem. size (24 lines * 80 char)	*/
#define VLINSIZ  80	/* Number of char. per video line	*/
#define F1       0x01	/* F1 key on TRS-80 Model II	 	*/
#define F2       0x02	/* F2 key on TRS-80 Model II	 	*/
#define CR       0x0d	/* Carriage return	 	 	*/
#define BRK	 0x03	/* BREAK key on TRS-80 Model II	 	*/
#define NON_CPMEOF 1	/* Use if you expect EOF's other than ^Z*/
#define USR_EOF  NULL	/* Non-standard EOF character	 	*/
#define not !

int fd1;	 	/* File descriptor for the file	 	*/
char buffer[BUFSECTS * SECSIZ];	/* The raw data transfer buffer	*/
char vidbuf[VIDSIZE];	/* Video buffer, holds processed data 	*/

main(argc,argv)
int argc;	 	/* Arg count	 	*/
char *argv[];	 	/* Arg vector	 	*/
{
    int i, j, k;
    int cc;	 	/* Character counter for 1 line	*/
    int vidbuftotal;	/* Count of chars processed into vidbuf[] */
    int rawcount;	/* Count of chars processed from buffer[] */
    int pagenum;
    char c, firstpage, done;	/* General purpose flags	*/
    char *ptr;	 	/* General purpose pointer	*/
    char pgstr[12];	/* Holds "page nnn"	 	*/

     	 	/* Make sure exactly 1 arg was given	*/
    if (argc != 2)
     	perror("Usage: screen filename <cr>\n");

     	 	/* Try to open file; abort on error	*/
    if ((fd1 = open(argv[1],0)) == ERROR)
     	perror("Can't open: %s\n",argv[1]);

    firstpage = TRUE;
    done = FALSE;
    rawcount = 0;
    pagenum = 1;

	 	/* Move data from th floppy to the buffer	*/
    if (read(fd1, buffer, BUFSECTS) == ERROR)
	perror("Error reading: %s\n",argv[1]);

    while (not done)
	{
        vidbuftotal = cc = 0;
        for (i = rawcount; vidbuftotal < VIDSIZE - VLINSIZ; i++)
    	    {

     	 	/* Raw buffer has been processed & file is not done	*/
	    if (i >= BUFSECTS * SECSIZ)
	        {
                if (read(fd1, buffer, BUFSECTS) == ERROR)
	            perror("Error reading: %s\n",argv[1]);
	        i = rawcount = 0;
	        }

     	 	/* Check for end of file (EOF)	*/
#ifndef NON_CPMEOF

    	    if ((c = buffer[i]) == CPMEOF)

#else
#ifdef NON_CPMEOF

	    if ((c = buffer[i]) == CPMEOF || c == USR_EOF)

#endif
	 	{
	 	done = TRUE;
	        while (vidbuftotal < VIDSIZE)
	 	    vidbuf[vidbuftotal++] = ' ';
	 	}

     	 	/* Not at EOF so make video buffer from raw ASCII	*/
    	    else switch (c)
    	        {
    	        case '\t':	/* Tab: insert up to 8 spaces	*/
    	 	    for (j = cc, k = cc % 8; j < VLINSIZ && k < 8; j++,k++)
	 	        {
    	 	        vidbuf[vidbuftotal++] = ' ';
	 	        cc++;
	 	        }
    	 	    break;
    	        case CR:	/* Carriage return: merely incr. raw buffer */
    	 	    break;
    	        case '\n':   /* Line feed: fill remainder if line w/ spaces */
    	 	    for (j = cc; j < VLINSIZ; j++)
    	 	        vidbuf[vidbuftotal++] = ' ';
	 	    cc = 0;	 	/* Reset cc char count of line	*/
    	 	    break;
    	        default:	/* Most characters are processed here	*/
    	 	    vidbuf[vidbuftotal++] = c;
	 	    cc++;
    	 	    break;
    	        }
    	    }
	rawcount = i;	 	/* Bump rawcount */

	 	 	 	/* Bottom line data */
	sprintf(pgstr, "page %d", pagenum);
	if (not done)	 	/* Put info line on line 24	*/
	    {
	    for (i = VIDSIZE - VLINSIZ; i < VIDSIZE; i++)
	        vidbuf[i] = '-';
	    for (i = 1844, ptr = pgstr; *ptr != NULL; i++, ptr++)
	        vidbuf[i] = 0x80 | *ptr;
	    for (i = 1904, ptr = argv[1]; *ptr != NULL; i++, ptr++)
	    vidbuf[i] = 0x80 | *ptr;
	    }
	else	 	 	/* Put info line on line 23	*/
	    {
	    for (i = VIDSIZE - 2*VLINSIZ; i < VIDSIZE - VLINSIZ; i++)
	        vidbuf[i] = '-';
	    for (i = 1764, ptr = pgstr; *ptr != NULL; i++, ptr++)
	        vidbuf[i] = 0x80 | *ptr;
	    for (i = 1824, ptr = argv[1]; *ptr != NULL; i++, ptr++)
	    vidbuf[i] = 0x80 | *ptr;
	    }

 	 	/* Video buffer has know been made so move it */
        if (firstpage)
	    {
     	    call(0x40,NULL,NULL,0x1500,NULL);   /* Enable video RAM	*/
            movmem(vidbuf,VIDADDR,VIDSIZE); 	/* Data to screen mem	*/
      	    call(0x40,NULL,NULL,0x1600,NULL);   /* Disable video RAM	*/
	    firstpage = FALSE;	 	 	/* Not firstpage now	*/
	    pagenum++;
    	    }
        else while ((c = bios(3)) != F2 && c != BRK) /* Raw KB input	*/
	    ;
	if (c == F2)	/* Descend one page deeper into file	 	*/
	    {
     	    call(0x40,NULL,NULL,0x1500,NULL);   /* Enable video RAM	*/
            movmem(vidbuf,VIDADDR,VIDSIZE); 	/* Data to screen mem	*/
      	    call(0x40,NULL,NULL,0x1600,NULL);   /* Disable video RAM	*/
	    pagenum++;
    	    }
	if (c == BRK) exit();	/* Not at EOF but user wants out	*/
	}

     	 	/* File has been totally displayed. Now close the file: */
    close(fd1);
    printf("\033Y\066\076Screen complete");
}

perror(format,arg)    /* Print error message and abort	*/

    printf(format, arg);	/* Print message	*/
    exit();	 	 	/* Return to CP/M	*/
}
