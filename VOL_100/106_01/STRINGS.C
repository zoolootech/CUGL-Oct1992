/*	strings.c

	Version 2.1	15-Apr-80

  	this set of string functions manipulates character
	arrays in a variety of useful ways. note that positions
	are offset 1 from the internal workings of 'C'. Thus
	the first position in a string is 1 which corresponds
	to array position [0].

	NOTE: none of these routines alter the original string. 
	      therefore, to assign the altered version of the
	      string to the original string, use:

		   strcpy(str,funct(s,..));
	           (or c = match(str,"xxx",x); in the case of match)

	      since the result is stored in a work buffer (strbuf),
	      it should be transferred immediately to avoid being
	      clobbered. therefore, complex statements involving
	      more than one of these functions are illegal and must
	      be broken into steps.

	IMPORTANT: in order to communicate with the external
		   work buffer, all separately compiled files
		   should contain the following declarations
		   as the first order of business.

		   char strbuf[256];
		   #define SBP	&strbuf[0]

		   failure to do this will result in garbage
		   results from independent functions which
		   must refer to this common memory block.
		   NOTE: files created prior to 04-Apr-80
		   use 'work' instead of 'strbuf' - change to
		   'strbuf' before recompiling (and WP to SBP).

		Function Summary
		----------------

1. delete(string,n,fpos)

	delete n characters from a copy of the given string
	beginning at the first position (fpos). return the
	altered string.

2. insert(string,istring,fpos)

	insert the insertion string (istring) into a copy of
	the given string beginning at the first position (fpos).
	return the altered string.

3. left(string,n)

	return the leftmost n characters from a copy of the
	given string.

4. mask(string,mstring)

	return a subset of a copy of the given string according
	to the following rules:

	     i) if the mask character is '?' the character is
	        returned.
	    ii) if the mask character is 'x' the character is
	        deleted.
	   iii) if the mask character is ' ' the character is
	        replaced by a space.

	example:

	#define TESTMASK "???  ?xx?xxx";
	mask("ABCDEFGHIJKL",TESTMASK) = "ABC  FI"

5. match(string,cstring,fpos)

	return the first position of a match of the comparison
	string (cstring) within the string beginning at the first
	position (fpos). return NONE (-1) if there is no match.

6. mid(string,fpos,n)

	return a subset of n characters beginning at the first
	position (fpos) of a copy of the given string.

7. replace(string,rstring,fpos)

	replace (an equal number of) characters in a copy of the
	given string with the replacement string (rstring) beginning
	at the first position (fpos). return the altered string.

8. reverse(string)

	return a copy of the given string with the characters reversed.

9. right(string,n)

	return the rightmost n characters of a copy of the
	given string.
*/

char strbuf[256];

#define SBP	&strbuf[0]	/* fwa of work buffer */
#define EOS	'\0'		/* string terminator  */
#define NONE	-1		/* negative indicator */

delete(s,n,fpos)
int n, fpos;
char *s;
{
	char *tp; tp = SBP;

	while (--fpos != 0) *tp++ = *s++; s += n;
	while (*tp++ = *s++); return SBP;
}

insert(s,is,fpos)
int fpos;
char *s, *is;
{
	char *tp; tp = SBP;

	while (--fpos != 0) *tp++ = *s++;
	while (*tp++ = *is++); --tp;
	while (*tp++ = *s++); return SBP;
}
	
left(s,n)
int n;
char *s;
{
	char *tp; tp = SBP;

	while (n-- != 0) *tp++ = *s++;
	*tp = EOS; return SBP;
}

mask(s,ms)
char *s, *ms;
{
	int n;
	char *tp; tp = SBP;

	n = strlen(s);
	while (n-- != 0) {
	switch(*ms) {
	   case '?': *tp = *s;  break;
	   case ' ': *tp = ' '; break;
	   case 'x': --tp;      break;
	}
	tp++; s++; ms++;
	}
	*tp = EOS; return SBP;
}
	  
match(s,cs,fpos)
char s[], cs[];
int fpos;
{
	int i, j, k;

	for (i=--fpos; s[i] != EOS; i++) {
	    for (j=i, k=0; cs[k] != EOS && s[j] == cs[k]; j++, k++)
	        ;
	    if (cs[k] == EOS) return(++i);
	}
	return(NONE);
}

mid(s,fpos,n)
int fpos, n;
char *s;
{
	char *tp; tp = SBP;

	s += --fpos;
	while (n-- != 0) *tp++ = *s++;
	*tp = EOS; return SBP;
}

replace(s,rs,fpos)
int fpos;
char *s, *rs;
{
	int n;
	char *tp; tp = SBP;

	n = strlen(rs);
	while (--fpos != 0) *tp++ = *s++; s += n;
	while (*tp++ = *rs++); --tp;
	while (*tp++ = *s++); return SBP;
}

reverse(s)
char *s;
{
	int n;
	char *tp; tp = SBP;

	n = strlen(s); s += n-1;
	while (n-- != 0) *tp++ = *s--;
	*tp = EOS; return SBP;
}

right(s,n)
int n;
char *s;
{
	char *tp; tp = SBP;

	s += strlen(s)-n;
	while (*tp++ = *s++); return SBP;
}
