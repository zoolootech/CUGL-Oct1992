/*
 * from pcboard to roverboard
 *
 */

#if 0
1234567890123456789012345678901
9600TEXT.ZIP    16576  09-26-91  A pair of US Robotics text files about HST
                               | modems.
---------->
01 4DOS40D.ZIP 11/17/91;00001;*;4DOS 4.0 - manual
01 4UTILS.ZIP 08/10/90;00002;*;Utils for 4DOS
#endif

#define LINE_MAX		150

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct data {
	char filename[15];
	long filesize;
	int year;
	int month;
	int day;
	char description[900];
};

int pastheader = 0;
struct data line;

#define CONTINUE_POS	sizeof("                               |") - 1

void
readline(void) {
	static char buf[1000];
	static char datebuf[5] = "12";
	char *r;

	if (0 == buf[0]) {
		if (NULL == gets(buf))
			exit(0);
	}
	
	if (!buf[0] || isspace(buf[0]) || buf[0] == '*') {
		/* a comment */
		line.filename[0] = 0;
		buf[0] = 0;
		return;
	}
	
	if (strlen(buf) < 31) {
		fprintf(stderr, "warning: line too short:\n%s\n", buf);
	}
	
	r = strtok(buf, " ");
	if (! r) {
		fprintf(stderr, "unknown line:\n%s\n", buf);
		buf[0] = 0;
		return;
	}
	strcpy(line.filename, r);
	
	r += strlen(r) + 1;		/* skip name */
	while (isspace(*r))		/* skip white */
		++r;
	
	r = strtok(r, " ");
	if (! r) {
		fprintf(stderr, "unknown line:\n%s\n", buf);
		buf[0] = 0;
		return;
	}
	line.filesize = atol(r);
	r += strlen(r) + 1;
	
	while (isspace(*r))		/* skip white */
		++r;
	
	datebuf[0] = *r;
	++r;
	datebuf[1] = *r;
	++r;
	line.month = atoi(datebuf);
	++r;					/* - */
	
	datebuf[0] = *r;
	++r;
	datebuf[1] = *r;
	++r;
	line.day = atoi(datebuf);
	++r;
	
	datebuf[0] = *r;
	++r;
	datebuf[1] = *r;
	++r;
	line.year = atoi(datebuf);
	
	while (isspace(*r))		/* skip white */
		++r;
	
	/* now read in continue lines, appending to description */
	if (*r) {
		strcpy(line.description, r);
	
		while (1) {
			if (NULL == gets(buf)) {
				buf[0] = 0;
				return;
			}
		
			if (0 == strncmp(buf, "                               |",
										CONTINUE_POS)) {
				/* continue line... */
				strcat(line.description, " ");
				strcat(line.description, buf + CONTINUE_POS + 1);
			} else {
				/* forget it ... but keeping line in buf. */
				strcat(line.description, "\n");
				return;
			}
		}
	}
}

void
writeline(void) {

	if (! line.filename[0])
		return;

#if 0	
01 4DOS40D.ZIP 11/17/91;00001;*;4DOS 4.0 - manual
#endif

	line.description[LINE_MAX] = '\r';			/* limit this line */
	line.description[LINE_MAX + 1] = '\n';		/* limit this line */
	line.description[LINE_MAX + 2] = 0;			/* limit this line */

	printf("00 %s %02d/%02d/%02d;00000;cdrom;%s",
		line.filename,
		line.month,
		line.day,
		line.year,
		line.description);
}

void _Cdecl
main(void) {
	
	while (1) {
		readline();
		writeline();
	}
}
