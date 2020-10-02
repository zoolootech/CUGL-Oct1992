/******************************************************************
	bc.c, code building function of as68 assembler
*/

/*		(C) Copyright 1982 Steve Passe		*/
/*		All Rights Reserved					*/

/* version 1.00 */
/* created 10/21/82 */

/* version 1.01

	8/30/83	ver. 1.01 modified for Aztec ver. 1.05g	smp

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

/* externals */

extern char pass;							/* present pass number, 1 or 2 */
extern unsigned line_count;					/* line number of source file */
extern long loc_counter;					/* address to assemble obj code */
extern int loc_plus;						/* increment to loc counter */
extern FLAG abs_long;						/* default to absolute long add.*/
extern FLAG rorg;							/* in pc relative mode */
extern char label[32];					/* buffer for label from preparse */
extern char instr[33];					/* buffer for mnem, psdo or macro */

extern struct _mtable mtable[];				/* mnemonic lookup table */
extern struct _mvalue mvalue[];				/* mnemonic code table */
extern struct _oprnd op1, op2;				/* structs to hold operand val */
extern char code[];							/* code array */

p1_mnem(mt)
struct _mtable *mt;
{
	int result, index;

	if ((index = match(mt)) == ERROR) {
/** flush error stack someday? */
		return ERROR;
	}
	switch (result = mvalue[index]._opc_len) {	/* determine code length... */
	case 1:
	case 2:
	case 3:
		break;
	case 12:								/* _imd not contained in opcode */
	case 13:
		result -= 10;						/* strip '10' flag */
		result += p1_codelen(&op2);			/* add second operand length */
		break;
	case 0:										/* gotta figure it out */
		if (/**rorg**/ op1._rel_lbl && mvalue[index]._optyp1 == _sadr
			&& (op1._typ == _address || op1._typ == _d16_ani)) {
			result = 2 + p1_codelen(&op2);		/* 1 for opcode, 1 for raddr */
		}
		else result = p1_codelen(&op1) + p1_codelen(&op2) + 1; /* 1 for code */
		break;
	}
	loc_plus = result * 2;
	return result;
}

p1_codelen(op)
struct _oprnd *op;
{
	int result;

	switch (op->_typ) {
	case _address:
		result = (op->_long_val) ? 2 : 1;
		break;
	case _d16_ani:
	case _d8_anx:
	case _labeli:
	case _reglst:
	case _label:	/* needed? */
		result = 1;
		break;
	default:
		result = 0;
	}
	return result;
}

p2_mnem(mt)
struct _mtable *mt;
{
	register int x;									/* scratch */
	int index		;								/* index to mvalue */
	int result;

	for (x = 0; x < 10; ++x) {			/* init code array to 0 */
		code[x] = 0;
	}
	if ((index = match(mt)) == ERROR) {
		dump_code(CODE, code, 0);
		return ERROR;
	}
	code[0] = mvalue[index]._opcb1;							/* get opcode */
	code[1] = mvalue[index]._opcb2;							/* get opcode */

	if ((result = (*mvalue[index]._p2_action)()) < 0) {		/* call funct */
		err_out(result);
		loc_plus = 0;
	}
	else loc_plus = ++result * 2;					/* bytes created */
	dump_code(CODE, code, loc_plus);				/* send them */
	return result;									/* words created */
}

match(mt)
struct _mtable *mt;
{
	int index;										/* index to mvalue */
	int type1, type2;						/* operand types */

	if ((type1 = op_eval(&op1)) < 0) {				/* legal type found? */
		err_out(BAD_OP1);			/* first op bad */
	}
	if ((type2 = op_eval(&op2)) < 0) {				/* second op? */
		err_out(BAD_OP2);			/* no good */
	}
	if (type1 < 0 || type2 < 0) {
		return ERROR;							/* use NULL opcode */
	}
	if ((index = type_search(mt, type1, type2)) <=0) {
		switch (index) {
		case ILGL_OP1:
			err_out(ILGL_OP1);			/* no good */
			return ERROR;
		case ILGL_OP2:
			err_out(ILGL_OP2);			/* no good */
			return ERROR;
		default:
			return ERROR;
		}
	}
	return index;
}

