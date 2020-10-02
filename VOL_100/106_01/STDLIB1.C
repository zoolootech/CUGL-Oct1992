/*	------------------------------------------------------------
	Mods by H Moran:

	In case you wish to use some but not all of the mods,
	lines modified are marked with HRM mod and
	lines added are marked with HRM
	This is not a plea for recognition, simply a convient marker.

		1) add: recogintion of list reader and punch devices
		   to getc(), putc() and fflush()
		2) add: fill remaining buffer with 0x1a's to fflush()
		3) add new function ungetc() to back up the pointer
		   of a buffered file. This will not back up
		   past the 1'st char in the buffer.

	-------------------------------------------------------------
*/


/*

	The files STDLIB*.C contain source for all DEFF.CRL
	functions which are written in C; Any functions which
	appear in DEFF.CRL but have no corresponding source
	were written in machine code and hand-converted to
	.CRL format (as described in the User's Guide.)
	Conversely, any functions whose source appears
	in one of these files but for which there are no
	entries in DEFF.CRL must be compiled before using.
	There wasn't enough space in the DEFF.CRL directory
	for all of them, and this package has got enough
	separate files as it is.
	All functions written by Leor Zolman....who is
	soley responsible for their kludginess.

	Functions appearing in this file:

	fopen		getc		getw
	fcreat		putc		putw
	fflush
	atoi
	strcat		strcmp		strcpy		strlen
	isalpha		isupper		islower		isdigit
	isspace		toupper		tolower
	qsort *
	initw		initb		getval
	abs

	*) re-written for version 1.31. Now sorts in ASCENDING order
	   instead of the previous DESCENDINGG; also, a shell sort
	   algorithm is used instead of the incredibly inefficient
	   previous bubble-sort technique.

*/

/* Buffered I/O for C  */

struct buf {
	int fd;
	int nleft;
	char *nextp;
	char buff[128];
 };


fopen(filename,iobuf)
struct buf *iobuf;
char *filename;
{
	int fd2;
	if ((fd2= open(filename,0))<0) return -1;
	iobuf -> fd = fd2;
	iobuf -> nleft = 0;
	return fd2;
}


getc(iobuf)
struct buf *iobuf;
{
	if (iobuf==0) return getchar();	/* HRM console */
	if (iobuf==3) return bdos(3);	/* HRM reader device */
	if (iobuf -> nleft--) return *iobuf -> nextp++;
	if ((read(iobuf -> fd, iobuf -> buff, 1)) <= 0)
				return -1;
	iobuf -> nleft = 127;
	iobuf -> nextp = iobuf -> buff;
	return *iobuf -> nextp++;
}


/*	function added by HRM
	Back up nextp and nleft, return nleft
	if we are already at the 1'st char (nleft==128)  
	then return 0 as an error indicator.
	This error indicator was picked to allow calls
	of the form:
			if ( ungetc(iobuf) ) <handle the error>

	The error can never happen if only 1 invocation
	of ungetc() is made between invocations of getc()
*/

ungetc(iobuf)
struct buf *iobuf;
{
	if (iobuf < 4) return 0;
	if (iobuf -> nleft == 128) return 0;
	iobuf -> nextp--;
	return ++(iobuf -> nleft);
}


getw(iobuf)
struct buf *iobuf;
{
	int a,b;	
	if (((a=getc(iobuf)) >= 0) && ((b= getc(iobuf)) >=0))
			return 256*b+a;
	return -1;
}


fcreat(name,iobuf)
char *name;
struct buf *iobuf;
{
	int fd2;
	unlink(name);
	if ((fd2 = creat(name))<0 ) return -1;
	iobuf -> fd = fd2;
	iobuf -> nextp = iobuf -> buff;
	iobuf -> nleft = 128;
	return fd2;
}


putc(c,iobuf)
int c;
struct buf *iobuf;
{
	if (iobuf == 1) return putchar(c);	/* HRM console */
	if (iobuf == 2) return bdos(5,c);	/* HRM list device */
	if (iobuf == 3) return bdos(4,c);	/* HRM punch device */
	if (iobuf -> nleft--) return *iobuf->nextp++ =c;
	if ((write(iobuf -> fd, iobuf -> buff,1)) != 1)
			return -1;
	iobuf -> nleft = 127;
	iobuf -> nextp = iobuf -> buff;
	return *iobuf -> nextp++ = c;
}


