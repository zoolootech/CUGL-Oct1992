/* TYPEXXR.C   VERS:- 01.00  DATE:- 09/26/86  TIME:- 09:37:26 PM */
/*
%CC1 $1.C -O -X -E3B00
%CLINK $1 WILDEXP -S -E3B00
%DELETE $1.CRL 
*/
/* 
Description:

Sequential display of a wildcard filelist, with option for printing.

Rewrite of TYPE20, by sfk, W. Earnest, and others.

Restored user number prefixes and introduced recognition of <du:> form
	(new versions of DEFFx.CRL and WILDEXP).
Introduced command_line options and flags toggled or set through them.
Revised dochar() and xxxx_putchar() for efficiency.
Sort filelist before display.
Changed format of display, to accomodate printing.
Option to add formfeed if needed to maintain phase of fanfold paper.
OUT_BDOS option, so can print with cp/m ^p; route input also through bdos.
General changes in code.

Examples of option settings:

	option = -S		option = -T		option = -U

	display 19 lines	display all file	display all file
	no pausing		no pausing		pause at page/file
	no form feed output	maintain paper phase	form feed output
	<-----allow routing of output to printer through bdos ^p----->

LINES_MAX   21			0			0
PAGE	    0			0			1
FILE_PAUSE  0			0			1	
FORM_FEEDS  0			1			0
EXPAND	    0			0			0
NOSYS	    0			0			0
OUT_BDOS    1			1			1



By J.A. Rupley, Tucson, Arizona
Coded for BDS C compiler, version 1.50a
*/

        /* page eject */

#include <bdscio.h>

#undef NSECTS
#undef BUFSIZ
#define NSECTS 64 /* Number of sectors to buffer up in ram */
#define BUFSIZ (NSECTS * SECSIZ + 6) 
#define TAB 9
#define LF 10
#define FF 12
#define CR 13
#define BACKSPACE 8
#define SPACE 32
#define CNTRL_C 3
#define CNTRL_S 19
#define CNTRL_X 24
#define	CONST	2
#define CONIN	3
#define CONOUT	4
#define RECOGNIZE 0xFF76	/* unlikely pattern */
#define DLE 0x90		/* repeat byte flag */
#define	CHAR_AVAILABLE 0xff
#define SPEOF 256		/* special endfile token */
#define NUMVALS 257		/* 256 data values plus SPEOF*/
#define LARGE 30000
#define PATHLEN 20	/* file string size */
#define TLENGTH 24	/* number of lines on screen */
#define PAGE_LENGTH 66 		/* number of lines per printed page*/
#define COLUMNS 80	/* number of columns per line*/
#define TAB_SPACES 8	/* number of spaces separating tab positions*/

#define EXPAND FALSE	/* expand tabs */
#define NOSYS FALSE	/* display system files */
#define LINES_MAX 21     /* default line limit if select option = screen */
#define PAGE TRUE	/* stop at full screen */
#define FILE_PAUSE TRUE		/* pause before each file */
#define FORM_FEEDS FALSE        /* print form feeds & maintain paper phase*/

#define OUT_BDOS	/* comment out for output through bios */

struct _sqleaf        /* Decoding tree */
{
	int _children[2];        /* left, right */
}
;
struct _sqleaf Dnode[NUMVALS -1];
unsigned Sqcksum;
int Bpos;        /* last bit position read */
int Curin;        /* last byte value read */
int Repct;        /* Number of times to retirn value */
int Value;        /* current byte value or EOF */

char File_buf[BUFSIZ];

int col, line_cnt, page_cnt, last_pause, early_exit;
int expand, tab_spaces, nosys, maxlines, page, file_pause, form_feeds;
int optionerr;
int c, yy;

