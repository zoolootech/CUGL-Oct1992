;				strfun.asm
;
;	Copyright (C) 1980, M J Maney
;
;	First created 8/25/80
;	Last revised 8/30/80 19:15
;
;	Herein are the basic string functions that C people like to use,
;	coded in assembler to make them as fast as can be.
;
;	NOTE that I have used .NE. in the comments in place of the usual C
;	symbol, because MAC sometimes objects to such usage.
;
	maclib	crl
;
;
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
;				string functions
;
;	strlen		strcmp		strcpy		strcat
;
; * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
;
;
;	strlen(str)
;	char *str;
;
;	Returns the length of the string pointed to by str. It is assumed
;	that the string is terminated by '\0', and the terminator is NOT
;	counted as a character, so the storage length of a string s is
;	strlen(s) + 1.
;
	PROC	STRLEN
	lxi	d,0
	lhld	ARG1		;str to HL
	mvi	a,0
strlen1	cmp	m
	BZ	strlen2		;for (i=0; *str++ .NE. EOS; i++) ;
	inx	h
	inx	d
	BRA	strlen1
;
strlen2	xchg			;return i;
	ret
	PEND	STRLEN
;
;
;	strcmp(sx,sy)
;	char *sx,*sy;
;
;	Returns 1, 0, or -1, depending upon the lexicographical relation
;	of the strings pointed to by sx and sy. The ordering is essentially
;	that which would be used if the strings were words being alphebetized
;	except that upper case and lower case are distinct, and digits and
;	punctuation are treated as significant characters. The relation
;	between any two characters is simply the relation between the bit
;	patterns that are used to represent them, treated as unsigned
;	integers. It is also assumed that the string terminator, '\0', has
;	a smaller value than any other character (in fact, its zero).
;
;	1 if sx > sy, 0 if sx == sy, or -1 if sx < sy
;
	PROC	STRCMP
	lhld	ARG1
	xchg			;sx to DE
	lhld	ARG2		;sy to HL
;
strcmp1	ldax	d
	cmp	m		;while (*sy == (c = *sx)) {
	BNZ	strcmp2
	inx	h		;	sy++;
	inx	d		;	sx++;
	ora	a
	BNZ	strcmp1		;	if (c == EOS)
	lxi	h,0		;		return 0;
	ret			;	}
strcmp2	lxi	h,1		;if (*sx > *sy)
	rnc			;	return 1;
	dcx	h		;else
	dcx	h
	ret			;	return -1;
	PEND	STRCMP
;
;
;	strcpy(des,src)
;	char *des,*src;
;
;	Copies string pointed to by src into memory starting at des. No
;	provision is made for limiting the amount of stuff copied: it is
;	the user's responsibility to insure that the string being copied
;	will fit where it is being copied to.
;
	PROC	STRCPY
	lhld	ARG1
	xchg			;des to DE
	lhld	ARG2		;src to HL
strcpy1	mov	a,m		;do {
	inx	h
	stax	d
	inx	d		;	*des++ = c = *src++;
	ora	a
	BNZ	strcpy1		;	}while (c .NE. EOS);
	ret			;return;
	PEND	STRCPY
;
;
;	strcat(des,src)
;	char *des,*src;
;
;	Copies the string pointed to by src onto the tail end of the string
;	pointed to by des. This is equivalent to
;
;		strcpy(des+strlen(des),src)
;
;	and there is an identical lack of testing for overrun during the
;	copying process.
;
	PROC	STRCAT
	lhld	ARG2
	xchg			;src to DE
	lhld	ARG1		;des to HL
	sub	a
strcat1	cmp	m		;while (*des++ .NE. EOS)
	inx	h		;	;
	BNZ	strcat1
	dcx	h		;des--;
strcat2	ldax	d		;do {
	inx	d
	mov	m,a
	inx	h		;	*des++ = c = *src++;
	ora	a
	BNZ	strcat2		;	}while (c .NE. EOS);
	ret			;return;
	PEND	STRCAT
;
;
	end
