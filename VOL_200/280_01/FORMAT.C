/* [FORMAT.C of JUGPDS Vol.46]*/
/*
*****************************************************************
*								*
*	Written by  Hakuo Katayose (JUG-CP/M No.179)		*
*		    49-114 Kawauchi-Sanjuunin-machi		*
*		    Sendai, Miyagi 980                          *
*		    Phone: 0222-61-3219				*
*								*
*       Modifird by Toshiya Oota   (JUG-CPM No.10)              *
*                   Sakae ko-po 205 				*
*		    5-19-6 Hosoda				*
*		    Katusikaku Tokyo 124			*
*								*
*		for MS-DOS Lattice C V3.1J & 80186/V20/V30	*
*								*
*	Compiler Option: -ccu -k0(1) -ms -n -v -w		*
*								*
*	Edited & tested by Y. Monma (JUG-CP/M Disk Editor)	*
*			&  T. Ota   (JUG-CP/M Sub Disk Editor)	*
*								*
*****************************************************************
*/

/*	format - text formatter 

---------------------------------------------------------------------
 #  dot command  break?  default     function
---------------------------------------------------------------------
 1  .fi          yes		  stat filling
 2  .nf          yes              stop filling
 3  .br n        yes              cause break
 4  .ls n        no       n=1     line spacing is n
 5  .bp n        yes      n=+1    begin page numbered n
 6  .sp n        yes      n=1     space down n lines
 7  .in n        no       n=0     indent n spaces
 8  .rm n        no       n=60    set right margin to n
 9  .ti n        yes      n=0     temporary indent of n
10  .ce n        yes      n=1     center nextn lines
11  .ul n        no       n=1     underline words from next n lines
12  .he          no      empty    header title
13  .fo          no      empty    footer title
14  .pl n        no       n=66    set page length to n
---------------------------------------------------------------------

.bp 1
.in 8
.rm 76
.he :        program     format.c                                       #
*/

#include "stdio.h"
#include "dos.h"
#include "tools.h"
#include "toolfunc.h"

#define UNKNOWN 0
#define FI 1
#define NF 2
#define BR 3
#define LS 4
#define BP 5
#define SP 6
#define IN 7
#define RM 8
#define TI 9
#define CE 10
#define UL 11
#define HE 12	/* Header enable */
#define FO 13
#define PL 14

#define SQUOTE '\''
#define DQUOTE '"'
#define MINUS '-'
#define COMMAND '.'
#define PAGENUM '#'
#define PAGELEN 66
#define PAGEWIDTH 60
#define MAXOUT 256
#define HUGE 1000
#define INSIZE 256


int	fill,	/* fill if YES; init = YES			*/
	lsval,	/* current line spacing; init = 1		*/
	inval,	/* current indent; >= 0; init = 0		*/
	rmval,	/* current right margin; init = PAGEWIDTH = 60	*/
	tival,	/* current temporary indent; init = 0		*/
	ceval,	/* # of lines to center; init = 0		*/
	ulval,	/* # of lines to underline; init = 0		*/
	curpag,
	newpag,
	lineno,
	plval;

int	m1val, m2val, m3val, m4val, bottom;

char	__header[MAXLINE], __footer[MAXLINE], *header, *footer;

int	outp, outw, outwds, dir;
char	outbuf[(MAXOUT+1)];
/*
.bp
*/

void init(),Comand(),Text(),LineSpace(),brk(),GetTitle(),pfoot(),phead();
void putchr(),skip(),LeadBlank(),PutWord(),spread(),PutTitle(),put();

void main(argc, argv)
int	argc;
char 	**argv;
{
char inbuf[MAXLINE], *line;

	if (argc < 2)
		error("FRM999 Usage: format <infile >outfile");
	init();
	line = inbuf;
	while (getlin(line, MAXLINE) > 0)
		if (*line == COMMAND)
			Comand(line);
		else
			Text(line);
	if (lineno > 0)
		LineSpace(HUGE);
}

