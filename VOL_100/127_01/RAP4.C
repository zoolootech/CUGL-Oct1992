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

/*------------------------------------------------------------------------*/
init_ss() {
	FILE *fpss;
	int c, junk;

	Ss_pair_cnt = 0;
	Oldnewtot = 0;

	/* GET STRINGS FROM SSn */
	fpss = fopen(Ss_name,"r-");
	if (!fpss) {
		fprintf(STDERR,"Warning: file \"%s\" does not exist.\n",Ss_name);
		return(0);
	}
	fprintf(STDERR,"Reading file \"%s\":\n",Ss_name);

	/* PROCESS CONTENTS OF *.SS FILE */
	/* ONLY BLANK LINES, COMMENTS ALLOWED BEFORE COMMANDS */

	FOREVER {
		c = getc(fpss);
		switch (c) {
			case '\n':
			case '\r':
				/* INGNORE BLANK LINES */
				break;
			case '!':
				/* READ AND IGNORE REMAINDER OF LINE */
				rapgets(Inbuf,fpss);
				break;
			case COMMAND:
				ungetc(c,fpss);
				rapgets(Inbuf,fpss);
				de_comment(Inbuf);
				command(Inbuf, Nxname, &junk);
				break;
			default:
				goto pastcomms;
		}
	}
pastcomms:
	if (c EQ EOF) c = CPMEOF;
	ungetc(c,fpss);

	/* GET SUBSTITUTION PAIRS AND HARDWARE INFO */
	if (!getpairs(fpss, Ss_name)) {
		fprintf(STDERR, "ERROR in SS file:\n");
		fprintf(STDERR, "  }u+}, }u-}, }b+}, or }b-} is missing.\n");
	}

	fpfree(fpss);

	/* put "}RF}" in Oldstring[0] for use in ss() */
	Inbuf[0] = Post_ss_delim;
	Inbuf[1] = NULL;
	strcat(Inbuf,"RF");
	Inbuf[3] = Post_ss_delim;
	Inbuf[4] = NULL;
	Oldstring[0] = pack(Oldnewbuf,&Oldnewtot,MAXPACK,Inbuf);
}
/*------------------------------------------------------------------------*/
ss(bf,mark)
	char *bf, *mark;
	{
	char double_delim[3], newrf[5], prompt[3], delim_str[2], wait[7];
	char up[3], down[3], under[3], bold[3], arab[8];
	int start, stop, i, pr;

	delim_str[0] = *mark;
	delim_str[1] = NULL;
	wait[0] = Post_ss_delim;
	wait[1] = NULL;
	strcpy(wait, "wait");
	wait[5] = Post_ss_delim;
	wait[6] = NULL;
	up[0] = down[0] = under[0] = bold[0] = Pre_ss_delim;
	up[1] = 'u';
	down[1] = 'd';
	under[1] = '_';
	bold[1] = '!';
	up[2] = down[2] = under[2] = bold[2] = NULL;


#ifdef DEBUG
	if (Debug) {
		fprintf(STDERR,"Entering ss%s:",mark);
		checkbig("",bf);
	}
#endif
	stop = 0;

	/* CONVERT CONSOLE PROMPT DELIMITERS */
	prompt[0] = Post_ss_delim;
	prompt[1] = '"';
	prompt[2] = NULL;
	while (mark[0] EQ Post_ss_delim
		AND (i = instr(0, bf, prompt)) NE ERROR) {

		bf[i] = c1(PROMPT);
		pr = i;
		do {	/* SHIFT DOWN TO DELETE " */
			i++;
			bf[i] = bf[i+1];
		} while (bf[i]);

		i = instr(pr, bf, delim_str);
		if (i EQ ERROR) {
			i = strlen(bf);
			bf[i+1] = NULL;
		}
		bf[i] = c1(PROMPT);
	}

	/* SQUEEZE BLANKS OUT OF ORIGINAL STRINGS FOR SUBSTITUTION */
	while ((start = instr(stop, bf, mark)) NE -1) {

		stop = instr(start+1, bf, mark);
		if (stop EQ -1) {
			fprintf(STDERR,
"Incomplete \"%s\"-delimited substitution string at line %d:\n",
				mark,In_linecnt);
			checkbig("",bf);
			exit(0);
		}
		stop = stop +1;
			/* -(delchars(bf+start, bf+stop, " ")); defeated 2.21 */
	}
#ifdef DEBUG
	if (Debug AND stop NE 0) checkbig("",bf);
#endif
	/* NO SUBSTITUTIONS NECESSARY */
	/* IF \\ THEN SS IS NOT CALLED UNLESS \\ OR Press_all */
	if (stop EQ 0 AND mark[0] EQ Post_ss_delim) return(0);

	/* INSTALL AUTO-INCREMENTING ARABIC NUMBER IF NEEDED */
	if (mark[0] EQ Pre_ss_delim) {
		arab[0] = Pre_ss_delim;
		arab[1] = '#';
		arab[2] = NULL;
		if ((i=instr(0, bf, arab)) NE ERROR) {
			if (bf[i+2] EQ '+') Arabic++;
			sprintf(newrf, "%d", Arabic);
			arab[2] = '+';
			arab[3] = Pre_ss_delim;
			arab[4] = NULL;
			if (bf[i+2] EQ '+') substitute(bf, arab, newrf);
			else {
				arab[2] = Pre_ss_delim;
				arab[3] = NULL;
				substitute(bf, arab, newrf);
			}
		}
	}
	
	/* EXPAND SUPERSCRIPT/SUBSCRIPT MACRO */
	if (mark[0] EQ Pre_ss_delim) {
		start = 0;
		while ((start=instr(start,bf, up)) NE ERROR)
			updown(bf,start,'u');
		start = 0;
		while ((start=instr(start,bf, down)) NE ERROR)
			updown(bf,start,'d');
		start = 0;
		while ((start=instr(start,bf, under)) NE ERROR)
			updown(bf,start,'_');
		start = 0;
		while ((start=instr(start,bf, bold)) NE ERROR)
			updown(bf,start,'!');
	}
	
	/* WAIT */
	if (mark[0] EQ Post_ss_delim) substitute(bf, wait, WAIT);
	
	/* MAIN SUBSTITUTION LOOP */
	for (i = 1; i <= Ss_pair_cnt; i++) {
		if (*Oldstring[i] EQ mark[0]
			OR

			/* FOR NON-DELIMITED ORIGINAL STRINGS (WHICH MUST HAVE BEEN
				SUFFIXED WITH '@' IN THE SSn FILE) */

			(mark[0] EQ Pre_ss_delim
			AND
			*Oldstring[i] NE Post_ss_delim))

			substitute (bf, Oldstring[i], Newstring[i]);
	}

	/* RESTORE FONT IF REQUESTED */
	if (mark[0] EQ Post_ss_delim AND Font_o) {
		sprintf(newrf, "%s%d%s", Font_o, Font, Font_c);
		substitute(bf, Oldstring[0], newrf);
			/* Oldstring[0] was initialized to "}RF}" in init_ss */
	}
	
	/* CONVERT }} TO } AND {{ TO { */
	if (bf[0] NE COMMAND) {
		double_delim[0] = double_delim[1] = mark[0];
		double_delim[2] = NULL;
		substitute (bf, double_delim, mark);
	}

#ifdef DEBUG
	if (Debug) checkbig("ss done:",bf);
#endif
}

