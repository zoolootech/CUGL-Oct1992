###portdefs.h
/* portdefs.h - definitions for portability */
@@ ENVIRONMENT-DEPENDENT - ADJUST TO LOCAL SYSTEM */
@@ Edit your local changes, then replace each "@@" with "/*" */
#ifndef PORTDEFS_H
#define PORTDEFS_H

/* adjust these names to local machine/compiler environment */
typedef unsigned short ushort;	@@ or "unsigned" if short-size == int-size */
typedef unsigned char utiny;	@@ to get unsigned byte */
typedef int void;				@@ delete if compiler supports void */
typedef unsigned index_t;		@@ may be chosen ad-lib locally */
typedef char *data_ptr;			@@ use ANSI "generic ptr" if available */

/* next 5 names require no local changes, will work anywhere */
typedef char tbits;				/* one byte, for bitwise uses */
typedef char tbool;				/* one byte: {0:1} */
typedef ushort bits;			/* 16 bits (or more), for bitwise uses */			
typedef int bool;				/* for function returns: {0:1} */
typedef short metachar;			/* return from getchar: {EOF,0:UCHAR_MAX} */

/* modulo function giving non-negative result */
#define IMOD(i, j) (((i) % (j)) < 0 ? ((i) % (j)) + (j) : ((i) % (j)))
@@ if i % j is never negative, replace with the following line: */
@@ #define IMOD(i, j) ((i) % (j)) */

/* portably convert unsigned number to signed */
#define UI_TO_I(ui) (int)(ui)	@@ more complicated on ones complement */

/* structure offsets and bounds; adjust to local system */
#define STRICT_ALIGN int		@@ adjust to local alignment requirement */
#define OFFSET(st, m) \
	((char *)&((st *)&struct_addr)->m - (char *)&struct_addr)
#define BOUNDOF(t) \
	((char *)(struct {char byte0; t byten; } *)&struct_addr)->byten - \
	(char *)&struct_addr)
static STRICT_ALIGN struct_addr = 0;
#define STRUCTASST(a, b) memcpy(&(a), &(b), sizeof(a))

/* defined constants */
#define FAIL			1			@@ failure exit */
#define SUCCEED			0			@@ normal exit */
#define STDIN			0			/* standard input */
#define STDOUT			1			/* standard output */
#define STDERR			2			/* standard error output */
#define SEEK_SET		0			/* seek relative to start of file */
#define SEEK_CUR		1			/* seek relative to current position */
#define SEEK_END		2			/* seek relative to end */
#endif
###stddef.h
/* stddef.h - standard definitions (partial listing) */
@@ ENVIRONMENT-DEPENDENT - ADJUST TO LOCAL SYSTEM */
@@ Edit your local changes, then replace each "@@" with "/*" */
@@ Use your compiler's own <stddef.h>, if available */
#ifndef STDDEF_H
#define STDDEF_H
typedef unsigned size_t;	@@ use unsigned long in large-object model */

#ifndef NULL
#define NULL 0				@@ use 0L if  int-size < long-size == ptr-size */
#endif

extern int errno;
#endif
###limits.h
/* limits.h - environment limits (partial listing) */
@@ ENVIRONMENT-DEPENDENT - CONFIGURE BEFORE USING */
@@ Edit your local changes, then replace each "@@" with "/*" */
@@ Use your compiler's own <limits.h>, if available */
#ifndef LIMITS_H
#define LIMITS_H
#define CHAR_BIT	8	@@ CONFIGURE */
#endif
###c.pro
# C.PRO - A CONFIGURATION FILE FOR WHITESMITHS C 3.0
#	STANDARD ANSI C LIBRARY PROTO FILE
#	CNDOS86 	NATIVE MSDOS
#	PROGRAMMABLE FLAG OPTIONS:
#
#	com	: create an 8080 model .com file
#	debug	: load debugger with any C compile
#	fpp	: use hardware floating point
#	lincl	: include header files in listing or diagnostic output
#	listcs	: create c/assembler listing
#	lo	: create a file (r).lst for listings (default to STDOUT)
#	mods	: uniform small model (64K data and 64K text maximum)
#	modp	: uniform medium model (64K data and multiple text sections)
#	modd	: uniform compact model (64K text and multiple data sections)
#	modf	: uniform far model (multiple text and data sections)
#	proto	: enable prototype checking
#	savlnk	: save linker input file as (r).lnk
#	pponly	: run pp only

