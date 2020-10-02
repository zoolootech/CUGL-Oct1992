/*********************************************************************\
** .---------------------------------------------------------------. **
** |                                                               | **
** |                                                               | **
** |         Copyright (c) 1981, 1982, 1983 by Eric Martz.         | **
** |                                                               | **
** |                                                               | **
** |       Permission is hereby granted to use this source         | **
** |       code only for non-profit purposes. Publication of       | **
** |       all or any part of this source code, as well as         | **
** |       use for business purposes is forbidden without          | **
** |       written permission of the author and copyright          | **
** |       holder:                                                 | **
** |                                                               | **
** |                          Eric Martz                           | **
** |                         POWER  TOOLS                          | **
** |                    48 Hunter's Hill Circle                    | **
** |                      Amherst MA 01002 USA                     | **
** |                                                               | **
** |                                                               | **
** `---------------------------------------------------------------' **
\*********************************************************************/

#include "rap.h"

/****************************************************************
handles case of leading blanks or tabs; empty lines 
******************************************************************/

leadbl (line)
	char *line;
	{
	int i;
	char c;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"Entering leadbl.\n");
#endif
	if (Outwrds) brk(CR);

	Tival = Inval; /* literal leading whitespace overrides Pival */
	Skip_cnt = i = 0;
	while((c = line[i]) EQ SPACE OR c EQ TAB) {
		if (c EQ SPACE) Tival += 1;
		if (c EQ TAB) {
			Tival += 1;
			while (Tival - (Tabval * (Tival/Tabval))) Tival += 1;
		}
		i++;
		Skip_cnt++;
	}
	Nextin += Skip_cnt;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,
		"leadbl: Tival=%d, Skip_cnt=%d, Tabval=%d\n",
		Tival, Skip_cnt, Tabval);
#endif
}
/********************************************************************
		produces n empty lines
*********************************************************************/
skip (n)
	int n;
	{
	int i;
	if (Print_at) return(0);
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"SKIP %d line(s)\n", n);
#endif
	for ( i=0; i<n; i++) {
		screen_status();
#ifdef DEBUG
	     if (Debug) putchar ('!');
#endif
		putchar (NEWLINE);
	}
}


/******************************************************************
	indents the proper number of spaces
*******************************************************************/
indent(val)
int val;
{
int i;
if (Print_at) return(0);

#ifdef DEBUG
if (Debug) fprintf(STDERR,"INDENT %d spaces(s)\n",val);
#endif
	for ( i=0; i<val; i++ )		putchar( BLANK );


}



/*******************************************************************
		puts out page header
*******************************************************************/
phead()
	{
	Curpag = Newpag;
	if (Print_at EQ Newpag) Print_at = 0;
	Newpag++;
	Bottom = Plval - M3val - M4val;
	if ( M1val > 0 ) {
		skip ( M1val - 1 );
		puttl ( Header);
	}
	skip ( M2val );
	Lineno = M1val + M2val + 1;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"After PHEAD, Lineno=%d\n", Lineno);
#endif
	return;
}


/*********************************************************************
		puts out page footer
*********************************************************************/
pfoot() {
	skip ( M3val );
	if ( M4val > 0 ) {
		puttl ( Footer);
		skip ( M4val - 1 );
     }
	if (Screen_size AND !Print_at)
		fprintf(STDERR,"%s<Bottom of page %d>%s\n", Bar, Curpag, Bar);
	if (!Con_forms AND !Print_at) {
		fprintf(STDERR,"\7");
		wait();
	}
	if (End_wait) {
		fprintf(STDERR,"\7");
		wait();
		End_wait = NO;
	}
}
/*******************************************************************
	put out title line with optional page no.
*******************************************************************/
puttl(title_str)
	char *title_str;
	{
	char buf[MAXLINE], left[MAXLINE], center[MAXLINE], delim_str[2];
	char number[8];
	int space1, len1;

	if (Print_at) return(0);

	delim_str[0] = Pre_ss_delim;
	delim_str[1] = NULL;

	strcpy(buf, title_str); /* SAVE title_str FOR FUTURE USE */
	ss(buf, delim_str); /* PRE SUBSTITUTE CURRENT VALUES */

	delim_str[0] = Post_ss_delim;
	sprintf(number, "%d", Curpag);

#ifdef DEBUG
	if (Debug) fprintf(STDERR,"Puttl:");
#endif

	/* EXTRACT LEFT, CENTER, RIGHT(BUF) */
	substitute(buf, "$F", (Fpin EQ Fp_main? Fn_main: Fn_read));
	center[0] = NULL;
	if (todelim(left, buf, "|") NE ERROR)
		todelim(center,buf,"|");
	else {
		strcpy(left, buf);
		buf[0] = NULL;
	}
	if (*buf) {
		substitute(buf, "#", number);
		if (!*Flush_right)
			pad(buf, 'l', Rmval + (strlen(buf)-truelen(buf)), SPACE);
		else puts(Flush_right);
		ss(buf, delim_str);
		puts(buf);
		putchar('\r');
		if (*Flush_right) restore_mode();
	}
	if (*center) {
		substitute(center, "#", number);
		if (!(*Center_mode)) 
			pad(center, 'l', ((Rmval-truelen(center))/2), SPACE);
		else puts(Center_mode);
		ss(center, delim_str);
		puts(center);
		putchar('\r');
		if (*Center_mode) restore_mode();
	}
	substitute(left, "#", number);
	ss(left, delim_str);
	screen_status();
	puts(left);
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"%s\n",buf);
#endif
	putchar(NEWLINE);	
}
/********************************************************************
	copy title from com_line to ttl
**********************************************************************/
gettl(com_line, ttl)
	char *com_line, *ttl;
	{
	int i;
	char d[2];

	#ifdef DEBUG
	if (Debug) fprintf(STDERR,"GETTL command line= <%s>\n", com_line);
	#endif

	if (strlen(com_line) < 5) {
		*ttl = NULL;
		return(0);
	}

	/* SKIP UNQUOTED LEADING WHITESPACE */
	i=3;
	while(isspace(com_line[i])) i++;

	/* STRIP QUOTE IF FOUND */
	if ( com_line[i]  EQ  0x27 /* single quote */
		OR  com_line[i]  EQ  '"')	i++;

	strcpy (ttl, &com_line[i]);

	#ifdef DEBUG
	if (Debug) fprintf(STDERR,"Title = <%s>\n", ttl);
	#endif
}
/******************************************************************
	space n lines or to bottom of the page
*******************************************************************/
space (n)
	int n;
	{

#ifdef DEBUG
	if (Debug) fprintf(STDERR,
		"SPACE %d line(s), Lineno= %d, Outbuf[0]=<decimal %d>\n",
		n, Lineno, Outbuf[0]);
#endif
	brk(CR);	/* flush out last unfilled line */
	if (Lineno > Bottom)	return;	/* end of page */

	if ( Lineno  EQ  0 )	/* top of page */
		phead();

	skip( min( n, Bottom+1-Lineno ));	/* can't skip past bottom  */
	Lineno = Lineno + n;	/* obvious */

#ifdef DEBUG
	if (Debug) fprintf(STDERR,"After spacing, Lineno = %d\n", Lineno);
#endif
	if (Lineno > Bottom) {
		pfoot();	/* print footer if bottom */
		Lineno = 0;
	}
	if (Fill) Tival = Pival;
}


