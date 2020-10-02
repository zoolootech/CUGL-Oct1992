/******************************************************************
	as68.c, the source of a 68000 assembler....
*/

/*		(C) Copyright 1982 Steve Passe		*/
/*		All Rights Reserved					*/

/*  ver. 1.00 */
/* created 10/7/82 */

/* version 1.01

	8/30/83	ver. 1.01 modified for Aztec ver. 1.05g	smp
	12/23/83	fixed 24 bit address bug in flush_rcrd smp
	12/24/83	fixed lowercase a-f in S-records smp

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

struct _symbol *symtbl;
struct _symbol *syms_head;					/* head of sym table free space */
char *syms_tail;							/* tail of sym table free space */
int symspace = SYMTSIZE;					/* bytes in symbol table */
int symbols = 0;							/* number of symbols in table */

FLAG fatal = NO;
char pass = 1;								/* present pass number, 1 or 2 */
unsigned line_count = 0;					/* line number of source file */
unsigned list_count = 0;					/* line # of list file */
long loc_counter = 0;						/* address to assemble obj code */
int loc_plus = 0;							/* increment to loc counter */
char lcksm;						/* bytes + addr. + count + lcksm == 0xff */
FLAG abs_long = YES;						/* default to absolute long add.*/
FLAG rorg = NO;								/* prog. counter relative flag */
FLAG do_label = YES;						/* process label if set */
char statement[STMNT_SIZE];					/* statement line buffer */
char label[32];								/* bufr for label from preparse */
char instr[32];								/* bufr for mnem, psdo or macro */
char *opfld_ptr;							/* pointer to operand field */
char *stmnt_ptr;							/* ptr to statement field */
char stmnt_typ;								/* statement type, instr, pseudo */
int wrap = 80;								/* wrap column for list output */
FLAG trunc = NO;							/* truncate lines in listing */
char source_name[FNAME_SIZE];				/* source file name */
int src_level = 0;							/* source file level */
FILE *_src[SRCLEVELS];						/* source file descripters */
FLAG objchnl = 's';							/* Motorola S ('s') or nil ('x') */
FLAG obj_open = NO;							/* object channel open flag */
char of_disk = NULL;						/* default object file drive */
char object_name[FNAME_SIZE];				/* object file name */
FILE *obj_f;								/* object file descripter */
FLAG lstchnl = NO;							/* list channel(s) open */
FLAG nolist = NO;							/* flag for list/nolist pseudos */
FLAG lcon = NO;								/* list to console flag */
FLAG llst = NO;								/* list to list device flag */
FLAG lfile = NO;							/* list to file flag */
FLAG lfile_open = NO;						/* list file open flag */
char lf_disk = NULL;						/* default list file drive */
char list_name[FNAME_SIZE];					/* listing file name */
FILE *lst_f, *lst_d;						/* listing file & dev descripter */
FLAG errchnl = YES;							/* error channel(s) open */
FLAG econ = YES;							/* errors to console flag */
FLAG elst = NO;								/* errors to list device flag */
FLAG elfile = NO;							/* errors to listing file */
FLAG efile = NO;							/* errors to error file */
FLAG efile_open = NO;						/* error file open flag */
char ef_disk = NULL;						/* default error file drive */
char error_name[FNAME_SIZE];				/* error file name */
FILE *err_f;								/* error file descripter */

struct _oprnd op1, op2;					/* structs to hold operand val */

char code[12];					/* 12 minimum for overflow */
char buf[85];

long lex_val;
char *p;

extern struct _mtable mtable[];

/* beginmain */

