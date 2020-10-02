/*
 *
 *    G N U P L O T  --  util.c
 *
 *  Copyright (C) 1986 Thomas Williams, Colin Kelley
 *
 *  You may use this code as you wish if credit is given and this message
 *  is retained.
 *
 *  Please e-mail any useful additions to vu-vlsi!plot so they may be
 *  included in later releases.
 *
 *  This file should be edited with 4-column tabs!  (:set ts=4 sw=4 in vi)
 */

#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <errno.h>
#include "plot.h"

#ifndef vms
extern int errno, sys_nerr;
extern char *sys_errlist[];
#endif /* vms */

extern int inline;			/* line number of input */
extern char input_line[MAX_LINE_LEN];
extern struct lexical_unit token[MAX_TOKENS];
extern jmp_buf env;	/* from plot.c */

/*
 * equals() compares string value of token number t_num with str[], and
 *   returns TRUE if they are identical.
 */
equals(t_num, str)
int t_num;
char *str;
{
register int i;

	if (!token[t_num].is_token)
		return(FALSE);				/* must be a value--can't be equal */
	for (i = 0; i < token[t_num].length; i++) {
		if (input_line[token[t_num].start_index+i] != str[i])
			return(FALSE);
		}
	/* now return TRUE if at end of str[], FALSE if not */
	return(str[i] == '\0');
}



/*
 * almost_equals() compares string value of token number t_num with str[], and
 *   returns TRUE if they are identical up to the first $ in str[].
 */
almost_equals(t_num, str)
int t_num;
char *str;
{
register int i;
register int after = 0;
register start = token[t_num].start_index;
register length = token[t_num].length;

	if (!token[t_num].is_token)
		return(FALSE);				/* must be a value--can't be equal */
	for (i = 0; i < length + after; i++) {
		if (str[i] != input_line[start + i]) {
			if (str[i] != '$')
				return(FALSE);
			else {
				after = 1;
				start--;	/* back up token ptr */
				}
			}
		}

	/* i now beyond end of token string */

	return(after || str[i] == '$' || str[i] == '\0');
}



isstring(t_num)
int t_num;
{
	
	return(token[t_num].is_token &&
		   (input_line[token[t_num].start_index] == '\'' ||
		   input_line[token[t_num].start_index] == '\"'));
}


isnumber(t_num)
int t_num;
{
	return(!token[t_num].is_token);
}


isletter(t_num)
int t_num;
{
	return(token[t_num].is_token &&
			(isalpha(input_line[token[t_num].start_index])));
}


/*
 * is_definition() returns TRUE if the next tokens are of the form
 *   identifier =
 *		-or-
 *   identifier ( identifer ) =
 */
is_definition(t_num)
int t_num;
{
	return (isletter(t_num) &&
			(equals(t_num+1,"=") ||			/* variable */
			(equals(t_num+1,"(") &&		/* function */
			 isletter(t_num+2)   &&
			 equals(t_num+3,")") &&
			 equals(t_num+4,"=") )
		));
}



/*
 * copy_str() copies the string in token number t_num into str, appending
 *   a null.  No more than MAX_ID_LEN chars are copied.
 */
copy_str(str, t_num)
char str[];
int t_num;
{
register int i = 0;
register int start = token[t_num].start_index;
register int count;

	if ((count = token[t_num].length) > MAX_ID_LEN)
		count = MAX_ID_LEN;
	do {
		str[i++] = input_line[start++];
		} while (i != count);
	str[i] = '\0';
}


/*
 * quote_str() does the same thing as copy_str, except it ignores the
 *   quotes at both ends.  This seems redundant, but is done for 
 *   efficency.
 */
quote_str(str, t_num)
char str[];
int t_num;
{
register int i = 0;
register int start = token[t_num].start_index + 1;
register int count;

	if ((count = token[t_num].length - 2) > MAX_ID_LEN)
		count = MAX_ID_LEN;
    while (i != count) {
		str[i++] = input_line[start++];
	 }
	str[i] = '\0';
}

/*
 *	capture() returns in str[] the the part of input_line[] which lies
 *	between the begining of token[start] and end of token[end]
 */
capture(str,start,end)
char str[];
int start,end;
{
register int i,j;
char *s = str;

	j = token[end].start_index + token[end].length;
	for (i = token[start].start_index; i < j && input_line[i] != '\0'; i++)
		*s++ = input_line[i];
	*s = '\0';
}


convert(val_ptr, t_num)
struct value *val_ptr;
int t_num;
{
	*val_ptr = token[t_num].l_val;
}



show_value(fp,val)
FILE *fp;
struct value *val;
{
		switch(val->type) {
			case INT:
				fprintf(fp,"%d",val->v.int_val);
				break;
			case CMPLX:
				if (val->v.cmplx_val.imag != 0.0 )
					fprintf(fp,"{%g, %g}",
						val->v.cmplx_val.real,val->v.cmplx_val.imag);
				else
					fprintf(fp,"%g", val->v.cmplx_val.real);
				break;
			default:
				int_error("unknown type in show_value()",NO_CARET);
		}
}


