###bin_io.h
/* bin_io.h - header for binary file I/O functions 
 * SYSTEM DEPENDENT - MUST BE CONFIGURED FOR EACH TARGET SYSTEM
 */
#ifndef BIN_IO_H
#define BIN_IO_H
#include "local.h"

#include "fcntl.h"		/* provide your own if not standard on system */

typedef int bin_fd;		/* "binary file descriptor" {0:BIN_NFILE-1} */

#define BIN_NFILE 20	/* adjust to local system */

#define O_RWMODE (O_RDONLY|O_WRONLY|O_RDWR)	/* uses symbols from fcntl.h */

#ifndef IDRIS
#define bin_open(s, m)		open(s, m)
#endif
#define bin_close(f)		close(f)
#define bin_lseek(f, o, w)	lseek(f, o, w)
#define bin_read(f, b, n)	read(f, b, n)
#define bin_write(f, b, n)	write(f, b, n)

#endif
###fcntl.h
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
###local.h
/* local.h - Definitions for use with Reliable Data Structures in C */
#ifndef LOCAL_H
#define LOCAL_H
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#define FALSE			0			/* Boolean value */
#define FOREVER			for(;;)		/* endless loop */
#define NO				0			/* Boolean value */
#define TRUE			1			/* Boolean value */
#define YES				1			/* Boolean value */
#define getln(s, n)		((fgets(s, n, stdin)==NULL) ? EOF : strlen(s))
#define ABS(x)			(((x) < 0) ? -(x) : (x))
#define MAX(x, y)		(((x) < (y)) ? (y) : (x))
#define MIN(x, y)		(((x) < (y)) ? (x) : (y))
#define DIM(a)			(sizeof(a) / sizeof(a[0]))
#define IN_RANGE(n, lo, hi) ((lo) <= (n) && (n) <= (hi))
#ifndef NDEBUG
#define asserts(cond, str) \
	{if (!(cond)) fprintf(stderr, "Assertion '%s' failed\n", str);}
#else
#define asserts(cond, str)
#endif
#define SWAP(a, b, t) 	((t) = (a), (a) = (b), (b) = (t))
#define LOOPDN(r, n)	for ((r) = (n)+1; --(r) > 0; )
#define STREQ(s, t)		(strcmp(s, t) == 0)
#define STRLT(s, t)		(strcmp(s, t) < 0)
#define STRGT(s, t)		(strcmp(s, t) > 0)
#include "portdefs.h"	/* portability definitions */
#include "stddef.h"		/* (ANSI) standard definitions */
#include "limits.h"		/* (ANSI) machine parameters */
#include "string.h"		/* (ANSI) string functions */
#include "stdlib.h"		/* (ANSI) miscellaneous standard functions */
#include "rdslib.h"		/* functions from Reliable Data Structures in C */
#endif
###menu.h
/* menu.h - header for menu pgm */
#ifndef MENU_H
#define MENU_H
#include "local.h"
#include "screen.h"
typedef struct string_val
	{
	char edit_type;		/* 'n' = numeric, 'd' = short int, 'a' = anything */
	char *string;		/* ptr to the string value */
	short maxsize;		/* maximum size of string: {2:SHORT_MAX} */
	short *pnum;		/* if non-NULL, ptr to numeric storage */
	} STRING_VAL;
typedef struct choice_val
	{
	short nchoices;		/* number of possible choices: {2:10} */
	char **choices;		/* ptr to start of array of choice strings */
	char *pc;			/* ptr to the one-char coded choice value */
	} CHOICE_VAL;
/* (more)
.PE
.PS 33
 */
typedef struct field
	{
	short line, col;	/* co-ordinates of screen display position */
	char *desc;			/* description (title) of this field: string */
	char val_type;		/* what type of field is this?  */
	STRING_VAL *psv;	/* non-NULL iff val_type == 's' or 'S' */
	CHOICE_VAL *pcv;	/* non-NULL iff val_type == 'c' */
	struct menu *pmenu;	/* non-NULL iff val_type == 'm' */
	bool (*pfn)();		/* function to do after str or choice field */
	} FIELD;
typedef struct menu
	{
	char *title;		/* alphabetic title for menu: string */
	FIELD *fields;		/* ptr to beginning of array of FIELDs */
	short nfields;		/* how many fields in this menu */
	short cur_field;	/* index of the current field */
	} MENU;
