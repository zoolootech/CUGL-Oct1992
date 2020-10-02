/*
 * z8   assembler hgw july 1983: h.-g. willers
 *
 *      register syntax:
 *              register:               r0 - r127, r240 - r255
 *              working register:       w0 - w15
 *
 */
#include <stdio.h>
#include <setjmp.h>

#define NCPS    33	/* number of characters per symbol */
#define NCPLE   NCPS+2  /* number of characters per listing element */
#define NLEPL   1       /* number of listing elements per line */
#define	NBIN	32
#define NTSYM   512     /* length of numeric-label table */
#define NERR	10
#define NINPUT	128
#define NCODE	128
#define NTIT	64
#define HASHSIZ 503     /* hashtable size */
			/* must be prime */
			/* more primes:	*/
			/* 503, 607, 701, 809, 907, 1009 */
			/* see: handbook of math. functions p. 879 */

#define NLIST	0
#define SLIST	1
#define ALIST	2
#define CLIST	3

typedef	unsigned addr_t;

struct	sym
{
	char    *s_name;
	char	s_type;
	char	s_flag;
	addr_t	s_addr;
};

#define S_UND	0
#define S_ABS	1

#define S_OP0   2
#define S_OP2   4
#define S_OP3   5
#define S_OP4   6
#define S_OP5   7
#define S_OP6   8
#define S_OP7   9
#define S_OP8   10
#define S_OP9   11
#define S_OP10  12
#define S_OP11  13
#define S_OP12  14
#define S_OP13  15
#define S_OP14  16
#define S_OP15  17

#define S_LIT   19
#define S_REG   20
#define S_WREG  21
#define S_IR    22              /* register indirect */
#define S_WIR   23              /* working register indirect */
#define S_INDEX 24

#define	S_WORD	25
#define S_ASCII	26
#define S_ASCIZ	27
#define S_BLKB	28
#define S_PAGE	29
#define S_TITLE	30
#define S_BYTE	31
#define S_HLIST	32
#define S_OLIST	33
#define S_INTEL	34
#define S_MKBUG	35
#define	S_LIST 36
#define S_NLIST 37
#define S_START 38

#define WRMASK  0xe0            /* msbs for  working reg. add. */
#define CALL    0xd6            /* direct call */
#define CALLI   0xd4            /* indirect call */
#define SOB     0x0a            /* sob-instruction */
#define JUMP    0x8d            /* jump-always instruction */
#define JUMPI   0x30            /* jump indirect */
#define SRP     0x31
#define INCW    0x0e            /* inc working register */

#define	S_MDF	01		/* Mult. def */
#define S_ASG	02		/* Assigned def */

struct	tsym
{
	int	t_num;
	addr_t	t_addr;
};

struct	tsymp
{
	struct	tsym *t_fp;
	struct	tsym *t_bp;
};

struct	addr
{
	char	a_type;
	char	a_reg;
	addr_t	a_addr;
};

int	line;
int	page;
int	lop;
int	pass;
int	ntsym;
int	sflag = 0;		/* symbol-table-flag */
int	lflag;
int	nflag;
int	eflag;
addr_t	laddr;
int	lmode;
int	ldis = 0;		/* listing disable */
				/* =1 listing is disabled */
int     hlist = 1;              /* hex listing is default */
int	ninst = 0;		/* number of installs into symbol table */
char	*ep;
char	eb[NERR];
char	*ip;
char	ib[NINPUT];
char	*cp;
char	cb[NCODE];
char	tb[NTIT];
int	bo;
int	ba;
char	bb[NBIN];

unsigned int    startadr = 0xffff;      /* start address */

char liname[80];
char obname[80];	/* listing and object filenames */
char stbnam[80];

jmp_buf env;

long    daytime;

long    time();
char *ctime();
char *malloc();
struct sym *lookup();
struct sym *addr1();

struct sym *dot;	/* pointer to . value into symbol table */

struct sym sym[HASHSIZ] = {0};	/* space for symbol table */

extern struct sym optab[];

struct	tsym tsym[NTSYM];
struct	tsymp tsymp[10];

/* lower opcode nibbles for class-8 opcodes */
/* ordered as follows:  destination, source, nibble */
/* table ends at zero destination */

char c8nib[] = { S_WREG, S_WREG, 2,
		S_WREG, S_WIR, 3,
		S_WREG, S_WREG, 4,
		S_WREG, S_REG, 4,
		S_REG, S_WREG, 4,
		S_REG, S_REG, 4,
		S_WREG, S_WIR, 5,
		S_WREG, S_IR, 5,
		S_REG, S_WIR, 5,
		S_REG, S_IR, 5,
		S_WREG, S_LIT, 6,
		S_REG, S_LIT, 6,
		S_WIR, S_LIT, 7,
		S_IR, S_LIT, 7,
		0, 0, 0
		};

char movtab[] = { S_WREG, S_LIT, 0x0c,
		S_WREG, S_REG, 0x08,
		S_WREG, S_WREG, 0x08,
		S_REG,  S_WREG, 0x09,
		S_WREG, S_INDEX, 0xc7,
		S_INDEX, S_WREG, 0xd7,
		S_WREG, S_WIR, 0xe3,
		S_WIR, S_WREG, 0xf3,
		S_REG, S_REG, 0xe4,
		S_REG, S_IR, 0xe5,
		S_REG, S_LIT, 0xe6,
		S_IR, S_LIT, 0xe7,
		S_IR, S_REG, 0xf5,
		S_WIR, S_REG, 0xf5,
		S_IR, S_WREG, 0xf5,
		S_WIR, S_WREG, 0xf5,
		0, 0, 0
		};

