/* CCRL - BDS-C CRL file generator
*/

#include <bdscio.h>

#define DIRSTART        0x0     /*CRL directory*/
#define DIREND          0x1ff   /*dir. stops here*/
#define FNCSTART        0x205   /*fnc.'s can start*/
#define TPAHBRLC        0x1     /*hi byte of tpa add*/
#define TPA		0x100	/* tpa itself */

#define JUMPINSTR       0xc3    /*jump instr. op code*/
#define MASK7           0x7f    /*seven bit mask*/
#define NULENTRY        0x80    /*null function*/

#define CR              0xd     /*carriage return*/
#define LF              0xa     /*line feed*/

#define EXTERN		'e'	/* rlc.type for j.t. */
#define INTOFF		'i'	/* rlc.type for internal offset */
/*
**	The following defines may require changing for other
**	assemblers, or for very large functions
**	Specifically, for CP/M's ASM, the first 2 defines should be:
**	#define LIST_TYPE "PRN"		and
**	#define LIST_COL 1
*/
#define LIST_TYPE	"LST"	/*file type of listing*/
#define LIST_COL	5	/*1st column of address field in listing*/
#define LIST_FIRST	14	/*1st col of source code*/
#define MAXLINE		135	/*max input line from LST file*/
#define MXFNSIZE        4096    /*max function size*/
#define MXRELOCS        640     /*max no. reloc. symbls*/
#define MXSYMSIZE	4096	/*max size of all external symbols*/
#define FILESTRLEN	20	/*max length of one external symbol*/

char  filestr[FILESTRLEN+1];	/*where file/external names are kept*/
char  function[MXFNSIZE];	/*object code*/
char  symbols[MXSYMSIZE];	/*external symbol strings*/
char  *s, *fn, *symp;
char  fncbuf[BUFSIZ], extbuf[BUFSIZ], outbuf[BUFSIZ];
char  rlctype;
int   fncfd, extfd, outfd;
int   addr, fsize, size, fend, outbytes;
int   fip, rip, ripjt, sip, rlcoffset;
int   i, j, k, ip, c, cp, pflag;
struct {
	int val;
	char type;
	} rlc[MXRELOCS];