FIELD *mu_ask();		/* PARMS(MENU *pm) */
SCR_CMDCHAR mu_chv();	/* PARMS(FIELD *pf) */
void mu_do();			/* PARMS(MENU *pm) */
void mu_pr();			/* PARMS(MENU *pm) */
int mu_reply();			/* PARMS(char *prompt, char *reply, int size) */
SCR_CMDCHAR mu_str();	/* PARMS(FIELD *pf) */
SCR_CMDCHAR mu_sval();	/* PARMS(STRING_VAL *psv) */
#endif
###part.h
/* part.h - header for parts structure
 */
#ifndef PART_H
#define PART_H
typedef struct part
	{
	char part_no[14];	/* part number; string */
	char lead_time[3];	/* lead time (in weeks); num string {0:99} */
	char unit_meas[2];	/* unit of measure; coded string */
						/* {"0"=each, "1"=lb, "2"=box, other=each} */
	char unit_cost[8];	/* cost; num string {0.00:9999.99} */
	char cost_qty[5];	/* qty required for price */
						/* num string {0:9999} */
	} PART;
extern PART part1;
#endif
###part.h1
/* part.h - header for PART structure */
#ifndef PART_H
#define PART_H
typedef struct part
	{
	char part_no[14];	/* part number: string */
	short lead_time;	/* lead time (in weeks): {0:99} */
	char unit_meas[6];	/* unit of measure: string {"each", "lb", "box"}  */
	float unit_cost;	/* cost for one unit_meas: {0.00:9999.99} */
	short cost_qty;		/* quantity required for price: {0:9999} */
	} PART;
#endif
###part.h2
/* part.h - header for parts structure
 */
#ifndef PART_H
#define PART_H
typedef struct part
	{
	char part_no[14];	/* part number: string */
	char lead_time[3];	/* lead time (in weeks): num string {0:99} */
	char unit_meas[2];	/* unit of measure: coded string */
						/* {"0"=each, "1"=lb, "2"=box, other=each} */
	char unit_cost[8];	/* cost: num string {0.00 : 9999.99} */
	char cost_qty[5];	/* qty required for price: num string {0:9999} */
	} PART;
extern PART part1;
#endif
###pointer.h
/* pointer.h - concise macros for pointer checking and casting */
/* PNN(p) 		- test that p is not NULL, return p */
/* PNNC(p, t) 	- test that p is not NULL, return p cast to type t */
#ifndef POINTER_H
#define POINTER_H
#ifndef NDEBUG
#define PNN(p)			((p) != NULL ? (p) : PNN_BAD(p))
#define PNNC(p, t)		((p) != NULL ? (t)(p) : (t)PNN_BAD(p))
#define PNN_BAD(p)		(fprintf(stderr, "NULL\n"), p)
#else
#define PNN(p)			(p)
#define PNNC(p,t)		((t)(p))
#endif
#endif
###queue.h
/* queue.h - header for queue package */
#ifndef QUEUE_H
#define QUEUE_H
#include "local.h"
#include "pointer.h"
#define Q_NODE struct q_node
Q_NODE
	{
	Q_NODE *next;
	/* ... */
	};

void q_append();	/* PARMS(Q_NODE **frontp, Q_NODE **rearp, Q_NODE *p) */
void q_close();		/* PARMS(Q_NODE **frontp, Q_NODE **rearp) */
void q_insert();
	/* PARMS(Q_NODE **frontp, Q_NODE **rearp, Q_NODE *p, int (*cmpfn)()) */
Q_NODE **q_lfind();
	/* PARMS(Q_NODE **frontp, Q_NODE **rearp, Q_NODE *p, int (*cmpfn)()) */
Q_NODE *q_pop();	/* PARMS(Q_NODE **frontp, Q_NODE **rearp) */
void q_push();		/* PARMS(Q_NODE **frontp, Q_NODE **rearp, Q_NODE *p) */
Q_NODE *q_r_detach();
	/* PARMS(Q_NODE **frontp, Q_NODE **rearp, Q_NODE **pp) */
void q_r_insert();
	/* PARMS(Q_NODE **frontp, Q_NODE **rearp, Q_NODE *p, Q_NODE **pp) */

/* (more)
.PE
.PS 20
 */

#define Q_PP(p)		PNNC(p, Q_NODE **)
#define Q_P(p)		PNNC(p, Q_NODE *)