/*	comand - perform formatting command
.bp
*/
void Comand(line)
char *line;
{
int ctyp, argtyp, val, spval;

	if ((ctyp = ComandTyp(line)) == UNKNOWN)
		return;
	val = GetArgVal(line, &argtyp);
	switch (ctyp) {
	case FI :
		brk();
		fill = YES;
		break;
	case NF :
		brk();
		fill = NO;
		break;
	case BR :
		brk();
		break;
	case LS :
		lsval = set(&lsval, val, argtyp, 1, 1, HUGE);
		break;
	case BP :
		if (lineno > 0)
			LineSpace(HUGE);
		curpag = set(&curpag, val, argtyp, curpag+1, -HUGE, HUGE);
		newpag = curpag;
		break;
	case SP :
		spval = set(&spval, val, argtyp, 1, 0, HUGE);
		LineSpace(spval);
		break;
	case IN :
		inval = set(&inval, val, argtyp, 0, 0, rmval-1);
		tival = inval;
		break;
	case RM :
		rmval = set(&rmval, val, argtyp, PAGEWIDTH, tival+1, HUGE);
		break;
	case TI :
		brk();
		tival = set(&tival, val, argtyp, 0, 0, rmval);
		break;
	case CE :
		brk();
		ceval = set(&ceval, val, argtyp, 1, 0, HUGE);
		break;
	case UL :
		ulval = set(&ulval, val, argtyp, 0, 1, HUGE);
		break;
	case HE :
		GetTitle(line, header);
		break;
	case FO :
		GetTitle(line, footer);
		break;
	case PL :
		plval = set(&plval, val, argtyp, PAGELEN,
			m1val + m2val + m3val + m4val + 1, HUGE);
		bottom = plval - m3val - m4val;
		break;
	}
}

ComandTyp(line)
char *line;
{
char cmd[3];
	++line;	cmd[0] = toupper(*line);
	++line;	cmd[1] = toupper(*line);
	cmd[2] = '\0';
	if (!strcmp(cmd, "FI"))
		return(FI);
	else if (!strcmp(cmd, "NF"))
		return(NF);
	else if (!strcmp(cmd, "BR"))
		return(BR);
	else if (!strcmp(cmd, "LS"))
		return(LS);
	else if (!strcmp(cmd, "BP"))
		return(BP);
	else if (!strcmp(cmd, "IN"))
		return(IN);
	else if (!strcmp(cmd, "RM"))
		return(RM);
	else if (!strcmp(cmd, "TI"))
		return(TI);
	else if (!strcmp(cmd, "CE"))
		return(CE);
	else if (!strcmp(cmd, "UL"))
		return(UL);
	else if (!strcmp(cmd, "HE"))
		return(HE);
	else if (!strcmp(cmd, "FO"))
		return(FO);
	else if (!strcmp(cmd, "PL"))
		return(PL);
	return(UNKNOWN);
}

/* GetAgVal - get argument value */
GetArgVal(line, argtyp)
char *line;
int  *argtyp;
{
	while (!isspace(*line))
		line++;
	while (*line == ' ' || *line == '\t')
		line++;
	*argtyp = *line;
	if (*line == PLUS || *line == MINUS)
		line++;
	return(atoi(line));
}

/* set - set parameter and check range */
set(param, val, argtyp, defval, minval, maxval)
int	*param;
int	val,argtyp,defval,minval,maxval;
{
	if (argtyp == '\n')
		*param = defval;
	else if (argtyp == PLUS)
		*param += val;
	else if (argtyp == MINUS)
		*param -= val;
	else
		*param = val;
	*param = min(*param, maxval);
	return max(*param, minval);
}

/* Text - process text lines */
void Text(line)
char *line;
{
char wrdbuf[INSIZE];
int  i;
	i = 0;
	if (isspace(*line))
		LeadBlank(line);
	if (ceval > 0) {
		tival = max((rmval + tival - width(line))/2, 0);
		put(line);
		ceval--;
		}
	else if (*line == '\n')
		put(line);
	else if (fill == NO)	
		put(line);
	else
		while (GetWord(line, &i, wrdbuf) > 0)
			PutWord(wrdbuf);
}

/* put - put out line with proper spacing andindenting */
void put(line)
char *line;
{
int	i;
	if (lineno == 0 || lineno > bottom)
		phead();
	for (i = 0; i < tival; i++)
		putchar(' ');
	tival = inval;
	puts(line);
	skip(min(lsval-1, bottom - lineno));
	lineno += lsval;
	if (lineno > bottom)
		pfoot();
}

void LineSpace(i)
int	i;
{
	brk();
	if (lineno > bottom)
		return;
	if (lineno == 0)
		phead();
	skip(min(i,bottom + 1 - lineno));
	lineno += i;
	if (lineno > bottom)
		pfoot();
}

/* phead - put out line with proper spacing and indenting */
void phead()
{
	curpag = newpag++;
	if (m1val > 0) {
		skip(m1val-1);
		PutTitle(header, curpag);
		}
	skip(m2val);
	lineno = m1val + m2val + 1;
}

