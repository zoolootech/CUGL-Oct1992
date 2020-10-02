/* LOGONR.C  VERS:- 03.00  DATE:- 09/26/86  TIME:- 09:39:11 PM */
/*
cp b:$1 -e -s0
z80asm $1.bb
slrnk b:$1, b:sys1call/s, b:$1/n/e/*
*/
Description: 

Logon routine, for ZCPR2 system:
	load SYSIO.IO at iobuf;
	load NAMES.DIR at ndbase;
	display ruler line and prompts for operator logon information;
	fill time and date arrays from Hayes clock;
	display time, date, message, etc on crt;
	timestmp logon information given by the operator;
        append it to the LOG file.

Special requirements:
	sysicall.mac/rel = call and return from a program, here used in
		initializing the system through a call to bios.

By J.A. Rupley, Tucson, Arizona
Coded for the Eco-C Compiler, version 3.40
*/


#include <stdio.h>

#include <ctrlcnst.h>

#define BUF_SIZE	80		/* Screen parameters used in typer*/
#define SCREEN_HEIGHT	23

char line_buf[BUF_SIZE];		/* Buffer sent to printer or file */

char time[] = "hh:mm:ss XM \0";
char date[] = "mm/dd/yy \0";
char day[] = "Wednesday \0";
char month[] = "September \0";
char date_mes[80] = "";
char log_mes[80] = "";


main()
{
	int fclose(),putc();
	void log_typer(), printf(), fputs(), exit();
	void chrono(), ldsysio(), ldnames(),user_mes();

	FILE *fptr, *fopen();		/* Buffered IO file pointer	*/


				/* Reverse video			*/
	INTOREV;
Š				/* Load SYSIO.IO at IOBUF
					End with call to TINIT
					to initialize new io drivers	*/
	ldsysio();


				/* Load NAMES.DIR at NDBASE, with
					appropriate editing of file	*/
	ldnames();



				/* Fill line_buf with one-line entry for
					logon description.
					Entry ended by CR, LF, or ^Z.
					Abort logon process with ^C.	*/
	log_typer();			



				/* Fill time and date arrays		*/
	chrono();



				/* Open the LOG file			*/

	if ((fptr = fopen("LOG", "a")) == NULL){
		printf("\nSorry, I can\'t open the file.\n");
		exit(ERROR);
	}


				/* Append logon header and logon descriptor
					to LOG file			*/
	putc('\n', fptr);
	fputs(log_mes, fptr);
	putc('\n', fptr);
	fputs(line_buf, fptr);


					
				/* Display time and date on crt
					and display user message	*/
	CLEARS;
	INTOREV;
	puts("\n");
	puts(date_mes);

	user_mes();
	OUTAREV;


				/* Write eof, close file, and exit	*/
	putc(CPMEOF,fptr);Š	if (fclose(fptr) == ERROR) {
		printf("\n\nerror in closing file\n");
		exit(ERROR);
	}

	exit(OK);
}					/* END OF MAIN			*/

	/* page eject */


					/* Primary typer routine.
					   Create logon string in line_buf.
					   One line of input, terminated by
						CR, LF or ^Z
					   ^C aborts logon process by
						jump to wboot.		*/
