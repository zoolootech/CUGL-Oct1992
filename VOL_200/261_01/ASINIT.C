/*    asinit.c, the source of a 68000 assembler.... initialization
 *	(C) Copyright 1982 Steve Passe
 *	All Rights Reserved
 *
 *  ver. 1.00
 * created 10/7/82
 *
 * version 1.01
 *
 *    8/30/83 ver. 1.01 modified for Aztec ver. 1.05g smp
 *    12/23/83	fixed 24 bit address bug in flush_rcrd smp
 *    12/24/83	fixed lowercase	a-f in S-records smp
 *    10/09/87	split from as68.c for overlay on PDP-11
 */
 
/* begincode */
 
/* includes */
 
#include <stdio.h>
#include "as68.h"
 
/* externals */
 
extern struct _symbol *symtbl;
extern struct _symbol *syms_head;	/* head of sym table free space */
extern char *syms_tail;			/* tail of sym table free space */
extern int symspace;			/* bytes in symbol table */
extern int symbols;			/* number of symbols in table */
 
extern FLAG fatal;
extern char pass;			/* present pass number, 1 or 2 */
extern unsigned line_count;		/* line number of source file */
extern unsigned list_count;		/* line # of list file */
extern long loc_counter;		/* address to assemble obj code */
extern int loc_plus;			/* increment to loc counter	*/
extern char lcksm;		/* bytes + addr. + count + lcksm == 0xff */
extern FLAG abs_long;			/* default to absolute long add.*/
extern FLAG rorg;			/* prog. counter relative flag */
extern FLAG do_label;			/* process label if set */
extern char statement[STMNT_SIZE];	/* statement line buffer */
extern char label[32];			/* bufr for label from preparse */
extern char instr[32];			/* bufr for mnem, psdo or macro */
extern char *opfld_ptr;			/* pointer to operand field */
extern int wrap;			/* wrap column for list output */
extern FLAG trunc;			/* truncate lines in listing */
extern char source_name[FNAME_SIZE];	/* source file name */
extern int src_level;			/* source file level */
extern FILE *_src[SRCLEVELS];		/* source file descripters */
extern FLAG objchnl;			/* Motorola S ('s') or nil ('x') */
extern FLAG obj_open;			/* object channel open flag */
extern char of_disk;			/* default object file drive */
extern char object_name[FNAME_SIZE];	/* object file name */
extern FILE *obj_f;			/* object file descripter */
extern FLAG lstchnl;			/* list channel(s) open */
extern FLAG nolist;			/* flag for list/nolist pseudos */
extern FLAG lcon;			/* list to console flag */
extern FLAG llst;			/* list to list device flag */
extern FLAG lfile;			/* list to file flag */
extern FLAG lfile_open;   		/* list file open flag */
extern char lf_disk;			/* default list file drive */
extern char list_name[FNAME_SIZE];	/* listing file name */
extern FILE *lst_f, *lst_d;		/* listing file & dev descripter */
extern FLAG errchnl;			/* error channel(s) open */
extern FLAG econ;			/* errors to console flag */
extern FLAG elst;			/* errors to list device flag */
extern FLAG elfile;			/* errors to listing file */
extern FLAG efile;			/* errors to error file */
extern FLAG efile_open;   		/* error file open flag */
extern char ef_disk;			/* default error file drive */
extern char error_name[FNAME_SIZE];	/* error file name */
extern FILE *err_f;			/* error file descripter */
extern struct _oprnd op1, op2;		/* structs to hold operand val */
 
extern char code[12];			/* 12 minimum for overflow */
extern char buf[85];
 
extern long lex_val;
extern char *p;
 
extern struct _mtable mtable[];

/* types */

char *nextfield();
long dtol();
char * cisat();
 
/* beginmain */
 
