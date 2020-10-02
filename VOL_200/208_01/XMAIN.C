/* xmain.c
 *	called by the C program starter s.s
 *	eventually calls main()
 *	handles argv,argc,quotes,upper/lowercase,redirections
 */
#include <stdio.h>
#include <ctype.h>
#include <osif.h>


#define EOLINE 0
#define T_LT 1
#define INDQUOTE 2
#define INWORD 3
#define T_GTGT 4
#define T_GT 5
#define T_WORD 6
#define NEUTRAL 7
#define INSQUOTE 8
#define T_SQWORD 9

#define GETC() *cptr++
#define UNGETC(c) cptr--

static char *cptr;

static int argc=0;
static char **argv;
static char *argl;
extern char __tname[];
extern char __pname[];
extern char *index();
extern char *malloc();

_main(com,len)
char *com;
{
register char c;
register int token;
int inputp,outputp;
char word[130];

	_chinit(); /*initialise channels*/
	open(__tname,0);	/* CON: */
	open(__tname,1);
	open(__tname,2);/*want to read stderr*/

	com[len]=0;
	cptr=com; argl=0; inputp=outputp=0;
	strcpy(word,__pname);token=T_WORD; /*argv[0]*/
	do {
		switch(token){
		case T_WORD:
			addargv(word);
			break;
		case T_SQWORD:
			ADDarg(word);
			break;
		case T_LT:
			if(inputp){
				fprintf(stderr,"extra <\n");
				exit(1);
			}
			inputp++;
			if(gettoken(word) == T_WORD) {
				close(0);
				if(open(word,0)==0)
					break;
			}
			fprintf(stderr,"cannot redirect input\n");
			exit(1);
		case T_GT:
		case T_GTGT:
			if(outputp){
				fprintf(stderr,"extra > or >>\n");
				exit(1);
			}
			outputp++;
			if(gettoken(word)== T_WORD ) {
				close(1);
				if(open(word,1) == 1 || creat(word,0666) == 1){
					if(token==T_GTGT)lseek(1,0L,2);
					break;
				}
			}
			fprintf(stderr,"output redirection error\n");
			exit(1);
		}
		token=gettoken(word);
	} while(token!= EOLINE);

	makeargv();

	exit(main(argc,argv,(char *)0)); /* environment ptr nil */
}

static gettoken(word)
char *word;
{
register int state=NEUTRAL;
register int c;
register char *w;
int slash=0;
int qflag=0;
	w= word;
	for(;;) {
		c= GETC();
		switch(state) {
		case NEUTRAL:
			switch(c){
			case ' ': case '\t':
				continue;
			case '<':
				return(T_LT);
			case '>':
				if((c= GETC())== '>')
					return(T_GTGT);
				UNGETC(c);
				return(T_GT);
			case '\'':
				state=INSQUOTE; continue;
			case '"':
				state=INDQUOTE; continue;
			case EOLINE:
				UNGETC(c);
				return(EOLINE);
			default:
				state=INWORD;
				if(c=='\\')
					slash++;
				else
					*w++ = tolower(c);
				continue;
			}/* endsw inside case NEUTRAL*/
		case INDQUOTE:
		case INSQUOTE:
			if(c==EOLINE){ *w=0;
				fprintf(stderr,"unmatched quote: %s\n",word);
				exit(1);
			}
			if(slash) {
				slash=0;
				*w++ = c;
				continue;
			}
			if(c== '\\'){
				slash++;
				continue;
			}
			if(c=='\'')
				if (state==INDQUOTE) {
					if(qflag==0)
						qflag++;
					else
						qflag=0;
					continue;
					}
				else {
					*w = '\0';return(T_SQWORD);
				}

			if(c== '"' && state==INDQUOTE && qflag==0) {
				*w = '\0';
				return(T_WORD);
			}
			*w++ = tolower(c);
			continue;
		case INWORD:
			if(c==EOLINE){
				UNGETC(c);
				*w= '\0';
				return(T_WORD);
			}
			if(slash){
				slash=0;
				*w++ = c; continue;
			}
			if(c== '\\') {
				slash++; continue;
			}
			if(index("&();<> \t|",c)) {
				UNGETC(c);
				*w = '\0';
				return(T_WORD);
			}
			*w++ = tolower(c);
			continue;
		}/*endsw of state*/
	}/*end of FOR*/
}

/* we are going to use the file control block area allocated for
 * the file descriptor 3. If you do not like this, you must set
 * char dma[128]; char fcb[36]; to call search for first etc.
 * NB you cannot call malloc() while you are arbitrarily breaking
 * _break.
 */
static
addargv(ptr)
register char *ptr;
{
register char c;
struct ccb *pfd;
char tmpbuf[30];
	if(index(ptr,'*')||index(ptr,'?')) {
		/*NOSTRICT*/
		pfd= _getccb(3); /* use the buffer of FD 3 */
		__BDOS(26, pfd->buffer);
		c= __open(3,ptr,17); /* search for first */
		if(c != '\377')
			for(;;){
				_toasc(pfd,c,tmpbuf);
				ADDarg(tmpbuf);
				c= __open(3,ptr,18); /* search for nxt */
				if(c == '\377')return;
			}
	}
	ADDarg(ptr);
}
static
memfault()
{
fprintf(stderr,"out of memory\n");
exit(1);
}

static
ADDarg(ptr)
register char *ptr;
{
register char **p;
	/* make a list of ptr->back + strings */
	if((p= (char **)malloc(sizeof(char *)+strlen(ptr)+1))==0)
		memfault();
	*p = argl;
	argl=p;
	strcpy((char *)&p[1],ptr);
	argc++;
}
static
makeargv()
{
register char **p;
register int i;
	if((argv=(char **)malloc((1+argc)* sizeof(char *)))==0)
		memfault();
	p= argl; i=argc;argv[i--]=0;
	do {
		argv[i]= p+1;
		p = *p;
	} while(i--);
}

static
_toasc(p,c,buf)
register struct ccb *p;
register char c;	/* c*32 gives position in the buffer */
register char *buf;
{
	register char *f;
	int i;
	*buf=0;
	i=0;
	f= p->buffer;
	f += c*32;
	if(p->user) {
		i=p->user-1;
		if(i>=10)
			*buf++ = '1';
		*buf++ = i%10+'0';
		*buf++ = ':';
		i=1;
	}
	if(p->fcb.drive) {
		if(i)
			buf--;
		*buf++ = p->fcb.drive +'a'-1;
		*buf++ = ':';
	}
	for(i=1;i<9;i++)
		if(f[i] !=' ')
			*buf++ = tolower(toascii(f[i]));
		else break;
	*buf++ = '.';
	for(i=9;i<12;i++)
		if(f[i] !=' ')
			*buf++ = tolower(toascii(f[i]));
		else break;
	if(buf[-1]== '.')buf--;
	*buf= 0;
}

