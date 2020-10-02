/******************************************************************
	pop.c, the operand field parser...
*/

/*		(C) Copyright 1982 Steve Passe		*/
/*		All Rights Reserved					*/

/* version 1.00 */
/* created 11/8/82 */

/* version 1.01

	8/30/83	modified for Aztec ver. 1.05g	smp
	12/23/83 modified '*' to evaluate according to rorg smp

*/

/* begincode */

/* includes */

#define AZTECZII 1

#ifndef AZTECZII
#include <stdio.h>
#else
#include "stdio.h"								/* with aztecII compiler */
#endif

#include "b:as68.h"

#define RTERM		0x0100
#define SLASH		0x0101
#define HYPHEN		0x0102

#define PC_VALUE	0x0103
#define IMD			0x0104
#define COMMA		0x0105
#define POINT		0x0106
#define VALUE		0x0107
#define LVALUE		0x0108
#define FWRD_REF 	0x0109

#define TOP			0x0200
#define BOTTOM		0x0201
#define LP			0x0202
#define RP			0x0203
#define PLUS		0x0204
#define MINUS		0x0205
#define MULT		0x0206
#define DIV			0x0207
#define LAND		0x0208
#define LOR			0x0209
#define LSHIFT		0x020a
#define RSHIFT		0x020b
#define NEGATE		0x020c
#define LIM_C		1
#define LM_C		2
#define RXP			0x0303			/* signals a forced short address */

/* externals */

extern struct _oprnd op1, op2;			/* structs to hold operand val */
extern struct _symbol *symtbl;
extern char pass;
extern long loc_counter;
extern FLAG abs_long;					/* default to absolute long add.*/
extern FLAG rorg;
extern char statement[STMNT_SIZE];		/* statement line buffer */
extern char *opfld_ptr;
extern long lex_val;
extern char *p;
static FLAG lv;

unsigned reg_num;
int last_typ;

long val_stk[10] = {0L};
int oprtr_stk[10] = {BOTTOM};
int val_sp;
int oprtr_sp;

op_eval(op)
struct _oprnd *op;
{
/**	register int r; */
	int shorty = NULL;					/* force short address this line */
	long val_push();

	op->_long_val = op->_inxl = op->_rel_lbl = NULL;
	op->_typ = last_typ = NULL;
	op->_regtyp = op->_iregtyp = op->_reg = op->_ireg = NULL;
	oprtr_sp = val_sp = 0;
	reg_num = NULL;
	switch (*opfld_ptr) {
	case '\t':
	case '\n':
	case ' ':
	case NULL:
		return (op->_typ = _none);
	case ',':
		++opfld_ptr;
	}
top:
	switch (/**r =**/ op_scan(op)) {
	case COMMA:
	case NULL:				/* determine operand type, finalize values in op */
		reduce(TOP);		/* get into one value */
		switch (op->_typ) {
		case _imd:
			op->_data = val_stk[1];
			return _imd;
		case _reglst:
			return _reglst;
		default:
			if (abs_long) op->_long_val = TRUE;
			if (shorty) op->_long_val = FALSE;	/* shorty overrides abs_long
											AND long values in expression */
			op->_addr = val_stk[1];
			if (!op->_long_val && pass == 2) { /* pass 2/short, check range */
				if (op->_addr & ~0xffff) {
					op->_addr = 0;
					return (op->_typ = LBL_RANGE);
				}
			}
			return (op->_typ = _address);
		}
	case PLUS:
		reduce(PLUS);
		break;
	case MINUS:
		reduce(MINUS);
		break;
	case MULT:
		reduce(MULT);
		break;
	case DIV:
		reduce(DIV);
		break;
	case LAND:
		reduce(LAND);
		break;
	case LOR:
		reduce(LOR);
		break;
	case LSHIFT:
		reduce(LSHIFT);
		break;
	case RSHIFT:
		reduce(RSHIFT);
		break;
	case NEGATE:
		reduce(NEGATE);
		break;
	case RXP:
		reduce(RP);
		if (val_sp != 1) return (op->_typ = OPRND_EVAL);
		shorty = TRUE;
		break;
	case RP:
		reduce(RP);
		break;
	case LP:
		oprtr_push(LP);
		break;
	case PC_VALUE:
		if (rorg) op->_rel_lbl = TRUE;
		val_push(loc_counter);
		break;
	case IMD:
		op->_typ = _imd;
		break;
	case LVALUE:
		op->_long_val = TRUE;
	case VALUE:
		val_push(lex_val);
		break;
	case FWRD_REF:
/** op->_forward = TRUE; ? */
		val_push(lex_val);
		break;
	case _an:
		return (op->_typ = _an);
	case _dn:
		return (op->_typ = _dn);
	case _pd_ani:
		return (op->_typ = _pd_ani);
	case _ani_pi:
		return (op->_typ = _ani_pi);
	case _ani:
		if (val_sp == 0) {
			return (op->_typ = _ani);
		}
		if (reduce(TOP) != 1) return (op->_typ = OPRND_EVAL);
		op->_displ = val_stk[1];
		return (op->_typ = _d16_ani);
	case _pc:
		if (reduce(TOP) != 1) return (op->_typ = OPRND_EVAL);
		op->_displ = val_stk[1];
		return (op->_typ = (op->_iregtyp ? _labeli : _label));
/**		return (op->_typ = _labeli); **/
	case _an_inx:
		if (reduce(TOP) != 1) return (op->_typ = OPRND_EVAL);
		op->_displ = val_stk[1];
		return (op->_typ = _d8_anx);
	case _ccr:		return (op->_typ = _ccr);
	case _sr:		return (op->_typ = _sr);
	case _usp:		return (op->_typ = _usp);
	case _reglst:	return (op->_typ = _reglst);

	case ERROR:
	default:
		return (op->_typ = OPRND_EVAL);
	}
	goto top;
}