main(argc, argv)
int argc;
char **argv;
{
	char file[PATHLEN];
	int x, y;
	int string_compare();

	nosys = NOSYS;        /*OPTION FLAGS*/
	maxlines = 0;        /*no output line limit unless set by option*/
	expand = EXPAND;
	tab_spaces = TAB_SPACES;
	page = PAGE;
	file_pause = FILE_PAUSE;
	form_feeds = FORM_FEEDS;
	optionerr = 0;

	get_options(&argc, argv);        /*RESET OPTION FLAGS*/
	if (optionerr || (argc < 2))
		help_mess();

	wildexp(&argc, &argv);        /*WILDCARD & SORT COMMAND LINE*/
	qsort(&argv[1], argc - 1, 2, &string_compare);

	early_exit = 0;        /*FLOW CONTROL FLAGS*/
	col = 0;
	line_cnt = 0;
	page_cnt = -1;
	last_pause = 0;

	for (x = 1; x < argc; ++x)
	{
		switch (catvalid(file, argv[x]))
		{
		case 'q' :
			if (qsend(file) == 'a')
				send_text(file);
			break;
		case 'a' :
			send_text(file);
			break;
		case 'x' :
			break;
		case ERROR :
		default :
			y = 3;
			while (y--)
			{
				printf("\n");
				up_line_cnt();
			}
			printf("\tcan't type %s...", argv[x]);
		}

		if (file_pause && (early_exit != 2))
		{
			printf("\n[next file?]");
			up_line_cnt();
			while ((y = bdos(1, 0)) == 0)
				;
			printf("\r             \r");
			if (y == 0x03)
			{
				printf("that's all folks...");
				exit();
			}
		}
	}
	return (OK);
}
        /* page eject */

int send_text(file)
char *file;
{
	int i;

	if (page_cnt >= 0)
	{
		/*3-line spacer between files*/
		i = 3;
		while (i--)
		{
			printf("\n");
			up_line_cnt();
		}
		/*FF if needed to maintain phase of paper*/
		if (form_feeds)
		{
			if ((page_cnt % 2) == 0)
				printf("\f");
			/* form feed at start of file*/
			printf("\f");
		}
	}
	printf("\rListing file %s\n\n", file);
	page_cnt = 0;
	last_pause = 0;
	line_cnt = 2;
	early_exit = 0;
	col = 0;
	while (((c = getc(File_buf)) != EOF) && (!early_exit))
		dochar();
	fclose(File_buf);
	return (OK);
}



void dochar()
{
	if ((c = (c & 0x7f)) > 0x1f)
	{
#ifdef OUT_BDOS
		bdos(2, c);        /*some direct output calls for speed*/
#else
		bios(CONOUT, c);
#endif
		if (++col >= COLUMNS)
		{
			col %= COLUMNS;
			up_line_cnt();
		}
	}
	else
	    switch (c)
	{
	case TAB :
		yy = (tab_spaces - col % tab_spaces);
		if ((col += yy) >= COLUMNS)
		{
			col %= COLUMNS;
			up_line_cnt();
		}
		if (!expand)
		{
#ifdef OUT_BDOS
			bdos(2, c);
#else	
			bios(CONOUT, c);
#endif
		}
		else
		    {
			while (yy--)
			{
#ifdef OUT_BDOS
				bdos(2, SPACE);
#else	
				bios(CONOUT, SPACE);
#endif
			}
		}
		break;
	case CR :
		b_putchar(c);
		col = 0;
		break;
	case 0x1a :
		early_exit = 1;
		break;
	case LF :
		b_putchar(c);
		up_line_cnt();
		if (maxlines)
		{
			yy = page_cnt * PAGE_LENGTH + line_cnt;
			if (yy >= maxlines)
				early_exit = 1;
		}
		if (page)
		{
			yy = page_cnt * PAGE_LENGTH + line_cnt;
			if ((yy - last_pause) >= (TLENGTH - 2))
			{
				last_pause = yy;
				printf("\r[more..]");
				switch (bios(CONIN, 0))
				{
				case CNTRL_C :
					printf("\rthat's all folks...");
					exit();
				case CNTRL_X :
					early_exit = 2;
					printf("\rno more...");
					break;
				default :
					yy = 8;
					while (yy--)
					{
						b_putchar(BACKSPACE);
						b_putchar(SPACE);
						b_putchar(BACKSPACE);
					}
				}
			}
		}
		break;
	case FF :
		if (form_feeds)
		{
			b_putchar(c);
			col = 0;
			line_cnt = 0;
			page_cnt++;
		}
		break;
	}

}