double
real(val)		/* returns the real part of val */
struct value *val;
{
	switch(val->type) {
		case INT:
			return((double) val->v.int_val);
			break;
		case CMPLX:
			return(val->v.cmplx_val.real);
	}
	int_error("unknown type in real()",NO_CARET);
	/* NOTREACHED */
}


double
imag(val)		/* returns the imag part of val */
struct value *val;
{
	switch(val->type) {
		case INT:
			return(0.0);
			break;
		case CMPLX:
			return(val->v.cmplx_val.imag);
	}
	int_error("unknown type in real()",NO_CARET);
	/* NOTREACHED */
}



double
magnitude(val)		/* returns the magnitude of val */
struct value *val;
{
	double sqrt();

	switch(val->type) {
		case INT:
			return((double) abs(val->v.int_val));
			break;
		case CMPLX:
			return(sqrt(val->v.cmplx_val.real*
				    val->v.cmplx_val.real +
				    val->v.cmplx_val.imag*
				    val->v.cmplx_val.imag));
	}
	int_error("unknown type in magnitude()",NO_CARET);
	/* NOTREACHED */
}



double
angle(val)		/* returns the angle of val */
struct value *val;
{
	double atan2();

	switch(val->type) {
		case INT:
			return((val->v.int_val > 0) ? 0.0 : Pi);
			break;
		case CMPLX:
			if (val->v.cmplx_val.imag == 0.0) {
				if (val->v.cmplx_val.real >= 0.0)
					return(0.0);
				else
					return(Pi);
			}
			return(atan2(val->v.cmplx_val.imag,
				     val->v.cmplx_val.real));
	}
	int_error("unknown type in angle()",NO_CARET);
	/* NOTREACHED */
}


struct value *
complex(a,realpart,imagpart)
struct value *a;
double realpart, imagpart;
{
	a->type = CMPLX;
	a->v.cmplx_val.real = realpart;
	a->v.cmplx_val.imag = imagpart;
	return(a);
}


struct value *
integer(a,i)
struct value *a;
int i;
{
	a->type = INT;
	a->v.int_val = i;
	return(a);
}

/* Lower-case the given string (DFK) */
/* Done in place. */
void
lower_case(s)
     char *s;
{
  register char *p = s;

  while (*p != '\0') {
    if (*p >= 'A' && *p <= 'Z')
      *p = *p - 'A' + 'a';
    p++;
  }
}


int space_count(line, t_num)
	int line;
	int t_num;
{
    char string[20];

    sprintf(string, "---%d: ", line);
    return(strlen(string) + token[t_num].start_index);
}

show_line(always)
	BOOLEAN always;
{
    static int last_line_printed = 0;	/* number of line printed out last. */

    if (always || inline != last_line_printed) {
	   /* print line if not printed yet */
	   fprintf(stderr,"\n---%d: %s\n", inline, input_line);
	   last_line_printed = inline;
    }
}

os_error(str,t_num)
char str[];
int t_num;
{
#ifdef vms
static status[2] = {1, 0};		/* 1 is count of error msgs */
#endif

register int i;
int count;

    show_line(TRUE);

	if (t_num != NO_CARET) {		/* put caret under error */
	    count = space_count(inline, t_num);
		for (i = 0; i < count; i++) {
			(void) putc((input_line[i] == '\t') ? '\t' : ' ',stderr);
			}
		(void) putc('^',stderr);
		(void) putc('\n',stderr);
	}

	fprintf(stderr,"###%d: %s\n",str);

#ifdef vms
	status[1] = vaxc$errno;
	sys$putmsg(status);
	(void) putc('\n',stderr);
#else
	if (errno >= sys_nerr)
		fprintf(stderr, "unknown errno %d\n\n", errno);
	else
		fprintf(stderr,"(%s)\n",sys_errlist[errno]);
#endif

	longjmp(env, TRUE);	/* bail out to command line */
}

/* fatal error for this line; no return */
int_error(str,t_num)
char str[];
int t_num;
{
    register int i;
    int count;

    show_line(TRUE);

    if (t_num != NO_CARET) {	/* put caret under error */
	   count = space_count(inline, t_num);
	   for (i = 0; i < count; i++) {
		  (void) putc((input_line[i] == '\t') ? '\t' : ' ',stderr);
	   }
	   (void) putc('^',stderr);
	   (void) putc('\n',stderr);
    }

    fprintf(stderr,"###%d: %s\n", inline, str);

    longjmp(env, TRUE);		/* bail out to command line */
}

/* print an error for this line and return */
err_msg(str)
	char str[];
{
    show_line(FALSE);
    fprintf(stderr,"***%d: %s\n", inline, str);
}


/* find char c in string str; return p such that str[p]==c;
 * if c not in str then p=strlen(str)
 */
int
instring(str, c)
	char *str;
	char c;
{
    int pos = 0;

    while (str != NULL && *str != '\0' && c != *str) {
	   str++; 
	   pos++;
    }
    return (pos);
}