char c13tab[] = {S_WREG, S_WIR, 1,
		S_WIR, S_WREG, 2,
		0, 0, 0
		};

FILE	*lfp;
FILE	*ofp;
FILE	*sfp;

char *strrchr();
char *strchr();

main(argc, argv)
char *argv[];
{
	register char *p;
	register c, i;
	struct tsym *tp;
	char *iptr;		/* index-pointer */
	char *fn;
	FILE *afile();

	fn = NULL;
	for (i=1; i<argc; ++i) {
		p = argv[i];
		if (*p == '-') {
			++p;
			while (c = *p++)
				switch(c) {

				case 'l':
				case 'L':
					++lflag;
					break;

				case 'n':
				case 'N':
					++nflag;
					break;

				default:
					fprintf(stderr, "%c: ignored.\n", c);
				}
		} else
			fn = p;
	}
	if (fn != NULL) {
		sfp = afile(fn, 0);
		if((iptr = strrchr(fn, '/')) == NULL)
			strcpy(liname, fn);
		else
			strcpy(liname, ++iptr);
		if((iptr = strrchr(liname, '.')) != NULL)
			*iptr = 0;              /* truncate suffix (if any) */
		if (!nflag) {
			strcpy(obname, liname);
			strcat(obname, ".o");
		}
		if (lflag) {
			strcat(liname, ".l");
			lfp = afile(liname, 1);
		}
		if(strcmp (fn, obname) == 0) {
			fprintf(stderr, "Would overwrite %s\n", fn);
			exit(1);
		}
		ofp = afile(obname, 1);
		for (pass=0; pass<2; ++pass) {
			if(pass == 0)
				insopc();          /* install opcodes */
			dot = lookup(".", 0, 0);   /* locate . in table */
			dot->s_type = S_ABS;
			dot->s_flag = S_ASG;
			dot->s_addr = 0;
			line = 0;
			page = 1;
			lop =  60;
			if (pass == 1) {
				rewind(sfp);
				for (i=0; i<10; ++i) {
					tsymp[i].t_fp = NULL;
					tsymp[i].t_bp = NULL;
					tp = tsym;
					while (tp < &tsym[ntsym]) {
						if (tp->t_num == i) {
							tsymp[i].t_fp = tp;
							break;
						}
						++tp;
					}
				}
			}
			while (fgets(ib, sizeof ib, sfp) != NULL) {
				iptr = strchr(ib,'\n');
				*iptr = 0;
				++line;
				cp = cb;
				ep = eb;
				ip = ib;
				eflag = 0;
				setjmp(env);
				if (eflag == 0)
					xasm();
				if (pass == 1) {
					diag();
					list();
				}
			}
		}
		if (!nflag) {
			bflush();
			beof();
		}
		prstb();
		prstat();
	}
}

FILE *
afile(fn, wf)
char *fn;
{
	register FILE *fp;

	fp = fopen(fn, wf?"w":"r");
	if (fp == NULL) {
		fprintf(stderr, "%s: cannot open.\n", fn);
		exit(1);
	}
	return (fp);
}