sp:pp		-o (o) \
		-i  "|a:/headers.d/|c:/c/hdrs/" \
		c:/c/hdrs/do86{modp?p:{modd?d:{modf?f:s}}}f.h (i)
s:

c:pp		-o (o) {lincl?+lincl} {pponly?:-x}  \
		-i  "|a:/headers.d/|c:/c/hdrs/" \
		-d_PROTO \
		c:/c/hdrs/do86{modp?p:{modd?d:{modf?f:s}}}f.h (i)
1:cp1		-o (o) -cm {debug?+debug} -n31 +std \
		-model{modp?p:{modd?d:{modf?f:s}}} (i)
2:p286		-o (o) -f \
		{listcs?+list} (i)
s:as86		-o (o) \
		{listcs?+list -lo (r).lst} (i)
 {listcs?lm	-o (r).asl -lt (r).lst}
 {listcs?pr	(r).asl {lo?>(r).lst}}
 {listcs?del	(r).asl}

o::echo 	-o (o) > (r).lnk
 type		c:\\c\\hdrs\\do86{com?c:{modp?p:{modd?d:{modf?f:s}}}}.lf \
			>> (r).lnk
 echo		(i) {modp?+text:{modd?+data:{modf?+text +data}}} >> (r).lnk
 echo		\
		c:/c/lib/liba{modp?p:{modd?d:{modf?f:s}}}f.86 \
		c:/c/lib/libb{modp?p:{modd?d:{modf?f:s}}}f.86 \
		c:/c/lib/libm{modp?p:{modd?d:{modf?f:s}}}f.86 \
		{modd?+bss:{modf?+bss:}} >> (r).lnk
 lnk	      < (r).lnk
 {savlnk?:del	(r).lnk}
{com?:xeq:}
com:
xeq:todos -o (o) (i)
exe:
###makefile
@@ ENVIRONMENT-DEPENDENT - HAND-TAILOR EVERY LINE WITH "@@"
@@ THEN EITHER DELETE THE "@@" MESSAGE OR CHANGE TO "#"
#
# Assumes that all files are named  function-name.c
# For complexities like atopi(#2.c, copy the last one to
# a simple name like atopi.c
#
# Note: Successful result of a test is recorded by creation of
# a file named <pgm-name>.ok; see cmpok.c for further info.

# Configuration
RM = del	@@ what command removes a file
CC = cc	@@ the compile command
CFLAGS = 	@@ what options to pass to the compile command
O = obj	@@ the file "extension" for object files
X = exe	@@ the file "extension" for executable files
NOLINK = -c	@@ the CC option that says "don't link"
LIB = lib	@@ the library/archive command
LIBADD = lib -a	@@ library/archive command to add a module
L = a	@@ the file "extension" for object libraries
RUN = 	@@ command prefix (if any) for executing a program
.C.O = $(CC) $(CFLAGS) $(NOLINK) $&		@@ rule for making p.c into p.o
.C.EXE = $(CC) $(CFLAGS) -o $*.$X $&	@@ rule for making p.c into p.exe
.EXE.OK = $(RUN) $*.$X 	@@ rule for making p.exe into p.ok


COMMON = rdslib.$L
ST_MAIN = st_main.$O st_close.$O st_pop.$O st_push.$O 
T_MAIN = t_main.$O q_pop.$O q_close.$O q_insert.$O  q_push.$O q_append.$O q_r_insert.$O
SCREEN = screen86.$O

all : atopi.ok  st_main.ok t_main.ok t2_main.ok tr_main.ok dq_main.ok dq2_main.ok plot_m.$X run_cars.$X rec_main.$X crhash.$X part_menu.$X part_tmenu.$X part_hmenu.$X
	touch uptodate.all

rdslib.$L : itoa.$O fgetsnn.$O getsnn.$O getreply.$O getpstr.$O getplin.$O plot_trk.$O reverse.$O strfit.$O error.$O fixstr.$O
	$(LIB) rdslib.$L getreply.$O getpstr.$O getplin.$O getsnn.$O fgetsnn.$O
	$(LIBADD) rdslib.$L itoa.$O plot_trk.$O reverse.$O strfit.$O error.$O fixstr.$O

