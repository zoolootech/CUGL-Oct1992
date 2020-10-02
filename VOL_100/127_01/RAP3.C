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

/*-------------------------------------------------------------------------*/
getwrd(next)

/* STARTING AT THE POINTER *next, FINDS THE NEXT WORD AND SETS THE POINTERS
Wordbegin AND Wordend, MOVING *next TO THE CHARACTER AFTER Wordend. Wordlen
IS CALCULATED TAKING INTO ACCOUNT IMBEDDED POST_SS COMMANDS OR BACKSPACES.
RETURNS Wordlen WHICH IS ZERO IF LAST WORD WAS GOTTEN ON PREVIOUS CALL. IF
AN OPENING POST_SS_DELIMITER IS FOUND IN THE WORD, THE WORD MUST NOT END
UNTIL A CLOSING DELIMITER IS FOUND, TO PREVENT BREAKING A LINE WITHIN AN
ORIGINAL STRING TO BE POST-SUBSTITUTED. */

	char **next;
	{
	char *p1, *p2;
	int even_delim;
	if (**next EQ NULL) return(0);
	even_delim = YES;

	/* FIND WORD */
	if (Post_ss) {
		for(p1 = *next;
			((*p1 NE NULL) AND
			((even_delim EQ NO) OR
			(*p1 NE SPACE AND *p1 NE TAB AND *p1 NE DASH)))
			;p1++) {
				if(*p1 EQ Post_ss_delim) {
					if (even_delim) even_delim = NO;
					else even_delim = YES;
				}
		}
	}
	else {
		for(p1 = *next;
			((*p1 NE NULL) AND
			(*p1 NE SPACE AND *p1 NE TAB AND *p1 NE DASH))
			;p1++)
				;
	}
	

	if ((*p1 EQ NULL) AND (even_delim EQ NO)) {
		fprintf(STDERR,
			"Fatal error in line %d:\n%s%s",
			In_linecnt,
			"opening post-formatting string substitution delimiter\n",
			"without closing delimiter.\n");
		exit(0);
	}

	/* ALLOW BREAK AT DASH BUT INCLUDE DASH IN WORD */
	if (*p1 EQ DASH) p1++;

	/* SET EXTERNALS */
	Wordbegin = *next;
	Wordend = p1 - 1;
	Wordlen = p1 - *next;
	*next = p1;

	/* Wordlen CORRECTION ROUTINE */
	for (p1 = Wordbegin; p1 <= Wordend; p1++) {
		if (Post_ss AND *p1 EQ Post_ss_delim) {
			/* IF NO CHARACTERS BETWEEN COMMAND DELIMITERS, LEAVE FOR
			LATER SUBSTITUTION OF A SINGLE COMMAND DELIMITER TO BE
			PRINTED. */
			if (*p1+1 EQ Post_ss_delim) {
				Wordlen -= 1;
				p1++; /* IGNORE SECOND COMMAND DELIMITER IN THE PRESENT
					FOR LOOP */
				continue;
			}
			else {

				/* FIND CLOSING DELIMITER */
				for (p2 = p1+1; *p2 NE Post_ss_delim; p2++)
					;
				
				/* CORRECT Wordlen */
				Wordlen -= (p2 - p1 + 1);

				/* CONTINUE SCANNING AFTER THE TERMINAL DELIMITER */
				p1 = p2; /* for loop will now p1++ */
			}
		}
		else if (*p1 EQ BACKSPACE) Wordlen -= 2;
	}
#ifdef DEBUG
	if (Debug) {
		fprintf(STDERR,"Getwrd: \"");
		for (p1 = Wordbegin; p1 <= Wordend; p1++) putc(*p1,STDERR);
		fprintf(STDERR,"\" (Wordlen = %d)\n",Wordlen);
	}
#endif
	return(1);
}

/*-------------------------------------------------------------------------*/
getspaces(next)

