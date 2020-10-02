/*	uspell - UNIX spell checker

	based on spell.c by Kenji Hino
*/

#include <termio.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h>

#define MAXWORD		30	/* The maximum number of chars per word */
#define MAXLINE		100	/* The maximum number of chars per line */
#define GRANULE	16		/* index granularity			*/

struct	stat	stats;
struct	termio	termio;

struct	wpspcl	*wpspcl;

extern	int	errno;

unsigned	char	flag;	/* suffix flags				*/

int	txtfil,			/* file descriptor for text file	*/
	dctfil,			/* file descriptor for dictionary	*/
	idxfil,			/* file descriptor for index file	*/
	nbad,			/* number of bad word entries allocated	*/
	lastbad,		/* next available bad word entry	*/
	cmpval;			/* result of last word compare		*/
	
struct idx
	{
	unsigned char	*key;
	long	addr;
	};

struct	suffix
	{
	char	*value;
	int	length;
	};

struct	suffix	suffix[]=
	{
	{"ers",3},
	{"ing",3},
	{"ed",2},
	{"er",2},
	{"es",2},
	{"ly",2},
	{"d",1},
	{"s",1},
	0
	};

struct	idx	*idx, *first, *last;
struct	idx	*idxptr;
struct	idx	*idxend;
struct	idx	*lastidx;
struct	idx	*badword;

char *malloc();
char *filename;

main(argc,argv)
	int argc;
	char **argv;
	{
	long	addr;
	int	x, savflg, i, dirsize;
	unsigned	char	*direct, *dirend, *dirptr, *cp1;

	ioctl(0,TCGETA,&termio);
	savflg=termio.c_oflag;
	termio.c_oflag=OPOST+ONLCR+TAB3;
	ioctl(0, TCSETA, &termio);
	idxend=idx=(struct idx *) malloc((sizeof *idx)*(43000/GRANULE));
	if ((idxfil = open("wpdict.idx", O_RDONLY)) == -1)
		cant("wpdict.idx");

	fstat(idxfil,&stats);
	dirsize=stats.st_size;
	direct=(unsigned char *) malloc(dirsize);
	dirend=direct+dirsize;
	read(idxfil,direct,dirsize);
	for (dirptr=direct; dirptr<dirend;)
		{
		for(cp1=dirptr; *cp1; cp1++);
		cp1++;
		addr=*cp1;
		cp1++;
		addr+=*cp1*256;
		cp1++;
		addr+=*cp1*65536;
		cp1++;
		idxend->key=dirptr;
		idxend->addr=addr;
		idxend++;
		dirptr=cp1;
		}

	lastidx=idxend;
	lastidx--;
	dctfil=open("wpdict.dat", O_RDONLY);
	if (dctfil<0) cant("wpdict.dat");
	argv++;
	filename=*argv;
	if ((txtfil = open(filename,O_RDONLY)) == -1)
		cant(*argv);
	else
		{
		dospel();
		close(txtfil);
		}

	termio.c_oflag=savflg;
	ioctl(0,TCSETAW,&termio);	/* restore terminal settings */
	}

dospel() /* do spell checking */
	{
	struct	idx	*idxpt2;
	int	i, i2, j, start, end, size, iferr, spelled, suflen;
	int	txtsize;
	unsigned	char	word8[MAXWORD];
	unsigned	char	word5[MAXWORD];
	unsigned	char	word52[MAXWORD];
	char	*txt, *txtptr, *txtend;
	char	*cp1, *cp2, *cp3;
	char	errbuf[MAXLINE];
	char	c;

	fstat(txtfil,&stats);
	txtsize=stats.st_size;
	txt=malloc(txtsize);
	txtend=txt+txtsize;
	read(txtfil,txt,txtsize);
	for (txtptr=txt; txtptr<txtend;)
		{
		for(cp1=txtptr, cp2=errbuf; *cp1!='\n'; cp1++, cp2++)
			if (*cp1=='\t')
				*cp2='\t';
			else
				*cp2=' ';

		*cp2++=*cp1++;
		display_line(txtptr,cp1-txtptr);
		iferr=-1;
		start=end=0;
		cp2=txtptr;
		size=skipwhite(cp2);
		cp2+=size;
		while (cp2<cp1)
			{
			size=gtword(cp2);
			sstor5(cp2,0,size,word5);
			binsrc(word5);
			if (!cmpval)
				spelled=1;
			else
				spelled=srcfwd(word5);

			idxpt2=idxptr;
			for (i=0, suflen=0; suffix[i].length && !spelled; i++)
				{
				if(size>suffix[i].length
					&& strncmp(cp2+(size-suffix[i].length),
					suffix[i].value,
					suffix[i].length)==0)
					{
					suflen=suffix[i].length;
					sstor5(cp2,0,size-suflen,word52);
					idxptr=idxpt2;
					i2=srcbak(word52,1);
					if (i2 && flag&(1<<i)) spelled=1;
					}
				}

			if (!spelled
				&& size>2
				&& cp2[start+size-2]=='\''
				&& cp2[start+size-1]=='s')
				{
				sstor5(cp2,0,size-2,word52);
				spelled=srcbak(word52,0);
				}

			if (!spelled)
				{
/*				savbad(word5);*/
				iferr=1;
				cp3=errbuf+(cp2-txtptr);
				for(i=0; i<size; i++) *cp3++='*';
				}

			cp2+=size;
			size=skipwhite(cp2);
			cp2+=size;
			}

		if (iferr==1) display_line(errbuf,cp1-txtptr);
		txtptr=cp1;
		}
	}