type_search(mt, type1, type2)
struct _mtable *mt;
int type1;
int type2;
{
	register int x, y;								/* count and index */
	char t1, t2;									/* temp for array value */

	for (x = mt->_mvc, y = mt->_mvi; x; --x, ++y) {	/* search */
											/* find op1 match... */
		if (((t1 = mvalue[y]._optyp1) == type1)		/* if exact match */
		|| ((!(t1 & ~0xf0)) && (t1 & type1))) {	/* or classtyp and match */
											/* search for second match */
			for ( ; (mvalue[y]._optyp1 == t1) && x; --x, ++y) {
				if (((t2 = mvalue[y]._optyp2) == type2)	/* 2nd matches */
				|| ((!(t2 & ~0xf0)) && (t2 & type2))) {	/* or typ & match */
					return y;								/* found a match */
				}
			}
			return ILGL_OP2;
		}
	}
	return ILGL_OP1;
}



/* mask 3 bit immediate data from op1 over bits 3-1 of code [0] */

bbb(op)
struct _oprnd *op;
{
	if (op->_data & ~7) return ERR_BBB;
	code[0] |= (op->_data << 1);
	return NULL;
}

/* first ext word holds bit number (7/31) from first operand */

bbbx(size, op)
int size;
struct _oprnd *op;
{
	switch (size) {
	case 3:
		if (op->_data & ~7L) return ERR_BX3;					/* range? */
	case 5:
		if (op->_data & ~31L) return ERR_BX5;
		code[3] = op->_data;
		return 1;											/* 1 ext word */
	default:
		return ERR_BX;
	}
}

/** mask bits 3-1 of first byte with op1, 8 = 00 */

ccc(op)
struct _oprnd *op;
{
	if (op->_data < 1L || op->_data > 8L) return ERR_CCC;
	code[0] |= (op->_data << 1) & 0x0f;
/**	if (op->_data != 8L) code[0] |= op->_data << 1; **/

	return NULL;									/* no words added */
}

/* mask bits 3-1 of first byte, or bits 2-0 of second, with op reg */

rsd(byt, op)
int byt;
struct _oprnd *op;
{
	code[byt] |= (byt) ? op->_reg : op->_reg << 1;
	return NULL;
}

/* register mask list from op into ext */

mmkk(order, op)
char order;
struct _oprnd *op;
{
	register int x;
	unsigned u1 = 0;
	unsigned m1 = 0x0001;
	unsigned m2 = 0x8000;

	switch (order) {
	case '>':
		code[2] = op->_reg_list >> 8;						/* high byte */
		code[3] = op->_reg_list;							/* low byte */
		return 1;
	case '<':

		for (x = 15; x >= 0; --x) {
			u1 |= (op->_reg_list & (m1 << x)) ? (m2 >> x) : 0;
		}

		code[2] = u1 >> 8;						/* high byte */
		code[3] = u1;							/* low byte */
		return 1;
	default:
		return ERR_MK;
	}
}

/* combined e, f, g, h, and j routines, 12/4/82 */

efghj(arg, i, op)
char arg;
int i;
struct _oprnd *op;
{
	int len, displ;