mu_lib.$L : mu_lib.$O mu_ask.$O mu_chv.$O mu_str.$O mu_sval.$O mu_pr.$O mu_do.$O mu_reply.$O 
	$(LIB) mu_lib.$L  mu_lib.$O mu_ask.$O mu_chv.$O mu_str.$O mu_sval.$O mu_pr.$O mu_do.$O mu_reply.$O 

dq_lib.$L : dq_lib.$O dq_open.$O dq_close.$O dq_detach.$O dq_r_insert.$O dq_pop.$O dq_insert.$O dq_find.$O dq_first.$O
	$(LIB) dq_lib.$L dq_lib.$O dq_open.$O dq_close.$O dq_detach.$O dq_r_insert.$O dq_pop.$O dq_insert.$O dq_find.$O dq_first.$O

tr_lib.$L : tr_delete.$O tr_lnext.$O tr_lfind.$O tr_lpfind.$O tr_insert.$O tr_detach.$O tr_close.$O tr_lfirst.$O
	$(LIB) tr_lib.$L tr_delete.$O tr_lnext.$O tr_lfind.$O tr_lpfind.$O tr_insert.$O tr_detach.$O tr_close.$O tr_lfirst.$O

rec_lib.$L : rec_hfind.$O rec_havail.$O rec_open.$O rec_close.$O rec_get.$O rec_put.$O 
	$(LIB) rec_lib.$L  rec_hfind.$O rec_havail.$O rec_open.$O rec_close.$O rec_get.$O rec_put.$O 

menu_lib.$L : chg_menu.$O add_menu.$O acd_menu.$O del_menu.$O
	$(LIB) menu_lib.$L chg_menu.$O add_menu.$O acd_menu.$O del_menu.$O

tr_main.$X : tr_main.$O tr_lib.$L $(COMMON)
	$(CC) $(CFLAGS) -o tr_main.$X tr_main.$O tr_lib.$L $(COMMON)

tr_main.ok : tr_main.$X
	$(RUN) tr_main.$X >tr_main.out
	cmpok tr_main

part_menu.$X : part_menu.$O mu_lib.$O menu_lib.$L part_db.$O $(SCREEN) $(COMMON)
	$(CC) $(CFLAGS) -o part_menu.$X part_menu.$O mu_lib.$O menu_lib.$L part_db.$O $(SCREEN) $(COMMON)

part_tmenu.$X : part_tmenu.$O mu_lib.$O menu_lib.$L part_dbt.$O tr_lib.$L $(SCREEN) $(COMMON)
	$(CC) $(CFLAGS) -o part_tmenu.$X part_tmenu.$O mu_lib.$O menu_lib.$L part_dbt.$O tr_lib.$L $(SCREEN) $(COMMON)

plot_m.$X : plot_m.$O $(SCREEN) $(COMMON)
	$(CC) $(CFLAGS) -o plot_m.$X plot_m.$O $(SCREEN) $(COMMON)

cmpok.$X : cmpok.$O $(COMMON)
	$(CC) $(CFLAGS) -o cmpok.$X cmpok.$O $(COMMON)

atopi.ok : atopi.$X atopi.in
atopi.$X : xatopi.$O atopi.$O $(COMMON)
	$(CC) $(CFLAGS) -o atopi.$X  xatopi.$O atopi.$O $(COMMON)

st_main.ok : st_main.$X st_main.in
	$(RUN) st_main <st_main.in >st_main.out
	cmpok st_main
st_main.$X : $(ST_MAIN) $(COMMON)
	$(CC) $(CFLAGS) -o st_main.$X $(ST_MAIN) $(COMMON)

t_main.ok : t_main.$X t_main.in
	$(RUN) t_main <t_main.in >t_main.out
	cmpok t_main
t_main.$X : $(T_MAIN) $(COMMON)
	$(CC) $(CFLAGS) -o t_main.$X $(T_MAIN) $(COMMON)

t2_main.ok : t2_main.$X 
	$(RUN) t2_main.$X >t2_main.out