void log_typer(fptr)
{
	void ruler();
	int getchar(), putchar();
	int c;
	int i, j, m, vert;		
					/* i is the line_buffer subscript
					   m is the screen image cursor positn
					   j is for loop indexing
					   vert is the vertical line counter */
	
	i = m = NULL;			/* Reset character counter	*/
	vert = 22;		   	/* Begin at line >> #1		*/
	ruler();
	CURSOR(vert,m);

	while ((c = getchar()) != EOF && c != CPMEOF){
				/* Get characters one at a time		*/

				/* Display if displayable		*/
		if (c >= SPACE && c <= 126){   	/* Displayable?		*/
			if (m >= (BUF_SIZE - 1)){	/* End of line?	*/
		           	putchar(BKSP);	/* Delete last char on crt */
				putchar(SPACE);
				putchar(BKSP);
				continue;
			}
			line_buf[i] = c; /* Put char into buffer	*/
			++m; ++i;	/* And increment pointers	*/
		}


				/* Perform various ctrl-char operations	*/
		elså iæ (ã =½ BKSP)û	/ª Backspacå			*/
			if (i == 0)	/* If beginning of line		*/
				putchar(CTRLL); /* Move right		*/
			else{		/* Not beginning of line	*/
				putchar(SPACE);	/* Write a space	*/
				putchar(BKSP); /* Backspace		*/
				--m; --i;	/* Decrement subscript	*/
			}
		}
		else if (c == CTRLU){ 	/* Cancel line_buf contents	*/
			m = m - i;	/* Delete line_buf contents on screen*/
			if (m > 0){
				CURSOR(vert,--m);Š				putchar(ESC);	/* Delete line right	*/
				putchar('T');	/* For televideo 920 	*/
				putchar(CTRLL);	/* Advance cursor 	*/
				m++;		/* And pointer		*/
			}
			else{
				CURSOR(vert, 0);/* Ditto for first colmn*/
				putchar(ESC);
				putchar('T');
				putchar(SPACE);
				CURSOR(vert, 0);
			}
			i = NULL;	/* Zero line_buf 		*/
		}
		else if (c == '\n' || c == CR){ 	/* CRLF		*/
			break;
		}
		else if (c == CTRLC)	/* Abort			*/
			exit(ERROR);

				/* Adjust cursor if necessary		*/
		if (m < 0)
			m = 0;
		else if (m > (BUF_SIZE - 1)){
			m = (BUF_SIZE - 1);
		}

		CURSOR(vert, m);
	}

					/* Terminate line and return	*/
	if (i > 78) {
		i = 78;
	}
	line_buf[i] = '\n';
	line_buf[++i] = '\0';


}					/* END OF TYPER			*/



void ruler()				/* Put ruler line at screen top	*/
{
	void puts();
	int i;

	static char *rul_mes[] = {
"\nPlease type in a login message, of the following form:\n",
"<initials>  <description of what you will do with the computer>\n",
"The message can be only one line.",
"It is terminated by  <return>  or  <line feed>  or  <ctrl-Z>.\n",
"Type  <ctrl-C>  to abort logon process, by a warm boot.\n",
"L----!----!----!----!----!----!----!----!----!----!----!----!----!----!-------R",
NULLŠ	};

	for (i = 0; rul_mes[i] != NULL; i++) {
		puts(rul_mes[i]);
	}

	OUTAREV;	
}					/* END OF RULER			*/


					/* Display message for user	*/
void user_mes()
{
	void puts();
	int i;
	static char *array[] = {
"\n\nYou are now logged into the system:\n\n",
"            drive   A:",
"            user area  0  =  directory START\n\n\n",
"If you need help, please type:\n",
"            MENU\n\n",
"When you are done, please remember to LOGOFF\n\n",
NULL
	};

	for (i = 0; array[i] != NULL; i++)
		puts(array[i]);
}					/* END OF USER_MES		*/


	/* page eject */

/*
Load NAMES.DIR file of current directory into memory at NDBASE.

NAMES.DIR, created by MKDIR.COM, is a list of 11-character entries:
     <drive letter><user number><filename, padded to 8 char with NULLS><NULL>

The area above NDBASE is structured as follows:
	NDBASE+0  =  max number of entries allowed
	NDBASE+1  =  number of entries read into area from NAMES.DIR
	NDBASE+2  =  start of first entry
		     each entry is 10 characters (the ending NULL of the
			NAMES.DIR entry is deleted
*/

