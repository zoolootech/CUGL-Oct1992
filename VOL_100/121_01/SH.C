/*
HEADER: CUG 121.??;

    TITLE:	sh - a 'little shell' command interpreter;
    VERSION:	2.2;
    DATE:	01/06/86;
    DESCRIPTION: "The little shell is designed to provide a `Unix-like'
		shell for CP/M, replacing the CP/M CCP as the primary user
		interface.  It provides two convenient mechanisms for
		chaining commands together: they may be typed on a single
		line, separated by commas; or files of commands called Shell
		scripts may be executed.
		The CP/M operating environment does not lend itself to the
		use of frequently invoked commands in the form of executable
		files.  Consequently, the shell has an extensive array of
		built-in commands, including: cat, ccp, cd, clr, echo, exit,
		lock, logout, ls, pwd, ren, rm, sleep, unlock.
		The above is accomplished at a cost:  the shell is written in
		BDS C and is five times the size of the CP/M CCP; hence it
		takes somewhat longer to load into memory at warm boots.
		Also, since the shell clobbers the CCP, submit does not work
		when the shell is invoked.";
    KEYWORDS:	shell, command, interpreter;
    SYSTEM:	CP/M;
    FILENAME:	SH.C;
    WARNINGS:	"Copyright (c) 1982 Steve Blasingame.
		Requires doglob.h and fcb.h for compile.";
    AUTHORS:	Steve Blasingame;
    COMPILERS:	BDS-C 1.50;
*/

#include <bdscio.h>
#include <hardware.h>
#include "fcb.h"
#include "doglob.h"

#define	EOT	4	/* ascii eot */
#define	PROMPT	"$ "	/* prompt string */
#define TABSIZE	7	/* spaces in a tab */
#define	CNTLH	8	/* backspace */
#define DEL	0x7f	/* ascii del */
#define QUIT	0x1c	/* cntl-backslash */
#define LINEKILL 24	/* cntl-x */
#define	SLEEPNO	16	/* sleep constant for 4 Mz Z80a */
#define COMMENT	'#'	/* comment line */
#define	stdin	0	/* for console io */
#define	stdout	1	/* for console io */
#define	FILE	struct _buf
#define SHBUF	0xf54b	/* Buffer in current bios */
char	combuf[MAXLINE]; /* command line buffer */
char	globs[GLOBMAX*FILENAMESIZE]; /* expansion buffer */
int	argvp[20];	/* array of argument pointers */
int	argcp;		/* argument count for execv */
int	testmode;	/* verbose flag */
int	flag;		/* for lst function */

/* Structure of Shell's Storage Area in the CBIOS */
struct	exbuf	{
	char	_shdsk;		/* current directory */
	int	_shsav;		/* buffer save flag  */
	char	_nocli;		/* no shell option */
	char	_shbuf[BUFSIZ]; /* buffer */
};

struct exbuf *iop;		/* pointer to bios buffer */
int	parse();		/* parse command line */
int	dolocal();		/* do built-in shell commands */
int	lexecv();		/* local execv until C1.45a */
char	*b_gets();		/* local crt driver */
char	pwd();			/* return current logical disk */
char	chdir();		/* select disk */
int	filprt();		/* list selected file on screen */
int	lst();			/* format directory listing */
int	dofile();		/* process shell script */
int	flock();		/* lock or unlock file */
int	doshell();		/* exec the shell */
int	expand();		/* expand global filenames */
int	dozap();		/* interactively delete files */
int	doglob();		/* parse global file expressions */
int	more();			/* list multiple files on screen */
int	crfil();		/* fill memory */
int	strcmp();