main(argc, argv)
int argc;
char *argv[];
{

	register int x, y;
	char *c_ptr;					/* scratch pointer to char */
	char _ext[5];					/* scratch ext buffer */
	struct _ptable *pt;				/* function ptr returned by psdosearch */
	struct _mtable *mt;				/* function ptr returned by mnemsearch */
	long _dtol();

	puts("\nas68, ver 1.01, 68000 assembler, (C) Copyright 1982 Steve Passe\n");

/** parse the command line, setting variables as needed */

	if (argc < 2) {
		puts("\n...source file?");
		exit(0);
	}
/** set flag variables according to command line args */

	for (x = 2; x < argc; ++x) {
		switch (tolower(argv[x][0])) {
		case 'e':										/* error file opt */
			econ = NO;
			for (y = 1; argv[x][y]; ++y) {
				switch (tolower(argv[x][y])) {
				case 'c':	econ = YES;		continue;
				case 'l':	elst = YES;		continue;
				case 'f':	elfile = YES;	continue;
				case 'e':	efile = YES;
					if (argv[x][y+1] && argv[x][y+2] == ':') {
						ef_disk = argv[x][++y];
						++y;
					}
					continue;
				default:	errchnl = NO;
				}
			}
			continue;
		case 'l':										/* list file opt */
			lstchnl = YES;
			for (y = 1; argv[x][y]; ++y) {
				switch (tolower(argv[x][y])) {
				case 'c':	lcon = YES;		continue;
				case 'l':	llst = YES;		continue;
				case 'f':	lfile = YES;
					if (argv[x][y+1] && argv[x][y+2] == ':') {
						lf_disk = argv[x][++y];
						++y;
					}
					continue;
				default:	lstchnl = NO;
				}
			}
			continue;
		case 'o':										/* object file opt */
			for (y = 1; argv[x][y]; ++y) {
				switch (tolower(argv[x][y])) {
				case 's':	objchnl = 's';
					if (argv[x][y+1] && argv[x][y+2] == ':') {
						of_disk = argv[x][++y];
						++y;
					}
					continue;
				default:	objchnl = 'x';
				}
			}
			continue;
		case 's':
			opfld_ptr = &argv[x][1];
			symspace = (int) _dtol();
			continue;
		case 'w':
			wrap = atoi(&argv[x][1]);
			if (wrap < 60) wrap = 60;
			continue;
		case 't':
			trunc = YES;
			continue;
		default:	puts("\nhuh?");	exit(0);
		}
	}

	if (!(symtbl = alloc(symspace))) {	/* allocate space for symbol table */
		printf("\n...symbol table too large (%d)", symspace);
		exit(0);
	}
	syms_head = symtbl + 1;							/* init free space head */
	syms_tail = symtbl;	syms_tail += symspace;		/* init free space tail */

	symtbl[0]._sym = "";							/* dummy entry */
	symtbl[0]._val = NULL;
	symtbl[0]._atr = NULL;

/* initialize sourcename, errname, listname, and objname */

	if (too_long(argv[1], 14)) {
		exit(0);
	}
	if (argv[1][1] == ':') {					/* a disk identifier exists */
		strcpy(source_name, argv[1]);				/* get source file name */
	}
	else {										/* default to logged in disk */
		if (too_long(argv[1], 12)) {
			exit(0);
		}
		source_name[0] = bdos(CURR_DISK, 0) + 'A';		/* read default disk */
		source_name[1] = ':';							/* separator */
		source_name[2] = '\0';							/* make a string */
		strcat(source_name, argv[1]);					/* add the name */
	}
	if (c_ptr = cisat('.', source_name)) {		/* if extension */
		word_copy(_ext, 4, c_ptr);					/* save ext */
		*c_ptr = '\0';								/* erase it */
	}
	else strcpy(_ext, ".sa");					/* default extension */
	if (too_long(source_name, 10)) {
		exit(0);
	}
	strcpy(object_name, source_name);			/* make object file name */
	switch (objchnl) {
	case 'x':	break;								/* none wanted */
	case 's':	strcat(object_name, ".mx");		/* for Motorola S FILE */
				break;
	default:	printf("\n...bad object file type ('%c')", objchnl);
	}
	if (of_disk) object_name[0] = of_disk;		/* pick up option */
	strcpy(list_name, source_name);
	strcat(list_name, ".ls");					/* make list file name */
	if (lf_disk) list_name[0] = lf_disk;		/* option */
	strcpy(error_name, source_name);
	strcat(error_name, ".err");					/* make error file name */
	if (ef_disk) error_name[0] = ef_disk;		/* option */
	strcat(source_name, _ext);					/* add ext. */

	/* open appropriate source, list, error, and object files */

	if (!(_src[0] = fopen(source_name, "r"))) {
		printf("\n...can't open source (\"%s\")", source_name);
		exit(0);
	}
									/* open list, err, and obj as needed */
	if (objchnl != 'x') {
		if (!(obj_f = fopen(object_name, "w"))) {
			printf("\n...can't open object (\"%s\")", object_name);
			exit(0);
		}
		obj_open = TRUE;
		obj_out(OPEN, 0, 0);
	}
	if (lfile) {
		if (!(lst_f = fopen(list_name, "w"))) {
			printf("\n...can't open list (\"%s\")", list_name);
			exit(0);
		}
		lfile_open = TRUE;
	}
	if (efile) {
		if (!(err_f = fopen(error_name, "w"))) {
			printf("\n...can't open error (\"%s\")", error_name);
			exit(0);
		}
		efile_open = TRUE;
	}
	if (llst || elst) {
		if (!(lst_d = fopen("lst:", "w"))) {
			puts("\n...can't access list device.");
			exit(0);
		}
	}

/******************************************** start pass loop... */

	while (nextline()) {

/* parse line for label, instruction and operand field pointer */

		if (preparse() == COMMENT) {
			if (pass == 2) dump_code(MSG, code, 0);		/* dump the line */
			continue;
		}
		if (instr[0]) {		/*  mnemonic/pseudo-op/macro field is present) */
			if (instr[31] && (pass == 2)) err_out(BAD_INSTR);
													/* check for mnemonic */
			if (mt = mnemsearch(instr)) {
				stmnt_typ = MNEMONIC;
				pass == 1 ? p1_mnem(mt) : p2_mnem(mt);
			}
													/* or pseudo-op */
			else if (pt = psdosearch(instr)) {
				stmnt_typ = PSEUDO;				/* record the fact */
				switch (pass) {
				case 1:
					(*pt->_p1)(pt->_psdo, pt->_arg);
					if (pass == 2) continue;
					break;
				case 2:
					(*pt->_p2)(pt->_psdo, pt->_arg);
					if (pass == 3) goto finis;
				}
			}
												/** or macro use */
			else if (FALSE /** macros not used yet **/) {
						/** MACROS NOT IMPLIMENTED in version 1.xx */
						stmnt_typ = MACRO;
			}
			else {
				stmnt_typ = NULL;
				err_out(UNREC_INSTR);
				dump_code (CODE, code, 0);
			}
		}
		else {
			if (pass == 2) {
				dump_code (CODE, code, 0);				/* label only line ? */
			}
		}

		if (label[0] && do_label) {
			label_do();
		}
		do_label = YES;						/* reset for next line */
		loc_counter += loc_plus;			/* update loc_counter */
		loc_plus = NULL;					/* clear for next instruction */
		if (fatal) {						/* fatal system ERROR */
			puts("\007\nFATAL SYSTEM ERROR!\007");
			break;
		}
	}										/* end of pass loop */

/** close shop... */

finis:
	if (obj_open) {
		fclose(obj_f);
	}
	if (efile_open) {
		fclose(err_f);
	}
	if (lstchnl) {									/* display symbol table */
		sprintf(buf,"\n\n\n%-32s%-14s%-12s%s\n\n",
					"symbol -", "hex value -", "decimal -", "atrb. -");
		list_dump(buf);
		for (x = 0; x++ < symbols; ) {
			sprintf(buf, " %-32s%9lx%12ld      0x%02x\n",
				symtbl[x]._sym, symtbl[x]._val,symtbl[x]._val, symtbl[x]._atr);
			list_dump(buf);
		}
		if (lfile_open) {
			/** finish listing file */
			fclose(lst_f);
		}
	}
	exit(0);
}


