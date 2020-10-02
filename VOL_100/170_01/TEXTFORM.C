/********************************************************
*							*
*		   T E X T  F O R M 			*
*	       Text file formatter for MSDOS		*
*							*
*		  T. Jennings 23 Dec. 82		*
*			created 14 Nov. 82		*
*							*
*********************************************************

	TEXTFORM prints and/or formats edited text, using WordStar
compatible dot commands, for use on non-WordStar systems. See 
TEXTFORM.DOC for details. */

#include <stdio.h>
#include <ctype.h>

#define FALSE 0
#define TRUE 1
#define ERROR -1
#define CONTROLZ 0x1a
#define CR 0x0d				/* useful ASCII characters */
#define LF 0x0a
#define FF 0x0c
#define TAB 9
#define BS 8
#define PAGE_LEN 66			/* default lines per page */
#define DEF_LMARGIN 8			/* default left margin, */
#define DEF_TMARGIN 1			/* default top of page margin */
#define DEF_BMARGIN 8			/* bottom of page margin */

char c;					/* a useful variable for everyone */
int line_count;				/* total lines, */
int this_line;				/* current line #/page */
int this_page;				/* current page */
int page_size;				/* current max page length */
int lmargin,rmargin;			/* current left and right margins */
int top_margin;				/* lines from top of page */
int bot_margin;				/* lines at bottom of page */
int line_num;				/* true if line numbers in index */

int inbuf;				/* text file */
int outbuf;
char inname[80];			/* where we save ASCII filenames */
char outname[80];
char header[134];			/* top of page header */
char subheader[134];			/* sub title, */

int flag;				/* user variable */
int cond_flag;				/* conditional print flag */

change_page()				/* dummy function to start a new page */
{}
/* Process the input file. If no output is specified, write it it 
the standard output. */

main(argc,argv)
int argc;
char *argv[];
{
char *p,*pp;
int i;
int have_file;
int arg_error;

	fprintf (stderr,"TEXTFORM -- Text file Formatter (c) T. Jennings 23 Dec. 1982\n");
	page_size =PAGE_LEN;
	lmargin =DEF_LMARGIN;
	top_margin= DEF_TMARGIN;
	bot_margin= DEF_BMARGIN;
	line_num= FALSE;
	strcpy(header,"");			/* no header */
	strcpy(subheader,"");
	flag= 0;				/* flags off */
	cond_flag= 1;				/* allow printing */
	have_file= 0;				/* no output file yet */
	arg_error= 0;				/* no error yet, */

/* Process the command line. We must have a filename, and optionally one
of two options: list amount of memory, of add line numbers to the index. */

	--argc;
	strcpy (inname,argv[1]);		/* get input file, */
	inbuf= open(inname,0x8000);		/* try to open, */
	if (inbuf == ERROR) {
		fprintf(stderr,"Can't seem to find %s.\n",inname);
		fprintf(stderr,"Try: TEXTFORM <file> <outfile>\n");
		fprintf(stderr,"If <outfile> is omitted, output goes\n");
		fprintf(stderr,"to the PRN device.\n");
		exit();
	}
	strcpy(outname,"PRN");			/* default output file */
	--argc; i= 2;
	while (argc-- > 0) {
		p= argv[i++];
		if (*p == '-') {		/* check for options, */
			++p;			/* skip the dash, */
			fprintf(stderr,"'%c' is not an option.\n",*p);
			++arg_error;
		} else {
			if (! have_file) {
				strcpy(outname,p);
				have_file= 1;
			} else {
				fprintf(stderr,"Already have an output file!\n");
				++arg_error;
			}
		}
	}
	if (arg_error) {
		fprintf(stderr,"Too many mistakes.\n");
		exit();
	}
	outbuf= creat(outname,0x8001);		/* open output file */
	if (outbuf == ERROR) {
		fprintf(stderr,"Can't make new file '%s'.\n",outname);
		exit();
	}
	fprintf(stderr,"Printing to %s.\n",outname);
	print();
	wrtc(FF,outbuf);	/* final formfeed, */
	close(inbuf); close(outbuf);
	exit();			/* exit. */
}
/* Print each line to the printer, processing dot commands as we go. */

print()
{
char linebuf[132];		/* character line buffer */
int i;

	line_count =0;
	this_page =1;
	this_line =0;

	while ( fill_line(linebuf) !=CONTROLZ)		/* until EOF, */
	{	if (scan_line(linebuf) ==TRUE)		/* look for dot cmds */
			continue;			/* get next line */
		if (cond_flag == 0)
			continue;			/* print if enabled */

		++line_count;				/* count total lines,*/
		if (this_line == 0) {
			do_header();			/* do headers, */
			i= top_margin;
			wrtc(CR,outbuf); 
			while (i--) {			/* do top margin, */
				wrtc(LF,outbuf);
			}
		}
		i= lmargin;				/* do left margin, */
		while (i--) {
			wrtc(' ',outbuf);
		}
		sendstr(linebuf,outbuf);		/* print line, */

		if (this_line > (page_size- bot_margin))
		{	wrtc(CR,outbuf);
			while (this_line < page_size)
				wrtc(LF,outbuf);
			++this_page;
			this_line= 0;
		}
	}
	return;
}
/* Compare two strings, of a given length. Check only for equality. Return
0 if equal, else 1. */