xasm()
{
	register c;
	register addr_t a;
	struct sym *sp;
	struct tsym *tp;
	int b, d, op;
	char *p, id[NCPS];
	struct addr a1, a2;

	laddr = dot->s_addr;
	lmode = SLIST;
loop:
	while ((c = getnb()) == ';')
		;
	if (c == 0 || c == '/')
		return;
	if (isdigit(c)) {
		if (getnb() != ':')
			err('q');
		c -= '0';
		if (pass == 0) {
			if (ntsym >= NTSYM)
				error("Too many temp. symbols!\n");
			tsym[ntsym].t_num = c;
			tsym[ntsym].t_addr = dot->s_addr;
			++ntsym;
		}
		else {
			tp = tsymp[c].t_fp;
			tsymp[c].t_bp = tp;
			tsymp[c].t_fp = NULL;
			++tp;
			while (tp < &tsym[ntsym]) {
				if (tp->t_num == c) {
					tsymp[c].t_fp = tp;
					break;
				}
				++tp;
			}
		}
		goto loop;
	}
	if (!isidc(c))
		err('q');
	getid(c, id);
	if ((c = getnb()) == ':') {
		sp = lookup(id, 1, 0);
		if (pass == 0) {
			if (sp->s_type!=S_UND && (sp->s_flag&S_ASG)==0)
				sp->s_flag |= S_MDF;
			sp->s_type = S_ABS;
			sp->s_addr = dot->s_addr;
		} else {
			if ((sp->s_flag&S_MDF) != 0)
				err('m');
			if (sp->s_type!=S_ABS || sp->s_addr!=dot->s_addr)
				err('p');
		}
		lmode = ALIST;
		goto loop;
	}
	if (c == '=') {
		sp = lookup(id, 1, 0);
		if (sp->s_type!=S_UND && (sp->s_flag&S_ASG)==0)
			err('m');
		expr(&a1);
		sp->s_addr = laddr = a1.a_addr;
		sp->s_flag = S_ASG;
		sp->s_type = a1.a_type;
		lmode = ALIST;
		goto loop;
	}
	unget(c);
	lmode = CLIST;
	if ((sp=lookup(id, 0, 0)) == NULL) {
		err('o');
		return;
	}
	op = sp->s_addr;
	switch(sp->s_type) {

	case S_HLIST:
	case S_OLIST:
		hlist = op;
		lmode = SLIST;
		break;

	case S_LIST:
		ldis = 0;
		break;

	case S_NLIST:
		ldis = 1;
		break;

	case S_BYTE:
	case S_WORD:
		do {
			expr(&a1);
			a = a1.a_addr;
			if (sp->s_type == S_BYTE) {
				bcheck(a, 't');
				outb(a);
			} else
				outw(a);
		} while ((c = getnb()) == ',');
		unget(c);
		break;

	case S_ASCII:
	case S_ASCIZ:
		if ((d = getnb()) == 0)
			err('q');
		while ((c = get()) != 0 && c != d)
			outb(c);
		if (c == 0)
			err('q');
		if (sp->s_type == S_ASCIZ)
			outb(0);
		break;

	case S_BLKB:
		expr(&a1);
		a = a1.a_addr;
		if ((c = getnb()) == ',') {
			expr(&a1);
			b = a1.a_addr;
		} else {
			unget(c);
			b = 0;
		}
		while (a--)
			outb(b);
		lmode = ALIST;
		break;

	case S_TITLE:
		p = tb;
		if (c = getnb()) {
			do {
				if (p < &tb[NTIT-1])
					*p++ = c;
			} while (c = get());
		}
		*p = 0;
		unget(c);

	case S_PAGE:
		lop = 60;
		lmode = NLIST;
		break;

	case S_START:
		expr(&a1);
		startadr = a1.a_addr;
		break;

	case S_OP0:
		outb(op);
		break;

	case S_OP2:
		addr(&a1);
		if(a1.a_type == S_LIT) {
			a = a1.a_addr;                  /* only banks 0 - 7 */
			if((0 <= a) && (a < 128)) {
				if(a % 16) aerr();
				else {
					outb(SRP); outb(a);
					break;
				}
			}
		}
		aerr();
		break;

	case S_OP3:
		addr(&a1);
		if(a1.a_type == S_REG) {
			outb(op);
			outb(a1.a_addr);
			break;
		}
		if(a1.a_type == S_WREG) {
			outb(op);
			outb(a1.a_addr | WRMASK);
			break;
		}
		if(a1.a_type == S_IR) {
			outb(op + 1);
			outb(a1.a_addr);
			break;
		}
		if(a1.a_type == S_WIR) {
			outb(op + 1);
			outb(a1.a_addr | WRMASK);
			break;
		}
		aerr();
		break;

	case S_OP4:
		addr(&a1);
		if(a1.a_type == S_ABS) {
			outb(CALL);
			outw(a1.a_addr);
			break;
		}
		if(a1.a_type == S_IR) {
			a = a1.a_addr;
			even(a);
			outb(CALLI);
			outb(a);
			break;
		}

		if(a1.a_type == S_WIR) {
			a = a1.a_addr;
			even(a);
			outb(CALLI);
			outb(a | WRMASK);
			break;
		}
		aerr();
		break;

	case S_OP5:
		addr(&a1);
		if(a1.a_type == S_ABS ) {
			outb(op);
			outw(a1.a_addr);
			break;
		}
		if(a1.a_type == S_IR) {
			if(op != JUMP) aerr();
			outb(JUMPI);
			a = a1.a_addr;
			even(a);
			outb(a);
			break;
		}
		if(a1.a_type == S_WIR) {
			if(op != JUMP) aerr();
			outb(JUMPI);
			a = (a1.a_addr | WRMASK);
			even(a);
			outb(a);
			break;
		}
		aerr();
		break;

	case S_OP7:
		addr(&a1);
		if(a1.a_type == S_WREG)
			op |=  (a1.a_addr << 4);
		else
			aerr();
		comma();

	case S_OP6:
		expr(&a1);
		a = a1.a_addr - dot->s_addr - 2;
		bcheck(a, 'a');
		outb(op);
		outb(a);
		break;

	case S_OP8:
		addr(&a1);
		comma();
		addr(&a2);
		if((a = check(a1.a_type, a2.a_type, c8nib)) == 0) {
			aerr();
			break;
		}
		op |= a;        /* mask in lower opcode nibble */
		if((a1.a_type == S_WREG) && (a2.a_type == S_WREG)) {
			outb(op);
			outb(a1.a_addr | ((a2.a_addr) << 4));
			break;
		}
		if((a1.a_type == S_WIR) && (a2.a_type == S_WREG)) {
			outb(op);
			outb(a1.a_addr | ((a2.a_addr) << 4));
			break;
		}
		if(a1.a_type == S_LIT) {
			outb(op);
			if((a2.a_type == S_WREG) || (a2.a_type == S_WIR))
				outb(a2.a_addr | WRMASK);
			else
				outb(a2.a_addr);
			outb(a1.a_addr);        /* immediate value */
			break;
		}
		outb(op);
		if((a1.a_type == S_WREG) || (a1.a_type == S_WIR))
			outb(a1.a_addr | WRMASK);
		else
			outb(a1.a_addr);
		if((a2.a_type == S_WREG) || (a2.a_type == S_WIR))
			outb(a2.a_addr | WRMASK);
		else
			outb(a2.a_addr);
		break;

	case S_OP9:
		addr(&a1);
		comma();
		addr(&a2);
		if((a = check(a1.a_type, a2.a_type, movtab)) > 0) {
			if(a < 20) {
				if(a2.a_type == S_WREG) {
					op = a | (a2.a_addr << 4);
					a = a1.a_addr;
					if(a1.a_type == S_WREG)
						a |= WRMASK;
				}
				else {
					op = a | (a1.a_addr << 4);
					a = a2.a_addr;
					if(a2.a_type == S_WREG)
						a |= WRMASK;
				}
				outb(op); outb(a); break;
			}
			switch(a & 0377) {

				case 0xc7:
					b = a1.a_addr | (a2.a_addr << 4);
					d = a1.a_reg;
					break;

				case 0xd7:
					b = a2.a_addr | (a1.a_addr << 4);
					d = a2.a_reg;
					break;

				case 0xe3:
				case 0xf3:
					b = a1.a_addr | (a2.a_addr << 4);
					d = 10000;
					break;

				case 0xe4:
				case 0xe5:
				case 0xf5:
					if((a1.a_type == S_WIR) || (a1.a_type == S_WREG))
						b = a1.a_addr | WRMASK;
					else
						b = a1.a_addr;
					if((a2.a_type == S_WIR) || (a2.a_type == S_WREG))
						d = a2.a_addr | WRMASK;
					else
						d = a2.a_addr;
					break;

				case 0xe6:
				case 0xe7:
					if((a2.a_type == S_WIR) || (a2.a_type == S_WREG))
						b = a2.a_addr | WRMASK;
					else
						b = a2.a_addr;
					d = a1.a_addr;
					break;
			}
			outb(a); outb(b);
			if(d != 10000) outb(d);
			break;
		}
		else
			aerr();
		break;

	case S_OP10:
		addr(&a1);
		if(a1.a_type == S_WREG) {
			even(a1.a_addr);
			outb(op);
			outb(a1.a_addr | WRMASK);
			break;
		}
		if(a1.a_type == S_REG) {
			even(a1.a_addr);
			outb(op); outb(a1.a_addr);
			break;
		}
		if(a1.a_type == S_WIR) {
			outb(op + 1);
			outb(a1.a_addr | WRMASK);
			break;
		}
		if(a1.a_type == S_IR) {
			outb(op + 1);
			outb(a1.a_addr);
			break;
		}
		aerr();
		break;

	case S_OP11:
	case S_OP12:
		addr(&a1);
		if(a1.a_type == S_WREG) {
			if(sp->s_type == S_OP11) {
				outb(INCW | (a1.a_addr << 4));
				break;
			}
			else {
				outb(op);
				outb(a1.a_addr | WRMASK);
				break;
			}
		}
		if(a1.a_type == S_REG) {
			outb(op);
			outb(a1.a_addr);
			break;
		}
		if(a1.a_type == S_WIR) {
			outb(op + 1);
			outb(a1.a_addr | WRMASK);
			break;
		}
		if(a1.a_type == S_IR) {
			outb(op + 1);
			outb(a1.a_addr);
			break;
		}
		aerr();
		break;

	case S_OP13:
		addr(&a1);
		comma();
		addr(&a2);
		if((a = check(a1.a_type, a2.a_type, c13tab)) > 0) {
			if(a == 1)
				outb(sp->s_flag);
			else
				outb(sp->s_addr);
			if (a1.a_type == S_WREG)
				outb(a2.a_addr | (a1.a_addr << 4));
			else
				outb(a1.a_addr | (a2.a_addr << 4));
			break;
		}
		else  {
			aerr();
			break;
		}


	case S_OP14:
	case S_OP15:
		addr(&a1);
		comma();
		addr(&a2);
		if((a1.a_type == S_WIR) && (a2.a_type == S_WIR)) {
			outb(op);
			outb(a1.a_addr | (a2.a_addr << 4));
			break;
		}
		aerr();
		break;


	default:
		err('o');
	}
	goto loop;
}

