###acd_menu.c
/* acd_menu - PART MENU */
#include "local.h"
#include "menu.h"
#include "part.h"
extern bool addpart();
extern bool chgpart();
extern bool delpart();
static FIELD Facd_menu[] =
	{
	{2, 0, "Add a part record", 'f', NULL, NULL, NULL, addpart},
	{4, 0, "Change a part record", 'f', NULL, NULL, NULL, chgpart},
	{6, 0, "Delete a part record", 'f', NULL, NULL, NULL, delpart},
	};
MENU acd_menu = {"PART MENU", Facd_menu, DIM(Facd_menu), 0};
###acd_menu.mu
Menu name:  acd_menu    Menu title: PART MENU
	Header: part.h		C-struct: part1

Field name: 0     Desc: Add a part record
	Line: 2 Col: 0	 Type: f	 		 Post-fn: addpart

Field name: 0     Desc: Change a part record
	Line: 4 Col: 0	 Type: f	 		 Post-fn: chgpart

Field name: 0     Desc: Delete a part record
	Line: 6 Col: 0	 Type: f	 		 Post-fn: delpart
###add_menu.c
/* add_menu - ADD A PART RECORD */
#include "local.h"
#include "menu.h"
#include "part.h"
extern bool isn_part();
static STRING_VAL Spart_no =
	{'a', (part1).part_no, sizeof((part1).part_no), NULL};
static STRING_VAL Slead_time =
	{'n', (part1).lead_time, sizeof((part1).lead_time), NULL};
static char *Tunit_meas[]=
	
	{
	"each", "lb", "box",
	};
static CHOICE_VAL Cunit_meas =
	{DIM(Tunit_meas), Tunit_meas, (part1).unit_meas};
static STRING_VAL Sunit_cost =
	{'a', (part1).unit_cost, sizeof((part1).unit_cost), NULL};
static STRING_VAL Scost_qty =
	{'n', (part1).cost_qty, sizeof((part1).cost_qty), NULL};
static FIELD Fadd_menu[] =
	{
	{2, 0, "Part number", 'S', &Spart_no, NULL, NULL, isn_part},
	{4, 0, "Lead time (in weeks)", 's', &Slead_time, NULL, NULL, 0},
	{6, 0, "Unit of measure", 'c', NULL, &Cunit_meas, NULL, 0},
	{8, 0, "Cost per unit", 's', &Sunit_cost, NULL, NULL, 0},
	{10, 0, "Qty required for price", 's', &Scost_qty, NULL, NULL, 0},
	};
MENU add_menu = {"ADD A PART RECORD", Fadd_menu, DIM(Fadd_menu), 0};
###add_menu.mu
Menu name: add_menu    Menu title: ADD A PART RECORD
	Header: part.h		C-struct: part1

Field name: part_no    Desc: Part number
	Line: 2 Col: 0	 Type: S	 Edit: a	 Post-fn: isn_part	

Field name: lead_time    Desc: Lead time (in weeks)
	Line: 4 Col: 0	 Type: s	 Edit: n	 Post-fn: 0	

Field name: unit_meas    Desc: Unit of measure
	Line: 6 Col: 0	 Type: c	 Post-fn: 0	
	{
	"each", "lb", "box",
	};

Field name: unit_cost    Desc: Cost per unit
	Line: 8 Col: 0	 Type: s	 Edit: a	 Post-fn: 0	

Field name: cost_qty    Desc: Qty required for price
	Line: 10 Col: 0	 Type: s	 Edit: n	 Post-fn: 0	
###args.c
/* args - show command-line arguments, one line each
 */
#include "local.h"
main(ac, av)
	int ac;
	char *av[];
	{
	int i;

	for (i = 0; i < ac; ++i)
		printf("av[%d]=<%s>\n", i, av[i]);
	}
###atopi(#1).c
/* atopi - convert string to positive integer */
int atopi(s)
	char s[];
	{
	int n = 0;
	int i;

	for (i = 0; isdigit(s[i]); ++i)
		n = 10 * n + s[i] - '0';
	return (n);
	}
###atopi(#2).c
/* atopi - convert string to positive integer {0:INT_MAX} */
#include <limits.h>
int atopi(s)
	char s[];
	{
	unsigned n = 0;	/* the converted number: {0:INT_MAX} */
	int i;

	for (i = 0; isdigit(s[i]) && n <= INT_MAX/10; ++i)
		{
		if (n > INT_MAX / 10)
			return (-1);
		n = 10 * n + (s[i] - '0');
		}
	if (n > INT_MAX || isdigit(s[i]))
		return (-1);
	else
		return (n);
	}
###chg_menu.c
/* chg_menu - CHANGE A PART RECORD */
#include "local.h"
#include "menu.h"
#include "part.h"
extern bool is_part();
static STRING_VAL Spart_no =
	{'a', (part1).part_no, sizeof((part1).part_no), NULL};
static STRING_VAL Slead_time =
	{'n', (part1).lead_time, sizeof((part1).lead_time), NULL};
static char *Tunit_meas[]=
	{
	"each", "lb", "box",
	};
static CHOICE_VAL Cunit_meas =
	{DIM(Tunit_meas), Tunit_meas, (part1).unit_meas};
static STRING_VAL Sunit_cost =
	{'a', (part1).unit_cost, sizeof((part1).unit_cost), NULL};
static STRING_VAL Scost_qty =
	{'n', (part1).cost_qty, sizeof((part1).cost_qty), NULL};
static FIELD Fchg_menu[] =
	{
	{2, 0, "Part number", 'S', &Spart_no, NULL, NULL, is_part},
	{4, 0, "Lead time (in weeks)", 's', &Slead_time, NULL, NULL, 0},
	{6, 0, "Unit of measure", 'c', NULL, &Cunit_meas, NULL, 0},
	{8, 0, "Cost per unit", 's', &Sunit_cost, NULL, NULL, 0},
	{10, 0, "Qty required for price", 's', &Scost_qty, NULL, NULL, 0},
	};
MENU chg_menu = {"CHANGE A PART RECORD", Fchg_menu, DIM(Fchg_menu), 0};
###chg_menu.mu
Menu name: chg_menu    Menu title: CHANGE A PART RECORD
	Header: part.h		C-struct: part1	

Field name: part_no    Desc: Part number
	Line: 2 Col: 0	 Type: S	 Edit: a	 Post-fn: is_part

Field name: lead_time    Desc: Lead time (in weeks)
	Line: 4 Col: 0	 Type: s	 Edit: n	 Post-fn: 0

Field name: unit_meas    Desc: Unit of measure
	Line: 6 Col: 0	 Type: c	 		 Post-fn: 0
	{
	"each", "lb", "box",
	};

Field name: unit_cost    Desc: Cost per unit
	Line: 8 Col: 0	 Type: s	 Edit: a	 Post-fn: 0

Field name: cost_qty    Desc: Qty required for price
	Line: 10 Col: 0	 Type: s	 Edit: n	 Post-fn: 0
###cpystr.c
/* cpystr - copy several source strings into one destination string
 * NOT UNIVERSALLY PORTABLE: assumes argument stack layout
 */
#include "local.h"
char *cpystr(olddest, s)
	char *olddest;
	char *s;
	{
	char **ps;
	register char *dest = olddest;
	register char *src;

	ps = &s + 1;	/* no range limits! */
	for (src = s; src != NULL; src = *ps++)
		while (*src != '\0')
			*dest++ = *src++;
	*dest = '\0';
	return (olddest);
	}
###cpystrm.c
#include "cpystr.c"
main()
	{
	char buf[BUFSIZ];

	cpystr(buf, "ab", "c", "de", NULL);
	printf("<%s>\n", buf);
	}
###crhash.c
/* crhash -- create and initialize an empty hash file
 *		used to hold part database records
 */
#include "local.h"
#include "part.h"
#include "rec_io.h"

main(ac, av)
	int ac;
	char *av[];
	{
	rec_fd fd;			/* record file descriptor */
	short size;			/* size of one record in created file */
	long nrecs;			/* maximum numbers of records in created file */
	char *buf;			/* data to init. each record */
	long i;				/* counter to write records in created file */

	if (ac != 4)
		error("usage: crhash filename nrecs recsize", "");

	nrecs = atol(av[2]);
	size = atoi(av[3]);
	fd = rec_open(av[1], O_RDONLY, size);
	if (fd > 0)
		{
		rec_close(fd);
		error("file already exists:", av[1]);
		}

	fd = rec_open(av[1], O_RDWR|O_CREAT, size);
	if (fd < 0)
		error("can't create file", av[1]);

	buf = (char *)calloc(size, 1);
	if (buf == NULL)
		error("Need more heap space to allow size =", av[3]);
	*buf = REC_AVAIL;
	/* file was created ok, initialize file */
	for (i = 0; i < nrecs; ++i)
		{
		if (!rec_put(fd, buf, REC_W_END))
			error("I/O error writing new file", "");
		}
	rec_close(fd);
	exit(SUCCEED);
	}
###dangling.c
/* dangling - example of dangling pointer
 */
#include "local.h"
static short *pi = NULL;
main()
	{
	void f1();

				/* pi => NULL initially */
	f1();		/* pi => undefined suddenly! */
	}
void f1()
	{
	short i;

	pi = &i;	/* pi => complete,  momentarily */
	}
###del_menu.c
/* del_menu - DELETE A PART RECORD */
#include "local.h"
#include "menu.h"
#include "part.h"
extern bool is_part();
static STRING_VAL Spart_no =
	{'a', (part1).part_no, sizeof((part1).part_no), NULL};
static FIELD Fdel_menu[] =
	{
	{2, 0, "Part number", 'S', &Spart_no, NULL, NULL, is_part},
	};
MENU del_menu = {"DELETE A PART RECORD", Fdel_menu, DIM(Fdel_menu), 0};
###del_menu.mu
Menu name: del_menu    Menu title: DELETE A PART RECORD
	Header: part.h		C-struct: part1

Field name: part_no    Desc: Part number
	Line: 2 Col: 0	 Type: S	 Edit: a	 Post-fn: is_part

###dq_close.c
/* dq_close - close a deque  */
#include "deque.h"
void dq_close(pdq)
	DQ_NODE **pdq;	/* ptr to ptr to master DQ_NODE */
	{
	DQ_NODE *p;
	DQ_NODE *pnext;

	for (p = (*pdq)->right; p != (*pdq); p = pnext)
		{
		pnext = p->right;
		free(p);
		}
	free(*pdq);
	*pdq = NULL;
	}
###dq_detach.c
/* dq_detach - detach node d from deque */
#include "deque.h"
DQ_NODE *dq_detach(dq, d)
	DQ_NODE *dq;
	DQ_NODE *d;
	{
	DQ_NODE *p;
	DQ_NODE *q;

	if (d == dq)
		return (NULL);
	q = d->right;
	p = d->left;
	p->right = q;
	q->left = p;
	return (d);
	}
###dq_find.c
/* dq_find - search deque for equal match to d, return ptr to match
 * Return NULL if no match
 */
#include "deque.h"
DQ_NODE *dq_find(dq, d, cmpfn)
	DQ_NODE *dq;
	DQ_NODE *d;
	int (*cmpfn)();						/*function for comparisons */
	{
	DQ_NODE *d2;

	EACH_DQ(dq, d2, DQ_NODE)
		if ((*cmpfn)(d2, d) == 0)
			return (d2);
	return (NULL);
	}
###dq_first.c
/* dq_first - produce ptr to first deque node, otherwise NULL */
#include "deque.h"
DQ_NODE *dq_first(dq)
	DQ_NODE *dq;
	{
	if (dq->right == dq)
		return (NULL);
	return (dq->right);
	}
###dq_insert.c
/* dq_insert - install DQ_NODE at proper place in deque */
#include "deque.h"
void dq_insert(dq, p, cmpfn)
	DQ_NODE *dq;
	DQ_NODE *p;
	int (*cmpfn)();		/* function for comparisons: neg, zero, or pos */
	{
	DQ_NODE *p2;

	if (DQ_IS_EMPTY(dq)				/* if deque was empty, */
		|| (*cmpfn)(p, DQ_FIRST(dq)) < 0) /* or if p comes first, */
		DQ_PUSH(dq, p);				/* push p onto front */
	else							/* else, p must sort after p2 */
		{
		EACH_DQ(dq, p2, DQ_NODE)	/* find where p belongs */
			{
			if (p2->right == dq)
				{ 					/* if end of deque, */
				DQ_APPEND(dq, p);	/* append p at rear of deque */
				break;
				}
			else if ((*cmpfn)(p, p2->right) < 0)
				{ 					/* if p sorts before p2->right */
				DQ_R_INSERT(p, p2);	/* insert after p2 */
				break;
				}
			}
		}
	}
###dq_main.c
/* dq_main - test routine for deque package
 */
#include "local.h"
#include "deque.h"
DQ_NODE *dq = NULL;
#define MYNODE struct mynode
MYNODE
	{
	MYNODE *left;
	MYNODE *right;
	int data;
	};
#define LINESIZE 20
main()
	{
	MYNODE *p;			/* current node */
	char buf[LINESIZE];	/* buffer for input */

	DQ_OPEN(&dq);
	printf("Type < to push (insert at left end of deque)\n");
	printf("Type > to enque (insert at right end of deque)\n");
	printf("Type - to pop (remove leftmost node)\n");
	printf("Type = to print contents of deque\n");
	printf("Type 0 to reset deque\n");
	for (;;)
		{
		getreply("?: ", buf, 2);
		switch (buf[0])
			{
		case '<':
			p = (MYNODE *)malloc(sizeof(MYNODE));
			if (p == NULL)
				error("out of space", "");
			getreply("data: ", buf, 5);	/* bound to 4 digits */
			p->data = atoi(buf);
			DQ_PUSH(dq, p);
			break;
		case '>':
			p = (MYNODE *)malloc(sizeof(MYNODE));
			if (p == NULL)
				error("out of space", "");
			getreply("data: ", buf, 5);	/* bound to 4 digits */
			p->data = atoi(buf);
			DQ_APPEND(dq, p);
			break;
		case '-':
			p = (MYNODE *)DQ_POP(dq);
			if (p == NULL)
				printf("EMPTY DEQUE\n");
			else
				{
				printf("data=%6d\n", p->data);
				free((data_ptr)p);
				}
			break;
		case '=':
			if (DQ_IS_EMPTY(dq))
				printf("EMPTY DEQUE\n");
			else
				EACH_DQ(dq, p, MYNODE)
					printf("data=%6d\n", p->data);
			break;
		case '0':
			DQ_CLOSE(&dq);
			break;
		default:
			printf("unknown command: %c\n", buf[0]);
			break;
			}
		}
	}
###dq_open.c
/* dq_open - open a deque */
#include "deque.h"
void dq_open(pdq)
	DQ_NODE **pdq;
	{
	*pdq = (DQ_NODE *)malloc(sizeof(DQ_NODE));
	if (*pdq == NULL)
		return;
	(*pdq)->left = (*pdq)->right = (*pdq);
	}
###dq_pop.c
/* dq_pop - remove leftmost node and return pointer to it
 * Treats deque as a stack to the right of dq master node.
 */
#include "deque.h"
DQ_NODE *dq_pop(dq)
	DQ_NODE *dq;
	{
	DQ_NODE *d;

	d = dq->right;
	if (d == dq)
		return (NULL);
	return (DQ_DETACH(dq, d));
	}
###dq_r_insert.c
/* dq_r_insert - insert node d to the right of node p */
#include "deque.h"
void dq_r_insert(d, p)
	DQ_NODE *d;
	DQ_NODE *p;
	{
	DQ_NODE *q;

	q = p->right;
	d->left = p;
	d->right = q;
	p->right = d;
	q->left = d;
	}
###dump.c
/* dump - print memory bytes
 *		(Warning - some usages may be non-portable)
 */
#include "local.h"
#define LINESIZE 16
#define BYTEMASK 0xFF
void dump(s, n)
	char s[];		/* byte address to be dumped */
	unsigned n;		/* number of bytes to dump */
	{
	unsigned i;		/* byte counter */

	for (i = 0; i < n; ++i)
		{
		if (i % LINESIZE == 0)
			printf("\n%08x: ", &s[i]);
		printf(" %02x", s[i] & BYTEMASK);
		}
	printf("\n");
	}
