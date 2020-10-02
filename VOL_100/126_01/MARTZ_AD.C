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

	MARTZ-AD.C CONTAINS:

		argmatch(match, &argc, argv, delete)
		ati(n,s)
		badname(name)
		blankout(s,b)
		charq(s,a)
		checkint(s,i,buf,min,max,def)
		chopwhite(side, buf)
		copy(to, from, count)
		delchars(start,stop,todelete)
		dispchar(c)
		dispexp(c, out);
*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
char *argmatch(match, p_argc, argv, delete)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* CALL: string = argmatch(match, &argc, argv, delete);

RETURNS 0 IF NO ARGUMENT MATCHES match.

ELSE RETURNS THE STRING ADDRESS OF THE FIRST MATCHING ARGUMENT,
AFTER DELETING (IF (delete)) REFERENCE TO THIS ARGUMENT IN argc, argv.

match CAN HAVE LEADING OR TRAILING AMBIGUITIES REPRESENTED AS *.
* CANNOT BE IMBEDDED, NOR IS *X* PERMITTED.

EXAMPLES:

	if (arg = argmatch("-d*", &argc, argv, YES)) Debug = arg[1];
	
	if (name = argmatch("*.SS", &argc, argv, YES)) Fpin = fopen(name, "r");

*/
#define NONE 0
#define LEAD 1
#define TRAIL 2

	int *p_argc, delete;
	char *match, **argv;
	{
	int i, ambig, mlen, hit;
	char *arg;

/*
printf("ARGMATCH\n");
printf("match\"%s\"\n", match);
printf("argc = %d\n", *p_argc);
for (i=1; i< (*p_argc); i++) printf("\t\"%s\"\n", argv[i]);
*/
	ambig = NONE;
	
	if (match[0] EQ '*') {
		ambig = LEAD;
		match++;
	}
	
	if (match[strlen(match)-1] EQ '*') {
		ambig = TRAIL;
		match[strlen(match)-1] = NULL;
	}
	
	mlen = strlen(match);
	hit = NO;
	
	for (i=1; !hit AND i< (*p_argc) ; i++) {
		switch(ambig) {

			case LEAD:
				if (equal(match, argv[i]+strlen(argv[i])-mlen))
					hit = i;
				break;

			case TRAIL:
				if(lefteq(match, argv[i]) EQ mlen) hit = i;
				break;

			default:
				if(equal(match, argv[i])) hit = i;
		}
	}
	if (hit) {

		/* SAVE ARG */
		arg = argv[hit];
		
		/* DELETE ARG */
		if (delete) {
			(*p_argc)--;
			for (i=hit; i< (*p_argc) ; i++) argv[i] = argv[i+1];
		}

		/* RETURN POINTER TO ARGUMENT FOUND */
		return(arg);
	}
	else return(NO);
}
#undef NONE 0
#undef LEAD 1
#undef TRAIL 2
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
ati(n,s)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* LOOKS FOR FIRST INTEGER IN STRING
AFTER IGNORING LEADING BLANKS, TABS, AND ZEROS;
IF FIRST NON-WHITE IS EOS OR NON-DIGIT,
RETURNS -1; IF INTEGER EXCEEDS MAX,
RETURNS +1. */

        int *n;
        char *s;
        {
        int toobig, zer, eq, gr, m;
        char max[6];
        int i, sign;
        toobig = zer = eq = gr = m = 0;
        max[0]='\0';
        strcat(max,"32767");
        for (i=0; s[i]==' ' || s[i]=='\t'; i++)
                ; /* SKIP WHITE SPACE WITHIN LINE */
        sign = 1;
        if (s[i]=='+' || s[i]=='-')  /* SIGN */
                sign = (s[i++]=='+')? 1 : -1;
        for (;s[i]=='0'; i++) zer = 1;
                /* SKIP ZEROS */
        if (s[i]<'0' || s[i] >'9') {
                if (zer == 0) return(-1);
                else {
                        *n = 0;
                        return(0);
                }
        }
        for (*n=0; s[i]>='0' && s[i]<='9'; i++, m++) {
                if (m==5) return (1);
                if (s[i]>max[m]) gr += 1;
                if (s[i]==max[m]) eq += 1;
                if ((eq<(m+1)) && ((m+1) == (eq + gr))) toobig = 1;
                if ((toobig == 1) && (m == 4)) return (1);
                /* ABOVE 4 LINES DETECT INTEGERS >32767 */
                *n = (10 * *n) + (s[i] - '0');
        }
        *n = *n * sign;
        return (0);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
badname(dname)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* CHECKS FOR VALID CP/M FILENAME. RETURNS YES IF BAD, NO IF GOOD. */
/* ISSUES DETAILED ERROR MESSAGE IF BAD. */
/* THIS VERSION CHECKS FOR EMBEDDED SPACES */

	char *dname;
	{
	char left[16], right[32], name[32];
	int len, count;

	strcpy(name, dname);
	
	/* DOES NAME CONTAIN AN IMBEDDED SPACE? */
	if ((instr(0,name," ")) NE ERROR) goto bad;

	/* CHECK FOR : IN WRONG PLACE, OR TWO : */
	count = freq(name, ":");
	if (count >= 2) goto bad;
	if (count EQ 1) {
		if (name[1] NE ':') goto bad;
		/* NO CHECKING FOR VALID DRIVE LETTER */
		strcpy(name, name+2); /* DELETE DRIVE PREFIX */
		if (!*name) goto bad;
	}

	/* IS THE PREFIX TOO LONG > 8 ? */
	strcpy(right, name);
	len = todelim(left,right,".");
	if (len EQ ERROR) {
		strcpy(left,name);
		len = strlen(left);
		right[0] = NULL;
	}
	if (len > 8) {
bad:
		fprintf(STDERR,
"Invalid CP/M file name:\"%s\"\n",name);
		fprintf(STDERR,
"Name must have not more than 8 characters before\n");
		fprintf(STDERR,
"and 3 after the period. It may not include a space or any of the\n");
		fprintf(STDERR,
"characters <>,;=[]. It may have only one '.' and only one ':'.\n");
		fprintf(STDERR,
"The ':', if any, must be the second character.\n");
		return(1);
	}
	
	/* IS NAME EMPTY, NAMELY "."? */
	if (equal(name,".")) goto bad;

	/* A SECOND PERIOD IS BAD */
	if (firstone(right,".")) goto bad;

	/* THE TYPE MAY NOT EXCEED 3 CHARACTERS */
	if (strlen(right) > 3) goto bad;

	/* CHECK FOR BAD CHARACTERS */
	if (firstone(dname,"<>,;=[]")) goto bad;

	/* ITS OK! */
	return(0);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
blankout(s,b)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* WITHIN STRING S, BLANKS OUT ALL CHARACTERS IN THE STRING B */ 
/* Typical use: to blank out all acceptable delimiters (tab, ",",
	newline, ";", etc.) so that blanks become the only delimiters
	in a list of words/tokens/numbers. */

	char *s, *b;
	{
	int slen, blen, i, j;
	slen = strlen(s);
	blen = strlen(b);
	for(i=0; i<slen; i++) {
		for(j=0; j<blen; j++) {
			if (s[i] == b[j]) {
				s[i] = ' ';
				break;
			}
		}
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
charq(s,a)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* Character question. Prints the string in "s" suffixed with a question mark,
then obtains the first non-blank character from the input (discarding the
remainder of the input line). If the character is contained in the string "a"
(for answers), it is returned; else the question is reposed and another line
of input is examined.

Example:

	answer = charq("Type of sort (numeric, alphabetic, mixed)","nam");	*/

        char *s, *a;
        {
        char c;
        int i, ok;
        ok = 0;
        while (!ok) {
                printf("%s?",s);
                leftc1(&c);
                i = strlen(a);
                while (i+1) if (c == a[i--]) ok = 1;
        }
        return(c);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
checkint(s,i,buf,min,max,def)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* PRINTS THE MESSAGE s, THEN GETS A LINE FROM THE STANDARD INPUT
AND ATTEMPTS TO CONVERT THE FIRST WORD TO AN INTEGER. REPEATS
MESSAGE UNTIL A VALID INTEGER IS OBTAINED WITHIN THE RANGE min-max;
THE VALUE OBTAINED IS ASSIGNED TO THE CONTENTS OF THE ADDRESS i.
IF AN EMPTY LINE IS RETURNED, *i IS ASSIGNED THE DEFAULT VALUE def.
RETURNS NOTHING.
*/
	char *s, *buf;
	int *i, min, max, def;
	{
	int check;
	while (1) {
		printf(s,def);
		gets(buf);
		if (!buf[0]) { /* EMPTY LINE: DEFAULT */
			*i = def;
			return(0);
		}
		check = ati(i,buf);
		if (check < 0) {
			printf("Integer not found.\n");
			continue;
		}
		if (check > 0) {
			printf("Invalid integer (outside +/- 32767).\n");
			continue;
		}
		if (*i > max || *i < min) {
			printf("Out of range.\n");
			continue;
		}
		else return(0);
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
chopwhite(side, buf)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* CHOPS WHITESPACE (SPACES, TABS, NEWLINES) OFF SIDE ('r' RIGHT,
'l' LEFT, 'b' BOTH) */

	char side, *buf;
	{
	char *p;
	if (side EQ 'r' OR side EQ 'b') {
		p = buf;
		while(*p) p++;
		p--;
		while (isspace(*p)) p--;
		*(++p) = NULL;
	}
	if (side EQ 'l' OR side EQ 'b') {
		p = buf;
		while(isspace(*p)) p++;
		if (p NE buf) while(*(buf++)=*(p++));
	}
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
copy(to, from, count)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* COPIES count CHARACTERS FROM from TO to. NULLS HAVE NO EFFECT
AND MAY BE COPIED. */

	char *to, *from;
	{
	int i;
	for (i=0; i<= count-1; i++) to[i] = from[i];
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
delchars(start,stop,todelete)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* STARTING AT start, DELETES ALL CHARACTERS INCLUDED IN THE STRING todelete
UP TO stop, OR IF stop IS 0, UP TO FIRST NULL. */

	char *start, *stop, *todelete;
	{
	char *p;
	int delete, del_count;
	del_count = 0;
	if (stop EQ 0) stop = start + (strlen(start)) -1;
	while (start <= stop) {
		delete = NO;
		for (p = todelete; *p; p++) {
			if (*p EQ *start) delete = YES;
			break;
		}
		if (delete) {
			del_count++;
			for (p = start; *p; p++)
				*p = *(p+1);
			stop -= 1;
			if (stop <start) return(del_count);
		}
		start++;
	}
	return(del_count);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
dispchar(c)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* DISPLAYS ANY ASCII VALUE AS A PRINTABLE CHARACTER.

	FOR EXAMPLE, CONSIDER THAT WE WISH TO PRINT AN ASCII CHARACTER
		WHICH HAS A DECIMAL VALUE OF d.

	ORDINARY PRINTABLE CHARACTERS ARE IN THE RANGE d=32-126 AND ARE
		SIMPLY PRINTED.

	CONTROL CHARACTERS ARE IN THE RANGE d=0-31. THEY ARE PRINTED AS
		'^' PLUS THE PRINTABLE CHARACTER d+64.

	DELETE/RUB OUT (d=127) IS PRINTED AS '^?', THAT IS ^(d-64).

	META CHARACTERS (THOSE WITH THE HIGH BIT SET, DECIMAL 128-255)
		ARE PRINTED AS TILDE '~' PLUS d-128.
*/
	int c;
	{
	char out[4];
	dispexp(c, out);
	puts(out);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
dispexp(c, out)

/* Copyright (c) 1983 by Eric Martz, Amherst MA */

/* USED BY DISPCHAR */

	int c;
	char *out;
	{
	char cs[2];
	out[0] = NULL;
	if (c > 126) {
		if (c != 127) strcat(out, "~");
		c -= 128;
		if (c == -1 || c == 127) {
			strcat(out, "^?");
			return(0);
		}
	}
	if (c <= 31) {
		strcat(out, "^");
		c += 64;
	}
	cs[0] = c;
	cs[1] = NULL;
	strcat(out, cs);
}
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*	END OF MARTZ-AD.C	*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
NTED.

	CONTROL CHARACTERS ARE IN THE RANGE d=0-31. THEY ARE PRINTED AS