/* pfoot - put out page footer */
void pfoot()
{
	skip(m3val);
	if (m4val > 0) {
		PutTitle(footer, curpag);
		skip(m4val-1);
		}
}


/* PutTitle - put out title line with optional pagenumber */
void PutTitle(line, pageno)
char	*line;
int	pageno;
{
	*line--;
	while (*++line)
		if (*line == PAGENUM)
			printf("%3d", pageno);
		else
			putchar(*line);
}

/* skip - output n blank lines */
void skip(i)
int	i;
{
	while (i--)
		putchar('\n');
}

void LeadBlank(buf)
char *buf;
{
char *temp;
int  i;
	temp = buf;
	i = 0;
/*
	brk();
*/
	while (*buf == ' ' || *buf == '\t') {
		if (*buf++ == '\t')
			i = (i & ~0x07) + 8;
		else
			i++;
		}
	if (*buf != '\n' && fill == NO)
		tival = i;
	while (*buf)
		*temp++ = *buf++;
	*temp = NULL;
}

GetWord(in, ii, out)
char in[], *out;
int  *ii;
{
int i;
char *p;
	p = out;
	i = *ii;
	while (in[i] == ' ' || in[i] == '\t')
		i++;
	while (in[i] && !isspace(in[i]))
		*out++ = in[i++];
	*out = '\0';
	*ii = i;
	return(out - p);
}

void PutWord(wrdbuf)
char *wrdbuf;
{
int last, llval, nextra, w;
	w = width(wrdbuf);
	last = strlen(wrdbuf) + outp + 1;
	llval = rmval - tival;
	if (outp > 0 && (outw + w > llval || last >= MAXOUT)) {
		last -= outp;
		nextra = llval - outp + 1;
		spread(outbuf, outp, nextra, outwds);
		if (nextra > 0 && outwds > 1)
			outp += nextra;
		brk();
		}
	strcpy(outbuf+outp+1, wrdbuf);
	outp = last;
	outbuf[outp] = BLANK;
	outw += w + 1;
	outwds++;
}

/* brk - end current filled line */
void brk()
{
	if (outp > 0) {
		outbuf[outp] = '\n';
		outbuf[outp+1] = '\0';
		put(outbuf+1);
		}
	outp = 0;
	outw = 0;
	outwds = 0;
}

/* width - compute width of character string */
width(buf)
char *buf;
{
int w;
	w = 0;
	for ( ;*buf != EOS; *buf++)
		if (*buf == BACKSPACE)
			w--;
		else if (*buf != '\n')
			w++;
	return(w);
}

/* spread - spread words to justify  right margin */
void spread(buf, outp, nextra, outwds)
char *buf;
int	outp,nextra,outwds;
{
int i, j, nb, ne, nholes;
	if (nextra <= 0 || outwds <= 1)
		return;
	dir = 1 - dir;
	ne = nextra;
	nholes = outwds - 1;
	i = outp - 1;
	j = min(MAXOUT-2, i + ne);
	while (i < j) {
		buf[j] = buf[i];
		if (buf[i] == ' ') {
			if (dir == 0)
				nb = (ne - 1) / nholes + 1;
			else
				nb = ne / nholes;
			ne = ne - nb;
			nholes = nholes - 1;
			for (; nb > 0; nb--) {
				j--;
				buf[j] = ' ';
				}
			}
		i--;
		j--;
		}
	return;
}

void GetTitle(line, title)
char *line, *title;
{
	while (!isspace(*line)) line++;
	while (*line == ' ' || *line == '\t') line++;
	if (*line == SQUOTE || *line == DQUOTE)
		line++;
	strcpy(title, line);
}

void init()
{
	fill  = YES;	/* fill if YES; init = YES			*/
	lsval = 1;	/* current line spacing; init = 1		*/
	inval = 0;	/* current indent; >= 0; init = 0		*/
	rmval = 60;	/* current right margin; init = PAGEWIDTH = 60	*/
	tival = 0;	/* current temporary indent; init = 0		*/
	ceval = 0;	/* # of lines to center; init = 0		*/
	ulval = 0;	/* # of lines to underline; init = 0		*/
	curpag = 0;
	newpag = 1;
	lineno = 0;
	plval  = 66;

	m1val  = 2;
	m2val  = 1;
	m3val  = 1;
	m4val  = 2;
	bottom = plval - m3val - m4val;
	header = __header;
	*header++ = '\n';  *header-- = '\0';
	footer = __footer;
	*footer++ = '\n';  *footer-- = '\0';

	outp = 0;
	outw = 0;
	outwds = 0;
	dir = 0;
}
