
/*** vtype filespec [t] ***  Source for DeSmet C, Rev. 1.7 */

#define CR '\015' /* code for carriage return (in octal) */
#define ESC '\033'/* code for escape character (in octal) */

main(argc,argv)	  /* Display a text file on the console with tabs expanded */
int  argc;		  /* to spaces.  Also, pause and restart on user keypress. */
char *argv[];	  /* The number of spaces per tab may be specified after   */
{				  /* the filespec.  Example: vtype mprog.c 4 <enter>.  The */
				  /* range is 1 to 20 spaces per tab and the default is 8. */
				  /* Output may be aborted at any time by pressing [Esc].  */
				  /* - PUBLIC DOMAIN software -  Vincent T. Bly  12/9/1983 */

	int   i, j, e, tab;
	int   s_file, col_count;
	char  c, key, buffer[256];

	if (argc < 2) {					/* abort if no filespec */
		puts("* Need filespec *\n");
		exit(1);
	}
	s_file = fopen(argv[1], "r");
	if (s_file == 0) {				/* abort if can't open file */
		printf("* Cannot Open: %s *\n", argv[1]);
		exit(1);
	}
	tab = 8;						/* set default tab expansion to 8 */
	if (argc > 2) {
		i = atoi(argv[2]);
		if ((i >= 1) && (i <= 20))	/* if filespec followed by a number, */
			tab = i;				/* set tab expansion to "i" spaces/tab */
	}
	do {
		e = fguts(buffer, s_file);		/* get a line from the file */
		i = 0;							/* (fguts equivalent to fgets) */
		col_count = 0;
		while ((c = buffer[i++]) != '\0') {
			if (c == '\t') {			/* if character is a tab */
				for (j = 0; j < (tab - (col_count % tab)); j++)
					putchar(' ');		/* expand tab to "tab" spaces */
				col_count += j;			/* and update column count */
			} else if (c != CR) {		/* else, if not a CR, */
				putchar(c);				/* print character and */
				++col_count;			/* increment column count */
			}
		}
		key = csts();					/* check keyboard */
		if (key == ESC)
			break;						/* if ESC key pressed, abort output */
		else if (key != '\0') {			/* if any other key pressed, pause */
			ci();						/* remove keystroke from buffer */
			key = ci();					/* wait for next keystroke */
			if (key == ESC)
				break;					/* if ESC key pressed, abort output */
		}
	} while (e != -1);					/* loop until end of file */
	close(s_file);						/* close it & return to DOS */
}

fguts(buffer, file)		/* This is a replacement for fgets() for DeSmet C, */
char  buffer[];			/* version 1.7 that DOES return -1 for EOF.  Later */ 
int	  file;				/* revisions of DeSmet C and most other compilers  */ 
{						/* can eliminate this function and use fgets().    */
	int  i = 0;
	int	 j = 0;

	while ((j = getc(file)) != -1) { /* set j to file char & check for EOF */
		buffer[i++] = j;			 /* put char in buffer & bump pointer */
		if (j == 10)				 /* break loop if char was a linefeed */
			break;					 /* (indicates end of current line) */
	}
	buffer[i] = '\0';				 /* terminate with end-of-string marker */
	return(j);						 /* return value of last char read */
}

/* NOTES:
     The csts() function returns the scan code from a keyboard input if a
 key has been pressed.  It returns '\0' if no key has been pressed.  It does
 not remove the keystroke from the keyboard buffer.
     The ci() function returns the scan code from a keyboard input, but waits
 if no key has been pressed.  It does remove the keystroke from the buffer.
	 The main do loop expands tabs and prints all other characters--except 
 carriage returns.  The carriage return should not be printed, since putchar()
 will expand the following line feed into a carriage return/line feed.
	 When redirecting the output of VTYPE to a disk file under MS-DOS 2.0,
 the End-OF-File termination will be slightly abnormal.  Some text handling
 programs, such as VEDIT, may display a string of "^@"s at the end of the
 file.  If you encounter this, simply delete the string of "^@"s and re-save
 the file. */