main(argc,argv)
int argc;
char *argv[];
{
char *cp;

_allocp = 0;	/* initializer */
iop = SHBUF;	/* initializer */
testmode = 0;	/* initializer */
iop->_nocli = 0;	/* default shell on reboot */
if (argc == 2 && (strcmp(argv[1],"-T") == 0)) testmode =1;
if (argc == 3 && (strcmp(argv[1],"-F") == 0)) parse(strlower(argv[2]));


	bdos(0x0e, iop->_shdsk);	/* restore cwd */

	while (1) {
		crfil(combuf,MAXLINE,0);
		if (!iop->_shsav) {
			puts(PROMPT);
			if (!b_gets(combuf))
				break;
			else {
				cp = combuf;
				if (parse(cp) == -1)
					puts("what?\n");
			}
		}
		else {
			strcpy(combuf,&iop->_shbuf);
			iop->_shsav = 0;
			cp = combuf;
			if (parse(cp) == -1)
				puts("syntax error\n");
		}
	}
chdir('a');			/* home directory */
if (execl("a:login",0) == -1)	/* admittedly a kluge */
	;
doshell();
}
parse(cp)
char *cp;
{
int i;
char	*pointer;
i=0;
argvp[i]=cp;

	if (testmode) puts(cp);
	if (*cp == '\n')
		return 0;
	else {
		while(*cp == ' ')	/* skip blanks */
			cp++;
		argvp[i]=cp;
		while(*++cp != NULL) {
			if (*cp == ' ') {
				i++;
				*cp = NULL;
				argvp[i]=cp+1;
			}
			else if (*cp == ';') {
				if (cp == combuf || *(cp+1) != ' ')
					return -1;
				else {
					i++;
					*cp = NULL;
					argvp[i]=NULL;
					cp += 2;
					strcpy(iop->_shbuf,cp);
					iop->_shsav = 1;
					break;
				}
			}
			else if (*cp == '\n') {
				i++;
				*cp = NULL;
				argvp[i]=NULL;
				break;
			}
		}
	argcp = 0;
	while(1) {
		if (!argvp[argcp]) break;
		argcp++;
	}
	if (dolocal(&argvp)) {
		return 0;
	}
	else {
		pointer = &argvp + 1;
		if (lexecv(argvp[0], pointer) == -1) {
			if (dofile(argcp,&argvp) == -1) {
				puts(argvp[0]);
				puts(": not found\n");
			}
		return 0;
		}
	}
	}
return(-1);
}
dolocal(ptr)
char *ptr[];
{
int value;
char *tpoint;
	if (strcmp(*ptr,"clr") == 0) {
		puts(CLEARS);
		return 1;
	}
	else if (strcmp(*ptr,"ccp") == 0) {
		iop->_nocli = 1;	/* set no shell mode */
		bios(1,0);		/* warm boot cpm */
	}
	else if (strcmp(*ptr,"exit") == 0) {
		iop->_shsav = 0;	/* flush shell buffer */
		bios(1,0);		/* warm boot cpm */
	}
	else if (**ptr == COMMENT) return 1;
	else if (strcmp(*ptr,"logout") == 0) {
		chdir('a');	/* set default directory */
		execl("a:login",0);
		;
		return 1;
	}
	else if (strcmp(*ptr,"echo") == 0) {
		++ptr;
		while (*ptr != 0) {
			puts(*ptr);
			putchar(' ');
			ptr++;
		}
		putchar('\n');
		return 1;
	}
	else if (strcmp(*ptr,"pwd") == 0) {
			putchar(pwd());	/* print current disk */
			puts(":\n");
			return 1;
	}
	else if (strcmp(*ptr,"cd") == 0) {
			chdir(**++ptr);	/* set default disk */
		return 1;
	}
	else if (strcmp(*ptr,"sleep") == 0) {
		value = (SLEEPNO * atoi(*++ptr));
		sleep(value);
		return 1;
	}
	else if (strcmp(*ptr,"rm") == 0) {
		++ptr;	/* remove files */
		if (*ptr == 0 || **ptr == '\n' || **ptr == 0) {
			puts("usage: rm [file] [file...]\n");
			return 1;
		}
		while (*ptr != NULL && **ptr != NULL) {
			expand(dozap,*ptr,0);
			ptr++;
		}
	return 1;
	}
	else if (strcmp(*ptr,"lock") == 0) {
		++ptr;
		if (*ptr == 0 || **ptr == '\n' || **ptr == 0) {
			puts("usage: lock [file] [file...]\n");
			return 1;
		}
		while (*ptr != NULL && **ptr != NULL) {
			expand(flock,*ptr,1);
			ptr++;
		}
	return 1;
	}
	else if (strcmp(*ptr,"unlock") == 0) {
		++ptr;
		if (*ptr == 0 || **ptr == '\n' || **ptr == 0) {
			puts("usage: unlock [file] [file...]\n");
			return 1;
		}
		while (*ptr != NULL && **ptr != NULL) {
			expand(flock,*ptr,0);
			ptr++;
		}
	return 1;
	}
	else if (strcmp(*ptr,"mv") == 0) {
		tpoint = *++ptr;
		++ptr;
		value = *ptr;
		if (tpoint == 0 || *tpoint == '\n' || **ptr == '\n' || *ptr == 0) {
			puts("usage: mv [source] [dest]\n");
			return 1;
		}
			if (index(tpoint,"*") != -1 || index(tpoint,"?") != -1) {
				puts(tpoint);
				puts(": ambiguous\n");
				return 1;
			}
			else if (index(value,"*") != -1 || index(value,"?") != -1) {
				puts(value);
				puts(": ambiguous\n");
				return 1;
			}
			else if (fstat(*ptr) == -1) {
				if ((value=fstat(tpoint)) == 1) {
					puts(tpoint);
					puts(": readonly\n");
					return 1;
				}
				else if (value == 0) {
					if (rename(tpoint,*ptr) == -1) {
						puts(*ptr);
						puts(": file not found\n");
						return 1;
					}
				}
				else if (value == -1) {
					puts(tpoint);
					puts(": file not found\n");
					return 1;
				}
			}
			else {
				puts(*ptr);
				puts(": file already exists\n");
				return 1;
			}
		return 1;
	}
	else if (strcmp(*ptr,"ls") == 0) {
		++ptr;
		flag = 0; /* a kluge */
		if (!(*ptr == 0 || **ptr == '\n' || **ptr == 0)) {
			while (*ptr != NULL && **ptr != NULL) {
				expand(lst,*ptr,0);
				ptr++;
			}
			putchar('\n');
		}
		else {
			expand(lst,"*.*",0);
			putchar('\n');
		}
		return 1;
	}
	else if (strcmp(*ptr,"cat") == 0) {
		++ptr;
		if (*ptr == 0 || **ptr == '\n' || **ptr == 0) {
			puts("usage: cat [file] [file...]\n");
			return 1;
		}
		while (*ptr != 0) {
			expand(filprt,*ptr,0);
			ptr++;
		}
		return 1;
	}
	else return 0;
}
lexecv(pgm,argv)
char *pgm;
char **argv;
{
int p[10];	/* only 10 for now */
int argcp, val;
	argcp=10;
	val=0;

	while (*argv != NULL) {
		p[11-argcp] = *argv++;
		if (--argcp < 0) {
			argcp = 0;
			break;
		}
	}
	p[11-argcp] = NULL;

	/* replace this grossness with something reasonable */
	val=execl(pgm,p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10]);
	return val;
}
dofile(largc,largv)
int largc;
char *largv[];
{
char local[BUFSIZ], tmpbuf[BUFSIZ], *bp, scratch[BUFSIZ];
int x;
FILE file;
scratch[0]=0;	/* initializer */

	strcpy(local,largv[0]);
	strcat(local,".sh");
	if (fopen(local,file) == -1) return -1;

	while (fgets(local,file)) {
		bp = local;
		while (1) {
			if (*bp) {
				if (*bp == '$') {
					*bp = NULL;
					strcpy(tmpbuf,local);
					if ((x=atoi(++bp)) <= largc) {
						strcat(tmpbuf,largv[x]);
					}
					else {
						puts("invalid argument\n");
						return 0;
					}
					strcat(tmpbuf,++bp);
					strcpy(local,tmpbuf);
					bp -= 2; /* klugy */
				}
				else if (*bp == '\n') {
					*bp = NULL;
				}
				bp++;
			}
			else break;
		}
		strcat(scratch,local);
		strcat(scratch," ; ");
		if (iop->_shsav) {
			if ((strlen(iop->_shbuf)+strlen(scratch)) >= BUFSIZ) {
				puts("sh: buffer overflow\n");
				return 0;
			}
		}
	}
	if (iop->_shsav) {
		strcat(scratch,iop->_shbuf);
		strcpy(iop->_shbuf,scratch);
	}
	else {
		*(scratch+strlen(scratch)-1) = NULL; /* kluge */
		*(scratch+strlen(scratch)-2) = '\n'; /* more */
		strcpy(iop->_shbuf,scratch);
		iop->_shsav=1;
	}
return 0;
}
char *
b_gets(buf)
char *buf;
{
char c;
int tab;
tab = TABSIZE;

	while((c=bios(3,0)) != '\r') {
		if (c == QUIT) {
			puts("\nQuit\n");
			bios(1,0);	/* warm boot cpm */
		}
		if ((c == CNTLH || c == DEL) && buf !=combuf) {
			putchar(CNTLH);
			putchar(' ');
			putchar(CNTLH);
			buf -= 2;
		}
		else if (c == '\t') {
			putchar(' ');
			while (--tab) {
				*buf = ' ';
				buf++;
				putchar(' ');
			}
		tab = TABSIZE;
		}
		else if (c == LINEKILL) {
			while (1) {
				putchar(CNTLH);
				putchar(' ');
				putchar(CNTLH);
				if (buf == (combuf-1))
					break;
				else
					--buf;
			}
			puts(PROMPT);
		}
		else if (c == EOT) {
			puts("logout\n");
			return 0;
		}
		else {
			*buf = c;
			putchar(c);
		}
		if (buf != (combuf+MAXLINE))
			buf++;
	}
putchar('\n');
*buf = '\n';
*++buf = '\0';
return combuf;
}
pwd()		/* return current directory */
{
	return(iop->_shdsk+97);
}
chdir(c)	/* cp/m function 14 */
char c;
{
	if (c < 65 || c > 122)
		return 0;
	c -= 97;
	bdos(0x0e,c);
	iop->_shdsk = c;
	return c;
}
flock(filnam,mode) char *filnam; int mode; {
fcb address;
char *offset;
offset = &address.f_name[8];

	setfcb(address,filnam);
	if (mode == TRUE) *offset |= '\200';
	else *offset &= 0x7f;
	if (bdos(30,&address) == 0xff) {
		puts(filnam);
		puts(": not found.\n");
	}
return(0);
}
filprt(filnam)
char *filnam;
{
FILE fbuf;
char c, str[BUFSIZ];
int count;

	count = 0;
	if (fopen(filnam,fbuf) == -1)
		return -1;
	else {
		puts(CLEARS);
		putchar('-'); puts(filnam); puts("-\n");
		for (;;) {
			if (!fgets(str,fbuf)) {
				fclose(fbuf);
				fflush(fbuf);
				puts("-eof-");
				if (bios(3,0) == 'q') {
					puts("\010 \010\010 \010\010 \010\010 \010\010 \010\010 ");
					return -1;
				}
				putchar('\n');
				return 0;
			}
			if ((count += 1) == 22) {
				count = 0;
				puts("-more-");
				if ((c=bios(3,0)) == 'n') {
				puts("\010 \010\010 \010\010 \010\010 \010\010 \010\010 \010\010 ");
					fclose(fbuf);
					return 1;
				}
				else if (c == 'q') {
					fclose(fbuf);
					puts("\010 \010\010 \010\010 \010\010 \010\010 \010\010 \010\010 ");
					return -1;
				}
				puts(CLEARS);
				putchar('-'); puts(filnam); puts("-\n");
			}
			fputs(str,stdout);
		}
	}
puts("-eof-");
if (bios(3,0) == 'q') {
	puts("\010 \010\010 \010\010 \010\010 \010\010 \010\010 ");
	return -1;
}
else putchar('\n');
return 0;
}