/* DETERMINES THE VALUE OF Spacecnt, WHICH IS THE NUMBER OF SPACES WHICH
SHOULD SEPARATE THE PREVIOUS WORD FROM THE NEXT WORD. NOTE THAT EXPANSION OF
NON-LEADING TABS IS NOT APPLICABLE WHEN FILLING, i. e. IN THIS FUNCTION */

	char **next;
	{
	Spacecnt = 0;
	while (**next EQ SPACE OR
			**next EQ TAB OR
			**next EQ NULL) {
		Spacecnt++; /* formerly Spacecnt=1 */
		if (**next NE NULL) (*next)++;
		else break;
	}
/*
	/* PUT 2 SPACES AFTER PERIOD ENDING SENTENCE IF
		1. NEXT WORD IS CAPITALIZED
		2. PRECEDING WORD IS LONGER THAN 2 LETTERS (FOR INITIALS, Wm., Dr.)
	*/
	if (*Wordend EQ '.' AND (Wordlen > 3)
	AND Spacecnt AND (isupper(**next)))
		Spacecnt = 2;
*/
	/* IF THE PREVIOUS WORD ENDS IN A DASH AT THE END OF THE LINE, THEN WE
	ARE IN THE MIDDLE OF A HYPHENATED WORD AND NO SPACES SHOULD FOLLOW THE
	DASH. */

	if (*Wordend EQ DASH
		AND ((*next - Wordend) EQ 1)
		AND (**next EQ NULL)) Spacecnt = 0;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"Getspaces: Spacecnt = %d\n",Spacecnt);
#endif
}

/*-------------------------------------------------------------------------*/
putwrd() {
	int newlen, line_len, indelim;
	char *p1;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"putwrd(entry)");
#endif
	line_len = Rmval - Tival;
	newlen = Outlen + Wordlen + Spacecnt;
	if (newlen > line_len) {
		if (!Sanders AND Justify) {
			spread (Outbuf, line_len - Outlen + 1, Outwrds);
		}
		if (Sanders AND Justify) brk(FORCEPRINT);
		else brk(CR);
	}
	if (Wordlen) Outwrds++;
	for (p1 = Wordbegin; p1 <= Wordend; p1++) {
#ifdef DEBUG
		if (Debug) putc(*p1,STDERR);
#endif
		*(Nextout++) = *p1;
	}
	if (Spacecnt < 0 OR Wordlen < 0) {
		fprintf(STDERR,
			"Wordlen=%d, Spacecnt=%d, EXIT IN getwrd()\n",
			Wordlen,Spacecnt);
		exit(0);
	}
	fillbuf(&Nextout, SPACE, Spacecnt);
	Outlen += Wordlen + Spacecnt;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,
		"(return)Outwrds=%d  Outlen=%d\n",Outwrds,Outlen);
#endif
}
/*-------------------------------------------------------------------------*/
fillbuf(next, c, cnt)
	char c, **next;
	int cnt;
	{
	while (cnt--) {
		**next = c;
		*next += 1;
	}
}
/*-------------------------------------------------------------------------*/
fillulbuf(textline)
	char *textline;
	{
	int indelim;
	char *p, *nextul, *pflip;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"Fillulbuf\n");
#endif
	pflip = getflip('U', Ul_plus, textline);
	if (!pflip) {
		*Ulbuf = NULL;
		return(0);
	}
	nextul = Ulbuf;
	indelim = NO;
	for(p = textline; *p; p++) {
		/* SET Ul_plus */
		if (p EQ pflip) {
			Ul_plus? Ul_plus=NO: Ul_plus=YES;
			pflip = getflip('U', Ul_plus, pflip+1);
		}
		/* SKIP POST SUBSTITUTIONS */
		if (Post_ss AND *p EQ Post_ss_delim) {
			if (indelim) {
				indelim = NO;
				continue;
			}
			else indelim = YES;
		}
		if (indelim) continue;
		
		if (*p EQ BACKSPACE) {
			nextul--;
			continue;
		}
		if (Ul_plus AND (isalpha(*p) OR isdigit(*p)))
			*nextul++ = '_';
		else *nextul++ = SPACE;
	}
	*nextul = NULL;
}
/*-------------------------------------------------------------------------*/
boldonly(textline)
	char *textline;
	{
	int indelim;
	char *p, *nextul, *pflip;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"BOLDONLY\n");
