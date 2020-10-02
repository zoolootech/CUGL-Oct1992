/******************************************************************
	mnemonic tables for as68, a 68000 assembler...
*/

/*		(C) Copyright 1982 Steve Passe		*/
/*		All Rights Reserved					*/

/* version 1.00 */
/* created 11/2/82 */

/* version 1.01

	8/30/83	ver. 1.01 modified for Aztec ver. 1.05g	smp

*/

/* begincode */

#include "b:as68.h"
#define NULL 0

/* def of mtable size, 'MTSIZE'(205), is now in as68.h */

struct _mtable mtable[MTSIZE] = {
	"!dummy",		0,		1,
	"abcd",			1,		2,
	"add",			18,		10,
	"add.b",		3,		5,
	"add.l",		8,		10,
	"add.w",		18,		10,
	"addq",			33,		3,
	"addq.b",		28,		2,
	"addq.l",		30,		3,
	"addq.w",		33,		3,
	"addx",			40,		2,
	"addx.b",		36,		2,
	"addx.l",		38,		2,
	"addx.w",		40,		2,
	"and",			53,		6,
	"and.b",		42,		6,
	"and.l",		48,		5,
	"and.w",		53,		6,
	"asl",			63,		3,
	"asl.b",		59,		2,
	"asl.l",		61,		2,
	"asl.w",		63,		3,
	"asr",			70,		3,
	"asr.b",		66,		2,
	"asr.l",		68,		2,
	"asr.w",		70,		3,
	"bcc",			73,		1,
	"bcc.s",		74,		1,
	"bchg",			75,		4,
	"bclr",			79,		4,
	"bcs",			83,		1,
	"bcs.s",		84,		1,
	"beq",			85,		1,
	"beq.s",		86,		1,
	"bge",			87,		1,
	"bge.s",		88,		1,
	"bgt",			89,		1,
	"bgt.s",		90,		1,
	"bhi",			91,		1,
	"bhi.s",		92,		1,
	"ble",			93,		1,
	"ble.s",		94,		1,
	"bls",			95,		1,
	"bls.s",		96,		1,
	"blt",			97,		1,
	"blt.s",		98,		1,
	"bmi",			99,		1,
	"bmi.s",		100,	1,
	"bne",			101,	1,
	"bne.s",		102,	1,
	"bpl",			103,	1,
	"bpl.s",		104,	1,
	"bra",			105,	1,
	"bra.s",		106,	1,
	"bset",			107,	4,
	"bsr",			111,	1,
	"bsr.s",		112,	1,
	"btst",			113,	4,
	"bvc",			117,	1,
	"bvc.s",		118,	1,
	"bvs",			119,	1,
	"bvs.s",		120,	1,
	"chk",			121,	3,
	"clr",			128,	2,
	"clr.b",		124,	2,
	"clr.l",		126,	2,
	"clr.w",		128,	2,
	"cmp",			143,	9,
	"cmp.b",		130,	4,
	"cmp.l",		134,	9,
	"cmp.w",		143,	9,
	"cmpm",			154,	1,
	"cmpm.b",		152,	1,
	"cmpm.l",		153,	1,
	"cmpm.w",		154,	1,
	"dbcc",			155,	1,
	"dbcs",			156,	1,
	"dbeq",			157,	1,
	"dbf",			158,	1,
	"dbge",			159,	1,
	"dbgt",			160,	1,
	"dbhi",			161,	1,
	"dble",			162,	1,
	"dbls",			163,	1,
	"dblt",			164,	1,
	"dbmi",			165,	1,
	"dbne",			166,	1,
	"dbpl",			167,	1,
	"dbra",			168,	1,
	"dbt",			169,	1,
	"dbvc",			170,	1,
	"dbvs",			171,	1,
	"divs",			172,	3,
	"divu",			175,	3,
	"eor",			187,	5,
	"eor.b",		178,	5,
	"eor.l",		183,	4,
	"eor.w",		187,	5,
	"exg",			192,	4,
	"ext",			197,	1,
	"ext.l",		196,	1,
	"ext.w",		197,	1,
	"jmp",			198,	1,
	"jsr",			199,	1,
	"lea",			200,	1,
	"link",			201,	1,
	"lsl",			206,	3,
	"lsl.b",		202,	2,
	"lsl.l",		204,	2,
	"lsl.w",		206,	3,
	"lsr",			213,	3,
	"lsr.b",		209,	2,
	"lsr.l",		211,	2,
	"lsr.w",		213,	3,
	"move",			236,	20,
	"move.b",		216,	6,
	"move.l",		222,	14,
	"move.w",		236,	20,
	"movem",		260,	4,
	"movem.l",		256,	4,
	"movem.w",		260,	4,
	"movep",		266,	2,
	"movep.l",		264,	2,
	"movep.w",		266,	2,
	"moveq",		268,	1,
	"muls",			269,	3,
	"mulu",			272,	3,
	"nbcd",			275,	2,
	"neg",			281,	2,
	"neg.b",		277,	2,
	"neg.l",		279,	2,
	"neg.w",		281,	2,
	"negx",			287,	2,
	"negx.b",		283,	2,
	"negx.l",		285,	2,
	"negx.w",		287,	2,
	"nop",			289,	1,
	"not",			294,	2,
	"not.b",		290,	2,
	"not.l",		292,	2,
	"not.w",		294,	2,
	"or",			307,	6,
	"or.b",			296,	6,
	"or.l",			302,	5,
	"or.w",			307,	6,
	"pea",			313,	1,
	"reset",		314,	1,
	"rol",			319,	3,
	"rol.b",		315,	2,
	"rol.l",		317,	2,
	"rol.w",		319,	3,
	"ror",			326,	3,
	"ror.b",		322,	2,
	"ror.l",		324,	2,
	"ror.w",		326,	3,
	"roxl",			333,	3,
	"roxl.b",		329,	2,
	"roxl.l",		331,	2,
	"roxl.w",		333,	3,
	"roxr",			340,	3,
	"roxr.b",		336,	2,
	"roxr.l",		338,	2,
	"roxr.w",		340,	3,
	"rte",			343,	1,
	"rtr",			344,	1,
	"rts",			345,	1,
	"sbcd",			346,	2,
	"scc",			348,	2,
	"scs",			350,	2,
	"seq",			352,	2,
	"sf",			354,	2,
	"sge",			356,	2,
	"sgt",			358,	2,
	"shi",			360,	2,
	"sle",			362,	2,
	"sls",			364,	2,
	"slt",			366,	2,
	"smi",			368,	2,
	"sne",			370,	2,
	"spl",			372,	2,
	"st",			374,	2,
	"stop",			376,	1,
	"sub",			392,	10,
	"sub.b",		377,	5,
	"sub.l",		382,	10,
	"sub.w",		392,	10,
	"subq",			407,	3,
	"subq.b",		402,	2,
	"subq.l",		404,	3,
	"subq.w",		407,	3,
	"subx",			414,	2,
	"subx.b",		410,	2,
	"subx.l",		412,	2,
	"subx.w",		414,	2,
	"svc",			416,	2,
	"svs",			418,	2,
	"swap",			420,	1,
	"tas",			421,	2,
	"trap",			423,	1,
	"trapv",		424,	1,
	"tst",			429,	2,
	"tst.b",		425,	2,
	"tst.l",		427,	2,
	"tst.w",		429,	2,
	"unlk",			431,	1
};