lst(vector)
char *vector;
{
int q;	/* flag must be an external */

		if (flag == 3) {
			puts(vector);
			putchar('\n');
			flag = 0;
		}
		else {
			puts(vector);
			for (q = 0; q != (20-strlen(vector)); q++)
				putchar(' ');
			flag++;
		}
return 0;
}
pickout(fcon,name) fcb *fcon; char *name; {
	char i;

	if (fcon->f_entry) {
		*name++ = " abcd"[fcon->f_entry & 0x7f] ;
		*name++ = ':' ;
	}

	for (i=0; fcon->f_name[i] != ' ' && i < 8; i++)
		*name++ = tolower(fcon->f_name[i]) ;
	*name++ = '.';
	for (i=8; fcon->f_name[i] != ' ' && i < 11; i++)
		*name++ = tolower(fcon->f_name[i]) ;
	*name = 0;
}
remove(filnam) char *filnam; {
fcb address;

	if (fstat(filnam) == 1) return 1; /* readonly file */
	setfcb(address,filnam);
	if (bdos(19,&address) == 0xff)	/* cpm delete call */
		return -1;	/* file not found */
	else return 0;
}
doshell()
{
execl("a:sh",0);
}
fstat(filnam) char *filnam; {
char *byte, c;
fcb address;

	setfcb(address,filnam);
	if ((c=bdos(17,&address)) == 255)
		return -1;	/* file not found */
	byte =(0x80 + (c * 32) + _MBYTE); /* permission byte */

	if ((*byte & '\200') == 0) return 0; /* readwrite */
	else return 1;			    /* readonly */
}
expand(func,parm1,parm2) /* perform global file expansions */
int (*func)();
char *parm1;
int parm2;
{
	int count, ret;
	count = GLOBMAX;

		crfil(globs,count*FILENAMESIZE,0xff);
		while (count--)
			globv[count] = &globs[count*FILENAMESIZE];
		globv[GLOBMAX]=0;
		if ((ret = doglob(parm1)) >= 1) {
			qsort(globs,ret,FILENAMESIZE,strcmp);
			count = 0;
			while (count < ret) {
				if ((*func)(globv[count],parm2) == -1)
					break;
				count++;
			}
		}
		else {
			puts(parm1);
			puts(": no match\n");
		}
return 0;
}
dozap(file) char *file; {
int ret;

	if ((ret = remove(file)) == -1) {
		puts(file);
		puts(": not found\n");
	}
	else if (ret == 1) {
		puts(file);
		puts(" readonly -- zap?");
		if (bios(3,0) == 'y') { 
			flock(file,0);
			remove(file);
		}
		putchar('\n');
	}
return 0;
}
doglob(string) /* glob - expand filename expressions */
char *string;
{
char f, c, buf[20];
fcb address;
fcb *dirbuf;
int flag, q, i;
flag=0;
i=0;

	if (strlen(string) == 2 && *(string+1) == ':') {
		setfcb(address,"????????.???");
		address.f_entry = tolower(*string)-96;
	}
	else
		setfcb(address,string);

	for (f=17; (c=bdos(f,&address)) != 255; f=18) {
		dirbuf = (0x80 + (c * 32));
		dirbuf->f_entry = address.f_entry; 
		pickout(dirbuf, buf);
			strcpy(globv[i],buf);
			if (++i == GLOBMAX) break;
	}
globv[i] = 0;		/* null terminate vector */
return i;
}
index(string,ch) /* return pointer to ch in string */
char *string, *ch;
{

	while (*string) {
		if (*string==*ch)
			return string;
		string++;
	}
return -1;
}
crfil(address,length,byte) /* zero fill at address for length bytes */
char *address, byte;
unsigned length;
{

	while (--length) {
		*++address = byte;
	}
return 0;
}
                