check(source, dest, table)
register char dest, source;
char *table;
{       register char *ptr;

	ptr = table;                   /* get address of legal var. table */
	while(*ptr != 0) {
		if(*ptr++ == dest ) {
			if(*ptr++ == source)
				return(*ptr);   /* return nibble */
			else
				ptr++;
		}
		else {
			ptr++; ptr++;
		}
	}
	return(0);                      /* not found */
}

/* check for even argument */
even(arg)
int arg;
{       if(arg & 1) err('e'); }

addr(ap)
register struct addr *ap;
{       register int c, n;
	register addr_t;
	register struct sym *sp;
	char id[NCPS];

	if((c = getnb()) == '$') {
		expr(ap);
		if ((ap->a_type != S_UND) && (ap->a_type != S_ABS))
			aerr();
		ap->a_type = S_LIT;
		return;
	}
	if(c == '(') {
		if(isidc(c = getnb())) {
			getid(c, id);
			if(getnb() != ')') err('q');
			if(chkreg(ap, id) > 0) {
				if(ap->a_type == S_REG) {
					ap->a_type = S_IR;
					return;
				}
				if(ap->a_type == S_WREG) {
					ap->a_type = S_WIR;
					return;
				}
				err('q');
			}
			else {
				sp = lookup(id, 0, 0);
				if(sp == NULL) {
					err('u');
					return;
				}
				else {
					if(sp->s_type == S_REG) {
						ap->a_type = S_IR;
						ap->a_addr = sp->s_addr;
						return;
					}
					if(sp->s_type == S_WREG) {
						ap->a_type = S_WIR;
						ap->a_addr = sp->s_addr;
						return;
					}
					err('q');
					return;
				}
			}
		}
		else
			err('q');
	}
	unget(c);
	expr(ap);
	if(ap->a_type != S_REG)
		return;
	if((c = getnb()) != '(') {
		unget(c);
		return;
	}
	/* for indexed addressing a_addr contains index part */
	/* a_reg contains offset part */
	if(isidc(c = getnb())) {
		getid(c, id);
		if(getnb() != ')') err('q');
		n = ap->a_addr;                 /* save offset */
		if(chkreg(ap, id) > 0) {
			if(ap->a_type == S_WREG) {
				ap->a_type = S_INDEX;
				ap->a_reg = n;
				return;
			}
			err('q');
		}
		else {
			sp = lookup(id, 0, 0);
			if(sp == NULL) {
				err('u');
				return;
			}
			else {
				if(sp->s_type == S_WREG) {
					ap->a_type = S_INDEX;
					ap->a_addr = sp->s_addr;
					ap->a_reg = n;
					return;
				}
				err('q');
			}
		}
	}
	else
		err('Q');
	return;
}