	switch (op->_typ) {
	case _ani:								/* (an) */
		if (arg == 'g') {
			code[0] |= (op->_reg << 1);
			code[1] |= 0x80;
		}
		else {
			code[1] |= (0x10 | op->_reg);
		}
		return NULL;
	case _pd_ani:								/* -(an) */
		switch (arg) {
		case 'g':
			code[0] |= (0x01 | op->_reg << 1);
			return NULL;
		case 'j': return ERR_J;
		case 'h': return ERR_H;
		default:
			code[1] |= (0x20 | op->_reg);
			return NULL;
		}

	case _ani_pi:								/* (an)+ */
		switch (arg) {
		case 'g':
			code[0] |= (op->_reg << 1);
			code[1] |= 0xc0;
			return NULL;
		case 'j': return ERR_J;
		case 'h': return ERR_H;
		default:
			code[1] |= (0x18 | op->_reg);
			return NULL;
		}

	case _d16_ani:								/* d16(an) */
		switch (arg) {
		case 'e':
		case 'j':
			if (/**rorg**/ op->_rel_lbl) {
				op->_ireg = op->_reg;				/* reg is an index */
				op->_iregtyp = op->_regtyp;			/* move type */
				op->_displ -= (loc_counter + 2);	/* make pc relative */
				code[1] |= 0x3b;
				return _d8_i(i, op);				/* indexed label */
			}
			break;
		case 'g':
			code[0] |= (0x01 | op->_reg << 1);
			code[1] |= 0x40;
			return _d16(i, op);
		}
		code[1] |= (0x28 | op->_reg);
		return _d16(i, op);

	case _d8_anx:								/* d8(an,i) */
		if (arg == 'g') {
			code[0] |= (0x01 | op->_reg << 1);
			code[1] |= 0x80;
		}
		else {
			code[1] |= (0x30 | op->_reg);
		}
		return _d8_i(i, op);

	case _address:		/* abs. add., short or long, or label if _rel_lbl */
		switch (arg) {
		case 'e':
		case 'j':
			if (/**rorg**/ op->_rel_lbl) {
				code[1] |= 0x3a;
				op->_displ = op->_addr - (loc_counter + 2);	/* rel. to pc */
				return _d16(i, op);						/* finish as 'label' */
			}
			break;
		case 'g':
			code[1] |= 0xc0;
			if (op->_long_val) {
				code[0] |= 0x03;
				code[i++] = op->_addr >> 24;
				code[i++] = op->_addr >> 16;
				len = 2;
			}
			else {
				code[0] |= (0x01);
				len = 1;
			}
			code[i++] = op->_addr >> 8;
			code[i] = op->_addr;
			return len;
		}
		if (op->_long_val) {
			code[1] |= (0x39);
			code[i++] = op->_addr >> 24;
			code[i++] = op->_addr >> 16;
			len = 2;
		}
		else {
			code[1] |= (0x38);
			len = 1;
		}
		code[i++] = op->_addr >> 8;
		code[i] = op->_addr;
		return len;

	case _label:								/* relative label */
		switch (arg) {
		case 'g': return ERR_G;
		case 'f': return ERR_F;
		case 'h': return ERR_H;
		default:
			code[1] |= 0x3a;
			op->_displ -= (loc_counter + 2);	/* rel. to pc */
			return _d16(i, op);
		}

	case _labeli:								/* label(i) */
		switch (arg) {
		case 'g': return ERR_G;
		case 'f': return ERR_F;
		case 'h': return ERR_H;
		default:
			code[1] |= 0x3b;
			op->_displ -= (loc_counter + 2);	/* rel. to pc */
			return _d8_i(i, op);
		}

	default:
		return ERR_EFGHJ;
	}
}

_d16(i, op)
int i;
struct _oprnd *op;
{
	if ((op->_displ > 32767) || (op->_displ < -32768)) return ERR_D16;
	code[i] = op->_displ >> 8;						/* displacement... */
	code[i+1] = op->_displ;							/* ...into ext */
	return 1;										/* 1 ext word returned */
}

_d8_i(i, op)
int i;
struct _oprnd *op;
{
	if ((op->_displ > 127) || (op->_displ < -128)) return ERR_D8I;
	if (op->_iregtyp == 'a') code[i] |= 0x80;		/* index is addr reg */
	code[i] |= (op->_ireg << 4);					/* add index reg # */
	if (op->_inxl) code[i] |= 0x08;					/* mark long index size */
	code[i+1] = op->_displ;							/* 8 bit displacement */
	return 1;
}

/* 8/16 bit address displacement */

xxxx(size, op)
int size;
struct _oprnd *op;
{
	int displ;

	switch (op->_typ) {
	case _address:									/* absolute address */
		displ = op->_addr - (loc_counter + 2);		/* make relative to pc */
		break;
	case _label:									/* relative address */
		displ = op->_displ;
		break;
	default:										/* somethings wrong... */
		return ERR_XXX;
	}
	switch (size) {
		case 8:
			if (displ > 127 || displ < -128) return ERR_XX;	/* check range */
			code[1] |= displ;							/* put into array */
			return NULL;								/* no ext word */
		case 16:
			if (displ > 32766 || displ < -32768) return ERR_XXXX; /* range */
			code[2] = displ >> 8;
			code[3] = displ;
			return 1;								/* 1 ext word returned */
		default:
			return ERR_XXX;
	}
}

/** */

yy(op)
struct _oprnd *op;
{
	if (op->_data > 255L || op->_data < -128L) return ERR_Y;
	code[1] = op->_data;
	return NULL;								/* no ext word */
}