/*  type.c will use its own version of getc.*/
/*  This is so we can have a bigger buffer.*/

int getc(iobuf)
struct _buf *iobuf;
{
	int nsecs;

	IF(!iobuf->_nleft--)
	{
		if ((nsecs = read(iobuf->_fd, iobuf->_buff, NSECTS)) <= 0)
			return iobuf->_nleft++;
		iobuf->_nleft = nsecs * SECSIZ - 1;
		iobuf->_nextp = iobuf->_buff;
	}
	return *iobuf->_nextp++;
}



int up_line_cnt()
{
	if (++line_cnt >= PAGE_LENGTH)
	{
		page_cnt++;
		line_cnt = 0;
	}
	return line_cnt;
}



void b_putchar(cc)
int cc;
{
#ifdef	OUT_BDOS
	int t;

	bdos(2, cc);
	if (bdos(11, 0))
	{
		if ((t = bdos(1, 0)) == CNTRL_C)
			exit();
		else
			if (t == CNTRL_X)
				early_exit = 2;
	}
#else
	int t;

	bios(CONOUT, cc);
	if (bios(CONST, 0) == CHAR_AVAILABLE)
	{
		if ((t = bios(CONIN, 0)) == CNTRL_C)
			exit();
		else
			if (t == CNTRL_S)
			{
				while (bios(CONST, 0) != CHAR_AVAILABLE)
					;
				t = bios(CONIN, 0);
			}
		else
		    if (t == CNTRL_X)
			early_exit = 2;
	}
#endif
}
        /* page eject*/

int catvalid(pname, name)
char *pname;
char *name;
{
	char *pptr, *qptr, *npnt;
	int fd;
	int y;

	if (strlen(name) > 17)
	{
		y = 3;
		while (y--)
		{
			printf("\n");
			up_line_cnt();
		}
		printf("\t'%s' bad name, (name too long)", name);
		return 'x';
	}
	if ((fd = fopen(name, File_buf)) == ERROR)
	{
		y = 3;
		while (y--)
		{
			printf("\n");
			up_line_cnt();
		}
		printf("\tCan't find %s on disk - check your spelling.", name);
		return 'x';
	}
	npnt = fcbaddr(fd);
	if (nosys && (npnt[10] > 127))
	{
		y = 3;
		while (y--)
		{
			printf("\n");
			up_line_cnt();
		}
		printf("\tCan't find %s on disk - check your spelling.", name);
		fclose(File_buf);
		return 'x';
	}
	if (npnt[1] > 127 || npnt[2] > 127)
	{
		if (npnt[10] < 128)
		{
			y = 3;
			while (y--)
			{
				printf("\n");
				up_line_cnt();
			}
			printf("\tFile %s not for distribution.", name);
		}
		fclose(File_buf);
		return 'x';
	}

	strcpy(pname, name);
	if ((y = bad_type(name)) == ERROR)
		fclose(File_buf);
	return y;
}