###errno.c
int errno = 0;
###error.c
/* error - print error message and exit */
#include <stdio.h>
error(s1, s2)
	char s1[], s2[];
	{
	fprintf(stderr, "%s %s\n", s1, s2);
	fflush(stderr);
	exit(1);
	}
###fgetsnn.c
/* fgetsnn - get one line, omitting the newline (if any)
 * Returns the number of characters stored,
 * which equals size-1 only if no newline was found to that point.
 * Returns EOF at end of file.
 */
#include "local.h"
int fgetsnn(s, size, fp)
	char s[];
	int size;
	FILE *fp;
	{
	int i;
	metachar c;

	for (i = 0; i < size-1 && (c = getc(fp)) != EOF && c != '\n'; ++i)
		s[i] = c;
	if (c == EOF)
		return (EOF);
	/* else */
	s[i] = '\0';
	return (i);
	}
###gen_menu.c
/* gen_menu - generate C source for menu from description file */
#include "local.h"
#define MAXBFLDS 8000
#define C_ID_LEN 31
#define FILE_NM_LEN 64		/* max length of file name */
#define C_EXPR_LEN 80		/* arbitrarly limit on length of struct ref */
#define TITLE_LEN 80		/* max length of displayed title */
#define SCR_POS_LEN 3		/* max digits in screen position */
#define FTYPE_LEN 1			/* field type is only one char */
#define EDIT_LEN 2			/* edit type is 1 char plus 1 optional digit */
#define MAXFLINE (TITLE_LEN+FTYPE_LEN+EDIT_LEN+SCR_POS_LEN+SCR_POS_LEN+50)
#define GETPSTR(p, s)  \
	{  if (!getpstr(p, s, sizeof(s)))  error("Cannot match" , p);  }
#define OPTPSTR(p, s)  \
	getpstr(p, s, sizeof(s))
#define GETPLIN(p, s)  \
	{  if (!getplin(p, s, sizeof(s)))  error("Cannot match" , p);  }																					                                                                         /*
.PE
.PS 17
/* internal variables */
static char buf[BUFSIZ] = {""};
static char buf_flds[MAXBFLDS] = {""};
static short i_flds = 0;
static char mname[C_ID_LEN+1] = {""};
static char mstruct[C_EXPR_LEN+1] = {""};
static char mtitle[TITLE_LEN+1] = {""};
static char mheader[FILE_NM_LEN+1] = {""};
static char fname[C_ID_LEN+1] = {""};
static char fline[SCR_POS_LEN+1] = {""};
static char fcol[SCR_POS_LEN+1] = {""};
static char ffn[C_ID_LEN+1] = {""};
static char ftype[FTYPE_LEN+1] = {""};
static char fedit[EDIT_LEN+1] = {""};
static char fdesc[TITLE_LEN+1] = {""};
void do_1_menu(), do_1_fld(), pr_s_fld(), pr_c_fld(), pr_m_fld(), pr_f_fld();																					                                                                         /*
.PE
.PS 15
/* gen_menu (main) */
void main()
	{
	do_1_menu();
	while (OPTPSTR("Field name:", fname))
		do_1_fld();		/* process one field */
	printf("static FIELD F%s[] =\n", mname);
	printf("\t{\n");
	for (i_flds = 0; buf_flds[i_flds] != '\0'; ++i_flds)
		putchar(buf_flds[i_flds]);
	printf("\t};\n");
	printf("MENU %s = {\"%s\", F%s, DIM(F%s), 0};\n",
		mname, mtitle, mname, mname);
	}																					                                                                         /*
.PE
.PS 13
/* do_1_menu - process the menu information */
void do_1_menu()
	{
	GETPSTR("Menu name:", mname);
	GETPLIN("Menu title:", mtitle);
	GETPSTR("Header:", mheader);
	GETPSTR("C-struct:", mstruct);
	printf("/* %s - %s */\n", mname, mtitle);
	printf("#include \"local.h\"\n");
	printf("#include \"menu.h\"\n");
	printf("#include \"%s\"\n", mheader);
	}																					                                                                         /*
.PE
.PS 33
/* do_1_fld - process the info for one field */
void do_1_fld()
	{
	asserts(IN_RANGE(i_flds, 0, MAXBFLDS - MAXFLINE), "buffer space ok"); 
	GETPLIN("Desc:", fdesc);
	GETPSTR("Line:", fline);
	GETPSTR("Col:", fcol);
	GETPSTR("Type:", ftype);
	if (tolower(ftype[0]) == 's')
		GETPSTR("Edit:", fedit);
	if (ftype[0] != 'm')
		GETPSTR("Post-fn:", ffn);
	if (!STREQ(ffn, "0") && !STREQ(ffn, "NULL"))
		printf("extern bool %s();\n", ffn);
	switch (ftype[0])
		{
	case 's':
	case 'S':
		pr_s_fld();
		break;
	case 'c':
		pr_c_fld();
		break;
	case 'm':
		pr_m_fld();
		break;
	case 'f':
		pr_f_fld();
		break;
	default:
		error("Unknown field type", ftype);
		}
	i_flds += strlen(&buf_flds[i_flds]);
	if (i_flds > MAXBFLDS - MAXFLINE)
		error("out of buffer space", "");
	}																					                                                                         /*
.PE
.PS 22
/* pr_s_fld - print the info for a string field */
void pr_s_fld()
	{
	sprintf(&buf_flds[i_flds],
		"\t{%s, %s, \"%s\", '%c', &S%s, NULL, NULL, %s},\n",
		fline, fcol, fdesc, ftype[0], fname, ffn);
	if (fedit[1] == '\0')
		{
		printf("static STRING_VAL S%s =\n", fname);
		printf("\t{'%c', (%s).%s, sizeof((%s).%s), NULL};\n",
			fedit[0], mstruct, fname, mstruct, fname);
		}
	else
		{
		printf("static char N%s[%c+1] = {0};\n",
			fname, fedit[1]);
		printf("static STRING_VAL S%s =\n", fname);
		printf("\t{'%c', N%s, sizeof(N%s), &(%s).%s};\n",
			fedit[0], fname, fname, mstruct, fname);
		}
	}																					                                                                         /*
.PE
.PS 18
/* pr_c_fld - print the info for a choice field */
void pr_c_fld()
	{
	int ret;

	sprintf(&buf_flds[i_flds],
		"\t{%s, %s, \"%s\", '%c', NULL, &C%s, NULL, %s},\n",
		fline, fcol, fdesc, ftype[0], fname, ffn);
	printf("static char *T%s[]=\n", fname);
	do	{
		ret = getsnn(buf, BUFSIZ);
		if (ret != EOF && ret > 0)	/* skip blank lines */
			printf("%s\n", buf);
		} while (ret != EOF && strchr(buf, ';') == NULL);
	printf("static CHOICE_VAL C%s =\n", fname);
	printf("\t{DIM(T%s), T%s, (%s).%s};\n",
		fname, fname, mstruct, fname);
	}																					                                                                         /*
.PE
.PS 15
/* pr_m_fld - print the info for a menu field */
void pr_m_fld()
	{
	printf("extern MENU %s;\n", fname);
	sprintf(&buf_flds[i_flds],
		"\t{%s, %s, \"%s\", '%c', NULL, NULL, &%s, %s},\n",
		fline, fcol, fdesc, ftype[0], fname, ffn);
	}
/* pr_f_fld - print the info for a function field */
void pr_f_fld()
	{
	sprintf(&buf_flds[i_flds],
		"\t{%s, %s, \"%s\", '%c', NULL, NULL, NULL, %s},\n",
		fline, fcol, fdesc, ftype[0], ffn);
	}
###getplin.c
/* getplin - get a line from input
 * line must start with specified "prompt" text
 * return YES if text matched, NO otherwise
 */
#include "local.h"
#define FMTSIZE 84
bool getplin(p, s, n)
	char p[];
	char s[];
	size_t n;
	{
	char buf[BUFSIZ];
	char fmt[FMTSIZE];
	metachar c;

	if (!strfit(fmt, p, FMTSIZE - 4))
		return (NO);	/* prompt text is too long */
	strcat(fmt, "%c");
	while (isspace(c = getchar()))
		;
	ungetc(c, stdin);
	if (scanf(fmt, buf) != 1)
		return (NO);
	if (getsnn(buf, BUFSIZ) == EOF)
		return (NO);
	if (!strfit(s, buf, n))
		fprintf(stderr, "<%s> chopped to \n<%s>\n", buf, s);
	return (YES);
	}
###getpstr.c
/* getpstr - get a string from input
 * string must be preceded by specified "prompt" text
 * return YES if text matched, NO otherwise
 */
#include "local.h"
#define FMTSIZE 84	/* arbitrary limit on size of "prompt" */

bool getpstr(p, s, n)
	char p[];				/* : !NULL */
	char s[];				/* : string */
	size_t n;				/* : {1:sizeof(p[*])} */
	{
	char buf[BUFSIZ];		/* : string */
	char fmt[FMTSIZE];		/* : string */
	metachar c;

	if (!strfit(fmt, p, FMTSIZE - 4))	/* copy "prompt" into fmt */
		return (NO);		/* prompt text is too long */
	strcat(fmt, "%s");		/* make fmt into a scanf format */
	while (isspace(c = getchar()))
		;					/* skip leading whitespace */
	ungetc(c, stdin);		/* put the non-whitespace back */
	if (scanf(fmt, buf) != 1)
		return (NO);		/* scanf match failed */
	if (!strfit(s, buf, n))
		fprintf(stderr, "<%s> chopped to <%s>\n", buf, s);
	return (YES);
	}
###getreply.c
/* getreply - print a prompt and get one-line reply
 * Returns length of reply string, or EOF if end-of-file
 * Consumes an entire input line, even if over-long.
 */
#include "local.h"
int getreply(prompt, reply, size)
	char prompt[];
	char reply[];
	int size;
	{
	metachar last_char;			/* the last char read */
	int get_ret;				/* getsnn return: length of reply or EOF */

	printf("%s", prompt);
	get_ret = getsnn(reply, size);
	if (get_ret == EOF)
		return (EOF);
	else if (get_ret == size-1)	/* no newline was read */
		while ((last_char = getchar()) != EOF && last_char != '\n')
			;
	if (last_char == EOF)
		return (EOF);
	return (get_ret);
	}
###getsnn.c
/* getsnn - get one line, omitting the newline (if any)
 * Returns the number of characters stored,
 * which equals size-1 only if no newline was found to that point.
 * Returns EOF at end of file.
 */
#include "local.h"
int getsnn(s, size)
	char s[];
	int size;
	{
	int i;
	metachar c;

	for (i = 0; i < size-1 && (c = getchar()) != EOF && c != '\n'; ++i)
		s[i] = c;
	s[i] = '\0';
	if (c == EOF)
		return (EOF);
	return (i);
	}
###isort.c
/* isort - sort array a (dimension n) using insertion sort
 */
#include "local.h"
void isort(a, n)
	short a[];	/* array to be sorted; a[0:n-1]: multiset */
	index_t n;	/* dimension of a */
	{
	index_t i;
	index_t j;
	short t;

	for (i = 1; i < n; ++i)
		{
		/* a[0:i-1] => sorted */
		t = a[i];
		for (j = i; j > 0 && a[j-1] > t; --j)
			a[j] = a[j-1];		/* a[0:n-1] => !multiset */
		a[j] = t;				/* a[0:n-1] => multiset */
		}
	/* a[0:n-1] => sorted */
	}
###isort1.c
/* isort - sort array a (dimension n) using insertion sort
 */
