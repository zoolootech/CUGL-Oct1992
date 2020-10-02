/*	dctcvt - convert spelling dictionary and build index	*/

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define GRANULE	16	/* index granularity in words	*/

struct idx
	{
	char	*key;		/* address of key in main mem */
	long	addr;		/* file pointer where a word is stored */
	};

struct suffix
	{
	char	*value;		/* suffix value			*/
	int	length;		/* length of suffix value	*/
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

char	*malloc();

main(argc,argv)
	int argc;
	char **argv;
	{
	struct	stat	stats;
	long	addr;
	int	first=1;
	int	size, idxsize, shortsize, dsize, dsize2, fileno, pos, words;
	int	i, suflen;
	char	*dict, *dict2, *dictend, *cp1, *cp2, *idx, *shortaddr;
	char	*root[30], *flag[30];
	char	c;

	printf("reading source dictionary\n");
	fileno=open("wpdict.nl", O_RDONLY);
	if (fileno<0)
		{
		printf("cant open old dictionary\n");
		exit();
		}

	fstat(fileno,&stats);
	dsize=stats.st_size;
	dict=malloc(dsize);
	dictend=dict+dsize;
	read(fileno,dict,dsize);
	close(fileno);
	printf("read %d\n",dsize);
	printf("converting dictionary\n");
	words=0;
	for(cp1=dict; cp1<dictend; cp1++)
		if (*cp1==10)
			{
			*cp1=0;
			words++;
			}

	printf("%d words processed\n",words);
	for(i=0; i<sizeof(root)/sizeof(root[0]); root[i++]="");
	dict2=malloc(dsize+words);
	words=0;
	for(cp1=dict, cp2=dict2; cp1<dictend; cp1++)
		{
		size=strlen(cp1);
		for (i=0, suflen=0; suffix[i].length && !suflen; i++)
			{
			if (size>suffix[i].length
				&& strcmp(cp1+size-suffix[i].length,
					suffix[i].value)==0
				&& strcmp(cp1+size-suffix[i].length,
					suffix[i].value)==0
				&& strncmp(cp1,
					root[size-suffix[i].length],
					size-suffix[i].length)==0)
				{
				suflen=suffix[i].length;
				*flag[size-suflen]=(*flag[size-suflen])|(1<<i);
				cp1+=size;
				}
			}

		if (!suflen)
			{
			root[size]=cp1;
			for (pos=1; *cp1; cp1++, pos++)
				{
				c=*cp1;
				if (c=='\'')
					c=1;
				else
					c=(c&31)+1;

				if (pos==1)
					{
					*cp2=c<<3;
					*(cp2+1)=0;
					}
				else if (pos==2)
					{
					*cp2=*cp2|(c>>2);
					cp2++;
					*cp2=c<<6;
					*(cp2+1)=0;
					}
				else
					{
					*cp2=*cp2|c;
					cp2++;
					*cp2=0;
					pos=0;
					}
				}

			if (*cp2) *++cp2=0;
			cp2++;
			flag[size]=cp2;
			*cp2++=0;
			words++;
			}
		}

	printf("retained %d words\n",words);
	dsize2=cp2-dict2;
	printf("writing target dictionary\n");
	fileno=creat("wpdict.dat",0666);
	if (fileno<0)
		{
		printf("cant open new dictionary\n");
		exit();
		}

	write(fileno,dict2,dsize2);
	close(fileno);
	printf("wrote %d\n",dsize2);
	printf("creating index\n");
	dictend=dict2+dsize2;
	cp2=idx=malloc(dsize);
	for(cp1=dict2; cp1<dictend;)
		{
		shortsize=99;
		for (i=0; i<GRANULE && cp1<dictend; i++)
			{
			size=strlen(cp1);
			if (size<shortsize)
				{
				shortsize=size;
				shortaddr=cp1;
				}

			cp1+=size;
			cp1++;
			cp1++;
			}

		if (first)
			{
			first=0;
			shortaddr=dict2;
			}

		strcpy(cp2,shortaddr);
		cp2+=strlen(cp2);
		cp2++;
		addr=shortaddr-dict2;
		*cp2++=addr%256;
		addr/=256;
		*cp2++=addr%256;
		addr/=256;
		*cp2++=addr;
		}

	idxsize=cp2-idx;
	printf("writing index\n");
	fileno=creat("wpdict.idx",0666);
	if (fileno<0)
		{
		printf("cant open new index file\n");
		exit();
		}

	write(fileno,idx,idxsize);
	printf("wrote %d\n",idxsize);
	}

