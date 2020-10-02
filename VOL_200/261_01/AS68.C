/* as68.c, the source of a 68000 assembler.... (root)
 *	(C) Copyright 1982 Steve Passe
 *	All Rights Reserved
 * 
 *  ver. 1.00
 * created 10/7/82
 * 
 * version 1.01
 *
 *   8/30/83 ver. 1.01 modified for Aztec ver. 1.05g smp
 *   12/23/83	fixed 24 bit address bug in flush_rcrd smp
 *   12/24/83	fixed lowercase	a-f in S-records smp
 *
 * Oct-87 srd - fix 'char' - smp assumed chars were UNSIGNED - ha ha.
 *	Also, fix numerous bugs and break into overlays for small
 *	machines (like PDP-11, Whitesmith's compiler in particular)
 *	This version works on both IBM-PC (Microsoft C vers 4.0) and
 *	PDP-11 (TSX+, Whitesmith's RT-11 compiler). Seems solid.
 *	I am beginning to understand how it works...
 *		-Stuart R Dole, SAMKHYA Corp, 707-763-2800
 */
 
/* begincode */
 
/* includes */
 
#include <stdio.h>
#include "as68.h"
 
/* externals */
 
struct _symbol *symtbl = {0};
struct _symbol *syms_head = {0};	/* head of sym table free space */
char *syms_tail = NULL;			/* tail of sym table free space */
int symspace = SYMTSIZE;		/* bytes in symbol table */
int symbols = 0;			/* number of symbols in table */

FLAG fatal = NO;
char pass = 1;  			/* present pass number, 1 or 2 */
unsigned line_count = 0;		/* line number of source file */
unsigned list_count = 0;		/* line # of list file */
long loc_counter = 0;   		/* address to assemble obj code */
int loc_plus = 0;			/* increment to loc counter	*/
char lcksm = 0;			/* bytes + addr. + count + lcksm == 0xff */
FLAG abs_long = YES;			/* default to absolute long add.*/
FLAG rorg = NO; 			/* prog. counter relative flag */
FLAG do_label = YES;			/* process label if set */
char statement[STMNT_SIZE] = {0};	/* statement line buffer */
char label[32] = {0};			/* bufr for label from preparse */
char instr[32] = {0};			/* bufr for mnem, psdo or macro */
char *opfld_ptr = NULL;			/* pointer to operand field */
char stnt_typ = 0;			/* statement type, instr, pseudo */
int wrap = 80;  			/* wrap column for list output */
FLAG trunc = NO;			/* truncate lines in listing */
char source_name[FNAME_SIZE] = {0};	/* source file name */
int src_level = 0;			/* source file level */
FILE *_src[SRCLEVELS] = {0};		/* source file descripters */
FLAG objchnl = 's';			/* Motorola S ('s') or nil ('x') */
FLAG obj_open = NO;			/* object channel open flag */
char of_disk = NULL;			/* default object file drive */
char object_name[FNAME_SIZE] = {0};	/* object file name */
FILE *obj_f = NULL;			/* object file descripter */
FLAG lstchnl = NO;			/* list channel(s) open */
FLAG nolist = NO;			/* flag for list/nolist pseudos */
FLAG lcon = NO; 			/* list to console flag */
FLAG llst = NO; 			/* list to list device flag */
FLAG lfile = NO;			/* list to file flag */
FLAG lfile_open	= NO;   		/* list file open flag */
char lf_disk = NULL;			/* default list file drive */
char list_name[FNAME_SIZE] = {0};	/* listing file name */
FILE *lst_f = NULL, *lst_d = NULL;	/* listing file & dev descripter */
FLAG errchnl = YES;			/* error channel(s) open */
FLAG econ = YES;			/* errors to console flag */
FLAG elst = NO; 			/* errors to list device flag */
FLAG elfile = NO;			/* errors to listing file */
FLAG efile = NO;			/* errors to error file */
FLAG efile_open	= NO;   		/* error file open flag */
char ef_disk = NULL;			/* default error file drive */
char error_name[FNAME_SIZE] = {0};	/* error file name */
FILE *err_f = NULL;			/* error file descripter */
struct _oprnd op1 = {0}, op2 = {0};	/* structs to hold operand val */
char code[12] = {0};			/* 12 minimum for overflow */
char buf[85] = {0};
long lex_val = 0L;
char *p = NULL;
 
extern struct _mtable mtable[];

/* things that were 'static' in overlays... */

FLAG any_errors = NO;
int err_stk[MAXERR] = {0};	/* these two used to be 'static', moved to */
int err_sp = MAXERR;		/*  root for overlay purposes */
char *stmnt_ptr = NULL;	/* from ASETC.C, fcn obj_out */
long s_lc = 0L;
char s_buf[48] = "S";	/* init first character */
int s_cnt = 0;		/* bytes in s_buf */
int s_x = 0;		/* index into s_buf */

/* types */

char *nextfield();
struct _mtable *mnemsearch();
struct _ptable *psdosearch();
 
/* beginmain */
 