#include "local.h"
void isort(a, n)
	int a[];
	int n;
	{
	int i;
	int j;
	int t;

	for (i = 1; i < n; ++i)	/* a[0:i-1]=>sorted */
		{
		t = a[i];
		j = i;
		while (j > 0 && a[j-1] > t)
			{
			a[j] = a[j-1];
			--j;
			}
		a[j] = t;
		}
	/* a=>sorted */
	}
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static int a[10000];
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
	cputim();
	isort(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	printf("N**2 = %.0f\n", (double)lim * lim);
	printf("cpu time = %.2f N**2 (usec)\n", tim / ((double)lim * lim));
	for (i = 0; i < lim-1; ++i)
		{
		if (a[i] > a[i+1])
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###isort2.c
/* isort - sort array a (dimension n) using insertion sort
 */
#include "local.h"
void isort(a, n)
	int a[];
	int n;
	{
	int i;
	int *pj;
	int *pjm;
	int t;

	for (i = 1; i < n; ++i)	/* a[0:i-1]=>sorted */
		{
		t = a[i];
		pj = &a[i];
		pjm = pj - 1;
		while (pj > a && *pjm > t)
			{
			*pj = *pjm;
			--pj;
			--pjm;
			}
		*pj = t;
		}
	/* a=>sorted */
	}
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static int a[10000];
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
	cputim();
	isort(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	printf("N**2 = %.0f\n", (double)lim * lim);
	printf("cpu time = %.2f N**2 (usec)\n", tim / ((double)lim * lim));
	for (i = 0; i < lim-1; ++i)
		{
		if (a[i] > a[i+1])
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###isort3.c
/* isort - sort array a (dimension n) using insertion sort
 */
#include "local.h"
void isort(a, n)
	int a[];
	int n;
	{
	int i;
	register int *pj;
	register int *pjm;
	int t;

	for (i = 1; i < n; ++i)	/* a[0:i-1]=>sorted */
		{
		t = a[i];
		pj = &a[i];
		pjm = pj - 1;
		while (pj > a && *pjm > t)
			{
			*pj = *pjm;
			--pj;
			--pjm;
			}
		*pj = t;
		}
	/* a=>sorted */
	}
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static int a[10000];
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
	cputim();
	isort(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	printf("N**2 = %.0f\n", (double)lim * lim);
	printf("cpu time = %.2f N**2 (usec)\n", tim / ((double)lim * lim));
	for (i = 0; i < lim-1; ++i)
		{
		if (a[i] > a[i+1])
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###isortp.c
/* isort - sort array a (dimension n) using insertion sort
 */
#include "local.h"
void isort(a, n)
	int a[];
	int n;
	{
	int i;
	int *pj;
	int *pjm;
	int t;

	for (i = 1; i < n; ++i)	/* a[0:i-1]=>sorted */
		{
		t = a[i];
		pj = &a[i];
		pjm = pj - 1;
		while (pj > a && *pjm > t)
			{
			*pj = *pjm;
			--pj;
			--pjm;
			}
		*pj = t;
		}
	/* a=>sorted */
	}
###itoa.c
/* itoa - convert integer n to string (ndigits max) */
#include "local.h"
bool itoa(n, str, ndigits)
	int n;
	char str[];
	int ndigits;
	{
	int sign;
	short i;
	bool success;
	unsigned un = n;	/* need room for most-negative n */

	success = YES;
	if (ndigits < 1 || n < 0 && ndigits < 2)
		return (NO);		/* can't even get started */
	else if (n == 0)
		{
		strcpy(str, "0");	/* dispose of a special case */
		return (YES);
		}
	else if (n < 0)
		{
		sign = -1;
		un = -n;
		--ndigits;			/* sign consumes one print digit */
		}
	else
		sign = 1;
	for (i = 0; i < ndigits && un != 0; ++i)
		{
		str[i] = '0' + un % 10;
		un /= 10;
		}
	if (un != 0)		/* still more digits to print, and no room */
		{
		for (i = 0; i < ndigits; ++i)
			str[i] = '9';
		success = NO;
		}
	if (sign < 0)
		str[i++] = '-';
	str[i] = '\0';
	reverse(str);
	return (success);
	}
###itoa_demo.c
/* itoa_demo - demonstrate itoa */
#include "local.h"
#define TEST(n, str, ndigits) \
	{ \
	printf("%8d %4d %8d ", n, ndigits, itoa(n, str, ndigits)); \
	printf("<%s>\n", str); \
	}
main()
	{
	char str[20];

	TEST(0, str, 1);
	TEST(0, str, 0);
	TEST(-1, str, 1);
	TEST(-1, str, 2);
	TEST(100, str, 2);
	TEST(10000, str, 5);
	TEST(10000, str, 6);
	TEST(-10000, str, 6);
	}
###memcpy.c
/* memcpy - copy n bytes from b to a */
#include "local.h"
data_ptr memcpy(s1, s2, n)
	data_ptr s1;
	data_ptr s2;
	register size_t n;
	{
	register char *a = s1;
	register char *b = s2;

	for (; n > 0; --n, ++a, ++b)
		*a = *b;
	return (s1);
	}
###mu_ask.c
/* mu_ask - get responses to menu items
 * Keeps looping over string and choice fields;
 * terminates when user selects an action field, or hits SCR_EXIT.
 */
#include "menu.h"
FIELD *mu_ask(pm)
	MENU *pm;		/* ptr to current MENU */
	{
	FIELD *pf;		/* ptr to current FIELD */
	FIELD *pfj;		/* ptr to j-th FIELD of this MENU */
	SCR_CMDCHAR c;	/* most recent user key input */
	char vtype;		/* value type of current FIELD */
	bool legal;		/* is  c  valid input in this state? */
	short j;		/* index over FIELDs of this MENU */

	pm->cur_field = 0;		/* start with first field */
	FOREVER
		{
		pf = &pm->fields[pm->cur_field];
		legal = YES;
		vtype = pf->val_type;

		/* accept user input, get user's SCR_CMDCHAR */
		if (vtype == 'c' || tolower(vtype) == 's')
			{
			if (vtype == 'c')
				c = mu_chv(pf);
			else			/* vtype == 's' OR 'S' */
				c = mu_str(pf);
			if (pf->pfn != NULL)
				legal = (*pf->pfn)(pm);
			if (c == SCR_EXIT)
				return (NULL);
			}
		else
			{
			scr_curs(pf->line, pf->col);
			c = scr_getkey();
			}
		/* at this point, c is the user's response, and */
		/* legal records validity of 'c' or 's' field */
		/* (more) */																/*
.PE
.PS  46
		/* determine next action, based on c */
		if (c == SCR_RETURN && (vtype == 'm' || vtype == 'f'))
			return (pf);
		else if (c == SCR_EXIT)
			return (NULL);
		else if (!legal)
			;		/* no actions allowed if validation failed */
		else if (vtype == 'S' && pf->psv->string[0] == '\0')
			legal = NO;
		else if (!SCR_CMD(c) && tolower(vtype) != 's')
			{
			legal = NO;	/* tentatively */
			for (j = 0; j < pm->nfields; ++j)
				{
				pfj = &pm->fields[j];
				if (toupper(pfj->desc[0]) == toupper(c))
					{
					pm->cur_field = j;
					legal = YES;
					break;
					}
				}
			}
		else if (SCR_CMD(c))
			{
			switch (c)
				{
			case SCR_UP:
				pm->cur_field = IMOD(pm->cur_field - 1, pm->nfields);
				break;
			case SCR_DOWN:
			case SCR_RETURN:
				pm->cur_field = IMOD(pm->cur_field + 1, pm->nfields);
				break;
			case SCR_HOME:
				pm->cur_field = 0;
				break;
			default:
				legal = NO;
				break;
				}
			}
		if (!legal)
			scr_beep();
		} /* end FOREVER */
	}
###mu_chv.c
/* mu_chv - get a value for a CHOICE_VAL field
 * Return the SCR_CMDCHAR that terminated the input of this field.
 * fld_sync = field in data rec agrees with screen
 */
#include "menu.h"
SCR_CMDCHAR mu_chv(pf)
	FIELD *pf;				/* ptr to current FIELD : fld_sync */
	{
	short nchoice;			/* current choice value: {0:9} */
	CHOICE_VAL *pcv;		/* ptr to this field's CHOICE_VAL */
	SCR_CMDCHAR c;			/* most recent user input char */
	short prevlen;			/* length of previous choice display string */
	short curlen;			/* length of current choice display string */
	short i;
	
	prevlen = 0;
	pcv = pf->pcv;
	if ('0' <= *pcv->pc && *pcv->pc <= '9')
		nchoice = *pcv->pc - '0';
	else
		nchoice = 0;
	FOREVER
		{
		/* scr_curs to start of choice display, display current choice */
		scr_curs(pf->line, pf->col + strlen(pf->desc) + 2);
		scr_print("%s", pcv->choices[nchoice]);		/* pf => fld_sync */

		/* erase any leftover from previous choice */
		curlen = strlen(pcv->choices[nchoice]);
		for (i = 0; i < prevlen - curlen; ++i)
			scr_putc(' ');
		for (i = 0; i < prevlen - curlen; ++i)
			scr_putc('\b');
		prevlen = curlen;

		/* get user input and process it */
		c = scr_getkey();
		if (c != SCR_LEFT && c != SCR_RIGHT)
			return (c);
		else if (c == SCR_RIGHT)
			nchoice = IMOD(nchoice + 1, pcv->nchoices);
		else /* c == SCR_LEFT */
			nchoice = IMOD(nchoice - 1, pcv->nchoices);
		*pcv->pc = '0' + nchoice;					/* pf => !fld_sync */
		}
	}
###mu_do.c
#include "menu.h"
/* mu_do - present a menu, process actions until user SCR_EXIT
 */
void mu_do(pm)
	MENU *pm;
	{
	FIELD *pf;
	static short level = 0;

	if (level == 0)
		scr_open();	/* initialize the terminal state */
	++level;
	FOREVER
		{
		mu_pr(pm);						/* print the menu */
		pf = mu_ask(pm);				/* get a new field ptr */
		if (pf == NULL)
			break;
		else if (pf->val_type == 'm')	/* new field is a menu */
			mu_do(pf->pmenu);
		else /* pf->val_type == 'f' -- new field is a C fn */
			{
			asserts(pf->val_type == 'f', "val_type is 'f'");
			(void)(*pf->pfn)(pm);
			}
		}
	--level;
	if (level == 0)
		{
		scr_curs(scr_lins-1, 0);
		scr_close();	/* restore terminal state */
		}
	}
###mu_pr.c
/* mu_pr - display a menu on the screen */
#include "menu.h"
void mu_pr(pm)		/* at return, pm => menu-sync */
	MENU *pm;		/* ptr to the MENU to be displayed */
	{
	short i;		/* index for loop over fields */
	FIELD *pf;		/* pointer to each FIELD */
	short i_choice;	/* numeric value of choice field */

	scr_clear();

	/* print the menu title, centered on top line */
	scr_curs(0, (scr_cols - strlen(pm->title))/2 - 3);
	scr_print("%s", pm->title);

	for (i = 0; i < pm->nfields; ++i)		/* for each field */
		{
		pf = &pm->fields[i];

		/* print the field description at proper x,y location */
		scr_curs(pf->line, pf->col);
		scr_print("%s  ", pf->desc);

		/* for string and choice fields, print current value */
		if (tolower(pf->val_type) == 's')
			{
			if (pf->psv->pnum != NULL)		/* field has numeric form */
				itoa(*pf->psv->pnum, pf->psv->string, pf->psv->maxsize -1);
			scr_print("%s", pf->psv->string);
			}
		else if (pf->val_type == 'c')
			{
			if ('0' <= *pf->pcv->pc && *pf->pcv->pc <= '9')
				i_choice = *pf->pcv->pc - '0';
			else
				i_choice = 0;
			scr_print("%s", pf->pcv->choices[i_choice]);
			}
		}
	scr_refresh();
	}
###mu_reply.c
/*  mu_reply -- get a reply in response to a prompt
 *				on the last screen line
 */
#include "local.h"
#include "menu.h"

mu_reply(prompt, reply, size)
	char prompt[];		/* : string */
	char reply[];		/* : !NULL */
	int size;			/* max number of chars allowed in reply */
	{
	STRING_VAL rsv;		/* reply string value structure */

	if (scr_mode == SCR_TTY)
		return (getreply(prompt, reply, size));
	else
		{
		scr_curs(scr_lins - 1, 0);
		scr_print("%s", prompt);
		reply[0] = '\0';		/* reply string initially empty */
		rsv.edit_type = 'a';	/* initialize the STRING_VAL rsv */
		rsv.string = reply;
		rsv.maxsize = size;
		rsv.pnum = NULL;		/* rsv => well-defined */
		mu_sval(&rsv);			/* let mu_sval handle the dialog */
		return (strlen(reply));
		}
	}
###mu_str.c
/* mu_str - get a string for a STRING_VAL field */
#include "menu.h"
SCR_CMDCHAR mu_str(pf)
	FIELD *pf;		/* : fld_sync */
	{ 				/* fld_sync = field in data rec agrees with screen */
	SCR_CMDCHAR ret;
	STRING_VAL *psv;

	psv = pf->psv;	/* get the string-value pointer for this field */

	/* position scr_curs just to right of string value */
	scr_curs(pf->line, pf->col + strlen(pf->desc) + 2 + strlen(psv->string));

	ret = mu_sval(psv);						/* get string value */
	if (psv->pnum != NULL)					/* pf => !fld_sync */
		*psv->pnum = atoi(psv->string);		/* pf => fld_sync */
	return (ret);
	}
###mu_sval.c
/* mu_sval - get the string contents for a STRING_VAL field
 * Assumes cursor is just to right of string
 */
#include "menu.h"
SCR_CMDCHAR mu_sval(psv)
	STRING_VAL *psv;	/* ptr to the STRING_VAL structure : str_sync  */
						/* str_sync = screen agrees with contents of   */
						/*            psv->string                      */
	{
	char *s;			/* ptr to start of string */
	short i;			/* index of current nul-terminator */
	SCR_CMDCHAR c;		/* latest SCR_CMDCHAR returned from scr_getkey */

	s = psv->string;
	i = strlen(s);
	FOREVER
		{
		c = scr_getkey();
		if (SCR_CMD(c))				/* if c is a command character, */
			return (c);				/* return it */
		else if (c == '\b')
			{
			if (i > 0)
				{
				scr_print("\b \b");	/* erase previous character */
				s[--i] = '\0';		/* shorten the string */
				}
			}
		else if (psv->edit_type == 'n' && !isdigit(c) &&
			!(i == 0 && c == '-'))
			{
			scr_beep();				/* non-digit invalid */
			}
		else if (i < psv->maxsize - 1)
			{
			scr_putc(s[i++] = c);	/* append the char, and echo it */
			s[i] = '\0';
			}
		else
			scr_beep();				/* no room left in string */
		}
	}
###nfrom.c
/* nfrom - {{ get listing from lpc files }} */
int nfrom(lo, hi)
	int lo, hi;
	{
	return (rand() % (hi - lo + 1) + lo);
	}
###part_db.c
/* part_db.c - simulated database access to parts */
#include "local.h"
#include "part.h"
static PART parts[3] = {0};
static short n_part = 0;

/* db_locate_part -- see if a part is in the database */
static int db_locate_part(partp)
	PART *partp;
	{
	short i;

	for (i = 0; i < n_part; ++i)
		if (STREQ(partp->part_no, parts[i].part_no))
			return (i);		/* this is the part, return its index */
	return (-1); /* no part from 0 to n_part matches this part_no */
	}																					                                                                         /*
.PE
.PS 13
/* db_add_part - add a part record to database */
bool db_add_part(partp)
	PART *partp;
	{
	if (n_part >= DIM(parts))
		{
		remark("Part storage is full", "");
		return (NO);
		}
	STRUCTASST(parts[n_part], *partp);
	++n_part;
	return (YES);
	}																					                                                                         /*
.PE
.PS 12
/* db_find_part - find a part record in database */
bool db_find_part(partp)
	PART *partp;
	{
	short i;

	i = db_locate_part(partp);
	if (i == -1)
		return(NO);
	STRUCTASST(*partp, parts[i]);
	return(YES);
	}																				                                                              /* 
.PE
.PS 17
/* db_del_part - delete a part record in database */
bool db_del_part(partp)
	PART *partp;
	{
	short i;

	for (i = 0; i < n_part; ++i)
		{
		if (STREQ(partp->part_no, parts[i].part_no))
			{
			--n_part;
			STRUCTASST(parts[i], parts[n_part]);
			return (YES);
			}
		}
	return (NO);
	}																					                                                                         /*
.PE
.PS 16
/* db_chg_part - change a part record in database */
bool db_chg_part(partp)
	PART *partp;
	{
	short i;

	for (i = 0; i < n_part; ++i)
		{
		if (STREQ(partp->part_no, parts[i].part_no))
			{
			STRUCTASST(parts[i], *partp);
			return (YES);
			}
		}
	return (NO);
	}																					                                                                         /*
.PE
.PS 12
/* db_open_part - open the parts database
 * Dummy - no action needed
 */
void db_open_part()
	{
	}
/* db_close_part - close the parts database
 * Dummy - no action needed
 */
void db_close_part()
	{
	}
###part_db_tr.c
/* part_db_tr.c - simulated database access to parts 
 * uses binary tree for in-memory storage
 */
#include "tree.h"
#include "part.h"
#define TPART struct tr_part
TPART
	{
	TPART *right;
	TPART *left;
	PART part;
	};
static TPART tpart = {0};
static TPART *parts = NULL;

/* db_cmp_part - comparison function for database access */
static int db_cmp_part(p1, p2)
	TPART *p1, *p2;
	{
	return (strcmp(p1->part.part_no, p2->part.part_no));
	}
/* (more) */																															/*
.PE
.PS 16
/* db_add_part - add a part record to database */
bool db_add_part(partp)
	PART *partp;
	{
	TPART *p;

	p = (TPART *)malloc(sizeof(TPART));
	if (p == NULL)
		{
		remark("Part storage is full", "");
		return (NO);
		}
	STRUCTASST(p->part, *partp);
	TR_INSERT(&parts, p, db_cmp_part);
	return (YES);
	}
																					                                                      /* 
.PE
.PS  50
/* db_find_part - find a part record in database */
bool db_find_part(partp)
	PART *partp;
	{
	TPART *pn;		/* ptr to part record in tree */

	STRUCTASST(tpart.part, *partp);
	pn = (TPART *)TR_FIND(&parts, &tpart, db_cmp_part);
	if (pn == NULL)
		return (NO);
	STRUCTASST(*partp, pn->part);
	return (YES);
	}
/* db_del_part - delete a part record in database */
bool db_del_part(partp)
	PART *partp;
	{
	TPART **pln;		/* ptr to link-in of part record in tree */

	STRUCTASST(tpart.part, *partp);
	pln = (TPART **)TR_LFIND(&parts, &tpart, db_cmp_part);
	if (*pln == NULL)
		{
		remark("No record for this part", "");
		return (NO);
		}
	TR_DELETE(&parts, pln, db_cmp_part);
	return (YES);
	}
/* db_chg_part - change a part record in database */
bool db_chg_part(partp)
	PART *partp;
	{
	TPART *pn;		/* ptr to part record in tree */

	STRUCTASST(tpart.part, *partp);
	pn = (TPART *)TR_FIND(&parts, &tpart, db_cmp_part);
	if (pn == NULL)		/* validation done in menu should prevent */
		return (NO);	/* non-existent part from reaching here */
	STRUCTASST(pn->part, *partp);
	return (YES);
	}
/* (more) */																															/*
.PE
.PS  50
/* db_open_part - open a part record database */
void db_open_part()
	{
	TR_OPEN(&parts);
	}

/* db_close_part - close the part record database */
void db_close_part()
	{
	TR_CLOSE(&parts);
	}
###part_hash.c
/* part_hash.c -- hash file database access to parts */
#include "rec_io.h"
#include "part.h"
#define HPART struct hash_part
HPART
	{
	char hcode;
	PART part;
	};
static HPART hpart = {0};
static HPART kpart = {0};
static rec_fd part_fd = 0;

/* db_cmp_part - compare function for database access */
static int db_cmp_part(p1, p2)
	HPART *p1, *p2;
	{
	return (strcmp(p1->part.part_no, p2->part.part_no));
	}																					                                                                         /*
.PE
.PS 11
/* db_hash_part - hash the part_no field */
static long db_hash_part(p1)
	HPART *p1;
	{
	char *p;
	long sum = 0;

	for (p = p1->part.part_no; *p != '\0'; ++p)
		sum += *p;
	return (IMOD(sum, rec_nrecs(part_fd)));
	}																					                                                                         /*
.PE
.PS 7
/* db_open_part - open the parts database */
void db_open_part()
	{
	part_fd = rec_open("part.dat", O_RDWR, sizeof(HPART));
	if (part_fd < 0)
		error("can't open ", "part.dat");
	}																					                                                                         /*
.PE
.PS 6
/* db_close_part - close the parts database */
void db_close_part()
	{
	if (rec_close(part_fd) < 0)
		error("can't close ", "part.dat");
	}																					                                                                         /*
.PE
.PS 21
/* db_add_part - add a part to the database */
bool db_add_part(partp)
	PART *partp;
	{
	long i;

	STRUCTASST(hpart.part, *partp);
	hpart.hcode = REC_OCCUP;
	i = rec_havail(part_fd, &hpart, &kpart, db_hash_part);
	if (i == REC_FULL)
		{
		remark("Part storage is full", "");
		return (NO);
		}
	else if (i == REC_ERROR || !rec_put(part_fd, &hpart, i))
		{
		remark("I/O error", "");
		return (NO);
		}
	return (YES);
	}																					                                                                         /*
.PE
.PS 11
/* db_chg_part - change a part record on database */
bool db_chg_part(partp)
	PART *partp;
	{
	long this_rec;
	
	STRUCTASST(kpart.part, *partp);
	this_rec = rec_hfind(part_fd, &kpart, &hpart, db_cmp_part, db_hash_part);
	kpart.hcode = REC_OCCUP;
	return (rec_put(part_fd, &kpart, this_rec));
	}																					                                                                         /*
.PE
.PS 11
/* db_del_part - delete a part record on database */
bool db_del_part(partp)
	PART *partp;
	{
	long this_rec;
	
	STRUCTASST(kpart.part, *partp);
	this_rec = rec_hfind(part_fd, &kpart, &hpart, db_cmp_part, db_hash_part);
	kpart.hcode = REC_DELET;
	return (rec_put(part_fd, &kpart, this_rec));
	}																					                                                                         /*
.PE
.PS 21
/* db_find_part - find a part on the database */
bool db_find_part(partp)
	PART *partp;
	{
	long this_rec;

	STRUCTASST(kpart.part, *partp);
	this_rec = rec_hfind(part_fd, &kpart, &hpart, db_cmp_part, db_hash_part);
	if (this_rec >= 0)
		{
		STRUCTASST(*partp, hpart.part);
		return (YES);
		}
	else if (this_rec == REC_ERROR)
		{
		remark("I/O error", "");
		return (NO);
		}
	else
		return (NO);
	}
###part_menu.c
#include "menu.h"
#include "part.h"
/* main menu */
/* menu-sync = screen agrees with menu contents */
PART part1 = {0};
PART null_part = {0};
extern MENU add_menu, chg_menu, del_menu, acd_menu;
/* isn_part - verify that no record exists for this key */
bool isn_part(pm)
	MENU *pm;		/* : menu-sync */
	{
	if (db_find_part(&part1))
		{
		/* pm => !menu-sync, new part contents */
		remark("This part is entered already", "");
		STRUCTASST(part1, null_part);	/* clear the part struct */
		mu_pr(pm);						/* pm => menu-sync */
		return (NO);
		}
	return (YES);
	}																					                                                                         /*
.PE
.PS 14
/* is_part - verify that record exists and read it into part1 */
bool is_part(pm)
	MENU *pm;
	{
	if (!db_find_part(&part1))
		{
		remark("Unknown part number", "");
		STRUCTASST(part1, null_part);		/* clear the part struct */
		mu_pr(pm);		/* display the cleared record, pm => menu-sync */
		return (NO);
		}
	/* pm => !menu-sync, new part contents */
	mu_pr(pm);			/* display the found record, pm => menu-sync */
	return (YES);
	}																					                                                                         /*
.PE
.PS 19
/* addpart - add a part record */
bool addpart(pm)
	MENU *pm;
	{
	char reply[2];

	STRUCTASST(part1, null_part);
	mu_do(&add_menu);
	/* if no valid transaction occured, exit without change to db */
	if (part1.part_no[0] == '\0')
		return (NO);
	mu_reply("Add this part [y/n]? ", reply, 2);
	if (reply[0] == 'y')
		{
		return (db_add_part(&part1));
		}
	else
		return (NO);
	}																				                                                                         /*
.PE
.PS 18
/* chgpart - change a part record */
bool chgpart(pm)
	MENU *pm;
	{
	char reply[2];

	STRUCTASST(part1, null_part);
	mu_do(&chg_menu);
	/* if no valid transaction occured, exit without change to db */
	if (part1.part_no[0] == '\0')
		return (NO);
	mu_reply("Change this part [y/n]? ", reply, 2);
	if (reply[0] == 'y')
		return (db_chg_part(&part1));
	else
		return (NO);
	}																					                                                                         /*
.PE
.PS 18
/* delpart - delete a part record */
bool delpart(pm)
	MENU *pm;
	{
	char reply[2];

	STRUCTASST(part1, null_part);
	mu_do(&del_menu);
	/* if no valid transaction occured, exit without change to db */
	if (part1.part_no[0] == '\0')
		return (NO);
	mu_reply("Delete this part [y/n]? ", reply, 2);
	if (reply[0] == 'y')
		return (db_del_part(&part1));
	else
		return (NO);
	}																					                                                                         /*
.PE
.PS 7
/* part_menu (main) - execute the parts menu */
main()
	{
	db_open_part();
	mu_do(&acd_menu);
	db_close_part();
	}
###plot_m.c
/* plot_m - demonstrate plot_trk function */
#include "local.h"
#include "screen.h"
main()
	{
	short i;

	scr_open();
	scr_clear();
	for (i = 0; i < 400; ++i)	/* four times around the track */
		plot_trk(i, 'X');
	scr_curs(scr_lins-1, 0);
	scr_close();
	}
###plot_trk.c
/* plot_trk - plot position around a pseudo-oval track */
#include "local.h"
#include "screen.h"
#define LIN_ORIGIN 11
#define COL_ORIGIN 40
#define NPOINTS 26
#define IMAX (NPOINTS - 1)
#define POS(n, signlin, signcol) \
	scr_curs(LIN_ORIGIN + (signlin) * coords[n].lin, \
		COL_ORIGIN + (signcol) * coords[n].col)
static struct coords
	{
	short lin, col;
	} coords[NPOINTS] =
	{		/* points for one quadrant (quarter-screen) */
	{11,  0},  {11,  2},  {11,  4},  {11,  6},  {11,  8},
	{11, 10},  {11, 12},  {11, 14},  {11, 16},  {11, 18},
	{11, 20},  {11, 22},  {11, 24},  {11, 26},  {11, 28},
	{10, 29},  { 9, 30},  { 8, 31},  { 7, 32},  { 6, 33},
	{ 5, 34},  { 4, 35},  { 3, 36},  { 2, 36},  { 1, 36},
	{ 0, 36},
	};																			                                                            /*
.PE
.PS 26
/* plot_trk - plot a point */
void plot_trk(n, c)
	int n;
	char c;
	{
	asserts(n >= 0, "plot_trk: n is non-negative");
	n %= 4 * IMAX;
	if (n < IMAX)
		POS(n, 1, 1);				/* 1st quadrant - lower right */
	else if (n < 2 * IMAX)
		POS(2 * IMAX - n, -1, 1);	/* 2nd quadrant - upper right */
	else if (n < 3 * IMAX)
		POS(n - 2 * IMAX, -1, -1);	/* 3rd quadrant - upper left */
	else
		POS(4 * IMAX - n, 1, -1);	/* 4th quadrant - lower left */
	scr_putc(c);
	}
###pmt.c
/* pmt - compute payment on mortgage
 */
#include <local.h>
main()
	{
	double pow();	/* power function */
	double round();	/* rounding function */
	double intmo;	/* monthly interest */
	double intyr;	/* annual interest */
	double bal;		/* balance remaining */
	double pmt;		/* monthly payment */
	short npmts;	/* number of payments */
	short nyears;	/* number of years */

	/* get input and compute monthly payment */
	printf("Enter principal (e.g. 82500.00): ");
	scanf("%lf", &bal);
	printf("Enter annual interest rate (e.g. 16.25): ");
	scanf("%lf", &intyr);
	printf("Enter number of years: ");
	scanf("%hd", &nyears);
	printf("\nprincipal=%.2f", bal);
	printf("  interest=%.4f%%", intyr);
	printf("  years=%d\n\n", nyears);
	intyr /= 100.;
	intmo = intyr / 12.;
	npmts = nyears * 12;
	pmt = bal * (intmo / (1. - pow(1. + intmo, (double)-npmts)));
	pmt = round(pmt, .0099);

	printf("pmt=%.2f\n", pmt);
	}
double round(dollars, adjust)
	double dollars;
	double adjust;
	{
	long pennies;

	pennies = (dollars + adjust) * 100.;
	return (pennies / 100.);
	}
###powtst.c
/* powtst - test accuracy of pow
 */
#include "local.h"
#include <math.h>
main()
	{
	double x, y, z;

	for (x = 1.; x <= 25.; x = x + 1.)
		{
		y = x * x;
		z = pow(x, 2.);
		if (y != z)
			printf("%5.1f %20.16f %20.16f %20.16f\n", x, y, z, y-z);
		}
	}
###q_append.c
/* q_append - install Q_NODE at rear of queue */
#include "queue.h"
void q_append(frontp, rearp, p)
	Q_NODE **frontp, **rearp;
	Q_NODE *p;
	{
	Q_NEXT(p) = NULL;			/* new node will be rear of queue */
	if (*frontp == NULL)		/* if queue was empty, */
		*frontp = *rearp = p;	/* p becomes front and rear */
	else
		{
		Q_NEXT(*rearp) = p;		/* splice p into queue */
		*rearp = p;				/* p becomes rear of queue */
		}
	}
###q_close.c
/* q_close - close the queue accessed via *frontp and *rearp */
#include "queue.h"
void q_close(frontp, rearp)
	Q_NODE **frontp, **rearp;
	{
	Q_NODE *p;
	Q_NODE *pnext;

	for (p = *frontp; p != NULL; p = pnext)
		{
		pnext = Q_NEXT(p);
		free(p);		/* p is (momentarily) undefined */
		}
	*frontp = *rearp = NULL;		/* prevent dangling ptr */
	}
###q_insert.c
/* q_insert - install Q_NODE at proper place in queue */
#include "queue.h"
void q_insert(frontp, rearp, p, cmpfn)
	Q_NODE **frontp, **rearp;
	Q_NODE *p;
	int (*cmpfn)();				/* function for comparisons */
	{
	Q_NODE *p2;

	if (*frontp == NULL					/* if queue was empty, */
		|| (*cmpfn)(p, *frontp) < 0)	/* or if p comes first, */
		Q_PUSH(frontp, rearp, p);		/* push p onto front */
	else
		{
		for (p2 = *frontp; ; p2 = Q_NEXT(p2))
			{
			if (Q_NEXT(p2) == NULL)			
				{ 							/* if end of queue, */
				Q_APPEND(frontp, rearp, p);	/* append p at rear of queue */
				break;
				}
			else if ((*cmpfn)(p, Q_NEXT(p2)) < 0)
				{						/* if p sorts before Q_NEXT(p2) */
				Q_R_INSERT(frontp, rearp, p, p2);	/* insert after p2 */
				break;
				}
			}
		}
	}
###q_lfind.c
/* q_lfind - search queue for equal match to p, return its link-in 
 * Return NULL if no match
 */
#include "queue.h"
Q_NODE **q_lfind(frontp, rearp, p, cmpfn)
	Q_NODE **frontp, **rearp;
	Q_NODE *p;
	int (*cmpfn)();
	{
	Q_NODE **pp;

	for (pp = frontp; *pp != NULL; pp = &Q_NEXT(*pp))
		if ((*cmpfn)(*pp, p) == 0)
			return (pp);
	return (NULL);
	}
###q_main.c
/* q_main - test routine for queue package */
#include "local.h"
#include "queue.h"
#define NAME_NODE struct name_node
NAME_NODE
	{
	NAME_NODE *next;
	char name[4];
	};
NAME_NODE *front = NULL;		/* front node of queue */
NAME_NODE *rear = NULL;			/* rear node of queue */
NAME_NODE *p = NULL;			/* current node */
void show_cmds(), append_name(), push_name(), pop_name(), dump_names();
main()
	{
	char buf[2];				/* buffer for input */

	show_cmds();
	Q_OPEN(&front, &rear);		/* open the queue */
	FOREVER
		{
		if (getreply("?: ", buf, 2) == EOF)
			exit(0);
		switch (buf[0])
			{
		case '>':
			append_name();
			break;
		case '+':
			push_name();
			break;
		case '-':
			pop_name();
			break;
		case '=':
			dump_names();
			break;
		case '0':
			Q_CLOSE(&front, &rear);
			Q_OPEN(&front, &rear);		/* open the queue */
			break;
		case '?':
			show_cmds();
			break;
		default:
			printf("unknown command: %c\n", buf[0]);
			show_cmds();
			break;
			}
		}
	}																	/*
.PE
.PS 44
/* show_cmds -- show legal commands */
void show_cmds()
	{
	printf("Type > to append, + to push, - to pop, = to print, 0 to reset:\n");
	}
/* append_name - append new name on queue */
void append_name()
	{
	p = (NAME_NODE *)malloc(sizeof(NAME_NODE));
	if (p == NULL)
		error("out of space", "");
	getreply("name: ", p->name, 4);
	Q_APPEND(&front, &rear, p);
	}
/* push_name - push new name on queue */
void push_name()
	{
	p = (NAME_NODE *)malloc(sizeof(NAME_NODE));
	if (p == NULL)
		error("out of space", "");
	getreply("name: ", p->name, 4);
	Q_PUSH(&front, &rear, p);
	}
/* pop_name - pop a name off queue */
void pop_name()
	{
	p = (NAME_NODE *)Q_POP(&front, &rear);
	if (p == NULL)
		printf("EMPTY QUEUE\n");
	else
		{
		printf("name= %s\n", p->name);
		free(p);
		}
	}
/* dump_names - print the current queue of names */
void dump_names()
	{
	if (front == NULL)
		printf("EMPTY QUEUE\n");
	else
		EACH_Q(front, p)
			printf("name= %s\n", p->name);
	}
###q_pop.c
/* q_pop - detach front node of queue and return pointer to it */
#include "queue.h"
Q_NODE *q_pop(frontp, rearp)
	Q_NODE **frontp, **rearp;
	{
	Q_NODE *p;

	if (*frontp == NULL)
		return (NULL);
	p = *frontp;			/* p points to front of queue */
	*frontp = Q_NEXT(p);	/* front now points to next node (or NULL) */
	if (*frontp == NULL)	/* if queue is now empty, */
		*rearp = NULL;		/* then make both ptrs NULL */
	Q_NEXT(p) = NULL;		/* prevent dangling ptr */
	return (p);
	}
###q_push.c
/* q_push - install Q_NODE at front of queue */
#include "queue.h"
void q_push(frontp, rearp, p)
	Q_NODE **frontp, **rearp;
	Q_NODE *p;
	{
	Q_NEXT(p) = *frontp;	/* p points to previous front (or NULL) */
	*frontp = p;			/* *frontp now points to p */
	if (*rearp == NULL)		/* if queue was empty, */
		*rearp = p;			/* *rearp also points to p */
	}
###q_r_detach.c
/* q_r_detach - detach queue node that *pp links-in and return pointer to it */
#include "queue.h"
Q_NODE *q_r_detach(frontp, rearp, pp)
	Q_NODE **frontp, **rearp;
	Q_NODE **pp;
	{
	Q_NODE *p2;				/* ptr to detached node */

	if (*pp == NULL)
		return (NULL);
	p2 = *pp;				/* p2 points to node to detach */
	*pp = Q_NEXT(p2);		/* *pp now points to next node (or NULL) */
	if (*frontp == NULL)	/* if queue is now empty, */
		*rearp = NULL;		/* then make both ptrs NULL */
	Q_NEXT(p2) = NULL;		/* prevent dangling ptr */
	return (p2);
	}
###q_r_insert.c
/* q_r_insert - install Q_NODE after *pp */
#include "queue.h"
void q_r_insert(frontp, rearp, p, pp)
	Q_NODE **frontp, **rearp;
	Q_NODE *p;
	Q_NODE **pp;
	{
	Q_NEXT(p) = *pp;		/* p points to node following *pp (or NULL) */
	*pp = p;				/* *pp now points to p */
	if (*rearp == NULL)		/* if queue was empty, */
		*rearp = p;			/* *rearp also points to p */
	}
###qkmain3.c
#include "qksort3.c"
#define SORTFN(a, lim) qksort(a, lim)
static int intcmp(a, b)
	int *a, *b;
	{
	if (*a > *b)
		return (1);
	else if (*a == *b)
		return (0);
	else
		return (-1);
	}
#include "qkmain.h"
###qksort.c
/* qksort - sort array a (dimension n) using quicksort */
#include "local.h"
/* iqksort - internal function to partition a[lo:hi] */
static void iqksort(a, lo, hi)
	short a[];	/* array to be sorted; a[lo:hi]: multiset */
	index_t lo;	/* lowest subscript */
	index_t hi;	/* highest subscript */
	{
	index_t mid = lo + (hi-lo)/2;	/* : {lo:hi} */
	index_t i;						/* : {lo+1:hi+1} */
	index_t lastlo;					/* : {lo:hi-1} */
	short tmp;

	SWAP(a[lo], a[mid], tmp);
	lastlo = lo;
	for (i = lo + 1; i <= hi; ++i)
		{
		if (a[lo] > a[i])
			{
			++lastlo;
			SWAP(a[lastlo], a[i], tmp);
			}
		}
	SWAP(a[lo], a[lastlo], tmp);
	if (lastlo != 0 && lo < lastlo - 1)
		iqksort(a, lo, lastlo - 1);
	if (lastlo + 1 < hi)
		iqksort(a, lastlo + 1, hi);
	}
/* qksort - external entry point */
void qksort(a, n)
	short a[];	/* array to be sorted; a[0:n-1]: multiset */
	index_t n;	/* number of elements */
	{
	if (n > 1)
		iqksort(a, 0, n - 1);
	asserts(tst_sort(a, n), "a is sorted");
	}
###qksort1a.c
/* qksort - sort array a (dimension n) using quicksort
 */
#include "local.h"
void qksort(a, n)
	int a[];
	int n;
	{
	iqksort(a, 0, n-1);
	isort(a, n);
	}
static void iqksort(a, lo, hi)
	int a[];
	int lo, hi;
	{
	int mid = (lo + hi) / 2;
	int i;
	int lastlo;
	int t;
	int tmp;

	if (hi - lo < 16)		/* should not use (hi < lo + 16) */
		return;
	SWAP(a[lo], a[mid], tmp);
	t = a[lo];
	lastlo = lo;
	for (i = lo+1; i <= hi; ++i)
		{
		if (a[i] < t)
			{
			++lastlo;
			SWAP(a[lastlo], a[i], tmp);
			}
		}
	SWAP(a[lo], a[lastlo], tmp);
	iqksort(a, lo, lastlo - 1);
	iqksort(a, lastlo + 1, hi);
	}
/* isort - sort array a (dimension n) using insertion sort
 */
void isort(a, n)
	int a[];
	int n;
	{
	int i;
	int j;
	int t;

	for (i = 1; i < n; ++i)	/* a[0:i-1]=>sorted */
		{
		t = a[i];
		j = i;
		while (j > 0 && a[j-1] > t)
			{
			a[j] = a[j-1];
			--j;
			}
		a[j] = t;
		}
	/* a=>sorted */
	}
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static int a[10000];
	double nlogn;
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
	cputim();
	qksort(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	nlogn = lim * log((double)lim) / log(2.);
	printf("log N = %.3f\n", log((double)lim) / log(2.));
	printf("N log N = %.3f\n", nlogn);
	printf("cpu time = %.2f N log N (usec)\n", tim / nlogn);
	for (i = 0; i < lim-1; ++i)
		{
		if (a[i] > a[i+1])
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###qksort2.c
/* qksort - sort array a (dimension n) using quicksort
 */
#include "local.h"
void qksort(a, n)
	int a[];
	int n;
	{
	iqksort(a, &a[n-1]);
	}
static void iqksort(lo, hi)
	int *lo, *hi;
	{
	int *mid = lo + (hi - lo) / 2;
	int *i;
	int *lastlo;
	int t;
	int tmp;

	if (hi <= lo)
		return;
	SWAP(*lo, *mid, tmp);
	t = *lo;
	lastlo = lo;
	for (i = lo+1; i <= hi; ++i)
		{
		if (*i < t)
			{
			++lastlo;
			SWAP(*lastlo, *i, tmp);
			}
		}
	SWAP(*lo, *lastlo, tmp);
	iqksort(lo, lastlo - 1);
	iqksort(lastlo + 1, hi);
	}
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static int a[10000];
	double nlogn;
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
	cputim();
	qksort(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	nlogn = lim * log((double)lim) / log(2.);
	printf("log N = %.3f\n", log((double)lim) / log(2.));
	printf("N log N = %.3f\n", nlogn);
	printf("cpu time = %.2f N log N (usec)\n", tim / nlogn);
	for (i = 0; i < lim-1; ++i)
		{
		if (a[i] > a[i+1])
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###qksort3.c
/* qksort - sort array a (dimension n) using quicksort
 */
#include "local.h"
void qksort(a, n)
	int a[];
	int n;
	{
	iqksort(a, &a[n-1]);
	}
static void iqksort(lo, hi)
	int *lo, *hi;
	{
	int *mid = lo + (hi - lo) / 2;
	register int *i;
	register int *lastlo;
	int t;
	register int tmp;

	if (hi <= lo)
		return;
	SWAP(*lo, *mid, tmp);
	t = *lo;
	lastlo = lo;
	for (i = lo+1; i <= hi; ++i)
		{
		if (*i < t)
			{
			++lastlo;
			SWAP(*lastlo, *i, tmp);
			}
		}
	SWAP(*lo, *lastlo, tmp);
	iqksort(lo, lastlo - 1);
	iqksort(lastlo + 1, hi);
	}
###qksort4.c
/* qksort - sort array a (dimension n) using quicksort
 */
#include "local.h"
void qksort(a, n)
	int a[];
	int n;
	{
	iqksort(a, &a[n-1]);
	isort(a, n);
	}
static void iqksort(lo, hi)
	int *lo, *hi;
	{
	int *mid = lo + (hi - lo) / 2;
	register int *i;
	register int *lastlo;
	int t;
	register int tmp;

	if (hi < lo + 16)	/* cannot use   (hi < lo + 16) */
		return;
	SWAP(*lo, *mid, tmp);
	t = *lo;
	lastlo = lo;
	for (i = lo+1; i <= hi; ++i)
		{
		if (*i < t)
			{
			++lastlo;
			SWAP(*lastlo, *i, tmp);
			}
		}
	SWAP(*lo, *lastlo, tmp);
	iqksort(lo, lastlo - 1);
	iqksort(lastlo + 1, hi);
	}
/* isort - sort array a (dimension n) using insertion sort
 */
void isort(a, n)
	int a[];
	int n;
	{
	int i;
	register int *pj;
	register int *pjm;
	int t;

	for (i = 1; i < n; ++i)	/* a[0:i-1]=>sorted */
		{
		t = a[i];
		pj = &a[i];
		pjm = pj - 1;
		while (pj > a && *pjm > t)
			{
			*pj = *pjm;
			--pj;
			--pjm;
			}
		*pj = t;
		}
	/* a=>sorted */
	}
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static int a[10000];
	double nlogn;
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
	cputim();
	qksort(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	nlogn = lim * log((double)lim) / log(2.);
	printf("log N = %.3f\n", log((double)lim) / log(2.));
	printf("N log N = %.3f\n", nlogn);
	printf("cpu time = %.2f N log N (usec)\n", tim / nlogn);
	for (i = 0; i < lim-1; ++i)
		{
		if (a[i] > a[i+1])
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###qksortm.c
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static int a[10000];
	double nlogn;
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
	cputim();
	qksort(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	nlogn = lim * log((double)lim) / log(2.);
	printf("log N = %.3f\n", log((double)lim) / log(2.));
	printf("N log N = %.3f\n", nlogn);
	printf("cpu time = %.2f N log N (usec)\n", tim / nlogn);
	for (i = 0; i < lim-1; ++i)
		{
		if (a[i] > a[i+1])
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###qksortp.c
/* qksort - sort array a (dimension n) using quicksort */
#include "local.h"
/* iqksort - internal function to partition the array [*lo:*hi] */
static void iqksort(p_lo, p_hi)
	short *p_lo, *p_hi;
	{
	short *p_mid = p_lo + (p_hi - p_lo) / 2;	/* : {p_lo:p_hi} */
	short *p_i;									/* : {p_lo+1:p_hi+1} */
	short *p_lastlo;							/* : {p_lo:p_hi-1} */
	short tmp;

	SWAP(*p_lo, *p_mid, tmp);
	p_lastlo = p_lo;
	for (p_i = p_lo+1; p_i <= p_hi; ++p_i)
		{
		if (*p_lo > *p_i)
			{
			++p_lastlo;
			SWAP(*p_lastlo, *p_i, tmp);
			}
		}
	SWAP(*p_lo, *p_lastlo, tmp);
	if (p_lo < p_lastlo && p_lo < p_lastlo - 1)
		iqksort(p_lo, p_lastlo - 1);
	if (p_lastlo + 1 < p_hi)
		iqksort(p_lastlo + 1, p_hi);
	}
/* qksort - external entry point */
void qksort(a, n)
	short a[];	/* array to be sorted; a[0:n-1]: multiset */
	size_t n;	/* number of elements */
	{
	if (n > 1)
		iqksort(a, &a[n-1]);
	}
###qmain.c
#include "qsort.c"
#define SORTFN(a, lim) qsort(a, lim, sizeof(a[0]), intcmp)
static int intcmp(a, b)
	int *a, *b;
	{
	if (*a > *b)
		return (1);
	else if (*a == *b)
		return (0);
	else
		return (-1);
	}
#include "qkmain.h"
###qsort.c
/* qsort - sort array a (dimension n) using quicksort
 * based on Bentley, CACM April 84
 * Comments use notation A[i], a (fictitious) array of things
 * that are of size elt_size.
 */
#include "local.h"
/* swapfn - swap  elt_size bytes  a <--> b (internal routine) 
 */
static void swapfn(a, b, elt_size)
	register char *a;	/* pointer to one element of A */
	register char *b;	/* pointer to another element of A */
	size_t elt_size;	/* size of each element */
	{
	register size_t i;
	char tmp;

	for (i = 0; i < elt_size; ++i, ++a, ++b)
		SWAP(*a, *b, tmp);
	}																				                                                  /*
.PE
.PS 25
/* iqsort - internal quicksort routine */
static void iqsort(p_lo, p_hi, elt_size, cmpfn)
	char *p_lo;			/* ptr to low element of (sub)array */
	char *p_hi;			/* ptr to high element of (sub)array */
	size_t elt_size;	/* size of each element */
	int (*cmpfn)();		/* comparison function ptr */
	{
	char *p_mid = p_lo + ((((p_hi - p_lo) / elt_size) / 2) * elt_size);
	register char *p_i;			/* pointer to A[i] */
	register char *p_lastlo;	/* pointer to A[lastlo] */

	swapfn(p_lo, p_mid, elt_size);	/* pick the middle element as pivot */
	p_lastlo = p_lo;
	for (p_i = p_lo + elt_size;  p_i <= p_hi; p_i += elt_size)
		{
		if ((*cmpfn)(p_lo, p_i) > 0)
			{
			p_lastlo += elt_size;
			swapfn(p_lastlo, p_i, elt_size);
			}
		}
	swapfn(p_lo, p_lastlo, elt_size);
	if (p_lo < p_lastlo && p_lo < p_lastlo - elt_size)
		iqsort(p_lo, p_lastlo - elt_size, elt_size, cmpfn);	/* lower */
	if (p_lastlo + elt_size < p_hi)
		iqsort(p_lastlo + elt_size, p_hi, elt_size, cmpfn);	/* upper */
	}																				                                                  /*
.PE
.PS 9
/* qsort - the callable entry point */
void qsort(a, n, size, pf)
	data_ptr a;		/* address of array A to be sorted */
	size_t n;		/* number of elements in A */
	size_t size;	/* size of each element */
	int (*pf)();	/* comparison function ptr */
	{
	if (n > 1)
		iqsort((char *)a, (char *)a + (n-1) * size, size, pf);
	}
###qsort_i.c
/* qsort - sort array a (dimension n) using quicksort
 * based on Bentley, CACM April 84
 * Iterative version; avoids recursion, uses little stack
 * Comments use notation A[i], a (fictitious) array of things
 * that are of size elt_size.
 * Uses static storage, not re-entrant.
 */
#include "local.h"

#define STACK_DEPTH (sizeof(size_t) * CHAR_BIT)

static size_t elt_size = 0;		/* size of one element */
static int (*cmpfn)() = NULL;	/* the comparison function ptr */
/* swapfn - swap  elt_size bytes  a <--> b (internal routine)  */
static void swapfn(a, b)
	register char *a;	/* pointer to one element of A */
	register char *b;	/* pointer to another element of A */
	{
	register size_t i;
	char tmp;

	LOOPDN(i, elt_size)
		{
		SWAP(*a, *b, tmp);
		++a, ++b;
		}
	}																	                      /*
.PE
.PS 24
/* sort1 - partition one (sub)array, returning p_lastlo */
static char *sort1(p_lo, p_hi)
	char *p_lo;			/* ptr to low element of (sub)array */
	char *p_hi;			/* ptr to high element of (sub)array */
	{
	char *p_mid;
	register char *p_i;			/* pointer to A[i] */
	register char *p_lastlo;	/* pointer to A[lastlo] */

	p_mid = p_lo + ((((p_hi - p_lo) / elt_size) / 2) * elt_size);
	swapfn(p_lo, p_mid);		/* pick the middle element as pivot */
	p_lastlo = p_lo;
	for (p_i = p_lo + elt_size;  p_i <= p_hi; p_i += elt_size)
		{
		if ((*cmpfn)(p_lo, p_i) > 0)
			{
			p_lastlo += elt_size;
			swapfn(p_lastlo, p_i);
			}
		}
	swapfn(p_lo, p_lastlo);
	return (p_lastlo);
	}																		                           /*
.PE
.PS 23
/* qsort - the callable entry point */
void qsort(a, n, size, pf)
	data_ptr a;		/* address of array A to be sorted */
	size_t n;		/* number of elements in A */
	size_t size;	/* size of each element */
	int (*pf)();	/* comparison function ptr */
	{
	static char *histack[STACK_DEPTH] = {0};
	static char *lostack[STACK_DEPTH] = {0};
	int istack;				/* index of next free stack cell */
	char *p_lo;				/* ptr to A[lo] */
	char *p_hi;				/* ptr to A[hi] */
	char *p_lastlo;			/* ptr to A[lastlo] */
	char *p_lo1, *p_hi1;	/* partition 1 */
	char *p_lo2, *p_hi2;	/* partition 2 */
	char *tpc;				/* tmp ptr for swaps */

	elt_size = size;
	cmpfn = pf;
	istack = 0;
	p_lo = a;
	p_hi = (char *)a + (n-1) * elt_size;												                            /*
.PE
.PS 45
	/* loop until no non-trivial partitions remain */
	while (istack != 0 || p_lo < p_hi)
		{
		p_lastlo = sort1(p_lo, p_hi);
		p_lo1 = p_lo;
		p_hi1 = p_lastlo - elt_size;
		p_lo2 = p_lastlo + elt_size;
		p_hi2 = p_hi;
		if (p_hi1 - p_lo1 > p_hi2 - p_lo2)
			{
			SWAP(p_lo1, p_lo2, tpc);
			SWAP(p_hi1, p_hi2, tpc);
			}
		/* now [p_lo1,p_hi1] is smaller partition */
		if (p_lo1 >= p_hi1)
			{
			if (p_lo2 < p_hi2)
				{
				/* do next iteration on [p_lo2, p_hi2] */
				p_lo = p_lo2;
				p_hi = p_hi2;
				}
			else if (istack > 0)
				{
				/* take next iteration from stack */
				--istack;
				p_hi = histack[istack];
				p_lo = lostack[istack];
				}
			else
				p_hi = p_lo;	/* done */
			}
		else
			{
			/* push [p_lo2, p_hi2] on stack */
			histack[istack] = p_hi2;
			lostack[istack] = p_lo2;
			++istack;
			/* take next iteration from [p_lo1, p_hi1] */
			p_lo = p_lo1;
			p_hi = p_hi1;
			}
		}
	}
###qsort_r.c
/* qsort - sort array a (dimension n) using quicksort
 * based on Bentley, CACM April 84
 * Comments use notation A[i], a (fictitious) array of things
 * that are of size elt_size.
 */
#include "local.h"
/* swapfn - swap  elt_size bytes  a <--> b (internal routine) 
 */
static void swapfn(a, b, elt_size)
	register char *a;	/* pointer to one element of A */
	register char *b;	/* pointer to another element of A */
	size_t elt_size;	/* size of each element */
	{
	register size_t i;
	char tmp;

	for (i = 0; i < elt_size; ++i, ++a, ++b)
		SWAP(*a, *b, tmp);
	}																				                                                  /*
.PE
.PS 25
/* iqsort - internal quicksort routine */
static void iqsort(p_lo, p_hi, elt_size, cmpfn)
	char *p_lo;			/* ptr to low element of (sub)array */
	char *p_hi;			/* ptr to high element of (sub)array */
	size_t elt_size;	/* size of each element */
	int (*cmpfn)();		/* comparison function ptr */
	{
	char *p_mid = p_lo + ((((p_hi - p_lo) / elt_size) / 2) * elt_size);
	register char *p_i;			/* pointer to A[i] */
	register char *p_lastlo;	/* pointer to A[lastlo] */

	swapfn(p_lo, p_mid, elt_size);	/* pick the middle element as pivot */
	p_lastlo = p_lo;
	for (p_i = p_lo + elt_size;  p_i <= p_hi; p_i += elt_size)
		{
		if ((*cmpfn)(p_lo, p_i) > 0)
			{
			p_lastlo += elt_size;
			swapfn(p_lastlo, p_i, elt_size);
			}
		}
	swapfn(p_lo, p_lastlo, elt_size);
	if (p_lo < p_lastlo && p_lo < p_lastlo - elt_size)
		iqsort(p_lo, p_lastlo - elt_size, elt_size, cmpfn);	/* lower */
	if (p_lastlo + elt_size < p_hi)
		iqsort(p_lastlo + elt_size, p_hi, elt_size, cmpfn);	/* upper */
	}																				                                                  /*
.PE
.PS 9
/* qsort - the callable entry point */
void qsort(a, n, size, pf)
	data_ptr a;		/* address of array A to be sorted */
	size_t n;		/* number of elements in A */
	size_t size;	/* size of each element */
	int (*pf)();	/* comparison function ptr */
	{
	if (n > 1)
		iqsort((char *)a, (char *)a + (n-1) * size, size, pf);
	}
###qsortm.c
/* qsortm - test the qsort function
 */
#include "local.h"
#include "cputim.h"
/* cmpfn - compare two ints
 */
int cmpfn(pi, pj)
	register int *pi, *pj;
	{
	if (*pi < *pj)
		return (-1);
	else if (*pi == *pj)
		return (0);
	else
		return (1);
	}
/* qsortm (main) - run the test */
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static int a[10000];
	double nlogn;
	double tim;	/* in usec */
	cputim_t cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < lim; ++i)
		a[i] = rand();
#if 0
	if (lim <= 100)
		for (i = 0; i < lim; ++i)
			printf("%5d\n", a[i]);
#endif
	printf("start:\n");
	cputim();
	qsort((data_ptr)a, lim, sizeof(int), cmpfn);
	tim = 1e6 * (double)cputim() / CLOCK_TICKS_PER_SECOND;
#if 0
	if (lim <= 100)
		for (i = 0; i < lim; ++i)
			printf("%5d\n", a[i]);
#endif
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	nlogn = lim * log((double)lim) / log(2.);
	printf("log N = %.3f\n", log((double)lim) / log(2.));
	printf("N log N = %.3f\n", nlogn);
	printf("cpu time = %.2f N log N (usec)\n", tim / nlogn);
	for (i = 0; i < lim-1; ++i)
		{
		if (a[i] > a[i+1])
			error("bad value", "");
		}
	exit(SUCCEED);
	}
###qsortm.out
qsortm.x 100
lim=100
cpu time = 0.267 (sec)
log N = 6.644
N log N = 664.386
cpu time = 401.37 N log N (usec)
qsortm.x 1000
lim=1000
cpu time = 3.633 (sec)
log N = 9.966
N log N = 9965.784
cpu time = 364.58 N log N (usec)
qsortm.x 10000
lim=10000
cpu time = 44.000 (sec)
log N = 13.288
N log N = 132877.124
cpu time = 331.13 N log N (usec)
###rec_close.c
/* rec_close - close the REC_FILE fd */
#include "rec_io.h"
int rec_close(fd)
	rec_fd fd;
	{
	REC_FILE *rfp;
	int old_errno = errno;

	errno = 0;
	if (fd < 0 || fd >= REC_NFILE)		/* validate fd */
		return (-1);
	rfp = &rec_files[fd];
	if ((rfp->_status & O_RWMODE) == O_RDONLY)
		return (bin_close(fd));			/* if read-only, all done */
	bin_lseek(fd, (long)0, SEEK_SET);	
	bin_write(fd, rfp, sizeof(*rfp));	/* write new REC_FILE info */
	bin_close(fd);
	if (errno == 0)
		{
		errno = old_errno;
		return (0);
		}
	else
		{
		return (-1);
		}
	}
###rec_files.c
/* rec_files - global array of REC_FILEs */
#include "rec_io.h"
REC_FILE rec_files[REC_NFILE] = {0};
###rec_get.c
/* rec_get - read one record from record-binary file */
#include "rec_io.h"
bool rec_get(fd, buf, recno)
	rec_fd fd;		/* which file */
	data_ptr buf;	/* where to put the data */
	long recno;		/* {REC_NEXT,0:rec_nrecs(fd)-1} */
	{
	REC_FILE *rfp;
	short n;		/* size of each record */

	if (fd < 0 || fd >= REC_NFILE)	/* validate fd */
		return (NO);
	rfp = &rec_files[fd];
	n = rfp->_recsize;
	if (recno != REC_NEXT && recno < 0 || recno >= rfp->_nrecs)
		return (NO);
	else if (recno == REC_NEXT)
		;	/* no seek, ready for next record */
	else if (bin_lseek(fd, rfp->_byte0 + recno * n, SEEK_SET) < 0)
		return (NO);
	return (bin_read(fd, buf, n) == n);
	}
###rec_havail.c
/* rec_havail.c -- find where a record belongs in a hashed file */
#include "rec_io.h"

long rec_havail(fd, keybuf, buf, hashfn)
	rec_fd fd;			/* hashed file */
	data_ptr keybuf;	/* record key to find */
	data_ptr buf;		/* buffer for found record */
	long (*hashfn)();	/* hash lookup function */
	{
	long nrecs;			/* number of records in file */
	long ntry;
	long i;

	nrecs = rec_nrecs(fd);
	i = (*hashfn)(keybuf);
	for (ntry = 0; ntry < nrecs; ++ntry)
		{
		if (!rec_get(fd, buf, i))
			return (REC_ERROR);
		if (*(char *)buf == REC_AVAIL || *(char *)buf == REC_DELET)
			return (i);
		i = IMOD(i + 1, nrecs);
		}
	return (REC_FULL);
	}
###rec_hfind.c
/* rec_hfind.c -- find a record in a hashed file */
#include "rec_io.h"

long rec_hfind(fd, keybuf, buf, cmpfn, hashfn)
	rec_fd fd;			/* hashed file */
	data_ptr keybuf;	/* record key to find */
	data_ptr buf;		/* buffer for found record */
	int (*cmpfn)();		/* record key comparison function */
	long (*hashfn)();	/* hash lookup function */
	{
	long nrecs;			/* number of records in file */
	long ntry;
	long i;

	nrecs = rec_nrecs(fd);
	i = (*hashfn)(keybuf);
	for (ntry = 0; ntry < nrecs; ++ntry)
		{
		if (!rec_get(fd, buf, i))
			return (REC_ERROR);				/* i/o error during rec_get */
		if (*(char *)buf == REC_AVAIL)		/* examine first byte of buf */
			return (REC_NOT_FOUND);
		if (*(char *)buf == REC_OCCUP)
			{
			if ((*cmpfn)(keybuf, buf) == 0)
					return (i);
			}
		i = IMOD(i + 1, nrecs);
		}
	return (REC_FULL);
	}
###rec_main.c
/* rec_main - simple test harness for rec_io */
#include "local.h"
#include "rec_io.h"

main()
	{
	long lnum;
	long rec_no;
	rec_fd rfd;

	rfd = rec_open("lnum.dat", O_WRONLY|O_CREAT|O_TRUNC, sizeof(lnum));
	if (rfd < 0)
		error("can't open (output)", "lnum.dat");
	for (lnum = 0; lnum <= 9; ++lnum)
		if (!rec_put(rfd, (data_ptr)&lnum, REC_W_END))
			error("rec_put (END) error", "");
	rec_close(rfd);

	rfd = rec_open("lnum.dat", O_RDONLY, sizeof(lnum));
	if (rfd < 0)
		error("can't open (input)", "lnum.dat");
	while (rec_get(rfd, (data_ptr)&lnum, REC_NEXT))
		printf("%4ld\n", lnum);
	rec_close(rfd);

	rfd = rec_open("lnum.dat", O_RDWR, sizeof(lnum));
	if (rfd < 0)
		error("can't open (update-output)", "lnum.dat");
	for (lnum = 109; lnum >= 100; --lnum)
		if (!rec_put(rfd, (data_ptr)&lnum, lnum - 100))
			error("rec_put (direct) error", "");
	rec_close(rfd);

	rfd = rec_open("lnum.dat", O_RDWR, sizeof(lnum));
	if (rfd < 0)
		error("can't open (update-input)", "lnum.dat");
	for (rec_no = 0; rec_no <= 9; ++rec_no)
		{
		if (!rec_get(rfd, (data_ptr)&lnum, rec_no))
			error("rec_get (direct) error", "");
		else if (lnum != rec_no + 100)
			error("bad data on re-read", "");
		else
			printf("%4ld %4ld\n", rec_no, lnum);
		}
	rec_close(rfd);
	exit(SUCCEED);
	}
###rec_open.c
/* rec_open - open a record-binary file */
#include "rec_io.h"
REC_FILE rec_files[REC_NFILE] = {0};
rec_fd rec_open(fname, type, recsize)
	char fname[];
	int type;
	int recsize;
	{
	rec_fd fd;
	REC_FILE *rfp;			/* pointer to a REC_FILE entry */
	int old_errno = errno;	/* save system error indicator */
	short i;				/* counter to clear first record */

	errno = 0;				/* clear error indicator */
	fd = bin_open(fname, type);
	if (fd < 0 || fd >= REC_NFILE)	/* validate fd */
		return (-1);
	rfp = &rec_files[fd];
	bin_lseek(fd, (long)0, SEEK_SET);	/* seek to initial byte of file */
	if ((type & O_RWMODE) == O_WRONLY ||	/* new file: opened write-only, or */
		bin_read(fd, rfp, sizeof(*rfp)) != sizeof(*rfp))	/* can't be read */
		{
		bin_lseek(fd, (long)0, SEEK_SET);
		rfp->_byte0 = REC_BYTE0;
		rfp->_recsize = recsize;
		rfp->_nrecs = 0;
		bin_write(fd, rfp, sizeof(*rfp));
		for (i = 1; i <= REC_BYTE0 - sizeof(*rfp); ++i)
			bin_write(fd, "\0", 1);
		}
	else if (recsize != rfp->_recsize)	/* file exists, but bad recsize */
		{
		bin_close(fd);
		return (-1);
		}
	if (errno == 0)
		{
		errno = old_errno;				/* restore saved value */
		bin_lseek(fd, (long)rfp->_byte0, 0);
		rfp->_status = type;			/* save the open-mode of file */
		return (fd);
		}
	else /* error was returned from some bin_io function */
		{
		bin_close(fd);
		return (-1);
		}
	}
###rec_put.c
/* rec_put - write one record to record-binary file
 */
#include "rec_io.h"
bool rec_put(fd, buf, recno)
	rec_fd fd;
	data_ptr buf;	/* where to get the data */
	long recno;		/* {REC_W_END,REC_NEXT,0:rec_nrecs(fd)} */
	{
	REC_FILE *rfp;
	short n;		/* size of each record */

	if (fd < 0 || fd >= REC_NFILE)
		return (NO);
	rfp = &rec_files[fd];
	n = rfp->_recsize;
	if (recno != REC_NEXT && recno != REC_W_END && recno < 0 ||
		recno > rfp->_nrecs)
		return (NO);
	else if (recno == REC_W_END || recno == rfp->_nrecs)
		{
		recno = rfp->_nrecs;	/* block will be added at end */
		++rfp->_nrecs;			/* extend the file size */
		}
	if (bin_lseek(fd, rfp->_byte0 + recno * n, SEEK_SET) < 0)
		return (NO);
	return (bin_write(fd, buf, n) == n);
	}
###reverse.c
/* reverse - reverse the order of a string */
#include "local.h"
void reverse(s)
	char s[];
	{
	char t;
	size_t i, j;

	if ((j = strlen(s)) == 0)
		return;
	for (i = 0, j = j - 1; i < j; ++i, --j)
		SWAP(s[i], s[j], t);
	}
###round.c
/* round - adjust floating dollars to even pennies */
#include "local.h"
double round(dollars)
	double dollars;
	{
	double pennies;

	pennies = floor(dollars * 100 + .5);
	return (pennies / 100.);
	}
###roundup.c
/* roundup - adjust floating dollars to even pennies
 * round all fractional pennies upward
 */
#include "local.h"
double roundup(dollars)
	double dollars;
	{
	double pennies;

	pennies = ceil(dollars * 100.);
	return (pennies / 100.);
	}
###run_cars.c
/* run_cars - simulate airport terminal train cars */
#include "deque.h"
#include "screen.h"
#define TRKSIZE 1000
#define CAR struct car
CAR
	{
	CAR *left;		/* pointer to previous deque node; !NULL */
	CAR *right;		/* pointer to next deque node; !NULL */
	short pos;		/* position on track; {0:TRKSIZE-1} */
	short vel;		/* velocity; {0:SHORT_MAX} */
	char ident[2];	/* identifier for this car; {"a":"z"} */
	};
void init();			/* initialize the simulation */
void run();				/* run one time step */
DQ_NODE *cars = NULL;	/* pointer to master deque node */
CAR *p = NULL;			/* pointer to a CAR */
short ncars = 0;		/* number of cars on track; {2:26} */
char idents[] = "abcdefghijklmnopqrstuvwxyz";
main(ac, av)
	int ac;
	char *av[];
	{
	short t;			/* loop counter */
	SCR_CMDCHAR c;		/* input from keyboard */

	if (ac < 2)
		error("usage: run_cars  #-of-cars", "");
	ncars = atoi(av[1]);
	if (ncars < 2 || ncars > 26)
		error("#-of-cars must be between 2 and 26", "");
	scr_open();
	scr_clear();
	scr_curs(21, 40); scr_putc('A');
	scr_curs(11, 75); scr_putc('B');
	scr_curs( 1, 40); scr_putc('C');
	scr_curs(11,  5); scr_putc('D');
	init();
	do {
		for (t = 0; t < 200; ++t)
			run();
		scr_curs(scr_lins - 1, 0);
		scr_print("More? ");
		scr_refresh();
		c = scr_getkey();
		scr_putc(c);
		} while (c == 'y');
	scr_close();
	exit(SUCCEED);
	}																						                                                                                           /*
.PE
.PS 50
/* init - initialize the simulation */
void init()
	{
	short i;			/* loop counter; {0:ncars-1} */

	DQ_OPEN(&cars);
	for (i = 0; i < ncars; ++i)
		{
		p = (CAR *)malloc(sizeof(CAR));
		if (p == NULL)
			error("out of space", "");
		p->pos = i * (TRKSIZE / ncars);
		p->vel = 0;
		p->ident[0] = idents[i];
		p->ident[1] = '\0';
		DQ_APPEND(cars, p);
		}
	}
/* run - run the simulation for one time step */
void run()
	{
	short to_station;	/* distance to next station */
	short to_car;		/* distance to next car */
	short to_stop;		/* safe distance required to stop this car */
	short i;			/* loop counter */
	CAR *psucc;			/* ptr to successor of car p */

	EACH_DQ(cars, p, CAR)
		{
		plot_trk(p->pos / (TRKSIZE/100), ' ');
		p->pos = IMOD(p->pos + p->vel, TRKSIZE);
		plot_trk(p->pos / (TRKSIZE/100), p->ident[0]);
		to_station = IMOD(p->pos, TRKSIZE / 4);
		psucc = (CAR *)DQ_SUCC(cars, p);
		to_car = IMOD(psucc->pos - p->pos, TRKSIZE);
		for (i = 1, to_stop = 0; i <= p->vel+1; ++i)
			to_stop += i;
		if (to_car < 10)
			p->vel = 0;				/* screeching halt */
		else if (to_station <= 5)
			p->vel = rand() & 0x1;	/* random jerk in station */
		else if (MIN(to_station, to_car) < to_stop && p->vel > 0)
			--p->vel;				/* slow down */
		else
			++p->vel;				/* speed up */
		}
	scr_refresh();
	}
###screen.c
/* screen - environment-specific terminal functions
 * Idris version for ANSI terminal
 */
#include "local.h"
#include "screen.h"
#define get_7bit_char() (getchar() & 0177)	/* "raw" getchar never EOF */
short scr_mode = SCR_TTY;	/* screen mode - TTY or RAW */
short scr_lines = 24;	/* screen lines (default) */
short scr_cols = 80;	/* screen columns (default) */
/* scr_getkey - get a (coded) keyboard char */
SCR_CMDCHAR scr_getkey()
	{
	char c1, c2, c3;
	
	if ((c1 = get_7bit_char()) != '\33')
		return (c1);
	else if ((c2 = get_7bit_char()) == 'O')
		{
		if ((c3 = get_7bit_char()) == 'S')
			return (SCR_EXIT);
		return (getkey());
		}
	else if (c2 == '[')
		{
		switch (c3 = get_7bit_char())
			{
		case 'A':	return (SCR_UP);	/* no "break" needed - all returns */
		case 'B':	return (SCR_DOWN);
		case 'C':	return (SCR_RIGHT);
		case 'D':	return (SCR_LEFT);
		case 'H':	return (SCR_HOME);
		default:	return (scr_getkey());
			}
		}
	else
		return (scr_getkey());
	}
/* remark - print error message, appropriate for scr_mode */
void remark(s1, s2)
	char s1[], s2[];	/* strings to be printed */
	{
	if (scr_mode == SCR_TTY)
		fprintf(stderr, "%s %s\n", s1, s2);
	else
		{
		scr_curs(scr_lines-1, 0);
		scr_print("%s %s; hit any key to continue", s1, s2);
		scr_getkey();
		/* clear message from screen after response */
		/* print 79 spaces, 80 would cause CR on last line
		**     screen would scroll up (on most terminals  
		*/
		scr_curs(scr_lines-1, 0);
		scr_print("                                                    ");
		scr_print("                           ");
		}
	}
/* (new page)
.PE
.PS
 */
/* scr_open - initialize the terminal
 */
void scr_open()
	{
	system("stty +raw -echo");	/* slow but universal */
	printf("\33[>6h");			/* enter keypad-shifted mode */
	scr_mode = SCR_RAW;
	}
/* scr_close - re-establish normal terminal environment
 */
void scr_close()
	{
	printf("\33[>6l");			/* exit keypad-shifted mode */
	system("stty -raw +echo");	/* slow but universal */
	scr_mode = SCR_TTY;
	}
###screen86.c
/* screen - environment-specific terminal functions
 * PC Version - uses  bdos  function
 */
#include "local.h"
#include "screen.h"
short scr_mode = SCR_TTY;	/* screen mode - TTY or RAW */
short scr_lins = 24;	/* screen lines (default) */
short scr_cols = 80;	/* screen columns (default) */
/* scr_getkey - get a (coded) keyboard char */
SCR_CMDCHAR scr_getkey()
	{
	char c1;
	
	scr_refresh();
	if ((c1 = bdos(8)) != '\0')
		return (c1 & 0x7F);
	switch (c1 = bdos(8))
		{
	/* no "break" needed - all returns */
	case 'H':	return (SCR_UP);
	case 'P':	return (SCR_DOWN);
	case 'M':	return (SCR_RIGHT);
	case 'K':	return (SCR_LEFT);
	case 'G':	return (SCR_HOME);
	case ';':	return (SCR_EXIT);	/* F1 function key */
	default:	scr_beep();
				return (scr_getkey());
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
		scr_curs(scr_lins-1, 0);
		scr_cel();
		}
	}
/* scr_open - enter "raw" screen mode */
void scr_open()
	{
	scr_mode = SCR_RAW;	/* otherwise no work; bdos(8) is unbuffered */
	}																				/*
.PE
.PS
/* scr_close - restore normal tty state */
void scr_close()
	{
	scr_mode = SCR_TTY;
	}
###screenU.c
/* screen - environment-specific terminal functions
 * UNIX version for ANSI terminal
 */
#include "local.h"
#include "screen.h"
#define get_7bit_char() (getchar() & 0177)	/* "raw" getchar never EOF */
short scr_mode = SCR_TTY;	/* screen mode - TTY or RAW */
short scr_lins = 24;	/* screen lines (default) */
short scr_cols = 80;	/* screen columns (default) */
/* scr_getkey - get a (coded) keyboard char */
SCR_CMDCHAR scr_getkey()
	{
	char c1, c2;
	
	scr_refresh();
	if ((c1 = get_7bit_char()) != '\33')
		return (c1);
	else if ((c2 = get_7bit_char()) == 'O')
		{
		if (get_7bit_char() == 'S')		/* F1 function key */
			return (SCR_EXIT);
		scr_beep();
		return (scr_getkey());
		}
	else if (c2 == '[')
		{
		switch (get_7bit_char())
			{
		case 'A':	return (SCR_UP);	/* no "break" needed - all returns */
		case 'B':	return (SCR_DOWN);
		case 'C':	return (SCR_RIGHT);
		case 'D':	return (SCR_LEFT);
		case 'H':	return (SCR_HOME);
		default:	scr_beep();
					return (scr_getkey());
			}
		}
	else
		{
		scr_beep();
		return (scr_getkey());
		}
	}																		                                                                      /*
.PE
.PS 30
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
		scr_curs(scr_lins-1, 0);
		scr_cel();
		}
	}
/* scr_open - initialize the terminal */
void scr_open()
	{
	system("stty raw -echo");	/* slow but universal */
	printf("\33[>6h");			/* keypad-shifted; not universal ANSI */
	scr_mode = SCR_RAW;
	}
/* scr_close - re-establish normal terminal environment */
void scr_close()
	{
	printf("\33[>6l");			/* exit keypad-shifted mode */
	system("stty -raw echo");	/* slow but universal */
	scr_mode = SCR_TTY;
	}
###short_order.c
/* short_order - put two short integers into order
 */
#include "local.h"
void short_order(pi, pj)
	short *pi, *pj;
	{
	short t;

	if (*pj < *pi)
		t = *pi, *pi = *pj, *pj = t;
	}
###sisort.c
/* sisort - string version of isort
 * sorts an array of pointers to char
 */
#define ISORT_NAME sisort
#define ISORT_TYPE char *
#define ISORT_GT(a, b) (strcmp(a, b) > 0)
#include "isort.h"
###sqksort.c
/* sqksort - string version of qksort
 */
#define QKSORT_NAME sqksort
#define QKSORT_TYPE char *
#define QKSORT_GT(a, b) (strcmp(a, b) > 0)
#include "qksort.h"
###sqksortm.c
/* sqksort - string version of qksort
 */
#define QKSORT_NAME sqksort
#define QKSORT_TYPE char *
#define QKSORT_GT(a, b) (strcmp(a, b) > 0)
#include "qksort.h"
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static char * a[10000];
	static char pie[10001] = {0};
	double nlogn;
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < 10000; ++i)
		pie[i] = rand() & 63;
	for (i = 0; i < lim; ++i)
		a[i] = &pie[rand() % 10000];
	cputim();
	QKSORT_NAME(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	nlogn = lim * log((double)lim) / log(2.);
	printf("log N = %.3f\n", log((double)lim) / log(2.));
	printf("N log N = %.3f\n", nlogn);
	printf("cpu time = %.2f N log N (usec)\n", tim / nlogn);
	for (i = 0; i < lim-1; ++i)
		{
		if (QKSORT_GT(a[i], a[i+1]))
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###sqksortm2.c
/* sqksort - string version of qksort
 */
#define QKSORT_NAME sqksort
#define QKSORT_TYPE char *
#define QKSORT_GT(a, b) \
	((a)[0] != (b)[0] ? (a)[0] > (b)[0] : strcmp(a, b) > 0)
#include "qksort.h"
#ifdef TRYMAIN
#include <math.h>
main(ac, av)
	int ac;
	char *av[];
	{
	int i;
	int lim;
	static char * a[10000];
	static char pie[10001] = {0};
	double nlogn;
	double tim;
	long cputim();

	lim = atoi(av[1]);
	printf("lim=%d\n", lim);
	for (i = 0; i < 10000; ++i)
		pie[i] = rand() & 63;
	for (i = 0; i < lim; ++i)
		a[i] = &pie[rand() % 10000];
	cputim();
	QKSORT_NAME(a, lim);
	tim = cputim() * 1e6 / 60;
	printf("cpu time = %.3f (sec)\n", tim / 1e6);
	nlogn = lim * log((double)lim) / log(2.);
	printf("log N = %.3f\n", log((double)lim) / log(2.));
	printf("N log N = %.3f\n", nlogn);
	printf("cpu time = %.2f N log N (usec)\n", tim / nlogn);
	for (i = 0; i < lim-1; ++i)
		{
		if (QKSORT_GT(a[i], a[i+1]))
			error("bad value", "");
		}
	exit(SUCCEED);
	}
#endif
###sqrtx.c
/* sqrttst - test accuracy of sqrt
 */
#include "local.h"
main()
	{
	double x, y;

	for (x = 1.; x <= 25.; x = x + 1.)
		{
		y = sqrt(x) * sqrt(x);
		if (y != x)
			printf("%5.1f %21.17f %10.2e\n", x, y, x-y);
		}
	}
###sqrtx.out
  2.0   2.00000000000000010  -5.55e-17
  3.0   3.00000000000000010  -5.55e-17
  7.0   6.99999999999999990   2.22e-16
  8.0   8.00000000000000020  -2.22e-16
 10.0  10.00000000000000000  -2.22e-16
 12.0  12.00000000000000000  -2.22e-16
 15.0  15.00000000000000000  -2.22e-16
 17.0  17.00000000000000100  -4.44e-16
 18.0  18.00000000000000000   4.44e-16
 21.0  21.00000000000000000   4.44e-16
 22.0  22.00000000000000000   4.44e-16
 23.0  22.99999999999999900   8.88e-16
###sqrtx86.out
  2.0   2.00000000000000044  -4.44E-16
  3.0   2.99999999999999955   4.44E-16
  5.0   5.00000000000000088  -8.88E-16
  6.0   5.99999999999999911   8.88E-16
  7.0   7.00000000000000088  -8.88E-16
  8.0   8.00000000000000177  -1.78E-15
 10.0  10.00000000000000176  -1.78E-15
 12.0  11.99999999999999821   1.78E-15
 13.0  12.99999999999999822   1.78E-15
 15.0  15.00000000000000176  -1.78E-15
 18.0  17.99999999999999642   3.55E-15
 19.0  19.00000000000000355  -3.55E-15
 20.0  20.00000000000000353  -3.55E-15
 23.0  22.99999999999999642   3.55E-15
 24.0  23.99999999999999642   3.55E-15
###st_close.c
/* st_close - close the stack accessed via *headp */
#include "stack.h"
void st_close(headp)
	ST_NODE **headp;
	{
	ST_NODE *p;
	ST_NODE *pnext;

	for (p = *headp; p != NULL; p = pnext)
		{
		pnext = p->next;
		free(p);	/* p is (momentarily) undefined */
		}
	*headp = NULL;		/* prevent dangling ptr */
	}
###st_main.c
/* st_main - test routine for stack package
 */
#include "local.h"
#include "stack.h"

#define NAME_NODE struct name_node
NAME_NODE
	{
	NAME_NODE *next;
	char data[4];
	};
NAME_NODE *head = NULL;		/* head node of stack */
NAME_NODE *p = NULL;		/* current node */
void show_cmds(), push_name(), pop_name(), dump_names();
																					                                                                         /*
.PE
.PS 26
/* st_main (main) */
main()
	{
	char buf[2];			/* buffer for input */

	ST_OPEN(&head);			/* open the stack */
	show_cmds();
	while (getreply("?: ", buf, 2) != EOF)
		{
		switch (buf[0])
			{
		case '+':
			push_name();
			break;
		case '-':
			pop_name();
			break;
		case '=':
			dump_names();
			break;
		case '0':
			ST_CLOSE(&head);
			ST_OPEN(&head);		/* open the stack again */
			break;
		case '?':
			show_cmds();
			break;
		default:
			printf("unknown command: %c\n", buf[0]);
			show_cmds();
			break;
			}
		}
	exit(SUCCEED);
	}																															                                                          /*
.PE
.PS  30
/* show_cmds -- show legal commands */
void show_cmds()
	{
	printf("Type + to push, - to pop, = to print, 0 to reset:\n");
	}
/* push_name - push new name on stack */
void push_name()
	{
	p = (NAME_NODE *)malloc(sizeof(NAME_NODE));
	if (p == NULL)
		error("out of space", "");
	if (getreply("name: ", p->data, 4) == EOF)
		 error("unexpected EOF", ""); 
	ST_PUSH(&head, p);
	}
/* pop_name - pop a name off stack */
void pop_name()
	{
	p = (NAME_NODE *)ST_POP(&head);
	if (p == NULL)
		printf("EMPTY STACK\n");
	else
		{
		printf("name= %s\n", p->data);
		free(p);
		}
	}
/* dump_names - print the current stack of names */
void dump_names()
	{
	if (ST_IS_EMPTY(head))
		printf("EMPTY STACK\n");
	else
		EACH_ST(head, p)
			printf("name= %s\n", p->data);
	}
###st_pop.c
/* st_pop - detach top node of stack and return pointer to it */
#include "stack.h"
ST_NODE *st_pop(headp)
	ST_NODE **headp;
	{
	ST_NODE *p;

	if (*headp == NULL)
		return (NULL);
	p = *headp;			/* p points to top of stack */
	*headp = p->next;	/* headp now points to next node (or is NULL) */
	p->next = NULL;		/* prevent dangling ptr */
	return (p);
	}
###st_push.c
/* st_push - install ST_NODE at head of stack */
#include "stack.h"
void st_push(headp, p)
	ST_NODE **headp;
	ST_NODE *p;
	{
	p->next = *headp;	/* p->next now points to previous head (or is NULL) */
	*headp = p;			/* *headp now points to p */
	}
###strcspn.c
/* strcspn - xxx
 */
size_t strcspn(s1, s2)
char s1[];
char s2[];
	{					/* strcspn */
	register size_t i;
	register char *p;

	for (i = 0;  s1[i] != '\0';  ++i)
		{
		for (p = s2;  *p != '\0';  ++p)
			{
			if (*p == s1[i])
				return (i);
			}
		}
	return (i);
	}					/* strcspn */
###strfit.c
/* strfit - copy s2 to s1, chopping to n chars
 * return YES if sufficient space, no if not
 */
#include "local.h"

bool strfit(s1, s2, n)
	char s1[];
	char s2[];
	size_t n;
	{
	size_t i;

	for (i = 0; i < n-1 && s2[i] != '\0'; ++i)
		s1[i] = s2[i];
	s1[i] = '\0';
	return (s2[i] == '\0');
	}
###strspn.c
/* strspn - xxx
 */
#include "local.h"
size_t strspn(s1, s2)
char s1[];
char s2[];
	{					/* strspn */
	register size_t i;
	register char *p;

	for (i = 0;  s1[i] != '\0';  ++i)
		{
		for (p = s2;  *p != '\0';  ++p)
			{
			if (*p == s1[i])
				break;
			}
		if (*p == '\0')
			return (i);
		}
	return (i);
	}					/* strspn */
###strtok.c
/* strtok - return pointer to next token in string
 */
#include "local.h"
static char *nextpos = NULL;

char *strtok(s1, s2)
char s1[];
char s2[];
	{					/* strtok */
	char *returnp;
	size_t nchars;

	if (s1 != NULL)
		nextpos = s1;
	nchars = strspn(nextpos, s2);	/* length of sep char sequence */
	if (*nextpos == '\0')
		return (NULL);	/* no more tokens left */
	nextpos += nchars;
	nchars = strcspn(nextpos, s2);	/* length of token char sequence */
	nextpos[nchars] = '\0';
	returnp = nextpos;
	nextpos += nchars + 1;
	return (returnp);
	}					/* strtok */
###testmain.c
#include "qsort.c"
#define SORTFN(a, lim) qsort(a, lim, sizeof(a[0]), intcmp)
static int intcmp(a, b)
	int *a, *b;
	{
	if (*a > *b)
		return (1);
	else if (*a == *b)
		return (0);
	else
		return (-1);
	}
#include "qkmain.h"
###tr_close.c
/* tr_close - free a tree or subtree */
#include "tree.h"
void tr_close(plt)
	TR_NODE **plt;				/* ptr to link-in of tree */
	{
	TR_NODE *pt;				/* ptr to root of tree */

	pt = *plt;					/* pt now points to root of tree */
	if (pt == NULL)				/* if link-in is NULL, */
		return;					/* nothing to do, so return */
	TR_CLOSE(&pt->left);		/* free left subtree */
	TR_CLOSE(&pt->right);		/* free right subtree */
	free(pt);					/* free the node itself */
	}
###tr_delete.c
/* tr_delete - delete the node **pln from tree *plt
 */
#include "tree.h"
void tr_delete(plt, pln, cmpfn)
	TR_NODE **plt;			/* ptr to link-in of tree */
	TR_NODE **pln;			/* ptr to link-in of node */
	int (*cmpfn)();			/* ptr to comparison function */
	{
	TR_NODE *pn;			/* ptr to specific node of tree */
	TR_NODE *ps;			/* ptr to node's successor */
	TR_NODE **pls;			/* ptr to link-in of successor */

	pn = *pln;					/* pn pts to the node to delete */
	if (pn->right == NULL)		/* if node has no right subtree, */
		{
		if (pn->left == NULL)	/* if node has no children, */
			*pln = NULL;		/* replacement for pn is NULL */
		else					/* if node has L subtree, but not R, */
			*pln = pn->left;	/* replacement is pn's L subtree */
		}
	else if (pn->left == NULL)	/* if node has R subtree, but not L */
		*pln = pn->right;		/* replacement is pn's R subtree */
	else						/* if node has R and L subtrees *
		{
		pls = TR_LNEXT(plt, pn, cmpfn);	/* get ptr to link-in of succ */
		ps = *pls;				/* ps now points to successor */
		*pls = ps->right;		/* succ's R subtree takes succ's place */
		ps->right = pn->right;	/* succ acquires node's R ... */
		ps->left = pn->left;	/* ... and L subtree */
		*pln = ps;				/* replacement is successor */
		}
	free(pn);
	}
###tr_detach.c
/* tr_detach - detach node (or subtree) from tree, given link-in
 */
#include "tree.h"
TR_NODE *tr_detach(pln)
	TR_NODE **pln;		/* ptr to link-in of node to be detached */
	{
	TR_NODE *p;

	p = *pln;			/* hold the address of the node */
	*pln = NULL;		/* detach the node */
	return (p);			/* return its address */
	}
###tr_insert.c
/* tr_insert - insert a single node in tree (recursive version) */
#include "tree.h"
void tr_insert(plt, pn, cmpfn)
	TR_NODE **plt;		/* ptr to link-in of tree or subtree */
	TR_NODE *pn;		/* ptr to node to be inserted */
	int (*cmpfn)();		/* ptr to comparison function */
	{
	TR_NODE *pt;		/* ptr to tree (or subtree) */
	int cmp;			/* result of key comparison; neg, zero, or pos */

	pt = *plt;			/* pt now pts to current tree (if any) */
	if (pt == NULL)		/* if plt pointed to a null pointer, */
		{
		pn->left = pn->right = NULL;	/* has no sub trees yet */
		*plt = pn;		/* then this is the place to install pn; do so */
		}
	else if ((cmp = (*cmpfn)(pn, pt)) == 0)	/* if already in tree, */
		return;								/* then don't install */
	else if (cmp < 0)				/* if node's key compares low */
		TR_INSERT(&pt->left, pn, cmpfn);	/* then insert in left tree */
	else							/* otherwise, */
		TR_INSERT(&pt->right, pn, cmpfn);	/* insert in right tree */
	}
###tr_lfin1.c
/* tr_lfind - find node matching a specified key
 * recursive version
 * never returns NULL
 */
#include "tree.h"
TR_NODE **tr_lfind(plt, pn, cmpfn)
	TR_NODE **plt;			/* ptr to link-in of tree or subtree */
	TR_NODE *pn;			/* ptr to structure containing key to match */
	int (*cmpfn)();			/* ptr to comparison function */
	{
	TR_NODE *pt;			/* ptr to current tree */
	int cmp;				/* comparison result: neg, zero, pos */

	pt = *plt;				/* pt now points to current tree */
	if (pt == NULL)			/* if plt points to a null pointer, */
		return (plt);		/* the data isn't in the tree */
	else if ((cmp = (*cmpfn)(pn, pt)) == 0)	/* if key already in tree, */
		return (plt);					/* return its node's link-in */
	else if (cmp < 0)					/* if key compares low, */
		return (TR_LFIND(&pt->left, pn, cmpfn));	/* search in left tree */
	else								/* otherwise, */
		return (TR_LFIND(&pt->right, pn, cmpfn));	/* search in right tree */
	}
###tr_lfin2.c
/* tr_lfind - find node matching a specified key
 * iterative version
 * never returns NULL
 */
#include "tree.h"
TR_NODE **tr_lfind(plt, pn, cmpfn)
	TR_NODE **plt;			/* ptr to link-in of tree or subtree */
	TR_NODE *pn;			/* ptr to structure containing key to match */
	int (*cmpfn)();			/* ptr to comparison function */
	{
	TR_NODE *pt;			/* ptr to current tree */
	int cmp;				/* comparison result: neg, zero, pos */

	FOREVER
		{
		pt = *plt;					/* pt now points to current tree */
		if (pt == NULL)				/* if plt points to a null pointer, */
			return (plt);			/* the data isn't in the tree */
		else if ((cmp = (*cmpfn)(pn, pt)) == 0) /* if key already in tree, */
			return (plt);			/* return its node's link-in */
		else if (cmp < 0)			/* if key compares low, */
			plt = &pt->left;		/* then search in left tree */
		else						/* otherwise, */
			plt = &pt->right;		/* search in right tree */
		}
	}
###tr_lfind.c
/* tr_lfind - find node matching a specified key
 * iterative version
 * never returns NULL
 */
#include "tree.h"
TR_NODE **tr_lfind(plt, pn, cmpfn)
	TR_NODE **plt;			/* ptr to link-in of tree or subtree */
	TR_NODE *pn;			/* ptr to structure containing key to match */
	int (*cmpfn)();			/* ptr to comparison function */
	{
	TR_NODE *pt;			/* ptr to current tree */
	int cmp;				/* comparison result: neg, zero, pos */

	FOREVER
		{
		pt = *plt;					/* pt now points to current tree */
		if (pt == NULL)				/* if plt points to a null pointer, */
			return (plt);			/* the data isn't in the tree */
		else if ((cmp = (*cmpfn)(pn, pt)) == 0) /* if key already in tree, */
			return (plt);			/* return its node's link-in */
		else if (cmp < 0)			/* if key compares low, */
			plt = &TR_LEFT(pt);		/* then search in left tree */
		else						/* otherwise, */
			plt = &TR_RIGHT(pt);		/* search in right tree */
		}
	}
###tr_lfirst.c
/* tr_lfirst - find first node in tree
 * Returned value is ptr to link-in of first node
 * never returns NULL, and never points to null ptr
 */
#include "tree.h"
TR_NODE **tr_lfirst(plt)
	TR_NODE **plt;			/* ptr to (non-null) link-in of tree */
	{
	TR_NODE **pln;			/* ptr to link-in of node */

	for (pln = plt; (*pln)->left != NULL; pln = &(*pln)->left)
		;
	return (pln);
	}
###tr_lnext.c
/* tr_lnext - find the successor of node pn
 * Returned value is ptr to link-in of successor node
 * Never returns NULL; may return addr of a null ptr
 */
#include "tree.h"
static TR_NODE *nullp = NULL;	/* used to return addr of null ptr */
TR_NODE **tr_lnext(plt, pn, cmpfn)
	TR_NODE **plt;			/* ptr to link-in of tree */
	TR_NODE *pn;			/* ptr to specific node of tree */
	int (*cmpfn)();			/* ptr to comparison function */
	{
	TR_NODE **pls;			/* ptr to link-in of successor */
	TR_NODE **pln;			/* ptr to link-in of a TR_NODE */

	if (pn->right != NULL)	/* if node has a right subtree, */
		{ /* return ptr to link-in of "leftmost"  node in right subtree */
		return (TR_LFIRST(&pn->right));
		}
	else						/* if node has no right subtree, */
		{		/* return ptr to link-in of parent of pn */
		pln = TR_LPFIND(plt, pn, cmpfn);
		if (*pln != NULL && (*pln)->left == pn)
			return (pln);		/* node is left subtree of parent */
		else
			return (&nullp);	/* node is rightmost in tree */
		}
	}
###tr_lpfind.c
/* tr_lpfind - find parent of node matching a specified key
 * iterative version
 * never returns NULL; may return addr of null ptr
 */
#include "tree.h"
static TR_NODE *nullp = NULL;	/* used to return addr of null ptr */
TR_NODE **tr_lpfind(plt, pn, cmpfn)
	TR_NODE **plt;			/* ptr to link-in of tree or subtree */
	TR_NODE *pn;			/* ptr to structure containing key to match */
	int (*cmpfn)();			/* ptr to comparison function */
	{
	TR_NODE *pt;			/* ptr to current tree */
	TR_NODE **plp;			/* ptr to link-in of parent of tree */
	int cmp;				/* comparison result: neg, zero, pos */

	plp = &nullp;					/* root has no parent */
	FOREVER
		{
		pt = *plt;					/* pt now points to current tree */
		if (pt == NULL)				/* if plt points to a null pointer, */
			return (&nullp);		/* the data isn't in the tree */
		else if ((cmp = (*cmpfn)(pn, pt)) == 0) /* if key already in tree, */
			return (plp);			/* return its parent's link-in */
		plp = plt;				/* before starting subtree, save plt */
		if (cmp < 0)				/* if key compares low, */
			plt = &pt->left;		/* then search in left tree */
		else						/* otherwise, */
			plt = &pt->right;	/* search in right tree */
		}
	}
###tr_main.c
/* tr_main - main pgm to test tree pkg
 */
#include "tree.h"
#define DATA_NODE struct data_node
DATA_NODE
	{
	DATA_NODE *right;
	DATA_NODE *left;
	char data[4];
	};

/* mknode - allocate a new node with specified key
 */
DATA_NODE *mknode(key)
	char key[];
	{
	DATA_NODE *p;

	p = (DATA_NODE *)malloc(sizeof(DATA_NODE));
	if (p == NULL)
		return (NULL);
	strncpy(p->data, key, sizeof(p->data));
	TR_RIGHT(p) = TR_LEFT(p) = NULL;
	return (p);
	}

/*
	tr_pr -- print the contents of the tree
*/
void tr_pr(pt)
	DATA_NODE *pt;	/* ptr to root of tree */
	{
	static short level = -1;
	short i;

	if (pt == NULL)
		return;
	++level;
	tr_pr(TR_LEFT(pt));
	for (i = 0; i < level; ++i)
		putchar('.');
	printf("%s\n", pt->data);
	tr_pr(TR_RIGHT(pt));
	--level;
	}

/*
	comp_str -- comparison function 
*/

int comp_str(p1, p2)
	DATA_NODE *p1;
	DATA_NODE *p2;
	{
	return(strcmp(p1->data, p2->data));
	}


main()
	{

	DATA_NODE *root = NULL;
	DATA_NODE *p = NULL;
	DATA_NODE **q = NULL;
	static char *keys[] = 
		{"d", "b", "c", "a", "e"};
	int i = 0;
	int comp_str();		/* comparison function */

	for (i = 0; i < DIM(keys); ++i)
		{
		p = (DATA_NODE *) mknode(keys[i]);
		TR_INSERT(&root, p, comp_str);
		q = (DATA_NODE **) TR_LFIND(&root, p, comp_str);
		tr_pr(root);
		printf("q=<%s>\n", (*q)->data);
		q = (DATA_NODE **) TR_LPFIND(&root, p, comp_str);
		printf("parent=<%s>\n", *q != NULL ? (*q)->data : "NULL");
		q = (DATA_NODE **) TR_LFIND(&root, p, comp_str);
		TR_DELETE(&root, q, comp_str);
		printf("after deletion\n");
		tr_pr(root);
		printf("after re-inserting\n");
		p = (DATA_NODE *) mknode(keys[i]);
		TR_INSERT(&root, p, comp_str);
		q = (DATA_NODE **) TR_LFIND(&root, p, comp_str);
		tr_pr(root);
		printf("q=<%s>\n\n", (*q)->data);
		}
	p = mknode("a");	/* dup, already in tree */
	TR_INSERT(&root, p, comp_str);
	printf("after dup:\n");
	tr_pr(root);

	p = mknode("x");	/* not in tree */
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	printf("LFIND(missing) %s properly NULL\n", *q == NULL ? "is" : "NOT");

	q = (DATA_NODE **)TR_LPFIND(&root, p, comp_str);
	printf("LPFIND(missing) %s properly NULL\n", *q == NULL ? "is" : "NOT");

	p = mknode("d");	/* the root */
	q = (DATA_NODE **)TR_LPFIND(&root, p, comp_str);
	printf("LPFIND(root) %s properly NULL\n", *q == NULL ? "is" : "NOT");

	p = mknode("d");	/* root, 2 subtrees */
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	q = (DATA_NODE **)TR_LNEXT(&root, *q, comp_str);
	printf("LNEXT(d) == <%s>\n", *q != NULL ? (*q)->data : "NULL");

	p = mknode("a");	/* no subtrees, successor is parent */
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	q = (DATA_NODE **)TR_LNEXT(&root, *q, comp_str);
	printf("LNEXT(a) == <%s>\n", *q != NULL ? (*q)->data : "NULL");

	p = mknode("e");	/* no subtrees, has no successor */
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	q = (DATA_NODE **)TR_LNEXT(&root, *q, comp_str);
	printf("LNEXT(e) == <%s>\n", *q == NULL ? "NULL" : (*q)->data);

	p = mknode("e");	/* no subtrees, has no successor */
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	q = (DATA_NODE **)TR_LFIRST(q);
	printf("LFIRST(e) == <%s>\n", *q == NULL ? "NULL" : (*q)->data);

	p = mknode("d");
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	TR_DELETE(&root, q, comp_str);
	printf("\nAfter deleting 'd':\n");
	tr_pr(root);

	p = mknode("b");
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	TR_DELETE(&root, q, comp_str);
	printf("\nAfter deleting 'b':\n");
	tr_pr(root);

	p = mknode("c");
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	TR_DELETE(&root, q, comp_str);
	printf("\nAfter deleting 'c':\n");
	tr_pr(root);

	p = mknode("a");
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	TR_DELETE(&root, q, comp_str);
	printf("\nAfter deleting 'a':\n");
	tr_pr(root);

	p = mknode("e");
	q = (DATA_NODE **)TR_LFIND(&root, p, comp_str);
	TR_DELETE(&root, q, comp_str);
	printf("\nAfter deleting 'e':\n");
	tr_pr(root);
	}

###tst_sort.c
/* tst_sort - returns YES if array a is sorted
 * specialized "short" version
 */
#include "local.h"
bool tst_sort(a, n)
	short a[];
	index_t n;
	{
	index_t i;

	if (n <= 0)
		return (NO);
	for (i = 1; i < n; ++i)
		{
		/* a[0:i-1] => sorted */
		if (a[i] < a[i-1])		/* compare adjacent elements */
			return (NO);
		}
	/* a[0:n-1] => sorted */
	return (YES);
	}
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