/*------------------------------------------------------------------------*/
expantab(bf)
	char *bf;
	{
	int b, t, inss, pos;
	char tmp[BIGBUF];
	
	/* IF THERE'S NO TAB, RETURN */
	if ((b=instr(0,bf,"\t")) EQ -1) return(0);
	
	/* IF THERE'S A TAB, TRANSFER PRE-TAB STRING TO TMP */
	for (t=0, pos=0; t<b; t++, pos++) tmp[t] = bf[t];
	t = b;
	
	/* EXPAND TABS, TAKING POST-SUBSTITUTIONS INTO ACCOUNT */
	inss = 0;
	while (bf[b]) {
		if (bf[b] NE TAB) {
			if (bf[b] EQ Post_ss_delim) {
				if (!inss) inss = YES; /* OPENING DELIM */
				if (!inss) pos++;
				if (inss) inss = NO; /* CLOSING DELIM */
			}
			else if (!inss) pos++;
			tmp[t++] = bf[b++];
		}
		else {
			do {
				tmp[t++] = SPACE;
				pos++;
			} while (pos%Tabval);
			b++;
		}
	}
	tmp[t] = NULL;
	strcpy(bf, tmp);
}
/*------------------------------------------------------------------------*/
/* FNNLGETS = File-No-NewLine-GET-String
SAME AS FGETS BUT STRING LEFT IN S IS STRIPPED OF THE TERMINAL NEWLINE.
RAPGETS: SETS Press_flg +  FOR EVEN NUMBER OF Pre_ss_delim, - FOR ODD,
0 FOR NONE. */

