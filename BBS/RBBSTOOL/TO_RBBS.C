/*
 * from SIMTEL20, CICA, ulowel:/games format to:
 * rbbs .dir format
 */

#if 0
1234567890123456789012345678901 [Simtel]
4DOSANN.ZIP   B    6864  910819  4DOS runs on DOS 5.0 and Norton DOS info
1234567890123456789012345678901 [CICA]
diskindx.txt	901031	Cumulative Index of the WRK Disks (below)
diskroot.zip	920610	Windows Resource Kit 4/3/92
---------->
9600TEXT.ZIP    16576  09-26-91  A pair of US Robotics text files about HST modems.
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys\stat.h>
#include <time.h>

#define SIMTEL	1
#define CICA	2
#define ULOWEL	3
#define CUG		4

struct data {
	char filename[15];
	long filesize;
	int year;
	int month;
	int day;
	char description[900];
};

int pastheader = 0;
int in_type;
struct data line;
char path[128];

int
readline(void) {
	static char buf[1000];
	char workbuf[1000];
	static char datebuf[5] = "12";
	char *r;
	
	if (buf[0] == 0)
		if (NULL == gets(buf))
			exit(0);
	
	line.filename[0] = 0;
	line.description[0] = 0;
		
	switch (in_type) {
	case ULOWEL:
		if (0 != strnicmp(buf, "file:", 5)) {
			buf[0] = 0;
			return(0);
		}
		
		r = strtok(buf, "\r\n\t ");
		if (! r) {
			fprintf(stderr, "unknown line:\n%s\n", buf);
			exit(1);
			buf[0] = 0;
			return(0);
		}
		
		r += strlen(r) + 1;		/* skip 'file:' */
		while (isspace(*r))		/* skip white */
			++r;
		
		r = strtok(r, "\r\n\t ");
		if (! r) {
			fprintf(stderr, "unknown line:\n%s\n", buf);
			exit(1);
			buf[0] = 0;
			return(0);
		}
		
		strcpy(line.filename, r);

		while (1) {
			if (NULL == gets(buf))
				return(1);
			if (buf[0] == '\r')
				continue;
			if (! buf[0])
				continue;
			if (isspace(buf[0])) {
				strcat(line.description, " ");
				strcat(line.description, &buf[1]);
			} else
				return(0);
		}
		break;
		
	case SIMTEL:
	case CICA:

		/* wait for header */
		if (! pastheader) {
			if (in_type == CICA) {
				if (0 == strlen(buf))
					++pastheader;
			} else {
				if (strstr(buf, "================"))
					++pastheader;
			}
			buf[0] = 0;
			return(0);
		}
	
		if (strlen(buf) == 0) {
			fprintf(stderr, "zero length line\n");
			exit(1);
			buf[0] = 0;
			return(0);
		}

		if (strlen(buf) < 20) {
			fprintf(stderr, "warning: line too short:\n%s\n", buf);
			exit(1);
		}
	
		strcpy(workbuf, buf);
	
		r = strtok(buf, "\r\n\t ");
		if (! r) {
			fprintf(stderr, "unknown line:\n%s\n", buf);
			exit(1);
			buf[0] = 0;
			return(0);
		}
	
		strcpy(line.filename, r);
	
		r += strlen(r) + 1;		/* skip name */
		while (isspace(*r))		/* skip white */
			++r;

		if (in_type == SIMTEL) {
			++r;					/* skip file type */
	
			while (isspace(*r))		/* skip white */
				++r;
	
			r = strtok(r, "\r\n\t ");
			if (! r) {
				fprintf(stderr, "unknown line:\n%s\n", buf);
				exit(1);
				line.filename[0] = 0;
				buf[0] = 0;
				return(0);
			}
			
			line.filesize = atol(r);
			r += strlen(r) + 1;
	
			while (isspace(*r))		/* skip white */
				++r;
		}
	
		datebuf[0] = *r;
		++r;
		datebuf[1] = *r;
		++r;
		line.year = atoi(datebuf);
	
		datebuf[0] = *r;
		++r;
		datebuf[1] = *r;
		++r;
		line.month = atoi(datebuf);
		datebuf[0] = *r;
		++r;
		datebuf[1] = *r;
		++r;
		line.day = atoi(datebuf);
	
		while (isspace(*r))		/* skip white */
			++r;
	
		if (*r)	
			strcpy(line.description, r);
		buf[0] = 0;
		return(0);
		
	case CUG:
		if (strlen(buf) == 0) {
			fprintf(stderr, "zero length line\n");
			buf[0] = 0;
			return(0);
		}

		if (strlen(buf) < 10) {
			fprintf(stderr, "warning: line too short:\n%s\n", buf);
		}
	
		strcpy(workbuf, buf);
	
		r = strtok(buf, "\r\n\t ");
		if (! r) {
			fprintf(stderr, "unknown line:\n%s\n", buf);
			buf[0] = 0;
			return(0);
		}
	
		strcpy(line.filename, r);
	
		r += strlen(r) + 1;		/* skip name */
		while (isspace(*r))		/* skip white */
			++r;

		if (*r)	
			strcpy(line.description, r);
		buf[0] = 0;
		return(0);
		
	default:
		fprintf(stderr, "bad input type\n");
		exit(1);
	}
}