/* endmain */

label_do()
{
	int x;

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
	else {										/* pass == 2 */
		if (x = symsearch(label)) {
			if (symtbl[x]._atr & 4) {				/* redefined */
				err_out(REDEF);
				err_out(NULL);	/* cause the dumping of this error */
			}
		}
		else {
			err_out(UNDEF_SYMBOL);	/* label never defined */
			err_out(NULL);		/* cause the dumping of this error */
		}
	}
	return OK;
}

/* parse statement for label and instruction, find operand field */

/* state types */
#define START 1
#define LBL 2
#define LIM 3
#define STOP 4

/* character types */
#define LIM_C 1
#define LM_C 2
#define LBLTRM_C 3
#define WS_C 4
#define STMEND_C 5
#define CMNT_C 6
#define ILGL_C 7

preparse()
{
	char *sbuf_ptr = statement;			/* pointer to statement buffer */
	char *field_ptr = NULL;				/* pointer to present field */
	char field_len = 0;					/* length of present scanned field */
	FLAG state = START;					/* present state */
	FLAG bad_field = NO;

	label[0] = label[31] = instr[0] = instr[31] = NULL;
	opfld_ptr = NULL;
	stmnt_ptr = statement;				/* init for get_source() */

/* LIM_C:		'A'-'Z','a'-'z','.'		*/
/* LM_C:		'0'-'9','_',?'-'?,'$'	*/
/* LBLTRM_C:	':'						*/
/* WS_C:		' ','\t'				*/
/* STMEND_C:	'\n'					*/
/* CMNT_C:		'*'						*/
/* ILGL_C:		<anything else>			*/

/* START state */

	switch (nextc(&sbuf_ptr)) {
	case CMNT_C:
	case STMEND_C:
		opfld_ptr = statement;			/* operands or comment */
		return COMMENT;					/* nothing here... */
	case ILGL_C:						/* illegal character */
	case LM_C:							/* LM_C's can't start label */
		label[31] = YES;				/* bad label char in label field */
	case LIM_C:
		field_ptr = sbuf_ptr - 1;		/* nextc already inc. sbuf_ptr */
		++field_len;					/* got 1 so far */
		state = LBL;					/* enter LBL state */
		break;
	case WS_C:
		field_ptr = nextfield(&sbuf_ptr);		/* skip the white space */
		if (*field_ptr == '*' || *field_ptr == '\n') {
			opfld_ptr = statement;				/* operands or comment */
			return COMMENT;
		}
		state = LIM;
		break;									/* try again */
	}

/* LBL state */

	while (state == LBL) {
		switch(nextc(&sbuf_ptr)) {
		case ILGL_C:
			label[31] = YES;				/* got a bad char */
		case LIM_C:
		case LM_C:
			++field_len;					/* one more label char */
			continue;						/* still in LBL state */
		case STMEND_C:
			state = STOP;
		case LBLTRM_C:							/* label ended... */
		case WS_C:
			savefield(field_ptr, field_len, label);	/* save the label */
			if (state != STOP) {
				field_len = NULL;					/* starting fresh */
				field_ptr = nextfield(&sbuf_ptr);	/* get a new field */
/**f3*/			state = (*field_ptr == '*') ? STOP : LIM;
			}
		}
	}

/* LIM state */

	while (state == LIM) {
		switch(nextc(&sbuf_ptr)) {
		case LM_C:
		case LIM_C:
				++field_len;
				continue;								/* try another */
		case STMEND_C:
		case WS_C:
				state = STOP;
				savefield(field_ptr, field_len, instr);	/* save legal instr */
				instr[31] = bad_field;
				break;
		case LBLTRM_C:
				savefield(field_ptr, field_len, label);	/* save label */
				label[31] = bad_field;
				bad_field = field_len = NULL;			/* going for instr */
				field_ptr = nextfield(&sbuf_ptr);		/* look for it */
				continue;								/* still LIM state */
		case ILGL_C:
				bad_field = YES;						/* bad char */
				++field_len;
				continue;								/* try another */
		}
	}


/* STOP state */

	nextfield(&sbuf_ptr);							/* get to next field */
	opfld_ptr =  sbuf_ptr;							/* operands or comment */
	return OK;
}