compl(length,first,last)
int length;
char *first,*last;
{

	for (; length >0; length--)
	{	if (  (toupper(*first)) != (toupper(*last))  )
			return (1)
		;
		++first; ++last;
	}
	return (0);
}
/* Fill a line buffer with characters. Return CONTROLZ if end of file. */

fill_line(buffer)
char buffer[];
{
int i;
char d;

	i=0;
	do {	if (read(inbuf,&d,1) != 1)
			return (CONTROLZ);	/* check physical end */
		if (d ==CONTROLZ) {
			return (d);
		}
		buffer[i++]= d& 0x7f;
	}
	while ((d !=LF) && (i <132));
	buffer[i]= 0x00;			/* terminate */
	return(d);
}
/* Scan for dot commands. When and if we find a dot command, fool the 
caller into thinking that the line we were passed is now empty. (Return TRUE)
If we find a formfeed, treat it the same as a .pa. */

scan_line(buffer)
char *buffer;
{
int i;
char c;
	if (*buffer =='.')		/* as per WS specs, must be first atom */
	{	if (compl (3,buffer,".pa") ==0)
		{	wrtc(CR,outbuf);
			while (this_line < page_size) {
				wrtc(LF,outbuf);
			}
			++this_page;
			this_line =0;

		} else if (compl (3,buffer,".pn") == 0) {
			i= get_next_num(buffer);	/* get new page num, */
			if (i)				/* if not zero, */
				this_page= i;		/* set it, */

		} else if (compl (3,buffer,".pl") == 0) {
			i= get_next_num(buffer);	/* get new paper */
			if (i > 5)			/* length, */
				page_size= i;

		} else if (compl (3,buffer,".mt") == 0) {
			i= get_next_num(buffer);
			if (i == 0)
				i= 1;
			if (i < 40)			/* get top margin, */
				top_margin= i;		/* set if reasonable */
		} else if (compl (3,buffer,".mb") == 0) {
			i= get_next_num(buffer);
			if (i < page_size)		/* set bottom margin */
				bot_margin= i;

		} else if (compl (3,buffer,".he") == 0) {
			while (isgraph(*buffer))	/* skip the .he, */
				++buffer;
			if (*buffer == ' ')		/* and any space, */
				++buffer;
			strcpy(header,buffer);		/* copy the header, */

		} else if (compl (3,buffer,".sh") == 0) {
			while (isgraph(*buffer))
				++buffer;		/* skip the .sh, */
			if (*buffer == ' ')
				++buffer;
			strcpy(subheader,buffer);

		} else if (compl (5,buffer,".flag") == 0) {
				flag= get_next_num(buffer); /* set variable */

		} else if (compl (7,buffer,".ifflag") == 0) {
				cond_flag= flag;

		} else if (compl (6,buffer,".endif") == 0) {
				cond_flag= 1;		/* allow printing */

		} else if (compl (3,buffer,".lm") == 0) {
				i= get_next_num(buffer);
				if (i <80)
					lmargin= i;
		}
		return (TRUE);
	}

	if (*buffer == FF) {
		wrtc(FF,outbuf);
		++this_page;
		this_line= 0;
		return(TRUE);
	}
	return (FALSE);
}
/* Print the top of page header. If we find a #, replace it with 
the current page number, and replace % with the user flag. */

do_header() {

char c;
char *p;
char pnum[8];

	p= header;
	while (*p) {			/* look at current char, */
		if (*p == '#') {
			sprintf(pnum,"%d",this_page);
			sendstr(pnum,outbuf);
		} else if (*p == '%') {
			sprintf(pnum,"%d",flag);
			sendstr(pnum,outbuf);
		} else
			wrtc(*p,outbuf);
		++p;
	}
	p= subheader;
	while (*p) {
		if (*p == '#') {
			sendstr(pnum,outbuf);
			sprintf(pnum,"%d",this_page);
		} else if (*p == '%') {
			sprintf(pnum,"%d",flag);
			sendstr(pnum,outbuf);
		} else 
			wrtc(*p,outbuf);
		++p;
	}
	return;
}
/* Send an ascii string to the output file */

sendstr(string)
char *string;
{
	while (*string)
	{	wrtc (*string++,outbuf);
	}
}
/* Write a character to the file. Count LF's as lines as they go by. */

wrtc(c,file)
char c;
int file;
{
	if (iscntrl(c)) { 
		switch(c) {
			case LF:
				++this_line;
			case CR:
			case FF:
			case TAB:
			case BS:
				write(file,&c,1);
				break;
			default:
				break;
		}
	} else write(file,&c,1);

	return;
}
/* Find the next number in the string, return it's value, or 0 
if none found. */

get_next_num(string)
char *string;
{
int i;
	while (! isdigit(*string))
		++string;		/* skip leading text, */
	if (sscanf(string,"%d",&i) == 0)/* if cant convert, */
		return(0);		/* return 0000 */
	return(i);
}
