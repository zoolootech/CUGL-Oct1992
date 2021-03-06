/* m11adr.c */

/*
 * (C) Copyright 1989,1990
 * All Rights Reserved
 *
 * Alan R. Baldwin
 * 721 Berkeley St.
 * Kent, Ohio  44240
 */

#include <stdio.h>
#include <setjmp.h>
#include "asm.h"
#include "m6811.h"

int
addr(esp)
register struct expr *esp;
{
	register c;
	register struct area *espa;
	register char *tcp;

	if ((c = getnb()) == '#') {
		expr(esp, 0);
		esp->e_mode = S_IMMED;
	} else
	if (c == ',') {
		esp->e_mode = 0;
		esp->e_flag = 0;
		esp->e_addr = 0;
		esp->e_base.e_ap = NULL;
		c = admode(abdxy);
		if (c == S_X) {
			esp->e_mode = S_INDX;
		} else
		if (c == S_Y) {
			esp->e_mode = S_INDY;
		} else {
			aerr();
		}
	} else
	if (c == '*') {
		expr(esp, 0);
		esp->e_mode = S_DIR;
		if (more()) {
			comma();
			tcp = ip;
			if ((c = admode(abdxy)) != 0) {
				if (c == S_X) {
					esp->e_mode = S_INDX;
				} else
				if (c == S_Y) {
					esp->e_mode = S_INDY;
				} else {
					aerr();
				}
			} else {
				ip = --tcp;
			}
		}
	} else {
		unget(c);
		if ((esp->e_mode = admode(abdxy)) != 0) {
			;
		} else {
			expr(esp, 0);
			if (more()) {
				comma();
				tcp = ip;
				if ((c = admode(abdxy)) != 0) {
					if (c == S_X) {
						esp->e_mode = S_INDX;
					} else
					if (c == S_Y) {
						esp->e_mode = S_INDY;
					} else {
						aerr();
					}
				} else {
					ip = --tcp;
				}
			} else {
				if (esp->e_flag == 0 &&
					esp->e_base.e_ap == NULL &&
					(esp->e_addr & ~0xFF) == 0 ) {
					esp->e_mode = S_DIR;
				} else {
					esp->e_mode = S_EXT;
				}
			}
		}
	}
	c = esp->e_mode;
	if (c == S_INDX || c == S_INDY || c == S_DIR) {
		if (esp->e_flag == 0 && esp->e_base.e_ap == NULL) {
			if (esp->e_addr & ~0xFF)
				err('d');
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

struct adsym	abdxy[] = {	/* a, b, d, x, or y registers */
	"a",	S_A,
	"b",	S_B,
	"d",	S_D,
	"x",	S_X,
	"y",	S_Y,
	"",	0x00
};