int bad_type(name)
char *name;
{
	char *ss, *tt, *bad;

	bad = ".COM.OBJ.BAD.LOG.SYS.OV?.SEN.REL.CRL.LBR.ARC.NDX.DBF.ZIP.ZBA.HEX.DIC.IO .?Q*.";

	for (; *name; name++)
		if (*name == '.')
			break;
	if (*name)
		if (*(name + 2) == 'Q')
			return 'q';
	for (; *bad; bad++)
		if (*bad == *name)
			for (ss = bad, tt = name;; ss++, tt++)
			{
				while (*tt == ' ')
					tt++;
				while (*ss == ' ')
					ss++;
				if (!*tt)
				{
					if ((*ss == '.') || (*ss == '*'))
						return ERROR;
					else
					    break;
				}
				if ((*ss == '?') || (*ss == '*'))
					continue;
				if (*ss != *tt)
					break;
			}
	return 'a';
}

        /* page eject*/

/*
The following code is primarily from typesq.c and utr.c.  Typesq
is a modification of USQ by Dick Greenlaw.  Those modifications (usq
to typesq) were made by Bob Mathias, I am responsible for the butchery
done to make it work with cat.
*/

int qsend(infile)        /* #define VERSION "1.3   07/21/81" */
char *infile;
{
	char origname[14];        /* Original file name without drive */
	int y, i;
	char *p;
	unsigned filecksum;        /* checksum */
	int numnodes;        /* size of decoding tree */

	Sqcksum = 0;        /* Initialization */
	init_cr();
	init_huff();

	if (getw(File_buf) != RECOGNIZE)        /* Process header */
	{
		fclose(File_buf);
		return 'a';        /* not squeezed after all */
	}
	filecksum = getw(File_buf);
	p = origname;        /* Get original file name */
	do        /* send it to array */
	{
		*p = getc(File_buf);
	}
	while (*p++ != '\0')
		;

	numnodes = getw(File_buf);
	if (numnodes < 0 || numnodes >= NUMVALS)
	{
		y = 3;
		while (y--)
		{
			printf("\n");
			up_line_cnt();
		}
		printf("\t%s has invalid decode tree size", infile);
		fclose(File_buf);
		return ERROR;
	}
	/* Initialize for possible empty tree (SPEOF only) */
	Dnode[0]._children[0] = -(SPEOF + 1);
	Dnode[0]._children[1] = -(SPEOF + 1);

	for (i = 0; i < numnodes; ++i)        /* Get decoding tree from file */
	{
		Dnode[i]._children[0] = getw(File_buf);
		Dnode[i]._children[1] = getw(File_buf);
	}

	if (page_cnt >= 0)
	{
		/*3-line spacer between files*/
		i = 3;
		while (i--)
		{
			printf("\n");
			up_line_cnt();
		}
		/*FF if needed to maintain phase of paper*/
		if (form_feeds)
		{
			if ((page_cnt % 2) == 0)
				printf("\f");
			/* form feed at start of file*/
			printf("\f");
		}
	}
	printf("\r%s -> %s\n\n", infile, origname);
	page_cnt = 0;
	last_pause = 0;
	line_cnt = 2;
	early_exit = 0;
	col = 0;
	while (((c = getcr(File_buf)) != EOF) && (!early_exit))
		dochar();
	fclose(File_buf);
	return OK;
}


/*** from utr.c - */
/* initialize decoding functions */

init_cr()
{
	Repct = 0;
}



init_huff()
{
	Bpos = 99;        /* force initial read */
}



/*
Get bytes with decoding - this decodes repetition,
calls getuhuff to decode file stream into byte
level code with only repetition encoding.
 *
The code is simple passing through of bytes except
that DLE is encoded as DLE-zero and other values
repeated more than twice are encoded as value-DLE-count.
*/

int getcr(ib)
char *ib;
{
	int cc;

	if (Repct > 0)
	{
		/* Expanding a repeated char */
		--Repct;
		return Value;
	}
	else
	    {
		/* Nothing unusual */
		if ((cc = getuhuff(ib)) != DLE)
		{
			/* It's not the special delimiter */
			Value = cc;
			if (Value == EOF)
				Repct = LARGE;
			return Value;
		}
		else
		    {
			/* Special token */
			if ((Repct = getuhuff(ib)) == 0)
				/* DLE, zero represents DLE */
				return DLE;
			else
			    {
				/* Begin expanding repetition */
				Repct -= 2;        /* 2nd time */
				return Value;
			}
		}
	}
}