reduce(tkn)
int tkn;
{
	long val_push(), val_pop();
	long temp;

	while (oprtr_stk[oprtr_sp] >= (tkn & ~1)) {
		switch (oprtr_stk[oprtr_sp--]) {
		case PLUS:
			val_push(val_pop() + val_pop());
			continue;
		case MINUS:
			temp = val_pop();
			val_push(val_pop() - temp);
			continue;
		case MULT:
			val_push(val_pop() * val_pop());
			continue;
		case DIV:
			temp = val_pop();
			val_push(val_pop() / temp);
			continue;
		case LAND:
			val_push(val_pop() & val_pop());
			continue;
		case LOR:
			val_push(val_pop() | val_pop());
			continue;
		case LSHIFT:
			temp = val_pop();
			val_push(val_pop() << temp);
			continue;
		case RSHIFT:
			temp = val_pop();
			val_push(val_pop() >> temp);
			continue;
		case NEGATE:
			val_push(-1 * val_pop());
			continue;
		case LP :
			return val_sp;
		case BOTTOM:
			++oprtr_sp;
			return val_sp;
		}
	}
	oprtr_push(tkn);
	return val_sp;
}

long
val_push(l)
long l;
{
	return (val_stk[++val_sp] = l);
}

long
val_pop()
{
	return (val_stk[val_sp--]);
}

oprtr_push(t)
int t;
{
	return (oprtr_stk[++oprtr_sp] = t);
}

oprtr_pop()
{
	return (oprtr_stk[oprtr_sp--]);
}

op_scan(op)
struct _oprnd *op;
{
	int typ, len;
	char s[32];
	long _dtol(), _htol(), _btol(), _actol();

	p = opfld_ptr;