expr(ap)
register struct addr *ap;
{
	register int c;
	struct addr right;

	term(ap);
	if (ap->a_type != S_ABS)
		return;
	for (;;) {
		c = getnb();
		if ((c=='<' || c=='>') && c!=getnb())
			err('q');
		if (!any(c, "+-*%&|><^")) {
			unget(c);
			break;
		}
		term(&right);
		if (right.a_type != S_ABS)
			err('r');
		switch(c) {

		case '+':
			ap->a_addr += right.a_addr;
			break;

		case '-':
			ap->a_addr -= right.a_addr;
			break;

		case '*':
			ap->a_addr *= right.a_addr;
			break;

		case '%':
			ap->a_addr /= right.a_addr;
			break;

		case '&':
			ap->a_addr &= right.a_addr;
			break;

		case '|':
			ap->a_addr |= right.a_addr;
			break;

		case '^':
			ap->a_addr ^= right.a_addr;     /* xor */
			break;

		case '>':
			ap->a_addr >>= right.a_addr;
			break;

		case '<':
			ap->a_addr <<= right.a_addr;
		}
	}
}

term(ap)
register struct addr *ap;
{
	register int c;
	register int n, r, v;
	char id[NCPS];
	register struct sym *sp;
	register struct tsym *tp;

	c = getnb();
	if (c == '[') {
		expr(ap);
		if (getnb() != ']')
			err('q');
		return;
	}
/*	added literal	hgw	*/
	if (c == '\'') {
		c = get();
		c = c&0177;
		ap->a_type = S_ABS;
		ap->a_addr = c;
		return;
	}
/*      operator-precedence of unary operators changed [hgw]    */
	if (c == '-' || c == '!') {
		term(ap);
		if (ap->a_type != S_ABS) {
			err('r');
			ap->a_type = S_ABS;
		}
		if (c == '-')
			ap->a_addr = -ap->a_addr;
		else
			ap->a_addr = ~ap->a_addr;
		return;
	}
	if (isdigit(c)) {
		ap->a_type = S_ABS;
		r = 10;
		if (c == '0') {
			r = 8;
			if ((c = get()) == 'x') {
				r = 16;
				c = get();
			}
			else
				if (c  == 'B') {
					r = 2;
					c = get();
				}
		}
		n = 0;
		while ((v = digit(c, r)) >= 0) {
			n = r*n + v;
			c = get();
		}
		if (c=='f' || c=='b') {
			if ((n < 10) && (n >= 0)) {
				if (c == 'f')
					tp = tsymp[n].t_fp; else
					tp = tsymp[n].t_bp;
				if (tp != NULL) {
					ap->a_addr = tp->t_addr;
					return;
				}
			}
			err('u');
			ap->a_addr = 0;
			return;
		}
		unget(c);
		ap->a_addr = n;
		return;
	}
	if (isidc(c)) {
		getid(c, id);
		if(chkreg(ap, id) > 0 ) return;
		if ((sp = lookup(id, 0, 0)) != NULL) {
			ap->a_type = sp->s_type;
			ap->a_addr = sp->s_addr;
			return;
		}
		err('u');
		ap->a_type = S_ABS;
		ap->a_addr = 0;
		return;
	}
	err('q');
}

/* check for register usage */
/* w must be in the range 0 - 15 */
/* r must be in the range 0 - 127, or 240 - 255 */
/* if a valid register, set type in the supplied structure */
/* and return +1 */
/* otherwise return 0 */