main(argc, argv)
int argc;
char **argv;
{
	printf("CCRL Ver. 2.0\n");
	pflag = FALSE;
	if (argc < 2)
		{
		printf("usage: CCRL filename [-p]\n");
		exit();
		}
	s = *++argv; argc -= 2;
	if ((fncfd = fopen((fn = filename(s, "COM")), fncbuf)) <= 0)
		{
		printf("Cannot open %s\n",fn);
		exit();
		}
	if ((extfd = fopen((fn = filename(s, LIST_TYPE)), extbuf)) <= 0)
		{
		printf("Cannot open %s\n",fn);
		exit();
		}
	if ((outfd = fcreat((fn = filename(s, "CRL")), outbuf)) <= 0)
		{
		printf("Cannot create %s\n",fn);
		exit();
		}
	else
		printf("Creating %s\n",fn);
	while (argc-- > 0)
		{
		s = *++argv;
		if (strcmp(s, "-P") == EQUAL) pflag = TRUE;
		}
	if (pflag) printf("Listing will be shown\n");
/*
**	Initialize and read first function name in .LST file
*/
	outbytes = -1;
	addr = FNCSTART;
	fn = funcname(extbuf);
	if (fn == NULL)
	    {
	     printf("Error: no function name in .%s file\n", LIST_TYPE);
	     exit();
	    }
	fputs(fn, outbuf);	/* send fn name to output buffer */
	outbytes += strlen(fn);	/* update count of output buffer */
	putw(addr, outbuf);	/* send its start address */
	outbytes += 2;
	putc(NULENTRY, outbuf);	/* send end of directory */
	++outbytes;
/*
**	Read function length & total length
*/
	if (outbytes+2 > DIREND)
		/* This is only possible when more than 1
		   function can be created per CRL file! */
		{ printf("Error: directory overflow\n");
		  exit();
		}
/*
**	Now read external function names
*/
	ip = 0; 		/* count of external functions */
	symp = symbols;
	while ( *(fn = extname(extbuf)) != NUL)
		{
		strcpy(symp, fn);	/* store for later output */
		symp += strlen(fn) + 1;
		ip++;
		}
	*symp++ == NUL;		/* null byte ends it */
	sip = (symp - symbols) - ip;	/* length for output format */
	if(ip > 0)
		{
		if(3*ip >= 0xff) /* where did this number come from? */
			{
			printf("Sorry, to many external functions\n");
			exit();
			}
/*
**	set up JMP XXXX to hop over external jumps
**	followed by the external JMP's themselves
*/
		printf("Number of external jumps is %d\n", ip + 1);

		function[0] = JUMPINSTR;
		function[1] = 3*(ip + 1);
		function[2] = TPAHBRLC;
		fip = 3;
		for (j = 1; j <= ip; j++)
			{
			function[fip++] = JUMPINSTR;
			function[fip++] = 0;	/* these get filled in by */
			function[fip++] = 0;	/* the linker */
			}
		rlcoffset = 3*(ip + 1);
		rlc[0].val = 1;	/* re-locate jump over jump table */
		rlc[0].type = EXTERN;
		rip = 1;	/* this is first relocation parameter */
		}
	else			/* no external calls */
		{
		fip = 0;
		rlcoffset = 0;
		rip = 0;
		}
	ripjt = fip;	/* part of final count */
/*
**	Read the relocation addresses from the .LST(.PRN) file
*/
	do
		{
		if ((c = readhex(extbuf, &rlctype)) >= 0)
			{
			rlc[rip].type = rlctype;
			rlc[rip++].val = rlcoffset + c - TPA;
			}
		if(rip > MXRELOCS)
			{
			printf("Sorry, too many reloc. parameters\n");
			exit();
			}
		}  while (c >= 0);
	printf("Number of relocations is %d\n",rip);
/*
**	Now read fsize bytes of object code from the .COM file
*/
	fsize = fend - TPA;	/* this should be end address */
	if (fsize + rlcoffset > MXFNSIZE)
		{
		printf("Sorry, function buffer size exceeded\n");
		exit();
		}
	printf("Object code length expected is %d (%xH)\n",
						fsize, fsize);
	for (j = 0; j < fsize; j++)
		{
		if ((c = getc(fncbuf)) == ERROR)
			{
			printf("Error: EOF encountered before \
fsize bytes read\n");
			exit();
			}
		function[fip++] = c;
		}

/*
**	Now reloc to 0 from beginning of TPA (0 or 4300)
*/
	if (rip > 0)
		{
/*
**		printf("Relocating jump table by %04xH\n", TPAHBRLC << 8);
**		printf("Relocating local references by %04xH\n",
**				(TPAHBRLC << 8) - rlcoffset);
*/
		for (j=0; j < rip; j++)
			{
			if (pflag)
				printf("rlc: %4x %02x %02x %02x",
					rlc[j].val,
					function[rlc[j].val - 1],
					function[rlc[j].val],
					function[rlc[j].val + 1]);

			if (rlc[j].type == EXTERN)
				function[rlc[j].val + 1] -= TPAHBRLC;
			else	{
				function[rlc[j].val] += rlcoffset;
				function[rlc[j].val + 1] -= TPAHBRLC;
				if (function[rlc[j].val] < rlcoffset)
					++function[rlc[j].val + 1];
				}
			if (pflag)
				printf(" --> %02x %02x %02x %c\n",
					function[rlc[j].val - 1],
					function[rlc[j].val],
					function[rlc[j].val + 1],
					rlc[j].type);
			}
		}
/*
**	Send pointer to next free space to output
**	Finish up directory portion
*/
	if (pflag) printf("rip: %d, ripjt: %d, sip: %d, fsize: %d\n",
			   rip * 2, ripjt,	sip,	fsize);
	fend = 2 * rip + 2 + ripjt + sip + fsize + 2 + addr;
	putw(fend, outbuf);	/* send next free space to dir. */
	outbytes += 2;
	if (pflag) printf("Directory size: %d\n", outbytes);

	while (outbytes < (addr - 1))	/* null fill directory */
		{ putc(0,outbuf);
		  ++outbytes;
		}
/*
**	Send symbols to output file
*/
	symp = symbols;
	for (i=0; i < ip; i++)
		while (*symp)
			{
			putc(*symp++, outbuf);
			outbytes++;
			}
	putc(NUL, outbuf);	/* null byte ends it */
	outbytes++;
/*
**	Send fixed up function code to output file
**	First comes the length of the code to follow
*/
	putw(fsize + ripjt * 3, outbuf);
	outbytes += 2;
	for (j=0; j < fip; j++)
		{
		putc(function[j], outbuf);
		++outbytes;
		}
/*
**	This outputs the relocation parameters
**	First comes the amount of them
*/
	putw(rip, outbuf);
	outbytes += 2;
	for (i=0; i < rip; i++)
		{
		putw(rlc[i].val, outbuf);
		outbytes += 2;
		}

	printf("outbytes = 0x%04x, fend = 0x%04x\n", outbytes, fend);
	if(outbytes >= fend)	/* somebody goofed */
		{
		printf("Error: outbytes >= fend\n");
		exit();
		}
	else if ((outbytes + 1) == fend)
		printf("Everything seems hunky-dory.\n");
	else	printf("Something's wrong here...\
actual end differs from expected.\n");
/*
**	Done, close up and leave
*/
	fabort(fncfd);
	fabort(extfd);
	fflush(outbuf);
	fclose(outbuf);
	printf("\nCRL file created.\n");
}