t2_main.$X : t2_main.$O $(COMMON)
	$(CC) $(CFLAGS) -o t2_main.$X  t2_main.$O $(COMMON)

run_cars.ok : run_cars.$X
	$(RUN) run_cars.$X 2 <run_cars.in >run_cars.out
	cmpok run_cars
run_cars.$X : run_cars.$O dq_lib.$O $(SCREEN) $(COMMON)
	$(CC) $(CFLAGS) -o run_cars.$X  run_cars.$O dq_lib.$O $(SCREEN) $(COMMON)

dq_main.ok : dq_main.$X
	$(RUN) dq_main.$X  <dq_main.in >dq_main.out
	cmpok dq_main
dq_main.$X : dq_main.$O dq_lib.$O $(COMMON)
	$(CC) $(CFLAGS) -o dq_main.$X  dq_main.$O dq_lib.$O $(COMMON) 

dq2_main.ok : dq2_main.$X
	$(RUN) dq2_main.$X  >dq2_main.out
	cmpok dq2_main
dq2_main.$X : dq2_main.$O dq_lib.$O $(COMMON)
	$(CC) $(CFLAGS) -o dq2_main.$X  dq2_main.$O dq_lib.$O $(COMMON) 

rec_main.ok : rec_main.$X
	$(RUN) rec_main.$X  >rec_main.out
	cmpok rec_main
rec_main.$X : rec_main.$O rec_lib.$L $(COMMON)
	$(CC) $(CFLAGS) -o rec_main.$X rec_main.$O rec_lib.$L  $(COMMON) 

crhash.$X : crhash.$O rec_lib.$L $(COMMON)
	$(CC) $(CFLAGS) -o crhash.$X crhash.$O rec_lib.$L  $(COMMON) 

part.dat : crhash.$X
	crhash.$X part.dat 5 33

part_hmenu.$X : part_menu.$O mu_lib.$O menu_lib.$L part_hash.$O rec_lib.$L $(SCREEN) $(COMMON)
	$(CC) $(CFLAGS) -o part_hmenu.$X part_menu.$O mu_lib.$O menu_lib.$L part_hash.$O rec_lib.$L $(SCREEN) $(COMMON)

gen_menu.ok : gen_menu.$X
	$(RUN) gen_menu.$X "<add_menu.mu >gen_menu.out"
	cmpok.$X gen_menu
gen_menu.$X : gen_menu.$O $(COMMON)
	$(CC) $(CFLAGS) -o gen_menu.$X  gen_menu.$O $(COMMON)
###myfcntl.h
/* fcntl.h - definitions for binary  open
 * Compatible with UNIX Sys V, ...
 */
#ifndef FCNTL_H
#define FCNTL_H
#define O_RDONLY 0		/* delete or change to conform to local */
#define O_WRONLY 1		/* delete or change to conform to local */
#define O_RDWR   2		/* delete or change to conform to local */
#define O_NDELAY 4		/* NOT USED BY bin_io FUNCTIONS */
#define O_APPEND 8		/* delete or change to conform to local */
#define O_CREAT  0x100	/* delete or change to conform to local */
#define O_TRUNC  0x200	/* delete or change to conform to local */
#define O_EXCL   0x400	/* delete or change to conform to local */
#endif
###mystdarg.h
typedef char *va_list;
#define va_start(a, p) (a = ((char *)&p + sizeof(p)))
#define va_arg(a, t) ((t *)(a += sizeof(t)))[-1]
#define va_end(a)
###mystddef.h
/* stddef.h - standard definitions (partial listing) */
/* ENVIRONMENT-DEPENDENT - ADJUST TO LOCAL SYSTEM */
@@ Change each @@ to start-comment after configuring */
#ifndef STDDEF_H
#define STDDEF_H
typedef unsigned size_t;	@@ use unsigned long in large-object model */

#ifndef NULL
#define NULL 0				@@ use 0L if  int-size < long-size == ptr-size */
#endif

extern int errno;
#endif
###mystdio.h
/**
*
* This header file defines the information used by the standard I/O
* package.
*
**/
#define _BUFSIZ 512		/* standard buffer size */
#define BUFSIZ 512		/* standard buffer size */
#define _NFILE 20		/* maximum number of files */

