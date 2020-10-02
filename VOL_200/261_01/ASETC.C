/* asetc.c - misc routines
 * parse statement for label and instruction, find operand field
 */

#include <stdio.h>
#include <ctype.h>
#include "as68.h"
 
/* state types */
#define	START 1
#define	LBL 2
#define	LIM 3
#define	STOP 4

/* character types */
#define	LIM_C 1
#define	LM_C 2
#define	LBLTRM_C 3
#define	WS_C 4
#define	STMEND_C 5
#define	CMNT_C 6
#define	ILGL_C 7

extern char *stmnt_ptr;		/* moved to root */
extern char statement[STMNT_SIZE];
extern char instr[32];
extern char label[32];
extern char *opfld_ptr;
extern char lcksm;
extern char pass;
extern int src_level;
extern int loc_plus;
extern int wrap;
extern unsigned line_count;
extern long loc_counter;
extern FLAG nolist;
extern FLAG lcon;
extern FLAG llst;
extern FLAG lfile;
extern FLAG obj_open;
extern FLAG trunc;
extern FILE *_src[SRCLEVELS];
extern FILE *lst_d;
extern FILE *lst_f;
extern FILE *obj_f;
 
/* types */

char *nextfield();

/* code begins */

preparse()
{
    char *sbuf_ptr = statement;		/* pointer to statement	buffer */
    char *field_ptr = NULL;		/* pointer to present field */
    int field_len = 0; 			/* length of present scanned field */
    register FLAG state = START; 	/* present state */
    FLAG bad_field = NO;
 
    label[0] = label[31] = instr[0] = instr[31]	= NULL;
    opfld_ptr = NULL;
    stmnt_ptr = statement;		/* init for get_source() */
 
/* LIM_C:	'A'-'Z','a'-'z','.'	*/
/* LM_C:	'0'-'9','_',?'-'?,'$'	*/
/* LBLTRM_C:	':'			*/
/* WS_C:	' ','\t'		*/
/* STMEND_C:	'\n'			*/
/* CMNT_C:	'*'			*/
/* ILGL_C:	<anything else> 	*/
 
/* START state */
 
    switch (nextc(&sbuf_ptr)) {
    case CMNT_C:
    case STMEND_C:
	opfld_ptr = statement;		/* operands or comment */
	return COMMENT;			/* nothing here... */
    case ILGL_C:			/* illegal character */
    case LM_C:  			/* LM_C's can't start label */
	label[31] = YES;		/* bad label char in label field */
    case LIM_C:
	field_ptr = sbuf_ptr - 1;	/* nextc already inc. sbuf_ptr */
	++field_len;			/* got 1 so far */
	state = LBL;			/* enter LBL state */
	break;
    case WS_C:
	field_ptr = nextfield(&sbuf_ptr);	/* skip the white space */
	if (*field_ptr == '*' || *field_ptr == '\n') {
	    opfld_ptr = statement;		/* operands or comment */
	    return COMMENT;
	}
	state = LIM;
	break;					/* try again */
    }
 
/* LBL state */
 
    while (state == LBL) {
	switch(nextc(&sbuf_ptr)) {
	case ILGL_C:
	    label[31] = YES;		    /* got a bad char */
	case LIM_C:
	case LM_C:
	    ++field_len;		    /* one more label char */
	    continue;			    /* still in LBL state */
	case STMEND_C:
	    state = STOP;
	case LBLTRM_C:				/* label ended... */
	case WS_C:
	    savefield(field_ptr, field_len, label); /* save the label */
	    if (state != STOP) {
		field_len = NULL;		    /* starting	fresh */
		field_ptr = nextfield(&sbuf_ptr);   /* get a new field */
/**f3*/		state = (*field_ptr == '*') ? STOP : LIM;
	    }
	}
    }
 
/* LIM state */
 
    while (state == LIM) {
	switch(nextc(&sbuf_ptr)) {
	case LM_C:
	case LIM_C:
		++field_len;
		continue;				/* try another */
	case STMEND_C:
	case WS_C:
		state = STOP;
		savefield(field_ptr, field_len,	instr);	/* save legal instr */
		instr[31] = bad_field;
		break;
	case LBLTRM_C:
		savefield(field_ptr, field_len,	label);	/* save label */
		label[31] = bad_field;
		bad_field = field_len = NULL;		/* going for instr */
		field_ptr = nextfield(&sbuf_ptr);	/* look for it */
		continue;				/* still LIM state */
	case ILGL_C:
		bad_field = YES;			/* bad char */
		++field_len;
		continue;				/* try another */
	}
    }
 
 
/* STOP state */
 
    nextfield(&sbuf_ptr);			    /* get to next field */
    opfld_ptr =  sbuf_ptr;			    /* operands	or comment */
    return OK;
}
 