#define Q_APPEND(fp, rp, p)		q_append(Q_PP(fp), Q_PP(rp), Q_P(p))
#define Q_CLOSE(fp, rp)			q_close(Q_PP(fp), Q_PP(rp))
#define Q_INSERT(fp, rp, p, fn)	q_insert(Q_PP(fp), Q_PP(rp), Q_P(p), fn)
#define Q_LFIND(fp, rp, p, fn)	q_lfind(Q_PP(fp), Q_PP(rp), Q_P(p), fn)
#define Q_POP(fp, rp)			q_pop(Q_PP(fp), Q_PP(rp))
#define Q_PUSH(fp, rp, p)		q_push(Q_PP(fp), Q_PP(rp), Q_P(p))
#define Q_R_DETACH(fp, rp, pp)	q_r_detach(Q_PP(fp), Q_PP(rp), Q_PP(pp))
#define Q_R_INSERT(fp, rp, p, pp) q_r_insert(Q_PP(fp), Q_PP(rp), Q_P(p), Q_PP(pp))

#define Q_IS_EMPTY(front)       ((front) == NULL)
#define Q_OPEN(frontp, rearp)   (*(frontp) = *(rearp) = NULL)
#define Q_NEXT(p)               ((p)->next)
#define EACH_Q(front, p)        for ((p) = front; (p) != NULL; p = (p)->next)
#endif
###rdslib.h
#ifndef RDSLIB_H
#define RDSLIB_H
bool itoa();		/* PARMS(int n, char *str, int ndigits) */
int fgetsnn();		/* PARMS(char *str, int size, FILE *fp) */
int getsnn();		/* PARMS(char *str, int size) */
int getreply();		/* PARMS(char *prompt, char *reply, int size) */
bool getpstr();		/* PARMS(char *p, char *s, size_t n) */
bool getplin();		/* PARMS(char *p, char *s, size_t n) */
void plot_trk();	/* PARMS(int n, char c) */
void reverse();		/* PARMS(char *s) */
bool strfit();		/* PARMS(char *s1, char *s2, size_t n) */
#endif
###rec_io.h
/* rec_io.h - header for record file I/O functions */
#ifndef REC_IO_H
#define REC_IO_H
#include "local.h"
#include "bin_io.h"

#define REC_NFILE 		BIN_NFILE	/* adjust to local system */
#define REC_NEXT		(-1L)
#define REC_W_END		(-2L)
#define REC_NOT_FOUND	(-3L)	/* codes for hashed file access */
#define REC_FULL		(-4L)	/* codes for hashed file access */
#define REC_ERROR		(-5L)	/* codes for hashed file access */
#define REC_AVAIL		'a'		/* codes for hashed file access */
#define REC_OCCUP		'o'		/* codes for hashed file access */
#define REC_DELET		'd'		/* codes for hashed file access */
#define REC_FILE struct rec_file
REC_FILE
	{
	short _byte0;				/* offset of the first data byte in file */
	short _recsize;				/* size (in bytes) of each record */
	long _nrecs;				/* size (in records) of current file */
	bits _status;				/* open-modes of file */
	};
#define REC_BYTE0 BUFSIZ
#define rec_nrecs(fd)	(rec_files[fd]._nrecs + 0)
#define rec_recsize(fd) (rec_files[fd]._recsize + 0)

typedef int rec_fd;

extern int errno;
extern REC_FILE rec_files[REC_NFILE];

int rec_close();	/* PARMS(rec_fd fd) */
rec_fd rec_open();	/* PARMS(char fname[], int type, int recsize) */
bool rec_get();		/* PARMS(rec_fd fd, data_ptr buf, long i) */
bool rec_put();		/* PARMS(rec_fd fd, data_ptr buf, long i) */
long rec_hfind();
/* PARMS(rec_fd fd, data_ptr keybuf, data_ptr buf, int (*cmp)(), long (*hash)()) */
long rec_havail();
/* PARMS(rec_fd fd, data_ptr keybuf, data_ptr buf, long (*hash)()) */
#endif
###screen.h
/* screen.h - header for terminal package
 * Assumes ANSI terminal
 */
#ifndef SCREEN_H
#define SCREEN_H

typedef short SCR_CMDCHAR;
#define SCR_RETURN '\r'
#define SCR_EXIT   0x101
#define SCR_UP     0x102
#define SCR_DOWN   0x103
#define SCR_LEFT   0x104
#define SCR_RIGHT  0x105
#define SCR_HOME   0x106
#define SCR_CLEAR  "\33[2J"		/* clear entire screen, go HOME */
#define SCR_CEL    "\33[K"		/* clear to end of line */
#define SCR_TTY 1	/* screen is in ordinary (non-direct) mode */
#define SCR_RAW 2	/* screen is in "total-control" mode */