chkreg(ap, id)
register struct addr *ap;
char *id;
{
	register char *ptr;
	register int n;

	ptr = id;               /* get address of token */
	if(*ptr == 'w') {
		n = getnum(ptr, 2);      /* 2 digits max */
		if((0 <= n) && (n < 16)) {
			ap->a_type = S_WREG;     /* short */
			ap->a_addr = n;
			return(1);
		}
	}
	if(*ptr == 'r') {
		n = getnum(ptr, 3);     /* 3 digits max */
		if(((0 <= n) && (n < 128)) || ((240 <= n) && (n < 256))) {
			ap->a_type = S_REG;      /* long */
			ap->a_addr = n;
			return(1);
		}
	}
	return(0);
}

/* get number of working register */
getnum(str, nmax)
char *str;
int nmax;
{
	register int n, c, i;
	register int imax;
	register char *ptr;

	ptr = str;
	imax = nmax;
	ptr++;                  /* skip leading 'w', 'r' or 'R' */
	n = i = 0;
	while((i < imax) && ((c = *ptr++) != '\0')) {
		n = 10 * n + c - '0';
		i++;
	}
	if((i == imax) && (*ptr != '\0')) return(-1);  /* since no '\0' reached */
	else return(n);
}

digit(c, r)
register c, r;
{
	if (r == 16) {
		if (c >= 'A' && c <= 'F')
			return (c - 'A' + 10);
		if (c >= 'a' && c <= 'f')
			return (c - 'a' + 10);
	}
	if (r == 8) {
		if (c >= '0' && c <= '7')
			return (c - '0');
		else
			return (-1);
	}
	if (r == 2) {
		if (c >= '0' && c <= '1')
			return (c - '0');
		else
			return (-1);
	}
	if (c >= '0' && c <= '9')
		return (c - '0');
	return (-1);
}

any(c, s)
register c;
register char *s;
{
	register char b;

	while (b = *s++)
		if (b == c)
			return (1);
	return (0);
}

list()
{
	register char *wp;
	register nb;

	if (lfp==NULL || lmode==NLIST || ldis==1)
		return;
	slew();
	while (ep < &eb[NERR])
		*ep++ = ' ';
	fprintf(lfp, "%.10s", eb);
	if (lmode == SLIST) {
		fprintf(lfp, "%24s%5d %s\n", "", line, ib);
		return;
	}
	fprintf(lfp, hlist?"   %04x":" %06o", laddr);
	if (lmode == ALIST) {
		fprintf(lfp, "%17s%5d %s\n", "", line, ib);
		return;
	}
	wp = cb;
	nb = cp - cb;
	list1(wp, nb, 1);
	fprintf(lfp, " %5d %s\n", line, ib);
	while ((nb -= 4) > 0) {
		wp += 4;
		slew();
		fprintf(lfp, "%17s", "");
		list1(wp, nb, 0);
		putc('\n', lfp);
	}
}

list1(wp, nb, f)
register char *wp;
register nb;
{
	register i;

	if (nb > 4)
		nb = 4;
	for (i=0; i<nb; ++i)
		fprintf(lfp, hlist?"  %02x":" %03o", (*wp++)&0377);
	if (f)
		while (i < 4) {
			fprintf(lfp, "    ");
			++i;
		}
}

slew()
{
	if(lfp == NULL)
		return;
	if (lop++ >= 60) {
		daytime = time(NULL);
		if (page != 1 )       /* suppress form-feed on 1st page */
			fprintf(lfp, "\f");
		fprintf(lfp, "Z8 Assembler: h.-g. willers\tpage %d\t\t%s", page++, ctime(&daytime));
		fprintf(lfp, "%s\n\n", tb);
		lop = 4;
	}
}


/*	print symbol table	*/
prstb()
{
	register struct sym *sp;
	register char *p1;
	register char ochar;
	register int i, j;
	int cntr;

	if(!lflag) return;
	if(lflag) {
		lop = 60;		/* force new page */
		slew();
		fprintf(lfp, "Symboltable dump\n\n");
	}
	sp = &sym[0];		/* get pointer to begin of symbol table */
	/* remove any item not assigned or label */
	for(i= 0; i < HASHSIZ; i++) {
		p1 = sp->s_name;
		if(p1 != 0) {
			if(sp->s_type != S_ABS)
				if(sp->s_flag != S_ASG)
					sp->s_name = 0;
		}
		sp++;
	}
	dot->s_name = 0;                /* remove '.' */
	cntr = 0;
	sp = &sym[0];
	for(i=0; i < HASHSIZ; i++) {
		p1 = sp->s_name;
		if(p1 == 0) sp++;
		else {
			if(cntr != 0)
				fprintf(lfp, "    ");
			j = 0;
			while(j < (NCPS - 1)) {
				j++;
				if((ochar = *p1++) != '\0')
					fprintf(lfp,"%c", ochar);
				else {
					fprintf(lfp,"%c", ' ');
					break;
				}
			}
			while(j < NCPLE) {
				j++;
				fprintf(lfp, "%c", ' ');
			}
			j = sp->s_addr;
			fprintf(lfp, hlist?"  %04x":"%06o", j);
			cntr++;
			if(cntr == NLEPL) {
				cntr = 0;
				fprintf(lfp,"\n");
				slew();
			}
			sp++;
		}
	}
	fprintf(lfp, "\n");
	slew();
}