struct _iobuf
{
char *_ptr;			/* current buffer pointer */
int _rcnt;			/* current byte count for reading */
int _wcnt;			/* current byte count for writing */
char *_base;		 	/* base address of I/O buffer */
char _flag;			/* control flags */
char _file;			/* file number */
int _size;			/* size of buffer */
char _cbuff;			/* single char buffer */
char _pad;			/* (pad to even number of bytes) */
};

extern struct _iobuf _iob[_NFILE];

#define _IOREAD 1		/* read flag */
#define _IOWRT 2		/* write flag */
#define _IONBF 4		/* non-buffered flag */
#define _IOMYBUF 8		/* private buffer flag */
#define _IOEOF 16		/* end-of-file flag */
#define _IOERR 32		/* error flag */
#define _IOSTRG 64
#define _IORW 128		/* read-write (update) flag */

#define NULL 0			/* null pointer value */
#define FILE struct _iobuf	/* shorthand */
#define EOF (-1)		/* end-of-file code */

#define stdin (&_iob[0])	/* standard input file pointer */
#define stdout (&_iob[1])	/* standard output file pointer */
#define stderr (&_iob[2])	/* standard error file pointer */

#define getc(p) (--(p)->_rcnt>=0? *(p)->_ptr++:_filbf(p))
#define getchar() getc(stdin)
#define putc(c,p) (--(p)->_wcnt>=0? ((int)(*(p)->_ptr++=(c))):_flsbf((c),p))
#define putchar(c) putc(c,stdout)
#define feof(p) (((p)->_flag&_IOEOF)!=0)
#define ferror(p) (((p)->_flag&_IOERR)!=0)
#define fileno(p) (p)->_file
#define rewind(fp) fseek(fp,0L,0)
#define fflush(fp) _flsbf(-1,fp)

FILE *fopen();
FILE *freopen();
long ftell();
char *fgets();

#define abs(x) ((x)<0?-(x):(x))
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<=(b)?(a):(b))

/* Additions needed by Safe C */
#ifdef SC
#undef getc
#undef putc
/* When using the D or L model, define NULL as 0L */
/* */
#ifdef LARGEMODEL
#undef NULL
#define NULL 0L
#endif
/* */

char *_gets(), *_fgets();
struct _iobuf *__fopen(), *__freopen();
long ftell();
#endif
###scr_w_86.c
/* screen - environment-specific terminal functions
 * PC Version - uses  WSL 3.1 sys-dependent fns  
 */
#include "local.h"
#include "screen.h"
short scr_mode = SCR_TTY;	/* screen mode - TTY or RAW */
short scr_lins = 24;	/* screen lines (default) */
short scr_cols = 80;	/* screen columns (default) */
/* rawget - get one char (assuming raw mode) */
static char rawget()
	{
	char c;

	_read(0, &c, 1);
	return (c);
	}
/* scr_getkey - get a (coded) keyboard char */
SCR_CMDCHAR scr_getkey()
	{
	char c1;
	
	scr_refresh();	/* ensure all output is flushed */
	if ((c1 = rawget()) != '\0')
		return (c1 & 0x7F);
	switch (c1 = rawget())
		{
	/* no "break" needed - all returns */
	case 'H':	return (SCR_UP);
	case 'P':	return (SCR_DOWN);
	case 'M':	return (SCR_RIGHT);
	case 'K':	return (SCR_LEFT);
	case 'G':	return (SCR_HOME);
	case ';':	return (SCR_EXIT);	/* F1 function key */
	case 'O':	return ('\r');	/* use END for RETURN */
	default:	return (scr_getkey());
		}
	}
/* remark - print error message, appropriate for scr_mode */
void remark(s1, s2)
	char s1[], s2[];	/* strings to be printed */
	{
	if (scr_mode == SCR_TTY)
		fprintf(stderr, "%s %s\n", s1, s2);
	else
		{
		scr_curs(scr_lins-1, 0);
		scr_print("%s %s; hit any key to continue", s1, s2);
		scr_getkey();
		}
	}