oyz(size, op)
int size;
struct _oprnd *op;
{
	switch (size) {
	case 8:
		if (op->_data > 255L || op->_data < -128L) return ERR_OY;
		code[3] = op->_data;
		return 1;								/* one ext word added */
	case 16:
		if (op->_data > 65535L || op->_data < -32768L) return ERR_YY;
		code[2] = op->_data >> 8;
		code[3] = op->_data;
		return 1;								/* one ext word added */
	case 32:
		code[2] = op->_data >> 24;
		code[3] = op->_data >> 16;
		code[4] = op->_data >> 8;
		code[5] = op->_data;
		return 2;
	default:
		return ERR_OYZ;
	}
}

/*
byte 1	byte 2	byte 3	byte 4	byte 5	byte 6	byte 7	byte 8	byte 9	byte 10
*/

/* none	none	*/

nn()
{
	return NULL;							/* no action required */
}

/*	none	none	zero	yy	*/

nnoy()
{
	return oyz(8, &op1);					/* 8 bits imd data */
}

/*	none	none		yyyy	*/

nnyy()
{
	return oyz(16, &op1);					/* 16 bits imd data */
}

/*	none	xx	*/

nx()
{
	return xxxx(8, &op1);						/* 16 bit displ from op1 */
}

/*	none	none		xxxx	*/

nnxx()
{
	return xxxx(16, &op1);						/* 16 bit displ from op1 */
}

/*	none	ddd	*/

nd()
{
	return rsd(1, &op1);					/* dest reg from op1 into byte 1 */
}

nd2()
{
	return rsd(1, &op2);					/* dest reg from op2 into byte 1 */
}

/*	none	ddd		zero	bbbbb	*/

ndob()
{
	rsd(1, &op2);							/* dest reg from op2 into byte 1 */
	return bbbx(5, &op1);					/* bit field spec into first ext */
}

/*	none	ddd			kkkk	*/

ndkk()
{
	rsd(1, &op2);							/* dest reg from op2 into byte 1 */
	return mmkk('<', &op1);						/* reglist from op1 into ext */
}

/*	none	ddd		zero	yy	*/

ndoy()
{
	rsd(1, &op2);							/* dest reg from op2 into byte 1 */
	return oyz(8, &op1);					/* 8 bits imd data */
}

/*	none	ddd			yyyy	*/

ndyy()
{
	rsd(1, &op2);							/* dest reg from op2 into byte 1 */
	return oyz(16, &op1);					/* 16 bits of immediate data */
}

/*	none	ddd			yyyy			zzzz	*/

ndyz()
{
	rsd(1, &op2);							/* dest reg from op2 into byte 1 */
	return oyz(32, &op1);					/* 32 bits of immediate data */
}

/*	none	eeeeee		[ext]			[ext]	*/

ne()
{
	return efghj('e', 2, &op1);
}

/*	none	ffffff		[ext]			[ext]	*/

nf()
{
	return efghj('f', 2, &op1);
}

nf2()
{
	return efghj('f', 2, &op2);
}

/*	none	ffffff	zero	bbb			[ext]			[ext]	*/

nfob()
{
	bbbx(3, &op1);
	return (efghj('f', 4, &op2) + 1);
}

/*	none	ffffff	zero	yy			[ext]			[ext]	*/

nfoy()
{
	oyz(8, &op1);								/* 8 bits imd data */
	return (efghj('f', 4, &op2) + 1);				/* op2 destination address */
}

/*	none	ffffff		yyyy			[ext]			[ext]	*/

nfyy()
{
	oyz(16, &op1);								/* 16 bits imd data */
	return (efghj('f', 4, &op2) + 1);				/* op2 destination address */
}

/*	none	ffffff		yyyy			zzzz			[ext]		[ext] */

nfyz()
{
	oyz(32, &op1);								/* 32 bits imd data */
	return (efghj('f', 6, &op2) + 2);				/* op2 destination address */
}

/*	none	hhhhhh		mmmm			[ext]			[ext]	*/

nhmm()
{
	mmkk('>', &op1);							/* reglist from op1 into ext */
	return (efghj('h', 4, &op2) + 1);				/* op2 destination address */
}

/*	none	jjjjjj		[ext]			[ext]	*/

nj()
{
	return efghj('j', 2, &op1);
}

/*	none	jjjjjj		mmmm			[ext]			[ext]	*/

njmm()
{
	mmkk('>', &op2);					/* reglist from op2 into ext */
	return (efghj('j', 4, &op1) + 1);
}

/*	none 	rrr	*/

nr()
{
	return rsd(1, &op1);				/* source reg from op1 into byte 1 */
}

nr2()
{
	return rsd(1, &op2);				/* source reg from op2 into byte 1 */
}