/*******************************************************

*******************************************************/
text (line)
	char *line;
	{
	char wrdbuf [MAXLINE];
	int i, j, k;
	char *p1, *p2;

	Nextin = line;
#ifdef DEBUG
	if (Debug) checkbig("TEXT:",line);
#endif

	/* USE space() FOR BLANK LINES TO SET Tival TO Pival */
	if (!line[0] OR only(line, "\t ")) {
		space(1);
		goto ul;
	}
	/* IGNORE LEADING @ */
	if (*line EQ '@' AND Ignore_at) {
		line++;
		Nextin++;
	}

	/* LEADING WHITESPACE */
	if (Fill AND (line[0]  EQ  BLANK  OR  line[0]  EQ  TAB))
		leadbl (line); /* SETS Skip_cnt */
	else Skip_cnt = 0;

	if (Ce_input OR !Fill ) {
		expantab(line+Skip_cnt);
		if (Ce_input) {
			chopwhite('b', line);
			if (!(*Center_mode)) {
				center (line);
				Skip_cnt = 0;
			}
		}
		put (line+Skip_cnt,CR);
	}
	else {
		while (getwrd (&Nextin)) {
			getspaces(&Nextin);
			putwrd ();
		}
	}
ul:
	if (Ul_input) Ul_input--;
	if (Ce_input) Ce_input--;
	if (Bo_input) Bo_input--;
#ifdef DEBUG
	if (Debug) checkbig("Text returning.\n","");
#endif
}

/***********************************************************
	put out a line of text with correct indentation
	underlining if specified
************************************************************/
put (line, eol)
	char *line, eol;
	{
	int i, j, k;

#ifdef DEBUG
	if (Debug) checkbig("PUT:",line);
#endif
	if (Lineno  EQ  0) phead();
	putline (line,eol);
	skip (min (Lsval-1, Bottom-Lineno));
	Lineno = Lineno + Lsval;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,
		"After PUT: Lineno=%d,  Lsval=%d\n",Lineno, Lsval);
#endif
	if (Lineno > Bottom) {
		pfoot();
		Lineno = 0;
	}
}
/***************************************************
***************************************************/
restore_mode() {

/*	if (Print_at) return(0); */

	switch (Justify) {
		case 0:
			puts(Flush_left);
			break;
		case 1:
			puts(Just_one);
			break;
		case 2:
			puts(Just_two);
	}
}
/***************************************************
	TRUE LENGTH
***************************************************/
truelen(buf)
	char *buf;
	{
	char *p;
	int len, indelim;

	len = 0;
	indelim = NO;

	/* GET LENGTH OF LINE DISCOUNTING }}, ^H, ETC. */

	for (p=buf; *p; p++) {
		if (*p EQ Post_ss_delim) {
			if (!indelim) {
				indelim = YES;
				continue;
			}
			else {
				indelim = NO;
				if (p > buf AND (*(p-1) EQ Post_ss_delim)) len++;
				continue;
			}
		}
		if (indelim) continue;
		if (*p EQ BACKSPACE) {
			len--;
			continue;
		}
		len++;
	}
	return(len);
}
/***************************************************
	END OF RAP2.C
***************************************************/
**********************************
***************************************************/
restore_mo