/* return type of char pointed to, bump pointer */

nextc(ptr)
char **ptr;
{
	register char c;

	c = *(*ptr)++;	/* get the buf ptr, post inc., get char it points to */
	if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || (c == '.'))
		return LIM_C;
	if (((c >= '0') && (c <= '9')) || (c == '_') || (c == '$'))
		return LM_C;				/** || (c == '-') ??? **/
	switch (c) {
	case '\n':	return STMEND_C;
	case ' ':
	case '\t':	return WS_C;
	case ':':	return LBLTRM_C;
	case '*':	return CMNT_C;
	default:	return ILGL_C;
	}
}

/* align pointer on next char other than ' ' or '\t' */

nextfield(ptr)
char **ptr;
{
	if (!**ptr)	return --(*ptr);
	while ((**ptr == ' ') || (**ptr == '\t'))	++(*ptr);	/* bump buf ptr */
	return *ptr;											/* value of ptr */
}

/***
	get a line of text into statement.  Allows nesting of source files and
	input from stdin.
***/

char *
nextline()
{												/* get a line into buffer */
	while (!fgets(statement, STMNT_SIZE, _src[src_level])) {
		if (src_level) {					/* in an included file */
			fclose(_src[src_level--]);		/* close file, pop stack */
			continue;						/* try level above */
		}	
		if (++pass > 2) return NULL;		/* end second pass main file */
		loc_counter = loc_plus = 0;			/* set counter to default */
		line_count = 0;						/* rewind for second pass */
		for ( ; src_level; fclose(_src[src_level--]));/* close include files */
		fseek(_src[0], 0L, 0);				/* rewind the source file */
		continue;							/* start second pass */
	}
	++line_count;							/* count the line */
	return statement;						/* anything but NULL would work */
}