	if (typ = reg_scan(op)) {
		opfld_ptr = p;
		if (typ > 0) return typ;
		goto foops;
	}
	switch (*opfld_ptr++) {
	case '-':	/* choose between NEGATE and MINUS */
		switch (last_typ) {
		case NULL:
		case IMD:
		case LP:
		case PLUS:
		case MINUS:
		case MULT:
		case DIV:
		case LAND:
		case LOR:
			return (last_typ = NEGATE);
		case RXP:
		case LSHIFT:
		case RSHIFT:
			goto foops;
		default:
			return (last_typ = MINUS);
		}
	case '(':										/* (			*/
		return (last_typ = LP);
	case ')':										/* )			*/
		if (opfld_ptr[0] == '.'						/* ).s			*/
			&& (opfld_ptr[1] == 's' || opfld_ptr[1] == 'S')) {
			p = (opfld_ptr += 2);		/* skip the '.s' */
			return (last_typ = RXP);
		}
		else return (last_typ = RP);
	case '#':										/* #			*/
		return (last_typ = IMD);
	case '$':										/* $			*/
		lex_val = _htol();
		return (last_typ = (lv) ? LVALUE : VALUE);
	case '%':										/* %			*/
		lex_val = _btol();
		return (last_typ = VALUE);
	case '*':										/* *			*/
		switch (last_typ) {
		case NULL:
		case IMD:
		case LP:
		case PLUS:
		case MINUS:
		case MULT:
		case DIV:
		case IMD:
		case LAND:
		case LOR:
		case LSHIFT:
		case RSHIFT:
			return (last_typ = PC_VALUE);
		case RXP: goto foops;
		default:
			return (last_typ = MULT);
		}
	case '/':										/* /			*/
		return (last_typ = DIV);
	case '+':										/* +			*/
		return (last_typ = PLUS);
	case '&':										/* &			*/
		return (last_typ = LAND);
	case '!':										/* !			*/
		return (last_typ = LOR);
	case '>':										/* >...			*/
		if (*opfld_ptr++ != '>') goto foops;		/* >FAIL		*/
		return (last_typ = RSHIFT);					/* >>			*/
	case '<':										/* <...			*/
		if (*opfld_ptr++ != '<') goto foops;		/* <FAIL		*/
		return (last_typ = LSHIFT);					/* <<			*/
	case '\'':										/* '			*/
		lex_val = _actol();
		return (last_typ = VALUE);
	case ',':										/* ,			*/
		return (last_typ = COMMA);
	case ' ':
	case '\t':
	case '\n':
		--opfld_ptr;
		return (last_typ = NULL);
	}
	--opfld_ptr;

/* look for decimal number */
	if (*opfld_ptr >= '0' && *opfld_ptr <= '9') {
		lex_val = _dtol();
		return (last_typ = (lv) ? LVALUE : VALUE);
	}
	p = opfld_ptr;
	if (nextc(&p) == LIM_C) {							/* look for symbol...*/
		len = 1;
		while ((typ = nextc(&p)) == LIM_C || typ == LM_C) ++len;
		savefield(opfld_ptr, len, s);
		opfld_ptr = p - 1; /* back up from failure char */
		if (typ = symsearch(s)) {
			lex_val = symtbl[typ]._val;
			if (symtbl[typ]._atr & RELOCATABLE) op->_rel_lbl = TRUE;
			return (last_typ = (lex_val > 32767L || lex_val < -32768L) ?
									LVALUE : VALUE);
		}
		lex_val = 0;
		return (last_typ = (pass == 1) ? FWRD_REF : UNDEF_SYMBOL);
	}

	++opfld_ptr;

foops:	/* try to align on next operand arg */
	--opfld_ptr;
	while ((*opfld_ptr != ',') && (*opfld_ptr != '\t')
			&& (*opfld_ptr != '\n') && (*opfld_ptr != ' ')) {
		++opfld_ptr;
	}
	return (last_typ = ERROR);
}

reg_scan(op)
struct _oprnd *op;
{
	p = opfld_ptr;

	while (TRUE) {
		switch (*p++) {
		case '-':										/* -...			*/
			if (*p++ != '(') return NULL;
			switch (reg_name()) {
			case NULL:	return NULL;
			case _an:									/* -(an...		*/
				if (*p++ != ')') return ERROR;
				op->_reg = reg_num;
				op->_regtyp = 'a';
				return _pd_ani;							/* -(an)		*/
			default: return ERROR;						/* -(FAIL		*/
			}
		case '(':										/* (...			*/
			switch (reg_name()) {
			case NULL:	return NULL;
			case _an:									/* (an...		*/
				op->_reg = reg_num;
				op->_regtyp = 'a';
				switch (*p++) {
				case ')':								/* (an)...		*/
					if (*p == '+') {					/* (an)+		*/
						++p;
						return _ani_pi;
					}
					return _ani;						/* (an)			*/
				case ',':
					goto index;			/* look for index reg */
				default: return ERROR;
				}
			case _pc:									/* (pc...		*/
				op->_reg = 8;	/* special case of addr. reg. */
				op->_regtyp = 'p';
				switch (*p++) {
				case ')':								/* (pc)			*/
					return _pc;
				case ',':
index:				switch (reg_name()) {	/* look for index reg. */
					case _dn:							/* (an/pc,dn...		*/
					case _dnw:							/* (an/pc,dn.w...	*/
						op->_iregtyp = 'd';
						op->_inxl = FALSE;
						break;
					case _dnl:							/* (an/pc,dn.l...	*/
						op->_iregtyp = 'd';
						op->_inxl = TRUE;
						break;
					case _an:							/* (an/pc,an...		*/
					case _anw:							/* (an/pc,an.w...	*/
						op->_iregtyp = 'a';
						op->_inxl = FALSE;
						break;
					case _anl:							/* (an/pc,an.l...	*/
						op->_iregtyp = 'a';
						op->_inxl = TRUE;
						break;
					default:
						return ERROR;
					}
					if (*p++ != ')') return ERROR;
					op->_ireg = reg_num;
					return (op->_regtyp == 'a') ? _an_inx : _pc;
					break;
				default: return ERROR;
				}
			default: return ERROR;
			}
		default: break;		/* neither '-' or '(' */
		}
		--p;
		switch (reg_name()) {
		case NULL:
			return NULL;
		case _an:
			op->_reg = reg_num;
			op->_regtyp = 'a';
			return _an;
		case _dn:
			op->_reg = reg_num;
			op->_regtyp = 'd';
			return _dn;
		case _ccr:
			op->_regtyp = 'c';
			return _ccr;
		case _sr:
			op->_regtyp = 's';
			return _sr;
		case _usp:
			op->_regtyp = 'u';
			return _usp;
		case _reglst:
			op->_reg_list = reg_num;
			return _reglst;
		case ERROR:
		default:
			++p;		/* foops will backtrack...*/
			return ERROR;
		}
	}
}


