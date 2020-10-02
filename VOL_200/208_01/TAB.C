

/*
NAME
	tab
SYSNOPSIS
	tab file ...
NOTE
	the original file will be lost.
AUTHOUR
	Yoshimasa Tsuji
PURPOSE
	spaces are changed into a tab or simply eliminated.
 	a tab at column just before a tab column will be replaced with
 	a space.
RESTRICTION
	tabbing is only at every eight columns.
 */
#include <stdio.h>
#define TAB '\t'
#define SPACE 040

char tmpfile[18];
char tmpname[] = "tabtmp.$$$";
char *infile;
FILE *fopen();
FILE *fi;
char *trimdir();
char *index();

int fd;
#define MAXBUF 0x6000
char buff[MAXBUF];
int ptr;

main(argc,argv)
char **argv;
{
register int tabpos, scount, tcount, c, onespace;

while(--argc) {
	if((fi = fopen(infile = *++argv, "r")) == NULL) {
		fprintf(stderr,"tab: cannot open %s\n", infile);
		continue;
	}
	/* create temporay file */
	strcpy(tmpfile, trimdir(infile));
	strcat(tmpfile, tmpname);
	if((fd = creat(tmpfile,0644)) == -1 ) {
		fprintf(stderr,"tab: cannot create %s\n",tmpfile);
		fclose(fi);
		continue;
	}
	/* copy */
	tabpos = onespace = scount = tcount = 0;
	while((c = fgetc(fi))!= EOF) {
		if(c == TAB) {
			if( tabpos == 7 && scount == 0)
				onespace = 1;
			else
				tcount++;
			scount = tabpos = 0;
			continue;
		}
		if(c == SPACE) {
			if(++tabpos == 8) {
				if(scount == 0)
					onespace = 1;
				else  {
					tcount++;
					scount = 0;
				}
				tabpos = 0;
			}
			else
				scount++;
			continue;
		}
		if(c == '\n') {
			onespace = tabpos = tcount = scount = 0;
			Putc(c);
			continue;
		}
		/* ordinary */
		if(++tabpos == 8)
			tabpos = 0;
		if(onespace) {
			onespace = 0;
			Putc(SPACE);
		}
		while(tcount) {
			Putc(TAB);
			tcount--;
		}
		while(scount) {
			Putc(' ');
			scount--;
		}
		Putc(c);
	}
	Close();
	if(fclose(fi) == EOF) {
		fprintf(stderr,"tab: cannot close %s\n", infile);
		exit(1);
		}
	if(unlink(infile) == -1)
		fprintf(stderr,"tab: cannot delete %s\n", infile);
	if(rename(tmpfile,infile) == -1)
		fprintf(stderr,"tab: unable to create new %s\n", infile);
  }
  exit(0);
}

char *
trimdir(file)
char *file;
{
static char s[6];
char *p, *q;
	s[0] = '\0';
	if(index(file,':') == NULL)
		return s;
	p = file;
	q = s;
		do {
			*q++ = *p;
			if(*p == ':')
				break;
			p++;
		} while(1);
	*q = 0;
	return s;
}

static
Putc(c)
	char c;
{
	buff[ptr] = c;
	if(++ptr == MAXBUF) {
		ptr = 0;
		if(write(fd, buff, MAXBUF) != MAXBUF) {
			fprintf(stderr,"tab: write error on %s\n", tmpfile);
			exit(1);
		}
	}
}

static
Close()
{
	if(write(fd, buff, ptr) != ptr) {
		fprintf(stderr,"tab: write error on %s\n", tmpfile);
		exit(1);
	}
	ptr = 0;
	if(close(fd) != 0) {
		fprintf(stderr,"tab: close error on %s\n", tmpfile);
		exit(1);
	}
}

