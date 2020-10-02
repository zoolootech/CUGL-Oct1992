/*	A SMALL C COMPILER	*/
/*	VERSION 1.1		*/
/* By	Ron Cain		*/
/* Modified:			*/
/*	12/16/80	by 	A.Griggs	*/
/*							*/
/* 	Define system dependent parameters		*/
#INCLUDE BDSCIO.H
/* Stand alone definitions				*/
#DEFINE	eol	13
#DEFINE lf	10
/*	Unix definitions (if not stand alone)		*/
/*#INCLUDE	<stdio.h>				*/
/*#DEFINE	eol	10				*/
/*	Define the syjbol table parameters		*/
#DEFINE	symsiz	14
#DEFINE	symtbsz	5040
#DEFINE	numglbs	300
#DEFINE	startglb	symtab
#DEFINE	endglb	startglb+numglbs*symsiz
#DEFINE startloc endglb+symsiz
#DEFINE endloc symtab+symtbsz-symsiz
/*	define symbol table entry points		*/
#DEFINE	name	0
#DEFINE	type	10
#DEFINE	ident	9
#DEFINE	storage	11
#DEFINE	offset	12
/*	System wide name size (for symbols)		*/
#DEFINE	namesize	9
#DEFINE	namemax	8
/*	define possible entries for ident		*/
#DEFINE	variable	1
#DEFINE	array	2
#DEFINE	pointer	3
#DEFINE	function	4
/*	define possible entries for type		*/
#DEFINE	cchar	1
#DEFINE	cint	2
/*	define possible entries for "storage"		*/
#DEFINE	statik	1
#DEFINE	stkloc	2
/*	define the "while" statment queue		*/
#DEFINE	wqtabsz	100
#DEFINE	wqsiz	4
#DEFINE	wqmax	wq+wqtabsz-wqsiz
/*	define entry offset in while que		*/
#DEFINE	wqsym	0
#DEFINE	wqsp	1
#DEFINE	wqloop	2
#DEFINE	wqlab	3
/*	define the literal pool				*/
#DEFINE	litabsz	2000
#DEFINE	litmax	litabsz-1
/*	define the input line				*/
#DEFINE linesize MAXLINE
#DEFINE	linemax	linesize-1
#DEFINE	mpmax	linemax
/*	define the macro (define) pool			*/
#DEFINE	macqsize	1000
#DEFINE	macmax	macqsize-1
/*	define statement types (tokens)			*/
#DEFINE	stif	1
#DEFINE	stwhile	2
#DEFINE	streturn	3
#DEFINE	stbreak	4
#DEFINE	stcont	5
#DEFINE	stasm	6
#DEFINE	stexp	7
/*	Reserve some storage words			*/
char iobuf0[BUFSIZ];		/* write buffer		*/
char iobuf1[BUFSIZ];		/* Read buffer		*/
char iobuf2[BUFSIZ];		/* Include read buffer	*/
char	symtab[symtbsz];	/* symbol table		*/
char	*glbptr,*locptr;	/* ptrs to next entries	*/
int	wq[wqtabsz];		/* while queue		*/
int	*wqptr;			/* ptr to next entry	*/
char	litq[litabsz];		/* literal pool		*/
int	litptr;			/* ptr to next entry	*/
char	macq[macqsize];		/* macro string buffer	*/
int	macptr;			/*   and its index	*/
char	line[linesize];		/* parsing buffer	*/
char	mline[linesize];	/* temp macro buffer	*/
int	lptr,mptr;		/* ptrs into each	*/
/*	Miscellaneous storage				*/
int	nxtlab,	/* next available label#*/
	litlab,	/* label # assigned to literal pool*/
	sp,	/* compiler relative stack pointer	*/
	argstk,	/* function argument stack pointer	*/
	ncmp,	/* # of open compound statments		*/
	errcnt,	/* # of errors in compilation		*/
	eof,	/* set non-zero on final input eof	*/
	input,	/* iob # for input file			*/
	output,	/* iob # for output file (if any)	*/
	input2,	/* iob # for "include" file		*/
	glbflag,/* non-zero if internal globals		*/
	ctext,	/* non-zero to intermix c-source	*/
	cmode,	/* non-zero while parsing c-code	*/
		/* zero when parsing assembly code	*/
	lastst;	/* last executed statement type		*/
char	quote[2];	/* literal string for '"'	*/
char	*cptr;		/* work ptr to any char buffer	*/
int	*iptr;		/* work ptr to any int buffer	*/
øÑáÝ6É#¾ ï#Fñáþ.(èEQGTLTGELE	NE åÅÍÏ!ÁëãËP /ËH ;å·íRá(8Ë` >ËP :Ëh 6=Ë@ 07ËX *ËH &Ëh ")Ë|(