/* save the field pointed to by ptr, max field length is 30 + NULL */

savefield(ptr, len, dest)
char *ptr;
int len;
char *dest;
{
	char x;
	for (x = 30; x && len; --x, --len)			/* dec. x and len each time */
		*dest++ = *ptr++;			/* move ptr char to dest char, inc. both */
	*dest = '\0';					/* terminate string */
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
***/													/* end of preparse() */

/***
	send data for present line here, dump when appropriate

line format:

         1         2         3         4         5         6         7
1234567890123456789012345678901234567890123456789012345678901234567890123456789
|  | |    | |  | |       | |       |     |     | |             | |            |
line locval opcd 1st  word 2nd  word     instrct operand field.. .comment field

1234 123456 1234 1234 1234 1234 1234    123456789223456789323456789423456789...
           [---- ---- ---- ---- ----]   [-----------------------------------..]

***/

/*
	raw code is sent here as it is produced by the system. this
	function will send it to appropriate places, in the correct form.
*/

dump_code(typ, cdp, cdl)
int typ;
char *cdp;
int cdl;
{
	register int x, y;
	char buf[85];
	char acode[20];

	switch (typ) {
	case CODE:					/* dump all fields, including error stack */
	case DATA:					/* dump line, loc, code, pseudo, op/comments */
	case MSG:
		obj_out(typ, cdp, cdl);
		if (!lstchnl) {
			if (typ == CODE  || typ == MSG)			/* dump remaining op/com */
				if (errchnl) err_out(NULL);			/* dump errors */
			break;
		}
		sprintf(buf, " %04d %06lx ", line_count, loc_counter); /* bytes 0-12 */
		x = 13;
		if (cdl) {
			hex_byt(acode, cdp, cdl);		/* convert raw code to ascii hex */
			for (y = 0; y < (cdl * 2); ) {	/* for each byt in acode */
				buf[x++] = acode[y++];			/* ...move to buf... */
				if (!(y%4)) buf[x++] = ' ';		/* ...if 4th byte add space */
			}
		}
		while (x < 40) buf[x++] = ' ';		/* pad rest with spaces */
		buf[x] = '\0';
		list_dump(buf);						/* send it */
		if (get_source(buf)) {				/* dump the source */
			list_dump(buf);
		}
		if (typ == DATA) break;				/* let data sender flush */
		goto flush;							/* dump source */
	case MDATA:								/* dump loc, code, op/comments */
		obj_out(typ, cdp, cdl);
		if (!lstchnl) break;
		hex_byt(acode, cdp, cdl);			/* convert raw code to ascii hex */
		for (x = 0; x < 13; ) buf[x++] = ' ';				/* pad it */
		for (y = 0; y < (cdl * 2); ) {		/* for each byte in acode... */
			buf[x++] = acode[y++];			/* ...move to buf... */
			if (!(y%4)) buf[x++] = ' ';		/* ...if 4th byte add space */
		}
		while (x < 40) buf[x++] = ' ';
		buf[x] = '\0';
		list_dump(buf);
		if (get_source(buf)) {					/* dump the source */
			list_dump(buf);
		}
		else list_dump("\n");				/* terminate the data line */
		break;
	case FLUSH:	/* dump remaining comment field, error stack */
flush:
		while (get_source(buf)) {
/**			if (buf[0] == '\n') break;	*/
			list_dump("\t\t\t\t\t");			/* 5 tabs equals 40 spaces */
			list_dump(buf);
		}
		if (errchnl) err_out(NULL);							/* dump errors */
		break;
	}
}

/* send buffer to all open list channels */

list_dump(buf)
char *buf;
{
	if (nolist) return;
	if (lcon) puts(buf);
	if (llst) fputs(buf, lst_d);
	if (lfile) fputs(buf, lst_f);
}

/* report the error to all open error channels */

err_out(err_num)
int err_num;
{
	register int x;
	char buf[85];
	static int err_stk[10];
	static int err_sp = 10;

	switch (err_num) {
	case NULL:
		for (x = 10; --x >= err_sp; ) {
			sprintf(buf, "?> error # %d\n", -(err_stk[x]) - 100);
			if (econ) {
				puts(buf);
			}
			if (elst) {
				fputs(buf, lst_d);
			}
			if (elfile) {
				fputs(buf, lst_f);
			}
			if (efile) {
				fprintf(err_f, ">> level: %d, line: %d\n",
							src_level + 1, line_count);
				fputs(buf, err_f);
			}
		}
	case FLUSH:
		err_sp = 10;									/* reset */
		break;
	default:
		if (err_sp) {
			err_stk[--err_sp] = err_num;
		}
	}
	return err_num;
}

obj_out(action, cdp, size)
int action;
char *cdp;
int size;
{
	register int y;
	static long lc = 0L;
	static char s_buf[48] = "S";	/* init first char, room to spare */
	static char cnt;				/* bytes in current buffer */
	static int x;					/* index into s_buf */

	if (!obj_open) return NULL;					/* put into obj file */
	switch (action) {
	case CODE:
	case DATA:
		lc = 0L;
	case MDATA:
top:
		for (y = 0; size && (cnt < 16); --size, ++cnt) {
/**f4*/		lcksm += cdp[y++];							/* line checksum */
		}
		hex_byt(s_buf + x, cdp, y);						/* get them in buf */
		x += y * 2;										/* bump x */
		lc += y;
		if (cnt == 16) {								/* time to flush */
			flush_rcrd(s_buf, cnt, x);
			new_rcrd(s_buf, lc, &x);
			cnt = 0;
			if (size) {									/* more to buffer */
				cdp += y;								/* skip already done */
				goto top;
			}
		}
		break;
	case SYNC:
/**f4*/	flush_rcrd(s_buf, cnt, x);				/* flush s_buf */
		new_rcrd(s_buf, 0L, &x);
		cnt = 0;
		break;
	case OPEN:
		fputs("S0030000FC\n", obj_f);					/* header record */
/**f4*/ new_rcrd(s_buf, 0L, &x);
		cnt = 0;
		break;
	case CLOSE:
		flush_rcrd(s_buf, cnt, x);				/* flush s_buf */
/**f4*/	fputs("S9020000\n", obj_f);
/**		fprintf(obj_f, "S9030000%02x\n", fcksm);*/
		break;
	case MSG:	break;
	default:	return ERROR;
	}
	return NULL;
}

new_rcrd(s_buf, lc, x)
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

	if (!cnt) return NULL;						/* empty record */
/** fixed 12/20/83 tnx m hitch */
	cnt += 1 + ((s_buf[1] == '2') ? 3 : 2);		/* add byte count */
	hex_byt(s_buf + 2, &cnt, 1);				/* place in s_buf */
	lcksm = 0xff - (lcksm + cnt);				/* add checksum */
	hex_byt(&s_buf[x], &lcksm, 1);				/* place it in s_buf */
	strcpy(&s_buf[x+2], "\n");					/* terminate record */
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
char *ap;
char *bp;
int len;
{
	for ( ; len; --len, ++bp, ap += 2) {
		ap[0] = *bp >> 4;
		ap[0] += (ap[0] > 9) ? 55 : 48;	/* ':' + ('A' - ':') : '0' */
		ap[1] = *bp & 0x0f;
		ap[1] += (ap[1] > 9) ? 55 : 48;
	}
}

/***
	gets up to wrap - 40 chars from statement, places into buf.
***/

get_source(buf)
char *buf;
{
	register int size, cnt = 1;
	int x;

	if ((stmnt_ptr != statement) && trunc) return (int)(buf[0] = '\0');
	for (size = wrap - (40 + 1); --size; ++cnt) {
		switch (*stmnt_ptr) {
		case NULL:	return (int)(buf[0] = '\0');
		case '\n':
			break;
		case '\t':
			size -= (x = 7 - (cnt % 8));	/* 7 'cause loop subs 1 */
			if (size < 1) {					/* tab would overflow.. */
				size = 1;					/* ..so cause exit without */
				continue;					/* placing tab into buffer */
			}
			cnt += x;						/* fall thru to default */
		default:
			*buf++ = *stmnt_ptr++;
			continue;
		}
	}
/**	while ((buf-1) != ' ' && (buf-1) != '\t') {
		--buf;
		--stmnt_ptr;
	}
**/
	*buf++ = '\n';
	*buf = '\0';
	if (*stmnt_ptr == '\n') ++stmnt_ptr;	/* make NULL for next call */
	return TRUE;
}

/* endcode */
5 : 48;	/* ':' + ('A' - ':') : '0' */
		ap[1] = *bp & 0x0f;
		ap[1] += (ap[1] > 9) ? 55 : 48;
	}
}

/***
	gets up