asinit(argc, argv)
int argc;
char *argv[];
{
    register int x, y;
    char *c_ptr;		    /* scratch pointer to char */
    char _ext[5];		    /* scratch ext buffer */
 
    puts("\nAS68, ver 1.01, 68000 assembler, (C) Copyright 1982 Steve Passe\n");
 
/** parse the command line, setting variables as needed */
 
    if (argc < 2) {
	puts("\n...source file?");
	exit(0);
    }
/** set flag variables according to command line args */
 
    for (x = 2; x < argc; ++x) {
	switch (tolower(argv[x][0])) {
	case 'e':					/* error file opt */
	    econ = NO;
	    for (y = 1; argv[x][y]; ++y) {
		switch (tolower(argv[x][y])) {
		case 'c':   econ = YES;     continue;
		case 'l':   elst = YES;     continue;
		case 'f':   elfile = YES;   continue;
		case 'e':   efile = YES;
		    if (argv[x][y+1] && argv[x][y+2] == ':') {
			ef_disk	= argv[x][++y];
			++y;
		    }
		    continue;
		default:    errchnl = NO;
		}
	    }
	    continue;
	case 'l':					/* list file opt */
	    lstchnl = YES;
	    for (y = 1; argv[x][y]; ++y) {
		switch (tolower(argv[x][y])) {
		case 'c':   lcon = YES;     continue;
		case 'l':   llst = YES;     continue;
		case 'f':   lfile = YES;
		    if (argv[x][y+1] && argv[x][y+2] == ':') {
			lf_disk	= argv[x][++y];
			++y;
		    }
		    continue;
		default:    lstchnl = NO;
		}
	    }
	    continue;
	case 'o':					/* object file opt */
	    for (y = 1; argv[x][y]; ++y) {
		switch (tolower(argv[x][y])) {
		case 's':   objchnl = 's';
		    if (argv[x][y+1] && argv[x][y+2] == ':') {
			of_disk	= argv[x][++y];
			++y;
		    }
		    continue;
		default:    objchnl = 'x';
		}
	    }
	    continue;
	case 's':
	    opfld_ptr = &argv[x][1];
	    symspace = (int) dtol();
	    continue;
	case 'w':
	    wrap = atoi(&argv[x][1]);
	    if (wrap < 60) wrap = 60;
	    continue;
	case 't':
	    trunc = YES;
	    continue;
	default:    puts("\nhuh?"); exit(0);
	}
    }

    /* allocate space for symbol table */ 
    if (!(symtbl = (struct _symbol *) malloc(symspace))) {
	printf("\n...symbol table too large (%d)", symspace);
	exit(0);
    }
    syms_head = symtbl + 1;     		/* init free space head */
    syms_tail = (char *) symtbl;
    syms_tail += symspace;			/* init free space tail */
 
    symtbl[0]._sym = "";			/* dummy entry */
    symtbl[0]._val = NULL;
    symtbl[0]._atr = NULL;
 
/* initialize sourcename, errname, listname, and objname */
 
    if (too_long(argv[1], 14)) {
	exit(0);
    }
    if (argv[1][1] == ':') {    		/* a disk identifier exists */
	strcpy(source_name, argv[1]);		    /* get source file name */
    }
    else {      				/* default to logged in disk */
	if (too_long(argv[1], 12)) {
	    exit(0);
	}
#ifdef CPM
	source_name[0] = bdos(CURR_DISK, 0) + 'A';	/* read default	disk */
	source_name[1] = ':';   			/* separator */
	source_name[2] = '\0';  			/* make a string */
#endif /*CPM*/
	strcat(source_name, argv[1]);			/* add the name */
    }
    if (c_ptr = cisat('.', source_name)) {      /* if extension	*/
	word_copy(_ext,	4, c_ptr);		    /* save ext */
	*c_ptr = '\0';  			    /* erase it */
    }
    else strcpy(_ext, ".sa");			/* default extension */
    if (too_long(source_name, 10)) {
	exit(0);
    }
    strcpy(object_name,	source_name);		/* make object file name */
    switch (objchnl) {
    case 'x':   break;				    /* none wanted */
    case 's':   strcat(object_name, ".mx");	/* for Motorola	S FILE */
		break;
    default:	printf("\n...bad object file type ('%c')", objchnl);
    }
    if (of_disk) object_name[0]	= of_disk;	/* pick up option */
    strcpy(list_name, source_name);
    strcat(list_name, ".ls");			/* make list file name */
    if (lf_disk) list_name[0] = lf_disk;	/* option */
    strcpy(error_name, source_name);
    strcat(error_name, ".err");			/* make error file name */
    if (ef_disk) error_name[0] = ef_disk;	/* option */
    strcat(source_name,	_ext);			/* add ext. */
 
    /* open appropriate	source,	list, error, and object files */
 
    if (!(_src[0] = fopen(source_name, "r"))) {
	printf("\n...can't open source (\"%s\")", source_name);
	exit(0);
    }
				    /* open list, err, and obj as needed */
    if (objchnl	!= 'x') {
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
	if (!(err_f = fopen(error_name,	"w"))) {
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
 }
 
/** close shop... */
 
finis()
{
    register int x;

    if (obj_open) {
	fclose(obj_f);
    }
    if (efile_open) {
	fclose(err_f);
    }
    if (lstchnl) {      			    /* display symbol table */
	sprintf(buf,"\n\n\n%-32s%9s%12s%10s\n\n",
		    "symbol", "hex value", "decimal", "atrb");
	list_dump(buf);
	for (x = 0; x++ < symbols; ) {
	    sprintf(buf, "%-32s%9lx%12ld      0x%02x\n",
		symtbl[x]._sym,
		symtbl[x]._val,
		symtbl[x]._val,
		symtbl[x]._atr);
	    list_dump(buf);
	}
	if (lfile_open)	{
	    /** finish listing file */
	    fclose(lst_f);
	}
    }
    exit(0);
}

/* check for length, report error to console */
 
too_long(str, maxl)
char *str;
int maxl;
{
    if (strlen(str) > maxl) {
	printf("\...too	long (\"%s\")",	str);
	return YES;
    }
    return OK;
}

/* returns pointer to char c in str, or NULL if not found */
 
char *
cisat(c, str)
register char c;
register char *str;
{
    while (*str) if (c == *str++) return --str;
    return NULL;
}

/* private copy of dtol (the one in POP.C overlays this one?)
 */

long
dtol()
{
    long val = 0L;
    register char c;
 
    while ((c = *opfld_ptr++) >= '0' && c <= '9') {
	val = val * 10 + c - '0';
    }
    --opfld_ptr;
    return val;
}