#define SCR_CMD(c) \
	(c == SCR_RETURN || c >= 0x101 && c <= 0x106)

extern short scr_mode;		/* {SCR_TTY, SCR_RAW} */
extern short scr_lins;		/* number of lines on screen */
extern short scr_cols;		/* number of columns on screen */

void scr_close();			/* PARMS(void) */
SCR_CMDCHAR scr_getkey();	/* PARMS(void) */
void scr_open();			/* PARMS(void) */

#define scr_beep()		putchar('\7')
#define scr_clear()		printf(SCR_CLEAR)
#define scr_cel()		printf(SCR_CEL)
#define scr_curs(r, c)	printf("\33[%d;%dH", (r)+1, (c)+1)
#define scr_print		printf
#define scr_putc(c)		putchar(c)
#define scr_refresh()	fflush(stdout)
#endif
###setjmp.h
int setjmp PARMS((jmp_buf env));
void longjmp PARMS((jmp_buf env, int val));
###stack.h
/* stack.h - header for stack package */
#ifndef STACK_H
#define STACK_H
#include "local.h"
#include "pointer.h"
#define ST_NODE struct st_node
ST_NODE
	{
	ST_NODE *next;
	/* ... */
	};

void st_close();			/* PARMS(ST_NODE **headp) */
ST_NODE *st_pop();			/* PARMS(ST_NODE **headp) */
void st_push();				/* PARMS(ST_NODE **headp, ST_NODE *p) */

#define ST_P(p)				PNNC(p, ST_NODE *)
#define ST_PP(p)			PNNC(p, ST_NODE **)

#define ST_CLOSE(h)			(st_close(ST_PP(h)))
#define ST_POP(h)			(st_pop(ST_PP(h)))
#define ST_PUSH(h, p)		(st_push(ST_PP(h), ST_P(p)))

#define EACH_ST(head, p)	for ((p) = (head); (p) != NULL; (p) = (p)->next)
#define ST_FIRST(head)		(head)
#define ST_IS_EMPTY(head)	((head) == NULL)
#define ST_OPEN(headp)		(*(headp) = NULL)
#define ST_NEXT(p)			((p)->next)
#endif
###stdlib.h
/* stdlib.h - miscellaneous library functions (partial listing) */
#ifndef STDLIB_H
#define STDLIB_H
#include "stddef.h"
double atof();		/* PARMS(char *s) */
int atoi();			/* PARMS(char *s) */
long atol();		/* PARMS(char *s) */
data_ptr calloc();	/* PARMS(unsigned int n, size_t size) */
void exit();		/* PARMS(int status) */
void free();		/* PARMS(data_ptr ptr) */
data_ptr malloc();	/* PARMS(size_t size) */
int rand();			/* PARMS(void) */
void srand();		/* PARMS(unsigned int seed) */
#endif
###task.h
/* task.h - header for task package */
#ifndef TASK_H
#define TASK_H
#include "local.h"
#include "queue.h"
typedef short TIME;
#define TASK struct task
TASK
	{
	TASK *next;
	TIME start;
	char desc[40];
	};
#endif
###time.h
struct tm {
	int tm_sec;		/* seconds {0:59} */
	int tm_min;		/* minutes {0:59} */
	int tm_hour;	/* hours {0:23} */
	int tm_mday;	/* day of the month {1:31} */
	int tm_mon;		/* month of the year {0:11} */
	int tm_year;	/* years since 1900 A.D. */
	int tm_wday;	/* days since previous Sunday {0:6} */
	int tm_yday;	/* day of the year {0:365} */
	int tm_isdst;	/* nonzero for daylight savings time */
	/* ... other members are allowed ... */
	};

typedef long time_t
typedef long clock_t;
#define CLK_TCK (1e6/60.)	/* 60 cycle clock */

char *asctime PARMS((const struct tm *timeptr));
clock_t clock PARMS((void));
char *ctime PARMS((time_t *timer));
double difftime PARMS((time_t timer1, time_t timer2));
struct tm *gmtime PARMS((const time_t *timer));               
struct tm *localtime PARMS((const time_t *timer));
time_t time PARMS((time_t *timer));

