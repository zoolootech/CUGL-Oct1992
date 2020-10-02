/* m16adr.c */

/*
 * (C) Copyright 1991
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asm.h"
#include "m6816.h"

int
addr(esp)
register struct expr *esp;
{
	register c;
	register struct area *espa;
	register char *tcp;

	if ((c = getnb()) == '#') {
		expr(esp, 0);
		esp->e_mode = T_IMM;
	} else
	if (c == ',') {
		esp->e_mode = 0;
		esp->e_flag = 0;
		esp->e_addr = 0;
		esp->e_mode = 0;
		esp->e_base.e_ap = NULL;
		c = admode(xyz);
		if (c && T_INDX) {
			esp->e_mode = c;
		} else {
			aerr();
		}
	} else {
		unget(c);
		if(admode(e) != 0) {
			comma();
			esp->e_mode = 0;
			esp->e_flag = 0;
			esp->e_addr = 0;
			esp->e_mode = 0;
			esp->e_base.e_ap = NULL;
			c = admode(xyz);
			if (c && T_INDX) {
				esp->e_mode = T_E_I | (c & 0x30);
			} else {
				aerr();
			}
		} else {
			expr(esp, 0);
			esp->e_mode = T_EXT;
			if (more()) {
				comma();
				tcp = ip;
				if ((c = admode(xyz)) != 0) {
					if (c && T_INDX) {
						esp->e_mode = c;
					} else {
						aerr();
					}
				} else {
					ip = --tcp;
				}
			}
		}
	}
	return (esp->e_mode);
}
	
/*
 * Enter admode() to search a specific addressing mode table
 * for a match. Return the addressing value on a match or
 * zero for no match.
 */
int
admode(sp)
register struct adsym *sp;
{
	register char *ptr;
	register int i;
	unget(getnb());
	i = 0;
	while ( *(ptr = (char *) &sp[i]) ) {
		if (srch(ptr)) {
			return(sp[i].a_val);
		}
		i++;
	}
	return(0);
}

/*
 *      srch --- does string match ?
 */
int
srch(str)
register char *str;
{
	register char *ptr;
	ptr = ip;

#if	CASE_SENSITIVE
	while (*ptr && *str) {
		if(*ptr != *str)
			break;
		ptr++;
		str++;
	}
	if (*ptr == *str) {
		ip = ptr;
		return(1);
	}
#else
	while (*ptr && *str) {
		if(ccase[*ptr] != ccase[*str])
			break;
		ptr++;
		str++;
	}
	if (ccase[*ptr] == ccase[*str]) {
		ip = ptr;
		return(1);
	}
#endif

	if (!*str)
		if (any(*ptr," \t\n,];")) {
			ip = ptr;
			return(1);
		}
	return(0);
}

/*
 *      any --- does str contain c?
 */
int
any(c,str)
char    c, *str;
{
	while (*str)
		if(*str++ == c)
			return(1);
	return(0);
}

struct adsym	xyz[] = {	/* all indexed modes */
	"x",	0x00 | T_INDX,
	"y",	0x10 | T_INDX,
	"z",	0x20 | T_INDX,
	"x8",	0x00 | T_INDX | T_IND8,
	"y8",	0x10 | T_INDX | T_IND8,
	"z8",	0x20 | T_INDX | T_IND8,
	"x16",	0x00 | T_INDX | T_IND16,
	"y16",	0x10 | T_INDX | T_IND16,
	"z16",	0x20 | T_INDX | T_IND16,
	"",	0x00
};

struct adsym	e[] = {		/* e register */
	"e",	0x01,
	"",	0x00
};

struct adsym	pshm[] = {	/* push on system stack */
	"d",	0x01,
	"e",	0x02,
	"x",	0x04,
	"y",	0x08,
	"z",	0x10,
	"k",	0x20,
	"ccr",	0x40,
	"",	0x00
};

struct adsym	pulm[] = {	/* pull from on system stack */
	"ccr",	0x01,
	"k",	0x02,
	"z",	0x04,
	"y",	0x08,
	"x",	0x10,
	"e",	0x20,
	"d",	0x40,
	"",	0x00
};