/* return type of char pointed to, bump pointer	*/
 
nextc(ptr)
register char **ptr;
{
    register char c;
 
    c = *(*ptr)++;  /* get the buf ptr, post inc., get char it points to */
    if (isalpha(c) || (c == '.'))
	return LIM_C;
    if (isdigit(c) || (c == '_') || (c == '$'))
	return LM_C;		    /** || (c == '-') ??? **/
    switch (c) {
    case '\n':  return STMEND_C;
    case ' ':
    case '\t':  return WS_C;
    case ':':   return LBLTRM_C;
    case '*':   return CMNT_C;
    default:	return ILGL_C;
    }
}
 
/* align pointer on next char other than ' ' or '\t' */
 
char *nextfield(ptr)
register char **ptr;
{
    if (!**ptr)	return --(*ptr);
    while				/* skips too much...? */
    /*if*/ ((**ptr == ' ') || (**ptr == '\t'))
	++(*ptr);			/* bump buf ptr ???*/
    return *ptr;			/* value of ptr ???*/
}
 
/***
    get a line of text into statement.	Allows nesting of source files and
    input from stdin.
***/
 
char *
nextline()
{						/* get a line into buffer */
    while (!fgets(statement, STMNT_SIZE, _src[src_level])) {
	if (src_level) {		    /* in an included file */
	    fclose(_src[src_level--]);	    /* close file, pop stack */
	    continue;			    /* try level above */
	}   
	if (++pass > 2) return NULL;	    /* end second pass main file */
	loc_counter = loc_plus = 0;         /* set counter to default */
	line_count = 0; 		    /* rewind for second pass */
	for ( ; src_level; fclose(_src[src_level--]));/* close include files */
	fseek(_src[0], 0L, 0);  	    /* rewind the source file */
	/* err_out(FLUSH); */		    /* get rid of pending errors */
	continue;			    /* start second pass */
    }
    ++line_count;			    /* count the line */
    return statement;			    /* anything	but NULL would work */
}
 
/* save the field pointed to by ptr, max field length is 30 + NULL */
 
savefield(ptr, len, dest)
register char *ptr;
int len;
register char *dest;
{
    register int x;
    for (x = 30; x && len; --x, --len)		/* dec. x and len each time */
	*dest++	= *ptr++;
    *dest = '\0';		    /* terminate string */
}
 
/*** aztec can't...
#undef START
#undef LBL
#undef LIM
#undef STOP
 
#undef LIM_C
#undef LM_C
#undef LBLTRM_C
#undef WS_C
#undef STMEND_C
#undef CMNT_C
#undef ILGL_C
***/							/* end of preparse() */
 
/***
    send data for present line here, dump when appropriate
 
line format:
 
	 1         2         3         4	 5         6         7
1234567890123456789012345678901234567890123456789012345678901234567890123456789
|  | |    | |  | |       | |       |     |     | |             | |            |
line locval opcd 1st  word 2nd  word	 instrct operand field.. .comment field
 
1234 123456 1234 1234 1234 1234 1234	123456789223456789323456789423456789...
	   [---- ---- ---- ---- ----]	[-----------------------------------..]
 
***/
 
/*
    raw code is sent here as it is produced by the system. this
    function will send it to appropriate places, in the correct	form.
*/
/* send buffer to all open list channels */
 
list_dump(buf)
char *buf;
{
    if (nolist)	return;
    if (lcon) puts(buf);
    if (llst) fputs(buf, lst_d);
    if (lfile) fputs(buf, lst_f);
}
 