/* scr_open - enter "raw" screen mode */
void scr_open()
	{
	scr_mode = SCR_RAW;
	_rawmode(0, 1);
	_rawmode(1, 1);

	/* DEBUGGING: REMOVE BEFORE FINAL PRINTING */
	if (strcmp(getenv("BUFTYPE"), "setbuf") == 0)
		setbuf(stdout, NULL);
	else if (strcmp(getenv("BUFTYPE"), "setvbuf") == 0)
		setvbuf(stdout, NULL, _IONBF, 0);
	/* END OF DEBUGGING */
	}
/* scr_close - restore normal tty state */
void scr_close()
	{
	scr_mode = SCR_TTY;
	_rawmode(0, 0);
	_rawmode(1, 0);
	}
###wsl_bin_io.h
/* bin_io.h - header for binary file I/O functions */
/* Whitesmiths 2.3 version */
#ifndef BIN_IO_H
#define BIN_IO_H
#include "local.h"

#define BIN_FILE FILE

int bin_fclose();		/* PARMS(BIN_FILE *bf) */
long bin_fseek();		/* PARMS(BIN_FILE *bf, long offset, int whence) */
BIN_FILE *bin_fopen();	/* PARMS(char fname[], char type[]) */
int bin_fread();
			/* PARMS(data_ptr buf, size_t size, int n, BIN_FILE *bf) */
int bin_fread();
			/* PARMS(data_ptr buf, size_t size, int n, BIN_FILE *bf) */

/* {{ CHANGE THE FOLLOWING TO ANSI STYLE BEFORE PRINTING }} */

#define bin_b4r_seek(bf)	/* nothing, no action needed */
#define bin_b4w_seek(bf)	/* nothing, no action needed */

#define bin_fclose(bf)		close(bf)
#define bin_fseek(bf, o, w)	(long)lseek(bf, o, w)	/* WSL - no "tell" */
#define bin_fwrite(b, sz, n, bf)	write(bf, b, (sz)*(n))

#endif
###w_bin_open.c
/* bin_open - open a binary file 
 * WSL 2.3 version
 */
#include "bin_io.h"
bin_fd bin_open(fname, type)
	char fname[];
	int type;
	{
	int fd;

	if ((type & O_TRUNC) != O_TRUNC)	/* not TRUNC mode */
		{
		fd = open(fname, type & O_RWMODE, 1);		/* attempt open */
		if (fd >= 0)
			{
			if ((type & (O_EXCL|O_CREAT)) == (O_EXCL|O_CREAT))
				return (-1);			/* not allowed to exist */
			else
				return (fd);			/* open succeeded */
			}
		else if ((type & O_RWMODE) == O_RDONLY)
			return (fd);				/* rdonly, open failed */
		}
	if ((type & O_CREAT) != O_CREAT)
		return (-1);					/* not allowed to create */
	fd = create(fname, type & O_RWMODE, 1);		/* attempt create */
	return (fd);
	}
###xatopi.c
/* xatopi - test harness for atopi function */
#include "local.h"
#define MINIMAL_WRITE(f) /* write nothing; empty file ok for semaphore */
void fixstr(char *);
main()
	{
	int i;		/* which case {0:N} */
	int ret;	/* returned value from atopi */
	char source[80];	/* source string for atopi */
	char desc[80];		/* description of test case */
	int expect;		/* expected result */
	bool was_ok = YES;	/* successful test? */
	FILE *fin;		/* input file with test cases */
	FILE *fout;		/* output file for "ok" */
	int nread;		/* number of input items read */

	fin = fopen("atopi.in", "r");
	if (fin == NULL)
		error("can't open", "atopi.in");
	while (getc(fin) != '\n')
		;
	while (getc(fin) != '\n')
		;
	FOREVER
		{
		nread = fscanf(fin,
			"%s %d %[^\n]",
			source, &expect, desc);
		if (nread == EOF)
			break;
		else if (nread != 3)
			error("bad input at or after source =", source);
		fixstr(source);
		ret = atopi(source);
		if (ret != expect)
			{
			printf("Failed case: <%s>: ", desc);
			printf("source=<%s>, ret=<%d>\n", source, ret);
			was_ok = NO;
			}
		}
	if (was_ok)
		{
		fout = fopen("atopi.ok", "w");
		MINIMAL_WRITE(fout);
		fclose(fout);
		exit(SUCCEED);
		}
	else
		exit(FAIL);
	}
###EOF