ldnames()
{
	FILE *fopen();
	int getc();
	void printf(), exit();

	int char_max, num_max;
	char *px, *py;
	register FILE *fptr;
	register int c, j, k;


				/* Setup for read of NAMES.DIR		*/
	fptr = fopen("NAMES.DIR", "r");
	if (fptr == NULL) {
		printf("\n\ncannot open file NAMES.DIR-- sorry\n\n");
		exit(ERROR);
	}

				/* Setup locations filled with NAMES.DIR*/
	px = NDBASE;
	py = px + 2;			/* Start of name entries 	*/
	num_max = *px;			/* Max number names allowed	*/
	char_max = num_max * 11;

				/* Loop to read from NAMES.DIR and store
					above NDBASE.  Because of differences
					in the structure of the entries in
					memory and the file, editing is
					carried out.			*/
					/* Increment address py with j	*/
	for (j = 0; j < char_max; j++, py++) {
					/* k = ordinal of char in entry	*/
		k = j % 11;
					/* Read byte from NAMES.DIR	*/
		if ((c = getc(fptr)) == ERROR) {
			printf("\n\nerror in reading file\n\n");
			exit(ERROR);
		}Š					/* Exit on eof			*/
		if (c == EOF || c == CPMEOF)
			break;
					/* Editing of NAMES.DIR input	*/
					/* Pad with spaces, not nulls	*/
		else if (k > 2 && c == NULL)
			c = SPACE;
					/* Convert drive letter to number*/
		else if (k == NULL)
			c = c - 'A';   
					/* Exit if no filename		*/
		else if (k == 2 && c == NULL) {
			break;
		}
					/* Put edited character in memory*/
					/* Pass over 11th character	*/
		if (k == 10)
			py--;
		else
			*py = c;
	}
					/* Set NDBASE+1 at number of
						names read = number of char
						less (k = 2) for exit test
						over char per entry	*/
						
	*(px+1) = (j - 2) / 11;


				/* Print completion message		*/
	printf("\1load of NAMES.DIR completed:\n");
	printf("        %d entries read of %d entries allowed\n", *(px+1), *px);

				/* Close file and exit			*/
	if (fclose(fptr) == ERROR) {
		printf("\n\nerror in closing file\n\n");
		exit(ERROR);
	}

}				/* END OF LDNAMES			*/


	/* page eject */

/* 
Load SYSIO.IO from current directory into memory at IOBASE.
*/


ldsysio()
{
	int read(), close(), open();
	void printf(), exit();
	inô x, count, fdes;
	int *px, *py;
	char *io_buffer, *tinit;
	
				/* Extract address of IOBASE
					mix pointers to integers and integers
					to carry out proper arithmetic	*/
	px = 1;				/* WBOOT address at mem loc 1	*/	
	x = *px;			
	py  = x -3 +IOBASE_PNTR;	/* Offset from start of BIOS	*/
	io_buffer = *py;		/* Extract IOBASE		*/
	x = io_buffer;			/* Offset to obtain TINIT	*/
	tinit = x + 9;


				/* Open SYSIO.IO			*/	
	if ((fdes = open("SYSIO.IO", 0)) == ERROR) {
		printf("\n\ncould not open SYSIO.IO-- sorry\n\n");
		exit(ERROR);
	}

				/* Read SYSIO.IO.  Set count at more bytes
					than in file, 
						letting eof stop read	*/
	count = 0x1000;
	count = read(fdes, io_buffer, count);


				/* Print completion message		*/
	printf("load of SYSIO.IO completed:\n");
	printf("        %x bytes read at IOBASE = %x\n", count, io_buffer);

				/* Close file 				*/
	if(close(fdes) == ERROR) {
		printf("\n\nerror in closing file\n\n");
		exit(ERROR);
	}


				/* Initialize SYSIO devices by call
					to TINIT = IOBASE + 9		*/
	sys1call(tinit);

Š}				/* END OF LDSYSIO			*/

	/* page eject */

/* 
Read Hayes Chronograph, as device PUN:/RDR:.
Return strings for time, date, day of week, month, and displayable 
	summary of these data.
Also, construct a logon header message (see arrays declared as external).
*/

				/* Fill time and date strings		*/