int nn(), nnoy(), nnyy(), nx(), nnxx(), nd(), nd2(), ndob(), ndkk();
int ndoy(), ndyy(), ndyz(), ne(), nf(), nf2(), nfob(), nfoy(), nfyy(), nfyz();
int nhmm(), nj(), njmm(), nr(), nr2(), nrxx(), nsmm(), nv(), cd(), cf();
int cd(), dnoy(), dnyy(), dnyz(), de(), dj(), dr(), dsxx(), dy();
int gnoy(), gnyy(), gnyz(), ge(), gs(), rd(), rf(), sdxx(), sf();

struct _mvalue mvalue[] = {
				NULL,		NULL,		NULL,	NULL,	NULL,	NULL,
/* ABCD */
				_pd_ani,	_pd_ani,	0xc1,	0x08,	1,		dr,
				_dn,		_dn,		0xc1,	0x00, 	1,		dr,
/* ADD.B */
				_imd,		_dadr,		0x06,	0x00,	12,		nfoy,
				_imd,		_dn,		0x06,	0x00,	2,		ndoy,
				_sadr,		_dn,		0xd0,	0x00,	0,		de,
				_dn,		_dadr,		0xd1,	0x00,	0,		sf,
				_dn,		_dn,		0xd0,	0x00,	1,		dr,
/* ADD.L */
				_imd,		_an,		0xd1,	0xfc,	3,		dnyz,
				_imd,		_dadr,		0x06,	0x80,	13,		nfyz,
				_imd,		_dn,		0x06,	0x80,	3,		ndyz,
				_an,		_an,		0xd1,	0xc8,	1,		dr,
				_an,		_dn,		0xd0,	0x88,	1,		dr,
				_dn,		_an,		0xd1,	0xc0,	1,		dr,
				_dn,		_dn,		0xd0,	0x80,	1,		dr,
				_dn,		_dadr,		0xd1,	0x80,	0,		sf,
				_sadr,		_an,		0xd1,	0xc0,	0,		de,
				_sadr,		_dn,		0xd0,	0x80,	0,		de,
/* ADD.W */
				_imd,		_an,		0xd0,	0xfc,	2,		dnyy,
				_imd,		_dadr,		0x06,	0x40,	12,		nfyy,
				_imd,		_dn,		0x06,	0x40,	2,		ndyy,
				_an,		_an,		0xd0,	0xc8,	1,		dr,
				_an,		_dn,		0xd0,	0x48,	1,		dr,
				_dn,		_an,		0xd0,	0xc0,	1,		dr,
				_dn,		_dn,		0xd0,	0x40,	1,		dr,
				_dn,		_dadr,		0xd1,	0x40,	0,		sf,
				_sadr,		_an,		0xd0,	0xc0,	0,		de,
				_sadr,		_dn,		0xd0,	0x40,	0,		de,
/* ADDQ.B */
				_imd,		_dadr,		0x50,	0x00,	0,		cf,
				_imd,		_dn,		0x50,	0x00,	1,		cd,
/* ADDQ.L */
				_imd,		_an,		0x50,	0x88,	1,		cd,
				_imd,		_dadr,		0x50,	0x80,	0,		cf,
				_imd,		_dn,		0x50,	0x80,	1,		cd,
/* ADDQ.W */
				_imd,		_an,		0x50,	0x48,	1,		cd,
				_imd,		_dadr,		0x50,	0x40,	0,		cf,
				_imd,		_dn,		0x50,	0x40,	1,		cd,
/* ADDX.B */
				_pd_ani,	_pd_ani,	0xd1,	0x08,	1,		dr,
				_dn,		_dn,		0xd1,	0x00,	1,		dr,
/* ADDX.L */
				_pd_ani,	_pd_ani,	0xd1,	0x88,	1,		dr,
				_dn,		_dn,		0xd1,	0x80,	1,		dr,
/* ADDX.W */
				_pd_ani,	_pd_ani,	0xd1,	0x48,	1,		dr,
				_dn,		_dn,		0xd1,	0x40,	1,		dr,
/* AND.B */
				_imd,		_dadr,		0x02,	0x00,	12,		nfoy,
				_imd,		_dn,		0x02,	0x00,	2,		ndoy,
				_imd,		_sr,		0x02,	0x3c,	2,		nnoy,
				_sadr,		_dn,		0xc0,	0x00,	0,		de,
				_dn,		_dadr,		0xc1,	0x00,	0,		sf,
				_dn,		_dn,		0xc0,	0x00,	1,		dr,
/* AND.L */
				_imd,		_dadr,		0x02,	0x80,	12,		nfyz,
				_imd,		_dn,		0x02,	0x80,	3,		ndyz,
				_sadr,		_dn,		0xc0,	0x80,	0,		de,
				_dn,		_dadr,		0xc1,	0x80,	0,		sf,
				_dn,		_dn,		0xc0,	0x80,	1,		dr,
/* AND.W */
				_imd,		_dadr,		0x02,	0x40,	12,		nfyy,
				_imd,		_dn,		0x02,	0x40,	2,		ndyy,
				_imd,		_sr,		0x02,	0x7c,	2,		nnyy,
				_sadr,		_dn,		0xc0,	0x40,	0,		de,
				_dn,		_dadr,		0xc1,	0x40,	0,		sf,
				_dn,		_dn,		0xc0,	0x40,	1,		dr,
/* ASL.B */
				_imd,		_dn,		0xe1,	0x00,	1,		cd,
				_dn,		_dn,		0xe1,	0x20,	1,		rd,
/* ASL.L */
				_imd,		_dn,		0xe1,	0x80,	1,		cd,
				_dn,		_dn,		0xe1,	0xa0,	1,		rd,
/* ASL.W */
				_imd,		_dn,		0xe1,	0x40,	1,		cd,
				_dn,		_dn,		0xe1,	0x60,	1,		rd,
/* ASL */
				_dadr,		_none,		0xe1,	0xc0,	0,		nf,
/* ASR.B */
				_imd,		_dn,		0xe0,	0x00,	1,		cd,
				_dn,		_dn,		0xe0,	0x20,	1,		rd,
/* ASR.L */
				_imd,		_dn,		0xe0,	0x80,	1,		cd,
				_dn,		_dn,		0xe0,	0xa0,	1,		rd,
/* ASR.W */
				_imd,		_dn,		0xe0,	0x40,	1,		cd,
				_dn,		_dn,		0xe0,	0x60,	1,		rd,
/* ASR */
				_dadr,		_none,		0xe0,	0xc0,	0,		nf,
/* BCC */
				_address,	_none,		0x64,	0x00,	2,		nnxx,
/* BCC.S */
				_address,	_none,		0x64,	0x00,	1,		nx,
/* BCHG */
				_imd,		_dadr,		0x08,	0x40,	12,		nfob,
				_imd,		_dn,		0x08,	0x40,	2,		ndob,
				_dn,		_dadr,		0x01,	0x40,	0,		rf,
				_dn,		_dn,		0x01,	0x40,	1,		rd,
/* BCLR */
				_imd,		_dadr,		0x08,	0x80,	12,		nfob,
				_imd,		_dn,		0x08,	0x80,	2,		ndob,
				_dn,		_dadr,		0x01,	0x80,	0,		rf,
				_dn,		_dn,		0x01,	0x80,	1,		rd,
/* BCS */
				_address,	_none,		0x65,	0x00,	2,		nnxx,
/* BCS.S */
				_address,	_none,		0x65,	0x00,	1,		nx,
/* BEQ */
				_address,	_none,		0x67,	0x00,	2,		nnxx,
/* BEQ.S */
				_address,	_none,		0x67,	0x00,	1,		nx,
/* BGE */
				_address,	_none,		0x6c,	0x00,	2,		nnxx,
/* BGE.S */
				_address,	_none,		0x6c,	0x00,	1,		nx,
/* BGT */
				_address,	_none,		0x6e,	0x00,	2,		nnxx,
/* BGT.S */
				_address,	_none,		0x6e,	0x00,	1,		nx,
/* BHI */
				_address,	_none,		0x62,	0x00,	2,		nnxx,
/* BHI.S */
				_address,	_none,		0x62,	0x00,	1,		nx,
/* BLE */
				_address,	_none,		0x6f,	0x00,	2,		nnxx,
/* BLE.S */
				_address,	_none,		0x6f,	0x00,	1,		nx,
/* BLS */
				_address,	_none,		0x63,	0x00,	2,		nnxx,
/* BLS.S */
				_address,	_none,		0x63,	0x00,	1,		nx,
/* BLT */
				_address,	_none,		0x6d,	0x00,	2,		nnxx,
/* BLT.S */
				_address,	_none,		0x6d,	0x00,	1,		nx,
/* BMI */
				_address,	_none,		0x6b,	0x00,	2,		nnxx,
/* BMI.S */
				_address,	_none,		0x6b,	0x00,	1,		nx,
/* BNE */
				_address,	_none,		0x66,	0x00,	2,		nnxx,
/* BNE.S */
				_address,	_none,		0x66,	0x00,	1,		nx,
/* BPL */
				_address,	_none,		0x6a,	0x00,	2,		nnxx,
/* BPL.S */
				_address,	_none,		0x6a,	0x00,	1,		nx,
/* BRA */
				_address,	_none,		0x60,	0x00,	2,		nnxx,
/* BRA.S */
				_address,	_none,		0x60,	0x00,	1,		nx,
/* BSET */
				_imd,		_dadr,		0x08,	0xc0,	12,		nfob,
				_imd,		_dn,		0x08,	0xc0,	2,		ndob,
				_dn,		_dadr,		0x01,	0xc0,	0,		rf,
				_dn,		_dn,		0x01,	0xc0,	1,		rd,
/* BSR */
				_address,	_none,		0x61,	0x00,	2,		nnxx,
/* BSR.S */
				_address,	_none,		0x61,	0x00,	1,		nx,
/* BTST */
				_imd,		_dadr,		0x08,	0x00,	12,		nfob,
				_imd,		_dn,		0x08,	0x00,	2,		ndob,
				_dn,		_dadr,		0x01,	0x00,	0,		rf,
				_dn,		_dn,		0x01,	0x00,	1,		rd,
/* BVC */
				_address,	_none,		0x68,	0x00,	2,		nnxx,
/* BVC.S */
				_address,	_none,		0x68,	0x00,	1,		nx,
/* BVS */
				_address,	_none,		0x69,	0x00,	2,		nnxx,
/* BVS.S */
				_address,	_none,		0x69,	0x00,	1,		nx,
/* CHK */
				_imd,		_dn,		0x41,	0xbc,	2,		dnyy,
				_dn,		_dn,		0x41,	0x80,	1,		dr,
				_sadr,		_dn,		0x41,	0x80,	0,		de,
/* CLR.B */
				_dadr,		_none,		0x42,	0x00,	0,		nf,
				_dn,		_none,		0x42,	0x00,	1,		nd,
/* CLR.L */
				_dadr,		_none,		0x42,	0x80,	0,		nf,
				_dn,		_none,		0x42,	0x80,	1,		nd,
/* CLR.W */
				_dadr,		_none,		0x42,	0x40,	0,		nf,
				_dn,		_none,		0x42,	0x40,	1,		nd,
/* CMP.B */
				_imd,		_dadr,		0x0c,	0x00,	12,		nfoy,
				_imd,		_dn,		0x0c,	0x00,	2,		ndoy,
				_sadr,		_dn,		0xb0,	0x00,	0,		de,
				_dn,		_dn,		0xb0,	0x00,	1,		dr,
/* CMP.L */
				_imd,		_an,		0xb1,	0xfc,	3,		dnyz,
				_imd,		_dadr,		0x0c,	0x80,	13,		nfyz,
				_imd,		_dn,		0x0c,	0x80,	3,		ndyz,
				_an,		_an,		0xb1,	0xc8,	1,		dr,
				_an,		_dn,		0xb0,	0x88,	1,		dr,
				_dn,		_an,		0xb1,	0xc0,	1,		dr,
				_dn,		_dn,		0xb0,	0x80,	1,		dr,
				_sadr,		_an,		0xb1,	0xc0,	0,		de,
				_sadr,		_dn,		0xb0,	0x80,	0,		de,
/* CMP.W */
				_imd,		_an,		0xb0,	0xfc,	2,		dnyy,
				_imd,		_dadr,		0x0c,	0x40,	12,		nfyy,
				_imd,		_dn,		0x0c,	0x40,	2,		ndyy,
				_an,		_an,		0xb0,	0xc8,	1,		dr,
				_an,		_dn,		0xb0,	0x48,	1,		dr,
				_dn,		_an,		0xb0,	0xc0,	1,		dr,
				_dn,		_dn,		0xb0,	0x40,	1,		dr,
				_sadr,		_an,		0xb0,	0xc0,	0,		de,
				_sadr,		_dn,		0xb0,	0x40,	0,		de,
/* CMPM.B */
				_ani_pi,	_ani_pi,	0xb1,	0x08,	1,		dr,
/* CMPM.L */
				_ani_pi,	_ani_pi,	0xb1,	0x88,	1,		dr,
/* CMPM.W */
				_ani_pi,	_ani_pi,	0xb1,	0x48,	1,		dr,
/* DBCC */
				_dn,		_address,	0x54,	0xc8,	2,		nrxx,
/* DBCS */
				_dn,		_address,	0x55,	0xc8,	2,		nrxx,
/* DBEQ */
				_dn,		_address,	0x57,	0xc8,	2,		nrxx,
/* DBF */
				_dn,		_address,	0x51,	0xc8,	2,		nrxx,
/* DBGE */
				_dn,		_address,	0x5c,	0xc8,	2,		nrxx,
/* DBGT */
				_dn,		_address,	0x5e,	0xc8,	2,		nrxx,
/* DBHI */
				_dn,		_address,	0x52,	0xc8,	2,		nrxx,
/* DBLE */
				_dn,		_address,	0x5f,	0xc8,	2,		nrxx,
/* DBLS */
				_dn,		_address,	0x53,	0xc8,	2,		nrxx,
/* DBLT */
				_dn,		_address,	0x5d,	0xc8,	2,		nrxx,
/* DBMI */
				_dn,		_address,	0x5b,	0xc8,	2,		nrxx,
/* DBNE */
				_dn,		_address,	0x56,	0xc8,	2,		nrxx,
/* DBPL */
				_dn,		_address,	0x5a,	0xc8,	2,		nrxx,
/* DBRA */
				_dn,		_address,	0x51,	0xc8,	2,		nrxx,
/* DBT */
				_dn,		_address,	0x50,	0xc8,	2,		nrxx,
/* DBVC */
				_dn,		_address,	0x58,	0xc8,	2,		nrxx,
/* DBVS */
				_dn,		_address,	0x59,	0xc8,	2,		nrxx,
/* DIVS */
				_imd,		_dn,		0x81,	0xfc,	2,		dnyy,
				_sadr,		_dn,		0x81,	0xc0,	0,		de,
				_dn,		_dn,		0x81,	0xc0,	1,		dr,
/* DIVU */
				_imd,		_dn,		0x80,	0xfc,	2,		dnyy,
				_sadr,		_dn,		0x80,	0xc0,	0,		de,
				_dn,		_dn,		0x80,	0xc0,	1,		dr,
/* EOR.B */
				_imd,		_dadr,		0x0a,	0x00,	12,		nfoy,
				_imd,		_dn,		0x0a,	0x00,	2,		ndoy,
				_imd,		_sr,		0x0a,	0x3c,	2,		nnoy,
				_dn,		_dadr,		0xb1,	0x00,	0,		sf,
				_dn,		_dn,		0xb1,	0x00,	1,		rd,
/* EOR.L */
				_imd,		_dadr,		0x0a,	0x80,	13,		nfyz,
				_imd,		_dn,		0x0a,	0x80,	3,		ndyz,
				_dn,		_dadr,		0xb1,	0x80,	0,		sf,
				_dn,		_dn,		0xb1,	0x80,	1,		rd,
/* EOR.W */
				_imd,		_dadr,		0x0a,	0x40,	12,		nfyy,
				_imd,		_dn,		0x0a,	0x40,	2,		ndyy,
				_imd,		_sr,		0x0a,	0x7c,	2,		nnyy,
				_dn,		_dadr,		0xb1,	0x40,	0,		sf,
				_dn,		_dn,		0xb1,	0x40,	1,		rd,
/* EXG */
				_an,		_an,		0xc1,	0x48,	1,		rd,
				_an,		_dn,		0xc1,	0x88,	1,		rd,
				_dn,		_an,		0xc1,	0x88,	1,		rd,
				_dn,		_dn,		0xc1,	0x40,	1,		rd,
/* EXT.L */
				_dn,		_none,		0x48,	0xc0,	1,		nd,
/* EXT.W */
				_dn,		_none,		0x48,	0x80,	1,		nd,
/* JMP */
				_jadr,		_none,		0x4e,	0xc0, 	0,		nj,
/* JSR */
				_jadr,		_none,		0x4e,	0x80,	0,		nj,
/* LEA */
				_jadr,		_an,		0x41,	0xc0,	0,		dj,
/* LINK */
				_an,		_imd,		0x4e,	0x50,	2,		nrxx,
/* LSL.B */
				_imd,		_dn,		0xe1,	0x08,	1,		cd,
				_dn,		_dn,		0xe1,	0x28,	1,		rd,
/* LSL.L */
				_imd,		_dn,		0xe1,	0x88,	1,		cd,
				_dn,		_dn,		0xe1,	0xa8,	1,		rd,
/* LSL.W */
				_imd,		_dn,		0xe1,	0x48,	1,		cd,
				_dn,		_dn,		0xe1,	0x68,	1,		rd,
/* LSL */
				_dadr,		_none,		0xe3,	0xc0,	0,		nf,
/* LSR.B */
				_imd,		_dn,		0xe0,	0x08,	1,		cd,
				_dn,		_dn,		0xe0,	0x28,	1,		rd,
/* LSR.L */
				_imd,		_dn,		0xe0,	0x88,	1,		cd,
				_dn,		_dn,		0xe0,	0xa8,	1,		rd,
/* LSR.W */
				_imd,		_dn,		0xe0,	0x48,	1,		cd,
				_dn,		_dn,		0xe0,	0x68,	1,		rd,
/* LSR */
				_dadr,		_none,		0xe2,	0xc0,	0,		nf,
/* MOVE.B */
				_imd,		_dn,		0x10,	0x3c,	2,		dnoy,
				_imd,		_dadr,		0x10,	0x3c,	12,		gnoy,
				_dn,		_dadr,		0x10,	0x00,	0,		gs,
				_dn,		_dn,		0x10,	0x00,	1,		dr,
				_sadr,		_dadr,		0x10,	0x00,	0,		ge,
				_sadr,		_dn,		0x10,	0x00,	0,		de,
/* MOVE.L */
				_imd,		_an,		0x20,	0x7c,	3,		dnyz,
				_imd,		_dadr,		0x20,	0x3c,	13,		gnyz,
				_imd,		_dn,		0x20,	0x3c,	3,		dnyz,
				_an,		_an,		0x20,	0x48,	1,		dr,
				_an,		_dn,		0x20,	0x08,	1,		dr,
				_an,		_dadr,		0x20,	0x08,	0,		gs,
				_an,		_usp,		0x4e,	0x60,	1,		nr,
				_dn,		_an,		0x20,	0x40,	1,		dr,
				_dn,		_dn,		0x20,	0x00,	1,		dr,
				_dn,		_dadr,		0x20,	0x00,	0,		gs,
				_sadr,		_an,		0x20,	0x40,	0,		de,
				_sadr,		_dadr,		0x20,	0x00,	0,		ge,
				_sadr,		_dn,		0x20,	0x00,	0,		de,
				_usp,		_an,		0x4e,	0x68,	1,		nr2,
/* MOVE.W */
				_imd,		_ccr,		0x44,	0xfc,	2,		nnoy,
				_imd,		_sr,		0x46,	0xfc,	2,		nnyy,
				_imd,		_an,		0x30,	0x7c,	2,		dnyy,
				_imd,		_dadr,		0x30,	0x3c,	12,		gnyy,
				_imd,		_dn,		0x30,	0x3c,	2,		dnyy,
				_an,		_an,		0x30,	0x48,	1,		dr,
				_an,		_dn,		0x30,	0x08,	1,		dr,
				_an,		_dadr,		0x30,	0x08,	0,		gs,
				_dn,		_ccr,		0x44,	0xc0,	1,		nr,
				_dn,		_sr,		0x46,	0xc0,	1,		nr,
				_dn,		_an,		0x30,	0x40,	1,		dr,
				_dn,		_dn,		0x30,	0x00,	1,		dr,
				_dn,		_dadr,		0x30,	0x00,	0,		gs,
				_sadr,		_ccr,		0x44,	0xc0,	0,		ne,
				_sadr,		_sr,		0x46,	0xc0,	0,		ne,
				_sadr,		_an,		0x30,	0x40,	0,		de,
				_sadr,		_dadr,		0x30,	0x00,	0,		ge,
				_sadr,		_dn,		0x30,	0x00,	0,		de,
				_sr,		_dadr,		0x40,	0xc0,	0,		nf2,
				_sr,		_dn,		0x40,	0xc0,	1,		nd2,
/* MOVEM.L */
				_ani_pi,	_reglst,	0x4c,	0xd8,	2,		nsmm,
				_jadr,		_reglst,	0x4c,	0xc0,	0,		njmm,
				_reglst,	_pd_ani,	0x48,	0xe0,	2,		ndkk,
				_reglst,	_madr,		0x48,	0xc0,	0,		nhmm,
/* MOVEM.W */
				_ani_pi,	_reglst,	0x4c,	0x98,	2,		nsmm,
				_jadr,		_reglst,	0x4c,	0x80,	0,		njmm,
				_reglst,	_pd_ani,	0x48,	0xa0,	2,		ndkk,
				_reglst,	_madr,		0x48,	0x80,	0,		nhmm,
/* MOVEP.L */
				_d16_ani,	_dn,		0x01,	0x48,	2,		dsxx,
				_dn,		_d16_ani,	0x01,	0xc8,	2,		sdxx,
/* MOVEP.W */
				_d16_ani,	_dn,		0x01,	0x08,	2,		dsxx,
				_dn,		_d16_ani,	0x01,	0x88,	2,		sdxx,
/* MOVEQ */
				_imd,		_dn,		0x70,	0x00,	1,		dy,
/* MULS */
				_imd,		_dn,		0xc1,	0xfc,	2,		dnyy,
				_sadr,		_dn,		0xc1,	0xc0,	0,		de,
				_dn,		_dn,		0xc1,	0xc0,	1,		dr,
/* MULU */
				_imd,		_dn,		0xc0,	0xfc,	2,		dnyy,
				_sadr,		_dn,		0xc0,	0xc0,	0,		de,
				_dn,		_dn,		0xc0,	0xc0,	1,		dr,
/* NBCD */
				_dadr,		_none,		0x48,	0x00,	0,		nf,
				_dn,		_none,		0x48,	0x00,	1,		nd,
/* NEG.B */
				_dadr,		_none,		0x44,	0x00,	0,		nf,
				_dn,		_none,		0x44,	0x00,	1,		nd,
/* NEG.L */
				_dadr,		_none,		0x44,	0x80,	0,		nf,
				_dn,		_none,		0x44,	0x80,	1,		nd,
/* NEG.W */
				_dadr,		_none,		0x44,	0x40,	0,		nf,
				_dn,		_none,		0x44,	0x40,	1,		nd,
/* NEGX.B */
				_dadr,		_none,		0x40,	0x00,	0,		nf,
				_dn,		_none,		0x40,	0x00,	1,		nd,
/* NEGX.L */
				_dadr,		_none,		0x40,	0x80,	0,		nf,
				_dn,		_none,		0x40,	0x80,	1,		nd,
/* NEGX.W */
				_dadr,		_none,		0x40,	0x40,	0,		nf,
				_dn,		_none,		0x40,	0x40,	1,		nd,
/* NOP */
				_none,		_none,		0x4e,	0x71,	1,		nn,
/* NOT.B */
				_dadr,		_none,		0x46,	0x00,	0,		nf,
				_dn,		_none,		0x46,	0x00,	1,		nd,
/* NOT.L */
				_dadr,		_none,		0x46,	0x80,	0,		nf,
				_dn,		_none,		0x46,	0x80,	1,		nd,
/* NOT.W */
				_dadr,		_none,		0x46,	0x40,	0,		nf,
				_dn,		_none,		0x46,	0x40,	1,		nd,
/* OR.B */
				_imd,		_dadr,		0x00,	0x00,	12,		nfoy,
				_imd,		_dn,		0x00,	0x00,	2,		ndoy,
				_imd,		_sr,		0x00,	0x3c,	2,		nnoy,
				_sadr,		_dn,		0x80,	0x00,	0,		de,
				_dn,		_dadr,		0x81,	0x00,	0,		sf,
				_dn,		_dn,		0x80,	0x00,	1,		dr,
/* OR.L */
				_imd,		_dadr,		0x00,	0x80,	13,		nfyz,
				_imd,		_dn,		0x00,	0x80,	3,		ndyz,
				_sadr,		_dn,		0x80,	0x80,	0,		de,
				_dn,		_dadr,		0x81,	0x80,	0,		sf,
				_dn,		_dn,		0x80,	0x80,	1,		dr,
/* OR.W */
				_imd,		_dadr,		0x00,	0x40,	12,		nfyy,
				_imd,		_dn,		0x00,	0x40,	2,		ndyy,
				_imd,		_sr,		0x00,	0x7c,	2,		nnyy,
				_sadr,		_dn,		0x80,	0x40,	0,		de,
				_dn,		_dadr,		0x81,	0x80,	0,		sf,
				_dn,		_dn,		0x80,	0x40,	1,		dr,
/* PEA */
				_jadr,		_none,		0x48,	0x40,	0,		nj,
/* RESET */
				_none,		_none,		0x4e,	0x70,	1,		nn,
/* ROL.B */
				_imd,		_dn,		0xe1,	0x18,	1,		cd,
				_dn,		_dn,		0xe1,	0x38,	1,		rd,
/* ROL.L */
				_imd,		_dn,		0xe1,	0x98,	1,		cd,
				_dn,		_dn,		0xe1,	0xb8,	1,		rd,
/* ROL.W */
				_imd,		_dn,		0xe1,	0x58,	1,		cd,
				_dn,		_dn,		0xe1,	0x78,	1,		rd,
/* ROL */
				_dadr,		_none,		0xe7,	0xc0,	0,		nf,
/* ROR.B */
				_imd,		_dn,		0xe0,	0x18,	1,		cd,
				_dn,		_dn,		0xe0,	0x38,	1,		rd,
/* ROR.L */
				_imd,		_dn,		0xe0,	0x98,	1,		cd,
				_dn,		_dn,		0xe0,	0xb8,	1,		rd,
/* ROR.W */
				_imd,		_dn,		0xe0,	0x58,	1,		cd,
				_dn,		_dn,		0xe0,	0x78,	1,		rd,
/* ROR */
				_dadr,		_none,		0xe6,	0xc0,	0,		nf,
/* ROXL.B */
				_imd,		_dn,		0xe1,	0x10,	1,		cd,
				_dn,		_dn,		0xe1,	0x30,	1,		rd,
/* ROXL.L */
				_imd,		_dn,		0xe1,	0x90,	1,		cd,
				_dn,		_dn,		0xe1,	0xb0,	1,		rd,
/* ROXL.W */
				_imd,		_dn,		0xe1,	0x50,	1,		cd,
				_dn,		_dn,		0xe1,	0x70,	1,		rd,
/* ROXL */
				_dadr,		_none,		0xe5,	0xc0,	0,		nf,
/* ROXR.B */
				_imd,		_dn,		0xe0,	0x10,	1,		cd,
				_dn,		_dn,		0xe0,	0x30,	1,		rd,
/* ROXR.L */
				_imd,		_dn,		0xe0,	0x40,	1,		cd,
				_dn,		_dn,		0xe0,	0xb0,	1,		rd,
/* ROXR.W */
				_imd,		_dn,		0xe0,	0x50,	1,		cd,
				_dn,		_dn,		0xe0,	0x70,	1,		rd,
/* ROXR */
				_dadr,		_none,		0xe4,	0xc0,	0,		nf,
/* RTE */
				_none,		_none,		0x4e,	0x73,	1,		nn,
/* RTR */
				_none,		_none,		0x4e,	0x77,	1,		nn,
/* RTS */
				_none,		_none,		0x4e,	0x75,	1,		nn,
/* SBCD */
				_pd_ani,	_pd_ani,	0x81,	0x08,	1,		dr,
				_dn,		_dn,		0x81,	0x00,	1,		dr,
/* SCC */
				_dadr,		_none,		0x54,	0xc0,	0,		nf,
				_dn,		_none,		0x54,	0xc0,	1,		nd,
/* SCS */
				_dadr,		_none,		0x55,	0xc0,	0,		nf,
				_dn,		_none,		0x55,	0xc0,	1,		nd,
/* SEQ */
				_dadr,		_none,		0x57,	0xc0,	0,		nf,
				_dn,		_none,		0x57,	0xc0,	1,		nd,
/* SF */
				_dadr,		_none,		0x51,	0xc0,	0,		nf,
				_dn,		_none,		0x51,	0xc0,	1,		nd,
/* SGE */
				_dadr,		_none,		0x5c,	0xc0,	0,		nf,
				_dn,		_none,		0x5c,	0xc0,	1,		nd,
/* SGT */
				_dadr,		_none,		0x5e,	0xc0,	0,		nf,
				_dn,		_none,		0x5e,	0xc0,	1,		nd,
/* SHI */
				_dadr,		_none,		0x52,	0xc0,	0,		nf,
				_dn,		_none,		0x52,	0xc0,	1,		nd,
/* SLE */
				_dadr,		_none,		0x5f,	0xc0,	0,		nf,
				_dn,		_none,		0x5f,	0xc0,	1,		nd,
/* SLS */
				_dadr,		_none,		0x53,	0xc0,	0,		nf,
				_dn,		_none,		0x53,	0xc0,	1,		nd,
/* SLT */
				_dadr,		_none,		0x5d,	0xc0,	0,		nf,
				_dn,		_none,		0x5d,	0xc0,	1,		nd,
/* SMI */
				_dadr,		_none,		0x5b,	0xc0,	0,		nf,
				_dn,		_none,		0x5b,	0xc0,	1,		nd,
/* SNE */
				_dadr,		_none,		0x56,	0xc0,	0,		nf,
				_dn,		_none,		0x56,	0xc0,	1,		nd,
/* SPL */
				_dadr,		_none,		0x5a,	0xc0,	0,		nf,
				_dn,		_none,		0x5a,	0xc0,	1,		nd,
/* ST */
				_dadr,		_none,		0x50,	0xc0,	0,		nf,
				_dn,		_none,		0x50,	0xc0,	1,		nd,
/* STOP */
				_imd,		_none,		0x4e,	0x72,	2,		nnyy,
/* SUB.B */
				_imd,		_dadr,		0x04,	0x00,	12,		nfoy,
				_imd,		_dn,		0x04,	0x00,	2,		ndoy,
				_sadr,		_dn,		0x90,	0x00,	0,		de,
				_dn,		_dadr,		0x91,	0x00,	0,		sf,
				_dn,		_dn,		0x90,	0x00,	1,		dr,
/* SUB.L */
				_imd,		_an,		0x91,	0xfc,	3,		dnyz,
				_imd,		_dadr,		0x04,	0x80,	13,		nfyz,
				_imd,		_dn,		0x04,	0x80,	3,		ndyz,
				_an,		_an,		0x91,	0xc8,	1,		dr,
				_an,		_dn,		0x90,	0x88,	1,		dr,
				_dn,		_an,		0x91,	0xc0,	1,		dr,
				_dn,		_dn,		0x90,	0x80,	1,		dr,
				_dn,		_dadr,		0x91,	0x80,	0,		sf,
				_sadr,		_an,		0x91,	0xc0,	0,		de,
				_sadr,		_dn,		0x90,	0x80,	0,		de,
/* SUB.W */
				_imd,		_an,		0x90,	0xfc,	2,		dnyy,
				_imd,		_dadr,		0x04,	0x40,	12,		nfyy,
				_imd,		_dn,		0x04,	0x40,	2,		ndyy,
				_an,		_an,		0x90,	0xc8,	1,		dr,
				_an,		_dn,		0x90,	0x48,	1,		dr,
				_dn,		_an,		0x90,	0xc0,	1,		dr,
				_dn,		_dn,		0x90,	0x40,	1,		dr,
				_dn,		_dadr,		0x91,	0x40,	0,		sf,
				_sadr,		_an,		0x90,	0xc0,	0,		de,
				_sadr,		_dn,		0x90,	0x40,	0,		de,
/* SUBQ.B */
				_imd,		_dadr,		0x51,	0x00,	0,		cf,
				_imd,		_dn,		0x51,	0x00,	1,		cd,
/* SUBQ.L */
				_imd,		_an,		0x51,	0x88,	1,		cd,
				_imd,		_dadr,		0x51,	0x80,	0,		cf,
				_imd,		_dn,		0x51,	0x80,	1,		cd,
/* SUBQ.W */
				_imd,		_an,		0x51,	0x48,	1,		cd,
				_imd,		_dadr,		0x51,	0x40,	0,		cf,
				_imd,		_dn,		0x51,	0x40,	1,		cd,
/* SUBX.B */
				_pd_ani,	_pd_ani,	0x91,	0x08,	1,		dr,
				_dn,		_dn,		0x91,	0x00,	1,		dr,
/* SUBX.L */
				_pd_ani,	_pd_ani,	0x91,	0x88,	1,		dr,
				_dn,		_dn,		0x91,	0x80,	1,		dr,
/* SUBX.W */
				_pd_ani,	_pd_ani,	0x91,	0x48,	1,		dr,
				_dn,		_dn,		0x91,	0x40,	1,		dr,
/* SVC */
				_dadr,		_none,		0x58,	0xc0,	0,		nf,
				_dn,		_none,		0x58,	0xc0,	1,		nd,
/* SVS */
				_dadr,		_none,		0x59,	0xc0,	0,		nf,
				_dn,		_none,		0x59,	0xc0,	1,		nd,
/* SWAP */
				_dn,		_none,		0x48,	0x40,	1,		nr,
/* TAS */
				_dadr,		_none,		0x4a,	0xc0,	0,		nf,
				_dn,		_none,		0x4a,	0xc0,	1,		nr,
/* TRAP */
				_imd,		_none,		0x4e,	0x40,	1,		nv,
/* TRAPV */
				_none,		_none,		0x4e,	0x76,	1,		nn,
/* TST.B */
				_dadr,		_none,		0x4a,	0x00,	0,		nf,
				_dn,		_none,		0x4a,	0x00,	1,		nr,
/* TST.L */
				_dadr,		_none,		0x4a,	0x80,	0,		nf,
				_dn,		_none,		0x4a,	0x80,	1,		nr,
/* TST.W */
				_dadr,		_none,		0x4a,	0x40,	0,		nf,
				_dn,		_none,		0x4a,	0x40,	1,		nr,
/* UNLK */
				_an,		_none,		0x4e,	0x58,	1,		nr
};
ani,	_pd_ani,	0x91,	0x08,	1,		dr,
				_dn,		_dn,		0x91,	0x00,	1,		dr,
/* SUBX.L */
				_pd_ani,	_pd_an