reg_name()
{
	int m_offset = 0;
	FLAG list = FALSE;
	unsigned mask = 0x0001;

top:
	switch (*p) {
	case 'a':										/* a...			*/
	case 'A':
		m_offset = 8;
	case 'd':										/* d...			*/
	case 'D':
		if (p[1] < '0' || p[1] > '7') return (list) ? ERROR : NULL;
		switch (term_char(p[2])) {
		case RTERM:									/* xn			*/
			p += 2;
			if (list) {
				reg_num |= mask << ((*(p - 1) - '0') + m_offset);
				return _reglst;
			}
			reg_num = *(p - 1) -'0';
			return (m_offset) ? _an : _dn;
		case SLASH:									/* xn/...		*/
			list = TRUE;
			reg_num |= mask << ((p[1] - '0') + m_offset);
			p += 3;
			goto top;
		case HYPHEN:								/* xn-...		*/
			list = TRUE;
			switch (reg_range()) {
			case NULL:								/* xn-xn		*/
				p += 5;
				return _reglst;
			case _reglst:							/* xn-xn/...	*/
				p += 6;
				goto top;
			case ERROR:								/* xn-xnFAIL	*/
				return ERROR;
			}
		case POINT:									/* xn....		*/
			reg_num = p[1] - '0';
			p += 4;
			switch (*(p - 1)) {
			case 'l':								/* xn.l...		*/
			case 'L':
				return (m_offset) ? _anl : _dnl;
			case 'w':								/* xn.w...		*/
			case 'W':
				return (m_offset) ? _anw : _dnw;
			default:								/* xn.FAIL		*/
				return ERROR;
			}
		default:	return (list) ? ERROR : NULL;	/* xnFAIL		*/
		}
	case 'c':										/* c...			*/
	case 'C':
		if (p[1] != 'c' && p[1] != 'C') return NULL;/* cFAIL		*/
		if (p[2] != 'r' && p[2] != 'R') return NULL;/* ccFAIL		*/
		switch (term_char(p[3])) {
		case RTERM:									/* ccr			*/
			p += 3;
			return (list) ? ERROR : _ccr;
		case SLASH: return ERROR;					/* ccrFAIL		*/
		default:	return (list) ? ERROR : NULL;	/* ccrFAIL		*/
		}
	case 's':										/* s...			*/
	case 'S':
		switch (p[1]) {
		case 'r':									/* sr...		*/
		case 'R':
			switch (term_char(p[2])) {
			case RTERM:								/* sr			*/
				p += 2;
				return (list) ? ERROR : _sr;
			case SLASH: return ERROR;				/* srFAIL		*/
			default:	return (list) ? ERROR : NULL;/* srFAIL		*/
			}
		case 'p':									/* sp...		*/
		case 'P':
			switch (term_char(p[2])) {
			case RTERM:								/* sp			*/
				p += 2;
				if (list) {
					reg_num |= 0x8000;
					return _reglst;
				}
				reg_num = 7;
				return _an;
			case SLASH:								/* sp/...		*/
				list = TRUE;
				reg_num |= 0x8000;
				p += 3;
				goto top;
			default:	return (list) ? ERROR : NULL;/* spFAIL		*/
			}
		}
	case 'u':										/* u...			*/
	case 'U':
		if (p[1] != 's' && p[1] != 'S') return NULL;/* uFAIL		*/
		if (p[2] != 'p' && p[2] != 'P') return NULL;/* usFAIL		*/
		switch (term_char(p[3])) {
		case RTERM:									/* usp			*/
			p += 3;
			return (list) ? ERROR : _usp;
		case SLASH: return ERROR;					/* uspFAIL		*/
		default:	return (list) ? ERROR : NULL;	/* uspFAIL		*/
		}
	case 'p':
	case 'P':
		if (p[1] != 'c' && p[1] != 'C') return NULL;/* pFAIL		*/
		switch (term_char(p[2])) {
		case RTERM:									/* pc			*/
			p += 2;
			return (list) ? ERROR : _pc;
		case SLASH: return ERROR;					/* pcFAIL		*/
		default:	return (list) ? ERROR : NULL;	/* pcFAIL		*/
		}
	default:										/* FAIL			*/
		return NULL;
	}
}