obj_out(action,	cdp, size)
int action;
char *cdp;
int size;
{
    register int y;
    extern long s_lc;
    extern char s_buf[48];	/* init first char, room to spare */
    extern int s_cnt;		/* bytes in current buffer */
    extern int s_x;		/* index into s_buf */
 
    if (!obj_open) return NULL; 		/* put into obj file */
    switch (action) {
    case CODE:
    case DATA:
	s_lc = 0L;
    case MDATA:
top:
	for (y = 0; size && (s_cnt < 16); --size,	++s_cnt) {
/**f4*/	    lcksm += cdp[y++];				/* line checksum */
	}
	hex_byt(s_buf + s_x, cdp, y);			/* get them in buf */
	s_x += y * 2;     				/* bump x */
	s_lc += y;
	if (s_cnt == 16) {				/* time to flush */
	    flush_rcrd(s_buf, s_cnt, s_x);
	    new_rcrd(s_buf, s_lc, &s_x);
	    s_cnt = 0;
	    if (size) { 				/* more to buffer */
		cdp += y;				/* skip already	done */
		goto top;
	    }
	}
	break;
    case SYNC:
/**f4*/	flush_rcrd(s_buf, s_cnt, s_x);		/* flush s_buf */
	new_rcrd(s_buf,	0L, &s_x);
	s_cnt = 0;
	break;
    case OPEN:
	fputs("S0030000FC\n", obj_f);			/* header record */
/**f4*/	new_rcrd(s_buf,	0L, &s_x);
	s_cnt = 0;
	break;
    case CLOSE:
	flush_rcrd(s_buf, s_cnt, s_x);		/* flush s_buf */
/**f4*/	fputs("S9020000\n", obj_f);
/**	fprintf(obj_f, "S9030000%02x\n", fcksm);*/
	break;
    case MSG:   break;
    default:	return ERROR;
    }
    return NULL;
}
 
new_rcrd(s_buf,	lc, x)
char *s_buf;
long lc;
int *x;
{
/**f4*/
    long temp;
 
    temp = loc_counter + lc;
    if (temp & ~0xffffL) {
	sprintf(s_buf + 1, "200%06lx", temp);
/**f4?*/
	lcksm = (temp >> 16) + (temp >> 8) + temp;
	*x = 10;
    }
    else {
	sprintf(s_buf + 1, "100%04lx", temp);
	lcksm = (temp >> 8) + temp;
	*x = 8;
    }
}
 
flush_rcrd(s_buf, cnt, x)
char *s_buf;
char cnt;
char x;
{
    register char *c;
 
    if (!cnt) return NULL;			/* empty record */
/** fixed 12/20/83 tnx m hitch */
    cnt += 1 + ((s_buf[1] == '2') ? 3 : 2);	/* add byte count */
    hex_byt(s_buf + 2, &cnt, 1);		/* place in s_buf */
    lcksm = 0xff - (lcksm + cnt);		/* add checksum	*/
    hex_byt(&s_buf[x], &lcksm, 1);		/* place it in s_buf */
    strcpy(&s_buf[x+2],	"\n");			/* terminate record */
/** lc fix */
    for (c = s_buf; *c; ++c)	/* some printf()'s make a-f lowercase  */
	if (*c >= 'a' && *c <= 'f') *c -= ('a' - 'A');  		
    fputs(s_buf, obj_f);
    return TRUE;
}
 
/**
    given a pointer to an array of bytes, convert each to a 2 byte
    ascii rep, and place into char array pointed at by ap, for len
    bytes
**/
 
hex_byt(ap, bp, len)
register unsigned char *ap;
register unsigned char *bp;
register int len;
{
    for ( ; len; --len, ++bp, ap += 2) {
	ap[0] = (*bp >> 4) & 0x0f;
	ap[0] += (ap[0] > 9) ? 55 : 48; /* ':' + ('A' - ':') : '0' */
	ap[1] = *bp & 0x0f;
	ap[1] += (ap[1] > 9) ? 55 : 48;
    }
}
 
/***
    gets up to wrap - 40 chars from statement, places into buf.
***/
 
get_source(buf)
register char *buf;
{
    register int size, cnt = 1;
    int x;
 
    if ((stmnt_ptr != statement) && trunc) return (int)(buf[0] = '\0');
    for (size = wrap - (40 + 1); --size; ++cnt) {
	switch (*stmnt_ptr) {
	case NULL:  return (int)(buf[0]	= '\0');
	case '\n':
	    break;
	case '\t':
	    size -= (x = 7 - (cnt % 8));    /* 7 'cause loop subs 1 */
	    if (size < 1) {     	    /* tab would overflow.. */
		size = 1;		    /* ..so cause exit without */
		continue;		    /* placing tab into buffer */
	    }
	    cnt += x;   		    /* fall thru to default */
	default:
	    *buf++ = *stmnt_ptr++;
	    continue;
	}
    }
    *buf++ = '\n';
    *buf = '\0';
    if (*stmnt_ptr == '\n') ++stmnt_ptr;    /* make NULL for next call */
    return TRUE;
}
 
/* endcode */


