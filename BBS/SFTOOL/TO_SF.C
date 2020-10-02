/*
 * from SIMTEL20, CICA, ulowel:/games format to:
 * spitfire
 */

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
	int first = 1;
	
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
				if (first)
					first = 0;
				else {
					strcat(line.description, " ");
				}
				strcat(line.description, &buf[1]);
			} else
				return(0);
		}
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
		break;
	}
}


void
writeline(void) {
	char *r;
	char *p;
	FILE *foo;
	char buf[128];
	struct stat stat_buf;
	struct tm *tm;
	time_t t;
	int x;
	char buf2[20];

	if (! line.filename[0])
		return;

	switch (in_type) {
	case CICA:
	case ULOWEL:
	case CUG:
		sprintf(buf, "%s\\%s", path, line.filename);
		if (NULL == (foo = fopen(buf, "r"))) {
			fprintf(stderr, "failed opening '%s'\n", buf);
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
	
	/* description -- wack off at 40 chars.*/
	line.description[41] = 0;
	
	printf("%-13s",	line.filename);
	
	sprintf(buf, "%ld", line.filesize);
	strrev(buf);
	
	p = buf;
	r = buf2;
	x = 0;
	while (*p) {
		if (x == 3) {
			*r++ = ',';
			x = 0;
		}
		++x;
		*r++ = *p++;
	}
	*r = 0;

	strrev(buf2);
	printf("%8s ", buf2);

	printf(" %02d-%02d-%02d  %s\n",
		line.month,
		line.day,
		line.year,
		line.description);
}

void
help(void) {

	switch (in_type) {
		case CUG:
			fprintf(stderr,
				"usage: to_sf cug [directory path] < in > out\n");
			break;
		case SIMTEL:
			fprintf(stderr,
				"usage: to_sf simtel < in > out\n");
			break;
		case ULOWEL:
			fprintf(stderr,
				"usage: to_sf ulowel [directory path] < in > out\n");
			break;
		case CICA:
			fprintf(stderr,
				"usage: to_sf cica [directory path] < in > out\n");
			break;
		default:
			fprintf(stderr,
				"usage: to_sf cug [directory path] < in > out\n");
			fprintf(stderr,
				"       to_sf simtel < in > out\n");
			fprintf(stderr,
				"       to_sf ulowel [directory path] < in > out\n");
			fprintf(stderr,
				"       to_sf cica [directory path] < in > out\n");
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