/*	none	rrr			xxxx	*/

nrxx()
{
	rsd(1, &op1);						/* source reg from op1 into byte 1 */
	return xxxx(16, &op2);				/* 16 bit displ from op1 */
}

/*	none	sss	[same as 'none rrr':nr()] */

/*	none	sss			mmmm	*/

nsmm()
{
	rsd(1, &op1);						/* source reg from op1 into byte 1 */
	return mmkk('>', &op2);				/* reglist from op1 into ext */
}

/*	none	vvvv	*/

nv()
{
	if (op1._data & ~15l) return ERR_V;
	code[1] |= op1._data;
	return NULL;
}

/*	bbb		ddd	*/
	/* same as ccc	ddd (sort of) */

/*	bbb		ffffff		[ext]			[ext]	*/
	/* bbb is a misnomer for these two... */

cf()
{
	ccc(&op1);
	return efghj('f', 2, &op2);
}

/*	ccc		ddd	*/

cd()
{
	ccc(&op1);
	return rsd(1, &op2);
}
/*	ddd		none	zero	yy	*/

dnoy()
{
	rsd(0, &op2);
	return oyz(8, &op1);					/* 8 bits of immediate data */
}

/*	ddd		none		yyyy	*/

dnyy()
{
	rsd(0, &op2);
	return oyz(16, &op1);					/* 16 bits of immediate data */
}

/*	ddd		none		yyyy			zzzz	*/

dnyz()
{
	rsd(0, &op2);
	return oyz(32, &op1);					/* 32 bits of immediate data */
}

/*	ddd		eeeeee		[ext]			[ext]	*/

de()
{
	rsd(0, &op2);							/* dest reg from op2 into byte 1 */
	return efghj('e', 2, &op1);
}

/*	ddd		jjjjjj		[ext]			[ext]	*/

dj()
{
	rsd(0, &op2);
	return efghj('j', 2, &op1);
}

/*	ddd		rrr	*/

dr()
{
	rsd(0, &op2);
	return rsd(1, &op1);				/* source reg from op1 into byte 1 */
}

/*	ddd		sss	[same as 'ddd rrr': dr()] */

/*	ddd		sss			xxxx	*/

dsxx()
{
	rsd(0, &op2);
	rsd(1, &op1);					/* source reg from op1 into byte 1 */
	return xxxx(16, &op1);				/* 16 bit displ from op1 */
}

/*	ddd		yy	*/

dy()
{
	rsd(0, &op2);
	return yy(&op1);
}

/*	gggggg	none	zero	yy			[ext]			[ext]	*/

gnoy()
{
	oyz(8, &op1);					/* 8 bits of immediate data */
	return (efghj('g', 4, &op2) + 1);
}

/*	gggggg	none		yyyy			[ext]			[ext]	*/

gnyy()
{
	oyz(16, &op1);					/* 16 bits of immediate data */
	return (efghj('g', 4, &op2) + 1);
}

/*	gggggg	none		yyyy			zzzz			[ext]		[ext]	*/

gnyz()
{
	oyz(32, &op1);					/* 32 bits of immediate data */
	return (efghj('g', 6, &op2) + 2);
}

/*	gggggg	eeeeee		[exts]			[exts]			[extd]		[extd]	*/

ge()
{
	int temp;
	temp = efghj('e', 2, &op1);
	return (efghj('g', 2 + (temp * 2), &op2) + temp);
}

/*	gggggg	sss			[ext]			[ext]	*/

gs()
{
	rsd(1, &op1);
	return efghj('g', 2, &op2);
}

/*	rrr		ddd	*/

rd()
{
	rsd(0, &op1);
	return rsd(1, &op2);
}

/*	rrr		ffffff		[ext]			[ext]	*/

rf()
{
	rsd(0, &op1);
	return efghj('f', 2, &op2);
}

/*	sss		ddd	[same as 'rrr ddd': rd()] */

/*	sss		ddd			xxxx	*/

sdxx()
{
	rsd(0, &op1);						/* source reg from op1 into byte 1 */
	rsd(1, &op2);
	return xxxx(16, &op2);				/* 16 bit displ from op2 */
}

/*	sss		ffffff		[ext]			[ext] */

sf()
{
	rsd(0, &op1);						/* source reg from op1 into byte 0 */
	return efghj('f', 2, &op2);
}

/* endcode */
16 bits of immediate data */
	return (efghj('g', 