main(argc, argv)
int argc;
char *argv[];
{
    register struct _ptable *pt;    /* function	ptr returned by psdosearch */
    register struct _mtable *mt;    /* function	ptr returned by mnemsearch */
 
    if (asinit(argc, argv)) {	/*************** start pass loop... */
 
    while (nextline()) {
 
/* parse line for label, instruction and operand field pointer */
 
	if (preparse() == COMMENT) {
	    if (pass == 2) dump_code(MSG, code, 0);	/* dump the line */
	    continue;
	}
	if (instr[0]) {     /*  mnemonic/pseudo-op/macro field is present) */
	    if (instr[31] && (pass == 2)) err_out(BAD_INSTR);
						    /* check for mnemonic */
	    if (mt = mnemsearch(instr))	{
		stnt_typ = MNEMONIC;
		pass == 1 ? p1_mnem(mt)	: p2_mnem(mt);
	    }
						    /* or pseudo-op */
	    else if (pt = psdosearch(instr)) {
		stnt_typ = PSEUDO;		/* record the fact */
		switch (pass) {
		case 1:
		    (*pt->_p1)(pt->_psdo, pt->_arg);
		    if (pass == 2) continue;
		    break;
		case 2:
		    (*pt->_p2)(pt->_psdo, pt->_arg);
		    if (pass == 3) goto finish;
		}
	    }
						/** or macro use */
	    else if (FALSE /** macros not used yet **/) {
			/** MACROS NOT IMPLIMENTED in version 1.xx */
			stnt_typ = MACRO;
	    }
	    else {
		stnt_typ = NULL;
		err_out(UNREC_INSTR);
		dump_code (CODE, code, 0);
	    }
	}
	else {
	    if (pass == 2) {
		dump_code (CODE, code, 0);	/* label only line ? */
	    }
	}
 
	if (label[0] && do_label) {
	    label_do();
	}
	do_label = YES; 		    /* reset for next line */
	loc_counter += loc_plus;	    /* update loc_counter */
	loc_plus = NULL;		    /* clear for next instruction */
	if (fatal) {    		    /* fatal system ERROR */
	    puts("\007\nFATAL SYSTEM ERROR!\007");
	    break;
	}
    }   				    /* end of pass loop */
    } /* endif(asinit()) */

/** close shop... */
finish: 
    finis();
}
 
/* endmain */
 
label_do()
{
    register int x;
 
    do_label = NO;
    if (pass == 1) {
	/** add as permanent (and/or relative) */
	if (symenter(label, loc_counter, (rorg) ? 'r' : 'a')
	    == ERROR) {
	    fatal = YES;
	    puts("\007\nsymbol table FULL!\007");
	    return ERROR;
	}
    }
    else {      				/* pass == 2 */
	if (x = symsearch(label)) {
	    if (symtbl[x]._atr & 4) {   	    /* redefined */
		err_out(REDEF);
		err_out(NULL);	/* cause the dumping of this error */
	    if (symtbl[x]._val != loc_counter) {
		err_out(PHASE);
		err_out(NULL);
	    }
	    }
	}
	else {
	    err_out(UNDEF_SYMBOL);  /* label never defined */
	    err_out(NULL);	/* cause the dumping of this error */
	}
    }
    return OK;
}
 
dump_code(typ, cdp, cdl)
int typ;
char *cdp;
int cdl;
{
    register int x, y;
    char buf[85];
    char acode[20];
 
    switch (typ) {
    case CODE: 		/* dump all fields, including error stack */
    case DATA: 		/* dump line, loc, code, pseudo, op/comments */
    case MSG:
	obj_out(typ, cdp, cdl);
	if (!lstchnl) {
	    if (typ == CODE  || typ == MSG)	/* dump remaining op/com */
		if (errchnl && any_errors) err_out(NULL); /* dump errors */
	    break;
	}
	if (!cdl) {		/* print loc only if data */
	    sprintf(buf, " %4d        ", line_count);
	} else {
	    sprintf(buf, " %4d %06lx ", line_count, loc_counter);
	}
	x = 13;
	if (cdl) {
	    hex_byt(acode, cdp, cdl);	    /* convert raw code to ascii hex */
	    for (y = 0; y < (cdl * 2); ) {  /* for each byt in acode */
		buf[x++] = acode[y++];		/* ...move to buf... */
		if (!(y%4)) buf[x++] = ' ';     /* ...if 4th byte add space */
	    }
	}
	while (x < 40) buf[x++]	= ' ';      /* pad rest with spaces */
	buf[x] = '\0';
	list_dump(buf);			    /* send it */
	if (get_source(buf)) {  	    /* dump the source */
	    list_dump(buf);
	}
	if (typ == DATA) break; 	    /* let data sender flush */
	goto flush;			    /* dump source */
    case MDATA: 			    /* dump loc, code, op/comments */
	obj_out(typ, cdp, cdl);
	if (!lstchnl) break;
	hex_byt(acode, cdp, cdl);	    /* convert raw code to ascii hex */
	for (x = 0; x < 13; ) buf[x++] = ' ';   	    /* pad it */
	for (y = 0; y < (cdl * 2); ) {      /* for each byte in acode... */
	    buf[x++] = acode[y++];	    /* ...move to buf... */
	    if (!(y%4))	buf[x++] = ' ';     /* ...if 4th byte add space */
	}
	while (x < 40) buf[x++]	= ' ';
	buf[x] = '\0';
	list_dump(buf);
	if (get_source(buf)) {  		/* dump the source */
	    list_dump(buf);
	}
	else list_dump("\n");		    /* terminate the data line */
	break;
    case FLUSH: /* dump remaining comment field, error stack */
flush:
	while (get_source(buf))	{
/**	    if (buf[0] == '\n') break;	*/
	    list_dump("\t\t\t\t\t");		/* 5 tabs equals 40 spaces */
	    list_dump(buf);
	}
	if (errchnl && any_errors) err_out(NULL);    /* dump errors */
	break;
    }
}
 


