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

	MARTZ-JR.C CONTAINS:

		jindex(s,t)
		jsubstr(s,t,x,y)
		leftc1(c)
		lefteq(s,match)
		lower(s)
		only(s,o)
	char *pack (dest_buf, pt_dest_cnt, max_cnt, src_buf)
		pad(buf, side, width, c)
		reset()
		reverse(s)
		righteq(s, m)
		rindex(s,k,maxpos)
		ring(times)
		rshift(buf,dist)
*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
jindex(s,t)

/* Finds first occurrence of "t" in "s", and returns the index (position in
"s" where "t" begins). Returns -1 if "t" is not found in "s"; returns 0 if "t"
is a null string.

This function supplied by Jeremy Poole, HSCF at Harvard School of Public
Health. */

char s[], t[];
{       register char *p1, *p2, *p3;
	if (!*t)
		return(0);
	for (p1 = s; *p1; p1++)
		if (*p1 == *t)
		{       for (p2 = p1+1, p3 = t+1; *p3 && *p2 == *p3; p2++, p3++);
			if (!*p3)
				return(p1 - s);
		}
	return(-1);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
jsubstr(s,t,x,y)

/* puts into s the substring of t from position x onward
including y characters. if y < 0 then all characters past
position x are included. Returns actual number of characters put
into s.

This function supplied by Jeremy Poole, HSCF at Harvard School of Public
Health. */

char s[], t[];
int x, y;
{	register i, j;
	register char *pp;
	for (j = 0, pp = t; *pp++; j++);
	if (x < 0)
		x = 0;
	if (x > j)
		x = j;
	if (y < 0)
		y = j;
	for (i = 0, pp = &t[x]; i < y && (*s++ = *pp++); i++);
	if (i == y)
		*s = '\0';
	return(i);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
leftc1(c)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* ARGUMENT RETURNED IS THE SINGLE LEFTMOST NONBLANK CHARACTER
OF THE NEXT INPUT LINE.  THIS FUNCTION MUST BE CALLED WITH
AN ADDRESS ARGUMENT, SO IF CALLING ROUTINE DECLARES char c;
THEN CALL SHOULD BE leftc1(&c).
UPPER CASE IS CONVERTED TO LOWER. */

        char *c;
        {
        char s[MAXLINE];
        int i,len;
        len = gets(s);
        /* LOOP TO STRIP OFF LEADING SPACES */
        i = 0;
        while (s[i] == ' ') ++i;
        *c=s[i];
        *c = (isupper(*c) ? tolower(*c) : *c);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
lefteq(s,match)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* This function returns the number of characters in "match" which match the
characters in "s" beginning with the first non-blank character in "s". */

/* Example: lefteq("  init","initialize") returns 4. */

	char *s, *match;
	{
	char *start;
	while (isspace(*s)) s++;
	start = s;
	for (; *match; s++, match++) {
		if (*s != *match) break;
	}
	return (s - start);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
lower(s)

/* CONVERTS ALL UPPER CASE LETTERS IN s TO UPPER CASE */
	char *s;
	{
	for (; *s; s++) *s = tolower(*s);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
only(s,o)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* Returns TRUE if string "s" contains only characters found in
string "o" (in any order) */

	char *s, *o;
	{
	char *p;
	for (;*s;s++) {
		for (p=o; *p; p++) {
			if (*p == *s) goto ok;
		}
		return(FALSE);
ok:		continue;
	}
	return(TRUE);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *pack (dest_buf, pt_dest_cnt, max_cnt, src_buf)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* PACKS A NULL TERMINATED STRING ONTO THE END OF DEST_BUF BEGINNING AT
DEST_CNT; EACH STRING PACKED ONTO THE END OF DEST_BUF IS DELIMITED FROM THE
PRECEDING STRING WITH A NULL. THUS, A PACKED BUFFER MUST BE USED IN
CONJUNCTION WITH AN ARRAY OF POINTERS TO STRINGS. PACK RETURNS THE POINTER TO
THE STRING PACKED, OR ZERO UPON OVERFLOW OF DEST_BUF. */

	char *dest_buf, *src_buf;
	int *pt_dest_cnt, max_cnt;
	{
	int newcnt;
	char *pointer;
	max_cnt -= 1;
	newcnt = *pt_dest_cnt + strlen(src_buf) + 1;
	if (newcnt > max_cnt) return (0);
	pointer = dest_buf + *pt_dest_cnt;
	strcpy(pointer, src_buf);
	*pt_dest_cnt = newcnt;
	return (pointer);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
pad(buf, side, width, c)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* PADS STRING IN buf OUT TO width WITH SPACES, ADDING SPACES TO
R OR L side. */

	char *buf, side, c;
	int width;
	{
	int i, len, toadd, start;
	len = strlen(buf);
	toadd = width - len;
	if (side EQ 'r' OR side EQ 'R') start = len;
	else {
		start = 0;
		rshift(buf,toadd);
	}
	for (i=1; i<= toadd; i++)
		buf[start++] = c;
	if (side EQ 'r' OR side EQ 'R') buf[len+toadd] = NULL;
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
reset() {
	int disk;

/* Copyright (c) 1983 by Eric Martz, Amherst MA */
/* RESETS CP/M 2.2 DISK SYSTEM */

	disk = bdos(25, 0); /* GET CURRENT DISK */
	bdos(13, 0);		/* RESET */
	bdos(14, disk);	/* SELECT SAME DISK */
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
reverse(s)

/* The sequence of characters in the string "s" is reversed. */

        char *s;
        {
        int c, i, j;
        for (i=0,j=strlen(s)-1;i<j;i++,j--) {
                c=s[i];
                s[i]=s[j];
                s[j]=c;
        }
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
righteq(s, m)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* RETURNS THE NUMBER OF CHARACTERS AT THE RIGHT OF s WHICH MATCH THOSE IN
m. EXAMPLE: righteq("FILE.SS", ".SS") RETURNS 3; righteq("FILE.RAP", "SNAP")
RETURNS 2 */

	char *s, *m;
	{
	char *es, *em;
	int count;
	count = 0;

	/* RETURN 0 IF EITHER ARGUMENT IS A NULL STRING */
	if (*s EQ NULL OR *m EQ NULL) return(count);

	/* SET END OF STRING POINTERS TO ENDS */
	es = s;
	em = m;
	while (*es NE NULL) es++;
	while (*em NE NULL) em++;
	es--;
	em--;

	/* COUNT MATCHES */	
	while (*(es--) EQ *(em--)) {
		count++;
		if (es < s OR em < m) return(count);
	}
	return (count);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
rindex(s,k,maxpos)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* FIND INDEX OF RIGHTMOST OCCURRENCE OF KEY STRING "K" IN STRING "S"
   WHERE INDEX <= MAXPOS */

/* This function was written to break lines between words. For example, if
lines of text are to be fitted onto a page width of 78 characters, you need to
find the rightmost occurrence of " " in "s" which occurs at or before position
78. Thus, "rindex(s," ",78);". */

	char *s, *k;
	int maxpos;
	{
	int start, i, is, ik;
	if (maxpos < 0) maxpos = 0;
	start = strlen(s)-1;
	if (maxpos < start) start = maxpos;
	for (i = start; i >= 0; i--) {
		for (is=i,ik=0;k[ik];is++,ik++)
			if (s[is] != k[ik]) goto decrement;
		return(i);
decrement:
		continue;
	}
	return(ERROR);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ring(times)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* RINGS BELL times TIMES; IF TIMES IS ZERO, RINGS FOREVER */
	int times;
	{
	int i, dec;
	if (!times) dec = 0;
	else dec = 1;
	times++;
	while(times -= dec) {
		putchar(BELL);
		for (i=1; i<=10000; i++); /* DELAY */
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
rshift(buf,dist)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* SHIFTS STRING IN buf dist POSITIONS TO THE RIGHT */
/* EXAMPLE: rshift("abc",2) -> "ababc" */

	char *buf;
	int dist;
	{
	int len, i;
	len = strlen(buf);
	for (i=len; i>=0; i--) {
		buf[i+dist] = buf[i];
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*	END OF MARTZ-JR.C	*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
o decrement;
		return(i);
decrement:
		continue;
	}
	return(ERROR);
}
/*++++++++++++++++++++