#endif
	pflip = getflip('B', Bo_plus, textline);
	if (!pflip) return(NO);
	indelim = NO;
	for(p = textline; *p; p++) {
		/* SET Bo_plus */
		if (p EQ pflip) {
			Bo_plus? Bo_plus=NO: Bo_plus=YES;
			pflip = getflip('B', Bo_plus, pflip+1);
		}
		/* SKIP POST SUBSTITUTIONS */
		if (Post_ss AND *p EQ Post_ss_delim) {
			if (indelim) {
				indelim = NO;
				continue;
			}
			else indelim = YES;
		}
		if (indelim OR
			(*p EQ BACKSPACE) OR
			Bo_plus
			) continue;
		else *p = SPACE;
	}
	return(YES);
}
/*-------------------------------------------------------------------------*/
getflip(type, status, previous)
	char type, *previous;
	int status;
	{
	char flip[16];
	char b1[5], b2[5], u1[5], u2[5];
	makebu(b1, b2, u1, u2);
	if (type EQ 'U') { /* UNDERLINE */
		if (status) strcpy(flip, u2);
		else strcpy(flip, u1);
	}
	else { /* BOLDFACE */
		if (status) strcpy(flip, b2);
		else strcpy(flip, b1);
	}
	return(instrp(previous, flip));
}
/*-------------------------------------------------------------------------*/
instrp(start, tofind)

/* IN-STRING-POINTER: LOOKS FOR tofind BEGINNING AT start.
	RETURNS POINTER TO BEGINNING OF FIRST INSTANCE OF tofind,
	OR 0 IF NOT FOUND. */

	char *start, *tofind;
	{
	char *ptr;
	int len;
	len = strlen(tofind);
	for (ptr=start; *ptr; ptr++)
		if (lefteq(ptr, tofind) EQ len) return(ptr);
	return (0);
}
/*-------------------------------------------------------------------------*/
putline(line, eol)
	char *line, eol;
	{
	char delim_str[2];
	if (Print_at) goto done;
	delim_str[0] = Post_ss_delim;
	delim_str[1] = NULL;
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"Putline\n");
#endif
	/* SAVE LINE BEFORE POST SS */
	strcpy(Ulbuf, line);

	/* POST SS AND PRINT */
	if (Post_ss) ss(line, delim_str);
	screen_status();
	if (Ce_input AND *Center_mode)
		puts(Center_mode);
	else indent (Tival);
	puts(line);

	/* BOLDFACE */
	if (!(*Bo_on)) {
		strcpy(line, Ulbuf);
		if (boldonly(line)) {
			if (Post_ss) ss(line, delim_str);
			putchar(CR);
			puts(Bo_shift);
			indent(Tival);
			puts(line);
			puts(Bo_restore);
		}
	}
	
	/* UNDERLINE */
	if (!(*Ul_on)) {
		strcpy(line, Ulbuf);
		fillulbuf(line);
		if (*Ulbuf) {
			putchar(CR);
			puts(Ul_shift);
			indent(Tival);
			puts(Ulbuf);
			puts(Ul_restore);
		}
	}
	if (eol EQ CR OR !Forceprint) putchar(NEWLINE);
	/* i.e. IF EOL NE FORCEPRINT */
	/* N. B. putchar EXPANDS NEWLINE TO CR-NEWLINE! */
	else puts(Forceprint);
	if (Ce_input AND *Center_mode) restore_mode();
done:
	Tival = Inval;
}
/*-------------------------------------------------------------------------*/

brk(eol)
	char eol;
	{
#ifdef DEBUG
	if (Debug) fprintf(STDERR,"Brk\n");
#endif
	*Nextout = NULL;
	/* TRIM OFF TRAILING BLANKS */
	while (*(Nextout - 1) EQ SPACE AND Nextout > Outbuf) {
		Nextout--;
		*Nextout = NULL;
	}
	if (Outbuf[0]) put (Outbuf,eol);
	Outwrds = 0;
	Outlen = 0;
	Nextout = Outbuf;
}
/*------------------------------------------------------------------------*/

anotherline(bf)

/* GETS ONE LINE FROM INPUT FILE AND PERFORMS PRE_SS. IF PRE_SS RESULTS IN
BREAKING THE INPUT LINE INTO MULTIPLE LINES, THE EXTRA LINES ARE STORED IN
Extralines AND HANDED OUT ONE AT A TIME. */

	char *bf;
	{
	char delim_str[2], discard[BIGBUF];
	char b1[5], b2[5], u1[5], u2[5];
	int i;

	delim_str[0] = Pre_ss_delim;
	delim_str[1] = NULL;
	if (Extrafull) {
		strcpy(bf, Extralines);
		saveextra(bf);
	}
	else {
again:
		if ((rapgets(bf, Fpin)) EQ 0) return(0);
		/* RAPGETS IS FNNGETS WITH SETTING OF Press_flg + FOR \\(EVEN),
		- FOR \(ODD) AND ZERO FOR NONE */
/*		
		if (New_portion) {
			if ((i=instr(0,bf,PORTIT_O)) NE ERROR) {
				strcpy(discard,bf);
				while((instr(0,discard,PORTIT_C)) EQ ERROR)
					if (!rapgets(discard, Fpin)) return(0);
				bf[i] = NULL;
				Fpin = Fp_main;
				i = In_linecnt;
				In_linecnt = In_main;
				In_main = i;
				Old_portion = New_portion;
				New_portion = 0;
				/* if blank line, dont feed thru since will break fill */
				if (!(*bf)) goto again;
			}
		}