/* 
Decode file stream into a byte level code with only
repetition encoding remaining.
*/

int getuhuff(ib)
char *ib;
{
	int i;
	int bitval;

	/* Follow bit stream in tree to a leaf*/
	i = 0;        /* Start at root of tree */
	do
	    {
		if (++Bpos > 7)
		{
			if ((Curin = getc(ib)) == ERROR)
				return ERROR;
			Bpos = 0;
			/* move a level deeper in tree */
			i = Dnode[i]._children[1 & Curin];
		}
		else
			i = Dnode[i]._children[1 & (Curin >>= 1)];
	}
	while (i >= 0)
		;

	/* Decode fake node index to original data value */
	i = -(i + 1);
	/* Decode special endfile token to normal EOF */
	i = (i == SPEOF) ? EOF : i;
	return i;
}
        /* page eject */

void get_options(argcpntr, argv)
char **argv;
int *argcpntr;
{
	char *ss;
	int jj, ii;

	for (ii = *argcpntr - 1; ii > 0; ii--)
		if (argv[ii][0] == '-')
		{
			for (ss = &argv[ii][1]; *ss != '\0';)
			{
				switch (toupper(*ss++))
				{
				case 'S' :
					maxlines = LINES_MAX;
					page = file_pause = 0;
					form_feeds = 0;
					break;
				case 'T' :
					maxlines = 0;
					page = file_pause = 0;
					form_feeds = 1;
					break;
				case 'U' :
					maxlines = 0;
					page = file_pause = 1;
					form_feeds = 0;
					break;
				case 'F' :
					form_feeds = !form_feeds;
					break;
				case 'P' :
					page = !page;
					file_pause = !file_pause;
					break;
				case 'L' :
					if (!(maxlines = atoi(ss)))
						maxlines = LINES_MAX;
					break;
				case 'E' :
					if (expand = atoi(ss))
						tab_spaces = expand;
					else
					    tab_spaces = expand = TAB_SPACES;
					break;
				case 'H' :
					optionerr = TRUE;
					break;
				default :
					printf("\tillegal option %c.\n", *--ss);
					ss++;
					optionerr = TRUE;
					break;
				}
				while (isdigit(*ss))
					ss++;
			}
			for (jj = ii; jj < (*argcpntr - 1); jj++)
				argv[jj] = argv[jj + 1];
			(*argcpntr)--;
		}
	return;
}


int string_compare(s, t)
char **s, **t;
{
	char *s1, *t1;
	int i;
	s1 = *s;
	t1 = *t;
	for (i = 0; i < MAXLINE; i++)
	{
		if (t1[i] != s1[i])
			return s1[i] - t1[i];
		if (s1[i] == '\0')
			return 0;
	}
	return 0;
}



void help_mess()
{
	printf("USAGE: TYPE  filname.typ  [-options = STUFPL[#]E[#]H]\n\n");
	printf("Options: -S  = print without pause first 19 lines of files\n");
	printf("         -T  = print without pause, with form feeds and phase\n");
	printf("         -U  = page through file; default setting\n");
	printf("         -F  = toggle print of form feeds and paper phasing\n");
	printf("         -P  = toggle paging \n");
	printf("         -L# = print only first # lines of a file; if no #, # = 19\n");
	printf("         -E# = expand tabs to # spaces; if no #, # = 8\n");
	printf("         -H  = print this help message\n\n");
	printf("Wild chars. (* and ?) OK,\n");
	printf("Multiple file names OK,\n");
	printf("User numbers and <du:> form OK;\n");
	printf("During output, Ctrl-S pauses, Ctrl-C aborts, Ctrl-X skips to next file.\n");
	exit();
}
