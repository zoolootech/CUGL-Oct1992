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

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* MARTZLIB.C IS A LIBRARY OF FUNCTIONS OF GENERAL USE.

	MARTZ-EI.C CONTAINS:

		equal(s1,s2)
		fbrkout(width, buf, tnull, force_nl, fpout)
		findwords(s,p)
		firstone(buf,tofind)
    char *fnnlgetl(s,fp,max)
    char *fnnlgets(s,fp)
		freq(buf,item)
    char *ftgets(buf,fp,maxcount)
		getint(string,ptr)
		getwrds(message,wordbuf,pp)
		goodfile(message,name)
		icata(n,stout)
		instr(i,s,t)
*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
equal(s1, s2)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* RETURNS YES IF THE STRINGS IN s1 AND s2 ARE EQUAL, ELSE NO */

	char *s1, *s2;
	{
	if (strcmp(s1, s2) EQ 0) return(YES);
	else return(NO);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
fbrkout(width, buf, tnull, force_nl, fpout, displayall, indent)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* PUTS INTO fpout CONTENTS OF buf, BROKEN AT SPACES, TABS, OR
NEWLINES INTO LINES OF LENGTH <= width.

--->  WARNING!!! FBRKOUT() MODIFIES THE CONTENTS OF buf!!! <---

BREAK IS FORCED AT THE STRING force_nl (WHICH IS DELETED FROM
OUTPUT). IF YOU DO NOT WISH TO FORCE BREAKS, CALL WITH 0.

IF displayall IS YES, UNPRINTABLE CHARACTERS ARE EXPANDED TO
PRINTABLE FORM.

IF indent IS YES, THE 2ND-NTH LINES PUT OUT BY FBRKOUT ARE
INDENTED 5 SPACES.

IF TNULL (POINTER TO TERMINAL NULL OF buf) IS NOT AVAILABLE, CALL
WITH ZERO. TNULL IS NOT NECESSARY BUT IMPROVES EFFICIENCY(?). */

	char *buf, *tnull, *force_nl;
	int width, displayall, indent;
	FILE *fpout;
	{
	char *brk, out[4];
	int len, force, firstline, lesswidth;

	if (force_nl EQ 0) force_nl = "";
	force = ERROR;
	if (tnull EQ 0) tnull = buf + strlen(buf);
	if ((tnull - buf) EQ 0) { /* GUARANTEE NEWLINE FOR EMPTY BUFFER */
		fputs("\n", fpout);
		return(0);
	}
	firstline = YES;
	while ((len = (tnull - buf)) > 0) {

		/* INDENT IF NEEDED AND SET lesswidth */
		if (indent AND !firstline) {
			lesswidth = width - 5;
			fputs("     ", fpout); /* 5 spaces */
		}
		else lesswidth = width;

		if (len > lesswidth) { /* IF THE LINE IS TOO LONG */
			/* WE'LL ASSUME THERE IS A SPACE BEFORE lesswidth! */
			/* FIND A BREAKPOINT AT A SPACE */
			brk = buf + rindex(buf, " ", lesswidth);
		}
		else brk = tnull;

		/* LOOK FOR force_nl */
		if (force_nl[0]) force = instr(0, buf, force_nl);

		/* SET BREAK AT WHICHEVER OCCURS FIRST */
		if (force NE ERROR AND (brk > buf + force)) brk = buf + force;

		/* BREAK THE BUFFER */
		*brk = NULL;

		/* WRITE OUT THE LINE SEGMENT */
		while (*buf) {
			if (displayall) {
				dispexp(*buf, out);
				fputs(out, fpout);
			}
			else putc(*buf, fpout);
			buf++;
		}
		fputs("\n", fpout); /* putc() does not expand \n */


		/* SET THE POINTER PAST END OF STRING */
		buf = brk + strlen(force_nl);
		if(!*force_nl) buf++;

		firstline = NO;
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
findwords(s,p)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* FINDS THE WHITESPACE-DELIMITED WORDS IN STRING "s" (WHERE WHITESPACE
INCLUDES SPACES, TABS AND NEWLINES). RETURNS COUNT OF WORDS FOUND. EACH WORD
IS POINTED TO BY A MEMBER OF THE ARRAY "p". THE STRING POINTED TO BY "s" IS
COMPRESSED INTO WORDS DELIMITED ONLY BY NULLS. THE FIRST WORD IS POINTED TO BY
p[1], NOT p[0]. (p[0] IS WASTED.) */

	char *s, **p;
	{
	int wcnt, atend;
	char *beginw, *endw, *nextat;
	blankout(s,"\t\n");
	atend = FALSE;
	wcnt = 0;
	beginw = nextat = s;
	while(1){
		for(;*beginw; beginw++)
			if(*beginw != ' ') break;
		if (!*beginw) return(wcnt);
		for(endw=beginw; *endw; endw++)
			if (*endw == ' ') break;
		if (!*endw) atend = TRUE;
		*endw = '\0';
		wcnt += 1;
		p[wcnt] = nextat;
		if (nextat != beginw) strcpy(nextat,beginw);
		if (atend) return(wcnt);
		nextat += (endw + 1) - beginw;
		beginw = endw + 1;
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *firstone(buf, tofind)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* RETURNS POINTER [NOT AN INDEX!] TO FIRST OCCURRENCE IN buf OF
A CHARACTER INCLUDED IN THE STRING tofind, OR NULL IF NOT FOUND.

*/
	char *buf, *tofind;
	{
	char *p, *f;
	for (p=buf; *p; p++) {
		f = tofind;
		while (*f)
			if (*p == *(f++))
				return (p);
	}
	return(0);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *fnnlgetl(s,fp,max)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* FNNLGET = File-No-NewLine-GET-Longstring

SAME AS FTGETS BUT STRING LEFT IN S IS STRIPPED OF THE TERMINAL NEWLINE
AND BUFFER LENGTH IS AN ARGUMENT FOR LONG INPUT LINES */

	char *s;
	FILE *fp;
	{
	char *tnull;
	if (!(tnull = ftgets(s,fp,max))) return(0);
	if (*(--tnull) EQ '\n') *tnull = NULL;
	return (tnull);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *fnnlgets(s,fp)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* FNNLGETS = File-No-NewLine-GET-String

SAME AS FTGETS BUT STRING LEFT IN S IS STRIPPED OF THE TERMINAL NEWLINE */

	char *s;
	FILE *fp;
	{
	char *tnull;
	if (!(tnull = ftgets(s,fp,MAXLINE))) return(0);
	if (*(--tnull) EQ '\n') *tnull = NULL;
	return (tnull);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
freq(buf, item)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* RETURNS THE FREQUENCY OF item IN buf. FREQ IS FOR
NONOVERLAPPING OCCURRENCES. THUS IF buf CONTAINS "...." AND item
CONTAINS  "..", FREQ IS 2 NOT 3. */

	char *buf, *item;
	{
	int i, count, start, itemlen;
	count = start = 0;
	itemlen = strlen(item);
	while ((i=instr(start,buf,item)) NE ERROR) {
		count++;
		start = i + itemlen;
	}
	return(count);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *ftgets(buf,fp,maxcount)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* This is BDSC fgets (from stdlib2.c) with the following modifications:
	Returns pointer to terminal null in buf;
	The buffer length restriction (formerly MAXLINE) has been added as
		an argument.
		
ftgets() retains inclusion of newline in buf (like fgets).

*/
	char *buf;
	FILE *fp;
	int maxcount;
	{
	int c;
	char *cptr;
	cptr = buf;
	if ( (c = rawgetc(fp)) == CPMEOF || c == EOF) return NULL;

	do {
		if ((*cptr++ = c) == '\n') {
			if (cptr>buf+1 && *(cptr-2) == '\r')
				*(--cptr - 1) = '\n';
			break;
		}
	 } while (maxcount-- && (c=rawgetc(fp)) != EOF && c != CPMEOF);

	/* N.B. IF GETC IS USED, THE MARTZ MODIFIED VERSION NEVER
	RETURNS CPMEOF HENCE ^Z IS NOT PUSHED BACK AND FTGETS FAILS
	WHEN THE LAST LINE DOES NOT END IN A \r\n! */
	if (c == CPMEOF) ungetc(c,fp);	/* push back control-Z */

	*cptr = '\0';
	return (cptr);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
getint(string,ptr) 

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* Prints "string" suffixed with a "? ", then converts next line
of input to an integer. If integer is valid (-32767 to 32767) it
is stored in the location pointed to by the argument "ptr". If
invalid, a diagnostic is printed, the question in "string" is
repeated, and another line of input is examined. Thus, this
function will not return until a valid integer is obtained.
Nothing is returned (?! Why not return the integer found instead
of making it an argument?). */

	char *string;
	int *ptr;
	{
	char numst[MAXLINE];
	int n;
	n = 1;
	while (n != 0) {
		printf("%s? ",string);
		gets(numst);
		n = ati(ptr,numst);
		if (n == -1) printf("Integer not found\n");
		if (n == 1) printf(
			"Absolute value of integer too large: maximum is 32767\n");
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
getwrds(message, wordbuf, pp)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* PRINTS message, GETS A LINE INTO wordbuf, BLANKS OUT COMMAS,
PUTS NULL DELIMITED WORDS IN wordbuf, LEAVING POINTERS TO EACH IN
pp, RETURNS COUNT OF WORDS FOUND. 

CAN BE USED WITH ati() TO GET A SERIES OF ONE OR MORE INTEGERS.

*/

	char *message, *wordbuf, **pp;
	{
	fprintf(STDERR,"%s",message);
	gets(wordbuf);
	blankout(wordbuf,",");
	return (findword(wordbuf,pp));
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
FILE *goodfile(message, name)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* PRINTS MESSAGE AND INPUTS FILE NAME. THIS IS REPEATED UNTIL
NAME IS A VALID CP/M FILENAME AND IS SUCCESSFULLY OPENED FOR
READING. SUPPLIES *name AND RETURNS FILE POINTER. */
	
	char *message, *name;
	{
	FILE *fp;
	int check;
	char *pp[16], buf[MAXLINE];
	check = 0;
	while (check NE 1) {
		check = getwrds(message, buf, pp);
		if (check NE 1) {
			fprintf(STDERR,"Single name please.\n");
			continue;
		}
		if (badname(pp[1])) {
			check = 0;
			continue;
		}
		strcpy(name,pp[1]);
		fp = fopen(name,"r-");
		if (fp EQ 0) {
			check = 0;
			fprintf(STDERR,"File \"%s\" does not exist.\n",name);
		}
	}
	return(fp);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
icata(n,stout)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* The integer "n" is converted to an ascii string which is appended
(concatenated) onto the end of the null-terminated string in "stout". */

        int n;
        char *stout;
        {
        char s[12];
        int i, sign;
        if ((sign=n)<0) n = -n;
        i=0;
        do {
                s[i++] = n % 10 + '0';
        } while ((n /= 10)>0);
        if (sign<0) s[i++] = '-';
        s[i]='\0';
        reverse(s);
        strcat(stout,s);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
instr(i,s,t)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* Finds first occurrence of string "t" in "s", and returns the
index (position in "s" where "t" begins). Returns -1 if "t" is
not found in "s" or if "t" is a null string. The search for "t"
begins at the "i"th position in "s".

This is "jindex" modified to begin at the "i"th position; also
modified because unlike instr "jindex" returns 0 if "t" is a null
string. */

char s[], t[];
int i;
{       register char *p1, *p2, *p3;
        if (!*t)
                return(-1);
        for (p1 = s+i; *p1; p1++)
                if (*p1 == *t) {
	                for (p2 = p1+1, p3 = t+1; *p3 && *p2 == *p3; p2++, p3++);
                     if (!*p3)
                                return(p1 - s);
                }
        return(-1);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*	END OF MARTZ-EI.C	*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
+++++++*/
instr(i,s,t)

/* Copyright (