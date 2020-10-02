/*
 *	Word Processor
 *	similar to Unix NROFF or RSX-11M RNO -
 *	adaptation of text processor given in
 *	"Software Tools", Kernighan and Plauger.
 *
 *	Stephen L. Browning
 *	5723 North Parker Avenue
 *	Indianapolis, Indiana 46220
 */

#include "a:bdscio.h"
#include "nro.h"
#include "nrocom.c"

main(argc,argv)
int argc;
char *argv[];
{
	int ifp, ofp;
	int i;
	int swflg;

	swflg = FALSE;
	pout = stdout;
	ifp = ofp = 0;
	init();
	for (i=1; i<argc; ++i) {
		if (*argv[i] == '-' || *argv[i] == '+') {
			if (pswitch(argv[i],&swflg) == ERR) exit(-1);
		}
		else if (*argv[i] == '>') {
			if (ofp == 0) {
				if (!strcmp(argv[i]+1,"$P")) {
					ofp = 1;
					co.lpr = TRUE;
				}
				else if ((ofp = fcreat(argv[i]+1,&oub)) == ERR) {
					printf("nro: cannot create %s\n",argv[i]+1);
					exit(-1);
				}
				else {
					pout = &oub;
				}
			}
			else {
				puts("nro: too many output files\n");
				exit(-1);
			}
		}
	}
	for (i=1; i<argc; ++i) {
		if (*argv[i] != '-' && *argv[i] != '+' && *argv[i] != '>') {
			if ((ifp = fopen(argv[i],&sofile[0])) == ERR) {
				printf("nro: unable to open file %s\n",argv[i]);
				exit(-1);
			}
			else {
				profile();
				fclose(&sofile[0]);
			}
		}
	}
	if ((ifp == 0 && swflg == FALSE) || argc <= 1) {
		puts("Usage: nro [-n] [+n] [-pxx] [-v] [-b] [-mmacfile] infile ... [>outfile]\n");
		exit(-1);
	}
	if (pout != stdout) {
		putc(CPMEOF,pout);
		fflush(pout);
		fclose(pout);
	}
}



/*
 *	retrieve one line of input text
 */

getlin(p,in_buf)
char *p;
struct _buf *in_buf;
{
	int i;
	int c;
	char *q;

	q = p;
	for (i=0; i<MAXLINE-1; ++i) {
		c = ngetc(in_buf);
		if (c == CPMEOF || c == EOF) {
			*q = EOS;
			c = strlen(p);
			return(c == 0 ? EOF : c);
		}
		*q++ = c;
		if (c == '\n') break;
	}
	*q = EOS;
	return(strlen(p));
}



/*
 *	initialize parameters for nro word processor
 */

init()
{
	int i;

	dc.fill = YES;
	dc.lsval = 1;
	dc.inval = 0;
	dc.rmval = PAGEWIDTH - 1;
	dc.tival = 0;
	dc.ceval = 0;
	dc.ulval = 0;
	dc.cuval = 0;
	dc.juval = YES;
	dc.boval = 0;
	dc.bsflg = FALSE;
	dc.pgchr = '#';
	dc.cmdchr = '.';
	dc.prflg = TRUE;
	dc.sprdir = 0;
	for (i=0; i<26; ++i) dc.nr[i] = 0;
	pg.curpag = 0;
	pg.newpag = 1;
	pg.lineno = 0;
	pg.plval = PAGELEN;
	pg.m1val = 2;
	pg.m2val = 2;
	pg.m3val = 2;
	pg.m4val = 2;
	pg.bottom = pg.plval - pg.m4val - pg.m3val;
	pg.offset = 0;
	pg.frstpg = 0;
	pg.lastpg = 30000;
	pg.ehead[0] = pg.ohead[0] = '\n';
	pg.efoot[0] = pg.ofoot[0] = '\n';
	for (i=1; i<MAXLINE; ++i) {
		pg.ehead[i] = pg.ohead[i] = EOS;
		pg.efoot[i] = pg.ofoot[i] = EOS;
	}
	pg.ehlim[LEFT] = pg.ohlim[LEFT] = dc.inval;
	pg.eflim[LEFT] = pg.oflim[LEFT] = dc.inval;
	pg.ehlim[RIGHT] = pg.ohlim[RIGHT] = dc.rmval;
	pg.eflim[RIGHT] = pg.oflim[RIGHT] = dc.rmval;
	co.outp = 0;
	co.outw = 0;
	co.outwds = 0;
	co.lpr = FALSE;
	for (i=0; i<MAXLINE; ++i) co.outbuf[i] = EOS;
	for (i=0; i<MXMDEF; ++i) mac.mnames[i] = NULL;
	mac.lastp = 0;
	mac.emb = &mac.mb[0];
	mac.ppb = NULL;
}


/*
 *	get character from input file or push back buffer
 */

ngetc(infp)
struct _buf *infp;
{
	int c;

	if (mac.ppb >= &mac.pbb[0]) {
		c = *mac.ppb--;
	}
	else {
		c = getc(infp);
	}
	return(c);
}



/*
 *	process input files from command line
 */

profile()
{
	char ibuf[MAXLINE];

	for (dc.flevel=0; dc.flevel>=0; --dc.flevel) {
		while (getlin(ibuf,&sofile[dc.flevel]) != EOF) {
			if (ibuf[0] == dc.cmdchr) comand(ibuf);
			else text(ibuf);
		}
		if (dc.flevel > 0) fclose(&sofile[dc.flevel]);
	}
	if (pg.lineno > 0) space(HUGE);
}



/*
 *	process switch values from command line
 */

pswitch(p,q)
char *p;
int *q;
{
	int swgood;

	swgood = TRUE;
	if (*p == '-') {
		switch (tolower(*++p)) {
		case 'b':
			dc.bsflg = TRUE;
			break;
		case 'm':
			if (fopen(++p,&sofile[0]) == ERR) {
				printf("***nro: unable to open file %s\n",p);
				exit(-1);
			}
			profile();
			fclose(&sofile[0]);
			break;
		case 'p':
			set(&pg.offset,ctod(++p),'1',0,0,HUGE);
			break;
		case 'v':
			printf("NRO version 1.0\n");
			*q = TRUE;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			pg.lastpg = ctod(p);
			break;
		default:
			swgood = FALSE;
			break;
		}
	}
	else if (*p == '+') {
		pg.frstpg = ctod(++p);
	}
	else {
		swgood = FALSE;
	}
	if (swgood == FALSE) {
		printf("nro: illegal switch %s\n",p);
		return(ERR);
	}
	return(OK);
}