/*	print assembler statistics	*/
prstat()
{
	register int i;

	if(!lflag)
		return;
	if(lop > 50)            /* force new page if neccesary */
		lop = 60;

	slew();
	i = HASHSIZ;
	fprintf(lfp, "\n\n symboltable-size is %d ; %d entries used\n", i, ninst);
}

bcheck(n, f)
register n;
{
	if((n < -128 || n > 127) && ((n & ~(0xff)) != 0))
		err(f);
}

comma()
{
	if (getnb() != ',')
		err('q');
}

getid(c, id)
register c;
char *id;
{
	register char *p;

	p = id;
	do {
		if (p < ((&id[NCPS]) - 1))
			*p++ = c;
	} while (isidc(c=get()) || c=='\'' || (c>='0' && c<='9'));
	unget(c);
	while (p < &id[NCPS])
		*p++ = 0;
}

getnb()
{
	register c;

	while ((c=get())==' ' || c=='\t')
		;
	return (c);
}

get()
{
	register c;

	if ((c = *ip) != '\0')
		++ip;
	return (c);
}

unget(c)
{
	if (c != '\0')
		--ip;
}

isdigit(c)
register c;
{
	return (c>='0' && c<='9');
}

isidc(c)
register c;
{
	if (c == '_')
		return (1);
	if (c == '.')
		return (1);
	if (c>='a' && c<='z')
		return (1);
	if (c>='A' && c<='Z')
		return (1);
	return (0);
}

struct sym *
lookup(id, f, x)
char *id;
{
	register struct sym *sp;
	register char *p1, *p2;
	register int fihash, lasthash, i;

	lasthash = fihash = hash(id);
	for(i = 0; i < HASHSIZ; i++) {
		sp = &sym[lasthash];
		p1 = id;
		p2 = sp->s_name;
		if(p2 == 0) break;     /* found free entry */
		else {
			if(strncmp(p1,p2,NCPS) != 0) {
				lasthash += fihash;
				lasthash %= HASHSIZ;
			}
			else return(sp);
		}
	}
	if (f == 0)
		return (NULL);
	if(i == HASHSIZ)
		error("Too many symbols!\n");
	ninst++;		/* increment # of install in table */
	if(x == 1)
		sp->s_name = id;
	else {
		p2 = malloc((strlen(id) + 1));
		sp->s_name = p2;
		p1 = id;
		strcpy(p2, p1);
	}
	sp->s_type = S_UND;
	sp->s_flag = 0;
	sp->s_addr = 0;
	return (sp);
}

/* compute hashindex of string	*/

hash(str)
char *str;
{
	register unsigned int i;
	register char c;

	i = 0;
	while((c = *str++) != 0)
		i += ( (int) c) & 0377;
	i %= HASHSIZ;
	if(i == 0 )
		i++;		/* since we use 'add the hash rehash' */
	return(i);
}

aerr()
{
	err('a');
}

err(c)
register c;
{
	register char *p;

	p = eb;
	while (p < ep)
		if (c == *p++)
			return;
	if (p < &eb[NERR]) {
		*p++ = c;
		ep = p;
	}
	if (c == 'q') {
		++eflag;
		longjmp(env, 0);
	}
}

error(str)
char *str;
{	fprintf(stderr, str);}

diag()
{
	register char *p;

	p = eb;
	while (p < ep)
		printf("%c %04d %s\n", *p++, line, ib);
}

outw(w)
register w;
{
	outb(w>>8);
	outb(w);
}

outb(b)
register b;
{
	if (pass != 0) {
		if (cp >= &cb[NCODE])
			err('z');
		else
			*cp++ = b;
		if (!nflag) {
			if (bo>=NBIN || ba+bo!=dot->s_addr) {
				bflush();
				ba = dot->s_addr;
				bo = 0;
			}
			bb[bo++] = b;
		}
	}
	++dot->s_addr;
}

bflush()
{
	register b, c, i;

	if (bo == 0)
		return;
	fprintf(ofp, ":%02x%04x00", bo, ba);
	c = bo + ba + (ba>>8);
	for (i=0; i<bo; ++i) {
		b = bb[i]&0377;
		fprintf(ofp, "%02x", b);
		c += b;
	}
	fprintf(ofp, "%02x\n", (-c)&0377);
}

beof()
{
	register int dummy;

	dummy = ((startadr & 0377) + (startadr >> 8) + 1) & 0377;
	dummy = -dummy & 0377;

	if (startadr == 0xffff)         /* not given */
		fprintf(ofp, ":0000000000\n");
	else
		fprintf(ofp, ":00%04x01%02x\n", startadr, dummy);
}

/*
 * install z8 opcodes
 */
insopc()
{
	register struct sym *sp;
	register struct sym *pp;

	for (sp = optab;;) {
		if (*(sp->s_name) == 0)
			return;
		pp = lookup(sp->s_name, 1, 1);
		if(pp == NULL) {
			fprintf(stderr,"hashtable-error with symbol: %s\n", sp->s_name);
			exit(1);
		}
		pp->s_type = sp->s_type;
		pp->s_flag = sp->s_flag;
		pp->s_addr = sp->s_addr;
		sp++;
	}
}