rapgets(s,fp)
	char *s;
	FILE *fp;
	{
	char *p;
	int length;
	Press_flg = 0;
	if ((length = biggets(s,fp)) EQ 0) return(0);
	In_linecnt++;
	if (Pre_ss) for(p = s; *p; p++) {
		if (*p EQ Pre_ss_delim) {
			if (Press_flg >= 0) Press_flg = -1;
			else Press_flg = 1;
		}
	}
	/* TRIMS OFF TERMINAL NEWLINE */
	if (s[length-1] EQ '\n') s[length-1] = NULL;
	return(1);
}
/*------------------------------------------------------------------------*/
checkbig(m,s)
/* ROUTINE TO WRITE STRINGS TOO BIG FOR FPRINTF TO STDERR */
	char *m, *s;
	{
	while (*m) putc(*m++,STDERR);
	if (*s) {
		putc('<',STDERR);
		while (*s) putc(*s++,STDERR);
		if (*(s-1) NE NEWLINE)
			fputs(">\n",STDERR);
	}
}
/*------------------------------------------------------------------------*/
/*
	fgets:
	This next function is like "gets", except that
	a) the line is taken from a buffered input file instead
	of from the console, and b) the newline is INCLUDED in
	the string and followed by a null byte. 
	
	This one is a little tricky due to the CP/M convention
	of having a carriage-return AND a linefeed character
	at the end of every text line. In order to make text
	easier to deal with from C programs, this function (fgets)
	automatically strips off the CR from any CR-LF combinations
	that come in from the file. Any CR characters not im-
	mediately followed by LF are left intact. The LF
	is included as part of the string, and is followed
	by a null byte. (Note that LF equals "newline".)
	There is no limit to how long a line
	can be here; care should be taken to make sure the
	string pointer passed to fgets points to an area
	large enough to accept any possible line length
	(a line must be terminated by a newline (LF, or '\n')
	character before it is considered complete.)

	The value NULL (defined to be 0 here) is returned
	on EOF, whether it be a physical EOF (attempting to
	read past last sector of the file) OR a logical EOF
	(encountered a control-Z.) The 1.3 version didn't
	recognize logical EOFs, because I did't realize how
	SIMPLE it was to implement a buffered I/O "ungetc"
	function.
*/

int biggets(s,iobuf)
	char *s;
	FILE *iobuf;
	{
	int count, c;
	char *cptr;
	count = BIGBUF-1;
	cptr = s;
	if ( (c = getc(iobuf)) == EOF) return NULL;

	do {
		if ((*cptr++ = c) == '\n') {
			if (cptr>s+1 && *(cptr-2) == '\r')
				*(--cptr - 1) = '\n';
			break;
		}
	} while (count-- && (c=getc(iobuf)) != EOF);
	if (count <= 0) fprintf(STDERR,
"Line %d is too long [biggets()]; input lines must be <%d characters.\n",
		In_linecnt,BIGBUF);
	if (c == EOF) ungetc(CPMEOF, iobuf);	/* push back control-Z */
	*cptr = NULL;
#ifdef DEBUG
	if(Debug) checkbig("BIGGETS:", s);
#endif
	return (BIGBUF-1 - count);
}