term_char(c)
char c;
{
	switch (c) {
	case ')':
	case '\t':
	case ',':
	case '\n':
	case ' ':
		return RTERM;
	case '.':
		return POINT;
	case '/':
		return SLASH;
	case '-':
		return HYPHEN;
	default:
		return NULL;
	}
}

reg_range()
{
	int start, stop;
	unsigned mask = 0x0001;

	if (p[0] != p[3]) return ERROR;
	if ((p[4] < '0') || (p[4] > '7')) return ERROR;
	start = p[1] - '0';
	if ((p[4] - '0') < start) {
		stop = start;
		start = p[4] - '0';
	}
	else stop = p[4] - '0';
	if (p[0] == 'a'  || p[0] == 'A') {
		start += 8;
		stop += 8;
	}
	for ( ; start <= stop; ++start) {
/**		reg_num |= mask << start; */
/** aztec has problems... */
		if (start) reg_num |= mask << start;
		else reg_num |= mask;
	}
	switch (term_char(p[5])) {
	case RTERM:
		return NULL;
	case SLASH:
		return _reglst;
	case HYPHEN:
		return ERROR;
	}
}

long
_dtol()
{
	long val = 0L;
	register char c, x = 0;

	while ((c = *opfld_ptr++) >= '0' && c <= '9') {
		val = val * 10 + c - '0';
		++x;
	}
	lv = (x > 5) ? YES : NO;
	--opfld_ptr;
	return val;
}

long
_htol()
{
	long val = 0L;
	int i;
	register char c, x = 0;

	while (TRUE) {
		if ((c = *opfld_ptr++) >= '0' && c <= '9') {
			i = c - '0';
		}
		else if ((c = tolower(c)) >= 'a' && c <= 'f') {
			i = (c - 'a') + 10;
		}
		else {
			lv = (x > 4) ? YES : NO;
			--opfld_ptr;
			return val;
		}
		val = val * 16 + i;
		++x;
	}
}

long
_btol(s)
char *s;
{
	long val = 0L;
	char c;

	while ((c = *opfld_ptr++) >= '0' && c <= '1')
		val = val * 2 + c - '0';
	--opfld_ptr;
	return val;
}

long
_actol()
{
	register int x;
	long val = 0L;
	char c;

	for (x = 4; x; --x) {
		if (*opfld_ptr == '\'') {
			++opfld_ptr;					/* pass it */
			if (*opfld_ptr != '\'') {		/* end of ascii str */
				return val;
			}
		}
		val = val * 256 + *opfld_ptr++;
	}
	if (*opfld_ptr != '\'') return ERROR;
	++opfld_ptr;
	return val;
}

/*
#undef RTERM
#undef SLASH
#undef HYPHEN
#undef PLUS
#undef MINUS
#undef MULT
#undef DIV
#undef LAND
#undef LOR
#undef LSHIFT
#undef RSHIFT
#undef NEGATE
#undef PC_VALUE
#undef IMD
#undef LP
#undef RP
#undef COMMA
#undef POINT
#undef VALUE
#undef LIM_C
#undef LM_C

*/
 - '0';
		}
		else if ((c = tolower(c)) >= 'a' && c <= 'f') {
			i = (c - 'a') + 10;
		}
		els