putw(w,iobuf)
unsigned w;
struct buf *iobuf;
{
	if ((putc(w%256,iobuf) >=0)&&(putc(w/256,iobuf)>=0))
				return w;
	return -1;
}


fflush(iobuf)
struct buf *iobuf;
{
int cnt;
char *fill;

	if (iobuf < 4) return 0;	/* HRM mod non-disk device */
	if (iobuf -> nleft == 128) return 0;
	for( fill=iobuf->nextp,cnt=iobuf->nleft; cnt; cnt-- )	/* HRM */
	  *fill++ = 0x1a;					/* HRM */
	if ((write(iobuf -> fd, iobuf -> buff,1)) <=0)
			return -1;
	if (iobuf -> nleft != 0)
		return seek(iobuf->fd, -1, 1);
	iobuf -> nleft = 128;
	iobuf -> nextp = iobuf -> buff;
	return 0;
}

/*
	Some string functions
*/


atoi(n)
char *n;
{
	int val; 
	char c;
	int sign;
	val=0;
	sign=1;
	while ((c = *n) == '\t' || c== ' ') ++n;
	if (c== '-') {sign = -1; n++;}
	while (  isdigit(c = *n++)) val = val * 10 + c - '0';
	return sign*val;
}


strcat(s1,s2)
char *s1, *s2;
{
	char *temp; temp=s1;
	while(*s1) s1++;
	do *s1++ = *s2; while (*s2++);
	return temp;
}


strcmp(s,t)
char s[], t[];
{
	int i;
	i = 0;
	while (s[i] == t[i])
		if (s[i++] == '\0')
			return 0;
	return s[i] - t[i];
}


strcpy(s1,s2)
char *s1, *s2;
{
	char *temp; temp=s1;
	while (*s1++ = *s2++);
	return temp;
}


strlen(s)
char *s;
{
	int len;
	len=0;
	while (*s++) len++;
	return(len);
}


/*
	Some character diddling functions
*/

isalpha(c)
char c;
{
	return isupper(c) || islower(c);
}


isupper(c)
char c;
{
	return c>='A' && c<='Z';
}


islower(c)
char c;
{
	return c>='a' && c<='z';
}


isdigit(c)
char c;
{
	return c>='0' && c<='9';
}


isspace(c)
char c;
{
	return c==' ' || c=='\t' || c=='\n';
}


char toupper(c)
char c;
{
	return islower(c) ? c-32 : c;
}


char tolower(c)
char c;
{
	return isupper(c) ? c+32 : c;
}


/*
	Other stuff...
*/


/*
	This is the new qsort routine, utilizing the shell sort
	technique given in the Software Tools book (by Kernighan 
	& Plauger.)

	NOTE: this "qsort" function is different from the "qsort" given
	in prior releases -- here, the items are sorted in ASCENDING
	order. The old "qsort" sorted stuff in DESCENDING order, and
	was in part responsible for the atrocious play of the "Othello"
	program (it always made the WORST moves it could find...) 
*/

qsort(base, nel, width, compar)
char *base; int (*compar)();
{	int gap,ngap, i, j;
	int jd, t1, t2;
	t1 = nel * width;
	for (ngap = nel / 2; ngap > 0; ngap /= 2) {
	   gap = ngap * width;
	   t2 = gap + width;
	   jd = base + gap;
	   for (i = t2; i <= t1; i += width)
	      for (j =  i - t2; j >= 0; j -= gap) {
		if ((*compar)(base+j, jd+j) <=0) break;
			 _swp(width, base+j, jd+j);
	      }
	}
}

_swp(w,a,b)
char *a,*b;
int w;
{
	char tmp;
	while(w--) {tmp=*a; *a++=*b; *b++=tmp;}
}



/*
 	Initialization functions
*/


initw(var,string)
int *var;
char *string;
{
	int n;
	while ((n = getval(&string)) != -32760) *var++ = n;
}

initb(var,string)
char *var, *string;
{
	int n;
	while ((n = getval(&string)) != -32760) *var++ = n;
}

getval(strptr)
char **strptr;
{
	int n;
	if (!**strptr) return -32760;
	n = atoi(*strptr);
	while (**strptr && *(*strptr)++ != ',');
	return n;
}

/*
	One lone little last function:
*/

abs(n)
{
	return (n<0) ? -n : n;
}