void
writeline(void) {
	FILE *foo;
	char buf[128];
	struct stat stat_buf;
	struct tm *tm;
	time_t t;
	char *p, *r;

	if (! line.filename[0])
		return;

#if 0	
9600TEXT.ZIP    16576  09-26-91  A pair of US Robotics text files about HST modems.
#endif

	switch (in_type) {
	case CICA:
	case ULOWEL:
	case CUG:
		sprintf(buf, "%s\\%s", path, line.filename);
		if (NULL == (foo = fopen(buf, "r"))) {
			fprintf(stderr, "failed opening '%s'\n", buf);
			exit(1);
			exit(1);
		}

		stat(buf, &stat_buf);

		fclose(foo);
		
		line.filesize = stat_buf.st_size;
		break;
		
	default:
		break;
	}

	switch (in_type) {
	case ULOWEL:
	case CUG:
		t = stat_buf.st_ctime;
		tm = localtime(&t);
		line.month = tm->tm_mon;
		line.day = tm->tm_mday;
		line.year = tm->tm_year;
		break;
		
	default:
		break;
	}
	
	printf("%-13s%8ld  %02d-%02d-%02d ",
		line.filename,
		line.filesize,
		line.month,
		line.day,
		line.year);
	
	/* write wrapping description */

	r = line.description;
	if (strlen(r) <= 45) {		
		printf("%s\n", r);
		return;
	}

	p = r + 45;
	while (! isspace(*p))
		--p;
	
	*p = 0;
	printf("%s\n  ", r);
	r = p + 1;
		
	while (1) {
		if (strlen(r) <= 76) {		
			printf("%s\n", r);
			return;
		}
		p = r + 76;
		while (! isspace(*p))
			--p;

#if 0
9600TEXT.ZIP    16576  09-26-91  A pair of US Robotics text files about HST
                               | modems.
#endif
		*p = 0;
		printf("%s\n  ", r);
		r = p + 1;
	}
}

void
help(void) {

	switch (in_type) {
		case CUG:
			fprintf(stderr,
				"usage: to_rbbs cug [directory path] < in > out\n");
			break;
		case SIMTEL:
			fprintf(stderr,
				"usage: to_rbbs simtel < in > out\n");
			break;
		case ULOWEL:
			fprintf(stderr,
				"usage: to_rbbs ulowel [directory path] < in > out\n");
			break;
		case CICA:
			fprintf(stderr,
				"usage: to_rbbs cica [directory path] < in > out\n");
			break;
		default:
			fprintf(stderr,
				"usage: to_rbbs cug [directory path] < in > out\n");
			fprintf(stderr,
				"       to_rbbs simtel < in > out\n");
			fprintf(stderr,
				"       to_rbbs ulowel [directory path] < in > out\n");
			fprintf(stderr,
				"       to_rbbs cica [directory path] < in > out\n");
			break;
	}
	
	exit(1);
}

void _Cdecl
main(int argc, char *argv[]) {
	int rv;

	if (argc == 1)
		help();

	if (0 == stricmp("simtel", argv[1])) {
		in_type = SIMTEL;
	} else if (0 == stricmp("cica", argv[1])) {
		in_type = CICA;
	} else if (0 == stricmp("cug", argv[1])) {
		in_type = CUG;
	} else if (0 == stricmp("ulowel", argv[1])) {
		in_type = ULOWEL;
	} else
		help();

	switch (in_type) {
		case ULOWEL:
		case CICA:
		case CUG:
			strcpy(path, argv[2]);
			break;
		default:
			break;
	}
	
	while (1) {
		rv = readline();
		writeline();
		if (rv)
			exit(0);
	}
}