skipwhite(cp1)
	char	*cp1;
	{
	char	*cp2;

	for (cp2=cp1; !isalpha(*cp2) && *cp2!='\'' && *cp2!='\n'; cp2++);
	if (*cp2=='\n') cp2++;
	return(cp2-cp1);
	}

gtword(cp1)
	char	*cp1;
	{
	char	*cp2;

	for (cp2=cp1; isalpha(*cp2) || *cp2=='\''; cp2++);
	return(cp2-cp1);
	}

srcbak(word)
	char	*word;
	{
	for(; r5cmp(word,idxptr->key)<0 && idxptr > idx; idxptr--);
		if (idxptr == idx) return 0;
	return(srcfwd(word));
	}

int srcfwd(word)
	char	*word;
	{
	struct	statfs	statfs;
	static	int	gransize=MAXWORD*GRANULE;
	static	int	blksiz;
	int	cmp, dctsiz, dctblk, mapsiz, addr, startblk, endblk;
	unsigned	char	buf[MAXWORD][GRANULE];
	static	unsigned	char	*dict, *map;
	unsigned	char	*cp1, *cp2;
	unsigned	char	mask;

	if (!dict)
		{
		fstat(dctfil,&stats);
		dctsiz=stats.st_size;
		dict=(unsigned char *) malloc(dctsiz);
		fstatfs(txtfil,&statfs,sizeof(statfs),0);
		blksiz=statfs.f_bsize;
		dctblk=(dctsiz/blksiz)+1;
		mapsiz=(dctblk/8)+1;
		map=(unsigned char *) calloc(1,mapsiz);
		}

	addr=idxptr->addr;
	startblk=addr/blksiz;
	endblk=(addr+gransize)/blksiz;
	cp1=map+(startblk/8);
	mask=1<<(startblk%8);
	if(!((*cp1)&mask))
		{
		lseek(dctfil,startblk*blksiz,0);
		read(dctfil,dict+(startblk*blksiz),blksiz);
		*cp1=(*cp1)|mask;
		}

	if (startblk!=endblk)
		{
		cp1=map+(endblk/8);
		mask=1<<(endblk%8);
		if(!((*cp1)&mask))
			{
			lseek(dctfil,endblk*blksiz,0);
			read(dctfil,dict+(endblk*blksiz),blksiz);
			*cp1=(*cp1)|mask;
			}
		}

	for (cp1=dict+addr, cp2=dict+addr+gransize; cp1<cp2;)
		{
		if ((cmp=r5cmp(word,cp1))==0)
			{
			for(; *cp1; cp1++);
			cp1++;
			flag=*cp1;
			return(1);
			}
		else if (cmp < 0)
			{
			return(0);
			}

		for (; *cp1; cp1++);
		cp1++;
		cp1++;
		}

	return(0);
	}

int	binsrc(word)       /* do binary search */
	char *word;
	{
	int	i;

	idxptr=idx;
	cmpval=r5cmp(word,idx->key);
	if (cmpval<=0) return;
	idxptr=lastidx;
	cmpval=r5cmp(word,lastidx->key);
	if (cmpval>=0) return;
	first=idx;
	last=idxend;
	while (last-first > 1)
		{
		idxptr=first+((last-first)/2);
/*
		idxptr=&idx[((((int) (first) - (int) (idx)) / sizeof(*idx))
			+ (((int) (last) - (int) (idx)) / sizeof(*idx))) /2];
*/
		if ((cmpval=r5cmp(word,idxptr->key)) < 0)
			last=idxptr;
		else if (cmpval==0)
			last=first=idxptr;
		else if (cmpval>0)
			first=idxptr;
		}

	idxptr=first;
	}

savbad(word)
	char	*word;
	{
	unsigned	char	*str;

	if (!nbad)
		{
		nbad=256;
		badword=(struct idx *) malloc((sizeof *badword)*nbad);
		}
	else if (lastbad==nbad)
		{
		nbad*=2;
		badword=(struct idx *) realloc(badword,sizeof(*badword)*nbad);
		}

	str=(unsigned char *) malloc(strlen(word)+1);
	strcpy(str,word);
	badword[lastbad++].key = str;
	}

cant(name)
	char	*name;
	{

	write(1,"Can't open ",11);
	write(1,name,strlen(name));
	write(1,"\n",1);
	exit(1);
	}

errormes(message)
	char	*message;
	{
	write(1,message,strlen(message));
	write(1,"\n",1);
	}

display_line(line,length)
	char	*line;
	int	length;
	{
	write(1,line,length);
	}

r5cmp(cp1,cp2)
	unsigned	char	*cp1, *cp2;
	{
	for (; *cp1 && *cp2 && *cp1==*cp2; cp1++, cp2++);
	return(*cp1-*cp2);
	}

sstor5(cp1,disp,len1,cp2)
	register	char	*cp1, *cp2;	/* buffer addresses	*/
	int		disp;			/* field displacement	*/
	register	int	len1;		/* field length		*/
	{
	int	pos, len2;
	char	c;

	cp1+=disp;
	for (pos=1, len2=0; *cp1 && len2<len1; cp1++, pos++, len2++)
		{
		c=*cp1;
		if (c=='\'')
			c=1;
		else
			c=(c&31)+1;

		if (pos==1)
			{
			*cp2=c<<3;
			}
		else if (pos==2)
			{
			*cp2=*cp2|(c>>2);
			cp2++;
			*cp2=c<<6;
			}
		else
			{
			*cp2=*cp2|c;
			cp2++;
			pos=0;
			}
		}

	if (pos!=1) cp2++;
	*cp2=0;
	}