/*
**	Stick extension (type) to end of name
**	Ignore any existing extension
**	Returns new name in filestr
*/
char *filename(flnmptr, flnmend)
char *flnmptr, *flnmend;
{
	int i, havedot;

	havedot = 0;
	i = -1;
	while ((filestr[++i] = *flnmptr++)  !=  NUL)
		havedot |= (filestr[i] == '.');
	if (!havedot)
		{
		filestr[i] = '.';
		while((filestr[++i] = *flnmend++)  !=  NUL)
			;
		}
	return(filestr);
}

/*
**	Gets a function name from buf file
**	Creates a string in upper case,
**	with last byte's high-order bit set
*/
char *funcname(buf)
int buf[];
{
	char tmpline[MAXLINE];
	int c, i, j;

	for(;;)
		{
		if ((c = fgets(tmpline, buf)) == NULL)
			return (NULL);
		j = 0;
		i = LIST_FIRST;
		while (tmpline[i] != ';' && tmpline[i] != NUL) i++;
		if (tmpline[i] == ';' && tmpline[i+1] == '#') break;
		}
		if (pflag) printf("%s",tmpline);
		i += 2;
		while (isspace(tmpline[i])) i++; /* skip beg. space */
		while ((c = toupper(tmpline[i++]))
			&& !isspace(c)
			&& j < 8 )	/* max of 8 chars in name */
			filestr[j++] = c;
	filestr[j-1] |= NULENTRY;	/*set high bit of last char*/
	filestr[j] = NUL;
	return (filestr);
}
/*
**	Gets a external function name from .LST(.PRN) file
**	Creates a string in upper case,
**	with last byte's high-order bit set
*/
char *extname(buf)
int buf[];
{
	char tmpline[MAXLINE];
	int c, i, j;

	for(;;)
		{
		if ((c = fgets(tmpline, buf)) == NULL)
			{
			printf("Error: No externals ($) found\n");
			exit();
			}
		j = 0;
		i = LIST_FIRST;
		while (tmpline[i] != ';' && tmpline[i] != NUL) i++;
		if (tmpline[i] == ';' && tmpline[i+1] == '$') break;
		}
		if (pflag) printf("%s",tmpline);
		i += 2;
		while (isspace(tmpline[i])) i++; /* skip beg. space */
		while ((c = tmpline[i++]) != NUL
			&& !isspace(c)
			&& j < FILESTRLEN )
			filestr[j++] = toupper(c);
	if (j > 0)
		{
		filestr[j-1] |= NULENTRY; /*set high bit of last char*/
		}
	filestr[j] = NUL;
	return (filestr);
}

/*
**	reads a hex word
**	returns -1 if not hex number
*/
readhex(buf,type)
int buf[];
char *type;
{
	char tmpline[MAXLINE];
	int c, i, j, sum, fudge;

	for(;;)
		{
		if ((c = fgets(tmpline, buf)) == NULL)
			{
			printf("Error: No end function (#) found\n");
			exit();
			}
		i = min(strlen(tmpline), LIST_FIRST);
		while (tmpline[i] != ';' && tmpline[i] != NUL) i++;
		if (tmpline[i] == ';')
			{
			switch (tmpline[i+1])
				{
			case '\'':
				*type = INTOFF;
				break;
			case '"':
				*type = EXTERN;
				break;
			case '#':
				*type = 0;
				break;
			default:
				continue;
				}
			break;
			} /* end if */
		} /* end forever */

		if (tmpline[i+2] == '+')
			fudge = 1;
		else if (tmpline[i+2]  == '-')
			fudge = -1;
		else	fudge = 0;
		if (pflag)
			{
			if (*type) printf("%c%2d  ", *type, fudge);
			printf("%s",tmpline);
			}
	for (j = LIST_COL, sum = 0; j < LIST_COL + 4; j++)
		sum = (sum << 4) + hexdig(tmpline[j]);
	if (*type == 0)
		{
		fend = sum;
		return(ERROR);
		}
	else	return (sum + 1 + fudge);
}

/*
**	returns binary of c or -1 if not hex digit
*/
hexdig(c)
int c;
{ 
	if (isdigit(c)) return (c - '0');
	c = toupper(c);
	if(c >= 'A' || c <= 'F') return (c - 'A' + 0xa);
	return (ERROR);
}

/* that's all, ffolkes */