###time_day.h1
/* time_day.h - bit-field structure for  hh:mm:ss.fff */
#ifndef TIME_DAY_H
#define TIME_DAY_H
typedef struct time_day
	{
	unsigned h1 : 2;	/* tens digit of hours		{0:2} */
	unsigned h2 : 4;	/* units digit of hours		{0:9} */
	unsigned m1 : 3;	/* tens digit of minutes	{0:5} */
	unsigned m2 : 4;	/* units digit of minutes	{0:9} */
	unsigned s1 : 3;	/* tens digit of seconds	{0:5} */
	unsigned s2 : 4;	/* units digit of seconds	{0:9} */
	unsigned f1 : 4;	/* first digit of fraction	{0:9} */
	unsigned f2 : 4;	/* second digit of fraction	{0:9} */
	unsigned f3 : 4;	/* third digit of fraction	{0:9} */
	} TIME_DAY;		/* 32 bits total */
#endif
###time_day.h2
/* time_day.h - bit-field structure for  hh:mm:ss.fff */
#ifndef TIME_DAY_H
#define TIME_DAY_H
typedef struct _time_day
	{
	unsigned _h1 : 2;	/* tens digit of hours		{0:2} */
	unsigned _h2 : 4;	/* units digit of hours		{0:9} */
	unsigned _m1 : 3;	/* tens digit of minutes	{0:5} */
	unsigned _m2 : 4;	/* units digit of minutes	{0:9} */
	unsigned _s1 : 3;	/* tens digit of seconds	{0:5} */
	unsigned _s2 : 4;	/* units digit of seconds	{0:9} */
	unsigned _f1 : 4;	/* first digit of fraction	{0:9} */
	unsigned _f2 : 4;	/* second digit of fraction	{0:9} */
	unsigned _f3 : 4;	/* third digit of fraction	{0:9} */
	} TIME_DAY;		/* 32 bits total */
#endif
###tree.h
/* tree.h - header for tree functions
 */
#ifndef TREE_H
#define TREE_H
#include "local.h"
#include "pointer.h"
#define TR_NODE struct tr_node
TR_NODE
	{
	TR_NODE *right;
	TR_NODE *left;
	/* ... */
	};
void tr_close();		/* PARMS(TR_NODE **plt) */
void tr_delete();		/* PARMS(TR_NODE **plt, TR_NODE **pln, int (*cmpfn)()) */
TR_NODE *tr_detach();	/* PARMS(TR_NODE **pln) */
void tr_insert();		/* PARMS(TR_NODE **plt, TR_NODE *pn, int (*cmpfn)()) */
TR_NODE **tr_lfind();	/* PARMS(TR_NODE **plt, TR_NODE *pn, int (*cmpfn)()) */
TR_NODE **tr_lfirst();	/* PARMS(TR_NODE **plt) */
TR_NODE **tr_lpfind();	/* PARMS(TR_NODE **plt, TR_NODE *pn, int (*cmpfn)()) */
TR_NODE **tr_lnext();	/* PARMS(TR_NODE **plt, TR_NODE *pn, int (*cmpfn)()) */

#define TR_RIGHT(p)		((p)->right)
#define TR_LEFT(p)		((p)->left)

#define TR_P(p)			PNNC(p, TR_NODE *)
#define TR_PP(p)		PNNC(p, TR_NODE **)

#define TR_CLOSE(plt)			tr_close(TR_PP(plt))
#define TR_DELETE(plt, pln, fn)	tr_delete(TR_PP(plt), TR_PP(pln), fn)
#define TR_DETACH(pln)			tr_detach(TR_PP(pln))
#define TR_INSERT(plt, pn, fn)	tr_insert(TR_PP(plt), TR_P(pn), fn)
#define TR_LFIND(plt, pn, fn)	tr_lfind(TR_PP(plt), TR_P(pn), fn)
#define TR_LFIRST(plt)			tr_lfirst(TR_PP(plt))
#define TR_LPFIND(plt, pn, fn)	tr_lpfind(TR_PP(plt), TR_P(pn), fn)
#define TR_LNEXT(plt, pn, fn)	tr_lnext(TR_PP(plt), TR_P(pn), fn)

/* true macros */
#define TR_OPEN(plt)			(*(plt) = NULL)
#define TR_FIRST(plt)			(*TR_LFIRST(plt))
#define TR_FIND(plt, pn, fn)	(*TR_LFIND(plt, pn, fn))
#define TR_NEXT(plt, pn, fn)	(*TR_LNEXT(plt, pn, fn))
#define EACH_TR(plt, p, t, fn) \
	for (p = (t *)*TR_LFIRST(plt); (p) != NULL; p = (t *)*TR_LNEXT(plt, p, fn))
#endif
###wsl_bin_io.h
/* bin_io.h - header for binary file I/O functions */
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
###EOF