void chrono()
{
	void set_time(), date_set(), day_set(), set_month();
	void mes_set(), set_log();

	set_time();

	date_set();

	day_set();

	set_month();

	mes_set();

	set_log();


}				/* END OF CHRONO				*/


				/* Fill time string			*/
void set_time()
{
	void hayes_chrono();
	char clock_in[12];
	int i,j;

	hayes_chrono("RT\r", clock_in);
					/* Clock_in gives hhmmss x	*/
					/* Convert to     hh:mm:ss: xm	*/
	for (j = 0, i = 0; clock_in[i] != NULL; j++, i++){
		if (i == 2 || i == 4 || i == 6) j++;
		time[j] = clock_in[i];
	}
}				/*END OF SET_TIME			*/


				/* Fill date string			*/
void date_set()
{
	void hayes_chrono(), swap();
	char clock_in[12];
	int i,j;
Š	hayes_chrono("RD\r", clock_in);
				 	/* Clock_in gives yymmdd	*/
					/* Convert to     mm/dd/yy	*/
	for (j = 6, i = 0; clock_in[i] != NULL; j++, i++){
		if (j == 8) j = 0;
		if (i == 4) j++;
		date[j] = clock_in[i];
	}

}				/*END OF DATE_SET			*/


				/* Fill day string			*/
void day_set()
{
	int atoi();
	char *strcpy();
	void hayes_chrono();
					/* Clock_in gives  d = acsii #	*/
					/* Convert to int ptr to string	*/
					/* Monday is  d = '0'		*/
	char clock_in[12];
	int i;
	static char *weekday[]={
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday",
		"Sunday",
		"Illegal"
	};

	hayes_chrono("RW\r", clock_in);
	i = atoi(clock_in);
	if (i < 0 || i > 6) i = 7;
	strcpy(day, weekday[i]);
}				/*END OF DAY_SET			*/


				/* Fill month string			*/
void set_month()
{
	char *strcpy(), *strncpy();
	int atoi();
	char c[3];
	int i;
	static char *year_month[]={
		"Illegal",
		"January",
		"February",
		"March",
		"April",
		"May",Š		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December",
	};

	strncpy(c, date, 2);
	i = atoi(c);
	if (i < 1 || i > 12) i = 0;
	strcpy(month, year_month[i]);
}				/*END OF SET_MONTH			*/


				/* Fill message string			*/
void mes_set()
{
	char *strcat(), *strcpy(), *strncat();

	strcpy(date_mes, time);
	strcat(date_mes, "  on ");
	strcat(date_mes, day);
	strcat(date_mes, " ");
	strcat(date_mes, month);
	strcat(date_mes, " ");
	strncat(date_mes, &date[3], 2);
	strcat(date_mes, ", 19");
	strncat(date_mes, &date[6], 2);
}				/*END OF SET_MES			*/


				/* Fill logon string			*/
void set_log()
{



	char *strcat(), *strcpy();

	strcpy(log_mes, "LOGON     DATE:- ");
	strcat(log_mes, date);
	strcat(log_mes, "  TIME:- ");
	strcat(log_mes, time);
}				/*END OF SET_LOG			*/


				/* Send command to clock 
					and return with string		*/
void hayes_chrono(command, buffer)
char *command, *buffer;
{
	char *strcpy(), *strcat();
	int bios();Š	char temp[12];
	int i;
	strcpy(temp,"AT");
	strcat(temp,command);		/* Form command			*/

	for (i = 0; temp[i] != NULL; i++)
		bios(PUNCH, temp[i]);	/* Use direct bios call		*/

	for (i = 0; i <= 11; i++){
		temp[i] = bios(READER, 0);
		if (temp[i] == CR) break;
	}
	temp[i] = '\0';

	strcpy(buffer, temp);
}				/* END OF HAYES_CHRONO			*/