struct sym optab[] = {
	".",            S_ABS,          S_ASG,  0,
	".byte",        S_BYTE,         0,      0,
	".word",        S_WORD,         0,      0,
	".ascii",       S_ASCII,        0,      0,
	".asciz",       S_ASCIZ,        0,      0,
	".blkb",        S_BLKB,         0,      0,
	".page",        S_PAGE,         0,      0,
	".title",       S_TITLE,        0,      0,
	".hlist",       S_HLIST,        0,      1,
	".olist",       S_OLIST,        0,      0,
	".list",        S_LIST,         0,      0,
	".nlist",       S_NLIST,        0,      0,
	".end",         S_START,        0,      0,

	"ccf",          S_OP0,          0,      0xef,
	"di",           S_OP0,          0,      0x8f,
	"ei",           S_OP0,          0,      0x9f,
	"reti",         S_OP0,          0,      0xbf,
	"nop",          S_OP0,          0,      0xff,
	"rcf",          S_OP0,          0,      0xcf,
	"ret",          S_OP0,          0,      0xaf,
	"scf",          S_OP0,          0,      0xdf,

	"srp",          S_OP2,          0,      0x31,

	"clr",          S_OP3,          0,      0xb0,
	"com",          S_OP3,          0,      0x60,
	"da",           S_OP3,          0,      0x40,
	"dec",          S_OP3,          0,      0,
	"pop",          S_OP3,          0,      0x50,
	"push",         S_OP3,          0,      0x70,
	"rl",           S_OP3,          0,      0x90,
	"rlc",          S_OP3,          0,      0x10,
	"rr",           S_OP3,          0,      0xe0,
	"rrc",          S_OP3,          0,      0xc0,
	"sra",          S_OP3,          0,      0xd0,
	"swap",         S_OP3,          0,      0xf0,

	"call",         S_OP4,          0,      0,

	"jmp",          S_OP5,          0,      0x8d,
	"jcs",          S_OP5,          0,      0x7d,
	"jcc",          S_OP5,          0,      0xfd,
	"jeq",          S_OP5,          0,      0x6d,
	"jne",          S_OP5,          0,      0xed,
	"jpl",          S_OP5,          0,      0xdd,
	"jmi",          S_OP5,          0,      0x5d,
	"jov",          S_OP5,          0,      0x4d,
	"jnov",         S_OP5,          0,      0xcd,
	"jge",          S_OP5,          0,      0x9d,
	"jlt",          S_OP5,          0,      0x1d,
	"jgt",          S_OP5,          0,      0xad,
	"jle",          S_OP5,          0,      0x2d,
	"juge",         S_OP5,          0,      0xfd,
	"jult",         S_OP5,          0,      0x7d,
	"jugt",         S_OP5,          0,      0xbd,
	"jule",         S_OP5,          0,      0x3d,

	"br",           S_OP6,          0,      0x8b,
	"bcs",          S_OP6,          0,      0x7b,
	"bcc",          S_OP6,          0,      0xfb,
	"beq",          S_OP6,          0,      0x6b,
	"bne",          S_OP6,          0,      0xeb,
	"bpl",          S_OP6,          0,      0xdb,
	"bmi",          S_OP6,          0,      0x5b,
	"bov",          S_OP6,          0,      0x4b,
	"bnov",         S_OP6,          0,      0xcb,
	"bge",          S_OP6,          0,      0x9b,
	"blt",          S_OP6,          0,      0x1b,
	"bgt",          S_OP6,          0,      0xab,
	"ble",          S_OP6,          0,      0x2b,
	"buge",         S_OP6,          0,      0xfb,
	"bult",         S_OP6,          0,      0x7b,
	"bugt",         S_OP6,          0,      0xbb,
	"bule",         S_OP6,          0,      0x3b,

	"sob",          S_OP7,          0,      0x0a,

	"add",          S_OP8,          0,      0x00,
	"adc",          S_OP8,          0,      0x10,
	"sub",          S_OP8,          0,      0x20,
	"sbc",          S_OP8,          0,      0x30,
	"or",           S_OP8,          0,      0x40,
	"and",          S_OP8,          0,      0x50,
	"tcm",          S_OP8,          0,      0x60,
	"tm",           S_OP8,          0,      0x70,
	"cmp",          S_OP8,          0,      0xa0,
	"xor",          S_OP8,          0,      0xb0,

	"mov",          S_OP9,          0,      0,

	"incw",         S_OP10,         0,      0xa0,
	"decw",         S_OP10,         0,      0x80,

	"inc",          S_OP11,         0,      0x20,
	"dec",          S_OP12,         0,      0,

/*      movp    is      ldc     'move from/to program memory' */
/*      movd    is      lde     'move from/to data memory' */
/*      movpi   is      ldci    'move from/to prog. mem autoincr.' */
/*      movdi   is      ldei    'move from/to data mem autoincr.' */
/* s_flag contains first line of inst. summary */
/* s_addr contains second line */

	"movp",         S_OP13,         0xc2,   0xd2,
	"movd",         S_OP13,         0x82,   0x92,
	"movfpi",       S_OP14,         0,      0xc3,
	"movfdi",       S_OP14,         0,      0x83,
	"movtpi",       S_OP15,         0,      0xd3,
	"movtdi",       S_OP15,         0,      0x93,
	"",             0,              0,      0
};