/*------------------------------------------------------------------------*/
/* DELETE TERMINAL COMMENTS PRECEDED WITH "!"; "!!" = "!" */

de_comment(buf)
	char *buf;
	{
	int i;
	i = 0;
	while ((i = instr(i,buf,"!")) != -1) {
		if (buf[i+1] == '!') {
			strcpy(buf+i,buf+i+1); /* "!!" = "!" */
			i++;
		}
		else buf[i] = NULL; /* CHOP OFF COMMENTS */
	}
}
/*------------------------------------------------------------------------*/
updown(buf,start,direction)

/* CALLED WHEN
	buf CONTAINS A STRING INCLUDING "{{u" OR "{d" AT buf[start]
	direction IS 'u' OR 'd'

	THIS FUNCTION REPLACES, FOR EXAMPLE
	
		...{u51{Cr release...
		
	WITH
	
		...}u1}51}d2}Cr release...

	THUS THE FOLLOWING MUST BE DEFINED:
	
		}u1}, }d2} and }d1}, }u2}

	FOR EXAMPLE:
	
		}u1}
		}u}}font2} !NOTE NO SPACES ALLOWED SINCE COMPRESSION PRECEDES
		
		}d2}
		}d}}RF}
	*/

	char *buf;
	int start, direction;
	{
	int stop;
	char *after, left[MAXLINE], delim_str[2];
	char b1[5], b2[5], u1[5], u2[5];
	char post[5];

	delim_str[0] = Pre_ss_delim;
	delim_str[1] = NULL;
	makebu(b1, b2, u1, u2);

	after = Ulbuf;
	buf[start] = NULL;
	strcpy(after, buf);
	switch (direction) {
		case 'u':
			strcat(after, posttwo(post, 'u', '1'));
			break;
		case 'd':
			strcat(after, posttwo(post, 'd', '1'));
			break;
		case '_':
			strcat(after, u1);
			break;
		case '!':
			strcat(after, b1);
			break;
	}
	stop = instr(start+2,buf, delim_str);
	if (stop NE ERROR) buf[stop] = NULL;
	strcat(after, buf+start+2);
	switch (direction) {
		case 'u':
			strcat(after, posttwo(post, 'd', '2'));
			break;
		case 'd':
			strcat(after, posttwo(post, 'u', '2'));
			break;
		case '_':
			strcat(after, u2);
			break;
		case '!':
			strcat(after, b2);
			break;
	}
	if (stop NE ERROR) strcat(after,buf+stop+1);
	strcpy(buf,after);
}
/*------------------------------------------------------------------------*/
char *posttwo(bf, one, two)
	char *bf, one, two;
	{
	bf[0] = Post_ss_delim;
	bf[1] = one;
	bf[2] = two;
	bf[3] = Post_ss_delim;
	bf[4] = NULL;
	return(bf);
}
/*------------------------------------------------------------------------*/
getpairs(fpss, sslist)
	FILE *fpss;
	char *sslist;
	{
	char bf2[MAXLINE], pssd, *o, *n, built_in;
	int i, old, oldold, eof_flag, summary, lastchar, u1, u2, b1, b2;

	u1 = u2 = b1 = b2 = NO;
/*	summary = !Ss_pair_cnt;*/
	summary = YES;
	if (Ss_pair_count) summary = NO;
	if (Ss_pair_cnt EQ -1) Ss_pair_cnt = 0;
	old = YES;
	eof_flag = NO;

	while(ssgets(Inbuf,fpss)) { /* OR FNNGETS */

#ifdef DEBUG
		if (Debug) checkbig("getpairs",Inbuf);
#endif
		de_comment(Inbuf);

		/* SKIP BLANK LINES WHILE LOOKING FOR OLD STRING */
		if (old && !strlen(Inbuf)) continue;

		/* CONCATENATE NEW STRINGS UNTIL BLANK LINE */
		if (!old) {
			if ((ssgets(bf2,fpss)) EQ 0) {
				eof_flag = YES;
				goto eof;
			}
			de_comment(bf2);
			while (strlen(bf2)) {
				if (Inbuf[strlen(Inbuf)-1] EQ '_')
					Inbuf[strlen(Inbuf)-1] = NULL;
				else strcat(Inbuf,"\r\n");
				strcat(Inbuf, bf2);
				if ((ssgets(bf2,fpss)) EQ 0) {
					eof_flag = YES;
					break;
				}
				de_comment(bf2);
			}
		}
eof:
#ifdef DEBUG
		if (Debug) {
			fprintf(STDERR,"GOT(%d)",strlen(Inbuf));
			checkbig("",Inbuf);
		}
#endif
		if(Ss_pair_cnt+1 > MAXPAIRS-1) {
			printf("Too many pairs in SS file (Max = %d).\n",
				MAXPAIRS-1);
			exit(0);
		}

		substitute(Inbuf,"|N","\n");
		substitute(Inbuf,"|R","\r");
		lastchar = strlen(Inbuf)-1;
		if (old) {
			if (equal(Inbuf,"i/o port")) {
				io_port(fpss);
				continue;
			}
			if (equal(Inbuf,"baud")) {
				baud(fpss);
				continue;
			}
			if (Inbuf[lastchar] EQ '@') {
				Inbuf[lastchar] = NULL;
				Press_all = YES;
				lastchar--;
			}
			else {
				/* LEADING SPACE IN }...} MEANS NOT BUILT IN */
/*				built_in = !(Inbuf[1] EQ ' ');*/
				built_in = YES;
/*				if (Inbuf[1] EQ ' ') built_in = NO; defeated 2.21 */
/*				delchars(Inbuf,0," "); defeated 2.21 */
				lastchar = strlen(Inbuf)-1;

				if ((Inbuf[0] NE Pre_ss_delim
				AND Inbuf[0] NE Post_ss_delim)
				OR
				(Inbuf[lastchar] NE Pre_ss_delim
				AND Inbuf[lastchar] NE Post_ss_delim)
				OR
				(Inbuf[0] EQ Pre_ss_delim
				AND Inbuf[lastchar] EQ Post_ss_delim)
				OR
				(Inbuf[lastchar] EQ Pre_ss_delim
				AND Inbuf[0] EQ Post_ss_delim)){

					fprintf(STDERR,
"\nInvalid delimiters (%c,%c) around original string in %s:\n\"%s\"\n",
						Pre_ss_delim, Post_ss_delim,sslist,Inbuf);
					exit(0);
				}
			}
			oldold = 0;
			for (i=1; i<=Ss_pair_cnt; i++) {
				if (equal(Inbuf, Oldstring[i])) {
					oldold = i;
					break;
				}
			}
			if (!oldold) Oldstring [Ss_pair_cnt + 1] =
				pack(Oldnewbuf,&Oldnewtot,MAXPACK,Inbuf);
			old = NO;
#ifdef DEBUG
			if (Debug) fprintf(STDERR,
				"Old:<%s>[Oldnewtot=%d]\n",Inbuf,Oldnewtot);
#endif
			if (Oldnewtot >= MAXPACK) {
				fprintf(STDERR,
"Out of storage space for string substitution pairs during reading of %s.\n",
					sslist);
				exit(0);
			}
		}
		else {
			if (oldold) i = oldold;
			else i = Ss_pair_cnt + 1;
			Newstring [i] =
				pack(Oldnewbuf,&Oldnewtot,MAXPACK,Inbuf);

			if (built_in) {
				o = 1 + Oldstring[Ss_pair_cnt+1];
				n = Newstring[Ss_pair_cnt+1];
				pssd = Post_ss_delim;
				if (lefteq(o,"init") EQ 4 AND o[4] EQ pssd) Init = n;
				if (lefteq(o,"deinit") EQ 6 AND o[6] EQ pssd) Deinit = n;
				if (lefteq(o,"left") EQ 4 AND o[4] EQ pssd)
					Flush_left = n;
				if (lefteq(o,"right") EQ 5 AND o[5] EQ pssd)
					Flush_right = n;
				if (lefteq(o,"center") EQ 6 AND o[6] EQ pssd)
					Center_mode = n;
				if (lefteq(o,"just1") EQ 5 AND o[5] EQ pssd) Just_one = n;
				if (lefteq(o,"just2") EQ 5 AND o[5] EQ pssd) Just_two = n;
				if (lefteq(o,"fn-open") EQ 7 AND o[7] EQ pssd) Font_o = n;
				if (lefteq(o,"fn-close") EQ 8 AND o[8] EQ pssd)
					Font_c = n;
				if (lefteq(o,"forceprint") EQ 10 AND o[10] EQ pssd)
					Forceprint = n;
				if (lefteq(o,"u+") EQ 2 AND o[2] EQ pssd) {
					Ul_on = n;
					u1 = YES;
				}
				if (lefteq(o,"u-") EQ 2 AND o[2] EQ pssd) {
					Ul_off = n;
					u2 = YES;
				}
				if (lefteq(o,"b+") EQ 2 AND o[2] EQ pssd) {
					Bo_on = n;
					b1 = YES;
				}
				if (lefteq(o,"b-") EQ 2 AND o[2] EQ pssd) {
					Bo_off = n;
					b2 = YES;
				}
				if (lefteq(o,"bo-shift") EQ 8 AND o[8] EQ pssd)
					Bo_shift = n;
				if (lefteq(o,"bo-restore") EQ 10 AND o[10] EQ pssd)
					Bo_restore = n;
				if (lefteq(o,"ul-shift") EQ 8 AND o[8] EQ pssd)
					Ul_shift = n;
				if (lefteq(o,"ul-restore") EQ 10 AND o[10] EQ pssd)
					Ul_restore = n;
				if (lefteq(o,"rlf") EQ 3 AND o[3] EQ pssd) Rev_lf = n;
			}
			old = YES;
#ifdef DEBUG
			if (Debug) {
				fprintf(STDERR,"New[Oldnewtot=%d]:",Oldnewtot);
				checkbig("",Inbuf);
			}
#endif
			if (Oldnewtot >= MAXPACK) {
				fprintf(STDERR,
"Out of storage space for string substitution pairs during reading of %s.\n",
					sslist);
				exit(0);
			}
		}
		if (old AND !oldold) Ss_pair_cnt++;
		if (eof_flag) break;
	}
	if (!Screen_size OR summary) {
		fprintf(STDERR,"\tSS Pair count = %d\n",Ss_pair_cnt);
		old = ((MAXPACK + 50)/100);
		fprintf(STDERR,"\tSS Pair Buffer %d%% Full.\n",
			((((10 * Oldnewtot)/old) +5)/10));
	}
	if (!Ss_pair_cnt) Ss_pair_cnt = -1;
	if (u1 AND u2 AND b1 AND b2) return(YES);
	else return(NO);
}
/*------------------------------------------------------------------------*/
ssgets(buf, fp)
	char *buf;
	FILE *fp;
	{
	if (!rapgets(buf, fp)) return(NO);
	if ((lefteq(buf,".ss 0")) EQ 5) return(NO);
	if ((lefteq(buf,".ss 1")) EQ 5) {
		fprintf(STDERR,
"Error: Second consecutive '.ss 1' command at line %d\n", In_linecnt);
		exit(0);
	}
	if ((lefteq(buf,".ss")) EQ 3)
		fprintf(STDERR,
"Warning: possible invalid '.ss' command at line %d\n", In_linecnt);
	return(YES);
}
/*------------------------------------------------------------------------*/
/*	END of RAP4.C	*/
/*------------------------------------------------------------------------*/
r_cnt++;
		