*/
#ifdef DEBUG
		if (Debug) fprintf(STDERR,
			"\nAnotherline reads input line %d\n",In_linecnt);
#endif
		/* IF AN INCOMPLETE PRE_SS IS FOUND, CONCATENATE ANOTHER LINE */
		if (Pre_ss AND Press_flg < 0) {
			if ((rapgets(Extralines, Fpin)) EQ 0) {
				fprintf(STDERR,
				"Odd number of pre-format substitution delimiters\n%s",
				"in last line of input file.\n");
				return(1);
			}
			if (Press_flg >= 0) {
				fprintf(STDERR,
				"Incomplete pre-format substitution string in line %d\n",
					In_linecnt-1);
				fprintf(STDERR,
					"not completed in line %d.\n",
					In_linecnt);
				exit(0);
			}
			strcat(bf," ");
			strcat(bf, Extralines);
			Press_flg = 1;
		}
		if (Pre_ss AND
			(Press_flg OR Press_all) AND
			(lefteq(bf, ".he") NE 3) AND
			(lefteq(bf, ".fo") NE 3)
			) {
			ss(bf, delim_str);
			saveextra(bf);
		}
		if (Exp_ctrl_chars) expand_controls(bf);

		/* COMMAND TEXT LINES (TAB CONT OR INDEX) MUST USE }b+}/}u+} */
		if (*Inbuf EQ COMMAND) return(1);

		/* UNDERLINING */
		makebu(b1, b2, u1, u2);
		if (Ul_input) {
			for(i=0; isspace(bf[i]); i++)
				discard[i] = bf[i];
			discard[i] = NULL;
			strcat(discard, u1);
			strcat(discard,bf+i);
			strcpy(bf,discard);
			strcat(bf, u2);
		}
		/* BOLDFACE */
		if (Bo_input) {
			for(i=0; isspace(bf[i]); i++)
				discard[i] = bf[i];
			discard[i] = NULL;
			strcat(discard, b1);
			strcat(discard,bf+i);
			strcpy(bf,discard);
			strcat(bf, b2);
		}
		return(1);
	}
}
/*------------------------------------------------------------------------*/
makebu(b1, b2, u1, u2)
	char *b1, *b2, *u1, *u2;
	{
	b1[0] = b2[0] = u1[0] = u2[0] = Post_ss_delim;
	b1[3] = b2[3] = u1[3] = u2[3] = Post_ss_delim;
	b1[4] = b2[4] = u1[4] = u2[4] = NULL;
	b1[1] = b2[1] = 'b';
	u1[1] = u2[1] = 'u';
	b1[2] = u1[2] = '+';
	b2[2] = u2[2] = '-';
}
/*------------------------------------------------------------------------*/
saveextra(next)
	char *next;
	{
	Extrafull = NO;
	for (; *next; next++) {
		if ((*next EQ CR) AND (*(next+1) EQ NEWLINE)) {
			*next = NULL;
			if (*(next+2)) {
				strcpy (Extralines, next+2);
				Extrafull = YES;
			}
			break;
		}
	}
}
/*------------------------------------------------------------------------*/
expand_controls(buf)
	char *buf;
	{
	char tmp[512], *p;
	int i;
	i = 0;
	p = buf;
	while(*p) {
		if (*p >= SPACE OR *p EQ TAB OR *p EQ BACKSPACE OR *p EQ FORMFEED) {
			if (*p NE '\177' /* DEL */) tmp[i++] = *(p++);
			else {
				tmp[i++] = '^';
				tmp[i++] = '?';
				p++;
			}
		}
		else {
			tmp[i++] = '^';
			tmp[i++] = (*p + '@');
			p++;
		}
	}
	tmp[i] = NULL;
	strcpy(buf, tmp);
}

/*------------------------------------------------------------------------*/
/* END OF RAP3.C */

	b1[1] = b2[1] = 'b';
	u1[1] = u